/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: RpMatFX
 * Reconstrucción: COMPLETA — 28/28 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/28 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "rpmatfx.h"
#include "X87Math.h"

#include <emmintrin.h>

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R8A (sin validar): RpMatFX (plugin de efectos de material, id 0x120) y su canal D3D9. Cada cuerpo se transcribio del desensamblado (dumpbin /disasm)
// instruccion por instruccion; los nombres sin simbolo en el SDK son inferidos. Estado global del modulo en el EXE (direcciones):
//   0xC9AB74 desplazamiento de la extension de RpMaterial (MatFXMaterialDataOffset, en rpmatfx.h)   0xC9AB84 / 0xC9AB88 desplazamiento de RpAtomic / RpWorldSector (un RwBool)
//   0xC9AB80 lista libre de rpMatFXMaterialData (0x34 bytes, pista 0x30120)                          0xC9AB7C contador de aperturas del plugin de motor
//   0xC9AB8C / 0xC9AB90 pipelines de atomico / sector                                                 0xC9ABC4 / 0xC9ABC8 callbacks de instanciado originales del nodo AllInOne
//   0xC9ABAC dos o mas texturas simultaneas  0xC9ABB0 TextureOpCaps & MULTIPLYADD  0xC9ABB4 TextureOpCaps & MODULATE2X  0xC9ABBC MaxTextureBlendStages
//   0xC9ABC0 pixel shaders >= 1.1  0xC9AB94..0xC9ABA8 seis pixel shaders   0x8E244C ultimo pixel shader   0x8E2448 ultimo vertex shader   0x8E2440 ultimo FVF (0x252 = XYZ|NORMAL|DIFFUSE|TEX2)
// ---------------------------------------------------------------------------------------------------------------------------------------------
namespace {
constexpr RwUInt32 kMatFXPluginID        = 0x120;
constexpr RwUInt32 kMatFXMaterialMemHint = 0x30120;
auto& MatFXDataFreeList          = StaticRef<RwFreeList*>(0xC9AB80);
auto& MatFXAtomicDataOffset      = StaticRef<RwInt32>(0xC9AB84);
auto& MatFXWorldSectorDataOffset = StaticRef<RwInt32>(0xC9AB88);
auto& MatFXAtomicPipeline        = StaticRef<RxPipeline*>(0xC9AB8C);
auto& MatFXWorldSectorPipeline   = StaticRef<RxPipeline*>(0xC9AB90);
auto& MatFXAtomicInstanceCB      = StaticRef<RxD3D9AllInOneInstanceCallBack>(0xC9ABC4);
auto& MatFXSectorInstanceCB      = StaticRef<RxD3D9AllInOneInstanceCallBack>(0xC9ABC8);
auto& MatFXMultiTexture          = StaticRef<RwInt32>(0xC9ABAC);
auto& MatFXMultiplyAdd           = StaticRef<RwInt32>(0xC9ABB0);
auto& MatFXModulate2X            = StaticRef<RwInt32>(0xC9ABB4);
auto& MatFXUnknownB8             = StaticRef<RwInt32>(0xC9ABB8);
auto& MatFXMaxBlendStages        = StaticRef<RwInt32>(0xC9ABBC);
auto& MatFXPixelShadersOk        = StaticRef<RwInt32>(0xC9ABC0);
auto& MatFXUnknown29C0           = StaticRef<RwInt32>(0x8E29C0);
auto& MatFXUnknown29C4           = StaticRef<RwInt32>(0x8E29C4);
auto& D3D9LastVertexShader       = StaticRef<void*>(0x8E2448);
auto& D3D9LastPixelShader        = StaticRef<void*>(0x8E244C);

// Los desplazamientos y los punteros de datos se leen en cada llamada (el EXE no los guarda).
rpMatFXMaterialData*& MaterialData(const RpMaterial* material) {
    return *reinterpret_cast<rpMatFXMaterialData**>(reinterpret_cast<RwUInt8*>(const_cast<RpMaterial*>(material)) + MatFXMaterialDataOffset);
}

// Macro de busqueda del SDK inlineada en todas las funciones de acceso: primera ranura (de dos) cuya bandera es `type`; NULL si no hay (el EXE desreferencia ese NULL).
MatFXEffectData* FindEffect(const RpMaterial* material, RwUInt32 type) {
    rpMatFXMaterialData* const data = MaterialData(material);
    for (RwUInt8 i = 0; i < 2; ++i) {
        if (static_cast<RwUInt32>(data->data[i].flag) == type) {
            return &data->data[i];
        }
    }
    return nullptr;
}

// Palabras de 32 bits de un RwReal (copia sin pasar por la x87).
RwUInt32 RealBits(RwReal value) {
    RwUInt32 bits;
    memcpy(&bits, &value, sizeof(bits));
    return bits;
}
void StoreBits(RwReal& field, RwUInt32 bits) {
    memcpy(&field, &bits, sizeof(bits));
}
// `fld dword` + `fstp dword` de la x87: un NaN senalizante sale como NaN silencioso (bit 22 a 1); el resto de valores (denormales incluidos) sale con los mismos bits.
RwUInt32 X87Reload(RwUInt32 bits) {
    if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0) {
        bits |= 0x00400000u;
    }
    return bits;
}
// `fld dword` + `fchs` + `fstp dword`.
RwUInt32 X87ReloadNegated(RwUInt32 bits) {
    return X87Reload(bits) ^ 0x80000000u;
}

// Llamada COM stdcall por la tabla virtual (this primero).
template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
constexpr size_t kSlotDrawPrimitive        = 81;  // IDirect3DDevice9::DrawPrimitive (+0x144)
constexpr size_t kSlotDrawIndexedPrimitive = 82;  // IDirect3DDevice9::DrawIndexedPrimitive (+0x148)
constexpr size_t kSlotSetIndices           = 104; // IDirect3DDevice9::SetIndices (+0x1A0)
constexpr size_t kSlotSetVertexShader      = 92;  // IDirect3DDevice9::SetVertexShader (+0x170)
constexpr size_t kSlotSetPixelShader       = 107; // IDirect3DDevice9::SetPixelShader (+0x1AC)

void* Device() { return *reinterpret_cast<void**>(0xC97C28); } // _RwD3DDevice

constexpr uintptr_t kRasterIsCube          = 0x4C9F70; // _rwD3D9RasterIsCubeRaster(RwRaster*)
constexpr uintptr_t kD3D9FlushRenderStates = 0x7FC200; // vacia los estados de render pendientes antes de dibujar
constexpr uintptr_t kEnvMapHalfMatrix      = 0x885608; // RwMatrix constante (0.5, -0.5, 1, traslacion 0.5/0.5): de [-1,1] a [0,1]

// Lote R10A (sin validar): ayudantes de los callbacks de render de MatFX (0x812AC0, 0x812D40, 0x813A10, 0x814460).
constexpr size_t kSlotValidateDevice         = 70;  // IDirect3DDevice9::ValidateDevice (+0x118)
constexpr size_t kSlotSetVertexDeclaration   = 87;  // IDirect3DDevice9::SetVertexDeclaration (+0x15C)
constexpr size_t kSlotSetFVF                 = 89;  // IDirect3DDevice9::SetFVF (+0x164)
constexpr size_t kSlotSetPixelShaderConstantF = 109; // IDirect3DDevice9::SetPixelShaderConstantF (+0x1B4)

constexpr uintptr_t kTextureHasAlpha         = 0x4C9EA0; // _rwD3D9TextureHasAlpha(RwTexture*)
constexpr uintptr_t kConvertRasterToDisplay  = 0x4CD250; // D3D9RasterFormat::ConvertRasterToDisplayFormat(RwRaster*)
constexpr uintptr_t kVertexAlphaGet          = 0x7FE190; // estado actual del alfa por vertice del controlador D3D9 (devuelve un entero)
constexpr uintptr_t kVertexAlphaSet          = 0x7FE0A0; // activa o desactiva el alfa por vertice del controlador D3D9
constexpr uintptr_t kUvScratchMatrix         = 0x8E29C8; // matriz D3D de trabajo (16 dwords) de las matrices UV
constexpr uintptr_t kEnvPixelShaders         = 0xC9AB94; // seis pixel shaders consecutivos 0xC9AB94..0xC9ABA8 (ver _rpMatFXPipelinesCreate)
auto& D3D9LastFVF                = StaticRef<RwUInt32>(0x8E2440);  // ultimo FVF (0xFFFFFFFF = declaracion de vertices activa)
auto& D3D9LastVertexDeclaration  = StaticRef<uintptr_t>(0x8E2444); // ultima declaracion de vertices (0xFFFFFFFF = FVF activo)
auto& D3D9LastIndexBuffer        = StaticRef<void*>(0x8E2450);     // ultimo bufer de indices activo (SetIndices)
auto& MatFXFreeListBlockSize     = StaticRef<RwInt32>(0x8E29B8);   // parametros de RpMatFXMaterialDataSetFreeListCreateParams
auto& MatFXFreeListNumPrealloc   = StaticRef<RwInt32>(0x8E29BC);

void*& MatFXPixelShader(size_t index) { return reinterpret_cast<void**>(kEnvPixelShaders)[index]; }

RwInt32 TextureHasAlpha(RwTexture* texture) {
    return reinterpret_cast<RwInt32(__cdecl *)(RwTexture*)>(kTextureHasAlpha)(texture);
}
void ConvertRasterToDisplayFormatExe(RwRaster* raster) {
    reinterpret_cast<void(__cdecl *)(RwRaster*)>(kConvertRasterToDisplay)(raster);
}
RwInt32 VertexAlphaEnabled() {
    return reinterpret_cast<RwInt32(__cdecl *)()>(kVertexAlphaGet)();
}
void VertexAlphaEnable(RwBool enable) {
    reinterpret_cast<void(__cdecl *)(RwBool)>(kVertexAlphaSet)(enable);
}

// fistp con el redondeo vigente (al mas cercano) del float guardado: cvtss2si; fuera de rango o NaN dan 0x80000000 como la x87.
RwInt32 FloatToIntRounded(float value) {
    return _mm_cvtss_si32(_mm_set_ss(value));
}

void Tss(RwUInt32 stage, RwUInt32 type, RwUInt32 value) { RwD3D9SetTextureStageState(stage, type, value); }
void SetRs(RwUInt32 state, RwUInt32 value) { RwD3D9SetRenderState(state, value); }
RwUInt32 GetRs(RwUInt32 state) {
    RwUInt32 value = 0;
    RwD3D9GetRenderState(state, &value);
    return value;
}

// RwRenderStateGet / RwRenderStateSet del motor (tabla de funciones estandar de RwEngineInstance: +0x24 y +0x20).
RwUInt32 EngineRenderStateGet(RwUInt32 state) {
    RwUInt32 value = 0;
    (*reinterpret_cast<RwBool(__cdecl **)(RwUInt32, void*)>(StaticRef<uintptr_t>(0xC97B24) + 0x24))(state, &value);
    return value;
}
void EngineRenderStateSet(RwUInt32 state, RwUInt32 value) {
    (*reinterpret_cast<RwBool(__cdecl **)(RwUInt32, RwUInt32)>(StaticRef<uintptr_t>(0xC97B24) + 0x20))(state, value);
}

// Quita el pixel shader activo (SetPixelShader(NULL), +0x1AC) si lo hay.
void ResetPixelShader() {
    if (D3D9LastPixelShader != nullptr) {
        D3D9LastPixelShader = nullptr;
        ComCall<kSlotSetPixelShader, long>(Device(), static_cast<void*>(nullptr));
    }
}
// Cambia el pixel shader activo (SetPixelShader, +0x1AC) si difiere del ultimo.
void SelectPixelShader(void* shader) {
    if (D3D9LastPixelShader != shader) {
        D3D9LastPixelShader = shader;
        ComCall<kSlotSetPixelShader, long>(Device(), shader);
    }
}
// Cambia el vertex shader activo (SetVertexShader, +0x170) al de la malla si difiere del ultimo.
void SelectVertexShader(const RxD3D9InstanceData* instance) {
    if (D3D9LastVertexShader != instance->vertexShader) {
        D3D9LastVertexShader = instance->vertexShader;
        ComCall<kSlotSetVertexShader, long>(Device(), static_cast<void*>(instance->vertexShader));
    }
}
// Vacia los estados pendientes (0x7FC200) y dibuja la malla: con bufer de indices DrawIndexedPrimitive(tipo, baseIndex, 0, numVertices, startIndex, numPrimitives; +0x148), si no DrawPrimitive(tipo, baseIndex, numPrimitives; +0x144).
void DrawBatch(const RxD3D9ResEntryHeader* header, const RxD3D9InstanceData* instance, RwUInt32 baseIndex) {
    const bool indexed = header->indexBuffer != nullptr;
    reinterpret_cast<void(__cdecl *)()>(kD3D9FlushRenderStates)();
    if (indexed) {
        ComCall<kSlotDrawIndexedPrimitive, long>(Device(), static_cast<RwUInt32>(header->primType), baseIndex, 0u, instance->numVertices, instance->startIndex, instance->numPrimitives);
    } else {
        ComCall<kSlotDrawPrimitive, long>(Device(), static_cast<RwUInt32>(header->primType), baseIndex, instance->numPrimitives);
    }
}
// Si la matriz UV existe y no es la identidad (bandera interna 0x20000): TEXTURETRANSFORMFLAGS (0x18) de la etapa = COUNT2, copia los seis dwords utiles (right.x, right.y, up.x, up.y, pos.x, pos.y) a _11, _12, _21, _22, _31, _32
// de la matriz de trabajo 0x8E29C8 (el resto no se toca) y la activa como transformada `transformState` (0x10 + etapa).
void ApplyUvTransform(const RwMatrix* matrix, RwUInt32 stage, RwUInt32 transformState) {
    if (matrix == nullptr || (matrix->flags & 0x20000) != 0) {
        return;
    }
    Tss(stage, 0x18, 2);
    auto* const scratch = reinterpret_cast<RwUInt32*>(kUvScratchMatrix);
    const auto* const raw = reinterpret_cast<const RwUInt32*>(matrix);
    scratch[0] = raw[0];
    scratch[1] = raw[1];
    scratch[4] = raw[4];
    scratch[5] = raw[5];
    scratch[8] = raw[12];
    scratch[9] = raw[13];
    RwD3D9SetTransform(transformState, scratch);
}

// Vertice de salida del instanciado del relieve (FVF 0x252 = XYZ | NORMAL | DIFFUSE | TEX2, 0x2C bytes). Posicion, normal y coordenadas se copian como dwords (sin pasar por la x87).
struct BumpVertex {
    RwUInt32 pos[3];    // +0x00
    RwUInt32 normal[3]; // +0x0C
    RwUInt32 color;     // +0x18
    RwUInt32 uv[2];     // +0x1C
    RwReal   bump[2];   // +0x24
};
static_assert(sizeof(BumpVertex) == 0x2C && offsetof(BumpVertex, bump) == 0x24);
} // namespace

static_assert(sizeof(MatFXEffectData) == 0x18 && offsetof(MatFXEffectData, flag) == 0x14 && sizeof(rpMatFXMaterialData) == 0x34 && offsetof(rpMatFXMaterialData, flags) == 0x30);
static_assert(offsetof(MatFXBumpMapData, texture) == 4 && offsetof(MatFXBumpMapData, bumpTexture) == 8 && offsetof(MatFXBumpMapData, coef) == 0xC && offsetof(MatFXBumpMapData, invBumpWidth) == 0x10);
static_assert(offsetof(MatFXEnvMapData, coef) == 8 && offsetof(MatFXEnvMapData, useFrameBufferAlpha) == 0xC && offsetof(MatFXDualData, srcBlendMode) == 4 && offsetof(MatFXDualData, dstBlendMode) == 8);
static_assert(offsetof(RpAtomic, pipeline) == 0x6C && offsetof(RpWorldSector, pipeline) == 0x7C && offsetof(RwTexture, refCount) == 0x54 && offsetof(RwTexture, name) == 0x10 && offsetof(RwRaster, width) == 0xC);
static_assert(offsetof(RwFrame, objectList) == 0x90 && sizeof(RxD3D9InstanceData) == 0x24 && offsetof(RxD3D9ResEntryHeader, vertexDeclaration) == 0x34 && offsetof(RxD3D9ResEntryHeader, vertexStream) == 0x10);

// Funciones del modulo sin nombre en el SDK (nombres inferidos); solo se enganchan, por eso se declaran aqui.
rpMatFXMaterialData* _rpMatFXMaterialDataClean(rpMatFXMaterialData* data);
MatFXEffectData* _rpMatFXMaterialFindEffect(const RpMaterial* material, RwUInt32 type);
MatFXEffectData* _rpMatFXMaterialFindEffectConst(const RpMaterial* material, RwUInt32 type);
RwStream* _rpMatFXAtomicStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
RwStream* _rpMatFXWorldSectorStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
void _rpMatFXD3D9DrawMesh(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance);
RwBool _rpMatFXD3D9SetEnvMapTexture(RwTexture* texture, RwUInt32 stage, RwFrame* frame);
RwBool _rpMatFXD3D9BumpMapInstance(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, void* destination);
RwBool _rpMatFXPipelinesCreate();
RpAtomic* _rpMatFXAtomicPipelineSet(RpAtomic* atomic);
RpWorldSector* _rpMatFXWorldSectorPipelineSet(RpWorldSector* sector);
RwBool _rpMatFXD3D9EffectStub(void* effect, RwInt32 pass);
RwTexture* _rpMatFXD3D9BumpedTextureCreate(RwTexture* texture, RwTexture* bumpTexture);
RwTexture* _rpMatFXD3D9BumpedTextureCombine(RwTexture* texture, RwTexture* bumpTexture);
void _rpMatFXD3D9DrawMeshUVTransform(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwTexture* texture);
void _rpMatFXD3D9RenderDualTexture(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwTexture* baseTexture, RwTexture* dualTexture);
void _rpMatFXD3D9RenderEnvMapPass(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwUInt32 pass, RwTexture* baseTexture, RwTexture* envTexture);
void _rpMatFXD3D9RenderBumpMap(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwTexture* baseTexture, RwTexture* bumpedTexture, RwTexture* envTexture);
RwBool _rpMatFXPipelinesDestroy();
void _rwD3D9SetClippingForObject(void* object, RwInt32 type);   // 0x756D90 (rpworld.cpp)
RwBool _rwD3D9RenderStateVertexAlphaEnable(RwBool enable);      // 0x7FE0A0 (rwcore.cpp)

// Lotes S13A y S14A (sin validar): callbacks del plugin 0x120 y del canal D3D9 que el codigo propio pasaba por direccion del EXE (todos cdecl). Se registran por su direccion propia (RpMatFXPluginAttach y
// _rpMatFXPipelinesCreate) y llevan hook en la direccion del EXE.
static void*     _rpMatFXEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                               // 0x810BB0
static void*     _rpMatFXEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                              // 0x810BF0
static void*     _rpMatFXMaterialConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                            // 0x810C40
static void*     _rpMatFXMaterialDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                             // 0x810C60
static void*     _rpMatFXMaterialCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);                         // 0x810D20
static RwStream* _rpMatFXMaterialStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x8111F0
static RwStream* _rpMatFXMaterialStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);        // 0x811410
static RwInt32   _rpMatFXMaterialStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                   // 0x811970
static void*     _rpMatFXAtomicConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                              // 0x811A00
static void*     _rpMatFXAtomicDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                               // 0x811A20
static void*     _rpMatFXAtomicCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);                          // 0x811A40
static RwStream* _rpMatFXAtomicStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);   // 0x811A60
static RwInt32   _rpMatFXAtomicStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                     // 0x811AE0
static void*     _rpMatFXWorldSectorConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                         // 0x811B00
static void*     _rpMatFXWorldSectorDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                          // 0x811B20
static void*     _rpMatFXWorldSectorCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);                     // 0x811B40
static RwStream* _rpMatFXWorldSectorStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject); // 0x811B60
static RwInt32   _rpMatFXWorldSectorStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);                                // 0x811BE0
static void      _rpMatFXD3D9AllInOneRender(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags);                                       // 0x815C20
static RwBool    _rpMatFXAtomicInstanceCallBack(void* object, RxD3D9ResEntryHeader* header, RwBool reinstance);                                      // 0x8160A0
static RwBool    _rpMatFXWorldSectorInstanceCallBack(void* object, RxD3D9ResEntryHeader* header, RwBool reinstance);                                 // 0x816120

