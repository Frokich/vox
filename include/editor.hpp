/**
 * @file editor.hpp
 * @brief Declares core editor state and rendering interface for 'vox' — a minimal, Lua-extensible terminal text editor.
 * 
 * Classes:
 * - `EditorRow` — represents a single line of text with render cache for tabs.
 * - `EditorState` — holds cursor, scroll offset, rows, filename, and dirty flag.
 * 
 * Functions:
 * - `initEditor()` — initializes terminal dimensions and editor state.
 * - `editorRefreshScreen()` — triggers full screen redraw.
 * - `editorSetStatusMessage(fmt, ...)` — sets temporary status message (auto-expires in 5s).
 * - `editorPrompt(prompt)` — shows input prompt at status bar, returns user string.
 * 
 * Lua integration:
 *   EditorState is bound as global 'vox' in Lua VM with methods:
 *     save(), quit(), cursor(), insert(text), bind_key(key, func)
 * 
 * Note: Uses ANSI escape sequences for rendering. No logging. Depends on terminal.hpp, row_operations.hpp.
 */

#pragma once
#define AKVIM_TAB_STOP 8
#include <string>
#include <vector>
#include <ctime>
#include <termios.h>

namespace vox {

struct EditorRow {
    int size;      // original character count
    int rsize;     // rendered size (after expanding tabs)
    std::string render;  // cached rendered string with tabs expanded
    std::string chars;   // actual content
};

struct EditorState {
    int cx, cy;        // cursor x, y (logical)
    int rx;            // rendered cursor x (after tab expansion)
    int rowoff;        // row scroll offset
    int coloff;        // column scroll offset
    int screenrows;    // visible rows (excluding status bar)
    int screencols;    // visible columns
    int numrows;       // total rows in file
    int dirty;         // modified flag
    std::string filename;
    std::string statusmsg;
    time_t statusmsg_time;
    std::vector<EditorRow> row;
    termios orig_termios;
};

extern EditorState E;

void initEditor();
void editorRefreshScreen();
void editorScroll();
void editorSetStatusMessage(const char* fmt, ...);
std::string editorPrompt(const std::string& prompt);

} // namespace vox
