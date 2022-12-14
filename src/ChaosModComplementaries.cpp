#include "GameHandler.h"

#include <plugin.h>

#define VERSION 1.2

using namespace plugin;

class ChaosModComplementaries
{
public:
    ChaosModComplementaries ()
    {
        Events::initRwEvent += GameHandler::Initialise;
    }
} chaosModComplementaries;
