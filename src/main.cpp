/**
 * @file main.cpp
 * @brief Entry point for 'vox' — minimal Lua-extensible terminal text editor.
 * 
 * Flow:
 * - Enables raw terminal mode
 * - Initializes editor state and Lua
 * - Loads file if provided
 * - Enters main loop: refresh screen → process keypress
 * 
 * Lua: Loads 'scripts/init.lua' on startup.
 * 
 * Note: No logging. Exits cleanly with disableRawMode(). POSIX only.
 */

#include "file_io.hpp"  
#include "editor.hpp"
#include "terminal.hpp"
#include "input.hpp"
#include "lua_integration.hpp"
#include <cstdlib>

using namespace vox;

int main(int argc, char* argv[]) {
    enableRawMode();
    initEditor();
    initLua();

    if (argc >= 2) {
        editorOpen(argv[1]);
    }

    editorSetStatusMessage("HELP: Ctrl-S = save | Ctrl-Q = quit | Lua loaded");

    while (true) {
        editorRefreshScreen();
        editorProcessKeypress();
    }

    return 0;
}
