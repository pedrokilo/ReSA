/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlantMgr
 * Reconstrucción: COMPLETA — 20/20 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 20/20 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "X87Math.h"

#include "PlantMgr.h"
#include "GrassRenderer.h"
#include "PlantColEntEntry.h"
#include "PlantLocTri.h"
#include "PlantSurfPropMgr.h"
#include "ColHelpers.h"
#include "ProcObjectMan.h"
#include "CustomBuildingDNPipeline.h"

// 0x5DD100 (todo: move)
// Si la geometria del atomico no tiene luces precalculadas (prelit), crea una copia con tristrip + texcoords + prelit (formato 0xD) con los mismos
// vertices, texcoords, triangulos y materiales por triangulo, y se la asigna al atomico (rpATOMICSAMEBOUNDINGSPHERE).
static void AtomicCreatePrelitIfNeeded(RpAtomic* atomic) {
    RpGeometry* const oldGeometry = atomic->geometry;
    if (oldGeometry->flags & rpGEOMETRYPRELIT) {
        return;
    }
    const int32 numTriangles = oldGeometry->numTriangles;
    const int32 numVertices  = oldGeometry->numVertices;
    RpMorphTarget* const oldMorphTarget = oldGeometry->morphTarget;

    RpGeometry* const newGeometry = RpGeometryCreate(numVertices, numTriangles, 0xD);
    RpMorphTarget* const newMorphTarget = newGeometry->morphTarget;
    RwTexCoords* const newTexCoords = newGeometry->texCoords[0];
    RwTexCoords* const oldTexCoords = oldGeometry->texCoords[0];

    std::memcpy(newMorphTarget->verts, oldMorphTarget->verts, static_cast<size_t>(numVertices) * sizeof(RwV3d));
    std::memcpy(newTexCoords, oldTexCoords, static_cast<size_t>(numVertices) * sizeof(RwTexCoords));
    RpTriangle* const oldTriangles = oldGeometry->triangles;
    RpTriangle* const newTriangles = newGeometry->triangles;
    std::memcpy(newTriangles, oldTriangles, static_cast<size_t>(numTriangles) * sizeof(RpTriangle));

    for (int32 i = 0; i < numTriangles; ++i) {
        RpMaterial* const material = RpGeometryTriangleGetMaterial(oldGeometry, &oldTriangles[i]);
        RpGeometryTriangleSetMaterial(newGeometry, &newTriangles[i], material);
    }
    RpGeometryUnlock(newGeometry);
    newGeometry->flags |= 2; // rpGEOMETRYPOSITIONS
    RpAtomicSetGeometry(atomic, newGeometry, 1);
}

// 0x5DD1E0 (do not hook! it has retarded calling conv)
static bool GeometrySetPrelitConstantColor(RpGeometry* geometry, RwRGBA color) {
    if ((RpGeometryGetFlags(geometry) & rpGEOMETRYPRELIT) == 0) {
        return false;
    }

    RpGeometryLock(geometry, rpGEOMETRYLOCKALL);
    auto prelit = RpGeometryGetPreLightColors(geometry);
    if (prelit) {
        auto numVertices = RpGeometryGetNumVertices(geometry);
        for (auto i = 0; i < numVertices; ++i) {
            prelit[i] = color;
        }
    }
    RpGeometryUnlock(geometry);
    return true;
}

// 0x5DD220
bool CPlantMgr::LoadModels(std::initializer_list<const char*> models, RpAtomic** atomics) {
    for (auto& model : models) {
        auto stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, std::format("models\\grass\\{}", model).c_str());
        RpClump* clump = nullptr;

        if (stream && RwStreamFindChunk(stream, rwID_CLUMP, nullptr, nullptr)) {
            clump = RpClumpStreamRead(stream);
        }
        RwStreamClose(stream, nullptr);
        assert(clump);

        auto firstAtomic = GetFirstAtomic(clump);
        SetFilterModeOnAtomicsTextures(firstAtomic, rwFILTERMIPLINEAR);
        AtomicCreatePrelitIfNeeded(firstAtomic);

        auto geometry = RpAtomicGetGeometry(firstAtomic);
        RpGeometryLock(geometry, rpGEOMETRYLOCKALL);
        RpGeometryGetFlags(geometry) &= 0xFFFFFF8F; // Negate bit 5 and 6, nothing seems to match it currently
        RpGeometryGetFlags(geometry) |= rpGEOMETRYMODULATEMATERIALCOLOR;
        RpGeometryUnlock(geometry);
        GeometrySetPrelitConstantColor(geometry, RwRGBA{ 255, 255, 255, 255 });

        RwRGBA color = { 0, 0, 0, 50 };
        RpGeometryForAllMaterials(geometry, [](RpMaterial* material, void* color) {
                RpMaterialSetColor(material, (RwRGBA*)color);
                RpMaterialSetTexture(material, tex_gras07Si);
                return material;
            }, &color
        );

        auto atomicCopy = RpAtomicClone(firstAtomic);
        RpClumpDestroy(clump);
        SetFilterModeOnAtomicsTextures(atomicCopy, rwFILTERLINEAR);
        RpAtomicSetFrame(atomicCopy, RwFrameCreate());
        *atomics++ = atomicCopy;
    }

    return true;
}

