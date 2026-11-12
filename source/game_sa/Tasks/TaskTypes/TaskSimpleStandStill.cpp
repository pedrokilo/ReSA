/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleStandStill
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleStandStill.h"
#include "TaskSimpleDuck.h"

namespace CTaskSimpleStandStillBody {
// 0x62F360 (thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CTaskSimpleStandStill que usan los destructores del EXE y de sus subclases. Deja la vtable de
// CTaskSimpleStandStill del EXE (0x86DD2C) y salta a CTask::Destructor (0x61A3A0), que la cambia a la de CTask (0x86D48C). No toca mas memoria; los llamadores solo conservan los
// registros no volatiles. __fastcall con un segundo registro sin usar equivale a thiscall sin argumentos de pila (lote 106A, banco Fn106A).
void __fastcall StandStillDestructorBody(CTaskSimpleStandStill* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DD2C;
    self->CTask::Destructor();
}
} // namespace CTaskSimpleStandStillBody

using CTaskSimpleStandStillBody::StandStillDestructorBody;

void CTaskSimpleStandStill::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleStandStill, 0x86DD2C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x62F310);
    RH_ScopedNamedGlobalInstall(StandStillDestructorBody, "Destructor", 0x62F360);
    RH_ScopedVMTInstall(GetTaskType, 0x62F350); // lote 171A (banco Fn171A): retorno de constante
    RH_ScopedVMTInstall(MakeAbortable, 0x4B8690);
    RH_ScopedVMTInstall(ProcessPed, 0x62F370);
}

// 0x62F310
CTaskSimpleStandStill::CTaskSimpleStandStill(int32 nTime, bool Looped, bool bUseAnimIdleStance, float fBlendData) {
    m_nTime = nTime;
    m_fBlendData = fBlendData;
    m_bLooped = Looped;
    m_bUseAnimIdleStance = bUseAnimIdleStance;
}

// 0x62F350 (lote 171A, banco Fn171A; thiscall, `ret`, 6 bytes: `mov eax,0CBh`): TASK_SIMPLE_STAND_STILL (203); no lee el objeto (ranura 4 de la vtable 0x86DD2C).
eTaskType CTaskSimpleStandStill::GetTaskType() const {
    static_assert(Type == 0xCB);
    return Type;
}

// 0x4B8690
bool CTaskSimpleStandStill::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority)
        return true;
    m_timer.m_nStartTime = CTimer::GetTimeInMS();
    m_timer.m_nInterval = -1;
    m_timer.m_bStarted = true;
    return true;
}

// 0x62F370
bool CTaskSimpleStandStill::ProcessPed(CPed* ped) {
    if (!m_timer.m_bStarted && m_timer.Start(m_nTime)) {
        if (!ped->bInVehicle) {
            ped->SetMoveState(PEDMOVE_STILL);
            ped->m_nSwimmingMoveState = PEDMOVE_STILL;
            if (!ped->bIsDucking || !ped->GetIntelligence()->GetTaskDuck(false)) {
                CAnimManager::BlendAnimation(ped->GetRpClump(), ped->m_nAnimGroup, ANIM_ID_IDLE, m_fBlendData);
            } else {
                CTaskSimpleDuck* pDuckTask = ped->GetIntelligence()->GetTaskDuck(false);
                pDuckTask->ControlDuckMove();
            }
            if (ped->GetPlayerData())
                ped->GetPlayerData()->m_fMoveBlendRatio = 0.0f;
        }
    }

    if (ped->bIsDucking && ped->GetIntelligence()->GetTaskDuck(false)) {
        CTaskSimpleDuck* pDuckTask = ped->GetIntelligence()->GetTaskDuck(false);
        pDuckTask->ControlDuckMove();
    } else {
        ped->SetMoveState(PEDMOVE_STILL);
    }

    if (m_bUseAnimIdleStance) {
        auto pIdleAnimAssoc = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_IDLE);
        if (pIdleAnimAssoc && pIdleAnimAssoc->m_BlendAmount > 0.99f)
            return true;
    }

    if (m_bLooped || !m_timer.Reset())
        return false;

    return m_timer.IsOutOfTime();
}