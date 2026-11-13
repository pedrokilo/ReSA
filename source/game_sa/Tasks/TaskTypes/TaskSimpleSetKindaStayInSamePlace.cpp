/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleSetKindaStayInSamePlace
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
#include "TaskSimpleSetKindaStayInSamePlace.h"

namespace {
// 0x62F650: cuerpo no virtual de ~CTaskSimpleSetKindaStayInSamePlace del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx], 0x86DD74 ; jmp 0x61A3A0`). Deja la vtable de la clase del EXE (0x86DD74) y salta a CTask::Destructor (0x61A3A0),
// que deja la vtable de CTask. El hook `Destructor` de esta clase (0x62F650) usa el destructor del compilador, que escribiria la vtable de la DLL: aqui se reproduce el cuerpo del EXE.
void __fastcall SetKindaStayInSamePlaceDestructorBodyExe(CTaskSimpleSetKindaStayInSamePlace* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DD74;
    self->CTask::Destructor();
}

// 0x6374C0 (lote 183A, sin validar; el listado decia CTaskSimpleHitFromFront): destructor con borrado de CTaskSimpleSetKindaStayInSamePlace del EXE (ranura 0 de su vtable; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62F650;
// test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). El cuerpo es el de la clase (0x62F650, con hook). Como el original: libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CTaskSimpleSetKindaStayInSamePlace* __fastcall SetKindaStayInSamePlaceScalarDeletingDestructor(CTaskSimpleSetKindaStayInSamePlace* self, void* /*edx*/, uint8 flags) {
    SetKindaStayInSamePlaceDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote183ABody_CTaskSimpleSetKindaStayInSamePlace() { return reinterpret_cast<uintptr_t>(&SetKindaStayInSamePlaceDestructorBodyExe); } // para el banco Fn183A

void CTaskSimpleSetKindaStayInSamePlace::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleSetKindaStayInSamePlace, 0x86dd74, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(SetKindaStayInSamePlaceScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6374C0); // lote 183A (sin validar)

    RH_ScopedInstall(Constructor, 0x62F610);
    RH_ScopedInstall(Destructor, 0x62F650);

    RH_ScopedVMTInstall(Clone, 0x635C30);
    RH_ScopedVMTInstall(GetTaskType, 0x62F630);
    RH_ScopedVMTInstall(MakeAbortable, 0x62F640);
    RH_ScopedVMTInstall(ProcessPed, 0x62F660);
}

// 0x62F610
CTaskSimpleSetKindaStayInSamePlace::CTaskSimpleSetKindaStayInSamePlace(bool bKindaStayInSamePlace) :
    m_bKindaStayInSamePlace{ bKindaStayInSamePlace }
{
}

// 0x635C30
CTaskSimpleSetKindaStayInSamePlace::CTaskSimpleSetKindaStayInSamePlace(const CTaskSimpleSetKindaStayInSamePlace& o) :
    CTaskSimpleSetKindaStayInSamePlace{ o.m_bKindaStayInSamePlace }
{
}
