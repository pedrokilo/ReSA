/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimManager
 * Reconstrucción: COMPLETA — 31/31 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 31/31 entradas.
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

#include <extensions/ci_string.hpp>

#include "AnimManager.h"
#include "AnimAssocDescriptions.h"

void CAnimManager::InjectHooks() {
    RH_ScopedClass(CAnimManager);
    RH_ScopedCategory("Animation");

    RH_ScopedInstall(Initialise, 0x5BF6B0);
    RH_ScopedInstall(ReadAnimAssociationDefinitions, 0x5BC910);
    RH_ScopedInstall(Shutdown, 0x4D4130);
    RH_ScopedOverloadedInstall(GetAnimationBlock, "", 0x4D3940, CAnimBlock*(*)(const char*));
    RH_ScopedOverloadedInstall(GetAnimationBlockIndex, "by-name", 0x4D3990, int32(*)(const char*));
    RH_ScopedInstall(GetFirstAssocGroup, 0x4D39B0);
    RH_ScopedOverloadedInstall(GetAnimation, "0", 0x4D39F0, CAnimBlendHierarchy*(*)(uint32, const CAnimBlock*));
    RH_ScopedOverloadedInstall(GetAnimation, "1", 0x4D42F0, CAnimBlendHierarchy*(*)(const char*, const CAnimBlock*));
    RH_ScopedInstall(GetAnimGroupName, 0x4D3A20);
    RH_ScopedInstall(GetAnimBlockName, 0x4D3A30);
    RH_ScopedInstall(CreateAnimAssociation, 0x4D3A40);
    RH_ScopedOverloadedInstall(GetAnimAssociation, "by-id", 0x4D3A60, CAnimBlendStaticAssociation*(*)(AssocGroupId, AnimationId));
    RH_ScopedOverloadedInstall(GetAnimAssociation, "by-name", 0x4D3A80, CAnimBlendStaticAssociation*(*)(AssocGroupId, const char*));
    RH_ScopedOverloadedInstall(AddAnimation, "id", 0x4D3AA0, CAnimBlendAssociation*(*)(RpClump*, AssocGroupId, AnimationId));
    RH_ScopedOverloadedInstall(AddAnimation, "hier", 0x4D4330, CAnimBlendAssociation*(*)(RpClump*, CAnimBlendHierarchy*, int32));
    RH_ScopedInstall(AddAnimationAndSync, 0x4D3B30);
    RH_ScopedInstall(AddAnimAssocDefinition, 0x4D3BA0);
    RH_ScopedInstall(AddAnimToAssocDefinition, 0x4D3C80);
    RH_ScopedInstall(CreateAnimAssocGroups, 0x4D3CC0);
    RH_ScopedInstall(RegisterAnimBlock, 0x4D3E50);
    RH_ScopedInstall(RemoveLastAnimFile, 0x4D3ED0);
    RH_ScopedInstall(RemoveAnimBlock, 0x4D3F40);
    RH_ScopedInstall(AddAnimBlockRef, 0x4D3FB0);
    RH_ScopedInstall(RemoveAnimBlockRefWithoutDelete, 0x4D3FF0);
    RH_ScopedInstall(GetNumRefsToAnimBlock, 0x4D4010);
    RH_ScopedInstall(UncompressAnimation, 0x4D41C0);
    RH_ScopedInstall(RemoveFromUncompressedCache, 0x4D42A0);
    RH_ScopedOverloadedInstall(BlendAnimation, "id", 0x4D4610, CAnimBlendAssociation*(*)(RpClump*, AssocGroupId, AnimationId, float));
    RH_ScopedOverloadedInstall(BlendAnimation, "hier", 0x4D4410, CAnimBlendAssociation*(*)(RpClump*, CAnimBlendHierarchy*, int32, float));
    RH_ScopedInstall(LoadAnimFiles, 0x4D5620);
    RH_ScopedInstall(LoadAnimFile, 0x4D47F0);
}

// 0x5BF6B0
void CAnimManager::Initialise() {
    ZoneScoped;

    ms_numAnimations = 0;
    ms_numAnimBlocks = 0;
    ms_numAnimAssocDefinitions = 118; // ANIM_TOTAL_GROUPS aka NUM_ANIM_ASSOC_GROUPS
    ms_AnimCache.Init(50);
    ReadAnimAssociationDefinitions();
    RegisterAnimBlock("ped");
}

// 0x5BC910
void CAnimManager::ReadAnimAssociationDefinitions() {
    // Como el original: sscanf sin límites ni comprobación del resultado en las cabeceras, y el mismo búfer para
    // el nombre del grupo y los nombres de las animaciones.
    char groupName[32], blockName[32], type[32];
    bool                 isAnimSection = false;
    AnimAssocDefinition* def = nullptr; // El EXE lo deja sin inicializar (solo se usa tras una cabecera)
    int32                animCount;

    CFileMgr::SetDir("");
    const auto f = CFileMgr::OpenFile("DATA\\ANIMGRP.DAT", "rb");
    for (;;) {
        const auto l = CFileLoader::LoadLine(f);
        if (!l) {
            break;
        }
        if (*l == '#' || !*l) {
            continue;
        }
        if (isAnimSection) {
            if (sscanf(l, "%s", groupName) == 1) {
                if (!memcmp(groupName, "end", 4)) {
                    isAnimSection = false;
                } else {
                    AddAnimToAssocDefinition(def, groupName);
                }
            }
        } else {
            sscanf(l, "%s %s %s %d", groupName, blockName, type, &animCount);
            def = AddAnimAssocDefinition(groupName, blockName, MODEL_MALE01, animCount, aStdAnimDescs);
            isAnimSection = true;
        }
    }
    CFileMgr::CloseFile(f);
}

