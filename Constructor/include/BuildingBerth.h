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

#include "Constructor.h"
#include "LibraryImpl.h"
#include "ObjectConstructor.h"
#include "HullConstructor.h"

struct IMesh;

namespace ConstructorImpl
{
///////////////////////////////////////////////////////////////////////////////////////////////
//
// primary building berth.
//
///////////////////////////////////////////////////////////////////////////////////////////////
    class BuildingBerth : public Constructor
    {
    public:
        BuildingBerth();
        ~BuildingBerth() {};

        ///////////////////////////////////////////////////////////////////////////////////
        // Constructor interface

        ///////////////////////////////////////////////////////////////////////////////////
        // Reset content of all constructor elements
        virtual void        NewConstruction() { m_core.Reset(); };

        ///////////////////////////////////////////////////////////////////////////////////
        // Attaches element to construction
        virtual bool        SetElement(ElementType type, const vector3i_t& position, Directions direction, Directions copySettingsFrom = Directions::nY);

///////////////////////////////////////////////////////////////////////////////////////////////
//
// Construction assembly.
//
///////////////////////////////////////////////////////////////////////////////////////////////

        ///////////////////////////////////////////////////////////////////////////////////
        // Add construction object into building berth
        virtual Status      PlaceObject(PlacementParameters& parameters);

        ///////////////////////////////////////////////////////////////////////////////////
        // Welds two groups into single one
        virtual bool        Weld(uint32_t group1, uint32_t group2);

        ///////////////////////////////////////////////////////////////////////////////////
        // Returns group of element at position, or -1 if no element is there
        virtual uint32_t    GetGroup(const vector3i_t& position);

        ///////////////////////////////////////////////////////////////////////////////////
        // Attaches element to construction
        virtual IMesh&      GetMesh();

        ///////////////////////////////////////////////////////////////////////////////////
        // Return bounding box of constructed object
        virtual BBox        GetBoundingBox() const;

        ///////////////////////////////////////////////////////////////////////////////////
        // Return reference to library object
        virtual ILibrary&   GetLibrary();

        ///////////////////////////////////////////////////////////////////////////////////
        // Invalidates all content of Building berth including bound libraries
        // set building berth to state as if it was just constructed
        void        Reset();

        ///////////////////////////////////////////////////////////////////////////////////
        // Return reference to logic constructor object
        Core&  GetCore();

        ///////////////////////////////////////////////////////////////////////////////////
        // Object
        // virtual IMeshPtr        GetMesh() const {return m_mesh;}
        // virtual const GLMeshDescs& GetMeshDescs() const override;

    private:
        Library     m_buildingBlocksLibrary;
        Core        m_core;
        Hull        m_hull;

        PREVENT_COPY(BuildingBerth);
    };

}//end  of namespace constructor

// eof