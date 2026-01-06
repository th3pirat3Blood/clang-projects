# Clang Projects

This repository hosts a collection of compiler tooling utilities built on LLVM and Clang LibTooling for analyzing and transforming C/C++ source code at compile time. The tools leverage Clang’s *AST, AST Matchers, Rewriter, and Preprocessor APIs* to perform both read-only static analysis and source-to-source transformations.

The suite includes tools for function-level instrumentation, code metric extraction, naming convention enforcement, and comprehensive source code introspection. Together, these tools demonstrate practical compiler engineering techniques for building scalable analysis and transformation pipelines on top of Clang.

Designed to operate on real-world projects via *compile_commands.json*, the tools are suitable for code quality analysis, debugging instrumentation, and compiler research or tooling extensions.

## Components

### [Function Metrics Analyzer](Function_metrics_analyzer) 

Function Metrics Analyzer is a static analysis tool built using Clang LibTooling that extracts function-level code metrics from C/C++ source files.

Its goal is to:
+ Analyze source code without executing it
+ Collect structural metrics for each function
+ Provide insights useful for:
  + Code quality assessment
  + Complexity estimation
  + Refactoring decisions
  + API analysis
    

### [Function Log Injector](Function_log_injector) 

FunctionLogInjector is a Clang LibTooling–based source-to-source transformation tool that automatically instruments C/C++ code with logging statements at the entry and exit of every function. This removes the need for developers to manually add repetitive logging code and ensures consistent, centralized function-level tracing across large codebases.

Its goal is to:
+ Analyze source code without executing it
+ Collect structural metrics for each function
+ Provide insights useful for:
+ Code quality assessment
+ Complexity estimation
+ Refactoring decisions
+ API analysis



### [Naming Convention Checker](Naming_Convention_Checker)

Naming Convention Checker is a static code quality enforcement tool built using Clang LibTooling that verifies whether C/C++ identifiers follow predefined coding style and naming conventions. 

Its purpose is to:
+ Enforce consistent naming standards across a codebase
+ Detect style violations at compile time
+ Improve readability, maintainability, and code review efficiency
+ Automate what is usually a manual code review task

Naming Conventions enforced by the tool are:

| Entity Type                      | Expected Convention | Example           |
| -------------------------------- | ------------------- | ----------------- |
| Variables                        | `camelCase`         | `myVariable`      |
| Functions                        | `camelCase`         | `doComputation()` |
| Classes / Structs                | `PascalCase`        | `MatrixSolver`    |
| Constants (`const`, `constexpr`) | `UPPER_CASE`        | `MAX_SIZE`        |


### [Source Code Information Getter](Source_Code_Info_getter)

Source Code Information Getter (SCIG) is a Clang LibTooling–based static analysis tool that extracts high-level structural and documentation metrics from C/C++ source code.

Its objective is to:
+ Provide a summary view of a source file without executing it
+ Collect functions, variables, and comment statistics
+ Support selective analysis via command-line flags
+ Help developers understand code size, structure, and documentation density

