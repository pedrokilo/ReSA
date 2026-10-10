#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1): CAudioEngine, 82 funciones (lista final 4).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato (hooks activos) sobre un CAudioEngine FALSO en
// memoria propia (0x1FD8 bytes) y un mundo falso (entidades opacas, vectores, ajustes de vehiculo, un CAEGlobalWeaponAudioEntity
// con ceros). Todo lo que llama CAudioEngine (hardware, entidades de audio, managers de radio / cutscene / ambiente, SoundManager,
// ficheros, pantalla de carga, inicializadores estaticos) se sustituye con dobles trazados por EntryPatch (direccion del EXE y
// simbolo/cuerpo de la DLL, cuerpos calculados ANTES de crear ningun parche) cuyos retornos salen de un guion por caso. Los globales
// reales que lee o escribe alguna funcion se guardan y se restauran en cada ejecucion. Ver README.md.
#include "Cases.hpp"

namespace AudioEngineFinal4Live {
inline const std::array<Fn,82> fns{{
    {"CAudioEngine","AttachMissionAudioToObject",0x507320,0,-1,128},{"CAudioEngine","AttachMissionAudioToPed",0x507310,0,-1,128},{"CAudioEngine","AttachMissionAudioToPhysical",0x507330,0,-1,128},
    {"CAudioEngine","ClearMissionAudio",0x5072F0,0,-1,128},{"CAudioEngine","Constructor",0x507670,0,-1,64},{"CAudioEngine","Destructor",0x506CD0,0,-1,64},
    {"CAudioEngine","DisableEffectsLoading",0x506E80,0,-1,64},{"CAudioEngine","DisplayRadioStationName",0x507030,0,-1,64},{"CAudioEngine","DoesAmbienceTrackOverrideRadio",0x507270,0,-1,64},
    {"CAudioEngine","EnableEffectsLoading",0x506E90,0,-1,64},{"CAudioEngine","GetBeatInfo",0x5071B0,0,-1,128},{"CAudioEngine","GetBeatTrackStatus",0x507170,0,-1,128},
    {"CAudioEngine","GetCurrentRadioStationID",0x507040,0,-1,128},{"CAudioEngine","GetCutsceneTrackStatus",0x507160,0,-1,128},{"CAudioEngine","GetMissionAudioEvent",0x5072D0,0,-1,128},
    {"CAudioEngine","GetMissionAudioLoadingStatus",0x5072A0,0,-1,128},{"CAudioEngine","GetMissionAudioPosition",0x5072E0,0,-1,128},{"CAudioEngine","GetRadioStationListenTimes",0x507020,0,-1,64},
    {"CAudioEngine","GetRadioStationName",0x507000,0,-1,128},{"CAudioEngine","GetRadioStationNameKey",0x507010,0,-1,128},{"CAudioEngine","HasRadioRetuneJustStarted",0x506FE0,0,-1,64},
    {"CAudioEngine","Initialise",0x5B9C60,0,-1,1024},{"CAudioEngine","InitialisePostLoading",0x5078F0,0,-1,512},{"CAudioEngine","InitialiseRadioStationID",0x506FC0,0,-1,128},
    {"CAudioEngine","IsAmbienceRadioActive",0x507280,0,-1,64},{"CAudioEngine","IsAmbienceTrackActive",0x507210,0,-1,64},{"CAudioEngine","IsBeatInfoPresent",0x5071D0,0,-1,128},
    {"CAudioEngine","IsCutsceneTrackActive",0x507150,0,-1,64},{"CAudioEngine","IsLoadingTuneActive",0x506D90,0,-1,64},{"CAudioEngine","IsMissionAudioSampleFinished",0x5072C0,0,-1,128},
    {"CAudioEngine","IsRadioOn",0x506FD0,0,-1,64},{"CAudioEngine","IsRadioRetuneInProgress",0x506FF0,0,-1,64},{"CAudioEngine","IsVehicleRadioActive",0x507050,0,-1,64},
    {"CAudioEngine","PauseAllSounds",0x507430,0,-1,64},{"CAudioEngine","PauseBeatTrack",0x507200,0,-1,64},{"CAudioEngine","PlayLoadedMissionAudio",0x5072B0,0,-1,128},
    {"CAudioEngine","PlayPreloadedBeatTrack",0x507180,0,-1,128},{"CAudioEngine","PlayPreloadedCutsceneTrack",0x507070,0,-1,64},{"CAudioEngine","PreloadBeatTrack",0x507F40,0,-1,512},
    {"CAudioEngine","PreloadCutsceneTrack",0x507E30,0,-1,512},{"CAudioEngine","PreloadMissionAudio",0x507290,0,-1,128},{"CAudioEngine","ReportBulletHit",0x506EC0,0,-1,256},
    {"CAudioEngine","ReportCollision",0x506EB0,0,-1,512},{"CAudioEngine","ReportDoorMovement",0x506F50,0,-1,128},{"CAudioEngine","ReportFrontendAudioEvent",0x506EA0,0,-1,256},
    {"CAudioEngine","ReportGlassCollisionEvent",0x506EE0,0,-1,256},{"CAudioEngine","ReportMissionAudioEvent-obj",0x507350,0,-1,128},{"CAudioEngine","ReportMissionAudioEvent-ped",0x507370,0,-1,128},
    {"CAudioEngine","ReportMissionAudioEvent-phs",0x5073B0,0,-1,256},{"CAudioEngine","ReportMissionAudioEvent-vec",0x507340,0,-1,128},{"CAudioEngine","ReportMissionAudioEvent-veh",0x507390,0,-1,128},
    {"CAudioEngine","ReportObjectDestruction",0x506ED0,0,-1,128},{"CAudioEngine","ReportWaterSplash-phs",0x506F00,0,-1,256},{"CAudioEngine","ReportWaterSplash-vec",0x506F10,0,-1,256},
    {"CAudioEngine","ReportWeaponEvent",0x506F40,0,-1,256},{"CAudioEngine","Reset",0x507A90,0,-1,1024},{"CAudioEngine","ResetSoundEffects",0x507C30,0,-1,128},
    {"CAudioEngine","ResetStatistics",0x506DA0,0,-1,64},{"CAudioEngine","Restart",0x506DB0,0,-1,64},{"CAudioEngine","ResumeAllSounds",0x507440,0,-1,64},
    {"CAudioEngine","RetuneRadio",0x507E10,0,-1,256},{"CAudioEngine","SayPedless",0x5073C0,0,-1,256},{"CAudioEngine","Service",0x507750,0,-1,1024},
    {"CAudioEngine","ServiceLoadingTune",0x5078A0,0,-1,1024},{"CAudioEngine","SetBassEnhanceOnOff",0x506F90,0,-1,64},{"CAudioEngine","SetEffectsFaderScalingFactor",0x506E50,0,-1,256},
    {"CAudioEngine","SetEffectsMasterVolume",0x506E10,0,-1,256},{"CAudioEngine","SetMissionAudioPosition",0x507300,0,-1,128},{"CAudioEngine","SetMusicFaderScalingFactor",0x506E40,0,-1,256},
    {"CAudioEngine","SetMusicMasterVolume",0x506DE0,0,-1,256},{"CAudioEngine","SetNonStreamFaderScalingFactor",0x506E60,0,-1,256},{"CAudioEngine","SetRadioAutoRetuneOnOff",0x506F80,0,-1,64},
    {"CAudioEngine","SetRadioBassSetting",0x506FA0,0,-1,128},{"CAudioEngine","SetStreamFaderScalingFactor",0x506E70,0,-1,256},{"CAudioEngine","Shutdown",0x507CB0,0,-1,512},
    {"CAudioEngine","StartLoadingTune",0x507410,0,-1,64},{"CAudioEngine","StartRadio-",0x507DF0,0,-1,256},{"CAudioEngine","StartRadio-1",0x507DC0,0,-1,256},
    {"CAudioEngine","StopAmbienceTrack",0x507220,0,-1,256},{"CAudioEngine","StopBeatTrack",0x5071A0,0,-1,256},{"CAudioEngine","StopCutsceneTrack",0x507080,0,-1,512},
    {"CAudioEngine","StopRadio",0x506F70,0,-1,128},
}};

// Registra los dobles (direccion del EXE + simbolo de la DLL). Devuelve el nombre de lo que falte.
inline std::string RegisterStubs(){
    stubs.clear(); std::string miss;
    auto hk=[&](uint32 g,uintptr_t exe,const char* cls,const char* name,void* fn){ if(!AddHookStub(g,exe,cls,name,fn))miss+=std::string(cls)+"::"+name+" "; };
    const uint32 GC=G_COMMON, GI=G_INIT;
    // CAEAudioHardware
    hk(GC,0x4D9530,"CAEAudioHardware","SetMusicMasterScalingFactor",FnPtr(&H_SetMusicMaster)); hk(GC,0x4D9540,"CAEAudioHardware","SetEffectsMasterScalingFactor",FnPtr(&H_SetEffMaster));
    hk(GC,0x4D9550,"CAEAudioHardware","SetMusicFaderScalingFactor",FnPtr(&H_SetMusicFader)); hk(GC,0x4D9560,"CAEAudioHardware","SetEffectsFaderScalingFactor",FnPtr(&H_SetEffFader));
    hk(GC,0x4D95A0,"CAEAudioHardware","SetNonStreamFaderScalingFactor",FnPtr(&H_SetNonStream)); hk(GC,0x4D95B0,"CAEAudioHardware","SetStreamFaderScalingFactor",FnPtr(&H_SetStream));
    hk(GC,0x4D8E50,"CAEAudioHardware","DisableEffectsLoading",FnPtr(&H_DisableFx)); hk(GC,0x4D8E40,"CAEAudioHardware","EnableEffectsLoading",FnPtr(&H_EnableFx));
    hk(GC,0x4D95F0,"CAEAudioHardware","PauseAllSounds",FnPtr(&H_PauseAll)); hk(GC,0x4D9630,"CAEAudioHardware","ResumeAllSounds",FnPtr(&H_ResumeAll));
    hk(GC,0x4D8F60,"CAEAudioHardware","GetTrackPlayTime",FnPtr(&H_TrackTime)); hk(GC,0x4D9870,"CAEAudioHardware","Service",FnPtr(&H_Service));
    hk(GC,0x4D8FA0,"CAEAudioHardware","GetBeatInfo",FnPtr(&H_BeatInfo)); hk(GC,0x4D8820,"CAEAudioHardware","GetChannelPlayTimes",FnPtr(&H_ChanPlay));
    hk(GC,0x4D9590,"CAEAudioHardware","GetEffectsFaderScalingFactor",FnPtr(&H_GetEffFader)); hk(GC,0x4D88C0,"CAEAudioHardware","IsSoundBankLoaded",FnPtr(&H_BankLoaded));
    hk(GC,0x4D97A0,"CAEAudioHardware","Terminate",FnPtr(&H_Terminate)); hk(GI,0x4D9930,"CAEAudioHardware","Initialise",FnPtr(&H_Init));
    // CAEFrontendAudioEntity
    hk(GC,0x4DD4A0,"CAEFrontendAudioEntity","AddAudioEvent",FnPtr(&F_Add)); hk(GC,0x4DD470,"CAEFrontendAudioEntity","IsLoadingTuneActive",FnPtr(&F_LoadActive));
    hk(GC,0x4DD480,"CAEFrontendAudioEntity","IsRadioTuneSoundActive",FnPtr(&F_RetuneActive)); hk(GC,0x4DD440,"CAEFrontendAudioEntity","Reset",FnPtr(&F_Reset));
    hk(GI,0x5B9AB0,"CAEFrontendAudioEntity","Initialise",FnPtr(&F_Init));
    // CAEScriptAudioEntity
    hk(GC,0x4EC190,"CAEScriptAudioEntity","PreloadMissionAudio",FnPtr(&SC_Preload)); hk(GC,0x4EBF60,"CAEScriptAudioEntity","GetMissionAudioLoadingStatus",FnPtr(&SC_Status));
    hk(GC,0x4EC270,"CAEScriptAudioEntity","PlayLoadedMissionAudio",FnPtr(&SC_Play)); hk(GC,0x4EBFE0,"CAEScriptAudioEntity","IsMissionAudioSampleFinished",FnPtr(&SC_Finished));
    hk(GC,0x4EC020,"CAEScriptAudioEntity","GetMissionAudioEvent",FnPtr(&SC_Event)); hk(GC,0x4EC4D0,"CAEScriptAudioEntity","GetMissionAudioPosition",FnPtr(&SC_Pos));
    hk(GC,0x4EC040,"CAEScriptAudioEntity","ClearMissionAudio",FnPtr(&SC_Clear)); hk(GC,0x4EC0C0,"CAEScriptAudioEntity","SetMissionAudioPosition",FnPtr(&SC_SetPos));
    hk(GC,0x4EC100,"CAEScriptAudioEntity","AttachMissionAudioToPhysical",FnPtr(&SC_Attach)); hk(GC,0x4EE940,"CAEScriptAudioEntity","ReportMissionAudioEvent-2",FnPtr(&SC_RepVec));
    hk(GC,0x4EE960,"CAEScriptAudioEntity","ReportMissionAudioEvent-1",FnPtr(&SC_RepPhys)); hk(GC,0x4EC900,"CAEScriptAudioEntity","Service",FnPtr(&SC_Service));
    hk(GC,0x4EC150,"CAEScriptAudioEntity","Reset",FnPtr(&SC_Reset)); hk(GI,0x5B9B60,"CAEScriptAudioEntity","Initialise",FnPtr(&SC_Init));
    // CAECollisionAudioEntity
    hk(GC,0x4DBA10,"CAECollisionAudioEntity","ReportCollision",FnPtr(&CO_Report)); hk(GC,0x4DBDF0,"CAECollisionAudioEntity","ReportBulletHit",FnPtr(&CO_Bullet));
    hk(GC,0x4DAB60,"CAECollisionAudioEntity","ReportObjectDestruction",FnPtr(&CO_Destroy)); hk(GC,0x4DA070,"CAECollisionAudioEntity","ReportGlassCollisionEvent",FnPtr(&CO_Glass));
    hk(GC,0x4DAE40,"CAECollisionAudioEntity","ReportWaterSplash-for-physical",FnPtr(&CO_SplashPhys)); hk(GC,0x4DA190,"CAECollisionAudioEntity","ReportWaterSplash-at-position",FnPtr(&CO_SplashPos));
    hk(GC,0x4DA2C0,"CAECollisionAudioEntity","Service",FnPtr(&CO_Service)); hk(GC,0x4DA320,"CAECollisionAudioEntity","Reset",FnPtr(&CO_Reset));
    hk(GC,0x4DA050,"CAECollisionAudioEntity","InitialisePostLoading",FnPtr(&CO_PostLoading)); hk(GI,0x5B9BD0,"CAECollisionAudioEntity","Initialise",FnPtr(&CO_Init));
    // armas globales, puertas, habla sin ped
    hk(GC,0x4DFAA0,"CAEGlobalWeaponAudioEntity","AddAudioEvent",FnPtr(&GW_Add)); hk(GC,0x4DF210,"CAEGlobalWeaponAudioEntity","ServiceAmbientGunFire",FnPtr(&GW_Ambient));
    hk(GC,0x503490,"CAEWeaponAudioEntity","Reset",FnPtr(&GW_Reset)); hk(GC,0x503450,"CAEWeaponAudioEntity","Initialise",FnPtr(&GW_Init));
    hk(GC,0x4DC9F0,"CAEDoorAudioEntity","AddAudioEvent-1",FnPtr(&DR_Add));
    hk(GC,0x4E60D0,"CAEPedlessSpeechAudioEntity","AddSayEvent",FnPtr(&PL_Say)); hk(GI,0x4E4E10,"CAEPedlessSpeechAudioEntity","Initialise",FnPtr(&PL_Init));
    // CAERadioTrackManager
    hk(GC,0x4E9820,"CAERadioTrackManager","StopRadio",FnPtr(&RD_Stop)); hk(GC,0x4E82E0,"CAERadioTrackManager","SetRadioAutoRetuneOnOff",FnPtr(&RD_Auto));
    hk(GC,0x4E9DB0,"CAERadioTrackManager","SetBassEnhanceOnOff",FnPtr(&RD_BassEnh)); hk(GC,0x4E82F0,"CAERadioTrackManager","SetBassSetting",FnPtr(&RD_Bass));
    hk(GC,0x4E8330,"CAERadioTrackManager","InitialiseRadioStationID",FnPtr(&RD_InitId)); hk(GC,0x4E8350,"CAERadioTrackManager","IsRadioOn",FnPtr(&RD_On));
    hk(GC,0x4E8370,"CAERadioTrackManager","HasRadioRetuneJustStarted",FnPtr(&RD_Just)); hk(GC,0x4E9E10,"CAERadioTrackManager","GetRadioStationName",FnPtr(&RD_Name));
    hk(GC,0x4E8380,"CAERadioTrackManager","GetRadioStationNameKey",FnPtr(&RD_Key)); hk(GC,0x4E83E0,"CAERadioTrackManager","GetRadioStationListenTimes",FnPtr(&RD_Listen));
    hk(GC,0x4E9E50,"CAERadioTrackManager","DisplayRadioStationName",FnPtr(&RD_Display)); hk(GC,0x4E83F0,"CAERadioTrackManager","GetCurrentRadioStationID",FnPtr(&RD_Cur));
    hk(GC,0x4E9800,"CAERadioTrackManager","IsVehicleRadioActive",FnPtr(&RD_Veh)); hk(GC,0x4EB3C0,"CAERadioTrackManager","StartRadio-manual",FnPtr(&RD_Start1));
    hk(GC,0x4EB550,"CAERadioTrackManager","StartRadio-with-settings",FnPtr(&RD_Start2)); hk(GC,0x4E8290,"CAERadioTrackManager","RetuneRadio",FnPtr(&RD_Retune));
    hk(GC,0x4EB9A0,"CAERadioTrackManager","Service",FnPtr(&RD_Service)); hk(GC,0x4E7F80,"CAERadioTrackManager","Reset",FnPtr(&RD_Reset));
    hk(GC,0x4E8200,"CAERadioTrackManager","ResetStatistics",FnPtr(&RD_ResetStats)); hk(GI,0x5B9390,"CAERadioTrackManager","Initialise",FnPtr(&RD_Init));
    hk(GC,0x4F4ED0,"CAEVehicleAudioEntity","StaticGetPlayerVehicleAudioSettingsForRadio",FnPtr(&VE_Settings));
    // CAECutsceneTrackManager (PreloadCutsceneTrack, 0x4DC0E0, no esta enganchada: se parchea el simbolo de la DLL)
    hk(GC,0x4DBE70,"CAECutsceneTrackManager","PlayPreloadedCutsceneTrack",FnPtr(&CT_Play)); hk(GC,0x4DBE80,"CAECutsceneTrackManager","StopCutsceneTrack",FnPtr(&CT_Stop));
    hk(GC,0x4DBEB0,"CAECutsceneTrackManager","IsCutsceneTrackActive",FnPtr(&CT_Active)); hk(GC,0x4DBEC0,"CAECutsceneTrackManager","GetCutsceneTrackStatus",FnPtr(&CT_Status));
    hk(GC,0x4DBF00,"CAECutsceneTrackManager","PauseTrack",FnPtr(&CT_Pause)); hk(GC,0x4DBFB0,"CAECutsceneTrackManager","Service",FnPtr(&CT_Service));
    hk(GC,0x4DC130,"CAECutsceneTrackManager","PreloadBeatTrack",FnPtr(&CT_PreBeat));
    AddStub(GC,0x4DC0E0,bit_cast<uintptr_t>(&CAECutsceneTrackManager::PreloadCutsceneTrack),FnPtr(&CT_PreCut));
    // CAEAmbienceTrackManager (StopAmbienceTrack, 0x4D6CC0, no esta enganchada)
    hk(GC,0x4D6CF0,"CAEAmbienceTrackManager","IsAmbienceTrackActive",FnPtr(&AM_Active)); hk(GC,0x4D6D40,"CAEAmbienceTrackManager","IsAmbienceRadioActive",FnPtr(&AM_Radio));
    hk(GC,0x4D76C0,"CAEAmbienceTrackManager","Service",FnPtr(&AM_Service)); hk(GC,0x4D6D60,"CAEAmbienceTrackManager","StopSpecialMissionAmbienceTrack",FnPtr(&AM_Special));
    hk(GC,0x4D6CA0,"CAEAmbienceTrackManager","Reset",FnPtr(&AM_Reset));
    AddStub(GC,0x4D6CC0,bit_cast<uintptr_t>(&CAEAmbienceTrackManager::StopAmbienceTrack),FnPtr(&AM_Stop));
    // CAESoundManager
    hk(GC,0x4F0000,"CAESoundManager","Service",FnPtr(&SM_Service)); hk(GC,0x4EF4D0,"CAESoundManager","Reset",FnPtr(&SM_Reset));
    hk(GC,0x4EF510,"CAESoundManager","PauseManually",FnPtr(&SM_Pause)); hk(GC,0x4EFAA0,"CAESoundManager","Terminate",FnPtr(&SM_Terminate));
    hk(GI,0x5B9690,"CAESoundManager","Initialise",FnPtr(&SM_Init));
    // varios
    hk(G_FIND,0x56E210,"CWorld","FindPlayerPed",FnPtr(&X_FindPlayerPed));   // solo durante Service (otros hilos del juego pueden llamarla)
    hk(GC,0x4E37B0,"CAEPedSpeechAudioEntity","Reset",FnPtr(&X_PedSpeechReset)); hk(GC,0x4E3710,"CAEPedSpeechAudioEntity","Service",FnPtr(&X_PedSpeechService));
    hk(GC,0x4E7630,"CAEPoliceScannerAudioEntity","Service",FnPtr(&X_PoliceService)); hk(GC,0x4E6E90,"CAEPoliceScannerAudioEntity","Reset",FnPtr(&X_PoliceReset));
    hk(GC,0x5052B0,"CAEWeatherAudioEntity","StaticReset",FnPtr(&X_WeatherReset)); hk(GC,0x4F2FB0,"CAEUserRadioTrackManager","Shutdown",FnPtr(&X_UserRadioShutdown));
    // arranque (solo Initialise)
    hk(GI,0x590310,"CLoadingScreen","Pause",FnPtr(&X_LoadPause)); hk(GI,0x590320,"CLoadingScreen","Continue",FnPtr(&X_LoadContinue));
    hk(GI,0x538900,"CFileMgr","OpenFile",FnPtr(&X_OpenFile)); hk(GI,0x538950,"CFileMgr","Read",FnPtr(&X_ReadFile)); hk(GI,0x5389D0,"CFileMgr","CloseFile",FnPtr(&X_CloseFile));
    hk(GI,0x5B97F0,"CAEAudioUtility","StaticInitialise",FnPtr(&X_InitUtil)); hk(GI,0x5B98A0,"CAEPedAudioEntity","StaticInitialise",FnPtr(&X_InitPed));
    hk(GI,0x5B98C0,"CAEPedSpeechAudioEntity","StaticInitialise",FnPtr(&X_InitSpeech)); hk(GI,0x5B99F0,"CAEVehicleAudioEntity","StaticInitialise",FnPtr(&X_InitVeh));
    hk(GI,0x5B9A60,"CAEExplosionAudioEntity","StaticInitialise",FnPtr(&X_InitExpl)); hk(GI,0x5B9A70,"CAEWeatherAudioEntity","StaticInitialise",FnPtr(&X_InitWeather));
    hk(GI,0x5B9A80,"CAEDoorAudioEntity","StaticInitialise",FnPtr(&X_InitDoor)); hk(GI,0x5B9A90,"CAEFireAudioEntity","StaticInitialise",FnPtr(&X_InitFire));
    hk(GI,0x5B9C30,"CAEPoliceScannerAudioEntity","StaticInitialise",FnPtr(&X_InitPolice));
    // InitialisePostLoading: construccion real de CAEGlobalWeaponAudioEntity (+ CPhysical) con la matriz y el azar sustituidos
    hk(G_CTOR,0x54F4C0,"CPlaceable","AllocateStaticMatrix",FnPtr(&X_AllocStaticMatrix));
    AddStub(G_CTOR,0x821B1E,bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FnPtr(&X_Rand));
    hk(G_ENG,0x507750,"CAudioEngine","Service",FnPtr(&X_EngineService));
    // Shutdown: operator delete / delete[] del EXE (la DLL los llama a estas direcciones) sin liberar el CAEGlobalWeaponAudioEntity falso
    AddStub(G_DEL,0x8214BD,0,FnPtr(&X_Delete)); AddStub(G_DEL,0x8213AE,0,FnPtr(&X_Delete));
    return miss;
}

// Resumenes (nunca nulos, tambien para funciones void)
inline json Summary(const json& o){
    return json{{"ret",o["ret"]},{"events",o["trace"].size()},{"trace_fnv",Fnv64(o["trace"].dump().data(),o["trace"].dump().size())},
                {"world",o["world"]},{"eng",o["eng"]},{"globals",o["globals"]},{"fields_fnv",Fnv64(o.dump().data(),o.dump().size())}};
}
inline uint64 EngHash(std::array<uint8,ENG_SIZE>* dump){
    static std::array<uint8,ENG_SIZE> t; std::memcpy(t.data(),eng.data(),ENG_SIZE);
    At<uint32>(t.data(),0x1E34)=Id(At<void*>(t.data(),0x1E34));      // el puntero a m_GlobalWeaponAE se normaliza (memoria dinamica en InitialisePostLoading)
    if(dump)*dump=t; return Fnv64(t.data(),ENG_SIZE);
}
inline World wt;
inline uint64 WorldHash(){ std::memcpy(&wt,&w,sizeof(World)); At<uint32>(wt.gw,0)=0; return Fnv64(&wt,sizeof(World)); }   // vptr del GlobalWeapon falso enmascarado (el destructor de la DLL escribe el suyo)
inline void FixVolumes(){
    void* cur=*reinterpret_cast<void**>(0xBD00F8);
    if(cur&&uint32(uintptr_t(cur))!=volSaved&&cur!=fixtureVol)RealDeleteArr(cur);
    if(fixtureVol){ RealDeleteArr(fixtureVol); fixtureVol=nullptr; }
    *reinterpret_cast<uint32*>(0xBD00F8)=volSaved;
}
inline bool Layout(){
    bool ok=true; json l=json::object();
    auto chk=[&](const char* n,size_t got,size_t want){ const bool g=got==want; ok&=g; l[n]=g; };
    chk("sizeof_eng",sizeof(CAudioEngine),ENG_SIZE); chk("bg_channel",offsetof(CAudioEngine,m_nBackgroundAudioChannel),4); chk("beat",offsetof(CAudioEngine,m_BeatInfo),8);
    chk("frontend",offsetof(CAudioEngine,m_FrontendAE),0xB4); chk("script",offsetof(CAudioEngine,m_ScriptAE),0x2A0); chk("collision",offsetof(CAudioEngine,m_CollisionAE),0x4BC);
    chk("gw",offsetof(CAudioEngine,m_GlobalWeaponAE),0x1E34); chk("pedless",offsetof(CAudioEngine,m_PedlessSpeechAE),0x1E38); chk("door",offsetof(CAudioEngine,m_DoorAE),0x1F50);
    chk("sizeof_beat",sizeof(tBeatInfo),0xAC); chk("beat_present",offsetof(tBeatInfo,IsBeatInfoPresent),0xA0);
    chk("sizeof_cut",sizeof(CAECutsceneTrackManager),0x30); chk("sizeof_amb",sizeof(CAEAmbienceTrackManager),0x2C);
    chk("addr_engine",reinterpret_cast<uintptr_t>(&AudioEngine),0xB6BC90); chk("addr_cut",reinterpret_cast<uintptr_t>(&AECutsceneTrackManager),0x8AE554);
    chk("addr_amb",reinterpret_cast<uintptr_t>(&AEAmbienceTrackManager),0x8AC15C); chk("amb_override",offsetof(CAEAmbienceTrackManager,m_OverrideRadio),0);
    chk("fn_count",fns.size(),82);
    Write({{"event","layout"},{"ok",ok},{"checks",l}});
    return ok;
}
inline std::array<std::array<uint8,ENG_SIZE>,2> engDump{};

inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audioenginefinal4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audioenginefinal4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audioenginefinal4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"audioenginefinal4-only.txt"); if(f)std::getline(f,only); }

    std::array<ReversibleHooks::ReversibleHook::Base*,82> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    if(!Layout()){ Write({{"event","error"},{"reason","layout"}}); return; }
    const auto miss=RegisterStubs(); if(!miss.empty()){ Write({{"event","error"},{"reason","stubs not found"},{"missing",miss}}); return; }
    if(stubs.size()*3>PatchSet::MAXP){ Write({{"event","error"},{"reason","too many stubs for PatchSet"},{"stubs",stubs.size()}}); return; }
    for(auto& s:stubs)if(s.dll&&((s.dll>=0x400000&&s.dll<0x1000000)||(s.body>=0x400000&&s.body<0x1000000))){ Write({{"event","error"},{"reason","dll body inside exe"},{"exe",s.exe}}); return; }

    nregs=0; Reg(&w,sizeof(World)); Reg(eng.data(),eng.size());
    // Instantanea de globales (restauradas al final de cada ejecucion)
    saved.clear();
    Save(0xB6BC90,0x10);                 // cabecera del CAudioEngine REAL (no debe cambiar nunca)
    Save(0x8AE554,0x30);                 // CAECutsceneTrackManager
    Save(0x8AC15C,0x2C);                 // CAEAmbienceTrackManager
    Save(0xB5F944,0x84);                 // CAEAudioHardware: m_nNumAvailableChannels + m_anNumChannelsInSlot
    Save(0xB7CB48,2);                    // CTimer: pausas de codigo y de usuario
    Save(0xB5F851,1);                    // CCutsceneMgr::ms_running
    Save(0xBD00F8,4,false);              // tabla de volumenes de eventos (puntero; solo se restaura)
    volSaved=*reinterpret_cast<uint32*>(0xBD00F8); fixtureVol=nullptr;
    std::array<bool,82> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    for(auto* h:hooks)h->State(false);
    PrepareMasks();

    const unsigned savedFP=_controlfp(0,0);
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                Ctx c; c.fn=&fn; c.nm=fn.name; c.i=i; c.f=uint32(f); c.candidate=candidate;
                c.st=uint32(i+1)*2654435761u^uint32(f+1)*0x9E3779B9u^0xA0D1u; if(!c.st)c.st=1;
                nrec=0; recOverflow=false; fixtureVol=nullptr;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                c.a=fn.address; c.out=json::object(); c.ret=nullptr;
                _controlfp(_PC_24,_MCW_PC);
                const bool handled=RunCase(c);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                json out=c.out; out["switches"]=switches; out["handled"]=handled; out["ret"]=c.ret; out["trace"]=Recs();
                out["world"]=WorldHash(); out["eng"]= c.engCustom ? 0ull : EngHash(&engDump[candidate?1:0]);
                if(c.engCustom)std::memcpy(engDump[candidate?1:0].data(),eng.data(),ENG_SIZE);
                std::memset(reinterpret_cast<void*>(0x8AC185),0,3);          // relleno de tAmbienceSettings: la DLL copia la estructura entera, el EXE solo el byte
                uint64 g=1469598103934665603ull; for(auto& s:saved)if(s.hashed){ g^=Fnv64(reinterpret_cast<void*>(s.addr),s.bytes.size()); g*=1099511628211ull; } out["globals"]=g;
                FixVolumes(); RestoreAll();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["handled"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(), vals=json::object(); for(auto& [k,v]:original.items())if(candidate[k]!=v){ diff.push_back(k);
                       const auto so=v.dump(), sc=candidate[k].dump(); vals[k]={{"o",so.substr(0,400)},{"c",sc.substr(0,400)}}; }
                // bloques de 64 bytes del CAudioEngine falso que difieren entre lados (maximo 8, con los 32 primeros bytes de cada uno)
                json blocks=json::array(); int shown=0;
                for(size_t b=0;b<ENG_SIZE/64&&shown<8;++b)if(std::memcmp(engDump[0].data()+64*b,engDump[1].data()+64*b,64)!=0){
                    blocks.push_back({{"block",b},{"offset",64*b},{"original",ReversingProbe::Bytes(engDump[0].data()+64*b,32)},{"candidate",ReversingProbe::Bytes(engDump[1].data()+64*b,32)}}); ++shown; }
                // bytes del resumen de constructor/destructor que difieren (offset, marca original/candidata, dword crudo de cada lado, vptr incluido)
                json tagDiff=json::array();
                for(size_t b=0;b<ENG_SIZE&&tagDiff.size()<48;++b)if(tagDump[0][b]!=tagDump[1][b]){
                    const size_t sl=b&~size_t(3); uint32 ro{},rc{}; std::memcpy(&ro,engDump[0].data()+sl,4); std::memcpy(&rc,engDump[1].data()+sl,4);
                    tagDiff.push_back({{"off",b},{"o",tagDump[0][b]},{"c",tagDump[1][b]},{"raw_o",ro},{"raw_c",rc},{"vptr_slot",vptrSlot[sl]!=0}}); }
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"diff_values",vals},{"diff_blocks",blocks},{"tag_diff",tagDiff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    FixVolumes(); RestoreAll();
    const bool restored=AllRestored()&&_controlfp(0,0)==savedFP;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
