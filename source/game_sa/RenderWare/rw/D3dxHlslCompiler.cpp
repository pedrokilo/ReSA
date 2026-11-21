/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: D3DX9
 * Reconstrucción: COMPLETA — 18/18 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/18 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: D3dxHlslCompiler
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "D3dxHlslCompiler.h"
#include "Crt/CrtHeap.h"

// Compilador HLSL de D3DX9 del EXE (lote NEWDEL): constructor, destructor y entrada de D3DXCompileShader*. Ver D3dxHlslCompiler.h y reversing/FnNEWDEL/README.md.

namespace {
constexpr int32 HR_INVALID_CALL = static_cast<int32>(0x8876086C);

inline void DeleteVirtual(void* object) {
    void** const vtbl = *static_cast<void***>(object);
    reinterpret_cast<void*(__thiscall*)(void*, uint32)>(vtbl[0])(object, 1);
}
} // namespace

// 0x773022
D3dxHlslCompiler* D3dxHlslCompiler::Constructor() {
    f00     = 0;
    obj04   = nullptr;
    obj08   = nullptr;
    block18 = nullptr;
    f34     = 0;
    f38     = 0;
    f50     = 0;
    return this;
}

// 0x77303B
void D3dxHlslCompiler::Destructor() {
    if (obj04) {
        DeleteVirtual(obj04);
    }
    if (obj08) {
        DeleteVirtual(obj08);
    }
    ExeCrt::OperatorDelete(block18);
}

// 0x77EE77
int32 __stdcall D3dxCompileShaderEntry(uintptr_t a0, uintptr_t a1, uintptr_t a2, uintptr_t a3, uintptr_t a4, uintptr_t a5, uint32* a6, uint32* a7) {
    if (a6) {
        *a6 = 0;
    }
    if (a7) {
        *a7 = 0;
    }
    if (a2 == 0) {
        return HR_INVALID_CALL;
    }
    using CompileFn = int32(__stdcall*)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uint32*, uint32*);
    return reinterpret_cast<CompileFn>(0x77EB3C)(a0, a1, a2, 0, 0, 0, a3, a4, a5, a6, a7);
}

void D3dxHlslCompiler::InjectHooks() {
    RH_ScopedClass(D3dxHlslCompiler);
    RH_ScopedCategory("Plugins/D3DX9");

    RH_ScopedInstall(Constructor, 0x773022);
    RH_ScopedInstall(Destructor, 0x77303B);

    {
        RH_ScopedNamespaceName("D3DX9");
        RH_ScopedGlobalInstall(D3dxCompileShaderEntry, 0x77EE77);
    }
}
