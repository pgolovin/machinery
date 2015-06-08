#include "HullConstructor.h"
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

Hull::Hull(MeshLibrary& library) 
    : m_library(library)
{
    m_hullDescription.Shapes.resize(ConstructorElements::ComponentsCount);
    m_hullDescription.Shapes[ConstructorElements::BaseIndex].Indices.ElementSize     = 1;
    m_hullDescription.Shapes[ConstructorElements::BaseIndex].Positions.ElementSize   = 3;
    m_hullDescription.Shapes[ConstructorElements::BaseIndex].Normals.ElementSize     = 3;
    m_hullDescription.Shapes[ConstructorElements::BaseIndex].LayoutType = IMesh::LayoutType::Fan;

    const float normals[] = {0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0};
    const float vertices[] = {0, 0, 0, 0, 0, 32, 32, 0, 32, 32, 0, 0};
    m_hullDescription.Shapes[ConstructorElements::BaseIndex].Positions.Data.assign(vertices, vertices + sizeof(vertices)/sizeof(float));
    m_hullDescription.Shapes[ConstructorElements::BaseIndex].Normals.Data.assign(normals, normals + sizeof(normals)/sizeof(float));

    m_hullDescription.Shapes[ConstructorElements::MeshIndex].Indices.ElementSize     = 1;
    m_hullDescription.Shapes[ConstructorElements::MeshIndex].Positions.ElementSize   = 3;
    m_hullDescription.Shapes[ConstructorElements::MeshIndex].Normals.ElementSize     = 3;
    m_hullDescription.Shapes[ConstructorElements::MeshIndex].LayoutType = IMesh::LayoutType::Triangle;
}

void Hull::ConstructMesh(Core& objectCore)
{
    m_hullDescription.Shapes[ConstructorElements::MeshIndex].Positions.Data.clear();
    m_hullDescription.Shapes[ConstructorElements::MeshIndex].Normals.Data.clear();

    objectCore.IterrateObject([&](size_t x, size_t y, size_t z, Element& e)
    {
        MeshProperties prop = {~e.neighbourhood, vector3f_t(x,y,z), e.direction};
        m_library.GetMeshObject(e.construction->primitiveUID).ConstructGeometry(prop, m_hullDescription.Shapes[ConstructorElements::MeshIndex]);
    });
}

const IMesh::Desc& Hull::GetDesc() const
{
    return m_hullDescription;
}
// eof