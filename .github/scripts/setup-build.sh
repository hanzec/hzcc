#!/usr/bin/env bash
# Shared setup for the Linux containers declared in the GitHub workflows.
set -euo pipefail

if [[ "$(uname -s)" != Linux || "$(uname -m)" != x86_64 || "$(id -u)" != 0 ]]; then
    printf '%s\n' 'This setup script requires a root Linux x86_64 CI container.' >&2
    exit 2
fi
case "${HZCC_CI_COMPILER:-}" in
    gcc|clang) ;;
    *) printf '%s\n' 'HZCC_CI_COMPILER must be gcc or clang.' >&2; exit 2 ;;
esac

# Download official release assets and reject anything with a different digest.
fetch_checked() {
    local url=$1 destination=$2 digest=$3
    curl --fail --location --retry 3 --connect-timeout 30 "$url" --output "$destination"
    printf '%s  %s\n' "$digest" "$destination" | sha256sum --check --status
}

export DEBIAN_FRONTEND=noninteractive
apt-get update
apt-get install --yes --no-install-recommends \
    build-essential ca-certificates curl git ninja-build pkg-config \
    autoconf autoconf-archive automake libtool ccache \
    python3 python3-venv tar unzip zip xz-utils zstd \
    libatomic1 libedit2 libtinfo6 libxml2 zlib1g

# Workflows restore this directory before setup, including before Doxygen builds.
# Compiler launchers leave CC/CXX and vcpkg's own binary cache unchanged.
export CCACHE_DIR="${CCACHE_DIR:-$GITHUB_WORKSPACE/.cache/ccache}"
export CCACHE_BASEDIR="${CCACHE_BASEDIR:-$GITHUB_WORKSPACE}"
export CCACHE_COMPILERCHECK=content
export CCACHE_MAXSIZE="${CCACHE_MAXSIZE:-512M}"
mkdir -p "$CCACHE_DIR"
ccache --version
ccache --zero-stats

cmake_prefix=/opt/hzcc-cmake-4.4.5
cmake_archive="$RUNNER_TEMP/cmake-4.4.5-linux-x86_64.sh"
fetch_checked \
    'https://github.com/Kitware/CMake/releases/download/v4.4.5/cmake-4.4.5-linux-x86_64.sh' \
    "$cmake_archive" \
    '19a27f98586816e61167b8abad8cba38a58097cadf58c2e927f9552db9be5027'
mkdir -p "$cmake_prefix"
sh "$cmake_archive" --skip-license --prefix="$cmake_prefix"
rm "$cmake_archive"
export PATH="$cmake_prefix/bin:$PATH"
printf '%s\n' "$cmake_prefix/bin" >> "$GITHUB_PATH"

if [[ "$HZCC_CI_COMPILER" == clang ]]; then
    # apt.llvm.org's release branch also contains unreleased patch snapshots.
    # Use the official stable archive so both the compiler and its runtimes match.
    llvm_prefix=/opt/hzcc-llvm-23.1.5
    llvm_archive="$RUNNER_TEMP/LLVM-23.1.5-Linux-X64.tar.zst"
    fetch_checked \
        'https://github.com/llvm/llvm-project/releases/download/llvmorg-23.1.5/LLVM-23.1.5-Linux-X64.tar.zst' \
        "$llvm_archive" \
        'c3107384835f788768f614db610e87fba7077f125ca950eb02a78bab0a308065'
    mkdir -p "$llvm_prefix"
    # Release archives use a large zstd window. Keep the compiler, coverage tools,
    # headers and runtime libraries instead of unpacking every LLVM utility.
    tar --use-compress-program='zstd --long=31 -d' \
        -xf "$llvm_archive" --strip-components=1 -C "$llvm_prefix" \
        --wildcards --wildcards-match-slash \
        '*/bin/clang*' '*/bin/lld' '*/bin/ld.lld' \
        '*/bin/llvm-ar' '*/bin/llvm-ranlib' \
        '*/bin/llvm-cov' '*/bin/llvm-profdata' '*/bin/llvm-symbolizer' \
        '*/lib/*' '*/include/*'
    rm "$llvm_archive"
    export PATH="$llvm_prefix/bin:$PATH"
    printf '%s\n' "$llvm_prefix/bin" >> "$GITHUB_PATH"
    export CC="$llvm_prefix/bin/clang" CXX="$llvm_prefix/bin/clang++"
    test "$("$CXX" -dumpversion)" = 23.1.5
    test -f "$("$CC" -print-resource-dir)/include/stddef.h"
    # Executing each tool also checks retained symlink targets and shared libraries.
    for llvm_tool in ld.lld llvm-ar llvm-ranlib llvm-cov llvm-profdata llvm-symbolizer; do
        "$llvm_prefix/bin/$llvm_tool" --version
    done
