// Solo para compilar Release sin tocar el codigo fuente: en Release (NDEBUG) <assert.h> no declara
// _wassert, pero app_debug.cpp toma su direccion. Se declara aqui (el simbolo existe en el CRT estatico).
#pragma once
#ifdef __cplusplus
extern "C" void __cdecl _wassert(const wchar_t*, const wchar_t*, unsigned);
#endif
