#pragma once
// Grupo G5 (parte b): fixtures compartidos y banco de CAutomobile::ProcessControl (0x6B1880, ranura 10 de la vtable).
#include "CasesG5_a.hpp"

namespace AutomobileFinal4Live {

// Modelos con rama propia en ProcessControl / ProcessAI (indices reales del juego)
inline constexpr int kG5Special[]{ 432,564,443,486,406,524,592,530,525,531,407,601,460,464,539,420,438,532,447,417,423,495,424,441 };

// Reinicia el estado de los dobles y los buferes propios; deja un jugador falso valido (g5ped[3]) con CWanted falso
inline void G5Reset(){
    std::memset(&g5,0,sizeof(g5));
    std::memset(g5ped,0,sizeof(g5ped)); std::memset(g5intel,0,sizeof(g5intel)); std::memset(g5aux,0,sizeof(g5aux));
    g5.fat=1.0f;
    At<void*>(g5aux,0)=g5aux+0x100;                                  // CPlayerPedData::m_pWanted
    for(int k=0;k<4;++k){
        At<void*>(g5ped[k],0x47C)=g5intel[k];                        // CPed::m_pIntelligence
        At<void*>(g5ped[k],0x480)=g5aux;                             // CPed::m_pPlayerData
        At<uint8>(g5ped[k],G5o::Status)=uint8(3);                    // tipo de entidad: peaton
    }
}
// Peaton falso: tipo de peaton (0 = jugador 1, 1 = jugador 2) y estado
inline void G5InitPed(int k,uint32 pedType,uint32 pedState){ At<uint32>(g5ped[k],0x598)=pedType; At<uint32>(g5ped[k],0x530)=pedState; }

// Cambia la entrada de handling del coche k (copia real del EXE) y recoloca lo que BuildBase deriva de ella
inline void G5Handling(Ctx& c,int k,int idx){
    c.FillHandling(k,idx);
    auto* v=Car(k); v->m_pHandlingData=reinterpret_cast<tHandlingData*>(w.hd[k]);
    v->m_nHandlingFlagsIntValue=v->m_pHandlingData->m_nHandlingFlags;
    v->m_fMass=v->m_pHandlingData->m_fMass; v->m_fTurnMass=v->m_pHandlingData->m_fTurnMass; v->m_vecCentreOfMass=v->m_pHandlingData->m_vecCentreOfMass;
}

// Modelo de colision falso del coche 0: caja, 4 u 8 lineas (o 4 discos) y puntero a los datos
inline void G5ColModel(Ctx& c,int k,uint32 numLines,bool disks){
    auto* cm=w.colm[k]; std::memset(cm,0,COLM_SIZE);
    const float ux=c.uni(0.8f,1.2f), uy=c.uni(2.0f,3.0f), uz=c.uni(0.5f,0.9f);
    At<float>(cm,0)=-ux; At<float>(cm,4)=-uy; At<float>(cm,8)=-c.uni(0.4f,0.9f); At<float>(cm,12)=ux; At<float>(cm,16)=uy; At<float>(cm,20)=uz;
    At<void*>(cm,0x2C)=w.cdata[k];
    auto* cd=w.cdata[k]; std::memset(cd,0,CDATA_SIZE);
    At<uint8>(cd,6)=uint8(numLines); At<uint8>(cd,7)=uint8(disks ? 1 : 0); At<void*>(cd,0x10)=w.lines[k];
    std::memset(w.lines[k],0,sizeof(w.lines[k]));
    for(uint32 i=0;i<numLines && i<12;++i){
        const float x=(i&1) ? ux*0.9f : -ux*0.9f, y=(i&2) ? -uy*0.6f : uy*0.6f, z=c.uni(-0.2f,0.3f);
        if(disks){ auto* d=reinterpret_cast<uint8*>(w.lines[k])+0x24u*i; At<float>(d,0)=x; At<float>(d,4)=y; At<float>(d,8)=z; At<float>(d,12)=c.uni(0.25f,0.45f); }
        else { auto* l=reinterpret_cast<uint8*>(w.lines[k])+0x20u*i; At<float>(l,0)=x; At<float>(l,4)=y; At<float>(l,8)=z; At<float>(l,16)=x; At<float>(l,20)=y; At<float>(l,24)=z-c.uni(0.6f,1.2f); }
    }
}

// Superficies reales (en el rango de CollisionSurfaceType): arena (grupo de adherencia 4) y barro (marca 2) descubiertas una vez
inline std::array<uint8,64> g5Sand{}, g5Mud{}; inline size_t g5NumSand{}, g5NumMud{};
inline uint8 G5Surface(Ctx& c){
    switch(c.below(8)){
        case 0: return uint8(39);                                                  // agua poco profunda
        case 1: return g5NumSand ? g5Sand[c.below(uint32(g5NumSand))] : uint8(28);
        case 2: return g5NumMud ? g5Mud[c.below(uint32(g5NumMud))] : uint8(5);
        case 3: return uint8(1);                                                   // asfalto
        default: return uint8(c.below(120)); }
}

// Globales que leen ProcessControl / ProcessAI (todos se guardan con Save() y se restauran al terminar el lado)
inline void G5Globals(Ctx& c,bool ai){
    AtA<float>(0xB7CB5C)=c.pick<float>({0.5f,0.75f,1.0f,1.0f,1.0f,1.25f,2.0f});          // CTimer::ms_fTimeStep
    AtA<uint32>(0xB7CB84)=c.next();                                                        // CTimer::m_snTimeInMilliseconds
    AtA<uint32>(0xB7CB4C)=c.next();                                                        // CTimer::m_FrameCounter
    AtA<float>(0xC81340)=c.chance(6) ? c.uni(0.05f,2.0f) : 0.0f;                           // CWeather::Earthquake
    AtA<float>(0xC81308)=c.chance(3) ? c.uni(0.05f,0.9f) : 0.0f;                           // CWeather::WetRoads
    AtA<uint8>(0xB7CD6E)=c.chance(3) ? 1 : 0;                                              // CWorld::bForceProcessControl
    AtA<uint8>(0x96914C)=c.chance(6) ? 1 : 0;                                              // CHEAT_PERFECT_HANDLING
    AtA<uint8>(0x969152)=c.chance(8) ? 1 : 0;                                              // CHEAT_CARS_ON_WATER
    AtA<uint8>(0x969165)=c.chance(10) ? 1 : 0;                                             // CHEAT_ALL_CARS_HAVE_NITRO
    AtA<uint8>(0x96918B)=c.chance(8) ? 1 : 0;                                              // CHEAT_ALL_TAXIS_NITRO
    AtA<uint8>(0xC1CC00)=c.chance(4) ? 1 : 0; AtA<uint8>(0xC1CC01)=c.chance(4) ? 1 : 0;   // bDisableRemoteDetonation(+OnContact)
    AtA<uint8>(0xB6EC2E)=c.chance(2) ? 1 : 0;                                              // CCamera::m_bUseMouse3rdPerson
    const uint32 cam=c.below(3);
    AtA<uint8>(0xB6F081)=uint8(cam);                                                       // TheCamera.m_nActiveCam
    for(uint32 q=0;q<3;++q) AtA<uint16>(0xB6F1A8+0x238u*q)=uint16(c.chance(3) ? 16 : c.below(60)); // modo de cada camara (16 = primera persona)
    AtA<float>(0xB6F02C)=c.uni(-200.f,200.f); AtA<float>(0xB6F030)=c.uni(-200.f,200.f); AtA<float>(0xB6F034)=c.uni(0.f,30.f);
    AtA<void*>(0xB6F03C)=c.chance(2) ? static_cast<void*>(w.mat[3]) : nullptr;             // TheCamera.m_matrix
    AtA<float>(0xB73424)=c.chance(2) ? 0.0f : c.uni(-120.f,120.f); AtA<float>(0xB73428)=c.chance(2) ? 0.0f : c.uni(-120.f,120.f);   // raton: movimiento
    AtA<float>(0x8D3214)=c.chance(14) ? 0.0f : 4.0f;                                       // global que activa el bloque de direccion (6B28D6)
    AtA<uint8>(0xB7CD74)=0;                                                                // CWorld::PlayerInFocus
    for(uint32 q=0;q<16;++q) AtA<uint8>(0x97D6C0+q)=c.chance(3) ? 0 : 1;                  // CVehicleRecording::bUseCarAI
    (void)ai;
}

// Rellena la parte comun del coche 0 (fisica, banderas, ocupantes, ruedas...). 'st' = estado de entidad (0..12)
inline void G5FillCar(Ctx& c,uint8* car,uint32 st,int model,uint32 sub,bool forAi){
    auto* v=reinterpret_cast<CAutomobile*>(car);
    auto Fl=[&](size_t o,float x){ At<float>(car,o)=x; };
    auto Vec=[&](size_t o,float x,float y,float z){ At<float>(car,o)=x; At<float>(car,o+4)=y; At<float>(car,o+8)=z; };
    At<uint8>(car,G5o::Status)=uint8((st<<3)|2);
    At<uint32>(car,G5o::Flags)|=c.chance(5) ? 0x1000u : 0u;                                 // bHasHitWall
    At<uint8>(car,G5o::Seed)=uint8(c.next());
    // velocidades
    const uint32 sp=c.below(5);
    const float s0 = sp==0 ? 0.0f : sp==1 ? 0.0005f : sp==2 ? 0.002f : 0.12f;
    Vec(G5o::MoveSpd, c.chance(3)?0.f:c.uni(-s0,s0), c.chance(3)?0.f:c.uni(-s0,s0), c.chance(3)?0.f:c.uni(-s0,s0));
    if(sp==4){   // marcha rapida hacia delante (velocidad hacia delante > 0.15 y modulo > 0.2)
        const float* fwd=reinterpret_cast<const float*>(w.mat[0])+4; const float f=c.uni(-0.1f,0.55f);
        Vec(G5o::MoveSpd, fwd[0]*f+c.uni(-0.02f,0.02f), fwd[1]*f+c.uni(-0.02f,0.02f), fwd[2]*f+c.uni(-0.01f,0.01f)); }
    Vec(G5o::TurnSpd, c.uni(-s0,s0), c.uni(-s0,s0), c.uni(-s0,s0));
    Vec(G5o::FricMove, c.uni(-s0,s0), c.uni(-s0,s0), c.uni(-s0,s0)); Vec(G5o::FricTurn, c.uni(-s0,s0), c.uni(-s0,s0), c.uni(-s0,s0));
    Vec(G5o::Force, c.uni(-s0,s0), c.uni(-s0,s0), c.uni(-s0,s0)); Vec(G5o::Torque, c.uni(-s0,s0), c.uni(-s0,s0), c.uni(-s0,s0));
    Fl(G5o::VelFreq, c.chance(3) ? c.uni(0.8f,1.5f) : 1.0f);
    At<uint8>(car,G5o::Fake)=uint8(c.below(13));
    Fl(G5o::MovingSpeed, c.chance(2) ? c.uni(0.f,0.02f) : c.uni(0.f,0.3f));
    Fl(G5o::DmgInt, c.chance(2) ? 0.0f : c.uni(0.1f,50.f));
    At<void*>(car,G5o::DmgEnt) = c.chance(3) ? nullptr : (c.chance(2) ? static_cast<void*>(Cr(1)) : static_cast<void*>(g5ped[3]));
    Vec(G5o::Impact, c.uni(-1.f,1.f), c.uni(-1.f,1.f), c.chance(2) ? c.uni(0.1001f,2.f) : c.uni(-1.f,0.1f));
    // fisica / banderas
    At<uint32>(car,G5o::Phys)|= c.chance(6) ? 0x100u : 0u;                                  // bSubmergedInWater
    At<uint32>(car,G5o::Phys)|= c.chance(9) ? 0xCu : 0u;                                    // bDisableCollisionForce + bCollidable
    At<uint32>(car,G5o::Phys)&= c.chance(3) ? ~0x8u : ~0x0u;
    // handling del coche (banderas propias y del modelo)
    uint32 hf=At<uint32>(car,G5o::HFlags);
    auto Bit=[&](uint32 m,uint32 n){ if(c.chance(n)) hf|=m; else if(c.chance(3)) hf&=~m; };
    Bit(0x4u,4); Bit(0x8u,5); Bit(0x20u,6); Bit(0x40u,6); Bit(0x10000u,6); Bit(0x20000u,7); Bit(0x80000u,5); Bit(0x800000u,4);
    At<uint32>(car,G5o::HFlags)=hf;
    auto* hd=w.hd[0];
    uint32 mf=At<uint32>(hd,0xCC);
    auto MBit=[&](uint32 m,uint32 n,bool force){ if(force) mf|=m; else if(c.chance(n)) mf|=m; else mf&=~m; };
    MBit(0x2u,12,model==431||model==437||model==420&&c.chance(2));                         // bIsBus
    MBit(0x2000000u,(sub==3)?1:14,sub==3); MBit(0x4000000u,(sub==4)?1:14,sub==4);           // bIsHeli / bIsPlane
    At<uint32>(hd,0xCC)=mf;
    At<uint32>(car,G5o::SubType)=sub;
    // mision del piloto automatico (57/58 = accidente de avion / helicoptero)
    At<uint8>(car,G5o::Mission)= (sub==3||sub==4) ? uint8(c.chance(2) ? (c.chance(2) ? 57 : 58) : c.below(12)) : uint8(c.below(12));
    // ocupantes
    const uint32 dr=c.below(8);                                                             // 0..2: sin conductor
    if(dr>=3){ G5InitPed(0,c.chance(5)?1u:(c.chance(7)?7u:0u),c.chance(4)?uint32(0x3F):c.chance(4)?uint32(0x3D):c.chance(5)?uint32(0x3A):uint32(c.below(60))); At<void*>(car,G5o::Driver)=g5ped[0]; }
    const uint32 np=c.below(3);
    for(uint32 q=0;q<np && q<2;++q){ G5InitPed(int(1+q),c.chance(3)?0u:5u,uint32(c.below(60))); At<void*>(car,G5o::Pass+4u*q)=g5ped[1+q]; }
    At<uint8>(car,G5o::NumPass)=uint8(np); At<uint8>(car,G5o::MaxPass)=uint8(c.chance(4) ? 0 : 2+c.below(3));
    At<void*>(car,G5o::BombOwner)= c.chance(2) ? static_cast<void*>(g5ped[3]) : nullptr;
    At<uint8>(car,G5o::Bomb)=uint8((At<uint8>(car,G5o::Bomb)&~7u)|c.below(6));
    At<uint16>(car,G5o::BombTimer)=uint16(c.below(2000));
    At<uint16>(car,G5o::Alarm)=c.chance(3) ? 0 : c.chance(2) ? uint16(0xFFFF) : uint16(1+c.below(60000));
    At<uint32>(car,G5o::Horn)=c.below(6);
    At<int8>(car,G5o::Nitro)=int8(c.below(120));
    At<int8>(car,G5o::SpecCol)=int8(-1);
    Fl(G5o::Steer, c.chance(4) ? 0.f : c.uni(-0.6f,0.6f)); Fl(G5o::Steer2, c.uni(-0.5f,0.5f));
    Fl(G5o::Gas, c.chance(4) ? 0.f : c.uni(-1.f,1.f)); Fl(G5o::Brake, c.chance(3) ? 0.f : c.uni(0.f,1.f));
    if(c.chance(3)) Fl(G5o::Gas,c.uni(0.41f,1.f)), Fl(G5o::Brake,c.uni(0.f,0.09f));
    {   // marcha valida para el handling (cTransmission recorre las marchas hasta aceptar la aceleracion)
        const uint32 ng=reinterpret_cast<tHandlingData*>(w.hd[0])->GetTransmission().m_nNumberOfGears;
        At<uint8>(car,G5o::Gear)=uint8(c.below((ng<5u ? ng : 5u)+1u)); }
    Fl(G5o::GearCount,c.uni(0.f,1.f)); Fl(G5o::SpinAudio,c.uni(0.f,1.f));
    Fl(G5o::Health, c.chance(3) ? c.uni(50.f,249.f) : c.uni(250.f,1000.f));
    // banderas del vehiculo
    v->vehicleFlags.bUseCarCheats = c.chance(10); v->vehicleFlags.bIsBus = (mf&2u)!=0 && c.chance(2); v->vehicleFlags.bDriverLastFrame = c.chance(2);
    v->vehicleFlags.bVehicleColProcessed = c.chance(3); v->vehicleFlags.bAudioChangingGear = c.chance(2); v->vehicleFlags.bEngineOn = !c.chance(4);
    v->vehicleFlags.bIsHandbrakeOn = c.chance(5); v->vehicleFlags.bSirenOrAlarm = c.chance(3); v->vehicleFlags.bIsLawEnforcer = c.chance(3);
    v->vehicleFlags.bIsBeingCarJacked = c.chance(6); v->vehicleFlags.bCanPark = c.chance(2); v->vehicleFlags.bParking = c.chance(5);
    v->vehicleFlags.bRestingOnPhysical = c.chance(5); v->vehicleFlags.bWarnedPeds = c.chance(2);
    v->autoFlags.bIsMonsterTruck = c.chance(9); v->autoFlags.bIsBoggedDownInSand = c.chance(5);
    At<uint8>(car,G5o::CreatedBy)=uint8(c.chance(4) ? 2 : c.below(5));
    // ruedas
    for(int q=0;q<4;++q){
        const size_t cp=G5o::WheelCol+0x2Cu*unsigned(q);
        Vec(cp, At<float>(car,4)+c.uni(-2.f,2.f), At<float>(car,8)+c.uni(-2.f,2.f), At<float>(car,12)+c.uni(-1.f,0.f));
        At<uint8>(car,cp+0x23)=G5Surface(c);
        Fl(G5o::Comp+4u*unsigned(q), c.chance(4) ? 1.0f : c.uni(0.2f,1.0f));
        Fl(G5o::CompPrev+4u*unsigned(q), c.chance(4) ? 1.0f : c.uni(0.2f,1.0f));
        Fl(G5o::Counts+4u*unsigned(q), c.chance(3) ? 0.f : c.uni(0.f,4.f));
        const float sl=c.uni(0.12f,0.5f); Fl(G5o::SpringLen+4u*unsigned(q),sl); Fl(G5o::LineLen+4u*unsigned(q),sl+c.uni(0.1f,0.4f));
        At<uint32>(car,G5o::WheelState+4u*unsigned(q))=c.below(4);
        Fl(G5o::WheelSpeed+4u*unsigned(q),c.chance(3)?0.f:c.uni(0.f,2.f));
        Fl(G5o::WheelRot+4u*unsigned(q),c.uni(-3.f,3.f));
    }
    Fl(G5o::FrontH,c.uni(0.f,0.5f)); Fl(G5o::CarTraction,c.uni(0.5f,2.f)); Fl(G5o::TireTemp,c.chance(3) ? c.uni(-1.f,0.f) : c.uni(0.f,2.f));
    At<uint8>(car,G5o::Contact)=uint8(c.below(5)); At<uint8>(car,G5o::DriveWheels)=uint8(c.below(5)); At<uint8>(car,G5o::DriveWheelsLast)=uint8(c.below(5));
    At<uint16>(car,G5o::MiscAngle)=c.chance(3) ? 0 : uint16(c.below(2000)); At<uint16>(car,G5o::MiscAnglePrev)=c.chance(2) ? At<uint16>(car,G5o::MiscAngle) : uint16(c.below(2000));
    At<uint32>(car,G5o::BusEnd)=c.chance(3) ? 0 : 1+c.below(2000);
    // plano / helicoptero
    Fl(G5o::PlaneBreak,c.chance(2)?0.f:c.uni(0.f,1.f)); Fl(G5o::HeliBreak,c.chance(2)?0.f:c.uni(0.f,1.f));
    At<uint8>(car,G5o::FireCounter)=uint8(c.chance(2) ? 2 : c.below(5));
    for(int q=0;q<2;++q) At<void*>(car,G5o::NitroFx+4u*unsigned(q)) = c.chance(2) ? static_cast<void*>(w.misc+0x100+0x40*q) : nullptr;
    (void)forAi;
}

// ---------------------------------------------------------------------------------------------------------------------
// Banco de ProcessControl
// ---------------------------------------------------------------------------------------------------------------------
inline void G5RunPC(Ctx& c){
    G5Reset(); G5Globals(c,false);
    // coches auxiliares (remolque, victima, entidad de colision, jugador): el probado se construye el ultimo para que su model info gane
    for(int k=3;k>=1;--k){ c.BuildBase(k,c.carModel()); At<uint8>(Cr(k),G5o::Status)=uint8((c.below(13)<<3)|2); At<uint32>(Cr(k),G5o::Flags)|=c.chance(2) ? 0x20u : 0u;
        for(int q=0;q<3;++q) At<float>(Cr(k),G5o::MoveSpd+4u*unsigned(q))=c.chance(2) ? 0.f : c.uni(-0.3f,0.3f); }
    const uint32 th = uint32(c.i)%32u;                                                      // tema: fuerza combinaciones raras
    const bool sp = c.chance(2) || c.i%5==0;
    int model = sp ? kG5Special[c.below(uint32(std::size(kG5Special)))] : c.carModel();
    if(th==3) model = c.chance(2) ? 420 : 438;                                              // taxi / cabbie
    else if(th==9) model = c.chance(2) ? 431 : 437;                                         // autobuses
    else if(th==15) model = c.pick<int>({460,464,539,447});                                 // skimmer / rcbaron / vortex / helicoptero anfibio
    else if(th==1) model = c.chance(2) ? 407 : 601;                                         // camion de bomberos / SWAT con canon de agua
    else if(th==11) model = c.chance(2) ? 525 : 531;                                        // grua / tractor
    else if(th==7||th==17||th==21||th==27||th==29||th==31||th==5||th==23) model = c.carModel();
    const uint32 sub = (model==447||model==417) ? 3u : (model==460||model==464) ? 4u : (th==25) ? 4u : c.chance(14) ? 3u : c.chance(14) ? 4u : c.chance(14) ? 1u : 0u;
    auto* v=c.BuildBase(0,model,c.chance(4) ? 0.0f : 0.3f); uint8* car=Cr(0);
    G5Handling(c,0,int(c.below(100)));
    const uint32 sw=c.below(100);
    const uint32 st = sw<22 ? 0u : sw<40 ? 3u : sw<58 ? 4u : sw<72 ? 5u : sw<77 ? 2u : sw<83 ? 10u : sw<87 ? 9u : sw<91 ? 8u : sw<95 ? 12u : 1u;
    G5ColModel(c,0,(model==432) ? 8u : 4u, c.chance(8));
    G5FillCar(c,car,st,model,sub,false);
    // la direccion de la rueda 0..3 del helicoptero etc. ya quedo en G5FillCar; ahora, lo especifico de ProcessControl
    // remolque / remolcado
    if(c.chance(5)){ At<void*>(car,G5o::Towing)=Cr(1); At<uint8>(Cr(1),G5o::Status)=uint8(((c.chance(2) ? 10u : 3u)<<3)|2); }
    else if(c.chance(6)){ At<void*>(car,G5o::BeingTowed)=Cr(2); At<uint8>(Cr(2),G5o::Status)=uint8(((c.chance(2) ? 10u : 3u)<<3)|2); At<void*>(Cr(2),G5o::Towing)=c.chance(2) ? static_cast<void*>(car) : static_cast<void*>(Cr(3)); }
    // entidades bajo las ruedas
    for(int q=0;q<4;++q) if(c.chance(12)){ At<void*>(car,G5o::WheelEnt+4u*unsigned(q))=c.chance(2) ? static_cast<void*>(Cr(3)) : static_cast<void*>(Cr(1));
        At<float>(car,G5o::WheelEntPos+12u*unsigned(q))=c.uni(-1.f,1.f); At<float>(car,G5o::WheelEntPos+12u*unsigned(q)+4)=c.uni(-1.f,1.f); At<float>(car,G5o::WheelEntPos+12u*unsigned(q)+8)=c.uni(-1.f,1.f); }
    // hidraulicos: indice de colision especial valido solo si ProcessSuspension deja todas las ruedas con contacto (el modelo de colision especial es del juego)
    for(int q=0;q<4;++q){ AtA<float>(0xC1CB78+4u*unsigned(q))=c.uni(0.f,1.f); AtA<float>(0xC1CB78+0x28+4u*unsigned(q))=c.uni(0.f,1.f);
                           AtA<float>(0xC1CB78+0x50+4u*unsigned(q))=c.uni(0.f,1.f); AtA<float>(0xC1CB78+0x78+4u*unsigned(q))=c.uni(0.f,1.f); }
    if((At<uint32>(car,G5o::HFlags)&0x20000u) && c.chance(2)) At<int8>(car,G5o::SpecCol)=int8(c.below(4));
    // dobles
    g5.pPed=g5ped[3]; At<uint32>(g5ped[3],0x598)=0;
    const uint32 pv=c.below(6); g5.pVeh = pv<3 ? static_cast<void*>(car) : pv==3 ? static_cast<void*>(Cr(1)) : nullptr;
    g5.aiRet=c.chance(10) ? 1 : 0; g5.aiOr=c.chance(4) ? c.pick<uint32>({1u,2u,8u,0x10u,3u,9u,0x1Au}) : 0u;
    g5.upsideDown=c.chance(3); g5.canPedEnter=c.chance(2); g5.usesSiren=c.chance(2); g5.umc=c.chance(3); g5.gunDown=c.chance(3);
    g5.gunLR=c.pick<int32>({0,60,-80,20,120,-30}); g5.gunUD=c.pick<int32>({0,20,-30,70,10});
    g5.buoy=c.below(3);
    g5.suspDo = c.chance(9) ? 0 : 1;
    for(int q=0;q<4;++q) g5.susp[q]=c.chance(4) ? 1.0f : c.uni(0.2f,0.99f);
    if(At<int8>(car,G5o::SpecCol)>=0){ g5.suspDo=1; for(int q=0;q<4;++q) g5.susp[q]=c.uni(0.2f,0.95f); }
    // --- temas (indice de caso % 32): combinaciones raras que el azar casi nunca alcanza; los temas impares dejan seguir a ProcessControl ---
    {
        auto SetSt=[&](uint32 s){ At<uint8>(car,G5o::Status)=uint8((s<<3)|2); };
        auto Fl=[&](size_t o,float x){ At<float>(car,o)=x; };
        auto Vec=[&](size_t o,float x,float y,float z){ Fl(o,x); Fl(o+4,y); Fl(o+8,z); };
        const float* fwd=reinterpret_cast<const float*>(w.mat[0])+4;
        if(th&1u){ g5.aiRet=0; g5.aiOr=0; }
        switch(th){
        case 1:  SetSt(0); break;                                                                    // camion con canon de agua en estado jugador
        case 3:  SetSt(0); AtA<uint8>(0x96918B)=1; Fl(G5o::TireTemp,-0.5f); break;                  // taxis con nitro
        case 5:  AtA<uint8>(0x969152)=1; SetSt(c.chance(2) ? 4u : 5u); Vec(G5o::MoveSpd,0.001f,0.001f,0.001f); Fl(G5o::Gas,0.f); Fl(G5o::DmgInt,0.f);   // coches sobre el agua
                 At<uint32>(car,G5o::Phys)&=~0x10Cu; At<uint32>(car,G5o::SubType)=0; At<uint16>(car,G5o::MiscAngle)=0; At<void*>(car,G5o::Towing)=nullptr;
                 for(int q=0;q<4;++q){ At<uint8>(car,G5o::WheelCol+0x2Cu*unsigned(q)+0x23)=39; Fl(G5o::Comp+4u*unsigned(q),0.5f); Fl(G5o::CompPrev+4u*unsigned(q),0.5f);
                                       At<void*>(car,G5o::WheelEnt+4u*unsigned(q))=nullptr; } break;
        case 7: {                                                                                    // hidraulicos
            At<uint32>(car,G5o::HFlags)|=0x20000u; const bool pl=c.chance(2); SetSt(pl ? 0u : 3u); Vec(G5o::MoveSpd,0.02f,0.f,0.f);
            if(pl){ G5InitPed(0,0u,uint32(c.below(40))); At<void*>(car,G5o::Driver)=g5ped[0]; g5.gunLR=60; g5.gunUD=10; }
            else { for(int q=0;q<4;++q){ AtA<float>(0xC1CB78+4u*unsigned(q))=0.8f; AtA<float>(0xC1CB78+0x28+4u*unsigned(q))=0.8f; }
                   At<int8>(car,G5o::SpecCol)=int8(c.below(4)); g5.suspDo=1; for(int q=0;q<4;++q) g5.susp[q]=c.uni(0.2f,0.95f); }
            break; }
        case 9:  v->vehicleFlags.bIsBus=true; SetSt(c.chance(2) ? 0u : 3u); break;                  // autobus
        case 11: break;                                                                              // grua / tractor
        case 13: SetSt(10); At<void*>(car,G5o::Towing)=Cr(1); At<void*>(car,G5o::BeingTowed)=nullptr; Vec(G5o::MoveSpd,0.f,0.f,0.f);        // remolcado
                 for(int q=0;q<3;++q) At<float>(Cr(1),G5o::MoveSpd+4u*unsigned(q))=c.chance(2) ? 0.f : 0.2f; break;
        case 15: SetSt(c.chance(2) ? 0u : 3u); g5.buoy=1+c.below(2); At<uint32>(car,G5o::Phys)&=~0x100u; break;                          // skimmer / avion / hovercraft
        case 17: SetSt(0); g5.pVeh=car; v->vehicleFlags.bAudioChangingGear=true; Fl(G5o::Gas,0.7f); Fl(G5o::Brake,0.05f);               // fuerza de giro del turbo
                 Vec(G5o::MoveSpd,fwd[0]*0.3f,fwd[1]*0.3f,fwd[2]*0.3f); for(uint32 q=0;q<3;++q) AtA<uint16>(0xB6F1A8+0x238u*q)=uint16(5);
                 At<uint32>(w.hd[0],0xCC)&=~2u; At<uint32>(car,G5o::BusEnd)=c.chance(2) ? 0u : uint32(1+c.below(2000));
                 At<int8>(car,G5o::SpecCol)=int8(-1); g5.suspDo=1; for(int q=0;q<4;++q) g5.susp[q]=c.uni(0.3f,0.9f); break;
        case 19: AtA<float>(0xC81340)=c.uni(0.1f,2.0f); break;                                      // terremoto
        case 21: SetSt(4); v->vehicleFlags.bVehicleColProcessed=false; Vec(G5o::MoveSpd,0.f,0.f,0.f); Fl(G5o::CompPrev+12,0.5f); break;  // abandonado, parado
        case 23: AtA<uint8>(0x969165)=1; SetSt(0); v->vehicleFlags.bEngineOn=true; break;           // todos los coches con nitro
        case 25: SetSt(c.chance(2) ? 5u : 4u); break;                                                // avion/helicoptero abandonado o destruido
        case 27: SetSt(0); Vec(G5o::MoveSpd,fwd[0]*0.4f,fwd[1]*0.4f,fwd[2]*0.4f); AtA<uint32>(0xB7CB84)=(c.next()&~2047u)|c.below(800);    // sacudidas del mando
                 for(int q=0;q<4;++q){ Fl(G5o::CompPrev+4u*unsigned(q),1.0f); g5.susp[q]=c.uni(0.4f,0.6f); }
                 g5.suspDo=1; At<int8>(car,G5o::SpecCol)=int8(-1); break;
        case 29: SetSt(0); v->vehicleFlags.bSirenOrAlarm=true; AtA<uint32>(0xB7CB4C)=(c.next()&~7u)|5u; g5.pVeh=car; g5.usesSiren=1; break;     // sirena
        case 31: SetSt(0); At<uint32>(car,G5o::HFlags)&=~0x80000u; AtA<uint8>(0x969165)=0; AtA<uint8>(0x96918B)=0; v->vehicleFlags.bUseCarCheats=false;  // parar el nitro
                 At<void*>(car,G5o::NitroFx)=w.misc+0x100; At<void*>(car,G5o::NitroFx+4)=w.misc+0x140; break;
        default: break;
        }
    }
    G5SetVirtuals();
    { auto P=MakePatches(G_RNG,5,G5_ALL); CallThis<void>(c.a,car); }
    c.ret=nullptr;
    c.out["g5"]=G5Fnv();
    c.out["status"]=At<uint8>(car,G5o::Status);
    (void)v;
}
}
