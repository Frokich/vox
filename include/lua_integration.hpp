/**
 * @file lua_integration.hpp
 * @brief Lua 5.4 scripting interface for 'vox' editor.
 * 
 * Functions:
 * - `initLua()` — creates Lua state and binds C++ functions.
 * - `runLuaScript(filename)` — executes Lua file.
 * - `callLuaFunction(name, ...)` — calls global Lua function.
 * 
 * Lua API (global 'vox' table):
 *   vox.save()              — saves current file
 *   vox.quit()              — exits editor
 *   vox.status(msg)         — sets status message
 *   vox.insert(text)        — inserts text at cursor
 *   vox.cursor() -> {x,y}   — returns cursor position
 *   vox.bind_key(key, func) — binds function to key (e.g., "Ctrl+S")
 * 
 * Note: Requires Lua 5.4. No logging. Initialized in main().
 */

#pragma once
#include <string>

struct lua_State;

namespace vox {

void initLua();
void runLuaScript(const std::string& filename);
void callLuaFunction(const char* name);

extern lua_State* L;

} // namespace vox
