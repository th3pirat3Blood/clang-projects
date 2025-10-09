# Function Metrics Analyzer
---

This tool works on a C++ code and gives details of the user defined functions in the file, including name, lines of code, source location etc.

## Prerequisite

The building and installation of this tool requires following

+ cmake v3.28.0 or more
+ clang-extra-tools v20.1.0
+ Make / Ninja

## Building & Installation

Create a build directory and traverse into it using following: 

```bash
mkdir -p build && cd build
```

If using **Ninja** then:

```bash
cmake ../ -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && ninja 
```

If using **make** then:

```bash
cmake ../ -G "Unix Makefiles" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON && make 
```

## Usage

Once the tool is built then use the following command to run it over a C++ source code

```bash
    ./function_metrics <path to c++ source file>
```

### Example

```bash 
    ./function_metrics test/sample1.cpp
```

Expected Output:

```bash
Function : bar
Lines of code : 4
Parameter count : 0
Location : clang-projects/Function_metrics_analyzer/build/../test/sample1.cpp:7

Function : main
Lines of code : 4
Parameter count : 0
Location : clang-projects/Function_metrics_analyzer/build/../test/sample1.cpp:13
```

 
