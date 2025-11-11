cmake_minimum_required(VERSION 3.20)
project(Converter C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CONVERTERLIB converter_lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(PROJECT_INCLUDE_DIR 
  ${CMAKE_SOURCE_DIR}/include
)

option(COVERAGE "Enable code coverage reporting" OFF)

if(CMAKE_C_COMPILER_ID MATCHES "GNU|Clang")
  set(COMMON_FLAGS -Wall -Wextra -Wpedantic -Werror -std=c17)
  if(COVERAGE)
    add_compile_options(--coverage -O0 -g)
    add_link_options(--coverage)
    set(COMMON_FLAGS ${COMMON_FLAGS} -O0 -g)
  else()
    set(COMMON_FLAGS ${COMMON_FLAGS} -O2)
  endif()
elseif(MSVC)
  set(COMMON_FLAGS /W4 /WX)
endif()

add_library(${CONVERTERLIB} STATIC
  src/converter.c
  src/converter_core.c
  src/utils.c
)

target_include_directories(${CONVERTERLIB} PUBLIC ${PROJECT_INCLUDE_DIR})

add_executable(${PROJECT_NAME}
  src/main.c
)

target_link_libraries(${PROJECT_NAME} ${CONVERTERLIB})

target_compile_options(${CONVERTERLIB} PRIVATE ${COMMON_FLAGS})
target_compile_options(${PROJECT_NAME} PRIVATE ${COMMON_FLAGS})

enable_testing()

add_subdirectory(tests)


include(cmake/Format.cmake) 
include(cmake/Valgrind.cmake) 

if(COVERAGE)
  include(cmake/Coverage.cmake)
endif()