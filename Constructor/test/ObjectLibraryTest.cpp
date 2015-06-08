#include "Constructor.h"
#include <gtest/gtest.h>
#include <memory>

class ObjectLibraryPrimitivesTest : public ::testing::Test
{
public:
	ObjectLibraryPrimitivesTest() //: m_constructor()
	{
	}

	~ObjectLibraryPrimitivesTest()
	{
		Constructor::Destroy(m_constructor);
	}
protected:
	Constructor* m_constructor = Constructor::Create();
};

#define BEGIN_CHECK_PRIMITIVE_TEST(Fixture, Type, tlf, brb)                                                             \
    TEST_F(Fixture, DescriptionOf_##Type##_Element)																		\
	{																													\
	const ConstructionDescription* desc = m_constructor->GetLibrary().GetConstructionByName(#Type);						\
    ASSERT_EQ(ElementType::##Type, desc->primitiveUID) << "incorrect primitive type expected: ElementType::" << #Type;  \
    EXPECT_EQ(desc->boundingBox.LFT, tlf);                                                                              \
    EXPECT_EQ(desc->boundingBox.RBB, (brb));                                                                            \
    EXPECT_EQ(desc->direction, Directions::pZ);

#define END_CHECK_PRIMITIVE_TEST() }

BEGIN_CHECK_PRIMITIVE_TEST(ObjectLibraryPrimitivesTest, Space, vector3i_t(0, 0, 0), vector3i_t(1, 1, 1))
END_CHECK_PRIMITIVE_TEST();

BEGIN_CHECK_PRIMITIVE_TEST(ObjectLibraryPrimitivesTest, Cube, vector3i_t(0, 0, 0), vector3i_t(1, 1, 1))
END_CHECK_PRIMITIVE_TEST();

BEGIN_CHECK_PRIMITIVE_TEST(ObjectLibraryPrimitivesTest, Wedge, vector3i_t(0, 0, 0), vector3i_t(1, 1, 1))
END_CHECK_PRIMITIVE_TEST();

BEGIN_CHECK_PRIMITIVE_TEST(ObjectLibraryPrimitivesTest, Ledder, vector3i_t(0, 0, 0), vector3i_t(1, 1, 1))
END_CHECK_PRIMITIVE_TEST();

BEGIN_CHECK_PRIMITIVE_TEST(ObjectLibraryPrimitivesTest, Cilinder, vector3i_t(0, 0, 0), vector3i_t(1, 1, 1))
END_CHECK_PRIMITIVE_TEST();

BEGIN_CHECK_PRIMITIVE_TEST(ObjectLibraryPrimitivesTest, CilindricPlatform, vector3i_t(-1, 0, -1), vector3i_t(2, 1, 2))
END_CHECK_PRIMITIVE_TEST();

BEGIN_CHECK_PRIMITIVE_TEST(ObjectLibraryPrimitivesTest, Sphere, vector3i_t(0, 0, 0), vector3i_t(1, 1, 1))
END_CHECK_PRIMITIVE_TEST();

class testConstruction : public IConstructable
{
public:
    testConstruction() {};
    virtual ~testConstruction() {};

    virtual const ConstructionDescription& ConstructionDesc() const {return m_desc;}

    ConstructionDescription m_desc;

};

class ObjectLibraryTest : public ::testing::Test
{
public:
    ObjectLibraryTest() {};
	~ObjectLibraryTest() { Constructor::Destroy(m_constructor); }

    void SetUp()
    {
        ASSERT_TRUE(nullptr != m_constructor->GetLibrary().GetConstructionByName("Cube"));
    }
    void TearDown()
    {
    }

    Constructor* m_constructor = Constructor::Create();
};

TEST_F(ObjectLibraryTest, Registration)
{
    std::string testName = "testObject";
    {
        IConstructorObjectPtr test_obj(new ConstructorObjectBase(testName));
        ASSERT_EQ(Status::OK, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
    }
    const IConstructorObject* desc = m_constructor->GetLibrary().GetObjectByName(testName);
    ASSERT_TRUE(desc != nullptr);
    ASSERT_STREQ(testName.c_str(), desc->GetName().c_str());
}

TEST_F(ObjectLibraryTest, RegisterationWithElement)
{
    std::string testName = "testObject";
    {
        ObjectProperties newComplexObject = {testName, "", "", "Cube"};
        IConstructorObjectPtr test_obj(new ConstructorObjectBase(newComplexObject));
        ASSERT_EQ(Status::OK, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
    }
    const IConstructorObject* desc = m_constructor->GetLibrary().GetObjectByName(testName);
    ASSERT_TRUE(desc != nullptr);
    ASSERT_STREQ(testName.c_str(), desc->GetName().c_str());
}

TEST_F(ObjectLibraryTest, Reset)
{
    std::string testName = "testObject";
    {
        IConstructorObjectPtr test_obj(new ConstructorObjectBase(testName));
        m_constructor->GetLibrary().RegisterObject(testName, test_obj);
    }
    const IConstructorObject* desc = m_constructor->GetLibrary().GetObjectByName(testName);
    ASSERT_EQ(Status::OK, m_constructor->GetLibrary().CheckObjectStatus(testName));
    ASSERT_TRUE(desc != nullptr);
    m_constructor->Reset();

    ASSERT_EQ(Status::ResourceNotFound, m_constructor->GetLibrary().CheckObjectStatus(testName));
    desc = m_constructor->GetLibrary().GetObjectByName(testName);
    ASSERT_TRUE(desc == nullptr);
}

TEST_F(ObjectLibraryTest, DoubleElementRegistration)
{
    IConstructablePtr ptr(new testConstruction());
    //ptr.reset(new testConstruction());
    ASSERT_EQ(Status::AlreadyExists, m_constructor->GetLibrary().RegisterConstruction("Cube", ptr));
}

TEST_F(ObjectLibraryTest, DoubleObjectRegistration)
{
    std::string testName = "testObject";

    IConstructorObjectPtr test_obj(new ConstructorObjectBase(testName));
    m_constructor->GetLibrary().RegisterObject(testName, test_obj);

    ASSERT_EQ(Status::AlreadyExists, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
}

TEST_F(ObjectLibraryTest, DoubleObjectRegistrationPending)
{
    std::string testName = "testObject";

    IConstructorObjectPtr test_obj(new ConstructorObjectBase(testName));
    ASSERT_EQ(Status::OK, m_constructor->GetLibrary().RegisterObject(testName, test_obj));

    ObjectProperties newComplexObject = {testName, "SomeMesh", "SomeMaterial", "SomeElement"};
    test_obj.reset(new ConstructorObjectBase(newComplexObject));
    ASSERT_EQ(Status::AlreadyExists, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
}

TEST_F(ObjectLibraryTest, PendingRegistration)
{
    std::string testName = "testObject";

    ObjectProperties newComplexObject = {testName, "SomeMesh", "SomeMaterial", "SomeElement"};
    IConstructorObjectPtr test_obj(new ConstructorObjectBase(newComplexObject));

    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().CheckObjectStatus(testName));
    ASSERT_TRUE(nullptr == m_constructor->GetLibrary().GetObjectByName(testName));
}

TEST_F(ObjectLibraryTest, DoublePendingRegistration)
{
    std::string testName = "testObject";

    ObjectProperties newComplexObject = {testName, "SomeMesh", "SomeMaterial", "SomeElement"};
    IConstructorObjectPtr test_obj(new ConstructorObjectBase(newComplexObject));

    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
    ASSERT_EQ(Status::AlreadyExists, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
}

TEST_F(ObjectLibraryTest, PendingRegistrationMovesOnline)
{
    std::string testName = "testObject";

    ObjectProperties newComplexObject = {testName, "", "", "SomeElement"};
    IConstructorObjectPtr test_obj(new ConstructorObjectBase(newComplexObject));

    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().CheckObjectStatus(testName));

    IConstructablePtr ptr(new testConstruction());
    ASSERT_EQ(Status::OK, m_constructor->GetLibrary().RegisterConstruction("SomeElement", ptr));
    ASSERT_EQ(Status::OK, m_constructor->GetLibrary().CheckObjectStatus(testName));
}

TEST_F(ObjectLibraryTest, PendingRegistrationMultipleConstructionWaiters)
{
    std::string testName = "testObject";

    ASSERT_TRUE(nullptr == m_constructor->GetLibrary().GetConstructionByName("SomeElement"));

    ObjectProperties newComplexObject = {testName, "", "", "SomeElement"};
    IConstructorObjectPtr test_obj(new ConstructorObjectBase(newComplexObject));

    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().RegisterObject(testName, test_obj));
    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().CheckObjectStatus(testName));

    std::string SecondTestName = testName + "1";
    newComplexObject.name = SecondTestName;
    test_obj.reset(new ConstructorObjectBase(newComplexObject));
    ASSERT_EQ(Status::Pending, m_constructor->GetLibrary().RegisterObject(SecondTestName, test_obj));

    IConstructablePtr ptr(new testConstruction());
    ASSERT_EQ(Status::OK, m_constructor->GetLibrary().RegisterConstruction("SomeElement", ptr));
    ASSERT_EQ(Status::OK, m_constructor->GetLibrary().CheckObjectStatus(testName));
    ASSERT_EQ(Status::OK, m_constructor->GetLibrary().CheckObjectStatus(SecondTestName));
}
// eof