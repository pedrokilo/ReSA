/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CRenderer
 * Reconstrucción: COMPLETA — 33/33 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 33/33 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Renderer.h"

#include "Occlusion.h"
#include "PostEffects.h"
#include "Shadows.h"
#include "CarFXRenderer.h"

#include "toolsmenu/UIRenderer.h"
auto& gnRendererModelRequestFlags = StaticRef<uint32>(0xB745C4);
auto& gpOutEntitiesForGetObjectsInFrustum = StaticRef<CEntity**>(0xB76854);

namespace {
// Como el original: distancia sqrt((z*z + y*y) + x*x), el orden de sumas del x87 en SetupEntityVisibility,
// SetupBigBuildingVisibility, ShouldModelBeStreamed y la lista submarina de RenderEverythingBarRoads
float DistanceZYX(const CVector& a, const CVector& b) {
    const float dx = a.x - b.x, dy = a.y - b.y, dz = a.z - b.z;
    return std::sqrt(dz * dz + dy * dy + dx * dx);
}

// Como el original: atan2(num, den) con fpatan (precisión completa) menos `heading`, redondeado a float
float FpatanMinus(float num, float den, float heading) {
    float result;
    __asm {
        fld num
        fld den
        fpatan
        fsub heading
        fstp result
    }
    return result;
}

// Como el original: sectores con x * 0,02 + 60 y sectores LOD con x * 0,005 + 15 (multiplicación, no división)
float SectorfX(float coord) { return coord * 0.02f + 60.0f; }
float LodSectorfX(float coord) { return coord * 0.005f + 15.0f; }

// Como el original: el sector repetido se indexa con & 15 (también con coordenadas negativas)
CRepeatSector& RepeatSectorAnd15(int32 sectorX, int32 sectorY) {
    return CWorld::ms_aRepeatSectors[uint32(sectorY) & 15u][uint32(sectorX) & 15u];
}
} // namespace

void CRenderer::InjectHooks()
{
    RH_ScopedClass(CRenderer);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(RenderFadingInEntities, 0x5531E0);
    RH_ScopedInstall(RenderFadingInUnderwaterEntities, 0x553220);
    RH_ScopedInstall(RenderOneRoad, 0x553230);
    RH_ScopedInstall(RenderOneNonRoad, 0x553260);
    RH_ScopedInstall(RemoveVehiclePedLights, 0x553390);
    RH_ScopedInstall(AddEntityToRenderList, 0x5534B0);
    RH_ScopedInstall(GetLodRenderListBase, 0x5536D0);
    RH_ScopedInstall(GetLodDontRenderListBase, 0x5536E0);
    RH_ScopedInstall(ResetLodRenderLists, 0x5536F0);
    RH_ScopedInstall(AddToLodRenderList, 0x553710);
    RH_ScopedInstall(AddToLodDontRenderList, 0x553740);
    RH_ScopedInstall(ProcessLodRenderLists, 0x553770);
    RH_ScopedInstall(PreRender, 0x553910);
    RH_ScopedInstall(RenderRoads, 0x553A10);
    RH_ScopedInstall(RenderEverythingBarRoads, 0x553AA0);
    RH_ScopedInstall(RenderFirstPersonVehicle, 0x553D00);
    RH_ScopedInstall(SetupLightingForEntity, 0x553E40);
    RH_ScopedInstall(SetupEntityVisibility, 0x554230);
    RH_ScopedInstall(SetupMapEntityVisibility, 0x553F60);
    RH_ScopedInstall(SetupBigBuildingVisibility, 0x554650);
    RH_ScopedInstall(SetupScanLists, 0x553540);
    RH_ScopedInstall(ScanSectorList_ListModels, 0x5535D0);
    RH_ScopedInstall(ScanSectorList_ListModelsVisible, 0x553650);
    RH_ScopedInstall(ScanSectorList, 0x554840);
    RH_ScopedInstall(ScanBigBuildingList, 0x554B10);
    RH_ScopedInstall(ShouldModelBeStreamed, 0x554EB0);
    RH_ScopedInstall(ScanPtrList_RequestModels<CPtrListSingleLink<CPhysical*>>, 0x555680);
    RH_ScopedInstall(ConstructRenderList, 0x5556E0);
    RH_ScopedInstall(ScanSectorList_RequestModels, 0x555900);
    RH_ScopedInstall(ScanWorld, 0x554FE0);
    RH_ScopedInstall(GetObjectsInFrustum, 0x554C60);
    RH_ScopedInstall(RequestObjectsInFrustum, 0x555960);
    RH_ScopedInstall(RequestObjectsInDirection, 0x555CB0);

}

void CWorldScan::ScanWorld(CVector2D *points, int32 pointsCount, tScanFunction scanFunction)
{
    plugin::Call<0x72CAE0,CVector2D*, int32, tScanFunction>(points,pointsCount, scanFunction);
}

void CWorldScan::SetExtraRectangleToScan(float minX, float maxX, float minY, float maxY)
{
    plugin::Call<0x72D5E0, float, float, float, float>(minX, maxX, minY, maxY);
}

void CRenderer::Init() {
    // empty
}

void CRenderer::Shutdown() {
    // empty
}

// 0x5531E0
void CRenderer::RenderFadingInEntities() {
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));
    DeActivateDirectional();
    SetAmbientColours();
    CVisibilityPlugins::RenderFadingEntities();
}

// 0x553220
void CRenderer::RenderFadingInUnderwaterEntities() {
    DeActivateDirectional();
    SetAmbientColours();
    CVisibilityPlugins::RenderFadingUnderwaterEntities();
}

// 0x553230
void CRenderer::RenderOneRoad(CEntity* entity) {
    if (CPostEffects::IsVisionFXActive()) {
        CPostEffects::FilterFX_StoreAndSetDayNightBalance();
        entity->Render();
        CPostEffects::FilterFX_RestoreDayNightBalance();
        return;
    }
    entity->Render();
}

// 0x553260
void CRenderer::RenderOneNonRoad(CEntity* entity) {
    if (entity->GetIsTypePed() && entity->AsPed()->m_nPedState == PEDSTATE_DRIVING)
        return;

    bool bSetupLighting = entity->SetupLighting();
    auto* vehicle = entity->AsVehicle();
    if (entity->GetIsTypeVehicle()) {
        CVisibilityPlugins::SetupVehicleVariables(entity->GetRpClump());
        CVisibilityPlugins::InitAlphaAtomicList();
        vehicle->RenderDriverAndPassengers();
        vehicle->SetupRender();
    } else if (!entity->GetIsBackfaceCulled()) {
        RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLNONE));
    }

    if (CPostEffects::IsVisionFXActive()) {
        if (CPostEffects::m_bNightVision)
            CPostEffects::NightVisionSetLights();
        if (CPostEffects::m_bInfraredVision)
            CPostEffects::InfraredVisionSetLightsForDefaultObjects();
        CPostEffects::FilterFX_StoreAndSetDayNightBalance();
        entity->Render();
        CPostEffects::FilterFX_RestoreDayNightBalance();
    } else {
        entity->Render();
    }

    if (entity->GetIsTypeVehicle()) {
        vehicle->m_bImBeingRendered = true;
        CVisibilityPlugins::RenderAlphaAtomics();
        vehicle->m_bImBeingRendered = false;
        vehicle->ResetAfterRender();
        vehicle->RemoveLighting(bSetupLighting);
    } else {
        if (!entity->GetIsBackfaceCulled())
            RwRenderStateSet(rwRENDERSTATECULLMODE, RWRSTATE(rwCULLMODECULLBACK));
        entity->RemoveLighting(bSetupLighting);
    }
}

