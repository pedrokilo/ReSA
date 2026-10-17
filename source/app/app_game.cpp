/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: Game
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <Tracy.hpp>

#include "app_game.h"
#include "X87Math.h"
#include "LoadingScreen.h"
#include "PlantMgr.h"
#include "Shadows.h"
#include "Clouds.h"
#include "MovingThings.h"
#include "BreakManager_c.h"
#include "RealTimeShadowManager.h"
#include "Credits.h"
#include "Securom.h"
#include "UserDisplay.h"
#include "Garages.h"
#include "Gamma.h"
#include "Birds.h"
#include "Skidmarks.h"
#include "Ropes.h"
#include "Glass.h"
#include "WaterCannons.h"
#include "VehicleRecording.h"
#include "PostEffects.h"
#include "CarFXRenderer.h"

#include "extensions/Configs/FastLoader.hpp"

void AppGameInjectHooks() {
    RH_ScopedCategory("App");
    RH_ScopedNamespaceName("Game");

    RH_ScopedGlobalInstall(GameInit, 0x5BF3B0);
    RH_ScopedGlobalInstall(InitialiseGame, 0x53E580);
    RH_ScopedGlobalInstall(TheGame, 0x53DF10);

    RH_ScopedGlobalInstall(RwInitialize, 0x5BF390);
    RH_ScopedGlobalInstall(RwTerminate, 0x53D910);

    RH_ScopedGlobalInstall(RenderEffects, 0x53E170);
    RH_ScopedGlobalInstall(RenderScene, 0x53DF40);
    RH_ScopedGlobalInstall(RenderMenus, 0x53E530);
    RH_ScopedGlobalInstall(Render2dStuff, 0x53E230);
    RH_ScopedGlobalInstall(RenderDebugShit, 0x53E160);

    // Conmutación probada en reversing/HookUnlockGame; el dispatcher y WinMain protegen el Idle original en segundo plano.
    RH_ScopedGlobalInstall(Idle, 0x53E920);
    RH_ScopedGlobalInstall(FrontendIdle, 0x53E770);
}

// 0x5BF3B0
void GameInit() {
    // Como el original: RwInitialize va en línea y su resultado se ignora (sin VERIFY).
    if (RsRwInitialize(nullptr)) {
        CGame::InitialiseRenderWare();
    }
    srand(RsTimer());
}

// Como el original: la ventana de vista que Idle y FrontendIdle pasan a CameraSize es tan(FOV · π/360)
// calculada con fptan del x87 (precisión completa) sobre el producto en float (fld; fmul [0x8631D4]; fptan; fstp st(0); fstp [esp]).
// WeaponTanCpp (X87Math.cpp) es float(tan(double(x))) con fptan en línea; se reutiliza para no duplicar el ayudante.
static float ExeViewWindow() {
    const float angle = CDraw::ms_fFOV * std::bit_cast<float>(0x3C0EFA35u); // 0x8631D4
    return WeaponTanCpp(angle);
}

// 0x53E580
void InitialiseGame() {
    ZoneScoped;

    static auto& version_number = StaticRef<int16>(0xB72C68);
    version_number = 78;

    CGame::Initialise(GAME_LEVEL_FILE);
    AudioEngine.Restart();
    if (FrontEndMenuManager.m_bLoadingData) {
        FrontEndMenuManager.m_bStartGameLoading = true;
    }
}

// unused
// 0x53DF10
void TheGame() {
    NOTSA_LOG_DEBUG("Into TheGame!!!\n");
    CTimer::Initialise();
    CGame::Initialise(GAME_LEVEL_FILE);
    CLoadingScreen::SetLoadingBarMsg("Starting Game", nullptr);
    CLoadingScreen::NewChunkLoaded();
}

// 0x5BF390
bool RwInitialize(void* param) {
    return RsRwInitialize(param) && CGame::InitialiseRenderWare();
}

// 0x53D910
void RwTerminate() {
    CGame::ShutdownRenderWare();
    ResetGammaWhenExiting();
    RsRwTerminate();
}

