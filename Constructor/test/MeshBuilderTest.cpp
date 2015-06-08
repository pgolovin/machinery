#include "BuildingBerth.h"
#include <gtest/gtest.h>
#include <memory>

using namespace ConstructorImpl;

#ifndef min 
#define min(a, b) (a)<(b) ? (a) : (b)
#endif

#ifndef max 
#define max(a, b) (a)>(b) ? (a) : (b)
#endif

class MeshBuilderTest : public ::testing::Test
{
public:
    MeshBuilderTest() {};

	static void SetUpTestCase() { m_builder = Constructor::Create(); }
	static void TearDownTestCase() { Constructor::Destroy(m_builder); }

    void SetUp()
    {
        m_builder->NewConstruction();
    }
protected:

    void checkMesh(size_t refCount, vector3f_t refmin, vector3f_t refmax, std::string fileName = "")
    {
        const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();

        auto& positions = desc.Shapes[ConstructorElements::MeshIndex].Positions;
        size_t verticesTotal = positions.Data.size();

        vector3f_t maximum(0,0,0);
        vector3f_t minimum(500,500,500);

        for (size_t j = 0; j < verticesTotal; j += 3)
        {
            vector3f_t current(positions.Data[j], positions.Data[j+1], positions.Data[j+2]);
            maximum.x = (max(maximum.x, current.x));
            maximum.y = (max(maximum.y, current.y));
            maximum.z = (max(maximum.z, current.z));

            minimum.x = (min(minimum.x, current.x));
            minimum.y = (min(minimum.y, current.y));
            minimum.z = (min(minimum.z, current.z));
        }

        if (refCount)
        {
            EXPECT_EQ(refCount, verticesTotal/3);
        }

        ASSERT_FLOAT_EQ(refmax.x, maximum.x);
        ASSERT_FLOAT_EQ(refmax.y, maximum.y);
        ASSERT_FLOAT_EQ(refmax.z, maximum.z);

        ASSERT_FLOAT_EQ(refmin.x, minimum.x);
        ASSERT_FLOAT_EQ(refmin.y, minimum.y);
        ASSERT_FLOAT_EQ(refmin.z, minimum.z);

        BBox box = m_builder->GetBoundingBox();

        ASSERT_FLOAT_EQ(refmax.x, (float)box.RBB.x);
        ASSERT_FLOAT_EQ(refmax.y, (float)box.RBB.y);
        ASSERT_FLOAT_EQ(refmax.z, (float)box.RBB.z);

        ASSERT_FLOAT_EQ(refmin.x, (float)box.LFT.x);
        ASSERT_FLOAT_EQ(refmin.y, (float)box.LFT.y);
        ASSERT_FLOAT_EQ(refmin.z, (float)box.LFT.z);

        if (fileName.size())
        {
            exportMesh(desc, fileName);
        }
    }

    void exportMesh(const IMesh::Desc& desc, std::string fileName)
    {
        FILE *f = nullptr;
        fopen_s(&f, fileName.c_str(), "w");
        if (nullptr == f)
            return;
        vector3i_t wlh = m_builder->GetBoundingBox().RBB - m_builder->GetBoundingBox().LFT;
        vector3i_t lft = m_builder->GetBoundingBox().LFT;
        // save vertices
        auto& positions = desc.Shapes[ConstructorElements::MeshIndex].Positions;
        for (size_t j = 0; j < positions.Data.size(); j += 3)
        {
            vector3f_t current(positions.Data[j], positions.Data[j + 1], positions.Data[j + 2]);
            fprintf(f, "v %.3f %.3f %.3f\n", current.x - lft.x - wlh.x/2.0, current.y, current.z - lft.z - wlh.z/2.0);
        }
        auto& normals = desc.Shapes[ConstructorElements::MeshIndex].Normals.Data;
        for (size_t j = 0; j < normals.size(); j += 3)
        {
            fprintf(f, "vn %.3f %.3f %.3f\n", normals[j], normals[j + 1], normals[j + 2]);
        }

        //save indices
        for (size_t j = 0; j < (positions.Data.size() / desc.Shapes[ConstructorElements::MeshIndex].Positions.ElementSize); j += 3)
        {
            fprintf(f, "f %u/%u %u/%u %u/%u\n", j + 1, j + 1, j + 2, j + 2, j + 3, j + 3);
        }

        fprintf(f, "o surface\n");
        auto& positions1 = desc.Shapes[ConstructorElements::BaseIndex].Positions;
        for (size_t j = 0; j < positions1.Data.size(); j += 3)
        {
            vector3f_t current(positions1.Data[j], positions1.Data[j + 1], positions1.Data[j + 2]);
            fprintf(f, "v %.3f %.3f %.3f\n", current.x - lft.x - wlh.x/2.0, current.y, current.z - lft.z - wlh.z/2.0);
        }
        auto& normals1 = desc.Shapes[ConstructorElements::BaseIndex].Normals.Data;
        for (size_t j = 0; j < normals1.size(); j += 3)
        {
            fprintf(f, "vn %.3f %.3f %.3f\n", normals1[j], normals1[j + 1], normals1[j + 2]);
        }

        fprintf(f, "f -4/-4 -3/-3 -2/-2 -1/-1\n");

        fclose(f);
    }

