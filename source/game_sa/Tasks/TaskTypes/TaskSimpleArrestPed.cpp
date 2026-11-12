/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleArrestPed
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

#include "TaskSimpleArrestPed.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace {
// 0x68B620 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskSimpleArrestPed del EXE (thiscall, un puntero a entidad en la pila, `ret 4`, devuelve this). Base CTaskSimple (0x61A390), vtable 0x870984, el peaton en +8, byte +0xC y dword +0x10 a cero y, si el peaton no es nulo, CEntity::RegisterReference (0x571B70) sobre +8.
CTaskSimpleArrestPed* __fastcall ArrestPedConstructorBody(CTaskSimpleArrestPed* self, void* /*edx*/, CEntity* ped) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x870984;
    *reinterpret_cast<CEntity**>(bytes + 8) = ped;
    bytes[0xC] = 0;
    *reinterpret_cast<uint32*>(bytes + 0x10) = 0;
    if (ped) {
        ped->RegisterReference(reinterpret_cast<CEntity**>(bytes + 8));
    }
    return self;
}

// 0x68B690 (lote 167A, banco Fn167A; thiscall, sin argumentos de pila, `ret`, 162 bytes con marco SEH que solo desenrolla): cuerpo del destructor de CTaskSimpleArrestPed del EXE. Deja la vtable de la clase del EXE
// (0x870984); si hay animacion (+0x10) le pone el DefaultAnimCB del EXE (0x4D1390) como callback de fin con dato nulo (CAnimBlendAssociation::SetFinishCallback 0x4CEBE0); libera con CleanUpOldReference (0x571A00)
// la referencia al ped (+8) si no es nula; con la animacion y el ped (releidos) no nulos, si el ped no es jugador (0x5DF8F0 devuelve 0), esta en vehiculo (bit 8 de +0x46C) y su estado (+0x530) es 0x3F
// (PEDSTATE_ARRESTED), llama a SetPedState (0x5E4500) con 0x32 (PEDSTATE_DRIVING); y termina con CTask::Destructor (0x61A3A0). El marco SEH se omite.
__declspec(noinline) void __fastcall ArrestPedDestructorBody(CTaskSimpleArrestPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870984;
    if (self->m_Assoc) {
        self->m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    CEntity::SafeCleanUpRef(self->m_Ped);
    if (self->m_Assoc && self->m_Ped && !self->m_Ped->IsPlayer()) {
        if (self->m_Ped->bInVehicle && self->m_Ped->m_nPedState == PEDSTATE_ARRESTED) {
            self->m_Ped->SetPedState(PEDSTATE_DRIVING);
        }
    }
    self->CTask::Destructor();
}
} // namespace

void CTaskSimpleArrestPed__InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleArrestPed, 0x870984, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(ArrestPedConstructorBody, "Constructor", 0x68B620);
    RH_ScopedNamedGlobalInstall(ArrestPedDestructorBody, "DestructorBody", 0x68B690); // lote 167A (banco Fn167A)
    RH_ScopedVMTInstall(GetTaskType, 0x68B680); // lote 167A (banco Fn167A): retorno de constante

    RH_ScopedVMTInstall(MakeAbortable, 0x68B740);
    RH_ScopedVMTInstall(ProcessPed, 0x68B8A0);
    RH_ScopedInstall(StartAnim, 0x68B7E0);
}

// 0x68B620
CTaskSimpleArrestPed::CTaskSimpleArrestPed(CPed* ped) {
    m_Ped = ped;
    m_bFinished = false;
    m_Assoc = nullptr;
    CEntity::SafeRegisterRef(m_Ped);
}

// 0x68B680 (lote 167A, banco Fn167A; thiscall, `ret`, 6 bytes: `mov eax,44Ch`): TASK_SIMPLE_ARREST_PED (1100); no lee el objeto (ranura 4 de la vtable 0x870984; el listado lo atribuia a CTaskSimpleSwim).
eTaskType CTaskSimpleArrestPed::GetTaskType() const {
    return Type;
}