// 0x4D4130
void CAnimManager::Shutdown() {
    for (auto i = 0; i < NUM_ANIM_BLOCKS; i++) {
        CStreaming::RemoveModel(IFPToModelId(i));
    }

    for (auto i = 0; i < ms_numAnimations; i++) {
        ms_aAnimations[i].Shutdown();
    }

    ms_AnimCache.Shutdown();
    delete[] ms_aAnimAssocGroups;
}

CAnimBlock* CAnimManager::GetAnimationBlock(AssocGroupId animGroup) {
    return GetAssocGroups()[animGroup].m_AnimBlock;
}

// 0x4D3940
CAnimBlock* CAnimManager::GetAnimationBlock(const char* name) {
    // Como el original: _stricmp(bloque, nombre) con el contador con signo
    for (int32 i = 0; i < (int32)ms_numAnimBlocks; i++) {
        const auto ab = &ms_aAnimBlocks.data()[i];
        if (!_stricmp(ab->Name, name)) {
            return ab;
        }
    }
    return nullptr;
}

int32 CAnimManager::GetAnimationBlockIndex(AssocGroupId animGroup) {
    return GetAnimationBlock(animGroup) - ms_aAnimBlocks.data();
}

int32 CAnimManager::GetAnimationBlockIndex(CAnimBlock* animBlock) {
    return animBlock - ms_aAnimBlocks.data();
}

// 0x4D3990
int32 CAnimManager::GetAnimationBlockIndex(const char* name) {
    const auto b = GetAnimationBlock(name);
    return b
        ? (int32)(std::distance(ms_aAnimBlocks.data(), b))
        : -1;
}

// 0x4D39B0
AssocGroupId CAnimManager::GetFirstAssocGroup(const char* name) {
    // Como el original: _stricmp sobre las 118 definiciones fijas
    for (auto i = 0; i < ANIM_GROUP_MAN; i++) {
        if (!_stricmp(ms_aAnimAssocDefinitions[i].BlockName, name)) {
            return static_cast<AssocGroupId>(i);
        }
    }
    return ANIM_GROUP_MAN;
}

// 0x4D39F0
CAnimBlendHierarchy* CAnimManager::GetAnimation(uint32 hash, const CAnimBlock* animBlock) {
    // Como el original: el número de animaciones del bloque es con signo y el índice no se comprueba
    auto h = &ms_aAnimations.data()[animBlock->FirstAnimIdx];
    for (int32 i = 0; i < (int32)animBlock->NumAnims; i++, h++) {
        if (h->m_hashKey == hash) {
            return h;
        }
    }
    return nullptr;
}

// 0x4D42F0
CAnimBlendHierarchy* CAnimManager::GetAnimation(const char* animName, const CAnimBlock* animBlock) {
    return GetAnimation(CKeyGen::GetUppercaseKey(animName), animBlock);
}

// notsa
CAnimBlendHierarchy& CAnimManager::GetAnimation(AnimationId id) {
    return ms_aAnimations[(size_t)id];
}

// 0x4D3A20
const char* CAnimManager::GetAnimGroupName(AssocGroupId groupId) {
    return ms_aAnimAssocDefinitions[groupId].GroupName;
}

// 0x4D3A30
const char* CAnimManager::GetAnimBlockName(AssocGroupId groupId) {
    return ms_aAnimAssocDefinitions[groupId].BlockName;
}

// NOTSA
AssocGroupId CAnimManager::GetAnimationGroupIdByName(notsa::ci_string_view name) {
    for (const auto& [i, gd] : rngv::enumerate(GetAssocGroupDefs())) {
        if (gd.GroupName == name) {
            return (AssocGroupId)(i);
        }
    }
    NOTSA_UNREACHABLE("Couldn't find group with name {}", name);
}

// 0x4D3A40
CAnimBlendAssociation* CAnimManager::CreateAnimAssociation(AssocGroupId groupId, AnimationId animId) {
    return ms_aAnimAssocGroups[groupId].CopyAnimation(animId); // Como el original: sin comprobar el rango
}

// 0x4D3A60
CAnimBlendStaticAssociation* CAnimManager::GetAnimAssociation(AssocGroupId groupId, AnimationId animId) {
    return ms_aAnimAssocGroups[groupId].GetAnimation(animId);
}

// 0x4D3A80
CAnimBlendStaticAssociation* CAnimManager::GetAnimAssociation(AssocGroupId groupId, const char* animName) {
    return ms_aAnimAssocGroups[groupId].GetAnimation(animName);
}

