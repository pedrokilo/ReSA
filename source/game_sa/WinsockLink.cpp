#include "StdInc.h"

#include "WinsockLink.h"

namespace {
// 0x59CBA0 (lote 178A, sin validar; el listado de entrada decia CQuaternion): cuerpo del destructor de la base abstracta de red del EXE (thiscall, sin argumentos de pila, `ret`, 7 bytes: `mov [ecx],866DB0h; ret`). Deja la
// vtable de la base (0x866DB0) y no hace nada mas; no escribe EAX. Funcion propia sin expansion en linea; la llama el destructor con borrado de la clase derivada (0x59CD50) como el `call 0x59CBA0` del EXE.
__declspec(noinline) void __fastcall WinsockLinkBaseDestructorBody(void* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x866DB0;
}

// 0x59CD50 (lote 178A, sin validar; el listado de entrada decia CQuaternion): destructor con borrado de la clase derivada de red del EXE (ranura 0 de su vtable 0x866DC4; thiscall, un argumento de pila, `ret 4`, 27 bytes:
// `push esi; mov esi,ecx; call 0x59CBA0; test byte [esp+8],1; je ..; push esi; call 0x8214BD; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la base (0x59CBA0), libera `this` con el
// operator delete del CRT del EXE (0x8214BD, cdecl, por direccion: el objeto lo reserva el EXE) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (sin ranura nombrable).
__declspec(noinline) void* __fastcall WinsockLinkScalarDeletingDestructor(void* self, void* /*edx*/, uint8 flags) {
    WinsockLinkBaseDestructorBody(self, nullptr);
    if (flags & 1) {
        reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(self);
    }
    return self;
}
} // namespace

void CWinsockLink::InjectHooks() {
    RH_ScopedClass(CWinsockLink);
    RH_ScopedCategoryGlobal();

    RH_ScopedNamedGlobalInstall(WinsockLinkBaseDestructorBody, "BaseDestructorBody", 0x59CBA0);                // lote 178A (sin validar)
    RH_ScopedNamedGlobalInstall(WinsockLinkScalarDeletingDestructor, "ScalarDeletingDestructor", 0x59CD50);   // lote 178A (sin validar)
}
