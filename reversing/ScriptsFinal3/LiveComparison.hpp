#pragma once
// Banco dentro del motor (flujo simple, estilo Small2/PadMouse1/HudCamFinal2): ScriptsFinal3 = las 69 funciones de CTheScripts.
// Original (hook desactivado = codigo del EXE) frente a candidato (hook activo) con el mismo fixture determinista por caso. Corre dentro del juego completo
// (partida cargada) y se engancha en CGame::Process con probe_build.py. Colaboradores simulados con EntryPatch (direccion del EXE + simbolo de la DLL + su cuerpo),
// activados solo mientras se prueba la funcion que los usa. Toda la zona de estado de CTheScripts (0xA43C78..0xA95780) se guarda y se restaura tras cada ejecucion;
// los pools de peds/edificios/objetos/dummies/TXD y el motor RW son falsos (el puntero global se intercambia y se restaura). No se toca ningun fichero.
#include "Cases4.hpp"
#include "Tasks/Task.h"

namespace ScriptsFinal3Live {

struct Entry { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

inline const std::array<Fn, 69> fns{{
    {"CTheScripts","AddScriptCheckpoint",0x4935A0,0,-1,256},
    {"CTheScripts","AddScriptEffectSystem",0x492F90,0,-1,128},
    {"CTheScripts","AddScriptSearchLight",0x493000,0,-1,256},
    {"CTheScripts","AddScriptSphere",0x483B30,0,-1,128},
    {"CTheScripts","AddToBuildingSwapArray",0x481140,0,-1,256},
    {"CTheScripts","AddToInvisibilitySwapArray",0x481200,0,-1,256},
    {"CTheScripts","AddToListOfConnectedLodObjects",0x470980,0,-1,256},
    {"CTheScripts","AddToListOfSpecialAnimGroupsAttachedToCharModels",0x474750,0,-1,256},
    {"CTheScripts","AddToSwitchJumpTable",0x470390,0,-1,128},
    {"CTheScripts","AddToVehicleModelsBlockedByScript",0x46B200,0,-1,128},
    {"CTheScripts","AddToWaitingForScriptBrainArray",0x46AB60,0,-1,256},
    {"CTheScripts","AttachSearchlightToSearchlightObject",0x4934F0,0,-1,256},
    {"CTheScripts","CheckStreamedScriptVersion",0x464FF0,0,-1,64},
    {"CTheScripts","CleanUpThisObject",0x4866C0,0,-1,128},
    {"CTheScripts","CleanUpThisPed",0x486300,0,-1,512},
    {"CTheScripts","CleanUpThisVehicle",0x486670,0,-1,128},
    {"CTheScripts","ClearAllSuppressedCarModels",0x46A7C0,0,-1,64},
    {"CTheScripts","ClearAllVehicleModelsBlockedByScript",0x46A840,0,-1,64},
    {"CTheScripts","ClearSpaceForMissionEntity",0x486B00,0,-1,512},
    {"CTheScripts","DoScriptSetupAfterPoolsHaveLoaded",0x5D3390,0,-1,128},
    {"CTheScripts","DrawDebugAngledSquare",0x486990,0,-1,128},
    {"CTheScripts","DrawDebugSquare-",0x486840,0,-1,128},
    {"CTheScripts","DrawScriptSpheres",0x4810E0,0,-1,128},
    {"CTheScripts","DrawScriptSpritesAndRectangles",0x464980,0,-1,512},
    {"CTheScripts","GetActualScriptThingIndex",0x4839A0,0,-1,1024},
    {"CTheScripts","GetNewUniqueScriptThingIndex",0x483720,0,-1,1024},
    {"CTheScripts","GetScriptIndexFromPointer",0x464D20,0,-1,64},
    {"CTheScripts","GetUniqueScriptThingIndex",0x4810C0,0,-1,128},
    {"CTheScripts","HasCarModelBeenSuppressed",0x46A810,0,-1,128},
    {"CTheScripts","HasVehicleModelBeenBlockedByScript",0x46A890,0,-1,128},
    {"CTheScripts","HighlightImportantAngledArea",0x485EF0,0,-1,512},
    {"CTheScripts","HighlightImportantArea-",0x485E00,0,-1,512},
    {"CTheScripts","Init",0x468D50,0,-1,256},
    {"CTheScripts","InitialiseAllConnectLodObjects",0x470960,0,-1,64},
    {"CTheScripts","InitialiseConnectLodObjects",0x470940,0,-1,128},
    {"CTheScripts","InitialiseSpecialAnimGroup",0x474710,0,-1,128},
    {"CTheScripts","InitialiseSpecialAnimGroupsAttachedToCharModels",0x474730,0,-1,64},
    {"CTheScripts","IsPedStopped",0x486110,0,-1,512},
    {"CTheScripts","IsPlayerOnAMission",0x464D50,0,-1,128},
    {"CTheScripts","IsVehicleStopped",0x4861F0,0,-1,256},
    {"CTheScripts","Load",0x5D4FD0,0,-1,128},
    {"CTheScripts","PrintListSizes",0x4646D0,0,-1,64},
    {"CTheScripts","Process",0x46A000,0,-1,512},
    {"CTheScripts","ProcessAllSearchLights",0x4939F0,0,-1,512},
    {"CTheScripts","ProcessWaitingForScriptBrainArray",0x46CF00,0,-1,512},
    {"CTheScripts","ReadMultiScriptFileOffsetsFromScript",0x4867C0,0,-1,128},
    {"CTheScripts","ReadObjectNamesFromScript",0x486720,0,-1,128},
    {"CTheScripts","ReinitialiseSwitchStatementData",0x470370,0,-1,64},
    {"CTheScripts","RemoveFromWaitingForScriptBrainArray",0x46ABC0,0,-1,256},
    {"CTheScripts","RemoveScriptCheckpoint",0x4936C0,0,-1,256},
    {"CTheScripts","RemoveScriptEffectSystem",0x492FD0,0,-1,128},
    {"CTheScripts","RemoveScriptSearchLight",0x493160,0,-1,256},
    {"CTheScripts","RemoveScriptSphere",0x483BA0,0,-1,128},
    {"CTheScripts","RemoveScriptTextureDictionary",0x465A40,0,-1,128},
    {"CTheScripts","RemoveThisPed",0x486240,0,-1,256},
    {"CTheScripts","RenderAllSearchLights",0x493E30,0,-1,256},
    {"CTheScripts","Save",0x5D4C40,0,-1,128},
    {"CTheScripts","ScriptAttachAnimGroupToCharModel",0x474800,0,-1,128},
    {"CTheScripts","ScriptConnectLodsFunction",0x470A20,0,-1,256},
    {"CTheScripts","ScriptDebugCircle2D",0x485C20,0,-1,256},
    {"CTheScripts","ScriptDebugLine3D",0x485DE0,0,-1,64},
    {"CTheScripts","StartNewScript-indexed",0x464C90,0,-1,256},
    {"CTheScripts","StartNewScript-last-idle",0x464C20,0,-1,256},
    {"CTheScripts","StartTestScript",0x464D40,0,-1,128},
    {"CTheScripts","UndoBuildingSwaps",0x481290,0,-1,128},
    {"CTheScripts","UndoEntityInvisibilitySettings",0x4812D0,0,-1,128},
    {"CTheScripts","UpdateObjectIndices",0x486780,0,-1,256},
    {"CTheScripts","UseSwitchJumpTable",0x4703C0,0,-1,512},
    {"CTheScripts","WipeLocalVariableMemoryForMissionScript",0x464BB0,0,-1,64},
}};
inline const std::array<CaseFn, 69> caseFns{{
    CaseAddCheckpoint, CaseAddEffect, CaseAddSearchLight, CaseAddSphere, CaseAddBuildingSwap, CaseAddInvisibility, CaseAddConnectedLod, CaseAddAnim,
    CaseAddSwitch, CaseAddBlocked, CaseAddWaiting, CaseAttachSearchLight, CaseCheckStreamedVersion, CaseCleanObject, CaseCleanPed, CaseCleanVehicle,
    CaseClearSuppressed, CaseClearSuppressed, CaseClearSpace, CaseDoScriptSetup, CaseDebugAngledSquare, CaseDebugSquare, CaseDrawSpheres, CaseDrawRects,
    CaseGetActual, CaseGetNewUnique, CaseGetScriptIndex, CaseGetUnique, CaseHasSuppressed, CaseHasBlocked, CaseHighlightAngled, CaseHighlightArea,
    CaseInit, CaseInitConnectAll, CaseInitConnectOne, CaseInitAnimOne, CaseInitAnimAll, CaseIsPedStopped, CaseIsPlayerOnMission, CaseIsVehStopped,
    CaseLoad, CasePrintListSizes, CaseProcess, CaseProcessSearchLights, CaseProcessWaiting, CaseReadMulti, CaseReadObjectNames, CaseReinitSwitch,
    CaseRemoveWaiting, CaseRemoveCheckpoint, CaseRemoveEffect, CaseRemoveSearchLight, CaseRemoveSphere, CaseRemoveTxd, CaseRemovePed, CaseRenderSearchLights,
    CaseSave, CaseAttachAnim, CaseConnectLods, CaseDebugCircle, CaseDebugLine, CaseStartNewScriptIdx, CaseStartNewScript, CaseStartTestScript,
    CaseUndoBuildingSwaps, CaseUndoInvisibility, CaseUpdateObjectIndices, CaseUseSwitch, CaseWipeLocals,
}};

inline void RegisterSites() {
    using std::bit_cast;
    AddSite("RegisterReference", 0x571B70, Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)), &SRegRef);
    AddSite("CleanUpOldReference", 0x571A00, Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)), &SCleanRef);
    AddSite("CWorld::Add", 0x563220, Addr(static_cast<void(*)(CEntity*)>(&CWorld::Add)), &SWorldAdd);
    AddSite("CWorld::Remove", 0x563280, Addr(static_cast<void(*)(CEntity*)>(&CWorld::Remove)), &SWorldRemove);
    AddSite("CBuilding::ReplaceWithNewModel", 0x403EC0, Addr(&CBuilding::ReplaceWithNewModel), &SReplaceModel);
    AddSite("CEntity::SetupBigBuilding", 0x533150, Addr(&CEntity::SetupBigBuilding), &SSetupBig);
    AddSite("CWorld::RemoveReferencesToDeletedObject", 0x565510, Addr(&CWorld::RemoveReferencesToDeletedObject), &SRemoveRefs);
    AddSite("FindPlayerCentreOfWorld", 0x56E250, Addr(static_cast<const CVector&(*)(int32)>(&FindPlayerCentreOfWorld)), &SCentre);
    AddSite("CScriptsForBrains::StartNewStreamedScriptBrain", 0x46B270, Addr(&CScriptsForBrains::StartNewStreamedScriptBrain), &SBrainStart);
    AddSite("CScriptsForBrains::IsObjectWithinBrainActivationRange", 0x46B3D0, Addr(&CScriptsForBrains::IsObjectWithinBrainActivationRange), &SBrainRange);
    AddSite("CScriptsForBrains::StartOrRequestNewStreamedScriptBrain", 0x46CD80, Addr(&CScriptsForBrains::StartOrRequestNewStreamedScriptBrain), &SBrainRequest);
    AddSite("CStreaming::RequestModel", 0x4087E0, Addr(&CStreaming::RequestModel), &SRequestModel);
    AddSite("SaveDataToWorkBuffer", 0x5D1270, Addr(static_cast<bool(*)(void*, int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)), &SSave);
    AddSite("LoadDataToWorkBuffer", 0x5D1300, Addr(static_cast<bool(*)(void*, int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)), &SLoad);
    AddSite("CFileMgr::SetDir", 0x5387D0, Addr(&CFileMgr::SetDir), &SSetDir);
    AddSite("CFileMgr::OpenFile", 0x538900, Addr(&CFileMgr::OpenFile), &SOpenFile);
    AddSite("CFileMgr::Read", 0x538950, Addr(&CFileMgr::Read), &SReadFile);
    AddSite("CFileMgr::CloseFile", 0x5389D0, Addr(&CFileMgr::CloseFile), &SCloseFile);
    AddSite("CScripted2dEffects::Init", 0x6FA6F0, Addr(&CScripted2dEffects::Init), &SInit2d);
    AddSite("CTaskSequences::Init", 0x632D90, Addr(&CTaskSequences::Init), &SInitSeq);
    AddSite("CPedGroups::Init", 0x5FB8C0, Addr(&CPedGroups::Init), &SInitGroups);
    AddSite("CInformFriendsEventQueue::Init", 0x4B2990, Addr(&CInformFriendsEventQueue::Init), &SInitFriends);
    AddSite("CInformGroupEventQueue::Init", 0x4B2AD0, Addr(&CInformGroupEventQueue::Init), &SInitGroupQ);
    AddSite("CStreamedScripts::ReadStreamedScriptData", 0x470750, Addr(&CStreamedScripts::ReadStreamedScriptData), &SReadStreamed);
    AddSite("CTxdStore::FindTxdSlot", 0x731850, Addr(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)), &SFindTxd);
    AddSite("CTxdStore::RemoveTxd", 0x731E90, Addr(&CTxdStore::RemoveTxd), &SRemoveTxd);
    AddSite("CSprite2d::Delete", 0x727240, Addr(&CSprite2d::Delete), &SSpriteDelete);
    AddSite("CModelInfo::GetModelInfo(name)", 0x4C5940, Addr(static_cast<CBaseModelInfo*(*)(const char*, int32*)>(&CModelInfo::GetModelInfo)), &SGetModelInfo);
    AddSite("CCheckpoints::PlaceMarker", 0x722C40, Addr(&CCheckpoints::PlaceMarker), &SPlaceMarker12, &SPlaceMarker9);
    AddSite("CCheckpoints::DeleteCP", 0x722FC0, Addr(&CCheckpoints::DeleteCP), &SDeleteCP);
    AddSite("C3dMarkers::PlaceMarkerSet", 0x725BA0, Addr(&C3dMarkers::PlaceMarkerSet), &SMarkerSet);
    AddSite("CHeli::SearchLightCone", 0x6C58E0, Addr(&CHeli::SearchLightCone), &SSearchCone);
    AddSite("CWorld::FindGroundZForCoord", 0x569660, Addr(&CWorld::FindGroundZForCoord), &SGroundZ);
    AddSite("CShadows::RenderIndicatorShadow", 0x70CCB0, Addr(&CShadows::RenderIndicatorShadow), &SIndicator);
    AddSite("CWorld::ProcessVerticalLine", 0x5674E0, Addr(&CWorld::ProcessVerticalLine), &SVertLine);
    AddSite("CSprite2d::DrawRect", 0x727B60, Addr(static_cast<void(*)(const CRect&, const CRGBA&)>(&CSprite2d::DrawRect)), &SDrawRect);
    AddSite("CSprite2d::Draw(rect)", 0x728350, Addr(static_cast<void(CSprite2d::*)(const CRect&, const CRGBA&)>(&CSprite2d::Draw)), &SSpriteDraw);
    AddSite("CSprite2d::Draw(8)", 0x728520, Addr(static_cast<void(CSprite2d::*)(float, float, float, float, float, float, float, float, const CRGBA&)>(&CSprite2d::Draw)), &SSpriteDraw8);
    AddSite("CMenuManager::DrawWindowedText", 0x578F50, Addr(&CMenuManager::DrawWindowedText), &SWindowedText);
    AddSite("CMenuManager::DrawWindow", 0x573EE0, Addr(&CMenuManager::DrawWindow), &SWindow);
    AddSite("CRunningScript::Process", 0x469F00, Addr(&CRunningScript::Process), &SScriptProcess);
    AddSite("CUpsideDownCarCheck::UpdateTimers", 0x4655E0, Addr(&CUpsideDownCarCheck::UpdateTimers), &SUpsideDown);
    AddSite("CStuckCarCheck::Process", 0x465680, Addr(&CStuckCarCheck::Process), &SStuckCars);
    AddSite("CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects", 0x4652D0, Addr(&CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects), &SCleanupColl);
    AddSite("CLoadingScreen::NewChunkLoaded", 0x590D00, Addr(&CLoadingScreen::NewChunkLoaded), &SNewChunk);
    AddSite("CPedIntelligence::RecordEventForScript", 0x602050, Addr(&CPedIntelligence::RecordEventForScript), &SRecordEvent);
    AddSite("CTheScripts::DrawScriptSpheres", 0x4810E0, Addr(&CTheScripts::DrawScriptSpheres), &SSpheres);
    AddSite("CTheScripts::ProcessAllSearchLights", 0x4939F0, Addr(&CTheScripts::ProcessAllSearchLights), &SSearchProc);
    AddSite("CTheScripts::ProcessWaitingForScriptBrainArray", 0x46CF00, Addr(&CTheScripts::ProcessWaitingForScriptBrainArray), &SBrainProc);
    AddSite("CMatrix::UpdateRwMatrix", 0x59AD70, Addr(&CMatrix::UpdateRwMatrix), &SUpdMatrix);
    AddSite("CSimpleTransform::UpdateRwMatrix", 0x54EF40, Addr(&CSimpleTransform::UpdateRwMatrix), &SUpdSimple);
    AddSite("CEntity::UpdateRwFrame", 0x532B00, Addr(&CEntity::UpdateRwFrame), &SUpdRwFrame);
    AddSite("CAnimManager::GetAnimationBlockIndex", 0x4D3990, Addr(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex)), &SAnimBlock);
    AddSite("CPed::SetCharCreatedBy", 0x5E47E0, Addr(&CPed::SetCharCreatedBy), &SSetCreatedBy);
    AddSite("CCarCtrl::JoinCarWithRoadSystem", 0x42F5A0, Addr(&CCarCtrl::JoinCarWithRoadSystem), &SJoinCar);
    AddSite("CPedGroups::GetPedsGroup", 0x5F7E80, Addr(&CPedGroups::GetPedsGroup), &SGetGroup);
    AddSite("CPedGroupMembership::IsFollower", 0x5F69E0, Addr(&CPedGroupMembership::IsFollower), &SIsFollower);
    AddSite("CPedGroupMembership::RemoveMember", 0x5FB210, Addr(static_cast<void(CPedGroupMembership::*)(CPed*)>(&CPedGroupMembership::RemoveMember)), &SRemoveMember);
    AddSite("CEventGroup::GetEventOfType", 0x4AB650, Addr(&CEventGroup::GetEventOfType), &SGetEvent);
    AddSite("CPed::CanBeDeleted", 0x5DF100, Addr(&CPed::CanBeDeleted), &SPedDeletable);
    AddSite("CVehicle::CanBeDeleted", 0x6D1180, Addr(&CVehicle::CanBeDeleted), &SVehDeletable);
    AddSite("CCarCtrl::RemoveFromInterestingVehicleList", 0x423ED0, Addr(&CCarCtrl::RemoveFromInterestingVehicleList), &SRemoveInteresting);
    AddSite("CVehicleRecording::StopPlaybackRecordedCar", 0x45A280, Addr(&CVehicleRecording::StopPlaybackRecordedCar), &SStopRec);
    AddSite("CVehicle::SetVehicleCreatedBy", 0x6D5D70, Addr(&CVehicle::SetVehicleCreatedBy), &SSetVehCreated);
    AddSite("CVehicle::RemoveDriver", 0x6D1950, Addr(&CVehicle::RemoveDriver), &SRemoveDriver);
    AddSite("CVehicle::RemovePassenger", 0x6D1610, Addr(&CVehicle::RemovePassenger), &SRemovePassenger);
    AddSite("CVehicle::IsLawEnforcementVehicle", 0x6D2370, Addr(&CVehicle::IsLawEnforcementVehicle), &SIsLaw);
    AddSite("CVehicle::ChangeLawEnforcerState", 0x6D2330, Addr(&CVehicle::ChangeLawEnforcerState), &SChangeLaw);
    AddSite("CPopulation::RemovePed", 0x610F20, Addr(&CPopulation::RemovePed), &SRemovePed);
    AddSite("CWorld::FindObjectsKindaColliding", 0x568B80, Addr(&CWorld::FindObjectsKindaColliding), &SFindObjects);
    AddSite("CCollision::ProcessColModels", 0x4185C0, Addr(&CCollision::ProcessColModels), &SProcessCol);
    AddSite("CEntity::GetColModel", 0x535300, Addr(&CEntity::GetColModel), &SGetColModel);
    AddSite("RpAnimBlendClumpGetAssociation(id)", 0x4D68B0, Addr(static_cast<CAnimBlendAssociation*(*)(RpClump*, uint32)>(&RpAnimBlendClumpGetAssociation)), &SAnimAssoc);
}

