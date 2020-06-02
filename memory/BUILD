package(
    default_visibility = ["//visibility:private"],
)

licenses(["notice"])

cc_library(
    name = "placement_new",
    hdrs = [
        "placement_new.h",
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "placement_new_test",
    srcs = [
        "placement_new_test.cc",
    ],
    deps = [
        ":placement_new",
        "@com_googletest//:gtest_main",
    ],
)
