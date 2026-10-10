#pragma once
// Grupo G3 (parte c): ejecutores de PlaceOnRoadProperly, RcbanditCheck1CarWheels, RcbanditCheckHitWheels, HydraulicControl,
// UpdateMovingCollision, ProcessHarvester, ProcessAutoBusDoors, PlayCarHorn y ProcessControlInputs.
#include "CasesG3_b.hpp"
#include "Pools/PtrNodeDoubleLinkPool.h"

namespace AutomobileFinal4Live {
namespace g3 {

// ---------------------------------------------------------------------------------------------------------------------
// Pools falsos (nodos de lista doble, peatones, vehiculos, objetos): se sustituye el puntero de la tabla de pools del EXE
// (0xB74484) y se restaura con RestoreAll (Save de la tabla). Cabeceras CPool: {almacen, estados, capacidad, ultimoLibre, flags}.
// ---------------------------------------------------------------------------------------------------------------------
constexpr int POOL_NODE_DBL=1, POOL_PED=3, POOL_VEH=4, POOL_OBJ=6;
inline void** PoolTable(){ return reinterpret_cast<void**>(A_POOLS); }
inline void PoolHdr(uint8* h,void* storage,void* states,int32 cap){
    std::memset(h,0,0x20); At<void*>(h,0)=storage; At<void*>(h,4)=states; At<int32>(h,8)=cap; At<int32>(h,0xC)=-1; }
// Pool de nodos de lista doble propio + pool de vehiculos que cubre w.car (todas las ranuras ocupadas)
inline void NodePoolOn(){
    static uint8 vehStates[256];
    new (gw.poolHdr[0]) CPtrNodeDoubleLinkPool(NNODE,gw.nodeStore,gw.nodeState);
    std::memset(gw.nodeStore,0xFD,sizeof(gw.nodeStore)); std::memset(gw.nodeState,0x80,sizeof(gw.nodeState));
    std::memset(vehStates,0,sizeof(vehStates));
    PoolHdr(gw.poolHdr[1],w.car,vehStates,int32((sizeof(w.car)/0x38)+1));
    PoolTable()[POOL_NODE_DBL]=gw.poolHdr[0]; PoolTable()[POOL_VEH]=gw.poolHdr[1];
}
// Nodo k (0..NNODE-1) del pool propio, marcado como ocupado
inline uint8* Node(int k){ gw.nodeState[k]=0; return gw.nodeStore+12*k; }
// Construye una lista doble con los elementos dados; devuelve la cabeza (nullptr si no hay elementos). 'first' = primer nodo libre del almacen
inline void* BuildList(int& next,std::initializer_list<void*> items){
    uint8* head=nullptr; uint8* prev=nullptr;
    for(void* it:items){
        uint8* n=Node(next++); At<void*>(n,0)=it; At<void*>(n,4)=nullptr; At<void*>(n,8)=prev;
        if(prev)At<void*>(prev,4)=n; else head=n;
        prev=n;
    }
    return head;
}

// ---------------------------------------------------------------------------------------------------------------------
inline void R_PlaceOnRoad(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    for(int k=1;k<4;++k){ c.BuildBase(k,c.carModel()); At<uint32>(Cr(k),0x1C)=c.next(); }       // bits 30/31 de las banderas de la entidad
    SetVt(53,FnPtr(&S3_VGetHeightAboveRoad));
    // Si los coches 1..3 comparten modelo con el 0, InstallMi deja en la tabla el model info del ULTIMO: se dotan de colision las 4 ranuras
    for(int k=0;k<NCOLM;++k){
        uint8* cm=w.colm[k]; uint8* cd=w.cdata[k];
        At<void*>(cm,0x2C)=cd; std::memset(cd,0,0x40); At<void*>(cd,0x10)=w.lines[k]; cd[6]=4;
        std::memset(w.lines[k],0,sizeof(w.lines[k]));
        At<float>(w.lines[k][0],4)=c.uni(1.0f,3.0f);        // lines[0].start.y
        At<float>(w.lines[k][3],4)=-c.uni(1.0f,3.0f);       // lines[3].start.y
    }
    At<float>(cp,0x89C)=c.uni(0.1f,1.2f);               // m_fRearHeightAboveRoad
    At<uint32>(cp,0x594)= c.chance(8) ? 4u : 0u;       // subtipo avion
    { auto P=MakePatches(G_RNG|G_NEW,3,B_PVL); CallThis<void>(c.a,v); }
    Finish(c);
}

// Escenario comun de Rcbandit: coche 0 (el probado) y 1..3 vecinos con modelo/estado/codigo de escaneo variados
inline void RcFixture(Ctx& c,float spread){
    auto* v=c.BuildBase(0,c.carModel()); (void)v;
    for(int k=1;k<4;++k){
        c.BuildBase(k, c.chance(4) ? 441 : c.carModel());
        uint8* o=Cr(k); auto* m=reinterpret_cast<uint8*>(w.mat[k]); auto* m0=reinterpret_cast<uint8*>(w.mat[0]);
        At<float>(m,0x30)=At<float>(m0,0x30)+c.uni(-spread,spread); At<float>(m,0x34)=At<float>(m0,0x34)+c.uni(-spread,spread); At<float>(m,0x38)=At<float>(m0,0x38)+c.uni(-1.f,1.f);
        At<uint32>(o,0x590)= c.chance(10) ? 1u : 0u;                    // m_nVehicleType (0 = coche)
        { static const int st[]{2,2,3,0,5}; SetStatus(o,st[c.below(5)]); }
        for(int q=0;q<4;++q)At<float>(o,0x7E4+4*q)= c.chance(2) ? 1.0f : c.uni(0.f,1.3f);   // m_fWheelsSuspensionCompressionPrev
        At<uint16>(o,0x2C)= c.chance(3) ? AtA<uint16>(A_SCANCODE) : uint16(c.next());       // codigo de escaneo
    }
    for(int k=0;k<4;++k)At<float>(w.mi[k],0x40)=c.uni(0.4f,1.3f);                          // m_fWheelSizeFront
    for(int k=0;k<NCOLM;++k){ uint8* cm=w.colm[k]; for(int q=0;q<3;++q){ At<float>(cm,4*q)=-c.uni(0.5f,3.f); At<float>(cm,0xC+4*q)=c.uni(0.5f,3.f); } }   // las 4 ranuras (modelos repetidos)
}
constexpr uint64 M_RC=B_WHEELPOSN|B_SPHBOX|B_CLRSCAN;

inline void R_RcCheck1(Ctx& c){
    Begin(c);
    AtA<uint16>(A_SCANCODE)=uint16(1+c.below(60000));
    RcFixture(c,15.f);
    NodePoolOn();
    int next=0; const uint32 n=c.next()%5; void* items[4]{};
    for(uint32 i=0;i<n;++i)items[i]= c.chance(8) ? static_cast<void*>(Cr(0)) : static_cast<void*>(Cr(1+int(c.below(3))));
    void* head=nullptr; uint8* prev=nullptr;
    for(uint32 i=0;i<n;++i){ uint8* nd=Node(next++); At<void*>(nd,0)=items[i]; At<void*>(nd,4)=nullptr; At<void*>(nd,8)=prev; if(prev)At<void*>(prev,4)=nd; else head=nd; prev=nd; }
    std::memset(gw.list,0,sizeof(gw.list)); At<void*>(gw.list,0)=head;
    bool r; uint8 sm[0x50]; std::memcpy(sm,reinterpret_cast<void*>(A_STATICMAT),sizeof(sm));
    { auto P=MakePatches(G_RNG|G_NEW,3,M_RC); r=CallThis<bool>(c.a,Cr(0),static_cast<void*>(gw.list)); }
    std::memcpy(reinterpret_cast<void*>(A_STATICMAT),sm,sizeof(sm));   // la estatica del EXE (0xC1C220) solo la escribe el lado EXE
    c.ret=r;
    Finish(c);
}

inline void R_RcHitWheels(Ctx& c){
    Begin(c);
    const uint32 sc=c.next()%12; AtA<uint16>(A_SCANCODE)= sc==0 ? uint16(0xFFFF) : uint16(1+c.below(60000));
    RcFixture(c,6.f);
    NodePoolOn();
    std::memset(reinterpret_cast<void*>(A_REPEAT),0,16*16*12);
    // posicion del coche: la mitad de las veces cerca de un limite de sector (cada 50 unidades)
    float x=c.uni(-300.f,300.f), y=c.uni(-300.f,300.f);
    if(c.chance(2)){ x=50.f*std::floor(x/50.f)+c.uni(-2.6f,2.6f); }
    if(c.chance(2)){ y=50.f*std::floor(y/50.f)+c.uni(-2.6f,2.6f); }
    auto* m0=reinterpret_cast<uint8*>(w.mat[0]); At<float>(m0,0x30)=x; At<float>(m0,0x34)=y;
    for(int q=0;q<3;++q)At<float>(Cr(0),4+4*q)=At<float>(m0,0x30+4*q);
    for(int k=1;k<4;++k){ auto* m=reinterpret_cast<uint8*>(w.mat[k]); At<float>(m,0x30)=x+c.uni(-6.f,6.f); At<float>(m,0x34)=y+c.uni(-6.f,6.f); }
    const int sx=int(std::floor(x*0.02f+60.f)), sy=int(std::floor(y*0.02f+60.f));
    int next=0;
    for(int dy=-1;dy<=1;++dy)for(int dx=-1;dx<=1;++dx){
        if(next>NNODE-3||c.chance(3))continue;
        const int idx=(((sy+dy)&15)<<4)+((sx+dx)&15);
        void* head;
        const uint32 t=c.next()%3;
        if(t==0)head=BuildList(next,{static_cast<void*>(Cr(1))});
        else if(t==1)head=BuildList(next,{static_cast<void*>(Cr(2)),static_cast<void*>(Cr(3))});
        else head=BuildList(next,{static_cast<void*>(Cr(3)),static_cast<void*>(Cr(1)),static_cast<void*>(Cr(2))});
        At<void*>(reinterpret_cast<void*>(A_REPEAT+size_t(idx)*12),0)=head;
    }
    bool r; uint8 sm[0x50]; std::memcpy(sm,reinterpret_cast<void*>(A_STATICMAT),sizeof(sm));
    { auto P=MakePatches(G_RNG|G_NEW,3,M_RC); r=CallThis<bool>(c.a,Cr(0)); }
    std::memcpy(reinterpret_cast<void*>(A_STATICMAT),sm,sizeof(sm));
    c.ret=r;
    c.out["scan"]=int(AtA<uint16>(A_SCANCODE));
    Finish(c);
}

// ---------------------------------------------------------------------------------------------------------------------
inline void R_Hydraulic(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    const uint32 sm=c.next()%8;                         // 0..4: PLAYER, 5: PHYSICS de mision, 6: otros (salida temprana), 7: bHydraulicNone
    const int idx=int(c.below(4));
    SetStatus(cp, sm<5 ? 0 : sm==5 ? 3 : (c.chance(2) ? 2 : 4));
    cp[0x4A4]= (sm==5 && !c.chance(10)) ? 2 : uint8(c.below(4));        // m_nCreatedBy
    cp[0x48B]= (sm==5 && c.chance(10)) ? uint8(0xFF) : uint8(idx);      // m_vehicleSpecialColIndex (signed)
    SetBit(cp,0x38C,16,c.chance(2)); SetBit(cp,0x38C,18,sm==7);          // bHydraulicGeom, bHydraulicNone
    At<void*>(cp,0x460)=MakePed(0,c.chance(8) ? 4 : int(c.below(2)));
    if(c.chance(10))AtA<uint8>(A_GAMESTATE)=1;
    // handling y modelo
    At<float>(w.hd[0],0xB8)=c.uni(0.1f,0.5f); At<float>(w.hd[0],0xBC)=-c.uni(0.05f,0.5f);
    At<float>(w.mi[0],0x40)=c.uni(0.4f,1.3f);
    // datos de hidraulica del hueco idx
    uint8* hd=reinterpret_cast<uint8*>(A_HYDRAULIC+size_t(idx)*0x28);
    if(c.chance(3)){ At<float>(hd,0)=0.f; At<float>(hd,4)=0.f; At<float>(hd,8)=c.chance(2) ? 100.f : c.uni(100.f,500.f); At<float>(hd,0xC)=0.f; At<float>(hd,0x10)=0.f; At<float>(hd,0x14)=0.f; }
    else {
        const float nu=c.uni(0.05f,0.4f), nl=-c.uni(0.05f,0.4f), eu=nu-c.uni(0.1f,0.5f), el=nl-c.uni(0.1f,0.5f);
        At<float>(hd,0)=nu; At<float>(hd,4)=nl; At<float>(hd,8)=eu; At<float>(hd,0xC)=el;
        At<float>(hd,0x10)=nu-c.uni(0.f,0.1f); At<float>(hd,0x14)=nl+c.uni(0.f,0.1f);
        if(c.chance(10)){ At<float>(hd,0xC)=nl; }          // limitDiff == 0
    }
    for(int q=0;q<4;++q)At<float>(hd,0x18+4*q)= c.chance(3) ? 0.f : (c.chance(3) ? c.uni(0.f,2.f) : 1.f);   // m_aWheelSuspension
    // modelo de colision especial del hueco: esferas y lineas
    uint8* sp=reinterpret_cast<uint8*>(A_SPECIALCOL+size_t(idx)*0x30);
    uint8* cd=gw.cmDataB; std::memset(cd,0,0x40);
    const uint32 ns=c.next()%5; At<uint16>(cd,0)=uint16(ns); At<void*>(cd,8)=gw.spB; At<void*>(cd,0x10)=gw.lnB;
    for(uint32 q=0;q<ns;++q){ for(int a=0;a<3;++a)At<float>(gw.spB[q],4*a)=c.uni(-1.5f,1.5f); At<float>(gw.spB[q],0xC)=c.uni(0.1f,0.8f); }
    for(int q=0;q<4;++q){ for(int a=0;a<3;++a){ At<float>(gw.lnB[q],4*a)=c.uni(-1.f,1.f); At<float>(gw.lnB[q],0x10+4*a)=c.uni(-1.f,1.f); } }
    At<void*>(sp,0x2C)=cd;
    // suspension del coche
    for(int q=0;q<4;++q){
        At<float>(cp,0x7D4+4*q)= c.chance(3) ? 1.0f : c.uni(0.f,1.f);
        At<float>(cp,0x7E4+4*q)=c.uni(0.f,1.f);
        At<float>(cp,0x878+4*q)=c.uni(0.1f,0.6f); At<float>(cp,0x888+4*q)=c.uni(0.4f,1.2f);
    }
    static const uint16 angs[]{0,1,2,19,20,21,59,60,61,100,499,500,501,503,504,505,2000};
    At<uint16>(cp,0x86C)= c.chance(5) ? uint16(c.next()%700) : angs[c.below(uint32(std::size(angs)))];
    At<float>(cp,0x8B0)= c.chance(3) ? c.uni(0.f,0.03f) : c.uni(0.f,0.5f);
    At<float>(cp,0x49C)= c.chance(4) ? 0.f : c.uni(-1.f,1.f);
    { auto P=MakePatches(G_RNG|G_NEW,3,B_AUDEV|B_SPECIAL|B_WHEELPOSN); CallThis<void>(c.a,v); }
    c.out["idx"]=idx;
    Finish(c);
}

// ---------------------------------------------------------------------------------------------------------------------
inline void R_UpdateMovingCollision(Ctx& c){
    Begin(c);
    static const int models[]{443,486,406,524,592,530,544,443,400,406};
    const uint32 mode=c.i%10;
    auto* v=c.BuildBase(0,models[mode]); uint8* cp=Cr(0);
    const bool player= c.next()%3!=0;
    SetStatus(cp, player ? 0 : 3);
    cp[0x4A4]= c.chance(8) ? uint8(c.below(4)) : uint8(2);                      // m_nCreatedBy
    At<uint32>(cp,0x594)= (mode==2) ? 1u : 0u;                                  // subtipo (el volquete exige camion monstruo)
    if(mode==9)At<uint32>(cp,0x594)=0;
    if(player)At<void*>(cp,0x460)= c.chance(10) ? nullptr : static_cast<void*>(MakePed(0,c.chance(10) ? 4 : int(c.below(2))));
    if(c.chance(12))AtA<uint8>(A_GAMESTATE)=1;
    const int idx=int(c.below(4)); cp[0x48B]=uint8(idx);
    for(int n:{0x698,0x6A0,0x6A8})At<void*>(cp,size_t(n))= c.Frame(6+(n-0x698)/8,c.uni(-1.5f,1.5f),c.uni(-3.f,3.f),c.uni(0.f,2.f));
    At<uint16>(cp,0x86C)=uint16(c.below(2501)); At<uint16>(cp,0x86E)=uint16(c.next());
    // datos de colision: modelo normal (cmDataA) y especial (cmDataB), con el mismo numero de esferas/triangulos
    uint8* cm=w.colm[0]; At<void*>(cm,0x2C)=gw.cmDataA;
    std::memset(gw.cmDataA,0,0x40); std::memset(gw.cmDataB,0,0x40);
    const uint32 ns=c.next()%5, nt=c.next()%7, nv=8;
    for(uint8* d:{gw.cmDataA,gw.cmDataB}){ At<uint16>(d,0)=uint16(ns); At<uint16>(d,4)=uint16(nt); }
    At<void*>(gw.cmDataA,8)=gw.spA; At<void*>(gw.cmDataA,0x14)=gw.vtA; At<void*>(gw.cmDataA,0x18)=gw.trA;
    At<void*>(gw.cmDataB,8)=gw.spB; At<void*>(gw.cmDataB,0x14)=gw.vtB; At<void*>(gw.cmDataB,0x18)=gw.trB;
    At<void*>(gw.cmDataB,0x1C)= c.chance(4) ? nullptr : static_cast<void*>(gw.plB);
    for(uint32 q=0;q<nv;++q)for(int a=0;a<3;++a){ At<int16>(gw.vtA[q],2*a)=int16(int(c.below(801))-400); At<int16>(gw.vtB[q],2*a)=int16(int(c.below(801))-400); }
    for(uint32 q=0;q<nt;++q){
        for(int a=0;a<3;++a){ At<uint16>(gw.trA[q],2*a)=uint16(c.below(nv)); At<uint16>(gw.trB[q],2*a)=uint16(c.below(nv)); }
        gw.trA[q][6]=uint8(c.below(80)); gw.trB[q][6]= c.chance(2) ? uint8(0x41) : uint8(c.below(80));
    }
    for(uint32 q=0;q<ns;++q){
        for(int a=0;a<3;++a){ At<float>(gw.spA[q],4*a)=c.uni(-2.f,2.f); At<float>(gw.spB[q],4*a)=c.uni(-2.f,2.f); }
        At<float>(gw.spA[q],0xC)=c.uni(0.1f,1.f); At<float>(gw.spB[q],0xC)=c.uni(0.1f,1.f);
        gw.spA[q][0x10]=uint8(c.below(80)); gw.spB[q][0x10]= c.chance(2) ? uint8(0x41) : uint8(c.below(80));
    }
    if(mode==8||mode==9){ for(uint32 q=0;q<nt;++q)if(gw.trB[q][6]==0x41)gw.trB[q][6]=0x42; for(uint32 q=0;q<ns;++q)if(gw.spB[q][0x10]==0x41)gw.spB[q][0x10]=0x42; }   // el EXE usa rotMatrix sin inicializar
    uint8* sp=reinterpret_cast<uint8*>(A_SPECIALCOL+size_t(idx)*0x30); At<void*>(sp,0x2C)=gw.cmDataB;
    for(int q=0;q<3;++q){ At<float>(sp,4*q)=-c.uni(0.3f,2.f); At<float>(sp,0xC+4*q)=c.uni(0.3f,2.f); At<float>(cm,4*q)=-c.uni(0.3f,3.f); At<float>(cm,0xC+4*q)=c.uni(0.3f,3.f); }
    const float angle= c.chance(2) ? c.uni(0.f,1.f) : -1.f;
    bool r;
    { auto P=MakePatches(G_RNG|G_NEW,3,B_SPECIAL); r=CallThis<bool>(c.a,v,angle); }
    c.ret=r;
    c.out["angle"]=int(At<uint16>(cp,0x86C));
    Finish(c);
}

// ---------------------------------------------------------------------------------------------------------------------
inline void R_Harvester(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,532); uint8* cp=Cr(0);
    SetStatus(cp, c.chance(8) ? 3 : 0);
    static const float mags[]{0.f,0.005f,0.0099f,0.01f,0.0101f,0.3f};
    const float mg= c.chance(2) ? mags[c.below(uint32(std::size(mags)))] : c.uni(0.f,0.6f);
    const float ang=c.uni(0.f,6.28f); At<float>(cp,0x44)=mg*std::cos(ang); At<float>(cp,0x48)=mg*std::sin(ang); At<float>(cp,0x4C)=c.uni(-0.05f,0.05f);
    AtA<uint32>(A_FRAMECTR)= (AtA<uint32>(A_FRAMECTR)&~1u)|(c.chance(2) ? 1u : 0u);
    cp[0x980]=uint8(c.below(9));
    AtA<uint8>(A_NASTY)=0;                                    // ShootLimbs() = false: el constructor de CObject de la DLL no se puede parchear
    AtA<uint16>(A_MODELIDX+0x10)=3000; AtA<uint16>(A_MODELIDX+0x14)=3001;      // MI_GRASSHOUSE / MI_GRASSPLANT
    AtA<uint8>(A_HARVSTREAM)=0;
    // pools de peatones y objetos falsos
    PoolHdr(gw.poolHdr[3],gw.poolPed,gw.poolPedState,int32(NPOOLPED)); PoolHdr(gw.poolHdr[6],gw.poolObj,gw.poolObjState,int32(NPOOLOBJ));
    PoolTable()[POOL_PED]=gw.poolHdr[3]; PoolTable()[POOL_OBJ]=gw.poolHdr[6];
    auto* m0=reinterpret_cast<uint8*>(w.mat[0]);
    auto RelPos=[&](uint8* mat){
        const float fd= c.chance(2) ? c.uni(3.5f,5.5f) : c.uni(-8.f,8.f), rd= c.chance(2) ? c.uni(-4.5f,4.5f) : c.uni(-9.f,9.f), ud= c.chance(2) ? c.uni(-4.5f,4.5f) : c.uni(-9.f,9.f);
        for(int q=0;q<3;++q)At<float>(mat,0x30+4*q)=At<float>(m0,0x30+4*q)+fd*At<float>(m0,0x10+4*q)+rd*At<float>(m0,4*q)+ud*At<float>(m0,0x20+4*q);
    };
    for(size_t i=0;i<NPOOLPED;++i){
        std::memset(gw.poolPed[i],0,0x7C4); uint8* m=gw.pmat[i]; std::memset(m,0,0x60); const double id[9]{1,0,0,0,1,0,0,0,1}; Ctx::SetMatrix(m,id,0.f,0.f,0.f); RelPos(m);
        At<void*>(gw.poolPed[i],0)=gw.pedVt; At<void*>(gw.poolPed[i],0x14)=m; At<uint32>(gw.poolPed[i],0x598)= c.chance(4) ? 0u : 4u;
        gw.poolPedState[i]= c.chance(5) ? uint8(0x80) : uint8(0);
    }
    for(size_t i=0;i<NPOOLOBJ;++i){
        std::memset(gw.poolObj[i],0,OBJ_SIZE); uint8* m=gw.pmat[NPOOLPED+i]; std::memset(m,0,0x60); const double id[9]{1,0,0,0,1,0,0,0,1}; Ctx::SetMatrix(m,id,0.f,0.f,0.f); RelPos(m);
        At<void*>(gw.poolObj[i],0)=gw.objVt; At<void*>(gw.poolObj[i],0x14)=m;
        At<uint16>(gw.poolObj[i],0x22)= c.chance(4) ? uint16(5) : uint16(3000+c.below(2));
        At<uint32>(gw.poolObj[i],0x1C)= (c.chance(4) ? 0u : 0x80u) | (c.chance(5) ? 0x200u : 0u);       // bIsVisible (bit 7), bRenderDamaged (bit 9)
        gw.poolObjState[i]= c.chance(5) ? uint8(0x80) : uint8(0);
    }
    { auto P=MakePatches(G_RNG|G_NEW,3,B_AUDEV|B_OBJDMG|B_FXADD); CallThis<void>(c.a,v); }
    Finish(c);
}

// ---------------------------------------------------------------------------------------------------------------------
inline void R_BusDoors(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    SetVt(38,FnPtr(&S3_VIsDoorMissing)); SetVt(36,FnPtr(&S3_VIsDoorClosed)); SetVt(27,FnPtr(&S3_VOpenDoor));
    const uint32 now=AtA<uint32>(A_TIMEMS);
    const uint32 ends[]{0u,now-5u,now,now+10u,now+499u,now+500u,now+501u,now+2000u,100u,now+1u};
    At<uint32>(cp,0x870)= c.chance(5) ? c.next() : ends[c.below(uint32(std::size(ends)))];
    At<uint32>(cp,0x874)= c.chance(3) ? 0u : (c.chance(2) ? 1000u : c.next());
    cp[0x486]=uint8(c.below(8));                              // m_nGettingInFlags
    { auto P=MakePatches(G_RNG|G_NEW,3,0); CallThis<void>(c.a,v); }
    Finish(c);
}

inline void R_PlayCarHorn(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    static const uint16 alarms[]{0,0xFFFF,1,2,300};
    At<uint16>(cp,0x45C)= c.chance(2) ? alarms[c.below(uint32(std::size(alarms)))] : uint16(c.next());
    SetStatus(cp, c.chance(3) ? 5 : int(c.below(5)));
    At<uint32>(cp,0x514)= c.chance(4) ? 1u + c.below(40) : 0u;                  // m_HornCounter
    static const uint8 timers[]{0,0,1,2,5,200,255,128};
    cp[0x519]= c.chance(2) ? timers[c.below(uint32(std::size(timers)))] : uint8(c.next());
    if(c.chance(4)==false)At<void*>(cp,0x460)=MakePed(0,4);
    SetBit(cp,0x3DB,0,c.chance(2));                           // m_autoPilot.carCtrlFlags.bHonkAtCar
    { auto P=MakePatches(G_RNG|G_NEW,3,B_SAY); CallThis<void>(c.a,v); }
    Finish(c);
}

// ---------------------------------------------------------------------------------------------------------------------
inline void R_ControlInputs(Ctx& c){
    Begin(c);
    const uint32 mm=c.next()%16;
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    if(mm==0){ cp[0x22]=0xFE; cp[0x23]=0xFF; }                                  // modelo -2 (UNLOAD_MODEL)
    const uint8 pn=uint8(c.below(2));
    CPad::NewMouseControllerState.m_AmountMoved.x= c.chance(2) ? 0.f : c.uni(-30.f,30.f);
    AtA<uint8>(A_USEMOUSE3RD)=uint8(c.chance(2)); AtA<uint8>(A_CTRLFLAGS+2)=uint8(c.chance(2));
    { static const uint32 ctl[]{0,2,2,1,3}; AtA<uint32>(A_CTRLFLAGS+4)=ctl[c.below(5)]; }
    At<float>(cp,0x58C)= c.chance(4) ? 0.f : c.uni(-1.2f,1.2f);                // m_fRawSteerAngle
    At<float>(cp,0x498)=c.uni(-0.5f,0.5f); At<float>(cp,0x49C)=c.uni(-1.f,1.f); At<float>(cp,0x4A0)=c.uni(0.f,1.f);
    static const float sp[]{0.f,0.005f,0.02f,-0.02f,0.1f,-0.3f,0.5f};
    const float spd= c.chance(2) ? sp[c.below(uint32(std::size(sp)))] : c.uni(-0.8f,0.8f);
    auto* m=reinterpret_cast<uint8*>(w.mat[0]);
    for(int q=0;q<3;++q)At<float>(cp,0x44+4*q)=spd*At<float>(m,0x10+4*q)+(q==2 ? 0.f : c.uni(-0.02f,0.02f));
    SetBit(cp,0x428,4,c.chance(2)); SetBit(cp,0x428,30,c.chance(6)); SetBit(cp,0x428,5,c.chance(4));    // bEngineOn, bIsDrowning, bIsHandbrakeOn
    SetBit(cp,0x428,12,c.chance(5)); SetBit(cp,0x428,41,c.chance(6));                                   // bComedyControls, bEngineBroken
    cp[0x51A]=uint8(c.below(3));                                                                         // m_comedyControlState
    SetBit(w.hd[0],0xD0,6,c.chance(3));                                                                  // m_bHbRearwheelSteer
    const uint32 dm=c.next()%8;
    if(dm!=0){ uint8* d=MakePed(0, dm==1 ? 4 : int(c.below(2)), c.chance(8) ? 63u : 0u); At<void*>(cp,0x460)=d;
        if(c.chance(4))At<void*>(gw.intel[0],4)=gw.task[0]; }                                            // tarea primaria 0 = tiroteo desde coche
    player_ped=MakePed(1,0);
    for(int k=0;k<2;++k){ PadRand(c,k); CPad::GetPad(k)->Mode=int16(c.below(4)); }
    if(c.chance(6)){ auto* p0=reinterpret_cast<uint8*>(CPad::GetPad(0)); At<uint16>(p0,0x10E)=uint16(1+c.below(2)); }
    if(c.chance(8)){ auto* p0=reinterpret_cast<uint8*>(CPad::GetPad(0)); At<uint8>(p0,0x118)=1; }
    { auto P=MakePatches(G_RNG|G_NEW,3,B_FINDTASK|B_ALLOWED|B_FPP_T|B_KEEPCLR); CallThis<void>(c.a,v,pn); }
    c.out["pn"]=int(pn);
    Finish(c);
}

} // namespace g3
} // namespace AutomobileFinal4Live
