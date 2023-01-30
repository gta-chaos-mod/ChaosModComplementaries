#pragma once

#include "missions/M_BurningDesire.h"
#include "missions/M_SaintMarcsBistro.h"

class Missions
{
public:
    static void
    Initialise ()
    {
        m_SaintMarcsBistro.Initialise ();
        m_BurningDesire.Initialise ();
    }

    static void
    ProcessGame ()
    {
    }

    static void
    ProcessScripts ()
    {
        m_BurningDesire.ProcessScripts ();
    }
};