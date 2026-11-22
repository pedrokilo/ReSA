/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3dxShaderSource
 * Reconstrucción: COMPLETA — 5/5 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/5 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "D3dxShaderSource.h"
#include "Crt/CrtHeap.h"
#include "Crt/CrtMem.h"

// Objeto fuente del ensamblador de shaders de D3DX9 del EXE (lote NEWDEL). Ver D3dxShaderSource.h y reversing/FnNEWDEL/README.md.
// Colaboradores sin reconstruir (por direccion): 0x76E390/0x76E3FD/0x76E41D/0x76E396 (cadena de bloques y registro de errores), 0x76B87F/0x76B4FD/0x76B5EF
// (destructores de f48/f4C/lector), 0x76B567 (constructor del lector), 0x76B818/0x76B60B/0x76B7B2 (carga del lector), 0x76D37E (preparacion), y del CRT del EXE:
// setlocale 0x825A79, _strdup 0x82514C, _controlfp 0x825136.

namespace {
constexpr uint32 HR_NO_MEMORY_CODE = 0x8007000Eu;
constexpr int    LC_NUMERIC_ID    = 4;

using SetLocaleFn  = char*(__cdecl*)(int category, const char* locale);
using ControlFpFn  = uint32(__cdecl*)(uint32 value, uint32 mask);
inline char*  ExeSetLocale(int category, const char* locale) { return reinterpret_cast<SetLocaleFn>(0x825A79)(category, locale); }
inline char*  ExeStrdup(const char* text)                     { return ExeCrt::Strdup(text); } // 0x82514C (CrtMem)
inline uint32 ExeControlFp(uint32 value, uint32 mask)         { return reinterpret_cast<ControlFpFn>(0x825136)(value, mask); }

// `repe cmpsb` de 2 bytes contra "C\0" (0x877778): la configuracion regional guardada es exactamente "C"
inline bool IsCLocale(const char* locale) { return locale[0] == 'C' && locale[1] == '\0'; }

// Destructor virtual "con borrado" (ranura 0 de la vtable, __thiscall con bandera 1)
inline void DeleteVirtual(void* object) {
    void** const vtbl = *static_cast<void***>(object);
    reinterpret_cast<void*(__thiscall*)(void*, uint32)>(vtbl[0])(object, 1);
}
inline void CallThis(uintptr_t fn, void* self)               { reinterpret_cast<void(__thiscall*)(void*)>(fn)(self); }
inline void CallThis1(uintptr_t fn, void* self, uint32 arg)  { reinterpret_cast<void(__thiscall*)(void*, uint32)>(fn)(self, arg); }
inline void* CallThisRet(uintptr_t fn, void* self)           { return reinterpret_cast<void*(__thiscall*)(void*)>(fn)(self); }
} // namespace

// 0x76B89B
D3dxShaderSource* D3dxShaderSource::Constructor() {
    CallThis(0x76E390, this);        // pool = nullptr
    CallThis(0x76E3FD, &errors);     // registro de errores vacio
    f24 = 0;
    f28 = 1;
    f2C = 0;
    f30 = 0;
    f34 = 1;
    f38 = 1;
    f3C = 1;
    f40 = nullptr;
    f44 = nullptr;
    f48 = nullptr;
    f4C    = nullptr;
    reader = nullptr;
    readerCopy = nullptr;
    f80 = 1;
    // Guarda la configuracion regional numerica y la fuerza a "C" (la restaura el destructor); precision de coma flotante a 24 bits
    savedLocale = ExeStrdup(ExeSetLocale(LC_NUMERIC_ID, nullptr));
    if (!savedLocale || !IsCLocale(savedLocale)) {
        ExeSetLocale(LC_NUMERIC_ID, "C");
    }
    savedFpCtrl = ExeControlFp(0, 0);
    ExeControlFp(0x10000, 0x30000);
    return this;
}

