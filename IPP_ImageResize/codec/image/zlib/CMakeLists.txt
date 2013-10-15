make_library( shortname static )
make_use_openmp( )

if( ${CMAKE_SYSTEM_NAME} MATCHES Windows )
    set_target_properties( ${target} PROPERTIES COMPILE_FLAGS "/wd4131 /wd4101 /wd4244 /wd4996 /wd4127" )
endif( )
