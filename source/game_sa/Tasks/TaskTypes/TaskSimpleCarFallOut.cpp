/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleCarFallOut
 * Reconstrucción: COMPLETA — 11/11 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 11/11 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskSimpleCarFallOut.h"

// 0x648D60
CTaskSimpleCarFallOut::CTaskSimpleCarFallOut(CVehicle* target, eTargetDoor door, CTaskUtilityLineUpPedWithCar* lineUpUtility) :
    m_Vehicle{ target },
    m_Door{ door },
    m_LineUpUtility{ lineUpUtility }
{
    CEntity::SafeRegisterRef(m_Vehicle); // Como el original: registra la referencia al vehículo
}

// 0x64A280 (Clone)
CTaskSimpleCarFallOut::CTaskSimpleCarFallOut(const CTaskSimpleCarFallOut& o) :
    CTaskSimpleCarFallOut{ o.m_Vehicle, o.m_Door, o.m_LineUpUtility }
{
}

// 0x648DE0
CTaskSimpleCarFallOut::~CTaskSimpleCarFallOut() {
    CEntity::SafeCleanUpRef(m_Vehicle); // Como el original: la animación conserva su callback hacia la tarea
}

// 0x648EE0
void CTaskSimpleCarFallOut::FinishAnimFallOutCB(CAnimBlendAssociation* anim, void* data) {
    auto* const self = static_cast<CTaskSimpleCarFallOut*>(data);

    self->m_HasFinished = true;
    self->m_Anim        = nullptr;

    if (self->m_Vehicle && self->m_Vehicle->IsAutomobile()) {
        // Como el original: abre la puerta (cerrada -> abierta, dañada -> abierta y dañada), pidiendo el estado dos veces
        auto& dm = self->m_Vehicle->AsAutomobile()->GetDamageManager();
        const auto comp = (tComponent)(self->m_Door);
        // Como el original: Get == OK; si no, otro Get == DAMAGED; y un último Get para el +1 (2 o 3 lecturas)
        if (dm.GetDoorStatus_Component(comp) == DAMSTATE_OK || dm.GetDoorStatus_Component(comp) == DAMSTATE_DAMAGED) {
            dm.SetDoorStatus_Component(comp, (eDoorStatus)(dm.GetDoorStatus_Component(comp) + 1));
        }
    }
}

// 0x648E90
auto CTaskSimpleCarFallOut::ComputeAnimID() {
    const auto animId = notsa::contains({ TARGET_DOOR_DRIVER, TARGET_DOOR_REAR_LEFT }, m_Door)
        ? ANIM_ID_CAR_FALLOUT_LHS
        : ANIM_ID_CAR_FALLOUT_RHS;
    return std::make_pair(m_Vehicle->GetAnimGroup().GetGroup(animId), animId);
}

// 0x648E90 (lote 180A, sin validar; thiscall, dos punteros de pila, `ret 8`, 80 bytes; la reconstruida ComputeAnimID devuelve un par y no se puede enganchar): la funcion del EXE. Como el original: puerta del conductor (10) o trasera
// izquierda (11) -> ANIM_ID_CAR_FALLOUT_LHS (0x183); cualquier otra -> RHS (0x184), escrito PRIMERO por el segundo puntero (indice); luego relee ese indice, toma el grupo de animacion del vehiculo (manejo +0x384, byte +0xDE,
// `CVehicleAnimGroup` de 0x94 bytes en 0xC1CDC0) con GetGroup (0x6E3B00, con hook) y lo escribe por el primer puntero (grupo). Los llamadores del EXE son 0x64CCE0 (StartAnim) y los cuerpos de 0x64CC60 y 0x64E2C0 (DoOpenCarDoor en linea).
void CTaskSimpleCarFallOut::ComputeAnimIDExe(AssocGroupId* outGroup, AnimationId* outAnimId) const {
    *outAnimId = (m_Door == TARGET_DOOR_DRIVER || m_Door == TARGET_DOOR_REAR_LEFT)
        ? ANIM_ID_CAR_FALLOUT_LHS
        : ANIM_ID_CAR_FALLOUT_RHS;
    const auto id = *outAnimId;
    *outGroup = m_Vehicle->GetAnimGroup().GetGroup(id);
}

// 0x64CCE0
void CTaskSimpleCarFallOut::StartAnim(const CPed* ped) {
    CCarEnterExit::RemoveCarSitAnim(ped);

    const auto [animGrp, animId] = ComputeAnimID();
    m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), animGrp, animId, 1000.0f);
    m_Anim->SetFinishCallback(FinishAnimFallOutCB, this);
}

// notsa
void CTaskSimpleCarFallOut::DoOpenCarDoor(CPed* ped, float time) {
    const auto [animGrp, animId] = ComputeAnimID();
    m_Vehicle->ProcessOpenDoor(ped, m_Door, animGrp, animId, time);
}

// 0x64CC60
bool CTaskSimpleCarFallOut::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        return false;
    }
    if (m_Anim) {
        m_Anim->SetBlendDelta(-1000.f);
        m_Anim->SetDefaultFinishCallback();
        m_Anim = nullptr;
    }
    if (m_Vehicle) {
        DoOpenCarDoor(ped, 1.f);
    }
    return true;
}

// 0x64E2C0
bool CTaskSimpleCarFallOut::ProcessPed(CPed* ped) {
    if (!m_Vehicle || m_HasFinished) {
        return true;
    }
    if (!m_Anim) {
        StartAnim(ped);
        return false;
    }
    if (!ped->bInVehicle) {
        return true;
    }
    DoOpenCarDoor(ped, m_Anim->GetCurrentTime()); // Como el original: la puerta sigue el tiempo de la animación
    return false;
}

// 0x648E40
bool CTaskSimpleCarFallOut::SetPedPosition(CPed* ped) {
    if (m_Vehicle) {
        if (m_HasFinished || (m_Anim && m_Anim->GetBlendAmount() != 0.f)) {
            m_LineUpUtility->ProcessPed(ped, m_Vehicle, m_Anim);
        } else {
            ped->SetPedPositionInCar();
        }
    }
    return true;
}

void CTaskSimpleCarFallOut::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleCarFallOut, 0x86efd0, 9);
    RH_ScopedCategory(); // TODO: Change this to the appropriate category!

    RH_ScopedInstall(Constructor, 0x648D60);
    RH_ScopedInstall(Destructor, 0x648DE0);
    RH_ScopedInstall(ScalarDeletingDestructor, 0x64CC40);

    RH_ScopedGlobalInstall(FinishAnimFallOutCB, 0x648EE0);

    //RH_ScopedInstall(ComputeAnimID, 0x648E90); // cant hook, changed signature
    RH_ScopedInstall(ComputeAnimIDExe, 0x648E90); // lote 180A (sin validar): la forma del EXE
    RH_ScopedInstall(StartAnim, 0x64CCE0);

    RH_ScopedVMTInstall(Clone, 0x64A280);
    RH_ScopedVMTInstall(GetTaskType, 0x648DD0);
    RH_ScopedVMTInstall(MakeAbortable, 0x64CC60);
    RH_ScopedVMTInstall(ProcessPed, 0x64E2C0);
    RH_ScopedVMTInstall(SetPedPosition, 0x648E40);
}

// 0x64CC40 (lote 127A, banco Fn127A) : destructor con borrado de CTaskSimpleCarFallOut del EXE (ranura 0 de su vtable 0x86EFD0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x648DE0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x648DE0), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskSimpleCarFallOut* CTaskSimpleCarFallOut::ScalarDeletingDestructor(uint8 flags) {
    Destructor();
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
