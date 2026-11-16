#include "StdInc.h"

#include "rpmatfx.h"

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

// Llamada COM stdcall por la tabla virtual (this primero).
template<size_t Slot, typename R, typename... Args>
R ComCall(void* self, Args... args) {
    auto* const vtbl = *reinterpret_cast<void***>(self);
    return reinterpret_cast<R(__stdcall*)(void*, Args...)>(vtbl[Slot])(self, args...);
}
constexpr size_t kSlotDrawPrimitive        = 81;  // IDirect3DDevice9::DrawPrimitive (+0x144)
constexpr size_t kSlotDrawIndexedPrimitive = 82;  // IDirect3DDevice9::DrawIndexedPrimitive (+0x148)
constexpr size_t kSlotSetVertexShader      = 92;  // IDirect3DDevice9::SetVertexShader (+0x170)
constexpr size_t kSlotSetPixelShader       = 107; // IDirect3DDevice9::SetPixelShader (+0x1AC)

void* Device() { return *reinterpret_cast<void**>(0xC97C28); } // _RwD3DDevice

constexpr uintptr_t kBumpedTextureCreate   = 0x810830; // RwTexture* (RwTexture* base, RwTexture* bump): combina la textura del material con el mapa de relieve (fuera del lote)
constexpr uintptr_t kRasterIsCube          = 0x4C9F70; // _rwD3D9RasterIsCubeRaster(RwRaster*)
constexpr uintptr_t kD3D9FlushRenderStates = 0x7FC200; // vacia los estados de render pendientes antes de dibujar
constexpr uintptr_t kEnvMapHalfMatrix      = 0x885608; // RwMatrix constante (0.5, -0.5, 1, traslacion 0.5/0.5): de [-1,1] a [0,1]

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
}

void RpMatFXMaterialDataSetFreeListCreateParams(RwInt32 blockSize, RwInt32 numBlocksToPrealloc) {
    ((void(__cdecl *)(RwInt32, RwInt32))0x810700)(blockSize, numBlocksToPrealloc);
}

// 0x810AA0 (lote R8A, sin validar; cdecl sin argumentos, `ret`, 266 bytes): registra el plugin 0x120 y devuelve TRUE si todos los registros dieron >= 0 (el ultimo lo comprueba con setge; los demas
// devuelven FALSE de inmediato). Orden: motor (ctor 0x810BF0 abre el modulo y crea la lista libre y los pipelines con 0x815E60; dtor 0x810BB0), material (tamano 4, ctor 0x810C40, dtor 0x810C60, copia 0x810D20;
// el desplazamiento va a 0xC9AB74) y su flujo (lee 0x811410, escribe 0x8111F0, tamano 0x811970), atomico (tamano 4, ctor 0x811A00, dtor 0x811A20, copia 0x811A40; desplazamiento a 0xC9AB84) y su flujo
// (lee 0x811A90, escribe 0x811A60, tamano 0x811AE0) y sector (tamano 4, ctor 0x811B00, dtor 0x811B20, copia 0x811B40; a 0xC9AB88) con su flujo (lee 0x811B90, escribe 0x811B60, tamano 0x811BE0).
RwBool RpMatFXPluginAttach() {
    if (RwEngineRegisterPlugin(0, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(0x810BF0), reinterpret_cast<RwPluginObjectDestructor>(0x810BB0)) < 0) {
        return FALSE;
    }
    MatFXMaterialDataOffset = RpMaterialRegisterPlugin(4, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(0x810C40), reinterpret_cast<RwPluginObjectDestructor>(0x810C60),
                                                       reinterpret_cast<RwPluginObjectCopy>(0x810D20));
    if (MatFXMaterialDataOffset < 0) {
        return FALSE;
    }
    if (RpMaterialRegisterPluginStream(kMatFXPluginID, reinterpret_cast<RwPluginDataChunkReadCallBack>(0x811410), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x8111F0),
                                       reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(0x811970)) < 0) {
        return FALSE;
    }
    MatFXAtomicDataOffset = RpAtomicRegisterPlugin(4, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(0x811A00), reinterpret_cast<RwPluginObjectDestructor>(0x811A20),
                                                   reinterpret_cast<RwPluginObjectCopy>(0x811A40));
    if (MatFXAtomicDataOffset < 0) {
        return FALSE;
    }
    if (RpAtomicRegisterPluginStream(kMatFXPluginID, reinterpret_cast<RwPluginDataChunkReadCallBack>(&_rpMatFXAtomicStreamRead), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x811A60),
                                     reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(0x811AE0)) < 0) {
        return FALSE;
    }
    MatFXWorldSectorDataOffset = RpWorldSectorRegisterPlugin(4, kMatFXPluginID, reinterpret_cast<RwPluginObjectConstructor>(0x811B00), reinterpret_cast<RwPluginObjectDestructor>(0x811B20),
                                                             reinterpret_cast<RwPluginObjectCopy>(0x811B40));
    if (MatFXWorldSectorDataOffset < 0) {
        return FALSE;
    }
    return RpWorldSectorRegisterPluginStream(kMatFXPluginID, reinterpret_cast<RwPluginDataChunkReadCallBack>(&_rpMatFXWorldSectorStreamRead), reinterpret_cast<RwPluginDataChunkWriteCallBack>(0x811B60),
                                             reinterpret_cast<RwPluginDataChunkGetSizeCallBack>(0x811BE0)) >= 0
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