// 0x553390
void CRenderer::RemoveVehiclePedLights(CPhysical* entity) {
    if (!entity->physicalFlags.bRenderScorched)
        CPointLights::RemoveLightsAffectingObject();
}

// 0x05534B0
void CRenderer::AddEntityToRenderList(CEntity* entity, float fDistance)
{
    CBaseModelInfo* mi = entity->GetModelInfo();
    mi->SetHasBeenPreRendered(false);
    if (!entity->m_bDistanceFade) {
        if (entity->m_bDrawLast && CVisibilityPlugins::InsertEntityIntoSortedList(entity, fDistance)) {
            entity->m_bDistanceFade = false;
            return;
        }
    }
    else if (CVisibilityPlugins::InsertEntityIntoSortedList(entity, fDistance)) {
        return;
    }
    if (entity->GetNumLodChildren() && !entity->m_bUnderwater) {
        ms_aVisibleLodPtrs[ms_nNoOfVisibleLods] = entity;
        ms_nNoOfVisibleLods++;
        assert(ms_nNoOfVisibleLods <= MAX_VISIBLE_LOD_PTRS);
    }
    else {
        ms_aVisibleEntityPtrs[ms_nNoOfVisibleEntities] = entity;
        ms_nNoOfVisibleEntities++;
        assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);
    }
}

// 0x5536D0
tRenderListEntry* CRenderer::GetLodRenderListBase() {
    return reinterpret_cast<tRenderListEntry*>(&PC_Scratch[24]);
}

// 0x5536E0
tRenderListEntry* CRenderer::GetLodDontRenderListBase() {
    return reinterpret_cast<tRenderListEntry*>(&PC_Scratch[0x2000]);
}

// 0x5536F0
void CRenderer::ResetLodRenderLists() {
    ms_pLodRenderList = GetLodRenderListBase();
    ms_pLodDontRenderList = GetLodDontRenderListBase();
}

// 0x553710
void CRenderer::AddToLodRenderList(CEntity* entity, float distance) {
    ms_pLodRenderList->entity = entity;
    ms_pLodRenderList->distance = distance;
    ++ms_pLodRenderList;
}

// unused
// 0x553740
void CRenderer::AddToLodDontRenderList(CEntity* entity, float distance) {
    ms_pLodDontRenderList->entity = entity;
    ms_pLodDontRenderList->distance = distance;
    ++ms_pLodDontRenderList;
}

// 0x553770
void CRenderer::ProcessLodRenderLists() {
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->entity;
        if (entity && !entity->GetIsVisible()) {
            entity->ResetLodChildrenRendered();
            renderListEntry->entity = nullptr;
        }
    }

    // Como el original: do/while (el bucle se ejecuta al menos una vez y se repite mientras se libere algún LOD)
    bool bAllLodsRendered;
    do {
        bAllLodsRendered = false;
        for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
            CEntity* entity = renderListEntry->entity;
            if (entity) {
                if (entity->HasLodChildrenRendered() && entity->GetNumLodChildrenRendered() == entity->GetNumLodChildren()) {
                    entity->ResetLodChildrenRendered();
                    renderListEntry->entity = nullptr;
                    bAllLodsRendered = true;
                }
                else if (entity->GetLod()) {
                    auto modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                    if (modelInfo->m_nAlpha < 255u
                        && entity->GetLod()->CanLodChildrenRender()
                        && entity->GetLod()->m_bDisplayedSuperLowLOD
                    ) {
                        entity->GetLod()->ResetLodChildrenRendered();
                    }
                    if (!entity->GetRwObject()) {
                        if (entity->GetLod()->m_bDisplayedSuperLowLOD)
                            entity->GetLod()->SetCannotLodChildrenRender();
                        renderListEntry->entity = nullptr;
                        entity->ResetLodChildrenRendered();
                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                    }
                }
            }
        }
    } while (bAllLodsRendered);
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->entity;
        if (entity && entity->HasLodChildrenRendered()) {
            entity->m_bDisplayedSuperLowLOD = false;
            entity->ResetLodChildrenRendered();
            renderListEntry->entity = nullptr;
        }
    }
    for (auto renderListEntry = GetLodRenderListBase(); renderListEntry != ms_pLodRenderList; renderListEntry++) {
        CEntity* entity = renderListEntry->entity;
        if (entity) {
            if (!entity->CanLodChildrenRender() || !entity->GetNumLodChildrenRendered())
            {
                entity->m_bDisplayedSuperLowLOD = true;
                auto modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
                if (modelInfo->m_nAlpha != 255)
                    entity->m_bDistanceFade = true;
                AddEntityToRenderList(entity, renderListEntry->distance);
            }
            entity->ResetLodChildrenRendered();
        }
    }
}

// 0x553910
void CRenderer::PreRender() {
    ZoneScoped;

    assert(ms_nNoOfVisibleLods <= MAX_VISIBLE_LOD_PTRS);
    std::ranges::for_each(GetVisibleLodPtrs(), [](auto& entity) { entity->PreRender(); });

    assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);
    std::ranges::for_each(GetVisibleEntityPtrs(), [](auto& entity) { entity->PreRender(); });

    assert(ms_nNoOfVisibleSuperLods <= MAX_VISIBLE_SUPERLOD_PTRS);
    std::ranges::for_each(GetVisibleSuperLodPtrs(), [](auto& entity) { entity->PreRender(); });

    assert(ms_nNoOfInVisibleEntities <= MAX_INVISIBLE_ENTITY_PTRS);
    std::ranges::for_each(GetInVisibleEntityPtrs(), [](auto& entity) { entity->PreRender(); });

    for (auto* link = CVisibilityPlugins::GetAlphaList().usedListHead.next;
        link != &CVisibilityPlugins::GetAlphaList().usedListTail;
        link = link->next
    ) {
        // NOTSA: HACK: We compare function pointers, and want it to work with reversible hooks,
        // we need to check for both original function and our one
        if (link->data.m_pCallback == CVisibilityPlugins::RenderEntity || link->data.m_pCallback == (void*)0x732B40) {
            auto* entity = (CEntity*)link->data.m_pObj;
            entity->m_bOffscreen = false;
            entity->PreRender();
        }
    }

    for (auto* link = CVisibilityPlugins::GetAlphaUnderwaterList().usedListHead.next;
        link != &CVisibilityPlugins::GetAlphaUnderwaterList().usedListTail;
        link = link->next
    ) {
        // todo: NOTSA: HACK: We compare function pointers, and want it to work with reversible hooks,
        // we need to check for both original function and our one
        if (link->data.m_pCallback == CVisibilityPlugins::RenderEntity || link->data.m_pCallback == (void*)0x732B40) {
            auto* entity = (CEntity*)link->data.m_pObj;
            entity->m_bOffscreen = false;
            entity->PreRender();
        }
    }
    CHeli::SpecialHeliPreRender();
    CShadows::RenderExtraPlayerShadows();
}

