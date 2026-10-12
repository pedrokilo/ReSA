#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1): CVehicleModelInfo, 75 funciones (lista final 4).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato (hooks activos) sobre un mundo RW falso y
// determinista: model infos y CVehicleStructure en buffers propios (vtable copiada del EXE), clumps/frames/atomics/geometrias/
// materiales/texturas falsos con los offsets de plugin leidos del EXE, pool falso de CVehicleStructure (0xB4E680) y de TXD
// (0xC8800C). Las primitivas RW, CFileMgr/CFileLoader/CTxdStore, CModelInfo::GetModelInfo, el azar (0x821B1E +
// CGeneral::GetRandomNumber), operator new/delete y las subfunciones de CClumpModelInfo se simulan con trazas (direccion del EXE
// y simbolo de la DLL). Cada global tocado se guarda y se restaura en cada ejecucion. Ver README.md.
#include "CasesD.hpp"

namespace VehModelInfoFinal4Live {
#define VMI "CVehicleModelInfo"
inline const std::array<Fn,75> fns{{
    {"CVehicleModelInfo","AddRemap",0x4C86D0,0,-1,256},{"CVehicleModelInfo","AddWheelUpgrade",0x4C8700,0,-1,256},{"CVehicleModelInfo","AssignRemapTxd",0x4C9360,0,-1,256},
    {"CVehicleModelInfo","ChooseComponent",0x4C8040,0,-1,512},{"CVehicleModelInfo","ChooseComponent-Global",0x4C7FB0,0,-1,512},{"CVehicleModelInfo","ChooseSecondComponent",0x4C8120,0,-1,512},
    {"CVehicleModelInfo","ChooseVehicleColour",0x4C8500,0,-1,512},{"CVehicleModelInfo","ClearAtomicFlagCB",0x4C7BB0,0,-1,128},{"CVehicleModelInfo","CollapseFramesCB",0x4C8E30,0,-1,256},
    {"CVehicleModelInfo","ConvertAnimFileIndex",0,0x85C5C8,13,128},{"CVehicleModelInfo","CountCompsInRule",0x4C7F80,0,-1,256},{"CVehicleModelInfo","CreateInstance",0,0x85C5C8,11,512},
    {"CVehicleModelInfo","DeleteRwObject",0,0x85C5C8,8,256},{"CVehicleModelInfo","DeleteVehicleColourTextures",0x4C8770,0,-1,64},{"CVehicleModelInfo","DisableEnvMap",0x4C97E0,0,-1,128},
    {"CVehicleModelInfo","FindTextureCB",0x4C7510,0,-1,512},{"CVehicleModelInfo","GetAnimFileIndex",0,0x85C5C8,14,64},{"CVehicleModelInfo","GetCustomCarPlateText",0x4C8970,0,-1,128},
    {"CVehicleModelInfo","GetListOfComponentsNotUsedByRules",0x4C7E50,0,-1,512},{"CVehicleModelInfo","GetMatFXEffectMaterialCB",0x4C8810,0,-1,128},
    {"CVehicleModelInfo","GetMaximumNumberOfPassengersFromNumberOfDoors",0x4C89B0,0,-1,512},{"CVehicleModelInfo","GetModelType",0,0x85C5C8,4,64},{"CVehicleModelInfo","GetNumDoors",0x4C73C0,0,-1,256},
    {"CVehicleModelInfo","GetNumRemaps",0x4C86B0,0,-1,128},{"CVehicleModelInfo","GetNumWheelUpgrades",0x4C8740,0,-1,128},{"CVehicleModelInfo","GetOkAndDamagedAtomicCB",0x4C7BD0,0,-1,128},
    {"CVehicleModelInfo","GetOriginalCompPosition",0x4C7DD0,0,-1,128},{"CVehicleModelInfo","GetWheelPosn",0x4C7D20,0,-1,256},{"CVehicleModelInfo","GetWheelUpgrade",0x4C8750,0,-1,128},
    {"CVehicleModelInfo","HasAlphaMaterialCB",0x4C77C0,0,-1,128},{"CVehicleModelInfo","HideAllComponentsAtomicCB",0x4C7790,0,-1,128},{"CVehicleModelInfo","HideDamagedAtomicCB",0x4C7720,0,-1,256},
    {"CVehicleModelInfo","Init",0,0x85C5C8,6,64},{"CVehicleModelInfo","IsUpgradeAvailable",0x4C8200,0,-1,256},{"CVehicleModelInfo","IsValidCompRule",0x4C7E10,0,-1,256},
    {"CVehicleModelInfo","LoadEnvironmentMaps",0x4C8780,0,-1,128},{"CVehicleModelInfo","LoadVehicleColours",0x5B6890,0,-1,256},{"CVehicleModelInfo","LoadVehicleUpgrades",0x5B65A0,0,-1,256},
    {"CVehicleModelInfo","MoveObjectsCB",0x4C7700,0,-1,64},{"CVehicleModelInfo","PreprocessHierarchy",0x4C8E60,0,-1,512},{"CVehicleModelInfo","ReduceMaterialsInVehicle",0x4C8BD0,0,-1,256},
    {"CVehicleModelInfo","RemoveWindowAlphaCB",0x4C83B0,0,-1,128},{"CVehicleModelInfo","ResetEditableMaterials",0x4C8460,0,-1,128},{"CVehicleModelInfo","SetAnimFile",0,0x85C5C8,12,256},
    {"CVehicleModelInfo","SetAtomicFlagCB",0x4C7B90,0,-1,128},{"CVehicleModelInfo","SetAtomicRenderCallbacks",0x4C7B10,0,-1,128},{"CVehicleModelInfo","SetAtomicRendererCB",0x4C77E0,0,-1,256},
    {"CVehicleModelInfo","SetAtomicRendererCB_Boat",0x4C79A0,0,-1,128},{"CVehicleModelInfo","SetAtomicRendererCB_Heli",0x4C7A30,0,-1,128},{"CVehicleModelInfo","SetAtomicRendererCB_Plane",0x4C7930,0,-1,128},
    {"CVehicleModelInfo","SetAtomicRendererCB_RealHeli",0x4C7870,0,-1,128},{"CVehicleModelInfo","SetAtomicRendererCB_Train",0x4C7AA0,0,-1,128},{"CVehicleModelInfo","SetCarCustomPlate",0x4C9450,0,-1,128},
    {"CVehicleModelInfo","SetClump",0,0x85C5C8,16,256},{"CVehicleModelInfo","SetCustomCarPlateText",0x4C8980,0,-1,256},{"CVehicleModelInfo","SetDirtTextures",0x5D5DB0,0,-1,128},
    {"CVehicleModelInfo","SetEditableMaterials",0x4C8430,0,-1,256},{"CVehicleModelInfo","SetEditableMaterialsCB-RpAtomic",0x4C83E0,0,-1,256},{"CVehicleModelInfo","SetEditableMaterialsCB-RpMaterial",0x4C8220,0,-1,512},
    {"CVehicleModelInfo","SetEnvMapCoeff",0x4C9800,0,-1,256},{"CVehicleModelInfo","SetEnvMapCoeffAtomicCB",0x4C9430,0,-1,64},{"CVehicleModelInfo","SetEnvMapCoeffCB",0x4C88B0,0,-1,256},
    {"CVehicleModelInfo","SetEnvironmentMapAtomicCB",0x4C9410,0,-1,64},{"CVehicleModelInfo","SetEnvironmentMapCB",0x4C8840,0,-1,256},{"CVehicleModelInfo","SetRenderPipelines",0x4C8900,0,-1,128},
    {"CVehicleModelInfo","SetRenderPipelinesCB",0x4C88F0,0,-1,64},{"CVehicleModelInfo","SetVehicleColour",0x4C84B0,0,-1,128},{"CVehicleModelInfo","SetVehicleComponentFlags",0x4C7C10,0,-1,512},
    {"CVehicleModelInfo","SetupCommonData",0x5B8F00,0,-1,128},{"CVehicleModelInfo","SetupLightFlags",0x4C8C90,0,-1,256},{"CVehicleModelInfo","ShutdownEnvironmentMaps",0x4C87D0,0,-1,128},
    {"CVehicleModelInfo","ShutdownLightTexture",0x4C7470,0,-1,128},{"CVehicleModelInfo","StopUsingCommonVehicleTexDicationary",0x4C75C0,0,-1,64},{"CVehicleModelInfo","StoreAtomicUsedMaterialsCB",0x4C8B60,0,-1,256},
    {"CVehicleModelInfo","UseCommonVehicleTexDicationary",0x4C75A0,0,-1,64},
}};

// Registra los stubs (direccion del EXE + simbolo de la DLL). Devuelve el nombre de lo que falte.
inline std::string RegisterStubs(){
    stubs.clear();
    auto hk=[&](uint32 g,uintptr_t exe,const char* cls,const char* name,void* fn)->std::string{ return AddHookStub(g,exe,cls,name,fn) ? std::string() : std::string(cls)+"::"+name; };
    std::string miss;
    auto chk=[&](const std::string& s){ if(!s.empty())miss+=s+" "; };
    // arbol RW
    AddStub(G_TREE,0x749B70,0,FnPtr(&S_ClumpForAllAtomics)); AddStub(G_TREE,0x7F0DC0,0,FnPtr(&S_FrameForAllChildren));
    AddStub(G_TREE,0x7F1200,0,FnPtr(&S_FrameForAllObjects)); AddStub(G_TREE,0x74C790,0,FnPtr(&S_GeometryForAllMaterials));
    AddStub(G_TREE,0x7348C0,bit_cast<uintptr_t>(&GetFirstObject),FnPtr(&S_GetFirstObject)); AddStub(G_TREE,0x734900,bit_cast<uintptr_t>(&GetFirstChild),FnPtr(&S_GetFirstChild));
    // objetos RW
    AddStub(G_RWOBJ,0x7F0410,0,FnPtr(&S_FrameCreate)); AddStub(G_RWOBJ,0x7F05A0,0,FnPtr(&S_FrameDestroy)); AddStub(G_RWOBJ,0x7F0B00,0,FnPtr(&S_FrameAddChild));
    AddStub(G_RWOBJ,0x7F0CD0,0,FnPtr(&S_FrameRemoveChild)); AddStub(G_RWOBJ,0x7F0F70,0,FnPtr(&S_FrameTransform)); AddStub(G_RWOBJ,0x7F0910,0,FnPtr(&S_FrameUpdateObjects));
    AddStub(G_RWOBJ,0x7F0990,0,FnPtr(&S_FrameGetLTM)); AddStub(G_RWOBJ,0x7F2A50,0,FnPtr(&S_MatrixCreate)); AddStub(G_RWOBJ,0x7F2A20,0,FnPtr(&S_MatrixDestroy));
    AddStub(G_RWOBJ,0x7F25A0,0,FnPtr(&S_MatrixTransform)); AddStub(G_RWOBJ,0x7F1FD0,0,FnPtr(&S_MatrixRotate)); AddStub(G_RWOBJ,0x7EDD90,0,FnPtr(&S_V3dTransformPoints));
    AddStub(G_RWOBJ,0x74A4C0,0,FnPtr(&S_ClumpRemoveAtomic)); AddStub(G_RWOBJ,0x74A490,0,FnPtr(&S_ClumpAddAtomic)); AddStub(G_RWOBJ,0x749E60,0,FnPtr(&S_AtomicClone));
    AddStub(G_RWOBJ,0x749DC0,0,FnPtr(&S_AtomicDestroy)); AddStub(G_RWOBJ,0x74BF20,0,FnPtr(&S_AtomicSetFrame)); AddStub(G_RWOBJ,0x59C3E0,bit_cast<uintptr_t>(static_cast<void(CQuaternion::*)(const RwMatrix&)>(&CQuaternion::Set)),FnPtr(&S_QuatSet));
    // materiales / MatFX
    AddStub(G_MATFX,0x812140,0,FnPtr(&S_MatFXGetEffects)); AddStub(G_MATFX,0x811C80,0,FnPtr(&S_MatFXSetEffects)); AddStub(G_MATFX,0x8125B0,0,FnPtr(&S_MatFXSetEnvMapFrame));
    AddStub(G_MATFX,0x812680,0,FnPtr(&S_MatFXSetEnvMapCoeff)); AddStub(G_MATFX,0x74DBC0,0,FnPtr(&S_MaterialSetTexture)); AddStub(G_MATFX,0x74E350,0,FnPtr(&S_MatListAppend));
    AddStub(G_MATFX,0x74E150,0,FnPtr(&S_MatListDeinit));
    // texturas
    AddStub(G_TEX,0x7F39F0,0,FnPtr(&S_TexDictFind)); AddStub(G_TEX,0x7F3A90,0,FnPtr(&S_TexDictGetCurrent)); AddStub(G_TEX,0x7F34F0,0,FnPtr(&S_TexGetFindCB));
    AddStub(G_TEX,0x7F34D0,0,FnPtr(&S_TexSetFindCB)); AddStub(G_TEX,0x7F3820,0,FnPtr(&S_TexDestroy)); AddStub(G_TEX,0x7F3AC0,0,FnPtr(&S_TexRead));
    // memoria, azar, scratchpad, reloj
    // CQuaternion::Set y CMemoryMgr::InitScratchPad: el candidato es C++ y llama a las funciones de la DLL; se interceptan también esos puntos de entrada.
    AddStub(G_RNG,0x821B1E,bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FnPtr(&S_RandExe));
    stubs.back().fn=FnPtr(&S_RandExe);
    AddStub(G_NEW,0x82119A,0,FnPtr(&S_New)); AddStub(G_NEW,0x821195,0,FnPtr(&S_New)); AddStub(G_NEW,0x8214BD,0,FnPtr(&S_Delete)); AddStub(G_NEW,0x8213AE,0,FnPtr(&S_Delete));
    AddStub(G_NEW,0x72F420,0,FnPtr(&S_Malloc72)); AddStub(G_SCRATCH,0x72F480,bit_cast<uintptr_t>(&CMemoryMgr::InitScratchPad),FnPtr(&S_InitScratch));
    // visibilidad y clump base
    chk(hk(G_VIS,0x7328A0,"CVisibilityPlugins","SetAtomicRenderCallback",FnPtr(&S_SetAtomicRenderCB)));
    chk(hk(G_VIS,0x732350,"CVisibilityPlugins","ClearClumpForAllAtomicsFlag",FnPtr(&S_ClearClumpFlag)));
    chk(hk(G_CLUMP,0x4C4E40,"CClumpModelInfo","Init",FnPtr(&S_ClumpInit))); chk(hk(G_CLUMP,0x4C4F70,"CClumpModelInfo","SetClump",FnPtr(&S_ClumpSetClump)));
    chk(hk(G_CLUMP,0x4C4E70,"CClumpModelInfo","DeleteRwObject",FnPtr(&S_ClumpDelete))); chk(hk(G_CLUMP,0x4C5140,"CClumpModelInfo","CreateInstance-void",FnPtr(&S_ClumpCreate)));
    chk(hk(G_CLUMP,0x4C5460,"CClumpModelInfo","SetFrameIds",FnPtr(&S_ClumpSetIds))); chk(hk(G_CLUMP,0x4C53C0,"CClumpModelInfo","GetFrameFromId",FnPtr(&S_GetFrameFromId)));
    chk(hk(G_CLUMP,0x4C5A20,"CModelInfo","GetModelInfo-minmax",FnPtr(&S_GetModelInfoRange)));
    // subfunciones de la propia clase
    chk(hk(G_OWN,0x4C8040,VMI,"ChooseComponent",FnPtr(&S_Choose1))); chk(hk(G_OWN,0x4C8120,VMI,"ChooseSecondComponent",FnPtr(&S_Choose2)));
    chk(hk(G_OWN,0x4C7B10,VMI,"SetAtomicRenderCallbacks",FnPtr(&S_SetARCBs))); chk(hk(G_OWN,0x4C8900,VMI,"SetRenderPipelines",FnPtr(&S_SetRPipes)));
    chk(hk(G_OWN,0x4C8E60,VMI,"PreprocessHierarchy",FnPtr(&S_Preprocess))); chk(hk(G_OWN,0x4C8BD0,VMI,"ReduceMaterialsInVehicle",FnPtr(&S_Reduce)));
    chk(hk(G_OWN,0x4C9450,VMI,"SetCarCustomPlate",FnPtr(&S_SetCCPlate)));
    chk(hk(G_LOADERS,0x5B6890,VMI,"LoadVehicleColours",FnPtr(&S_LoadColours))); chk(hk(G_LOADERS,0x5B65A0,VMI,"LoadVehicleUpgrades",FnPtr(&S_LoadUpgrades)));
    chk(hk(G_LOADERS,0x4C8780,VMI,"LoadEnvironmentMaps",FnPtr(&S_LoadEnv)));
    // coche, jugador, animaciones, tiempos
    chk(hk(G_CAR,0x5D5B40,"CCarFXRenderer","CustomCarPipeClumpSetup",FnPtr(&S_FxClumpSetup))); chk(hk(G_CAR,0x5D5BC0,"CCarFXRenderer","InitialiseDirtTexture",FnPtr(&S_InitDirt)));
    chk(hk(G_CAR,0x6FD5B0,"CCustomCarPlateMgr","GeneratePlateText",FnPtr(&S_PlateGenerate))); chk(hk(G_CAR,0x6FE0F0,"CCustomCarPlateMgr","SetupClump",FnPtr(&S_PlateSetupClump)));
    chk(hk(G_PLAYER,0x56E0D0,"CWorld","FindPlayerVehicle",FnPtr(&S_FindPlayerVehicle)));
    chk(hk(G_ANIM,0x4D3990,"CAnimManager","GetAnimationBlockIndex-by-name",FnPtr(&S_AnimBlockIndex)));
    chk(hk(G_TIMER,0x561A80,"CTimer","GetCurrentTimeInCycles",FnPtr(&S_Cycles))); chk(hk(G_TIMER,0x561A40,"CTimer","GetCyclesPerMillisecond",FnPtr(&S_CyclesMs)));
    // ficheros, modelos y TXD
    chk(hk(G_FILE,0x538900,"CFileMgr","OpenFile",FnPtr(&S_OpenFile))); chk(hk(G_FILE,0x5389B0,"CFileMgr","ReadLine",FnPtr(&S_ReadLine))); chk(hk(G_FILE,0x5389D0,"CFileMgr","CloseFile",FnPtr(&S_CloseFile)));
    chk(hk(G_FILE,0x536F80,"CFileLoader","LoadLine-File",FnPtr(&S_LoadLine))); chk(hk(G_FILE,0x4C5940,"CModelInfo","GetModelInfo-full",FnPtr(&S_GetModelInfoName)));
    chk(hk(G_FILE,0x590D00,"CLoadingScreen","NewChunkLoaded",FnPtr(&S_NewChunk))); chk(hk(G_UPG,0x4C4570,"CAtomicModelInfo","SetupVehicleUpgradeFlags",FnPtr(&S_UpgradeFlags)));
    chk(hk(G_TXD,0x731850,"CTxdStore","FindTxdSlot-name",FnPtr(&S_FindTxdSlot))); chk(hk(G_TXD,0x731C80,"CTxdStore","AddTxdSlot",FnPtr(&S_AddTxdSlot)));
    chk(hk(G_TXD,0x7316A0,"CTxdStore","PushCurrentTxd",FnPtr(&S_PushTxd))); chk(hk(G_TXD,0x7316B0,"CTxdStore","PopCurrentTxd",FnPtr(&S_PopTxd)));
    chk(hk(G_TXD,0x7319C0,"CTxdStore","SetCurrentTxd",FnPtr(&S_SetCurrentTxd))); chk(hk(G_TXD,0x7320B0,"CTxdStore","LoadTxd-filename",FnPtr(&S_LoadTxd)));
    chk(hk(G_TXD,0x731A00,"CTxdStore","AddRef",FnPtr(&S_TxdAddRef)));
    return miss;
}

inline std::array<std::array<uint8,0x20000>,2> arenaDump{}; inline std::array<size_t,2> dumpLen{};
// Resumen compacto y determinista (nunca nulo, tambien para funciones void): retorno, nº de eventos y hashes de traza, mundo, arena y globales.
inline json Summary(const json& o){
    return json{{"ret",o["ret"]},{"events",o["trace"].size()},{"trace_fnv",Fnv64(o["trace"].dump().data(),o["trace"].dump().size())},
                {"world",o["world"]},{"arena",o["arena"]},{"arena_top",o["arena_top"]},{"globals",o["globals"]},
                {"fields_fnv",Fnv64(o.dump().data(),o.dump().size())}};
}
inline bool Layout(){
    using M=CVehicleModelInfo; using VS=CVehicleModelInfo::CVehicleStructure;
    bool ok=true; json l=json::object();
    auto chk=[&](const char* n,size_t got,size_t want){ const bool g=got==want; ok&=g; l[n]=g; };
    chk("sizeof_mi",sizeof(M),0x308); chk("sizeof_vs",sizeof(VS),0x314); chk("sizeof_upg",sizeof(UpgradePosnDesc),0x20); chk("sizeof_restore",sizeof(tRestoreEntry),8);
    chk("plateMat",offsetof(M,m_pPlateMaterial),0x24); chk("plateText",offsetof(M,m_szPlateText),0x28); chk("plateType",offsetof(M,m_nPlateType),0x31);
    chk("type",offsetof(M,m_nVehicleType),0x3C); chk("wheelModel",offsetof(M,m_nWheelModelIndex),0x48); chk("handling",offsetof(M,m_nHandlingId),0x4A);
    chk("doors",offsetof(M,m_nNumDoors),0x4C); chk("comps",offsetof(M,m_extraComps),0x54); chk("steer",offsetof(M,m_fBikeSteerAngle),0x58);
    chk("struct",offsetof(M,m_pVehicleStruct),0x5C); chk("dirt",offsetof(M,m_apDirtMaterials),0x230); chk("prim",offsetof(M,m_anPrimaryColors),0x2B0);
    chk("numCols",offsetof(M,m_nNumColorVariations),0x2D0); chk("curCol",offsetof(M,m_nCurrentPrimaryColor),0x2D2); chk("upgrades",offsetof(M,m_anUpgrades),0x2D6);
    chk("remaps",offsetof(M,m_anRemapTxds),0x2FA); chk("anim",offsetof(M,m_nAnimBlockIndex),0x304);
    chk("vs_upg",offsetof(VS,m_aUpgrades),0xB4); chk("vs_extras",offsetof(VS,m_apExtras),0x2F4); chk("vs_n",offsetof(VS,m_nNumExtras),0x30C); chk("vs_mask",offsetof(VS,m_nMaskComponentsDamagable),0x310);
    chk("pos_in_upg",offsetof(UpgradePosnDesc,m_nParentComponentId),0x1C);
    chk("atomic_ok",ATOMIC_OK,1); chk("atomic_dam",ATOMIC_DAMAGED,2); chk("atomic_flat",ATOMIC_FLAT,0x80); chk("atomic_unique",ATOMIC_UNIQUE_MATERIALS,0x2000);
    chk("atomic_noalpha",ATOMIC_DONT_RENDER_ALPHA,0x1000); chk("atomic_top",ATOMIC_TOP,0x8000);
    chk("t_boat",VEHICLE_TYPE_BOAT,5); chk("t_plane",VEHICLE_TYPE_PLANE,4); chk("t_heli",VEHICLE_TYPE_HELI,3); chk("t_train",VEHICLE_TYPE_TRAIN,6); chk("t_fplane",VEHICLE_TYPE_FPLANE,8);
    chk("t_bike",VEHICLE_TYPE_BIKE,9); chk("t_bmx",VEHICLE_TYPE_BMX,10); chk("t_trailer",VEHICLE_TYPE_TRAILER,11);
    chk("carplate",CARPLATE_DEFAULT,255); chk("wheel_rf",CAR_WHEEL_RF,2); chk("wheel_lf",CAR_WHEEL_LF,5);
    chk("m_firetruk",MODEL_FIRETRUK,407); chk("m_journey",MODEL_JOURNEY,508); chk("m_hunter",MODEL_HUNTER,425); chk("m_bus",MODEL_BUS,431); chk("m_coach",MODEL_COACH,437);
    chk("m_rcbandit",MODEL_RCBANDIT,441); chk("m_rctiger",MODEL_RCTIGER,564); chk("rain_sf",WEATHER_RAINY_SF,8); chk("rain_cs",WEATHER_RAINY_COUNTRYSIDE,16);
    chk("cheat_pink",CHEAT_PINK_TRAFFIC,32); chk("cheat_black",CHEAT_BLACK_TRAFFIC,33);
    Write({{"event","layout"},{"ok",ok},{"checks",l}});
    return ok;
}

inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"vehmodelinfofinal4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vehmodelinfofinal4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vehmodelinfofinal4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"vehmodelinfofinal4-only.txt"); if(f)std::getline(f,only); }

