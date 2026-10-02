/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWalkRoundObject
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

#include "TaskComplexWalkRoundObject.h"
// #include "PointRoute.h"

void CTaskComplexWalkRoundObject::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWalkRoundObject, 0x86F364, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x655020);
}

// 0x655020
CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(int32 moveState, const CVector& targetPoint, CEntity* object) : CTaskComplex() {
    m_moveState   = moveState;
    m_targetPoint = targetPoint;
    m_object      = object;
    field_24      = 0;
    field_28      = 0;
    field_2C      = 0;
    field_2D      = 0;

    CEntity::SafeRegisterRef(m_object);

    // Como el original: CPointRoute::operator new (0x41B7C0) y solo se pone a cero el contador;
    // `new CPointRoute()` inicializaba por valor los 0x60 bytes de puntos
    auto* route = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (route) {
        route->m_NumEntries = 0;
    }
    m_pointRoute = route;
}

CTaskComplexWalkRoundObject::~CTaskComplexWalkRoundObject() {
    CEntity::SafeCleanUpRef(m_object);

    delete m_pointRoute;
    // todo: m_pointRoute = nullptr;
}

CTaskComplexWalkRoundObject* CTaskComplexWalkRoundObject::Constructor(int32 moveState, const CVector& targetPoint, CEntity* object) {
    this->CTaskComplexWalkRoundObject::CTaskComplexWalkRoundObject(moveState, targetPoint, object);
    return this;
}

CTask* CTaskComplexWalkRoundObject::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6575F0, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}

CTask* CTaskComplexWalkRoundObject::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x657220, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}

CTask* CTaskComplexWalkRoundObject::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x657380, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}

CTask* CTaskComplexWalkRoundObject::CreateRouteTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x655140, CTaskComplexWalkRoundObject*, CPed*>(this, ped);
}
