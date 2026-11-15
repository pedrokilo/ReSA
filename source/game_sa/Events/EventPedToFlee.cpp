/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPedToFlee
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

#include "EventPedToFlee.h"

namespace {
// Cuerpo no virtual de ~CEventPedToFlee del EXE (thiscall, sin argumentos de pila, `ret`): deja la vtable de CEventPedToFlee del EXE (0x85B178), libera con CEntity::CleanUpOldReference (0x571A00) las referencias no nulas
// en el orden del EXE y deja la vtable de CEvent del EXE (0x85AB38). El EXE lo envuelve en un marco SEH que solo desenrolla. (Lote 117A, banco Fn117A.)
void __fastcall PedToFleeDestructorBody(CEventPedToFlee* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85B178;
    CEntity::SafeCleanUpRef(self->m_ped);
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
// 0x4B6210 (lote 205A, sin validar; el listado decia CEventDamage; ranura 0 de la vtable 0x85B178 de CEventPedToFlee; thiscall, un argumento de pila, `ret 4`, 76 bytes: `push esi; push edi; mov edi,ecx; call 0x4AF2D0; test byte [esp+0Ch],1; je ..;` y, con el bit 0 del BYTE de banderas,
// el operator delete de CEvent EN LINEA (pool de eventos 0xB744AC, indice = (this - base) / 0x44 con signo, estado |= 0x80, cursor = min(cursor, indice) con signo); devuelve `this`): destructor con borrado de CEventPedToFlee del EXE.
// Como el original: destruye con el cuerpo de la clase (0x4AF2D0), libera `this` con CEvent::operator delete (0x4B5630, con hook; mismo efecto que la copia en linea) solo si el bit 0 esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventPedToFlee* __fastcall PedToFleeScalarDeletingDestructor(CEventPedToFlee* self, void* /*edx*/, uint8 flags) {
    PedToFleeDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventPedToFlee::InjectHooks() {
    RH_ScopedVirtualClass(CEventPedToFlee, 0x85B178, 16);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(PedToFleeDestructorBody, "Destructor", 0x4AF2D0);
    RH_ScopedNamedGlobalInstall(PedToFleeScalarDeletingDestructor, "ScalarDeletingDestructor", 0x4B6210); // lote 205A (sin validar)

    RH_ScopedInstall(Constructor, 0x4AF240);
    RH_ScopedVMTInstall(Clone, 0x4B73D0); // lote 199A (sin validar): ranura 4
}

// 0x4AF240
CEventPedToFlee::CEventPedToFlee(CPed* ped) {
    m_ped = ped;
    CEntity::SafeRegisterRef(m_ped);
}

// 0x4AF240
CEventPedToFlee* CEventPedToFlee::Constructor(CPed* ped) {
    this->CEventPedToFlee::CEventPedToFlee(ped);
    return this;
}

// 0x4B73D0 (lote 199A, sin validar; thiscall, sin argumentos, `ret`, 97 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 4 de la vtable 0x85B178).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x10 bytes; sin memoria devuelve nulo sin leer campos; si no, el constructor 0x4AF240 (Constructor, con hook) con el peaton (+0xC) y devuelve la memoria.
CEvent* CEventPedToFlee::Clone() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventPedToFlee));
    if (!memory) {
        return nullptr;
    }
    return static_cast<CEventPedToFlee*>(memory)->Constructor(m_ped);
}

// 0x4AF2D0
CEventPedToFlee::~CEventPedToFlee() {
    CEntity::SafeCleanUpRef(m_ped);
}
