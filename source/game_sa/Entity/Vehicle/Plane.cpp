/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlane
 * Reconstrucción: COMPLETA — 23/23 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 23/23 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Plane.h"
#include "CarCtrl.h"
#include "FireManager.h"
#include "Streaming.h"
#include "TheScripts.h"
#include "GameLogic.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupIntelligence.h"
#include "PedIntelligence.h"
#include "EventDanger.h"
#include "EventGroup.h"
#include "PlayerPed.h"
#include "Shadows.h"
#include "MotionBlurStreaks.h"
#include "BouncingPanel.h"
#include "X87Math.h"

auto& HARRIER_NOZZLE_ROTATERATE = StaticRef<float>(0x8D33DC);       // 25.0f
auto& PLANE_DAMAGE_WAVE_COUNTER_VAR = StaticRef<float>(0x8D33E0);   // 0.75f
auto& PLANE_DAMAGE_THRESHHOLD = StaticRef<float>(0x8D33E4);         // 500.0f
auto& PLANE_DAMAGE_SCALE_MASS = StaticRef<float>(0x8D33E8);         // 10000.0f
auto& PLANE_DAMAGE_DESTROY_THRESHHOLD = StaticRef<float>(0x8D33EC); // 5000.0f
auto& vecRCBaronGunPos = StaticRef<CVector>(0x8D33F0);            // <0.0f, 0.45f, 0.0f>

void CPlane::InjectHooks() {
    RH_ScopedVirtualClass(CPlane, 0x871948, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6C8E20);
    RH_ScopedInstall(InitPlaneGenerationAndRemoval, 0x6CAD90);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6C9140);
    RH_ScopedVMTInstall(BurstTyre, 0x6C9150);
    RH_ScopedVMTInstall(PreRender, 0x6C94A0);
    RH_ScopedVMTInstall(Render, 0x6CAB70);
    RH_ScopedInstall(IsAlreadyFlying, 0x6CAB90);
    RH_ScopedVMTInstall(Fix, 0x6CABB0);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6CAC10);
    RH_ScopedInstall(SetGearUp, 0x6CAC20);
    RH_ScopedInstall(SetGearDown, 0x6CAC70);
    RH_ScopedVMTInstall(OpenDoor, 0x6CACB0);
    RH_ScopedVMTInstall(ProcessControl, 0x6C9260);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6CADD0);
    RH_ScopedVMTInstall(ProcessFlyingCarStuff, 0x6CB7C0);
    RH_ScopedVMTInstall(VehicleDamage, 0x6CC4B0);
    RH_ScopedInstall(CountPlanesAndHelis, 0x6CCA50);
    RH_ScopedInstall(AreWeInNoPlaneZone, 0x6CCAA0);
    RH_ScopedInstall(AreWeInNoBigPlaneZone, 0x6CCBB0);
    RH_ScopedInstall(SwitchAmbientPlanes, 0x6CCC50);
    RH_ScopedVMTInstall(BlowUpCar, 0x6CCCF0);
    RH_ScopedInstall(FindPlaneCreationCoors, 0x6CD090);
    RH_ScopedInstall(DoPlaneGenerationAndRemoval, 0x6CD2F0);
}

// 0x6C8E20
CPlane::CPlane(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, true) {
    m_nVehicleSubType = VEHICLE_TYPE_PLANE;

    m_fLeftRightSkid               = 0.0f;
    m_fSteeringUpDown              = 0.0f;
    m_fSteeringLeftRight           = 0.0f;
    m_fAccelerationBreakStatus     = 0.0f;
    m_fAccelerationBreakStatusPrev = 1.0f;
    m_fPropSpeed                   = 0.0f;
    field_9C8                      = 0.0f;
    m_fLandingGearStatus           = 0.0f;
    field_9A0                      = 0;
    m_planeCreationHeading         = 0.0f;
    m_planeHeading                 = 0.0f;
    m_planeHeadingPrev             = 0.0f;
    m_maxAltitude                  = 15.0f;
    m_altitude                     = 25.0f;
    m_minAltitude                  = 20.0f;
    m_forwardZ                     = 0;
    m_nStartedFlyingTime           = 0;
    m_fSteeringFactor              = 0.0f;

    if (m_nModelIndex != MODEL_VORTEX)
        physicalFlags.bDontCollideWithFlyers = true;

    m_nExtendedRemovalRange = 255;
    vehicleFlags.bNeverUseSmallerRemovalRange = true;
    vehicleFlags.bIsBig = true;

    auto& leftDoor = m_doors[DOOR_LEFT_FRONT];
    switch (modelIndex) {
    case MODEL_HYDRA:
    case MODEL_RUSTLER:
    case MODEL_CROPDUST:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init(std::bit_cast<float>(0x3FF1463Bu), 0.0f, DOOR_AXIS_NEG_X, DOOR_AXIS_Y, DOOR_EXTRA_BASED); // Como el original (0x6C9053/0x6C9010): mov [esi+5E8h],3FF1463Bh (no 3*PI/5 con PI=3.14159f)
        break;
    case MODEL_SHAMAL:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init(std::bit_cast<float>(0xC016CBE4u), 0.0f, DOOR_AXIS_Z, DOOR_AXIS_Y, DOOR_EXTRA_BASED); // Como el original (0x6C8F5F): mov [esi+5E8h],0C016CBE4h
        rwObjectSetFlags(GetFirstObject(m_aCarNodes[PLANE_WHEEL_LF]), 0);
        break;
    case MODEL_NEVADA:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init(std::bit_cast<float>(0xBFA0D97Cu), 0.0f, DOOR_AXIS_NEG_Y, DOOR_AXIS_Z, DOOR_EXTRA_BASED); // Como el original (0x6C8FAC): mov [esi+5E8h],0BFA0D97Ch
        break;
    case MODEL_VORTEX:
        if (m_panels[FRONT_LEFT_PANEL].m_nFrameId == (uint16)-1)
            m_panels[FRONT_LEFT_PANEL].SetPanel(PLANE_GEAR_L, 1, -0.25f);
        break;
    case MODEL_STUNT:
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_OK);
        leftDoor.Init(std::bit_cast<float>(0x3FF1463Bu), 0.0f, DOOR_AXIS_NEG_X, DOOR_AXIS_Y, DOOR_EXTRA_BASED); // Como el original (0x6C9053/0x6C9010): mov [esi+5E8h],3FF1463Bh (no 3*PI/5 con PI=3.14159f)
        rwObjectSetFlags(GetFirstObject(m_aCarNodes[PLANE_WHEEL_LB]), 0);
        rwObjectSetFlags(GetFirstObject(m_aCarNodes[PLANE_WHEEL_RB]), 0);
        break;
    }

    CVector modelPos, localPos;
    for (auto wheelId = 0; wheelId < 4; wheelId++) {
        GetVehicleModelInfo()->GetWheelPosn(wheelId, modelPos, false);
        GetVehicleModelInfo()->GetWheelPosn(wheelId, localPos, true);
        m_wheelPosition[wheelId] = m_wheelPosition[wheelId] - modelPos.z + localPos.z;
    }

    m_planeDamageWave = 0;
    m_pGunParticles = nullptr;
    m_nFiringMultiplier = 16;
    field_9DC = 0;
    field_9E0 = 0;
    m_apJettrusParticles.fill(nullptr);

    m_pSmokeParticle = nullptr;

    if (m_nModelIndex == MODEL_HYDRA)
        m_wMiscComponentAngle = HARRIER_NOZZLE_ROTATE_LIMIT;

    m_bSmokeEjectorEnabled = false;
}

// 0x6C9160
CPlane::~CPlane() {
    if (m_pGunParticles) {
        for (auto i = 0; i < CVehicle::GetPlaneNumGuns(); i++) {
            if (auto& particle = m_pGunParticles[i]) {
                particle->Kill();
                g_fxMan.DestroyFxSystem(particle);
            }
        }
        delete[] m_pGunParticles;
        m_pGunParticles = nullptr;
    }

    for (auto particle : m_apJettrusParticles) {
        if (particle) {
            FxSystem_c::KillAndClear(particle);
        }
    }

    FxSystem_c::SafeKillAndClear(m_pSmokeParticle);

    m_vehicleAudio.Terminate();
}

// 0x6CAD90
void CPlane::InitPlaneGenerationAndRemoval() {
    GenPlane_Status = 0;
    GenPlane_LastTimeGenerated = 0;
    GenPlane_Active = true;
}

// 0x6CCCF0
void CPlane::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    auto* bytes = reinterpret_cast<uint8*>(this);
    if (!(bytes[0x42A] & 0x20)) return;
    if (GetStatus() != STATUS_PLAYER && uint8(m_autoPilot.m_nCarMission) != 57 && m_nModelIndex != MODEL_RCBARON) {
        m_autoPilot.m_nCarMission = static_cast<eCarMission>(57);
        m_fHealth = 0.f;
        return;
    }
    if (damager == FindPlayerPed(-1) || damager == FindPlayerVehicle(-1, false)) {
        auto& info = CWorld::Players[CWorld::PlayerInFocus];
        info.m_nHavocCaused += 20;
        info.m_fCurrentChaseValue += 10.f;
        const int32 cost = CGeneral::GetRandomNumber() % 6000 + 4000;
        CStats::IncrementStat(static_cast<eStats>(125), float(cost));
    }
    if (GetStatus() == STATUS_PLAYER) {
        if (m_pDriver) m_pDriver->bDontRender = true;
        for (auto* ped : m_apPassengers) if (ped) ped->bDontRender = true;
        m_nFlags &= 0xFFFFFF7E;
        m_vecMoveSpeed = CVector{0.f, 0.f, 0.f};
        m_vecTurnSpeed = CVector{0.f, 0.f, 0.f};
    }
    bytes[0x36] = (bytes[0x36] & 7) | 0x28;
    m_nPhysicalFlags |= 0x20000000;
    m_nTimeWhenBlowedUp = CTimer::GetTimeInMS();
    CVisibilityPlugins::SetClumpForAllAtomicsFlag(GetRpClump(), 0x4000);
    m_damageManager.FuckCarCompletely(false);
    if (m_nModelIndex != MODEL_RCBARON) {
        CAutomobile::SetBumperDamage(static_cast<ePanels>(5), false);
        CAutomobile::SetBumperDamage(static_cast<ePanels>(6), false);
        for (int32 door = 0; door < 6; ++door) CAutomobile::SetDoorDamage(static_cast<eDoors>(door), false);
        CAutomobile::SpawnFlyingComponent(static_cast<eCarNodes>(5), 1);
        if (auto* node = m_aCarNodes[5]) {
            RpAtomic* atomic{};
            RwFrameForAllObjects(node, GetCurrentAtomicObjectCB, &atomic);
            if (atomic) rwObjectSetFlags(atomic, 0);
        }
    }
    bytes[0x4A8] &= 0xF8;
    m_fHealth = 0.f;
    m_wBombTimer = 0;
    TheCamera.CamShake(.4f, GetPosition());
    KillPedsInVehicle();
    bytes[0x4A8] &= 0xE7;
    bytes[0x428] &= 0xAF;
    bytes[0x42D] &= 0x7F;
    bytes[0x868] &= 0xFE;
    m_bSmokeEjectorEnabled = false;
    if (bytes[0x428] & 2) { bytes[0x428] &= 0xFD; --CCarCtrl::NumAmbulancesOnDuty; }
    if (bytes[0x428] & 4) { bytes[0x428] &= 0xFB; --CCarCtrl::NumFireTrucksOnDuty; }
    ChangeLawEnforcerState(false);
    gFireManager.StartFire(this, damager, .8f, 1, 7000, 0);
    CDarkel::RegisterCarBlownUpByPlayer(*this, 0);
    CExplosion::AddExplosion(this, damager, static_cast<eExplosionType>(m_nModelIndex == MODEL_RCBARON ? 12 : 7), GetPosition(), 0, 1, -1.f, 0);
}

