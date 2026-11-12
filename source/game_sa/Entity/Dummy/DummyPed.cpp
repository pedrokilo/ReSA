/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CDummyPed
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

#include "DummyPed.h"

namespace {
// 0x5DE2E0 (lote 178A, sin validar; el listado de entrada decia CCopPed): cuerpo del constructor de CDummyPed del EXE (thiscall, sin argumentos, `ret`, 18 bytes: `push esi; mov esi,ecx; call 0x532540; mov [esi],86C198h;
// mov eax,esi; pop esi; ret`). Llama al cuerpo del constructor de CDummy (0x532540, con hook), deja la vtable de la clase del EXE (0x86C198) y devuelve this. Sin mas miembros. Funcion propia (la clase usa el constructor
// generado por el compilador).
__declspec(noinline) CDummyPed* __fastcall DummyPedConstructorBody(CDummyPed* self, void* /*edx*/) {
    CDummyBody::Constructor(self, nullptr);
    *reinterpret_cast<uint32*>(self) = 0x86C198;
    return self;
}

// 0x5DE300 (lote 178A, sin validar; el listado decia CCopPed): cuerpo del destructor de CDummyPed del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86C198h; jmp 0x532560`). Deja la vtable de la clase
// del EXE (0x86C198) y salta al cuerpo del destructor de CDummy (0x532560, con hook). Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call 0x5DE300` del EXE.
__declspec(noinline) void __fastcall DummyPedDestructorBody(CDummyPed* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86C198;
    CDummyBody::Destructor(self, nullptr);
}

// 0x5DE310 (lote 178A, sin validar; el listado decia CCopPed): destructor con borrado de CDummyPed del EXE (ranura 0 de su vtable 0x86C198; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x5DE300; test byte [esp+8],1; je ..; push esi; call 0x5326F0; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5DE300), libera `this` con CDummy::operator delete
// (0x5326F0, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CDummyPed* __fastcall DummyPedScalarDeletingDestructor(CDummyPed* self, void* /*edx*/, uint8 flags) {
    DummyPedDestructorBody(self, nullptr);
    if (flags & 1) {
        CDummy::operator delete(self);
    }
    return self;
}
} // namespace

void CDummyPed::InjectHooks() {
    RH_ScopedClass(CDummyPed);
    RH_ScopedCategory("Entity/Dummy");

    RH_ScopedNamedGlobalInstall(DummyPedConstructorBody, "Constructor", 0x5DE2E0);                       // lote 178A (sin validar)
    RH_ScopedNamedGlobalInstall(DummyPedDestructorBody, "DestructorBody", 0x5DE300);                     // lote 178A (sin validar)
    RH_ScopedNamedGlobalInstall(DummyPedScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5DE310); // lote 178A (sin validar)
}