CAnimBlendAssociation* CAnimManager::AddAnimationToClump(RpClump* clump, CAnimBlendAssociation* anim) {
    const auto clumpAnims = &RpAnimBlendClumpGetData(clump)->m_AnimList;

    CAnimBlendAssociation* syncWith{};
    if (anim->IsSyncronised()) {
        for (auto l = clumpAnims->next; l; l = l->next) {
            const auto a = CAnimBlendAssociation::FromLink(l);
            if (a->IsSyncronised()) {
                syncWith = a;
                break;
            }
        }
    }

    if (syncWith) {
        anim->SyncAnimation(syncWith);
        anim->m_Flags |= ANIMATION_IS_PLAYING;
    } else {
        anim->Start(0.0f);
    }

    clumpAnims->Prepend(&anim->m_Link);

    return anim;
}

// 0x4D3AA0
CAnimBlendAssociation* CAnimManager::AddAnimation(RpClump* clump, AssocGroupId groupId, AnimationId animId) {
    return AddAnimationToClump(clump, CreateAnimAssociation(groupId, animId));
}

// 0x4D4330
CAnimBlendAssociation* CAnimManager::AddAnimation(RpClump* clump, CAnimBlendHierarchy* hier, int32 clumpAssocFlag) {
    const auto anim = new CAnimBlendAssociation(clump, hier);
    anim->m_Flags = (uint16)clumpAssocFlag; // Como el original: asigna los flags (no OR)
    anim->ReferenceAnimBlock();
    UncompressAnimation(hier);
    return AddAnimationToClump(clump, anim);
}

// 0x4D3B30
CAnimBlendAssociation* CAnimManager::AddAnimationAndSync(RpClump* clump, CAnimBlendAssociation* syncWith, AssocGroupId groupId, AnimationId animId) {
    const auto a = CreateAnimAssociation(groupId, animId);
    if (a->IsSyncronised() && syncWith) {
        a->SyncAnimation(syncWith);
        a->m_Flags |= ANIMATION_IS_PLAYING;
    } else {
        a->Start(0.0f);
    }

    RpAnimBlendClumpGetData(clump)->m_AnimList.Prepend(&a->m_Link);

    return a;
}

// 0x4D3BA0
AnimAssocDefinition* CAnimManager::AddAnimAssocDefinition(const char* groupName, const char* blockName, uint32 modelIndex, uint32 animsCount, AnimDescriptor* descriptor) {
    // Como el original: salta las definiciones ya ocupadas (nombre de grupo no vacío) y deja el contador en la
    // primera libre; el índice no se comprueba.
    AnimAssocDefinition* const defs = ms_aAnimAssocDefinitions;
    uint32 idx = ms_numAnimAssocDefinitions;
    if (defs[idx].GroupName[0]) {
        do {
            idx++;
        } while (defs[idx].GroupName[0]);
        ms_numAnimAssocDefinitions = idx;
    }
    const auto def = &defs[idx];

    strcpy(def->GroupName, groupName);
    strcpy(def->BlockName, blockName);

    def->ModelIndex = modelIndex;
    def->NumAnims   = animsCount;
    def->AnimDescr  = descriptor;

    // Como el original: dos bloques con operator new escalar; solo se pone a cero el primer byte de cada nombre.
    def->AnimNames = static_cast<const char**>(::operator new(animsCount * sizeof(const char*)));
    auto buf       = static_cast<char*>(::operator new(animsCount * AnimAssocDefinition::ANIM_NAME_BUF_SZ));
    for (uint32 i = 0; i < animsCount; i++, buf += AnimAssocDefinition::ANIM_NAME_BUF_SZ) {
        def->AnimNames[i] = buf;
        const_cast<char*>(def->AnimNames[i])[0] = '\0';
    }

    // Como el original: marca como libre la definición siguiente (si el contador queda por debajo de 145)
    if ((int32)++ms_numAnimAssocDefinitions < 0x91) {
        defs[ms_numAnimAssocDefinitions].GroupName[0] = '\0';
    }
    return def;
}

// 0x4D3C80
void CAnimManager::AddAnimToAssocDefinition(AnimAssocDefinition* def, const char* animName) {
    // Como el original: primer nombre vacío, sin comprobar el número de animaciones, y strcpy
    int32 i = 0;
    while (def->AnimNames[i][0]) {
        i++;
    }
    strcpy(const_cast<char*>(def->AnimNames[i]), animName);
}

