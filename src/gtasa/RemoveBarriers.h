#pragma once

#include "util/Config.h"

#include <CIplStore.h>
#include <CStats.h>
#include <extensions/ScriptCommands.h>

using namespace plugin;

// Thanks to zolika for the find and help on this!
class RemoveBarriers
{
    static bool
    IsIPLEnabled (char *name)
    {
        int     index = CIplStore::FindIplSlot (name);
        IplDef *def   = CIplStore::ms_pPool->GetAt (index);
        if (!def->field_2D || !def->m_bDisableDynamicStreaming) return false;

        return true;
    }

    static void
    SwitchRoad (float leftBottomX, float leftBottomY, float leftBottomZ,
                float rightTopX, float rightTopY, float rightTopZ,
                bool on = true)
    {
        if (on)
            Command<eScriptCommands::COMMAND_SWITCH_ROADS_ON> (
                leftBottomX, leftBottomY, leftBottomZ, rightTopX, rightTopY,
                rightTopZ);
        else
            Command<eScriptCommands::COMMAND_SWITCH_ROADS_OFF> (
                leftBottomX, leftBottomY, leftBottomZ, rightTopX, rightTopY,
                rightTopZ);
    }

    static void
    EnableRoads ()
    {
        SwitchRoad (-2696.464, 1239.866, 40.7599, -2665.359, 2190.96, 70.8125);
        SwitchRoad (-2740.694, 2233.618, 40.8431, -2720.91, 2338.224, 80.4822);
        SwitchRoad (-2695.584, 1237.981, 40.7328, -2664.417, 1454.767, 60.8126);
        SwitchRoad (-995.0013, -416.2032, 30.4207, -940.5399, -251.8564,
                    40.6762);
        SwitchRoad (-205.8387, 250.7443, 7.2472, -131.0039, 481.8496, 15.9152);
        SwitchRoad (-100.7515, -927.8298, 18, -68.3752, -891.9871, 14);
        SwitchRoad (609.7595, 327.3437, 15.8783, 429.8884, 616.0168, 20.289);
        SwitchRoad (317.1688, 707.7672, 7, 437.5726, 709.0657, 20.5578);
        SwitchRoad (391.1194, 640.015, 7, 402.2627, 664.798, 18.5098);
        SwitchRoad (289.2904, 636.3991, 7.8675, 409.4943, 702.3849, 20.0345);
        SwitchRoad (300.3153, 718.7909, 7.7846, 316.7906, 781.0926, 14.0795);
        SwitchRoad (254.9982, 837.029, 10.1731, 270.7453, 929.2404, 30.2553);
        SwitchRoad (210.7811, 939.2068, 10.9062, 249.4799, 959.1111, 30.2141);
        SwitchRoad (230.4545, 946.0961, 20.6674, 255.9772, 969.2755, 30.4776);
        SwitchRoad (249.4279, 899.7975, 10.5871, 268.6826, 933.5995, 30.3975);
        SwitchRoad (312.1081, 694.1089, 6, 324.0811, 733.0005, 10);
        SwitchRoad (324.4526, 804.9198, 9.6186, 332.8747, 814.356, 14.3925);
        SwitchRoad (1690.819, 376.5103, 28.1103, 1730.223, 445.2955, 30.8414);
        SwitchRoad (1643.536, 227.3723, 27.4457, 1673.062, 295.5788, 30.0815);
        SwitchRoad (1673.765, 388.1013, 40.2331, 1815.862, 804.9291, 10);
        SwitchRoad (1705.156, 308.3448, 20, 1710.948, 316.4094, 23.5612);
        SwitchRoad (-12.7067, -1522.455, 1, 80.8463, -1517.111, 5);
        SwitchRoad (-16.3392, -1532.882, 0.0394, 69.3401, -1523.771, 5.922);
        SwitchRoad (618.7253, -1189.606, 18, 623.5441, -1161.981, 22);
        SwitchRoad (-33.4208, -1341.84, 9, 35.3764, -1303.948, 13);
        SwitchRoad (-41.2393, -1385.87, 8, -3.5883, -1368.856, 10.5);
        SwitchRoad (-1690.705, 539.6102, 30.3278, -1100.567, 1140.569, 50.735);
        SwitchRoad (-1799.541, 379.7155, 16, -1780.199, 392.2779, 18);
        SwitchRoad (-1092.429, 1286.505, 30, -1077.038, 1319.495, 35);
        SwitchRoad (-1860.133, 314.7891, 38, -1638.563, 557.4354, 40);
        SwitchRoad (-1737.333, 455.9431, 30.3573, -1710.363, 500.6261, 40.4891);
        SwitchRoad (-1689.229, 513.0995, 30.2597, -1679.124, 524.8383, 40.25);
        SwitchRoad (-1742.906, 500.7302, 30.4679, -1650.312, 551.8201, 40.7455);
        SwitchRoad (-1761.95, 507.8931, 35.0533, -1751.361, 531.5917, 41.3335,
                    false);
    }

public:
    static void
    Process ()
    {
        if (!Config::GetOrDefault ("Fixes.RemoveBarriers", false)) return;

        int citiesUnlocked = CStats::GetStatValue (181);
        if (citiesUnlocked != 3)
        {
            CStats::SetStatValue (181, 3);

            if (IsIPLEnabled ((char *) "BARRIERS1"))
                Command<eScriptCommands::COMMAND_REMOVE_IPL> ("BARRIERS1");

            if (IsIPLEnabled ((char *) "BARRIERS2"))
                Command<eScriptCommands::COMMAND_REMOVE_IPL> ("BARRIERS2");

            EnableRoads ();
        }
    }
};