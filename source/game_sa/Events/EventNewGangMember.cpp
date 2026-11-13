/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventNewGangMember
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
#include "EventNewGangMember.h"

namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x609080 (lote 168A, banco Fn168A): cuerpo del destructor de CEventNewGangMember del EXE (thiscall, sin argumentos de pila, `ret`, 90 bytes con marco SEH que solo desenrolla). Deja la vtable de la clase del EXE (0x86D0A0; la de la
// DLL no vale para los llamadores del EXE), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula del miembro (+0xC) y llama al cuerpo de ~CEvent (0x4AC010, con hook propio). Funcion propia sin expansion en
// linea para que el banco pueda sustituirla; la llama el destructor con borrado como el `call 0x609080` del EXE.
__declspec(noinline) void __fastcall NewGangMemberDestructorBody(CEventNewGangMember* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86D0A0;
    CEntity::SafeCleanUpRef(self->m_member);
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x609060 (lote 168A, banco Fn168A): destructor con borrado de CEventNewGangMember del EXE (ranura 0 de su vtable 0x86D0A0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x609080;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x609080), libera `this` con CEvent::operator delete (0x4B5630, con hook)
// solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventNewGangMember* __fastcall NewGangMemberScalarDeletingDestructor(CEventNewGangMember* self, void* /*edx*/, uint8 flags) {
    NewGangMemberDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventNewGangMember::InjectHooks()
{
    RH_ScopedVirtualClass(CEventNewGangMember, 0x86D0A0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(NewGangMemberDestructorBody, "DestructorBody", 0x609080); // lote 168A (banco Fn168A)
    RH_ScopedNamedGlobalInstall(NewGangMemberScalarDeletingDestructor, "ScalarDeletingDestructor", 0x609060); // lote 168A (banco Fn168A)
    RH_ScopedInstall(Constructor, 0x608F70);
    RH_ScopedVMTInstall(GetEventType, 0x608FD0);      // lote 168A (banco Fn168A): retorno de constante
    RH_ScopedVMTInstall(GetEventPriority, 0x609310);  // lote 179A (sin validar): ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x608FE0);       // lote 168A (banco Fn168A): retorno de constante
    RH_ScopedVMTInstall(Clone, 0x608FF0);             // lote 168A (banco Fn168A)
    RH_ScopedVMTInstall(AffectsPed, 0x609050);        // lote 168A (banco Fn168A): retorno de constante
}

// 0x608FD0 (lote 168A, banco Fn168A; thiscall, `ret`, 6 bytes: `mov eax,5Ah`): EVENT_NEW_GANG_MEMBER (90); no lee el objeto (ranura 1 de la vtable 0x86D0A0).
eEventType CEventNewGangMember::GetEventType() const {
    return EVENT_NEW_GANG_MEMBER;
}

// 0x609310 (lote 179A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,2Ah`): prioridad 42; no lee el objeto (ranura 2 de la vtable 0x86D0A0).
int32 CEventNewGangMember::GetEventPriority() const {
    return 42;
}

// 0x608FE0 (lote 168A, banco Fn168A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3).
int32 CEventNewGangMember::GetLifeTime() {
    return 0;
}

// 0x608FF0 (lote 168A, banco Fn168A; thiscall, sin argumentos, `ret`, 96 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x86D0A0).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x10 bytes; sin memoria devuelve nulo; si no, llama al constructor (0x608F70, con hook; ECX = memoria) con el miembro (+0xC, dword completo) y devuelve
// lo que el constructor deja en EAX.
CEventNewGangMember* CEventNewGangMember::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventNewGangMember));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventNewGangMember*>(memory)->Constructor(m_member);
}

// 0x609050 (lote 168A, banco Fn168A; thiscall, un puntero de pila, `ret 4`, 5 bytes: `mov al,1`): siempre verdadero; no lee ni el objeto ni el peaton (ranura 5).
bool CEventNewGangMember::AffectsPed(CPed* ped) {
    return true;
}

// 0x608F70
CEventNewGangMember::CEventNewGangMember(CPed* member)
{
    m_member = member;
    CEntity::SafeRegisterRef(m_member);
}

CEventNewGangMember::~CEventNewGangMember()
{
    CEntity::SafeCleanUpRef(m_member);
}

// 0x608F70
CEventNewGangMember* CEventNewGangMember::Constructor(CPed* member)
{
    this->CEventNewGangMember::CEventNewGangMember(member);
    return this;
}

