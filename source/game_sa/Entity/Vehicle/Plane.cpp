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
 * Funciones bloqueadas: 9.
 * Validación registrada: 23/23 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   BlowUpCar | 0x00000000 | V
 *   DoPlaneGenerationAndRemoval | 0x006CD2F0 | S
 *   FindPlaneCreationCoors | 0x006CD090 | S
 *   PreRender | 0x00000000 | V
 *   ProcessControl | 0x00000000 | V
 *   ProcessControlInputs | 0x00000000 | V
 *   ProcessFlyingCarStuff | 0x00000000 | V
 *   SwitchAmbientPlanes | 0x006CCC50 | S
 *   VehicleDamage | 0x00000000 | V
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
static float PlaneMassThreshold355(float mass, float threshold, bool minimumOne) {
    const float scale = PLANE_DAMAGE_SCALE_MASS, one = 1.f;
    float result;
    __asm {
        fld mass
        fdiv scale
        cmp minimumOne, 0
        je threshold_multiply
        fld one
        fcomp st(1)
        fnstsw ax
        test ah, 41h
        jne threshold_multiply
        fstp st(0)
        fld one
    threshold_multiply:
        fmul threshold
        fstp result
    }
    return result;
}

static bool PlaneSelectCloser355(const CVector* delta,float* closest){
 uint16 compareStatus;
 __asm {
  mov ecx,delta
  fld dword ptr [ecx+8]
  fld dword ptr [ecx+4]
  fld dword ptr [ecx]
  fld st(0)
  fmul st,st(1)
  fld st(2)
  fmul st,st(3)
  faddp st(1),st
  fld st(3)
  fmul st,st(4)
  faddp st(1),st
  fstp st(3)
  fstp st(0)
  fstp st(0)
  mov edx,closest
  fld dword ptr [edx]
  fmul dword ptr [edx]
  fld st(1)
  fcompp
  fnstsw ax
  mov compareStatus,ax
  test ah,5
  jp plane_not_closer
  fsqrt
  fstp dword ptr [edx]
  jmp plane_closer_done
 plane_not_closer:
  fstp st(0)
 plane_closer_done:
 }
 return (compareStatus&0x500)==0x100;
}

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
        partThreshold = PlaneMassThreshold355(m_fMass, partThreshold, true);
        damageThreshold = PlaneMassThreshold355(m_fMass, damageThreshold, false);
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
    float* health = &m_fHealth;
    __asm {
        mov eax, health
        fld healthDamage
        fmul factor
        fsubr dword ptr [eax]
        fstp dword ptr [eax]
    }
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
            const auto delta = CVector{node->modelling.pos} - relative;
            if (PlaneSelectCloser355(&delta, &closest)) selected = i;
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
static float PlaneSpawnTrig355(float angle, float radius, float position, bool sine) {
    float result;
    if (sine) {
        __asm {
            fld angle
            fsin
            fmul radius
            fadd position
            fstp result
        }
    } else {
        __asm {
            fld angle
            fcos
            fmul radius
            fadd position
            fstp result
        }
    }
    return result;
}

