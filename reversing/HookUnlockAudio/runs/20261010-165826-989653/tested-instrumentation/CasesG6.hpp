#pragma once
// Grupo 6 (las tres funciones mas grandes): ProcessPlayerVehicleEngine (0x4FBB10), ProcessVehicle (0x501E10) y Service (0x502280).
// Los tres hooks estan { .locked = true } en la fuente: el banco hace LockState(false)/State(false) en el lado original.
//
// Falsos propios (todos respetan thiscall y el 'ret N' de cada original; solo actuan en el hilo del banco):
//   G_X6 (ProcessPlayerVehicleEngine): GetAccelAndBrake 0x4F5080 (ret 4; fija ThisAccel/ThisBrake del caso), GetVolForPlayerEngineSound 0x4F5D00 (ret 8,
//        float en ST0), GetFreqForPlayerEngineSound 0x4F8070 (ret 8), RequestNewPlayerCarEngineSound 0x4F7A50 (ret 0xC), AddAudioEvent-0 0x4F6420 (ret 8),
//        CCullZones::DoExtraAirResistanceForPlayer 0x72DDD0 (cdecl). Quedan REALES: CancelVehicleEngineSound (el EXE inlinea parte), IsAccInhibited*,
//        JustFinishedAccelerationLoop, Inhibit*ForTime, UpdateVehicleEngineSound (0x4F56D0 sin hook: se compara contra la DLL), CAESound::* y los falsos G_STD.
//   G_X7 (ProcessVehicle): las 21 hermanas Process*/UpdateGasPedalAudio con traza (argumentos + campos de tVehicleParams sin relleno ni los campos que
//        el EXE deja sin inicializar: ZOverSpeed, NumGears, NumDriveWheels*, WheelState).
//   G_X8 (Service): FindVehicleOfPlayer 0x4D9E10, PlayerAboutToExit 0x4F5BA0, JustWrecked 0x4FD0B0, JustGotIn 0x4F5700, JustGotOut 0x4FCF40,
//        TurnOn 0x4F5B20, TurnOff 0x4F5B60 y ProcessVehicle 0x501E10 (CPed::IsPlayer/IsAlive quedan reales sobre peatones falsos).
//   G_X7 y G_X8 no los usa ningun otro grupo (los grupos 1..5 usan G_X1..G_X5).
// Tipos de traza propios: T_GROUP+96 .. T_GROUP+106.
#include "Ctx.hpp"
#include "CullZones.h"

