#include "StdInc.h"
#include "EventAcquaintancePedDislike.h"

namespace CEventAcquaintancePedBody {
void __fastcall AcquaintancePedDestructorBody(CEventAcquaintancePed* self, void* edx); // EventAcquaintancePed.cpp: 0x4AF890
}

namespace {
// 0x5FF1B0 (lote 188A, sin validar): cuerpo del destructor de CEventAcquaintancePedDislike del EXE (thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov [ecx],86CAC8h; jmp 0x4AF890`). Deja la vtable de la clase
// del EXE (0x86CAC8) y sigue con el cuerpo de ~CEventAcquaintancePed (0x4AF890, con hook). No es un salto puro: escribe la vtable antes. Funcion propia sin expansion en linea; la llama el destructor con borrado como el `call` del EXE.
__declspec(noinline) void __fastcall DislikeDestructorBody(CEventAcquaintancePedDislike* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CAC8;
    CEventAcquaintancePedBody::AcquaintancePedDestructorBody(self, nullptr);
}

// 0x5FF230 (lote 188A, sin validar): destructor con borrado de CEventAcquaintancePedDislike del EXE (ranura 0 de su vtable 0x86CAC8; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx;
// call 0x5FF1B0; test byte [esp+8],1; je ..; push esi; call 0x4B5630; add esp,4; mov eax,esi; pop esi; ret 4`). Como el original: destruye con el cuerpo de la clase (0x5FF1B0), libera `this` con CEvent::operator delete
// (0x4B5630, con hook) solo si el bit 0 del BYTE de banderas esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) CEventAcquaintancePedDislike* __fastcall DislikeScalarDeletingDestructor(CEventAcquaintancePedDislike* self, void* /*edx*/, uint8 flags) {
    DislikeDestructorBody(self, nullptr);
    if (flags & 1) {
        CEvent::operator delete(self);
    }
    return self;
}
} // namespace

void CEventAcquaintancePedDislike::InjectHooks()
{
    RH_ScopedVirtualClass(CEventAcquaintancePedDislike, 0x86CAC8, 17);
    RH_ScopedCategory("Events");

    RH_ScopedNamedGlobalInstall(DislikeDestructorBody, "DestructorBody", 0x5FF1B0);                           // lote 188A (sin validar)
    RH_ScopedNamedGlobalInstall(DislikeScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5FF230);        // lote 188A (sin validar)
    RH_ScopedVMTInstall(CloneEditable, 0x5FF1D0);                                                          // lote 188A (sin validar)
}

// 0x5FF1D0 (lote 188A, sin validar; thiscall, sin argumentos, `ret`, 88 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite); ranura 16 de la vtable 0x86CAC8).
// Como el original: CEvent::operator new (0x4B5620, con hook) con 0x18 bytes; sin memoria devuelve nulo; si no, llama al constructor de CEventAcquaintancePed (0x4AF820, con hook) con el conocido (+0x14),
// deja la vtable de la clase y devuelve la memoria. En C++ el constructor heredado de CEventAcquaintancePedDislike hace lo mismo.
CEventEditableResponse* CEventAcquaintancePedDislike::CloneEditable() const noexcept {
    void* const memory = CEvent::operator new(sizeof(CEventAcquaintancePedDislike));
    if (!memory) {
        return nullptr;
    }
    return ::new (memory) CEventAcquaintancePedDislike(m_AcquaintancePed);
}