void CPlane::FindPlaneCreationCoors(CVector* outCoors, CVector* outTargetCoors, float* outPlaneOrientation, float* outFlightHeight, bool isBigPlane) {
    const auto radius = isBigPlane ? 340.f : 140.f;
    const auto baseHeight = isBigPlane ? 200.f : 25.f;
    CColSphere sphere;
    CColBox box;
    for (int32 offset = 0; offset < 500; offset += 10) {
        const auto angle = float(CGeneral::GetRandomNumber() % 360) * StaticRef<float>(0x8595EC);
        *outCoors = FindPlayerCoors(-1);
        *outFlightHeight = (float(CGeneral::GetRandomNumber() & 15) + float(offset)) + baseHeight;
        outCoors->x = PlaneSpawnTrig355(angle, radius, outCoors->x, false);
        outCoors->y = PlaneSpawnTrig355(angle, radius, outCoors->y, true);
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

static void PlaneGenerationHeading355(CVehicle* vehicle,float heading){
 auto* matrix=vehicle->m_matrix;
 __asm {
  fld heading
  mov eax,matrix
  fsin
  xor ecx,ecx
  mov dword ptr [eax+8],ecx
  mov dword ptr [eax+18h],ecx
  mov dword ptr [eax+20h],ecx
  mov dword ptr [eax+24h],ecx
  mov dword ptr [eax+28h],3F800000h
  fst dword ptr [eax]
  fld heading
  fcos
  fld st(0)
  fchs
  fstp dword ptr [eax+4]
  fstp dword ptr [eax+10h]
  fstp dword ptr [eax+14h]
 }
}
static CVector PlaneGenerationVelocity355(float heading,bool helicopter){
 const float speed=StaticRef<float>(0x862D68);CVector result;
 auto* output=&result;
 __asm {
  mov edx,output
  cmp helicopter,0
  je plane_generated_speed
  fld heading
  fcos
  fld heading
  fsin
  fmul speed
  fstp dword ptr [edx+4]
  fmul speed
  fstp dword ptr [edx]
  jmp generated_speed_done
 plane_generated_speed:
  fld heading
  fsin
  fld heading
  fcos
  fmul speed
  fstp dword ptr [edx]
  fmul speed
  fstp dword ptr [edx+4]
 generated_speed_done:
  mov dword ptr [edx+8],0
 }
 return result;
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
  while(remaining){--remaining;auto* vehicle=reinterpret_cast<CVehicle*(__thiscall*)(void*,int32)>(0x41CC10)(pool,remaining);if(vehicle&&reinterpret_cast<uint8*>(vehicle)[0x3BA]==59)++hydras;}
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
// Complete REA transcription preserves x87 precision and original collaborator ABI.
// All internal code, switch entries and five SEH unwind states reside in the DLL.
static const uintptr_t PlanePreRender355Dispatcher=0x8215A0;
static const uintptr_t PlanePreRender355MatrixDestructor=0x59ACD0;
static __declspec(naked) void __cdecl PlanePreRender355Unwind0() {
    __asm { lea ecx,[ebp-0D4h] }
    __asm { jmp dword ptr [PlanePreRender355MatrixDestructor] }
}
static __declspec(naked) void __cdecl PlanePreRender355Unwind1() {
    __asm { lea ecx,[ebp-70h] }
    __asm { jmp dword ptr [PlanePreRender355MatrixDestructor] }
}
static __declspec(naked) void __cdecl PlanePreRender355Unwind2() {
    __asm { lea ecx,[ebp-70h] }
    __asm { jmp dword ptr [PlanePreRender355MatrixDestructor] }
}
static __declspec(naked) void __cdecl PlanePreRender355Unwind3() {
    __asm { lea ecx,[ebp-70h] }
    __asm { jmp dword ptr [PlanePreRender355MatrixDestructor] }
}
static __declspec(naked) void __cdecl PlanePreRender355Unwind4() {
    __asm { lea ecx,[ebp-70h] }
    __asm { jmp dword ptr [PlanePreRender355MatrixDestructor] }
}
struct PlanePreRender355UnwindEntry { int32 previousState; void(__cdecl* action)(); };
static const PlanePreRender355UnwindEntry PlanePreRender355UnwindMap[] = {
    {-1, &PlanePreRender355Unwind0},
    {0, &PlanePreRender355Unwind1},
    {0, &PlanePreRender355Unwind2},
    {0, &PlanePreRender355Unwind3},
    {0, &PlanePreRender355Unwind4},
};
struct PlanePreRender355FuncInfo { uint32 magic; int32 maxState; const PlanePreRender355UnwindEntry* unwind; int32 tryCount; const void* tries; int32 ipCount; const void* ipMap; };
static const PlanePreRender355FuncInfo PlanePreRender355ExceptionInfo{0x19930520,5,PlanePreRender355UnwindMap,0,nullptr,0,nullptr};
static_assert(sizeof(PlanePreRender355FuncInfo)==28);
static_assert(sizeof(PlanePreRender355UnwindEntry)==8);
static __declspec(naked) void __cdecl PlanePreRender355ExceptionHandler() {
    __asm { mov eax,offset PlanePreRender355ExceptionInfo }
    __asm { jmp dword ptr [PlanePreRender355Dispatcher] }
}
static const uint8 PlanePreRender355SwitchMap[] = {0,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,2,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,3,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,5,1,5,5,5,5,5,5,5,5,5,5,5,5,5,5,4};
static const uintptr_t PlanePreRender355Calls[] = {
    0x004A9BE0, // collaborator 0
    0x004AA2F0, // collaborator 1
    0x004AA3F0, // collaborator 2
    0x004AA440, // collaborator 3
    0x004AA630, // collaborator 4
    0x004AA660, // collaborator 5
    0x004AA6C0, // collaborator 6
    0x004AA890, // collaborator 7
    0x004AA910, // collaborator 8
    0x004AB290, // collaborator 9
    0x004C7D20, // collaborator 10
    0x004C7DD0, // collaborator 11
    0x00535300, // collaborator 12
    0x0059ACD0, // collaborator 13
    0x0059AF80, // collaborator 14
    0x0059BB60, // collaborator 15
    0x0059BBB0, // collaborator 16
    0x0059BD10, // collaborator 17
    0x0059BE30, // collaborator 18
    0x0059C080, // collaborator 19
    0x0059C600, // collaborator 20
    0x0059C890, // collaborator 21
    0x0059C910, // collaborator 22
    0x006AA290, // collaborator 23
    0x006B0690, // collaborator 24
    0x006D2960, // collaborator 25
    0x006D6480, // collaborator 26
    0x006DBA30, // collaborator 27
    0x006DD130, // collaborator 28
    0x006F49A0, // collaborator 29
    0x0070BDA0, // collaborator 30
    0x00721DC0, // collaborator 31
    0x007F1200, // collaborator 32
    0x00821B40, // collaborator 33
    0x00821E70, // collaborator 34
    0x00822130, // collaborator 35
};
static uintptr_t PlanePreRender355SwitchTargets[6]{};
#pragma warning(push)
#pragma warning(disable:4733)
__declspec(naked) void CPlane::PreRender() {
    __asm {
        mov dword ptr [PlanePreRender355SwitchTargets+0],offset plane_prerender_006C9DB1
        mov dword ptr [PlanePreRender355SwitchTargets+4],offset plane_prerender_006C9DCC
        mov dword ptr [PlanePreRender355SwitchTargets+8],offset plane_prerender_006C9DED
        mov dword ptr [PlanePreRender355SwitchTargets+12],offset plane_prerender_006C9E37
        mov dword ptr [PlanePreRender355SwitchTargets+16],offset plane_prerender_006C9E16
        mov dword ptr [PlanePreRender355SwitchTargets+20],offset plane_prerender_006C9EE3

        push 0FFFFFFFFh
        push offset PlanePreRender355ExceptionHandler
        mov eax,dword ptr fs:[00000000h]
        push eax
        mov dword ptr fs:[00000000h],esp
        sub esp,128h
        push ebx
        push ebp
        push esi
        xor ebx,ebx
        push edi
        mov esi,ecx
        mov dword ptr [esp+000000B0h],ebx
        mov dword ptr [esp+000000B4h],ebx
        movsx eax,word ptr [esi+22h]
        mov ebp,dword ptr [eax*4+00A9B0C8h]
        mov dword ptr [esp+00000140h],ebx
        mov dword ptr [esp+38h],ebx
        mov dword ptr [esp+48h],ebp
        call dword ptr [PlanePreRender355Calls+48]
        mov ecx,esi
        mov dword ptr [esp+10h],eax
        call dword ptr [PlanePreRender355Calls+104]
        cmp word ptr [esi+22h],21Bh
        jne plane_prerender_006C950D
        mov edx,dword ptr [esi]
        mov ecx,esi
        call dword ptr [edx+00000114h]
    plane_prerender_006C950D:
        test byte ptr [esi+0000042Bh],1
        je plane_prerender_006C966B
        mov eax,dword ptr [esi]
        mov ecx,esi
        call dword ptr [eax+000000D0h]
        xor ebp,ebp
        lea edi,[esi+00000838h]
        lea ebx,[esi+00000738h]
    plane_prerender_006C9532:
        fld dword ptr [edi+40h]
        push 1
        fdiv dword ptr [edi+50h]
        lea ecx,[esp+50h]
        push ecx
        mov ecx,dword ptr [esp+50h]
        push ebp
        fsubr dword ptr ds:[00858624h]
        fld dword ptr [edi-64h]
        fsub st,st(1)
        fld dword ptr ds:[00858624h]
        fsub st,st(2)
        fdivp st(1),st
        fstp dword ptr [esp+28h]
        fstp st(0)
        call dword ptr [PlanePreRender355Calls+40]
        mov edx,dword ptr [esi+00000384h]
        fld dword ptr [esp+54h]
        fadd dword ptr [edx+000000B8h]
        fld dword ptr [esp+1Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006C958E
        fld dword ptr [esp+1Ch]
        fmul dword ptr [edi+40h]
        fsubp st(1),st
    plane_prerender_006C958E:
        fcom dword ptr [edi]
        fnstsw ax
        test ah,41h
        je plane_prerender_006C95B6
        test dword ptr [esi+40h],4000000h
        je plane_prerender_006C95AC
        test dword ptr [esi+0000038Ch],20000h
        jne plane_prerender_006C95B6
    plane_prerender_006C95AC:
        fsub dword ptr [edi]
        fmul dword ptr ds:[00858F34h]
        fadd dword ptr [edi]
    plane_prerender_006C95B6:
        fstp dword ptr [edi]
        cmp word ptr [esi+22h],250h
        jne plane_prerender_006C9655
        fld dword ptr [edi-64h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jp plane_prerender_006C9649
        mov eax,dword ptr [esi+14h]
        fld dword ptr [eax+18h]
        fmul dword ptr [ebx+4]
        fld dword ptr [ebx-4]
        fmul dword ptr [eax+10h]
        faddp st(1),st
        fld dword ptr [eax+14h]
        fmul dword ptr [ebx]
        faddp st(1),st
        fld dword ptr ds:[00858C1Ch]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        je plane_prerender_006C9621
        fld dword ptr ds:[00858624h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp plane_prerender_006C9638
        fstp st(0)
        mov dword ptr [esp+14h],3F800000h
        fld dword ptr [esp+14h]
        call dword ptr [PlanePreRender355Calls+136]
        fchs
        jmp plane_prerender_006C9652
    plane_prerender_006C9621:
        fstp st(0)
        mov dword ptr [esp+14h],0BF800000h
        fld dword ptr [esp+14h]
        call dword ptr [PlanePreRender355Calls+136]
        fchs
        jmp plane_prerender_006C9652
    plane_prerender_006C9638:
        fstp dword ptr [esp+14h]
        fld dword ptr [esp+14h]
        call dword ptr [PlanePreRender355Calls+136]
        fchs
        jmp plane_prerender_006C9652
    plane_prerender_006C9649:
        fld dword ptr [edi-10h]
        fmul dword ptr ds:[00858EF0h]
    plane_prerender_006C9652:
        fstp dword ptr [edi-10h]
    plane_prerender_006C9655:
        inc ebp
        add edi,4
        add ebx,2Ch
        cmp ebp,4
        jl plane_prerender_006C9532
        mov ebp,dword ptr [esp+48h]
        xor ebx,ebx
    plane_prerender_006C966B:
        mov edi,1
        push edi
        push 4
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+92]
        push edi
        push 7
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+92]
        push edi
        push 2
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+92]
        cmp word ptr [esi+22h],208h
        jne plane_prerender_006C969B
        mov edi,3
    plane_prerender_006C969B:
        push edi
        push 5
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+92]
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr [esi+000009C4h]
        fadd dword ptr [esi+000009C8h]
        fst dword ptr [esi+000009C8h]
        fcomp dword ptr ds:[00858CBCh]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006C96E9
        fld dword ptr [esi+000009C8h]
    plane_prerender_006C96D0:
        fsub dword ptr ds:[00858CBCh]
        fcom dword ptr ds:[00858CBCh]
        fnstsw ax
        test ah,41h
        je plane_prerender_006C96D0
        fstp dword ptr [esi+000009C8h]
    plane_prerender_006C96E9:
        mov al,byte ptr [esi+36h]
        and al,0F8h
        cmp al,18h
        ja plane_prerender_006C9C94
        mov eax,dword ptr [esi+00000688h]
        cmp eax,ebx
        je plane_prerender_006C9823
        push ebx
        add eax,10h
        push eax
        lea ecx,[esp+78h]
        call dword ptr [PlanePreRender355Calls+68]
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
        mov ecx,dword ptr [ebp+5Ch]
        add ecx,84h
        mov edx,dword ptr [ecx]
        mov dword ptr [esp+20h],edx
        fld dword ptr [esp+20h]
        mov eax,dword ptr [ecx+4]
        fcomp dword ptr ds:[00858B50h]
        mov dword ptr [esp+24h],eax
        mov ecx,dword ptr [ecx+8]
        mov dword ptr [esp+28h],ecx
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9775
        fld dword ptr [esp+24h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9775
        fld dword ptr [esp+28h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9775
        mov dword ptr [esp+28h],3F800000h
        jmp plane_prerender_006C97AB
    plane_prerender_006C9775:
        fld dword ptr [esp+20h]
        lea ecx,[esp+20h]
        fsub dword ptr [esp+000000A0h]
        fstp dword ptr [esp+20h]
        fld dword ptr [esp+24h]
        fsub dword ptr [esp+000000A4h]
        fstp dword ptr [esp+24h]
        fld dword ptr [esp+28h]
        fsub dword ptr [esp+000000A8h]
        fstp dword ptr [esp+28h]
        call dword ptr [PlanePreRender355Calls+88]
    plane_prerender_006C97AB:
        fld dword ptr ds:[00871914h]
        push ecx
        fmul dword ptr ds:[008595ECh]
        lea edx,[esp+24h]
        lea ecx,[esp+000000BCh]
        fmul dword ptr [esi+00000988h]
        fstp dword ptr [esp]
        push edx
        call dword ptr [PlanePreRender355Calls+80]
        mov eax,dword ptr [esp+000000B0h]
        push eax
        lea ecx,[esp+000000BCh]
        call dword ptr [PlanePreRender355Calls+76]
        cmp word ptr [esi+22h],21Bh
        jne plane_prerender_006C9823
        mov eax,dword ptr [esi+000006A8h]
        cmp eax,ebx
        je plane_prerender_006C9823
        push ebx
        add eax,10h
        push eax
        lea ecx,[esp+78h]
        call dword ptr [PlanePreRender355Calls+68]
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
        mov ecx,dword ptr [esp+000000B0h]
        push ecx
        lea ecx,[esp+000000BCh]
        call dword ptr [PlanePreRender355Calls+76]
    plane_prerender_006C9823:
        mov ecx,dword ptr [esi+00000698h]
        cmp ecx,ebx
        je plane_prerender_006C9B31
        mov edx,dword ptr [esi+00000388h]
        fld dword ptr [edx+34h]
        mov dword ptr [esp+18h],3F800000h
        fcomp dword ptr ds:[00858624h]
        mov dword ptr [esp+14h],0
        fnstsw ax
        test ah,41h
        jne plane_prerender_006C98B2
        fld dword ptr [esi+000009CCh]
        fabs
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jp plane_prerender_006C98B2
        cmp dword ptr [esi+0000069Ch],ebx
        jne plane_prerender_006C987C
        cmp dword ptr [esi+000006A0h],ebx
        je plane_prerender_006C98B2
    plane_prerender_006C987C:
        fld dword ptr [esi+000009CCh]
        fabs
        fsubr dword ptr ds:[00858624h]
        fmul dword ptr ds:[00871920h]
        fadd dword ptr ds:[00858624h]
        fstp dword ptr [esp+18h]
        fld dword ptr [esi+000009CCh]
        fabs
        fsubr dword ptr ds:[00858624h]
        fmul dword ptr ds:[00871924h]
        fstp dword ptr [esp+14h]
    plane_prerender_006C98B2:
        add ecx,10h
        push ebx
        push ecx
        lea ecx,[esp+78h]
        call dword ptr [PlanePreRender355Calls+68]
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
        mov eax,dword ptr [ebp+5Ch]
        add eax,6Ch
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+20h],ecx
        fld dword ptr [esp+20h]
        mov edx,dword ptr [eax+4]
        fcomp dword ptr ds:[00858B50h]
        mov dword ptr [esp+24h],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+28h],eax
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9920
        fld dword ptr [esp+24h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9920
        fld dword ptr [esp+28h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9920
        mov dword ptr [esp+20h],3F800000h
        jmp plane_prerender_006C9956
    plane_prerender_006C9920:
        fld dword ptr [esp+20h]
        lea ecx,[esp+20h]
        fsub dword ptr [esp+000000A0h]
        fstp dword ptr [esp+20h]
        fld dword ptr [esp+24h]
        fsub dword ptr [esp+000000A4h]
        fstp dword ptr [esp+24h]
        fld dword ptr [esp+28h]
        fsub dword ptr [esp+000000A8h]
        fstp dword ptr [esp+28h]
        call dword ptr [PlanePreRender355Calls+88]
    plane_prerender_006C9956:
        fld dword ptr ds:[0087191Ch]
        push ecx
        fmul dword ptr ds:[008595ECh]
        lea ecx,[esp+24h]
        fmul dword ptr [esi+00000990h]
        fadd dword ptr [esp+18h]
        fstp dword ptr [esp]
        push ecx
        lea ecx,[esp+000000C0h]
        call dword ptr [PlanePreRender355Calls+80]
        mov edx,dword ptr [esp+000000B0h]
        push edx
        lea ecx,[esp+000000BCh]
        call dword ptr [PlanePreRender355Calls+76]
        fld dword ptr [esp+18h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006C9A2A
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+60]
        fld dword ptr [esp+20h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C99D8
        fld dword ptr [esp+00000084h]
        fmul dword ptr [esp+18h]
        fstp dword ptr [esp+00000084h]
        jmp plane_prerender_006C9A21
    plane_prerender_006C99D8:
        push 14h
        lea eax,[esp+50h]
        push eax
        mov ecx,ebp
        call dword ptr [PlanePreRender355Calls+44]
        test al,al
        je plane_prerender_006C9A21
        fld dword ptr ds:[00858624h]
        mov ecx,dword ptr [esp+10h]
        fsub dword ptr [esp+18h]
        mov edx,dword ptr [esp+54h]
        push edx
        fmul dword ptr [ecx+10h]
        mov ecx,dword ptr [esp+50h]
        fmul dword ptr ds:[00871928h]
        fadd dword ptr [esp+54h]
        fstp dword ptr [esp+54h]
        mov eax,dword ptr [esp+54h]
        push eax
        push ecx
        lea ecx,[esp+7Ch]
        call dword ptr [PlanePreRender355Calls+56]
    plane_prerender_006C9A21:
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
    plane_prerender_006C9A2A:
        mov eax,dword ptr [esi+00000694h]
        cmp eax,ebx
        je plane_prerender_006C9B31
        push ebx
        add eax,10h
        push eax
        lea ecx,[esp+78h]
        call dword ptr [PlanePreRender355Calls+68]
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
        fld dword ptr [esp+20h]
        fmul dword ptr ds:[00858C1Ch]
        push ecx
        lea edx,[esp+24h]
        lea ecx,[esp+000000BCh]
        fstp dword ptr [esp+24h]
        fld dword ptr ds:[0087191Ch]
        fmul dword ptr ds:[008595ECh]
        fmul dword ptr [esi+00000990h]
        fsub dword ptr [esp+18h]
        fstp dword ptr [esp]
        push edx
        call dword ptr [PlanePreRender355Calls+80]
        mov eax,dword ptr [esp+000000B0h]
        push eax
        lea ecx,[esp+000000BCh]
        call dword ptr [PlanePreRender355Calls+76]
        fld dword ptr [esp+18h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006C9B31
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+60]
        fld dword ptr [esp+20h]
        fcomp dword ptr ds:[00858C1Ch]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9ADF
        fld dword ptr [esp+00000084h]
        fmul dword ptr [esp+18h]
        fstp dword ptr [esp+00000084h]
        jmp plane_prerender_006C9B28
    plane_prerender_006C9ADF:
        push 13h
        lea ecx,[esp+50h]
        push ecx
        mov ecx,ebp
        call dword ptr [PlanePreRender355Calls+44]
        test al,al
        je plane_prerender_006C9B28
        fld dword ptr ds:[00858624h]
        mov edx,dword ptr [esp+10h]
        fsub dword ptr [esp+18h]
        mov eax,dword ptr [esp+54h]
        push eax
        fmul dword ptr [edx+10h]
        mov edx,dword ptr [esp+50h]
        fmul dword ptr ds:[00871928h]
        fadd dword ptr [esp+54h]
        fstp dword ptr [esp+54h]
        mov ecx,dword ptr [esp+54h]
        push ecx
        push edx
        lea ecx,[esp+7Ch]
        call dword ptr [PlanePreRender355Calls+56]
    plane_prerender_006C9B28:
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
    plane_prerender_006C9B31:
        mov eax,dword ptr [esi+00000690h]
        cmp eax,ebx
        je plane_prerender_006C9C94
        push ebx
        add eax,10h
        push eax
        lea ecx,[esp+78h]
        call dword ptr [PlanePreRender355Calls+68]
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
        mov eax,dword ptr [ebp+5Ch]
        add eax,78h
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+20h],ecx
        fld dword ptr [esp+20h]
        mov edx,dword ptr [eax+4]
        fcomp dword ptr ds:[00858B50h]
        mov dword ptr [esp+24h],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+28h],eax
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9BAD
        fld dword ptr [esp+24h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9BAD
        fld dword ptr [esp+28h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp plane_prerender_006C9BAD
        mov dword ptr [esp+20h],3F800000h
        jmp plane_prerender_006C9BE3
    plane_prerender_006C9BAD:
        fld dword ptr [esp+20h]
        lea ecx,[esp+20h]
        fsub dword ptr [esp+000000A0h]
        fstp dword ptr [esp+20h]
        fld dword ptr [esp+24h]
        fsub dword ptr [esp+000000A4h]
        fstp dword ptr [esp+24h]
        fld dword ptr [esp+28h]
        fsub dword ptr [esp+000000A8h]
        fstp dword ptr [esp+28h]
        call dword ptr [PlanePreRender355Calls+88]
    plane_prerender_006C9BE3:
        fld dword ptr ds:[00871918h]
        lea edx,[esp+20h]
        fmul dword ptr ds:[008595ECh]
        fmul dword ptr [esi+0000098Ch]
        fchs
        fstp dword ptr [esp+14h]
        mov ecx,dword ptr [esp+14h]
        push ecx
        push edx
        lea ecx,[esp+000000C0h]
        call dword ptr [PlanePreRender355Calls+80]
        mov eax,dword ptr [esp+000000B0h]
        push eax
        lea ecx,[esp+000000BCh]
        call dword ptr [PlanePreRender355Calls+76]
        mov eax,dword ptr [esi+0000068Ch]
        cmp eax,ebx
        je plane_prerender_006C9C94
        push ebx
        add eax,10h
        push eax
        lea ecx,[esp+78h]
        call dword ptr [PlanePreRender355Calls+68]
        lea ecx,[esp+70h]
        call dword ptr [PlanePreRender355Calls+64]
        fld dword ptr ds:[00871918h]
        fmul dword ptr ds:[008595ECh]
        lea edx,[esp+20h]
        fmul dword ptr [esi+0000098Ch]
        fstp dword ptr [esp+14h]
        fld dword ptr [esp+20h]
        mov ecx,dword ptr [esp+14h]
        fmul dword ptr ds:[00858C1Ch]
        push ecx
        push edx
        lea ecx,[esp+000000C0h]
        fstp dword ptr [esp+28h]
        call dword ptr [PlanePreRender355Calls+80]
        mov eax,dword ptr [esp+000000B0h]
        push eax
        lea ecx,[esp+000000BCh]
        call dword ptr [PlanePreRender355Calls+76]
    plane_prerender_006C9C94:
        mov edi,0Ch
        mov ebp,0Dh
        mov dword ptr [esp+14h],3F800000h
        jmp plane_prerender_006C9CB0
        lea esp,[esp+00000000h]
    plane_prerender_006C9CB0:
        mov eax,dword ptr [esi+edi*4+00000648h]
        cmp eax,ebx
        je plane_prerender_006C9D0B
        fld dword ptr [esp+14h]
        push 1
        fmul dword ptr [esi+000009C8h]
        push ecx
        mov ecx,esi
        fadd st(0),st
        fstp dword ptr [esp]
        push 1
        push eax
        call dword ptr [PlanePreRender355Calls+108]
        lea ecx,[esp+38h]
        push ecx
        mov dword ptr [esp+3Ch],ebx
        mov edx,dword ptr [esi+edi*4+00000648h]
        push 6A0750h
        push edx
        call dword ptr [PlanePreRender355Calls+128]
        mov eax,dword ptr [esp+44h]
        add esp,0Ch
        cmp eax,ebx
        je plane_prerender_006C9D0B
        push 0FFh
        push eax
        call dword ptr [PlanePreRender355Calls+100]
        add esp,8
    plane_prerender_006C9D0B:
        mov eax,dword ptr [esi+ebp*4+00000648h]
        cmp eax,ebx
        je plane_prerender_006C9D62
        fld dword ptr [esp+14h]
        push 1
        fmul dword ptr [esi+000009C8h]
        push ecx
        mov ecx,esi
        fchs
        fstp dword ptr [esp]
        push 1
        push eax
        call dword ptr [PlanePreRender355Calls+108]
        lea eax,[esp+38h]
        push eax
        mov dword ptr [esp+3Ch],ebx
        mov ecx,dword ptr [esi+ebp*4+00000648h]
        push 6A0750h
        push ecx
        call dword ptr [PlanePreRender355Calls+128]
        mov eax,dword ptr [esp+44h]
        add esp,0Ch
        cmp eax,ebx
        je plane_prerender_006C9D62
        push ebx
        push eax
        call dword ptr [PlanePreRender355Calls+100]
        add esp,8
    plane_prerender_006C9D62:
        cmp edi,0Ch
        jne plane_prerender_006C9D7E
        mov edi,0Eh
        mov ebp,0Fh
        mov dword ptr [esp+14h],0BF800000h
        jmp plane_prerender_006C9CB0
    plane_prerender_006C9D7E:
        movsx eax,word ptr [esi+22h]
        add eax,0FFFFFE24h
        or ebp,0FFFFFFFFh
        cmp eax,74h
        mov dword ptr [esp+14h],0
        mov dword ptr [esp+1Ch],0
        ja plane_prerender_006C9EE3
        movzx edx,byte ptr [eax+PlanePreRender355SwitchMap]
        jmp dword ptr [edx*4+PlanePreRender355SwitchTargets]
    plane_prerender_006C9DB1:
        fld dword ptr ds:[0087192Ch]
        mov eax,dword ptr ds:[0087192Ch]
        mov edi,1
        fchs
        mov dword ptr [esp+18h],eax
        jmp plane_prerender_006C9E49
    plane_prerender_006C9DCC:
        fld dword ptr ds:[0087192Ch]
        mov edx,dword ptr ds:[00871938h]
        mov ecx,dword ptr ds:[0087192Ch]
        fchs
        mov edi,1
        xor ebp,ebp
        mov dword ptr [esp+14h],edx
        jmp plane_prerender_006C9E45
    plane_prerender_006C9DED:
        mov eax,dword ptr ds:[00871930h]
        fld dword ptr ds:[00871930h]
        mov ecx,dword ptr ds:[00871934h]
        mov edx,dword ptr ds:[00871938h]
        xor edi,edi
        mov dword ptr [esp+18h],eax
        xor ebp,ebp
        mov dword ptr [esp+14h],ecx
        mov dword ptr [esp+1Ch],edx
        jmp plane_prerender_006C9E49
    plane_prerender_006C9E16:
        fld dword ptr ds:[0087193Ch]
        mov eax,dword ptr ds:[0087193Ch]
        fld dword ptr ds:[0087193Ch]
        xor edi,edi
        fchs
        mov dword ptr [esp+18h],eax
        fstp dword ptr [esp+14h]
        xor ebp,ebp
        jmp plane_prerender_006C9E49
    plane_prerender_006C9E37:
        fld dword ptr ds:[00871940h]
        mov ecx,dword ptr ds:[00871940h]
        xor edi,edi
    plane_prerender_006C9E45:
        mov dword ptr [esp+18h],ecx
    plane_prerender_006C9E49:
        fld dword ptr [esi+000009CCh]
        mov edx,dword ptr [esi+0000069Ch]
        fabs
        push 1
        push ecx
        fmul st,st(1)
        mov ecx,esi
        fstp dword ptr [esp]
        push edi
        push edx
        fstp st(0)
        call dword ptr [PlanePreRender355Calls+108]
        fld dword ptr [esi+000009CCh]
        mov eax,dword ptr [esi+000006A0h]
        fabs
        push 1
        push ecx
        fmul dword ptr [esp+20h]
        mov ecx,esi
        fstp dword ptr [esp]
        push edi
        push eax
        call dword ptr [PlanePreRender355Calls+108]
        cmp ebp,0FFFFFFFFh
        jle plane_prerender_006C9EE3
        fld dword ptr [esi+000009CCh]
        push 1
        fabs
        push ecx
        mov ecx,dword ptr [esi+000006A4h]
        fmul dword ptr [esp+1Ch]
        fstp dword ptr [esp]
        push ebp
        push ecx
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+108]
        fld dword ptr [esp+1Ch]
        fcomp qword ptr ds:[00859EF8h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006C9EE3
        fld dword ptr [esi+000009CCh]
        mov edx,dword ptr [esi+000006A8h]
        fabs
        push 1
        push ecx
        fmul dword ptr [esp+24h]
        mov ecx,esi
        fstp dword ptr [esp]
        push ebp
        push edx
        call dword ptr [PlanePreRender355Calls+108]
    plane_prerender_006C9EE3:
        mov ax,word ptr [esi+22h]
        cmp ax,250h
        jne plane_prerender_006C9F1C
        movzx eax,word ptr [esi+0000086Ch]
        mov dword ptr [esp+10h],eax
        push 1
        push ecx
        fild dword ptr [esp+18h]
        mov ecx,dword ptr [esi+000006A8h]
        fmul dword ptr ds:[008D33C0h]
        fstp dword ptr [esp]
        push ebx
        push ecx
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+108]
        jmp plane_prerender_006CA638
    plane_prerender_006C9F1C:
        cmp ax,208h
        jne plane_prerender_006CA2E9
        movzx edx,word ptr [esi+0000086Ch]
        movsx eax,word ptr ds:[008D33C4h]
        mov dword ptr [esp+10h],edx
        mov ecx,dword ptr [esi+00000660h]
        push 1
        fild dword ptr [esp+14h]
        mov dword ptr [esp+14h],eax
        fmul dword ptr ds:[00858FE4h]
        fidiv dword ptr [esp+14h]
        fstp dword ptr [esp+14h]
        mov edi,dword ptr [esp+14h]
        push edi
        push ebx
        push ecx
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+108]
        mov edx,dword ptr [esi+00000654h]
        push 1
        push edi
        push ebx
        push edx
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+108]
        test byte ptr [esi+00000428h],10h
        je plane_prerender_006CA2A0
        fld dword ptr [esi+00000994h]
        movzx eax,word ptr [esi+0000086Ch]
        fadd dword ptr ds:[00858624h]
        movsx ecx,word ptr ds:[008D33C8h]
        cmp eax,ecx
        fmul dword ptr ds:[00858B8Ch]
        fstp dword ptr [esp+18h]
        jl plane_prerender_006C9FBE
        mov edx,dword ptr [esp+18h]
        push 40000000h
        push edx
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+96]
        jmp plane_prerender_006C9FD9
    plane_prerender_006C9FBE:
        mov ecx,dword ptr [esi+00000580h]
        cmp ecx,ebx
        je plane_prerender_006C9FD9
        call dword ptr [PlanePreRender355Calls+8]
        mov dword ptr [esi+00000580h],ebx
        mov dword ptr [esi+00000984h],ebx
    plane_prerender_006C9FD9:
        mov ebx,dword ptr [esi+00000660h]
        mov eax,dword ptr [esi+00000654h]
        add ebx,10h
        add eax,10h
        lea ebp,[esi+000009E8h]
        mov dword ptr [esp+1Ch],eax
        mov edi,ebp
        mov dword ptr [esp+10h],4
    plane_prerender_006CA000:
        mov eax,dword ptr [esi+18h]
        test eax,eax
        je plane_prerender_006CA062
        mov eax,dword ptr [eax+4]
        add eax,10h
        test eax,eax
        je plane_prerender_006CA062
        cmp dword ptr [edi],0
        jne plane_prerender_006CA062
        push 0
        push eax
        lea eax,[esp+34h]
        push eax
        push 871A64h
        mov ecx,0A9AE80h
        mov dword ptr [esp+3Ch],0
        mov dword ptr [esp+40h],0
        mov dword ptr [esp+44h],0
        call dword ptr [PlanePreRender355Calls+0]
        test eax,eax
        mov dword ptr [edi],eax
        je plane_prerender_006CA062
        mov ecx,eax
        call dword ptr [PlanePreRender355Calls+4]
        mov ecx,dword ptr [edi]
        push 1
        call dword ptr [PlanePreRender355Calls+32]
        mov ecx,dword ptr [edi]
        call dword ptr [PlanePreRender355Calls+28]
    plane_prerender_006CA062:
        mov eax,dword ptr [esp+10h]
        add edi,4
        dec eax
        mov dword ptr [esp+10h],eax
        jne plane_prerender_006CA000
        cmp dword ptr [ebp],0
        je plane_prerender_006CA0EF
        mov eax,dword ptr [esi+14h]
        push ebx
        push eax
        lea ecx,[esp+000000DCh]
        push ecx
        mov dword ptr [esp+38h],3F333333h
        mov dword ptr [esp+3Ch],0BEE66666h
        mov dword ptr [esp+40h],3D4CCCCDh
        call dword ptr [PlanePreRender355Calls+72]
        add esp,0Ch
        mov ecx,dword ptr [ebp]
        lea edx,[esp+000000D4h]
        push edx
        mov byte ptr [esp+00000144h],1
        call dword ptr [PlanePreRender355Calls+16]
        mov ecx,dword ptr [ebp]
        lea eax,[esp+2Ch]
        push eax
        call dword ptr [PlanePreRender355Calls+20]
        mov ecx,dword ptr [esp+18h]
        push ecx
        mov ecx,dword ptr [ebp]
        push 1
        call dword ptr [PlanePreRender355Calls+24]
        lea ecx,[esp+000000D4h]
        mov byte ptr [esp+00000140h],0
        call dword ptr [PlanePreRender355Calls+52]
    plane_prerender_006CA0EF:
        mov eax,dword ptr [esi+000009ECh]
        test eax,eax
        je plane_prerender_006CA17B
        mov eax,dword ptr [esi+14h]
        push ebx
        push eax
        lea edx,[esp+000000DCh]
        push edx
        mov dword ptr [esp+38h],0BF51EB85h
        mov dword ptr [esp+3Ch],0BEE66666h
        mov dword ptr [esp+40h],3D4CCCCDh
        call dword ptr [PlanePreRender355Calls+72]
        add esp,0Ch
        mov ecx,dword ptr [esi+000009ECh]
        lea eax,[esp+000000D4h]
        push eax
        mov byte ptr [esp+00000144h],2
        call dword ptr [PlanePreRender355Calls+16]
        lea ecx,[esp+2Ch]
        push ecx
        mov ecx,dword ptr [esi+000009ECh]
        call dword ptr [PlanePreRender355Calls+20]
        mov edx,dword ptr [esp+18h]
        mov ecx,dword ptr [esi+000009ECh]
        push edx
        push 1
        call dword ptr [PlanePreRender355Calls+24]
        lea ecx,[esp+000000D4h]
        mov byte ptr [esp+00000140h],0
        call dword ptr [PlanePreRender355Calls+52]
    plane_prerender_006CA17B:
        mov eax,dword ptr [esi+000009F0h]
        test eax,eax
        je plane_prerender_006CA20B
        mov ecx,dword ptr [esp+1Ch]
        mov eax,dword ptr [esi+14h]
        push ecx
        push eax
        lea edx,[esp+000000DCh]
        push edx
        mov dword ptr [esp+38h],3F2147AEh
        mov dword ptr [esp+3Ch],0BEE66666h
        mov dword ptr [esp+40h],3D8F5C29h
        call dword ptr [PlanePreRender355Calls+72]
        add esp,0Ch
        mov ecx,dword ptr [esi+000009F0h]
        lea eax,[esp+000000D4h]
        push eax
        mov byte ptr [esp+00000144h],3
        call dword ptr [PlanePreRender355Calls+16]
        lea ecx,[esp+2Ch]
        push ecx
        mov ecx,dword ptr [esi+000009F0h]
        call dword ptr [PlanePreRender355Calls+20]
        mov edx,dword ptr [esp+18h]
        mov ecx,dword ptr [esi+000009F0h]
        push edx
        push 1
        call dword ptr [PlanePreRender355Calls+24]
        lea ecx,[esp+000000D4h]
        mov byte ptr [esp+00000140h],0
        call dword ptr [PlanePreRender355Calls+52]
    plane_prerender_006CA20B:
        mov eax,dword ptr [esi+000009F4h]
        test eax,eax
        je plane_prerender_006CA638
        mov ecx,dword ptr [esp+1Ch]
        mov eax,dword ptr [esi+14h]
        push ecx
        push eax
        lea edx,[esp+000000DCh]
        push edx
        mov dword ptr [esp+38h],0BF400000h
        mov dword ptr [esp+3Ch],0BEE66666h
        mov dword ptr [esp+40h],3D8F5C29h
        call dword ptr [PlanePreRender355Calls+72]
        add esp,0Ch
        mov ecx,dword ptr [esi+000009F4h]
        lea eax,[esp+000000D4h]
        push eax
        mov byte ptr [esp+00000144h],4
        call dword ptr [PlanePreRender355Calls+16]
        lea ecx,[esp+2Ch]
        push ecx
        mov ecx,dword ptr [esi+000009F4h]
        call dword ptr [PlanePreRender355Calls+20]
        mov edx,dword ptr [esp+18h]
        mov ecx,dword ptr [esi+000009F4h]
        push edx
        push 1
        call dword ptr [PlanePreRender355Calls+24]
        lea ecx,[esp+000000D4h]
        mov byte ptr [esp+00000140h],0
        call dword ptr [PlanePreRender355Calls+52]
        jmp plane_prerender_006CA638
    plane_prerender_006CA2A0:
        lea edi,[esi+000009E8h]
        mov ebp,4
        jmp plane_prerender_006CA2B0
        lea ecx,[ecx]
    plane_prerender_006CA2B0:
        mov ecx,dword ptr [edi]
        cmp ecx,ebx
        je plane_prerender_006CA2BD
        call dword ptr [PlanePreRender355Calls+8]
        mov dword ptr [edi],ebx
    plane_prerender_006CA2BD:
        add edi,4
        dec ebp
        jne plane_prerender_006CA2B0
        mov ecx,dword ptr [esi+00000580h]
        cmp ecx,ebx
        je plane_prerender_006CA638
        call dword ptr [PlanePreRender355Calls+8]
        xor eax,eax
        mov dword ptr [esi+00000580h],eax
        mov dword ptr [esi+00000984h],eax
        jmp plane_prerender_006CA638
    plane_prerender_006CA2E9:
        cmp ax,21Bh
        jne plane_prerender_006CA638
        mov eax,dword ptr [esi+000006A4h]
        test eax,eax
        je plane_prerender_006CA638
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+48]
        mov edi,dword ptr [eax+2Ch]
        mov ebx,dword ptr [esp+48h]
        push 0
        lea eax,[esp+30h]
        push eax
        push 0
        mov ecx,ebx
        call dword ptr [PlanePreRender355Calls+40]
        mov ecx,dword ptr [esi+000006A4h]
        add ecx,10h
        push 0
        push ecx
        lea ecx,[esp+78h]
        call dword ptr [PlanePreRender355Calls+68]
        mov ecx,dword ptr [esi+14h]
        fld dword ptr [ecx+18h]
        fmul dword ptr [esi+4Ch]
        fld dword ptr [ecx+14h]
        fmul dword ptr [esi+48h]
        faddp st(1),st
        fld dword ptr [ecx+10h]
        fmul dword ptr [esi+44h]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
        fcomp dword ptr ds:[008D3414h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA368
        fld dword ptr ds:[008D3414h]
        jmp plane_prerender_006CA384
    plane_prerender_006CA368:
        fld dword ptr [esi+4Ch]
        fmul dword ptr [ecx+18h]
        fld dword ptr [esi+48h]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fld dword ptr [esi+44h]
        fmul dword ptr [ecx+10h]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
    plane_prerender_006CA384:
        fld dword ptr ds:[008D3414h]
        fchs
        fst dword ptr [esp+1Ch]
        fcomp st(1)
        fnstsw ax
        fstp st(0)
        test ah,41h
        jne plane_prerender_006CA3A8
        mov edx,dword ptr [esp+1Ch]
        mov dword ptr [esp+00000094h],edx
        jmp plane_prerender_006CA402
    plane_prerender_006CA3A8:
        fld dword ptr [esi+4Ch]
        fmul dword ptr [ecx+18h]
        fld dword ptr [esi+48h]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fld dword ptr [esi+44h]
        fmul dword ptr [ecx+10h]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
        fcomp dword ptr ds:[008D3414h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA3DF
        mov eax,dword ptr ds:[008D3414h]
        mov dword ptr [esp+00000094h],eax
        jmp plane_prerender_006CA402
    plane_prerender_006CA3DF:
        fld dword ptr [esi+4Ch]
        fmul dword ptr [ecx+18h]
        fld dword ptr [esi+48h]
        fmul dword ptr [ecx+14h]
        faddp st(1),st
        fld dword ptr [esi+44h]
        fmul dword ptr [ecx+10h]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
        fstp dword ptr [esp+00000094h]
    plane_prerender_006CA402:
        fld dword ptr [esi+4Ch]
        fmul dword ptr [ecx+8]
        fld dword ptr [esi+48h]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [ecx]
        fmul dword ptr [esi+44h]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
        fcomp dword ptr ds:[008D3414h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA432
        fld dword ptr ds:[008D3414h]
        jmp plane_prerender_006CA44D
    plane_prerender_006CA432:
        fld dword ptr [esi+4Ch]
        fmul dword ptr [ecx+8]
        fld dword ptr [esi+48h]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [ecx]
        fmul dword ptr [esi+44h]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
    plane_prerender_006CA44D:
        fld dword ptr [esp+1Ch]
        fcomp st(1)
        fnstsw ax
        fstp st(0)
        test ah,41h
        jne plane_prerender_006CA469
        mov ecx,dword ptr [esp+1Ch]
        mov dword ptr [esp+00000090h],ecx
        jmp plane_prerender_006CA4C2
    plane_prerender_006CA469:
        fld dword ptr [esi+4Ch]
        fmul dword ptr [ecx+8]
        fld dword ptr [esi+48h]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [esi+44h]
        fmul dword ptr [ecx]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
        fcomp dword ptr ds:[008D3414h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA4A0
        mov edx,dword ptr ds:[008D3414h]
        mov dword ptr [esp+00000090h],edx
        jmp plane_prerender_006CA4C2
    plane_prerender_006CA4A0:
        fld dword ptr [esi+4Ch]
        fmul dword ptr [ecx+8]
        fld dword ptr [esi+48h]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [esi+44h]
        fmul dword ptr [ecx]
        faddp st(1),st
        fmul dword ptr ds:[008D3418h]
        fstp dword ptr [esp+00000090h]
    plane_prerender_006CA4C2:
        fld dword ptr ds:[008D3410h]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [PlanePreRender355Calls+140]
        fld dword ptr [esi+00000840h]
        fsub dword ptr [esi+00000844h]
        mov eax,dword ptr [edi+10h]
        fld dword ptr [esi+00000838h]
        fsub dword ptr [esi+0000083Ch]
        faddp st(1),st
        fmul dword ptr ds:[00858B8Ch]
        fld dword ptr [eax+4]
        fsub dword ptr [eax+24h]
        fdivp st(1),st
        fcom dword ptr ds:[008D340Ch]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA515
        fstp st(0)
        fld dword ptr ds:[008D340Ch]
        jmp plane_prerender_006CA532
    plane_prerender_006CA515:
        fld dword ptr ds:[008D340Ch]
        fchs
        fstp dword ptr [esp+10h]
        fcom dword ptr [esp+10h]
        fnstsw ax
        test ah,5
        jp plane_prerender_006CA532
        fstp st(0)
        fld dword ptr [esp+10h]
    plane_prerender_006CA532:
        fld dword ptr ds:[00858624h]
        fsub st,st(2)
        fstp dword ptr [esp+1Ch]
        fld dword ptr [esp+00000088h]
        fmul st,st(2)
        fld dword ptr [esp+1Ch]
        fmul st,st(2)
        faddp st(1),st
        fstp dword ptr [esp+00000088h]
        mov eax,dword ptr [edi+10h]
        fstp st(0)
        fld dword ptr [esi+00000844h]
        fsub dword ptr [esi+0000083Ch]
        fld dword ptr [esi+00000840h]
        fsub dword ptr [esi+00000838h]
        faddp st(1),st
        fmul dword ptr ds:[00858B8Ch]
        fld dword ptr [eax+60h]
        fsub dword ptr [eax+20h]
        fdivp st(1),st
        fcom dword ptr ds:[008D3408h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA59A
        fstp st(0)
        fld dword ptr ds:[008D3408h]
        jmp plane_prerender_006CA5B7
    plane_prerender_006CA59A:
        fld dword ptr ds:[008D3408h]
        fchs
        fstp dword ptr [esp+10h]
        fcom dword ptr [esp+10h]
        fnstsw ax
        test ah,5
        jp plane_prerender_006CA5B7
        fstp st(0)
        fld dword ptr [esp+10h]
    plane_prerender_006CA5B7:
        fld dword ptr [esp+78h]
        fmul st,st(2)
        fld dword ptr [esp+1Ch]
        fmul st,st(2)
        faddp st(1),st
        fstp dword ptr [esp+78h]
        fstp st(0)
        fld dword ptr [esi+00000844h]
        fadd dword ptr [esi+0000083Ch]
        fadd dword ptr [esi+00000840h]
        fadd dword ptr [esi+00000838h]
        fmul dword ptr ds:[00858C84h]
        fsub dword ptr [esp+34h]
        fld dword ptr [ebx+40h]
        fmul dword ptr ds:[00858B8Ch]
        fdivp st(1),st
        fsubr dword ptr ds:[00858624h]
        fcom dword ptr ds:[008D3404h]
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA613
        fstp st(0)
        fld dword ptr ds:[008D3404h]
    plane_prerender_006CA613:
        fld dword ptr [esp+00000098h]
        lea ecx,[esp+70h]
        fmul st,st(2)
        fld dword ptr [esp+1Ch]
        fmul st,st(2)
        faddp st(1),st
        fstp dword ptr [esp+00000098h]
        fstp st(0)
        fstp st(0)
        call dword ptr [PlanePreRender355Calls+64]
    plane_prerender_006CA638:
        lea edi,[esi+000006ACh]
        mov ebx,3
    plane_prerender_006CA643:
        mov ax,word ptr [edi]
        cmp ax,0FFFFh
        jle plane_prerender_006CA6A3
        mov ecx,dword ptr ds:[008D3400h]
        mov edx,dword ptr ds:[008D33FCh]
        push ecx
        push edx
        sub esp,0Ch
        mov edx,esp
        lea ecx,[esi+000008C0h]
        mov ebp,dword ptr [ecx]
        mov dword ptr [edx],ebp
        mov ebp,dword ptr [ecx+4]
        mov ecx,dword ptr [ecx+8]
        mov dword ptr [edx+4],ebp
        mov dword ptr [edx+8],ecx
        sub esp,0Ch
        lea edx,[esi+000008B4h]
        mov ebp,dword ptr [edx]
        mov ecx,esp
        mov dword ptr [ecx],ebp
        mov ebp,dword ptr [edx+4]
        mov edx,dword ptr [edx+8]
        mov dword ptr [ecx+4],ebp
        movsx eax,ax
        mov dword ptr [ecx+8],edx
        mov ecx,dword ptr [esi+eax*4+00000648h]
        push ecx
        push esi
        mov ecx,edi
        call dword ptr [PlanePreRender355Calls+116]
    plane_prerender_006CA6A3:
        add edi,20h
        dec ebx
        jne plane_prerender_006CA643
        fld dword ptr [esi+4Ch]
        lea edx,[esi+000008B4h]
        fadd dword ptr [esi+64h]
        fld dword ptr [esi+48h]
        fadd dword ptr [esi+60h]
        fld dword ptr [esi+44h]
        fadd dword ptr [esi+5Ch]
        fstp dword ptr [esp+2Ch]
        mov eax,dword ptr [esp+2Ch]
        mov dword ptr [edx],eax
        fstp dword ptr [esp+30h]
        mov ecx,dword ptr [esp+30h]
        mov dword ptr [edx+4],ecx
        fstp dword ptr [esp+34h]
        mov eax,dword ptr [esp+34h]
        mov dword ptr [edx+8],eax
        fld dword ptr [esi+58h]
        fadd dword ptr [esi+70h]
        lea ecx,[esi+000008C0h]
        fld dword ptr [esi+54h]
        fadd dword ptr [esi+6Ch]
        fld dword ptr [esi+50h]
        fadd dword ptr [esi+68h]
        fstp dword ptr [esp+2Ch]
        mov edx,dword ptr [esp+2Ch]
        mov dword ptr [ecx],edx
        fstp dword ptr [esp+30h]
        mov eax,dword ptr [esp+30h]
        mov dword ptr [ecx+4],eax
        mov ax,word ptr [esi+22h]
        fstp dword ptr [esp+34h]
        cmp ax,21Bh
        mov edx,dword ptr [esp+34h]
        mov dword ptr [ecx+8],edx
        jne plane_prerender_006CA727
        push 0
        jmp plane_prerender_006CA739
    plane_prerender_006CA727:
        cmp ax,241h
        je plane_prerender_006CA737
        cmp ax,250h
        je plane_prerender_006CA737
        push 3
        jmp plane_prerender_006CA739
    plane_prerender_006CA737:
        push 5
    plane_prerender_006CA739:
        push esi
        call dword ptr [PlanePreRender355Calls+120]
        mov al,byte ptr [esi+0000042Bh]
        add esp,8
        test al,40h
        jne plane_prerender_006CA937
        mov ax,word ptr [esi+22h]
        cmp ax,21Bh
        je plane_prerender_006CA937
        cmp ax,1D0h
        je plane_prerender_006CA937
        mov edi,dword ptr [esi+14h]
        fld dword ptr [esi+4Ch]
        fmul dword ptr [edi+28h]
        mov eax,dword ptr [esi+00000388h]
        fld dword ptr [esi+48h]
        fmul dword ptr [edi+24h]
        faddp st(1),st
        fld dword ptr [edi+20h]
        fmul dword ptr [esi+44h]
        faddp st(1),st
        fmul dword ptr [eax+2Ch]
        fmul dword ptr ds:[00859AECh]
        fabs
        fsub dword ptr ds:[0085950Ch]
        fld dword ptr ds:[00858B50h]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne plane_prerender_006CA7AF
        fstp st(0)
        fld dword ptr ds:[00858B50h]
    plane_prerender_006CA7AF:
        call dword ptr [PlanePreRender355Calls+132]
        mov ecx,dword ptr [esp+48h]
        mov edx,dword ptr [ecx+5Ch]
        add edx,90h
        mov byte ptr [esp+10h],al
        mov eax,dword ptr [edx]
        mov dword ptr [esp+58h],eax
        fld dword ptr [esp+58h]
        mov ecx,dword ptr [edx+4]
        fsub dword ptr ds:[00858B1Ch]
        mov dword ptr [esp+5Ch],ecx
        mov edx,dword ptr [edx+8]
        mov dword ptr [esp+3Ch],eax
        fstp dword ptr [esp+3Ch]
        lea eax,[esp+58h]
        push eax
        mov dword ptr [esp+44h],ecx
        lea ecx,[esp+68h]
        push edi
        push ecx
        mov dword ptr [esp+6Ch],edx
        mov dword ptr [esp+50h],edx
        call dword ptr [PlanePreRender355Calls+84]
        mov eax,dword ptr [esi+14h]
        lea edx,[esp+48h]
        push edx
        push eax
        lea eax,[esp+40h]
        push eax
        call dword ptr [PlanePreRender355Calls+84]
        mov edx,dword ptr [esp+44h]
        mov eax,dword ptr [esp+48h]
        mov edi,dword ptr [esp+28h]
        add esp,0Ch
        mov ecx,esp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+40h]
        mov dword ptr [ecx+4],eax
        sub esp,0Ch
        mov eax,esp
        mov dword ptr [ecx+8],edx
        mov ecx,dword ptr [esp+7Ch]
        mov edx,dword ptr [esp+00000080h]
        push edi
        push 0FFh
        mov dword ptr [eax],ecx
        mov ecx,dword ptr [esp+0000008Ch]
        push 0FFh
        mov dword ptr [eax+4],edx
        push 0FFh
        push esi
        mov dword ptr [eax+8],ecx
        call dword ptr [PlanePreRender355Calls+124]
        fld dword ptr [esp+00000084h]
        mov eax,dword ptr [esi+14h]
        fchs
        lea edx,[esp+00000084h]
        fstp dword ptr [esp+00000084h]
        fld dword ptr [esp+68h]
        push edx
        push eax
        fchs
        lea eax,[esp+000000FCh]
        fstp dword ptr [esp+70h]
        push eax
        call dword ptr [PlanePreRender355Calls+84]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+0000009Ch],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+000000A0h],edx
        mov eax,dword ptr [eax+8]
        lea ecx,[esp+74h]
        mov dword ptr [esp+000000A4h],eax
        mov eax,dword ptr [esi+14h]
        push ecx
        push eax
        lea edx,[esp+00000108h]
        push edx
        call dword ptr [PlanePreRender355Calls+84]
        add esp,44h
        mov ecx,eax
        mov edx,dword ptr [ecx]
        mov dword ptr [esp+2Ch],edx
        mov edx,dword ptr [ecx+4]
        mov dword ptr [esp+30h],edx
        mov ecx,dword ptr [ecx+8]
        sub esp,0Ch
        mov dword ptr [esp+40h],ecx
        mov ecx,dword ptr [eax]
        mov edx,esp
        mov dword ptr [edx],ecx
        mov ecx,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [edx+4],ecx
        sub esp,0Ch
        mov dword ptr [edx+8],eax
        mov edx,dword ptr [esp+7Ch]
        mov eax,dword ptr [esp+00000080h]
        mov ecx,esp
        push edi
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+00000088h]
        push 0FFh
        mov dword ptr [ecx+4],eax
        push 0FFh
        lea eax,[esi+1]
        push 0FFh
        push eax
        mov dword ptr [ecx+8],edx
        call dword ptr [PlanePreRender355Calls+124]
        add esp,2Ch
    plane_prerender_006CA937:
        mov al,byte ptr [esi+00000A00h]
        test al,al
        je plane_prerender_006CAA93
        mov ax,word ptr [esi+22h]
        cmp ax,200h
        jne plane_prerender_006CA9DB
        push 3E4CCCCDh
        push 3F800000h
        push 3F800000h
        push 3ECCCCCDh
        push 3F800000h
        push 3F800000h
        push 3F800000h
        lea ecx,[esp+00000138h]
        call dword ptr [PlanePreRender355Calls+36]
        mov eax,dword ptr [esi+14h]
        lea ecx,[esp+2Ch]
        push ecx
        push eax
        lea edx,[esp+60h]
        push edx
        mov dword ptr [esp+38h],0
        mov dword ptr [esp+3Ch],0BF000000h
        mov dword ptr [esp+40h],0BF000000h
        call dword ptr [PlanePreRender355Calls+84]
        fld dword ptr [esi+4Ch]
        fmul dword ptr ds:[0085862Ch]
        fld dword ptr [esi+48h]
        fmul dword ptr ds:[0085862Ch]
        fld dword ptr [esi+44h]
        fmul dword ptr ds:[0085862Ch]
        fstp dword ptr [esp+48h]
        fstp dword ptr [esp+4Ch]
        fsub dword ptr ds:[00858B3Ch]
        jmp plane_prerender_006CAA5C
    plane_prerender_006CA9DB:
        cmp ax,201h
        jne plane_prerender_006CAA93
        push 3E99999Ah
        push 3F800000h
        push 3F800000h
        push 3ECCCCCDh
        push 0
        push 0
        push 3F800000h
        lea ecx,[esp+00000138h]
        call dword ptr [PlanePreRender355Calls+36]
        mov eax,dword ptr [esi+14h]
        lea ecx,[esp+2Ch]
        push ecx
        push eax
        lea edx,[esp+60h]
        push edx
        mov dword ptr [esp+38h],0
        mov dword ptr [esp+3Ch],0C0A00000h
        mov dword ptr [esp+40h],0
        call dword ptr [PlanePreRender355Calls+84]
        fld dword ptr [esi+4Ch]
        fmul dword ptr ds:[0085862Ch]
        fld dword ptr [esi+48h]
        fmul dword ptr ds:[0085862Ch]
        fld dword ptr [esi+44h]
        fmul dword ptr ds:[0085862Ch]
        fstp dword ptr [esp+48h]
        fstp dword ptr [esp+4Ch]
    plane_prerender_006CAA5C:
        add esp,0Ch
        fstp dword ptr [esp+44h]
        push 0
        push 3F19999Ah
        push 3F99999Ah
        push 0BF800000h
        lea eax,[esp+0000012Ch]
        push eax
        push 0
        lea ecx,[esp+54h]
        push ecx
        mov ecx,dword ptr ds:[00A9AE20h]
        lea edx,[esp+74h]
        push edx
        call dword ptr [PlanePreRender355Calls+12]
    plane_prerender_006CAA93:
        cmp word ptr [esi+22h],1CCh
        jne plane_prerender_006CAAB1
        mov eax,dword ptr [esi+40h]
        test ah,1
        je plane_prerender_006CAAB1
        mov eax,dword ptr [esi+00000950h]
        push eax
        mov ecx,esi
        call dword ptr [PlanePreRender355Calls+112]
    plane_prerender_006CAAB1:
        lea ecx,[esp+70h]
        mov dword ptr [esp+00000140h],0FFFFFFFFh
        call dword ptr [PlanePreRender355Calls+52]
        mov ecx,dword ptr [esp+00000138h]
        pop edi
        pop esi
        pop ebp
        mov dword ptr fs:[00000000h],ecx
        pop ebx
        add esp,134h
        ret
    }
}
#pragma warning(pop)

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
static float PlaneControlBlend355(float oldValue, int32 input, float step, float factor) {
    const float scale = StaticRef<float>(0x858B88);
    float result;
    __asm {
        fild input
        fmul scale
        fsub oldValue
        fmul step
        fmul factor
        fadd oldValue
        fstp result
    }
    return result;
}

static float PlaneControlProductAdd355(float oldValue, float input, float factor) {
    float result;
    __asm {
        fld input
        fmul factor
        fadd oldValue
        fstp result
    }
    return result;
}

static float PlaneForwardSpeed355(const CVector* speed, const CVector* forward) {
    float result;
    __asm {
        mov eax, speed
        mov edx, forward
        fld dword ptr [eax+8]
        fmul dword ptr [edx+8]
        fld dword ptr [eax+4]
        fmul dword ptr [edx+4]
        faddp st(1), st
        fld dword ptr [eax]
        fmul dword ptr [edx]
        faddp st(1), st
        fstp result
    }
    return result;
}

static void PlaneMouseDecay355(float* value) {
    const float base = StaticRef<float>(0x871910), step = CTimer::GetTimeStep();
    const uintptr_t powAddress = 0x822130;
    __asm {
        fld base
        fld step
        mov eax, powAddress
        call eax
        mov eax, value
        fmul dword ptr [eax]
        fstp dword ptr [eax]
    }
}

static void PlaneInputSpeedCap355(CVector* speed) {
    const float limit = StaticRef<float>(0x871254);
    __asm {
        mov eax, speed
        fld dword ptr [eax+8]
        fld dword ptr [eax+4]
        fld dword ptr [eax]
        fld st(0)
        fmul st, st(1)
        fld st(2)
        fmul st, st(3)
        faddp st(1), st
        fld st(3)
        fmul st, st(4)
        faddp st(1), st
        fsqrt
        fstp st(3)
        fstp st(0)
        fstp st(0)
        fcom limit
        fnstsw ax
        test ah, 41h
        jne no_cap
        fdivr limit
        mov eax, speed
        fld st(0)
        fmul dword ptr [eax]
        fstp dword ptr [eax]
        fld st(0)
        fmul dword ptr [eax+4]
        fstp dword ptr [eax+4]
        fmul dword ptr [eax+8]
        fstp dword ptr [eax+8]
        jmp cap_done
    no_cap:
        fstp st(0)
    cap_done:
    }
}


static float PlaneInputSkid355(float oldValue,float target,float step){
 const float factor=StaticRef<float>(0x858CC4),lower=-1.f,upper=1.f;float result;
 __asm {
  fld target
  fsub oldValue
  fmul step
  fmul factor
  fadd oldValue
  fcom lower
  fnstsw ax
  test ah,41h
  jne skid_lower
  fcom upper
  fnstsw ax
  test ah,5h
  jp skid_upper
  jmp skid_store
 skid_lower:
  fcom lower
  fnstsw ax
  test ah,41h
  je skid_store
  fstp st(0)
  fld lower
  jmp skid_store
 skid_upper:
  fstp st(0)
  fld upper
 skid_store:
  fstp result
 }
 return result;
}
static float PlaneGroundSteer355(float speed,float degrees,float skid){
 const float maximum=StaticRef<float>(0x858B1C),one=1.f,radians=StaticRef<float>(0x8595EC);float result;
 __asm {
  fld one
  fsub speed
  fld maximum
  fcomp st(1)
  fnstsw ax
  test ah,41h
  jne ground_angle
  fstp st(0)
  fld maximum
 ground_angle:
  fld degrees
  fmul radians
  fmul skid
  fmulp st(1),st
  fchs
  fstp result
 }
 return result;
}
static int16 PlaneNozzleChange355(int32 axis,float step){
 const float rate=StaticRef<float>(0x8D33DC),scale=StaticRef<float>(0x858B88);const uintptr_t truncate=0x821B40;int32 result;
 __asm {
  fild axis
  fmul step
  fmul rate
  fmul scale
  mov eax,truncate
  call eax
  mov result,eax
 }
 return int16(result);
}

static bool PlaneGearAdvance355(float* gear,float step,float comparisonLimit){
 const float factor=StaticRef<float>(0x871904);uint16 status;
 __asm {
  mov edx,gear
  fld step
  fmul factor
  fadd dword ptr [edx]
  fst dword ptr [edx]
  fcomp comparisonLimit
  fnstsw ax
  mov status,ax
 }
 return !(status&0x100);
}
static bool PlaneSpeedBelow355(const CVector* speed,float comparisonLimit){
 uint16 status;
 __asm {
  mov edx,speed
  fld dword ptr [edx+8]
  fld dword ptr [edx+4]
  fld dword ptr [edx]
  fld st(0)
  fmul st,st(1)
  fld st(2)
  fmul st,st(3)
  faddp st(1),st
  fld st(3)
  fmul st,st(4)
  faddp st(1),st
  fsqrt
  fstp st(3)
  fstp st(0)
  fstp st(0)
  fcomp comparisonLimit
  fnstsw ax
  mov status,ax
 }
 return (status&0x500)==0x100;
}
static float PlaneGearResistance355(float drag){
 const float divisor=StaticRef<float>(0x871900),factor=StaticRef<float>(0x858B8C);float result;
 __asm {fld drag
 fdiv divisor
 fmul factor
 fstp result}
 return result;
}
void CPlane::ProcessControlInputs(uint8 playerNum){
 auto* bytes=reinterpret_cast<uint8*>(this);
 const float forwardSpeed=PlaneForwardSpeed355(&m_vecMoveSpeed,&m_matrix->GetForward());
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
   (void)(void)CPad::GetPad(playerNum);m_fSteeringLeftRight=PlaneControlProductAdd355(m_fSteeringLeftRight,mouseX,StaticRef<float>(0x871908));
   (void)(void)CPad::GetPad(playerNum);m_fSteeringUpDown=PlaneControlProductAdd355(m_fSteeringUpDown,mouseY,StaticRef<float>(0x871908));
  }
  if(std::abs(m_fSteeringLeftRight)<StaticRef<float>(0x87190C))PlaneMouseDecay355(&m_fSteeringLeftRight);
  if(std::abs(m_fSteeringUpDown)<StaticRef<float>(0x87190C))PlaneMouseDecay355(&m_fSteeringUpDown);
 }else if(keyboard){
  mode=0;float step=CTimer::GetTimeStep();const int32 lr=CPad::GetPad(playerNum)->GetSteeringLeftRight();
  m_fSteeringLeftRight=PlaneControlBlend355(m_fSteeringLeftRight,lr,step,StaticRef<float>(0x858CC4));
  step=CTimer::GetTimeStep();const int32 ud=CPad::GetPad(playerNum)->GetSteeringUpDown();
  m_fSteeringUpDown=PlaneControlBlend355(m_fSteeringUpDown,ud,step,StaticRef<float>(0x858CC4));
 }
 auto clamp=[](float value){return value>-1.f?(value<1.f?value:1.f):-1.f;};
 m_fSteeringLeftRight=clamp(m_fSteeringLeftRight);m_fSteeringUpDown=clamp(m_fSteeringUpDown);
 if(m_nModelIndex==539){
  const float step=CTimer::GetTimeStep();const int32 axis=CPad::GetPad(playerNum)->GetSteeringLeftRight();
  const float target=float(axis)*StaticRef<float>(0x858B88);m_fLeftRightSkid=PlaneInputSkid355(m_fLeftRightSkid,target,step);
 }else{
  const float right=float(uint8(CPad::GetPad(playerNum)->GetLookRight()));const bool left=CPad::GetPad(playerNum)->GetLookLeft();
  m_fLeftRightSkid=PlaneInputSkid355(m_fLeftRightSkid,left?-1.f:right,CTimer::GetTimeStep());
 }
 m_fSteerAngle=PlaneGroundSteer355(forwardSpeed,m_pHandlingData->m_fSteeringLock,m_fLeftRightSkid);
 if(m_nModelIndex==539&&CPad::GetPad(playerNum)->GetHandBrake()){
  const float multiplier=StaticRef<float>(0x858CE8);m_fSteerAngle*=multiplier;m_fLeftRightSkid*=multiplier;
 }
 const int32 accelerator=CPad::GetPad(playerNum)->GetAccelerate();const int32 brake=CPad::GetPad(playerNum)->GetBrake();
 m_fAccelerationBreakStatus=float(accelerator-brake)*StaticRef<float>(0x859A3C);
 auto* pad=CPad::GetPad(playerNum);auto* padBytes=reinterpret_cast<uint8*>(pad);
 const bool toggle=*reinterpret_cast<int16*>(padBytes+0x26)!=0&&*reinterpret_cast<int16*>(padBytes+0x56)==0;
 const auto* flying=reinterpret_cast<const uint8*>(m_pFlyingHandlingData);
 if(toggle&&std::ranges::all_of(m_fWheelsSuspensionCompression,[](float v){return std::bit_cast<uint32>(v)==0x3F800000u;})&&*reinterpret_cast<const float*>(flying+0x30)<1.f&&m_nModelIndex!=464){
  if(m_fLandingGearStatus==0.f){bytes[0x5A5]=bytes[0x5A6]=bytes[0x5A7]=bytes[0x5A8]=2;m_fLandingGearStatus=PlaneControlProductAdd355(0.f,CTimer::GetTimeStep(),StaticRef<float>(0x871904));}
  else if(std::bit_cast<uint32>(m_fLandingGearStatus)==0x3F800000u)m_fLandingGearStatus=PlaneControlProductAdd355(-1.f,CTimer::GetTimeStep(),StaticRef<float>(0x871904));
  else m_fLandingGearStatus=-m_fLandingGearStatus;
 }else if(m_fLandingGearStatus<0.f){
  const bool complete=PlaneGearAdvance355(&m_fLandingGearStatus,CTimer::GetTimeStep(),0.f);
  if(complete){m_fLandingGearStatus=0.f;bytes[0x5A5]=bytes[0x5A6]=bytes[0x5A7]=bytes[0x5A8]=0;m_fAirResistance=PlaneGearResistance355(m_pHandlingData->m_fDragMult);}
 }else if(m_fLandingGearStatus>0.f&&m_fLandingGearStatus<1.f){
  if(PlaneGearAdvance355(&m_fLandingGearStatus,CTimer::GetTimeStep(),1.f))SetGearUp();
 }
 if(m_nModelIndex==520&&std::abs(CPad::GetPad(playerNum)->GetCarGunUpDown())>StaticRef<float>(0x85862C)){
  auto& nozzle=*reinterpret_cast<uint16*>(bytes+0x86C);*reinterpret_cast<uint16*>(bytes+0x86E)=nozzle;
  float step=CTimer::GetTimeStep();const int32 firstAxis=CPad::GetPad(playerNum)->GetCarGunUpDown();const int32 first=int32(nozzle)+PlaneNozzleChange355(firstAxis,step);
  if(first<0)nozzle=0;else{step=CTimer::GetTimeStep();const int32 secondAxis=CPad::GetPad(playerNum)->GetCarGunUpDown();nozzle=uint16(int32(nozzle)+PlaneNozzleChange355(secondAxis,step));}
  const int32 limit=int16(HARRIER_NOZZLE_ROTATE_LIMIT);if(int32(nozzle)>limit)nozzle=uint16(limit);
 }
 bytes[0x428]&=0xDF;m_GasPedal=0.f;
 if(forwardSpeed>0.f&&CPad::GetPad(playerNum)->GetBrake()!=0){const int32 input=CPad::GetPad(playerNum)->GetBrake();m_BrakePedal=float(input)*StaticRef<float>(forwardSpeed>StaticRef<float>(0x858F9C)?0x871A70:0x859A3C);}
 else if(PlaneSpeedBelow355(&m_vecMoveSpeed,StaticRef<float>(0x858B1C))&&CPad::GetPad(playerNum)->GetBrake()<10&&CPad::GetPad(playerNum)->GetAccelerate()<10)m_BrakePedal=.5f;
 else m_BrakePedal=0.f;
 if(CPad::GetPad(playerNum)->DisablePlayerControls){bytes[0x428]|=0x20;m_BrakePedal=1.f;m_GasPedal=0.f;FindPlayerPed(-1)->KeepAreaAroundPlayerClear();PlaneInputSpeedCap355(&m_vecMoveSpeed);}
}

