include_directories (
  ${CMAKE_SOURCE_DIR}/codec/image/jpegxr/common/include
  ${CMAKE_SOURCE_DIR}/codec/image/jpegxr/dec/include
  ${CMAKE_SOURCE_DIR}/core/uic/include
)

make_library( longname static )
