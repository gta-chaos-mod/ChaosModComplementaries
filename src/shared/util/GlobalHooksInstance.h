#pragma once

#include "util/hooks/HookMacros.h"

class GlobalHooksInstance
{
public:
    static auto
    Get ()
    {
        static GlobalHooksInstance instance;
        return &instance;
    }
};