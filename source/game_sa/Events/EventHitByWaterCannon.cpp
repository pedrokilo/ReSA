/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventHitByWaterCannon
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventHitByWaterCannon.h"

namespace {
// Cuerpo no virtual de ~CEventHitByWaterCannon del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall HitByWaterCannonDestructorBody(CEventHitByWaterCannon* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace


void CEventHitByWaterCannon::InjectHooks()
{
    RH_ScopedVirtualClass(CEventHitByWaterCannon, 0x85B488, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(HitByWaterCannonDestructorBody, "Destructor", 0x4B1320);

    RH_ScopedInstall(Constructor, 0x4B1290);
    RH_ScopedVMTInstall(AffectsPed, 0x4B1330);
    RH_ScopedVMTInstall(GetEventType, 0x4B12E0);        // lote 166A (banco Fn166A)
    RH_ScopedVMTInstall(GetLifeTime, 0x4B12F0);         // lote 166A
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x4B1300);  // lote 166A
    RH_ScopedVMTInstall(GetEventPriority, 0x4B1310);    // lote 166A
}

// 0x4B12E0 (lote 166A, banco Fn166A; ranura 1 de la vtable 0x85B488; thiscall, `ret`): `mov eax, 0x40` (EVENT_WATER_CANNON = 64).
eEventType CEventHitByWaterCannon::GetEventType() const {
    return EVENT_WATER_CANNON;
}

// 0x4B1310 (lote 166A; ranura 2): `mov eax, 0x40` (prioridad 64).
int32 CEventHitByWaterCannon::GetEventPriority() const {
    return 64;
}

// 0x4B12F0 (lote 166A; ranura 3): `xor eax, eax` (vida util 0).
int32 CEventHitByWaterCannon::GetLifeTime() {
    return 0;
}

// 0x4B1300 (lote 166A; ranura 12): `fld dword [0x85ADBC]` (55.0f); `ret`. El resultado queda en ST0.
float CEventHitByWaterCannon::GetLocalSoundLevel() {
    return 55.0f;
}

// 0x4B1290
CEventHitByWaterCannon::CEventHitByWaterCannon(const CVector& point, const CVector& moveSpeed)
{
    m_point = point;
    m_moveSpeed = moveSpeed;
}

// 0x4B1290
CEventHitByWaterCannon* CEventHitByWaterCannon::Constructor(const CVector& point, const CVector& moveSpeed)
{
    this->CEventHitByWaterCannon::CEventHitByWaterCannon(point, moveSpeed);
    return this;
}

// 0x4B1330
bool CEventHitByWaterCannon::AffectsPed(CPed* ped)
{
    if (ped->IsAlive()) {
        CTask* task = ped->GetTaskManager().GetActiveTask();
        if (!task || task->GetTaskType() != TASK_COMPLEX_FALL_AND_GET_UP)
            return true;
    }
    return false;
}