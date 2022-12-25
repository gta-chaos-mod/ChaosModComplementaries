#include "GameHandler.h"
#include "util/Config.h"

#include <plugin.h>

#define VERSION 1.2

using namespace plugin;

class ChaosModComplementaries
{
public:
    ChaosModComplementaries ()
    {
        Config::Init ();

        Events::initRwEvent += GameHandler::Initialise;
    }
} chaosModComplementaries;
