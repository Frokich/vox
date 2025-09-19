/**
 * @file terminal.hpp
 * @brief Terminal control for raw mode, key reading, and screen size detection in 'vox'.
 * 
 * Functions:
 * - `die(s)` — exits with error message and screen reset.
 * - `enableRawMode()` / `disableRawMode()` — switches terminal to raw input mode.
 * - `editorReadKey()` — reads and decodes keypresses (arrows, home, del, etc).
 * - `getWindowSize(rows, cols)` — queries terminal dimensions via ioctl.
 * 
 * Note: POSIX-specific (termios, ioctl). No logging. Used by editor and input modules.
 */

#pragma once

#include <termios.h>

namespace vox {

#define CTRL_KEY(k) ((k) & 0x1f)

enum editorKey {
    BACKSPACE = 127,
    ARROW_LEFT = 1000,
    ARROW_RIGHT,
    ARROW_UP,
    ARROW_DOWN,
    PAGE_UP,
    DEL_KEY,
    PAGE_DOWN,
    HOME_KEY,
    END_KEY
};

[[noreturn]] void die(const char* s);
void disableRawMode();
void enableRawMode();
int editorReadKey();
int getWindowSize(int* rows, int* cols);

} // namespace vox
