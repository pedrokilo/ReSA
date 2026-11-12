/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexPartnerShove
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexPartnerShove.h"
#include "TaskComplexSequence.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleRunAnim.h"

namespace {
// 0x684750 (lote 151A, banco Fn151A; el listado decia CTaskComplexPartnerShove y lo es): cuerpo no virtual de ~CTaskComplexPartnerShove del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes:
// `mov [ecx], 0x870800; jmp 0x683A40`). Deja la vtable de la clase del EXE (0x870800) y sigue con el cuerpo de ~CTaskComplexPartner (0x683A40, con hook). Lo llama el destructor con borrado (0x684FE0).
void __fastcall PartnerShoveDestructorBody(CTaskComplexPartnerShove* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x870800;
    CTaskComplexPartnerBody::Destructor(self, nullptr);
}
// 0x684FE0 (lote 185A, sin validar; el listado decia CTaskComplexPartnerGreet): destructor con borrado de CTaskComplexPartnerShove del EXE (ranura 0 de la vtable 0x870800; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x684750;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). 0x684750 es el cuerpo del destructor de la clase (PartnerShoveDestructorBody, con hook). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas
// esta puesto y devuelve `this`. Se engancha como S.
CTaskComplexPartnerShove* __fastcall PartnerShoveScalarDeletingDestructor(CTaskComplexPartnerShove* self, void* /*edx*/, uint8 flags) {
    PartnerShoveDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexPartnerShove::InjectHooks()
{
    RH_ScopedVirtualClass(CTaskComplexPartnerShove, 0x870800, 14);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(PartnerShoveDestructorBody, "Destructor", 0x684750); // lote 151A (banco Fn151A)
    RH_ScopedInstall(Constructor, 0x6846F0);
    RH_ScopedNamedGlobalInstall(PartnerShoveScalarDeletingDestructor, "ScalarDeletingDestructor", 0x684FE0); // lote 185A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x681E20); // lote 185A (sin validar): ranura 4
    RH_ScopedVMTInstall(Clone, 0x684ED0);       // lote 185A (sin validar): ranura 1
    RH_ScopedVMTInstall(GetPartnerSequence, 0x683120);
}

CTaskComplexPartnerShove::CTaskComplexPartnerShove(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point) :
    CTaskComplexPartner(commandName, partner, leadSpeaker, distanceMultiplier, false, updateDirectionCount, point)
{
    m_updateDirectionCount = updateDirectionCount;
    m_taskId = TASK_COMPLEX_PARTNER_SHOVE;
}

// 0x684ED0 (lote 185A, sin validar; el listado decia CTaskComplexPartnerGreet; thiscall, sin argumentos, `ret`, 142 bytes con marco SEH que solo libera la memoria si el constructor lanza y se omite): Clone del EXE. Reserva 0x74 bytes
// con CTask::operator new (0x61A5A0); si devuelve nulo, devuelve nulo. Si no, llama al constructor (0x6846F0) sobre la memoria con (nombre de comando +0x14, companero +0x38, lider +0x58 sin signo, multiplicador +0x3C, cuenta de
// direcciones +0x5B con signo y el punto +0x40 por valor) y devuelve lo que el constructor deja en EAX.
CTask* CTaskComplexPartnerShove::Clone() const {
    auto* const memory = static_cast<CTaskComplexPartnerShove*>(CTask::operator new(sizeof(CTaskComplexPartnerShove)));
    if (!memory) {
        return nullptr;
    }
    return memory->Constructor(m_commandName, m_partner, m_leadSpeaker, m_distanceMultiplier, m_updateDirectionCount, m_point);
}

CTaskComplexPartnerShove* CTaskComplexPartnerShove::Constructor(const char* commandName, CPed* partner, bool leadSpeaker, float distanceMultiplier, int8 updateDirectionCount, CVector point)
{
    this->CTaskComplexPartnerShove::CTaskComplexPartnerShove(commandName, partner, leadSpeaker, distanceMultiplier, updateDirectionCount, point);
    return this;
}

// 0x683120
CTaskComplexSequence* CTaskComplexPartnerShove::GetPartnerSequence()
{
    auto* seq = new CTaskComplexSequence; // Como el original: sin inicialización por valor
    seq->AddTask(new CTaskComplexTurnToFaceEntityOrCoord(m_partner, 0.5f, 0.001f)); // 0x86FC7C y 0x3A83126F
    seq->AddTask(new CTaskSimpleRunAnim(ANIM_GROUP_DEFAULT, m_leadSpeaker ? ANIM_ID_SHOVE_PARTIAL : ANIM_ID_HANDSUP, 4.0f, false)); // 0xA4 si m_leadSpeaker != 0, si no 0x8E; mezcla 4.0f (0x40800000)
    return seq;
}
