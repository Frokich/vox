/**
 * @file row_operations.cpp
 * @brief Implements row-level text operations for 'vox': cursor mapping, insertion, deletion, tab expansion.
 * 
 * Functions:
 * - `editorCxToRx()` — maps logical cursor to rendered position (tabs = 8 spaces).
 * - `editorUpdateRow()` — rebuilds 'render' string with tabs expanded to spaces.
 * - Row insert/delete — modifies editor state and increments dirty flag.
 * 
 * Note: All operations update E.dirty. Uses std::string for safety. No malloc/free.
 */

#include "row_operations.hpp"
#include "editor.hpp"

namespace vox {

int editorCxToRx(EditorRow& row, int cx) {
    int rx = 0;
    for (int i = 0; i < cx; i++) {
        if (row.chars[i] == '\t')
            rx += (AKVIM_TAB_STOP - 1) - (rx % AKVIM_TAB_STOP);
        rx++;
    }
    return rx;
}

void editorUpdateRow(EditorRow& row) {
    int tabs = 0;
    for (char c : row.chars)
        if (c == '\t') tabs++;

    std::string render;
    render.reserve(row.chars.length() + tabs * (AKVIM_TAB_STOP - 1));

    for (char c : row.chars) {
        if (c == '\t') {
            render += ' ';
            while (render.length() % AKVIM_TAB_STOP != 0)
                render += ' ';
        } else {
            render += c;
        }
    }

    row.render = render;
    row.rsize = render.length();
}

void editorInsertRow(int at, const std::string& s) {
    if (at < 0 || at > E.numrows) return;

    EditorRow newRow;
    newRow.size = s.length();
    newRow.chars = s;
    E.row.insert(E.row.begin() + at, newRow);
    E.numrows = E.row.size();
    editorUpdateRow(E.row[at]);
    E.dirty++;
}

void editorDelRow(int at) {
    if (at < 0 || at >= E.numrows) return;
    E.row.erase(E.row.begin() + at);
    E.numrows = E.row.size();
    E.dirty++;
}

void editorRowInsertChar(EditorRow* row, int at, int c) {
    if (at < 0 || at > (int)row->chars.size())
        at = row->chars.size();
    row->chars.insert(at, 1, (char)c);
    row->size = row->chars.size();
    editorUpdateRow(*row);
    E.dirty++;
}

void editorRowAppendString(EditorRow* row, const std::string& s) {
    row->chars += s;
    row->size = row->chars.size();
    editorUpdateRow(*row);
    E.dirty++;
}

void editorRowDelChar(EditorRow* row, int at) {
    if (at < 0 || at >= (int)row->chars.size()) return;
    row->chars.erase(at, 1);
    row->size = row->chars.size();
    editorUpdateRow(*row);
    E.dirty++;
}

} // namespace vox