void CPlantMgr::InjectHooks() {
    RH_ScopedClass(CPlantMgr);
    RH_ScopedCategory("Plant");

    RH_ScopedInstall(Initialise, 0x5DD910);
    RH_ScopedInstall(Shutdown, 0x5DB940);
    RH_ScopedInstall(ReloadConfig, 0x5DD780);
    RH_ScopedInstall(MoveLocTriToList, 0x5DB590);
    RH_ScopedInstall(MoveColEntToList, 0x5DB5F0);
    RH_ScopedInstall(SetPlantFriendlyFlagInAtomicMI, 0x5DB650);
    RH_ScopedInstall(Update, 0x5DCFA0);
    RH_ScopedInstall(PreUpdateOnceForNewCameraPos, 0x5DCF30);
    RH_ScopedInstall(UpdateAmbientColor, 0x5DB310);
    RH_ScopedInstall(CalculateWindBending, 0x5DB3D0);
    RH_ScopedInstall(_ColEntityCache_Add, 0x5DBEB0);
    RH_ScopedInstall(_ColEntityCache_FindInCache, 0x5DB530);
    RH_ScopedInstall(_ColEntityCache_Remove, 0x5DBEF0);
    RH_ScopedInstall(_ColEntityCache_Update, 0x5DC510);
    RH_ScopedInstall(_ProcessEntryCollisionDataSections, 0x5DCD80);
    RH_ScopedInstall(_ProcessEntryCollisionDataSections_AddLocTris, 0x5DC8B0);
    RH_ScopedInstall(_ProcessEntryCollisionDataSections_RemoveLocTris, 0x5DBF20);
    RH_ScopedInstall(_UpdateLocTris, 0x5DCF00);
    RH_ScopedInstall(Render, 0x5DBAE0);
    RH_ScopedGlobalInstall(AtomicCreatePrelitIfNeeded, 0x5DD100);
    //RH_ScopedGlobalInstall(LoadModels, 0x5DD220); // uses `__usercall`, can't hook

    // Do not uncomment!
    // RH_ScopedInstall(_CalcDistanceSqrToEntity, 0x5DBE40, {.reversed = false}); <-- bad call conv.

    // debug shit, all of them just return true.
    // addresses (probably not in order): 0x5DB550 - 0x5DB580

    // RH_ScopedInstall(DbgCountCachedEntities, 0x0, {.reversed = false});
    // RH_ScopedInstall(DbgCountLocTrisAndPlants, 0x0, {.reversed = false});
    // RH_ScopedInstall(DbgRenderCachedEntities, 0x0, {.reversed = false});
    // RH_ScopedInstall(DbgRenderLocTris, 0x0, {.reversed = false});
}

// 0x5DD910
bool CPlantMgr::Initialise() {
    ZoneScoped;

    if (!ReloadConfig())
        return false;

    // Load textures
    {
        CStreaming::MakeSpaceFor(0x8800);
        CStreaming::ImGonnaUseStreamingMemory();
        CTxdStore::PushCurrentTxd();
        auto slot = CTxdStore::FindOrAddTxdSlot("grass_pc");
        CTxdStore::LoadTxd(slot, "models\\grass\\plant1.txd");
        CTxdStore::AddRef(slot);
        CTxdStore::SetCurrentTxd(slot);

        const auto ReadTexture = [](const char* name) {
            auto texture = RwTextureRead(name, nullptr);
            RwTextureSetAddressing(texture, rwTEXTUREADDRESSWRAP);
            RwTextureSetFilterMode(texture, rwFILTERLINEAR);
            return texture;
        };

        for (auto&& [i, tab] : rngv::enumerate(PC_PlantTextureTab | rng::views::take(2))) {
            for (auto&& [j, tex] : rngv::enumerate(tab)) {
                tex = ReadTexture(std::format("txgrass{}_{}", i, j).c_str());
            }
        }
        tex_gras07Si = ReadTexture("gras07Si");
        CTxdStore::PopCurrentTxd();
        CStreaming::IHaveUsedStreamingMemory();

        grassTexturesPtr[0] = grassTexturesPtr[2] = PC_PlantTextureTab[0];
        grassTexturesPtr[1] = grassTexturesPtr[3] = PC_PlantTextureTab[1];
    }

    // Load models
    {
        const auto models1 = { "grass0_1.dff", "grass0_2.dff", "grass0_3.dff", "grass0_4.dff" };
        const auto models2 = { "grass1_1.dff", "grass1_2.dff", "grass1_3.dff", "grass1_4.dff" };
        if (LoadModels(models1, PC_PlantModelsTab[0]) && LoadModels(models2, PC_PlantModelsTab[1])) {
            // Como el original: se guardan punteros a las filas de la tabla, no el primer atomic
            grassModelsPtr[0] = grassModelsPtr[2] = PC_PlantModelsTab[0];
            grassModelsPtr[1] = grassModelsPtr[3] = PC_PlantModelsTab[1];

            for (auto i = 0u; i < 4u; i++) {
                CGrassRenderer::SetPlantModelsTab(i, PC_PlantModelsTab[0]); // grassModelsPtr[0]
            }

            CGrassRenderer::SetCloseFarAlphaDist(PLANTS_ALPHA_MIN_DIST, PLANTS_ALPHA_MAX_DIST);
            return true;
        }
    }

    return false;
}

