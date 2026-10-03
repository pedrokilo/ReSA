/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTrafficLights
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

#include "TrafficLights.h"
#include "Shadows.h"

void CTrafficLights::InjectHooks()
{
    RH_ScopedClass(CTrafficLights);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ShouldCarStopForLight, 0x49D610);
    RH_ScopedInstall(ShouldCarStopForBridge, 0x49D420);
    RH_ScopedInstall(IsMITrafficLight, 0x49D5A0);
    RH_ScopedInstall(DisplayActualLight, 0x49DAB0);
    RH_ScopedInstall(FindTrafficLightType, 0x49D580);
    RH_ScopedInstall(FindTrafficLightTypeFromOrientation, 0x49D4D0);
    RH_ScopedInstall(FindOrientationForTrafficLightType, 0x49D520);
    RH_ScopedInstall(LightForPeds, 0x49D400);
    RH_ScopedInstall(LightForCars1, 0x49D2D0);
    RH_ScopedInstall(LightForCars2, 0x49D310);
    RH_ScopedInstall(LightForCars1_Visual, 0x49D350);
    RH_ScopedInstall(LightForCars2_Visual, 0x49D3A0);
}

// Como el original: producto 2D (pos - nodo) · dir con el orden del EXE (término y primero) y las constantes
// 0.125 y 0.01 del EXE (la conversión de FixedFloat divide entre 100 y redondea distinto)
static float NaviNodeDot(CVehicle* vehicle, const CCarPathLink& link) {
    const auto* raw = reinterpret_cast<const uint8*>(&link);
    const auto& pos = vehicle->GetPosition();
    const float termY = (pos.y - (float)*reinterpret_cast<const int16*>(raw + 2) * 0.125f) * ((float)*reinterpret_cast<const int8*>(raw + 9) * 0.01f);
    const float termX = (pos.x - (float)*reinterpret_cast<const int16*>(raw + 0) * 0.125f) * ((float)*reinterpret_cast<const int8*>(raw + 8) * 0.01f);
    return termY + termX;
}

// Como el original: un bloque de ShouldCarStopForLight (inline 3 veces en el EXE).
// Devuelve 1 = parar, 0 = devolver false, -1 = seguir con el siguiente nodo.
static int32 ShouldCarStopForLightLink(CVehicle* vehicle, CCarPathLinkAddress& info, const CNodeAddress& address, bool bUnkn, int8 side, float range, bool requirePhysics) {
    if (!info.IsValid() || !ThePaths.m_pPathNodes[info.m_wAreaId]) {
        return -1;
    }
    const auto& link = ThePaths.GetCarPathLink(info);
    if (requirePhysics && vehicle->GetStatus() != STATUS_PHYSICS) {
        return 0;
    }
    const auto state = link.m_nTrafficLightState;
    if (!state) {
        return -1;
    }
    if (link.m_bTrafficLightDirection ? !(link.m_attachedTo == address) : !(link.m_attachedTo != address)) {
        return -1;
    }
    if (!bUnkn
        && !(state == DIR_NORTH_SOUTH && CTrafficLights::LightForCars1())
        && !(link.m_nTrafficLightState == DIR_WEST_EAST && CTrafficLights::LightForCars2())
    ) {
        return 0;
    }
    const float dot = NaviNodeDot(vehicle, link);
    if (side == -1 ? dot > 0.0F && dot < range : dot < 0.0F && dot > -range) {
        return 1;
    }
    return -1;
}

// 0x49D610
bool CTrafficLights::ShouldCarStopForLight(CVehicle* vehicle, bool bUnkn) {
    // Como el original: GetVehicleAppearance se llama una vez por cada comparación
    if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI || vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE)
        return false;

    auto& ap = vehicle->m_autoPilot;
    if (const auto r = ShouldCarStopForLightLink(vehicle, ap.m_nCurrentPathNodeInfo, ap.m_currentAddress, bUnkn, ap._smthCurr, 12.0F, false); r >= 0) {
        return r != 0;
    }
    if (const auto r = ShouldCarStopForLightLink(vehicle, ap.m_nNextPathNodeInfo, ap.m_startingRouteNode, bUnkn, ap._smthNext, 12.0F, false); r >= 0) {
        return r != 0;
    }
    return ShouldCarStopForLightLink(vehicle, ap.m_nPreviousPathNodeInfo, ap.m_endingRouteNode, bUnkn, ap._smthPrev, 6.0F, true) == 1;
}

