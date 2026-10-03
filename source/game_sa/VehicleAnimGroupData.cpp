/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehicleAnimGroup
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: CVehicleAnimGroupData
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "VehicleAnimGroupData.h"

void CVehicleAnimGroup::InjectHooks() {
    RH_ScopedClass(CVehicleAnimGroup);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(InitAnimGroup, 0x5B9EB0);
    RH_ScopedInstall(CopyAnimGroup, 0x5B9E40);
    RH_ScopedInstall(GetGroup, 0x6E3B00);
    RH_ScopedInstall(ComputeCriticalBlendTime, 0x6E3C80);
    RH_ScopedInstall(ComputeAnimDoorOffsets, 0x6E3D10);
}

// 0x5BD420
CVehicleAnimGroup::CVehicleAnimGroup() : m_animFlags(), m_specialFlags(), m_generalTiming(), m_aInOutTiming() {
    m_ucFirstGroup = 0;
    m_ucSecondGroup = 0;

    for (auto i = 0; i < 5; ++i) {
        m_aVecDoorOffsets[i] = CVector(0.0F, 0.0F, 0.0F); //BUG? The 2 last entries in the array aren't initialized to 0
    }
}

void CVehicleAnimGroup::InitAnimGroup(
    uint8 firstGroup,
    uint8 secondGroup,
    int32 animFlags,
    int32 animSpecialFlags,
    sVehAnimGroupGeneralTiming* generalTiming,
    sVehAnimGroupInOutTiming* startTiming, sVehAnimGroupInOutTiming* endTiming
) {
    assert(firstGroup >= ANIM_GROUP_CARS_BEGIN && firstGroup < ANIM_GROUP_CARS_END);
    assert(secondGroup >= ANIM_GROUP_CARS_BEGIN && secondGroup < ANIM_GROUP_CARS_END);

    m_ucFirstGroup = firstGroup;
    m_ucSecondGroup = secondGroup;
    m_animFlags.intValue = animFlags;
    m_specialFlags.intValue = animSpecialFlags;
    m_generalTiming = *generalTiming;
    GetInOutTiming(eInOutTiming::TIMING_START) = *startTiming;
    GetInOutTiming(eInOutTiming::TIMING_END) = *endTiming;
}

void CVehicleAnimGroup::CopyAnimGroup(CVehicleAnimGroup* src) {
    m_ucFirstGroup = src->m_ucFirstGroup;
    m_ucSecondGroup = src->m_ucSecondGroup;
    m_animFlags = src->m_animFlags;
    m_specialFlags = src->m_specialFlags;
    m_generalTiming = src->m_generalTiming;
    GetInOutTiming(eInOutTiming::TIMING_START) = src->GetInOutTiming(eInOutTiming::TIMING_START);
    GetInOutTiming(eInOutTiming::TIMING_END) = src->GetInOutTiming(eInOutTiming::TIMING_END);
}

