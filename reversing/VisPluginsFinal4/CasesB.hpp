#pragma once
// Familia C: funciones de render con RW falso (atomics, clumps, geometrias y materiales falsos; render real sustituido por una traza).
#include "CasesA.hpp"

namespace VisPluginsFinal4Live {
inline int32 PickAlpha(Ctx& c){ switch(c.next()%10){ case 0: return 255; case 1: return 0; case 2: return -int32(c.next()%300); case 3: return 256+int32(c.next()%300); case 4: return 128;
        case 5: return int32(c.next()); default: return int32(c.next()%256); } }

inline bool RunC(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; auto& out=c.out; auto& ret=c.ret;
    auto lists=[&]{ out["alpha"]=c.DumpList(L_ALPHA); out["boat"]=c.DumpList(L_BOAT); };
    // Atomics de vehiculo: se rellenan alfa y limites; la primera caida a "AtomicDefaultRenderCallBack" se ve en la traza
    const bool vehCb = nm=="RenderVehicleReallyLowDetailCB"||nm=="RenderVehicleHiDetailCB"||nm=="RenderVehicleHiDetailAlphaCB"||nm=="RenderTrainHiDetailCB"||nm=="RenderTrainHiDetailAlphaCB"
        ||nm=="RenderVehicleReallyLowDetailCB_BigVehicle"||nm=="RenderVehicleHiDetailCB_BigVehicle"||nm=="RenderVehicleHiDetailAlphaCB_BigVehicle"||nm=="RenderVehicleLoDetailCB_Boat"
        ||nm=="RenderVehicleHiDetailCB_Boat"||nm=="RenderVehicleHiDetailAlphaCB_Boat"||nm=="RenderHeliRotorAlphaCB"||nm=="RenderHeliTailRotorAlphaCB";
    if(vehCb){
        c.BuildScene(); c.SetDists(); c.BuildAllLists();
        uint8* at=c.RndAt(); void* r;
        { auto P=MakePatches(G_RW); r=CallCdecl<void*>(a,static_cast<void*>(at)); }
        ret=Id(r); out["fl"]=At<uint16>(at,atomOff+2); lists(); return true; }
    if(nm=="RenderPedCB"||nm=="RenderFadingClumpCB"||nm=="RenderWeaponCB"||nm=="RenderPlayerCB"||nm=="RenderObjNormalAtomic"){
        c.BuildScene(); c.SetDists(false); c.SetCameraMults();
        uint8* at=c.RndAt(); uint8* clump=At<uint8*>(at,0x3C);
        At<uint32>(At<uint8*>(clump,4),frameOff)=uint32(uintptr_t(w.mi[c.next()%NMI]));     // CClumpModelInfo del frame (RenderWeaponCB)
        if(c.chance(3))At<int32>(clump,clumpOff+4)=255;
        if(nm=="RenderPlayerCB")Gu(0xB7CF1C)= c.chance(3) ? 0u : uint32(uintptr_t(w.tex[c.next()%NTX]));
        void* r; { auto P=MakePatches(G_RW); r=CallCdecl<void*>(a,static_cast<void*>(at)); }
        ret=Id(r); json m=json::array(); for(int k=0;k<NMAT;++k)m.push_back(Id(At<void*>(w.mat[k],0))); out["tex"]=m; return true; }

    if(nm=="RenderAtomicWithAlpha"){
        c.BuildScene(); uint8* at=c.RndAt(); const int32 alpha=PickAlpha(c);
        { auto P=MakePatches(G_RW); CallCdecl<void>(a,static_cast<void*>(at),alpha); }
        json al=json::array(); for(int k=0;k<NMAT;++k)al.push_back(w.mat[k][7]); out["alphas"]=al; out["gflags"]=At<uint32>(At<uint8*>(at,0x18),8); out["alpha"]=alpha; return true; }
    if(nm=="RenderAtomicWithAlphaCB"){
        c.BuildScene(); uint8* at=c.RndAt(); if(c.chance(3))at[2]&=~4u;
        At<int32>(w.vec,0x20)=PickAlpha(c); void* r;
        { auto P=MakePatches(G_RW); r=CallCdecl<void*>(a,static_cast<void*>(at),static_cast<void*>(w.vec+0x20)); }
        ret=Id(r); json al=json::array(); for(int k=0;k<NMAT;++k)al.push_back(w.mat[k][7]); out["alphas"]=al; return true; }
    if(nm=="RenderFadingAtomic"||nm=="RenderFadingClump"){
        c.BuildScene(); uint8* mi=w.mi[c.next()%NMI]; const int32 alpha=PickAlpha(c);
        void* obj= nm=="RenderFadingAtomic" ? static_cast<void*>(c.RndAt()) : static_cast<void*>(c.RndCl());
        { auto P=MakePatches(G_RW); RsGuard rs; CallCdecl<void>(a,static_cast<void*>(mi),obj,alpha); }
        json al=json::array(); for(int k=0;k<NMAT;++k)al.push_back(w.mat[k][7]); out["alphas"]=al; return true; }
    if(nm=="CalculateFadingAtomicAlpha"){
        c.BuildScene(); c.SetCameraMults(); uint8* mi=w.mi[c.next()%NMI]; uint8* e=w.ent[c.next()%NENT];
        float dist; switch(c.next()%8){ case 0: dist=0.f; break; case 1: dist=c.uni(0.f,40000.f); break; case 2: dist=Ctx::QNaN(); break; case 3: dist=1e12f; break; case 4: dist=-c.uni(0.f,300.f); break; default: dist=c.uni(0.f,1500.f); break; }
        if(c.chance(4))At<float>(mi,0x18)=c.uni(100.f,1200.f);
        ret=CallCdecl<int32>(a,static_cast<void*>(mi),static_cast<void*>(e),dist); out["dist"]=F(dist); return true; }
    if(nm=="RenderEntity"){
        c.BuildScene(); c.SetCameraMults(); Gu(0xB72914)=c.pick({0u,0u,1u,2u}); uint8* e=w.ent[c.next()%NENT];
        if(c.chance(4))At<void*>(e,0x18)=nullptr;
        float dist=PickDist(c,false);
        { auto P=MakePatches(G_RW); RsGuard rs; CallCdecl<void>(a,static_cast<void*>(e),dist); }
        out["eflags"]=At<uint32>(e,0x1C); out["dist"]=F(dist); json al=json::array(); for(int k=0;k<NMAT;++k)al.push_back(w.mat[k][7]); out["alphas"]=al; return true; }

    if(nm=="RenderWeaponPedsForPC"){
        c.BuildScene(); const int cap= c.chance(8) ? 0 : 1+int(c.next()%12); c.BuildWList(cap,int(c.next()%unsigned(cap+1)));
        { auto P=MakePatches(G_RW); RsGuard rs; CallCdecl<void>(a); }
        out["w"]=c.DumpWList(); json fm=json::array(); for(int k=0;k<NWC;++k)fm.push_back(Fnv32(w.wfr[k]+0x10,0x40)); out["wfr"]=fm; return true; }

    if(nm=="FrustumSphereCB"||nm=="VehicleVisibilityCB"||nm=="VehicleVisibilityCB_BigVehicle"){
        c.BuildScene(); c.SetDists(false);
        uint8* cl=c.RndCl(); At<uint32>(At<uint8*>(cl,4),frameOff)=uint32(uintptr_t(w.mi[c.next()%NMI]));
        if(nm=="VehicleVisibilityCB"){ switch(c.next()%8){ case 0: Gf(G_LOD1)=Ctx::QNaN(); break; case 1: Gf(G_LOD1)=1e9f; break; case 2: Gf(G_LOD1)=0.f; break; default: break; } }
        { auto P=MakePatches(G_RW); ret=CallCdecl<int32>(a,static_cast<void*>(cl)); }
        return true; }
    if(nm=="IsAtomicVisible"){
        c.BuildScene(); uint8* at=c.RndAt();
        { auto P=MakePatches(G_RW); ret=CallCdecl<uint32>(a,static_cast<void*>(at))&0xFFu; }
        out["sphere"]=c.Bytes(at+0x1C,16); out["iflags"]=At<uint32>(at,0x4C); return true; }
    if(nm=="SetupVehicleVariables"){
        c.BuildScene(); c.fill(reinterpret_cast<void*>(G_DIST),8); uint8* cl=c.RndCl(); if(c.chance(6))cl[0]=1;
        { auto P=MakePatches(G_RW); CallCdecl<void>(a,static_cast<void*>(cl)); }
        out["dist"]=Gu(G_DIST); out["angle"]=Gu(G_ANGLE); return true; }
    if(nm=="SetRenderWareCamera"){
        c.BuildScene(); c.SetCameraMults(); c.fill(reinterpret_cast<void*>(0xC88028),0x28);
        Gu(G_CAMERA)=0; Gu(G_CAMPOS)=0;
        if(c.chance(3)){ Gf(0xB6F118)=c.uni(0.1f,5.f); Gf(0xB6F11C)=c.uni(0.1f,5.f); }
        { auto P=MakePatches(G_RW); CallCdecl<void>(a,static_cast<void*>(w.cam)); }
        json v=json::array(); for(uintptr_t q=0xC88028;q<=0xC88050;q+=4)v.push_back(Gu(q)==uint32(uintptr_t(w.cam))||Gu(q)==uint32(uintptr_t(w.camFr+0x40)) ? Id(reinterpret_cast<void*>(Gu(q))) : Gu(q));
        out["vars"]=v; return true; }
    if(nm=="SetClumpModelInfo"){
        c.BuildScene(); uint8* cl=c.RndCl(); uint8* mi=w.mi[c.next()%NMI];
        { auto P=MakePatches(G_RW); CallCdecl<void>(a,static_cast<void*>(cl),static_cast<void*>(mi)); }
        out["vtype"]=At<int32>(mi,0x3C); out["mt"]=tb.miType; out["fr"]=Id(reinterpret_cast<void*>(At<uint32>(At<uint8*>(cl,4),frameOff)));
        out["cb"]=Canon(At<uint32>(cl,clumpOff)); out["alpha"]=At<int32>(cl,clumpOff+4); return true; }
    return false;
}
}