// 0x6CB7C0

static uint32 PlaneWaveAdvance355(){
 const float spread=StaticRef<float>(0x8D33E0),one=StaticRef<float>(0x858624),randomScale=StaticRef<float>(0x858C7C),timeScale=StaticRef<float>(0x858B38),countScale=StaticRef<float>(0x858C4C),unsignedAdjustment=StaticRef<float>(0x858C54);const double rangeBase=StaticRef<double>(0x85A310);const uintptr_t truncate=0x821B40,randomFunction=0x821B1E;float upper,lower;int32 random;uint32 result;
 __asm {
  fld spread
  fadd rangeBase
  fstp upper
  fld one
  fsub spread
  fstp lower
  mov eax,randomFunction
  call eax
  mov random,eax
  fild random
  fmul randomScale
  fld upper
  fsub lower
  fmul st,st(1)
  fadd lower
  fld dword ptr ds:[0B7CB5Ch]
  fmul timeScale
  fmul countScale
  mov eax,truncate
  call eax
  mov result,eax
  fild result
  cmp result,0
  jge unsigned_wave_count
  fadd unsignedAdjustment
 unsigned_wave_count:
  fmulp st(1),st
  mov eax,truncate
  call eax
  fstp st(0)
  mov result,eax
 }
 return result;
}
static float PlaneDamageSpeed355(const CVector* speed){
 const float scale=StaticRef<float>(0x858B3C),one=1.f;float result;
 __asm {
  mov edx,speed
  fld dword ptr [edx+8]
  fld dword ptr [edx+4]
  fld dword ptr [edx]
  fld st(0)
  fmul st,st(1)
  fld st(2)
  fmul st,st(3)
  faddp st(1),st
  fld st(3)
  fmul st,st(4)
  faddp st(1),st
  fsqrt
  fstp st(3)
  fstp st(0)
  fstp st(0)
  fmul scale
  fcom one
  fnstsw ax
  test ah,41h
  jne plane_speed_store
  fstp st(0)
  fld one
 plane_speed_store:
  fstp result
 }
 return result;
}
static float PlaneDamageNoise355(float oldValue,float amplitude,int32 random,float squaredDamage,float step,float speed){
 const float randomScale=StaticRef<float>(0x858C7C);float result;
 __asm {
  fild random
  fmul randomScale
  fld amplitude
  fadd amplitude
  fmulp st(1),st
  fsub amplitude
  fmul squaredDamage
  fmul step
  fmul speed
  fadd oldValue
  fstp result
 }
 return result;
}
static float PlanePropellerWave355(float oldValue,float upper,float lower,int32 random,int32 damage,uint32 wave,uint32 period,float step){
 const float randomScale=StaticRef<float>(0x858C7C),circle=StaticRef<float>(0x858CBC),unsignedAdjustment=StaticRef<float>(0x858C54),one=1.f,amount=StaticRef<float>(0x8D3444);float result;
 __asm {
  fild random
  fmul randomScale
  fild damage
  fild wave
  cmp wave,0
  jge unsigned_wave
  fadd unsignedAdjustment
 unsigned_wave:
  fmul circle
  fild period
  cmp period,0
  jge unsigned_period
  fadd unsignedAdjustment
 unsigned_period:
  fdivp st(1),st
  fsin
  fsub one
  fld upper
  fsub lower
  fmul st,st(3)
  fadd lower
  fmulp st(1),st
  fmul st,st(1)
  fmul st,st(1)
  fmul step
  fmul amount
  fadd oldValue
  fstp result
  fstp st(0)
  fstp st(0)
 }
 return result;
}

