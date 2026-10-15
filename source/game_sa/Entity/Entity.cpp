/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntity
 * Reconstrucción: COMPLETA — 70/70 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 70/70 entradas.
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
#include "X87Math.h"

#include "Entity.h"

#include "PointLights.h"
#include "Escalators.h"
#include "CustomBuildingDNPipeline.h"
#include "ActiveOccluder.h"
#include "Occlusion.h"
#include "MotionBlurStreaks.h"
#include "TagManager.h"
#include "WindModifiers.h"
#include "EntryExitManager.h"
#include "TrafficLights.h"
#include "Glass.h"
#include "TheScripts.h"
#include "Shadows.h"
#include "CustomBuildingRenderer.h"

/**
 * Registra el ciclo de vida, streaming, referencias, render y efectos comunes de CEntity.
 */
void CEntity::InjectHooks() {
    RH_ScopedVirtualClass(CEntity, 0x863928, 22);
    RH_ScopedCategory("Entity");

    RH_ScopedInstall(Constructor, 0x532A90);
    RH_ScopedInstall(Destructor, 0x535E90);

    //RH_ScopedOverloadedInstall(Add, "void", 0x533020, void(CEntity::*)());
    RH_ScopedVMTOverloadedInstall(Add, "rect", 0x5347D0, void(CEntity::*)(const CRect&)); // Como el original: virtual (hueco 1); un hook plano entra en bucle
    RH_ScopedVMTInstall(Remove, 0x534AE0);
    RH_ScopedVMTInstall(SetIsStatic, 0x403E20);
    RH_ScopedVMTInstall(SetModelIndex, 0x532AE0);
    RH_ScopedVMTInstall(SetModelIndexNoCreate, 0x533700);
    RH_ScopedVMTInstall(CreateRwObject, 0x533D30);
    RH_ScopedVMTInstall(DeleteRwObject, 0x534030);
    RH_ScopedVMTInstall(GetBoundRect, 0x534120);
    RH_ScopedVMTInstall(ProcessControl, 0x403E40);
    RH_ScopedVMTInstall(ProcessCollision, 0x403E50);
    RH_ScopedVMTInstall(ProcessShift, 0x403E60);
    RH_ScopedVMTInstall(TestCollision, 0x403E70);
    RH_ScopedVMTInstall(Teleport, 0x403E80);
    RH_ScopedVMTInstall(SpecialEntityPreCollisionStuff, 0x403E90);
    RH_ScopedVMTInstall(SpecialEntityCalcCollisionSteps, 0x403EA0);
    RH_ScopedVMTInstall(PreRender, 0x535FA0);
    RH_ScopedVMTInstall(Render, 0x534310);
    RH_ScopedVMTInstall(SetupLighting, 0x553DC0);
    RH_ScopedVMTInstall(RemoveLighting, 0x553370);
    RH_ScopedVMTInstall(FlagToDestroyWhenNextProcessed, 0x403EB0);
    RH_ScopedInstall(UpdateRwFrame, 0x532B00);
    RH_ScopedInstall(UpdateRpHAnim, 0x532B20);
    RH_ScopedInstall(HasPreRenderEffects, 0x532B70);
    RH_ScopedInstall(DoesNotCollideWithFlyers, 0x532D40);
    RH_ScopedInstall(BuildWindSockMatrix, 0x532DB0);
    RH_ScopedInstall(LivesInThisNonOverlapSector, 0x533050);
    RH_ScopedInstall(SetupBigBuilding, 0x533150);
    RH_ScopedInstall(ModifyMatrixForCrane, 0x533170);
    RH_ScopedInstall(PreRenderForGlassWindow, 0x533240);
    RH_ScopedInstall(SetRwObjectAlpha, 0x5332C0);
    RH_ScopedInstall(FindTriggerPointCoors, 0x533380);
    RH_ScopedInstall(GetRandom2dEffect, 0x533410);
    RH_ScopedOverloadedInstall(TransformFromObjectSpace, "ref", 0x5334F0, CVector(CEntity::*)(const CVector&) const);
    RH_ScopedOverloadedInstall(TransformFromObjectSpace, "ptr", 0x533560, CVector*(CEntity::*)(CVector&, const CVector&) const);
    RH_ScopedInstall(CreateEffects, 0x533790);
    RH_ScopedInstall(DestroyEffects, 0x533BF0);
    RH_ScopedInstall(AttachToRwObject, 0x533ED0);
    RH_ScopedInstall(DetachFromRwObject, 0x533FB0);
    RH_ScopedOverloadedInstall(GetBoundCentre, "vec", 0x534250, CVector(CEntity::*)() const);
    RH_ScopedOverloadedInstall(GetBoundCentre, "ref", 0x534290, void(CEntity::*)(CVector&) const);
    RH_ScopedInstall(RenderEffects, 0x5342B0);
    RH_ScopedOverloadedInstall(GetIsTouching, "ent", 0x5343F0, bool(CEntity::*)(CEntity*) const);
    RH_ScopedOverloadedInstall(GetIsTouching, "vec", 0x5344B0, bool(CEntity::*)(const CVector&, float) const);
    RH_ScopedInstall(GetIsOnScreen, 0x534540);
    RH_ScopedInstall(GetIsBoundingBoxOnScreen, 0x5345D0);
    RH_ScopedInstall(ModifyMatrixForTreeInWind, 0x534E90);
    RH_ScopedInstall(ModifyMatrixForBannerInWind, 0x535040);
    RH_ScopedInstall(GetColModel, 0x535300);
    RH_ScopedInstall(CalculateBBProjection, 0x535340);
    RH_ScopedInstall(UpdateAnim, 0x535F00);
    RH_ScopedInstall(IsVisible, 0x536BC0);
    RH_ScopedInstall(GetDistanceFromCentreOfMassToBaseOfModel, 0x536BE0);
    RH_ScopedOverloadedInstall(CleanUpOldReference, "", 0x571A00, void(CEntity::*)(CEntity**));
    RH_ScopedInstall(ResolveReferences, 0x571A40);
    RH_ScopedInstall(PruneReferences, 0x571A90);
    RH_ScopedOverloadedInstall(RegisterReference, "", 0x571B70, void(CEntity::*)(CEntity**));
    RH_ScopedInstall(ProcessLightsForEntity, 0x6FC7A0);
    RH_ScopedInstall(RemoveEscalatorsForEntity, 0x717900);
    RH_ScopedInstall(IsEntityOccluded, 0x71FAE0);
    RH_ScopedInstall(GetModellingMatrix, 0x46A2D0);
    RH_ScopedInstall(UpdateRwMatrix, 0x446F90);

    RH_ScopedInstall(GetIsTypePhysical, 0x4DA030);
    RH_ScopedInstall(GetIsStatic, 0x4633E0);
    RH_ScopedInstall(TreatAsPlayerForCollisions, 0x541F70);

    RH_ScopedGlobalInstall(SetAtomicAlpha, 0x533290);
    RH_ScopedGlobalInstall(SetCompAlphaCB, 0x533280);
    RH_ScopedGlobalInstall(MaterialUpdateUVAnimCB, 0x532D70);
    RH_ScopedGlobalInstall(IsEntityPointerValid, 0x533310);
}

// 0x532A90
/**
 * Inicializa una entidad sin modelo ni RwObject, con estado/area/LOD en sus valores base.
 */
CEntity::CEntity() : CPlaceable() {
    SetStatus(STATUS_ABANDONED);
    SetType(ENTITY_TYPE_NOTHING);

    m_nFlags = 0;
    SetIsVisible(true);
    SetIsBackfaceCulled(true);

    SetScanCode(0);
    SetAreaCode(AREA_CODE_NORMAL_WORLD);
    m_nModelIndex = MODEL_INVALID;
    m_pRwObject = nullptr;
    SetIplIndex(0);
    m_nRandomSeed = CGeneral::GetRandomNumber();
    m_pReferences = nullptr;
    m_pStreamingLink = nullptr;
    m_NumLodChildren = 0;
    ResetLodChildrenRendered();
    SetLod(nullptr);
}

// 0x535E90
/**
 * Desvincula la relacion LOD, destruye el recurso RenderWare y resuelve todas las
 * referencias registradas para que ningun puntero externo conserve this.
 */
CEntity::~CEntity() {
    if (GetLod()) {
        GetLod()->RemoveLodChildren();
    }

    CEntity::DeleteRwObject();
    CEntity::ResolveReferences();
}

// 0x533020
// Inserta la entidad en los sectores del mundo cubiertos por su bounding rectangle.
void CEntity::Add() {
    Add(GetBoundRect());
}

namespace {
// Como el original (0x5347D0/0x534AE0): sector = floor(v * 0.02 + 60) con el producto y la suma redondeados a float (x87 a 24 bits)
// y _ftol2 (64 bits) sobre el resultado; el sector LOD es trunc(v * 0.005 + 15) sin floor.
int32 EntityA_SectorCoord(float v) {
    const float scaled = v * 0.02F;
    const float sector = scaled + 60.0F;
    return static_cast<int32>(static_cast<int64>(std::floor(sector)));
}
int32 EntityA_LodSectorCoord(float v) {
    const float scaled = v * 0.005F;
    const float sector = scaled + 15.0F;
    return static_cast<int32>(static_cast<int64>(sector));
}
// Como el original: recorte del rectangulo con las comparaciones del x87 (NaN no recorta).
// Los campos en memoria son left(+0), top(+4), right(+8), bottom(+0xC): el EXE recorta +4 con '> 3000' y +0xC con '< -3000'.
void EntityA_ClampBoundRect(CRect& r) {
    if (r.left < -3000.0F) {
        r.left = -3000.0F;
    }
    if (r.right >= 3000.0F) {
        r.right = 2999.0F;
    }
    if (r.bottom < -3000.0F) {
        r.bottom = -3000.0F;
    }
    if (r.top > 3000.0F) {
        r.top = 2999.0F;
    }
}
CPtrListSingleLink<CEntity*>& EntityA_LodList(int32 x, int32 y) { // sin comprobar rangos, como el original (indice plano y * 30 + x)
    return *(reinterpret_cast<CPtrListSingleLink<CEntity*>*>(&CWorld::ms_aLodPtrLists) + (y * 30 + x));
}
CRepeatSector& EntityA_RepeatSector(int32 x, int32 y) { // Como el original: (y & 15) * 16 + (x & 15)
    return CWorld::ms_aRepeatSectors[static_cast<uint32>(y) & 0xF][static_cast<uint32>(x) & 0xF];
}
// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE (x: z, y, x; y: z, x, y; z: z, x, y)
CVector EntityA_MultiplyPoint59C890(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}
// Como el original: fsin a precision completa y el producto redondeado a 24 bits por el x87
float EntityA_SinTimes(float angle, float factor) {
    return TrigSimpleSinMulCpp(angle, factor);
}
// Como el original: (v < 0) ? -v : v (con -0.0 y NaN sin cambios, a diferencia de fabs)
float EntityA_AbsLikeExe(float v) {
    return v < 0.0F ? -v : v;
}
}

// 0x5347D0
/**
 * Inserta la entidad en sectores LOD o normales segun m_bIsBIGBuilding.
 *
 * La lista concreta depende de eEntityType; el rectangulo se recorta primero a los
 * limites validos del mapa.
 */
void CEntity::Add(const CRect& rect) {
    CRect usedRect = rect;
    EntityA_ClampBoundRect(usedRect);

    if (m_bIsBIGBuilding) {
        // Como el original: los LOD se calculan con trunc(v * 0.005 + 15) (sin floor) y sin recorte de indices
        const auto startX = EntityA_LodSectorCoord(usedRect.left);
        const auto endX   = EntityA_LodSectorCoord(usedRect.right);
        const auto endY   = EntityA_LodSectorCoord(usedRect.top);
        const auto startY = EntityA_LodSectorCoord(usedRect.bottom);
        for (auto y = startY; y <= endY; ++y) {
            for (auto x = startX; x <= endX; ++x) {
                EntityA_LodList(x, y).AddItem(this);
            }
        }
    } else {
        const auto startX = EntityA_SectorCoord(usedRect.left);
        const auto startY = EntityA_SectorCoord(usedRect.bottom);
        const auto endX   = EntityA_SectorCoord(usedRect.right);
        const auto endY   = EntityA_SectorCoord(usedRect.top);
        for (auto y = startY; y <= endY; ++y) {
            for (auto x = startX; x <= endX; ++x) {
                switch (GetType()) {
                case ENTITY_TYPE_DUMMY:    CWorld::GetSector(x, y).Dummies.AddItem(static_cast<CDummy*>(this)); break;
                case ENTITY_TYPE_VEHICLE:  EntityA_RepeatSector(x, y).Vehicles.AddItem(static_cast<CVehicle*>(this)); break;
                case ENTITY_TYPE_PED:      EntityA_RepeatSector(x, y).Peds.AddItem(static_cast<CPed*>(this)); break;
                case ENTITY_TYPE_OBJECT:   EntityA_RepeatSector(x, y).Objects.AddItem(static_cast<CObject*>(this)); break;
                case ENTITY_TYPE_BUILDING: CWorld::GetSector(x, y).Buildings.AddItem(static_cast<CBuilding*>(this)); break;
                default: break;
                }
            }
        }
    }
}

