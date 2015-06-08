#pragma once
#include "common.h"
#include "MathBasics.h"
#include "Resources.h"
#include <memory>

using std::shared_ptr;

// Specifies elements that could be added to scene graph
// There are a few ways to attach object:
// 1. Bidirectional attachment (rigid connection) - move of one object causes move of another one
// 2. Directiona attachmant - A->B move of object A causes move of object B, move of object B doesn't cause move of object A 

struct IObject;
typedef shared_ptr<IObject> IObjectPtr;

struct IObject
    : public ISceneElement
    , public IClonable<IObjectPtr>
{
    virtual void            SetCenter(const vector3f_t& m_center)                   = 0;
    virtual void            SetCenter(float x, float y, float z)                    = 0;
    virtual void            ShiftCenter(const vector3f_t& shift)                    = 0;
    virtual void            ShiftCenter(float shiftX, float shiftY, float shiftZ)   = 0;

    virtual void            SetPosition(const vector3f_t& pos)                      = 0;
    virtual void            SetPosition(float x, float y, float z)                  = 0;
    virtual void            Shift(const vector3f_t& shift)                          = 0;
    virtual void            Shift(float shiftX, float shiftY, float shiftZ)         = 0;

    virtual void            SetAngle(const vector3f_t& angles)                      = 0;
    virtual void            SetAngle(float angleX, float angleY, float angleZ)      = 0;
    virtual void            Rotate(const vector3f_t& angles)                        = 0;
    virtual void            Rotate(float angleX, float angleY, float angleZ)        = 0;

    virtual void            SetScale(const vector3f_t& scales)                      = 0;
    virtual void            SetScale(float angleX, float angleY, float angleZ)      = 0;
    virtual void            Scale(const vector3f_t& scales)                         = 0;
    virtual void            Scale(float angleX, float angleY, float angleZ)         = 0;

    virtual vector3f_t      GetPosition() const                                     = 0;

    virtual IMeshPtr        GetMesh() const                                         = 0;
    virtual ITexturePtr     GetTexture() const                                      = 0;

    virtual void            AttachBidirectional(IObjectPtr object)                  = 0;
    virtual void            AttachDirectional(IObjectPtr object)                    = 0;
    virtual uint32_t        GetNumAttached() const                                  = 0;
    virtual IObjectPtr      GetAttached(uint32_t index) const                       = 0;
    virtual void            Detach()                                                = 0;
    virtual void            Detach(IObjectPtr object)                               = 0;

    static LIB_EXPORT IObjectPtr CALLING_CONVENTION CreateObject(IMeshPtr mesh, ITexturePtr texture);
};