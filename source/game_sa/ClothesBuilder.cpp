/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CClothesBuilder
 * Reconstrucción: COMPLETA — 23/23 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 23/23 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <extensions/ci_string.hpp>

#include "ClothesBuilder.h"
#include "PedClothesDesc.h"

auto& playerImg = StaticRef<CDirectory>(0xBC12C0);
auto& playerImgEntries = StaticRef<CDirectory::DirectoryInfo>(0xBBCDC8);

auto& gBoneIndices = StaticRef<notsa::mdarray<int16, 10, 64>>(0xBBC8C8);

auto& ms_ratiosHaveChanged  = StaticRef<bool>(0x8D0AA4);
auto& ms_geometryHasChanged = StaticRef<bool>(0x8D0AA5);
auto& ms_textureHasChanged  = StaticRef<bool>(0x8D0AA6);

void CClothesBuilder::InjectHooks() {
    RH_ScopedClass(CClothesBuilder);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(LoadCdDirectory, 0x5A4190);
    RH_ScopedInstall(RequestGeometry, 0x5A41C0);
    RH_ScopedInstall(RequestTexture, 0x5A4220);
    //RH_ScopedInstall(nullptr, 0x5A42B0, { .reversed = false }); 
    //RH_ScopedInstall(nullptr, 0x5A4380, { .reversed = false }); AtomicInstanceCB
    //RH_ScopedInstall(nullptr, 0x5A43A0, { .reversed = false });
    //RH_ScopedInstall(nullptr, 0x5A44A0, { .reversed = false }); DestroyTextureCB
    RH_ScopedInstall(PreprocessClothesDesc, 0x5A44C0);
    RH_ScopedInstall(ReleaseGeometry, 0x5A47B0);
    RH_ScopedGlobalInstall(GetAtomicWithName, 0x5A4810);
    RH_ScopedInstall(AddWeightToBoneVertex, 0x5A4840);
    RH_ScopedInstall(StoreBoneArray, 0x5A48B0);
    RH_ScopedOverloadedInstall(BlendGeometry, "3", 0x5A4940, RpGeometry * (*)(RpClump*, const char*, const char*, const char*, float, float, float));
    RH_ScopedOverloadedInstall(BlendGeometry, "2", 0x5A4F10, RpGeometry* (*)(RpClump*, const char*, const char*, float, float));
    RH_ScopedInstall(CopyGeometry, 0x5A5340);
    RH_ScopedInstall(ConstructGeometryArray, 0x5A55A0); // Makes the game crash - Probably a register is changed or smth
    RH_ScopedInstall(DestroySkinArrays, 0x5A56C0);
    RH_ScopedInstall(BuildBoneIndexConversionTable, 0x5A56E0);
    RH_ScopedInstall(CopyTexture, 0x5A5730);
    RH_ScopedInstall(PlaceTextureOnTopOfTexture, 0x5A57B0);
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src", 0x5A5820, void (*)(RwTexture*, RwTexture*, float, float, int32));
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src1-Src2", 0x5A59C0, void (*)(RwTexture*, RwTexture*, RwTexture*, float, float, float, int32));
    RH_ScopedOverloadedInstall(BlendTextures, "Dst-Src1-Src2-Tat", 0x5A5BC0, void (*)(RwTexture*, RwTexture*, RwTexture*, float, float, float, int32, RwTexture*));
    RH_ScopedGlobalInstall(GetTextureFromTxdAndLoadNextTxd, 0x5A5F70);
    RH_ScopedInstall(ConstructTextures, 0x5A6040);
    RH_ScopedInstall(ConstructGeometryAndSkinArrays, 0x5A6530);
    RH_ScopedInstall(CreateSkinnedClump, 0x5A69D0);
}

// inlined
// 0x5A4190
void CClothesBuilder::LoadCdDirectory() {
    playerImg.Init(550, &playerImgEntries);
    playerImg.ReadDirFile("MODELS\\PLAYER.IMG");
}

// 0x5A41C0
void CClothesBuilder::RequestGeometry(int32 modelId, uint32 modelNameKey) {
    CModelInfo::GetModelInfo(modelId)->bHasComplexHierarchy = true; // TODO/NOTE: Not sure

    uint32 size;
    CdStreamPos pos;
    VERIFY(playerImg.FindItem(CKeyGen::AppendStringToKey(modelNameKey, ".DFF"), pos, size));
    CStreaming::RequestFile(modelId, pos, size, CClothes::ms_clothesImageId, STREAMING_PRIORITY_REQUEST | STREAMING_GAME_REQUIRED);
}

// 0x5A4220
int32 CClothesBuilder::RequestTexture(uint32 txdNameKey) {
    if (txdNameKey == 0) {
        return -1;
    }

    auto& defaultTxdIdx = StaticRef<uint32>(0xBC12D0);
    const auto defaultTxd = CTxdStore::defaultTxds[defaultTxdIdx];
    defaultTxdIdx = (defaultTxdIdx + 1) % 4;

    uint32 size;
    CdStreamPos pos;
    VERIFY(playerImg.FindItem(CKeyGen::AppendStringToKey(txdNameKey, ".TXD"), pos, size));
    CStreaming::RequestFile(TXDToModelId(defaultTxd), pos, size, CClothes::ms_clothesImageId, STREAMING_PRIORITY_REQUEST | STREAMING_GAME_REQUIRED);

    return defaultTxd;
}

