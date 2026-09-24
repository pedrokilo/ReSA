/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDraw
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Draw.h"

auto& FadeTimer = StaticRef<uint32>(0xC16EE0);
auto& FadeOutTime = StaticRef<uint32>(0x8D2BD4);

void CDraw::InjectHooks() {
    RH_ScopedClass(CDraw);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetFOV, 0x6FF410);
    RH_ScopedInstall(CalculateAspectRatio, 0x6FF420);
}

// 0x6FF410
void CDraw::SetFOV(float fov) {
    ms_fFOV = fov;
}

// 0x6FF420
void CDraw::CalculateAspectRatio() {
    if (FrontEndMenuManager.m_bWidescreenOn)
        ms_fAspectRatio = 16.f / 9.f;
    else if (TheCamera.m_bWideScreenOn)
        ms_fAspectRatio = 5.f / 4.f;
    else
        ms_fAspectRatio = 4.f / 3.f;
}

// 0x53E600
void DoFade() {
    ZoneScoped;

    if (CTimer::GetIsPaused())
        return;

    if (JustLoadedDontFadeInYet) {
        FadeTimer = CTimer::GetTimeInMS();
        JustLoadedDontFadeInYet = false;
    }

    if (StillToFadeOut) {
        if (CTimer::GetTimeInMS() - FadeTimer <= FadeOutTime) {
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(0.0f, eFadeFlag::FADE_IN);
            TheCamera.ProcessFade();
        } else {
            StillToFadeOut = false;

            TheCamera.Fade(3.0f, eFadeFlag::FADE_OUT);
            TheCamera.ProcessFade();
            TheCamera.ProcessMusicFade();
        }
    }

    if (CDraw::IsFading()) {
        CRGBA color(0, 0, 0, 0);

        if (!TheCamera.m_bFadeTargetIsSplashScreen) {
            color.Set(
                (CDraw::FadeRed   * CDraw::FadeValue) / CDraw::FadeValue,
                (CDraw::FadeGreen * CDraw::FadeValue) / CDraw::FadeValue,
                (CDraw::FadeBlue  * CDraw::FadeValue) / CDraw::FadeValue,
                std::max<uint8>(0, CDraw::FadeValue)
            );
        }

        #if FIX_BUGS
        CSprite2d::DrawRect({-5.0f, -5.0f, SCREEN_WIDTH + 5.0f, SCREEN_HEIGHT + 5.0f}, color);
        #else
        CSprite2d::DrawRect({-5.0f, SCREEN_HEIGHT + 5.0f, SCREEN_WIDTH + 5.0f, -5.0f}, color);
        #endif
    }
}
