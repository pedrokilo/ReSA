/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventOnFire
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
#include "EventOnFire.h"


namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x5FF760 (lote 197A, sin validar): cuerpo del destructor de CEventOnFire del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86CCC0h; jmp 0x4AC010`). Deja la vtable de la clase del EXE (0x86CCC0)
// y sigue con el cuerpo de ~CEvent (0x4AC010, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado del EXE (0x5FF7F3).
__declspec(noinline) void __fastcall OnFireDestructorBody(CEventOnFire* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CCC0;
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x5FF7F0 (lote 197A, sin validar): destructor con borrado de CEventOnFire del EXE (ranura 0 de su vtable 0x86CCC0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x5FF760;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FF760), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventOnFire* __fastcall OnFireScalarDeletingDestructor(CEventOnFire* self, void* /*edx*/, uint8 flags) {
    OnFireDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventOnFire::InjectHooks() {
    RH_ScopedVirtualClass(CEventOnFire, 0x86CCC0, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(OnFireDestructorBody, "DestructorBody", 0x5FF760); // lote 197A (sin validar)
    RH_ScopedNamedGlobalInstall(OnFireScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FF7F0); // lote 197A (sin validar)
    RH_ScopedInstall(Constructor, 0x5FF740); // lote 197A (sin validar)
    RH_ScopedVMTInstall(GetEventType, 0x5FF770); // lote 197A (sin validar): retorno de constante
    RH_ScopedVMTInstall(GetLifeTime, 0x5FF780); // lote 197A (sin validar): retorno de constante
    RH_ScopedVMTInstall(Clone, 0x5FF790); // lote 197A (sin validar)
    RH_ScopedVMTInstall(GetLocalSoundLevel, 0x5FF7E0); // lote 197A (sin validar): retorno de constante x87
    RH_ScopedVMTInstall(GetEventPriority, 0x5FF890); // lote 197A (sin validar): retorno de constante
    RH_ScopedVMTInstall(AffectsPed, 0x4B1050);
}

// 0x5FF740 (lote 197A, sin validar; thiscall, sin argumentos de pila, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x4ABFC0; mov [esi],86CCC0h; mov eax,esi; pop esi; ret`). Constructor de CEvent
// (0x4ABFC0, con hook) y vtable de la clase del EXE (0x86CCC0); devuelve this.
CEventOnFire* CEventOnFire::Constructor() {
    static_cast<CEvent*>(this)->Constructor();
    *reinterpret_cast<uint32*>(this) = 0x86CCC0;
    return this;
}

// 0x5FF770 (lote 197A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,3Ah`): EVENT_ON_FIRE (58); no lee el objeto (ranura 1 de la vtable 0x86CCC0).
eEventType CEventOnFire::GetEventType() const {
    return EVENT_ON_FIRE;
}

// 0x5FF890 (lote 197A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,42h`): prioridad 66; no lee el objeto (ranura 2).
int32 CEventOnFire::GetEventPriority() const {
    return 66;
}

// 0x5FF780 (lote 197A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3).
int32 CEventOnFire::GetLifeTime() {
    return 0;
}

// 0x5FF7E0 (lote 197A, sin validar; thiscall, `ret`, 7 bytes: `fld dword [0x858B34]`): nivel de sonido local 60.0f (constante del EXE 0x42700000) en ST(0); no lee el objeto (ranura 12). Un solo float, sin aritmetica.
float CEventOnFire::GetLocalSoundLevel() {
    return 60.0f;
}

// 0x5FF790 (lote 197A, sin validar; thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x86CCC0).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0xC bytes; sin memoria devuelve nulo (EAX = 0); si no, CEvent::CEvent (0x4ABFC0, con hook; ECX = memoria), deja la vtable de la clase del EXE
// (0x86CCC0; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria. No lee `this`.
CEventOnFire* CEventOnFire::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventOnFire));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEvent*>(memory)->Constructor();
    *reinterpret_cast<uint32*>(memory) = 0x86CCC0;
    return static_cast<CEventOnFire*>(memory);
}

// 0x4B1050
bool CEventOnFire::AffectsPed(CPed* ped) {
    if (ped->m_pFire && !ped->physicalFlags.bFireProof) {
        CTask* activeTask = ped->GetTaskManager().GetActiveTask();
        if (activeTask && activeTask->GetTaskType() == TASK_COMPLEX_ON_FIRE)
            return false;

        CTask* partialAnimTask = ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM);
        if (partialAnimTask && partialAnimTask->GetTaskType() == TASK_SIMPLE_PLAYER_ON_FIRE)
            return false;

        return ped->IsAlive();
    }
    return false;
}