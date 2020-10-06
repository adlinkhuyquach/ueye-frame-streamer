file(GLOB configfiles "./cmake/*.txt.in")
foreach(conf ${configfiles})
  configure_file(${conf} external-dependencies/CMakeLists.txt)

  execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external-dependencies)

  if (result)
    message(FATAL_ERROR "CMake step for external-dependencies failed: ${result}")
  endif ()

  execute_process(COMMAND ${CMAKE_COMMAND} --build .
    RESULT_VARIABLE result
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/external-dependencies)

  if (result)
    message(FATAL_ERROR "Build step for external-dependencies failed: ${result}")
  endif ()
endforeach(conf)
