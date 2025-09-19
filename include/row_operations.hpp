/**
 * @file row_operations.hpp
 * @brief Row-level text manipulation for 'vox': cursor mapping, insertion, deletion, tab expansion.
 * 
 * Functions:
 * - `editorCxToRx(row, cx)` — converts logical cursor X to rendered X (tabs expanded).
 * - `editorUpdateRow(row)` — rebuilds render cache with expanded tabs.
 * - `editorInsertRow(at, s)` — inserts new row at position.
 * - `editorDelRow(at)` — deletes row.
 * - `editorRowInsertChar(row, at, c)` — inserts char in row.
 * - `editorRowDelChar(row, at)` — deletes char in row.
 * 
 * Note: Modifies E.dirty flag. Used by editor and input modules. No logging.
 */

#pragma once
#include "editor.hpp"

namespace vox {

int editorCxToRx(EditorRow& row, int cx);
void editorUpdateRow(EditorRow& row);
void editorInsertRow(int at, const std::string& s);
void editorDelRow(int at);
void editorRowInsertChar(EditorRow* row, int at, int c);
void editorRowAppendString(EditorRow* row, const std::string& s);
void editorRowDelChar(EditorRow* row, int at);

} // namespace vox
