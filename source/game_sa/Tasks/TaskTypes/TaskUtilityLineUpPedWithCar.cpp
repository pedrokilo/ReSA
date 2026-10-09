/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskUtilityLineUpPedWithCar
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskUtilityLineUpPedWithCar.h"
#include "VehicleAnimGroupData.h"
#include "Models/VehicleModelInfo.h"
#include "PedPlacement.h"
#include "RenderWare/rw/rtslerp.h"

void CTaskUtilityLineUpPedWithCar::InjectHooks() {
    RH_ScopedClass(CTaskUtilityLineUpPedWithCar);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x64FBB0);
    RH_ScopedInstall(Destructor, 0x64FC00);

    RH_ScopedInstall(GetLocalPositionToOpenCarDoor, 0x64FC10);
    RH_ScopedInstall(GetPositionToOpenCarDoor, 0x650A80);
    RH_ScopedInstall(ProcessPed, 0x6513A0);
}

// 0x64FBB0
CTaskUtilityLineUpPedWithCar::CTaskUtilityLineUpPedWithCar(const CVector& offset, int32 time, int32 doorOpenPosType, int32 doorIdx) {
    m_Offset = offset;
    m_fDoorOpenPosZ = -999.99f;
    m_fTime = int32(CTimer::GetTimeInMS()) + time; // Como el original: 0xB7CB84 (m_snTimeInMilliseconds) + time
    m_nDoorOpenPosType = doorOpenPosType;
    m_nDoorIdx = doorIdx;
}

// The following 2 functions seem to have copy elision on the returned CVector, that the compiled functions
// took a vector ptr as their first arg. Now, hopefully our code will compile to the same stuff.
// If not, it might crash here, in that case a wrapper function should be used.

// 0x64FC10
CVector CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    auto* model = static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo(vehicle->m_nModelIndex));
    if ((*reinterpret_cast<const uint8*>(reinterpret_cast<const uint8*>(vehicle) + 0x429) & 1) && (m_nDoorIdx == 11 || m_nDoorIdx == 9)) {
        animProgress = 0.0f;
    } else {
        animProgress *= vehicle->m_pHandlingData->m_fSeatOffsetDistance;
    }
    int32 category = 0;
    if (assoc) {
        const auto id = int(assoc->m_AnimId);
        if (id >= 0x15F && id <= 0x174) category = 0;
        else if ((id >= 0x6C && id <= 0x6D) || (id >= 0x175 && id <= 0x178) ||
                 (id >= 0x17C && id <= 0x17F) || id == 0x183 || id == 0x184) category = 3;
        else if (id == 0x17A || id == 0x17B) category = 5;
        else if (id == 0x185 || id == 0x186) category = 0;
        else category = int32(reinterpret_cast<uintptr_t>(vehicle)); // Original fallback, only the listed animations are supported.
    }
    if (category == 0 || category == 3) {
        if (m_nDoorIdx == 10 || m_nDoorIdx == 8) category = 0;
        else if (m_nDoorIdx == 11 || m_nDoorIdx == 9) category = 1;
        else if (m_nDoorIdx == 18) category = 2;
    } else if (category == 5) {
        category = 0;
    }
    auto& group = CVehicleAnimGroupData::GetVehicleAnimGroup(vehicle->m_pHandlingData->m_nAnimGroup);
    CVector offset;
    if (assoc && (int(assoc->m_AnimId) == 0x185 || int(assoc->m_AnimId) == 0x186) && group.m_specialFlags.bRunSpecialLockedDoor) {
        const auto first = group.ComputeAnimDoorOffsets(eVehAnimDoorOffset(category));
        const auto weightedFirst = first * (1.0f - assoc->m_BlendAmount);
        const auto second = CVehicleAnimGroupData::GetVehicleAnimGroup(0).ComputeAnimDoorOffsets(eVehAnimDoorOffset(category));
        offset = second * assoc->m_BlendAmount + weightedFirst;
    } else {
        offset = group.ComputeAnimDoorOffsets(eVehAnimDoorOffset(category));
    }
    CVector seat, result;
    switch (m_nDoorIdx) {
    case 8:
        seat = model->GetFrontSeatPosn();
        seat.x += animProgress;
        offset.x = -offset.x;
        result = seat - offset;
        break;
    case 9:
        seat = model->GetBackSeatPosn();
        seat.x += animProgress;
        offset.x = -offset.x;
        result = seat - offset;
        break;
    case 10:
        seat = model->GetFrontSeatPosn();
        seat.x = -(seat.x + animProgress);
        result = seat - offset;
        break;
    case 11:
        seat = model->GetBackSeatPosn();
        seat.x = -(seat.x + animProgress);
        result = seat - offset;
        break;
    case 18:
        seat = model->GetFrontSeatPosn();
        result = offset + seat;
        break;
    default:
        seat = model->GetFrontSeatPosn();
        offset = CVector{};
        result = seat;
        break;
    }
    if (int(vehicle->m_nVehicleType) == 9 && m_nDoorIdx != 18) {
        vehicle->AsBike()->GetCorrectedWorldDoorPosition(result, offset, seat);
        result -= vehicle->GetPosition();
        result = vehicle->m_matrix->InverseTransformVector(result);
    }
    return result;
}

