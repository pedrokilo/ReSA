/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3dxShaderAssembler
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "D3dxShaderAssembler.h"
#include "Crt/CrtHeap.h"

// Ensamblador de shaders de D3DX9 del EXE (lote NEWDEL). Ver D3dxShaderAssembler.h y reversing/FnNEWDEL/README.md. Cada cuerpo sigue el desensamblado del EXE;
// las direcciones del comentario son del EXE. Colaboradores sin reconstruir (por direccion):
//   0x7716DD (ctor del objeto f78), 0x77194F (su destructor), 0x76BA24 / 0x76BA49 / 0x76B9E0 / 0x76E336 (lector de la fuente), 0x77196B, 0x770835, 0x770767, 0x77107F,
//   0x77129F, 0x7715F1 (fases del ensamblado), 0x7725E3 (comprobacion tras el validador), 0x766088 (version del shader), 0x76EB84 / 0x76EC4D (mensajes de error y de aviso)
//   y 0x76FBC5 (callback del validador de d3d9.dll). El estado global 0xC96438 guarda el ensamblador en curso.

namespace {
constexpr int32  HR_INVALID_CALL = static_cast<int32>(0x8876086C);
constexpr int32  HR_NO_MEMORY        = static_cast<int32>(0x8007000E);
constexpr int32  HR_ASM_FAILED      = static_cast<int32>(0x88760B59);
constexpr uint32 UNSUPPORTED_FLAGS  = 0xFFFFFFFAu; // el EXE solo admite las banderas 1 y 4

inline const char* ExeString(uintptr_t address) { return reinterpret_cast<const char*>(address); }

// Mensajes del EXE (.rdata): "shader version expected", "vs_1_0 is no longer supported; using vs_1_1", "ps_1_0 ...", "unrecognized shader version", "only vs_1_1, vs_2_0, ..."
constexpr uintptr_t STR_VERSION_EXPECTED = 0x87A7D4;
constexpr uintptr_t STR_VS10_OBSOLETE    = 0x87A7A8;
constexpr uintptr_t STR_PS10_OBSOLETE    = 0x87A77C;
constexpr uintptr_t STR_UNRECOGNIZED     = 0x87A760;
constexpr uintptr_t STR_ONLY_SUPPORTED   = 0x87A710;
constexpr uintptr_t STR_VALIDATOR_DLL    = 0x87A704; // "d3d9.dll"
constexpr uintptr_t STR_VALIDATOR_PROC   = 0x87A6E4; // "Direct3DShaderValidatorCreate9"
constexpr uintptr_t VALIDATOR_CALLBACK   = 0x76FBC5;
constexpr uintptr_t G_CURRENT_ASSEMBLER  = 0xC96438;

// 0x76EB84 (error) y 0x76EC4D (aviso): cdecl, 4 argumentos
using ReportFn = void(__cdecl*)(void* errorLog, uint32* errorCode, uint32 code, const char* text);

inline void* This0(uintptr_t fn, void* self)                  { return reinterpret_cast<void*(__thiscall*)(void*)>(fn)(self); }
inline int32 ThisHr0(uintptr_t fn, void* self)                { return reinterpret_cast<int32(__thiscall*)(void*)>(fn)(self); }
inline int32 ThisHr1(uintptr_t fn, void* self, uintptr_t arg) { return reinterpret_cast<int32(__thiscall*)(void*, uintptr_t)>(fn)(self, arg); }
inline void  DeleteObject78(void* object)                     { reinterpret_cast<void*(__thiscall*)(void*, uint32)>(0x77194F)(object, 1); }

// Destructor virtual "con borrado" (ranura 0 de la vtable, __thiscall con bandera 1)
inline void DeleteVirtual(void* object) {
    void** const vtbl = *static_cast<void***>(object);
    reinterpret_cast<void*(__thiscall*)(void*, uint32)>(vtbl[0])(object, 1);
}
// IUnknown::Release (ranura 2, __stdcall)
inline void ReleaseCom(void* object) {
    void** const vtbl = *static_cast<void***>(object);
    reinterpret_cast<uint32(__stdcall*)(void*)>(vtbl[2])(object);
}

// Indice de version (+0x38) de las versiones reconocidas; false si no existe (0x772E28: "unrecognized shader version")
bool VersionToMode(uint32 version, int32& mode) {
    switch (version) {
    case 0xFFFE0101: mode = 0;   return true;
    case 0xFFFE0200: mode = 1;   return true;
    case 0xFFFE0201: mode = 2;   return true;
    case 0xFFFE02FF: mode = 3;   return true;
    case 0xFFFE0300: mode = 4;   return true;
    case 0xFFFE03FF: mode = 5;   return true;
    case 0xFFFF0101: mode = 6;   return true;
    case 0xFFFF0102: mode = 7;   return true;
    case 0xFFFF0103: mode = 8;   return true;
    case 0xFFFF0104: mode = 9;   return true;
    case 0xFFFF0200: mode = 0xA; return true;
    case 0xFFFF0201: mode = 0xB; return true;
    case 0xFFFF02FF: mode = 0xC; return true;
    case 0xFFFF0300: mode = 0xD; return true;
    case 0xFFFF03FF: mode = 0xE; return true;
    default:         return false;
    }
}
} // namespace

