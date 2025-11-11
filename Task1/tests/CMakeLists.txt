if(WIN32)
  find_library(Check_LIBRARY NAMES checkDynamic)
else()
  find_library(Check_LIBRARY NAMES check)
endif()

find_path(Check_INCLUDE_DIR check.h)

if(Check_INCLUDE_DIR AND Check_LIBRARY)
  message(STATUS "Check found!")
  add_library(CheckLib INTERFACE IMPORTED)
  target_include_directories(CheckLib INTERFACE ${Check_INCLUDE_DIR})
  target_link_libraries(CheckLib INTERFACE
    ${Check_LIBRARY}
    m
    subunit
  )

  add_executable(${PROJECT_NAME}_tests
    test_main.c
    test_parser.c
    test_conversion.c
    test_validation.c
    test_utils.c
    test_integration.c
  )

  target_include_directories(${PROJECT_NAME}_tests PRIVATE ../include)
  target_link_libraries(${PROJECT_NAME}_tests ${CONVERTERLIB} CheckLib)

  target_compile_options(${PROJECT_NAME}_tests PRIVATE ${COMMON_FLAGS})
  
  add_test(NAME ${PROJECT_NAME}_tests COMMAND ${PROJECT_NAME}_tests)
else()
  message(STATUS "Check NOT found")
endif()
