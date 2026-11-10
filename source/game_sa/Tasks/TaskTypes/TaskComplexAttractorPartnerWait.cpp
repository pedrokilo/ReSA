/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexAttractorPartnerWait
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

#include "TaskComplexAttractorPartnerWait.h"
#include "Scripted2dEffects.h"

namespace CTaskComplexBody {
void __fastcall Destructor(CTask* self, void* edx); // Task.cpp: 0x61A3D0
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x633250 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskComplexAttractorPartnerWait del EXE (thiscall, un byte y un dword en la pila, `ret 8`, devuelve this). Base CTaskComplex (0x61A3B0); byte +0xC = byte bajo del primer argumento (sin normalizar), vtable 0x86E284 y +0x10 = segundo argumento.
CTaskComplexAttractorPartnerWait* __fastcall AttractorPartnerWaitConstructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/, uint32 flag, uint32 value) {
    CTaskComplexBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    bytes[0xC] = static_cast<uint8>(flag);
    *reinterpret_cast<uint32*>(bytes) = 0x86E284;
    *reinterpret_cast<uint32*>(bytes + 0x10) = value;
    return self;
}
// 0x633290 (lote 141A, banco Fn141A): cuerpo no virtual de ~CTaskComplexAttractorPartnerWait del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86E284; jmp 0x61A3D0 (~CTaskComplex, con hook)`). Deja la vtable de la clase del EXE (0x86E284) y sigue con ese cuerpo.
__declspec(noinline) void __fastcall AttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E284;
    CTaskComplexBody::Destructor(self, nullptr);
}
} // namespace

namespace {
// 0x633290 (lote 145A, banco Fn145A): cuerpo del destructor de CTaskComplexAttractorPartnerWait del EXE (con hook propio (Destructor); vtable 0x86E284). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x633290` del EXE.
__declspec(noinline) void __fastcall CTaskComplexAttractorPartnerWaitDestructorBody(CTaskComplexAttractorPartnerWait* self, void* /*edx*/) { self->CTaskComplexAttractorPartnerWait::~CTaskComplexAttractorPartnerWait(); }

// 0x638F00 (lote 145A, banco Fn145A): destructor con borrado de CTaskComplexAttractorPartnerWait del EXE (ranura 0 de su vtable 0x86E284; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x633290; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase,
// libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CTaskComplexAttractorPartnerWait* __fastcall CTaskComplexAttractorPartnerWaitScalarDeletingDestructor(CTaskComplexAttractorPartnerWait* self, void* /*edx*/, uint8 flags) {
    CTaskComplexAttractorPartnerWaitDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote145ABody_CTaskComplexAttractorPartnerWait() { return reinterpret_cast<uintptr_t>(&CTaskComplexAttractorPartnerWaitDestructorBody); } // para el banco Fn145A

void CTaskComplexAttractorPartnerWait::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexAttractorPartnerWait, 0x86e284, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(CTaskComplexAttractorPartnerWaitScalarDeletingDestructor, "ScalarDeletingDestructor", 0x638F00); // lote 145A (banco Fn145A)
    RH_ScopedNamedGlobalInstall(AttractorPartnerWaitDestructorBody, "Destructor", 0x633290); // lote 141A (banco Fn141A)
    RH_ScopedNamedGlobalInstall(AttractorPartnerWaitConstructorBody, "Constructor", 0x633250);

    RH_ScopedVMTInstall(Clone, 0x636CF0);
    RH_ScopedVMTInstall(GetTaskType, 0x633280);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6332A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x638F20);
    RH_ScopedVMTInstall(ControlSubTask, 0x6332B0);
}

// 0x633250
CTaskComplexAttractorPartnerWait::CTaskComplexAttractorPartnerWait(bool isLeader, const CScriptedEffectPair& pair) :
    m_IsLeader{ isLeader },
    m_CurrentFxPair{ &pair }
{
}

// 0x636CF0
CTaskComplexAttractorPartnerWait::CTaskComplexAttractorPartnerWait(const CTaskComplexAttractorPartnerWait& o) :
    CTaskComplexAttractorPartnerWait{ o.m_IsLeader, *o.m_CurrentFxPair }
{
}

// 0x638F20
CTask* CTaskComplexAttractorPartnerWait::CreateFirstSubTask(CPed* ped) {
    return new CTaskComplexUseSequence{ m_CurrentFxPair->Effects[m_IsLeader ? 0 : 1].WaitingTask };
}
