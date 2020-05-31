function(_Abseil_import)
  # Tensorflow lite 2.2 uses below abseil version
  nnas_find_package(AbseilSource-43ef214 QUIET)

  if(NOT AbseilSource_FOUND)
    message("Abseil: NOT FOUND (Cannot access source)")
    set(Abseil_FOUND FALSE PARENT_SCOPE)
    return()
  endif(NOT AbseilSource_FOUND)

  if(NOT TARGET abseil)
    nnas_include(ExternalProjectTools)

    # NOTE Turn off abseil testing
    set(BUILD_TESTING OFF)
    add_extdirectory("${AbseilSource_DIR}" ABSEIL)

    add_library(abseil INTERFACE)
    target_link_libraries(abseil INTERFACE
      # From "Available Abseil CMake Public Targets" in CMake/README.md
      absl::base
      absl::algorithm
      absl::container
      absl::debugging
      absl::memory
      absl::meta
      absl::numeric
      absl::strings
      absl::synchronization
      absl::time
      absl::utility
    )
  endif(NOT TARGET abseil)

  set(Abseil_FOUND TRUE PARENT_SCOPE)
endfunction(_Abseil_import)

_Abseil_import()
