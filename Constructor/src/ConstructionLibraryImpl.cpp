#include "ConstructionLibraryImpl.h"
#include <memory>

using namespace ConstructorImpl;

ConstructionLibrary::ConstructionLibrary() 
{
    m_primitives.resize(ElementType::SimplePrimitivesCount);
}

void ConstructionLibrary::Cleanup() 
{
    m_primitiveNameIdMap.clear(); 
    m_primitives.clear();
    m_primitives.resize(ElementType::SimplePrimitivesCount);
}

Status ConstructionLibrary::RegisterPrimitive(std::string name, IConstructablePtr& element)
{
    if (m_primitiveNameIdMap.find(name) != m_primitiveNameIdMap.end())
        return Status::AlreadyExists;

    size_t id = m_primitives.size();
    m_primitives.push_back(element);
    m_primitiveNameIdMap[name] = id;

    return Status::OK;
}

void ConstructionLibrary::RegisterSimplePrimitive(std::string name, IConstructable* element)
{
    size_t id = element->ConstructionDesc().primitiveUID;
    assert(id < ElementType::SimplePrimitivesCount);
    m_primitives[id].reset(element);
    m_primitiveNameIdMap[name] = id;
}

const uint32_t ConstructionLibrary::GetConstructionId(std::string& name) const
{
    auto found = m_primitiveNameIdMap.find(name);
    return found != m_primitiveNameIdMap.end() ? found->second : ~0x0;
}

const ConstructionDescription* ConstructionLibrary::GetConstructionDescription(uint32_t type) const
{
    return (type >= m_primitives.size()) ? nullptr : &m_primitives[type]->ConstructionDesc();
}

const ConstructionDescription* ConstructionLibrary::GetConstructionDescription(const std::string& name) const
{
    auto found = m_primitiveNameIdMap.find(name);
    return (found != m_primitiveNameIdMap.end()) ? &m_primitives[found->second]->ConstructionDesc() : nullptr;
}

// eof