// 0x5DB940
void CPlantMgr::Shutdown() {
    auto* nextEntry = CPlantMgr::m_CloseColEntListHead;
    while (nextEntry) {
        auto* curEntry = nextEntry;
        nextEntry      = curEntry->m_NextEntry;
        curEntry->ReleaseEntry();
    }

    // Destroy atomics
    // Como el original: las tablas se recorren por columnas ([0][j], [1][j], [2][j], [3][j])
    for (auto j = 0u; j < 4u; j++) {
        for (auto i = 0u; i < 4u; i++) {
            auto& a = PC_PlantModelsTab[i][j];
            if (a) {
                if (const auto f = RpAtomicGetFrame(a)) {
                    RpAtomicSetFrame(a, nullptr);
                    RwFrameDestroy(f);
                }
                RpAtomicDestroy(a);
                a = nullptr;
            }
        }
    }

    // Destroy textures
    for (auto j = 0u; j < 4u; j++) {
        for (auto i = 0u; i < 4u; i++) {
            auto& t = PC_PlantTextureTab[i][j];
            if (t) {
                RwTextureDestroy(t);
                t = nullptr;
            }
        }
    }

    // And finally unload the texture's txd
    CTxdStore::SafeRemoveTxdSlot("grass_pc");
}

// 0x5DD780
bool CPlantMgr::ReloadConfig() {
    if (!CPlantSurfPropMgr::Initialise()) {
        return false;
    }

    std::ranges::fill(m_CloseLocTriListHead, nullptr);
    m_UnusedLocTriListHead = m_LocTrisTab;

    CPlantLocTri* prevTri = nullptr;
    for (auto& tab : m_LocTrisTab) {
        tab.m_V1 = tab.m_V2 = tab.m_V3 = tab.m_Center = CVector{0.0f, 0.0f, 0.0f};
        tab.m_SurfaceId = 0u;
        rng::fill(tab.m_nMaxNumPlants, 0u);

        tab.m_PrevTri = prevTri;
        if (prevTri) {
            prevTri->m_NextTri = &tab;
        }

        prevTri = &tab;
    }
    m_LocTrisTab[MAX_NUM_PLANT_TRIANGLES - 1].m_NextTri = nullptr;
    m_CloseColEntListHead = nullptr;
    m_UnusedColEntListHead = m_ColEntCacheTab;

    CPlantColEntEntry* prevEntry = nullptr;
    for (auto& tab : m_ColEntCacheTab) {
        tab.m_Entity = nullptr;
        tab.m_Objects = nullptr;
        tab.m_numTriangles = 0u;

        tab.m_PrevEntry = prevEntry;
        if (prevEntry) {
            prevEntry->m_NextEntry = &tab;
        }

        prevEntry = &tab;
    }
    m_ColEntCacheTab[MAX_NUM_PROC_OBJECTS - 1].m_NextEntry = nullptr;

    return true;
}

// 0x5DB590
void CPlantMgr::MoveLocTriToList(CPlantLocTri*& oldList, CPlantLocTri*& newList, CPlantLocTri* triangle) {
    if (auto prev = triangle->m_PrevTri) {
        if (auto next = triangle->m_NextTri) {
            next->m_PrevTri = prev;
            prev->m_NextTri = next;
        } else {
            prev->m_NextTri = nullptr;
        }
    } else {
        if (oldList = triangle->m_NextTri) {
            oldList->m_PrevTri = nullptr;
        }
    }
    triangle->m_NextTri = newList;
    triangle->m_PrevTri = nullptr;
    newList = triangle;

    if (auto next = triangle->m_NextTri) {
        next->m_PrevTri = triangle;
    }
}

