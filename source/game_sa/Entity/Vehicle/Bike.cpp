/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBike
 * Reconstrucción: COMPLETA — 40/40 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 18.
 * Validación registrada: 36/40 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   BlowUpCar | 0x00000000 | V
 *   BurstTyre | 0x00000000 | V
 *   DoBurstAndSoftGroundRatios | 0x00000000 | V
 *   DoSoftGroundResistance | 0x006B6D40 | S
 *   FixHandsToBars | 0x006B7F90 | S
 *   GetCorrectedWorldDoorPosition | 0x006BF230 | S
 *   PlaceOnRoadProperly | 0x006BEEB0 | S
 *   PlayCarHorn | 0x00000000 | V
 *   PreRender | 0x00000000 | V
 *   ProcessAI | 0x00000000 | V
 *   ProcessControl | 0x00000000 | V
 *   ProcessControlInputs | 0x00000000 | V
 *   ProcessRiderAnims | 0x006B7280 | S
 *   ResetSuspension | 0x006B6740 | S
 *   SetRemoveAnimFlags | 0x006B5F50 | S
 *   SetUpWheelColModel | 0x00000000 | V
 *   SetupSuspensionLines | 0x00000000 | V
 *   VehicleDamage | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Bike.h"

#include "Buoyancy.h"
#include "Shadows.h"
#include "Events/EventKnockOffBike.h"
#include "Events/EventGroup.h"
#include "Animation/AnimBlendHierarchy.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimBlendFrameData.h"



void CBike::InjectHooks() {
    RH_ScopedVirtualClass(CBike, 0x871360, 67);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6BF430);
    RH_ScopedInstall(Destructor, 0x6B57A0);
    RH_ScopedInstall(dmgDrawCarCollidingParticles, 0x6B5A00);
    RH_ScopedInstall(DamageKnockOffRider, 0x6B5A10);
    RH_ScopedInstall(KnockOffRider, 0x6B5F40);
    RH_ScopedInstall(SetRemoveAnimFlags, 0x6B5F50);
    RH_ScopedInstall(ReduceHornCounter, 0x6B5F90);
    RH_ScopedVMTInstall(ProcessAI, 0x6BC930);
    RH_ScopedInstall(ProcessBuoyancy, 0x6B5FB0);
    RH_ScopedInstall(ResetSuspension, 0x6B6740);
    RH_ScopedInstall(GetAllWheelsOffGround, 0x6B6790);
    RH_ScopedInstall(DebugCode, 0x6B67A0);
    RH_ScopedInstall(DoSoftGroundResistance, 0x6B6D40);
    RH_ScopedInstall(PlayHornIfNecessary, 0x6B7130);
    RH_ScopedInstall(CalculateLeanMatrix, 0x6B7150);
    RH_ScopedInstall(ProcessRiderAnims, 0x6B7280);
    RH_ScopedInstall(FixHandsToBars, 0x6B7F90);
    RH_ScopedInstall(PlaceOnRoadProperly, 0x6BEEB0);
    RH_ScopedInstall(GetCorrectedWorldDoorPosition, 0x6BF230);
    RH_ScopedVMTInstall(Fix, 0x6B7050);
    RH_ScopedVMTInstall(BlowUpCar, 0x6BEA10);
    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6BF400);
    RH_ScopedVMTInstall(BurstTyre, 0x6BEB20);
    RH_ScopedVMTInstall(ProcessControlInputs, 0x6BE310);
    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6BDEA0);
    RH_ScopedVMTInstall(Render, 0x6BDE20);
    RH_ScopedVMTInstall(PreRender, 0x6BD090);
    RH_ScopedVMTInstall(Teleport, 0x6BCFC0);
    RH_ScopedVMTInstall(ProcessControl, 0x6B9250);
    RH_ScopedVMTInstall(VehicleDamage, 0x6B8EC0);
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6B89B0);
    RH_ScopedVMTInstall(SetModelIndex, 0x6B8970);
    RH_ScopedVMTInstall(PlayCarHorn, 0x6B7080);
    RH_ScopedVMTInstall(SetupDamageAfterLoad, 0x6B7070);
    RH_ScopedVMTInstall(DoBurstAndSoftGroundRatios, 0x6B6950);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6B67E0);
    RH_ScopedVMTInstall(RemoveRefsToVehicle, 0x6B67B0);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6B6620);
    RH_ScopedVMTInstall(GetComponentWorldPosition, 0x6B5990);
    RH_ScopedVMTInstall(ProcessOpenDoor, 0x6B58D0);
}

// 0x6BF430
CBike::CBike(int32 modelIndex, eVehicleCreatedBy createdBy) : CVehicle(createdBy) {
    auto mi = CModelInfo::GetModelInfo(modelIndex)->AsVehicleModelInfoPtr();
    if (mi->m_nVehicleType == VEHICLE_TYPE_BIKE) {
        const auto& animationStyle = CAnimManager::GetAnimBlocks()[mi->GetAnimFileIndex()].GroupId;
        m_RideAnimData.AnimGroup = animationStyle;
        if (animationStyle < ANIM_GROUP_BIKES || animationStyle > ANIM_GROUP_WAYFARER) {
            m_RideAnimData.AnimGroup = ANIM_GROUP_BIKES;
        }
    }

    m_nVehicleSubType = VEHICLE_TYPE_BIKE;
    m_nVehicleType = VEHICLE_TYPE_BIKE;

    m_BlowUpTimer = 0.0f;
    m_nBrakesOn = false;
    nBikeFlags = 0;
    SetModelIndex(modelIndex);

    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(mi->m_nHandlingId);
    m_BikeHandling = gHandlingDataMgr.GetBikeHandlingPointer(mi->m_nHandlingId);
    m_nHandlingFlagsIntValue = m_pHandlingData->m_nHandlingFlags;
    m_pFlyingHandlingData = gHandlingDataMgr.GetFlyingPointer(static_cast<uint8>(mi->m_nHandlingId));
    m_fBrakeCount = 20.0f;
    mi->ChooseVehicleColour(m_nPrimaryColor, m_nSecondaryColor, m_nTertiaryColor, m_nQuaternaryColor, 1);
    m_fSwingArmLength = 0.0f;
    m_fForkYOffset = 0.0f;
    m_fForkZOffset = 0.0f;
    m_nFixLeftHand = false;
    m_nFixRightHand = false;
    m_fSteerAngleTan = std::tan(DegreesToRadians(mi->m_fBikeSteerAngle));
    m_fMass = m_pHandlingData->m_fMass;
    m_fTurnMass = m_pHandlingData->m_fTurnMass;
    m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;
    m_vecCentreOfMass.z = 0.1f;
    m_fAirResistance = GetDefaultAirResistance();
    m_fElasticity = 0.05f;
    m_fBuoyancyConstant = m_pHandlingData->m_fBuoyancyConstant;
    m_fSteerAngle = 0.0f;
    m_GasPedal = 0.0f;
    m_BrakePedal = 0.0f;
    m_Damager = nullptr;
    m_pWhoInstalledBombOnMe = nullptr;
    m_GasPedalAudioRevs = 0.0f;
    m_fTyreTemp = 1.0f;
    m_fBrakingSlide = 0.0f;
    m_PrevSpeed = 0.0f;

    for (auto i = 0; i < 2; ++i) {
        m_nWheelStatus[i] = 0;
        m_aWheelSkidmarkType[i] = eSkidmarkType::DEFAULT;
        m_bWheelBloody[i] = false;
        m_bMoreSkidMarks[i] = false;
        m_aWheelPitchAngles[i] = 0.0f;
        m_aWheelAngularVelocity[i] = 0.0f;
        m_aWheelSuspensionHeights[i] = 0.0f;
        m_aWheelOrigHeights[i] = 0.0f;
        m_WheelStates[i] = WHEEL_STATE_NORMAL;
    }

    for (auto i = 0; i < 4; ++i) {
        m_aWheelColPoints[i] = {};
        m_aWheelRatios[i] = 1.0f;
        m_aRatioHistory[i] = 0.0f;
        m_WheelCounts[i] = 0.0f;
        m_fSuspensionLength[i] = 0.0f;
        m_fLineLength[i] = 0.0f;
        m_aGroundPhysicalPtrs[i] = nullptr;
        m_aGroundOffsets[i] = CVector{};
    }

    m_nNoOfContactWheels = 0;
    m_NumDriveWheelsOnGround = 0;
    m_NumDriveWheelsOnGroundLastFrame = 0;
    m_fHeightAboveRoad = 0.0f;
    m_fExtraTractionMult = 1.0f;

    if (!mi->m_pColModel->m_pColData->m_pLines) {
        mi->m_pColModel->m_pColData->m_nNumLines = 4;
        mi->m_pColModel->m_pColData->m_pLines = static_cast<CColLine*>(CMemoryMgr::Malloc(4 * sizeof(CColLine)));
        mi->m_pColModel->m_pColData->m_pLines[1].m_vecStart.x = 99'999.99f; // todo: explain this
    }
    mi->m_pColModel->m_pColData->m_pLines[0].m_vecStart.z = 99'999.99f;
    CBike::SetupSuspensionLines();

    m_autoPilot.m_nTempAction = TEMPACT_NONE;
    m_autoPilot.SetCarMission(MISSION_NONE, 0);
    m_autoPilot.carCtrlFlags.bAvoidLevelTransitions = false;

    SetStatus(STATUS_SIMPLE);
    m_nNumPassengers = 0;
    vehicleFlags.bLowVehicle = false;
    vehicleFlags.bIsBig = false;
    vehicleFlags.bIsVan = false;

    m_bLeanMatrixCalculated = false;
    m_mLeanMatrix = *m_matrix;
    m_vecOldSpeedForPlayback = CVector{};
    m_vehicleAudio.Initialise(this);
}

// 0x6B57A0
CBike::~CBike() {
    m_vehicleAudio.Terminate();
}

// 0x6B5A00
void CBike::dmgDrawCarCollidingParticles(const CVector& position, float power, eWeaponType weaponType) {
    // NOP
}

// 0x6B5A10
bool CBike::DamageKnockOffRider(CVehicle* vehicle, float damageIntensity, uint16 pieceType, CEntity* damager, const CVector& collisionPos, const CVector& collisionImpactVelocity) {
    const auto driver    = vehicle->m_pDriver;
    const auto passenger = vehicle->m_apPassengers[0];

    // Impact force relative to the bike's mass
    auto force = damageIntensity / vehicle->m_fMass * 800.0f;

    // A skilled rider resists being knocked off (unless flagged to always come off)
    if (vehicle->GetStatus() != STATUS_PLAYER) {
        if (driver && driver->CantBeKnockedOffBike != CANT_BE_KNOCKED_OFF_ALWAYS_NORMAL) {
            force *= 1.0f - driver->GetBikeRidingSkill() * 0.6f;
        }
    } else {
        force *= 0.75f;
        if (driver) {
            force *= 1.0f - driver->GetBikeRidingSkill() * 0.5f;
        }
    }

    // Only an actual driver gets knocked off
    // Como el original (0x6B5AC1..0x6B5AD0): fcomp + test ah,41h/jne -> tambien falla con NaN, por eso !(force > 10)
    if (!driver || !driver->IsStateDriving() || !(force > 10.0f)) {
        return false;
    }

    // A ped already reacting to a hit isn't also knocked off (cops are exempt)
    if (const auto task = driver->GetIntelligence()->GetTaskManager().GetActiveTask()) {
        if (task->GetTaskType() == TASK_SIMPLE_BE_HIT && !driver->IsCop()) {
            return false;
        }
    }

    // Como el original (0x6B5B27..0x6B5D15): cada producto escalar se suma como (y*vy + z*vz) + x*vx, no x*vx + y*vy + z*vz
    const auto Dot3 = [&](const CVector& axis) { return (axis.y * collisionImpactVelocity.y + axis.z * collisionImpactVelocity.z) + axis.x * collisionImpactVelocity.x; };
    const auto impactFwdMag   = Dot3(vehicle->GetForward());
    const auto impactUpMag    = Dot3(vehicle->GetUp());
    const auto impactRightMag = Dot3(vehicle->GetRight());

    // Per-axis weighting of the impact
    auto fwdWeight = 0.6f;
    if (std::abs(impactFwdMag) > 0.85f) {
        // Como el original (0x6B5B4B..0x6B5B8D): vertical = (vy + vx) * 0 + vz; el peso es (7 * vertical) * vertical + 0.6 (no 7 * (vertical*vertical))
        const auto z = (collisionImpactVelocity.y + collisionImpactVelocity.x) * 0.0f + collisionImpactVelocity.z;
        const auto vertical = z < 0.85f ? 0.0f : z;
        fwdWeight = (7.0f * vertical) * vertical + 0.6f;
    }
    if (vehicle->GetUp().z < 0.0f) { // bike lying on its side / upside down
        fwdWeight = 5.0f;
    }

    auto backWeight = 1.5f;
    auto upWeight   = 0.05f;
    if (vehicle->m_nModelIndex == MODEL_SANCHEZ) {
        fwdWeight *= 0.65f;
        upWeight  *= 0.75f;
    } else if (vehicle->IsSubQuad()) {
        backWeight = 3.0f;
        fwdWeight *= 0.65f;
        upWeight  *= 0.75f;
    }

    if (impactFwdMag > 0.0f) {
        fwdWeight *= 1.0f - driver->GetBikeRidingSkill() * 0.6f;
    }

    force *= std::abs(impactFwdMag) * fwdWeight
           + std::max(impactUpMag, 0.0f) * upWeight
           + std::abs(impactRightMag) * 0.45f
           - std::min(impactUpMag, 0.0f) * backWeight;

    // Don't knock the player off while they're on stairs
    if (driver->IsPlayer() && CCullZones::CamStairsForPlayer() && CCullZones::FindZoneWithStairsAttributeForPlayer()) {
        force = 0.0f;
    }

    // ALWAYS_HARD peds come off at a much lower force threshold
    if (!(force > (driver->CantBeKnockedOffBike == CANT_BE_KNOCKED_OFF_ALWAYS_HARD ? 20.0f : 75.0f))) {
        return false;
    }

    // NEVER peds are never knocked off
    if (driver->CantBeKnockedOffBike == CANT_BE_KNOCKED_OFF_NEVER) {
        return false;
    }
    if (passenger && passenger->CantBeKnockedOffBike == CANT_BE_KNOCKED_OFF_NEVER) {
        return false;
    }

    // The driver (guaranteed present here) is thrown off, and so is the passenger, both reacting with the driver's facing
    const auto knockOffDir = (uint8)driver->GetLocalDirection(-CVector2D{ collisionImpactVelocity });

    driver->GetEventGroup().Add(CEventKnockOffBike{
        vehicle, vehicle->m_vecMoveSpeed, collisionImpactVelocity, damageIntensity, 0.05f * force, KNOCK_OFF_TYPE_SKIDBACKFRONT, knockOffDir, 0, nullptr, true, false
    });
    if (passenger) {
        passenger->GetEventGroup().Add(CEventKnockOffBike{
            vehicle, vehicle->m_vecMoveSpeed, collisionImpactVelocity, damageIntensity, 0.05f * force, KNOCK_OFF_TYPE_SKIDBACKFRONT, knockOffDir, 0, nullptr, false, false
        });
    }
    return true;
}

// dummy function
// 0x6B5F40
CPed* CBike::KnockOffRider(eWeaponType arg0, uint8 arg1, CPed* ped, bool arg3) {
    return ped;
}

// 0x6B5F50
void CBike::SetRemoveAnimFlags(CPed* ped) {
    if (ped->GetType() != ENTITY_TYPE_PED) {
        return;
    }
    for (auto* association = RpAnimBlendClumpGetFirstAssociation(ped->GetRpClump(), 0x1000);
        association; association = RpAnimBlendGetNextAssociation(association, 0x1000)) {
        association->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
    }
}

// 0x6B5F90
void CBike::ReduceHornCounter() {
    if (m_HornCounter)
        m_HornCounter -= 1;
}

// 0x6B5FB0
void CBike::ProcessBuoyancy() {
    CVector vecBuoyancyTurnPoint;
    CVector vecBuoyancyForce;
    if (!mod_Buoyancy.ProcessBuoyancy(this, m_fBuoyancyConstant, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
        physicalFlags.bTouchingWater = false;
        return;
    }

    physicalFlags.bTouchingWater = true;
    ApplyMoveForce(vecBuoyancyForce);
    ApplyTurnForce(vecBuoyancyForce, vecBuoyancyTurnPoint);

    // Como el original (0x6B6061..0x6B6143): timestep minimo 0.01; fuerza Z = force.z / ((timestep * masa) * 0.008) (no masa/125);
    // si masaUsada (= masa * 0.008) > constante de flotabilidad: fuerza = ((masaUsada / constante) * fuerza) * 1.05; masa doble: * 1.5;
    // multiplicador = max(0.5, 1 - fuerza * 0.05) (no /20); pow del EXE (0x822130, precision completa) multiplica sin redondear a float.
    const float timeStep = CTimer::GetTimeStep();
    // 0x6B605B..0x6B606C: fcomp + test ah,41h + jne -> usa timeStep salvo que 0.01 > timeStep (con NaN usa timeStep)
    const float fTimeStep = (0.01F > timeStep) ? 0.01F : timeStep;
    const float fUsedMass = m_fMass * 0.008F;
    float fBuoyancyForceZ = vecBuoyancyForce.z / ((fTimeStep * m_fMass) * 0.008F);

    if (fUsedMass > m_fBuoyancyConstant)
        fBuoyancyForceZ = ((fUsedMass / m_fBuoyancyConstant) * fBuoyancyForceZ) * 1.05F;

    if (physicalFlags.bMakeMassTwiceAsBig)
        fBuoyancyForceZ *= 1.5F;

    const float fMultCandidate = 1.0F - fBuoyancyForceZ * 0.05F;
    // 0x6B60E5..0x6B60F2: usa 0.5 solo si 0.5 > candidato (con NaN el EXE conserva el candidato)
    const float fBuoyancyForceMult = (0.5F > fMultCandidate) ? 0.5F : fMultCandidate;
    {
        float* moveSpeedPtr = &m_vecMoveSpeed.x;
        float* turnSpeedPtr = &m_vecTurnSpeed.x;
        __asm {
            fld fBuoyancyForceMult
            fld timeStep
            mov eax, 0x822130
            call eax
            mov ecx, moveSpeedPtr
            fld st(0)
            fmul dword ptr [ecx]
            fstp dword ptr [ecx]
            fld st(0)
            fmul dword ptr [ecx+4]
            fstp dword ptr [ecx+4]
            fld st(0)
            fmul dword ptr [ecx+8]
            fstp dword ptr [ecx+8]
            mov ecx, turnSpeedPtr
            fld st(0)
            fmul dword ptr [ecx]
            fstp dword ptr [ecx]
            fld st(0)
            fmul dword ptr [ecx+4]
            fstp dword ptr [ecx+4]
            fmul dword ptr [ecx+8]
            fstp dword ptr [ecx+8]
        }
    }

    // 0x6B6443
    if (fBuoyancyForceZ > 0.8F || (fBuoyancyForceZ > 0.4F && IsAnyWheelNotMakingContactWithGround())) {
        vehicleFlags.bIsDrowning = true;
        physicalFlags.bSubmergedInWater = true;

        // 0x6B61D6..0x6B61FA: fcomp + test ah,5 + jp -> solo asigna si z < -0.1 (NaN se conserva)
        if (m_vecMoveSpeed.z < -0.1F)
            m_vecMoveSpeed.z = -0.1F;

        if (m_pDriver) {
            ProcessPedInVehicleBuoyancy(m_pDriver->AsPed(), true);
        }
        else {
            vehicleFlags.bEngineOn = false;
        }

        for (const auto passenger : GetPassengers()) {
            ProcessPedInVehicleBuoyancy(passenger, false);
        }
    }
    else {
        vehicleFlags.bIsDrowning = false;
        physicalFlags.bSubmergedInWater = false;
    }
}

inline void CBike::ProcessPedInVehicleBuoyancy(CPed* ped, bool bIsDriver) {
    if (!ped)
        return;

    ped->physicalFlags.bTouchingWater = true;
    if (!ped->IsPlayer() && bikeFlags.bWaterTight)
        return;

    if (ped->IsPlayer())
        ped->AsPlayer()->HandlePlayerBreath(true, 1.0F);

    if (IsAnyWheelMakingContactWithGround()) {
        if (!ped->IsPlayer()) {
            auto pedDamageResponseCalc = CPedDamageResponseCalculator(this, CTimer::GetTimeStep(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, false);
            auto damageEvent = CEventDamage(this, CTimer::GetTimeInMS(), eWeaponType::WEAPON_DROWNING, PED_PIECE_TORSO, 0, false, true);
            if (damageEvent.AffectsPed(ped))
                pedDamageResponseCalc.ComputeDamageResponse(ped, damageEvent.m_damageResponse, true);
            else
                damageEvent.m_damageResponse.m_bDamageCalculated = true;

            ped->GetEventGroup().Add(&damageEvent, false);
        }
    } else {
        auto knockOffBikeEvent = CEventKnockOffBike(this, m_vecMoveSpeed, m_vecLastCollisionImpactVelocity, m_fDamageIntensity, 0.0F, KNOCK_OFF_TYPE_FALL, 0, 0, nullptr, bIsDriver, false);
        ped->GetEventGroup().Add(&knockOffBikeEvent);
        if (bIsDriver) {
            vehicleFlags.bEngineOn = false;
        }
    }
}

// 0x6BC930
bool CBike::ProcessAI(uint32& extraHandlingFlags) {
    const auto byte=[&](size_t offset)->uint8& { return *reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(this)+offset); };
    const auto number=[&](size_t offset)->float& { return *reinterpret_cast<float*>(reinterpret_cast<uint8*>(this)+offset); };
    const auto word=[&](size_t offset)->uint32& { return *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this)+offset); };
    const auto f=[](uintptr_t address){return StaticRef<float>(address);};
    const uintptr_t modelInfo=StaticRef<uintptr_t>(0xA9B0C8+uintptr_t(uint16(GetModelIndex()))*4);
    byte(0x3DB)&=0xFC;
    const int8 area=int8(byte(0x424));
    if (area>=0 && !StaticRef<uint8>(0x97D6C0+area)) { extraHandlingFlags+=2;return false; }
    switch (byte(0x36)>>3) {
    case 0: {
        extraHandlingFlags+=2;byte(0x614)&=0xF7;
        if (int(FindPlayerPed(-1)->m_nPedState)!=61 && int(FindPlayerPed(-1)->m_nPedState)!=58) {
            if (m_pDriver) {
                if (StaticRef<CPed*>(0xB7CD98)==m_pDriver) ProcessControlInputs(0);
                else if (StaticRef<CPed*>(0xB7CF28)==m_pDriver) ProcessControlInputs(1);
            }
            const auto bikeHandling=word(0x63C);
            const auto handling=word(0x384);
            const float lean=number(0x650);
            const bool positive=lean>=0.f;
            number(0xA8)=*reinterpret_cast<const float*>(bikeHandling+(positive?4:12))*lean+*reinterpret_cast<const float*>(handling+24);
            const bool forceNeeded=positive?(number(0x4A0)<0.f || !byte(0x804)):((number(0x4A0)==0.f && !(byte(0x428)&0x20)) || !byte(0x804));
            if (forceNeeded) {
                const float speed=std::min(f(0x8712FC),m_vecMoveSpeed.Magnitude());
                const float pedal=number(positive?0x4A0:0x49C);
                const float clamped=std::max(speed/f(0x8712FC),pedal);
                float force=*reinterpret_cast<const float*>(bikeHandling+(positive?8:16))*number(0x90)*lean*speed*(clamped+pedal)*f(0x858B8C);
                force*=CStats::GetFatAndMuscleModifier(static_cast<eStatModAbilities>(11));
                const auto& matrix=GetMatrix();
                ApplyTurnForce(matrix.GetUp()*-(CTimer::GetTimeStep()*force),*reinterpret_cast<const CVector*>(reinterpret_cast<const uint8*>(this)+0xA4)+matrix.GetForward());
            }
            PruneReferences();
            if (!(byte(0x36)&0xF8)) DoDriveByShootings();
            DoSoftGroundResistance(extraHandlingFlags);
        }
        if (CPad::GetPad(0)->CarGunJustDown()) ActivateBomb();
        return true;
    }
    case 1:extraHandlingFlags+=2;return true;
    case 2: {
        CCarAI::UpdateCarAI(this);CPhysical::ProcessControl();CCarCtrl::UpdateCarOnRails(this);
        byte(0x806)=byte(0x805);byte(0x804)=byte(0x805)=2;
        auto* transmission=reinterpret_cast<cTransmission*>(uintptr_t(word(0x384))+0x2C);
        transmission->CalculateGearForSimpleCar(number(0x3C8)*f(0x858B38),*reinterpret_cast<uint8*>(reinterpret_cast<uint8*>(this)+0x4B4));
        const float frontRadius=*reinterpret_cast<const float*>(modelInfo+0x40)*f(0x858B8C);
        const float firstStep=CTimer::GetTimeStep();
        number(0x750)+=ProcessWheelRotation(static_cast<tWheelState>(0),GetMatrix().GetForward(),m_vecMoveSpeed,frontRadius)*firstStep;
        const float secondStep=CTimer::GetTimeStep();
        number(0x754)+=ProcessWheelRotation(static_cast<tWheelState>(0),GetMatrix().GetForward(),m_vecMoveSpeed,*reinterpret_cast<const float*>(modelInfo+0x44)*f(0x858B8C))*secondStep;
        if (byte(0x3DB)&3) PlayCarHorn();
        if (word(0x514)) --word(0x514);
        byte(0x42B)&=0xDE;byte(0x614)&=0x7F;return true;
    }
    case 3:case 12:
        CCarAI::UpdateCarAI(this);CCarCtrl::SteerAICarWithPhysics(this);
        if (byte(0x3DB)&3) PlayCarHorn();
        extraHandlingFlags+=2;byte(0x614)&=0x7F;
        if (byte(0x42A)&8) { byte(0x428)|=0x20;number(0x49C)=0.f;number(0x4A0)=1.f; }
        else byte(0x614)&=0xF7;
        return true;
    case 4:
        number(0x4A0)=0.f;
        if (m_vecMoveSpeed.SquaredMagnitude()<f(0x858C58) || (byte(0x614)&0x10)) byte(0x428)|=0x20;
        else byte(0x428)&=0xDF;
        number(0x49C)=0.f;word(0x514)=0;
        if ((m_pDriver || m_apPassengers[0] || (byte(0x42A)&8)) && !(byte(0x614)&0x10)) extraHandlingFlags+=2;
        number(0x654)=number(0x658)=0.f;byte(0x614)&=0x7F;
        if (byte(0x42A)&8) { byte(0x428)|=0x20;number(0x49C)=0.f;number(0x4A0)=1.f; }
        return true;
    case 5:
        number(0x4A0)=.05f;byte(0x428)|=0x20;number(0x494)=number(0x49C)=0.f;word(0x514)=0;byte(0x614)&=0x7F;number(0x654)=number(0x658)=0.f;return true;
    case 9:
        if (m_vecMoveSpeed.SquaredMagnitude()<f(0x858C58)) { byte(0x428)|=0x20;number(0x4A0)=1.f; }
        else { byte(0x428)&=0xDF;number(0x4A0)=0.f; }
        number(0x494)=number(0x49C)=0.f;word(0x514)=0;extraHandlingFlags+=2;byte(0x614)&=0x7F;return true;
    default:return true;
    }
}

// 0x6BF400
void CBike::ProcessDrivingAnims(CPed* driver, bool blend) {
    // Como el original (0x6BF400): test [ecx+1Ch],20000h + test byte [ecx+36h],0F8h / jne -> sale si esta fuera de pantalla y el estado NO es STATUS_PLAYER
    if (m_bOffscreen && GetStatus() != STATUS_PLAYER)
        return;

    ProcessRiderAnims(driver, this, &m_RideAnimData, m_BikeHandling, 0);
}

