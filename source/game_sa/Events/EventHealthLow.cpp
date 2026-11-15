/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventHealthLow
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EventHealthLow.h"

namespace CEventEditableBody {
void __fastcall EditableResponseDestructorBody(CEventEditableResponse* self, void* edx); // EventEditableResponse.cpp: 0x4AC480
}

namespace {
// 0x5FF460 (lote 196A, sin validar; aqui SIN hook: lo valida el lote 196A): cuerpo del destructor de CEventHealthLow del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86CBA0h; jmp 0x4AC480`). Deja la vtable de la clase del EXE (0x86CBA0)
// y sigue con el cuerpo de ~CEventEditableResponse (0x4AC480, con hook, que deja la vtable de CEvent). Funcion propia sin expansion en linea; la llama el destructor con borrado del EXE (0x5FF4E3) y la alcanzan por salto
// los desenrollados SEH de 0x83E72B y 0x83E743.
__declspec(noinline) void __fastcall HealthLowDestructorBody(CEventHealthLow* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CBA0;
    CEventEditableBody::EditableResponseDestructorBody(self, nullptr);
}

// 0x5FF4E0 (lote 197A, sin validar): destructor con borrado de CEventHealthLow del EXE (ranura 0 de su vtable 0x86CBA0; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x5FF460;
// test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FF460), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventHealthLow* __fastcall HealthLowScalarDeletingDestructor(CEventHealthLow* self, void* /*edx*/, uint8 flags) {
    HealthLowDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventHealthLow::InjectHooks()
{
    RH_ScopedVirtualClass(CEventHealthLow, 0x86CBA0, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(HealthLowScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FF4E0); // lote 197A (sin validar)
    RH_ScopedVMTInstall(CloneEditable, 0x5FF490);                                               // lote 197A (sin validar)
}

// 0x5FF490 (lote 197A, sin validar; thiscall, sin argumentos, `ret`, 80 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86CBA0).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x14 bytes; sin memoria devuelve nulo (EAX = 0); si no, CEventEditableResponse::CEventEditableResponse (0x4AC450, con hook; ECX = memoria),
// deja la vtable de la clase del EXE (0x86CBA0; la de la DLL no vale para los llamadores del EXE) y devuelve la memoria. No lee `this`.
CEventEditableResponse* CEventHealthLow::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventHealthLow));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventEditableResponse*>(memory)->Constructor();
    *reinterpret_cast<uint32*>(memory) = 0x86CBA0;
    return static_cast<CEventHealthLow*>(memory);
}
