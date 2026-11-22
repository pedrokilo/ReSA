/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpAnisot
 * Reconstrucción: COMPLETA — 12/12 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 12/12 entradas.
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
static auto& s_anisotEngineCount   = StaticRef<RwInt32>(0xC92474); // objetos del plugin de motor 0x127 (contador que suben y bajan 0x749000 y 0x749010)

// Callbacks del plugin 0x127 (lote S7A; todos cdecl). Se registran por su direccion propia en RpAnisotPluginAttach y llevan hook en la direccion del EXE.
static void*     RpAnisotEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                         // 0x749000
static void*     RpAnisotEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                          // 0x749010
static void*     RpAnisotTextureConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                        // 0x749020
static void*     RpAnisotTextureDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                         // 0x749030
static void*     RpAnisotTextureCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);     // 0x749040
static RwStream* RpAnisotStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);        // 0x749060
static RwStream* RpAnisotStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x7490B0
static RwInt32   RpAnisotStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                       // 0x7490F0

void RpAnisotInjectHooks() {
    RH_ScopedCategory("Plugins");
    RH_ScopedNamespaceName("RpAnisot");

    RH_ScopedGlobalInstall(RpAnisotGetMaxSupportedMaxAnisotropy, 0x748F20);
    RH_ScopedGlobalInstall(RpAnisotTextureSetMaxAnisotropy, 0x748F30);
    RH_ScopedGlobalInstall(RpAnisotTextureGetMaxAnisotropy, 0x748F50);
    RH_ScopedGlobalInstall(RpAnisotPluginAttach, 0x748F70); // lote 226A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotEngineConstructor, 0x749000);   // lote S7A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotEngineDestructor, 0x749010);    // lote S7A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotTextureConstructor, 0x749020);  // lote S7A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotTextureDestructor, 0x749030);   // lote S7A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotTextureCopy, 0x749040);         // lote S7A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotStreamRead, 0x749060);          // lote S7A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotStreamWrite, 0x7490B0);         // lote S7A (sin validar)
    RH_ScopedGlobalInstall(RpAnisotStreamGetSize, 0x7490F0);       // lote S7A (sin validar)
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
// seguir (los desplazamientos ya escritos se conservan). Los callbacks (0x749000..0x7490F0) son ahora funciones propias de este archivo con hook en su direccion del EXE (lote S7A).
RwBool RpAnisotPluginAttach() {
    if (RwEngineRegisterPlugin(0, 0x127, RpAnisotEngineConstructor, RpAnisotEngineDestructor) < 0) {
        return FALSE;
    }
    s_anisotTextureOffset = RwTextureRegisterPlugin(1, 0x127, RpAnisotTextureConstructor, RpAnisotTextureDestructor, RpAnisotTextureCopy);
    if (s_anisotTextureOffset < 0) {
        return FALSE;
    }
    s_anisotStreamResult = RwTextureRegisterPluginStream(0x127, RpAnisotStreamRead, RpAnisotStreamWrite, RpAnisotStreamGetSize);
    if (s_anisotStreamResult < 0) {
        return FALSE;
    }
    _rwD3D9SetAnisotropyTextureOffset(s_anisotTextureOffset); // 0x7FDE60
    return TRUE;
}

// Byte del dato de anisotropia de una textura (o del objeto dado): objeto + desplazamiento registrado (0xC92478, releido en cada uso, suma de 32 bits como el EXE).
static uint8* AnisotByte(void* object) {
    return reinterpret_cast<uint8*>(static_cast<uintptr_t>(static_cast<uint32>(reinterpret_cast<uintptr_t>(object)) + static_cast<uint32>(s_anisotTextureOffset)));
}

// 0x749000 (constructor del plugin de motor 0x127): sube el contador de 0xC92474 y devuelve el objeto.
static void* RpAnisotEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    s_anisotEngineCount = s_anisotEngineCount + 1;
    return object;
}

// 0x749010 (destructor del plugin de motor): baja el contador de 0xC92474 y devuelve el objeto.
static void* RpAnisotEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    s_anisotEngineCount = s_anisotEngineCount - 1;
    return object;
}

// 0x749020 (constructor del plugin de textura): pone a 1 el byte de anisotropia en objeto + [0xC92478] (ignora el desplazamiento recibido) y devuelve el objeto.
static void* RpAnisotTextureConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    *AnisotByte(object) = 1;
    return object;
}

// 0x749030 (destructor del plugin de textura): no hace nada, devuelve el objeto.
static void* RpAnisotTextureDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    return object;
}

// 0x749040 (copia del plugin de textura): copia el byte de anisotropia de origen a destino (desplazamiento [0xC92478] releido para cada acceso) y devuelve el destino.
static void* RpAnisotTextureCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    const uint8 value = *AnisotByte(const_cast<void*>(srcObject));
    *AnisotByte(dstObject) = value;
    return dstObject;
}

// 0x749060 (lectura del chunk de anisotropia): solo con binaryLength == 4 y objeto no nulo; lee 4 bytes (RwStreamReadInt32) y guarda el byte bajo en objeto + [0xC92478] (relee el
// desplazamiento tras la lectura). Devuelve el flujo o nulo. Como el EXE, el entero destino es el propio argumento binaryLength (valor 4 si la lectura "tiene exito" sin escribir nada).
static RwStream* RpAnisotStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    if (binaryLength != 4 || !object) {
        return nullptr;
    }
    RwInt32 value = binaryLength;
    if (!RwStreamReadInt32(stream, &value, 4)) {
        return nullptr;
    }
    *AnisotByte(object) = static_cast<uint8>(value);
    return stream;
}

// 0x7490B0 (escritura del chunk): con objeto nulo o con su primera palabra a cero devuelve nulo; si no escribe el byte de anisotropia (desplazamiento y byte leidos antes de la llamada) como
// entero de 4 bytes y devuelve el flujo si RwStreamWriteInt32 devuelve algo distinto de nulo.
static RwStream* RpAnisotStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(binaryLength); UNUSED(offsetInObject); UNUSED(sizeInObject);
    if (!object || *static_cast<const RwInt32*>(object) == 0) {
        return nullptr;
    }
    RwInt32 value = *AnisotByte(const_cast<void*>(object));
    return RwStreamWriteInt32(stream, &value, 4) ? stream : nullptr;
}

// 0x7490F0 (tamano del chunk): 4 si el objeto no es nulo, su primera palabra no es cero y el byte de anisotropia no vale 1; si no 0.
static RwInt32 RpAnisotStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    UNUSED(offsetInObject); UNUSED(sizeInObject);
    if (!object || *static_cast<const RwInt32*>(object) == 0) {
        return 0;
    }
    return *AnisotByte(const_cast<void*>(object)) != 1 ? 4 : 0;
}
