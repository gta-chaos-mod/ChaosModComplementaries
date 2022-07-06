#pragma once

#include "util/CleanupHandler.h"
#include "util/hooks/HookMacros.h"

class GlobalHooksInstance
{
    CleanupHandler cleanupHandler;

public:
    auto
    Get ()
    {
        return this;
    }

    auto &
    GetCleanupHandler ()
    {
        return cleanupHandler;
    }
} globalHooksInstance;