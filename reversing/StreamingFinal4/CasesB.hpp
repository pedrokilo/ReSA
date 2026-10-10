#pragma once
// Familia B: enlaces de objetos RW y recorridos del mundo (sectores, listas de entidades, pools de edificios / objetos / dummies).
#include "CasesA.hpp"

namespace StreamingFinal4Live {
inline std::array<uint32,4> lh{};                         // cabeza de la lista falsa CPtrListSingleLink<CEntity*>
inline void BuildOneList(Ctx& c,int count){
    lh[0]=0; for(int k=0;k<count&&k<nBld;++k)SglPush(&lh[0],Bld(int(c.next()%unsigned(nBld))));
}
inline std::array<float,12> pts{};                        // vectores pasados por referencia a las funciones probadas
inline const float* Pt(const CVector& v,size_t k=0){ pts[3*k]=v.x; pts[3*k+1]=v.y; pts[3*k+2]=v.z; return pts.data()+3*k; }

inline bool RunB(Ctx& c){
    const std::string& nm=c.nm;

    if(nm=="AddEntity"){
        Prep(c); const int total=2+int(c.next()%10); const int used=std::min<int>(int(c.next()%unsigned(total+1)),nBld);
        BuildLinks(c.r,total,used);
        // la entidad de la ultima vez usada (cola) puede borrarse; si la lista esta llena debe haber una borrable (si no, el EXE devuelve nulo y la DLL aborta en VERIFY)
        auto* L=reinterpret_cast<uint8*>(0x9654F0);
        for(uint8* l=At<uint8*>(L+0xC,4);l!=L;l=At<uint8*>(l,4)){ auto* e=At<uint8*>(l,0); if(c.pct(60))At<uint32>(e,0x1C)&=~0x2400u; }
        if(used==total&&used>0){ uint8* l=At<uint8*>(L+0xC,4); bool any=false; for(;l!=L;l=At<uint8*>(l,4)){ if(!(At<uint32>(At<uint8*>(l,0),0x1C)&0x2400u))any=true; }
            if(!any){ uint8* t=At<uint8*>(L+0xC,4); At<uint32>(At<uint8*>(t,0),0x1C)&=~0x2400u; } }
        auto* e=Dum(0); static const uint8 types[]{1,2,3,4,5,0,6,7}; e[0x36]=uint8((e[0x36]&0xF8)|types[c.next()%std::size(types)]);
        At<void*>(e,0x28)=nullptr;
        void* raw_{}; { auto P_=PatchSet(M_EXT); raw_=CallCdecl<void*>(c.a,static_cast<void*>(e)); }
        c.ret=Id(raw_); return true; }
    if(nm=="RemoveEntity"||nm=="RenderEntity"){
        Prep(c); const int total=2+int(c.next()%10); const int used=std::min<int>(1+int(c.next()%unsigned(total)),nBld); BuildLinks(c.r,total,used);
        // el EXE desreferencia el enlace sin comprobar nulos: siempre un enlace valido de la lista de usados (nunca nulo, centinela ni libre)
        auto* L=reinterpret_cast<uint8*>(0x9654F0); std::vector<uint8*> ul; for(uint8* l=At<uint8*>(L,8);l!=L+0xC;l=At<uint8*>(l,8))ul.push_back(l);
        uint8* arg=ul[c.next()%unsigned(ul.size())];
        RUN_VOID(0, static_cast<void*>(arg)); return true; }

    if(nm=="DeleteAllRwObjects"){ Prep(c,W_LISTS); RUN_VOID(M_EXT); return true; }
    if(nm=="DeleteRwObjectsAfterDeath"){ Prep(c,W_LISTS); const CVector v=RandPoint(c); RUN_VOID(M_EXT, Pt(v)); return true; }
    if(nm=="DeleteRwObjectsBehindCamera"){
        Prep(c,W_LISTS); At<void*>(reinterpret_cast<uint8*>(PLAYERS),0)=Ped(0); const uint32 used=CStreaming::ms_memoryUsedBytes; uint32 n;
        const uint32 k=c.next()%8; n= k==0 ? used+1+c.next()%1000 : k==1 ? 0 : k==2 ? c.next() : used-std::min<uint32>(used,c.next()%(used+1));
        RUN_VOID(M_EXT|B_REMOVELEAST, n); return true; }
    if(nm.starts_with("DeleteRwObjectsBehindCameraInSectorList")||nm.starts_with("DeleteRwObjectsNotInFrustumInSectorList")){
        Prep(c,W_LISTS); BuildOneList(c,1+int(c.next()%10));
        At<void*>(reinterpret_cast<uint8*>(PLAYERS),0)=Ped(0);
        const uint32 used=CStreaming::ms_memoryUsedBytes; const uint32 n= c.pct(30) ? used+1+c.next()%1000 : used-std::min<uint32>(used,c.next()%(used+1));
        RUN_RET(bool, M_EXT, static_cast<void*>(&lh[0]), n); return true; }
    if(nm.starts_with("DeleteRwObjectsInSectorList")){
        Prep(c,W_LISTS); BuildOneList(c,int(c.next()%10));
        const int32 sx= c.pct(40) ? -1 : int32(c.next()%120), sy=int32(c.next()%120);
        RUN_VOID(M_EXT, static_cast<void*>(&lh[0]), sx, sy); return true; }
    if(nm.starts_with("ProcessEntitiesInSectorList")){
        Prep(c,W_LISTS); BuildOneList(c,1+int(c.next()%10));
        CVector p=RandPoint(c); if(nBld>0&&c.pct(60)){ auto* e=Bld(int(c.next()%unsigned(nBld))); p=CVector{At<float>(e,4)+c.uni(-20.f,20.f),At<float>(e,8)+c.uni(-20.f,20.f),At<float>(e,0xC)}; }
        float hx=c.uni(5.f,200.f), hy=c.uni(5.f,200.f), rad=c.pct(20) ? 0.f : c.uni(5.f,250.f);
        float minX=p.x-hx, minY=p.y-hy, maxX=p.x+hx, maxY=p.y+hy;
        if(nBld>0&&c.pct(30)){ auto* e=Bld(int(c.next()%unsigned(nBld))); const uint32 k=c.next()%4; if(k==0)minX=At<float>(e,4); else if(k==1)maxX=At<float>(e,4); else if(k==2)minY=At<float>(e,8); else maxY=At<float>(e,8); }
        RUN_VOID(M_EXT, static_cast<void*>(&lh[0]), p.x, p.y, minX, minY, maxX, maxY, rad, FlagsArg(c)); return true; }

    if(nm=="AddLodsToRequestList"||nm=="AddModelsToRequestList"){
        Prep(c,W_LISTS); const CVector v=RandPoint(c); RUN_VOID(M_EXT, Pt(v), FlagsArg(c)); return true; }
    if(nm=="InstanceLoadedModels"){ Prep(c,W_LISTS); const CVector v=RandPoint(c); RUN_VOID(M_EXT, Pt(v)); return true; }
    if(nm=="RemoveBigBuildings"){ Prep(c); RUN_VOID(M_EXT); return true; }
    if(nm=="RequestBigBuildings"){ Prep(c); const CVector v=RandPoint(c); RUN_VOID(M_EXT, Pt(v)); return true; }
    if(nm=="RemoveBuildingsNotInArea"){ Prep(c); RUN_VOID(M_EXT, int32(c.pct(30) ? 0 : (c.pct(30) ? 13 : c.next()%19))); return true; }
    if(nm=="DeleteLeastUsedEntityRwObject"){
        Prep(c); const int total=4+int(c.next()%12); BuildLinks(c.r,total,std::min<int>(total,nBld));
        for(int k=0;k<nBld;++k)if(At<void*>(Bld(k),0x28)&&c.pct(60))At<uint32>(Bld(k),0x1C)&=~0x2400u;
        RUN_RET(bool, M_EXT, int32(c.next()&1), int32(c.pct(40) ? 0 : (c.pct(40) ? 0x20 : int32(c.next()&0x3F)))); return true; }
    return false;
}
}
