find_program(CLANG_FORMAT clang-format)

if(CLANG_FORMAT)
  message(STATUS "clang-format found: ${CLANG_FORMAT}")
  
  file(GLOB_RECURSE FORMAT_SOURCES
    ${CMAKE_SOURCE_DIR}/src/*.c
    ${CMAKE_SOURCE_DIR}/include/*.h
    ${CMAKE_SOURCE_DIR}/tests/*.c
    ${CMAKE_SOURCE_DIR}/tests/*.h
  )
  
  add_custom_target(clang-format-check
    COMMAND ${CLANG_FORMAT} --dry-run --Werror -style=Google ${FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking code formatting with clang-format"
  )

  add_custom_target(clang-format
    COMMAND ${CLANG_FORMAT} -i -style=Google ${FORMAT_SOURCES}
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
    COMMENT "Checking code formatting with clang-format"
  )

else()
  message(WARNING "clang-format not found - format targets disabled")
endif()