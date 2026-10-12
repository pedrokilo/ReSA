/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLoadingScreen
 * Reconstrucción: COMPLETA — 20/20 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 20/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/

#include "StdInc.h"
#include "X87Math.h"
#include "platform.h"
#include "LoadingScreen.h"

#include "extensions/Configs/FastLoader.hpp"

void CLoadingScreen::InjectHooks() {
    RH_ScopedClass(CLoadingScreen);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x5902B0);
    RH_ScopedInstall(Shutdown, 0x58FF10);
    RH_ScopedInstall(RenderSplash, 0x58FF60);
    RH_ScopedInstall(LoadSplashes, 0x5900B0);
    RH_ScopedInstall(DisplayMessage, 0x590220);
    RH_ScopedInstall(SetLoadingBarMsg, 0x590240);
    RH_ScopedInstall(GetClockTime, 0x590280);
    RH_ScopedInstall(Pause, 0x590310);
    RH_ScopedInstall(Continue, 0x590320);
    RH_ScopedInstall(RenderLoadingBar, 0x590370);
    RH_ScopedInstall(DisplayNextSplash, 0x5904D0);
    RH_ScopedInstall(StartFading, 0x590530);
    RH_ScopedInstall(DisplayPCScreen, 0x590570);
    RH_ScopedInstall(Update, 0x5905E0);
    RH_ScopedInstall(DoPCTitleFadeIn, 0x590860);
    RH_ScopedInstall(DoPCTitleFadeOut, 0x590990);
    RH_ScopedInstall(DoPCScreenChange, 0x590AC0);
    RH_ScopedInstall(NewChunkLoaded, 0x590D00);
    RH_ScopedInstall(IsActive, 0x744DB0); // Antes 0x744DB5: era el ret; la función es mov al,[0xBAB318]; ret

    RH_ScopedGlobalInstall(LoadingScreen, 0x53DED0);
}

// Reloj del original (inline en varias funciones): milisegundos como entero sin signo cargados con fild,
// por 0.001 con la precisión del FPU del juego.
static float ClockSeconds() {
    const int32 ms = (int32)GetMillisecondTime();
    // fild con corrección de 2^32 si es negativo y fmul 0.001f (X87Math.cpp)
    const float secs = UnsignedMillisToSecondsCpp(uint32(ms));
    return secs;
}

// Escalado de pantalla del original: (ancho · 1/640) · valor y (alto · 1/448) · valor.
static float LoadingStretchX(float a) { return ((float)RsGlobal.maximumWidth * 0.0015625f) * a; }
static float LoadingStretchY(float a) { return ((float)RsGlobal.maximumHeight * 0.002232142857f) * a; }

// 0x5902B0
void CLoadingScreen::Init(bool isLegalScreen, bool isReloadDisabled) {
    if (IsActive()) {
        return;
    }

    if (!isReloadDisabled) {
        LoadSplashes(false, eLoadingLogo::EAX);
    }

    m_currDisplayedSplash = eDisplayedSplash::NONE;

    m_timeSinceLastScreen = GetClockTime();

    m_bActive = true;
}

// 0x58FF10
void CLoadingScreen::Shutdown(bool isForce) {
    m_bActive = false;

    // Release splash textures
    for (auto& splash : m_aSplashes) {
        if (splash.m_pTexture) {
            splash.Delete();
        }
    }

    if (const auto slot = CTxdStore::FindTxdSlot("loadscs"); slot != -1) {
        CTxdStore::RemoveTxd(slot);
        CTxdStore::RemoveTxdSlot(slot);
    }
}

// 0x58FF60
// Edit in Mobile
void CLoadingScreen::RenderSplash() {
    // Screen dimensions
    constexpr float SCREEN_MARGIN = 5.0f;

    CSprite2d::InitPerFrame();

    const CRect screenRect{
        -SCREEN_MARGIN,
        -SCREEN_MARGIN,
        SCREEN_WIDTH + SCREEN_MARGIN,
        SCREEN_HEIGHT + SCREEN_MARGIN
    };

    CRGBA color(255, 255, 255, 255);
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSCLAMP));

    if (m_bFading) {
        GetCurrentDisplayedSplash().Draw(screenRect, color);

        if (m_bFadeInNextSplashFromBlack || m_bFadeOutCurrSplashToBlack) {
            color.Set(0, 0, 0);
            color.a = m_bFadeInNextSplashFromBlack
                ? (255 - m_FadeAlpha)
                : m_FadeAlpha;
            CSprite2d::DrawRect(screenRect, color);
        } else {
            color.a = 255 - m_FadeAlpha;
            m_aSplashes.data()[+m_currDisplayedSplash - 1].Draw(screenRect, color);
        }
    } else if (!m_bReadyToDelete) {
        GetCurrentDisplayedSplash().Draw(screenRect, color);
    }
}

