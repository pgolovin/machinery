#include "Library.h"
#include <memory>
#include "HandleImpl.h"

///////////////////////////////////////////////////////////////////////////////////
// base class for all primitive meshes
///////////////////////////////////////////////////////////////////////////////////
class BaseMesh : public ILibraryMesh
{
public:
    BaseMesh() { }
    const IMesh::Desc& GetDesc() const override
    {
        return m_desc;
    }

    virtual void ConstructGeometry(const MeshProperties& properties, IMesh::Shape& out_descriptor) const
    {
        properties;
        out_descriptor.Positions.Data.clear();
    }

    virtual void Release() {};

protected:
    void rotate(const float* in, uint32_t dst, const vector3f_t& offset, float* out) const
    {
        memcpy(out, in, sizeof(float) * 3);

        switch(dst & MODIFICATOR_MASK)
        {
        case Directions::LeftToRight:
            out[0] = 1 - out[0];
            break;
        }
        
        float tmp = out[0];
        switch(dst & DIRECTION_MASK)
        {
        case Directions::nX :
            out[0] = 1 - out[2];
            out[2] = tmp;
            break;
        case Directions::pX :
            out[0] = out[2];
            out[2] = 1 - tmp;
            break;
        case Directions::nZ : 
            out[0] = 1 - out[0];
            out[2] = 1 - out[2];
            break;
        default:
            break;
        }
        out[0] += offset.x; out[1] += offset.y; out[2] += offset.z;
    }

    void copyTriangles(IMesh::Shape& out_descriptor, const vector3f_t& offset, uint32_t orientation, const index_t* vertexIndices, const index_t* normalIndices, size_t size) const
    {
        // triangles supported only. for now
        assert(0 == size % 3);
        std::vector<float>& vertices = out_descriptor.Positions.Data;
        std::vector<float>& normals = out_descriptor.Normals.Data;
        vertices.reserve(vertices.size() + size * 3);
        normals.reserve(normals.size() + size * 3);

        const int directIndexOrder[3] = {0, 1, 2};
        const int morrorIndexOrder[3] = {1, 0, 2};
        const int *indexOrder = (orientation & MODIFICATOR_MASK) ? morrorIndexOrder : directIndexOrder;

        // work with vertex triplets
        for (size_t i = 0; i < size; i += 3)
        {
            float outVertex[9] = {};
            rotate(&m_vertices[ vertexIndices[i + indexOrder[0]] * 3], orientation, offset, outVertex);
            rotate(&m_vertices[ vertexIndices[i + indexOrder[1]] * 3], orientation, offset, outVertex + 3);
            rotate(&m_vertices[ vertexIndices[i + indexOrder[2]] * 3], orientation, offset, outVertex + 6);
            vertices.insert(vertices.end(), outVertex, outVertex + 9);

            float outNormals[9] = {};
            rotate(&m_normals[ normalIndices[i + indexOrder[0]] * 3], orientation, vector3f_t(0,0,0), outNormals);
            rotate(&m_normals[ normalIndices[i + indexOrder[1]] * 3], orientation, vector3f_t(0,0,0), outNormals + 3);
            rotate(&m_normals[ normalIndices[i + indexOrder[2]] * 3], orientation, vector3f_t(0,0,0), outNormals + 6);
            normals.insert(normals.end(), outNormals, outNormals + 9);
        }
    }

    std::vector<float>      m_vertices;
    std::vector<float>      m_normals;
    IMesh::Desc             m_desc;
};
///////////////////////////////////////////////////////////////////////////////////
// Space mesh, this mesh will be provided as dummy mesh object
///////////////////////////////////////////////////////////////////////////////////
class mesh_Space : public BaseMesh
{
public:
    mesh_Space() {};
    virtual IMeshPtr Clone() const 
    {
        CLONE_HANDLE(IMesh, mesh_Space);
    };
};

///////////////////////////////////////////////////////////////////////////////////
// Simple Cube mesh
///////////////////////////////////////////////////////////////////////////////////
class mesh_Cube : public BaseMesh
{
public:
    mesh_Cube()
    {
        const float vertices[] = 
        {//     vertex         normal    texcoord
        //     front face
            0.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
        //     back face
            1.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
        };
        const float normals[] = 
        {
            1, 0, 0,
            0, 1, 0,
            0, 0, 1,
            -1, 0, 0,
            0, -1, 0,
            0, 0, -1,
        };
        m_vertices.assign(vertices, vertices + sizeof(vertices)/sizeof(float));
        m_normals.assign(normals, normals + sizeof(normals)/sizeof(float));
    }

