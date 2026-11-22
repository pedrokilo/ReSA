#pragma once

#include <cstdint>

// Destructores con borrado escalar de la familia std::exception del CRT del EXE (MSVC 7.1) y std::exception::what, lote P4 E2-montón/E2-sin. Son las entradas 0 de las tablas virtuales
// de std::bad_alloc, std::exception, std::logic_error/out_of_range/length_error y std::bad_cast-like (0x887D68..0x887EC4): solo se llaman por puntero desde la tabla (o desde el
// codigo de excepciones del EXE), por eso el catalogo las veia como «tabla».
// Todas son thiscall con un argumento de pila (`ret 4`); se modelan como __fastcall con EDX de relleno (misma disposicion de registros y de pila):
//     push esi; mov esi,ecx; call cuerpo; test byte [esp+8],1; je ..; push esi; call operator delete (0x8214BD); pop ecx; mov eax,esi; pop esi; ret 4
//   0x8211D6 -> cuerpo 0x8211F2 (vtable 0x887D74 y salto a 0x4F3D10)   0x8212B9 -> cuerpo 0x8212AE (vtable 0x887DA8 y destructor 0x822E2F)
//   0x822F05 -> cuerpo 0x822E2F (std::exception, ya reconstruido)      0x822F40 -> 0x822E83   0x822F5C -> 0x822EBF   0x822F78 -> 0x822EFA   0x822FBD -> 0x822F94
//   0x822E45 what(): devuelve el texto (+4) o, si es nulo, el literal del EXE en 0x887E88 («unknown exception»)
// Los cuerpos 0x822E83 (vtable 0x887EA0), 0x822EBF y 0x822EFA (vtable 0x887EAC) y 0x822F94 (vtable 0x887EC4 y libera el texto bajo el bloqueo 0xE) estan reconstruidos aqui y enganchados;
// solo 0x4F3D10 (cuerpo de 0x8211F2, el destructor de std::string) se sigue llamando por su direccion.

namespace ExeCrt {
void*       __fastcall DeletingDtor8211D6(void* self, void* edx, uint32_t flags); // 0x8211D6
void        __fastcall DtorBody8211F2(void* self, void* edx);                      // 0x8211F2
void*       __fastcall DeletingDtor8212B9(void* self, void* edx, uint32_t flags); // 0x8212B9
void        __fastcall DtorBody8212AE(void* self, void* edx);                      // 0x8212AE
void*       __fastcall DeletingDtor822F05(void* self, void* edx, uint32_t flags); // 0x822F05
void        __fastcall DtorBody822E83(void* self, void* edx);                      // 0x822E83
void        __fastcall DtorBody822EBF(void* self, void* edx);                      // 0x822EBF
void        __fastcall DtorBody822EFA(void* self, void* edx);                      // 0x822EFA
void        __fastcall DtorBody822F94(void* self, void* edx);                      // 0x822F94
void*       __fastcall DeletingDtor822F40(void* self, void* edx, uint32_t flags); // 0x822F40
void*       __fastcall DeletingDtor822F5C(void* self, void* edx, uint32_t flags); // 0x822F5C
void*       __fastcall DeletingDtor822F78(void* self, void* edx, uint32_t flags); // 0x822F78
void*       __fastcall DeletingDtor822FBD(void* self, void* edx, uint32_t flags); // 0x822FBD
const char* __fastcall StdExceptionWhat(const void* self, void* edx);              // 0x822E45
} // namespace ExeCrt

void CrtExceptionDtorInjectHooks();