// 0x6E3B00
AssocGroupId CVehicleAnimGroup::GetGroup(AnimationId animId) const {
    int32 bit;
    switch (animId) {
    case ANIM_ID_CAR_ALIGN_LHS:
    case ANIM_ID_CAR_ALIGN_RHS:
    case ANIM_ID_CAR_ALIGNHI_LHS:
    case ANIM_ID_CAR_ALIGNHI_RHS:     bit = 0;  break; // bAlign
    case ANIM_ID_CAR_OPEN_LHS:
    case ANIM_ID_CAR_OPEN_RHS:        bit = 1;  break; // bOpenFrontDoorsOnExit
    case ANIM_ID_CAR_OPEN_LHS_1:
    case ANIM_ID_CAR_OPEN_RHS_1:      bit = 2;  break; // bOpenRearDoorsOnExit
    case ANIM_ID_CAR_GETIN_LHS_0:
    case ANIM_ID_CAR_GETIN_RHS_0:
    case ANIM_ID_CAR_GETIN_BIKE_FRONT: bit = 3; break; // bCanEnterFrontDoors
    case ANIM_ID_CAR_GETIN_LHS_1:
    case ANIM_ID_CAR_GETIN_RHS_1:     bit = 4;  break; // bCanEnterRearDoors
    case ANIM_ID_CAR_PULLOUT_LHS:
    case ANIM_ID_CAR_PULLOUT_RHS:
    case ANIM_ID_UNKNOWN_15:          bit = 5;  break; // bCanPulloutPed
    case ANIM_ID_CAR_CLOSEDOOR_LHS_0:
    case ANIM_ID_CAR_CLOSEDOOR_RHS_0: bit = 6;  break; // bCloseFrontDoorWhenInside
    case ANIM_ID_CAR_CLOSEDOOR_LHS_1:
    case ANIM_ID_CAR_CLOSEDOOR_RHS_1: bit = 7;  break; // bCloseRearDoorWhenInside
    case ANIM_ID_CAR_SHUFFLE_RHS_0:
    case ANIM_ID_CAR_SHUFFLE_RHS_1:   bit = 8;  break; // bShuffle
    case ANIM_ID_CAR_GETOUT_LHS_0:
    case ANIM_ID_CAR_GETOUT_RHS_0:    bit = 9;  break; // bCanExitFrontDoors
    case ANIM_ID_CAR_GETOUT_LHS_1:
    case ANIM_ID_CAR_GETOUT_RHS_1:    bit = 10; break; // bCanExitRearDoors
    case ANIM_ID_CAR_JACKEDLHS:
    case ANIM_ID_CAR_JACKEDRHS:       bit = 11; break; // bPlayerCanBeJacked
    case ANIM_ID_CAR_CLOSE_LHS_0:
    case ANIM_ID_CAR_CLOSE_RHS_0:     bit = 12; break; // bCloseFrontDoorWhenOutside
    case ANIM_ID_CAR_CLOSE_LHS_1:
    case ANIM_ID_CAR_CLOSE_RHS_1:     bit = 13; break; // bCloseRearDoorWhenOutside
    case ANIM_ID_CAR_ROLLOUT_LHS:
    case ANIM_ID_CAR_ROLLOUT_RHS:     bit = 14; break; // bCanJumpOutOfVehicle
    case ANIM_ID_CAR_ROLLDOOR:        bit = 15; break; // bRollDownWindowOnDoorClose
    case ANIM_ID_CAR_FALLOUT_LHS:
    case ANIM_ID_CAR_FALLOUT_RHS:     bit = 16; break; // bPlayerCanFallOut
    case ANIM_ID_CAR_DOORLOCKED_LHS:
    case ANIM_ID_CAR_DOORLOCKED_RHS:  bit = 17; break; // bDoorLocked
    default:                          bit = (int32)animId; break; // Como el original: el propio id como bit (p. ej. ANIM_ID_UNKNOWN_26)
    }

    // Como el original: máscara = _ftol2(pow(2.0, bit)); fuera de [0, 31] la parte baja es 0 (se usa el primer grupo).
    // El grupo se extiende con signo (movsx).
    const uint32 mask = bit >= 0 && bit < 32 ? 1u << bit : 0u;
    return (AssocGroupId)(int8)((m_animFlags.intValue & mask) ? m_ucSecondGroup : m_ucFirstGroup);
}

// 0x6E3C80
float CVehicleAnimGroup::ComputeCriticalBlendTime(AnimationId animId) {
    switch (animId) {
    case ANIM_ID_CAR_ALIGN_LHS:
    case ANIM_ID_CAR_ALIGN_RHS:
    case ANIM_ID_CAR_ALIGNHI_LHS:
    case ANIM_ID_CAR_ALIGNHI_RHS:
    case ANIM_ID_CAR_OPEN_LHS:
    case ANIM_ID_CAR_OPEN_RHS:
    case ANIM_ID_CAR_OPEN_LHS_1:
    case ANIM_ID_CAR_OPEN_RHS_1:
    case ANIM_ID_CAR_GETIN_LHS_0:
    case ANIM_ID_CAR_GETIN_RHS_0:
    case ANIM_ID_CAR_GETIN_LHS_1:
    case ANIM_ID_CAR_GETIN_RHS_1:
    case ANIM_ID_CAR_GETIN_BIKE_FRONT:
        return m_generalTiming.m_fGetInTime;

    case ANIM_ID_CAR_GETOUT_LHS_0:
    case ANIM_ID_CAR_GETOUT_RHS_0:
    case ANIM_ID_CAR_GETOUT_LHS_1:
    case ANIM_ID_CAR_GETOUT_RHS_1:
        return m_generalTiming.m_fGetOutTime;

    case ANIM_ID_CAR_JACKEDLHS:
    case ANIM_ID_CAR_JACKEDRHS:
        return m_generalTiming.m_fCarJackTime;

    case ANIM_ID_CAR_ROLLOUT_LHS:
    case ANIM_ID_CAR_ROLLOUT_RHS:
        return m_generalTiming.m_fJumpOutTime;

    case ANIM_ID_CAR_FALLOUT_LHS:
    case ANIM_ID_CAR_FALLOUT_RHS:
        return m_generalTiming.m_fFallOutTime;

    default:
        return 0.0F;
    }
}

