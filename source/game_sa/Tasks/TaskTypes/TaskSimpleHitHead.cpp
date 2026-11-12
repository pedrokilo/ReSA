/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleHitHead
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

#include "TaskSimpleHitHead.h"

namespace {
// Callback por defecto de animaciones del EXE (CDefaultAnimCallback::DefaultAnimCB, 0x4D1390: `ret`). Como el original, el destructor pasa la direccion del EXE a SetFinishCallback (la de la DLL no
// vale para la comparacion de lo que se le pasa).
using AnimFinishCallback = void (*)(CAnimBlendAssociation*, void*);
const AnimFinishCallback kExeDefaultAnimCB = reinterpret_cast<AnimFinishCallback>(0x4D1390);

// 0x653090 (lote 150A, banco Fn150A): cuerpo del destructor de CTaskSimpleHitHead del EXE (thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla): deja la vtable de la clase (0x86F1D0); si hay
// animacion (m_pAnim, +0xC) le pone SetFinishCallback(0x4D1390, nullptr) (0x4CEBE0, con hook); y llama al cuerpo de ~CTask (0x61A3A0, con hook: vtable base 0x86D48C). Conserva ESI pero no EAX, ECX ni EDX
// (los llamadores, los destructores con borrado, no los leen). El marco SEH se omite.
void __fastcall CTaskSimpleHitHeadDestructorBodyExe(CTaskSimpleHitHead* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86F1D0;
    if (self->m_pAnim) {
        self->m_pAnim->SetFinishCallback(kExeDefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}

// 0x655E30 (lote 180A, sin validar; el listado decia CTaskComplexWalkRoundFire): destructor con borrado de CTaskSimpleHitHead del EXE (ranura 0 de su vtable 0x86F1D0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x653090; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x653090, con hook), libera `this` con
// CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskSimpleHitHead* __fastcall CTaskSimpleHitHeadScalarDeletingDestructor(CTaskSimpleHitHead* self, void* /*edx*/, uint8 flags) {
    CTaskSimpleHitHeadDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

// 0x653080 (lote 150A, banco Fn150A; thiscall, `ret`, 6 bytes: `mov eax,0x1F4`): TASK_SIMPLE_HIT_HEAD (500); no lee el objeto.
eTaskType CTaskSimpleHitHead::GetTaskType() const {
    return Type;
}

void CTaskSimpleHitHead::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleHitHead, 0x86F1D0, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskSimpleHitHeadDestructorBodyExe, "DestructorBody", 0x653090); // lote 150A (banco Fn150A)
    RH_ScopedNamedGlobalInstall(CTaskSimpleHitHeadScalarDeletingDestructor, "ScalarDeletingDestructor", 0x655E30); // lote 180A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x6557A0); // lote 180A (sin validar): reserva 0x10 bytes en el pool de tareas; si no es nula, CTaskSimple (0x61A390), vtable del EXE (0x86F1D0), +8 (byte) = 0 y +0xC = 0; nula -> nulo
    RH_ScopedVMTInstall(GetTaskType, 0x653080); // lote 150A (banco Fn150A): retorno de constante

    RH_ScopedInstall(Constructor, 0x653060);
    RH_ScopedInstall(FinishAnimCB, 0x653150);
    RH_ScopedVMTInstall(MakeAbortable, 0x6530F0);
    RH_ScopedVMTInstall(ProcessPed, 0x657A10);
}
CTaskSimpleHitHead* CTaskSimpleHitHead::Constructor() { this->CTaskSimpleHitHead::CTaskSimpleHitHead(); return this; }

// 0x653060
CTaskSimpleHitHead::CTaskSimpleHitHead() : CTaskSimple() {
    m_bIsFinished = false;
    m_pAnim = nullptr;
}

// 0x653090
CTaskSimpleHitHead::~CTaskSimpleHitHead() {
    if (m_pAnim) {
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
}

// 0x653150
void CTaskSimpleHitHead::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    auto task = reinterpret_cast<CTaskSimpleHitHead*>(data);
    task->m_pAnim = nullptr;
    task->m_bIsFinished = true;
}

// 0x6530F0


bool CTaskSimpleHitHead::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority == ABORT_PRIORITY_URGENT || priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_pAnim) {
            m_pAnim->m_BlendDelta = -4.0F;
            m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            m_pAnim = nullptr;
        }

        m_bIsFinished = true;
        return true;
    }

    if (m_pAnim) {
        m_pAnim->m_BlendDelta = -4.0F;
    }
    return false;
}

// 0x657A10

// 0x0
bool CTaskSimpleHitHead::ProcessPed(CPed* ped) {
    if (m_bIsFinished)
        return true;

    if (!m_pAnim) {
        m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_HIT_WALL, 8.0F);
        m_pAnim->SetFinishCallback(FinishAnimCB, this);
    }

    return m_bIsFinished;
}