// 0x49D420
bool CTrafficLights::ShouldCarStopForBridge(CVehicle* vehicle) {
    // Como el original: GetVehicleAppearance se llama una vez por cada comparación
    if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI || vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE)
        return false;

    auto& nextNodeInfo = vehicle->m_autoPilot.m_nNextPathNodeInfo;
    if (!nextNodeInfo.IsValid() || !ThePaths.m_pPathNodes[nextNodeInfo.m_wAreaId])
        return false;

    // Como el original: el área del nodo actual también tiene que estar cargada
    auto& curNodeInfo = vehicle->m_autoPilot.m_nCurrentPathNodeInfo;
    if (!curNodeInfo.IsValid() || !ThePaths.m_pPathNodes[curNodeInfo.m_wAreaId])
        return false;

    // Como el original: el siguiente enlace tiene luces de puente y el actual no
    return ThePaths.GetCarPathLink(nextNodeInfo).m_bridgeLights && !ThePaths.GetCarPathLink(curNodeInfo).m_bridgeLights;
}

// 0x49D580
int32 CTrafficLights::FindTrafficLightType(CEntity* entity) {
    auto heading = FindOrientationForTrafficLightType(entity);
    return FindTrafficLightTypeFromOrientation(heading);
}

// 0x49D520
float CTrafficLights::FindOrientationForTrafficLightType(CEntity* entity) {
    const auto& mat = entity->GetMatrix();
    // Como el original: multiplica por la constante del EXE (0x859878), un ulp por debajo de 180/pi
    return CGeneral::GetATanOfXY(mat.GetForward().x, mat.GetForward().y) * std::bit_cast<float>(0x42652EE0u);
}

// 0x49D4D0
int32 CTrafficLights::FindTrafficLightTypeFromOrientation(float fOrientation) {
    // Como el original: comparaciones estrictas, NaN da DIR_WEST_EAST
    if (fOrientation > 60.0F && fOrientation < 150.0F) {
        return eTrafficLightsDirection::DIR_NORTH_SOUTH;
    }
    if (fOrientation > 240.0F && fOrientation < 330.0F) {
        return eTrafficLightsDirection::DIR_NORTH_SOUTH;
    }
    return eTrafficLightsDirection::DIR_WEST_EAST;
}

// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE
static CVector MatrixTimesPoint59C890(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z
    };
}