// 0x4D3CC0
void CAnimManager::CreateAnimAssocGroups() {
    // Como el original: contador con signo y sin comprobar el rango de los grupos
    for (int32 i = 0; i < (int32)ms_numAnimAssocDefinitions; i++) {
        const auto def   = &ms_aAnimAssocDefinitions[i];
        const auto block = GetAnimationBlock(def->BlockName);
        if (block == nullptr || !block->IsLoaded) {
            continue;
        }
        auto& group = ms_aAnimAssocGroups[i];
        if (group.m_Anims) {
            continue;
        }

        RpClump* clump = nullptr;
        if (def->ModelIndex != MODEL_INVALID) {
            clump = (RpClump*)CModelInfo::GetModelInfo(def->ModelIndex)->CreateInstance();
            RpAnimBlendClumpInit(clump);
        }

        group.m_GroupID = (AssocGroupId)i;
        group.m_IdOffset = def->AnimDescr->AnimId;
        group.CreateAssociations(def->BlockName, clump, def->AnimNames, def->NumAnims);
        for (auto j = 0u; j < group.m_NumAnims; j++) {
            group.GetAnimation(def->AnimDescr[j].AnimId)->m_Flags |= def->AnimDescr[j].Flags;
        }

        if (clump) {
#ifdef SA_SKINNED_PEDS
            if (IsClumpSkinned(clump)) {
                RpClumpForAllAtomics(clump, AtomicRemoveAnimFromSkinCB, nullptr);
            }
#endif
            RpClumpDestroy(clump);
        }
    }
}

// 0x4D3E50
int32 CAnimManager::RegisterAnimBlock(const char* name) {
    CAnimBlock* ab = GetAnimationBlock(name);
    if (ab == nullptr) { // Initialize a new anim block
        // Como el original: strncpy de 16 (sin terminador si el nombre es largo), grupo buscado con el nombre
        // ya copiado y sin mirar las referencias.
        ab = &ms_aAnimBlocks.data()[ms_numAnimBlocks++];
        strncpy(ab->Name, name, MAX_ANIM_BLOCK_NAME);
        ab->NumAnims = 0;
        ab->GroupId = GetFirstAssocGroup(ab->Name);
    }

    return GetAnimationBlockIndex(ab);
}

// 0x4D3ED0
void CAnimManager::RemoveLastAnimFile() {
    const auto ab = &ms_aAnimBlocks.data()[--ms_numAnimBlocks];
    ms_numAnimations = ab->FirstAnimIdx;
    for (int32 i = 0; i < (int32)ab->NumAnims; i++) { // Como el original: contador con signo
        ms_aAnimations.data()[ab->FirstAnimIdx + i].Shutdown();
    }
    ab->IsLoaded = false;
}

// 0x4D3F40
void CAnimManager::RemoveAnimBlock(int32 index) {
    const auto ab = &ms_aAnimBlocks.data()[index];

    for (int32 i = 0; i < (int32)ms_numAnimAssocDefinitions; i++) { // Como el original: contador con signo
        if (ms_aAnimAssocGroups[i].m_AnimBlock == ab) {
            ms_aAnimAssocGroups[i].DestroyAssociations();
        }
    }

    for (int32 i = 0; i < (int32)ab->NumAnims; i++) { // Como el original: contador con signo
        ms_aAnimations.data()[ab->FirstAnimIdx + i].Shutdown();
    }

    ab->IsLoaded = false;
    ab->RefCnt  = 0;
}

// 0x4D3FB0
void CAnimManager::AddAnimBlockRef(int32 index) {
    ms_aAnimBlocks.data()[index].RefCnt++;
}

// 0x4D3FD0
void CAnimManager::RemoveAnimBlockRef(int32 index) {
    GetAnimBlocks()[index].RefCnt--;
    /* see RemoveAnimBlockRefWithoutDelete, logically here should be called RemoveModel or something
    if (--ms_aAnimBlocks[index].usRefs == 0) {
    CStreaming::RemoveModel(IFPToModelId(index));
    }
    */
}

// 0x4D3FF0
void CAnimManager::RemoveAnimBlockRefWithoutDelete(int32 index) {
    ms_aAnimBlocks.data()[index].RefCnt--;
}

// 0x4D4010
int32 CAnimManager::GetNumRefsToAnimBlock(int32 index) {
    return ms_aAnimBlocks.data()[index].RefCnt;
}

// 0x4D41C0
void CAnimManager::UncompressAnimation(CAnimBlendHierarchy* h) {
    if (h->IsRunningCompressed()) { // Keep as compressed?
        if (h->GetTotalTime() == 0.f) {
            h->CalcTotalTimeCompressed();
        }
    } else if (!h->IsUncompressed()) { // Need to uncompress?
        auto l = ms_AnimCache.Insert(h);
        if (!l) { // No more free links? (This is totally normal as animations aren't compressed back unless the cache is full)
            // Como el original: libera el último de la lista de usados, lo pasa a la lista libre y anula el m_Link
            // de su jerarquía sin mirarlo; el segundo Insert no se comprueba.
            const auto llr = ms_AnimCache.GetTail();
            llr->data->RemoveUncompressedData();
            ms_AnimCache.Remove(llr);
            llr->data->m_Link = nullptr;
            l = ms_AnimCache.Insert(h);
        }
        h->m_Link = l;
        h->Uncompress();
    } else if (h->m_Link) { // Already uncompressed, mark as recently-used in cache
        ms_AnimCache.Insert(*h->m_Link); // Remove + re-insert at head
    }
}

// 0x4D42A0
void CAnimManager::RemoveFromUncompressedCache(CAnimBlendHierarchy* h) {
    if (const auto l = h->m_Link) { // Como el original: sin comprobar que el enlace apunte a `h`
        ms_AnimCache.Remove(l);
        h->m_Link = nullptr;
    }
}

