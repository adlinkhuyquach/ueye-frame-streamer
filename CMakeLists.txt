cmake_minimum_required (VERSION 3.5)
project (aea-ueye-frame-streamer)

# Use ThingAPI CMake package
list(APPEND CMAKE_MODULE_PATH "$ENV{EDGE_SDK_HOME}/cmake")
find_package(ThingAPI REQUIRED)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -O3 -pthread")

file (GLOB_RECURSE SRC "src/*.cpp")

# Glob for additional EDGE APP SDK sources required by the project
file(GLOB_RECURSE SDK_COMPONENT_SRC
  "$ENV{EDGE_APP_SDK_HOME}/src/edge/logger/*.cpp"
  "$ENV{EDGE_APP_SDK_HOME}/src/edge/exception/*.cpp"
)

add_executable (${PROJECT_NAME} ${SRC} ${SDK_COMPONENT_SRC})

find_package( OpenCV REQUIRED )


include(./cmake/instlib.cmake)
find_library(XERCES-C NAMES libxerces-c-3.2.so PATHS ${CMAKE_BINARY_DIR}/xerces-c/src NO_DEFAULT_PATH)

target_include_directories (${PROJECT_NAME}
                            PRIVATE $ENV{EDGE_APP_SDK_HOME}/include
                            PRIVATE ${EDGE_SDK_HOME}/include
                            ${SDK_COMPONENT_INC}
                            PRIVATE ${CMAKE_BINARY_DIR}/xerces-c/src
                            PRIVATE /usr/local/include/opencv4
                            PRIVATE ${CMAKE_BINARY_DIR}/spdlog/include
                            PRIVATE /usr/include/
)

target_link_libraries (${PROJECT_NAME}
                       opencv_core
                       opencv_videoio
                       opencv_imgproc
                       opencv_imgcodecs
                       ThingAPI::ThingAPI
                       ${XERCES-C}
                       /usr/lib/libueye_api.so
                       /usr/lib/libueye_api.so.1
                       /usr/lib/libueye_api.so.4.93
)