// 0x76FAEB
D3dxShaderAssembler* D3dxShaderAssembler::Constructor() {
    errorLog  = nullptr;
    source    = nullptr;
    validator = nullptr;
    f34       = nullptr;
    f58       = nullptr;
    f5C       = 0;
    f60       = 0;
    f78       = nullptr;
    return this;
}

// 0x771F63
void D3dxShaderAssembler::Destructor() {
    if (validator) {
        ReleaseCom(validator);
        validator = nullptr;
    }
    if (f34) {
        DeleteVirtual(f34);
    }
    ExeCrt::OperatorDelete(f58);
    if (f78) {
        DeleteObject78(f78);
    }
}

// 0x772BB1
int32 D3dxShaderAssembler::Assemble(D3dxShaderSource* src, uint32 flagsIn, uint32 unkC, void** ppShader) {
    if (flagsIn & UNSUPPORTED_FLAGS) {
        return HR_INVALID_CALL;
    }
    if (!ppShader) {
        return HR_INVALID_CALL;
    }
    *ppShader = nullptr;

    // Libera lo que dejo una pasada anterior de este mismo objeto
    if (f34) {
        DeleteVirtual(f34);
    }
    ExeCrt::OperatorDelete(f58);
    if (f78) {
        DeleteObject78(f78);
    }

    StaticRef<D3dxShaderAssembler*>(G_CURRENT_ASSEMBLER) = this;
    mode      = -1;
    source    = src;
    errorLog  = &src->errors;
    validator = nullptr;
    f30       = 0;
    f34       = nullptr;
    f58       = nullptr;
    f5C       = 0;
    f60       = 0;
    f64       = 0;
    f68       = 0;
    failed    = 0;
    f50       = 0;
    f54       = 0;
    flags     = flagsIn;
    offset    = 0;
    length    = 0;
    f74       = 0;
    // Como el EXE, f78 solo se escribe en esta rama (el constructor lo deja en 0): con unkC == 0 conserva el valor de antes
    if (unkC != 0) {
        flags = flagsIn | 4;
        f7C   = unkC;
        void* const obj = ExeCrt::OperatorNew(0x64);
        f78 = obj ? This0(0x7716DD, obj) : nullptr;
        if (!f78) {
            return HR_NO_MEMORY;
        }
    }

    const auto report = [&](uintptr_t fn, uint32 code, uintptr_t text) {
        reinterpret_cast<ReportFn>(fn)(errorLog, &errorCode, code, ExeString(text));
    };
    // 0x772FF6 + 0x773001 + 0x773019: cola comun (marca el fallo, libera el validador, cierra el lector de la fuente y devuelve el HRESULT)
    const auto finish = [&](int32 hr, bool markFailed) -> int32 {
        if (markFailed) {
            failed = 1;
        }
        if (validator) {
            ReleaseCom(validator);
            validator = nullptr;
        }
        This0(0x76E336, src);
        return hr;
    };

    // 0x772C84
    uint32 scratch = 0;
    reinterpret_cast<void(__thiscall*)(void*, uint32*, uint32)>(0x76BA24)(src, &scratch, 0);
    if (scratch == 0) {
        reinterpret_cast<void(__thiscall*)(void*, uint32*, uint32*)>(0x76BA49)(src, &offset, &length);
    }
    if (const int32 hr = reinterpret_cast<int32(__thiscall*)(void*, uint32)>(0x76B9E0)(src, 2); hr < 0) {
        return hr; // 0x772CB2: sale sin la cola comun
    }
    This0(0x77196B, this);

    if (errorCode == 9) { // 0x772CBF
        uint32 info[3];
        if (reinterpret_cast<int32(__stdcall*)(uint32, uint32, uint32*)>(0x766088)(version, 1, info) >= 0) {
            errorCode = 0;
            version   = info[1];
        }
    }
    if (errorCode != 0) { // 0x772CE0
        report(0x76EB84, 0x7D1, STR_VERSION_EXPECTED);
        return finish(HR_ASM_FAILED, true);
    }
    if (version == 0xFFFE0100) { // 0x772D07: vs_1_0 -> vs_1_1 con aviso
        report(0x76EC4D, 0x7DF, STR_VS10_OBSOLETE);
        version = 0xFFFE0101;
    }
    if (version == 0xFFFF0100) { // 0x772D30: ps_1_0 -> ps_1_1 con aviso
        report(0x76EC4D, 0x7DF, STR_PS10_OBSOLETE);
        version = 0xFFFF0101;
    }
    if (!VersionToMode(version, mode)) { // 0x772E28 (mismo mensaje y salida que 0x772CEF)
        report(0x76EB84, 0x7D2, STR_UNRECOGNIZED);
        return finish(HR_ASM_FAILED, true);
    }
    if (f78) { // 0x772E74: con f78 el modo debe estar entre 0 y 3; si no, solo se anota el error y se sigue
        if (mode < 0 || mode > 3) {
            report(0x76EB84, 0x7D1, STR_ONLY_SUPPORTED);
        }
        const uint32 high = static_cast<uint32>(reinterpret_cast<const uint8*>(this)[0x19]) | 0x7FFE00u;
        version = (high << 8) | (version & 0xFFu);
    }

    int32 hr = 0;
    if (!(flags & 4)) { // 0x772EB6: validador de d3d9.dll (el juego pasa la bandera 4 y no entra)
        const char* const dllName = ExeString(STR_VALIDATOR_DLL);
        HMODULE module = GetModuleHandleA(dllName);
        if (!module) {
            module = LoadLibraryA(dllName);
        }
        if (module) {
            if (const auto create = reinterpret_cast<void*(__stdcall*)()>(GetProcAddress(module, ExeString(STR_VALIDATOR_PROC)))) {
                validator = create();
                if (validator) { // ranura 3: Instruction(callback, contexto, 0)
                    void** const vtbl = *static_cast<void***>(validator);
                    hr = reinterpret_cast<int32(__stdcall*)(void*, uintptr_t, void*, uint32)>(vtbl[3])(validator, VALIDATOR_CALLBACK, this, 0);
                    if (hr < 0) {
                        return finish(hr, true);
                    }
                }
            }
        }
    }
    if (hr = ThisHr1(0x770835, this, version); hr < 0) { // 0x772F07
        return finish(hr, true);
    }
    if (hr = ThisHr1(0x770767, this, reinterpret_cast<uintptr_t>(&errorCode)); hr < 0) {
        return finish(hr, true);
    }
    if (failed != 0) { // 0x772F30
        return finish(HR_ASM_FAILED, true);
    }
    if (reinterpret_cast<int32(__cdecl*)()>(0x7725E3)() != 0) { // 0x772F39
        failed = 1;
    }
    if (failed != 0) {
        return finish(HR_ASM_FAILED, true);
    }
    if (offset != 0) { // 0x772F52: recorta la longitud al ancho real del texto
        uint32 textWidth = 0;
        reinterpret_cast<void(__thiscall*)(void*, uint32*, uint32)>(0x76BA49)(src, &textWidth, 0);
        if (offset < textWidth && length + offset > textWidth) {
            length = textWidth - offset;
        }
    }
    if (f78) { // 0x772F7C
        if (hr = ThisHr0(0x77107F, this); hr < 0) {
            return finish(hr, true);
        }
    }
    if (flags & 1) { // 0x772F8E
        if (hr = ThisHr0(0x77129F, this); hr < 0) {
            return finish(hr, true);
        }
    }
    if (hr = ThisHr1(0x770835, this, 0xFFFF); hr < 0) { // 0x772FA1
        return finish(hr, true);
    }
    if (hr = ThisHr1(0x770767, this, reinterpret_cast<uintptr_t>(&errorCode)); hr < 0) {
        return finish(hr, true);
    }
    if (validator) { // 0x772FC4: End (ranura 5)
        void** const vtbl = *static_cast<void***>(validator);
        if (hr = reinterpret_cast<int32(__stdcall*)(void*)>(vtbl[5])(validator); hr < 0) {
            return finish(hr, true);
        }
    }
    if (errorLog->GetErrorCount() != 0) { // 0x772FD7 (0x76E664)
        return finish(HR_ASM_FAILED, true);
    }
    hr = ThisHr1(0x7715F1, this, reinterpret_cast<uintptr_t>(ppShader)); // 0x772FE6
    if (hr < 0) {
        return finish(hr, true);
    }
    return finish(0, false);
}

void D3dxShaderAssembler::InjectHooks() {
    RH_ScopedClass(D3dxShaderAssembler);
    RH_ScopedCategory("Plugins/D3DX9");

    RH_ScopedInstall(Constructor, 0x76FAEB);
    RH_ScopedInstall(Destructor, 0x771F63);
    RH_ScopedInstall(Assemble, 0x772BB1);
}
