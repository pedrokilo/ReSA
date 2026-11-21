#pragma once

// std::exception del CRT estatico del EXE (MSVC 7.1) y los dos lanzadores de std::string (_Xran, _Xlen), lote CRT3A.
//
// std::exception en el EXE: 12 bytes {vtable 0x887E80, texto (+4), bandera «el texto es propio» (+8)}. MSVC no admite `__thiscall` en funciones libres, asi que la clase se modela
// aqui sin metodos virtuales (el puntero a la vtable es un campo mas: se escribe el 0x887E80 del EXE, por lo que los objetos creados por el proyecto y por el EXE son intercambiables).
//   0x822D97 constructor   0x822DE5 constructor de copia (ret 4)   0x822E2F destructor (no libera nada mas)
// Lanzadores (marcos con _EH_prolog y tablas de desenrollado propias del EXE, no se pueden sustituir por un `throw` de C++: la excepcion lanzada debe ser el objeto del EXE
// std::out_of_range / std::length_error con su ThrowInfo del EXE): se reconstruyen construyendo ese mismo objeto y llamando a _CxxThrowException (0x822D5D, ya en CrtFile);
// la tabla de desenrollado del EXE (un solo estado: destruir el std::string temporal con _Tidy 0x4F3BA0) se sustituye por un objeto RAII.
//   0x8211FD std::string::_Xran ("invalid string position", vtable 0x887D74, ThrowInfo 0x8A1E54)
//   0x821255 std::string::_Xlen ("string too long", vtable 0x887D68, ThrowInfo 0x893634)

#include <cstdint>

namespace notsa {
struct StdException {
    const void* m_vtbl;     // +0, 0x887E80 (vtable de std::exception del EXE)
    const char* m_what;     // +4
    int32_t     m_doFree;   // +8, distinto de cero si m_what es una copia propia (se libera en el destructor)

    StdException* Construct();                           // 0x822D97
    StdException* CopyConstruct(const StdException& o);  // 0x822DE5
    void          Destruct();                            // 0x822E2F
};
static_assert(sizeof(StdException) == 0xC);
} // namespace notsa

namespace ExeCrt {
[[noreturn]] void __cdecl StringXran(); // 0x8211FD
[[noreturn]] void __cdecl StringXlen(); // 0x821255
} // namespace ExeCrt

void CrtExceptionInjectHooks();
