/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderExitedCarAsDriver
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
#include "EventLeaderExitedCarAsDriver.h"

namespace CEventEditableBody {
void __fastcall EditableResponseDestructorBody(CEventEditableResponse* self, void* edx); // EventEditableResponse.cpp: 0x4AC480
}

namespace {
// 0x4B83C0 (lote 155A, banco Fn155A): cuerpo no virtual de ~CEventLeaderExitedCarAsDriver del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],85B898h ; jmp 0x4AC480`). Deja la vtable de la
// clase del EXE (0x85B898) y salta al cuerpo de ~CEventEditableResponse (0x4AC480, con hook), que deja la vtable de CEvent (0x85AB38). Lo llama el destructor con borrado (0x4B83A0); ningun llamador lee EAX, ECX ni EDX
// tras la llamada. Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall LeaderExitedCarAsDriverDestructorBody(CEventLeaderExitedCarAsDriver* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B898;
    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);
}

// 0x4B83A0 (lote 155A, banco Fn155A): destructor con borrado de CEventLeaderExitedCarAsDriver del EXE (ranura 0 de su vtable 0x85B898; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x4B83C0; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x4B83C0),
// libera `this` con CEvent::operator delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CEventLeaderExitedCarAsDriver* __fastcall LeaderExitedCarAsDriverScalarDeletingDestructor(CEventLeaderExitedCarAsDriver* self, void* /*edx*/, uint8 flags) {
    LeaderExitedCarAsDriverDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventLeaderExitedCarAsDriver::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderExitedCarAsDriver, 0x85B898, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(LeaderExitedCarAsDriverDestructorBody, "DestructorBody", 0x4B83C0);                                  // lote 155A (banco Fn155A)
    RH_ScopedNamedGlobalInstall(LeaderExitedCarAsDriverScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B83A0);              // lote 155A

    RH_ScopedInstall(Constructor, 0x4B8300);
    RH_ScopedVMTInstall(CloneEditable, 0x4B8340); // lote 156A (banco Fn156A)
    RH_ScopedVMTInstall(GetEventType, 0x4B8320);      // lote 155A (banco Fn155A): ranura 1 de la vtable 0x85B898
    RH_ScopedVMTInstall(GetEventPriority, 0x4B84D0);  // lote 155A: ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x4B8330);       // lote 155A: ranura 3
    RH_ScopedVMTInstall(AffectsPed, 0x4B8390);        // lote 155A: ranura 5
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B0F80);
}

// 0x4B8340 (lote 156A, banco Fn156A; thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); no lee `this`; ranura 16 de la vtable 0x85B898).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo; si no, CEventEditableResponse::CEventEditableResponse (0x4AC450, con hook; ECX = memoria), deja la vtable
// de la clase del EXE (0x85B898; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria.
CEventLeaderExitedCarAsDriver* CEventLeaderExitedCarAsDriver::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventLeaderExitedCarAsDriver));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventEditableResponse*>(memory)->Constructor();
    *reinterpret_cast<uint32*>(memory) = 0x85B898;
    return static_cast<CEventLeaderExitedCarAsDriver*>(memory);
}

// 0x4B8320 (lote 155A, banco Fn155A; thiscall, `ret`, 6 bytes: `mov eax,37h`): EVENT_LEADER_EXITED_CAR_AS_DRIVER (55); no lee el objeto.
eEventType CEventLeaderExitedCarAsDriver::GetEventType() const {
    return EVENT_LEADER_EXITED_CAR_AS_DRIVER;
}

// 0x4B84D0 (lote 155A, banco Fn155A; thiscall, `ret`, 6 bytes: `mov eax,2Fh`): prioridad 47; no lee el objeto.
int32 CEventLeaderExitedCarAsDriver::GetEventPriority() const {
    return 47;
}

// 0x4B8330 (lote 155A, banco Fn155A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto.
int32 CEventLeaderExitedCarAsDriver::GetLifeTime() {
    return 0;
}

// 0x4B8390 (lote 155A, banco Fn155A; thiscall, un argumento de pila, `ret 4`, 5 bytes: `xor al,al`): siempre falso; no lee el objeto ni el ped.
bool CEventLeaderExitedCarAsDriver::AffectsPed(CPed* /*ped*/) {
    return false;
}

// 0x4B8300
CEventLeaderExitedCarAsDriver* CEventLeaderExitedCarAsDriver::Constructor()
{
    this->CEventLeaderExitedCarAsDriver::CEventLeaderExitedCarAsDriver();
    return this;
}

// 0x4B0F80
bool CEventLeaderExitedCarAsDriver::AffectsPedGroup(CPedGroup* pg) {
    const auto leader = pg->GetMembership().GetLeader();
    for (auto* const m : pg->GetMembership().GetFollowers()) {
        if (m->m_pVehicle && m->bInVehicle && m->m_pVehicle == leader->m_pVehicle) {
            return true;
        }
        if (m->GetTaskManager().HasAnyOf<TASK_COMPLEX_ENTER_CAR_AS_PASSENGER, TASK_COMPLEX_ENTER_CAR_AS_PASSENGER_WAIT>(false)) {
            return true;
        }
    }
    return false;
}
