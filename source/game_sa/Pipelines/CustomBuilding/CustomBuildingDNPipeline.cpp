/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCustomBuildingDNPipeline
 * Reconstrucción: COMPLETA — 14/14 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 14/14 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CustomBuildingDNPipeline.h"
#include "CustomCarEnvMapPipeline.h"

auto& s_Magic1 = StaticRef<uint32>(0xC02C14);
auto& s_Magic2 = StaticRef<uint32>(0xC02C18);

// 0x5D72E0
bool CCustomBuildingDNPipeline::ExtraVertColourPluginAttach() {
    ms_extraVertColourPluginOffset = -1; // Como el original: se marca como no registrado antes de registrar
    if ((ms_extraVertColourPluginOffset = RpGeometryRegisterPlugin(
             sizeof(ExtraVertColour),
             rwID_EXTRAVERCOLOURPLUGIN,
             pluginExtraVertColourConstructorCB,
             pluginExtraVertColourDestructorCB,
             nullptr
         )) == -1
    ) {
        return false;
    }

    if (RpGeometryRegisterPluginStream(
            rwID_EXTRAVERCOLOURPLUGIN,
            pluginExtraVertColourStreamReadCB,
            pluginExtraVertColourStreamWriteCB,
            pluginExtraVertColourStreamGetSizeCB
        ) < 0 // Como el original: cualquier resultado negativo es fallo (jge)
    ) {
        ms_extraVertColourPluginOffset = -1;
        return false;
    }

    return true;
}

// 0x5D6D10
void* CCustomBuildingDNPipeline::pluginExtraVertColourConstructorCB(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    // Como el original: solo inicializa si el offset del plugin es positivo (con signo).
    if ((int32)ms_extraVertColourPluginOffset > 0) {
        const auto self = GetExtraVertColourPtr(object);

        self->NightColors = nullptr;
        self->DayColors   = nullptr;
        self->DNBalance   = 0.f;
    }

    return object;
}

// 0x5D6D30
void* CCustomBuildingDNPipeline::pluginExtraVertColourDestructorCB(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const auto self = GetExtraVertColourPtr(object);

    // Como el original: solo libera (y pone a nulo) los punteros no nulos.
    for (const auto ptr : { &self->NightColors, &self->DayColors }) {
        if (*ptr) {
            CMemoryMgr::Free(*ptr);
            *ptr = nullptr;
        }
    }

    return object;
}

// 0x5D6DE0
RwStream* CCustomBuildingDNPipeline::pluginExtraVertColourStreamReadCB(
    RwStream* stream,
    RwInt32   binaryLength,
    void*     object,
    RwInt32   offsetInObject,
    RwInt32   sizeInObject
) {
    const auto self = GetExtraVertColourPtr(object);
    const auto geo  = static_cast<RpGeometry*>(object);

    uint32 magicNumber;
    VERIFY(RwStreamRead(stream, &magicNumber, sizeof(magicNumber)) != 0);

    if (magicNumber) {
        for (const auto ptr : { &self->NightColors, &self->DayColors }) {
            *ptr = static_cast<RwRGBA*>(CMemoryMgr::Malloc(sizeof(RwRGBA) * geo->numVertices, 0));
        }
        self->DNBalance = 1.f;

        VERIFY(RwStreamRead(stream, self->NightColors, sizeof(RwRGBA) * geo->numVertices) != 0);

        // Como el original: copia con el numero de vertices con signo (no copia nada si es <= 0).
        if (const auto preLitColors = RpGeometryGetPreLightColors(geo)) {
            for (int32 i = 0; i < geo->numVertices; i++) {
                self->DayColors[i] = preLitColors[i];
            }
        }
    }

    return stream;
}

// 0x5D6D80
RwStream* CCustomBuildingDNPipeline::pluginExtraVertColourStreamWriteCB(RwStream* stream, RwInt32 binaryLength, const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const auto self = GetExtraVertColourPtr(object);
    const auto geo  = static_cast<const RpGeometry*>(object);

    // Como el original (bug del EXE; el juego nunca usa este callback de escritura): escribe los 4 primeros bytes del
    // bloque NightColors (no el puntero) y usa ese primer color como puntero del bloque que escribe a continuacion.
    RwStreamWrite(stream, self->NightColors, sizeof(self->NightColors));
    if (const auto firstColor = *reinterpret_cast<const void* const*>(self->NightColors)) {
        RwStreamWrite(stream, firstColor, sizeof(RwRGBA) * geo->numVertices);
    }
    return stream;
}

