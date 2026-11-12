/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleDrown
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

#include "TaskSimpleDrown.h"
#include "TaskSimpleDie.h"

namespace CTaskSimpleBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A390
}

namespace CTaskSimpleDieBody {
void __fastcall SimpleDieDestructorBodyExe(CTaskSimpleDie* self, void* edx); // TaskSimpleDie.cpp: 0x62FB40
}

namespace {
// 0x62FEC0 (lote 115A, banco Fn115A): cuerpo del constructor de CTaskSimpleDrown del EXE (thiscall, sin argumentos, `ret`, devuelve this). Base CTaskSimple (0x61A390); los dwords +8, +0x10, +0x14, +0x1C y +0x24 a cero, +0xC = 0x8C, +0x18 = 4.0f, los dos bits bajos del byte +0x20 a cero (el resto se conserva), vtable 0x86DE30 y byte +0x28 = 1.
CTaskSimpleDrown* __fastcall DrownConstructorBody(CTaskSimpleDrown* self, void* /*edx*/) {
    CTaskSimpleBody::Constructor(self, nullptr);
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    const uint8 flags = bytes[0x20];
    *reinterpret_cast<uint32*>(bytes + 8) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x10) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x14) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x1C) = 0;
    *reinterpret_cast<uint32*>(bytes + 0x24) = 0;
    *reinterpret_cast<uint32*>(bytes + 0xC) = 0x8C;
    *reinterpret_cast<uint32*>(bytes + 0x18) = 0x40800000;
    bytes[0x20] = flags & 0xFC;
    *reinterpret_cast<uint32*>(bytes) = 0x86DE30;
    bytes[0x28] = 1;
    return self;
}

// 0x62FF10 (lote 177A, sin validar): cuerpo del destructor de CTaskSimpleDrown del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86DE30h; jmp 0x62FB40`). Deja la vtable de la clase del EXE (0x86DE30) y sigue
// con el cuerpo de ~CTaskSimpleDie (0x62FB40, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x62FF10` del EXE.
__declspec(noinline) void __fastcall DrownDestructorBody(CTaskSimpleDrown* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86DE30;
    CTaskSimpleDieBody::SimpleDieDestructorBodyExe(self, nullptr);
}

// 0x6378D0 (lote 177A, sin validar): destructor con borrado de CTaskSimpleDrown del EXE (ranura 0 de su vtable 0x86DE30; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x62FF10; test byte [esp+8],1;
// je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x62FF10), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del
// BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CTaskSimpleDrown* __fastcall DrownScalarDeletingDestructor(CTaskSimpleDrown* self, void* /*edx*/, uint8 flags) {
    DrownDestructorBody(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskSimpleDrown::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleDrown, 0x86DE30, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedNamedGlobalInstall(DrownConstructorBody, "Constructor", 0x62FEC0);
    RH_ScopedNamedGlobalInstall(DrownDestructorBody, "DestructorBody", 0x62FF10);                         // lote 177A (sin validar)
    RH_ScopedNamedGlobalInstall(DrownScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6378D0);     // lote 177A (sin validar)
    RH_ScopedVMTInstall(GetTaskType, 0x62FF00);                                                           // lote 177A (sin validar): retorno de constante
    RH_ScopedVMTInstall(ProcessPed, 0x639900);
}

// 0x62FF00 (lote 177A, sin validar; thiscall, `ret`, 5 bytes: `mov eax,0D5h`): TASK_SIMPLE_DROWN (213); no lee el objeto (ranura 4 de la vtable 0x86DE30).
eTaskType CTaskSimpleDrown::GetTaskType() const {
    return Type;
}

// 0x62FEC0
CTaskSimpleDrown::CTaskSimpleDrown() : CTaskSimpleDie(ANIM_GROUP_DEFAULT, ANIM_ID_DROWN, 4.0f, 0.0f) {
    m_bShouldExecute = true;
}

// 0x639900
bool CTaskSimpleDrown::ProcessPed(CPed* ped) {
    if (m_bShouldExecute) {
        ped->SetPedState(PEDSTATE_DIE);
        ped->bIsInTheAir = false; 
        if (ped == FindPlayerPed()) {
            CStats::IncrementStat(STAT_TIMES_DROWNED, 1.0f);
        }
        m_bShouldExecute = false;
    }
    return CTaskSimpleDie::ProcessPed(ped);
}