// 0x5A44C0
void CClothesBuilder::PreprocessClothesDesc(CPedClothesDesc& desc, bool a2) {
    std::array<int32, 8> keys;
    keys.fill(-1);
    std::array<bool, 8> ignore{};
    uint32 cursor = 0;
    uint32 key = 0, replacement = 0, texture = 0;
    while (cursor < CClothes::ms_numRuleTags) {
        const auto tag = CClothes::ms_clothesRules[cursor++];
        int32 target = -1;
        switch (tag) {
        case 0:
            key = CClothes::ms_clothesRules[cursor++];
            replacement = CClothes::ms_clothesRules[cursor++];
            break;
        case 1:
            key = CClothes::ms_clothesRules[cursor++];
            target = int32(CClothes::ms_clothesRules[cursor++]);
            replacement = CClothes::ms_clothesRules[cursor++];
            texture = CClothes::ms_clothesRules[cursor++];
            break;
        case 2:
            key = CClothes::ms_clothesRules[cursor++];
            texture = CClothes::ms_clothesRules[cursor++];
            break;
        case 3:
            key = CClothes::ms_clothesRules[cursor++];
            target = int32(CClothes::ms_clothesRules[cursor++]);
            break;
        case 4: case 6:
            key = CClothes::ms_clothesRules[cursor++];
            for (size_t i = 0; i < keys.size(); ++i) {
                if (keys[i] == int32(key)) {keys[i] = -1; ignore[i] = tag == 4; break;}
            }
            break;
        case 5: case 7:
            for (size_t i = 0; i < keys.size(); ++i) {
                if (keys[i] == -1) {keys[i] = int32(CClothes::ms_clothesRules[cursor++]); ignore[i] = tag == 5; break;}
            }
            break;
        }
        for (int32 part = 0; part < 10; ++part) {
            const auto tested = target == -1 ? part : target;
            bool blocked = false;
            for (size_t i = 0; i < keys.size(); ++i) {
                if (keys[i] != -1 && (desc.m_anModelKeys[tested] == uint32(keys[i])) == ignore[i]) {blocked = true; break;}
            }
            if (blocked || desc.m_anModelKeys[part] != key) continue;
            switch (tag) {
            case 0:
                if (a2) desc.SetModel(replacement, static_cast<eClothesModelPart>(part));
                break;
            case 1:
                if (target == 2) {desc.SetModel(replacement, static_cast<eClothesModelPart>(2)); break;}
                if (texture == 0) texture = desc.m_anTextureKeys[CClothes::GetDependentTexture(static_cast<eClothesModelPart>(target))];
                if (replacement == 0) replacement = desc.m_anModelKeys[target];
                desc.SetTextureAndModel(texture, replacement, CClothes::GetDependentTexture(static_cast<eClothesModelPart>(target)));
                break;
            case 2:
                desc.SetTextureAndModel(texture, key, CClothes::GetDependentTexture(static_cast<eClothesModelPart>(part)));
                break;
            case 3:
                desc.SetModel(uint32(0), static_cast<eClothesModelPart>(target));
                break;
            }
        }
    }
}

// unused
// 0x5A47B0
void CClothesBuilder::ReleaseGeometry(int32 numToRelease) {
    for (auto i = numToRelease; i; CStreaming::SetModelIsDeletable(MODEL_CLOTHES01_ID384 + i))
        --i;
}

// 0x5A4810
RpAtomic* GetAtomicWithName(RpClump* clump, const char* name) {
    struct Context {
        notsa::ci_string_view name{};
        RpAtomic*             atomic{};
    } c{name};
    RpClumpForAllAtomics(clump, [](RpAtomic* a, void* data) -> RpAtomic* { // 0x5A47E0
        auto& ctx = *static_cast<Context*>(data);
        if (ctx.name == GetFrameNodeName(RpAtomicGetFrame(a))) {
            ctx.atomic = a;
            return nullptr; // Como el original (0x5A4801 jne no tomado -> eax=0): detiene la iteracion en la PRIMERA coincidencia
        }
        return a;
    }, &c);
    return c.atomic;
}

// 0x5A4840
void CClothesBuilder::AddWeightToBoneVertex(float (&weights)[8], uint8(&boneVertexIdxs)[8], float weightToAdd, RwUInt32 targetVertexIdx) { // Unknown OG name
    if (weightToAdd == 0.f) {
        return;
    }
    for (auto i = 0; i < 8; i++) {
        if (weights[i] == 0.f) { // Weight not yet used?
            boneVertexIdxs[i] = (uint8)targetVertexIdx; // Add to list
            (&weights[0])[i + 1] = 0.f;                 // Como el original (0x5A488F): marca el siguiente como libre (escribe weights[8] si i==7)
            weights[i] += weightToAdd;                  // Como el original (0x5A489C fadd [edx+ecx*4])
            return;
        }
        if (boneVertexIdxs[i] == targetVertexIdx) { // Already in the list, use that
            weights[i] += weightToAdd;
            return;
        }
    }
    // Como el original (0x5A487F-0x5A4888): si las 8 ranuras estan ocupadas con otros indices, suma a weights[8] (fuera de rango)
    (&weights[0])[8] += weightToAdd;
}

// 0x5A48B0
void CClothesBuilder::StoreBoneArray(RpClump* clump, int32 idx) {
    const auto a = GetAtomicWithName(clump, "normal");
    assert(a);

    const auto h = RpSkinAtomicGetHAnimHierarchy(a);
    assert(h);
    
    // Como el original (0x5A48EF-0x5A4923): copia hacia delante sin limite, y solo escribe UN -1 de cierre si i < 64 (no rellena la fila)
    auto* row = reinterpret_cast<int16*>(0xBBC8C8) + idx * 64;
    int32 i = 0;
    for (; i < h->numNodes; ++i) {
        row[i] = static_cast<int16>(h->pNodeInfo[i].nodeID);
    }
    if (i < 64) {
        row[i] = -1;
    }
}

/*
* @notsa
* 
* Based on 0x5A4940
* Blend any number of geometries together
* The result is stored in the 0th frame's geometry.
*
* @arg clump  The clump to which the frames belong to
* @arg frames A list of frames whose geometry should be blended together
*/
// Como el original (0x5A4940/0x5A4F10): x87 a 24 bits, cada fmul/fadd redondea a float; precise impide reasociar.
#pragma float_control(precise, on, push)
static float ClothesBlendPair(float a, float b, float ra, float rb) {
    const float tb = b * rb;
    const float ta = a * ra;
    return tb + ta;
}

static float ClothesBlendTriple(float a, float b, float c, float ra, float rb, float rc) {
    const float ta = a * ra;
    const float tb = b * rb;
    const float s = ta + tb;
    const float tc = c * rc;
    return s + tc;
}

static void ClothesNormalizeWeights(float* weights) {
    // Suma 3+2+1+0, inversa 1/suma y producto in situ, como en 0x5A4E1B
    const float sum = ((weights[3] + weights[2]) + weights[1]) + weights[0];
    const float inv = 1.0f / sum;
    weights[0] = weights[0] * inv;
    weights[1] = weights[1] * inv;
    weights[2] = weights[2] * inv;
    weights[3] = weights[3] * inv;
}
#pragma float_control(pop)