void RpMatFXInjectHooks() {
    RH_ScopedCategory("Renderware");
    RH_ScopedNamespaceName("RpMatFX");

    // Lote R8A (sin validar)
    RH_ScopedGlobalInstall(RpMatFXPluginAttach, 0x810AA0);
    RH_ScopedGlobalInstall(_rpMatFXMaterialDataClean, 0x810CA0);
    RH_ScopedGlobalInstall(_rpMatFXMaterialFindEffect, 0x811150);
    RH_ScopedGlobalInstall(_rpMatFXMaterialFindEffectConst, 0x8111A0);
    RH_ScopedGlobalInstall(_rpMatFXAtomicStreamRead, 0x811A90);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorStreamRead, 0x811B90);
    RH_ScopedGlobalInstall(RpMatFXAtomicQueryEffects, 0x811C30);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetEffects, 0x811C80);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetEffects, 0x812140);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetBumpMapTexture, 0x812160);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetEnvMapFrame, 0x8125B0);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetEnvMapCoefficient, 0x812680);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetDualBlendModes, 0x8128C0);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetUVTransformMatrices, 0x8129E0);
    RH_ScopedGlobalInstall(_rpMatFXD3D9DrawMesh, 0x812C90);
    RH_ScopedGlobalInstall(_rpMatFXD3D9SetEnvMapTexture, 0x814270);
    RH_ScopedGlobalInstall(_rpMatFXD3D9BumpMapInstance, 0x8149E0);
    RH_ScopedGlobalInstall(_rpMatFXPipelinesCreate, 0x815E60);
    RH_ScopedGlobalInstall(_rpMatFXAtomicPipelineSet, 0x816260);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorPipelineSet, 0x816270);
    RH_ScopedGlobalInstall(_rpMatFXD3D9EffectStub, 0x816280);
    RH_ScopedGlobalInstall(_rpMatFXD3D9BumpedTextureCreate, 0x816290);

    // Lote R10A (sin validar)
    RH_ScopedGlobalInstall(_rpMatFXD3D9BumpedTextureCombine, 0x810830);
    RH_ScopedGlobalInstall(_rpMatFXD3D9DrawMeshUVTransform, 0x812AC0);
    RH_ScopedGlobalInstall(_rpMatFXD3D9RenderDualTexture, 0x812D40);
    RH_ScopedGlobalInstall(_rpMatFXD3D9RenderEnvMapPass, 0x813A10);
    RH_ScopedGlobalInstall(_rpMatFXD3D9RenderBumpMap, 0x814460);
    RH_ScopedGlobalInstall(_rpMatFXPipelinesDestroy, 0x816190);

    // Lotes S13A y S14A (sin validar)
    RH_ScopedGlobalInstall(RpMatFXMaterialDataSetFreeListCreateParams, 0x810700);
    RH_ScopedGlobalInstall(_rpMatFXEngineDestructor, 0x810BB0);
    RH_ScopedGlobalInstall(_rpMatFXEngineConstructor, 0x810BF0);
    RH_ScopedGlobalInstall(_rpMatFXMaterialConstructor, 0x810C40);
    RH_ScopedGlobalInstall(_rpMatFXMaterialDestructor, 0x810C60);
    RH_ScopedGlobalInstall(_rpMatFXMaterialCopy, 0x810D20);
    RH_ScopedGlobalInstall(_rpMatFXMaterialStreamWrite, 0x8111F0);
    RH_ScopedGlobalInstall(_rpMatFXMaterialStreamRead, 0x811410);
    RH_ScopedGlobalInstall(_rpMatFXMaterialStreamGetSize, 0x811970);
    RH_ScopedGlobalInstall(_rpMatFXAtomicConstructor, 0x811A00);
    RH_ScopedGlobalInstall(_rpMatFXAtomicDestructor, 0x811A20);
    RH_ScopedGlobalInstall(_rpMatFXAtomicCopy, 0x811A40);
    RH_ScopedGlobalInstall(_rpMatFXAtomicStreamWrite, 0x811A60);
    RH_ScopedGlobalInstall(_rpMatFXAtomicStreamGetSize, 0x811AE0);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorConstructor, 0x811B00);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorDestructor, 0x811B20);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorCopy, 0x811B40);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorStreamWrite, 0x811B60);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorStreamGetSize, 0x811BE0);
    RH_ScopedGlobalInstall(RpMatFXAtomicEnableEffects, 0x811C00);
    RH_ScopedGlobalInstall(RpMatFXWorldSectorEnableEffects, 0x811C40);
    RH_ScopedGlobalInstall(RpMatFXWorldSectorQueryEffects, 0x811C70);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetupBumpMap, 0x811E00);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetupEnvMap, 0x811ED0);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetupDualTexture, 0x812040);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetBumpMapFrame, 0x812320);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetBumpMapCoefficient, 0x812380);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetBumpMapBumpedTexture, 0x8123E0);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetBumpMapTexture, 0x812430);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetBumpMapFrame, 0x812480);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetBumpMapCoefficient, 0x8124D0);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetEnvMapTexture, 0x812530);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetEnvMapFrameBufferAlpha, 0x812610);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetEnvMapTexture, 0x8126F0);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetEnvMapFrame, 0x812740);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetEnvMapFrameBufferAlpha, 0x812790);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetEnvMapCoefficient, 0x8127E0);
    RH_ScopedGlobalInstall(RpMatFXMaterialSetDualTexture, 0x812830);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetDualTexture, 0x812930);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetDualBlendModes, 0x812980);
    RH_ScopedGlobalInstall(RpMatFXMaterialGetUVTransformMatrices, 0x812A50);
    RH_ScopedGlobalInstall(_rpMatFXD3D9AllInOneRender, 0x815C20);
    RH_ScopedGlobalInstall(_rpMatFXAtomicInstanceCallBack, 0x8160A0);
    RH_ScopedGlobalInstall(_rpMatFXWorldSectorInstanceCallBack, 0x816120);
    RH_ScopedGlobalInstall(RpMatFXGetD3D9Pipeline, 0x8162F0);
}

void RpMatFXMaterialDataSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    MatFXFreeListBlockSize = blockSize;
    MatFXFreeListNumPrealloc = numBlocksToPrealloc;
}

// 0x810AA0 (lote R8A, sin validar; cdecl sin argumentos, `ret`, 266 bytes): registra el plugin 0x120 y devuelve TRUE si todos los registros dieron >= 0 (el ultimo lo comprueba con setge; los demas
// devuelven FALSE de inmediato). Orden: motor (ctor 0x810BF0 abre el modulo y crea la lista libre y los pipelines con 0x815E60; dtor 0x810BB0), material (tamano 4, ctor 0x810C40, dtor 0x810C60, copia 0x810D20;
// el desplazamiento va a 0xC9AB74) y su flujo (lee 0x811410, escribe 0x8111F0, tamano 0x811970), atomico (tamano 4, ctor 0x811A00, dtor 0x811A20, copia 0x811A40; desplazamiento a 0xC9AB84) y su flujo
// (lee 0x811A90, escribe 0x811A60, tamano 0x811AE0) y sector (tamano 4, ctor 0x811B00, dtor 0x811B20, copia 0x811B40; a 0xC9AB88) con su flujo (lee 0x811B90, escribe 0x811B60, tamano 0x811BE0).
RwBool RpMatFXPluginAttach() {
    if (RwEngineRegisterPlugin(0, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(&_rpMatFXEngineConstructor), reinterpret_cast<RwPluginObjectDestructor>(&_rpMatFXEngineDestructor)) < 0) {
        return FALSE;
    }
    MatFXMaterialDataOffset = RpMaterialRegisterPlugin(4, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(&_rpMatFXMaterialConstructor), reinterpret_cast<RwPluginObjectDestructor>(&_rpMatFXMaterialDestructor),
                                                       reinterpret_cast<RwPluginObjectCopy>(&_rpMatFXMaterialCopy));
    if (MatFXMaterialDataOffset < 0) {
        return FALSE;
    }
    if (RpMaterialRegisterPluginStream(kMatFXPluginID, reinterpret_cast<RwPluginDataChunkReadCallBack>(&_rpMatFXMaterialStreamRead), reinterpret_cast<RwPluginDataChunkWriteCallBack>(&_rpMatFXMaterialStreamWrite),
                                       reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(&_rpMatFXMaterialStreamGetSize)) < 0) {
        return FALSE;
    }
    MatFXAtomicDataOffset = RpAtomicRegisterPlugin(4, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(&_rpMatFXAtomicConstructor), reinterpret_cast<RwPluginObjectDestructor>(&_rpMatFXAtomicDestructor),
                                                   reinterpret_cast<RwPluginObjectCopy>(&_rpMatFXAtomicCopy));
    if (MatFXAtomicDataOffset < 0) {
        return FALSE;
    }
    if (RpAtomicRegisterPluginStream(kMatFXPluginID, reinterpret_cast<RwPluginDataChunkReadCallBack>(&_rpMatFXAtomicStreamRead), reinterpret_cast<RwPluginDataChunkWriteCallBack>(&_rpMatFXAtomicStreamWrite),
                                     reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(&_rpMatFXAtomicStreamGetSize)) < 0) {
        return FALSE;
    }
    MatFXWorldSectorDataOffset = RpWorldSectorRegisterPlugin(4, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(&_rpMatFXWorldSectorConstructor), reinterpret_cast<RwPluginObjectDestructor>(&_rpMatFXWorldSectorDestructor),
                                                             reinterpret_cast<RwPluginObjectCopy>(&_rpMatFXWorldSectorCopy));
    if (MatFXWorldSectorDataOffset < 0) {
        return FALSE;
    }
    return RpWorldSectorRegisterPluginStream(kMatFXPluginID, reinterpret_cast<RwPluginDataChunkReadCallBack>(&_rpMatFXWorldSectorStreamRead), reinterpret_cast<RwPluginDataChunkWriteCallBack>(&_rpMatFXWorldSectorStreamWrite),
                                             reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(&_rpMatFXWorldSectorStreamGetSize)) >= 0
               ? TRUE
               : FALSE;
}

// 0x810CA0 (lote R8A, sin validar; cdecl, un argumento, `ret`, 105 bytes): libera lo que cuelga de cada una de las dos ranuras de efecto segun su bandera y pone a cero los 13 dwords de los datos. Relieve (1):
// destruye `texture` (+4) y `bumpTexture` (+8) si no son NULL; entorno (2): `texture` (+4); dual (4): `texture` (+0); transformacion UV (5): pone a NULL los dos punteros de matriz (+0 y +4); las banderas
// 3 y 6 y las demas no liberan nada. Devuelve los datos.
rpMatFXMaterialData* _rpMatFXMaterialDataClean(rpMatFXMaterialData* data) {
    for (RwInt32 slot = 0; slot < 2; ++slot) {
        MatFXEffectData& effect = data->data[slot];
        RwTexture* texture = nullptr;
        switch (static_cast<RwUInt32>(effect.flag)) {
        case rpMATFXEFFECTBUMPMAP:
            if (effect.data.bumpMap.texture != nullptr) {
                RwTextureDestroy(effect.data.bumpMap.texture);
            }
            texture = effect.data.bumpMap.bumpTexture;
            break;
        case rpMATFXEFFECTENVMAP:
            texture = effect.data.envMap.texture;
            break;
        case rpMATFXEFFECTDUAL:
            texture = effect.data.dual.texture;
            break;
        case rpMATFXEFFECTUVTRANSFORM:
            effect.data.uvAnim.baseTransform = nullptr;
            effect.data.uvAnim.dualTransform = nullptr;
            continue;
        default:
            continue;
        }
        if (texture != nullptr) {
            RwTextureDestroy(texture);
        }
    }
    memset(data, 0, sizeof(rpMatFXMaterialData));
    return data;
}

// 0x811150 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 76 bytes): macro de busqueda del SDK compilada como funcion: primera ranura de efecto (de dos) del material cuya bandera es `type` (comparacion
// de 32 bits); NULL si no hay. Misma logica que 0x8111A0 (el compilador la emitio dos veces).
MatFXEffectData* _rpMatFXMaterialFindEffect(const RpMaterial* material, RwUInt32 type) {
    return FindEffect(material, type);
}

// 0x8111A0 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 76 bytes): idem 0x811150 (copia identica del compilador).
MatFXEffectData* _rpMatFXMaterialFindEffectConst(const RpMaterial* material, RwUInt32 type) {
    return FindEffect(material, type);
}

// 0x811A90 (lote R8A, sin validar; cdecl, 5 argumentos, `ret`, 80 bytes): lectura del flujo de la extension de RpAtomic (RwPluginDataChunkReadCallBack; solo se usan el flujo y el objeto): un entero de 4 bytes
// (si falla: NULL); si es distinto de cero y la bandera del atomico (en object + 0xC9AB84) es 0, pone el pipeline de MatFX (0x816260) y, si devuelve algo, la bandera a 1. Devuelve el flujo.
RwStream* _rpMatFXAtomicStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwInt32 enabled;
    if (RwStreamReadInt32(stream, &enabled, 4) == nullptr) {
        return nullptr;
    }
    if (enabled != 0) {
        RwInt32* const flag = reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(object) + MatFXAtomicDataOffset);
        if (*flag == 0 && _rpMatFXAtomicPipelineSet(static_cast<RpAtomic*>(object)) != nullptr) {
            *flag = 1;
        }
    }
    return stream;
}

// 0x811B90 (lote R8A, sin validar; cdecl, 5 argumentos, `ret`, 80 bytes): igual que 0x811A90 para RpWorldSector (bandera en object + 0xC9AB88, pipeline con 0x816270).
RwStream* _rpMatFXWorldSectorStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwInt32 enabled;
    if (RwStreamReadInt32(stream, &enabled, 4) == nullptr) {
        return nullptr;
    }
    if (enabled != 0) {
        RwInt32* const flag = reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(object) + MatFXWorldSectorDataOffset);
        if (*flag == 0 && _rpMatFXWorldSectorPipelineSet(static_cast<RpWorldSector*>(object)) != nullptr) {
            *flag = 1;
        }
    }
    return stream;
}

// 0x811C00 (lote S13A, sin validar; 48 bytes; era una envoltura; cdecl, (atomico)): si la bandera del atomico (en atomic + 0xC9AB84) es 0 pone el pipeline de MatFX (0x816260); si devuelve NULL, devuelve NULL sin tocar
// la bandera; si no la pone a 1. Devuelve el atomico.
RpAtomic* RpMatFXAtomicEnableEffects(RpAtomic* atomic) {
    RwInt32* const flag = reinterpret_cast<RwInt32*>(reinterpret_cast<RwUInt8*>(atomic) + MatFXAtomicDataOffset);
    if (*flag == 0) {
        if (_rpMatFXAtomicPipelineSet(atomic) == nullptr) {
            return nullptr;
        }
        *flag = 1;
    }
    return atomic;
}

// 0x811C30 (lote R8A, sin validar; cdecl, 1 argumento, `ret`, 14 bytes): lee la bandera del atomico (en atomic + 0xC9AB84).
RwBool RpMatFXAtomicQueryEffects(RpAtomic* atomic) {
    return *reinterpret_cast<RwBool*>(reinterpret_cast<RwUInt8*>(atomic) + MatFXAtomicDataOffset);
}

// 0x811C40 (lote S13A, sin validar; 48 bytes; era una envoltura; cdecl, (sector)): igual que 0x811C00 para RpWorldSector (bandera en sector + 0xC9AB88, pipeline con 0x816270).
RpWorldSector* RpMatFXWorldSectorEnableEffects(RpWorldSector* worldSector) {
    RwInt32* const flag = reinterpret_cast<RwInt32*>(reinterpret_cast<RwUInt8*>(worldSector) + MatFXWorldSectorDataOffset);
    if (*flag == 0) {
        if (_rpMatFXWorldSectorPipelineSet(worldSector) == nullptr) {
            return nullptr;
        }
        *flag = 1;
    }
    return worldSector;
}

// 0x811C70 (lote S13A, sin validar; 14 bytes; era una envoltura; cdecl, (sector)): lee la bandera del sector (en sector + 0xC9AB88).
RwBool RpMatFXWorldSectorQueryEffects(RpWorldSector* worldSector) {
    return *reinterpret_cast<RwBool*>(reinterpret_cast<RwUInt8*>(worldSector) + MatFXWorldSectorDataOffset);
}

// 0x811C80 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 356 bytes): asigna (RwFreeListAlloc de 0xC9AB80, pista 0x30120; NULL si falla) y pone a cero los datos del material si no los tiene; limpia
// los anteriores (0x810CA0) si flags == 0 o si habia otros efectos distintos; guarda las banderas (+0x30) y prepara las ranuras: 1 bump; 2 entorno; 3 bump en la ranura 0 y entorno en la 1; 4 dual (ranura 0, con mezcla
// SRCALPHA/INVSRCALPHA y las llamadas al relleno 0x816280 con 0xA y 0xB); 5 transformacion UV; 6 transformacion UV en la 0 y dual en la 1 (con lo mismo). Las demas banderas no hacen nada. Devuelve el material.
RpMaterial* RpMatFXMaterialSetEffects(RpMaterial* material, RpMatFXMaterialFlags flags) {
    rpMatFXMaterialData* data = MaterialData(material);
    if (data == nullptr) {
        data = static_cast<rpMatFXMaterialData*>(RwFreeListAlloc(MatFXDataFreeList, kMatFXMaterialMemHint));
        if (data == nullptr) {
            return nullptr;
        }
        memset(data, 0, sizeof(rpMatFXMaterialData));
        MaterialData(material) = data;
    }
    if (flags == 0 || (data->flags != 0 && data->flags != flags)) {
        _rpMatFXMaterialDataClean(data);
    }
    data->flags = flags;
    switch (static_cast<RwUInt32>(flags)) {
    case 1:
        data->data[0].flag = rpMATFXEFFECTBUMPMAP;
        return material;
    case 2:
        data->data[0].flag = rpMATFXEFFECTENVMAP;
        return material;
    case 3:
        data->data[0].flag = rpMATFXEFFECTBUMPMAP;
        data->data[1].flag = rpMATFXEFFECTENVMAP;
        return material;
    case 4:
    case 6: {
        if (flags == 4) {
            data->data[0].flag = rpMATFXEFFECTDUAL;
        } else {
            data->data[0].flag = rpMATFXEFFECTUVTRANSFORM;
            data->data[1].flag = rpMATFXEFFECTDUAL;
        }
        MatFXEffectData* const dual = FindEffect(material, rpMATFXEFFECTDUAL);
        dual->data.dual.dstBlendMode = rwBLENDINVSRCALPHA;
        dual->data.dual.srcBlendMode = rwBLENDSRCALPHA;
        _rpMatFXD3D9EffectStub(dual, 0xA);
        _rpMatFXD3D9EffectStub(dual, 0xB);
        return material;
    }
    case 5:
        data->data[0].flag = rpMATFXEFFECTUVTRANSFORM;
        return material;
    default:
        return material;
    }
}

// 0x811E00 (lote S14A, sin validar; 202 bytes; era una envoltura; cdecl, (material, textura de relieve, marco, coeficiente)): RpMatFXMaterialSetBumpMapTexture (0x812160); si devuelve NULL, devuelve NULL. Despues guarda el
// marco en la ranura de relieve (bandera 1; NULL si no existe y entonces el EXE escribe en la direccion 0), comprueba que el material no sea NULL (si lo es, NULL) y guarda el coeficiente NEGADO en +0xC (fld + fchs + fstp:
// un NaN senalizante sale silencioso). Devuelve el material.
RpMaterial* RpMatFXMaterialSetupBumpMap(RpMaterial* material, RwTexture* texture, RwFrame* frame, RwReal coef) {
    if (RpMatFXMaterialSetBumpMapTexture(material, texture) == nullptr) {
        return nullptr;
    }
    FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.frame = frame;
    if (material == nullptr) {
        return nullptr;
    }
    StoreBits(FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.coef, X87ReloadNegated(RealBits(coef)));
    return material;
}

// 0x811ED0 (lote S14A, sin validar; 355 bytes; era una envoltura; cdecl, (material, textura, marco, usar alfa del bufer, coeficiente)): en la ranura de entorno (bandera 2; NULL si no existe y entonces el EXE escribe
// en la direccion 4): textura + 1 referencia (SIN comprobar NULL), destruye la anterior (+4) y guarda la nueva. Material NULL: devuelve NULL. Despues guarda el marco (+0), el alfa del bufer (+0xC) y el coeficiente
// (+8, palabra tal cual, sin x87). Devuelve el material.
RpMaterial* RpMatFXMaterialSetupEnvMap(RpMaterial* material, RwTexture* texture, RwFrame* frame, RwBool useFrameBufferAlpha, RwReal coef) {
    MatFXEnvMapData& env = FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap;
    ++texture->refCount;
    if (env.texture != nullptr) {
        RwTextureDestroy(env.texture);
        env.texture = nullptr;
    }
    env.texture = texture;
    if (material == nullptr) {
        return nullptr;
    }
    FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.frame = frame;
    FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.useFrameBufferAlpha = useFrameBufferAlpha;
    StoreBits(FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.coef, RealBits(coef));
    return material;
}

// 0x812040 (lote S13A, sin validar; 247 bytes; era una envoltura; cdecl, (material, textura, mezcla origen, mezcla destino)): en la ranura dual (bandera 4; NULL si no existe y entonces el EXE escribe en 0): textura + 1
// referencia (SIN comprobar NULL), destruye la anterior (+0), guarda la nueva y llama al relleno 0x816280 con 2 y 1. Material NULL: devuelve NULL. Despues guarda las dos mezclas (+4, +8) y llama al relleno con 0xA y
// 0xB. Devuelve el material.
RpMaterial* RpMatFXMaterialSetupDualTexture(RpMaterial* material, RwTexture* texture, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode) {
    MatFXEffectData* const effect = FindEffect(material, rpMATFXEFFECTDUAL);
    ++texture->refCount;
    if (effect->data.dual.texture != nullptr) {
        RwTextureDestroy(effect->data.dual.texture);
        effect->data.dual.texture = nullptr;
    }
    effect->data.dual.texture = texture;
    _rpMatFXD3D9EffectStub(effect, 2);
    _rpMatFXD3D9EffectStub(effect, 1);
    if (material == nullptr) {
        return nullptr;
    }
    MatFXEffectData* const dual = FindEffect(material, rpMATFXEFFECTDUAL);
    dual->data.dual.srcBlendMode = srcBlendMode;
    dual->data.dual.dstBlendMode = dstBlendMode;
    _rpMatFXD3D9EffectStub(dual, 0xA);
    _rpMatFXD3D9EffectStub(dual, 0xB);
    return material;
}

