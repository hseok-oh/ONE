function(_TensorFlowLite_import)
  macro(return_unless VAR)
  if(NOT ${VAR})
    set(TensorFlowLite_FOUND PARENT_SCOPE)
    return()
  endif(NOT ${VAR})
  endmacro(return_unless)

  nnas_find_package(TensorFlowSource EXACT 2.2 QUIET)
  return_unless(TensorFlowSource_FOUND)

  nnas_find_package(FlatBuffersSource EXACT 1.12 QUIET)
  return_unless(FlatBuffersSource_FOUND)

  nnas_find_package(Farmhash QUIET)
  return_unless(Farmhash_FOUND)

  nnfw_find_package(Eigen-52a2fbbb QUIET)
  return_unless(Eigen_FOUND)

  nnfw_find_package(GEMMLowp-12fed0c QUIET)
  return_unless(GEMMLowp_FOUND)

  # Optional packages for x64 architecture
  nnas_find_package(NEON2SSESource-1200fe9 QUIET)

  nnfw_find_package(Abseil QUIET)
  return_unless(Abseil_FOUND)


  #=================================
  # Below needs to update for 2.2
  #=================================

  if(NOT TARGET tensorflowlite-2.2)
    nnas_include(ExternalProjectTools)
    add_extdirectory("${CMAKE_CURRENT_LIST_DIR}/Lite" tflite-2.22)
  endif(NOT TARGET tensorflowlite-2.2)

  set(TensorFlowLite_FOUND TRUE PARENT_SCOPE)
endfunction(_TensorFlowLite_import)

_TensorFlowLite_import()