// 0x6CABB0
void CPlane::Fix() {
    m_damageManager.ResetDamageStatus();
    if (m_pHandlingData->m_bNoDoors) {
        m_damageManager.SetDoorStatus(DOOR_LEFT_FRONT, DAMSTATE_NOTPRESENT);
        m_damageManager.SetDoorStatus(DOOR_RIGHT_FRONT, DAMSTATE_NOTPRESENT);
        m_damageManager.SetDoorStatus(DOOR_LEFT_REAR, DAMSTATE_NOTPRESENT);
        m_damageManager.SetDoorStatus(DOOR_RIGHT_REAR, DAMSTATE_NOTPRESENT);
    }
    SetupDamageAfterLoad();
}

// 0x6CACB0
void CPlane::OpenDoor(CPed* ped, int32 componentId, eDoors door, float doorOpenRatio, bool playSound) {
    CAutomobile::OpenDoor(ped, componentId, door, doorOpenRatio, playSound);

    if (m_nModelIndex != MODEL_STUNT)
        return;

    // Unfinished code R*, which removed in Android
    if (false) // byte_C1CAFC
    {
        CMatrix matrix(RwFrameGetMatrix(m_aCarNodes[componentId]), false);
        const auto y = m_doors[door].m_angle - m_doors[door].m_prevAngle + matrix.GetPosition().y;
        matrix.SetTranslate({matrix.GetPosition().x, y, matrix.GetPosition().z});
        matrix.UpdateRW();
    }
}

// 0x6CAC10
void CPlane::SetupDamageAfterLoad() {
    vehicleFlags.bIsDamaged = false;
}

// 0x6CC4B0
void CPlane::VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    if (m_nModelIndex == 539) {
        CAutomobile::VehicleDamage(damageIntensity, component, damager, vecCollisionCoors, vecCollisionDirection, weapon);
        return;
    }
    auto* bytes = reinterpret_cast<uint8*>(this);
    if (!(bytes[0x42A] & 0x20)) return;
    if (GetStatus() != STATUS_PLAYER && (m_nPhysicalFlags & 0x400000)) {
        auto* oldDamager = m_pDamageEntity;
        if (oldDamager != FindPlayerPed(-1)) {
            oldDamager = m_pDamageEntity;
            if (oldDamager != FindPlayerVehicle(-1, false)) return;
        }
    }
    // push EDI at 0x6CC51A shifts the initial local offsets: subsequent
    // [esp+0x10] is the 500 part threshold, [esp+0x0C] the 5000 destroy threshold.
    float damageThreshold = PLANE_DAMAGE_DESTROY_THRESHHOLD;
    float partThreshold = PLANE_DAMAGE_THRESHHOLD;
    bool explosive = false;
    if (damageIntensity == 0.f) {
        damageIntensity = m_fDamageIntensity;
        partThreshold = PlaneMassThresholdCpp(m_fMass, PLANE_DAMAGE_SCALE_MASS, partThreshold, true);
        damageThreshold = PlaneMassThresholdCpp(m_fMass, PLANE_DAMAGE_SCALE_MASS, damageThreshold, false);
        vecCollisionCoors = &m_vecLastCollisionPosn;
        if (damageIntensity <= 0.f || (m_nPhysicalFlags & 0x100000) || (m_pDamageEntity && m_pDamageEntity->GetIsTypePed())) return;
    } else {
        bool fireDamage = false;
        if (!CanVehicleBeDamaged(damager, weapon, fireDamage)) return;
        if (weapon != static_cast<eWeaponType>(19) && weapon != static_cast<eWeaponType>(20) && weapon != static_cast<eWeaponType>(21) && weapon != static_cast<eWeaponType>(51)) return;
        explosive = true;
        damageIntensity = 0.f;
    }
    const float healthDamage = damageIntensity * m_pHandlingData->m_fCollisionDamageMultiplier;
    const float previousHealth = m_fHealth;
    float factor;
    if (FindPlayerVehicle(-1, false) == this) factor = bytes[0x429] & 0x80 ? StaticRef<float>(0x85F0A0) : .5f;
    else if (bytes[0x429] & 0x80) factor = StaticRef<float>(0x85F0FC);
    else if (m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle(-1, false)) factor = StaticRef<float>(0x862D68);
    else factor = StaticRef<float>(0x858C84);
    m_fHealth = PlaneHealthAfterDamageCpp(m_fHealth, healthDamage, factor); // 0x6CC642..0x6CC6CE: health - healthDamage * factor con un solo fstp
    if (m_fHealth <= 0.f && previousHealth > 0.f) m_fHealth = 1.f;
    if (damageIntensity > damageThreshold || (m_fHealth <= 1.f && previousHealth > 0.f)) {
        BlowUpCar(this, false);
        return;
    }
    if (!(damageIntensity > partThreshold) && !explosive) return;
    if (GetStatus() == STATUS_WRECKED || !(m_fMass > 1000.f)) return;
    const CVector relative = m_matrix->InverseTransformVector(*vecCollisionCoors - GetPosition());
    float closest = 1000.f;
    int32 selected = -1;
    for (int32 i = 12; i < 25; ++i) {
        if (auto* node = m_aCarNodes[i]) {
            const CVector nodePosition{node->modelling.pos};
            if (PlaneSelectCloserCpp(&nodePosition.x, &relative.x, &closest)) selected = i;
        }
    }
    if (selected < 0) return;
    if (selected == 16 && (CGeneral::GetRandomNumber() & 1)) {
        if (m_aCarNodes[17] && (CGeneral::GetRandomNumber() & 1)) selected = 17;
        else if (m_aCarNodes[18]) selected = 18;
    }
    if (m_damageManager.ProgressAeroplaneDamage(uint8(selected))) {
        const auto status = m_damageManager.GetAeroplaneCompStatus(uint8(selected));
        if (status == 1 && selected >= 16 && selected <= 22) {
            auto* panels = reinterpret_cast<CBouncingPanel*>(bytes + 0x6AC);
            for (int32 i = 0; i < 4; ++i) {
                if (panels[i].m_nFrameId == uint16(-1)) {
                    panels[i].SetPanel(int16(selected), 0, std::bit_cast<float>(0xBCA3D70Au));
                    break;
                }
                if (int32(int16(panels[i].m_nFrameId)) == selected) break;
            }
        } else if (m_damageManager.GetAeroplaneCompStatus(uint8(selected)) == 2) {
            SetComponentVisibility(m_aCarNodes[selected], 2);
        }
    }
    // The EXE's explosive path reaches 0x6CC915 without initializing [esp+14h].
    // Collision sets that factor to 1 at 0x6CC57F; explosive smoke is outside
    // the comparison bank's approved scope. Use defined +0 for its particle
    // force rather than reproduce an indeterminate stack read in C++.
    dmgDrawCarCollidingParticles(*vecCollisionCoors, damageIntensity, weapon);
    const CVector smokePosition{m_aCarNodes[selected]->modelling.pos};
    if (m_pSmokeParticle) {
        m_pSmokeParticle->Kill();
        m_pSmokeParticle = nullptr;
    }
    auto* matrix = GetModellingMatrix();
    if (!matrix) return;
    m_pSmokeParticle = g_fxMan.CreateFxSystem("fire_med", smokePosition, matrix, false);
    if (!m_pSmokeParticle) return;
    m_pSmokeParticle->Play();
    m_pSmokeParticle->SetVelAdd(-m_vecMoveSpeed * 5.f);
    m_pSmokeParticle->SetLocalParticles(true);
    m_nSmokeTimer = uint32(CGeneral::GetRandomIntInRange(2000, 4000));
}

// 0x6CAB90
void CPlane::IsAlreadyFlying() {
    m_nStartedFlyingTime = CTimer::GetTimeInMS() - 20000;
}

// 0x6CAC20
void CPlane::SetGearUp() {
    m_fLandingGearStatus = 1.0f;
    m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0f / 2.0f * m_pFlyingHandlingData->m_fGearUpR;
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_LEFT,  WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_LEFT,   WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_RIGHT, WHEEL_STATUS_MISSING);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_RIGHT,  WHEEL_STATUS_MISSING);
}

// 0x6CAC70
void CPlane::SetGearDown() {
    m_fLandingGearStatus = 0.0f;
    m_fAirResistance = m_pHandlingData->m_fDragMult / 1000.0f / 2.0f;
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_LEFT,  WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_LEFT,   WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CAR_WHEEL_FRONT_RIGHT, WHEEL_STATUS_OK);
    m_damageManager.SetWheelStatus(CAR_WHEEL_REAR_RIGHT,  WHEEL_STATUS_OK);
}

