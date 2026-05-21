# xrdp-macos-ard Serena Project Notes

## Purpose

This repository is an xrdp fork with a focus on macOS / Apple Remote
Desktop compatibility and NeutrinoRDP hardening.

## Tech Stack

- C and C++
- Autotools and Make
- Check and CMocka for unit tests
- Homebrew, Xcode, and the macOS SDK for local Darwin builds

## Important Directories

- `common`: shared portability and utility code
- `xrdp`: main xrdp daemon code
- `neutrinordp`: NeutrinoRDP proxy module
- `sesman`: session manager and related helpers
- `libxrdp`: RDP protocol library code
- `libipm`: internal protocol messaging code
- `tests`: unit and build-level tests
- `docs`: documentation and manpage sources

## Working Rules

- Never reboot or restart a computer or server without explicit user
  confirmation.
- Keep changes surgical and limited to the task.
- Read the relevant code before editing it.
- Prefer simple, established local patterns over new abstractions.
- Preserve Linux compatibility while adding macOS-specific support.
- Surface skipped checks, missing dependencies, and uncertainty explicitly.

## Build And Test Commands

- `./bootstrap`
- `./configure --enable-tests`
- `make`
- `make check`
- `./configure --enable-tests --enable-macos-frameworks=yes`
- `./configure --enable-tests --enable-macos-frameworks=yes --disable-x11`
- `./configure --enable-neutrinordp --enable-tests`
  - Use this only when FreeRDP development files are available.

## Current macOS Notes

- `check` and `cmocka` are required for local unit tests.
- macOS framework support is limited to Security/CoreFoundation trust helpers;
  OpenSSL remains the TLS engine.
- `--disable-x11` is available for macOS builds that intentionally omit
  X11-dependent modules and tools.
- Homebrew FreeRDP currently exposes `freerdp5.pc`; the NeutrinoRDP module
  still requires the legacy `freerdp.pc` API unless a FreeRDP 5 port is done.
- macOS-specific test guards exist for platform differences such as missing
  realtime signal macros, file-descriptor scanning behavior, and GNU
  linker-only wrapping flags.

## Git Notes

- Use the local fork remote named `fork` for pushing work.
- Do not push local changes to the original upstream `origin` remote.
