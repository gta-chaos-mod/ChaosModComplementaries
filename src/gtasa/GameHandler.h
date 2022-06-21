#pragma once

#include "Config.h"

#include <CAnimManager.h>
#include <CCamera.h>
#include <CCheat.h>
#include <CPickups.h>
#include <CStats.h>
#include <CTaskSimpleRunNamedAnim.h>
#include <CTheScripts.h>

using namespace plugin;

template <typename T>
T &
GetGlobalVariable (uint32_t index)
{
    return *reinterpret_cast<T *> (CTheScripts::ScriptSpace + 4 * index);
}

class GameHandler
{
public:
    static void
    Initialise ()
    {
        Config::Init ();

        Events::gameProcessEvent.after += ProcessGame;

        if (Config::GetOrDefault ("Fixes.DisableInteriorMusic", true))
        {
            // Disable Interior Music
            patch::Nop (0x508450, 6);
            patch::Nop (0x508817, 6);
        }

        if (Config::GetOrDefault ("Fixes.RemoveFrameDelay", true))
        {
            // Fix frame delay so the game runs at proper 30 FPS and not 30 - 5
            // / "25 FPS"
            injector::WriteMemory<byte> (0x53E94C, 0, true);
        }

        if (Config::GetOrDefault ("Fixes.SkipGangTerritoriesCheck", false))
        {
            // Overwrite gang territories check for the finale of the game
            patch::RedirectCall (0x4759B0, Hooked_Finale_GetGangTerritories);
        }

        if (Config::GetOrDefault ("Fixes.DisableReplays", false))
        {
            patch::Nop (0x53C090, 5);
        }

        if (Config::GetOrDefault ("Fixes.DisableBlur", false))
        {
            patch::RedirectCall (0x704E8A, Hooked_DrawBlur);
        }

        if (Config::GetOrDefault ("Fixes.PreventLosingWeapons", true))
        {
            for (int address : {0x442E16 + 1, 0x4431CF + 1})
            {
                static bool loseWeaponsAfterDeathOrBusted = false;

                injector::WriteMemory<bool *> (address,
                                               &loseWeaponsAfterDeathOrBusted);
            }
        }

        if (Config::GetOrDefault ("Fixes.DisableMinigameZoneMusic", false))
        {
            patch::RedirectCall (0x476838, Hooked_AudioZoneEnableSound);
        }

        // TODO: Option to disable music during dance and lowrider minigame
        // We will probably have to set the SFX volume to 0.
        // Alternatively, changing the pointer to our own variable

        // Overwrite "GetStatValue" OpCode for mission checks
        // Right now it can help with Amphibious Assault, Black Project and
        // Green Goo
        patch::RedirectCall (0x49444E, HookedOpCodeGetStatValue);

        // Parachute landing fix
        // https://gtaforums.com/topic/808143-parachute-landing-fixed-scm/
        /*
        patch::RedirectCall (0x470302, Hooked_CTaskSimpleRunNamedAnim);
        patch::RedirectCall (0x4702B5, Hooked_CTaskSimpleRunNamedAnim);
        */
    }

    static void
    Hooked_AudioZoneEnableSound (char *zoneName, bool enable)
    {
        std::string zoneName_str (zoneName);
        if (zoneName_str == "BEACH" || zoneName_str == "LOWRIDE")
            enable = false;

        Call<0x508320> (zoneName, enable);
    }

    static CTaskSimpleRunNamedAnim *
    Hooked_CTaskSimpleRunNamedAnim (CTaskSimpleRunNamedAnim *thisAnim,
                                    char *animName, char *fileName, int flags,
                                    float frameDelta, int time,
                                    char nonInterruptable,
                                    char isActiveSequence, char dontLockZ,
                                    char a10)
    {
        MessageBox (NULL, animName, NULL, NULL);
        if (std::string (animName) == "FALL_FRONT")
        {
            animName   = (char *) "PARA_LAND";
            fileName   = (char *) "PARACHUTE";
            frameDelta = 10.0f;
            time       = -2;
        }
        // 0812: AS_actor -1 perform_animation "FALL_FRONT" IFP "PED"
        // framedelta 20.0 loopA 0 lockX 0 lockY 0 lockF 1 time 700 // versionB

        return new CTaskSimpleRunNamedAnim (animName, fileName, flags,
                                            frameDelta, time, nonInterruptable,
                                            isActiveSequence, dontLockZ, a10);
    }

    static void
    ProcessGame ()
    {
        HandleCheatWarning ();
        HandleNoCheatInput ();
        HandleSkipWastedBustedHelpMessages ();
    }

    static void
    HandleCheatWarning ()
    {
        if (Config::GetOrDefault ("Fixes.DisableCheatWarning", true))
        {
            // Make sure the player never cheated
            CCheat::m_bHasPlayerCheated = false;
            CStats::SetStatValue (eStats::STAT_TIMES_CHEATED, 0.0);
        }
    }

    static void
    HandleNoCheatInput ()
    {
        if (Config::GetOrDefault ("Fixes.DisableCheatInput", false)
            && !KeyPressed (VK_SHIFT))
        {
            CCheat::m_CheatString[0] = 0;
        }
    }

    static void
    HandleSkipWastedBustedHelpMessages ()
    {
        if (Config::GetOrDefault ("Fixes.SkipWastedBustedMessages", false))
        {
            CPickups::RemovePickUp (GetGlobalVariable<int> (669));
            CPickups::RemovePickUp (GetGlobalVariable<int> (670));
            CPickups::RemovePickUp (GetGlobalVariable<int> (671));
        }
    }

    static void __fastcall Hooked_Finale_GetGangTerritories (
        CRunningScript *thisScript, void *edx, __int16 count)
    {
        CTheScripts::ScriptParams[0].iParam
            = std::max (35, CTheScripts::ScriptParams[0].iParam);

        thisScript->StoreParameters (count);
    }

    static double
    HookedOpCodeGetStatValue (int statid)
    {
        double stat = CStats::GetStatValue (statid);
        if (statid == eStats::STAT_FAT
            && Config::GetOrDefault ("Fixes.SkipFatCheck", false))
        {
            stat = std::min (stat, 600.0);
        }
        else if (statid == eStats::STAT_LUNG_CAPACITY
                 && Config::GetOrDefault ("Fixes.SkipLungCapacityCheck", false))
        {
            stat = std::max (51.0, stat);
        }
        return stat;
    }

    static void
    Hooked_DrawBlur ()
    {
    }
};
