/////////////////////////////////////////////////////////////////////////////////
//
// class ObjectConstructor generates object from primitives and modules. 
//    the class doesn't generate mesh
//
// the class is responcible for:
//    - create element based structure
//    - split structure on separate solid modules
//    - create skeleton for object
//
/////////////////////////////////////////////////////////////////////////////////
#pragma once
#include <Overmind.h>
#include <Constructor.h>
#include <vector>
#include <string>

struct lua_State;
struct Constructor;

namespace OvermindImpl
{

    class OvermindCerebro : public Overmind
    {
    public:
        OvermindCerebro();
        ~OvermindCerebro();

        ///////////////////////////////////////////////////////////////////////////////////
        // executes given script
        virtual Status ExecuteScript(std::string script);

        ///////////////////////////////////////////////////////////////////////////////////
        // return last error message
        // if string is empty, no errors happened
        virtual std::string GetLastError();

        ///////////////////////////////////////////////////////////////////////////////////
        // Access to modules
        virtual Constructor& GetConstructor() {return m_constructor;}

        ///////////////////////////////////////////////////////////////////////////////////
        // cleanup everything in overmind
        virtual void Amnesia() {m_constructor.Reset();};

    private:
        void registerGlobals();

        lua_State *m_lua;
        Constructor& m_constructor;

        std::vector<std::string> m_errorMessages;

        PREVENT_COPY(OvermindCerebro);
    };

}//end  of namespace constructor

// eof