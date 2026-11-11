/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToAttractor
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

#include "TaskComplexGoToAttractor.h"
#include "PedAtmAttractor.h"
#include "PedPlacement.h"
#include "Tasks/TaskComplexSequence.h"
#include "TaskComplexGoToPointAndStandStill.h"
#include "TaskSimpleSlideToCoord.h"
#include "TaskSimpleStandStill.h"

namespace {
// 0x66B6A0 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexGoToAttractor del EXE (con hook propio (Destructor); vtable 0x86FF3C). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x66B6A0` del EXE.
__declspec(noinline) void __fastcall CTaskComplexGoToAttractorDestructorBody(CTaskComplexGoToAttractor* self, void* /*edx*/) { self->CTaskComplexGoToAttractor::~CTaskComplexGoToAttractor(); }

// 0x670400 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexGoToAttractor del EXE (ranura 0 de su vtable 0x86FF3C; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x66B6A0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexGoToAttractor* __fastcall CTaskComplexGoToAttractorScalarDeletingDestructor(CTaskComplexGoToAttractor* self, void* /*edx*/, uint8 flags) {
    CTaskComplexGoToAttractorDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexGoToAttractor() { return reinterpret_cast<uintptr_t>(&CTaskComplexGoToAttractorDestructorBody); } // para el banco Fn145A

void CTaskComplexGoToAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToAttractor, 0x86FF3C, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexGoToAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x670400); // lote 145A (banco Fn145A)

    RH_ScopedInstall(Constructor, 0x66B640);
    RH_ScopedInstall(Destructor, 0x66B6A0);

    RH_ScopedVMTInstall(Clone, 0x66D130);
    RH_ScopedVMTInstall(GetTaskType, 0x66B690);      // lote 160A (banco Fn160A): ranura 4 de la vtable 0x86FF3C
    RH_ScopedVMTInstall(MakeAbortable, 0x66B6B0);    // lote 160A: ranura 6
    RH_ScopedVMTInstall(ControlSubTask, 0x66B710);   // lote 160A: ranura 10
    RH_ScopedVMTInstall(CreateNextSubTask, 0x66B6C0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x670420);
}

// 0x66B690 (lote 160A, banco Fn160A; thiscall, `ret`, 6 bytes: `mov eax,393h`): TASK_COMPLEX_GO_TO_ATTRACTOR (915); no lee el objeto.
eTaskType CTaskComplexGoToAttractor::GetTaskType() const {
    return Type;
}

// 0x66B640
CTaskComplexGoToAttractor::CTaskComplexGoToAttractor(CPedAttractor* attractor, const CVector& pos, float heading, float attrTime, int32 queueNumber, eMoveState ms) {
    m_Attractor = attractor;
    m_vecAttrPosn = pos;
    m_fAttrHeading = heading;
    m_MoveState = ms;
    m_fAttrTime = attrTime;
    m_nQueueNumber = queueNumber;
}

// 0x66B6B0 (lote 160A, banco Fn160A; thiscall, `ret 0Ch`, 8 bytes: `mov ecx,[ecx+8]; mov eax,[ecx]; jmp [eax+18h]`): salto a la ranura 6 (MakeAbortable) de la subtarea (+8) con los mismos tres argumentos.
bool CTaskComplexGoToAttractor::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    return m_pSubTask->MakeAbortable(ped, priority, event);
}

// 0x66B6C0
CTask* CTaskComplexGoToAttractor::CreateNextSubTask(CPed* ped) {
    GetPedAttractorManager()->BroadcastArrival(ped, m_Attractor);
    if (ped->bUseAttractorInstantly && m_Attractor->GetHeadOfQueue() != ped) {
        ped->bUseAttractorInstantly = false;
    }
    return nullptr;
}

// 0x670420
CTask* CTaskComplexGoToAttractor::CreateFirstSubTask(CPed* ped) {
    // Como el original: el tipo 4 fuerza carrera; la llegada instantanea corrige Z en el destino almacenado.
    const auto moveState = m_Attractor->GetType() == PED_ATTRACTOR_SHELTER ? PEDMOVE_RUN : m_MoveState;
    if (ped->bUseAttractorInstantly) {
        m_vecAttrPosn = CPedPlacement::FindZCoorForPed(m_vecAttrPosn).first;
        ped->GetPosition() = m_vecAttrPosn;
        ped->m_fAimingRotation = m_fAttrHeading;
        ped->m_fCurrentRotation = m_fAttrHeading;
        return new CTaskSimpleStandStill{0, false, false, 8.f};
    }
    auto* sequence = new CTaskComplexSequence;
    sequence->AddTask(new CTaskComplexGoToPointAndStandStill{moveState, m_vecAttrPosn, 0.5f, 2.f, false, false});
    sequence->AddTask(new CTaskSimpleSlideToCoord{m_vecAttrPosn, m_fAttrHeading, 0.5f});
    return sequence;
}

// 0x66B710 (lote 160A, banco Fn160A; thiscall, `ret 4`, 6 bytes: `mov eax,[ecx+8]; ret 4`): devuelve la subtarea (+8); no lee el ped.
CTask* CTaskComplexGoToAttractor::ControlSubTask(CPed* ped) {
    return m_pSubTask;
}
