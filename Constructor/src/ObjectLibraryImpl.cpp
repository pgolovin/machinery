#include "ObjectLibraryImpl.h"
#include <memory>

using namespace ConstructorImpl;

ObjectLibrary::ObjectLibrary() 
{
}

void ObjectLibrary::Cleanup()
{
    m_primitives.clear();
}

Status ObjectLibrary::CheckObjectStatus(std::string name)
{
    auto primitive = m_primitives.find(name);
    return m_primitives.end() != primitive ? Status::OK : Status::ResourceNotFound;
}

const IConstructorObject* ObjectLibrary::GetObject(std::string name)
{
    auto object = m_primitives.find(name);
    if (object != m_primitives.end())
        return object->second.get();

    // try to load object from file;

    return nullptr;
}

Status ObjectLibrary::RegisterObject(std::string name, IConstructorObjectPtr & primitive)
{
    auto object = m_primitives.find(name);
    if (object != m_primitives.end())
        return Status::AlreadyExists;

    m_primitives[name] = primitive;
    return Status::OK;
}

//#include "ConstructionLibraryPrimitives.cpp"

// eof