// 0x812140 (lote R8A, sin validar; cdecl, 1 argumento, `ret`, 22 bytes): banderas de efecto del material; 0 si no tiene datos.
RpMatFXMaterialFlags RpMatFXMaterialGetEffects(const RpMaterial* material) {
    const rpMatFXMaterialData* const data = MaterialData(material);
    if (data == nullptr) {
        return rpMATFXEFFECTNULL;
    }
    return data->flags;
}

// 0x812160 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 438 bytes; sustituye la envoltura): ranura de relieve (bandera 1; NULL si no existe y entonces el EXE desreferencia NULL). Destruye `bumpTexture` (+8)
// y `texture` (+4, con invBumpWidth a 0) anteriores. Con textura NULL: invBumpWidth = 1.0f / ancho del raster de la textura del material. Con textura: la guarda en +8 con refCount + 1; si el raster de la textura de
// relieve o el de la del material (si existe) tiene ancho 0, termina sin generar nada. Si no, el nombre es el entrelazado de los de las dos texturas (hasta 30 caracteres, un caracter de cada una por vuelta; si el
// material no tiene textura, el de la de relieve dos veces) y se busca en el diccionario actual: si no esta, se crea con 0x816290 (NULL: devuelve NULL) y se anade al diccionario; si esta, refCount + 1.
// Termina con invBumpWidth = 1.0f / ancho del raster de la textura resultante (la generada, o la del material en el caso de textura NULL). Devuelve el material.
RpMaterial* RpMatFXMaterialSetBumpMapTexture(RpMaterial* material, RwTexture* texture) {
    MatFXEffectData* const effect = FindEffect(material, rpMATFXEFFECTBUMPMAP);
    MatFXBumpMapData& bump = effect->data.bumpMap;
    if (bump.bumpTexture != nullptr) {
        RwTextureDestroy(bump.bumpTexture);
        bump.bumpTexture = nullptr;
    }
    if (bump.texture != nullptr) {
        RwTextureDestroy(bump.texture);
        bump.texture = nullptr;
        bump.invBumpWidth = 0.0f;
    }
    if (texture == nullptr) {
        bump.invBumpWidth = 1.0f / static_cast<RwReal>(material->texture->raster->width);
        return material;
    }
    bump.bumpTexture = texture;
    ++texture->refCount;
    RwTexture* const materialTexture = material->texture;
    if (texture->raster->width == 0) {
        return material;
    }
    if (materialTexture != nullptr && materialTexture->raster->width == 0) {
        return material;
    }
    RwChar name[32] = {};
    const RwChar* sources[2] = { materialTexture != nullptr ? materialTexture->name : texture->name, texture->name };
    RwChar* out = name;
    RwInt32 count = 0;
    while (*sources[0] != 0 || *sources[1] != 0) {
        for (RwInt32 k = 0; k < 2; ++k) {
            const RwChar c = *sources[k];
            if (c != 0) {
                *out++ = c;
                ++sources[k];
                ++count;
            }
        }
        if (count >= 0x1E) {
            break;
        }
    }
    *out = 0;
    RwTexDictionary* const dict = RwTexDictionaryGetCurrent();
    bump.texture = nullptr;
    if (dict != nullptr) {
        bump.texture = RwTexDictionaryFindNamedTexture(dict, name);
    }
    if (bump.texture == nullptr) {
        bump.texture = _rpMatFXD3D9BumpedTextureCreate(materialTexture, texture);
        if (bump.texture == nullptr) {
            return nullptr;
        }
        if (dict != nullptr) {
            RwTexDictionaryAddTexture(dict, bump.texture);
        }
    } else {
        ++bump.texture->refCount;
    }
    bump.invBumpWidth = 1.0f / static_cast<RwReal>(bump.texture->raster->width);
    return material;
}

// 0x812320 (lote S13A, sin validar; 86 bytes; era una envoltura; cdecl, (material, marco)): guarda el marco en la ranura de relieve (bandera 1; NULL si no existe y entonces el EXE escribe en 0). Devuelve el material.
RpMaterial* RpMatFXMaterialSetBumpMapFrame(RpMaterial* material, RwFrame* frame) {
    FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.frame = frame;
    return material;
}

// 0x812380 (lote S14A, sin validar; 93 bytes; era una envoltura; cdecl, (material, coeficiente)): guarda el coeficiente NEGADO (fld + fchs + fstp: un NaN senalizante sale silencioso) en +0xC de la ranura de relieve
// (NULL si no existe y entonces el EXE escribe en 0xC). Devuelve el material.
RpMaterial* RpMatFXMaterialSetBumpMapCoefficient(RpMaterial* material, RwReal coef) {
    StoreBits(FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.coef, X87ReloadNegated(RealBits(coef)));
    return material;
}

// 0x812430 (lote S13A, sin validar; 77 bytes; era una envoltura; cdecl, (material)): textura de relieve (+8) de la ranura de relieve (NULL si no existe y entonces el EXE lee la direccion 8).
RwTexture* RpMatFXMaterialGetBumpMapTexture(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.bumpTexture;
}

// 0x8123E0 (lote S13A, sin validar; 77 bytes; era una envoltura; cdecl, (material)): textura generada (+4) de la ranura de relieve (NULL si no existe y entonces el EXE lee la direccion 4).
RwTexture* RpMatFXMaterialGetBumpMapBumpedTexture(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.texture;
}

// 0x812480 (lote S14A, sin validar; 75 bytes; era una envoltura; cdecl, (material)): marco (+0) de la ranura de relieve (NULL si no existe y entonces el EXE lee la direccion 0).
RwFrame* RpMatFXMaterialGetBumpMapFrame(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.frame;
}

// 0x8124D0 (lote S14A, sin validar; 81 bytes; era una envoltura; cdecl, (material)): coeficiente de relieve = - (+0xC) (fld + fchs; devuelto en ST0). NULL si no existe la ranura: el EXE lee la direccion 0xC.
RwReal RpMatFXMaterialGetBumpMapCoefficient(const RpMaterial* material) {
    return -FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.coef;
}

// 0x812530 (lote S14A, sin validar; 114 bytes; era una envoltura; cdecl, (material, textura)): en la ranura de entorno (bandera 2; NULL si no existe y entonces el EXE escribe en 4): textura + 1 referencia (SIN
// comprobar NULL), destruye la anterior (+4) y guarda la nueva. Devuelve el material.
RpMaterial* RpMatFXMaterialSetEnvMapTexture(RpMaterial* material, RwTexture* texture) {
    MatFXEnvMapData& env = FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap;
    ++texture->refCount;
    if (env.texture != nullptr) {
        RwTextureDestroy(env.texture);
        env.texture = nullptr;
    }
    env.texture = texture;
    return material;
}

// 0x8125B0 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 95 bytes): guarda el marco en la ranura de entorno (bandera 2; NULL si no existe y entonces el EXE escribe en la direccion 0). Sin cuenta de
// referencias. Devuelve el material.
RpMaterial* RpMatFXMaterialSetEnvMapFrame(RpMaterial* material, RwFrame* frame) {
    FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.frame = frame;
    return material;
}

// 0x812610 (lote S14A, sin validar; 97 bytes; era una envoltura; cdecl, (material, alfa del bufer)): guarda el valor en +0xC de la ranura de entorno (NULL si no existe y entonces el EXE escribe en 0xC). Devuelve el material.
RpMaterial* RpMatFXMaterialSetEnvMapFrameBufferAlpha(RpMaterial* material, RwBool useFrameBufferAlpha) {
    FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.useFrameBufferAlpha = useFrameBufferAlpha;
    return material;
}

// 0x812680 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 97 bytes): guarda el coeficiente (copiado como dword, sin pasar por la x87) en la ranura de entorno (+8; NULL si no existe y entonces
// escribe en 8). Devuelve el material.
RpMaterial* RpMatFXMaterialSetEnvMapCoefficient(RpMaterial* material, RwReal coef) {
    FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.coef = coef;
    return material;
}

// 0x8126F0 / 0x812740 / 0x812790 / 0x8127E0 (lote S14A, sin validar; 77, 75, 77 y 77 bytes; eran envolturas; cdecl, (material)): campos de la ranura de entorno (bandera 2; NULL si no existe y entonces el EXE lee la direccion
// del campo): textura (+4), marco (+0), alfa del bufer (+0xC) y coeficiente (+8, fld: devuelto en ST0).
RwTexture* RpMatFXMaterialGetEnvMapTexture(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.texture;
}

RwFrame* RpMatFXMaterialGetEnvMapFrame(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.frame;
}

RwBool RpMatFXMaterialGetEnvMapFrameBufferAlpha(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.useFrameBufferAlpha;
}

RwReal RpMatFXMaterialGetEnvMapCoefficient(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.coef;
}

// 0x812830 (lote S14A, sin validar; 134 bytes; era una envoltura; cdecl, (material, textura)): en la ranura dual (bandera 4; NULL si no existe y entonces el EXE escribe en 0): textura + 1 referencia (SIN comprobar NULL),
// destruye la anterior (+0), guarda la nueva y llama al relleno 0x816280 con 2 y 1. Devuelve el material.
RpMaterial* RpMatFXMaterialSetDualTexture(RpMaterial* material, RwTexture* texture) {
    MatFXEffectData* const effect = FindEffect(material, rpMATFXEFFECTDUAL);
    ++texture->refCount;
    if (effect->data.dual.texture != nullptr) {
        RwTextureDestroy(effect->data.dual.texture);
        effect->data.dual.texture = nullptr;
    }
    effect->data.dual.texture = texture;
    _rpMatFXD3D9EffectStub(effect, 2);
    _rpMatFXD3D9EffectStub(effect, 1);
    return material;
}

// 0x8128C0 (lote R8A, sin validar; cdecl, 3 argumentos, `ret`, 112 bytes; sustituye la envoltura): guarda las dos funciones de mezcla en la ranura dual (bandera 4; NULL si no existe y entonces el EXE escribe
// en 4 y 8) y llama al relleno 0x816280 con 0xA y 0xB. Devuelve el material.
RpMaterial* RpMatFXMaterialSetDualBlendModes(RpMaterial* material, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode) {
    MatFXEffectData* const dual = FindEffect(material, rpMATFXEFFECTDUAL);
    dual->data.dual.srcBlendMode = srcBlendMode;
    dual->data.dual.dstBlendMode = dstBlendMode;
    _rpMatFXD3D9EffectStub(dual, 0xA);
    _rpMatFXD3D9EffectStub(dual, 0xB);
    return material;
}

// 0x812930 (lote S14A, sin validar; 75 bytes; era una envoltura; cdecl, (material)): textura (+0) de la ranura dual (bandera 4; NULL si no existe y entonces el EXE lee la direccion 0).
RwTexture* RpMatFXMaterialGetDualTexture(const RpMaterial* material) {
    return FindEffect(material, rpMATFXEFFECTDUAL)->data.dual.texture;
}

// 0x812980 (lote S14A, sin validar; 91 bytes; era una envoltura; cdecl, (material, mezcla origen*, mezcla destino*)): escribe +4 y +8 de la ranura dual (NULL si no existe y entonces el EXE lee las direcciones 4 y 8)
// en los dos punteros (sin comprobar NULL). Devuelve el material.
const RpMaterial* RpMatFXMaterialGetDualBlendModes(const RpMaterial* material, RwBlendFunction* srcBlendMode, RwBlendFunction* dstBlendMode) {
    MatFXEffectData* const effect = FindEffect(material, rpMATFXEFFECTDUAL);
    *srcBlendMode = effect->data.dual.srcBlendMode;
    *dstBlendMode = effect->data.dual.dstBlendMode;
    return material;
}

// 0x8129E0 (lote R8A, sin validar; cdecl, 3 argumentos, `ret`, 100 bytes; sustituye la envoltura): guarda las dos matrices UV en la ranura de transformacion UV (bandera 5; NULL si no existe y entonces el EXE
// escribe en 0 y 4). Devuelve el material.
RpMaterial* RpMatFXMaterialSetUVTransformMatrices(RpMaterial* material, RwMatrix* baseTransform, RwMatrix* dualTransform) {
    MatFXEffectData* const uv = FindEffect(material, rpMATFXEFFECTUVTRANSFORM);
    uv->data.uvAnim.baseTransform = baseTransform;
    uv->data.uvAnim.dualTransform = dualTransform;
    return material;
}

// 0x812A50 (lote S14A, sin validar; 98 bytes; era una envoltura; cdecl, (material, matriz base**, matriz dual**)): con cada puntero de salida no NULL escribe la matriz base (+0) y la dual (+4) de la ranura de
// transformacion UV (bandera 5; NULL si no existe y entonces el EXE lee las direcciones 0 y 4 solo si el puntero no es NULL). La firma del cpp tenia un solo nivel de puntero; la del SDK (y la del .h) es RwMatrix**.
// Devuelve el material.
const RpMaterial* RpMatFXMaterialGetUVTransformMatrices(const RpMaterial* material, RwMatrix** baseTransform, RwMatrix** dualTransform) {
    MatFXEffectData* const effect = FindEffect(material, rpMATFXEFFECTUVTRANSFORM);
    if (baseTransform != nullptr) {
        *baseTransform = effect->data.uvAnim.baseTransform;
    }
    if (dualTransform != nullptr) {
        *dualTransform = effect->data.uvAnim.dualTransform;
    }
    return material;
}

// 0x812C90 (lote R8A, sin validar; cdecl, 2 argumentos (cabecera de la entrada de recursos y malla instanciada), `ret`, 166 bytes; llamador: 0x815E01, del callback de render 0x815C20): dibujo por defecto de una malla.
// Si hay un pixel shader activo (0x8E244C) lo quita (SetPixelShader(NULL), +0x1AC); si el vertex shader de la malla (+0x10) no es el ultimo (0x8E2448) lo cambia (+0x170); vacia los estados (0x7FC200) y dibuja:
// con bufer de indices (+8 de la cabecera) DrawIndexedPrimitive(tipo +0xC, baseIndex +0x14, 0, numVertices +0x18, startIndex +0x1C, numPrimitives +0x20; +0x148; corregido en R10A: R8A tenia baseIndex y el 0 intercambiados, el EXE empuja +0x14 antes que el 0), si no DrawPrimitive(tipo, baseIndex, numPrimitives; +0x144).
void _rpMatFXD3D9DrawMesh(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance) {
    if (D3D9LastPixelShader != nullptr) {
        D3D9LastPixelShader = nullptr;
        ComCall<kSlotSetPixelShader, long>(Device(), static_cast<void*>(nullptr));
    }
    if (D3D9LastVertexShader != instance->vertexShader) {
        D3D9LastVertexShader = instance->vertexShader;
        ComCall<kSlotSetVertexShader, long>(Device(), static_cast<void*>(instance->vertexShader));
    }
    if (header->indexBuffer != nullptr) {
        ((void(__cdecl *)())kD3D9FlushRenderStates)();
        ComCall<kSlotDrawIndexedPrimitive, long>(Device(), static_cast<RwUInt32>(header->primType), instance->baseIndex, 0, instance->numVertices, instance->startIndex, instance->numPrimitives);
    } else {
        ((void(__cdecl *)())kD3D9FlushRenderStates)();
        ComCall<kSlotDrawPrimitive, long>(Device(), static_cast<RwUInt32>(header->primType), instance->baseIndex, instance->numPrimitives);
    }
}

// 0x814270 (lote R8A, sin validar; cdecl, 3 argumentos (textura, etapa, marco), `ret`, 490 bytes; llamadores: seis, de los callbacks de render 0x813A10 y 0x814460): activa la textura en la etapa (RwD3D9SetTexture) y
// prepara su matriz de transformacion de coordenadas. Raster cubico (0x4C9F70): TEXTURETRANSFORMFLAGS (0x18) = COUNT3 y TEXCOORDINDEX (0xB) = CAMERASPACEREFLECTIONVECTOR (0x30000); matriz = (-derecha, arriba,
// adelante) de la LTM del marco de la camara actual (filas 0..2; ultima columna 0 y ultima fila (0,0,0,1)) en el estado 0x10 + etapa. Si no: COUNT2 y CAMERASPACENORMAL (0x10000); con marco: inversa de la LTM del marco
// por la LTM de la camara, con la fila derecha negada y pos/pad a cero, por la matriz constante 0x885608, al estado 0x10 + etapa; sin marco: la constante directamente. Devuelve TRUE.
RwBool _rpMatFXD3D9SetEnvMapTexture(RwTexture* texture, RwUInt32 stage, RwFrame* frame) {
    RwD3D9SetTexture(texture, stage);
    if (reinterpret_cast<RwInt32(__cdecl *)(RwRaster*)>(kRasterIsCube)(texture->raster) != 0) {
        RwD3D9SetTextureStageState(stage, 0x18, 3);
        RwD3D9SetTextureStageState(stage, 0xB, 0x30000);
        RwFrame* const cameraFrame = static_cast<RwFrame*>(reinterpret_cast<RwObject*>(RwEngineInstance->curCamera)->parent);
        const RwMatrix* const ltm = RwFrameGetLTM(cameraFrame);
        RwReal m[16];
        m[0] = -ltm->right.x;
        m[1] = -ltm->right.y;
        m[2] = -ltm->right.z;
        memcpy(&m[4], &ltm->up, 3 * sizeof(RwReal));
        memcpy(&m[8], &ltm->at, 3 * sizeof(RwReal));
        m[3] = 0.0f;
        m[7] = 0.0f;
        m[11] = 0.0f;
        m[12] = 0.0f;
        m[13] = 0.0f;
        m[14] = 0.0f;
        m[15] = 1.0f;
        RwD3D9SetTransform(stage + 0x10, m);
        return TRUE;
    }
    RwD3D9SetTextureStageState(stage, 0x18, 2);
    RwD3D9SetTextureStageState(stage, 0xB, 0x10000);
    if (frame != nullptr) {
        RwFrame* const cameraFrame = static_cast<RwFrame*>(reinterpret_cast<RwObject*>(RwEngineInstance->curCamera)->parent);
        const RwMatrix* const cameraLtm = RwFrameGetLTM(cameraFrame);
        const RwMatrix* const frameLtm = RwFrameGetLTM(frame);
        RwMatrix inverse;
        RwMatrixInvert(&inverse, frameLtm);
        RwMatrix combined;
        RwMatrixMultiply(&combined, &inverse, cameraLtm);
        combined.right.x = -combined.right.x;
        combined.right.y = -combined.right.y;
        combined.right.z = -combined.right.z;
        combined.flags = 0;
        combined.pos.x = 0.0f;
        combined.pos.y = 0.0f;
        combined.pos.z = 0.0f;
        RwMatrix result;
        RwMatrixMultiply(&result, &combined, reinterpret_cast<const RwMatrix*>(kEnvMapHalfMatrix));
        RwD3D9SetTransform(stage + 0x10, &result);
        return TRUE;
    }
    RwD3D9SetTransform(stage + 0x10, reinterpret_cast<const void*>(kEnvMapHalfMatrix));
    return TRUE;
}