    static Constructor* m_builder;
};

Constructor *MeshBuilderTest::m_builder = nullptr;

TEST_F(MeshBuilderTest, DISABLED_SingleElementMesh)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    ASSERT_NO_FATAL_FAILURE(checkMesh(36, vector3f_t(0,0,0), vector3f_t(1,1,1), "c:\\tmp\\cube.obj"));
}

TEST_F(MeshBuilderTest, DISABLED_ElementsMesh)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,2,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,4,0), Directions::pZ);

    ASSERT_NO_FATAL_FAILURE(checkMesh(36 * 3, vector3f_t(0,0,0), vector3f_t(1,5,1), "c:\\tmp\\cube_pillar.obj"));
}

TEST_F(MeshBuilderTest, DISABLED_SinglePillarMesh)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,1,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,2,0), Directions::pZ);

    ASSERT_NO_FATAL_FAILURE(checkMesh(84, vector3f_t(0,0,0), vector3f_t(1,3,1), "c:\\tmp\\united_pillar.obj"));
}

TEST_F(MeshBuilderTest, DISABLED_CubeMesh)
{
    const size_t cubeScales = 3;
    for (size_t x = 0; x < cubeScales; ++x)
        for (size_t y = 0; y < cubeScales; ++y)
            for (size_t z = 0; z < cubeScales; ++z)
            {
                m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }
    const float BBox = (float)cubeScales;
    ASSERT_NO_FATAL_FAILURE(checkMesh(cubeScales*cubeScales*6*6, vector3f_t(0,0,0), vector3f_t(BBox, BBox, BBox), "c:\\tmp\\HugeCube.obj"));
}

TEST_F(MeshBuilderTest, DISABLED_CubeWithPillarMesh)
{
    const size_t cubeScales = 9;
    for (size_t x = 0; x < cubeScales; ++x)
        for (size_t y = 0; y < cubeScales; ++y)
            for (size_t z = 0; z < cubeScales; ++z)
            {
                if (x != cubeScales/2 || z != cubeScales/2)
                    m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }

    for (size_t y = 1; y < cubeScales-1; ++y)
    {
        m_builder->SetElement(ElementType::Cube, vector3i_t(cubeScales/2, y, cubeScales/2), Directions::pZ);
    }

    const float BBox = (float)cubeScales;
    ASSERT_NO_FATAL_FAILURE(checkMesh(0, vector3f_t(0,0,0), vector3f_t(BBox, BBox, BBox), "c:\\tmp\\PillaredCube.obj"));
}

TEST_F(MeshBuilderTest, DISABLED_WeldedCubeWithPillarMesh)
{
    const size_t cubeScales = 9;
    for (size_t x = 0; x < cubeScales; ++x)
        for (size_t y = 0; y < cubeScales; ++y)
            for (size_t z = 0; z < cubeScales; ++z)
            {
                if (x != cubeScales/2 || z != cubeScales/2)
                    m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }

    for (size_t y = 1; y < cubeScales - 1; ++y)
    {
        m_builder->SetElement(ElementType::Cube, vector3i_t(cubeScales/2, y, cubeScales/2), Directions::pZ);
    }

    ASSERT_NE(uint32_t(~0x0), m_builder->GetGroup(vector3i_t(cubeScales/2, cubeScales/2, cubeScales/2)));
    ASSERT_NE(uint32_t(~0x0), m_builder->GetGroup(vector3i_t(0,0,0)));
    ASSERT_NE(m_builder->GetGroup(vector3i_t(cubeScales/2, cubeScales/2, cubeScales/2)), m_builder->GetGroup(vector3i_t(0,0,0)));

    ASSERT_TRUE(m_builder->Weld(m_builder->GetGroup(vector3i_t(0,0,0)), m_builder->GetGroup(vector3i_t(cubeScales/2, cubeScales/2, cubeScales/2))));

    ASSERT_EQ(m_builder->GetGroup(vector3i_t(cubeScales/2, cubeScales/2, cubeScales/2)), m_builder->GetGroup(vector3i_t(0,0,0)));
    const float BBox = (float)cubeScales;
    ASSERT_NO_FATAL_FAILURE(checkMesh(0, vector3f_t(0,0,0), vector3f_t(BBox, BBox, BBox), "c:\\tmp\\PillaredCubeWelded.obj"));
}

