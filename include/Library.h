/////////////////////////////////////////////////////////////////////////////////
//
// class ObjectConstructor generates object from primitives and modules. 
//    the class doesn't generate mesh
//    
/////////////////////////////////////////////////////////////////////////////////
#pragma once

#include <vector>
#include "Resources.h"
#include "Construction.h"
#include "LibraryPrimitives.h"

/////////////////////////////////////////////////////////////////////
///
/// Resource library public interface. object is a singletone
///
/////////////////////////////////////////////////////////////////////

structure ILibrary
{
    metamethod void Reset() = 0;
    metamethod Status NewObject(ObjectProperties& object) = 0;

    virtual const ConstructionDescription* GetConstructionByName(std::string name) = 0;
    virtual Status RegisterConstruction(std::string name, IConstructablePtr& element) = 0;

    virtual const ILibraryMesh& GetMesh(uint32_t id) = 0;
    virtual void RegisterMesh(uint32_t id, ILibraryMeshPtr& mesh) = 0;

    virtual Status CheckObjectStatus(std::string name) = 0;
    virtual const IConstructorObject* GetObjectByName(std::string name) = 0;
    virtual Status RegisterObject(std::string name, IConstructorObjectPtr & prototype) = 0;
};
// eof