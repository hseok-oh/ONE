if(NOT BUILD_TFLITE2_DRIVER)
  return()
endif(NOT BUILD_TFLITE2_DRIVER)

nnfw_find_package(TensorFlowLite EXACT 2.2)