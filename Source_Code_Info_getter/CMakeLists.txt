# CMAKE for Source Code Information Getter
cmake_minimum_required(VERSION 3.28.0)
project(SourceCodeInfoGet)

message(STATUS "Configuring SourceCodeInfoGet")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(scig 
	src/main.cpp
)

target_link_libraries(scig
 	PRIVATE
 	clangTooling
 	clangBasic
 	clangASTMatchers
	clangRewrite
)

message(STATUS "Configured SourceCodeInfoGet")

