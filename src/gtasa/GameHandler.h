#pragma once

#include "CityUnlockHandler.h"
#include "missions/Missions.h"
#include "util/Config.h"
#include "util/GlobalHooksInstance.h"
#include "util/hooks/HookMacros.h"

#include <thread>

#include <CAEAudioHardware.h>
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

template <uintptr_t address, uint32_t size> class AddressNopper
{
    static inline bool                       nopped = false;
    static inline injector::scoped_nop<size> scoped_nop_obj;

public:
    static void
    apply ()
    {
        if (nopped) return;

        scoped_nop_obj.make_nop (address, size);

        nopped = true;
    }

    static void
    restore ()
    {
        if (!nopped) return;

        scoped_nop_obj.restore ();

        nopped = false;
    }
};

template <uintptr_t address, typename T> class AddressWriter
{
    static inline bool                        written = false;
    static inline injector::scoped_write<10U> scoped_write_obj;

public:
    static void
    apply (const T &value)
    {
        if (written) return;

        scoped_write_obj.write<T> (address, value, true);

        written = true;
    }

    static void
    restore ()
    {
        if (!written) return;

        scoped_write_obj.restore ();

        written = false;
    }
};

class GameHandler
{
    static inline bool initialised                   = false;
    static inline bool loseWeaponsAfterDeathOrBusted = false;

    static inline bool removeFrameDelayEnabled     = true;
    static inline bool disableReplaysEnabled       = false;
    static inline bool disableInteriorMusicEnabled = false;

public:
    static void
    Initialise ()
    {
        if (initialised) return;

        Events::gameProcessEvent.after += ProcessGame;

        // Overwrite gang territories check for the finale of the game
        HOOK_METHOD (GlobalHooksInstance::Get (),
                     Hooked_Finale_GetGangTerritories,
                     void (CRunningScript *, __int16), 0x4759B0);

        // Disable Blur
        HOOK (GlobalHooksInstance::Get (), Hooked_DrawBlur, void (float),
              0x704E8A);

        // Lose weapons after busted or wasted
        for (int address : {0x442E16 + 1, 0x4431CF + 1})
        {
            injector::WriteMemory<bool *> (address,
                                           &loseWeaponsAfterDeathOrBusted,
                                           true);
        }

        // TODO: Option to disable music during dance and lowrider minigame
        // We will probably have to set the SFX volume to 0.
        // Alternatively, changing the pointer to our own variable

        // Mute lowrider and dancing minigame music as well as some cutscene
        // music
        HOOK_METHOD_ARGS (GlobalHooksInstance::Get (),
                          Hooked_CutsceneTrackManager_SetFrequencyScalingFactor,
                          int (CAEAudioHardware *, int, int, float), 0x4DBF9B);

        // Overwrite "GetStatValue" OpCode for mission checks
        HOOK_ARGS (GlobalHooksInstance::Get (), Hooked_OpCodeGetStatValue,
                   double (int), 0x49444E);

        // Override Lowrider and Street Racer checks
        HOOK_METHOD_ARGS (GlobalHooksInstance::Get (),
                          Hooked_IsLowRiderOrStreetRacer,
                          unsigned __int16 (CRunningScript *, char), 0x478528,
                          0x478575);

        // Override Cesar Race Badlands
        HOOK_METHOD_ARGS (GlobalHooksInstance::Get (), Hooked_CesarRaceBadlands,
                          unsigned __int16 (CRunningScript *, char), 0x48ABC6);

        // Disable mission time checks
        HOOK_METHOD_ARGS (GlobalHooksInstance::Get (),
                          Hooked_DisableMissionTimeChecks,
                          void (CRunningScript *, __int16), 0x46821E);

        Missions::Initialise ();

        initialised = true;
    }

    static void
    ProcessGame ()
    {
        UpdateConfigValues ();

        HandleCheatWarning ();
        HandleNoCheatInput ();
        HandleSkipWastedBustedHelpMessages ();
        HandleCheapAirport ();

        CityUnlockHandler::Process ();
    }

private:
    static void
    UpdateConfigValues ()
    {
        loseWeaponsAfterDeathOrBusted
            = !CONFIG ("Fixes.PreventLosingWeapons", false);

        UpdateFrameDelay ();
        UpdateDisableReplays ();
        UpdateDisableInteriorMusic ();
    }