template<size_t N>
RpGeometry* BlendGeometry(RpClump* clump, std::pair<const char*, float> (&&frameNamesRatios)[N]) {

    // Process data needed for blending
    struct GeoBlendData {
        RpAtomic*        a;
        RpGeometry*      g;
        const RwUInt8*   boneIdxs;
        RwMatrixWeights* boneWeights;
        RwTexCoords*     uvs;
        CVector*         verts;
        CVector*         nrmls;
        float            r; // Blend ratio
    } fds[N];
    auto& out = fds[0];
    for (auto&& [i, v] : rngv::enumerate(frameNamesRatios)) {
        const auto a  = GetAtomicWithName(clump, v.first);
        const auto g  = RpAtomicGetGeometry(a);
        const auto s  = RpSkinGeometryGetSkin(g);
        const auto mt = RpGeometryGetMorphTarget(g, 0);
        fds[i] = {
            a,
            g,
            (const RwUInt8*)RpSkinGetVertexBoneIndices(s), // NOTE: Not sure why it's casted to UInt8, but that really is how the data is stored / TODO: Okay, so actually it seems like something is fucked
            RpSkinGetVertexBoneWeights(s),
            RpGeometryGetVertexTexCoords(g, 1),
            (CVector*)RpMorphTargetGetVertices(mt),
            (CVector*)RpMorphTargetGetVertexNormals(mt),
            v.second
        };
    }

    RpGeometryLock(out.g, rpGEOMETRYLOCKALL);

    uint8 carriedBoneIndices[8]{};
    if constexpr (N == 2) {
        // The two-frame routine reuses the atomic-pointer scratch area for
        // bone indices and retains it between vertices, including zero weights.
        const uint32 atomicBits = uint32(reinterpret_cast<uintptr_t>(out.a));
        memcpy(carriedBoneIndices, &atomicBits, sizeof(atomicBits));
    }

    for (auto i = 0; i < RpGeometryGetNumVertices(out.g); i++) {
        const auto BlendComponent = [&](auto member, int component, int first, int second, int third) {
            const auto value = [&](int index) {
                const auto* values = reinterpret_cast<const float*>(fds[index].*member);
                const int stride = std::is_same_v<decltype(member), RwTexCoords* GeoBlendData::*> ? 2 : 3;
                return values[i * stride + component];
            };
            if constexpr (N == 3) {
                return ClothesBlendTriple(value(first), value(second), value(third), fds[first].r, fds[second].r, fds[third].r);
            } else {
                return ClothesBlendPair(value(0), value(1), fds[0].r, fds[1].r);
            }
        };
        for (int component = 0; component < 3; ++component) {
            reinterpret_cast<float*>(&out.verts[i])[component] = BlendComponent(&GeoBlendData::verts, component, 1, component ? 2 : 0, component ? 0 : 2);
            reinterpret_cast<float*>(&out.nrmls[i])[component] = BlendComponent(&GeoBlendData::nrmls, component, component ? 1 : 2, component ? 2 : 1, 0);
        }
        RwV3dNormalize(reinterpret_cast<RwV3d*>(&out.nrmls[i]), reinterpret_cast<RwV3d*>(&out.nrmls[i]));
        for (int component = 0; component < 2; ++component) {
            reinterpret_cast<float*>(&out.uvs[i])[component] = BlendComponent(&GeoBlendData::uvs, component, component ? 1 : 2, component ? 2 : 1, 0);
        }

        // Helper function to get the weight at a given weight index
        const auto RwMatrixWeightsGetWeight = [](RwMatrixWeights& mw, int32 wi) -> float& {
            switch (wi) {
            case 0:  return mw.w0;
            case 1:  return mw.w1;
            case 2:  return mw.w2;
            case 3:  return mw.w3;
            default: NOTSA_UNREACHABLE();
            }
        };

        // Calculate bone weights
        float weights[8]{};
        uint8 boneVertexIdxs[8]{};
        if constexpr (N == 2) memcpy(boneVertexIdxs, carriedBoneIndices, sizeof(carriedBoneIndices));
        for (auto& fd : fds) {
            for (auto wi = 0; wi < 4; wi++) {
                const auto vtxIdx = i * 4 + wi;
                CClothesBuilder::AddWeightToBoneVertex(
                    weights,
                    boneVertexIdxs,
                    float(double(RwMatrixWeightsGetWeight(fd.boneWeights[i], wi)) * double(fd.r)),
                    fd.boneIdxs[vtxIdx]
                );
            }
        }
        for (auto b = 0; b < 4; b++) {
            const_cast<RwUInt8*>(out.boneIdxs)[i * 4 + b] = boneVertexIdxs[b];
        }
        if constexpr (N == 2) memcpy(carriedBoneIndices, boneVertexIdxs, sizeof(carriedBoneIndices));
        if (weights[4] != 0.f) ClothesNormalizeWeights(weights);
        for (auto wi = 0; wi < 4; wi++) {
            RwMatrixWeightsGetWeight(out.boneWeights[i], wi) = weights[wi];
        }
    }

    RpGeometryUnlock(out.g);
    out.g->refCount++; // TODO: Function missing

    return out.g;
}

// 0x5A4940
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, const char* frameName2, float r0, float r1, float r2) {
    return ::BlendGeometry(clump, { {frameName0, r0}, {frameName1, r1}, {frameName2, r2} });
}

// 0x5A4F10
RpGeometry* CClothesBuilder::BlendGeometry(RpClump* clump, const char* frameName0, const char* frameName1, float r0, float r1) {
    return ::BlendGeometry(clump, { {frameName0, r0}, {frameName1, r1} });
}

