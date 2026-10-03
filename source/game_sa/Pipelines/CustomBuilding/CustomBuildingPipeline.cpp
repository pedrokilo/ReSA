/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCustomBuildingPipeline
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CustomBuildingPipeline.h"
#include <CustomBuildingDNPipeline.h>
#include <CustomCarEnvMapPipeline.h>
#include <PipelinesCommon.hpp>

constexpr auto PLUGIN_ID = 0x53F2009C;

// 0xC02C70 - matriz de textura del mapa de entorno (solo se escriben la diagonal)
static inline auto& EnvMapTexMatrix = StaticRef<D3DMATRIX>(0xC02C70);

// 0x7FE0A0 - función interna del driver D3D9 que activa/desactiva el alfa por vértice
static void _rwD3D9RenderStateVertexAlphaEnable(RwBool enable) {
    ((void(__cdecl*)(RwBool))0x7FE0A0)(enable);
}

void CCustomBuildingPipeline::InjectHooks() {
    RH_ScopedClass(CCustomBuildingPipeline);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(CreatePipe, 0x5D7D90);
    RH_ScopedInstall(DestroyPipe, 0x5D7380);
    RH_ScopedInstall(CustomPipeAtomicSetup, 0x5D7E50);
    RH_ScopedInstall(CreateCustomObjPipe, 0x5D7AA0);
    RH_ScopedInstall(CustomPipeMaterialSetup, 0x5D7DB0);
    RH_ScopedInstall(CustomPipeRenderCB, 0x5D77D0);
    RH_ScopedInstall(CustomPipeReinstanceCB, 0x5D77A0);
}

// 0x5D7D90
bool CCustomBuildingPipeline::CreatePipe() {
    ObjPipeline = CreateCustomObjPipe();
    return ObjPipeline != nullptr;
}

// 0x5D7380
void CCustomBuildingPipeline::DestroyPipe() {
    if (auto* const pipe = std::exchange(ObjPipeline, nullptr)) {
        RxPipelineDestroy(pipe);
    }
}

// 0x5D7E50
RpAtomic* CCustomBuildingPipeline::CustomPipeAtomicSetup(RpAtomic* atomic) {
    RpGeometryForAllMaterials(RpAtomicGetGeometry(atomic), CustomPipeMaterialSetup, nullptr);
    atomic->pipeline = ObjPipeline;
    SetPipelineID(atomic, PLUGIN_ID);
    return atomic;
}

// 0x5D7AA0
RxPipeline* CCustomBuildingPipeline::CreateCustomObjPipe() {
    // Como el original: la definición del nodo se pide una sola vez, justo después de crear la pipeline.
    auto* const pipe    = RxPipelineCreate();
    auto* const nodeDef = RxNodeDefinitionGetD3D9AtomicAllInOne();
    if (!pipe) {
        return nullptr;
    }

    auto* const lockedPipe = RxPipelineLock(pipe);
    if (!lockedPipe || !RxLockedPipeAddFragment(lockedPipe, 0, nodeDef, 0) || !RxLockedPipeUnlock(lockedPipe)) {
        RxPipelineDestroy(pipe);
        return nullptr;
    }

    auto* const node = RxPipelineFindNodeByName(pipe, nodeDef->name, nullptr, nullptr);
    RxD3D9AllInOneSetInstanceCallBack(node, RxD3D9AllInOneGetInstanceCallBack(node));
    RxD3D9AllInOneSetReinstanceCallBack(node, CustomPipeReinstanceCB);
    RxD3D9AllInOneSetRenderCallBack(node, CustomPipeRenderCB);

    pipe->pluginId = PLUGIN_ID;
    pipe->pluginData = PLUGIN_ID;

    return pipe;
}

// 0x5D7DB0
RpMaterial* CCustomBuildingPipeline::CustomPipeMaterialSetup(RpMaterial* material, void* data) {
    auto** const envMapData = &CCustomCarEnvMapPipeline::EnvMapPlGetData(material);

    if (RpMatFXMaterialGetEffects(material) == rpMATFXEFFECTENVMAP) {
        if (auto* const data = CCustomBuildingDNPipeline::SetFxEnvTexture(envMapData)) {
            data->Texture = (MATFXD3D9ENVMAPGETDATA(material, 0))->texture; // this macro (afaik) isn't otherwise public, I have no clue how they managed to get this working

            if (data->Texture) {
                RwTextureSetAddressing(data->Texture, rwTEXTUREADDRESSWRAP);
                RwTextureSetFilterMode(data->Texture, rwFILTERLINEAR);
            }
        }
    }

    CCustomCarEnvMapPipeline::SetMaterialFlags(
        material,
        *envMapData && (*envMapData)->Shininess != 0.f && (*envMapData)->Texture
        ? CCustomCarEnvMapPipeline::MF_HAS_SHINE_CAM
        : CCustomCarEnvMapPipeline::MF_NONE
    );

    return material;
}

