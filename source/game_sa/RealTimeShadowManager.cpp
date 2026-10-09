/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRealTimeShadowManager
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "RealTimeShadowManager.h"
#include "Shadows.h"

auto& g_realTimeShadowMan = StaticRef<CRealTimeShadowManager>(0xC40350);

void CRealTimeShadowManager::InjectHooks() {
    RH_ScopedClass(CRealTimeShadowManager);
    RH_ScopedCategory("Shadows");

    RH_ScopedInstall(Init, 0x7067C0);
    RH_ScopedInstall(ReInit, 0x706870);
    RH_ScopedInstall(ReturnRealTimeShadow, 0x705B30);
    RH_ScopedInstall(GetRealTimeShadow, 0x706970);
    RH_ScopedInstall(Update, 0x706AB0);
    RH_ScopedInstall(DoShadowThisFrame, 0x706BA0);
    RH_ScopedInstall(Exit, 0x706A60);
}

// 0x7067C0
void CRealTimeShadowManager::Init() {
    if (m_bInitialised) {
        return;
    }

    for (auto& shdw : m_apShadows) {
        shdw = new CRealTimeShadow();
        shdw->Create(true, 4, true);
    }

    m_BlurCamera.Create(6);

    m_GradientCamera.Create(6);
    m_GradientCamera.MakeGradientRaster();

    m_bInitialised = true;
}

// 0x706A60
void CRealTimeShadowManager::Exit() { // AKA `Shutdown`
    if (!m_bInitialised) {
        return;
    }

    // Como el original (0x706A74-0x706A90): cada sombra NO nula se destruye (0x706440) y se libera (0x8214BD) y la casilla
    // queda a nullptr; no se mira el propietario ni se libera nada mas (antes se borraba tambien el owner, que el EXE no hace).
    for (auto& shdw : m_apShadows) {
        if (shdw) {
            delete shdw;
        }
        shdw = nullptr;
    }

    // Nice hack
    m_BlurCamera.Destroy();
    m_GradientCamera.Destroy();

    m_bInitialised = false;
}

// 0x705B30
void CRealTimeShadowManager::ReturnRealTimeShadow(CRealTimeShadow* shdw) {
    if (m_bInitialised) {
        shdw->m_pOwner->m_pShadowData = nullptr;
        shdw->m_pOwner = nullptr;
    }
}

// 0x706870
void CRealTimeShadowManager::ReInit() {
    const auto recreateRaster = [](CShadowCamera& camera) {
        auto* const raster = RwCameraGetRaster(camera.m_pRwCamera);
        const auto size = RwRasterGetWidth(raster);
        RwCameraSetRaster(camera.m_pRwCamera, nullptr);
        RwRasterDestroy(raster);
        auto* const replacement = RwRasterCreate(size, size, 0, 5);
        RwCameraSetRaster(camera.m_pRwCamera, replacement);
        RwTextureSetRaster(camera.m_pRwRenderTexture, replacement);
    };
    for (auto* shadow : m_apShadows) {
        recreateRaster(shadow->m_camera);
        recreateRaster(shadow->m_blurCamera);
    }
    recreateRaster(m_BlurCamera);
    recreateRaster(m_GradientCamera);
    m_GradientCamera.MakeGradientRaster();
}

// 0x706AB0
void CRealTimeShadowManager::Update() {
    ZoneScoped;

    if (m_bInitialised && m_bNeedsReinit) {
        ReInit();
        m_bNeedsReinit = false;
    }

    for (const auto shdw : m_apShadows) {
        if (!shdw->m_pOwner) {
            continue;
        }

        // Como el original (0x706AED-0x706B0F): la intensidad se lee con movsx (byte CON SIGNO); al subir se suma 3 y se
        // acota por arriba a 100 (jl, con signo); al bajar se resta 3 y se acota por abajo a 0 (setle, con signo).
        // El EXE no comprueba que owner->m_pShadowData == shdw.
        constexpr int32 INTENSITY_STEP = 3;
        const int32 intensity = static_cast<int8>(shdw->m_nIntensity);
        if (shdw->m_bKeepAlive) {
            const int32 up = intensity + INTENSITY_STEP;
            shdw->m_nIntensity = static_cast<uint8>(up < 100 ? up : 100);
        } else { // Fade out
            const int32 down = intensity - INTENSITY_STEP;
            shdw->m_nIntensity = static_cast<uint8>(down <= 0 ? 0 : down);
        }

        if (shdw->m_nIntensity) {
            shdw->Update();
            CShadows::StoreRealTimeShadow(
                shdw->m_pOwner,

                CTimeCycle::m_fShadowDisplacementX[CTimeCycle::m_CurrentStoredValue],
                CTimeCycle::m_fShadowDisplacementY[CTimeCycle::m_CurrentStoredValue],

                CTimeCycle::m_fShadowFrontX[CTimeCycle::m_CurrentStoredValue],
                CTimeCycle::m_fShadowFrontY[CTimeCycle::m_CurrentStoredValue],

                CTimeCycle::m_fShadowSideX[CTimeCycle::m_CurrentStoredValue],
                CTimeCycle::m_fShadowSideY[CTimeCycle::m_CurrentStoredValue]
            );
        } else if (m_bInitialised) {
            shdw->m_pOwner->m_pShadowData = nullptr;
            shdw->m_pOwner = nullptr;
        }
    }

    // TODO: ??? - Perhaps debug code left accidentally in?
    for (const auto shdw : m_apShadows) {
        shdw->m_bKeepAlive = false;
    }
}

// 0x706970
CRealTimeShadow* CRealTimeShadowManager::GetRealTimeShadow(CPhysical* physical) {
    const bool firstPlayer = physical->GetIsTypePed() && physical->AsPed()->m_nPedType == PED_TYPE_PLAYER1;
    if (!firstPlayer) {
        const auto* player = FindPlayerPed();
        if (player->bInVehicle && player->m_pVehicle && player->m_pVehicle->GetMoveSpeed().SquaredMagnitude() > 0.09f) {
            return nullptr;
        }
    }
    if (!m_bInitialised) {
        return nullptr;
    }
    CRealTimeShadow* selected = firstPlayer ? m_apShadows[0] : nullptr;
    if (!firstPlayer) {
        for (size_t i = 1; i < NUM_REALTIME_SHADOWS; ++i) {
            if (!m_apShadows[i]->m_pOwner) {
                selected = m_apShadows[i];
            }
        }
    }
    if (selected) {
        selected->SetupForThisEntity(physical);
        physical->m_pShadowData = selected;
        selected->m_bKeepAlive = true;
        selected->m_nIntensity = 0;
    }
    return selected;
}

// 0x706BA0
void CRealTimeShadowManager::DoShadowThisFrame(CPhysical* physical) {
    // Como el original (0x706BA6-0x706BE1): se calcula antes "es el PED_TYPE_PLAYER1" (tipo de entidad & 7 == 3 y
    // m_nPedType == 0) y GetFxQuality se llama UNA vez, y una segunda solo si no era VERY_HIGH.
    const bool isPlayer1 = physical->GetIsTypePed() && physical->AsPed()->m_nPedType == PED_TYPE_PLAYER1;
    if (g_fx.GetFxQuality() != FX_QUALITY_VERY_HIGH) {
        if (g_fx.GetFxQuality() != FX_QUALITY_HIGH || !isPlayer1) {
            return;
        }
    }

    if (const auto shdw = physical->m_pShadowData) {
        shdw->m_bKeepAlive = true;
    } else {
        (void)GetRealTimeShadow(physical); // ???
    }
}
