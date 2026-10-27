/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerInfo
 * Reconstrucción: COMPLETA — 29/29 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 29/29 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include "PlayerInfo.h"
#include "FireManager.h"
#include "MenuSystem.h"
#include "Hud.h"
#include "Cranes.h"
#include "PedGeometryAnalyser.h"
#include "CarEnterExit.h"
#include "Bridge.h"
#include "TaskComplexLeaveCar.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "TaskComplexEnterBoatAsDriver.h"
#include "TaskComplexStealCar.h"
#include "TaskComplexGoPickUpEntity.h"
#include "TaskSimpleSwim.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleJetPack.h"
#include "EventScriptCommand.h"
#include "CullZones.h"
#include "Renderer.h"

static void __fastcall ProcessCrosshair_Hook(uint32* crosshair, void* edx, uint32 playerIndex, CPad* pad);

void CPlayerInfo::InjectHooks() {
    RH_ScopedClass(CPlayerInfo);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x571920);
    RH_ScopedInstall(CancelPlayerEnteringCars, 0x56E860);
    RH_ScopedInstall(FindObjectToSteal, 0x56DBD0);
    RH_ScopedInstall(EvaluateCarPosition, 0x56DAD0);
    RH_ScopedInstall(Process, 0x56F8D0);
    RH_ScopedNamedGlobalInstall(ProcessCrosshair_Hook, "ProcessCrosshair", 0x56EC80);
    RH_ScopedInstall(FindClosestCarSectorList, 0x56F4E0);
    RH_ScopedInstall(Clear, 0x56F330);
    RH_ScopedInstall(StreamParachuteWeapon, 0x56EB30);
    RH_ScopedInstall(AddHealth, 0x56EAB0);
    RH_ScopedInstall(ArrestPlayer, 0x56E5D0);
    RH_ScopedInstall(SetPlayerSkin, 0x5717F0);
    RH_ScopedInstall(LoadPlayerSkin, 0x56F7D0);
    RH_ScopedInstall(DeletePlayerSkin, 0x56EA80);
    RH_ScopedInstall(BlowUpRCBuggy, 0x56EA30);
    RH_ScopedInstall(MakePlayerSafe, 0x56E870);
    RH_ScopedInstall(PlayerFailedCriticalMission, 0x56E830);
    RH_ScopedInstall(WorkOutEnergyFromHunger, 0x56E610);
    RH_ScopedInstall(KillPlayer, 0x56E580);
    RH_ScopedInstall(IsRestartingAfterMissionFailed, 0x56E570);
    RH_ScopedInstall(IsRestartingAfterArrest, 0x56E560);
    RH_ScopedInstall(IsRestartingAfterDeath, 0x56E550);
    RH_ScopedInstall(IsPlayerInRemoteMode, 0x56DAB0);
    RH_ScopedInstall(GetPos_Hook, 0x56DFB0);
    RH_ScopedInstall(GetSpeed_Hook, 0x56DF50);
    RH_ScopedInstall(GivePlayerParachute, 0x56EC40);
    RH_ScopedInstall(SetLastTargetVehicle, 0x56DA80);
    RH_ScopedInstall(Load, 0x5D3B00);
    RH_ScopedInstall(Save, 0x5D3AC0);
}

// 0x571920
CPlayerInfo::CPlayerInfo() {
    // m_PlayerData is constructed once, in place, before this body.
    // Copying a temporary would overwrite fields the original leaves untouched.
    m_pSkinTexture = nullptr;
    m_bParachuteReferenced = false;
    m_nRequireParachuteTimer = 0;
}

// 0x571920
CPlayerInfo* CPlayerInfo::Constructor() {
    this->CPlayerInfo::CPlayerInfo();
    return this;
}

CVector* CPlayerInfo::GetSpeed_Hook(CVector* out) {
    *out = GetSpeed();
    return out;
}

CVector* CPlayerInfo::GetPos_Hook(CVector* outPos) {
    *outPos = GetPos();
    return outPos;
}

// 0x56EC80: el EXE llama con this = &m_nCrosshairActivated (el byte activo y, a continuacion, la mira en pantalla); ret 8, sin valor de retorno que lea ningun llamador.
static void __fastcall ProcessCrosshair_Hook(uint32* crosshair, void* /*edx*/, uint32 playerIndex, CPad* pad) {
    reinterpret_cast<CPlayerInfo*>(reinterpret_cast<uint8*>(crosshair) - offsetof(CPlayerInfo, m_nCrosshairActivated))->ProcessCrosshair(playerIndex, pad);
}

// 0x56E860
void CPlayerInfo::CancelPlayerEnteringCars(CVehicle* vehicle) {
    // Como el original: no hace nada, pero es __thiscall y saca el argumento de la pila (ret 4)
}

// 0x56DBD0
CEntity* CPlayerInfo::FindObjectToSteal(CPed* ped) {
    const auto centre = ped->GetPosition() + (ped->GetForward() * 0.5f) * 3.0f;
    const auto minX = std::max(0, CWorld::GetSectorX(centre.x - 3.0f));
    const auto minY = std::max(0, CWorld::GetSectorY(centre.y - 3.0f));
    const auto maxX = std::min(119, CWorld::GetSectorX(centre.x + 3.0f));
    const auto maxY = std::min(119, CWorld::GetSectorY(centre.y + 3.0f));
    CWorld::AdvanceCurrentScanCode();
    CEntity* result = nullptr;
    for (auto y = minY; y <= maxY; ++y) {
        for (auto x = minX; x <= maxX; ++x) {
            for (auto* node = CWorld::GetRepeatSector(x, y).Objects.GetNode(); node;) {
                auto* object = node->Item;
                node = node->Next;
                const auto delta = object->GetPosition() - centre;
                if (!object->objectFlags.bIsLiftable || object->GetScanCode() == CWorld::ms_nCurrentScanCode) {
                    continue;
                }
                if (delta.z * delta.z + delta.y * delta.y + delta.x * delta.x < 4.5f) {
                    const auto dot = [&](const CVector& v) { return delta.z * v.z + delta.y * v.y + delta.x * v.x; };
                    const auto forward = dot(ped->GetForward());
                    const auto score = std::abs(dot(ped->GetRight())) * 3.0f + (forward < 0.0f ? 10.0f - forward : forward);
                    // The original neither marks scan codes nor lowers the score limit:
                    // the last qualifying object in sector/list order wins.
                    if (score < 1000.0f) {
                        result = object;
                    }
                }
            }
        }
    }
    return result;
}

