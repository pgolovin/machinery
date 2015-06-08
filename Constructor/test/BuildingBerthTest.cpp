#include "BuildingBerth.h"
#include <gtest/gtest.h>
#include <memory>

using namespace ConstructorImpl;

class BuildingBerthTest : public ::testing::Test
{
public:

    void SetUp()
    {
        m_builder.reset(new BuildingBerth);
    }
    void TearDown()
    {
        m_builder.reset();
    }
protected:
    std::unique_ptr<BuildingBerth> m_builder;
};

TEST_F(BuildingBerthTest, AddSpace)
{
    ASSERT_FALSE(m_builder->SetElement(ElementType::Space, vector3i_t(0,0,0), Directions::pZ));
}

TEST_F(BuildingBerthTest, AddInvalidPrimitive)
{
    ASSERT_FALSE(m_builder->SetElement(ElementType::SimplePrimitivesCount, vector3i_t(0,0,0), Directions::pZ));
}

TEST_F(BuildingBerthTest, AddPrimitive)
{
    ASSERT_TRUE(m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ));

    const ConstructionDescription desc = m_builder->GetCore().ConstructionDesc();
    EXPECT_EQ(vector3i_t(1,1,1), desc.boundingBox.RBB);
}

TEST_F(BuildingBerthTest, AddPrimitiveInNonZeroPosition)
{
    ASSERT_TRUE(m_builder->SetElement(ElementType::Cube, vector3i_t(2,5,3), Directions::pZ));

    const ConstructionDescription desc = m_builder->GetCore().ConstructionDesc();
    EXPECT_EQ(vector3i_t(1,1,1), (desc.boundingBox.RBB - desc.boundingBox.LFT));
}

TEST_F(BuildingBerthTest, AddSetOfPrimitives3X3X3)
{
// complex ? structure 3x3x3 created by 3 primitives
//      [0] --- ---
//      --- [2] ---
//      --- --- [0]
// keep in mind Up is on Y axis

    ASSERT_TRUE(m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ));
    ASSERT_TRUE(m_builder->SetElement(ElementType::Cube, vector3i_t(1,2,1), Directions::pZ));
    ASSERT_TRUE(m_builder->SetElement(ElementType::Cube, vector3i_t(2,0,2), Directions::pZ));

    const ConstructionDescription desc = m_builder->GetCore().ConstructionDesc();
    EXPECT_EQ(vector3i_t(3,3,3), desc.boundingBox.RBB);
}

TEST_F(BuildingBerthTest, AddBigPrimitive3X3X1)
{
    m_builder->SetElement(ElementType::CilindricPlatform, vector3i_t(1,0,1), Directions::pZ);

    const ConstructionDescription desc = m_builder->GetCore().ConstructionDesc();
    EXPECT_EQ(vector3i_t(3,1,3), (desc.boundingBox.RBB));
}

TEST_F(BuildingBerthTest, ConstructPillar)
{
    for (size_t i = 0; i < 100; ++i)
    {
        m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,i), Directions::pZ);
    }

    const ConstructionDescription desc = m_builder->GetCore().ConstructionDesc();
    EXPECT_EQ(vector3i_t(1,1,100), (desc.boundingBox.RBB));
}

TEST_F(BuildingBerthTest, DISABLED_ConstructSolidQube)
{
    const size_t cubeScales = 64;
    for (size_t x = 0; x < cubeScales; ++x)
    {
        for (size_t y = 0; y < cubeScales; ++y)
        {
            for (size_t z = 0; z < cubeScales; ++z)
            {
                m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }
        }
    }

    const ConstructionDescription desc = m_builder->GetCore().ConstructionDesc();
    EXPECT_EQ(vector3i_t(cubeScales, cubeScales, cubeScales), (desc.boundingBox.RBB));
}

