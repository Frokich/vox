#include "Terminal.h"
#include <iostream>

namespace Terminal {
    void clear() {
#ifdef _WIN32
        system("cls");
#else
        std::cout << "\033[2J\033[1;1H";
#endif
    }

    void moveCursor(int row, int col) {
        std::cout << "\033[" << row+1 << ";" << col+1 << "H";
    }

    void clearLine() {
        std::cout << "\033[2K";
    }
}
