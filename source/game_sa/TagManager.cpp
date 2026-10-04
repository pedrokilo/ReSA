/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTagManager
 * Reconstrucción: COMPLETA — 19/19 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 19/19 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include <reversiblebugfixes/Bugs.hpp>

#include "TagManager.h"
#include "Garages.h"

void CTagManager::InjectHooks() {
    RH_ScopedClass(CTagManager);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x49CC50);
    RH_ScopedInstall(ShutdownForRestart, 0x49CC60);
    RH_ScopedInstall(GetTagPos, 0x49CEA0);
    RH_ScopedInstall(AddTag, 0x49CC90);
    RH_ScopedInstall(FindTagDesc, 0x49CCB0);
    RH_ScopedInstall(IsTag, 0x49CCE0);
    RH_ScopedOverloadedInstall(SetAlpha, "RpAtomic", 0x49CD30, void(*)(RpAtomic&, uint8));
    RH_ScopedOverloadedInstall(SetAlpha, "Entity", 0x49CEC0, void (*)(CEntity&, uint8));
    RH_ScopedOverloadedInstall(GetAlpha, "RpAtomic", 0x49CD40, uint8(*)(const RpAtomic&));
    RH_ScopedOverloadedInstall(GetAlpha, "Entity", 0x49CF90, uint8(*)(const CEntity&));
    RH_ScopedInstall(GetPercentageTagged, 0x49CDA0);
    RH_ScopedInstall(GetPercentageTaggedInArea, 0x49D0B0);
    RH_ScopedInstall(UpdateNumTagged, 0x49CDE0);
    RH_ScopedInstall(SetAlphaInArea, 0x49CFE0);
    RH_ScopedInstall(GetNearestTag, 0x49D160);
    RH_ScopedInstall(SetupAtomic, 0x49CE10);
    RH_ScopedInstall(RenderTagForPC, 0x49CE40);
    RH_ScopedInstall(Save, 0x5D3D60);
    RH_ScopedInstall(Load, 0x5D3DA0);
}

// 0x49CC50
void CTagManager::Init() {
    ZoneScoped;

    ms_numTags   = 0;
    ms_numTagged = 0;
}

// 0x49CC60
void CTagManager::ShutdownForRestart() {
#if 0
    // Doing this here breaks the game, so i'll just leave it as it was originally...
    rng::fill(ms_tagDesc, tTagDesc{});
    if (notsa::bugfixes::CTagManager_MissingTagCountResetOnShutdown) {
        ms_numTags = 0;
    }
#else
    // Como el original: solo se limpian las ms_numTags entradas, de la última a la primera
    for (auto i = (uint32)ms_numTags; i != 0;) {
        ms_tagDesc.data()[--i].Alpha = 0;
    }
#endif
    ms_numTagged = 0;
}

// 0x49CEA0
const CVector& CTagManager::GetTagPos(int32 idx) {
    // Como el original: sin comprobar el índice
    return ms_tagDesc.data()[idx].Entity->GetPosition();
}

// 0x49CC90
void CTagManager::AddTag(CEntity& entity) {
    // Como el original: sin assert ni comprobación de límite; solo se escriben la entidad y el alfa
    auto& tag  = ms_tagDesc.data()[ms_numTags];
    tag.Entity = &entity;
    tag.Alpha  = 0;
    ms_numTags++;
}

// 0x49CCB0
tTagDesc* CTagManager::FindTagDesc(const CEntity& entity) {
    // Como el original: se busca de la última entrada a la primera
    for (auto i = (uint32)ms_numTags; i != 0;) {
        auto& tag = ms_tagDesc.data()[--i];
        if (tag.Entity == &entity) {
            return &tag;
        }
    }
    NOTSA_LOG_WARN("CTagManager::FindTagDesc: Couldn't find tag desc for entity %p - This shouldn't happen! Perhaps corrupted savefile?", LOG_PTR(&entity));
    return nullptr;
}

// 0x49CCE0
bool CTagManager::IsTag(const CEntity& entity) {
    auto mi = entity.GetModelInfo();
    if (mi->GetRwModelType() != rpATOMIC) {
        return false;
    }

    return mi->IsTagModel() && !mi->AsAtomicModelInfoPtr()->bTagDisabled;
}