TEST_F(MeshBuilderTest, DISABLED_SpongeMesh)
{
    const size_t cubeScales = 6;
    for (size_t x = 0; x < cubeScales; ++x)
        for (size_t y = 0; y < cubeScales; ++y)
            for (size_t z = 0; z < cubeScales; ++z)
            {
                if ((x+y+z) % 2)
                    m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }
    const float BBox = (float)cubeScales;
    //hmmm... 64? * 64?
    ASSERT_NO_FATAL_FAILURE(checkMesh((cubeScales*cubeScales*cubeScales/2)*36, vector3f_t(0,0,0), vector3f_t(BBox, BBox, BBox), "c:\\tmp\\sponge.obj"));
}

TEST_F(MeshBuilderTest, DISABLED_WedgeCross)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,1), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(2,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,2), Directions::pZ);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\wedge_cross.obj");
}

TEST_F(MeshBuilderTest, DISABLED_Wedges)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::pZ);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\wedges.obj");
}

TEST_F(MeshBuilderTest, DISABLED_Pyramid)
{
    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(3,0,3), Directions::pX);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(1,0,3), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(2,0,3), Directions::pZ);
    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(3,0,0), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(3,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(3,0,2), Directions::pX);
    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(0,0,3), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(1,0,0), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(2,0,0), Directions::nZ);
    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(0,0,0), Directions::nX);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(0,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge,       vector3i_t(0,0,2), Directions::nX);
    m_builder->SetElement(ElementType::Cube ,       vector3i_t(1,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Cube ,       vector3i_t(1,0,2), Directions::nX);
    m_builder->SetElement(ElementType::Cube ,       vector3i_t(2,0,2), Directions::nX);
    m_builder->SetElement(ElementType::Cube ,       vector3i_t(2,0,1), Directions::nX);

    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(2,1,2), Directions::pX);
    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(2,1,1), Directions::nZ);
    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(1,1,2), Directions::pZ);
    m_builder->SetElement(ElementType::WedgeOutCorner, vector3i_t(1,1,1), Directions::nX);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\piramid.obj");
}

TEST_F(MeshBuilderTest, DISABLED_OuterWedgeAngle)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,2), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,2), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::nZ);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\outCorner.obj");
}

TEST_F(MeshBuilderTest, DISABLED_InWedgeAngle)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,2), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,2), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::pZ);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\inCorner.obj");
}

TEST_F(MeshBuilderTest, DISABLED_Cocentric_in)
{
    size_t size = 3;
    size_t offset = 2;
    for (size_t i = 0; i < size; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset,0,offset+i), Directions::nX);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+size-1,0,offset+i), Directions::pX);
    }

    for (size_t i = 1; i < size - 1; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+i,0,offset+0), Directions::nZ);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+i,0,offset+size-1), Directions::pZ);
    }

    size = 7;

    for (size_t i = 1; i < size - 1; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,0), Directions::nZ);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,size-1), Directions::pZ);
    }

    for (size_t i = 0; i < size; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,i), Directions::nX);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(size-1,0,i), Directions::pX);
    }

    offset = 12;
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+1,0,0), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+1,0,1), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset,0,0), Directions::nZ);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(10,0,0), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(10,0,1), Directions::nZ);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\OuterCenters.obj");
}


TEST_F(MeshBuilderTest, DISABLED_Cocentric_out)
{
    size_t size = 3;
    size_t offset = 2;
    for (size_t i = 0; i < size; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset,0,offset+i), Directions::pX);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+size-1,0,offset+i), Directions::nX);
    }

    for (size_t i = 1; i < size - 1; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+i,0,offset+0), Directions::pZ);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+i,0,offset+size-1), Directions::nZ);
    }

    size = 7;

    for (size_t i = 1; i < size - 1; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,0), Directions::pZ);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,size-1), Directions::nZ);
    }

    for (size_t i = 0; i < size; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,i), Directions::pX);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(size-1,0,i), Directions::nX);
    }

    offset = 12;
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+1,0,0), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset+1,0,1), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(offset,0,0), Directions::pZ);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\InnerCenters.obj");
}

TEST_F(MeshBuilderTest, DISABLED_WedgeSpikes)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(3,0,3), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(3,0,4), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(4,0,4), Directions::nX);

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\WedgeSpikes.obj");
}

TEST_F(MeshBuilderTest, DISABLED_CilindricPillar)
{
    for (size_t i = 0; i < 5; ++i)
    {
        m_builder->SetElement(ElementType::Cilinder, vector3i_t(0,i,0), Directions::pZ);
    }

    const IMesh::Desc& desc = m_builder->GetMesh().GetDesc();
    exportMesh(desc, "c:\\tmp\\CilindricPillar.obj");
}
// eof