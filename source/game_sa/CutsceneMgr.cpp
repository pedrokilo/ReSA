/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCutsceneMgr
 * Reconstrucción: COMPLETA — 35/35 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 35/35 entradas.
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
#include <platform/win/WinPlatform.h>
#include <extensions/ci_string.hpp>

#include "Fx.h"
#include "CutsceneMgr.h"
#include "TaskSimpleCarSetPedOut.h"
#include "Rubbish.h"
#include <TempColModels.h>
#include <PlantMgr.h>

uint32 MAX_NUM_CUTSCENE_OBJECTS = 50;
uint32 MAX_NUM_CUTSCENE_PARTICLE_EFFECTS = 8;
uint32 MAX_NUM_CUTSCENE_ITEMS_TO_HIDE = 50;
uint32 MAX_NUM_CUTSCENE_ATTACHMENTS = 50;

//! Is the cutscene close to finishing (If this is set the camera is already fading in)
static inline auto& g_bCutSceneFinishing = StaticRef<bool>(0xBC1CF8);

// 0x5B0380
int32 CCutsceneMgr::AddCutsceneHead(CObject* object, int32 arg1) {
    return 0;
}

// 0x4D5DB0
void CCutsceneMgr::AppendToNextCutscene(const char* objectName, const char* animName) {
    // Como el original: strcpy + _strlwr sin límites, releyendo el contador global
    strcpy(ms_cAppendObjectName.data()[ms_numAppendObjectNames], objectName);
    _strlwr(ms_cAppendObjectName.data()[ms_numAppendObjectNames]);
    strcpy(ms_cAppendAnimName.data()[ms_numAppendObjectNames], animName);
    _strlwr(ms_cAppendAnimName.data()[ms_numAppendObjectNames]);
    ms_numAppendObjectNames++;
}

// 0x5B0450
void CCutsceneMgr::AttachObjectToBone(CCutsceneObject* attachment, CCutsceneObject* object, int32 boneId) {
    const auto boneIdx = RpHAnimIDGetIndex(GetAnimHierarchyFromSkinClump(reinterpret_cast<RpClump*>(object->GetRwObject())), boneId);
    attachment->m_pAttachmentObject = object;
    attachment->m_nAttachBone       = boneIdx;
}

// 0x5B0480
void CCutsceneMgr::AttachObjectToFrame(CCutsceneObject* attachment, CEntity* object, const char* frameName) {
    const auto frame = RpAnimBlendClumpFindFrame(reinterpret_cast<RpClump*>(object->GetRwObject()), frameName)->Frame;
    attachment->m_pAttachmentObject = nullptr;
    attachment->m_pAttachToFrame    = frame;
}

// 0x5B04B0
void CCutsceneMgr::AttachObjectToParent(CCutsceneObject* attachment, CEntity* object) {
    attachment->m_pAttachmentObject = nullptr;
    attachment->m_pAttachToFrame    = RpClumpGetFrame(reinterpret_cast<RpClump*>(object->GetRwObject()));
}

