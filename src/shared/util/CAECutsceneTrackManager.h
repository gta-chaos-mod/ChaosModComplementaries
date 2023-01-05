// https://github.com/gta-reversed/gta-reversed-modern/blob/master/source/game_sa/Audio/managers/AECutsceneTrackManager.h

#pragma once

class CAECutsceneTrackManager
{
public:
    enum Status : int
    {
        S0            = 0,
        S1            = 1,
        S2            = 2,
        STATE_PLAYING = 3,
        S4            = 4,
        S5            = 5,
        S6            = 6,
        S7            = 7,
        S8            = 8,
        S9            = 9,
        S10           = 10,
    };

    bool         m_bPlayRequest;
    bool         m_bPaused;
    int          m_nAudioChannel;
    int          m_nStatus;
    unsigned int m_nTrackId;
    int          _f10;
    unsigned int _f14;
    int          _f18;
    char         m_bIsUserTrack;
    char         _pad[4];
    unsigned int m_nPlayingTrackId;
    int          m_nTrackPlayTime;
    int          m_nTrackLengthMs;
};

extern CAECutsceneTrackManager &AECutsceneTrackManager;