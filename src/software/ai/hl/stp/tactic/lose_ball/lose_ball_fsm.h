#pragma once

#include "proto/parameters.pb.h"
#include "shared/constants.h"
#include "software/ai/evaluation/time_to_travel.h"
#include "software/ai/hl/stp/tactic/move/move_fsm.h"
#include "software/ai/hl/stp/tactic/tactic_base.hpp"
#include "software/ai/hl/stp/tactic/transition_conditions.h"
#include "software/geom/algorithms/contains.h"
#include "software/geom/algorithms/convex_angle.h"
#include "software/geom/algorithms/distance.h"

/**
 * Finite State Machine class for Losing the Ball
 */
struct LoseBallFSM : TacticFSM<LoseBallFSM>
{
   public:
    struct ControlParams
    {
    };

    using Update = TacticFSM<LoseBallFSM>::Update;

    class LoseBall;

    /**
     * Constructor for LoseBallFSM
     *
     * @param ai_config_ptr shared ptr to ai_config
     */
    explicit LoseBallFSM(std::shared_ptr<const TbotsProto::AiConfig> ai_config_ptr);

    /**
     * Converts the ball position to the robot's position given the direction that the
     * robot faces the ball
     *
     * @param ball_position The ball position
     * @param face_ball_angle The angle to face the ball
     * @param additional_offset Additional offset from facing the ball
     *
     * @return the point that the robot should be positioned to face the ball and dribble
     * the ball
     */
    static Point robotPositionToFaceBall(const Point& ball_position,
                                         const Angle& face_ball_angle,
                                         double additional_offset = 0.0);

    /**
     * Action to lose possession of the ball
     *
     * @param event LoseBallFSM::Update
     */
    void loseBall(const Update& event);

    /**
     * Guard that checks if the robot has lost possession of the ball
     *
     * @param event LoseBallFSM::Update
     *
     * @return if the ball possession has been lost
     */
    bool lostPossession(const Update& event);

    auto operator()()
    {
        using namespace boost::sml;

        DEFINE_SML_STATE(LoseBall)

        DEFINE_SML_EVENT(Update)
        DEFINE_SML_GUARD(lostPossession)
        DEFINE_SML_ACTION(loseBall)

        return make_transition_table(
            *LoseBall_S + Update_E[!lostPossession_G] / loseBall_A,
            LoseBall_S + Update_E[lostPossession_G]              = X);
    }
};
