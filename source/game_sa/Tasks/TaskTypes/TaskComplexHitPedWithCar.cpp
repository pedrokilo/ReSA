/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexHitPedWithCar
 * Reconstrucción: INCOMPLETA — 2/10 funciones reconstruidas.
 * Pendientes de reconstruir: 8.
 * Funciones bloqueadas: 8.
 * Validación registrada: 0/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x006559B0 | V
 *   ComputeEvasiveStepMoveDir | 0x00653B40 | S
 *   ControlSubTask | 0x00653A90 | V
 *   CreateFirstSubTask | 0x00656300 | V
 *   CreateNextSubTask | 0x00657AF0 | V
 *   CreateSubTask | 0x006560E0 | S
 *   GetTaskType | 0x00653A20 | V
 *   HitHurtsPed | 0x00653AE0 | S
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

void CTaskComplexHitPedWithCar::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitPedWithCar, 0x86f294, 11);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x6539A0);
    RH_ScopedInstall(Destructor, 0x653A30);

    RH_ScopedGlobalInstall(ComputeEvasiveStepMoveDir, 0x653B40, { .reversed = false });

    RH_ScopedInstall(HitHurtsPed, 0x653AE0, { .reversed = false });
    RH_ScopedInstall(CreateSubTask, 0x6560E0, { .reversed = false });

    RH_ScopedVMTInstall(Clone, 0x6559B0, { .reversed = false });
    RH_ScopedVMTInstall(GetTaskType, 0x653A20, { .reversed = false });
    RH_ScopedVMTInstall(CreateNextSubTask, 0x657AF0, { .reversed = false });
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x656300, { .reversed = false });
    RH_ScopedVMTInstall(ControlSubTask, 0x653A90, { .reversed = false });

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
    return plugin::CallMethodAndReturn<bool, 0x653AE0, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}

// 0x6560E0
CTask* CTaskComplexHitPedWithCar::CreateSubTask(eTaskType tt) {
    return plugin::CallMethodAndReturn<CTask*, 0x6560E0, CTaskComplexHitPedWithCar*, eTaskType>(this, tt);
}

// 0x657AF0
CTask* CTaskComplexHitPedWithCar::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x657AF0, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}

// 0x656300
CTask* CTaskComplexHitPedWithCar::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x656300, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}

// 0x653A90
CTask* CTaskComplexHitPedWithCar::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x653A90, CTaskComplexHitPedWithCar*, CPed*>(this, ped);
}
