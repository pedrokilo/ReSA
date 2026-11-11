/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventInWater
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventInWater.h"

namespace {
// Cuerpo no virtual de ~CEventInWater del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall InWaterDestructorBody(CEventInWater* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventInWater::InjectHooks() {
    RH_ScopedVirtualClass(CEventInWater, 0x85B4C8, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(InWaterDestructorBody, "Destructor", 0x4B13C0);

    RH_ScopedInstall(Constructor, 0x4B1370);
    RH_ScopedVMTInstall(AffectsPed, 0x4B13D0);        // lote 166A (banco Fn166A)
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B1420); // lote 166A
}



CEventInWater::CEventInWater(float acceleration) {
    m_acceleration = acceleration;
}

// 0x4B13D0 (lote 166A, banco Fn166A; ranura 5 de la vtable 0x85B4C8; thiscall, un argumento de pila, `ret 4`, 75 bytes). Como el original: tarea activa del gestor de tareas del peaton (CTaskManager::GetActiveTask), luego
// CPed::IsPlayer; si no es el jugador y hay tarea y su tipo (llamada virtual, ranura 4) es TASK_COMPLEX_IN_WATER (0x10C) devuelve falso; en otro caso devuelve CPed::IsAlive.
bool CEventInWater::AffectsPed(CPed* ped) {
    CTask* task = ped->GetTaskManager().GetActiveTask();
    if (!ped->IsPlayer() && task && task->GetTaskType() == TASK_COMPLEX_IN_WATER)
        return false;
    return ped->IsAlive();
}

// 0x4B1420 (lote 166A, banco Fn166A; ranura 11 de la vtable 0x85B4C8; thiscall, un argumento de pila, `ret 4`, 97 bytes). Como el original: consulta el tipo del otro evento hasta tres veces (llamada
// virtual cada vez): EVENT_KNOCK_OFF_BIKE (8) y EVENT_DAMAGE (9) devuelven verdadero sin mirar la aceleracion; EVENT_STUCK_IN_AIR (0x5D) devuelve verdadero si la aceleracion (+0xC) es MAYOR que 1.0f
// (con NaN no); en cualquier otro caso (o si no se cumple) compara las prioridades: la del otro evento (ranura 2) se consulta ANTES que la de este y devuelve prioridad propia >= prioridad ajena (con signo).
bool CEventInWater::TakesPriorityOver(const CEvent& refEvent) {
    if (refEvent.GetEventType() == EVENT_KNOCK_OFF_BIKE) {
        return true;
    }
    if (refEvent.GetEventType() == EVENT_DAMAGE) {
        return true;
    }
    if (refEvent.GetEventType() == EVENT_STUCK_IN_AIR && m_acceleration > 1.0f) {
        return true;
    }
    const int32 otherPriority = refEvent.GetEventPriority();
    return GetEventPriority() >= otherPriority;
}

// 0x4B1370
CEventInWater* CEventInWater::Constructor(float acceleration) {
    this->CEventInWater::CEventInWater(acceleration);
    return this;
}
