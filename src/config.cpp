#include "config.h"
#include <lua.hpp>
#include <iostream>

void Config::load(const std::string& path) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    if (luaL_dofile(L, path.c_str()) != LUA_OK) {
        std::cerr << "Failed to load config: " << lua_tostring(L, -1) << "\n";
        lua_pop(L, 1);
        lua_close(L);
        return;
    }

    lua_getglobal(L, "line_number");
    if (lua_isboolean(L, -1)) line_number = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getglobal(L, "expandTab");
    if (lua_isboolean(L, -1)) expandTab = lua_toboolean(L, -1);
    lua_pop(L, 1);

    lua_getglobal(L, "tabWidth");
    if (lua_isinteger(L, -1)) tabWidth = lua_tointeger(L, -1);
    lua_pop(L, 1);

    lua_close(L);
}