// 0x534AE0
// Ejecuta el proceso inverso de Add(), retirando la entidad de todas las listas espaciales.
void CEntity::Remove() {
    auto usedRect = GetBoundRect();
    EntityA_ClampBoundRect(usedRect);

    if (m_bIsBIGBuilding) {
        const auto startX = EntityA_LodSectorCoord(usedRect.left);
        const auto endX   = EntityA_LodSectorCoord(usedRect.right);
        const auto endY   = EntityA_LodSectorCoord(usedRect.top);
        const auto startY = EntityA_LodSectorCoord(usedRect.bottom);
        for (auto y = startY; y <= endY; ++y) {
            for (auto x = startX; x <= endX; ++x) {
                EntityA_LodList(x, y).DeleteItem(this);
            }
        }
    } else {
        const auto startX = EntityA_SectorCoord(usedRect.left);
        const auto startY = EntityA_SectorCoord(usedRect.bottom);
        const auto endX   = EntityA_SectorCoord(usedRect.right);
        const auto endY   = EntityA_SectorCoord(usedRect.top);
        for (auto y = startY; y <= endY; ++y) {
            for (auto x = startX; x <= endX; ++x) {
                switch (GetType()) {
                case ENTITY_TYPE_DUMMY:    CWorld::GetSector(x, y).Dummies.DeleteItem(static_cast<CDummy*>(this)); break;
                case ENTITY_TYPE_VEHICLE:  EntityA_RepeatSector(x, y).Vehicles.DeleteItem(static_cast<CVehicle*>(this)); break;
                case ENTITY_TYPE_PED:      EntityA_RepeatSector(x, y).Peds.DeleteItem(static_cast<CPed*>(this)); break;
                case ENTITY_TYPE_OBJECT:   EntityA_RepeatSector(x, y).Objects.DeleteItem(static_cast<CObject*>(this)); break;
                case ENTITY_TYPE_BUILDING: CWorld::GetSector(x, y).Buildings.DeleteItem(static_cast<CBuilding*>(this)); break;
                default: break;
                }
            }
        }
    }
}

// 0x532AE0
// Cambia model info y materializa inmediatamente el recurso RenderWare correspondiente.
void CEntity::SetModelIndex(uint32 index) {
    SetModelIndexNoCreate(index);
    CreateRwObject();
}

// 0x533700
/**
 * Cambia solo el model index y sincroniza flags derivados del CBaseModelInfo.
 * No crea todavia el RpAtomic/RpClump.
 */
void CEntity::SetModelIndexNoCreate(uint32 index) {
    auto mi = CModelInfo::GetModelInfo(index);
    m_nModelIndex = index;

    m_bHasPreRenderEffects = HasPreRenderEffects();

    if (mi->GetIsDrawLast()) {
        m_bDrawLast = true;
    }

    if (!mi->IsBackfaceCulled()) {
        SetIsBackfaceCulled(false);
    }

    auto ami = mi->AsAtomicModelInfoPtr();
    if (ami && !ami->bTagDisabled && ami->IsTagModel()) {
        CTagManager::AddTag(*this);
    }
}

// Creates the RenderWare object (RpAtomic or RpClump) for the entity
// This function handles model instantiation, streaming, and effect creation
// 0x533D30
void CEntity::CreateRwObject() {
    if (!GetIsVisible()) {
        return;
    }

    auto* mi = GetModelInfo();

    // Create instance based on damage state
    if (m_bRenderDamaged) {
        CDamageAtomicModelInfo::ms_bCreateDamagedVersion = true;
        m_pRwObject = mi->CreateInstance();
        CDamageAtomicModelInfo::ms_bCreateDamagedVersion = false;
    } else {
        m_pRwObject = mi->CreateInstance();
    }

    if (!GetRwObject()) {
        return;
    }

    // Update building counter and RenderWare state
    if (GetIsTypeBuilding()) {
        gBuildings++;
    }
    UpdateRwMatrix();

    // Handle different RenderWare object types
    switch (RwObjectGetType(GetRwObject())) {
    case rpATOMIC:
        if (CTagManager::IsTag(*this)) {
            CTagManager::ResetAlpha(*this);
        }
        CCustomBuildingDNPipeline::PreRenderUpdate(GetRpAtomic(), true);
        break;
    case rpCLUMP:
        if (mi->bIsRoad) {
            // Add to moving list for road objects
            if (GetIsTypeObject()) {
                auto* obj = AsObject();
                if (!obj->m_pMovingList) {
                    obj->AddToMovingList();
                }
                obj->SetIsStatic(false);
            } else {
                CWorld::ms_listMovingEntityPtrs.AddItem(AsPhysical());
            }

            // Synchronize LOD animation timing
            if (GetLod() && GetLod()->GetRwObject() && RwObjectGetType(GetLod()->GetRwObject()) == rpCLUMP) {
                if (auto* pLodAssoc = RpAnimBlendClumpGetFirstAssociation(GetLod()->GetRpClump())) {
                    if (auto* pAssoc = RpAnimBlendClumpGetFirstAssociation(GetRpClump())) {
                        pAssoc->SetCurrentTime(pLodAssoc->m_CurrentTime);
                    }
                }
            }
        }
        break;
    }

    // Finalize object setup
    mi->AddRef();
    m_pStreamingLink = CStreaming::AddEntity(this);
    CreateEffects();

    // Determine lighting requirement
    auto* usedAtomic = RwObjectGetType(GetRwObject()) == rpATOMIC ? GetRpAtomic() : GetFirstAtomic(GetRpClump());

    if (!CCustomBuildingRenderer::IsCBPCPipelineAttached(usedAtomic)) {
        m_bLightObject = true;
    }
}

// Destroys the entity's RenderWare object, removing it from the world
// It also cleans up streaming links, model references, and associated effects
// 0x534030
void CEntity::DeleteRwObject() {
    if (!GetRwObject()) {
        return;
    }

    switch (RwObjectGetType(GetRwObject())) {
    case rpATOMIC: {
        // Como el original (0x534046..0x534055): lee el frame, destruye el atomic y destruye el frame sin comprobar que sea nulo
        auto* const frame = RpAtomicGetFrame(GetRpAtomic());
        RpAtomicDestroy(GetRpAtomic());
        RwFrameDestroy(frame);
        break;
    }
    case rpCLUMP:
#ifdef SA_SKINNED_PEDS
        if (IsClumpSkinned(GetRpClump())) {
            RpClumpForAllAtomics(GetRpClump(), AtomicRemoveAnimFromSkinCB, nullptr);
        }
#endif
        RpClumpDestroy(GetRpClump());
        break;
    }

    m_pRwObject = nullptr;
    auto* mi = GetModelInfo();
    mi->RemoveRef();
    // Como el original (0x5340B1..0x5340C4): el enlace se pasa a RemoveEntity y solo despues se pone a nulo
    CStreaming::RemoveEntity(m_pStreamingLink);
    m_pStreamingLink = nullptr;

    if (GetIsTypeBuilding()) {
        gBuildings--;
    }

    if (mi->GetModelType() == MODEL_INFO_CLUMP && mi->IsRoad() && !GetIsTypeObject()) {
        CWorld::ms_listMovingEntityPtrs.DeleteItem(AsPhysical());
    }

    CEntity::DestroyEffects();
    CEntity::RemoveEscalatorsForEntity();
}

// Calculates the 2D bounding rectangle of the entity in world space
// by transforming the corners of its collision model's bounding box
// 0x534120
CRect CEntity::GetBoundRect() const {
    auto* colModel = GetModelInfo()->GetColModel();
    auto [boundBoxMin, boundBoxMax] = std::pair{ colModel->GetBoundingBox().m_vecMin, colModel->GetBoundingBox().m_vecMax };

    CRect rect;
    CVector result;

    // Processing the first two corners
    TransformFromObjectSpace(result, boundBoxMin);
    rect.StretchToPoint(result.x, result.y);

    TransformFromObjectSpace(result, boundBoxMax);
    rect.StretchToPoint(result.x, result.y);

    // Exchange of X coordinates and processing of remaining angles
    std::ranges::swap(boundBoxMin.x, boundBoxMax.x);

    TransformFromObjectSpace(result, boundBoxMin);
    rect.StretchToPoint(result.x, result.y);

    TransformFromObjectSpace(result, boundBoxMax);
    rect.StretchToPoint(result.x, result.y);

    return rect;
}