// 0x5DB5F0
// unused/inlined
void CPlantMgr::MoveColEntToList(CPlantColEntEntry*& oldList, CPlantColEntEntry*& newList, CPlantColEntEntry* entry) {
    if (auto prev = entry->m_PrevEntry) {
        if (auto next = entry->m_NextEntry) {
            next->m_PrevEntry = prev;
            prev->m_NextEntry = next;
        } else {
            prev->m_NextEntry = nullptr;
        }
    } else {
        if (oldList = entry->m_NextEntry) {
            oldList->m_PrevEntry = nullptr;
        }
    }
    entry->m_NextEntry = newList;
    entry->m_PrevEntry = nullptr;
    newList = entry;

    if (auto next = entry->m_NextEntry) {
        next->m_PrevEntry = entry;
    }
}

// 0x5DB650
bool CPlantMgr::SetPlantFriendlyFlagInAtomicMI(CAtomicModelInfo* ami) {
    ami->bAtomicFlag0x200 = false;

    auto cd = ami->GetColModel()->GetData();
    if (!cd)
        return false;

    // Como el original: el número de triángulos se lee con signo (movsx/jle)
    const int32 numTriangles = (int16)cd->m_nNumTriangles;
    for (int32 i = 0; i < numTriangles; i++) {
        const auto& triangle = cd->m_pTriangles[i];
        if (g_surfaceInfos.CreatesPlants(triangle.m_nMaterial)
            || g_surfaceInfos.CreatesObjects(triangle.m_nMaterial)) {
            ami->bAtomicFlag0x200 = true;
            return true;
        }
    }
    return false;
}

// 0x5DCFA0
bool CPlantMgr::Update(const CVector& cameraPosition) {
    ZoneScoped;

    static auto& nUpdateEntCache = StaticRef<uint8>(0xC09171);
    static auto& nLocTriSkipCounter = StaticRef<uint8>(0xC09170);

    IncrementScanCode();
    CGrassRenderer::SetCurrentScanCode(m_scanCode);
    CGrassRenderer::SetGlobalCameraPos(cameraPosition);

    UpdateAmbientColor();
    CGrassRenderer::SetGlobalWindBending(CalculateWindBending());

    // Como el original: contadores de un byte sin signo (test al,1Fh / and ecx,7)
    _ColEntityCache_Update(cameraPosition, (++nUpdateEntCache & 0x1F) != 0);

    const int32 skipMask = nLocTriSkipCounter++ & 7;
    for (auto head = m_CloseColEntListHead; head; head = head->m_NextEntry) {
        _ProcessEntryCollisionDataSections(*head, cameraPosition, skipMask);
    }
    return true;
}

// 0x5DCF30
bool CPlantMgr::PreUpdateOnceForNewCameraPos(const CVector& posn) {
    IncrementScanCode();
    CGrassRenderer::SetCurrentScanCode(m_scanCode);
    CGrassRenderer::SetGlobalCameraPos(posn);
    UpdateAmbientColor();
    CGrassRenderer::SetGlobalWindBending(CalculateWindBending());
    _ColEntityCache_Update(posn, false);

    for (auto i = m_CloseColEntListHead; i; i = i->m_NextEntry) {
        _ProcessEntryCollisionDataSections(*i, posn, 0xFAFAFAFA);
    }
    return true;
}

// 0x5DB310
void CPlantMgr::UpdateAmbientColor() {
    const float r = CTimeCycle::GetAmbientRed();
    const float g = CTimeCycle::GetAmbientGreen();
    const float b = CTimeCycle::GetAmbientBlue();
    // Como el original: 64 - _ftol(c * 2.5 * -255.0) y mínimo con 255 comparando sólo los 16 bits bajos
    const auto Channel = [](float c) {
        const auto v = (uint16)(64 - (int32)(int64)(c * 2.5f * -255.0f));
        return (uint8)(v > 255 ? 255 : v);
    };
    m_AmbientColor.r = Channel(r);
    m_AmbientColor.g = Channel(g);
    m_AmbientColor.b = Channel(b);
}

// 0x5DB3D0
float CPlantMgr::CalculateWindBending() {
    static auto& calculateTimer = StaticRef<uint32>(0xC0916C);
    static auto& RandomSeed = StaticRef<uint32>(0xC09168); // Como el original: se escribe y se lee como dword

    if ((calculateTimer & 1) == 0) {
        calculateTimer |= 1;
        RandomSeed = CGeneral::GetRandomNumber();
    }

    // The 2PI approximation here is nice, they could have went for `PI = 3` tho.
    constexpr float scalingFactor = 6.28f / 4096.f;

    // TODO: Look CEntity::ModifyMatrixForTreeInWind; it's definitely inlined somewhere. (Not actually inlined anywhere, according to android debug symbols)
    const float wind = CWeather::Wind;
    if (!(wind < 0.5f)) { // Como el original: NaN entra en esta rama (test ah,5 / jp)
        const uint32 offset    = (RandomSeed + CTimer::GetTimeInMS() * 8) & 0xFFFF;
        const uint32 uiOffset1 = (offset >> 12) & 0xF;
        const uint32 uiOffset2 = (uiOffset1 + 1) & 0xF;
        const float  fContrib  = (float)(int32)(offset & 0xFFF) * (1.0f / 4096.0f);

        // Como el original: ((1 - f) * o1 + f * o2) + 1
        const float fWindOffset = (1.0f - fContrib) * CWeather::saTreeWindOffsets[uiOffset1] + fContrib * CWeather::saTreeWindOffsets[uiOffset2];
        return (fWindOffset + 1.0f) * wind * 0.015f;
    }

    // Como el original: fsin de precisión completa multiplicado por 0.008 o 0.005 (no se divide)
    const float angle = (float)(int32)(CTimer::GetTimeInMS() & 0xFFF) * scalingFactor;
    const float scale = wind >= 0.2f ? 0.008f : 0.005f;
    return TrigSimpleSinMulCpp(angle, scale);
}