// 0x8149E0 (lote R8A, sin validar; cdecl, 3 argumentos (cabecera de la entrada de recursos, malla instanciada, destino), `ret`, 4666 bytes; llamador: 0x8144C0, del callback de render 0x814460): instancia los vertices
// de una malla con relieve en el formato 0x2C bytes (FVF 0x252) de destino. escala = invBumpWidth * coef de la ranura 0; marco = el del relieve o, si es NULL, el de la camara actual. Matriz del mundo (D3DTS_WORLD =
// 0x100) invertida; la luz es el primer objeto de tipo 3 del marco: con subtipo 1, 0x81 o 0x82 (direccional, foco) la direccion = -adelante de su LTM transformada como vector por la inversa, en cualquier otro caso (y
// sin luces) la posicion de la LTM del marco transformada como punto; es la `luz` de todos los calculos (el EXE no distingue direccion de posicion). GetDeclaration lee los elementos; se bloquean (0x800) los flujos
// con banderas de geometria & 0xFF6 [OffsetToLock = minVert * paso + desplazamiento, tamano = numVertices * paso]. Se buscan posicion (uso 0), normal (3), color (0xA), texcoord (5) y tangente (6), todos con indice 0
// (normal, color y tangente terminan tambien en el elemento final, tipo 0x11; posicion y texcoord no tienen tope). Lecturas por tipo: FLOAT3 (2) y FLOAT2 (1) se copian como dwords, los demas tipos por 0x7551F0 /
// 0x7555E0. Tres casos: (A) con tangente en el flujo: por vertice, l = normalizar(luz - posicion) con _rwInvSqrt (0x7EDB90) de la suma (y*y + x*x) + z*z; c = T x N; la coordenada de relieve = uv + escala * (T.l,
// c.l), con T.l = (T.y*l.y + T.x*l.x) + T.z*l.z y c.l = (c.y*l.y + c.x*l.x) + c.z*l.z. (B) sin tangente y con bufer de indices (se bloquea con 0x800 [numIndex * 2 bytes desde startIndex * 2]; vector `visited` de numVertices
// bytes con RwMalloc 0x30120): por indice k, si el vertice indices[k] no se ha escrito, toma el triangulo (lista: k, tri + (k + 1) % 3, tri + (k + 2) % 3 con tri = 3 * (k / 3); tira: k con los dos siguientes o anteriores segun la
// paridad; otros tipos conservan los indices anteriores, que valen numIndex al empezar) y calcula T = normalizar(e1 * dv2 - e2 * dv1) con e1 = p1 - p0, e2 = p2 - p0, dv = v - v0, c = T x N (N = normal del vertice i0, o 0) y
// la coordenada de relieve = uv - escala * (T.l, c.l); marca visited[i0]. (C) sin tangente ni indices: igual pero recorre los numVertices vertices (i0 = k). Despues desbloquea el bufer de indices y libera `visited` (solo en B)
// y los flujos bloqueados. Devuelve TRUE.
RwBool _rpMatFXD3D9BumpMapInstance(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, void* destination) {
    rpMatFXMaterialData* const matfx = MaterialData(instance->material);
    const RwReal bumpScale = matfx->data[0].data.bumpMap.invBumpWidth * matfx->data[0].data.bumpMap.coef;
    RwFrame* frame = matfx->data[0].data.bumpMap.frame;
    if (frame == nullptr) {
        frame = static_cast<RwFrame*>(reinterpret_cast<RwObject*>(RwEngineInstance->curCamera)->parent);
    }

    RwMatrix worldMatrix;
    RwD3D9GetTransform(0x100, &worldMatrix);
    RwMatrix inverseWorld;
    RwMatrixInvert(&inverseWorld, &worldMatrix);

    bool useDirection = false;
    {
        RwLLLink* const end = &frame->objectList.link;
        for (RwLLLink* link = end->next; link != end; link = link->next) {
            const RwObject* const object = reinterpret_cast<const RwObject*>(reinterpret_cast<const RwUInt8*>(link) - 8);
            if (object->type == 3) {
                useDirection = object->subType == 1 || object->subType == 0x81 || object->subType == 0x82;
                break;
            }
        }
    }
    RwV3d lightVec;
    if (useDirection) {
        const RwMatrix* const ltm = RwFrameGetLTM(frame);
        RwV3d direction;
        direction.x = -ltm->at.x;
        direction.y = -ltm->at.y;
        direction.z = -ltm->at.z;
        RwV3dTransformVector(&lightVec, &direction, &inverseWorld);
    } else {
        const RwMatrix* const ltm = RwFrameGetLTM(frame);
        RwV3dTransformPoint(&lightVec, &ltm->pos, &inverseWorld);
    }

    D3DVERTEXELEMENT9 elements[32];
    UINT numElements = 0;
    header->vertexDeclaration->GetDeclaration(elements, &numElements);

    RwUInt8* locked[2] = { nullptr, nullptr };
    for (RwInt32 stream = 0; stream < 2; ++stream) {
        locked[stream] = nullptr;
        const RxD3D9VertexStream& vs = header->vertexStream[stream];
        if (vs.vertexBuffer != nullptr && (vs.geometryFlags & 0xFF6) != 0) {
            static_cast<IDirect3DVertexBuffer9*>(vs.vertexBuffer)->Lock(instance->minVert * vs.stride + vs.offset, instance->numVertices * vs.stride, reinterpret_cast<void**>(&locked[stream]), 0x800);
        }
    }

    // Busqueda de un elemento de la declaracion por uso (con indice 0); con `stopAtEnd` termina tambien en el elemento final (tipo 0x11).
    auto findElement = [&elements](RwUInt32 usage, bool stopAtEnd) -> const D3DVERTEXELEMENT9* {
        const D3DVERTEXELEMENT9* e = elements;
        for (;; ++e) {
            if (e->Usage == usage && e->UsageIndex == 0) {
                return e;
            }
            if (stopAtEnd && e->Type == 0x11) {
                return e;
            }
        }
    };
    auto streamBase = [&](const D3DVERTEXELEMENT9* e) -> uintptr_t { return reinterpret_cast<uintptr_t>(locked[e->Stream]) + e->Offset; };
    auto streamStride = [&](const D3DVERTEXELEMENT9* e) -> RwUInt32 { return header->vertexStream[e->Stream].stride; };

    const D3DVERTEXELEMENT9* element = findElement(0, false);
    const RwUInt32 posType = element->Type;
    const RwUInt32 posStride = streamStride(element);
    const uintptr_t posBase = streamBase(element);

    uintptr_t normalBase = 0;
    RwUInt32 normalStride = 0;
    RwUInt32 normalType = 0;
    element = findElement(3, true);
    if (element->Type != 0x11) {
        normalType = element->Type;
        normalBase = streamBase(element);
        normalStride = streamStride(element);
    }

    uintptr_t colorBase = 0;
    RwUInt32 colorStride = 0;
    element = findElement(0xA, true);
    if (element->Type != 0x11) {
        colorBase = streamBase(element);
        colorStride = streamStride(element);
    }

    element = findElement(5, false);
    const uintptr_t uvBase = streamBase(element);
    const RwUInt32 uvStride = streamStride(element);
    const RwUInt32 uvType = element->Type;

    uintptr_t tangentBase = 0;
    RwUInt32 tangentStride = 0;
    RwUInt32 tangentType = 0;
    element = findElement(6, true);
    if (element->Type != 0x11) {
        tangentType = element->Type;
        tangentBase = streamBase(element);
        tangentStride = streamStride(element);
    }

    auto readV3 = [](RwUInt32 type, RwV3d& dst, uintptr_t src) {
        if (type == 2) {
            memcpy(&dst, reinterpret_cast<const void*>(src), 3 * sizeof(RwUInt32));
        } else {
            _rpD3D9VertexDeclarationUnInstV3d(type, &dst, reinterpret_cast<const RwUInt8*>(src));
        }
    };
    auto readV2 = [](RwUInt32 type, RwV2d& dst, uintptr_t src) {
        if (type == 1) {
            memcpy(&dst, reinterpret_cast<const void*>(src), 2 * sizeof(RwUInt32));
        } else {
            _rpD3D9VertexDeclarationUnInstV2d(type, &dst, reinterpret_cast<const RwUInt8*>(src));
        }
    };
    BumpVertex* const vertices = static_cast<BumpVertex*>(destination);
    // Normal y color del vertice de salida (comunes a los tres casos): normal copiada como dwords o ceros, color o 0xFFFFFFFF.
    auto writeNormalAndColor = [&](BumpVertex& v, const RwV3d& n, RwUInt32 vertexIndex) {
        if (normalBase != 0) {
            memcpy(v.normal, &n, 3 * sizeof(RwUInt32));
        } else {
            v.normal[0] = 0;
            v.normal[1] = 0;
            v.normal[2] = 0;
        }
        v.color = colorBase != 0 ? *reinterpret_cast<const RwUInt32*>(colorBase + vertexIndex * colorStride) : 0xFFFFFFFFu;
    };

    const RwInt32 numVertices = static_cast<RwInt32>(instance->numVertices);
    if (tangentBase != 0) {
        // (A) la malla trae tangentes.
        for (RwInt32 i = 0; i < numVertices; ++i) {
            RwV3d pos;
            RwV2d uv;
            RwV3d n;
            RwV3d t;
            readV3(posType, pos, posBase + posStride * i);
            readV2(uvType, uv, uvBase + uvStride * i);
            if (normalBase != 0) {
                readV3(normalType, n, normalBase + normalStride * i);
            } else {
                n.x = 0.0f;
                n.y = 0.0f;
                n.z = 0.0f;
            }
            readV3(tangentType, t, tangentBase + tangentStride * i);

            const RwReal d0 = lightVec.x - pos.x;
            const RwReal d1 = lightVec.y - pos.y;
            const RwReal d2 = lightVec.z - pos.z;
            const RwReal r = _rwInvSqrt((d1 * d1 + d0 * d0) + d2 * d2);
            const RwReal l0 = d0 * r;
            const RwReal l1 = r * d1;
            const RwReal l2 = r * d2;
            const RwReal c0 = t.y * n.z - t.z * n.y;
            const RwReal c1 = t.z * n.x - t.x * n.z;
            const RwReal c2 = t.x * n.y - t.y * n.x;
            const RwReal tl = (t.y * l1 + t.x * l0) + t.z * l2;
            const RwReal cl = (c1 * l1 + c0 * l0) + c2 * l2;

            BumpVertex& v = vertices[i];
            memcpy(v.pos, &pos, 3 * sizeof(RwUInt32));
            writeNormalAndColor(v, n, i);
            memcpy(v.uv, &uv, 2 * sizeof(RwUInt32));
            v.bump[0] = tl * bumpScale + uv.x;
            v.bump[1] = cl * bumpScale + uv.y;
        }
    } else {
        // (B) y (C): la tangente sale del triangulo (posiciones y coordenada v de los tres vertices).
        auto computeVertex = [&](RwUInt32 i0, RwUInt32 i1, RwUInt32 i2) {
            RwV3d p0;
            RwV3d p1;
            RwV3d p2;
            readV3(posType, p0, posBase + posStride * i0);
            readV3(posType, p1, posBase + posStride * i1);
            readV3(posType, p2, posBase + posStride * i2);
            RwV2d u0;
            RwV2d u1;
            RwV2d u2;
            readV2(uvType, u0, uvBase + uvStride * i0);
            readV2(uvType, u1, uvBase + uvStride * i1);
            readV2(uvType, u2, uvBase + uvStride * i2);
            RwV3d n;
            if (normalBase != 0) {
                readV3(normalType, n, normalBase + normalStride * i0);
            } else {
                n.x = 0.0f;
                n.y = 0.0f;
                n.z = 0.0f;
            }

            const RwReal d0 = lightVec.x - p0.x;
            const RwReal d1 = lightVec.y - p0.y;
            const RwReal d2 = lightVec.z - p0.z;
            const RwReal r = _rwInvSqrt((d1 * d1 + d0 * d0) + d2 * d2);
            const RwReal l0 = d0 * r;
            const RwReal l1 = d1 * r;
            const RwReal l2 = r * d2;

            const RwReal e1x = p1.x - p0.x;
            const RwReal e1y = p1.y - p0.y;
            const RwReal e1z = p1.z - p0.z;
            const RwReal e2x = p2.x - p0.x;
            const RwReal e2y = p2.y - p0.y;
            const RwReal e2z = p2.z - p0.z;
            const RwReal dv2 = u2.y - u0.y;
            const RwReal dv1 = u1.y - u0.y;
            const RwReal e2xdv1 = e2x * dv1;
            const RwReal e2ydv1 = e2y * dv1;
            const RwReal e2zdv1 = e2z * dv1;
            const RwReal tx0 = e1x * dv2 - e2xdv1;
            const RwReal ty0 = e1y * dv2 - e2ydv1;
            const RwReal tz0 = e1z * dv2 - e2zdv1;
            const RwReal r2 = _rwInvSqrt((ty0 * ty0 + tx0 * tx0) + tz0 * tz0);
            const RwReal tx = tx0 * r2;
            const RwReal ty = ty0 * r2;
            const RwReal tz = tz0 * r2;

            const RwReal c0 = ty * n.z - tz * n.y;
            const RwReal c1 = n.x * tz - tx * n.z;
            const RwReal c2 = tx * n.y - ty * n.x;
            const RwReal tl = (ty * l1 + tx * l0) + tz * l2;
            const RwReal cl = (c1 * l1 + c0 * l0) + c2 * l2;

            BumpVertex& v = vertices[i0];
            memcpy(v.pos, &p0, 3 * sizeof(RwUInt32));
            writeNormalAndColor(v, n, i0);
            memcpy(v.uv, &u0, 2 * sizeof(RwUInt32));
            v.bump[0] = u0.x - tl * bumpScale;
            v.bump[1] = u0.y - cl * bumpScale;
        };
        // Los tres indices del triangulo de la posicion k (lista o tira); otros tipos de primitiva no los tocan.
        const RwUInt32 primType = static_cast<RwUInt32>(header->primType);

        if (header->indexBuffer != nullptr) {
            // (B) con bufer de indices
            RwUInt8* const visited = static_cast<RwUInt8*>(RwMalloc(instance->numVertices, kMatFXMaterialMemHint));
            memset(visited, 0, instance->numVertices);
            const RwInt32 numIndex = static_cast<RwInt32>(instance->numIndex);
            RwUInt16* indices = nullptr;
            header->indexBuffer->Lock(instance->startIndex * 2, instance->numIndex * 2, reinterpret_cast<void**>(&indices), 0x800);
            RwUInt32 i0 = static_cast<RwUInt32>(numIndex);
            RwUInt32 i1 = i0;
            RwUInt32 i2 = i0;
            for (RwInt32 k = 0; k < numIndex; ++k) {
                if (visited[indices[k]] != 0) {
                    continue;
                }
                if (primType == 4) {
                    const RwInt32 tri = 3 * (k / 3);
                    i0 = indices[k];
                    i1 = indices[tri + (k + 1) % 3];
                    i2 = indices[tri + (k + 2) % 3];
                } else if (primType == 5) {
                    RwInt32 a;
                    RwInt32 b;
                    if (k < 2) {
                        if (k % 2 != 0) {
                            a = k + 2;
                            b = k + 1;
                        } else {
                            a = k + 1;
                            b = k + 2;
                        }
                    } else if (k % 2 != 0) {
                        a = k - 1;
                        b = k - 2;
                    } else {
                        a = k - 2;
                        b = k - 1;
                    }
                    i0 = indices[k];
                    i1 = indices[a];
                    i2 = indices[b];
                }
                computeVertex(i0, i1, i2);
                visited[i0] = 1;
            }
            header->indexBuffer->Unlock();
            RwFree(visited);
        } else {
            // (C) sin indices: cada vertice con los dos que le siguen en la lista o la tira.
            RwUInt32 i0 = static_cast<RwUInt32>(numVertices);
            RwUInt32 i1 = i0;
            RwUInt32 i2 = i0;
            for (RwInt32 k = 0; k < numVertices; ++k) {
                if (primType == 4) {
                    const RwInt32 tri = 3 * (k / 3);
                    i0 = k;
                    i1 = tri + (k + 1) % 3;
                    i2 = tri + (k + 2) % 3;
                } else if (primType == 5) {
                    RwInt32 a;
                    RwInt32 b;
                    if (k < 2) {
                        if (k % 2 != 0) {
                            a = k + 2;
                            b = k + 1;
                        } else {
                            a = k + 1;
                            b = k + 2;
                        }
                    } else if (k % 2 != 0) {
                        a = k - 1;
                        b = k - 2;
                    } else {
                        a = k - 2;
                        b = k - 1;
                    }
                    i0 = k;
                    i1 = a;
                    i2 = b;
                }
                computeVertex(i0, i1, i2);
            }
        }
    }

    for (RwInt32 stream = 0; stream < 2; ++stream) {
        if (locked[stream] != nullptr) {
            static_cast<IDirect3DVertexBuffer9*>(header->vertexStream[stream].vertexBuffer)->Unlock();
        }
    }
    return TRUE;
}

// 0x815E60 (lote R8A, sin validar; cdecl sin argumentos, `ret`, 566 bytes; llamador: el ctor del plugin de motor 0x810BF0): crea los pipelines de MatFX y lee las capacidades del dispositivo. Para atomico y sector:
// RxPipelineCreate (0x8057B0), pluginId (+0x2C) = 0x120, RxPipelineLock (si devuelve NULL destruye el pipeline y deja NULL), RxLockedPipeAddFragment(bloqueado, NULL, nodo AllInOne D3D9, NULL) con su Unlock, el
// pipeline a 0xC9AB8C / 0xC9AB90; busca el nodo por el nombre de la definicion, guarda su callback de instanciado en 0xC9ABC4 / 0xC9ABC8 y pone 0x8160A0 / 0x816120 como instanciado y 0x815C20 como render.
// Despues D3DCAPS9: MaxTextureBlendStages (+0x94) a 0xC9ABBC; con >= 2: 0xC9ABAC = MaxSimultaneousTextures (+0x98) >= 2, 0xC9ABB0 = TextureOpCaps (+0x90) & 0x1000000, 0xC9ABB4 = TextureOpCaps & 0x10; si no: esos tres, 0xC9ABB8,
// 0x8E29C0 y 0x8E29C4 a 0. Con PixelShaderVersion (+0xCC) & 0xFFFF >= 0x101: 0xC9ABC0 = 1 y seis RwD3D9CreatePixelShader(0x8852F0, 0x885350, 0x8853A0, 0x8853E0, 0x885468, 0x885518 -> 0xC9AB94..0xC9ABA8);
// si no: 0xC9ABC0 y los seis a 0. Devuelve TRUE siempre.
RwBool _rpMatFXPipelinesCreate() {
    RxPipeline* atomicPipeline = RxPipelineCreate();
    if (atomicPipeline != nullptr) {
        atomicPipeline->pluginId = kMatFXPluginID;
        RxLockedPipe* const locked = RxPipelineLock(atomicPipeline);
        if (locked != nullptr) {
            RxLockedPipeUnlock(RxLockedPipeAddFragment(locked, nullptr, RxNodeDefinitionGetD3D9AtomicAllInOne(), nullptr));
        } else {
            _rxPipelineDestroy(atomicPipeline);
            atomicPipeline = nullptr;
        }
    }
    MatFXAtomicPipeline = atomicPipeline;
    RxPipelineNode* node = RxPipelineFindNodeByName(MatFXAtomicPipeline, RxNodeDefinitionGetD3D9AtomicAllInOne()->name, nullptr, nullptr);
    MatFXAtomicInstanceCB = RxD3D9AllInOneGetInstanceCallBack(node);
    RxD3D9AllInOneSetInstanceCallBack(node, &_rpMatFXAtomicInstanceCallBack);
    RxD3D9AllInOneSetRenderCallBack(node, &_rpMatFXD3D9AllInOneRender);

    RxPipeline* sectorPipeline = RxPipelineCreate();
    if (sectorPipeline != nullptr) {
        sectorPipeline->pluginId = kMatFXPluginID;
        RxLockedPipe* const locked = RxPipelineLock(sectorPipeline);
        if (locked != nullptr) {
            RxLockedPipeUnlock(RxLockedPipeAddFragment(locked, nullptr, RxNodeDefinitionGetD3D9WorldSectorAllInOne(), nullptr));
        } else {
            _rxPipelineDestroy(sectorPipeline);
            sectorPipeline = nullptr;
        }
    }
    MatFXWorldSectorPipeline = sectorPipeline;
    node = RxPipelineFindNodeByName(MatFXWorldSectorPipeline, RxNodeDefinitionGetD3D9WorldSectorAllInOne()->name, nullptr, nullptr);
    MatFXSectorInstanceCB = RxD3D9AllInOneGetInstanceCallBack(node);
    RxD3D9AllInOneSetInstanceCallBack(node, &_rpMatFXWorldSectorInstanceCallBack);
    RxD3D9AllInOneSetRenderCallBack(node, &_rpMatFXD3D9AllInOneRender);

    const D3DCAPS9* const caps = static_cast<const D3DCAPS9*>(RwD3D9GetCaps());
    MatFXMaxBlendStages = static_cast<RwInt32>(caps->MaxTextureBlendStages);
    if (caps->MaxTextureBlendStages >= 2) {
        MatFXMultiTexture = caps->MaxSimultaneousTextures >= 2 ? 1 : 0;
        MatFXMultiplyAdd = static_cast<RwInt32>(caps->TextureOpCaps & 0x1000000);
        MatFXModulate2X = static_cast<RwInt32>(caps->TextureOpCaps & 0x10);
    } else {
        MatFXMultiTexture = 0;
        MatFXMultiplyAdd = 0;
        MatFXModulate2X = 0;
        MatFXUnknownB8 = 0;
        MatFXUnknown29C0 = 0;
        MatFXUnknown29C4 = 0;
    }
    if ((caps->PixelShaderVersion & 0xFFFF) >= 0x101) {
        MatFXPixelShadersOk = 1;
        RwD3D9CreatePixelShader(reinterpret_cast<const RwUInt32*>(0x8852F0), reinterpret_cast<void**>(0xC9AB94));
        RwD3D9CreatePixelShader(reinterpret_cast<const RwUInt32*>(0x885350), reinterpret_cast<void**>(0xC9AB98));
        RwD3D9CreatePixelShader(reinterpret_cast<const RwUInt32*>(0x8853A0), reinterpret_cast<void**>(0xC9AB9C));
        RwD3D9CreatePixelShader(reinterpret_cast<const RwUInt32*>(0x8853E0), reinterpret_cast<void**>(0xC9ABA0));
        RwD3D9CreatePixelShader(reinterpret_cast<const RwUInt32*>(0x885468), reinterpret_cast<void**>(0xC9ABA4));
        RwD3D9CreatePixelShader(reinterpret_cast<const RwUInt32*>(0x885518), reinterpret_cast<void**>(0xC9ABA8));
        return TRUE;
    }
    MatFXPixelShadersOk = 0;
    for (const uintptr_t slot : { 0xC9AB94u, 0xC9AB98u, 0xC9AB9Cu, 0xC9ABA0u, 0xC9ABA4u, 0xC9ABA8u }) {
        *reinterpret_cast<RwInt32*>(slot) = 0;
    }
    return TRUE;
}

