cmake_minimum_required(VERSION 4.0.0)

project(compiler C)

set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED true)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ../bin/)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

set(FLEX_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/tools/flex")
set(BISON_EXECUTABLE "${CMAKE_CURRENT_SOURCE_DIR}/tools/bison")

find_package(FLEX REQUIRED)
find_package(BISON REQUIRED)

set(GEN_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
file(MAKE_DIRECTORY ${GEN_DIR})


set(BISON_OUTPUT_C "${GEN_DIR}/parser.c")
set(BISON_OUTPUT_H "${GEN_DIR}/parser.h")

BISON_TARGET(parser
  src/parser.yy 
  ${BISON_OUTPUT_C}
  DEFINES_FILE ${BISON_OUTPUT_H}
)

set(FLEX_OUTPUT_C "${GEN_DIR}/scanner.c")

FLEX_TARGET(scanner
  src/scanner.flex 
  ${FLEX_OUTPUT_C}
)

ADD_FLEX_BISON_DEPENDENCY(scanner parser)


set(INCLUDE_DIRS 
  "${CMAKE_CURRENT_SOURCE_DIR}/include" 
  "${GEN_DIR}"
)

set(PCHS "")
set(COMPILE_OPTS "-O0" "-g3")

file(GLOB SOURCES src/*.c)


add_executable(compiler)
target_include_directories(compiler PRIVATE ${INCLUDE_DIRS})
target_link_directories(compiler PRIVATE ${LINK_DIRS})
target_link_libraries(compiler PRIVATE ${LINK_LIBS})
target_compile_definitions(compiler PRIVATE ${COMPILE_DEFS})
target_compile_options(compiler PRIVATE  ${COMPILE_OPTS})
target_precompile_headers(compiler PRIVATE ${PCHS})

target_sources(
  compiler 
  PRIVATE 
  ${SOURCES} 
  ${FLEX_scanner_OUTPUTS}
  ${BISON_parser_OUTPUTS}
)
