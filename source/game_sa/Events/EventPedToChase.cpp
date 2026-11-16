/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPedToChase
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventPedToChase.h"

namespace {
// Cuerpo no virtual de ~CEventPedToChase del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPedToChase del EXE (0x85B138), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall PedToChaseDestructorBody(CEventPedToChase* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B138;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B61C0 (lote 205A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable 0x85B138 de CEventPedToChase; thiscall, un argumento de pila, `ret 4`, 76 bytes: `push esi; push edi; mov edi,ecx; call 0x4AF1C0; test byte [esp+0Ch],1; je ..;` y, con el bit 0 del BYTE de banderas,
// el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice) con signo); devuelve `this`): destructor con borrado de CEventPedToChase del EXE.
// Como el original: destruye con el cuerpo de la clase (0x4AF1C0), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventPedToChase* __fastcall PedToChaseScalarDeletingDestructor(CEventPedToChase* self, void* /*edx*/, uint8 flags) {
    PedToChaseDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPedToChase::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedToChase, 0x85B138, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PedToChaseDestructorBody, "Destructor", 0x4AF1C0);
    RH_ScopedNamedGlobalInstall(PedToChaseScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B61C0); // lote 205A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AF130);
    RH_ScopedVMTInstall(Clone, 0x4B7360); // lote 199A (sin validar): ranura 4
}

// 0x4AF130
CEventPedToChase::CEventPedToChase(CPed* ped) {
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AF130
CEventPedToChase* CEventPedToChase::Constructor(CPed* ped) {
    this->CEventPedToChase::CEventPedToChase(ped);
    return this;
}

// 0x4B7360 (lote 199A, sin validar; thiscall, sin argumentos, `ret`, 97 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x85B138).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x10 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AF130 (Constructor, con hook) con el peaton (+0xC) y devuelve la memoria.
CEvent* CEventPedToChase::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventPedToChase));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventPedToChase*>(memory)->Constructor(m_ped);
}

// 0x4AF1C0
CEventPedToChase::~CEventPedToChase() {
    CEntity::SafeCleanUpRef(m_ped);
}
