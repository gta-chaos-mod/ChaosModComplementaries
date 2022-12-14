#pragma once

#include "CityUnlockHandler.h"
#include "missions/Missions.h"
#include "util/Config.h"
#include "util/GlobalHooksInstance.h"
#include "util/hooks/HookMacros.h"

#include <thread>

#include <CAnimManager.h>
#include <CCamera.h>
#include <CCheat.h>
#include <CMenuManager.h>
#include <CPickups.h>
#include <CReferences.h>
#include <CStats.h>
#include <CTaskSimpleRunNamedAnim.h>
#include <CTheScripts.h>
#include <CTimer.h>

using namespace plugin;

template <typename T>
T &
GetGlobalVariable (uint32_t index)
{
    return *reinterpret_cast<T *> (CTheScripts::ScriptSpace + 4 * index);
}

class GameHandler
{
    static inline bool initialised      = false;
    static inline int  lastConfigReload = 0;

public:
    static void
    Initialise ()
    {
        if (initialised) return;

        Config::Init ();

        Events::gameProcessEvent.after += ProcessGame;

        // Disable Interior Music
        if (CONFIG ("Fixes.DisableInteriorMusic", true))
        {
            // Spheres
            patch::Nop (0x50844A, 6);
            patch::Nop (0x5084B0, 6);

            // Boxes
            patch::Nop (0x508450, 6);
            patch::Nop (0x508817, 6);
        }

        if (CONFIG ("Fixes.RemoveFrameDelay", true))
        {
            // Fix frame delay so the game runs at proper 30 FPS and not 30 - 5
            // / "25 FPS"
            injector::WriteMemory<byte> (0x53E94C, 0, true);
        }

        if (CONFIG ("Fixes.SkipGangTerritoriesCheck", false))
        {
            // Overwrite gang territories check for the finale of the game
            HOOK_METHOD (globalHooksInstance.Get (),
                         Hooked_Finale_GetGangTerritories,
                         void (CRunningScript *, __int16), 0x4759B0);
        }

        if (CONFIG ("Fixes.DisableReplays", false))
        {
            patch::Nop (0x53C090, 5);
        }

        if (CONFIG ("Fixes.DisableBlur", false))
        {
            HOOK (globalHooksInstance.Get (), Hooked_DrawBlur, void (float),
                  0x704E8A);
        }

        if (CONFIG ("Fixes.PreventLosingWeapons", true))
        {
            for (int address : {0x442E16 + 1, 0x4431CF + 1})
            {
                static bool loseWeaponsAfterDeathOrBusted = false;

                injector::WriteMemory<bool *> (address,
                                               &loseWeaponsAfterDeathOrBusted);
            }
        }

        // TODO: Option to disable music during dance and lowrider minigame
        // We will probably have to set the SFX volume to 0.
        // Alternatively, changing the pointer to our own variable

        // Overwrite "GetStatValue" OpCode for mission checks
        // Right now it can help with Amphibious Assault, Black Project and
        // Green Goo
        HOOK_ARGS (globalHooksInstance.Get (), Hooked_OpCodeGetStatValue,
                   double (int), 0x49444E);

        if (CONFIG ("Fixes.AllowRacesWithAllVehicles", false))
        {
            // Override Lowrider and Street Racer checks
            HOOK_METHOD_ARGS (globalHooksInstance.Get (),
                              Hooked_IsLowRiderOrStreetRacer,
                              unsigned __int16 (CRunningScript *, char),
                              0x478528, 0x478575);

            // Override Cesar Race Badlands
            HOOK_METHOD_ARGS (globalHooksInstance.Get (),
                              Hooked_CesarRaceBadlands,
                              unsigned __int16 (CRunningScript *, char),
                              0x48ABC6);
        }

        if (CONFIG ("Fixes.DisableMissionTimeChecks", true))
        {
            HOOK_METHOD_ARGS (globalHooksInstance.Get (),
                              Hooked_DisableMissionTimeChecks,
                              void (CRunningScript *, __int16), 0x46821E);
        }

        Missions::Initialise ();

        initialised = true;
    }