// 0x816260 (lote R8A, sin validar; cdecl, 1 argumento, `ret`, 14 bytes): pone el pipeline de atomico de MatFX (0xC9AB8C) en atomic + 0x6C y devuelve el atomico.
RpAtomic* _rpMatFXAtomicPipelineSet(RpAtomic* atomic) {
    atomic->pipeline = MatFXAtomicPipeline;
    return atomic;
}

// 0x816270 (lote R8A, sin validar; cdecl, 1 argumento, `ret`, 14 bytes): pone el pipeline de sector de MatFX (0xC9AB90) en sector + 0x7C y devuelve el sector.
RpWorldSector* _rpMatFXWorldSectorPipelineSet(RpWorldSector* sector) {
    sector->pipeline = MatFXWorldSectorPipeline;
    return sector;
}

// 0x816280 (lote R8A, sin validar; cdecl, 2 argumentos que ignora, `ret`, 6 bytes): `mov eax, 1; ret`. Relleno del SDK (preparacion de los pases del efecto en D3D9); 10 llamadores del EXE.
RwBool _rpMatFXD3D9EffectStub(void* effect, RwInt32 pass) {
    return TRUE;
}

// 0x816290 (lote R8A, sin validar; cdecl, 2 argumentos (textura del material, textura de relieve), `ret`, 94 bytes; llamador: 0x8122C4, de RpMatFXMaterialSetBumpMapTexture): sin textura de relieve devuelve
// la del material; si alguno de los rasters (el del material solo si existe) esta comprimido (RwD3D9RasterIsCompressed, 0x820C90) devuelve NULL; si no, crea la textura combinada con 0x810830 (_rpMatFXD3D9BumpedTextureCombine, lote R10A).
RwTexture* _rpMatFXD3D9BumpedTextureCreate(RwTexture* texture, RwTexture* bumpTexture) {
    if (bumpTexture == nullptr) {
        return texture;
    }
    if (texture != nullptr) {
        if (RwD3D9RasterIsCompressed(texture->raster) != 0) {
            return nullptr;
        }
        if (RwD3D9RasterIsCompressed(bumpTexture->raster) != 0) {
            return nullptr;
        }
    } else if (RwD3D9RasterIsCompressed(bumpTexture->raster) != 0) {
        return nullptr;
    }
    return _rpMatFXD3D9BumpedTextureCombine(texture, bumpTexture);
}

