function(_GEMMLowp_import)
  nnas_find_package(GEMMLowpSource-12fed0c QUIET)

  if(NOT GEMMLowpSource-12fed0c_FOUND)
    set(GEMMLowp_FOUND FALSE PARENT_SCOPE)
    return()
  endif(NOT GEMMLowpSource-12fed0c_FOUND)

  if(NOT TARGET gemmlowp-2.2)
    find_package(Threads REQUIRED)

    add_library(gemmlowp-2.2 INTERFACE)
    target_include_directories(gemmlowp-2.2 INTERFACE ${GEMMLowpSource_DIR})
    target_link_libraries(gemmlowp-2.2 INTERFACE Threads::Threads)
  endif(NOT TARGET gemmlowp-2.2)

  set(GEMMLowp_FOUND TRUE PARENT_SCOPE)
endfunction(_GEMMLowp_import)

_GEMMLowp_import()
