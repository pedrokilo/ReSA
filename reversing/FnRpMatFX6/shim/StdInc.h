// Sustituto minimo de StdInc.h para compilar QTMLRuntime.cpp (el archivo real del proyecto, sin cambios) dentro del banco fuera del juego.
#pragma once
#include <windows.h>
#include <stdint.h>
#include <cstring>
typedef int16_t int16; typedef int32_t int32; typedef int32_t RwInt32;
template <typename T> inline T& StaticRef(uintptr_t addr) { return *reinterpret_cast<T*>(addr); }
#define RH_ScopedCategory(x)
#define RH_ScopedNamespaceName(x)
#define RH_ScopedGlobalInstall(f, a)
