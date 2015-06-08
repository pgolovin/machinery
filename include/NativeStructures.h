/////////////////////////////////////////////////////////////////////////////////
//
// C++ representation of Scriptable structures
//
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <internal/ScriptBase.h>
#include <string>

enumeration Status
{
    OK = 0,
    Pending,
    // Error codes
    ResourceNotFound,
    AlreadyExists,
    ScriptFailure
};

structure Vector
{
    float x;
    float y;
    float z;
};

structure ObjectProperties
{
    std::string name;
    std::string meshName;
    std::string materialName;
    std::string elementName;
};

structure PlacementParameters
{
    std::string name;
    Vector position;
    int orientation;
    int placeDirection;
};