// 0x8162F0 (lote S14A, sin validar; 25 bytes; era una envoltura; cdecl, (indice)): 1 = pipeline de atomico (0xC9AB8C), 2 = pipeline de sector (0xC9AB90); otro valor: NULL.
RxPipeline* RpMatFXGetD3D9Pipeline(RpMatFXD3D9Pipeline d3d9Pipeline) {
    switch (static_cast<RwInt32>(d3d9Pipeline)) {
    case rpMATFXD3D9ATOMICPIPELINE:
        return MatFXAtomicPipeline;
    case rpMATFXD3D9WORLDSECTORPIPELINE:
        return MatFXWorldSectorPipeline;
    default:
        return nullptr;
    }
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lote R10A (sin validar): lo que quedaba de RpMatFX en 0x80FFE0-0x81F0C0 (combinacion de texturas del relieve, callbacks de render de D3D9 y destruccion de los pipelines). Cada cuerpo sigue el desensamblado instruccion por instruccion;
// los almacenamientos de la pila que el EXE reutiliza (ranuras de los argumentos como variables locales) se han pasado a variables locales. Constantes de los estados de etapa (D3DTSS): 1 COLOROP, 2 COLORARG1, 3 COLORARG2, 4 ALPHAOP,
// 5 ALPHAARG1, 6 ALPHAARG2, 0xB TEXCOORDINDEX, 0x18 TEXTURETRANSFORMFLAGS, 0x19 COLORARG0, 0x1A... (valores D3DTOP: 1 DISABLE, 2 SELECTARG1, 3 SELECTARG2, 4 MODULATE, 5 MODULATE2X, 7 ADD, 0xD BLENDCURRENTALPHA...). Estados de render (D3DRS): 0xE ZWRITEENABLE,
// 0xF ALPHATESTENABLE, 0x1B ALPHABLENDENABLE, 0x1C FOGENABLE, 0x22 FOGCOLOR, 0x3C TEXTUREFACTOR, 0x89 LIGHTING.
// ---------------------------------------------------------------------------------------------------------------------------------------------

// 0x810830 (lote R10A, sin validar; cdecl, 2 argumentos (textura del material, que puede ser NULL, y textura de relieve), `ret`, 614 bytes; llamador: 0x8162DA, de _rpMatFXD3D9BumpedTextureCreate): combina la textura del material con el mapa de
// relieve en una textura nueva (la usan los efectos de relieve, donde el relieve va en el alfa). Imagen de relieve = RwImageCreate(ancho, alto del raster de relieve, 32) + AllocatePixels + SetFromRaster. Imagen base = lo mismo con el raster del material
// o, sin textura, una imagen blanca (0xFFFFFFFF) del tamano del relieve. Si los tamanos difieren, el relieve se remuestrea (RwImageResample) a una imagen nueva del tamano de la base y se destruye el original; esa imagen (o el relieve) es la mascara:
// RwImageMakeMask(mascara), RwImageApplyMask(base, mascara). Formato de raster = RwImageFindRasterFormat(base, 4 = textura, &ancho, &alto, &profundidad, &formato); si el raster del material (o el de relieve sin material) tiene la bandera 0x80 en su
// formato (cFormat), formato |= 0x9000 (mipmaps y generacion automatica). Raster = RwRasterCreate(ancho, alto, profundidad, formato) + RwRasterSetFromImage(raster, base); textura = RwTextureCreate(raster). Filtrado y direccionamiento de la nueva
// textura (+0x50, de la del material o, sin ella, la de relieve): byte bajo copiado; direccion U (bits 8-11) y V (12-15) = la comun si coinciden, 0 si no, repetida en ambos nibbles; se conservan los 16 bits altos de la nueva. Destruye la base y la mascara.
// Nombre: intercala caracter a caracter los nombres de la textura del material (o el del relieve dos veces si no hay material) y el del relieve, hasta que se acaban los dos o se han copiado 30 o mas caracteres (la vuelta termina de copiar los de
// los dos); buffer de 32 bytes puestos a cero; RwTextureSetName. Devuelve la textura nueva.
RwTexture* _rpMatFXD3D9BumpedTextureCombine(RwTexture* texture, RwTexture* bumpTexture) {
    RwRaster* const bumpRaster = bumpTexture->raster;
    RwInt32 bumpWidth  = bumpRaster->width;
    RwInt32 bumpHeight = bumpRaster->height;
    RwImage* const bumpImage = RwImageCreate(bumpWidth, bumpHeight, 32);
    RwImageAllocatePixels(bumpImage);
    RwImageSetFromRaster(bumpImage, bumpRaster);

    RwRaster* baseRaster = nullptr;
    RwImage*  baseImage;
    RwInt32   baseWidth;
    RwInt32   baseHeight;
    if (texture != nullptr) {
        baseRaster = texture->raster;
        baseHeight = baseRaster->height;
        baseWidth  = baseRaster->width;
        baseImage  = RwImageCreate(baseWidth, baseHeight, 32);
        RwImageAllocatePixels(baseImage);
        RwImageSetFromRaster(baseImage, baseRaster);
    } else {
        baseHeight = bumpRaster->height;
        baseWidth  = bumpRaster->width;
        baseImage  = RwImageCreate(baseWidth, baseHeight, 32);
        RwImageAllocatePixels(baseImage);
        for (RwInt32 y = 0; y < baseHeight; ++y) {
            if (baseWidth > 0) {
                RwInt32 x = 0;
                do {
                    *reinterpret_cast<RwUInt32*>(baseImage->cpPixels + baseImage->stride * y + x * 4) = 0xFFFFFFFFu;
                    ++x;
                } while (x < baseWidth);
            }
        }
    }

    RwImage* maskImage;
    if (baseWidth != bumpWidth || baseHeight != bumpHeight) {
        maskImage = RwImageCreate(baseWidth, baseHeight, 32);
        RwImageAllocatePixels(maskImage);
        RwImageResample(maskImage, bumpImage);
        RwImageDestroy(bumpImage);
    } else {
        maskImage = bumpImage;
    }
    RwImageMakeMask(maskImage);
    RwImageApplyMask(baseImage, maskImage);

    RwInt32 rasterWidth;
    RwInt32 rasterHeight;
    RwInt32 rasterDepth;
    RwInt32 rasterFormat;
    RwImageFindRasterFormat(baseImage, 4, &rasterWidth, &rasterHeight, &rasterDepth, &rasterFormat);
    const RwUInt8 sourceFormat = (texture != nullptr) ? baseRaster->cFormat : bumpRaster->cFormat;
    if ((sourceFormat & 0x80) != 0) {
        rasterFormat |= 0x9000;
    }
    RwRaster* const raster = RwRasterCreate(rasterWidth, rasterHeight, rasterDepth, rasterFormat);
    RwRasterSetFromImage(raster, baseImage);
    RwTexture* const result = RwTextureCreate(raster);

    const RwTexture* const filterSource = (texture != nullptr) ? texture : bumpTexture;
    const RwUInt32 sourceFilter = filterSource->filterAddressing;
    const RwUInt32 addressU = (sourceFilter >> 8) & 0xF;
    const RwUInt32 addressV = (sourceFilter >> 12) & 0xF;
    const RwUInt32 address  = (addressU == addressV) ? addressV : 0;
    const RwUInt32 nibbles  = address | (address << 4);
    result->filterAddressing = (result->filterAddressing & ~0xFF00u) | (nibbles << 8);
    *reinterpret_cast<RwUInt8*>(&result->filterAddressing) = static_cast<RwUInt8>(sourceFilter & 0xFF);

    RwImageDestroy(baseImage);
    RwImageDestroy(maskImage);

    RwChar name[32] = {};
    const RwChar* sources[2] = { (texture != nullptr) ? texture->name : bumpTexture->name, bumpTexture->name };
    RwChar* out = name;
    RwInt32 copied = 0;
    while (*sources[0] != 0 || *sources[1] != 0) {
        for (RwInt32 i = 0; i < 2; ++i) {
            const RwChar c = *sources[i];
            if (c != 0) {
                *out++ = c;
                ++sources[i];
                ++copied;
            }
        }
        if (copied >= 0x1E) {
            break;
        }
    }
    *out = 0;
    RwTextureSetName(result, name);
    return result;
}

// 0x812AC0 (lote R10A, sin validar; cdecl, 4 argumentos (cabecera de la entrada de recursos, malla instanciada, banderas de la geometria (solo el byte bajo), textura), `ret`, 454 bytes; llamadores: 0x813A02, 0x814260, 0x8149C6, 0x815DF5 y los de
// los callbacks de render): dibujo de una malla con la etapa 0 y, si el material tiene la transformacion UV sola (efecto 5 = rpMATFXEFFECTUVTRANSFORM), su matriz base. Con geometria texturada (banderas & 0x84: 0x4 textura, 0x80 dos juegos de UV):
// etapa 0 = textura; si el material tiene datos de MatFX con banderas 5 y matriz base no identidad, aplica la matriz (ApplyUvTransform con la etapa 0 y la transformada 0x10) y recuerda los datos; luego etapa 0: COLOROP MODULATE, COLORARG1 TEXTURE (2),
// COLORARG2 DIFFUSE (0), ALPHAOP MODULATE, ALPHAARG1 TEXTURE, ALPHAARG2 DIFFUSE. Sin textura: textura NULL en la etapa 0 y COLOROP = SELECTARG2 (3), COLORARG2 = 0, ALPHAOP = 3, ALPHAARG2 = 0. Despues quita el pixel shader activo (0x8E244C), cambia el
// vertex shader si difiere (0x8E2448) y dibuja (vaciado de estados 0x7FC200, DrawIndexedPrimitive o DrawPrimitive). Si recordo datos de UV, deja TEXTURETRANSFORMFLAGS de la etapa 0 a 0.
void _rpMatFXD3D9DrawMeshUVTransform(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwTexture* texture) {
    rpMatFXMaterialData* uvData = nullptr;
    if ((flags & 0x84) != 0) {
        RwD3D9SetTexture(texture, 0);
        rpMatFXMaterialData* const data = MaterialData(instance->material);
        if (data != nullptr && static_cast<RwUInt32>(data->flags) == 5) {
            uvData = data;
            const RwMatrix* const baseTransform = data->data[0].data.uvAnim.baseTransform;
            ApplyUvTransform(baseTransform, 0, 0x10);
        }
        Tss(0, 1, 4);
        Tss(0, 2, 2);
        Tss(0, 3, 0);
        Tss(0, 4, 4);
        Tss(0, 5, 2);
        Tss(0, 6, 0);
    } else {
        RwD3D9SetTexture(nullptr, 0);
        Tss(0, 1, 3);
        Tss(0, 3, 0);
        Tss(0, 4, 3);
        Tss(0, 6, 0);
    }
    ResetPixelShader();
    SelectVertexShader(instance);
    DrawBatch(header, instance, instance->baseIndex);
    if (uvData != nullptr) {
        Tss(0, 0x18, 0);
    }
}

// 0x812D40 (lote R10A, sin validar; cdecl, 5 argumentos (cabecera, malla instanciada, banderas de la geometria (byte bajo), textura base, textura dual), `ret`, 3279 bytes; llamadores: 0x815DD1 y 0x815DE5, del callback de render 0x815C20 con los efectos
// 4 = DUAL y 6 = DUALUVTRANSFORM; la ranura dual es la 0 del 4 y la 1 del 6): dibujo con textura dual. Sin textura dual (o sin raster) llama a _rpMatFXD3D9DrawMeshUVTransform(cabecera, malla, banderas, base) y sale. Si no, `efecto` = ranura dual (src +4 y dst +8 de la
// mezcla); con efecto 6 se recuerdan los datos (la matriz base esta en la ranura 0). Etapa 0 como en 0x812AC0 (con o sin geometria texturada; con efecto 6 aplica la matriz base de la ranura 0 con la transformada 0x10). Si hay textura base y los dos rasters tienen la
// bandera 0x20 del formato y las texturas son distintas, convierte el raster de la textura dual (0x4CD250). Sin multitextura (0xC9ABAC) va directo a dibujar. Segun el par de mezcla (src, dst): (DESTCOLOR 9, ZERO 1) o (ZERO 1, SRCCOLOR 3) = multiplicar; (SRCALPHA 5,
// INVSRCALPHA 6) = mezcla de alfa (con base exige al menos 3 etapas, MaxTextureBlendStages 0xC9ABBC); (9, 3) = multiplicar x2 (exige TextureOpCaps & MODULATE2X 0xC9ABB4 y se valida con ValidateDevice la primera vez, 0x8E29C0; si falla apaga la capacidad y deja
// la segunda pasada); (ONE 2, ONE 2) = sumar (exige MULTIPLYADD 0xC9ABB0); (1, SRCALPHA... 5) = efecto con COLORARG2 = 0x22; otro par = segunda pasada. Si el modo se resuelve en una sola pasada pone `segundaPasada` a 0 y termina la configuracion (TEXCOORDINDEX de la
// etapa 1 = 0 si la geometria no tiene dos juegos de UV y matriz dual de la ranura 0 con la transformada 0x11 si hay datos). Despues quita el pixel shader, cambia el vertex shader y dibuja. Si quedo una sola pasada restaura las etapas 0, 1 (textura NULL) y 2 y, sin
// dos juegos de UV, TEXCOORDINDEX de la etapa 1 = 1. Si no, segunda pasada: etapa 0 = textura dual (con la matriz dual de la ranura 0 si hay datos), alfa por vertice activado si no lo estaba, ALPHATESTENABLE = 0 en los modos de multiplicar, mezcla del motor = la del efecto
// (se guardan SRCBLEND y DESTBLEND), ZWRITEENABLE = 0 y, si hay niebla, FOGCOLOR = 0 (dst ONE) o 0xFFFFFFFF (src DESTCOLOR o dst SRCCOLOR); TEXCOORDINDEX 1 en la etapa 0 si la geometria tiene dos juegos de UV; dibuja y restaura todo. Al salir, si habia datos de UV,
// TEXTURETRANSFORMFLAGS de las etapas 0 y 1 = 0.
void _rpMatFXD3D9RenderDualTexture(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwTexture* baseTexture, RwTexture* dualTexture) {
    if (dualTexture == nullptr || dualTexture->raster == nullptr) {
        _rpMatFXD3D9DrawMeshUVTransform(header, instance, flags, baseTexture);
        return;
    }
    rpMatFXMaterialData* const data = MaterialData(instance->material);
    RwInt32 secondPass = 1;
    rpMatFXMaterialData* uvData = nullptr;
    MatFXEffectData* effect;
    if (static_cast<RwUInt32>(data->flags) == 6) {
        effect = &data->data[1];
        uvData = data;
    } else {
        effect = &data->data[0];
    }
    const RwUInt32 blendSrc = static_cast<RwUInt32>(effect->data.dual.srcBlendMode);
    const RwUInt32 blendDst = static_cast<RwUInt32>(effect->data.dual.dstBlendMode);

    if ((flags & 0x84) != 0) {
        RwD3D9SetTexture(baseTexture, 0);
        Tss(0, 1, 4);
        Tss(0, 2, 2);
        Tss(0, 3, 0);
        Tss(0, 4, 4);
        Tss(0, 5, 2);
        Tss(0, 6, 0);
    } else {
        RwD3D9SetTexture(nullptr, 0);
        Tss(0, 1, 3);
        Tss(0, 3, 0);
        Tss(0, 4, 3);
        Tss(0, 6, 0);
    }
    if (uvData != nullptr) {
        ApplyUvTransform(uvData->data[0].data.uvAnim.baseTransform, 0, 0x10);
    }
    if (baseTexture != nullptr && (baseTexture->raster->cFormat & 0x20) != 0 && (dualTexture->raster->cFormat & 0x20) != 0 && baseTexture != dualTexture) {
        ConvertRasterToDisplayFormatExe(dualTexture->raster);
    }

    // 0x8133C8: fin de la configuracion de multitextura.
    const auto finishMultiTexture = [&]() {
        if ((flags & 0x80) == 0) {
            Tss(1, 0xB, 0);
        }
        if (uvData != nullptr) {
            ApplyUvTransform(uvData->data[0].data.uvAnim.dualTransform, 1, 0x11);
        }
    };

    do {
        if (MatFXMultiTexture == 0) {
            break;
        }
        if ((blendSrc == 9 && blendDst == 1) || (blendSrc == 1 && blendDst == 3)) {
            if (baseTexture != nullptr) {
                RwD3D9SetTexture(dualTexture, 1);
                if (MatFXMaxBlendStages == 3) {
                    Tss(0, 1, 2);
                    Tss(0, 2, 2);
                    Tss(0, 4, 2);
                    Tss(0, 5, 2);
                    Tss(1, 1, 4);
                    Tss(1, 2, 2);
                    Tss(1, 3, 1);
                    Tss(1, 4, 3);
                    Tss(1, 6, 1);
                    Tss(2, 1, 4);
                    Tss(2, 2, 0);
                    Tss(2, 3, 1);
                    Tss(2, 4, 4);
                    Tss(2, 5, 0);
                    Tss(2, 6, 1);
                } else {
                    Tss(1, 1, 4);
                    Tss(1, 2, 2);
                    Tss(1, 3, 1);
                    Tss(1, 4, 3);
                    Tss(1, 6, 1);
                }
            } else {
                RwD3D9SetTexture(dualTexture, 0);
                Tss(0, 1, 4);
                Tss(0, 2, 2);
                Tss(0, 3, 0);
                Tss(0, 4, 4);
                Tss(0, 5, 2);
                Tss(0, 6, 0);
            }
            secondPass = 0;
            finishMultiTexture();
            break;
        }
        if (blendSrc == 5 && blendDst == 6) {
            if (baseTexture != nullptr) {
                if (MatFXMaxBlendStages < 3) {
                    break;
                }
                RwD3D9SetTexture(dualTexture, 1);
                Tss(0, 1, 2);
                Tss(0, 2, 2);
                Tss(0, 4, 2);
                Tss(0, 5, 2);
                Tss(1, 1, 0xD);
                Tss(1, 2, 2);
                Tss(1, 3, 1);
                Tss(1, 4, 0xD);
                Tss(1, 5, 2);
                Tss(1, 6, 1);
                Tss(2, 1, 4);
                Tss(2, 2, 0);
                Tss(2, 3, 1);
                Tss(2, 4, 4);
                Tss(2, 5, 0);
                Tss(2, 6, 1);
            } else {
                SetRs(0x3C, 0xFFFFFFFFu);
                RwD3D9SetTexture(dualTexture, 0);
                Tss(0, 1, 0xD);
                Tss(0, 2, 2);
                Tss(0, 3, 3);
                Tss(0, 1, 0xD);
                Tss(0, 2, 2);
                Tss(0, 3, 3);
                Tss(1, 1, 4);
                Tss(1, 2, 0);
                Tss(1, 3, 1);
                Tss(1, 4, 4);
                Tss(1, 5, 0);
                Tss(1, 6, 1);
            }
            secondPass = 0;
            finishMultiTexture();
            break;
        }
        if (blendSrc == 9 && blendDst == 3) {
            if (MatFXModulate2X == 0) {
                break;
            }
            if (baseTexture != nullptr) {
                RwD3D9SetTexture(dualTexture, 1);
                Tss(1, 1, 5);
                Tss(1, 2, 2);
                Tss(1, 3, 1);
                if (MatFXMaxBlendStages == 3) {
                    Tss(0, 1, 2);
                    Tss(0, 2, 2);
                    Tss(0, 4, 2);
                    Tss(0, 5, 2);
                    Tss(1, 4, 3);
                    Tss(1, 6, 1);
                    Tss(2, 1, 4);
                    Tss(2, 2, 0);
                    Tss(2, 3, 1);
                    Tss(2, 4, 4);
                    Tss(2, 5, 0);
                    Tss(2, 6, 1);
                } else {
                    Tss(1, 4, 3);
                    Tss(1, 6, 1);
                }
            } else {
                RwD3D9SetTexture(dualTexture, 0);
                Tss(0, 1, 5);
                Tss(0, 4, 3);
                Tss(0, 6, 0);
            }
            if (MatFXUnknown29C0 == 0) {
                secondPass = 0;
            } else {
                RwUInt32 numPasses = 0;
                reinterpret_cast<void(__cdecl *)()>(kD3D9FlushRenderStates)();
                const long hr = ComCall<kSlotValidateDevice, long>(Device(), &numPasses);
                if (hr >= 0 && numPasses == 1) {
                    secondPass = 0;
                } else {
                    MatFXModulate2X = 0;
                    RwD3D9SetTexture(baseTexture, 0);
                    Tss(0, 1, 4);
                    Tss(0, 2, 2);
                    Tss(0, 3, 0);
                    Tss(0, 4, 4);
                    Tss(0, 5, 2);
                    Tss(0, 6, 0);
                    Tss(1, 1, 1);
                    Tss(1, 4, 1);
                    Tss(2, 1, 1);
                    Tss(2, 4, 1);
                    RwD3D9SetTexture(nullptr, 1);
                }
                MatFXUnknown29C0 = 0;
                if (secondPass != 0) {
                    break;
                }
            }
            finishMultiTexture();
            break;
        }
        if (blendSrc == 2 && blendDst == 2) {
            if (baseTexture != nullptr) {
                if (MatFXMultiplyAdd == 0) {
                    break;
                }
                RwD3D9SetTexture(dualTexture, 1);
                Tss(1, 1, 0x19);
                Tss(1, 0x1A, 1);
                Tss(1, 2, 2);
                Tss(1, 3, 0);
                Tss(1, 4, 3);
                Tss(1, 6, 1);
            } else {
                RwD3D9SetTexture(dualTexture, 0);
                Tss(0, 4, 3);
                Tss(0, 6, 0);
                Tss(1, 1, 7);
                Tss(1, 2, 0);
                Tss(1, 3, 1);
                Tss(1, 4, 3);
                Tss(1, 6, 1);
            }
            secondPass = 0;
            finishMultiTexture();
            break;
        }
        if (blendSrc == 1 && blendDst == 5) {
            if (baseTexture != nullptr) {
                RwD3D9SetTexture(dualTexture, 1);
                if (MatFXMaxBlendStages == 3) {
                    Tss(0, 1, 2);
                    Tss(0, 2, 2);
                    Tss(0, 4, 2);
                    Tss(0, 5, 2);
                    Tss(1, 1, 4);
                    Tss(1, 2, 0x22);
                    Tss(1, 3, 1);
                    Tss(1, 4, 3);
                    Tss(1, 6, 1);
                    Tss(2, 1, 4);
                    Tss(2, 2, 0);
                    Tss(2, 3, 1);
                    Tss(2, 4, 4);
                    Tss(2, 5, 0);
                    Tss(2, 6, 1);
                } else {
                    Tss(1, 1, 4);
                    Tss(1, 2, 0x22);
                    Tss(1, 3, 1);
                    Tss(1, 4, 3);
                    Tss(1, 6, 1);
                }
            } else {
                RwD3D9SetTexture(dualTexture, 0);
                Tss(0, 1, 4);
                Tss(0, 2, 0x22);
                Tss(0, 3, 0);
                Tss(0, 4, 3);
                Tss(0, 6, 0);
            }
            secondPass = 0;
            finishMultiTexture();
            break;
        }
    } while (false);

    // 0x813443: primera pasada.
    ResetPixelShader();
    SelectVertexShader(instance);
    DrawBatch(header, instance, instance->baseIndex);

    if (secondPass == 0) {
        // 0x81396C
        Tss(0, 1, 4);
        Tss(0, 2, 2);
        Tss(0, 3, 0);
        RwD3D9SetTexture(nullptr, 1);
        Tss(2, 1, 1);
        Tss(2, 4, 1);
        if ((flags & 0x80) == 0) {
            Tss(1, 0xB, 1);
        }
    } else {
        // 0x81374D: segunda pasada con la textura dual.
        RwD3D9SetTexture(dualTexture, 0);
        if (uvData != nullptr) {
            ApplyUvTransform(uvData->data[0].data.uvAnim.dualTransform, 0, 0x10);
        }
        if (VertexAlphaEnabled() == 0) {
            VertexAlphaEnable(TRUE);
        }
        if ((blendSrc == 9 && blendDst == 1) || (blendSrc == 1 && blendDst == 3) || (blendSrc == 9 && blendDst == 3)) {
            SetRs(0xF, 0);
        }
        const RwUInt32 savedSrcBlend = EngineRenderStateGet(0xA);
        const RwUInt32 savedDstBlend = EngineRenderStateGet(0xB);
        _rwD3D9RenderStateSrcBlend(static_cast<RwInt32>(blendSrc));
        _rwD3D9RenderStateDestBlend(static_cast<RwInt32>(blendDst));
        const RwUInt32 savedZWrite = GetRs(0xE);
        SetRs(0xE, 0);
        const RwUInt32 savedFogEnable = GetRs(0x1C);
        RwUInt32 savedFogColor = 0;
        if (savedFogEnable != 0) {
            savedFogColor = GetRs(0x22);
            if (blendDst == 2) {
                SetRs(0x22, 0);
            } else if (blendSrc == 9 || blendDst == 3) {
                SetRs(0x22, 0xFFFFFFFFu);
            }
        }
        const bool secondUVSet = (flags & 0x80) != 0;
        if (secondUVSet) {
            Tss(0, 0xB, 1);
        }
        DrawBatch(header, instance, instance->baseIndex);
        SetRs(0xE, savedZWrite);
        if (savedFogEnable != 0) {
            SetRs(0x22, savedFogColor);
        }
        if (secondUVSet) {
            Tss(0, 0xB, 0);
        }
        VertexAlphaEnable(FALSE);
        EngineRenderStateSet(0xA, savedSrcBlend);
        EngineRenderStateSet(0xB, savedDstBlend);
    }
    if (uvData != nullptr) {
        Tss(0, 0x18, 0);
        Tss(1, 0x18, 0);
    }
}

// 0x813A10 (lote R10A, sin validar; cdecl, 6 argumentos (cabecera, malla instanciada, banderas de la geometria (byte bajo), pasada, textura base, textura de entorno), `ret`, 2144 bytes; llamadores: 0x815DB8 (pasada 0, efecto 2 = ENVMAP) y 0x8149A6
// (pasada 1, desde 0x814460)): una pasada de mapa de entorno. Intensidad = (int)(coef * 255.0f) & 0xFF con el coef de la ranura `pasada` (el producto se guarda como float y fistp redondea al mas cercano); sin intensidad, sin textura de entorno o sin su raster, la pasada 0
// llama a _rpMatFXD3D9DrawMeshUVTransform(cabecera, malla, banderas, base) y la 1 no hace nada. Cambia el vertex shader si difiere. Pasada distinta de 0: solo la segunda parte (abajo). Pasada 0: con geometria texturada y textura base, etapa 0 = base (`conBase` = 1;
// y si ambos rasters tienen la bandera 0x20 y las texturas difieren convierte el raster de entorno con 0x4CD250), si no etapa 0 = textura NULL. Decide la ruta: sin usar el alfa del bufer (ranura de entorno +0xC = 0) y con alfa de vertice, material con alfa != 255 o
// base con alfa, salta a la ruta de dos pasadas (sin pixel shader); si el entorno tiene alfa y existe el pixel shader 0xC9AB98 usa la ruta de pixel shader (abajo), y si no existe cae en la ruta de dos pasadas; sin alfa en el entorno, `combinada` = 1 si
// MULTIPLYADD (0xC9ABB0) o, con intensidad 255, si no hay base, o 0xC9ABB8 o 0x8E29C4 distintos de 0. Quita el pixel shader activo y configura la etapa 0 (con base: MODULATE; sin base: SELECTARG2). Si no es `combinada`: dibuja la malla (primera pasada) y sigue a la
// segunda parte. Si lo es: con intensidad 255 y sin base, entorno en la etapa 0 (0x814270) con COLOROP = ADD, dibuja y restaura las etapas 0 (textura NULL, TEXTURETRANSFORMFLAGS y TEXCOORDINDEX a 0); si no, entorno en la etapa 1: con intensidad < 255,
// TEXTUREFACTOR (0x3C) = intensidad * 0x01010101 y etapa 1 con COLOROP = 0x19 (LERP), COLORARG0 = TFACTOR (0x1A: 1), COLORARG1 = 2, COLORARG2 = 3; con 255, etapa 1 con ADD (7), COLORARG1 = 2, COLORARG2 = 1 y, si 0x8E29C4 != 0, ValidateDevice para fijar 0xC9ABB8
// (1 si no devuelve error y una pasada; si no 0) y poner 0x8E29C4 = 0; dibuja y restaura la etapa 1 (textura NULL, TEXTURETRANSFORMFLAGS = 0, TEXCOORDINDEX = 1). Ruta de pixel shader: constante 0 del pixel shader = (coef, coef, coef, coef) (SetPixelShaderConstantF); con base,
// entorno en la etapa 1, pixel shader 0xC9AB98, dibuja y restaura la etapa 1; sin base, entorno en la etapa 0, pixel shader 0xC9AB9C y restauracion de la etapa 0. Segunda parte (envio aditivo): entorno en la etapa 0 (con el marco de la ranura), alfa por vertice
// activado si no lo estaba, mezcla del motor SRCALPHA (5) / ONE (2) con las anteriores guardadas, TEXTUREFACTOR = intensidad * 0x01010101 y con intensidad < 255 etapa 0 con COLOROP = MODULATE (4), COLORARG1 = 2, COLORARG2 = 3 (TFACTOR), ALPHAOP = 2, ALPHAARG1 = 2,
// ALPHAARG2 = 3; con 255 SELECTARG1 en color y alfa; LIGHTING (0x89), ZWRITEENABLE (0xE) = 0 y, con niebla, FOGCOLOR = 0; dibuja y restaura todo (con intensidad < 255 COLORARG2 = 0 y ALPHAARG2 = 0, con 255 MODULATE otra vez), alfa por vertice desactivado y mezcla del motor.
void _rpMatFXD3D9RenderEnvMapPass(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwUInt32 pass, RwTexture* baseTexture, RwTexture* envTexture) {
    rpMatFXMaterialData* const data = MaterialData(instance->material);
    MatFXEffectData* const effect = &data->data[pass];
    const RwReal scaled = effect->data.envMap.coef * 255.0f;
    const RwUInt32 intensity = static_cast<RwUInt32>(FloatToIntRounded(scaled)) & 0xFFu;
    if (intensity == 0 || envTexture == nullptr || envTexture->raster == nullptr) {
        if (pass == 0) {
            _rpMatFXD3D9DrawMeshUVTransform(header, instance, flags, baseTexture);
        }
        return;
    }
    RwFrame* const envFrame = effect->data.envMap.frame;
    RwBool combined = FALSE;
    RwBool hasBase  = FALSE;
    SelectVertexShader(instance);

    if (pass == 0) {
        if ((flags & 0x84) != 0 && baseTexture != nullptr) {
            RwD3D9SetTexture(baseTexture, 0);
            hasBase = TRUE;
            if ((baseTexture->raster->cFormat & 0x20) != 0 && (envTexture->raster->cFormat & 0x20) != 0 && baseTexture != envTexture) {
                ConvertRasterToDisplayFormatExe(envTexture->raster);
            }
        } else {
            RwD3D9SetTexture(nullptr, 0);
        }

        bool useTwoPasses = false;  // 0x813B75 directo
        bool usePixelShader = false;
        do {
            if (effect->data.envMap.useFrameBufferAlpha == 0) {
                if (instance->vertexAlpha != 0) {
                    useTwoPasses = true;
                    break;
                }
                if (instance->material->color.alpha != 0xFF) {
                    useTwoPasses = true;
                    break;
                }
                if (hasBase && TextureHasAlpha(baseTexture) != 0) {
                    useTwoPasses = true;
                    break;
                }
            }
            if (TextureHasAlpha(envTexture) != 0) {
                if (MatFXPixelShader(1) == nullptr) {
                    useTwoPasses = true;
                } else {
                    usePixelShader = true;
                }
                break;
            }
            if (MatFXMultiplyAdd != 0) {
                combined = TRUE;
                break;
            }
            if (intensity != 0xFF) {
                break;
            }
            if (!hasBase || MatFXUnknownB8 != 0 || MatFXUnknown29C4 != 0) {
                combined = TRUE;
            }
        } while (false);
        (void)useTwoPasses;

        if (usePixelShader) {
            const RwReal coef = effect->data.envMap.coef;
            const RwReal constants[4] = { coef, coef, coef, coef };
            ComCall<kSlotSetPixelShaderConstantF, long>(Device(), 0u, static_cast<const void*>(constants), 1u);
            if (hasBase) {
                _rpMatFXD3D9SetEnvMapTexture(envTexture, 1, envFrame);
                SelectPixelShader(MatFXPixelShader(1));
                DrawBatch(header, instance, instance->baseIndex);
                RwD3D9SetTexture(nullptr, 1);
                Tss(1, 0x18, 0);
                Tss(1, 0xB, 1);
            } else {
                _rpMatFXD3D9SetEnvMapTexture(envTexture, 0, envFrame);
                SelectPixelShader(MatFXPixelShader(2));
                DrawBatch(header, instance, instance->baseIndex);
                RwD3D9SetTexture(nullptr, 0);
                Tss(0, 0x18, 0);
                Tss(0, 0xB, 0);
            }
            return;
        }

        ResetPixelShader();
        if (hasBase) {
            Tss(0, 1, 4);
            Tss(0, 2, 2);
            Tss(0, 3, 0);
            Tss(0, 4, 4);
            Tss(0, 5, 2);
            Tss(0, 6, 0);
        } else {
            Tss(0, 1, 3);
            Tss(0, 3, 0);
            Tss(0, 4, 3);
            Tss(0, 6, 0);
        }

        if (!combined) {
            DrawBatch(header, instance, instance->baseIndex);
        } else if (intensity == 0xFF && !hasBase) {
            _rpMatFXD3D9SetEnvMapTexture(envTexture, 0, envFrame);
            Tss(0, 1, 7);
            DrawBatch(header, instance, instance->baseIndex);
            RwD3D9SetTexture(nullptr, 0);
            Tss(0, 0x18, 0);
            Tss(0, 0xB, 0);
            return;
        } else {
            _rpMatFXD3D9SetEnvMapTexture(envTexture, 1, envFrame);
            if (intensity != 0xFF) {
                SetRs(0x3C, intensity * 0x01010101u);
                Tss(1, 1, 0x19);
                Tss(1, 0x1A, 1);
                Tss(1, 2, 2);
                Tss(1, 3, 3);
            } else {
                Tss(1, 1, 7);
                Tss(1, 2, 2);
                Tss(1, 3, 1);
                if (MatFXUnknown29C4 != 0) {
                    RwUInt32 numPasses = 0;
                    reinterpret_cast<void(__cdecl *)()>(kD3D9FlushRenderStates)();
                    const long hr = ComCall<kSlotValidateDevice, long>(Device(), &numPasses);
                    if (hr >= 0 && numPasses == 1) {
                        MatFXUnknownB8 = 1;
                    } else {
                        MatFXUnknownB8 = 0;
                    }
                    MatFXUnknown29C4 = 0;
                }
            }
            DrawBatch(header, instance, instance->baseIndex);
            RwD3D9SetTexture(nullptr, 1);
            Tss(1, 0x18, 0);
            Tss(1, 0xB, 1);
            return;
        }
    }

    // 0x814003: pasada aditiva del entorno.
    _rpMatFXD3D9SetEnvMapTexture(envTexture, 0, envFrame);
    if (VertexAlphaEnabled() == 0) {
        VertexAlphaEnable(TRUE);
    }
    const RwUInt32 savedSrcBlend = EngineRenderStateGet(0xA);
    const RwUInt32 savedDstBlend = EngineRenderStateGet(0xB);
    _rwD3D9RenderStateSrcBlend(5);
    _rwD3D9RenderStateDestBlend(2);
    const RwUInt32 color = intensity * 0x01010101u;
    if (color < 0xFFFFFFFFu) {
        SetRs(0x3C, color);
        Tss(0, 1, 4);
        Tss(0, 2, 2);
        Tss(0, 3, 3);
        Tss(0, 4, 2);
        Tss(0, 5, 2);
        Tss(0, 6, 3);
    } else {
        Tss(0, 1, 2);
        Tss(0, 2, 2);
        Tss(0, 4, 2);
        Tss(0, 5, 2);
    }
    const RwUInt32 savedLighting = GetRs(0x89);
    const RwUInt32 savedZWrite   = GetRs(0xE);
    const RwUInt32 savedFogEnable = GetRs(0x1C);
    SetRs(0x89, 0);
    SetRs(0xE, 0);
    RwUInt32 savedFogColor = 0;
    if (savedFogEnable != 0) {
        savedFogColor = GetRs(0x22);
        SetRs(0x22, 0);
    }
    DrawBatch(header, instance, instance->baseIndex);
    if (savedFogEnable != 0) {
        SetRs(0x22, savedFogColor);
    }
    SetRs(0xE, savedZWrite);
    SetRs(0x89, savedLighting);
    if (color < 0xFFFFFFFFu) {
        Tss(0, 3, 0);
        Tss(0, 6, 0);
    } else {
        Tss(0, 1, 4);
        Tss(0, 4, 4);
    }
    VertexAlphaEnable(FALSE);
    EngineRenderStateSet(0xA, savedSrcBlend);
    EngineRenderStateSet(0xB, savedDstBlend);
    Tss(0, 0x18, 0);
    Tss(0, 0xB, 0);
}

// 0x814460 (lote R10A, sin validar; cdecl, 6 argumentos (cabecera, malla instanciada, banderas de la geometria (byte bajo), textura base, textura combinada del relieve (MatFXBumpMapData.texture), textura de entorno o NULL), `ret`, 1398 bytes; llamadores: 0x815DA2
// con los efectos 1 = BUMPMAP (entorno NULL) y 3 = BUMPENVMAP (entorno = textura de la ranura 1)): dibujo con relieve. Textura de trabajo = la combinada o, si es NULL, la base si tiene alfa (_rwD3D9TextureHasAlpha); si no hay ninguna llama a _rpMatFXD3D9DrawMeshUVTransform
// (cabecera, malla, banderas, base) y sale. Reserva 0x2C bytes por vertice en el bufer dinamico (RwD3D9DynamicVertexBufferLock; si falla sale sin dibujar), instancia el relieve en el (_rpMatFXD3D9BumpMapInstance, 0x8149E0), lo desbloquea, pone la textura de trabajo en la etapa 0,
// FVF 0x252 (si cambia; invalida la declaracion), flujo 0 = el bufer (desplazamiento 0, paso 0x2C) y el vertex shader de la malla. Con pixel shaders (0xC9AB94 con o sin entorno, o 0xC9ABA0 con entorno): alfa por vertice desactivado, textura de trabajo en la etapa 1, guarda
// ALPHABLENDENABLE (0x1B) y ALPHATESTENABLE (0xF) y los pone a 0; con entorno y pixel shader 0xC9ABA0, intensidad del entorno de la ranura 1 (coef * 255 con fistp, byte bajo; 0 la salta): entorno en la etapa 2 (0x814270 con el marco de la ranura 1), pixel shader = 0xC9ABA4 o 0xC9ABA8
// (con el alfa del bufer segun el alfa del entorno) o 0xC9ABA0, constante 0 = (coef x4) y dibuja con la base compartida del bufer; restaura la etapa 2; si no, pixel shader 0xC9AB94 y dibuja; restaura ALPHABLENDENABLE y ALPHATESTENABLE y deja la textura NULL en la etapa 1. Sin pixel shaders (ruta fija):
// etapa 0 MODULATE con TEXTURE y DIFFUSE, quita el pixel shader, alfa por vertice si no estaba, ALPHATESTENABLE = 0, mezcla del motor SRC = INVSRCALPHA (6) y DST = ZERO (1), dibuja; ALPHATESTENABLE = 1, mezcla SRC = SRCALPHA (5) y DST = ONE (2), ZWRITEENABLE = 0, TEXCOORDINDEX de la etapa 0 = 1
// y dibuja otra vez; restaura todo. Despues vuelve a poner los flujos de la entrada de recursos (_rwD3D9SetStreams) y su declaracion de vertices (si cambia; invalida el FVF) y, con entorno y sin pixel shader 0xC9ABA0, llama a _rpMatFXD3D9RenderEnvMapPass(cabecera, malla, banderas, 1, base, entorno).
void _rpMatFXD3D9RenderBumpMap(RxD3D9ResEntryHeader* header, RxD3D9InstanceData* instance, RwUInt32 flags, RwTexture* baseTexture, RwTexture* bumpedTexture, RwTexture* envTexture) {
    RwTexture* texture = bumpedTexture;
    if (texture == nullptr) {
        texture = baseTexture;
        if (texture == nullptr || TextureHasAlpha(texture) == 0) {
            _rpMatFXD3D9DrawMeshUVTransform(header, instance, flags, texture);
            return;
        }
    }

    void* vertexBuffer = nullptr;
    void* vertexData   = nullptr;
    RwUInt32 baseIndex = 0;
    if (reinterpret_cast<RwBool(__cdecl *)(RwUInt32, RwUInt32, void**, void**, RwUInt32*)>(0x7F5B10)(0x2C, instance->numVertices, &vertexBuffer, &vertexData, &baseIndex) == 0) {
        return;
    }
    _rpMatFXD3D9BumpMapInstance(header, instance, vertexData);
    RwD3D9DynamicVertexBufferUnlock(vertexBuffer);
    RwD3D9SetTexture(texture, 0);
    if (D3D9LastFVF != 0x252) {
        D3D9LastFVF = 0x252;
        D3D9LastVertexDeclaration = 0xFFFFFFFFu;
        ComCall<kSlotSetFVF, long>(Device(), 0x252u);
    }
    RwD3D9SetStreamSource(0, vertexBuffer, 0, 0x2C);
    SelectVertexShader(instance);

    const bool usePixelShaders = (envTexture != nullptr && MatFXPixelShader(3) != nullptr) || MatFXPixelShader(0) != nullptr;
    if (usePixelShaders) {
        VertexAlphaEnable(FALSE);
        RwD3D9SetTexture(texture, 1);
        const RwUInt32 savedAlphaBlend = GetRs(0x1B);
        const RwUInt32 savedAlphaTest  = GetRs(0xF);
        SetRs(0x1B, 0);
        SetRs(0xF, 0);
        bool withEnv = false;
        if (envTexture != nullptr && MatFXPixelShader(3) != nullptr) {
            MatFXEffectData* const envEffect = &MaterialData(instance->material)->data[1];
            const RwReal scaled = envEffect->data.envMap.coef * 255.0f;
            if ((static_cast<RwUInt32>(FloatToIntRounded(scaled)) & 0xFFu) != 0) {
                withEnv = true;
                const RwReal coef = envEffect->data.envMap.coef;
                const RwReal constants[4] = { coef, coef, coef, coef };
                _rpMatFXD3D9SetEnvMapTexture(envTexture, 2, envEffect->data.envMap.frame);
                void* shader;
                if (envEffect->data.envMap.useFrameBufferAlpha != 0) {
                    shader = (TextureHasAlpha(envTexture) != 0) ? MatFXPixelShader(4) : MatFXPixelShader(5);
                } else {
                    shader = MatFXPixelShader(3);
                }
                SelectPixelShader(shader);
                ComCall<kSlotSetPixelShaderConstantF, long>(Device(), 0u, static_cast<const void*>(constants), 1u);
                DrawBatch(header, instance, baseIndex);
                RwD3D9SetTexture(nullptr, 2);
                Tss(2, 0x18, 0);
                Tss(2, 0xB, 2);
            }
        }
        if (!withEnv) {
            SelectPixelShader(MatFXPixelShader(0));
            DrawBatch(header, instance, baseIndex);
        }
        SetRs(0x1B, savedAlphaBlend);
        SetRs(0xF, savedAlphaTest);
        RwD3D9SetTexture(nullptr, 1);
    } else {
        Tss(0, 1, 4);
        Tss(0, 2, 2);
        Tss(0, 3, 0);
        Tss(0, 4, 4);
        Tss(0, 5, 2);
        Tss(0, 6, 0);
        ResetPixelShader();
        if (VertexAlphaEnabled() == 0) {
            VertexAlphaEnable(TRUE);
        }
        SetRs(0xF, 0);
        const RwUInt32 savedSrcBlend = EngineRenderStateGet(0xA);
        const RwUInt32 savedDstBlend = EngineRenderStateGet(0xB);
        _rwD3D9RenderStateSrcBlend(6);
        _rwD3D9RenderStateDestBlend(1);
        DrawBatch(header, instance, baseIndex);
        SetRs(0xF, 1);
        _rwD3D9RenderStateSrcBlend(5);
        _rwD3D9RenderStateDestBlend(2);
        const RwUInt32 savedZWrite = GetRs(0xE);
        SetRs(0xE, 0);
        Tss(0, 0xB, 1);
        DrawBatch(header, instance, baseIndex);
        Tss(0, 0xB, 0);
        SetRs(0xE, savedZWrite);
        VertexAlphaEnable(FALSE);
        EngineRenderStateSet(0xA, savedSrcBlend);
        EngineRenderStateSet(0xB, savedDstBlend);
    }

    _rwD3D9SetStreams(header->vertexStream, header->useOffsets);
    const uintptr_t declaration = reinterpret_cast<uintptr_t>(header->vertexDeclaration);
    if (D3D9LastVertexDeclaration != declaration) {
        D3D9LastVertexDeclaration = declaration;
        D3D9LastFVF = 0xFFFFFFFFu;
        ComCall<kSlotSetVertexDeclaration, long>(Device(), static_cast<void*>(header->vertexDeclaration));
    }
    if (envTexture != nullptr && MatFXPixelShader(3) == nullptr) {
        _rpMatFXD3D9RenderEnvMapPass(header, instance, flags, 1, baseTexture, envTexture);
    }
}

// 0x816190 (lote R10A, sin validar; cdecl sin argumentos, `ret`, 202 bytes; llamador: 0x810BBD, el destructor del plugin de motor 0x810BB0): destruye los pipelines de MatFX de atomico (0xC9AB8C) y de sector (0xC9AB90) con _rxPipelineDestroy (0x805820) y los seis pixel shaders
// (0xC9AB94..0xC9ABA8) con RwD3D9DeletePixelShader (0x7FACF0), poniendo a NULL cada variable que no lo era. Devuelve TRUE.
RwBool _rpMatFXPipelinesDestroy() {
    if (MatFXAtomicPipeline != nullptr) {
        _rxPipelineDestroy(MatFXAtomicPipeline);
        MatFXAtomicPipeline = nullptr;
    }
    if (MatFXWorldSectorPipeline != nullptr) {
        _rxPipelineDestroy(MatFXWorldSectorPipeline);
        MatFXWorldSectorPipeline = nullptr;
    }
    for (size_t i = 0; i < 6; ++i) {
        void*& shader = MatFXPixelShader(i);
        if (shader != nullptr) {
            RwD3D9DeletePixelShader(shader);
            shader = nullptr;
        }
    }
    return TRUE;
}

// ---------------------------------------------------------------------------------------------------------------------------------------------
// Lotes S13A y S14A (sin validar): callbacks del plugin 0x120 (motor, RpMaterial, RpAtomic, RpWorldSector) y del canal D3D9 de MatFX. Cada cuerpo sale del desensamblado del EXE (capstone); los almacenamientos que el
// compilador del EXE hacia en las ranuras de los argumentos de la pila se han pasado a variables locales. Las busquedas de ranura en linea del EXE son `FindEffect` (NULL si no existe: el EXE desreferencia ese NULL).
// ---------------------------------------------------------------------------------------------------------------------------------------------

// 0x810BB0 (cdecl, (objeto, desplazamiento, tamano); 51 bytes): contador de aperturas (0xC9AB7C) - 1; al llegar a 0 destruye los pipelines (0x816190). Despues, si existe la lista libre de datos (0xC9AB80) la destruye
// (RwFreeListDestroy 0x801B80) y la pone a NULL. Devuelve el objeto.
static void* _rpMatFXEngineDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    StaticRef<RwInt32>(0xC9AB7C) -= 1;
    if (StaticRef<RwInt32>(0xC9AB7C) == 0) {
        _rpMatFXPipelinesDestroy();
    }
    if (MatFXDataFreeList != nullptr) {
        RwFreeListDestroy(MatFXDataFreeList);
        MatFXDataFreeList = nullptr;
    }
    return object;
}

