#pragma once
// Contexto de una ejecucion (un lado de un par): semilla, mundo falso y utilidades de salida.
#include "Stubs.hpp"

namespace StreamingFinal4Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

constexpr uint64 M_EXT = E_RNG|E_CD|E_TXD|E_ANIM|E_COL|E_IPL|E_PATH|E_SCRIPT|E_RW|E_LOADER|E_REC|E_ENT|E_POP|E_WANTED|E_ZONES|E_PLAYER|E_CAM|E_REND|E_MISC|E_MI|E_PAD;

struct Ctx {
    const Fn* fn{}; uintptr_t a{}; std::string nm; int i{}; uint32 f{}; bool candidate{};
    uint32 seed{1};
    Rng r;
    json out, ret;
    uint32 next(){ return r.next(); }
    float uni(float lo,float hi){ return r.uni(lo,hi); }
    bool chance(uint32 n){ return r.chance(n); }
    bool pct(uint32 p){ return r.pct(p); }
};

// ---------------------------------------------------------------------------------------------------------------------
// Lista de objetos RW (ms_rwObjectInstances) con enlaces falsos
// ---------------------------------------------------------------------------------------------------------------------
constexpr int NLINK=16;
alignas(16) inline std::array<std::array<uint8,12>,NLINK> linkMem{};
inline void LinkInsert(uint8* l,uint8* after){ uint8* nx=At<uint8*>(after,8); At<uint8*>(l,8)=nx; At<uint8*>(nx,4)=l; At<uint8*>(l,4)=after; At<uint8*>(after,8)=l; }
inline void LinkRemove(uint8* l){ At<uint8*>(At<uint8*>(l,8),4)=At<uint8*>(l,4); At<uint8*>(At<uint8*>(l,4),8)=At<uint8*>(l,8); }
// total enlaces en la lista, `used` ocupados por las primeras entidades de edificios
inline void BuildLinks(Rng& r,int total,int used){
    auto* L=reinterpret_cast<uint8*>(0x9654F0); std::memset(L,0,0x34);
    uint8 *uh=L, *ut=L+0xC, *fh=L+0x18, *ft=L+0x24;
    At<uint8*>(uh,8)=ut; At<uint8*>(ut,4)=uh; At<uint8*>(fh,8)=ft; At<uint8*>(ft,4)=fh;
    At<void*>(L,0x30)=linkMem.data();
    for(int i=total-1;i>=0;--i){ uint8* l=linkMem[size_t(i)].data(); std::memset(l,0,12); LinkInsert(l,fh); }
    for(int k=0;k<used&&k<nBld;++k){
        uint8* l=At<uint8*>(fh,8); if(l==ft)break; LinkRemove(l); LinkInsert(l,uh);
        At<void*>(l,0)=Bld(k); At<void*>(Bld(k),0x28)=l;
    }
    // enlace de dibujo
    std::vector<uint8*> usedLinks; for(uint8* l=At<uint8*>(uh,8);l!=ut;l=At<uint8*>(l,8))usedLinks.push_back(l);
    AtA<uintptr_t>(0x8E48A0)= (!usedLinks.empty()&&r.pct(70)) ? uintptr_t(usedLinks[r.below(unsigned(usedLinks.size()))]) : uintptr_t(uh);
}

// ---------------------------------------------------------------------------------------------------------------------
// Construye todo el mundo de una ejecucion de forma determinista a partir de la semilla del contexto
// ---------------------------------------------------------------------------------------------------------------------
enum : uint32 { W_LISTS=1 };
inline void Prep(Ctx& c,uint32 wf=0){
    rs=RunState{}; rs.seed=c.seed; rs.sq.st=Mix(c.seed,0x5EED)|1u;
    std::memset(arena.data(),0xCD,arena.size()); arenaTop=0; nrec=0; recOverflow=false;
    InitVtables();
    Rng& r=c.r;
    BuildModelInfos(r);
    BuildTxd(r);
    BuildEnv(r);
    BuildInfo(r);
    BuildPopulation(r);
    BuildEntities(r,wf);
    // dobles
    for(int k=0;k<2;++k){ const uint32 m=r.below(10); for(int j=0;j<8;++j){ rs.cdScript[k][j]= m<6 ? uint8(0) : (r.pct(50) ? uint8(0) : (r.pct(33) ? uint8(250) : (r.pct(50) ? uint8(254) : uint8(255)))); }
                          if(m>=8)rs.cdScript[k][0]=255; }
    auto* cm=reinterpret_cast<uint8*>(0xB6F99C);
    rs.playerPos=CVector{At<float>(cm,0x30)+r.uni(-30.f,30.f),At<float>(cm,0x34)+r.uni(-30.f,30.f),At<float>(cm,0x38)+r.uni(-5.f,5.f)};
    rs.groundHeight=At<float>(cm,0x38)-(r.pct(50) ? r.uni(0.f,49.f) : r.uni(48.f,120.f));
    rs.haveWanted=r.pct(90); rs.haveVehicle=r.pct(50);
    rs.carChooseN=int(r.below(7)); for(int k=0;k<rs.carChooseN;++k)rs.carChoose[k]= r.pct(25) ? -1 : VehId(r);
    rs.pedPickN=int(r.below(12)); for(int k=0;k<rs.pedPickN;++k)rs.pedPick[k]= r.pct(20) ? -1 : PedId(r);
    rs.grpPickN=int(r.below(6)); for(int k=0;k<rs.grpPickN;++k)rs.grpPick[k]=int32(r.below(18));
    At<int32>(wantedMem.data(),0x2C)=int32(r.below(7));
}
inline CVector RandPoint(Ctx& c){
    auto* cm=reinterpret_cast<uint8*>(0xB6F99C); const float cx=At<float>(cm,0x30), cy=At<float>(cm,0x34);
    const uint32 k=c.next()%6;
    if(k==0&&nBld>0){ auto* e=Bld(int(c.next()%unsigned(nBld))); return CVector{At<float>(e,4),At<float>(e,8),At<float>(e,0xC)}; }
    if(k==1)return CVector{c.uni(-2900.f,2900.f),c.uni(-2900.f,2900.f),c.uni(-10.f,100.f)};
    if(k==2)return CVector{cx,cy,At<float>(cm,0x38)};
    return CVector{cx+c.uni(-250.f,250.f),cy+c.uni(-250.f,250.f),c.uni(-10.f,100.f)};
}
}