TEST_F(BuildingBerthTest, DISABLED_IterateThroughSolidCube)
{
    const size_t cubeScales = 64;
    for (size_t x = 0; x < cubeScales; ++x)
    {
        for (size_t y = 0; y < cubeScales; ++y)
        {
            for (size_t z = 0; z < cubeScales; ++z)
            {
                 m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }
        }
    }
    ASSERT_NO_THROW ( m_builder->GetCore().IterrateObject([&](size_t, size_t, size_t, Element&){}));
}

TEST_F(BuildingBerthTest, DISABLED_ConstructSpongeSystem)
{
    const size_t cubeScales = 64;
    for (size_t x = 0; x < cubeScales; ++x)
    {
        for (size_t y = 0; y < cubeScales; ++y)
        {
            for (size_t z = 0; z < cubeScales; ++z)
            {
                if ((x+y+z) % 2)
                    m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }
        }
    }

    const ConstructionDescription desc = m_builder->GetCore().ConstructionDesc();
    EXPECT_EQ(vector3i_t(cubeScales, cubeScales, cubeScales), (desc.boundingBox.RBB));
}

TEST_F(BuildingBerthTest, DISABLED_IterateThroughSpongeSystem)
{
    const size_t cubeScales = 64;
    for (size_t x = 0; x < cubeScales; ++x)
    {
        for (size_t y = 0; y < cubeScales; ++y)
        {
            for (size_t z = 0; z < cubeScales; ++z)
            {
                if ((x+y+z) % 2)
                    m_builder->SetElement(ElementType::Cube, vector3i_t(x,y,z), Directions::pZ);
            }
        }
    }
    ASSERT_NO_FATAL_FAILURE ( m_builder->GetCore().IterrateObject([&](size_t x, size_t y, size_t z, Element&){
        ASSERT_EQ(1, ((x+y+z) % 2));
    }));
}

TEST_F(BuildingBerthTest, ElementNeighborhood)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,1,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,2,0), Directions::pZ);
    ASSERT_NO_FATAL_FAILURE (m_builder->GetCore().IterrateObject(
        [&](size_t, size_t y, size_t, Element& item){
            switch(y)
            {
            case 0:
                ASSERT_EQ(Directions::pY, item.neighbourhood); break;
            case 1:
                ASSERT_EQ(Directions::pY | Directions::nY, item.neighbourhood); break;
            case 2:
                ASSERT_EQ(Directions::nY, item.neighbourhood); break;
            default:
                FAIL();
            }
    }));
}

TEST_F(BuildingBerthTest, CubeToCubeNeighbors)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,1), Directions::pZ);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));

    m_builder->SetElement(ElementType::Cube, vector3i_t(2,0,1), Directions::pZ);
    ASSERT_EQ(Directions::pX, el->neighbourhood);

    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,1), Directions::pZ);
    ASSERT_EQ(Directions::pX | Directions::nX, el->neighbourhood);

    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,0), Directions::pZ);
    ASSERT_EQ(Directions::pX | Directions::nX | Directions::nZ, el->neighbourhood);

    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,2), Directions::pZ);
    ASSERT_EQ(Directions::pX | Directions::nX | Directions::nZ | Directions::pZ, el->neighbourhood);
}

TEST_F(BuildingBerthTest, CubeToWedgeNeighbors)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,1), Directions::pZ);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));

    m_builder->SetElement(ElementType::Wedge, vector3i_t(2,0,1), Directions::pZ);
    ASSERT_EQ(0, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pZ);
    ASSERT_EQ(0, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::pZ);
    ASSERT_EQ(0, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,2), Directions::pZ);
    ASSERT_EQ(Directions::pZ, el->neighbourhood);
}

TEST_F(BuildingBerthTest, RotatedNeighboursAffected)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,1), Directions::pZ);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));

    m_builder->SetElement(ElementType::Wedge, vector3i_t(2,0,1), Directions::pX);
    ASSERT_EQ(Directions::pX, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nX);
    ASSERT_EQ(Directions::pX | Directions::nX, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::nZ);
    ASSERT_EQ(Directions::pX | Directions::nX | Directions::nZ, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,2), Directions::pZ);
    ASSERT_EQ(Directions::pX | Directions::nX | Directions::nZ | Directions::pZ, el->neighbourhood);
}