// 0x553A10
void CRenderer::RenderRoads() {
    assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);

    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));

    DeActivateDirectional();
    SetAmbientColours();

    for (auto& entity : GetVisibleEntityPtrs()) {
        if (entity->GetIsTypeBuilding() && CModelInfo::GetModelInfo(entity->m_nModelIndex)->IsRoad()) {
            if (CPostEffects::IsVisionFXActive()) {
                CPostEffects::FilterFX_StoreAndSetDayNightBalance();
                entity->Render();
                CPostEffects::FilterFX_RestoreDayNightBalance();
            }
            else {
                entity->Render();
            }
        }
    }
}

// 0x553AA0
void CRenderer::RenderEverythingBarRoads() {
    RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
    RwRenderStateSet(rwRENDERSTATECULLMODE,          RWRSTATE(rwCULLMODECULLBACK));
    if (!CGame::currArea)
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(140u));

    assert(ms_nNoOfVisibleEntities <= MAX_VISIBLE_ENTITY_PTRS);
    for (auto& entity : GetVisibleEntityPtrs()) {
        auto* vehicle = entity->AsVehicle();
        if (entity->GetIsTypeBuilding() && CModelInfo::GetModelInfo(entity->m_nModelIndex)->IsRoad())
            continue;

        bool bInserted = false;
        if (entity->GetIsTypeVehicle() || (entity->GetIsTypePed() && CVisibilityPlugins::GetClumpAlpha(entity->GetRpClump()) != 255)) {
            // Como el original: los peatones con alpha != 255 también van a la lista ordenada
            bool bInsertIntoSortedList = true;
            if (entity->GetIsTypeVehicle()) {
                if (vehicle->IsBoat()) {
                    // Como el original: GetLookDirection solo se consulta en primera persona (y dos veces)
                    const auto camMode = CCamera::GetActiveCamera().m_nMode;
                    bInsertIntoSortedList = camMode == MODE_WHEELCAM
                        || camMode == MODE_1STPERSON && TheCamera.GetLookDirection() != LOOKING_DIRECTION_FORWARD && TheCamera.GetLookDirection() != LOOKING_DIRECTION_UNKNOWN_1
                        || CVisibilityPlugins::GetClumpAlpha(entity->GetRpClump()) != 255;
                } else {
                    bInsertIntoSortedList = !vehicle->physicalFlags.bTouchingWater;
                }
            }
            const CVector& entityPos = entity->GetPosition();
            const float dx = ms_vecCameraPosition.x - entityPos.x;
            const float dy = ms_vecCameraPosition.y - entityPos.y;
            const float dz = ms_vecCameraPosition.z - entityPos.z;
            // Como el original: orden de sumas distinto en cada rama (z, y, x para la lista ordenada; z, x, y para la submarina)
            if (bInsertIntoSortedList)
                bInserted = CVisibilityPlugins::InsertEntityIntoSortedList(entity, std::sqrt(dz * dz + dy * dy + dx * dx));
            else
                bInserted = CVisibilityPlugins::InsertEntityIntoUnderwaterList(entity, std::sqrt(dz * dz + dx * dx + dy * dy));
        }
        if (!bInserted)
            RenderOneNonRoad(entity);
    }
    float oldzShift = Scene.m_pRwCamera->zShift;
    RwCameraEndUpdate(Scene.m_pRwCamera);

    Scene.m_pRwCamera->zShift -= 100.0f;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
    for (auto& entity : GetVisibleLodPtrs()) {
        RenderOneNonRoad(entity);
    }
    RwCameraEndUpdate(Scene.m_pRwCamera);

    Scene.m_pRwCamera->zShift = oldzShift;
    RwCameraBeginUpdate(Scene.m_pRwCamera);
}

// 0x553D00
void CRenderer::RenderFirstPersonVehicle() {
    ZoneScoped;

    if (m_pFirstPersonVehicle) {
        bool bRestoreAlphaTest = false;
        // Como el original: FindPlayerPed(0) está inlinado (lee CWorld::Players[0].m_pPed sin llamar)
        if (CWorld::Players[0].m_pPed->GetActiveWeapon().m_Type == WEAPON_MICRO_UZI) {
            bRestoreAlphaTest = true;
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(80u));
        }
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEZWRITEENABLE,      RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEZTESTENABLE,       RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATEVERTEXALPHAENABLE, RWRSTATE(TRUE));
        RwRenderStateSet(rwRENDERSTATESRCBLEND,          RWRSTATE(rwBLENDSRCALPHA));
        RwRenderStateSet(rwRENDERSTATEDESTBLEND,         RWRSTATE(rwBLENDINVSRCALPHA));
        RenderOneNonRoad(m_pFirstPersonVehicle);
        RwRenderStateSet(rwRENDERSTATEFOGENABLE,         RWRSTATE(FALSE));
        if (bRestoreAlphaTest)
            RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(NULL));
    }
}

// 0x553E40
bool CRenderer::SetupLightingForEntity(CPhysical* entity) {
    CCarFXRenderer::SetFxEnvMapLightMult(1.0f);
    if (entity->physicalFlags.bRenderScorched) {
        WorldReplaceNormalLightsWithScorched(Scene.m_pRpWorld, 0.18f);
        return false;
    }
    entity->m_fDynamicLighting = 0.0f;
    const CVector point = entity->GetPosition(); // Como el original: copia local de la posición
    float generatedLightings = CPointLights::GenerateLightsAffectingObject(&point, &entity->m_fDynamicLighting, entity);
    float lightingMultiplier = (entity->GetLightingFromCol(true) * (1.0f - 0.05f) + 0.05f) * generatedLightings;
    CCarFXRenderer::SetFxEnvMapLightMult(lightingMultiplier);
    SetLightColoursForPedsCarsAndObjects(lightingMultiplier);
    return true;
}

