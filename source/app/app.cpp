/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: App
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "app.h"
#include "app_input.h"
#include "app_game.h"
#include "platform.h"
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>

#include "Clouds.h"
#include "CarFXRenderer.h"

#include "Plugins/BreakablePlugin/BreakablePlugin.h"
#include "Pipelines/CustomBuilding/CustomBuildingRenderer.h"

#include "platform/win/WinPlatform.h"

#include "Enums/eGameState.h"
#include "Base.h"

void AppInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("App");

    RH_ScopedGlobalInstall(DoRWStuffStartOfFrame, 0x53D690);
    RH_ScopedGlobalInstall(DoRWStuffStartOfFrame_Horizon, 0x53D7A0);
    RH_ScopedGlobalInstall(DoRWStuffEndOfFrame, 0x53D840);

    RH_ScopedGlobalInstall(PluginAttach, 0x53D870);
    RH_ScopedGlobalInstall(AppEventHandler, 0x53EC10);

    RH_ScopedGlobalInstall(DefinedState, 0x734650);
    RH_ScopedGlobalInstall(DefinedState2d, 0x734750);

    AppInputInjectHooks();
    AppGameInjectHooks();
    AppLightInjectHooks();
}

void ChangeGameStateTo(eGameState newgs) {
    if (gGameState != newgs) {
        //NOTSA_LOG_DEBUG("GS Change: `{}` to `{}`", (eGameState)(gGameState), (eGameState)(newgs)); // doesn't compile???
        gGameState = newgs;
    }
};

// 0x53D690
bool DoRWStuffStartOfFrame(int16 TopRed, int16 TopGreen, int16 TopBlue, int16 BottomRed, int16 BottomGreen, int16 BottomBlue, int16 Alpha) {
    RwRGBA TopColor{ (uint8)TopRed, (uint8)TopGreen, (uint8)TopBlue, (uint8)Alpha };
    RwRGBA BottomColor{ (uint8)BottomRed, (uint8)BottomGreen, (uint8)BottomBlue, (uint8)Alpha };

    CDraw::CalculateAspectRatio();
    CFont::SetAlphaFade(255.f); // 0x53D6A0
    CameraSize(Scene.m_pRwCamera, nullptr, SCREEN_VIEW_WINDOW, SCREEN_ASPECT_RATIO);
    CVisibilityPlugins::SetRenderWareCamera(Scene.m_pRwCamera);
    RwCameraClear(Scene.m_pRwCamera, &TopColor, rwCAMERACLEARZ); // 0x53D71C: Z only (the horizon variant also clears stencil)

    if (!RsCameraBeginUpdate(Scene.m_pRwCamera))
        return false;

    CSprite2d::InitPerFrame();

    if (Alpha != 0)
        CSprite2d::DrawRect(CRect(0.0f, 0.0f, SCREEN_WIDTH, SCREEN_HEIGHT), BottomColor, BottomColor, TopColor, TopColor);

    return true;
}

// 0x53D7A0
bool DoRWStuffStartOfFrame_Horizon(int16 TopRed, int16 TopGreen, int16 TopBlue, int16 BottomRed, int16 BottomGreen, int16 BottomBlue, int16 Alpha) {
    ZoneScoped;

    CDraw::CalculateAspectRatio();
    CameraSize(Scene.m_pRwCamera, nullptr, SCREEN_VIEW_WINDOW, SCREEN_ASPECT_RATIO);
    CVisibilityPlugins::SetRenderWareCamera(Scene.m_pRwCamera);
    RwCameraClear(Scene.m_pRwCamera, &gColourTop, rwCAMERACLEARZ | rwCAMERACLEARSTENCIL);

    if (!RsCameraBeginUpdate(Scene.m_pRwCamera))
        return false;

    DefinedState();
    TheCamera.m_mViewMatrix.Update();
    CClouds::RenderSkyPolys();

    return true;
}

// 0x53D840
void DoRWStuffEndOfFrame() {
    CDebug::DebugDisplayTextBuffer();
    FlushObrsPrintfs();
    RwCameraEndUpdate(Scene.m_pRwCamera);
    RsCameraShowRaster(Scene.m_pRwCamera);
}

// Política del bucle NOTSA cuando se selecciona el Idle original.
bool ShouldPauseOriginalIdleInBackground() {
    if (StaticRef<uint32>(0x8D621C) != 0) {
        return false;
    }
    static auto* idleHook = [] {
        ReversibleHooks::ReversibleHook::Base* result{};
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category) {
            if (category.Name() == "Game") {
                for (auto& hook : category.Items()) {
                    if (hook->Name() == "Idle") {
                        result = hook.get();
                    }
                }
            }
        });
        return result;
    }();
    return idleHook && !idleHook->Hooked();
}

