
IF(NOT WIN32)
    include(ExternalProject)  
    ExternalProject_Add(libdivecomputer
        URL https://github.com/libdivecomputer/libdivecomputer/archive/refs/heads/master.zip
        CONFIGURE_COMMAND autoreconf --install && ./configure --prefix=${CMAKE_CURRENT_SOURCE_DIR}/deps/
        BUILD_IN_SOURCE TRUE
    )
ENDIF()

IF(WIN32)
    add_custom_target(libdivecomputer)
    add_library(divecomputer SHARED IMPORTED)
    add_custom_command(
        TARGET libdivecomputer PRE_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/dev/include
        ${CMAKE_SOURCE_DIR}/deps/include
    )
    set_property(TARGET divecomputer PROPERTY IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/dev/bin/libdivecomputer-0.dll")
    set_property(TARGET divecomputer PROPERTY IMPORTED_IMPLIB "${CMAKE_CURRENT_SOURCE_DIR}/dev/def/libdivecomputer-0.lib")
    set_target_properties(divecomputer PROPERTIES LINKER_LANGUAGE C)
    set_target_properties(divecomputer PROPERTIES BIN_FILES "${CMAKE_CURRENT_SOURCE_DIR}/dev/bin/")
ENDIF()

include_directories(${CMAKE_SOURCE_DIR}/deps/include)
link_directories(${CMAKE_SOURCE_DIR}/deps/lib)