// 0x5D6DC0
RwInt32 CCustomBuildingDNPipeline::pluginExtraVertColourStreamGetSizeCB(const void* object, RwInt32 offsetInObject, RwInt32 sizeInObject) {
    const auto geo = static_cast<const RpGeometry*>(object);
    if (!geo) { // Como el original
        return 0;
    }

    return 2 * sizeof(RwRGBA) * RpGeometryGetNumVertices(geo) /*Day and Night colors*/ + sizeof(ExtraVertColour::DNBalance);
}

// 0x5D5FC0
bool AtomicHasNVCPipeline(RpAtomic* atomic) {
    return atomic->pipeline == CCustomBuildingDNPipeline::ObjPipeline;
}

// 0x5D6850 [AKA `NVC__Process`]
// Update atomic's geometry's pre-lit vertices according
void NVCPipelineProcess(RpAtomic* a, float DNBalance) {
    auto* const geo  = RpAtomicGetGeometry(a);
    auto* const data = CCustomBuildingDNPipeline::GetExtraVertColourPtr(geo);

    data->DNBalance = DNBalance;

    // Como el original: colores, numero de vertices y prelit se leen de la geometria ANTES de bloquearla.
    const auto* const night    = data->NightColors;
    const auto* const day      = data->DayColors;
    const auto        numVerts = geo->numVertices;
    auto* const       prelit   = geo->preLitLum;

    auto* const locked = RpGeometryLock(geo, rpGEOMETRYLOCKPRELIGHT);
    DNBalance = std::clamp(DNBalance, 0.f, 1.f);
    const float inv = 1.0f - DNBalance;

    // Como el original: dia*(1 - t) + noche*t por componente, redondeos float y truncado con _ftol2.
    const auto Mix = [&](uint8 d, uint8 n) { return static_cast<uint8>(static_cast<int64>((float)d * inv + (float)n * DNBalance)); };
    for (int32 i = 0; i < numVerts; i++) {
        prelit[i].red   = Mix(day[i].red, night[i].red);
        prelit[i].blue  = Mix(day[i].blue, night[i].blue);
        prelit[i].green = Mix(day[i].green, night[i].green);
        prelit[i].alpha = Mix(day[i].alpha, night[i].alpha);
    }
    RpGeometryUnlock(locked);
}

// 0x5D7200
void CCustomBuildingDNPipeline::PreRenderUpdate(RpAtomic* a, bool ignoreDNBalanceParam) {
    if (!a) {
        return;
    }
    if (!AtomicHasNVCPipeline(a)) {
        return;
    }
    const auto intensity = std::abs(GetExtraVertColourPtr(RpAtomicGetGeometry(a))->DNBalance - m_fDNBalanceParam);
    // Como el original: comparaciones "!(x > c)" (con NaN se sale igual que con valores pequenos).
    if (!(intensity > 0.01f) && !ignoreDNBalanceParam) {
        return;
    }
    if (((uint32)a / 0x70) % 16 != s_Magic1 && !s_Magic2 && !ignoreDNBalanceParam && !(intensity > 0.3f)) {
        return;
    }
    NVCPipelineProcess(a, m_fDNBalanceParam);
}

// 0x5D72C0
void CCustomBuildingDNPipeline::PreRenderUpdate(RpClump* clump, bool ignoreDNBalanceParam) {
    RpClumpForAllAtomics(clump, [](RpAtomic* atomic, void* data) { // 0x5D72A0
        CCustomBuildingDNPipeline::PreRenderUpdate(atomic, *(bool*)(data));
        return atomic;
    }, &ignoreDNBalanceParam);
}

// 0x5D7100
bool CCustomBuildingDNPipeline::CreatePipe() {
    ObjPipeline = CreateCustomObjPipe();
    return ObjPipeline != nullptr;
}

