/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleEvasiveDive
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventDamage.h"
#include "TaskSimpleEvasiveDive.h"

namespace {
// 0x6535D0 (lote 146A, banco Fn146A): cuerpo del destructor de CTaskSimpleEvasiveDive que usa el destructor con borrado del EXE (thiscall, sin argumentos de pila; el hook Destructor de la clase). Funcion propia sin expansion
// en linea para que el banco pueda sustituirla; el destructor con borrado la llama como el `call 0x6535D0` del EXE.
__declspec(noinline) void __fastcall EvasiveDiveDestructorBody(CTaskSimpleEvasiveDive* self, void* /*edx*/) { self->CTaskSimpleEvasiveDive::~CTaskSimpleEvasiveDive(); }

// 0x655F00 (lote 146A, banco Fn146A): destructor con borrado de CTaskSimpleEvasiveDive del EXE (ranura 0 de su vtable 0x86F244; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x6535D0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x6535D0), libera `this`
// con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleEvasiveDive* __fastcall EvasiveDiveScalarDeletingDestructor(CTaskSimpleEvasiveDive* self, void* /*edx*/, uint8 flags) {
    EvasiveDiveDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote146AEvasiveDiveDestructorBody() { return reinterpret_cast<uintptr_t>(&EvasiveDiveDestructorBody); } // para el banco Fn146A

void CTaskSimpleEvasiveDive::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleEvasiveDive, 0x86F244, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(EvasiveDiveScalarDeletingDestructor, "ScalarDeletingDestructor", 0x655F00); // lote 146A (banco Fn146A)

    RH_ScopedInstall(Constructor, 0x653560);
    RH_ScopedInstall(Destructor, 0x6535D0);
    RH_ScopedInstall(StartAnim, 0x655F20);
    RH_ScopedInstall(FinishAnimEvasiveDiveCB, 0x6536A0);

    RH_ScopedVMTInstall(Clone, 0x6558E0);
    RH_ScopedVMTInstall(GetTaskType, 0x6535C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x653640);
    RH_ScopedVMTInstall(ProcessPed, 0x657AC0);
}

// 0x653560
CTaskSimpleEvasiveDive::CTaskSimpleEvasiveDive(CVehicle* vehicle) {
    m_EvadeVeh  = vehicle;
    m_bFinished = false;
    m_DiveAnim  = nullptr;
    CEntity::SafeRegisterRef(m_EvadeVeh);
}

// 0x6535D0
CTaskSimpleEvasiveDive::~CTaskSimpleEvasiveDive() {
    // Como el original: la referencia al vehiculo se limpia antes de quitar el callback de la animacion
    CEntity::SafeCleanUpRef(m_EvadeVeh);
    if (m_DiveAnim) {
        m_DiveAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x653640
bool CTaskSimpleEvasiveDive::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_DiveAnim) {
            m_DiveAnim->SetBlendDelta(-1000.f);
        }
        return true;
    }
    if (const auto eDmg = notsa::dyn_cast_if_present<const CEventDamage>(event)) {
        if (eDmg->m_pSourceEntity && eDmg->m_pSourceEntity->GetIsTypeVehicle()) {
            switch (eDmg->m_weaponType) {
            case WEAPON_RAMMEDBYCAR:
            case WEAPON_RUNOVERBYCAR:
                return true;
            }
        }
    }
    return false;
}

// 0x657AC0
bool CTaskSimpleEvasiveDive::ProcessPed(CPed* ped) {
    if (m_bFinished) {
        return true;
    }
    if (!m_DiveAnim) {
        StartAnim(ped);
    }
    return false;
}

// 0x655F20
void CTaskSimpleEvasiveDive::StartAnim(CPed* ped) {
    ped->Say(CTX_GLOBAL_DODGE);

    m_DiveAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_EV_DIVE, 8.0f);
    m_DiveAnim->SetFinishCallback(FinishAnimEvasiveDiveCB, this);

    if (m_EvadeVeh && ped->IsCop()) {
        if (m_EvadeVeh->m_pDriver && m_EvadeVeh->m_pDriver->IsPlayer()) {
            // Como el original: FindPlayerWanted se llama antes de cada delito
            FindPlayerWanted()->RegisterCrime_Immediately(CRIME_VEHICLE_DAMAGE, ped->GetPosition(), (uint32)ped, false);
            FindPlayerWanted()->RegisterCrime_Immediately(CRIME_SPEEDING, ped->GetPosition(), (uint32)ped, false);
        }
    }
}

// 0x6536A0
void CTaskSimpleEvasiveDive::FinishAnimEvasiveDiveCB(CAnimBlendAssociation* assoc, void* data) {
    const auto self = static_cast<CTaskSimpleEvasiveDive*>(data); // Como el original: sin comprobar el tipo

    self->m_bFinished = true;
    self->m_DiveAnim  = nullptr;
}