// 0x76B940
void D3dxShaderSource::Destructor() {
    if (f40) {
        DeleteVirtual(f40);
    }
    if (f44) {
        DeleteVirtual(f44);
    }
    if (f48) {
        CallThis1(0x76B87F, f48, 1);
    }
    if (reader) {
        CallThis1(0x76B5EF, reader, 1);
    }
    if (f4C) {
        CallThis1(0x76B4FD, f4C, 1);
    }
    if (savedLocale && !IsCLocale(savedLocale)) {
        ExeSetLocale(LC_NUMERIC_ID, savedLocale);
    }
    if (savedLocale) {
        ExeCrt::Free(savedLocale);
    }
    ExeControlFp(savedFpCtrl, 0x30000);
    CallThis(0x76E41D, &errors);     // destructor del registro de errores
    CallThis(0x76E396, this);        // libera la cadena de bloques (salto final del EXE)
}

// 0x76DFD1
int32 D3dxShaderSource::InitFromMemory(uintptr_t data, uintptr_t size, uintptr_t defines, uintptr_t include) {
    void* const obj = ExeCrt::OperatorNew(0x70);
    reader = obj ? CallThisRet(0x76B567, obj) : nullptr;
    if (!reader) {
        return static_cast<int32>(HR_NO_MEMORY_CODE);
    }
    using LoadFn = int32(__thiscall*)(void*, uintptr_t, uintptr_t, void*, void*);
    if (const int32 hr = reinterpret_cast<LoadFn>(0x76B818)(reader, data, size, this, &errors); hr < 0) {
        return hr;
    }
    using PrepareFn = int32(__thiscall*)(void*, uintptr_t);
    if (const int32 hr = reinterpret_cast<PrepareFn>(0x76D37E)(this, defines); hr < 0) {
        return hr;
    }
    readerCopy = reader;
    loaderArg  = reinterpret_cast<void*>(include);
    return 0;
}

// 0x76DF03
int32 D3dxShaderSource::InitFromFile(uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3) {
    void* const obj = ExeCrt::OperatorNew(0x70);
    reader = obj ? CallThisRet(0x76B567, obj) : nullptr;
    if (!reader) {
        return static_cast<int32>(HR_NO_MEMORY_CODE);
    }
    using LoadFn = int32(__thiscall*)(void*, uintptr_t, uintptr_t, void*, void*, uintptr_t, uint32, uint32);
    if (const int32 hr = reinterpret_cast<LoadFn>(0x76B60B)(reader, a0, a1, this, &errors, a3, 0, 0); hr < 0) {
        return hr;
    }
    using PrepareFn = int32(__thiscall*)(void*, uintptr_t);
    if (const int32 hr = reinterpret_cast<PrepareFn>(0x76D37E)(this, a2); hr < 0) {
        return hr;
    }
    readerCopy = reader;
    loaderArg  = reinterpret_cast<void*>(a3);
    return 0;
}

// 0x76DF6C
int32 D3dxShaderSource::InitFromResource(uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4) {
    void* const obj = ExeCrt::OperatorNew(0x70);
    reader = obj ? CallThisRet(0x76B567, obj) : nullptr;
    if (!reader) {
        return static_cast<int32>(HR_NO_MEMORY_CODE);
    }
    using LoadFn = int32(__thiscall*)(void*, uintptr_t, uintptr_t, uintptr_t, void*, void*);
    if (const int32 hr = reinterpret_cast<LoadFn>(0x76B7B2)(reader, a0, a1, a2, this, &errors); hr < 0) {
        return hr;
    }
    using PrepareFn = int32(__thiscall*)(void*, uintptr_t);
    if (const int32 hr = reinterpret_cast<PrepareFn>(0x76D37E)(this, a3); hr < 0) {
        return hr;
    }
    readerCopy = reader;
    loaderArg  = reinterpret_cast<void*>(a4);
    return 0;
}

void D3dxShaderSource::InjectHooks() {
    RH_ScopedClass(D3dxShaderSource);
    RH_ScopedCategory("Plugins/D3DX9");

    RH_ScopedInstall(Constructor, 0x76B89B);
    RH_ScopedInstall(Destructor, 0x76B940);
    RH_ScopedInstall(InitFromMemory, 0x76DFD1);
    RH_ScopedInstall(InitFromFile, 0x76DF03);
    RH_ScopedInstall(InitFromResource, 0x76DF6C);
}