// 0x49CDA0
int32 CTagManager::GetPercentageTagged() {
    // Como el original: los contadores se convierten sin signo (fild + 2^32) y el resultado se trunca con _ftol2 (64 bits)
    return (int32)(int64)((float)(uint32)ms_numTagged / (float)(uint32)ms_numTags * 100.0f);
}

// Como el original: posición de la entidad en el área (x en [left, right], y en [bottom, top]; NaN pasa)
static bool IsTagInArea(const CRect& area, const tTagDesc& tag) {
    const auto& pos = tag.Entity->GetPosition();
    return !(pos.x < area.left || pos.x > area.right || pos.y < area.bottom || pos.y > area.top);
}

// 0x49D0B0
int32 CTagManager::GetPercentageTaggedInArea(const CRect& area) {
    // Como el original: recorrido de la última a la primera y división en coma flotante truncada con _ftol2
    int32 numTotalTaggable = 0, numTagged = 0;
    for (auto i = (uint32)ms_numTags; i != 0;) {
        const auto& tag = ms_tagDesc.data()[--i];
        if (!IsTagInArea(area, tag)) {
            continue;
        }
        ++numTotalTaggable;
        if (tag.Alpha > ALPHA_TAGGED) {
            ++numTagged;
        }
    }
    return (int32)(int64)((float)numTagged / (float)numTotalTaggable * 100.0f);
}

// 0x49CDE0
void CTagManager::UpdateNumTagged() {
    ms_numTagged = rng::count_if(GetTags(), [](const tTagDesc& tag) {
        return tag.Alpha > ALPHA_TAGGED;
    });
}

uint8 CTagManager::GetAlpha(const RpAtomic& atomic) {
    return static_cast<uint8>(CVisibilityPlugins::GetUserValue(&atomic));
}

uint8 CTagManager::GetAlpha(const CEntity& entity) {
    // Como el original: sin assert
    if (entity.GetRpAtomic()) {
        return static_cast<uint8>(CVisibilityPlugins::GetUserValue(entity.GetRpAtomic()));
    }

    auto* const desc = FindTagDesc(entity);
    if (notsa::bugfixes::GenericCrashing) {
        if (!desc) {
            return 0;
        }
    }
    return desc->Alpha;
}

// 0x49CD30
void CTagManager::SetAlpha(RpAtomic& atomic, uint8 alphaToSet) {
    CVisibilityPlugins::SetUserValue(&atomic, alphaToSet);
}