// Performs pre-rendering tasks for the entity, such as processing lights,
// updating model animations, and applying special effects like wind on trees or pickup effects
// 0x535FA0
void CEntity::PreRender() {
    const auto mi = GetModelInfo();
    const auto ami = mi->AsAtomicModelInfoPtr();

    if (mi->m_n2dfxCount) {
        ProcessLightsForEntity();
    }

    if (!mi->HasBeenPreRendered()) {
        mi->SetHasBeenPreRendered(true);

        if (ami && ami->GetRpAtomic()) {
            if (RpMatFXAtomicQueryEffects(ami->GetRpAtomic()) && RpAtomicGetGeometry(ami->GetRpAtomic())) {
                RpGeometryForAllMaterials(RpAtomicGetGeometry(ami->GetRpAtomic()), MaterialUpdateUVAnimCB, nullptr);
            }
        }

        mi->IncreaseAlpha();

        // PC Only
        if (ami) {
            CCustomBuildingDNPipeline::PreRenderUpdate(ami->GetRpAtomic(), false);
        } else if (mi->GetModelType() == MODEL_INFO_CLUMP) {
            CCustomBuildingDNPipeline::PreRenderUpdate(static_cast<CClumpModelInfo*>(mi)->GetRpClump(), false);
        }
        // PC Only
    }

    if (!m_bHasPreRenderEffects) {
        return;
    }

    if (ami && ami->SwaysInWind() && (!GetIsTypeObject() || !AsObject()->objectFlags.bIsExploded)) {
        const auto fDist = DistanceBetweenPoints2D(GetPosition(), TheCamera.GetPosition());
        // Como el original (0x536130): fcomp + test ah,5 -> se guarda siempre que NO sea (distancia al arbol < fDist)
        if (!(CObject::fDistToNearestTree < fDist)) {
            CObject::fDistToNearestTree = fDist;
        }
        ModifyMatrixForTreeInWind();
    }

    if (GetIsTypeBuilding()) {
        if (ami && ami->IsCrane()) {
            ModifyMatrixForCrane();
        }
        return;
    }

    if (!GetIsTypeObject() && !GetIsTypeDummy()) {
        return;
    }

    if (GetIsTypeObject()) {
        const auto obj = AsObject();
        const auto modelIndex = GetModelIndex();
        
        if (modelIndex == ModelIndices::MI_COLLECTABLE1) {
            CPickups::DoCollectableEffects(this);
            UpdateRwMatrix();
            UpdateRwFrame();
        } else if (modelIndex == ModelIndices::MI_MONEY) {
            CPickups::DoMoneyEffects(this);
            UpdateRwMatrix();
            UpdateRwFrame();
        } else if (modelIndex == ModelIndices::MI_CARMINE
                   || modelIndex == ModelIndices::MI_NAUTICALMINE
                   || modelIndex == ModelIndices::MI_BRIEFCASE) {
            if (obj->objectFlags.bIsPickup) {
                CPickups::DoMineEffects(this);
                UpdateRwMatrix();
                UpdateRwFrame();
            }
        } else if (modelIndex == MODEL_MISSILE) {
            if (CReplay::Mode != MODE_PLAYBACK) {
                const auto& vecPos = GetPosition();
                const auto fRand = static_cast<float>(CGeneral::GetRandomNumber() % 16) / 16.0F;
                
                CShadows::StoreShadowToBeRendered(
                    eShadowTextureType::SHADOW_TEX_PED, gpShadowExplosionTex, vecPos,
                    8.0F, 0.0F, 0.0F, -8.0F, 255,
                    static_cast<uint8>(fRand * 200.0F),
                    static_cast<uint8>(fRand * 160.0F),
                    static_cast<uint8>(fRand * 120.0F),
                    20.0F, false, 1.0F, nullptr, false
                );

                CPointLights::AddLight(
                    ePointLightType::PLTYPE_POINTLIGHT, vecPos, CVector(0.0F, 0.0F, 0.0F),
                    8.0F, fRand, fRand * 0.8F, fRand * 0.6F,
                    RwFogType::rwFOGTYPENAFOGTYPE, true, nullptr
                );

                CCoronas::RegisterCorona(
                    (uint32)this, nullptr,
                    static_cast<uint8>(fRand * 255.0F),
                    static_cast<uint8>(fRand * 220.0F),
                    static_cast<uint8>(fRand * 190.0F), 255, vecPos,
                    fRand * 6.0F, 300.0F, gpCoronaTexture[CORONATYPE_SHINYSTAR],
                    eCoronaFlareType::FLARETYPE_NONE, eCoronaReflType::CORREFL_SIMPLE,
                    eCoronaLOSCheck::LOSCHECK_OFF, eCoronaTrail::TRAIL_OFF,
                    0.0F, false, 1.5F, 0, 15.0F, false, false
                );
            }
        } else if (modelIndex == ModelIndices::MI_FLARE) {
            const auto& vecPos = GetPosition();
            const auto fRand = std::max(static_cast<float>(CGeneral::GetRandomNumber() % 16) / 16.0F, 0.5F);
            
            CShadows::StoreShadowToBeRendered(
                eShadowTextureType::SHADOW_TEX_PED, gpShadowExplosionTex, vecPos,
                8.0F, 0.0F, 0.0F, -8.0F, 255,
                static_cast<uint8>(fRand * 200.0F),
                static_cast<uint8>(fRand * 200.0F),
                static_cast<uint8>(fRand * 200.0F),
                20.0F, false, 1.0F, nullptr, false
            );

            CPointLights::AddLight(
                ePointLightType::PLTYPE_POINTLIGHT, vecPos, CVector(0.0F, 0.0F, 0.0F),
                32.0F, fRand, fRand, fRand,
                RwFogType::rwFOGTYPENAFOGTYPE, true, nullptr
            );

            CCoronas::RegisterCorona(
                (uint32)this, nullptr,
                static_cast<uint8>(fRand * 255.0F),
                static_cast<uint8>(fRand * 255.0F),
                static_cast<uint8>(fRand * 255.0F), 255, vecPos,
                fRand * 6.0F, 300.0F, gpCoronaTexture[CORONATYPE_SHINYSTAR],
                eCoronaFlareType::FLARETYPE_NONE, eCoronaReflType::CORREFL_SIMPLE,
                eCoronaLOSCheck::LOSCHECK_OFF, eCoronaTrail::TRAIL_OFF,
                0.0F, false, 1.5F, 0, 15.0F, false, false
            );
        } else if (CGlass::IsObjectGlass(this)) {
            PreRenderForGlassWindow();
        } else if (obj->objectFlags.bIsPickup) {
            CPickups::DoPickUpEffects(this);
            UpdateRwMatrix();
            UpdateRwFrame();
        } else if (modelIndex == MODEL_GRENADE) {
            const auto& vecPos = GetPosition();
            const auto vecScaledCam = TheCamera.m_mCameraMatrix.GetRight() * 0.07F;
            const auto vecStreakStart = vecPos - vecScaledCam;
            const auto vecStreakEnd = vecPos + vecScaledCam;
            
            if (CVector2D(obj->m_vecMoveSpeed).Magnitude() > 0.03F) {
                CMotionBlurStreaks::RegisterStreak(
                    reinterpret_cast<uint32>(this), 100, 100, 100, 255, 
                    vecStreakStart, vecStreakEnd
                );
            }
        } else if (modelIndex == MODEL_MOLOTOV) {
            const auto& vecPos = GetPosition();
            const auto vecScaledCam = TheCamera.m_mCameraMatrix.GetRight() * 0.07F;
            const auto vecStreakStart = vecPos - vecScaledCam;
            const auto vecStreakEnd = vecPos + vecScaledCam;
            
            if (CVector2D(obj->m_vecMoveSpeed).Magnitude() > 0.03F) {
                float fWaterLevel;
                if (!CWaterLevel::GetWaterLevelNoWaves(vecPos, &fWaterLevel, nullptr, nullptr) || 
                    vecPos.z > fWaterLevel) {
                    CMotionBlurStreaks::RegisterStreak(
                        reinterpret_cast<uint32>(this), 255, 160, 100, 255, 
                        vecStreakStart, vecStreakEnd
                    );
                }
            }
        } else if (modelIndex == ModelIndices::MI_BEACHBALL) {
            if (DistanceBetweenPoints(GetPosition(), TheCamera.GetPosition()) < 50.0F) {
                // Como el original: la intensidad es la palabra de 16 bits completa y solo r/g/b usan el byte bajo
                const auto shadowStrength = CTimeCycle::m_CurrentColours.m_nShadowStrength;
                const auto ucShadowStrength = static_cast<uint8>(shadowStrength);
                CShadows::StoreShadowToBeRendered(
                    eShadowType::SHADOW_DEFAULT, gpShadowPedTex, GetPosition(),
                    0.4F, 0.0F, 0.0F, -0.4F,
                    shadowStrength, ucShadowStrength, ucShadowStrength, ucShadowStrength,
                    20.0F, false, 1.0F, nullptr, false
                );
            }
        } else if (modelIndex == ModelIndices::MI_MAGNOCRANE_HOOK
                   || modelIndex == ModelIndices::MI_WRECKING_BALL
                   || modelIndex == ModelIndices::MI_CRANE_MAGNET
                   || modelIndex == ModelIndices::MI_MINI_MAGNET
                   || modelIndex == ModelIndices::MI_CRANE_HARNESS) {
            // Como el original (0x5368DE): el limite es 110,0 (0x858BA8), no 100
            if (DistanceBetweenPoints(GetPosition(), TheCamera.GetPosition()) < 110.0F) {
                CShadows::StoreShadowToBeRendered(
                    eShadowType::SHADOW_DEFAULT, gpShadowPedTex, GetPosition(),
                    2.0F, 0.0F, 0.0F, -2.0F, 128, 128, 128, 128,
                    50.0F, false, 1.0F, nullptr, false
                );
            }
        } else if (modelIndex == ModelIndices::MI_WINDSOCK) {
            BuildWindSockMatrix();
        }
    }

    const auto modelIndex = GetModelIndex();
    constexpr auto storeShadowForPole = [](auto* entity, float x, float y, float z) {
        CShadows::StoreShadowForPole(entity, x, y, z, 16.0F, 0.4F, 0);
    };

    if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 2.957F, 0.147F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VERTICAL) {
        CTrafficLights::DisplayActualLight(this);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_MIAMI) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 4.81F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_TWOVERTICAL) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 7.503F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_3 || 
               modelIndex == ModelIndices::MI_TRAFFICLIGHTS_4 || 
               modelIndex == ModelIndices::MI_TRAFFICLIGHTS_5 || 
               modelIndex == ModelIndices::MI_TRAFFICLIGHTS_GAY) {
        CTrafficLights::DisplayActualLight(this);
    } else if (modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS1) {
        storeShadowForPole(this, 0.744F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS2) {
        storeShadowForPole(this, 0.043F, 0.0F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_SINGLESTREETLIGHTS3) {
        storeShadowForPole(this, 1.143F, 0.145F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_DOUBLESTREETLIGHTS) {
        storeShadowForPole(this, 0.0F, -0.048F, 0.0F);
    } else if (modelIndex == ModelIndices::MI_TRAFFICLIGHTS_VEGAS) {
        CTrafficLights::DisplayActualLight(this);
        storeShadowForPole(this, 7.5F, 0.2F, 0.0F);
    }
}

// Renders the entity's RenderWare object if it exists and is visible
// 0x534310
void CEntity::Render() {
    if (!GetRwObject()) {
        return;
    }

    if (RwObjectGetType(GetRwObject()) == rpATOMIC && CTagManager::IsTag(*this)) {
        CTagManager::RenderTagForPC(*GetRpAtomic());
        return;
    }

    const bool isJellyfish = GetModelIndex() == ModelIndices::MI_JELLYFISH || GetModelIndex() == ModelIndices::MI_JELLYFISH01;

    uint32 savedAlphaRef;
    if (isJellyfish) {
        RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(&savedAlphaRef));
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(0u));
    }

    m_bImBeingRendered = true;

    if (RwObjectGetType(GetRwObject()) == rpATOMIC) {
        RpAtomicRender(GetRpAtomic());
    } else {
        RpClumpRender(GetRpClump());
    }

    CStreaming::RenderEntity(m_pStreamingLink);
    RenderEffects();

    // Como el original: primero se restaura el estado de render y despues se baja la bandera
    if (isJellyfish) {
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(savedAlphaRef));
    }

    m_bImBeingRendered = false;
}

// Sets up lighting for the entity by activating directional lights
// and calculating the contribution from nearby point lights
// in Renderer.cpp
// 0x553DC0
bool CEntity::SetupLighting() {
    if (!m_bLightObject) {
        return false;
    }

    ActivateDirectional();
    const auto fLight = CPointLights::GenerateLightsAffectingObject(&GetPosition(), nullptr, this) * 0.5F;
    SetLightColoursForPedsCarsAndObjects(fLight);

    return true;
}

// Resets the entity's lighting to the default ambient and directional colors
// in Renderer.cpp
// 0x553370
void CEntity::RemoveLighting(bool reset) {
    if (reset) {
        SetAmbientColours();
        DeActivateDirectional();
        CPointLights::RemoveLightsAffectingObject();
    }
}

// Updates the matrices of the RenderWare object's frame hierarchy
// 0x532B00
void CEntity::UpdateRwFrame() {
    if (GetRwObject()) {
        RwFrameUpdateObjects(static_cast<RwFrame*>(rwObjectGetParent(GetRwObject())));
    }
}

// Updates the bone matrices for a skinned (animated) clump
// 0x532B20
void CEntity::UpdateRpHAnim() {
    if (const auto atomic = GetFirstAtomic(GetRpClump())) {
        if (RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic)) && !m_bDontUpdateHierarchy) {
            RpHAnimHierarchyUpdateMatrices(GetAnimHierarchyFromSkinClump(GetRpClump()));
        }
    }
}

// Checks if the entity model has effects that need to be processed
// during the pre-render stage, such as wind effects, glass, or pickups
// 0x532B70
bool CEntity::HasPreRenderEffects() {
    const auto modelIndex = GetModelIndex();
    auto mi = CModelInfo::GetModelInfo(modelIndex);

    if (mi->SwaysInWind() || mi->IsCrane()) {
        return true;
    }

    // Check for special models
    if (notsa::contains<eModelID>(
            { // dynamic.ide
              ModelIndices::MI_COLLECTABLE1,
              ModelIndices::MI_MONEY,
              ModelIndices::MI_CARMINE,
              ModelIndices::MI_NAUTICALMINE,
              ModelIndices::MI_BRIEFCASE,

              // multiobj.ide
              ModelIndices::MI_BEACHBALL,

              ModelIndices::MI_MAGNOCRANE_HOOK,

              // levelxre.ide
              ModelIndices::MI_WRECKING_BALL,
              ModelIndices::MI_CRANE_MAGNET,
              ModelIndices::MI_MINI_MAGNET,

              ModelIndices::MI_CRANE_HARNESS,

              // countn2.ide
              ModelIndices::MI_WINDSOCK,

              // defauld.ide
              ModelIndices::MI_FLARE },
            GetModelId()
        )) {
        return true;
    }

    // fixed model IDs, in defauld.ide
    if (notsa::contains({MODEL_MISSILE, MODEL_GRENADE, MODEL_MOLOTOV }, GetModelId())) {
        return true;
    }

    if (CGlass::IsObjectGlass(this)) {
        return true;
    }

    // Checking for a pickup object
    if (GetIsTypeObject() && AsObject()->objectFlags.bIsPickup) {
        return true;
    }

    // Checking traffic lights
    if (CTrafficLights::IsMITrafficLight(modelIndex)) {
        return true;
    }

    // Checking street lights
    if (notsa::contains<eModelID>(
            { ModelIndices::MI_SINGLESTREETLIGHTS1,
              ModelIndices::MI_SINGLESTREETLIGHTS2,
              ModelIndices::MI_SINGLESTREETLIGHTS3,
              ModelIndices::MI_DOUBLESTREETLIGHTS },
            GetModelId()
        )) {
        return true;
    }

    // Checking for 2D light effects
    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return false;
    }

    for (int32 i = 0; i < numEffects; ++i) {
        if (mi->Get2dEffect(i)->m_Type == e2dEffectType::EFFECT_LIGHT) {
            return true;
        }
    }

    return false;
}

// Determines if the entity should be ignored by flying vehicles during collision checks
// 0x532D40
bool CEntity::DoesNotCollideWithFlyers() {
    auto mi = GetModelInfo();
    return mi->SwaysInWind() || mi->bDontCollideWithFlyer;
}

// RenderWare callback to update UV animations for a material
// 0x532D70
RpMaterial* MaterialUpdateUVAnimCB(RpMaterial* material, void* data) {
    if (!RpMaterialUVAnimExists(material)) {
        return material;
    }

    // Como el original: ms_fTimeStep * 0,02f (0x858B38), no una division entre 50
    const float fTimeStep = CTimer::ms_fTimeStep * 0.02f;
    RpMaterialUVAnimAddAnimTime(material, fTimeStep);
    RpMaterialUVAnimApplyUpdate(material);
    return material;
}

// Calculates and applies a rotation matrix to a windsock entity
// to make it point in the direction of the current wind
// 0x532DB0
void CEntity::BuildWindSockMatrix() {
    // Como el original (0x532DB0): el viento se invierte (fchs) antes de normalizar
    auto vecWindDir = CVector(-(CWeather::WindDir.x + 0.01F), -(CWeather::WindDir.y + 0.01F), 0.1F);

    auto forward = vecWindDir;
    forward.Normalise();

    constexpr CVector side{ 0.0F, 0.0F, 1.0F };
    auto right = side.Cross(forward);
    right.Normalise();

    const auto up = forward.Cross(right);
    auto& matrix = GetMatrix();
    matrix.GetRight() = right;
    matrix.GetForward() = forward;
    matrix.GetUp() = up;

    UpdateRwMatrix();
    UpdateRwFrame();
}

