/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Light
 * Reconstrucción: COMPLETA — 27/27 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 27/27 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "app.h"
#include "PostEffects.h"

void AppLightInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("Light");

    RH_ScopedGlobalInstall(ActivateDirectional, 0x735C80);
    RH_ScopedGlobalInstall(DeActivateDirectional, 0x735C70);
    RH_ScopedGlobalInstall(LightsCreate, 0x5BA520);
    RH_ScopedGlobalInstall(LightsDestroy, 0x735730);
    RH_ScopedGlobalInstall(LightsEnable, 0x735720);
    RH_ScopedGlobalInstall(SetLightsWithTimeOfDayColour, 0x7354E0);
    RH_ScopedGlobalInstall(WorldReplaceNormalLightsWithScorched, 0x7357E0);
    RH_ScopedGlobalInstall(WorldReplaceScorchedLightsWithNormal, 0x735820);
    RH_ScopedGlobalInstall(AddAnExtraDirectionalLight, 0x735840);
    RH_ScopedGlobalInstall(RemoveExtraDirectionalLights, 0x7359E0);
    RH_ScopedGlobalInstall(SetAmbientAndDirectionalColours, 0x735A20);
    RH_ScopedGlobalInstall(ReSetAmbientAndDirectionalColours, 0x735C40);
    RH_ScopedGlobalInstall(SetFlashyColours, 0x735AB0);
    RH_ScopedGlobalInstall(SetFlashyColours_Mild, 0x735B40);
    RH_ScopedGlobalInstall(SetBrightMarkerColours, 0x735BD0);
    RH_ScopedGlobalInstall(SetDirectionalColours, 0x735D70);
    RH_ScopedGlobalInstall(SetAmbientColoursToIndicateRoadGroup, 0x735C90);
    RH_ScopedGlobalInstall(SetFullAmbient, 0x735D10);
    RH_ScopedGlobalOverloadedInstall(SetAmbientColours, "void", 0x735D30, void(*)());
    RH_ScopedGlobalOverloadedInstall(SetAmbientColours, "color", 0x735D50, void(*)(RwRGBAReal* color));
    RH_ScopedGlobalInstall(SetLightColoursForPedsCarsAndObjects, 0x735D90);
    RH_ScopedGlobalInstall(SetLightsForInfraredVisionHeatObjects, 0x735E40);
    RH_ScopedGlobalInstall(StoreAndSetLightsForInfraredVisionHeatObjects, 0x735E70);
    RH_ScopedGlobalInstall(RestoreLightsForInfraredVisionHeatObjects, 0x735EF0);
    RH_ScopedGlobalInstall(SetLightsForInfraredVisionDefaultObjects, 0x735F20);
    RH_ScopedGlobalInstall(SetLightsForNightVision, 0x735F70);
    RH_ScopedGlobalInstall(GetDayNightBalance, 0x6FAB30);
}

// 0x735C80
void ActivateDirectional() {
    RpLightSetFlags(pDirect, rpLIGHTLIGHTATOMICS);
}

// 0x735C70
void DeActivateDirectional() {
    RpLightSetFlags(pDirect, 0x0);
}

// 0x5BA520
void LightsCreate(RpWorld* world) {
    if (!world) {
        return;
    }

    RwRGBAReal color;

    pAmbient = RpLightCreate(rpLIGHTAMBIENT);
    RpLightSetFlags(pAmbient, rpLIGHTLIGHTATOMICS);
    color.red   = 0.25f;
    color.green = 0.25f;
    color.blue  = 0.20f;
    RpLightSetColor(pAmbient, &color);

    pDirect = RpLightCreate(rpLIGHTDIRECTIONAL);
    RpLightSetFlags(pDirect, rpLIGHTLIGHTATOMICS);
    color.red   = 1.00f;
    color.green = 0.85f;
    color.blue  = 0.45f;
    RpLightSetColor(pDirect, &color);
    RpLightSetRadius(pDirect, 2.0f);

    RwFrame* frame = RwFrameCreate();
    rwObjectHasFrameSetFrame(pDirect, frame);
    CVector vecLight = { 1.0f, 1.0f, 0.0f };
    RwFrameRotate(frame, &vecLight, 160.0f, rwCOMBINEPRECONCAT);

    RpWorldAddLight(world, pAmbient);
    RpWorldAddLight(world, pDirect);

    for (auto& light : pExtraDirectionals) {
        light = RpLightCreate(rpLIGHTDIRECTIONAL);
        RpLightSetFlags(light, NULL);

        color.red   = 1.0f;
        color.green = 0.5f;
        color.blue  = 0.0f;
        RpLightSetColor(light, &color);
        RpLightSetRadius(light, 2.0f);
        rwObjectHasFrameSetFrame(light, RwFrameCreate());

        RpWorldAddLight(world, light);
    }
}