    virtual IMeshPtr Clone() const 
    {
        CLONE_HANDLE(IMesh, mesh_Cube);
    };

    virtual void ConstructGeometry(const MeshProperties& properties, IMesh::Shape& out_descriptor) const
    {
        const size_t groupsCount = 6;
        const index_t indexGroups[groupsCount][groupsCount] = 
        {
            {3, 2, 4, 4, 2, 5,}, // front  +x
            {6, 1, 4, 4, 1, 3,}, // top    +y
            {1, 0, 2, 1, 2, 3,}, // right  +z
            {0, 1, 7, 7, 1, 6,}, // back   -x
            {2, 0, 7, 5, 2, 7,}, // bottom -y
            {5, 7, 4, 4, 7, 6,}, // left   -z
        };

        const index_t normalIndices[groupsCount][groupsCount] = 
        {
            {0, 0, 0, 0, 0, 0,}, // front  +x
            {1, 1, 1, 1, 1, 1,}, // top    +y
            {2, 2, 2, 2, 2, 2,}, // right  +z
            {3, 3, 3, 3, 3, 3,}, // back   -x
            {4, 4, 4, 4, 4, 4,}, // bottom -y
            {5, 5, 5, 5, 5, 5,}, // left   -z
        };

        for (size_t i = 0; i < 6; ++i )
        {
            if (properties.flags & (1 << i))
            {
                copyTriangles(out_descriptor, properties.offset, properties.orientation, indexGroups[i], normalIndices[i], sizeof(indexGroups[i])/sizeof(index_t)); 
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////
// Simple Wedge mesh
///////////////////////////////////////////////////////////////////////////////////
class mesh_Wedge : public BaseMesh
{
public:
    mesh_Wedge()
    {
        float vertices[] = 
        {//     vertex         normal    texcoord
        //     front edge
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
        //     back face
            1.0f, 1.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
        };
        const float normals[] = 
        {
            0.0f, 0.0f, 1.0f,
            0.0f, 0.707106f, 0.707106f,
            0.0f, 0.0f, -1.0f,
            0.0f, -1.0f, 0.0f,
            -1.0f, 0.0f, 0.0f,
        };

        m_vertices.assign(vertices, vertices + sizeof(vertices)/sizeof(float));
        m_normals.assign(normals, normals + sizeof(normals)/sizeof(float));
    }

    virtual IMeshPtr Clone() const 
    {
        CLONE_HANDLE(IMesh, mesh_Wedge);
    };

    virtual void ConstructGeometry(const MeshProperties& properties, IMesh::Shape& out_descriptor) const
    {
        uint32_t flags = properties.flags;

        const index_t indexGroups[] = 
        {
            5, 0, 4,          // front  +x
            2, 0, 1, 4, 0, 2, // top    +y
                              //        +z
            2, 1, 3,          // back   -x
            1, 0, 5, 3, 1, 5, // bottom -y
            3, 5, 2, 2, 5, 4, // left   -z
        };

        const index_t normalGroups[] = 
        {
            0, 0, 0,          // front  +x
            1, 1, 1, 1, 1, 1, // top    +y
                              //        +z
            2, 2, 2,          // back   -x
            3, 3, 3, 3, 3, 3, // bottom -y
            4, 4, 4, 4, 4, 4, // left   -z
        };

        // pY, pZ
        if (flags & Directions::pX || flags & (size_t)Directions::nX || flags & (size_t)Directions::pZ || flags & (size_t)Directions::pY)
        {
            copyTriangles(out_descriptor, properties.offset, properties.orientation, indexGroups + 3, normalGroups + 3, 6);
        }

        const int groups[] = {9,  3,  3,  0, 12, 18};
        const int sizes[]  = {3,  6,  6,  3,  6,  6};

        for (size_t i = 0; i < 6; ++i )
        {
            if (flags & (1 << i) && i != DirectionIndices::pY_idx && i != DirectionIndices::pZ_idx)
            {
                copyTriangles(out_descriptor, properties.offset, properties.orientation, indexGroups + groups[i], normalGroups + groups[i], sizes[i]);
            }
        }
    }
};

///////////////////////////////////////////////////////////////////////////////////
// Wedge angle mesh
///////////////////////////////////////////////////////////////////////////////////
class mesh_WedgeOutCorner : public BaseMesh
{
public:
    mesh_WedgeOutCorner()
    {
        float vertices[] = 
        {//     vertex         normal    texcoord
        //     front edge
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f,

            1.0f, 1.0f, 0.0f,
        };

        float normals[] = 
        {//     vertex         normal    texcoord
        //     front edge
            1.0f, 0.0f, 0.0f,
            0.000000f, 0.707107f, 0.707107f,
            0.707107f, 0.707107f, 0.000000f,
            0.0f, 0.0f, 1.0f,
            0.0f, -1.0f, 0.0f,
        };

        m_vertices.assign(vertices, vertices + sizeof(vertices)/sizeof(float));
        m_normals.assign(normals, normals + sizeof(normals)/sizeof(float));
    }

    virtual IMeshPtr Clone() const 
    {
        CLONE_HANDLE(IMesh, mesh_WedgeOutCorner);
    };

    virtual void ConstructGeometry(const MeshProperties& properties, IMesh::Shape& out_descriptor) const
    {
        //uint32_t flags = properties.flags;

        // pyramid of outer angle is invisible only if all neighbours are available except nY
        //if (flags & (~Directions::nY))
        
        const index_t indexGroups[] = 
        {
            2, 4, 1,          // front  +x
            0, 4, 3, 1, 4, 0, // left   +y
            3, 4, 2,          // top    +z
               //               -x
            1, 0, 3, 2, 1, 3, // bottom -y
            //                  -z
        };
        const index_t normalGroups[] = 
        {
            0, 0, 0,          // front  +x
            2, 2, 2, 1, 1, 1, // left   +y
            3, 3, 3,          // top    +z
               //               -x
            4, 4, 4, 4, 4, 4, // bottom -y
            //                  -z
        };
        {
            copyTriangles(out_descriptor, properties.offset, properties.orientation, indexGroups, normalGroups, sizeof(indexGroups)/sizeof(index_t));
        }
        /*if (flags & Directions::pX)
        {
            copyTriangles(out_descriptor, properties.offset, properties.orientation, &m_indices[0], 3);
        }
        /*if (flags & Directions::pX)
        {
            copyTriangles(out_descriptor, properties.offset, properties.orientation, &m_indices[0], 3);
        }
        if (flags & Directions::nY)
        {
            copyTriangles(out_descriptor, properties.offset, properties.orientation, &m_indices[ 12 ], 6);
        }*/

        //copyTriangles(out_descriptor, properties.offset, properties.orientation, m_indices.data(), m_indices.size());
    }

};

class mesh_WedgeInCorner : public BaseMesh
{
public:
    mesh_WedgeInCorner()
    {
        float vertices[] = 
        {//     vertex         normal    texcoord
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
        };
        float normals[] = 
        {//     vertex         normal    texcoord
            1.0f,  0.0f,  0.0f,
            0.707107f, 0.707107f, 0.000000f,
            0.000000f, 0.707107f, 0.707107f,
            0.0f,  0.0f,  1.0f,
            -1.0f, 0.0f,  0.0f,
            0.0f,  -1.0f, 0.0f,
            0.0f,  0.0f, -1.0f,
        };

        m_vertices.assign(vertices, vertices + sizeof(vertices)/sizeof(float));
        m_normals.assign(normals, normals + sizeof(normals)/sizeof(float));
    }

    virtual IMeshPtr Clone() const 
    {
        CLONE_HANDLE(IMesh, mesh_WedgeInCorner);
    };

    virtual void ConstructGeometry(const MeshProperties& properties, IMesh::Shape& out_descriptor) const
    {
        const index_t indexGroups[] = 
        {
            4, 1, 6,            // +x
            1, 3, 2, 4, 3, 1,   // +y
            1, 2, 0,            // +z
            5, 0, 3, 3, 0, 2,   // -x
            5, 6, 0, 0, 6, 1,   // -y
            3, 4, 5, 5, 4, 6,   // -z
        };
        const index_t normalGroups[] = 
        {
            0, 0, 0,            // +x
            1, 1, 1, 2, 2, 2,   // +y
            3, 3, 3,            // +z
            4, 4, 4, 4, 4, 4,   // -x
            5, 5, 5, 5, 5, 5,   // -y
            6, 6, 6, 6, 6, 6,   // -z
        };
        copyTriangles(out_descriptor, properties.offset, properties.orientation, indexGroups, normalGroups, sizeof(indexGroups)/sizeof(index_t));
    }
};

///////////////////////////////////////////////////////////////////////////////////
// Common cilinder mesh
///////////////////////////////////////////////////////////////////////////////////
class mesh_Cilinder : public BaseMesh
{
public:
    mesh_Cilinder()
    {
        float vertices[] = 
        {//     vertex         normal    texcoord
            0.782843f, 0.f, 0.217157f,
            0.500000f, 0.f, 0.100000f,
            0.217157f, 0.f, 0.217157f,
            0.100000f, 0.f, 0.500000f,
            0.217157f, 0.f, 0.782843f,
            0.500000f, 0.f, 0.900000f,
            0.782843f, 0.f, 0.782843f,
            0.900000f, 0.f, 0.500000f,
            0.782843f, 1.f, 0.217157f,
            0.500000f, 1.f, 0.100000f,
            0.217157f, 1.f, 0.217157f,
            0.100000f, 1.f, 0.500000f,
            0.217157f, 1.f, 0.782843f,
            0.500000f, 1.f, 0.900000f,
            0.782843f, 1.f, 0.782843f,
            0.900000f, 1.f, 0.500000f,
        };
        float normals[] = 
        {
            0.797175f ,  0.000000f, -0.603748f,
            0.136774f ,  0.000000f, -0.990602f,
            0.603748f ,  0.000000f, -0.797175f,
            -0.136774f,  0.000000f, -0.990602f,
            -0.603748f,  0.000000f, -0.797175f,
            -0.797175f,  0.000000f, -0.603748f,
            -0.990602f,  0.000000f, -0.136774f,
            -0.990602f,  0.000000f, 0.136774f ,
            -0.797175f,  0.000000f, 0.603748f ,
            -0.603748f,  0.000000f, 0.797175f ,
            -0.136774f,  0.000000f, 0.990602f ,
            0.136774f ,  0.000000f, 0.990602f ,
            0.603748f ,  0.000000f, 0.797175f ,
            0.797175f ,  0.000000f, 0.603748f ,
            0.990602f ,  0.000000f, 0.136774f ,
            0.990602f ,  0.000000f, -0.136774f,
            0.000000f , -1.000000f, 0.000000f, //16
            0.000000f,   1.000000f, 0.000000f,   //24
        };
        m_vertices.assign(vertices, vertices + sizeof(vertices)/sizeof(float));
        m_normals.assign(normals, normals + sizeof(normals)/sizeof(float));
    }

    virtual IMeshPtr Clone() const 
    {
        CLONE_HANDLE(IMesh, mesh_Cilinder);
    };

    virtual void ConstructGeometry(const MeshProperties& properties, IMesh::Shape& out_descriptor) const
    {
        const index_t indexGroups[] = 
        {
            0,  1,  8,  8,  1,  9,
            1,  2,  9,  9,  2,  10,
            2,  3,  10, 10, 3,  11,
            3,  4,  11, 11, 4,  12,
            4,  5,  12, 12, 5,  13,
            5,  6,  13, 13, 6,  14,
            6,  7,  14, 14, 7,  15,
            7,  0,  15, 15, 0,  8,
            6,  5,  7,  5,  4,  7,
            4,  3,  7,  3,  2,  7,
            2,  1,  7,  1,  0,  7,
            15, 8,  14, 8,  9,  14,
            9,  10, 14, 10, 11, 14,
            11, 12, 14, 12, 13, 14,
        };

        const index_t normalGroups[] = 
        {
            0,  1,  2,  2,  1,  3,
            1,  4,  3,  3,  4,  5,
            4,  6,  5,  5,  6,  7,
            6,  8,  7,  7,  8,  9,
            8,  10, 9,  9,  10, 11,
            10, 12, 11, 11, 12, 13,
            12, 14, 13, 13, 14, 15,
            14, 0,  15, 15, 0,  2,
            16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16, 16,
            17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
        };

        copyTriangles(out_descriptor, properties.offset, properties.orientation, indexGroups, normalGroups, sizeof(indexGroups)/sizeof(index_t));
    }
};


#define REGISTER_MESH(PrimitiveType) library.RegisterSimpleMesh(#PrimitiveType, PrimitiveType, new mesh_##PrimitiveType())

static void RegisterDefaultMeshes(MeshLibrary& library)
{
    REGISTER_MESH(Space);
    REGISTER_MESH(Cube);
    REGISTER_MESH(Wedge);
    REGISTER_MESH(WedgeOutCorner);
    REGISTER_MESH(WedgeInCorner);
//    REGISTER_MESH(Ledder);
    REGISTER_MESH(Cilinder);
//    REGISTER_MESH(CilindricPlatform);
//    REGISTER_MESH(Sphere);
}
//eof