/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexHitPedWithCar
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 8.
 * Validación registrada: 8/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x006559B0 | V
 *   ComputeEvasiveStepMoveDir | 0x00653B40 | S
 *   ControlSubTask | 0x00653A90 | V
 *   CreateFirstSubTask | 0x00656300 | V
 *   CreateNextSubTask | 0x00657AF0 | V
 *   CreateSubTask | 0x006560E0 | S
 *   GetTaskType | 0x00653A20 | V
 *   HitHurtsPed | 0x00653AE0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexHitPedWithCar.h"
#include "TaskSimpleKillPedWithCar.h"
#include "TaskSimpleHurtPedWithCar.h"
#include "TaskComplexEvasiveStep.h"
#include "TaskSimpleHitFromBehind.h"
#include "TaskComplexFallAndGetUp.h"
#include "TaskSimpleLeaveGroup.h"

void CTaskComplexHitPedWithCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitPedWithCar, 0x86f294, 11);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x6539A0);
    RH_ScopedInstall(Destructor, 0x653A30);

    RH_ScopedGlobalInstall(ComputeEvasiveStepMoveDir, 0x653B40);

    RH_ScopedInstall(HitHurtsPed, 0x653AE0);
    RH_ScopedInstall(CreateSubTask, 0x6560E0);

    RH_ScopedVMTInstall(Clone, 0x6559B0);
    RH_ScopedVMTInstall(GetTaskType, 0x653A20);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657AF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x656300);
    RH_ScopedVMTInstall(ControlSubTask, 0x653A90);

}

// 0x6539A0
CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(CVehicle* veh, float impulseMagnitude) :
    m_Veh{veh},
    m_ImpulseMag{impulseMagnitude}
{
    CEntity::SafeRegisterRef(m_Veh);
}

CTaskComplexHitPedWithCar::CTaskComplexHitPedWithCar(const CTaskComplexHitPedWithCar& o) :
    CTaskComplexHitPedWithCar{o.m_Veh, o.m_ImpulseMag}
{
}

// 0x653A30
CTaskComplexHitPedWithCar::~CTaskComplexHitPedWithCar() {
    CEntity::SafeCleanUpRef(m_Veh);
}

// 0x653B40
CVector CTaskComplexHitPedWithCar::ComputeEvasiveStepMoveDir(const CPed* ped, CVehicle* veh) {
    return CPedGeometryAnalyser::ComputeEntityDir(*veh, (eDirection)CPedGeometryAnalyser::ComputeEntityHitSide(*ped, *veh)); // Same shit, difference packaging
}

// 0x653AE0
bool CTaskComplexHitPedWithCar::HitHurtsPed(CPed* ped) {
    const float threshold = ped->IsPlayer() ? 10.0f : 6.0f;
    if (m_ImpulseMag > threshold) {
        return true;
    }
    return ped->m_vecLastCollisionImpactVelocity.z < -0.8f && m_ImpulseMag > 3.0f;
}

// 0x6560E0
CTask* CTaskComplexHitPedWithCar::CreateSubTask(eTaskType tt) {
    switch (tt) {
    case static_cast<eTaskType>(0x1FA): return new CTaskSimpleKillPedWithCar(m_Veh, m_ImpulseMag);
    case static_cast<eTaskType>(0x1FB): return new CTaskSimpleHurtPedWithCar(m_Veh, m_ImpulseMag);
    case static_cast<eTaskType>(0x1F6): return new CTaskComplexEvasiveStep(m_Veh, m_MoveDir);
    case static_cast<eTaskType>(0x19E): return new CTaskSimpleHitFromBehind;
    case static_cast<eTaskType>(0xD0): {
        const AnimationId anims[]{static_cast<AnimationId>(0x1A), static_cast<AnimationId>(0x19), static_cast<AnimationId>(0x18), static_cast<AnimationId>(0x1B)};
        return new CTaskComplexFallAndGetUp(m_PedHitSide >= 0 && m_PedHitSide < 4 ? anims[m_PedHitSide] : static_cast<AnimationId>(tt), static_cast<AssocGroupId>(0), m_DownTime);
    }
    case static_cast<eTaskType>(0xC8): return new CTaskSimpleLeaveGroup;
    default: return nullptr;
    }
}

// 0x657AF0
CTask* CTaskComplexHitPedWithCar::CreateNextSubTask(CPed* ped) {
    switch (m_pSubTask->GetTaskType()) {
    case static_cast<eTaskType>(0x1FB):
        if (reinterpret_cast<const uint8*>(m_pSubTask)[0x10]) {
            return nullptr;
        }
        return CreateSubTask(static_cast<eTaskType>(0xD0));
    case static_cast<eTaskType>(0x1FA):
    case static_cast<eTaskType>(0x1F6):
    case static_cast<eTaskType>(0xC8):
    case static_cast<eTaskType>(0xD0):
    case static_cast<eTaskType>(0x19E):
        return CreateSubTask(static_cast<eTaskType>(0x516));
    default: return nullptr;
    }
}

// 0x656300
CTask* CTaskComplexHitPedWithCar::CreateFirstSubTask(CPed* ped) {
    m_PedHitSide = CPedGeometryAnalyser::ComputePedHitSide(*ped, *m_Veh);
    const float threshold = ped->IsPlayer() ? 20.0f : 12.0f;
    if (m_ImpulseMag > threshold) {
        return CreateSubTask(static_cast<eTaskType>(0x1FA));
    }
    if (HitHurtsPed(ped)) {
        return CreateSubTask(static_cast<eTaskType>(0x1FB));
    }
    m_MoveDir = ComputeEvasiveStepMoveDir(ped, m_Veh);
    return CreateSubTask(static_cast<eTaskType>(0x1F6));
}

// 0x653A90
CTask* CTaskComplexHitPedWithCar::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
