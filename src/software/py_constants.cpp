#include <pybind11/pybind11.h>

#include "shared/constants.h"
#include "software/constants.h"
namespace py = pybind11;

PYBIND11_MODULE(py_constants, m)
{
    m.attr("BALL_MAX_SPEED_METERS_PER_SECOND") = BALL_MAX_SPEED_METERS_PER_SECOND;
    m.attr("ROBOT_MAX_HEIGHT_METERS")          = ROBOT_MAX_HEIGHT_METERS;
    m.attr("ROBOT_MAX_RADIUS_METERS")          = ROBOT_MAX_RADIUS_METERS;
    m.attr("ROBOT_MAX_HEIGHT_MILLIMETERS") =
        ROBOT_MAX_HEIGHT_METERS * MILLIMETERS_PER_METER;
    m.attr("ROBOT_MAX_RADIUS_MILLIMETERS") =
        ROBOT_MAX_RADIUS_METERS * MILLIMETERS_PER_METER;
    m.attr("DIST_TO_FRONT_OF_ROBOT_METERS") = DIST_TO_FRONT_OF_ROBOT_METERS;
    m.attr("BALL_MAX_RADIUS_METERS")        = BALL_MAX_RADIUS_METERS;
    m.attr("BALL_MAX_RADIUS_MILLIMETERS") =
        BALL_MAX_RADIUS_METERS * MILLIMETERS_PER_METER;

    m.attr("MAX_FRACTION_OF_BALL_COVERED_BY_ROBOT") =
        MAX_FRACTION_OF_BALL_COVERED_BY_ROBOT;
    m.attr("BALL_MASS_KG") = BALL_MASS_KG;
    m.attr("STOP_COMMAND_ROBOT_MAX_SPEED_METERS_PER_SECOND") =
        STOP_COMMAND_ROBOT_MAX_SPEED_METERS_PER_SECOND;
    m.attr("COLLISION_ALLOWED_ROBOT_MAX_SPEED_METERS_PER_SECOND") =
        COLLISION_ALLOWED_ROBOT_MAX_SPEED_METERS_PER_SECOND;
    m.attr("ENEMY_ROBOT_MAX_SPEED_METERS_PER_SECOND") =
        ENEMY_ROBOT_MAX_SPEED_METERS_PER_SECOND;

    m.attr("ENEMY_ROBOT_MAX_ACCELERATION_METERS_PER_SECOND_SQUARED") =
        ENEMY_ROBOT_MAX_ACCELERATION_METERS_PER_SECOND_SQUARED;
    m.attr("ACCELERATION_DUE_TO_GRAVITY_METERS_PER_SECOND_SQUARED") =
        ACCELERATION_DUE_TO_GRAVITY_METERS_PER_SECOND_SQUARED;


    m.attr("TACTIC_OVERRIDE_PATH")     = TACTIC_OVERRIDE_PATH;
    m.attr("PLAY_OVERRIDE_PATH")       = PLAY_OVERRIDE_PATH;
    m.attr("WORLD_PATH")               = WORLD_PATH;
    m.attr("PRIMITIVE_PATH")           = PRIMITIVE_PATH;
    m.attr("ROBOT_STATUS_PATH")        = ROBOT_STATUS_PATH;
    m.attr("DEFENDING_SIDE")           = DEFENDING_SIDE;
    m.attr("SSL_WRAPPER_PATH")         = SSL_WRAPPER_PATH;
    m.attr("BLUE_SSL_WRAPPER_PATH")    = BLUE_SSL_WRAPPER_PATH;
    m.attr("YELLOW_SSL_WRAPPER_PATH")  = YELLOW_SSL_WRAPPER_PATH;
    m.attr("SSL_REFEREE_PATH")         = SSL_REFEREE_PATH;
    m.attr("SENSOR_PROTO_PATH")        = SENSOR_PROTO_PATH;
    m.attr("WORLD_STATE_PATH")         = WORLD_STATE_PATH;
    m.attr("BLUE_ROBOT_STATUS_PATH")   = BLUE_ROBOT_STATUS_PATH;
    m.attr("YELLOW_ROBOT_STATUS_PATH") = YELLOW_ROBOT_STATUS_PATH;
    m.attr("SIMULATION_TICK_PATH")     = SIMULATION_TICK_PATH;
    m.attr("YELLOW_WORLD_PATH")        = YELLOW_WORLD_PATH;
    m.attr("BLUE_WORLD_PATH")          = BLUE_WORLD_PATH;
    m.attr("YELLOW_HRVO_PATH")         = YELLOW_HRVO_PATH;
    m.attr("BLUE_HRVO_PATH")           = BLUE_HRVO_PATH;
    m.attr("BLUE_PRIMITIVE_SET")       = BLUE_PRIMITIVE_SET;
    m.attr("YELLOW_PRIMITIVE_SET")     = YELLOW_PRIMITIVE_SET;
    m.attr("SIMULATOR_STATE_PATH")     = SIMULATOR_STATE_PATH;
    m.attr("UNIX_BUFFER_SIZE")         = UNIX_BUFFER_SIZE;
    m.attr("DYNAMIC_PARAMETER_UPDATE_REQUEST_PATH") =
        DYNAMIC_PARAMETER_UPDATE_REQUEST_PATH;
    m.attr("DYNAMIC_PARAMETER_UPDATE_RESPONSE_PATH") =
        DYNAMIC_PARAMETER_UPDATE_RESPONSE_PATH;
    m.attr("WORLD_STATE_RECEIVED_TRIGGER_PATH") = WORLD_STATE_RECEIVED_TRIGGER_PATH;

    // Multicast Channels
    m.def("getRobotMulticastChannel", [](py::args& args) {
        if (args.size() != 1)
        {
            throw std::runtime_error("must provide channel number only");
        }

        return ROBOT_MULTICAST_CHANNELS.at(args[0].cast<int>());
    });

    // Ports
    m.attr("VISION_PORT")       = VISION_PORT;
    m.attr("PRIMITIVE_PORT")    = PRIMITIVE_PORT;
    m.attr("ROBOT_STATUS_PORT") = ROBOT_STATUS_PORT;
    m.attr("ROBOT_LOGS_PORT")   = ROBOT_LOGS_PORT;

    // SSL
    m.attr("SSL_VISION_ADDRESS") = SSL_VISION_ADDRESS;
    m.attr("SSL_VISION_PORT")    = SSL_VISION_PORT;

    m.attr("SSL_REFEREE_ADDRESS") = SSL_REFEREE_ADDRESS;
    m.attr("SSL_REFEREE_PORT")    = SSL_REFEREE_PORT;

    // Units
    m.attr("MILLIMETERS_PER_METER") = MILLIMETERS_PER_METER;
    m.attr("METERS_PER_MILLIMETER") = METERS_PER_MILLIMETER;
    m.attr("CENTIMETERS_PER_METER") = CENTIMETERS_PER_METER;
    m.attr("METERS_PER_CENTIMETER") = METERS_PER_CENTIMETER;

    m.attr("CENTIRADIANS_PER_RADIAN") = CENTIRADIANS_PER_RADIAN;
    m.attr("RADIANS_PER_CENTIRADIAN") = RADIANS_PER_CENTIRADIAN;

    m.attr("NANOSECONDS_PER_MILLISECOND")  = NANOSECONDS_PER_MILLISECOND;
    m.attr("NANOSECONDS_PER_SECOND")       = NANOSECONDS_PER_SECOND;
    m.attr("MICROSECONDS_PER_MILLISECOND") = MICROSECONDS_PER_MILLISECOND;
    m.attr("MICROSECONDS_PER_SECOND")      = MICROSECONDS_PER_SECOND;
    m.attr("MILLISECONDS_PER_SECOND")      = MILLISECONDS_PER_SECOND;
    m.attr("SECONDS_PER_MICROSECOND")      = SECONDS_PER_MICROSECOND;
    m.attr("SECONDS_PER_NANOSECOND")       = SECONDS_PER_NANOSECOND;
    m.attr("SECONDS_PER_MILLISECOND")      = SECONDS_PER_MILLISECOND;
    m.attr("MILLISECONDS_PER_MICROSECOND") = MILLISECONDS_PER_MICROSECOND;
    m.attr("MILLISECONDS_PER_NANOSECOND")  = MILLISECONDS_PER_NANOSECOND;
    m.attr("SECONDS_PER_MINUTE")           = SECONDS_PER_MINUTE;

    m.attr("NUM_GENEVA_ANGLES") = NUM_GENEVA_ANGLES;
    m.attr("CHICKER_TIMEOUT")   = CHICKER_TIMEOUT;
    m.attr("MAX_PASS_RECEIVE_SPEED") = MAX_PASS_RECEIVE_SPEED;

    m.attr("MAX_ROBOT_IDS_PER_SIDE") = MAX_ROBOT_IDS_PER_SIDE;

    // Redis Keys
    m.attr("ROBOT_ID_REDIS_KEY")                = ROBOT_ID_REDIS_KEY;
    m.attr("ROBOT_MULTICAST_CHANNEL_REDIS_KEY") = ROBOT_MULTICAST_CHANNEL_REDIS_KEY;
    m.attr("ROBOT_NETWORK_INTERFACE_REDIS_KEY") = ROBOT_NETWORK_INTERFACE_REDIS_KEY;
    m.attr("ROBOT_BATTERY_VOLTAGE_REDIS_KEY")   = ROBOT_BATTERY_VOLTAGE_REDIS_KEY;
    m.attr("ROBOT_CURRENT_DRAW_REDIS_KEY")      = ROBOT_CURRENT_DRAW_REDIS_KEY;
    m.attr("CAPACITOR_VOLTAGE_REDIS_KEY")       = ROBOT_CAPACITOR_VOLTAGE_REDIS_KEY;

    // Robot power constants
    m.attr("MIN_CAPACITOR_VOLTAGE")   = MIN_CAPACITOR_VOLTAGE;
    m.attr("MAX_CAPACITOR_VOLTAGE")   = MAX_CAPACITOR_VOLTAGE;
    m.attr("MIN_BATTERY_VOLTAGE")     = MIN_BATTERY_VOLTAGE;
    m.attr("MAX_BATTERY_VOLTAGE")     = MAX_BATTERY_VOLTAGE;
    m.attr("BATTERY_WARNING_VOLTAGE") = BATTERY_WARNING_VOLTAGE;

    // Robot Communication
    m.attr("NUM_TIMES_SEND_STOP") = NUM_TIMES_SEND_STOP;
}
