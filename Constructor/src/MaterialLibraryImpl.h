/////////////////////////////////////////////////////////////////////////////////
//
// class ObjectConstructor generates object from primitives and modules. 
//    the class doesn't generate mesh
//    
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Library.h"
#include <vector>

/////////////////////////////////////////////////////////////////////
///
/// object library public interface. Library is a singletone
///
/////////////////////////////////////////////////////////////////////
namespace ConstructorImpl
{
    class MaterialLibrary
    {
    public:
        const ILibraryMesh& GetMeshObject(unsigned int id);
        void RegisterMesh(unsigned int id, ILibraryMeshPtr& mesh);

        void RegisterSimpleMesh(std::string name, int id, ILibraryMesh* mesh);

        MaterialLibrary();
        virtual ~MaterialLibrary() {};

    private: // arguments
        std::vector<ILibraryMeshPtr>  m_primitives;
    };
}
//end  of namespace

// eof