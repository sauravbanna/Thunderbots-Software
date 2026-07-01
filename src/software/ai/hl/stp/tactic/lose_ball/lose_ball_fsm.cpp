#include "software/ai/hl/stp/tactic/lose_ball/lose_ball_fsm.h"

#include "proto/message_translation/tbots_protobuf.h"
#include "software/ai/hl/stp/tactic/move_primitive.h"
#include "shared/constants.h"

LoseBallFSM::LoseBallFSM(std::shared_ptr<const TbotsProto::AiConfig> ai_config_ptr)
    : TacticFSM<LoseBallFSM>(ai_config_ptr)
{
}

Point LoseBallFSM::robotPositionToFaceBall(const Point& ball_position,
                                           const Angle& face_ball_angle,
                                           double additional_offset)
{
    return ball_position - Vector::createFromAngle(face_ball_angle)
                               .normalize(DIST_TO_FRONT_OF_ROBOT_METERS +
                                          BALL_MAX_RADIUS_METERS + additional_offset);
}

bool LoseBallFSM::lostPossession(const Update& event)
{
    return !event.common.robot.isNearDribbler(
        // avoid cases where ball is exactly on the edge of the robot
        event.common.world_ptr->ball().position(),
        ai_config_ptr->dribble_tactic_config().lose_ball_possession_threshold());
};

void LoseBallFSM::loseBall(const Update& event)
{
    Point ball_position = event.common.world_ptr->ball().position();

    Angle face_ball_orientation =
        (ball_position - event.common.robot.position()).orientation();

    Point away_from_ball_position = robotPositionToFaceBall(
        ball_position, face_ball_orientation,
        ai_config_ptr->lose_ball_tactic_config().lose_ball_possession_threshold() * 2);

    event.common.set_primitive(std::make_unique<MovePrimitive>(
        event.common.robot, away_from_ball_position, face_ball_orientation,
        TbotsProto::MaxAllowedSpeedMode::PHYSICAL_LIMIT,
        TbotsProto::ObstacleAvoidanceMode::AGGRESSIVE, TbotsProto::DribblerMode::OFF,
        TbotsProto::BallCollisionType::AVOID,
        AutoChipOrKick{AutoChipOrKickMode::OFF, 0}));
}
