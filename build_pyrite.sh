#!/bin/sh
set -eu

# Keep builds off slow network home directories. Only this script's mktemp
# directory is removed; HZCC_BUILD_ROOT itself is never deleted.
source_dir=$(CDPATH= cd "$(dirname "$0")" && pwd -P)
build_root=${HZCC_BUILD_ROOT:-${TMPDIR:-/tmp}}
output_dir=${HZCC_OUTPUT_DIR:-"$source_dir/build/pyrite"}
build_docs=${HZCC_BUILD_DOCUMENTATION:-OFF}
case "$build_docs" in
    ON|on|1|TRUE|true|YES|yes) build_docs=ON ;;
    OFF|off|0|FALSE|false|NO|no) build_docs=OFF ;;
    *) printf '%s\n' 'HZCC_BUILD_DOCUMENTATION must be ON or OFF' >&2; exit 2 ;;
esac

mkdir -p "$build_root"
build_root=$(CDPATH= cd "$build_root" && pwd -P)
build_dir=$(mktemp -d "$build_root/hzcc-build.XXXXXX")
cleanup() {
    rm -rf -- "$build_dir"
}
trap cleanup 0
trap 'exit 130' INT
trap 'exit 143' TERM

# Extra arguments are forwarded to CMake, for example -DCMAKE_BUILD_TYPE=Debug.
cmake -S "$source_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE=Release \
    -DUNIT_TESTING=OFF -DGENERATE_DOCUMENTATION="$build_docs" "$@"
cmake --build "$build_dir" --target hzcc --parallel
if [ "$build_docs" = ON ]; then
    cmake --build "$build_dir" --target documentation --parallel
fi

mkdir -p "$output_dir"
cp "$build_dir/bin/hzcc" "$output_dir/"
if [ "$build_docs" = ON ]; then
    if [ -f "$build_dir/doc/developers.pdf" ]; then
        cp "$build_dir/doc/developers.pdf" "$output_dir/"
    fi
    if [ -d "$build_dir/doxygen_html" ]; then
        mkdir -p "$output_dir/doxygen_html"
        cp -R "$build_dir/doxygen_html/." "$output_dir/doxygen_html/"
    fi
fi
printf 'Build products copied to %s\n' "$output_dir"