namespace HookUnlockAudioLive {
constexpr uint32 T6_VOL=T_GROUP+96u, T6_FRQ=T_GROUP+97u, T6_REQ=T_GROUP+98u, T6_EVENT=T_GROUP+99u, T6_ACCEL=T_GROUP+100u, T6_AIR=T_GROUP+101u,
                 T6_SISTER=T_GROUP+102u, T6_SVC=T_GROUP+103u, T6_FINDVEH=T_GROUP+104u, T6_GAS=T_GROUP+105u, T6_PV=T_GROUP+106u;

struct G6State {
    int16  accel{}, brake{};
    uint32 airBits{}, airIdx{}, volIdx{}, frqIdx{};
    void*  playerVeh{};
    uint32 mutKind{};
};
inline G6State g6{};

// Hash de los campos reales de tVehicleParams (sin relleno). full=false: solo los que ProcessVehicle inicializa en el EXE
// (ZOverSpeed 0x28, NumGears 0x44, NumDriveWheelsOnGround 0x46/0x47 y WheelState 0x48 quedan sin inicializar en el EXE).
inline uint32 G6VpHash(const void* v,bool full){
    uint64 h=1469598103934665603ull;
    auto add=[&](size_t o,size_t n){ h=Fnv64(static_cast<const uint8*>(v)+o,n,h); };
    add(0,4); add(4,4); add(8,1); add(0xC,4); add(0x10,4); add(0x14,4); add(0x18,4); add(0x1C,4); add(0x20,2); add(0x22,2); add(0x24,4);
    add(0x2C,4); add(0x30,4); add(0x34,4); add(0x38,2); add(0x3C,4); add(0x40,4);
    if(full){ add(0x28,4); add(0x44,2); add(0x46,2); add(0x48,4); }
    return uint32(h^(h>>32));
}
inline std::string G6VpDump(const void* v){
    std::string s; auto add=[&](size_t o,size_t n){ s+=Hex(static_cast<const uint8*>(v)+o,n); s+='|'; };
    add(0,4); add(4,4); add(8,1); add(0xC,4); add(0x10,4); add(0x14,4); add(0x18,4); add(0x1C,4); add(0x20,2); add(0x22,2); add(0x24,4);
    add(0x28,4); add(0x2C,4); add(0x30,4); add(0x34,4); add(0x38,2); add(0x3C,4); add(0x40,4); add(0x44,2); add(0x46,2); add(0x48,4);
    return s;
}

// ---------------------------------------------------------------------------------------------------------------------
// Falsos de ProcessPlayerVehicleEngine
// ---------------------------------------------------------------------------------------------------------------------
// GetAccelAndBrake(tVehicleParams&) 0x4F5080 (ret 4)
inline void __fastcall G6_Accel(void* self,void*,void* vp){
    if(!Mine())return;
    Push(T6_ACCEL,Id(self),G6VpHash(vp,false));
    At<int16>(vp,0x20)=g6.accel; At<int16>(vp,0x22)=g6.brake;
}
// GetVolForPlayerEngineSound(tVehicleParams&, eVehicleEngineSoundType) 0x4F5D00 (ret 8, float en ST0)
inline float __fastcall G6_VolBody(void* self,void*,void* vp,int16 st){
    if(!Mine())return 0.f;
    const uint32 h=G6VpHash(vp,true), k=g6.volIdx++, s=uint32(uint16(st));
    Push(T6_VOL,Id(self),s,h,k);
    return Fl(0x3E800000u+((h^(k*0x9E3779B1u)^(s*0x85EBCA6Bu))&0x3FFFFFu));
}
// Como el original (0x4F5D00): conserva ECX. ProcessPlayerVehicleEngine vuelve a usarlo sin recargarlo
// entre call 0x4F5D00 en 0x4FC9C1 y call 0x4F8070 en 0x4FC9CD (tambien 0x4FC903/0x4FC90F).
inline __declspec(naked) void G6_Vol(){
    __asm {
        push ecx
        push dword ptr [esp+12]
        push dword ptr [esp+12]
        call G6_VolBody
        pop ecx
        ret 8
    }
}
// GetFreqForPlayerEngineSound(tVehicleParams&, eVehicleEngineSoundType) 0x4F8070 (ret 8, float en ST0)
inline float __fastcall G6_Frq(void* self,void*,void* vp,int16 st){
    if(!Mine())return 0.f;
    const uint32 h=G6VpHash(vp,true), k=g6.frqIdx++, s=uint32(uint16(st));
    Push(T6_FRQ,Id(self),s,h,k);
    return Fl(0x3F000000u+((h^(k*0x7FEB352Du)^(s*0x846CA68Bu))&0x3FFFFFu));
}
// RequestNewPlayerCarEngineSound(st, speed, volume) 0x4F7A50 (ret 0xC)
inline void __fastcall G6_Req(void* self,void*,int16 st,float frq,float vol){
    if(!Mine())return;
    const uint32 s=uint32(uint16(st));
    const void* cur= s<12u ? static_cast<const void*>(static_cast<CAEVehicleAudioEntity*>(self)->m_EngineSounds[s].Sound) : nullptr;
    Push(T6_REQ,s,F(frq),F(vol),Id(cur));
}
// AddAudioEvent(eAudioEvents, float) 0x4F6420 (ret 8)
inline void __fastcall G6_Event(void* self,void*,int32 ev,float v){
    if(!Mine())return;
    Push(T6_EVENT,Id(self),uint32(ev),F(v));
}
// CCullZones::DoExtraAirResistanceForPlayer() 0x72DDD0 (cdecl, bool en AL)
inline bool __cdecl G6_Air(){
    if(!Mine())return false;
    const uint32 k=g6.airIdx++; const bool r=((g6.airBits>>(k&7u))&1u)!=0;
    Push(T6_AIR,k,r?1u:0u); return r;
}

// ---------------------------------------------------------------------------------------------------------------------
// Falsos de ProcessVehicle (hermanas: thiscall con tVehicleParams&, ret 4)
// ---------------------------------------------------------------------------------------------------------------------
template<uint32 K> inline void __fastcall G6_Sis(void* self,void*,void* vp){
    if(!Mine())return;
    Push(T6_SISTER,K,Id(self),G6VpHash(vp,false),Id(AtC<void*>(vp,0x10)));
    if(g6.mutKind==K)At<float>(vp,0x1C)+=0.0625f;   // una hermana modifica vp.Speed: m_PrevSpeed debe guardar la velocidad final
}
// UpdateGasPedalAudio(CVehicle*, int32) 0x4F5EB0 (ret 8)
inline void __fastcall G6_Gas(void* self,void*,void* veh,int32 type){
    if(!Mine())return;
    Push(T6_GAS,Id(self),Id(veh),uint32(type));
}

// ---------------------------------------------------------------------------------------------------------------------
// Falsos de Service
// ---------------------------------------------------------------------------------------------------------------------
inline uint32 G6Flags(const void* ent){
    return uint32(AtC<uint8>(ent,0xA5))|(uint32(AtC<uint8>(ent,0xA6))<<8)|(uint32(AtC<uint8>(ent,0xA7))<<16)|(uint32(AtC<uint8>(ent,0xA8))<<24);
}
template<uint32 K> inline void __fastcall G6_Svc(void* self,void*){
    if(!Mine())return;
    Push(T6_SVC,K,Id(self),G6Flags(self));
}
// ProcessVehicle(CPhysical*) 0x501E10 (ret 4), llamado desde Service
inline void __fastcall G6_ProcVeh(void* self,void*,void* phys){
    if(!Mine())return;
    Push(T6_PV,Id(self),Id(phys),G6Flags(self));
}
// CAEAudioUtility::FindVehicleOfPlayer() 0x4D9E10 (cdecl)
inline void* __cdecl G6_FindVeh(){
    if(!Mine())return nullptr;
    Push(T6_FINDVEH,Id(g6.playerVeh)); return g6.playerVeh;
}

// ---------------------------------------------------------------------------------------------------------------------
// Tabla de funciones (formato exacto para run_local.py)
// ---------------------------------------------------------------------------------------------------------------------
inline const std::vector<Fn> kFnsG6{
    {"CAEVehicleAudioEntity","ProcessPlayerVehicleEngine",0x4FBB10,0,-1,1200},
    {"CAEVehicleAudioEntity","ProcessVehicle",0x501E10,0,-1,640},
    {"CAEVehicleAudioEntity","Service",0x502280,0,-1,640},
};

inline void SavesG6(){}   // sin globales propios: s_pPlayerDriver (0xB6B990) y el reloj ya estan en las instantaneas del banco

inline void StubsG6(){
    constexpr const char* K="CAEVehicleAudioEntity";
    // --- ProcessPlayerVehicleEngine
    AddHookStub(G_X6,0x4F5080,K,"GetAccelAndBrake",FnPtr(&G6_Accel));
    AddHookStub(G_X6,0x4F5D00,K,"GetVolForPlayerEngineSound",FnPtr(&G6_Vol));
    // El hook conserva ECX mediante un wrapper distinto del miembro que llama directamente la fuente.
    AddStub(G_X6,0,bit_cast<uintptr_t>(&CAEVehicleAudioEntity::GetVolForPlayerEngineSound),FnPtr(&G6_Vol));
    AddHookStub(G_X6,0x4F8070,K,"GetFreqForPlayerEngineSound",FnPtr(&G6_Frq));
    AddHookStub(G_X6,0x4F7A50,K,"RequestNewPlayerCarEngineSound",FnPtr(&G6_Req));
    AddHookStub(G_X6,0x4F6420,K,"AddAudioEvent-0",FnPtr(&G6_Event));
    AddStub(G_X6,0x72DDD0,bit_cast<uintptr_t>(&CCullZones::DoExtraAirResistanceForPlayer),FnPtr(&G6_Air));
    // --- ProcessVehicle: hermanas
    struct Sis { uintptr_t exe; const char* name; void* fn; };
    const Sis sis[]{
        {0x4F8B00,"ProcessVehicleRoadNoise",FnPtr(&G6_Sis<10>)},     {0x4F8DF0,"ProcessReverseGear",FnPtr(&G6_Sis<11>)},
        {0x4F8F10,"ProcessVehicleSkidding",FnPtr(&G6_Sis<12>)},      {0x4F8940,"ProcessVehicleFlatTyre",FnPtr(&G6_Sis<13>)},
        {0x5002C0,"ProcessVehicleSirenAlarmHorn",FnPtr(&G6_Sis<14>)},{0x4FBB10,"ProcessPlayerVehicleEngine",FnPtr(&G6_Sis<15>)},
        {0x501480,"ProcessDummyVehicleEngine",FnPtr(&G6_Sis<16>)},   {0x4FAE20,"ProcessEngineDamage",FnPtr(&G6_Sis<17>)},
        {0x4F92C0,"ProcessRainOnVehicle",FnPtr(&G6_Sis<18>)},        {0x4FB070,"ProcessNitro",FnPtr(&G6_Sis<19>)},
        {0x4FB260,"ProcessMovingParts",FnPtr(&G6_Sis<20>)},          {0x500CE0,"ProcessPlayerCombine",FnPtr(&G6_Sis<21>)},
        {0x5003A0,"ProcessBoatEngine",FnPtr(&G6_Sis<22>)},           {0x4FA0C0,"ProcessBoatMovingOverWater",FnPtr(&G6_Sis<23>)},
        {0x501C50,"ProcessAircraft",FnPtr(&G6_Sis<24>)},             {0x500040,"ProcessPlayerBicycle",FnPtr(&G6_Sis<25>)},
        {0x4FFDC0,"ProcessDummyBicycle",FnPtr(&G6_Sis<26>)},         {0x4FA3F0,"ProcessTrainTrackSound",FnPtr(&G6_Sis<27>)},
        {0x500710,"ProcessDummyTrainEngine",FnPtr(&G6_Sis<28>)},     {0x500AB0,"ProcessPlayerTrainBrakes",FnPtr(&G6_Sis<29>)},
        {0x501AB0,"ProcessSpecialVehicle",FnPtr(&G6_Sis<30>)},
    };
    for(const auto& s:sis)AddHookStub(G_X7,s.exe,K,s.name,s.fn);
    AddHookStub(G_X7,0x4F5EB0,K,"UpdateGasPedalAudio",FnPtr(&G6_Gas));
    // --- Service
    AddStub(G_X8,0x4D9E10,bit_cast<uintptr_t>(&CAEAudioUtility::FindVehicleOfPlayer),FnPtr(&G6_FindVeh));
    AddHookStub(G_X8,0x4F5BA0,K,"PlayerAboutToExitVehicleAsDriver",FnPtr(&G6_Svc<1>));
    AddHookStub(G_X8,0x4FD0B0,K,"JustWreckedVehicle",FnPtr(&G6_Svc<2>));
    AddHookStub(G_X8,0x4F5700,K,"JustGotInVehicleAsDriver",FnPtr(&G6_Svc<3>));
    AddHookStub(G_X8,0x4FCF40,K,"JustGotOutOfVehicleAsDriver",FnPtr(&G6_Svc<4>));
    AddHookStub(G_X8,0x4F5B20,K,"TurnOnRadioForVehicle",FnPtr(&G6_Svc<5>));
    AddHookStub(G_X8,0x4F5B60,K,"TurnOffRadioForVehicle",FnPtr(&G6_Svc<6>));
    AddHookStub(G_X8,0x501E10,K,"ProcessVehicle",FnPtr(&G6_ProcVeh));
}

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------------------------------------------------
inline float G6Nan(){ return std::bit_cast<float>(0x7FC00000u); }

// Vehiculo + transmision + vp + entidad para ProcessPlayerVehicleEngine (el dominio real: marchas 0..6, estado 0..9, tipo base 0/9 y algun otro)
inline void G6PlayerEngineFixture(Ctx& c){
    uint8* const ev=c.V(0); uint8* const vp=w.params; uint8* const tr=w.tr[0];
    const uint32 now=c.SetTime(); c.FreshPool(); c.FillEntity(0,0);
    auto* const e=c.Ent(0);
    c.fill(ev,0xA00);   // basura aleatoria: una lectura en un desplazamiento equivocado de la DLL se detecta

    // --- tipo base
    const bool happy=c.chance(2);   // "camino feliz": ninguna inhibicion de aceleracion (banco cargado, sin freno/freno de mano, sin patinaje, ruedas motrices en suelo)
    const uint32 kt=c.next()%20;
    const int32 base= kt<11 ? 0 : (kt<18 ? 9 : c.pick<int32>({1,2,3,4,5,6,7,8,10}));
    At<int32>(ev,0x590)=base; At<int32>(ev,0x594)=c.pick<int32>({0,9,5});

    // --- banderas del vehiculo (bEngineOn bit 4, bIsHandbrakeOn bit 5, bAudioChangingGear bit 29 de la palabra en 0x428)
    uint32 fl=c.next(); fl&=~((1u<<4)|(1u<<5)|(1u<<29));
    if(happy||!c.chance(12))fl|=1u<<4;
    if(!happy&&c.chance(10))fl|=1u<<5;
    if(c.chance(3))fl|=1u<<29;
    At<uint32>(ev,0x428)=fl;

    // --- transmision
    const int nGears= c.chance(4) ? 1 : (c.chance(14) ? 0 : 1+int(c.next()%6));
    tr[0x4A]=uint8(nGears);
    float vmax; { const uint32 k=c.next()%32; vmax= k==0 ? 0.f : (k==1 ? -c.uni(0.2f,2.5f) : c.uni(0.2f,2.5f)); }
    At<float>(tr,0x58)=vmax;
    for(int g=0;g<6;++g){
        const float top=std::fabs(vmax==0.f ? 1.f : vmax)*float(g+1)/6.f;
        const float down= c.chance(14) ? top : (g ? std::fabs(vmax==0.f ? 1.f : vmax)*float(g)/6.f*c.uni(0.5f,1.f) : 0.f);
        At<float>(tr,12*g)=top; At<float>(tr,12*g+4)=top*0.9f; At<float>(tr,12*g+8)=down;
    }

    // --- velocidad y movimiento
    float speed;
    if(happy&&!c.chance(8))speed=c.uni(0.15f,2.2f);
    else switch(c.next()%16){
        case 0:  speed=0.f; break;
        case 1:  speed=-0.f; break;
        case 2:  speed=-0.001f; break;
        case 3:  speed=std::nextafterf(-0.001f,-1.f); break;
        case 4:  speed=std::nextafterf(-0.001f,0.f); break;
        case 5:  speed=c.uni(0.f,0.12f); break;
        case 6:  speed=0.1f; break;
        case 7:  speed=c.uni(-1.5f,-0.0011f); break;
        case 8:  speed= c.chance(4) ? G6Nan() : c.uni(0.1f,2.f); break;
        default: speed=c.uni(0.1f,2.2f); break;
    }
    static const float deltas[]{ -0.001f, 0.f, 0.001f, 0.0015f, 0.05f, -0.05f, 0.01f, -0.01f, 0.1f, -0.2f, 0.0005f, 0.002f, 0.049f, 0.051f };
    const float delta= c.chance(4) ? c.uni(-0.3f,0.3f) : deltas[c.next()%std::size(deltas)];
    const float prevA=speed-delta, prevB=speed+c.uni(-0.2f,0.2f);   // PrevSpeed del automovil / de la moto (offsets distintos)
    static const float zr[]{ 0.f, 0.1f, 0.2f, 0.2001f, 0.21f, 0.5f, -0.3f, 1.5f, 0.19999f };
    At<float>(ev,0x44)=c.uni(-1.f,1.f); At<float>(ev,0x48)=c.uni(-1.f,1.f);
    At<float>(ev,0x4C)= (speed==0.f||speed!=speed) ? c.uni(-0.5f,0.5f) : speed*zr[c.next()%std::size(zr)];
    { const uint32 k= happy ? c.next()%5u : c.next()%10u; At<float>(ev,0x4BC)= k<5 ? c.uni(0.f,4.9f) : (k==5 ? 5.f : (k==6 ? std::nextafterf(5.f,10.f) : (k==7 ? c.uni(5.f,30.f) : 0.f))); }
    ev[0x4B4]=uint8((nGears>0 && !c.chance(3)) ? c.next()%uint32(nGears+1) : c.next()%7u);

    // --- campos de automovil (0x961,0x962,0x964,0x968,0x8B0) y de moto (0x805,0x806,0x808,0x80C,0x7B8)
    ev[0x961]=uint8((!happy&&c.chance(8)) ? 0u : 1u+c.next()%4u); ev[0x962]=uint8(c.next()%5u);
    ev[0x805]=uint8((!happy&&c.chance(8)) ? 0u : 1u+c.next()%3u); ev[0x806]=uint8(c.next()%4u);
    At<float>(ev,0x8B0)=prevA; At<float>(ev,0x7B8)=prevB;

    // --- tVehicleParams (relleno aleatorio: lo que la funcion no escribe es identico en ambos lados)
    std::memset(vp,0,0x60); c.fill(vp,0x4C);
    At<int32>(vp,0)=c.pick<int32>({-1,0,1,9}); At<int32>(vp,4)=base; vp[8]=0; At<float>(vp,0xC)=0.f;
    At<void*>(vp,0x10)=ev; At<void*>(vp,0x14)=tr; At<uint32>(vp,0x18)=400u+c.next()%212u; At<float>(vp,0x1C)=speed;

    // --- entidad de audio
    { const uint32 s=c.next()%100; int st;
      if(s<10)st=0; else if(s<28)st=3; else if(s<42)st=4; else if(s<58)st=5; else if(s<80)st=6; else st=c.pick<int>({1,2,7,8,9});
      e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(st); }
    e->m_AuGear=uint8(c.next()%7);
    { const uint32 k=c.next()%8; e->m_CrzCount= k==0 ? 0.f : (k==1 ? 150.f : (k==2 ? 149.5f : (k==3 ? (c.chance(2) ? G6Nan() : 0.25f) : (k==4 ? 0.2f : c.uni(0.f,200.f))))); }
    e->m_IsSingleGear=c.chance(6);
    e->m_FramesAgoACLooped=int16(c.pick<int>({0,3,9,10,11,15}));
    e->m_ACPlayPositionThisFrame=int16(c.chance(8) ? -1 : int(c.next()%2200));
    e->m_ACPlayPositionLastFrame=int16(c.chance(8) ? -1 : int(c.next()%2200));
    for(size_t s=0;s<e->m_EngineSounds.size();++s){
        const bool on= s==4 ? !c.chance(4) : c.chance(2);
        e->m_EngineSounds[s].Sound= on ? c.Pre(s) : nullptr;
        if(on&&c.chance(5))At<void*>(c.Pre(s),offsetof(CAESound,m_PhysicalEntity))=ev;
    }
    { auto* ac=c.Pre(4); const int p=e->m_ACPlayPositionThisFrame; const uint32 k=c.next()%6;
      ac->m_Length= k==0 ? int16(-1) : (k==1 ? int16(p+119) : (k==2 ? int16(p+120) : (k==3 ? int16(p+121) : int16(p+int(c.next()%600)-200)))); }
    static const int32 offs[]{ -2000,-1,0,1,2000 };
    e->m_TimeBeforeAllowAccelerate=now+uint32(offs[c.next()%5]);
    e->m_TimeBeforeAllowCruise=now+uint32(offs[c.next()%5]);

    // --- falsos
    { const uint32 m=c.next()%16; fk.bankMode= (happy||m<11) ? 0 : (m<13 ? 1 : 2); }
    g6=G6State{};
    { const uint32 k=c.next()%10; int a;
      switch(k){ case 0: a=0; break; case 1: a=1+int(c.next()%148); break; case 2: a=149; break; case 3: a=150; break; case 4: a=151; break;
                 case 5: a=152+int(c.next()%104); break; case 6: a=255; break; case 7: a=int(c.next()%256); break; case 8: a=200; break; default: a=0; break; }
      if(c.chance(40))a=-int(1+c.next()%50);
      g6.accel=int16(a); }
    g6.brake= (!happy&&c.chance(6)) ? int16(1+c.next()%255) : int16(0);
    g6.airBits=c.next();
}

inline bool RunG6(Ctx& c){
    const std::string& nm=c.nm; auto& out=c.out; uint8* const E=c.E(0); uint8* const ev=c.V(0);

    // ===================================================================================================================
    if(nm=="ProcessPlayerVehicleEngine"){
        G6PlayerEngineFixture(c);
        Write({{"event","probe_stage"},{"function",c.nm},{"case",c.i},{"candidate",c.candidate},{"stage","before_call"},
               {"state",int(c.Ent()->m_State)},{"gear",c.Ent()->m_AuGear},{"base",At<int32>(ev,0x590)}});
        { auto P=MakePatches(G_STD|G_X6); CallThis<void>(c.a,E,w.params); }
        Write({{"event","probe_stage"},{"function",c.nm},{"case",c.i},{"candidate",c.candidate},{"stage","after_call"}});
        out["ent"]=Hex(E,TEMP_SOUND_OFF)+"|"+Hex(E+TEMP_SOUND_END,ENT_SIZE-TEMP_SOUND_END);
        out["veh"]=Hex(ev+0x428,8);
        out["vp"]=G6VpDump(w.params);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessVehicle"){
        c.SetTime(); c.FreshPool(); c.FillEntity(0,0);
        auto* const e=c.Ent(0);
        c.fill(ev,0xA00);
        // tipo de audio del vehiculo: 0..10 y valores fuera de rango (el EXE sale con 'ja' sin signo; negativo incluido)
        { const uint32 k=c.next()%16; const int t= k<11 ? int(k) : c.pick<int>({-1,11,12,127,-128,9,0,1});
          e->m_AuSettings.VehicleAudioType=static_cast<eAEVehicleSoundType>(t); }
        e->m_IsPlayerDriver=c.chance(2); e->m_IsWreckedVehicle=c.chance(5);
        // estado: SIMPLE (2) 30% -> velocidad = m_speed * 0.02f; los demas -> producto escalar con la matriz
        const uint32 stat= c.chance(3) ? 2u : c.pick<uint32>({0u,1u,3u,3u,4u,5u,6u,7u});
        ev[0x36]=uint8((stat<<3)|2u);
        At<int32>(ev,0x590)=c.pick<int32>({0,0,0,9,9,5,1,3,2,4,8,10,-1});
        At<int32>(ev,0x594)=c.pick<int32>({0,9,5,1,3,-1});
        At<void*>(ev,0x384)= c.chance(6) ? nullptr : static_cast<void*>(w.misc);
        At<void*>(ev,0x14)=w.mat[0];
        At<float>(w.mat[0],0x10)=c.uni(-1.f,1.f); At<float>(w.mat[0],0x14)=c.uni(-1.f,1.f); At<float>(w.mat[0],0x18)=c.uni(-1.f,1.f);
        { const uint32 k=c.next()%10; At<int16>(ev,0x22)= int16(k<7 ? 400+int(c.next()%212) : (k<9 ? 532 : int(c.next()%2000))); }
        At<float>(ev,0x3C8)= c.chance(8) ? 0.f : c.uni(-60.f,60.f);
        At<float>(ev,0x44)=c.uni(-2.f,2.f); At<float>(ev,0x48)=c.uni(-2.f,2.f); At<float>(ev,0x4C)=c.uni(-2.f,2.f);
        g6=G6State{};
        g6.mutKind= c.chance(3) ? 0u : 10u+c.next()%21u;
        g6.playerVeh=nullptr;
        { auto P=MakePatches(G_X7); CallThis<void>(c.a,E,static_cast<void*>(ev)); }
        out["prev"]=Hex(ev+0x7B8,4)+"|"+Hex(ev+0x8B0,4);
        out["ent"]=Hex(E+0xA4,8);
        return true;
    }

    // ===================================================================================================================
    if(nm=="Service"){
        c.SetTime(); c.FreshPool(); c.FillEntity(0,0);
        auto* const e=c.Ent(0);
        c.fill(ev,0x800);
        uint8* const drv=w.ped[0]; uint8* const pl=w.ped[1];
        c.fill(drv,0x700); c.fill(pl,0x700);
        const uint32 b= c.i<128 ? uint32(c.i) : c.next();   // los 128 primeros casos recorren todas las combinaciones de 7 bits
        e->m_IsInitialized=!c.chance(10);
        e->m_IsPlayerDriver=(b&1u)!=0; e->m_IsPlayerPassenger=(b&2u)!=0; e->m_IsPlayerDriverAboutToExit=(b&4u)!=0; e->m_IsWreckedVehicle=(b&8u)!=0;
        const bool wrecked=(b&16u)!=0;
        ev[0x36]=uint8(((wrecked ? 5u : c.pick<uint32>({0u,1u,2u,3u,4u,6u,7u}))<<3)|2u);
        // conductor del vehiculo (+0x460) y su tipo (+0x598): 0 sin conductor, 1 no jugador, 2/3 jugador 1/2
        const uint32 dk=c.next()%4;
        At<void*>(ev,0x460)= dk==0 ? nullptr : static_cast<void*>(drv);
        At<int32>(drv,0x598)= dk==1 ? int32(2+c.next()%20) : (dk==2 ? 0 : (dk==3 ? 1 : int32(c.next()%24)));
        // jugador asociado (s_pPlayerDriver 0xB6B990) y su estado (+0x530): 0 nulo, 1 vivo, 2 arrestado (0x3F), 3 muerto (0x36/0x37)
        const uint32 sk=(b>>5)&3u;
        uint8* const spd= c.chance(2) ? drv : pl;
        CAEVehicleAudioEntity::s_pPlayerDriver= sk==0 ? nullptr : reinterpret_cast<CPed*>(spd);
        At<int32>(spd,0x530)= sk==2 ? 0x3F : (sk==3 ? int32(c.chance(2) ? 0x36 : 0x37) : c.pick<int32>({0,1,0x32,0x35,0x38,0x3E,0x40}));
        g6=G6State{};
        { const uint32 k=c.next()%4; g6.playerVeh= k<2 ? static_cast<void*>(ev) : (k==2 ? static_cast<void*>(w.veh[1]) : nullptr); }
        { auto P=MakePatches(G_X8); CallThis<void>(c.a,E); }
        out["flags"]=Hex(E+0xA4,6);
        out["spd"]=Id(reinterpret_cast<const void*>(CAEVehicleAudioEntity::s_pPlayerDriver));
        return true;
    }
    return false;
}
}
