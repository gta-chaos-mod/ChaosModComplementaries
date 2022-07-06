#include "GameHandler.h"

#include <plugin.h>

class ChaosModComplementaries
{
public:
    ChaosModComplementaries ()
    {
        Events::initRwEvent += GameHandler::Initialise;
    }
} chaosModComplementaries;
