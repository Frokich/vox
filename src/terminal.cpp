/**
 * @file terminal.cpp
 * @brief Implements terminal control functions for 'vox': raw mode, key input, screen size.
 * 
 * Functions:
 * - `die(s)` — resets terminal and exits with perror.
 * - `enableRawMode()` — configures termios for raw input (no echo, no canonical).
 * - `editorReadKey()` — handles escape sequences for arrows, home, del, etc.
 * - `getWindowSize()` — uses TIOCGWINSZ ioctl to get terminal size.
 * 
 * Note: All functions are POSIX-specific. No logging. Safe cleanup via atexit.
 */

#include "terminal.hpp"
#include "editor.hpp"
#include <unistd.h>
#include <sys/ioctl.h>
#include <cstring>
#include <cstdlib>
#include <cerrno>
#include <csignal>

namespace vox {

void die(const char* s) {
    write(STDOUT_FILENO, "\x1b[2j", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.orig_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);

    termios raw = E.orig_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | IXON | INPCK | ISTRIP);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= CS8;
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 1;
    raw.c_cc[VTIME] = 0;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

int editorReadKey() {
    char c;
    while (read(STDIN_FILENO, &c, 1) != 1) {
        if (errno != EAGAIN) die("read");
    }

    if (c == '\x1b') {
        char seq[3];
        if (read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
        if (read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
                if (seq[2] == '~') {
                    switch (seq[1]) {
                        case '1': case '7': return HOME_KEY;
                        case '4': case '8': return END_KEY;
                        case '3': return DEL_KEY;
                        case '5': return PAGE_UP;
                        case '6': return PAGE_DOWN;
                    }
                }
            } else {
                switch (seq[1]) {
                    case 'A': return ARROW_UP;
                    case 'B': return ARROW_DOWN;
                    case 'C': return ARROW_RIGHT;
                    case 'D': return ARROW_LEFT;
                    case 'H': return HOME_KEY;
                    case 'F': return END_KEY;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
                case 'H': return HOME_KEY;
                case 'F': return END_KEY;
            }
        }
        return '\x1b';
    }
    return c;
}

int getWindowSize(int* rows, int* cols) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0)
        return -1;
    *cols = ws.ws_col;
    *rows = ws.ws_row;
    return 0;
}

} // namespace vox
