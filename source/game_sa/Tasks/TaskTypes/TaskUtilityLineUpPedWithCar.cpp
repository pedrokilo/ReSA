/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskUtilityLineUpPedWithCar
 * Reconstrucción: INCOMPLETA — 2/5 funciones reconstruidas.
 * Pendientes de reconstruir: 3.
 * Funciones bloqueadas: 3.
 * Evidencia aprobada vinculada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   GetLocalPositionToOpenCarDoor | 0x0064FC10 | S
 *   GetPositionToOpenCarDoor | 0x00650A80 | S
 *   ProcessPed | 0x006513A0 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   GetLocalPositionToOpenCarDoor | 0x0064FC10 | S
 *   GetPositionToOpenCarDoor | 0x00650A80 | S
 *   ProcessPed | 0x006513A0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskUtilityLineUpPedWithCar.h"

void CTaskUtilityLineUpPedWithCar::InjectHooks() {
    RH_ScopedClass(CTaskUtilityLineUpPedWithCar);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x64FBB0);
    RH_ScopedInstall(Destructor, 0x64FC00);

    RH_ScopedInstall(GetLocalPositionToOpenCarDoor, 0x64FC10, { .reversed = false });
    RH_ScopedInstall(GetPositionToOpenCarDoor, 0x650A80, { .reversed = false });
    RH_ScopedInstall(ProcessPed, 0x6513A0, { .reversed = false });
}

// 0x64FBB0
CTaskUtilityLineUpPedWithCar::CTaskUtilityLineUpPedWithCar(const CVector& offset, int32 time, int32 doorOpenPosType, int32 doorIdx) {
    m_Offset = offset;
    m_fDoorOpenPosZ = -999.99f;
    m_fTime = time;
    m_nDoorOpenPosType = doorOpenPosType;
    m_nDoorIdx = doorIdx;
}

// The following 2 functions seem to have copy elision on the returned CVector, that the compiled functions
// took a vector ptr as their first arg. Now, hopefully our code will compile to the same stuff.
// If not, it might crash here, in that case a wrapper function should be used.

// 0x64FC10
CVector CTaskUtilityLineUpPedWithCar::GetLocalPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    CVector out;
    plugin::CallMethodAndReturn<CVector, 0x64FC10, CTaskUtilityLineUpPedWithCar*, CVector*, CVehicle*, float, CAnimBlendAssociation*>(this, &out, vehicle, animProgress, assoc);
    return out;
}

// 0x650A80
CVector CTaskUtilityLineUpPedWithCar::GetPositionToOpenCarDoor(CVehicle* vehicle, float animProgress, CAnimBlendAssociation* assoc) {
    CVector out;
    plugin::CallMethodAndReturn<CVector, 0x650A80, CTaskUtilityLineUpPedWithCar*, CVector*, CVehicle*, float, CAnimBlendAssociation*>(this, &out, vehicle, animProgress, assoc);
    return out;
}

// 0x6513A0
bool CTaskUtilityLineUpPedWithCar::ProcessPed(CPed* ped, CVehicle* vehicle, CAnimBlendAssociation* assoc) {
    return plugin::CallMethodAndReturn<bool, 0x6513A0, CTaskUtilityLineUpPedWithCar*, CPed*, CVehicle*, CAnimBlendAssociation*>(this, ped, vehicle, assoc);
}
