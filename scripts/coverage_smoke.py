#!/usr/bin/env python3
"""Exercise coverage with a real compiler, object library, and isolated test runs.

Run with a coverage-enabled Python environment, e.g.:
  python scripts/coverage_smoke.py --compiler /usr/bin/clang++ --python build/coverage-venv/bin/python
  python scripts/coverage_smoke.py --compiler /opt/homebrew/bin/g++-16 --python build/coverage-venv/bin/python
The selected interpreter must provide lcov_cobertura (Clang) or gcovr (GCC).
No project compiler sources are built; temporary fixtures are removed afterward.
"""

import argparse
from pathlib import Path
import shutil
import subprocess
import sys
import tempfile
import xml.etree.ElementTree as ET


def run(command, expect_failure=False):
    """Run an argument vector, surfacing full output only when the result is wrong."""
    result = subprocess.run(command, text=True, capture_output=True, check=False)
    if (result.returncode != 0) != expect_failure:
        raise AssertionError(f"Unexpected exit {result.returncode}: {command}\n"
                             f"{result.stdout}\n{result.stderr}")
    return result


def write_fixture(root, module_dir):
    """Create owned, third-party, and external sources plus two core consumers."""
    source = root / "source tree"
    (source / "module").mkdir(parents=True)
    (source / "third_party").mkdir()
    (root / "external sources").mkdir()
    (source / "CMakeLists.txt").write_text(f'''cmake_minimum_required(VERSION 3.25)
project(CoverageSmoke LANGUAGES CXX)
list(APPEND CMAKE_MODULE_PATH [==[{module_dir}]==])
include(code-coverage)
add_subdirectory(module)
''')
    (source / "module/CMakeLists.txt").write_text('''
add_library(hzcc_core OBJECT core.cc ../third_party/third.cc "../../external sources/outside.cc")
target_code_coverage(hzcc_core PUBLIC)
add_executable(hzcc_test main.cc)
target_link_libraries(hzcc_test PRIVATE hzcc_core)
if(FAIL_COVERAGE_TEST)
    target_code_coverage(hzcc_test ARGS fail)
else()
    target_code_coverage(hzcc_test)
endif()
add_executable(smoke_other other.cc)
target_link_libraries(smoke_other PRIVATE hzcc_core)
target_code_coverage(smoke_other)
''')
    (source / "module/core.cc").write_text(
        "int core(int x) {\n    if (x > 0) { return 2; }\n    return 7;\n}\n")
    (source / "module/main.cc").write_text(
        "int core(int); int third(); int outside();\n"
        "int main(int argc, char**) {\n    if (argc > 1) return 13;\n"
        "    return core(argc) == 2 && third() == 3 && outside() == 5 ? 0 : 1;\n}\n")
    (source / "module/other.cc").write_text(
        "int core(int);\nint main() { return core(-1) == 7 ? 0 : 1; }\n")
    (source / "third_party/third.cc").write_text("int third() { return 3; }\n")
    (root / "external sources/outside.cc").write_text("int outside() { return 5; }\n")
    return source


def inspect_xml(path, other=False):
    """Assert core coverage, external filtering, and target-specific branch hits."""
    document = ET.parse(path)
    classes = document.findall(".//class")
    names = {item.attrib["filename"] for item in classes}
    assert "module/core.cc" in names, names
    assert ("module/other.cc" if other else "module/main.cc") in names, names
    assert not any("third" in name or "outside" in name for name in names), names
    core = next(item for item in classes if item.attrib["filename"] == "module/core.cc")
    line = next(item for item in core.findall("./lines/line") if item.attrib["number"] == "3")
    assert (int(line.attrib["hits"]) > 0) == other, ET.tostring(core)
    return {item.attrib["number"]: item.attrib["hits"] for item in core.findall("./lines/line")}


def main():
    """Check single/multi-config reports, repeatability, and failed-test cleanup."""
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--compiler", required=True)
    parser.add_argument("--python", default=sys.executable)
    parser.add_argument("--cmake", default="cmake")
    args = parser.parse_args()
    compiler = shutil.which(args.compiler) or args.compiler
    python = str(Path(args.python).absolute())
    module_dir = Path(__file__).resolve().parents[1] / "cmake"
    with tempfile.TemporaryDirectory(prefix="hzcc coverage smoke ") as temporary:
        root = Path(temporary)
        source = write_fixture(root, module_dir)
        for generator in ("Ninja", "Ninja Multi-Config"):
            build = root / f"{generator} build"
            configure = [args.cmake, "-S", str(source), "-B", str(build), "-G", generator,
                         "-DCMAKE_BUILD_TYPE=Debug", f"-DCMAKE_CXX_COMPILER={compiler}",
                         f"-DPython3_EXECUTABLE={python}"]
            run(configure)
            # Build a second configuration first so the Debug GCC scan must
            # exclude its counters; LLVM profiles must have distinct locations.
            configs = ("Release", "Debug") if generator.endswith("Multi-Config") else ("Debug",)
            for config in configs:
                command = [args.cmake, "--build", str(build), "--config", config,
                           "--target", "ccov-export", "ccov-report", "--parallel", "4"]
                run(command)
                report = build / "ccov"
                if generator.endswith("Multi-Config"):
                    report /= config
                # Line numbers are stable in Debug; optimized Release can fold lines.
                if config == "Debug":
                    first_counts = inspect_xml(report / "hzcc_test.xml")
                    inspect_xml(report / "smoke_other.xml", other=True)
                first = (report / "hzcc_test.xml").read_text()
                run(command)
                if config == "Debug":
                    assert inspect_xml(report / "hzcc_test.xml") == first_counts, "Stale counters accumulated"
                assert first, "Empty Cobertura report"
            run(configure + ["-DFAIL_COVERAGE_TEST=ON"])
            run([args.cmake, "--build", str(build), "--config", "Debug",
                 "--target", "ccov-export-hzcc_test"], expect_failure=True)
            failed_report = build / "ccov"
            if generator.endswith("Multi-Config"):
                failed_report /= "Debug"
            assert not (failed_report / "hzcc_test.xml").exists(), "Stale XML survived test failure"
            cache = (build / "CMakeCache.txt").read_text()
            tool_variable = "LLVM_COV_PATH" if "LLVM_COV_PATH:FILEPATH=" in cache else "GCOV_PATH"
            # Python is executable but its version is incompatible with either
            # tested C++ compiler: explicit overrides must still be validated.
            run(configure + [f"-D{tool_variable}={python}"], expect_failure=True)
            print(f"PASS {generator}: object coverage, filtering, isolation, repeated runs, "
                  "failure cleanup, incompatible-tool rejection")


if __name__ == "__main__":
    main()