// Determines if the entity's center point is located within the given world sector
// 0x533050
bool CEntity::LivesInThisNonOverlapSector(int32 x, int32 y) {
    const auto rect = GetBoundRect();

    // Como el original (0x533050): centro = (derecha + izquierda) * 0.5 y (borde +4 + borde +0xC) * 0.5; sector = floor(c * 0.02 + 60)
    const float centerX = (rect.right + rect.left) * 0.5F;
    const float centerY = (rect.top + rect.bottom) * 0.5F;

    const auto middleX = EntityA_SectorCoord(centerX);
    const auto middleY = EntityA_SectorCoord(centerY);

    return x == middleX && y == middleY;
}

// Configures the entity as a "big building" for LOD streaming purposes,
// disabling its collision and making it persistent
// 0x533150
void CEntity::SetupBigBuilding() {
    // Como el original (0x533150): el EXE pone los bits 8 (m_bIsBIGBuilding) y 16 (m_bDontCastShadowsOn) de +0x1C
    // (`or eax,10100h`); la DLL ponia el bit 10 (m_bStreamingDontDelete).
    SetUsesCollision(false);
    m_bIsBIGBuilding = true;
    m_bDontCastShadowsOn = true;
    GetModelInfo()->SetOwnsColModel(true);
}

// Rotates the entity's matrix around the Z-axis over time
// This is used for the spinning part of a crane
// 0x533170
void CEntity::ModifyMatrixForCrane() {
    if (!CTimer::GetIsPaused()) {
        if (GetRwObject()) {
            auto parentMatrix = GetModellingMatrix();
            if (!parentMatrix) {
                return;
            }

            auto tempMat = CMatrix(parentMatrix, false);
            // Como el original (0x533170): constante exacta del EXE (0x863980), no PI / 512.26F
            const auto fRot = static_cast<float>(CTimer::GetTimeInMS() & 0x3FF) * std::bit_cast<float>(0x3BC8F5C3u);
            tempMat.SetRotateZOnly(fRot);
            tempMat.UpdateRW();
            UpdateRwFrame();
        }
    }
}

// Handles pre-rendering logic for glass objects by registering
// them with the glass manager and temporarily hiding them
// 0x533240
void CEntity::PreRenderForGlassWindow() {
    if (!GetModelInfo()->IsGlassType2()) {
        CGlass::AskForObjectToBeRenderedInGlass(this);
        SetIsVisible(false);
    }
}

// Sets the alpha transparency for all materials of the entity's RenderWare object
// 0x5332C0
void CEntity::SetRwObjectAlpha(int32 alpha) {
    auto* const object = GetRwObject();
    if (!object) {
        return;
    }
    // Como el original: cualquier otro tipo de objeto no hace nada (sin NOTSA_UNREACHABLE)
    switch (RwObjectGetType(object)) {
    case rpATOMIC: SetAtomicAlpha(GetRpAtomic(), (void*)(alpha)); break;
    case rpCLUMP:  RpClumpForAllAtomics(GetRpClump(), SetAtomicAlpha, (void*)alpha); break;
    default:       break;
    }
}

// Checks if an entity pointer is valid by verifying it's within the
// memory pool for its respective type (e.g., building, ped, vehicle)
// 0x533310
bool IsEntityPointerValid(CEntity* entity) {
    if (!entity) {
        return false;
    }

    switch (entity->GetType()) {
    case ENTITY_TYPE_BUILDING:
        return IsBuildingPointerValid(entity->AsBuilding());
    case ENTITY_TYPE_VEHICLE:
        return IsVehiclePointerValid(entity->AsVehicle());
    case ENTITY_TYPE_PED:
        return IsPedPointerValid(entity->AsPed());
    case ENTITY_TYPE_OBJECT:
        return IsObjectPointerValid(entity->AsObject());
    case ENTITY_TYPE_DUMMY:
        return IsDummyPointerValid(entity->AsDummy());
    case ENTITY_TYPE_NOTINPOOLS:
        return true;
    }

    return false;
}

// Como el original: MultiplyMatrixWithVector (0x59C890) con el orden de sumas del EXE (x: z, y, x; y: z, x, y; z: z, x, y)
static CVector EntityGroupB_MultiplyPoint59C890(const CMatrix& m, const CVector& v) {
    return CVector{
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// Finds the world coordinates of a 2dEffect trigger point on the entity's model by its index
// 0x533380
CVector* CEntity::FindTriggerPointCoors(CVector* outVec, int32 index) {
    auto mi = GetModelInfo();
    const int32 numEffects = mi->m_n2dfxCount;
    if (numEffects) {
        for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
            auto effect = mi->Get2dEffect(iFxInd);
            if (effect->m_Type == e2dEffectType::EFFECT_TRIGGER_POINT && effect->slotMachineIndex.m_nId == index) {
                *outVec = EntityGroupB_MultiplyPoint59C890(GetMatrix(), effect->m_Pos);
                return outVec;
            }
        }
    }

    outVec->Reset();
    return outVec;
}

// Finds a random 2dEffect of a specific type attached to the entity's model,
// optionally only considering effects with free slots (for ped attractors)
// 0x533410
C2dEffect* CEntity::GetRandom2dEffect(int32 effectType, bool mustBeFree) {
    auto mi = GetModelInfo();

    std::array<C2dEffect*, 32> candidates{};
    size_t numCandidates = 0;

    const int32 numEffects = mi->m_n2dfxCount;
    if (numEffects) {
        for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
            auto effect = mi->Get2dEffect(iFxInd);

            if (effect->m_Type != effectType) {
                continue;
            }

            // Como el original: sin comprobar el tipo (HasEmptySlot lo comprueba por dentro)
            if (mustBeFree && !GetPedAttractorManager()->HasEmptySlot(reinterpret_cast<C2dEffectPedAttractor*>(effect), this)) {
                continue;
            }

            if (numCandidates < candidates.size()) {
                candidates[numCandidates++] = effect;
            }
        }
    }

    // Como el original: (rand() & 0xFFFF) * (1/32768) * numCandidates truncado (0x858B14), no la interpolacion 1/32767
    return numCandidates ? candidates.data()[CGeneral::GetRandomIntInRange(0, (int32)numCandidates)] : nullptr;
}

// Transforms a point from the entity's local (object) space to world space
// PC Only
// 0x5334F0
CVector CEntity::TransformFromObjectSpace(const CVector& offset) const {
    if (m_matrix) {
        return EntityA_MultiplyPoint59C890(*m_matrix, offset); // Como el original: 0x59C890 con su orden de sumas
    }
    CVector result;
    TransformPoint(result, m_placement, offset);
    return result;
}

// Transforms a point from the entity's local (object) space to world space,
// storing the result in an output parameter
// PC Only
// 0x533560
CVector* CEntity::TransformFromObjectSpace(CVector& outPos, const CVector& offset) const {
    outPos = TransformFromObjectSpace(offset);
    return &outPos;
}

// Como el original (0x533A21...): atan2(-forward.x, forward.y) con fpatan (precision completa, sin pasar por float),
// por 57,29578f (0x859878) y mas el angulo base, con redondeo a 24 bits del x87 en el producto y la suma.
static float EntityGroupC_HeadingDegreesPlus(const CMatrix* mat, float placementHeading, float baseAngle) {
    static const float kRadToDeg = 57.2957763671875f;
    float outDeg;
    if (mat) {
        const float negFwdX = -mat->GetForward().x;
        const float fwdY    = mat->GetForward().y;
        outDeg = EntityHeadingDegreesPlusCpp(negFwdX, fwdY, kRadToDeg, baseAngle);
    } else {
        outDeg = placementHeading * kRadToDeg + baseAngle; // fld + fmul + fadd + fstp: un redondeo a 24 bits por operacion
    }
    return outDeg;
}

// Creates all 2dEffects (e.g., lights, particles, entry/exits)
// associated with the entity's model when the entity is created
// 0x533790
void CEntity::CreateEffects() {
    m_bHasRoadsignText = false;
    const auto* const mi = GetModelInfo();

    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
        auto* effect = mi->Get2dEffect(iFxInd);

        switch (effect->m_Type) {
        case e2dEffectType::EFFECT_LIGHT: {
            m_bHasPreRenderEffects = true;
            break;
        }
        case e2dEffectType::EFFECT_PARTICLE: {
            g_fx.CreateEntityFx(this, effect->particle.m_szName, effect->m_Pos, GetModellingMatrix());
            break;
        }
        case e2dEffectType::EFFECT_ATTRACTOR: {
            if (effect->pedAttractor.m_nAttractorType == ePedAttractorType::PED_ATTRACTOR_TRIGGER_SCRIPT) {
                CTheScripts::ScriptsForBrains.RequestAttractorScriptBrainWithThisName(effect->pedAttractor.m_szScriptName);
            }
            break;
        }
        case e2dEffectType::EFFECT_ENEX: {
            const auto vecExit = effect->m_Pos + effect->enEx.m_vecExitPosn;
            const auto vecWorldEffect = TransformFromObjectSpace(effect->m_Pos);
            const auto vecWorldExit = TransformFromObjectSpace(vecExit);

            if (effect->enEx.bTimedEffect) {
                // Como el original: los dias son un entero de 32 bits (dec eax: 0 pasa a 0xFFFFFFFF), no un uint8
                uint32 ucDays = CClock::GetGameClockDays();
                if (effect->enEx.m_nTimeOn > effect->enEx.m_nTimeOff && CClock::ms_nGameClockHours < effect->enEx.m_nTimeOff) {
                    ucDays--;
                }
                srand(std::bit_cast<uint32>(this) + ucDays);
            }

            // Como el original: el rumbo (fpatan, precision completa) se convierte a grados y se suma sin redondearlo antes
            const auto fExitRot = EntityGroupC_HeadingDegreesPlus(m_matrix, m_placement.m_fHeading, effect->enEx.m_fExitAngle);
            const auto fEnterRot = EntityGroupC_HeadingDegreesPlus(m_matrix, m_placement.m_fHeading, effect->enEx.m_fEnterAngle);
            const auto iEnExId = CEntryExitManager::AddOne(
                vecWorldEffect.x, vecWorldEffect.y, vecWorldEffect.z,
                fEnterRot,
                effect->enEx.m_vecRadius.x, effect->enEx.m_vecRadius.y,
                0,
                vecWorldExit.x, vecWorldExit.y, vecWorldExit.z,
                fExitRot,
                effect->enEx.m_nInteriorId,
                static_cast<CEntryExit::eFlags>(effect->enEx.m_nFlags1 + (effect->enEx.m_nFlags2 << 8)),
                effect->enEx.m_nSkyColor, effect->enEx.m_nTimeOn, effect->enEx.m_nTimeOff, 0,
                effect->enEx.m_szInteriorName
            );

            if (iEnExId != -1) {
                if (auto* const enex = CEntryExitManager::GetInSlot(iEnExId)) {
                    if (enex->m_pLink && !enex->m_pLink->bEnableAccess) {
                        enex->bEnableAccess = false;
                    }
                }
            }
            break;
        }
        case e2dEffectType::EFFECT_ROADSIGN: {
            m_bHasRoadsignText = true;
            const auto uiPalleteId = C2dEffect::Roadsign_GetPaletteIDFromFlags(effect->roadsign.m_nFlags);
            const auto uiLettersPerLine = C2dEffect::Roadsign_GetNumLettersFromFlags(effect->roadsign.m_nFlags);
            const auto uiNumLines = C2dEffect::Roadsign_GetNumLinesFromFlags(effect->roadsign.m_nFlags);

            const auto signAtomic = CCustomRoadsignMgr::CreateRoadsignAtomic(
                effect->roadsign.m_vecSize.x, effect->roadsign.m_vecSize.y, uiNumLines,
                &effect->roadsign.m_pText[0],
                &effect->roadsign.m_pText[16],
                &effect->roadsign.m_pText[32],
                &effect->roadsign.m_pText[48],
                uiLettersPerLine, uiPalleteId
            );

            const auto frame = RpAtomicGetFrame(signAtomic);
            RwFrameSetIdentity(frame);

            static const RwV3d axisX{ 1.0F, 0.0F, 0.0F };
            static const RwV3d axisY{ 0.0F, 1.0F, 0.0F };
            static const RwV3d axisZ{ 0.0F, 0.0F, 1.0F };

            RwFrameRotate(frame, &axisZ, effect->roadsign.m_vecRotation.z, RwOpCombineType::rwCOMBINEREPLACE);
            RwFrameRotate(frame, &axisX, effect->roadsign.m_vecRotation.x, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameRotate(frame, &axisY, effect->roadsign.m_vecRotation.y, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameTranslate(frame, &effect->m_Pos, RwOpCombineType::rwCOMBINEPOSTCONCAT);
            RwFrameUpdateObjects(frame);

            effect->roadsign.m_pAtomic = signAtomic;
            break;
        }
        case e2dEffectType::EFFECT_ESCALATOR: {
            const auto vecStart = TransformFromObjectSpace(effect->m_Pos);
            const auto vecBottom = TransformFromObjectSpace(effect->escalator.m_vecBottom);
            const auto vecTop = TransformFromObjectSpace(effect->escalator.m_vecTop);
            const auto vecEnd = TransformFromObjectSpace(effect->escalator.m_vecEnd);
            const auto bMovingDown = effect->escalator.m_nDirection == 0;

            CEscalators::AddOne(vecStart, vecBottom, vecTop, vecEnd, bMovingDown, this);
            break;
        }
        }
    }
}

// Destroys all 2dEffects associated with the entity,
// cleaning up particles, entry/exits, and other special effects
// 0x533BF0
void CEntity::DestroyEffects() {
    auto mi = GetModelInfo();
    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);

        switch (effect->m_Type) {
        case e2dEffectType::EFFECT_ATTRACTOR: {
            if (effect->pedAttractor.m_nAttractorType == ePedAttractorType::PED_ATTRACTOR_TRIGGER_SCRIPT) {
                CTheScripts::ScriptsForBrains.MarkAttractorScriptBrainWithThisNameAsNoLongerNeeded(effect->pedAttractor.m_szScriptName);
            }
            break;
        }
        case e2dEffectType::EFFECT_PARTICLE: {
            g_fx.DestroyEntityFx(this);
            break;
        }
        case e2dEffectType::EFFECT_ROADSIGN: {
            C2dEffect::DestroyAtomic(std::exchange(effect->roadsign.m_pAtomic, nullptr));
            break;
        }
        case e2dEffectType::EFFECT_ENEX: {
            const auto vecWorld = TransformFromObjectSpace(effect->m_Pos);
            const auto iNearestEnex = CEntryExitManager::FindNearestEntryExit(vecWorld, 1.5F, -1);

            if (iNearestEnex != -1) {
                auto enex = CEntryExitManager::GetInSlot(iNearestEnex);
                if (enex->bEnteredWithoutExit) {
                    enex->bDeleteEnex = true;
                } else {
                    CEntryExitManager::DeleteOne(iNearestEnex);
                }
            }
            break;
        }
        }
    }
}

