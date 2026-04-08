#pragma once



class cGameSettings {
public:
    cGameSettings() = default;

    int getScreenW() const {
        return m_screenW;
    }
    int getScreenH() const {
        return m_screenH;
    }
    bool isDisableAI() const {
        return m_disableAI;
    }
    bool isOneAi() const {
        return m_oneAi;
    }
    bool isDisableWormAi() const {
        return m_disableWormAi;
    }
    bool isDisableReinforcements() const {
        return m_disableReinforcements;
    }
    bool isDrawUsages() const {
        return m_drawUsages;
    }
    bool isDrawUnitDebug() const {
        return m_drawUnitDebug;
    }
    bool isNoAiRest() const {
        return m_noAiRest;
    }
    bool isPlayMusic() const {
        return m_playMusic;
    }

    bool isPlaying() const {
        return m_playing;
    }
    bool isSkirmish() const {
        return m_skirmish;
    }
    bool shouldDrawFps() const {
        return m_drawFps;
    }
    bool shouldDrawTime() const {
        return m_drawTime;
    }

    bool isAllowRepeatingReinforcements() const {
        return m_allowRepeatingReinforcements;
    }
    int getPathsCreated() const {
        return m_pathsCreated;
    }
    int getMusicType() const {
        return m_musicType;
    }
    bool isPlaySound() const {
        return m_playSound;
    }
    bool isDebugMode() const {
        return m_debugMode;
    }
    bool isWindowed() const {
        return m_windowed;
    }

private:
    // resolution of the game
    int m_screenW;
    int m_screenH;

    bool m_disableAI;  
    bool m_oneAi;      
    bool m_disableWormAi;
    bool m_disableReinforcements;
    bool m_drawUsages;           
    bool m_drawUnitDebug;        
    bool m_noAiRest;             
    bool m_playMusic;            

    bool m_playing;
    bool m_skirmish;
    bool m_drawFps;
    bool m_drawTime;
    bool m_allowRepeatingReinforcements;

    int m_pathsCreated;
    int m_musicType;

    //-----------------------
    bool m_playSound;
    bool m_debugMode;
    bool m_windowed;
    //-----------------------

    friend class cGame;
};