// 0x53E170
void RenderEffects() {
    ZoneScoped;

    CBirds::Render();
    CSkidmarks::Render();
    CRopes::Render();
    CGlass::Render();
    CMovingThings::Render();
    CVisibilityPlugins::RenderReallyDrawLastObjects();
    CCoronas::Render();
    g_fx.Render(TheCamera.m_pRwCamera, false);
    CWaterCannons::Render();
    CWaterLevel::RenderWaterFog();
    CClouds::MovingFogRender();
    CClouds::VolumetricCloudsRender();
    if (CHeli::NumberOfSearchLights || CTheScripts::NumberOfScriptSearchLights) {
        CHeli::Pre_SearchLightCone();
        CHeli::RenderAllHeliSearchLights();
        CTheScripts::RenderAllSearchLights();
        CHeli::Post_SearchLightCone();
    }
    CWeaponEffects::Render();
    if (CReplay::Mode != MODE_PLAYBACK && !CPad::GetPad(0)->DisablePlayerControls) {
        FindPlayerPed()->DrawTriangleForMouseRecruitPed();
    }
    CSpecialFX::Render();
    CVehicleRecording::Render();
    CPointLights::RenderFogEffect();
    CRenderer::RenderFirstPersonVehicle();
    CPostEffects::Render();
}

// 0x53DF40
void RenderScene() {
    ZoneScoped;

    // Como el original: el agua se dibuja pronto salvo con UnderWaterness > 0 (con NaN también pronto).
    const auto aboveWater = !(CWeather::UnderWaterness > 0.0f);

    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(FALSE));

    if (CMirrors::TypeOfMirror == MIRROR_TYPE_NONE) {
        CMovingThings::Render_BeforeClouds();
        CClouds::Render();
    }

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESHADEMODE, RWRSTATE(rwSHADEMODEGOURAUD));

    CCarFXRenderer::PreRenderUpdate();
    CRenderer::RenderRoads();
    CCoronas::RenderReflections();
    CRenderer::RenderEverythingBarRoads();
    g_breakMan.Render(false);

    CRenderer::RenderFadingInUnderwaterEntities();

    // If under water render this immediately, otherwise
    // only after rendering the stuff below.
    if (aboveWater) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        CWaterLevel::RenderWater();
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
    }

    CRenderer::RenderFadingInEntities();
    if (!CMirrors::bRenderingReflection) {
        float nearClipPlaneOld = RwCameraGetNearClipPlane(Scene.m_pRwCamera);
        float farPlane = RwCameraGetFarClipPlane(Scene.m_pRwCamera);

        // Como el original: con Z NaN se usa -Z (fcom + test ah,41h).
        float v3;
        float z = CCamera::GetActiveCamera().m_vecFront.z;
        if (!(z > 0.0f))
            v3 = -z;
        else
            v3 = 0.0f;

        constexpr float flt_8CD4F0 = 2.0f;
        constexpr float flt_8CD4EC = 5.9604645e-8f; // Same as 1 / (float)(1 << 24) => 1 / 16777216.f ; Not sure if that means anything..

        float unknown = ((flt_8CD4F0 * flt_8CD4EC * 0.25f - flt_8CD4F0 * flt_8CD4EC) * v3 + flt_8CD4F0 * flt_8CD4EC) * (farPlane - nearClipPlaneOld);

        RwCameraEndUpdate(Scene.m_pRwCamera);

        RwCameraSetNearClipPlane(Scene.m_pRwCamera, unknown + nearClipPlaneOld);
        RwCameraBeginUpdate(Scene.m_pRwCamera);
        CShadows::UpdateStaticShadows();
        CShadows::RenderStaticShadows();
        CShadows::RenderStoredShadows();
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RwCameraSetNearClipPlane(Scene.m_pRwCamera, nearClipPlaneOld);

        RwCameraBeginUpdate(Scene.m_pRwCamera);
    }

    g_breakMan.Render(true);
    CPlantMgr::Render();

    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));

    if (CMirrors::TypeOfMirror == MIRROR_TYPE_NONE) {
        CClouds::RenderBottomFromHeight();
        CWeather::RenderRainStreaks();
        CCoronas::RenderSunReflection();
    }

    if (!aboveWater) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
        CWaterLevel::RenderWater();
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
    }

    CStencilShadows::RenderStencilShadows();
}