// 0x4D5E20
void CCutsceneMgr::BuildCutscenePlayer() {
    const auto plyr = FindPlayerPed(-1);
    CClothes::RebuildPlayerIfNeeded(plyr);
    CStreaming::RequestModel(MODEL_CSPLAY, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
    CStreaming::LoadAllRequestedModels(true);
    CClothes::RebuildCutscenePlayer(plyr, true);
}

// 0x5B0130
RpAtomic* CalculateBoundingSphereRadiusCB(RpAtomic* atomic, void* data) {
    auto& maxRadius = *(float*)data;

    // Como el original: la esfera se sincroniza (si está sucia) una vez para el radio y otra para el centro
    const float radius = RpAtomicGetBoundingSphere(atomic)->radius;
    RwV3d       center = RpAtomicGetBoundingSphere(atomic)->center;

    // Lleva el centro al espacio del clump recorriendo los frames padre (el raíz no transforma)
    for (RwFrame* frame = RpAtomicGetFrame(atomic); RwFrameGetParent(frame); frame = RwFrameGetParent(frame)) {
        RwV3dTransformPoints(&center, &center, 1, RwFrameGetMatrix(frame));
    }

    // Como el original: solo se guarda si es estrictamente mayor (NaN no actualiza)
    const float r = RwV3dLength(&center) + radius;
    if (r > maxRadius) {
        maxRadius = r;
    }

    return atomic;
}

// 0x5B01E0
void CCutsceneMgr::UpdateCutsceneObjectBoundingBox(RpClump* clump, eModelID modelId) {
    auto& cm = CTempColModels::ms_colModelCutObj.data()[modelId - MODEL_CUTOBJ01];

    // Como el original: parte del radio actual del col model, no de cero
    const float prevRadius = cm.m_boundSphere.m_fRadius;
    float       radius     = prevRadius;
    RpClumpForAllAtomics(clump, CalculateBoundingSphereRadiusCB, &radius);

    // Como el original: el radio queda limitado a max(radio previo, 20)
    const float maxRadius = prevRadius < 20.0f ? 20.0f : prevRadius;
    if (radius > maxRadius) {
        radius = maxRadius;
    }

    cm.m_boundSphere.m_fRadius = radius;
    cm.m_boundBox.m_vecMin     = CVector{ -radius, -radius, -radius };
    cm.m_boundBox.m_vecMax     = CVector{ radius, radius, radius };
}

// 0x5B02A0
CCutsceneObject* CCutsceneMgr::CreateCutsceneObject(eModelID modelId) {
    CStreaming::ImGonnaUseStreamingMemory();

    // Create col model for it (If cutscene object)
    if ((int32)modelId >= MODEL_CUTOBJ01 && (int32)modelId <= MODEL_CUTOBJ20) {
        auto& cm = CTempColModels::ms_colModelCutObj.data()[modelId - MODEL_CUTOBJ01];
        CModelInfo::GetModelInfo(modelId)->SetColModel(&cm, false);
        cm.m_boundSphere.m_fRadius = 0.0f; // Como el original: se pone a cero antes de recalcular
        // Como el original: m_pRwObject (+0x1C, protegido) del model info leído en crudo, sin comprobar el tipo
        UpdateCutsceneObjectBoundingBox(*reinterpret_cast<RpClump**>(reinterpret_cast<uint8*>(CModelInfo::GetModelInfo(modelId)) + 0x1C), modelId);
    }

    // Como el original: SetModelIndex antes de guardarlo en la lista
    const auto obj = new CCutsceneObject;
    obj->SetModelIndex(modelId);
    ms_pCutsceneObjects.data()[ms_numCutsceneObjs] = obj;
    ms_numCutsceneObjs++;

    CStreaming::IHaveUsedStreamingMemory();

    return obj;
}

// 0x4D5ED0
void CCutsceneMgr::DeleteCutsceneData() {
    DeleteCutsceneData_overlay();
    CStreaming::SetMissionDoesntRequireModel(MODEL_CSPLAY);
    if (restoreEverythingAfterCutscene) {
        LoadEverythingBecauseCutsceneDeletedAllOfIt();
    }
}

// Código común (inline en el EXE: 0x4D5C42 y 0x5B0010): recarga el modelo del arma y se la devuelve al ped
static void RestorePlayerWeapon(CPed* ped, eWeaponType type, int32 ammo) {
    const auto model1 = CWeaponInfo::GetWeaponInfo(type, eWeaponSkill::STD)->m_nModelId1;
    const auto flags  = CStreaming::ms_aInfoForModel[model1].GetFlags();
    CStreaming::RequestModel(model1, STREAMING_GAME_REQUIRED);
    CStreaming::LoadAllRequestedModels(false);
    if (CWeaponInfo::GetWeaponInfo(type, eWeaponSkill::STD)->m_nModelId2 != -1) {
        CStreaming::RequestModel(CWeaponInfo::GetWeaponInfo(type, eWeaponSkill::STD)->m_nModelId2, STREAMING_DEFAULT);
        CStreaming::LoadAllRequestedModels(false);
    }
    if (!(flags & STREAMING_GAME_REQUIRED)) {
        CStreaming::SetModelIsDeletable(model1);
    }
    ped->GiveWeapon(type, ammo, true);
}

// 0x5AFD60
void CCutsceneMgr::DeleteCutsceneData_overlay() {
    if (ms_cutsceneLoadStatus == LoadStatus::NOT_LOADED) {
        return;
    }

    // We suspend the timer here (is resumed at the end)
    CTimer::Suspend();

    // Restore multipliers
    CPopulation::PedDensityMultiplier = m_fPrevPedDensity;
    CCarCtrl::CarDensityMultiplier = m_fPrevCarDensity;

    // Restore extra colors
    if (m_PrevExtraColourOn) {
        CTimeCycle::StartExtraColour(m_PrevExtraColour, false);
    } else {
        CTimeCycle::StopExtraColour(false);
    }

    // Make hidden entities visible again (Como el original: contador con signo y releído)
    for (int32 i = 0; i < (int32)ms_iNumHiddenEntities; i++) {
        if (auto& e = ms_pHiddenEntities.data()[i]) {
            e->CleanUpOldReference(&e);
            e->SetIsVisible(true);
        }
    }
    ms_iNumHiddenEntities = 0;

    // Destroy particle effects
    for (int32 i = 0; i < (int32)ms_iNumParticleEffects; i++) {
        auto& prt = ms_pParticleEffects.data()[i];
        if (prt.m_pFxSystem) {
            g_fxMan.DestroyFxSystem(prt.m_pFxSystem);
        }
        prt.m_pFxSystem = nullptr;
    }
    ms_iNumParticleEffects = 0;

    CMessages::ClearMessages(false);
    CRubbish::SetVisibility(true);

    ms_cutsceneProcessing = false;
    ms_useLodMultiplier = false;
    ms_useCutsceneShadows = true; // Como el original

    // Delete cutscene objects (Como el original: del último al primero, bajando el contador en cada paso)
    while (ms_numCutsceneObjs != 0) {
        ms_numCutsceneObjs--;
        CWorld::Remove(ms_pCutsceneObjects.data()[ms_numCutsceneObjs]);
        ms_pCutsceneObjects.data()[ms_numCutsceneObjs]->DeleteRwObject();
        if (const auto obj = ms_pCutsceneObjects.data()[ms_numCutsceneObjs]) {
            delete obj;
        }
        ms_pCutsceneObjects.data()[ms_numCutsceneObjs] = nullptr;
    }
    ms_numCutsceneObjs = 0;

    // Unload anims
    if (ms_animLoaded) {
        CAnimManager::RemoveLastAnimFile();
    }
    ms_animLoaded = false;
    ms_cutsceneAssociations.DestroyAssociations();
    ms_aUncompressedCutsceneAnims[0][0] = 0;
    ms_numUncompressedCutsceneAnims = 0;

    if (dataFileLoaded) {
        TheCamera.RestoreWithJumpCut();
        TheCamera.SetWideScreenOff();
        TheCamera.DeleteCutSceneCamDataMemory();
    }

    CIplStore::ClearIplsNeededAtPosn();

    ms_cutsceneLoadStatus = LoadStatus::NOT_LOADED;
    ms_running = false;

    // Como el original: CWorld::Players[PlayerInFocus], y CPad::Clear tras DrasticTidyUpMemory
    const auto player = FindPlayerPed(-1);
    auto&      playerInfo = CWorld::Players[CWorld::PlayerInFocus];
    player->SetIsVisible(true);
    CPad::GetPad(0)->bPlayerSafeForCutscene = false;
    playerInfo.MakePlayerSafe(false, 10000.0f);

    if (!IsPlayingCSTheFinale()) {
        AudioEngine.StopCutsceneTrack(true);
        CAudioEngine::EnableEffectsLoading();
        CAEPedSpeechAudioEntity::EnableAllPedSpeech();
    }

    CStreaming::ms_disableStreaming = false;
    CWorld::bProcessCutsceneOnly = false;

    if (dataFileLoaded) {
        CGame::DrasticTidyUpMemory(TheCamera.CCamera::GetScreenFadeStatus() == NAME_FADE_IN);
    }

    CPad::GetPad(0)->Clear(false, false);

    // Como el original: con m_bDontClearZone se recargan los modelos iniciales y se devuelven las armas guardadas
    if (m_bDontClearZone) {
        CStreaming::LoadInitialPeds();
        CStreaming::LoadInitialWeapons();
        CStreaming::LoadInitialVehicles();
        m_bDontClearZone = false;

        const auto ped = FindPlayerPed(-1);
        for (int32 i = 0; i < numPlayerWeaponsToRestoreInZone; i++) {
            RestorePlayerWeapon(ped, (eWeaponType)playerWeaponsToRestoreInZone_Type.data()[i], playerWeaponsToRestoreInZone_Ammo.data()[i]);
        }
        numPlayerWeaponsToRestoreInZone = 0;
    }

    // Tell the streamer that we don't need any (possibly) loaded special chars
    for (int32 i = 0; i < ms_numLoadObjectNames; i++) {
        if (!CTheScripts::ScriptResourceManager.HasResourceBeenRequested(ms_iModelIndex.data()[i], RESOURCE_TYPE_MODEL_OR_SPECIAL_CHAR)) {
            CStreaming::SetMissionDoesntRequireModel(ms_iModelIndex.data()[i]);
        }
    }

    CStreaming::SetMissionDoesntRequireModel(MODEL_CSPLAY);
    CStreaming::StreamZoneModels(FindPlayerCoors(-1));

    CTimer::Resume();

    CStreaming::ForceLayerToRead(true);
}

// Constantes del EXE para FinishCutscene (0x858C54 y 0x858CDC)
static const float s_fFinishTwoPow32 = 4294967296.0f;
static const float s_fFinishMilli    = std::bit_cast<float>(0x3A83126Fu);

// 0x5B04D0
void CCutsceneMgr::FinishCutscene() {
    if (dataFileLoaded) {
        // Como el original: fild del tiempo (sin signo) y fmul por 0.001f en la x87
        const uint32 finishTime = TheCamera.GetCutSceneFinishTime();
        float timer;
        __asm {
            fild dword ptr finishTime
            mov eax, finishTime
            test eax, eax
            jge positive
            fadd dword ptr s_fFinishTwoPow32
        positive:
            fmul dword ptr s_fFinishMilli
            fstp dword ptr timer
        }
        ms_cutsceneTimerS = timer;
        TheCamera.FinishCutscene();
    }
    FindPlayerPed(-1)->SetIsVisible(true);
    CWorld::Players[CWorld::PlayerInFocus].MakePlayerSafe(false, 10000.0f);
}

// 0x5B0550
uint64 CCutsceneMgr::GetCutsceneTimeInMilleseconds() {
    // Como el original: _ftol2 (truncado a 64 bits) de segundos * 1000
    return (uint64)(int64)(ms_cutsceneTimerS * 1000.0f);
}

// 0x5B0570
bool CCutsceneMgr::HasCutsceneFinished() {
    return !dataFileLoaded || TheCamera.GetPositionAlongSpline() == 1.0f;
}

// 0x5AFAD0
void CCutsceneMgr::HideRequestedObjects() {
    // Como el original: el contador se pone a cero y se rellena de nuevo con las entidades ocultadas
    const auto numToHide = (int32)ms_iNumHiddenEntities;
    ms_iNumHiddenEntities = 0;
    if (numToHide <= 0) {
        return;
    }

    for (int32 i = 0; i < numToHide; i++) {
        const auto& cr = ms_crToHideItems.data()[i];

        int32 modelId;
        if (!CModelInfo::GetModelInfo(cr.m_szObjectName, &modelId)) {
            continue;
        }

        const CVector pos = cr.m_vecPosn;
        int16    nObjInRng;
        CEntity* objInRng[32];
        CWorld::FindObjectsOfTypeInRange(modelId, pos, 1.5f, true, &nObjInRng, 32, objInRng, true, false, false, true, true);
        for (int32 k = 0; k < nObjInRng; k++) {
            const auto e = objInRng[k];
            if (!e->GetIsVisible()) {
                continue;
            }
            auto& slot = ms_pHiddenEntities.data()[ms_iNumHiddenEntities];
            slot = e;
            e->RegisterReference(&slot);
            slot->SetIsVisible(false);
            ms_iNumHiddenEntities++;
        }
    }
}

// 0x4D5A20
void CCutsceneMgr::Initialise() {
    ZoneScoped;

    ms_cutsceneLoadStatus = LoadStatus::NOT_LOADED;
    ms_running            = false;
    ms_animLoaded         = false;
    ms_cutsceneProcessing = false;
    ms_useLodMultiplier   = false;
    ms_wasCutsceneSkipped = false;
    ms_hasFileInfo        = false;
    ms_pCutsceneDir       = new CDirectory(512);
}

// 0x4D5D10
bool CCutsceneMgr::IsCutsceneSkipButtonBeingPressed() {
    const auto pad = CPad::GetPad(0);
    return pad->IsCrossPressed()
        || CPad::IsMouseLButtonPressed()
        || CPad::IsEnterJustPressed()
        || pad->IsStandardKeyJustPressed(' ')                  // Como el original: tecla nueva y no la anterior (0x4D5D80)
        || reinterpret_cast<const int32&>(isForeground) == 0;  // Como el original: 0x746070 lee el dword
}

// 0x4D5AB0
void CCutsceneMgr::LoadAnimationUncompressed(const char* animName) {
    // Como el original: strcpy sin límite y se anula el siguiente nombre (aunque sea el índice 8)
    strcpy(ms_aUncompressedCutsceneAnims.data()[ms_numUncompressedCutsceneAnims], animName);
    ms_numUncompressedCutsceneAnims++;
    ms_aUncompressedCutsceneAnims.data()[ms_numUncompressedCutsceneAnims][0] = 0;
}

// 0x4D5E80
void CCutsceneMgr::LoadCutsceneData(const char* cutsceneName) {
    const auto plyr = FindPlayerPed(-1);

    CClothes::RebuildPlayerIfNeeded(plyr);

    CStreaming::RequestModel(MODEL_CSPLAY, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
    CStreaming::LoadAllRequestedModels(true);

    CClothes::RebuildCutscenePlayer(plyr, MODEL_CSPLAY);

    if (!ms_pCutsceneDir->HasLoaded()) {
        ms_pCutsceneDir->ReadDirFile("ANIM\\CUTS.IMG");
    }

    LoadCutsceneData_overlay(cutsceneName);
}

// 0x5B11C0
void CCutsceneMgr::LoadCutsceneData_loading() {
    // Como el original: solo se espera a los modelos especiales de cinemática (300-319)
    for (int32 i = 0; i < ms_numLoadObjectNames; i++) {
        const auto model = (int32)ms_iModelIndex.data()[i];
        if (model >= MODEL_CUTOBJ01 && model <= MODEL_CUTOBJ20 && CStreaming::ms_aInfoForModel[model].m_LoadState != LOADSTATE_LOADED) {
            return;
        }
    }

    // All models are streamed in now
    LoadCutsceneData_postload();

    // Load custcene anims and create objects
    CCutsceneObject* obj = nullptr; // NOTE: en el EXE es una variable de pila sin inicializar (si el primer objeto se repite)
    for (int32 i = 0; i < ms_numLoadObjectNames; i++) {
        if (!ms_bRepeatObject.data()[i]) {
            obj = CreateCutsceneObject(ms_iModelIndex.data()[i]);
        }
        if (const auto& animName = ms_cLoadAnimName.data()[i]; animName[0]) {
            SetCutsceneAnim(animName, obj);
        }
    }

    // Create FXs that are attached to objects
    for (int32 k = 0; k < (int32)ms_iNumParticleEffects; k++) {
        auto& csfx = ms_pParticleEffects.data()[k];

        RwMatrix fxTransform;
        CVector  pos = csfx.m_vecPosn, dir = csfx.m_vecDirection;
        g_fx.CreateMatFromVec(&fxTransform, &pos, &dir);

        RwMatrix* objMat = nullptr;
        // The effect's object id is 1-based (0 means "no object")
        if (const auto objIdx = csfx.m_nObjectId; objIdx > 0 && objIdx < ms_numCutsceneObjs + 1) {
            const auto clump = reinterpret_cast<RpClump*>(ms_pCutsceneObjects.data()[objIdx - 1]->GetRwObject());
            const auto atomic = GetFirstAtomic(clump);
            if (atomic && RpSkinGeometryGetSkin(RpAtomicGetGeometry(atomic))) {
                // Skinned: la parte es el ID del hueso (Como el original: atoi)
                const auto boneId = atoi(csfx.m_szObjectPart);
                const auto hier   = GetAnimHierarchyFromSkinClump(clump);
                const auto idx    = RpHAnimIDGetIndex(hier, boneId);
                objMat = &RpHAnimHierarchyGetMatrixArray(hier)[idx];
            } else if (const auto frame = CClumpModelInfo::GetFrameFromName(clump, csfx.m_szObjectPart)) {
                objMat = RwFrameGetLTM(frame); // Como el original: la LTM del frame
            }
        }

        csfx.m_pFxSystem = g_fxMan.CreateFxSystem(csfx.m_szEffectName, fxTransform, objMat, true);
    }

    // Finally, process attachments (Como el original: se adjunta el objeto `attachment` al ped)
    for (int32 k = 0; k < ms_numAttachObjectToBones; k++) {
        const auto& v = ms_iAttachObjectToBone.data()[k];
        AttachObjectToBone(ms_pCutsceneObjects.data()[v.m_nCutsceneAttachmentObjectId], ms_pCutsceneObjects.data()[v.m_nCutscenePedObjectId], v.m_nBoneId);
    }
}

// 0x5B13F0
void CCutsceneMgr::LoadCutsceneData_overlay(const char* cutsceneName) {
    CTimer::Suspend();

    ms_cutsceneProcessing = true;
    ms_wasCutsceneSkipped = false;

    if (!m_bDontClearZone) {
        CStreaming::RemoveCurrentZonesModels();
    }

    ms_pCutsceneDir->Clear();
    ms_pCutsceneDir->ReadDirFile("ANIM\\CUTS.IMG");

    CStreaming::RemoveUnusedModelsInLoadedList();
    CGame::DrasticTidyUpMemory(true);
    strcpy(ms_cutsceneName, cutsceneName); // Como el original: strcpy sin límite
    LoadCutsceneData_preload();

    CTimer::Resume();
}

// 0x5AFBC0
void CCutsceneMgr::LoadCutsceneData_postload() {
    CMessages::ClearThisPrintBigNow(STYLE_MIDDLE);

    CPopulation::PedDensityMultiplier = 0.0f;
    CCarCtrl::CarDensityMultiplier = 0.0f;
    CStreaming::ms_disableStreaming = false;

    // Load animations for this cutscene (Como el original: el nombre se forma en gString)
    {
        const auto stream = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\CUTS.IMG");
        sprintf(gString, "%s.IFP", ms_cutsceneName);

        uint32      size;
        CdStreamPos pos;
        if (ms_pCutsceneDir->FindItem(gString, pos, size)) {
            CStreaming::MakeSpaceFor(size << 10); // Not sure why it's only half, but okay

            CStreaming::ImGonnaUseStreamingMemory();

            // Load ifp file
            RwStreamSkip(stream, pos.ToInt() << 11);
            CAnimManager::LoadAnimFile(stream, true, ms_aUncompressedCutsceneAnims.data());

            // Now create the anims in memory
            ms_cutsceneAssociations.CreateAssociations(ms_cutsceneName, ms_cLoadAnimName[0], ms_cLoadObjectName[0], 32);

            CStreaming::IHaveUsedStreamingMemory();
            ms_animLoaded = true;
        } else {
            ms_animLoaded = false;
        }

        RwStreamClose(stream, nullptr);
    }

    // Load camera path splines for this cutscene
    {
        const auto img = CFileMgr::OpenFile("ANIM\\CUTS.IMG", "rb");
        sprintf(gString, "%s.DAT", ms_cutsceneName);

        uint32      size;
        CdStreamPos pos;
        if (ms_pCutsceneDir->FindItem(gString, pos, size)) {
            CStreaming::ImGonnaUseStreamingMemory();

            CFileMgr::Seek(img, pos.ToInt() << 11, SEEK_SET);
            TheCamera.LoadPathSplines(img);

            CStreaming::IHaveUsedStreamingMemory();
            dataFileLoaded = true;
        } else {
            dataFileLoaded = false;
        }

        CFileMgr::CloseFile(img);
    }

    ms_cutsceneLoadStatus = LoadStatus::LOADED;

    ms_cutsceneTimerS = 0.f;

    // Como el original: wanted del ped (nulo si no tiene datos de jugador)
    const auto ped = FindPlayerPed(-1);
    const auto wanted = ped->GetPlayerData() ? ped->GetPlayerData()->m_pWanted : nullptr;
    wanted->ClearQdCrimes();
}

// 0x5B05A0
void CCutsceneMgr::LoadCutsceneData_preload() {
    // Preload cutscene track (Except for the "finale" cutscene)
    if (!IsPlayingCSTheFinale()) {
        if (const auto trkId = FindCutsceneAudioTrackId(ms_cutsceneName); trkId != -1) {
            AudioEngine.PreloadCutsceneTrack(trkId, true);
        }
    }

    // Unload cutscene models (Como el original: 300-319 incluidos, refcount con signo, la clave se cambia siempre)
    for (int32 i = MODEL_CUTOBJ01; i <= MODEL_CUTOBJ20; i++) {
        if ((int16)CModelInfo::GetModelInfo(i)->m_nRefCount > 0) {
            continue;
        }
        if (CStreaming::ms_aInfoForModel[i].m_LoadState == LOADSTATE_LOADED) {
            CStreaming::RemoveModel(i);
        }
        CModelInfo::GetModelInfo(i)->m_nKey = CKeyGen::GetUppercaseKey("&*%");
    }

    // Save states
    m_PrevExtraColour   = CTimeCycle::m_ExtraColour;
    m_PrevExtraColourOn = CTimeCycle::m_bExtraColourOn != 0;

    m_fPrevCarDensity   = CCarCtrl::CarDensityMultiplier;
    m_fPrevPedDensity   = CPopulation::PedDensityMultiplier;

    // Reset internal states
    ms_cutsceneOffset               = CVector{ 0.0f, 0.0f, 0.0f };
    ms_numTextOutput                = 0;
    ms_currTextOutput               = 0;
    ms_numLoadObjectNames           = 0;
    ms_numUncompressedCutsceneAnims = 0;
    ms_numAttachObjectToBones       = 0;
    ms_iNumHiddenEntities           = 0;
    ms_iNumParticleEffects          = 0;
    g_bCutSceneFinishing            = false;

    for (auto&& name : ms_aUncompressedCutsceneAnims) { // Clear all names
        name[0] = '\0';
    }

    rng::fill(ms_iModelIndex, MODEL_PLAYER);

    TheCamera.SetNearClipScript(0.1f);

    CRubbish::SetVisibility(false);

    // Como el original: una sola llamada a FindPlayerPed; el wanted sale de m_pPlayerData
    const auto player = FindPlayerPed(-1);
    auto&      playerInfo = CWorld::Players[CWorld::PlayerInFocus];
    const auto wanted = player->GetPlayerData() ? player->GetPlayerData()->m_pWanted : nullptr;
    wanted->ClearQdCrimes();
    player->SetIsVisible(false);
    player->ResetSprintEnergy();
    CPad::GetPad(0)->bPlayerSafeForCutscene = true;
    playerInfo.MakePlayerSafe(true, 10000.0f);

    // Load cutscene data file from `CUTS.IMG` (Como el original: el nombre se forma en gString)
    sprintf(gString, "%s.CUT", ms_cutsceneName);
    uint32      csFileSize;
    CdStreamPos csFilePos;
    if (!ms_pCutsceneDir->FindItem(gString, csFilePos, csFileSize)) {
        LoadCutsceneData_postload();
        return;
    }
    const uint32 csFileOffset = csFilePos.ToInt() << 11;
    csFileSize <<= 11;

    const auto csFileData = new char[csFileSize];
    {
        const auto s = RwStreamOpen(rwSTREAMFILENAME, rwSTREAMREAD, "ANIM\\CUTS.IMG");
        RwStreamSkip(s, csFileOffset);
        RwStreamRead(s, csFileData, csFileSize);
        RwStreamClose(s, nullptr);
    }

    // 0x5B0811 - Como el original: lector de líneas propio (\n y \r separan; el texto termina en '\0'),
    // secciones por igualdad exacta, sscanf/strtok/atoi/atof con comas como en el EXE.
    enum Section {
        NONE,
        INFO,
        MODEL,
        TEXT,
        UNCOMPRESS,
        ATTACH,
        REMOVE,
        PEFFECT,
        EXTRACOL,
    };

    char* cursor            = csFileData;
    auto  curSec            = NONE;
    bool  hasSetExtraColors = false;
    char  line[1024];
    for (;;) {
        // Read the next line
        while (*cursor == '\n' || *cursor == '\r') {
            cursor++;
        }
        if (!*cursor) {
            break;
        }
        {
            char* out = line;
            for (char c = *cursor; c != '\n'; c = *++cursor) {
                if (c == '\r' || c == '\0') {
                    break;
                }
                *out++ = c;
            }
            *out = '\0';
        }

        if (strcmp(line, "end") == 0) { // End of section
            curSec = NONE;
            continue;
        }

        switch (curSec) {
        case NONE: {
            if (strcmp(line, "info") == 0)       curSec = INFO;
            if (strcmp(line, "model") == 0)      curSec = MODEL;
            if (strcmp(line, "text") == 0)       curSec = TEXT;
            if (strcmp(line, "uncompress") == 0) curSec = UNCOMPRESS;
            if (strcmp(line, "attach") == 0)     curSec = ATTACH;
            if (strcmp(line, "remove") == 0)     curSec = REMOVE;
            if (strcmp(line, "peffect") == 0)    curSec = PEFFECT;
            if (strcmp(line, "extracol") == 0)   curSec = EXTRACOL;
            break;
        }
        case INFO: { // 0x5B09E4
            if (strncmp(line, "offset", 6) != 0) {
                break;
            }
            float x, y, z;
            sscanf(line + 7, "%f %f %f", &x, &y, &z);

            // Warp ped out of the vehicle
            if (const auto plyr = FindPlayerPed(-1); plyr->bInVehicle && plyr->m_pVehicle) {
                CTaskSimpleCarSetPedOut{ plyr->m_pVehicle, TARGET_DOOR_FRONT_LEFT, true, true }.ProcessPed(plyr);
            }

            ms_cutsceneOffset = CVector{ x, y, z };

            // Load IPLs
            CIplStore::AddIplsNeededAtPosn(ms_cutsceneOffset);

            // Load the scene now
            CStreaming::SetLoadVehiclesInLoadScene(false);
            CStreaming::LoadScene(CVector{ x, y, z });
            CStreaming::SetLoadVehiclesInLoadScene(true);
            break;
        }
        case MODEL: { // 0x5B0B04
            int32 unusedId;
            sscanf(strtok(line, " ,"), "%d", &unusedId); // first value is unused
            char modelName[256];
            strcpy(modelName, strtok(nullptr, " ,"));
            auto animTok = strtok(nullptr, " ,");
            _strlwr(modelName);

            for (bool first = true; animTok; animTok = strtok(nullptr, " ,")) {
                char animName[256];
                animName[0] = 0;
                strcpy(animName, animTok);
                _strlwr(animName);

                const auto n = ms_numLoadObjectNames;
                strcpy(ms_cLoadObjectName.data()[n], modelName);
                strcpy(ms_cLoadAnimName.data()[n], animName);
                if (first) {
                    first = false;
                    ms_iModelIndex.data()[n]   = MODEL_LOAD_THIS;
                    ms_bRepeatObject.data()[n] = false;
                } else {
                    ms_bRepeatObject.data()[n] = true;
                    ms_iModelIndex.data()[n]   = MODEL_USE_PREV;
                }
                ms_numLoadObjectNames = n + 1;
            }
            break;
        }
        case TEXT: { // 0x5B0C46
            int32 startTime, duration;
            char  gxtEntry[256];
            sscanf(line, "%d,%d,%s", &startTime, &duration, gxtEntry);

            // Como el original: mayúsculas ASCII con índice de 8 bits
            for (uint8 c = 0; c < strlen(gxtEntry); c++) {
                if (gxtEntry[c] >= 'a' && gxtEntry[c] <= 'z') {
                    gxtEntry[c] -= 0x20;
                }
            }

            const auto n = ms_numTextOutput;
            strcpy(ms_cTextOutput.data()[n], gxtEntry);
            ms_iTextStartTime.data()[n] = startTime;
            ms_iTextDuration.data()[n]  = duration;
            ms_numTextOutput = n + 1;
            break;
        }
        case UNCOMPRESS: { // 0x5B0CFC
            LoadAnimationUncompressed(strtok(line, " ,"));
            break;
        }
        case ATTACH: { // 0x5B0D1C
            int32 pedId, objId, boneId;
            sscanf(line, "%d,%d,%d", &pedId, &objId, &boneId);
            auto& a = ms_iAttachObjectToBone.data()[ms_numAttachObjectToBones];
            a.m_nCutscenePedObjectId        = pedId;
            a.m_nCutsceneAttachmentObjectId = objId;
            a.m_nBoneId                     = boneId;
            ms_numAttachObjectToBones++;
            break;
        }
        case REMOVE: { // 0x5B0D7F
            float x, y, z;
            char  name[256];
            sscanf(line, "%f,%f,%f,%s", &x, &y, &z, name);
            auto& cr = ms_crToHideItems.data()[ms_iNumHiddenEntities];
            cr.m_vecPosn = CVector{ x, y, z };
            strcpy(cr.m_szObjectName, name);
            ms_iNumHiddenEntities++;
            break;
        }
        case PEFFECT: { // 0x5B0E08
            char effectName[32]{}, objectPart[32]{};
            strncpy(effectName, strtok(line, ","), 31);
            const auto startTime = atoi(strtok(nullptr, ","));
            const auto endTime   = atoi(strtok(nullptr, ","));
            const auto objectId  = atoi(strtok(nullptr, ","));
            strncpy(objectPart, strtok(nullptr, ","), 31);
            const auto px = (float)atof(strtok(nullptr, ","));
            const auto py = (float)atof(strtok(nullptr, ","));
            const auto pz = (float)atof(strtok(nullptr, ","));
            const auto dx = (float)atof(strtok(nullptr, ","));
            const auto dy = (float)atof(strtok(nullptr, ","));
            const auto dz = (float)atof(strtok(nullptr, ","));

            auto& csfx = ms_pParticleEffects.data()[ms_iNumParticleEffects];
            csfx.m_pFxSystem  = nullptr;
            csfx.m_nStartTime = startTime;
            csfx.m_nEndTime   = endTime;
            csfx.m_nObjectId  = objectId;
            strcpy(csfx.m_szObjectPart, objectPart);
            csfx.m_vecPosn      = CVector{ px, py, pz };
            csfx.m_vecDirection = CVector{ dx, dy, dz };
            csfx.m_bPlaying     = false;
            csfx.m_bStopped     = false;
            strcpy(csfx.m_szEffectName, effectName);
            ms_iNumParticleEffects++;
            break;
        }
        case EXTRACOL: { // 0x5B0998
            if (hasSetExtraColors) {
                break;
            }
            int32 extraColIdx;
            sscanf(line, "%d", &extraColIdx);
            if (extraColIdx) {
                CTimeCycle::StartExtraColour(extraColIdx - 1, false);
                hasSetExtraColors = true;
            } else {
                hasSetExtraColors = false;
            }
            break;
        }
        }
    }

    delete[] csFileData;

    if (!hasSetExtraColors) {
        CTimeCycle::StopExtraColour(false);
    }

    // 0x5B100A - Append objects added by script to the ones added by the .CUT file (Como el original: origen indexado desde 0)
    if (const auto numAppend = ms_numAppendObjectNames; numAppend > 0) {
        const auto first = ms_numLoadObjectNames;
        for (int32 k = 0; k < numAppend; k++) {
            ms_iModelIndex.data()[first + k] = MODEL_LOAD_THIS;
        }
        ms_numLoadObjectNames = first + numAppend;
        for (int32 k = 0; k < numAppend; k++) {
            strcpy(ms_cLoadObjectName.data()[first + k], ms_cAppendObjectName.data()[k]);
            strcpy(ms_cLoadAnimName.data()[first + k], ms_cAppendAnimName.data()[k]);
        }
    }
    ms_numAppendObjectNames = 0;

    const auto numLoadObjects = ms_numLoadObjectNames;
    if (numLoadObjects == 0) {
        LoadCutsceneData_postload();
        return;
    }

    // Request all models to be loaded [Added by the .CUT file and the scripts]
    if (numLoadObjects > 0) {
        int32 specialModelOffset = 0;
        for (int32 i = 0; i < ms_numLoadObjectNames; i++) {
            const auto& modelName = ms_cLoadObjectName.data()[i];

            if (_stricmp(modelName, "csplay") == 0) {
                ms_iModelIndex.data()[i] = MODEL_CSPLAY;
                continue;
            }

            switch (ms_iModelIndex.data()[i]) {
            case MODEL_LOAD_THIS: { // 0x5B10EA - Load a new model
                int32 modelId;
                if (CModelInfo::GetModelInfo(modelName, &modelId)) { // Regular model
                    ms_iModelIndex.data()[i] = (eModelID)modelId;
                    CStreaming::RequestModel(modelId, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
                } else { // Special model
                    const auto specialId = MODEL_CUTOBJ01 + specialModelOffset;
                    ms_iModelIndex.data()[i] = (eModelID)specialId;
                    CStreaming::RequestSpecialModel(specialId, modelName, STREAMING_PRIORITY_REQUEST | STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);

                    // Find next not-yet loaded cutscene model (Como el original: sin límite superior)
                    specialModelOffset++;
                    while (CStreaming::ms_aInfoForModel[MODEL_CUTOBJ01 + specialModelOffset].m_LoadState == LOADSTATE_LOADED) {
                        specialModelOffset++;
                    }
                }
                break;
            }
            case MODEL_USE_PREV: { // Just use the previous model
                ms_iModelIndex.data()[i] = ms_iModelIndex.data()[i - 1];
                break;
            }
            }
        }
    }
    CStreaming::LoadAllRequestedModels(true);
    ms_cutsceneLoadStatus = LoadStatus::LOADING;
}

// 0x4D5C10
void CCutsceneMgr::LoadEverythingBecauseCutsceneDeletedAllOfIt() {
    restoreEverythingAfterCutscene = false;
    CStreaming::LoadInitialPeds();
    CStreaming::LoadInitialWeapons();

    // Como el original: devuelve las armas guardadas por RemoveEverythingBecauseCutsceneDoesntFitInMemory
    const auto ped = FindPlayerPed(-1);
    for (int32 i = 0; i < numPlayerWeaponsToRestore; i++) {
        RestorePlayerWeapon(ped, (eWeaponType)playerWeaponsToRestore_Type.data()[i], playerWeaponsToRestore_Ammo.data()[i]);
    }
    numPlayerWeaponsToRestore = 0;
}

// 0x4D5E50
void CCutsceneMgr::RemoveCutscenePlayer() {
    CStreaming::SetMissionDoesntRequireModel(MODEL_CSPLAY);
}

// 0x4D5AF0
void CCutsceneMgr::RemoveEverythingBecauseCutsceneDoesntFitInMemory() {
    if (restoreEverythingAfterCutscene) {
        CGame::DrasticTidyUpMemory(true);
        return;
    }

    CWorld::ClearExcitingStuffFromArea(FindPlayerCoors(-1), 120.0f, true);
    CColStore::RemoveAllCollision();
    CWorld::bProcessCutsceneOnly = true;
    CStreaming::RemoveCurrentZonesModels();
    while (CStreaming::RemoveLoadedVehicle()) {
        ;
    }
    CRadar::RemoveRadarSections();

    for (const auto model : { 7, 346, 334, 345 }) {
        CStreaming::SetModelIsDeletable(model);
        CStreaming::SetModelTxdIsDeletable(model);
    }

    // Guarda las armas del jugador y se las quita
    const auto ped = FindPlayerPed(-1);
    ped->RemoveWeaponAnims(0, -1000.0f);
    numPlayerWeaponsToRestore = 0;
    int32 num = 0;
    for (auto& weapon : ped->m_aWeapons) {
        if (weapon.m_Type != WEAPON_UNARMED) {
            playerWeaponsToRestore_Type[num] = (int32)weapon.m_Type;
            playerWeaponsToRestore_Ammo[num] = (int32)weapon.m_TotalAmmo;
            num++;
        }
    }
    numPlayerWeaponsToRestore = num;
    ped->ClearWeapons();

    restoreEverythingAfterCutscene = true;
    CGame::DrasticTidyUpMemory(true);
}

// 0x5B0390
void CCutsceneMgr::SetCutsceneAnim(const char* animName, CObject* object) {
    const auto clump = reinterpret_cast<RpClump*>(object->GetRwObject()); // Como el original: se lee antes de buscar la animación

    const auto theAnim = ms_cutsceneAssociations.GetAnimation(animName);
    if (!theAnim) {
        return;
    }

    if (theAnim->m_BlendHier->m_bIsCompressed) {
        theAnim->m_BlendHier->m_bKeepCompressed = true;
    }

    CStreaming::ImGonnaUseStreamingMemory();
    const auto cpyOfTheAnim = ms_cutsceneAssociations.CopyAnimation(animName);
    CStreaming::IHaveUsedStreamingMemory();

    // Como el original: SetCurrentTime(0) (no Start); después se quita IS_PLAYING y se pone CAN_EXTRACT_VELOCITY
    cpyOfTheAnim->SetCurrentTime(0.0f);
    cpyOfTheAnim->m_Flags = (cpyOfTheAnim->m_Flags & 0xFFFE) | ANIMATION_CAN_EXTRACT_VELOCITY;

    const auto blendData = RpAnimBlendClumpGetData(clump);
    blendData->m_AnimList.Prepend(&cpyOfTheAnim->m_Link);

    if (cpyOfTheAnim->m_BlendHier->m_bKeepCompressed) {
        blendData->m_FrameDatas[0].IsCompressed = true;
    }
}

// 0x5B0420
void CCutsceneMgr::SetCutsceneAnimToLoop(const char* animName) {
    ms_cutsceneAssociations.GetAnimation(animName)->m_Flags |= ANIMATION_IS_LOOPED;
}

// 0x5B0440
void CCutsceneMgr::SetHeadAnim(const char* animName, CObject* headObject) {
    // NOP
}

// 0x5B14D0
void CCutsceneMgr::SetupCutsceneToStart() {
    // Calculate cutscene object world positions
    ms_cutsceneOffset.z += 1.0f;
    for (auto i = ms_numCutsceneObjs; i != 0;) {
        const auto anim = RpAnimBlendClumpGetFirstAssociation(reinterpret_cast<RpClump*>(ms_pCutsceneObjects.data()[i - 1]->GetRwObject()));
        i--;
        const auto csobj = ms_pCutsceneObjects.data()[i];

        if (anim) {
            if (csobj->m_pAttachToFrame) {
                anim->SetFlag(ANIMATION_CAN_EXTRACT_VELOCITY, false);
                anim->SetFlag(ANIMATION_IS_PLAYING, true);
            } else {
                // Como el original: traslación del primer keyframe de la primera secuencia, leída en crudo
                const auto hier   = anim->m_BlendHier;
                const auto frames = static_cast<const uint8*>(hier->m_pSequences[0].m_Frames);
                float tx, ty, tz;
                if (!hier->m_bIsCompressed) {
                    tx = *reinterpret_cast<const float*>(frames + 0x14);
                    ty = *reinterpret_cast<const float*>(frames + 0x18);
                    tz = *reinterpret_cast<const float*>(frames + 0x1C);
                } else {
                    constexpr float scale = 1.0f / 1024.0f; // 0x85C6D4
                    tx = (float)*reinterpret_cast<const int16*>(frames + 0xA) * scale;
                    ty = (float)*reinterpret_cast<const int16*>(frames + 0xC) * scale;
                    tz = (float)*reinterpret_cast<const int16*>(frames + 0xE) * scale;
                }
                const CVector pos{ ms_cutsceneOffset.x + tx, ty + ms_cutsceneOffset.y, tz + ms_cutsceneOffset.z };
                csobj->SetPosn(pos);
                csobj->m_vWorldPosition = pos;

                // Start the anim
                anim->SetFlag(ANIMATION_IS_PLAYING, true);
            }
        } else { // Object has no animation applied
            csobj->SetPosn(ms_cutsceneOffset);
            csobj->m_vWorldPosition = ms_cutsceneOffset;
        }

        // Add it to the world and update skinning
        CWorld::Add(csobj);
        if (RwObjectGetType(csobj->GetRwObject()) == rpCLUMP) {
            csobj->UpdateRpHAnim();
        }
    }

    CTimer::Update();
    CTimer::Update();

    ms_cutsceneTimerS = 0.f;
    ms_running       = true;
}

// 0x4D5E60
void CCutsceneMgr::Shutdown() {
    delete ms_pCutsceneDir;
}

// 0x5B1700
void CCutsceneMgr::SkipCutscene() {
    CHud::m_BigMessage[STYLE_BOTTOM_RIGHT][0] = 0; // todo: add CHud::ClearBigMessage
    ms_wasCutsceneSkipped = true;
    FinishCutscene();
}

// 0x5B1460
void CCutsceneMgr::StartCutscene() {
    CCutsceneMgr::ms_cutscenePlayStatus = PlayStatus::STARTING;
    if (dataFileLoaded) {
        TheCamera.SetCamCutSceneOffSet(ms_cutsceneOffset);
        TheCamera.TakeControlWithSpline(eSwitchType::JUMPCUT);
        TheCamera.SetWideScreenOn();

        CHud::SetHelpMessage(nullptr, true, false, false);
        CPlantMgr::PreUpdateOnceForNewCameraPos(TheCamera.GetPosition());
    }
}

// 0x4D5D00
void CCutsceneMgr::Update() {
    ZoneScoped;

    if (ms_cutsceneLoadStatus != LoadStatus::NOT_LOADED) {
        Update_overlay();
    }
}

// 0x5B1720
void CCutsceneMgr::Update_overlay() {
    CIplStore::AddIplsNeededAtPosn(ms_cutsceneOffset);
    switch (ms_cutsceneLoadStatus) {
    case LoadStatus::LOADING: {
        CTimer::Suspend();
        LoadCutsceneData_loading();
        CTimer::Resume();
        return;
    }
    case LoadStatus::LOADED:
        break;
    default:
        return; // Como el original: cualquier otro estado no hace nada
    }

    // Como el original: STARTING..S3 avanzan uno (tras las llamadas), S4 vuelve a S0, el resto no cambia
    switch (ms_cutscenePlayStatus) {
    case PlayStatus::STARTING:
        SetupCutsceneToStart();
        HideRequestedObjects();
        if (!IsPlayingCSTheFinale()) {
            CAudioEngine::DisableEffectsLoading();
            CAEPedSpeechAudioEntity::DisableAllPedSpeech();
            CAudioEngine::PlayPreloadedCutsceneTrack();
        }
        [[fallthrough]];
    case PlayStatus::S2:
    case PlayStatus::S3:
        ms_cutscenePlayStatus = (PlayStatus)((int32)ms_cutscenePlayStatus + 1);
        break;
    case PlayStatus::S4:
        ms_cutscenePlayStatus = PlayStatus::S0;
        break;
    default:
        break;
    }

    if (!ms_running) {
        return;
    }

    // Como el original: paso no recortado * 0.02f y _ftol2 (64 bits) de ms; las comparaciones son sin signo
    ms_cutsceneTimerS += CTimer::ms_fTimeStepNonClipped * 0.02f;
    const auto csTimeMS = (uint32)(int64)(ms_cutsceneTimerS * 1000.0f);

    // Deal with fx [Start and stop them]
    for (int32 k = 0; k < (int32)ms_iNumParticleEffects; k++) {
        auto& csfx = ms_pParticleEffects.data()[k];
        if (csfx.m_bPlaying && csfx.m_nEndTime != -1 && csfx.m_pFxSystem && csTimeMS > (uint32)csfx.m_nStartTime + (uint32)csfx.m_nEndTime) {
            csfx.m_pFxSystem->Stop();
            csfx.m_bPlaying = false;
            csfx.m_bStopped = true;
        }
        if (csfx.m_pFxSystem && !csfx.m_bPlaying && csTimeMS >= (uint32)csfx.m_nStartTime && !csfx.m_bStopped) {
            csfx.m_pFxSystem->Play();
            csfx.m_bPlaying = true;
        }
    }

    // Deal with text upcoming texts
    if (ms_currTextOutput < ms_numTextOutput && csTimeMS > (uint32)ms_iTextStartTime.data()[ms_currTextOutput]) {
        CMessages::AddMessageJump(
            TheText.Get(ms_cTextOutput.data()[ms_currTextOutput]),
            ms_iTextDuration.data()[ms_currTextOutput],
            1,
            true
        );
        ms_currTextOutput++;
    }

    // Update cutscene specific model bounding boxes
    for (int32 i = 0; i < ms_numCutsceneObjs; i++) {
        const auto csobj   = ms_pCutsceneObjects.data()[i];
        const auto modelId = (int32)(int16)csobj->m_nModelIndex;
        if (modelId >= MODEL_CUTOBJ01 && modelId <= MODEL_CUTOBJ20) {
            UpdateCutsceneObjectBoundingBox(reinterpret_cast<RpClump*>(csobj->GetRwObject()), (eModelID)modelId);
        }
    }

    // Deal with cutscene skip user input + fading at the end
    if (dataFileLoaded && !IsPlayingCSTheFinale()) {
        if (TheCamera.GetActiveCam().m_nMode == MODE_FLYBY && ms_cutsceneLoadStatus == LoadStatus::LOADED) {
            if (csTimeMS + 1000 > TheCamera.GetCutSceneFinishTime() && !g_bCutSceneFinishing) {
                g_bCutSceneFinishing = true;
                TheCamera.Fade(1.0f, eFadeFlag::FADE_IN);
            }
            if (IsCutsceneSkipButtonBeingPressed()) {
                SkipCutscene();
            }
        }
    }
}

bool CCutsceneMgr::IsPlayingCSTheFinale() {
    return _stricmp(ms_cutsceneName, "finale") == 0; // Como el original: _stricmp
}

// 0x5AFA50
int16 FindCutsceneAudioTrackId(const char* cutsceneName) {
    // from 0x8D0AA8 
    constexpr struct {
        notsa::ci_string_view name;
        int32 id;
    } mapping[]{
        { "BCESAR2", 626 }, { "BCESAR4", 627 }, { "BCESA4W", 628 }, { "BCESAR5", 629 }, { "BCESA5W", 630 }, { "BCRAS1", 631 }, { "BCRAS2", 632 }, { "BHILL1", 633 }, { "BHILL2", 634 }, { "BHILL3a", 635 }, { "BHILL3b", 636 }, { "BHILL3c", 637 }, { "BHILL5a", 638 }, { "BHILL5b", 639 }, { "CAS_1a", 640 }, { "CAS_1b", 642 }, { "CAS_2", 643 }, { "CAS_3", 644 }, { "CAS_4a", 645 }, { "CAS_4b", 646 }, { "CAS_4c", 647 }, { "CAS_5a", 648 }, { "CAS_6a", 649 }, { "CAS6b_1", 650 }, { "CAS6b_2", 651 }, { "CAS_7b", 652 }, { "CAS_9a1", 653 }, { "CAS_9a2", 654 }, { "CAS_11a", 641 }, { "CAT_1", 655 }, { "CAT_2", 656 }, { "CAT_3", 657 }, { "CAT_4", 658 }, { "CESAR1A", 659 }, { "CESAR2A", 660 }, { "CRASH1A", 661 }, { "CRASH2A", 662 }, { "CRASH3A", 663 }, { "CRASHV1", 664 }, { "CRASv2a", 665 }, { "CRASv2b", 666 }, { "D10_ALT", 667 }, { "D8_ALT", 668 }, { "DESERT1", 671 }, { "DESERT2", 672 }, { "DESERT3", 673 }, { "DESERT4", 674 }, { "DESERT6", 675 }, { "DESERT8", 676 }, { "DESERT9", 677 }, { "DES_10A", 678 }, { "DES_10B", 679 }, { "DOC_2", 680 }, { "EPILOG", 681 }, { "FARL_2A", 682 }, { "FARL_3A", 683 }, { "FARL_3B", 684 }, { "FARL_4A", 685 }, { "FARL_5A", 686 }, { "FINAL1A", 687 }, { "FINAL2A", 688 }, { "FINAL2B", 689 }, { "GARAG1B", 690 }, { "GARAG1C", 691 }, { "GARAG3A", 692 }, { "GROVE1a", 693 }, { "GROVE1b", 694 }, { "GROVE1c", 695 }, { "GROVE2", 696 }, { "HEIST1a", 697 }, { "HEIST2a", 698 }, { "HEIST4a", 699 }, { "HEIST5a", 700 }, { "HEIST6a", 701 }, { "HEIST8a", 702 }, { "INTRO1A", 703 }, { "INTRO1B", 704 }, { "INTRO2A", 705 }, { "PROLOG1", 706 }, { "PROLOG2", 707 }, { "PROLOG3", 708 }, { "RIOT_1a", 709 }, { "RIOT_1b", 710 }, { "RIOT_2", 711 }, { "RIOT_4a", 712 }, { "RIOT_4b", 713 }, { "RIOT_4c", 714 }, { "RIOT_4d", 715 }, { "RIOT4e1", 716 }, { "RIOT4e2", 717 }, { "RYDER1A", 718 }, { "RYDER2A", 719 }, { "RYDER3A", 720 }, { "SCRASH1", 721 }, { "SCRASH2", 722 }, { "SMOKE1A", 723 }, { "SMOKE1B", 724 }, { "SMOKE2A", 725 }, { "SMOKE2B", 726 }, { "SMOKE3A", 727 }, { "SMOKE4A", 728 }, { "STEAL_1", 729 }, { "STEAL_2", 730 }, { "STEAL_4", 731 }, { "STEAL_5", 732 }, { "STRAP1A", 733 }, { "STRAP2A", 734 }, { "STRAP3A", 735 }, { "STRAP4A", 736 }, { "STRP4B1", 737 }, { "STRP4B2", 738 }, { "SWEET1A", 739 }, { "SWEET1B", 740 }, { "SWEET1C", 741 }, { "SWEET2A", 742 }, { "SWEET2B", 743 }, { "SWEET3A", 744 }, { "SWEET3B", 745 }, { "SWEET4A", 746 }, { "SWEET5A", 747 }, { "SWEET6A", 748 }, { "SWEET6B", 749 }, { "SWEET7A", 750 }, { "SYND_2A", 751 }, { "SYND_2B", 752 }, { "SYND_3A", 753 }, { "SYND_4A", 754 }, { "SYND_4B", 755 }, { "SYND_7", 756 }, { "TRUTH_1", 758 }, { "TRUTH_2", 757 }, { "W2_ALT", 759 }, { "WOOZI1A", 761 }, { "WOOZI1B", 762 }, { "WOOZIE2", 760 }, { "WOOZIE4", 763 }, { "ZERO_1", 764 }, { "ZERO_2", 765 }, { "ZERO_4", 766 }, { "DATE1a", 670 }, { "DATE1b", 669 }, { "DATE2a", 670 }, { "DATE2b", 669 }, { "DATE3a", 670 }, { "DATE3b", 669 }, { "DATE4a", 670 }, { "DATE4b", 669 }, { "DATE5a", 670 }, { "DATE5b", 669 }, { "DATE6a", 670 }, { "DATE6b", 669 }
    };
    const auto csName = notsa::ci_string_view{ cutsceneName };
    for (const auto& [name, id] : mapping) {
        if (csName == name) {
            return id;
        }
    }
    return -1;
}

void CCutsceneMgr::InjectHooks() {
    RH_ScopedClass(CCutsceneMgr);
    RH_ScopedCategoryGlobal();

    //RH_ScopedGlobalInstall(FindCutsceneAudioTrackId, 0x8D0AA8, {.locked = true}); // Calling the original function from our code crashes, and vice versa
    RH_ScopedGlobalInstall(SetCutsceneAnim, 0x5B0390);
    RH_ScopedGlobalInstall(SetCutsceneAnimToLoop, 0x5B0420);
    RH_ScopedGlobalInstall(SetHeadAnim, 0x5B0440);
    RH_ScopedGlobalInstall(AttachObjectToBone, 0x5B0450);
    RH_ScopedGlobalInstall(AttachObjectToFrame, 0x5B0480);
    RH_ScopedGlobalInstall(AttachObjectToParent, 0x5B04B0);
    RH_ScopedGlobalInstall(AddCutsceneHead, 0x5B0380);
    RH_ScopedGlobalInstall(FinishCutscene, 0x5B04D0);
    RH_ScopedGlobalInstall(HasCutsceneFinished, 0x5B0570);
    RH_ScopedGlobalInstall(LoadCutsceneData_preload, 0x5B05A0);
    RH_ScopedGlobalInstall(LoadCutsceneData_loading, 0x5B11C0);
    RH_ScopedGlobalInstall(LoadCutsceneData_overlay, 0x5B13F0);
    RH_ScopedGlobalInstall(StartCutscene, 0x5B1460);
    RH_ScopedGlobalInstall(SetupCutsceneToStart, 0x5B14D0);
    RH_ScopedGlobalInstall(GetCutsceneTimeInMilleseconds, 0x5B0550);
    RH_ScopedGlobalInstall(CreateCutsceneObject, 0x5B02A0);
    RH_ScopedGlobalInstall(DeleteCutsceneData_overlay, 0x5AFD60);
    RH_ScopedGlobalInstall(LoadCutsceneData_postload, 0x5AFBC0);
    RH_ScopedGlobalInstall(Initialise, 0x4D5A20);
    RH_ScopedGlobalInstall(LoadAnimationUncompressed, 0x4D5AB0);
    RH_ScopedGlobalInstall(RemoveEverythingBecauseCutsceneDoesntFitInMemory, 0x4D5AF0);
    RH_ScopedGlobalInstall(LoadEverythingBecauseCutsceneDeletedAllOfIt, 0x4D5C10);
    RH_ScopedGlobalInstall(Update, 0x4D5D00);
    RH_ScopedGlobalInstall(IsCutsceneSkipButtonBeingPressed, 0x4D5D10);
    RH_ScopedGlobalInstall(AppendToNextCutscene, 0x4D5DB0);
    RH_ScopedGlobalInstall(BuildCutscenePlayer, 0x4D5E20);
    RH_ScopedGlobalInstall(RemoveCutscenePlayer, 0x4D5E50);
    RH_ScopedGlobalInstall(Shutdown, 0x4D5E60);
    RH_ScopedGlobalInstall(LoadCutsceneData, 0x4D5E80);
    RH_ScopedGlobalInstall(DeleteCutsceneData, 0x4D5ED0);
    RH_ScopedGlobalInstall(HideRequestedObjects, 0x5AFAD0);
    RH_ScopedGlobalInstall(UpdateCutsceneObjectBoundingBox, 0x5B01E0);
    RH_ScopedGlobalInstall(CalculateBoundingSphereRadiusCB, 0x5B0130);
    RH_ScopedGlobalInstall(SkipCutscene, 0x5B1700);
    RH_ScopedGlobalInstall(Update_overlay, 0x5B1720);
}
