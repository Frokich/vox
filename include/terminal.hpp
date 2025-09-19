/**
 * @file terminal.hpp
 * @brief Terminal abstraction layer for 'vox' using ncurses — replaces raw ANSI escape code rendering.
 * 
 * Functions:
 * - `initTerminal()` — initializes ncurses, hides cursor initially, enables keypad input.
 * - `cleanupTerminal()` — restores terminal state on exit.
 * - `editorReadKey()` — reads keypress and returns editorKey enum (ARROW_LEFT, DEL_KEY, etc) — same interface as before.
 * - `refreshTerminal()` — full screen redraw using ncurses (replaces editorRefreshScreen).
 * 
 * Lua integration: Not directly exposed, but enables stable UI for Lua-bound functions.
 * 
 * Note: No logging. Replaces all raw ANSI code handling. Depends on ncurses.h. Cursor is always visible and correctly positioned.
 */

#pragma once

#include <curses.h>

namespace vox {

// Перенесены сюда из старого terminal.hpp — чтобы другие файлы видели
#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    DEL_KEY,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY
};

void initTerminal();
void cleanupTerminal();
int editorReadKey();
void refreshTerminal();

} // namespace vox