// 0x49CEC0
void CTagManager::SetAlpha(CEntity& entity, uint8 alphaToSet) {
    // Como el original: sin assert
    if (auto* const atomicOfEntity = entity.GetRpAtomic()) {
        SetAlpha(*atomicOfEntity, alphaToSet);
    }

    auto* const tag = FindTagDesc(entity);
    if (notsa::bugfixes::GenericCrashing) {
        if (!tag) {
            return;
        }
    }

    const auto justGotTagged = alphaToSet > ALPHA_TAGGED && tag->Alpha <= ALPHA_TAGGED;
    tag->Alpha               = alphaToSet;
    UpdateNumTagged();

    if (justGotTagged && !TheCamera.m_bWideScreenOn) {
        if (ms_numTagged == ms_numTags) {
            CGarages::TriggerMessage("TAG_ALL", -1, 5'000u, -1);
        } else {
            CGarages::TriggerMessage("TAG_ONE", ms_numTagged, 5'000u, ms_numTags);
        }
    }
}

void CTagManager::ResetAlpha(const CEntity& entity) {
    assert(IsTag(entity));

    auto* const atomicOfEntity = entity.GetRpAtomic();
    if (!atomicOfEntity) {
        return;
    }
    auto* const tagOfEntity = FindTagDesc(entity);
    if (notsa::bugfixes::GenericCrashing) {
        if (!tagOfEntity) {
            return;
        }
    }
    SetAlpha(*atomicOfEntity, tagOfEntity->Alpha);
}

// 0x49CFE0
void CTagManager::SetAlphaInArea(const CRect& area, uint8 alphaToSet) {
    // Como el original: de la última a la primera; el alfa de la etiqueta se guarda aunque no tenga atómico
    for (auto i = (uint32)ms_numTags; i != 0;) {
        auto& tag = ms_tagDesc.data()[--i];
        if (!IsTagInArea(area, tag)) {
            continue;
        }
        if (auto* const atomicOfTag = tag.Entity->GetRpAtomic()) {
            SetAlpha(*atomicOfTag, alphaToSet);
        }
        tag.Alpha = alphaToSet;
    }
    UpdateNumTagged();
}

// 0x49D160
CEntity* CTagManager::GetNearestTag(const CVector& nearestToPoint) {
    // Como el original: de la última a la primera (en empate gana el índice más alto), distancia dy² + dx²
    int32 closest         = -1;
    float closestDist2DSq = RwRealMAXVAL;
    for (auto i = (uint32)ms_numTags; i != 0;) {
        --i;
        const auto& pos      = ms_tagDesc.data()[i].Entity->GetPosition();
        const float dx       = nearestToPoint.x - pos.x;
        const float dy       = nearestToPoint.y - pos.y;
        const float dist2DSq = dy * dy + dx * dx;
        if (dist2DSq < closestDist2DSq) {
            closestDist2DSq = dist2DSq;
            closest         = (int32)i;
        }
    }
    return closest >= 0
        ? ms_tagDesc.data()[closest].Entity
        : nullptr; // BUGFIX(Pirulax): Original caller site does handle nullptr just fine, they just kinda forgot to return null if there are no tags at all xD
}

// 0x49CE10
void CTagManager::SetupAtomic(RpAtomic& atomic) {
    auto geometry      = RpAtomicGetGeometry(&atomic);
    auto material      = RpGeometryGetMaterial(geometry, 1);
    material->pipeline = ms_pPipeline;
    RpGeometrySetFlags(geometry, RpGeometryGetFlags(geometry) | rpGEOMETRYMODULATEMATERIALCOLOR);
    SetAlpha(atomic, 0);
}

// 0x49CE40
void CTagManager::RenderTagForPC(RpAtomic& atomic) {
    auto geometry = RpAtomicGetGeometry(&atomic);
    auto material = RpGeometryGetMaterial(geometry, 1);
    // Como el original: alfa = byte bajo del valor de usuario * valor de usuario completo / 255 (dos lecturas)
    auto color             = *RpMaterialGetColor(material);
    const int32 alphaLow   = static_cast<uint8>(CVisibilityPlugins::GetUserValue(&atomic));
    const int32 alphaWhole = CVisibilityPlugins::GetUserValue(&atomic);
    color.alpha            = static_cast<uint8>(alphaLow * alphaWhole / 255);
    *RpMaterialGetColor(material) = color;
    RpAtomicRender(&atomic);
}

// 0x5D3D60
bool CTagManager::Save() {
    CGenericGameStorage::SaveDataToWorkBuffer(ms_numTags);

    // Como el original: el contador se relee en cada vuelta y se devuelve true
    for (uint32 i = 0; i < (uint32)ms_numTags; i++) {
        CGenericGameStorage::SaveDataToWorkBuffer(ms_tagDesc.data()[i].Alpha);
    }
    return true;
}

// 0x5D3DA0
bool CTagManager::Load() {
#if 0 // FIX_BUGS - NOTE(Pirulax): I've tested, and it breaks the tags (Just try the "tagging the turf" mission), so I guess just leave it as-is
    // TODO: Verify if there are no side effects due to this fix - it's done the same way on android but I'm not sure why this worked even without reading the data to ms_numTags
    // The loop below always iterated the proper amount of times, so i assume the tags are loaded at some point before save is loaded, and we already have all the tags initialized
    CGenericGameStorage::LoadDataFromWorkBuffer(ms_numTags);
#else
    CGenericGameStorage::SaveDataToWorkBuffer(ms_numTags); // Yeah, original also saves into buffer instead of loading
#endif

    // Como el original: el contador se relee en cada vuelta y se devuelve true
    for (uint32 i = 0; i < (uint32)ms_numTags; i++) {
        CGenericGameStorage::LoadDataFromWorkBuffer(ms_tagDesc.data()[i].Alpha);
    }

    NOTSA_LOG_DEBUG("Loaded {} tags", ms_numTags);
    return true;
}