// 0x5A5340
RpGeometry* CClothesBuilder::CopyGeometry(RpClump* clump, const char* a2, const char* a3) {
    auto* destination = RpAtomicGetGeometry(GetAtomicWithName(clump, a2));
    auto* source = RpAtomicGetGeometry(GetAtomicWithName(clump, a3));
    auto* dstSkin = RpSkinGeometryGetSkin(destination);
    auto* srcSkin = RpSkinGeometryGetSkin(source);
    auto* dstBones = const_cast<RwUInt32*>(RpSkinGetVertexBoneIndices(dstSkin));
    auto* srcBones = RpSkinGetVertexBoneIndices(srcSkin);
    auto* dstWeights = RpSkinGetVertexBoneWeights(dstSkin);
    auto* srcWeights = RpSkinGetVertexBoneWeights(srcSkin);
    auto* dstMorph = RpGeometryGetMorphTarget(destination, 0);
    auto* srcMorph = RpGeometryGetMorphTarget(source, 0);
    RpGeometryLock(destination, rpGEOMETRYLOCKALL);
    for (int32 i = 0; i < RpGeometryGetNumVertices(destination); ++i) {
        RpMorphTargetGetVertices(dstMorph)[i] = RpMorphTargetGetVertices(srcMorph)[i];
        RpMorphTargetGetVertexNormals(dstMorph)[i] = RpMorphTargetGetVertexNormals(srcMorph)[i];
        RpGeometryGetVertexTexCoords(destination, 1)[i] = RpGeometryGetVertexTexCoords(source, 1)[i];
        dstBones[i] = srcBones[i];
        dstWeights[i] = srcWeights[i];
    }
    RpGeometryUnlock(destination);
    // RenderWare's 0x74CCB0 only increments this reference count.
    ++destination->refCount;
    return destination;
}

// 0x5A55A0
void CClothesBuilder::ConstructGeometryArray(RpGeometry** out, uint32* modelNameKeys, float normal, float fatness, float strength) {
    for (auto i = 0; i < 10; i++) {
        if (modelNameKeys[i] == 0) {
            out[i] = nullptr;
            continue;
        }
        const auto modelIdx = (eModelID)((int)MODEL_CLOTHES01_ID384 + i);

        CModelInfo::GetModelInfo(modelIdx)->bHasComplexHierarchy = true;
        RequestGeometry(modelIdx, modelNameKeys[i]);
        CStreaming::LoadAllRequestedModels(true);

        if (i + 1 < 10 && modelNameKeys[i + 1]) { // Request next model to be loaded in advance
            RequestGeometry((eModelID)((int)MODEL_CLOTHES01_ID384 + i + 1), modelNameKeys[i + 1]);
            CStreaming::LoadRequestedModels();
        }

        const auto mi = CModelInfo::GetModelInfo(modelIdx);
        out[i] = BlendGeometry(mi->GetRpClump(), "normal", "fat", "ripped", normal, fatness, strength);
        StoreBoneArray(mi->GetRpClump(), i);
        CStreaming::RemoveModel(modelIdx);
    }
}

// inlined, see 0x5A6CE1
// 0x5A56C0
void CClothesBuilder::DestroySkinArrays(RwMatrixWeights* weights, RwUInt32* bones) {
    // TODO: Should this be `delete[]` or `delete`?
    delete weights;
    delete bones;
}

// 0x5A56E0
void CClothesBuilder::BuildBoneIndexConversionTable(uint8* pTable, RpHAnimHierarchy* hier, int32 index) {
    for (const auto [tableIdx, boneId] : rngv::enumerate(gBoneIndices[index])) {
        if (boneId == -1) {
            break;
        }
        const auto idx = (uint8)RpHAnimIDGetIndex(hier, boneId); // Como el original (0x5A5712 cmp al,0FFh): compara solo el byte bajo (-1 -> 0)
        pTable[tableIdx] = idx == 0xFF ? 0 : idx;
    }
}

void AssertTextureLayouts(std::initializer_list<RwTexture*> textures) {
    assert(textures.size() >= 2);
    for (auto i = 0u; i < textures.size() - 1; i++) {
        const auto r1 = RwTextureGetRaster(textures.begin()[i]), r2 = RwTextureGetRaster(textures.begin()[i + 1]);

        assert(RwRasterGetWidth(r1) == RwRasterGetWidth(r2));
        assert(RwRasterGetHeight(r1) == RwRasterGetHeight(r2));
        assert(RwRasterGetDepth(r1) == RwRasterGetDepth(r2));
        assert(RwRasterGetDepth(r1) == 32);
    }
}

// 0x5A5730
RwTexture* CClothesBuilder::CopyTexture(RwTexture* srcTex) {
    const auto srcRaster = RwTextureGetRaster(srcTex);
    
    // Create a new raster to which we're going to copy to
    const auto dstRaster = RwRasterCreate(
        RwRasterGetWidth(srcRaster),
        RwRasterGetHeight(srcRaster),
        RwRasterGetDepth(srcRaster),
        (RwRasterGetFormat(srcRaster) & 0x6F00) | 4 // Como el original (0x5A5742 and eax,0FFFFFF6Fh ; shl 8 ; or 4)
    );
    
    // Copy data from the src raster to this one
    const auto srcLck = RwRasterLock(srcRaster, 0, rwRASTERLOCKREAD);
    memcpy(
        RwRasterLock(dstRaster, 0, rwRASTERLOCKWRITE),
        srcLck,
        RwRasterGetHeight(srcRaster) * RwRasterGetStride(srcRaster)
    );
    RwRasterUnlock(srcRaster);
    RwRasterUnlock(dstRaster);

    // Create a texture from the copied raster
    const auto dstTex = RwTextureCreate(dstRaster);
    RwTextureSetFilterMode(dstTex, rwFILTERLINEAR);

    AssertTextureLayouts({ dstTex, srcTex });

    return dstTex;
}

// 0x5A57B0
void CClothesBuilder::PlaceTextureOnTopOfTexture(RwTexture* dstTex, RwTexture* srcTex) {
    ZoneScoped;

    AssertTextureLayouts({ dstTex, srcTex });

    const auto dstRaster = RwTextureGetRaster(dstTex);
    const auto srcRaster = RwTextureGetRaster(srcTex);

    // Como el original (0x5A57C4/0x5A57D0): primero se bloquea el ORIGEN y luego el destino, ambos con modo 3
    auto srcIt = (RwUInt32*)RwRasterLock(srcRaster, 0, rwRASTERLOCKREADWRITE);
    auto dstIt = (RwUInt32*)RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);

    // Como el original (0x5A57D5): el numero de pixeles sale del raster ORIGEN
    for (auto i = RwRasterGetHeight(srcRaster) * RwRasterGetWidth(srcRaster); i-- > 0; dstIt++, srcIt++) {
        if (*srcIt & 0xFF000000) { // Check alpha != 0
            *dstIt = *srcIt;
        }
    }

    // Como el original (0x5A5804): se desbloquea primero el origen
    RwRasterUnlock(srcRaster);
    RwRasterUnlock(dstRaster);
}

