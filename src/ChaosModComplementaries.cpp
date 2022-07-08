#include "GameHandler.h"

#include <plugin.h>

using namespace plugin;

class ChaosModComplementaries
{
public:
    ChaosModComplementaries ()
    {
        Events::initRwEvent += GameHandler::Initialise;
    }
} chaosModComplementaries;
