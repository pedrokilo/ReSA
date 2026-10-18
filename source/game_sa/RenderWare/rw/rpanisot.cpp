/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpAnisot
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpanisot.h"

// Desplazamiento del dato de anisotropia dentro de RwTexture (lo registra RpAnisotPluginAttach, 0x748FAE).
static auto& s_anisotTextureOffset = StaticRef<RwInt32>(0xC92478);

void RpAnisotInjectHooks() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpAnisot");

    RH_ScopedGlobalInstall(RpAnisotGetMaxSupportedMaxAnisotropy, 0x748F20);
    RH_ScopedGlobalInstall(RpAnisotTextureSetMaxAnisotropy, 0x748F30);
    RH_ScopedGlobalInstall(RpAnisotTextureGetMaxAnisotropy, 0x748F50);
}

// 0x748F20
// Byte bajo de D3DCAPS9::MaxAnisotropy (DWORD en +0x6C) del D3DCAPS9 de RenderWare (0x7FAD20 = RwD3D9GetCaps).
RwInt8 RpAnisotGetMaxSupportedMaxAnisotropy() {
    static_assert(offsetof(D3DCAPS9, MaxAnisotropy) == 0x6C);
    return *(static_cast<const RwInt8*>(RwD3D9GetCaps()) + offsetof(D3DCAPS9, MaxAnisotropy));
}

// 0x748F30
RwTexture* RpAnisotTextureSetMaxAnisotropy(RwTexture* tex, RwInt8 val) {
    if (!tex) {
        return nullptr;
    }
    *reinterpret_cast<RwInt8*>(reinterpret_cast<uintptr_t>(tex) + static_cast<uintptr_t>(s_anisotTextureOffset)) = val;
    return tex;
}

// 0x748F50
RwInt8 RpAnisotTextureGetMaxAnisotropy(RwTexture* tex) {
    if (!tex) {
        return 0;
    }
    return *reinterpret_cast<const RwInt8*>(reinterpret_cast<uintptr_t>(tex) + static_cast<uintptr_t>(s_anisotTextureOffset));
}

RwBool RpAnisotPluginAttach() {
    return ((RwBool(__cdecl *)())0x748F70)();
}
