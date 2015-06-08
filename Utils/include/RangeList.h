#pragma once
#include <list>
#include <vector>
#include <memory>
#include <functional>

namespace Utils
{
    template <class T>
    class RangeList
    {
    public:
        struct range_desc
        {
            size_t start;
            std::vector<T> items;
        };

    public:
        RangeList() {}
        ~RangeList() {}

        size_t ranges_count() {return m_rs.size();}

        // get item by index
        T* get_item_at(size_t index) 
        {
            auto range = m_rs.begin();
            for (; range != m_rs.end(); ++range)
            {
                if (range->start <= index && (index - range->start) < range->items.size())
                {
                    return &range->items[index - range->start];
                }
            }
            return nullptr; 
        }

        // insert item to the range list
        void insert(size_t index, T value) 
        {
            auto range = m_rs.begin();

            for (; range != m_rs.end(); ++range)
            {
                range_desc &desc = *range;
                size_t local_index = index - desc.start;
                if (range->start > index)
                    break;

                if (desc.start <= index && local_index <= desc.items.size())
                {
                    if (local_index == desc.items.size())
                    {
                        desc.items.push_back(value);
                        if (range != m_rs.end())
                        {
                            // mergetwo items if item is inserted between 2 neighbour ranges
                            auto next = range;
                            if (++next != m_rs.end() && desc.start + desc.items.size() == next->start)
                            {
                                desc.items.insert(desc.items.end(), next->items.begin(), next->items.end());
                                m_rs.erase(next);
                            }
                        }
                    }
                    else // replacevalueif we are in range
                        desc.items[local_index] = value;
                    return;
                }
            }
            // insert item to the beginning of the range
            if (m_rs.end() != range && index + 1 == range->start)
            {
                range->items.insert(range->items.begin(), value);
                --range->start;
            }
            else // add new range
            {
                range_desc desc;
                desc.start = index;
                desc.items.push_back(value);

                m_rs.insert(range, desc);
            }
        }

        void remove(size_t index)
        {
            auto range = m_rs.begin();

            for (; range != m_rs.end(); ++range)
            {
                range_desc &desc = *range;
                size_t local_index = index - desc.start;
                if (range->start > index)
                    break;

                if (desc.start <= index && local_index <= desc.items.size())
                {
                    range_desc splitted;
                    splitted.start = index + 1;
                    T* start = &desc.items[local_index + 1];
                    splitted.items.assign(start, start + desc.items.size() - local_index - 1);

                    m_rs.insert(++range, splitted);
                    desc.items.resize(local_index);

                    return;
                }
            }
        }

        size_t start() 
        {
            return m_rs.size() ? m_rs.front().start : 0;
        }

        size_t size() 
        {
            return m_rs.size() ? m_rs.back().start + m_rs.back().items.size() : 0;
        }

        void for_each(std::function<void(size_t, T&)> visitor)
        {
            for (auto& range : m_rs)
            {
                for (size_t index = 0; index != range.items.size(); ++index)
                {
                    visitor(range.start + index, range.items[index]);
                }
            }
        }
    private:
        //iterator m_iterator;
        std::list<range_desc> m_rs; //list of ranges
    };
}
// eof