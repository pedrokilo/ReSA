/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleTurn180
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleTurn180.h"

void CTaskSimpleTurn180::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleTurn180, 0x86e01c, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x631CC0);
    RH_ScopedInstall(Destructor, 0x631D00);

    RH_ScopedVMTInstall(Clone, 0x636570);
    RH_ScopedVMTInstall(GetTaskType, 0x631CE0);
    RH_ScopedVMTInstall(MakeAbortable, 0x631CF0);
    RH_ScopedVMTInstall(ProcessPed, 0x639990);
}

// NOTSA
CTaskSimpleTurn180::CTaskSimpleTurn180(const CTaskSimpleTurn180& o) :
    CTaskSimpleTurn180{}
{
}

// 0x631D00
CTaskSimpleTurn180::~CTaskSimpleTurn180() {
    if (m_anim) {
        m_anim->SetDefaultFinishCallback();
    }
}

void CTaskSimpleTurn180::FinishAnimTurn180CB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleTurn180*>(data);
    self->m_animHasFinished = true;
    self->m_anim = nullptr;
}

// 0x639990
bool CTaskSimpleTurn180::ProcessPed(CPed* ped) {
    if (m_animHasFinished) {
        ped->RestoreHeadingRate();
        return true;
    }

    if (!m_anim) {
        ped->m_fHeadingChangeRate = 0.f;
        m_anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_TURN_180, 4.f);
        m_anim->SetFinishCallback(FinishAnimTurn180CB, this);
    }

    return false;
}