// 0x6CCA50
uint32 CPlane::CountPlanesAndHelis() {
    uint32 counter = 0;
    for (auto& vehicle : GetVehiclePool()->GetAllValid()) {
        if (vehicle.IsSubHeli() || vehicle.IsSubPlane()) {
            counter++;
        }
    }
    return counter;
}

// 0x6CCAA0
bool CPlane::AreWeInNoPlaneZone() {
    const auto& camPos = TheCamera.GetPosition();
    // Como el original (0x6CCAA0): la distancia se suma como (dz*dz + dy*dy) + dx*dx (fld/fmul/faddp en 0x6CCAE8..0x6CCAF6), no x*x + y*y + z*z
    const float dx = camPos.x - (-1073.0f), dy = camPos.y - (-675.0f), dz = camPos.z - 50.0f;
    const float distance = std::sqrt((dz * dz + dy * dy) + dx * dx);

    return distance < 200.0f ||
           camPos.x > -2743.0f && camPos.x < -2626.0f && camPos.y > 1300.0f && camPos.y < 2200.0f || // todo: Is point inside
           camPos.x > -1668.0f && camPos.x < -1122.0f && camPos.y > 541.0f && camPos.y < 1118.0f;
}

// 0x6CCBB0
bool CPlane::AreWeInNoBigPlaneZone() {
    // untested
    const auto& camPos = TheCamera.GetPosition();
    return DistanceBetweenPoints2D({ +1522.0f, -1237.0f }, camPos) < 800.0f ||
           DistanceBetweenPoints2D({ -1836.0f, +659.0f }, camPos) < 800.0f;
}

// 0x6CCC50
void CPlane::SwitchAmbientPlanes(bool enable) {
    if (GenPlane_Active && !enable) {
        auto* pool = GetVehiclePool();
        for (int32 i = int32(pool->GetSize()) - 1; i >= 0; --i) {
            auto* vehicle = pool->GetAt(i);
            if (vehicle && (vehicle->m_nVehicleSubType == VEHICLE_TYPE_PLANE || vehicle->m_nVehicleSubType == VEHICLE_TYPE_HELI) && vehicle->m_nCreatedBy == RANDOM_VEHICLE) {
                CWorld::Remove(vehicle);
                delete vehicle;
            }
        }
    }
    GenPlane_Active = enable;
}

// 0x6CD090
void CPlane::FindPlaneCreationCoors(CVector* outCoors, CVector* outTargetCoors, float* outPlaneOrientation, float* outFlightHeight, bool isBigPlane) {
    const auto radius = isBigPlane ? 340.f : 140.f;
    const auto baseHeight = isBigPlane ? 200.f : 25.f;
    CColSphere sphere;
    CColBox box;
    for (int32 offset = 0; offset < 500; offset += 10) {
        const auto angle = float(CGeneral::GetRandomNumber() % 360) * StaticRef<float>(0x8595EC);
        *outCoors = FindPlayerCoors(-1);
        *outFlightHeight = (float(CGeneral::GetRandomNumber() & 15) + float(offset)) + baseHeight;
        outCoors->x = TrigCompoundCosMulAdd(angle, radius, outCoors->x); // 0x6CD15C..0x6CD168: fcos; fmul radius; fadd x; fstp
        outCoors->y = TrigCompoundSinMulAdd(angle, radius, outCoors->y); // 0x6CD16A..0x6CD177: fsin; fmul radius; fadd y; fstp
        outCoors->z += *outFlightHeight;
        *outTargetCoors = FindPlayerCoors(-1);
        const auto ahead = float(CGeneral::GetRandomNumber() & 31) + 20.f;
        const auto& cameraForward = StaticRef<CVector>(0xB6F9AC);
        outTargetCoors->x += cameraForward.x * ahead;
        outTargetCoors->y += cameraForward.y * ahead;
        outTargetCoors->z += cameraForward.z * ahead;
        outTargetCoors->z += *outFlightHeight;
        *outPlaneOrientation = CGeneral::GetATanOfXY(outTargetCoors->x - outCoors->x, outTargetCoors->y - outCoors->y);
        const CVector end{(outTargetCoors->x - outCoors->x) + outTargetCoors->x, (outTargetCoors->y - outCoors->y) + outTargetCoors->y, (outTargetCoors->z - outCoors->z) + outTargetCoors->z};
        CWorld::AdvanceCurrentScanCode();
        sphere.Set(15.f, *outCoors, static_cast<eSurfaceType>(0), 0, tColLighting{0xFF});
        if (CWorld::GetIsLineOfSightClear(*outCoors, end, true, false, false, false, false, false, false)) {
            const auto y = int32(int64(outCoors->y));
            const auto x = int32(int64(outCoors->x));
            if (!CCollision::CheckCameraCollisionBuildings(x, y, box, sphere, sphere, sphere)) break;
        }
    }
    *outFlightHeight += 20.f;
    outTargetCoors->z += 20.f;
}

// 0x6CD2F0
// 0x6C4130 (sin reconstruir en el EXE): right = {sin, -cos, 0}, forward = {cos, sin, 0}, up = {0, 0, 1}; sin y cos salen de fsin/fcos con un solo redondeo a float
// (fst/fstp), -cos es fchs sobre el ST0 sin redondear.
static void PlaneGenerationHeading355(CVehicle* vehicle, float heading) {
    auto* matrix = vehicle->m_matrix;
    const float sine = TrigSimpleSinCpp(heading), cosine = TrigSimpleCosCpp(heading);
    matrix->GetRight().z = 0.f;
    matrix->GetForward().z = 0.f;
    matrix->GetUp().x = 0.f;
    matrix->GetUp().y = 0.f;
    matrix->GetUp().z = 1.f;
    matrix->GetRight().x = sine;
    matrix->GetRight().y = -cosine;
    matrix->GetForward().x = cosine;
    matrix->GetForward().y = sine;
}
// 0x6CD413..0x6CD45B (helicoptero) y 0x6CD722..0x6CD772 (avion): x = cos * speed, y = sin * speed (fsin/fcos sin redondear, un redondeo por fmul + fstp), z = 0; ambas ramas dan lo mismo.
static CVector PlaneGenerationVelocity355(float heading, bool helicopter) {
    (void)helicopter;
    const float speed = StaticRef<float>(0x862D68);
    return CVector{TrigSimpleCosMulCpp(heading, speed), TrigSimpleSinMulCpp(heading, speed), 0.f};
}
static void PlaneGenerationSelect355(int32 aircraftCount){
 auto& state=CPlane::GenPlane_Status;auto& model=CPlane::GenPlane_ModelIndex;auto& previous=CPlane::GenPlane_LastTimeGenerated;
 if(StaticRef<int32>(0xB72914)||aircraftCount>4||CPlane::AreWeInNoPlaneZone()||StaticRef<bool>(0xB5F852)){
  previous=CTimer::GetTimeInMS();return;
 }
 bool policeAllowed=static_cast<int32>(FindPlayerPed(-1)->GetWantedLevel())>=4;
 if(policeAllowed)policeAllowed=FindPlayerVehicle(-1,false)!=nullptr;
 if(policeAllowed){
  auto* vehicle=FindPlayerVehicle(-1,false);
  if(*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(vehicle)+0x594)!=4){vehicle=FindPlayerVehicle(-1,false);policeAllowed=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(vehicle)+0x594)==3;}
 }
 if(policeAllowed)policeAllowed=FindPlayerVehicle(-1,false)->m_nModelIndex!=577;
 if(policeAllowed)policeAllowed=FindPlayerVehicle(-1,false)->m_nModelIndex!=592;
 if(policeAllowed)policeAllowed=CTimer::GetTimeInMS()>uint32(previous+60000u);
 if(policeAllowed){
  auto* pool=StaticRef<void*>(0xB74494);int32 remaining=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(pool)+8),hydras=0;
  while(remaining){--remaining;auto* vehicle=reinterpret_cast<CVehiclePool*>(pool)->GetAt(static_cast<size_t>(remaining));if(vehicle&&reinterpret_cast<uint8*>(vehicle)[0x3BA]==59)++hydras;}
  bool spawn=hydras<1;
  if(!spawn&&hydras<2)spawn=static_cast<int32>(FindPlayerPed(-1)->GetWantedLevel())>4;
  if(spawn){model=520;state=1;}
 }
 if(aircraftCount>0||state)return;
 const auto appearance=[](){auto* vehicle=FindPlayerVehicle(-1,false);return vehicle->GetVehicleAppearance();};
 if((StaticRef<int16>(0xC81314)==0||StaticRef<int16>(0xC81314)==4)&&StaticRef<int16>(0xC81320)!=19&&StaticRef<int16>(0xC8131C)!=19){
  bool allowed=FindPlayerVehicle(-1,false)==nullptr;
  if(!allowed){allowed=int32(appearance())!=3;if(allowed)allowed=int32(appearance())!=5;}
  if(allowed&&uint32(previous+120000u)<CTimer::GetTimeInMS()){
   const auto random=CGeneral::GetRandomNumber()&7;
   model=random==3||random==4?512:random==5||random==6?511:476;state=1;return;
  }
  if(state)return;
 }
 if(StaticRef<bool>(0xC0BC5C)){
  if(!CTheScripts::IsPlayerOnAMission()&&uint32(previous+350000u)<CTimer::GetTimeInMS()){model=497;state=1;return;}
  if(state)return;
 }
 if(CGameLogic::LaRiotsActiveHere()){state=1;model=497;return;}
 if(!state&&StaticRef<int16>(0xC81314)!=0&&StaticRef<int16>(0xC81314)!=4&&!StaticRef<bool>(0xC0BC5C)&&StaticRef<int16>(0xC81320)!=19&&StaticRef<int16>(0xC8131C)!=19&&uint32(previous+200000u)<CTimer::GetTimeInMS()&&!CPlane::AreWeInNoBigPlaneZone()){
  bool allowed=FindPlayerVehicle(-1,false)==nullptr;
  if(!allowed){allowed=int32(appearance())!=3;if(allowed)allowed=int32(appearance())!=5;}
  if(allowed){model=(CGeneral::GetRandomNumber()&1)?592:577;state=1;}
 }
}