// 0x5900B0
// Edit in Mobile
void CLoadingScreen::LoadSplashes(bool useSplashId, eLoadingLogo id) {
    CFileMgr::SetDir("MODELS\\TXD\\");

    auto slot = CTxdStore::AddTxdSlot("loadscs");
    CTxdStore::LoadTxd(slot, "loadscs.txd");
    CTxdStore::AddRef(slot);
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(slot);

    // Orden aleatorio como el original: permutación de las 15 pantallas loadsc0..14 sacando una al azar
    // cada vez (la primera, la del título, siempre es la 0).
    uint8 pool[15];
    for (auto i = 0; i < 15; i++) {
        pool[i] = (uint8)i;
    }

    LARGE_INTEGER pc;
    QueryPerformanceCounter(&pc);
    srand(pc.u.LowPart);

    uint8 order[15];
    for (auto i = 0, remaining = 14; i < 15; i++, remaining--) {
        const auto pick = i == 0 ? 0 : (int32)((float)(CGeneral::GetRandomNumber() & 0xFFFF) * (1.0f / 32768.0f) * (float)(remaining + 1));
        order[i] = pool[pick];
        if (pick < remaining) {
            std::memmove(&pool[pick], &pool[pick + 1], remaining - pick);
        }
    }

    for (auto i = 0; i < +eDisplayedSplash::COUNT; i++) {
        char texName[20];
        if (useSplashId) {
            std::strcpy(texName, id == eLoadingLogo::NVIDIA ? "nvidia" : "eax");
        } else if (i == 0) {
#ifdef USE_EU_STUFF
            std::strcpy(texName, "title_pc_EU");
#else
            std::strcpy(texName, "title_pc_US");
#endif
        } else {
            std::sprintf(texName, "loadsc%d", order[i]);
        }
        m_aSplashes[i].SetTexture(texName);
    }

    CTxdStore::PopCurrentTxd();
    CFileMgr::SetDir("");
}

// 0x590220
void CLoadingScreen::DisplayMessage(const char* msg) {
    std::strcpy(m_PopUpMessage, msg);
}

// 0x590240
void CLoadingScreen::SetLoadingBarMsg(const char* msg1, const char* msg2) {
    if (msg1) {
        AsciiToGxtChar(msg1, m_LoadingGxtMsg1);
    } else {
        m_LoadingGxtMsg1[0] = '\0';
    }

    if (msg2) {
        AsciiToGxtChar(msg2, m_LoadingGxtMsg2);
    } else {
        m_LoadingGxtMsg2[0] = '\0';
    }
}

// 0x590280
float CLoadingScreen::GetClockTime(bool isRealTime) {
    const auto secs = ClockSeconds();
    return isRealTime ? secs : secs - m_PauseTime;
}

// 0x590310
void CLoadingScreen::Pause() {
    // NOP
}

// 0x590320
void CLoadingScreen::Continue() {
    if (!IsActive()) {
        return;
    }

    m_bWantToPause = false;
    if (m_bPaused) {
        m_bPaused = false;
        m_PauseTime = GetClockTime() - m_ClockTimeOnPause + m_PauseTime;
    }
}

// 0x590370
void CLoadingScreen::RenderLoadingBar() {
#ifdef PRINT_LOADMSG
    // NOTSA
    char loadingMsg[1024];
    *std::format_to(loadingMsg, "{}\n{}", m_LoadingGxtMsg1, m_LoadingGxtMsg2) = 0;
    CFont::SetOrientation(eFontAlignment::ALIGN_LEFT);
    CFont::SetDropShadowPosition(2);
    CFont::SetJustify(false);
    CFont::PrintString(SCREEN_STRETCH_X(50.0f), SCREEN_STRETCH_Y(40.0f), loadingMsg);
    CFont::RenderFontBuffer();
#endif

    // Como el original: el color se pide componente a componente y la geometría con su escalado
    CRGBA color;
    color.r = HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY).r;
    color.g = HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY).g;
    color.b = HudColour.GetRGB(HUD_COLOUR_LIGHT_GRAY).b;
    color.a = 255;

    const float x      = LoadingStretchX(50.0f);
    const float hScale = (float)RsGlobal.maximumHeight * 0.002232142857f;
    const float y      = (float)RsGlobal.maximumHeight - hScale * 40.0f;
    const auto  width  = (int32)LoadingStretchX(180.0f);
    const auto  height = (int32)(hScale * 10.0f);

    if (m_bLegalScreen || !(m_PercentLoaded > 0.0f) || !(m_PercentLoaded < 100.0f)) {
        return;
    }

    CSprite2d::DrawBarChart(x, y, (uint16)width, (uint8)height, m_PercentLoaded, 0, false, true, color, CRGBA{ 0, 0, 0, 0 });

    // El original solo anota el momento en que aparece la barra después de dibujarla
    if (m_TimeBarAppeared == 0.0f) {
        m_TimeBarAppeared = GetClockTime(false);
    }
}

