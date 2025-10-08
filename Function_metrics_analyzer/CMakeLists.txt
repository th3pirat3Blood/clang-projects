# CMAKE for Function Metrics analyzer
cmake_minimum_required(VERSION 3.28.0)
project(FunctionMetricsAnalyzer)

message(STATUS "Configuring FunctionMetricsAnalyzer")

find_package(LLVM REQUIRED CONFIG)
find_package(Clang REQUIRED CONFIG)

include_directories(${LLVM_INCLUDE_DIRS})
include_directories(${CLANG_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

add_executable(function_metrics 
	src/main.cpp
)

target_link_libraries(function_metrics
 	PRIVATE
 	clangTooling
 	clangBasic
 	clangASTMatchers
)

message(STATUS "Configured FunctionMetricsAnalyzer")

