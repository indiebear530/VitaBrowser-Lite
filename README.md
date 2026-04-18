# VitaBrowser-Lite

A better and modernized web browser for the PlayStation Vita.

## Features
- (List what it does better than the stock browser)
- Faster page loading?
- Better JavaScript support?
- Modern UI?

## Installation
1. Build the project (see Build section below)
2. Copy the generated `.vpk` to your Vita
3. Install with VitaShell (or use VHBB)

## Building
Requirements:
- [Vita SDK](https://vitasdk.org/) installed
- CMake

```bash
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake
make
