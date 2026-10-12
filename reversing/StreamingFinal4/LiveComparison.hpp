#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1/VehModelInfoFinal4): CStreaming, 94 funciones (lista final 4).
// Original (hooks de CStreaming desactivados, codigo del EXE) frente a candidato (hooks activos) sobre un mundo falso y determinista:
// model infos (vtable propia), pools de edificios / dummies / objetos / peatones y de nodos de lista intercambiados en CPools, pool de TXD, listas de
// sectores, informacion de streaming con listas de cargados / pedidos y canales, grupos de poblacion, zona, camara, jugador y ajustes. Los buffers de
// streaming son propios; CdStream, RenderWare, cargadores, almacenes (TXD/COL/IPL/anim/rutas/scripts), poblacion, ficheros, directorio y memoria se
// simulan con dobles trazados (direccion del EXE y simbolo de la DLL). Cada global tocado se guarda y se restaura en cada ejecucion. Ver README.md.
#include "CasesD.hpp"

namespace StreamingFinal4Live {
#define CSTREAMING "CStreaming"
inline const std::array<Fn,94> fns{{
    {"CStreaming","AddEntity",0x409650,0,-1,256},{"CStreaming","AddImageToList",0x407610,0,-1,256},{"CStreaming","AddLodsToRequestList",0x40C520,0,-1,256},{"CStreaming","AddModelsToRequestList",0x40D3F0,0,-1,256},
    {"CStreaming","AddToLoadedVehiclesList",0x408000,0,-1,256},{"CStreaming","AreAnimsUsedByRequestedModels",0x407AD0,0,-1,256},{"CStreaming","AreTexturesUsedByRequestedModels",0x409A90,0,-1,256},
    {"CStreaming","ClearFlagForAll",0x407A40,0,-1,128},{"CStreaming","ClearSlots",0x40BAA0,0,-1,256},{"CStreaming","ConvertBufferToObject",0x40C6B0,0,-1,512},{"CStreaming","DeleteAllRwObjects",0x4090A0,0,-1,128},
    {"CStreaming","DeleteLeastUsedEntityRwObject",0x409760,0,-1,256},{"CStreaming","DeleteRwObjectsAfterDeath",0x409210,0,-1,128},{"CStreaming","DeleteRwObjectsBehindCamera",0x40D7C0,0,-1,256},
    {"CStreaming","DeleteRwObjectsBehindCameraInSectorList<CPtrListSingleLink<CEntity*>>",0x409940,0,-1,256},{"CStreaming","DeleteRwObjectsInSectorList<CPtrListSingleLink<CEntity*>>",0x407A70,0,-1,256},
    {"CStreaming","DeleteRwObjectsNotInFrustumInSectorList<CPtrListSingleLink<CEntity*>>",0x4099E0,0,-1,256},{"CStreaming","DisableCopBikes",0x407D10,0,-1,64},{"CStreaming","FindMIPedSlotForInterior",0x407FB0,0,-1,128},
    {"CStreaming","FinishLoadingLargeFile",0x408CB0,0,-1,256},{"CStreaming","FlushChannels",0x40E460,0,-1,128},{"CStreaming","FlushRequestList",0x40E4E0,0,-1,128},{"CStreaming","GetDefaultCabDriverModel",0x407D50,0,-1,128},
    {"CStreaming","GetDefaultCopCarModel",0x407C50,0,-1,256},{"CStreaming","GetDefaultCopModel",0x407C00,0,-1,128},{"CStreaming","GetDefaultFiremanModel",0x407D40,0,-1,64},{"CStreaming","GetDefaultMedicModel",0x407D20,0,-1,64},
    {"CStreaming","GetNextFileOnCd",0x408E20,0,-1,512},{"CStreaming","HasSpecialCharLoaded",0x407F00,0,-1,64},{"CStreaming","HasVehicleUpgradeLoaded",0x407820,0,-1,256},{"CStreaming","Init2",0x5B8AD0,0,-1,64},
    {"CStreaming","InitImageList",0x4083C0,0,-1,64},{"CStreaming","InstanceLoadedModels",0x4084F0,0,-1,128},{"CStreaming","IsCarModelNeededInCurrentZone",0x407DD0,0,-1,512},{"CStreaming","IsVeryBusy",0x4076A0,0,-1,64},
    {"CStreaming","Load",0x5D29E0,0,-1,64},{"CStreaming","LoadAllRequestedModels",0x40EA10,0,-1,256},{"CStreaming","LoadCdDirectory-char",0x5B6170,0,-1,512},{"CStreaming","LoadCdDirectory-void",0x5B82C0,0,-1,128},
    {"CStreaming","LoadInitialPeds",0x40D3D0,0,-1,64},{"CStreaming","LoadInitialWeapons",0x40A120,0,-1,64},{"CStreaming","LoadRequestedModels",0x40E3A0,0,-1,256},{"CStreaming","LoadScene",0x40EB70,0,-1,128},
    {"CStreaming","LoadSceneCollision",0x40ED80,0,-1,64},{"CStreaming","LoadZoneVehicle",0x40B4B0,0,-1,128},{"CStreaming","MakeSpaceFor",0x40E120,0,-1,256},{"CStreaming","PossiblyStreamCarOutAfterCreation",0x40BA70,0,-1,128},
    {"CStreaming","ProcessEntitiesInSectorList<CPtrListSingleLink<CEntity*>>-",0x40C270,0,-1,512},{"CStreaming","ProcessLoadingChannel",0x40E170,0,-1,512},{"CStreaming","PurgeRequestList",0x40C1E0,0,-1,128},
    {"CStreaming","ReInit",0x40E560,0,-1,64},{"CStreaming","ReadIniFile",0x5BCCD0,0,-1,256},{"CStreaming","ReclassifyLoadedCars",0x40AFA0,0,-1,128},{"CStreaming","RemoveAllUnusedModels",0x40CF80,0,-1,128},
    {"CStreaming","RemoveBigBuildings",0x4093B0,0,-1,128},{"CStreaming","RemoveBuildingsNotInArea",0x4094B0,0,-1,256},{"CStreaming","RemoveCarModel",0x4080F0,0,-1,256},{"CStreaming","RemoveCurrentZonesModels",0x40B080,0,-1,128},
    {"CStreaming","RemoveDodgyPedsFromRandomSlots",0x40BE60,0,-1,128},{"CStreaming","RemoveEntity",0x409710,0,-1,256},{"CStreaming","RemoveInappropriatePedModels",0x40B3A0,0,-1,128},{"CStreaming","RemoveLeastUsedModel",0x40CFD0,0,-1,256},
    {"CStreaming","RemoveLoadedVehicle",0x40C020,0,-1,256},{"CStreaming","RemoveLoadedZoneModel",0x40B340,0,-1,128},{"CStreaming","RemoveModel",0x4089A0,0,-1,512},{"CStreaming","RemoveReferencedTxds",0x40D2F0,0,-1,128},
    {"CStreaming","RemoveTxdModel",0x40C180,0,-1,128},{"CStreaming","RenderEntity",0x4096D0,0,-1,256},{"CStreaming","RequestBigBuildings",0x409430,0,-1,128},{"CStreaming","RequestFile",0x40A080,0,-1,256},
    {"CStreaming","RequestFilesInChannel",0x409050,0,-1,128},{"CStreaming","RequestModel",0x4087E0,0,-1,512},{"CStreaming","RequestModelStream",0x40CBA0,0,-1,512},{"CStreaming","RequestSpecialChar",0x40B450,0,-1,256},
    {"CStreaming","RequestSpecialModel",0x409D10,0,-1,512},{"CStreaming","RequestTxdModel",0x407100,0,-1,256},{"CStreaming","RequestVehicleUpgrade",0x408C70,0,-1,128},{"CStreaming","RetryLoadFile",0x4076C0,0,-1,256},
    {"CStreaming","Save",0x5D29A0,0,-1,64},{"CStreaming","SetMissionDoesntRequireModel",0x409C90,0,-1,256},{"CStreaming","SetModelIsDeletable",0x409C10,0,-1,256},{"CStreaming","Shutdown",0x4084B0,0,-1,64},
    {"CStreaming","StreamAmbulanceAndMedic",0x40A2A0,0,-1,128},{"CStreaming","StreamCopModels",0x40A150,0,-1,256},{"CStreaming","StreamFireEngineAndFireman",0x40A400,0,-1,128},{"CStreaming","StreamOneNewCar",0x40B4F0,0,-1,512},
    {"CStreaming","StreamPedsForInterior",0x40BBB0,0,-1,256},{"CStreaming","StreamPedsIntoRandomSlots",0x40BDA0,0,-1,256},{"CStreaming","StreamVehiclesAndPeds",0x40B700,0,-1,256},{"CStreaming","StreamVehiclesAndPeds_Always",0x40B650,0,-1,256},
    {"CStreaming","StreamZoneModels",0x40A560,0,-1,512},{"CStreaming","StreamZoneModels_Gangs",0x40AA10,0,-1,512},{"CStreaming","Update",0x40E670,0,-1,256},{"CStreaming","WeAreTryingToPhaseVehicleOut",0x407F80,0,-1,128},
}};

// Registra los dobles (direccion del EXE + simbolo/cuerpo de la DLL). Devuelve el nombre de lo que falte.
inline std::string RegisterStubs(){
    stubs.clear(); std::string miss;
    auto hk=[&](uint64 m,uintptr_t exe,const char* cls,const char* name,void* fn){ if(!AddHookStub(m,exe,cls,name,fn))miss+=std::string(cls)+"::"+name+" "; };
    // azar, memoria, pad
    AddStub(E_RNG,0x821B1E,bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FnPtr(&S_Rand));
    AddStub(E_NEW,0x82119A,0,FnPtr(&S_New)); AddStub(E_NEW,0x821195,0,FnPtr(&S_New)); AddStub(E_NEW,0x8214BD,0,FnPtr(&S_Delete)); AddStub(E_NEW,0x8213AE,0,FnPtr(&S_Delete));
    AddStub(E_MEM,0x72F4C0,0,FnPtr(&S_Malloc)); AddStub(E_MEM,0x72F4F0,bit_cast<uintptr_t>(&CMemoryMgr::FreeAlign),FnPtr(&S_FreeAlign));
    AddStub(E_PAD,0x541D70,bit_cast<uintptr_t>(&CPad::StopPadsShaking),FnPtr(&S_PadStop));
    // CdStream
    hk(E_CD,0x4063E0,"CdStream","CdStreamGetStatus",FnPtr(&S_CdStat)); hk(E_CD,0x406460,"CdStream","CdStreamSync",FnPtr(&S_CdSync));
    hk(E_CD,0x406A20,"CdStream","CdStreamRead",FnPtr(&S_CdRead)); hk(E_CD,0x4067B0,"CdStream","CdStreamOpen",FnPtr(&S_CdOpen));
    // TXD
    hk(E_TXD,0x731A00,"CTxdStore","AddRef",FnPtr(&S_TxdAddRef)); hk(E_TXD,0x731A70,"CTxdStore","RemoveRefWithoutDelete",FnPtr(&S_TxdRmRef));
    hk(E_TXD,0x7319C0,"CTxdStore","SetCurrentTxd",FnPtr(&S_TxdSetCur)); hk(E_TXD,0x731E90,"CTxdStore","RemoveTxd",FnPtr(&S_TxdRemove));
    hk(E_TXD,0x731DD0,"CTxdStore","LoadTxd-stream",FnPtr(&S_TxdLoad)); hk(E_TXD,0x731930,"CTxdStore","StartLoadTxd",FnPtr(&S_TxdStart));
    hk(E_TXD,0x731E40,"CTxdStore","FinishLoadTxd",FnPtr(&S_TxdFinish)); hk(E_TXD,0x731850,"CTxdStore","FindTxdSlot-name",FnPtr(&S_TxdFind)); hk(E_TXD,0x731C80,"CTxdStore","AddTxdSlot",FnPtr(&S_TxdAdd));
    // animaciones
    hk(E_ANIM,0x4D3FB0,"CAnimManager","AddAnimBlockRef",FnPtr(&S_AnimAddRef)); hk(E_ANIM,0x4D3FF0,"CAnimManager","RemoveAnimBlockRefWithoutDelete",FnPtr(&S_AnimRmRef));
    hk(E_ANIM,0x4D3F40,"CAnimManager","RemoveAnimBlock",FnPtr(&S_AnimRemove)); hk(E_ANIM,0x4D47F0,"CAnimManager","LoadAnimFile",FnPtr(&S_AnimLoad));
    hk(E_ANIM,0x4D3CC0,"CAnimManager","CreateAnimAssocGroups",FnPtr(&S_AnimCreate)); hk(E_ANIM,0x4D3E50,"CAnimManager","RegisterAnimBlock",FnPtr(&S_AnimReg));
    // colision / IPL
    hk(E_COL,0x4106D0,"CColStore","LoadCol-2",FnPtr(&S_ColLoad)); hk(E_COL,0x410730,"CColStore","RemoveCol",FnPtr(&S_ColRemove)); hk(E_COL,0x410860,"CColStore","LoadCollision",FnPtr(&S_ColLoadColl));
    hk(E_COL,0x410AD0,"CColStore","EnsureCollisionIsInMemory",FnPtr(&S_ColEnsure)); hk(E_COL,0x4103A0,"CColStore","AddCollisionNeededAtPosn",FnPtr(&S_ColAddNeeded));
    hk(E_COL,0x411140,"CColStore","AddColSlot",FnPtr(&S_ColAddSlot));
    hk(E_IPL,0x406080,"CIplStore","LoadIpl",FnPtr(&S_IplLoad)); hk(E_IPL,0x404B20,"CIplStore","RemoveIpl",FnPtr(&S_IplRemove)); hk(E_IPL,0x405170,"CIplStore","LoadIpls",FnPtr(&S_IplLoadIpls));
    hk(E_IPL,0x4053F0,"CIplStore","EnsureIplsAreInMemory",FnPtr(&S_IplEnsure)); hk(E_IPL,0x4045B0,"CIplStore","AddIplsNeededAtPosn",FnPtr(&S_IplAddNeeded));
    hk(E_IPL,0x404AC0,"CIplStore","FindIplSlot",FnPtr(&S_IplFind)); hk(E_IPL,0x405AC0,"CIplStore","AddIplSlot",FnPtr(&S_IplAdd));
    // rutas, scripts
    hk(E_PATH,0x4529F0,"CPathFind","LoadPathFindData-FromStream",FnPtr(&S_PathLoad)); hk(E_PATH,0x44D0F0,"CPathFind","UnLoadPathFindData",FnPtr(&S_PathUnload));
    hk(E_PATH,0x450DE0,"CPathFind","IsWaterNodeNearby",FnPtr(&S_PathWater)); hk(E_PATH,0x44DE00,"CPathFind","LoadSceneForPathNodes",FnPtr(&S_PathScene));
    hk(E_SCRIPT,0x4708E0,"CStreamedScripts","RemoveStreamedScriptFromMemory",FnPtr(&S_ScrRemove)); hk(E_SCRIPT,0x470840,"CStreamedScripts","LoadStreamedScript",FnPtr(&S_ScrLoad));
    hk(E_SCRIPT,0x4706C0,"CStreamedScripts","RegisterScript",FnPtr(&S_ScrReg)); hk(E_SCRIPT,0x4706A0,"CStreamedScripts","ReInitialise",FnPtr(&S_ScrReinit));
    hk(E_SCRIPT,0x470660,"CStreamedScripts","Initialise",FnPtr(&S_ScrInit));
    // RenderWare / Rt (solo EXE)
    AddStub(E_RW,0x7EC810,0,FnPtr(&S_RwInit)); AddStub(E_RW,0x7ECE20,0,FnPtr(&S_RwClose)); AddStub(E_RW,0x7ED590,0,FnPtr(&S_RwChunk)); AddStub(E_RW,0x7CF240,0,FnPtr(&S_RtRead));
    AddStub(E_RW,0x7CEEF0,0,FnPtr(&S_RtSetCur)); AddStub(E_RW,0x7CF130,0,FnPtr(&S_RtDestroy)); AddStub(E_RW,0x72E700,0,FnPtr(&S_ClumpCancel));
    // cargadores
    hk(E_LOADER,0x5372D0,"CFileLoader","LoadClumpFile-1",FnPtr(&S_LdClump)); hk(E_LOADER,0x5371F0,"CFileLoader","LoadAtomicFile-stream",FnPtr(&S_LdAtomic));
    hk(E_LOADER,0x537450,"CFileLoader","FinishLoadClumpFile",FnPtr(&S_LdFinish));
    hk(E_REC,0x45A8F0,"CVehicleRecording","Load",FnPtr(&S_RecLoad)); hk(E_REC,0x459F80,"CVehicleRecording","RegisterRecordingFile",FnPtr(&S_RecReg));
    // entidades y mundo
    hk(E_ENT,0x533050,"CEntity","LivesInThisNonOverlapSector",FnPtr(&S_Lives)); hk(E_ENT,0x536BC0,"CEntity","IsVisible",FnPtr(&S_Visible)); hk(E_ENT,0x534540,"CEntity","GetIsOnScreen",FnPtr(&S_OnScreen));
    hk(E_ENT,0x59F120,"CObject","CanBeDeleted",FnPtr(&S_CanDelete)); hk(E_ENT,0x5DF8F0,"CPed","IsPlayer",FnPtr(&S_IsPlayer)); hk(E_ENT,0x5DF150,"CPed","CanBeDeletedEvenInVehicle",FnPtr(&S_CanDeleteVeh));
    hk(E_ENT,0x486240,"CTheScripts","RemoveThisPed",FnPtr(&S_RemovePed)); hk(E_ENT,0x563280,"CWorld","Remove",FnPtr(&S_WorldRemove));
    hk(E_ENT,0x565510,"CWorld","RemoveReferencesToDeletedObject",FnPtr(&S_WorldRemRef));
    // poblacion
    hk(E_POP,0x6117F0,"CPopulation","LoadSpecificDriverModelsForCar",FnPtr(&S_LoadDrv)); hk(E_POP,0x6119D0,"CPopulation","RemoveSpecificDriverModelsForCar",FnPtr(&S_RemDrv));
    hk(E_POP,0x6160A0,"CPopulation","ManageAllPopulation",FnPtr(&S_Manage)); hk(E_POP,0x60FFD0,"CPopCycle","PickPedMIToStreamInForCurrentZone",FnPtr(&S_PickPed));
    hk(E_POP,0x610420,"CPopCycle","PickARandomGroupOfOtherPeds",FnPtr(&S_PickGrp)); hk(E_POP,0x610150,"CPopCycle","IsPedAppropriateForCurrentZone",FnPtr(&S_Approp));
    hk(E_POP,0x421900,"CCarCtrl","ChooseCarModelToLoad",FnPtr(&S_Choose)); hk(E_POP,0x443D50,"CGangWars","TellStreamingWhichGangsAreNeeded",FnPtr(&S_GangsNeeded));
    hk(E_WANTED,0x561F40,"CWanted","AreSwatRequired",FnPtr(&S_Swat)); hk(E_WANTED,0x561F60,"CWanted","AreFbiRequired",FnPtr(&S_Fbi)); hk(E_WANTED,0x561F80,"CWanted","AreArmyRequired",FnPtr(&S_Army));
    hk(E_WANTED,0x561FA0,"CWanted","NumOfHelisRequired",FnPtr(&S_Heli));
    hk(E_ZONES,0x572400,"CTheZones","GetZoneInfo-",FnPtr(&S_ZoneInfo)); hk(E_ZONES,0x572300,"CTheZones","GetLevelFromPosition",FnPtr(&S_ZoneLevel));
    hk(E_PLAYER,0x56E210,"CWorld","FindPlayerPed",FnPtr(&S_FindPed)); hk(E_PLAYER,0x56E0D0,"CWorld","FindPlayerVehicle",FnPtr(&S_FindVeh));
    hk(E_PLAYER,0x56E230,"CWorld","FindPlayerWanted",FnPtr(&S_FindWanted)); hk(E_PLAYER,0x56E010,"CWorld","FindPlayerCoors",FnPtr(&S_FindCoors));
    hk(E_CAM,0x514B80,"CCamera","CalculateGroundHeight",FnPtr(&S_Ground)); hk(E_REND,0x554EB0,"CRenderer","ShouldModelBeStreamed",FnPtr(&S_Should));
    hk(E_MISC,0x590310,"CLoadingScreen","Pause",FnPtr(&S_LsPause)); hk(E_MISC,0x590320,"CLoadingScreen","Continue",FnPtr(&S_LsCont));
    hk(E_MISC,0x5858D0,"CRadar","StreamRadarSections-vec",FnPtr(&S_Radar));
    hk(E_DIR,0x5324A0,"CDirectory","FindItem-ByName",FnPtr(&S_DirFind)); hk(E_DIR,0x532310,"CDirectory","AddItem-",FnPtr(&S_DirAdd));
    // CDirectory::CDirectory(size_t) y ~CDirectory NO se doblan: `new CDirectory(n)` / `delete` en Streaming.cpp llaman al constructor y destructor de la DLL directamente
    // (no al simbolo envoltorio `Constructor` que es el unico que el parche de la DLL intercepta), asi que el doble solo actuaba en el lado EXE y la DLL reservaba 550*32 bytes
    // de mas en la arena. Ambos lados ejecutan ahora su codigo real y sus reservas pasan por los dobles de operator new/delete.
    hk(E_STORE,0x5D1270,"CGenericGameStorage","SaveDataToWorkBuffer-org",FnPtr(&S_SaveData)); hk(E_STORE,0x5D1300,"CGenericGameStorage","LoadDataFromWorkBuffer-org",FnPtr(&S_LoadData));
    hk(E_FILE,0x538900,"CFileMgr","OpenFile",FnPtr(&S_FileOpen)); hk(E_FILE,0x538950,"CFileMgr","Read",FnPtr(&S_FileRead)); hk(E_FILE,0x5389D0,"CFileMgr","CloseFile",FnPtr(&S_FileClose));
    hk(E_FILE,0x536F80,"CFileLoader","LoadLine-File",FnPtr(&S_FileLine)); hk(E_FILE,0x4C5940,"CModelInfo","GetModelInfo-full",FnPtr(&S_GetMi));
    hk(E_FILE,0x4C9360,"CVehicleModelInfo","AssignRemapTxd",FnPtr(&S_AssignRemap));
    hk(E_MI,0x4C4B40,"CBaseModelInfo","SetTexDictionary",FnPtr(&S_MiSetTxd)); hk(E_MI,0x4C4B70,"CBaseModelInfo","ClearTexDictionary",FnPtr(&S_MiClrTxd));
    hk(E_MI,0x4C4BB0,"CBaseModelInfo","RemoveRef",FnPtr(&S_MiRemRef));
    // hermanas de CStreaming
    hk(B_REMOVEMODEL,0x4089A0,CSTREAMING,"RemoveModel",FnPtr(&B_RemoveModel)); hk(B_DELETEALL,0x4090A0,CSTREAMING,"DeleteAllRwObjects",FnPtr(&B_DeleteAll));
    hk(B_REQBIG,0x409430,CSTREAMING,"RequestBigBuildings",FnPtr(&B_ReqBig)); hk(B_LOADALL,0x40EA10,CSTREAMING,"LoadAllRequestedModels",FnPtr(&B_LoadAll));
    hk(B_ADDMODELS,0x40D3F0,CSTREAMING,"AddModelsToRequestList",FnPtr(&B_AddModels)); hk(B_ADDLODS,0x40C520,CSTREAMING,"AddLodsToRequestList",FnPtr(&B_AddLods));
    hk(B_STREAMCOP,0x40A150,CSTREAMING,"StreamCopModels",FnPtr(&B_StreamCop)); hk(B_INSTANCE,0x4084F0,CSTREAMING,"InstanceLoadedModels",FnPtr(&B_Instance));
    hk(B_ALWAYS,0x40B650,CSTREAMING,"StreamVehiclesAndPeds_Always",FnPtr(&B_Always)); hk(B_STREAMVP,0x40B700,CSTREAMING,"StreamVehiclesAndPeds",FnPtr(&B_StreamVP));
    hk(B_STREAMZONE,0x40A560,CSTREAMING,"StreamZoneModels",FnPtr(&B_StreamZone)); hk(B_LOADREQ,0x40E3A0,CSTREAMING,"LoadRequestedModels",FnPtr(&B_LoadReq));
    hk(B_PURGE,0x40C1E0,CSTREAMING,"PurgeRequestList",FnPtr(&B_Purge)); hk(B_MAKESPACE,0x40E120,CSTREAMING,"MakeSpaceFor",FnPtr(&B_MakeSpace));
    hk(B_STREAMONE,0x40B4F0,CSTREAMING,"StreamOneNewCar",FnPtr(&B_StreamOne)); hk(B_RECLASS,0x40AFA0,CSTREAMING,"ReclassifyLoadedCars",FnPtr(&B_Reclass));
    hk(B_GANGS,0x40AA10,CSTREAMING,"StreamZoneModels_Gangs",FnPtr(&B_Gangs)); hk(B_REMOVELEAST,0x40CFD0,CSTREAMING,"RemoveLeastUsedModel",FnPtr(&B_RemoveLeast));
    hk(B_REMOVELOADEDVEH,0x40C020,CSTREAMING,"RemoveLoadedVehicle",FnPtr(&B_RemoveLoadedVeh)); hk(B_REMOVELOADEDZONE,0x40B340,CSTREAMING,"RemoveLoadedZoneModel",FnPtr(&B_RemoveLoadedZone));
    hk(B_DELETEBEHIND,0x40D7C0,CSTREAMING,"DeleteRwObjectsBehindCamera",FnPtr(&B_DeleteBehind)); hk(B_RETRY,0x4076C0,CSTREAMING,"RetryLoadFile",FnPtr(&B_Retry));
    hk(B_PROCESSCH,0x40E170,CSTREAMING,"ProcessLoadingChannel",FnPtr(&B_ProcessCh)); hk(B_REQSTREAM,0x40CBA0,CSTREAMING,"RequestModelStream",FnPtr(&B_ReqStream));
    hk(B_DELETELEAST,0x409760,CSTREAMING,"DeleteLeastUsedEntityRwObject",FnPtr(&B_DeleteLeast)); hk(B_LOADCDDIR,0x5B82C0,CSTREAMING,"LoadCdDirectory-void",FnPtr(&B_LoadCdDir));
    hk(B_FLUSHCH,0x40E460,CSTREAMING,"FlushChannels",FnPtr(&B_FlushCh)); hk(B_FLUSHREQ,0x40E4E0,CSTREAMING,"FlushRequestList",FnPtr(&B_FlushReq));
    hk(B_DELETEAFTER,0x409210,CSTREAMING,"DeleteRwObjectsAfterDeath",FnPtr(&B_DeleteAfter));
    return miss;
}

// ---------------------------------------------------------------------------------------------------------------------
// Captura del estado tras cada ejecucion: cada region se copia y se resume (FNV-64) para comparar y localizar diferencias
// ---------------------------------------------------------------------------------------------------------------------
struct CapReg { std::string tag; const uint8* p; size_t n; };
inline std::vector<CapReg> capRegs;
inline std::array<std::vector<uint8>,2> capBuf;
inline std::array<std::vector<uint64>,2> capHash;
inline std::array<std::vector<Rec>,2> recCopy; inline std::array<size_t,2> recN{};
inline std::array<std::array<uint8,0x10000>,2> arenaDump{}; inline std::array<size_t,2> arenaTops{};
inline void AddCap(const char* tag,const void* p,size_t n){ capRegs.push_back({tag,static_cast<const uint8*>(p),n}); }
inline uint64 Capture(bool cand){
    const size_t s=cand ? 1 : 0; auto& buf=capBuf[s]; auto& hs=capHash[s]; hs.clear(); size_t off=0; uint64 comb=1469598103934665603ull;
    for(auto& r:capRegs){ std::memcpy(buf.data()+off,r.p,r.n); const uint64 h=FastHash(r.p,r.n); hs.push_back(h); off+=r.n; comb^=h; comb*=1099511628211ull; }
    arenaTops[s]=arenaTop; std::memcpy(arenaDump[s].data(),arena.data(),std::min<size_t>(arenaTop,arenaDump[s].size()));
    recN[s]=nrec; std::memcpy(recCopy[s].data(),recs.data(),nrec*sizeof(Rec));
    return comb;
}
// Resumen compacto y determinista (nunca nulo, tambien para funciones void)
inline json Summary(const json& o){
    return json{{"ret",o["ret"]},{"events",o["events"]},{"trace_fnv",o["trace_fnv"]},{"world",o["world"]},{"arena_top",o["arena_top"]},{"arena_fnv",o["arena_fnv"]}};
}

// Comprobacion de la disposicion de la DLL frente al EXE (solo direcciones, sin leer nada)
inline bool Layout(){
    bool ok=true; json l=json::object();
    auto chk=[&](const char* n,bool good){ ok&=good; l[n]=good; };
    chk("sizeof_info",sizeof(CStreamingInfo)==20); chk("sizeof_channel",sizeof(tStreamingChannel)==0x98); chk("sizeof_filedesc",sizeof(tStreamingFileDesc)==0x30);
    chk("sizeof_cargroup",sizeof(CLoadedCarGroup)==0x2E); chk("sizeof_link",sizeof(CLink<CEntity*>)==12); chk("sizeof_linklist",sizeof(CLinkList<CEntity*>)==0x34);
    chk("addr_info",uintptr_t(&CStreaming::ms_aInfoForModel)==INFO); chk("addr_channel",uintptr_t(&CStreaming::ms_channel)==0x8E4A60); chk("addr_files",uintptr_t(&CStreaming::ms_files)==0x8E48D8);
    chk("addr_peds",uintptr_t(&CStreaming::ms_pedsLoaded)==0x8E4C00); chk("addr_vehicles",uintptr_t(&CStreaming::ms_vehiclesLoaded)==0x8E4C24);
    chk("addr_instances",uintptr_t(&CStreaming::ms_rwObjectInstances)==0x9654F0); chk("addr_boats",uintptr_t(&CPopulation::m_LoadedBoats)==0xC0E998);
    chk("addr_sectors",uintptr_t(&CWorld::ms_aSectors)==SECTORS); chk("addr_txdpool",uintptr_t(&CTxdStore::ms_pTxdPool)==TXDPOOL); chk("addr_miptrs",uintptr_t(&CModelInfo::ms_modelInfoPtrs)==MIPTRS);
    chk("addr_players",uintptr_t(&CWorld::Players)==PLAYERS); chk("addr_cheats",uintptr_t(&CCheat::m_aCheatsActive)==0x969130); chk("addr_currarea",uintptr_t(&CGame::currArea)==CURRAREA);
    chk("addr_carbound",uintptr_t(&CPopulation::m_CarGroups)==0xC0ED38); chk("addr_pedgroups",uintptr_t(&CPopulation::m_PedGroups)==0xC0F358);
    chk("v_model_type_vehicle",int(MODEL_INFO_VEHICLE)==6); chk("v_model_type_ped",int(MODEL_INFO_PED)==7); chk("v_cargroup_ballas",int(POPCYCLE_CARGROUP_BALLAS)==18);
    chk("v_cargroup_boats",int(POPCYCLE_CARGROUP_BOATS)==29); chk("v_load_loaded",int(LOADSTATE_LOADED)==1); chk("v_load_finishing",int(LOADSTATE_FINISHING)==4);
    chk("v_cheat_beach",int(CHEAT_BEACH_PARTY)==41); chk("v_cheat_country",int(CHEAT_COUNTRY_TRAFFIC)==75); chk("v_cheat_gangs",int(CHEAT_GANGS_CONTROLS_THE_STREETS)==43);
    chk("v_object_game",int(eObjectType::OBJECT_GAME)==1); chk("v_area13",int(AREA_CODE_13)==13);
    { auto* e=reinterpret_cast<CEntity*>(Bld(0)); chk("ent_flags",uintptr_t(&e->m_nFlags)-uintptr_t(e)==0x1C); chk("ent_model",uintptr_t(&e->m_nModelIndex)-uintptr_t(e)==0x22);
      chk("ent_matrix",uintptr_t(&e->m_matrix)-uintptr_t(e)==0x14); chk("ent_placement",uintptr_t(&e->m_placement)-uintptr_t(e)==4); }
    { auto* v=reinterpret_cast<CVehicleModelInfo*>(miMem[0].data()); chk("vmi_flags",uintptr_t(&v->m_nFlags)-uintptr_t(v)==0x4E); chk("vmi_timesused",uintptr_t(&v->m_nTimesUsed)-uintptr_t(v)==0x50); }
    { auto* m=reinterpret_cast<CBaseModelInfo*>(miMem[0].data()); chk("mi_key",uintptr_t(&m->m_nKey)-uintptr_t(m)==4); chk("mi_txd",uintptr_t(&m->m_nTxdIndex)-uintptr_t(m)==0xA);
      chk("mi_ref",uintptr_t(&m->m_nRefCount)-uintptr_t(m)==8); chk("mi_draw",uintptr_t(&m->m_fDrawDistance)-uintptr_t(m)==0x18);
      std::memset(colMem[0].data(),0,0x30); At<float>(colMem[0].data(),0x24)=17.5f; chk("col_radius",reinterpret_cast<CColModel*>(colMem[0].data())->GetBoundRadius()==17.5f); }
    Write({{"event","layout"},{"ok",ok},{"checks",l}});
    return ok;
}

inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"streamingfinal4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"streamingfinal4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("streamingfinal4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"streamingfinal4-only.txt"); if(f)std::getline(f,only); }

    std::array<ReversibleHooks::ReversibleHook::Base*,94> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    if(!Layout()){ Write({{"event","error"},{"reason","layout"}}); return; }
    const auto miss=RegisterStubs(); if(!miss.empty()){ Write({{"event","error"},{"reason","stubs not found"},{"missing",miss}}); return; }

    // pools de nodos locales (cabecera construida una sola vez)
    new (sglHdr.data()) CPtrNodeSingleLinkPool(NSGL,sglStore.data(),sglSt.data()); sglImg=sglHdr;
    new (dblHdr.data()) CPtrNodeDoubleLinkPool(NDBL,dblStore.data(),dblSt.data()); dblImg=dblHdr;
    InitVtables();
    nregs=0;
    Reg(miMem.data(),sizeof(miMem)); Reg(atomMem.data(),sizeof(atomMem)); Reg(bldMem.data(),bldMem.size()); Reg(dumMem.data(),dumMem.size()); Reg(objMem.data(),objMem.size()); Reg(pedMem.data(),pedMem.size());
    Reg(sglStore.data(),sglStore.size()); Reg(dblStore.data(),dblStore.size()); Reg(linkMem.data(),sizeof(linkMem)); Reg(vehMem.data(),vehMem.size()); Reg(txdMem.data(),txdMem.size());
    Reg(streamBuf0.data(),streamBuf0.size()); Reg(streamBuf1.data(),streamBuf1.size()); Reg(arena.data(),arena.size()); Reg(rwObjs.data(),sizeof(rwObjs)); Reg(dirMem.data(),dirMem.size());
    Reg(wantedMem.data(),wantedMem.size()); Reg(zoneInfoMem.data(),zoneInfoMem.size()); Reg(colMem.data(),sizeof(colMem)); Reg(timeMem.data(),sizeof(timeMem)); Reg(rwCamMem.data(),rwCamMem.size());

    // instantanea de globales (restauradas al final de cada ejecucion)
    saved.clear();
    Save(0x8A5A80,0x8C,"cs_defaults"); Save(0x8E4898,0x428,"cs_state"); Save(INFO,0x80880,"cs_info"); Save(POOLS,0x20,"pools"); Save(0xB7CD60,0x20,"world_globals");
    Save(PLAYERS,0x320,"players"); Save(SECTORS,SECTORS_SIZE,"sectors"); Save(REPEAT,REPEAT_SIZE,"repeat_sectors"); Save(LODL,LOD_SIZE,"lod_lists"); Save(CAMERA,0xD78,"camera");
    Save(MIPTRS,80000,"model_ptrs"); Save(TXDPOOL,4,"txd_pool_ptr"); Save(ANIMBLK-0x10,0x20*NANIM+0x20,"anim_blocks"); Save(0xC0E998,0x1340,"population"); Save(0x8D2540,0x1B0,"pop_translation");
    Save(0xC0BC68,0x2D28,"popcycle"); Save(CURRLEVEL,4,"zone_level"); Save(0x969130,0x60,"cheats"); Save(0xB76848,0xC,"renderer"); Save(0xB745C1,1,"render_tunnels"); Save(0x8CD804,4,"lod_scale");
    Save(CURRAREA,4,"curr_area"); Save(0xB7CB48,0x48,"timer"); Save(0xA43088,4,"replay"); Save(0xB5F84C,0x10,"cutscene"); Save(0xA47B60,0xA40,"streamed_scripts"); Save(0x8CD6E4,0x10,"harvester_ids");
    Save(0xB4E6D8,0x80,"linked_upgrades"); Save(0xB72978,0x80,"load_monitor"); Save(0x8D5150,0x24,"post_effects"); Save(0xC402CC,1,"radiosity"); Save(0xBA6784,4,"brightness"); Save(0x96F016,1,"dont_build_paths");
    Save(0xC87B1C,4,"mem_id"); Save(0xC87C18,0x48,"mem_id_stack");
    capRegs.clear();
    for(auto& s:saved)AddCap(s.tag,reinterpret_cast<const void*>(s.addr),s.bytes.size());
    AddCap("fake_model_infos",miMem.data(),sizeof(miMem)); AddCap("fake_atomic_infos",atomMem.data(),sizeof(atomMem)); AddCap("fake_buildings",bldMem.data(),bldMem.size()); AddCap("fake_building_states",bldSt.data(),bldSt.size());
    AddCap("fake_dummies",dumMem.data(),dumMem.size()); AddCap("fake_objects",objMem.data(),objMem.size()); AddCap("fake_peds",pedMem.data(),pedMem.size()); AddCap("fake_ped_states",pedSt.data(),pedSt.size());
    AddCap("fake_sgl_nodes",sglStore.data(),sglStore.size()); AddCap("fake_dbl_nodes",dblStore.data(),dblStore.size()); AddCap("fake_links",linkMem.data(),sizeof(linkMem));
    AddCap("fake_txd",txdMem.data(),txdMem.size()); AddCap("fake_vehicle",vehMem.data(),vehMem.size()); AddCap("fake_zone",zoneInfoMem.data(),zoneInfoMem.size()); AddCap("fake_dir",dirMem.data(),dirMem.size());
    AddCap("fake_wanted",wantedMem.data(),wantedMem.size());
    size_t total=0; for(auto& r:capRegs)total+=r.n;
    for(auto& b:capBuf)b.assign(total,0); for(auto& r:recCopy)r.assign(recs.size(),Rec{});

    std::array<bool,94> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    const unsigned savedFP=_controlfp(0,0);
    bool passed=true; int pair=0;
    // vigilante: si una ejecucion supera 8 s sin terminar, registra el evento 'watchdog' con la funcion, el caso y el lado, y termina el proceso
    static std::atomic<ULONGLONG> wdDeadline{0}; static std::atomic<int> wdCase{-1}; static std::atomic<int> wdSide{0}; static std::atomic<const char*> wdFn{""};
    static std::atomic<bool> wdStarted{false};
    if(!wdStarted.exchange(true)){
        std::thread([](){ for(;;){ Sleep(250); const ULONGLONG d=wdDeadline.load();
            if(d&&GetTickCount64()>d){ wdDeadline.store(0);
                Write({{"event","watchdog"},{"function",std::string(wdFn.load())},{"case",wdCase.load()},{"candidate",wdSide.load()==1},{"accepted",false},{"equal",false}});
                std::fflush(nullptr); TerminateProcess(GetCurrentProcess(),3); } } }).detach();
    }
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                Ctx c; c.fn=&fn; c.nm=fn.name; c.i=i; c.f=uint32(f); c.candidate=candidate;
                c.seed=Mix(uint32(i)*2654435761u^0x56E1u,uint32(f+1)*0x9E3779B9u); if(!c.seed)c.seed=1; c.r.st=c.seed;
                RestoreAll();
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                c.a=fn.address; c.out=json::object(); c.ret=nullptr;
                _controlfp(_PC_24,_MCW_PC);
                wdFn.store(fn.name); wdCase.store(i); wdSide.store(candidate ? 1 : 0); wdDeadline.store(GetTickCount64()+8000);
                const bool handled=RunA(c)||RunB(c)||RunC(c)||RunD(c);
                wdDeadline.store(0);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                json out=c.out; out["switches"]=switches; out["handled"]=handled; out["ret"]=c.ret; out["events"]=nrec;
                out["trace_fnv"]=Fnv64(recs.data(),nrec*sizeof(Rec)); out["world"]=Capture(candidate);
                out["arena_top"]=arenaTop; out["arena_fnv"]=FastHash(arena.data(),std::min(arenaTop,arena.size()));
                RestoreAll();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["handled"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["events"]}});
            else {
                json diff=json::array(), vals=json::object(); for(auto& [k,v]:original.items())if(candidate[k]!=v){ diff.push_back(k); vals[k]={{"o",v.dump().substr(0,200)},{"c",candidate[k].dump().substr(0,200)}}; }
                // regiones de memoria que difieren (hasta 8 bloques de 64 bytes en total)
                json blocks=json::array(); int shown=0; size_t off=0;
                for(size_t k=0;k<capRegs.size();++k){ const auto& r=capRegs[k];
                    if(capHash[0][k]!=capHash[1][k]){ for(size_t o=0;o<r.n&&shown<8;o+=64){ const size_t n=std::min<size_t>(64,r.n-o);
                        if(std::memcmp(capBuf[0].data()+off+o,capBuf[1].data()+off+o,n)!=0){ blocks.push_back({{"region",r.tag},{"offset",o},{"original",ReversingProbe::Bytes(capBuf[0].data()+off+o,std::min<size_t>(n,32))},{"candidate",ReversingProbe::Bytes(capBuf[1].data()+off+o,std::min<size_t>(n,32))}}); ++shown; } } }
                    off+=r.n; }
                // primer evento de traza distinto
                json tr=json::object(); { const size_t n=std::min(recN[0],recN[1]); size_t d=0; while(d<n&&std::memcmp(&recCopy[0][d],&recCopy[1][d],sizeof(Rec))==0)++d;
                    tr["first_diff"]=d; tr["counts"]=json::array({recN[0],recN[1]}); json wo=json::array(), wc=json::array();
                    for(size_t q=(d>2?d-2:0);q<d+4;++q){ if(q<recN[0])wo.push_back({recCopy[0][q].t,recCopy[0][q].a,recCopy[0][q].b,recCopy[0][q].c,recCopy[0][q].d}); if(q<recN[1])wc.push_back({recCopy[1][q].t,recCopy[1][q].a,recCopy[1][q].b,recCopy[1][q].c,recCopy[1][q].d}); }
                    tr["original"]=wo; tr["candidate"]=wc; }
                json regionsDiff=json::array(); for(size_t k=0;k<capRegs.size();++k)if(capHash[0][k]!=capHash[1][k])regionsDiff.push_back(capRegs[k].tag);
                json arenaBlocks=json::array(); { const size_t n=std::min({arenaTops[0],arenaTops[1],arenaDump[0].size()}); int sh=0; for(size_t b=0;b<n&&sh<4;b+=64){ const size_t m=std::min<size_t>(64,n-b);
                    if(std::memcmp(arenaDump[0].data()+b,arenaDump[1].data()+b,m)!=0){ arenaBlocks.push_back({{"offset",b},{"original",ReversingProbe::Bytes(arenaDump[0].data()+b,std::min<size_t>(m,32))},{"candidate",ReversingProbe::Bytes(arenaDump[1].data()+b,std::min<size_t>(m,32))}}); ++sh; } } }
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"diff_values",vals},{"diff_blocks",blocks},{"diff_regions",regionsDiff},
                       {"diff_trace",tr},{"diff_arena",arenaBlocks},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    const bool restored=AllRestored()&&_controlfp(0,0)==savedFP;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