// Attaches an existing RenderWare object to this entity
// This is used for objects that share a single RwObject, like LODs
// 0x533ED0
void CEntity::AttachToRwObject(RwObject* object, bool updateMatrix) {
    if (!GetIsVisible()) {
        return;
    }

    m_pRwObject = object;
    if (!GetRwObject()) {
        return;
    }

    if (updateMatrix) {
        GetMatrix().UpdateMatrix(GetModellingMatrix());
    }

    auto* mi = GetModelInfo();

    if (RwObjectGetType(GetRwObject()) == rpCLUMP && mi->IsRoad()) {
        if (GetIsTypeObject()) {
            AsObject()->AddToMovingList();
            SetIsStatic(false);
        } else {
            CWorld::ms_listMovingEntityPtrs.AddItem(AsPhysical());
        }
    }

    mi->AddRef();
    m_pStreamingLink = CStreaming::AddEntity(this);
    CreateEffects();
}

// Detaches the RenderWare object from the entity, cleaning up references
// and effects without destroying the RwObject itself
// 0x533FB0
void CEntity::DetachFromRwObject() {
    if (!GetRwObject()) {
        return;
    }

    auto* mi = GetModelInfo();
    mi->RemoveRef();

    CStreaming::RemoveEntity(m_pStreamingLink);
    m_pStreamingLink = nullptr;

    if (mi->GetModelType() == MODEL_INFO_CLUMP && mi->IsRoad() && !GetIsTypeObject()) {
        CWorld::ms_listMovingEntityPtrs.DeleteItem(AsPhysical());
    }

    DestroyEffects();
    m_pRwObject = nullptr;
}

// Gets the center of the entity's collision bounding sphere in world coordinates
// 0x534290
void CEntity::GetBoundCentre(CVector& centre) const {
    TransformFromObjectSpace(centre, GetModelInfo()->GetColModel()->GetBoundCenter());
}

// Gets the center of the entity's collision bounding sphere in world coordinates
// 0x534250
CVector CEntity::GetBoundCentre() const {
    CVector result;
    TransformFromObjectSpace(result, GetModelInfo()->GetColModel()->GetBoundCenter());
    return result;
}

// Renders special 2dEffects associated with the entity, such as roadsign text
// 0x5342B0
void CEntity::RenderEffects() {
    if (!m_bHasRoadsignText) {
        return;
    }

    auto* mi = GetModelInfo();
    const int32 numEffects = mi->m_n2dfxCount;
    if (!numEffects) {
        return;
    }

    for (int32 iFxInd = 0; iFxInd < numEffects; ++iFxInd) {
        auto effect = mi->Get2dEffect(iFxInd);
        if (effect->m_Type != e2dEffectType::EFFECT_ROADSIGN) {
            continue;
        }

        CCustomRoadsignMgr::RenderRoadsignAtomic(effect->roadsign.m_pAtomic, TheCamera.GetPosition());
    }
}

// Checks if the bounding sphere of this entity intersects with another entity's bounding sphere
// 0x5343F0
bool CEntity::GetIsTouching(CEntity* entity) const {
    CVector centreA;
    CVector centreB;

    GetBoundCentre(centreA);
    entity->GetBoundCentre(centreB);

    const auto radius = GetBoundRadius() + entity->GetBoundRadius();

    // Como el original (0x53445B..0x534487): diferencias en x87 con la suma en orden z, y, x (no x, y, z)
    const float dx = centreA.x - centreB.x;
    const float dy = centreA.y - centreB.y;
    const float dz = centreA.z - centreB.z;
    return (dz * dz + dy * dy) + dx * dx < radius * radius;
}

// Checks if the bounding sphere of this entity intersects with a given sphere
// 0x5344B0
bool CEntity::GetIsTouching(const CVector& centre, float radius) const {
    CVector centreB;

    GetBoundCentre(centreB);
    float totalRadius = GetBoundRadius() + radius;

    // Como el original (0x5344EE..0x534516): diferencias en x87 con la suma en orden z, x, y
    const float dx = centreB.x - centre.x;
    const float dy = centreB.y - centre.y;
    const float dz = centreB.z - centre.z;
    return (dz * dz + dx * dx) + dy * dy < totalRadius * totalRadius;
}

// Checks if the entity's bounding sphere is visible within the camera's view frustum
// 0x534540
bool CEntity::GetIsOnScreen() {
    return TheCamera.IsSphereVisible(GetBoundCentre(), GetBoundRadius());
}

// Performs a more accurate check to see if the entity's oriented bounding box is on screen
// 0x5345D0
bool CEntity::GetIsBoundingBoxOnScreen() {
    auto cm = GetModelInfo()->GetColModel();

    std::array<RwV3d, 2> vecNormals;

    if (m_matrix) {
        auto tempMat = CMatrix();
        Invert(*m_matrix, tempMat);
        TransformVectors(vecNormals.data(), 2, tempMat, &TheCamera.m_avecFrustumWorldNormals[0]);
    } else {
        auto tempTrans = CSimpleTransform();
        tempTrans.Invert(m_placement);
        TransformVectors(vecNormals.data(), 2, tempTrans, &TheCamera.m_avecFrustumWorldNormals[0]);
    }

    for (int32 i = 0; i < 2; ++i) {
        // Como el original (0x5345D0): la esquina se elige por el bit de signo de la normal (-0.0 y NaN negativos eligen el maximo)
        const auto chooseComponent = [](float normal, float boxMin, float boxMax) {
            return (std::bit_cast<uint32>(normal) >> 31) ? boxMax : boxMin;
        };

        const CVector worldBBPoint = TransformFromObjectSpace(CVector{
            chooseComponent(vecNormals.data()[i].x, cm->m_boundBox.m_vecMin.x, cm->m_boundBox.m_vecMax.x),
            chooseComponent(vecNormals.data()[i].y, cm->m_boundBox.m_vecMin.y, cm->m_boundBox.m_vecMax.y),
            chooseComponent(vecNormals.data()[i].z, cm->m_boundBox.m_vecMin.z, cm->m_boundBox.m_vecMax.z)
        });

        // Como el original: producto escalar en el orden z, y, x del x87
        const auto& normal = TheCamera.m_avecFrustumWorldNormals[i];
        const float dot = (worldBBPoint.z * normal.z + worldBBPoint.y * normal.y) + worldBBPoint.x * normal.x;

        // Check whether the point is outside the main frustum
        if (dot > TheCamera.m_fFrustumPlaneOffsets[i]) {
            // If the mirror is active and the point is inside the mirror frustum, skip
            if (TheCamera.m_bMirrorActive) {
                const auto& mirrorNormal = TheCamera.m_avecFrustumWorldNormals_Mirror[i];
                const float mirrorDot = (worldBBPoint.z * mirrorNormal.z + worldBBPoint.y * mirrorNormal.y) + worldBBPoint.x * mirrorNormal.x;
                if (!(mirrorDot > TheCamera.m_fFrustumPlaneOffsets_Mirror[i])) {
                    continue;
                }
            }
            // Point outside the screen
            ++numBBFailed;
            return false;
        }
    }
    return true;
}

// Modifies the entity's matrix to simulate swaying in the wind,
// typically used for trees and other vegetation
// 0x534E90
void CEntity::ModifyMatrixForTreeInWind() {
    if (CTimer::GetIsPaused()) {
        return;
    }

    auto parentMatrix = GetModellingMatrix();
    if (!parentMatrix) {
        return;
    }

    auto at = RwMatrixGetAt(parentMatrix);

    float fWindOffset;
    if (!(CWeather::Wind < 0.5F)) { // Como el original (0x534E90): el salto jp lleva aqui tambien con NaN
        const uint32 uiPhase   = (m_nRandomSeed + CTimer::GetTimeInMS() * 8) & 0xFFFF;
        const uint32 uiOffset1 = (uiPhase >> 12) & 0xF;
        const uint32 uiOffset2 = (uiOffset1 + 1) & 0xF;
        const float  fContrib  = static_cast<float>(uiPhase & 0xFFF) * 0.000244140625F;

        // Como el original: ((1 - c) * t1 + c * t2) + 1, y luego * viento * 0.015
        const float fPart1 = (1.0F - fContrib) * CWeather::saTreeWindOffsets.data()[uiOffset1];
        const float fPart2 = fContrib * CWeather::saTreeWindOffsets.data()[uiOffset2];
        fWindOffset = fPart1 + fPart2;
        fWindOffset += 1.0F;
        fWindOffset *= CWeather::Wind;
        fWindOffset *= 0.015F;
    } else {
        const uint32 uiTimeOffset = (reinterpret_cast<uint32>(this) + CTimer::GetTimeInMS()) & 0xFFF;

        // Como el original: fsin a precision completa; con viento >= 0.2 el factor es 0.008 (0x863984), si no 0.005
        const float fAngle = static_cast<float>(uiTimeOffset) * 0.0015332031762227416F;
        const bool  bStrong = !(CWeather::Wind < 0.2F);
        fWindOffset = EntityA_SinTimes(fAngle, bStrong ? 0.008F : 0.005F);
    }

    at->x = fWindOffset;
    if (GetModelInfo()->IsSwayInWind2()) {
        at->x += CWeather::Wind * 0.03F;
    }

    at->y = at->x;
    at->x *= CWeather::WindDir.x;
    at->y *= CWeather::WindDir.y;

    CWindModifiers::FindWindModifier(GetPosition(), &at->x, &at->y);
    UpdateRwFrame();
}

// Modifies the entity's matrix to simulate a banner flapping in the wind
// This function is unused in the final game
// unused
// 0x535040
void CEntity::ModifyMatrixForBannerInWind() {
    if (CTimer::GetIsPaused()) {
        return;
    }

    // Como el original (0x535040): trunc(y + x) con _ftol2, (n << 6) + tiempo, << 4 y se queda con 16 bits (sin el uint16 intermedio)
    const auto& vecPos = GetPosition();
    const int32  iPosSum  = static_cast<int32>(static_cast<int64>(vecPos.y + vecPos.x));
    const uint32 uiPhase  = ((static_cast<uint32>(iPosSum) << 6) + CTimer::GetTimeInMS()) << 4;
    const auto   uiOffset = static_cast<uint16>(uiPhase);

    const float  fContrib = static_cast<float>(uiPhase & 0x7FF) * 0.00048828125F;
    const uint32 uiIndex  = uiOffset >> 11;
    const float  fPart1   = (1.0F - fContrib) * CWeather::saBannerWindOffsets.data()[uiIndex];
    const float  fPart2   = fContrib * CWeather::saBannerWindOffsets.data()[(uiIndex + 1) & 0x1F];

    // Como el original: se multiplica por la constante elegida (0.2 / 0.43 / 0.66), no por CWeather::Wind
    float wind;
    if (CWeather::Wind < 0.1F) {
        wind = 0.2F;
    } else if (CWeather::Wind < 0.8F) {
        wind = 0.43F;
    } else {
        wind = 0.66F;
    }
    float fWindOffset = fPart1 + fPart2;
    fWindOffset = wind * fWindOffset;

    const float fZPos = std::sqrt(1.0F - fWindOffset * fWindOffset);

    CMatrix& matrix = GetMatrix();
    auto vecCross = matrix.GetForward().Cross(matrix.GetUp());
    vecCross.z = 0.0F;
    vecCross.Normalise();

    const auto vecWind = CVector{ vecCross.x * fWindOffset, vecCross.y * fWindOffset, fZPos };
    const auto vecCross2 = matrix.GetForward().Cross(vecWind);

    matrix.GetRight() = vecCross2;
    matrix.GetUp() = vecWind;

    UpdateRwMatrix();
    UpdateRwFrame();
}