// 0x553F60
int32 CRenderer::SetupMapEntityVisibility(CEntity* entity, CBaseModelInfo* baseModelInfo, float fDistance, bool bIsTimeInRange) {
    if (!entity->m_bTunnelTransition) {
        if (!ms_bRenderTunnels && entity->m_bTunnel || (!ms_bRenderOutsideTunnels && !entity->m_bTunnel)) {
            return RENDERER_INVISIBLE;
        }
    }

    const float fFarClipRadius = baseModelInfo->GetColModel()->GetBoundRadius() + ms_fFarClipPlane;
    // Como el original: se queda con el radio de recorte salvo que la distancia sea estrictamente menor (NaN -> radio)
    float fDrawDistanceRadius = TheCamera.m_fLODDistMultiplier * baseModelInfo->m_fDrawDistance;
    if (!(fDrawDistanceRadius < fFarClipRadius))
        fDrawDistanceRadius = fFarClipRadius;
    float fFadingDistance = MAX_FADING_DISTANCE;
    if (!entity->GetLod()) {
        float fDrawDistance = baseModelInfo->m_fDrawDistance;
        if (!(fDrawDistance < fDrawDistanceRadius))
            fDrawDistance = fDrawDistanceRadius;
        if (fDrawDistance > MAX_LOWLOD_DISTANCE)
            fFadingDistance = fDrawDistance * std::bit_cast<float>(0x3D888889u) + 10.0f; // Como el original: * 0,06666667 (flt_863E0C), no / 15
        if (entity->m_bIsBIGBuilding)
            fDrawDistanceRadius *= ms_lowLodDistScale;
    }

    if (!baseModelInfo->GetRwObject()) {
        if (entity->GetLod() && entity->GetLod()->GetNumLodChildren() > 1u &&
            fFadingDistance + fDistance - MAX_FADING_DISTANCE < fDrawDistanceRadius)
        {
            AddToLodRenderList(entity, fDistance);
            return RENDERER_STREAMME;
        }
    }

    if (!baseModelInfo->GetRwObject() || !(fFadingDistance + fDistance - MAX_FADING_DISTANCE < fDrawDistanceRadius)) { // Como el original: NaN entra aquí
        if (entity->m_bDontStream)
            return RENDERER_INVISIBLE;
        if (baseModelInfo->GetRwObject() && fDistance - MAX_FADING_DISTANCE < fDrawDistanceRadius) {
            if (!entity->GetRwObject()) {
                entity->CreateRwObject();
                if (!entity->GetRwObject())
                    return RENDERER_INVISIBLE;
            }
            if (!entity->GetIsVisible())
                return RENDERER_INVISIBLE;
            if (!entity->GetIsOnScreen() || entity->IsEntityOccluded()) {
                if (!baseModelInfo->HasBeenPreRendered()) {
                    baseModelInfo->m_nAlpha = 255;
                }
                baseModelInfo->SetHasBeenPreRendered(false);
                return RENDERER_INVISIBLE;
            }
            entity->m_bDistanceFade = true;
            if (entity->GetLod() && entity->GetLod()->GetNumLodChildren() > 1u)
                AddToLodRenderList(entity, fDistance);
            else
                AddEntityToRenderList(entity, fDistance);
            return RENDERER_INVISIBLE;
        }
        if (!(fDistance - MAX_STREAMING_DISTANCE < fDrawDistanceRadius) || !bIsTimeInRange || !entity->GetIsVisible()) // Como el original: NaN es invisible
            return RENDERER_INVISIBLE;
        if (!entity->GetRwObject())
            entity->CreateRwObject();
        return RENDERER_STREAMME;
    }

    if (!entity->GetRwObject()) {
        entity->CreateRwObject();
        if (!entity->GetRwObject())
            return RENDERER_INVISIBLE;
    }

    if (!entity->GetIsVisible())
        return RENDERER_INVISIBLE;

    if (entity->GetIsOnScreen() && !entity->IsEntityOccluded()) {
        if (baseModelInfo->m_nAlpha == 255)
            entity->m_bDistanceFade = false;
        else
            entity->m_bDistanceFade = true;
        if (!entity->GetLod())
            return RENDERER_VISIBLE;
        if (baseModelInfo->m_nAlpha == 255)
            entity->GetLod()->AddLodChildrenRendered();
        if (entity->GetLod()->GetNumLodChildren() <= 1u)
            return RENDERER_VISIBLE;
        AddToLodRenderList(entity, fDistance);
        return RENDERER_INVISIBLE;
    }
    if (!baseModelInfo->HasBeenPreRendered())
        baseModelInfo->m_nAlpha = 255;
    baseModelInfo->SetHasBeenPreRendered(false);
    return RENDERER_CULLED;
}

// 0x554230
int32 CRenderer::SetupEntityVisibility(CEntity* entity, float& outDistance) {
    const int32& modelId = entity->m_nModelIndex;
    CBaseModelInfo* baseModelInfo = CModelInfo::GetModelInfo(modelId);
    CBaseModelInfo* baseAtomicModelInfo = baseModelInfo->AsAtomicModelInfoPtr();
    if (entity->GetIsTypeVehicle() && !entity->m_bTunnelTransition) {
        if (!ms_bRenderTunnels && entity->m_bTunnel || !ms_bRenderOutsideTunnels && !entity->m_bTunnel)
            return RENDERER_INVISIBLE;
    }

    bool bIsTimeInRange = true;
    if (!baseAtomicModelInfo)
    {
        if (baseModelInfo->GetModelType() != MODEL_INFO_CLUMP && baseModelInfo->GetModelType() != MODEL_INFO_WEAPON)
        {
            if (FindPlayerVehicle() == entity && gbFirstPersonRunThisFrame && CReplay::Mode != MODE_PLAYBACK) {
                uint32 dwDirectionWasLooking = CCamera::GetActiveCamera().m_nDirectionWasLooking;
                CVehicle* vehicle = entity->AsVehicle();
                // Como el original: FindPlayerVehicle se vuelve a llamar para el tipo y para los flags de la moto
                if (!FindPlayerVehicle()->IsBike() || !(FindPlayerVehicle()->AsBike()->bikeFlags.bWheelieForCamera))
                {
                    if (dwDirectionWasLooking == 3)
                        return RENDERER_CULLED;

                    if (modelId == MODEL_RHINO || modelId == MODEL_COACH || TheCamera.m_bInATunnelAndABigVehicle)
                        return RENDERER_CULLED;

                    if (dwDirectionWasLooking) {
                        m_pFirstPersonVehicle = entity->AsVehicle();
                        return RENDERER_CULLED;
                    }

                    if (vehicle->m_pHandlingData->m_bNo1fpsLookBehind)
                        return RENDERER_CULLED;

                    if (!vehicle->IsBoat()
                        || modelId == MODEL_REEFER || modelId == MODEL_TROPIC || modelId == MODEL_PREDATOR
                        || modelId == MODEL_SKIMMER
                    ) {
                        m_pFirstPersonVehicle = entity->AsVehicle();
                        return RENDERER_CULLED;
                    }
                }
            }

            if (!entity->GetRwObject()
                || !entity->GetIsVisible() && (!CMirrors::TypeOfMirror || entity->m_nModelIndex)
                || !entity->IsInCurrentArea() && entity->GetIsTypeVehicle()
            ) {
                return RENDERER_INVISIBLE;
            }

            if (!entity->GetIsOnScreen() || entity->IsEntityOccluded()) {
                return RENDERER_CULLED;
            }

            if (entity->m_bDrawLast) { // Como el original: bit 14 (m_bDrawLast), no m_bWasPostponed
                entity->m_bDistanceFade = false;
                AddEntityToRenderList(entity, DistanceBetweenPoints(ms_vecCameraPosition, entity->GetPosition()));
                return RENDERER_INVISIBLE;
            }
            return RENDERER_VISIBLE;
        }
    }
    else
    {
        if (baseModelInfo->GetModelType() == MODEL_INFO_TIME)
        {
            CTimeInfo* modelTimeInfo = baseModelInfo->GetTimeInfo();
            int32 otherTimeModel = modelTimeInfo->GetOtherTimeModel();
            if (CClock::GetIsTimeInRange(modelTimeInfo->GetTimeOn(), modelTimeInfo->GetTimeOff()))
            {
                if (otherTimeModel != -1 && CModelInfo::GetModelInfo(otherTimeModel)->GetRwObject()) {
                    baseModelInfo->m_nAlpha = 255;
                }
            }
            else
            {
                if (otherTimeModel == -1 || CModelInfo::GetModelInfo(otherTimeModel)->GetRwObject())
                {
                    entity->DeleteRwObject();
                    return RENDERER_INVISIBLE;
                }
                bIsTimeInRange = false;
            }
        }
        else
        {
            if (entity->m_bDontStream)
            {
                if (!entity->GetRwObject() || !entity->GetIsVisible() && (!CMirrors::TypeOfMirror || entity->m_nModelIndex))
                {
                    return RENDERER_INVISIBLE;
                }

                if (!entity->GetIsOnScreen() || entity->IsEntityOccluded())
                {
                    return RENDERER_CULLED;
                }

                if (!entity->m_bDrawLast) // Como el original: bit 14 (m_bDrawLast), no m_bWasPostponed
                {
                    return RENDERER_VISIBLE;
                }

                CVisibilityPlugins::InsertEntityIntoSortedList(entity, (entity->GetPosition() - ms_vecCameraPosition).Magnitude());
                entity->m_bDistanceFade = false;
                return RENDERER_INVISIBLE;
            }
        }
    }

    if (entity->IsInCurrentArea()) {
        CVector position = entity->GetPosition();
        if (entity->GetLod()) {
            position = entity->GetLod()->GetPosition();
        }

        outDistance = DistanceZYX(position, ms_vecCameraPosition);
        if (outDistance > MAX_LOD_DISTANCE) {
            const float fDrawDistanceRadius = TheCamera.m_fLODDistMultiplier * baseModelInfo->m_fDrawDistance;
            if (fDrawDistanceRadius > MAX_LOD_DISTANCE &&
                fDrawDistanceRadius + MAX_FADING_DISTANCE > outDistance
            ) {
                outDistance += fDrawDistanceRadius - MAX_LOD_DISTANCE;
            }
        }
        return SetupMapEntityVisibility(entity, baseModelInfo, outDistance, bIsTimeInRange);
    }

    return RENDERER_INVISIBLE;
}

