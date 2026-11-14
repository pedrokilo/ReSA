/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventAcquaintancePedHate
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventAcquaintancePedHate.h"

namespace CEventAcquaintancePedBody {
void __fastcall AcquaintancePedDestructorBody(CEventAcquaintancePed* self, void* edx); // EventAcquaintancePed.cpp: 0x4AF890
}

namespace {
// 0x420F30 (lote 190A, sin validar): cuerpo del destructor de CEventAcquaintancePedHate del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],858E68h; jmp 0x4AF890`). Deja la vtable de la clase
// del EXE (0x858E68) y sigue con el cuerpo de ~CEventAcquaintancePed (0x4AF890, con hook). No es un salto puro: escribe la vtable antes. Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call` del EXE.
__declspec(noinline) void __fastcall HateDestructorBody(CEventAcquaintancePedHate* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x858E68;
    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);
}

// 0x420FB0 (lote 190A, sin validar): destructor con borrado de CEventAcquaintancePedHate del EXE (ranura 0 de su vtable 0x858E68; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x420F30; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x420F30), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventAcquaintancePedHate* __fastcall HateScalarDeletingDestructor(CEventAcquaintancePedHate* self, void* /*edx*/, uint8 flags) {
    HateDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventAcquaintancePedHate::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAcquaintancePedHate, 0x858E68, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(HateDestructorBody, "DestructorBody", 0x420F30);                         // lote 190A (sin validar)
    RH_ScopedNamedGlobalInstall(HateScalarDeletingDestructor, "ScalarDeletingDestructor", 0x420FB0);      // lote 190A (sin validar)
    RH_ScopedInstall(Constructor2, 0x420E70);
    RH_ScopedVMTInstall(GetEventType, 0x420F40);                // lote 190A (sin validar): ranura 1
    RH_ScopedVMTInstall(GetEventPriority, 0x420FD0);            // lote 190A (sin validar): ranura 2
    RH_ScopedVMTInstall(GetLifeTime, 0x420F00);                 // lote 190A (sin validar): ranura 3
    RH_ScopedVMTInstall(GetSourceEntity, 0x420F10);             // lote 190A (sin validar): ranura 10
    RH_ScopedVMTInstall(CanBeInterruptedBySameEvent, 0x420F20); // lote 190A (sin validar): ranura 15
    RH_ScopedVMTInstall(CloneEditable, 0x420F50);               // lote 190A (sin validar): ranura 16
}

// 0x420F40 (lote 190A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,24h`): EVENT_ACQUAINTANCE_PED_HATE (36); no lee el objeto (ranura 1 de la vtable 0x858E68).
eEventType CEventAcquaintancePedHate::GetEventType() const {
    static_assert(EVENT_ACQUAINTANCE_PED_HATE == 0x24);
    return EVENT_ACQUAINTANCE_PED_HATE;
}

// 0x420FD0 (lote 190A, sin validar; thiscall, `ret`, 6 bytes: `mov eax,1Ah`): prioridad 26; no lee el objeto (ranura 2).
int32 CEventAcquaintancePedHate::GetEventPriority() const {
    return 26;
}

// 0x420F00 (lote 190A, sin validar; thiscall, `ret`, 3 bytes: `xor eax,eax`): vida util 0; no lee el objeto (ranura 3).
int32 CEventAcquaintancePedHate::GetLifeTime() {
    return 0;
}

// 0x420F10 (lote 190A, sin validar; thiscall, `ret`, 4 bytes: `mov eax,[ecx+14h]`): el conocido (m_AcquaintancePed, +0x14) como entidad, sin comprobar nulo (ranura 10).
CEntity* CEventAcquaintancePedHate::GetSourceEntity() const {
    return reinterpret_cast<CEntity*>(m_AcquaintancePed);
}

// 0x420F20 (lote 190A, sin validar; thiscall, `ret`, 3 bytes: `mov al,1`): siempre verdadero; solo se define AL (ranura 15).
bool CEventAcquaintancePedHate::CanBeInterruptedBySameEvent() {
    return true;
}

// 0x420F50 (lote 190A, sin validar; thiscall, sin argumentos, `ret`, 88 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x858E68).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x18 bytes; sin memoria devuelve nulo; si no, llama al constructor de CEventAcquaintancePed (0x4AF820, con hook) con el conocido (+0x14),
// deja la vtable de la clase y devuelve la memoria. En C++ el constructor heredado de CEventAcquaintancePedHate hace lo mismo.
CEventEditableResponse* CEventAcquaintancePedHate::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventAcquaintancePedHate));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CEventAcquaintancePedHate(m_AcquaintancePed);
}

// 0x420E70
CEventAcquaintancePedHate* CEventAcquaintancePedHate::Constructor2(CPed* ped)
{
    this->CEventAcquaintancePedHate::CEventAcquaintancePedHate(ped);
    return this;
}

