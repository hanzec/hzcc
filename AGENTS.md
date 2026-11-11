# Repository guide

## Project and layout

HZCC is an experimental C compiler written in C++17. The default build uses the
handwritten lexer and parser. JVM code generation and optimization sources are
present but are not enabled in `hzcc/CMakeLists.txt`.

- `hzcc/main.cc`: command-line entry point.
- `hzcc/parser/`: tokens, lexical analysis, syntax analysis, and the optional ANTLR path.
- `hzcc/ast/`: AST nodes, types, casts, and symbol tables.
- `hzcc/utils/`: logging, status handling, and shared utilities.
- `hzcc/**/test/`: GoogleTest unit tests beside the code they exercise.
- `test/`: lit/FileCheck integration fixtures and their CMake configuration.
- `cmake/`, `vcpkg.json`: build helpers and dependency configuration.
- `.github/workflows/`: GitHub Actions build, test, coverage, and documentation jobs.
- `scripts/`, `requirements-*.txt`: reproducible documentation and coverage checks.
- `third_party/`: bundled tools; avoid editing vendored sources during ordinary
  compiler changes. Keep dependency updates separate from behavioral fixes.
- `doc/`, `Doxyfile`: optional LaTeX and API documentation.

## Build and validation

Run commands from the repository root. Always use an out-of-source build:

```sh
cmake --preset debug -DUNIT_TESTING=ON
cmake --build --preset debug --parallel
ctest --preset debug -L unit
```

For integration tests, prepare the Python environment described in `README.md`,
then use the separate `e2e` preset to build the compiler and FileCheck tool:

```sh
cmake --preset e2e -DPython3_EXECUTABLE="$PWD/build/e2e-venv/bin/python"
cmake --build --preset e2e --parallel
ctest --preset e2e -L e2e
```

The first configure downloads and builds dependencies through vcpkg and needs
network access. The presets require CMake 3.25 or newer and Ninja. The source helper
uses `CONFIGURE_DEPENDS` to reconfigure automatically when files are added or removed
inside registered directories. See `README.md` for prerequisites, including the
Python environment for integration tests, and optional features.
Debug targets and their vcpkg dependencies must use matching AddressSanitizer
flags on Unix; the Linux/macOS overlays in `cmake/triplets/` provide this alignment.
Reinstall dependencies after changing triplets instead of reusing an incompatible
Abseil binary cache or turning off manifest installation.
Dependency updates follow the newest ports in the pinned vcpkg registry commit;
do not add upstream-version overlay ports to move ahead of that registry.
CI pins the latest stable GCC/LLVM and CMake releases and verifies their versions.

Run the checks relevant to the change and report the commands and results. For
parser or diagnostic behavior changes, add or update the relevant unit test or
lit regression fixture. Do not claim a successful build when dependency setup or
an existing compiler error prevented it.

## Editing conventions

- Follow `.clang-format`: Google-based C++17 style, four-space indentation, and a
  90-column limit. Format the code you change; avoid unrelated formatting churn.
- Match the neighboring file's naming and include-guard conventions. Existing
  mixed-case filenames are significant on case-sensitive filesystems.
- Use the existing `hzcc::Status` / `StatusOr` error-handling conventions and
  document ownership, lifetimes, parser state, or preconditions when they are not
  evident from the interface.
- Document every project-owned class/struct and method with Doxygen, including
  private helpers, constructors, destructors, and operators. This also applies to
  test helper types and experimental modules; vendored third-party code is excluded.
  A complete declaration comment covers its out-of-line definition; avoid duplicate
  or unresolved copydoc references. Explain parameters, results, and ownership where
  relevant. Keep comments accurate rather than implying missing features work.
  Check both Doxygen XML and raw source declarations using the commands in
  `README.md`. Review manifest hashes only after checking the exact new source;
  never update them merely to suppress a failed audit. Python helpers use docstrings
  parsed with `PYTHON_DOCSTRING=NO`.
- Preserve token numbers, diagnostic text, and fixture whitespace unless their
  behavior is intentionally changing. Many integration tests match exact output.
- Keep generated files, build directories, dependency caches, and binaries out of
  source control. Change dependency declarations and build integration together.
- Do not enable the experimental ANTLR, JVM, or optimization paths as an incidental
  part of an unrelated change.
