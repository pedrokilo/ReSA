/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPlayerCommandToGroupGather
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
#include "EventPlayerCommandToGroupGather.h"


namespace CEventPlayerCommandBody {
void __fastcall PlayerCommandToGroupDestructorBody(CEventPlayerCommandToGroup* self, void* edx); // EventPlayerCommandToGroup.cpp: 0x4B2470
}

namespace {
// 0x609300 (lote 179A, sin validar): cuerpo no virtual de ~CEventPlayerCommandToGroupGather del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86D120h ; jmp 0x4B2470`). Deja la vtable de la
// clase del EXE (0x86D120) y salta al cuerpo de ~CEventPlayerCommandToGroup (0x4B2470, con hook).
__declspec(noinline) void __fastcall PlayerCommandToGroupGatherDestructorBody(CEventPlayerCommandToGroupGather* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D120;
    CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);
}

// 0x6092E0 (lote 179A, sin validar): destructor con borrado (ranura 0 de la vtable 0x86D120; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x609300; test byte [esp+8],1; je ..;
// push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Destruye con el cuerpo de la clase, libera con CEvent::operator delete solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
__declspec(noinline) CEventPlayerCommandToGroupGather* __fastcall PlayerCommandToGroupGatherScalarDeletingDestructor(CEventPlayerCommandToGroupGather* self, void* /*edx*/, uint8 flags) {
    PlayerCommandToGroupGatherDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPlayerCommandToGroupGather::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroupGather, 0x86D120, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(PlayerCommandToGroupGatherDestructorBody, "DestructorBody", 0x609300);                      // lote 179A (sin validar)
    RH_ScopedNamedGlobalInstall(PlayerCommandToGroupGatherScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6092E0); // lote 179A (sin validar)
    RH_ScopedInstall(Constructor, 0x609250);
    RH_ScopedVMTInstall(GetEventType, 0x609270);     // lote 179A (sin validar): ranura 1 de la vtable 0x86D120
    RH_ScopedVMTInstall(GetEventPriority, 0x609330); // lote 179A: ranura 2
    RH_ScopedVMTInstall(CloneEditable, 0x609280);    // lote 179A: ranura 16
}

// 0x609270 (lote 179A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,4Ch`): EVENT_PLAYER_COMMAND_TO_GROUP_GATHER (76); no lee el objeto.
eEventType CEventPlayerCommandToGroupGather::GetEventType() const {
    return EVENT_PLAYER_COMMAND_TO_GROUP_GATHER;
}

// 0x609330 (lote 179A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,2Dh`): prioridad 45; no lee el objeto.
int32 CEventPlayerCommandToGroupGather::GetEventPriority() const {
    return 45;
}

// 0x609280 (lote 179A, sin validar; thiscall, sin argumentos, `ret`, 90 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86D120).
// Como el original: CEvent::operator new (0x4B5620) con 0x1C bytes; sin memoria devuelve nulo; si no, CEventPlayerCommandToGroup::CEventPlayerCommandToGroup (0x4B23D0; ECX = memoria, orden 2 = GATHER y m_target (+0x18)),
// deja la vtable de la clase del EXE (0x86D120) y devuelve la memoria.
CEventPlayerCommandToGroupGather* CEventPlayerCommandToGroupGather::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventPlayerCommandToGroupGather));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventPlayerCommandToGroup*>(memory)->Constructor(PLAYER_GROUP_COMMAND_GATHER, m_target);
    *reinterpret_cast<uint32*>(memory) = 0x86D120;
    return static_cast<CEventPlayerCommandToGroupGather*>(memory);
}

// 0x609250
CEventPlayerCommandToGroupGather* CEventPlayerCommandToGroupGather::Constructor(CPed* target)
{
    this->CEventPlayerCommandToGroupGather::CEventPlayerCommandToGroupGather(target);
    return this;
}

