/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventLeaderEntryExit
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
#include "EventLeaderEntryExit.h"

namespace CEventAreaCodesBody {
void __fastcall AreaCodesDestructorBody(CEventAreaCodes* self, void* edx); // EventAreaCodes.cpp: 0x4B2210
}

namespace {
// 0x43E2B0 (lote 158A, banco Fn158A): cuerpo no virtual de ~CEventLeaderEntryExit del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],859540h ; jmp 4B2210`).
// Deja la vtable de CEventLeaderEntryExit del EXE (0x859540) y salta al cuerpo de ~CEventAreaCodes (0x4B2210, con hook), que libera la referencia de m_ped y deja la vtable de CEvent.
__declspec(noinline) void __fastcall LeaderEntryExitDestructorBody(CEventLeaderEntryExit* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x859540;
    CEventAreaCodesBody::AreaCodesDestructorBody(self, nullptr);
}

// 0x43E290 (lote 158A, banco Fn158A): destructor con borrado de CEventLeaderEntryExit del EXE (ranura 0 de su vtable 0x859540; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x43E2B0; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase
// (0x43E2B0, aqui LeaderEntryExitDestructorBody), libera `this` con CEvent::operator delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CEventLeaderEntryExit* __fastcall LeaderEntryExitScalarDeletingDestructor(CEventLeaderEntryExit* self, void* /*edx*/, uint8 flags) {
    LeaderEntryExitDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventLeaderEntryExit::InjectHooks()
{
    RH_ScopedVirtualClass(CEventLeaderEntryExit, 0x859540, 16);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(LeaderEntryExitDestructorBody, "DestructorBody", 0x43E2B0);                       // lote 158A (banco Fn158A)
    RH_ScopedNamedGlobalInstall(LeaderEntryExitScalarDeletingDestructor, "ScalarDeletingDestructor", 0x43E290);  // lote 158A

    RH_ScopedInstall(Constructor, 0x43E1C0);
    RH_ScopedVMTInstall(GetEventType, 0x43E200);        // lote 158A (banco Fn158A): ranura 1 de la vtable 0x859540, retorno de constante
    RH_ScopedVMTInstall(GetEventPriority, 0x43E2C0);    // lote 158A: ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x43E210);         // lote 158A: ranura 3
    RH_ScopedVMTInstall(Clone, 0x43E220);               // lote 158A: ranura 4
    RH_ScopedVMTInstall(HasEditableResponse, 0x43E1E0); // lote 158A: ranura 9
    RH_ScopedVMTInstall(GetSourceEntity, 0x43E1F0);     // lote 158A: ranura 10
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x43E280); // lote 158A: ranura 15
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B23A0);     // lote 144A (banco Fn144A): ranura 6 de la vtable 0x859540 (el listado la atribuia a CEventAreaCodes)
    RH_ScopedVMTInstall(TakesPriorityOver, 0x4B2390);   // lote 144A: `mov al,1; ret 4` (verdadero siempre), ranura 11
}

// 0x43E200 (lote 158A, banco Fn158A; thiscall, `ret`, 6 bytes: `mov eax,4Dh`): EVENT_LEADER_ENTRY_EXIT (77); no lee el objeto.
eEventType CEventLeaderEntryExit::GetEventType() const {
    return EVENT_LEADER_ENTRY_EXIT;
}

// 0x43E2C0 (lote 158A, banco Fn158A; thiscall, `ret`, 6 bytes: `mov eax,37h`): prioridad 55; no lee el objeto.
int32 CEventLeaderEntryExit::GetEventPriority() const {
    return 55;
}

// 0x43E210 (lote 158A, banco Fn158A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto.
int32 CEventLeaderEntryExit::GetLifeTime() {
    return 0;
}

// 0x43E1E0 (lote 158A, banco Fn158A; thiscall, `ret`, 3 bytes: `xor al,al`): siempre falso; no lee el objeto (solo escribe AL).
bool CEventLeaderEntryExit::HasEditableResponse() const {
    return false;
}

// 0x43E1F0 (lote 158A, banco Fn158A; thiscall, `ret`, 3 bytes: `xor eax,eax`): sin entidad de origen (nulo); no lee el objeto.
CEntity* CEventLeaderEntryExit::GetSourceEntity() const {
    return nullptr;
}

// 0x43E280 (lote 158A, banco Fn158A; thiscall, `ret`, 3 bytes: `mov al,1`): siempre verdadero; no lee el objeto (solo escribe AL).
bool CEventLeaderEntryExit::CanBeInterruptedBySameEvent() {
    return true;
}

// 0x43E220 (lote 158A, banco Fn158A; thiscall, `ret`, 88 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x859540).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x10 bytes; sin memoria devuelve nulo; si no, llama al constructor de CEventAreaCodes (0x4B2190, con hook; ECX = memoria) con el ped
// guardado (+0xC), deja la vtable de la clase del EXE (0x859540; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria (no lo que deja el constructor en EAX).
CEventLeaderEntryExit* CEventLeaderEntryExit::Clone() const noexcept {
    auto* const object = static_cast<CEventLeaderEntryExit*>(CEvent::operator new(sizeof(CEventLeaderEntryExit)));
    if (!object) {
        return nullptr;
    }
    object->CEventAreaCodes::Constructor(m_ped);
    *reinterpret_cast<uint32*>(object) = 0x859540;
    return object;
}

// 0x43E1C0
CEventLeaderEntryExit* CEventLeaderEntryExit::Constructor(CPed* ped)
{
    this->CEventLeaderEntryExit::CEventLeaderEntryExit(ped);
    return this;
}

