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

#include "include/QuadTree.h"
#include "include/RangeList.h"
#include "ConstructionLibraryImpl.h"
#include <vector>
#include <list>
#include <memory>

#include "Resources.h"

namespace ConstructorImpl
{
    struct Element
    {
        const ConstructionDescription*  construction;
        unsigned int                    type;
        unsigned int                    direction;
        unsigned int                    originalDirection; //secondary direction required for generated elements
        unsigned int                    neighbourhood;
        unsigned int                    group;
    };

    enum ConstructorElements : size_t
    {
        BaseIndex = 0,
        MeshIndex,
        ComponentsCount,
    };

    typedef Utils::RangeList<Element> Pillar_t;

    class Core : public IConstructable
    {
    public:
        Core(ConstructionLibrary& objectLibrary);
        virtual ~Core() {};

        ///////////////////////////////////////////////////////////////////////////////////
        // IConstructable interface
        const ConstructionDescription& ConstructionDesc() const {return m_desc;};

        ///////////////////////////////////////////////////////////////////////////////////
        // Adds element to specified position
        void SetElement(const ConstructionDescription& element, const vector3i_t& position, Directions direction, Directions copySettingsFrom);
        
        ///////////////////////////////////////////////////////////////////////////////////
        // Weld two groups into single one
        bool Weld(uint32_t group1, uint32_t group2);

        ///////////////////////////////////////////////////////////////////////////////////
        // Returns element desc on requested position
        // the function is not a constant, so element can be modified
        Element* GetElement(const vector3i_t& position);

        ///////////////////////////////////////////////////////////////////////////////////
        // Updates neighborhood of component in pos
        void UpdateNeighbourhood(const vector3i_t& pos, Element& self);

        ///////////////////////////////////////////////////////////////////////////////////
        // Copies element settings from neighbor at copySettingsFrom direction
        void CopySettingsFrom(const vector3i_t& pos, Element& self, Directions copySettingsFrom);

        ///////////////////////////////////////////////////////////////////////////////////
        // Allows to iterrate through Core components
        void IterrateObject(std::function<void(size_t, size_t, size_t, Element&)> visitor);

        ///////////////////////////////////////////////////////////////////////////////////
        // Indicates that geometry was updated since last request
        bool IsUpdated();

        ///////////////////////////////////////////////////////////////////////////////////
        // Clear berth
        void Reset();

    private:
        ///////////////////////////////////////////////////////////////////////////////////
        // looks for relative element of item. Item will be searched in direction
        const NeighborDesc* findNeighbor(const Element& item, const vector3i_t& direction) const;

        ///////////////////////////////////////////////////////////////////////////////////
        // converts self or neighbor to "joint/connector" objects, angle joints etc.
        void morph(const vector3i_t& position, Element& item);

        ///////////////////////////////////////////////////////////////////////////////////
        // fast rotate operation on multiple by Pi/2 angles
        vector3i_t rotate(const vector3i_t& vec, unsigned int dst) const;

        ConstructionLibrary&        m_library;

        ConstructionDescription      m_desc;
        Utils::QuadTree< Pillar_t >  m_pillars;

        bool                         m_isDirty;

        ConstructionDescription      m_reference;

        unsigned int                m_lastGroupIndex;

        PREVENT_COPY(Core);
    };

}//end  of namespace constructor

// eof