// 0x5D77D0
void CCustomBuildingPipeline::CustomPipeRenderCB(RwResEntry* resEntry, void* object, RwUInt8 type, RwUInt32 rxGeoFlags) {
    const auto atomic = (RpAtomic*)(object);

    _rwD3D9EnableClippingIfNeeded(atomic, type);

    DWORD isLightingEnabled = 0;
    RwD3D9GetRenderState(D3DRS_LIGHTING, &isLightingEnabled);

    const auto geoHasNoLighting = !isLightingEnabled && !(rxGeoFlags & rxGEOMETRY_PRELIT);
    if (geoHasNoLighting) {
        RwD3D9SetTexture(NULL, 0);
        RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000);
        RwD3D9SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_TFACTOR);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
        RwD3D9SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
    }

    auto* const header = (RxD3D9ResEntryHeader*)(resEntry + 1);
    auto* const meshes = (RxD3D9InstanceData*)(header + 1);

    if (const auto i = header->indexBuffer) {
        RwD3D9SetIndices(i);
    }

    _rwD3D9SetStreams(header->vertexStream, header->useOffsets);
    RwD3D9SetVertexDeclaration(header->vertexDeclaration);

    // Como el original: el número de mallas se compara con signo (jle).
    for (int32 i = 0, numMeshes = (int32)header->numMeshes; i < numMeshes; i++) { // 0x5D78A0
        auto* const mesh       = &meshes[i];
        auto* const mat        = mesh->material;
        const auto  matFlags   = CCustomCarEnvMapPipeline::GetMaterialFlags(mat);
        const auto  envMapData = CCustomCarEnvMapPipeline::EnvMapPlGetData(mat);

        // 0x5D78B8
        RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
        RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTOP_DISABLE);

        // 0x5D78CD
        if (matFlags & CCustomCarEnvMapPipeline::MF_HAS_SHINE_CAM) {
            RwRenderStateSet(rwRENDERSTATETEXTUREADDRESS, RWRSTATE(rwTEXTUREADDRESSWRAP));

            // Como el original: guarda la escala en la diagonal de la matriz de textura global.
            const auto* const raw = reinterpret_cast<const uint8*>(envMapData);
            EnvMapTexMatrix._11 = (float)(int8)raw[0] * 0.125f;
            EnvMapTexMatrix._44 = 1.0f;
            EnvMapTexMatrix._33 = 1.0f;
            EnvMapTexMatrix._22 = (float)(int8)raw[1] * 0.125f;

            RwD3D9SetTexture(envMapData->Texture, 1);

            // Como el original: brillo (byte sin signo) · (1/255) · 254, truncado y saturado a 255;
            // el factor es opaco y repite el brillo en R, G y B.
            int32 shininess = (int32)((float)raw[4] * std::bit_cast<float>(0x3B808081u) * 254.0f);
            if (shininess > 255) {
                shininess = 255;
            }
            const uint32 shine = (uint8)shininess;
            RwD3D9SetRenderState(D3DRS_TEXTUREFACTOR, 0xFF000000u | (shine << 16) | (shine << 8) | shine);

            RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MULTIPLYADD);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG0, D3DTA_CURRENT);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_COLORARG2, D3DTA_TFACTOR);

            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
            RwD3D9SetTextureStageState(1, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

            RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1 | D3DTSS_TCI_CAMERASPACENORMAL);
            RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE);

            RwD3D9SetTextureStageState(2, D3DTSS_COLOROP, D3DTA_CURRENT);
        }

        // 0x5D79EE
        // Como el original: llama directamente a la función interna del driver (0x7FE0A0).
        _rwD3D9RenderStateVertexAlphaEnable(mesh->vertexAlpha || mesh->material->color.alpha != 0xFF);

        if (geoHasNoLighting) { // 0x5D7A12 - Render without lighting
            RxD3D9InstanceDataRender(header, mesh);
        } else { // 0x5D7A1A - Render with ligthing
            if (isLightingEnabled) {
                RwD3D9SetSurfaceProperties(&mesh->material->surfaceProps, &mesh->material->color, rxGeoFlags);
            }
            RxD3D9InstanceDataRenderLighting(header, mesh, rxGeoFlags, mesh->material->texture); // 0x5D7A41
        }
    }

    RwD3D9SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXCOORDINDEX, 1);
    RwD3D9SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, NULL);
}

// 0x5D77A0
RwBool CCustomBuildingPipeline::CustomPipeReinstanceCB(void* object, RwResEntry* resEntry, RxD3D9AllInOneInstanceCallBack instanceCallback) {
    auto* const header = (RxD3D9ResEntryHeader*)(resEntry + 1);
    // Como el original: sin callback devuelve TRUE.
    return !instanceCallback || instanceCallback(object, header, true);
}
