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

cc_library(
    name = "error",
    srcs = [
        "error.cc",
    ],
    hdrs = [
        "error.h",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "error_or",
    hdrs = [
        "error_or.h",
    ],
    deps = [
        ":error",
        ":placement_new",
    ],
    visibility = ["//visibility:public"],
)

cc_library(
    name = "type_traits",
    hdrs = [
        "type_traits.h",
    ],
    deps = [
    ],
    visibility = ["//visibility:public"],
)

cc_test(
    name = "error_test",
    srcs = [
        "error_test.cc",
    ],
    deps = [
        ":error",
        "@com_googletest//:gtest_main",
    ],
)

cc_test(
    name = "error_or_test",
    srcs = [
        "error_or_test.cc",
    ],
    deps = [
        ":error_or",
        "@com_googletest//:gtest_main",
    ],
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