// Como el original (_ftol2 0x821B40 + 0x5A58C5 cmp al,0FFh ; ja): al nunca supera 0xFF, asi que el resultado es el byte bajo del entero de 64 bits (sin saturar)
static RwUInt8 ClothesBlendByte(float v) {
    return (RwUInt8)(int64)v;
}

// 0x5A5820
void CClothesBuilder::BlendTextures(RwTexture* dst, RwTexture* src, float r1, float r2, int32 numColors) {
    ZoneScoped;

    // Como el original (0x5A582D-0x5A5830): ebp = raster del ORIGEN, esi = raster del destino
    const auto srcRaster = RwTextureGetRaster(src);
    const auto dstRaster = RwTextureGetRaster(dst);

    CTimer::Suspend();

    auto srcIt = RwRasterLock(srcRaster, 0, rwRASTERLOCKREAD);      // 0x5A583B-0x5A5840
    auto dstIt = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE); // 0x5A5845-0x5A584C

    // Como el original: el numero de pixeles sale del raster ORIGEN (0x5A5851); el alfa del destino se reescribe tal cual
    // Como el original (0x5A5880): ecx se recarga desde [esp+18h] = puntero BASE del destino, no el actual; el alfa que se escribe
    // en cada pixel es siempre el del primer pixel del destino (tomado antes del bucle; el pixel 0 solo reescribe el mismo valor).
    const auto dstBase = dstIt;
    for (RwInt32 n = 0; n < RwRasterGetWidth(srcRaster) * RwRasterGetHeight(srcRaster); ++n, srcIt += 4, dstIt += 4) {
        const RwUInt8 a  = dstBase[3];
        const RwUInt8 c0 = ClothesBlendByte((float)srcIt[0] * r2 + (float)dstIt[0] * r1);
        const RwUInt8 c1 = ClothesBlendByte((float)srcIt[1] * r2 + (float)dstIt[1] * r1);
        const RwUInt8 c2 = ClothesBlendByte((float)srcIt[2] * r2 + (float)dstIt[2] * r1);
        dstIt[0] = c0;
        dstIt[2] = c2;
        dstIt[1] = c1;
        dstIt[3] = a;
    }

    RwRasterUnlock(srcRaster); // 0x5A59A2
    RwRasterUnlock(dstRaster); // 0x5A59A8

    CTimer::Resume();
}

// 0x5A59C0
void CClothesBuilder::BlendTextures(RwTexture* dst, RwTexture* src1, RwTexture* src2, float r1, float r2, float r3, int32) {
    ZoneScoped;

    const auto src1Raster = RwTextureGetRaster(src1);
    const auto src2Raster = RwTextureGetRaster(src2);
    const auto dstRaster  = RwTextureGetRaster(dst);

    CTimer::Suspend();

    auto src1It = RwRasterLock(src1Raster, 0, rwRASTERLOCKREAD);      // 0x5A59E6
    auto src2It = RwRasterLock(src2Raster, 0, rwRASTERLOCKREAD);      // 0x5A59F7
    auto dstIt  = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE);  // 0x5A5A09

    // Como el original: pixeles = ancho*alto del raster src1 (0x5A5A10); el alfa no se toca; suma: src2*r3 + src1*r2 + dst*r1
    for (RwInt32 n = 0; n < RwRasterGetWidth(src1Raster) * RwRasterGetHeight(src1Raster); ++n, src1It += 4, src2It += 4, dstIt += 4) {
        const RwUInt8 c0 = ClothesBlendByte((float)src2It[0] * r3 + (float)src1It[0] * r2 + (float)dstIt[0] * r1);
        const RwUInt8 c1 = ClothesBlendByte((float)src2It[1] * r3 + (float)src1It[1] * r2 + (float)dstIt[1] * r1);
        const RwUInt8 c2 = ClothesBlendByte((float)src2It[2] * r3 + (float)src1It[2] * r2 + (float)dstIt[2] * r1);
        dstIt[0] = c0;
        dstIt[2] = c2;
        dstIt[1] = c1;
    }

    RwRasterUnlock(src1Raster); // 0x5A5B94
    RwRasterUnlock(src2Raster); // 0x5A5B9A
    RwRasterUnlock(dstRaster);  // 0x5A5BA4

    CTimer::Resume();
}

// 0x5A5BC0
void CClothesBuilder::BlendTextures(RwTexture* dst, RwTexture* src1, RwTexture* src2, float r1, float r2, float r3, int32 numColors, RwTexture* tattoos) {
    ZoneScoped;

    const auto src1Raster = RwTextureGetRaster(src1);
    const auto src2Raster = RwTextureGetRaster(src2);
    const auto dstRaster  = RwTextureGetRaster(dst);
    const auto tatRaster  = RwTextureGetRaster(tattoos);

    CTimer::Suspend();

    auto src1It = RwRasterLock(src1Raster, 0, rwRASTERLOCKREAD);     // 0x5A5BF9
    auto src2It = RwRasterLock(src2Raster, 0, rwRASTERLOCKREAD);     // 0x5A5C05
    auto dstIt  = RwRasterLock(dstRaster, 0, rwRASTERLOCKREADWRITE); // 0x5A5C13
    auto tatIt  = RwRasterLock(tatRaster, 0, rwRASTERLOCKREAD);      // 0x5A5C23

    // Como el original: pixeles = ancho*alto del raster src1 (0x5A5C2A); alfa del tatuaje en tat[3]
    const float k255 = std::bit_cast<float>(0x3B808081u); // 0x859A3C
    for (RwInt32 n = 0; n < RwRasterGetWidth(src1Raster) * RwRasterGetHeight(src1Raster); ++n, src1It += 4, src2It += 4, dstIt += 4, tatIt += 4) {
        const RwUInt8 a  = tatIt[3];
        const RwUInt8 v0 = ClothesBlendByte((float)src2It[0] * r3 + (float)src1It[0] * r2 + (float)dstIt[0] * r1);
        const RwUInt8 v1 = ClothesBlendByte((float)src2It[1] * r3 + (float)src1It[1] * r2 + (float)dstIt[1] * r1);
        const RwUInt8 v2 = ClothesBlendByte((float)src2It[2] * r3 + (float)src1It[2] * r2 + (float)dstIt[2] * r1);
        // Como el original (0x5A5DAF-0x5A5E30): fild((tat-v)*alfa) * (1/255) + v -> _ftol2 (byte bajo)
        const RwUInt8 o0 = ClothesBlendByte((float)(((int32)tatIt[0] - (int32)v0) * (int32)a) * k255 + (float)(int32)v0);
        const RwUInt8 o1 = ClothesBlendByte((float)(((int32)tatIt[1] - (int32)v1) * (int32)a) * k255 + (float)(int32)v1);
        const RwUInt8 o2 = ClothesBlendByte((float)(((int32)tatIt[2] - (int32)v2) * (int32)a) * k255 + (float)(int32)v2);
        dstIt[0] = o0;
        dstIt[1] = o1;
        dstIt[2] = o2;
    }

    RwRasterUnlock(src1Raster); // 0x5A5E71
    RwRasterUnlock(src2Raster); // 0x5A5E7B
    RwRasterUnlock(dstRaster);  // 0x5A5E85
    RwRasterUnlock(tatRaster);  // 0x5A5E8F

    CTimer::Resume();
}