// 0x554650
int32 CRenderer::SetupBigBuildingVisibility(CEntity* entity, float& outDistance) {
    CBaseModelInfo* baseModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    bool bIsTimeInRange = true;
    if (!entity->IsInCurrentArea())
        return RENDERER_INVISIBLE;

    if (baseModelInfo->GetModelType() == MODEL_INFO_TIME)
    {
        CTimeInfo* timeInfo = baseModelInfo->GetTimeInfo();
        int32 otherTimeModel = timeInfo->GetOtherTimeModel();
        if (CClock::GetIsTimeInRange(timeInfo->GetTimeOn(), timeInfo->GetTimeOff()))
        {
            if (otherTimeModel != -1 && CModelInfo::GetModelInfo(otherTimeModel)->GetRwObject())
                baseModelInfo->m_nAlpha = 255;
        }
        else
        {
            if (otherTimeModel == -1 || CModelInfo::GetModelInfo(otherTimeModel)->GetRwObject()) {
                entity->DeleteRwObject();
                return RENDERER_INVISIBLE;
            }
            bIsTimeInRange = false;
        }
    }
    else if (baseModelInfo->GetModelType() == MODEL_INFO_VEHICLE) {
        return entity->IsVisible() && !entity->IsEntityOccluded() ? RENDERER_VISIBLE : RENDERER_INVISIBLE;
    }

    CVector entityPos = entity->GetPosition();
    if (entity->GetLod()) {
        entityPos = entity->GetLod()->GetPosition();
    }

    outDistance = DistanceZYX(entityPos, ms_vecCameraPosition);
    if (!entity->HasLodChildrenRendered()) {
        int32 visibility = SetupMapEntityVisibility(entity, baseModelInfo, outDistance, bIsTimeInRange);
        if (visibility != RENDERER_VISIBLE || entity->GetNumLodChildren() <= 1u) {
            return visibility;
        }
        if (entity->GetLod() && baseModelInfo->m_nAlpha == 255) {
            entity->GetLod()->AddLodChildrenRendered();
        }
        AddToLodRenderList(entity, outDistance);
        return RENDERER_INVISIBLE;
    }

    if (entity->GetLod())
        entity->GetLod()->AddLodChildrenRendered();

    if (entity->GetNumLodChildren() <= 1u) {
        entity->ResetLodChildrenRendered();
    } else {
        ms_pLodRenderList->entity = entity;
        ms_pLodRenderList->distance = outDistance;
        ms_pLodRenderList++;
    }

    if (!entity->GetRwObject())
        entity->CreateRwObject();

    return RENDERER_STREAMME;
}

template<bool CheckIsVisible>
void I_ScanSectorList_ListModels(int32 sectorX, int32 sectorY) {
    CRenderer::SetupScanLists(sectorX, sectorY);
    auto* const scanLists = reinterpret_cast<tScanLists*>(&PC_Scratch);
    const auto ProcessList = [&]<typename PtrListType>(PtrListType* list) {
        if (!list) {
            // Como el original: ScanSectorList_ListModelsVisible (0x553650) no avanza el puntero de listas al
            // encontrar una lista nula, así que no recorre ninguna de las siguientes. ListModels sí sigue.
            return !CheckIsVisible;
        }
        for (auto* const entity : *list) {
            if (entity->IsScanCodeCurrent()) {
                continue;
            }
            entity->SetCurrentScanCode();
            if (!entity->IsInCurrentArea()) {
                continue;
            }
            if constexpr (CheckIsVisible) {
                if (!entity->IsVisible()) {
                    continue;
                }
            }
            *gpOutEntitiesForGetObjectsInFrustum = entity;
            gpOutEntitiesForGetObjectsInFrustum++;
        }
        return true;
    };
    (void)(ProcessList(scanLists->buildingsList) && ProcessList(scanLists->objectsList) && ProcessList(scanLists->vehiclesList)
        && ProcessList(scanLists->pedsList) && ProcessList(scanLists->dummiesList));
}

// 0x5535D0
void CRenderer::ScanSectorList_ListModels(int32 sectorX, int32 sectorY) {
    I_ScanSectorList_ListModels<false>(sectorX, sectorY);
}

// 0x553650
void CRenderer::ScanSectorList_ListModelsVisible(int32 sectorX, int32 sectorY) {
    I_ScanSectorList_ListModels<true>(sectorX, sectorY);
}