// 0x735730
void LightsDestroy(RpWorld* world) {
    if (!world) {
        return;
    }
    const auto DestroyLight = [world](RpLight*& light, bool destroyFrame) {
        if (light) {
            RpWorldRemoveLight(world, light);
            if (destroyFrame) {
                RwFrameDestroy(RpLightGetFrame(light));
            }
            RpLightDestroy(light);
            light = nullptr;
        }
    };
    // Como el original: destruye la luz y luego pone el puntero global a nullptr.
    DestroyLight(pAmbient, false);
    DestroyLight(pDirect, true);
    for (auto& light : pExtraDirectionals) {
        DestroyLight(light, true);
    }
}

// 0x735720 unused
void LightsEnable(int32 enable) {
    // NOP
}

// 0x7354E0
void SetLightsWithTimeOfDayColour(RpWorld* world) {
    ZoneScoped;

    // Como el original: no usa `world`.
    if (pAmbient) {
        AmbientLightColourForFrame = {
            CTimeCycle::GetAmbientRed()   * CCoronas::LightsMult,
            CTimeCycle::GetAmbientGreen() * CCoronas::LightsMult,
            CTimeCycle::GetAmbientBlue()  * CCoronas::LightsMult,
            AmbientLightColourForFrame.alpha
        };
        
        AmbientLightColourForFrame_PedsCarsAndObjects = {
            CTimeCycle::GetAmbientRed_Obj()   * CCoronas::LightsMult, 
            CTimeCycle::GetAmbientGreen_Obj() * CCoronas::LightsMult, 
            CTimeCycle::GetAmbientBlue_Obj()  * CCoronas::LightsMult,
            AmbientLightColourForFrame_PedsCarsAndObjects.alpha
        };

        if (CWeather::LightningFlash) {
            AmbientLightColourForFrame_PedsCarsAndObjects = { 1.f, 1.f, 1.f, AmbientLightColourForFrame_PedsCarsAndObjects.alpha };
            AmbientLightColourForFrame                    = { 1.f, 1.f, 1.f, AmbientLightColourForFrame.alpha };
        }

        RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
    }

    if (pDirect) {
        const float color = CTimeCycle::m_CurrentColours.m_fIllumination * 0.99609375f * CCoronas::LightsMult; // todo: magic
        DirectionalLightColourForFrame.red   = color;
        DirectionalLightColourForFrame.green = color;
        DirectionalLightColourForFrame.blue  = color;
        RpLightSetColor(pDirect, &DirectionalLightColourForFrame);

        const auto& dir  = CTimeCycle::m_vecDirnLightToSun;
        const auto right = CVector::ZAxisVector().Cross(dir).Normalized(); // NOTE: I don't normalization is needed here
        const auto up    = right.Cross(dir);

        // Como el original: flags, rellenos y pos de la matriz quedan sin inicializar (pila).
        RwMatrix transform; // RW has y and z swapped, so don't get confused
        transform.right = up;
        transform.up    = right;
        transform.at    = -dir;
        RwFrameTransform(RpLightGetFrame(pDirect), &transform, rwCOMBINEREPLACE);
    }
}

// lighting = [ 0.0f; 1.0f ]
// 0x7357E0
void WorldReplaceNormalLightsWithScorched(RpWorld* world, float lighting) {
    // Como el original: el alpha queda sin inicializar.
    RwRGBAReal color;
    color.red   = lighting;
    color.green = lighting;
    color.blue  = lighting;
    RpLightSetColor(pAmbient, &color);
    DeActivateDirectional();
}

// unused
// 0x735820
void WorldReplaceScorchedLightsWithNormal(RpWorld* world) {
    RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
    ActivateDirectional();
}

// 0x735840
void AddAnExtraDirectionalLight(RpWorld* world, float x, float y, float z, float red, float green, float blue) {
    // Como el original: (red > green ? red : green), luego compara con blue (NaN y ceros con signo incluidos).
    const float maxRG    = red > green ? red : green;
    const float strength = maxRG > blue ? maxRG : blue;
    const int32 numDirLights = CGame::currArea != AREA_CODE_NORMAL_WORLD ? 6 : 4;

    const auto slot = [&] {
        if (NumExtraDirectionalLights < numDirLights) {
            return NumExtraDirectionalLights;
        }

        // Como el original: empieza en -1 y solo recorre las `numDirLights` primeras.
        int32 msIdx = -1;
        float msValue{ strength };
        for (int32 i = 0; i < numDirLights; i++) {
            if (msValue > LightStrengths[i]) {
                msIdx = i;
                msValue = LightStrengths[i];
            }
        }
        return msIdx;
    }();

    if (slot < 0) {
        return;
    }

    // Como el original: el alpha queda sin inicializar.
    RwRGBAReal color;
    color.red   = red;
    color.green = green;
    color.blue  = blue;
    RpLightSetColor(pExtraDirectionals[slot], &color);

    auto* frame = RpLightGetFrame(pExtraDirectionals[slot]);
    auto* mat   = RwFrameGetMatrix(frame);
    *RwMatrixGetAt(mat) = { -x, -y, -z };
    RwMatrixUpdate(mat);
    RwFrameUpdateObjects(frame);

    // RpLightSetFlags "returns" the first arg, which is marked [[nodiscard]], so we have to use it.
    (void)RpLightSetFlags(pExtraDirectionals[slot], rpLIGHTLIGHTATOMICS);

    LightStrengths[slot] = strength;
    NumExtraDirectionalLights = std::min(NumExtraDirectionalLights + 1, numDirLights);
}