// 0x5D6750
RxPipeline* CCustomBuildingDNPipeline::CreateCustomObjPipe() {
    auto pipeline = RxPipelineCreate();
    auto nodeDef  = RxNodeDefinitionGetD3D9AtomicAllInOne();
    if (!pipeline) {
        return nullptr;
    }

    auto lockedPipe = RxPipelineLock(pipeline);
    if (!lockedPipe || !RxLockedPipeAddFragment(lockedPipe, 0, nodeDef, 0) || !RxLockedPipeUnlock(lockedPipe)) {
        RxPipelineDestroy(pipeline);
        return nullptr;
    }

    auto node = RxPipelineFindNodeByName(pipeline, nodeDef->name, nullptr, nullptr);
    if (*reinterpret_cast<int32*>(0xC02C20)) { // read only (0 by default)
        RxD3D9AllInOneSetInstanceCallBack(node, RxD3D9AllInOneGetInstanceCallBack(node));
        RxD3D9AllInOneSetReinstanceCallBack(node, CustomPipeInstanceCB);
    } else {
        RxD3D9AllInOneSetInstanceCallBack(node, RxD3D9AllInOneGetInstanceCallBack(node));
        RxD3D9AllInOneSetReinstanceCallBack(node, RxD3D9AllInOneGetReinstanceCallBack(node));
    }
    RxD3D9AllInOneSetRenderCallBack(node, CustomPipeRenderCB);

    // Como el original: 0x5D6801 guarda 0x53F20098 en pluginId y pluginData.
    pipeline->pluginId   = 0x53F20098;
    pipeline->pluginData = 0x53F20098;

    return pipeline;
}

// 0x5D5FA0
void CCustomBuildingDNPipeline::DestroyPipe() {
    if (ObjPipeline) {
        RxPipelineDestroy(ObjPipeline);
        ObjPipeline = nullptr; // Como el original: se anula despues de destruir
    }
}

// 0x5D63E0
RwBool CCustomBuildingDNPipeline::CustomPipeInstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback) {
    if (instanceCallback) {
        auto entry = *reinterpret_cast<RwResEntrySA*>(resEntry);
        return instanceCallback(object, &entry.header, true);
    }
    return true;
}

// 0x5D71C0
RpAtomic* CCustomBuildingDNPipeline::CustomPipeAtomicSetup(RpAtomic* atomic) {
    RpGeometryForAllMaterials(atomic->geometry, CustomPipeMaterialSetup, nullptr);
    RpD3D9GeometrySetUsageFlags(atomic->geometry, rpD3D9GEOMETRYUSAGE_DYNAMICPRELIT);
    atomic->pipeline = ObjPipeline;
    SetPipelineID(atomic, 0x53F20098); // Native per-atomic ID at 0x5D71E3.
    return atomic;
}

// android, unused
uint32 CCustomBuildingDNPipeline::UsesThisPipeline(RpAtomic* atomic) {
    return atomic->pipeline
        - ObjPipeline
        + (atomic->pipeline == ObjPipeline) // AtomicHasNVCPipeline	0x5D5FC0
        + ObjPipeline
        - atomic->pipeline;
}

// 0x5D7120
RpMaterial* CCustomBuildingDNPipeline::CustomPipeMaterialSetup(RpMaterial* material, void* data) {
    return plugin::CallAndReturn<RpMaterial*, 0x5D7120, RpMaterial*, void*>(material, data);
}

// 0x5D6480
void CCustomBuildingDNPipeline::CustomPipeRenderCB(RwResEntry* entry, void* object, uint8 type, uint32 flags) {
    plugin::Call<0x5D6480, RwResEntry*, void*, uint8, uint32>(entry, object, type, flags);
}

// 0x5D6E90
ExtraVertColour* CCustomBuildingDNPipeline::GetExtraVertColourPtr(const void* geometry) {
    return RWPLUGINOFFSET(ExtraVertColour, geometry, ms_extraVertColourPluginOffset);
}

// 0x5D9570
// Como el original: CPool::New del EXE para este pool (0x5D9110). Busca desde m_LastFreeSlot + 1, da como mucho una
// vuelta (si se llena deja m_LastFreeSlot = 0) y no rellena el hueco; la plantilla CPool::New de la DLL empieza en
// m_LastFreeSlot y rellena con 0xCD.
static CustomEnvMapPipeMaterialData* NewEnvMapPipeMatData(CustomEnvMapPipeMaterialDataPool* pool) {
    auto* const raw     = reinterpret_cast<uint8*>(pool);
    auto* const storage = *reinterpret_cast<CustomEnvMapPipeMaterialData**>(raw + 0x0);
    auto* const states  = *reinterpret_cast<uint8**>(raw + 0x4);
    const auto  size    = *reinterpret_cast<int32*>(raw + 0x8);
    auto&       last    = *reinterpret_cast<int32*>(raw + 0xC);
    bool wrapped = false;
    do {
        if (++last == size) {
            last = 0;
            if (wrapped) {
                return nullptr;
            }
            wrapped = true;
        }
    } while (!(states[last] & 0x80));
    states[last] = uint8((states[last] + 1) & 0x7F); // ocupado (bit 7 = 0) y ref + 1 en 7 bits
    return &storage[last];
}

