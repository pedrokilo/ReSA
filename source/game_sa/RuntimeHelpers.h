#pragma once

// Ayudantes del compilador del EXE (MSVC) que el codigo original conserva y que la DLL reconstruye (lote 105A).

// 0x403D60: `vector constructor iterator` (stdcall, `ret 10h`): llama `count` veces a `ctor` (thiscall) con ECX = base, base + size, base + 2 * size...; con count <= 0 no hace nada.
void __stdcall VectorConstructorIteratorExe(void* base, uint32 size, int32 count, void(__thiscall* ctor)(void*));

void RuntimeHelpersInjectHooksLote105A();