// 0x5DBAE0
void CPlantMgr::Render() {
    if (g_fx.GetFxQuality() == FX_QUALITY_LOW) {
        return;
    }

    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(FALSE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE,    RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATESRCBLEND,             RWRSTATE(rwBLENDSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEDESTBLEND,            RWRSTATE(rwBLENDINVSRCALPHA));
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,            RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONALWAYS));

    for (auto i = 0; i < 4; i++) {
        auto* plantTri = m_CloseLocTriListHead[i];
        auto* grassTextures = grassTexturesPtr[i];

        while (plantTri) {
            if (!plantTri->m_createsPlants) {
                plantTri = plantTri->m_NextTri;
                continue;
            }

            auto* surfData = CPlantSurfPropMgr::GetSurfProperties(plantTri->m_SurfaceId);
            for (auto k = 0; k < 3; k++) {
                // Como el original: 0x420D40 en línea (matriz inversa y, si hay espejo, la del espejo)
                if (!TheCamera.IsSphereVisible(plantTri->m_Center, plantTri->m_SphereRadius)) {
                    continue;
                }

                auto& plantSurf = surfData->m_Plants[k];
                if (plantSurf.model_id == 0xFFFF) {
                    continue;
                }

                PPTriPlant triPlant;
                triPlant.V1 = plantTri->m_V1;
                triPlant.V2 = plantTri->m_V2;
                triPlant.V3 = plantTri->m_V3;
                triPlant.Center = plantTri->m_Center;
                triPlant.seed            = plantTri->m_Seed[k];
                triPlant.num_plants = (plantTri->m_nMaxNumPlants[k] + 8) & 0xFFF8;

                triPlant.model_id = plantSurf.model_id;
                triPlant.scale.x  = plantSurf.scale_xy;
                triPlant.scale.y  = plantSurf.scale_z;
                triPlant.texture    = grassTextures[plantSurf.uv_offset];

                // Como el original: (day/30 * (1 - balance) + night/30 * balance) * 255, truncado a byte;
                // cada canal de color (salvo alfa) pasa a (c * mezcla) >> 8
                const float balance = CCustomBuildingDNPipeline::m_fDNBalanceParam;
                const uint8 lighting = plantTri->m_nLighting.value;
                const float day   = (float)(int32)(lighting & 0xF) * (1.0f / 30.0f);
                const float night = (float)(int32)(lighting >> 4) * (1.0f / 30.0f);
                const auto colorMix = (uint32)(uint8)(int64)((day * (1.0f - balance) + night * balance) * 255.0f);
                triPlant.color = plantSurf.color;
                triPlant.color.r = (uint8)((triPlant.color.r * colorMix) >> 8);
                triPlant.color.g = (uint8)((triPlant.color.g * colorMix) >> 8);
                triPlant.color.b = (uint8)((triPlant.color.b * colorMix) >> 8);

                triPlant.intensity = plantSurf.intensity;
                triPlant.intensity_var = plantSurf.intensity_variation;
                triPlant.scale_var_xy  = plantSurf.scale_variation_xy;
                triPlant.scale_var_z   = plantSurf.scale_variation_z;
                triPlant.wind_bend_scale = plantSurf.wind_blending_scale;
                triPlant.wind_bend_var   = plantSurf.wind_blending_variation;

                CGrassRenderer::AddTriPlant(&triPlant, i);
            }

            plantTri = plantTri->m_NextTri;
        }

        CGrassRenderer::FlushTriPlantBuffer();
    }
    RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEZTESTENABLE,          RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTION,    RWRSTATE(rwALPHATESTFUNCTIONGREATEREQUAL));
    RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
}

// 0x5DBEB0
CPlantColEntEntry* CPlantMgr::_ColEntityCache_Add(CEntity* entity, bool checkAlreadyExists) {
    if (checkAlreadyExists) {
        if (const auto entry = _ColEntityCache_FindInCache(entity)) {
            return entry; // Como el original: devuelve la entrada ya existente
        }
    }

    if (auto head = CPlantMgr::m_UnusedColEntListHead) {
        if (head->AddEntry(entity)) {
            return head;
        }
    }
    return nullptr;
}