TEST_F(BuildingBerthTest, RotatedNeighboursNotAffected)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,1), Directions::pZ);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));

    m_builder->SetElement(ElementType::Wedge, vector3i_t(2,0,1), Directions::nX);
    ASSERT_EQ(0, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pX);
    ASSERT_EQ(0, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::pZ);
    ASSERT_EQ(0, el->neighbourhood);

    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,2), Directions::nZ);
    ASSERT_EQ(0, el->neighbourhood);
}

TEST_F(BuildingBerthTest, Generated_NoWedgeMorfing)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pZ);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::Wedge, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_OuterWedgeAngle)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pZ);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_PiramidTop_1)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nZ);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_PiramidTop_2)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::pX);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_PiramidTop_3)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nZ);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_PiramidTop_4)
{
    const size_t size = 6;
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

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,size - 1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,size - 1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_PiramidTop_5)
{
    const size_t size = 3;
    for (size_t i = 0; i < size; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,i), Directions::nX);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(size-1,0,i), Directions::pX);
    }

    for (size_t i = 1; i < size - 1; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,0), Directions::nZ);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,size-1), Directions::pZ);
    }

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,size - 1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,size - 1));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_InnerWedgeAngle)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::nZ);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_ConeHall_1)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::pZ);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_ConeHall_2)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::nX);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_ConeHall_3)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,0), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(1,0,1), Directions::nZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,1), Directions::pX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,0), Directions::pZ);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(1,0,1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_ConeHall_4)
{
    const size_t size = 6;
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

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,size - 1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,size - 1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_ConeHall_5)
{
    const size_t size = 3;
    for (size_t i = 0; i < size; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(0,0,i), Directions::pX);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(size-1,0,i), Directions::nX);
    }

    for (size_t i = 1; i < size - 1; ++i)
    {
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,0), Directions::pZ);
        m_builder->SetElement(ElementType::Wedge, vector3i_t(i,0,size-1), Directions::nZ);
    }

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,size - 1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(size - 1,0,size - 1));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, Generated_WedgeSpikes)
{
    m_builder->SetElement(ElementType::Wedge, vector3i_t(3,0,3), Directions::nX);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(3,0,4), Directions::pZ);
    m_builder->SetElement(ElementType::Wedge, vector3i_t(4,0,4), Directions::nX);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(3,0,4));
    ASSERT_EQ(ElementType::WedgeOutCorner, el->construction->primitiveUID);
    el = m_builder->GetCore().GetElement(vector3i_t(4,0,4));
    ASSERT_EQ(ElementType::WedgeInCorner, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, CilindricPillar)
{
    m_builder->SetElement(ElementType::Cilinder, vector3i_t(0,0,0), Directions::nX);
    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(ElementType::Cilinder, el->construction->primitiveUID);
}

TEST_F(BuildingBerthTest, ElementGroups)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,1,0), Directions::pZ);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(0, el->group);
    el = m_builder->GetCore().GetElement(vector3i_t(0,1,0));
    ASSERT_EQ(0, el->group);
}

TEST_F(BuildingBerthTest, ElementGroupFromUpperObject)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,10,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,9,0), Directions::pZ, Directions::pY);

    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,10,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(0, 9,0));
    ASSERT_EQ(el1->group, el2->group);
}

TEST_F(BuildingBerthTest, BasementGroups)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(10,0,10), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(10,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,10), Directions::pZ);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    ASSERT_EQ(0, el->group);
    el = m_builder->GetCore().GetElement(vector3i_t(10,0,10));
    ASSERT_EQ(0, el->group);
    el = m_builder->GetCore().GetElement(vector3i_t(10,0,0));
    ASSERT_EQ(0, el->group);
    el = m_builder->GetCore().GetElement(vector3i_t(0,0,10));
    ASSERT_EQ(0, el->group);
}