// 0x5904D0
void CLoadingScreen::DisplayNextSplash() {
    if (m_currDisplayedSplash == eDisplayedSplash::SPLASH6 || m_bFading) {
        return;
    }

    m_FadeAlpha = 255;

    if (RwCameraBeginUpdate(Scene.m_pRwCamera)) {
        DefinedState2d();
        RenderSplash();
        RenderLoadingBar();
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RsCameraShowRaster(Scene.m_pRwCamera);
    }

    m_currDisplayedSplash = (eDisplayedSplash)(+m_currDisplayedSplash + 1);
}

// 0x590530
void CLoadingScreen::StartFading() {
    m_bFading = true;
    m_FadeAlpha = 0;
    m_StartFadeTime = GetClockTime(false);
}

// inline
// 0x590570
inline void CLoadingScreen::DisplayPCScreen() {
    if (RwCameraBeginUpdate(Scene.m_pRwCamera)) {
        DefinedState2d();
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RenderSplash();
        if (!g_FastLoaderConfig.NoLoadBar) {
            if (m_currDisplayedSplash > eDisplayedSplash::COPYRIGHT && (!m_bFading || m_currDisplayedSplash != eDisplayedSplash::SPLASH_1_NO_BAR)) {
                RenderLoadingBar();
            }
        }
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RsCameraShowRaster(Scene.m_pRwCamera);
    }
}

// 0x590860
void CLoadingScreen::DoPCTitleFadeIn() {
    m_bFadeInNextSplashFromBlack = true;
    m_currDisplayedSplash = eDisplayedSplash::COPYRIGHT;
    m_bFading = true;

    for (auto i = 0; i < 50; i++) {
        m_FadeAlpha = static_cast<uint8>(i * 5.0f);
        DisplayPCScreen();
    }

    m_FadeAlpha = 255;
    DisplayPCScreen();
    m_bFading = false;
}

// 0x590990
void CLoadingScreen::DoPCTitleFadeOut() {
    m_bFadeInNextSplashFromBlack = true;
    m_currDisplayedSplash = eDisplayedSplash::COPYRIGHT;
    m_bFading = true;

    for (auto i = 200u; i > 0; i--) {
        m_FadeAlpha = 255;
        DisplayPCScreen();
    }

    for (auto i = 50u; i > 0u; i--) {
        m_FadeAlpha = static_cast<uint8>(i * 5.0f);
        DisplayPCScreen();
    }

    m_bFadeInNextSplashFromBlack = true;
}

// 0x590AC0
// Edit in Mobile
void CLoadingScreen::DoPCScreenChange(bool isLastOne) {
    m_bFading = true;

    if (isLastOne) {
        m_bFadeOutCurrSplashToBlack = true;
    } else {
        // Como el original (inc 0x8D093C): siempre se avanza, aunque pase de la última pantalla
        m_currDisplayedSplash = (eDisplayedSplash)(+m_currDisplayedSplash + 1);
    }

    if (!g_FastLoaderConfig.NoFading) {
        for (auto i = 20; i > 0; i--) {
            m_FadeAlpha = 0;
            DisplayPCScreen();
        }
    }

    for (auto i = 0u; i < 50u; i++) {
        m_FadeAlpha = (uint8)((float)i * 5.0f);

        if (isLastOne || m_bFadeInNextSplashFromBlack) {
            // Volumen del tema: baja con el fundido a negro final (alpha ya truncado a byte)
            AudioEngine.ServiceLoadingTune(m_bFadeOutCurrSplashToBlack ? (255.0f - (float)m_FadeAlpha) * (1.0f / 255.0f) : 1.0f);
        }

        DisplayPCScreen();
    }

    m_FadeAlpha = 255;
    DisplayPCScreen();
    m_bFadeInNextSplashFromBlack = false;
    m_bFading = false;

    if (isLastOne) {
        Shutdown();
    }
}