static float PlaneFlightDecay355(float oldValue,int32 damage,bool storeFactor){
 const float scale=StaticRef<float>(0x858CC4),one=1.f;float decay,result;
 __asm {
  fild damage
  fmul scale
  cmp storeFactor,0
  je flight_decay_extended
  fstp decay
  fld decay
 flight_decay_extended:
  fsubr one
  fmul oldValue
  fstp result
 }
 return result;
}
static float PlaneFlightProductAdd355(float value,float a,float b,bool subtract){
 float result;
 __asm {
  fld a
  fmul b
  cmp subtract,0
  je flight_add_product
  fsubr value
  jmp flight_product_store
 flight_add_product:
  fadd value
 flight_product_store:
  fstp result
 }
 return result;
}
static float PlaneFloatFlightBlend355(float value,float target,float step,float factor){
 float result;
 __asm {
  fld target
  fsub value
  fmul step
  fmul factor
  fadd value
  fstp result
 }
 return result;
}
static bool PlaneFlightDistanceBelow355(const CVector* first,const CVector* second,float threshold){
 uint16 comparison;
 __asm {
  mov eax,first
  mov edx,second
  fld dword ptr [eax]
  fsub dword ptr [edx]
  fld dword ptr [eax+4]
  fsub dword ptr [edx+4]
  fld dword ptr [eax+8]
  fsub dword ptr [edx+8]
  fld st(0)
  fmul st,st(1)
  fld st(2)
  fmul st,st(3)
  faddp st(1),st
  fld st(3)
  fmul st,st(4)
  faddp st(1),st
  fcomp threshold
  fstp st(0)
  fnstsw ax
  fstp st(0)
  fstp st(0)
  mov comparison,ax
 }
 return (comparison&0x500)==0x100;
}
static bool PlaneFlightRollCompare355(CPlane* plane,float threshold,bool greater){
 const uintptr_t roll=0x420B30;uint16 comparison;
 __asm {
  mov ecx,plane
  mov eax,roll
  call eax
  fabs
  fcomp threshold
  fnstsw ax
  mov comparison,ax
 }
 return greater?!(comparison&0x4100):(comparison&0x500)==0x100;
}
static void PlaneFlightCeiling355(CPlane* plane,int32 model,float* target){
 const uintptr_t ceiling=0x6D2600;const float zero=0.f,lower=-1.f,scale=StaticRef<float>(0x858B38),adjust=StaticRef<float>(0x858CEC);
 __asm {
  mov ecx,plane
  mov eax,dword ptr [ecx+14h]
  test eax,eax
  je ceiling_first_placement
  add eax,30h
  jmp ceiling_first_ready
 ceiling_first_placement:
  lea eax,[ecx+4]
 ceiling_first_ready:
  push model
  push dword ptr [eax+8]
  mov eax,ceiling
  call eax
  fcomp zero
  fnstsw ax
  test ah,41h
  jne ceiling_done
  mov ecx,plane
  mov eax,dword ptr [ecx+14h]
  test eax,eax
  je ceiling_second_placement
  add eax,30h
  jmp ceiling_second_ready
 ceiling_second_placement:
  lea eax,[ecx+4]
 ceiling_second_ready:
  push model
  push dword ptr [eax+8]
  mov eax,ceiling
  call eax
  fmul scale
  fld zero
  fcomp st(1)
  fnstsw ax
  test ah,41h
  jne ceiling_target_multiply
  fstp st(0)
  fld zero
 ceiling_target_multiply:
  mov edx,target
  fmul dword ptr [edx]
  fstp dword ptr [edx]
  mov ecx,plane
  mov eax,dword ptr [ecx+14h]
  test eax,eax
  je ceiling_third_placement
  add eax,30h
  jmp ceiling_third_ready
 ceiling_third_placement:
  lea eax,[ecx+4]
 ceiling_third_ready:
  push model
  push dword ptr [eax+8]
  mov eax,ceiling
  call eax
  fmul adjust
  mov edx,plane
  fsubr dword ptr [edx+994h]
  fld lower
  fcomp st(1)
  fnstsw ax
  test ah,41h
  jne ceiling_acceleration_store
  fstp st(0)
  fld lower
 ceiling_acceleration_store:
  mov edx,plane
  fstp dword ptr [edx+994h]
 ceiling_done:
 }
}

