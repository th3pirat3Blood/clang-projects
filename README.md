# Clang Projects

This repo contains tools developed using clang's libtooling

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

### [Naming Convention Checker](Naming_Convention_Checker)

Naming Convention Checker is a static code quality enforcement tool built using Clang LibTooling that verifies whether C/C++ identifiers follow predefined coding style and naming conventions.

### [Source Code Information Getter](Source_Code_Info_getter)

Source Code Information Getter (SCIG) is a Clang LibTooling–based static analysis tool that extracts high-level structural and documentation metrics from C/C++ source code.

