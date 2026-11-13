/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventInAir
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventInAir.h"

namespace {
// Cuerpo no virtual de ~CEventInAir del EXE (thiscall, sin argumentos de pila, `ret`): solo deja la vtable de CEvent del EXE (0x85AB38) (la clase no libera referencias). (Lote 119A, banco Fn119A.)
void __fastcall InAirDestructorBody(CEventInAir* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace


void CEventInAir::InjectHooks()
{
    RH_ScopedVirtualClass(CEventInAir, 0x85B3C0, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(InAirDestructorBody, "Destructor", 0x4B0D00);

    RH_ScopedInstall(Constructor, 0x4B0CB0);
    RH_ScopedVMTInstall(GetEventType, 0x4B0CD0);     // lote 137A (banco Fn137A)
    RH_ScopedVMTInstall(GetLifeTime, 0x4B0CE0);      // lote 137A
    RH_ScopedVMTInstall(GetEventPriority, 0x4B0CF0); // lote 137A
    RH_ScopedVMTInstall(AffectsPed, 0x4B0C00);
    RH_ScopedVMTInstall(Clone, 0x4B76A0);            // lote 187A (sin validar): ranura 4
}

// 0x4B76A0 (lote 187A, sin validar; el listado decia CEventAttractor; thiscall, sin argumentos, `ret`, 33 bytes sin marco SEH; ranura 4 de la vtable 0x85B3C0). Como el original: CEvent::operator new (0x4B5620; el EXE
// lleva en linea el CEventPool::New 0x4B5570); sin memoria devuelve nulo; si no, construye en linea: m_nTimeActive (+4) = 0, m_bValid (BYTE +8) = 0 y la vtable (0x85B3C0), y devuelve la memoria. El constructor (0x4B0CB0,
// con hook) hace lo mismo, asi que se usa.
CEvent* CEventInAir::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventInAir));
    if (!memory) {
        return nullptr;
    }
    // Correccion hecha al validar (lote 187A): el EXE construye en linea (+4 = 0, BYTE +8 = 0, vtable 0x85B3C0 del EXE) y NO llama al constructor; el constructor de C++ dejaria la vtable de la DLL.
    auto* const bytes = static_cast<uint8*>(memory);
    *reinterpret_cast<uint32*>(bytes + 4) = 0;
    bytes[8] = 0;
    *reinterpret_cast<uint32*>(bytes) = 0x85B3C0;
    return static_cast<CEvent*>(memory);
}

// 0x4B0CB0
CEventInAir* CEventInAir::Constructor()
{
    this->CEventInAir::CEventInAir();
    return this;
}

// 0x4B0C00
bool CEventInAir::AffectsPed(CPed* ped)
{
    CTask* activeTask = ped->GetTaskManager().GetActiveTask();
    if (activeTask
        && (activeTask->GetTaskType() == TASK_COMPLEX_USE_SWAT_ROPE
            || activeTask->GetTaskType() == TASK_COMPLEX_DIVE_FROM_ATTACHED_ENTITY_AND_GET_UP
            ||
                       activeTask->GetTaskType() == TASK_COMPLEX_IN_AIR_AND_LAND))
    {
        return false;
    }
    CTask* simplestActiveTask = ped->GetTaskManager().GetSimplestActiveTask();
    if (simplestActiveTask && simplestActiveTask->GetTaskType() == TASK_SIMPLE_FALL ||
        ped->m_pDamageEntity ||
        !ped->GetUsesCollision() ||
        ped->m_pAttachedTo ||
        !CPedGeometryAnalyser::IsInAir(*ped)
    ) {
        return false;
    }
    return ped->IsAlive();
}
