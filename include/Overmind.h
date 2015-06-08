/////////////////////////////////////////////////////////////////////////////////
//
// class ObjectConstructor generates object from primitives and modules. 
//    the class doesn't generate mesh
//
// the class is responcible for:
//    - create element based structure
//    - split structure on separate solid modules
//    - create skeleton for object
//
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <internal\ScriptBase.h>
#include <NativeStructures.h>
#include "common.h"
#include <string>

struct Constructor;
struct ILibrary;

struct Overmind
{
    static Overmind& Get();
    ///////////////////////////////////////////////////////////////////////////////////
    // executes given script
    virtual Status ExecuteScript(std::string script) = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    // return last error message
    // if string is empty, no errors happened
    virtual std::string GetLastError() = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    // Access to modules
    virtual Constructor& GetConstructor() = 0;

    ///////////////////////////////////////////////////////////////////////////////////
    // cleanup everything in overmind
    virtual void Amnesia() = 0;

    virtual ~Overmind() {};
};//end  of namespace constructor

// eof
