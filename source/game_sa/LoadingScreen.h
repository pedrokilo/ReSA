/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#pragma once
#include "DatosExe/DataBssE5.h"
#include "DatosExe/DataBssE3.h"
#include "DatosExe/DataBssE1.h"
#include "DatosExe/DataInicializadaB.h"


enum class eLoadingLogo : uint8 {
    EAX = 0,
    NVIDIA = 1
};

enum class eDisplayedSplash : int32 {
    NONE = -1,

    COPYRIGHT = 0,
    SPLASH_1_NO_BAR = 1,
    SPLASH2,
    SPLASH3,
    SPLASH4,
    SPLASH5,
    SPLASH6,

    COUNT
};

class CSprite2d;

class CLoadingScreen {
private:
    static inline auto& m_bActive = DatosExe::Bss::bBAB318;
    static inline auto& m_bWantToPause = DatosExe::Bss::bBAB319; // unused
    static inline auto& m_bPaused = DatosExe::Bss::bBAB31A; // unused
    static bool m_bForceShutdown; // unknown, unused
    static inline auto& m_aSplashes = StaticRef<std::array<CSprite2d, +eDisplayedSplash::COUNT>>(0xBAB35C);
    static inline auto& m_currDisplayedSplash = StaticRef<eDisplayedSplash>(0x8D093C);

    static inline auto& m_bFading = DatosExe::Bss::bBAB31C;
    static inline auto& m_bLegalScreen = DatosExe::Bss::bBAB31D;
    static inline auto& m_bFadeInNextSplashFromBlack = DatosExe::Bss::bBAB31E;
    static inline auto& m_bFadeOutCurrSplashToBlack = DatosExe::Bss::bBAB31F;
    static inline auto& m_FadeAlpha = DatosExe::Bss::bBAB320;
    static inline auto& m_StartFadeTime = DatosExe::Bss::bBAB324;
    static inline auto& m_ClockTimeOnPause = DatosExe::Bss::bBAB328; // unused
    static inline auto& m_PauseTime = DatosExe::Bss::bBAB32C; // second

    static inline auto& m_PercentLoaded = DatosExe::Bss::bBAB330;
    static inline auto& m_TimeStartedLoading = DatosExe::Bss::bBAB334; // unused
    static inline auto& m_TimeBarAppeared = DatosExe::Bss::bBAB338;

    static inline auto& m_LoadingGxtMsg1 = DatosExe::Bss::bBAB2C8;
    static inline auto& m_LoadingGxtMsg2 = DatosExe::Bss::bBAB278;

    static inline auto& m_PopUpMessage = DatosExe::Bss::bBAB268;

    static inline auto& m_bSignalDelete = DatosExe::Bss::bBAB33C; // unused
    static inline auto& m_bReadyToDelete = DatosExe::Bss::bBAB33D;

    static inline auto& m_numChunksLoaded = DatosExe::Data::v8D0940; // -1
    // static int32 m_nChunksToLoad; // Added in Mobile
    static inline auto& m_chunkBarAppeared = DatosExe::Data::v8D0944; // -1
    static inline auto& m_timeSinceLastScreen = DatosExe::Bss::bBAB340;

public:
    static void LoadSplashes(bool useSplashId, eLoadingLogo id);

    [[nodiscard]] static bool IsActive() { return m_bActive; } // 0x744DB0
    [[nodiscard]] static bool IsPaused() { return m_bPaused; }

    static void Init(bool isLegalScreen, bool isReloadDisabled);
    static void Shutdown(bool isForce = false);

    // static bool Paused(); // unknown
    static void Pause();
    static void Continue();

    static void SetLoadingBarMsg(const char* msg, const char* msg2);

    static void DisplayMessage(const char* msg);
    static void DisplayNextSplash();
    static void RenderSplash();
    static void NewChunkLoaded();

    // static void Pump(); // Added in Mobile

    // static void SetChunksToLoad(int32 num, bool AddTo); // Added in Mobile

    static void DoPCTitleFadeOut();
    static void DoPCTitleFadeIn();
    static void DoPCScreenChange(bool isLastOne);

private:
    static void DisplayPCScreen();

    static void RenderLoadingBar();

    static void Update();

    static void StartFading();

    [[nodiscard]] static float GetClockTime(bool isRealTime = true);

private: // NOTSA
    friend void InjectHooksMain();
    static void InjectHooks();

public:
    // Sin comprobar límites, como el original: con NONE (-1) lee la entrada anterior a la tabla
    static CSprite2d& GetCurrentDisplayedSplash() { return m_aSplashes.data()[+m_currDisplayedSplash]; }

    // Skips the copyright splash
    static void SkipCopyrightSplash();
};

void LoadingScreen(const char* msg1, const char* msg2 = nullptr, const char* msg3 = nullptr);
