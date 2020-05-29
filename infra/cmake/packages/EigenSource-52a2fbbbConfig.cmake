# find_package rejects version with commit number. Commit ID is appended to the package name
# as a workaround.
#
# TODO Find a better way
function(_import)
  if(NOT DOWNLOAD_EIGEN)
    set(EigenSource_FOUND FALSE PARENT_SCOPE)
    return()
  endif(NOT DOWNLOAD_EIGEN)

  nnas_include(ExternalSourceTools)
  nnas_include(ThirdPartyTools)

  # NOTE TensorFlow 2.2 downloads Eign from the following URL
  envoption(EXTERNAL_DOWNLOAD_SERVER "https://gitlab.com")
  envoption(EIGEN_2_2_URL ${EXTERNAL_DOWNLOAD_SERVER}/libeigen/eigen/-/archive/52a2fbbb/eigen-52a2fbbb.tar.gz)
  ExternalSource_Download(EIGEN
    DIRNAME EIGEN-52a2fbbb
    ${EIGEN_2_2_URL}
  )

  set(EigenSource_DIR ${EIGEN_SOURCE_DIR} PARENT_SCOPE)
  set(EigenSource_FOUND TRUE PARENT_SCOPE)
endfunction(_import)

_import()
