#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <windows.h>
#include <cstdint>
#include "CrtException.h"
#include "CrtString.h"
#include "CrtFile.h"

// Reconstruccion de std::exception y de los lanzadores de std::string del CRT del EXE (lote CRT3A). Ver CrtException.h.

namespace notsa {
namespace {
constexpr uint32_t kStdExceptionVtbl = 0x887E80;
inline void* ExeMalloc(unsigned int size) { return reinterpret_cast<void*(__cdecl*)(unsigned int)>(0x824257)(size); } // malloc
inline void  ExeFree(void* p) { reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); }                                // free
} // namespace

// 0x822D97: and [this+4], 0; and [this+8], 0; vtable; devuelve this
StdException* StdException::Construct() {
    m_what   = nullptr;
    m_doFree = 0;
    m_vtbl   = reinterpret_cast<const void*>(kStdExceptionVtbl);
    return this;
}

// 0x822DE5: vtable; copia la bandera; con bandera: malloc(strlen + 1) + strcpy (si malloc falla, +4 queda en nulo); sin bandera: comparte el puntero
StdException* StdException::CopyConstruct(const StdException& o) {
    m_vtbl   = reinterpret_cast<const void*>(kStdExceptionVtbl);
    m_doFree = o.m_doFree;
    if (o.m_doFree != 0) {
        const unsigned int size = ExeCrt::Strlen(o.m_what) + 1;
        char* const copy = static_cast<char*>(ExeMalloc(size));
        m_what = copy;
        if (copy != nullptr) {
            ExeCrt::Strcpy(copy, o.m_what);
        }
    } else {
        m_what = o.m_what;
    }
    return this;
}

// 0x822E2F: vtable; si la bandera esta puesta libera el texto (no borra los campos)
void StdException::Destruct() {
    const int32_t doFree = m_doFree;
    m_vtbl = reinterpret_cast<const void*>(kStdExceptionVtbl);
    if (doFree != 0) {
        ExeFree(const_cast<char*>(m_what));
    }
}
} // namespace notsa

void ExeStdStringFromChars(void* string, const char* text);                    // Audio/Managers/AEUserRadioTrackManager.cpp
void ExeStdRuntimeErrorFromString(void* error, const void* string);           // idem
void ExeStdStringTidy(void* string);                                           // idem

namespace ExeCrt {
namespace {
// std::string de MSVC 7.1 (0x1C bytes) y std::runtime_error (0x28: std::exception + std::string en +0xC): opacos, los construyen las funciones del EXE (0x4F4170, 0x4F4090)
struct ExeString { uint8_t raw[0x1C]; };
struct ExeRuntimeError {
    uint32_t  vtable;
    uint32_t  what;
    uint32_t  doFree;
    ExeString message;
};
static_assert(sizeof(ExeRuntimeError) == 0x28);

// thiscall con un argumento de pila y `ret 4`: se invoca como __fastcall con un EDX de relleno (misma disposicion de registros y de pila)
// Las tres funciones viven en C++ en Audio/Managers/AEUserRadioTrackManager.cpp (cuerpos de 0x4F4170, 0x4F4090 y 0x4F3BA0).
inline void StringFromChars(ExeString* s, const char* text) { ::ExeStdStringFromChars(s, text); }
inline void RuntimeErrorFromString(ExeRuntimeError* e, const ExeString* s) { ::ExeStdRuntimeErrorFromString(e, s); }
inline void StringTidy(ExeString* s) { ::ExeStdStringTidy(s); }

// Unico estado de la tabla de desenrollado del EXE (0x8A1E64 / 0x8A1E88): destruir el std::string temporal con _Tidy si la excepcion atraviesa este marco
struct TempStringGuard {
    ExeString* s;
    ~TempStringGuard() { StringTidy(s); }
};

[[noreturn]] void ThrowRuntimeError(uint32_t textAddr, uint32_t vtable, uint32_t throwInfo) {
    ExeString tmp;
    StringFromChars(&tmp, reinterpret_cast<const char*>(textAddr));
    TempStringGuard guard{&tmp};
    ExeRuntimeError exc;
    RuntimeErrorFromString(&exc, &tmp);
    exc.vtable = vtable;
    CxxThrowException(&exc, reinterpret_cast<const void*>(throwInfo));
    __assume(0); // el EXE tiene un int3 aqui
}
} // namespace

// 0x8211FD
void __cdecl StringXran() { ThrowRuntimeError(0x887D7C, 0x887D74, 0x8A1E54); }

// 0x821255
void __cdecl StringXlen() { ThrowRuntimeError(0x887D94, 0x887D68, 0x893634); }
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtExceptionInjectHooks() {
    {
        using notsa::StdException;
        RH_ScopedClass(StdException);
        RH_ScopedCategory("Crt");

        RH_ScopedInstall(Construct, 0x822D97);
        RH_ScopedInstall(CopyConstruct, 0x822DE5);
        RH_ScopedInstall(Destruct, 0x822E2F);
    }
    {
        RH_ScopedNamespaceName("ExeCrt");
        RH_ScopedCategory("Crt");

        RH_ScopedGlobalInstall(ExeCrt::StringXran, 0x8211FD);
        RH_ScopedGlobalInstall(ExeCrt::StringXlen, 0x821255);
    }
}
#endif
