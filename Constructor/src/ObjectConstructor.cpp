#include "ObjectConstructor.h"
#include "Library.h"
#include <assert.h>

using namespace ConstructorImpl;

#ifndef min 
#define min(a, b) (a)<(b) ? (a) : (b)
#endif

#ifndef max 
#define max(a, b) (a)>(b) ? (a) : (b)
#endif

Core::Core(ConstructionLibrary& constructionLibrary) 
    : m_library(constructionLibrary)
    , m_pillars(256)
    , m_isDirty(false)
    , m_lastGroupIndex(0)
{
    m_desc.boundingBox = BBox(vector3i_t(INT32_MAX, INT32_MAX, INT32_MAX), vector3i_t(0,0,0));
}

void Core::SetElement(const ConstructionDescription& desc, const vector3i_t& position, Directions direction, Directions copySettingsFrom)
{
    copySettingsFrom;
    m_isDirty = true;

    m_desc.direction = direction;
    m_desc.primitiveUID = desc.primitiveUID;

    m_desc.boundingBox.LFT = vector3f_t(
        min(position.x + desc.boundingBox.LFT.x, m_desc.boundingBox.LFT.x),
        min(position.y + desc.boundingBox.LFT.y, m_desc.boundingBox.LFT.y),
        min(position.z + desc.boundingBox.LFT.z, m_desc.boundingBox.LFT.z));
    m_desc.boundingBox.RBB = vector3f_t(
        max(position.x + desc.boundingBox.RBB.x, m_desc.boundingBox.RBB.x),
        max(position.y + desc.boundingBox.RBB.y, m_desc.boundingBox.RBB.y),
        max(position.z + desc.boundingBox.RBB.z, m_desc.boundingBox.RBB.z));

    // Y is UP direction
    Element element = {&desc, desc.primitiveUID, direction, direction, 0};
    // if priitive can be morfed, morf it
    if (ElementType::Wedge == element.construction->primitiveUID)
    {
        morph(position, element);
    }

    CopySettingsFrom(position, element, copySettingsFrom);
    // notify neighbours about new element
    UpdateNeighbourhood(position, element);

    m_pillars.item(position.x, position.z).insert(position.y, element);

    if (vector3i_t(1,1,1) != (desc.boundingBox.RBB - desc.boundingBox.LFT))
    {
        for (int x = desc.boundingBox.LFT.x; x < desc.boundingBox.RBB.x; ++x)
        {
            for (int z = desc.boundingBox.LFT.z; z < desc.boundingBox.RBB.z; ++z)
            {
                if (x || z)
                {
                    Element ref = {&m_reference, desc.primitiveUID, direction, direction, 0};
                    m_pillars.item(position.x + x, position.z + z).insert(position.y, ref);
                }
            }
        }
    }
}

bool Core::Weld(uint32_t group1, uint32_t group2)
{
    if (group1 == group2)
        return false;

    bool res = false;

    m_pillars.for_each([&](size_t x, size_t z, Pillar_t& pillar)
    {
        pillar.for_each([&](size_t y, Element& e)
        {
            if (e.group == group2 || e.group == group1)
            {
                res = res || e.group == group2;
                e.group = group1;
                UpdateNeighbourhood(vector3i_t(x, y, z), e);
            }
        });
    });
    return res;
}

Element* Core::GetElement(const vector3i_t& position)
{
    auto pillar = m_pillars.get_item_at(position.x, position.z);
    return pillar ? pillar->get_item_at(position.y) : nullptr;
}

void Core::UpdateNeighbourhood(const vector3i_t& pos, Element& self)
{
    for (auto neighbor : self.construction->neighbors)
    {
        vector3i_t relativeDirection = rotate(neighbor.relationPosition, self.direction);

        Element* item = GetElement(relativeDirection + pos);
        if (!item || item->group != self.group )
            continue;

        const NeighborDesc* itemNeighbour = findNeighbor(*item, relativeDirection);
        if (!itemNeighbour)
            continue;

        if (itemNeighbour->relationWeight <= neighbor.relationWeight)
        {
            item->neighbourhood |= itemNeighbour->relationFlag;
        }

        if (itemNeighbour->relationWeight >= neighbor.relationWeight)
        {
            self.neighbourhood |= neighbor.relationFlag;
        }
    }
}

void Core::CopySettingsFrom(const vector3i_t& pos, Element& self, Directions copySettingsFrom)
{
    copySettingsFrom;
    pos;
    self;
    if (Directions::nY == copySettingsFrom && 0 == pos.y)
    {
        self.group = 0;
        return;
    }

    vector3i_t neighbour = rotate(vector3i_t(0,0,1), copySettingsFrom);
    Element* item = GetElement(neighbour + pos);
    if (item)
    {
        self.group = item->group;
    }
    else
    {
        self.group = ++m_lastGroupIndex;
    }
}