// 0x56DAD0
void CPlayerInfo::EvaluateCarPosition(CEntity* car, CPed* ped, float pedToVehDist, float* outDistance, CVehicle** outVehicle) {
    const auto& carPosn = car->GetPosition();
    const auto& pedPosn = ped->GetPosition();
    const auto& forward = ped->GetForward();

    // Find our rotation (so that is, at which angle the forward vector is)
    const auto angleFront = CGeneral::GetATanOfXY(forward.x, forward.y);

    // Find car angle, not relative to our rotation
    // Como el original: el segundo GetATanOfXY se queda en ST0 sin redondear a float (fpatan a precisión completa)
    // y se resta directamente (fsubr): un solo redondeo de angleFront - carAngle.
    const float diffX = carPosn.x - pedPosn.x;
    const float diffY = carPosn.y - pedPosn.y;
    float carAngleFromFront = PlayerInfoAtanFromCpp(&CGeneral::GetATanOfXY, angleFront, diffX, diffY);

    // Como el original: LimitRadianAngle inlinado sin el clamp a +-25 (while > PI: -= 2PI; while < -PI: += 2PI)
    // y la división por 2PI es una multiplicación por 1/(2PI) (0x8594F0).
    const float kPi    = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
    const float kTwoPi = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
    while (carAngleFromFront > kPi) {
        carAngleFromFront -= kTwoPi;
    }
    while (carAngleFromFront < -kPi) {
        carAngleFromFront += kTwoPi;
    }
    if (carAngleFromFront < 0.0f) {
        carAngleFromFront = -carAngleFromFront;
    }

    // Calculate imaginary distance based on the car's angle: The higher the angle the greater the distance
    const auto distance = (1.0f - carAngleFromFront * std::bit_cast<float>(0x3E22F983u)) * (10.0f - pedToVehDist);
    if (distance >= *outDistance) {
        *outDistance = distance;
        *outVehicle = car->AsVehicle();
    }
}

// 0x56F7D0
void CPlayerInfo::LoadPlayerSkin() {
    DeletePlayerSkin();
    m_pSkinTexture = CPlayerSkin::GetSkinTexture(m_szSkinName);
}

// 0x56EA80
void CPlayerInfo::DeletePlayerSkin() {
    if (m_pSkinTexture) {
        RwTextureDestroy(m_pSkinTexture);
        m_pSkinTexture = nullptr;
    }
}

// 0x5717F0
void CPlayerInfo::SetPlayerSkin(const char* name) {
    // Como el original: strncpy de 32 bytes (0x821F40) y LoadPlayerSkin inlinado
    strncpy(m_szSkinName, name, sizeof(m_szSkinName));
    LoadPlayerSkin();
}

// 0x56DA80
void CPlayerInfo::SetLastTargetVehicle(CVehicle* vehicle) {
    CEntity::SafeCleanUpRef(m_pLastTargetVehicle);
    m_pLastTargetVehicle = vehicle;
    CEntity::SafeRegisterRef(m_pLastTargetVehicle);
}

#include "PlayerInfoProcess.cpp.inc"

