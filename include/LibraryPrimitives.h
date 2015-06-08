/////////////////////////////////////////////////////////////////////////////////
//
// class ObjectConstructor generates object from primitives and modules. 
//    the class doesn't generate mesh
//    
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <Resources.h>
#include "MathBasics.h"
#include <vector>
#include <cstdint>

/////////////////////////////////////////////////////////////////////
///
/// Library implementation of Mesh object
///
/////////////////////////////////////////////////////////////////////

struct MeshProperties
{
    uint32_t    flags;
    vector3f_t  offset;
    uint32_t    orientation;
};

struct ILibraryMesh : public IMesh
{
    virtual void ConstructGeometry(const MeshProperties& properties, IMesh::Shape& out_descriptor) const = 0;
    virtual ~ILibraryMesh() {};
};

typedef std::shared_ptr<ILibraryMesh> ILibraryMeshPtr;

/////////////////////////////////////////////////////////////////////
///
/// Constructor object basic implementation
///
/////////////////////////////////////////////////////////////////////

class ConstructorObjectBase
    : public IConstructorObject
{
public:
    ConstructorObjectBase(std::string name) : m_name(name), m_constructionID(0) {}
    ConstructorObjectBase(ObjectProperties& objProperties) : m_name(objProperties.name), m_constructionID(0), m_properties(objProperties){;}

    virtual const       ObjectProperties& GetObjectContent()   const {return m_properties;};
    virtual const       ObjectResources&  GetObjectResources() const {return m_linkedResources;};
    virtual void        SetConstructionId(uint32_t id)               {m_constructionID = id;}
    virtual uint32_t    GetConstructionId() const                    {return m_constructionID;}

    const std::string& GetName() const {return m_name;}
private:
    bool m_isComplete;
    std::string m_name;
    uint32_t m_constructionID;

    ObjectProperties m_properties;
    ObjectResources  m_linkedResources;

    PREVENT_COPY(ConstructorObjectBase)
};


// eof