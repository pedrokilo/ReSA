/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleWaitForBus
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
#include "TaskSimpleWaitForBus.h"

void CTaskSimpleWaitForBus::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleWaitForBus, 0x86e1b8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x632A50);
    RH_ScopedInstall(Destructor, 0x632A90);

    RH_ScopedVMTInstall(Clone, 0x636AD0);
    RH_ScopedVMTInstall(GetTaskType, 0x632A70);
    RH_ScopedVMTInstall(MakeAbortable, 0x632A80);
    RH_ScopedVMTInstall(ProcessPed, 0x632AA0);
}

CTaskSimpleWaitForBus::CTaskSimpleWaitForBus(const CTaskSimpleWaitForBus& o) :
    CTaskSimpleWaitForBus{}
{
}

// 0x632AA0
bool CTaskSimpleWaitForBus::ProcessPed(CPed* ped) {
    // Como el original: m_bus solo se escribe al encontrar un autobús; si no hay ninguno devuelve false
    for (auto* const entity : ped->GetIntelligence()->GetVehicleScanner().m_apEntities) {
        const auto veh = static_cast<CVehicle*>(entity);
        if (!veh || !veh->vehicleFlags.bIsBus) {
            continue;
        }
        // Como el original: primero la velocidad (comparación estricta) y después las plazas libres
        if (!(sq(0.005f) > veh->m_vecMoveSpeed.SquaredMagnitude())) {
            continue;
        }
        if (veh->HasSpaceForAPassenger()) {
            m_bus = veh;
            return true;
        }
    }
    return false;
}