static float PlaneFlightBound355(float spread,bool subtract){
 const float one=1.f;const double extendedOne=1.;float result;
 __asm {
  cmp subtract,0
  jne flight_bound_subtract
  fld spread
  fadd extendedOne
  jmp flight_bound_store
 flight_bound_subtract:
  fld one
  fsub spread
 flight_bound_store:
  fstp result
 }
 return result;
}
static void PlaneFlightParticleVelocity355(CVector* result,const CVector* speed){
 const float quarter=StaticRef<float>(0x858C84),multiplier=StaticRef<float>(0x858B40);float roundedZ;
 __asm {
  mov edx,speed
  mov eax,result
  fld dword ptr [edx]
  fmul quarter
  fmul multiplier
  fstp dword ptr [eax]
  fld dword ptr [edx+4]
  fmul quarter
  fmul multiplier
  fstp dword ptr [eax+4]
  fld dword ptr [edx+8]
  fmul quarter
  fstp roundedZ
  fld roundedZ
  fmul multiplier
  fstp dword ptr [eax+8]
 }
}
static float PlaneFlightVelocityNoise355(float value,int32 random){
 const float randomScale=StaticRef<float>(0x858C7C),spread=StaticRef<float>(0x871A74),base=StaticRef<float>(0x858C20);float result;
 __asm {
  fild random
  fmul randomScale
  fmul spread
  fadd base
  fmul value
  fstp result
 }
 return result;
}
static float PlaneFlightPositionNoise355(float value,float strength,int32 random){
 const float randomScale=StaticRef<float>(0x858C7C);float negative=-strength,result;
 __asm {
  fild random
  fmul randomScale
  fld strength
  fsub negative
  fmul st,st(1)
  fadd negative
  fadd value
  fstp result
  fstp st(0)
 }
 return result;
}
static float PlaneFlightPropellerTarget355(float acceleration){
 const float standard=StaticRef<float>(0x8D33D0),maximum=StaticRef<float>(0x8D33D4),minimum=StaticRef<float>(0x8D33CC),zero=0.f;float result=standard;
 __asm {
  fld acceleration
  fcomp zero
  fnstsw ax
  test ah,41h
  jne flight_target_negative
  fld maximum
  fsub standard
  jmp flight_target_blend
 flight_target_negative:
  fld acceleration
  fcomp zero
  fnstsw ax
  test ah,5
  jp flight_target_done
  fld standard
  fsub minimum
 flight_target_blend:
  fmul acceleration
  fadd standard
  fstp result
 flight_target_done:
 }
 return result;
}
static bool PlaneFlightSpeedSquaredAbove355(const CVector* speed){
 const double threshold=StaticRef<double>(0x859068);uint16 comparison;
 __asm {
  mov edx,speed
  fld dword ptr [edx+8]
  fld dword ptr [edx+4]
  fld dword ptr [edx]
  fld st(0)
  fmul st,st(1)
  fld st(2)
  fmul st,st(3)
  faddp st(1),st
  fld st(3)
  fmul st,st(4)
  faddp st(1),st
  fcomp threshold
  fstp st(0)
  fnstsw ax
  fstp st(0)
  fstp st(0)
  mov comparison,ax
 }
 return !(comparison&0x4100);
}

