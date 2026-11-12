/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventDontJoinPlayerGroup
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
#include "EventDontJoinPlayerGroup.h"


namespace CEventBody {
void __fastcall EventDestructorBody(CEvent* self, void* edx); // Event.cpp: 0x4AC010
}

namespace {
// 0x609140 (lote 115A, banco Fn115A): cuerpo no virtual de ~CEventDontJoinPlayerGroup del EXE (thiscall, sin argumentos de pila, `ret`)
// Deja la vtable de CEventDontJoinPlayerGroup del EXE (0x86D0E0), libera con CEntity::CleanUpOldReference (0x571A00) la referencia no nula en +0xC y llama al cuerpo de ~CEvent (0x4AC010), que deja la vtable de CEvent (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla.
__declspec(noinline) void __fastcall DontJoinPlayerGroupDestructorBody(CEventDontJoinPlayerGroup* self, void* /*edx*/) {
    uint8* const bytes = reinterpret_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(bytes) = 0x86D0E0;
    CEntity::SafeCleanUpRef(*reinterpret_cast<CEntity**>(bytes + 0xC));
    CEventBody::EventDestructorBody(self, nullptr);
}

// 0x609230 (lote 155A, banco Fn155A): destructor con borrado de CEventDontJoinPlayerGroup del EXE (ranura 0 de su vtable 0x86D0E0; thiscall, un argumento de pila, `ret 4`, 30 bytes:
// `push esi; mov esi,ecx; call 0x609140; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x609140, con hook;
// aqui DontJoinPlayerGroupDestructorBody, sin expansion en linea), libera `this` con CEvent::operator delete (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`.
// Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CEventDontJoinPlayerGroup* __fastcall DontJoinPlayerGroupScalarDeletingDestructor(CEventDontJoinPlayerGroup* self, void* /*edx*/, uint8 flags) {
    DontJoinPlayerGroupDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

uintptr_t Lote155ABody_CEventDontJoinPlayerGroup() { return reinterpret_cast<uintptr_t>(&DontJoinPlayerGroupDestructorBody); } // para el banco Fn155A

void CEventDontJoinPlayerGroup::InjectHooks()
{
    RH_ScopedVirtualClass(CEventDontJoinPlayerGroup, 0x86D0E0, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(DontJoinPlayerGroupDestructorBody, "Destructor", 0x609140);
    RH_ScopedNamedGlobalInstall(DontJoinPlayerGroupScalarDeletingDestructor, "ScalarDeletingDestructor", 0x609230); // lote 155A (banco Fn155A)

    RH_ScopedInstall(Constructor, 0x6090E0);
    RH_ScopedVMTInstall(GetEventType, 0x6091A0);  // lote 155A (banco Fn155A): ranura 1 de la vtable 0x86D0E0
    RH_ScopedVMTInstall(GetEventPriority, 0x609320); // lote 179A (sin validar): ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x6091B0);   // lote 155A: ranura 3
    RH_ScopedVMTInstall(Clone, 0x6091C0);         // lote 155A: ranura 4
    RH_ScopedVMTInstall(AffectsPed, 0x609220);    // lote 155A: ranura 5
}

// 0x6091A0 (lote 155A, banco Fn155A; thiscall, `ret`, 6 bytes: `mov eax,5Eh`): EVENT_DONT_JOIN_GROUP (94); no lee el objeto.
eEventType CEventDontJoinPlayerGroup::GetEventType() const {
    return EVENT_DONT_JOIN_GROUP;
}

// 0x609320 (lote 179A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,2Bh`): prioridad 43; no lee el objeto (ranura 2 de la vtable 0x86D0E0).
int32 CEventDontJoinPlayerGroup::GetEventPriority() const {
    return 43;
}

// 0x6091B0 (lote 155A, banco Fn155A; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto.
int32 CEventDontJoinPlayerGroup::GetLifeTime() {
    return 0;
}

// 0x609220 (lote 155A, banco Fn155A; thiscall, un argumento de pila, `ret 4`, 5 bytes: `mov al,1`): siempre verdadero; no lee el objeto ni el ped.
bool CEventDontJoinPlayerGroup::AffectsPed(CPed* /*ped*/) {
    return true;
}

// 0x6091C0 (lote 155A, banco Fn155A; thiscall, `ret`, 96 bytes con marco SEH que solo desenrolla): reserva el evento con CEvent::operator new (0x4B5620, con hook) para 0x10 bytes y, si hay memoria, llama
// al constructor (0x6090E0, con hook; aqui Constructor, sin expansion en linea) con el ped guardado (+0xC); sin memoria devuelve nulo. El marco SEH se omite.
CEventDontJoinPlayerGroup* CEventDontJoinPlayerGroup::Clone() const noexcept {
    auto* const object = static_cast<CEventDontJoinPlayerGroup*>(CEvent::operator new(sizeof(CEventDontJoinPlayerGroup)));
    return object ? object->Constructor(m_player) : nullptr;
}

// 0x6090E0
CEventDontJoinPlayerGroup::CEventDontJoinPlayerGroup(CPed* player)
{
    m_player = player;
    CEntity::SafeRegisterRef(m_player);
}

CEventDontJoinPlayerGroup::~CEventDontJoinPlayerGroup()
{
    CEntity::SafeCleanUpRef(m_player);
}

// 0x6090E0
CEventDontJoinPlayerGroup* CEventDontJoinPlayerGroup::Constructor(CPed* player)
{
    this->CEventDontJoinPlayerGroup::CEventDontJoinPlayerGroup(player);
    return this;
}

