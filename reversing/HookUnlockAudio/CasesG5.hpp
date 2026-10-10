#pragma once
// Grupo 5 del banco VehAudioFinal4 (CAEVehicleAudioEntity): barco, tren, bicicleta y vehiculos especiales (14 funciones).
//   UpdateBoatSound 0x4F9E90, ProcessBoatEngine 0x5003A0, ProcessBoatMovingOverWater 0x4FA0C0, UpdateTrainSound 0x4FA1C0,
//   ProcessTrainTrackSound 0x4FA3F0, ProcessDummyTrainEngine 0x500710, ProcessPlayerTrainBrakes 0x500AB0, PlayTrainBrakeSound 0x4FA630,
//   PlayBicycleSound 0x4F9710, GetBaseVolumeForBicycleTyre 0x4F60B0, ProcessDummyRCCar 0x500DC0, ProcessPlayerCombine 0x500CE0,
//   ProcessDummyGolfCart 0x501270, ProcessSpecialVehicle 0x501AB0.
//
// Falsos propios (todos respetan thiscall y el 'ret N' de cada original; solo actuan en el hilo del banco):
//   G_X5  : GetAccelAndBrake 0x4F5080 (ret 4; fija ThisAccel/ThisBrake), CAETwinLoopSoundEntity::Initialise 0x4F28A0 (ret 0x20),
//           UpdateTwinLoopSound 0x4F29A0 (ret 0x14), StopSoundAndForget 0x4F2960 (sin argumentos), PlayTwinLoopSound 0x4F2CB0 (ret 0x20).
//   G_X10 : PlayTrainBrakeSound 0x4FA630 (ret 0xC) -> solo para ProcessPlayerTrainBrakes (en su propio banco la funcion es REAL).
//   G_X11 : las hermanas que ProcessSpecialVehicle delega fuera de este grupo: ProcessVehicleRoadNoise 0x4F8B00, ProcessVehicleSkidding 0x4F8F10,
//           ProcessVehicleFlatTyre 0x4F8940, ProcessVehicleSirenAlarmHorn 0x5002C0, ProcessRainOnVehicle 0x4F92C0, ProcessEngineDamage 0x4FAE20,
//           ProcessDummyRCPlane 0x4FA7C0, ProcessDummyRCHeli 0x4FAA80. Quedan REALES ProcessDummyRCCar, ProcessDummyGolfCart y ProcessDummyHovercraft
//           (esta ultima ya validada en una tanda anterior: aqui se vuelve a comprobar contra el EXE).
//   (G_X10 y G_X11 son mascaras secundarias de este grupo; los demas grupos usan G_X1..G_X4, G_X6..G_X8.)
// Tipos de traza propios: T_GROUP+80 .. T_GROUP+86.
// Supuestos documentados:
//  - m_tempSound (offsets 8..0x7C de la entidad) no se compara. El resto de la entidad se compara byte a byte (campo "ent") y por hash del mundo.
//  - ProcessPlayerTrainBrakes: solo se prueba con el tren en estado STATUS_PLAYER (con otro estado el EXE deja un local sin inicializar, 0x500B87).
//  - Los dummies que llaman a RequestBankSlot usan el RequestBankSlot REAL sobre los estaticos 0xB6B998/0xB6B9A0 (guardados y restaurados por el banco).
//  - La velocidad del tren, la del barco y demas floats se generan finitos (NaN solo en los puntos donde la fuente lo trata de forma explicita).
#include "Ctx.hpp"

