/**
 * @file file_io.cpp
 * @brief Implements file loading and saving for 'vox'.
 * 
 * Functions:
 * - `editorOpen(filename)` — reads file line-by-line, strips \r\n, inserts rows.
 * - `editorSave()` — prompts for filename if needed, writes buffer with ftruncate + write.
 * - `editorRowsToString()` — returns std::string with all rows joined by \n.
 * 
 * Note: Uses low-level I/O (open, write, ftruncate) for atomicity. No logging.
 */

#include "file_io.hpp"
#include "row_operations.hpp"   
#include "editor.hpp"
#include "input.hpp"
#include <fstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>

namespace vox {

void editorOpen(const std::string& filename) {
    E.filename = filename;
    std::ifstream file(filename);
    std::string line;

    while (std::getline(file, line)) {
        // Strip \r (for Windows)
        if (!line.empty() && line.back() == '\r')
            line.pop_back();
        editorInsertRow(E.numrows, line);
    }
    E.dirty = 0;
}

std::string editorRowsToString() {
    std::string buf;
    for (int j = 0; j < E.numrows; j++) {
        buf.append(E.row[j].chars);
        buf += '\n';
    }
    return buf;
}

void editorSave() {
    if (E.filename.empty()) {
        E.filename = editorPrompt("(Esc to cancel) Save as: ");
        if (E.filename.empty()) {
            editorSetStatusMessage("Save aborted.");
            return;
        }
    }

    std::string buf = editorRowsToString();
    int fd = open(E.filename.c_str(), O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        editorSetStatusMessage("I/O error: %s", strerror(errno));
        return;
    }

    if (ftruncate(fd, buf.size()) != -1 && write(fd, buf.c_str(), buf.size()) == (ssize_t)buf.size()) {
        close(fd);
        E.dirty = 0;
        editorSetStatusMessage("%zu bytes written to disk", buf.size());
        return;
    }

    close(fd);
    editorSetStatusMessage("I/O error: %s", strerror(errno));
}

} // namespace vox