    static void
    UpdateFrameDelay ()
    {
        if (CONFIG ("Fixes.RemoveFrameDelay", true))
            AddressWriter<0x53E94C, byte>::apply (0);
        else
            AddressWriter<0x53E94C, byte>::restore ();
    }

    static void
    UpdateDisableReplays ()
    {
        if (CONFIG ("Fixes.DisableReplays", false))
            AddressNopper<0x53C090, 5>::apply ();
        else
            AddressNopper<0x53C090, 5>::restore ();
    }

    static void
    UpdateDisableInteriorMusic ()
    {
        if (CONFIG ("Fixes.DisableInteriorMusic", false))
        {
            AddressNopper<0x50844A, 6>::apply ();
            AddressNopper<0x5084B0, 6>::apply ();
            AddressNopper<0x508450, 6>::apply ();
            AddressNopper<0x508817, 6>::apply ();
        }
        else
        {
            AddressNopper<0x50844A, 6>::restore ();
            AddressNopper<0x5084B0, 6>::restore ();
            AddressNopper<0x508450, 6>::restore ();
            AddressNopper<0x508817, 6>::restore ();
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
        CStats::SetStatValue (eStats::STAT_TIMES_CHEATED, 0.0f);
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
        if (CONFIG ("Fixes.SkipGangTerritoriesCheck", false))
        {
            CTheScripts::ScriptParams[0].iParam
                = std::max (35, CTheScripts::ScriptParams[0].iParam);
        }

        cb ();
    }

    static void
    Hooked_DrawBlur (auto &&cb)
    {
        if (CONFIG ("Fixes.DisableBlur", false)) return;

        cb ();
    }

    static int
    Hooked_CutsceneTrackManager_SetFrequencyScalingFactor (
        auto &&cb, CAEAudioHardware *thisAudioHardware, int slot, int offset,
        float &factor)
    {
        int result = cb ();

        if (!CONFIG ("Fixes.DisableMinigameAndCutsceneMusic", false))
            return result;

        if (factor > 0.0f)
            thisAudioHardware->m_afChannelVolumes[slot + offset] = -100.0f;

        return result;
    }

    static double
    Hooked_OpCodeGetStatValue (auto &&cb, int statID)
    {
        double stat = cb ();

        if (!CONFIG ("Fixes.SkipStatChecks", false)) return stat;

        if (statID == STAT_FAT)
        {
            stat = std::min (stat, 600.0);
        }
        else if (statID == STAT_LUNG_CAPACITY)
        {
            stat = std::max (51.0, stat);
        }
        else if (statID == STAT_DRIVING_SKILL)
        {
            stat = std::max (200.0, stat);
        }
        else if (statID == STAT_BIKE_SKILL)
        {
            stat = std::max (200.0, stat);
        }
        else if (statID == STAT_CYCLING_SKILL)
        {
            stat = std::max (500.0, stat);
        }
        else if (statID == STAT_STAMINA)
        {
            stat = std::max (500.0, stat);
        }

        return stat;
    }

    static unsigned __int16
    Hooked_IsLowRiderOrStreetRacer (auto &&cb, CRunningScript *thisScript,
                                    char &flag)
    {
        if (!CONFIG ("Fixes.AllowRacesWithAllVehicles", false)) return cb ();

        if (thisScript && std::string (thisScript->m_szName) == "cesar")
            flag = 1;

        return cb ();
    }

    static unsigned __int16
    Hooked_CesarRaceBadlands (auto &&cb, CRunningScript *thisScript, char &flag)
    {
        if (!CONFIG ("Fixes.AllowRacesWithAllVehicles", false)) return cb ();

        if (thisScript && std::string (thisScript->m_szName) == "bcesar")
            flag = 1;

        return cb ();
    }

    static void
    Hooked_DisableMissionTimeChecks (auto &&cb, CRunningScript *thisScript,
                                     __int16 count)
    {
        if (CONFIG ("Fixes.DisableMissionTimeChecks", true))
        {
            std::string missionName (thisScript->m_szName);

            std::map<std::string, int> hourMaps
                = {{"sweet", 10}, {"ryder", 13},  {"strap", 23}, {"wuzi", 21},
                   {"synd", 21},  {"desert", 21}, {"heist", 21}, {"oddveh", 8}};

            int hour = hourMaps.contains (missionName)
                           ? hourMaps[missionName]
                           : CTheScripts::ScriptParams[0].iParam;

            CTheScripts::ScriptParams[0].iParam = hour;
        }

        cb ();
    }
};
