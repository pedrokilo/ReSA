/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGangDriveBy
 * Reconstrucción: INCOMPLETA — 1/4 funciones reconstruidas.
 * Pendientes de reconstruir: 3.
 * Funciones bloqueadas: 3.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * PENDIENTES (nombre | dirección | tipo):
 *   Clone | 0x006236D0 | V
 *   MakeAbortable | 0x0062D290 | V
 *   ProcessPed | 0x0062D3B0 | V
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x006236D0 | V
 *   MakeAbortable | 0x0062D290 | V
 *   ProcessPed | 0x0062D3B0 | V
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGangDriveBy.h"

void CTaskSimpleGangDriveBy::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGangDriveBy, 0x86D944, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(GetTaskType, 0x6218B0);
    // Stubs hacia el original: activarlos provocaría recursión infinita.
    RH_ScopedVMTInstall(Clone, 0x6236D0, { .reversed = false });
    RH_ScopedVMTInstall(MakeAbortable, 0x62D290, { .reversed = false });
    RH_ScopedVMTInstall(ProcessPed, 0x62D3B0, { .reversed = false });
}

CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS) {
    m_bSeatRHS             = seatRHS;
    m_nDrivebyStyle        = drivebyStyle;
    m_fAbortRange          = abortRange;
    m_pTargetEntity        = target;
    m_nFrequencyPercentage = frequencyPercentage;
    m_bIsFinished          = false;
    m_bAnimsReferenced     = false;
    m_bInRangeToShoot      = false;
    m_bInWeaponRange       = false;
    m_bReachedAbortRange   = false;
    m_bFromScriptCommand   = false;
    m_nBurstShots          = -1;
    m_nFakeShootDirn       = -1;
    m_nAttackTimer         = -1;
    m_nLastCommand         = 0;
    m_nNextCommand         = 1;
    m_nLOSCheckTime        = 0;
    m_nLOSBlocked          = true;
    m_pAnimAssoc           = nullptr;
    m_nRequiredAnimID      = ANIM_ID_NO_ANIMATION_SET;
    m_nRequiredAnimGroup   = ANIM_GROUP_DEFAULT;
    m_pWeaponInfo          = nullptr;
    CEntity::SafeRegisterRef(m_pTargetEntity);
    if (targetPos) {
        m_vecCoords = *targetPos;
    }
}

CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy()
{
    if (m_bAnimsReferenced)
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_nRequiredAnimGroup));

    if (m_pAnimAssoc)
        m_pAnimAssoc->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);

    CEntity::SafeCleanUpRef(m_pTargetEntity);
}

CTask* CTaskSimpleGangDriveBy::Clone() const {
    return plugin::CallMethodAndReturn<CTask*, 0x6236D0, const CTask*>(this);
}

bool CTaskSimpleGangDriveBy::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event)
{
    return plugin::CallMethodAndReturn<bool, 0x62D290, CTask*, CPed*, int32, const CEvent*>(this, ped, priority, event);
}

bool CTaskSimpleGangDriveBy::ProcessPed(CPed* ped)
{
    return plugin::CallMethodAndReturn<bool, 0x62D3B0, CTask*, CPed*>(this, ped);
}