// 0x650A80
CVector CTaskUtilityLineUpPedWithCar::GetPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    const CMatrix matrix{*vehicle->m_matrix};
    const auto local = GetLocalPositionToOpenCarDoor(vehicle, animProgress, assoc);
    const auto transformed = matrix.TransformVector(local);
    const auto& position = vehicle->GetPosition();
    return CVector{position.x + transformed.x, position.y + transformed.y, position.z + transformed.z};
}

// 0x6513A0
bool CTaskUtilityLineUpPedWithCar::ProcessPed(CPed* ped, CVehicle* vehicle, CAnimBlendAssociation* assoc) {
    if (m_nDoorOpenPosType == 0) ped->m_vecMoveSpeed = CVector{};
    bool upright = false;
    const auto heading = vehicle->GetHeading();
    constexpr float pi = 3.1415927410125732f;
    // The EXE branches on PF after test AH,41h: positive up.z enters this arm.
    if (!(vehicle->m_matrix->GetUp().z <= -0.8f)) {
        if (m_nDoorIdx == 18) ped->m_fAimingRotation = heading + pi;
        else if (m_nDoorOpenPosType != 2) ped->m_fAimingRotation = heading;
    } else {
        upright = true;
        ped->m_fAimingRotation = m_nDoorIdx == 8 || m_nDoorIdx == 9 ? heading - pi : heading;
    }
    float progress = 0.0f, verticalBlend = 0.0f;
    int32 animId = assoc ? int(assoc->m_AnimId) : -1;
    auto& group = vehicle->GetAnimGroup();
    const auto blendTime = [&] {
        group.GetGroup(AnimationId(animId));
        return group.ComputeCriticalBlendTime(AnimationId(animId));
    };
    if (assoc) {
        const auto fraction = assoc->m_CurrentTime / assoc->m_BlendHier->m_fTotalTime;
        if (animId == 0x6C || animId == 0x6D) progress = fraction;
        else if (animId >= 0x15F && animId <= 0x162) {
            progress = 1.0f;
            const auto timing = blendTime();
            if (std::abs(timing) >= 10.0f) {
                const auto critical = std::abs(timing) - 11.0f;
                if (timing - 11.0f <= 0.0f) verticalBlend = fraction >= critical ? (fraction - critical) / (1.0f - critical) : 0.0f;
                else verticalBlend = fraction >= critical ? 1.0f : fraction / critical;
            }
            if (vehicle->m_nModelIndex == 577) verticalBlend = 1.0f;
        } else if (animId >= 0x163 && animId <= 0x166) {
            progress = 1.0f;
            if (std::abs(blendTime()) >= 10.0f || vehicle->m_nModelIndex == 577) verticalBlend = 1.0f;
        } else if (animId >= 0x167 && animId <= 0x16B) {
            progress = 1.0f - fraction;
            const auto timing = blendTime();
            const auto critical = std::abs(timing);
            if (critical >= 10.0f) verticalBlend = 1.0f;
            else if (timing <= 0.0f) verticalBlend = fraction >= critical ? (fraction - critical) / (1.0f - critical) : 0.0f;
            else verticalBlend = fraction >= critical ? 1.0f : fraction / critical;
            if (vehicle->m_nModelIndex == 577) verticalBlend = 1.0f;
        } else if ((animId >= 0x16C && animId <= 0x16E) || (animId >= 0x17C && animId <= 0x17F)) {
            progress = 1.0f;
        } else if ((animId >= 0x16F && animId <= 0x174) || animId == 0x182) {
            verticalBlend = 1.0f;
        } else if ((animId >= 0x175 && animId <= 0x17B) || animId == 0x180 || animId == 0x181 || animId == 0x183 || animId == 0x184) {
            progress = fraction;
            const auto timing = blendTime();
            const auto critical = std::abs(timing);
            if (timing <= 0.0f) verticalBlend = fraction < critical ? 1.0f : 1.0f - (fraction - critical) / (1.0f - critical);
            else verticalBlend = fraction >= critical ? 0.0f : 1.0f - fraction / critical;
        } else if (animId == 0x185 || animId == 0x186) {
            progress = 1.0f;
            if (group.m_specialFlags.bRunSpecialLockedDoor) verticalBlend = 1.0f;
        }
    }
    auto target = m_nDoorOpenPosType == 2 ? ped->GetPosition() : GetPositionToOpenCarDoor(vehicle, progress, assoc);
    auto groundTarget = target;
    if (int(vehicle->m_nVehicleType) != 9 && m_nDoorOpenPosType != 2) groundTarget = GetPositionToOpenCarDoor(vehicle, 1.0f, assoc);
    if (!vehicle->physicalFlags.bSubmergedInWater) {
        const auto oldZ = groundTarget.z;
        const auto carPos = vehicle->GetPosition();
        const auto right = vehicle->GetRight();
        const auto delta = groundTarget - carPos;
        const float projected = delta.x * right.x + right.y * delta.y + right.z * delta.z;
        const auto height = vehicle->GetHeightAboveRoad();
        groundTarget = CPedPlacement::FindZCoorForPed(groundTarget).first;
        const auto lowest = right.z * projected + (carPos.z - height) + 1.0f;
        if (groundTarget.z < lowest - 0.5f) groundTarget.z = oldZ;
    } else if (int(vehicle->m_nVehicleType) == 5 && vehicle->IsUpsideDown()) {
        groundTarget.z += 1.0f;
    }
    m_fDoorOpenPosZ = groundTarget.z;
    if (m_nDoorOpenPosType == 1 || m_nDoorOpenPosType == 2) {
        const auto fallingSpeed = ped->m_vecMoveSpeed.z - CTimer::GetTimeStep() * 0.008f;
        if (groundTarget.z <= fallingSpeed + ped->GetPosition().z) {
            ped->m_vecMoveSpeed.z = fallingSpeed;
            target.z = ped->GetPosition().z;
        } else {
            target.z = groundTarget.z;
            ped->m_vecMoveSpeed = CVector{};
        }
    }
    if (m_fDoorOpenPosZ > target.z || m_nDoorOpenPosType == 0) {
        float adjustment;
        if (m_fDoorOpenPosZ > target.z && int(vehicle->m_nVehicleType) == 9 && assoc && animId != 0x16B) {
            float bikeBlend = 0.0f;
            if (animId >= 0x175 && animId <= 0x178) bikeBlend = 1.0f - progress;
            else if (animId >= 0x167 && animId <= 0x16A) {
                const auto fraction = assoc->m_CurrentTime / assoc->m_BlendHier->m_fTotalTime;
                bikeBlend = fraction + fraction <= 1.0f ? fraction + fraction : 1.0f;
            }
            adjustment = ((vehicle->GetPosition().z - vehicle->GetHeightAboveRoad()) + 1.0f - m_fDoorOpenPosZ) * bikeBlend;
        } else {
            adjustment = (target.z - m_fDoorOpenPosZ) * verticalBlend;
        }
        target.z = adjustment + m_fDoorOpenPosZ;
    }
    if (CTimer::GetTimeInMS() < uint32(m_fTime)) {
        auto angle = CGeneral::LimitRadianAngle(ped->m_fAimingRotation);
        const auto current = ped->m_fCurrentRotation;
        const auto fade = float(uint32(m_fTime) - CTimer::GetTimeInMS()) * 0.0016666667070239782f;
        if (fade <= 0.0f) m_Offset.x = m_Offset.y = 0.0f;
        m_Offset.z = 0.0f;
        target.x -= fade * m_Offset.x;
        target.y -= fade * m_Offset.y;
        target.z -= 0.0f * fade;
        if (angle > current + pi) angle -= 6.2831854820251465f;
        else if (angle < current - pi) angle += 6.2831854820251465f;
        ped->m_fCurrentRotation = current - (1.0f - fade) * (current - angle);
    } else {
        ped->m_fCurrentRotation = ped->m_fAimingRotation;
    }
    if (assoc && ((animId >= 0x167 && animId <= 0x16B) || (animId >= 0x175 && animId <= 0x179))) {
        CMatrix carMatrix{*vehicle->m_matrix};
        CMatrix pedMatrix{*ped->m_matrix};
        const bool leaving = animId >= 0x167 && animId <= 0x16B;
        if (leaving && animId == 0x16B) {
            CMatrix rotate;
            rotate.SetRotateZ(pi);
            carMatrix *= rotate;
        } else if (!leaving) {
            pedMatrix.SetRotateZOnly(ped->m_fCurrentRotation);
        }
        RwMatrix carRw, pedRw;
        carMatrix.CopyToRwMatrix(&carRw);
        pedMatrix.CopyToRwMatrix(&pedRw);
        RtQuat carQuat, pedQuat, blended;
        RtQuatConvertFromMatrix(&carQuat, &carRw);
        RtQuatConvertFromMatrix(&pedQuat, &pedRw);
        RtQuatSlerpCache cache;
        const auto fraction = assoc->m_CurrentTime / assoc->m_BlendHier->m_fTotalTime;
        if (leaving) {
            RtQuatSetupSlerpCache(&pedQuat, &carQuat, &cache);
            RtQuatSlerp(&blended, &pedQuat, &carQuat, fraction, &cache);
        } else {
            RtQuatSetupSlerpCache(&carQuat, &pedQuat, &cache);
            RtQuatSlerp(&blended, &carQuat, &pedQuat, fraction, &cache);
        }
        RwMatrix out;
        RtQuatConvertToMatrix(&blended, &out);
        CMatrix result;
        result.UpdateMatrix(&out);
        result.GetPosition() = target;
        ped->SetMatrix(result);
    // 0x65264C also tests PF: progress > .2 (or unordered) takes orientation.
    } else if (!(progress <= 0.2f) || upright || int(vehicle->m_nVehicleType) == 9 || int(vehicle->m_nVehicleSubType) == 2) {
        ped->GetPosition() = target;
        ped->SetOrientation(0.0f, 0.0f, ped->m_fCurrentRotation);
    } else {
        CMatrix result{*vehicle->m_matrix};
        const auto local = GetLocalPositionToOpenCarDoor(vehicle, 0.0f, assoc);
        result.GetPosition() += result.TransformVector(local);
        ped->SetMatrix(result);
    }
    return false;
}
