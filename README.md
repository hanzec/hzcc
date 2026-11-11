# HZCC

[![CI](https://github.com/hanzec/hzcc/actions/workflows/ci.yml/badge.svg)](https://github.com/hanzec/hzcc/actions/workflows/ci.yml)
[![Documentation](https://github.com/hanzec/hzcc/actions/workflows/docs.yml/badge.svg)](https://github.com/hanzec/hzcc/actions/workflows/docs.yml)

An experimental C compiler written in C++17. The default build uses a handwritten
lexer and parser. JVM code generation and optimization code are present in the
repository but are not currently enabled in the compiler target.

## Prerequisites

- A C++17 compiler. CI pins the current stable toolchains,
  [GCC 16.2.0](https://gcc.gnu.org/gcc-16/) and
  [LLVM 23.1.5](https://github.com/llvm/llvm-project/releases/tag/llvmorg-23.1.5).
  On macOS, install the Xcode command-line tools for the platform SDK.
- CMake 3.25 or newer; [CMake 4.4.5](https://cmake.org/download/) is the current
  recommended stable release. Ninja is required for the supplied presets.
- Git, a C/C++ build toolchain, curl, zip, unzip, tar, and pkg-config for vcpkg.
- Python 3.6 or newer and psutil when running the integration tests.
- Python 3.10 or newer for source-documentation checks and coverage reports.
- Doxygen only when building documentation; `pdflatex` and `makeindex` additionally
  enable the developer guide PDF. Graphviz is optional.
- Java only for the optional, experimental ANTLR parser.

CMake downloads vcpkg automatically on the first configure unless you supply an
external `CMAKE_TOOLCHAIN_FILE`. The repository pins vcpkg and its package baseline
so dependency resolution is reproducible. Initial setup needs network access and
can take considerably longer than subsequent builds.

Dependencies are declared in `vcpkg.json`: Abseil, CLI11, gflags, glog, magic_enum,
and RE2, with GoogleTest and ANTLR selected by the corresponding build options.
The integration suite uses the bundled lit runner and `HZCC_FileCheck`; installing
lit with pip is unnecessary. Its per-test timeout support requires the pinned
`psutil` dependency in `requirements-test.txt`.

The registry is pinned to the latest vcpkg commit verified on 2026-11-11,
[`e54ef0fcc9a82033a61e51002552a910a5d57282`](https://github.com/microsoft/vcpkg/commit/e54ef0fcc9a82033a61e51002552a910a5d57282).
It selects the [vcpkg tool 2026-10-29](https://github.com/microsoft/vcpkg-tool/releases/tag/2026-10-29)
and the latest port versions in that
[package baseline](https://github.com/microsoft/vcpkg/blob/e54ef0fcc9a82033a61e51002552a910a5d57282/versions/baseline.json).
Dependency updates follow vcpkg ports rather than separate upstream overrides.
The `#` suffix is a vcpkg port revision.

| Dependency | Baseline version | Used for |
| --- | --- | --- |
| Abseil | `20260107.1#3` | Containers, strings, status utilities |
| CLI11 | `2.7.2` | Compiler command-line parsing |
| gflags | `2.3.0` | FileCheck command-line parsing |
| glog | `0.7.1#3` | Logging |
| magic_enum | `0.9.8` | Enum reflection |
| RE2 | `2025-11-05` | Regular expressions |
| GoogleTest | `1.18.0` | Optional unit tests |
| ANTLR4 | `4.13.2#1` | Optional experimental frontend |

## Build

Run these commands from the repository root:

```sh
cmake --preset debug
cmake --build --preset debug --parallel
./build/debug/bin/hzcc --help
```

To select a compiler explicitly, export `CC` and `CXX` before configuring and keep
them set for subsequent configuration commands. This gives the project and vcpkg
the same compiler pair. Use a fresh build directory when changing compilers.

All presets use Ninja and separate `build/<preset>` directories:

| Preset | Build type | Tests |
| --- | --- | --- |
| `default` | `RelWithDebInfo` | Unit tests |
| `debug` | `Debug` | Unit tests; AddressSanitizer with Clang/GCC on Unix |
| `release` | `Release` | Unit tests |
| `e2e` | `RelWithDebInfo` | Unit and integration tests |

Use `cmake --preset release` followed by `cmake --build --preset release --parallel`
for a release build. Build and test presets share their configure preset's name.

The repository's vcpkg overlay triplets enable AddressSanitizer for Debug
dependencies on x64/arm64 Linux and macOS, matching the project targets. Abseil
[requires consistent sanitizer instrumentation](https://github.com/abseil/abseil-cpp/blob/master/FAQ.md#how-do-i-use-the-llvm-sanitizers-with-abseil).
Keep manifest installation enabled when changing these triplets so vcpkg rebuilds
the affected libraries. Custom triplets or external dependency installations must
provide the same Debug sanitizer flags; do not reuse uninstrumented Abseil libraries.

Without presets, choose an out-of-source build directory and your preferred CMake
generator:

```sh
cmake -S . -B build/local -DCMAKE_BUILD_TYPE=Debug -DUNIT_TESTING=ON
cmake --build build/local --parallel
```

Executables are written to `<build-directory>/bin/`, and CMake generates
`compile_commands.json` in the build directory. The source helper uses
`CONFIGURE_DEPENDS`, so CMake automatically reconfigures when files are added to or
removed from registered source and test directories.

### Build options

| Option | Default | Purpose |
| --- | --- | --- |
| `UNIT_TESTING` | `ON` | Build GoogleTest unit tests. |
| `E2E_TESTING` | `OFF` | Register the lit/FileCheck integration suites. |
| `RUNTIME_CHECK` | `ON` | Enable `HZCC_ENABLE_RUNTIME_CHECK` blocks. |
| `ENABLE_ANTLR` | `OFF` | Select the experimental ANTLR parser path. |
| `GENERATE_DOCUMENTATION` | `OFF` | Configure Doxygen and optional LaTeX documentation. |
| `ENABLE_COVERAGE` | `OFF` | Instrument unit tests for coverage. |

Pass options during configuration, for example
`cmake --preset debug -DE2E_TESTING=ON`. The ANTLR path remains experimental; selecting
it does not imply that it implements all behavior of the handwritten parser.
The JVM and optimization modules still reference older AST APIs and cannot currently
be enabled as working backends; their source documentation is included in the API audit.

## Tests

Unit tests live beside their implementation under `hzcc/**/test/`. Configure and
run them with:

```sh
cmake --preset debug -DUNIT_TESTING=ON
cmake --build --preset debug --parallel
ctest --preset debug -L unit
```

You can also run `./build/debug/bin/hzcc_test` directly, including GoogleTest filters
such as `--gtest_filter='PARSER_COMMON_TOKENTYPE.*'`.

Integration fixtures live under `test/` and check compiler information, lexical
analysis, and syntax analysis. Enable them before building:

```sh
python3 -m venv build/e2e-venv
./build/e2e-venv/bin/python -m pip install -r requirements-test.txt
cmake --preset e2e -DPython3_EXECUTABLE="$PWD/build/e2e-venv/bin/python"
cmake --build --preset e2e --parallel
ctest --preset e2e -L e2e
```

The `hzcc_e2e_test` build target also runs all enabled integration suites. Run
`ctest --preset e2e` without a label filter to run both unit and integration tests.
Preserve fixture whitespace and expected diagnostic text when making unrelated edits.

The current handwritten syntax parser is incomplete: the `lexical_part3` suite
actually contains 85 legacy syntax cases, all of which currently fail. The compiler
information checks and 81 lexical cases pass. These failures remain visible in
CTest; passing unit tests does not imply full C syntax support.

Coverage is optional. Install the reporting tools in a virtual environment and
select that interpreter when configuring:

```sh
python3 -m venv build/coverage-venv
build/coverage-venv/bin/python -m pip install -r requirements-coverage.txt
cmake --preset debug -DENABLE_COVERAGE=ON \
  -DPython3_EXECUTABLE="$PWD/build/coverage-venv/bin/python"
cmake --build --preset debug --target ccov-export-hzcc_test
```

`ccov-hzcc_test` runs and captures the tests; `ccov-report-hzcc_test` prints a
summary; `ccov-export-hzcc_test` writes `build/debug/ccov/hzcc_test.xml` in Cobertura
format. Multi-config generators add the configuration name under `ccov/`.
The module selects LLVM coverage tools or gcov that match the C++ compiler and
rejects a version mismatch. Reports include the shared compiler objects and
exclude generated, external, and vendored dependency sources. The reusable smoke
check covers both CMake generators and failure cleanup:

```sh
build/coverage-venv/bin/python scripts/coverage_smoke.py \
  --compiler clang++ --python build/coverage-venv/bin/python
```

## GitHub Actions

[CI](.github/workflows/ci.yml) runs the latest stable GCC 16.2.0 and LLVM 23.1.5
with CMake 4.4.5 in Debug and Release configurations. Compiler archives, container
images, and external actions are pinned to verified digests or commits. Both the
project and vcpkg inherit the selected `CC` and `CXX`. Core jobs run all unit tests
and the compiler-information/lexical suites; GCC Debug also exports coverage.
The separate syntax job preserves the actual failures of the incomplete parser,
so the overall CI result will remain failing until those cases are implemented.
Logs and test results are retained as workflow artifacts.

[Documentation](.github/workflows/docs.yml) independently runs both documentation
audits and builds HTML/PDF with Doxygen 1.18.0. A manual run on `main` with
`publish_pages` enabled can deploy the verified HTML to GitHub Pages; the repository
must have GitHub Actions selected as its Pages source. The GitLab pipeline and
its private registry images have been removed.

## Usage

```sh
./build/debug/bin/hzcc --version
./build/debug/bin/hzcc --flexical_only test/lexical/hello.c
./build/debug/bin/hzcc --fsyntax_only path/to/source.c
```

Use `--help` for the command-line options. `--flexical_only` prints tokens;
`--fsyntax_only` performs syntax analysis. These modes are mutually exclusive.
`--fno_color` disables colored diagnostics. Lexical output can be written to a file
using `-o path/to/tokens.txt`. Code generation is not part of the active build.

## Development

See [AGENTS.md](AGENTS.md) for the repository layout, editing conventions, and
validation workflow. C++ formatting is defined by `.clang-format` (Google-based,
four-space indentation, 90-column limit). Optional pre-commit hooks are configured
in `.pre-commit-config.yaml` and require Python 3.9 or newer. They pin
[pre-commit-hooks v6.0.0](https://github.com/pre-commit/pre-commit-hooks/releases/tag/v6.0.0)
and [clang-format v23.1.5](https://github.com/pre-commit/mirrors-clang-format/tree/v23.1.5).
Run hooks on the files you change to avoid unrelated formatting changes.

Documentation sources live under `doc/`, with API configuration in `Doxyfile`.
CI builds Doxygen 1.18.0 with the
[upstream scanner fix](https://github.com/doxygen/doxygen/commit/26e908779f448574c6b8918aff21a40565a89a57)
in `cmake/patches/doxygen-1.18.0-find-guards.patch`. The unpatched release can read
before its comment buffer when an ordinary command such as `@brief` has no brace
options, causing an intermittent crash. The patch retains the complete extraction
scope and all output formats.

To reproduce that tool locally, install Flex and Bison 2.7 or newer, then build the
verified release archive with the patch. On macOS, install Bison with `brew install
bison` and add `-DBISON_EXECUTABLE="$(brew --prefix bison)/bin/bison"` to the tool's
CMake configure command below; the system Bison is too old.

```sh
mkdir -p build/tools/doxygen-source
curl --fail --location \
  https://github.com/doxygen/doxygen/releases/download/Release_1_18_0/doxygen-1.18.0.src.tar.gz \
  --output build/tools/doxygen-1.18.0.src.tar.gz
printf '%s  %s\n' a1deed70a6785bbec95a2b2a9e419dc7f7b223a9d74a8644ae611c8e2dcdd354 \
  build/tools/doxygen-1.18.0.src.tar.gz | shasum -a 256 --check
tar -xzf build/tools/doxygen-1.18.0.src.tar.gz --strip-components=1 \
  -C build/tools/doxygen-source
patch -d build/tools/doxygen-source -p1 \
  -i "$PWD/cmake/patches/doxygen-1.18.0-find-guards.patch"
cmake -S build/tools/doxygen-source -B build/tools/doxygen-build -G Ninja \
  -DCMAKE_BUILD_TYPE=Release -Dbuild_doc=OFF -Dbuild_wizard=OFF -Duse_libclang=OFF
cmake --build build/tools/doxygen-build --parallel 2
```

Enable `GENERATE_DOCUMENTATION`, select the patched executable, and build the manual:

```sh
cmake --preset default -DGENERATE_DOCUMENTATION=ON \
  -DDOXYGEN_EXECUTABLE="$PWD/build/tools/doxygen-build/bin/doxygen"
cmake --build --preset default --target documentation
```

HTML is generated under `build/default/doxygen_html/`. If `pdflatex` and `makeindex`
are available, the developer guide is also generated at
`build/default/doc/developers.pdf`; otherwise the target builds HTML only.

All project-owned classes and methods require Doxygen comments, including private
methods, constructors, destructors, operators, test helper types, and experimental
modules. A documented declaration also covers its out-of-line definition.

After enabling documentation, install the source-checking dependencies and run
both coverage gates and their independent checker tests:

```sh
python3 -m venv build/docs-venv
build/docs-venv/bin/python -m pip install -r requirements-docs.txt
cmake --build --preset default --target check_doxygen
build/docs-venv/bin/python scripts/check_source_doxygen.py hzcc scripts \
  --xml-dir build/default/doxygen_xml \
  --review-manifest scripts/source_doxygen_reviews.json \
  --report build/source-docs.json
build/docs-venv/bin/python -m unittest discover -s scripts/tests -v
```

The XML gate checks extracted classes and functions under `hzcc/` and `scripts/`,
including private, experimental, and test-helper code. It rejects missing comments,
empty extraction, and incomplete XML. The source gate independently parses C++,
Java, and Python declarations, including inactive branches and entities omitted
by Doxygen. A definition can inherit documentation only through a verified
declaration mapping. Framework-generated and implicit members are not explicit
source declarations; vendored code is excluded.

Some legacy macros and placeholder syntax cannot be parsed directly. Their
manually reviewed diagnostics are recorded with exact source hashes in
`scripts/source_doxygen_reviews.json`; changing the source requires re-review.
These records never exempt an identified class or method from documentation.
The checker tests use temporary fixtures and require no compiled compiler targets.

On machines with slow home-directory storage, `./build_pyrite.sh` builds the
compiler in a unique temporary directory and copies it to `build/pyrite/hzcc`.
Use `HZCC_BUILD_ROOT` to choose the temporary-build parent, `HZCC_OUTPUT_DIR` to
choose the artifact destination, and `HZCC_BUILD_DOCUMENTATION=ON` to include
documentation. Additional script arguments are forwarded to CMake configuration,
for example `./build_pyrite.sh -DCMAKE_BUILD_TYPE=Debug`.
