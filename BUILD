package(
    default_visibility = ["//visibility:private"],
)

licenses(["notice"])

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
    visibility = ["//visibility:public"],
    deps = [
        ":error",
        "//common/memory:placement_new",
    ],
)

cc_library(
    name = "type_traits",
    hdrs = [
        "enum_traits.h",
        "type_traits.h",
    ],
    visibility = ["//visibility:public"],
    deps = [
    ],
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
    name = "enum_traits_test",
    srcs = [
        "enum_traits_test.cc",
    ],
    deps = [
        ":type_traits",
        "@com_googletest//:gtest_main",
    ],
)
