# FileCheckTool

A tool for checking the contents of a file against a list of expected like the FileCheck tool in LLVM.

## Dependencies

This tool depends on [google effcee](https://github.com/google/effcee). The google effcee is imported by source code
level sitting in the `effcee` directory.

## Usage

This tool reads the test rules file from the command argument and reads the match input from stdin. Compare the input
against the test rules file. The details of the test rules syntax can be found in
the [LLVM FileCheck Document](https://llvm.org/docs/CommandGuide/FileCheck.html).

### Possible Arguments

- **--test_rules**: The path to the test rules file.

### Rules Example

- Example Test:

  Test Rules File:

    ```
    # CHECK: foo
    # CHECK-NEXT: bar
    ```

  Match output:

    ```
    foo
    bar
    ```

- Example Test Rules with Regex Support:

  Test Rules File:

    ```
    //RUN: %mycc 2>&1 | %FileCheck --test_rules="%s"
    //CHECK: Usage:
    //CHECK-NEXT:         mycc -mode [options] infile
    //CHECK-NEXT: Valid modes:
    //CHECK-NEXT:         -0: Version information only
    //CHECK-NEXT:         -1: {{[a-zA-Z0-9\s]*[[(]*[a-zA-Z0-9]*[\s]*[)]*]*}}
    //CHECK-NEXT:         -2: {{[a-zA-Z0-9\s]*[[(]*[a-zA-Z0-9]*[\s]*[)]*]*}}
    //CHECK-NEXT:         -3: {{[a-zA-Z0-9\s]*[[(]*[a-zA-Z0-9]*[\s]*[)]*]*}}
    //CHECK-NEXT:         -4: {{[a-zA-Z0-9\s]*[[(]*[a-zA-Z0-9]*[\s]*[)]*]*}}
    //CHECK-NEXT:         -5: {{[a-zA-Z0-9\s]*[[(]*[a-zA-Z0-9]*[\s]*[)]*]*}}
    //CHECK-NEXT: Valid options:
    //CHECK-NEXT:         -o outfile: write to outfile instead of standard output
    ```

  Match output:

    ```
    Usage:
        mycc -mode [options] 
    Valid modes:
        -0: Version information only
        -1: Part 1 (not yet implemented)
        -2: Part 2 (not yet implemented)
        -3: Part 3 (not yet implemented)
        -4: Part 4 (not yet implemented)
        -5: Part 5 (not yet implemented)
    Valid options:
        -o outfile: write to outfile instead of standard output
    ```