// 0x49DAB0
void CTrafficLights::DisplayActualLight(CEntity* entity) {
    const auto& entMat = entity->GetMatrix();
    if (entMat.GetUp().z < 0.96F || entity->m_bRenderDamaged)
        return;

    auto iLightColorBase = eBrightLightColor::BRIGHTLIGHT_GREEN_BIG;
    if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_3
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_4
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_5
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY
        || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_MIAMI
    ) {
        iLightColorBase = eBrightLightColor::BRIGHTLIGHT_GREEN_SMALL;
    }

    int32 iLightState;
    if (FindTrafficLightType(entity) == eTrafficLightsDirection::DIR_NORTH_SOUTH)
        iLightState = LightForCars1_Visual();
    else
        iLightState = LightForCars2_Visual();

    auto mi = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    // Como el original: orden de sumas del EXE (y, z, luego x)
    const auto& camFwd = TheCamera.m_mCameraMatrix.GetForward();
    const auto& entFwd = entity->GetMatrix().GetForward();
    bool bSameDir = camFwd.y * entFwd.y + camFwd.z * entFwd.z + camFwd.x * entFwd.x > 0.0F;

    CVector vecCenter(0.0F, 0.0F, 0.0F);
    for (int32 iFxInd = 0; iFxInd < mi->m_n2dfxCount; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        if (effect->m_Type != e2dEffectType::EFFECT_LIGHT)
            continue;

        auto vecLightPos = MatrixTimesPoint59C890(entity->GetMatrix(), effect->m_Pos);
        vecCenter += vecLightPos;
        int32 iColorState = eTrafficLightsState::LIGHT_GREEN;
        if (effect->light.m_color.red > 200) {
            if (effect->light.m_color.green > 100)
                iColorState = eTrafficLightsState::LIGHT_YELLOW;
            else
                iColorState = eTrafficLightsState::LIGHT_RED;
        }

        if (bSameDir == effect->m_Pos.y > 0.0F || iColorState != iLightState)
            continue;

        auto fBrightness = CTimeCycle::m_CurrentColours.m_fSpriteBrightness * 0.07F;
        auto fSize = CTimeCycle::m_CurrentColours.m_fSpriteSize * 0.175F;

        CCoronas::RegisterCorona(
            reinterpret_cast<uint32>(entity) + iFxInd,
            nullptr,
            static_cast<uint8>(static_cast<int64>((float)aTrafficLightColoursR[iColorState] * fBrightness)), // Como el original: _ftol2
            static_cast<uint8>(static_cast<int64>((float)aTrafficLightColoursG[iColorState] * fBrightness)),
            static_cast<uint8>(static_cast<int64>((float)aTrafficLightColoursB[iColorState] * fBrightness)),
            255,
            vecLightPos,
            fSize,
            50.0F,
            eCoronaType::CORONATYPE_SHINYSTAR,
            eCoronaFlareType::FLARETYPE_NONE,
            eCoronaReflType::CORREFL_SIMPLE,
            eCoronaLOSCheck::LOSCHECK_OFF,
            eCoronaTrail::TRAIL_OFF,
            0.0F,
            false,
            1.5F,
            0,
            15.0F,
            false,
            false
        );

        CBrightLights::RegisterOne(
            vecLightPos,
            entity->GetMatrix().GetUp(),
            entity->GetMatrix().GetRight(),
            CVector(0.0F, 0.0F, 0.0F),
            (eBrightLightColor)(iLightState + iLightColorBase),
            0,
            0,
            0
        );
    }

    if (mi->m_n2dfxCount) {
        // Como el original: multiplica por el inverso
        const float invCount = 1.0F / (float)mi->m_n2dfxCount;
        vecCenter.x *= invCount;
        vecCenter.y *= invCount;
        vecCenter.z *= invCount;
        if (iLightState < eTrafficLightsState::LIGHT_OFF) {
            if (CWeather::TrafficLightsBrightness > 0.5F) {
                auto ucRed =   (float)std::max(aTrafficLightColoursR[iLightState], uint8(50));
                auto ucGreen = (float)std::max(aTrafficLightColoursG[iLightState], uint8(50));
                auto ucBlue =  (float)std::max(aTrafficLightColoursB[iLightState], uint8(50));

                CPointLights::AddLight(
                    ePointLightType::PLTYPE_POINTLIGHT,
                    vecCenter,
                    CVector(0.0F, 0.0F, 0.0F),
                    14.0F,
                    fLightMult * ucRed,
                    fLightMult * ucGreen,
                    fLightMult * ucBlue,
                    RwFogType::rwFOGTYPELINEAR,
                    true,
                    nullptr
                );
            }

            if (CWeather::TrafficLightsBrightness > 0.05F) {
                // Como el original: ((color * brillo en el suelo) * brillo de semáforos) * 0.0125, truncado con _ftol2
                const auto colMult = [](uint8 c) {
                    return static_cast<uint8>(static_cast<int64>((float)c * CTimeCycle::m_CurrentColours.m_fLightsOnGroundBrightness * CWeather::TrafficLightsBrightness * 0.0125F));
                };
                auto ucBlue  = colMult(aTrafficLightColoursB[iLightState]);
                auto ucGreen = colMult(aTrafficLightColoursG[iLightState]);
                auto ucRed   = colMult(aTrafficLightColoursR[iLightState]);

                CShadows::StoreStaticShadow(
                    reinterpret_cast<uint32>(entity),
                    eShadowType::SHADOW_ADDITIVE,
                    gpShadowExplosionTex,
                    vecCenter,
                    8.0F,
                    0.0F,
                    0.0F,
                    -8.0F,
                    128,
                    ucRed,
                    ucGreen,
                    ucBlue,
                    12.0F,
                    1.0F,
                    40.0F,
                    false,
                    0.0F
                );
            }
        }
    }

    CVector vecCorner1(0.0F, 0.0F, 0.0F);
    CVector vecCorner2(0.0F, 0.0F, 0.0F);
    if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_4
     || entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY) {

        vecCorner1.Set(0.189F, -0.157F, 1.848F);
        vecCorner2.Set(-0.141F, -0.157F, 1.607F);
    }
    else if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS_5) {
        if (!(uiPedLightFlags & 1)) {
            uiPedLightFlags |= 1;
            vecTrafficLights5_1.Set(-0.044F, -0.261F, 1.561F);
        }

        if (!(uiPedLightFlags & 2)) {
            uiPedLightFlags |= 2;
            vecTrafficLights5_2.Set(0.222F, -0.261F, 1.867F);
        }
        vecCorner1 = vecTrafficLights5_2;
        vecCorner2 = vecTrafficLights5_1;
    }
    else if (entity->m_nModelIndex == ModelIndices::MI_TRAFFICLIGHTS) {
        if (!(uiPedLightFlags & 4)) {
            uiPedLightFlags |= 4;
            vecTrafficLights_1.Set(2.673F, 1.262F, -0.539F);
        }
        if (!(uiPedLightFlags & 8)) {
            uiPedLightFlags |= 8;
            vecTrafficLights_2.Set(2.673F, 0.713F, -0.127F);
        }
        vecCorner1 = vecTrafficLights_2;
        vecCorner2 = vecTrafficLights_1;
    }
    else {
        return;
    }

    auto iPedLightState = LightForPeds();
    if (iPedLightState == eTrafficLightsState::LIGHT_RED) {
        // Ped lights are composed of 2 images side by side, we want to draw only the required half
        auto vecMidPoint = (vecCorner1 + vecCorner2) * 0.5F;

        auto p1 = CVector(vecCorner2.x, vecCorner2.y, vecCorner1.z);
        auto p2 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner1.z);
        auto p3 = vecCorner2;
        auto p4 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner2.z);

        CShinyTexts::RegisterOne(
            MatrixTimesPoint59C890(entity->GetMatrix(), p1),
            MatrixTimesPoint59C890(entity->GetMatrix(), p2),
            MatrixTimesPoint59C890(entity->GetMatrix(), p3),
            MatrixTimesPoint59C890(entity->GetMatrix(), p4),
            0.5F,
            0.0F,
            0.0F,
            0.0F,
            0.5F,
            1.0F,
            0.0F,
            1.0F,
            1,
            255,
            0,
            0,
            60.0F
        );

    }
    else if (iPedLightState == eTrafficLightsState::LIGHT_GREEN || CTimer::GetTimeInMS() & 0x100) {
        // Ped lights are composed of 2 images side by side, we want to draw only the required half
        auto vecMidPoint = (vecCorner1 + vecCorner2) * 0.5F;

        auto p1 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner1.z);
        auto p2 = vecCorner1;
        auto p3 = CVector(vecMidPoint.x, vecMidPoint.y, vecCorner2.z);
        auto p4 = CVector(vecCorner1.x, vecCorner1.y, vecCorner2.z);

        CShinyTexts::RegisterOne(
            MatrixTimesPoint59C890(entity->GetMatrix(), p1),
            MatrixTimesPoint59C890(entity->GetMatrix(), p2),
            MatrixTimesPoint59C890(entity->GetMatrix(), p3),
            MatrixTimesPoint59C890(entity->GetMatrix(), p4),
            1.0F,
            0.0F,
            0.5F,
            0.0F,
            1.0F,
            1.0F,
            0.5F,
            1.0F,
            true,
            255,
            255,
            255,
            60.0F
        );
    }
}

