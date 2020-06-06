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
    name = "ref_counted",
    hdrs = [
        "ref_counted.h",
        "thread_unsafe_ref_control.h",
        "weak_ref_counted.h",
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

cc_test(
    name = "ref_counted_test",
    srcs = [
        "ref_counted_test.cc",
    ],
    deps = [
        ":ref_counted",
        "//common/test_structures:base_types",
        "@com_googletest//:gtest_main",
    ],
)

cc_test(
    name = "weak_ref_counted_test",
    srcs = [
        "weak_ref_counted_test.cc",
    ],
    deps = [
        ":ref_counted",
        "//common/test_structures:base_types",
        "@com_googletest//:gtest_main",
    ],
)
