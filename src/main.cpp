#include "Editor.h"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        std::cout << "Starting \033[1;36mvox\033[0m editor...\n";
        Editor editor;
        editor.run();
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
