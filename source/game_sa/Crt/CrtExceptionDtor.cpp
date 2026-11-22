#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include "CrtExceptionDtor.h"
#include "CrtException.h"
#include "CrtHeap.h"
#include "CrtState.h"

// Ver CrtExceptionDtor.h.

namespace ExeCrt {
namespace {
using BodyFn = void(__fastcall*)(void* self, void* edx);
inline BodyFn Body(uintptr_t address) { return reinterpret_cast<BodyFn>(address); }

// Cuerpo comun de los destructores con borrado escalar: destruye con `body`; si el bit 0 de la bandera (se lee como byte) esta puesto libera `self` con operator delete; devuelve `self`
inline void* DeletingDtor(BodyFn body, void* self, uint32_t flags) {
    body(self, nullptr);
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}
} // namespace

// 0x8211D6
void* __fastcall DeletingDtor8211D6(void* self, void*, uint32_t flags) { return DeletingDtor(&DtorBody8211F2, self, flags); }

// 0x8211F2: vtable = 0x887D74; jmp 0x4F3D10 (cuerpo del EXE)
void __fastcall DtorBody8211F2(void* self, void*) {
    *static_cast<uintptr_t*>(self) = 0x887D74;
    Body(0x4F3D10)(self, nullptr);
}

// 0x8212B9
void* __fastcall DeletingDtor8212B9(void* self, void*, uint32_t flags) { return DeletingDtor(&DtorBody8212AE, self, flags); }

// 0x8212AE: vtable = 0x887DA8; jmp 0x822E2F (std::exception::~exception)
void __fastcall DtorBody8212AE(void* self, void*) {
    *static_cast<uintptr_t*>(self) = 0x887DA8;
    static_cast<notsa::StdException*>(self)->Destruct();
}

// 0x822F05: cuerpo 0x822E2F (std::exception::~exception)
void* __fastcall DeletingDtor822F05(void* self, void*, uint32_t flags) {
    static_cast<notsa::StdException*>(self)->Destruct();
    if (flags & 1) {
        ::operator delete(self);
    }
    return self;
}

// 0x822E83: vtable = 0x887EA0; jmp 0x822E2F (std::exception::~exception)
void __fastcall DtorBody822E83(void* self, void*) {
    auto* const exception = static_cast<notsa::StdException*>(self);
    exception->m_vtbl = reinterpret_cast<const void*>(0x887EA0);
    exception->Destruct();
}

// 0x822EBF: vtable = 0x887EAC; jmp 0x822E2F
void __fastcall DtorBody822EBF(void* self, void*) {
    auto* const exception = static_cast<notsa::StdException*>(self);
    exception->m_vtbl = reinterpret_cast<const void*>(0x887EAC);
    exception->Destruct();
}

// 0x822EFA: vtable = 0x887EAC; jmp 0x822E2F (mismo cuerpo que 0x822EBF; el EXE tiene una copia por clase)
void __fastcall DtorBody822EFA(void* self, void*) {
    auto* const exception = static_cast<notsa::StdException*>(self);
    exception->m_vtbl = reinterpret_cast<const void*>(0x887EAC);
    exception->Destruct();
}

// 0x822F94: vtable = 0x887EC4; bajo el bloqueo 0xE libera el texto (+4) si no es nulo (free 0x82413F); no toca la bandera +8 ni anula el puntero
void __fastcall DtorBody822F94(void* self, void*) {
    auto* const exception = static_cast<notsa::StdException*>(self);
    exception->m_vtbl = reinterpret_cast<const void*>(0x887EC4);
    Lock(0xE);
    if (exception->m_what) {
        Free(const_cast<char*>(exception->m_what));
    }
    Unlock(0xE);
}

// 0x822F40, 0x822F5C, 0x822F78 y 0x822FBD: destructores con borrado escalar de las clases de arriba
void* __fastcall DeletingDtor822F40(void* self, void*, uint32_t flags) { return DeletingDtor(&DtorBody822E83, self, flags); }
void* __fastcall DeletingDtor822F5C(void* self, void*, uint32_t flags) { return DeletingDtor(&DtorBody822EBF, self, flags); }
void* __fastcall DeletingDtor822F78(void* self, void*, uint32_t flags) { return DeletingDtor(&DtorBody822EFA, self, flags); }
void* __fastcall DeletingDtor822FBD(void* self, void*, uint32_t flags) { return DeletingDtor(&DtorBody822F94, self, flags); }

// 0x822E45: mov eax,[ecx+4]; test eax,eax; jne ..; mov eax,0x887E88; ret
const char* __fastcall StdExceptionWhat(const void* self, void*) {
    const auto* what = static_cast<const notsa::StdException*>(self)->m_what;
    return what ? what : reinterpret_cast<const char*>(0x887E88);
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtExceptionDtorInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::DeletingDtor8211D6, 0x8211D6);
    RH_ScopedGlobalInstall(ExeCrt::DtorBody8211F2, 0x8211F2);
    RH_ScopedGlobalInstall(ExeCrt::DeletingDtor8212B9, 0x8212B9);
    RH_ScopedGlobalInstall(ExeCrt::DtorBody8212AE, 0x8212AE);
    RH_ScopedGlobalInstall(ExeCrt::DeletingDtor822F05, 0x822F05);
    RH_ScopedGlobalInstall(ExeCrt::DeletingDtor822F40, 0x822F40);
    RH_ScopedGlobalInstall(ExeCrt::DeletingDtor822F5C, 0x822F5C);
    RH_ScopedGlobalInstall(ExeCrt::DeletingDtor822F78, 0x822F78);
    RH_ScopedGlobalInstall(ExeCrt::DeletingDtor822FBD, 0x822FBD);
    RH_ScopedGlobalInstall(ExeCrt::DtorBody822E83, 0x822E83);
    RH_ScopedGlobalInstall(ExeCrt::DtorBody822EBF, 0x822EBF);
    RH_ScopedGlobalInstall(ExeCrt::DtorBody822EFA, 0x822EFA);
    RH_ScopedGlobalInstall(ExeCrt::DtorBody822F94, 0x822F94);
    RH_ScopedGlobalInstall(ExeCrt::StdExceptionWhat, 0x822E45);
}
#endif
