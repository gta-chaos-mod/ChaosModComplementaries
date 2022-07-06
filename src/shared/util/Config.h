#pragma once

#include "cpptoml.h"

#include <filesystem>

#include <plugin.h>

class Config
{
private:
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

# Skips the help messages that appear the first time you get wasted or busted.
# It removes the pickups so the messages won't appear in the first place
# Default: false
SkipWastedBustedMessages = false

# Whether or not to skip the final gang territories check (34% needed)
# Default: false
SkipGangTerritoriesCheck = false

# Whether to skip the fat stat check for missions like Black Project and Green Goo
# Default: false
SkipFatCheck = false

# Whether to skip the lung capacity stat check for Amphibious Assault
# Default: false
SkipLungCapacityCheck = false

# Will set the price of the airport to $5 so it can still be bought when very low on money
# Default: true
CheapAirport = true

# Allows taking part in the races ("High-Stakes, Low-Rider", "Wu Zi Mu" and "Farewell, My Love")
# with all vehicle classes
# Default: false
AllowRacesWithAllVehicles = false

# Disables the ability to play replays
# (Replays can cause crashes in some scenarios with certain effects)
# Default: false
DisableReplays = false

# Disables interior music in places such as Caligula's Casino (Helps against copyright)
# Also disables music playing at the van in Life's A Beach and the lowrider meetup in Cesar Vialpando
# Default: false
DisableInteriorMusic = false

# Remove the 14ms frame delay to make the game run in proper 30 FPS instead of 30 FPS - 5 / "25 FPS"
# Default: true
RemoveFrameDelay = true

# Disables the annoying blur that happens when you drive too fast to give you a sense of "speed"
# Default: false
DisableBlur = false

# Disables warning that the player has cheated when trying to save the game
# Default: true
DisableCheatWarning = true

# Disables input of cheats so no accidental cheats can be input.
# This will COMPLETELY disable cheat input - not just accidental hash collisions.
# Cheats can still be input when holding SHIFT.
# Default: false
DisableCheatInput = false

#######################################################)";
#else
    inline static std::string configContent = "";
#endif

public:
    static void
    Init ()
    {
        const std::string pluginFilename = std::string (PLUGIN_FILENAME);
        const std::string configFilename
            = "/" + pluginFilename.substr (0, pluginFilename.size () - 4)
              + ".toml";
        const std::filesystem::path config_path
            = PLUGIN_PATH ((char *) configFilename.c_str ());

        std::filesystem::create_directories (config_path.parent_path ());

        if (!std::filesystem::exists (config_path))
        {
            // Write default configuration file
            std::ofstream ConfigFile (config_path);

            ConfigFile << configContent;

            ConfigFile.close ();
        }

        if (std::filesystem::exists (config_path))
        {
            config = cpptoml::parse_file (config_path.string ());
        }
    }

    template <class T>
    static T
    GetOrDefault (std::string key, T default_val)
    {
        if (!config) return default_val;

        return config->get_qualified_as<T> (key).value_or (default_val);
    }
};