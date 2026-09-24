/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexHitByGunResponse
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexHitByGunResponse.h"
#include "TaskSimpleHitByGunFromFront.h"
#include "TaskSimpleHitByGunFromLeft.h"
#include "TaskSimpleHitByGunFromRear.h"
#include "TaskSimpleHitByGunFromRight.h"

void CTaskComplexHitByGunResponse::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexHitByGunResponse, 0x86e06c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631DD0);
    RH_ScopedInstall(Destructor, 0x631E30);

    RH_ScopedVMTInstall(Clone, 0x636640);
    RH_ScopedVMTInstall(GetTaskType, 0x631DF0);
    RH_ScopedVMTInstall(MakeAbortable, 0x631E00);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x631E40);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x631E50);
    RH_ScopedVMTInstall(ControlSubTask, 0x631F70);
}

// 0x631DD0
CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(eDirection hs) :
    m_HitSide{hs}
{
}

//! @notsa
CTaskComplexHitByGunResponse::CTaskComplexHitByGunResponse(const CTaskComplexHitByGunResponse& o) :
    CTaskComplexHitByGunResponse{ o.m_HitSide }
{
}

// 0x631E50
CTask* CTaskComplexHitByGunResponse::CreateFirstSubTask(CPed* ped) {
    switch (m_HitSide) {
    case eDirection::FORWARD:  return new CTaskSimpleHitByGunFromFront{};
    case eDirection::LEFT:     return new CTaskSimpleHitByGunFromLeft{};
    case eDirection::BACKWARD: return new CTaskSimpleHitByGunFromRear{};
    case eDirection::RIGHT:    return new CTaskSimpleHitByGunFromRight{};
    default:                   NOTSA_UNREACHABLE();
    }
}
