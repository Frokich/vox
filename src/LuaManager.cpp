#include "LuaManager.h"
#include "PluginAPI.h"
#include <iostream>

void LuaManager::initialize(Editor* editor) {
    m_editor = editor;
    m_lua.open_libraries(sol::lib::base, sol::lib::string, sol::lib::package);

    PluginAPI::registerAPI(m_lua, m_editor);

    std::cout << "Lua initialized. Version: " << LUA_VERSION << std::endl;
}

void LuaManager::executeFile(const std::string& filename) {
    try {
        m_lua.script_file(filename);
    } catch (const std::exception& e) {
        std::cerr << "Lua error in " << filename << ": " << e.what() << std::endl;
    }
}

void LuaManager::executeString(const std::string& code) {
    try {
        m_lua.script(code);
    } catch (const std::exception& e) {
        std::cerr << "Lua error: " << e.what() << std::endl;
    }
}
