/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexUseEffectRunning
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 5/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexUseEffectRunning.h"

namespace {
// 0x636780 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskComplexUseEffectRunning del EXE (thiscall, dos punteros en la pila, `ret 8`, devuelve this). Constructor de CTaskComplexUseEffect (0x6321F0) con los mismos dos argumentos, vtable 0x86E594 y +0x1C = 6 (estado de movimiento: correr).
CTaskComplexUseEffectRunning* __fastcall UseEffectRunningConstructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/, C2dEffectPedAttractor* effect, CEntity* entity) {
    reinterpret_cast<CTaskComplexUseEffect*>(self)->Constructor(effect, entity);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x86E594;
    *reinterpret_cast<uint32*>(bytes + 0x1C) = 6;
    return self;
}
// 0x636850 (lote 141A, banco Fn141A): cuerpo no virtual de ~CTaskComplexUseEffectRunning del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],0x86E594; jmp 0x632270 (~CTaskComplexUseEffect, con hook, envoltura Destructor())`). Deja la vtable de la clase del EXE (0x86E594) y sigue con ese cuerpo.
__declspec(noinline) void __fastcall UseEffectRunningDestructorBody(CTaskComplexUseEffectRunning* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E594;
    static_cast<CTaskComplexUseEffect*>(self)->Destructor();
}
} // namespace

void CTaskComplexUseEffectRunning::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexUseEffectRunning, 0x86e594, 11);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(ScalarDeletingDestructor, 0x636830); // lote 141A (banco Fn141A)
    RH_ScopedNamedGlobalInstall(UseEffectRunningDestructorBody, "Destructor", 0x636850); // lote 141A (banco Fn141A)
    RH_ScopedNamedGlobalInstall(UseEffectRunningConstructorBody, "Constructor", 0x636780);

    RH_ScopedVMTInstall(Clone, 0x6367C0);
    RH_ScopedVMTInstall(GetTaskType, 0x6367B0);
}

// 0x636780
CTaskComplexUseEffectRunning::CTaskComplexUseEffectRunning(C2dEffectPedAttractor* fx, CEntity* e) :
    CTaskComplexUseEffect{ fx, e }
{
    m_MoveState = PEDMOVE_RUN;
}

// 0x6367C0
CTaskComplexUseEffectRunning::CTaskComplexUseEffectRunning(const CTaskComplexUseEffectRunning& o) :
    CTaskComplexUseEffectRunning{ o.m_2dFx, o.m_Entity }
{
}

// 0x636830 (lote 141A, banco Fn141A): destructor con borrado de CTaskComplexUseEffectRunning del EXE (ranura 0 de su vtable 0x86E594; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x636850; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x636850; aqui la funcion UseEffectRunningDestructorBody), libera `this` con CTask::operator delete (0x61A5B0, con hook; marca el hueco del pool de tareas) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
CTaskComplexUseEffectRunning* CTaskComplexUseEffectRunning::ScalarDeletingDestructor(uint8 flags) {
    UseEffectRunningDestructorBody(this, nullptr);
    if (flags & 1) {
        CTask::operator delete(this);
    }
    return this;
}
