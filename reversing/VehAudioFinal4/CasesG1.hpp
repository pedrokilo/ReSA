#pragma once
// Grupo 1 del banco VehAudioFinal4 (CAEVehicleAudioEntity): 32 funciones de ciclo de vida, bancos dummy, radio, pedales e inhibiciones.
//   Constructor, Destructor, StaticInitialise, Initialise, Terminate, StaticGetPlayerVehicleAudioSettingsForRadio, StaticService,
//   Enable/DisableHelicoptors, Enable/DisableHelicoptor, DemandBankSlot, RequestBankSlot, StoppedUsingBankSlot, DoesBankSlotContainThisBank,
//   CopHeli, GetVehicleAudioSettings, GetVehicleTypeForAudio, GetFlyingMetalVolume, IsAccInhibited*, Inhibit*ForTime, IsCrzInhibitedForTime,
//   GetAccelAndBrake, UpdateGasPedalAudio, TurnOn/OffRadioForVehicle, PlayerAboutToExitVehicleAsDriver, JustFinishedAccelerationLoop.
// Falsos propios (G_X1): PlaySkidSound (0x4F8360) y PlayTrainBrakeSound (0x4FA630), que llama Terminate (no son de este grupo: se doblan con traza).
// Tipos de traza propios: T_GROUP+16 (PlaySkidSound) y T_GROUP+17 (PlayTrainBrakeSound).
// Supuestos documentados:
//  - m_tempSound (offset 8..0x7C) no se compara en el mundo; Constructor/Destructor comparan ademas los dwords +0x00/+0x04/+0x10 y +0x194 aparte.
//  - GetVehicleAudioSettings: el relleno de la entrada (offsets 1, 7, 0x11..0x13, 0x1D..0x1F) se enmascara a cero tras la llamada: en el EXE la
//    entrada 231 (modelo 631, inexistente) tiene 0x01 en 0x1D..0x1F y en la tabla de la DLL es 0. Initialise se prueba con modelos 400..630.
//  - GetVehicleAudioSettings: el EXE es __stdcall con puntero oculto (ret 8) y la DLL (MSVC, estatica __cdecl) deja la limpieza al llamador:
//    el lado EXE se llama como __stdcall y el lado DLL (candidato) como __cdecl.
#include "Ctx.hpp"

