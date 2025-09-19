/*
 * 
 * Functions:
 * - `initTerminal()` — calls initscr(), sets cbreak/noecho, enables keypad, makes cursor visible.
 * - `cleanupTerminal()` — calls endwin() to restore terminal.
 * - `editorReadKey()` — maps ncurses KEY_* codes to editorKey enum (e.g. KEY_LEFT → ARROW_LEFT).
 * - `refreshTerminal()` — redraws entire screen: text rows, status bar, message bar, positions cursor via move(y,x).
 * 
 * Lua integration: Provides reliable UI layer — Lua scripts see same behavior as before.
 * 
 * Note: No logging. No raw escape sequences. Cursor is always visible and correctly placed. External behavior identical to ANSI version.
 */

#include "terminal.hpp"
#include "editor.hpp"
#include <unistd.h>
#include <sstream>  

namespace vox {

WINDOW* win = nullptr;

void initTerminal() {
    system("stty -ixon 2>/dev/null");
    win = initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(win, FALSE);
    keypad(win, TRUE);
    curs_set(1);
    start_color();
    clear();
}

void cleanupTerminal() {
    endwin();
}

int editorReadKey() {
    int c = getch();

    switch (c) {
        case KEY_LEFT: return ARROW_LEFT;
        case KEY_RIGHT: return ARROW_RIGHT;
        case KEY_UP: return ARROW_UP;
        case KEY_DOWN: return ARROW_DOWN;
        case KEY_HOME: return HOME_KEY;
        case KEY_END: return END_KEY;
        case KEY_DC: return DEL_KEY;
        case KEY_PPAGE: return PAGE_UP;
        case KEY_NPAGE: return PAGE_DOWN;
        case 263:
        case 127: return BACKSPACE;
        case 27: return '\x1b';
        default: return c;
    }
}

void refreshTerminal() {
    editorScroll();

    clear();

    for (int y = 0; y < E.screenrows; y++) {
        int filerow = y + E.rowoff;

        if (filerow >= E.numrows) {
            if (E.numrows == 0 && y == E.screenrows / 3) {
                std::ostringstream oss;
                oss << "vox editor -- version 0.0.1";
                std::string welcome = oss.str();
                int welen = welcome.length();
                if (welen > E.screencols) welen = E.screencols;
                int padding = (E.screencols - welen) / 2;
                if (padding) {
                    mvaddch(y, 0, '~');
                    mvaddstr(y, 1, std::string(padding - 1, ' ').c_str());
                    mvaddstr(y, padding, welcome.substr(0, welen).c_str());
                } else {
                    mvaddstr(y, 0, welcome.substr(0, welen).c_str());
                }
            } else {
                mvaddch(y, 0, '~');
            }
        } else {
            int len = E.row[filerow].render.length() - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;
            if (len > 0) {
                std::string part = E.row[filerow].render.substr(E.coloff, len);
                mvaddstr(y, 0, part.c_str());
            }
        }
    }

    attron(A_REVERSE);
    std::string fname = E.filename.empty() ? "[No Name]" : E.filename;
    if (fname.length() > 20) fname = fname.substr(0, 20);
    std::ostringstream ossLeft;
    ossLeft << fname << " - " << E.numrows << " lines" << (E.dirty ? " (modified)" : "");
    std::string status = ossLeft.str();
    if (static_cast<int>(status.length()) > E.screencols)
        status = status.substr(0, E.screencols);
    mvaddstr(E.screenrows, 0, status.c_str());

    int padding = E.screencols - static_cast<int>(status.length());
    if (padding > 0) {
        std::ostringstream right;
        right << (E.cy + 1);
        std::string rstatus = right.str();
        if (padding >= static_cast<int>(rstatus.length())) {
            mvaddstr(E.screenrows, E.screencols - rstatus.length(), rstatus.c_str());
        }
    }
    attroff(A_REVERSE);

    if (!E.statusmsg.empty() && time(nullptr) - E.statusmsg_time < 5) {
        int msglen = E.statusmsg.length();
        if (msglen > E.screencols) msglen = E.screencols;
        std::string msg = E.statusmsg.substr(0, msglen);
        mvaddstr(E.screenrows + 1, 0, msg.c_str());
    }

    int cursor_y = E.cy - E.rowoff;
    int cursor_x = E.rx - E.coloff;

    if (cursor_y < 0) cursor_y = 0;
    if (cursor_y >= E.screenrows) cursor_y = E.screenrows - 1;
    if (cursor_x < 0) cursor_x = 0;
    if (cursor_x >= E.screencols) cursor_x = E.screencols - 1;

    move(cursor_y, cursor_x);
    refresh();
}

} // namespace vox
