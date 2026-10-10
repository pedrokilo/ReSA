#pragma once
// Grupo G3 (parte b): entorno comun y ejecutores de BoostJumpControl, NitrousControl, StopNitroEffect, TowTruckControl, TankControl,
// FireTruckControl, BlowUpCarsInPath, BlowUpCar, BlowUpCarCutSceneNoExtras, ProcessCarOnFireAndExplode y ScanForCrimes.
#include "CasesG3_a.hpp"

namespace AutomobileFinal4Live {
namespace g3 {

constexpr uint32 TASK_GANG_DRIVEBY=1022;       // 0x3FE
constexpr int32  MODE_ON_A_STRING=18, MODE_AIM_FROM_CAR=55;

inline void SetBit(uint8* base,size_t off,int bit,bool on){ uint8& b=base[off+size_t(bit>>3)]; const uint8 m=uint8(1u<<(bit&7)); b= on ? uint8(b|m) : uint8(b&~m); }
inline bool GetBit(const uint8* base,size_t off,int bit){ return (base[off+size_t(bit>>3)]>>(bit&7))&1; }
inline void SetStatus(uint8* car,int st){ car[0x36]=uint8((car[0x36]&7)|(st<<3)); }
inline void SetCamMode(CCam& cam,int mode){ *reinterpret_cast<int16*>(&cam.m_nMode)=int16(mode); }

// Rellena el estado de un mando (NewState + OldState) con valores del dominio real
inline void PadRand(Ctx& c,int idx){
    auto* pad=reinterpret_cast<uint8*>(CPad::GetPad(idx));
    auto S=[&](size_t base){
        for(size_t o:{size_t(0),size_t(2),size_t(4),size_t(6)})At<int16>(pad,base+o)= c.chance(3) ? int16(0) : int16(int(c.below(256))-128);
        for(size_t o=8;o<=0x26;o+=2)At<int16>(pad,base+o)= c.chance(2) ? int16(0) : int16(c.chance(3) ? 255 : int(c.below(256)));
        for(size_t o:{size_t(0x28),size_t(0x2A),size_t(0x2C),size_t(0x2E)})At<int16>(pad,base+o)=int16(c.chance(3) ? 1 : 0);
    };
    S(PAD_NEW); S(PAD_OLD);
    At<uint16>(pad,0x10E)=0; At<uint8>(pad,0x118)=0;     // DisablePlayerControls, bApplyBrakes
}

// Peaton falso. tipo: 0 PLAYER1, 1 PLAYER2, otro = no jugador
inline uint8* MakePed(int k,int type,uint32 pedState=0){
    auto* p=gw.ped[k]; std::memset(p,0,PED_SIZE); std::memset(gw.intel[k],0,INTEL_SIZE);
    At<void*>(p,0)=gw.pedVt;
    At<uint32>(p,0x598)=uint32(type);
    At<void*>(p,0x47C)=gw.intel[k];
    At<void*>(gw.intel[k],4+0x2C)=p;            // CTaskManager::m_pPed
    At<uint32>(p,0x530)=pedState;
    return p;
}

// Entorno global comun (identico en los dos lados: todo sale de c.next())
inline void Begin(Ctx& c){
    std::memset(&gw,0,sizeof(gw));
    for(auto& p:gw.pedVt)p=FnPtr(&S3_Trap);
    gw.pedVt[21]=FnPtr(&S3_PedFlagDestroy);
    for(auto& p:gw.objVt)p=FnPtr(&S3_Trap);
    for(auto& p:gw.taskVt)p=FnPtr(&S3_Trap);
    gw.taskVt[2]=FnPtr(&S3_TaskGetSubTask); gw.taskVt[3]=FnPtr(&S3_TaskIsSimple); gw.taskVt[4]=FnPtr(&S3_TaskGetType);
    for(auto& t:gw.task){ At<void*>(t,0)=gw.taskVt; At<uint32>(t,4)=TASK_GANG_DRIVEBY; }
    OrcFill(c);
    player_vehicle=nullptr; player_ped=nullptr; lastBar=V3{0,0,0};
    static const float steps[]{1.f,1.f,0.5f,2.f,3.f,0.25f,1.6666666f};
    AtA<float>(A_TIMESTEP)= c.chance(3) ? c.uni(0.05f,3.f) : steps[c.below(uint32(std::size(steps)))];
    AtA<uint32>(A_TIMEMS)=c.next()>>1;
    AtA<uint32>(A_FRAMECTR)=c.next()>>3;
    AtA<uint8>(A_PLAYERFOCUS)=0;
    std::memset(reinterpret_cast<void*>(A_PLAYERS),0,PLAYER_SIZE*2);
    AtA<uint8>(A_GAMESTATE)=0; AtA<uint32>(A_SKIPSTATE)=c.below(3);
    AtA<uint8>(A_NEWSHELI)=uint8(c.next()); AtA<uint32>(A_AMBULANCES)=c.below(50); AtA<uint32>(A_AMBULANCES+4)=c.below(50);
    AtA<uint16>(A_TEMPOBJS)=uint16(c.below(100)); AtA<uint32>(A_IGNOREENT)=0;
    AtA<uint8>(A_USEMOUSE3RD)=0; AtA<uint8>(A_CTRLFLAGS+2)=0; AtA<uint32>(A_CTRLFLAGS+4)=0;
    AtA<float>(A_SCRATCH)=0.f;
    PadRand(c,0); PadRand(c,1);
    // camara: tres camaras, una activa
    TheCamera.m_nActiveCam=uint8(c.below(3));
    for(auto& cam:TheCamera.m_aCams){
        SetCamMode(cam, c.chance(2) ? MODE_ON_A_STRING : int(c.below(60)));
        const float fx=c.uni(-1.f,1.f), fy=c.uni(-1.f,1.f), fz=c.uni(-1.f,1.f); const float m=std::sqrt(fx*fx+fy*fy+fz*fz)+0.01f;
        cam.m_vecFront=CVector{fx/m,fy/m,fz/m};
    }
}

inline void Finish(Ctx& c){ c.Hash("g3",&gw,sizeof(gw)); }

// ---------------------------------------------------------------------------------------------------------------------
inline void R_BoostJump(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    const uint32 mode=c.i%4;                  // 0 jugador 1, 1 jugador 2, 2 sin conductor, 3 conductor no jugador
    if(mode!=2)At<void*>(cp,0x460)=MakePed(0, mode==0 ? 0 : mode==1 ? 1 : 4+int(c.below(5)));
    At<float>(cp,0x7D4)= c.chance(3) ? 1.0f : c.uni(0.f,1.3f);
    { auto P=MakePatches(G_RNG|G_NEW,3,B_AMF|B_ATF); CallThis<void>(c.a,v); }
    Finish(c);
}

inline void R_Nitrous(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    SetBit(cp,0x38C,19,c.chance(2));                      // handlingFlags.bNosInst
    static const int sts[]{0,0,0,3,2,4,5};
    SetStatus(cp,sts[c.below(uint32(std::size(sts)))]);
    At<void*>(cp,0x460)=MakePed(0,c.chance(5) ? 4 : int(c.below(2)));       // el EXE desreferencia siempre m_pDriver con estado PLAYER
    static const float temps[]{1.0f,1.0f,0.f,0.5f,-0.000001f,0.9999f,-0.5f,-1.0f};
    At<float>(cp,0x8A4)= c.chance(2) ? temps[c.below(uint32(std::size(temps)))] : c.uni(-1.3f,1.2f);
    static const uint8 boosts[]{0,1,5,100,101,127,128,200,255};
    At<uint8>(cp,0x48A)= c.chance(3) ? uint8(c.next()) : boosts[c.below(uint32(std::size(boosts)))];
    At<float>(cp,0x49C)= c.chance(4) ? 0.f : c.uni(-1.f,1.f);
    At<void*>(cp,0x978)= c.chance(2) ? static_cast<void*>(gw.fx[0]) : nullptr;
    At<void*>(cp,0x97C)= c.chance(2) ? static_cast<void*>(gw.fx[1]) : nullptr;
    const uint32 bm=c.next()%6; int8 boost=0;
    if(bm==0)boost=int8(1+int(c.below(127))); else if(bm==1)boost=int8(-1-int(c.below(128))); else if(bm==2)boost=int8(c.chance(2) ? 1 : -1);
    { auto P=MakePatches(G_RNG|G_NEW,3,B_NITRO|B_FXKILL|B_REMUPG); CallThis<void>(c.a,v,boost); }
    c.out["boost"]=int(boost);
    Finish(c);
}

inline void R_StopNitro(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    At<void*>(cp,0x978)= c.chance(2) ? static_cast<void*>(gw.fx[0]) : nullptr;
    At<void*>(cp,0x97C)= c.chance(2) ? static_cast<void*>(gw.fx[1]) : nullptr;
    { auto P=MakePatches(G_RNG|G_NEW,3,B_FXKILL); CallThis<void>(c.a,v); }
    Finish(c);
}

inline void R_TowTruck(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    for(int k=1;k<4;++k){ c.BuildBase(k,c.carModel()); SetBit(Cr(k),0x428,3,c.chance(5)); }   // bIsLocked
    SetVt(60,FnPtr(&S3_VGetTowBarPos)); SetVt(59,FnPtr(&S3_VGetTowHitchPos)); SetVt(61,FnPtr(&S3_VSetTowLink));
    SetStatus(cp, c.chance(8) ? 3 : 0);
    const uint32 dm=c.next()%8;
    if(dm!=0)At<void*>(cp,0x460)=MakePed(0, dm==1 ? 6 : int(c.below(2)));
    static const uint16 angles[]{0,20000,20000,20000,19950,10000,15000,100,60,19999};
    At<uint16>(cp,0x86C)= c.chance(4) ? uint16(c.next()%20001) : angles[c.below(uint32(std::size(angles)))];
    At<void*>(cp,0x4C8)= c.chance(6) ? static_cast<void*>(Cr(1)) : nullptr;
    { auto P=MakePatches(G_RNG|G_NEW,3,B_FOIR); CallThis<void>(c.a,v); }
    Finish(c);
}

inline void R_Tank(Ctx& c){
    Begin(c);
    const uint32 mode=c.i%6;                    // 0,1 RC Tiger; 2,3,4 Rhino; 5 otro modelo
    const int model= mode<2 ? 564 : mode<5 ? 432 : c.carModel();
    auto* v=c.BuildBase(0,model); uint8* cp=Cr(0);
    int st;
    if(mode==0)st=8; else if(mode==1)st=int(c.below(8)); else if(mode<5)st= c.chance(8) ? 3 : 0; else st=0;
    if(mode==1&&st==8)st=3;
    SetStatus(cp,st);
    At<uint32>(cp,0x4D4)= c.chance(3) ? AtA<uint32>(A_TIMEMS)-c.below(200) : AtA<uint32>(A_TIMEMS)-60-c.below(500);
    At<void*>(cp,0x6A0)=c.Frame(5,c.uni(-2.f,2.f),c.uni(-2.f,2.f),c.uni(0.f,3.f));       // m_aCarNodes[CAR_MISC_C]
    // el EXE deja newTurretPosition sin inicializar si el marco es nulo: el banco no recorre esa rama
    At<float>(cp,0x94C)= c.chance(5) ? c.uni(-1.f,1.f) : c.uni(-3.4f,3.4f);
    At<float>(cp,0x950)= c.chance(4) ? c.uni(-0.3f,0.8f) : c.uni(-0.2f,0.62f);
    At<float>(cp,0x44)=c.uni(-0.6f,0.6f); At<float>(cp,0x48)=c.uni(-0.6f,0.6f); At<float>(cp,0x4C)=c.uni(-0.2f,0.2f);
    if(!c.chance(10))At<void*>(cp,0x460)=MakePed(0, c.chance(8) ? 4 : int(c.below(2)));
    AtA<uint8>(A_GAMESTATE)= c.chance(10) ? 1 : 0;
    // jugador: la ultima vez que disparo el canon grande
    AtA<uint32>(A_PLAYERS+0xEC)= c.chance(3) ? AtA<uint32>(A_TIMEMS)-c.below(900) : AtA<uint32>(A_TIMEMS)-800-c.below(2000);
    player_ped=MakePed(1,0);
    { auto P=MakePatches(G_RNG|G_NEW,3,B_FIREHIT|B_GUNSHELL|B_WEAPEV|B_LTM|B_AF|B_TANKAIM|B_PLOS|B_ADDEXPL|B_TANKFIRE|B_FPP_T); CallThis<void>(c.a,v); }
    Finish(c);
}

inline void R_FireTruck(Ctx& c){
    Begin(c);
    const uint32 mm=c.i%4;
    const int model= (mm==0||mm==2) ? 601 : mm==1 ? 407 : c.carModel();
    auto* v=c.BuildBase(0,model); uint8* cp=Cr(0);
    c.BuildBase(1,c.carModel());
    const bool isPlayer= ((c.i/4)%2)==0;
    player_vehicle= isPlayer ? static_cast<void*>(cp) : static_cast<void*>(Cr(1));
    SetStatus(cp, isPlayer ? 0 : (c.chance(6) ? 4 : 3));
    At<float>(cp,0x94C)= c.chance(5) ? c.uni(-1.f,1.f) : c.uni(-3.4f,3.4f);
    At<float>(cp,0x950)= c.uni(-0.5f,0.7f);
    At<void*>(cp,0x698)= c.chance(3) ? nullptr : c.Frame(5,c.uni(-2.f,2.f),c.uni(-2.f,2.f),c.uni(0.f,3.f));    // m_aCarNodes[CAR_MISC_A]
    At<float>(cp,0x44)=c.uni(-0.5f,0.5f); At<float>(cp,0x48)=c.uni(-0.5f,0.5f); At<float>(cp,0x4C)=c.uni(-0.2f,0.2f);   // m_vecMoveSpeed
    // fuego (la posicion esta en +4)
    std::memset(gw.fire,0,sizeof(gw.fire));
    auto* carPos=reinterpret_cast<uint8*>(w.mat[0])+0x30;
    At<float>(gw.fire,4)=At<float>(carPos,0)+c.uni(-30.f,30.f); At<float>(gw.fire,8)=At<float>(carPos,4)+c.uni(-30.f,30.f); At<float>(gw.fire,12)=At<float>(carPos,8)+c.uni(-3.f,3.f);
    void* fire= c.chance(8) ? nullptr : static_cast<void*>(gw.fire);
    { auto P=MakePatches(G_RNG|G_NEW,3,B_FPV_T|B_LTM|B_WCANNON); CallThis<void>(c.a,v,fire); }
    Finish(c);
}

inline void R_BlowUpInPath(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel()); uint8* cp=Cr(0);
    for(int k=1;k<4;++k)c.BuildBase(k,c.chance(3) ? 432 : c.carModel());
    SetVt(41,FnPtr(&S3_VBlowUpCar));
    static const float spd[]{0.f,0.05f,0.0999f,0.1f,0.1001f,0.5f};
    const float mag= c.chance(2) ? spd[c.below(uint32(std::size(spd)))] : c.uni(0.f,0.8f);
    const float dx=c.uni(-1.f,1.f), dy=c.uni(-1.f,1.f), dz=c.uni(-0.3f,0.3f); const float dm=std::sqrt(dx*dx+dy*dy+dz*dz)+1e-4f;
    At<float>(cp,0x44)=mag*dx/dm; At<float>(cp,0x48)=mag*dy/dm; At<float>(cp,0x4C)=mag*dz/dm;
    SetBit(cp,0x868,5,!c.chance(3));                                                  // autoFlags.bTankExplodesCars
    const uint32 n=c.next()%5; cp[0xB9]=uint8(n);
    for(uint32 i=0;i<n;++i){
        void* e;
        const uint32 t=c.next()%7;
        if(t==0)e=nullptr; else if(t==1)e=MakePed(2,4); else e=Cr(1+int(c.below(3)));
        At<void*>(cp,0xBC+4*i)=e;
    }
    for(int k=1;k<4;++k){ SetBit(Cr(k),0x40,29,c.chance(4)); if(c.chance(5))At<uint16>(Cr(k),0x22)=432; }   // physicalFlags.bRenderScorched, modelo Rhino
    if(c.chance(2))player_vehicle=cp; else if(c.chance(2))player_vehicle=Cr(1);
    player_ped=MakePed(1,0);
    { auto P=MakePatches(G_RNG|G_NEW,3,B_FPV_T|B_FPP_T|B_CRIME); CallThis<void>(c.a,v); }
    Finish(c);
}

// Comun a BlowUpCar / BlowUpCarCutSceneNoExtras
inline CAutomobile* BlowFixture(Ctx& c){
    static const int specials[]{441,564,488,432,400};
    const int model= c.chance(3) ? specials[c.below(uint32(std::size(specials)))] : c.carModel();
    auto* v=c.BuildBase(0,model); uint8* cp=Cr(0);
    c.BuildBase(1,c.carModel()); c.BuildBase(2,c.carModel());
    SetBit(cp,0x428,21,!c.chance(12));                    // vehicleFlags.bCanBeDamaged (byte 0x42A, bit 5)
    SetStatus(cp, int(c.below(6)));
    const uint32 sub=c.below(8);                          // subtipo del vehiculo (0 coche, 4 avion...)
    At<uint32>(cp,0x594)= sub<3 ? 0 : sub==3 ? 2 : sub==4 ? 4 : sub==5 ? 3 : sub==6 ? 1 : 0;
    static const uint8 missions[]{22,35,36,37,59,62,63,57,0,1,21,64,200};
    cp[0x3BA]= c.chance(2) ? missions[c.below(uint32(std::size(missions)))] : uint8(c.next());
    At<void*>(cp,0x18)=w.clump[0]; w.clump[0][0]=2;       // clump falso (tipo 2)
    At<void*>(cp,0x65C)= c.chance(4) ? nullptr : c.Frame(6,0.f,0.f,0.f);                 // m_aCarNodes[CAR_WHEEL_LF]
    std::memset(w.at[0],0,0x100); w.at[0][0]=1; w.at[0][2]=uint8(c.chance(2) ? 4 : 0);   // atomo falso (tipo 1, bandera de render)
    At<float>(cp,0x4C0)=c.uni(0.f,1000.f); At<uint16>(cp,0x4DE)=uint16(c.next()); cp[0x4A8]=uint8(c.next());
    SetBit(cp,0x428,1,c.chance(4));                       // bIsAmbulanceOnDuty
    SetBit(cp,0x428,2,!GetBit(cp,0x428,1)&&c.chance(4));  // bIsFireTruckOnDuty (nunca las dos a la vez)
    SetBit(cp,0x428,4,c.chance(2)); SetBit(cp,0x428,6,c.chance(2)); SetBit(cp,0x428,47,c.chance(2)); cp[0x868]=uint8(c.next());
    At<float>(cp,0x4C)=c.uni(-0.4f,0.4f);
    // caja delimitadora del colmodel del modelo con maximos positivos
    for(int k=0;k<NCOLM;++k){ uint8* cm=w.colm[k]; for(int q=0;q<3;++q){ At<float>(cm,4*q)=-c.uni(0.5f,3.f); At<float>(cm,0xC+4*q)=c.uni(0.5f,3.f); } }   // las 4 ranuras (modelos repetidos)
    // jugador / danador
    player_ped=MakePed(1,0);
    player_vehicle= c.chance(3) ? static_cast<void*>(Cr(1)) : nullptr;
    AtA<uint32>(A_PLAYERS+0x140)=c.next()>>4; AtA<float>(A_PLAYERS+0x148)=c.uni(0.f,100.f);
    return v;
}
inline void* BlowDamager(Ctx& c){
    switch(c.next()%5){ case 0: return nullptr; case 1: return player_ped; case 2: return player_vehicle ? player_vehicle : static_cast<void*>(Cr(1)); default: return Cr(2); }
}
constexpr uint64 M_BLOW=B_BUMPER|B_DOOR|B_SPAWN|B_CAMSHAKE|B_KILLP|B_KILLPG|B_LAW|B_DARKEL|B_STARTFIRE|B_ADDEXPL|B_INCSTAT|B_SETCLUMP|B_FORALL|B_FPV_T|B_FPP_T;
inline void R_BlowUpCar(Ctx& c){
    Begin(c);
    auto* v=BlowFixture(c);
    void* dmgr=BlowDamager(c); const bool hide=c.chance(3);
    { auto P=MakePatches(G_RNG|G_NEW,3,M_BLOW); CallThis<void>(c.a,v,dmgr,hide); }
    c.out["dmgr"]=Id(dmgr);
    Finish(c);
}
inline void R_BlowUpCut(Ctx& c){
    Begin(c);
    auto* v=BlowFixture(c);
    const bool noShake=c.chance(2), noStuff=c.chance(2), noExpl=c.chance(3), snd=c.chance(2);
    { auto P=MakePatches(G_RNG|G_NEW,3,M_BLOW); CallThis<void>(c.a,v,noShake,noStuff,noExpl,snd); }
    Finish(c);
}

inline void R_CarOnFire(Ctx& c){
    Begin(c);
    static const int models[]{464,465,501,441,564,520,513,447,417,425,553,577,592,476,511};
    const int model= c.chance(2) ? models[c.below(uint32(std::size(models)))] : c.carModel();
    auto* v=c.BuildBase(0,model); uint8* cp=Cr(0);
    SetVt(41,FnPtr(&S3_VBlowUpCar));
    const uint32 sub=c.below(8);
    At<uint32>(cp,0x594)= sub<3 ? 0 : sub==3 ? 3 : sub==4 ? 4 : sub==5 ? 2 : 0;       // 0 coche, 3 helicoptero, 4 avion
    static const float hp[]{100.f,249.f,249.99f,250.f,251.f,500.f,1000.f};
    At<float>(cp,0x4C0)= c.chance(2) ? hp[c.below(uint32(std::size(hp)))] : c.uni(0.f,1000.f);
    { static const int st[]{0,0,3,3,5,8,2,4}; SetStatus(cp,st[c.below(8)]); }
    SetBit(cp,0x428,30,c.chance(8));                                                    // bIsDrowning (byte 0x42B, bit 6)
    At<void*>(cp,0x57C)= c.chance(3) ? static_cast<void*>(gw.fx[2]) : nullptr;       // m_pFireParticle
    static const float burns[]{0.f,100.f,2499.f,2500.f,2501.f,4900.f,4999.f,5000.f,5001.f,6000.f};
    At<float>(cp,0x8E4)= c.chance(2) ? burns[c.below(uint32(std::size(burns)))] : c.uni(0.f,6000.f);
    cp[0x981]=uint8(c.below(3)); At<void*>(cp,0x928)= c.chance(2) ? static_cast<void*>(gw.ped[2]) : nullptr;
    c.Frame(0,0.f,0.f,0.f);
    w.clump[0][0]=2; At<void*>(w.clump[0],4)=w.fr[0];                                  // marco padre del clump (GetModellingMatrix = marco + 0x10)
    At<void*>(cp,0x18)= c.chance(10) ? nullptr : static_cast<void*>(w.clump[0]);
    for(int q=0;q<28;++q)At<float>(w.vs[0],4*q)=c.uni(-2.f,2.f);                       // dummies del modelo
    { auto* dm=cp+0x5A0; for(int q=0;q<0x30;++q)dm[q]=uint8(c.next()); }
    At<float>(cp,0x44)=c.uni(-0.6f,0.6f); At<float>(cp,0x48)=c.uni(-0.6f,0.6f); At<float>(cp,0x4C)=c.uni(-0.2f,0.2f);
    const bool explodeNow=c.chance(4);
    { auto P=MakePatches(G_RNG|G_NEW,3,B_ADDEXPL|B_DELEX|B_FXPLAY|B_FXKILL|B_FXVEL|B_FXADD|B_FXCREATE|B_REGREF|B_CLEANREF|B_GROUPADD|B_GETGROUP);
      CallThis<void>(c.a,v,explodeNow); }
    Finish(c);
}

inline void R_ScanForCrimes(Ctx& c){
    Begin(c);
    auto* v=c.BuildBase(0,c.carModel());
    c.BuildBase(1,c.carModel()); uint8* pv=Cr(1);
    player_vehicle= c.chance(8) ? nullptr : static_cast<void*>(pv);
    player_ped=MakePed(1,0);
    AtA<uint32>(A_PLAYERS)=uint32(uintptr_t(player_ped));                              // CWorld::Players[PlayerInFocus].m_pPed
    static const uint16 alarms[]{0,0xFFFF,1,2,300,0x7FFF};
    At<uint16>(pv,0x45C)= c.chance(2) ? alarms[c.below(uint32(std::size(alarms)))] : uint16(c.next());
    At<uint32>(pv,0x590)= c.chance(5) ? 1u : 0u;                                       // m_nVehicleType
    SetStatus(pv, c.chance(4) ? 5 : int(c.below(5)));
    const float d= c.chance(2) ? c.uni(19.f,21.f) : c.uni(0.f,40.f);
    const float ax=c.uni(-1.f,1.f), ay=c.uni(-1.f,1.f), az=c.uni(-1.f,1.f); const float am=std::sqrt(ax*ax+ay*ay+az*az)+1e-4f;
    auto* m0=reinterpret_cast<uint8*>(w.mat[0]); auto* m1=reinterpret_cast<uint8*>(w.mat[1]);
    At<float>(m1,0x30)=At<float>(m0,0x30)+d*ax/am; At<float>(m1,0x34)=At<float>(m0,0x34)+d*ay/am; At<float>(m1,0x38)=At<float>(m0,0x38)+d*az/am;
    { auto P=MakePatches(G_RNG|G_NEW,3,B_FPV_Q|B_FPP_Q|B_SETWANT); CallThis<void>(c.a,v); }
    Finish(c);
}

} // namespace g3
} // namespace AutomobileFinal4Live
