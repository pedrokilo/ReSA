/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPlayerCommandToGroupAttack
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
#include "EventPlayerCommandToGroupAttack.h"

namespace CEventPlayerCommandBody {
void __fastcall PlayerCommandToGroupDestructorBody(CEventPlayerCommandToGroup* self, void* edx); // EventPlayerCommandToGroup.cpp: 0x4B2470
}

namespace {
// 0x5F63F0 (lote 157A, banco Fn157A): cuerpo no virtual de ~CEventPlayerCommandToGroupAttack del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86C710h ; jmp 0x4B2470`). Deja la vtable de la
// clase del EXE (0x86C710) y salta al cuerpo de ~CEventPlayerCommandToGroup (0x4B2470, con hook), que libera la referencia de m_target y deja la vtable de CEvent (0x85AB38). Lo llama el destructor con borrado (0x5F63D0);
// ningun llamador lee EAX, ECX ni EDX tras la llamada. Funcion propia sin expansion en linea para que el banco pueda sustituirla.
__declspec(noinline) void __fastcall PlayerCommandToGroupAttackDestructorBody(CEventPlayerCommandToGroupAttack* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C710;
    CEventPlayerCommandBody::PlayerCommandToGroupDestructorBody(self, nullptr);
}

// 0x5F63D0 (lote 157A, banco Fn157A): destructor con borrado de CEventPlayerCommandToGroupAttack del EXE (ranura 0 de su vtable 0x86C710; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x5F63F0; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5F63F0),
// libera `this` con CEvent::operator delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CEventPlayerCommandToGroupAttack* __fastcall PlayerCommandToGroupAttackScalarDeletingDestructor(CEventPlayerCommandToGroupAttack* self, void* /*edx*/, uint8 flags) {
    PlayerCommandToGroupAttackDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPlayerCommandToGroupAttack::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCommandToGroupAttack, 0x86C710, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PlayerCommandToGroupAttackDestructorBody, "DestructorBody", 0x5F63F0);                                  // lote 157A (banco Fn157A)
    RH_ScopedNamedGlobalInstall(PlayerCommandToGroupAttackScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5F63D0);              // lote 157A

    RH_ScopedInstall(Constructor, 0x5F6340);
    RH_ScopedVMTInstall(GetEventType, 0x5F6360);       // lote 157A: ranura 1 de la vtable 0x86C710
    RH_ScopedVMTInstall(GetEventPriority, 0x5F6400);   // lote 157A: ranura 2
    RH_ScopedVMTInstall(CloneEditable, 0x5F6370);      // lote 157A: ranura 16
    RH_ScopedVMTInstall(AffectsPedGroup, 0x4B2530);
}

// 0x5F6360 (lote 157A, banco Fn157A; thiscall, `ret`, 6 bytes: `mov eax,47h`): EVENT_PLAYER_COMMAND_TO_GROUP (71); no lee el objeto.
eEventType CEventPlayerCommandToGroupAttack::GetEventType() const {
    return EVENT_PLAYER_COMMAND_TO_GROUP;
}

// 0x5F6400 (lote 157A, banco Fn157A; thiscall, `ret`, 6 bytes: `mov eax,2Ch`): prioridad 44; no lee el objeto.
int32 CEventPlayerCommandToGroupAttack::GetEventPriority() const {
    return 44;
}

// 0x5F6370 (lote 157A, banco Fn157A; thiscall, sin argumentos, `ret`, 89 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86C710).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x1C bytes; sin memoria devuelve nulo; si no, CEventPlayerCommandToGroup::CEventPlayerCommandToGroup (0x4B23D0, con hook; ECX = memoria,
// orden PLAYER_GROUP_COMMAND_ATTACK y el objetivo m_target de este evento), deja la vtable de la clase del EXE (0x86C710; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria.
CEventPlayerCommandToGroupAttack* CEventPlayerCommandToGroupAttack::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventPlayerCommandToGroupAttack));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventPlayerCommandToGroup*>(memory)->Constructor(PLAYER_GROUP_COMMAND_ATTACK, m_target);
    *reinterpret_cast<uint32*>(memory) = 0x86C710;
    return static_cast<CEventPlayerCommandToGroupAttack*>(memory);
}

// 0x5F6340
CEventPlayerCommandToGroupAttack* CEventPlayerCommandToGroupAttack::Constructor(CPed* target)
{
    this->CEventPlayerCommandToGroupAttack::CEventPlayerCommandToGroupAttack(target);
    return this;
}

// 0x4B2530
bool CEventPlayerCommandToGroupAttack::AffectsPedGroup(CPedGroup* pedGroup)
{
    bool affectsPedGroup = CEventPlayerCommandToGroup::AffectsPedGroup(pedGroup);
    if (affectsPedGroup) {
        for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
            CPed* member = pedGroup->GetMembership().GetMember(i);
            if (member && !member->bDoesntListenToPlayerGroupCommands)
                return true;
        }
        return false;
    }
    return affectsPedGroup;
}