// 0x68B690
CTaskSimpleArrestPed::~CTaskSimpleArrestPed() {
    if (m_Assoc) {
        m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }

    CEntity::SafeCleanUpRef(m_Ped);

    if (m_Assoc && m_Ped && !m_Ped->IsPlayer()) {
        if (m_Ped->bInVehicle && m_Ped->m_nPedState == PEDSTATE_ARRESTED) {
            m_Ped->SetPedState(PEDSTATE_DRIVING);
        }
    }
}

// 0x68B740
bool CTaskSimpleArrestPed::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Assoc) {
            m_Assoc->m_BlendDelta = -1000.0f;
            m_Assoc->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_Assoc = nullptr;
        }
        return true;
    }

    if (event && event->GetEventType() == EVENT_DAMAGE) {
        const auto dmgEvent = static_cast<const CEventDamage*>(event);
        if (dmgEvent->m_damageResponse.m_bHealthZero) {
            if (dmgEvent->m_bAddToEventGroup) {
                m_Ped->SetPedState(m_Ped->bInVehicle ? PEDSTATE_DRIVING : PEDSTATE_NONE);
                m_Ped->bIsBeingArrested = false;
            }
        }
        return true;
    }

    return false;
}

// 0x68B8A0
bool CTaskSimpleArrestPed::ProcessPed(CPed* ped) {
    if (m_bFinished)
        return false;

    if (m_Assoc) {
        // Como el original: GetBonePosition(CVector*, eBoneTag, bool) en 0x5E4280; sin m_Ped el EXE deja el punto sin inicializar
        CVector point{};
        if (m_Ped) {
            m_Ped->GetBonePosition(&point, BONE_SPINE1, false);
        }
        const auto& pedPos = ped->GetPosition();
        auto angle = CGeneral::GetRadianAngleBetweenPoints(point.x, point.y, pedPos.x, pedPos.y);
        ped->m_fAimingRotation = angle;
        ped->m_fCurrentRotation = angle;
        ped->SetOrientation(0.0f, 0.0f, angle);
        ped->m_pedIK.PointGunAtPosition(point, m_Assoc->m_BlendAmount);
        return false;
    }

    if (ped->GetActiveWeapon().IsTypeMelee()) {
        ped->GiveWeapon(WEAPON_PISTOL, 10, false);
        ped->SetCurrentWeapon(WEAPON_PISTOL);
    }

    StartAnim(ped);
    return false;
}

// 0x68B7E0
void CTaskSimpleArrestPed::StartAnim(CPed* ped) {
    m_Assoc = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_ARRESTGUN, 4.0f);
    m_Assoc->SetFlag(ANIMATION_IS_PLAYING, true);
    m_Assoc->SetDeleteCallback(FinishAnimArrestPedCB, this);

    if (m_Ped->IsPlayer()) {
        ped->Say(CTX_GLOBAL_ARREST, 0, 1.0f, true, true, true);
    }

    if (m_Ped->CanSetPedState()) {
        m_Ped->SetPedState(PEDSTATE_ARRESTED);
    }

    if (ped->m_nPedType == PED_TYPE_COP && m_Ped->m_nPedType <= PED_TYPE_PLAYER2) {
        if (!m_Ped->GetPlayerData()->m_pArrestingCop) {
            m_Ped->GetPlayerData()->m_pArrestingCop = static_cast<CCopPed*>(ped);
            m_Ped->GetPlayerData()->m_pArrestingCop->RegisterReference(m_Ped->GetPlayerData()->m_pArrestingCop);
        }
    }

    m_Ped->bIsBeingArrested = true;
    ped->SetPedState(PEDSTATE_ARREST_PLAYER);
}

// 0x68B7D0
void CTaskSimpleArrestPed::FinishAnimArrestPedCB(CAnimBlendAssociation* assoc, void* data) {
    auto task = static_cast<CTaskSimpleArrestPed*>(data);
    task->m_bFinished = true;
    task->m_Assoc = nullptr;
}
