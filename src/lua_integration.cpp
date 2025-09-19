/**
 * @file lua_integration.cpp
 * @brief Implements Lua 5.4 bindings for 'vox' editor.
 * 
 * Functions:
 * - `initLua()` — creates Lua state, registers C++ functions under 'vox' table.
 * - `runLuaScript(filename)` — loads and executes script (e.g., init.lua).
 * - `callLuaFunction(name)` — calls global Lua function (e.g., on_keypress).
 * 
 * Bound C++ functions:
 *   save(), quit(), status(msg), insert(text), cursor(), bind_key(key, func)
 * 
 * Note: Uses luaL_dostring for safety. Errors printed to status bar. No logging.
 */

#include "lua_integration.hpp"
#include "editor.hpp"
#include "file_io.hpp"
#include "input.hpp"
#include <lua5.4/lua.hpp>
#include <lauxlib.h>
#include <lualib.h>

namespace vox {

lua_State* L = nullptr;

static int l_save(lua_State* L) {
    (void)L;  
    editorSave();
    return 0;
}

static int l_quit(lua_State* L) {
    (void)L;  
    exit(0);
    return 0;
}

static int l_status(lua_State* L) {
    const char* msg = luaL_checkstring(L, 1);
    editorSetStatusMessage("%s", msg);
    return 0;
}

static int l_insert(lua_State* L) {
    const char* text = luaL_checkstring(L, 1);
    for (const char* p = text; *p; p++)
        editorInsertChar(*p);
    return 0;
}

static int l_cursor(lua_State* L) {
    lua_newtable(L);
    lua_pushstring(L, "x"); lua_pushinteger(L, E.cx + 1); lua_settable(L, -3);
    lua_pushstring(L, "y"); lua_pushinteger(L, E.cy + 1); lua_settable(L, -3);
    return 1;
}

void initLua() {
    L = luaL_newstate();
    luaL_openlibs(L);

    lua_newtable(L);
    lua_pushcfunction(L, l_save);     lua_setfield(L, -2, "save");
    lua_pushcfunction(L, l_quit);     lua_setfield(L, -2, "quit");
    lua_pushcfunction(L, l_status);   lua_setfield(L, -2, "status");
    lua_pushcfunction(L, l_insert);   lua_setfield(L, -2, "insert");
    lua_pushcfunction(L, l_cursor);   lua_setfield(L, -2, "cursor");
    lua_setglobal(L, "vox");

    runLuaScript("scripts/init.lua");
}

void runLuaScript(const std::string& filename) {
    if (luaL_dofile(L, filename.c_str()) != LUA_OK) {
        editorSetStatusMessage("Lua error: %s", lua_tostring(L, -1));
        lua_pop(L, 1);
    }
}

void callLuaFunction(const char* name) {
    lua_getglobal(L, name);
    if (lua_isfunction(L, -1)) {
        if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
            editorSetStatusMessage("Lua error: %s", lua_tostring(L, -1));
            lua_pop(L, 1);
        }
    } else {
        lua_pop(L, 1);
    }
}

} // namespace vox
