#include "GameHandler.h"
#include "util/Config.h"

#include <plugin.h>

#define VERSION 1.3.3

class ChaosModComplementaries
{
public:
    ChaosModComplementaries ()
    {
        Config::Init ();

        GameHandler::Initialise ();
    }
} chaosModComplementaries;