// 0x5A5F70
RwTexture* GetTextureFromTxdAndLoadNextTxd(RwTexture* dstTex, int32 txdId_withTexture, int32 CRC_nextTxd, int32* nextTxdId) {
    if (txdId_withTexture == -1) {
        if (CRC_nextTxd) {
            *nextTxdId = CClothesBuilder::RequestTexture(CRC_nextTxd);
            CStreaming::LoadRequestedModels();
        } else {
            *nextTxdId = -1;
        }
        return dstTex;
    }

    CStreaming::LoadAllRequestedModels(true);
    if (CRC_nextTxd) {
        *nextTxdId = CClothesBuilder::RequestTexture(CRC_nextTxd);
        CStreaming::LoadRequestedModels();
    } else {
        *nextTxdId = -1;
    }
    const auto tex = GetFirstTexture(CTxdStore::GetTxd(txdId_withTexture));
    const auto res = dstTex
        ? CClothesBuilder::PlaceTextureOnTopOfTexture(dstTex, tex), dstTex
        : CClothesBuilder::CopyTexture(tex);
    CStreaming::RemoveModel(TXDToModelId(txdId_withTexture));
    return res;
}

// 0x5A6040
void CClothesBuilder::ConstructTextures(RwTexDictionary* dict, uint32* hashes, float factorA, float factorB, float factorC) {
    const auto name = [](uintptr_t address) { return StaticRef<const char*>(address); };
    const auto defaultKey = [&](uintptr_t address) { return CKeyGen::GetUppercaseKey(name(address)); };
    const auto add = [&](RwTexture* texture, uintptr_t address) {
        RwTextureSetName(texture, name(address));
        RwTexDictionaryAddTexture(dict, texture);
    };
    int32 current = RequestTexture(hashes[4]), torsoTxd;
    CStreaming::LoadRequestedModels();
    RwTexture* tattoos = nullptr;
    for (int32 part = 4; part <= 12; ++part) {
        int32 next;
        tattoos = GetTextureFromTxdAndLoadNextTxd(tattoos, current,
            part == 12 ? defaultKey(0x8D0A4C) : hashes[part + 1],
            part == 12 ? &torsoTxd : &next);
        if (part != 12) current = next;
    }
    if (factorB < 0.0f) { factorA = float(double(factorA) + factorB); factorB = 0.0f; }
    CStreaming::LoadAllRequestedModels(true);
    current = RequestTexture(hashes[0]);
    CStreaming::LoadRequestedModels();
    auto* torsoDictionary = CTxdStore::GetTxd(torsoTxd);
    auto* torsoNormal = RwTexDictionaryFindNamedTexture(torsoDictionary, name(0x8D0A5C));
    auto* torsoFat = RwTexDictionaryFindNamedTexture(torsoDictionary, name(0x8D0A60));
    auto* torsoRipped = RwTexDictionaryFindNamedTexture(torsoDictionary, name(0x8D0A64));
    auto* torso = CopyTexture(torsoNormal);
    if (tattoos) {
        BlendTextures(torso, torsoFat, torsoRipped, factorA, factorB, factorC, 108, tattoos);
        RwTextureDestroy(tattoos);
    } else {
        BlendTextures(torso, torsoFat, torsoRipped, factorA, factorB, factorC, 108);
    }
    CStreaming::RemoveModel(TXDToModelId(torsoTxd));
    int32 legsTxd;
    GetTextureFromTxdAndLoadNextTxd(torso, current, defaultKey(0x8D0A50), &legsTxd);
    add(torso, 0x8D0A7C);
    CStreaming::LoadAllRequestedModels(true);
    current = RequestTexture(hashes[2]);
    CStreaming::LoadRequestedModels();
    auto* legsDictionary = CTxdStore::GetTxd(legsTxd);
    auto* legsNormal = RwTexDictionaryFindNamedTexture(legsDictionary, name(0x8D0A68));
    auto* legsFat = RwTexDictionaryFindNamedTexture(legsDictionary, name(0x8D0A6C));
    auto* legsRipped = RwTexDictionaryFindNamedTexture(legsDictionary, name(0x8D0A70));
    auto* legs = CopyTexture(legsNormal);
    BlendTextures(legs, legsFat, legsRipped, factorA, factorB, factorC, 108);
    CStreaming::RemoveModel(TXDToModelId(legsTxd));
    int32 faceTxd;
    GetTextureFromTxdAndLoadNextTxd(legs, current, hashes[1] ? hashes[1] : defaultKey(0x8D0A54), &faceTxd);
    add(legs, 0x8D0A88);
    CStreaming::LoadAllRequestedModels(true);
    current = RequestTexture(hashes[3] ? hashes[3] : defaultKey(0x8D0A58));
    CStreaming::LoadRequestedModels();
    auto* faceDictionary = CTxdStore::GetTxd(faceTxd);
    auto* faceNormal = RwTexDictionaryFindNamedTexture(faceDictionary, name(0x8D0A74));
    auto* faceFat = RwTexDictionaryFindNamedTexture(faceDictionary, name(0x8D0A78));
    RwTexture* face;
    if (faceNormal) {
        face = CopyTexture(faceNormal);
        if (faceFat) {
            const double partial = double(factorA) + factorB;
            const double sum = partial + factorC;
            const double ratio = double(factorA) + factorC;
            BlendTextures(face, faceFat, float(ratio / sum), float(double(factorB) / sum), 108);
        }
    } else {
        face = CopyTexture(GetFirstTexture(faceDictionary));
    }
    CStreaming::RemoveModel(TXDToModelId(faceTxd));
    add(face, 0x8D0A80);
    int32 next;
    auto* feet = GetTextureFromTxdAndLoadNextTxd(nullptr, current, hashes[13], &next);
    add(feet, 0x8D0A8C);
    current = next;
    for (int32 part = 14; part <= 17; ++part) {
        auto* texture = GetTextureFromTxdAndLoadNextTxd(nullptr, current, hashes[part], &next);
        if (texture) add(texture, 0x8D0A90 + uintptr_t(part - 14) * 4);
        current = next;
    }
    if (current != -1) {
        CStreaming::LoadAllRequestedModels(true);
        auto* extra = CopyTexture(GetFirstTexture(CTxdStore::GetTxd(current)));
        CStreaming::RemoveModel(TXDToModelId(current));
        if (extra) add(extra, 0x8D0AA0);
    }
}

