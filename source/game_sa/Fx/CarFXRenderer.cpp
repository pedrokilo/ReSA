/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCarFXRenderer
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CarFXRenderer.h"
#include "CustomCarEnvMapPipeline.h"
#include "ClothesBuilder.h"

void CCarFXRenderer::InjectHooks() {
    RH_ScopedClass(CCarFXRenderer);
    RH_ScopedCategory("Fx");

    RH_ScopedInstall(RegisterPlugins, 0x5D5B00);
    RH_ScopedInstall(Initialise, 0x5D5AC0);
    RH_ScopedInstall(InitialiseDirtTexture, 0x5D5BC0);
    RH_ScopedInstall(Shutdown, 0x5D5AD0);
    RH_ScopedInstall(PreRenderUpdate, 0x5D5B10);
    RH_ScopedInstall(IsCCPCPipelineAttached, 0x5D5B80);
    RH_ScopedInstall(CustomCarPipeAtomicSetup, 0x5D5B20);
    RH_ScopedInstall(CustomCarPipeClumpSetup, 0x5D5B40);
    RH_ScopedInstall(SetCustomFXAtomicRenderPipelinesVMICB, 0x5D5B60);
    RH_ScopedInstall(SetFxEnvMapLightMult, 0x5D5BA0);
}

// 0x5D5B00
bool CCarFXRenderer::RegisterPlugins() {
    return CCustomCarEnvMapPipeline::RegisterPlugin() != 0;
}

// 0x5D5AC0
bool CCarFXRenderer::Initialise() {
    return CCustomCarEnvMapPipeline::CreatePipe() != 0;
}

// 0x5D5BC0
bool CCarFXRenderer::InitialiseDirtTexture() {
    const auto txd = CTxdStore::FindTxdSlot("vehicle");
    CTxdStore::PushCurrentTxd();
    CTxdStore::SetCurrentTxd(txd);
    auto* texture = RwTextureRead("vehiclegrunge256", nullptr);
    RwTextureSetFilterMode(texture, rwFILTERLINEAR);
    const auto width = RwRasterGetWidth(RwTextureGetRaster(texture));
    const auto height = RwRasterGetHeight(RwTextureGetRaster(texture));
    for (int32 level = 0; level < NUM_DIRT_TEXTURES; ++level) {
        auto* dirt = ms_aDirtTextures[level] = CClothesBuilder::CopyTexture(texture);
        RwTextureSetName(dirt, "vehiclegrunge256");
        auto* raster = RwTextureGetRaster(dirt);
        auto* pixels = RwRasterLock(raster, 0, rwRASTERLOCKREAD | rwRASTERLOCKWRITE);
        const auto white = 255 * (NUM_DIRT_TEXTURES - level) / NUM_DIRT_TEXTURES;
        for (int32 y = 0; y < height; ++y) {
            for (int32 x = 0; x < width; ++x) {
                auto* pixel = pixels + 4 * (y * width + x);
                for (int32 channel = 0; channel < 3; ++channel) {
                    pixel[channel] = pixel[channel] * level / NUM_DIRT_TEXTURES + white;
                }
            }
        }
        RwRasterUnlock(raster);
    }
    CTxdStore::PopCurrentTxd();
    return true;
}

// 0x5D5AD0
void CCarFXRenderer::Shutdown() {
    // Como el original: solo se destruyen las texturas existentes y la tabla queda a nulo
    for (auto& texture : ms_aDirtTextures) {
        if (texture) {
            RwTextureDestroy(texture);
            texture = nullptr;
        }
    }
    CCustomCarEnvMapPipeline::DestroyPipe();
}

// 0x5D5B10
void CCarFXRenderer::PreRenderUpdate() {
    CCustomCarEnvMapPipeline::PreRenderUpdate();
}

// 0x5D5B80
bool CCarFXRenderer::IsCCPCPipelineAttached(RpAtomic* atomic) {
    return GetPipelineID(atomic) == 0x53F2009A;
}

// 0x5D5B20
RpAtomic* CCarFXRenderer::CustomCarPipeAtomicSetup(RpAtomic* atomic) {
    return CCustomCarEnvMapPipeline::CustomPipeAtomicSetup(atomic); // Como el original: salto a 0x5DA610 (devuelve el atomic)
}

// 0x5D5B40
RpClump* CCarFXRenderer::CustomCarPipeClumpSetup(RpClump* clump) {
    RpClumpForAllAtomics(clump, SetCustomFXAtomicRenderPipelinesVMICB, nullptr);
    return clump; // Como el original: devuelve el clump
}

// 0x5D5B60
RpAtomic* CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB(RpAtomic* atomic, void* data) {
    CCustomCarEnvMapPipeline::CustomPipeAtomicSetup(atomic);
    return atomic;
}

// 0x5D5BA0
void CCarFXRenderer::SetFxEnvMapLightMult(float multiplier) {
    gSpecIntensity = multiplier;
}
