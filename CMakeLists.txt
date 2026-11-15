cmake_minimum_required(VERSION 3.20)
project(cxx_from_scratch VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(cxx src/main.cpp)
target_compile_options(cxx PRIVATE
  $<$<CXX_COMPILER_ID:GNU,Clang>:-Wall;-Wextra;-Wpedantic;-Wconversion>
)

include(CTest)
if(BUILD_TESTING)
  add_test(NAME smoke_hello COMMAND sh ${CMAKE_CURRENT_SOURCE_DIR}/tests/smoke.sh $<TARGET_FILE:cxx> ${CMAKE_CURRENT_SOURCE_DIR}/examples/hello.cpp)
endif()