// 0x49D5A0
bool CTrafficLights::IsMITrafficLight(int32 modelIndex) {
    return modelIndex == ModelIndices::MI_TRAFFICLIGHTS
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VERTICAL
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_MIAMI
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VEGAS
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_TWOVERTICAL
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_3
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_4
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_5
        || modelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY;
}

// 0x49D400
uint8 CTrafficLights::LightForPeds() {
    auto uiMaskedTime = (CTimer::GetTimeInMS() / 2) & 16383;
    if (uiMaskedTime < 12000)
        return eTrafficLightsState::LIGHT_RED;

    // Como el original: verde hasta 15384 (0x3C18)
    if (uiMaskedTime < 15384)
        return eTrafficLightsState::LIGHT_GREEN;

    return eTrafficLightsState::LIGHT_YELLOW;
}

// 0x49D2D0
uint8 CTrafficLights::LightForCars1() {
    if (CGameLogic::LaRiotsActiveHere() || CCheat::IsActive(CHEAT_BGREEN_LIGHTS_CHEAT))
        return eTrafficLightsState::LIGHT_GREEN;

    auto uiMaskedTime = (CTimer::GetTimeInMS() / 2) & 16383;
    if (uiMaskedTime < 5000)
        return eTrafficLightsState::LIGHT_GREEN;

    if (uiMaskedTime < 6000)
        return eTrafficLightsState::LIGHT_YELLOW;

    return eTrafficLightsState::LIGHT_RED;
}

// 0x49D310
uint8 CTrafficLights::LightForCars2() {
    if (CGameLogic::LaRiotsActiveHere() || CCheat::IsActive(CHEAT_BGREEN_LIGHTS_CHEAT))
        return eTrafficLightsState::LIGHT_GREEN;

    auto uiMaskedTime = (CTimer::GetTimeInMS() / 2) & 16383;
    if (uiMaskedTime < 6000)
        return eTrafficLightsState::LIGHT_RED;

    if (uiMaskedTime < 11000)
        return eTrafficLightsState::LIGHT_GREEN;

    if (uiMaskedTime < 12000)
        return eTrafficLightsState::LIGHT_YELLOW;

    return eTrafficLightsState::LIGHT_RED;
}

// 0x49D350
uint8 CTrafficLights::LightForCars1_Visual() {
    if (CGameLogic::LaRiotsActiveHere()) {
        if ((CTimer::GetTimeInMS() / 1024) & 1)
            return eTrafficLightsState::LIGHT_OFF;
        else
            return eTrafficLightsState::LIGHT_YELLOW;
    }

    return LightForCars1();
}

// 0x49D3A0
uint8 CTrafficLights::LightForCars2_Visual() {
    if (CGameLogic::LaRiotsActiveHere()) {
        if ((CTimer::GetTimeInMS() / 1024) & 1)
            return eTrafficLightsState::LIGHT_OFF;
        else
            return eTrafficLightsState::LIGHT_YELLOW;
    }

    return LightForCars2();
}