// 0x5A6530
void CClothesBuilder::ConstructGeometryAndSkinArrays(RpHAnimHierarchy* pBoneHier, RpGeometry** ppGeometry, RwMatrixWeights** ppWeights, uint32** ppIndices, uint32 numModels, RpGeometry** pGeometrys, RpMaterial** pMaterial) {
    int32 totalVertices = 0, totalTriangles = 0;
    for (uint32 model = 0; model < numModels; ++model) {
        if (const auto* geometry = pGeometrys[model]) {
            totalVertices += geometry->numVertices;
            totalTriangles += geometry->numTriangles;
        }
    }
    auto* result = RpGeometryCreate(totalVertices, totalTriangles, 0x35);
    *ppGeometry = result;
    auto* morph = RpGeometryGetMorphTarget(result, 0);
    int32 vertexBase = 0, triangleBase = 0;
    for (uint32 model = 0; model < numModels; ++model) {
        auto* geometry = pGeometrys[model];
        if (!geometry) continue;
        const auto* sourceMorph = RpGeometryGetMorphTarget(geometry, 0);
        for (int32 vertex = 0; vertex < geometry->numVertices; ++vertex) {
            morph->verts[vertexBase + vertex] = sourceMorph->verts[vertex];
            morph->normals[vertexBase + vertex] = sourceMorph->normals[vertex];
            result->texCoords[0][vertexBase + vertex] = geometry->texCoords[0][vertex];
        }
        for (int32 triangle = 0; triangle < geometry->numTriangles; ++triangle) {
            auto* destination = &result->triangles[triangleBase++];
            const auto& source = geometry->triangles[triangle];
            RpGeometryTriangleSetVertexIndices(result, destination,
                uint16(source.vertIndex[0] + vertexBase),
                uint16(source.vertIndex[1] + vertexBase),
                uint16(source.vertIndex[2] + vertexBase));
            RpGeometryTriangleSetMaterial(result, destination, pMaterial[model]);
        }
        vertexBase += geometry->numVertices;
    }
    RwSphere sphere;
    RpMorphTargetCalcBoundingSphere(morph, &sphere);
    morph->boundingSphere = sphere;
    RpGeometryUnlock(result);
    *ppWeights = static_cast<RwMatrixWeights*>(::operator new(size_t(uint32(totalVertices) << 4)));
    *ppIndices = static_cast<uint32*>(::operator new(size_t(uint32(totalVertices) * 4)));
    auto* weights = *ppWeights;
    auto* indices = reinterpret_cast<uint8*>(*ppIndices);
    for (uint32 model = 0; model < numModels; ++model) {
        auto* geometry = pGeometrys[model];
        if (!geometry) continue;
        auto* skin = RpSkinGeometryGetSkin(geometry);
        uint8 conversion[64];
        BuildBoneIndexConversionTable(conversion, pBoneHier, int32(model));
        for (int32 vertex = 0; vertex < geometry->numVertices; ++vertex) {
            const auto* bones = reinterpret_cast<const uint8*>(RpSkinGetVertexBoneIndices(skin)) + vertex * 4;
            for (int32 bone = 0; bone < 4; ++bone) indices[bone] = conversion[bones[bone]];
            *weights++ = RpSkinGetVertexBoneWeights(skin)[vertex];
            indices += 4;
        }
    }
}