CVector CVehicleAnimGroup::ComputeAnimDoorOffsets(eVehAnimDoorOffset doorId) {
    // Memoize door offset if it's not set yet
    // Como el original: x*x + y*y + z*z se compara con 0 en el x87 (exponente extendido, sin subdesbordamiento): solo
    // es 0 si las tres componentes son ±0; con NaN se devuelve el valor guardado.
    if (const auto& offset = GetDoorOffset(doorId); !(offset.x == 0.0F && offset.y == 0.0F && offset.z == 0.0F)) {
        return GetDoorOffset(doorId);
    }

    AnimationId animId = [&doorId] {
        switch (doorId) {
        case ENTER_FRONT:      return ANIM_ID_CAR_GETIN_LHS_0;
        case ENTER_REAR:       return ANIM_ID_CAR_GETIN_LHS_1;
        case ENTER_BIKE_FRONT: return ANIM_ID_CAR_GETIN_BIKE_FRONT;
        case EXIT_FRONT:       return ANIM_ID_CAR_GETOUT_LHS_0;
        case EXIT_REAR:        return ANIM_ID_CAR_GETOUT_LHS_1;
        case JACK_PED_LEFT:    return ANIM_ID_CAR_JACKEDLHS;
        case JACK_PED_RIGHT:   return ANIM_ID_CAR_JACKEDRHS;
        default:               return (AnimationId)doorId; // Como el original: el propio índice de puerta como animación
        }
    }();

    const auto groupId = CVehicleAnimGroup::GetGroup(animId);
    auto* animAssoc = CAnimManager::GetAnimAssociation(groupId, animId);
    auto* sequences = animAssoc->m_BlendHier->m_pSequences;
    CAnimManager::UncompressAnimation(animAssoc->m_BlendHier);
    if (const auto numFrames = static_cast<int16>(sequences->m_FramesNum); numFrames > 0) { // Como el original: con signo (jle)
        auto* frame = sequences->GetUKeyFrame(numFrames - 1);
        GetDoorOffset(doorId) = frame->Trans;
    }

    return GetDoorOffset(doorId);
}

int32 CVehicleAnimGroup::InitFromData(const char* line) {
    int32 id{};
    int32 firstGroup{}, secondGroup{};
    int32 animSpecialFlags{};
    sVehAnimGroupGeneralTiming generalTiming{};
    sVehAnimGroupInOutTiming startTiming{}, stopTiming{};

    std::array<bool32, 18> flags{};

    const auto n = sscanf_s(
        line,
        "%*s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%d",
        &id,
        &firstGroup,
        &secondGroup,

        // OpenOutF, GetInF, Jack, CloseInsR, GetOutF, BeJacked
        &flags[0], &flags[1], &flags[2], &flags[3], &flags[4], &flags[5],

        // CloseOutR, CloseRoll, OpenLocked, Align, OpenOutR, GetInR
        &flags[6], &flags[7], &flags[8], &flags[9], &flags[10], &flags[11],

        // CloseInsF, Shuffle, GetOutR, CloseOutF, JumpOut, FallDie
        &flags[12], &flags[13], &flags[14], &flags[15], &flags[16], &flags[17],

        &generalTiming.m_fGetInTime,
        &generalTiming.m_fJumpOutTime,
        &generalTiming.m_fGetOutTime,
        &generalTiming.m_fCarJackTime,
        &generalTiming.m_fFallOutTime,

        &startTiming.OpenOut,
        &stopTiming.OpenOut,

        &startTiming.CloseIn,
        &stopTiming.CloseIn,

        &startTiming.OpenIn,
        &stopTiming.OpenIn,

        &startTiming.CloseOut,
        &stopTiming.CloseOut,

        &animSpecialFlags
    );
    // Check for fail
    if (n != 35) {
        return n;
    }

    // Actually build the flags value from it
    int32 animFlags{};
    for (auto [i, flag] : rngv::enumerate(flags)) {
        animFlags |= (int32)flag << (int32)i;
    }

    // Create anim group
    CVehicleAnimGroup grp{};
    grp.InitAnimGroup(
        (uint8)firstGroup + (uint8)ANIM_GROUP_STDCARAMIMS, (uint8)secondGroup + (uint8)ANIM_GROUP_STDCARAMIMS,
        animFlags, animSpecialFlags,
        &generalTiming,
        &startTiming, &stopTiming
    );

    // Copy it into the current anim group data
    CopyAnimGroup(&grp);

    return -1;
}


