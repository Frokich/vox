#pragma once
#include <string>
#include "config.h"
#include <vector>

class Editor {
public:
    Editor(const std::string& filename);
    void open();
    void save();
    void run();

private:
    std::string filename;
    std::vector<std::string> lines;
    Config config;

    void draw();
    void handleInput();
};

