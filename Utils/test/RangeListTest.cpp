#include "RangeList.h"
#include <gtest/gtest.h>
#include <memory>
#include <map>

using namespace Utils;

class RangeVectorTest : public ::testing::Test
{
public:
    void SetUp()
    {
        m_ranges.reset(new RangeList<int>());
    }
    void TearDown()
    {
        m_ranges.reset();
    }
protected:
    std::unique_ptr< RangeList<int> > m_ranges;
};

TEST_F(RangeVectorTest, EmptyItem)
{
    ASSERT_TRUE( nullptr == m_ranges->get_item_at(100) );
}

TEST_F(RangeVectorTest, AddItem)
{
    ASSERT_NO_THROW(m_ranges->insert(100, 10));
    ASSERT_EQ( 10, *m_ranges->get_item_at(100) );
}

TEST_F(RangeVectorTest, AddItems)
{
    ASSERT_NO_THROW(m_ranges->insert(100, 10));
    ASSERT_NO_THROW(m_ranges->insert(300, 30));
    ASSERT_EQ( 10, *m_ranges->get_item_at(100) );
    ASSERT_EQ( 30, *m_ranges->get_item_at(300) );
}

TEST_F(RangeVectorTest, AddBack)
{
    ASSERT_NO_THROW(m_ranges->insert(100, 10));
    ASSERT_NO_THROW(m_ranges->insert(101, 30));
    ASSERT_EQ( 1, m_ranges->ranges_count() );
}

TEST_F(RangeVectorTest, AddFront)
{
    ASSERT_NO_THROW(m_ranges->insert(100, 10));
    ASSERT_NO_THROW(m_ranges->insert(99, 30));
    ASSERT_EQ( 1, m_ranges->ranges_count() );
}

TEST_F(RangeVectorTest, AddMiddle)
{
    ASSERT_NO_THROW(m_ranges->insert(100, 10));
    ASSERT_NO_THROW(m_ranges->insert(98, 30));
    ASSERT_NO_THROW(m_ranges->insert(99, 30));
    ASSERT_EQ( 1, m_ranges->ranges_count() );
}


TEST_F(RangeVectorTest, RangesCount)
{
    m_ranges->insert(100, 10);
    m_ranges->insert(101, 11);
    m_ranges->insert(300, 30);
    ASSERT_EQ( 10, *m_ranges->get_item_at(100) );
    ASSERT_EQ( 11, *m_ranges->get_item_at(101) );
    ASSERT_EQ( 30, *m_ranges->get_item_at(300) );
    ASSERT_EQ( 2, m_ranges->ranges_count() );
}

TEST_F(RangeVectorTest, MergeRanges)
{
    m_ranges->insert(100, 10);
    m_ranges->insert(102, 11);
    m_ranges->insert(104, 11);
    ASSERT_EQ( 3, m_ranges->ranges_count() ) << "default ranges count incorrect";

    m_ranges->insert(101, 30);
    ASSERT_EQ( 2, m_ranges->ranges_count() ) << "merging first range";

    m_ranges->insert(103, 30);
    ASSERT_EQ( 1, m_ranges->ranges_count() ) << "merging second range";
}

TEST_F(RangeVectorTest, HalledArray_Add)
{
    for (int i = 9; i >= 0; --i)
    {
        for (size_t j = 1; j < 10; ++j)
        {
            m_ranges->insert(i*10 + j, i*10 + j);
        }
    }
    ASSERT_EQ(10, m_ranges->ranges_count());
    for (size_t i = 1; i < 10; ++i)
    {
        m_ranges->insert(i*10, 1);
        ASSERT_EQ(10 - i, m_ranges->ranges_count());
    }

    ASSERT_TRUE(nullptr == m_ranges->get_item_at(0));

    for (size_t i = 1; i < 100; ++i)
    {
        ASSERT_EQ(((i % 10) ? i : 1), *m_ranges->get_item_at(i));
    }
}

TEST_F(RangeVectorTest, RemoveItem)
{
    m_ranges->insert(100, 10);
    m_ranges->insert(101, 11);
    m_ranges->insert(102, 12);
    ASSERT_EQ( 1, m_ranges->ranges_count() ) << "default ranges count incorrect";

    m_ranges->remove(101);
    ASSERT_EQ( 2, m_ranges->ranges_count() ) << "remove should splitranges";

    ASSERT_TRUE(nullptr == m_ranges->get_item_at(101));
    ASSERT_TRUE(nullptr != m_ranges->get_item_at(102));
}

TEST_F(RangeVectorTest, HalledArray_Remove)
{
    for (int i = 9; i >= 0; --i)
    {
        for (size_t j = 0; j < 10; ++j)
        {
            m_ranges->insert(i*10 + j, i*10 + j);
        }
    }
    ASSERT_EQ(1, m_ranges->ranges_count());
    for (size_t i = 1; i < 10; ++i)
    {
        m_ranges->remove(i*10);
        ASSERT_EQ(i + 1, m_ranges->ranges_count());
    }

    for (size_t i = 1; i < 100; ++i)
    {
        if (i % 10)
            ASSERT_EQ(i, *m_ranges->get_item_at(i));
        else
            ASSERT_TRUE(nullptr == m_ranges->get_item_at(i));
    }
}

TEST_F(RangeVectorTest, Sizes)
{
    m_ranges->insert(100, 10);
    m_ranges->insert(150, 12);

    ASSERT_EQ( 151, m_ranges->size() );
    ASSERT_EQ( 100, m_ranges->start() );
}

TEST_F(RangeVectorTest, ForEachValue)
{
    m_ranges->insert(100, 10);
    int value = 0;
    m_ranges->for_each([&](size_t x, int v) {
        x; v;
        value = v;
    });
    ASSERT_EQ(10, value);
}

TEST_F(RangeVectorTest, ForEachItems)
{
    size_t reference = 0;
    for (int i = 9; i >= 0; --i)
    {
        for (size_t j = 1; j < 10; ++j)
        {
            m_ranges->insert(i*10 + j, i*10 + j);
            ++reference;
        }
    }
    ASSERT_EQ(10, m_ranges->ranges_count());

    size_t count = 0;
    m_ranges->for_each([&](size_t x, int v) {
        x; v;
        ++count;
    });
    ASSERT_EQ(reference, count);
}

TEST_F(RangeVectorTest, ForEachItemsUpdate)
{
    for (size_t i = 0; i < 100; ++i)
    {
        m_ranges->insert(i, 10);
    }

    m_ranges->for_each([&](size_t, int& v) {
        v = 100;
    });

    for (size_t i = 0; i < 100; ++i)
    {
        ASSERT_EQ(100, *m_ranges->get_item_at(i)) << "update for_each element failed on " << i << "th";
    }
}

// eof