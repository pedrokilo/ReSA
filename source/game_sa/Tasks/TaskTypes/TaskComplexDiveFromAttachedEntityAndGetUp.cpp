/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDiveFromAttachedEntityAndGetUp
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexDiveFromAttachedEntityAndGetUp.h"
#include "PedGeometryAnalyser.h"

void CTaskComplexDiveFromAttachedEntityAndGetUp::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDiveFromAttachedEntityAndGetUp, 0x85a358, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x492E20);
    RH_ScopedInstall(Destructor, 0x492F10);

    RH_ScopedVMTInstall(Clone, 0x492E70);
    RH_ScopedVMTInstall(GetTaskType, 0x492EE0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6577F0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x657820);
    RH_ScopedVMTInstall(ControlSubTask, 0x655710);
}

// 0x492E20
CTaskComplexDiveFromAttachedEntityAndGetUp::CTaskComplexDiveFromAttachedEntityAndGetUp(int32 unconsciousTime) :
    CTaskComplexEvasiveDiveAndGetUp{
        nullptr,
        unconsciousTime,
        {},
        true
    }
{
}

// 0x492E70
CTaskComplexDiveFromAttachedEntityAndGetUp::CTaskComplexDiveFromAttachedEntityAndGetUp(const CTaskComplexDiveFromAttachedEntityAndGetUp& o) :
    CTaskComplexDiveFromAttachedEntityAndGetUp{o.m_UnconsciousTime}
{
}

// 0x6577F0
CTask* CTaskComplexDiveFromAttachedEntityAndGetUp::CreateNextSubTask(CPed* ped) {
    if (m_pSubTask->GetTaskType() == TASK_SIMPLE_ACHIEVE_HEADING) {
        ped->DettachPedFromEntity();
    }
    return CTaskComplexEvasiveDiveAndGetUp::CreateNextSubTask(ped);
}

// 0x657820
CTask* CTaskComplexDiveFromAttachedEntityAndGetUp::CreateFirstSubTask(CPed* ped) {
    if (const auto attachedTo = ped->m_pAttachedTo) {
        const auto hitSide = CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *attachedTo);

        // Como el original: las cuatro direcciones salen de ComputeEntityDirs (0x5F1500) y se elige la del lado golpeado
        CVector dirs[4];
        CPedGeometryAnalyser::ComputeEntityDirs(*attachedTo, dirs);
        m_DiveDir = dirs[hitSide];

        if (attachedTo->GetIsTypeVehicle() && (hitSide == (int32)eDirection::BACKWARD || hitSide == (int32)eDirection::FORWARD)) {
            // Como el original: (z*z + y*y) + x*x, y NaN no cumple `> 0`
            const auto& moveSpeed = attachedTo->AsPhysical()->m_vecMoveSpeed;
            if (moveSpeed.z * m_DiveDir.z + moveSpeed.y * m_DiveDir.y + m_DiveDir.x * moveSpeed.x > 0.f) {
                CVector bb[4];
                float   bbPlanesDot[4];
                CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes(ped->GetPosition().z, *attachedTo, &bb, bbPlanesDot);
                const auto& pedPos = ped->GetPosition();
                const auto GetPlaneDot = [&](int32 i) {
                    return std::abs(bb[i].y * pedPos.y + bb[i].z * pedPos.z + bb[i].x * pedPos.x + bbPlanesDot[i]);
                };
                const float dot1 = GetPlaneDot(1);
                const float dot3 = GetPlaneDot(3);
                // Como el original: se copian las direcciones de la entidad (no las normales de los planos)
                m_DiveDir = dot3 > dot1 ? dirs[1] : dirs[3];
            }
        }
    } else {
        m_DiveDir = ped->GetForward();
    }
    if (ped->m_pAttachedTo) {
        ped->m_fTurretAngleB = std::bit_cast<float>(0x40C90FDBu); // Como el original: 2*pi exacto (PI de common.h es impreciso)
    }
    return CreateSubTask(m_bAchieveHeadingFirst ? TASK_SIMPLE_ACHIEVE_HEADING : TASK_SIMPLE_EVASIVE_DIVE);
}
