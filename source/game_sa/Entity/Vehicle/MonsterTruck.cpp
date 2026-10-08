/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CMonsterTruck
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 6.
 * Validación registrada: 6/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   ExtendSuspension | 0x006C7D80 | S
 *   ProcessControl | 0x00000000 | V
 *   ProcessControlCollisionCheck | 0x00000000 | V
 *   ProcessSuspension | 0x00000000 | V
 *   ResetSuspension | 0x00000000 | V
 *   SetupSuspensionLines | 0x00000000 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "MonsterTruck.h"

auto& fWheelExtensionRate = StaticRef<float>(0x8D33AC);

void CMonsterTruck::InjectHooks() {
    RH_ScopedVirtualClass(CMonsterTruck, 0x8717d8, 71);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6C8D60);

    RH_ScopedInstall(ExtendSuspension, 0x6C7D80);

    RH_ScopedVMTInstall(ProcessEntityCollision, 0x6C8AE0);
    RH_ScopedVMTInstall(ProcessSuspension, 0x6C83A0);
    RH_ScopedVMTInstall(ProcessControlCollisionCheck, 0x6C8330);
    RH_ScopedVMTInstall(ProcessControl, 0x6C8250);
    RH_ScopedVMTInstall(SetupSuspensionLines, 0x6C7FB0);
    RH_ScopedVMTInstall(PreRender, 0x6C7DE0);
    RH_ScopedVMTInstall(ResetSuspension, 0x6C7D40);
    RH_ScopedVMTInstall(BurstTyre, 0x6C7D30);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6C7D20);
}

// 0x6C8D60
CMonsterTruck::CMonsterTruck(int32 modelIndex, eVehicleCreatedBy createdBy) : CAutomobile(modelIndex, createdBy, false) {
    std::ranges::fill(field_988, 1.0f);
    CMonsterTruck::SetupSuspensionLines();
    autoFlags.bIsMonsterTruck = true;
    m_nVehicleSubType = VEHICLE_TYPE_MTRUCK;
}

