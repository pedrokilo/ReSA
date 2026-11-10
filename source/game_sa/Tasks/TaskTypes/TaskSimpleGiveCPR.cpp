/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGiveCPR
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGiveCPR.h"
#include "InterestingEvents.h"
#include "EventRevived.h"

namespace {
// Callback por defecto de animaciones del EXE (CDefaultAnimCallback::DefaultAnimCB, 0x4D1390: `ret`). Como el original, el destructor pasa la direccion del EXE a SetFinishCallback (la de la DLL no
// vale para la comparacion de lo que se le pasa).
using AnimFinishCallback = void (*)(CAnimBlendAssociation*, void*);
const AnimFinishCallback kExeDefaultAnimCB = reinterpret_cast<AnimFinishCallback>(0x4D1390);

// 0x6588A0 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleGiveCPR del EXE (thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla): deja la vtable de la clase (0x86F4C8); si hay
// animacion (m_pAnim, +0x10) le pone SetFinishCallback(0x4D1390, nullptr) (0x4CEBE0, con hook); y llama al cuerpo de ~CTask (0x61A3A0, con hook: vtable base 0x86D48C). Conserva ESI pero no EAX, ECX ni EDX
// (los llamadores, los destructores con borrado, no los leen). El marco SEH se omite.
void __fastcall CTaskSimpleGiveCPRDestructorBodyExe(CTaskSimpleGiveCPR* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F4C8;
    if (self->m_pAnim) {
        self->m_pAnim->SetFinishCallback(kExeDefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}
} // namespace

// 0x658890 (lote 150A, banco Fn150A; thiscall, `ret`, 6 bytes: `mov eax,0x66`): TASK_SIMPLE_GIVE_CPR (102); no lee el objeto.
eTaskType CTaskSimpleGiveCPR::GetTaskType() const {
    return Type;
}

void CTaskSimpleGiveCPR::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGiveCPR, 0x86F4C8, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleGiveCPRDestructorBodyExe, "DestructorBody", 0x6588A0); // lote 150A (banco Fn150A)
    RH_ScopedVMTInstall(GetTaskType, 0x658890); // lote 150A (banco Fn150A): retorno de constante
    RH_ScopedInstall(Constructor, 0x658860);
    RH_ScopedInstall(ReviveDeadPed, 0x658900);
    RH_ScopedInstall(FinishGiveCPRAnimCB, 0x658990); // Antes 0x658910: caía dentro de ReviveDeadPed; 0x658990 es el callback que ProcessPed pasa a SetFinishCallback
    RH_ScopedVMTInstall(Clone, 0x659A10);
    RH_ScopedVMTInstall(ProcessPed, 0x65A7C0);
    RH_ScopedVMTInstall(MakeAbortable, 0x6589B0);
}

CTaskSimpleGiveCPR* CTaskSimpleGiveCPR::Constructor(CAccident* pAccident) {
    this->CTaskSimpleGiveCPR::CTaskSimpleGiveCPR(pAccident);
    return this;
}

// 0x658860
CTaskSimpleGiveCPR::CTaskSimpleGiveCPR(CAccident* accident) : CTaskSimple() {
    bNoHealthValue = true;
    bAnimationFinished = false;
    m_pAccident = accident;
    m_pAnim = nullptr;
}

// 0x659E10
CTaskSimpleGiveCPR::~CTaskSimpleGiveCPR() {
    if (m_pAnim)
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
}

// 0x659A10
CTask* CTaskSimpleGiveCPR::Clone() const {
    return new CTaskSimpleGiveCPR(m_pAccident);
}

// 0x65A7C0
bool CTaskSimpleGiveCPR::ProcessPed(CPed* ped) {
    if (bNoHealthValue) {
        m_fInitialHealth = ped->m_fHealth;
        bNoHealthValue = false;
    }

    if (bAnimationFinished) {
        ReviveDeadPed(ped);
        return true;
    } else {
        if (!m_pAnim) {
            m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_MEDIC, ANIM_ID_CPR, 4.0F);
            m_pAnim->SetFinishCallback(FinishGiveCPRAnimCB, this);
        }
        return false;
    }
}

// 0x6589B0
bool CTaskSimpleGiveCPR::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    // Como el original: la prioridad se comprueba antes de leer el evento y, si ningun evento obliga,
    // tambien se aborta con la salud del peaton <= 0
    const auto ShouldAbort = [&] {
        if (priority == ABORT_PRIORITY_IMMEDIATE) {
            return true;
        }
        switch (event->GetEventType()) {
        case EVENT_DEATH:
        case EVENT_HEALTH_REALLY_LOW:
        case EVENT_ON_FIRE:
        case EVENT_IN_WATER:
        case EVENT_WATER_CANNON:
        case EVENT_VEHICLE_DAMAGE_WEAPON:
        case EVENT_VEHICLE_DAMAGE_COLLISION:
            return true;
        case EVENT_DAMAGE:
            if (ped->m_fHealth <= 10.0F || m_fInitialHealth - ped->m_fHealth >= 40.0F) {
                return true;
            }
            break;
        default:
            break;
        }
        return ped->m_fHealth <= 0.0F;
    };
    if (!ShouldAbort()) {
        return false;
    }
    if (m_pAnim) {
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_pAnim->m_BlendDelta = -1000.0F;
        m_pAnim = nullptr;
    }
    return true;
}

// 0x658900
void CTaskSimpleGiveCPR::ReviveDeadPed(CPed* ped) {
    if (m_pAccident && m_pAccident->m_pPed) {
        CPed* targetPed = m_pAccident->m_pPed;

        m_pAccident->m_bIsRevived = true;
        if (!targetPed->bRemoveHead) {
            g_InterestingEvents.Add(CInterestingEvents::EType::INTERESTING_EVENT_11, targetPed);
            CEventRevived eventRevived;
            targetPed->GetIntelligence()->m_eventGroup.Add(&eventRevived, false);
        }
    }
}

// 0x658990
void CTaskSimpleGiveCPR::FinishGiveCPRAnimCB(CAnimBlendAssociation* anim, void* ptask) {
    auto* task = reinterpret_cast<CTaskSimpleGiveCPR*>(ptask);
    task->m_pAnim = nullptr;
    task->bAnimationFinished = true;
}
