#pragma once
// G4 (parte b): fixtures comunes y casos de DoHoverSuspensionRatios, DoBurstAndSoftGroundRatios, DoSoftGroundResistance,
// ProcessSuspension y ProcessCarWheelPair.
#include "CasesG4_a.hpp"

namespace AutomobileFinal4Live {

// ---------------------------------------------------------------------------------------------------------------------
// Tablas de superficies (se calculan una vez en InitG4: lectura pura de la tabla del EXE)
// ---------------------------------------------------------------------------------------------------------------------
inline std::array<std::vector<int>,6> g4SurfByGroup{};
inline std::vector<int> g4WaterSurf;
inline void G4_BuildSurfaceTables(){
    g4WaterSurf.clear(); for(auto& v:g4SurfByGroup)v.clear();
    for(int id=0;id<179;++id){
        const int g=int(g_surfaceInfos.GetAdhesionGroup(SurfaceId(id)));
        if(g>=0&&g<6)g4SurfByGroup[size_t(g)].push_back(id);
        if(g_surfaceInfos.IsWater(SurfaceId(id)))g4WaterSurf.push_back(id);
    }
}
inline uint8 G4_Surf(Ctx& c,int group){
    if(group<0||group>5)return uint8(c.below(179));
    auto& v=g4SurfByGroup[size_t(group)]; return v.empty() ? uint8(c.below(179)) : uint8(v[c.below(uint32(v.size()))]); }
inline uint8 G4_SurfAny(Ctx& c){
    const uint32 k=c.below(10);
    if(k<2)return G4_Surf(c,4);          // arena (SAND)
    if(k==2)return uint8(178);           // via de tren (RAILTRACK)
    if(k==3&&!g4WaterSurf.empty())return uint8(g4WaterSurf[c.below(uint32(g4WaterSurf.size()))]);
    return G4_Surf(c,int(c.below(6))); }

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures comunes
// ---------------------------------------------------------------------------------------------------------------------
inline int G4_OtherModel(Ctx& c,int not_){ int m=c.carModel(); for(int t=0;t<8&&m==not_;++t)m=c.carModel(); if(m==not_)m=(not_==400?401:400); return m; }
inline int G4_OtherModel2(Ctx& c,int a,int b){ int m=c.carModel(); for(int t=0;t<16&&(m==a||m==b);++t)m=c.carModel(); if(m==a||m==b){ m=402; if(m==a||m==b)m=404; if(m==a||m==b)m=405; } return m; }

// Rellena el colmodel k (w.colm[k]) con 'nLines' lineas de suspension de coche (hasta 16) y su CCollisionData
inline void G4_ColModel(Ctx& c,int k,int nLines,int nTris=0){
    auto* cm=w.colm[k]; std::memset(cm,0,COLM_SIZE);
    const float hx=c.uni(0.8f,1.4f), hy=c.uni(1.8f,3.0f), hz=c.uni(0.6f,1.2f);
    At<float>(cm,0x00)=-hx; At<float>(cm,0x04)=-hy; At<float>(cm,0x08)=-hz;
    At<float>(cm,0x0C)=hx;  At<float>(cm,0x10)=hy;  At<float>(cm,0x14)=hz;
    At<float>(cm,0x18)=0.f; At<float>(cm,0x1C)=0.f; At<float>(cm,0x20)=0.f; At<float>(cm,0x24)=hy;
    At<uint8>(cm,0x29)=1; At<void*>(cm,0x2C)=w.cdata[k];
    auto* cd=w.cdata[k]; std::memset(cd,0,CDATA_SIZE);
    At<uint16>(cd,4)=uint16(nTris); At<uint8>(cd,6)=uint8(nLines); At<void*>(cd,0x10)=w.lines[k];
    for(int i=0;i<16;++i){
        auto* l=w.lines[k][i]; std::memset(l,0,0x20);
        const float sx=((i&2)?1.f:-1.f)*c.uni(0.6f,hx), sy=((i&1)?-1.f:1.f)*c.uni(0.9f,hy*0.8f), sz=c.uni(-0.2f,0.4f);
        At<float>(l,0x00)=sx; At<float>(l,0x04)=sy; At<float>(l,0x08)=sz;
        At<float>(l,0x10)=sx; At<float>(l,0x14)=sy; At<float>(l,0x18)=sz-c.uni(0.4f,1.0f);
    }
}
// Tamano de rueda del model info falso k
inline void G4_MiWheel(Ctx& c,int k){ auto* mi=reinterpret_cast<CVehicleModelInfo*>(w.mi[k]); mi->m_fWheelSizeFront=c.uni(0.5f,1.1f); mi->m_fWheelSizeRear=c.uni(0.5f,1.1f); }
// Punto de colision de rueda i del coche v: posicion cerca del coche, normal unitaria con z >= nzLo, superficie B dada
inline void G4_WheelCP(Ctx& c,CAutomobile* v,int i,float nzLo,uint8 surfB){
    auto* cp=reinterpret_cast<uint8*>(&v->m_wheelColPoint[i]); std::memset(cp,0,0x2C);
    const uint8* m=w.mat[0]; (void)m;
    At<float>(cp,0)=v->GetPosition().x+c.uni(-2.f,2.f); At<float>(cp,4)=v->GetPosition().y+c.uni(-2.f,2.f); At<float>(cp,8)=v->GetPosition().z+c.uni(-1.5f,0.5f);
    float nx=c.uni(-0.6f,0.6f), ny=c.uni(-0.6f,0.6f), nz=c.uni(nzLo,1.f); double l=std::sqrt(double(nx)*nx+double(ny)*ny+double(nz)*nz);
    if(l<1e-3){ nx=0.f; ny=0.f; nz=1.f; l=1.0; }
    At<float>(cp,0x10)=float(nx/l); At<float>(cp,0x14)=float(ny/l); At<float>(cp,0x18)=float(nz/l);
    At<uint8>(cp,0x20)=uint8(c.below(100)); At<uint8>(cp,0x21)=uint8(c.below(20)); At<uint8>(cp,0x22)=uint8(c.next());
    At<uint8>(cp,0x23)=surfB; At<uint8>(cp,0x24)=uint8(c.below(20)); At<uint8>(cp,0x25)=uint8(c.next());
    At<float>(cp,0x28)=c.uni(0.f,1.f);
}
inline void G4_Speeds(Ctx& c,CAutomobile* v,float maxMove){
    v->m_vecMoveSpeed=CVector{c.uni(-maxMove,maxMove),c.uni(-maxMove,maxMove),c.uni(-0.2f*maxMove,0.2f*maxMove)};
    v->m_vecTurnSpeed=CVector{c.uni(-0.05f,0.05f),c.uni(-0.05f,0.05f),c.uni(-0.05f,0.05f)};
    if(c.chance(6))v->m_vecMoveSpeed=CVector{0.f,0.f,0.f};
}
inline void G4_Lengths(Ctx& c,CAutomobile* v){
    for(int i=0;i<4;++i){ const float line=c.uni(0.35f,1.3f); v->m_aSuspensionLineLength[i]=line; v->m_aSuspensionSpringLength[i]=line*c.uni(0.3f,0.95f); }
}
inline void G4_Globals(Ctx& c){
    AtA<float>(G4_X_TIMESTEP)=c.uni(0.2f,2.4f);
    AtA<uint32>(G4_X_TIMEMS)=1000u+c.next()%100000u;
    AtA<float>(G4_X_WETROADS)=c.chance(3) ? 0.f : c.uni(0.f,1.f);
}
inline void G4_RandFlags(Ctx& c,CAutomobile* v){
    uint32 fl=uint32(v->m_nHandlingFlagsIntValue);
    fl&=~(0x10000u|0x20000u|0x8u|0x80000u|0x100000u|0x200000u|0x40u);
    if(c.chance(3))fl|=0x10000u; if(c.chance(3))fl|=0x20000u; if(c.chance(3))fl|=0x8u; if(c.chance(5))fl|=0x80000u;
    if(c.chance(4))fl|=0x100000u; if(c.chance(4))fl|=0x200000u; if(c.chance(4))fl|=0x40u;
    v->m_nHandlingFlagsIntValue=eVehicleHandlingFlags(fl);
}
inline void G4_RandStatus(Ctx& c,CAutomobile* v){
    static constexpr eEntityStatus st[]{STATUS_PLAYER,STATUS_PHYSICS,STATUS_REMOTE_CONTROLLED,STATUS_ABANDONED,STATUS_PHYSICS,STATUS_PLAYER,STATUS_SIMPLE};
    v->SetStatus(st[c.below(uint32(std::size(st)))]);
}
inline json G4_CompJson(CAutomobile* v){ return json::array({v->m_fWheelsSuspensionCompression[0],v->m_fWheelsSuspensionCompression[1],v->m_fWheelsSuspensionCompression[2],v->m_fWheelsSuspensionCompression[3]}); }

// ---------------------------------------------------------------------------------------------------------------------
// DoHoverSuspensionRatios (ranura 69 de la vtable)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunHover(Ctx& c){
    g4S=G4_Script{}; G4_Globals(c);
    const bool steep=c.chance(6);
    auto* v=c.BuildBase(0,c.carModel(),steep ? 1.45f : 0.3f);
    G4_ColModel(c,0,4); G4_MiWheel(c,0);
    v->vehicleFlags.bIsDrowning=c.chance(6);
    const uint8* m=w.mat[0];
    const float* right=reinterpret_cast<const float*>(m), *fwd=reinterpret_cast<const float*>(m+0x10), *up=reinterpret_cast<const float*>(m+0x20), *pos=reinterpret_cast<const float*>(m+0x30);
    for(int i=0;i<4;++i){
        v->m_fWheelsSuspensionCompression[i]= c.chance(3) ? 1.0f : c.uni(0.f,0.99f);
        G4_WheelCP(c,v,i,0.2f,uint8(c.below(179)));
        const float* ls=reinterpret_cast<const float*>(w.lines[0][i]);
        const float sz=right[2]*ls[0]+fwd[2]*ls[1]+up[2]*ls[2]+pos[2];
        const float ez=right[2]*ls[4]+fwd[2]*ls[5]+up[2]*ls[6]+pos[2];
        // punto de colision por debajo de la linea para que el agua pueda superarlo
        At<float>(&v->m_wheelColPoint[i],8)=ez-c.uni(0.5f,3.f);
        const uint32 scen=c.below(7);
        g4S.wRet[i]= scen==0 ? uint8(0) : uint8(1);
        switch(scen){
        case 1: g4S.wLevel[i]=At<float>(&v->m_wheelColPoint[i],8)-c.uni(0.1f,1.f); break;     // nivel bajo el punto de colision
        case 2: g4S.wLevel[i]=ez-c.uni(0.f,0.9f); break;                                        // <= end.z
        case 3: g4S.wLevel[i]=ez+(sz-ez)*c.uni(0.05f,0.95f); break;                             // entre end.z y start.z
        case 4: g4S.wLevel[i]=sz+c.uni(0.01f,1.5f); break;                                      // por encima de start.z
        case 5: g4S.wLevel[i]=ez; break;                                                        // exactamente end.z
        default: g4S.wLevel[i]=ez+c.uni(-3.f,3.f); break;
        }
    }
    { auto P=MakePatches(0,4,G4_B_WATER); CallThis<void>(c.a,v); }
    c.out["comp"]=G4_CompJson(v);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// DoBurstAndSoftGroundRatios (ranura 52)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunBurst(Ctx& c){
    g4S=G4_Script{}; G4_Globals(c);
    const int model= c.chance(7) ? 432 : c.carModel();
    auto* v=c.BuildBase(0,model);
    G4_ColModel(c,0,4); G4_MiWheel(c,0); G4_Lengths(c,v); G4_Speeds(c,v,c.chance(2) ? 1.2f : 0.15f);
    v->m_vecMoveSpeed.z*=0.1f;
    for(int i=0;i<4;++i){
        const uint32 s=c.below(10);
        v->m_damageManager.m_anWheelsStatus[i]= s<2 ? WHEEL_STATUS_MISSING : (s<5 ? WHEEL_STATUS_BURST : WHEEL_STATUS_OK);
        v->m_fWheelsSuspensionCompression[i]= c.chance(5) ? 1.0f : c.uni(0.f,0.995f);
        const uint32 sf=c.below(10);
        const uint8 surf= sf<4 ? G4_Surf(c,4) : (sf<6 ? uint8(178) : G4_Surf(c,int(c.below(6))));
        G4_WheelCP(c,v,i,0.3f,surf);
        v->m_wheelRotation[i]=c.uni(-50.f,50.f); v->m_wheelSpeed[i]= c.chance(5) ? 0.f : c.uni(-1.f,1.f);
    }
    uint32 fl=uint32(v->m_nHandlingFlagsIntValue)&~(0x100000u|0x200000u);
    if(c.chance(3))fl|=0x100000u; if(c.chance(3))fl|=0x200000u; v->m_nHandlingFlagsIntValue=eVehicleHandlingFlags(fl);
    { auto P=MakePatches(G_RNG,4,0); CallThis<void>(c.a,v); }
    c.out["comp"]=G4_CompJson(v);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// DoSoftGroundResistance (no virtual, 0x6A4AF0, ret 4)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunSoft(Ctx& c){
    g4S=G4_Script{}; G4_Globals(c);
    const uint32 mk=c.below(10);
    const int model= mk==0 ? 441 : (mk==1 ? 432 : c.carModel());
    auto* v=c.BuildBase(0,model);
    G4_ColModel(c,0,4); G4_Lengths(c,v);
    G4_Speeds(c,v,c.chance(3) ? 0.2f : 1.0f);
    if(c.chance(3))v->m_vecMoveSpeed=CVector{c.uni(-0.25f,0.25f),c.uni(-0.25f,0.25f),0.f};
    const uint32 mode=c.below(6);   // 0-2 arena, 3-4 via de tren, 5 nada
    for(int i=0;i<4;++i){
        v->m_fWheelsSuspensionCompression[i]= c.chance(3) ? 1.0f : c.uni(0.f,0.99f);
        uint8 surf= mode<3 ? G4_Surf(c,4) : (mode<5 ? uint8(178) : G4_Surf(c,int(c.below(4))));
        if(c.chance(4))surf=G4_SurfAny(c);
        G4_WheelCP(c,v,i,0.3f,surf);
    }
    uint32 fl=uint32(v->m_nHandlingFlagsIntValue)&~(0x100000u|0x200000u);
    if(c.chance(3))fl|=0x100000u; if(c.chance(3))fl|=0x200000u; v->m_nHandlingFlagsIntValue=eVehicleHandlingFlags(fl);
    v->autoFlags.bIsBoggedDownInSand=c.chance(5);
    uint32 extra=c.chance(2) ? 0u : (c.next()&0x37u);
    { auto P=MakePatches(0,4,G4_B_FORCE); CallThis<void>(c.a,v,&extra); }
    c.out["extra"]=extra;
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// ProcessSuspension (ranura 70)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunSusp(Ctx& c){
    g4S=G4_Script{}; G4_Globals(c);
    const uint32 mk=c.below(12);
    const int model= mk<2 ? 432 : (mk==2 ? 539 : c.carModel());
    auto* v=c.BuildBase(0,model);
    G4_ColModel(c,0,12); G4_MiWheel(c,0); G4_Lengths(c,v);
    const int model2=G4_OtherModel(c,model);
    auto* o=c.BuildBase(1,model2,0.2f); G4_ColModel(c,1,4); G4_Speeds(c,o,1.f);
    G4_RandFlags(c,v); G4_RandStatus(c,v);
    v->autoFlags.bIsMonsterTruck=c.chance(8);
    At<float>(Cr(0),0x994)=c.uni(-1.f,1.f);                       // AsPlane()->m_fAccelerationBreakStatus (solo Vortex)
    AtA<uint8>(G4_X_REPLAY)= c.chance(5) ? uint8(1) : uint8(0);
    AtA<uint8>(G4_X_CHEATS+34)= c.chance(5) ? uint8(1) : uint8(0);
    // velocidad: a veces casi parado para recorrer la rama de "rodar sobre las ruedas"
    const uint32 sp=c.below(3);
    G4_Speeds(c,v,sp==0 ? 0.01f : 0.9f);
    if(sp==0)v->m_vecMoveSpeed=CVector{c.uni(-0.02f,0.02f),c.uni(-0.02f,0.02f),c.uni(-0.01f,0.01f)};
    const uint32 pat=c.below(8);
    for(int i=0;i<4;++i){
        float comp= c.chance(4) ? 1.0f : c.uni(0.f,0.99f);
        const bool left=(i==0||i==1);
        if(pat==1) comp=1.0f;
        else if(pat==2) comp= left ? 1.0f : (c.chance(3) ? 1.0f : c.uni(0.f,0.99f));
        else if(pat==3) comp= left ? (c.chance(3) ? 1.0f : c.uni(0.f,0.99f)) : 1.0f;
        v->m_fWheelsSuspensionCompression[i]=comp;
        G4_WheelCP(c,v,i,c.chance(3) ? -0.2f : 0.3f,G4_SurfAny(c));
        v->m_apWheelCollisionEntity[i]= c.chance(3) ? reinterpret_cast<CPhysical*>(Cr(1)) : nullptr;
        v->m_vWheelCollisionPos[i]=CVector{c.uni(-2.f,2.f),c.uni(-2.f,2.f),c.uni(-1.f,1.f)};
    }
    for(int d=4;d<6;++d){ v->m_doors[d].m_openAngle=c.uni(0.f,1.1f); v->m_doors[d].m_closedAngle=c.uni(0.f,1.1f); v->m_doors[d].m_angle=c.uni(0.f,1.1f); v->m_doors[d].m_prevAngle=c.uni(0.f,1.1f); }
    // tiempo / hidraulica
    v->m_pHandlingData->m_fSuspensionDampingLevel= c.chance(2) ? c.uni(0.05f,0.09f) : c.uni(0.11f,0.5f);
    { auto P=MakePatches(0,4,G4_B_FORCE|G4_B_SPRING); CallThis<void>(c.a,v); }
    c.out["comp"]=G4_CompJson(v);
    return true;
}

