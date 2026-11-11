# Experimental passes; the default hzcc build does not enter this directory.
hzcc_add_sources()
hzcc_add_sources(
    "${CMAKE_CURRENT_SOURCE_DIR}/pass"
    "${CMAKE_CURRENT_SOURCE_DIR}/pass_impl")