// 0x7359E0
void RemoveExtraDirectionalLights(RpWorld* world) {
    for (auto& light : pExtraDirectionals) {
        RpLightSetFlags(light, 0x0);
    }
    NumExtraDirectionalLights = 0;
}

// fMult = [ 0.0f; 1.0f ]
// used in SetFlashyColours and SetFlashyColours_Mild which unused
// 0x735A20
void SetAmbientAndDirectionalColours(float fMult) {
    AmbientLightColour.red       = fMult * AmbientLightColourForFrame.red;
    AmbientLightColour.green     = fMult * AmbientLightColourForFrame.green;
    AmbientLightColour.blue      = fMult * AmbientLightColourForFrame.blue;

    DirectionalLightColour.red   = fMult * DirectionalLightColourForFrame.red;
    DirectionalLightColour.green = fMult * DirectionalLightColourForFrame.green;
    DirectionalLightColour.blue  = fMult * DirectionalLightColourForFrame.blue;

    RpLightSetColor(pAmbient, &AmbientLightColour);
    RpLightSetColor(pDirect,  &DirectionalLightColour);
}

// 0x735C40
void ReSetAmbientAndDirectionalColours() {
    RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
    RpLightSetColor(pDirect, &DirectionalLightColourForFrame);
}

// fMult = [ 0.0f; 1.0f ]
// 0x735AB0 unused
void SetFlashyColours(float fMult) {
    if ((CTimer::GetTimeInMS() & 256) != 0) {
        AmbientLightColour.red   = 1.0f;
        AmbientLightColour.green = 1.0f;
        AmbientLightColour.blue  = 1.0f;
        // Como el original: copia solo rgb, no el alpha.
        DirectionalLightColour.red   = DirectionalLightColourForFrame.red;
        DirectionalLightColour.green = DirectionalLightColourForFrame.green;
        DirectionalLightColour.blue  = DirectionalLightColourForFrame.blue;

        RpLightSetColor(pAmbient, &AmbientLightColour);
        RpLightSetColor(pDirect,  &DirectionalLightColour);
        return;
    }
    SetAmbientAndDirectionalColours(fMult * 0.75f);
}

// fMult = [ 0.0f; 1.0f ]
// 0x735B40 unused
void SetFlashyColours_Mild(float fMult) {
    if ((CTimer::GetTimeInMS() & 256) != 0) {
        AmbientLightColour.red   = 0.65f;
        AmbientLightColour.green = 0.65f;
        AmbientLightColour.blue  = 0.65f;
        // Como el original: copia solo rgb, no el alpha.
        DirectionalLightColour.red   = DirectionalLightColourForFrame.red;
        DirectionalLightColour.green = DirectionalLightColourForFrame.green;
        DirectionalLightColour.blue  = DirectionalLightColourForFrame.blue;

        RpLightSetColor(pAmbient, &AmbientLightColour);
        RpLightSetColor(pDirect,  &DirectionalLightColour);
        return;
    }
    SetAmbientAndDirectionalColours(fMult * 0.9f);
}

// 0x735BD0
void SetBrightMarkerColours(float lighting) {
    UNUSED(lighting);
    AmbientLightColour.red   = 0.6f;
    AmbientLightColour.green = 0.6f;
    AmbientLightColour.blue  = 0.6f;

    DirectionalLightColour.red   = 1.0f;
    DirectionalLightColour.green = 1.0f;
    DirectionalLightColour.blue  = 1.0f;

    RpLightSetColor(pAmbient, &AmbientLightColour);
    RpLightSetColor(pDirect,  &DirectionalLightColour);
}

// 0x735D70
void SetDirectionalColours(RwRGBAReal* color) {
    assert(color);
    RpLightSetColor(pDirect, color);
}

