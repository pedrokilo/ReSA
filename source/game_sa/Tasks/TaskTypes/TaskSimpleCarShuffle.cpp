/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarShuffle
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleCarShuffle.h"
#include "TaskUtilityLineUpPedWithCar.h"

void CTaskSimpleCarShuffle::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarShuffle, 0x86ed74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x646810);
    RH_ScopedInstall(Destructor, 0x646890);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x64B3C0);

    RH_ScopedInstall(FinishAnimCarShuffleCB, 0x646970);
    RH_ScopedInstall(StartAnim, 0x64B3E0);
    RH_ScopedVMTInstall(Clone, 0x649C40);
    RH_ScopedVMTInstall(GetTaskType, 0x646880);
    RH_ScopedVMTInstall(MakeAbortable, 0x646900);
    RH_ScopedVMTInstall(ProcessPed, 0x64DC40);
    RH_ScopedVMTInstall(SetPedPosition, 0x646920);
}

// 0x646810
CTaskSimpleCarShuffle::CTaskSimpleCarShuffle(CVehicle* car, int32 targetDoor, CTaskUtilityLineUpPedWithCar* lineUpTask) :
    m_Car{ car },
    m_TargetDoor{ targetDoor },
    m_LineUpUtility{ lineUpTask }
{
    CEntity::SafeRegisterRef(m_Car);
}

// 0x649C40 (Clone)
CTaskSimpleCarShuffle::CTaskSimpleCarShuffle(const CTaskSimpleCarShuffle& o) :
    CTaskSimpleCarShuffle{ o.m_Car, o.m_TargetDoor, o.m_LineUpUtility } // Como el original: los datos del original, no los del objeto nuevo
{
}

// 0x646890
CTaskSimpleCarShuffle::~CTaskSimpleCarShuffle() {
    CEntity::SafeCleanUpRef(m_Car); // Como el original: primero la referencia y luego la animación
    if (m_Anim) {
        m_Anim->SetDefaultFinishCallback();
    }
}

// 0x646970
void CTaskSimpleCarShuffle::FinishAnimCarShuffleCB(CAnimBlendAssociation* anim, void* data) {
    const auto self = static_cast<CTaskSimpleCarShuffle*>(data);

    if (self->m_Anim) { // Como el original: se comprueba la animación de la tarea
        self->m_Anim->SetBlendDelta(-1000.f);
    }
    self->m_Anim = nullptr;

    self->m_IsFinished = true;
}

// 0x64B3E0
void CTaskSimpleCarShuffle::StartAnim(const CPed* target) {
    m_Anim = CAnimManager::BlendAnimation(
        target->GetRpClump(),
        m_Car->GetAnimGroup().GetGroup(ANIM_ID_CAR_SHUFFLE_RHS_1),
        ANIM_ID_CAR_SHUFFLE_RHS_1,
        1000.f
    );
    m_Anim->SetFinishCallback(FinishAnimCarShuffleCB, this);
}

// 0x646900
bool CTaskSimpleCarShuffle::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    if (m_Anim) {
        m_Anim->SetBlendDelta(-1000.f);
    }
    return true;
}

// 0x64DC40
bool CTaskSimpleCarShuffle::ProcessPed(CPed* ped) {
    if (m_IsFinished || !m_Car) {
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
    }
    return false;
}

// 0x646920
bool CTaskSimpleCarShuffle::SetPedPosition(CPed* ped) {
    if (!ped->bInVehicle || m_Anim && m_Anim->GetBlendAmount() > 0.9f) { // Como el original: estrictamente mayor
        m_LineUpUtility->ProcessPed(ped, m_Car, m_Anim);
    } else {
        ped->SetPedPositionInCar();
    }
    return true;
}

// 0x64B3C0 (lote 128A, banco Fn128A) : destructor con borrado de CTaskSimpleCarShuffle del EXE (ranura 0 de su vtable 0x86ED74; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x646890; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x646890), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleCarShuffle* CTaskSimpleCarShuffle::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
