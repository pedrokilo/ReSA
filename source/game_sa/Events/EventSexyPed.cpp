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

    if (ped->GetTaskManager().IsFirstFoundTaskMatching<TASK_COMPLEX_PARTNER_DEAL, TASK_COMPLEX_BE_IN_COUPLE, TASK_COMPLEX_PARTNER_GREET>(m_SexyPed->GetTaskManager())) {
        return false;
    }

    return true;
}

// 0x4AEDF0
CEventSexyPed* CEventSexyPed::Constructor(CPed* ped) {
    this->CEventSexyPed::CEventSexyPed(ped);
    return this;
}