static void PlaneGenerationEngine355(CVehicle* vehicle){auto* bytes=reinterpret_cast<uint8*>(vehicle);if(!(bytes[0x42D]&2))bytes[0x428]|=0x10;}
void CPlane::DoPlaneGenerationAndRemoval(){
 if((CTimer::GetFrameCounter()&31)!=30||!GenPlane_Active)return;
 const int32 aircraftCount=static_cast<int32>(CountPlanesAndHelis());
 if(!GenPlane_Status){PlaneGenerationSelect355(aircraftCount);return;}
 if(GenPlane_Status!=1)return;
 if(StaticRef<uint8>(0x8E4CD0+uintptr_t(GenPlane_ModelIndex)*0x14)!=1){CStreaming::RequestModel(GenPlane_ModelIndex,8);return;}
 CVector coordinates,target;float heading,height;
 if(GenPlane_ModelIndex==497){
  FindPlaneCreationCoors(&coordinates,&target,&heading,&height,false);
  auto* storage=plugin::CallAndReturn<void*,0x6E2D50>(0xA18);
  auto* vehicle=storage?reinterpret_cast<CVehicle*(__thiscall*)(void*,int32,int32)>(0x6C4190)(storage,GenPlane_ModelIndex,1):nullptr;
  vehicle->SetPosn(coordinates);PlaneGenerationHeading355(vehicle,heading);CCarCtrl::JoinCarWithRoadSystem(vehicle);
  auto* bytes=reinterpret_cast<uint8*>(vehicle);*reinterpret_cast<float*>(bytes+0x84C)=std::bit_cast<float>(0x3E6147AEu);vehicle->SetStatus(STATUS_PHYSICS);CWorld::Add(vehicle);
  *reinterpret_cast<float*>(bytes+0x9B0)=height;bytes[0x3BA]=38;*reinterpret_cast<float*>(bytes+0x9B4)=heading;*reinterpret_cast<float*>(bytes+0x9A8)=height;
  vehicle->SetVelocity(PlaneGenerationVelocity355(heading,true));bytes[0x42C]|=0x40;*reinterpret_cast<uint16*>(bytes+0x4A6)=255;bytes[0x3D0]=50;PlaneGenerationEngine355(vehicle);
  const auto random=CGeneral::GetRandomNumber()%2;
  if(random==1){
   const CVector* planePosition=&vehicle->GetPosition();const CVector playerPosition=FindPlayerCoors(-1);auto* ped=CWorld::FindUnsuspectingTargetPed(*planePosition,playerPosition);
   if(ped){
    bytes[0x3BA]=39;auto** reference=reinterpret_cast<CEntity**>(bytes+0x41C);*reference=ped;ped->RegisterReference(reference);bytes[0x988]|=2;bytes[0x3DB]|=0xA0;
    auto* group=CPedGroups::GetPedsGroup(ped);
    if(group&&reinterpret_cast<uint8*>(group)[0x2D0]!=1){CEventDanger event{vehicle,200.f};event.m_TaskId=static_cast<eTaskType>(1505);group->GetIntelligence().AddEvent(&event);}
    else if(!ped->IsPlayer()&&reinterpret_cast<uint8*>(ped)[0x484]!=2&&*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(ped)+0x598)!=6){CEventDanger event{vehicle,200.f};event.m_TaskId=static_cast<eTaskType>(911);ped->GetEventGroup().Add(&event,false);}
   }
  }else if(random==0){
   const CVector* planePosition=&vehicle->GetPosition();const CVector playerPosition=FindPlayerCoors(-1);auto* targetCar=CWorld::FindUnsuspectingTargetCar(*planePosition,playerPosition);
   if(targetCar){auto** reference=reinterpret_cast<CEntity**>(bytes+0x41C);*reference=targetCar;bytes[0x3BA]=39;targetCar->RegisterReference(reference);bytes[0x988]|=2;bytes[0x3DB]|=0xA0;}
  }
 }else{
  const bool large=GenPlane_ModelIndex==577||GenPlane_ModelIndex==592;
  FindPlaneCreationCoors(&coordinates,&target,&heading,&height,large);
  auto* storage=plugin::CallAndReturn<void*,0x6E2D50>(0xA04);
  auto* plane=storage?reinterpret_cast<CPlane*(__thiscall*)(void*,int32,int32)>(0x6C8E20)(storage,GenPlane_ModelIndex,1):nullptr;
  plane->SetPosn(coordinates);plane->m_planeCreationHeading=heading;PlaneGenerationHeading355(plane,heading);CCarCtrl::JoinCarWithRoadSystem(plane);
  plane->SetStatus(STATUS_PHYSICS);CWorld::Add(plane);auto* bytes=reinterpret_cast<uint8*>(plane);
  plane->m_planeHeading=heading;plane->m_minAltitude=height;bytes[0x3BA]=36;plane->m_planeHeadingPrev=heading;plane->m_maxAltitude=height;
  plane->SetVelocity(PlaneGenerationVelocity355(heading,false));bytes[0x42C]|=0x40;
  if(!large&&GenPlane_ModelIndex!=520)*reinterpret_cast<uint16*>(bytes+0x4A6)=150;
  else{*reinterpret_cast<uint16*>(bytes+0x4A6)=360;if(large)plane->m_nFlags&=0xFFFFFFFEu;}
  if(GenPlane_ModelIndex==520){bytes[0x3BA]=59;*reinterpret_cast<uint16*>(bytes+0x86C)=0;}
  plane->SetGearUp();PlaneGenerationEngine355(plane);CStreaming::SetModelIsDeletable(GenPlane_ModelIndex);CStreaming::SetModelTxdIsDeletable(GenPlane_ModelIndex);
 }
 GenPlane_LastTimeGenerated=CTimer::GetTimeInMS();GenPlane_Status=0;
}

// 0x6C9140
bool CPlane::SetUpWheelColModel(CColModel* wheelCol) {
    return false;
}

// 0x6C9150
bool CPlane::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C94A0
// Transcripcion del EXE a C++ (C++ puro; el CMatrix temporal es un objeto local y su destruccion la hace el compilador). Las cadenas x87 con redondeos propios o ST0 sin redondear
// van a ayudantes de X87Math (Plane...Cpp); los colaboradores reconstruidos se llaman directamente y los que siguen en el EXE (cuaternion -> matriz 0x59C080 y
// matriz * vector 0x59C890) por su direccion.
static const uint8 PlanePreRenderGearSwitchMap[] = {0,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,2,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4};
static_assert(sizeof(PlanePreRenderGearSwitchMap) == 0x75);

// 0x59C080: CQuaternion::Get(RwMatrix*) (sin reconstruir en el EXE): escribe solo right/up/at del RwMatrix indicado.
static void PlaneQuaternionToMatrix(const CQuaternion& quaternion, RwMatrix* matrix) {
    reinterpret_cast<void(__thiscall*)(const CQuaternion*, RwMatrix*)>(0x59C080)(&quaternion, matrix);
}

// 0x59C890: CMatrix * CVector (sin reconstruir en el EXE, orden de sumas propio).
static CVector PlaneTransformPoint(const CMatrix* matrix, const CVector& point) {
    CVector result;
    reinterpret_cast<CVector*(__cdecl*)(CVector*, const CMatrix*, const CVector*)>(0x59C890)(&result, matrix, &point);
    return result;
}