// 0x5DB530
CPlantColEntEntry* CPlantMgr::_ColEntityCache_FindInCache(CEntity* entity) {
    if (!CPlantMgr::m_CloseColEntListHead)
        return nullptr;

    for (auto i = CPlantMgr ::m_CloseColEntListHead; i; i = i->m_NextEntry) {
        if (i->m_Entity == entity) {
            return i;
        }
    }
    return nullptr;
}

// 0x5DBEF0
void CPlantMgr::_ColEntityCache_Remove(CEntity* entity) {
    if (auto entry = _ColEntityCache_FindInCache(entity)) {
        entry->ReleaseEntry();
    }
}

// 0x5DC510
bool CPlantMgr::_ColEntityCache_Update(const CVector& cameraPos, bool fast) {
    if (fast) {
        // doing a fast update, prune only ones that have no entity.
        auto* nextFastEntry = CPlantMgr::m_CloseColEntListHead;
        while (nextFastEntry) {
            auto* curEntry = nextFastEntry; // ReleaseEntry() Overwrites m_NextEntry pointer, we need to keep track of it before that can happen
            nextFastEntry  = curEntry->m_NextEntry;
            if (!curEntry->m_Entity) {
                curEntry->ReleaseEntry();
            }
        }

        return true;
    }

    // prune ones that have no entity, too far or not in the same area.
    auto* nextEntry = CPlantMgr::m_CloseColEntListHead;
    while (nextEntry) {
        auto* curEntry = nextEntry; // ReleaseEntry() Overwrites m_NextEntry pointer, we need to keep track of it before that can happen
        nextEntry      = curEntry->m_NextEntry;
        if (!curEntry->m_Entity || _CalcDistanceSqrToEntity(curEntry->m_Entity, cameraPos) > PROC_OBJECTS_MAX_DISTANCE_SQUARED || !curEntry->m_Entity->IsInCurrentArea()) {
            curEntry->ReleaseEntry();
        }
    }

    if (!CPlantMgr::m_UnusedColEntListHead)
        return true;

    // Como el original: sectores con floor((c * 0.02 + 60)) del rectángulo de 340 alrededor de la cámara
    const auto SectorCoord = [](float c) { return (int32)(int64)std::floor((double)(c * 0.02f + 60.0f)); };
    const float minX = cameraPos.x - PROC_OBJECTS_MAX_DISTANCE, minY = cameraPos.y - PROC_OBJECTS_MAX_DISTANCE;
    const float maxX = cameraPos.x + PROC_OBJECTS_MAX_DISTANCE, maxY = cameraPos.y + PROC_OBJECTS_MAX_DISTANCE;
    const int32 sx0 = SectorCoord(minX), startX = sx0 > 0 ? sx0 : 0;
    const int32 sy0 = SectorCoord(minY), startY = sy0 > 0 ? sy0 : 0;
    const int32 sx1 = SectorCoord(maxX), endX = sx1 < MAX_SECTORS_X - 1 ? sx1 : MAX_SECTORS_X - 1;
    const int32 sy1 = SectorCoord(maxY), endY = sy1 < MAX_SECTORS_Y - 1 ? sy1 : MAX_SECTORS_Y - 1;

    CWorld::AdvanceCurrentScanCode();
    for (int32 y = startY; y <= endY; y++) {
        for (int32 x = startX; x <= endX; x++) {
            // Como el original: recorrido directo de los nodos de la lista de edificios del sector
            for (auto* node = CWorld::GetSector(x, y).Buildings.GetNode(); node;) {
                auto* const item = node->Item;
                node = node->Next;

                if (item->m_bIsProcObject || item->IsScanCodeCurrent())
                    continue;
                item->SetCurrentScanCode(); // Como el original: se marca antes de mirar el área
                if (!item->IsInCurrentArea())
                    continue;

                const auto mi = item->GetModelInfo();
                if (mi->GetModelType() != MODEL_INFO_ATOMIC || !mi->bAtomicFlag0x200)
                    continue;
                if (_ColEntityCache_FindInCache(item))
                    continue;
                if (_CalcDistanceSqrToEntity(item, cameraPos) <= PROC_OBJECTS_MAX_DISTANCE_SQUARED) {
                    if (!m_UnusedColEntListHead)
                        return false;
                    m_UnusedColEntListHead->AddEntry(item); // Como el original: el resultado no se mira
                    if (!m_UnusedColEntListHead)
                        return false;
                }
            }
        }
    }
    return true;
}

