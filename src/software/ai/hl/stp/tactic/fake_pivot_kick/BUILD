load("@gameplay_tests_deps//:requirements.bzl", "requirement")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "fake_pivot_kick_tactic",
    srcs = [
        "fake_pivot_kick_fsm.cpp",
    ],
    hdrs = [
        "fake_pivot_kick_fsm.h",
    ],
    deps = [
        "//software/ai/hl/stp/tactic/lose_ball:lose_ball_tactic",
        "//software/ai/hl/stp/tactic/dribble:dribble_tactic"
    ],
)