void CPlane::PreRender() {
    auto* const bytes = reinterpret_cast<uint8*>(this);
    auto* const nodes = reinterpret_cast<RwFrame**>(bytes + 0x648); // CAutomobile::m_aCarNodes
    const auto floatAt = [bytes](size_t offset) -> float& { return *reinterpret_cast<float*>(bytes + offset); };
    const auto frameMatrix = [](RwFrame* frame) { return reinterpret_cast<RwMatrix*>(reinterpret_cast<uint8*>(frame) + 0x10); }; // &frame->modelling sin leer el marco
    const int32 model = int32(int16(m_nModelIndex));
    auto* const mi = static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(model));

    CMatrix workMatrix; // CMatrix en pila del EXE ([esp+70h]): m_pAttachMatrix y m_bOwnsAttachedMatrix a cero desde el prologo
    CQuaternion quaternion;

    CColModel* const colModel = GetColModel(); // 0x6C94EB
    CVehicle::PreRender();                     // 0x6C94F6 (0x6D6480: CVehicle::PreRender, no CAutomobile::PreRender)
    if (model == 539) {                        // 0x21B Vortex
        DoHoverSuspensionRatios();             // slot 69 (0x114)
    }

    // Ruedas: alturas suavizadas (0x6C950D..0x6C9655)
    if (vehicleFlags.bVehicleColProcessed) {
        DoBurstAndSoftGroundRatios(); // slot 52 (0xD0)
        for (int32 wheel = 0; wheel < 4; ++wheel) {
            const float compression = PlaneWheelCompressionCpp(m_aSuspensionSpringLength[wheel], m_aSuspensionLineLength[wheel], m_fWheelsSuspensionCompression[wheel]);
            CVector wheelPosition;
            mi->GetWheelPosn(wheel, wheelPosition, true);
            const bool snap = (m_nPhysicalFlags & 0x4000000) && (*reinterpret_cast<uint32*>(bytes + 0x38C) & 0x20000);
            m_wheelPosition[wheel] = PlaneWheelHeightCpp(wheelPosition.z, *reinterpret_cast<float*>(reinterpret_cast<uint8*>(m_pHandlingData) + 0xB8), compression,
                                                         m_aSuspensionSpringLength[wheel], m_wheelPosition[wheel], snap);
            if (model == 592) { // 0x250 Andromada
                m_wheelRotation[wheel] = PlaneAndromadaWheelRotationCpp(m_wheelRotation[wheel], m_fWheelsSuspensionCompression[wheel],
                                                                        reinterpret_cast<const float*>(bytes + 0x734 + wheel * 0x2C), &m_matrix->GetForward().x);
            }
        }
    }

    // Matrices de los nodos de rueda (0x6C966B..0x6C96A0)
    UpdateWheelMatrix(4, 1);
    UpdateWheelMatrix(7, 1);
    UpdateWheelMatrix(2, 1);
    UpdateWheelMatrix(5, model == 520 ? 3 : 1);

    // Angulo de las helices (0x6C96A5..0x6C96E3)
    field_9C8 = PlanePropAngleCpp(CTimer::GetTimeStep(), m_fPropSpeed, field_9C8, StaticRef<float>(0x858CBC));

    // Superficies de control (0x6C96E9..0x6C9C94): solo estados PLAYER..PHYSICS ((byte & 0xF8) <= 0x18)
    if ((bytes[0x36] & 0xF8) <= 0x18) {
        auto* const structure = mi->m_pVehicleStruct;
        CVector axis{};
        float finScale = 1.f, finOffset = 0.f; // [esp+18h], [esp+14h]
        const auto attach = [&](RwFrame* frame) {
            workMatrix.Attach(frameMatrix(frame), false);
            workMatrix.UpdateRW();
        };
        // Eje: posicion de la marca del modelo (copia por dword); si es (0, 0, 0) un eje unitario (z para el timon, x para el resto); si no, (marca - posicion del nodo) normalizado.
        const auto prepareAxis = [&](const CVector& dummy, bool unitZ) {
            axis = dummy;
            if (axis.x == 0.f && axis.y == 0.f && axis.z == 0.f) {
                if (unitZ) {
                    axis.z = 1.f;
                } else {
                    axis.x = 1.f;
                }
            } else {
                PlaneSubtractPositionCpp(&axis.x, &workMatrix.GetPosition().x);
                axis.Normalise();
            }
        };
        const auto rotateNode = [&](float angle) {
            quaternion.Set(&axis, angle);
            PlaneQuaternionToMatrix(quaternion, workMatrix.m_pAttachMatrix);
        };
        // Escala / traslacion de la aleta del tren (0x6C9995..0x6C9A21 y 0x6C9AB..0x6C9B28)
        const auto adjustFin = [&](int32 component, float expectedAxisX) {
            if (!(finScale > 1.f)) {
                return;
            }
            workMatrix.Update();
            if (axis.x == expectedAxisX) {
                workMatrix.GetForward().y = X87TimesFloatCpp(workMatrix.GetForward().y, finScale);
            } else {
                CVector original;
                if (mi->GetOriginalCompPosition(original, component)) {
                    original.y = PlaneFinOffsetCpp(finScale, *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(colModel) + 0x10), original.y, StaticRef<float>(0x871928));
                    workMatrix.SetTranslateOnly(original);
                }
            }
            workMatrix.UpdateRW();
        };

        if (RwFrame* const rudder = nodes[16]) { // [esi+688h]
            attach(rudder);
            prepareAxis(structure->m_avDummyPos[11], true); // marca +0x84
            rotateNode(PlaneSurfaceAngleCpp(StaticRef<float>(0x871914), StaticRef<float>(0x8595EC), m_fLeftRightSkid, 0.f, 0));
            if (model == 539) {
                if (RwFrame* const second = nodes[24]) { // [esi+6A8h]
                    attach(second);
                    PlaneQuaternionToMatrix(quaternion, workMatrix.m_pAttachMatrix);
                }
            }
        }

        if (RwFrame* const aileron = nodes[20]) { // [esi+698h]
            const float gear = m_fLandingGearStatus;
            if (m_pFlyingHandlingData->m_fGearDownL > 1.f && std::fabs(gear) < 1.f && (nodes[21] || nodes[22])) {
                PlaneGearFactorsCpp(gear, StaticRef<float>(0x871920), StaticRef<float>(0x871924), &finScale, &finOffset);
            }
            attach(aileron);
            prepareAxis(structure->m_avDummyPos[9], false); // marca +0x6C
            rotateNode(PlaneSurfaceAngleCpp(StaticRef<float>(0x87191C), StaticRef<float>(0x8595EC), m_fSteeringLeftRight, finOffset, 1));
            adjustFin(20, 1.f);

            if (RwFrame* const second = nodes[19]) { // [esi+694h]
                attach(second);
                axis.x = X87TimesFloatCpp(axis.x, -1.f); // fmul [0x858C1C] (no fchs)
                rotateNode(PlaneSurfaceAngleCpp(StaticRef<float>(0x87191C), StaticRef<float>(0x8595EC), m_fSteeringLeftRight, finOffset, 2));
                adjustFin(19, -1.f);
            }
        }

        if (RwFrame* const elevator = nodes[18]) { // [esi+690h]
            attach(elevator);
            prepareAxis(structure->m_avDummyPos[10], false); // marca +0x78
            rotateNode(PlaneSurfaceAngleCpp(StaticRef<float>(0x871918), StaticRef<float>(0x8595EC), m_fSteeringUpDown, 0.f, 3));
            if (RwFrame* const second = nodes[17]) { // [esi+68Ch]
                attach(second);
                const float angle = PlaneSurfaceAngleCpp(StaticRef<float>(0x871918), StaticRef<float>(0x8595EC), m_fSteeringUpDown, 0.f, 0);
                axis.x = X87TimesFloatCpp(axis.x, -1.f);
                rotateNode(angle);
            }
        }
    }

    // Helices (0x6C9C94..0x6C9D7E): nodos 12/13 con sentido +1, 14/15 con sentido -1
    for (int32 pair = 0; pair < 2; ++pair) {
        const float direction = pair == 0 ? 1.f : -1.f;
        RwFrame* const staticProp = nodes[pair == 0 ? 12 : 14];
        RwFrame* const movingProp = nodes[pair == 0 ? 13 : 15];
        if (staticProp) {
            SetComponentRotation(staticProp, AXIS_Y, PlanePropRotationCpp(direction, field_9C8, true), true);
            RpAtomic* atomic = nullptr;
            RwFrameForAllObjects(nodes[pair == 0 ? 12 : 14], GetCurrentAtomicObjectCB, &atomic);
            if (atomic) {
                SetComponentAtomicAlpha(atomic, 255);
            }
        }
        if (movingProp) {
            SetComponentRotation(movingProp, AXIS_Y, PlanePropRotationCpp(direction, field_9C8, false), true);
            RpAtomic* atomic = nullptr;
            RwFrameForAllObjects(nodes[pair == 0 ? 13 : 15], GetCurrentAtomicObjectCB, &atomic);
            if (atomic) {
                SetComponentAtomicAlpha(atomic, 0);
            }
        }
    }

    // Tren de aterrizaje / flaps por modelo (0x6C9D7E..0x6C9EE3)
    {
        const uint32 modelOffset = uint32(model - 476);
        if (modelOffset <= 0x74 && PlanePreRenderGearSwitchMap[modelOffset] != 5) {
            int32 firstAxis = 0, secondAxis = 0; // edi, ebp (-1 = sin segunda pareja)
            float firstValue = 0.f, secondValue = 0.f, thirdValue = 0.f, fourthValue = 0.f; // ST0, [esp+18h], [esp+14h], [esp+1Ch]
            switch (PlanePreRenderGearSwitchMap[modelOffset]) {
            case 0: // Rustler
                firstAxis = 1; secondAxis = -1;
                firstValue = -StaticRef<float>(0x87192C); secondValue = StaticRef<float>(0x87192C);
                break;
            case 1: // Shamal, AT400
                firstAxis = 1; secondAxis = 0;
                firstValue = -StaticRef<float>(0x87192C); secondValue = StaticRef<float>(0x87192C);
                thirdValue = StaticRef<float>(0x871938);
                break;
            case 2: // Hydra
                firstAxis = 0; secondAxis = 0;
                firstValue = StaticRef<float>(0x871930); secondValue = StaticRef<float>(0x871930);
                thirdValue = StaticRef<float>(0x871934); fourthValue = StaticRef<float>(0x871938);
                break;
            case 3: // Nevada
                firstAxis = 0; secondAxis = -1;
                firstValue = StaticRef<float>(0x871940); secondValue = StaticRef<float>(0x871940);
                break;
            default: // 4: Andromada
                firstAxis = 0; secondAxis = 0;
                firstValue = StaticRef<float>(0x87193C); secondValue = StaticRef<float>(0x87193C);
                thirdValue = -StaticRef<float>(0x87193C);
                break;
            }
            SetComponentRotation(nodes[21], eRotationAxis(firstAxis), PlaneGearRotationCpp(m_fLandingGearStatus, firstValue), true);
            SetComponentRotation(nodes[22], eRotationAxis(firstAxis), PlaneGearRotationCpp(m_fLandingGearStatus, secondValue), true);
            if (secondAxis > -1) {
                SetComponentRotation(nodes[23], eRotationAxis(secondAxis), PlaneGearRotationCpp(m_fLandingGearStatus, thirdValue), true);
                if (double(fourthValue) > StaticRef<double>(0x859EF8)) {
                    SetComponentRotation(nodes[24], eRotationAxis(secondAxis), PlaneGearRotationCpp(m_fLandingGearStatus, fourthValue), true);
                }
            }
        }
    }

    // Andromada / Hydra / Vortex (0x6C9EE3..0x6CA638)
    if (model == 592) {
        SetComponentRotation(nodes[24], AXIS_X, IntTimesFloatCpp(int32(m_wMiscComponentAngle), ANDROM_COL_ANGLE_MULT), true);
    } else if (model == 520) {
        const float nozzleAngle = PlaneNozzleAngleCpp(int32(m_wMiscComponentAngle), StaticRef<float>(0x858FE4), int32(int16(HARRIER_NOZZLE_ROTATE_LIMIT)));
        SetComponentRotation(nodes[6], AXIS_X, nozzleAngle, true);  // [esi+660h]
        SetComponentRotation(nodes[3], AXIS_X, nozzleAngle, true);  // [esi+654h]
        auto** const jets = reinterpret_cast<FxSystem_c**>(bytes + 0x9E8); // m_apJettrusParticles
        auto*& overheat = *reinterpret_cast<FxSystem_c**>(bytes + 0x580);
        if (vehicleFlags.bEngineOn) {
            const float dustTime = PlaneDustTimeCpp(m_fAccelerationBreakStatus, StaticRef<float>(0x858624), StaticRef<float>(0x858B8C));
            if (int32(m_wMiscComponentAngle) >= int32(int16(HARRIER_NOZZLE_SWITCH_LIMIT))) {
                DoHeliDustEffect(dustTime, 2.f);
            } else if (overheat) {
                overheat->Kill();
                overheat = nullptr;
                floatAt(0x984) = 0.f;
            }
            RwMatrix* const rearMatrix = frameMatrix(nodes[6]);  // ebx
            RwMatrix* const frontMatrix = frameMatrix(nodes[3]); // [esp+1Ch]
            for (int32 i = 0; i < 4; ++i) {
                if (GetRwObject()) {
                    RwMatrix* const parentMatrix = frameMatrix(static_cast<RwFrame*>(GetRwObject()->parent));
                    if (parentMatrix && !jets[i]) {
                        const CVector origin{0.f, 0.f, 0.f};
                        jets[i] = g_fxMan.CreateFxSystem("jetthrust", origin, parentMatrix, false);
                        if (jets[i]) {
                            jets[i]->Play();
                            jets[i]->SetLocalParticles(true);
                            jets[i]->CopyParentMatrix();
                        }
                    }
                }
            }
            struct JetOffset { uint32 x, y, z; };
            static constexpr JetOffset jetOffsets[4] = {{0x3F333333u, 0xBEE66666u, 0x3D4CCCCDu}, {0xBF51EB85u, 0xBEE66666u, 0x3D4CCCCDu}, {0x3F2147AEu, 0xBEE66666u, 0x3D8F5C29u}, {0xBF400000u, 0xBEE66666u, 0x3D8F5C29u}};
            for (int32 i = 0; i < 4; ++i) {
                if (!jets[i]) {
                    continue;
                }
                const CVector offset{std::bit_cast<float>(jetOffsets[i].x), std::bit_cast<float>(jetOffsets[i].y), std::bit_cast<float>(jetOffsets[i].z)};
                CMatrix product = *m_matrix * *reinterpret_cast<const CMatrix*>(i < 2 ? rearMatrix : frontMatrix);
                jets[i]->SetMatrix(reinterpret_cast<RwMatrix*>(&product));
                jets[i]->SetOffsetPos(offset);
                jets[i]->SetConstTime(true, dustTime);
            }
        } else {
            for (int32 i = 0; i < 4; ++i) {
                if (jets[i]) {
                    jets[i]->Kill();
                    jets[i] = nullptr;
                }
            }
            if (overheat) {
                overheat->Kill();
                overheat = nullptr;
                floatAt(0x984) = 0.f;
            }
        }
    } else if (model == 539) { // Vortex
        if (nodes[23]) { // [esi+6A4h]
            CColModel* const hoverColModel = GetColModel();
            const CCollisionData* const colData = hoverColModel->m_pColData; // [eax+2Ch]
            CVector firstWheel;
            mi->GetWheelPosn(0, firstWheel, false);
            workMatrix.Attach(frameMatrix(nodes[23]), false);
            PlaneHoverTiltCpp(&workMatrix.GetRight().z, &workMatrix.GetForward().z, &workMatrix.GetUp().x, &workMatrix.GetUp().y, &workMatrix.GetUp().z,
                              &m_matrix->GetRight().x, &m_matrix->GetForward().x, &m_vecMoveSpeed.x, m_wheelPosition.data(), reinterpret_cast<const float*>(colData->m_pLines),
                              firstWheel.z, mi->m_fWheelSizeFront, CTimer::GetTimeStep());
            workMatrix.UpdateRW();
        }
    }

    // Paneles rebotantes (0x6CA638..0x6CA6A7) y velocidades acumuladas (0x6CA6A9..0x6CA71E)
    {
        auto* const panels = reinterpret_cast<CBouncingPanel*>(bytes + 0x6AC);
        auto& moveSum = *reinterpret_cast<CVector*>(bytes + 0x8B4);
        auto& turnSum = *reinterpret_cast<CVector*>(bytes + 0x8C0);
        for (int32 i = 0; i < 3; ++i) {
            if (int16(panels[i].m_nFrameId) > -1) {
                panels[i].ProcessPanel(this, nodes[int16(panels[i].m_nFrameId)], moveSum, turnSum, StaticRef<float>(0x8D33FC), StaticRef<float>(0x8D3400));
            }
        }
        PlaneSumVector3Cpp(reinterpret_cast<const float*>(bytes + 0x44), reinterpret_cast<const float*>(bytes + 0x5C), &moveSum.x);
        PlaneSumVector3Cpp(reinterpret_cast<const float*>(bytes + 0x50), reinterpret_cast<const float*>(bytes + 0x68), &turnSum.x);
    }

    // Sombra (0x6CA721..0x6CA73A)
    CShadows::StoreShadowForVehicle(this, model == 539 ? VEH_SHD_CAR : (model == 577 || model == 592 ? VEH_SHD_BIG_PLANE : VEH_SHD_PLANE));

    // Estelas de las alas (0x6CA73F..0x6CA937)
    if (!vehicleFlags.bIsDrowning && model != 539 && model != 464) {
        const unsigned int alphaBits = PlaneStreakAlphaCpp(&m_vecMoveSpeed.x, &m_matrix->GetUp().x, m_pFlyingHandlingData->m_fAttackLift, StaticRef<float>(0x859AEC), StaticRef<float>(0x85950C));
        const CVector wingTip = mi->m_pVehicleStruct->m_avDummyPos[12]; // marca +0x90 (V)
        CVector start = wingTip;
        CVector end{RpNanSubCpp(wingTip.x, StaticRef<float>(0x858B1C)), wingTip.y, wingTip.z}; // W
        const CVector left = PlaneTransformPoint(m_matrix, start);
        const CVector right = PlaneTransformPoint(m_matrix, end);
        CMotionBlurStreaks::RegisterStreak(static_cast<uint32>(reinterpret_cast<uintptr_t>(this)), 255, 255, 255, uint8(alphaBits), left, right);
        start.x = PlaneNegateCpp(start.x);
        end.x = PlaneNegateCpp(end.x);
        const CVector mirroredLeft = PlaneTransformPoint(m_matrix, start);
        const CVector mirroredRight = PlaneTransformPoint(m_matrix, end);
        CMotionBlurStreaks::RegisterStreak(static_cast<uint32>(reinterpret_cast<uintptr_t>(this)) + 1, 255, 255, 255, uint8(alphaBits), mirroredLeft, mirroredRight);
    }

    // Eyector de humo del Cropduster / Stunt (0x6CA937..0x6CAA93)
    if (m_bSmokeEjectorEnabled && (model == 512 || model == 513)) {
        const bool cropduster = model == 512;
        const FxPrtMult_c multipliers = cropduster
            ? FxPrtMult_c{1.f, 1.f, 1.f, std::bit_cast<float>(0x3ECCCCCDu), 1.f, 1.f, std::bit_cast<float>(0x3E4CCCCDu)}
            : FxPrtMult_c{1.f, 0.f, 0.f, std::bit_cast<float>(0x3ECCCCCDu), 1.f, 1.f, std::bit_cast<float>(0x3E99999Au)};
        const CVector local = cropduster ? CVector{0.f, std::bit_cast<float>(0xBF000000u), std::bit_cast<float>(0xBF000000u)} : CVector{0.f, std::bit_cast<float>(0xC0A00000u), 0.f};
        const CVector position = PlaneTransformPoint(m_matrix, local);
        CVector velocity;
        PlaneSmokeVelocityCpp(&m_vecMoveSpeed.x, StaticRef<float>(0x85862C), StaticRef<float>(0x858B3C), cropduster, &velocity.x);
        g_fx.m_SmokeHuge->AddParticle(position, velocity, 0.f, multipliers, std::bit_cast<float>(0xBF800000u), std::bit_cast<float>(0x3F99999Au), std::bit_cast<float>(0x3F19999Au), false);
    }

    // Salpicaduras del Skimmer (0x6CAA93..0x6CAAB1)
    if (model == 460 && (m_nPhysicalFlags & 0x100)) {
        DoBoatSplashes(floatAt(0x950));
    }
}