// 0x554840
void CRenderer::ScanSectorList(int32 sectorX, int32 sectorY) {
    bool bRequestModel = false;
    const float fDistanceX = CWorld::GetSectorPosX(sectorX) - ms_vecCameraPosition.x;
    const float fDistanceY = CWorld::GetSectorPosY(sectorY) - ms_vecCameraPosition.y;
    // Como el original: el ángulo (fpatan de precisión completa menos el heading) solo se calcula si el sector no está cerca
    if (fDistanceY * fDistanceY + fDistanceX * fDistanceX < MAX_STREAMING_RADIUS_SQUARED ||
        std::fabs(CGeneral::LimitRadianAngle(FpatanMinus(-fDistanceX, fDistanceY, ms_fCameraHeading))) < STREAMING_ANGLE_THRESHOLD_RAD
    ) {
        bRequestModel = true;
    }

    CRenderer::SetupScanLists(sectorX, sectorY);
    // Como el original: la distancia no se reinicia por entidad; SetupEntityVisibility no la escribe en todos los
    // caminos y AddEntityToRenderList recibe entonces la de la entidad anterior (la inicial es basura de pila en el EXE)
    float fDistance = 0.0f;
    reinterpret_cast<tScanLists*>(&PC_Scratch)->VisitLists([&]<typename PtrListType>(PtrListType& list) {
        for (auto* const entity : list) {
            if (entity->IsScanCodeCurrent())
                continue;

            entity->SetCurrentScanCode();
            entity->m_bOffscreen = false;
            bool bInvisibleEntity = false;
            switch (SetupEntityVisibility(entity, fDistance)) {
            case RENDERER_INVISIBLE: {
                if (entity->GetIsTypeObject()) {
                    auto* atomicModelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex)->AsAtomicModelInfoPtr();
                    if (atomicModelInfo && atomicModelInfo->IsGlass()) {
                        bInvisibleEntity = true;
                    }
                }
                break;
            }
            case RENDERER_VISIBLE: {
                AddEntityToRenderList(entity, fDistance);
                break;
            }
            case RENDERER_CULLED: {
                bInvisibleEntity = true;
                break;
            }
            case RENDERER_STREAMME: {
                if (CStreaming::ms_disableStreaming || !entity->GetIsOnScreen() || ms_bInTheSky)
                    break;

                // Como el original: si el modelo no está cargado se consulta la oclusión antes que bRequestModel
                if (CStreaming::GetInfo(entity->m_nModelIndex).IsLoaded()) {
                    if (bRequestModel) {
                        CStreaming::RequestModel(entity->m_nModelIndex, 0);
                        break;
                    }
                } else if (!entity->IsEntityOccluded() && bRequestModel) {
                    SetLoadingPriority(1);
                    CStreaming::RequestModel(entity->m_nModelIndex, 0);
                    break;
                }

                if (!m_loadingPriority || CStreaming::ms_numModelsRequested < 1)
                    CStreaming::RequestModel(entity->m_nModelIndex, 0);

                break;
            }
            default:
                break;
            }

            if (!bInvisibleEntity)
                continue;

            entity->m_bOffscreen = true;
            if (entity->m_bHasPreRenderEffects) {
                float fDrawDistance = MAX_INVISIBLE_ENTITY_DISTANCE;
                const CVector& entityPos = entity->GetPosition();
                const float fDiffX = ms_vecCameraPosition.x - entityPos.x;
                if (entity->GetIsTypeVehicle()) {
                    if (entity->AsVehicle()->vehicleFlags.bAlwaysSkidMarks) {
                        fDrawDistance = MAX_INVISIBLE_VEHICLE_DISTANCE;
                    }
                }
                if (fDiffX > -fDrawDistance && fDiffX < fDrawDistance) {
                    const float fDiffY = ms_vecCameraPosition.y - entity->GetPosition().y;
                    if (fDiffY > -fDrawDistance && fDiffY < fDrawDistance) {
                        // Como el original: comparación con signo (jge 149)
                        if (ms_nNoOfInVisibleEntities < (int32)MAX_INVISIBLE_ENTITY_PTRS - 1) {
                            ms_aInVisibleEntityPtrs[ms_nNoOfInVisibleEntities] = entity;
                            ms_nNoOfInVisibleEntities++;
                        }
                    }
                }
            }
        }
    });
}

// 0x554B10
void CRenderer::ScanBigBuildingList(int32 sectorX, int32 sectorY) {
    if (sectorX < 0 || sectorY < 0 || sectorX >= MAX_LOD_PTR_LISTS_X || sectorY >= MAX_LOD_PTR_LISTS_Y)
        return;

    bool bRequestModel = false;
    const float fDistanceX = CWorld::GetLodSectorPosX(sectorX) - ms_vecCameraPosition.x;
    const float fDistanceY = CWorld::GetLodSectorPosY(sectorY) - ms_vecCameraPosition.y;
    // Como el original: el ángulo (fpatan de precisión completa menos el heading) solo se calcula si el sector no está cerca
    if (fDistanceY * fDistanceY + fDistanceX * fDistanceX < MAX_BIGBUILDING_STREAMING_RADIUS_SQUARED ||
        std::fabs(CGeneral::LimitRadianAngle(FpatanMinus(-fDistanceX, fDistanceY, ms_fCameraHeading))) <= BIGBUILDING_STREAMING_ANGLE_THRESHOLD_RAD
    ) {
        bRequestModel = true;
    }

    // Como el original: la distancia no se reinicia por entidad (ver ScanSectorList)
    float fDistance = 0.0f;
    for (auto* const entity : CWorld::GetLodPtrList(sectorX, sectorY)) {
        if (entity->IsScanCodeCurrent())
            continue;

        entity->SetCurrentScanCode();

        switch (SetupBigBuildingVisibility(entity, fDistance)) {
        case RENDERER_STREAMME:
            if (!CStreaming::ms_disableStreaming && bRequestModel) {
                CStreaming::RequestModel(entity->m_nModelIndex, 0);
            }
            break;
        case RENDERER_VISIBLE:
            AddEntityToRenderList(entity, fDistance + 0.01f);
            entity->m_bOffscreen = false;
            break;
        default: // Como el original: cualquier otro valor se ignora
            break;
        }
    }
}

// 0x554EB0
bool CRenderer::ShouldModelBeStreamed(CEntity* entity, const CVector& point, float farClip) {
    if (!entity->IsInCurrentArea())
        return false;

    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfo(entity->m_nModelIndex);
    CTimeInfo* timeInfo = modelInfo->GetTimeInfo();
    if (timeInfo && !CClock::GetIsTimeInRange(timeInfo->GetTimeOn(), timeInfo->GetTimeOff()))
        return false;

    const float fMagnitude = DistanceZYX(entity->GetPosition(), point);
    // Como el original: un radio NaN no descarta (solo sale si la distancia es estrictamente mayor)
    if (fMagnitude > modelInfo->GetColModel()->GetBoundRadius() + farClip)
        return false;

    return TheCamera.m_fLODDistMultiplier * modelInfo->m_fDrawDistance > fMagnitude;
}

// 0x555680
template<typename PtrListType>
void CRenderer::ScanPtrList_RequestModels(PtrListType& list) {
    for (auto* const entity : list) {
        if (!entity->IsScanCodeCurrent()) {
            entity->SetCurrentScanCode() ;
            if (ShouldModelBeStreamed(entity, ms_vecCameraPosition, ms_fFarClipPlane))
                CStreaming::RequestModel(entity->m_nModelIndex, gnRendererModelRequestFlags);
        }
    }
}