// 0x6C8AE0
int32 CMonsterTruck::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint) {
    if (GetStatus() != STATUS_SIMPLE) {
        vehicleFlags.bVehicleColProcessed = true; // OK
    }

    const auto tcm = GetColModel();

    if (physicalFlags.bSkipLineCol || physicalFlags.bProcessingShift || entity->GetIsTypePed()) {
        tcm->GetData()->m_nNumLines = 0; // hmm..... (Later reset back to 4)
    }

    auto wheelColPtsTouchDists{ m_wheelPosition };
    const auto numColPts = CCollision::ProcessColModels(
        GetMatrix(), *tcm,
        entity->GetMatrix(), *entity->GetColModel(),
        *(std::array<CColPoint, 32>*)(colPoint), // trust me bro
        m_wheelColPoint.data(),
        wheelColPtsTouchDists.data(),
        false
    );

    size_t numProcessedLines{};
    if (tcm->GetData()->m_nNumLines) {
        for (auto i = 0; i < MAX_CARWHEELS; i++) {
            const auto  thisWheelTouchDistNow = wheelColPtsTouchDists[i];
            const auto& thisWheelColPtNow = m_wheelColPoint[i];

            if (thisWheelTouchDistNow <= m_wheelPosition[i]) {
                continue;
            }

            if (!(GetUsesCollision() || !numColPts)) { // TODO: Why is this in the loop body?
                continue;
            }

            numProcessedLines++;

            m_fWheelsSuspensionCompression[i] = 0.f;
            m_wheelPosition[i] = thisWheelTouchDistNow;

            m_anCollisionLighting[i] = thisWheelColPtNow.m_nLightingB;
            m_nContactSurface = thisWheelColPtNow.m_nSurfaceTypeB;

            // Same as in CAutomobile::ProcessEntityCollision
            switch (entity->GetType()) {
            case ENTITY_TYPE_VEHICLE:
            case ENTITY_TYPE_OBJECT: {
                CEntity::ChangeEntityReference(m_apWheelCollisionEntity[i], entity->AsPhysical());

                m_vWheelCollisionPos[i] = thisWheelColPtNow.m_vecPoint - entity->GetPosition();
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
        tcm->GetData()->m_nNumLines = MAX_CARWHEELS; // TODO: Magic (Each wheel has 1 suspension line right now, but hardcoding like this isnt good)
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

// 0x6C83A0
void CMonsterTruck::ProcessSuspension() {
    std::array<CVector, 4> directions;
    std::array<CVector, 4> points;
    std::array<CVector, 4> speeds;
    std::array<float, 4> dampingLimits;
    const auto& up = GetUp();
    for (size_t i = 0; i < 4; ++i) {
        directions[i] = CVector{up.x * -1.f, up.y * -1.f, up.z * -1.f};
        points[i] = m_fWheelsSuspensionCompression[i] < 1.f ? m_wheelColPoint[i].m_vecPoint - GetPosition() : CVector{};
    }
    for (size_t i = 0; i < 4; ++i) {
        if (m_fWheelsSuspensionCompression[i] < 1.f) {
            const auto bias = i == 1 || i == 3 ? 1.f - m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear : m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear;
            ApplySpringCollisionAlt(m_pHandlingData->m_fSuspensionForceLevel, directions[i], points[i], m_fWheelsSuspensionCompression[i], bias, m_wheelColPoint[i].m_vecNormal, dampingLimits[i]);
        }
    }
    for (size_t i = 0; i < 4; ++i) {
        speeds[i] = GetSpeed(points[i]);
        if (m_apWheelCollisionEntity[i]) {
            speeds[i] -= m_apWheelCollisionEntity[i]->GetSpeed(m_vWheelCollisionPos[i]);
        }
        if (m_fWheelsSuspensionCompression[i] < StaticRef<float>(0x8717D4) && m_wheelColPoint[i].m_vecNormal.z > StaticRef<float>(0x858F9C)) {
            directions[i] = -m_wheelColPoint[i].m_vecNormal;
        }
    }
    for (size_t i = 0; i < 4; ++i) {
        if (m_fWheelsSuspensionCompression[i] < 1.f) {
            ApplySpringDampening(m_pHandlingData->m_fSuspensionDampingLevel, dampingLimits[i], directions[i], points[i], speeds[i]);
        }
    }
    for (size_t i = 0; i < 4; ++i) {
        auto* collision = m_apWheelCollisionEntity[i];
        auto& colPoint = m_wheelColPoint[i];
        if (m_fWheelsSuspensionCompression[i] < 1.f && collision && collision->GetType() == ENTITY_TYPE_VEHICLE) {
            if (m_fWheelsSuspensionCompression[i] < .5f) {
                static_cast<CVehicle*>(collision)->VehicleDamage((1.f - m_fWheelsSuspensionCompression[i]) * m_fMass * StaticRef<float>(0x8D33B8), static_cast<eVehicleCollisionComponent>(colPoint.m_nPieceTypeB), this, &colPoint.m_vecPoint, &colPoint.m_vecNormal, static_cast<eWeaponType>(49));
            }
            if (colPoint.m_vecNormal.z > .5f) {
                const auto point = colPoint.m_vecPoint - collision->GetPosition();
                const auto ratio = 1.f - m_fWheelsSuspensionCompression[i];
                CVector force{colPoint.m_vecNormal.x * .25f, colPoint.m_vecNormal.y * .25f, colPoint.m_vecNormal.z};
                force.x = ((force.x * StaticRef<float>(0x8D33B4)) * ratio) * collision->m_fMass;
                force.y = ((force.y * StaticRef<float>(0x8D33B4)) * ratio) * collision->m_fMass;
                force.z = ((force.z * StaticRef<float>(0x8D33B4)) * ratio) * collision->m_fMass;
                collision->ApplyForce(force, point, true);
            }
        }
        m_apWheelCollisionEntity[i] = nullptr;
    }
}

// 0x6C8330
void CMonsterTruck::ProcessControlCollisionCheck(bool applySpeed) {
    ExtendSuspension();
    CAutomobile::ProcessControlCollisionCheck(applySpeed);
    for (size_t i = 0; i < 4; ++i) {
        m_fWheelsSuspensionCompression[i] = m_fWheelsSuspensionCompression[i] < 1.f
            ? (m_aSuspensionSpringLength[i] - m_wheelPosition[i]) / (m_aSuspensionSpringLength[i] - m_aSuspensionLineLength[i])
            : 1.f;
    }
}

// 0x6C8250
void CMonsterTruck::ProcessControl() {
    const auto clampNegative = StaticRef<uint8>(0x8D33B0);
    for (size_t i = 0; i < 4; ++i) {
        if (m_fWheelsSuspensionCompression[i] < 1.f) {
            m_fWheelsSuspensionCompression[i] = (m_aSuspensionSpringLength[i] - m_wheelPosition[i]) / (m_aSuspensionSpringLength[i] - m_aSuspensionLineLength[i]);
            if (m_fWheelsSuspensionCompression[i] < 0.f && clampNegative) {
                m_fWheelsSuspensionCompression[i] = 0.f;
            }
        } else {
            m_fWheelsSuspensionCompression[i] = 1.f;
        }
    }
    CAutomobile::ProcessControl();
    if (!m_bWasPostponed && (m_vecMoveSpeed.x != 0.f || m_vecMoveSpeed.y != 0.f || m_vecMoveSpeed.z != 0.f
        || m_vecTurnSpeed.x != 0.f || m_vecTurnSpeed.y != 0.f || m_vecTurnSpeed.z != 0.f)) {
        ExtendSuspension();
    }
}

// 0x6C7FB0
void CMonsterTruck::SetupSuspensionLines() {
    auto& model = *GetVehicleModelInfo();
    auto& colModel = *model.GetColModel();
    auto& data = *colModel.GetData();
    m_fSuspensionRadius = model.m_fWheelSizeFront * .5f;
    if (!data.m_pDisks || !data.bUsesDisks) {
        if (data.m_pDisks) {
            CMemoryMgr::Free(data.m_pDisks);
        }
        data.bUsesDisks = true;
        data.m_nNumLines = 4;
        data.m_pDisks = static_cast<CColDisk*>(CMemoryMgr::Malloc(4 * sizeof(CColDisk), 0));
    }
    for (int32 i = 0; i < 4; ++i) {
        CVector wheel;
        model.GetWheelPosn(i, wheel, false);
        auto& disk = data.m_pDisks[i];
        disk.m_fRadius = m_fSuspensionRadius;
        disk.m_vecCenter = wheel;
        disk.m_vThickness = CVector{i < 2 ? -1.f : 1.f, 0.f, 0.f};
        disk.m_fThickness = m_fSuspensionRadius * StaticRef<float>(0x858CC8);
        disk.m_Surface.m_nMaterial = static_cast<eSurfaceType>(60);
        disk.m_Surface.m_nPiece = std::array<uint8, 4>{13, 15, 14, 16}[i];
        disk.m_Surface.m_nLighting = tColLighting(255);
        m_aSuspensionSpringLength[i] = wheel.z + m_pHandlingData->m_fSuspensionUpperLimit;
        m_aSuspensionLineLength[i] = wheel.z + m_pHandlingData->m_fSuspensionLowerLimit;
    }
    const auto ratio = 1.f - 1.f / (m_pHandlingData->m_fSuspensionForceLevel * 4.f);
    m_fFrontHeightAboveRoad = ratio * (m_aSuspensionSpringLength[0] - m_aSuspensionLineLength[0]) + (m_fSuspensionRadius - m_aSuspensionSpringLength[0]);
    m_fRearHeightAboveRoad = m_fFrontHeightAboveRoad;
    for (size_t i = 0; i < 4; ++i) {
        m_fWheelsSuspensionCompression[i] = 1.f;
        m_wheelPosition[i] = model.m_fWheelSizeFront * .5f - m_fFrontHeightAboveRoad;
    }
    const auto bottom = m_fFrontHeightAboveRoad - m_fSuspensionRadius;
    if (bottom < colModel.m_boundBox.m_vecMin.z) {
        colModel.m_boundBox.m_vecMin.z = bottom;
    }
    const auto minSize = colModel.m_boundBox.m_vecMin.Magnitude();
    const auto maxSize = colModel.m_boundBox.m_vecMax.Magnitude();
    const auto radius = minSize > maxSize ? minSize : maxSize;
    if (radius > colModel.m_boundSphere.m_fRadius) {
        colModel.m_boundSphere.m_fRadius = radius;
    }
}

// 0x6C7DE0
void CMonsterTruck::PreRender() {
    for (auto i = 0; i < 4; i++) {
        m_wheelPosition[i] = std::min(m_wheelPosition[i], m_aSuspensionSpringLength[i]);
    }

    CAutomobile::PreRender();

    const auto mi = GetVehicleModelInfo();
    CMatrix mat;
    CVector pos;

    mi->GetWheelPosn(CAR_WHEEL_FRONT_LEFT, pos, false);
    SetTransmissionRotation(m_aCarNodes[MONSTER_TRANSMISSION_F], m_wheelPosition[CAR_WHEEL_FRONT_LEFT], m_wheelPosition[CAR_WHEEL_FRONT_RIGHT], pos, true);

    mi->GetWheelPosn(CAR_WHEEL_REAR_LEFT, pos, false);
    SetTransmissionRotation(m_aCarNodes[MONSTER_TRANSMISSION_R], m_wheelPosition[CAR_WHEEL_REAR_LEFT], m_wheelPosition[CAR_WHEEL_REAR_RIGHT], pos, false);

    if (m_nModelIndex == MODEL_DUMPER && m_aCarNodes[MONSTER_MISC_A]) {
        SetComponentRotation(m_aCarNodes[MONSTER_MISC_A], AXIS_X, (float)m_wMiscComponentAngle * DUMPER_COL_ANGLEMULT, true);
    }
}

// 0x6C7D80
void CMonsterTruck::ExtendSuspension() {
    for (size_t i = 0; i < 4; ++i) {
        auto value = m_wheelPosition[i] - CTimer::GetTimeStep() * m_fSuspensionRadius * fWheelExtensionRate;
        m_wheelPosition[i] = value;
        if (value < m_aSuspensionLineLength[i]) {
            m_wheelPosition[i] = m_aSuspensionLineLength[i];
        } else if (value > m_aSuspensionSpringLength[i]) {
            m_wheelPosition[i] = m_aSuspensionSpringLength[i];
        }
        m_fWheelsSuspensionCompression[i] = 1.f;
    }
}

// 0x6C7D40
void CMonsterTruck::ResetSuspension() {
    CAutomobile::ResetSuspension();
    m_wheelPosition = m_aSuspensionLineLength;
    std::ranges::fill(field_988, 1.0f);
}

// 0x6C7D30
bool CMonsterTruck::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6C7D20
bool CMonsterTruck::SetUpWheelColModel(CColModel* colModel) {
    return false;
}
