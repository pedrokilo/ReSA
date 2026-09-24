/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskGangHassleVehicle
 * Reconstrucción: INCOMPLETA — 2/8 funciones reconstruidas.
 * Pendientes de reconstruir: 6.
 * Funciones bloqueadas: 6.
 * Evidencia aprobada vinculada: 0/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   CalcTargetOffset | 0x006641A0 | S
 *   Clone | 0x0065FC00 | S
 *   ControlSubTask | 0x006637C0 | S
 *   CreateFirstSubTask | 0x00664BA0 | S
 *   CreateNextSubTask | 0x0065FC80 | S
 *   GetTargetHeading | 0x0065FDD0 | S
 * BLOQUEADAS (nombre | dirección | tipo):
 *   CalcTargetOffset | 0x006641A0 | S
 *   Clone | 0x0065FC00 | S
 *   ControlSubTask | 0x006637C0 | S
 *   CreateFirstSubTask | 0x00664BA0 | S
 *   CreateNextSubTask | 0x0065FC80 | S
 *   GetTargetHeading | 0x0065FDD0 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskGangHassleVehicle.h"

void CTaskGangHassleVehicle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskGangHassleVehicle, 0x86F9D4, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65FAC0);
    RH_ScopedInstall(Destructor, 0x65FB60);
    RH_ScopedInstall(GetTargetHeading, 0x65FDD0, { .reversed = false });
    RH_ScopedInstall(CalcTargetOffset, 0x6641A0, { .reversed = false });
    RH_ScopedInstall(Clone, 0x65FC00, { .reversed = false });
    RH_ScopedInstall(CreateNextSubTask, 0x65FC80, { .reversed = false });
    RH_ScopedInstall(CreateFirstSubTask, 0x664BA0, { .reversed = false });
    RH_ScopedInstall(ControlSubTask, 0x6637C0, { .reversed = false });
}

// 0x65FAC0
CTaskGangHassleVehicle::CTaskGangHassleVehicle(CVehicle* vehicle, int32 a3, uint8 a4, float a5, float a6) : CTaskComplex() {
    m_nTime = 0;
    dword3C = 0;
    byte40 = 0;
    byte41 = 0;
    byte18 = a4;
    dword1C = a5;
    m_Vehicle = vehicle;
    m_nHasslePosId = -1;
    m_fOffsetX = a6;
    m_bRemoveAnim = 0;
    m_pEntity = nullptr;
    CEntity::SafeRegisterRef(m_Vehicle);
}

// 0x65FB60
CTaskGangHassleVehicle::~CTaskGangHassleVehicle() {
    if (m_Vehicle) {
        if (m_nHasslePosId > -1) {
            m_Vehicle->SetHasslePosId(m_nHasslePosId, false);
        }
        CEntity::SafeCleanUpRef(m_Vehicle);
    }

    CEntity::SafeCleanUpRef(m_pEntity);

    if (m_bRemoveAnim) {
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
        m_bRemoveAnim = false;
    }
}

// 0x65FDD0
float CTaskGangHassleVehicle::GetTargetHeading(CPed* ped) {
    return plugin::CallMethodAndReturn<float, 0x65FDD0, CTaskGangHassleVehicle*, CPed*>(this, ped);
}

// 0x6641A0
void CTaskGangHassleVehicle::CalcTargetOffset() {
    plugin::CallMethod<0x6641A0, CTaskGangHassleVehicle*>(this);
}

// 0x65FC80
CTask* CTaskGangHassleVehicle::CreateNextSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x65FC80, CTaskGangHassleVehicle*, CPed*>(this, ped);
}

// 0x664BA0
CTask* CTaskGangHassleVehicle::CreateFirstSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x664BA0, CTaskGangHassleVehicle*, CPed*>(this, ped);
}

// 0x6637C0
CTask* CTaskGangHassleVehicle::ControlSubTask(CPed* ped) {
    return plugin::CallMethodAndReturn<CTask*, 0x6637C0, CTaskGangHassleVehicle*, CPed*>(this, ped);
}