// 0x5556E0
void CRenderer::ConstructRenderList() {
    ZoneScoped;

    const auto& camPos = TheCamera.GetPosition();

    const int32 zoneAttributes = CCullZones::FindTunnelAttributesForCoors(camPos);
    ms_bRenderTunnels = (zoneAttributes & (eZoneAttributes::TUNNEL | eZoneAttributes::TUNNEL_TRANSITION)) != 0;
    if ((zoneAttributes & eZoneAttributes::TUNNEL_TRANSITION) || !(zoneAttributes & eZoneAttributes::TUNNEL))
        ms_bRenderOutsideTunnels = true;
    else
        ms_bRenderOutsideTunnels = false;

    ms_lowLodDistScale = 1.0f;
    ms_bInTheSky = false;

    CPlayerPed* player = FindPlayerPed();
    if (player && player->GetAreaCode() == AREA_CODE_NORMAL_WORLD) {
        // Como el original: las dos alturas del suelo se calculan siempre, antes de comparar
        const float fGroundHeightBottomZ = TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_BOTTOM);
        const float fGroundHeightTopZ = TheCamera.CalculateGroundHeight(eGroundHeightType::ENTITY_BB_TOP);
        const float fPlayerHeightZ = player->GetPosition().z;

        if (fPlayerHeightZ - fGroundHeightBottomZ > 50.0f) {
            if (fPlayerHeightZ - fGroundHeightTopZ > 10.0f && FindPlayerVehicle()) {
                ms_bInTheSky = true;
            }
        }

        if (camPos.z > LOWLOD_CAMERA_HEIGHT_THRESHOLD) {
            float fScale = (camPos.z - LOWLOD_CAMERA_HEIGHT_THRESHOLD) / (250.0f - LOWLOD_CAMERA_HEIGHT_THRESHOLD);
            if (fScale > 1.0f) // Como el original: NaN se queda
                fScale = 1.0f;
            ms_lowLodDistScale = fScale * (2.2f - 1.0f) + 1.0f;
        }
    }

    ms_lowLodDistScale *= CTimeCycle::m_CurrentColours.m_fLodDistMult;
    CMirrors::BeforeConstructRenderList();
    COcclusion::ProcessBeforeRendering();
    ms_nNoOfVisibleEntities = 0;
    ms_nNoOfVisibleLods = 0;
    ms_nNoOfInVisibleEntities = 0;
    ms_vecCameraPosition = camPos;
    ms_fCameraHeading = TheCamera.GetHeading();
    ms_fFarClipPlane = TheCamera.m_pRwCamera->farPlane;
    ResetLodRenderLists();
    ScanWorld();
    ProcessLodRenderLists();
    CStreaming::StartRenderEntities();
}

// 0x555900
void CRenderer::ScanSectorList_RequestModels(int32 sectorX, int32 sectorY) {
    if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y) {
        auto& sector = CWorld::GetSector(sectorX, sectorY);
        ScanPtrList_RequestModels(sector.Buildings);
        ScanPtrList_RequestModels(sector.Dummies);
        ScanPtrList_RequestModels(CWorld::GetRepeatSector(sectorX, sectorY).Objects);
    }
}

// 0x554FE0
void CRenderer::ScanWorld() {
    // Como el original: marca como construidas las dos variables estáticas locales (guarda 0xB76894, bits 0 y 1)
    StaticRef<uint32>(0xB76894) |= 3u;

    const float& farPlane = TheCamera.m_pRwCamera->farPlane;
    const float& width  = TheCamera.m_pRwCamera->viewWindow.x;
    const float& height = TheCamera.m_pRwCamera->viewWindow.y;

    CVector frustumPoints[13]{};
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);

    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;

    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;

    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);

    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;

    // Clear the rest of the array
    for (auto i = 5u; i < std::size(frustumPoints); i++) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    RwMatrix* const cameraMatrix = TheCamera.GetRwMatrix();
    m_pFirstPersonVehicle = nullptr;
    CVisibilityPlugins::InitAlphaEntityList();

    CWorld::AdvanceCurrentScanCode();

    static auto& lastCameraPosition = StaticRef<CVector>(0xB76888);
    static auto& lastCameraForward = StaticRef<CVector>(0xB7687C);
    static auto& bCameraHasNotMoved = StaticRef<bool>(0xB745C8); // sin uso en el EXE

    // Como el original: la cámara "no se ha movido" si |pos - última|² < 16 (sumando z, x, y) y
    // forward · último forward > 0,98 (sumando z, x, y)
    const CVector& cameraPos = TheCamera.GetPosition();
    const float dx = cameraPos.x - lastCameraPosition.x;
    const float dy = cameraPos.y - lastCameraPosition.y;
    const float dz = cameraPos.z - lastCameraPosition.z;
    const CVector& forward = TheCamera.m_mCameraMatrix.GetForward();
    bCameraHasNotMoved = dz * dz + dx * dx + dy * dy < 16.0f
        && forward.z * lastCameraForward.z + forward.x * lastCameraForward.x + forward.y * lastCameraForward.y > 0.98f;

    lastCameraPosition = TheCamera.GetPosition();
    lastCameraForward = TheCamera.m_mCameraMatrix.GetForward();

    // Como el original: (p * 300) * (1 / farPlane), y los puntos cercanos con * 0,2 (no / 5)
    const float invFarPlane = 1.0f / farPlane;
    const auto ScalePoint = [&](int32 dst, int32 src, int32 nearDst) {
        frustumPoints[dst].x = frustumPoints[src].x * MAX_LOD_DISTANCE * invFarPlane;
        frustumPoints[dst].y = frustumPoints[src].y * MAX_LOD_DISTANCE * invFarPlane;
        frustumPoints[dst].z = frustumPoints[src].z * MAX_LOD_DISTANCE * invFarPlane;
        frustumPoints[nearDst].x = frustumPoints[dst].x * 0.2f;
        frustumPoints[nearDst].y = frustumPoints[dst].y * 0.2f;
        frustumPoints[nearDst].z = frustumPoints[dst].z;
    };
    ScalePoint(5, 1, 7);
    ScalePoint(6, 2, 8);
    ScalePoint(9, 3, 11);
    ScalePoint(10, 4, 12);
    RwV3dTransformPoints(frustumPoints, frustumPoints, 13, cameraMatrix);
    m_loadingPriority = 0;

    CVector2D points[5] = {
        { SectorfX(frustumPoints[0].x),  SectorfX(frustumPoints[0].y)  },
        { SectorfX(frustumPoints[5].x),  SectorfX(frustumPoints[5].y)  },
        { SectorfX(frustumPoints[6].x),  SectorfX(frustumPoints[6].y)  },
        { SectorfX(frustumPoints[9].x),  SectorfX(frustumPoints[9].y)  },
        { SectorfX(frustumPoints[10].x), SectorfX(frustumPoints[10].y) },
    };
    CWorldScan::ScanWorld(points, (int)std::size(points), ScanSectorList);

    for (auto i = 0u; i < std::size(points); i++) {
        points[i].x = LodSectorfX(frustumPoints[i].x);
        points[i].y = LodSectorfX(frustumPoints[i].y);
    }

    CWorldScan::ScanWorld(points, (int)std::size(points), ScanBigBuildingList);
}