inline std::unique_ptr<Patches> PatchesFor(const std::string& name) {
    static const std::vector<const char*> initColl = { "CFileMgr::SetDir", "CFileMgr::OpenFile", "CFileMgr::Read", "CFileMgr::CloseFile", "CScripted2dEffects::Init", "CTaskSequences::Init",
        "CPedGroups::Init", "CInformFriendsEventQueue::Init", "CInformGroupEventQueue::Init", "CStreamedScripts::ReadStreamedScriptData", "CTxdStore::FindTxdSlot", "CTxdStore::RemoveTxd",
        "CSprite2d::Delete", "CModelInfo::GetModelInfo(name)" };
    auto many = [&](std::initializer_list<const char*> extra, bool withInit) {
        std::vector<const char*> v(extra); if (withInit) v.insert(v.end(), initColl.begin(), initColl.end());
        auto p = std::make_unique<Patches>();
        for (auto* n : v) { auto q = Use({ n }); for (auto& e : q->v) p->v.push_back(std::move(e)); for (auto a : q->seen) p->seen.insert(a); q->v.clear(); }
        return p;
    };
    const std::string& n = name;
    if (n == "AddScriptCheckpoint") return Use({ "CCheckpoints::PlaceMarker" });
    if (n == "RemoveScriptCheckpoint") return Use({ "CCheckpoints::DeleteCP" });
    if (n == "AddScriptSearchLight" || n == "RemoveScriptSearchLight" || n == "AttachSearchlightToSearchlightObject" || n == "AddToWaitingForScriptBrainArray") return Use({ "RegisterReference", "CleanUpOldReference" });
    if (n == "RemoveFromWaitingForScriptBrainArray") return Use({ "CleanUpOldReference" });
    if (n == "UndoBuildingSwaps") return Use({ "CBuilding::ReplaceWithNewModel" });
    if (n == "DoScriptSetupAfterPoolsHaveLoaded" || n == "ScriptConnectLodsFunction") return Use({ "CWorld::Add", "CWorld::Remove", "CEntity::SetupBigBuilding" });
    if (n == "DrawScriptSpheres") return Use({ "C3dMarkers::PlaceMarkerSet" });
    if (n == "RenderAllSearchLights") return Use({ "CHeli::SearchLightCone" });
    if (n == "HighlightImportantArea-" || n == "HighlightImportantAngledArea") return Use({ "CWorld::FindGroundZForCoord", "CShadows::RenderIndicatorShadow" });
    if (n == "DrawDebugSquare-" || n == "DrawDebugAngledSquare") return Use({ "CWorld::ProcessVerticalLine" });
    if (n == "DrawScriptSpritesAndRectangles") return Use({ "CSprite2d::DrawRect", "CSprite2d::Draw(rect)", "CSprite2d::Draw(8)", "CMenuManager::DrawWindowedText", "CMenuManager::DrawWindow" });
    if (n == "UpdateObjectIndices") return Use({ "CModelInfo::GetModelInfo(name)" });
    if (n == "RemoveScriptTextureDictionary") return Use({ "CTxdStore::FindTxdSlot", "CTxdStore::RemoveTxd", "CSprite2d::Delete" });
    if (n == "Init") return many({}, true);
    if (n == "Save") return Use({ "SaveDataToWorkBuffer" });
    if (n == "Load") return many({ "SaveDataToWorkBuffer", "LoadDataToWorkBuffer", "CWorld::Add", "CWorld::Remove", "CBuilding::ReplaceWithNewModel", "CAnimManager::GetAnimationBlockIndex" }, true);
    if (n == "ScriptAttachAnimGroupToCharModel") return Use({ "CAnimManager::GetAnimationBlockIndex" });
    if (n == "Process") return Use({ "CRunningScript::Process", "CUpsideDownCarCheck::UpdateTimers", "CStuckCarCheck::Process", "CMissionCleanup::CheckIfCollisionHasLoadedForMissionObjects",
        "CLoadingScreen::NewChunkLoaded", "CPedIntelligence::RecordEventForScript", "CTheScripts::DrawScriptSpheres", "CTheScripts::ProcessAllSearchLights", "CTheScripts::ProcessWaitingForScriptBrainArray" });
    if (n == "ProcessAllSearchLights") return Use({ "CMatrix::UpdateRwMatrix", "CSimpleTransform::UpdateRwMatrix", "CEntity::UpdateRwFrame" });
    if (n == "ProcessWaitingForScriptBrainArray") return Use({ "FindPlayerCentreOfWorld", "CScriptsForBrains::StartNewStreamedScriptBrain", "CScriptsForBrains::IsObjectWithinBrainActivationRange",
        "CScriptsForBrains::StartOrRequestNewStreamedScriptBrain", "CStreaming::RequestModel" });
    if (n == "CleanUpThisVehicle") return Use({ "CCarCtrl::RemoveFromInterestingVehicleList", "CVehicleRecording::StopPlaybackRecordedCar", "CVehicle::SetVehicleCreatedBy" });
    if (n == "CleanUpThisPed") return Use({ "CPed::SetCharCreatedBy", "CCarCtrl::JoinCarWithRoadSystem", "CPedGroups::GetPedsGroup", "CPedGroupMembership::IsFollower", "CPedGroupMembership::RemoveMember", "CEventGroup::GetEventOfType" });
    if (n == "RemoveThisPed") return Use({ "CVehicle::RemoveDriver", "CVehicle::RemovePassenger", "CVehicle::IsLawEnforcementVehicle", "CVehicle::ChangeLawEnforcerState", "CWorld::RemoveReferencesToDeletedObject" });
    if (n == "IsPedStopped") return Use({ "RpAnimBlendClumpGetAssociation(id)" });
    if (n == "ClearSpaceForMissionEntity") return Use({ "CWorld::FindObjectsKindaColliding", "CCollision::ProcessColModels", "CEntity::GetColModel", "CPed::CanBeDeleted", "CVehicle::CanBeDeleted", "CPopulation::RemovePed",
        "CVehicle::RemovePassenger", "CCarCtrl::RemoveFromInterestingVehicleList", "CWorld::Remove", "CleanUpOldReference" });
    return nullptr;
}

inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr, exe, MAX_PATH); const auto dir = std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir / "scriptsfinal3-runtime.start")) { done = true; return; }
    if (!log.is_open()) { log.open(dir / "scriptsfinal3-runtime.jsonl"); Write({ {"event", "waiting_for_player"} }); ReversingProbe::InstallFailureCapture("scriptsfinal3"); }
    auto* player = FindPlayerPed(); if (!player || CCutsceneMgr::IsRunning()) return; done = true; owner = GetCurrentThreadId();
    std::string only; { std::ifstream f(dir / "scriptsfinal3-only.txt"); if (f) std::getline(f, only); }
    std::vector<Hook*> hooks(fns.size(), nullptr);
    for (size_t f = 0; f < fns.size(); ++f) if (!(hooks[f] = FindHook(fns[f].cls, fns[f].name))) { Write({ {"event", "error"}, {"reason", "hook not found"}, {"function", std::string(fns[f].cls) + "::" + fns[f].name} }); return; }
    std::vector<bool> enabled(fns.size()), locked(fns.size());
    for (size_t f = 0; f < fns.size(); ++f) { enabled[f] = hooks[f]->Hooked(); locked[f] = hooks[f]->Locked(); hooks[f]->LockState(false); }
    gSaveHook = FindHook("CTheScripts", "Save");

    sites.clear(); RegisterSites();
    bool known = true; json siteInfo = json::array();
    for (auto& st : sites) st.body = st.dll ? Body(st.dll) : 0;       // ANTES de crear ningun EntryPatch
    for (auto& st : sites) {
        const bool ok = InExe(st.exeAddr) && (st.dll == 0 || (!InExe(st.dll) && st.body != 0 && !InExe(st.body)));
        known &= ok; siteInfo.push_back({ {"name", st.name}, {"exe", st.exeAddr}, {"dll", st.dll}, {"body", st.body}, {"ok", ok} });
    }
    std::vector<std::pair<uintptr_t, std::vector<uint8>>> entryBefore;
    for (auto& st : sites) for (auto a : { st.exeAddr, st.dll, st.body }) if (a) entryBefore.push_back({ a, ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8) });
    for (auto& fn : fns) entryBefore.push_back({ fn.address, ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address), 8) });

    BuildRanges(); BuildVtables();
    SaveGlobals();

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk = true; json lay = json::object();
    auto Chk = [&](const char* n, uint32 got, uint32 want) { lay[n] = got; if (got != want) { layoutOk = false; lay[std::string(n) + "_expected"] = want; } };
    {
        Chk("RwGlobals.dOpenDevice.fpRenderStateSet (0x485C30 call [eax+20h])", uint32(reinterpret_cast<uintptr_t>(&FakeGlobals().dOpenDevice.fpRenderStateSet) - reinterpret_cast<uintptr_t>(&FakeGlobals())), 0x20);
        Chk("RwGlobals.dOpenDevice.fpIm2DRenderLine (0x734EC6 jmp [ecx+28h])", uint32(reinterpret_cast<uintptr_t>(&FakeGlobals().dOpenDevice.fpIm2DRenderLine) - reinterpret_cast<uintptr_t>(&FakeGlobals())), 0x28);
        Chk("CVehicle.m_pDriver (0x486CD1)", uint32(offsetof(CVehicle, m_pDriver)), 0x460);
        Chk("CVehicle.m_apPassengers (0x486D08)", uint32(offsetof(CVehicle, m_apPassengers)), 0x464);
        Chk("CVehicle.m_nMaxPassengers (0x486CFC)", uint32(offsetof(CVehicle, m_nMaxPassengers)), 0x488);
        Chk("CVehicle.m_nCreatedBy (0x486679)", uint32(offsetof(CVehicle, m_nCreatedBy)), 0x4A4);
        Chk("CVehicle.m_nVehicleType (0x486448)", uint32(offsetof(CVehicle, m_nVehicleType)), 0x590);
        Chk("CVehicle.m_nVehicleSubType (0x486375)", uint32(offsetof(CVehicle, m_nVehicleSubType)), 0x594);
        Chk("CVehicle.m_nDoorLock (0x486296)", uint32(offsetof(CVehicle, m_nDoorLock)), 0x4F8);
        Chk("CVehicle.m_autoPilot.m_nCarMission (0x486388 [eax+3BAh])", uint32(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_nCarMission)), 0x3BA);
        Chk("CVehicle.m_autoPilot.m_vecDestinationCoors (0x4863AB [eax+3ECh])", uint32(offsetof(CVehicle, m_autoPilot) + offsetof(CAutoPilot, m_vecDestinationCoors)), 0x3EC);
        {
            const uint32 a = uint32(offsetof(CHeli, m_fMinAltitude)), b = uint32(offsetof(CHeli, m_fMaxAltitude));
            Chk("CHeli min+max altitude == 0x9A8+0x9B0 (0x4863D6/0x4863DC)", a + b, 0x9A8 + 0x9B0);
            const uint32 c = uint32(offsetof(CPlane, m_minAltitude)), d = uint32(offsetof(CPlane, m_maxAltitude));
            Chk("CPlane min+max altitude == 0x9A8+0x9B0 (0x486403..)", c + d, 0x9A8 + 0x9B0);
        }
        Chk("CObject.m_nObjectType (0x4866C8 [eax+13Ch])", uint32(offsetof(CObject, m_nObjectType)), 0x13C);
        Chk("CObject.m_nRemovalTime (0x4866EB [eax+150h])", uint32(offsetof(CObject, m_nRemovalTime)), 0x150);
        Chk("CObject.m_nRefModelIndex (0x4866FD [eax+14Ah])", uint32(offsetof(CObject, m_nRefModelIndex)), 0x14A);
        Chk("CCheckpoint.m_Type (0x4936E4/0x4936EF mov cx,[eax])", uint32(offsetof(CCheckpoint, m_Type)), 0);
        Chk("CCheckpoint.m_ID (0x4936EA [eax+4])", uint32(offsetof(CCheckpoint, m_ID)), 4);
        Chk("CCollisionData.m_nNumLines (0x486BDC [eax+6])", uint32(offsetof(CCollisionData, m_nNumLines)), 6);
        Chk("CColModel.m_pColData (0x486BCD [ebp+2Ch])", uint32(offsetof(CColModel, m_pColData)), 0x2C);
        Chk("sizeof(CColPoint) * 32 (guarda en 0xA95778)", uint32(sizeof(CColPoint) * 32), 0xA95778 - 0xA951F8);
        Chk("sizeof(tScriptText)", uint32(sizeof(tScriptText)), 0x44);
        Chk("sizeof(tScriptRectangle)", uint32(sizeof(tScriptRectangle)), 0x3C);
        Chk("sizeof(tScriptSearchlight)", uint32(sizeof(tScriptSearchlight)), 0x7C);
        Chk("sizeof(CFire)", uint32(sizeof(CFire)), 0x28);
        Chk("sizeof(CRunningScript)", uint32(sizeof(CRunningScript)), 0xE0);
        Chk("offsetof(tScriptText, WrapX)", uint32(offsetof(tScriptText, WrapX)), 0x10);
        Chk("offsetof(tScriptText, FontStyle)", uint32(offsetof(tScriptText, FontStyle)), 0x28);
        Chk("offsetof(tScriptRectangle, m_Alignment)", uint32(offsetof(tScriptRectangle, m_Alignment)), 0x34);
        Chk("offsetof(tScriptRectangle, m_nTextboxStyle)", uint32(offsetof(tScriptRectangle, m_nTextboxStyle)), 0x38);
        Chk("offsetof(tScriptSearchlight, m_Bulb)", uint32(offsetof(tScriptSearchlight, m_Bulb)), 0x54);
        Chk("offsetof(tScriptSearchlight, vf70)", uint32(offsetof(tScriptSearchlight, vf70)), 0x70);
        Chk("offsetof(CEventScriptCommand, m_task) (0x4864F3 [eax+10h])", uint32(offsetof(CEventScriptCommand, m_task)), 0x10);
        // tablas virtuales falsas: la ranura de la DLL debe coincidir con la del EXE
        InstallFakeModel(1.f, 0, 77);
        Chk("CBaseModelInfo::GetAnimFileIndex = vtable[14] (0x474812 call [edx+38h])", uint32(reinterpret_cast<CBaseModelInfo*>(modelInfoStore.data())->GetAnimFileIndex()), 77);
        Chk("CColModel radio (+0x24)", Bits(reinterpret_cast<CColModel*>(colModelStore.data())->GetBoundRadius()), Bits(1.f));
        At<uint32>(taskStore.data(), 0) = P(taskVt.data()); At<uint32>(taskStore.data(), 0x40) = 912;
        Chk("CTask::GetTaskType = vtable[4] (0x4864F7 call [edx+10h])", uint32(reinterpret_cast<CTask*>(taskStore.data())->GetTaskType()), 912);
        uint32 sp = 7; InstallPools(sp, 100);
        Chk("stride CPool<CObject> (0x470994 / 0x19C)", uint32(GetObjectPool()->GetIndex(reinterpret_cast<CObject*>(objStore.data() + 3 * OBJ_STRIDE))), 3);
        Chk("stride CPool<CBuilding> (0x5D4D18 / 0x38)", uint32(GetBuildingPool()->GetIndex(reinterpret_cast<CBuilding*>(bldStore.data() + 3 * BLD_STRIDE))), 3);
        Chk("stride CPool<CDummy> (0x5D4DC5 / 0x38)", uint32(GetDummyPool()->GetIndex(reinterpret_cast<CDummy*>(dumStore.data() + 3 * DUM_STRIDE))), 3);
        Chk("stride CPool<CPed> (0x46A242 / 0x7C4)", uint32(GetPedPool()->GetIndex(reinterpret_cast<CPed*>(pedStore.data() + 3 * PED_STRIDE))), 3);
        RestoreGlobals();
        Write({ {"event", "layout"}, {"ok", layoutOk}, {"fields", lay} });
    }

    const unsigned savedFP = _controlfp(0, 0);
    Write({ {"event", "begin"}, {"sites", siteInfo}, {"known", known}, {"fp_control", savedFP} });

    bool passed = known && layoutOk; int pair = 0;
    if (known) {
        for (size_t f = 0; f < fns.size(); ++f) {
            const auto& fn = fns[f]; int failures = 0; const std::string full = std::string(fn.cls) + "::" + fn.name;
            if (!only.empty() && only != full) continue;
            auto patches = PatchesFor(fn.name);   // colaboradores simulados solo mientras dura la funcion
            for (int i = 0; i < fn.cases; ++i) {
                fx = Fx{}; json original = caseFns[f](unsigned(i), false, hooks[f], fn.address); RestoreGlobals();
                fx = Fx{}; json candidate = caseFns[f](unsigned(i), true, hooks[f], fn.address); RestoreGlobals();
                const bool ok = original == candidate && original["switches"] == true && candidate["switches"] == true;
                const std::string od = original.dump(), cd = candidate.dump();
                passed &= ok; failures += !ok;
                if (ok) Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", true}, {"accepted", true},
                                {"original", {{"trace", original["trace"]}, {"state_fnv", Fnv(od.data(), od.size())}}},
                                {"candidate", {{"trace", candidate["trace"]}, {"state_fnv", Fnv(cd.data(), cd.size())}}},
                                {"events", original["trace"].size()} });
                else { json diff = json::array(); for (auto& [k, v] : original.items()) if (candidate[k] != v) diff.push_back(k);
                    Write({ {"event", "comparison"}, {"pair", pair++}, {"function", full}, {"case", i}, {"equal", false}, {"accepted", false}, {"differs", diff},
                            {"original", failures <= 2 ? original : json(nullptr)}, {"candidate", failures <= 2 ? candidate : json(nullptr)} }); }
            }
            patches.reset();   // restaura las entradas antes de seguir
            Write({ {"event", "function_summary"}, {"function", full}, {"failures", failures}, {"cases", fn.cases} });
        }
    }
    for (size_t f = 0; f < fns.size(); ++f) { hooks[f]->State(enabled[f]); hooks[f]->LockState(locked[f]); }
    RestoreGlobals();
    _controlfp(savedFP, _MCW_PC | _MCW_RC);
    bool restored = _controlfp(0, 0) == savedFP;
    for (size_t j = 0; j < globals.size(); ++j) restored &= savedGlobals[j] == ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].address), globals[j].size);
    for (auto& [a, b] : entryBefore) restored &= b == ReversingProbe::Bytes(reinterpret_cast<void*>(a), 8);
    for (size_t f = 0; f < fns.size(); ++f) restored &= hooks[f]->Hooked() == enabled[f] && hooks[f]->Locked() == locked[f];
    Write({ {"event", passed ? "complete" : "error"}, {"pairs", pair}, {"layout_ok", layoutOk}, {"state_restored", restored} });
}
}
