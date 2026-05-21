#!/usr/bin/env sh

set -eu

die()
{
    printf '%s\n' "$*" >&2
    exit 1
}

need_cmd()
{
    command -v "$1" >/dev/null 2>&1 || die "missing required command: $1"
}

need_pkg()
{
    pkg-config --exists "$1" || die "missing pkg-config package: $1"
}

run_matrix()
{
    name=$1
    shift

    printf '\n== %s ==\n' "$name"
    ./configure --enable-tests --enable-macos-frameworks=yes "$@"
    make
    make check
}

need_cmd pkg-config
need_cmd make
need_cmd xcode-select

xcode-select -p >/dev/null 2>&1 || die "Xcode Command Line Tools are not configured"
need_pkg check
need_pkg cmocka

./bootstrap

run_matrix "macOS frameworks with X11" "$@"
make distclean

run_matrix "macOS frameworks without X11" --disable-x11 "$@"
make distclean

if pkg-config --exists freerdp; then
    run_matrix "NeutrinoRDP with macOS frameworks" --enable-neutrinordp "$@"
elif pkg-config --exists freerdp5; then
    printf '\n== NeutrinoRDP with macOS frameworks ==\n'
    printf '%s\n' "skipped: freerdp5 is installed, but this module requires legacy freerdp.pc"
    if [ "${RUN_NEUTRINORDP:-0}" = "1" ]; then
        die "RUN_NEUTRINORDP=1 set, but only incompatible freerdp5 was found"
    fi
elif [ "${RUN_NEUTRINORDP:-0}" = "1" ]; then
    die "RUN_NEUTRINORDP=1 set, but pkg-config cannot find freerdp"
else
    printf '\n== NeutrinoRDP with macOS frameworks ==\n'
    printf '%s\n' "skipped: pkg-config cannot find freerdp"
fi
