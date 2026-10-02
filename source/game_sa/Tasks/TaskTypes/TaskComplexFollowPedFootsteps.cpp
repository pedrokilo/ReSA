/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexFollowPedFootsteps
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexFollowPedFootsteps.h"
// #include "PointRoute.h"

void CTaskComplexFollowPedFootsteps::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexFollowPedFootsteps, 0x870CC0, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x694E20);
}

// 0x694E20
CTaskComplexFollowPedFootsteps::CTaskComplexFollowPedFootsteps(CPed* ped) : CTaskComplex() {
    m_targetPed               = ped;
    m_updateGoToPoint         = false;
    m_subTaskCreateCheckTimer = CTimer::GetTimeInMS();
    m_lineOfSightCheckTimer   = 0;
    m_pointRoute              = nullptr;
    m_moveState               = PEDMOVE_WALK;

    CEntity::SafeRegisterRef(m_targetPed);

    // Como el original: CPointRoute::operator new (0x41B7C0) y solo se pone a cero el contador (dos veces, constructor
    // en línea y limpieza explícita); `new CPointRoute()` inicializaba por valor los 0x60 bytes de puntos
    auto* route = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (route) {
        route->m_NumEntries = 0;
    }
    m_pointRoute = route;
    if (m_pointRoute) {
        m_pointRoute->m_NumEntries = 0;
    }
}

CTaskComplexFollowPedFootsteps* CTaskComplexFollowPedFootsteps::Constructor(CPed* ped) {
    this->CTaskComplexFollowPedFootsteps::CTaskComplexFollowPedFootsteps(ped);
    return this;
}

CTaskComplexFollowPedFootsteps::~CTaskComplexFollowPedFootsteps() {
    CEntity::SafeCleanUpRef(m_targetPed);

    delete m_pointRoute;
    m_pointRoute = nullptr;
}

bool CTaskComplexFollowPedFootsteps::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return plugin::CallMethodAndReturn<bool, 0x694ED0, CTaskComplexFollowPedFootsteps*, CPed*, eAbortPriority, const CEvent*>(this, ped, priority, event);
}

CTask* CTaskComplexFollowPedFootsteps::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x694EE0, CTaskComplexFollowPedFootsteps*, CPed*>(this, ped);
}

CTask* CTaskComplexFollowPedFootsteps::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x695000, CTaskComplexFollowPedFootsteps*, CPed*>(this, ped);
}

CTask* CTaskComplexFollowPedFootsteps::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x695090, CTaskComplexFollowPedFootsteps*, CPed*>(this, ped);
}

CTask* CTaskComplexFollowPedFootsteps::CreateSubTask(eTaskType taskType, CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x695E40, CTaskComplexFollowPedFootsteps*, eTaskType, CPed*>(this, taskType, ped);
}
