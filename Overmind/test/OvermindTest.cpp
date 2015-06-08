#include <Overmind.h>
#include "Constructor.h"
#include <gtest/gtest.h>
#include <memory>

#ifndef min 
#define min(a, b) (a)<(b) ? (a) : (b)
#endif

#ifndef max 
#define max(a, b) (a)>(b) ? (a) : (b)
#endif

class OvermindTest : public ::testing::Test
{
public:

    void SetUp()
    {
        m_overmind = &Overmind::Get();
    }
    void TearDown()
    {
        m_overmind->Amnesia();
    }
protected:
    Overmind* m_overmind;

    void checkMesh(Constructor &builder, size_t refCount, vector3f_t refmin, vector3f_t refmax, std::string fileName = "")
    {
        const IMesh::Desc& desc = builder.GetMesh().GetDesc();

        auto& positions = desc.Shapes[1].Positions;
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

        BBox box = builder.GetBoundingBox();

        ASSERT_FLOAT_EQ(refmax.x, (float)box.RBB.x);
        ASSERT_FLOAT_EQ(refmax.y, (float)box.RBB.y);
        ASSERT_FLOAT_EQ(refmax.z, (float)box.RBB.z);

        ASSERT_FLOAT_EQ(refmin.x, (float)box.LFT.x);
        ASSERT_FLOAT_EQ(refmin.y, (float)box.LFT.y);
        ASSERT_FLOAT_EQ(refmin.z, (float)box.LFT.z);

        if (fileName.size())
        {
            exportMesh(builder, desc, fileName);
        }
    }

    void exportMesh(Constructor &builder, const IMesh::Desc& desc, std::string fileName)
    {
        FILE *f = nullptr;
        fopen_s(&f, fileName.c_str(), "w");
        if (nullptr == f)
            return;
        vector3i_t wlh = builder.GetBoundingBox().RBB - builder.GetBoundingBox().LFT;
        vector3i_t lft = builder.GetBoundingBox().LFT;
        // save vertices
        auto& positions = desc.Shapes[1].Positions;
        for (size_t j = 0; j < positions.Data.size(); j += 3)
        {
            vector3f_t current(positions.Data[j], positions.Data[j + 1], positions.Data[j + 2]);
            fprintf(f, "v %.3f %.3f %.3f\n", current.x - lft.x - wlh.x/2.0, current.y, current.z - lft.z - wlh.z/2.0);
        }
        auto& normals = desc.Shapes[1].Normals.Data;
        for (size_t j = 0; j < normals.size(); j += 3)
        {
            fprintf(f, "vn %.3f %.3f %.3f\n", normals[j], normals[j + 1], normals[j + 2]);
        }

        //save indices
        for (size_t j = 0; j < (positions.Data.size() / desc.Shapes[1].Positions.ElementSize); j += 3)
        {
            fprintf(f, "f %u/%u %u/%u %u/%u\n", j + 1, j + 1, j + 2, j + 2, j + 3, j + 3);
        }

        fprintf(f, "o surface\n");
        auto& positions1 = desc.Shapes[0].Positions;
        for (size_t j = 0; j < positions1.Data.size(); j += 3)
        {
            vector3f_t current(positions1.Data[j], positions1.Data[j + 1], positions1.Data[j + 2]);
            fprintf(f, "v %.3f %.3f %.3f\n", current.x - lft.x - wlh.x/2.0, current.y, current.z - lft.z - wlh.z/2.0);
        }
        auto& normals1 = desc.Shapes[0].Normals.Data;
        for (size_t j = 0; j < normals1.size(); j += 3)
        {
            fprintf(f, "vn %.3f %.3f %.3f\n", normals1[j], normals1[j + 1], normals1[j + 2]);
        }

        fprintf(f, "f -4/-4 -3/-3 -2/-2 -1/-1\n");

        fclose(f);
    }
};

TEST_F(OvermindTest, ErrorCheck)
{
    ASSERT_NE(Status::OK, m_overmind->ExecuteScript("blah.blah"));
    std::string message = m_overmind->GetLastError().c_str();
    ASSERT_STRNE("", message.c_str());
}

TEST_F(OvermindTest, ExecuteTestScript)
{
    ASSERT_EQ(Status::OK, m_overmind->ExecuteScript("test_scripts/empty.lua"));
    std::string message = m_overmind->GetLastError().c_str();
    ASSERT_STREQ("", message.c_str()) << "Error received: " << message.c_str();
}

TEST_F(OvermindTest, RegisterObject)
{
    ILibrary& lib = m_overmind->GetConstructor().GetLibrary();

    ASSERT_EQ(Status::ResourceNotFound, lib.CheckObjectStatus("NewObject"));

    EXPECT_EQ(Status::OK, m_overmind->ExecuteScript("test_scripts/registerObject.lua"));
    std::string message = m_overmind->GetLastError().c_str();
    ASSERT_STREQ("", message.c_str()) << "Error received: " << message.c_str();

    ASSERT_EQ(Status::OK, lib.CheckObjectStatus("NewObject"));
}

TEST_F(OvermindTest, BuildObject)
{
    EXPECT_EQ(Status::OK, m_overmind->ExecuteScript("test_scripts/building.lua"));
    std::string message = m_overmind->GetLastError().c_str();
    ASSERT_STREQ("", message.c_str()) << "Error received: " << message.c_str();
    checkMesh(m_overmind->GetConstructor(), 0, vector3f_t(15,0,15), vector3f_t(18,3,18), "c:/tmp/a.obj");
}

// eof