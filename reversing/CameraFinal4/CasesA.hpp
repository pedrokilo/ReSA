#pragma once
// Familia A: funciones escalares sobre la camara real (TheCamera), temporizadores, fundidos, mascaras de colision y calculos de rectangulos.
// Cada caso rellena el mundo falso, la camara y los globales con el dominio del juego, llama a la funcion por su direccion del EXE
// (con el hook apagado ejecuta el EXE, con el hook activo la DLL) y deja las salidas en c.out / c.ret.
#include "Ctx.hpp"

namespace CameraFinal4Live {
#define PREP() do{ c.FillWorld(); c.FillCamera(); c.FillGlobals(); }while(0)
inline uint8* CamAt(int k){ return Cam()+0x174+0x238*k; }
inline uint8* ActiveCam(){ return CamAt(Cam()[0x59]%3); }
inline void SetMode(uint8* cam,uint16 m){ At<uint16>(cam,0x0C)=m; }
inline void Mem8(uintptr_t a,uint8 v){ *reinterpret_cast<uint8*>(a)=v; }

inline bool RunA(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; void* const cam=Cam(); auto& out=c.out; auto& ret=c.ret;

    if(nm=="AddShakeSimple"){ PREP(); const float d=c.M(); const int32 t=int32(c.next()%6); const float in=c.M(); CallThis<void>(a,cam,d,t,in); return true; }
    if(nm=="AllowShootingWith2PlayersInCar"){ PREP(); CallThis<void>(a,cam,bool(c.next()&1)); return true; }
    if(nm=="ApplyVehicleCameraTweaks"){
        PREP(); uint8* v=c.Veh(int(c.pick(NVEH))); const int32 model=int32(At<int16>(v,E_MODEL));
        for(int k=0;k<5;++k)if(c.chance(3))At<int32>(Cam(),0xCF4+16*k)=model;
        if(c.chance(4))At<int32>(Cam(),0xD54)=model;
        CallThis<void>(a,cam,static_cast<void*>(v)); return true; }
    if(nm=="CalculateGroundHeight"){
        PREP(); auto P=MakePatches(G_VLINE);
        At<void*>(Cam(),0x14)= c.chance(3) ? nullptr : static_cast<void*>(Cam()+0x974);
        const float* pos= At<void*>(Cam(),0x14) ? reinterpret_cast<const float*>(Cam()+0x974+0x30) : reinterpret_cast<const float*>(Cam()+4);
        *reinterpret_cast<uint32*>(0xB70040)= c.chance(4) ? (c.next()&~1u) : (c.next()|1u);
        const bool near_= c.chance(2);
        for(int k=0;k<3;++k)*reinterpret_cast<float*>(0xB70034+4*k)= pos[k]+c.Unit()*(near_ ? 5.f : 60.f);
        for(int k=0;k<3;++k)*reinterpret_cast<float*>(0xB70028+4*k)=c.M();
        const int32 type= c.chance(10) ? int32(c.next()%7) : int32(c.next()%3);
        ret=F(CallThis<float>(a,cam,type)); out["g"]=c.Bytes(reinterpret_cast<void*>(0xB70028),0x1C); return true; }
    if(nm=="CamShake"){
        PREP(); const float s=c.M(); const float* src=reinterpret_cast<const float*>(ActiveCam()+0x19C);
        const CVector from= c.chance(6) ? CVector{src[0],src[1],src[2]} : (c.chance(3) ? CVector{src[0]+c.Unit()*20.f,src[1]+c.Unit()*20.f,src[2]} : c.Vec());
        CallThis<void>(a,cam,s,from); return true; }
    if(nm=="CamShakeNoPos"){ PREP(); const float s=c.M(); CallCdecl<void>(a,cam,s); return true; }
    if(nm=="CameraGenericModeSpecialCases"){
        PREP(); auto P=MakePatches(G_TASK); void* ped= c.chance(8) ? nullptr : static_cast<void*>(c.Ped(int(c.pick(NPED))));
        CallThis<void>(a,cam,ped); return true; }
    if(nm=="CameraPedAimModeSpecialCases"){
        PREP(); At<int32>(Cam(),0xC48)=int32(c.pick(2)); CallThis<void>(a,cam,static_cast<void*>(c.Ped(int(c.pick(NPED))))); return true; }
    if(nm=="CameraPedModeSpecialCases"){ PREP(); CallThis<void>(a,cam,static_cast<void*>(c.Ped(int(c.pick(NPED))))); return true; }
    if(nm=="CameraVehicleModeSpecialCases"){
        PREP(); At<int32>(Cam(),0xC48)=int32(c.pick(2)); CallThis<void>(a,cam,static_cast<void*>(c.Veh(int(c.pick(NVEH))))); return true; }
    if(nm=="ClearPlayerWeaponMode"){ PREP(); CallThis<void>(a,cam); return true; }
    if(nm=="ConeCastCollisionResolve"){
        PREP(); auto P=MakePatches(G_CONE);
        const CVector pos=c.Vec(); const CVector look= c.chance(5) ? pos : c.Vec(); CVector dest{c.M(),c.M(),c.M()}; float dist=c.M();
        const float radius=c.M(), minDist=c.M();
        const bool r=CallThis<bool>(a,cam,&pos,&look,&dest,radius,minDist,&dist);
        ret=int(r); out["dest"]=c.Bytes(&dest,12); out["dist"]=F(dist); return true; }
    if(nm=="ConsiderPedAsDucking"){
        PREP(); auto P=MakePatches(G_TASK); ret=int(CallThis<bool>(a,cam,static_cast<void*>(c.Ped(int(c.pick(NPED)))))); return true; }
    if(nm=="DontProcessObbeCinemaCamera"){ PREP(); Mem8(0xB6EC2D,uint8(c.next())); CallCdecl<void>(a); out["flag"]=*reinterpret_cast<uint8*>(0xB6EC2D); return true; }
    if(nm=="DeleteCutSceneCamDataMemory"){
        PREP(); auto P=MakePatches(G_NEW);
        for(int k=0;k<4;++k)At<void*>(Cam(),0x960+4*k)= c.chance(3) ? nullptr : static_cast<void*>(w.path[k]);
        CallThis<void>(a,cam); return true; }
    if(nm=="Enable1rstPersonCamCntrlsScript"||nm=="Enable1rstPersonWeaponsCamera"||nm=="ProcessObbeCinemaCameraPed"||nm=="InitialiseScriptableComponents"||nm=="InitCameraVehicleTweaks"
       ||nm=="InitialiseCameraForDebugMode"||nm=="ProcessWideScreenOn"||nm=="SetWideScreenOn"||nm=="SetWideScreenOff"||nm=="StartCooperativeCamMode"||nm=="StopCooperativeCamMode"){
        PREP(); *reinterpret_cast<int32*>(0x8A5E50)=int32(c.pick(3));
        if(nm=="ProcessWideScreenOn"){ SetMode(ActiveCam(),kModes[c.pick(uint32(std::size(kModes)))]); }
        CallThis<void>(a,cam); return true; }
    if(nm=="Fade"){
        PREP(); const float d=c.chance(8) ? 0.f : c.M(); const uint16 dir= c.chance(10) ? 2 : uint16(c.pick(2));
        CallThis<void>(a,cam,d,dir); return true; }
    if(nm=="FindCamFOV"){ PREP(); ret=F(CallThis<float>(a,cam)); return true; }
    if(nm=="Find3rdPersonQuickAimPitch"){ PREP(); ret=F(CallThis<float>(a,cam)); return true; }
    if(nm=="FinishCutscene"){
        PREP(); if(c.chance(2))SetMode(CamAt(int(c.pick(2))),17); CallThis<void>(a,cam); return true; }
    if(nm=="GetArrPosForVehicleType"){
        PREP(); int32 arr=int32(c.next()); const int32 type=int32(c.pick(16))-3;
        const bool r=CallThis<bool>(a,cam,type,&arr); ret=int(r); out["arr"]=arr; return true; }
    if(nm=="GetCutSceneFinishTime"){
        PREP(); if(c.chance(2))SetMode(CamAt(int(c.pick(3))),17); ret=CallThis<uint32>(a,cam); return true; }
    if(nm=="GetFading"){ PREP(); ret=int(CallThis<bool>(a,cam)); return true; }
    if(nm=="GetFadingDirection"){ PREP(); At<uint16>(Cam(),0xC30)=uint16(c.pick(3)); ret=CallThis<int32>(a,cam); return true; }
    if(nm=="GetGameCamPosition"){ PREP(); ret=Id(CallThis<void*>(a,cam)); return true; }
    if(nm=="GetLookDirection"||nm=="GetLookingForwardFirstPerson"||nm=="GetLookingLRBFirstPerson"){
        PREP(); static constexpr uint16 modes[]{16,18,22,4,5,1,10,16,16}; uint8* ac=ActiveCam();
        if(c.chance(5))SetMode(ac,modes[c.pick(uint32(std::size(modes)))]); At<uint32>(ac,0x28)= c.chance(2) ? 3u : c.pick(5);
        if(nm=="GetLookDirection")ret=CallThis<int32>(a,cam); else ret=int(CallThis<bool>(a,cam)); return true; }
    if(nm=="GetPositionAlongSpline"){ PREP(); ret=F(CallThis<float>(a,cam)); return true; }
    if(nm=="GetScreenFadeStatus"){
        PREP(); At<float>(Cam(),0xBFC)= c.chance(3) ? 0.f : (c.chance(2) ? 255.f : c.M()); ret=CallThis<int32>(a,cam); return true; }
    if(nm=="Get_Just_Switched_Status"){ PREP(); ret=int(CallThis<bool>(a,cam)); return true; }
    if(nm=="GetRoughDistanceToGround"){ PREP(); auto P=MakePatches(G_GROUND); ret=F(CallThis<float>(a,cam)); return true; }
    if(nm=="GetScreenRect"){
        PREP(); CRect r; std::memset(&r,0xAB,sizeof(r)); if(c.chance(3))At<float>(Cam(),0x144)=0.f; CallThis<void>(a,cam,&r); out["rect"]=c.Bytes(&r,16); return true; }
    if(nm=="IsExtraEntityToIgnore"){
        PREP(); At<int32>(Cam(),0xC48)=int32(c.pick(3));
        void* e0=c.Ent(); void* e1=c.Ent(); At<void*>(Cam(),0xC4C)=e0; At<void*>(Cam(),0xC50)=e1;
        void* arg= c.chance(6) ? nullptr : (c.chance(2) ? e1 : (c.chance(2) ? e0 : static_cast<void*>(c.Ent())));
        ret=int(CallThis<bool>(a,cam,arg)); return true; }
    if(nm=="LerpFOV"){ PREP(); const float zi=c.M(), zo=c.M(), tl=c.M(); CallThis<void>(a,cam,zi,zo,tl,bool(c.next()&1)); return true; }
    if(nm=="ProcessFOVLerp-1"){
        PREP(); const uint32 now=c.TimeNow(); At<float>(Cam(),0xCA4)=c.TimeNear(now); At<float>(Cam(),0xCA8)= c.chance(4) ? At<float>(Cam(),0xCA4) : c.TimeNear(now);
        CallThis<void>(a,cam); return true; }
    if(nm=="ProcessFade"){
        PREP(); static constexpr uint16 flags[]{0,1,1,0,2};
        At<uint16>(Cam(),0xC30)=flags[c.pick(5)]; Cam()[0x51]= c.chance(8) ? 0 : 1;
        At<float>(Cam(),0xC04)= c.chance(5) ? 0.f : c.M(); const uint32 k=c.pick(5);
        At<float>(Cam(),0xBFC)= k==0 ? 0.f : (k==1 ? 255.f : (k==2 ? c.uni(-5.f,260.f) : c.M()));
        CallThis<void>(a,cam); out["fade"]=*reinterpret_cast<uint8*>(0xC3EFAB); return true; }
    if(nm=="ProcessMusicFade"){
        PREP(); auto P=MakePatches(G_AUDIO); static constexpr uint16 dirs[]{0,1,1,0,2};
        At<uint16>(Cam(),0xC3A)=dirs[c.pick(5)]; Cam()[0x52]= c.chance(8) ? 0 : 1;
        const uint32 k=c.pick(5);
        At<float>(Cam(),0xC0C)= k==0 ? 0.f : (k==1 ? c.uni(0.f,0.3f) : c.M());
        At<float>(Cam(),0xC08)= c.chance(5) ? 0.f : c.M();
        At<float>(Cam(),0xC00)= c.chance(4) ? 0.f : (c.chance(3) ? 1.f : c.uni(-0.5f,1.5f));
        CallThis<void>(a,cam); return true; }
    if(nm=="ProcessScriptedCommands"){ PREP(); auto P=MakePatches(G_SCRIPTED); CallThis<void>(a,cam); return true; }
    if(nm=="RenderMotionBlur"){ PREP(); CallThis<void>(a,cam); return true; }
    if(nm=="ResetDuckingSystem"){
        PREP(); auto P=MakePatches(G_TASK); void* ped= c.chance(8) ? nullptr : static_cast<void*>(c.Ped(int(c.pick(NPED)))); CallThis<void>(a,cam,ped); return true; }
    if(nm=="SetCamCollisionVarDataSet"){
        PREP(); *reinterpret_cast<uint8*>(0x8CCB80)=uint8(c.pick(28)); *reinterpret_cast<float*>(0x8CCB84)=c.M(); *reinterpret_cast<void**>(0xB6FE88)=nullptr;
        CallCdecl<void>(a,int32(c.pick(28))); out["p"]=Val(*reinterpret_cast<uint32*>(0xB6FE88)); return true; }
    if(nm=="SetColVarsAimWeapon"||nm=="SetColVarsPed"||nm=="SetColVarsVehicle"){
        PREP(); *reinterpret_cast<uint8*>(0x8CCB80)=uint8(c.pick(28)); *reinterpret_cast<float*>(0x8CCB84)=c.M(); *reinterpret_cast<void**>(0xB6FE88)=nullptr;
        if(nm=="SetColVarsAimWeapon")CallCdecl<void>(a,int32(c.pick(8))-2);
        else if(nm=="SetColVarsPed")CallCdecl<void>(a,int32(c.pick(8)),int32(c.pick(4)));
        else CallCdecl<void>(a,int32(c.pick(15))-2,int32(c.pick(4)));
        out["p"]=Val(*reinterpret_cast<uint32*>(0xB6FE88)); return true; }
    if(nm=="SetCamCutSceneOffSet"||nm=="UpdateAimingCoors"){ PREP(); const CVector v=c.Vec(); CallThis<void>(a,cam,&v); return true; }
    if(nm=="SetCamPositionForFixedMode"){ PREP(); const CVector v=c.Vec(), u=c.Vec(); CallThis<void>(a,cam,&v,&u); return true; }
    if(nm=="SetFadeColour"){
        PREP(); const bool special=c.chance(3); const uint8 r=special ? 2 : uint8(c.next()), g=special ? 2 : uint8(c.next()), b=special&&!c.chance(3) ? 2 : uint8(c.next());
        CallThis<void>(a,cam,r,g,b); return true; }
    if(nm=="SetMotionBlur"){
        PREP(); const uint32 r=c.chance(2) ? c.pick(256) : c.pick(1000), g=c.pick(300), b=c.pick(300), type=c.pick(10); const int32 value=int32(c.next()%500)-100;
        CallThis<void>(a,cam,r,g,b,value,type); return true; }
    if(nm=="SetMotionBlurAlpha"){ PREP(); CallThis<void>(a,cam,int32(c.next())); return true; }
    if(nm=="SetNearClipScript"){ PREP(); CallThis<void>(a,cam,c.M()); return true; }
    if(nm=="SetNewPlayerWeaponMode"){
        PREP(); const uint16 mode=kModes[c.pick(uint32(std::size(kModes)))]; const int16 mn=int16(c.next()), mx=int16(c.next());
        CallThis<void>(a,cam,mode,mn,mx); return true; }
    if(nm=="SetParametersForScriptInterpolation"){ PREP(); const float s=c.M(), u=c.M(); CallThis<void>(a,cam,s,u,c.next()); return true; }
    if(nm=="SetPercentAlongCutScene"){
        PREP(); if(c.chance(2))SetMode(CamAt(int(c.pick(3))),17); if(c.chance(4))At<uint32>(CamAt(int(c.pick(3))),0x10)=c.Time();
        CallThis<void>(a,cam,c.chance(6) ? 100.f : c.M()); return true; }
    if(nm=="SetZoomValueFollowPedScript"){ PREP(); CallThis<void>(a,cam,int16(int32(c.pick(7))-2)); return true; }
    if(nm=="Using1stPersonWeaponMode"){
        PREP(); static constexpr uint16 modes[]{7,0x22,8,0x33,0x2D,0x2E,0x41,0,4,0x21,0x23,0x40,0x42,16,18};
        At<uint16>(Cam(),0x830)=modes[c.pick(uint32(std::size(modes)))]; ret=int(CallThis<bool>(a,cam)); return true; }
    if(nm=="TakeControlNoEntity"){
        PREP(); const CVector v=c.Vec(); At<uint32>(Cam(),0xB0)=c.pick(3); const uint16 sw=uint16(c.pick(3)); const int32 who=int32(c.pick(3));
        CallThis<void>(a,cam,&v,sw,who); return true; }
    if(nm=="TakeControlWithSpline"){ PREP(); CallThis<void>(a,cam,uint16(c.pick(3))); return true; }
    if(nm=="StoreValuesDuringInterPol"){
        PREP(); auto P=MakePatches(G_ATAN); CVector s=c.Vec(), t=c.Vec(), u=c.Vec(); float fov=c.M();
        CallThis<void>(a,cam,&s,&t,&u,&fov); return true; }
    return false;
}
}
