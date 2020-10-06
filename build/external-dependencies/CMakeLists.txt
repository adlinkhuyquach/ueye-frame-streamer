cmake_minimum_required(VERSION 2.8.2)

project(third-party-libs NONE)

include(ExternalProject)
ExternalProject_Add(xerces-c
  GIT_REPOSITORY    https://github.com/apache/xerces-c.git
  GIT_TAG           v3.2.2
  SOURCE_DIR        /workspaces/aea-ueye-frame-streamer/build/xerces-c
  BINARY_DIR        /workspaces/aea-ueye-frame-streamer/build/xerces-c
  # CONFIGURE_COMMAND ""
  # BUILD_COMMAND     ""
  INSTALL_COMMAND   ""
  TEST_COMMAND      ""
)
