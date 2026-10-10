#pragma once
// G4 (parte c): peatones falsos, simbolos exportados y casos de ProcessBuoyancy, ProcessEntityCollision,
// ProcessControlCollisionCheck y VehicleDamage.
#include "CasesG4_b.hpp"

namespace AutomobileFinal4Live {

// ---------------------------------------------------------------------------------------------------------------------
// Peatones falsos (tamano real de CPed / CPedIntelligence, desplazamientos crudos de los miembros protegidos)
// ---------------------------------------------------------------------------------------------------------------------
constexpr size_t G4_PEDSZ=sizeof(CPed)+64, G4_INTELSZ=sizeof(CPedIntelligence)+64;
alignas(16) inline std::array<std::array<uint8,G4_PEDSZ>,5> g4PedBuf{};       // 0 conductor, 1 pasajero, 2 conductor de la victima, 3 peaton agresor, 4 jugador
alignas(16) inline std::array<std::array<uint8,G4_INTELSZ>,5> g4IntelBuf{};
inline bool g4PedOk{};                                                         // todos los simbolos de peatones/eventos resueltos

// Peaton falso idx (matriz w.mat[matSlot] si matSlot >= 0). player: PED_TYPE_PLAYER1
inline CPed* G4_Ped(Ctx& c,int idx,bool player,int matSlot){
    auto* b=g4PedBuf[size_t(idx)].data(); std::memset(b,0,G4_PEDSZ); std::memset(g4IntelBuf[size_t(idx)].data(),0,G4_INTELSZ);
    auto* p=reinterpret_cast<CPed*>(b);
    p->SetType(ENTITY_TYPE_PED);
    At<void*>(b,0x47C)=g4IntelBuf[size_t(idx)].data();                         // m_pIntelligence
    p->m_nPedType= player ? PED_TYPE_PLAYER1 : PED_TYPE_CIVMALE;
    if(matSlot>=0){ c.RandMatrix(w.mat[matSlot],0.2f,50.f); At<void*>(b,0x14)=w.mat[matSlot]; }
    p->bIsStanding=c.chance(2);
    p->m_vecAnimMovingShiftLocal=CVector2D{c.uni(-1.f,1.f),c.uni(-1.f,1.f)};
    p->m_fMass=c.uni(40.f,100.f);
    return p;
}

// ---------------------------------------------------------------------------------------------------------------------
// Modulo de la DLL y simbolos exportados (destructores de eventos)
// ---------------------------------------------------------------------------------------------------------------------
inline uintptr_t G4_Exported(const char* mangled){
    HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&G4_Exported),&mod);
    if(!mod)return 0; return uintptr_t(GetProcAddress(mod,mangled)); }