TEST_F(BuildingBerthTest, ElementInDifferentGroups)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,10,0), Directions::pZ);

    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(0,10,0));

    ASSERT_NE(el1->group, el2->group);
}

TEST_F(BuildingBerthTest, ElementInDifferentGroups1)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,10,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,9,0), Directions::pZ);

    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,10,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(0, 9,0));

    ASSERT_NE(el1->group, el2->group);
    ASSERT_EQ(0, el1->neighbourhood);
    ASSERT_EQ(0, el2->neighbourhood);
}

TEST_F(BuildingBerthTest, NotNaighborsIfFromDifferentGroups)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,2,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(2,2,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(1,2,0), Directions::pZ, Directions::pX);

    Element *el = m_builder->GetCore().GetElement(vector3i_t(0,2,0));
    ASSERT_EQ(0, el->neighbourhood);
}

TEST_F(BuildingBerthTest, WeldingNothing)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    EXPECT_FALSE(m_builder->Weld(0, 100));
}

TEST_F(BuildingBerthTest, WeldSelf)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(1,0,0), Directions::pZ);
    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(1,0,0));
    ASSERT_EQ(el1->group, el2->group);

    EXPECT_FALSE(m_builder->Weld(el1->group, el1->group));
}

TEST_F(BuildingBerthTest, WeldingGroups)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,10,0), Directions::pZ, Directions::pZ);

    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(0,10,0));

    EXPECT_NE(el1->group, el2->group);
    EXPECT_TRUE(m_builder->Weld(el1->group, el2->group));
    EXPECT_EQ(el1->group, el2->group);
}

TEST_F(BuildingBerthTest, WeldingNeighbours)
{
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,0), Directions::pZ);
    m_builder->SetElement(ElementType::Cube, vector3i_t(0,1,0), Directions::pZ, Directions::pZ);

    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(0,1,0));

    EXPECT_NE(el1->group, el2->group);

    EXPECT_EQ(Directions::NO, el1->neighbourhood);
    EXPECT_EQ(Directions::NO, el2->neighbourhood);

    EXPECT_TRUE(m_builder->Weld(el2->group, el1->group));

    EXPECT_NE(Directions::NO, el1->neighbourhood);
    EXPECT_NE(Directions::NO, el2->neighbourhood);
}

TEST_F(BuildingBerthTest, WeldingHugeGroups)
{
    const size_t itemLength = 3;
    for (size_t i = 0; i < itemLength; ++i)
    {
        m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,i), Directions::pZ);
        m_builder->SetElement(ElementType::Cube, vector3i_t(0,1,i), Directions::pZ, Directions::nZ);
    }

    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(0,1,0));

    EXPECT_NE(el1->group, el2->group);
    EXPECT_TRUE(m_builder->Weld(el1->group, el2->group));

    el1 = m_builder->GetCore().GetElement(vector3i_t(0, 0, itemLength - 1));
    el2 = m_builder->GetCore().GetElement(vector3i_t(0, 1, itemLength - 1));

    EXPECT_EQ(el1->group, el2->group);

    EXPECT_EQ(Directions::nZ | Directions::pY, el1->neighbourhood);
    EXPECT_EQ(Directions::nZ | Directions::nY, el2->neighbourhood);
}

