include_directories (
  ${CMAKE_SOURCE_DIR}/codec/image/zlib/src
  ${CMAKE_SOURCE_DIR}/codec/image/png/common/src
  ${CMAKE_SOURCE_DIR}/codec/image/png/enc/include
  ${CMAKE_SOURCE_DIR}/core/uic/include
)

make_library( longname static )
