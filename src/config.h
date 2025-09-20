#pragma once
#include <string>

struct Config {
    bool line_number = true;
    bool expandTab = true;
    int tabWidth = 4;

    void load(const std::string& path);
};