// 0x56F8D0
void CPlayerInfo::Process(uint32 playerIndex) {
    if (CReplay::Mode == MODE_PLAYBACK) return;
    auto* pad = CPad::GetPad(playerIndex);
    auto* car = m_pPed->bInVehicle ? m_pPed->m_pVehicle : nullptr;
    if (m_bTaxiTimerScore && car && (car->m_nModelIndex == MODEL_TAXI || car->m_nModelIndex == MODEL_CABBIE) && car->m_pDriver == m_pPed && car->m_nNumPassengers) {
        const auto seconds = (CTimer::GetTimeInMS() - m_nTaxiTimer) / 1000;
        m_nMoney += seconds;
        m_nTaxiTimer += seconds * 1000;
    } else m_nTaxiTimer = CTimer::GetTimeInMS();
    ProcessPlayerStunts(*this);
    WorkOutEnergyFromHunger();
    if (m_nMoney != m_nDisplayMoney) {
        const auto difference = m_nMoney - m_nDisplayMoney;
        const auto amount = std::abs(difference);
        const auto step = amount > 100000 ? 12345 : amount > 10000 ? 1234 : amount > 1000 ? 123 : amount > 50 ? 42 : 1;
        m_nDisplayMoney += difference < 0 ? -step : step;
    }
    m_pPed->m_fMaxHealth = CStats::GetFatAndMuscleModifier(STAT_MOD_MAX_HEALTH);
    m_nMaxHealth = uint8(int32(m_pPed->m_fMaxHealth));
    if (m_pPed->bInVehicle && CStats::GetStatValue(STAT_FLYING_SKILL) >= 400.0f) StreamParachuteWeapon(true);
    else if (m_bParachuteReferenced) {
        CGameLogic::IsCoopGameGoingOn();
        if (m_bParachuteReferenced) {
            CStreaming::SetModelIsDeletable(MODEL_GUN_PARA);
            m_bParachuteReferenced = false;
            m_nRequireParachuteTimer = 0;
        }
    }
    if (!(CTimer::GetFrameCounter() & 15)) {
        const auto& pos = m_pPed->bInVehicle ? m_pPed->m_pVehicle->GetPosition() : m_pPed->GetPosition();
        m_fRoadDensityAroundPlayer = ThePaths.CalcRoadDensity(pos.x, pos.y);
    }
    m_fRoadDensityAroundPlayer = std::clamp((m_fRoadDensityAroundPlayer - 1.0f) * 0.6f + 1.0f, 0.5f, 1.45f);
    ProcessPlayerVehicleInteraction(*this, playerIndex, pad);

    if (m_bAfterRemoteVehicleExplosion) {
        const auto elapsed = CTimer::GetTimeInMS() - m_nTimeOfRemoteVehicleExplosion;
        if (CTimer::GetPreviousTimeInMS() - m_nTimeOfRemoteVehicleExplosion < 1000 && elapsed >= 1000 && !m_nPlayerState && m_bFadeAfterRemoteVehicleExplosion) {
            TheCamera.SetFadeColour(0, 0, 0);
            TheCamera.Fade(1.0f, eFadeFlag(0));
        }
        if (elapsed > 2000) {
            if (!m_nPlayerState && m_bFadeAfterRemoteVehicleExplosion) {
                TheCamera.RestoreWithJumpCut();
                TheCamera.SetFadeColour(0, 0, 0);
                TheCamera.Fade(1.0f, eFadeFlag(1));
                TheCamera.Process();
                CTimer::Stop();
                CRenderer::RequestObjectsInFrustum(nullptr, 0);
                CStreaming::LoadAllRequestedModels(false);
                CTimer::Update();
            }
            m_bAfterRemoteVehicleExplosion = false;
            auto& focused = FindPlayerInfo();
            if (focused.m_pRemoteVehicle) focused.m_pRemoteVehicle->m_bRemoveFromWorld = true;
            focused.m_pRemoteVehicle = nullptr;
            if (auto* vehicle = FindPlayerVehicle()) vehicle->SetStatus(STATUS_PLAYER);
        }
    }
    if (!(CTimer::GetFrameCounter() & 31)) {
        auto* vehicle = FindPlayerVehicle();
        if (vehicle && m_pPed->bInVehicle && vehicle->GetUp().z < 0.0f && vehicle->m_vecMoveSpeed.Magnitude() < 0.05f && (vehicle->IsAutomobile() || vehicle->IsBoat()) && !vehicle->physicalFlags.bSubmergedInWater) {
            m_nTimesUpsideDownInARow += vehicle->GetUp().z < -0.5f ? 2 : 1;
        } else m_nTimesUpsideDownInARow = 0;
        if (m_nTimesUpsideDownInARow > 6 && vehicle->vehicleFlags.bCanBeDamaged) {
            vehicle->m_fHealth = std::min(vehicle->m_fHealth, 249.0f);
            if (vehicle->IsAutomobile()) {
                vehicle->AsAutomobile()->m_damageManager.SetEngineStatus(225);
                vehicle->AsAutomobile()->m_fBurnTimer = 0.0f;
            }
        }
    }
    auto* focusedPed = FindPlayerPed();
    if (auto* vehicle = FindPlayerVehicle()) {
        if (vehicle->m_nModelIndex == MODEL_CADDY) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_GOLF_CART, vehicle->m_fMovingSpeed);
        else if (vehicle->IsSubBMX()) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_BICYCLE, vehicle->m_fMovingSpeed);
        else {
            if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_HELICOPTER, vehicle->m_fMovingSpeed);
            if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_PLANE, vehicle->m_fMovingSpeed);
            if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_AUTOMOBILE) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_CAR, vehicle->m_fMovingSpeed);
            if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_BIKE) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_MOTORBIKE, vehicle->m_fMovingSpeed);
            if (vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_BOAT) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_BOAT, vehicle->m_fMovingSpeed);
            if ((vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_PLANE || vehicle->GetVehicleAppearance() == VEHICLE_APPEARANCE_HELI) && vehicle->m_vecMoveSpeed.Magnitude() > 0.2f) CStats::IncrementStat(STAT_FLIGHT_TIME, CTimer::GetTimeStep() * 16.0f);
        }
        if (!FindPlayerTrain() && vehicle->m_vecMoveSpeed.SquaredMagnitude() > 0.0f) {
            switch (vehicle->GetVehicleAppearance()) {
            case VEHICLE_APPEARANCE_BIKE: if (!vehicle->IsSubBMX()) CStats::UpdateStatsWhenOnMotorBike(vehicle->AsBike()); break;
            case VEHICLE_APPEARANCE_HELI: case VEHICLE_APPEARANCE_PLANE: CStats::UpdateStatsWhenFlying(vehicle); break;
            case VEHICLE_APPEARANCE_BOAT: break;
            default: CStats::UpdateStatsWhenDriving(vehicle); break;
            }
        }
    } else if (focusedPed->m_fMovingSpeed > 0.0f) {
        if (m_pPed->GetIntelligence()->GetTaskSwim()) {
            if (CPad::GetPad(0)->GetPedWalkLeftRight() || CPad::GetPad(0)->GetAccelerate()) CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_BY_SWIMMING, focusedPed->m_fMovingSpeed);
        } else if (m_pPed->GetIntelligence()->GetTaskJetPack()) CStats::IncrementStat(STAT_TIME_ON_JETPACK, CTimer::GetTimeStep() * 16.0f);
        else CStats::IncrementStat(STAT_DISTANCE_TRAVELLED_ON_FOOT, focusedPed->m_fMovingSpeed);
    }
    const auto wanted = uint32(m_pPed->GetWanted()->GetWantedLevel());
    if (wanted && !CTheScripts::IsPlayerOnAMission()) {
        auto& lastPosition = StaticRef<CVector>(0xB9B994);
        auto& initialized = StaticRef<uint32>(0xB9B9A0);
        auto& nearRoad = StaticRef<bool>(0x8CDF20);
        auto& moved = StaticRef<bool>(0x8CDF21);
        initialized |= 1;
        if (CTimer::GetTimeInMS() / 20000 != CTimer::GetPreviousTimeInMS() / 20000) {
            moved = (lastPosition - FindPlayerCoors()).Magnitude() >= 10.0f;
            lastPosition = FindPlayerCoors();
            nearRoad = ThePaths.FindNodeClosestToCoors(FindPlayerCoors(), PATH_TYPE_VEH, 60.0f, 1, 0, 0, 0, 0).IsAreaValid();
        }
        constexpr std::array targets{31.0f, 62.0f, 125.0f, 250.0f, 500.0f, 1000.0f};
        const auto delta = ((wanted <= targets.size() ? targets[wanted - 1] : 0.0f) - m_fCurrentChaseValue) * CTimer::GetTimeStep() * 0.0001f;
        if (delta < 0.0f || (moved && nearRoad && !CCullZones::NoPolice() && !CCullZones::PoliceAbandonCars() && CGame::currArea == 0)) m_fCurrentChaseValue += delta;
    } else m_fCurrentChaseValue = 0.0f;
    ProcessCrosshair(playerIndex, pad);
    m_nMoney = std::min(m_nMoney, 999999999);
    m_nDisplayMoney = std::min(m_nDisplayMoney, 999999999);
}