namespace {
float BikeRiderSmooth355(float base,float step,float previous,float target) {
    float result;
    __asm {
        fld base
        fld step
        mov eax,00822130h
        call eax
        fld dword ptr ds:[00858624h]
        fsub st,st(1)
        fmul target
        fxch st(1)
        fmul previous
        faddp st(1),st
        fstp result
    }
    return result;
}
void BikeRiderWeights355(float lateral,float forward,float available,float* lateralOut,float* forwardOut) {
    float inverse;
    __asm {
        fld forward
        fld lateral
        fmul lateral
        fld st(1)
        fmul st,st(2)
        faddp st(1),st
        fsqrt
        fcom dword ptr ds:[0087135Ch]
        fnstsw ax
        test ah,41h
        jne rider_weights_small
        fld dword ptr ds:[00858624h]
        fdiv st,st(1)
        fstp inverse
        fstp st(0)
        fmul inverse
        fabs
        fld inverse
        fmul lateral
        fabs
        jmp rider_weights_store
    rider_weights_small:
        fstp st(0)
        fabs
        fld lateral
        fabs
    rider_weights_store:
        fxch st(1)
        fmul available
        mov eax,forwardOut
        fstp dword ptr [eax]
        fmul available
        mov eax,lateralOut
        fstp dword ptr [eax]
    }
}
float BikeRiderJitter355(float speed,int randomValue) {
    float maximum,minimum,result;
    __asm {
        fld dword ptr ds:[00871338h]
        fmul speed
        fst maximum
        fchs
        fstp minimum
        fild randomValue
        fmul dword ptr ds:[00858C7Ch]
        fld maximum
        fsub minimum
        fmul st,st(1)
        fadd minimum
        fstp result
        fstp st(0)
    }
    return result;
}
float BikeRiderForwardLimit355(float lean,float target) {
    float result;
    __asm {
        fld lean
        fabs
        fsub dword ptr ds:[00871358h]
        fld dword ptr ds:[00871354h]
        fsub dword ptr ds:[00871358h]
        fdivp st(1),st
        fsubr dword ptr ds:[00858624h]
        fld dword ptr ds:[00858B50h]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne rider_limit_nonnegative
        fstp st(0)
        fld dword ptr ds:[00858B50h]
    rider_limit_nonnegative:
        fmul target
        fstp result
    }
    return result;
}
}
// 0x6B7280
void CBike::ProcessRiderAnims(CPed* rider, CVehicle* vehicle, CRideAnimData* rideData, tBikeHandlingData* handling, int16) {
    const auto number=[](const void* object,size_t offset)->float& {return *reinterpret_cast<float*>(reinterpret_cast<uintptr_t>(object)+offset);};
    const auto byte=[](void* object,size_t offset)->uint8& {return *reinterpret_cast<uint8*>(reinterpret_cast<uintptr_t>(object)+offset);};
    const auto f=[](uintptr_t address){return StaticRef<float>(address);};
    const bool player=rider->IsPlayer();
    const int type=*reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(vehicle)+0x590);
    auto* bike=type==9?vehicle:nullptr;
    auto* automobile=type==0?vehicle:nullptr;
    const uint8 contacts=bike?byte(bike,0x804):automobile?byte(automobile,0x960):0;
    auto* clump=rider->GetRpClump();
    const auto association=[&](int id){return RpAnimBlendClumpGetAssociation(clump,static_cast<AnimationId>(id));};
    auto* left=association(196);auto* right=association(197);auto* idle=association(195);
    CAnimBlendAssociation* forward=nullptr;CAnimBlendAssociation* backward=nullptr;
    if (player) { forward=association(199);backward=association(198); }
    auto* reverse=association(200);auto* special=association(202);
    if (!special) special=association(203);
    if (!special) special=association(204);
    const auto& matrix=vehicle->GetMatrix();
    const float speed=vehicle->m_vecMoveSpeed.x*matrix.GetForward().x+vehicle->m_vecMoveSpeed.y*matrix.GetForward().y+vehicle->m_vecMoveSpeed.z*matrix.GetForward().z;
    if (*reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(vehicle)+0x594)==10 && (reverse || special)) byte(vehicle,0x7B5)=0;
    const auto knockOff=[&] {
        CEventKnockOffBike driverEvent(vehicle,vehicle->m_vecMoveSpeed,CVector{0,0,1},0.f,0.f,54,2,0,nullptr,true,false);
        rider->GetEventGroup().Add(&driverEvent,false);
        if (vehicle->m_apPassengers[0] && vehicle->m_apPassengers[0]!=rider) {
            CEventKnockOffBike passengerEvent(vehicle,vehicle->m_vecMoveSpeed,CVector{0,0,1},0.f,0.f,54,2,0,nullptr,false,false);
            vehicle->m_apPassengers[0]->GetEventGroup().Add(&passengerEvent,false);
        }
    };
    const float angularLimit=(rider->GetBikeRidingSkill()+1.f)*f(0x871348);
    if (angularLimit*angularLimit<vehicle->m_vecTurnSpeed.SquaredMagnitude()) knockOff();
    float leanFwdTarget=0.f;
    const auto blend=[&](int id,float delta){return CAnimManager::BlendAnimation(clump,rideData->AnimGroup,static_cast<AnimationId>(id),delta);};
    const auto faded=[](CAnimBlendAssociation* animation){return !animation || (animation->m_BlendAmount<1.f && animation->m_BlendDelta<=0.f);};
    const auto fade=[](CAnimBlendAssociation* animation){if(animation && animation->m_BlendDelta>=0.f)animation->m_BlendDelta=-4.f;};
    if (special || std::abs(speed)>=.02f) {
        if (speed>=0.f) { fade(idle);fade(reverse); }
        else {
            float reverseLimit=(rider->GetBikeRidingSkill()+1.f)*3.5f;
            if (*reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(vehicle)+0x594)==2) reverseLimit+=reverseLimit;
            const float reverseVelocity=number(vehicle->m_pHandlingData,0x8C);
            if (reverseLimit*reverseVelocity<=speed || (!contacts && (matrix.GetUp().z>=-.5f || CTimer::GetTimeInMS()-*reinterpret_cast<const uint32*>(reinterpret_cast<const uint8*>(vehicle)+0x3C)>99))) {
                if (number(vehicle,0x49C)>=0.f || speed<=reverseVelocity*1.5f) {
                    if (player && speed<reverseVelocity*1.5f) leanFwdTarget=-1.f;
                    fade(idle);fade(reverse);
                } else if (faded(reverse)) reverse=blend(200,4.f);
            } else knockOff();
        }
    } else if (faded(idle)) idle=blend(195,2.f);
    const float nonClippedStep=StaticRef<float>(0xB7CB58)*.02f;
    float available=1.f;
    if (idle) available=1.f-std::min(nonClippedStep*idle->m_BlendDelta+idle->m_BlendAmount,1.f);
    if (special) available-=std::min(nonClippedStep*special->m_BlendDelta+special->m_BlendAmount,available);
    if (reverse) available-=std::min(nonClippedStep*reverse->m_BlendDelta+reverse->m_BlendAmount,available);
    float lean=rideData->LeanAngle/handling->m_fFullAnimLean;
    if (leanFwdTarget==-1.f) lean=0.f;else lean=std::clamp(lean,-1.f,1.f);
    rideData->AnimLeanLeft=BikeRiderSmooth355(f(0x871350),CTimer::GetTimeStep(),rideData->AnimLeanLeft,lean);
    if (!player || vehicle->m_apPassengers[0]) {
        leanFwdTarget=0.f;if(bike)byte(bike,0x614)&=0x7F;
    } else if (leanFwdTarget> -1.f) {
        leanFwdTarget=rideData->LeanFwd;
        bool ordinary=true;
        bool wheelie=false;
        float wheelieDifference=0.f;
        if (bike) {
            auto& flags=byte(bike,0x614);flags&=0x7F;
            if (number(bike,0x730)<=0.f && number(bike,0x734)<=0.f && matrix.GetForward().z>0.f && (number(bike,0x738)>0.f || number(bike,0x73C)>0.f)) {
                wheelie=true;wheelieDifference=handling->m_fWheelieAng-matrix.GetForward().z;
                if(wheelieDifference<handling->m_fWheelieAng*.5f)flags|=0x80;
            } else if (number(bike,0x738)<=0.f && number(bike,0x73C)<=0.f && matrix.GetForward().z<0.f && (number(bike,0x730)>0.f || number(bike,0x734)>0.f) && handling->m_fStoppieAng*.6f<handling->m_fStoppieAng-matrix.GetForward().z) flags|=0x80;
        } else if (automobile && number(automobile,0x7F4)<=0.f && number(automobile,0x7F8)<=0.f && matrix.GetForward().z>0.f && (number(automobile,0x7FC)>0.f || number(automobile,0x800)>0.f)) {
            wheelie=true;wheelieDifference=handling->m_fWheelieAng-matrix.GetForward().z;
        }
        if (wheelie && wheelieDifference<.15f) { if(leanFwdTarget<=.25f)leanFwdTarget=.25f;ordinary=false; }
        if (ordinary) {
            if (number(vehicle,0x4A0)<=.5f || speed<=.01f) {
                if (number(vehicle,0x49C)>.5f && leanFwdTarget<=0.f && speed<number(vehicle->m_pHandlingData,0x88)*.3f && leanFwdTarget>=-.3f) leanFwdTarget=-.3f;
            } else if (leanFwdTarget<=.1f) leanFwdTarget=.1f;
        }
        if (std::abs(lean)>f(0x871358)) leanFwdTarget=BikeRiderForwardLimit355(lean,leanFwdTarget);
    }
    if (!player) rideData->AnimLeanFwd=0.f;
    else rideData->AnimLeanFwd=BikeRiderSmooth355(f(0x87134C),CTimer::GetTimeStep(),rideData->AnimLeanFwd,leanFwdTarget);
    float lateralWeight,forwardWeight;
    if (std::abs(rideData->AnimLeanLeft)>.56f || !player) { forwardWeight=0.f;lateralWeight=1.f; }
    else if (std::abs(rideData->AnimLeanFwd)>.56f) { forwardWeight=1.f;lateralWeight=0.f; }
    else {
        lateralWeight=rideData->AnimLeanLeft;forwardWeight=rideData->AnimLeanFwd;
        BikeRiderWeights355(lateralWeight,forwardWeight,available,&lateralWeight,&forwardWeight);
        available=1.f;
    }
    const auto add=[&](int id){return CAnimManager::AddAnimation(clump,rideData->AnimGroup,static_cast<AnimationId>(id));};
    const auto pose=[](CAnimBlendAssociation* animation,CAnimBlendAssociation* opposite,float amount,float value) {
        animation->m_BlendAmount=amount;animation->SetCurrentTime(animation->m_BlendHier->m_fTotalTime*value);animation->m_Flags&=uint16(~1);opposite->m_BlendAmount=0.f;
    };
    if (player) {
        if(!forward)forward=add(199);if(!backward)backward=add(198);
        if(rideData->AnimLeanFwd<0.f)pose(backward,forward,forwardWeight*available,-rideData->AnimLeanFwd);
        else pose(forward,backward,forwardWeight*available,rideData->AnimLeanFwd);
    }
    if(!left)left=add(196);if(!right)right=add(197);
    if(rideData->AnimLeanLeft<0.f)pose(left,right,lateralWeight*available,-rideData->AnimLeanLeft);
    else pose(right,left,lateralWeight*available,rideData->AnimLeanLeft);
    if (speed>.3f) {
        auto* boneMatrix=*reinterpret_cast<RwMatrix**>(*reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(rider)+0x490)+0x10);
        const auto jitter=[&](uintptr_t axis) {const int random=CGeneral::GetRandomNumber();const float angle=BikeRiderJitter355(speed,random);RwMatrixRotate(boneMatrix,reinterpret_cast<const RwV3d*>(axis),angle,rwCOMBINEPOSTCONCAT);};
        jitter(0x8D232C);jitter(0x8D2338);
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(rider)+0x470)|=0x4000;
    }
}