// 0x4D4410
CAnimBlendAssociation* CAnimManager::BlendAnimation(RpClump* clump, CAnimBlendHierarchy* toBlendHier, int32 toBlendFlags, float blendDelta) {
    const auto clumpAnimData = RpAnimBlendClumpGetData(clump); // Get running anim data
    const bool toBlendIsPartial = (toBlendFlags & ANIMATION_IS_PARTIAL) != 0;

    CAnimBlendAssociation* running{}; // Running instance of this anim
    bool                   bFadeThisOut = false;
    for (auto l = clumpAnimData->m_AnimList.next; l; l = l->next) {
        const auto a = CAnimBlendAssociation::FromLink(l);
        const auto h = a->m_BlendHier;
        if (h && h == toBlendHier) { // Found an instance of this anim running (como el original: gana la última)
            running = a;
            continue;
        }
        if (a->IsPartial() != toBlendIsPartial) {
            continue;
        }
        // Como el original: fcomp + test ah,41h (un NaN también pone -1), `bd < delta` estricto y el mínimo
        // con -0.05 conserva un NaN.
        if (!(a->m_BlendAmount > 0.f)) {
            a->m_BlendDelta = -1.f;
        } else {
            const float bd = -(a->m_BlendAmount * blendDelta);
            if (bd < a->m_BlendDelta || !toBlendIsPartial || (h->m_nAnimBlockId == toBlendHier->m_nAnimBlockId && h->m_nAnimBlockId)) {
                a->m_BlendDelta = -0.05f < bd ? -0.05f : bd;
            }
        }
        a->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        bFadeThisOut = true;
    }

    // If already running just re-adjust blend delta (and re-start it if it has finished)
    if (running) {
        running->m_BlendDelta = (1.f - running->m_BlendAmount) * blendDelta;
        if (running->HasFinished()) {
            running->Start(0.f);
        }
        UncompressAnimation(running->m_BlendHier); // Make sure anim doesn't get removed
        return running;
    }

    // Otherwise create new instance
    const auto a = new CAnimBlendAssociation{clump, toBlendHier};
    a->m_Flags = (uint16)toBlendFlags;
    a->ReferenceAnimBlock();
    UncompressAnimation(toBlendHier);
    clumpAnimData->m_AnimList.Prepend(&a->m_Link);
    a->Start(0.f);
    if (!bFadeThisOut && !toBlendIsPartial) {
        a->m_BlendAmount = 1.f;
    } else {
        a->m_BlendAmount = 0.f;
        a->m_BlendDelta  = blendDelta;
        UncompressAnimation(a->m_BlendHier);
    }
    return a;
}

// 0x4D4610
CAnimBlendAssociation* CAnimManager::BlendAnimation(RpClump* clump, AssocGroupId groupId, AnimationId animId, float blendDelta) {
    const auto clumpAnimData = RpAnimBlendClumpGetData(clump); // Get running anim data

    // Como el original: pide la asociación estática tres veces (una por flag)
    const bool toBlendIsMoving         = (ms_aAnimAssocGroups[groupId].GetAnimation(animId)->m_Flags & ANIMATION_IS_SYNCRONISED) != 0;
    const bool toBlendIsPartial        = (ms_aAnimAssocGroups[groupId].GetAnimation(animId)->m_Flags & ANIMATION_IS_PARTIAL) != 0;
    const bool toBlendIsIndestructible = (ms_aAnimAssocGroups[groupId].GetAnimation(animId)->m_Flags & ANIMATION_FACIAL) != 0;

    CAnimBlendAssociation *running{}, *movingAnim{}; // Running instance of this anim, and the last moving anim in the chain
    bool                   bFadeThisOut = false;
    for (auto l = clumpAnimData->m_AnimList.next; l; l = l->next) {
        const auto a = CAnimBlendAssociation::FromLink(l);

        if (toBlendIsMoving && a->IsSyncronised()) {
            movingAnim = a;
        }

        if (a->m_AnimId == animId && a->m_AnimGroupId == groupId) {
            running = a;
            continue;
        }
        if (a->IsPartial() != toBlendIsPartial || a->IsFacial() != toBlendIsIndestructible) {
            continue;
        }
        // Como el original: un NaN también pone -1, `bd < delta` estricto y el mínimo con -0.05 conserva un NaN
        if (!(a->m_BlendAmount > 0.f)) {
            a->m_BlendDelta = -1.f;
        } else {
            const float bd = -(blendDelta * a->m_BlendAmount);
            if (bd < a->m_BlendDelta || !toBlendIsPartial) {
                a->m_BlendDelta = -0.05f < bd ? -0.05f : bd;
            }
        }
        a->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        bFadeThisOut = true;
    }

    // If already running just re-adjust blend delta (and start it if it has finished)
    if (running) {
        running->m_BlendDelta = (1.f - running->m_BlendAmount) * blendDelta;
        if (running->HasFinished()) {
            running->Start(0.f);
        }
        UncompressAnimation(running->m_BlendHier);
        return running;
    }

    // Otherwise create new anim, and possibly sync it with the last moving anim of the clump
    const auto anim = AddAnimationAndSync(clump, movingAnim, groupId, animId);
    if (!bFadeThisOut && !toBlendIsPartial) {
        anim->m_BlendAmount = 1.f;
        return anim;
    }
    anim->m_BlendAmount = 0.f;
    anim->m_BlendDelta  = blendDelta;
    UncompressAnimation(anim->m_BlendHier);
    return anim;
}