    static void
    ProcessGame ()
    {
        HandleConfigReload ();

        HandleCheatWarning ();
        HandleNoCheatInput ();
        HandleSkipWastedBustedHelpMessages ();
        HandleCheapAirport ();

        CityUnlockHandler::Process ();
    }

private:
    static void
    HandleConfigReload ()
    {
        // F7 + C
        if (KeyPressed (VK_F7) && KeyPressed (67))
        {
            int currentTime
                = std::max (CTimer::m_snTimeInMillisecondsNonClipped,
                            (unsigned int) lastConfigReload);

            if (FrontEndMenuManager.m_bMenuActive
                && lastConfigReload <= currentTime)
            {
                lastConfigReload = currentTime + 3000;

                Config::Init ();
            }
        }
    }

    static void
    HandleCheapAirport ()
    {
        if (!CONFIG ("Fixes.CheapAirport", true)) return;

        for (int i = 0; i < MAX_NUM_PICKUPS; i++)
        {
            CPickup &pickup = CPickups::aPickUps[i];
            if (pickup.m_nPickupType == PICKUP_NONE) continue;

            if (pickup.m_nAmmo == 80000)
            {
                pickup.m_nAmmo = 5;
            }
        }
    }

    static void
    HandleCheatWarning ()
    {
        if (!CONFIG ("Fixes.DisableCheatWarning", true)) return;

        // Make sure the player never cheated
        CCheat::m_bHasPlayerCheated = false;
        CStats::SetStatValue (eStats::STAT_TIMES_CHEATED, 0.0);
    }

    static void
    HandleNoCheatInput ()
    {
        if (!CONFIG ("Fixes.DisableCheatInput", false) || KeyPressed (VK_SHIFT))
            return;

        CCheat::m_CheatString[0] = 0;
    }

    static void
    HandleSkipWastedBustedHelpMessages ()
    {
        if (!CONFIG ("Fixes.SkipWastedBustedMessages", false)) return;

        CPickups::RemovePickUp (GetGlobalVariable<int> (669));
        CPickups::RemovePickUp (GetGlobalVariable<int> (670));
        CPickups::RemovePickUp (GetGlobalVariable<int> (671));
    }

    static void
    Hooked_Finale_GetGangTerritories (auto &&cb)
    {
        CTheScripts::ScriptParams[0].iParam
            = std::max (35, CTheScripts::ScriptParams[0].iParam);

        cb ();
    }

    static void
    Hooked_DrawBlur (auto &&cb)
    {
    }

    static double
    Hooked_OpCodeGetStatValue (auto &&cb, int statID)
    {
        double stat = cb ();

        if (statID == STAT_FAT && CONFIG ("Fixes.SkipFatCheck", false))
        {
            stat = std::min (stat, 600.0);
        }
        else if (statID == STAT_LUNG_CAPACITY
                 && CONFIG ("Fixes.SkipLungCapacityCheck", false))
        {
            stat = std::max (51.0, stat);
        }

        return stat;
    }

    static unsigned __int16
    Hooked_IsLowRiderOrStreetRacer (auto &&cb, CRunningScript *thisScript,
                                    char &flag)
    {
        if (thisScript && std::string (thisScript->m_szName) == "cesar")
            flag = 1;

        return cb ();
    }

    static unsigned __int16
    Hooked_CesarRaceBadlands (auto &&cb, CRunningScript *thisScript, char &flag)
    {
        if (thisScript && std::string (thisScript->m_szName) == "bcesar")
            flag = 1;

        return cb ();
    }

    static void
    Hooked_DisableMissionTimeChecks (auto &&cb, CRunningScript *thisScript,
                                     __int16 count)
    {
        std::string missionName (thisScript->m_szName);

        std::map<std::string, int> hourMaps
            = {{"sweet", 9}, {"ryder", 12},  {"strap", 22}, {"wuzi", 20},
               {"synd", 20}, {"desert", 20}, {"heist", 20}, {"oddveh", 7}};

        int hour = hourMaps.contains (missionName)
                       ? hourMaps[missionName]
                       : CTheScripts::ScriptParams[0].iParam;

        CTheScripts::ScriptParams[0].iParam = hour;

        cb ();
    }
};