    std::array<ReversibleHooks::ReversibleHook::Base*,75> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    nameOff=*reinterpret_cast<int32*>(0xC87C5C); frameVisOff=*reinterpret_cast<int32*>(0x8D6090); atomVisOff=*reinterpret_cast<int32*>(0x8D608C);
    if(nameOff<0xA4||nameOff+24>int32(FR_SIZE)||frameVisOff<0xA4||frameVisOff+4>int32(FR_SIZE)||atomVisOff<0x70||atomVisOff+4>int32(AT_SIZE)||(frameVisOff+4>nameOff&&nameOff+24>frameVisOff)){
        Write({{"event","error"},{"reason","plugin offsets out of range"},{"name",nameOff},{"frame",frameVisOff},{"atomic",atomVisOff}});return; }
    if(!Layout()){ Write({{"event","error"},{"reason","layout"}}); return; }
    const auto miss=RegisterStubs(); if(!miss.empty()){ Write({{"event","error"},{"reason","stubs not found"},{"missing",miss}}); return; }

    nregs=0; Reg(&w,sizeof(World)); Reg(arena.data(),arena.size());
    for(auto& b:w.mi)Reg(b,MI_SIZE);   // regiones mas finas primero no son necesarias: Id() usa la primera que contiene el puntero
    // Instantanea de globales (restauradas al final de cada ejecucion)
    saved.clear();
    Save(0xB4DBE0,0xB4E780-0xB4DBE0);    // gRestoreEntries, luces, colores, ruedas, texturas, pool, TXD, callbacks, carFrame, mejoras enlazadas
    Save(0x969130,0x50);                 // cheats (trafico rosa/negro)
    Save(0xC81318,0x10);                 // clima
    Save(0x8A6458,0x10);                 // ms_compsToUse y superficie de luces
    Save(0x8A7740,0x50);                 // ms_vehicleDescs, ruedas, constantes
    Save(HANDLING_BASE,0xE0u*24u);       // handling (24 primeras entradas)
    Save(0xC02BD0,64);                   // texturas de suciedad
    Save(MODELPTRS+4u*390u,4u*250u);     // ms_modelInfoPtrs 390..639
    Save(0xC8800C,4);                    // pool de TXD
    std::array<bool,75> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    const std::vector<uint8> vtSaved(reinterpret_cast<uint8*>(0x85C5C8),reinterpret_cast<uint8*>(0x85C5C8)+17*4);