else
    # The official GCC image supplies this exact stable release in /usr/local.
    export CC=/usr/local/bin/gcc CXX=/usr/local/bin/g++
    test "$("$CXX" -dumpfullversion)" = 16.2.0
fi
printf 'CC=%s\nCXX=%s\n' "$CC" "$CXX" >> "$GITHUB_ENV"
"$CC" --version
"$CXX" --version
cmake --version
test "$(cmake --version | sed -n '1s/cmake version //p')" = 4.4.5

# Verify C linking, C++ standard-library discovery and the ASan runtime before
# configuring the project's dependencies with this same compiler pair.
printf '%s\n' 'int main(void) { return 0; }' > "$RUNNER_TEMP/hzcc-toolchain-smoke.c"
printf '%s\n' '#include <string>' \
    'int main() { std::string value = "hzcc"; return value.size() == 4 ? 0 : 1; }' \
    > "$RUNNER_TEMP/hzcc-toolchain-smoke.cc"
"$CC" "$RUNNER_TEMP/hzcc-toolchain-smoke.c" -o "$RUNNER_TEMP/hzcc-c-smoke"
"$CXX" -std=c++17 -fsanitize=address -fno-omit-frame-pointer \
    "$RUNNER_TEMP/hzcc-toolchain-smoke.cc" -o "$RUNNER_TEMP/hzcc-cxx-smoke"
"$RUNNER_TEMP/hzcc-c-smoke"
"$RUNNER_TEMP/hzcc-cxx-smoke"

# Never install pip packages into the distribution-managed interpreter.
python3 -m venv "$RUNNER_TEMP/hzcc-python"
python="$RUNNER_TEMP/hzcc-python/bin/python"
"$python" -m pip install --disable-pip-version-check -r "$GITHUB_WORKSPACE/requirements-test.txt"
if [[ "${HZCC_CI_COVERAGE:-false}" == true ]]; then
    "$python" -m pip install --disable-pip-version-check -r "$GITHUB_WORKSPACE/requirements-coverage.txt"
fi
printf 'HZCC_PYTHON=%s\n' "$python" >> "$GITHUB_ENV"
printf '%s\n' "$RUNNER_TEMP/hzcc-python/bin" >> "$GITHUB_PATH"
mkdir -p "$GITHUB_WORKSPACE/.cache/vcpkg"

if [[ "${HZCC_CI_DOCUMENTATION:-false}" == true ]]; then
    apt-get install --yes --no-install-recommends \
        bison flex graphviz patch texlive-latex-base texlive-latex-recommended \
        texlive-latex-extra texlive-fonts-recommended
    "$python" -m pip install --disable-pip-version-check -r "$GITHUB_WORKSPACE/requirements-docs.txt"

    # Build the official release with the same current GCC toolchain as the job.
    # This avoids relying on an older distribution Doxygen package.
    doxygen_prefix=/opt/hzcc-doxygen-1.18.0
    if [[ ! -x "$doxygen_prefix/bin/doxygen" ]]; then
        doxygen_archive="$RUNNER_TEMP/doxygen-1.18.0.src.tar.gz"
        doxygen_source="$RUNNER_TEMP/doxygen-source"
        doxygen_build="$RUNNER_TEMP/doxygen-build"
        fetch_checked \
            'https://github.com/doxygen/doxygen/releases/download/Release_1_18_0/doxygen-1.18.0.src.tar.gz' \
            "$doxygen_archive" \
            'a1deed70a6785bbec95a2b2a9e419dc7f7b223a9d74a8644ae611c8e2dcdd354'
        mkdir -p "$doxygen_source"
        tar -xzf "$doxygen_archive" --strip-components=1 -C "$doxygen_source"
        rm "$doxygen_archive"
        # Upstream fix for unchecked find() results in the 1.18.0 parser.
        patch --batch --forward --directory "$doxygen_source" --strip=1 \
            --input "$GITHUB_WORKSPACE/cmake/patches/doxygen-1.18.0-find-guards.patch"
        cmake -S "$doxygen_source" -B "$doxygen_build" -G Ninja \
            -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$doxygen_prefix" \
            -DCMAKE_C_COMPILER="$CC" -DCMAKE_CXX_COMPILER="$CXX" \
            -DCMAKE_C_COMPILER_LAUNCHER=ccache -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
            -Dbuild_doc=OFF -Dbuild_wizard=OFF -Duse_libclang=OFF
        cmake --build "$doxygen_build" --parallel 2
        cmake --install "$doxygen_build"
    fi
    "$doxygen_prefix/bin/doxygen" --version
    test "$("$doxygen_prefix/bin/doxygen" --version | awk 'NR == 1 { print $1 }')" = 1.18.0
    printf '%s\n' "$doxygen_prefix/bin" >> "$GITHUB_PATH"
fi