void Core::IterrateObject(std::function<void(size_t, size_t, size_t, Element&)> visitor) 
{
    m_pillars.for_each([&](size_t x, size_t z, Pillar_t& pillar){
        pillar.for_each([&](size_t y, Element& e){
            visitor(x,y,z,e);
        });
    });
}

bool Core::IsUpdated() 
{
    bool state = m_isDirty; 
    m_isDirty = false; 
    return state;
}

void Core::Reset()
{
    m_isDirty = true;
    m_lastGroupIndex = 0;
    m_pillars.clear();
    m_desc.boundingBox = BBox(vector3i_t(INT32_MAX, INT32_MAX, INT32_MAX), vector3i_t(0,0,0));
}

///////////////////////////////////////////////////////////////////////////////////
// private section
///////////////////////////////////////////////////////////////////////////////////

const NeighborDesc* Core::findNeighbor(const Element& item, const vector3i_t& direction) const
{
    const vector3i_t negative(-direction);
    for (const auto& relations : item.construction->neighbors)
    {
        if (rotate(relations.relationPosition, item.direction) == negative)
            return &relations;
    }
    return nullptr;
}

void Core::morph(const vector3i_t& position, Element& self)
{
    // fing neighbor on behind
    vector3i_t neighborPosition = rotate(self.construction->neighbors[DirectionIndices::nZ_idx].relationPosition, self.direction);
    Element* item = GetElement(neighborPosition + position);
    vector3i_t sD = rotate(vector3i_t(0, 0, 1), self.originalDirection);

    // morph self
    if (item && (item->type == Wedge))
    {
        //calculate absolute directions of current object and neighbour
        vector3i_t iD = rotate(vector3i_t(0, 0, 1), item->originalDirection);
        // morph objects if they are perpendicular
        if (iD.x * sD.x + iD.z * sD.z == 0 )
        {
            self.construction = m_library.GetConstructionDescription(WedgeOutCorner);
            //mirror wedge angle if required
            if (iD.x * sD.z - iD.z * sD.x > 0)
            {
                self.direction |= Directions::LeftToRight;
            }
        }
    }
    neighborPosition = rotate(self.construction->neighbors[DirectionIndices::pZ_idx].relationPosition, self.originalDirection);
    item = GetElement(neighborPosition + position);
    if (item && (item->type == Wedge))
    {
        //calculate absolute directions of current object and neighbour
        vector3i_t iD = rotate(vector3i_t(0, 0, 1), item->originalDirection);
        // morph objects if they are perpendicular
        if (iD.x * sD.x + iD.z * sD.z == 0 )
        {
            self.construction = m_library.GetConstructionDescription(WedgeInCorner);
            //mirror wedge angle if required
            if (iD.x * sD.z - iD.z * sD.x < 0)
            {
                self.direction |= Directions::LeftToRight;
            }
        }
    }
    neighborPosition = rotate(self.construction->neighbors[DirectionIndices::nX_idx].relationPosition, self.originalDirection);
    item = GetElement(neighborPosition + position);

    // morph neighbor
    if (item && item->type == Wedge)
    {
        //calculate absolute directions of current object and neighbour
        vector3i_t iD = rotate(vector3i_t(0, 0, 1), item->originalDirection);
        // morph objects if they are perpendicular
        if (iD.x * sD.x + iD.z * sD.z == 0)
        {
            item->direction |= Directions::LeftToRight;
            item->construction = m_library.GetConstructionDescription( (iD.x * sD.z - iD.z * sD.x < 0) ? WedgeOutCorner : WedgeInCorner);
        }
    }

    neighborPosition = rotate(self.construction->neighbors[DirectionIndices::pX_idx].relationPosition, self.originalDirection);
    item = GetElement(neighborPosition + position);

    // morph neighbor
    if (item && item->type == Wedge)
    {
        //calculate absolute directions of current object and neighbour
        vector3i_t iD = rotate(vector3i_t(0, 0, 1), item->originalDirection);
        // morph objects if they are perpendicular
        if (iD.x * sD.x + iD.z * sD.z == 0)
        {
            item->construction = m_library.GetConstructionDescription( (iD.x * sD.z - iD.z * sD.x > 0) ? WedgeOutCorner : WedgeInCorner);
        }
    }
}


vector3i_t Core::rotate(const vector3i_t& vec, unsigned int dst) const
{
    vector3i_t out = vec;

    switch(dst & MODIFICATOR_MASK)
    {
    case Directions::LeftToRight:
        out.x = -out.x;
        break;
    }

    switch(dst & DIRECTION_MASK)
    {
    case Directions::nX : return vector3i_t(-out.z, out.y,  out.x);
    case Directions::pX : return vector3i_t( out.z, out.y, -out.x);
    case Directions::nZ : return vector3i_t(-out.x, vec.y, -out.z);
    case Directions::nY : return vector3i_t( out.x, -out.z, out.y); // GHM GUANO
    case Directions::pY : return vector3i_t( out.x,  out.z, out.y); // GHM GUANO
    }

    return out;
}

// eof