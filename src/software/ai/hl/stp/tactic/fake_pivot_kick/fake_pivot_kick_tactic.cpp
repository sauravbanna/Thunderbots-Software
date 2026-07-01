#include "software/ai/hl/stp/tactic/fake_pivot_kick/fake_pivot_kick_tactic.h"

#include "proto/parameters.pb.h"
#include "shared/constants.h"
#include "software/geom/algorithms/intersection.h"
#include "software/geom/point.h"
#include "software/geom/ray.h"
#include "software/geom/segment.h"
#include "software/logger/logger.h"

FakePivotKickTactic::FakePivotKickTactic(
    std::shared_ptr<const TbotsProto::AiConfig> ai_config_ptr)
    : TacticBase<PivotKickFSM, DribbleFSM>(
          {RobotCapability::Move, RobotCapability::Dribble}, ai_config_ptr)
{
}

void FakePivotKickTactic::accept(TacticVisitor& visitor) const
{
    visitor.visit(*this);
}

void FakePivotKickTactic::updateControlParams(const Point& kick_origin,
                                              const Angle& kick_direction,
                                              AutoChipOrKick auto_chip_or_kick)
{
    control_params.kick_origin       = kick_origin;
    control_params.kick_direction    = kick_direction;
    control_params.auto_chip_or_kick = auto_chip_or_kick;
}
