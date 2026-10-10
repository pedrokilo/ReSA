#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1/VehModelInfoFinal4): CCamera, 91 funciones (lista final 4).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato (hooks activos) sobre la camara real (TheCamera, guardada y
// restaurada en cada ejecucion) y un mundo falso y determinista (peds, vehiculos, matrices, tareas, modelo de colision). Los colaboradores
// del motor y de otras clases (referencias de entidad, FindPlayerPed/Vehicle, GetATanOfXY, tareas, audio, linea vertical, cono de colision,
// dibujo, matrices, operator delete, CCam::Init) se doblan con trazas (direccion del EXE y simbolo de la DLL). Ver README.md.
#include "CasesC.hpp"

namespace CameraFinal4Live {
inline const std::array<Fn,91> fns{{
    {"CCamera","AddShakeSimple",0x50D240,0,-1,256}, {"CCamera","AllowShootingWith2PlayersInCar",0x50C280,0,-1,64},
    {"CCamera","ApplyVehicleCameraTweaks",0x50A480,0,-1,256}, {"CCamera","CalculateGroundHeight",0x514B80,0,-1,512},
    {"CCamera","CalculateMirroredMatrix",0x50B380,0,-1,512}, {"CCamera","CamShake",0x50A9F0,0,-1,512}, {"CCamera","CamShakeNoPos",0x50A970,0,-1,256},
    {"CCamera","CameraGenericModeSpecialCases",0x50CD30,0,-1,128}, {"CCamera","CameraPedAimModeSpecialCases",0x50CDA0,0,-1,128},
    {"CCamera","CameraPedModeSpecialCases",0x50CD80,0,-1,64}, {"CCamera","CameraVehicleModeSpecialCases",0x50CDE0,0,-1,256},
    {"CCamera","ClearPlayerWeaponMode",0x50AB10,0,-1,64}, {"CCamera","ConeCastCollisionResolve",0x51A5D0,0,-1,512},
    {"CCamera","ConsiderPedAsDucking",0x50CEB0,0,-1,128}, {"CCamera","Constructor",0x51A450,0,-1,64},
    {"CCamera","DealWithMirrorBeforeConstructRenderList",0x50B510,0,-1,256}, {"CCamera","DeleteCutSceneCamDataMemory",0x5B24A0,0,-1,128},
    {"CCamera","DontProcessObbeCinemaCamera",0x50AB40,0,-1,64}, {"CCamera","DrawBordersForWideScreen",0x514860,0,-1,256},
    {"CCamera","Enable1rstPersonCamCntrlsScript",0x50AC00,0,-1,64}, {"CCamera","Enable1rstPersonWeaponsCamera",0x50AC10,0,-1,64},
    {"CCamera","Fade",0x50AC20,0,-1,512}, {"CCamera","Find3rdPersonCamTargetVector",0x514970,0,-1,512},
    {"CCamera","Find3rdPersonQuickAimPitch",0x50AD40,0,-1,256}, {"CCamera","FindCamFOV",0x50AD20,0,-1,64}, {"CCamera","FinishCutscene",0x514950,0,-1,256},
    {"CCamera","GetArrPosForVehicleType",0x50AF00,0,-1,128}, {"CCamera","GetCutSceneFinishTime",0x50AD90,0,-1,128},
    {"CCamera","GetFading",0x50ADE0,0,-1,64}, {"CCamera","GetFadingDirection",0x50ADF0,0,-1,64}, {"CCamera","GetGameCamPosition",0x50AE50,0,-1,64},
    {"CCamera","GetLookDirection",0x50AE90,0,-1,128}, {"CCamera","GetLookingForwardFirstPerson",0x50AED0,0,-1,128},
    {"CCamera","GetLookingLRBFirstPerson",0x50AE60,0,-1,128}, {"CCamera","GetPositionAlongSpline",0x50AF80,0,-1,64},
    {"CCamera","GetRoughDistanceToGround",0x516B00,0,-1,128}, {"CCamera","GetScreenFadeStatus",0x50AE20,0,-1,128},
    {"CCamera","GetScreenRect",0x50AB50,0,-1,256}, {"CCamera","Get_Just_Switched_Status",0x50AE10,0,-1,64}, {"CCamera","Init",0x5BC520,0,-1,128},
    {"CCamera","InitCameraVehicleTweaks",0x50A3B0,0,-1,128}, {"CCamera","InitialiseCameraForDebugMode",0x50AF90,0,-1,64},
    {"CCamera","InitialiseScriptableComponents",0x50D2D0,0,-1,64}, {"CCamera","IsExtraEntityToIgnore",0x50CE80,0,-1,256},
    {"CCamera","LerpFOV",0x50D280,0,-1,256}, {"CCamera","ProcessFOVLerp-1",0x516500,0,-1,512}, {"CCamera","ProcessFade",0x50B5D0,0,-1,512},
    {"CCamera","ProcessMusicFade",0x50B6D0,0,-1,512}, {"CCamera","ProcessObbeCinemaCameraPed",0x50B880,0,-1,64},
    {"CCamera","ProcessScriptedCommands",0x516AE0,0,-1,256}, {"CCamera","ProcessWideScreenOn",0x50B890,0,-1,128},
    {"CCamera","RenderMotionBlur",0x50B8F0,0,-1,64}, {"CCamera","ResetDuckingSystem",0x50CEF0,0,-1,256}, {"CCamera","Restore",0x50B930,0,-1,512},
    {"CCamera","RestoreCameraAfterMirror",0x51A5A0,0,-1,128}, {"CCamera","RestoreWithJumpCut",0x50BAB0,0,-1,512},
    {"CCamera","SetCamCollisionVarDataSet",0x50CB60,0,-1,128}, {"CCamera","SetCamCutSceneOffSet",0x50BD20,0,-1,64},
    {"CCamera","SetCamPositionForFixedMode",0x50BEC0,0,-1,64}, {"CCamera","SetCameraDirectlyBehindForFollowPed_CamOnAString",0x50BD40,0,-1,128},
    {"CCamera","SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString",0x50BDA0,0,-1,256},
    {"CCamera","SetCameraDirectlyInFrontForFollowPed_CamOnAString",0x50BD70,0,-1,128},
    {"CCamera","SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString",0x50BE30,0,-1,256}, {"CCamera","SetCameraUpForMirror",0x51A560,0,-1,128},
    {"CCamera","SetColVarsAimWeapon",0x50CBF0,0,-1,128}, {"CCamera","SetColVarsPed",0x50CC50,0,-1,256}, {"CCamera","SetColVarsVehicle",0x50CCA0,0,-1,256},
    {"CCamera","SetFadeColour",0x50BF00,0,-1,256}, {"CCamera","SetMotionBlur",0x50BF40,0,-1,128}, {"CCamera","SetMotionBlurAlpha",0x50BF80,0,-1,64},
    {"CCamera","SetNearClipScript",0x50BF90,0,-1,64}, {"CCamera","SetNewPlayerWeaponMode",0x50BFB0,0,-1,64},
    {"CCamera","SetParametersForScriptInterpolation",0x50C030,0,-1,128}, {"CCamera","SetPercentAlongCutScene",0x50C070,0,-1,256},
    {"CCamera","SetRwCamera",0x50C100,0,-1,128}, {"CCamera","SetWideScreenOff",0x50C150,0,-1,64}, {"CCamera","SetWideScreenOn",0x50C140,0,-1,64},
    {"CCamera","SetZoomValueCamStringScript",0x50C1B0,0,-1,256}, {"CCamera","SetZoomValueFollowPedScript",0x50C160,0,-1,128},
    {"CCamera","StartCooperativeCamMode",0x50C260,0,-1,64}, {"CCamera","StartTransition",0x515200,0,-1,1024},
    {"CCamera","StartTransitionWhenNotFinishedInter",0x515BC0,0,-1,256}, {"CCamera","StopCooperativeCamMode",0x50C270,0,-1,64},
    {"CCamera","StoreValuesDuringInterPol",0x50C290,0,-1,256}, {"CCamera","TakeControl",0x50C7C0,0,-1,512},
    {"CCamera","TakeControlAttachToEntity",0x50C910,0,-1,512}, {"CCamera","TakeControlNoEntity",0x50C8B0,0,-1,256},
    {"CCamera","TakeControlWithSpline",0x50CAE0,0,-1,128}, {"CCamera","UpdateAimingCoors",0x50CB10,0,-1,64},
    {"CCamera","UpdateTargetEntity",0x50C360,0,-1,1024}, {"CCamera","Using1stPersonWeaponMode",0x50BFF0,0,-1,128},
}};

// Registra los dobles (direccion del EXE + simbolo de la DLL). Devuelve el nombre de lo que falte.
inline std::string RegisterStubs(){
    stubs.clear();
    std::string miss;
    const auto hk=[&](uint32 g,uintptr_t exe,const char* cls,const char* name,void* fn){ if(!AddHookStub(g,exe,cls,name,fn))miss+=std::string(cls)+"::"+name+" "; };
    // referencias de entidad (miembros de CEntity, 1 argumento)
    AddStub(G_REF,0x571B70,MA(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FnPtr(&S_Reg));
    AddStub(G_REF,0x571A00,MA(static_cast<void (CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FnPtr(&S_Unref));
    // jugador
    AddStub(G_PLAYER,0x56E210,std::bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FnPtr(&S_FindPlayerPed));
    AddStub(G_PLAYER,0x56E0D0,std::bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FnPtr(&S_FindPlayerVehicle));
    // angulo
    AddStub(G_ATAN,0x53CC70,std::bit_cast<uintptr_t>(&CGeneral::GetATanOfXY),FnPtr(&S_ATan));
    // tareas, cooperativo, cerraduras
    AddStub(G_TASK,0x600FF0,MA(&CPedIntelligence::GetTaskHold),FnPtr(&S_GetTaskHold));
    AddStub(G_TASK,0x6010A0,MA(&CPedIntelligence::GetTaskDuck),FnPtr(&S_GetTaskDuck));
    AddStub(G_TASK,0x441390,std::bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn),FnPtr(&S_Coop));
    AddStub(G_TASK,0x6D1E20,MA(&CVehicle::CanPedOpenLocks),FnPtr(&S_CanOpen));
    // audio
    AddStub(G_AUDIO,0x506D90,MA(&CAudioEngine::IsLoadingTuneActive),FnPtr(&S_AudLoading));
    AddStub(G_AUDIO,0x506E40,MA(&CAudioEngine::SetMusicFaderScalingFactor),FnPtr(&S_AudMusic));
    AddStub(G_AUDIO,0x506E50,MA(&CAudioEngine::SetEffectsFaderScalingFactor),FnPtr(&S_AudFx));
    // mundo, colision, dibujo
    AddStub(G_VLINE,0x5674E0,std::bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FnPtr(&S_VLine));
    AddStub(G_CONE,0x41B000,std::bit_cast<uintptr_t>(&CCollision::CameraConeCastVsWorldCollision),FnPtr(&S_Cone));
    AddStub(G_DRAW,0x727B60,std::bit_cast<uintptr_t>(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),FnPtr(&S_DrawRect));
    // matrices
    AddStub(G_MATRIX,0x59BBC0,MA(static_cast<void (CMatrix::*)(const CMatrix&)>(&CMatrix::operator=)),FnPtr(&S_MAssign));
    AddStub(G_MATRIX,0x59BDD0,std::bit_cast<uintptr_t>(static_cast<CMatrix(*)(const CMatrix&)>(&Invert)),FnPtr(&S_Invert));
    AddStub(G_MATRIX,0x59AED0,MA(static_cast<void (CMatrix::*)(float)>(&CMatrix::SetScale)),FnPtr(&S_SetScale));
    AddStub(G_MATRIX,0x59BD10,MA(&CMatrix::Attach),FnPtr(&S_Attach));
    AddStub(G_MATRIX,0x54F610,MA(&CPlaceable::SetMatrix),FnPtr(&S_SetMatrix));
    // operator delete (la DLL reenvia al del EXE)
    AddStub(G_NEW,0x8214BD,0,FnPtr(&S_Delete));
    // camaras auxiliares
    AddStub(G_CAMINIT,0x50E490,MA(&CCam::Init),FnPtr(&S_CamInit));
    AddStub(G_TWOP,0x513E40,MA(&CCam::Get_TwoPlayer_AimVector),FnPtr(&S_TwoPlayer));
    // colaboradores de la propia clase
    hk(G_CAMSUB,0x50AFA0,"CCamera","CopyCameraMatrixToRWCam",FnPtr(&S_CopyRw));
    hk(G_CAMSUB,0x5150E0,"CCamera","CalculateDerivedValues",FnPtr(&S_Derived));
    hk(G_GROUND,0x514B80,"CCamera","CalculateGroundHeight",FnPtr(&S_Ground));
    hk(G_SCRIPTED,0x5164A0,"CCamera","ProcessVectorMoveLinear-1",FnPtr(&S_ScriptedNaked));
    hk(G_SCRIPTED,0x516440,"CCamera","ProcessVectorTrackLinear-1",FnPtr(&S_ScriptedNaked));
    hk(G_SCRIPTED,0x516500,"CCamera","ProcessFOVLerp-1",FnPtr(&S_ScriptedNaked));
    return miss;
}

// Resumen compacto y determinista (nunca nulo, tambien para funciones void): retorno, nº de eventos y hashes de traza, camara, mundo y globales.
inline json Summary(const json& o){
    const auto trace=o["trace"].dump();
    return json{{"ret",o["ret"]},{"events",o["trace"].size()},{"trace_fnv",Fnv64(trace.data(),trace.size())},{"cam",o["cam"]},{"world",o["world"]},
                {"globals",o["globals"]},{"fields_fnv",Fnv64(o.dump().data(),o.dump().size())}};
}

inline bool Layout(){
    bool ok=true; json l=json::object();
    const auto chk=[&](const char* n,size_t got,size_t want){ const bool g=got==want; ok&=g; l[n]=g ? json(got) : json{{"got",got},{"want",want}}; };
    chk("sizeof_camera",sizeof(CCamera),CAM_SIZE); chk("sizeof_cam",sizeof(CCam),0x238); chk("sizeof_matrix",sizeof(CMatrix),0x48); chk("sizeof_playerinfo",sizeof(CPlayerInfo),0x190);
    chk("active_cam",offsetof(CCamera,m_nActiveCam),0x59); chk("wide_screen",offsetof(CCamera,m_bWideScreenOn),0x3D); chk("fade_alpha",offsetof(CCamera,m_fFadeAlpha),0xBFC);
    chk("fade_flag",offsetof(CCamera,m_nFadeInOutFlag),0xC30); chk("target",offsetof(CCamera,m_pTargetEntity),0x958); chk("attached",offsetof(CCamera,m_pAttachedEntity),0x95C);
    chk("cams",offsetof(CCamera,m_aCams),0x174); chk("weapon_mode",offsetof(CCamera,m_PlayerWeaponMode),0x830); chk("paths",offsetof(CCamera,m_aPathArray),0x960);
    chk("cam_matrix",offsetof(CCamera,m_mCameraMatrix),0x974); chk("mode_to_go",offsetof(CCamera,m_nModeToGoTo),0xC38); chk("extra_count",offsetof(CCamera,m_nExtraEntitiesCount),0xC48);
    chk("extra",offsetof(CCamera,m_pExtraEntity),0xC4C); chk("tweaks",offsetof(CCamera,m_aCamTweak),0xCF4); chk("tweak_index",offsetof(CCamera,m_nCurrentTweakModelIndex),0xD54);
    chk("shake_force",offsetof(CCamera,m_fCamShakeForce),0x12C); chk("shake_start",offsetof(CCamera,m_nCamShakeStart),0x5C); chk("zoom_mode",offsetof(CCamera,m_nZoomMode),0xCB4);
    chk("cam_mode",offsetof(CCam,m_nMode),0xC); chk("cam_finish",offsetof(CCam,m_nFinishTime),0x10); chk("cam_dir",offsetof(CCam,m_nDirectionWasLooking),0x28);
    chk("cam_fov",offsetof(CCam,m_fFOV),0xB4); chk("cam_source",offsetof(CCam,m_vecSource),0x19C); chk("cam_front",offsetof(CCam,m_vecFront),0x190); chk("cam_target",offsetof(CCam,m_pCamTargetEntity),0x21C);
    chk("e_matrix",offsetof(CPlaceable,m_matrix),E_MATRIX); chk("e_model",offsetof(CEntity,m_nModelIndex),E_MODEL); chk("speed",offsetof(CPhysical,m_vecMoveSpeed),E_SPEED);
    chk("p_state",offsetof(CPed,m_nPedState),P_STATE); chk("p_veh",offsetof(CPed,m_pVehicle),P_VEH);
    chk("p_rot",offsetof(CPed,m_fCurrentRotation),P_ROT); chk("p_aim",offsetof(CPed,m_fAimingRotation),P_AIM);
    chk("v_tow",offsetof(CVehicle,m_pVehicleBeingTowed),V_TOW);
    chk("task_hold",offsetof(CTaskSimpleHoldEntity,m_pEntityToHold),8); chk("task_abort",offsetof(CTaskSimpleDuck,m_bIsAborting),0x19);
    chk("intel_taskmgr",offsetof(CPedIntelligence,m_TaskMgr),4); chk("rwcam_view",offsetof(RwCamera,viewMatrix),0x20);
    chk("rect_top",offsetof(CRect,top),4); chk("rect_right",offsetof(CRect,right),8); chk("rect_bottom",offsetof(CRect,bottom),0xC);
    chk("matrix_attach",offsetof(CMatrix,m_pAttachMatrix),0x40);
    {   // bits de CPed y tipo de entidad
        alignas(16) static uint8 pedBuf[0x800]{}; auto* ped=reinterpret_cast<CPed*>(pedBuf);
        std::memset(pedBuf,0,sizeof(pedBuf)); ped->bInVehicle=true; chk("bit_in_vehicle",At<uint32>(pedBuf,P_FLAGS),0x100u);
        std::memset(pedBuf,0,sizeof(pedBuf)); ped->bIsDucking=true; chk("bit_ducking",At<uint32>(pedBuf,P_FLAGS),0x4000000u);
        alignas(16) static uint8 entBuf[0x200]{}; std::memset(entBuf,0,sizeof(entBuf)); entBuf[E_TYPE]=0x0B; chk("entity_type",size_t(reinterpret_cast<CEntity*>(entBuf)->GetType()),3);
        entBuf[E_TYPE]=0x0A; chk("entity_type_vehicle",size_t(reinterpret_cast<CEntity*>(entBuf)->GetType()),2);
    }
    {   // RwGlobals: fpRenderStateSet en +0x20
        static RwGlobals g{}; chk("rw_renderstate",uintptr_t(&g.dOpenDevice.fpRenderStateSet)-uintptr_t(&g),0x20);
    }
    chk("eFocusedPlayer_none",size_t(eFocusedPlayer::NONE),2); chk("model_rcgoblin",size_t(MODEL_RCGOBLIN),501);
    chk("ped_enter_car",size_t(PEDSTATE_ENTER_CAR),0x3B); chk("ped_exit_car",size_t(PEDSTATE_EXIT_CAR),0x3D); chk("ped_arrested",size_t(PEDSTATE_ARRESTED),0x3F);
    chk("ped_dragged",size_t(PEDSTATE_DRAGGED_FROM_CAR),0x3A); chk("ped_carjack",size_t(PEDSTATE_CARJACK),0x39); chk("ped_open_door",size_t(PEDSTATE_OPEN_DOOR),0x35);
    chk("task_gang_driveby",size_t(TASK_SIMPLE_GANG_DRIVEBY),0x3FE);
    Write({{"event","layout"},{"ok",ok},{"checks",l}});
    return ok;
}

inline std::array<std::array<uint8,CAM_SIZE>,2> camDump{}; inline std::array<std::array<uint8,sizeof(World)>,2> worldDump{};
inline json Blocks(const uint8* x,const uint8* y,size_t n,int maxBlocks){
    json blocks=json::array(); int shown=0;
    for(size_t b=0;b<n/16&&shown<maxBlocks;++b)if(std::memcmp(x+16*b,y+16*b,16)!=0){ blocks.push_back({{"offset",16*b},{"original",ReversingProbe::Bytes(x+16*b,16)},{"candidate",ReversingProbe::Bytes(y+16*b,16)}}); ++shown; }
    return blocks;
}

// Bytes que difieren de la camara: desplazamiento, valor del original y del candidato (maximo `maxBytes`)
inline json Bytes(const uint8* x,const uint8* y,size_t n,int maxBytes){
    json r=json::array(); int shown=0;
    for(size_t k=0;k<n&&shown<maxBytes;++k)if(x[k]!=y[k]){ r.push_back({{"offset",k},{"original",x[k]},{"candidate",y[k]}}); ++shown; }
    return r;
}

inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"camerafinal4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"camerafinal4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("camerafinal4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"camerafinal4-only.txt"); if(f)std::getline(f,only); }

    std::array<ReversibleHooks::ReversibleHook::Base*,91> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    if(!Layout()){ Write({{"event","error"},{"reason","layout"}}); return; }
    const auto miss=RegisterStubs(); if(!miss.empty()){ Write({{"event","error"},{"reason","stubs not found"},{"missing",miss}}); return; }

    nregs=0; Reg(Cam(),CAM_SIZE); Reg(&w,sizeof(World));
    // Instantanea de globales (restauradas al final de cada ejecucion)
    saved.clear();
    Save(TCAM,CAM_SIZE);                     // TheCamera completa
    Save(0xB7CD98,0x320);                    // CWorld::Players
    Save(0xB7CD70,0x10);                     // PlayerInFocus
    Save(0xB7CB40,0x50);                     // CTimer (paso y milisegundos)
    Save(0xC3EF98,0x20);                     // CDraw (aspecto, colores y valor del fundido)
    Save(0xC17040,0x10);                     // RsGlobal (ancho y alto)
    Save(0x8CCB80,0x10);                     // gCurCamColVars / gCurDistForCam
    Save(0xB6FE40,0x50);                     // matriz previa al espejo y gpCamColVars
    Save(0x8A5B10,0x10);                     // CCollision::bCamCollideWith* y relVelCamCollisionVehiclesSqr
    Save(0x8A5E50,4);                        // CGameLogic::n2PlayerPedInFocus
    Save(0xB70020,0x30);                     // estaticas de CalculateGroundHeight y de StartTransition
    Save(0xB6EC10,0x30);                     // constantes de mira y de raton, banderas de camara
    Save(0x8CC380,0x10);                     // gPlayerPedVisible y aledanos
    Save(0xBA67A0,8);                        // CMenuManager::m_bStartGameLoading
    Save(Ctx::MODELPTR_BASE+4u*uint32(Ctx::FAKE_MODEL),4);   // ranura de ModelInfo del edificio falso
    std::array<bool,91> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    const unsigned savedFP=_controlfp(0,0);
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                Ctx c; c.fn=&fn; c.nm=fn.name; c.i=i; c.f=uint32(f); c.candidate=candidate; c.a=fn.address;
                c.st=uint32(i+1)*2654435761u^uint32(f+1)*0x9E3779B9u^0xCA3E4A4u; if(!c.st)c.st=1;
                nrec=0; recOverflow=false;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                c.out=json::object(); c.ret=nullptr;
                _controlfp(_PC_24,_MCW_PC);
                const bool handled=RunA(c)||RunB(c)||RunC(c);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                json out=c.out; out["switches"]=switches; out["handled"]=handled; out["ret"]=c.ret; out["trace"]=Recs();
                out["cam"]=Fnv64(Cam(),CAM_SIZE); out["world"]=Fnv64(&w,sizeof(World)); out["globals"]=GlobalsHash();
                std::memcpy(camDump[candidate?1:0].data(),Cam(),CAM_SIZE); std::memcpy(worldDump[candidate?1:0].data(),&w,sizeof(World));
                RestoreAll();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["handled"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(), vals=json::object(); for(auto& [k,v]:original.items())if(candidate[k]!=v){ diff.push_back(k);
                       const auto so=v.dump(), sc=candidate[k].dump(); vals[k]={{"o",so.substr(0,300)},{"c",sc.substr(0,300)}}; }
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"diff_values",vals},
                       {"diff_cam_bytes",Bytes(camDump[0].data(),camDump[1].data(),CAM_SIZE,48)},{"diff_blocks",Blocks(camDump[0].data(),camDump[1].data(),CAM_SIZE,12)},{"diff_world",Blocks(worldDump[0].data(),worldDump[1].data(),sizeof(World),8)},
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