// 0x810BF0 (cdecl, (objeto, desplazamiento, tamano); 80 bytes): con el contador de aperturas (0xC9AB7C) a 0 crea la lista libre de datos (RwFreeListCreateAndPreallocateSpace: entrada 0x34, bloque 0x8E29B8, alineacion 4,
// bloques a reservar 0x8E29BC, espacio en el lugar 0xC9AB50, pista 0x40120; NULL -> NULL) y los pipelines (0x815E60; FALSE -> NULL). Cuenta + 1 y devuelve el objeto.
static void* _rpMatFXEngineConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (StaticRef<RwInt32>(0xC9AB7C) == 0) {
        MatFXDataFreeList = RwFreeListCreateAndPreallocateSpace(0x34, MatFXFreeListBlockSize, 4, MatFXFreeListNumPrealloc, reinterpret_cast<RwFreeList*>(0xC9AB50), 0x40120);
        if (MatFXDataFreeList == nullptr) {
            return nullptr;
        }
        if (_rpMatFXPipelinesCreate() == FALSE) {
            return nullptr;
        }
    }
    StaticRef<RwInt32>(0xC9AB7C) += 1;
    return object;
}

// 0x810C40 (cdecl, (objeto, desplazamiento, tamano); 18 bytes): puntero de datos de MatFX del material (en objeto + 0xC9AB74) = NULL. Devuelve el objeto.
static void* _rpMatFXMaterialConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    MaterialData(static_cast<const RpMaterial*>(object)) = nullptr;
    return object;
}

// 0x810C60 (cdecl, (objeto, desplazamiento, tamano); 64 bytes): si el material tiene datos: _rpMatFXMaterialDataClean (0x810CA0), los devuelve a la lista libre (RwFreeListFree) y deja el puntero a NULL. Devuelve el objeto.
static void* _rpMatFXMaterialDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    rpMatFXMaterialData* const data = MaterialData(static_cast<const RpMaterial*>(object));
    if (data != nullptr) {
        _rpMatFXMaterialDataClean(data);
        RwFreeListFree(MatFXDataFreeList, data);
        MaterialData(static_cast<const RpMaterial*>(object)) = nullptr;
    }
    return object;
}

// 0x810D20 (cdecl, (destino, origen, desplazamiento, tamano); 1043 bytes; fld/fchs/fstp en el coeficiente de relieve): copia de los datos de MatFX. Sin datos en el origen: NULL. Sin datos en el destino los reserva (lista
// libre, pista 0x30120; NULL -> NULL) a cero. RpMatFXMaterialSetEffects(destino, banderas del origen) y, por cada una de las dos ranuras segun la bandera de la ranura del origen:
//  1 relieve: marco (+0) y coeficiente (copiado por la x87: un NaN senalizante sale silencioso) con las busquedas de efecto del EXE; despues ranura destino +4 = textura generada y +8 = textura de relieve del origen; +1 al refCount de la textura +4 del origen y
//    +1 al de la +8 copiada (cada una si no es NULL; el marco ya se copio con la busqueda de efecto);
//  2 entorno: textura (+4), marco (+0), coeficiente (+8, por la x87) y alfa del bufer (+0xC); con textura: +1 referencia, destruye la anterior y la guarda; despues marco, alfa y coeficiente en la ranura destino;
//  3: nada; 4 dual: textura (+0) y mezclas (+4, +8); con textura: +1 referencia, destruye la anterior, la guarda y llama al relleno 0x816280 con 2 y 1; despues mezclas y relleno con 0xA y 0xB;
//  5 transformacion UV: matrices (+0, +4) a la ranura destino. Devuelve el destino.
static void* _rpMatFXMaterialCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RpMaterial* const dst = static_cast<RpMaterial*>(dstObject);
    const RpMaterial* const src = static_cast<const RpMaterial*>(srcObject);
    rpMatFXMaterialData* const srcData = MaterialData(src);
    if (srcData == nullptr) {
        return nullptr;
    }
    rpMatFXMaterialData* dstData = MaterialData(dst);
    if (dstData == nullptr) {
        dstData = static_cast<rpMatFXMaterialData*>(RwFreeListAlloc(MatFXDataFreeList, kMatFXMaterialMemHint));
        if (dstData == nullptr) {
            return nullptr;
        }
        memset(dstData, 0, sizeof(rpMatFXMaterialData));
        MaterialData(dst) = dstData;
    }
    RpMatFXMaterialSetEffects(dst, srcData->flags);
    for (RwInt32 slot = 0; slot < 2; ++slot) {
        MatFXEffectData& dstSlot = dstData->data[slot];
        const MatFXEffectData& srcSlot = srcData->data[slot];
        switch (static_cast<RwUInt32>(srcSlot.flag)) {
        case rpMATFXEFFECTBUMPMAP: {
            RwFrame* const frame = _rpMatFXMaterialFindEffectConst(src, rpMATFXEFFECTBUMPMAP)->data.bumpMap.frame;
            const RwUInt32 coefBits = X87Reload(RealBits(_rpMatFXMaterialFindEffectConst(src, rpMATFXEFFECTBUMPMAP)->data.bumpMap.coef)); // fld + fchs + fstp, y otra vez
            _rpMatFXMaterialFindEffect(dst, rpMATFXEFFECTBUMPMAP)->data.bumpMap.frame = frame;
            StoreBits(_rpMatFXMaterialFindEffect(dst, rpMATFXEFFECTBUMPMAP)->data.bumpMap.coef, coefBits);
            dstSlot.data.bumpMap.texture = srcSlot.data.bumpMap.texture;
            dstSlot.data.bumpMap.bumpTexture = srcSlot.data.bumpMap.bumpTexture;
            if (srcSlot.data.bumpMap.texture != nullptr) {
                ++srcSlot.data.bumpMap.texture->refCount;
            }
            if (dstSlot.data.bumpMap.bumpTexture != nullptr) {
                ++dstSlot.data.bumpMap.bumpTexture->refCount;
            }
            break;
        }
        case rpMATFXEFFECTENVMAP: {
            RwTexture* const texture = _rpMatFXMaterialFindEffectConst(src, rpMATFXEFFECTENVMAP)->data.envMap.texture;
            RwFrame* const frame = _rpMatFXMaterialFindEffectConst(src, rpMATFXEFFECTENVMAP)->data.envMap.frame;
            const RwUInt32 coefBits = X87Reload(RealBits(_rpMatFXMaterialFindEffectConst(src, rpMATFXEFFECTENVMAP)->data.envMap.coef));
            const RwBool fba = _rpMatFXMaterialFindEffectConst(src, rpMATFXEFFECTENVMAP)->data.envMap.useFrameBufferAlpha;
            if (texture != nullptr) {
                MatFXEnvMapData& env = _rpMatFXMaterialFindEffect(dst, rpMATFXEFFECTENVMAP)->data.envMap;
                ++texture->refCount;
                if (env.texture != nullptr) {
                    RwTextureDestroy(env.texture);
                    env.texture = nullptr;
                }
                env.texture = texture;
            }
            FindEffect(dst, rpMATFXEFFECTENVMAP)->data.envMap.frame = frame;
            FindEffect(dst, rpMATFXEFFECTENVMAP)->data.envMap.useFrameBufferAlpha = fba;
            StoreBits(FindEffect(dst, rpMATFXEFFECTENVMAP)->data.envMap.coef, coefBits);
            break;
        }
        case rpMATFXEFFECTDUAL: {
            RwTexture* const texture = FindEffect(src, rpMATFXEFFECTDUAL)->data.dual.texture;
            const RwBlendFunction srcBlend = FindEffect(src, rpMATFXEFFECTDUAL)->data.dual.srcBlendMode;
            const RwBlendFunction dstBlend = FindEffect(src, rpMATFXEFFECTDUAL)->data.dual.dstBlendMode;
            if (texture != nullptr) {
                MatFXEffectData* const effect = _rpMatFXMaterialFindEffect(dst, rpMATFXEFFECTDUAL);
                ++texture->refCount;
                if (effect->data.dual.texture != nullptr) {
                    RwTextureDestroy(effect->data.dual.texture);
                    effect->data.dual.texture = nullptr;
                }
                effect->data.dual.texture = texture;
                _rpMatFXD3D9EffectStub(effect, 2);
                _rpMatFXD3D9EffectStub(effect, 1);
            }
            MatFXEffectData* const effect = _rpMatFXMaterialFindEffect(dst, rpMATFXEFFECTDUAL);
            effect->data.dual.srcBlendMode = srcBlend;
            effect->data.dual.dstBlendMode = dstBlend;
            _rpMatFXD3D9EffectStub(effect, 0xA);
            _rpMatFXD3D9EffectStub(effect, 0xB);
            break;
        }
        case rpMATFXEFFECTUVTRANSFORM: {
            MatFXEffectData* const effect = _rpMatFXMaterialFindEffectConst(src, rpMATFXEFFECTUVTRANSFORM);
            RwMatrix* const baseTransform = effect->data.uvAnim.baseTransform;
            RwMatrix* const dualTransform = effect->data.uvAnim.dualTransform;
            MatFXEffectData* const target = FindEffect(dst, rpMATFXEFFECTUVTRANSFORM);
            target->data.uvAnim.baseTransform = baseTransform;
            target->data.uvAnim.dualTransform = dualTransform;
            break;
        }
        default:
            break;
        }
    }
    return dst;
}

// 0x8111F0 (cdecl, (flujo, longitud, objeto, desplazamiento, tamano); 534 bytes; RwStreamWriteReal 0x7ED3D0 y RwStreamWriteInt32 0x7ED460): escritura de los datos de MatFX. Banderas de los datos (4 bytes) y, por cada una
// de las dos ranuras: su bandera (4 bytes) y segun ella: 1 relieve = coeficiente NEGADO (fld + fchs + fstp, real), "hay textura generada" (+4; entero), su textura (RwTextureStreamWrite 0x8045E0 si hay), "hay textura de
// relieve" (+8) y su textura; 2 entorno = coeficiente (+8, real tal cual), alfa del bufer (+0xC), "hay textura" (+4) y su textura; 4 dual = mezclas (+4 y +8), "hay textura" (+0) y su textura; las demas banderas no
// escriben nada. Cualquier fallo de escritura devuelve NULL; al final devuelve el flujo. Sin comprobar que existan los datos (el EXE los desreferencia).
static RwStream* _rpMatFXMaterialStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    rpMatFXMaterialData* const data = MaterialData(static_cast<const RpMaterial*>(object));
    RwInt32 value = static_cast<RwInt32>(data->flags);
    if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
        return nullptr;
    }
    for (RwUInt8 i = 0; i < 2; ++i) {
        MatFXEffectData* const slot = &data->data[i];
        const RwInt32 flag = static_cast<RwInt32>(slot->flag);
        value = flag;
        if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
            return nullptr;
        }
        switch (flag) {
        case rpMATFXEFFECTBUMPMAP: {
            RwReal negatedCoef;
            StoreBits(negatedCoef, X87ReloadNegated(RealBits(slot->data.bumpMap.coef)));
            if (RwStreamWriteReal(stream, &negatedCoef, 4) == nullptr) {
                return nullptr;
            }
            RwTexture* texture = slot->data.bumpMap.texture;
            value = (texture != nullptr) ? 1 : 0;
            if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
                return nullptr;
            }
            if (value != 0 && RwTextureStreamWrite(texture, stream) == nullptr) {
                return nullptr;
            }
            if (stream == nullptr) {
                return nullptr;
            }
            texture = slot->data.bumpMap.bumpTexture;
            value = (texture != nullptr) ? 1 : 0;
            if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
                return nullptr;
            }
            if (value != 0 && RwTextureStreamWrite(texture, stream) == nullptr) {
                return nullptr;
            }
            break;
        }
        case rpMATFXEFFECTENVMAP:
        case rpMATFXEFFECTDUAL: {
            RwTexture* texture;
            if (flag == rpMATFXEFFECTENVMAP) {
                if (RwStreamWriteReal(stream, &slot->data.envMap.coef, 4) == nullptr) {
                    return nullptr;
                }
                value = slot->data.envMap.useFrameBufferAlpha;
                if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
                    return nullptr;
                }
                texture = slot->data.envMap.texture;
            } else {
                value = slot->data.dual.srcBlendMode;
                if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
                    return nullptr;
                }
                value = slot->data.dual.dstBlendMode;
                if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
                    return nullptr;
                }
                texture = slot->data.dual.texture;
            }
            value = (texture != nullptr) ? 1 : 0;
            if (RwStreamWriteInt32(stream, &value, 4) == nullptr) {
                return nullptr;
            }
            if (value != 0 && RwTextureStreamWrite(texture, stream) == nullptr) {
                return nullptr;
            }
            if (stream == nullptr) {
                return nullptr;
            }
            break;
        }
        default:
            break;
        }
    }
    return stream;
}

