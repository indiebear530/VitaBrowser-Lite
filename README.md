# VitaBrowser-Lite

A better and modernized web browser for the PlayStation Vita.

## Features
- (Currently a basic graphics demo; full browser features coming soon)
- Faster page loading?
- Better JavaScript support?
- Modern UI?

## Installation
1. Build the project (see **Building** section below)
2. Copy the generated `.vpk` to your Vita
3. Install with VitaShell (or use VHBB)

## Building

**Requirements:**
- [Vita SDK](https://vitasdk.org/) installed
- CMake
- **vita2d library** (install via [vdpm](https://github.com/vitasdk/vdpm#readme) — run `./install-all.sh` after bootstrap)

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake
make -j$(nproc)
