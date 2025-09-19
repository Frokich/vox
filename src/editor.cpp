/**
 * @file editor.cpp
 * @brief Implements core editor state and screen rendering for 'vox'.
 * 
 * Functions:
 * - `initEditor()` — queries terminal size, sets initial state.
 * - `editorRefreshScreen()` — draws rows, status bar, message bar, positions cursor.
 * - `editorDrawRows()` — renders visible text rows with tildes for empty lines.
 * - `editorStatusBar()` — displays filename, line count, and modified flag.
 * - `editorDrawMessageBar()` — shows temporary messages (5s timeout).
 * 
 * Lua bindings:
 *   vox.refresh() — forces redraw
 *   vox.status(msg) — updates status bar
 * 
 * Note: All output uses raw ANSI escape sequences. No logging.
 */

#include <cstdarg>       
#include "row_operations.hpp"  
#include "editor.hpp"
#include "terminal.hpp"
#include "row_operations.hpp"
#include <sstream>
#include <cstring>
#include <unistd.h>

namespace vox {

EditorState E;

void initEditor() {
    E.cx = 0;
    E.cy = 0;
    E.rx = 0;
    E.numrows = 0;
    E.rowoff = 0;
    E.coloff = 0;
    E.dirty = 0;
    E.statusmsg_time = 0;

    getmaxyx(stdscr, E.screenrows, E.screencols);
    E.screenrows -= 2; // Reserve for status + message bar
}

void editorDrawRows(std::string& ab) {
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
                    ab += "~";
                    padding--;
                }
                ab.append(padding, ' ');
                ab.append(welcome, 0, welen);
            } else {
                ab += "~";
            }
        } else {
            int len = E.row[filerow].render.length() - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;
            ab.append(E.row[filerow].render, E.coloff, len);
        }
        ab += "\x1b[K\r\n";  // \r\n — перевод строки
    }
}

void editorStatusBar(std::string& ab) {
    ab += "\x1b[7m";
    std::string fname = E.filename.empty() ? "[No Name]" : E.filename;
    if (fname.length() > 20) fname = fname.substr(0, 20);
    std::ostringstream oss;
    oss << fname << " - " << E.numrows << " lines" << (E.dirty ? " (modified)" : "");
    std::string status = oss.str();
    if (static_cast<int>(status.length()) > E.screencols)
        status = status.substr(0, E.screencols);

    ab += status;
    int padding = E.screencols - static_cast<int>(status.length());

    std::ostringstream right;
    right << (E.cy + 1);
    std::string rstatus = right.str();

    if (padding > 0) {
        if (padding >= static_cast<int>(rstatus.length())) {
            ab.append(padding - static_cast<int>(rstatus.length()), ' ');
            ab += rstatus;
        } else {
            ab.append(padding, ' ');
        }
    }
    ab += "\x1b[m\r\n";
}

void editorDrawMessageBar(std::string& ab) {
    ab += "\x1b[K";
    if (!E.statusmsg.empty() && time(nullptr) - E.statusmsg_time < 5) {
        int msglen = E.statusmsg.length();
        if (msglen > E.screencols) msglen = E.screencols;
        ab.append(E.statusmsg, 0, msglen);
    }
    ab += "\r\n";
}

void editorRefreshScreen() {
    editorScroll();
    std::string ab;

    ab += "\x1b[H";

    for (int y = 0; y < E.screenrows; y++) {
        int filerow = y + E.rowoff;
        if (filerow >= E.numrows) {
            ab += "~\x1b[K\r\n";
        } else {
            int len = E.row[filerow].render.length() - E.coloff;
            if (len < 0) len = 0;
            if (len > E.screencols) len = E.screencols;
            if (len > 0)
                ab.append(E.row[filerow].render, E.coloff, len);
            ab += "\x1b[K\r\n";
        }
    }

    ab += "\x1b[7m";
    ab += E.filename.empty() ? "[No Name]" : E.filename.substr(0, 20);
    ab += " - ";
    ab += std::to_string(E.numrows);
    ab += " lines";
    if (E.dirty) ab += " (modified)";
    ab += "\x1b[m\r\n";

    ab += "\x1b[K";
    if (!E.statusmsg.empty() && time(nullptr) - E.statusmsg_time < 5)
        ab += E.statusmsg;
    ab += "\r\n";

    int cy = E.cy - E.rowoff + 1;
    int cx = E.rx - E.coloff + 1;
    if (cy < 1) cy = 1;
    if (cy > E.screenrows) cy = E.screenrows;
    if (cx < 1) cx = 1;
    if (cx > E.screencols) cx = E.screencols;
    ab += "\x1b[" + std::to_string(cy) + ";" + std::to_string(cx) + "H";

    write(STDOUT_FILENO, ab.c_str(), ab.length());
}

void editorScroll() {
    if (E.cy >= 0 && E.cy < E.numrows) {
        E.rx = editorCxToRx(E.row[E.cy], E.cx);
    } else {
        E.rx = E.cx;
    }

    if (E.cy < E.rowoff) {
        E.rowoff = E.cy;
    }
    if (E.cy >= E.rowoff + E.screenrows) {
        E.rowoff = E.cy - E.screenrows + 1;
    }

    if (E.rx < E.coloff) {
        E.coloff = E.rx;
    }
    if (E.rx >= E.coloff + E.screencols) {
        E.coloff = E.rx - E.screencols + 1;
    }
}

void editorSetStatusMessage(const char* fmt, ...) {
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    E.statusmsg = buf;
    E.statusmsg_time = time(nullptr);
}

std::string editorPrompt(const std::string& prompt) {
    std::string buf;
    while (true) {
        editorSetStatusMessage("%s%s", prompt.c_str(), buf.c_str());
        editorRefreshScreen();

        int c = editorReadKey();
        if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE) {
            if (!buf.empty()) buf.pop_back();
        } else if (c == '\x1b') {
            editorSetStatusMessage("");
            return "";
        } else if (c == '\r') {
            if (!buf.empty()) {
                editorSetStatusMessage("");
                return buf;
            }
        } else if (!iscntrl(c) && c < 128) {
            buf.push_back(static_cast<char>(c));
        }
    }
}

} // namespace vox