CustomEnvMapPipeMaterialData* CCustomBuildingDNPipeline::SetFxEnvTexture(CustomEnvMapPipeMaterialData** inOutEnvMapData) {
    if (*inOutEnvMapData == &CCustomCarEnvMapPipeline::fakeEnvMapPipeMatData) {
        if (*inOutEnvMapData = NewEnvMapPipeMatData(CCustomCarEnvMapPipeline::m_gEnvMapPipeMatDataPool)) {
            // Como el original: copia los 12 bytes con dwords (incluido el byte de relleno 5).
            std::memcpy(*inOutEnvMapData, &CCustomCarEnvMapPipeline::fakeEnvMapPipeMatData, sizeof(CustomEnvMapPipeMaterialData));
        }
    }
    return *inOutEnvMapData;
}

RwTexture* CCustomBuildingDNPipeline::GetFxEnvTexture(RpMaterial* material) {
    return CCustomCarEnvMapPipeline::GetFxEnvTexture(material);
}

void CCustomBuildingDNPipeline::SetFxEnvScale(RpMaterial* material, float x, float y) {
    CCustomCarEnvMapPipeline::SetFxEnvScale(material, x, y);
}

float CCustomBuildingDNPipeline::GetFxEnvScaleX(RpMaterial* material) {
    return CCustomCarEnvMapPipeline::GetFxEnvScaleX(material);
}

float CCustomBuildingDNPipeline::GetFxEnvScaleY(RpMaterial* material) {
    return CCustomCarEnvMapPipeline::GetFxEnvScaleY(material);
}

void CCustomBuildingDNPipeline::SetFxEnvTransScl(RpMaterial* material, float x, float y) {
    CCustomCarEnvMapPipeline::SetFxEnvTransScl(material, x, y);
}

float CCustomBuildingDNPipeline::GetFxEnvTransSclX(RpMaterial* material) {
    return CCustomCarEnvMapPipeline::GetFxEnvTransSclX(material);
}

float CCustomBuildingDNPipeline::GetFxEnvTransSclY(RpMaterial* material) {
    return CCustomCarEnvMapPipeline::GetFxEnvTransSclY(material);
}

void CCustomBuildingDNPipeline::SetFxEnvShininess(RpMaterial* material, float value) {
    CCustomCarEnvMapPipeline::SetFxEnvShininess(material, value);
}

float CCustomBuildingDNPipeline::GetFxEnvShininess(RpMaterial* material) {
    return CCustomCarEnvMapPipeline::GetFxEnvShininess(material);
}

void CCustomBuildingDNPipeline::InjectHooks() {
    RH_ScopedClass(CCustomBuildingDNPipeline);
    RH_ScopedCategory("Pipelines");

    RH_ScopedInstall(ExtraVertColourPluginAttach, 0x5D72E0);
    RH_ScopedInstall(pluginExtraVertColourConstructorCB, 0x5D6D10);
    RH_ScopedInstall(pluginExtraVertColourDestructorCB, 0x5D6D30);
    RH_ScopedInstall(pluginExtraVertColourStreamReadCB, 0x5D6DE0);
    RH_ScopedInstall(pluginExtraVertColourStreamWriteCB, 0x5D6D80);
    RH_ScopedInstall(pluginExtraVertColourStreamGetSizeCB, 0x5D6DC0);
    RH_ScopedOverloadedInstall(PreRenderUpdate, "Atomic", 0x5D7200, void(*)(RpAtomic*, bool));
    RH_ScopedOverloadedInstall(PreRenderUpdate, "Clump", 0x5D72C0, void(*)(RpClump*, bool));
    RH_ScopedGlobalInstall(NVCPipelineProcess, 0x5D6850);
    RH_ScopedInstall(CreatePipe, 0x5D7100);
    RH_ScopedInstall(DestroyPipe, 0x5D5FA0);
    RH_ScopedInstall(CreateCustomObjPipe, 0x5D6750);
    RH_ScopedInstall(CustomPipeAtomicSetup, 0x5D71C0);
    RH_ScopedInstall(SetFxEnvTexture, 0x5D9570);
}
