#pragma once

#include "util/Config.h"
#include "util/GlobalHooksInstance.h"
#include "util/hooks/HookMacros.h"

#include <CTheScripts.h>

class M_BurningDesire
{
public:
    void
    Initialise ()
    {
        SetupHooks ();
    }

    void
    ProcessScripts ()
    {
        UpdateDeniseMissionPassFlag ();
    }

private:
    static inline bool shouldPassDeniseMission = false;

    void
    SetupHooks ()
    {
        HOOK_METHOD_ARGS (GlobalHooksInstance::Get (),
                          Hooked_CollectParameters_EarlyFinish,
                          __int16 (CRunningScript *, unsigned __int16),
                          0x47C664);
    }

    void
    UpdateDeniseMissionPassFlag ()
    {
        for (auto i = CTheScripts::pActiveScripts; i; i = i->m_pNext)
        {
            if (!i->m_bIsMission || !i->m_bIsActive || i->m_bIsExternal)
                continue;

            if (std::string (i->m_szName) != "crash1") continue;

            if (i->m_pCurrentIP - i->m_pBaseIP > 100)
            {
                if (shouldPassDeniseMission)
                {
                    i->m_pCurrentIP = i->m_pBaseIP + 41291;
                    ClearPlayerStatus ();

                    shouldPassDeniseMission = false;
                }

                continue;
            }

            shouldPassDeniseMission = false;
        }
    }

    static __int16
    Hooked_CollectParameters_EarlyFinish (auto &&cb, CRunningScript *script,
                                          unsigned __int16 count)
    {
        if (!CONFIG ("Fixes.BurningDesireHomeSoftlock", true)) return cb ();

        int offset = script->m_pCurrentIP - script->m_pBaseIP - 2;

        __int16 result = cb ();

        if (std::string (script->m_szName) == "crash1")
        {
            // Jump to mission finish
            if (offset == 13603)
            {
                shouldPassDeniseMission = true;
            }
        }

        return result;
    }

    static void
    ClearPlayerStatus ()
    {
        CPlayerPed *player = FindPlayerPed ();
        if (!player) return;

        Command<eScriptCommands::COMMAND_SKIP_CUTSCENE_END> ();
        Command<eScriptCommands::COMMAND_CLEAR_CHAR_TASKS> (player);
        Command<eScriptCommands::COMMAND_SET_CAMERA_BEHIND_PLAYER> ();
        Command<eScriptCommands::COMMAND_SWITCH_WIDESCREEN> (0);
        Command<eScriptCommands::COMMAND_SET_PLAYER_CONTROL> (0, 1);
        Command<eScriptCommands::COMMAND_RESTORE_CAMERA_JUMPCUT> ();
        Command<eScriptCommands::COMMAND_DETACH_CHAR_FROM_CAR> (player);
    }
} m_BurningDesire;