// Gets the RenderWare matrix associated with the entity's RwObject frame
// 0x46A2D0
RwMatrix* CEntity::GetModellingMatrix() {
    if (!GetRwObject()) {
        return nullptr;
    }

    return GetRwMatrix();
}

// Gets the collision model for the entity
// For vehicles, this can return a special collision model if one is active
// 0x535300
CColModel* CEntity::GetColModel() const {
    if (GetIsTypeVehicle()) {
        const auto veh = AsVehicle();
        if (veh->m_vehicleSpecialColIndex > -1) {
            return &CVehicle::m_aSpecialColModel.data()[veh->m_vehicleSpecialColIndex]; // Como el original: sin comprobar el rango del indice
        }
    }

    return GetModelInfo()->GetColModel();
}

// Calculates the four corner points of the 2D projection of the entity's
// oriented bounding box onto the XY plane
// https://gamedev.stackexchange.com/a/35948
// https://gamedev.stackexchange.com/questions/153326/how-to-rotate-directional-billboard-particle-sprites-toward-the-direction-the-pa/153814#153814
// 0x535340
void CEntity::CalculateBBProjection(CVector* point1, CVector* point2, CVector* point3, CVector* point4) {
    CMatrix& matrix = GetMatrix();

    // Extract local axes from the matrix
    const auto& [right, forward, up] = std::make_tuple(
        matrix.GetRight(),
        matrix.GetForward(),
        matrix.GetUp()
    );

    // Calculate the lengths of the projections of the object's local axes. This allows for scaling to be taken into account,
    // since when scaling is present, the lengths of the right, forward, and up vectors will differ from 1
    // Como el original (0x535340): la longitud es 2D (solo x e y), sqrt(y * y + x * x)
    const auto [lengthProjX, lengthProjY, lengthProjZ] = std::make_tuple(
        std::sqrt(right.y * right.y + right.x * right.x),
        std::sqrt(forward.x * forward.x + forward.y * forward.y),
        std::sqrt(up.x * up.x + up.y * up.y)
    );

    // Get bounding box
    const auto cm = GetModelInfo()->GetColModel();
    const auto [sizeBBX, sizeBBY, sizeBBZ] = std::make_tuple(
        std::max(-cm->m_boundBox.m_vecMin.x, cm->m_boundBox.m_vecMax.x),
        std::max(-cm->m_boundBox.m_vecMin.y, cm->m_boundBox.m_vecMax.y),
        std::max(-cm->m_boundBox.m_vecMin.z, cm->m_boundBox.m_vecMax.z)
    );

    // Calculate sizes
    const auto [totalSizeX, totalSizeY, totalSizeZ] = std::make_tuple(
        sizeBBX * lengthProjX * 2.0F,
        sizeBBY * lengthProjY * 2.0F,
        sizeBBZ * lengthProjZ * 2.0F
    );

    const auto& pos = GetPosition();

    float offsetLength1, offsetWidth1, offsetLength;
    float offsetLength2, offsetWidth2, offsetWidth;
    CVector main;
    CVector higherPoint, lowerPoint;

    // Determine dominant axis and compute direction vectors
    if (totalSizeX > totalSizeY && totalSizeX > totalSizeZ) {
        main = CVector(right.x, right.y, 0.0F);
        higherPoint = pos + main * sizeBBX;
        lowerPoint = pos - main * sizeBBX;

        main.Normalise();
        offsetLength1 = (main.x * forward.x + main.y * forward.y) * sizeBBY;
        offsetWidth1  = (main.x * forward.y - main.y * forward.x) * sizeBBY;
        offsetLength2 = (main.x * up.x      + main.y * up.y)      * sizeBBZ;
        offsetWidth2  = (main.x * up.y      - main.y * up.x)      * sizeBBZ;
    } else if (totalSizeY > totalSizeZ) {
        main = CVector(forward.x, forward.y, 0.0F);
        higherPoint = pos + main * sizeBBY;
        lowerPoint = pos - main * sizeBBY;

        main.Normalise();
        offsetLength1 = (main.x * right.x + main.y * right.y) * sizeBBX;
        offsetWidth1  = (main.x * right.y - main.y * right.x) * sizeBBX;
        offsetLength2 = (main.x * up.x    + main.y * up.y)    * sizeBBZ;
        offsetWidth2  = (main.x * up.y    - main.y * up.x)    * sizeBBZ;
    } else {
        main = CVector(up.x, up.y, 0.0F);
        higherPoint = pos + main * sizeBBZ;
        lowerPoint = pos - main * sizeBBZ;

        main.Normalise();
        offsetLength1 = (main.x * right.x   + main.y * right.y)   * sizeBBX;
        offsetWidth1  = (main.x * right.y   - main.y * right.x)   * sizeBBX;
        offsetLength2 = (main.x * forward.x + main.y * forward.y) * sizeBBY;
        offsetWidth2  = (main.x * forward.y - main.y * forward.x) * sizeBBY;
    }

    // Compute absolute values and sums
    // Como el original: (v < 0) ? -v : v, no fabs (que cambia el signo de -0.0)
    offsetLength = EntityA_AbsLikeExe(offsetLength2) + EntityA_AbsLikeExe(offsetLength1);
    offsetWidth  = EntityA_AbsLikeExe(offsetWidth2) + EntityA_AbsLikeExe(offsetWidth1);

    // Helper lambda for corner calculation
    const auto calcCorner = [&](const CVector& base, float sign1, float sign2) -> CVector {
        return {
            base.x + sign1 * (main.x * offsetLength) + sign2 * (main.y * offsetWidth),
            base.y + sign1 * (main.y * offsetLength) + sign2 * (-main.x * offsetWidth),
            pos.z
        };
    };

    *point1 = calcCorner(higherPoint, 1.0F, -1.0F);
    *point2 = calcCorner(higherPoint, 1.0F, 1.0F);
    *point3 = calcCorner(lowerPoint, -1.0F, 1.0F);
    *point4 = calcCorner(lowerPoint, -1.0F, -1.0F);
}

// Updates the animations for the entity's RenderWare clump, if it has any
// Animations are only updated if the entity is on screen
// 0x535F00
void CEntity::UpdateAnim() {
    m_bDontUpdateHierarchy = false;
    if (!GetRwObject() || RwObjectGetType(GetRwObject()) != rpCLUMP) {
        return;
    }

    if (!RpAnimBlendClumpGetFirstAssociation(GetRpClump())) {
        return;
    }

    bool  bOnScreen;
    float fStep;
    if (GetIsTypeObject() && AsObject()->m_nObjectType == eObjectType::OBJECT_TYPE_CUTSCENE) {
        bOnScreen = true;
        fStep = CTimer::ms_fTimeStepNonClipped * 0.02F; // Como el original (0x535F00): multiplica por 0.02, no divide entre 50
    } else {
        if (!m_bOffscreen) {
            m_bOffscreen = !GetIsOnScreen();
        }

        bOnScreen = !m_bOffscreen;
        fStep = CTimer::ms_fTimeStep * 0.02F;
    }

    RpAnimBlendClumpUpdateAnimations(GetRpClump(), fStep, bOnScreen);
}

// Checks if the entity is currently visible on screen
// 0x536BC0
bool CEntity::IsVisible() {
    if (!GetRwObject() || !GetIsVisible()) {
        return false;
    }

    return GetIsOnScreen();
}

// Calculates the distance from the collision model's center of mass
// to the base (lowest Z point) of the model
// 0x536BE0
float CEntity::GetDistanceFromCentreOfMassToBaseOfModel() const {
    float lowestZ;

    CColModel& colMod = *GetColModel();
    lowestZ = colMod.m_boundBox.m_vecMin.z;
    return -lowestZ;
}

// Removes a specific reference to this entity from its list of references
// Used when an entity that was pointing to this one is now pointing elsewhere or is being destroyed
// in references.cpp
// 0x571A00
void CEntity::CleanUpOldReference(CEntity** entity) {
    auto lastnextp = &m_pReferences;
    for (auto ref = m_pReferences; ref; ref = ref->m_pNext) {
        if (ref->m_ppEntity == entity) {
            *lastnextp = ref->m_pNext;
            ref->m_pNext = CReferences::pEmptyList;
            CReferences::pEmptyList = ref;
            ref->m_ppEntity = nullptr;
            break;
        }
        lastnextp = &ref->m_pNext;
    }
}

// Notifies all entities that reference this one that it's being destroyed
// It iterates through its list of references and sets their pointers to null
// in references.cpp
// 0x571A40
void CEntity::ResolveReferences() {
    for (auto ref = m_pReferences; ref; ref = ref->m_pNext) {
        // Como el original (0x571A50): sin comprobar que m_ppEntity sea nulo
        if (*ref->m_ppEntity == this) {
            *ref->m_ppEntity = nullptr;
        }
    }

    if (m_pReferences) {
        auto head = m_pReferences;

        auto tail = head;
        while (tail->m_pNext) {
            tail = tail->m_pNext;
        }

        for (auto ref = head; ref; ref = ref->m_pNext) {
            ref->m_ppEntity = nullptr;
        }

        tail->m_pNext = CReferences::pEmptyList;
        CReferences::pEmptyList = head;

        m_pReferences = nullptr;
    }
}

// Cleans up the entity's reference list by removing any references
// from other entities that no longer point to this one
// in references.cpp
// 0x571A90
void CEntity::PruneReferences() {
    if (!m_pReferences) {
        return;
    }

    auto refs = m_pReferences;
    auto ppPrev = &m_pReferences;

    while (refs) {
        if (*refs->m_ppEntity == this) {
            ppPrev = &refs->m_pNext;
            refs = refs->m_pNext;
        } else {
            auto refTemp = refs->m_pNext;
            *ppPrev = refs->m_pNext;
            refs->m_pNext = CReferences::pEmptyList;
            CReferences::pEmptyList = refs;
            refs->m_ppEntity = nullptr;
            refs = refTemp;
        }
    }
}

// Registers a new reference to this entity
// The `entity` parameter is a pointer to a CEntity pointer that now points to `this`
// in references.cpp
// 0x571B70
void CEntity::RegisterReference(CEntity** entity) {
    if (GetIsTypeBuilding() && !m_bIsTempBuilding && !m_bIsProcObject && !GetIplIndex()) {
        return;
    }

    for (auto refs = m_pReferences; refs; refs = refs->m_pNext) {
        if (refs->m_ppEntity == entity) {
            return;
        }
    }

    if (!CReferences::pEmptyList) {
        // Como el original (0x571BC3..0x571C90): cada pool (peatones, vehiculos y objetos) se recorre de la ultima
        // ranura a la primera, y se para en cuanto la lista de libres deja de estar vacia
        const auto PruneDescending = [](auto* pool) {
            for (int32 i = (int32)pool->GetSize(); i > 0;) {
                --i;
                if (auto* const entity = pool->GetAt(i)) {
                    entity->PruneReferences();
                    if (CReferences::pEmptyList) {
                        return;
                    }
                }
            }
        };

        PruneDescending(GetPedPool());
        if (!CReferences::pEmptyList) {
            PruneDescending(GetVehiclePool());
        }
        if (!CReferences::pEmptyList) {
            PruneDescending(GetObjectPool());
        }
    }

    if (CReferences::pEmptyList) {
        auto pEmptyRef = CReferences::pEmptyList;
        CReferences::pEmptyList = pEmptyRef->m_pNext;
        pEmptyRef->m_pNext = m_pReferences;
        m_pReferences = pEmptyRef;
        pEmptyRef->m_ppEntity = entity;
    }
}

