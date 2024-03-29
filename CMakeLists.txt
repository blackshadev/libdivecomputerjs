cmake_minimum_required(VERSION 3.9)
cmake_policy(SET CMP0042 NEW)
cmake_policy(SET CMP0015 NEW)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# set(CMAKE_SKIP_RPATH TRUE CACHE BOOL "If set, runtime paths are not added when using shared libraries.")
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")

project (libdivecomputerjs)

file(GLOB_RECURSE SOURCE_FILES src/*.cc)

include_directories(${CMAKE_JS_INC})
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES} ${CMAKE_JS_SRC})
set_target_properties(${PROJECT_NAME} PROPERTIES PREFIX "" SUFFIX ".node")

find_package(DiveComputer REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE "${LIBDIVECOMPUTER_INCLUDE_DIR}")
link_directories(./lib)
target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB} DiveComputer)

IF(NOT WIN32)
    set_target_properties(${PROJECT_NAME} PROPERTIES
            BUILD_WITH_INSTALL_RPATH FALSE
            LINK_FLAGS "-Wl,-rpath,$ORIGIN/lib")
ENDIF()

IF(WIN32)
    add_custom_command(
        TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${LIBDIVECOMPUTER_LIBRARY}
        $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
ENDIF()

# Include Node-API wrappers
execute_process(COMMAND node -p "require('node-addon-api').include"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    OUTPUT_VARIABLE NODE_ADDON_API_DIR
)
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR ${NODE_ADDON_API_DIR})

target_include_directories(${PROJECT_NAME} PRIVATE ${NODE_ADDON_API_DIR})

add_definitions(-DNAPI_VERSION=6)