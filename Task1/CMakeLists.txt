cmake_minimum_required(VERSION 3.20)
project(Converter C)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_executable(${PROJECT_NAME}
  src/main.c
  src/converter.c
  src/converter_core.c
  src/utils.c
)

target_include_directories(${PROJECT_NAME} PRIVATE include)

if(CMAKE_C_COMPILE_ID MATCHES "GNU|Clang")
  target_compile_options(${PROJECT_NAME} PRIVATE
    -Wall -Wextra -Wpedantic -Werror -std=c17
  )
elseif(CMAKE_C_COMPILE_ID STREQUAL "MSVC")
  target_compile_options(${PROJECT_NAME} PRIVATE
    /W4 /WX
  )
endif()