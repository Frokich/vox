# vox — Minimal Terminal Text Editor with Lua Scripting

A tiny, efficient terminal-based text editor inspired by `kilo`, now with **Lua 5.4** extensibility.

## Features

- Cursor movement, line editing, save/load
- Syntax-free, no dependencies beyond Lua and POSIX
- Lua scripting: automate tasks, bind keys, extend UI
- <2000 lines of clean C++17

## Build

```bash
mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make
./vox [filename]
