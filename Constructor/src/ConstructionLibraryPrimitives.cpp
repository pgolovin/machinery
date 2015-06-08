
//define standard primitive class.

#define BEGIN_PRIMITIVE_DEFINITION(PrimitiveType, BBOX)                                             \
class class_##PrimitiveType : public IConstructable                                                 \
{                                                                                                   \
public:                                                                                             \
    ConstructionDescription m_desc;                                                                 \
    const ConstructionDescription& ConstructionDesc() const {return m_desc;}                        \
                                                                                                    \
    class_##PrimitiveType()                                                                         \
    {                                                                                               \
        m_desc.primitiveUID = ElementType::##PrimitiveType;                                         \
        m_desc.boundingBox = (BBOX);

#define BEGIN_NEIGHBORS_LIST(count)                                                                 \
        const NeighborDesc neighbors[(count)] = {

#define END_NEIGHBORS_LIST(count)                                                                   \
        };                                                                                          \
        m_desc.neighbors.assign(neighbors, neighbors + count);

#define END_PRIMITIVE_DEFINITION(PrimitiveType)                                                     \
    }                                                                                               \
    virtual ~class_##PrimitiveType() {};                                                            \
};


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

BEGIN_PRIMITIVE_DEFINITION(Space,               BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
END_PRIMITIVE_DEFINITION(Space);

BEGIN_PRIMITIVE_DEFINITION(Cube,                BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(1,0,0),  Directions::pX), 
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,1,0),  Directions::pY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(-1,0,0), Directions::nX),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,-1,0), Directions::nY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(Cube);

BEGIN_PRIMITIVE_DEFINITION(Wedge,               BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(5,                         vector3i_t(1,0,0),  Directions::pX), 
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(0,1,0),  Directions::pY),
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(5,                         vector3i_t(-1,0,0), Directions::nX),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,-1,0), Directions::nY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(Wedge);

BEGIN_PRIMITIVE_DEFINITION(WedgeOutCorner,      BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(1,0,0),  Directions::pX), 
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(0,1,0),  Directions::pY),
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(5,                         vector3i_t(-1,0,0), Directions::nX),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,-1,0), Directions::nY),
        NeighborDesc(5,                         vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(WedgeOutCorner);

BEGIN_PRIMITIVE_DEFINITION(WedgeInCorner,       BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(5,                         vector3i_t(1,0,0),  Directions::pX), 
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(0,1,0),  Directions::pY),
        NeighborDesc(5,                         vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(-1,0,0), Directions::nX),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,-1,0), Directions::nY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(WedgeInCorner);

BEGIN_PRIMITIVE_DEFINITION(Ledder,              BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(6, vector3i_t(1,0,0),                          Directions::pX), 
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(0,1,0),  Directions::pY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(6, vector3i_t(-1,0,0),                         Directions::nX),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,-1,0), Directions::nY),
        NeighborDesc(Influences::NOT_AFFECTED,  vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(Ledder);

BEGIN_PRIMITIVE_DEFINITION(Cilinder,            BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(1,0,0),  Directions::pX), 
        NeighborDesc(8, vector3i_t(0,1,0),                          Directions::pY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(-1,0,0), Directions::nX),
        NeighborDesc(8, vector3i_t(0,-1,0),                         Directions::nY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(Cilinder);

BEGIN_PRIMITIVE_DEFINITION(CilindricPlatform,   BBox(vector3i_t(-1, 0, -1), vector3i_t(2, 1, 2)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(1,0,0),  Directions::pX), 
        NeighborDesc(8, vector3i_t(0,1,0),                          Directions::pY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(-1,0,0), Directions::nX),
        NeighborDesc(8, vector3i_t(0,-1,0),                         Directions::nY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(CilindricPlatform);

BEGIN_PRIMITIVE_DEFINITION(Sphere,              BBox(vector3i_t(0, 0, 0), vector3i_t(1, 1, 1)))
    BEGIN_NEIGHBORS_LIST(6)
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(1,0,0),  Directions::pX),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,1,0),  Directions::pY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,1),  Directions::pZ),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(-1,0,0), Directions::nX),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,-1,0), Directions::nY),
        NeighborDesc(Influences::FULLY_COVERED, vector3i_t(0,0,-1), Directions::nZ),
    END_NEIGHBORS_LIST(6);
END_PRIMITIVE_DEFINITION(Sphere);

#define REGISTER_PRIMITIVE(PrimitiveType) library.RegisterSimplePrimitive(#PrimitiveType, new class_##PrimitiveType())

static void RegisterDefaultConstructions(ConstructionLibrary& library)
{
    REGISTER_PRIMITIVE(Space);
    REGISTER_PRIMITIVE(Cube);
    REGISTER_PRIMITIVE(Wedge);
    REGISTER_PRIMITIVE(WedgeOutCorner);
    REGISTER_PRIMITIVE(WedgeInCorner);
    REGISTER_PRIMITIVE(Ledder);
    REGISTER_PRIMITIVE(Cilinder);
    REGISTER_PRIMITIVE(CilindricPlatform);
    REGISTER_PRIMITIVE(Sphere);
}
// eof