// 0x56EC80 (en el EXE this = &m_nCrosshairActivated: el byte "activa" y, tras el, la mira en pantalla -1..1 en +4/+8)
// Mueve la mira con la palanca de la direccion (x87 a 24 bits: Cross74StepCpp), la recorta a +-0.9f y, si se dispara el arma del coche, lanza un disparo instantaneo desde la camara hacia la mira (200 unidades).
void CPlayerInfo::ProcessCrosshair(uint32 playerIndex, CPad* pad) {
    if (!*reinterpret_cast<const uint8*>(&m_nCrosshairActivated)) {
        return;
    }
    float* const target = &m_vecCrosshairTarget.x; // +4: x, +8: y
    {
        const uint32 timeStep = std::bit_cast<uint32>(CTimer::ms_fTimeStep); // como el original: se lee antes de llamar al mando
        const int32 steer = pad->GetSteeringLeftRight();
        Cross74StepCpp(&target[0], steer, timeStep, false);
    }
    {
        const bool inverted = StaticRef<uint8>(0xB73402) != 0; // CPad::bInvertLook4Pad (cualquier byte distinto de 0)
        const uint32 timeStep = std::bit_cast<uint32>(CTimer::ms_fTimeStep);
        const int32 steer = pad->GetSteeringUpDown();
        Cross74StepCpp(&target[1], steer, timeStep, !inverted);
    }
    // Como el original: orden x alto, x bajo, y alto, y bajo; las comparaciones ordenadas dejan pasar los NaN
    if (target[0] > 0.9f) target[0] = 0.9f;
    if (target[0] < -0.9f) target[0] = -0.9f;
    if (target[1] > 0.9f) target[1] = 0.9f;
    if (target[1] < -0.9f) target[1] = -0.9f;

    if (!pad->GetCarGunFired()) {
        return;
    }
    CamShakeNoPos(&TheCamera, 0.2f);

    // Direccion de la mira: dos lecturas de FindCamFOV (0x50AD20) y la matriz de la camara (0xB6F99C: derecha, delante, arriba, posicion)
    uint32 in[17];
    in[0] = std::bit_cast<uint32>(TheCamera.FindCamFOV());
    in[1] = std::bit_cast<uint32>(TheCamera.FindCamFOV());
    in[2] = std::bit_cast<uint32>(CDraw::ms_fAspectRatio);
    memcpy(&in[3], &target[0], 4);
    memcpy(&in[4], &target[1], 4);
    const auto* const matrix = reinterpret_cast<const uint32*>(&TheCamera.m_mCameraMatrix);
    for (int i = 0; i < 3; ++i) {
        in[5 + i] = matrix[i];       // derecha
        in[8 + i] = matrix[4 + i];   // delante
        in[11 + i] = matrix[8 + i];  // arriba
        in[14 + i] = matrix[12 + i]; // posicion
    }
    uint32 muzzleBits[3], targetBits[3];
    Cross74AimCpp(in, muzzleBits, targetBits);

    CWeapon weapon(eWeaponType(31), 5000); // 0x1F, 0x1388
    // Como el original: el origen es la posicion de la camara copiada por palabras (CPlaceable::GetPosition)
    uint32 originBits[3];
    memcpy(originBits, &TheCamera.GetPosition(), 12);
    CVector origin, aim;
    memcpy(&origin, originBits, 12);
    memcpy(&aim, targetBits, 12);

    // Bit 25 del dword de banderas del ped en +0x470: se apaga durante el disparo y se restaura despues
    const auto flagsOf = [](CPed* ped) { return reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x470); };
    CPed* ped = CWorld::Players[playerIndex].m_pPed;
    const uint32 savedFlags = *flagsOf(ped);
    *flagsOf(ped) = savedFlags & 0xFDFFFFFFu;
    weapon.FireInstantHit(ped, &origin, &origin, nullptr, &aim, nullptr, true, true); // como el original: muzzlePosn es el mismo puntero que el origen (ambos apuntan a la copia local de la posicion de la camara)
    ped = CWorld::Players[playerIndex].m_pPed;
    uint32& flags = *flagsOf(ped);
    flags = (flags & ~0x02000000u) | (savedFlags & 0x02000000u);
}

