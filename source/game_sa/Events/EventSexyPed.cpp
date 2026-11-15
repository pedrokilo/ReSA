/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventSexyPed
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventSexyPed.h"

namespace {
// Cuerpo no virtual de ~CEventSexyPed del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventSexyPed del EXE (0x85B0B0), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 119A, banco Fn119A.)
void __fastcall SexyPedDestructorBody(CEventSexyPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B0B0;
    CEntity::SafeCleanUpRef(self->m_SexyPed);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
} // namespace

void CEventSexyPed::InjectHooks() {
    RH_ScopedVirtualClass(CEventSexyPed, 0x85B0B0, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(SexyPedDestructorBody, "Destructor", 0x4AEEA0);

    RH_ScopedInstall(Constructor, 0x4AEDF0);
    RH_ScopedVMTInstall(AffectsPed, 0x4AEF00);    // lote 199A (sin validar): ranura 5
    RH_ScopedVMTInstall(CloneEditable, 0x4B7280); // lote 199A (sin validar): ranura 16
}



CEventSexyPed::CEventSexyPed(CPed* ped) : CEventEditableResponse() {
    m_SexyPed = ped;
    CEntity::SafeRegisterRef(m_SexyPed);
}

CEventSexyPed::CEventSexyPed(CPed* sexyPed, eTaskType taskType) :
    CEventSexyPed{sexyPed}
{
    m_TaskId = taskType;
}

CEventSexyPed::~CEventSexyPed() {
    CEntity::SafeCleanUpRef(m_SexyPed);
}

// 0x4AEF00 (lote 199A, sin validar; thiscall, un argumento de pila, `ret 4`, 272 bytes; ranura 5 de la vtable 0x85B0B0). En este orden y cortando en el primer fallo (devuelve false): CPed::IsAlive (0x5E0170) del peaton,
// peaton atractivo (+0x14) no nulo y vivo, y que el peaton NO este mirando (IKChainManager_c::IsLooking 0x6181A0) al atractivo (GetLookAtEntity 0x6181D0 igual a +0x14). Despues busca en el gestor de tareas del peaton
// (CTaskManager::FindActiveTaskByType 0x681740, cuyo gestor es +4 de la inteligencia +0x47C) la primera tarea entre TASK_COMPLEX_PARTNER_DEAL (0x4B2), TASK_COMPLEX_BE_IN_COUPLE (0x4BF) y TASK_COMPLEX_PARTNER_GREET (0x4B3),
// en ese orden y parando en la primera; si no hay, devuelve true. Repite la busqueda en el gestor del atractivo; si no hay, true. Por ultimo llama a GetTaskType (ranura +0x10) de la tarea del atractivo PRIMERO y despues a
// la del peaton; si los tipos son iguales devuelve false, si no true.
bool CEventSexyPed::AffectsPed(CPed* ped) {
    if (!ped->IsAlive())
        return false;

    if (!m_SexyPed)
        return false;

    if (!m_SexyPed->IsAlive())
        return false;

    if (g_ikChainMan.IsLooking(ped) && g_ikChainMan.GetLookAtEntity(ped) == m_SexyPed) {
        return false;
    }

    CTask* const pedTask = ped->GetTaskManager().Find<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_BE_IN_COUPLE, TASK_COMPLEX_PARTNER_GREET>();
    if (!pedTask) {
        return true;
    }
    CTask* const sexyTask = m_SexyPed->GetTaskManager().Find<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_BE_IN_COUPLE, TASK_COMPLEX_PARTNER_GREET>();
    if (!sexyTask) {
        return true;
    }
    const eTaskType sexyTaskType = sexyTask->GetTaskType(); // el EXE pide primero el tipo de la tarea del atractivo
    const eTaskType pedTaskType = pedTask->GetTaskType();
    return sexyTaskType != pedTaskType;
}

// 0x4B7280 (lote 199A, sin validar; thiscall, sin argumentos, `ret`, 97 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x85B0B0).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x18 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AEDF0 (Constructor, con hook) con el atractivo (+0x14) y devuelve la memoria.
CEventEditableResponse* CEventSexyPed::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventSexyPed));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventSexyPed*>(memory)->Constructor(m_SexyPed);
}

// 0x4AEDF0
CEventSexyPed* CEventSexyPed::Constructor(CPed* ped) {
    this->CEventSexyPed::CEventSexyPed(ped);
    return this;
}
