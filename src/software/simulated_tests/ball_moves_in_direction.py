import software.python_bindings as tbots
from proto.import_all_protos import *

from software.simulated_tests.validation import (
    Validation,
    create_validation_geometry,
    create_validation_types,
)


class BallMovesForward(Validation):
    """Checks if ball is moving forward, i.e. in the +x direction"""

    def __init__(self, initial_ball_position: tbots.Point, tolerance: float = 0.1):
        """
        Constructs the validation
        :param initial_ball_position: the initial position of the ball
        :param tolerance: the tolerance for determining the ball's direction of movement
                          to account for noisy world data
        """
        self.last_ball_position = initial_ball_position
        self.tolerance = tolerance

    def get_validation_status(self, world) -> ValidationStatus:
        """Checks if ball is moving forward, i.e. in the +x direction

        :param world: The world msg to validate
        :returns: FAILING if ball doesn't move in the direction
                  PASSING if ball moves in the direction
        """
        current_ball_position = world.ball.current_state.global_position.x_meters

        # if current ball is moving in the right direction (within a tolerance) 
        # return PASSING
        if current_ball_position.x() > self.last_ball_position.x() - self.tolerance:
            return ValidationStatus.PASSING
        
        self.last_ball_position = current_ball_position

        # if current ball is in the wrong direction too far beyond a threshold, 
        # return FAILING
        return ValidationStatus.FAILING

    def get_validation_geometry(self, world) -> ValidationGeometry:
        """
        (override) Shows the last ball position line
        """
        return create_validation_geometry(
            [
                tbots.Rectangle(
                    tbots.Point(
                        self.last_ball_position.x(),
                        tbots.Field(world.field).fieldBoundary().yMin(),
                    ),
                    tbots.Point(
                        self.last_ball_position.x() + 0.01,
                        tbots.Field(world.field).fieldBoundary().yMax(),
                    ),
                )
            ]
        )

    def __repr__(self):
        return "Check that the ball moves forward"


(
    BallEventuallyMovesForward,
    BallEventuallyStopsMovingForward,
    BallAlwaysMovesForward,
    BallNeverMovesForward,
) = create_validation_types(BallMovesForward)


class BallMovesForwardInRegions(BallMovesForward):
    """Checks if ball is moving in a direction in certain regions"""

    def __init__(self, initial_ball_position, regions=[], tolerance: float = 0.1):
        super().__init__(initial_ball_position, tolerance=tolerance)
        self.regions = regions

    def get_validation_status(self, world) -> ValidationStatus:

        for region in self.regions:
            if tbots.contains(
                region, tbots.createPoint(world.ball.current_state.global_position)
            ):
                return super().get_validation_status(world)

        return ValidationStatus.PASSING

    def get_validation_geometry(self, world) -> ValidationGeometry:
        """
        (override) Shows the last ball position line, and the regions the ball should be moving in
        """
        return super().get_validation_geometry(world) + create_validation_geometry(self.regions)

    def __repr__(self):
        return (
            "Check that the ball moves forward"
            + " in regions "
            + ",".join(repr(region) for region in self.regions)
        )


(
    BallEventuallyMovesForwardInRegions,
    BallEventuallyStopsMovingForwardInRegions,
    BallAlwaysMovesForwardInRegions,
    BallNeverMovesForwardInRegions,
) = create_validation_types(BallMovesForwardInRegions)
