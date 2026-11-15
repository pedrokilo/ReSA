/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexWaitAtAttractor
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexWaitAtAttractor.h"
#include "TaskSimpleStandStill.h"

namespace {
// 0x638590 (lote 198A, sin validar; el listado decia CTaskComplexSunbathe: es la ranura 0 de la vtable 0x86E108 de CTaskComplexWaitAtAttractor; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x632630;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x632630, con hook: el destructor de C++), libera `this` con CTask::operator delete
// (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskComplexWaitAtAttractor* __fastcall WaitAtAttractorScalarDeletingDestructor(CTaskComplexWaitAtAttractor* self, void* /*edx*/, uint8 flags) {
    self->CTaskComplexWaitAtAttractor::~CTaskComplexWaitAtAttractor();
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexWaitAtAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexWaitAtAttractor, 0x86e108, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6325F0);
    RH_ScopedInstall(Destructor, 0x632630);
    RH_ScopedNamedGlobalInstall(WaitAtAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638590); // lote 198A (sin validar)

    RH_ScopedVMTInstall(Clone, 0x636940);
    RH_ScopedVMTInstall(GetTaskType, 0x632620);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x632640);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x632650);
    RH_ScopedVMTInstall(ControlSubTask, 0x6326E0);
}

// 0x6325F0
CTaskComplexWaitAtAttractor::CTaskComplexWaitAtAttractor(CPedAttractor* attractor, int32 slot) :
    m_attractor{attractor},
    m_slot{slot}
{
}

// 0x632630
CTaskComplexWaitAtAttractor::CTaskComplexWaitAtAttractor(const CTaskComplexWaitAtAttractor& o) :
    CTaskComplexWaitAtAttractor{o.m_attractor, o.m_slot}
{
}

// 0x632650
CTask* CTaskComplexWaitAtAttractor::CreateFirstSubTask(CPed* ped) {
    return new CTaskSimpleStandStill{ 10'000 };
}
