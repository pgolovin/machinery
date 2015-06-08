#include "BuildingBerth.h"
#include "Library.h"
#include "Resources.h"

using namespace ConstructorImpl;

#include "ConstructionLibraryPrimitives.cpp"
#include "MeshLibraryPrimitives.cpp"

Constructor* Constructor::Create()
{
    return new BuildingBerth();
}

void Constructor::Destroy(Constructor* constructor)
{
	delete constructor;
}

BuildingBerth::BuildingBerth()
    : m_core(m_buildingBlocksLibrary.GetConstructionLibrary())
    , m_hull(m_buildingBlocksLibrary.GetMeshLibrary())
{
    RegisterDefaultConstructions(m_buildingBlocksLibrary.GetConstructionLibrary());
    RegisterDefaultMeshes(m_buildingBlocksLibrary.GetMeshLibrary());
};

void BuildingBerth::Reset()
{
    m_buildingBlocksLibrary.Reset();
    RegisterDefaultConstructions(m_buildingBlocksLibrary.GetConstructionLibrary());
    RegisterDefaultMeshes(m_buildingBlocksLibrary.GetMeshLibrary());
}

Core& BuildingBerth::GetCore()
{
    //assert(index < m_cores.size());
    return m_core;
}

ILibrary& BuildingBerth::GetLibrary()
{
    return m_buildingBlocksLibrary;
}

Status BuildingBerth::PlaceObject(PlacementParameters& parameters)
{
    const IConstructorObject* obj = m_buildingBlocksLibrary.GetObjectByName(parameters.name);
    if (nullptr == obj)
        return Status::ResourceNotFound;

    if (ElementType::Space == obj->GetConstructionId())
        return Status::OK;

    const ConstructionDescription* desc = m_buildingBlocksLibrary.GetConstructionLibrary().GetConstructionDescription(obj->GetConstructionId());
    if (nullptr == desc)
        return Status::ResourceNotFound;

    vector3i_t pos = vector3i_t(parameters.position.x, parameters.position.y, parameters.position.z);
    m_core.SetElement(*desc, pos, (Directions)parameters.orientation, (Directions)parameters.placeDirection);

    return Status::OK;
}

bool BuildingBerth::SetElement(ElementType type, const vector3i_t& position, Directions direction, Directions copySettingsFrom)
{
    if (type == ElementType::Space || type >= ElementType::SimplePrimitivesCount) 
    {
        return false;
    }

    m_core.SetElement(*m_buildingBlocksLibrary.GetConstructionLibrary().GetConstructionDescription(type), position, direction, copySettingsFrom);
    return true;
}

bool BuildingBerth::Weld(uint32_t group1, uint32_t group2)
{
    return m_core.Weld(group1, group2);
}

uint32_t BuildingBerth::GetGroup(const vector3i_t& position)
{
    Element *el = m_core.GetElement(position);
    return el ? el->group : ~0x0;
}

IMesh& BuildingBerth::GetMesh()
{
    if (m_core.IsUpdated())
    {
        m_hull.ConstructMesh(m_core);
    }
    return m_hull;
}

BBox BuildingBerth::GetBoundingBox() const
{
    return m_core.ConstructionDesc().boundingBox;
}
// eof