RpAtomic* RpMatFXAtomicEnableEffects(RpAtomic* atomic) {
    return ((RpAtomic*(__cdecl *)(RpAtomic*))0x811C00)(atomic);
}

// 0x811C30 (lote R8A, sin validar; cdecl, 1 argumento, `ret`, 14 bytes): lee la bandera del atomico (en atomic + 0xC9AB84).
RwBool RpMatFXAtomicQueryEffects(RpAtomic* atomic) {
    return *reinterpret_cast<RwBool*>(reinterpret_cast<RwUInt8*>(atomic) + MatFXAtomicDataOffset);
}

RpWorldSector* RpMatFXWorldSectorEnableEffects(RpWorldSector* worldSector) {
    return ((RpWorldSector*(__cdecl *)(RpWorldSector*))0x811C40)(worldSector);
}

RwBool RpMatFXWorldSectorQueryEffects(RpWorldSector* worldSector) {
    return ((RwBool(__cdecl *)(RpWorldSector*))0x811C70)(worldSector);
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

RpMaterial* RpMatFXMaterialSetupBumpMap(RpMaterial* material, RwTexture* texture, RwFrame* frame, RwReal coef) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwTexture*, RwFrame*, RwReal))0x811E00)(material, texture, frame, coef);
}

RpMaterial* RpMatFXMaterialSetupEnvMap(RpMaterial* material, RwTexture* texture, RwFrame* frame, RwBool useFrameBufferAlpha, RwReal coef) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwTexture*, RwFrame*, RwBool, RwReal))0x811ED0)(material, texture, frame, useFrameBufferAlpha, coef);
}

RpMaterial* RpMatFXMaterialSetupDualTexture(RpMaterial* material, RwTexture* texture, RwBlendFunction srcBlendMode, RwBlendFunction dstBlendMode) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwTexture*, RwBlendFunction, RwBlendFunction))0x812040)(material, texture, srcBlendMode, dstBlendMode);
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

RpMaterial* RpMatFXMaterialSetBumpMapFrame(RpMaterial* material, RwFrame* frame) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwFrame*))0x812320)(material, frame);
}

RpMaterial* RpMatFXMaterialSetBumpMapCoefficient(RpMaterial* material, RwReal coef) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwReal))0x812380)(material, coef);
}

RwTexture* RpMatFXMaterialGetBumpMapTexture(const RpMaterial* material) {
    return ((RwTexture*(__cdecl *)(const RpMaterial*))0x812430)(material);
}

RwTexture* RpMatFXMaterialGetBumpMapBumpedTexture(const RpMaterial* material) {
    return ((RwTexture*(__cdecl *)(const RpMaterial*))0x8123E0)(material);
}

RwFrame* RpMatFXMaterialGetBumpMapFrame(const RpMaterial* material) {
    return ((RwFrame*(__cdecl *)(const RpMaterial*))0x812480)(material);
}

RwReal RpMatFXMaterialGetBumpMapCoefficient(const RpMaterial* material) {
    return ((RwReal(__cdecl *)(const RpMaterial*))0x8124D0)(material);
}

RpMaterial* RpMatFXMaterialSetEnvMapTexture(RpMaterial* material, RwTexture* texture) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwTexture*))0x812530)(material, texture);
}

// 0x8125B0 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 95 bytes): guarda el marco en la ranura de entorno (bandera 2; NULL si no existe y entonces el EXE escribe en la direccion 0). Sin cuenta de
// referencias. Devuelve el material.
RpMaterial* RpMatFXMaterialSetEnvMapFrame(RpMaterial* material, RwFrame* frame) {
    FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.frame = frame;
    return material;
}

RpMaterial* RpMatFXMaterialSetEnvMapFrameBufferAlpha(RpMaterial* material, RwBool useFrameBufferAlpha) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwBool))0x812610)(material, useFrameBufferAlpha);
}

// 0x812680 (lote R8A, sin validar; cdecl, 2 argumentos, `ret`, 97 bytes): guarda el coeficiente (copiado como dword, sin pasar por la x87) en la ranura de entorno (+8; NULL si no existe y entonces
// escribe en 8). Devuelve el material.
RpMaterial* RpMatFXMaterialSetEnvMapCoefficient(RpMaterial* material, RwReal coef) {
    FindEffect(material, rpMATFXEFFECTENVMAP)->data.envMap.coef = coef;
    return material;
}