namespace {
// Lectura de una textura del flujo como la compila el EXE en 0x811410: RwStreamFindChunk(flujo, rwID_TEXTURE = 6, NULL, NULL) (FALSE -> fallo), RwErrorGet, RwTextureStreamRead (0x8046E0) y, si da NULL, RwErrorGet otra vez:
// si el codigo de error no es 0x80000000 ni 0x16 hace RwErrorSet y es un fallo; con esos dos codigos sigue con textura NULL.
bool MatFXReadTexture(RwStream* stream, RwTexture*& texture) {
    texture = nullptr;
    if (RwStreamFindChunk(stream, 6, nullptr, nullptr) == FALSE) {
        return false;
    }
    RwError error;
    RwErrorGet(&error);
    texture = RwTextureStreamRead(stream);
    if (texture == nullptr) {
        RwErrorGet(&error);
        if (error.errorCode != static_cast<RwInt32>(0x80000000) && error.errorCode != 0x16) {
            RwErrorSet(&error);
            return false;
        }
    }
    return true;
}
} // namespace

// 0x811410 (cdecl, (flujo, longitud, objeto, desplazamiento, tamano); 1361 bytes; x87: 1.0f / ancho y fchs): lectura de los datos de MatFX. Reserva los datos si faltan (lista libre, pista 0x30120, a cero; NULL -> NULL).
// Lee las banderas (RwStreamReadInt32 0x7ED540; fallo -> NULL), RpMatFXMaterialSetEffects y por cada una de las dos ranuras lee el tipo (fallo -> NULL):
//  4 dual: dos mezclas (8 bytes), "hay textura" y su textura (MatFXReadTexture; sin textura NULL); con textura: +1 referencia, destruye la de +0, la guarda, relleno 0x816280 (2 y 1) y RwTextureDestroy; despues
//    RpMatFXMaterialSetDualBlendModes (0x8128C0);
//  1 relieve: coeficiente (RwStreamReadReal 0x7ED4F0), "hay textura generada" y su textura, y "hay textura de relieve" y la suya (un fallo de lectura de la segunda destruye la primera y devuelve NULL); con la
//    generada: +4 = esa, +8 = la de relieve (o NULL) e invBumpWidth (+0x10) = 1.0f / ancho de su raster (fild + fdivr 1.0f, en x87); sin ella y con la de relieve: RpMatFXMaterialSetBumpMapTexture (0x812160) y
//    RwTextureDestroy; sin ninguna: +4 = +8 = NULL. Termina con coeficiente (+0xC) = - lo leido (fld + fchs + fstp) en la ranura de relieve;
//  2 entorno: coeficiente, alfa del bufer y "hay textura" y su textura; con textura: +1 referencia, destruye la de +4, la guarda y RwTextureDestroy. Despues coeficiente (+8; palabra tal cual) y alfa (+0xC);
//  otros tipos: nada. Devuelve el flujo.
static RwStream* _rpMatFXMaterialStreamRead(RwStream* stream, RwInt32 binaryLength, void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RpMaterial* const material = static_cast<RpMaterial*>(object);
    rpMatFXMaterialData* data = MaterialData(material);
    if (data == nullptr) {
        data = static_cast<rpMatFXMaterialData*>(RwFreeListAlloc(MatFXDataFreeList, kMatFXMaterialMemHint));
        if (data == nullptr) {
            return nullptr;
        }
        memset(data, 0, sizeof(rpMatFXMaterialData));
        MaterialData(material) = data;
    }
    RwInt32 flags;
    if (RwStreamReadInt32(stream, &flags, 4) == nullptr) {
        return nullptr;
    }
    RpMatFXMaterialSetEffects(material, static_cast<RpMatFXMaterialFlags>(flags));
    for (RwUInt8 i = 0; i < 2; ++i) {
        RwInt32 type;
        if (RwStreamReadInt32(stream, &type, 4) == nullptr) {
            return nullptr;
        }
        switch (type) {
        case rpMATFXEFFECTDUAL: {
            RwInt32 blends[2];
            if (RwStreamReadInt32(stream, blends, 8) == nullptr) {
                return nullptr;
            }
            RwInt32 hasTexture;
            if (RwStreamReadInt32(stream, &hasTexture, 4) == nullptr) {
                return nullptr;
            }
            RwTexture* texture = nullptr;
            if (hasTexture != 0 && !MatFXReadTexture(stream, texture)) {
                return nullptr;
            }
            if (stream == nullptr) {
                return nullptr;
            }
            if (texture != nullptr) {
                MatFXEffectData* const effect = _rpMatFXMaterialFindEffect(material, rpMATFXEFFECTDUAL);
                ++texture->refCount;
                if (effect->data.dual.texture != nullptr) {
                    RwTextureDestroy(effect->data.dual.texture);
                    effect->data.dual.texture = nullptr;
                }
                effect->data.dual.texture = texture;
                _rpMatFXD3D9EffectStub(effect, 2);
                _rpMatFXD3D9EffectStub(effect, 1);
                RwTextureDestroy(texture);
            }
            RpMatFXMaterialSetDualBlendModes(material, static_cast<RwBlendFunction>(blends[0]), static_cast<RwBlendFunction>(blends[1]));
            break;
        }
        case rpMATFXEFFECTBUMPMAP: {
            MatFXEffectData* const slot = &data->data[i];
            RwReal coef;
            if (RwStreamReadReal(stream, &coef, 4) == nullptr) {
                return nullptr;
            }
            RwInt32 hasBumped;
            if (RwStreamReadInt32(stream, &hasBumped, 4) == nullptr) {
                return nullptr;
            }
            RwTexture* bumped = nullptr;
            if (hasBumped != 0 && !MatFXReadTexture(stream, bumped)) {
                return nullptr;
            }
            if (stream == nullptr) {
                return nullptr;
            }
            RwTexture* bumpTexture = nullptr;
            bool failed = false;
            RwInt32 hasBumpTexture;
            if (RwStreamReadInt32(stream, &hasBumpTexture, 4) == nullptr) {
                failed = true;
            } else if (hasBumpTexture != 0 && !MatFXReadTexture(stream, bumpTexture)) {
                failed = true;
            }
            if (failed) {
                if (bumped != nullptr) {
                    RwTextureDestroy(bumped);
                }
                return nullptr;
            }
            if (bumped != nullptr) {
                slot->data.bumpMap.texture = bumped;
                slot->data.bumpMap.bumpTexture = bumpTexture;
                const double inverse = Veh83DDiv(1.0, static_cast<double>(bumped->raster->width)); // fild + fdivr [1.0f]
                slot->data.bumpMap.invBumpWidth = Veh83DStore(inverse);
            } else if (bumpTexture != nullptr) {
                RpMatFXMaterialSetBumpMapTexture(material, bumpTexture);
                RwTextureDestroy(bumpTexture);
            } else {
                slot->data.bumpMap.texture = nullptr;
                slot->data.bumpMap.bumpTexture = nullptr;
            }
            StoreBits(FindEffect(material, rpMATFXEFFECTBUMPMAP)->data.bumpMap.coef, X87ReloadNegated(RealBits(coef)));
            break;
        }
        case rpMATFXEFFECTENVMAP: {
            RwReal coef;
            if (RwStreamReadReal(stream, &coef, 4) == nullptr) {
                return nullptr;
            }
            RwInt32 useFrameBufferAlpha;
            if (RwStreamReadInt32(stream, &useFrameBufferAlpha, 4) == nullptr) {
                return nullptr;
            }
            RwInt32 hasTexture;
            if (RwStreamReadInt32(stream, &hasTexture, 4) == nullptr) {
                return nullptr;
            }
            RwTexture* texture = nullptr;
            if (hasTexture != 0 && !MatFXReadTexture(stream, texture)) {
                return nullptr;
            }
            if (stream == nullptr) {
                return nullptr;
            }
            if (texture != nullptr) {
                MatFXEffectData* const effect = _rpMatFXMaterialFindEffect(material, rpMATFXEFFECTENVMAP);
                ++texture->refCount;
                if (effect->data.envMap.texture != nullptr) {
                    RwTextureDestroy(effect->data.envMap.texture);
                    effect->data.envMap.texture = nullptr;
                }
                effect->data.envMap.texture = texture;
                RwTextureDestroy(texture);
            }
            StoreBits(FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.coef, RealBits(coef));
            FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.useFrameBufferAlpha = useFrameBufferAlpha;
            break;
        }
        default:
            break;
        }
    }
    return stream;
}

// 0x811970 (cdecl, (objeto, desplazamiento, tamano); 141 bytes): tamano de los datos de MatFX en el flujo. Sin datos o con banderas 0: 0. Si no, 4 + por cada una de las dos ranuras (4 de la bandera mas): 1 relieve = 4 + el
// de la textura generada (+4: 4 si no hay, RwTextureStreamGetSize 0x8045A0 + 0x10 si hay) + el de la de relieve (+8); 2 entorno = 8 + el de la textura (+4); 4 dual = 8 + el de la textura (+0); otras banderas: nada mas.
static RwInt32 _rpMatFXMaterialStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const rpMatFXMaterialData* const data = MaterialData(static_cast<const RpMaterial*>(object));
    if (data == nullptr || data->flags == 0) {
        return 0;
    }
    const auto textureTerm = [](const RwTexture* texture) -> RwUInt32 {
        return texture != nullptr ? RwTextureStreamGetSize(texture) + 0x10 : 4;
    };
    RwUInt32 size = 4;
    for (RwInt32 i = 0; i < 2; ++i) {
        const MatFXEffectData& slot = data->data[i];
        size += 4;
        switch (static_cast<RwUInt32>(slot.flag)) {
        case rpMATFXEFFECTBUMPMAP:
            size += 4;
            size += textureTerm(slot.data.bumpMap.texture);
            size += textureTerm(slot.data.bumpMap.bumpTexture);
            break;
        case rpMATFXEFFECTENVMAP:
            size += 8;
            size += textureTerm(slot.data.envMap.texture);
            break;
        case rpMATFXEFFECTDUAL:
            size += 8;
            size += textureTerm(slot.data.dual.texture);
            break;
        default:
            break;
        }
    }
    return static_cast<RwInt32>(size);
}

// 0x811A00 / 0x811A20 / 0x811B00 / 0x811B20 (cdecl, (objeto, desplazamiento, tamano); 18 bytes cada una): ponen a 0 la bandera de MatFX del atomico (en objeto + 0xC9AB84) o del sector (+ 0xC9AB88). Devuelven el objeto.
static void* _rpMatFXAtomicConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    *reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(object) + MatFXAtomicDataOffset) = 0;
    return object;
}
static void* _rpMatFXAtomicDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    *reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(object) + MatFXAtomicDataOffset) = 0;
    return object;
}
static void* _rpMatFXWorldSectorConstructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    *reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(object) + MatFXWorldSectorDataOffset) = 0;
    return object;
}
static void* _rpMatFXWorldSectorDestructor(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    *reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(object) + MatFXWorldSectorDataOffset) = 0;
    return object;
}

// 0x811A40 / 0x811B40 (cdecl, (destino, origen, desplazamiento, tamano); 29 bytes): si la bandera del origen no es 0 pone la del destino a 1. Devuelven el destino.
static void* _rpMatFXAtomicCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (*reinterpret_cast<const RwInt32*>(static_cast<const RwUInt8*>(srcObject) + MatFXAtomicDataOffset) != 0) {
        *reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(dstObject) + MatFXAtomicDataOffset) = 1;
    }
    return dstObject;
}
static void* _rpMatFXWorldSectorCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    if (*reinterpret_cast<const RwInt32*>(static_cast<const RwUInt8*>(srcObject) + MatFXWorldSectorDataOffset) != 0) {
        *reinterpret_cast<RwInt32*>(static_cast<RwUInt8*>(dstObject) + MatFXWorldSectorDataOffset) = 1;
    }
    return dstObject;
}

// 0x811A60 / 0x811B60 (cdecl, (flujo, longitud, objeto, desplazamiento, tamano); 38 bytes): RwStreamWriteInt32(flujo, bandera del objeto, 4) (el valor entero tal cual) y devuelve su resultado.
static RwStream* _rpMatFXAtomicStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwInt32 value = *reinterpret_cast<const RwInt32*>(static_cast<const RwUInt8*>(object) + MatFXAtomicDataOffset);
    return RwStreamWriteInt32(stream, &value, 4);
}
static RwStream* _rpMatFXWorldSectorStreamWrite(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    RwInt32 value = *reinterpret_cast<const RwInt32*>(static_cast<const RwUInt8*>(object) + MatFXWorldSectorDataOffset);
    return RwStreamWriteInt32(stream, &value, 4);
}

// 0x811AE0 / 0x811BE0 (cdecl, (objeto, desplazamiento, tamano); 21 bytes): 4 si la bandera del objeto no es 0, si no 0.
static RwInt32 _rpMatFXAtomicStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return *reinterpret_cast<const RwInt32*>(static_cast<const RwUInt8*>(object) + MatFXAtomicDataOffset) != 0 ? 4 : 0;
}
static RwInt32 _rpMatFXWorldSectorStreamGetSize(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    return *reinterpret_cast<const RwInt32*>(static_cast<const RwUInt8*>(object) + MatFXWorldSectorDataOffset) != 0 ? 4 : 0;
}

// 0x815C20 (cdecl, (entrada de recursos, objeto, tipo, banderas); 504 bytes; callback de render de los dos pipelines de MatFX): _rwD3D9SetClippingForObject(objeto, tipo). Con la iluminacion (estado 0x89) apagada y sin
// la bandera 8: "modo plano" (textura 0, TEXTUREFACTOR 0xFF000000 y COLOROP/COLORARG2/ALPHAOP/ALPHAARG2 de la etapa 0 a 3). Cabecera = entrada + 0x18, mallas = cabecera + 0x40. Con bufer de indices distinto del ultimo
// (0x8E2450): SetIndices (+0x1A0). _rwD3D9SetStreams (flujos, useOffsets) y, si la declaracion de vertices difiere de la ultima (0x8E2444): la guarda, FVF 0x8E2440 = 0xFFFFFFFF y SetVertexDeclaration (+0x15C).
// Por malla (do-while sobre numMeshes): alfa por vertice 0 si la malla no lo tiene y el material es opaco (alfa 0xFF), si no 1 (0x7FE0A0); modo plano: _rpMatFXD3D9DrawMesh; si no, con la iluminacion encendida
// RwD3D9SetSurfaceProperties(material + 0xC, material + 4, banderas) y segun las banderas de los datos de MatFX (1 relieve, 2 entorno, 3 relieve + entorno, 4 dual, 6 UV + dual en la ranura 1, el resto
// transformacion UV): _rpMatFXD3D9RenderBumpMap / RenderEnvMapPass / RenderDualTexture / DrawMeshUVTransform con la textura del material como base.
static void _rpMatFXD3D9AllInOneRender(RwResEntry* repEntry, void* object, RwUInt8 type, RwUInt32 flags) {
    _rwD3D9SetClippingForObject(object, type);
    const RwUInt32 lighting = GetRs(0x89);
    bool plain = false;
    if (lighting == 0 && (flags & 8) == 0) {
        plain = true;
        RwD3D9SetTexture(nullptr, 0);
        SetRs(0x3C, 0xFF000000u);
        Tss(0, 1, 3);
        Tss(0, 3, 3);
        Tss(0, 4, 3);
        Tss(0, 6, 3);
    }
    RxD3D9ResEntryHeader* const header = reinterpret_cast<RxD3D9ResEntryHeader*>(reinterpret_cast<RwUInt8*>(repEntry) + 0x18);
    RxD3D9InstanceData* instance = reinterpret_cast<RxD3D9InstanceData*>(reinterpret_cast<RwUInt8*>(header) + 0x40);
    if (header->indexBuffer != nullptr && D3D9LastIndexBuffer != header->indexBuffer) {
        D3D9LastIndexBuffer = header->indexBuffer;
        ComCall<kSlotSetIndices, long>(Device(), static_cast<void*>(header->indexBuffer));
    }
    _rwD3D9SetStreams(header->vertexStream, header->useOffsets);
    if (D3D9LastVertexDeclaration != reinterpret_cast<uintptr_t>(header->vertexDeclaration)) {
        D3D9LastVertexDeclaration = reinterpret_cast<uintptr_t>(header->vertexDeclaration);
        D3D9LastFVF = 0xFFFFFFFFu;
        ComCall<kSlotSetVertexDeclaration, long>(Device(), static_cast<void*>(header->vertexDeclaration));
    }
    if (header->numMeshes == 0) {
        return;
    }
    RwUInt32 remaining = header->numMeshes;
    do {
        _rwD3D9RenderStateVertexAlphaEnable((instance->vertexAlpha == 0 && instance->material->color.alpha == 0xFF) ? FALSE : TRUE);
        if (plain) {
            _rpMatFXD3D9DrawMesh(header, instance);
        } else {
            RpMaterial* const material = instance->material;
            if (lighting != 0) {
                RwD3D9SetSurfaceProperties(&material->surfaceProps, &material->color, flags);
            }
            const rpMatFXMaterialData* const data = MaterialData(material);
            RwUInt32 effects = (data == nullptr) ? 0 : static_cast<RwUInt32>(data->flags);
            switch (effects) {
            case 1:
                _rpMatFXD3D9RenderBumpMap(header, instance, flags, material->texture, data->data[0].data.bumpMap.texture, nullptr);
                break;
            case 2:
                _rpMatFXD3D9RenderEnvMapPass(header, instance, flags, 0, material->texture, data->data[0].data.envMap.texture);
                break;
            case 3:
                _rpMatFXD3D9RenderBumpMap(header, instance, flags, material->texture, data->data[0].data.bumpMap.texture, data->data[1].data.envMap.texture);
                break;
            case 4:
                _rpMatFXD3D9RenderDualTexture(header, instance, flags, material->texture, data->data[0].data.dual.texture);
                break;
            case 6:
                _rpMatFXD3D9RenderDualTexture(header, instance, flags, material->texture, data->data[1].data.dual.texture);
                break;
            default:
                _rpMatFXD3D9DrawMeshUVTransform(header, instance, flags, material->texture);
                break;
            }
        }
        ++instance;
        --remaining;
    } while (remaining != 0);
}

// 0x8160A0 (cdecl, (atomico, cabecera, reinstanciar); 116 bytes; callback de instanciado del pipeline de atomico): con reinstanciar = 0, por cada malla de la cabecera (do-while sobre numMeshes; malla = cabecera + 0x40 + 0x24 * n)
// cuyo material tenga datos de MatFX con banderas 1 o 3 y con pixel shaders disponibles (0xC9ABC0): RpD3D9GeometrySetUsageFlags(geometria del atomico, uso | 0x10000). Termina llamando al instanciado original guardado
// (0xC9ABC4) con los mismos argumentos y devuelve su resultado.
static RwBool _rpMatFXAtomicInstanceCallBack(void* object, RxD3D9ResEntryHeader* header, RwBool reinstance) {
    if (reinstance == 0) {
        RxD3D9InstanceData* instance = reinterpret_cast<RxD3D9InstanceData*>(reinterpret_cast<RwUInt8*>(header) + 0x40);
        RwUInt32 remaining = header->numMeshes;
        do {
            const rpMatFXMaterialData* const data = MaterialData(instance->material);
            if (data != nullptr && (data->flags == 1 || data->flags == 3) && MatFXPixelShadersOk != 0) {
                RpGeometry* const geometry = static_cast<RpAtomic*>(object)->geometry;
                RpD3D9GeometrySetUsageFlags(geometry, static_cast<RpD3D9GeometryUsageFlag>(RpD3D9GeometryGetUsageFlags(geometry) | 0x10000));
            }
            ++instance;
            --remaining;
        } while (remaining != 0);
    }
    return MatFXAtomicInstanceCB(object, header, reinstance);
}

// 0x816120 (cdecl, (sector, cabecera, reinstanciar); 101 bytes; callback de instanciado del pipeline de sector): igual que 0x8160A0 pero SIN mirar `reinstanciar` y con RpD3D9WorldSector{Get,Set}UsageFlags sobre el sector;
// termina con el instanciado original guardado (0xC9ABC8).
static RwBool _rpMatFXWorldSectorInstanceCallBack(void* object, RxD3D9ResEntryHeader* header, RwBool reinstance) {
    RxD3D9InstanceData* instance = reinterpret_cast<RxD3D9InstanceData*>(reinterpret_cast<RwUInt8*>(header) + 0x40);
    RwUInt32 remaining = header->numMeshes;
    do {
        const rpMatFXMaterialData* const data = MaterialData(instance->material);
        if (data != nullptr && (data->flags == 1 || data->flags == 3) && MatFXPixelShadersOk != 0) {
            RpWorldSector* const sector = static_cast<RpWorldSector*>(object);
            RpD3D9WorldSectorSetUsageFlags(sector, static_cast<RpD3D9WorldSectorUsageFlag>(RpD3D9WorldSectorGetUsageFlags(sector) | 0x10000));
        }
        ++instance;
        --remaining;
    } while (remaining != 0);
    return MatFXSectorInstanceCB(object, header, reinstance);
}
