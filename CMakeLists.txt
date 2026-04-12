cmake_minimum_required(VERSION 3.27)
project(around_the_world VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

if (NOT WIN32)
    string(ASCII 27 ESC)
    set(BLUE "${ESC}[1;33m")
endif()

add_custom_target(
    copy_res ALL
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_SOURCE_DIR}/res"
    "${CMAKE_BINARY_DIR}/res"
    COMMENT "${BLUE}Resource files have been successfully copied into binary directory"
)

add_executable("${CMAKE_PROJECT_NAME}")

add_subdirectory(dependencies/glad)
add_subdirectory(dependencies/glfw)
add_subdirectory(dependencies/glm)
add_subdirectory(dependencies/stb_image)
add_subdirectory(dependencies/freetype)

option(RECORDING_MODE "Records frames for encoding" OFF)

if (RECORDING_MODE)
    # Finding ffmpeg libraries and headers
    find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h REQUIRED)
    find_library(AVFORMAT_LIBRARY avformat REQUIRED)

    find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h REQUIRED)
    find_library(AVCODEC_LIBRARY avcodec REQUIRED)

    find_path(AVUTIL_INCLUDE_DIR libavutil/avutil.h REQUIRED)
    find_library(AVUTIL_LIBRARY avutil REQUIRED)

    find_path(SWSCALE_INCLUDE_DIR libswscale/swscale.h REQUIRED)
    find_library(SWSCALE_LIBRARY swscale REQUIRED)

    set(EXTRA_SOURCES "${CMAKE_SOURCE_DIR}/src/special/video_encoder.cpp")
    set(EXTRA_LIBRARIES ${AVFORMAT_LIBRARY} ${AVCODEC_LIBRARY} ${AVUTIL_LIBRARY} ${SWSCALE_LIBRARY})
    set(EXTRA_INCLUDE_DIRS ${AVFORMAT_INCLUDE_DIR} ${AVCODEC_INCLUDE_DIR} ${AVUTIL_INCLUDE_DIR} ${SWSCALE_INCLUDE_DIR})

    target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE VIDEO_RECORDING)
endif()

set(
    SOURCES
    "${CMAKE_SOURCE_DIR}/src/main.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/engine.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/resource_manager.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/status_handler.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/text_manager.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/handles/shader.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/handles/entity.cpp"
    "${CMAKE_SOURCE_DIR}/src/core/handles/texture.cpp"
)

set(
    INCLUDE_DIRS
    "${CMAKE_SOURCE_DIR}/include/"
)

set_property(
    TARGET "${CMAKE_PROJECT_NAME}"
    PROPERTY CXX_STANDARD 20
)

target_sources("${CMAKE_PROJECT_NAME}" PRIVATE ${SOURCES} ${EXTRA_SOURCES})
target_include_directories("${CMAKE_PROJECT_NAME}" PUBLIC ${INCLUDE_DIRS} ${EXTRA_INCLUDE_DIRS})
target_link_libraries("${CMAKE_PROJECT_NAME}" glad glfw glm stb_image freetype ${EXTRA_LIBRARIES})
