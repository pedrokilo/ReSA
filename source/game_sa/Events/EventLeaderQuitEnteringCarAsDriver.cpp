/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderQuitEnteringCarAsDriver
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventLeaderQuitEnteringCarAsDriver.h"

namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x63A1D0 (lote 156A, banco Fn156A): cuerpo del destructor de CEventLeaderQuitEnteringCarAsDriver del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov dword [ecx],86E6B0h; jmp 0x4AC010`). Deja la vtable de
// la clase del EXE (la de la DLL no vale para los llamadores del EXE) y sigue con el cuerpo de ~CEvent (0x4AC010, con hook propio). Funcion propia sin expansion en linea para que el banco pueda sustituirla; la llama
// el destructor con borrado como el `call 0x63A1D0` del EXE.
__declspec(noinline) void __fastcall LeaderQuitEnteringDestructorBodyExe(CEventLeaderQuitEnteringCarAsDriver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86E6B0;
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x63A1B0 (lote 156A, banco Fn156A): destructor con borrado de CEventLeaderQuitEnteringCarAsDriver del EXE (ranura 0 de su vtable 0x86E6B0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x63A1D0; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x63A1D0),
// libera `this` con CEvent::operator delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
CEventLeaderQuitEnteringCarAsDriver* __fastcall LeaderQuitEnteringScalarDeletingDestructor(CEventLeaderQuitEnteringCarAsDriver* self, void* /*edx*/, uint8 flags) {
    LeaderQuitEnteringDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventLeaderQuitEnteringCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderQuitEnteringCarAsDriver, 0x86E6B0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(LeaderQuitEnteringDestructorBodyExe, "DestructorBody", 0x63A1D0); // lote 156A (banco Fn156A)
    RH_ScopedNamedGlobalInstall(LeaderQuitEnteringScalarDeletingDestructor, "ScalarDeletingDestructor", 0x63A1B0); // lote 156A (banco Fn156A)
    RH_ScopedInstall(Constructor, 0x63A110);
    RH_ScopedVMTInstall(GetEventType, 0x63A130);        // lote 156A (banco Fn156A): retorno de constante
    RH_ScopedVMTInstall(GetEventPriority, 0x63A1E0);    // lote 156A (banco Fn156A): retorno de constante
    RH_ScopedVMTInstall(GetLifeTime, 0x63A140);         // lote 156A (banco Fn156A): retorno de constante
    RH_ScopedVMTInstall(Clone, 0x63A150);               // lote 156A (banco Fn156A)
    RH_ScopedVMTInstall(AffectsPed, 0x63A1A0);          // lote 156A (banco Fn156A): retorno de constante
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B1040);   // lote 156A (banco Fn156A): retorno de constante
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B1010);
}

// 0x63A130 (lote 156A, banco Fn156A; thiscall, `ret`, 6 bytes: `mov eax,39h`): EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER (57); no lee el objeto (ranura 1 de la vtable 0x86E6B0).
eEventType CEventLeaderQuitEnteringCarAsDriver::GetEventType() const {
    return EVENT_LEADER_QUIT_ENTERING_CAR_AS_DRIVER;
}

// 0x63A1E0 (lote 156A, banco Fn156A; thiscall, `ret`, 6 bytes: `mov eax,30h`): prioridad 48; no lee el objeto (ranura 2).
int32 CEventLeaderQuitEnteringCarAsDriver::GetEventPriority() const {
    return 48;
}

// 0x63A140 (lote 156A, banco Fn156A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3).
int32 CEventLeaderQuitEnteringCarAsDriver::GetLifeTime() {
    return 0;
}

// 0x63A150 (lote 156A, banco Fn156A; thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); no lee `this`; ranura 4 de la vtable 0x86E6B0).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0xC bytes; sin memoria devuelve nulo; si no, CEvent::CEvent (0x4ABFC0, con hook; ECX = memoria), deja la vtable de la clase del EXE (0x86E6B0;
// la de la DLL no vale para los llamadores del EXE) y devuelve la memoria.
CEventLeaderQuitEnteringCarAsDriver* CEventLeaderQuitEnteringCarAsDriver::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventLeaderQuitEnteringCarAsDriver));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEvent*>(memory)->Constructor();
    *reinterpret_cast<uint32*>(memory) = 0x86E6B0;
    return static_cast<CEventLeaderQuitEnteringCarAsDriver*>(memory);
}

// 0x63A1A0 (lote 156A, banco Fn156A; thiscall, un puntero de pila, `ret 4`, 5 bytes: `xor al,al`): siempre falso; no lee ni el objeto ni el peaton (ranura 5).
bool CEventLeaderQuitEnteringCarAsDriver::AffectsPed(CPed* ped) {
    return false;
}

// 0x4B1040 (lote 156A, banco Fn156A; thiscall, una referencia de pila, `ret 4`, 5 bytes: `mov al,1`): siempre verdadero; no lee ni el objeto ni el otro evento (ranura 11).
bool CEventLeaderQuitEnteringCarAsDriver::TakesPriorityOver(const CEvent& refEvent) {
    return true;
}

// 0x63A110
CEventLeaderQuitEnteringCarAsDriver* CEventLeaderQuitEnteringCarAsDriver::Constructor()
{
    this->CEventLeaderQuitEnteringCarAsDriver::CEventLeaderQuitEnteringCarAsDriver();
    return this;
}

// 0x4B1010
bool CEventLeaderQuitEnteringCarAsDriver::AffectsPedGroup(CPedGroup* pedGroup) {
    const auto oe = pedGroup->GetIntelligence().GetCurrentEvent();
    return oe && oe->GetEvent().GetEventType() == EVENT_LEADER_ENTERED_CAR_AS_DRIVER;
}