RwTexture* RpMatFXMaterialGetEnvMapTexture(const RpMaterial* material) {
    return ((RwTexture*(__cdecl *)(const RpMaterial*))0x8126F0)(material);
}

RwFrame* RpMatFXMaterialGetEnvMapFrame(const RpMaterial* material) {
    return ((RwFrame*(__cdecl *)(const RpMaterial*))0x812740)(material);
}

RwBool RpMatFXMaterialGetEnvMapFrameBufferAlpha(const RpMaterial* material) {
    return ((RwBool(__cdecl *)(const RpMaterial*))0x812790)(material);
}

RwReal RpMatFXMaterialGetEnvMapCoefficient(const RpMaterial* material) {
    return ((RwReal(__cdecl *)(const RpMaterial*))0x8127E0)(material);
}

RpMaterial* RpMatFXMaterialSetDualTexture(RpMaterial* material, RwTexture* texture) {
    return ((RpMaterial*(__cdecl *)(RpMaterial*, RwTexture*))0x812830)(material, texture);
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

RwTexture* RpMatFXMaterialGetDualTexture(const RpMaterial* material) {
    return ((RwTexture*(__cdecl *)(const RpMaterial*))0x812930)(material);
}

const RpMaterial* RpMatFXMaterialGetDualBlendModes(const RpMaterial* material, RwBlendFunction* srcBlendMode, RwBlendFunction* dstBlendMode) {
    return ((const RpMaterial*(__cdecl *)(const RpMaterial*, RwBlendFunction*, RwBlendFunction*))0x812980)(material, srcBlendMode, dstBlendMode);
}

// 0x8129E0 (lote R8A, sin validar; cdecl, 3 argumentos, `ret`, 100 bytes; sustituye la envoltura): guarda las dos matrices UV en la ranura de transformacion UV (bandera 5; NULL si no existe y entonces el EXE
// escribe en 0 y 4). Devuelve el material.
RpMaterial* RpMatFXMaterialSetUVTransformMatrices(RpMaterial* material, RwMatrix* baseTransform, RwMatrix* dualTransform) {
    MatFXEffectData* const uv = FindEffect(material, rpMATFXEFFECTUVTRANSFORM);
    uv->data.uvAnim.baseTransform = baseTransform;
    uv->data.uvAnim.dualTransform = dualTransform;
    return material;
}

const RpMaterial* RpMatFXMaterialGetUVTransformMatrices(const RpMaterial* material, RwMatrix* baseTransform, RwMatrix* dualTransform) {
    return ((const RpMaterial*(__cdecl *)(const RpMaterial*, RwMatrix*, RwMatrix*))0x812A50)(material, baseTransform, dualTransform);
}

// 0x812C90 (lote R8A, sin validar; cdecl, 2 argumentos (cabecera de la entrada de recursos y malla instanciada), `ret`, 166 bytes; llamador: 0x815E01, del callback de render 0x815C20): dibujo por defecto de una malla.
// Si hay un pixel shader activo (0x8E244C) lo quita (SetPixelShader(NULL), +0x1AC); si el vertex shader de la malla (+0x10) no es el ultimo (0x8E2448) lo cambia (+0x170); vacia los estados (0x7FC200) y dibuja:
// con bufer de indices (+8 de la cabecera) DrawIndexedPrimitive(tipo +0xC, 0, baseIndex +0x14, numVertices +0x18, startIndex +0x1C, numPrimitives +0x20; +0x148), si no DrawPrimitive(tipo, baseIndex, numPrimitives; +0x144).
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
        ComCall<kSlotDrawIndexedPrimitive, long>(Device(), static_cast<RwUInt32>(header->primType), 0, instance->baseIndex, instance->numVertices, instance->startIndex, instance->numPrimitives);
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
    RxD3D9AllInOneSetInstanceCallBack(node, reinterpret_cast<RxD3D9AllInOneInstanceCallBack>(0x8160A0));
    RxD3D9AllInOneSetRenderCallBack(node, reinterpret_cast<RxD3D9AllInOneRenderCallBack>(0x815C20));

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
    RxD3D9AllInOneSetInstanceCallBack(node, reinterpret_cast<RxD3D9AllInOneInstanceCallBack>(0x816120));
    RxD3D9AllInOneSetRenderCallBack(node, reinterpret_cast<RxD3D9AllInOneRenderCallBack>(0x815C20));

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
// la del material; si alguno de los rasters (el del material solo si existe) esta comprimido (RwD3D9RasterIsCompressed, 0x820C90) devuelve NULL; si no, crea la textura combinada con 0x810830.
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
    return reinterpret_cast<RwTexture*(__cdecl *)(RwTexture*, RwTexture*)>(kBumpedTextureCreate)(texture, bumpTexture);
}

RxPipeline* RpMatFXGetD3D9Pipeline(RpMatFXD3D9Pipeline d3d9Pipeline) {
    return ((RxPipeline*(__cdecl *)(RpMatFXD3D9Pipeline))0x8162F0)(d3d9Pipeline);
}
