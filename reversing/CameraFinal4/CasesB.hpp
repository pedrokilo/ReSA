#pragma once
// Familia B: funciones que cambian referencias de entidad de la camara (Restore, TakeControl*, UpdateTargetEntity, SetCameraDirectly*...).
// RegisterReference/CleanUpOldReference, FindPlayerPed/FindPlayerVehicle, GetATanOfXY y las consultas de tareas se doblan con trazas.
#include "CasesA.hpp"

namespace CameraFinal4Live {
inline bool RunB(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; void* const cam=Cam(); auto& out=c.out; auto& ret=c.ret; (void)out; (void)ret;

    if(nm=="Restore"||nm=="RestoreWithJumpCut"){
        PREP(); auto P=MakePatches(G_REF|G_PLAYER);
        At<void*>(Cam(),0x958)= c.chance(4) ? nullptr : c.Ent();
        At<void*>(Cam(),0x95C)= c.chance(2) ? nullptr : c.Ent();
        CallThis<void>(a,cam); return true; }
    if(nm=="SetCameraDirectlyBehindForFollowPed_CamOnAString"||nm=="SetCameraDirectlyInFrontForFollowPed_CamOnAString"){
        PREP(); auto P=MakePatches(G_PLAYER|G_ATAN); CallThis<void>(a,cam); return true; }
    if(nm=="SetCameraDirectlyBehindForFollowPed_ForAPed_CamOnAString"||nm=="SetCameraDirectlyInFrontForFollowPed_ForAPed_CamOnAString"){
        PREP(); auto P=MakePatches(G_REF|G_ATAN);
        At<void*>(Cam(),0x958)= c.chance(2) ? nullptr : c.Ent();
        for(int k=0;k<3;++k)At<void*>(CamAt(k),0x21C)= c.chance(3) ? nullptr : c.Ent();
        void* ped= c.chance(8) ? nullptr : static_cast<void*>(c.Ped(int(c.pick(NPED))));
        CallThis<void>(a,cam,ped); return true; }
    if(nm=="UpdateTargetEntity"){
        PREP(); auto P=MakePatches(G_REF|G_PLAYER|G_TASK);
        At<void*>(Cam(),0x958)= c.chance(6) ? nullptr : c.Ent();
        for(int k=0;k<3;++k)At<void*>(CamAt(k),0x21C)= c.chance(3) ? nullptr : c.Ent();
        CallThis<void>(a,cam); return true; }
    if(nm=="TakeControl"){
        PREP(); auto P=MakePatches(G_REF|G_PLAYER);
        At<void*>(Cam(),0x958)=c.Ent();
        void* target= c.chance(4) ? nullptr : static_cast<void*>(c.Ent());
        const uint16 mode= c.chance(3) ? uint16(0) : kModes[c.pick(uint32(std::size(kModes)))]; const uint16 sw=uint16(c.pick(3)); const int32 who=int32(c.pick(3));
        CallThis<void>(a,cam,target,mode,sw,who); return true; }
    if(nm=="TakeControlAttachToEntity"){
        PREP(); auto P=MakePatches(G_REF|G_PLAYER);
        At<void*>(Cam(),0x958)=c.Ent();
        void* target= c.chance(2) ? nullptr : static_cast<void*>(c.Ent());
        void* attached= c.chance(3) ? nullptr : static_cast<void*>(c.Ent());
        CVector offset= c.chance(4) ? CVector{0.f,0.f,0.f} : c.Vec(); CVector look= c.chance(4) ? offset : c.Vec();
        const float tilt=c.M(); const uint16 sw=uint16(c.pick(3)); const int32 who=int32(c.pick(3));
        CallThis<void>(a,cam,target,attached,&offset,&look,tilt,sw,who); return true; }
    if(nm=="SetZoomValueCamStringScript"){
        PREP(); At<void*>(CamAt(0),0x21C)= c.chance(3) ? static_cast<void*>(c.Ped(int(c.pick(NPED)))) : static_cast<void*>(c.Veh(int(c.pick(NVEH))));
        CallThis<void>(a,cam,int16(int32(c.pick(7))-2)); return true; }
    return false;
}
}