// 0x5A69D0
RpClump* CClothesBuilder::CreateSkinnedClump(RpClump* bones, RwTexDictionary* dict, CPedClothesDesc& ndscr, const CPedClothesDesc* odscr, bool bCutscenePlayer) {
    LoadCdDirectory();

    const struct {
        eClothesModelPart mp;
        const char*       name;
    } parts[]{
        {eClothesModelPart::CLOTHES_MODEL_TORSO, "torso"},
        {eClothesModelPart::CLOTHES_MODEL_HEAD, "head"},
        {eClothesModelPart::CLOTHES_MODEL_HANDS, "hands"},
        {eClothesModelPart::CLOTHES_MODEL_LEGS, "legs"},
        {eClothesModelPart::CLOTHES_MODEL_SHOES, "feet"}
    };
    for (const auto [mp, name] : parts) {
        if (!ndscr.m_anModelKeys[(int)mp]) {
            ndscr.SetModel(name, mp);
        }
    }

    StaticRef<const CPedClothesDesc*>(0xBC1288) = odscr; // Como el original (0x5A6A85 mov [0BC1288h],edx)
    if (odscr) {
        // Como el original (0x5A6B84-0x5A6C2E)
        ms_geometryHasChanged = false;
        ms_textureHasChanged  = false;
        if (!(ndscr.m_fFatStat == odscr->m_fFatStat) || !(ndscr.m_fMuscleStat == odscr->m_fMuscleStat)) { // fcompp + test ah,44h ; jp (NaN = distinto)
            ms_ratiosHaveChanged  = true;
            ms_geometryHasChanged = true;
        } else {
            ms_ratiosHaveChanged = false;
        }
        if (!rng::equal(ndscr.m_anModelKeys, odscr->m_anModelKeys)) {
            ms_geometryHasChanged = true;
        }
        if (!rng::equal(ndscr.m_anTextureKeys, odscr->m_anTextureKeys)) {
            ms_textureHasChanged = true;
        }
        if (!ms_ratiosHaveChanged && !ms_geometryHasChanged && !ms_textureHasChanged) {
            return nullptr;
        }
    } else {
        ms_ratiosHaveChanged = ms_geometryHasChanged = ms_textureHasChanged = true;
    }
    CPedClothesDesc dscr = ndscr;
    PreprocessClothesDesc(dscr, bCutscenePlayer);

    //> 0x5A42B0 - Calculate blend ratios
    float rNormal, rFatness, rMuscle;
    {
        // Como el original (0x5A42B0): multiplica por constantes (no divide), x87 a 24 bits
        float m = CStats::GetStatValue(STAT_MUSCLE);
        float f = dscr.m_fFatStat - 200.f; // 0x858A48
        if (m < 0.f) m = 0.f;
        if (f < 0.f) f = 0.f;
        rFatness = f * std::bit_cast<float>(0x3AA3D70Au); // 0x865354
        rMuscle  = m * std::bit_cast<float>(0x3A83126Fu); // 0x858CDC
        if (rFatness > 1.f) rFatness = 1.f;
        if (rMuscle  > 1.f) rMuscle  = 1.f;
        rNormal = 1.f - (rFatness + rMuscle);
        if (rFatness + rMuscle > 1.f) {
            const float t = 1.f / (rFatness + rMuscle);
            rFatness *= t;
            rMuscle  *= t;
            rNormal   = 0.f;
        }
    }

    if ((ms_textureHasChanged || ms_ratiosHaveChanged) && !bCutscenePlayer) {
        RwTexDictionaryForAllTextures(dict, [](RwTexture* t, void* data) {
            RwTexDictionaryRemoveTexture(t);
            RwTextureDestroy(t);
            return t;
        }, nullptr);
        ConstructTextures(dict, dscr.m_anTextureKeys.data(), rNormal, rFatness, rMuscle);
    }

    constexpr auto NO_BODY_PARTS = 10;

    constexpr const char* BODY_PART_TEX_NAMES[NO_BODY_PARTS]{
        "torso",
        "head",
        "torso",
        "legs",
        "feet",
        "necklace",
        "watch",
        "glasses",
        "hat",
        "extra1"
    };

    //> 0x5A6B2C
    RpMaterial*       ms[NO_BODY_PARTS];
    for (uint32 i{}; const auto name : BODY_PART_TEX_NAMES) {
        ms[i++] = [&]() -> RpMaterial* {
            if (const auto tex = RwTexDictionaryFindNamedTexture(dict, name)) {
                const auto mat = RpMaterialCreate();
                // Como el original (0x5A6B51-0x5A6B77): primero el color en linea (mov [eax+4],ecx) y despues RpMaterialSetTexture
                const auto clr = RwRGBA(0xFF, 0xFF, 0xFF, 0xFF);
                RpMaterialSetColor(mat, &clr);
                RpMaterialSetTexture(mat, tex);
                return mat;
            }
            return nullptr;
        }();
    }

    //> 0x5A6C5D
    RpGeometry* gs[NO_BODY_PARTS]{};
    ConstructGeometryArray(gs, dscr.m_anModelKeys.data(), rNormal, rFatness, rMuscle);

    //> 0x5A6C6A
    const auto boneAtomic = GetFirstAtomic(bones);
    const auto boneSkin   = RpSkinGeometryGetSkin(RpAtomicGetGeometry(boneAtomic));
    const auto boneAnimHr = RpSkinAtomicGetHAnimHierarchy(boneAtomic);

    RwMatrixWeights* boneWeights;
    RwUInt32*        boneIdxs;
    RpGeometry*      tmpGeo;
    ConstructGeometryAndSkinArrays(
        boneAnimHr,
        &tmpGeo,
        &boneWeights,
        &boneIdxs,
        NO_BODY_PARTS,
        gs,
        ms
    );
    // Como el original (0x5A6CAD..0x5A6CD0): RpSkinGetNumBones (0x7C77E0) se llama antes que RpSkinGetSkinToBoneMatrices (0x7C7810),
    // y ambos antes de RpSkinCreate (0x7C75B0); el orden de evaluacion de los argumentos de C++ no esta garantizado
    const auto numBones = RpSkinGetNumBones(boneSkin);
    const auto boneMatrices = const_cast<RwMatrix*>(RpSkinGetSkinToBoneMatrices(boneSkin)); // TODO
    RpSkinGeometrySetSkin(
        tmpGeo,
        RpSkinCreate(
            RpGeometryGetNumVertices(tmpGeo),
            numBones,
            boneWeights,
            boneIdxs,
            boneMatrices
        )
    );
    DestroySkinArrays(boneWeights, boneIdxs);
    
    const auto hier = RpHAnimHierarchyCreateFromHierarchy(
        boneAnimHr,
        (RpHAnimHierarchyFlag)boneAnimHr->flags,       // TODO: Use function to access
        boneAnimHr->currentAnim->maxInterpKeyFrameSize // TODO: Use function to access
    );

    const auto childFrame = RwFrameCreate();
    RpHAnimFrameSetHierarchy(childFrame, hier);

    const auto atomic = RpAtomicCreate();
    RpAtomicSetGeometry(atomic, tmpGeo, NULL);
    RpSkinAtomicSetHAnimHierarchy(atomic, hier);
    RpAtomicSetFrame(atomic, childFrame);
    RpSkinAtomicSetType(atomic, rpSKINTYPEGENERIC);

    const auto rootFrame = RwFrameCreate();
    RwFrameAddChild(rootFrame, childFrame);

    const auto clump = RpClumpCreate();
    RpClumpSetFrame(clump, rootFrame);
    RpClumpAddAtomic(clump, atomic);

    // Free memory
    {
        RpGeometryDestroy(tmpGeo);
        tmpGeo->flags |= 2; // Como el original (0x5A6D5A or dword ptr [esi+8],2)

        RwTexDictionarySetCurrent(dict);
        for (auto i = 0; i < NO_BODY_PARTS; i++) {
            if (gs[i]) {
                RpGeometryDestroy(gs[i]);
            }
            if (ms[i]) {
                RpMaterialDestroy(ms[i]);
            }
        }
    }

    return clump;
}
