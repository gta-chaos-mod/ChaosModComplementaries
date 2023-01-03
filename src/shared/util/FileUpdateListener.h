#pragma once

#include "util/Config.h"

#ifdef GTASA
#include <CAudioEngine.h>
#endif
#include <efsw/efsw.hpp>

class FileUpdateListener : public efsw::FileWatchListener
{
public:
    void
    handleFileAction (efsw::WatchID watchid, const std::string &dir,
                      const std::string &filename, efsw::Action action,
                      std::string oldFilename) override
    {
        if (action != efsw::Actions::Modified) return;
        if (filename != Config::GetConfigFilename ()) return;

        Config::ReloadConfig ();

#ifdef GTASA
        AudioEngine.ReportFrontendAudioEvent (AE_FRONTEND_DISPLAY_INFO, 0.0f,
                                              1.0f);
#endif
    }
};