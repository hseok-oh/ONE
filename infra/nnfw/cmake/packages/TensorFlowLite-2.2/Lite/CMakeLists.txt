# NOTE The followings SHOULD be defined before using this CMakeLists.txt
#
#  'TensorFlowSource_DIR' variable
#  'FlatBuffersSource_DIR' variable
#  'NEON2SSESource_DIR' variable
#  'eigen-2.2' target
#  'gemmlowp-2.2' target
#  'farmhash' target
#  'abseil' target
#
message(STATUS "Build TensorFlow Lite from ${TensorFlowSource_DIR}")

set(TENSORFLOW_LITE_BASE ${TensorFlowSource_DIR}/tensorflow/lite)

file(GLOB CORE_SRCS "${TENSORFLOW_LITE_BASE}/*.c"
                    "${TENSORFLOW_LITE_BASE}/*.cc"
                    "${TENSORFLOW_LITE_BASE}/c/*.c"
                    "${TENSORFLOW_LITE_BASE}/core/api/*.cc"
                    "${TENSORFLOW_LITE_BASE}/experimental/resource/*.cc"
                    "${TENSORFLOW_LITE_BASE}/profiling/*.cc")
file(GLOB_RECURSE CORE_TESTS "${TENSORFLOW_LITE_BASE}/*test*.cc")
list(REMOVE_ITEM CORE_SRCS ${CORE_TESTS})

file(GLOB_RECURSE KERNEL_SRCS "${TENSORFLOW_LITE_BASE}/kernels/*.cc")
file(GLOB_RECURSE KERNEL_TESTS "${TENSORFLOW_LITE_BASE}/kernels/*test*.cc")
list(REMOVE_ITEM KERNEL_SRCS ${KERNEL_TESTS})

# This kernels are not used on nnfw
list(REMOVE_ITEM KERNEL_SRCS "${TENSORFLOW_LITE_BASE}/kernels/internal/spectrogram.cc")

file(GLOB DELEGATE_SRCS "${TENSORFLOW_LITE_BASE}/delegates/*.cc"
                        "${TENSORFLOW_LITE_BASE}/delegates/nnapi/*.cc")
file(GLOB DELEGATE_TESTS "${TENSORFLOW_LITE_BASE}/delegates/*test*.cc"
                         "${TENSORFLOW_LITE_BASE}/delegates/nnapi/*test*.cc")
list(REMOVE_ITEM DELEGATE_SRCS ${DELEGATE_TESTS})

list(APPEND TFLITE_SRCS ${CORE_SRCS})
list(APPEND TFLITE_SRCS ${KERNEL_SRCS})
list(APPEND TFLITE_SRCS ${DELEGATE_SRCS})

# Platform dependent codes
file(GLOB_RECURSE PLATFORM_DEP_SRCS "${TENSORFLOW_LITE_BASE}/*android*.cc"
                                    "${TENSORFLOW_LITE_BASE}/*ios*.cc"
                                    "${TENSORFLOW_LITE_BASE}/*disable*.cc")
list(REMOVE_ITEM TFLITE_SRCS ${PLATFORM_DEP_SRCS})

add_library(tensorflow-lite-2.2 STATIC ${TFLITE_SRCS})
target_include_directories(tensorflow-lite-2.2 SYSTEM PUBLIC ${TensorFlowSource_DIR} ${FlatBuffersSource_DIR}/include)
if(NEON2SSESource_FOUND)
  target_include_directories(tensorflow-lite-2.2 SYSTEM PUBLIC ${NEON2SSESource_DIR})
endif(NEON2SSESource_FOUND)

target_compile_definitions(tensorflow-lite-2.2 PUBLIC "GEMMLOWP_ALLOW_SLOW_SCALAR_FALLBACK")
set_property(TARGET tensorflow-lite-2.2 PROPERTY POSITION_INDEPENDENT_CODE ON)
target_link_libraries(tensorflow-lite-2.2 farmhash eigen-2.2 gemmlowp-2.2 abseil ${LIB_PTHREAD} dl)

if(ANDROID)
  target_link_libraries(tensorflow-lite-2.2 log)
  target_include_directories(tensorflow-lite-2.2 PUBLIC "${NDK_DIR}/..")
endif()
