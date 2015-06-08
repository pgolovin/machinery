#include "QuadTree.h"
#include <gtest/gtest.h>
#include <memory>
#include <map>

using namespace Utils;

class QuadTreeTest : public ::testing::Test
{
public:
    static const size_t c_size = 512;
    void SetUp()
    {
        m_tree.reset(new QuadTree<int>(c_size));
    }
    void TearDown()
    {
        m_tree.reset();
    }
protected:
    std::unique_ptr< QuadTree<int> > m_tree;
};

TEST_F(QuadTreeTest, EmptyItem)
{
    ASSERT_TRUE( nullptr == m_tree->get_item_at(254, 400) );
}

TEST_F(QuadTreeTest, Insert)
{
    ASSERT_NO_THROW( m_tree->insert(254, 400, 10) );
}

TEST_F(QuadTreeTest, InsertedItem)
{
    ASSERT_NO_THROW( m_tree->insert(254, 400, 10) );
    ASSERT_TRUE( nullptr == m_tree->get_item_at(400, 254) );
    ASSERT_EQ(10, *m_tree->get_item_at(254, 400));
}

TEST_F(QuadTreeTest, GetAndCreateItem)
{
    ASSERT_NO_THROW( m_tree->item(254, 400) = 10 );
    ASSERT_TRUE( nullptr == m_tree->get_item_at(400, 254) );
    ASSERT_EQ(10, *m_tree->get_item_at(254, 400));
}

TEST_F(QuadTreeTest, GetAndRewriteItem)
{
    int i = 0;
    for (; i < 100000; ++i)
    {
        m_tree->item(254, 400) = i;
    }
    ASSERT_EQ(i - 1, m_tree->item(254, 400));
}

TEST_F(QuadTreeTest, RewriteItem)
{
    m_tree->insert(254, 400, 10);
    ASSERT_EQ(10, *m_tree->get_item_at(254, 400));
    ASSERT_EQ(10, *m_tree->get_item_at(254, 400));
    m_tree->insert(254, 400, 50);
    ASSERT_EQ(50, *m_tree->get_item_at(254, 400));
}


TEST_F(QuadTreeTest, MultipleRewriteItem)
{
    int i = 0;
    for (; i < 100000; ++i)
    {
        m_tree->insert(254, 400, i);
    }
    ASSERT_EQ(i - 1, *m_tree->get_item_at(254, 400));
}

TEST_F(QuadTreeTest, RemoveItem)
{
    m_tree->insert(254, 400, 10);
    ASSERT_EQ(10, *m_tree->get_item_at(254, 400));
    ASSERT_NO_THROW( m_tree->remove(254, 400) );
    ASSERT_TRUE( nullptr == m_tree->get_item_at(254, 400) );
}

TEST_F(QuadTreeTest, RemoveItems)
{
    m_tree->insert(254, 400, 10);
    m_tree->insert(254, 401, 15);
    ASSERT_NO_THROW( m_tree->remove(254, 400) );
    ASSERT_TRUE( nullptr == m_tree->get_item_at(254, 400) );
    ASSERT_TRUE( nullptr != m_tree->get_item_at(254, 401) );
    ASSERT_EQ( 15, *m_tree->get_item_at(254, 401) );
}

// bounding rect
TEST_F(QuadTreeTest, BoundingRectSingleItem)
{
    for (size_t i = 0; i < 100; ++i)
    {
        size_t X = rand() % c_size;
        size_t Y = rand() % c_size;
        m_tree->item(X, Y) = 10;
        ASSERT_EQ(X, m_tree->left()) << "failed for X = " << X;
        ASSERT_EQ(Y, m_tree->top()) << "failed for Y = " << Y;
        ASSERT_EQ(X+1, m_tree->right()) << "failed for X = " << X;
        ASSERT_EQ(Y+1, m_tree->bottom()) << "failed for Y = " << Y;
        m_tree->remove(X, Y);
    }
}

// bounding rect
TEST_F(QuadTreeTest, BoundingRectDiagonale)
{
    m_tree->item(0, 0) = 10;
    m_tree->item(1, 2) = 10;
    m_tree->item(2, 0) = 10;
    EXPECT_EQ(0, m_tree->left());
    EXPECT_EQ(0, m_tree->top());
    EXPECT_EQ(3, m_tree->right());
    EXPECT_EQ(3, m_tree->bottom());

}