// 0x53E530
void RenderMenus() {
    ZoneScoped;

    if (FrontEndMenuManager.m_bMenuActive) {
        FrontEndMenuManager.DrawFrontEnd();
    }
}

// 0x53E230
void Render2dStuff() {
    ZoneScoped;

    // Como el original: el EXE no llama aquí a RenderDebugShit (vacía en el EXE).
    // Como el original: el marco del zoom usa mitades enteras de la resolución (ancho / 2), los factores
    // 1/448 (0x859524) y 1/640 (0x859520) del EXE en su orden de productos y sin corrección de aspecto.
    // Sin jugador el EXE usa un valor sin inicializar de la pila como arma; aquí se mantiene WEAPON_UNARMED.
    const auto DrawOuterZoomBox = []() {
        CPed* player = FindPlayerPed();
        eWeaponType weaponType = WEAPON_UNARMED;
        if (player)
            weaponType = player->GetActiveWeapon().m_Type;
        eCamMode camMode = CCamera::GetActiveCamera().m_nMode;
        bool firstPersonWeapon = false;
        if (camMode == MODE_SNIPER || camMode == MODE_SNIPER_RUNABOUT || camMode == MODE_ROCKETLAUNCHER || camMode == MODE_ROCKETLAUNCHER_RUNABOUT || camMode == MODE_CAMERA ||
            camMode == MODE_HELICANNON_1STPERSON) {
            firstPersonWeapon = true;
        }

        if ((weaponType == WEAPON_SNIPERRIFLE || weaponType == WEAPON_ROCKET) && firstPersonWeapon) {
            const float k448 = std::bit_cast<float>(0x3B124925u); // 0x859524
            const float k640 = std::bit_cast<float>(0x3ACCCCCDu); // 0x859520
            const int32 w = RsGlobal.maximumWidth, h = RsGlobal.maximumHeight;
            const float W = (float)w, H = (float)h;
            const float halfW = (float)(w / 2), halfH = (float)(h / 2);
            const float top    = weaponType == WEAPON_ROCKET ? 180.0f : 210.0f;
            const float bottom = weaponType == WEAPON_ROCKET ? 170.0f : 210.0f;
            const CRGBA black(0, 0, 0, 255);
            CSprite2d::DrawRect(CRect(0.0f, 0.0f, W, halfH - H * k448 * top), black);
            CSprite2d::DrawRect(CRect(0.0f, halfH + k448 * H * bottom, W, H), black);
            CSprite2d::DrawRect(CRect(0.0f, 0.0f, halfW - W * k640 * 210.0f, H), black);
            CSprite2d::DrawRect(CRect(halfW + k640 * W * 210.0f, 0.0f, W, H), black);
        }
    };

    RwRenderStateSet(rwRENDERSTATEZTESTENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE, RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND, RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND, RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE, RWRSTATE(rwRENDERSTATENARENDERSTATE));
    RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));

    CReplay::Display();
    // Como el original: aquí el EXE llama a una función vacía (0x60F8C0).
    CPickups::RenderPickUpText();
    if (TheCamera.m_bWideScreenOn && !FrontEndMenuManager.m_bWidescreenOn)
        TheCamera.DrawBordersForWideScreen();
    DrawOuterZoomBox();
    AudioEngine.DisplayRadioStationName();
    CHud::Draw();
    CSpecialFX::Render2DFXs();
    CUserDisplay::OnscnTimer.ProcessForDisplay();
    CMessages::Display(true);
    CDarkel::DrawMessages();
    CGarages::PrintMessages();
    CFont::DrawFonts();
}

