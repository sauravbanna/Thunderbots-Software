load("@gameplay_tests_deps//:requirements.bzl", "requirement")

package(default_visibility = ["//visibility:public"])

cc_library(
    name = "lose_ball_tactic",
    srcs = [
        "lose_ball_fsm.cpp",
    ],
    hdrs = [
        "lose_ball_fsm.h",
    ],
    deps = [
        "//shared:constants",
        "//software/ai/evaluation:time_to_travel",
        "//software/ai/hl/stp/tactic",
        "//software/ai/hl/stp/tactic:move_primitive",
        "//software/ai/hl/stp/tactic/move:move_tactic",
        "//software/geom/algorithms",
        "//software/logger",
    ],
)
