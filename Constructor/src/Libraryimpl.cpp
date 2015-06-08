#include "LibraryImpl.h"
#include <memory>

using namespace ConstructorImpl;

Library::Library() 
{
}

void Library::Reset()
{
    m_objectLibrary.Cleanup();
    m_constructionLibrary.Cleanup();
    m_pendingObjects.clear();
    m_constructionSubscribers.clear();
}

Status Library::NewObject(ObjectProperties& object)
{
    IConstructorObjectPtr obj(new ConstructorObjectBase(object));
    return RegisterObject(object.name, obj);
}

Status Library::RegisterConstruction(std::string name, IConstructablePtr& element)
{
    auto subscribers = m_constructionSubscribers.find(name);
    if (subscribers != m_constructionSubscribers.end())
    {
        for (auto& subscriber : subscribers->second)
        {
            std::string objName = subscriber->GetName();
            subscriber->SetConstructionId(element->ConstructionDesc().primitiveUID);
            m_objectLibrary.RegisterObject(objName, subscriber);
            m_pendingObjects.erase(objName);
        }
        m_constructionSubscribers.erase(subscribers);
    }
    return m_constructionLibrary.RegisterPrimitive(name, element);
}

const ConstructionDescription* Library::GetConstructionByName(std::string name)
{
    return m_constructionLibrary.GetConstructionDescription(name);
}

const ILibraryMesh& Library::GetMesh(unsigned int id)
{
    return m_meshLibrary.GetMeshObject(id);
}

void Library::RegisterMesh(unsigned int id, ILibraryMeshPtr& mesh)
{
    m_meshLibrary.RegisterMesh(id, mesh);
}

Status Library::CheckObjectStatus(std::string name)
{
    auto pending = m_pendingObjects.find(name);
    if (m_pendingObjects.end() != pending)
    {
        return Status::Pending;
    }
    return m_objectLibrary.CheckObjectStatus(name);
}

const IConstructorObject* Library::GetObjectByName(std::string name)
{
    return m_objectLibrary.GetObject(name);
}

Status Library::RegisterObject(std::string name, IConstructorObjectPtr & prototype)
{
    const ObjectProperties& properties = prototype->GetObjectContent();
    const ConstructionDescription* desc = m_constructionLibrary.GetConstructionDescription(properties.elementName);
    if (properties.elementName.empty() || desc)
    {
        prototype->SetConstructionId(desc ? desc->primitiveUID : ElementType::Space);
        return m_objectLibrary.RegisterObject(name, prototype);
    }

    if (Status::OK == m_objectLibrary.CheckObjectStatus(name) || m_pendingObjects.find(name) != m_pendingObjects.end())
    {
        return Status::AlreadyExists;
    }
    m_constructionSubscribers[properties.elementName].push_back(prototype);
    m_pendingObjects[name] = prototype;
    return Status::Pending;
}
// eof