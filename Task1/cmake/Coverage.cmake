find_program(GCOV gcov)
find_program(LCOV lcov)
find_program(GENHTML genhtml)

if(GCOV AND LCOV AND GENHTML)
    message(STATUS "Coverage tools found: gcov, lcov, genhtml")

  add_custom_target(coverage
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    COMMAND ${LCOV} --capture --directory ${CMAKE_BINARY_DIR} --output-file coverage.info
    COMMAND ${LCOV} --remove coverage.info '${CMAKE_SOURCE_DIR}/tests/*' --output-file coverage.filtered.info
    COMMAND ${GENHTML} coverage.filtered.info --output-directory coverage
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Generating code coverage report"
  )
else()
  message(WARNING "Coverage tools not found - coverage targets disabled")
  if(NOT GCOV)
    message(WARNING "gcov not found")
  endif()
  if(NOT LCOV)
    message(WARNING "lcov not found")
  endif()
  if(NOT GENHTML)
    message(WARNING "genhtml not found")
  endif()
endif()