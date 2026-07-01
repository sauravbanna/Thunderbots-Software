#include "software/ai/hl/stp/tactic/fake_pivot_kick/fake_pivot_kick_fsm.h"

#include "shared/constants.h"
#include "software/ai/hl/stp/tactic/move_primitive.h"
#include "software/geom/algorithms/distance.h"

FakePivotKickFSM::FakePivotKickFSM(std::shared_ptr<const TbotsProto::AiConfig> ai_config_ptr)
    : TacticFSM<FakePivotKickFSM>(ai_config_ptr)
{
}

void FakePivotKickFSM::getPossessionAndPivot(
    const Update& event, boost::sml::back::process<DribbleFSM::Update> processEvent)
{
    DribbleFSM::ControlParams control_params{
        .dribble_destination       = event.control_params.kick_origin,
        .final_dribble_orientation = event.control_params.kick_direction,
        .allow_excessive_dribbling = false};

    processEvent(DribbleFSM::Update(control_params, event.common));
}

void FakePivotKickFSM::fakeKickBall(const Update& event)
{
    // Move toward the kick origin as if kicking, but without actually kicking
    event.common.set_primitive(std::make_unique<MovePrimitive>(
        event.common.robot, event.control_params.kick_origin,
        event.control_params.kick_direction,
        TbotsProto::MaxAllowedSpeedMode::PHYSICAL_LIMIT,
        TbotsProto::ObstacleAvoidanceMode::AGGRESSIVE, TbotsProto::DribblerMode::OFF,
        TbotsProto::BallCollisionType::AVOID,
        AutoChipOrKick{AutoChipOrKickMode::OFF, 0}));
}

void FakePivotKickFSM::moveBack(const Update& event)
{
    // Move backwards, away from the kick origin in the opposite direction of the kick
    Point ball_position = event.common.world_ptr->ball().position();

    Angle face_ball_orientation =
        (ball_position - event.common.robot.position()).orientation();

    Point away_from_ball_position = robotPositionToFaceBall(
        ball_position, face_ball_orientation,
        ai_config_ptr->fake_pivot_kick_tactic_config().move_back_threshold() * 2);

    event.common.set_primitive(std::make_unique<MovePrimitive>(
        event.common.robot, away_from_ball_position, face_ball_orientation,
        TbotsProto::MaxAllowedSpeedMode::PHYSICAL_LIMIT,
        TbotsProto::ObstacleAvoidanceMode::AGGRESSIVE, TbotsProto::DribblerMode::OFF,
        TbotsProto::BallCollisionType::AVOID,
        AutoChipOrKick{AutoChipOrKickMode::OFF, 0}));        
}

Point FakePivotKickFSM::robotPositionToFaceBall(const Point& ball_position,
                                          const Angle& face_ball_angle,
                                          double additional_offset)
{
    return ball_position - Vector::createFromAngle(face_ball_angle)
                               .normalize(DIST_TO_FRONT_OF_ROBOT_METERS +
                                          BALL_MAX_RADIUS_METERS + additional_offset);
}


bool FakePivotKickFSM::movedBack(const Update& event)
{
    // Check if the robot has moved far enough from the kick origin
    double move_back_threshold = ai_config_ptr->fake_pivot_kick_tactic_config().move_back_threshold();
    return distance(event.common.robot.position(),
                    event.control_params.kick_origin) > move_back_threshold;
}

void FakePivotKickFSM::kickBall(const Update& event)
{
    event.common.set_primitive(std::make_unique<MovePrimitive>(
        event.common.robot, event.control_params.kick_origin,
        event.control_params.kick_direction,
        TbotsProto::MaxAllowedSpeedMode::PHYSICAL_LIMIT,
        TbotsProto::ObstacleAvoidanceMode::AGGRESSIVE, TbotsProto::DribblerMode::OFF,
        TbotsProto::BallCollisionType::ALLOW, 
        AutoChipOrKick{AutoChipOrKickMode::OFF, 0}));
}

bool FakePivotKickFSM::ballKicked(const Update& event)
{
    return event.common.world_ptr->ball().hasBallBeenKicked(
            event.control_params.kick_direction);
}