// ---------------------------------------------------------------------------------------------------------------------
// ProcessCarWheelPair (no virtual, 0x6A4EC0, ret 24h)
// ---------------------------------------------------------------------------------------------------------------------
inline bool G4_RunWheelPair(Ctx& c){
    g4S=G4_Script{}; G4_Globals(c);
    auto* v=c.BuildBase(0,c.carModel());
    G4_ColModel(c,0,4); G4_Lengths(c,v);
    G4_RandFlags(c,v); G4_RandStatus(c,v);
    const bool front=c.chance(2);
    const int left= front ? 0 : 1, right= front ? 2 : 3;
    const float steer= c.chance(5) ? -9999.99f : c.uni(-0.8f,0.8f);
    auto* speeds=reinterpret_cast<CVector*>(w.misc);            // 4 x 12 bytes
    auto* points=reinterpret_cast<CVector*>(w.misc+0x40);       // 4 x 12 bytes
    for(int i=0;i<4;++i){
        speeds[i]=CVector{c.uni(-1.f,1.f),c.uni(-1.f,1.f),c.uni(-0.3f,0.3f)};
        points[i]=CVector{c.uni(-1.5f,1.5f),c.uni(-2.f,2.f),c.uni(-1.f,0.f)};
        G4_WheelCP(c,v,i,0.3f,G4_SurfAny(c));
        v->m_fWheelsSuspensionCompression[i]= c.chance(4) ? 1.0f : c.uni(0.f,0.99f);
        const uint32 k=c.below(6);
        v->m_WheelCounts[i]= k==0 ? 0.f : (k==1 ? -1.f : c.uni(0.1f,3.f));
        v->m_WheelStates[i]=tWheelState(c.below(4));
        v->m_wheelSpeed[i]=c.uni(-1.2f,1.2f); v->m_wheelRotation[i]=c.uni(-30.f,30.f);
        v->m_fWheelBurnoutSpeed[i]=c.uni(0.f,1.f);
        const uint32 s=c.below(8);
        v->m_damageManager.m_anWheelsStatus[i]= s==0 ? WHEEL_STATUS_BURST : (s==1 ? WHEEL_STATUS_MISSING : WHEEL_STATUS_OK);
        g4S.wheelOut[i]=int32(c.below(4));
    }
    v->m_damageManager.m_fWheelDamageEffect=c.uni(0.1f,1.f);
    v->vehicleFlags.bIsHandbrakeOn=c.chance(4);
    v->m_bDoingBurnout= c.chance(3);
    At<uint8>(Cr(0),0x86B)= c.chance(25) ? uint8(1) : uint8(0);  // el EXE lee la palabra 0x86A (byte de relleno)
    v->m_fTireTemperature=c.uni(0.9f,3.5f);
    v->m_fSteerAngle=c.uni(-0.6f,0.6f); v->m_GasPedal=c.uni(-1.f,1.f); v->m_nNumContactWheels=uint8(c.below(5));
    v->m_pHandlingData->m_fSuspensionForceLevel=c.uni(0.4f,2.5f); v->m_pHandlingData->m_fBrakeBias=c.uni(0.2f,0.8f);
    v->m_pHandlingData->m_fTractionBias=c.uni(0.2f,0.8f); v->m_pHandlingData->m_fSuspensionBiasBetweenFrontAndRear=c.uni(0.3f,0.7f);
    if(c.chance(20))At<uint32>(w.hd[0],0xCC)|=0x2000000u;       // helicoptero real: salida anticipada
    const float traction=c.uni(0.3f,2.0f);
    const uint32 am=c.below(4);
    const float accel= am==0 ? 0.f : (am==1 ? c.uni(0.1f,1.f) : -c.uni(0.1f,1.f));
    const float brake= c.chance(3) ? 0.f : c.uni(0.f,1.f);
    { auto P=MakePatches(0,4,G4_B_FORCE|G4_B_WHEEL);
      CallThis<void>(c.a,v,int(left),int(right),steer,speeds,points,traction,accel,brake,front); }
    c.out["comp"]=G4_CompJson(v);
    return true;
}

}