// 0x56F4E0
void CPlayerInfo::FindClosestCarSectorList(CPtrListDoubleLink<CVehicle*>& ptrList, CPed* ped, float minX, float minY, float maxX, float maxY, float* outVehDist, CVehicle** outVehicle) {
    for (auto* node = ptrList.GetNode(); node;) {
        auto* car = node->Item;
        node = node->Next;
        if (car->GetScanCode() == CWorld::ms_nCurrentScanCode || !car->m_bUsesCollision || !car->GetIsTypeVehicle()) {
            continue;
        }
        car->SetScanCode(CWorld::ms_nCurrentScanCode);
        if (car->GetStatus() == STATUS_WRECKED || car->GetStatus() == STATUS_TRAIN_MOVING) {
            continue;
        }
        if (car->GetMatrix().GetUp().z <= 0.3f && !car->IsBike()) {
            continue;
        }
        CVector position;
        car->GetBoundCentre(position);
        position = car->GetPosition();
        position.z = position.z - car->GetHeightAboveRoad() + 1.0f;
        auto height = ped->GetPosition().z - position.z;
        CVector doorPosition;
        int32 door = 0;
        bool heightOK = false;
        if (car->m_nModelIndex == MODEL_AT400 && CCarEnterExit::GetNearestCarDoor(ped, car, doorPosition, door)) {
            height = std::abs(ped->GetPosition().z - doorPosition.z);
            heightOK = height < 1.0f;
        }
        heightOK = heightOK || std::abs(height) < 2.0f;
        if (!heightOK && !(car->IsBoat() && ((position.z < ped->GetPosition().z && ped->GetPosition().z - 4.0f < position.z) || ped->m_pContactEntity == car))) {
            continue;
        }
        door = 0;
        if (!car->vehicleFlags.bConsideredByPlayer || !CCarEnterExit::GetNearestCarDoor(ped, car, doorPosition, door)) {
            continue;
        }
        const auto delta = ped->GetPosition() - car->GetPosition();
        auto distance = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        if (distance > 10.0f && CPedGeometryAnalyser::LiesInsideBoundingBox(*ped, ped->GetPosition(), *car)) {
            distance = 10.0f;
        }
        if (distance <= 10.0f && !CCranes::IsThisCarBeingCarriedByAnyCrane(car)) {
            EvaluateCarPosition(car, ped, distance, outVehDist, outVehicle);
        }
    }
}

// 0x56F330
void CPlayerInfo::Clear() {
    // TODO: This should just use the constructor and `swap`,
    // like to just swap ourselves to a default constructed object
    // and do all the cleanup in the destructor

    m_pPed = nullptr;
    m_pRemoteVehicle = nullptr;
    if (m_pSpecCar) {
        m_pSpecCar->physicalFlags.bAddMovingCollisionSpeed = false;
        m_pSpecCar = nullptr;
    }
    m_nDisplayMoney = 0;
    m_nMoney = 0;
    m_nPlayerState = PLAYERSTATE_PLAYING;
    m_nCarDensityForCurrentZone = 0;
    m_fRoadDensityAroundPlayer = 1.0f;
    m_bAfterRemoteVehicleExplosion = false;
    m_bCreateRemoteVehicleExplosion = false;
    m_bFadeAfterRemoteVehicleExplosion = false;
    m_bTryingToExitCar = false;
    m_bTaxiTimerScore = false;
    m_nTaxiTimer = 0;
    m_nVehicleTimeCounter = CTimer::GetTimeInMS();
    m_nMaxArmour = 100;
    m_nMaxHealth = 100;
    m_bCanDoDriveBy = true;
    m_nCollectablesPickedUp = 0;
    m_nTotalNumCollectables = 3;
    m_nLastTimeEnergyLost = 0;
    m_nLastTimeArmourLost = 0;
    m_nLastTimeBigGunFired = 0;
    m_nTimesStuckInARow = 0;
    m_nTimesUpsideDownInARow = 0;
    m_nCarTwoWheelCounter = 0;
    m_fCarTwoWheelDist = 0.0f;
    m_nCarLess3WheelCounter = 0;
    m_nBikeRearWheelCounter = 0;
    m_fBikeRearWheelDist = 0.0f;
    m_nBikeFrontWheelCounter = 0;
    m_fBikeFrontWheelDist = 0.0f;
    m_nTempBufferCounter = 0;
    m_nBestCarTwoWheelsTimeMs = 0;
    m_fBestCarTwoWheelsDistM = 0.0f;
    m_nBestBikeWheelieTimeMs = 0;
    m_fBestBikeWheelieDistM = 0.0f;
    m_nBestBikeStoppieTimeMs = 0;
    m_fBestBikeStoppieDistM = 0.0f;
    m_bDoesNotGetTired = false;
    m_bFastReload = false;
    m_bFireProof = false;
    m_bGetOutOfJailFree = false;
    m_bFreeHealthCare = false;
    m_nTimeOfLastCarExplosionCaused = 0;
    m_nExplosionMultiplier = 0;
    m_nHavocCaused = 0;
    FindPlayerInfo().m_nNumHoursDidntEat = 0;
    m_nLastBustMessageNumber = 1;
    m_fCurrentChaseValue = 0.0f;
    m_nBustedAudioStatus = 0;
    *reinterpret_cast<uint8*>(&m_nCrosshairActivated) = 0; // Como el original: solo escribe el byte bajo (0x158)

    m_nRequireParachuteTimer = 0;

    if (m_bParachuteReferenced) {
        CGameLogic::IsCoopGameGoingOn();
        if (m_bParachuteReferenced) {
            CStreaming::SetModelIsDeletable(MODEL_GUN_PARA);
            m_bParachuteReferenced = false;
            m_nRequireParachuteTimer = 0;
        }
    }
}

