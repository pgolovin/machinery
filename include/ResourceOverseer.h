#pragma once
#include "Resources.h"
#include "common.h"
#include <string>

struct IResourceOverseer 
{
    virtual IMeshPtr            LoadMesh(const std::wstring& path)                 = 0;
    virtual ITexturePtr         LoadTexture(const std::wstring& path)              = 0;
    virtual IScriptPtr          LoadScript(const std::wstring& path)               = 0;

    static LIB_EXPORT IResourceOverseer& CALLING_CONVENTION Instance();
};

