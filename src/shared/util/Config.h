#pragma once

#include "cpptoml.h"

#include <filesystem>

#include <plugin.h>

#define CONFIG(_setting, _default) Config::GetOrDefault (_setting, _default)

class Config
{
    static inline bool fileWatcherInitialized = false;

    inline static std::shared_ptr<cpptoml::table> config;
#ifdef GTASA
    inline static std::string configContent
        = R"(########################################################
# Chaos Mod Complementaries
#######################################################

#######################################################
# Various fixes
[Fixes]

# Prevent losing weapons when dying or getting busted without the need of Katie or Barbara as girlfriends
# Default: false
PreventLosingWeapons = false

# Skips the help messages that appear the first time you get wasted or busted
# It removes the pickups so the messages won't appear in the first place
# Default: false
SkipWastedBustedMessages = false

# Whether or not to skip the final gang territories check (34% needed)
# Default: false
SkipGangTerritoriesCheck = false

# Whether to skip various stat checks found in the game, such as:
# - The fat check for Black Project and Green Goo
# - The lung capacity check for Amphibious Assault
# - The driving skill checks for 8-Track and Blood Bowl
# - The cycling and stamina skill checks for the Mount Chiliad challenge and Beat The Cock
# Default: false
SkipStatChecks = false

# Will set the price of the airport to $5 so it can still be bought when very low on money
# Default: true
CheapAirport = true

# Allows taking part in the races ("High-Stakes, Low-Rider", "Wu Zi Mu" and "Farewell, My Love")
# with all vehicle classes
# Default: false
AllowRacesWithAllVehicles = false

# Will move the location of the Liberty City marker closer to the map so the flight is not as long and boring
# (From ~1min down to 20s flight time)
# Default: true
CloserLibertyCityFlight = true

# Will allow you to start time-based missions at any time
# Default: true
DisableMissionTimeChecks = true

# Unlocks other cities so you can won't get wanted levels when you enter them
# Default: false
UnlockCities = false

# Removes barriers to other islands
# Default: true
RemoveBarriers = true

# Disables the ability to play replays
# (Replays can cause crashes in some scenarios with certain effects)
# Default: false
DisableReplays = false

# Disables interior music in places such as Caligula's Casino (Helps against copyright)
# Also disables music playing at the van in Life's A Beach and the lowrider meetup in Cesar Vialpando
# Default: false
DisableInteriorMusic = false

# Disables the minigame music during the lowrider and dancing minigames (Maybe also others)
# Also disables certain cutscene music as they apparently share the same method
# Default: false
DisableMinigameAndCutsceneMusic = false

# Remove the 14ms frame delay to make the game run in proper 30 FPS instead of 30 FPS - 5 / "25 FPS"
# Default: true
RemoveFrameDelay = true

# Disables the annoying blur that happens when you drive too fast to give you a sense of "speed"
# Default: false
DisableBlur = false

# Disables warning that the player has cheated when trying to save the game
# Default: true
DisableCheatWarning = true

# Disables input of cheats so no accidental cheats can be input
# This will COMPLETELY disable cheat input - not just accidental hash collisions
# Cheats can still be input when holding SHIFT
# Default: false
DisableCheatInput = false

# Disables the player vehicle getting damaged when upside down
# This makes it function like in GTA V for the player vehicle
# Empty or NPC-driven cars will still get damaged and eventually catch fire
# Default: false
DisablePlayerVehicleDamagesUpsideDown = false

# Ends the mission "Burning Desire" as soon as the marker infront of Denise's house is entered
# This is to prevent a potential softlock when she dies or can't pathfind to the house
# Default: true
BurningDesireHomeSoftlock = true

# Disables "has player weapon" checks in specific missions so they don't immediately fail
# For now this only overrides the check in Tagging Up Turf, as no other mission seems to fail
# Default: true
DisableMissionWeaponChecks = true

#######################################################)";
#else
    inline static std::string configContent = "";
#endif

public:
    static std::string
    GetConfigFilename ()
    {
        const std::string pluginFilename = std::string (PLUGIN_FILENAME);
        const std::string configFilename
            = pluginFilename.substr (0, pluginFilename.size () - 4) + ".toml";

        return configFilename;
    }

    static std::filesystem::path
    GetConfigPath ()
    {
        const std::string configFilename = GetConfigFilename ();

        const std::filesystem::path configPath
            = PLUGIN_PATH ((char *) configFilename.c_str ());

        std::filesystem::create_directories (configPath.parent_path ());

        return configPath;
    }

    static void Init ();

    static void ReloadConfig ();

    template <class T>
    static T
    GetOrDefault (std::string key, T defaultValue)
    {
        if (!config) return defaultValue;

        return config->get_qualified_as<T> (key).value_or (defaultValue);
    }
};