/**
 * @file main.cpp
 * @brief Entry point for 'vox' — minimal Lua-extensible terminal text editor, now using ncurses for stable rendering.
 * 
 * Flow:
 * - Initializes ncurses terminal
 * - Sets up editor state and Lua bindings
 * - Loads file if provided
 * - Enters main loop: refresh screen → process keypress
 * 
 * Lua: Loads 'scripts/init.lua' on startup. All Lua functions (save, quit, insert, etc) work unchanged.
 * 
 * Note: No logging. External behavior identical to ANSI version. Cursor is always visible and correctly positioned.
 */

#include "editor.hpp"
#include "terminal.hpp"
#include "input.hpp"
#include "file_io.hpp"
#include "lua_integration.hpp"

using namespace vox;

int main(int argc, char* argv[]) {
    initTerminal();
    initEditor();
    initLua();

    if (argc >= 2) {
        editorOpen(argv[1]);
    }

    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Lua loaded");

    while (true) {
        refreshTerminal();
        editorProcessKeypress();
    }

    cleanupTerminal();
    return 0;
}