// 0x56EC40
void CPlayerInfo::GivePlayerParachute() const {
    if (m_nRequireParachuteTimer) {
        if (CStreaming::IsModelLoaded(MODEL_GUN_PARA)) {
            m_pPed->GiveWeapon(WEAPON_PARACHUTE, 1, true);
            m_pPed->SetSavedWeapon(WEAPON_PARACHUTE);
        }
    }
}

// 0x56EB30
void CPlayerInfo::StreamParachuteWeapon(bool unk) {
    if (CGameLogic::IsCoopGameGoingOn()) {
        if (unk) {
            return;
        }
        if (m_bParachuteReferenced) {
            CStreaming::SetModelIsDeletable(MODEL_GUN_PARA);
            m_bParachuteReferenced = false;
            m_nRequireParachuteTimer = 0;
        }
        return;
    }

    // Como el original: fuera del modo cooperativo, con el argumento a false solo libera el paracaídas
    if (unk) {
        if (m_pPed && m_pPed->IsInVehicle()) {
            if (m_pPed->m_pVehicle->IsSubPlane() || m_pPed->m_pVehicle->IsSubHeli()) {
                // Como el original: ms_fTimeStep * 0.02f * 1000.0f truncado con _ftol2 (64 bits)
                const auto timeStepMS = (uint32)(int64)(CTimer::GetTimeStep() * 0.02f * 1000.0f);
                if (m_nRequireParachuteTimer <= timeStepMS) {
                    const auto groundHeight = TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_BOTTOM);
                    const auto vehToGroundZDist = m_pPed->m_pVehicle->GetPosition().z - groundHeight;
                    // Como el original: NaN -> 0
                    m_nRequireParachuteTimer = (vehToGroundZDist > 50.f) ? 5000 : 0;
                } else {
                    m_nRequireParachuteTimer -= timeStepMS;
                }
            }
        }

        if (m_nRequireParachuteTimer) {
            CStreaming::RequestModel(MODEL_GUN_PARA, STREAMING_MISSION_REQUIRED);
            m_bParachuteReferenced = true;
            return;
        }
    }

    if (m_bParachuteReferenced) {
        CStreaming::SetModelIsDeletable(MODEL_GUN_PARA);
        m_bParachuteReferenced = false;
        m_nRequireParachuteTimer = 0;
    }
}

// 0x56EAB0
void CPlayerInfo::AddHealth(int32 amount) const {
    // Como el original: fild + fadd, el entero se suma sin redondearlo antes a float
    const float health = m_pPed->m_fHealth;
    const float sum = FloatPlusIntCpp(health, amount);
    const auto newValue = std::min((float)m_nMaxHealth, sum); // Clamp to m_nMaxHealth
    m_pPed->m_fHealth = std::max(newValue, m_pPed->m_fHealth); // Don't change health to a lower value
}

// 0x56EA30
void CPlayerInfo::BlowUpRCBuggy(bool bExplode) const {
    if (m_pRemoteVehicle && !m_pRemoteVehicle->m_bRemoveFromWorld) {
        CRemote::TakeRemoteControlledCarFromPlayer(bExplode);
        if (bExplode)
            m_pRemoteVehicle->BlowUpCar(CWorld::Players[CWorld::PlayerInFocus].m_pPed, false); // Como el original: FindPlayerPed inlinado
    }
}

// 0x56E870
void CPlayerInfo::MakePlayerSafe(bool enable, float radius) {
    // Como el original: mismo orden de escrituras y llamadas; m_bCanBeDamaged se escribe en los
    // CPlayerPedData del ped (m_pPed->m_pPlayerData), no en los de este CPlayerInfo.
    auto* const pad = m_pPed->GetPadFromPlayer();
    auto* const wanted = m_PlayerData.m_pWanted;
    if (enable) {
        wanted->m_bEverybodyBackOff = true;
        CWorld::StopAllLawEnforcersInTheirTracks();
        pad->bPlayerSafe = true;
        CPad::StopPadsShaking();

        m_pPed->physicalFlags.bInvulnerable = true;
        m_pPed->physicalFlags.bBulletProof = true;
        m_pPed->physicalFlags.bFireProof = true;
        m_pPed->physicalFlags.bExplosionProof = true;
        m_pPed->physicalFlags.bCollisionProof = true;
        m_pPed->physicalFlags.bMeleeProof = true;
        m_pPed->GetPlayerData()->m_bCanBeDamaged = false;

        m_pPed->ClearAdrenaline();
        if (m_PlayerData.m_fTimeCanRun < 0.0f) {
            m_PlayerData.m_fTimeCanRun = 0.0f;
        }
        m_pPed->GetIntelligence()->ClearTasks(true, false);

        gFireManager.ExtinguishPoint(GetPos(), radius);
        CExplosion::RemoveAllExplosionsInArea(GetPos(), 4000.f);
        CProjectileInfo::RemoveAllProjectiles();
        CWorld::SetAllCarsCanBeDamaged(false);
        CWorld::ExtinguishAllCarFiresInArea(GetPos(), radius);
        CReplay::DisableReplays();
        m_pPed->ClearWeaponTarget();
    } else {
        wanted->m_bEverybodyBackOff = false;
        pad->bPlayerSafe = false;

        m_pPed->physicalFlags.bInvulnerable = false;
        m_pPed->physicalFlags.bBulletProof = false;
        m_pPed->physicalFlags.bFireProof = false;
        m_pPed->physicalFlags.bExplosionProof = false;
        m_pPed->physicalFlags.bCollisionProof = false;
        m_pPed->physicalFlags.bMeleeProof = false;
        m_pPed->GetPlayerData()->m_bCanBeDamaged = true;

        CWorld::SetAllCarsCanBeDamaged(true);
        CReplay::EnableReplays();
    }
}

