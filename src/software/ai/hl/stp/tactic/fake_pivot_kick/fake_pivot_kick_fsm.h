#pragma once

#include "software/ai/hl/stp/tactic/dribble/dribble_fsm.h"
#include "software/ai/hl/stp/tactic/move/move_fsm.h"
#include "software/ai/hl/stp/tactic/tactic_base.hpp"
#include "software/geom/point.h"
#include "software/logger/logger.h"

/**
 * Finite State Machine class for Pivot Kick
 */
struct FakePivotKickFSM : TacticFSM<FakePivotKickFSM>
{
    using Update = TacticFSM<FakePivotKickFSM>::Update;
    class StartState;
    class MoveBackState;
    class FakeKickState;

    struct ControlParams
    {
        // The location where the kick will be taken from
        Point kick_origin;
        // The direction the Robot will kick in
        Angle kick_direction;
        // How the robot will chip or kick the ball
        AutoChipOrKick auto_chip_or_kick;
    };

    /**
     * Constructor for FakePivotKickFSM
     *
     * @param ai_config_ptr shared pointer to ai_config
     */
    explicit FakePivotKickFSM(std::shared_ptr<const TbotsProto::AiConfig> ai_config_ptr);

    /**
     * Action that updates the DribbleFSM to get possession of the ball and pivot
     *
     * @param event FakePivotKickFSM::Update event
     * @param processEvent processes the GetBehindBallFSM::Update
     */
    void getPossessionAndPivot(
        const Update& event, boost::sml::back::process<DribbleFSM::Update> processEvent);

    /**
     * Action that kicks the ball
     *
     * @param event FakePivotKickFSM::Update event
     */
    void kickBall(const Update& event);

    /**
     * Guard that checks if the ball has been kicked
     *
     * @param event FakePivotKickFSM::Update event
     *
     * @return if the ball has been kicked
     */
    bool ballKicked(const Update& event);

    /**
     * Converts the ball position to the robot's position given the direction that the
     * robot faces the ball
     *
     * @param ball_position The ball position
     * @param face_ball_angle The angle to face the ball
     * @param additional_offset Additional offset from facing the ball
     *
     * @return the point that the robot should be positioned to face the ball
     */
    static Point robotPositionToFaceBall(const Point& ball_position,
                                         const Angle& face_ball_angle,
                                         double additional_offset = 0.0);

    /**
     * Action that fakes a kick of the ball
     *
     * @param event FakePivotKickFSM::Update event
     */
    void fakeKickBall(const Update& event);

    /**
     * Action that moves the robot back from the ball
     *
     * @param event FakePivotKickFSM::Update event
     */
    void moveBack(const Update& event);
    
    /**
     * Guard that checks if the robot has moved back far enough from the ball
     *
     * @param event FakePivotKickFSM::Update event
     *
     * @return if the robot has moved back far enough
     */
    bool movedBack(const Update& event);

    auto operator()()
    {
        using namespace boost::sml;

        DEFINE_SML_STATE(StartState)
        DEFINE_SML_STATE(MoveBackState)
        DEFINE_SML_STATE(FakeKickState)
        DEFINE_SML_STATE(DribbleFSM)
        DEFINE_SML_STATE(LoseBallFSM)
        DEFINE_SML_EVENT(Update)

        DEFINE_SML_GUARD(ballKicked)
        DEFINE_SML_GUARD(movedBack)
        DEFINE_SML_SUB_FSM_UPDATE_ACTION(getPossessionAndPivot, DribbleFSM)
        DEFINE_SML_ACTION(fakeKickBall)
        DEFINE_SML_ACTION(moveBack)

        return make_transition_table(
            // src_state + event [guard] / action = dest_state
            *StartState_S + Update_E / getPossessionAndPivot_A = DribbleFSM_S,
            DribbleFSM_S + Update_E / getPossessionAndPivot_A, DribbleFSM_S = LoseBallFSM_S,
            LoseBallFSM_S = MoveBackState_S,
            MoveBackState_S + Update_E[!movedBack_G] / moveBack_A, 
            MoveBackState_S + Update_E[movedBack_G] = FakeKickState_S,
            FakeKickState_S + Update_E[!ballKicked_G] / fakeKickBall_A,
            FakeKickState_S + Update_E[ballKicked_G] / SET_STOP_PRIMITIVE_ACTION = X,
            X + Update_E / SET_STOP_PRIMITIVE_ACTION                         = X);
    }
};
