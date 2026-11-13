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
 * Validación registrada: 2/2 entradas.
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
    RH_ScopedGlobalInstall(DoFade, 0x53E600); // lote 185A (sin validar)
}

// 0x6FF410
void CDraw::SetFOV(float fov) {
    ms_fFOV = fov;
}

// 0x6FF420
// Como el original: devuelve la relación calculada en ST0 (fst); los llamadores del EXE la desapilan
// con fstp st(0) (0x53D699, 0x53E775...), así que una versión void desequilibraría la pila x87.
float CDraw::CalculateAspectRatio() {
    if (FrontEndMenuManager.m_bWidescreenOn)
        ms_fAspectRatio = 16.f / 9.f;
    else if (TheCamera.m_bWideScreenOn)
        ms_fAspectRatio = 5.f / 4.f;
    else
        ms_fAspectRatio = 4.f / 3.f;
    return ms_fAspectRatio;
}

// 0x53E600 (lote 185A, sin validar; el listado decia Game; cdecl, sin argumentos, `ret`, 353 bytes): fundido de pantalla. Sin pausa (B7CB49 y B7CB48 a cero): si JustLoadedDontFadeInYet (0xC16EDC) es 1 toma la hora como inicio
// (0xC16EE0) y lo apaga; si StillToFadeOut (0xC16EDD) es 1: pasado FadeOutTime (0x8D2BD4, comparacion sin signo "mayor") apaga el indicador y hace Fade(3.0f, FADE_OUT) + ProcessFade + ProcessMusicFade; si no, color 0,0,0 +
// Fade(0.0f, FADE_IN) + ProcessFade. Con FadeValue (0xC3EFAB) distinto de cero dibuja un rectangulo (-5, -5, ancho + 5, alto + 5) en memoria (x1, y1, x2, y2) con el color (R*v/v, G*v/v, B*v/v, v), o 0,0,0,0 si
// el destino del fundido es la pantalla de carga. Como el original no hay variante FIX_BUGS: el EXE pasa top = -5 y bottom = alto + 5.
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

        // Como el original: ancho y alto se suman como ENTEROS (con desbordamiento) antes de pasar a float (fild + fstp)
        const auto right  = static_cast<float>(static_cast<int32>(static_cast<uint32>(RsGlobal.maximumWidth) + 5u));
        const auto bottom = static_cast<float>(static_cast<int32>(static_cast<uint32>(RsGlobal.maximumHeight) + 5u));
        // El rectangulo en memoria es (x1, y1, x2, y2) = (-5, -5, ancho + 5, alto + 5): el constructor de CRect (izquierda, abajo, derecha, arriba) lo da por invertido (assert) con y1 < y2, asi que se rellenan los campos
        CRect rect;
        rect.left   = -5.0f;
        rect.top    = -5.0f;
        rect.right  = right;
        rect.bottom = bottom;
        CSprite2d::DrawRect(rect, color);
    }
}
