#pragma once
#include <list>
#include <memory>
#include <functional>

namespace Utils
{
// MACRO!?! ugly, yes, but it help me to save about 5% of performance other than
// encapsulating the code to the function

#define _item_at(on_not_found, finalize)                            \
    Node* node      = & m_root;                                     \
    size_t width    = m_squareSide;                                 \
    while (width > 1)                                               \
    {                                                               \
        width >>= 1;                                                \
        size_t index = (!!(x & width)) + ((!!(y & width)) << 1);    \
        x &= ~width;                                                \
        y &= ~width;                                                \
        if ( !node->quadNodes[index] )                              \
        {                                                           \
            on_not_found;                                           \
        }                                                           \
        node = node->quadNodes[index].get();                        \
    }                                                               \
    finalize;

    // This quad tree is highly limited by it's usage
    // it works with square areas, the side is pow2
    template <class T>
    class QuadTree
    {
    public:
        QuadTree(size_t squareSide) : m_squareSide(squareSide) , m_treeDepth(1)
        {
            size_t depth = squareSide;
            while (depth>>= 1) ++m_treeDepth;
        }

        void insert(size_t x, size_t y, T value)
        {
            _item_at(node->quadNodes[index].reset(new Node()), node->value = value);
        }

        T* get_item_at(size_t x, size_t y)
        {
            _item_at(return nullptr, return &node->value);
        }

        T& item(size_t x, size_t y)
        {
            _item_at(node->quadNodes[index].reset(new Node()), return node->value);
        }

        void remove(size_t x, size_t y)
        {
            Node* node      = & m_root;
            size_t width    = m_squareSide;
            Node* lastFull  = & m_root;
            size_t targetIndex    = (!!(x & (m_squareSide >> 1))) + ((!!(y & (m_squareSide >> 1))) << 1);

            while (width > 1)
            {
                width >>= 1;

                // !!(A) convert any non-null number to 1
                size_t index = (!!(x & width)) + ((!!(y & width)) << 1);
                x &= ~width;
                y &= ~width;

                if ( !node->quadNodes[index] )
                {
                    return;
                }
                //Get number of non null nodes
                if (((!!node->quadNodes[0]) + (!!node->quadNodes[1]) + (!!node->quadNodes[2]) + (!!node->quadNodes[3])) > 1)
                {
                    lastFull = node;
                    targetIndex = index;
                }
                node = node->quadNodes[index].get();
                
            }
            lastFull->quadNodes[targetIndex].reset();
        }

        size_t left()
        {
            const size_t idxOrder[]   = {0, 2, 1, 3};
            size_t current = m_squareSide;
            return scan_nodes_lt(idxOrder, idxOrder + 2, &m_root, 0, m_squareSide, current);
        }

        size_t top()
        {
            const size_t primaryCheck[]   = {0, 1};
            const size_t secondaryCheck[] = {2, 3};
            size_t current = m_squareSide;
            return scan_nodes_lt(primaryCheck, secondaryCheck, &m_root, 0, m_squareSide, current);
        }

        size_t right()
        {
            const size_t primaryCheck[]   = {1, 3};
            const size_t secondaryCheck[] = {0, 2};
            size_t current = 0;
            return scan_nodes_rb(primaryCheck, secondaryCheck, &m_root, m_squareSide, m_squareSide, current);
        }

        size_t bottom()
        {
            const size_t primaryCheck[]   = {2, 3};
            const size_t secondaryCheck[] = {0, 1};
            size_t current = 0;
            return scan_nodes_rb(primaryCheck, secondaryCheck, &m_root, m_squareSide, m_squareSide, current);
        }

        void for_each(std::function<void(size_t, size_t, T&)> visitor)
        {
            _foreach(&m_root, 0, 0, m_squareSide, visitor);
        }

        void clear()
        {
            for (size_t i =0; i < 4; ++i)
            {
                m_root.quadNodes[i].reset();
            }
        }
    private:

        struct Node
        {
            // | 0 | 1 |
            // | 2 | 3 |
            std::unique_ptr<Node> quadNodes[4];
            T value;
        };

        size_t m_treeDepth;
        size_t m_squareSide;
        Node   m_root;

        QuadTree(const QuadTree&);
        const QuadTree& operator=(const QuadTree);
        size_t scan_nodes_lt(const size_t* pi, const size_t* si, const Node* n, size_t lt, size_t w, size_t &current)
        {
            w >>= 1;
            size_t retval = lt;
            if (w)
            {
                if ((n->quadNodes[ pi[0] ] || n->quadNodes[ pi[1] ]) && lt <= current)
                {
                    if (n->quadNodes[ pi[0] ]) retval = scan_nodes_lt(pi, si, n->quadNodes[ pi[0] ].get(), lt, w, current);
                    if (n->quadNodes[ pi[1] ]) retval = scan_nodes_lt(pi, si, n->quadNodes[ pi[1] ].get(), lt, w, current);
                }
                else if ((n->quadNodes[ si[0] ] || n->quadNodes[ si[1] ]) && (lt + w) <= current)
                {
                    lt += w;
                    if (n->quadNodes[ si[0] ]) retval = scan_nodes_lt(pi, si, n->quadNodes[ si[0] ].get(), lt, w, current);
                    if (n->quadNodes[ si[1] ]) retval = scan_nodes_lt(pi, si, n->quadNodes[ si[1] ].get(), lt, w, current);
                }
            }
            current = (retval < current) ? retval : current;
            return current;
        }

        size_t scan_nodes_rb(const size_t* pi, const size_t* si, const Node* n, size_t rb, size_t w, size_t &current)
        {
            w >>= 1;
            size_t retval = rb;
            if (w)
            {
                if ((n->quadNodes[ pi[0] ] || n->quadNodes[ pi[1] ]) && (rb + 1) >= current)
                {
                    if (n->quadNodes[ pi[0] ]) retval = scan_nodes_rb(pi, si, n->quadNodes[ pi[0] ].get(), rb, w, current);
                    if (n->quadNodes[ pi[1] ]) retval = scan_nodes_rb(pi, si, n->quadNodes[ pi[1] ].get(), rb, w, current);
                }
                else if ((n->quadNodes[ si[0] ] || n->quadNodes[ si[1] ]) && (rb + w + 1) >= current)
                {
                    rb -= w;
                    if (n->quadNodes[ si[0] ]) retval = scan_nodes_rb(pi, si, n->quadNodes[ si[0] ].get(), rb, w, current);
                    if (n->quadNodes[ si[1] ]) retval = scan_nodes_rb(pi, si, n->quadNodes[ si[1] ].get(), rb, w, current);
                }
            }
            current = (retval > current) ? retval : current;
            return current;
        }

        void _foreach(Node* n, size_t x, size_t y, size_t w, std::function<void(size_t, size_t, T&)> v)
        {
            w >>= 1;
            if (w)
            {
                if (n->quadNodes[0]) 
                    _foreach(n->quadNodes[0].get(), x, y, w, v);
                if (n->quadNodes[1]) 
                    _foreach(n->quadNodes[1].get(), x + w, y, w, v);
                if (n->quadNodes[2]) 
                    _foreach(n->quadNodes[2].get(), x, y + w, w, v);
                if (n->quadNodes[3]) 
                    _foreach(n->quadNodes[3].get(), x + w, y + w, w, v);
            }
            else
                v(x, y, n->value);
        }
    };

}
// eof