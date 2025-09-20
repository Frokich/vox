# Vox Editor

A minimal Vim-like text editor written in C++ with Lua configuration support.

## Features

- Normal and Insert modes
- Command-line (:w, :q, :e <file>)
- Lua configuration:
  - line_number — show line numbers
  - expandTab — use spaces instead of tabs
  - tabWidth — number of spaces for a tab
- Cursor movement with arrow keys in any mode
- Tab and Enter support
- Undo with u
- Text scrolling
- Basic Vim commands: h/j/k/l, i/a/o/O, x, dd

## Installation

Requirements:

```bash
sudo apt install g++ cmake libncurses5-dev liblua5.3-dev
````

Build:

```bash
mkdir build
cd build
cmake ..
make
```

## Usage

```bash
./vox <filename>
```

### Hotkeys

* i — enter Insert mode
* a — Insert after cursor
* o — new line below, Insert mode
* O — new line above, Insert mode
* h/j/k/l or arrow keys — move cursor
* x — delete character
* dd — delete line
* u — undo
* : — command-line
* \:w — save file
* \:q — quit editor
* \:e <file> — open another file
* Tab — insert spaces (configurable in Lua config)
* F2 — save (optional if implemented)

## Lua Configuration

File: \~/.config/vox/init.lua

```lua
line_number = true
expandTab = true
tabWidth = 2
```