// 0x53EC10
RsEventStatus AppEventHandler(RsEvent event, void* param) {
    switch (event) {
    case rsCAMERASIZE:
        // FIX_BUGS 4:3 -> SCREEN_ASPECT_RATIO.
        CameraSize(Scene.m_pRwCamera, (RwRect*)param, SCREEN_VIEW_WINDOW, SCREEN_ASPECT_RATIO);
        break;
    case rsINPUTDEVICEATTACH:
        AttachInputDevices();
        break;
    case rsPLUGINATTACH:
        return RSEVENT_SUCCEED(PluginAttach());
    case rsRWINITIALIZE:
        return RSEVENT_SUCCEED(RwInitialize(param));
    case rsRWTERMINATE:
        RwTerminate();
        break;
    case rsINITIALIZE:
        CGame::InitialiseOnceBeforeRW();
        return RSEVENT_SUCCEED(RsInitialize());
    case rsTERMINATE:
        CGame::FinalShutdown();
        break;
    case rsIDLE:
        // El Idle del EXE centra el cursor sin comprobar ForegroundApp. El bucle NOTSA continúa en segundo
        // plano: si se conmuta a ese cuerpo, conservar aquí la pausa del EXE y no mover el cursor de otra app.
        if (ShouldPauseOriginalIdleInBackground()) {
            break;
        }
        Idle(param);
        break;
    case rsFRONTENDIDLE:
        FrontendIdle();
        break;
    case rsACTIVATE:
        break;
    default:
        return rsEVENTNOTPROCESSED;
    }

    return rsEVENTPROCESSED;
}

// 0x53D870
bool PluginAttach() {
    const auto Attach = [](auto name, auto attachFn) {
        if (!attachFn()) {
            NOTSA_LOG_DEBUG("Couldn't attach {} plugin", name);
            return false;
        }
        return true;
    };

    return (
        Attach("Rp World",                 RpWorldPluginAttach) &&
        Attach("Rp Skin",                  RpSkinPluginAttach) &&
        Attach("Rt AnimInitialize",        RtAnimInitialize) &&
        Attach("Rp HAnim",                 RpHAnimPluginAttach) &&
        Attach("Node Name",                NodeNamePluginAttach) &&
        Attach("Visibility",               CVisibilityPlugins::PluginAttach) &&
        Attach("Rp Anim Blend",            RpAnimBlendPluginAttach) &&
        Attach("C Txd Store",              CTxdStore::PluginAttach) &&
        Attach("Rp Mat FX",                RpMatFXPluginAttach) &&
        Attach("Rp UV Anim",               RpUVAnimPluginAttach) &&
        Attach("Custom Building Renderer", CCustomBuildingRenderer::PluginAttach) &&
        Attach("Car FX Renderer",          CCarFXRenderer::RegisterPlugins) &&
        (RpAnisotPluginAttach(), true) && // 0x53D8DF: result ignored by the original
        Attach("Breakable",                BreakablePluginAttach) &&
        Attach("Collision",                CCollisionPlugin::PluginAttach) &&
        Attach("2d Effect",                C2dEffect::PluginAttach) &&
        Attach("Pipeline",                 PipelinePluginAttach)
    );
}

// 0x734650
void DefinedState() {
    // 0x7346E1: ARGB built from the raw 16-bit sky colours with alpha 0xFF (no clamping to 8 bits)
    uint32 fogColor = 0xFFFFFF00u | (uint32)(uint16)CTimeCycle::m_CurrentColours.m_nSkyBottomRed;
    fogColor = (fogColor << 8) | (uint32)(uint16)CTimeCycle::m_CurrentColours.m_nSkyBottomGreen;
    fogColor = (fogColor << 8) | (uint32)(uint16)CTimeCycle::m_CurrentColours.m_nSkyBottomBlue;

    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, RWRSTATE((RWRGBALONG(0, 0, 0, 255))));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEFOGCOLOR, RWRSTATE(fogColor));
    RwRenderStateSet(rwRENDERSTATEFOGTYPE, RWRSTATE(rwFOGTYPELINEAR));
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(2)); // TODO: ?
}

// 0x734750
void DefinedState2d() {
    RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATETEXTUREPERSPECTIVE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(rwSHADEMODEGOURAUD));
    RwRenderStateSet(rwRENDERSTATETEXTUREFILTER, RWRSTATE(rwFILTERLINEAR));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEBORDERCOLOR, RWRSTATE((RWRGBALONG(0, 0, 0, 255))));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION, RWRSTATE(rwALPHATESTFUNCTIONGREATER));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(2)); // TODO: ?
}
