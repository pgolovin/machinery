#include "Constructor.h"
#include <gtest/gtest.h>
#include <memory>

class MeshLibraryTest : public ::testing::Test
{
public:
	MeshLibraryTest() //: m_constructor()
	{
	}

	~MeshLibraryTest()
	{
		Constructor::Destroy(m_constructor);
	}
protected:
	Constructor* m_constructor = Constructor::Create();
};

TEST_F(MeshLibraryTest, GetSpaceMesh)
{
    ILibrary& lib = m_constructor->GetLibrary();
    const ILibraryMesh& mesh = lib.GetMesh(ElementType::Space);

    IMesh::Shape desc;
    MeshProperties prop = {Directions::All, vector3f_t(0,0,0)};
    mesh.ConstructGeometry(prop, desc);
    ASSERT_EQ(0, desc.Positions.Data.size());
    IMeshPtr meshCloneTemp = mesh.Clone();
    ILibraryMesh* meshClone = (ILibraryMesh*)meshCloneTemp.get();
    meshClone->ConstructGeometry(prop, desc);
    ASSERT_EQ(0, desc.Positions.Data.size()) << "incorrect cloned object";
}

TEST_F(MeshLibraryTest, GetCubeMesh)
{
    ILibrary& lib = m_constructor->GetLibrary();
    const ILibraryMesh& mesh = lib.GetMesh(ElementType::Cube);

    IMesh::Shape desc;
    MeshProperties prop = {Directions::All, vector3f_t(0,0,0)};
    mesh.ConstructGeometry(prop, desc);
    ASSERT_EQ(108, desc.Positions.Data.size()) << "incorrect number of vertices";
}

TEST_F(MeshLibraryTest, GetClonedCubeMesh)
{
    ILibrary& lib = m_constructor->GetLibrary();
    IMeshPtr meshCloneTemp = lib.GetMesh(ElementType::Cube).Clone();
    ILibraryMesh* meshClone = (ILibraryMesh*)meshCloneTemp.get();
    IMesh::Shape desc;

    MeshProperties prop = {Directions::All, vector3f_t(0,0,0)};
    meshClone->ConstructGeometry(prop, desc);
    ASSERT_EQ(108, desc.Positions.Data.size()) << "incorrect number of vertices";
}

TEST_F(MeshLibraryTest, GetCubeMeshFace)
{
    ILibrary& lib = m_constructor->GetLibrary();
    const ILibraryMesh& mesh = lib.GetMesh(ElementType::Cube);
    IMesh::Shape desc;
    MeshProperties prop = {Directions::pX, vector3f_t(0,0,0)};
    prop.flags = Directions::pX | Directions::nX | Directions::nZ;
    mesh.ConstructGeometry(prop, desc);
    ASSERT_EQ(54, desc.Positions.Data.size());
}


// eof