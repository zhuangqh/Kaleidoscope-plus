cmake_minimum_required(VERSION 3.6)
project(Kaleidoscope_plus)

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

find_package(LLVM CONFIG REQUIRED)

set(SOURCE_FILES
        example/main.cpp
    src/AST.hpp
    src/Lexer.cpp
    src/Lexer.hpp
    src/logging.cpp
    src/logging.hpp
    src/Parser.cpp
    src/Parser.hpp src/AST.cpp src/CodeGenerate.cpp src/CodeGenerate.h)

include_directories(${LLVM_INCLUDE_DIRS})

add_executable(Kaleidoscope_plus ${SOURCE_FILES})

llvm_map_components_to_libnames(llvm_libs support core irreader)

target_link_libraries(Kaleidoscope_plus ${llvm_libs})