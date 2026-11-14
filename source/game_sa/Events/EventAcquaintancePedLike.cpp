/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventAcquaintancePedLike
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
#include "EventAcquaintancePedLike.h"

namespace CEventAcquaintancePedBody {
void __fastcall AcquaintancePedDestructorBody(CEventAcquaintancePed* self, void* edx); // EventAcquaintancePed.cpp: 0x4AF890
}

namespace {
// 0x5FF0F0 (lote 188A, sin validar): cuerpo del destructor de CEventAcquaintancePedLike del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86CA80h; jmp 0x4AF890`). Deja la vtable de la clase
// del EXE (0x86CA80) y sigue con el cuerpo de ~CEventAcquaintancePed (0x4AF890, con hook). No es un salto puro: escribe la vtable antes. Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call` del EXE.
__declspec(noinline) void __fastcall LikeDestructorBody(CEventAcquaintancePedLike* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CA80;
    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);
}

// 0x5FF170 (lote 188A, sin validar): destructor con borrado de CEventAcquaintancePedLike del EXE (ranura 0 de su vtable 0x86CA80; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x5FF0F0; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FF0F0), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventAcquaintancePedLike* __fastcall LikeScalarDeletingDestructor(CEventAcquaintancePedLike* self, void* /*edx*/, uint8 flags) {
    LikeDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventAcquaintancePedLike::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAcquaintancePedLike, 0x86CA80, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(LikeDestructorBody, "DestructorBody", 0x5FF0F0);                           // lote 188A (sin validar)
    RH_ScopedNamedGlobalInstall(LikeScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FF170);        // lote 188A (sin validar)
    RH_ScopedVMTInstall(CloneEditable, 0x5FF110);                                                          // lote 188A (sin validar)
}

// 0x5FF110 (lote 188A, sin validar; thiscall, sin argumentos, `ret`, 88 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86CA80).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x18 bytes; sin memoria devuelve nulo; si no, llama al constructor de CEventAcquaintancePed (0x4AF820, con hook) con el conocido (+0x14),
// deja la vtable de la clase del EXE (0x86CA80) y devuelve la memoria. Llama al envoltorio enganchado del constructor (0x4AF820), no al constructor de C++ (que dejaria la vtable de la DLL).
CEventEditableResponse* CEventAcquaintancePedLike::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventAcquaintancePedLike));
    if (!memory) {
        return nullptr;
    }
    static_cast<CEventAcquaintancePed*>(memory)->Constructor(m_AcquaintancePed);
    *reinterpret_cast<uint32*>(memory) = 0x86CA80;
    return static_cast<CEventAcquaintancePedLike*>(memory);
}
