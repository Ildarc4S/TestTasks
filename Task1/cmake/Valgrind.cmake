find_program(VALGRIND valgrind)

if(VALGRIND)
  message(STATUS "valgrind found: ${VALGRIND}")
  
  add_custom_target(valgrind-test
    COMMAND ${VALGRIND} 
      --leak-check=full 
      --error-exitcode=1 
      ./tests/${PROJECT_NAME}_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    DEPENDS ${PROJECT_NAME}_tests
    COMMENT "Running valgrind memory check on Check tests"
  )
else()
  message(WARNING "valgrind not found - valgrind targets disabled")
endif()