static float PlaneFlightNormalizeRandom355(int32 randomValue) {
    float result;
    __asm {
        fild randomValue
        fmul dword ptr ds:[00858C7Ch]
        fstp result
    }
    return result;
}

void CPlane::ProcessFlyingCarStuff() {
    if (CTimer::GetTimeStep() <= 0.f) return;
    auto* bytes = reinterpret_cast<uint8*>(this);
    auto field = [bytes](size_t offset) -> float& { return *reinterpret_cast<float*>(bytes + offset); };
    auto status = [bytes] { return bytes[0x36] >> 3; };
    m_planeDamageWave = std::bit_cast<int32>(uint32(m_planeDamageWave) + PlaneWaveAdvance355());
    const float speed = PlaneDamageSpeed355(&m_vecMoveSpeed);
    float skid = m_fLeftRightSkid, up = m_fSteeringUpDown, lateral = m_fSteeringLeftRight;
    if (m_fHealth < StaticRef<float>(0x859F80) && status() == 0) {
        m_damageManager.SetAeroplaneCompStatus(12, 2);
        m_damageManager.SetAeroplaneCompStatus(13, 2);
        skid += .5f;
        if (PlaneFlightRollCompare355(this, StaticRef<float>(0x86DAF4), false)) lateral += .75f;
        if (PlaneFlightRollCompare355(this, StaticRef<float>(0x858FE4), true)) up += .5f;
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
                    const float upper = PlaneFlightBound355(spread, false), lower = PlaneFlightBound355(spread, true);
                    const int32 random = CGeneral::GetRandomNumber();
                    m_fAccelerationBreakStatus = PlanePropellerWave355(m_fAccelerationBreakStatus, upper, lower,
                        random, damage, uint32(m_planeDamageWave), StaticRef<uint32>(0x8D3448), CTimer::GetTimeStep());
                }
                strength = float(damage) * .5f;
            } else if (node <= 20) {
                const size_t axis = node == 16 ? 0x988 : node <= 18 ? 0x98C : 0x990;
                const uintptr_t amplitudeAddress = node == 16 ? 0x8D3440 : node <= 18 ? 0x8D3434 : 0x8D3428;
                const uintptr_t panelAddress = node == 16 ? 0x8D343C : node <= 18 ? 0x8D3430 : 0x8D3424;
                const uintptr_t feedbackAddress = node == 16 ? 0x8D3438 : node <= 18 ? 0x8D342C : 0x8D3420;
                const float amplitude = StaticRef<float>(amplitudeAddress);
                field(axis) = PlaneFlightDecay355(field(axis), damage, node == 16);
                const int32 random = CGeneral::GetRandomNumber();
                const float squaredDamage = float(std::bit_cast<int32>(uint32(damage) * uint32(damage)));
                field(axis) = PlaneDamageNoise355(field(axis), amplitude, random, squaredDamage, CTimer::GetTimeStep(), speed);
                float& local = node == 16 ? skid : node <= 18 ? up : lateral;
                local = field(axis);
                if (panel >= 0) {
                    const float panelAmplitude = StaticRef<float>(panelAddress);
                    const int32 panelRandom = CGeneral::GetRandomNumber();
                    auto& rotationSpeed = field(0x6C4 + panel * 0x20);
                    rotationSpeed = PlaneDamageNoise355(rotationSpeed, panelAmplitude, panelRandom,
                        squaredDamage, CTimer::GetTimeStep(), speed);
                    local = PlaneFlightProductAdd355(local, StaticRef<float>(feedbackAddress), field(0x6B8 + panel * 0x20), node != 19);
                }
                strength = float(damage) * StaticRef<float>(node == 16 ? 0x858CC4 : node <= 18 ? 0x858FCC : 0x858C84);
            } else continue;
            if (strength > 0.f && !(bytes[0x42B] & 0x40) && (speed > StaticRef<float>(0x858C24) || (CGeneral::GetRandomNumber() & 7) == 0)) {
                auto* cameraMatrix = StaticRef<CMatrix*>(0xB6F03C);
                const CVector* cameraPosition = cameraMatrix ? &cameraMatrix->GetPosition() : &StaticRef<CVector>(0xB6F02C);
                if ((PlaneFlightDistanceBelow355(&GetPosition(), cameraPosition, StaticRef<float>(0x859AEC)) || status() == 0) && m_aCarNodes[node]) {
                    CVector point;
                    reinterpret_cast<CVector*(__cdecl*)(CVector*, const CMatrix*, const CVector*)>(0x59C890)(
                        &point, m_matrix, reinterpret_cast<const CVector*>(&m_aCarNodes[node]->modelling.pos));
                    FxPrtMult_c mults{0.f, 0.f, 0.f, .2f, 1.f, 1.f, strength};
                    CVector velocity;
                    PlaneFlightParticleVelocity355(&velocity, &m_vecMoveSpeed);
                    velocity.x = PlaneFlightVelocityNoise355(velocity.x, CGeneral::GetRandomNumber());
                    velocity.y = PlaneFlightVelocityNoise355(velocity.y, CGeneral::GetRandomNumber());
                    velocity.z = PlaneFlightVelocityNoise355(velocity.z, CGeneral::GetRandomNumber());
                    mults.m_fLife = PlaneFlightNormalizeRandom355(CGeneral::GetRandomNumber()); // EXE overwrites multiplier life at stack+0x98, 0x6CC003.
                    point.x = PlaneFlightPositionNoise355(point.x, strength, CGeneral::GetRandomNumber());
                    point.y = PlaneFlightPositionNoise355(point.y, strength, CGeneral::GetRandomNumber());
                    point.z = PlaneFlightPositionNoise355(point.z, strength, CGeneral::GetRandomNumber());
                    g_fx.m_SmokeHuge->AddParticle(point, velocity, 0.f, mults, -1.f, 1.2f, .6f, false);
                }
            }
        }
    }
    const uint8 vehicleStatus = status();
    if (vehicleStatus != 0 && vehicleStatus != 8 && vehicleStatus != 3) {
        const float decrement = CTimer::GetTimeStep() * StaticRef<float>(0x858CDC);
        if (m_fPropSpeed > decrement) {
            if (m_fPropSpeed > StaticRef<float>(0x8D33D0)) m_fPropSpeed = PlaneFlightProductAdd355(m_fPropSpeed, CTimer::GetTimeStep(), StaticRef<float>(0x859CD8), true);
            else m_fPropSpeed -= decrement;
            bytes[0xB8] = 0;
        } else { m_fPropSpeed = 0.f; bytes[0x428] &= 0xEF; }
        return;
    }
    float target = PlaneFlightPropellerTarget355(m_fAccelerationBreakStatus);
    if (vehicleStatus == 0 || vehicleStatus == 8) PlaneFlightCeiling355(this, m_nModelIndex == 464 ? 1 : 3, &target);
    const bool drowned = (bytes[0x42B] & 0x40) != 0;
    m_fPropSpeed = PlaneFloatFlightBlend355(m_fPropSpeed, target, CTimer::GetTimeStep(), StaticRef<float>(0x8D33D8));
    if (drowned) { bytes[0x428] &= 0xEF; m_fPropSpeed = m_fAccelerationBreakStatus = 0.f; }
    if (m_nModelIndex == 464) {
        if (!drowned && (bytes[0x428] & 0x10)) FlyingControl(eFlightModel(1), skid, up, lateral, m_fAccelerationBreakStatus);
        return;
    }
    if (!(bytes[0x428] & 0x10)) return;
    if (!(m_fPropSpeed > StaticRef<float>(0x8D33CC)) && !PlaneFlightSpeedSquaredAbove355(&m_vecMoveSpeed)) return;
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