// 0x5DCD80
bool CPlantMgr::_ProcessEntryCollisionDataSections(const CPlantColEntEntry& entry, const CVector& center, int32 iTriProcessSkipMask) {
    const auto entity = entry.m_Entity;
    const auto cd = entity->GetColData();
    if (!cd)
        return false;

    // Como el original: el número de triángulos del ColData se extiende con signo
    const int32 numTriangles = entry.m_numTriangles;
    if ((int32)(int16)cd->m_nNumTriangles != numTriangles)
        return false;

    _ProcessEntryCollisionDataSections_RemoveLocTris(entry, center, iTriProcessSkipMask, 0, numTriangles - 1);

    if (!cd->bHasFaceGroups) {
        _ProcessEntryCollisionDataSections_AddLocTris(entry, center, iTriProcessSkipMask, 0, numTriangles - 1);
        return true;
    }

    // Como el original: los grupos se recorren desde el que está justo antes del contador hacia atrás
    const auto tris = reinterpret_cast<const uint8*>(cd->m_pTriangles);
    const int32 numFaceGroups = *reinterpret_cast<const int32*>(tris - sizeof(int32));
    for (int32 k = 0; k < numFaceGroups; k++) {
        const auto& faceGroup = *reinterpret_cast<const ColHelpers::TFaceGroup*>(tris - sizeof(int32) - sizeof(ColHelpers::TFaceGroup) * (k + 1));

        CVector bb[2];
        std::memcpy(bb, &faceGroup.bb, sizeof(bb));
        CVector out[2];
        TransformPoints(out, 2, entity->GetMatrix(), bb);
        CBox box;
        box.Set(out[0], out[1]);
        box.Recalc();

        if (CCollision::TestSphereBox(CSphere{ center, PLANTS_MAX_DISTANCE }, box)) {
            // Como el original: índices de cara con signo (movsx)
            _ProcessEntryCollisionDataSections_AddLocTris(entry, center, iTriProcessSkipMask, (int16)faceGroup.first, (int16)faceGroup.last);
        }
    }
    return true;
}

// 0x5DC8B0
void CPlantMgr::_ProcessEntryCollisionDataSections_AddLocTris(const CPlantColEntEntry& entry, const CVector& center, int32 iTriProcessSkipMask, int32 start, int32 end) {
    const auto entity = entry.m_Entity;
    const auto cd = entity->GetColData();
    if (!cd)
        return;

    // Como el original: sumas z*z + y*y + x*x
    const auto DistSq = [](const CVector& d) { return d.z * d.z + d.y * d.y + d.x * d.x; };

    for (auto i = start; i <= end; i++) {
        if (iTriProcessSkipMask != 0xFAFAFAFA && iTriProcessSkipMask != (i & 7))
            continue;
        if (entry.m_Objects[i])
            continue;
        if (!m_UnusedLocTriListHead)
            continue;

        const auto& tri = cd->m_pTriangles[i];

        CVector local[3];
        cd->GetTrianglePoint(local[0], tri.vA);
        cd->GetTrianglePoint(local[1], tri.vB);
        cd->GetTrianglePoint(local[2], tri.vC);

        CVector v[3];
        TransformPoints(v, 3, entity->GetMatrix(), local); // Como el original: a un búfer distinto

        // Como el original: centroide ((v1 + v0) + v2) * 0.33333334, distancias medidas como centro - punto
        // y puntos medios como media de esas diferencias
        const CVector s01{ v[1].x + v[0].x, v[1].y + v[0].y, v[1].z + v[0].z };
        const CVector centroid{ (v[2].x + s01.x) * 0.33333334f, (v[2].y + s01.y) * 0.33333334f, (v[2].z + s01.z) * 0.33333334f };
        const CVector d0{ center.x - v[0].x, center.y - v[0].y, center.z - v[0].z };
        const CVector d1{ center.x - v[1].x, center.y - v[1].y, center.z - v[1].z };
        const CVector d2{ center.x - v[2].x, center.y - v[2].y, center.z - v[2].z };
        const CVector dc{ center.x - centroid.x, center.y - centroid.y, center.z - centroid.z };
        const CVector m01{ (d1.x + d0.x) * 0.5f, (d1.y + d0.y) * 0.5f, (d1.z + d0.z) * 0.5f };
        const CVector m12{ (d2.x + d1.x) * 0.5f, (d2.y + d1.y) * 0.5f, (d2.z + d1.z) * 0.5f };
        const CVector m02{ (d2.x + d0.x) * 0.5f, (d2.y + d0.y) * 0.5f, (d2.z + d0.z) * 0.5f };

        if (!(DistSq(d0) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(d1) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(d2) < PLANTS_MAX_DISTANCE_SQUARED
            || DistSq(dc) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(m01) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(m12) < PLANTS_MAX_DISTANCE_SQUARED
            || DistSq(m02) < PLANTS_MAX_DISTANCE_SQUARED)) {
            continue;
        }

        const bool createsPlants = g_surfaceInfos.CreatesPlants(tri.m_nMaterial);
        const bool createsObjects = g_surfaceInfos.CreatesObjects(tri.m_nMaterial);

        if (!createsPlants && !createsObjects)
            continue;

        const auto unusedHead = m_UnusedLocTriListHead;
        if (!unusedHead->Add(v[0], v[1], v[2], tri.m_nMaterial, tri.m_nLight, createsPlants, createsObjects))
            continue;

        entry.m_Objects[i] = unusedHead;
        if (unusedHead->m_createsObjects) {
            if (g_procObjMan.ProcessTriangleAdded(unusedHead)) {
                unusedHead->m_createdObjects = true;
            } else if (!unusedHead->m_createsPlants) {
                unusedHead->Release();
                entry.m_Objects[i] = nullptr;
            }
        }
    }
}

