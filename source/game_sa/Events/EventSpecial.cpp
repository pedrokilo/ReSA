// Sin PCH (CMakeLists.txt): StdInc.h redefine NOMINMAX y el aviso C4005 es error; mismo recurso que EventEditableResponse.cpp y Task.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "EventSpecial.h"

namespace {
// 0x4B1B50 (lote 139A, banco Fn139A; thiscall, sin argumentos de pila, `ret`): cuerpo no virtual de ~CEventSpecial del EXE. La cadena de destructores (CEventEditableResponse, CEvent) se
// reduce a `mov dword [ecx],0x85AB38; ret`: deja la vtable de CEvent del EXE y conserva EAX, ECX y EDX. Su unico llamador (0x4B6750, el destructor con borrado) sigue usando ECX tras la
// llamada (lo copia a EDX para localizar el evento en la reserva y lo devuelve en EAX), asi que el cuerpo no puede llevar marco ni comprobaciones: se optimiza por pragma (CMakeLists.txt:
// /Zi y BasicRuntimeChecks=Default para este archivo). __fastcall con un segundo registro sin usar equivale a thiscall sin argumentos.
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
void __fastcall SpecialDestructorBody(CEventSpecial* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x85AB38;
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)
} // namespace

void CEventSpecial::InjectHooks() {
    RH_ScopedVirtualClass(CEventSpecial, 0x85B5D0, 17);
    RH_ScopedCategory("Events");
    RH_ScopedNamedGlobalInstall(SpecialDestructorBody, "Destructor", 0x4B1B50);
}