// 0x53E160
void RenderDebugShit() {
    // Como el original: vacía en el EXE (solo `ret`). La versión de depuración NOTSA dibujaba las líneas de
    // depuración de los scripts (CTheScripts::RenderTheScriptDebugLines) y llamaba a DefinedState.
}

// 0x53E920
void Idle(void* param) {
    ZoneScoped;

    // Como el original: limitador de fotogramas por espera activa (como mínimo 14 ms entre fotogramas) con
    // división y resta sin signo; las dos primeras lecturas del temporizador se descartan.
    static auto& LastTickTime = StaticRef<uint32>(0xB72CA8);
    CTimer::GetCurrentTimeInCycles();
    CTimer::GetCyclesPerMillisecond();
    uint32 timeInMs;
    do {
        const uint32 cycles = CTimer::GetCurrentTimeInCycles();
        timeInMs = cycles / CTimer::GetCyclesPerMillisecond();
    } while (timeInMs - LastTickTime < 14u);
    {
        const uint32 cycles = CTimer::GetCurrentTimeInCycles();
        LastTickTime = cycles / CTimer::GetCyclesPerMillisecond();
    }

    CTimer::Update();
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    CPointLights::NumLights = 0;
    CGame::Process();
    AudioEngine.Service();
    SetLightsWithTimeOfDayColour(Scene.m_pRpWorld);
    if (!param) {
        return;
    }

    // Como el original: un solo if/else; GetScreenFadeStatus se llama una vez y solo si el menú no está activo.
    if (!FrontEndMenuManager.m_bMenuActive && TheCamera.GetScreenFadeStatus() != eNameState::NAME_FADE_IN) {
        // Como el original: RsMouseSetPos en línea con (ancho · 0.5, alto · 0.5) en float, sin pasar por
        // CMenuManager::CentreMousePointer (que comprueba primero que el centro no sea 0).
        // NOTSA: no se centra el ratón con la aplicación en segundo plano (el EXE nunca llama a Idle sin
        // ForegroundApp; la DLL sí, porque su bucle principal no se pausa en segundo plano). También con SDL3 se
        // llama, como el EXE (antes se omitía). Se lee el dword de ForegroundApp (0x8D621C) como IsForegroundApp,
        // sin llamar a ninguna función.
        if (StaticRef<uint32>(0x8D621C) != 0) {
            RwV2d pos{ (float)RsGlobal.maximumWidth * 0.5f, (float)RsGlobal.maximumHeight * 0.5f }; // 0x858B8C
            RsMouseSetPos(&pos);
        }

        CRenderer::ConstructRenderList();
        CRenderer::PreRender();
        CWorld::ProcessPedsAfterPreRender();
        g_realTimeShadowMan.Update();
        CMirrors::BeforeMainRender();

        bool started;
        if (CWeather::LightningFlash) {
            CTimeCycle::SetFogRed(255);
            CTimeCycle::SetFogGreen(255);
            CTimeCycle::SetFogBlue(255);
            started = DoRWStuffStartOfFrame_Horizon(255, 255, 255, 255, 255, 255, 255);
        } else {
            started = DoRWStuffStartOfFrame_Horizon(
                CTimeCycle::GetSkyTopRed(), CTimeCycle::GetSkyTopGreen(), CTimeCycle::GetSkyTopBlue(),
                CTimeCycle::GetSkyBottomRed(), CTimeCycle::GetSkyBottomGreen(), CTimeCycle::GetSkyBottomBlue(),
                255
            );
        }
        if (!started)
            return;

        DefinedState();
        RwCameraSetFarClipPlane(Scene.m_pRwCamera, CTimeCycle::GetFarClip());
        Scene.m_pRwCamera->fogPlane = CTimeCycle::GetFogStart();
        CMirrors::RenderMirrorBuffer();
        RenderScene();
        CVisibilityPlugins::RenderWeaponPedsForPC();
        CVisibilityPlugins::ResetWeaponPedsForPC(); // Como el original: vacía ms_weaponPedsForPC (0x53E8D0 con ECX = 0xC88224)
        RenderEffects();
        // Como el original: con m_fScreenReductionPercentage NaN no se toca el alfa (fcomp + test ah,41h).
        if (TheCamera.m_nBlurType == eMotionBlurType::NONE || TheCamera.m_nBlurType == eMotionBlurType::LIGHT_SCENE) {
            if (TheCamera.m_fScreenReductionPercentage > 0.0f) {
                TheCamera.SetMotionBlurAlpha(150);
            }
        }
        TheCamera.RenderMotionBlur();
        Render2dStuff();
    } else {
        CDraw::CalculateAspectRatio();
        CameraSize(Scene.m_pRwCamera, nullptr, ExeViewWindow(), SCREEN_ASPECT_RATIO);
        CVisibilityPlugins::SetRenderWareCamera(Scene.m_pRwCamera);
        RwCameraClear(Scene.m_pRwCamera, &gColourTop, rwCAMERACLEARZ);
        if (!RsCameraBeginUpdate(Scene.m_pRwCamera)) {
            return;
        }
    }

    // Como el original: RenderMenus va en línea.
    if (FrontEndMenuManager.m_bMenuActive) {
        FrontEndMenuManager.DrawFrontEnd();
    }
    RwRenderStateSet(rwRENDERSTATETEXTURERASTER, RWRSTATE(NULL));
    DoFade();
    CHud::DrawAfterFade();
    CMessages::Display(false);
    CFont::DrawFonts();
    // Como el original: la comprobación y el dibujo de los créditos (CCredits::Render en línea) aparecen dos
    // veces seguidas en el EXE.
    if (CCredits::bCreditsGoing && !FrontEndMenuManager.m_bMenuActive) {
        CCredits::RenderCredits();
    }
    if (CCredits::bCreditsGoing && !FrontEndMenuManager.m_bMenuActive) {
        CCredits::RenderCredits();
    }
    CDebug::DebugDisplayTextBuffer();
    FlushObrsPrintfs();
    RwCameraEndUpdate(Scene.m_pRwCamera);
    RsCameraShowRaster(Scene.m_pRwCamera);
}