TEST_F(BuildingBerthTest, RevercedWeldingHugeGroups)
{
    const size_t itemLength = 3;
    for (size_t i = 0; i < itemLength; ++i)
    {
        m_builder->SetElement(ElementType::Cube, vector3i_t(0,0,i), Directions::pZ);
        m_builder->SetElement(ElementType::Cube, vector3i_t(0,1,i), Directions::pZ, Directions::nZ);
    }

    Element *el1 = m_builder->GetCore().GetElement(vector3i_t(0,0,0));
    Element *el2 = m_builder->GetCore().GetElement(vector3i_t(0,1,0));

    EXPECT_NE(el1->group, el2->group);
    EXPECT_TRUE(m_builder->Weld(el2->group, el1->group));

    el1 = m_builder->GetCore().GetElement(vector3i_t(0, 0, itemLength - 1));
    el2 = m_builder->GetCore().GetElement(vector3i_t(0, 1, itemLength - 1));

    EXPECT_EQ(el1->group, el2->group);

    EXPECT_EQ(Directions::nZ | Directions::pY, el1->neighbourhood);
    EXPECT_EQ(Directions::nZ | Directions::nY, el2->neighbourhood);
}

TEST_F(BuildingBerthTest, WeldedCubeWithPillarMesh)
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

    Element* el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, 1, cubeScales/2));
    ASSERT_EQ(Directions::pY, el->neighbourhood);
    for (size_t y = 2; y < cubeScales - 2; ++y)
    {
        Element* el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, y, cubeScales/2));
        ASSERT_EQ(Directions::pY | Directions::nY, el->neighbourhood);
        uint32_t grp = el->group;
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2-1, y, cubeScales/2));
        ASSERT_EQ(Directions::nX | Directions::pZ | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_NE(grp, el->group);
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2+1, y, cubeScales/2));
        ASSERT_EQ(Directions::pX | Directions::pZ | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_NE(grp, el->group);
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, y, cubeScales/2+1));
        ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_NE(grp, el->group);
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, y, cubeScales/2-1));
        ASSERT_EQ(Directions::pX | Directions::nX | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_NE(grp, el->group);
    }
    el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, cubeScales-2, cubeScales/2));
    ASSERT_EQ(Directions::nY, el->neighbourhood);

    ASSERT_TRUE(m_builder->Weld(0, 1));

    el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, 1, cubeScales/2));
    ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::nZ | Directions::pY, el->neighbourhood);
    for (size_t y = 2; y < cubeScales - 2; ++y)
    {
        Element* el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, y, cubeScales/2));
        ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        uint32_t grp = el->group;
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2-1, y, cubeScales/2));
        ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_EQ(grp, el->group);
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2+1, y, cubeScales/2));
        ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_EQ(grp, el->group);
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, y, cubeScales/2+1));
        ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_EQ(grp, el->group);
        el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, y, cubeScales/2-1));
        ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::nZ | Directions::pY | Directions::nY, el->neighbourhood);
        ASSERT_EQ(grp, el->group);
    }
    el = m_builder->GetCore().GetElement(vector3i_t(cubeScales/2, cubeScales-2, cubeScales/2));
    ASSERT_EQ(Directions::pX | Directions::nX | Directions::pZ | Directions::nZ | Directions::nY, el->neighbourhood);
}

TEST_F(BuildingBerthTest, AddInvalidObject)
{
    Vector pos = { 0 };
    PlacementParameters params = { "Nothing", pos, Directions::pZ, Directions::nY };
    ASSERT_EQ(Status::ResourceNotFound, m_builder->PlaceObject(params));
}

TEST_F(BuildingBerthTest, AddCubeShapedObject)
{
    ObjectProperties properties = {"Cube", "", "", "Cube"};
    IConstructorObjectPtr ptr( new ConstructorObjectBase(properties));
    m_builder->GetLibrary().RegisterObject("Cube", ptr);
    Vector pos = { 0 };
    PlacementParameters params = { "Cube", pos, Directions::pZ, Directions::nY };
    ASSERT_EQ(Status::OK, m_builder->PlaceObject(params));

    Element* el = m_builder->GetCore().GetElement(vector3i_t(0, 0, 0));
    ASSERT_TRUE(nullptr != el);
    ASSERT_EQ(ElementType::Cube, el->construction->primitiveUID);
}
// eof