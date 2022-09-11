#pragma once

#include "util/Config.h"
#include "util/GlobalHooksInstance.h"
#include "util/hooks/HookMacros.h"

#include <CTheScripts.h>

class M_SaintMarcsBistro
{
public:
    void
    Initialise ()
    {
        CloserLibertyCityFlight ();
    }

private:
    static inline CVector2D newLCLocation = {4500.0f, 2000.0f};

    void
    CloserLibertyCityFlight ()
    {
        if (!Config::GetOrDefault ("Fixes.CloserLibertyCityFlight", false))
            return;

        HOOK_METHOD_ARGS (globalHooksInstance.Get (),
                          Hooked_CloserLibertyCityFlight_Checkpoint,
                          void (CRunningScript *, __int16), 0x47CF52);

        HOOK_METHOD_ARGS (globalHooksInstance.Get (),
                          Hooked_CloserLibertyCityFlight_CreateShamal_1,
                          void (CRunningScript *, __int16), 0x467AB7);

        HOOK_METHOD_ARGS (globalHooksInstance.Get (),
                          Hooked_CloserLibertyCityFlight_CreateShamal_2,
                          void (CRunningScript *, __int16), 0x467DB9);

        HOOK_METHOD_ARGS (globalHooksInstance.Get (),
                          Hooked_CloserLibertyCityFlight_CheckpointCheck,
                          void (CRunningScript *, __int16), 0x486DB1);
    }

    static void
    Hooked_CloserLibertyCityFlight_Checkpoint (auto          &&cb,
                                               CRunningScript *thisScript,
                                               __int16         count)
    {
        cb ();

        if (std::string (thisScript->m_szName) == "casin10")
        {
            if (CTheScripts::ScriptParams[0].fParam == 7500.0f)
            {
                CTheScripts::ScriptParams[0].fParam = newLCLocation.x;
                CTheScripts::ScriptParams[1].fParam = newLCLocation.y;
            }
        }
    }

    static void
    Hooked_CloserLibertyCityFlight_CreateShamal_1 (auto          &&cb,
                                                   CRunningScript *thisScript,
                                                   __int16         count)
    {
        cb ();

        if (std::string (thisScript->m_szName) == "casin10"
            && CTheScripts::ScriptParams[0].iParam == 519)
        {
            if (CTheScripts::ScriptParams[1].fParam == 7500.0f)
            {
                CTheScripts::ScriptParams[1].fParam = newLCLocation.x;
                CTheScripts::ScriptParams[2].fParam = newLCLocation.y;
            }
        }
    }

    static void
    Hooked_CloserLibertyCityFlight_CreateShamal_2 (auto          &&cb,
                                                   CRunningScript *thisScript,
                                                   __int16         count)
    {
        cb ();

        if (std::string (thisScript->m_szName) == "casin10")
        {
            if (CTheScripts::ScriptParams[1].fParam == 7500.0f)
            {
                CTheScripts::ScriptParams[1].fParam = newLCLocation.x;
                CTheScripts::ScriptParams[2].fParam = newLCLocation.y;
            }
        }
    }

    static void
    Hooked_CloserLibertyCityFlight_CheckpointCheck (auto          &&cb,
                                                    CRunningScript *thisScript,
                                                    __int16         count)
    {
        cb ();

        if (std::string (thisScript->m_szName) == "casin10")
        {
            if (CTheScripts::ScriptParams[1].fParam == 7500.0f)
            {
                CTheScripts::ScriptParams[1].fParam = newLCLocation.x;
                CTheScripts::ScriptParams[2].fParam = newLCLocation.y;
            }
        }
    }
} m_SaintMarcsBistro;