namespace HookUnlockAudioLive {
constexpr uint32 T5_ACB=T_GROUP+80u, T5_TLINIT=T_GROUP+81u, T5_TLUPD=T_GROUP+82u, T5_TLSTOP=T_GROUP+83u, T5_TLPLAY=T_GROUP+84u,
                 T5_BRAKE=T_GROUP+85u, T5_SIS=T_GROUP+86u;
constexpr uint32 G5_FAKE_BRAKE=G_X10, G5_FAKE_SPECIAL=G_X11;
constexpr uintptr_t G5_NEXT=0xB6B998, G5_DRIVER=0xB6B990, G5_SLOTS=0xB6B9A0, G5_UNDERWATER=0xC8132C;

struct G5State { int16 accel{}, brake{}; };
inline G5State g5{};

inline uint32 G5P(const void* p){ return uint32(reinterpret_cast<uintptr_t>(p)); }
inline float G5Nan(){ return std::bit_cast<float>(0x7FC00000u); }

// ---------------------------------------------------------------------------------------------------------------------
// Falsos
// ---------------------------------------------------------------------------------------------------------------------
// GetAccelAndBrake(tVehicleParams&) 0x4F5080 (ret 4)
inline void __fastcall G5_AccBrake(void* self,void*,void* vp){
    if(!Mine())return;
    Push(T5_ACB,Id(self),Id(vp));
    At<int16>(vp,0x20)=g5.accel; At<int16>(vp,0x22)=g5.brake;
}
// CAETwinLoopSoundEntity::Initialise(bank, soundA, soundB, parent, swapMin, swapMax, pctA, pctB) 0x4F28A0 (ret 0x20)
inline void __fastcall G5_TlInit(void* self,void*,int32 bank,int32 a,int32 b,void* parent,uint32 mn,uint32 mx,int32 pa,int32 pb){
    if(!Mine())return;
    Push(T5_TLINIT,Id(self),uint32(bank),uint32(a),uint32(b));
    Push(T5_TLINIT,Id(parent),mn,mx,uint32(pa)^(uint32(pb)<<7));
    At<int16>(self,0x88)=1;   // m_IsInUse (+0x184+0x88 = +0x20C de la entidad)
}
// UpdateTwinLoopSound(CVector pos, float volume, float speed) 0x4F29A0 (ret 0x14)
inline void __fastcall G5_TlUpd(void* self,void*,float x,float y,float z,float vol,float spd){
    if(!Mine())return;
    Push(T5_TLUPD,Id(self),F(x),F(y),F(z));
    Push(T5_TLUPD,F(vol),F(spd));
}
// StopSoundAndForget() 0x4F2960
inline void __fastcall G5_TlStop(void* self,void*){
    if(!Mine())return;
    Push(T5_TLSTOP,Id(self),uint32(uint16(At<int16>(self,0x88))));
    At<int16>(self,0x88)=0;
}
// PlayTwinLoopSound(CVector pos, float volume, float speed, float rollOff, float doppler, flags) 0x4F2CB0 (ret 0x20)
inline void __fastcall G5_TlPlay(void* self,void*,float x,float y,float z,float vol,float spd,float roll,float dop,int32 flags){
    if(!Mine())return;
    Push(T5_TLPLAY,Id(self),F(x),F(y),F(z));
    Push(T5_TLPLAY,F(vol),F(spd),F(roll),F(dop)^uint32(flags));
}
// PlayTrainBrakeSound(eSoundID, float speed, float volume) 0x4FA630 (ret 0xC)
inline void __fastcall G5_PlayBrake(void* self,void*,int32 type,float speed,float vol){
    if(!Mine())return;
    Push(T5_BRAKE,Id(self),uint32(type),F(speed),F(vol));
}
// hermanas de ProcessSpecialVehicle (thiscall con tVehicleParams&, ret 4)
template<uint32 K> inline void __fastcall G5_Sis(void* self,void*,void* vp){
    if(!Mine())return;
    Push(T5_SIS,K,Id(self),Id(AtC<void*>(vp,0x10)),F(AtC<float>(vp,0x1C)));
}

// ---------------------------------------------------------------------------------------------------------------------
// Tabla de funciones (formato exacto para run_local.py)
// ---------------------------------------------------------------------------------------------------------------------
inline const std::vector<Fn> kFnsG5{
    {"CAEVehicleAudioEntity","UpdateBoatSound",0x4F9E90,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessBoatEngine",0x5003A0,0,-1,320},
    {"CAEVehicleAudioEntity","ProcessBoatMovingOverWater",0x4FA0C0,0,-1,192},
    {"CAEVehicleAudioEntity","UpdateTrainSound",0x4FA1C0,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessTrainTrackSound",0x4FA3F0,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessDummyTrainEngine",0x500710,0,-1,384},
    {"CAEVehicleAudioEntity","ProcessPlayerTrainBrakes",0x500AB0,0,-1,256},
    {"CAEVehicleAudioEntity","PlayTrainBrakeSound",0x4FA630,0,-1,192},
    {"CAEVehicleAudioEntity","PlayBicycleSound",0x4F9710,0,-1,256},
    {"CAEVehicleAudioEntity","GetBaseVolumeForBicycleTyre",0x4F60B0,0,-1,128},
    {"CAEVehicleAudioEntity","ProcessDummyRCCar",0x500DC0,0,-1,192},
    {"CAEVehicleAudioEntity","ProcessPlayerCombine",0x500CE0,0,-1,128},
    {"CAEVehicleAudioEntity","ProcessDummyGolfCart",0x501270,0,-1,192},
    {"CAEVehicleAudioEntity","ProcessSpecialVehicle",0x501AB0,0,-1,384},
};

// Globales propios: CWeather::UnderWaterness (los estaticos 0xB6B980..0xB6BB00 y el reloj ya los guarda LiveComparison)
inline void SavesG5(){ Save(G5_UNDERWATER,4); }

inline void StubsG5(){
    constexpr const char* K="CAEVehicleAudioEntity";
    constexpr const char* T="CAETwinLoopSoundEntity";
    AddHookStub(G_X5,0x4F5080,K,"GetAccelAndBrake",FnPtr(&G5_AccBrake));
    AddHookStub(G_X5,0x4F28A0,T,"Initialise",FnPtr(&G5_TlInit));
    AddHookStub(G_X5,0x4F29A0,T,"UpdateTwinLoopSound",FnPtr(&G5_TlUpd));
    AddHookStub(G_X5,0x4F2960,T,"StopSoundAndForget",FnPtr(&G5_TlStop));
    AddHookStub(G_X5,0x4F2CB0,T,"PlayTwinLoopSound",FnPtr(&G5_TlPlay));
    AddHookStub(G5_FAKE_BRAKE,0x4FA630,K,"PlayTrainBrakeSound",FnPtr(&G5_PlayBrake));
    AddHookStub(G5_FAKE_SPECIAL,0x4F8B00,K,"ProcessVehicleRoadNoise",FnPtr(&G5_Sis<1>));
    AddHookStub(G5_FAKE_SPECIAL,0x4F8F10,K,"ProcessVehicleSkidding",FnPtr(&G5_Sis<2>));
    AddHookStub(G5_FAKE_SPECIAL,0x4F8940,K,"ProcessVehicleFlatTyre",FnPtr(&G5_Sis<3>));
    AddHookStub(G5_FAKE_SPECIAL,0x5002C0,K,"ProcessVehicleSirenAlarmHorn",FnPtr(&G5_Sis<4>));
    AddHookStub(G5_FAKE_SPECIAL,0x4F92C0,K,"ProcessRainOnVehicle",FnPtr(&G5_Sis<5>));
    AddHookStub(G5_FAKE_SPECIAL,0x4FAE20,K,"ProcessEngineDamage",FnPtr(&G5_Sis<6>));
    AddHookStub(G5_FAKE_SPECIAL,0x4FA7C0,K,"ProcessDummyRCPlane",FnPtr(&G5_Sis<7>));
    AddHookStub(G5_FAKE_SPECIAL,0x4FAA80,K,"ProcessDummyRCHeli",FnPtr(&G5_Sis<8>));
}

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------------------------------------------------
// Valor float: mezcla de valores borde (1/3) y aleatorios (2/3)
template<size_t N> inline float G5Pick(Ctx& c,float lo,float hi,const float (&edges)[N]){
    return c.chance(3) ? edges[c.next()%N] : c.uni(lo,hi);
}

// Banco dummy: tranvia (0x85) 30%, desconocido (-1) 4%, otro 1..130
inline int G5Bank(Ctx& c){
    const uint32 k=c.next()%100;
    if(k<30)return 0x85;
    if(k<34)return -1;
    return int(1+c.next()%130);
}
// Ranura dummy de la entidad: -1 (20%), 7..16 (60%), otras
inline int G5SlotId(Ctx& c){
    const uint32 k=c.next()%100;
    if(k<20)return -1;
    if(k<80)return int(7+c.next()%10);
    return c.pick<int>({0,2,6,17,19,40});
}
// Estaticos de ranuras dummy: ranuras 0xB6B9A0 (BankID/RefCnt de 16 bits) y siguiente ranura 0xB6B998
inline void G5Slots(Ctx& c,int bank){
    G<int16>(G5_NEXT)=int16(c.next()%10);
    const bool full=c.chance(8);
    for(unsigned i=0;i<10;++i){
        const uint32 r=c.next()%8;
        int16 id= r<3 ? int16(bank) : (r<4 ? int16(-1) : int16(c.next()%140));
        int16 ref=int16(c.pick<int>({0,0,0,1,1,2,3}));
        if(full){ ref=int16(1+c.next()%3); if(id==int16(bank))id=int16(bank+1); }
        G<int16>(G5_SLOTS+4u*i)=id; G<int16>(G5_SLOTS+4u*i+2u)=ref;
    }
}
inline void G5Dummy(Ctx& c,CAEVehicleAudioEntity* e,int bank){
    At<int16>(&e->m_DummyEngineBank,0)=int16(bank);
    At<int16>(&e->m_DummySlot,0)=int16(G5SlotId(c));
    G5Slots(c,bank);
    e->m_EventVolume=c.chance(3) ? 0.f : c.uni(-12.f,6.f);
}

// Probabilidad: CWeather::UnderWaterness (0xC8132C) en torno al umbral 0.5
inline float G5Water(Ctx& c){ static const float v[]{0.f,0.2f,0.4999f,0.5f,0.51f,1.f}; return G5Pick(c,0.f,1.f,v); }

// Entidad base + vehiculo con basura aleatoria y posicion/matriz validas. nullMatrixOneIn: 1 de cada N casos sin matriz (0 = nunca)
inline CAEVehicleAudioEntity* G5Base(Ctx& c,uint32 nullMatrixOneIn){
    c.SetTime(); c.FreshPool(); c.FillEntity(0,0);
    uint8* const ev=c.V(0); uint8* const m=w.mat[0];
    c.fill(ev,0xA00);
    At<float>(ev,4)=c.uni(-100.f,100.f); At<float>(ev,8)=c.uni(-100.f,100.f); At<float>(ev,0xC)=c.uni(0.f,30.f);
    At<float>(m,0x00)=1.f; At<float>(m,0x14)=1.f; At<float>(m,0x28)=1.f;   // matriz: identidad con posicion
    At<float>(m,0x30)=c.uni(-100.f,100.f); At<float>(m,0x34)=c.uni(-100.f,100.f); At<float>(m,0x38)=c.uni(0.f,30.f);
    At<void*>(ev,0x14)= (nullMatrixOneIn && c.chance(nullMatrixOneIn)) ? nullptr : static_cast<void*>(m);
    g5.accel=int16(c.pick<int>({0,0,40,128,255,int(c.next()%256)})); g5.brake=int16(c.pick<int>({0,0,40,128,255,int(c.next()%256)}));
    G<float>(G5_UNDERWATER)=G5Water(c);
    return c.Ent(0);
}
// Estado del vehiculo: byte 0x36 = (estado << 3) | tipo 2 (vehiculo), palabra 0x428 con bEngineOn (bit 4), modelo (+0x22)
inline void G5Veh(Ctx& c,uint8* ev,int status,bool engineOn,int model){
    ev[0x36]=uint8((unsigned(status)<<3)|2u);
    uint32 fl=c.next(); fl&=~(1u<<4); if(engineOn)fl|=1u<<4;
    At<uint32>(ev,0x428)=fl;
    At<int16>(ev,0x22)=int16(model);
}
// tVehicleParams (relleno aleatorio salvo los campos que se leen): Vehicle +0x10, Transmission +0x14, Speed +0x1C
inline void G5Vp(Ctx& c,uint8* ev,uint8* tr,float speed){
    uint8* const vp=w.params; std::memset(vp,0,0x60); c.fill(vp,0x4C);
    At<int32>(vp,0)=c.pick<int32>({-1,0,1,9}); At<int32>(vp,4)=c.pick<int32>({0,1,2,3,9});
    vp[8]=0; At<float>(vp,0xC)=c.uni(0.f,100.f);
    At<void*>(vp,0x10)=ev; At<void*>(vp,0x14)=tr;
    At<uint32>(vp,0x18)=400u+c.next()%212u; At<float>(vp,0x1C)=speed;
}
// Tren: banderas (+0x5B8: bit 3 locomotora, bit 6 sentido horario, bit 8 fuera de carril), velocidad (+0x5A4) y cadena de vagones (+0x5D0).
// Los vagones son nodos solapados dentro de w.veh[1] (cada uno solo necesita su puntero +0x5D0 legible): nChain vagones anteriores.
inline void G5Train(Ctx& c,uint8* ev,bool front,bool notOnRail,bool clockwise,float speed,int nChain){
    At<float>(ev,0x5A4)=speed;
    uint32 fl=c.next()&0xFFFFu; fl&=~((1u<<3)|(1u<<6)|(1u<<8));
    if(front)fl|=1u<<3; if(clockwise)fl|=1u<<6; if(notOnRail)fl|=1u<<8;
    At<uint16>(ev,0x5B8)=uint16(fl);
    uint8* const cv=w.veh[1];
    for(int k=1;k<=nChain;++k)At<void*>(cv+0x10*k,0x5D0)= k<nChain ? static_cast<void*>(cv+0x10*(k+1)) : nullptr;
    At<void*>(ev,0x5D0)= nChain>0 ? static_cast<void*>(cv+0x10) : nullptr;
}
inline std::string G5Ent(Ctx& c){ uint8* const E=c.E(0); return Hex(E,TEMP_SOUND_OFF)+"|"+Hex(E+TEMP_SOUND_END,ENT_SIZE-TEMP_SOUND_END); }

// Velocidad de tren alrededor de los umbrales de ProcessTrainTrackSound (1e-5, 1.0) y de los pedales (0.001)
inline float G5TrainSpeed(Ctx& c){
    static const float e[]{0.f,-0.f,1e-6f,-1e-6f,7.5e-6f,1e-5f,std::nextafterf(1e-5f,0.f),std::nextafterf(1e-5f,1.f),0.0005f,-0.0005f,0.001f,
                           std::nextafterf(0.001f,0.f),std::nextafterf(0.001f,1.f),-0.001f,std::nextafterf(-0.001f,0.f),std::nextafterf(-0.001f,-1.f),
                           0.01f,0.5f,-0.5f,0.9999f,1.f,std::nextafterf(1.f,2.f),1.5f,-1.f,-1.5f,3.f};
    return c.chance(2) ? e[c.next()%(sizeof(e)/sizeof(e[0]))] : (c.chance(2) ? c.uni(-1.5f,1.5f) : c.uni(-0.01f,0.01f));
}
inline int G5Chain(Ctx& c){ return c.chance(5) ? 0 : int(c.next()%10); }

// PlayBicycleSound (0x4F9710) usa un CAESound LOCAL: el EXE no lo construye (solo pone +8 = 0) y la DLL lo construye con
// valores por defecto; los bytes de relleno que Initialise no escribe (+0x6A, +0x72) saldrian de la basura de la pila. Se deja
// a cero la zona de pila que ocuparan los marcos de la funcion (en ambos lados) para que la comparacion sea determinista.
__declspec(noinline) inline void G5ScrubStack(){ volatile uint8 buf[4096]; for(size_t i=0;i<sizeof(buf);++i)buf[i]=0; }


// ---------------------------------------------------------------------------------------------------------------------
// Ejecucion de un caso
// ---------------------------------------------------------------------------------------------------------------------
inline bool RunG5(Ctx& c){
    const std::string& nm=c.nm; auto& out=c.out; auto& ret=c.ret; uint8* const E=c.E(0);

    // ===================================================================================================================
    if(nm=="UpdateBoatSound"){
        auto* const e=G5Base(c,6);
        const uint32 k=c.next()%20;
        const int st= k<5 ? 2 : (k<9 ? 3 : (k<14 ? 6 : c.pick<int>({0,1,4,5,7,8,9,10,11})));
        e->m_AuSettings.VehicleAudioType= c.chance(3) ? AE_AIRCRAFT_SEAPLANE : static_cast<eAEVehicleSoundType>(c.pick<int>({0,1,2,3,4,5,7,8,9,10}));
        At<int16>(&e->m_DummyEngineBank,0)=int16(c.chance(8) ? -1 : int(c.next()%140));
        At<int16>(&e->m_DummySlot,0)=int16(G5SlotId(c));
        e->m_EventVolume=c.chance(3) ? 0.f : c.uni(-12.f,6.f);
        e->m_EngineSounds[size_t(st)].Sound= c.chance(3) ? c.Pre(size_t(st)) : nullptr;
        const int slotArg=c.pick<int>({2,7,8,9,12,16,19,40,0,6});
        const int sfx=int(c.next()%150);
        const float speed=c.uni(0.f,2.f);
        const float vol=c.chance(5) ? -100.f : c.uni(-60.f,12.f);
        { auto P=MakePatches(G_STD); CallThis<void>(c.a,E,st,slotArg,sfx,speed,vol); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessBoatEngine"){
        auto* const e=G5Base(c,0);
        uint8* const ev=c.V(0); uint8* const m=w.mat[0];
        const int bank=G5Bank(c); G5Dummy(c,e,bank);
        e->m_AuSettings.VehicleAudioType= c.chance(4) ? AE_AIRCRAFT_SEAPLANE : static_cast<eAEVehicleSoundType>(c.pick<int>({0,3,3,3,5,9}));
        e->m_IsWreckedVehicle=c.chance(10); e->m_IsPlayerDriver=c.chance(2);
        G<uint32>(G5_DRIVER)= c.chance(3) ? 0u : G5P(w.ped[0]);
        G5Veh(c,ev,c.pick<int>({0,1,2,3,4}),!c.chance(8),c.pick<int>({446,452,453,454,472,473,484,493,539}));
        // matriz: vector "arriba" (+0x20/+0x24/+0x28)
        static const float upz[]{1.f,0.99f,0.5f,0.f,-0.5f,-1.f,0.9999f,0.0001f};
        const float uz=G5Pick(c,-1.f,1.f,upz);
        At<float>(m,0x20)=c.uni(-0.5f,0.5f); At<float>(m,0x24)=c.uni(-0.5f,0.5f); At<float>(m,0x28)=uz;
        // velocidad del motor (+0x5A4) y banderas (+0x5AC: bit 0 en el agua, bit 1 motor en el agua)
        static const float eng[]{0.f,0.1f,0.33f,1.f/3.f,0.34f,0.5f,-0.1f};
        const float es=G5Pick(c,-0.05f,0.6f,eng);
        At<float>(ev,0x5A4)=es;
        ev[0x5AC]=uint8((c.next()&0xFCu)|(c.chance(2)?1u:0u)|(c.chance(2)?2u:0u));
        // frecuencia y volumen actuales: negativos (se toma el nuevo), cerca del objetivo (un paso) o lejos (sube/baja a pasos)
        const float esv=es*3.f;
        const float esc= esv>1.f ? 1.f : (esv<0.f ? 0.f : esv);
        const float tgtF=0.6f+0.1f*(1.f-uz)+esc*0.55f, tgtV=esc*3.f-3.f;
        { const uint32 kf=c.next()%10;
          e->m_CurrentDummyEngineFrequency= kf<2 ? c.uni(-2.f,-0.01f) : (kf<3 ? 0.f : (kf<6 ? tgtF+c.uni(-0.03f,0.03f) : c.uni(0.f,1.6f))); }
        { const uint32 kv=c.next()%10;
          e->m_CurrentDummyEngineVolume= kv<3 ? c.uni(-9.f,-0.01f) : (kv<4 ? 0.f : (kv<7 ? c.uni(0.f,3.5f)+tgtV : c.uni(-3.f,12.f))); }
        e->m_EngineSounds[2].Sound= c.chance(3) ? c.Pre(2) : nullptr;
        e->m_EngineSounds[3].Sound= c.chance(3) ? c.Pre(3) : nullptr;
        G5Vp(c,ev,nullptr,0.f);
        { auto P=MakePatches(G_STD|G_X5); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c); out["vp"]=Hex(w.params+0x20,4);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessBoatMovingOverWater"){
        auto* const e=G5Base(c,6);
        uint8* const ev=c.V(0);
        e->m_AuSettings.VehicleAudioType= c.chance(3) ? AE_AIRCRAFT_SEAPLANE : static_cast<eAEVehicleSoundType>(c.pick<int>({0,3,3,5,9}));
        e->m_EventVolume=c.chance(3) ? 0.f : c.uni(-12.f,6.f);
        ev[0x5AC]=uint8((c.next()&0xFEu)|(c.chance(3)?0u:1u));   // bit 0: en el agua (67%)
        e->m_EngineSounds[6].Sound= c.chance(3) ? c.Pre(6) : nullptr;
        static const float sp[]{0.f,-0.f,1e-6f,7e-6f,7.5e-6f,7.6e-6f,1e-5f,0.1f,0.3f,0.5f,0.7499999f,0.75f,0.76f,1.f,3.f,-0.2f,-0.9f,-1e-3f};
        float speed=G5Pick(c,-1.2f,1.2f,sp);
        if(c.chance(50))speed=G5Nan();
        G5Vp(c,ev,nullptr,speed);
        { auto P=MakePatches(G_STD); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="UpdateTrainSound"){
        auto* const e=G5Base(c,6);
        const int bank=G5Bank(c);
        At<int16>(&e->m_DummyEngineBank,0)=int16(bank);
        At<int16>(&e->m_DummySlot,0)=int16(G5SlotId(c));
        e->m_EventVolume=c.chance(3) ? 0.f : c.uni(-12.f,6.f);
        const uint32 k=c.next()%20;
        const int st= k<5 ? 1 : (k<10 ? 2 : (k<15 ? 3 : c.pick<int>({0,4,5,6,7,8,9,10,11})));
        e->m_EngineSounds[size_t(st)].Sound= c.chance(3) ? c.Pre(size_t(st)) : nullptr;
        const int slotArg=c.pick<int>({2,7,8,9,12,16,19,40,0,6});
        const int sfx=int(c.next()%150);
        const float speed=c.uni(0.5f,1.5f);
        const float vol=c.chance(5) ? -100.f : c.uni(-60.f,12.f);
        { auto P=MakePatches(G_STD); CallThis<void>(c.a,E,st,slotArg,sfx,speed,vol); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessTrainTrackSound"){
        auto* const e=G5Base(c,6);
        uint8* const ev=c.V(0);
        const int bank=G5Bank(c); G5Dummy(c,e,bank);
        G5Veh(c,ev,c.pick<int>({0,1,2,3,4}),true,c.pick<int>({449,537,538,569,590}));
        G5Train(c,ev,c.chance(2),c.chance(8),c.chance(2),G5TrainSpeed(c),G5Chain(c));
        e->m_EngineSounds[2].Sound= c.chance(3) ? c.Pre(2) : nullptr;
        G5Vp(c,ev,nullptr,0.f);
        { auto P=MakePatches(G_STD); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessDummyTrainEngine"){
        auto* const e=G5Base(c,0);
        uint8* const ev=c.V(0); uint8* const m=w.mat[0];
        const int bank=G5Bank(c); G5Dummy(c,e,bank);
        G5Veh(c,ev,c.chance(2) ? 0 : c.pick<int>({1,2,3,4}),true,c.pick<int>({449,537,538,569,590}));
        G5Train(c,ev,!c.chance(6),c.chance(8),c.chance(2),G5TrainSpeed(c),0);
        // matriz: vector "adelante" (+0x10/+0x14/+0x18), la inclinacion se satura con +-0.3 (doble)
        static const float fz[]{0.f,0.3f,-0.3f,std::nextafterf(0.3f,0.f),std::nextafterf(0.3f,1.f),std::nextafterf(-0.3f,0.f),std::nextafterf(-0.3f,-1.f),
                                0.1f,-0.1f,0.9f,-0.9f,1.f,-1.f};
        At<float>(m,0x10)=c.uni(-1.f,1.f); At<float>(m,0x14)=c.uni(-1.f,1.f); At<float>(m,0x18)=G5Pick(c,-1.f,1.f,fz);
        e->m_EngineSounds[1].Sound= c.chance(3) ? c.Pre(1) : nullptr;
        if(c.chance(4)){ fk.padAccel=0; fk.padBrake=0; }
        G5Vp(c,ev,nullptr,0.f);
        { auto P=MakePatches(G_STD|G_PAD); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessPlayerTrainBrakes"){
        auto* const e=G5Base(c,6);
        uint8* const ev=c.V(0);
        e->m_EventVolume=c.chance(3) ? 0.f : c.uni(-12.f,6.f);
        G5Veh(c,ev,0,true,c.pick<int>({449,537,538,569,590}));   // solo STATUS_PLAYER (con otro estado el EXE lee un local sin inicializar)
        G5Train(c,ev,!c.chance(6),c.chance(8),c.chance(2),G5TrainSpeed(c),0);
        { const uint32 k=c.next()%8; if(k==0){ fk.padAccel=0; fk.padBrake=0; } else if(k==1){ fk.padAccel=255; fk.padBrake=255; } }
        G5Vp(c,ev,nullptr,0.f);
        { auto P=MakePatches(G_STD|G_PAD|G5_FAKE_BRAKE); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="PlayTrainBrakeSound"){
        auto* const e=G5Base(c,6);
        At<int16>(&e->m_PlayerEngineBank,0)=int16(c.chance(6) ? -1 : int(c.next()%140));
        const int type=c.pick<int>({-1,0,0,1,2,5,9,12,200});
        e->m_SurfaceSoundType=int16(c.chance(3) ? type : (c.chance(4) ? -1 : int(c.next()%20)));
        At<int16>(E,0x20C)=int16(c.pick<int>({0,0,1,1,2,0x100}));   // m_SkidSound.m_IsInUse
        e->m_EventVolume=c.chance(3) ? 0.f : c.uni(-12.f,6.f);
        const float speed=c.uni(0.5f,1.5f);
        const float vol=c.chance(5) ? -100.f : c.uni(-100.f,12.f);
        { auto P=MakePatches(G_STD|G_X5); CallThis<void>(c.a,E,type,speed,vol); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="PlayBicycleSound"){
        auto* const e=G5Base(c,6);
        const int bank=G5Bank(c); G5Dummy(c,e,bank);
        At<int16>(&e->m_PlayerEngineBank,0)=int16(c.chance(6) ? -1 : int(c.next()%140));
        const uint32 k=c.next()%20;
        const int st= k<6 ? 1 : (k<12 ? 2 : (k<18 ? 3 : c.pick<int>({0,4,5,6,7,8,9,10,11})));
        e->m_EngineSounds[size_t(st)].Sound= c.chance(3) ? c.Pre(size_t(st)) : nullptr;
        const int slotArg=c.pick<int>({2,7,8,9,12,16,19,40,0,6});
        const int sfx=int(c.next()%150);
        const float vol=c.chance(5) ? -100.f : c.uni(-60.f,12.f);
        const float speed=c.uni(0.3f,2.f);
        { auto P=MakePatches(G_STD); G5ScrubStack(); CallThis<void>(c.a,E,st,slotArg,sfx,vol,speed); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="GetBaseVolumeForBicycleTyre"){
        const float last=std::bit_cast<float>(0x3F800347u);   // ultimo x de la tabla 0x8CC18C
        static const float edges[]{0.f,-0.f,0.1f,std::nextafterf(0.1f,0.f),std::nextafterf(0.1f,1.f),0.2f,std::nextafterf(0.2f,0.f),std::nextafterf(0.2f,1.f),
                                   0.5f,std::nextafterf(0.5f,0.f),std::nextafterf(0.5f,1.f),1.f,std::nextafterf(1.f,0.f),std::nextafterf(1.f,2.f),
                                   -0.01f,1.5f,-1.f,0.05f,0.15f,0.35f,0.75f};
        float r;
        { const uint32 k=c.next()%40;
          if(k<20)r=edges[c.next()%(sizeof(edges)/sizeof(edges[0]))];
          else if(k<23)r=last;
          else if(k<25)r=std::nextafterf(last,0.f);
          else if(k<27)r=std::nextafterf(last,2.f);
          else if(k==27)r=G5Nan();
          else r=c.uni(-0.3f,1.4f); }
        const float v=CallThis<float>(c.a,E,r);
        ret=F(v); out["r"]=F(r);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessDummyRCCar"){
        auto* const e=G5Base(c,5);
        uint8* const ev=c.V(0); uint8* const tr=w.tr[0];
        const int bank=G5Bank(c); G5Dummy(c,e,bank);
        e->m_IsWreckedVehicle=c.chance(8);
        G5Veh(c,ev,c.pick<int>({0,1,2,3}),true,c.pick<int>({441,564}));
        float maxV; { const uint32 k=c.next()%16; maxV= k==0 ? 0.f : (k==1 ? -c.uni(0.2f,2.5f) : c.uni(0.2f,2.5f)); }
        At<float>(tr,0x58)=maxV;
        static const float fac[]{0.f,0.05f,0.04999f,0.2f,0.20001f,1.f,1.2f,-0.05f,-1.f,0.5f};
        float speed;
        if(maxV==0.f) speed=c.uni(0.1f,1.f);                       // x / 0 = +inf
        else speed=maxV*G5Pick(c,-1.3f,1.3f,fac);
        e->m_EngineSounds[2].Sound= c.chance(2) ? c.Pre(2) : nullptr;
        G5Vp(c,ev,tr,speed);
        { auto P=MakePatches(G_STD); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessPlayerCombine"){
        auto* const e=G5Base(c,5);
        uint8* const ev=c.V(0);
        e->m_EventVolume=c.chance(3) ? 0.f : c.uni(-12.f,6.f);
        G5Veh(c,ev,c.pick<int>({0,1,2,3}),!c.chance(4),532);
        static const float sp[]{0.f,0.2f,0.1f,0.0001f,-0.01f,0.25f,0.19999999f,2.f};
        const float speed=G5Pick(c,-0.1f,0.4f,sp);
        e->m_EngineSounds[11].Sound= c.chance(2) ? c.Pre(11) : nullptr;
        G5Vp(c,ev,nullptr,speed);
        { auto P=MakePatches(G_STD); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessDummyGolfCart"){
        auto* const e=G5Base(c,5);
        uint8* const ev=c.V(0); uint8* const tr=w.tr[0];
        const int bank=G5Bank(c); G5Dummy(c,e,bank);
        e->m_IsWreckedVehicle=c.chance(8);
        G5Veh(c,ev,c.chance(5) ? 4 : c.pick<int>({0,1,2,3}),!c.chance(8),457);
        float maxV; { const uint32 k=c.next()%16; maxV= k==0 ? 0.f : (k==1 ? -c.uni(0.2f,2.5f) : c.uni(0.2f,2.5f)); }
        At<float>(tr,0x58)=maxV;
        static const float fac[]{0.f,0.05f,0.04999f,0.05001f,0.1f,0.2f,1.f,1.2f,-0.05f,-0.5f,-1.f};
        float speed;
        if(maxV==0.f) speed=c.uni(0.1f,1.f);
        else speed=maxV*G5Pick(c,-1.3f,1.3f,fac);
        e->m_EngineSounds[1].Sound= c.chance(3) ? c.Pre(1) : nullptr;
        G5Vp(c,ev,c.chance(12) ? nullptr : tr,speed);
        { auto P=MakePatches(G_STD); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }

    // ===================================================================================================================
    if(nm=="ProcessSpecialVehicle"){
        auto* const e=G5Base(c,5);
        uint8* const ev=c.V(0); uint8* const tr=w.tr[0];
        const uint32 k=c.next()%26;
        int model;
        if(k<3) model=c.pick<int>({435,450,584,591});
        else if(k<6) model=c.pick<int>({441,564});
        else if(k<10) model=457;
        else if(k<12) model=464;
        else if(k<15) model=c.pick<int>({465,501});
        else if(k<18) model=539;
        else model=c.pick<int>({400,434,436,440,442,449,451,463,466,500,502,538,540,563,565,583,585,590,592,600,611,0,-1,32767,-32768,int(435u+c.next()%157u)});
        const int status= c.chance(3) ? 0 : c.pick<int>({1,2,2,3,4,5});
        G5Veh(c,ev,status,!c.chance(5),model);
        const int bank=G5Bank(c); G5Dummy(c,e,bank);
        e->m_IsWreckedVehicle=c.chance(10);
        float maxV; { const uint32 q=c.next()%16; maxV= q==0 ? 0.f : (q==1 ? -c.uni(0.2f,2.5f) : c.uni(0.2f,2.5f)); }
        At<float>(tr,0x58)=maxV;
        static const float fac[]{0.f,0.05f,0.04999f,0.05001f,0.1f,0.2f,1.f,1.2f,-0.05f,-0.5f,-1.f};
        float speed;
        if(maxV==0.f) speed=c.uni(0.1f,1.f);
        else speed=maxV*G5Pick(c,-1.3f,1.3f,fac);
        if(model==539){   // hovercraft: velocidad cercana a 0.001, campos float en +0x988 y +0x9C4
            static const float hs[]{0.f,0.001f,std::nextafterf(0.001f,0.f),std::nextafterf(0.001f,1.f),0.5f,-0.1f};
            speed=G5Pick(c,-0.2f,1.2f,hs);
            At<float>(ev,0x988)=c.uni(-4.f,4.f); At<float>(ev,0x9C4)=c.uni(-1.f,2.f);
        }
        e->m_EngineSounds[1].Sound= c.chance(3) ? c.Pre(1) : nullptr;
        e->m_EngineSounds[2].Sound= c.chance(3) ? c.Pre(2) : nullptr;
        G5Vp(c,ev,(model==457 && c.chance(12)) ? nullptr : tr,speed);
        { auto P=MakePatches(G_STD|G_PAD|G_X5|G5_FAKE_SPECIAL); CallThis<void>(c.a,E,static_cast<void*>(w.params)); }
        out["ent"]=G5Ent(c);
        return true;
    }
    return false;
}
}
