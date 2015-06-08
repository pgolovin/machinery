/////////////////////////////////////////////////////////////////////////////////
//
// Script definitions required for parser
//
/////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <lua.hpp>
#include <assert.h>
#include <string>

///////////////////////////////////////////////////////////////////
// ERRORS
///////////////////////////////////////////////////////////////////
inline void RaiseError(lua_State* L, const std::string& message)
{
    lua_pushstring(L, message.c_str());
    lua_error(L);
}

///////////////////////////////////////////////////////////////////
// DEFAULT PARSERS
///////////////////////////////////////////////////////////////////
inline void ParseObject(lua_State* L, int index, int& value)
{
    luaL_checktype(L, index, LUA_TNUMBER);
    value = lua_tointeger(L, index);
}

inline void ParseObject(lua_State* L, int index, unsigned int& value)
{
    luaL_checktype(L, index, LUA_TNUMBER);
    value = lua_tointeger(L, index);
}

inline void ParseObject(lua_State* L, int index, bool& value)
{
    luaL_checktype(L, index, LUA_TBOOLEAN);
    value = !!lua_toboolean(L, index);
}

inline void ParseObject(lua_State* L, int index, double& value)
{
    luaL_checktype(L, index, LUA_TNUMBER);
    value = lua_tonumber(L, index);
}

inline void ParseObject(lua_State* L, int index, float& value)
{
    luaL_checktype(L, index, LUA_TNUMBER);
    value = (float)lua_tonumber(L, index);
}

inline void ParseObject(lua_State* L, int index, std::string& value)
{
    luaL_checktype(L, index, LUA_TSTRING);
    value = lua_tostring(L, index);
}

///////////////////////////////////////////////////////////////////
// DEFAULT PUSH FUNCTIONS
///////////////////////////////////////////////////////////////////
inline void PushObject(lua_State* L, const std::string& objectName, int value)
{
    lua_pushinteger(L, value);
    if (objectName != "")
        lua_setfield(L, -2, objectName.c_str());
}

inline void PushObject(lua_State* L, const std::string& objectName, unsigned int value)
{
    lua_pushinteger(L, value);
    if (objectName != "")
        lua_setfield(L, -2, objectName.c_str());
}

inline void PushObject(lua_State* L, const std::string& objectName, bool value)
{
    lua_pushboolean(L, (int)value);
    if (objectName != "")
        lua_setfield(L, -2, objectName.c_str());
}

inline void PushObject(lua_State* L, const std::string& objectName, double value)
{
    lua_pushnumber(L, value);
    if (objectName != "")
        lua_setfield(L, -2, objectName.c_str());
}

inline void PushObject(lua_State* L, const std::string& objectName, float value)
{
    lua_pushnumber(L, value);
    if (objectName != "")
        lua_setfield(L, -2, objectName.c_str());
}

inline void PushObject(lua_State* L, const std::string& objectName, const std::string& value)
{
    lua_pushstring(L, value.c_str());
    if (objectName != "")
        lua_setfield(L, -2, objectName.c_str());
}

///////////////////////////////////////////////////////////////////
// Registration for global objects
///////////////////////////////////////////////////////////////////
inline void SetGlobalObject(lua_State* L, const std::string& objectName)
{
    if (objectName == "")
    {
        RaiseError(L, "Unnamed global objects are not allowed");
        return;
    }
    lua_setglobal(L, objectName.c_str());
}

