#pragma once
// Grupo 3 (CAEVehicleAudioEntity): bocina/sirena, derrape, pinchazo, ruido de carretera, marcha atras y lluvia (15 funciones).
//   GetHornState-, GetSirenState, PlayHornOrSiren, ProcessVehicleSirenAlarmHorn, PlaySkidSound, ProcessVehicleSkidding,
//   GetVehicleDriveWheelSkidValue, GetVehicleNonDriveWheelSkidValue, PlayFlatTyreSound, PlayReverseSound, PlayRoadNoiseSound,
//   ProcessVehicleFlatTyre, ProcessVehicleRoadNoise, ProcessReverseGear, ProcessRainOnVehicle.
//
// Decisiones de diseno:
//  - No hay falsos propios: el gestor de sonidos, el hardware, el azar y las referencias se simulan con el conjunto estandar (G_STD).
//    CAETwinLoopSoundEntity (0x4F28A0, 0x4F29A0, 0x4F2960, 0x4F2CB0), SurfaceInfos_c::IsAudio* (0x55EA50..0x55EB10) y
//    CDamageManager::GetWheelStatus (0x6C21B0) se dejan REALES (son funciones validadas que ejecutan por igual los dos lados): el
//    vehiculo falso lleva el DamageManager (+0x5A0) y los campos de superficie en sus desplazamientos reales; la tabla real de
//    superficies solo se LEE (se escogen identificadores segun sus banderas de audio, sin modificarla).
//  - Vehiculo falso (w.veh[0]): +4 posicion, +0x14 matriz, +0x22 modelo, +0x36 estado (bits 3..7), +0xBA superficie, +0x428.. banderas,
//    +0x45C alarma, +0x49C pedal, +0x4B4 marcha, +0x514 contador de bocina, +0x518 patron, +0x5A5.. estado de ruedas del DamageManager,
//    +0x65C/+0x730 ruedas de moto, +0x7F4 cuentas de ruedas, +0x804.. moto, +0x960.. automovil.
//  - Globales tocados: CTimer (0xB7CB40.., ya guardado), CAEWeatherAudioEntity::m_sfRainVolume (0x8CC30C), estaticos de la clase (guardados).
//  - m_tempSound (entidad +8..+0x7C) no se compara (el EXE lo usa como escritorio); los sonidos creados se comparan por RequestNewSound.
//  - Dominio: sin NaN, sin patron de bocina > 7 con contador < 44, identificadores de superficie solo de la tabla real,
//    transmision no nula cuando el EXE la desreferencia.
#include "Ctx.hpp"
#include "SurfaceInfos_c.h"   // g_surfaceInfos (0xB79538) y TOTAL_NUM_SURFACE_TYPES

