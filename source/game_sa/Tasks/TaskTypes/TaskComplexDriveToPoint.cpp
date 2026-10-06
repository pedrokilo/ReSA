/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDriveToPoint
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexDriveToPoint.h"
#include "TaskComplexGoToPointAnyMeans.h"
#include "Models/ModelInfo.h"

void CTaskComplexDriveToPoint::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDriveToPoint, 0x86E9DC, 14);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x63CE00);

    RH_ScopedOverloadedInstall(IsTargetBlocked, "", 0x6452C0, bool(CTaskComplexDriveToPoint::*)(CPed*) const);
    RH_ScopedOverloadedInstall(IsTargetBlocked, "entities", 0x6432A0, bool(CTaskComplexDriveToPoint::*)(CPed*, CEntity**, int32) const);

    RH_ScopedVMTInstall(Clone, 0x63DDE0);
    RH_ScopedVMTInstall(GetTaskType, 0x63CE60);
    RH_ScopedVMTInstall(SetUpCar, 0x63CF00);
    RH_ScopedVMTInstall(CreateSubTaskCannotGetInCar, 0x63CE80);
    RH_ScopedVMTInstall(Drive, 0x645420);
}

// 0x63CE00
CTaskComplexDriveToPoint::CTaskComplexDriveToPoint(CVehicle* vehicle, const CVector& point, float speed, int32 arg4, eModelID carModelIndexToCreate, float radius, eCarDrivingStyle drivingStyle) :
      CTaskComplexCarDrive(vehicle, speed, carModelIndexToCreate, drivingStyle),
      m_Point{ point },
      field_30{ arg4 },
      m_Radius{ radius },
      field_38{ false }
{

}

// 0x63CE80
CTask* CTaskComplexDriveToPoint::CreateSubTaskCannotGetInCar(CPed* ped) {
    return new CTaskComplexGoToPointAnyMeans(PEDMOVE_RUN, m_Point, 0.5f, m_DesiredCarModel);
}

// 0x63CF00
void CTaskComplexDriveToPoint::SetUpCar() {
    m_OriginalDrivingStyle = m_Veh->m_autoPilot.m_nCarDrivingStyle;
    m_OriginalMission         = m_Veh->m_autoPilot.m_nCarMission;
    m_OriginalSpeed              = m_Veh->m_autoPilot.m_nCruiseSpeed;

    m_bIsCarSetUp = true;

    if (m_CruiseSpeed > 0.0f) {
        // Como el original: _ftol2 y se guarda el byte bajo (sin aserción de rango)
        m_Veh->m_autoPilot.m_nCruiseSpeed = (uint8)(int32)m_CruiseSpeed;
    }
    m_Veh->m_autoPilot.m_nCarDrivingStyle    = static_cast<eCarDrivingStyle>(m_CarDrivingStyle);
    m_Veh->m_autoPilot.m_nTimeToStartMission = CTimer::GetTimeInMS();
}

// 0x645420
CTask* CTaskComplexDriveToPoint::Drive(CPed* ped) {
    // Como el original: distancia con la suma en el orden del x87, (dz*dz + dy*dy) + dx*dx, y raíz en float.
    const auto& vehPos = m_Veh->GetPosition();
    const float dx = m_Point.x - vehPos.x;
    const float dy = m_Point.y - vehPos.y;
    const float dz = m_Point.z - vehPos.z;
    const float dist = std::sqrt((dz * dz + dy * dy) + dx * dx);

    if (dist < m_Radius) {
        m_Veh->m_autoPilot.m_nCarMission = MISSION_NONE;
        field_38 = true;
        return CTaskComplexCarDrive::CreateSubTask(TASK_FINISHED, ped);
    }

    if (dist < 3.0f && !m_Veh->m_autoPilot.m_nCarMission) {
        field_38 = true;
        return CTaskComplexCarDrive::CreateSubTask(TASK_FINISHED, ped);
    }

    if (!m_Veh->m_autoPilot.m_nCruiseSpeed) {
        m_Veh->m_autoPilot.m_nCruiseSpeed = (uint8)(int32)m_CruiseSpeed;
    }

    if (IsTargetBlocked(ped)) {
        field_38 = true;
        return CTaskComplexCarDrive::CreateSubTask(TASK_FINISHED, ped);
    }

    switch (field_30) {
    case field_30_enum::DEFAULT:       CCarAI::GetCarToGoToCoors(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    case field_30_enum::ACCURATE:      CCarAI::GetCarToGoToCoorsAccurate(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    case field_30_enum::STRAIGHT_LINE: CCarAI::GetCarToGoToCoorsStraightLine(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    case field_30_enum::RACING:        CCarAI::GetCarToGoToCoorsRacing(m_Veh, m_Point, m_CarDrivingStyle, false); break;
    default:                           break; // Como el original: valores fuera de 0..3 no hacen nada
    }
    return m_pSubTask;
}

// 0x6452C0
bool CTaskComplexDriveToPoint::IsTargetBlocked(CPed* ped) const {
    // Como el original: distancia al cuadrado con la suma (dz*dz + dx*dx) + dy*dy
    const auto& pedPos = ped->GetPosition();
    const float dx = pedPos.x - m_Point.x;
    const float dy = pedPos.y - m_Point.y;
    const float dz = pedPos.z - m_Point.z;
    const float distSq = (dz * dz + dx * dx) + dy * dy;
    if (36.0f < distSq) {
        return false;
    }

    auto intel = ped->GetIntelligence();
    return IsTargetBlocked(ped, intel->GetPedEntities(), 16) || IsTargetBlocked(ped, intel->GetVehicleEntities(), 16);
}

// 0x6432A0
bool CTaskComplexDriveToPoint::IsTargetBlocked(CPed* ped, CEntity** entities, int32 numEntities) const {
    const auto veh = ped->m_pVehicle;
    if (!veh) {
        return false;
    }

    const float vehRadius = CModelInfo::ms_modelInfoPtrs[(int16)veh->m_nModelIndex]->GetColModel()->GetBoundRadius();
    const auto& vehPos = veh->GetPosition();
    const float vdx = vehPos.x - m_Point.x;
    const float vdy = vehPos.y - m_Point.y;
    const float vdz = vehPos.z - m_Point.z;
    const float vehDistSq = (vdz * vdz + vdy * vdy) + vdx * vdx;

    for (int32 i = 0; i < numEntities; ++i) {
        const auto entity = entities[i];
        if (!entity || entity == veh) {
            continue;
        }

        // Como el original: se mide la distancia de la entidad al destino frente a su propio radio
        const auto& entPos = entity->GetPosition();
        const float edx = entPos.x - m_Point.x;
        const float edy = entPos.y - m_Point.y;
        const float edz = entPos.z - m_Point.z;
        const float entDistSq = (edz * edz + edy * edy) + edx * edx;
        const float entRadius = CModelInfo::ms_modelInfoPtrs[(int16)entity->m_nModelIndex]->GetColModel()->GetBoundRadius();
        if (!(entRadius * entRadius > entDistSq)) {
            continue;
        }

        const float radiusSum = entRadius + vehRadius;
        // Como el original: (suma de radios)^2 en float por la constante double 1.5 (0x86C680)
        if ((float)((double)(radiusSum * radiusSum) * 1.5) > vehDistSq) {
            return true;
        }
    }
    return false;
}

void CTaskComplexDriveToPoint::GoToPoint(const CVector& point) {
    m_Point = point;
}
