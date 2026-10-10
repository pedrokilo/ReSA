#pragma once
// Grupo 2 (23 funciones de CAEVehicleAudioEntity): frecuencias/volumenes puros (dummy y jugador), UpdateParameters (virtual),
// sonidos de motor (Start/Cancel/Stop/RequestNew/UpdateGeneric), cambios de conductor (JustGotIn/JustGotOut/JustWrecked), los dos
// AddAudioEvent, la transicion de estados dummy, el motor dummy, el dano de motor, el nitro y las piezas moviles.
// Contrato con LiveComparison.hpp: kFnsG2 / SavesG2 / StubsG2 / RunG2. Todos los helpers llevan el prefijo G2.
//
// Notas de dominio (documentadas para el README del banco):
//  - m_tempSound (offset 8..0x7C de la entidad) no se compara (el EXE lo usa como escritorio): ya lo excluye WorldHash.
//  - Nunca se usan NaN ni sNaN. Los umbrales del EXE (0.99 doble, 0.65, 0.75, 1e-5, 390/650...) se prueban por ambos lados.
//  - Helicopteros: UpdateParameters con GetEngineSound(AE_SOUND_AIRCRAFT_NEAR) == sonido llamaria a GetComponentWorldPosition (clump real):
//    para el tipo 4 ese sonido no se usa (solo avion/hidroavion).
//  - El vehiculo falso (w.veh[0]) solo rellena los campos que leen las funciones: modelo +0x22, estado +0x36, matriz +0x14 (o posicion
//    propia +4), banderas +0x428..0x42B, vida +0x4C0, remolque +0x4C8, conductor +0x460, vel. angular +0x50, nitro +0x38C/+0x8A4,
//    angulos de piezas +0x86C/+0x86E y moto +0x614/+0x648.
//  - m_SkidSound (twin loop en +0x184): m_IsInUse (+0x20C) y los dos sonidos (+0x224/+0x228) se fijan a un estado coherente.
//  - Constantes BSS del EXE (0xB6B9C8..0xB6BAC4) se leen tal cual (se calculan al arrancar el juego).
#include "Ctx.hpp"
#include <cmath>