// unused
// 0x735C90
void SetAmbientColoursToIndicateRoadGroup(int32 idx) {
    static auto& IndicateR = StaticRef<std::array<uint8, 8>>(0x8D60D0);
    static auto& IndicateG = StaticRef<std::array<uint8, 8>>(0x8D60D8);
    static auto& IndicateB = StaticRef<std::array<uint8, 8>>(0x8D60E0);

    // Como el original: índice idx % 7 con signo (cdq/idiv); un idx negativo lee antes de la tabla.
    const int32 i = idx % 7;
    AmbientLightColour.red   = (float)IndicateR.data()[i] * (1.0f / 255.0f);
    AmbientLightColour.green = (float)IndicateG.data()[i] * (1.0f / 255.0f);
    AmbientLightColour.blue  = (float)IndicateB.data()[i] * (1.0f / 255.0f);
    RpLightSetColor(pAmbient, &AmbientLightColour);
}

// unused
// 0x735D10
void SetFullAmbient() {
    static auto& FullLight = StaticRef<RwRGBAReal>(0x8D60C0); // { 1.0f, 1.0f, 1.0f, 1.0f }
    RpLightSetColor(pAmbient, &FullLight);
}

// 0x735D30
void SetAmbientColours() {
    RpLightSetColor(pAmbient, &AmbientLightColourForFrame);
}

// 0x735D50
void SetAmbientColours(RwRGBAReal* color) {
    assert(color);
    RpLightSetColor(pAmbient, color);
}

// fMult = [ 0.0f; 1.0f ]
// 0x735D90
void SetLightColoursForPedsCarsAndObjects(float fMult) {
    DirectionalLightColour.red   = fMult * DirectionalLightColourForFrame.red;
    DirectionalLightColour.green = fMult * DirectionalLightColourForFrame.green;
    DirectionalLightColour.blue  = fMult * DirectionalLightColourForFrame.blue;

    AmbientLightColour.red       = fMult * AmbientLightColourForFrame_PedsCarsAndObjects.red;
    AmbientLightColour.green     = fMult * AmbientLightColourForFrame_PedsCarsAndObjects.green;
    AmbientLightColour.blue      = fMult * AmbientLightColourForFrame_PedsCarsAndObjects.blue;

    // Como el original: cada canal suma su propio brillo (0xB79E38/34/30).
    AmbientLightColour.red      += CTimeCycle::m_BrightnessAddedToAmbientRed;
    AmbientLightColour.green    += CTimeCycle::m_BrightnessAddedToAmbientGreen;
    AmbientLightColour.blue     += CTimeCycle::m_BrightnessAddedToAmbientBlue;

    RpLightSetColor(pAmbient, &AmbientLightColour);
    RpLightSetColor(pDirect, &DirectionalLightColour);
}

// 0x735E40
void SetLightsForInfraredVisionHeatObjects() {
    RpLightSetColor(pAmbient, &CPostEffects::m_fInfraredVisionHeatObjectCol);
    RpLightSetColor(pDirect,  &CPostEffects::m_fInfraredVisionHeatObjectCol);
}

// 0x735E70
void StoreAndSetLightsForInfraredVisionHeatObjects() {
    DirectAmbientLight[1] = AmbientLightColour;
    DirectAmbientLight[0] = DirectionalLightColour;
    SetLightsForInfraredVisionHeatObjects();
}

// 0x735EF0
void RestoreLightsForInfraredVisionHeatObjects() {
    RpLightSetColor(pAmbient, &DirectAmbientLight[1]);
    RpLightSetColor(pDirect, &DirectAmbientLight[0]);
}

// 0x735F20
void SetLightsForInfraredVisionDefaultObjects() {
    RwRGBAReal color{ 0.0f, 0.0f, 1.0f, 1.0f };
    RpLightSetColor(pAmbient, &color);
    RpLightSetColor(pDirect,  &color);
}

// 0x735F70
void SetLightsForNightVision() {
    RwRGBAReal color{ 0.0f, 1.0f, 0.0f, 1.0f };
    RpLightSetColor(pAmbient, &color);
    RpLightSetColor(pDirect,  &color);
}

// 0x6FAB30
float GetDayNightBalance() {
    // Como el original: (horas * 60 + minutos) + segundos * 0.016666668f (0x859044), y multiplica en vez de dividir.
    constexpr float ONE_SIXTIETH = 0.016666668f; // 0x3C888889
    const float minutes = (float)(CClock::ms_nGameClockHours * 60 + CClock::ms_nGameClockMinutes) + (float)CClock::ms_nGameClockSeconds * ONE_SIXTIETH;

    if (minutes < 6.0f * 60.0f) // 0-6 o'clock
        return 1.0f;

    if (minutes < 7.0f * 60.0f) // 6-7 o'clock
        return (7.0f * 60.0f - minutes) * ONE_SIXTIETH;

    if (minutes < 20.0f * 60.0f) // 7-20 o'clock
        return 0.0f;

    if (!(minutes < 21.0f * 60.0f)) // 21-0 o'clock
        return 1.0f;

    return 1.0f - (21.0f * 60.0f - minutes) * ONE_SIXTIETH; // 20-21 o'clock
}
