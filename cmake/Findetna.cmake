
find_path(ETNA_INCLUDE_DIR NAMES etna)

find_library(ETNA_LIBRARY NAMES etna)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(etna DEFAULT_MSG ETNA_LIBRARY ETNA_INCLUDE_DIR)

mark_as_advanced(ETNA_INCLUDE_DIR ETNA_LIBRARY)