    const unsigned savedFP=_controlfp(0,0);
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                Ctx c; c.fn=&fn; c.nm=fn.name; c.i=i; c.f=uint32(f); c.candidate=candidate;
                c.st=uint32(i+1)*2654435761u^uint32(f+1)*0x9E3779B9u^0x56E1u; if(!c.st)c.st=1;
                std::memset(arena.data(),0xCD,arena.size()); nrec=0; recOverflow=false; arenaTop=0; fileLineCount=0; fileLinePos=0; rngState=1; gmiMode=1; gmiType=6;
                for(auto* h:hooks)h->State(candidate);
                const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                c.a= fn.slot>=0 ? vtTarget : fn.address;
                c.out=json::object(); c.ret=nullptr;
                _controlfp(_PC_24,_MCW_PC);
                const bool handled=RunA(c)||RunB(c)||RunC(c)||RunD(c);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                json out=c.out; out["switches"]=switches; out["handled"]=handled; out["ret"]=c.ret; out["trace"]=Recs();
                out["world"]=Fnv64(&w,sizeof(World)); out["arena"]=Fnv64(arena.data(),arena.size()); out["arena_top"]=arenaTop; out["arena_head"]=ReversingProbe::Bytes(arena.data(),std::min<size_t>(arenaTop+16,0x180));
                uint64 g=1469598103934665603ull; for(auto& s:saved){ g^=Fnv64(reinterpret_cast<void*>(s.addr),s.bytes.size()); g*=1099511628211ull; } out["globals"]=g;
                dumpLen[candidate?1:0]=std::min<size_t>(arenaTop+64,arena.size()); std::memcpy(arenaDump[candidate?1:0].data(),arena.data(),dumpLen[candidate?1:0]);
                RestoreAll();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["handled"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(), vals=json::object(); for(auto& [k,v]:original.items())if(candidate[k]!=v){ diff.push_back(k);
                       const auto so=v.dump(), sc=candidate[k].dump(); vals[k]={{"o",so.substr(0,300)},{"c",sc.substr(0,300)}}; }
                // bloques de 64 bytes de la arena que difieren entre lados (maximo 8, con los 32 primeros bytes de cada uno)
                json blocks=json::array(); const size_t n=std::min(dumpLen[0],dumpLen[1])/64; int shown=0;
                for(size_t b=0;b<n&&shown<8;++b)if(std::memcmp(arenaDump[0].data()+64*b,arenaDump[1].data()+64*b,64)!=0){
                    blocks.push_back({{"block",b},{"offset",64*b},{"original",ReversingProbe::Bytes(arenaDump[0].data()+64*b,32)},{"candidate",ReversingProbe::Bytes(arenaDump[1].data()+64*b,32)}}); ++shown; }
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"diff_values",vals},{"diff_blocks",blocks},
                       {"arena_len",json::array({dumpLen[0],dumpLen[1]})},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    const bool restored=AllRestored()&&std::memcmp(reinterpret_cast<void*>(0x85C5C8),vtSaved.data(),vtSaved.size())==0&&_controlfp(0,0)==savedFP;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
