/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexDrivePointRoute
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexDrivePointRoute.h"
#include "TaskComplexDriveToPoint.h"

void CTaskComplexDrivePointRoute::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexDrivePointRoute, 0x86ebf8, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6433E0);
    RH_ScopedInstall(Destructor, 0x643490);

    RH_ScopedVMTInstall(Clone, 0x6439C0);
    RH_ScopedVMTInstall(GetTaskType, 0x643480);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x63CF90);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x63D030);
    RH_ScopedVMTInstall(ControlSubTask, 0x63D0C0);
}

// 0x6433E0
CTaskComplexDrivePointRoute::CTaskComplexDrivePointRoute(CVehicle* vehicle, CPointRoute const& route, float cruiseSpeed, uint32 mode, eModelID carModelIdx, float targetRadius, eCarDrivingStyle drivingStyle) :
    m_mode{mode},
    m_veh{vehicle},
    m_cruiseSpeed{cruiseSpeed},
    m_desiredCarModel{carModelIdx},
    m_targetRadius{targetRadius},
    m_drivingStyle{drivingStyle}
{
    // Como el original: CPointRoute::operator new (0x41B7C0) y el constructor copia en línea 0x63A040, que solo copia
    // las entradas usadas (bucle con signo); `new CPointRoute{route}` copiaba los 8 puntos
    auto* const newRoute = static_cast<CPointRoute*>(CPointRoute::operator new(sizeof(CPointRoute)));
    if (newRoute) {
        newRoute->m_NumEntries = route.m_NumEntries;
        for (int32 i = 0; i < (int32)newRoute->m_NumEntries; i++) {
            newRoute->m_Entries.data()[i] = route.m_Entries.data()[i];
        }
    }
    m_route = newRoute;
    // CEntity::SafeRegisterRef(m_veh); // NOTE: Missing from OG code
}

CTaskComplexDrivePointRoute::CTaskComplexDrivePointRoute(const CTaskComplexDrivePointRoute& o) :
    CTaskComplexDrivePointRoute{
        o.m_veh,
       *o.m_route,
        o.m_cruiseSpeed,
        o.m_mode,
        o.m_desiredCarModel,
        o.m_targetRadius,
        o.m_drivingStyle
    }
{
}

// 0x643490
CTaskComplexDrivePointRoute::~CTaskComplexDrivePointRoute() {
    // Como el original (0x6434B6): solo se libera si no es nula (`delete nullptr` llegaba a CPool::Delete, que ya no
    // comprueba nullptr y abortaba en IsPtrFromPool)
    if (m_route) {
        delete m_route;
    }
    // Also no unregister ref here.. probably delibarate?
}

CTask* CTaskComplexDrivePointRoute::CreateTaskForCurrentPoint() const {
    // Como el original: se indexa el array crudo (el índice puede superar m_NumEntries; la vista `take` abortaba en debug)
    return new CTaskComplexDriveToPoint{ m_veh, m_route->m_Entries.data()[m_currPtIdx], m_cruiseSpeed, (int32)m_mode, m_desiredCarModel, m_targetRadius, m_drivingStyle };
}

// 0x63CF90
CTask* CTaskComplexDrivePointRoute::CreateNextSubTask(CPed* ped) {
    if (++m_currPtIdx == m_route->m_NumEntries) { // Como el original: se compara el índice ya incrementado
        return nullptr;
    }

    return CreateTaskForCurrentPoint();
}