// 0x6BEB20
bool CBike::BurstTyre(uint8 component, bool physicalEffect) {
    if (vehicleFlags.bTyresDontBurst || (m_nPhysicalFlags & 0x20000000u)) return false;
    if (component == 13) component = 0;
    else if (component == 15) component = 1;
    auto* status = reinterpret_cast<uint8*>(this) + 0x65C + component;
    bool changed = false;
    if (*status == 0) {
        *status = 1;
        m_vehicleAudio.AddAudioEvent(static_cast<eAudioEvents>(90), 0.f);
        if (GetStatus() == STATUS_SIMPLE) CCarCtrl::SwitchVehicleToRealPhysics(this);
        if (physicalEffect) {
            const auto limit = StaticRef<float>(0x8D322C);
            const auto force = CGeneral::GetRandomNumberInRange<float>(-limit, limit) * m_fMass;
            ApplyMoveForce(GetRight() * force);
            const auto turn = CGeneral::GetRandomNumberInRange<float>(-limit, limit) * m_fTurnMass;
            ApplyTurnForce(GetRight() * turn, GetForward());
        }
        changed = true;
    }
    // Component 13 is mapped to zero above; the EXE's later front-wheel event branch is unreachable.
    if (m_pDriver && component == 14 && (m_aRatioHistory[2] < StaticRef<float>(0x871284) || m_aRatioHistory[3] < StaticRef<float>(0x871284))) {
        const auto speed = std::sqrt((m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z);
        if (speed > StaticRef<float>(0x8D3230) && (GetStatus() != STATUS_PLAYER || speed > StaticRef<float>(0x8D3234))) {
            const auto amount = (StaticRef<float>(0x8D322C) * m_fTurnMass) * 2.f;
            ApplyTurnForce(GetRight() * amount, GetForward());
        }
    }
    return changed;
}


static float BikeControlBlend355(float oldValue, int32 input, float step, float factor) {
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

static float BikeControlProductAdd355(float oldValue, float input, float factor) {
    float result;
    __asm {
        fld input
        fmul factor
        fadd oldValue
        fstp result
    }
    return result;
}

static float BikeInputSteer355(float degrees, float steer) {
    const float radians = StaticRef<float>(0x8595EC);
    float result;
    __asm {
        fld degrees
        fmul radians
        fmul steer
        fstp result
    }
    return result;
}

static float BikeForwardSpeed355(const CVector* speed, const CVector* forward) {
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

static void BikeMouseDecay355(float* value) {
    const float base = StaticRef<float>(0x87130C), step = CTimer::GetTimeStep();
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

static void BikeInputSpeedCap355(CVector* speed) {
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

// 0x6BE310
void CBike::ProcessControlInputs(uint8 playerNum) {
    const auto& forward = GetForward();
    const float forwardSpeed = BikeForwardSpeed355(&m_vecMoveSpeed, &forward);
    auto* bytes = reinterpret_cast<uint8*>(this);
    if (CPad::GetPad(playerNum)->GetExitVehicle()) bytes[0x428] |= 0x20;
    else bytes[0x428] = (bytes[0x428] & 0xDF) | (CPad::GetPad(playerNum)->GetHandBrake() != 0 ? 0x20 : 0);
    auto& lean = m_RideAnimData.LeanFwd;
    bool mouse = false, keyboard = true;
    if (StaticRef<uint8>(0xB6EC2E) && m_bEnableMouseSteering) {
        CPad::GetPad(playerNum);
        if (StaticRef<float>(0xB73424) != 0.f) mouse = true;
        else {
            CPad::GetPad(playerNum);
            if (StaticRef<float>(0xB73428) != 0.f) mouse = true;
        }
        if (!mouse && std::abs(m_fRawSteerAngle) > 0.f && int32(m_nLastControlInput) == 2) {
            if (!CPad::GetPad(playerNum)->GetSteeringLeftRight() && !CPad::GetPad(playerNum)->GetSteeringUpDown()) mouse = true;
        }
        if (!mouse) {
            if (!CPad::GetPad(playerNum)->GetSteeringLeftRight() && !CPad::GetPad(playerNum)->GetSteeringUpDown() && int32(m_nLastControlInput) == 2) keyboard = false;
            else m_nLastControlInput = static_cast<eControllerType>(0);
        }
    }
    if (mouse) {
        m_nLastControlInput = static_cast<eControllerType>(2);
        if (!CPad::GetPad(playerNum)->NewState.m_bVehicleMouseLook) {
            CPad::GetPad(playerNum);
            m_fRawSteerAngle = BikeControlProductAdd355(m_fRawSteerAngle, StaticRef<float>(0xB73424), StaticRef<float>(0x871304));
            CPad::GetPad(playerNum);
            lean = BikeControlProductAdd355(lean, StaticRef<float>(0xB73428), StaticRef<float>(0x871304));
        }
        if (std::abs(m_fRawSteerAngle) < StaticRef<float>(0x871308) || CPad::GetPad(playerNum)->NewState.m_bVehicleMouseLook) BikeMouseDecay355(&m_fRawSteerAngle);
        if (std::abs(lean) < StaticRef<float>(0x871308) || CPad::GetPad(playerNum)->NewState.m_bVehicleMouseLook) BikeMouseDecay355(&lean);
    } else if (keyboard) {
        float step = CTimer::GetTimeStep();
        const int32 left = -int32(CPad::GetPad(playerNum)->GetSteeringLeftRight());
        m_fRawSteerAngle = BikeControlBlend355(m_fRawSteerAngle, left, step, StaticRef<float>(0x871300));
        step = CTimer::GetTimeStep();
        const int32 up = -int32(CPad::GetPad(playerNum)->GetSteeringUpDown());
        lean = BikeControlBlend355(lean, up, step, StaticRef<float>(0x858CC4));
    }
    m_fRawSteerAngle = !(m_fRawSteerAngle > -1.f) ? -1.f : m_fRawSteerAngle >= 1.f ? 1.f : m_fRawSteerAngle;
    lean = !(lean > -1.f) ? -1.f : lean >= 1.f ? 1.f : lean;
    const int32 accelerate = int32(CPad::GetPad(playerNum)->GetAccelerate());
    const int32 brake = int32(CPad::GetPad(playerNum)->GetBrake());
    const float pedal = float(accelerate - brake) * StaticRef<float>(0x859A3C);
    if (std::abs(forwardSpeed) < StaticRef<float>(0x858C58)) {
        if (CPad::GetPad(playerNum)->GetAccelerate() > 150 && CPad::GetPad(playerNum)->GetBrake() > 150 && int32(m_nVehicleSubType) != 10) {
            m_GasPedal = float(CPad::GetPad(playerNum)->GetAccelerate()) * StaticRef<float>(0x859A3C);
            m_BrakePedal = float(CPad::GetPad(playerNum)->GetBrake()) * StaticRef<float>(0x859A3C);
            m_nBrakesOn = 1;
        } else {
            m_GasPedal = pedal;
            m_BrakePedal = 0.f;
        }
    } else if (forwardSpeed >= 0.f) {
        m_GasPedal = pedal < 0.f ? 0.f : pedal;
        m_BrakePedal = pedal < 0.f ? -pedal : 0.f;
    } else {
        m_GasPedal = pedal < 0.f ? pedal : 0.f;
        m_BrakePedal = pedal < 0.f ? 0.f : pedal;
    }
    auto& steer = StaticRef<float>(0xC1C804);
    steer = m_fRawSteerAngle * m_fRawSteerAngle;
    if (m_fRawSteerAngle < 0.f) steer = -steer;
    const auto controller = int8(bytes[0x424]);
    if (controller < 0 || StaticRef<uint8>(0x97D6C0 + controller)) m_fSteerAngle = BikeInputSteer355(m_pHandlingData->m_fSteeringLock, steer);
    if (bytes[0x429] & 0x10) {
        if ((CTimer::GetTimeInMS() & 0x3C00) < 0x3000) m_GasPedal = 1.f;
        if ((((CTimer::GetTimeInMS() >> 10) + 6) & 15) < 12) m_BrakePedal = 0.f;
        bytes[0x428] &= 0xDF;
        m_fSteerAngle += CTimer::GetTimeInMS() & 0x800 ? StaticRef<float>(0x859018) : -StaticRef<float>(0x858B10);
    }
    if (*reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(CPad::GetPad(0)) + 0x10E) && StaticRef<int32>(0x96A8A0) != 2) {
        bytes[0x428] |= 0x20;
        m_BrakePedal = 1.f;
        m_GasPedal = 0.f;
        FindPlayerPed(-1)->KeepAreaAroundPlayerClear();
        BikeInputSpeedCap355(&m_vecMoveSpeed);
    }
}

// 0x6BDEA0
int32 CBike::ProcessEntityCollision(CEntity* entity, CColPoint* outColPoints) {
    if (GetStatus() != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true;
    }

    const auto tcd = GetColData(),
               ocd = entity->GetColData();

#ifdef FIX_BUGS // Text search for `FIX_BUGS@CAutomobile::ProcessEntityCollision:1`
    if (!tcd || !ocd) {
        return 0;
    }
#endif

    if (physicalFlags.bSkipLineCol || physicalFlags.bProcessingShift || entity->GetIsTypePed()) {
        tcd->m_nNumLines = 0; // Later reset back to original value
    }

    const auto ogWheelRatios = m_aWheelRatios;

    auto numColPts = CCollision::ProcessColModels(
        GetMatrix(), *GetColModel(),
        entity->GetMatrix(), *entity->GetColModel(),
        *(std::array<CColPoint, 32>*)(outColPoints),
        m_aWheelColPoints.data(),
        m_aWheelRatios.data(),
        false
    );

    // Possibly add driver & entity collisions to `outColPoints`
    if (m_pDriver && m_nTestPedCollision) {
        const auto pcd = m_pDriver->GetColData();
        if (!pcd->m_nNumLines) {
            std::array<CColPoint, 32> pedCPs{};

            CMatrix driverMat = GetMatrix();
            driverMat.GetPosition() += GetDriverSeatDummyPositionWS();

            std::array<CColPoint, 32> pedEntityColPts{};
            const auto numPedEntityColPts = CCollision::ProcessColModels(
                driverMat, *m_pDriver->GetColModel(),
                entity->GetMatrix(), *entity->GetColModel(),
                pedEntityColPts,
                nullptr,
                nullptr,
                false
            );

            if (numPedEntityColPts) {
                if (m_nTestPedCollision == 1) {
                    m_nTestPedCollision = 0;
                } else {
                    for (auto i = 0; i < numPedEntityColPts && numColPts < 32; i++) {
                        const auto& pedEntityCP = pedCPs[i];
                        if (pedEntityCP.m_nPieceTypeA == PED_COL_SPHERE_LEG) {
                            continue;
                        }
                        outColPoints[numColPts++] = pedEntityCP;
                    }
                }
            }
        }
    }

    size_t numProcessedLines{};
    if (tcd->m_nNumLines) {
        // Process the real wheels
        for (auto i = 0; i < NUM_SUSP_LINES; i++) {
            const auto& cp = m_aWheelColPoints[i];

            const auto wheelColPtsTouchDist = m_aWheelRatios[i];
            if (wheelColPtsTouchDist >= 1.f || wheelColPtsTouchDist >= ogWheelRatios[i]) {
                continue;
            }

            numProcessedLines++;

            m_anCollisionLighting[i] = cp.m_nLightingB;
            m_nContactSurface = cp.m_nSurfaceTypeB;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                CEntity::ChangeEntityReference(m_aGroundPhysicalPtrs[i], entity->AsPhysical());

                m_aGroundOffsets[i] = cp.m_vecPoint - entity->GetPosition();
                if (entity->GetIsTypeVehicle()) {
                    m_anCollisionLighting[i] = entity->AsVehicle()->m_anCollisionLighting[i];
                }
                break;
            }
            case ENTITY_TYPE_BUILDING: {
                m_pEntityWeAreOn = entity;
                m_bTunnel = entity->m_bTunnel;
                m_bTunnelTransition = entity->m_bTunnelTransition;
                break;
            }
            }
        }
    } else {
        tcd->m_nNumLines = NUM_SUSP_LINES;
    }

    if (numColPts > 0 || numProcessedLines > 0) {
        AddCollisionRecord(entity);
        if (!entity->GetIsTypeBuilding()) {
            entity->AsPhysical()->AddCollisionRecord(this);
        }
        if (numColPts > 0) {
            if (   entity->GetIsTypeBuilding()
                || (entity->GetIsTypeObject() && entity->AsPhysical()->physicalFlags.bDisableCollisionForce)
            ) {
                SetHasHitWall(true);
            }
        }
    }

    return numColPts;
}

// 0x6B9250
// Native transcription from complete canonical ASM; all internal branches relocated.
// SEH handler, one unwind state and fire-event destructor funclet are relocated locally.

// Own copies of VC7 FuncInfo and UnwindMap; no original handler/funclet pointers.
static const uintptr_t BikeControl355FrameDispatcher = 0x8215A0;
static const uintptr_t BikeControl355FireEventDestructor = 0x4B1170;
static void __declspec(naked) __cdecl BikeControl355UnwindFireEvent() {
    __asm {
        lea ecx,[ebp-108h]
        jmp dword ptr [BikeControl355FireEventDestructor]
    }
}
struct BikeControl355UnwindEntry { int32 previousState; void(__cdecl* action)(); };
static const BikeControl355UnwindEntry BikeControl355UnwindMap[] = {{-1,&BikeControl355UnwindFireEvent}};
struct BikeControl355FuncInfo {
    uint32 magic; int32 maxState; const BikeControl355UnwindEntry* unwind;
    int32 tryCount; const void* tries; int32 ipCount; const void* ipMap;
};
static const BikeControl355FuncInfo BikeControl355ExceptionInfo{0x19930520,1,BikeControl355UnwindMap,0,nullptr,0,nullptr};
static void __declspec(naked) __cdecl BikeControl355ExceptionHandler() {
    __asm {
        mov eax,offset BikeControl355ExceptionInfo
        jmp dword ptr [BikeControl355FrameDispatcher]
    }
}
static_assert(sizeof(BikeControl355FuncInfo)==28);
static_assert(sizeof(BikeControl355UnwindEntry)==8);

static const uintptr_t BikeControl355Calls[] = {
    0x00406DA0, // dependency 0
    0x0040FDB0, // dependency 1
    0x0040FE30, // dependency 2
    0x0040FE90, // dependency 3
    0x004119D0, // dependency 4
    0x00424100, // dependency 5
    0x0042CE40, // dependency 6
    0x004A9BE0, // dependency 7
    0x004AA2F0, // dependency 8
    0x004AA3F0, // dependency 9
    0x004AB420, // dependency 10
    0x004ABA50, // dependency 11
    0x004B10C0, // dependency 12
    0x004B1170, // dependency 13
    0x004D68B0, // dependency 14
    0x00502280, // dependency 15
    0x00535300, // dependency 16
    0x0053F920, // dependency 17
    0x0053FB70, // dependency 18
    0x005429F0, // dependency 19
    0x00542A50, // dependency 20
    0x00542CE0, // dependency 21
    0x005433B0, // dependency 22
    0x00543C90, // dependency 23
    0x00543D60, // dependency 24
    0x00543E90, // dependency 25
    0x005485E0, // dependency 26
    0x00559AF0, // dependency 27
    0x0055E5C0, // dependency 28
    0x0055E600, // dependency 29
    0x0055E630, // dependency 30
    0x0055E810, // dependency 31
    0x0055EB50, // dependency 32
    0x0056E210, // dependency 33
    0x0059C730, // dependency 34
    0x0059C790, // dependency 35
    0x0059C810, // dependency 36
    0x0059C910, // dependency 37
    0x005DF8F0, // dependency 38
    0x0060A8A0, // dependency 39
    0x006B5FB0, // dependency 40
    0x006D05E0, // dependency 41
    0x006D1340, // dependency 42
    0x006D21F0, // dependency 43
    0x006D2570, // dependency 44
    0x006D2980, // dependency 45
    0x006D73B0, // dependency 46
    0x006E0950, // dependency 47
    0x0072DDD0, // dependency 48
    0x00821B40, // dependency 49
    0x00821E70, // dependency 50
    0x00822130, // dependency 51
};
// El proyecto Windows enlaza /SAFESEH:NO (CMakeLists.txt): el handler VC7 está reubicado y se prueba su unwind.
#pragma warning(push)
#pragma warning(disable: 4733) // Registro FS:0 deliberado; delimitado a esta transcripción.
__declspec(naked) void CBike::ProcessControl() {
    __asm {
        push 0FFFFFFFFh
        push offset BikeControl355ExceptionHandler
        mov eax,dword ptr fs:[00000000h]
        push eax
        mov dword ptr fs:[00000000h],esp
        sub esp,17Ch
        push ebx
        push ebp
        push esi
        push edi
        mov esi,ecx
        call dword ptr [BikeControl355Calls+64]
        movsx ecx,word ptr [esi+22h]
        mov eax,dword ptr [eax+2Ch]
        mov edx,dword ptr [ecx*4+00A9B0C8h]
        xor edi,edi
        lea ecx,[esi+00000138h]
        mov dword ptr [esp+74h],eax
        mov dword ptr [esp+60h],edx
        mov dword ptr [esp+7Ch],edi
        call dword ptr [BikeControl355Calls+60]
        mov bl,byte ptr [esi+00000429h]
        mov dl,byte ptr [esi+00000614h]
        mov cl,byte ptr [esi+0000042Ch]
        mov al,0DFh
        and bl,al
        and dl,al
        and cl,0FDh
        mov byte ptr [esi+00000429h],bl
        mov byte ptr [esi+000005C8h],0
        mov word ptr [esi+000007A8h],di
        mov byte ptr [esi+00000614h],dl
        mov byte ptr [esi+0000042Ch],cl
        cmp byte ptr ds:[00A43088h],1
        je bike_control_006BC914
        mov ecx,esi
        call dword ptr [BikeControl355Calls+172]
        mov ecx,esi
        call dword ptr [BikeControl355Calls+176]
        mov ecx,esi
        call dword ptr [BikeControl355Calls+180]
        mov ecx,dword ptr [esi+00000460h]
        cmp ecx,edi
        je bike_control_006B933E
        call dword ptr [BikeControl355Calls+152]
        test al,al
        jne bike_control_006B9320
        mov ecx,dword ptr [esi+00000464h]
        cmp ecx,edi
        je bike_control_006B933E
        call dword ptr [BikeControl355Calls+152]
        test al,al
        je bike_control_006B933E
    bike_control_006B9320:
        mov al,byte ptr [esi+000007B6h]
        cmp al,1
        jne bike_control_006B9333
        mov byte ptr [esi+000007B6h],2
        jmp bike_control_006B9345
    bike_control_006B9333:
        jae bike_control_006B9345
        mov byte ptr [esi+000007B6h],1
        jmp bike_control_006B9345
    bike_control_006B933E:
        mov byte ptr [esi+000007B6h],0
    bike_control_006B9345:
        mov eax,dword ptr [esi]
        lea ecx,[esp+7Ch]
        push ecx
        mov ecx,esi
        call dword ptr [eax+00000108h]
        mov cl,byte ptr [esi+36h]
        shr cl,3
        cmp cl,2
        je bike_control_006BC914
        mov dl,byte ptr [esi+00000614h]
        test dl,10h
        je bike_control_006B939E
        mov edi,dword ptr [esi+14h]
        fld dword ptr [edi+8]
        fabs
        fcomp dword ptr ds:[00858F9Ch]
        fnstsw ax
        test ah,41h
        je bike_control_006B9395
        fld dword ptr [edi+18h]
        fabs
        fcomp dword ptr ds:[00858B8Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006B939E
    bike_control_006B9395:
        and dl,0EFh
        mov byte ptr [esi+00000614h],dl
    bike_control_006B939E:
        test byte ptr [esp+7Ch],2
        jne bike_control_006B93DA
        test byte ptr [esi+00000614h],18h
        jne bike_control_006B93DA
        mov eax,dword ptr [esi+00000384h]
        mov edx,dword ptr [eax+14h]
        mov dword ptr [esi+000000A4h],edx
        mov eax,dword ptr [eax+18h]
        mov edx,dword ptr [esi+0000063Ch]
        mov dword ptr [esi+000000A8h],eax
        mov eax,dword ptr [edx+28h]
        mov dword ptr [esi+000000ACh],eax
        jmp bike_control_006B97C9
    bike_control_006B93DA:
        mov eax,dword ptr ds:[008D323Ch]
        mov edx,dword ptr ds:[008D3238h]
        mov ecx,dword ptr ds:[008712D8h]
        mov dword ptr [esp+40h],eax
        mov eax,dword ptr [esi+14h]
        push eax
        mov dword ptr [esp+40h],edx
        lea edx,[esi+50h]
        mov dword ptr [esp+38h],ecx
        mov ecx,dword ptr ds:[008D3240h]
        push edx
        lea eax,[esp+70h]
        push eax
        mov dword ptr [esp+50h],ecx
        call dword ptr [BikeControl355Calls+144]
        mov al,byte ptr [esi+36h]
        add esp,0Ch
        test al,0F8h
        jne bike_control_006B95A9
        fld dword ptr [esi+00000710h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,1
        jne bike_control_006B94F4
        fld dword ptr [esi+00000714h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,1
        jne bike_control_006B94F4
        mov ecx,dword ptr ds:[008712DCh]
        push 0Dh
        mov dword ptr [esp+38h],ecx
        call dword ptr [BikeControl355Calls+108]
        fmul dword ptr ds:[00858CC4h]
        add esp,4
        fstp dword ptr [esp+10h]
        fld dword ptr [esi+00000718h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jnp bike_control_006B9493
        fld dword ptr [esi+0000071Ch]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jp bike_control_006B94E9
    bike_control_006B9493:
        mov ecx,dword ptr [esi+14h]
        fld dword ptr [ecx+18h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006B94E9
        mov edx,dword ptr [esi+0000063Ch]
        fld dword ptr [edx+2Ch]
        fsub dword ptr [ecx+18h]
        fabs
        fmul dword ptr [esp+10h]
        fcomp dword ptr ds:[00858C28h]
        fnstsw ax
        test ah,41h
        jne bike_control_006B94D4
        fld dword ptr ds:[00858C28h]
        fsubr dword ptr [esp+3Ch]
        jmp bike_control_006B95AD
    bike_control_006B94D4:
        fld dword ptr [edx+2Ch]
        fsub dword ptr [ecx+18h]
        fabs
        fmul dword ptr [esp+10h]
        fsubr dword ptr [esp+3Ch]
        jmp bike_control_006B95AD
    bike_control_006B94E9:
        fld dword ptr ds:[008712E0h]
        jmp bike_control_006B95AD
    bike_control_006B94F4:
        fld dword ptr [esi+00000738h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jp bike_control_006B95A9
        fld dword ptr [esi+0000073Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jp bike_control_006B95A9
        mov edx,dword ptr ds:[008712DCh]
        push 0Dh
        mov dword ptr [esp+38h],edx
        call dword ptr [BikeControl355Calls+108]
        fmul dword ptr ds:[008714A8h]
        push 0Dh
        fstp dword ptr [esp+18h]
        call dword ptr [BikeControl355Calls+108]
        fmul dword ptr ds:[00858C84h]
        mov ecx,dword ptr [esi+14h]
        add esp,8
        fstp dword ptr [esp+4Ch]
        fld dword ptr [ecx+18h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jp bike_control_006B95A9
        mov edx,dword ptr [esi+0000063Ch]
        fld dword ptr [edx+30h]
        fsub dword ptr [ecx+18h]
        fabs
        fmul dword ptr [esp+4Ch]
        fcomp dword ptr [esp+10h]
        fnstsw ax
        test ah,41h
        jne bike_control_006B9591
        fld dword ptr [esp+10h]
        fadd dword ptr ds:[00858C20h]
        fmul dword ptr [esp+3Ch]
        jmp bike_control_006B95AD
    bike_control_006B9591:
        fld dword ptr [edx+30h]
        fsub dword ptr [ecx+18h]
        fabs
        fmul dword ptr [esp+4Ch]
        fadd dword ptr ds:[00858C20h]
        fmul dword ptr [esp+3Ch]
        jmp bike_control_006B95AD
    bike_control_006B95A9:
        fld dword ptr [esp+3Ch]
    bike_control_006B95AD:
        fld dword ptr [esp+68h]
        fmul dword ptr [esp+68h]
        fmul dword ptr [esp+34h]
        fadd dword ptr ds:[00858624h]
        fdivr st,st(1)
        fstp dword ptr [esp+3Ch]
        fstp st(0)
        fld dword ptr ds:[008712E4h]
        fmul dword ptr [esp+6Ch]
        fmul dword ptr [esp+6Ch]
        fadd dword ptr ds:[00858624h]
        fdivr dword ptr [esp+40h]
        fstp dword ptr [esp+40h]
        fld dword ptr [esp+3Ch]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [BikeControl355Calls+204]
        fstp dword ptr [esp+3Ch]
        fld dword ptr [esp+40h]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [BikeControl355Calls+204]
        fld dword ptr [esp+68h]
        fmul dword ptr [esp+3Ch]
        mov edi,dword ptr [esi+14h]
        mov eax,dword ptr [esi+00000090h]
        mov dword ptr [esp+34h],eax
        fsub dword ptr [esp+68h]
        lea ebx,[esi+000000A4h]
        push ebx
        lea ecx,[esp+1Ch]
        fstp dword ptr [esp+00000084h]
        push edi
        push ecx
        fmul dword ptr [esp+78h]
        fsub dword ptr [esp+78h]
        fld dword ptr [edi+28h]
        fmul dword ptr ds:[00858C1Ch]
        fld dword ptr [edi+24h]
        fmul dword ptr ds:[00858C1Ch]
        fld dword ptr [edi+20h]
        fmul dword ptr ds:[00858C1Ch]
        fstp dword ptr [esp+24h]
        fxch st(1)
        fmul st,st(2)
        fstp dword ptr [esp+70h]
        fmul st,st(1)
        fstp dword ptr [esp+00000098h]
        fld dword ptr [esp+24h]
        fmul st,st(1)
        fstp dword ptr [esp+24h]
        fstp st(0)
        fld dword ptr [esp+70h]
        fmul dword ptr [esp+40h]
        fld dword ptr [esp+00000098h]
        fmul dword ptr [esp+40h]
        fld dword ptr [esp+24h]
        fmul dword ptr [esp+40h]
        fstp dword ptr [esp+48h]
        fstp dword ptr [esp+4Ch]
        fstp dword ptr [esp+50h]
        call dword ptr [BikeControl355Calls+140]
        fld dword ptr [eax+8]
        mov edx,esp
        fadd dword ptr [edi+8]
        sub esp,0Ch
        fld dword ptr [eax+4]
        fadd dword ptr [edi+4]
        fld dword ptr [eax]
        fadd dword ptr [edi]
        fstp dword ptr [esp+40h]
        mov eax,dword ptr [esp+40h]
        mov dword ptr [edx],eax
        fstp dword ptr [esp+44h]
        mov ecx,dword ptr [esp+44h]
        mov dword ptr [edx+4],ecx
        fstp dword ptr [esp+48h]
        mov eax,dword ptr [esp+48h]
        mov dword ptr [edx+8],eax
        mov edx,dword ptr [esp+54h]
        mov eax,dword ptr [esp+58h]
        mov ecx,esp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+5Ch]
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+8],edx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+80]
        fld dword ptr [esp+00000080h]
        mov edi,dword ptr [esi+14h]
        fmul dword ptr [edi+28h]
        mov eax,dword ptr [esi+00000090h]
        fld dword ptr [esp+00000080h]
        mov dword ptr [esp+34h],eax
        fmul dword ptr [edi+24h]
        push ebx
        lea ecx,[esp+1Ch]
        push edi
        fstp dword ptr [esp+6Ch]
        push ecx
        fld dword ptr [esp+0000008Ch]
        fmul dword ptr [edi+20h]
        fstp dword ptr [esp+24h]
        fmul dword ptr [esp+40h]
        fld dword ptr [esp+70h]
        fmul dword ptr [esp+40h]
        fld dword ptr [esp+24h]
        fmul dword ptr [esp+40h]
        fstp dword ptr [esp+48h]
        fstp dword ptr [esp+4Ch]
        fstp dword ptr [esp+50h]
        call dword ptr [BikeControl355Calls+140]
        fld dword ptr [eax+8]
        mov edx,esp
        fadd dword ptr [edi+18h]
        sub esp,0Ch
        fld dword ptr [eax+4]
        fadd dword ptr [edi+14h]
        fld dword ptr [eax]
        fadd dword ptr [edi+10h]
        fstp dword ptr [esp+40h]
        mov eax,dword ptr [esp+40h]
        mov dword ptr [edx],eax
        fstp dword ptr [esp+44h]
        mov ecx,dword ptr [esp+44h]
        mov dword ptr [edx+4],ecx
        fstp dword ptr [esp+48h]
        mov eax,dword ptr [esp+48h]
        mov dword ptr [edx+8],eax
        mov edx,dword ptr [esp+54h]
        mov eax,dword ptr [esp+58h]
        mov ecx,esp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+5Ch]
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+8],edx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+80]
        mov cl,byte ptr [esi+36h]
        shr cl,3
        test cl,cl
        je bike_control_006B97C9
        mov eax,dword ptr [esi+00000384h]
        add eax,14h
        mov edx,dword ptr [eax]
        mov dword ptr [ebx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ebx+4],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [ebx+8],eax
    bike_control_006B97C9:
        mov eax,dword ptr [esi+1Ch]
        shr eax,4
        test al,1
        mov byte ptr [esp+5Fh],0
        mov edx,3F800000h
        jne bike_control_006B9A44
        cmp cl,4
        je bike_control_006B97EF
        cmp cl,5
        jne bike_control_006B9A44
    bike_control_006B97EF:
        test byte ptr [esi+00000614h],8
        jne bike_control_006B9A44
        test byte ptr [esi+0000042Bh],1
        mov byte ptr [esp+27h],0
        jne bike_control_006B9855
        fld dword ptr [esi+44h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp bike_control_006B9855
        fld dword ptr [esi+48h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp bike_control_006B9855
        fld dword ptr [esi+4Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp bike_control_006B9855
        mov eax,dword ptr [esi+0000072Ch]
        cmp eax,edx
        jne bike_control_006B9850
        cmp eax,edx
        jne bike_control_006B9850
        cmp eax,edx
        jne bike_control_006B9850
        cmp eax,edx
        je bike_control_006B9855
    bike_control_006B9850:
        mov byte ptr [esp+27h],1
    bike_control_006B9855:
        cmp cl,5
        jne bike_control_006B9874
        mov dword ptr [esp+10h],3BC49BA6h
        mov dword ptr [esp+4Ch],3AC49BA6h
        mov dword ptr [esp+78h],3C75C28Fh
        jmp bike_control_006B988C
    bike_control_006B9874:
        mov dword ptr [esp+10h],3B449BA6h
        mov dword ptr [esp+4Ch],3A6BEDFBh
        mov dword ptr [esp+78h],3BA3D70Ah
    bike_control_006B988C:
        push 40000000h
        lea ebx,[esi+44h]
        push ebx
        lea edi,[esi+74h]
        lea ecx,[esp+000000BCh]
        push edi
        push ecx
        call dword ptr [BikeControl355Calls+8]
        add esp,0Ch
        push eax
        lea edx,[esp+20h]
        push edx
        call dword ptr [BikeControl355Calls+16]
        mov edx,dword ptr [eax]
        mov ecx,edi
        mov dword ptr [ecx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ecx+4],edx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        mov dword ptr [ecx+8],eax
        push 40000000h
        lea eax,[esi+50h]
        push eax
        lea ebp,[esi+00000080h]
        lea ecx,[esp+000000BCh]
        push ebp
        push ecx
        call dword ptr [BikeControl355Calls+8]
        add esp,0Ch
        push eax
        lea edx,[esp+20h]
        push edx
        call dword ptr [BikeControl355Calls+16]
        fld dword ptr [esp+1Ch]
        mov edx,dword ptr [eax]
        mov ecx,ebp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ecx+4],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [ecx+8],eax
        fmul dword ptr ds:[00B7CB5Ch]
        add esp,0Ch
        mov ecx,edi
        fstp dword ptr [esp+10h]
        call dword ptr [BikeControl355Calls+0]
        fld dword ptr [esp+10h]
        fmul dword ptr [esp+10h]
        fcompp
        fnstsw ax
        test ah,1
        jne bike_control_006B9966
        fld dword ptr [esp+4Ch]
        mov ecx,ebp
        fmul dword ptr ds:[00B7CB5Ch]
        fstp dword ptr [esp+10h]
        call dword ptr [BikeControl355Calls+0]
        fld dword ptr [esp+10h]
        fmul dword ptr [esp+10h]
        fcompp
        fnstsw ax
        test ah,1
        jne bike_control_006B9966
        fld dword ptr [esi+000000D4h]
        fcomp dword ptr [esp+78h]
        fnstsw ax
        test ah,5
        jnp bike_control_006B9972
    bike_control_006B9966:
        mov al,byte ptr [esp+27h]
        test al,al
        je bike_control_006B9A3D
    bike_control_006B9972:
        mov cl,byte ptr [esi+000000B8h]
        inc cl
        mov al,cl
        cmp al,0Ah
        mov byte ptr [esi+000000B8h],cl
        ja bike_control_006B9992
        mov al,byte ptr [esp+27h]
        test al,al
        je bike_control_006B9A44
    bike_control_006B9992:
        mov eax,dword ptr [esi+14h]
        test eax,eax
        lea ecx,[eax+30h]
        jne bike_control_006B9A35
        lea ecx,[esi+4]
        lea eax,[esi+4]
    bike_control_006B99A6:
        mov ecx,dword ptr [ecx+4]
        mov edx,dword ptr [eax]
        push ecx
        push edx
        call dword ptr [BikeControl355Calls+20]
        add esp,8
        test al,al
        jne bike_control_006B9A44
        mov al,byte ptr [esp+27h]
        test al,al
        je bike_control_006B99CE
        cmp byte ptr [esi+000000B8h],0Ah
        jbe bike_control_006B99D5
    bike_control_006B99CE:
        mov byte ptr [esi+000000B8h],0Ah
    bike_control_006B99D5:
        mov dword ptr [esp+28h],0
        mov eax,dword ptr [esp+28h]
        mov dword ptr [esp+2Ch],0
        mov ecx,dword ptr [esp+2Ch]
        mov dword ptr [esp+30h],0
        mov edx,dword ptr [esp+30h]
        mov dword ptr [ebx],eax
        mov dword ptr [ebx+4],ecx
        mov dword ptr [esp+28h],0
        mov eax,dword ptr [esp+28h]
        mov dword ptr [esp+2Ch],0
        mov ecx,dword ptr [esp+2Ch]
        mov dword ptr [ebx+8],edx
        mov dword ptr [esp+30h],0
        mov edx,dword ptr [esp+30h]
        mov dword ptr [esi+50h],eax
        mov dword ptr [esi+54h],ecx
        mov byte ptr [esp+5Fh],1
        mov dword ptr [esi+58h],edx
        jmp bike_control_006B9A44
    bike_control_006B9A35:
        add eax,30h
        jmp bike_control_006B99A6
    bike_control_006B9A3D:
        mov byte ptr [esi+000000B8h],0
    bike_control_006B9A44:
        xor ecx,ecx
        lea eax,[esi+000007C0h]
        lea esp,[esp]
    bike_control_006B9A50:
        cmp dword ptr [eax],0
        je bike_control_006B9A77
        or byte ptr [esi+0000042Ch],2
        mov dl,byte ptr ds:[00B7CD6Eh]
        test dl,dl
        je bike_control_006B9A77
        mov edx,dword ptr [eax]
        mov edx,dword ptr [edx+1Ch]
        shr edx,5
        test dl,1
        jne bike_control_006B9BD6
    bike_control_006B9A77:
        inc ecx
        add eax,4
        cmp ecx,4
        jl bike_control_006B9A50
        test byte ptr [esi+0000042Ch],2
        je bike_control_006B9A95
        mov byte ptr [esp+5Fh],0
        mov byte ptr [esi+000000B8h],0
    bike_control_006B9A95:
        mov eax,dword ptr [esi]
        push 31h
        push 0
        push 0
        push 0
        push 0
        push 0
        mov ecx,esi
        call dword ptr [eax+000000E0h]
        fld dword ptr [esi+000000D8h]
        fcomp qword ptr ds:[00859EF8h]
        mov byte ptr [esp+27h],0
        fnstsw ax
        test ah,41h
        jne bike_control_006B9B1F
        mov eax,dword ptr [esi+14h]
        fld dword ptr [esi+000000E8h]
        fmul dword ptr [eax+8]
        fld dword ptr [esi+000000E4h]
        fmul dword ptr [eax+4]
        faddp st(1),st
        fld dword ptr [esi+000000E0h]
        fmul dword ptr [eax]
        faddp st(1),st
        fabs
        fcomp dword ptr ds:[00858B8Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006B9B1F
        fld dword ptr [esi+4Ch]
        fld dword ptr [esi+48h]
        fld dword ptr [esi+44h]
        fld st(0)
        fmul st,st(1)
        fld st(2)
        fmul st,st(3)
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fcomp dword ptr ds:[00858B1Ch]
        fstp st(0)
        fnstsw ax
        fstp st(0)
        test ah,5
        fstp st(0)
        jnp bike_control_006B9B28
    bike_control_006B9B1F:
        test byte ptr [esi+00000614h],8
        je bike_control_006B9B2D
    bike_control_006B9B28:
        mov byte ptr [esp+27h],1
    bike_control_006B9B2D:
        mov al,byte ptr [esp+5Fh]
        test al,al
        je bike_control_006B9C3A
        mov ecx,esi
        call dword ptr [BikeControl355Calls+88]
        mov bl,byte ptr [esi+0000042Bh]
        mov cl,byte ptr [esi+00000614h]
        and bl,0DEh
        shr cl,4
        and cl,1
        mov byte ptr [esi+0000042Bh],bl
        je bike_control_006B9B88
        fld dword ptr [esi+00000644h]
        fcomp dword ptr ds:[008631B4h]
        fnstsw ax
        test ah,5
        jp bike_control_006B9B88
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[008595ECh]
        fadd dword ptr [esi+00000644h]
        fstp dword ptr [esi+00000644h]
    bike_control_006B9B88:
        test cl,cl
        je bike_control_006BBC18
        fld qword ptr ds:[00866FD0h]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [BikeControl355Calls+204]
        mov ecx,dword ptr [esi+14h]
        fstp dword ptr [esp+10h]
        fld dword ptr [ecx+8]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,41h
        je bike_control_006B9BDF
        fld dword ptr [ecx+8]
        fld dword ptr ds:[00858C1Ch]
        fcomp st(1)
        fnstsw ax
        fstp st(0)
        test ah,41h
        jne bike_control_006B9BDF
        mov dword ptr [esp+34h],0BF800000h
        jmp bike_control_006B9C00
    bike_control_006B9BD6:
        or dword ptr [esi+1Ch],40h
        jmp bike_control_006BC914
    bike_control_006B9BDF:
        fld dword ptr [ecx+8]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,41h
        jne bike_control_006B9BF9
        mov dword ptr [esp+34h],3F800000h
        jmp bike_control_006B9C00
    bike_control_006B9BF9:
        fld dword ptr [ecx+8]
        fstp dword ptr [esp+34h]
    bike_control_006B9C00:
        fld dword ptr [esp+34h]
        call dword ptr [BikeControl355Calls+200]
        fld dword ptr [esp+10h]
        fmul dword ptr [esi+0000064Ch]
        fxch st(1)
        fadd dword ptr ds:[00858F1Ch]
        fld dword ptr ds:[00858624h]
        fsub dword ptr [esp+10h]
        fmulp st(1),st
        fsubp st(1),st
        fst dword ptr [esi+0000064Ch]
        fstp dword ptr [esi+00000648h]
        jmp bike_control_006BBC18
    bike_control_006B9C3A:
        test byte ptr [esi+0000042Bh],1
        jne bike_control_006B9C4C
        mov edx,dword ptr [esi]
        push 1
        mov ecx,esi
        call dword ptr [edx+5Ch]
    bike_control_006B9C4C:
        test byte ptr [esp+7Ch],2
        jne bike_control_006B9CBB
        test byte ptr [esi+00000614h],18h
        jne bike_control_006B9CBB
        mov eax,dword ptr [esi+14h]
        fld dword ptr [eax+8]
        fcomp dword ptr ds:[00858B50h]
        fld dword ptr [esi+00000494h]
        fnstsw ax
        test ah,5
        jp bike_control_006B9C96
        fcomp dword ptr ds:[008714A4h]
        fnstsw ax
        test ah,41h
        jne bike_control_006B9CBB
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[008631D4h]
        fsubr dword ptr [esi+00000494h]
        jmp bike_control_006B9CB5
    bike_control_006B9C96:
        fcomp dword ptr ds:[008714A0h]
        fnstsw ax
        test ah,5
        jp bike_control_006B9CBB
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[008631D4h]
        fadd dword ptr [esi+00000494h]
    bike_control_006B9CB5:
        fstp dword ptr [esi+00000494h]
    bike_control_006B9CBB:
        mov eax,dword ptr [esi+00000098h]
        mov dword ptr [esp+64h],eax
        test byte ptr [esi+36h],0F8h
        jne bike_control_006B9DCE
        mov eax,dword ptr [esi+00000460h]
        test eax,eax
        je bike_control_006B9DCE
        mov ecx,dword ptr [eax+18h]
        push 0C7h
        push ecx
        call dword ptr [BikeControl355Calls+56]
        mov ecx,eax
        add esp,8
        test ecx,ecx
        je bike_control_006B9DCE
        fld dword ptr [ecx+18h]
        fcomp dword ptr ds:[00858B8Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006B9DCE
        fld dword ptr [ecx+20h]
        fcomp dword ptr ds:[00871294h]
        fnstsw ax
        test ah,41h
        jne bike_control_006B9DCE
        fld dword ptr [ecx+20h]
        fcomp dword ptr ds:[00871298h]
        fnstsw ax
        test ah,5
        jp bike_control_006B9DCE
        call dword ptr [BikeControl355Calls+192]
        fld dword ptr [esi+00000098h]
        test al,al
        je bike_control_006B9D4B
        fmul dword ptr ds:[00862E1Ch]
        jmp bike_control_006B9D51
    bike_control_006B9D4B:
        fmul dword ptr ds:[00858CC8h]
    bike_control_006B9D51:
        fstp dword ptr [esi+00000098h]
        fld dword ptr [esi+0000049Ch]
        fcomp dword ptr ds:[00858B8Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006B9DCE
        mov edi,dword ptr [esi+14h]
        lea edx,[edi+10h]
        push edx
        lea eax,[esi+44h]
        push eax
        call dword ptr [BikeControl355Calls+4]
        fcomp dword ptr ds:[00858C84h]
        add esp,8
        fnstsw ax
        test ah,41h
        jne bike_control_006B9DCE
        fld dword ptr ds:[00B7CB5Ch]
        add edi,10h
        fmul dword ptr [esi+0000008Ch]
        push edi
        push ecx
        lea ecx,[esp+20h]
        fmul dword ptr ds:[0087149Ch]
        fstp dword ptr [esp]
        push ecx
        call dword ptr [BikeControl355Calls+12]
        mov ecx,dword ptr [eax]
        mov edx,esp
        mov dword ptr [edx],ecx
        mov ecx,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [edx+4],ecx
        mov ecx,esi
        mov dword ptr [edx+8],eax
        call dword ptr [BikeControl355Calls+76]
        or byte ptr [esi+00000614h],20h
    bike_control_006B9DCE:
        mov edi,dword ptr [esi+40h]
        shr edi,8
        mov ecx,esi
        and edi,1
        call dword ptr [BikeControl355Calls+104]
        mov ecx,dword ptr [esp+64h]
        mov dword ptr [esi+00000098h],ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+160]
        test edi,edi
        jne bike_control_006B9E55
        mov eax,dword ptr [esi+40h]
        test ah,1
        je bike_control_006B9E55
        mov ecx,dword ptr [esi+00000460h]
        test ecx,ecx
        je bike_control_006B9E1B
        call dword ptr [BikeControl355Calls+152]
        test al,al
        je bike_control_006B9E1B
        mov ecx,dword ptr [esi+00000460h]
        call dword ptr [BikeControl355Calls+156]
        jmp bike_control_006B9E55
    bike_control_006B9E1B:
        mov al,byte ptr [esi+00000488h]
        xor ebx,ebx
        test al,al
        jbe bike_control_006B9E55
        lea edi,[esi+00000464h]
        lea ecx,[ecx]
    bike_control_006B9E30:
        mov ecx,dword ptr [edi]
        test ecx,ecx
        je bike_control_006B9E46
        call dword ptr [BikeControl355Calls+152]
        test al,al
        je bike_control_006B9E46
        mov ecx,dword ptr [edi]
        call dword ptr [BikeControl355Calls+156]
    bike_control_006B9E46:
        movzx edx,byte ptr [esi+00000488h]
        inc ebx
        add edi,4
        cmp ebx,edx
        jl bike_control_006B9E30
    bike_control_006B9E55:
        lea eax,[esi+00000710h]
        mov ecx,4
    bike_control_006B9E60:
        fld dword ptr [eax+60h]
        add eax,4
        dec ecx
        fdiv dword ptr [eax+6Ch]
        fsubr dword ptr ds:[00858624h]
        fld dword ptr [eax-4]
        fsub st,st(1)
        fld dword ptr ds:[00858624h]
        fsub st,st(2)
        fdivp st(1),st
        fstp dword ptr [eax-4]
        fstp st(0)
        jne bike_control_006B9E60
        xor ebp,ebp
        lea eax,[esi+00000710h]
        xor ebx,ebx
        mov dword ptr [esp+10h],eax
        lea edi,[esi+00000664h]
        mov dword ptr [esp+4Ch],ebx
        mov edi,edi
    bike_control_006B9EA0:
        fld dword ptr [eax]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006B9F54
        mov ecx,dword ptr [esi+14h]
        test ecx,ecx
        lea eax,[ecx+30h]
        jne bike_control_006B9EC0
        lea eax,[esi+4]
    bike_control_006B9EC0:
        fld dword ptr [edi+4]
        fsub dword ptr [eax+8]
        fld dword ptr [edi]
        fsub dword ptr [eax+4]
        fld dword ptr [edi-4]
        fsub dword ptr [eax]
        lea eax,[esp+ebx+000000F0h]
        fstp dword ptr [esp+28h]
        mov edx,dword ptr [esp+28h]
        mov dword ptr [eax],edx
        fstp dword ptr [esp+2Ch]
        mov edx,dword ptr [esp+2Ch]
        mov dword ptr [eax+4],edx
        fstp dword ptr [esp+30h]
        mov edx,dword ptr [esp+30h]
        mov dword ptr [eax+8],edx
        mov eax,dword ptr [esp+74h]
        mov edx,dword ptr [eax+10h]
        fld dword ptr [edx+ebp+18h]
        lea eax,[edx+ebp]
        fsub dword ptr [eax+8]
        fld dword ptr [eax+14h]
        fsub dword ptr [eax+4]
        fld dword ptr [eax+10h]
        fsub dword ptr [eax]
        lea eax,[esp+18h]
        push eax
        push ecx
        fstp dword ptr [esp+20h]
        lea ecx,[esp+44h]
        push ecx
        fstp dword ptr [esp+28h]
        fstp dword ptr [esp+2Ch]
        call dword ptr [BikeControl355Calls+140]
        lea ecx,[esp+ebx+000000CCh]
        mov ebx,dword ptr [eax]
        mov edx,ecx
        mov dword ptr [edx],ebx
        mov ebx,dword ptr [eax+4]
        mov dword ptr [edx+4],ebx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        mov dword ptr [edx+8],eax
        call dword ptr [BikeControl355Calls+148]
        mov ebx,dword ptr [esp+4Ch]
    bike_control_006B9F54:
        mov eax,dword ptr [esp+10h]
        add eax,4
        add ebp,20h
        add ebx,0Ch
        add edi,2Ch
        cmp ebp,80h
        mov dword ptr [esp+10h],eax
        mov dword ptr [esp+4Ch],ebx
        jl bike_control_006B9EA0
        lea eax,[esi+00000710h]
        xor edi,edi
        mov dword ptr [esp+34h],eax
        lea ecx,[esp+00000150h]
        lea eax,[esi+00000670h]
        mov dword ptr [esi+00000748h],edi
        mov dword ptr [esi+00000744h],edi
        mov byte ptr [esi+0000074Fh],0
        mov byte ptr [esi+0000074Eh],0
        mov dword ptr [esp+38h],ecx
        mov dword ptr [esp+14h],eax
        xor ebp,ebp
        jmp bike_control_006B9FC0
        lea esp,[esp+00000000h]
        mov edi,edi
    bike_control_006B9FC0:
        mov edx,dword ptr [esp+34h]
        fld dword ptr [edx]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA092
        cmp edi,40h
        mov ecx,dword ptr [esi+00000384h]
        mov eax,dword ptr [ecx+000000C0h]
        mov dword ptr [esp+48h],eax
        je bike_control_006B9FF1
        cmp edi,60h
        jne bike_control_006B9FFF
    bike_control_006B9FF1:
        fld dword ptr ds:[00858624h]
        fsub dword ptr [esp+48h]
        fstp dword ptr [esp+48h]
    bike_control_006B9FFF:
        mov ebx,dword ptr [esp+14h]
        fld dword ptr [ebx+8]
        mov edx,dword ptr [edx]
        fcomp dword ptr ds:[00858F9Ch]
        fnstsw ax
        test ah,41h
        mov eax,dword ptr [esp+38h]
        push eax
        mov eax,dword ptr [esp+4Ch]
        jne bike_control_006BA041
        push ebx
        push eax
        push edx
        lea eax,[esp+ebp+00000100h]
        push eax
        mov eax,dword ptr [ecx+000000ACh]
        lea edx,[esp+ebp+000000D4h]
        push edx
        push eax
        mov ecx,esi
        call dword ptr [BikeControl355Calls+96]
        jmp bike_control_006BA061
    bike_control_006BA041:
        push eax
        push edx
        lea eax,[esp+ebp+000000FCh]
        push eax
        mov eax,dword ptr [ecx+000000ACh]
        lea edx,[esp+ebp+000000D0h]
        push edx
        push eax
        mov ecx,esi
        call dword ptr [BikeControl355Calls+92]
    bike_control_006BA061:
        mov ecx,dword ptr [esp+14h]
        movzx edx,byte ptr [ecx+13h]
        xor ebx,ebx
        cmp edi,40h
        push edx
        mov ecx,0B79538h
        setge bl
        call dword ptr [BikeControl355Calls+120]
        cmp eax,2
        mov dword ptr [esi+ebx*4+00000744h],eax
        jne bike_control_006BA0C6
        mov byte ptr [ebx+esi+0000074Eh],1
        jmp bike_control_006BA0C6
    bike_control_006BA092:
        mov ecx,dword ptr [esp+74h]
        mov edx,dword ptr [ecx+10h]
        mov eax,dword ptr [esi+14h]
        lea ecx,[edx+edi+10h]
        push ecx
        push eax
        lea edx,[esp+20h]
        push edx
        call dword ptr [BikeControl355Calls+140]
        mov edx,dword ptr [eax]
        lea ecx,[esp+ebp+000000FCh]
        mov dword ptr [ecx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ecx+4],edx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        mov dword ptr [ecx+8],eax
    bike_control_006BA0C6:
        mov ebx,dword ptr [esp+38h]
        mov edx,dword ptr [esp+34h]
        mov eax,dword ptr [esp+14h]
        mov ecx,4
        add ebx,ecx
        add edx,ecx
        add eax,2Ch
        add edi,20h
        add ebp,0Ch
        cmp edi,80h
        mov dword ptr [esp+38h],ebx
        mov dword ptr [esp+34h],edx
        mov dword ptr [esp+14h],eax
        jl bike_control_006B9FC0
        lea eax,[esi+000007C0h]
        mov dword ptr [esp+34h],eax
        lea eax,[esi+000007D0h]
        mov dword ptr [esp+48h],eax
        xor edi,edi
        mov dword ptr [esp+10h],ecx
        jmp bike_control_006BA120
        lea esp,[esp+00000000h]
    bike_control_006BA120:
        sub esp,0Ch
        lea ecx,[esp+edi+000000FCh]
        mov eax,dword ptr [ecx]
        mov edx,esp
        mov dword ptr [edx],eax
        mov eax,dword ptr [ecx+4]
        mov ecx,dword ptr [ecx+8]
        mov dword ptr [edx+4],eax
        mov dword ptr [edx+8],ecx
        lea edx,[esp+24h]
        push edx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+84]
        mov edx,dword ptr [eax]
        lea ebp,[esp+edi+00000120h]
        mov ecx,ebp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ecx+4],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [ecx+8],eax
        mov ecx,dword ptr [esp+34h]
        mov ecx,dword ptr [ecx]
        test ecx,ecx
        je bike_control_006BA1B8
        mov edx,dword ptr [esp+48h]
        mov ebx,dword ptr [edx]
        sub esp,0Ch
        mov eax,esp
        mov dword ptr [eax],ebx
        mov ebx,dword ptr [edx+4]
        mov edx,dword ptr [edx+8]
        mov dword ptr [eax+4],ebx
        mov dword ptr [eax+8],edx
        lea eax,[esp+34h]
        push eax
        call dword ptr [BikeControl355Calls+84]
        fld dword ptr [ebp]
        fsub dword ptr [eax]
        fstp dword ptr [ebp]
        fld dword ptr [esp+edi+00000124h]
        fsub dword ptr [eax+4]
        fstp dword ptr [esp+edi+00000124h]
        fld dword ptr [esp+edi+00000128h]
        fsub dword ptr [eax+8]
        fstp dword ptr [esp+edi+00000128h]
    bike_control_006BA1B8:
        mov ebx,dword ptr [esp+34h]
        mov ecx,dword ptr [esp+48h]
        mov eax,dword ptr [esp+10h]
        add ebx,4
        add ecx,0Ch
        add edi,0Ch
        dec eax
        mov dword ptr [esp+34h],ebx
        mov dword ptr [esp+48h],ecx
        mov dword ptr [esp+10h],eax
        jne bike_control_006BA120
        fld dword ptr [esi+00000710h]
        lea ebx,[esi+00000710h]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jnp bike_control_006BA210
        fld dword ptr [esi+00000714h]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA33E
    bike_control_006BA210:
        fld dword ptr [ebx]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA23B
        lea eax,[esi+00000670h]
        mov ecx,dword ptr [eax]
        mov edx,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+50h],ecx
        mov dword ptr [esp+54h],edx
        mov dword ptr [esp+58h],eax
        jmp bike_control_006BA255
    bike_control_006BA23B:
        lea ecx,[esi+0000069Ch]
        mov edx,dword ptr [ecx]
        mov eax,dword ptr [ecx+4]
        mov ecx,dword ptr [ecx+8]
        mov dword ptr [esp+50h],edx
        mov dword ptr [esp+54h],eax
        mov dword ptr [esp+58h],ecx
    bike_control_006BA255:
        fld dword ptr [esp+58h]
        fcomp dword ptr ds:[00858F9Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BA2A5
        fld dword ptr [esp+58h]
        fchs
        fld dword ptr [esp+54h]
        fchs
        fld dword ptr [esp+50h]
        fchs
        fstp dword ptr [esp+28h]
        mov edx,dword ptr [esp+28h]
        mov dword ptr [esp+000000C0h],edx
        fstp dword ptr [esp+2Ch]
        mov eax,dword ptr [esp+2Ch]
        mov dword ptr [esp+000000C4h],eax
        fstp dword ptr [esp+30h]
        mov ecx,dword ptr [esp+30h]
        mov dword ptr [esp+000000C8h],ecx
    bike_control_006BA2A5:
        fld dword ptr [esi+00000714h]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA2D4
        lea edx,[esi+0000069Ch]
        mov eax,dword ptr [edx]
        mov ecx,dword ptr [edx+4]
        mov edx,dword ptr [edx+8]
        mov dword ptr [esp+50h],eax
        mov dword ptr [esp+54h],ecx
        mov dword ptr [esp+58h],edx
        jmp bike_control_006BA2EE
    bike_control_006BA2D4:
        lea eax,[esi+00000670h]
        mov ecx,dword ptr [eax]
        mov edx,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+50h],ecx
        mov dword ptr [esp+54h],edx
        mov dword ptr [esp+58h],eax
    bike_control_006BA2EE:
        fld dword ptr [esp+58h]
        fcomp dword ptr ds:[00858F9Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BA33E
        fld dword ptr [esp+58h]
        fchs
        fld dword ptr [esp+54h]
        fchs
        fld dword ptr [esp+50h]
        fchs
        fstp dword ptr [esp+28h]
        mov ecx,dword ptr [esp+28h]
        mov dword ptr [esp+000000CCh],ecx
        fstp dword ptr [esp+2Ch]
        mov edx,dword ptr [esp+2Ch]
        mov dword ptr [esp+000000D0h],edx
        fstp dword ptr [esp+30h]
        mov eax,dword ptr [esp+30h]
        mov dword ptr [esp+000000D4h],eax
    bike_control_006BA33E:
        fld dword ptr [esi+00000718h]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jnp bike_control_006BA368
        fld dword ptr [esi+0000071Ch]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA49A
    bike_control_006BA368:
        fld dword ptr [esi+00000718h]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA397
        lea ecx,[esi+000006C8h]
        mov edx,dword ptr [ecx]
        mov eax,dword ptr [ecx+4]
        mov ecx,dword ptr [ecx+8]
        mov dword ptr [esp+50h],edx
        mov dword ptr [esp+54h],eax
        mov dword ptr [esp+58h],ecx
        jmp bike_control_006BA3B1
    bike_control_006BA397:
        lea edx,[esi+000006F4h]
        mov eax,dword ptr [edx]
        mov ecx,dword ptr [edx+4]
        mov edx,dword ptr [edx+8]
        mov dword ptr [esp+50h],eax
        mov dword ptr [esp+54h],ecx
        mov dword ptr [esp+58h],edx
    bike_control_006BA3B1:
        fld dword ptr [esp+58h]
        fcomp dword ptr ds:[00858F9Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BA401
        fld dword ptr [esp+58h]
        fchs
        fld dword ptr [esp+54h]
        fchs
        fld dword ptr [esp+50h]
        fchs
        fstp dword ptr [esp+28h]
        mov eax,dword ptr [esp+28h]
        mov dword ptr [esp+000000D8h],eax
        fstp dword ptr [esp+2Ch]
        mov ecx,dword ptr [esp+2Ch]
        mov dword ptr [esp+000000DCh],ecx
        fstp dword ptr [esp+30h]
        mov edx,dword ptr [esp+30h]
        mov dword ptr [esp+000000E0h],edx
    bike_control_006BA401:
        fld dword ptr [esi+0000071Ch]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA430
        lea eax,[esi+000006F4h]
        mov ecx,dword ptr [eax]
        mov edx,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+50h],ecx
        mov dword ptr [esp+54h],edx
        mov dword ptr [esp+58h],eax
        jmp bike_control_006BA44A
    bike_control_006BA430:
        lea ecx,[esi+000006C8h]
        mov edx,dword ptr [ecx]
        mov eax,dword ptr [ecx+4]
        mov ecx,dword ptr [ecx+8]
        mov dword ptr [esp+50h],edx
        mov dword ptr [esp+54h],eax
        mov dword ptr [esp+58h],ecx
    bike_control_006BA44A:
        fld dword ptr [esp+58h]
        fcomp dword ptr ds:[00858F9Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BA49A
        fld dword ptr [esp+58h]
        fchs
        fld dword ptr [esp+54h]
        fchs
        fld dword ptr [esp+50h]
        fchs
        fstp dword ptr [esp+28h]
        mov edx,dword ptr [esp+28h]
        mov dword ptr [esp+000000E4h],edx
        fstp dword ptr [esp+2Ch]
        mov eax,dword ptr [esp+2Ch]
        mov dword ptr [esp+000000E8h],eax
        fstp dword ptr [esp+30h]
        mov ecx,dword ptr [esp+30h]
        mov dword ptr [esp+000000ECh],ecx
    bike_control_006BA49A:
        lea ebp,[esp+00000150h]
        xor edi,edi
        mov dword ptr [esp+10h],4
        jmp bike_control_006BA4B0
        lea ecx,[ecx]
    bike_control_006BA4B0:
        fld dword ptr [ebx]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA4EF
        lea edx,[esp+edi+00000120h]
        push edx
        mov edx,dword ptr [ebp]
        lea eax,[esp+edi+000000F4h]
        push eax
        mov eax,dword ptr [esi+00000384h]
        lea ecx,[esp+edi+000000C8h]
        push ecx
        mov ecx,dword ptr [eax+000000B0h]
        push edx
        push ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+100]
    bike_control_006BA4EF:
        mov eax,dword ptr [esp+10h]
        add edi,0Ch
        add ebx,4
        add ebp,4
        dec eax
        mov dword ptr [esp+10h],eax
        jne bike_control_006BA4B0
        lea eax,[esi+000007C0h]
        mov dword ptr [esp+34h],eax
        lea eax,[esi+000007D0h]
        mov dword ptr [esp+48h],eax
        xor edi,edi
        mov dword ptr [esp+10h],4
    bike_control_006BA521:
        sub esp,0Ch
        lea edx,[esp+edi+000000FCh]
        mov ecx,dword ptr [edx]
        mov eax,esp
        mov dword ptr [eax],ecx
        mov ecx,dword ptr [edx+4]
        mov edx,dword ptr [edx+8]
        mov dword ptr [eax+4],ecx
        mov dword ptr [eax+8],edx
        lea eax,[esp+24h]
        push eax
        mov ecx,esi
        call dword ptr [BikeControl355Calls+84]
        mov edx,dword ptr [eax]
        lea ebp,[esp+edi+00000120h]
        mov ecx,ebp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ecx+4],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [ecx+8],eax
        mov ecx,dword ptr [esp+34h]
        mov ecx,dword ptr [ecx]
        test ecx,ecx
        je bike_control_006BA5B9
        mov edx,dword ptr [esp+48h]
        mov ebx,dword ptr [edx]
        sub esp,0Ch
        mov eax,esp
        mov dword ptr [eax],ebx
        mov ebx,dword ptr [edx+4]
        mov edx,dword ptr [edx+8]
        mov dword ptr [eax+4],ebx
        mov dword ptr [eax+8],edx
        lea eax,[esp+34h]
        push eax
        call dword ptr [BikeControl355Calls+84]
        fld dword ptr [ebp]
        fsub dword ptr [eax]
        fstp dword ptr [ebp]
        fld dword ptr [esp+edi+00000124h]
        fsub dword ptr [eax+4]
        fstp dword ptr [esp+edi+00000124h]
        fld dword ptr [esp+edi+00000128h]
        fsub dword ptr [eax+8]
        fstp dword ptr [esp+edi+00000128h]
    bike_control_006BA5B9:
        mov ebx,dword ptr [esp+34h]
        mov ecx,dword ptr [esp+48h]
        mov eax,dword ptr [esp+10h]
        add ebx,4
        add ecx,0Ch
        add edi,0Ch
        dec eax
        mov dword ptr [esp+34h],ebx
        mov dword ptr [esp+48h],ecx
        mov dword ptr [esp+10h],eax
        jne bike_control_006BA521
        mov eax,dword ptr [esi+14h]
        fld dword ptr [esi+4Ch]
        fmul dword ptr [eax+18h]
        xor ecx,ecx
        mov cl,byte ptr ds:[0096914Ch]
        fld dword ptr [esi+48h]
        fmul dword ptr [eax+14h]
        xor edx,edx
        mov dl,byte ptr [esi+00000805h]
        faddp st(1),st
        fld dword ptr [esi+44h]
        fmul dword ptr [eax+10h]
        lea eax,[esi+0000049Ch]
        push ecx
        lea ecx,[esp+00000084h]
        push edx
        faddp st(1),st
        push 0
        push 0
        push ecx
        lea edx,[esi+000004B8h]
        push edx
        lea ecx,[esi+000004B4h]
        push ecx
        mov ecx,dword ptr [esi+00000384h]
        fstp dword ptr [esp+0000009Ch]
        push eax
        add ecx,2Ch
        call dword ptr [BikeControl355Calls+164]
        fdiv dword ptr [esi+00000094h]
        mov ecx,dword ptr [esi+00000384h]
        mov al,byte ptr [esi+36h]
        shr al,3
        test al,al
        fstp dword ptr [esp+00000084h]
        fld dword ptr [ecx+00000094h]
        fmul dword ptr [esi+000004A0h]
        fmul dword ptr ds:[00B7CB5Ch]
        fstp dword ptr [esp+48h]
        je bike_control_006BA6A7
        cmp al,8
        je bike_control_006BA6A7
        test byte ptr [ecx+000000D0h],8
        je bike_control_006BA6A7
        mov dword ptr [esp+10h],3F800000h
        mov dword ptr [esp+64h],3F800000h
        mov dword ptr [esp+34h],3F800000h
        mov dword ptr [esp+0000008Ch],3F800000h
        jmp bike_control_006BA6E2
    bike_control_006BA6A7:
        fld dword ptr [ecx+00000098h]
        fadd st(0),st
        fstp dword ptr [esp+10h]
        fld dword ptr ds:[00858624h]
        fsub dword ptr [ecx+00000098h]
        fadd st(0),st
        fstp dword ptr [esp+64h]
        fld dword ptr [ecx+000000A8h]
        fadd st(0),st
        fstp dword ptr [esp+34h]
        fld dword ptr ds:[00858CA0h]
        fsub dword ptr [esp+34h]
        fstp dword ptr [esp+0000008Ch]
    bike_control_006BA6E2:
        mov dl,byte ptr [esi+00000805h]
        mov byte ptr [esi+00000806h],dl
        mov byte ptr [esi+00000804h],0
        mov byte ptr [esi+00000805h],0
        xor ecx,ecx
        lea edx,[esi+00000670h]
        lea edi,[esi+00000730h]
        lea ebx,[ebx+00000000h]
    bike_control_006BA710:
        fld dword ptr [edi-20h]
        fcomp dword ptr ds:[00871284h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA728
        mov dword ptr [edi],40800000h
        jmp bike_control_006BA754
    bike_control_006BA728:
        fld dword ptr [edi]
        fsub dword ptr ds:[00B7CB5Ch]
        fcom dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BA745
        fstp st(0)
        fld dword ptr ds:[00858B50h]
    bike_control_006BA745:
        fst dword ptr [edi]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BA7BF
    bike_control_006BA754:
        mov al,byte ptr [esi+00000804h]
        inc al
        cmp cx,2
        mov byte ptr [esi+00000804h],al
        je bike_control_006BA76E
        cmp cx,3
        jne bike_control_006BA775
    bike_control_006BA76E:
        mov byte ptr [esi+00000805h],1
    bike_control_006BA775:
        cmp al,1
        jne bike_control_006BA793
        mov ebx,edx
        mov ebp,dword ptr [ebx]
        lea eax,[esi+00000618h]
        mov dword ptr [eax],ebp
        mov ebp,dword ptr [ebx+4]
        mov dword ptr [eax+4],ebp
        mov ebx,dword ptr [ebx+8]
        mov dword ptr [eax+8],ebx
        jmp bike_control_006BA7BF
    bike_control_006BA793:
        fld dword ptr [esi+00000618h]
        fadd dword ptr [edx]
        fstp dword ptr [esi+00000618h]
        fld dword ptr [edx+4]
        fadd dword ptr [esi+0000061Ch]
        fstp dword ptr [esi+0000061Ch]
        fld dword ptr [edx+8]
        fadd dword ptr [esi+00000620h]
        fstp dword ptr [esi+00000620h]
    bike_control_006BA7BF:
        inc ecx
        add edi,4
        add edx,2Ch
        cmp cx,4
        jb bike_control_006BA710
        mov al,byte ptr [esi+00000804h]
        test al,al
        jbe bike_control_006BA87C
        movzx ecx,al
        mov dword ptr [esp+4Ch],ecx
        lea ecx,[esi+00000618h]
        mov edx,ecx
        fild dword ptr [esp+4Ch]
        fld dword ptr ds:[00858624h]
        fdiv st,st(1)
        fstp dword ptr [esp+38h]
        fstp st(0)
        fld dword ptr [esp+38h]
        fmul dword ptr [ecx+8]
        fld dword ptr [esp+38h]
        fmul dword ptr [ecx+4]
        fld dword ptr [esp+38h]
        fmul dword ptr [ecx]
        fstp dword ptr [esp+28h]
        mov eax,dword ptr [esp+28h]
        mov dword ptr [edx],eax
        fstp dword ptr [esp+2Ch]
        mov eax,dword ptr [esp+2Ch]
        mov dword ptr [edx+4],eax
        fstp dword ptr [esp+30h]
        mov eax,dword ptr [esp+30h]
        mov dword ptr [edx+8],eax
        mov eax,dword ptr [esi+14h]
        fld dword ptr [eax+28h]
        fmul dword ptr [ecx+8]
        fld dword ptr [eax+24h]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [eax+20h]
        fmul dword ptr [ecx]
        faddp st(1),st
        fcomp dword ptr ds:[00858F40h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA8AE
        fld dword ptr [ecx]
        fmul dword ptr ds:[00858C1Ch]
        fstp dword ptr [ecx]
        fld dword ptr [ecx+4]
        fmul dword ptr ds:[00858C1Ch]
        fstp dword ptr [ecx+4]
        fld dword ptr [ecx+8]
        fmul dword ptr ds:[00858C1Ch]
        fstp dword ptr [ecx+8]
        jmp bike_control_006BA8AE
    bike_control_006BA87C:
        mov dword ptr [esp+28h],0
        mov ecx,dword ptr [esp+28h]
        lea eax,[esi+00000618h]
        mov dword ptr [eax],ecx
        mov dword ptr [esp+2Ch],0
        mov edx,dword ptr [esp+2Ch]
        mov dword ptr [esp+30h],3F800000h
        mov ecx,dword ptr [esp+30h]
        mov dword ptr [eax+4],edx
        mov dword ptr [eax+8],ecx
    bike_control_006BA8AE:
        fld dword ptr [esi+00000710h]
        fcomp dword ptr [esi+00000714h]
        fnstsw ax
        test ah,5
        jp bike_control_006BA8C5
        xor cl,cl
        jmp bike_control_006BA8C7
    bike_control_006BA8C5:
        mov cl,1
    bike_control_006BA8C7:
        mov edi,dword ptr [esp+74h]
        mov eax,dword ptr [edi+10h]
        mov ebp,dword ptr [esp+60h]
        mov dword ptr [esp+00000090h],0
        mov edx,dword ptr [eax+4]
        movzx ecx,cl
        mov dword ptr [esp+00000094h],edx
        mov dword ptr [esp+4Ch],ecx
        fld dword ptr [esi+ecx*4+00000710h]
        lea ecx,[esp+00000090h]
        fmul dword ptr [esi+00000770h]
        push ecx
        lea edx,[esp+1Ch]
        fsubr dword ptr [eax+8]
        mov eax,dword ptr [esi+14h]
        fld dword ptr [ebp+40h]
        push eax
        fmul dword ptr ds:[00858B8Ch]
        push edx
        fsubp st(1),st
        fstp dword ptr [esp+000000A4h]
        call dword ptr [BikeControl355Calls+140]
        mov ecx,dword ptr [eax]
        fld dword ptr [esi+00000718h]
        fcomp dword ptr [esi+0000071Ch]
        mov dword ptr [esp+0000009Ch],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+000000A0h],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+000000A4h],eax
        fnstsw ax
        add esp,0Ch
        test ah,5
        mov cl,2
        jnp bike_control_006BA95D
        mov cl,3
    bike_control_006BA95D:
        mov eax,dword ptr [edi+10h]
        mov dword ptr [esp+0000009Ch],0
        mov edx,dword ptr [eax+64h]
        movzx ebx,cl
        mov dword ptr [esp+000000A0h],edx
        lea ecx,[esp+0000009Ch]
        fld dword ptr [esi+ebx*4+00000710h]
        push ecx
        fmul dword ptr [esi+00000778h]
        lea edx,[esp+1Ch]
        fsubr dword ptr [eax+48h]
        mov eax,dword ptr [esi+14h]
        fld dword ptr [ebp+44h]
        push eax
        fmul dword ptr ds:[00858B8Ch]
        push edx
        fsubp st(1),st
        fstp dword ptr [esp+000000B0h]
        call dword ptr [BikeControl355Calls+140]
        mov ecx,dword ptr [eax]
        mov edi,dword ptr [esi+00000384h]
        mov dword ptr [esp+000000A8h],ecx
        mov edx,dword ptr [eax+4]
        mov cl,byte ptr [esi+36h]
        mov dword ptr [esp+000000ACh],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+000000B0h],eax
        fld dword ptr [edi+28h]
        fmul dword ptr [esi+00000794h]
        shr cl,3
        add esp,0Ch
        test cl,cl
        fmul dword ptr ds:[00C1C818h]
        fmul dword ptr ds:[00858C84h]
        fstp dword ptr [esp+38h]
        je bike_control_006BAA35
        mov al,byte ptr [esi+00000614h]
        test al,10h
        je bike_control_006BAA35
        test al,8
        jne bike_control_006BAA35
        fld dword ptr [esi+00000644h]
        fcomp dword ptr ds:[008631B4h]
        fnstsw ax
        test ah,5
        jp bike_control_006BAC22
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[008631A8h]
        fadd dword ptr [esi+00000644h]
        jmp bike_control_006BAC1C
    bike_control_006BAA35:
        fld dword ptr [esi+44h]
        fabs
        fcomp dword ptr ds:[00858C58h]
        fnstsw ax
        test ah,5
        jp bike_control_006BAA88
        fld dword ptr [esi+48h]
        fabs
        fcomp dword ptr ds:[00858C58h]
        fnstsw ax
        test ah,5
        jp bike_control_006BAA88
        fld dword ptr [esi+00000494h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jp bike_control_006BAA88
        fld qword ptr ds:[0086F0E0h]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [BikeControl355Calls+204]
        fmul dword ptr [esi+00000644h]
        jmp bike_control_006BAC1C
    bike_control_006BAA88:
        fld dword ptr [esp+00000080h]
        fcomp dword ptr ds:[00858C58h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BAC02
        fld dword ptr [esi+00000730h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BAACA
        fld dword ptr [esi+00000734h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BAC02
    bike_control_006BAACA:
        test cl,cl
        jne bike_control_006BAC02
        lea ecx,[esp+00000160h]
        push ecx
        mov ecx,0B79538h
        mov byte ptr [esp+00000184h],3Ch
        mov byte ptr [esp+00000187h],1
        call dword ptr [BikeControl355Calls+128]
        mov edx,dword ptr [esi+0000063Ch]
        fmul dword ptr [edx+20h]
        mov eax,ebx
        imul eax,eax,2Ch
        fmul dword ptr [esp+38h]
        fmul dword ptr ds:[00858B90h]
        movzx ecx,byte ptr [eax+esi+00000683h]
        fstp dword ptr [esp+14h]
        lea edi,[eax+esi+00000683h]
        push ecx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+112]
        cmp eax,3
        je bike_control_006BAB42
        movzx edx,byte ptr [edi]
        push edx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+112]
        cmp eax,4
        jne bike_control_006BAB51
    bike_control_006BAB42:
        mov eax,dword ptr [esi+0000063Ch]
        fld dword ptr [esp+14h]
        fmul dword ptr [eax+24h]
        jmp bike_control_006BAB55
    bike_control_006BAB51:
        fld dword ptr [esp+14h]
    bike_control_006BAB55:
        fld dword ptr [esp+00000080h]
        fmul dword ptr [esp+00000080h]
        fdivp st(1),st
        fld dword ptr ds:[00858624h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BAB80
        fstp st(0)
        mov dword ptr [esp+78h],3F800000h
        jmp bike_control_006BAB84
    bike_control_006BAB80:
        fstp dword ptr [esp+78h]
    bike_control_006BAB84:
        fld dword ptr [esp+78h]
        mov edi,dword ptr [esi+00000384h]
        call dword ptr [BikeControl355Calls+200]
        fld dword ptr [edi+000000A0h]
        fmul dword ptr ds:[008595ECh]
        fdivp st(1),st
        fld dword ptr [esi+00000494h]
        fcomp qword ptr ds:[00859EF8h]
        fnstsw ax
        test ah,5
        jp bike_control_006BABC7
        fld dword ptr [esi+00000648h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jnp bike_control_006BABED
    bike_control_006BABC7:
        fld dword ptr [esi+00000494h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BABF1
        fld dword ptr [esi+00000648h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BABF1
    bike_control_006BABED:
        fld st(0)
        faddp st(1),st
    bike_control_006BABF1:
        fld dword ptr ds:[00858624h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BAC08
        fstp st(0)
    bike_control_006BAC02:
        fld dword ptr ds:[00858624h]
    bike_control_006BAC08:
        test byte ptr [esi+36h],0F8h
        je bike_control_006BAC16
        fstp st(0)
        fld dword ptr ds:[00858624h]
    bike_control_006BAC16:
        fmul dword ptr [esi+00000494h]
    bike_control_006BAC1C:
        fstp dword ptr [esi+00000644h]
    bike_control_006BAC22:
        lea eax,[esi+44h]
        mov ecx,dword ptr [eax]
        mov edx,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+58h],eax
        mov eax,dword ptr [edi+000000D0h]
        shr eax,17h
        and al,1
        mov dword ptr [esp+50h],ecx
        mov dword ptr [esp+54h],edx
        mov byte ptr [esp+0000008Bh],al
        jne bike_control_006BAF62
        fld dword ptr [esi+00000730h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BAC9A
        fld dword ptr [esi+00000734h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BAC9A
        fld dword ptr [esi+00000758h]
        fmul dword ptr ds:[00858EF0h]
        fst dword ptr [esi+00000758h]
        fadd dword ptr [esi+00000750h]
        fstp dword ptr [esi+00000750h]
        jmp bike_control_006BAF62
    bike_control_006BAC9A:
        fld dword ptr [esi+00000644h]
        mov eax,dword ptr [esi+14h]
        fsin
        lea ecx,[esp+18h]
        push ecx
        push eax
        lea edx,[esp+000000B0h]
        push edx
        mov dword ptr [esp+2Ch],0
        fld dword ptr [esi+00000644h]
        fcos
        fxch st(1)
        fchs
        fstp dword ptr [esp+24h]
        fstp dword ptr [esp+28h]
        call dword ptr [BikeControl355Calls+140]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+48h],ecx
        mov ecx,dword ptr [esp+58h]
        mov edx,dword ptr [eax+4]
        imul ecx,ecx,2Ch
        mov dword ptr [esp+4Ch],edx
        mov eax,dword ptr [eax+8]
        fld dword ptr [esp+4Ch]
        lea ebp,[ecx+esi]
        lea edi,[ebp+00000670h]
        mov dword ptr [esp+50h],eax
        add esp,0Ch
        fmul dword ptr [edi+4]
        lea ecx,[esp+3Ch]
        fld dword ptr [esp+44h]
        fmul dword ptr [edi+8]
        faddp st(1),st
        fld dword ptr [esp+3Ch]
        fmul dword ptr [edi]
        faddp st(1),st
        fld st(0)
        fmul dword ptr [edi+8]
        fld st(1)
        fmul dword ptr [edi+4]
        fxch st(2)
        fmul dword ptr [edi]
        fld dword ptr [esp+3Ch]
        fsub st,st(1)
        fstp dword ptr [esp+3Ch]
        fstp st(0)
        fld dword ptr [esp+40h]
        fsub st,st(2)
        fstp dword ptr [esp+40h]
        fstp st(1)
        fld dword ptr [esp+44h]
        fsub st,st(1)
        fstp dword ptr [esp+44h]
        fstp st(0)
        call dword ptr [BikeControl355Calls+148]
        push edi
        lea edx,[esp+40h]
        push edx
        lea eax,[esp+000000B0h]
        push eax
        call dword ptr [BikeControl355Calls+136]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+34h],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+38h],edx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        lea ecx,[esp+28h]
        mov dword ptr [esp+30h],eax
        call dword ptr [BikeControl355Calls+148]
        mov al,byte ptr [esp+27h]
        test al,al
        je bike_control_006BAD91
        mov dword ptr [esp+30h],0
    bike_control_006BAD91:
        lea ecx,[ebp+00000660h]
        push ecx
        mov ecx,0B79538h
        mov dword ptr ds:[00C1C27Ch],0
        mov byte ptr [ebp+00000680h],3Ch
        call dword ptr [BikeControl355Calls+128]
        fmul dword ptr [esp+38h]
        mov dword ptr [esp+74h],3F800000h
        fstp dword ptr [esp+14h]
        fld dword ptr [esi+000007B0h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BADFF
        movzx edx,byte ptr [ebp+00000683h]
        push edx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+112]
        dec eax
        je bike_control_006BADF5
        dec eax
        je bike_control_006BAFA5
        dec eax
        jne bike_control_006BADFF
    bike_control_006BADF5:
        mov ecx,dword ptr ds:[008712F8h]
        mov dword ptr [esp+74h],ecx
    bike_control_006BADFF:
        test byte ptr [esi+36h],0F8h
        jne bike_control_006BAE1F
        movzx edx,byte ptr [ebp+00000683h]
        push edx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+116]
        fmul dword ptr [esp+14h]
        fstp dword ptr [esp+14h]
    bike_control_006BAE1F:
        cmp byte ptr [esi+0000065Ch],1
        jne bike_control_006BAE36
        fld dword ptr [esp+14h]
        fmul dword ptr ds:[0087128Ch]
        fstp dword ptr [esp+14h]
    bike_control_006BAE36:
        mov eax,dword ptr [esi+0000080Ch]
        mov edx,dword ptr [esp+00000090h]
        sub esp,0Ch
        mov ecx,esp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+000000A4h]
        mov dword ptr ds:[00C1C26Ch],eax
        mov eax,dword ptr [esp+000000A0h]
        mov dword ptr [ecx+4],eax
        lea eax,[esp+000000B4h]
        mov dword ptr [ecx+8],edx
        push eax
        mov ecx,esi
        call dword ptr [BikeControl355Calls+84]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+68h],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+6Ch],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+70h],eax
        mov eax,dword ptr [esp+4Ch]
        mov ecx,dword ptr [esi+eax*4+000007C0h]
        test ecx,ecx
        je bike_control_006BAEE1
        lea edx,[eax+eax*2]
        lea eax,[esi+edx*4+000007D0h]
        mov edi,dword ptr [eax]
        sub esp,0Ch
        mov edx,esp
        mov dword ptr [edx],edi
        mov edi,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [edx+4],edi
        mov dword ptr [edx+8],eax
        lea edx,[esp+000000B4h]
        push edx
        call dword ptr [BikeControl355Calls+84]
        fld dword ptr [esp+68h]
        fsub dword ptr [eax]
        fstp dword ptr [esp+68h]
        fld dword ptr [esp+6Ch]
        fsub dword ptr [eax+4]
        fstp dword ptr [esp+6Ch]
        fld dword ptr [esp+70h]
        fsub dword ptr [eax+8]
        fstp dword ptr [esp+70h]
    bike_control_006BAEE1:
        movzx ax,byte ptr [esi+0000065Ch]
        fld dword ptr [esp+14h]
        mov edx,dword ptr [esp+74h]
        fmul dword ptr [esp+34h]
        lea ecx,[esi+00000758h]
        push eax
        mov eax,dword ptr ds:[00C1C27Ch]
        push 0
        push 0C1C26Ch
        push ecx
        push 0
        push edx
        sub esp,8
        fstp dword ptr [esp+4]
        lea ecx,[esp+000000B0h]
        fld dword ptr [esp+30h]
        lea edx,[esp+00000088h]
        fmul dword ptr [esp+68h]
        fstp dword ptr [esp]
        push eax
        push 2
        push ecx
        push edx
        lea eax,[esp+58h]
        push eax
        lea ecx,[esp+70h]
        push ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+184]
        test byte ptr [esp+7Ch],4
        je bike_control_006BAF62
        mov eax,dword ptr ds:[00C1C26Ch]
        cmp eax,1
        je bike_control_006BAF58
        cmp eax,2
        jne bike_control_006BAF62
    bike_control_006BAF58:
        mov dword ptr ds:[00C1C26Ch],0
    bike_control_006BAF62:
        fld dword ptr [esi+00000738h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BB038
        fld dword ptr [esi+0000073Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BB038
        test byte ptr [esi+00000428h],20h
        je bike_control_006BAFB3
        mov dword ptr [esi+0000075Ch],0
        jmp bike_control_006BB01B
    bike_control_006BAFA5:
        mov eax,dword ptr ds:[008712F4h]
        mov dword ptr [esp+74h],eax
        jmp bike_control_006BADFF
    bike_control_006BAFB3:
        fld dword ptr [esp+00000084h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jnp bike_control_006BB01B
        fld dword ptr [esp+00000084h]
        fcomp dword ptr ds:[00858B50h]
        fld dword ptr [esi+0000075Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BAFFC
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jp bike_control_006BB01B
        fld dword ptr [esi+0000075Ch]
        fsub dword ptr ds:[00858B1Ch]
        jmp bike_control_006BB015
    bike_control_006BAFFC:
        fcomp dword ptr ds:[00858C1Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BB01B
        fld dword ptr [esi+0000075Ch]
        fadd dword ptr ds:[00858C28h]
    bike_control_006BB015:
        fstp dword ptr [esi+0000075Ch]
    bike_control_006BB01B:
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr [esi+0000075Ch]
        fadd dword ptr [esi+00000754h]
        fstp dword ptr [esi+00000754h]
        jmp bike_control_006BB4E6
    bike_control_006BB038:
        mov eax,dword ptr [esp+48h]
        mov edx,dword ptr [esp+38h]
        mov dword ptr [esp+78h],eax
        mov eax,dword ptr [esi+14h]
        lea ecx,[eax+10h]
        mov dword ptr [esp+74h],edx
        mov edx,dword ptr [ecx]
        mov dword ptr [esp+3Ch],edx
        mov edx,dword ptr [ecx+4]
        mov dword ptr [esp+40h],edx
        fld dword ptr [esp+40h]
        mov ecx,dword ptr [ecx+8]
        mov dword ptr [esp+44h],ecx
        mov edx,dword ptr [eax]
        mov dword ptr [esp+28h],edx
        mov ecx,dword ptr [eax+4]
        mov dword ptr [esp+2Ch],ecx
        mov edx,dword ptr [eax+8]
        mov eax,ebx
        imul eax,eax,2Ch
        lea ebp,[eax+esi]
        lea edi,[ebp+00000670h]
        lea ecx,[esp+3Ch]
        mov dword ptr [esp+30h],edx
        fmul dword ptr [edi+4]
        fld dword ptr [esp+44h]
        fmul dword ptr [edi+8]
        faddp st(1),st
        fld dword ptr [esp+3Ch]
        fmul dword ptr [edi]
        faddp st(1),st
        fld st(0)
        fmul dword ptr [edi+8]
        fld st(1)
        fmul dword ptr [edi+4]
        fxch st(2)
        fmul dword ptr [edi]
        fld dword ptr [esp+3Ch]
        fsub st,st(1)
        fstp dword ptr [esp+3Ch]
        fstp st(0)
        fld dword ptr [esp+40h]
        fsub st,st(2)
        fstp dword ptr [esp+40h]
        fstp st(1)
        fld dword ptr [esp+44h]
        fsub st,st(1)
        fstp dword ptr [esp+44h]
        fstp st(0)
        call dword ptr [BikeControl355Calls+148]
        push edi
        lea ecx,[esp+40h]
        push ecx
        lea edx,[esp+000000B0h]
        push edx
        call dword ptr [BikeControl355Calls+136]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+34h],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+38h],edx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        lea ecx,[esp+28h]
        mov dword ptr [esp+30h],eax
        call dword ptr [BikeControl355Calls+148]
        mov al,byte ptr [esp+27h]
        test al,al
        je bike_control_006BB11A
        mov dword ptr [esp+30h],0
    bike_control_006BB11A:
        test byte ptr [esi+00000428h],20h
        je bike_control_006BB13A
        mov dword ptr [esp+78h],469C4000h
        mov dword ptr [esi+000007ACh],3F800000h
        jmp bike_control_006BB2D9
    bike_control_006BB13A:
        cmp word ptr [esi+000007A8h],0
        je bike_control_006BB1F5
        fld dword ptr [esi+00000494h]
        mov eax,dword ptr [esi+14h]
        fmul dword ptr [esi+00000090h]
        sub esp,0Ch
        mov ecx,esp
        sub esp,0Ch
        fmul dword ptr ds:[008712F0h]
        mov dword ptr [esp+00000090h],0
        mov dword ptr [esp+0000008Ch],0
        fld st(0)
        fmul dword ptr [eax+8]
        fstp dword ptr [esp+78h]
        fld st(0)
        fmul dword ptr [eax+4]
        fstp dword ptr [esp+2Ch]
        fmul dword ptr [eax]
        fstp dword ptr [esp+30h]
        fld dword ptr [esp+78h]
        fmul dword ptr ds:[00B7CB5Ch]
        fld dword ptr [esp+2Ch]
        fmul dword ptr ds:[00B7CB5Ch]
        fld dword ptr [esp+30h]
        fmul dword ptr ds:[00B7CB5Ch]
        fstp dword ptr [esp+30h]
        mov edx,dword ptr [esp+30h]
        mov dword ptr [ecx],edx
        fstp dword ptr [esp+34h]
        mov eax,dword ptr [esp+34h]
        mov dword ptr [ecx+4],eax
        fstp dword ptr [esp+38h]
        mov edx,dword ptr [esp+38h]
        mov dword ptr [ecx+8],edx
        mov ecx,dword ptr [esp+00000120h]
        mov edx,dword ptr [esp+00000124h]
        mov eax,esp
        mov dword ptr [eax],ecx
        mov ecx,dword ptr [esp+00000128h]
        mov dword ptr [eax+4],edx
        mov dword ptr [eax+8],ecx
        jmp bike_control_006BB2D2
    bike_control_006BB1F5:
        fld dword ptr [esi+000007ACh]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jp bike_control_006BB2D9
        fld dword ptr [esi+0000049Ch]
        fcomp dword ptr ds:[00858F34h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BB2D9
        fld dword ptr [esp+38h]
        mov eax,dword ptr [esi+14h]
        fmul dword ptr [esi+000007ACh]
        sub esp,0Ch
        mov edx,esp
        sub esp,0Ch
        fstp dword ptr [esp+0000008Ch]
        fld dword ptr ds:[00858624h]
        fsub dword ptr [esi+000007ACh]
        fmul dword ptr [esi+00000494h]
        fmul dword ptr [esi+00000090h]
        fmul dword ptr ds:[008712F0h]
        fld st(0)
        fmul dword ptr [eax+8]
        fstp dword ptr [esp+78h]
        fld st(0)
        fmul dword ptr [eax+4]
        fstp dword ptr [esp+2Ch]
        fmul dword ptr [eax]
        fstp dword ptr [esp+30h]
        fld dword ptr [esp+78h]
        fmul dword ptr ds:[00B7CB5Ch]
        fld dword ptr [esp+2Ch]
        fmul dword ptr ds:[00B7CB5Ch]
        fld dword ptr [esp+30h]
        fmul dword ptr ds:[00B7CB5Ch]
        fstp dword ptr [esp+30h]
        mov eax,dword ptr [esp+30h]
        mov dword ptr [edx],eax
        fstp dword ptr [esp+34h]
        mov ecx,dword ptr [esp+34h]
        mov dword ptr [edx+4],ecx
        fstp dword ptr [esp+38h]
        mov eax,dword ptr [esp+38h]
        mov dword ptr [edx+8],eax
        mov edx,dword ptr [esp+00000120h]
        mov eax,dword ptr [esp+00000124h]
        mov ecx,esp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+00000128h]
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+8],edx
    bike_control_006BB2D2:
        mov ecx,esi
        call dword ptr [BikeControl355Calls+80]
    bike_control_006BB2D9:
        fld dword ptr ds:[00C1C27Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BB305
        fld dword ptr [esp+48h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BB305
        mov dword ptr [esp+48h],0
    bike_control_006BB305:
        mov eax,dword ptr [esp+00000084h]
        lea ecx,[ebp+00000660h]
        push ecx
        mov ecx,0B79538h
        mov dword ptr ds:[00C1C27Ch],eax
        mov byte ptr [ebp+00000680h],3Ch
        call dword ptr [BikeControl355Calls+128]
        fmul dword ptr [esp+74h]
        mov dword ptr [esp+00000084h],3F800000h
        fstp dword ptr [esp+14h]
        fld dword ptr [esi+000007B0h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BB37B
        movzx edx,byte ptr [ebp+00000683h]
        push edx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+112]
        dec eax
        je bike_control_006BB36E
        dec eax
        je bike_control_006BB52A
        dec eax
        jne bike_control_006BB37B
    bike_control_006BB36E:
        mov ecx,dword ptr ds:[008712F8h]
        mov dword ptr [esp+00000084h],ecx
    bike_control_006BB37B:
        test byte ptr [esi+36h],0F8h
        jne bike_control_006BB39B
        movzx edx,byte ptr [ebp+00000683h]
        push edx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+116]
        fmul dword ptr [esp+14h]
        fstp dword ptr [esp+14h]
    bike_control_006BB39B:
        cmp byte ptr [esi+0000065Dh],1
        jne bike_control_006BB3B2
        fld dword ptr [esp+14h]
        fmul dword ptr ds:[0087128Ch]
        fstp dword ptr [esp+14h]
    bike_control_006BB3B2:
        mov eax,dword ptr [esi+00000810h]
        mov edx,dword ptr [esp+0000009Ch]
        sub esp,0Ch
        mov ecx,esp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+000000B0h]
        mov dword ptr ds:[00C1C270h],eax
        mov eax,dword ptr [esp+000000ACh]
        mov dword ptr [ecx+4],eax
        lea eax,[esp+000000B4h]
        mov dword ptr [ecx+8],edx
        push eax
        mov ecx,esi
        call dword ptr [BikeControl355Calls+84]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+68h],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+6Ch],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+70h],eax
        mov ecx,dword ptr [esi+ebx*4+000007C0h]
        test ecx,ecx
        je bike_control_006BB459
        lea edx,[ebx+ebx*2]
        lea eax,[esi+edx*4+000007D0h]
        mov edi,dword ptr [eax]
        sub esp,0Ch
        mov edx,esp
        mov dword ptr [edx],edi
        mov edi,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [edx+4],edi
        mov dword ptr [edx+8],eax
        lea edx,[esp+000000B4h]
        push edx
        call dword ptr [BikeControl355Calls+84]
        fld dword ptr [esp+68h]
        fsub dword ptr [eax]
        fstp dword ptr [esp+68h]
        fld dword ptr [esp+6Ch]
        fsub dword ptr [eax+4]
        fstp dword ptr [esp+6Ch]
        fld dword ptr [esp+70h]
        fsub dword ptr [eax+8]
        fstp dword ptr [esp+70h]
    bike_control_006BB459:
        movzx ax,byte ptr [esi+0000065Dh]
        fld dword ptr [esp+14h]
        mov edx,dword ptr [esp+00000084h]
        fmul dword ptr [esp+0000008Ch]
        lea ecx,[esi+0000075Ch]
        push eax
        mov eax,dword ptr ds:[00C1C27Ch]
        push 1
        push 0C1C270h
        push ecx
        push 1
        push edx
        sub esp,8
        fstp dword ptr [esp+4]
        lea ecx,[esp+000000BCh]
        fld dword ptr [esp+00000098h]
        lea edx,[esp+00000088h]
        fmul dword ptr [esp+00000084h]
        fstp dword ptr [esp]
        push eax
        push 2
        push ecx
        push edx
        lea eax,[esp+58h]
        push eax
        lea ecx,[esp+70h]
        push ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+184]
        test byte ptr [esp+7Ch],4
        je bike_control_006BB4E6
        mov eax,dword ptr ds:[00C1C270h]
        cmp eax,1
        je bike_control_006BB4DC
        cmp eax,2
        jne bike_control_006BB4E6
    bike_control_006BB4DC:
        mov dword ptr ds:[00C1C270h],0
    bike_control_006BB4E6:
        cmp word ptr [esi+000007A8h],0
        je bike_control_006BB53B
        cmp dword ptr [esi+00000810h],1
        jne bike_control_006BB53B
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00858F44h]
        fsubr dword ptr [esi+000007ACh]
        fst dword ptr [esi+000007ACh]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jp bike_control_006BB566
        mov dword ptr [esi+000007ACh],0
        jmp bike_control_006BB566
    bike_control_006BB52A:
        mov eax,dword ptr ds:[008712F4h]
        mov dword ptr [esp+00000084h],eax
        jmp bike_control_006BB37B
    bike_control_006BB53B:
        fld dword ptr [esi+000007ACh]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jp bike_control_006BB566
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00858B4Ch]
        fadd dword ptr [esi+000007ACh]
        fstp dword ptr [esi+000007ACh]
    bike_control_006BB566:
        mov al,byte ptr [esp+0000008Bh]
        test al,al
        je bike_control_006BB87A
        fld dword ptr [esi+00000730h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BB5C4
        fld dword ptr [esi+00000734h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BB5C4
        fld dword ptr [esi+00000758h]
        fmul dword ptr ds:[00858EF0h]
        fst dword ptr [esi+00000758h]
        fmul dword ptr ds:[00B7CB5Ch]
        fadd dword ptr [esi+00000750h]
        fstp dword ptr [esi+00000750h]
        jmp bike_control_006BB87A
    bike_control_006BB5C4:
        fld dword ptr [esi+00000644h]
        mov eax,dword ptr [esi+14h]
        fsin
        lea edx,[esp+28h]
        push edx
        push eax
        lea eax,[esp+000000B0h]
        push eax
        mov dword ptr [esp+3Ch],0
        fld dword ptr [esi+00000644h]
        fcos
        fxch st(1)
        fchs
        fstp dword ptr [esp+34h]
        fstp dword ptr [esp+38h]
        call dword ptr [BikeControl355Calls+140]
        mov ecx,dword ptr [eax]
        mov ebx,dword ptr [esp+58h]
        mov dword ptr [esp+48h],ecx
        mov edx,dword ptr [eax+4]
        mov ecx,ebx
        imul ecx,ecx,2Ch
        mov dword ptr [esp+4Ch],edx
        mov eax,dword ptr [eax+8]
        fld dword ptr [esp+4Ch]
        lea ebp,[ecx+esi]
        lea edi,[ebp+00000670h]
        mov dword ptr [esp+50h],eax
        add esp,0Ch
        fmul dword ptr [edi+4]
        lea ecx,[esp+3Ch]
        fld dword ptr [esp+44h]
        fmul dword ptr [edi+8]
        faddp st(1),st
        fld dword ptr [esp+3Ch]
        fmul dword ptr [edi]
        faddp st(1),st
        fld st(0)
        fmul dword ptr [edi+8]
        fld st(1)
        fmul dword ptr [edi+4]
        fxch st(2)
        fmul dword ptr [edi]
        fld dword ptr [esp+3Ch]
        fsub st,st(1)
        fstp dword ptr [esp+3Ch]
        fstp st(0)
        fld dword ptr [esp+40h]
        fsub st,st(2)
        fstp dword ptr [esp+40h]
        fstp st(1)
        fld dword ptr [esp+44h]
        fsub st,st(1)
        fstp dword ptr [esp+44h]
        fstp st(0)
        call dword ptr [BikeControl355Calls+148]
        push edi
        lea edx,[esp+40h]
        push edx
        lea eax,[esp+000000B0h]
        push eax
        call dword ptr [BikeControl355Calls+136]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+24h],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+28h],edx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        lea ecx,[esp+18h]
        mov dword ptr [esp+20h],eax
        call dword ptr [BikeControl355Calls+148]
        lea ecx,[ebp+00000660h]
        push ecx
        mov ecx,0B79538h
        mov dword ptr ds:[00C1C27Ch],0
        mov byte ptr [ebp+00000680h],3Ch
        call dword ptr [BikeControl355Calls+128]
        fmul dword ptr [esp+38h]
        fstp dword ptr [esp+14h]
        mov dword ptr [esp+4Ch],3F800000h
        fld dword ptr [esi+000007B0h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BB71B
        movzx edx,byte ptr [ebp+00000683h]
        push edx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+112]
        dec eax
        je bike_control_006BB711
        dec eax
        je bike_control_006BB9BF
        dec eax
        jne bike_control_006BB71B
    bike_control_006BB711:
        mov ecx,dword ptr ds:[008712F8h]
        mov dword ptr [esp+4Ch],ecx
    bike_control_006BB71B:
        test byte ptr [esi+36h],0F8h
        jne bike_control_006BB73B
        movzx edx,byte ptr [ebp+00000683h]
        push edx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+116]
        fmul dword ptr [esp+14h]
        fstp dword ptr [esp+14h]
    bike_control_006BB73B:
        cmp byte ptr [esi+0000065Ch],1
        jne bike_control_006BB752
        fld dword ptr [esp+14h]
        fmul dword ptr ds:[0087128Ch]
        fstp dword ptr [esp+14h]
    bike_control_006BB752:
        mov eax,dword ptr [esi+0000080Ch]
        mov edx,dword ptr [esp+00000090h]
        sub esp,0Ch
        mov ecx,esp
        mov dword ptr [ecx],edx
        mov edx,dword ptr [esp+000000A4h]
        mov dword ptr ds:[00C1C26Ch],eax
        mov eax,dword ptr [esp+000000A0h]
        mov dword ptr [ecx+4],eax
        lea eax,[esp+000000B4h]
        mov dword ptr [ecx+8],edx
        push eax
        mov ecx,esi
        call dword ptr [BikeControl355Calls+84]
        mov ecx,dword ptr [eax]
        mov dword ptr [esp+68h],ecx
        mov edx,dword ptr [eax+4]
        mov dword ptr [esp+6Ch],edx
        mov eax,dword ptr [eax+8]
        mov dword ptr [esp+70h],eax
        mov ecx,dword ptr [esi+ebx*4+000007C0h]
        test ecx,ecx
        je bike_control_006BB7F9
        lea edx,[ebx+ebx*2]
        lea eax,[esi+edx*4+000007D0h]
        mov edi,dword ptr [eax]
        sub esp,0Ch
        mov edx,esp
        mov dword ptr [edx],edi
        mov edi,dword ptr [eax+4]
        mov eax,dword ptr [eax+8]
        mov dword ptr [edx+4],edi
        mov dword ptr [edx+8],eax
        lea edx,[esp+000000B4h]
        push edx
        call dword ptr [BikeControl355Calls+84]
        fld dword ptr [esp+68h]
        fsub dword ptr [eax]
        fstp dword ptr [esp+68h]
        fld dword ptr [esp+6Ch]
        fsub dword ptr [eax+4]
        fstp dword ptr [esp+6Ch]
        fld dword ptr [esp+70h]
        fsub dword ptr [eax+8]
        fstp dword ptr [esp+70h]
    bike_control_006BB7F9:
        movzx ax,byte ptr [esi+0000065Ch]
        fld dword ptr [esp+14h]
        mov edx,dword ptr [esp+4Ch]
        fmul dword ptr [esp+34h]
        lea ecx,[esi+00000758h]
        push eax
        mov eax,dword ptr ds:[00C1C27Ch]
        push 0
        push 0C1C26Ch
        push ecx
        push 0
        push edx
        sub esp,8
        fstp dword ptr [esp+4]
        lea ecx,[esp+000000B0h]
        fld dword ptr [esp+30h]
        lea edx,[esp+00000088h]
        fmul dword ptr [esp+68h]
        fstp dword ptr [esp]
        push eax
        push 2
        push ecx
        push edx
        lea eax,[esp+48h]
        push eax
        lea ecx,[esp+70h]
        push ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+184]
        test byte ptr [esp+7Ch],4
        je bike_control_006BB87A
        mov eax,dword ptr ds:[00C1C26Ch]
        cmp eax,1
        je bike_control_006BB870
        cmp eax,2
        jne bike_control_006BB87A
    bike_control_006BB870:
        mov dword ptr ds:[00C1C26Ch],0
    bike_control_006BB87A:
        xor edx,edx
        mov dword ptr [esi+000007C0h],edx
        mov dword ptr [esi+000007C4h],edx
        mov dword ptr [esi+000007C8h],edx
        mov dword ptr [esi+000007CCh],edx
        mov eax,dword ptr [esi+00000460h]
        test eax,eax
        mov dword ptr [esp+10h],edx
        je bike_control_006BB8C4
        mov eax,dword ptr [eax+18h]
        push 0C3h
        push eax
        call dword ptr [BikeControl355Calls+56]
        add esp,8
        test eax,eax
        je bike_control_006BB8C4
        fld dword ptr [eax+18h]
        fmul dword ptr ds:[00871498h]
        fstp dword ptr [esp+10h]
    bike_control_006BB8C4:
        mov al,byte ptr [esp+27h]
        test al,al
        je bike_control_006BB955
        lea edi,[esi+00000618h]
        mov dword ptr [esp+18h],0
        mov edx,dword ptr [esp+18h]
        mov dword ptr [esp+1Ch],0
        mov eax,dword ptr [esp+1Ch]
        mov ecx,edi
        mov dword ptr [ecx],edx
        mov dword ptr [ecx+4],eax
        mov eax,dword ptr [esi+14h]
        add eax,10h
        push edi
        push eax
        mov dword ptr [esp+28h],3F800000h
        mov edx,dword ptr [esp+28h]
        lea eax,[esp+20h]
        push eax
        mov dword ptr [ecx+8],edx
        call dword ptr [BikeControl355Calls+136]
        add esp,0Ch
        lea ecx,[esp+18h]
        call dword ptr [BikeControl355Calls+148]
        mov eax,dword ptr [esi+14h]
        add eax,10h
        push eax
        lea ecx,[esp+1Ch]
        push ecx
        lea edx,[esp+000000B0h]
        push edx
        call dword ptr [BikeControl355Calls+136]
        mov edx,dword ptr [eax]
        mov ecx,edi
        mov dword ptr [ecx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ecx+4],edx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        mov dword ptr [ecx+8],eax
        call dword ptr [BikeControl355Calls+148]
    bike_control_006BB955:
        test byte ptr [esp+7Ch],2
        jne bike_control_006BB9E9
        mov al,byte ptr [esi+00000614h]
        test al,8
        jne bike_control_006BB9E9
        test al,10h
        je bike_control_006BB9CD
        fld qword ptr ds:[00866FD0h]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [BikeControl355Calls+204]
        mov eax,dword ptr [esi+14h]
        mov ecx,dword ptr [eax+8]
        fstp dword ptr [esp+48h]
        mov dword ptr [esp+60h],ecx
        fld dword ptr [esp+60h]
        call dword ptr [BikeControl355Calls+200]
        fld dword ptr [esp+48h]
        fmul dword ptr [esi+0000064Ch]
        fxch st(1)
        fadd dword ptr [esp+10h]
        fadd dword ptr ds:[00858F1Ch]
        fld dword ptr ds:[00858624h]
        fsub dword ptr [esp+48h]
        fmulp st(1),st
        fsubp st(1),st
        jmp bike_control_006BBBA1
    bike_control_006BB9BF:
        mov eax,dword ptr ds:[008712F4h]
        mov dword ptr [esp+4Ch],eax
        jmp bike_control_006BB71B
    bike_control_006BB9CD:
        fld qword ptr ds:[0086C3C8h]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [BikeControl355Calls+204]
        fmul dword ptr [esi+0000064Ch]
        jmp bike_control_006BBBA1
    bike_control_006BB9E9:
        mov eax,dword ptr [esi+14h]
        lea ecx,[esi+00000618h]
        push ecx
        add eax,10h
        push eax
        lea edx,[esp+000000B0h]
        push edx
        call dword ptr [BikeControl355Calls+136]
        mov edx,dword ptr [eax]
        lea edi,[esi+00000624h]
        mov ecx,edi
        mov dword ptr [ecx],edx
        mov edx,dword ptr [eax+4]
        mov dword ptr [ecx+4],edx
        mov eax,dword ptr [eax+8]
        add esp,0Ch
        mov dword ptr [ecx+8],eax
        call dword ptr [BikeControl355Calls+148]
        mov eax,dword ptr [esi+000000FCh]
        test eax,eax
        je bike_control_006BBA39
        fld dword ptr ds:[00858B50h]
        jmp bike_control_006BBAD6
    bike_control_006BBA39:
        mov al,byte ptr [esi+00000804h]
        test al,al
        jne bike_control_006BBA69
        mov ecx,dword ptr [esi+00000384h]
        fld dword ptr [ecx+000000A0h]
        fmul dword ptr ds:[008595ECh]
        fdivr dword ptr [esi+00000494h]
        fmul dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00871494h]
        jmp bike_control_006BBAD6
    bike_control_006BBA69:
        test byte ptr [esi+40h],4
        je bike_control_006BBAAD
        fld dword ptr [esp+58h]
        mov edx,dword ptr [esp+50h]
        fsub dword ptr [esi+00000638h]
        mov eax,dword ptr [esp+54h]
        fld dword ptr [esp+54h]
        mov ecx,dword ptr [esp+58h]
        fsub dword ptr [esi+00000634h]
        fld dword ptr [esp+50h]
        fsub dword ptr [esi+00000630h]
        mov dword ptr [esi+00000630h],edx
        mov dword ptr [esi+00000634h],eax
        mov dword ptr [esi+00000638h],ecx
        jmp bike_control_006BBAC2
    bike_control_006BBAAD:
        fld dword ptr [esi+4Ch]
        fsub dword ptr [esp+58h]
        fld dword ptr [esi+48h]
        fsub dword ptr [esp+54h]
        fld dword ptr [esi+44h]
        fsub dword ptr [esp+50h]
    bike_control_006BBAC2:
        fxch st(1)
        fmul dword ptr [edi+4]
        fxch st(2)
        fmul dword ptr [edi+8]
        faddp st(2),st
        fld st(0)
        fmul dword ptr [edi]
        faddp st(2),st
        fstp st(0)
    bike_control_006BBAD6:
        fld dword ptr ds:[00858C58h]
        fcomp dword ptr ds:[00B7CB5Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BBAF1
        fld dword ptr ds:[00858C58h]
        jmp bike_control_006BBAF7
    bike_control_006BBAF1:
        fld dword ptr ds:[00B7CB5Ch]
    bike_control_006BBAF7:
        fmul dword ptr ds:[00863984h]
        mov al,byte ptr [esi+0000065Ch]
        cmp al,1
        mov ecx,dword ptr [esi+0000063Ch]
        fdivr st,st(1)
        fstp dword ptr [esp+14h]
        fstp st(0)
        jne bike_control_006BBB31
        fld dword ptr ds:[0087128Ch]
        fmul dword ptr [ecx+14h]
        fld dword ptr [esp+14h]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne bike_control_006BBB4B
        fstp dword ptr [esp+14h]
        jmp bike_control_006BBB62
    bike_control_006BBB31:
        fld dword ptr [esp+14h]
        fcomp dword ptr [ecx+14h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BBB48
        mov edx,dword ptr [ecx+14h]
        mov dword ptr [esp+14h],edx
        jmp bike_control_006BBB62
    bike_control_006BBB48:
        fld dword ptr [ecx+14h]
    bike_control_006BBB4B:
        fchs
        fld dword ptr [esp+14h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BBB60
        fstp dword ptr [esp+14h]
        jmp bike_control_006BBB62
    bike_control_006BBB60:
        fstp st(0)
    bike_control_006BBB62:
        mov eax,dword ptr [ecx+1Ch]
        mov dword ptr [esp+60h],eax
        fld dword ptr [esp+60h]
        fld dword ptr ds:[00B7CB5Ch]
        call dword ptr [BikeControl355Calls+204]
        fstp dword ptr [esp+48h]
        fld dword ptr [esp+14h]
        call dword ptr [BikeControl355Calls+200]
        fsub dword ptr [esp+10h]
        fld dword ptr ds:[00858624h]
        fsub dword ptr [esp+48h]
        fmulp st(1),st
        fld dword ptr [esp+48h]
        fmul dword ptr [esi+0000064Ch]
        faddp st(1),st
    bike_control_006BBBA1:
        fstp dword ptr [esi+0000064Ch]
        mov ecx,dword ptr [esi+0000064Ch]
        mov dword ptr [esi+00000648h],ecx
        mov eax,0C1C26Ch
        lea ecx,[esi+0000080Ch]
        mov edx,2
    bike_control_006BBBC3:
        mov edi,dword ptr [eax]
        mov dword ptr [ecx],edi
        add eax,4
        add ecx,4
        dec edx
        jne bike_control_006BBBC3
        fld dword ptr [esi+0000049Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jp bike_control_006BBBF6
        cmp dword ptr [esi+00000810h],1
        jne bike_control_006BBBF6
        mov dword ptr [esi+00000810h],0
    bike_control_006BBBF6:
        test byte ptr [esi+36h],0F8h
        jne bike_control_006BBC07
        push 1
        mov ecx,esi
        call dword ptr [BikeControl355Calls+188]
        jmp bike_control_006BBC18
    bike_control_006BBC07:
        mov eax,dword ptr [esi+00000514h]
        test eax,eax
        jbe bike_control_006BBC18
        dec eax
        mov dword ptr [esi+00000514h],eax
    bike_control_006BBC18:
        fld dword ptr [esi+000004C0h]
        fcomp dword ptr ds:[00859F80h]
        fnstsw ax
        test ah,5
        jp bike_control_006BBD5B
        mov dl,byte ptr [esi+36h]
        and dl,0F8h
        cmp dl,28h
        je bike_control_006BBD5B
        cmp dword ptr [esi+00000594h],0Ah
        je bike_control_006BBD04
        mov eax,dword ptr [esi+18h]
        test eax,eax
        je bike_control_006BBC5A
        mov eax,dword ptr [eax+4]
        add eax,10h
        jmp bike_control_006BBC5C
    bike_control_006BBC5A:
        xor eax,eax
    bike_control_006BBC5C:
        mov ecx,dword ptr [esi+0000057Ch]
        test ecx,ecx
        jne bike_control_006BBD04
        test eax,eax
        je bike_control_006BBD04
        movsx ecx,word ptr [esi+22h]
        mov ecx,dword ptr [ecx*4+00A9B0C8h]
        mov edx,dword ptr [ecx+5Ch]
        add edx,54h
        mov ecx,dword ptr [edx]
        push 0
        mov dword ptr [esp+1Ch],ecx
        mov ecx,dword ptr [edx+4]
        push eax
        lea eax,[esp+20h]
        mov dword ptr [esp+24h],ecx
        mov edx,dword ptr [edx+8]
        push eax
        push 85AA5Ch
        mov ecx,0A9AE80h
        mov dword ptr [esp+30h],edx
        call dword ptr [BikeControl355Calls+28]
        test eax,eax
        mov dword ptr [esi+0000057Ch],eax
        je bike_control_006BBD04
        mov ecx,eax
        call dword ptr [BikeControl355Calls+32]
        push esi
        lea ecx,[esp+00000094h]
        call dword ptr [BikeControl355Calls+48]
        lea ecx,[esp+00000090h]
        push 0
        push ecx
        mov dword ptr [esp+0000019Ch],0
        call dword ptr [BikeControl355Calls+44]
        mov ecx,eax
        call dword ptr [BikeControl355Calls+40]
        lea ecx,[esp+00000090h]
        mov dword ptr [esp+00000194h],0FFFFFFFFh
        call dword ptr [BikeControl355Calls+52]
    bike_control_006BBD04:
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00858B38h]
        fmul dword ptr ds:[00858C4Ch]
        call dword ptr [BikeControl355Calls+196]
        test eax,eax
        mov dword ptr [esp+60h],eax
        fild dword ptr [esp+60h]
        jge bike_control_006BBD2D
        fadd dword ptr ds:[00858C54h]
    bike_control_006BBD2D:
        fadd dword ptr [esi+000007BCh]
        fst dword ptr [esi+000007BCh]
        fcomp dword ptr ds:[0086CD78h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BBD78
        mov eax,dword ptr [esi+00000800h]
        mov edx,dword ptr [esi]
        push 0
        push eax
        mov ecx,esi
        call dword ptr [edx+000000A4h]
        jmp bike_control_006BBD78
    bike_control_006BBD5B:
        mov ecx,dword ptr [esi+0000057Ch]
        xor edi,edi
        cmp ecx,edi
        mov dword ptr [esi+000007BCh],edi
        je bike_control_006BBD78
        call dword ptr [BikeControl355Calls+36]
        mov dword ptr [esi+0000057Ch],edi
    bike_control_006BBD78:
        mov ecx,esi
        call dword ptr [BikeControl355Calls+168]
        fld dword ptr [esi+4Ch]
        fld dword ptr [esi+48h]
        lea edi,[esi+44h]
        fld dword ptr [edi]
        lea ecx,[esi+00000683h]
        fld st(0)
        mov dword ptr [esp+14h],0
        fmul st,st(1)
        mov dword ptr [esp+38h],0
        fld st(2)
        xor ebp,ebp
        fmul st,st(3)
        lea ebx,[esi+00000710h]
        mov dword ptr [esp+4Ch],ecx
        faddp st(1),st
        fld st(3)
        fmul st,st(4)
        faddp st(1),st
        fstp dword ptr [esp+60h]
        fstp st(0)
        fstp st(0)
        fstp st(0)
    bike_control_006BBDC6:
        fld dword ptr [ebx+10h]
        fsub dword ptr [ebx]
        fst dword ptr [esp+10h]
        fcomp dword ptr ds:[00858C24h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BBE1B
        test ebp,ebp
        je bike_control_006BBDE5
        cmp ebp,2
        jne bike_control_006BBE1B
    bike_control_006BBDE5:
        fld dword ptr [esp+60h]
        fcomp dword ptr ds:[00863244h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BBE1B
        mov al,byte ptr [esi+36h]
        shr al,3
        test al,al
        je bike_control_006BBE04
        cmp al,3
        jne bike_control_006BBE1B
    bike_control_006BBE04:
        fld dword ptr [esp+10h]
        fcomp dword ptr [esp+14h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BBE1B
        mov edx,dword ptr [esp+10h]
        mov dword ptr [esp+14h],edx
    bike_control_006BBE1B:
        fld dword ptr [ebx]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,5
        jp bike_control_006BBE65
        test byte ptr [esi+36h],0F8h
        jne bike_control_006BBE65
        mov eax,dword ptr [esp+4Ch]
        movzx ecx,byte ptr [eax]
        push ecx
        mov ecx,0B79538h
        call dword ptr [BikeControl355Calls+124]
        mov dword ptr [esp+64h],eax
        fild dword ptr [esp+64h]
        fmul dword ptr ds:[00858B1Ch]
        fld dword ptr [esp+38h]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        je bike_control_006BBE63
        fstp dword ptr [esp+38h]
        jmp bike_control_006BBE65
    bike_control_006BBE63:
        fstp st(0)
    bike_control_006BBE65:
        mov edx,dword ptr [ebx]
        mov eax,dword ptr ds:[00871284h]
        mov ecx,dword ptr [esp+4Ch]
        mov dword ptr [ebx+10h],edx
        mov dword ptr [ebx],eax
        inc ebp
        add ecx,2Ch
        add ebx,4
        cmp ebp,4
        mov dword ptr [esp+4Ch],ecx
        jl bike_control_006BBDC6
        mov ecx,dword ptr ds:[00B7CB84h]
        and ecx,7FFh
        cmp ecx,320h
        jbe bike_control_006BBEBE
        fld dword ptr [esp+38h]
        fcomp dword ptr ds:[00871268h]
        fnstsw ax
        test ah,1
        jne bike_control_006BBEB6
        mov dword ptr [esp+14h],0
    bike_control_006BBEB6:
        mov dword ptr [esp+38h],0
    bike_control_006BBEBE:
        fld dword ptr [esp+14h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BBEE4
        fld dword ptr [esp+38h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC006
    bike_control_006BBEE4:
        test byte ptr [esi+36h],0F8h
        jne bike_control_006BC006
        fld dword ptr [edi+8]
        fld dword ptr [edi+4]
        fld dword ptr [edi]
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
        fcom dword ptr ds:[0086CD7Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC004
        fsqrt
        fld dword ptr [esp+14h]
        fcomp dword ptr ds:[00858B50h]
        fdiv dword ptr [esi+0000008Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BBF9D
        fmul dword ptr [esp+14h]
        fmul dword ptr ds:[00871264h]
        fadd dword ptr ds:[00858628h]
        fld dword ptr ds:[00859F80h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BBF5D
        fstp st(0)
        fld dword ptr ds:[00859F80h]
    bike_control_006BBF5D:
        call dword ptr [BikeControl355Calls+196]
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00871260h]
        movzx edx,al
        mov dword ptr [esp+60h],edx
        mov byte ptr [esp+64h],al
        fidiv dword ptr [esp+60h]
        call dword ptr [BikeControl355Calls+196]
        mov ecx,dword ptr [esp+64h]
        push 0
        push ecx
        push eax
        push 0
        call dword ptr [BikeControl355Calls+72]
        add esp,4
        mov ecx,eax
        call dword ptr [BikeControl355Calls+68]
        jmp bike_control_006BC006
    bike_control_006BBF9D:
        fmul dword ptr [esp+38h]
        fmul dword ptr ds:[00871264h]
        fadd dword ptr ds:[00858A10h]
        fld dword ptr ds:[00858A28h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BBFC4
        fstp st(0)
        fld dword ptr ds:[00858A28h]
    bike_control_006BBFC4:
        call dword ptr [BikeControl355Calls+196]
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[0086CD78h]
        movzx edx,al
        mov dword ptr [esp+60h],edx
        mov byte ptr [esp+64h],al
        fidiv dword ptr [esp+60h]
        call dword ptr [BikeControl355Calls+196]
        mov ecx,dword ptr [esp+64h]
        push 0
        push ecx
        push eax
        push 0
        call dword ptr [BikeControl355Calls+72]
        add esp,4
        mov ecx,eax
        call dword ptr [BikeControl355Calls+68]
        jmp bike_control_006BC006
    bike_control_006BC004:
        fstp st(0)
    bike_control_006BC006:
        mov cl,byte ptr [esi+0000042Bh]
        mov al,byte ptr [esi+00000429h]
        and cl,0DEh
        test al,20h
        mov byte ptr [esi+0000042Bh],cl
        jne bike_control_006BC028
        push esi
        call dword ptr [BikeControl355Calls+24]
        add esp,4
    bike_control_006BC028:
        mov eax,dword ptr [esi+40h]
        test al,4
        je bike_control_006BC0CF
        test al,8
        je bike_control_006BC0CF
        mov dword ptr [esp+18h],0
        mov eax,dword ptr [esp+18h]
        mov dword ptr [esp+1Ch],0
        mov ecx,dword ptr [esp+1Ch]
        mov edx,edi
        mov dword ptr [edx],eax
        mov dword ptr [edx+4],ecx
        mov dword ptr [esp+20h],0
        mov eax,dword ptr [esp+20h]
        mov dword ptr [edx+8],eax
        mov dword ptr [esp+18h],0
        mov edx,dword ptr [esp+18h]
        lea ecx,[esi+50h]
        mov dword ptr [ecx],edx
        mov dword ptr [esp+1Ch],0
        mov eax,dword ptr [esp+1Ch]
        mov dword ptr [ecx+4],eax
        mov dword ptr [esp+20h],0
        mov edx,dword ptr [esp+20h]
        mov dword ptr [ecx+8],edx
        mov dword ptr [esp+18h],0
        mov ecx,dword ptr [esp+18h]
        lea eax,[esi+5Ch]
        mov dword ptr [eax],ecx
        mov dword ptr [esp+1Ch],0
        mov edx,dword ptr [esp+1Ch]
        mov dword ptr [esp+20h],0
        mov ecx,dword ptr [esp+20h]
        mov dword ptr [eax+4],edx
        mov dword ptr [eax+8],ecx
        lea edx,[esi+68h]
        jmp bike_control_006BC166
    bike_control_006BC0CF:
        mov al,byte ptr [esp+5Fh]
        test al,al
        jne bike_control_006BC192
        fld dword ptr [esi+0000049Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,44h
        jnp bike_control_006BC0FD
        mov cl,byte ptr [esi+36h]
        and cl,0F8h
        cmp cl,28h
        jne bike_control_006BC192
    bike_control_006BC0FD:
        fld dword ptr [edi]
        fabs
        fcomp dword ptr ds:[00858B4Ch]
        fnstsw ax
        test ah,5
        jp bike_control_006BC192
        fld dword ptr [esi+48h]
        fabs
        fcomp dword ptr ds:[00858B4Ch]
        fnstsw ax
        test ah,5
        jp bike_control_006BC192
        fld dword ptr [esi+4Ch]
        fabs
        fcomp dword ptr ds:[00858B4Ch]
        fnstsw ax
        test ah,5
        jp bike_control_006BC192
        fld dword ptr [esi+000000D8h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC15D
        mov ebx,dword ptr [esi+000000DCh]
        push 0FFFFFFFFh
        call dword ptr [BikeControl355Calls+132]
        add esp,4
        cmp ebx,eax
        je bike_control_006BC192
    bike_control_006BC15D:
        mov edx,edi
        mov dword ptr [esi+58h],0
    bike_control_006BC166:
        mov dword ptr [esp+18h],0
        mov eax,dword ptr [esp+18h]
        mov dword ptr [edx],eax
        mov dword ptr [esp+1Ch],0
        mov ecx,dword ptr [esp+1Ch]
        mov dword ptr [esp+20h],0
        mov eax,dword ptr [esp+20h]
        mov dword ptr [edx+4],ecx
        mov dword ptr [edx+8],eax
    bike_control_006BC192:
        test byte ptr [esp+7Ch],2
        jne bike_control_006BC1A6
        test byte ptr [esi+00000614h],18h
        je bike_control_006BC914
    bike_control_006BC1A6:
        mov ecx,dword ptr [esi+14h]
        fld dword ptr [esi+00000620h]
        fmul dword ptr [ecx+8]
        fld dword ptr [esi+0000061Ch]
        fmul dword ptr [ecx+4]
        faddp st(1),st
        fld dword ptr [ecx]
        fmul dword ptr [esi+00000618h]
        faddp st(1),st
        fst dword ptr [esp+38h]
        fcomp dword ptr ds:[00858624h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC1E2
        mov dword ptr [esp+38h],3F800000h
        jmp bike_control_006BC1FB
    bike_control_006BC1E2:
        fld dword ptr [esp+38h]
        fcomp dword ptr ds:[00858C1Ch]
        fnstsw ax
        test ah,5
        jp bike_control_006BC1FB
        mov dword ptr [esp+38h],0BF800000h
    bike_control_006BC1FB:
        lea eax,[esi+000000A4h]
        push eax
        push ecx
        lea ecx,[esp+58h]
        push ecx
        call dword ptr [BikeControl355Calls+140]
        fld dword ptr [esp+5Ch]
        mov al,byte ptr [esp+00000088h]
        add esp,0Ch
        test al,2
        mov eax,dword ptr [esi+14h]
        fadd dword ptr [eax]
        fstp dword ptr [esp+18h]
        fld dword ptr [esp+54h]
        fadd dword ptr [eax+4]
        fstp dword ptr [esp+1Ch]
        fld dword ptr [esp+58h]
        fadd dword ptr [eax+8]
        fstp dword ptr [esp+20h]
        fld dword ptr [esp+38h]
        fmul dword ptr [esi+00000090h]
        je bike_control_006BC2D1
        fmul dword ptr ds:[008712E8h]
        mov ecx,dword ptr [esp+1Ch]
        sub esp,0Ch
        mov edx,esp
        fld st(0)
        sub esp,0Ch
        fmul dword ptr [eax+20h]
        fld st(1)
        fmul dword ptr [eax+24h]
        fstp dword ptr [esp+58h]
        fxch st(1)
        fmul dword ptr [eax+28h]
        mov eax,dword ptr [esp+30h]
        mov dword ptr [edx],eax
        mov eax,dword ptr [esp+38h]
        fstp dword ptr [esp+5Ch]
        mov dword ptr [edx+4],ecx
        fmul dword ptr ds:[00B7CB5Ch]
        mov dword ptr [edx+8],eax
        mov ecx,esp
        fstp dword ptr [esp+40h]
        mov edx,dword ptr [esp+40h]
        fld dword ptr [esp+58h]
        mov dword ptr [ecx],edx
        fmul dword ptr ds:[00B7CB5Ch]
        fstp dword ptr [esp+44h]
        fld dword ptr [esp+5Ch]
        mov eax,dword ptr [esp+44h]
        fmul dword ptr ds:[00B7CB5Ch]
        mov dword ptr [ecx+4],eax
        fstp dword ptr [esp+48h]
        mov edx,dword ptr [esp+48h]
        mov dword ptr [ecx+8],edx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+80]
        and byte ptr [esi+00000614h],0EFh
        jmp bike_control_006BC34D
    bike_control_006BC2D1:
        fmul dword ptr ds:[008712ECh]
        mov ecx,dword ptr [esp+18h]
        mov edx,dword ptr [esp+1Ch]
        sub esp,0Ch
        fld st(0)
        fmul dword ptr [eax+20h]
        fld st(1)
        fmul dword ptr [eax+24h]
        fstp dword ptr [esp+4Ch]
        fxch st(1)
        fmul dword ptr [eax+28h]
        mov eax,esp
        mov dword ptr [eax],ecx
        mov ecx,dword ptr [esp+2Ch]
        fstp dword ptr [esp+50h]
        mov dword ptr [eax+4],edx
        fmul dword ptr ds:[00B7CB5Ch]
        mov dword ptr [eax+8],ecx
        sub esp,0Ch
        mov edx,esp
        fstp dword ptr [esp+40h]
        mov eax,dword ptr [esp+40h]
        fld dword ptr [esp+58h]
        mov dword ptr [edx],eax
        fmul dword ptr ds:[00B7CB5Ch]
        fstp dword ptr [esp+44h]
        fld dword ptr [esp+5Ch]
        mov ecx,dword ptr [esp+44h]
        fmul dword ptr ds:[00B7CB5Ch]
        mov dword ptr [edx+4],ecx
        mov ecx,esi
        fstp dword ptr [esp+48h]
        mov eax,dword ptr [esp+48h]
        mov dword ptr [edx+8],eax
        call dword ptr [BikeControl355Calls+80]
    bike_control_006BC34D:
        test byte ptr [esi+36h],0F8h
        jne bike_control_006BC914
        fld dword ptr [esi+00000730h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jp bike_control_006BC68C
        fld dword ptr [esi+00000734h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jp bike_control_006BC68C
        mov edx,dword ptr [esi+14h]
        fld dword ptr [edx+18h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC68C
        fld dword ptr [esi+00000738h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BC3C6
        fld dword ptr [esi+0000073Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC68C
    bike_control_006BC3C6:
        mov ecx,dword ptr [esi+0000063Ch]
        fld dword ptr [ecx+2Ch]
        fsub dword ptr [edx+18h]
        fcom dword ptr ds:[00858FCCh]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC3F6
        fsubr dword ptr ds:[00858C24h]
        fld dword ptr ds:[00858B50h]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne bike_control_006BC420
        jmp bike_control_006BC418
    bike_control_006BC3F6:
        fcom dword ptr ds:[00871490h]
        fnstsw ax
        test ah,5
        jp bike_control_006BC420
        fsubr dword ptr ds:[0087148Ch]
        fld dword ptr ds:[00858B50h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BC420
    bike_control_006BC418:
        fstp st(0)
        fld dword ptr ds:[00858B50h]
    bike_control_006BC420:
        fld dword ptr [edi+8]
        fld dword ptr [edi+4]
        fld dword ptr [edi]
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
        fcomp dword ptr ds:[00858B1Ch]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC455
        fld dword ptr ds:[00858B1Ch]
        jmp bike_control_006BC475
    bike_control_006BC455:
        fld dword ptr [edi+8]
        fld dword ptr [edi+4]
        fld dword ptr [edi]
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
    bike_control_006BC475:
        fxch st(1)
        push 0Ch
        fmul dword ptr [ecx+38h]
        fmul st,st(1)
        fstp dword ptr [esp+14h]
        fstp st(0)
        call dword ptr [BikeControl355Calls+108]
        fmul dword ptr [esp+14h]
        mov eax,dword ptr [esi+14h]
        fld dword ptr [esp+54h]
        sub esp,8
        fadd dword ptr [eax+10h]
        mov ecx,esp
        sub esp,0Ch
        fstp dword ptr [esp+30h]
        mov edx,dword ptr [esp+30h]
        fld dword ptr [esp+6Ch]
        mov dword ptr [ecx],edx
        fadd dword ptr [eax+14h]
        fstp dword ptr [esp+34h]
        fld dword ptr [esp+70h]
        fadd dword ptr [eax+18h]
        fstp dword ptr [esp+38h]
        mov edx,dword ptr [esp+38h]
        fmul dword ptr [esi+00000090h]
        fmul dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00858B8Ch]
        fld st(0)
        fmul dword ptr [eax+20h]
        fstp dword ptr [esp+40h]
        fld st(0)
        fmul dword ptr [eax+24h]
        fstp dword ptr [esp+44h]
        fmul dword ptr [eax+28h]
        mov eax,dword ptr [esp+34h]
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+8],edx
        mov ecx,dword ptr [esp+40h]
        mov edx,dword ptr [esp+44h]
        mov eax,esp
        mov dword ptr [eax],ecx
        fstp dword ptr [esp+48h]
        mov ecx,dword ptr [esp+48h]
        mov dword ptr [eax+4],edx
        mov dword ptr [eax+8],ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+80]
        fld dword ptr [esp+50h]
        mov eax,dword ptr [esi+14h]
        fadd dword ptr [eax+10h]
        mov edx,dword ptr [esi+0000063Ch]
        sub esp,0Ch
        fstp dword ptr [esp+24h]
        fld dword ptr [esp+60h]
        fadd dword ptr [eax+14h]
        fstp dword ptr [esp+28h]
        fld dword ptr [esp+64h]
        fadd dword ptr [eax+18h]
        fstp dword ptr [esp+2Ch]
        fld dword ptr [edx+34h]
        fmul dword ptr [esi+00000644h]
        fmul dword ptr [esi+00000090h]
        fmul dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00858B8Ch]
        fld st(0)
        fmul dword ptr [eax]
        fstp dword ptr [esp+34h]
        fld st(0)
        fmul dword ptr [eax+4]
        fstp dword ptr [esp+38h]
        fmul dword ptr [eax+8]
        mov eax,esp
        fstp dword ptr [esp+3Ch]
        mov ecx,dword ptr [esp+24h]
        mov edx,dword ptr [esp+28h]
        mov dword ptr [eax],ecx
        mov ecx,dword ptr [esp+2Ch]
        mov dword ptr [eax+4],edx
        mov dword ptr [eax+8],ecx
        mov eax,dword ptr [esp+34h]
        mov ecx,dword ptr [esp+38h]
        sub esp,0Ch
        mov edx,esp
        mov dword ptr [edx],eax
        mov eax,dword ptr [esp+48h]
        mov dword ptr [edx+4],ecx
        mov ecx,esi
        mov dword ptr [edx+8],eax
        call dword ptr [BikeControl355Calls+80]
        fld dword ptr [edi+8]
        fld dword ptr [edi+4]
        mov ecx,dword ptr [esi+0000063Ch]
        fld dword ptr [edi]
        mov eax,dword ptr [esi+14h]
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
        fmul dword ptr [esi+0000008Ch]
        fmul dword ptr [ecx+34h]
        fmul dword ptr [esi+00000644h]
        fmul dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[008D3288h]
        fld st(0)
        fmul dword ptr [eax]
        fld st(1)
        fmul dword ptr [eax+4]
        fstp dword ptr [esp+2Ch]
        fxch st(1)
        fmul dword ptr [eax+8]
        fstp dword ptr [esp+30h]
        fld dword ptr [edi+8]
        fld dword ptr [edi+4]
        fld dword ptr [edi]
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
        fstp dword ptr [esp+10h]
        fmul dword ptr [esp+10h]
        fstp dword ptr [esp+18h]
        fld dword ptr [esp+2Ch]
        fmul dword ptr [esp+10h]
        fstp dword ptr [esp+1Ch]
        fld dword ptr [esp+30h]
        fmul dword ptr [esp+10h]
        fstp dword ptr [esp+20h]
        mov eax,dword ptr [esp+18h]
        mov ecx,dword ptr [esp+1Ch]
        sub esp,0Ch
        mov edx,esp
        mov dword ptr [edx],eax
        mov eax,dword ptr [esp+2Ch]
        mov dword ptr [edx+4],ecx
        mov ecx,esi
        mov dword ptr [edx+8],eax
        call dword ptr [BikeControl355Calls+76]
        fld dword ptr ds:[00B7CB5Ch]
        fmul dword ptr [esi+00000644h]
        fmul dword ptr ds:[008D3284h]
        fadd dword ptr [esi+00000648h]
        fstp dword ptr [esi+00000648h]
        jmp bike_control_006BC914
    bike_control_006BC68C:
        fld dword ptr [esi+00000738h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jp bike_control_006BC914
        fld dword ptr [esi+0000073Ch]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jp bike_control_006BC914
        mov edx,dword ptr [esi+14h]
        fld dword ptr [edx+18h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,5
        jp bike_control_006BC914
        fld dword ptr [esi+00000730h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        je bike_control_006BC6FB
        fld dword ptr [esi+00000734h]
        fcomp dword ptr ds:[00858B50h]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC914
    bike_control_006BC6FB:
        mov ecx,dword ptr [esi+0000063Ch]
        fld dword ptr [ecx+30h]
        fsub dword ptr [edx+18h]
        fcom dword ptr ds:[00858FCCh]
        fnstsw ax
        test ah,41h
        jne bike_control_006BC72B
        fsubr dword ptr ds:[00858C24h]
        fld dword ptr ds:[00858B50h]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne bike_control_006BC755
        jmp bike_control_006BC74D
    bike_control_006BC72B:
        fcom dword ptr ds:[0086C330h]
        fnstsw ax
        test ah,5
        jp bike_control_006BC755
        fsubr dword ptr ds:[00858EE4h]
        fld dword ptr ds:[00858B50h]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BC755
    bike_control_006BC74D:
        fstp st(0)
        fld dword ptr ds:[00858B50h]
    bike_control_006BC755:
        fld dword ptr [edi+8]
        fld dword ptr [edi+4]
        fld dword ptr [edi]
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
        fld dword ptr ds:[00858B1Ch]
        fcomp st(1)
        fnstsw ax
        test ah,5
        jp bike_control_006BC78C
        fstp st(0)
        fld dword ptr ds:[00858B1Ch]
    bike_control_006BC78C:
        fxch st(1)
        push 0Ch
        fmul dword ptr [ecx+3Ch]
        fmul st,st(1)
        fstp dword ptr [esp+14h]
        fstp st(0)
        call dword ptr [BikeControl355Calls+108]
        fmul dword ptr [esp+14h]
        mov eax,dword ptr [esi+14h]
        fld dword ptr [esp+54h]
        sub esp,8
        fadd dword ptr [eax+10h]
        mov ecx,esp
        sub esp,0Ch
        fstp dword ptr [esp+30h]
        mov edx,dword ptr [esp+30h]
        fld dword ptr [esp+6Ch]
        mov dword ptr [ecx],edx
        fadd dword ptr [eax+14h]
        fstp dword ptr [esp+34h]
        fld dword ptr [esp+70h]
        fadd dword ptr [eax+18h]
        fstp dword ptr [esp+38h]
        mov edx,dword ptr [esp+38h]
        fmul dword ptr [esi+00000090h]
        fmul dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[00858B8Ch]
        fld st(0)
        fmul dword ptr [eax+20h]
        fstp dword ptr [esp+40h]
        fld st(0)
        fmul dword ptr [eax+24h]
        fstp dword ptr [esp+44h]
        fmul dword ptr [eax+28h]
        mov eax,dword ptr [esp+34h]
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+8],edx
        mov ecx,dword ptr [esp+40h]
        mov edx,dword ptr [esp+44h]
        mov eax,esp
        mov dword ptr [eax],ecx
        fstp dword ptr [esp+48h]
        mov ecx,dword ptr [esp+48h]
        mov dword ptr [eax+4],edx
        mov dword ptr [eax+8],ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+80]
        fld dword ptr [edi+8]
        mov eax,dword ptr [esi+14h]
        fmul dword ptr [eax+8]
        push eax
        fld dword ptr [edi+4]
        lea edx,[esp+1Ch]
        fmul dword ptr [eax+4]
        push edx
        mov dword ptr [esp+20h],0
        mov dword ptr [esp+24h],0
        faddp st(1),st
        mov dword ptr [esp+28h],3F800000h
        fld dword ptr [eax]
        lea eax,[esp+000000BCh]
        fmul dword ptr [edi]
        push eax
        faddp st(1),st
        fmul dword ptr [esi+00000090h]
        fmul dword ptr ds:[00B7CB5Ch]
        fmul dword ptr ds:[008D3280h]
        fstp dword ptr [esp+6Ch]
        call dword ptr [BikeControl355Calls+136]
        add esp,0Ch
        lea ecx,[esp+000000B4h]
        call dword ptr [BikeControl355Calls+148]
        fld dword ptr [esp+000000B4h]
        fchs
        fstp dword ptr [esp+18h]
        fld dword ptr [esp+000000B8h]
        mov eax,dword ptr [esi+14h]
        fchs
        mov edx,dword ptr [esp+18h]
        fstp dword ptr [esp+1Ch]
        fld dword ptr [esp+000000BCh]
        sub esp,0Ch
        fchs
        mov ecx,esp
        fstp dword ptr [esp+2Ch]
        fld dword ptr [esp+6Ch]
        mov dword ptr [ecx],edx
        fchs
        mov edx,dword ptr [esp+2Ch]
        fld st(0)
        sub esp,0Ch
        fmul dword ptr [eax]
        fstp dword ptr [esp+40h]
        fld st(0)
        fmul dword ptr [eax+4]
        fstp dword ptr [esp+44h]
        fmul dword ptr [eax+8]
        mov eax,dword ptr [esp+34h]
        mov dword ptr [ecx+4],eax
        mov dword ptr [ecx+8],edx
        mov ecx,dword ptr [esp+40h]
        mov edx,dword ptr [esp+44h]
        mov eax,esp
        mov dword ptr [eax],ecx
        fstp dword ptr [esp+48h]
        mov ecx,dword ptr [esp+48h]
        mov dword ptr [eax+4],edx
        mov dword ptr [eax+8],ecx
        mov ecx,esi
        call dword ptr [BikeControl355Calls+80]
    bike_control_006BC914:
        mov ecx,dword ptr [esp+0000018Ch]
        pop edi
        pop esi
        pop ebp
        mov dword ptr fs:[00000000h],ecx
        pop ebx
        add esp,188h
        ret
    }
}
#pragma warning(pop)


// 0x6B6740
void CBike::ResetSuspension() {
    std::ranges::fill(m_WheelStates, WHEEL_STATE_NORMAL);
    std::ranges::fill(m_aWheelPitchAngles, 0.f);
    std::ranges::fill(m_aWheelRatios, 1.f);
    std::ranges::fill(m_WheelCounts, 0.f);
}

// 0x6B6790
bool CBike::GetAllWheelsOffGround() const {
    // Como el original (0x6B6790): lee el byte +0x805 (m_NumDriveWheelsOnGround), no +0x804 (m_nNoOfContactWheels): mov al,[ecx+805h] / test al,al / sete al.
    return m_NumDriveWheelsOnGround == 0;
}

// 0x6B67A0
void CBike::DebugCode() {
    // NOP
}

// 0x6B6D40
void CBike::DoSoftGroundResistance(uint32& arg0) {
    bool sand = false;
    for (size_t i = 0; i < 4; ++i) {
        if (m_aWheelRatios[i] < 1.f && int32(g_surfaceInfos.GetAdhesionGroup(m_aWheelColPoints[i].m_nSurfaceTypeB)) == 4) {
            sand = true;
            break;
        }
    }
    if (sand) {
        const auto& up = GetUp();
        auto vertical = (m_vecMoveSpeed.x * up.x + m_vecMoveSpeed.y * up.y) + m_vecMoveSpeed.z * up.z;
        CVector speed{m_vecMoveSpeed.x - vertical * up.x, m_vecMoveSpeed.y - vertical * up.y, m_vecMoveSpeed.z - vertical * up.z};
        if (m_GasPedal > .3f) {
            if ((speed.x * speed.x + speed.y * speed.y) + speed.z * speed.z < .09f) {
                arg0 += 4;
            }
            const auto& forward = GetForward();
            const auto longitudinal = (speed.x * forward.x + speed.y * forward.y) + speed.z * forward.z;
            speed = CVector{speed.x - longitudinal * forward.x, speed.y - longitudinal * forward.y, speed.z - longitudinal * forward.z};
        }
        const auto factor = -(CTimer::GetTimeStep() * m_fMass * .02f);
        ApplyMoveForce(CVector{factor * speed.x, speed.y * factor, speed.z * factor});
        return;
    }
    for (size_t i = 0; i < 4; ++i) {
        if (m_aWheelRatios[i] < 1.f && m_aWheelColPoints[i].m_nSurfaceTypeB == 178) {
            const auto& up = GetUp();
            const auto vertical = (m_vecMoveSpeed.x * up.x + m_vecMoveSpeed.y * up.y) + m_vecMoveSpeed.z * up.z;
            const auto factor = -(CTimer::GetTimeStep() * m_fMass * StaticRef<float>(0x8D34AC));
            ApplyMoveForce(CVector{(m_vecMoveSpeed.x - vertical * up.x) * factor, (m_vecMoveSpeed.y - vertical * up.y) * factor, (m_vecMoveSpeed.z - vertical * up.z) * factor});
            return;
        }
    }
}

// 0x6B7130
void CBike::PlayHornIfNecessary() {
    if (m_autoPilot.carCtrlFlags.bHonkAtCar || m_autoPilot.carCtrlFlags.bHonkAtPed)
        PlayCarHorn();
}

// 0x6B7150
void CBike::CalculateLeanMatrix() {
    if (m_bLeanMatrixCalculated)
        return;

    CMatrix mat;
    mat.SetRotateX(fabs(m_RideAnimData.LeanAngle) * -0.05f);
    mat.RotateY(m_RideAnimData.LeanAngle);
    m_mLeanMatrix = GetMatrix();
    m_mLeanMatrix = m_mLeanMatrix * mat;
    // place wheel back on ground
    // Como el original (0x6B71E1..0x6B7253): (minZ * eje) se redondea primero a float, despues se multiplica por (1 - fcos(angulo)) (fcos x87 de precision completa,
    // fsubr redondeado a 24 bits) y se suma a la posicion de la matriz de inclinacion en el orden x, y, z. La DLL multiplicaba al reves ((eje * (1-cos)) * minZ).
    const float leanAngle = m_RideAnimData.LeanAngle;
    const float minZ = GetColModel()->GetBoundingBox().m_vecMin.z;
    const auto& axis = GetMatrix().GetUp();
    const float partZ = minZ * axis.z;
    const float partY = minZ * axis.y;
    const float partX = minZ * axis.x;
    float oneMinusCos;
    __asm {
        fld leanAngle
        fcos
        fld1
        fsubrp st(1), st(0)
        fstp oneMinusCos
    }
    m_mLeanMatrix.GetPosition().x += partX * oneMinusCos;
    m_mLeanMatrix.GetPosition().y += partY * oneMinusCos;
    m_mLeanMatrix.GetPosition().z += partZ * oneMinusCos;
    m_bLeanMatrixCalculated = true;
}

static void BikeMoveHandBone355(RwMatrix* bone, const CVector& delta, float factor) {
    float deltaX, deltaY;
    const float* values = &delta.x;
    __asm {
        mov eax, values
        mov edx, bone
        fld dword ptr [eax]
        fmul factor
        fstp deltaX
        fld dword ptr [eax+4]
        fmul factor
        fstp deltaY
        fld deltaX
        fadd dword ptr [edx+30h]
        fstp dword ptr [edx+30h]
        fld deltaY
        fadd dword ptr [edx+34h]
        fstp dword ptr [edx+34h]
        fld dword ptr [eax+8]
        fmul factor
        fadd dword ptr [edx+38h]
        fstp dword ptr [edx+38h]
    }
}

// 0x6B7F90
void CBike::FixHandsToBars(CPed* rider) {
    if (!m_nFixRightHand && !m_nFixLeftHand) return;
    CMatrix handleWorld, handleLocal;
    if (!m_aBikeNodes[BIKE_CHASSIS]) return;
    handleLocal.Attach(&m_aBikeNodes[BIKE_CHASSIS]->modelling, false);
    handleWorld = *m_matrix;
    handleWorld *= handleLocal;
    auto* hierarchy = GetAnimHierarchyFromSkinClump(rider->GetRpClump());
    CVector hand = CModelInfo::GetModelInfo(m_nModelIndex)->AsVehicleModelInfoPtr()->GetModelDummyPosition(DUMMY_HAND_REST);
    if (hand.x == 0.f && hand.y == 0.f && hand.z == 0.f) {
        const auto address = m_nModelIndex == 510 ? 0x8D3250 : m_nModelIndex == 509 ? 0x8D325C : 0x8D3244;
        hand = *reinterpret_cast<const CVector*>(address);
    } else {
        const auto boneIndex = RpHAnimIDGetIndex(hierarchy, 0x18);
        auto* matrices = RpHAnimHierarchyGetMatrixArray(hierarchy);
        CMatrix bone{matrices + boneIndex, false};
        const CVector origin = *reinterpret_cast<const CVector*>(0x8D3274);
        const CVector transformed = bone.TransformVector(origin);
        hand += handleWorld.InverseTransformVector(transformed);
    }
    auto matrixFor = [&](int32 id) {
        const auto index = RpHAnimIDGetIndex(hierarchy, id);
        return RpHAnimHierarchyGetMatrixArray(hierarchy) + index;
    };
    if (m_nFixRightHand) {
        const auto target = handleWorld.TransformPoint(hand);
        auto* bone = matrixFor(0x18);
        const CVector delta{target.x - bone->pos.x, target.y - bone->pos.y, target.z - bone->pos.z};
        BikeMoveHandBone355(bone, delta, 1.f);
        BikeMoveHandBone355(matrixFor(0x19), delta, 1.f);
        BikeMoveHandBone355(matrixFor(0x1A), delta, 1.f);
        BikeMoveHandBone355(matrixFor(0x17), delta, StaticRef<float>(0x871488));
        BikeMoveHandBone355(matrixFor(0x16), delta, StaticRef<float>(0x864E30));
        if (rider->IsPlayer()) BikeMoveHandBone355(matrixFor(0x12D), delta, StaticRef<float>(0x864E30));
    }
    if (m_nFixLeftHand) {
        hand.x = -hand.x;
        const auto target = handleWorld.TransformPoint(hand);
        auto* bone = matrixFor(0x22);
        const CVector delta{target.x - bone->pos.x, target.y - bone->pos.y, target.z - bone->pos.z};
        BikeMoveHandBone355(bone, delta, 1.f);
        BikeMoveHandBone355(matrixFor(0x23), delta, 1.f);
        BikeMoveHandBone355(matrixFor(0x24), delta, 1.f);
        BikeMoveHandBone355(matrixFor(0x21), delta, StaticRef<float>(0x858F34));
        BikeMoveHandBone355(matrixFor(0x20), delta, StaticRef<float>(0x858EE8));
        if (rider->IsPlayer()) BikeMoveHandBone355(matrixFor(0x12E), delta, StaticRef<float>(0x858EE8));
    }
    m_nFixLeftHand = m_nFixRightHand = 0;
}

static void BikeRoadOrientation355(CMatrix* matrix, float rear, float front, float backX, float by, float bz, float fx, float fy, float fz) {
    __asm {
        mov eax, matrix
        fld rear
        fadd front
        fld fz
        fsub bz
        fdiv st, st(1)
        fld1
        fpatan
        fld st(0)
        fcos
        fld fy
        fsub by
        fdiv st, st(3)
        fstp dword ptr [eax]
        fld fx
        fsub backX
        fdiv st, st(3)
        fchs
        fstp dword ptr [eax+4]
        mov dword ptr [eax+8], 0
        fld st(0)
        fmul dword ptr [eax+4]
        fchs
        fstp dword ptr [eax+10h]
        fmul dword ptr [eax]
        fstp dword ptr [eax+14h]
        fsin
        fstp st(1)
        fstp dword ptr [eax+18h]
    }
}

// 0x6BEEB0
void CBike::PlaceOnRoadProperly() {
    const float front = GetColModel()->m_boundBox.m_vecMax.y;
    const float rear = -GetColModel()->m_boundBox.m_vecMin.y;
    const CVector direction = GetForward();
    const float backX = GetPosition().x - direction.x * rear;
    const float by = GetPosition().y - direction.y * rear;
    const float fx = direction.x * front + GetPosition().x;
    const float fy = direction.y * front + GetPosition().y;
    CColPoint point;
    CEntity* entity;
    auto recordCollision = [&](size_t offset) {
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + offset) = point.m_nLightingB.value;
        m_pEntityWeAreOn = entity;
        auto& flags = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0x1C);
        const auto otherFlags = *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(entity) + 0x1C);
        flags = (flags & 0x3FFFFFFF) | (otherFlags & 0xC0000000);
    };
    float fz = GetPosition().z - 5.f;
    const CVector frontOrigin{fx, fy, GetPosition().z + 5.f};
    if (CWorld::ProcessVerticalLine(frontOrigin, fz, point, entity, true, false, false, false, false, false, nullptr)) {
        fz = point.m_vecPoint.z;
        recordCollision(0x544);
    } else {
        fz = GetPosition().z;
    }
    float bz = GetPosition().z - 5.f;
    const CVector backOrigin{backX, by, GetPosition().z + 5.f};
    if (CWorld::ProcessVerticalLine(backOrigin, bz, point, entity, true, false, false, false, false, false, nullptr)) {
        bz = point.m_vecPoint.z;
        recordCollision(0x570);
    } else {
        bz = GetPosition().z;
    }
    BikeRoadOrientation355(m_matrix, rear, front, backX, by, bz, fx, fy, fz);
    GetUp() = CrossProduct(GetRight(), GetForward());
    const float half = .5f;
    float midX, midY, midZ;
    __asm {
        fld fx
        fadd backX
        fmul half
        fstp midX
        fld fy
        fadd by
        fmul half
        fstp midY
        fld fz
        fadd bz
        fstp midZ
        fld midZ
        fmul half
        fstp midZ
    }
    const float height = reinterpret_cast<float(__thiscall*)(CBike*)>((*reinterpret_cast<uintptr_t**>(this))[53])(this);
    GetPosition().x = midX;
    GetPosition().y = midY;
    GetPosition().z = height + midZ;
}

// 0x6BF230
void CBike::GetCorrectedWorldDoorPosition(CVector& out, CVector arg1, CVector arg2) {
    const auto& forward = GetForward();
    const auto side = CrossProduct(forward, CVector{0.f, 0.f, 1.f});
    const auto up = CrossProduct(side, forward);
    const auto& ownUp = GetUp();
    const auto projection = (side.y * ownUp.y + side.z * ownUp.z) + side.x * ownUp.x;
    const auto& bounds = GetColModel()->GetBoundingBox();
    const auto height = bounds.m_vecMax.z > bounds.m_vecMax.x ? bounds.m_vecMax.z - bounds.m_vecMax.x : 0.f;
    const auto deltaY = arg2.y - arg1.y;
    out = CVector{deltaY * forward.x + 0.f, deltaY * forward.y + 0.f, deltaY * forward.z + 0.f};
    const auto deltaX = height * projection + (arg2.x - arg1.x);
    out = CVector{out.x + side.x * deltaX, out.y + side.y * deltaX, out.z + side.z * deltaX};
    const auto deltaZ = arg2.z - arg1.z;
    out = CVector{out.x + up.x * deltaZ, out.y + up.y * deltaZ, out.z + up.z * deltaZ};
    const auto& pos = GetPosition();
    out = CVector{out.x + pos.x, out.y + pos.y, out.z + pos.z};
}

// 0x6BEA10
void CBike::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    auto* bytes = reinterpret_cast<uint8*>(this);
    if (!(bytes[0x42A] & 0x20)) return;
    m_vecMoveSpeed.z += StaticRef<float>(0x859020);
    bytes[0x36] = (bytes[0x36] & 7) | 0x28;
    m_nPhysicalFlags |= 0x20000000;
    CVisibilityPlugins::SetClumpForAllAtomicsFlag(GetRpClump(), static_cast<eAtomicComponentFlag>(0x4000));
    m_fHealth = 0.f;
    m_wBombTimer = 0;
    TheCamera.CamShake(.4f, GetPosition());
    KillPedsInVehicle();
    bytes[0x4A8] &= 0xE7;
    bytes[0x428] &= 0xAF;
    ChangeLawEnforcerState(false);
    CExplosion::AddExplosion(this, damager, static_cast<eExplosionType>(4), GetPosition(), 0, true, -1.f, bHideExplosion);
    CDarkel::RegisterCarBlownUpByPlayer(*this, false);
}

// 0x6B7050
void CBike::Fix() {
    vehicleFlags.bIsDamaged = false;
    bikeFlags.bEngineOnFire = false;
    m_nWheelStatus[0] = 0;
    m_nWheelStatus[1] = 0;
}

namespace {
float BikePreRenderChassisHeight355(float lean, float height) {
    float result;
    __asm {
        fld lean
        fcos
        fld dword ptr ds:[00858624h]
        fsub st,st(1)
        fmul height
        fmul dword ptr ds:[00858C20h]
        fstp result
        fstp st(0)
    }
    return result;
}
void BikePreRenderContact355(CVector& out, const CVector& point, const CVector& right, float lean, float height) {
    const float* p = &point.x;
    const float* r = &right.x;
    float* o = &out.x;
    __asm {
        mov eax, p
        mov edx, r
        mov ecx, o
        fld lean
        fsin
        fmul height
        fmul dword ptr ds:[00858C98h]
        fld st(0)
        fmul dword ptr [edx]
        fadd dword ptr [eax]
        fstp dword ptr [ecx]
        fld st(0)
        fmul dword ptr [edx+4]
        fadd dword ptr [eax+4]
        fstp dword ptr [ecx+4]
        fmul dword ptr [edx+8]
        fadd dword ptr [eax+8]
        fstp dword ptr [ecx+8]
    }
}
void BikePreRenderTrig355(float angle, float multiplier, float& sineOut, float& cosineOut) {
    float sine, cosine;
    __asm {
        fld angle
        fmul multiplier
        fld st(0)
        fsin
        fstp sine
        fcos
        fstp cosine
    }
    sineOut = sine; cosineOut = cosine;
}
float BikePreRenderSine355(float angle, float factor) {
    float result;
    __asm {
        fld angle
        fsin
        fmul factor
        fstp result
    }
    return result;
}
float BikePreRenderForkAngle355(float height, float original, float forkLength) {
    float result;
    __asm {
        fld height
        fsub original
        fdiv forkLength
        fst result
        mov eax, 00821E70h
        call eax
        fchs
        fstp result
    }
    return result;
}
}

// 0x6BD090
void CBike::PreRender() {
    CVehicle::PreRender();
    auto* col = GetColModel();
    auto* data = col->m_pColData;
    auto* lines = data->m_pLines;
    auto* bytes = reinterpret_cast<uint8*>(this);
    const auto flag = [&](size_t offset, uint8 mask) { return (bytes[offset] & mask) != 0; };
    if (flag(0x42B, 1)) {
        reinterpret_cast<void(__thiscall*)(CBike*)>((*reinterpret_cast<uintptr_t**>(this))[52])(this);
        for (int wheel = 0; wheel < 2; ++wheel) {
            const int first = wheel * 2;
            const float resting = 1.f - m_fSuspensionLength[first] / m_fLineLength[first];
            const float compression = (std::min(m_aWheelRatios[first], m_aWheelRatios[first + 1]) - resting) / (1.f - resting);
            float height = lines[first].m_vecStart.z;
            if (compression > 0.f) height -= compression * m_fSuspensionLength[first];
            m_aWheelSuspensionHeights[wheel] = (height - m_aWheelSuspensionHeights[wheel]) * .75f + m_aWheelSuspensionHeights[wheel];
        }
    }
    const auto status = uint8(bytes[0x36] >> 3);
    if (status <= 3) {
        const float speed = m_vecMoveSpeed.Magnitude();
        const auto rearState = m_WheelStates[1];
        for (int wheel = 0; wheel < 2; ++wheel) {
            int line = wheel ? 3 : 0;
            if (!wheel && m_aRatioHistory[0] >= 1.f && m_aRatioHistory[1] < 1.f) line = 1;
            if (wheel && m_aRatioHistory[3] >= 1.f && m_aRatioHistory[2] < 1.f) line = 2;
            uint32 particles = !wheel && rearState != static_cast<tWheelState>(2) ? 4 : 0;
            auto* contact = &m_aWheelColPoints[line];
            CVector position;
            BikePreRenderContact355(position, contact->m_vecPoint, m_matrix->GetRight(), m_RideAnimData.LeanAngle, GetColModel()->m_boundBox.m_vecMin.z);
            particles += uint32(m_bWheelBloody[wheel]) + 2 * uint32(m_bMoreSkidMarks[wheel]);
            AddSingleWheelParticles(m_WheelStates[wheel], m_nWheelStatus[wheel], m_aRatioHistory[line], speed,
                                    contact, &position, m_RideAnimData.LeanAngle > 0.f ? -1.f : 1.f,
                                    wheel, uint32(m_aWheelSkidmarkType[wheel]), &m_bWheelBloody[wheel], particles);
        }
    }
    m_bLeanMatrixCalculated = false;
    CalculateLeanMatrix();
    if (flag(0x428, 0x10) && !(uint32(m_pHandlingData->m_nModelFlags) & 0x1000) &&
        DotProduct(m_matrix->GetForward(), m_vecMoveSpeed) / StaticRef<float>(0xC1C81C) < 130.f && !flag(0x42B, 0x40)) {
        AddExhaustParticles();
    }
    AddDamagedVehicleParticles();
    if (m_nModelIndex == 523 && flag(0x42D, 0x80) && flag(0x428, 0x10) && !(StaticRef<uint32>(uintptr_t(this) + 0x40) & 0x20000000)) {
        const uint32 phase = CTimer::GetTimeInMS() & 0x1FF;
        const uint8 red = phase < 256 ? uint8(phase) : uint8(-int32(phase));
        const uint8 blue = uint8(255 - red);
        const CVector left{-.28f, .6f, .3f}, right{.28f, .6f, .3f};
        const int intensity = int(StaticRef<float>(0xB7C4E4) * StaticRef<float>(0x8714AC));
        const bool first = phase < 256;
        CCoronas::RegisterCorona(uint32(uintptr_t(this) + (first ? 0x15 : 0x16)), this,
            first ? uint8(intensity) : 0, 0, first ? 0 : uint8(intensity), 255, first ? left : right,
            .4f, 40.f, static_cast<eCoronaType>(0), static_cast<eCoronaFlareType>(0), static_cast<eCoronaReflType>(0),
            static_cast<eCoronaLOSCheck>(0), static_cast<eCoronaTrail>(0), 0.f, false, 1.5f, false, 30.f, false, true);
        const CVector light = m_matrix->GetPosition() + m_matrix->GetForward() + m_matrix->GetUp() * .5f;
        CPointLights::AddLight(0, light, CVector{0.f, 0.f, 0.f}, 10.f,
                              float(red) * StaticRef<float>(0x85C6D4), 0.f,
                              float(blue) * StaticRef<float>(0x85C6D4), 0, true, nullptr);
    }
    DoVehicleLights(m_mLeanMatrix, static_cast<eVehicleLightsFlags>(4));
    CShadows::StoreShadowForVehicle(this, VEH_SHD_BIKE);
    auto* model = GetVehicleModelInfo();
    float steerSine, steerCosine;
    BikePreRenderTrig355(m_fSteerAngle, 1.f, steerSine, steerCosine);
    const CVector wheelDirection = m_matrix->TransformVector(CVector{-steerSine, steerCosine, 0.f});
    const CVector rearDirection = m_matrix->GetForward();
    for (int wheel = 0; wheel < 2; ++wheel) {
        const int first = wheel * 2;
        if (!(m_WheelCounts[first] > 0.f || m_WheelCounts[first + 1] > 0.f)) continue;
        const float radius = (wheel ? model->m_fWheelSizeRear : model->m_fWheelSizeFront) * .5f;
        const CVector local{0.f, (lines[first + 1].m_vecStart.y + lines[first].m_vecStart.y) * .5f,
            lines[first].m_vecStart.z - std::min(m_aRatioHistory[first], m_aRatioHistory[first + 1]) * m_fSuspensionLength[first] - model->m_fWheelSizeFront * .5f};
        // 6BD78D/6BD88A evaluate speed at the local contact, not its world position.
        const CVector contact = GetSpeed(local);
        const float rotation = ProcessWheelRotation(wheel ? m_WheelStates[1] : static_cast<tWheelState>(0), wheel ? rearDirection : wheelDirection, contact, radius);
        m_aWheelAngularVelocity[wheel] = rotation;
        m_aWheelPitchAngles[wheel] += rotation * CTimer::GetTimeStep();
    }
    CMatrix frameMatrix;
    if (m_aBikeNodes[BIKE_FORKS_FRONT]) {
        frameMatrix.Attach(&m_aBikeNodes[BIKE_FORKS_FRONT]->modelling, false);
        const CVector position = frameMatrix.GetPosition();
        CMatrix rotation;
        RwMatrix attached{};
        rotation.Attach(&attached, false);
        rotation.SetUnity();
        rotation.UpdateRW();
        float axisSine, axisCosine;
        BikePreRenderTrig355(model->m_fBikeSteerAngle, StaticRef<float>(0x8595EC), axisSine, axisCosine);
        CVector axis{0.f, axisSine, -axisCosine};
        axis.Normalise();
        CQuaternion quaternion;
        quaternion.Set(reinterpret_cast<RwV3d*>(&axis), -m_RideAnimData.BarSteerAngle);
        quaternion.Get(&attached);
        rotation.Update();
        frameMatrix.SetUnity();
        frameMatrix = frameMatrix * rotation;
        frameMatrix.GetPosition() += position;
        frameMatrix.UpdateRW();
        if (m_aBikeNodes[BIKE_HANDLEBARS]) {
            frameMatrix.Attach(&m_aBikeNodes[BIKE_HANDLEBARS]->modelling, false);
            const CVector extraPosition = frameMatrix.GetPosition();
            if (status == 4 || status == 5) {
                frameMatrix.SetUnity(); frameMatrix *= rotation; frameMatrix.GetPosition() += extraPosition;
            } else frameMatrix.SetTranslate(extraPosition);
            frameMatrix.UpdateRW();
        }
    }
    if (m_aBikeNodes[BIKE_FORKS_REAR]) {
        frameMatrix.Attach(&m_aBikeNodes[BIKE_FORKS_REAR]->modelling, false);
        const CVector position = frameMatrix.GetPosition();
        frameMatrix.SetRotate(BikePreRenderForkAngle355(m_aWheelSuspensionHeights[1], m_aWheelOrigHeights[1], m_fSwingArmLength), 0.f, 0.f);
        frameMatrix.GetPosition() += position; frameMatrix.UpdateRW();
    }
    frameMatrix.Attach(&m_aBikeNodes[BIKE_WHEEL_FRONT]->modelling, false);
    const CVector frontPosition = frameMatrix.GetPosition();
    const float height = m_aWheelSuspensionHeights[0] - m_aWheelOrigHeights[0];
    const CVector suspensionOffset{frontPosition.x,
        (lines[1].m_vecStart.y + lines[0].m_vecStart.y) * .5f - m_fForkYOffset - height * m_fSteerAngleTan, m_aWheelSuspensionHeights[0] - m_fForkZOffset};
    if (m_nWheelStatus[0] == 1) frameMatrix.SetRotate(m_aWheelPitchAngles[0], 0.f, BikePreRenderSine355(m_aWheelPitchAngles[0], .05f));
    else frameMatrix.SetRotateX(m_aWheelPitchAngles[0]);
    frameMatrix.GetPosition() += suspensionOffset; frameMatrix.UpdateRW();
    frameMatrix.Attach(&m_aBikeNodes[BIKE_MUDGUARD]->modelling, false);
    frameMatrix.GetPosition() = suspensionOffset; frameMatrix.UpdateRW();
    frameMatrix.Attach(&m_aBikeNodes[BIKE_WHEEL_REAR]->modelling, false);
    const CVector rearPosition = frameMatrix.GetPosition();
    if (m_nWheelStatus[1] == 1) frameMatrix.SetRotate(m_aWheelPitchAngles[1], 0.f, BikePreRenderSine355(m_aWheelPitchAngles[1], .07f));
    else frameMatrix.SetRotateX(m_aWheelPitchAngles[1]);
    frameMatrix.GetPosition() += rearPosition; frameMatrix.UpdateRW();
    if (m_aBikeNodes[BIKE_CHASSIS]) {
        frameMatrix.Attach(&m_aBikeNodes[BIKE_CHASSIS]->modelling, false);
        const CVector position = frameMatrix.GetPosition();
        const float z = BikePreRenderChassisHeight355(m_RideAnimData.LeanAngle, col->m_boundBox.m_vecMin.z);
        frameMatrix.SetRotateX(std::abs(m_RideAnimData.LeanAngle) * -.05f);
        frameMatrix.RotateY(m_RideAnimData.LeanAngle);
        frameMatrix.GetPosition() += CVector{position.x, position.y, z};
        frameMatrix.UpdateRW();
    }
}


// 0x6BDE20
void CBike::Render() {
    auto savedRef = 0;
    RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, &savedRef);
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(1));

    m_nTimeTillWeNeedThisCar = CTimer::GetTimeInMS() + 3000;
    CVehicle::Render();

    if (m_renderLights.m_bRightFront) {
        CalculateLeanMatrix();
        CVehicle::DoHeadLightBeam(DUMMY_LIGHT_FRONT_MAIN, m_mLeanMatrix, true);
    }

    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(savedRef));
}

// 0x6BCFC0
void CBike::Teleport(CVector destination, bool resetRotation) {
    CWorld::Remove(this);

    GetPosition() = destination;
    if (resetRotation)
        SetOrientation(0.0f, 0.0f, 0.0f);

    ResetMoveSpeed();
    ResetTurnSpeed();
    ResetSuspension();

    CWorld::Add(this);
}

// 0x6B8EC0
void CBike::VehicleDamage(float damageIntensity, eVehicleCollisionComponent component, CEntity* damager, CVector* vecCollisionCoors, CVector* vecCollisionDirection, eWeaponType weapon) {
    if (damageIntensity > 0.f || m_fDamageIntensity < 1.f) return;
    auto* bytes = reinterpret_cast<uint8*>(this);
    if (!(bytes[0x42A] & 0x20)) return;
    float damage = m_fDamageIntensity;
    if (GetStatus() == STATUS_PLAYER && CStats::GetPercentageProgress() >= 100.f) damage *= .5f;
    if ((bytes[0x614] & 0x10) && damage > 20.f) bytes[0x614] &= 0xEF;
    DamageKnockOffRider(this, m_fDamageIntensity, m_nPieceType, m_pDamageEntity, m_vecLastCollisionPosn, m_vecLastCollisionImpactVelocity);
    if (m_pDamageEntity && m_pDamageEntity->GetIsTypeVehicle()) {
        m_nLastWeaponDamageType = 49;
        m_pLastDamageEntity = m_pDamageEntity;
        m_pDamageEntity->RegisterReference(&m_pLastDamageEntity);
    }
    if (m_nPhysicalFlags & 0x100000) return;
    if (m_pDamageEntity && m_pDamageEntity->GetIsTypeBuilding() && DotProduct(m_vecLastCollisionImpactVelocity, GetUp()) > StaticRef<float>(0x858CC8)) return;
    if (damage > 25.f && GetStatus() != STATUS_WRECKED) {
        if ((bytes[0x428] & 1) && FindPlayerVehicle(-1, false) && m_pDamageEntity == FindPlayerVehicle(-1, false) && GetStatus() != STATUS_ABANDONED) {
            auto* playerVehicle = FindPlayerVehicle(-1, false);
            const float ourSpeed = m_vecMoveSpeed.Magnitude();
            if (playerVehicle->m_vecMoveSpeed.Magnitude() >= ourSpeed && FindPlayerVehicle(-1, false)->m_vecMoveSpeed.Magnitude() > StaticRef<float>(0x858B1C)) {
                FindPlayerPed(-1)->SetWantedLevelNoDrop(static_cast<eWantedLevel>(1));
            }
        }
        damage = (damage - 25.f) * m_pHandlingData->m_fCollisionDamageMultiplier;
        if (damage > 0.f) {
            if (damage > 5.f && m_pDriver && m_pDamageEntity && m_pDamageEntity->GetIsTypeVehicle()) {
                auto* other = static_cast<CVehicle*>(m_pDamageEntity);
                if ((FindPlayerVehicle(-1, false) != this || reinterpret_cast<uint8*>(other)[0x4A4] != 2) && other->m_pDriver) {
                    m_pDriver->Say(static_cast<eGlobalSpeechContext>(66), 0, 1.f, false, false, false);
                }
            }
            const float previousHealth = m_fHealth;
            float factor;
            if (FindPlayerVehicle(-1, false) == this) factor = bytes[0x429] & 0x80 ? StaticRef<float>(0x85F0A0) : .5f;
            else if (bytes[0x429] & 0x80) factor = StaticRef<float>(0x85F0FC);
            else if (m_pDamageEntity && m_pDamageEntity == FindPlayerVehicle(-1, false)) factor = StaticRef<float>(0x862D68);
            else factor = StaticRef<float>(0x858C84);
            float* health = &m_fHealth;
            __asm {
                mov eax, health
                fld damage
                fmul factor
                fsubr dword ptr [eax]
                fstp dword ptr [eax]
            }
            if (m_fHealth <= 1.f && previousHealth > 1.f) m_fHealth = 1.f;
        }
    }
    if (m_fHealth < 250.f && !(bytes[0x614] & 0x40)) {
        bytes[0x614] |= 0x40;
        m_BlowUpTimer = 0.f;
        m_Damager = m_pDamageEntity;
        if (m_Damager) m_Damager->RegisterReference(&m_Damager);
    }
}

static CVector BikeFramePosition355(RwFrame* frame, RwFrame* stop) {
    RwMatrix matrix = frame->modelling;
    for (auto* parent = RwFrameGetParent(frame); parent; parent = RwFrameGetParent(parent)) {
        RwMatrixTransform(&matrix, &parent->modelling, rwCOMBINEPOSTCONCAT);
        if (RwFrameGetParent(parent) == stop) break;
    }
    return CVector{matrix.pos};
}

static void BikeSetupEnd355(CColLine* line, float upper, float lower, float wheel) {
    const float half = .5f;
    __asm {
        mov eax, line
        fld wheel
        fmul half
        fld lower
        fsub upper
        fxch st(1)
        fsubp st(1), st
        fadd dword ptr [eax+8]
        fstp dword ptr [eax+18h]
    }
}

static float BikeSetupHeight355(float force, float suspensionLength, float wheel, float startZ) {
    const float half = .5f, one = 1.f, divisor = StaticRef<float>(0x858B90);
    float result;
    __asm {
        fld force
        fmul divisor
        fdivr one
        fsubr one
        fmul suspensionLength
        fld wheel
        fmul half
        fsub startZ
        faddp st(1), st
        fstp result
    }
    return result;
}

// 0x6B89B0
static void BikeSetupSphere355(CColSphere* sphere,float limit) {
    __asm {
        mov ecx,sphere
        fld dword ptr [ecx+8]
        fsub dword ptr [ecx+0Ch]
        fcomp limit
        fnstsw ax
        test ah,5
        jp bike_setup_sphere_done355
        fld dword ptr [ecx+0Ch]
        fcomp dword ptr ds:[00858EE8h]
        fnstsw ax
        test ah,41h
        jne bike_setup_sphere_center355
        fld dword ptr [ecx+8]
        fsub limit
        fld dword ptr ds:[00858EE8h]
        fcomp st(1)
        fnstsw ax
        test ah,41h
        jne bike_setup_sphere_radius355
        fstp st(0)
        fld dword ptr ds:[00858EE8h]
    bike_setup_sphere_radius355:
        fstp dword ptr [ecx+0Ch]
    bike_setup_sphere_center355:
        fld limit
        fadd dword ptr [ecx+0Ch]
        fstp dword ptr [ecx+8]
    bike_setup_sphere_done355:
    }
}

void CBike::SetupSuspensionLines() {
    auto* model = CModelInfo::GetModelInfo(m_nModelIndex)->AsVehicleModelInfoPtr();
    auto* col = model->GetColModel();
    auto* data = col->m_pColData;
    const bool adjustSpheres = std::bit_cast<uint32>(data->m_pLines[1].m_vecStart.z) == 0x47C34FFF;
    const bool haveLines = std::bit_cast<uint32>(data->m_pLines[0].m_vecStart.z) != 0x47C34FFF;
    for (size_t i = 0; i < 4; ++i) {
        CVector start;
        const auto wheelSize = i < 2 ? model->m_fWheelSizeFront : model->m_fWheelSizeRear;
        if (haveLines) {
            start = data->m_pLines[i].m_vecStart;
            start.z = m_aWheelOrigHeights[i < 2 ? 0 : 1];
        } else {
            auto* frame = m_aBikeNodes[i < 2 ? BIKE_WHEEL_FRONT : BIKE_WHEEL_REAR];
            start = BikeFramePosition355(frame, static_cast<RwFrame*>(GetRpClump()->object.parent));
            if (i == 0) m_aWheelOrigHeights[0] = start.z;
            if (i == 2) {
                m_aWheelOrigHeights[1] = start.z;
                if (m_aBikeNodes[BIKE_FORKS_REAR]) {
                    const auto fork = BikeFramePosition355(m_aBikeNodes[BIKE_FORKS_REAR], static_cast<RwFrame*>(GetRpClump()->object.parent));
                    const auto dy = start.y - fork.y;
                    const auto dz = start.z - fork.z;
                    m_fSwingArmLength = std::sqrt(dy * dy + dz * dz);
                } else {
                    m_fSwingArmLength = 0.f;
                }
            }
            start.y += wheelSize * StaticRef<float>(i % 2 == 0 ? 0x858C84 : 0x85C654);
        }
        start.z += m_pHandlingData->m_fSuspensionUpperLimit;
        auto& line = data->m_pLines[i];
        line.m_vecStart = start;
        line.m_vecEnd.x = start.x;
        line.m_vecEnd.y = start.y;
        BikeSetupEnd355(&line, m_pHandlingData->m_fSuspensionUpperLimit, m_pHandlingData->m_fSuspensionLowerLimit, wheelSize);
        m_fSuspensionLength[i] = m_pHandlingData->m_fSuspensionUpperLimit - m_pHandlingData->m_fSuspensionLowerLimit;
        m_fLineLength[i] = line.m_vecStart.z - line.m_vecEnd.z;
    }
    if (!haveLines) {
        const auto fork = BikeFramePosition355(m_aBikeNodes[BIKE_FORKS_FRONT], m_aBikeNodes[BIKE_FORKS_FRONT]);
        m_fForkYOffset = fork.y;
        m_fForkZOffset = fork.z;
    }
    m_fHeightAboveRoad = BikeSetupHeight355(m_pHandlingData->m_fSuspensionForceLevel, m_fSuspensionLength[0], model->m_fWheelSizeFront, data->m_pLines[0].m_vecStart.z);
    m_aWheelSuspensionHeights[0] = model->m_fWheelSizeFront * .5f - m_fHeightAboveRoad;
    m_aWheelSuspensionHeights[1] = model->m_fWheelSizeRear * .5f - m_fHeightAboveRoad;
    if (data->m_pLines[0].m_vecEnd.z < col->m_boundBox.m_vecMin.z) col->m_boundBox.m_vecMin.z = data->m_pLines[0].m_vecEnd.z;
    const auto radius = std::max(col->m_boundBox.m_vecMin.Magnitude(), col->m_boundBox.m_vecMax.Magnitude());
    if (radius > col->m_boundSphere.m_fRadius) col->m_boundSphere.m_fRadius = radius;
    if ((uint32(m_pHandlingData->m_nModelFlags) & 0x40000000) && adjustSpheres) {
        const float limit = StaticRef<float>(0x858C84) - m_fHeightAboveRoad;
        for (size_t i = 0; i < data->m_nNumSpheres; ++i) {
            auto& sphere = data->m_pSpheres[i];
            BikeSetupSphere355(&sphere,limit);
        }
    }
}

// 0x6B8970
void CBike::SetModelIndex(uint32 index) {
    CVehicle::SetModelIndex(index);
    SetupModelNodes();
}

// 0x6B5960
void CBike::SetupModelNodes() {
    std::ranges::fill(m_aBikeNodes, nullptr);
    // Como el original (0x6B5975): pasa [this+0x18] tal cual a FillFrameArray, sin comprobar el tipo del objeto RW (GetRpClump() haria assert/lectura)
    CClumpModelInfo::FillFrameArray(reinterpret_cast<RpClump*>(GetRwObject()), m_aBikeNodes.data());
}

// 0x6B7080
void CBike::PlayCarHorn() {
    if (!(m_nAlarmState == 0 || m_nAlarmState == uint16(0xFFFF) || GetStatus() == STATUS_WRECKED) || m_HornCounter) {
        return;
    }
    auto& delay = *reinterpret_cast<uint8*>(&m_nCarHornTimer);
    if (delay) {
        --delay;
        return;
    }
    const auto random = CGeneral::GetRandomNumber();
    delay = uint8((random & 127) - 106);
    const auto pattern = uint32(delay) & 7;
    if (pattern < 4) {
        if (pattern >= 2 && m_pDriver && m_autoPilot.carCtrlFlags.bHonkAtCar) {
            m_pDriver->Say(static_cast<eGlobalSpeechContext>(22));
        }
        m_HornCounter = 45;
    } else if (m_pDriver) {
        m_pDriver->Say(static_cast<eGlobalSpeechContext>(22));
    }
}

// 0x6B7070
void CBike::SetupDamageAfterLoad() {
    // NOP
}

// 0x6B6950
void CBike::DoBurstAndSoftGroundRatios() {
    const auto* model = GetVehicleModelInfo();
    const auto& forward = GetForward();
    const auto speed = std::fabs((m_vecMoveSpeed.z * forward.z + m_vecMoveSpeed.y * forward.y) + m_vecMoveSpeed.x * forward.x);
    std::array<bool,4> applySoft{true,true,true,true};
    for (size_t wheel = 0; wheel < 2; ++wheel) {
        const auto first = wheel * 2;
        const auto second = first + 1;
        if (m_nWheelStatus[wheel] == 2) {
            m_aWheelRatios[first] = 1.f;
            m_aWheelRatios[second] = 1.f;
            applySoft[first] = applySoft[second] = false;
        } else if (m_nWheelStatus[wheel] == 1) {
            const auto random = float(CGeneral::GetRandomNumber() & 0xFFFF) * StaticRef<float>(0x858B14);
            const auto speedInteger = uint16(int64(speed * StaticRef<float>(0x858A10)));
            if (int64(random * float(uint32(speedInteger) + 98u)) < 100) {
                const auto amount = (m_fLineLength[first] - m_fSuspensionLength[first]) / m_fLineLength[first] * StaticRef<float>(0x871310);
                const auto left = m_aWheelRatios[first] + amount;
                const auto right = m_aWheelRatios[second] + amount;
                m_aWheelRatios[first] = left < 1.f ? left : 1.f;
                m_aWheelRatios[second] = right < 1.f ? right : 1.f;
            }
            applySoft[first] = applySoft[second] = false;
        } else if ((m_aWheelRatios[first] < 1.f && m_aWheelColPoints[first].m_nSurfaceTypeB == static_cast<eSurfaceType>(178)) || (m_aWheelRatios[second] < 1.f && m_aWheelColPoints[second].m_nSurfaceTypeB == static_cast<eSurfaceType>(178))) {
            auto cycle = StaticRef<float>(0x871328) / ((wheel == 0 ? model->m_fWheelSizeFront : model->m_fWheelSizeRear) * StaticRef<float>(0x858B8C));
            if (speed > StaticRef<float>(0x87132C)) cycle *= speed / StaticRef<float>(0x87132C);
            const auto inverse = 1.f / cycle;
            const auto previous = inverse * m_aWheelPitchAngles[wheel];
            const auto previousPhase = previous - float(std::floor(double(previous)));
            const auto next = (CTimer::GetTimeStep() * m_aWheelAngularVelocity[wheel] + m_aWheelPitchAngles[wheel]) * inverse;
            const auto nextPhase = next - float(std::floor(double(next)));
            if ((m_aWheelAngularVelocity[wheel] > 0.f && nextPhase < previousPhase) || (m_aWheelAngularVelocity[wheel] < 0.f && nextPhase > previousPhase)) {
                const auto amount = (m_fLineLength[first] - m_fSuspensionLength[first]) / m_fLineLength[first] * StaticRef<float>(0x871324);
                const auto left = m_aWheelRatios[first] - amount;
                const auto right = m_aWheelRatios[second] - amount;
                m_aWheelRatios[first] = left > StaticRef<float>(0x858CC4) ? left : StaticRef<float>(0x858CC4);
                m_aWheelRatios[second] = right > StaticRef<float>(0x858CC4) ? right : StaticRef<float>(0x858CC4);
            }
            applySoft[first] = applySoft[second] = false;
        }
    }
    for (size_t i = 0; i < 4; ++i) {
        if (!applySoft[i] || !(m_aWheelRatios[i] < 1.f) || g_surfaceInfos.GetAdhesionGroup(m_aWheelColPoints[i].m_nSurfaceTypeB) != ADHESION_GROUP_SAND || m_nModelIndex == 432) continue;
        const auto flags = uint32(m_nHandlingFlagsIntValue);
        const auto multiplier = flags & 0x200000u ? StaticRef<float>(0x87131C) : flags & 0x100000u ? StaticRef<float>(0x871318) : StaticRef<float>(0x871314);
        auto factor = (1.f - speed / StaticRef<float>(0x871320) * StaticRef<float>(0x858CB0)) - StaticRef<float>(0xC81308) * StaticRef<float>(0x858CB0);
        if (StaticRef<float>(0x858EE8) > factor) factor = StaticRef<float>(0x858EE8);
        const auto amount = (m_fLineLength[i] - m_fSuspensionLength[i]) / m_fLineLength[i] * factor * multiplier;
        const auto ratio = amount + m_aWheelRatios[i];
        m_aWheelRatios[i] = ratio;
        if (ratio > 1.f) m_aWheelRatios[i] = 1.f;
    }
}

// 0x6B67E0
bool CBike::SetUpWheelColModel(CColModel* wheelCol) {
    const auto* source = GetColModel();
    wheelCol->m_boundBox = source->m_boundBox;
    wheelCol->m_boundSphere = source->m_boundSphere;
    auto* data = wheelCol->GetData();
    const auto* model = GetVehicleModelInfo();
    const auto setWheel = [&](int32 component, int32 index, float radius, uint8 piece) {
        auto* frame = m_aBikeNodes[component];
        RwMatrix matrix = *RwFrameGetMatrix(frame);
        for (auto* parent = RwFrameGetParent(frame); parent; parent = RwFrameGetParent(parent)) {
            RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
            if (RwFrameGetParent(parent) == m_aBikeNodes[BIKE_CHASSIS]) break;
        }
        data->m_pSpheres[index].Set(radius, CVector{matrix.pos}, static_cast<eSurfaceType>(67), piece, tColLighting{255});
    };
    setWheel(BIKE_WHEEL_FRONT, 0, model->m_fWheelSizeFront * .5f, 13);
    setWheel(BIKE_WHEEL_REAR, 1, model->m_fWheelSizeRear * .5f, 15);
    data->m_nNumSpheres = 2;
    return true;
}

// 0x6B67B0
void CBike::RemoveRefsToVehicle(CEntity* entityToRemove) {
    for (auto& entity: m_aGroundPhysicalPtrs) {
        if (entity == entityToRemove)
            entity = nullptr;
    }
}

// 0x6B6620
void CBike::ProcessControlCollisionCheck(bool applySpeed) {
    const CMatrix oldMat = GetMatrix();
    SetIsStuck(false);
    SkipPhysics();
    physicalFlags.bSkipLineCol     = false;
    physicalFlags.bProcessingShift = false;
    m_fMovingSpeed                 = 0.0f;
    rng::fill(m_aWheelRatios, 1.0f);

    if (applySpeed) {
        ApplyMoveSpeed();
        ApplyTurnSpeed();

        for (auto i = 0; CheckCollision() && i < 5; i++) {
            GetMatrix() = oldMat;
            ApplyMoveSpeed();
            ApplyTurnSpeed();
        }
    } else {
        const auto usesCollision = GetUsesCollision();
        SetUsesCollision(false);
        CheckCollision();
        SetUsesCollision(usesCollision);
    }

    SetIsStuck(false);
    SetIsInSafePosition(true);
}

// 0x6B5990
void CBike::GetComponentWorldPosition(int32 componentId, CVector& outPos) {
    if (IsComponentPresent(componentId))
        outPos = RwFrameGetLTM(m_aBikeNodes[componentId])->pos;
    else
        NOTSA_LOG_DEBUG("BikeNode missing: model={}, nodeIdx={}", m_nModelIndex, componentId);
}

// 0x6B58D0
void CBike::ProcessOpenDoor(CPed* ped, uint32 doorComponentId, uint32 animGroup, uint32 animId, float fTime) {
    // NOP
}