// 0x5DBF20
void CPlantMgr::_ProcessEntryCollisionDataSections_RemoveLocTris(const CPlantColEntEntry& entry, const CVector& center, int32 iTriProcessSkipMask, int32 start, int32 end) {
    const auto entity = entry.m_Entity;
    entity->GetColModel(); // Como el original: llamada cuyo resultado no se usa

    // Como el original: el primer vértice suma z*z + x*x + y*y y el resto z*z + y*y + x*x
    const auto DistSqZXY = [](const CVector& d) { return d.z * d.z + d.x * d.x + d.y * d.y; };
    const auto DistSq    = [](const CVector& d) { return d.z * d.z + d.y * d.y + d.x * d.x; };

    for (auto i = start; i <= end; i++) {
        if (auto object = entry.m_Objects[i]; object) {
            if (object->m_createsObjects && !object->m_createdObjects && g_procObjMan.ProcessTriangleAdded(object)) {
                object->m_createdObjects = true;
            }
        }

        if (iTriProcessSkipMask != 0xFAFAFAFA && iTriProcessSkipMask != (i & 7))
            continue;

        auto& object = entry.m_Objects[i];
        if (!object)
            continue;

        // Como el original: también se mira el centro del triángulo; los puntos medios son medias de las diferencias
        const CVector d1{ center.x - object->m_V1.x, center.y - object->m_V1.y, center.z - object->m_V1.z };
        const CVector d2{ center.x - object->m_V2.x, center.y - object->m_V2.y, center.z - object->m_V2.z };
        const CVector d3{ center.x - object->m_V3.x, center.y - object->m_V3.y, center.z - object->m_V3.z };
        const CVector dc{ center.x - object->m_Center.x, center.y - object->m_Center.y, center.z - object->m_Center.z };
        const CVector m12{ (d2.x + d1.x) * 0.5f, (d2.y + d1.y) * 0.5f, (d2.z + d1.z) * 0.5f };
        const CVector m23{ (d3.x + d2.x) * 0.5f, (d3.y + d2.y) * 0.5f, (d3.z + d2.z) * 0.5f };
        const CVector m13{ (d3.x + d1.x) * 0.5f, (d3.y + d1.y) * 0.5f, (d3.z + d1.z) * 0.5f };

        if (DistSqZXY(d1) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(d2) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(d3) < PLANTS_MAX_DISTANCE_SQUARED
            || DistSq(dc) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(m12) < PLANTS_MAX_DISTANCE_SQUARED || DistSq(m23) < PLANTS_MAX_DISTANCE_SQUARED
            || DistSq(m13) < PLANTS_MAX_DISTANCE_SQUARED) {
            continue;
        }

        object->Release();
        object = nullptr;
    }
}

// 0x5DCF00
bool CPlantMgr::_UpdateLocTris(const CVector& center, int32 a2) {
    for (auto i = m_CloseColEntListHead; i; i = i->m_NextEntry) {
        _ProcessEntryCollisionDataSections(*i, center, a2);
    }
    return true;
}

// 0x5DBE40
float CPlantMgr::_CalcDistanceSqrToEntity(CEntity* entity, const CVector& posn) {
    const auto colModel = entity->GetColModel();
    CVector dst;
    entity->TransformFromObjectSpace(dst, colModel->m_boundSphere.m_vecCenter);

    // Como el original: posn - centro, suma z*z + y*y + x*x
    const float dx = posn.x - dst.x, dy = posn.y - dst.y, dz = posn.z - dst.z;
    float d = std::sqrt(dz * dz + dy * dy + dx * dx);
    if (const auto r = colModel->m_boundSphere.m_fRadius; d > r) {
        d -= r;
    }

    return d * d;
}

bool CPlantMgr::DbgCountCachedEntities(uint32*) {
    return true;
}

bool CPlantMgr::DbgCountLocTrisAndPlants(uint32, uint32*, uint32*) {
    return true;
}

bool CPlantMgr::DbgRenderCachedEntities(uint32*) {
    return true;
}

bool CPlantMgr::DbgRenderLocTris() {
    return true;
}
