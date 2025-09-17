#pragma once
#include <sol/sol.hpp>
#include "Editor.h"

class LuaManager {
public:
    static LuaManager& instance() {
        static LuaManager inst;
        return inst;
    }

    void initialize(Editor* editor);
    void executeFile(const std::string& filename);
    void executeString(const std::string& code);
    sol::state& lua() { return m_lua; }

private:
    LuaManager() = default;
    sol::state m_lua;
    Editor* m_editor = nullptr;
};