namespace VehAudioFinal4Live {
constexpr uintptr_t G1_ATTACHED=0xB6B98C, G1_DRIVER=0xB6B990, G1_HELI=0xB6B994, G1_NEXT=0xB6B998, G1_RADIOSET=0xB6B99C, G1_SLOTS=0xB6B9A0, G1_REPLAY=0xA43088;

inline const std::vector<Fn> kFnsG1{
    {"CAEVehicleAudioEntity","Constructor",0x4F63E0,0,-1,64},
    {"CAEVehicleAudioEntity","Destructor",0x6D0A10,0,-1,128},
    {"CAEVehicleAudioEntity","StaticInitialise",0x5B99F0,0,-1,64},
    {"CAEVehicleAudioEntity","Initialise",0x4F7670,0,-1,288},
    {"CAEVehicleAudioEntity","Terminate",0x4FB8C0,0,-1,192},
    {"CAEVehicleAudioEntity","StaticGetPlayerVehicleAudioSettingsForRadio",0x4F4ED0,0,-1,64},
    {"CAEVehicleAudioEntity","StaticService",0x4F4EC0,0,-1,64},
    {"CAEVehicleAudioEntity","EnableHelicoptors",0x4F4EF0,0,-1,64},
    {"CAEVehicleAudioEntity","DisableHelicoptors",0x4F4EE0,0,-1,64},
    {"CAEVehicleAudioEntity","EnableHelicoptor",0x4F5C00,0,-1,64},
    {"CAEVehicleAudioEntity","DisableHelicoptor",0x4F5BF0,0,-1,64},
    {"CAEVehicleAudioEntity","DemandBankSlot",0x4F4E60,0,-1,128},
    {"CAEVehicleAudioEntity","RequestBankSlot",0x4F4D10,0,-1,192},
    {"CAEVehicleAudioEntity","StoppedUsingBankSlot",0x4F4DF0,0,-1,96},
    {"CAEVehicleAudioEntity","DoesBankSlotContainThisBank",0x4F4E30,0,-1,96},
    {"CAEVehicleAudioEntity","CopHeli",0x4F5C40,0,-1,64},
    {"CAEVehicleAudioEntity","GetVehicleAudioSettings",0x4F5C10,0,-1,232},
    {"CAEVehicleAudioEntity","GetVehicleTypeForAudio",0x4F4F00,0,-1,256},
    {"CAEVehicleAudioEntity","GetFlyingMetalVolume",0x4F6150,0,-1,128},
    {"CAEVehicleAudioEntity","IsAccInhibited",0x4F4F70,0,-1,160},
    {"CAEVehicleAudioEntity","IsAccInhibitedBackwards",0x4F4FC0,0,-1,80},
    {"CAEVehicleAudioEntity","IsAccInhibitedForLowSpeed",0x4F4FF0,0,-1,80},
    {"CAEVehicleAudioEntity","IsAccInhibitedForTime",0x4F5020,0,-1,64},
    {"CAEVehicleAudioEntity","InhibitAccForTime",0x4F5030,0,-1,64},
    {"CAEVehicleAudioEntity","InhibitCrzForTime",0x4F5060,0,-1,64},
    {"CAEVehicleAudioEntity","IsCrzInhibitedForTime",0x4F5050,0,-1,64},
    {"CAEVehicleAudioEntity","GetAccelAndBrake",0x4F5080,0,-1,256},
    {"CAEVehicleAudioEntity","UpdateGasPedalAudio",0x4F5EB0,0,-1,160},
    {"CAEVehicleAudioEntity","TurnOnRadioForVehicle",0x4F5B20,0,-1,64},
    {"CAEVehicleAudioEntity","TurnOffRadioForVehicle",0x4F5B60,0,-1,64},
    {"CAEVehicleAudioEntity","PlayerAboutToExitVehicleAsDriver",0x4F5BA0,0,-1,64},
    {"CAEVehicleAudioEntity","JustFinishedAccelerationLoop",0x4F5E50,0,-1,128},
};

// Globales propios: modo de la repeticion (CReplay::Mode, byte). Los estaticos 0xB6B980..0xB6BB00 ya los guarda LiveComparison.
inline void SavesG1(){ Save(G1_REPLAY,1); }

// CAEVehicleAudioEntity::PlaySkidSound (0x4F8360, thiscall, ret 0Ch) / PlayTrainBrakeSound (0x4FA630, ret 0Ch): dobles trazados
inline void __fastcall G1_PlaySkid(void* self,void*,int32 type,float speed,float vol){ if(!Mine())return; Push(T_GROUP+16,Id(self),uint16(type),F(speed),F(vol)); }
inline void __fastcall G1_PlayTrain(void* self,void*,int32 type,float speed,float vol){ if(!Mine())return; Push(T_GROUP+17,Id(self),uint16(type),F(speed),F(vol)); }
inline void StubsG1(){
    AddHookStub(G_X1,0x4F8360,"CAEVehicleAudioEntity","PlaySkidSound",FnPtr(&G1_PlaySkid));
    AddHookStub(G_X1,0x4FA630,"CAEVehicleAudioEntity","PlayTrainBrakeSound",FnPtr(&G1_PlayTrain));
}

// ---------------------------------------------------------------------------------------------------------------------
// Utilidades
// ---------------------------------------------------------------------------------------------------------------------
inline uint32 G1_P(const void* p){ return uint32(reinterpret_cast<uintptr_t>(p)); }
inline void* G1_Slots_(size_t byteOffset){ return reinterpret_cast<void*>(G1_SLOTS+byteOffset); }

// Ranuras dummy de bancos (10 x {BankID int16, RefCnt int16} en 0xB6B9A0) e indice siguiente (0xB6B998)
inline void G1_Slots(Ctx& c){
    const uint32 mode=c.next()%7;      // 0: todas ocupadas, 1: todas libres, resto: mezcla
    for(size_t k=0;k<10;++k){
        const int16 bank= c.chance(6) ? int16(-1) : int16(7+c.next()%8);
        int16 ref;
        if(mode==0)ref=int16(1+c.next()%3);
        else if(mode==1)ref= c.chance(4) ? int16(-1) : int16(0);
        else ref=int16(c.pick<int>({0,0,1,1,2,5,-1}));
        At<int16>(G1_Slots_(4*k),0)=bank; At<int16>(G1_Slots_(4*k+2),0)=ref;
    }
    G<int16>(G1_NEXT)=int16(c.next()%10);
}
// Banco para las peticiones: normalmente alguno que ya esta en una ranura (coincidencia), a veces otro o -1
inline int32 G1_Bank(Ctx& c){
    const uint32 k=c.next()%10;
    if(k<6)return int32(At<int16>(G1_Slots_(4*(c.next()%10)),0));
    if(k<8)return int32(7+c.next()%8);
    if(k==8)return int32(c.next()%200);
    return -1;
}
// Instante cercano a 'now' (comparaciones sin signo con envoltura)
inline uint32 G1_TimeNear(Ctx& c,uint32 now){
    switch(c.next()%9){
    case 0: return now;
    case 1: return now-1u;
    case 2: return now+1u;
    case 3: return now+(c.next()%2000u)-1000u;
    case 4: return c.next();
    case 5: return now+0x80000000u;
    case 6: return now-0x80000000u;
    case 7: return 0u;
    default: return now-(c.next()%100000u);
    }
}
// Sonido falso k de la reserva con flags, entidad de audio y referencia fisica aleatorios (null con probabilidad 1/nullOneIn)
inline CAESound* G1_Give(Ctx& c,size_t& next,uint32 nullOneIn){
    if(c.chance(nullOneIn))return nullptr;
    uint8* s=w.snd[next++];
    At<uint16>(s,0x56)=uint16(c.next());
    At<uint32>(s,4)=G1_P(w.ent[0]);
    At<uint32>(s,8)= c.chance(2) ? G1_P(w.veh[1]) : 0u;
    return reinterpret_cast<CAESound*>(s);
}
// tVehicleParams falso en w.params con los campos que leen IsAcc*
inline uint8* G1_Vp(Ctx& c){
    uint8* vp=w.params; c.fill(vp,sizeof(w.params));
    At<int16>(vp,0x22)=int16(c.pick<int>({0,0,0,1,10,128,255,-1,-100,int(c.next()%256)}));
    At<uint8>(vp,0x39)=uint8(c.chance(3) ? 1 : 0);
    float ws;
    switch(c.next()%8){ case 0: ws=5.f; break; case 1: ws=Fl(F(5.f)+1u); break; case 2: ws=Fl(F(5.f)-1u); break; case 3: ws=0.f; break; case 4: ws=-3.f; break; case 5: ws=100.f; break; default: ws=c.uni(0.f,10.f); break; }
    At<float>(vp,0x40)=ws;
    At<uint8>(vp,0x46)=uint8(c.pick<int>({0,0,1,2,3,4,4,int(c.next()%6)}));
    float sp;
    switch(c.next()%8){ case 0: sp=0.1f; break; case 1: sp=Fl(F(0.1f)+1u); break; case 2: sp=Fl(F(0.1f)-1u); break; case 3: sp=0.f; break; case 4: sp=-1.f; break; case 5: sp=2.f; break; case 6: sp=0.05f; break; default: sp=c.uni(0.f,0.3f); break; }
    At<float>(vp,0x1C)=sp;
    return vp;
}
// Pedal del vehiculo (m_GasPedal/m_BrakePedal): dentro, fuera y en los bordes de [0, 1]
inline float G1_Pedal(Ctx& c){
    switch(c.next()%12){
    case 0: return 0.f;
    case 1: return 1.f;
    case 2: return -0.f;
    case 3: return c.uni(-2.f,-0.0001f);
    case 4: return c.uni(1.0001f,3.f);
    case 5: return Fl(F(1.f)+1u);
    case 6: return Fl(F(1.f)-1u);
    case 7: return c.uni(0.f,1.f)*0.001f;
    case 8: return float(c.next()%256)/255.f;
    case 9: return float(c.next()%256)/256.f;
    default: return c.uni(0.f,1.f);
    }
}
// Estado para Terminate y Destructor: sonidos repartidos, radio, ranura dummy, estaticos y campos del derrape (twin loop)
inline void G1_PrepTerm(Ctx& c){
    c.SetTime(); c.FreshPool(); c.FillEntity(0,0);
    auto* const e=c.Ent(0); uint8* const b=c.E(0);
    G1_Slots(c);
    e->m_IsInitialized=!c.chance(7);
    e->m_IsPlayerDriver=c.chance(2); e->m_IsPlayerPassenger=c.chance(2); e->m_IsPlayerDriverAboutToExit=c.chance(3);
    e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(1+c.next()%9);
    const int rt=c.pick<int>({-1,0,0,0,1,2,2,3,3,int(c.next()%256)-128});
    e->m_AuSettings.RadioType=static_cast<eAERadioType>(rt);
    size_t k=0;
    for(auto& es:e->m_EngineSounds)es.Sound=G1_Give(c,k,4);
    e->m_SurfaceSound=G1_Give(c,k,3); e->m_RoadNoiseSound=G1_Give(c,k,3); e->m_FlatTireSound=G1_Give(c,k,3); e->m_ReverseGearSound=G1_Give(c,k,3);
    e->m_HornSound=G1_Give(c,k,3); e->m_SirenSound=G1_Give(c,k,3); e->m_FastSirenSound=G1_Give(c,k,3);
    e->m_SurfaceSoundType=int16(c.next()%30); e->m_RoadNoiseSoundType=int16(c.next()%30); e->m_FlatTireSoundType=int16(c.next()%30); e->m_ReverseGearSoundType=int16(c.next()%30);
    // ranura dummy y bancos: la referencia solo baja si la ranura contiene todavia el banco de la entidad
    const int16 slotId=int16(c.pick<int>({-1,-1,7,8,9,10,11,12,13,14,15,16,0,6,17,40}));
    At<int16>(&e->m_DummySlot,0)=slotId;
    const int idx=int(slotId)-7;
    int16 bank= c.chance(3) ? int16(-1) : int16(c.next()%40);
    if(idx>=0&&idx<10&&c.next()%4!=0)bank=At<int16>(G1_Slots_(4*size_t(idx)),0);
    At<int16>(&e->m_DummyEngineBank,0)=bank; At<int16>(&e->m_PlayerEngineBank,0)=int16(c.next()%40);
    // estaticos de radio y conductor
    G<uint32>(G1_DRIVER)= c.chance(2) ? G1_P(w.ped[0]) : 0u;
    G<uint32>(G1_ATTACHED)= c.chance(2) ? G1_P(w.ped[1]) : 0u;
    G<uint32>(G1_RADIOSET)= c.chance(2) ? G1_P(&e->m_AuSettings) : 0u;
    // m_SkidSound (twin loop en +0x184): bandera de uso (+0x20C), sonidos (+0x224, +0x228) y referencia fisica de su escritorio (+0x194)
    const int16 inUse=int16(c.pick<int>({0,0,1,1,0x100}));
    At<int16>(b,0x20C)=inUse;
    At<uint32>(b,0x224)=G1_P(G1_Give(c,k,3)); At<uint32>(b,0x228)=G1_P(G1_Give(c,k,3));
    At<uint32>(b,0x194)= c.chance(2) ? G1_P(w.veh[1]) : 0u;
    At<uint32>(b,0x10)= c.chance(2) ? G1_P(w.veh[1]) : 0u;     // referencia fisica del m_tempSound de la entidad
}

// ---------------------------------------------------------------------------------------------------------------------
// Ejecucion de un caso
// ---------------------------------------------------------------------------------------------------------------------
inline bool RunG1(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; auto& out=c.out; auto& ret=c.ret;
    uint8* const E0=c.E(0);

    // ---- Constructor (0x4F63E0): solo escribe +0x00, +0x04, +0x10, +0x184, +0x194, +0x20C(16), +0x224, +0x228 y 0xA4 ----
    if(nm=="Constructor"){
        const uint32 mode=c.next()%3;
        if(mode==0)std::memset(E0,0xCD,ENT_BUF); else if(mode==1)std::memset(E0,0,ENT_BUF); else c.fill(E0,ENT_BUF);
        void* const r=CallThis<void*>(a,E0);
        ret=(r==static_cast<void*>(E0));
        out["head"]=c.Bytes(E0,0x20); out["ent"]=c.Bytes(E0+TEMP_SOUND_END,ENT_BUF-TEMP_SOUND_END);
        return true;
    }
    // ---- Destructor (0x6D0A10): Terminate si estaba inicializada, destructor de m_SkidSound y de m_tempSound ----
    if(nm=="Destructor"){
        G1_PrepTerm(c);
        void* r=nullptr;
        { auto P=MakePatches(G_REF|G_RADIO|G_X1); r=CallThis<void*>(a,E0); }
        (void)r; ret=nullptr;   // el EXE no devuelve 'this' (6D0A70: eax = resultado del destructor de m_tempSound); no se compara
        out["head"]=c.Bytes(E0,0x20); out["ent"]=c.Bytes(E0+TEMP_SOUND_END,ENT_BUF-TEMP_SOUND_END);
        return true;
    }
    // ---- StaticInitialise (0x5B99F0): 10 ranuras a {-1, 0}, tres LoadSoundBank y estaticos a cero ----
    if(nm=="StaticInitialise"){
        c.fill(reinterpret_cast<void*>(G1_ATTACHED),0x3C);     // 0xB6B98C..0xB6B9C8: estaticos de radio, ranuras dummy
        { auto P=MakePatches(G_HW); CallCdecl<void>(a); }
        out["statics"]=c.Bytes(reinterpret_cast<void*>(G1_ATTACHED),0x3C);
        return true;
    }
    // ---- Initialise (0x4F7670): modelos 400..630 de la tabla (la 631 no es un vehiculo) ----
    if(nm=="Initialise"){
        c.SetTime(); c.FreshPool();
        std::memset(E0,0xCD,ENT_BUF);
        G1_Slots(c);
        const uint32 model= c.i<231 ? uint32(400+c.i) : 400u+c.next()%231u;
        const bool init= c.i<231 ? (c.i&1)!=0 : c.chance(2);
        uint8* const v=c.V(0); At<uint16>(v,0x22)=uint16(model);
        At<uint8>(E0,0xA4)=uint8(init ? 1 : 0);
        { auto P=MakePatches(G_SND|G_HW); CallThis<void>(a,E0,static_cast<void*>(v)); }
        out["ent"]=c.Bytes(E0+TEMP_SOUND_END,ENT_BUF-TEMP_SOUND_END);
        return true;
    }
    // ---- Terminate (0x4FB8C0) ----
    if(nm=="Terminate"){
        G1_PrepTerm(c);
        { auto P=MakePatches(G_REF|G_RADIO|G_X1); CallThis<void>(a,E0); }
        out["ent"]=c.Bytes(E0+TEMP_SOUND_END,ENT_BUF-TEMP_SOUND_END);
        return true;
    }
    // ---- estaticos de radio / servicio / helicopteros ----
    if(nm=="StaticGetPlayerVehicleAudioSettingsForRadio"){
        c.FillEntity(0,0);
        const uint32 k=c.next()%3;
        G<uint32>(G1_RADIOSET)= k==0 ? 0u : (k==1 ? G1_P(&c.Ent(0)->m_AuSettings) : c.next());
        ret=uint32(CallCdecl<uintptr_t>(a));
        return true;
    }
    if(nm=="StaticService"){
        c.fill(reinterpret_cast<void*>(G1_ATTACHED),0x3C);
        CallCdecl<void>(a);
        out["statics"]=c.Bytes(reinterpret_cast<void*>(G1_ATTACHED),0x3C);
        return true;
    }
    if(nm=="EnableHelicoptors"||nm=="DisableHelicoptors"){
        G<uint8>(G1_HELI)=uint8(c.pick<int>({0,1,1,0x55,int(c.next()%256)}));
        CallCdecl<void>(a);
        out["flag"]=int(G<uint8>(G1_HELI));
        return true;
    }
    if(nm=="EnableHelicoptor"||nm=="DisableHelicoptor"){
        c.fill(E0,ENT_BUF);
        At<uint8>(E0,0xBD)=uint8(c.pick<int>({0,1,1,0x55,int(c.next()%256)}));
        CallThis<void>(a,E0);
        out["flag"]=int(At<uint8>(E0,0xBD));
        return true;
    }
    // ---- ranuras dummy de bancos ----
    if(nm=="RequestBankSlot"||nm=="DemandBankSlot"){
        G1_Slots(c);
        const int32 bank=G1_Bank(c);
        int32 r=0;
        { auto P=MakePatches(G_SND|G_HW); r=CallCdecl<int32>(a,bank); }
        ret=int(int16(r));
        out["slots"]=c.Bytes(G1_Slots_(0),40); out["next"]=int(G<int16>(G1_NEXT)); out["bank"]=bank;
        return true;
    }
    if(nm=="StoppedUsingBankSlot"){
        G1_Slots(c);
        const int32 slot= c.chance(4) ? int32(c.next()%60)-10 : int32(7+c.next()%10);
        CallCdecl<void>(a,slot);
        out["slots"]=c.Bytes(G1_Slots_(0),40); out["slot"]=slot;
        return true;
    }
    if(nm=="DoesBankSlotContainThisBank"){
        G1_Slots(c);
        const int32 slot= c.chance(5) ? int32(c.next()%40)-8 : int32(7+c.next()%10);
        const int32 bank= c.chance(2) ? int32(At<int16>(G1_Slots_(4*(c.next()%10)),0)) : G1_Bank(c);
        ret=CallCdecl<bool>(a,slot,bank);
        out["slot"]=slot; out["bank"]=bank;
        return true;
    }
    // ---- CopHeli ----
    if(nm=="CopHeli"){
        c.FillEntity(0,0);
        static const int models[]{487,488,486,489,0,400,611,1,0xFFFF,0x1E7,0x1E8};
        At<uint16>(c.V(0),0x22)= c.chance(3) ? uint16(c.next()) : uint16(models[c.next()%std::size(models)]);
        ret=CallThis<bool>(a,E0);
        return true;
    }
    // ---- GetVehicleAudioSettings (0x4F5C10, ret 8; devuelve la estructura por puntero oculto): una prueba por modelo 400..631 ----
    if(nm=="GetVehicleAudioSettings"){
        c.fill(w.settings,sizeof(w.settings));
        const int32 id=int32(400+c.i%232);
        void* const dest=w.settings;
        if(c.candidate)reinterpret_cast<void*(__cdecl*)(void*,int32)>(a)(dest,id);       // DLL (MSVC): el llamador limpia la pila
        else reinterpret_cast<void*(__stdcall*)(void*,int32)>(a)(dest,id);               // EXE: ret 8
        static const size_t pad[]{1,7,0x11,0x12,0x13,0x1D,0x1E,0x1F};
        for(const size_t p:pad)w.settings[p]=0;
        out["s"]=c.Bytes(w.settings,0x24); out["id"]=id;
        return true;
    }
    // ---- GetVehicleTypeForAudio: los 256 valores del byte VehicleAudioTypeForName (+0x9C) ----
    if(nm=="GetVehicleTypeForAudio"){
        c.FillEntity(0,0);
        At<uint8>(E0,0x9C)= c.i<256 ? uint8(c.i) : uint8(c.next());
        auto* const e=c.Ent(0);
        e->m_IsInitialized= c.i<256 ? !c.chance(9) : c.chance(2);
        if(c.chance(9))e->m_Entity=nullptr;
        ret=int(CallThis<int16>(a,E0));
        return true;
    }
    // ---- GetFlyingMetalVolume ----
    if(nm=="GetFlyingMetalVolume"){
        c.FillEntity(0,0);
        uint8* const ph=c.V(1);
        auto comp=[&]()->float{
            switch(c.next()%8){
            case 0: return 0.f;
            case 1: return c.uni(-1e-5f,1e-5f);
            case 2: return c.uni(-0.3f,0.3f);
            case 3: case 4: case 5: return c.uni(-1.f,1.f);
            case 6: return c.uni(-3.f,3.f);
            default: return c.uni(-0.01f,0.01f);
            }
        };
        float x=comp(),y=comp(),z=comp();
        if(c.i<8){
            static const float sp[8][3]{{0.f,0.f,0.f},{0.75f,0.f,0.f},{0.7499f,0.f,0.f},{1e-5f,0.f,0.f},{4e-6f,0.f,0.f},{0.f,0.f,-0.75f},{0.5f,0.5f,0.5f},{0.f,0.3f,0.f}};
            x=sp[c.i][0]; y=sp[c.i][1]; z=sp[c.i][2];
        }
        At<float>(ph,0x50)=x; At<float>(ph,0x54)=y; At<float>(ph,0x58)=z;
        ret=F(CallThis<float>(a,E0,static_cast<void*>(ph)));
        return true;
    }
    // ---- IsAcc* ----
    if(nm=="IsAccInhibited"||nm=="IsAccInhibitedBackwards"||nm=="IsAccInhibitedForLowSpeed"){
        c.FillEntity(0,0);
        auto* const e=c.Ent(0);
        e->m_IsSingleGear=c.chance(3);
        uint8* const vp=G1_Vp(c);
        bool r=false;
        { auto P=MakePatches(G_HW); r=CallThis<bool>(a,E0,static_cast<void*>(vp)); }
        ret=r;
        out["vp"]=c.Bytes(vp,0x4C);
        return true;
    }
    if(nm=="IsAccInhibitedForTime"||nm=="IsCrzInhibitedForTime"){
        c.FillEntity(0,0);
        const uint32 now=c.SetTime();
        At<uint32>(E0,nm=="IsAccInhibitedForTime" ? 0xD0 : 0xD4)=G1_TimeNear(c,now);
        ret=CallThis<bool>(a,E0);
        out["now"]=now;
        return true;
    }
    if(nm=="InhibitAccForTime"||nm=="InhibitCrzForTime"){
        c.FillEntity(0,0);
        const uint32 now=c.SetTime();
        const uint32 t= c.chance(5) ? c.pick<uint32>({0u,1u,0xFFFFFFFFu,0x80000000u}) : (c.chance(2) ? c.next()%5000u : c.next());
        CallThis<void>(a,E0,t);
        out["now"]=now; out["t"]=t;
        out["acc"]=At<uint32>(E0,0xD0); out["crz"]=At<uint32>(E0,0xD4);
        return true;
    }
    // ---- GetAccelAndBrake (0x4F5080): modo de repeticion (byte 0xA43088), conductor del jugador y pad ----
    if(nm=="GetAccelAndBrake"){
        c.SetTime(); c.FillEntity(0,0);
        auto* const e=c.Ent(0);
        G<uint8>(G1_REPLAY)=uint8(c.pick<int>({0,0,1,1,1,2,0x7F}));
        uint8* const veh=c.V(0);
        At<float>(veh,0x49C)=G1_Pedal(c); At<float>(veh,0x4A0)=G1_Pedal(c);
        uint8* const vp=w.params; c.fill(vp,sizeof(w.params));
        At<uint32>(vp,0x10)=G1_P(veh);
        uint8* const ped=w.ped[0]; c.fill(ped,0x800);
        const int state=c.pick<int>({0,1,2,3,8,20,50,0x36,0x37,0x3E,0x3F,0x40,int(c.next()%70)});
        At<uint32>(ped,0x530)=uint32(state);
        At<uint32>(ped,0x598)=uint32(c.pick<int>({0,0,0,1}));      // PED_TYPE_PLAYER1 / PLAYER2 (otro tipo daria un pad nulo)
        G<uint32>(G1_DRIVER)= c.chance(5) ? 0u : G1_P(ped);
        e->m_IsPlayerDriverAboutToExit=c.chance(4);
        { auto P=MakePatches(G_PAD); CallThis<void>(a,E0,static_cast<void*>(vp)); }
        out["vp"]=c.Bytes(vp,0x4C);
        out["accel"]=int(At<int16>(vp,0x20)); out["brake"]=int(At<int16>(vp,0x22));
        return true;
    }
    // ---- UpdateGasPedalAudio (0x4F5EB0): solo automovil (tipo 0, +0x964) y moto (tipo 9, +0x808) ----
    if(nm=="UpdateGasPedalAudio"){
        c.FillEntity(0,0);
        uint8* const v=c.V(0);
        const float cur= c.chance(8) ? 0.f : (c.chance(8) ? 1.f : c.uni(0.f,1.f));
        const float curB= c.uni(0.f,1.f);
        float gas;
        switch(c.next()%10){
        case 0: gas=cur+0.09f; break;
        case 1: gas=cur-0.07f; break;
        case 2: gas=cur; break;
        case 3: gas=-cur; break;
        case 4: gas=c.uni(-1.f,1.f); break;
        case 5: gas=cur+c.uni(0.f,0.2f); break;
        case 6: gas=cur-c.uni(0.f,0.2f); break;
        case 7: gas=0.f; break;
        case 8: gas=1.f; break;
        default: gas=c.uni(-1.2f,1.2f); break;
        }
        At<float>(v,0x49C)=gas; At<float>(v,0x964)=cur; At<float>(v,0x808)=curB;
        const int32 type=c.pick<int>({0,0,0,9,9,9,1,2,3,4,5,6,7,8,10,-1,100});
        CallThis<void>(a,E0,static_cast<void*>(v),type);
        out["ra"]=F(At<float>(v,0x964)); out["rb"]=F(At<float>(v,0x808));
        return true;
    }
    // ---- radio ----
    if(nm=="TurnOnRadioForVehicle"||nm=="TurnOffRadioForVehicle"||nm=="PlayerAboutToExitVehicleAsDriver"){
        c.FillEntity(0,0);
        auto* const e=c.Ent(0);
        const int rt=c.pick<int>({-1,0,0,1,2,2,3,3,int(c.next()%256)-128});
        e->m_AuSettings.RadioType=static_cast<eAERadioType>(rt);
        e->m_IsPlayerDriver=c.chance(2); e->m_IsPlayerDriverAboutToExit=c.chance(3);
        At<uint32>(c.V(0),0x460)= c.chance(3) ? 0u : (c.chance(2) ? G1_P(w.ped[0]) : 0x12345678u);     // m_pDriver
        G<uint32>(G1_ATTACHED)= c.chance(2) ? G1_P(w.ped[1]) : 0u;
        G<uint32>(G1_DRIVER)= c.chance(2) ? G1_P(w.ped[0]) : 0u;
        G<uint32>(G1_RADIOSET)= c.chance(2) ? G1_P(&e->m_AuSettings) : 0u;
        { auto P=MakePatches(G_RADIO); CallThis<void>(a,E0); }
        out["attached"]=G<uint32>(G1_ATTACHED); out["settings"]=G<uint32>(G1_RADIOSET); out["exit"]=int(At<uint8>(E0,0xA7));
        return true;
    }
    // ---- JustFinishedAccelerationLoop (0x4F5E50) ----
    if(nm=="JustFinishedAccelerationLoop"){
        c.FreshPool(); c.FillEntity(0,0);
        auto* const e=c.Ent(0);
        e->m_FramesAgoACLooped=int16(c.pick<int>({0,1,5,8,9,9,10,10,11,12,40,-1,-5,32767,int(c.next()%14)}));
        const int pos= c.chance(5) ? -1 : int(c.pick<int>({0,1,500,1000,3000,32000,int(c.next()%3000)}));
        e->m_ACPlayPositionThisFrame=int16(pos);
        e->m_ACPlayPositionLastFrame= c.chance(5) ? int16(-1) : int16(c.next()%3000);
        if(c.chance(6)){ e->m_EngineSounds[CAEVehicleAudioEntity::AE_SOUND_PLAYER_AC].Sound=nullptr; }
        else {
            CAESound* const s=c.Pre(4);
            e->m_EngineSounds[CAEVehicleAudioEntity::AE_SOUND_PLAYER_AC].Sound=s;
            const int delta=c.pick<int>({-2,-1,0,1,2,119,120,121,-120,500,-500,int(c.next()%400)-200});
            int len=pos+120+delta;
            if(len>32767)len=32767;
            if(len<-32768)len=-32768;
            s->m_Length=int16(len);
        }
        ret=CallThis<bool>(a,E0);
        out["frames"]=int(e->m_FramesAgoACLooped);
        return true;
    }
    return false;
}
}