//! @notsa
uint32 CAnimManager::GetAnimIndex(const CAnimBlendHierarchy* h) {
    const auto idx = h - ms_aAnimations.data();
    assert(idx >= 0 && idx <= ms_numAnimations);
    return (uint32)idx;
}

//! @notsa
bool CAnimManager::IsAnimInBlock(const CAnimBlendHierarchy* h, const CAnimBlock* b) {
    const auto animIdx = (int32)h->GetIndex();
    return animIdx >= b->FirstAnimIdx && animIdx <= b->FirstAnimIdx + (int32)b->NumAnims;
}

//! @notsa
//! @brief Function for the very common pattern found in many tasks. See xrefs to `CTaskComplexGangLeader::ShouldLoadGangAnims()`
//! Usual usage would be: `StreamAnimBlock(m_animBlockName, CTaskComplexGangLeader::ShouldLoadGangAnims(), m_areAnimsReferenced)`
void CAnimManager::StreamAnimBlock(const char* blck, bool shouldBeLoaded, bool& isLoaded) {
    if (shouldBeLoaded && !isLoaded) {
        RemoveAnimBlockRef(GetAnimationBlockIndex(blck));
        isLoaded = false;
    } else if (!shouldBeLoaded && isLoaded) {
        const auto blkIdx = GetAnimationBlockIndex(blck);
        if (GetAnimBlocks()[blkIdx].IsLoaded) {
            AddAnimBlockRef(blkIdx);
            isLoaded = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blkIdx), STREAMING_KEEP_IN_MEMORY);
        }
    }
}

// 0x4D5620
void CAnimManager::LoadAnimFiles() {
    // Como el original: sin comprobar el stream
    RwStream* stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\PED.IFP");
    LoadAnimFile(stream, true, nullptr);
    RwStreamClose(stream, nullptr);

    ms_aAnimAssocGroups = new CAnimBlendAssocGroup[ms_numAnimAssocDefinitions];
    CreateAnimAssocGroups();
}

struct IFPSectionHeader {
    union {
        uint32 ID;
        char   IDFourCC[4];
    };
    uint32 Size;
};

namespace {
//! Como el original: redondea el tamaño de la sección a 4 bytes
uint32 RoundSectionSize(uint32 size) {
    return (size & 3) ? size + 4 - (size & 3) : size;
}

//! Como el original: operator new escalar (n·12 + 4) con el contador delante y el constructor de cada secuencia
CAnimBlendSequence* NewAnimSequences(int32 n) {
    const auto mem = static_cast<int32*>(::operator new(n * sizeof(CAnimBlendSequence) + 4));
    if (!mem) {
        return nullptr;
    }
    *mem = n;
    const auto seqs = reinterpret_cast<CAnimBlendSequence*>(mem + 1);
    for (int32 i = 0; i < n; i++) {
        new (&seqs[i]) CAnimBlendSequence;
    }
    return seqs;
}

//! Como el original: _ftol2 (truncamiento a 64 bits) y se guarda la palabra baja
int16 FloatToKeyFrameWord(float v) {
    return (int16)(int64)v;
}
} // namespace

//! @notsa - Helper
auto CAnimManager::GetOrCreateAnimBlock(const char* name, uint32 numAnims) {
    // Como el original (inline en 0x4D47F0): bloque nuevo con strncpy de 16, o bloque existente sin animaciones
    CAnimBlock* ab = GetAnimationBlock(name);
    if (!ab) {
        ab = &ms_aAnimBlocks.data()[ms_numAnimBlocks++];
        strncpy(ab->Name, name, MAX_ANIM_BLOCK_NAME);
        ab->NumAnims     = numAnims;
        ab->FirstAnimIdx = ms_numAnimations;
        ab->GroupId      = GetFirstAssocGroup(ab->Name);
    } else if (!ab->NumAnims) {
        ab->NumAnims     = numAnims;
        ab->FirstAnimIdx = ms_numAnimations;
    }
    ab->IsLoaded = true;
    return std::make_tuple(ab, GetAnimationBlockIndex(ab));
}

// 0x4D47F0
void CAnimManager::LoadAnimFile(RwStream* stream, bool loadCompressed, char const(*uncompressedAnimations)[32]) {
    // Como el original: "ANP2"/"ANP3" con strncmp; cualquier otra cabecera se lee como ANPK
    IFPSectionHeader h;
    RwStreamRead(stream, &h, sizeof(h));
    if (!strncmp(h.IDFourCC, "ANP2", 4) || !strncmp(h.IDFourCC, "ANP3", 4)) {
        LoadAnimFile_ANP23(stream, h, loadCompressed, !strncmp(h.IDFourCC, "ANP3", 4));
    } else {
        LoadAnimFile_ANPK(stream, h, loadCompressed, uncompressedAnimations);
    }
}

