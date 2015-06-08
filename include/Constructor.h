/////////////////////////////////////////////////////////////////////////////////
//
// class ObjectConstructor generates object from primitives and modules. 
//    the class doesn't generate mesh
//    
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <internal\ScriptBase.h>
#include <Resources.h>
#include <Construction.h>
#include <Library.h>
//#include <../Engine/include/ObjectImpl.h>
#include "MathBasics.h"
#include <vector>
#include <cstdint>

/////////////////////////////////////////////////////////////////////
///
/// Constructor public interface
///
/////////////////////////////////////////////////////////////////////
structure Constructor// : public Object
{
    // script published methods
    metamethod void     NewConstruction() = 0;
    metamethod Status PlaceObject(PlacementParameters& parameters) = 0;
    metamethod bool     Weld(uint32_t group1, uint32_t group2) = 0;
    metamethod void     Reset() = 0;

    // native methods
    virtual bool        SetElement(ElementType type, const vector3i_t& position, Directions orientation, Directions copySettingsFrom = Directions::nY) = 0; // to be depricated
    
    virtual uint32_t    GetGroup(const vector3i_t& position) = 0;
    virtual IMesh&      GetMesh() = 0;
    virtual BBox        GetBoundingBox() const = 0;
    virtual ILibrary&   GetLibrary() = 0;

    static Constructor* Create();
	static void			Destroy(Constructor* constructor);
};

// eof