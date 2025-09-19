/**
 * @file file_io.hpp
 * @brief File loading and saving for 'vox' editor.
 * 
 * Functions:
 * - `editorOpen(filename)` — loads file into editor rows.
 * - `editorSave()` — saves current buffer to file (prompts if unnamed).
 * - `editorRowsToString()` — serializes all rows to single string with newlines.
 * 
 * Note: Uses fstream and fcntl. Modifies E.dirty and E.filename. No logging.
 */

#pragma once
#include <string>

namespace vox {

void editorOpen(const std::string& filename);
void editorSave();
std::string editorRowsToString();

} // namespace vox
