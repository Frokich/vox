/**
 * @file input.cpp
 * @brief Implements user input handling for 'vox': keypress routing, cursor movement, text editing.
 * 
 * Functions:
 * - `editorProcessKeypress()` — main input loop dispatcher (Ctrl+S, Ctrl+Q, arrows, etc).
 * - `editorMoveCursor()` — moves cursor with bounds checking.
 * - `editorInsertNewLine()` — splits current line at cursor.
 * - `editorDelChar()` — handles backspace/delete with line merging.
 * 
 * Note: Uses global E state. Modifies rows and dirty flag. No logging.
 */

#include "input.hpp"
#include <unistd.h>
#include "editor.hpp"
#include "file_io.hpp"
#include "terminal.hpp"
#include "row_operations.hpp"
#include <cstdlib>

namespace vox {

void editorMoveCursor(int key) {
    EditorRow* row = (E.cy >= E.numrows) ? nullptr : &E.row[E.cy];

    switch (key) {
        case ARROW_LEFT:
            if (E.cx != 0) {
                E.cx--;
            } else if (E.cy > 0) {
                E.cy--;
                E.cx = E.row[E.cy].size;
            }
            break;
        case ARROW_RIGHT:
            if (row && E.cx < row->size) {
                E.cx++;
            } else if (row && E.cx == row->size) {
                E.cy++;
                E.cx = 0;
            }
            break;
        case ARROW_UP:
            if (E.cy != 0) E.cy--;
            break;
        case ARROW_DOWN:
            if (E.cy < E.numrows) E.cy++;
            break;
        case HOME_KEY:
            E.cx = 0;
            break;
        case END_KEY:
            if (E.cy < E.numrows) E.cx = E.row[E.cy].size;
            break;
    }

    row = (E.cy >= E.numrows) ? nullptr : &E.row[E.cy];
    int rowlen = row ? row->size : 0;
    if (E.cx > rowlen) E.cx = rowlen;
}

void editorInsertChar(int c) {
    if (E.cy == E.numrows)
        editorInsertRow(E.numrows, "");
    editorRowInsertChar(&E.row[E.cy], E.cx, c);
    E.cx++;

    fprintf(stderr, "Inserted char '%c' at (%d,%d)\n", c, E.cx, E.cy);
}

void editorInsertNewLine() {
    if (E.cx == 0) {
        editorInsertRow(E.cy, "");
    } else {
        std::string rightPart = E.row[E.cy].chars.substr(E.cx);
        editorInsertRow(E.cy + 1, rightPart);
        E.row[E.cy].chars.erase(E.cx);
        E.row[E.cy].size = E.row[E.cy].chars.size();
        editorUpdateRow(E.row[E.cy]);
    }
    E.cy++;
    E.cx = 0;
}

void editorDelChar() {
    if (E.cy == E.numrows) return;
    if (E.cx == 0 && E.cy == 0) return;

    EditorRow* row = &E.row[E.cy];
    if (E.cx > 0) {
        editorRowDelChar(row, E.cx - 1);
        E.cx--;
    } else {
        E.cx = E.row[E.cy - 1].size;
        editorRowAppendString(&E.row[E.cy - 1], row->chars);
        editorDelRow(E.cy);
        E.cy--;
    }
}

void editorProcessKeypress() {
    static int quit_times = 3;

    int c = editorReadKey();
    switch (c) {
        case '\r':
            editorInsertNewLine();
            break;
        case CTRL_KEY('q'):
            if (E.dirty && quit_times > 0) {
                editorSetStatusMessage("WARNING! File has unsaved changes. Press Ctrl+Q %d more times to quit.", quit_times);
                quit_times--;
                return;
            }
            write(STDOUT_FILENO, "\x1b[2j", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        case CTRL_KEY('s'):
            editorSave();
            break;
        case BACKSPACE:
        case CTRL_KEY('h'):
        case DEL_KEY:
            if (c == DEL_KEY) editorMoveCursor(ARROW_RIGHT);
            editorDelChar();
            break;
        case PAGE_UP:
        case PAGE_DOWN: {
            if (c == PAGE_UP) {
                E.cy = E.rowoff;
            } else {
                E.cy = E.rowoff + E.screenrows - 1;
                if (E.cy > E.numrows) E.cy = E.numrows;
            }
            int times = E.screenrows;
            while (times--) editorMoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
        } break;
        case ARROW_UP:
        case ARROW_DOWN:
        case ARROW_LEFT:
        case ARROW_RIGHT:
        case HOME_KEY:
        case END_KEY:
            editorMoveCursor(c);
            break;
        case CTRL_KEY('l'):
        case '\x1b':
            break;
        default:
            if (!iscntrl(c) && c < 128)
                editorInsertChar(c);
            break;
    }
    quit_times = 3;
}

} // namespace vox
