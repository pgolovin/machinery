#include "MaterialLibraryImpl.h"
#include <memory>

using namespace ConstructorImpl;

MaterialLibrary::MaterialLibrary() 
{
    m_primitives.resize(ElementType::SimplePrimitivesCount);
}

const ILibraryMesh& MaterialLibrary::GetMeshObject(unsigned int id)
{
    return *m_primitives[id];
}

void MaterialLibrary::RegisterMesh(unsigned int id, ILibraryMeshPtr& mesh)
{
    m_primitives[id] = mesh;
}

void MaterialLibrary::RegisterSimpleMesh(std::string name, int id, ILibraryMesh* mesh)
{
    m_primitives[id].reset(mesh);
}
// eof