TEST_F(QuadTreeTest, BoundingRectSameVerticalSlice)
{
    for (size_t i = 2; i < c_size; ++i)
        m_tree->item(2, i) = 10;
    for (size_t i = 2; i < c_size; ++i)
        m_tree->item(i, 2) = 10;
    m_tree->item(c_size - 1, 1) = 10;
    m_tree->item(1, c_size - 1) = 10;

    EXPECT_EQ(1, m_tree->left()) << "the worst case: left is on the bottom";
    EXPECT_EQ(1, m_tree->top()) << "the worst case: top is on the right";
}

TEST_F(QuadTreeTest, ForEachCoordinates)
{
    for (size_t i = 0; i < 100; ++i)
    {
        size_t X, Y;
        size_t x = rand()%c_size, y = rand()%c_size;
        m_tree->item(x, y) = i;
        int V;
        m_tree->for_each
        (
            [&](size_t x, size_t y, int& v)
            { X = x; Y = y, V = v; }
        );
        m_tree->remove(x, y);

        ASSERT_EQ(x, X);
        ASSERT_EQ(y, Y);
        ASSERT_EQ(i, V);
    }
}

TEST_F(QuadTreeTest, ForEachItems)
{
    for (size_t i = 0; i < c_size; ++i)
        m_tree->item(i, i) = 10;

    int count = 0;
    m_tree->for_each([&](size_t, size_t, int&){++count;});

    EXPECT_EQ(c_size, count) << "for each should be called for all non-null items";
}

TEST_F(QuadTreeTest, ForEachUpdateItems)
{
    for (size_t i = 0; i < c_size; ++i)
        m_tree->item(i, i) = 10;

    m_tree->for_each([&](size_t, size_t, int& v){v = 100;});

    for (size_t i = 0; i < c_size; ++i)
        ASSERT_EQ(100, *m_tree->get_item_at(i, i)) << "update for_each element failed on [" << i << "," << i << "]th";
}

class QuadTreeBenchmark : public ::testing::Test
{
public:
    static void SetUpTestCase()
    {
        m_tree.reset(new QuadTree<int>(512));
        x.resize(iterrations);
        y.resize(iterrations);
    }
    static void TearDownTestCase()
    {
        m_tree.reset();
    }
protected:
    static const size_t iterrations = 1000000;
    static std::vector<size_t> x;
    static std::vector<size_t> y;
    static std::map<int, int> v;
    static std::unique_ptr< QuadTree<int> > m_tree;
};

std::vector<size_t> QuadTreeBenchmark::x;
std::vector<size_t> QuadTreeBenchmark::y;
std::map<int, int> QuadTreeBenchmark::v;
std::unique_ptr< QuadTree<int> > QuadTreeBenchmark::m_tree;

TEST_F(QuadTreeBenchmark, InsertPerformance)
{
    for (unsigned int i = 0; i < iterrations; ++i)
    {
        size_t X= rand() % 512;
        size_t Y = rand() % 512;
        int V = rand() % 512;
        x[i] = X;
        y[i] = Y;
        m_tree->insert(X, Y, V);
    }
}

TEST_F(QuadTreeBenchmark, GetPerformance)
{
    for (unsigned int i = 0; i < iterrations; ++i)
    {
        m_tree->get_item_at(x[i], y[i]);
    }
}

TEST_F(QuadTreeBenchmark, RemovePerformance)
{
    for (unsigned int i = 0; i < iterrations; ++i)
    {
        m_tree->remove(x[i], y[i]);
    }
}

TEST_F(QuadTreeBenchmark, QuadTreeCorrectness)
{
    size_t targetX = rand() % 512;
    size_t targetY = rand() % 512;
    int targetValue = rand() % 512;
    m_tree->insert(targetX, targetY, targetValue);

    for (unsigned int i = 0; i < iterrations; ++i)
    {
        size_t X = rand() % 512;
        size_t Y = rand() % 512;
        int V = rand() % 512;
        if (X == targetX && Y == targetY)
        {
            targetValue = V;
        }
        m_tree->insert(X, Y, V);
        ASSERT_EQ(V, *m_tree->get_item_at(X, Y));
    }

    ASSERT_EQ(targetValue, *m_tree->get_item_at(targetX, targetY));
}

TEST_F(QuadTreeBenchmark, AllOperations)
{
    for (unsigned int i = 0; i < iterrations; ++i)
    {
        size_t X = rand() % 512;
        size_t Y = rand() % 512;
        int V = rand() % 512;

        int operation = rand() % 4;
        switch(operation)
        {
            case 0: m_tree->insert(X, Y, V); break;
            case 1: m_tree->item(X, Y) = V; break;
            case 2: m_tree->get_item_at(X, Y); break;
            default: m_tree->remove(X, Y);
        }
    }
}
// eof