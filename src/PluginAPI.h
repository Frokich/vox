#pragma once
#include <sol/sol.hpp>
#include "Editor.h"

class PluginAPI {
public:
    static void registerAPI(sol::state& lua, Editor* editor) {
        auto editor_table = lua.create_named_table("editor");

        editor_table.set_function("get_lines", [editor]() {
            return editor->getLines();
        });

        editor_table.set_function("insert_line", [editor](int index, const std::string& line) {
            editor->insertLine(index, line);
        });

        editor_table.set_function("delete_line", [editor](int index) {
            editor->deleteLine(index);
        });

        editor_table.set_function("replace_line", [editor](int index, const std::string& line) {
            editor->replaceLine(index, line);
        });

        // ✅ ИСПРАВЛЕНО: захватываем &lua
        editor_table.set_function("get_cursor", [&lua, editor]() {
            return sol::make_object(lua, sol::as_table(std::vector<int>{
                editor->getCursorRow(), editor->getCursorCol()
            }));
        });

        editor_table.set_function("set_cursor", [editor](int row, int col) {
            editor->setCursor(row, col);
        });

        editor_table.set_function("bind_command", [editor](const std::string& cmd, sol::protected_function func) {
            editor->bindCommand(cmd, [func]() {
                try { func(); } catch (const std::exception& e) {
                    std::cerr << "Error in Lua command: " << e.what() << std::endl;
                }
            });
        });

        editor_table.set_function("trigger_event", [editor](const std::string& event) {
            editor->triggerEvent(event);
        });

        editor_table.set_function("log", [](const std::string& msg) {
            std::cout << "[Lua] " << msg << std::endl;
        });

        editor_table.set_function("set_status_message", [editor](const std::string& msg) {
            editor->setStatusMessage(msg);
        });

        std::cout << "Plugin API registered with status support." << std::endl;
    }
};