// returns objects count
// 0x554C60
int32 CRenderer::GetObjectsInFrustum(CEntity** outEntities, float farPlane, RwMatrix* transformMatrix)
{
    CVector frustumPoints[13]{};
    const float& width = TheCamera.m_pRwCamera->viewWindow.x;
    const float& height = TheCamera.m_pRwCamera->viewWindow.y;
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;
    for (auto i = 5u; i < std::size(frustumPoints); i++) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    CWorld::AdvanceCurrentScanCode();

    RwMatrix* theTransformMatrix = transformMatrix;
    if (!theTransformMatrix)
        theTransformMatrix = TheCamera.GetRwMatrix();
    ms_vecCameraPosition = TheCamera.GetPosition();
    ms_fFarClipPlane = MAX_LOD_DISTANCE;
    if (theTransformMatrix->at.z > 0.0f) {
        frustumPoints[1] = frustumPoints[4];
        frustumPoints[2] = frustumPoints[3];
    }
    RwV3dTransformPoints(frustumPoints, frustumPoints, 13, theTransformMatrix);
    gpOutEntitiesForGetObjectsInFrustum = outEntities;
    CVector2D points[3];
    for (auto i = 0u; i < std::size(points); i++) {
        points[i].x = SectorfX(frustumPoints[i].x);
        points[i].y = SectorfX(frustumPoints[i].y);
    }
    if (transformMatrix)
        CWorldScan::ScanWorld(points, (int)std::size(points), ScanSectorList_ListModels);
    else
        CWorldScan::ScanWorld(points, (int)std::size(points), ScanSectorList_ListModelsVisible);
    return gpOutEntitiesForGetObjectsInFrustum - outEntities;
}

// 0x555960
void CRenderer::RequestObjectsInFrustum(RwMatrix* transformMatrix, int32 modelRequestFlags) {
    const float farPlane = TheCamera.m_pRwCamera->farPlane;
    const float& width  = TheCamera.m_pRwCamera->viewWindow.x;
    const float& height = TheCamera.m_pRwCamera->viewWindow.y;

    CVector frustumPoints[13]{};
    frustumPoints[0] = CVector(0.0f, 0.0f, 0.0f);
    frustumPoints[1].x = frustumPoints[4].x = -(farPlane * width);
    frustumPoints[1].y = frustumPoints[2].y = farPlane * height;
    frustumPoints[2].x = frustumPoints[3].x = farPlane * width;
    frustumPoints[3].y = frustumPoints[4].y = -(farPlane * height);
    frustumPoints[1].z = frustumPoints[2].z = frustumPoints[3].z = frustumPoints[4].z = farPlane;
    for (auto i = 5u; i < std::size(frustumPoints); i++) {
        frustumPoints[i] = CVector(0.0f, 0.0f, 0.0f);
    }

    CWorld::AdvanceCurrentScanCode();

    if (!transformMatrix) {
        transformMatrix = TheCamera.GetRwMatrix();
    }
    ms_vecCameraPosition = transformMatrix->pos;
    ms_fFarClipPlane = MAX_LOD_DISTANCE;
    gnRendererModelRequestFlags = modelRequestFlags;
    // Como el original: (p * 300) * (1 / farPlane); con at.z NaN también se usan los puntos de abajo (4 y 3)
    const float invFarPlane = 1.0f / farPlane;
    const auto ScalePoint = [&](int32 dst, int32 src) {
        frustumPoints[dst].x = frustumPoints[src].x * MAX_LOD_DISTANCE * invFarPlane;
        frustumPoints[dst].y = frustumPoints[src].y * MAX_LOD_DISTANCE * invFarPlane;
        frustumPoints[dst].z = frustumPoints[src].z * MAX_LOD_DISTANCE * invFarPlane;
    };
    if (!(transformMatrix->at.z <= 0.0f)) {
        ScalePoint(5, 4);
        ScalePoint(6, 3);
    }
    else {
        ScalePoint(5, 1);
        ScalePoint(6, 2);
    }
    RwV3dTransformPoints(frustumPoints, frustumPoints, (int)std::size(frustumPoints), transformMatrix);
    CVector2D points[3] = {
        { SectorfX(frustumPoints[0].x), SectorfX(frustumPoints[0].y) },
        { SectorfX(frustumPoints[5].x), SectorfX(frustumPoints[5].y) },
        { SectorfX(frustumPoints[6].x), SectorfX(frustumPoints[6].y) },
    };
    CWorldScan::ScanWorld(points, std::size(points), ScanSectorList_RequestModels);
}

// 0x555CB0
void CRenderer::RequestObjectsInDirection(const CVector& posn, float angle, int32 modelRequestFlags)
{
    // NOTA: el EXE hace OR 0x20003 sobre los flags sin inicializar de la matriz local
    RwMatrix matrix {
        .right = { 1.0f, 0.0f, 0.0f },
        .flags = rwMATRIXINTERNALIDENTITY | rwMATRIXTYPEORTHONORMAL,
        .up =    { 0.0f, 1.0f, 0.0f },
        .at =    { 0.0f, 0.0f, 1.0f },
        .pos =   { 0.0f, 0.0f, 0.0f },
    };
    RwV3d axis = { 1.0f, 0.0f, 0.0f };
    RwMatrixRotate(&matrix, &axis, 90.0f, rwCOMBINEREPLACE);
    angle = angle * std::bit_cast<float>(0x42652EE0u) + 180.0f; // Como el original: * 57,295776 (flt_859878) + 180
    axis = { 0.0f, 0.0f, 1.0f };
    RwMatrixRotate(&matrix, &axis, angle, rwCOMBINEPOSTCONCAT);
    RwMatrixTranslate(&matrix, &posn, rwCOMBINEPOSTCONCAT);
    // Como el original: siempre pide con STREAMING_LOADING_SCENE (0x20), ignora `modelRequestFlags`
    (void)modelRequestFlags;
    RequestObjectsInFrustum(&matrix, STREAMING_LOADING_SCENE);
}

// 0x553540
void CRenderer::SetupScanLists(int32 sectorX, int32 sectorY) {
    auto& repeatSector = RepeatSectorAnd15(sectorX, sectorY); // Como el original: & 15 (no abs % 16)
    auto* scanLists    = reinterpret_cast<tScanLists*>(&PC_Scratch);
    if (sectorX >= 0 && sectorY >= 0 && sectorX < MAX_SECTORS_X && sectorY < MAX_SECTORS_Y) {
        auto& sector             = CWorld::GetSector(sectorX, sectorY);
        scanLists->buildingsList = &sector.Buildings;
        scanLists->objectsList   = &repeatSector.Objects;
        scanLists->vehiclesList  = &repeatSector.Vehicles;
        scanLists->pedsList      = &repeatSector.Peds;
        scanLists->dummiesList   = &sector.Dummies;
    } else {
        // sector x and y are out of bounds
        scanLists->buildingsList = nullptr;
        scanLists->objectsList   = &repeatSector.Objects;
        scanLists->vehiclesList  = &repeatSector.Vehicles;
        scanLists->pedsList      = &repeatSector.Peds;
        scanLists->dummiesList   = nullptr;
    }
}
