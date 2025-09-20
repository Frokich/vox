#include "editor.h"

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    Editor editor(argv[1]);
    editor.open();
    editor.run();
    return 0;
}