// Code from 0x4D485C
inline void CAnimManager::LoadAnimFile_ANPK(RwStream* s, const IFPSectionHeader& h, bool bLoadCompressed, const char (*pLoadUncompressed)[32]) {
    // Como el original: un solo búfer para las secciones, tamaños redondeados a 4 sin límite, el bucle usa el
    // número de animaciones del bloque y los quaternions se guardan conjugados.
    IFPSectionHeader sh, dgan;
    char             buf[256];

    RwStreamRead(s, &sh, sizeof(sh)); // INFO
    sh.Size = RoundSectionSize(sh.Size);
    RwStreamRead(s, buf, sh.Size);

    const auto [ablock, ablockIdx] = GetOrCreateAnimBlock(buf + 4, *reinterpret_cast<int32*>(buf));
    int32 lastIdx = ablock->FirstAnimIdx;
    auto  hier    = &ms_aAnimations.data()[ablock->FirstAnimIdx];

    for (int32 animN = 0; animN < (int32)ablock->NumAnims; animN++, hier++) {
        RwStreamRead(s, &sh, sizeof(sh)); // NAME
        sh.Size = RoundSectionSize(sh.Size);
        RwStreamRead(s, buf, sh.Size);
        lastIdx++;
        hier->SetName(buf);

        bool isCompressed    = bLoadCompressed;
        hier->m_nAnimBlockId = ablockIdx;
        if (pLoadUncompressed && pLoadUncompressed[0][0]) {
            for (auto n = pLoadUncompressed; (*n)[0]; n++) { // Como el original: sin break
                const auto key = hier->m_hashKey;
                if (CKeyGen::GetUppercaseKey(*n) == key) {
                    isCompressed = false;
                }
            }
        }
        hier->m_bIsCompressed   = isCompressed;
        hier->m_bKeepCompressed = false;

        RwStreamRead(s, &dgan, sizeof(dgan)); // DGAN
        RwStreamRead(s, &sh, sizeof(sh));     // INFO
        sh.Size = RoundSectionSize(sh.Size);
        RwStreamRead(s, buf, sh.Size);
        const int32 numSeq = *reinterpret_cast<int32*>(buf);
        hier->m_nSeqCount  = (uint16)numSeq;
        hier->m_pSequences = NewAnimSequences(numSeq);

        for (int32 seqN = 0; seqN < numSeq; seqN++) {
            RwStreamRead(s, &sh, sizeof(sh)); // CPAN
            RwStreamRead(s, &sh, sizeof(sh)); // ANIM
            sh.Size = RoundSectionSize(sh.Size);
            RwStreamRead(s, buf, sh.Size);

            const auto seq = &hier->m_pSequences[seqN];
            seq->SetName(buf);
            if (sh.Size == 0x2C) {
                seq->SetBoneTag(*reinterpret_cast<eBoneTag32*>(buf + 0x28));
            }
            const int32 numFrames = *reinterpret_cast<int32*>(buf + 0x1C);
            if (!numFrames) {
                continue;
            }

            bool isKRTS = false, isKRT0 = false;
            RwStreamRead(s, &sh, sizeof(sh)); // KRTS / KRT0 / KR00
            if (!strncmp(sh.IDFourCC, "KRTS", 4)) {
                isKRTS = true;
                seq->SetNumFrames(numFrames, true, isCompressed, nullptr);
            } else if (!strncmp(sh.IDFourCC, "KRT0", 4)) {
                isKRT0 = true;
                seq->SetNumFrames(numFrames, true, isCompressed, nullptr);
            } else if (!strncmp(sh.IDFourCC, "KR00", 4)) {
                seq->SetNumFrames(numFrames, false, isCompressed, nullptr);
            }

            for (int32 k = 0; k < (int16)seq->m_FramesNum; k++) {
                // Quaternion (x, y, z, w), traslación, escala (ignorada) y tiempo
                float kf[11];
                float time;
                const bool hasTrans = isKRTS || isKRT0;
                if (isKRTS) {
                    RwStreamRead(s, kf, 0x2C);
                    time = kf[10];
                } else if (isKRT0) {
                    RwStreamRead(s, kf, 0x20);
                    time = kf[7];
                } else {
                    RwStreamRead(s, kf, 0x14);
                    time = kf[4];
                }
                const float qx = -kf[0], qy = -kf[1], qz = -kf[2], qw = kf[3];
                const auto  frames = static_cast<uint8*>(seq->m_Frames);
                if (isCompressed) {
                    const auto d = reinterpret_cast<int16*>(frames + (seq->m_bHasTranslation ? k * 16 : k * 10));
                    d[0] = FloatToKeyFrameWord(qx * 4096.f);
                    d[1] = FloatToKeyFrameWord(qy * 4096.f);
                    d[2] = FloatToKeyFrameWord(qz * 4096.f);
                    d[3] = FloatToKeyFrameWord(qw * 4096.f);
                    if (hasTrans) {
                        d[5] = FloatToKeyFrameWord(kf[4] * 1024.f);
                        d[6] = FloatToKeyFrameWord(kf[5] * 1024.f);
                        d[7] = FloatToKeyFrameWord(kf[6] * 1024.f);
                    }
                    d[4] = FloatToKeyFrameWord(time * 60.f + 0.5f);
                } else {
                    const auto d = reinterpret_cast<float*>(frames + (seq->m_bHasTranslation ? k * 32 : k * 20));
                    d[0] = qx;
                    d[1] = qy;
                    d[2] = qz;
                    d[3] = qw;
                    if (hasTrans) {
                        d[5] = kf[4];
                        d[6] = kf[5];
                        d[7] = kf[6];
                    }
                    d[4] = time;
                }
            }
        }

        if (!isCompressed) {
            hier->RemoveQuaternionFlips();
            hier->CalcTotalTime();
        }
    }
    if (lastIdx > ms_numAnimations) {
        ms_numAnimations = lastIdx;
    }
}