// 0x53E770
void FrontendIdle() {
    ZoneScoped;

    CDraw::CalculateAspectRatio();
    CTimer::Update();
    CSprite2d::SetRecipNearClip();
    CSprite2d::InitPerFrame();
    CFont::InitPerFrame();
    CPad::UpdatePads();

    // Como el original: no se borra m_bActivateMenuNextFrame.
    if (FrontEndMenuManager.m_bActivateMenuNextFrame) {
        FrontEndMenuManager.m_bMenuActive = true;
    }
    FrontEndMenuManager.Process();

    if (RsGlobal.quit) {
        return;
    }

    AudioEngine.Service();
    CameraSize(Scene.m_pRwCamera, nullptr, ExeViewWindow(), SCREEN_ASPECT_RATIO);
    CVisibilityPlugins::SetRenderWareCamera(Scene.m_pRwCamera);

    // Como el original: siempre se limpia y se dibuja (se quitó la salida NOTSA de FastLoader, que no dibujaba
    // el fotograma mientras quedaba una partida por cargar).
    RwCameraClear(Scene.m_pRwCamera, &gColourTop, rwCAMERACLEARZ);
    if (RsCameraBeginUpdate(Scene.m_pRwCamera)) {
        DefinedState();

        if (FrontEndMenuManager.m_bMenuActive) {
            FrontEndMenuManager.DrawFrontEnd();
        }

        SecuromStateDisplay();
        DoFade();
        CHud::DrawAfterFade();
        CMessages::Display(false);
        CFont::DrawFonts();

        if (CCredits::bCreditsGoing && !FrontEndMenuManager.m_bMenuActive) {
            CCredits::RenderCredits();
        }

        CFont::DrawFonts();
        CDebug::DebugDisplayTextBuffer();
        FlushObrsPrintfs();
        RwCameraEndUpdate(Scene.m_pRwCamera);
        RsCameraShowRaster(Scene.m_pRwCamera);
    }
}
