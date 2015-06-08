#include "OvermindImpl.h"
#include "Constructor.h"
#include "lua.hpp"
//#include <ScriptEngine_gen.h>

using namespace OvermindImpl;

///////////////////////////////////////////////////////////////////////////////////////////////
//
// MACRO MADNESS
//
///////////////////////////////////////////////////////////////////////////////////////////////

#define INTERFACE_FUNCTION(InterfaceName, FunctionName)                                       \
    lua_pushcfunction(m_lua, LUA_##InterfaceName##FunctionName);                              \
    lua_setfield(m_lua, -2, #FunctionName)

#define STRUCTURE_FIELD(value)                                                                \
    lua_pushinteger(m_lua, value);                                                            \
    lua_setfield(m_lua, -2, #value)

#define LUA_CHECK_ARGUMENTS(count)                                                            \
    if (lua_gettop(L) > count)                                                                \
        return luaL_error(L, "invalid arguments count, only " #count " expected")

#define LUA_GETGLOBAL(type, object)                                                           \
    lua_getfield(L, LUA_REGISTRYINDEX, "_" #object);                                          \
    type* object = (type*)lua_touserdata(L, -1);                                              \
    lua_pop(L, 1);                                                                            \
    if (!object)                                                                              \
        return luaL_error(L, "PANIC: " #object " interface not found")

#define LUA_VERIFY(func) if (int err = (func)){ return err;}

///////////////////////////////////////////////////////////////////////////////////////////////
//
// LUA reflections. 
// Generate parsers for all structures defined in reflections.h 
//
///////////////////////////////////////////////////////////////////////////////////////////////

#include "ScriptEngine_gen.h"

///////////////////////////////////////////////////////////////////////////////////////////////
//
// OVERMIND itself
//
///////////////////////////////////////////////////////////////////////////////////////////////

Overmind& Overmind::Get()
{
    static unique_ptr<Overmind> overmind(new OvermindCerebro);
    return *(overmind.get());
}

OvermindCerebro::OvermindCerebro()
    : m_lua(luaL_newstate())
    , m_constructor(*Constructor::Create())
{
    luaL_openlibs(m_lua);

    registerGlobals();
}

OvermindCerebro::~OvermindCerebro()
{
    lua_close(m_lua);
	Constructor::Destroy(&m_constructor);
}

Status OvermindCerebro::ExecuteScript(std::string filename)
{
    if (luaL_dofile(m_lua, filename.c_str()))
    {
        m_errorMessages.push_back(lua_tostring(m_lua, -1));
        return Status::ScriptFailure;
    }
    return Status::OK;
}

std::string OvermindCerebro::GetLastError()
{
    if (m_errorMessages.empty())
        return "";

    std::string errorString = m_errorMessages.back();
    m_errorMessages.pop_back();
    return errorString;
}

void OvermindCerebro::registerGlobals()
{
    RegisterGlobals(m_lua);
    PushObject(m_lua, "", m_constructor);
    SetGlobalObject(m_lua, "Constructor");

    PushObject(m_lua, "", m_constructor.GetLibrary());
    SetGlobalObject(m_lua, "Library");
}


// eof