// Code from 0x4D5172
inline void CAnimManager::LoadAnimFile_ANP23(RwStream* s, const IFPSectionHeader& h, bool bLoadCompressed, bool isANP3) {
    // Como el original: un solo búfer de 24 bytes para los nombres, la compresión la decide el tipo de frame de la
    // primera secuencia (también en ANP3) y los tipos desconocidos no reservan frames.
    char  name[24];
    int32 numAnims;
    RwStreamRead(s, name, sizeof(name));
    RwStreamRead(s, &numAnims, sizeof(numAnims));

    const auto [ablock, ablockIdx] = GetOrCreateAnimBlock(name, numAnims);
    int32 lastIdx = ablock->FirstAnimIdx;
    auto  hier    = &ms_aAnimations.data()[ablock->FirstAnimIdx];

    for (int32 animN = 0; animN < numAnims; animN++, hier++) {
        lastIdx++;
        uint8* frames = nullptr;
        int32  numSeq;
        RwStreamRead(s, name, sizeof(name));
        RwStreamRead(s, &numSeq, sizeof(numSeq));
        if (isANP3) {
            int32 size, flags;
            RwStreamRead(s, &size, sizeof(size));
            RwStreamRead(s, &flags, sizeof(flags));
            hier->m_bIsCompressed = (flags & 1) != 0;
            frames = static_cast<uint8*>(CMemoryMgr::Malloc(size, 0));
        }
        hier->SetName(name);
        hier->m_nSeqCount       = (uint16)numSeq;
        hier->m_nAnimBlockId    = ablockIdx;
        hier->m_bKeepCompressed = false;
        hier->m_pSequences      = NewAnimSequences(numSeq);

        bool first = true;
        for (int32 seqN = 0; seqN < numSeq; seqN++) {
            int32 frameType, numFrames, boneTag;
            RwStreamRead(s, name, sizeof(name));
            RwStreamRead(s, &frameType, sizeof(frameType));
            RwStreamRead(s, &numFrames, sizeof(numFrames));
            RwStreamRead(s, &boneTag, sizeof(boneTag));

            const auto seq = &hier->m_pSequences[seqN];
            seq->SetName(name);
            seq->SetBoneTag(*reinterpret_cast<eBoneTag32*>(&boneTag));
            if (first) {
                first                 = false;
                hier->m_bIsCompressed = frameType != 1 && frameType != 2;
            }

            uint32 kfSize = 0;
            switch (frameType) {
            case 1: seq->SetNumFrames(numFrames, false, false, frames); kfSize = 20; break;
            case 2: seq->SetNumFrames(numFrames, true, false, frames);  kfSize = 32; break;
            case 3: seq->SetNumFrames(numFrames, false, true, frames);  kfSize = 10; break;
            case 4: seq->SetNumFrames(numFrames, true, true, frames);   kfSize = 16; break;
            }
            if (kfSize) {
                RwStreamRead(s, seq->m_Frames, numFrames * kfSize);
                if (!isANP3) {
                    continue;
                }
                frames += numFrames * kfSize;
            }
            if (isANP3) {
                seq->m_bUsingExternalMemory = true;
            }
        }

        if (!hier->m_bIsCompressed) {
            hier->RemoveQuaternionFlips();
            hier->CalcTotalTime();
        }
    }
    if (lastIdx > ms_numAnimations) {
        ms_numAnimations = lastIdx;
    }
}

AnimationId CAnimManager::GetRandomGangTalkAnim() {
    constexpr AnimationId gangTalkAnims[]{
        ANIM_ID_PRTIAL_GNGTLKA,
        ANIM_ID_PRTIAL_GNGTLKB,
        ANIM_ID_PRTIAL_GNGTLKC,
        ANIM_ID_PRTIAL_GNGTLKD,

        ANIM_ID_PRTIAL_GNGTLKE,
        ANIM_ID_PRTIAL_GNGTLKF,
        ANIM_ID_PRTIAL_GNGTLKG,
        ANIM_ID_PRTIAL_GNGTLKH,
    };
    return CGeneral::RandomChoice(gangTalkAnims);
}
