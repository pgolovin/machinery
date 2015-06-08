/////////////////////////////////////////////////////////////////////////////////
//
// class ObjectConstructor generates object from primitives and modules. 
//    the class doesn't generate mesh
//    
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#include "Library.h"
#include <map>
#include <string>

/////////////////////////////////////////////////////////////////////
///
/// object library public interface. Library is a singletone
///
/////////////////////////////////////////////////////////////////////
namespace ConstructorImpl
{

    class ObjectLibrary
    {
    public:
        void Cleanup();

        Status CheckObjectStatus(std::string name);
        const IConstructorObject* GetObject(std::string name);
        Status RegisterObject(std::string name, IConstructorObjectPtr & primitive);

        ObjectLibrary();
        virtual ~ObjectLibrary() {};

    private: 
        // loaded objects. all dependencies are loaded properly.
        // The objects are fully equiped and can be used in game.
        std::map<std::string, IConstructorObjectPtr> m_primitives;
    };
}
//end  of namespace

// eof