namespace HookUnlockAudioLive {
inline const std::vector<Fn> kFnsG2{
    {"CAEVehicleAudioEntity","GetFreqForIdle",0x4F5C60,0,-1,64},
    {"CAEVehicleAudioEntity","GetFrequencyForDummyIdle",0x4F5310,0,-1,64},
    {"CAEVehicleAudioEntity","GetFrequencyForDummyRev",0x4F54F0,0,-1,64},
    {"CAEVehicleAudioEntity","GetVolumeForDummyIdle",0x4F51F0,0,-1,96},
    {"CAEVehicleAudioEntity","GetVolumeForDummyRev",0x4F53D0,0,-1,96},
    {"CAEVehicleAudioEntity","GetFreqForPlayerEngineSound",0x4F8070,0,-1,192},
    {"CAEVehicleAudioEntity","GetVolForPlayerEngineSound",0x4F5D00,0,-1,128},
    {"CAEVehicleAudioEntity","UpdateParameters",0,0x862CEC,0,192},
    {"CAEVehicleAudioEntity","StartVehicleEngineSound",0x4F7F20,0,-1,64},
    {"CAEVehicleAudioEntity","CancelVehicleEngineSound",0x4F55C0,0,-1,96},
    {"CAEVehicleAudioEntity","StopGenericEngineSound",0x4F6320,0,-1,64},
    {"CAEVehicleAudioEntity","RequestNewPlayerCarEngineSound",0x4F7A50,0,-1,256},
    {"CAEVehicleAudioEntity","UpdateGenericVehicleSound",0x4FAD40,0,-1,96},
    {"CAEVehicleAudioEntity","JustWreckedVehicle",0x4FD0B0,0,-1,96},
    {"CAEVehicleAudioEntity","JustGotOutOfVehicleAsDriver",0x4FCF40,0,-1,96},
    {"CAEVehicleAudioEntity","JustGotInVehicleAsDriver",0x4F5700,0,-1,192},
    {"CAEVehicleAudioEntity","AddAudioEvent-0",0x4F6420,0,-1,768},
    {"CAEVehicleAudioEntity","AddAudioEvent-1",0x4F7580,0,-1,96},
    {"CAEVehicleAudioEntity","ProcessDummyStateTransition",0x4FCA10,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessDummyVehicleEngine",0x501480,0,-1,192},
    {"CAEVehicleAudioEntity","ProcessEngineDamage",0x4FAE20,0,-1,160},
    {"CAEVehicleAudioEntity","ProcessNitro",0x4FB070,0,-1,160},
    {"CAEVehicleAudioEntity","ProcessMovingParts",0x4FB260,0,-1,160},
};

constexpr int G2_AC=CAEVehicleAudioEntity::AE_SOUND_PLAYER_AC, G2_NEAR=CAEVehicleAudioEntity::AE_SOUND_AIRCRAFT_NEAR;
constexpr uintptr_t G2_UNDERWATER=0xC8132C;   // CWeather::UnderWaterness
inline float& G2UnderWater(){ return *reinterpret_cast<float*>(G2_UNDERWATER); }
inline void SavesG2(){ Save(G2_UNDERWATER,4); }
inline void StubsG2(){}

// ---------------------------------------------------------------------------------------------------------------------
// Utilidades de fixtures
// ---------------------------------------------------------------------------------------------------------------------
inline void G2Flag(uint8& b,uint8 mask,bool on){ b= on ? uint8(b|mask) : uint8(b&uint8(~mask)); }
// Valor entre los bordes dados (con probabilidad 1/odds) o uniforme en [lo, hi]
inline float G2Pick(Ctx& c,std::initializer_list<float> edges,float lo,float hi,uint32 odds=3){
    if(c.chance(odds))return *(edges.begin()+c.next()%edges.size());
    return c.uni(lo,hi);
}
inline float G2Next(float f){ return std::nextafter(f,1.0e9f); }
inline float G2Prev(float f){ return std::nextafter(f,-1.0e9f); }
// Relacion de velocidad (0..1) con los bordes de los umbrales del EXE (0.2 / 0.15 / 0.075 / 0.25 / 1.0001...)
inline float G2Ratio(Ctx& c){
    return G2Pick(c,{0.f,0.2f,G2Prev(0.2f),G2Next(0.2f),0.15f,G2Prev(0.15f),G2Next(0.15f),1.f,0.075f,0.25f,1.0001f,-0.05f,1.2f},-0.05f,1.1f,3);
}
// Relacion de fundido (0..1) con los bordes de las tablas por tramos y del umbral doble 0.99 / 0.65 / 0.75
inline float G2Fade(Ctx& c){
    return G2Pick(c,{0.f,0.3f,0.5f,0.7f,0.85f,0.99f,G2Prev(0.99f),G2Next(0.99f),1.f,1.0001f,0.65f,G2Next(0.65f),0.75f,G2Prev(0.75f),-0.1f,1.1f},-0.1f,1.1f,2);
}

// Vehiculo falso k: modelo, estado, matriz (o posicion propia), banderas, vida, remolque, conductor, vel. angular
inline void G2Veh(Ctx& c,int k){
    uint8* v=c.V(k);
    At<uint16>(v,0x22)= c.chance(10) ? uint16(457) : uint16(400+c.next()%212);
    v[0x36]=uint8((c.pick<int>({0,0,1,2,3,4,5,8})<<3)|2);   // CEntity: tipo 2 (vehiculo) en 3 bits bajos, estado en los 5 altos
    const CVector pe{c.uni(-300.f,300.f),c.uni(-300.f,300.f),c.uni(-5.f,100.f)};
    const CVector pm{c.uni(-300.f,300.f),c.uni(-300.f,300.f),c.uni(-5.f,100.f)};
    At<float>(v,4)=pe.x; At<float>(v,8)=pe.y; At<float>(v,0xC)=pe.z;
    if(c.chance(5)){ At<void*>(v,0x14)=nullptr; }
    else { At<void*>(v,0x14)=w.mat[k]; float* m=reinterpret_cast<float*>(w.mat[k]+0x30); m[0]=pm.x; m[1]=pm.y; m[2]=pm.z; }
    G2Flag(v[0x428],0x10,c.chance(2));    // bEngineOn
    G2Flag(v[0x42B],0x40,c.chance(5));    // bIsDrowning
    G2Flag(v[0x42B],0x20,c.chance(4));    // bAudioChangingGear
    At<float>(v,0x4C0)=c.uni(0.f,1000.f);
    At<void*>(v,0x4C8)= c.chance(4) ? static_cast<void*>(c.V(2)) : nullptr;
    At<void*>(v,0x460)= c.chance(4) ? nullptr : static_cast<void*>(w.ped[0]);
    At<float>(v,0x50)=c.uni(-1.2f,1.2f); At<float>(v,0x54)=c.uni(-1.2f,1.2f); At<float>(v,0x58)=c.uni(-1.2f,1.2f);
}

// Pool de sonidos + hora + entidad 0 sobre el vehiculo 0; el twin loop (m_SkidSound) queda como recien construido
inline CAEVehicleAudioEntity* G2Base(Ctx& c){
    c.SetTime(); c.FreshPool(); c.FillEntity(0,0);
    uint8* b=c.E(0);
    At<uint16>(b,0x20C)=0; At<void*>(b,0x224)=nullptr; At<void*>(b,0x228)=nullptr;   // m_SkidSound: m_IsInUse y m_Sounds[2]
    G2Veh(c,0);
    return c.Ent(0);
}
// Twin loop (m_SkidSound) a veces activo con sonidos preexistentes (solo lo tocan JustWrecked/JustGotOut)
inline void G2Twin(Ctx& c){
    uint8* b=c.E(0);
    if(c.chance(4)){
        At<uint16>(b,0x20C)=1;
        At<void*>(b,0x224)= c.chance(3) ? nullptr : static_cast<void*>(c.Pre(c.next()%NPRE));
        At<void*>(b,0x228)= c.chance(3) ? nullptr : static_cast<void*>(c.Pre(c.next()%NPRE));
    }
}
inline void G2SetType(CAEVehicleAudioEntity* e,int t){ e->m_AuSettings.VehicleAudioType=static_cast<eAEVehicleSoundType>(t); }
inline void G2Ent(Ctx& c){ c.out["ent"]=Hex(c.E(0),8)+Hex(c.E(0)+TEMP_SOUND_END,ENT_SIZE-TEMP_SOUND_END); }
inline int16 G2Bank(Ctx& c){ return c.chance(6) ? int16(-1) : int16(c.next()%100); }

// Estaticos de la clase (slots de motor dummy 0xB6B9A0, siguiente slot 0xB6B998): coherentes con los bancos de la entidad
inline void G2Statics(Ctx& c,CAEVehicleAudioEntity* e){
    G<int16>(0xB6B998)=int16(c.next()%10);
    for(int s=0;s<10;++s){
        int16 bank= c.chance(4) ? ((c.next()&1) ? At<int16>(e,0xDC) : At<int16>(e,0xDE)) : int16(c.next()%100);
        if(c.chance(12))bank=-1;
        G<int16>(0xB6B9A0+4*s)=bank;
        G<int16>(0xB6B9A2+4*s)= c.chance(3) ? int16(1+c.next()%3) : int16(0);
    }
}

// tVehicleParams (w.params, 0x4C) y transmision falsa: solo los campos que leen estas funciones
inline uint8* G2Params(Ctx& c){
    uint8* p=w.params; uint8* tr=w.tr[0]; uint8* v=c.V(0);
    float prev=0.f;
    for(int g=0;g<6;++g){   // tTransmissionGear: velocidad maxima / cambio arriba / cambio abajo (positivas y crecientes)
        prev+=c.uni(0.05f,0.4f);
        At<float>(tr,0xC*g)=prev; At<float>(tr,0xC*g+4)=prev*c.uni(0.5f,1.f); At<float>(tr,0xC*g+8)=prev*c.uni(0.2f,0.6f);
    }
    At<float>(tr,0x58)=c.uni(0.2f,3.f);     // m_MaxVelocity
    At<int32>(p,0)=0; At<int32>(p,4)=0;
    At<void*>(p,0x10)=v; At<void*>(p,0x14)=static_cast<void*>(tr);   // el EXE desreferencia la transmision sin comprobar (0x4F810E fld [eax+10h])
    const float sp=G2Pick(c,{-0.001f,G2Next(-0.001f),G2Prev(-0.001f),0.f,0.5f,-0.5f},-1.f,1.f,4);
    At<float>(p,0x1C)=sp;
    At<float>(p,0x24)=std::fabs(sp)*(c.chance(4) ? c.uni(0.5f,3.f) : 1.f);
    At<float>(p,0x28)=G2Pick(c,{0.f,0.2f,-0.2f,G2Next(0.2f),G2Prev(-0.2f),0.1f,-0.1f},-0.5f,0.5f,3);
    At<float>(p,0x2C)=c.uni(0.f,1.1f);
    At<float>(p,0x3C)=c.uni(-0.1f,1.1f);
    At<float>(p,0x40)=G2Pick(c,{0.f,5.f,G2Next(5.f),G2Prev(5.f),10.f},-1.f,12.f,4);
    return p;
}

// ---------------------------------------------------------------------------------------------------------------------
inline bool RunG2(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a;
    auto& out=c.out; auto& ret=c.ret;

    // --- Funciones puras de float sobre la entidad -------------------------------------------------------------------
    if(nm=="GetVolumeForDummyIdle"||nm=="GetFrequencyForDummyIdle"||nm=="GetVolumeForDummyRev"||nm=="GetFrequencyForDummyRev"){
        auto* e=G2Base(c); uint8* v=c.V(0);
        if(c.chance(4))At<uint16>(v,0x22)=457;   // MODEL_CADDY
        e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(c.pick<int>({1,2,2,2,0,3,10,1}));
        const float ratio=G2Ratio(c), fade=G2Fade(c);
        ret=F(CallThis<float>(a,e,ratio,fade));
        return true; }

    if(nm=="GetFreqForIdle"){
        auto* e=G2Base(c);
        const float r=G2Pick(c,{0.f,0.075f,0.15f,0.25f,1.f,1.0001f,G2Next(1.0001f),G2Prev(0.075f),G2Next(0.075f),-0.01f,1.5f,0.2f},-0.1f,1.3f,2);
        ret=F(CallThis<float>(a,e,r));
        return true; }

    if(nm=="GetFreqForPlayerEngineSound"||nm=="GetVolForPlayerEngineSound"){
        auto* e=G2Base(c); uint8* v=c.V(0); uint8* p=G2Params(c);
        if(c.chance(6))At<uint16>(v,0x22)=457;
        G2SetType(e,c.chance(3) ? 1 : 0);   // moto (tipo 1) o automovil
        At<float>(v,0x648)= c.chance(8) ? 0.f : c.uni(-2.f,2.f);   // angulo de inclinacion de la moto
        G2Flag(v[0x614],0x20,c.chance(2));                      // bPlayerBoost
        e->m_AuGear=uint8(c.next()%6); e->m_IsSingleGear=c.chance(3);
        e->m_CrzCount=c.uni(0.f,160.f);
        e->m_bNitroOnLastFrame=c.chance(2);
        e->m_CurrentNitroRatio=G2Pick(c,{0.f,1.f,G2Next(1.f),-0.0001f,0.0001f,0.5f},-0.2f,1.2f,3);
        const int st=c.pick<int>({1,2,3,4,5,1,2,3,4,5,2});
        ret=F(CallThis<float>(a,e,static_cast<void*>(p),int32(st)));
        return true; }

    // --- UpdateParameters (virtual, ranura 0 de la vtable 0x862CEC) --------------------------------------------------
    if(nm=="UpdateParameters"){
        auto* e=G2Base(c); uint8* v=c.V(0);
        const uint32 mode=c.next()%8;
        G2SetType(e,c.pick<int>({0,0,1,2,3,4,5,6,9,10}));
        CAESound* snd=c.Pre(c.next()%NPRE);
        int16 pos=int16(c.chance(3) ? -1 : int(c.next()%1000));
        uint8* sb=reinterpret_cast<uint8*>(snd);
        G2UnderWater()=G2Pick(c,{0.f,0.5f,G2Prev(0.5f),G2Next(0.5f),1.f},0.f,1.f,2);
        G2Veh(c,1);
        if(mode==0){ if(c.chance(3))snd=nullptr; }
        else if(mode==1){ // AE_BONNET_FLUBBER_FLUBBER: volumen por vel. angular del fisico del sonido
            At<int32>(sb,0xC)=93; At<void*>(sb,8)= c.chance(4) ? nullptr : static_cast<void*>(c.V(1)); }
        else if(mode==2){ At<int32>(sb,0xC)=99; if(c.chance(4))pos=-1; }   // AE_BOAT_HIT_WAVE
        else if(mode==3){ // sonido propio de la entidad que termina (curPlayPos == -1)
            pos=-1;
            CAESound* own[]{e->m_SurfaceSound,e->m_RoadNoiseSound,e->m_FlatTireSound,e->m_ReverseGearSound,e->m_HornSound,e->m_SirenSound,e->m_FastSirenSound,
                            e->m_EngineSounds[c.next()%e->m_EngineSounds.size()].Sound};
            CAESound* chosen=own[c.next()%std::size(own)];
            if(chosen)snd=chosen;
            else { snd=c.Pre(c.next()%NPRE); }
            if(c.chance(3))At<int32>(reinterpret_cast<uint8*>(snd),0xC)=c.pick<int>({93,99,5,-1}); }
        else if(mode==4){ // motor AC del jugador (coches y motos): intercambia las posiciones de reproduccion
            G2SetType(e,c.pick<int>({0,1,0,1,2,5}));
            e->m_EngineSounds[G2_AC].Sound=snd; if(pos==-1)pos=int16(c.next()%1000); }
        else if(mode==5){ // aeronave: sonido cercano (solo avion/hidroavion: el helicoptero llamaria a un clump real)
            G2SetType(e,c.chance(2) ? 5 : 6);
            e->m_EngineSounds[G2_NEAR].Sound=snd; if(pos==-1)pos=int16(c.next()%1000); }
        else if(mode==6){ G2SetType(e,4); }
        // modo 7: sonido ajeno y sin evento
        // Helicoptero: GetAircraftNearPosition llamaria a GetComponentWorldPosition (clump real): el sonido cercano nunca es el recibido
        if(e->m_AuSettings.VehicleAudioType==AE_AIRCRAFT_HELICOPTER && e->m_EngineSounds[G2_NEAR].Sound==snd)e->m_EngineSounds[G2_NEAR].Sound=nullptr;
        (void)v;
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,snd,int32(pos)); }
        G2Ent(c);
        if(snd)out["snd"]=Hex(snd,SND_SIZE);
        return true; }

