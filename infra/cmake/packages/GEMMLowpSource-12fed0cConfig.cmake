function(_GEMMLowpSource_import)
  if(NOT DOWNLOAD_GEMMLOWP)
    set(GEMMLowpSource_FOUND FALSE PARENT_SCOPE)
    return()
  endif(NOT DOWNLOAD_GEMMLOWP)

  nnas_include(ExternalSourceTools)
  nnas_include(OptionTools)

  # NOTE TensorFlow 2.2 uses the following URL
  envoption(EXTERNAL_DOWNLOAD_SERVER "https://github.com")
  envoption(GEMMLOWP_2_2_URL ${EXTERNAL_DOWNLOAD_SERVER}/google/gemmlowp/archive/12fed0cd7cfcd9e169bf1925bc3a7a58725fdcc3.tar.gz)
  ExternalSource_Download(GEMMLOWP
    DIRNAME GEMMLOWP-12fed0c
    ${GEMMLOWP_2_2_URL}
  )

  set(GEMMLowpSource_DIR ${GEMMLOWP_SOURCE_DIR} PARENT_SCOPE)
  set(GEMMLowpSource_FOUND TRUE PARENT_SCOPE)
endfunction(_GEMMLowpSource_import)

_GEMMLowpSource_import()