// Processes all light-type 2dEffects for this entity
// This creates coronas, point lights, and shadows based on the effect's properties
// in Coronas.cpp
// 0x6FC7A0
// Como el original: transcripcion del EXE (orden de las sumas del x87, constantes exactas, ids de corona, tiradas de azar
// y la estructura de saltos entre coronas "encendida", "apagada" y actualizacion de coordenadas).
void CEntity::ProcessLightsForEntity() {
    constexpr static const uint16 randomSeedRandomiser[8] = { 0, 27'034, 43'861, 52'326, 64'495, 38'437, 21'930, 39'117 }; // 0x8D5028
    constexpr static const float FADE_RATE = std::bit_cast<float>(0x3A6BF405u); // 0x872538 (1/1111 como float, no 0,0009)
    constexpr static const float RECIP_256 = 0.00390625f;                       // 0x859AA0
    const auto toByte = [](float f) { return static_cast<uint8>(static_cast<int32>(f)); }; // _ftol2 y byte bajo del argumento

    const float dayNightBalance = GetDayNightBalance();
    if (m_bRenderDamaged || !GetIsVisible()) {
        return;
    }

    if (GetIsTypeVehicle()) {
        if (AsVehicle()->physicalFlags.bRenderScorched) {
            return;
        }
    } else if (m_matrix && m_matrix->GetUp().z < 0.96f) {
        return;
    }

    CBaseModelInfo* mi = GetModelInfo();
    if (!mi->m_n2dfxCount) {
        return;
    }

    // Como el original: el numero de efectos se relee del model info en cada vuelta
    for (int32 C = 0; C < mi->m_n2dfxCount; ++C) {
        const auto* effect = mi->Get2dEffect(C);
        const uint32 randomSeed = static_cast<uint16>(randomSeedRandomiser[C & 7] ^ m_nRandomSeed);
        float TimeFade = 1.0f;

        if (effect->m_Type == e2dEffectType::EFFECT_SUN_GLARE) {
            if (!(CWeather::SunGlare >= 0.0f)) {
                continue;
            }

            CVector effectPos = TransformFromObjectSpace(effect->m_Pos);
            const CVector& entityPos = GetPosition();
            CVector direction{ effectPos.x - entityPos.x, effectPos.y - entityPos.y, effectPos.z - entityPos.z };
            direction.Normalise();

            const CVector& cameraPos = TheCamera.GetPosition();
            const float camDx = cameraPos.x - effectPos.x;
            const float camDy = cameraPos.y - effectPos.y;
            const float camDz = cameraPos.z - effectPos.z;
            const float cameraDist = std::sqrt((camDy * camDy + camDz * camDz) + camDx * camDx); // orden y, z, x
            const float scale = 2.0f / cameraDist;                                                // 0x858CA0
            const CVector scaledCameraDir{ camDx * scale, camDy * scale, camDz * scale };

            direction.x = scaledCameraDir.x + direction.x;
            direction.y = scaledCameraDir.y + direction.y;
            direction.z = scaledCameraDir.z + direction.z;
            direction.Normalise();

            const CVector& sunVec = CTimeCycle::m_VectorToSun.data()[CTimeCycle::m_CurrentStoredValue];
            const float dotProduct = -((direction.y * sunVec.y + direction.z * sunVec.z) + direction.x * sunVec.x);
            if (!(dotProduct > 0.0f)) {
                continue;
            }

            const float glare = std::sqrt(dotProduct) * CWeather::SunGlare;
            const float radius = std::sqrt(cameraDist) * CWeather::SunGlare * 0.5f;
            effectPos.x = scaledCameraDir.x + effectPos.x;
            effectPos.y = scaledCameraDir.y + effectPos.y;
            effectPos.z = scaledCameraDir.z + effectPos.z;

            // Como el original: se multiplica por 0,33333334f (0x859040) en vez de dividir entre 3
            const uint8 blue  = toByte(static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreBlue  + 510) * glare * 0.33333334f);
            const uint8 green = toByte(static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreGreen + 510) * glare * 0.33333334f);
            const uint8 red   = toByte(static_cast<float>(CTimeCycle::m_CurrentColours.m_nSunCoreRed   + 510) * glare * 0.33333334f);

            // Como el original: el id es this + C + 0x21 (direccion del objeto), no la semilla
            CCoronas::RegisterCorona(
                std::bit_cast<uint32>(this) + static_cast<uint32>(C) + 0x21u,
                nullptr,
                red,
                green,
                blue,
                255,
                effectPos,
                radius,
                120.0f,
                gpCoronaTexture[CORONATYPE_SHINYSTAR],
                eCoronaFlareType::FLARETYPE_NONE,
                eCoronaReflType::CORREFL_NONE,
                eCoronaLOSCheck::LOSCHECK_OFF,
                eCoronaTrail::TRAIL_OFF,
                0.0f,
                false,
                1.5f,
                0,
                15.0f,
                false,
                false
            );
            continue;
        }

        if (effect->m_Type != e2dEffectType::EFFECT_LIGHT) {
            continue;
        }

        CVector effectPos = TransformFromObjectSpace(effect->m_Pos);
        const uint16 lightFlags = effect->light.m_nFlags;
        const uint32 timeMs = CTimer::GetTimeInMS();
        bool ApplyLight = false;
        bool ApplyOnVersion = false;
        bool UpdateCoors = false;
        bool ApplyOffVersion = false;
        float LightFade = 1.0f;

        if ((lightFlags & 0x20) && (lightFlags & 0x40)) { // de dia y de noche
            ApplyOffVersion = true;
        } else if ((lightFlags & 0x20) && dayNightBalance < 1.0f) {
            ApplyOffVersion = true;
            LightFade = 1.0f - dayNightBalance;
        } else if ((lightFlags & 0x40) && dayNightBalance > 0.0f) {
            ApplyOffVersion = true;
            LightFade = dayNightBalance;
        }

        const uint8 flashType = effect->light.m_nCoronaFlashType;
        if ((flashType == e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET && CWeather::WetRoads > 0.5f) || ApplyOffVersion) {
            switch (flashType) {
            case e2dCoronaFlashType::FLASH_DEFAULT:
                ApplyLight = true;
                break;
            case e2dCoronaFlashType::FLASH_RANDOM:
            case e2dCoronaFlashType::FLASH_RANDOM_WHEN_WET:
                if ((randomSeed ^ timeMs) & 0x60) {
                    ApplyLight = true;
                } else {
                    ApplyOnVersion = true;
                }
                // Como el original: el tiempo se desplaza 11 bits (/2048), no 12
                if (((timeMs >> 11) ^ randomSeed) & 0x3) {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ANIM_SPEED_4X:
                if (((timeMs + C * 128) & 0x200) == 0) {
                    UpdateCoors = true;
                } else {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ANIM_SPEED_2X:
                if (((timeMs + C * 256) & 0x400) == 0) {
                    UpdateCoors = true;
                } else {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ANIM_SPEED_1X:
                if (((timeMs + C * 512) & 0x800) == 0) {
                    UpdateCoors = true;
                } else {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_UNKN:
                if ((randomSeed & 0xFF) > 0x10) {
                    ApplyLight = true;
                    break;
                }
                if (((randomSeed * 8) ^ timeMs) & 0x60) {
                    ApplyLight = true;
                } else {
                    ApplyOnVersion = true;
                }
                if (((timeMs >> 11) ^ randomSeed) & 0x3) {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_TRAFFICLIGHT:
                break;
            case e2dCoronaFlashType::FLASH_TRAINCROSSING:
                if (GetIsTypeObject() && AsObject()->objectFlags.bTrainCrossEnabled) {
                    if (timeMs & 0x400) {
                        ApplyLight = true;
                    }
                    if (C & 1) {
                        ApplyLight = !ApplyLight;
                    }
                }
                if (C >= 4) {
                    ApplyLight = false;
                }
                break;
            case e2dCoronaFlashType::FLASH_UNUSED:
                if (CBridge::ShouldLightsBeFlashing() && (timeMs & 0x1FF) < 0x3C) {
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_ONLY_RAIN:
                if (CWeather::Rain > 0.0001f) {
                    TimeFade = CWeather::Rain;
                    ApplyLight = true;
                }
                break;
            case e2dCoronaFlashType::FLASH_5ON_5OFF:
            case e2dCoronaFlashType::FLASH_6ON_4OFF:
            case e2dCoronaFlashType::FLASH_4ON_6OFF: {
                ApplyLight = true;
                // Como el original: cada suma pasa por el x87 (float de 24 bits) antes del _ftol2 y el resto es entero con signo
                const int32 iBaseOff = static_cast<int32>(timeMs + static_cast<uint32>((static_cast<int32>(flashType) - 11) * 3333));
                const CVector& entityPos = GetPosition();
                const float fPosX = entityPos.x, fPosY = entityPos.y;
                static const float kMulX = 20.0f, kMulY = 10.0f; // 0x858BA4, 0x85862C
                const float fSumX = FloatPlusIntCpp(fPosX * kMulX, iBaseOff); // fld + fmul + fiadd (el producto se redondea a 24 bits)
                const int32 iOffX = static_cast<int32>(fSumX);
                const float fSumY = FloatPlusIntCpp(fPosY * kMulY, iOffX);
                const int32 iOffY = static_cast<int32>(fSumY);

                // Como el original: el modo sale de (resto * 9) / 10000 (con signo), no de resto / 10000 * 9
                const int32 rem = iOffY % 10'000;
                const int32 mode = rem * 9 / 10'000;
                const float fade = static_cast<float>(rem - 1'111 * mode) * FADE_RATE;
                if (static_cast<uint32>(mode) > 3u) {
                    ApplyLight = false;
                } else {
                    switch (mode) {
                    case 0:  TimeFade = fade; break;
                    case 1:
                    case 2:  TimeFade = 1.0f; break;
                    case 3:  TimeFade = 1.0f - fade; break;
                    }
                }
                break;
            }
            default:
                break;
            }
        }

        // CORONAS
        // Como el original: 0x6FCC15 llama siempre a LaRiotsActiveHere; los saltos del EXE se reproducen con tres banderas
        bool gotoNormalCorona = false; // 0x6FCECD
        bool gotoOnCheck      = false; // 0x6FCE65
        bool gotoOnCorona     = false; // 0x6FCE71
        if (CGameLogic::LaRiotsActiveHere()) {
            if (!ApplyLight) {
                gotoOnCheck = true;
            } else if (!GetIsTypeVehicle()) {
                const uint32 seedLow = randomSeed & 3;
                if (seedLow == 0 || (seedLow == 1 && (((randomSeed * 8) ^ timeMs) & 0x60))) {
                    ApplyLight = false;
                    ApplyOnVersion = true;
                    gotoOnCorona = true;
                }
            }
        }

        if (!gotoOnCheck && !gotoOnCorona) { // 0x6FCD84
            if (!ApplyLight) {
                gotoOnCheck = true;
            } else if (lightFlags & 0x800) { // m_bCheckDirection
                const float offX = static_cast<float>(static_cast<int8>(effect->light.offsetX));
                const float offY = static_cast<float>(static_cast<int8>(effect->light.offsetY));
                const float offZ = static_cast<float>(static_cast<int8>(effect->light.offsetZ));
                // Como el original: AllocateMatrix + UpdateMatrix si no hay matriz; Multiply3x3 (0x59C790) con su orden de sumas
                (void)GetMatrix();
                const CMatrix& mat = *m_matrix;
                const CVector lightPos{
                    (mat.GetUp().x * offZ + mat.GetForward().x * offY) + mat.GetRight().x * offX,
                    (mat.GetUp().y * offZ + mat.GetRight().y * offX) + mat.GetForward().y * offY,
                    (mat.GetUp().z * offZ + mat.GetRight().z * offX) + mat.GetForward().z * offY
                };
                const CVector& camPos = TheCamera.GetPosition();
                const float toCamX = camPos.x - effectPos.x;
                const float toCamY = camPos.y - effectPos.y;
                const float toCamZ = camPos.z - effectPos.z;
                // Como el original: producto escalar (y + z) + x
                const float dot = (lightPos.y * toCamY + lightPos.z * toCamZ) + toCamX * lightPos.x;
                if (dot < 0.0f) {
                    ApplyLight = false;
                    UpdateCoors = true;
                    gotoOnCheck = true;
                } else {
                    gotoNormalCorona = true;
                }
            } else {
                gotoNormalCorona = true;
            }
        }

        if (gotoNormalCorona) {
            float brightness;
            if (lightFlags & 0x80) { // m_bBlinking1
                brightness = (1.0f - static_cast<float>(CGeneral::GetRandomNumber() & 0x1F) * 0.012f) * TimeFade;
            } else {
                brightness = TimeFade;
            }
            const uint32 frame = CTimer::GetFrameCounter();
            if (lightFlags & 0x200) { // m_bBlinking2
                if ((randomSeed + frame) & 3) {
                    brightness = 0.0f;
                }
            }
            if (lightFlags & 0x1000) { // m_bBlinking3
                const uint32 phase = (frame + randomSeed) & 0x3F;
                if (phase == 1) {
                    brightness *= 0.5f;
                } else if (phase != 0) {
                    brightness = 0.0f;
                }
            }

            float sizeMult = 1.0f;
            if (m_nModelIndex == MODEL_RCBARON) {
                brightness *= 1.9f; // 0x866C0C
                sizeMult = 2.0f;
            }

            // Como el original: * 0,1f (0x858B1C), no / 10
            TimeFade = CTimeCycle::m_CurrentColours.m_fSpriteBrightness * brightness * 0.1f;
            const float coronaSize = sizeMult * effect->light.m_fCoronaSize;

            CCoronas::RegisterCorona(
                std::bit_cast<uint32>(this) + static_cast<uint32>(C),
                nullptr,
                toByte(static_cast<float>(effect->light.m_color.red) * TimeFade),
                toByte(static_cast<float>(effect->light.m_color.green) * TimeFade),
                toByte(static_cast<float>(effect->light.m_color.blue) * TimeFade),
                toByte(LightFade * 255.0f),
                effectPos,
                coronaSize,
                effect->light.m_fCoronaFarClip,
                effect->light.m_pCoronaTex,
                static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                eCoronaTrail::TRAIL_OFF,
                0.0f,
                effect->light.m_bOnlyLongDistance,
                0.8f,
                0,
                15.0f,
                effect->light.m_bOnlyFromBelow,
                effect->light.m_bUpdateHeightAboveGround
            );
        } else {
            if (gotoOnCheck && ApplyOnVersion) {
                gotoOnCorona = true;
            }
            if (gotoOnCorona) {
                CCoronas::RegisterCorona(
                    std::bit_cast<uint32>(this) + static_cast<uint32>(C),
                    nullptr,
                    0,
                    0,
                    0,
                    255,
                    effectPos,
                    effect->light.m_fCoronaSize,
                    effect->light.m_fCoronaFarClip,
                    effect->light.m_pCoronaTex,
                    static_cast<eCoronaFlareType>(effect->light.m_nCoronaFlareType),
                    static_cast<eCoronaReflType>(effect->light.m_bCoronaEnableReflection),
                    static_cast<eCoronaLOSCheck>(effect->light.m_bCheckObstacles),
                    eCoronaTrail::TRAIL_OFF,
                    0.0f,
                    effect->light.m_bOnlyLongDistance,
                    1.5f,
                    0,
                    15.0f,
                    false,
                    false
                );
            } else if (UpdateCoors) {
                CCoronas::UpdateCoronaCoors(
                    std::bit_cast<uint32>(this) + static_cast<uint32>(C),
                    effectPos,
                    effect->light.m_fCoronaFarClip,
                    0.0f
                );
            }
        }

        // POINT LIGHTS
        // Como el original: con rango != 0 y ApplyLight, los colores (si alguno != 0) van con el desvanecimiento; sin color es
        // una luz anti-luz y entonces se continua con la niebla; con rango 0 solo la niebla.
        bool skipFog = false;
        if (effect->light.m_fPointlightRange != 0.0f && ApplyLight) {
            const auto& color = effect->light.m_color;
            if (color.red || color.green || color.blue) {
                const float colorMult = LightFade * TimeFade * RECIP_256;
                skipFog = true;
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_POINTLIGHT,
                    effectPos,
                    CVector{},
                    effect->light.m_fPointlightRange,
                    static_cast<float>(color.red) * colorMult,
                    static_cast<float>(color.green) * colorMult,
                    static_cast<float>(color.blue) * colorMult,
                    static_cast<uint8>((lightFlags >> 1) & 3),
                    true,
                    nullptr
                );
            } else {
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_ANTILIGHT,
                    effectPos,
                    CVector{},
                    effect->light.m_fPointlightRange,
                    0.0f,
                    0.0f,
                    0.0f,
                    RwFogType::rwFOGTYPENAFOGTYPE,
                    true,
                    nullptr
                );
            }
        }

        if (!skipFog) {
            const auto& color = effect->light.m_color;
            if (lightFlags & 0x4) { // niebla exponencial
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_ONLYFOGEFFECT_ALWAYS,
                    effectPos,
                    CVector{},
                    0.0f,
                    static_cast<float>(color.red) * RECIP_256,
                    static_cast<float>(color.green) * RECIP_256,
                    static_cast<float>(color.blue) * RECIP_256,
                    RwFogType::rwFOGTYPEEXPONENTIAL,
                    true,
                    nullptr
                );
            } else if ((lightFlags & 0x2) && ApplyLight && effect->light.m_fPointlightRange == 0.0f) { // niebla lineal
                CPointLights::AddLight(
                    ePointLightType::PLTYPE_ONLYFOGEFFECT,
                    effectPos,
                    CVector{},
                    0.0f,
                    static_cast<float>(color.red) * RECIP_256,
                    static_cast<float>(color.green) * RECIP_256,
                    static_cast<float>(color.blue) * RECIP_256,
                    RwFogType::rwFOGTYPELINEAR,
                    true,
                    nullptr
                );
            }
        }

        // PC Only?
        // SHADOWS
        if (effect->light.m_fShadowSize != 0.0f) {
            // Como el original: la distancia en Z se lee sin signo (movzx)
            float shadowZ = 15.0f;
            if (effect->light.m_nShadowZDistance) {
                shadowZ = static_cast<float>(static_cast<uint8>(effect->light.m_nShadowZDistance));
            }

            if (ApplyLight) {
                // Como el original: ((canal * multiplicador) * TimeFade) * (1/256), cada paso a float
                const float mult = static_cast<float>(effect->light.m_nShadowColorMultiplier);
                const uint8 blue  = toByte(static_cast<float>(effect->light.m_color.blue)  * mult * TimeFade * RECIP_256);
                const uint8 green = toByte(static_cast<float>(effect->light.m_color.green) * mult * TimeFade * RECIP_256);
                const uint8 red   = toByte(static_cast<float>(effect->light.m_color.red)   * mult * TimeFade * RECIP_256);

                CShadows::StoreStaticShadow(
                    std::bit_cast<uint32>(this) + static_cast<uint32>(C),
                    eShadowType::SHADOW_ADDITIVE,
                    effect->light.m_pShadowTex,
                    effectPos,
                    effect->light.m_fShadowSize,
                    0.0f,
                    0.0f,
                    -effect->light.m_fShadowSize,
                    128,
                    red,
                    green,
                    blue,
                    shadowZ,
                    1.0f,
                    40.0f,
                    false,
                    0.0f
                );
            } else if (ApplyOnVersion) {
                CShadows::StoreStaticShadow(
                    std::bit_cast<uint32>(this) + static_cast<uint32>(C),
                    eShadowType::SHADOW_ADDITIVE,
                    effect->light.m_pShadowTex,
                    effectPos,
                    effect->light.m_fShadowSize,
                    0.0f,
                    0.0f,
                    -effect->light.m_fShadowSize,
                    0,
                    0,
                    0,
                    0,
                    shadowZ,
                    1.0f,
                    40.0f,
                    false,
                    0.0f
                );
            }
        }
    }
}

// Finds and removes any escalator data associated with this entity
// in fluff.cpp
// 0x717900
void CEntity::RemoveEscalatorsForEntity() {
    // Como el original: no se sale tras la primera coincidencia (la entidad puede tener varias escaleras)
    for (auto& escalator : CEscalators::GetAllExists()) {
        if (escalator.GetEntity() == this) {
            escalator.Remove();
        }
    }
}

// Checks if the entity is occluded by any active occluders in the world
// It tests various points on the entity's bounding box against the occluders
// in occlusion.cpp
// 0x71FAE0
bool CEntity::IsEntityOccluded() {
    // Como el original (0x71FAE0): el centro se calcula antes de mirar si hay oclusores y los puntos se transforman con la
    // matriz (0x59C890, reservandola si hace falta) y se comprueban uno a uno, sin precalcularlos
    CVector center;
    GetBoundCentre(center);

    if (static_cast<int32>(COcclusion::NumActiveOccluders) == 0) {
        return false;
    }

    CVector centerScrPos;
    float   scaleX, scaleY;
    if (!CalcScreenCoors(center, centerScrPos, scaleX, scaleY)) {
        return false;
    }

    const float longEdge = (scaleX > scaleY) ? scaleX : scaleY; // fcomp + jne: con NaN o igualdad se queda con scaleY
    const float boundingRadius = GetModelInfo()->GetColModel()->GetBoundingSphere().m_fRadius;
    const float occlusionRadius = (boundingRadius * longEdge) * 0.9F;

    const int32 numOccluders = static_cast<int32>(COcclusion::NumActiveOccluders);
    for (int32 i = 0; i < numOccluders; ++i) {
        const CActiveOccluder& o = COcclusion::ActiveOccluders.data()[i];

        // Como el original: la distancia es un int16 con signo (movsx)
        if (!(static_cast<float>(static_cast<int16>(o.m_DistToCam)) < centerScrPos.z - boundingRadius)) {
            continue;
        }

        if (o.IsPointWithinOcclusionArea(CVector2D{ centerScrPos.x, centerScrPos.y }, occlusionRadius)) {
            if (o.IsPointBehindOccluder(center, boundingRadius)) {
                return true;
            }
        }

        if (!o.IsPointWithinOcclusionArea(CVector2D{ centerScrPos.x, centerScrPos.y }, 0.0F)) {
            continue;
        }

        const auto GetBox = [this]() -> decltype(auto) { return GetModelInfo()->GetColModel()->GetBoundingBox(); };

        // Prueba de un punto de la caja: si falla, no se prueban los siguientes (pero se siguen transformando)
        bool failed = false;
        const auto TestPoint = [&](const CVector& objectSpacePoint) {
            const CVector worldPoint = EntityA_MultiplyPoint59C890(GetMatrix(), objectSpacePoint);
            if (failed) {
                return;
            }
            CVector screenPoint;
            if (!CalcScreenCoors(worldPoint, screenPoint)
                || !o.IsPointWithinOcclusionArea(CVector2D{ screenPoint.x, screenPoint.y }, 0.0F)
                || !o.IsPointBehindOccluder(worldPoint, 0.0F)) {
                failed = true;
            }
        };

        TestPoint(GetBox().m_vecMin);
        TestPoint(GetBox().m_vecMax);
        TestPoint(CVector{ GetBox().m_vecMin.x, GetBox().m_vecMax.y, GetBox().m_vecMax.z });
        TestPoint(CVector{ GetBox().m_vecMax.x, GetBox().m_vecMin.y, GetBox().m_vecMin.z });
        if (failed) {
            continue;
        }

        // Cajas pequenas: valen los cuatro puntos. Con NaN se comporta como el x87 (no es "> 60" ni "> 30")
        const float width  = GetBox().m_vecMax.x - GetBox().m_vecMin.x;
        const float length = GetBox().m_vecMax.y - GetBox().m_vecMin.y;
        const float height = GetBox().m_vecMax.z - GetBox().m_vecMin.z;
        if (!(width > 60.0F) && !(length > 60.0F)) {
            return true;
        }
        if (!(height > 30.0F)) {
            return true;
        }

        TestPoint(CVector{ GetBox().m_vecMin.x, GetBox().m_vecMin.y, GetBox().m_vecMax.z });
        TestPoint(CVector{ GetBox().m_vecMax.x, GetBox().m_vecMin.y, GetBox().m_vecMax.z });
        if (!failed) {
            return true;
        }
    }

    return false;
}

// in header
inline float CEntity::GetBoundRadius() const {
    return GetModelInfo()->GetColModel()->GetBoundingSphere().m_fRadius;
}

// Updates the entity's RenderWare matrix from its CPlaceable transform data
// This synchronizes the game's coordinate system with RenderWare's
// 0x446F90
void CEntity::UpdateRwMatrix() {
    if (!GetRwObject()) {
        return;
    }

    auto parentMatrix = GetModellingMatrix();
    if (m_matrix) {
        m_matrix->UpdateRwMatrix(parentMatrix);
    } else {
        m_placement.UpdateRwMatrix(parentMatrix);
    }
}

// notsa
RwMatrix* CEntity::GetBoneMatrix(RwUInt32 nodeID) const {
    if (const auto h = GetAnimHierarchyFromClump(GetRpClump())) {
        return RpHAnimHierarchyGetNodeMatrix(h, nodeID);
    }
    return nullptr;
}

// notsa
RpAtomic* CEntity::GetRpAtomicOrFirstAtomicOfClump() const noexcept {
    const auto obj = GetRwObject();
    if (!obj) {
        return nullptr;
    }
    switch (const auto type = RwObjectGetType(obj)) {
    case rpATOMIC: return GetRpAtomic();                /* If we're an atomic, use that */
    case rpCLUMP:  return GetFirstAtomic(GetRpClump()); /* Otherwise use the first atomic of the clump */
    default:       NOTSA_UNREACHABLE_CASE(type);        /* Invalid type */
    }
}

// Traverses the LOD chain to find the last (lowest-detail) LOD entity
// If the entity has no LOD, it returns itself
// NOTSA
CEntity* CEntity::FindLastLOD() noexcept {
    CEntity* it = this;
    for (; it->m_pLod; it = it->m_pLod);
    return it;
}

// A helper to get the CBaseModelInfo pointer for the entity's model index
// NOTSA
CBaseModelInfo* CEntity::GetModelInfo() const {
    return CModelInfo::GetModelInfo(GetModelIndex());
}

// Updates the entity's scan code to the current frame's code
// This is used to prevent processing the same entity multiple times in one frame
// Returns true if the entity's scan code was updated
// NOTSA
bool CEntity::ProcessScan() {
    if (IsScanCodeCurrent()) {
        return false;
    }
    SetCurrentScanCode();
    return true;
}

// A RenderWare callback to set the alpha on all materials of an atomic
// 0x533290
RpAtomic* SetAtomicAlpha(RpAtomic* atomic, void* data) {
    auto geometry = RpAtomicGetGeometry(atomic);
    RpGeometrySetFlags(geometry, RpGeometryGetFlags(geometry) | rpGEOMETRYMODULATEMATERIALCOLOR);
    RpGeometryForAllMaterials(geometry, SetCompAlphaCB, data);
    return atomic;
}

// A RenderWare callback to set the alpha component of a single material's color
// 0x533280
RpMaterial* SetCompAlphaCB(RpMaterial* material, void* data) {
    RpMaterialGetColor(material)->alpha = (RwUInt8)(uintptr)data;
    return material;
}

// Checks if the entity's scan code matches the current global scan code,
// indicating if it has already been processed in the current frame
// NOTSA
bool CEntity::IsScanCodeCurrent() const {
    return GetScanCode() == CWorld::GetCurrentScanCode();
}

// Sets the entity's scan code to the current global scan code
// NOTSA
void CEntity::SetCurrentScanCode() {
    SetScanCode(CWorld::GetCurrentScanCode());
}