void CVehicleAnimGroupData::InjectHooks()
{
    RH_ScopedClass(CVehicleAnimGroupData);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetInOutTimings, 0x645630);
    RH_ScopedInstall(GetGroupForAnim, 0x639FC0);
    RH_ScopedInstall(GetAnimDoorOffset, 0x645600);
    RH_ScopedInstall(UsesTruckDrivingAnims, 0x639FE0);
    RH_ScopedInstall(UsesKartDrivingAnims, 0x6D09E0);
}

// 0x645630
void CVehicleAnimGroupData::GetInOutTimings(AssocGroupId groupId, eInOutTimingMode mode, float* pfAnimStart, float* pfAnimEnd) {
    auto& group = CVehicleAnimGroupData::GetVehicleAnimGroup(groupId);
    *pfAnimStart = group.GetInOutTiming(eInOutTiming::TIMING_START)[mode];
    *pfAnimEnd = group.GetInOutTiming(eInOutTiming::TIMING_END)[mode];
}

// 0x639FC0
AssocGroupId CVehicleAnimGroupData::GetGroupForAnim(AssocGroupId groupId, AnimationId animId) {
    return CVehicleAnimGroupData::GetVehicleAnimGroup(groupId).GetGroup(animId);
}

// 0x645600
CVector CVehicleAnimGroupData::GetAnimDoorOffset(AssocGroupId groupId, eVehAnimDoorOffset doorId) {
    return CVehicleAnimGroupData::GetVehicleAnimGroup(groupId).ComputeAnimDoorOffsets(doorId);
}

float CVehicleAnimGroupData::ComputeCriticalBlendTime(AssocGroupId groupId, AnimationId animId) {
    // CVehicleAnimGroupData::GetVehicleAnimGroup(iGroup).GetGroup(animId)
    return CVehicleAnimGroupData::GetVehicleAnimGroup(groupId).ComputeCriticalBlendTime(animId);
}

// 0x639FE0
bool CVehicleAnimGroupData::UsesTruckDrivingAnims(AssocGroupId groupId) {
    return CVehicleAnimGroupData::GetVehicleAnimGroup(groupId).m_specialFlags.bUseTruckDriveAnims;
}

// 0x6D09E0
bool CVehicleAnimGroupData::UsesKartDrivingAnims(AssocGroupId groupId) {
    return CVehicleAnimGroupData::GetVehicleAnimGroup(groupId).m_specialFlags.bUseKartDriveAnims;
}

bool CVehicleAnimGroupData::UsesHovercraftDrivingAnims(AssocGroupId groupId) {
    return CVehicleAnimGroupData::GetVehicleAnimGroup(groupId).m_specialFlags.bUseHovercraftDriveAnims;
}

// NOTSA
int32 CVehicleAnimGroupData::LoadAGroupFromData(const char* line) {
    int32 id{};
    if (sscanf_s(line, "^\t%d", &id) != 1) {
        return 0;
    }
    return GetVehicleAnimGroup(id).InitFromData(line);
}