// 0x56E830
void CPlayerInfo::PlayerFailedCriticalMission() {
    if (m_nPlayerState == PLAYERSTATE_PLAYING) {
        m_nPlayerState = PLAYERSTATE_FAILED_MISSION;
        CGameLogic::SetMissionFailed();
        CDarkel::ResetOnPlayerDeath();
    }
}

// 0x56E610
void CPlayerInfo::WorkOutEnergyFromHunger() {
    // Como el original: la guarda del static es el bit 0 del dword 0xB9B8F4 y el último estado de hambre es un byte sin signo
    static auto& s_lastTimeHungryStateProcessedGuard = StaticRef<uint32>(0xB9B8F4);
    static auto& s_lastTimeHungryStateProcessed = StaticRef<uint8>(0xB9B8F2);
    static auto& s_LastHungryState = StaticRef<uint8>(0xB9B8F1);
    static auto& s_bHungryMessageShown = StaticRef<bool>(0xB9B8F0);

    if (CCheat::IsActive(CHEAT_NEVER_GET_HUNGRY)) {
        return;
    }

    if (!(s_lastTimeHungryStateProcessedGuard & 1)) {
        s_lastTimeHungryStateProcessedGuard |= 1;
        s_lastTimeHungryStateProcessed = CClock::GetGameClockHours();
    }

    if (   CPad::GetPad(0)->ArePlayerControlsDisabled()
        || CMenuSystem::num_menus_in_use
        || TheCamera.m_bWideScreenOn
        || CCutsceneMgr::ms_running
        || CGameLogic::IsCoopGameGoingOn()
        || m_pRemoteVehicle
    ) {
        return;
    }

    if (!m_pPed)
        return;

    if (m_pPed->m_pAttachedTo)
        return;

    // Como el original: la hora procesada se lee una vez aquí (dl) y se reutiliza en la segunda comparación
    const auto lastProcessed = s_lastTimeHungryStateProcessed;
    if (CClock::GetGameClockHours() != lastProcessed) {
        if (!m_nNumHoursDidntEat)
            s_LastHungryState = 0;
        m_nNumHoursDidntEat += 1;
    }

    // Como el original: comparación con signo de 16 bits (jle)
    if ((int16)m_nNumHoursDidntEat <= 48) {
        s_bHungryMessageShown = false;
    } else {
        if (CClock::GetGameClockHours() == lastProcessed)
            return;

        m_pPed->Say(CTX_GLOBAL_STOMACH_RUMBLE);
        CPad::GetPad(0)->StartShake(400, 110u, 0);

        if (s_bHungryMessageShown) {
            bool bDecreaseHealth{};
            if (CStats::GetStatValue(STAT_FAT) > 0.0f) {
                CStats::DecrementStat(STAT_FAT, 25.0f);
                CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_FAT, 25.0f);
                bDecreaseHealth = true;
                if (!s_LastHungryState) {
                    s_LastHungryState = (uint8)((uint8)m_nNumHoursDidntEat + 24);
                }
            }

            // Como el original: (int16)horas > estado (byte sin signo) con jg; NaN en el músculo va a la rama de salud
            if (CStats::GetStatValue(STAT_MUSCLE) > 0.0f && ((int16)m_nNumHoursDidntEat > (int32)s_LastHungryState || !s_LastHungryState)) {
                CStats::DecrementStat(STAT_MUSCLE, 25.0f);
                CStats::DisplayScriptStatUpdateMessage(STAT_UPDATE_DECREASE, STAT_MUSCLE, 25.0f);
            } else if (!bDecreaseHealth) {
                m_pPed->m_fHealth -= 2.0f;
            }
        } else {
            CHud::SetHelpMessage(TheText.Get("NOTEAT"), true, false, true);
            s_bHungryMessageShown = true;
        }
    }

    if (CClock::GetGameClockHours() != s_lastTimeHungryStateProcessed) {
        s_lastTimeHungryStateProcessed = CClock::GetGameClockHours();
    }
}

// 0x56E5D0
void CPlayerInfo::ArrestPlayer() {
    if (m_nPlayerState == PLAYERSTATE_PLAYING) {
        m_nPlayerState = PLAYERSTATE_HAS_BEEN_ARRESTED;
        m_nBustedAudioStatus = 0;
        CDarkel::ResetOnPlayerDeath();
        CStats::IncrementStat(STAT_TIMES_BUSTED, 1.0f);
        CGangWars::EndGangWar(false);
    }
}

// 0x56E580
void CPlayerInfo::KillPlayer() {
    if (m_nPlayerState == PLAYERSTATE_PLAYING) {
        m_nPlayerState = PLAYERSTATE_HAS_DIED;
        CDarkel::ResetOnPlayerDeath();
        CMessages::AddBigMessage(TheText.Get("DEAD"), 4000, STYLE_WHITE_MIDDLE);
        CStats::IncrementStat(STAT_NUMBER_OF_HOSPITAL_VISITS, 1.0f);
        CGangWars::EndGangWar(false);
    }
}

