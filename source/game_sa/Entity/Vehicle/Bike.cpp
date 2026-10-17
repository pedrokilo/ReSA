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
 * Funciones bloqueadas: 0.
 * Validación registrada: 40/40 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
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
#include "X87Math.h"
#include "CarCtrl.h"
#include "CullZones.h"
#include "Stats.h"
#include "Pad.h"
#include "SurfaceInfos_c.h"
#include "RwHelper.h"
#include "Fx/FxManager.h"
#include "Fx/FxSystem.h"
#include "Events/EventVehicleOnFire.h"
#include "Events/EventGlobalGroup.h"
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

// Como el original (0x6BF59E..0x6BF5BD): fld [mi+58h]; fmul [8595ECh] (0.017453292f = 0x3C8EFA35); fptan; fstp st(0); fstp [this+7A4h].
// La tangente sale de fptan del x87 (precision completa) sobre el producto de 24 bits (BikeTanScaledCpp, X87Math.cpp); std::tan (tanf del CRT) puede diferir en el ultimo bit.
static float BikeSteerAngleTanExe(float degrees) {
    return BikeTanScaledCpp(degrees, std::bit_cast<float>(0x3C8EFA35u));
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

    m_pHandlingData = gHandlingDataMgr.GetVehiclePointer(static_cast<uint8>(mi->m_nHandlingId)); // 0x6BF520: movzx eax,dl (solo el byte bajo)
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
    m_fSteerAngleTan = BikeSteerAngleTanExe(mi->m_fBikeSteerAngle); // 0x6BF59E..0x6BF5BD
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

    // Como el original (0x6BF690..0x6BF6AD): el bucle de 2 iteraciones solo escribe m_nWheelStatus (0x65C), m_aWheelSkidmarkType (0x744),
    // m_bWheelBloody (0x74C), m_bMoreSkidMarks (0x74E), m_aWheelPitchAngles (0x750), m_aWheelAngularVelocity (0x758) y m_WheelStates (0x80C);
    // m_aWheelSuspensionHeights (0x760) y m_aWheelOrigHeights (0x768) NO se inicializan (los rellena SetupSuspensionLines)
    for (auto i = 0; i < 2; ++i) {
        m_nWheelStatus[i] = 0;
        m_aWheelSkidmarkType[i] = eSkidmarkType::DEFAULT;
        m_bWheelBloody[i] = false;
        m_bMoreSkidMarks[i] = false;
        m_aWheelPitchAngles[i] = 0.0f;
        m_aWheelAngularVelocity[i] = 0.0f;
        m_WheelStates[i] = WHEEL_STATE_NORMAL;
    }

    // Como el original (0x6BF6E0..0x6BF714): el bucle de 4 iteraciones escribe m_aGroundOffsets (0x7D0, vector cero), m_aGroundPhysicalPtrs (0x7C0),
    // m_aWheelRatios (0x710) = 1.0, m_aRatioHistory (0x720) = 1.0 (mov [eax+10h],edx con edx = 3F800000h) y m_WheelCounts (0x730) = 0;
    // m_aWheelColPoints (0x660), m_fSuspensionLength (0x770) y m_fLineLength (0x780) NO se inicializan
    for (auto i = 0; i < 4; ++i) {
        m_aWheelRatios[i] = 1.0f;
        m_aRatioHistory[i] = 1.0f;
        m_WheelCounts[i] = 0.0f;
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
        // Como el original (0x6BF75D): mov [eax+28h],ebp (47C34FFFh) -> lines[1].m_vecStart.z (stride 0x20 + 8), no .x
        mi->m_pColModel->m_pColData->m_pLines[1].m_vecStart.z = 99'999.99f; // todo: explain this
    }
    mi->m_pColModel->m_pColData->m_pLines[0].m_vecStart.z = 99'999.99f;
    CBike::SetupSuspensionLines();

    m_autoPilot.m_nTempAction = TEMPACT_NONE;
    m_autoPilot.SetCarMission(MISSION_NONE, 0);
    m_autoPilot.carCtrlFlags.bAvoidLevelTransitions = false;

    SetStatus(STATUS_SIMPLE);
    m_nNumPassengers = 0;
    // Como el original (0x6BF79A..0x6BF7B5): and al,0F0h sobre el byte 0x429 = bIsVan, bIsBus, bIsBig y bLowVehicle (la fuente olvidaba bIsBus)
    vehicleFlags.bLowVehicle = false;
    vehicleFlags.bIsBig = false;
    vehicleFlags.bIsBus = false;
    vehicleFlags.bIsVan = false;

    m_bLeanMatrixCalculated = false;
    // Como el original (0x6BF468 y 0x6BF46E): mov [esi+60Ch],ebx / mov [esi+610h],ebx = dos stores de dword (puntero RW y bandera de propiedad con su
    // relleno de 3 bytes); el constructor por defecto de la DLL solo escribe el puntero y el bool
    std::memset(reinterpret_cast<uint8*>(&m_mLeanMatrix) + 0x40, 0, 8);
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

    // A ped doing a gang drive-by isn't knocked off either (cops are exempt)
    // Como el original (0x6B5AD6..0x6B5B17): GetActiveTask y luego GetTaskType (vtable +10h) contra 3FEh = 1022 = TASK_SIMPLE_GANG_DRIVEBY
    // (CTaskSimpleGangDriveBy::GetTaskType = 0x6218B0 devuelve 3FEh; TASK_SIMPLE_BE_HIT es 3F0h) y despues pedType == 6 (cop) en [driver+598h]
    if (const auto task = driver->GetIntelligence()->GetTaskManager().GetActiveTask()) {
        if (task->GetTaskType() == TASK_SIMPLE_GANG_DRIVEBY && !driver->IsCop()) {
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
    // 0x6B6113..0x6B6140: pow(multiplicador, paso) con el _CIpow del EXE (0x822130) sin redondear; cada producto se guarda como float
    PowScaleVector3Cpp(&m_vecMoveSpeed.x, fBuoyancyForceMult, timeStep);
    PowScaleVector3Cpp(&m_vecTurnSpeed.x, fBuoyancyForceMult, timeStep);

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
    return BikeRiderSmoothCpp(base, step, previous, target);
}
void BikeRiderWeights355(float lateral,float forward,float available,float* lateralOut,float* forwardOut) {
    BikeRiderWeightsCpp(lateral, forward, available, lateralOut, forwardOut);
}
float BikeRiderJitter355(float speed,int randomValue) {
    return BikeRiderJitterCpp(speed, randomValue);
}
float BikeRiderForwardLimit355(float lean,float target) {
    return BikeRiderForwardLimitCpp(lean, target);
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
    // Como el original: pasa [rider+0x18] sin comprobar el tipo del objeto RW (GetRpClump() hace assert con el fixture del banco)
    auto* clump=reinterpret_cast<RpClump*>(rider->GetRwObject());
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
    return BikeControlBlendCpp(oldValue, input, step, factor);   // 0x6BE497: la constante 0.0078125f (0x858B88) va dentro del ayudante
}

static float BikeControlProductAdd355(float oldValue, float input, float factor) {
    return BikeControlProductAddCpp(oldValue, input, factor);
}

static float BikeInputSteer355(float degrees, float steer) {
    return BikeInputSteerCpp(degrees, steer);
}

static float BikeForwardSpeed355(const CVector* speed, const CVector* forward) {
    return BikeForwardSpeedCpp(&speed->x, &forward->x);
}

static void BikeMouseDecay355(float* value) {
    PowScaleFloatCpp(value, StaticRef<float>(0x87130C), CTimer::GetTimeStep());   // 0x6BE562..0x6BE579: pow(base, paso) * valor, el pow sin redondear
}

static void BikeInputSpeedCap355(CVector* speed) {
    BikeSpeedCapCpp(&speed->x, StaticRef<float>(0x871254));
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

namespace {
// Como el original (0x40FC80): CColPoint::operator= del EXE copia campo a campo sin los rellenos 0xC, 0x1C y 0x26..0x27
void BikeCopyColPointExe(CColPoint& dst, const CColPoint& src) {
    auto*       d = reinterpret_cast<uint8*>(&dst);
    const auto* s = reinterpret_cast<const uint8*>(&src);
    std::memcpy(d + 0x00, s + 0x00, 0xC);
    std::memcpy(d + 0x10, s + 0x10, 0xC);
    std::memcpy(d + 0x20, s + 0x20, 6);
    std::memcpy(d + 0x28, s + 0x28, 4);
}

// Como el original (0x59C790, Multiply3x3): orden de sumas del EXE; x = (up.x*z + fwd.x*y) + right.x*x, y = (up.y*z + right.y*x) + fwd.y*y,
// z = (up.z*z + right.z*x) + fwd.z*y; sin traslacion
CVector BikeMultiply3x3Exe(const CMatrix& m, const CVector& v) {
    return {
        (m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y
    };
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
            // Como el original (0x6BDFF8..0x6BE08C): copia de la matriz del vehiculo (CMatrix::CMatrix(const CMatrix&) 0x59BCF0) y su posicion
            // se suma el asiento del conductor rotado con Multiply3x3 (0x59C790, SIN traslacion; la fuente sumaba TransformPoint, que
            // duplicaba la posicion del vehiculo). El asiento es el dummy 0 si el model info es un barco (+3Ch == 5) y el 4 en otro caso.
            CMatrix driverMat = GetMatrix();
            {
                const auto* const mi = CModelInfo::GetModelInfo(static_cast<int16>(m_nModelIndex))->AsVehicleModelInfoPtr(); // 0x6BE005: movsx eax,word ptr [esi+22h]
                const CVector seatOS = mi->m_pVehicleStruct->m_avDummyPos[mi->m_nVehicleType == VEHICLE_TYPE_BOAT ? 0 : 4];
                const CVector seatWS = BikeMultiply3x3Exe(GetMatrix(), seatOS);
                driverMat.GetPosition().x += seatWS.x;
                driverMat.GetPosition().y += seatWS.y;
                driverMat.GetPosition().z += seatWS.z;
            }

            // Como el original (0x6BE0A9..0x6BE0D6): el EXE escribe en un array estatico de 32 CColPoint (0xC1C280); aqui es un local
            std::array<CColPoint, 32> pedEntityColPts;
            const auto numPedEntityColPts = CCollision::ProcessColModels(
                driverMat, *m_pDriver->GetColModel(),
                entity->GetMatrix(), *entity->GetColModel(),
                pedEntityColPts,
                nullptr,
                nullptr,
                false
            );

            // Como el original (0x6BE0D9..0x6BE0DF): jle -> solo con resultado positivo
            if (numPedEntityColPts > 0) {
                if (m_nTestPedCollision == 1) {
                    m_nTestPedCollision = 0;
                } else {
                    // Como el original (0x6BE110..0x6BE144): sale cuando numColPts >= 31 (cmp [esp+18h],1Fh / jge), omite las piezas con
                    // m_nPieceTypeA == 0 (PED_COL_SPHERE_LEG: test cl,cl / jbe) y copia con CColPoint::operator= del EXE (0x40FC80)
                    for (auto i = 0; i < numPedEntityColPts && numColPts < 31; i++) {
                        const auto& pedEntityCP = pedEntityColPts[i];
                        if (pedEntityCP.m_nPieceTypeA == PED_COL_SPHERE_LEG) {
                            continue;
                        }
                        BikeCopyColPointExe(outColPoints[numColPts++], pedEntityCP);
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
            // Como el original (0x6BE190..0x6BE1B6): fcomp + test ah,5 + jp salta con ratio >= 1.0, ratio >= anterior y NaN
            if (!(wheelColPtsTouchDist < 1.f) || !(wheelColPtsTouchDist < ogWheelRatios[i])) {
                continue;
            }

            numProcessedLines++;

            m_anCollisionLighting[i] = cp.m_nLightingB;
            m_nContactSurface = cp.m_nSurfaceTypeB;

            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                // Como el original (0x6BE1DC..0x6BE1E1): mov [ecx],edi + CEntity::RegisterReference (0x571B70), sin limpiar la referencia anterior
                CEntity::SetEntityReference(m_aGroundPhysicalPtrs[i], entity->AsPhysical());

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

namespace {
// Como el original (0x406DA0): CVector::SquaredMagnitude suma (x*x + y*y) + z*z en ese orden.
float BikeSqMagExe(const CVector& v) { return (v.x * v.x + v.y * v.y) + v.z * v.z; }

// Como el original (0x40FDB0): producto escalar con la suma en orden z, y, x.
float BikeDotZYXExe(const CVector& a, const CVector& b) { return (a.z * b.z + a.y * b.y) + a.x * b.x; }

// Como el original (0x59C810, Multiply3x3 vector * matriz): x = (r.y*v.y + r.z*v.z) + v.x*r.x; y = (f.y*v.y + f.x*v.x) + f.z*v.z; z = (u.y*v.y + u.x*v.x) + u.z*v.z
CVector BikeMultiply3x3VMExe(const CVector& v, const CMatrix& m) {
    return {
        (m.GetRight().y * v.y + m.GetRight().z * v.z) + v.x * m.GetRight().x,
        (m.GetForward().y * v.y + m.GetForward().x * v.x) + m.GetForward().z * v.z,
        (m.GetUp().y * v.y + m.GetUp().x * v.x) + m.GetUp().z * v.z
    };
}

// Como el original (0x59C730): producto vectorial a x b con las restas en el orden del EXE.
CVector BikeCrossExe(const CVector& a, const CVector& b) {
    return {
        b.z * a.y - a.z * b.y,
        a.z * b.x - b.z * a.x,
        a.x * b.y - b.x * a.y
    };
}
}

// 0x6B9250
void CBike::ProcessControl() {
    // ---- 0x6B9250: prologo ----
    CColModel* const colModel = GetColModel();                                  // 0x6B9271
    CCollisionData* const colData = colModel->m_pColData;                       // [esp+74h] = [colModel+2Ch]
    CVehicleModelInfo* const modelInfo = GetVehicleModelInfo();                 // [esp+60h] = ms_modelInfoPtrs[m_nModelIndex]
    uint32 extraHandlingFlags = 0;                                              // [esp+7Ch]
    m_vehicleAudio.Service();                                                   // 0x6B9298
    vehicleFlags.bWarnedPeds = false;                                           // [429h] &= DFh
    m_bLeanMatrixCalculated = false;                                            // [5C8h] = 0
    m_nBrakesOn = 0;                                                            // [7A8h] = 0 (word)
    bikeFlags.bPlayerBoost = false;                                             // [614h] &= DFh
    vehicleFlags.bRestingOnPhysical = false;                                    // [42Ch] &= FDh
    if (StaticRef<uint8>(0xA43088) == 1) {                                      // CReplay::Mode == MODE_PLAYBACK
        return;
    }

    ProcessCarAlarm();                                                          // 0x6B92E7
    ActivateBombWhenEntered();                                                  // 0x6B92EE
    UpdateClumpAlpha();                                                         // 0x6B92F5

    // 0x6B92FA..0x6B933E: el segundo ocupante solo se mira si hay conductor y no es el jugador
    if (m_pDriver && (m_pDriver->IsPlayer() || (m_apPassengers[0] && m_apPassengers[0]->IsPlayer()))) {
        if (m_nTestPedCollision == 1) {
            m_nTestPedCollision = 2;
        } else if (m_nTestPedCollision == 0) {
            m_nTestPedCollision = 1;
        }
    } else {
        m_nTestPedCollision = 0;
    }

    ProcessAI(extraHandlingFlags);                                              // 0x6B934E: call [vtbl+108h]

    if (GetStatus() == STATUS_SIMPLE) {                                         // 0x6B935A
        return;
    }

    // 0x6B9363..0x6B9398: el caballete se retira si la moto se inclina demasiado
    if (bikeFlags.bOnSideStand) {
        const CMatrix& matrix = *m_matrix;
        if (std::abs(matrix.GetRight().z) > StaticRef<float>(0x858F9C) || std::abs(matrix.GetForward().z) > 0.5f) {
            bikeFlags.bOnSideStand = false;
        }
    }

    // ---- 0x6B939E: centro de masa / estabilizacion de la inclinacion ----
    if (!(extraHandlingFlags & 2) && !(nBikeFlags & 0x18)) {
        m_vecCentreOfMass.x = m_pHandlingData->m_vecCentreOfMass.x;             // [esi+A4h] = [handling+14h]
        m_vecCentreOfMass.y = m_pHandlingData->m_vecCentreOfMass.y;             // [esi+A8h] = [handling+18h]
        m_vecCentreOfMass.z = m_BikeHandling->m_fNoPlayerCOMz;                  // [esi+ACh] = [bikeHandling+28h]
    } else {
        // 0x6B93DA..0x6B97A6
        float leanCoef  = StaticRef<float>(0x8712D8);                           // [esp+34h]
        float dampX     = StaticRef<float>(0x8D3238);                           // [esp+3Ch]
        float dampY     = StaticRef<float>(0x8D323C);                           // [esp+40h]
        const CVector angVel = BikeMultiply3x3VMExe(m_vecTurnSpeed, *m_matrix); // 0x59C810 sobre [esp+68h]
        float stabilise;                                                        // ST0 en 0x6B95AD
        if (GetStatus() == STATUS_PLAYER) {                                     // test [esi+36h],F8h
            if (m_aWheelRatios[0] >= 1.0f && m_aWheelRatios[1] >= 1.0f) {       // rueda delantera en el aire
                leanCoef = StaticRef<float>(0x8712DC);
                const float muscle = CStats::GetFatAndMuscleModifier(STAT_MOD_13) * StaticRef<float>(0x858CC4); // [esp+10h]
                if ((m_aWheelRatios[2] < 1.0f || m_aWheelRatios[3] < 1.0f) && m_matrix->GetForward().z > 0.0f) {
                    const float excess = std::abs(m_BikeHandling->m_fWheelieAng - m_matrix->GetForward().z) * muscle;
                    stabilise = dampX - (excess > StaticRef<float>(0x858C28) ? StaticRef<float>(0x858C28) : excess);
                } else {
                    stabilise = StaticRef<float>(0x8712E0);
                }
            } else if (m_WheelCounts[2] <= 0.0f && m_WheelCounts[3] <= 0.0f) {
                leanCoef = StaticRef<float>(0x8712DC);
                const float limit = CStats::GetFatAndMuscleModifier(STAT_MOD_13) * StaticRef<float>(0x8714A8);   // [esp+10h]
                const float scale = CStats::GetFatAndMuscleModifier(STAT_MOD_13) * StaticRef<float>(0x858C84);   // [esp+4Ch]
                if (m_matrix->GetForward().z < 0.0f) {
                    const float excess = std::abs(m_BikeHandling->m_fStoppieAng - m_matrix->GetForward().z) * scale;
                    stabilise = ((excess > limit ? limit : excess) + StaticRef<float>(0x858C20)) * dampX;
                } else {
                    stabilise = dampX;
                }
            } else {
                stabilise = dampX;
            }
        } else {
            stabilise = dampX;
        }
        dampX = stabilise / ((angVel.x * angVel.x) * leanCoef + 1.0f);
        dampY = dampY / (((StaticRef<float>(0x8712E4) * angVel.y) * angVel.y) + 1.0f);
        dampX = CnvDecayPowCpp(dampX, CTimer::GetTimeStep());                   // 0x6B95ED: _CIpow guardado como float
        const float deltaY = QuadBikeDampDeltaFullCpp(dampY, CTimer::GetTimeStep(), angVel.y); // pow(dampY, paso) * angY - angY, pow sin redondear
        const float deltaX = angVel.x * dampX - angVel.x;                       // [esp+80h]
        const float turnMass = m_fTurnMass;
        const CMatrix& matrix = *m_matrix;
        const float minusOne = StaticRef<float>(0x858C1C);                      // fmul -1.0f (no cambia el signo de un NaN)
        {
            const float termZ = (matrix.GetUp().z * minusOne) * deltaY;
            const float termY = (matrix.GetUp().y * minusOne) * deltaY;
            const float termX = (matrix.GetUp().x * minusOne) * deltaY;
            const CVector force{termX * turnMass, termY * turnMass, termZ * turnMass};
            const CVector arm = BikeMultiply3x3Exe(matrix, m_vecCentreOfMass);
            const CVector point{arm.x + matrix.GetRight().x, arm.y + matrix.GetRight().y, arm.z + matrix.GetRight().z};
            ApplyTurnForce(force, point);                                       // 0x6B96EF
        }
        {
            const float termY = deltaX * matrix.GetUp().y;
            const float termX = deltaX * matrix.GetUp().x;
            const float termZ = deltaX * matrix.GetUp().z;
            const CVector force{termX * turnMass, termY * turnMass, termZ * turnMass};
            const CVector arm = BikeMultiply3x3Exe(matrix, m_vecCentreOfMass);
            const CVector point{arm.x + matrix.GetForward().x, arm.y + matrix.GetForward().y, arm.z + matrix.GetForward().z};
            ApplyTurnForce(force, point);                                       // 0x6B97A1
        }
        if (GetStatus() != STATUS_PLAYER) {
            m_vecCentreOfMass = m_pHandlingData->m_vecCentreOfMass;             // 0x6B97B0
        }
    }

    // ---- 0x6B97C9: moto abandonada o destruida: se frena hasta quedar quieta ----
    bool bStopped = false;                                                      // [esp+5Fh]
    if (!m_bIsStuck && (GetStatus() == STATUS_ABANDONED || GetStatus() == STATUS_WRECKED) && !bikeFlags.bGettingPickedUp) {
        bool bMoving = false;                                                   // [esp+27h]
        if (!vehicleFlags.bVehicleColProcessed && m_vecMoveSpeed.x == 0.0f && m_vecMoveSpeed.y == 0.0f && m_vecMoveSpeed.z == 0.0f
            && std::bit_cast<uint32>(m_aRatioHistory[3]) != 0x3F800000u) {      // [72Ch] contra 1.0f (cmp eax, 3F800000h)
            bMoving = true;
        }
        float forceLimit, torqueLimit, speedLimit;                              // [esp+10h], [esp+4Ch], [esp+78h]
        if (GetStatus() == STATUS_WRECKED) {
            forceLimit = std::bit_cast<float>(0x3BC49BA6u);
            torqueLimit = std::bit_cast<float>(0x3AC49BA6u);
            speedLimit = std::bit_cast<float>(0x3C75C28Fu);
        } else {
            forceLimit = std::bit_cast<float>(0x3B449BA6u);
            torqueLimit = std::bit_cast<float>(0x3A6BEDFBu);
            speedLimit = std::bit_cast<float>(0x3BA3D70Au);
        }
        {   // 0x40FE30 (a + b) y 0x4119D0 (v * (1.0f / 2.0f))
            const float half = 1.0f / 2.0f;
            const CVector sumF{m_vecForce.x + m_vecMoveSpeed.x, m_vecForce.y + m_vecMoveSpeed.y, m_vecForce.z + m_vecMoveSpeed.z};
            m_vecForce = CVector{sumF.x * half, sumF.y * half, sumF.z * half};
            const CVector sumT{m_vecTorque.x + m_vecTurnSpeed.x, m_vecTorque.y + m_vecTurnSpeed.y, m_vecTorque.z + m_vecTurnSpeed.z};
            m_vecTorque = CVector{sumT.x * half, sumT.y * half, sumT.z * half};
        }
        forceLimit = forceLimit * CTimer::GetTimeStep();
        const bool forceSettled = BikeSqMagExe(m_vecForce) <= forceLimit * forceLimit;
        bool torqueSettled = false, speedSettled = false;
        if (forceSettled) {
            torqueLimit = torqueLimit * CTimer::GetTimeStep();
            torqueSettled = BikeSqMagExe(m_vecTorque) <= torqueLimit * torqueLimit;
            if (torqueSettled) {
                speedSettled = m_fMovingSpeed < speedLimit;
            }
        }
        if (!(forceSettled && torqueSettled && speedSettled) && !bMoving) {
            m_nFakePhysics = 0;                                                 // 0x6B9A3D
        } else {
            m_nFakePhysics = uint8(m_nFakePhysics + 1);
            if (m_nFakePhysics > 10 || bMoving) {
                const CVector& position = GetPosition();
                if (!Native_00424100(position.x, position.y)) {                 // 0x6B99AD: siempre false
                    if (!bMoving || m_nFakePhysics > 10) {
                        m_nFakePhysics = 10;
                    }
                    m_vecMoveSpeed = CVector{0.0f, 0.0f, 0.0f};
                    m_vecTurnSpeed = CVector{0.0f, 0.0f, 0.0f};
                    bStopped = true;
                }
            }
        }
    }

    // ---- 0x6B9A44: objetos sobre los que reposa la moto ----
    for (size_t i = 0; i < m_aGroundPhysicalPtrs.size(); ++i) {
        if (m_aGroundPhysicalPtrs[i]) {
            vehicleFlags.bRestingOnPhysical = true;                             // [42Ch] |= 2
            if (StaticRef<uint8>(0xB7CD6E) && m_aGroundPhysicalPtrs[i]->m_bIsInSafePosition) {   // [obj+1Ch] >> 5 & 1
                m_bWasPostponed = true;                                         // [1Ch] |= 40h
                return;                                                         // 0x6B9BD6 -> 0x6BC914
            }
        }
    }
    if (vehicleFlags.bRestingOnPhysical) {                                      // test [42Ch],2
        bStopped = false;
        m_nFakePhysics = 0;
    }

    // ---- 0x6B9A95: dano por defecto (virtual 0xE0) ----
    VehicleDamage(0.0f, static_cast<eVehicleCollisionComponent>(0), nullptr, nullptr, nullptr, static_cast<eWeaponType>(0x31));

    // 0x6B9AAB..0x6B9B28: ¿la moto esta sujeta en vertical (golpe lateral lento o recogida por una grua)?
    bool bHeldUpright = false;                                                  // [esp+27h]
    {
        const CMatrix& matrix = *m_matrix;
        if (m_fDamageIntensity > 0.0f
            && std::abs((m_vecLastCollisionImpactVelocity.z * matrix.GetRight().z + m_vecLastCollisionImpactVelocity.y * matrix.GetRight().y)
                        + m_vecLastCollisionImpactVelocity.x * matrix.GetRight().x) > 0.5f
            && BikeSqMagExe(m_vecMoveSpeed) < StaticRef<float>(0x858B1C)) {
            bHeldUpright = true;
        } else if (bikeFlags.bGettingPickedUp) {
            bHeldUpright = true;
        }
    }

    if (bStopped) {
        // ---- 0x6B9B39: la moto se ha parado ----
        SkipPhysics();                                                          // 0x5433B0
        vehicleFlags.bVehicleColProcessed = false;                              // [42Bh] &= DEh
        vehicleFlags.bAudioChangingGear = false;
        const bool onSideStand = bikeFlags.bOnSideStand;
        if (onSideStand && m_RideAnimData.BarSteerAngle < StaticRef<float>(0x8631B4)) {
            m_RideAnimData.BarSteerAngle = CTimer::GetTimeStep() * StaticRef<float>(0x8595EC) + m_RideAnimData.BarSteerAngle;
        }
        if (onSideStand) {
            const float keep = CnvDecayPowCpp(0.97f, CTimer::GetTimeStep());    // pow(0.97 (qword 0x866FD0), paso) guardado como float
            const float rightZ = m_matrix->GetRight().z;
            const float clamped = rightZ > 1.0f ? 1.0f : (-1.0f > rightZ ? -1.0f : rightZ);
            const float lean = BikeStoppedLeanCpp(keep, m_RideAnimData.DesiredLeanAngle, clamped);
            m_RideAnimData.DesiredLeanAngle = lean;
            m_RideAnimData.LeanAngle = lean;
        }
    } else {
        // ---- 0x6B9C3A: movimiento normal ----
        if (!vehicleFlags.bVehicleColProcessed) {
            ProcessControlCollisionCheck(true);                                     // call [vtbl+5Ch](1)
        }

        // 0x6B9C4C..0x6B9CB5: el manillar vuelve hacia el lado de la inclinacion cuando no hay conductor controlando
        if (!(extraHandlingFlags & 2) && !(nBikeFlags & 0x18)) {
            if (m_matrix->GetRight().z < 0.0f) {
                if (m_fSteerAngle > StaticRef<float>(0x8714A4)) {
                    m_fSteerAngle = m_fSteerAngle - CTimer::GetTimeStep() * StaticRef<float>(0x8631D4);
                }
            } else if (m_fSteerAngle < StaticRef<float>(0x8714A0)) {
                m_fSteerAngle = CTimer::GetTimeStep() * StaticRef<float>(0x8631D4) + m_fSteerAngle;
            }
        }

        // 0x6B9CBB..0x6B9DC7: resistencia del aire extra en la animacion de "tumbarse" del conductor
        const float savedAirResistance = m_fAirResistance;                          // [esp+64h]
        if (GetStatus() == STATUS_PLAYER && m_pDriver) {
            if (auto* const assoc = RpAnimBlendClumpGetAssociation(reinterpret_cast<RpClump*>(m_pDriver->GetRwObject()), uint32(0xC7))) {
                if (assoc->m_BlendAmount > 0.5f && assoc->m_CurrentTime > StaticRef<float>(0x871294) && assoc->m_CurrentTime < StaticRef<float>(0x871298)) {
                    const bool extraResistance = CCullZones::DoExtraAirResistanceForPlayer();
                    m_fAirResistance = m_fAirResistance * (extraResistance ? StaticRef<float>(0x862E1C) : StaticRef<float>(0x858CC8));
                    if (m_GasPedal > 0.5f) {
                        const CMatrix& matrix = *m_matrix;
                        if (BikeDotZYXExe(m_vecMoveSpeed, matrix.GetForward()) > StaticRef<float>(0x858C84)) {
                            const float scale = (CTimer::GetTimeStep() * m_fMass) * StaticRef<float>(0x87149C);
                            const CVector push{scale * matrix.GetForward().x, scale * matrix.GetForward().y, scale * matrix.GetForward().z};
                            ApplyMoveForce(push);                                   // 0x5429F0
                            bikeFlags.bPlayerBoost = true;                          // [614h] |= 20h
                        }
                    }
                }
            }
        }

        // ---- 0x6B9DCE: fisica base, flotabilidad y aliento del jugador ----
        const bool wasSubmerged = physicalFlags.bSubmergedInWater;                  // edi
        CPhysical::ProcessControl();                                                // 0x5485E0
        m_fAirResistance = savedAirResistance;
        ProcessBuoyancy();                                                          // 0x6B5FB0
        if (!wasSubmerged && physicalFlags.bSubmergedInWater) {
            if (m_pDriver && m_pDriver->IsPlayer()) {
                m_pDriver->AsPlayer()->ResetPlayerBreath();                         // 0x60A8A0
            } else {
                for (int32 i = 0; i < int32(m_nMaxPassengers); ++i) {                  // [488h] (m_nMaxPassengers), releido en cada vuelta
                    if (m_apPassengers[i] && m_apPassengers[i]->IsPlayer()) {
                        m_apPassengers[i]->AsPlayer()->ResetPlayerBreath();
                    }
                }
            }
        }

        // ---- 0x6B9E55: ratios de las ruedas respecto al reposo de la suspension ----
        for (size_t i = 0; i < 4; ++i) {
            const float resting = 1.0f - m_fSuspensionLength[i] / m_fLineLength[i];
            m_aWheelRatios[i] = (m_aWheelRatios[i] - resting) / (1.0f - resting);
        }

        // ---- 0x6B9E86: puntos de contacto y direccion de las lineas de suspension ----
        CVector lineDirWorld[4]{};                                                  // [esp+C0h]
        CVector contactOffset[4]{};                                                 // [esp+F0h]
        CVector contactSpeed[4]{};                                                  // [esp+120h]
        float   dampLimit[4]{};                                                     // [esp+150h]
        for (size_t i = 0; i < 4; ++i) {
            if (m_aWheelRatios[i] < 1.0f) {
                const CVector& position = GetPosition();
                const CVector& point = m_aWheelColPoints[i].m_vecPoint;
                contactOffset[i] = CVector{point.x - position.x, point.y - position.y, point.z - position.z};
                const CColLine& line = colData->m_pLines[i];
                const CVector lineVector{line.m_vecEnd.x - line.m_vecStart.x, line.m_vecEnd.y - line.m_vecStart.y, line.m_vecEnd.z - line.m_vecStart.z};
                lineDirWorld[i] = BikeMultiply3x3Exe(*m_matrix, lineVector);
                lineDirWorld[i].Normalise();                                        // 0x59C910
            }
        }

        // ---- 0x6B9F78: fuerzas de los muelles de la suspension ----
        m_aWheelSkidmarkType[0] = m_aWheelSkidmarkType[1] = static_cast<eSkidmarkType>(0);   // [748h] = [744h] = 0
        m_bMoreSkidMarks[0] = m_bMoreSkidMarks[1] = false;                                   // [74Fh] = [74Eh] = 0
        for (size_t i = 0; i < 4; ++i) {
            if (m_aWheelRatios[i] < 1.0f) {
                float springBias = m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;     // [handling+C0h]
                if (i >= 2) {
                    springBias = 1.0f - springBias;
                }
                CColPoint& colPoint = m_aWheelColPoints[i];
                if (colPoint.m_vecNormal.z > StaticRef<float>(0x858F9C)) {
                    ApplySpringCollisionAlt(m_pHandlingData->m_fSuspensionForceLevel, lineDirWorld[i], contactOffset[i], m_aWheelRatios[i], springBias, colPoint.m_vecNormal, dampLimit[i]);   // 0x6BA03A
                } else {
                    ApplySpringCollision(m_pHandlingData->m_fSuspensionForceLevel, lineDirWorld[i], contactOffset[i], m_aWheelRatios[i], springBias, dampLimit[i]);                      // 0x6BA05C
                }
                const size_t axle = i >= 2 ? 1 : 0;
                const uint32 skidmark = g_surfaceInfos.GetSkidmarkType(uint32(colPoint.m_nSurfaceTypeB));   // 0x6BA077
                m_aWheelSkidmarkType[axle] = static_cast<eSkidmarkType>(skidmark);
                if (skidmark == 2) {
                    m_bMoreSkidMarks[axle] = true;
                }
            } else {
                contactOffset[i] = BikeMultiply3x3Exe(*m_matrix, colData->m_pLines[i].m_vecEnd);   // 0x6BA0A7: matriz * extremo de la linea
            }
        }

        // Velocidad del punto de contacto menos la del objeto sobre el que apoya (0x6BA120 y 0x6BA521, mismo codigo)
        const auto computeContactSpeeds = [&]() {
            for (size_t i = 0; i < 4; ++i) {
                contactSpeed[i] = GetSpeed(contactOffset[i]);                       // 0x542CE0
                if (m_aGroundPhysicalPtrs[i]) {
                    const CVector groundSpeed = m_aGroundPhysicalPtrs[i]->GetSpeed(m_aGroundOffsets[i]);
                    contactSpeed[i].x = contactSpeed[i].x - groundSpeed.x;
                    contactSpeed[i].y = contactSpeed[i].y - groundSpeed.y;
                    contactSpeed[i].z = contactSpeed[i].z - groundSpeed.z;
                }
            }
        };
        computeContactSpeeds();

        // 0x6BA1E0..0x6BA49A: las direcciones de las lineas con normal casi vertical pasan a ser la normal invertida
        const auto flipLineDirection = [&](size_t wheel, size_t normalOwner) {
            const CVector& normal = m_aWheelColPoints[normalOwner].m_vecNormal;
            if (normal.z > StaticRef<float>(0x858F9C)) {
                lineDirWorld[wheel] = CVector{-normal.x, -normal.y, -normal.z};
            }
        };
        if (m_aWheelRatios[0] < 1.0f || m_aWheelRatios[1] < 1.0f) {
            flipLineDirection(0, !(m_aWheelRatios[0] < 1.0f) ? 1 : 0);
            flipLineDirection(1, !(m_aWheelRatios[1] < 1.0f) ? 0 : 1);
        }
        if (m_aWheelRatios[2] < 1.0f || m_aWheelRatios[3] < 1.0f) {
            flipLineDirection(2, !(m_aWheelRatios[2] < 1.0f) ? 3 : 2);
            flipLineDirection(3, !(m_aWheelRatios[3] < 1.0f) ? 2 : 3);
        }

        // 0x6BA4B0: amortiguacion
        for (size_t i = 0; i < 4; ++i) {
            if (m_aWheelRatios[i] < 1.0f) {
                ApplySpringDampening(m_pHandlingData->m_fSuspensionDampingLevel, dampLimit[i], lineDirWorld[i], contactOffset[i], contactSpeed[i]);   // 0x6BA4EA
            }
        }
        computeContactSpeeds();

        // ---- 0x6BA5E1: aceleracion del motor y reparto de frenos / traccion ----
        float forwardSpeed = (m_vecMoveSpeed.z * m_matrix->GetForward().z + m_vecMoveSpeed.y * m_matrix->GetForward().y)
                           + m_vecMoveSpeed.x * m_matrix->GetForward().x;             // [esp+80h]; la transmision lo recibe por referencia
        float driveAccel = m_pHandlingData->GetTransmission().CalculateDriveAcceleration(
            m_GasPedal, m_nCurrentGear, m_fGearChangeCount, forwardSpeed, nullptr, nullptr, m_NumDriveWheelsOnGround, StaticRef<uint8>(0x96914C));   // 0x6BA63D
        driveAccel = driveAccel / m_fVelocityFrequency;                               // [esp+84h]
        float brakeForce = (m_pHandlingData->m_fBrakeDeceleration * m_BrakePedal) * CTimer::GetTimeStep();   // [esp+48h]; 0x6BB2FD puede ponerla a cero
        float frontBias, rearBias, tractionBias, tractionBiasInv;                     // [esp+10h], [esp+64h], [esp+34h], [esp+8Ch]
        if (GetStatus() != STATUS_PLAYER && GetStatus() != STATUS_REMOTE_CONTROLLED && m_pHandlingData->m_bNpcNeutralHandl) {
            frontBias = rearBias = tractionBias = tractionBiasInv = 1.0f;
        } else {
            frontBias = m_pHandlingData->m_fBrakeBias + m_pHandlingData->m_fBrakeBias;
            rearBias = 1.0f - m_pHandlingData->m_fBrakeBias;
            rearBias = rearBias + rearBias;
            tractionBias = m_pHandlingData->m_fTractionBias + m_pHandlingData->m_fTractionBias;
            tractionBiasInv = StaticRef<float>(0x858CA0) - tractionBias;
        }

        // ---- 0x6BA6E2: ruedas en contacto, contadores y normal media del suelo ----
        m_NumDriveWheelsOnGroundLastFrame = m_NumDriveWheelsOnGround;
        m_nNoOfContactWheels = 0;
        m_NumDriveWheelsOnGround = 0;
        for (uint16 i = 0; i < 4; ++i) {
            bool inContact;
            if (m_aWheelRatios[i] < 1.0f) {
                m_WheelCounts[i] = 4.0f;
                inContact = true;
            } else {
                float count = m_WheelCounts[i] - CTimer::GetTimeStep();
                if (!(count > 0.0f)) {
                    count = 0.0f;
                }
                m_WheelCounts[i] = count;
                inContact = count > 0.0f;
            }
            if (inContact) {
                ++m_nNoOfContactWheels;
                if (i == 2 || i == 3) {
                    m_NumDriveWheelsOnGround = 1;
                }
                const CVector& normal = m_aWheelColPoints[i].m_vecNormal;
                if (m_nNoOfContactWheels == 1) {
                    m_vecAveGroundNormal = normal;
                } else {
                    m_vecAveGroundNormal.x = m_vecAveGroundNormal.x + normal.x;
                    m_vecAveGroundNormal.y = normal.y + m_vecAveGroundNormal.y;
                    m_vecAveGroundNormal.z = normal.z + m_vecAveGroundNormal.z;
                }
            }
        }
        if (m_nNoOfContactWheels > 0) {
            const float inverse = 1.0f / float(m_nNoOfContactWheels);
            m_vecAveGroundNormal.x = inverse * m_vecAveGroundNormal.x;
            m_vecAveGroundNormal.y = inverse * m_vecAveGroundNormal.y;
            m_vecAveGroundNormal.z = inverse * m_vecAveGroundNormal.z;
            const CMatrix& matrix = *m_matrix;
            if ((matrix.GetUp().z * m_vecAveGroundNormal.z + matrix.GetUp().y * m_vecAveGroundNormal.y) + matrix.GetUp().x * m_vecAveGroundNormal.x < StaticRef<float>(0x858F40)) {
                m_vecAveGroundNormal.x = m_vecAveGroundNormal.x * -1.0f;
                m_vecAveGroundNormal.y = m_vecAveGroundNormal.y * -1.0f;
                m_vecAveGroundNormal.z = m_vecAveGroundNormal.z * -1.0f;
            }
        } else {
            m_vecAveGroundNormal = CVector{0.0f, 0.0f, 1.0f};
        }

        // ---- 0x6BA8AE: puntos de apoyo de las ruedas en el espacio del vehiculo (rueda delantera y trasera con mejor contacto) ----
        const size_t frontIdx = !(m_aWheelRatios[0] < m_aWheelRatios[1]) ? 1 : 0;      // [esp+4Ch]
        CVector frontPoint{0.0f, colData->m_pLines[0].m_vecStart.y,
                           (colData->m_pLines[0].m_vecStart.z - m_aWheelRatios[frontIdx] * m_fSuspensionLength[0]) - modelInfo->m_fWheelSizeFront * 0.5f};   // [esp+90h]
        frontPoint = BikeMultiply3x3Exe(*m_matrix, frontPoint);                         // 0x6BA921
        const size_t rearIdx = (m_aWheelRatios[2] < m_aWheelRatios[3]) ? 2 : 3;         // ebx
        CVector rearPoint{0.0f, colData->m_pLines[3].m_vecStart.y,
                          (colData->m_pLines[2].m_vecStart.z - m_aWheelRatios[rearIdx] * m_fSuspensionLength[2]) - modelInfo->m_fWheelSizeRear * 0.5f};      // [esp+9Ch]
        rearPoint = BikeMultiply3x3Exe(*m_matrix, rearPoint);                           // 0x6BA9AB
        const float tractionTerm = ((m_pHandlingData->m_fTractionMultiplier * m_fExtraTractionMult) * StaticRef<float>(0xC1C818)) * StaticRef<float>(0x858C84);   // [esp+38h]

        // ---- 0x6BA9F7: angulo del manillar ----
        if (GetStatus() != STATUS_PLAYER && bikeFlags.bOnSideStand && !bikeFlags.bGettingPickedUp) {
            if (m_RideAnimData.BarSteerAngle < StaticRef<float>(0x8631B4)) {
                m_RideAnimData.BarSteerAngle = CTimer::GetTimeStep() * StaticRef<float>(0x8631A8) + m_RideAnimData.BarSteerAngle;
            }
        } else if (std::abs(m_vecMoveSpeed.x) < StaticRef<float>(0x858C58) && std::abs(m_vecMoveSpeed.y) < StaticRef<float>(0x858C58) && m_fSteerAngle == 0.0f) {
            float steerBar = m_RideAnimData.BarSteerAngle;
            PowScaleFloatCpp(&steerBar, 0.96f, CTimer::GetTimeStep());                  // pow(0.96 (qword 0x86F0E0), paso) * barra, sin redondear el pow
            m_RideAnimData.BarSteerAngle = steerBar;
        } else {
            bool useUnit = false;
            if (!(forwardSpeed > StaticRef<float>(0x858C58))) {
                useUnit = true;
            } else if (!(m_WheelCounts[0] > 0.0f || m_WheelCounts[1] > 0.0f)) {
                useUnit = true;
            } else if (GetStatus() != STATUS_PLAYER) {
                useUnit = true;
            }
            if (useUnit) {
                m_RideAnimData.BarSteerAngle = 1.0f * m_fSteerAngle;
            } else {
                // El EXE pasa un CColPoint de la pila sin inicializar: solo se escriben los tipos de superficie A (0x3C) y B (1).
                CColPoint scratch;
                scratch.m_nSurfaceTypeA = static_cast<eSurfaceType>(0x3C);
                scratch.m_nSurfaceTypeB = static_cast<eSurfaceType>(1);
                float grip = ((g_surfaceInfos.GetAdhesiveLimit(&scratch) * m_BikeHandling->m_fSpeedSteer) * tractionTerm) * StaticRef<float>(0x858B90);   // [esp+14h]
                const uint32 rearSurface = uint32(m_aWheelColPoints[rearIdx].m_nSurfaceTypeB);
                if (int32(g_surfaceInfos.GetAdhesionGroup(rearSurface)) == 3 || int32(g_surfaceInfos.GetAdhesionGroup(rearSurface)) == 4) {
                    grip = grip * m_BikeHandling->m_fSlipSteer;
                }
                float ratio = grip / (forwardSpeed * forwardSpeed);
                if (1.0f < ratio) {
                    ratio = 1.0f;                                                       // [esp+78h]
                }
                const bool doubleIt = (m_fSteerAngle < 0.0f && m_RideAnimData.LeanAngle < 0.0f) || (m_fSteerAngle > 0.0f && m_RideAnimData.LeanAngle > 0.0f);
                m_RideAnimData.BarSteerAngle = BikeSteerBarCpp(ratio, m_pHandlingData->m_fSteeringLock, m_fSteerAngle, doubleIt);   // asin / (steeringLock * pi/180)
            }
        }

        // ---- 0x6BAC22: ruedas ----
        const CVector moveSnapshot = m_vecMoveSpeed;                                // [esp+50h]
        const bool procRearFirst = m_pHandlingData->m_bProcRearwheelFirst;          // bit 23 de los flags de handling -> [esp+8Bh]
        tWheelState& frontState = StaticRef<tWheelState>(0xC1C26C);
        tWheelState& rearState = StaticRef<tWheelState>(0xC1C270);
        float& wheelThrust = StaticRef<float>(0xC1C27C);

        // 0x6BADC3 / 0x6BB34F: el deslizamiento por frenada depende del tipo de superficie
        const auto brakingSlideScale = [&](uint32 surface) {
            float scale = 1.0f;
            if (m_fBrakingSlide > 0.0f) {
                switch (int32(g_surfaceInfos.GetAdhesionGroup(surface))) {
                case 1:
                case 3:
                    scale = StaticRef<float>(0x8712F8);
                    break;
                case 2:
                    scale = StaticRef<float>(0x8712F4);
                    break;
                }
            }
            return scale;
        };
        // Proyecta la direccion de la rueda sobre el plano del suelo, la normaliza y saca el eje lateral (0x6BAD02..0x6BAD7C y equivalentes)
        const auto groundAxes = [&](CVector& wheelForward, CVector& wheelRight, const CVector& normal) {
            const float along = (wheelForward.y * normal.y + wheelForward.z * normal.z) + wheelForward.x * normal.x;
            wheelForward.x = wheelForward.x - along * normal.x;
            wheelForward.y = wheelForward.y - along * normal.y;
            wheelForward.z = wheelForward.z - along * normal.z;
            wheelForward.Normalise();                                               // 0x59C910
            wheelRight = BikeCrossExe(wheelForward, normal);                        // 0x59C730
            wheelRight.Normalise();
        };
        // Velocidad del punto de contacto de la rueda menos la del suelo (0x6BAE6D.. / 0x6BB3E9..)
        const auto wheelContactSpeed = [&](const CVector& point, size_t idx) {
            CVector speed = GetSpeed(point);
            if (m_aGroundPhysicalPtrs[idx]) {
                const CVector groundSpeed = m_aGroundPhysicalPtrs[idx]->GetSpeed(m_aGroundOffsets[idx]);
                speed.x = speed.x - groundSpeed.x;
                speed.y = speed.y - groundSpeed.y;
                speed.z = speed.z - groundSpeed.z;
            }
            return speed;
        };
        const auto clearWheelStateIfRequested = [&](tWheelState& state) {
            if ((extraHandlingFlags & 4) && (int32(state) == 1 || int32(state) == 2)) {
                state = static_cast<tWheelState>(0);
            }
        };

        // Rueda delantera. afterRear distingue las dos copias del EXE: 0x6BAC51 (antes de la trasera) y 0x6BB575 (despues, con procRearFirst).
        const auto processFrontWheel = [&](bool afterRear) {
            if (!(m_WheelCounts[0] > 0.0f) && !(m_WheelCounts[1] > 0.0f)) {
                m_aWheelAngularVelocity[0] = m_aWheelAngularVelocity[0] * StaticRef<float>(0x858EF0);
                if (afterRear) {
                    m_aWheelPitchAngles[0] = (m_aWheelAngularVelocity[0] * CTimer::GetTimeStep()) + m_aWheelPitchAngles[0];
                } else {
                    m_aWheelPitchAngles[0] = m_aWheelAngularVelocity[0] + m_aWheelPitchAngles[0];
                }
                return;
            }
            CVector steerDirection{-TrigSimpleSinCpp(m_RideAnimData.BarSteerAngle), TrigSimpleCosCpp(m_RideAnimData.BarSteerAngle), 0.0f};
            CVector wheelForward = BikeMultiply3x3Exe(*m_matrix, steerDirection);
            CColPoint& colPoint = m_aWheelColPoints[frontIdx];
            CVector wheelRight;
            groundAxes(wheelForward, wheelRight, colPoint.m_vecNormal);
            if (!afterRear && bHeldUpright) {
                wheelRight.z = 0.0f;
            }
            wheelThrust = 0.0f;
            colPoint.m_nSurfaceTypeA = static_cast<eSurfaceType>(0x3C);
            float grip = g_surfaceInfos.GetAdhesiveLimit(&colPoint) * tractionTerm;          // [esp+14h]
            const float slide = brakingSlideScale(uint32(colPoint.m_nSurfaceTypeB));
            if (GetStatus() == STATUS_PLAYER) {
                grip = g_surfaceInfos.GetWetMultiplier(uint32(colPoint.m_nSurfaceTypeB)) * grip;
            }
            if (m_nWheelStatus[0] == 1) {
                grip = grip * StaticRef<float>(0x87128C);
            }
            frontState = m_WheelStates[0];
            CVector contactSpeedFront = wheelContactSpeed(frontPoint, frontIdx);
            // ProcessBikeWheel reenvia al EXE (0x6D73B0) con adhesion y destabTraction intercambiados: el 8o argumento del EXE es grip * traccion y el 9o el deslizamiento.
            ProcessBikeWheel(wheelForward, wheelRight, contactSpeedFront, frontPoint, 2, wheelThrust, frontBias * brakeForce,
                             /*adhesion (9o arg. del EXE)*/ slide, /*destabTraction (8o arg. del EXE)*/ grip * tractionBias,
                             0, &m_aWheelAngularVelocity[0], &frontState, 0, uint16(m_nWheelStatus[0]));
            clearWheelStateIfRequested(frontState);
        };

        if (!procRearFirst) {
            processFrontWheel(false);
        }

        // ---- 0x6BAF62: rueda trasera ----
        if (m_WheelCounts[2] > 0.0f || m_WheelCounts[3] > 0.0f) {
            float rearBrake = brakeForce;                                           // [esp+78h]
            float rearTraction = tractionTerm;                                      // [esp+74h]
            CVector wheelForward = m_matrix->GetForward();
            CColPoint& colPoint = m_aWheelColPoints[rearIdx];
            CVector wheelRight;
            groundAxes(wheelForward, wheelRight, colPoint.m_vecNormal);
            if (bHeldUpright) {
                wheelRight.z = 0.0f;
            }
            const auto applyTyreSlideForce = [&](float strength) {
                const CVector& right = m_matrix->GetRight();
                const float forceZ = strength * right.z;
                const float forceY = strength * right.y;
                const float forceX = strength * right.x;
                // El EXE (0x6BB1D1 / 0x6BB2B3) rellena el primer argumento (fuerza) con el desplazamiento de contacto [2] y el segundo (punto) con este vector: argumentos invertidos en el original.
                ApplyTurnForce(contactOffset[2], CVector{forceX * CTimer::GetTimeStep(), forceY * CTimer::GetTimeStep(), forceZ * CTimer::GetTimeStep()});   // 0x6BB2D4
            };
            if (vehicleFlags.bIsHandbrakeOn) {
                rearBrake = std::bit_cast<float>(0x469C4000u);
                m_fTyreTemp = 1.0f;
            } else if (m_nBrakesOn != 0) {
                rearBrake = 0.0f;                                                   // 0x6BB165 / 0x6BB170: [esp+90h] = [esp+8Ch] = 0 (con 24 bytes de argumentos: [esp+78h] y [esp+74h])
                rearTraction = 0.0f;
                applyTyreSlideForce((m_fSteerAngle * m_fTurnMass) * StaticRef<float>(0x8712F0));
            } else if (m_fTyreTemp < 1.0f && m_GasPedal > StaticRef<float>(0x858F34)) {
                rearTraction = tractionTerm * m_fTyreTemp;
                applyTyreSlideForce((((1.0f - m_fTyreTemp) * m_fSteerAngle) * m_fTurnMass) * StaticRef<float>(0x8712F0));
            }
            if (wheelThrust > 0.0f && brakeForce > 0.0f) {                          // 0x6BB2D9: lee la global que dejo la delantera
                brakeForce = 0.0f;
            }
            wheelThrust = driveAccel;
            colPoint.m_nSurfaceTypeA = static_cast<eSurfaceType>(0x3C);
            float grip = g_surfaceInfos.GetAdhesiveLimit(&colPoint) * rearTraction;
            const float slide = brakingSlideScale(uint32(colPoint.m_nSurfaceTypeB));
            if (GetStatus() == STATUS_PLAYER) {
                grip = g_surfaceInfos.GetWetMultiplier(uint32(colPoint.m_nSurfaceTypeB)) * grip;
            }
            if (m_nWheelStatus[1] == 1) {
                grip = grip * StaticRef<float>(0x87128C);
            }
            rearState = m_WheelStates[1];
            CVector contactSpeedRear = wheelContactSpeed(rearPoint, rearIdx);
            ProcessBikeWheel(wheelForward, wheelRight, contactSpeedRear, rearPoint, 2, wheelThrust, rearBrake * rearBias,
                             /*adhesion (9o arg. del EXE)*/ slide, /*destabTraction (8o arg. del EXE)*/ grip * tractionBiasInv,
                             1, &m_aWheelAngularVelocity[1], &rearState, 1, uint16(m_nWheelStatus[1]));
            clearWheelStateIfRequested(rearState);
        } else {
            if (vehicleFlags.bIsHandbrakeOn) {
                m_aWheelAngularVelocity[1] = 0.0f;
            } else if (!(driveAccel == 0.0f)) {
                if (driveAccel > 0.0f) {
                    if (m_aWheelAngularVelocity[1] < 1.0f) {
                        m_aWheelAngularVelocity[1] = m_aWheelAngularVelocity[1] - StaticRef<float>(0x858B1C);
                    }
                } else if (m_aWheelAngularVelocity[1] > -1.0f) {
                    m_aWheelAngularVelocity[1] = m_aWheelAngularVelocity[1] + StaticRef<float>(0x858C28);
                }
            }
            m_aWheelPitchAngles[1] = CTimer::GetTimeStep() * m_aWheelAngularVelocity[1] + m_aWheelPitchAngles[1];
        }

        // 0x6BB4E6: temperatura del neumatico trasero
        if (m_nBrakesOn != 0 && int32(m_WheelStates[1]) == 1) {
            m_fTyreTemp = m_fTyreTemp - CTimer::GetTimeStep() * StaticRef<float>(0x858F44);
            if (m_fTyreTemp < 0.0f) {
                m_fTyreTemp = 0.0f;
            }
        } else if (m_fTyreTemp < 1.0f) {
            m_fTyreTemp = CTimer::GetTimeStep() * StaticRef<float>(0x858B4C) + m_fTyreTemp;
        }

        if (procRearFirst) {
            processFrontWheel(true);
        }

        // ---- 0x6BB87A: despues de las ruedas ----
        std::ranges::fill(m_aGroundPhysicalPtrs, nullptr);                          // [7C0h..7CCh] = 0
        float driverLeanOffset = 0.0f;                                              // [esp+10h]
        if (m_pDriver) {
            if (auto* const assoc = RpAnimBlendClumpGetAssociation(reinterpret_cast<RpClump*>(m_pDriver->GetRwObject()), uint32(0xC3))) {
                driverLeanOffset = assoc->m_BlendAmount * StaticRef<float>(0x871498);
            }
        }
        if (bHeldUpright) {
            m_vecAveGroundNormal = CVector{0.0f, 0.0f, 1.0f};
            CVector sideways = BikeCrossExe(m_matrix->GetForward(), m_vecAveGroundNormal);       // 0x6BB911
            sideways.Normalise();
            m_vecAveGroundNormal = BikeCrossExe(sideways, m_matrix->GetForward());               // 0x6BB936
            m_vecAveGroundNormal.Normalise();
        }

        // 0x6BB955: inclinacion deseada
        if (!(extraHandlingFlags & 2) && !bikeFlags.bGettingPickedUp) {
            float lean;
            if (bikeFlags.bOnSideStand) {
                const float keep = CnvDecayPowCpp(0.97f, CTimer::GetTimeStep());    // 0x6BB97A: pow(0.97 (qword 0x866FD0), paso) guardado como float
                lean = BikeSideStandLeanCpp(keep, m_RideAnimData.DesiredLeanAngle, m_matrix->GetRight().z, driverLeanOffset);
            } else {
                lean = m_RideAnimData.DesiredLeanAngle;
                PowScaleFloatCpp(&lean, 0.95f, CTimer::GetTimeStep());              // 0x6BB9D9: pow(0.95 (qword 0x86C3C8), paso) * inclinacion, sin redondear el pow
            }
            m_RideAnimData.DesiredLeanAngle = lean;
        } else {
            m_vecGroundRight = BikeCrossExe(m_matrix->GetForward(), m_vecAveGroundNormal);      // 0x6BB9FF
            m_vecGroundRight.Normalise();
            float lateralSpeed;                                                     // ST0 en 0x6BBAD6
            if (m_pAttachedTo) {
                lateralSpeed = 0.0f;
            } else if (m_nNoOfContactWheels == 0) {
                lateralSpeed = ((m_fSteerAngle / (m_pHandlingData->m_fSteeringLock * StaticRef<float>(0x8595EC))) * CTimer::GetTimeStep()) * StaticRef<float>(0x871494);
            } else {
                CVector delta;
                if (physicalFlags.bDisableCollisionForce) {
                    delta = CVector{moveSnapshot.x - m_vecOldSpeedForPlayback.x, moveSnapshot.y - m_vecOldSpeedForPlayback.y, moveSnapshot.z - m_vecOldSpeedForPlayback.z};
                    m_vecOldSpeedForPlayback = moveSnapshot;
                } else {
                    delta = CVector{m_vecMoveSpeed.x - moveSnapshot.x, m_vecMoveSpeed.y - moveSnapshot.y, m_vecMoveSpeed.z - moveSnapshot.z};
                }
                lateralSpeed = (delta.y * m_vecGroundRight.y + delta.z * m_vecGroundRight.z) + delta.x * m_vecGroundRight.x;
            }
            const float stepLimit = StaticRef<float>(0x858C58) > CTimer::GetTimeStep() ? StaticRef<float>(0x858C58) : CTimer::GetTimeStep();
            float wanted = lateralSpeed / (stepLimit * StaticRef<float>(0x863984));            // [esp+14h]
            const float leanLimit = m_nWheelStatus[0] == 1 ? StaticRef<float>(0x87128C) * m_BikeHandling->m_fMaxLean : m_BikeHandling->m_fMaxLean;
            if (wanted > leanLimit) {
                wanted = leanLimit;
            } else if (wanted < -leanLimit) {
                wanted = -leanLimit;
            }
            const float keep = CnvDecayPowCpp(m_BikeHandling->m_fDesLean, CTimer::GetTimeStep());
            m_RideAnimData.DesiredLeanAngle = BikeFreeLeanCpp(keep, m_RideAnimData.DesiredLeanAngle, wanted, driverLeanOffset);
        }
        m_RideAnimData.LeanAngle = m_RideAnimData.DesiredLeanAngle;                 // [648h] = [64Ch]

        // 0x6BBBB3: estados de rueda calculados arriba
        m_WheelStates[0] = frontState;
        m_WheelStates[1] = rearState;
        if (m_GasPedal < 0.0f && int32(m_WheelStates[1]) == 1) {
            m_WheelStates[1] = static_cast<tWheelState>(0);
        }
        if (GetStatus() == STATUS_PLAYER) {
            ProcessSirenAndHorn(true);                                              // 0x6E0950
        } else if (m_HornCounter != 0) {
            --m_HornCounter;
        }
    }

    // ---- 0x6BBC18: fuego y explosion ----
    if (m_fHealth < 250.0f && GetStatus() != STATUS_WRECKED) {
        if (m_nVehicleSubType != VEHICLE_TYPE_BMX) {                            // cmp [esi+594h], 0Ah (m_nVehicleSubType)
            RwMatrix* const frameMatrix = GetRwObject() ? reinterpret_cast<RwMatrix*>(reinterpret_cast<uint8*>(GetRwObject()->parent) + 0x10) : nullptr;
            if (!m_pFireParticle && frameMatrix) {
                const CVector position = modelInfo->GetModelDummyPosition(DUMMY_ENGINE);   // [vehicleStruct + 54h]
                m_pFireParticle = g_fxMan.CreateFxSystem("fire_bike", position, frameMatrix, false);   // 0x4A9BE0
                if (m_pFireParticle) {
                    m_pFireParticle->Play();                                    // 0x4AA2F0
                    CEventVehicleOnFire event{this};                            // 0x4B10C0
                    GetEventGlobalGroup()->Add(&event, false);                  // 0x4ABA50 + 0x4AB420; ~CEventVehicleOnFire = 0x4B1170 al salir (o al desenrollar)
                }
            }
        }
        const uint32 elapsed = uint32(int64(CTimer::GetTimeStep() * StaticRef<float>(0x858B38) * StaticRef<float>(0x858C4C)));   // _ftol2 (eax)
        m_BlowUpTimer = UnsignedPlusFloatCpp(int32(elapsed), m_BlowUpTimer);    // fild + 2^32 si es negativo; fadd [7BCh]; fst
        if (m_BlowUpTimer > StaticRef<float>(0x86CD78)) {                          // 5000.0f
            BlowUpCar(m_Damager, false);                                        // call [vtbl+0A4h]
        }
    } else {
        m_BlowUpTimer = 0.0f;
        if (m_pFireParticle) {
            m_pFireParticle->Kill();                                            // 0x4AA3F0
            m_pFireParticle = nullptr;
        }
    }

    ProcessDelayedExplosion();                                                  // 0x6BBD7A

    // ---- 0x6BBD7F: compresion de la suspension y rugosidad del suelo -> vibracion del mando ----
    {
        const float speedSquared = (m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z;   // [esp+60h]
        float maxCompression = 0.0f;                                            // [esp+14h]
        float roughness = 0.0f;                                                 // [esp+38h]
        for (size_t i = 0; i < 4; ++i) {
            const float compression = m_aRatioHistory[i] - m_aWheelRatios[i];   // [esp+10h]
            if (compression > StaticRef<float>(0x858C24) && (i == 0 || i == 2) && speedSquared > StaticRef<float>(0x863244)
                && (GetStatus() == STATUS_PLAYER || GetStatus() == STATUS_PHYSICS) && compression > maxCompression) {
                maxCompression = compression;
            }
            if (m_aWheelRatios[i] < 1.0f && GetStatus() == STATUS_PLAYER) {
                const float surfaceRoughness = float(int32(g_surfaceInfos.GetRoughness(uint32(m_aWheelColPoints[i].m_nSurfaceTypeB)))) * StaticRef<float>(0x858B1C);
                if (!(roughness > surfaceRoughness)) {
                    roughness = surfaceRoughness;
                }
            }
            m_aRatioHistory[i] = m_aWheelRatios[i];
            m_aWheelRatios[i] = StaticRef<float>(0x871284);                     // 1.0f
        }
        if ((CTimer::GetTimeInMS() & 0x7FF) > 0x320) {
            if (roughness >= StaticRef<float>(0x871268)) {                      // fcomp + test ah,1 + jne: salta con menor o NaN
                maxCompression = 0.0f;
            }
            roughness = 0.0f;
        }
        if ((maxCompression > 0.0f || roughness > 0.0f) && GetStatus() == STATUS_PLAYER) {
            const float squared = (m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z;
            if (squared > StaticRef<float>(0x86CD7C)) {                         // 0.01f
                const float speedPerMass = std::sqrt(squared) / m_fMass;
                if (maxCompression > 0.0f) {
                    float intensityValue = ((speedPerMass * maxCompression) * StaticRef<float>(0x871264)) + StaticRef<float>(0x858628);
                    if (StaticRef<float>(0x859F80) < intensityValue) {
                        intensityValue = StaticRef<float>(0x859F80);
                    }
                    const uint8 intensity = uint8(uint32(int64(intensityValue)));
                    const float duration = (CTimer::GetTimeStep() * StaticRef<float>(0x871260)) / float(int32(uint32(intensity)));
                    CPad::GetPad(0)->StartShake(int16(int64(duration)), intensity, 0);   // 0x53FB70 + 0x53F920
                } else {
                    float intensityValue = ((speedPerMass * roughness) * StaticRef<float>(0x871264)) + StaticRef<float>(0x858A10);
                    if (StaticRef<float>(0x858A28) < intensityValue) {
                        intensityValue = StaticRef<float>(0x858A28);
                    }
                    const uint8 intensity = uint8(uint32(int64(intensityValue)));
                    const float duration = (CTimer::GetTimeStep() * StaticRef<float>(0x86CD78)) / float(int32(uint32(intensity)));
                    CPad::GetPad(0)->StartShake(int16(int64(duration)), intensity, 0);
                }
            }
        }
    }

    // ---- 0x6BC006 ----
    vehicleFlags.bVehicleColProcessed = false;                                  // [42Bh] &= DEh
    vehicleFlags.bAudioChangingGear = false;
    if (!vehicleFlags.bWarnedPeds) {
        CCarCtrl::ScanForPedDanger(this);                                       // 0x42CE40
    }

    // 0x6BC028: congelar la moto cuando esta quieta
    if (physicalFlags.bDisableCollisionForce && physicalFlags.bCollidable) {
        m_vecMoveSpeed = CVector{0.0f, 0.0f, 0.0f};
        m_vecTurnSpeed = CVector{0.0f, 0.0f, 0.0f};
        m_vecFrictionMoveSpeed = CVector{0.0f, 0.0f, 0.0f};
        m_vecFrictionTurnSpeed = CVector{0.0f, 0.0f, 0.0f};
    } else if (!bStopped) {
        if ((m_GasPedal == 0.0f || GetStatus() == STATUS_WRECKED)
            && std::abs(m_vecMoveSpeed.x) < StaticRef<float>(0x858B4C) && std::abs(m_vecMoveSpeed.y) < StaticRef<float>(0x858B4C) && std::abs(m_vecMoveSpeed.z) < StaticRef<float>(0x858B4C)
            && (!(m_fDamageIntensity > 0.0f) || m_pDamageEntity != FindPlayerPed(-1))) {
            m_vecTurnSpeed.z = 0.0f;
            m_vecMoveSpeed = CVector{0.0f, 0.0f, 0.0f};
        }
    }

    // ---- 0x6BC192: fuerzas de equilibrado de la moto ----
    if (!(extraHandlingFlags & 2) && !(nBikeFlags & 0x18)) {
        return;
    }
    const CMatrix& matrix = *m_matrix;
    float lateral = (m_vecAveGroundNormal.z * matrix.GetRight().z + m_vecAveGroundNormal.y * matrix.GetRight().y) + matrix.GetRight().x * m_vecAveGroundNormal.x;   // [esp+38h]
    if (lateral > 1.0f) {
        lateral = 1.0f;
    } else if (lateral < -1.0f) {
        lateral = -1.0f;
    }
    const CVector comRotated = BikeMultiply3x3Exe(matrix, m_vecCentreOfMass);   // [esp+50h]
    {
        const CVector point{comRotated.x + matrix.GetRight().x, comRotated.y + matrix.GetRight().y, comRotated.z + matrix.GetRight().z};
        const float torque = (lateral * m_fTurnMass) * ((extraHandlingFlags & 2) ? StaticRef<float>(0x8712E8) : StaticRef<float>(0x8712EC));
        const float timeStep = CTimer::GetTimeStep();
        ApplyTurnForce(CVector{(torque * matrix.GetUp().x) * timeStep, (torque * matrix.GetUp().y) * timeStep, (torque * matrix.GetUp().z) * timeStep}, point);   // 0x6BC2C3 / 0x6BC348
        if (extraHandlingFlags & 2) {
            bikeFlags.bOnSideStand = false;                                     // [614h] &= EFh
        }
    }
    if (GetStatus() != STATUS_PLAYER) {
        return;
    }

    // Cargas de peso al levantar una rueda (caballito / "stoppie")
    const auto pitchForce = [&](float angleDifference, float stabilisation) {
        const float length = std::sqrt((m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z);
        const float speedTerm = StaticRef<float>(0x858B1C) < length ? StaticRef<float>(0x858B1C) : length;        // 0.1f
        const float basis = (angleDifference * stabilisation) * speedTerm;                                          // [esp+14h]
        const float modifier = CStats::GetFatAndMuscleModifier(STAT_MOD_12) * basis;
        const CVector point{comRotated.x + matrix.GetForward().x, comRotated.y + matrix.GetForward().y, comRotated.z + matrix.GetForward().z};
        const float strength = ((modifier * m_fTurnMass) * CTimer::GetTimeStep()) * StaticRef<float>(0x858B8C);
        ApplyTurnForce(CVector{strength * matrix.GetUp().x, strength * matrix.GetUp().y, strength * matrix.GetUp().z}, point);
        return point;
    };

    if (m_WheelCounts[0] <= 0.0f && m_WheelCounts[1] <= 0.0f && matrix.GetForward().z > 0.0f && (m_WheelCounts[2] > 0.0f || m_WheelCounts[3] > 0.0f)) {
        // 0x6BC3C6: caballito
        const float w = m_BikeHandling->m_fWheelieAng - matrix.GetForward().z;
        float angleDifference;
        if (w > StaticRef<float>(0x858FCC)) {
            const float shifted = StaticRef<float>(0x858C24) - w;
            angleDifference = (0.0f > shifted) ? 0.0f : shifted;
        } else {
            angleDifference = w;
            if (w < StaticRef<float>(0x871490)) {
                angleDifference = StaticRef<float>(0x87148C) - w;
                if (0.0f < angleDifference) {
                    angleDifference = 0.0f;
                }
            }
        }
        const CVector point = pitchForce(angleDifference, m_BikeHandling->m_fWheelieStabMult);
        {
            const float steerStrength = (((m_BikeHandling->m_fWheelieSteer * m_RideAnimData.BarSteerAngle) * m_fTurnMass) * CTimer::GetTimeStep()) * StaticRef<float>(0x858B8C);
            ApplyTurnForce(CVector{steerStrength * matrix.GetRight().x, steerStrength * matrix.GetRight().y, steerStrength * matrix.GetRight().z}, point);   // 0x6BC5A6
        }
        {
            const float length = std::sqrt((m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z);
            const float sideStrength = ((((length * m_fMass) * m_BikeHandling->m_fWheelieSteer) * m_RideAnimData.BarSteerAngle) * CTimer::GetTimeStep()) * StaticRef<float>(0x8D3288);
            const float forceY = sideStrength * matrix.GetRight().y;
            const float forceZ = sideStrength * matrix.GetRight().z;
            const float speed = std::sqrt((m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) + m_vecMoveSpeed.z * m_vecMoveSpeed.z);   // [esp+10h]
            ApplyMoveForce(CVector{(sideStrength * matrix.GetRight().x) * speed, forceY * speed, forceZ * speed});                                        // 0x6BC664
        }
        m_RideAnimData.LeanAngle = CTimer::GetTimeStep() * m_RideAnimData.BarSteerAngle * StaticRef<float>(0x8D3284) + m_RideAnimData.LeanAngle;
        return;
    }

    if (m_WheelCounts[2] <= 0.0f && m_WheelCounts[3] <= 0.0f && matrix.GetForward().z < 0.0f && (m_WheelCounts[0] > 0.0f || m_WheelCounts[1] > 0.0f)) {
        // 0x6BC6FB: "stoppie"
        const float w = m_BikeHandling->m_fStoppieAng - matrix.GetForward().z;
        float angleDifference;
        if (w > StaticRef<float>(0x858FCC)) {
            const float shifted = StaticRef<float>(0x858C24) - w;
            angleDifference = (0.0f > shifted) ? 0.0f : shifted;
        } else {
            angleDifference = w;
            if (w < StaticRef<float>(0x86C330)) {
                angleDifference = StaticRef<float>(0x858EE4) - w;
                if (0.0f < angleDifference) {
                    angleDifference = 0.0f;
                }
            }
        }
        pitchForce(angleDifference, m_BikeHandling->m_fStoppieStabMult);
        {
            const float alongRight = (m_vecMoveSpeed.z * matrix.GetRight().z + m_vecMoveSpeed.y * matrix.GetRight().y) + matrix.GetRight().x * m_vecMoveSpeed.x;
            const float strength = ((alongRight * m_fTurnMass) * CTimer::GetTimeStep()) * StaticRef<float>(0x8D3280);
            CVector axis = BikeCrossExe(CVector{0.0f, 0.0f, 1.0f}, matrix.GetRight());   // 0x6BC87F
            axis.Normalise();
            const float negated = -strength;
            ApplyTurnForce(CVector{negated * matrix.GetRight().x, negated * matrix.GetRight().y, negated * matrix.GetRight().z},
                           CVector{-axis.x, -axis.y, -axis.z});                 // 0x6BC90F
        }
    }
}


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
    const float oneMinusCos = BikeOneMinusCosCpp(leanAngle);
    m_mLeanMatrix.GetPosition().x += partX * oneMinusCos;
    m_mLeanMatrix.GetPosition().y += partY * oneMinusCos;
    m_mLeanMatrix.GetPosition().z += partZ * oneMinusCos;
    m_bLeanMatrixCalculated = true;
}

static void BikeMoveHandBone355(RwMatrix* bone, const CVector& delta, float factor) {
    BikeMoveHandBoneCpp(&bone->pos.x, &delta.x, factor);
}

// 0x6B7F90
void CBike::FixHandsToBars(CPed* rider) {
    if (!m_nFixRightHand && !m_nFixLeftHand) return;
    CMatrix handleWorld, handleLocal;
    if (!m_aBikeNodes[BIKE_CHASSIS]) return;
    handleLocal.Attach(&m_aBikeNodes[BIKE_CHASSIS]->modelling, false);
    handleWorld = *m_matrix;
    handleWorld *= handleLocal;
    auto* hierarchy = GetAnimHierarchyFromSkinClump(reinterpret_cast<RpClump*>(rider->GetRwObject()));
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
    BikeRoadOrientationCpp(reinterpret_cast<float*>(matrix), rear, front, backX, by, bz, fx, fy, fz);
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
    float mid[3];
    BikeRoadMidpointCpp(fx, backX, fy, by, fz, bz, mid);   // 0x6BF167..0x6BF1E0
    const float midX = mid[0], midY = mid[1], midZ = mid[2];
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
    return BikeChassisHeightCpp(lean, height);
}
void BikePreRenderContact355(CVector& out, const CVector& point, const CVector& right, float lean, float height) {
    BikeContactOffsetCpp(&out.x, &point.x, &right.x, lean, height);
}
void BikePreRenderTrig355(float angle, float multiplier, float& sineOut, float& cosineOut) {
    sineOut = BmxSinScaledCpp(angle, multiplier);      // fld angle; fmul k; fsin / fcos (producto de 24 bits sin guardar), como CBmx::PreRender
    cosineOut = BmxCosScaledCpp(angle, multiplier);
}
float BikePreRenderSine355(float angle, float factor) {
    return TrigSimpleSinMulCpp(angle, factor);
}
float BikePreRenderForkAngle355(float height, float original, float forkLength) {
    return BikeForkAngleCpp(height, original, forkLength);
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
    const float steerSine = TrigSimpleSinCpp(m_fSteerAngle);       // 0x6BD674: fld [esi+494h]; fsin (sin multiplicar)
    const float steerCosine = TrigSimpleCosCpp(m_fSteerAngle);
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
            BikeHealthMinusCpp(&m_fHealth, damage, factor);   // 0x6B91B0..0x6B91C8: fmul factor; fsubr [vida]; fstp
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
    line->m_vecEnd.z = BikeSetupEndCpp(upper, lower, wheel, line->m_vecStart.z);
}

static float BikeSetupHeight355(float force, float suspensionLength, float wheel, float startZ) {
    return BikeSetupHeightCpp(force, StaticRef<float>(0x858B90), suspensionLength, wheel, startZ);
}

// 0x6B89B0
static void BikeSetupSphere355(CColSphere* sphere,float limit) {
    BikeSetupSphereCpp(reinterpret_cast<float*>(sphere), limit);   // x, y, z, radio consecutivos
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
