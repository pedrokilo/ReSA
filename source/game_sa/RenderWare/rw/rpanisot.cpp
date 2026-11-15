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
static auto& s_anisotStreamResult  = StaticRef<RwInt32>(0xC9247C); // resultado de RwTextureRegisterPluginStream (0x748FD6)

void RpAnisotInjectHooks() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpAnisot");

    RH_ScopedGlobalInstall(RpAnisotGetMaxSupportedMaxAnisotropy, 0x748F20);
    RH_ScopedGlobalInstall(RpAnisotTextureSetMaxAnisotropy, 0x748F30);
    RH_ScopedGlobalInstall(RpAnisotTextureGetMaxAnisotropy, 0x748F50);
    RH_ScopedGlobalInstall(RpAnisotPluginAttach, 0x748F70); // lote 226A (sin validar)
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

// 0x748F70
// Lote 226A (sin validar). Registra el plugin 0x127: plugin de motor sin datos (RwEngineRegisterPlugin(0, 0x127, 0x749000, 0x749010)); plugin de textura de 1 byte con constructor,
// destructor y copia del EXE (0x749020, 0x749030, 0x749040) cuyo desplazamiento queda en 0xC92478; plugin de flujo de textura (lectura 0x749060, escritura 0x7490B0, tamano 0x7490F0)
// cuyo resultado queda en 0xC9247C; y llama a 0x7FDE60 (cdecl, un argumento) con el desplazamiento de la textura (releido de 0xC92478). Cualquier resultado negativo devuelve 0 sin
// seguir (los desplazamientos ya escritos se conservan). Las funciones de retorno se llaman por su direccion: no se enganchan aqui.
RwBool RpAnisotPluginAttach() {
    if (RwEngineRegisterPlugin(0, 0x127, reinterpret_cast<RwPluginObjectConstructor>(0x749000), reinterpret_cast<RwPluginObjectDestructor>(0x749010)) < 0) {
        return FALSE;
    }
    s_anisotTextureOffset = RwTextureRegisterPlugin(1, 0x127, reinterpret_cast<RwPluginObjectConstructor>(0x749020), reinterpret_cast<RwPluginObjectDestructor>(0x749030), reinterpret_cast<RwPluginObjectCopy>(0x749040));
    if (s_anisotTextureOffset < 0) {
        return FALSE;
    }
    s_anisotStreamResult = RwTextureRegisterPluginStream(0x127, reinterpret_cast<RwPluginDataChunkReadCallBack>(0x749060), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x7490B0), reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(0x7490F0));
    if (s_anisotStreamResult < 0) {
        return FALSE;
    }
    reinterpret_cast<void(__cdecl*)(RwInt32)>(0x7FDE60)(s_anisotTextureOffset);
    return TRUE;
}