// 0x6CAB70
void CPlane::Render() {
    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000;
    CVehicle::Render();
}

// 0x6C9260
void CPlane::ProcessControl() {
    if (GetStatus() == STATUS_PLAYER && (m_nModelIndex == MODEL_CROPDUST || m_nModelIndex == MODEL_STUNT)) {
        const auto player = m_pDriver && m_pDriver->m_nPedType == PED_TYPE_PLAYER2 ? 1 : 0;
        if (CPad::GetPad(player)->IsRightShockPressed()) {
            m_bSmokeEjectorEnabled = !m_bSmokeEjectorEnabled;
        }
    }
    if (m_bSmokeEjectorEnabled && (!vehicleFlags.bEngineOn || vehicleFlags.bIsDrowning || !m_pDriver)) {
        m_bSmokeEjectorEnabled = false;
    }
    if (m_nModelIndex == MODEL_SKIMMER) {
        for (int32 i = 0; i < 4; ++i) {
            m_damageManager.SetWheelStatus(static_cast<eCarWheel>(i), WHEEL_STATUS_MISSING);
        }
    }
    CAutomobile::ProcessControl();
    m_vehicleAudio.m_DoCountStalls = int16(field_9A0);
    if (field_9A0 != 0) {
        field_9A0 = 0;
    }
    CVehicle::ProcessWeapons();
    if (m_nModelIndex == MODEL_VORTEX) {
        std::ranges::fill(m_WheelStates, WHEEL_STATE_NORMAL);
    }
    if (m_pSmokeParticle) {
        const auto decrement = CTimer::GetTimeStep() * StaticRef<float>(0x858B38) * StaticRef<float>(0x859948);
        const auto integer = int64(decrement); // MSVC x86 _ftol2: retain low EAX and x87 conversion state.
        m_nSmokeTimer += uint32(integer);
        RwMatrix matrix;
        m_pSmokeParticle->GetCompositeMatrix(&matrix);
        const auto velocity = -m_vecMoveSpeed * 5.f;
        const FxPrtMult_c mults{0.f, 0.f, 0.f, .2f, 1.f, 1.f, .1f};
        g_fx.m_SmokeHuge->AddParticle(CVector{matrix.pos}, velocity, 0.f, mults);
        g_fx.m_SmokeHuge->AddParticle(CVector{matrix.pos}, velocity, .05f, mults);
        if (std::bit_cast<int32>(m_nSmokeTimer) <= 0 || vehicleFlags.bIsDrowning) {
            m_pSmokeParticle->Kill();
            m_pSmokeParticle = nullptr;
        }
    }
}
// 0x6CADD0
void CPlane::ProcessControlInputs(uint8 playerNum){
 auto* bytes=reinterpret_cast<uint8*>(this);
 const float forwardSpeed=PlaneForwardSpeedCpp(&m_vecMoveSpeed.x,&m_matrix->GetForward().x);
 auto& mode=StaticRef<int32>(0xC1CC04);auto& mouseX=StaticRef<float>(0xB73424);auto& mouseY=StaticRef<float>(0xB73428);
 bool mouse=false,keyboard=true;
 if(StaticRef<bool>(0xB6EC2E)&&StaticRef<bool>(0xC1CC03)){
  (void)CPad::GetPad(playerNum);
  if(mouseX!=0.f)mouse=true;
  else{
   (void)CPad::GetPad(playerNum);
   if(mouseY!=0.f)mouse=true;
   else if((std::abs(m_fSteeringLeftRight)>0.f||std::abs(m_fSteeringUpDown)>0.f)&&mode==2){
    if(CPad::GetPad(playerNum)->GetSteeringLeftRight()==0&&CPad::GetPad(playerNum)->GetSteeringUpDown()==0)mouse=true;
   }
  }
  if(!mouse)keyboard=CPad::GetPad(playerNum)->GetSteeringLeftRight()!=0||CPad::GetPad(playerNum)->GetSteeringUpDown()!=0||mode!=2;
 }
 if(mouse){
  mode=2;
  if(!CPad::GetPad(playerNum)->NewState.m_bVehicleMouseLook){
   (void)(void)CPad::GetPad(playerNum);m_fSteeringLeftRight=RpNanMulAddCpp(mouseX,StaticRef<float>(0x871908),m_fSteeringLeftRight);
   (void)(void)CPad::GetPad(playerNum);m_fSteeringUpDown=RpNanMulAddCpp(mouseY,StaticRef<float>(0x871908),m_fSteeringUpDown);
  }
  if(std::abs(m_fSteeringLeftRight)<StaticRef<float>(0x87190C))PowScaleFloatCpp(&m_fSteeringLeftRight,StaticRef<float>(0x871910),CTimer::GetTimeStep());
  if(std::abs(m_fSteeringUpDown)<StaticRef<float>(0x87190C))PowScaleFloatCpp(&m_fSteeringUpDown,StaticRef<float>(0x871910),CTimer::GetTimeStep());
 }else if(keyboard){
  mode=0;float step=CTimer::GetTimeStep();const int32 lr=CPad::GetPad(playerNum)->GetSteeringLeftRight();
  m_fSteeringLeftRight=PlaneControlBlendCpp(m_fSteeringLeftRight,lr,StaticRef<float>(0x858B88),step,StaticRef<float>(0x858CC4));
  step=CTimer::GetTimeStep();const int32 ud=CPad::GetPad(playerNum)->GetSteeringUpDown();
  m_fSteeringUpDown=PlaneControlBlendCpp(m_fSteeringUpDown,ud,StaticRef<float>(0x858B88),step,StaticRef<float>(0x858CC4));
 }
 auto clamp=[](float value){return value>-1.f?(value<1.f?value:1.f):-1.f;};
 m_fSteeringLeftRight=clamp(m_fSteeringLeftRight);m_fSteeringUpDown=clamp(m_fSteeringUpDown);
 if(m_nModelIndex==539){
  const float step=CTimer::GetTimeStep();const int32 axis=CPad::GetPad(playerNum)->GetSteeringLeftRight();
  const float target=float(axis)*StaticRef<float>(0x858B88);m_fLeftRightSkid=PlaneInputSkidCpp(m_fLeftRightSkid,target,step,StaticRef<float>(0x858CC4));
 }else{
  const float right=float(uint8(CPad::GetPad(playerNum)->GetLookRight()));const bool left=CPad::GetPad(playerNum)->GetLookLeft();
  m_fLeftRightSkid=PlaneInputSkidCpp(m_fLeftRightSkid,left?-1.f:right,CTimer::GetTimeStep(),StaticRef<float>(0x858CC4));
 }
 m_fSteerAngle=PlaneGroundSteerCpp(forwardSpeed,m_pHandlingData->m_fSteeringLock,m_fLeftRightSkid,StaticRef<float>(0x858B1C),StaticRef<float>(0x8595EC));
 if(m_nModelIndex==539&&CPad::GetPad(playerNum)->GetHandBrake()){
  const float multiplier=StaticRef<float>(0x858CE8);m_fSteerAngle*=multiplier;m_fLeftRightSkid*=multiplier;
 }
 const int32 accelerator=CPad::GetPad(playerNum)->GetAccelerate();const int32 brake=CPad::GetPad(playerNum)->GetBrake();
 m_fAccelerationBreakStatus=float(accelerator-brake)*StaticRef<float>(0x859A3C);
 auto* pad=CPad::GetPad(playerNum);auto* padBytes=reinterpret_cast<uint8*>(pad);
 const bool toggle=*reinterpret_cast<int16*>(padBytes+0x26)!=0&&*reinterpret_cast<int16*>(padBytes+0x56)==0;
 const auto* flying=reinterpret_cast<const uint8*>(m_pFlyingHandlingData);
 if(toggle&&std::ranges::all_of(m_fWheelsSuspensionCompression,[](float v){return std::bit_cast<uint32>(v)==0x3F800000u;})&&*reinterpret_cast<const float*>(flying+0x30)<1.f&&m_nModelIndex!=464){
  if(m_fLandingGearStatus==0.f){bytes[0x5A5]=bytes[0x5A6]=bytes[0x5A7]=bytes[0x5A8]=2;m_fLandingGearStatus=RpNanMulAddCpp(CTimer::GetTimeStep(),StaticRef<float>(0x871904),m_fLandingGearStatus);}
  else if(std::bit_cast<uint32>(m_fLandingGearStatus)==0x3F800000u)m_fLandingGearStatus=RpNanMulAddCpp(CTimer::GetTimeStep(),StaticRef<float>(0x871904),-1.f);
  else m_fLandingGearStatus=-m_fLandingGearStatus;
 }else if(m_fLandingGearStatus<0.f){
  const bool complete=PlaneGearAdvanceCpp(&m_fLandingGearStatus,CTimer::GetTimeStep(),StaticRef<float>(0x871904),0.f);
  if(complete){m_fLandingGearStatus=0.f;bytes[0x5A5]=bytes[0x5A6]=bytes[0x5A7]=bytes[0x5A8]=0;m_fAirResistance=PlaneGearResistanceCpp(m_pHandlingData->m_fDragMult,StaticRef<float>(0x871900),StaticRef<float>(0x858B8C));}
 }else if(m_fLandingGearStatus>0.f&&m_fLandingGearStatus<1.f){
  if(PlaneGearAdvanceCpp(&m_fLandingGearStatus,CTimer::GetTimeStep(),StaticRef<float>(0x871904),1.f))SetGearUp();
 }
 if(m_nModelIndex==520&&std::abs(CPad::GetPad(playerNum)->GetCarGunUpDown())>StaticRef<float>(0x85862C)){
  auto& nozzle=*reinterpret_cast<uint16*>(bytes+0x86C);*reinterpret_cast<uint16*>(bytes+0x86E)=nozzle;
  float step=CTimer::GetTimeStep();const int32 firstAxis=CPad::GetPad(playerNum)->GetCarGunUpDown();const int32 first=int32(nozzle)+int16(PlaneNozzleChangeCpp(firstAxis,step,StaticRef<float>(0x8D33DC),StaticRef<float>(0x858B88)));
  if(first<0)nozzle=0;else{step=CTimer::GetTimeStep();const int32 secondAxis=CPad::GetPad(playerNum)->GetCarGunUpDown();nozzle=uint16(int32(nozzle)+int16(PlaneNozzleChangeCpp(secondAxis,step,StaticRef<float>(0x8D33DC),StaticRef<float>(0x858B88))));}
  const int32 limit=int16(HARRIER_NOZZLE_ROTATE_LIMIT);if(int32(nozzle)>limit)nozzle=uint16(limit);
 }
 bytes[0x428]&=0xDF;m_GasPedal=0.f;
 if(forwardSpeed>0.f&&CPad::GetPad(playerNum)->GetBrake()!=0){const int32 input=CPad::GetPad(playerNum)->GetBrake();m_BrakePedal=float(input)*StaticRef<float>(forwardSpeed>StaticRef<float>(0x858F9C)?0x871A70:0x859A3C);}
 else if(PlaneSpeedBelowCpp(&m_vecMoveSpeed.x,StaticRef<float>(0x858B1C))&&CPad::GetPad(playerNum)->GetBrake()<10&&CPad::GetPad(playerNum)->GetAccelerate()<10)m_BrakePedal=.5f;
 else m_BrakePedal=0.f;
 if(CPad::GetPad(playerNum)->DisablePlayerControls){bytes[0x428]|=0x20;m_BrakePedal=1.f;m_GasPedal=0.f;FindPlayerPed(-1)->KeepAreaAroundPlayerClear();PlaneSpeedCapCpp(&m_vecMoveSpeed.x,StaticRef<float>(0x871254));}
}