// ---------------------------------------------------------------------------------------------------------------------
// ProcessBuoyancy (no virtual, 0x6A8C00)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunBuoy(Ctx& c){
    g4S=G4_Script{}; g4nObj=0; G4_Globals(c);
    const uint32 mk=c.below(14);
    const int model= mk==0 ? 447 : (mk==1 ? 417 : (mk==2 ? 539 : c.carModel()));
    const bool steep=c.chance(8);
    auto* v=c.BuildBase(0,model,steep ? 2.6f : 0.3f);
    G4_ColModel(c,0,4); G4_Lengths(c,v); G4_RandStatus(c,v); G4_Speeds(c,v,0.8f);
    const float ts=AtA<float>(G4_X_TIMESTEP);
    const float usedMass=v->m_fMass*0.008f;
    v->m_fBuoyancyConstant= usedMass*c.uni(0.4f,1.8f);
    if(c.chance(6))v->m_vecMoveSpeed.z=-c.uni(1.f,3.f);
    const uint32 sub=c.below(10);
    v->m_nVehicleSubType= sub==0 ? VEHICLE_TYPE_PLANE : (sub<3 ? VEHICLE_TYPE_QUAD : VEHICLE_TYPE_AUTOMOBILE);
    if(mk<2 || c.chance(7)){ At<uint32>(w.hd[0],0xCC)|=0x2000000u; v->m_fHeliRotorSpeed=c.uni(0.f,0.6f); }   // helicoptero real
    v->physicalFlags.bMakeMassTwiceAsBig=c.chance(4);
    v->vehicleFlags.bIsDrowning=c.chance(4); v->vehicleFlags.bEngineOn=c.chance(2);
    v->autoFlags.bWaterTight=c.chance(3);
    AtA<uint8>(G4_X_CHEATS+34)= c.chance(4) ? uint8(1) : uint8(0);
    const uint32 cp=c.below(6);
    for(int i=0;i<4;++i){
        v->m_fWheelsSuspensionCompression[i]= (cp==0) ? 1.0f : (c.chance(4) ? 1.0f : c.uni(0.f,0.99f));
        G4_WheelCP(c,v,i,0.2f,G4_SurfAny(c));
    }
    // guion de cBuoyancy::ProcessBuoyancy
    static constexpr float targets[]{0.2f,0.5f,0.7f,0.95f,1.1f,1.5f,2.5f,3.2f,5.f,-0.3f};
    const float T=targets[c.below(uint32(std::size(targets)))];
    const float tsUsed= (0.01f>ts) ? 0.01f : ts;
    g4S.buoyRet= c.chance(5) ? uint8(0) : uint8(1);
    g4S.buoyForce[0]=c.uni(-5.f,5.f); g4S.buoyForce[1]=c.uni(-5.f,5.f); g4S.buoyForce[2]=T*((tsUsed*v->m_fMass)*0.008f);
    g4S.buoyTurn[0]=c.uni(-1.f,1.f); g4S.buoyTurn[1]=c.uni(-1.f,1.f); g4S.buoyTurn[2]=c.uni(-1.f,0.f);
    // peatones
    if(g4PedOk){
        const uint32 dk=c.below(4);
        if(dk!=0){
            v->m_pDriver=G4_Ped(c,0,dk==2,-1);
            v->m_nMaxPassengers=uint8(1+c.below(4));
            for(uint32 j=0;j<v->m_nMaxPassengers;++j){ v->m_apPassengers[j]= c.chance(3) ? nullptr : (j==0 ? G4_Ped(c,1,c.chance(3),-1) : v->m_apPassengers[0]); }
        } else {
            v->m_pDriver=nullptr; v->m_nMaxPassengers=uint8(c.below(3));
            for(uint32 j=0;j<v->m_nMaxPassengers;++j) v->m_apPassengers[j]= c.chance(2) ? nullptr : G4_Ped(c,1,c.chance(3),-1);
        }
        g4S.affects=c.chance(2) ? uint8(1) : uint8(0);
    }
    SetVt(41,FnPtr(&G4_BlowUp));
    { auto P=MakePatches(G_RNG,4,G4_B_FORCE|G4_B_BUOY|G4_B_REF|(g4PedOk ? G4_B_PED : 0ull)); CallThis<void>(c.a,v); }
    c.out["comp"]=G4_CompJson(v);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// ProcessEntityCollision (ranura 22)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunEntCol(Ctx& c){
    g4S=G4_Script{}; G4_Globals(c);
    const uint32 mk=c.below(14);
    const int model= mk<2 ? 432 : (mk==2 ? 530 : c.carModel());
    const bool rhino= model==432;
    auto* v=c.BuildBase(0,model);
    G4_ColModel(c,0,rhino ? 12 : 4,int(c.below(20))); G4_MiWheel(c,0); G4_Lengths(c,v);
    const int model2=G4_OtherModel(c,model);
    auto* e=c.BuildBase(1,model2,0.2f);
    G4_ColModel(c,1,4,int(c.below(20)));
    static constexpr eEntityType types[]{ENTITY_TYPE_VEHICLE,ENTITY_TYPE_VEHICLE,ENTITY_TYPE_OBJECT,ENTITY_TYPE_OBJECT,ENTITY_TYPE_BUILDING,ENTITY_TYPE_BUILDING,ENTITY_TYPE_PED,ENTITY_TYPE_VEHICLE};
    const eEntityType et=types[c.below(uint32(std::size(types)))];
    e->SetType(et);
    At<uint32>(Cr(1),0x1C)|= (c.chance(2) ? 0x40000000u : 0u) | (c.chance(2) ? 0x80000000u : 0u);       // m_bTunnel / m_bTunnelTransition
    if(et==ENTITY_TYPE_OBJECT) e->physicalFlags.bDisableCollisionForce=c.chance(2);
    e->m_apWheelCollisionEntity[0]=nullptr;
    for(int i=0;i<4;++i) e->m_anCollisionLighting[i].value=uint8(c.next());
    const uint32 sm=c.below(8);
    if(sm==0)v->SetStatus(STATUS_GHOST); else if(sm==1)v->SetStatus(STATUS_SIMPLE); else v->SetStatus(STATUS_PHYSICS);
    v->physicalFlags.bSkipLineCol=c.chance(8); v->physicalFlags.bProcessingShift=c.chance(10);
    v->m_pTowingVehicle= c.chance(8) ? reinterpret_cast<CVehicle*>(e) : nullptr;
    v->m_pVehicleBeingTowed= c.chance(8) ? reinterpret_cast<CVehicle*>(e) : nullptr;
    v->m_pHandlingData->m_fSuspensionHighSpdComDamp= c.chance(4) ? 0.f : c.uni(10.f,300.f);
    G4_Speeds(c,v,c.chance(3) ? 0.3f : 1.2f);
    for(int i=0;i<4;++i){
        v->m_fWheelsSuspensionCompression[i]= c.chance(3) ? 1.0f : c.uni(0.1f,1.0f);
        v->m_fWheelsSuspensionCompressionPrev[i]= c.uni(0.f,1.f);
        G4_WheelCP(c,v,i,0.3f,G4_SurfAny(c));
        v->m_apWheelCollisionEntity[i]=nullptr;
        const uint32 s=c.below(8);
        v->m_damageManager.m_anWheelsStatus[i]= s==0 ? WHEEL_STATUS_BURST : WHEEL_STATUS_OK;
    }
    for(int d=4;d<6;++d){ v->m_doors[d].m_openAngle=c.uni(0.f,1.f); v->m_doors[d].m_closedAngle=c.uni(0.f,1.f); v->m_doors[d].m_angle=c.uni(0.f,1.f); v->m_doors[d].m_prevAngle=c.uni(0.f,1.f); }
    v->m_wMiscComponentAngle= c.chance(5) ? uint16(c.below(100)) : uint16(0); v->m_wMiscComponentAnglePrev= c.chance(8) ? uint16(c.below(100)) : uint16(0);
    // guion de ProcessColModels: numero de puntos, distancias de ruedas (0..11) y semilla
    g4S.pcN= c.chance(3) ? 0 : int32(c.below(6)); g4S.pcSeed=c.next()|1u; g4S.pcForklift= model==530 ? uint8(1) : uint8(0);
    for(int i=0;i<12;++i){ const uint32 k=c.below(5); g4S.pcDist[i]= k==0 ? 1.0f : (k==1 ? c.uni(1.f,1.3f) : c.uni(0.f,0.99f)); }
    std::memset(g4OutBuf.data(),0x5A,g4OutBuf.size());
    int32 r=0;
    { auto P=MakePatches(0,4,G4_B_COLL|G4_B_REF); r=CallThis<int32>(c.a,v,static_cast<void*>(e),static_cast<void*>(g4OutBuf.data())); }
    // diagnostico: el lado candidato anota los bytes de outbuf que difieren del original (solo si hay diferencias)
    static std::array<uint8,32*0x2C+64> savedOut{};
    if(!c.candidate) savedOut=g4OutBuf;
    else { json d=json::array(); for(size_t k=0;k<g4OutBuf.size()&&d.size()<12;++k) if(savedOut[k]!=g4OutBuf[k]) d.push_back({{"off",k},{"pt",k/0x2C},{"field",k%0x2C},{"o",savedOut[k]},{"c",g4OutBuf[k]}});
           if(!d.empty()) c.out["outbuf_diff"]=d; }
    c.ret=r; c.Hash("outbuf",g4OutBuf.data(),g4OutBuf.size()); c.out["comp"]=G4_CompJson(v);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// ProcessControlCollisionCheck (ranura 23, ret 4)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunCtrlCol(Ctx& c){
    g4S=G4_Script{}; G4_Globals(c);
    auto* v=c.BuildBase(0,c.carModel());
    G4_ColModel(c,0,4); G4_Speeds(c,v,1.0f);
    v->physicalFlags.bSkipLineCol=c.chance(3); v->physicalFlags.bProcessingShift=c.chance(3);
    { uint32& fl=At<uint32>(Cr(0),0x1C); fl&=~0x31u; fl|= (c.chance(2) ? 0x10u : 0u) | (c.chance(2) ? 0x20u : 0u) | (c.chance(2) ? 0x1u : 0u); }   // bIsStuck / bIsInSafePosition / bUsesCollision
    v->m_fMovingSpeed=c.uni(0.f,2.f);
    for(int i=0;i<4;++i) v->m_fWheelsSuspensionCompression[i]=c.uni(0.f,1.f);
    g4S.checkBits=c.next();
    if(c.chance(4))g4S.checkBits=0u; if(c.chance(6))g4S.checkBits=0xFFFFFFFFu;
    const bool applySpeed=c.chance(2);
    { auto P=MakePatches(0,4,G4_B_CHECK); CallThis<void>(c.a,v,applySpeed); }
    c.out["calls"]=g4S.checkN;
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// VehicleDamage (ranura 56, ret 18h)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunDamage(Ctx& c){
    g4S=G4_Script{}; g4nObj=0; G4_Globals(c);
    const uint32 mk=c.below(24);
    int model= c.carModel();
    if(mk==0)model=428; else if(mk==1)model=465; else if(mk==2)model=501; else if(mk==3)model=564; else if(mk==4||mk==5)model=424; else if(mk==6)model=441;
    const bool flipped=c.chance(5);
    auto* v=c.BuildBase(0,model,flipped ? 2.9f : 0.3f);
    G4_ColModel(c,0,4); G4_MiWheel(c,0); G4_Speeds(c,v,c.chance(2) ? 1.2f : 0.1f);
    v->vehicleFlags.bCanBeDamaged= !c.chance(14);
    v->vehicleFlags.bIsLawEnforcer=c.chance(4); v->vehicleFlags.bTakeLessDamage=c.chance(4); v->vehicleFlags.bIsVan=c.chance(4);
    v->vehicleFlags.bEngineOn=c.chance(2);
    v->physicalFlags.bInvulnerable=c.chance(6); v->physicalFlags.bCollisionProof=c.chance(10); v->physicalFlags.bSubmergedInWater=c.chance(8);
    v->autoFlags.bDoesNotGetDamagedUpsideDown=c.chance(6);
    static constexpr eEntityStatus sts[]{STATUS_PLAYER,STATUS_PHYSICS,STATUS_PHYSICS,STATUS_WRECKED,STATUS_REMOTE_CONTROLLED,STATUS_ABANDONED,STATUS_PLAYER};
    v->SetStatus(sts[c.below(uint32(std::size(sts)))]);
    v->m_nCreatedBy= c.chance(4) ? MISSION_VEHICLE : RANDOM_VEHICLE;
    v->m_nVehicleSubType= c.chance(8) ? VEHICLE_TYPE_QUAD : VEHICLE_TYPE_AUTOMOBILE;
    v->m_pHandlingData->m_fCollisionDamageMultiplier=c.uni(0.2f,3.f);
    // salud: valores cerca de los umbrales 250 / 400 / 600
    static constexpr float healths[]{1000.f,800.f,620.f,590.f,420.f,390.f,260.f,240.f,120.f,30.f,5.f,0.5f};
    v->m_fHealth= c.chance(2) ? c.uni(1.f,1000.f) : healths[c.below(uint32(std::size(healths)))];
    v->m_damageManager.m_nEngineStatus=uint8(c.chance(2) ? 0u : c.below(250));
    v->m_damageManager.m_panelStates.frontBumper=c.below(4); v->m_damageManager.m_panelStates.rearBumper=c.below(4);
    v->m_damageManager.m_nLightsStatus=c.next()&0x55u;                       // luces: 2 bits por luz, valores 0/1
    // entidad que causa el dano (m_pDamageEntity): 0 nada, 1-3 vehiculo, 4 peaton, 5 edificio, 6 objeto, 7 vehiculo con conductor
    const uint32 dk=c.below(8);
    const int model2= c.chance(6) ? 432 : G4_OtherModel(c,model);
    auto* o=c.BuildBase(1,model2,0.2f); G4_ColModel(c,1,4); G4_Speeds(c,o,1.2f);
    o->SetStatus(c.chance(3) ? STATUS_PLAYER : STATUS_PHYSICS); o->m_nCreatedBy= c.chance(4) ? MISSION_VEHICLE : RANDOM_VEHICLE;
    CPed* dmgPed=nullptr; CPed* playerPed=nullptr;
    if(g4PedOk){
        o->m_pDriver= (dk==7||c.chance(3)) ? G4_Ped(c,2,false,-1) : nullptr;
        v->m_pDriver= c.chance(3) ? nullptr : G4_Ped(c,0,c.chance(4),-1);
        g4S.pickPed= c.chance(2) ? static_cast<void*>(G4_Ped(c,1,false,-1)) : nullptr;
        if(dk==4)dmgPed=G4_Ped(c,3,false,5);
        playerPed=G4_Ped(c,4,true,-1);
    } else { o->m_pDriver=nullptr; v->m_pDriver=nullptr; }
    CEntity* dmg=nullptr;
    switch(dk){
    case 0: dmg=nullptr; break;
    case 4: dmg=dmgPed; break;
    case 5: o->SetType(ENTITY_TYPE_BUILDING); dmg=o; break;
    case 6: o->SetType(ENTITY_TYPE_OBJECT); dmg=o; break;
    default: dmg=o; break;
    }
    v->m_pDamageEntity=dmg;
    v->m_pTowingVehicle= (dmg&&c.chance(10)) ? reinterpret_cast<CVehicle*>(dmg) : nullptr;
    v->m_pVehicleBeingTowed= (dmg&&c.chance(12)) ? reinterpret_cast<CVehicle*>(dmg) : nullptr;
    // colision guardada
    const float spd=c.uni(0.f,2.f);
    v->m_vecLastCollisionImpactVelocity=CVector{c.uni(-1.f,1.f)*spd,c.uni(-1.f,1.f)*spd,c.uni(-0.3f,0.3f)*spd};
    v->m_vecLastCollisionPosn=CVector{v->GetPosition().x+c.uni(-2.5f,2.5f),v->GetPosition().y+c.uni(-3.f,3.f),v->GetPosition().z+c.uni(-1.f,1.f)};
    static constexpr uint16 pieces[]{0,1,2,3,3,4,4,5,6,7,8,9,10,11,12,19,13,18};
    v->m_nPieceType=pieces[c.below(uint32(std::size(pieces)))];
    v->m_fDamageIntensity= c.chance(8) ? 0.f : c.uni(5.f,600.f);
    // parametros de la llamada
    const bool zeroCall=c.chance(2);
    const float dmgIntensity= zeroCall ? 0.f : c.uni(2.f,500.f);
    const uint32 component=pieces[c.below(uint32(std::size(pieces)))];
    CEntity* damager=nullptr;
    if(!zeroCall){
        const uint32 q=c.below(5);
        if(q==1||q==2) damager=dmg;
        else if(q>=3){
            auto* d2=c.BuildBase(2,G4_OtherModel2(c,model,model2),0.2f); G4_ColModel(c,2,4);
            d2->SetType(c.chance(3) ? ENTITY_TYPE_BUILDING : ENTITY_TYPE_VEHICLE); damager=reinterpret_cast<CEntity*>(Cr(2)); }
    }
    auto* coors=reinterpret_cast<float*>(w.misc+0x100); auto* dir=reinterpret_cast<float*>(w.misc+0x110);
    coors[0]=v->GetPosition().x+c.uni(-2.5f,2.5f); coors[1]=v->GetPosition().y+c.uni(-3.f,3.f); coors[2]=v->GetPosition().z+c.uni(-1.f,1.f);
    dir[0]=c.uni(-1.f,1.f); dir[1]=c.uni(-1.f,1.f); dir[2]=c.uni(-1.f,1.f);
    const int32 weapon= c.chance(3) ? 49 : int32(c.below(54));
    // guion de los dobles
    g4S.canDamage= c.chance(7) ? uint8(0) : uint8(1); g4S.knockRet=c.chance(5) ? uint8(1) : uint8(0); AtA<int32>(G4_X_CURRAREA)= c.chance(2) ? 0 : int32(1+c.below(3));
    static constexpr float progs[]{0.f,50.f,99.f,100.f,120.f};
    g4S.progress=progs[c.below(uint32(std::size(progs)))];
    g4S.vehAudioType=int16(c.below(3)); g4S.lightSmash= c.chance(2) ? 0u : c.below(4)+1u; g4S.applyRet=1; g4S.affects=1;
    // jugador (FindPlayerVehicle / FindPlayerPed)
    const uint32 pk=c.below(4);
    tbPlayerVehicle= pk==0 ? nullptr : (pk==2 ? static_cast<void*>(Cr(1)) : static_cast<void*>(Cr(0)));
    tbPlayerPed= (dk==4 && dmgPed && c.chance(2)) ? static_cast<void*>(dmgPed) : static_cast<void*>(playerPed);
    SetVt(41,FnPtr(&G4_BlowUp));
    { auto P=MakePatches(G_RNG|G_PLAYER,4,G4_B_DMG|G4_B_REF|G4_B_FORCE|(g4PedOk ? G4_B_PED : 0ull));
      CallThis<void>(c.a,v,dmgIntensity,component,static_cast<void*>(damager),coors,dir,weapon); }
    c.out["health"]=v->m_fHealth;
    return true;
}

}
