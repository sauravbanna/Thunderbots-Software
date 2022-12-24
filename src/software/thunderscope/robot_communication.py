from software.py_constants import *
from software.thunderscope.thread_safe_buffer import ThreadSafeBuffer
from software.python_bindings import *
from proto.import_all_protos import *
import threading
import time


class RobotCommunication(object):

    """ Communicate with the robots """

    def __init__(
            self,
            full_system_proto_unix_io,
            diagnostics_proto_unix_io,
            multicast_channel,
            interface,
            estop_path="/dev/ttyACM0",
            estop_buadrate=115200,
    ):
        """Initialize the communication with the robots

        :param full_system_proto_unix_io: full_system_proto_unix_io object
        :param diagnostics_proto_unix_io: proto_unix_io object for diagnostics input
        :param multicast_channel: The multicast channel to use
        :param interface: The interface to use
        :param estop_path: The path to the estop
        :param estop_baudrate: The baudrate of the estop

        """
        self.sequence_number = 0
        self.last_time = time.time()
        self.full_system_proto_unix_io = full_system_proto_unix_io
        self.diagnostics_proto_unix_io = diagnostics_proto_unix_io
        self.multicast_channel = str(multicast_channel)
        self.interface = interface
        self.estop_path = estop_path
        self.estop_buadrate = estop_buadrate

        self.robots_connected_to_diagnostics = {1, 2, 3}

        self.world_buffer = ThreadSafeBuffer(1, World)
        self.primitive_buffer = ThreadSafeBuffer(1, PrimitiveSet)

        self.motor_control_diagnostics_buffer = ThreadSafeBuffer(1, MotorControl)
        self.power_control_diagnostics_buffer = ThreadSafeBuffer(1, PowerControl)

        if self.full_system_proto_unix_io:
            self.full_system_proto_unix_io.register_observer(World, self.world_buffer)

            self.full_system_proto_unix_io.register_observer(
                PrimitiveSet, self.primitive_buffer
            )

        if self.diagnostics_proto_unix_io:
            self.diagnostics_proto_unix_io.register_observer(
                MotorControl, self.motor_control_diagnostics_buffer
            )
            self.diagnostics_proto_unix_io.register_observer(
                PowerControl, self.power_control_diagnostics_buffer
            )

        self.send_estop_state_thread = threading.Thread(target=self.__send_estop_state)
        self.run_thread = threading.Thread(target=self.run)

        # try:
        #     self.estop_reader = ThreadedEstopReader(
        #         self.estop_path, self.estop_buadrate
        #     )
        # except Exception:
        #     raise Exception("Could not find estop, make sure its plugged in")

    def __send_estop_state(self):
        print('yea')
        # while True:
        #     self.full_system_proto_unix_io.send_proto(
        #         EstopState, EstopState(is_playing=self.estop_reader.isEstopPlay())
        #     )
        #     time.sleep(0.1)

    def run(self):
        """Forward World and PrimitiveSet protos from fullsystem to the robots.

        If the emergency stop is tripped, the PrimitiveSet will not be sent so
        that the robots timeout and stop.

        NOTE: If disconnect_fullsystem_from_robots is called, then the packets
        will not be forwarded to the robots.

        send_override_primitive_set can be used to send a primitive set, which
        is useful to dip in and out of robot diagnostics.

        """
        while True:
            robot_primitives = dict()

            # Send the world
            world = self.world_buffer.get(block=False)

            try:
                self.send_world.send_proto(world)

                # Send the primitive set
                primitive_set = self.primitive_buffer.get(block=False)

                for robot_id in self.robots_connected_to_diagnostics:
                    del primitive_set[robot_id]

                robot_primitives = primitive_set
            except RuntimeError:
                pass

            diagnostics_primitive = DirectControlPrimitive(
                motor_control=self.motor_control_diagnostics_buffer.get(
                    block=False
                ),
                power_control=self.power_control_diagnostics_buffer.get(
                    block=False
                ),
            )

            for robot_id in self.robots_connected_to_diagnostics:
                robot_primitives[robot_id] = Primitive(direct_control=diagnostics_primitive)

            primitive_set = PrimitiveSet(
                time_sent=Timestamp(epoch_timestamp_seconds=time.time()),
                stay_away_from_ball=False,
                robot_primitives=robot_primitives,
                sequence_number=self.sequence_number,
            )

            # print(primitive_set)

            self.sequence_number += 1

            if True:
                self.last_time = primitive_set.time_sent.epoch_timestamp_seconds
                self.send_primitive_set.send_proto(primitive_set)

            time.sleep(0.001)

    def connect_fullsystem_to_robots(self):
        """ Connect the robots to fullsystem """

        self.fullsystem_connected_to_robots = True
        self.robots_connected_to_diagnostics = {1, 2, 3}

    def disconnect_fullsystem_from_robots(self):
        """ Disconnect the robots from fullsystem """

        self.fullsystem_connected_to_robots = False

    def toggle_robot_connection(self, robot_id):
        """
        Connects a robot to or disconnects a robot from diagnostics
        :param robot_id: the id of the robot to be added or removed from the diagnostics set
        """
        print(robot_id)
        if robot_id in self.robots_connected_to_diagnostics:
            self.robots_connected_to_diagnostics.remove(robot_id)
        else:
            self.robots_connected_to_diagnostics.add(robot_id)

    def __enter__(self):
        """Enter RobotCommunication context manager. Setup multicast listener
        for RobotStatus and multicast senders for World and PrimitiveSet

        """
        # Create the multicast listeners
        self.receive_robot_status = RobotStatusProtoListener(
            self.multicast_channel + "%" + self.interface,
            ROBOT_STATUS_PORT,
            lambda data: self.full_system_proto_unix_io.send_proto(RobotStatus, data),
            True,
            )

        self.receive_robot_log = RobotLogProtoListener(
            self.multicast_channel + "%" + self.interface,
            ROBOT_LOGS_PORT,
            lambda data: self.full_system_proto_unix_io.send_proto(RobotLog, data),
            True,
            )

        self.receive_ssl_wrapper = SSLWrapperPacketProtoListener(
            SSL_VISION_ADDRESS,
            SSL_VISION_PORT,
            lambda data: self.full_system_proto_unix_io.send_proto(
                SSL_WrapperPacket, data
            ),
            True,
        )

        # Create multicast senders
        self.send_primitive_set = PrimitiveSetProtoSender(
            self.multicast_channel + "%" + self.interface, PRIMITIVE_PORT, True
        )

        self.send_world = WorldProtoSender(
            self.multicast_channel + "%" + self.interface, VISION_PORT, True
        )

        # TODO (#2741): we might not want to support robot diagnostics in tscope
        # make a ticket here to create a widget to call these functions to detach
        # from AI and connect to robots/or remove

        print(self.robots_connected_to_diagnostics)

        self.send_estop_state_thread.start()
        self.run_thread.start()

    def __exit__(self):
        """Exit RobotCommunication context manager

        """
        self.run_thread.join()
