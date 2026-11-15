/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventHealthReallyLow
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

#include "EventHealthReallyLow.h"

namespace CEventEditableBody {
void __fastcall EditableResponseDestructorBody(CEventEditableResponse* self, void* edx); // EventEditableResponse.cpp: 0x4AC480
}

namespace {
// 0x5FF520 (lote 197A, sin validar): cuerpo del destructor de CEventHealthReallyLow del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86CBE8h; jmp 0x4AC480`). Deja la vtable de la clase del EXE (0x86CBE8)
// y sigue con el cuerpo de ~CEventEditableResponse (0x4AC480, con hook, que deja la vtable de CEvent). Funcion propia sin expansion en linea; la llama el destructor con borrado del EXE (0x5FF5A0).
__declspec(noinline) void __fastcall HealthReallyLowDestructorBody(CEventHealthReallyLow* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CBE8;
    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);
}

// 0x5FF5A0 (lote 197A, sin validar): destructor con borrado de CEventHealthReallyLow del EXE (ranura 0 de su vtable 0x86CBE8; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x5FF520;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FF520), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventHealthReallyLow* __fastcall HealthReallyLowScalarDeletingDestructor(CEventHealthReallyLow* self, void* /*edx*/, uint8 flags) {
    HealthReallyLowDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventHealthReallyLow::InjectHooks()
{
    RH_ScopedVirtualClass(CEventHealthReallyLow, 0x86CBE8, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FF500);                                                  // lote 197A (sin validar)
    RH_ScopedNamedGlobalInstall(HealthReallyLowDestructorBody, "DestructorBody", 0x5FF520);          // lote 197A (sin validar)
    RH_ScopedNamedGlobalInstall(HealthReallyLowScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FF5A0); // lote 197A (sin validar)
    RH_ScopedVMTInstall(CloneEditable, 0x5FF550);                                           // lote 197A (sin validar)
}

// 0x5FF500 (lote 197A, sin validar; thiscall, sin argumentos de pila, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x4AC450; mov [esi],86CBE8h; mov eax,esi; pop esi; ret`). Constructor de
// CEventEditableResponse (0x4AC450, con hook) y vtable de la clase del EXE (0x86CBE8); devuelve this.
CEventHealthReallyLow* CEventHealthReallyLow::Constructor() {
    static_cast<CEventEditableResponse*>(this)->Constructor();
    *reinterpret_cast<uint32*>(this) = 0x86CBE8;
    return this;
}

// 0x5FF550 (lote 197A, sin validar; thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86CBE8).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo (EAX = 0); si no, CEventEditableResponse::CEventEditableResponse (0x4AC450, con hook; ECX = memoria),
// deja la vtable de la clase del EXE (0x86CBE8; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria. No lee `this`.
CEventEditableResponse* CEventHealthReallyLow::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventHealthReallyLow));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventEditableResponse*>(memory)->Constructor();
    *reinterpret_cast<uint32*>(memory) = 0x86CBE8;
    return static_cast<CEventHealthReallyLow*>(memory);
}
