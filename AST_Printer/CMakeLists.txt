cmake_minimum_required(VERSION 3.28.3)
project(ASTViewer)

message(STATUS "Configuring ASTViewer")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(astviewer 
	src/main.cpp
)

target_link_libraries(astviewer
	PRIVATE
	clangTooling
	clangBasic
	clangASTMatchers
)

message(STATUS "Configuring ASTViewer")

