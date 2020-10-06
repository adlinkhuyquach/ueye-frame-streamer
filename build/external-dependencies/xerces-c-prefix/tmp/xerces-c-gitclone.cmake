
if(NOT "/workspaces/aea-ueye-frame-streamer/build/external-dependencies/xerces-c-prefix/src/xerces-c-stamp/xerces-c-gitinfo.txt" IS_NEWER_THAN "/workspaces/aea-ueye-frame-streamer/build/external-dependencies/xerces-c-prefix/src/xerces-c-stamp/xerces-c-gitclone-lastrun.txt")
  message(STATUS "Avoiding repeated git clone, stamp file is up to date: '/workspaces/aea-ueye-frame-streamer/build/external-dependencies/xerces-c-prefix/src/xerces-c-stamp/xerces-c-gitclone-lastrun.txt'")
  return()
endif()

execute_process(
  COMMAND ${CMAKE_COMMAND} -E rm -rf "/workspaces/aea-ueye-frame-streamer/build/xerces-c"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to remove directory: '/workspaces/aea-ueye-frame-streamer/build/xerces-c'")
endif()

# try the clone 3 times in case there is an odd git clone issue
set(error_code 1)
set(number_of_tries 0)
while(error_code AND number_of_tries LESS 3)
  execute_process(
    COMMAND "/usr/bin/git"  clone --no-checkout "https://github.com/apache/xerces-c.git" "xerces-c"
    WORKING_DIRECTORY "/workspaces/aea-ueye-frame-streamer/build"
    RESULT_VARIABLE error_code
    )
  math(EXPR number_of_tries "${number_of_tries} + 1")
endwhile()
if(number_of_tries GREATER 1)
  message(STATUS "Had to git clone more than once:
          ${number_of_tries} times.")
endif()
if(error_code)
  message(FATAL_ERROR "Failed to clone repository: 'https://github.com/apache/xerces-c.git'")
endif()

execute_process(
  COMMAND "/usr/bin/git"  checkout v3.2.2 --
  WORKING_DIRECTORY "/workspaces/aea-ueye-frame-streamer/build/xerces-c"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to checkout tag: 'v3.2.2'")
endif()

set(init_submodules TRUE)
if(init_submodules)
  execute_process(
    COMMAND "/usr/bin/git"  submodule update --recursive --init 
    WORKING_DIRECTORY "/workspaces/aea-ueye-frame-streamer/build/xerces-c"
    RESULT_VARIABLE error_code
    )
endif()
if(error_code)
  message(FATAL_ERROR "Failed to update submodules in: '/workspaces/aea-ueye-frame-streamer/build/xerces-c'")
endif()

# Complete success, update the script-last-run stamp file:
#
execute_process(
  COMMAND ${CMAKE_COMMAND} -E copy
    "/workspaces/aea-ueye-frame-streamer/build/external-dependencies/xerces-c-prefix/src/xerces-c-stamp/xerces-c-gitinfo.txt"
    "/workspaces/aea-ueye-frame-streamer/build/external-dependencies/xerces-c-prefix/src/xerces-c-stamp/xerces-c-gitclone-lastrun.txt"
  RESULT_VARIABLE error_code
  )
if(error_code)
  message(FATAL_ERROR "Failed to copy script-last-run stamp file: '/workspaces/aea-ueye-frame-streamer/build/external-dependencies/xerces-c-prefix/src/xerces-c-stamp/xerces-c-gitclone-lastrun.txt'")
endif()