    // --- Sonidos de motor ------------------------------------------------------------------------------------------
    if(nm=="StartVehicleEngineSound"){
        auto* e=G2Base(c);
        e->m_EventVolume=c.uni(-12.f,6.f);
        const int st=int(c.next()%12);
        const float sp=c.uni(0.3f,2.f), vol=c.uni(-40.f,6.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,int32(st),sp,vol); }
        G2Ent(c); return true; }

    if(nm=="CancelVehicleEngineSound"){
        auto* e=G2Base(c);
        // Solo indices 0..11: el EXE lee [ecx+70h] del puntero situado en el indice recibido antes de comprobar el rango (0x4F55E3)
        const int st= c.chance(2) ? G2_AC : int(c.next()%12);
        if(st==G2_AC && c.chance(4)){ e->m_EngineSounds[G2_AC].Sound=nullptr; }
        else if(st==G2_AC && c.chance(3)){ e->m_EngineSounds[G2_AC].Sound=c.Pre(4); }
        if(st==G2_AC && e->m_EngineSounds[G2_AC].Sound){   // longitud del sonido: <=0, pequeña, normal
            e->m_EngineSounds[G2_AC].Sound->m_Length=int16(c.pick<int>({-1,0,1,50,700,1000,2000})); }
        e->m_ACPlayPositionThisFrame=int16(c.chance(5) ? int(c.next()%3000) : (c.chance(4) ? -1 : int(c.next()%1000)));
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,int32(st)); }
        G2Ent(c); return true; }

    if(nm=="StopGenericEngineSound"){
        auto* e=G2Base(c);
        const int st=int(c.next()%12);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,int32(st)); }
        G2Ent(c); return true; }

    if(nm=="RequestNewPlayerCarEngineSound"){
        auto* e=G2Base(c);
        const int st=c.pick<int>({1,2,3,4,5,1,2,3,4,5,4,4});
        const float sp=c.uni(0.3f,2.f); const float vol= c.chance(4) ? -100.f : c.uni(-40.f,6.f);
        const uint32 now=G<uint32>(TIMER_MS);
        if(st==G2_AC){   // posicion de reproduccion en % segun el tiempo desde que se detuvo (SoundLength = 1000)
            static const uint32 deltas[]{0u,1u,10u,100u,500u,899u,900u,999u,1000u,1001u,5000u};
            const uint32 d=deltas[c.next()%std::size(deltas)];
            e->m_TimeACStopped= c.chance(8) ? now+d : now-d;
            e->m_ACPlayPercentWhenStopped=int16(c.pick<int>({0,1,10,11,50,79,80,81,99,100}));
            e->m_FramesAgoACLooped=int16(c.next()%20); }
        if(c.chance(6)){ At<int16>(e,0xE0)=-1; }   // sin slot de motor dummy
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,int32(st),sp,vol); }
        G2Ent(c); return true; }

    if(nm=="UpdateGenericVehicleSound"){
        auto* e=G2Base(c);
        const int st=int(c.next()%12);
        e->m_EventVolume=c.uni(-12.f,6.f);
        const int slot=c.pick<int>({5,19,40,7,8,16,-1}); const int bank=c.pick<int>({0x8F,0x8A,0x39,3,200,0});
        const int sfx=int(c.next()%40);
        const float sp=c.uni(0.3f,2.f), vol=c.uni(-40.f,6.f), ro=c.uni(0.5f,4.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,int32(st),int32(slot),int32(bank),int32(sfx),sp,vol,ro); }
        G2Ent(c); return true; }

    // --- Cambios de conductor / destruccion ------------------------------------------------------------------------
    if(nm=="JustWreckedVehicle"){
        auto* e=G2Base(c); uint8* b=c.E(0);
        G2SetType(e,int(c.next()%11));
        At<int8>(b,0x9B)=int8(c.next()%5);   // m_AuSettings.RadioType
        e->m_IsPlayerDriver=c.chance(2);
        G2Twin(c);
        e->m_SurfaceSoundType=static_cast<eSoundID>(c.chance(4) ? -1 : int(c.next()%20));
        e->m_RoadNoiseSoundType=static_cast<eSoundID>(c.chance(4) ? -1 : int(c.next()%20));
        e->m_FlatTireSoundType=static_cast<eSoundID>(c.chance(4) ? -1 : int(c.next()%20));
        e->m_ReverseGearSoundType=static_cast<eSoundID>(c.chance(4) ? -1 : int(c.next()%20));
        e->m_SurfaceSound= c.chance(2) ? nullptr : c.Pre(1);
        G<void*>(0xB6B98C)=w.ped[0]; G<void*>(0xB6B99C)=b;
        { auto P=MakePatches(G_STD|G_RADIO); CallThis<void>(a,e); }
        G2Ent(c); return true; }

    if(nm=="JustGotOutOfVehicleAsDriver"){
        auto* e=G2Base(c); uint8* b=c.E(0);
        G2SetType(e,int(c.next()%11));
        e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(c.pick<int>({6,6,1,0,10,2,5}));
        e->m_IsPlayerDriverAboutToExit=c.chance(2);
        G2Twin(c);
        e->m_SurfaceSoundType=static_cast<eSoundID>(c.chance(3) ? -1 : int(c.next()%20));
        e->m_ReverseGearSoundType=static_cast<eSoundID>(c.chance(3) ? -1 : int(c.next()%20));
        G<void*>(0xB6B990)=w.ped[0];
        (void)b;
        { auto P=MakePatches(G_STD); CallThis<void>(a,e); }
        G2Ent(c); return true; }

    if(nm=="JustGotInVehicleAsDriver"){
        auto* e=G2Base(c); uint8* b=c.E(0);
        G2SetType(e,c.pick<int>({0,0,1,2,3,4,5,6,7,8,9,10}));
        e->m_IsPlayerDriverAboutToExit=c.chance(2);
        if(c.chance(5))At<int16>(b,0xDC)=-1;   // sin banco dummy
        if(c.chance(5))At<int16>(b,0xDE)=-1;   // sin banco de jugador
        e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(c.pick<int>({1,1,1,0,2,6,10}));
        G2Statics(c,e);
        G<void*>(0xB6B990)=nullptr;
        { auto P=MakePatches(G_STD); CallThis<void>(a,e); }
        G2Ent(c); return true; }

    // --- Eventos ---------------------------------------------------------------------------------------------------
    if(nm=="AddAudioEvent-0"){
        auto* e=G2Base(c); uint8* v=c.V(0);
        G2SetType(e,c.pick<int>({0,0,1,2,3,5,9}));
        At<int8>(&e->m_AuSettings.DoorType,0)=int8(c.pick<int>({-1,0,1,2,3,4,0,1,2,3,4,5,100,-2}));
        int event;
        if(c.chance(7)) event=c.pick<int>({0,10,77,123,200,-5,93,94,95,98,101,104,105,106,112,116,117,118});
        else event=78+int(c.next()%45);
        if(c.chance(3)) event=c.pick<int>({99,119,120,121,122,102,103,107,108,109,110,111,113,114,115,100,99,121});
        const float p1= G2Pick(c,{0.f,100.f,449.99f,450.f,450.01f,500.f,799.9f,800.f,1000.f,2000.f,-5.f,600.f},0.f,1500.f,2);
        const uint32 now=G<uint32>(TIMER_MS);
        static const uint32 deltas[]{0u,1000u,1499u,1500u,1501u,3000u,100000u};
        const uint32 d=deltas[c.next()%std::size(deltas)];
        e->m_TimeSplashLastTriggered= c.chance(12) ? now+d : now-d;
        G2UnderWater()=G2Pick(c,{0.f,0.1f,G2Prev(0.5f),0.5f,G2Next(0.5f),0.75f,1.f},0.f,1.f,2);
        if(c.chance(10))At<int16>(e,0xE0)=-1;   // sin slot dummy (AE_TRAIN_CLACK)
        (void)v;
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,int32(event),p1); }
        G2Ent(c); return true; }

    if(nm=="AddAudioEvent-1"){
        auto* e=G2Base(c);
        G2Veh(c,1);
        const int event= c.chance(5) ? c.pick<int>({0,92,94,78,122}) : 93;
        void* phys= c.chance(8) ? nullptr : static_cast<void*>(c.V(1));
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,int32(event),phys); }
        G2Ent(c); return true; }

    // --- Dummies: transicion de estados y motor ----------------------------------------------------------------------
    if(nm=="ProcessDummyStateTransition"){
        auto* e=G2Base(c); uint8* p=G2Params(c);
        const int old=c.pick<int>({1,1,2,2,0,10,3,6,9});
        e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(old);
        const int nw=c.pick<int>({0,1,2,1,2,3,0,5});
        e->m_FadeIn=G2Pick(c,{0.f,0.9f,0.95f,0.96f,1.f,0.6f,0.7f},0.f,1.f,3);
        e->m_FadeOut=G2Pick(c,{0.f,0.59f,0.6f,0.64f,0.65f,G2Next(0.65f),0.7f,0.69f,0.70f,0.74f,0.75f,0.94f,0.95f,0.98f,1.f},0.f,1.f,2);
        e->m_EventVolume=c.uni(-12.f,6.f);
        const float ratio=G2Ratio(c);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,uint32(nw),ratio,static_cast<void*>(p)); }
        G2Ent(c); out["veh"]=Hex(c.V(0)+0x428,8); return true; }

    if(nm=="ProcessDummyVehicleEngine"){
        auto* e=G2Base(c); uint8* p=G2Params(c); uint8* v=c.V(0);
        G2Statics(c,e);
        e->m_State=static_cast<CAEVehicleAudioEntity::eAEState>(c.pick<int>({0,1,2,2,1,3,10,0,6}));
        if(c.chance(5))At<int16>(e,0xE0)=-1;                    // sin slot: RequestBankSlot
        At<int16>(e,0xDC)=G2Bank(c);
        if(c.chance(2))v[0x428]=uint8(v[0x428]|0x10);           // motor encendido
        else v[0x428]=uint8(v[0x428]&uint8(~0x10));
        if(c.chance(5)) v[0x36]=uint8((4<<3)|2);                // abandonado
        e->m_FadeIn=c.uni(0.f,1.f); e->m_FadeOut=c.uni(0.f,1.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        G2Ent(c); out["statics"]=Hex(reinterpret_cast<void*>(0xB6B998),0x30); return true; }

    // --- Dano de motor, nitro, piezas moviles ------------------------------------------------------------------------
    if(nm=="ProcessEngineDamage"){
        auto* e=G2Base(c); uint8* p=G2Params(c); uint8* v=c.V(0);
        At<float>(v,0x4C0)=G2Pick(c,{100.f,249.f,250.f,389.f,390.f,G2Prev(390.f),G2Next(390.f),459.f,460.f,461.f,649.f,650.f,G2Prev(650.f),G2Next(650.f),1000.f,0.f},0.f,1000.f,2);
        G2SetType(e,c.chance(3) ? c.pick<int>({1,2,4,5,9}) : 0);
        e->m_IsWreckedVehicle=c.chance(4);
        e->m_EventVolume=c.uni(-12.f,6.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        G2Ent(c); return true; }

    if(nm=="ProcessNitro"){
        auto* e=G2Base(c); uint8* p=G2Params(c); uint8* v=c.V(0);
        G2Flag(v[0x428],0x10,!c.chance(5)); G2Flag(v[0x42B],0x40,c.chance(8));   // motor encendido casi siempre, ahogado rara vez
        At<uint32>(v,0x38C)= c.chance(6) ? 0u : (0x80000u|(c.next()&0x7000u));    // handlingFlags.bNosInst
        At<float>(v,0x8A4)= c.chance(5) ? c.uni(0.f,2.f) : -c.uni(0.f,2.f);
        At<int16>(p,0x20)=int16(c.pick<int>({0,1,64,128,255,200,10,int(c.next()%256)}));
        e->m_CurrentNitroRatio=G2Pick(c,{-1.f,0.f,0.5f,1.f,0.1f},-1.f,1.f,3);
        e->m_EventVolume=c.uni(-12.f,6.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        G2Ent(c); return true; }

    if(nm=="ProcessMovingParts"){
        auto* e=G2Base(c); uint8* p=G2Params(c); uint8* v=c.V(0);
        static const uint16 models[]{443,486,406,524,592,530,443,486,406,592,530,400,457,500,900};
        At<uint16>(v,0x22)=models[c.next()%std::size(models)];
        const uint16 ang=uint16(c.next());
        const int d=c.pick<int>({-60,-31,-30,-29,-1,0,1,15,29,30,31,60,int(c.next()%200)-100});
        At<uint16>(v,0x86C)=ang; At<uint16>(v,0x86E)= c.chance(6) ? uint16(c.next()) : uint16(int(ang)-d);
        e->m_MovingPartSmoothedSpeed=G2Pick(c,{0.f,1.f,0.5f,0.2f,0.05f},0.f,1.f,3);
        if(c.chance(6))At<int16>(e,0xE0)=-1;
        e->m_EventVolume=c.uni(-12.f,6.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        G2Ent(c); return true; }

    return false;
}
}