// 0x6CB7C0
void CPlane::ProcessFlyingCarStuff() {
    if (CTimer::GetTimeStep() <= 0.f) return;
    auto* bytes = reinterpret_cast<uint8*>(this);
    auto field = [bytes](size_t offset) -> float& { return *reinterpret_cast<float*>(bytes + offset); };
    auto status = [bytes] { return bytes[0x36] >> 3; };
    const auto rollFn = reinterpret_cast<float(__fastcall*)(const void*, void*)>(std::bit_cast<uintptr_t>(&CPlaceable::GetRoll)); // 0x420B30: deja el fpatan sin redondear en ST0
    const int32 waveRandom = CGeneral::GetRandomNumber();
    m_planeDamageWave = std::bit_cast<int32>(uint32(m_planeDamageWave) + PlaneWaveAdvanceCpp(StaticRef<float>(0x8D33E0), CTimer::GetTimeStep(), waveRandom, StaticRef<float>(0x858C7C), StaticRef<float>(0x858B38), StaticRef<float>(0x858C4C)));
    const float speed = PlaneDamageSpeedCpp(&m_vecMoveSpeed.x);
    float skid = m_fLeftRightSkid, up = m_fSteeringUpDown, lateral = m_fSteeringLeftRight;
    if (m_fHealth < StaticRef<float>(0x859F80) && status() == 0) {
        m_damageManager.SetAeroplaneCompStatus(12, 2);
        m_damageManager.SetAeroplaneCompStatus(13, 2);
        skid += .5f;
        if (PlaneRollCompareCpp(rollFn, this, StaticRef<float>(0x86DAF4), false)) lateral += .75f;
        if (PlaneRollCompareCpp(rollFn, this, StaticRef<float>(0x858FE4), true)) up += .5f;
    }
    if (m_nModelIndex != 464) {
        for (int32 node = 12; node < 25; ++node) {
            const int32 damage = std::bit_cast<int32>(m_damageManager.GetAeroplaneCompStatus(uint8(node)));
            if (!m_aCarNodes[node] || damage <= 0) continue;
            int32 panel = -1;
            for (int32 i = 0; i < 4; ++i) {
                if (*reinterpret_cast<int16*>(bytes + 0x6AC + 0x20 * i) == node) { panel = i; break; }
            }
            float strength;
            if (node <= 15) {
                if (m_fAccelerationBreakStatus > 0.f) {
                    const float spread = StaticRef<float>(0x8D344C);
                    const float upper = PlaneFlightBoundCpp(spread, false), lower = PlaneFlightBoundCpp(spread, true);
                    const int32 random = CGeneral::GetRandomNumber();
                    m_fAccelerationBreakStatus = PlanePropellerWaveCpp(m_fAccelerationBreakStatus, upper, lower,
                        random, damage, uint32(m_planeDamageWave), StaticRef<uint32>(0x8D3448), CTimer::GetTimeStep(),
                        StaticRef<float>(0x858C7C), StaticRef<float>(0x858CBC), StaticRef<float>(0x8D3444));
                }
                strength = float(damage) * .5f;
            } else if (node <= 20) {
                const size_t axis = node == 16 ? 0x988 : node <= 18 ? 0x98C : 0x990;
                const uintptr_t amplitudeAddress = node == 16 ? 0x8D3440 : node <= 18 ? 0x8D3434 : 0x8D3428;
                const uintptr_t panelAddress = node == 16 ? 0x8D343C : node <= 18 ? 0x8D3430 : 0x8D3424;
                const uintptr_t feedbackAddress = node == 16 ? 0x8D3438 : node <= 18 ? 0x8D342C : 0x8D3420;
                const float amplitude = StaticRef<float>(amplitudeAddress);
                field(axis) = PlaneFlightDecayCpp(field(axis), damage, StaticRef<float>(0x858CC4), node == 16);
                const int32 random = CGeneral::GetRandomNumber();
                const float squaredDamage = float(std::bit_cast<int32>(uint32(damage) * uint32(damage)));
                field(axis) = PlaneDamageNoiseCpp(field(axis), amplitude, random, squaredDamage, CTimer::GetTimeStep(), speed, StaticRef<float>(0x858C7C));
                float& local = node == 16 ? skid : node <= 18 ? up : lateral;
                local = field(axis);
                if (panel >= 0) {
                    const float panelAmplitude = StaticRef<float>(panelAddress);
                    const int32 panelRandom = CGeneral::GetRandomNumber();
                    auto& rotationSpeed = field(0x6C4 + panel * 0x20);
                    rotationSpeed = PlaneDamageNoiseCpp(rotationSpeed, panelAmplitude, panelRandom,
                        squaredDamage, CTimer::GetTimeStep(), speed, StaticRef<float>(0x858C7C));
                    local = PlaneFlightProductAddCpp(local, StaticRef<float>(feedbackAddress), field(0x6B8 + panel * 0x20), node != 19);
                }
                strength = float(damage) * StaticRef<float>(node == 16 ? 0x858CC4 : node <= 18 ? 0x858FCC : 0x858C84);
            } else continue;
            if (strength > 0.f && !(bytes[0x42B] & 0x40) && (speed > StaticRef<float>(0x858C24) || (CGeneral::GetRandomNumber() & 7) == 0)) {
                auto* cameraMatrix = StaticRef<CMatrix*>(0xB6F03C);
                const CVector* cameraPosition = cameraMatrix ? &cameraMatrix->GetPosition() : &StaticRef<CVector>(0xB6F02C);
                if ((PlaneDistanceBelowCpp(&GetPosition().x, &cameraPosition->x, StaticRef<float>(0x859AEC)) || status() == 0) && m_aCarNodes[node]) {
                    CVector point;
                    reinterpret_cast<CVector*(__cdecl*)(CVector*, const CMatrix*, const CVector*)>(0x59C890)(
                        &point, m_matrix, reinterpret_cast<const CVector*>(&m_aCarNodes[node]->modelling.pos));
                    FxPrtMult_c mults{0.f, 0.f, 0.f, .2f, 1.f, 1.f, strength};
                    CVector velocity;
                    PlaneSmokeParticleVelocityCpp(&velocity.x, &m_vecMoveSpeed.x, StaticRef<float>(0x858C84), StaticRef<float>(0x858B40));
                    velocity.x = PlaneVelocityNoiseCpp(velocity.x, CGeneral::GetRandomNumber(), StaticRef<float>(0x858C7C), StaticRef<float>(0x871A74), StaticRef<float>(0x858C20));
                    velocity.y = PlaneVelocityNoiseCpp(velocity.y, CGeneral::GetRandomNumber(), StaticRef<float>(0x858C7C), StaticRef<float>(0x871A74), StaticRef<float>(0x858C20));
                    velocity.z = PlaneVelocityNoiseCpp(velocity.z, CGeneral::GetRandomNumber(), StaticRef<float>(0x858C7C), StaticRef<float>(0x871A74), StaticRef<float>(0x858C20));
                    mults.m_fLife = IntTimesFloatCpp(CGeneral::GetRandomNumber(), StaticRef<float>(0x858C7C)); // EXE overwrites multiplier life at stack+0x98, 0x6CC003.
                    point.x = PlanePositionNoiseCpp(point.x, strength, CGeneral::GetRandomNumber(), StaticRef<float>(0x858C7C));
                    point.y = PlanePositionNoiseCpp(point.y, strength, CGeneral::GetRandomNumber(), StaticRef<float>(0x858C7C));
                    point.z = PlanePositionNoiseCpp(point.z, strength, CGeneral::GetRandomNumber(), StaticRef<float>(0x858C7C));
                    g_fx.m_SmokeHuge->AddParticle(point, velocity, 0.f, mults, -1.f, 1.2f, .6f, false);
                }
            }
        }
    }
    const uint8 vehicleStatus = status();
    if (vehicleStatus != 0 && vehicleStatus != 8 && vehicleStatus != 3) {
        const float decrement = CTimer::GetTimeStep() * StaticRef<float>(0x858CDC);
        if (m_fPropSpeed > decrement) {
            if (m_fPropSpeed > StaticRef<float>(0x8D33D0)) m_fPropSpeed = PlaneFlightProductAddCpp(m_fPropSpeed, CTimer::GetTimeStep(), StaticRef<float>(0x859CD8), true);
            else m_fPropSpeed -= decrement;
            bytes[0xB8] = 0;
        } else { m_fPropSpeed = 0.f; bytes[0x428] &= 0xEF; }
        return;
    }
    float target = PlanePropellerTargetCpp(m_fAccelerationBreakStatus, StaticRef<float>(0x8D33D0), StaticRef<float>(0x8D33D4), StaticRef<float>(0x8D33CC));
    if (vehicleStatus == 0 || vehicleStatus == 8) { // 0x6CC20D..0x6CC2C8: tres consultas a CVehicle::HeightAboveCeiling (0x6D2600) con la z leida cada vez
        const auto flightModel = eFlightModel(m_nModelIndex == 464 ? 1 : 3);
        if (HeightAboveCeiling(GetPosition().z, flightModel) > 0.f) {
            target = PlaneCeilingTargetCpp(HeightAboveCeiling(GetPosition().z, flightModel), target, StaticRef<float>(0x858B38));
            m_fAccelerationBreakStatus = PlaneCeilingAccelerationCpp(HeightAboveCeiling(GetPosition().z, flightModel), m_fAccelerationBreakStatus, StaticRef<float>(0x858CEC));
        }
    }
    const bool drowned = (bytes[0x42B] & 0x40) != 0;
    m_fPropSpeed = PlaneFloatBlendCpp(m_fPropSpeed, target, CTimer::GetTimeStep(), StaticRef<float>(0x8D33D8));
    if (drowned) { bytes[0x428] &= 0xEF; m_fPropSpeed = m_fAccelerationBreakStatus = 0.f; }
    if (m_nModelIndex == 464) {
        if (!drowned && (bytes[0x428] & 0x10)) FlyingControl(eFlightModel(1), skid, up, lateral, m_fAccelerationBreakStatus);
        return;
    }
    if (!(bytes[0x428] & 0x10)) return;
    if (!(m_fPropSpeed > StaticRef<float>(0x8D33CC)) && !PlaneSpeedSquaredAboveCpp(&m_vecMoveSpeed.x, StaticRef<double>(0x859068))) return;
    if (m_nModelIndex == 520 && status() == 0 && int32(*reinterpret_cast<uint16*>(bytes + 0x86C)) >= int32(StaticRef<int16>(0x8D33C8))) {
        auto* old = m_pFlyingHandlingData;
        auto* modelInfo = StaticRef<uint8*>(0xA9B76C);
        m_pFlyingHandlingData = reinterpret_cast<tFlyingHandlingData*(__thiscall*)(void*, uint8)>(0x6F52D0)(reinterpret_cast<void*>(0xC2B9C8), modelInfo[0x4A]);
        if (double(m_fAccelerationBreakStatus) > StaticRef<double>(0x859EF8) || (bytes[0x960] < 4 && !(*reinterpret_cast<uint32*>(bytes + 0x40) & 0x08000000)))
            FlyingControl(eFlightModel(6), skid, up, -lateral, m_fAccelerationBreakStatus * StaticRef<float>(0x8D341C));
        m_pFlyingHandlingData = old;
        return;
    }
    FlyingControl(eFlightModel(3), skid, up, lateral, m_fAccelerationBreakStatus);
}