// 0x56E570
bool CPlayerInfo::IsRestartingAfterMissionFailed() const {
    return m_nPlayerState == PLAYERSTATE_FAILED_MISSION;
}

// 0x56E560
bool CPlayerInfo::IsRestartingAfterArrest() const {
    return m_nPlayerState == PLAYERSTATE_HAS_BEEN_ARRESTED;
}

// 0x56E550
bool CPlayerInfo::IsRestartingAfterDeath() const {
    return m_nPlayerState == PLAYERSTATE_HAS_DIED;
}

// 0x56DAB0
bool CPlayerInfo::IsPlayerInRemoteMode() const {
    return m_pRemoteVehicle || m_bAfterRemoteVehicleExplosion;
}

// 0x56DFB0
// Return occupied vehicle's (if in any) or player's ped position
CVector CPlayerInfo::GetPos() const {
    return m_pPed->IsInVehicle() ? m_pPed->m_pVehicle->GetPosition() : m_pPed->GetPosition();
}

// 0x56DF50
// Return occupied vehicle's (if in any) or player's ped move speed
CVector CPlayerInfo::GetSpeed() const {
    return m_pPed->IsInVehicle() ? m_pPed->m_pVehicle->GetMoveSpeed() : m_pPed->GetMoveSpeed();
}

// 0x5D3B00
bool CPlayerInfo::Load() {
    CGenericGameStorage::LoadDataFromWorkBuffer<int32>(); // Discarded
    auto data = CGenericGameStorage::LoadDataFromWorkBuffer<CPlayerInfoSaveStructure>();
    data.Extract(this);
    return true;
}

// 0x5D3AC0
bool CPlayerInfo::Save() {
    CPlayerInfoSaveStructure data;
    data.Construct(this);
    CGenericGameStorage::SaveDataToWorkBuffer(sizeof(CPlayerInfoSaveStructure));
    CGenericGameStorage::SaveDataToWorkBuffer(data);
    return true;
}

// 0x45DEF0
CPlayerInfo& CPlayerInfo::operator=(const CPlayerInfo& rhs) {
    // Como el original: copia por campos sin tocar el relleno (el EXE no usa memcpy de la estructura).
    auto* const dst = reinterpret_cast<uint8*>(this);
    const auto* const src = reinterpret_cast<const uint8*>(&rhs);
    const auto copy = [&](size_t off, size_t size) { memcpy(dst + off, src + off, size); };

    copy(0x0, 4); // m_pPed

    // 0x45C4B0 (CPlayerPedData, +4): solo copia los campos, no el relleno (+0x22/0x23, +0x87, +0x8F).
    // Las banderas de +0x34 las mezcla con un registro sin inicializar (ESI = this del CPlayerInfo):
    // solo el bit 9 queda con el valor de destino, los bits 10-12 con el de origen y el resto con ESI.
    uint8* const pd = dst + 4;
    const uint8* const ps = src + 4;
    const auto copyPd = [&](size_t off, size_t size) { memcpy(pd + off, ps + off, size); };
    copyPd(0x00, 0x22); // 0x00..0x21 (dwords hasta 0x1C + bytes 0x20/0x21)
    copyPd(0x24, 0x10); // 0x24..0x33
    {
        uint32 oldFlags, srcFlags;
        memcpy(&oldFlags, pd + 0x34, 4);
        memcpy(&srcFlags, ps + 0x34, 4);
        const uint32 esi = static_cast<uint32>(reinterpret_cast<uintptr_t>(this));
        const uint32 newFlags = (esi & ~0x1E00u) | (oldFlags & 0x200u) | (srcFlags & 0x1C00u);
        memcpy(pd + 0x34, &newFlags, 4);
    }
    copyPd(0x38, 0x08); // 0x38..0x3F
    copyPd(0x40, 0x04);
    copyPd(0x44, 0x30); // 0x44..0x73
    copyPd(0x74, 0x0C);
    copyPd(0x80, 0x07); // 0x80 dword + bytes 0x84..0x86
    copyPd(0x88, 0x07); // words 0x88/0x8A + byte 0x8C..0x8E (0x88..0x8E)
    copyPd(0x90, 0x10); // 0x90..0x9F
    {
        // Bits 0-30 de +0xA0 (el bit 31 acaba también con el valor de origen)
        memcpy(pd + 0xA0, ps + 0xA0, 4);
    }
    copyPd(0xA4, 0x08);

    copy(0xB0, 0x24); // 0xB0..0xD3 (dwords)
    copy(0xD4, 2);    // m_bTaxiTimerScore, m_bTryingToExitCar
    copy(0xD8, 4);    // m_pLastTargetVehicle
    copy(0xDC, 4);    // estado y 3 bytes
    copy(0xE0, 0x50); // 0xE0..0x12F
    copy(0x130, 2);   // m_nCarDensityForCurrentZone
    copy(0x134, 0x10);// 0x134..0x143
    copy(0x144, 2);   // m_nNumHoursDidntEat
    copy(0x148, 4);   // m_fCurrentChaseValue
    copy(0x14C, 9);   // 0x14C..0x154
    copy(0x156, 2);   // m_nLastBustMessageNumber
    copy(0x158, 0x0C);
    copy(0x164, 0x20);// m_szSkinName entero (el EXE copia los 32 bytes)
    copy(0x184, 4);   // m_pSkinTexture
    copy(0x188, 1);   // m_bParachuteReferenced
    copy(0x18C, 4);   // m_nRequireParachuteTimer
    return *this;
}