namespace HookUnlockAudioLive {
inline const std::vector<Fn> kFnsG3{
    {"CAEVehicleAudioEntity","GetHornState-",0x4F61E0,0,-1,192},
    {"CAEVehicleAudioEntity","GetSirenState",0x4F62A0,0,-1,160},
    {"CAEVehicleAudioEntity","PlayHornOrSiren",0x4F99D0,0,-1,352},
    {"CAEVehicleAudioEntity","ProcessVehicleSirenAlarmHorn",0x5002C0,0,-1,320},
    {"CAEVehicleAudioEntity","PlaySkidSound",0x4F8360,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessVehicleSkidding",0x4F8F10,0,-1,448},
    {"CAEVehicleAudioEntity","GetVehicleDriveWheelSkidValue",0x4F5F30,0,-1,192},
    {"CAEVehicleAudioEntity","GetVehicleNonDriveWheelSkidValue",0x4F6000,0,-1,192},
    {"CAEVehicleAudioEntity","PlayFlatTyreSound",0x4F8650,0,-1,160},
    {"CAEVehicleAudioEntity","PlayReverseSound",0x4F87D0,0,-1,160},
    {"CAEVehicleAudioEntity","PlayRoadNoiseSound",0x4F84D0,0,-1,160},
    {"CAEVehicleAudioEntity","ProcessVehicleFlatTyre",0x4F8940,0,-1,224},
    {"CAEVehicleAudioEntity","ProcessVehicleRoadNoise",0x4F8B00,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessReverseGear",0x4F8DF0,0,-1,224},
    {"CAEVehicleAudioEntity","ProcessRainOnVehicle",0x4F92C0,0,-1,192},
};

constexpr uintptr_t G3_PAUSE_MS=0xB7CB7C, G3_TIMESCALE=0xB7CB64, G3_RAIN=0x8CC30C;

inline void SavesG3(){ Save(G3_RAIN,4); }
inline void StubsG3(){}

// ---------------------------------------------------------------------------------------------------------------------
// Utilidades de fixtures
// ---------------------------------------------------------------------------------------------------------------------
// Listas de identificadores de superficie de la tabla REAL segun las banderas de audio que consulta el EXE (solo lectura)
struct G3_Surf { std::vector<uint8> grass, wet, other; bool ready{}; };
inline G3_Surf g3s;
inline void G3_InitSurf(){
    if(g3s.ready)return;
    g3s.ready=true;
    for(uint32 id=0; id<static_cast<uint32>(TOTAL_NUM_SURFACE_TYPES); ++id){
        const bool grass=g_surfaceInfos.IsAudioGrass(id)||g_surfaceInfos.IsAudioLongGrass(id);
        const bool wet=g_surfaceInfos.IsAudioGravel(id)||g_surfaceInfos.IsAudioSand(id)||g_surfaceInfos.IsAudioWater(id);
        if(grass)g3s.grass.push_back(static_cast<uint8>(id)); else if(wet)g3s.wet.push_back(static_cast<uint8>(id)); else g3s.other.push_back(static_cast<uint8>(id));
    }
    if(g3s.other.empty())g3s.other.push_back(0);
    if(g3s.grass.empty())g3s.grass.push_back(g3s.other[0]);
    if(g3s.wet.empty())g3s.wet.push_back(g3s.other[0]);
}
inline uint8 G3_Surface(Ctx& c){
    G3_InitSurf();
    const uint32 k=c.next()%8;
    const std::vector<uint8>& l= k<2 ? g3s.grass : (k<4 ? g3s.wet : g3s.other);
    return l[c.next()%l.size()];
}

inline float G3_Speed(Ctx& c){
    switch(c.next()%12){
    case 0:  return 0.f;
    case 1:  return -0.f;
    case 2:  return c.uni(-0.05f,0.05f);
    case 3:  return c.uni(0.03f,0.05f);
    case 4:  return c.uni(-1.f,1.f);
    case 5:  return c.uni(0.f,0.3f);
    case 6:  return c.uni(-0.3f,0.f);
    case 7:  return c.uni(0.5f,3.f);
    case 8:  return c.uni(-3.f,-0.5f);
    case 9:  return c.uni(0.f,0.02f);
    case 10: return c.pick<float>({0.04f,-0.04f,1.f,-1.f,0.2f,-0.2f,0.5f,0.0001f});
    default: return c.uni(-1.5f,1.5f);
    }
}
inline float G3_Gas(Ctx& c){ return c.chance(3) ? c.pick<float>({0.f,0.4f,0.39f,0.41f,1.f,0.5f,0.9f}) : c.uni(0.f,1.f); }
inline float G3_Cnt(Ctx& c){ const uint32 k=c.next()%10; return k<2 ? 0.f : (k<3 ? -c.uni(0.f,1.f) : c.uni(0.01f,1.f)); }
inline int32 G3_WS(Ctx& c){ return c.pick<int32>({0,0,1,2,2,3,1,2}); }

// Preparacion comun: relojes, reserva de sonidos y entidad de audio con estado plausible
inline void G3_Prep(Ctx& c){
    c.SetTime();
    G<uint32>(G3_PAUSE_MS)= c.chance(8) ? 0xFFFFF000u+c.next()%0x1000u : 50000u+c.next()%2000000u;
    c.FreshPool();
    c.FillEntity(0,0);
}

// Vehiculo falso: modelo y posicion (con matriz o sin ella)
inline uint8* G3_Veh(Ctx& c,int model){
    uint8* v=c.V(0);
    At<int16>(v,0x22)=static_cast<int16>(model);
    if(c.chance(2)){
        At<void*>(v,0x14)=w.mat[0];
        for(int k=0;k<3;++k)At<float>(w.mat[0],0x30+4*k)=c.uni(-200.f,200.f);
    } else {
        for(int k=0;k<3;++k)At<float>(v,4+4*k)=c.uni(-200.f,200.f);
    }
    return v;
}
inline void G3_Status(Ctx& c,uint8* v,int status){ v[0x36]=static_cast<uint8>((status<<3)|static_cast<int>(c.next()&7u)); }

// Zonas de ruedas del automovil y de la moto (se rellenan las dos con valores independientes)
inline void G3_WheelZones(Ctx& c,uint8* v){
    for(int i=0;i<4;++i){
        At<float>(v,0x7F4+4*i)=G3_Cnt(c);
        v[0x5A5+i]=static_cast<uint8>(c.pick<int>({0,1,1,2,3}));
        At<int32>(v,0x968+4*i)=G3_WS(c);
    }
    v[0x960]=static_cast<uint8>(c.pick<int>({0,0,1,2,3,4,4}));
    v[0x961]=static_cast<uint8>(c.pick<int>({0,0,1,2,4}));
    v[0x962]=static_cast<uint8>(c.pick<int>({0,1,2,4}));
    At<float>(v,0x964)=G3_Gas(c);
    for(int i=0;i<2;++i){
        v[0x65C+i]=static_cast<uint8>(c.pick<int>({0,1,1,2,3}));
        At<float>(v,0x730+4*i)=G3_Cnt(c);
        At<int32>(v,0x80C+4*i)=G3_WS(c);
    }
    v[0x804]=static_cast<uint8>(c.pick<int>({0,0,1,2,2}));
    At<float>(v,0x808)=G3_Gas(c);
}

// Transmision falsa (w.tr[0]): tipo de traccion, velocidad maxima y maxima marcha atras
inline uint8* G3_Tr(uint8 driveType,float maxVel,float maxRev){
    uint8* t=w.tr[0];
    At<uint8>(t,0x48)=driveType; At<float>(t,0x58)=maxVel; At<float>(t,0x60)=maxRev;
    return t;
}
// tVehicleParams falso (w.params): tipo base, vehiculo, transmision y velocidad
inline uint8* G3_Params(int baseType,uint8* v,bool withTr,float speed){
    uint8* p=w.params;
    At<int32>(p,0)=baseType; At<int32>(p,4)=baseType;
    At<void*>(p,0x10)=v;
    At<void*>(p,0x14)= withTr ? static_cast<void*>(w.tr[0]) : nullptr;
    At<float>(p,0x1C)=speed;
    return p;
}

// Tipo de sonido y puntero de sonido coherentes (-1 sin sonido; en general sonido presente si el tipo es valido)
inline void G3_TypeSound(Ctx& c,eSoundID& type,CAESound*& snd,size_t pre,std::initializer_list<int> types){
    const uint32 k=c.next()%10;
    if(k<3){ type=-1; snd= c.chance(5) ? c.Pre(pre) : nullptr; }
    else { type=static_cast<eSoundID>(*(types.begin()+c.next()%types.size())); snd= c.chance(6) ? nullptr : c.Pre(pre); }
}
// Identificador de sonido a pedir: a veces el actual (rama "mismo tipo"), a veces -1, a veces otro
inline int32 G3_Sfx(Ctx& c,eSoundID cur,std::initializer_list<int> types){
    const uint32 k=c.next()%10;
    if(k<3)return static_cast<int32>(cur);
    if(k<5)return -1;
    if(k<9)return static_cast<int32>(*(types.begin()+c.next()%types.size()));
    return static_cast<int32>(c.next()%40u);
}
inline float G3_Vol(Ctx& c){ return c.chance(4) ? -100.f : c.uni(-60.f,10.f); }

// Estado de m_SkidSound (CAETwinLoopSoundEntity en entidad +0x184), activo o no, con sus dos voces nulas o preexistentes
inline void G3_Twin(Ctx& c){
    uint8* b=c.E(0);
    constexpr size_t T=0x184;
    const bool act=c.chance(2);
    const int16 a0=static_cast<int16>(c.pick<int>({0,6,8,24,3,10}));
    At<uint32>(b,T)=0x85F438;
    At<int16>(b,T+0x7C)=static_cast<int16>(c.pick<int>({0x13,0x13,0x13,0x28,0x11}));
    At<int16>(b,T+0x7E)=a0; At<int16>(b,T+0x80)=static_cast<int16>(a0+1);
    At<void*>(b,T+0x84)=b;
    At<int16>(b,T+0x88)= act ? static_cast<int16>(1) : static_cast<int16>(0);
    At<int16>(b,T+0x8A)=-1; At<int16>(b,T+0x8C)=-1;
    At<uint16>(b,T+0x8E)=200; At<uint16>(b,T+0x90)=1000;
    At<uint32>(b,T+0x94)=G<uint32>(G3_PAUSE_MS)+c.next()%3000u-1500u;
    b[T+0x98]=static_cast<uint8>(c.chance(2) ? 1 : 0);
    At<int16>(b,T+0x9A)=-1; At<int16>(b,T+0x9C)=-1;
    const uint32 ks=c.next()%6;
    At<void*>(b,T+0xA0)= ks<2 ? nullptr : static_cast<void*>(c.Pre(1));
    At<void*>(b,T+0xA4)= (ks==0||ks==2) ? nullptr : static_cast<void*>(c.Pre(2));
    At<uint32>(b,T+0x10)=0;   // m_tempSound.m_PhysicalEntity de la parte CAEAudioEntity
}

// Tipo de sonido de audio (VehicleAudioType) con sesgo hacia automovil/moto/BMX
inline void G3_AudioType(Ctx& c,CAEVehicleAudioEntity* e,std::initializer_list<int> types){
    e->m_AuSettings.VehicleAudioType=static_cast<eAEVehicleSoundType>(*(types.begin()+c.next()%types.size()));
}

// Bocina / sirena: ajustes y banderas de la entidad
inline void G3_HornEntity(Ctx& c,CAEVehicleAudioEntity* e){
    const int ht=c.pick<int>({0,0,1,2,3,4,5,6,7,8,9,9,-1,10,-5,100,127});
    e->m_AuSettings.HornType=static_cast<eAEVehicleHornType>(static_cast<int8>(ht));
    e->m_AuSettings.HornPitch=c.uni(0.5f,1.6f);
    e->m_AuSettings.EngineVolumeOffset=c.uni(-6.f,6.f);
    e->m_IsHornOn=c.chance(2); e->m_IsSirenOn=c.chance(2); e->m_IsFastSirenOn=c.chance(3);
    e->m_HornVolume=c.uni(-30.f,5.f);
    e->m_HasSiren=!c.chance(4); e->m_IsWreckedVehicle=c.chance(8);
    e->m_HornSound= c.chance(2) ? c.Pre(10) : nullptr;
    e->m_SirenSound= c.chance(2) ? c.Pre(11) : nullptr;
    e->m_FastSirenSound= c.chance(2) ? c.Pre(0) : nullptr;
}
// Vehiculo con alarma, estado y contador de bocina
inline void G3_HornVeh(Ctx& c,uint8* v,bool inPattern){
    const uint32 ka=c.next()%10;
    At<uint16>(v,0x45C)= ka<4 ? static_cast<uint16>(0) : (ka<6 ? static_cast<uint16>(0xFFFF) : static_cast<uint16>(1u+c.next()%0xFFFEu));
    const uint32 ks=c.next()%10;
    G3_Status(c,v, ks<3 ? 0 : (ks<5 ? 5 : (ks<6 ? 4 : static_cast<int>(c.next()%32u))));
    const uint32 kc=c.next()%20;
    uint32 cnt;
    if(kc<2)cnt=0u; else if(kc<12)cnt=1u+c.next()%43u; else if(kc<15)cnt=44u; else if(kc<18)cnt=45u+c.next()%200u; else if(kc==18)cnt=0xFFFFFFFFu-c.next()%4u; else cnt=0x80000000u+c.next()%100u;
    At<uint32>(v,0x514)=cnt;
    v[0x518]= (inPattern && cnt>=1u && cnt<44u) ? static_cast<uint8>(c.next()%8u) : static_cast<uint8>(c.next()&0xFFu);
}

inline bool RunG3(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a;
    auto& out=c.out; auto& ret=c.ret;
    CAEVehicleAudioEntity* const e=c.Ent(0);
    uint8* const eb=c.E(0);

    // --- GetHornState (void(bool*, tVehicleParams&) const): 0x4F61E0 ---
    if(nm=="GetHornState-"){
        G3_Prep(c);
        e->m_IsWreckedVehicle=c.chance(6);
        uint8* v=G3_Veh(c,400+static_cast<int>(c.next()%212u));
        G3_HornVeh(c,v,true);
        uint8* p=G3_Params(0,v,false,0.f);
        uint8* o=w.misc; *o=0x55;
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(o),static_cast<void*>(p)); }
        ret=static_cast<int>(*o);
        out["veh"]=Hex(v+0x514,8);
        return true; }

    // --- GetSirenState: 0x4F62A0 ---
    if(nm=="GetSirenState"){
        G3_Prep(c);
        e->m_IsWreckedVehicle=c.chance(8); e->m_HasSiren=!c.chance(4);
        const int model= c.chance(4) ? 423 : 400+static_cast<int>(c.next()%212u);
        uint8* v=G3_Veh(c,model);
        v[0x42D]=static_cast<uint8>((c.chance(4) ? 0u : 0x80u)|(c.next()&0x7Fu));
        const uint32 ks=c.next()%8;
        G3_Status(c,v, ks<3 ? 4 : (ks<5 ? 0 : (ks<6 ? 5 : static_cast<int>(c.next()%32u))));
        At<uint32>(v,0x514)= c.chance(3) ? 0u : 1u+c.next()%60u;
        uint8* p=G3_Params(0,v,false,0.f);
        uint8* so=w.misc; uint8* fo=w.misc+1;
        *so=static_cast<uint8>(c.next()&1u); *fo=static_cast<uint8>(c.next()&1u);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(so),static_cast<void*>(fo),static_cast<void*>(p)); }
        out["siren"]=static_cast<int>(*so); out["fast"]=static_cast<int>(*fo);
        return true; }

    // --- PlayHornOrSiren: 0x4F99D0 ---
    if(nm=="PlayHornOrSiren"){
        G3_Prep(c);
        G3_HornEntity(c,e);
        const int model= c.chance(4) ? 423 : 400+static_cast<int>(c.next()%212u);
        uint8* v=G3_Veh(c,model);
        uint8* p=G3_Params(0,v,false,0.f);
        const bool h=c.chance(2)==false, s=c.chance(2), f=c.chance(3);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,h,s,f,static_cast<void*>(p)); }
        out["ent"]=Hex(eb+0xA8,0x40);
        return true; }

    // --- ProcessVehicleSirenAlarmHorn: 0x5002C0 ---
    if(nm=="ProcessVehicleSirenAlarmHorn"){
        G3_Prep(c);
        G3_HornEntity(c,e);
        const int model= c.chance(4) ? 423 : 400+static_cast<int>(c.next()%212u);
        uint8* v=G3_Veh(c,model);
        v[0x42D]=static_cast<uint8>((c.chance(3) ? 0u : 0x80u)|(c.next()&0x7Fu));
        G3_HornVeh(c,v,true);
        const uint32 now=G<uint32>(TIMER_MS);
        if(c.chance(2)){   // contador relativo al reloj (rama de alarma)
            At<uint16>(v,0x45C)= c.chance(5) ? static_cast<uint16>(0x1234) : static_cast<uint16>(1u+c.next()%0xFFFEu);
            if(v[0x36]>>3==5)G3_Status(c,v,3);
            const uint32 k=c.next()%8;
            At<uint32>(v,0x514)= k==0 ? 0u : (k==1 ? now-100u : (k==2 ? now+100u : (k==3 ? now+1000u : (k==4 ? now : (k==5 ? now+375u : (k==6 ? now+374u : now-5000u))))));
        }
        uint8* p=G3_Params(0,v,false,0.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        out["ent"]=Hex(eb+0xBC,0x10); out["veh"]=Hex(v+0x514,8);
        return true; }

    // --- PlaySkidSound: 0x4F8360 ---
    if(nm=="PlaySkidSound"){
        G3_Prep(c);
        G3_Veh(c,400+static_cast<int>(c.next()%212u));
        G3_Twin(c);
        e->m_SurfaceSoundType=static_cast<eSoundID>(c.pick<int>({-1,-1,0,6,8,24,3}));
        const int32 sfx=G3_Sfx(c,e->m_SurfaceSoundType,{0,6,8,24});
        const float speed=c.uni(0.4f,2.5f), vol=G3_Vol(c);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,sfx,speed,vol); }
        out["skid"]=Hex(eb+0x154,0x10); out["twin"]=Hex(eb+0x184,0xA8);
        return true; }

    // --- ProcessVehicleSkidding: 0x4F8F10 ---
    if(nm=="ProcessVehicleSkidding"){
        G3_Prep(c);
        const uint32 kt=c.next()%10;
        const int base= kt<6 ? 0 : (kt<9 ? 9 : c.pick<int>({1,2,3,4,5,6,7,8,10,11}));
        G3_AudioType(c,e,{0,0,0,1,1,2,2,4,5,9,3,6,7,8,10});
        uint8* v=G3_Veh(c,400+static_cast<int>(c.next()%212u));
        v[0xBA]=G3_Surface(c);
        G3_WheelZones(c,v);
        if(c.chance(3)){ At<int32>(v,0x968+4)=2; if(c.chance(2))At<int32>(v,0x968+12)=2; At<int32>(v,0x80C+4)=2; }   // trasera derrapando
        static const uint8 drives[]{'4','F','R'};
        const uint32 kd=c.next()%20;
        const uint8 drive= kd<15 ? drives[kd%3] : static_cast<uint8>(c.next()&0xFFu);
        G3_Tr(drive,c.uni(5.f,150.f),c.uni(1.f,60.f));
        uint8* p=G3_Params(base,v,true,G3_Speed(c));
        G<float>(G3_TIMESCALE)=c.pick<float>({1.f,1.f,1.f,0.f,0.5f,2.f,0.25f});
        G3_Twin(c);
        e->m_SurfaceSoundType=static_cast<eSoundID>(c.pick<int>({-1,0,6,8,24,24,3}));
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        out["skid"]=Hex(eb+0x154,0x10); out["twin"]=Hex(eb+0x184,0xA8);
        return true; }

    // --- GetVehicleDriveWheelSkidValue: 0x4F5F30 (veh, wheelState, gasPedalAudioRevs, tr, speed) ---
    if(nm=="GetVehicleDriveWheelSkidValue"){
        G3_Prep(c);
        uint8* v=G3_Veh(c,400+static_cast<int>(c.next()%212u));
        uint8* t=G3_Tr('F',c.uni(5.f,150.f),c.uni(1.f,60.f));
        const int32 ws= c.i<56 ? static_cast<int32>(c.i%7)-2 : (c.chance(10) ? static_cast<int32>(c.next()%11u)-5 : G3_WS(c));
        const float gas=G3_Gas(c), speed=G3_Speed(c);
        const float r=CallThis<float>(a,e,static_cast<void*>(v),ws,gas,static_cast<void*>(t),speed);
        ret=F(r);
        return true; }

    // --- GetVehicleNonDriveWheelSkidValue: 0x4F6000 (veh, wheelState, tr, velocity); usa VehicleAudioType (BMX) ---
    if(nm=="GetVehicleNonDriveWheelSkidValue"){
        G3_Prep(c);
        G3_AudioType(c,e,{0,1,2,2,2,9,4,5});
        uint8* v=G3_Veh(c,400+static_cast<int>(c.next()%212u));
        uint8* t=G3_Tr('R',c.uni(5.f,150.f),c.uni(1.f,60.f));
        const int32 ws= c.i<56 ? static_cast<int32>(c.i%7)-2 : (c.chance(10) ? static_cast<int32>(c.next()%11u)-5 : G3_WS(c));
        const float speed=G3_Speed(c);
        const float r=CallThis<float>(a,e,static_cast<void*>(v),ws,static_cast<void*>(t),speed);
        ret=F(r);
        return true; }

    // --- PlayFlatTyreSound / PlayReverseSound / PlayRoadNoiseSound (sfx, speed, volume) ---
    if(nm=="PlayFlatTyreSound"){
        G3_Prep(c);
        G3_Veh(c,400+static_cast<int>(c.next()%212u));
        G3_TypeSound(c,e->m_FlatTireSoundType,e->m_FlatTireSound,8,{4,3,10});
        const int32 sfx=G3_Sfx(c,e->m_FlatTireSoundType,{4});
        const float speed=c.uni(0.4f,2.5f), vol=G3_Vol(c);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,sfx,speed,vol); }
        out["ent"]=Hex(eb+0x164,0x08);
        return true; }
    if(nm=="PlayReverseSound"){
        G3_Prep(c);
        G3_Veh(c,400+static_cast<int>(c.next()%212u));
        G3_TypeSound(c,e->m_ReverseGearSoundType,e->m_ReverseGearSound,9,{19,20,5});
        const int32 sfx=G3_Sfx(c,e->m_ReverseGearSoundType,{19,20});
        const float speed=c.uni(0.4f,2.5f), vol=G3_Vol(c);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,sfx,speed,vol); }
        out["ent"]=Hex(eb+0x16C,0x08);
        return true; }
    if(nm=="PlayRoadNoiseSound"){
        G3_Prep(c);
        G3_Veh(c,400+static_cast<int>(c.next()%212u));
        G3_TypeSound(c,e->m_RoadNoiseSoundType,e->m_RoadNoiseSound,7,{21,22,0,5});
        const int32 sfx=G3_Sfx(c,e->m_RoadNoiseSoundType,{21,22});
        const float speed=c.uni(0.4f,2.5f), vol=G3_Vol(c);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,sfx,speed,vol); }
        out["ent"]=Hex(eb+0x15C,0x08);
        return true; }

    // --- ProcessVehicleFlatTyre: 0x4F8940 ---
    if(nm=="ProcessVehicleFlatTyre"){
        G3_Prep(c);
        G3_AudioType(c,e,{0,0,0,0,1,1,2,9,9,9,3,4,5,6,7,8,10,-1,-5,11});
        const bool caddy= e->m_AuSettings.VehicleAudioType==AE_SPECIAL && c.chance(2);
        uint8* v=G3_Veh(c, caddy ? 457 : 400+static_cast<int>(c.next()%212u));
        G3_WheelZones(c,v);
        if(c.chance(2)){ const int i=static_cast<int>(c.next()%4u); v[0x5A5+i]=1; At<float>(v,0x7F4+4*i)=c.uni(0.1f,1.f); }
        if(c.chance(3)){ const int i=static_cast<int>(c.next()%2u); v[0x65C+i]=1; At<float>(v,0x730+4*i)=c.uni(0.1f,1.f); }
        G3_Tr('F',c.uni(1.f,150.f),c.uni(1.f,60.f));
        const float sp=G3_Speed(c)*c.pick<float>({1.f,1.f,10.f,50.f});
        uint8* p=G3_Params(0,v,true,sp);
        G3_TypeSound(c,e->m_FlatTireSoundType,e->m_FlatTireSound,8,{4,3,10});
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        out["ent"]=Hex(eb+0x164,0x08);
        return true; }

    // --- ProcessVehicleRoadNoise: 0x4F8B00 ---
    if(nm=="ProcessVehicleRoadNoise"){
        G3_Prep(c);
        const uint32 kt=c.next()%10;
        const int base= kt<5 ? 0 : (kt<8 ? 9 : c.pick<int>({1,2,3,4,5,6,7,8,10,11}));
        const uint32 km=c.next()%8;
        const int model= km<2 ? c.pick<int>({435,450,584,591}) : 400+static_cast<int>(c.next()%212u);
        uint8* v=G3_Veh(c,model);
        v[0xBA]=G3_Surface(c);
        v[0x960]=static_cast<uint8>(c.pick<int>({0,0,1,2,3,4}));
        v[0x804]=static_cast<uint8>(c.pick<int>({0,0,1,2,2}));
        const bool hasTr=!c.chance(4);
        G3_Tr('F',c.uni(1.f,150.f),c.uni(1.f,60.f));
        const float sp=G3_Speed(c)*c.pick<float>({1.f,1.f,0.01f,0.1f,5.f});
        uint8* p=G3_Params(base,v,hasTr,sp);
        G3_TypeSound(c,e->m_RoadNoiseSoundType,e->m_RoadNoiseSound,7,{21,22,0,5});
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        out["ent"]=Hex(eb+0x15C,0x08);
        return true; }

    // --- ProcessReverseGear: 0x4F8DF0 (solo automovil) ---
    if(nm=="ProcessReverseGear"){
        G3_Prep(c);
        uint8* v=G3_Veh(c,400+static_cast<int>(c.next()%212u));
        v[0x428]=static_cast<uint8>((c.chance(5) ? 0u : 0x10u)|(c.next()&0xEFu));
        const uint32 kg=c.next()%10;
        At<float>(v,0x49C)= kg<3 ? -c.uni(0.f,1.f) : (kg<5 ? 0.f : (kg<6 ? -0.f : c.uni(0.01f,1.f)));
        v[0x4B4]=static_cast<uint8>(c.pick<int>({0,0,1,2,3,4,5}));
        v[0x961]=static_cast<uint8>(c.pick<int>({0,0,1,2,4}));
        v[0x962]=static_cast<uint8>(c.pick<int>({0,1,2,4}));
        At<float>(v,0x964)= c.chance(4) ? -c.uni(0.f,1.f) : G3_Gas(c);
        G3_Tr('R',c.uni(5.f,150.f),c.uni(0.5f,60.f));
        const float sp=G3_Speed(c)*c.pick<float>({1.f,1.f,5.f,20.f});
        uint8* p=G3_Params(0,v,true,sp);
        G3_TypeSound(c,e->m_ReverseGearSoundType,e->m_ReverseGearSound,9,{19,20,5});
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        out["ent"]=Hex(eb+0x16C,0x08); out["veh"]=Hex(v+0x960,0x0C);
        return true; }

    // --- ProcessRainOnVehicle: 0x4F92C0 ---
    if(nm=="ProcessRainOnVehicle"){
        G3_Prep(c);
        G3_Veh(c,400+static_cast<int>(c.next()%212u));
        G<float>(G3_RAIN)=c.pick<float>({-100.f,-100.f,-99.9f,-100.5f,-50.f,-10.f,0.f,5.f,-150.f,-30.f});
        e->m_RainDropCounter=static_cast<int16>(c.pick<int>({0,1,2,2,3,4,10,-1,-3,32766,32767,-32768}));
        uint8* p=G3_Params(0,c.V(0),false,0.f);
        { auto P=MakePatches(G_STD); CallThis<void>(a,e,static_cast<void*>(p)); }
        out["cnt"]=static_cast<int>(e->m_RainDropCounter);
        return true; }

    return false;
}
}
