/**
 * @file input.hpp
 * @brief Input processing and cursor movement for 'vox' editor.
 * 
 * Functions:
 * - `editorMoveCursor(key)` — moves cursor by arrow/home/end keys.
 * - `editorProcessKeypress()` — handles all user input: insert, delete, save, quit.
 * - `editorInsertChar(c)` — inserts character at cursor.
 * - `editorInsertNewLine()` — splits line at cursor.
 * - `editorDelChar()` — deletes character left of cursor.
 * 
 * Note: Modifies editor state and E.dirty. Uses editorRow* operations. No logging.
 */

#pragma once


namespace vox {

void editorMoveCursor(int key);
void editorProcessKeypress();
void editorInsertChar(int c);
void editorInsertNewLine();
void editorDelChar();

} // namespace vox
