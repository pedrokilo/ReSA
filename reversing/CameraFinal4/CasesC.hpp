#pragma once
// Familia C: geometria y espejo (CalculateMirroredMatrix, DealWithMirror..., SetCameraUpForMirror, RestoreCameraAfterMirror), apuntado en
// tercera persona, bordes de pantalla ancha, SetRwCamera, StartTransition, Init y Constructor.
#include "CasesB.hpp"

namespace CameraFinal4Live {
inline void RandomPreMirror(Ctx& c){ alignas(16) uint8 tmp[0x50]; Ctx::MakeMatrix(tmp,c,c.chance(2)); std::memcpy(reinterpret_cast<void*>(0xB6FE40),tmp,0x48); }

inline bool RunC(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; void* const cam=Cam(); auto& out=c.out; auto& ret=c.ret;

    if(nm=="CalculateMirroredMatrix"){
        PREP(); const CVector n= c.chance(2) ? CVector{c.Unit(),c.Unit(),c.Unit()} : c.Vec(); const float v=c.M();
        CallThis<void>(a,cam,n,v,static_cast<void*>(w.mat[0]),static_cast<void*>(w.mat[1]));
        out["mirror"]=c.Bytes(w.mat[1],0x40); return true; }
    if(nm=="DealWithMirrorBeforeConstructRenderList"){
        PREP(); auto P=MakePatches(G_MATRIX);
        const bool active= !c.chance(4); const CVector n=c.Vec(); const float v=c.M(); void* m= c.chance(2) ? nullptr : static_cast<void*>(w.mat[2]);
        CallThis<void>(a,cam,active,n,v,m); return true; }
    if(nm=="DrawBordersForWideScreen"){
        PREP(); auto P=MakePatches(G_DRAW); RenderStatePatch rs(true);
        if(c.chance(2))At<float>(Cam(),0x144)=c.uni(0.f,50.f);
        CallThis<void>(a,cam); return true; }
    if(nm=="Find3rdPersonCamTargetVector"){
        PREP(); auto P=MakePatches(G_TWOP); if(c.chance(3))SetMode(ActiveCam(),49);
        const float range=c.M(); const CVector muzzle=c.Vec(); CVector os{c.M(),c.M(),c.M()}, ot{c.M(),c.M(),c.M()};
        CallThis<void>(a,cam,range,muzzle,&os,&ot);
        out["source"]=c.Bytes(&os,12); out["target"]=c.Bytes(&ot,12); return true; }
    if(nm=="Init"){
        PREP(); auto P=MakePatches(G_REF|G_PLAYER|G_CAMINIT|G_MATRIX);
        *reinterpret_cast<uint8*>(0xBA67A5)=uint8(c.pick(3));
        CallThis<void>(a,cam); return true; }
    if(nm=="Constructor"){
        PREP(); auto P=MakePatches(G_CAMINIT);
        c.fill(w.ctor,sizeof(w.ctor));
        ret=Id(CallThis<void*>(a,static_cast<void*>(CtorObject()))); return true; }
    if(nm=="RestoreCameraAfterMirror"||nm=="SetCameraUpForMirror"){
        PREP(); auto P=MakePatches(G_MATRIX|G_CAMSUB); RandomPreMirror(c);
        CallThis<void>(a,cam); return true; }
    if(nm=="SetRwCamera"){
        PREP(); auto P=MakePatches(G_MATRIX); c.fill(w.rwcam,sizeof(w.rwcam));
        CallThis<void>(a,cam,static_cast<void*>(w.rwcam)); return true; }
    if(nm=="StartTransition"||nm=="StartTransitionWhenNotFinishedInter"){
        PREP(); auto P=MakePatches(G_REF|G_PLAYER|G_ATAN|G_CAMINIT);
        At<void*>(Cam(),0x958)=c.Ent();
        for(int k=0;k<3;++k)At<void*>(CamAt(k),0x21C)= c.chance(3) ? nullptr : c.Ent();
        *reinterpret_cast<float*>(0xB70044)= c.chance(2) ? 0.f : c.M();
        const uint16 mode=kModes[c.pick(uint32(std::size(kModes)))];
        CallThis<void>(a,cam,mode); return true; }
    return false;
}
}