// 0x590D00
// Edit in Mobile
void CLoadingScreen::NewChunkLoaded() {
    if (!IsActive()) {
        return;
    }

    ++m_numChunksLoaded;

    if (m_chunkBarAppeared != -1) {
        m_PercentLoaded = ((float)(m_numChunksLoaded - m_chunkBarAppeared) * 100.0f) / (float)(140 - m_chunkBarAppeared);
    }

    const auto now   = ClockSeconds();
    const auto delta = now - m_timeSinceLastScreen;

    if (m_numChunksLoaded == 140) {
        return DoPCScreenChange(true);
    }

    // Como el original: 5.5 s en la pantalla de copyright y 5 s (ScreenChangeTime) en las demás, incluida NONE
    const auto limit = m_currDisplayedSplash == eDisplayedSplash::COPYRIGHT ? 5.5f : g_FastLoaderConfig.ScreenChangeTime;
    if (delta < limit) {
        if (!g_FastLoaderConfig.NoLoadScreen || !g_FastLoaderConfig.NoLoadBar) {
            DisplayPCScreen();
        }
    } else { // New splash screen
        DoPCScreenChange(false);
        m_timeSinceLastScreen = now;

        if (m_chunkBarAppeared == -1) {
            m_chunkBarAppeared = m_numChunksLoaded;
        }
    }
}

// 0x5905E0
// unused. Transcrito del original (la reconstrucción anterior no coincidía).
void CLoadingScreen::Update() {
    if (m_TimeBarAppeared > 0.0f) {
        const auto elapsed = (GetClockTime(false) - m_TimeBarAppeared) * 100.0f;
        m_PercentLoaded = elapsed / (36.0f - (m_TimeBarAppeared - m_TimeStartedLoading));
        if (m_PercentLoaded > 100.0f) {
            m_PercentLoaded = 100.0f;
        }
    }

    bool fadeBlock = m_bFading;
    if (!m_bReadyToDelete) {
        if (m_bLegalScreen) {
            if (GetClockTime(false) - m_TimeStartedLoading > 5.5f) {
                DisplayNextSplash();
            }
        } else if ((float)+m_currDisplayedSplash * 16.666666f < m_PercentLoaded) {
            DisplayNextSplash();
        }

        if (m_bSignalDelete) {
            if (!m_bFading) {
                if (!(m_PercentLoaded >= 100.0f)) { // por debajo de 100 (o NaN): alfa 255 y al audio
                    m_FadeAlpha = 255;
                    fadeBlock = false;
                    goto audio;
                }
                m_bReadyToDelete            = true;
                m_bFadeOutCurrSplashToBlack = true;
                StartFading();
            }
        }
        fadeBlock = m_bFading;
    }

    if (fadeBlock) {
        const auto elapsed  = GetClockTime(false) - m_StartFadeTime;
        const auto duration = (+m_currDisplayedSplash >= 0 && (m_bLegalScreen || m_bFadeInNextSplashFromBlack)) ? 0.6f : 2.0f;
        if (elapsed > duration) {
            m_bFading   = false;
            m_FadeAlpha = 255;
            if (m_bLegalScreen && m_bFadeOutCurrSplashToBlack) {
                m_bFadeOutCurrSplashToBlack  = false;
                m_bFadeInNextSplashFromBlack = true;
                m_bLegalScreen               = false;
                DisplayNextSplash();
            } else {
                m_bFadeInNextSplashFromBlack = false;
                m_bFadeOutCurrSplashToBlack  = false;
            }
        } else {
            m_FadeAlpha = (uint8)(int32)((elapsed / duration) * 255.0f);
        }
    } else {
        m_FadeAlpha = 255;
    }

audio:
    if (!m_bLegalScreen) {
        float volume;
        if (m_bFadeInNextSplashFromBlack) {
            volume = (float)m_FadeAlpha * (1.0f / 255.0f);
        } else if (m_bFadeOutCurrSplashToBlack) {
            volume = (255.0f - (float)m_FadeAlpha) * (1.0f / 255.0f);
        } else {
            volume = 1.0f;
        }
        AudioEngine.ServiceLoadingTune(volume);
    }
}

// NOTSA
void CLoadingScreen::SkipCopyrightSplash() {
    m_currDisplayedSplash = eDisplayedSplash::COPYRIGHT; // Copyright splash
#ifndef FIX_BUGS // Fixed this in DoPCScreenChange
    m_timeSinceLastScreen -= 1000.f; // Decrease timeSinceLastScreen, so it will change immediately
#endif // !FIX_BUGS
    m_bFadeInNextSplashFromBlack = true; // First Loading Splash
}

// 0x53DED0
void LoadingScreen(const char* msg1, const char* msg2, const char* msg3) {
    if (msg1) {
        if (!g_FastLoaderConfig.NoDbgLogScreens) { // Very slow, so skip it
            NOTSA_LOG_DEBUG("Loadingscreen: {} [{}][{}]", msg1, msg2 ? msg2 : "NULL", msg3 ? msg3 : "NULL");
        }
        CLoadingScreen::SetLoadingBarMsg(msg1, msg2);
    }
    CLoadingScreen::NewChunkLoaded();
}
