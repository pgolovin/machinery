#include "MeshLibraryImpl.h"
#include <memory>

using namespace ConstructorImpl;

MeshLibrary::MeshLibrary() 
{
    m_primitives.resize(ElementType::SimplePrimitivesCount);
}

const ILibraryMesh& MeshLibrary::GetMeshObject(unsigned int id)
{
    return *m_primitives[id];
}

void MeshLibrary::RegisterMesh(unsigned int id, ILibraryMeshPtr& mesh)
{
    m_primitives[id] = mesh;
}

void MeshLibrary::RegisterSimpleMesh(std::string name, int id, ILibraryMesh* mesh)
{
    m_primitives[id].reset(mesh);
}
// eof