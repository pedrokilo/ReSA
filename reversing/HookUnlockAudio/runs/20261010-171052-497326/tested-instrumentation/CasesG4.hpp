#pragma once
// Grupo 4: aeronaves de CAEVehicleAudioEntity (15 funciones): PlayAircraftSound, GetAircraftNearPosition, ProcessAircraft, helices
// (jugador / dummy / IA), helicopteros (jugador / dummy / IA), hidroavion (jugador / dummy), jets (jugador / dummy) y los dummies RC.
// Mundo falso: entidad de audio w.ent[0], vehiculo w.veh[0] (campos reales del EXE: matriz +0x14, modelo +0x22, estado +0x36, grabacion
// +0x424, banderas +0x428, pedales +0x49C/+0x4A0, salud +0x4C0, +0x5A4, rotor +0x84C, tren +0x960, helice +0x9C4), tVehicleParams w.params,
// camara real (TheCamera, posicion en 0xB6F02C o en su matriz) y tabla de ranuras dummy 0xB6B9A0 (todo guardado/restaurado).
// Falsos propios (G_X4, direccion del EXE + simbolo de la DLL): GetAccelAndBrake, JustWreckedVehicle, CancelVehicleEngineSound, RequestBankSlot.
// GetComponentWorldPosition (virtual del vehiculo) se atiende con una vtable falsa. Tipos de traza T_GROUP+64..T_GROUP+68.
// No se prueban: PlayAircraftSound con el tipo 6 (agua) ni fuera de 1..7: el EXE llama a RequestNewSound con un CAESound local SIN inicializar.
#include "Ctx.hpp"

namespace HookUnlockAudioLive {
constexpr uint32 G4_T_GETAB=T_GROUP+64, G4_T_WRECK=T_GROUP+65, G4_T_CANCEL=T_GROUP+66, G4_T_SLOT=T_GROUP+67, G4_T_COMP=T_GROUP+68;

// Parametros de los falsos (los fija cada caso)
struct G4State { int16 accel, brake; std::array<int32,4> slotSeq; uint32 slotIdx; CVector comp; };
inline G4State g4{};
inline std::array<uintptr_t,48> g4Vt{};   // vtable falsa del vehiculo (solo la ranura 0x64/4 = GetComponentWorldPosition)

// CAEVehicleAudioEntity::GetAccelAndBrake (0x4F5080, ret 4): deja los pedales (palabras de 16 bits en +0x20 / +0x22)
inline void __fastcall G4_GetAB(void* self,void*,void* vp){ if(!Mine())return; Push(G4_T_GETAB,Id(self),Id(vp),uint32(uint16(g4.accel)),uint32(uint16(g4.brake))); At<int16>(vp,0x20)=g4.accel; At<int16>(vp,0x22)=g4.brake; }
// CAEVehicleAudioEntity::JustWreckedVehicle (0x4FD0B0, sin argumentos)
inline void __fastcall G4_Wrecked(void* self,void*){ if(!Mine())return; Push(G4_T_WRECK,Id(self)); }
// CAEVehicleAudioEntity::CancelVehicleEngineSound (0x4F55C0, ret 4): olvida el sonido (puntero a nulo), como el original
inline void __fastcall G4_Cancel(void* self,void*,int32 st){ if(!Mine())return; const int16 s=int16(st); Push(G4_T_CANCEL,Id(self),uint32(uint16(s))); if(s>=0&&s<12)At<uint32>(self,0xE8+8u*uint32(s))=0; }
// CAEVehicleAudioEntity::RequestBankSlot (0x4F4D10, cdecl estatica)
inline int __cdecl G4_ReqSlot(int bank){ if(!Mine())return -1; const int32 r=g4.slotSeq[g4.slotIdx++%4]; Push(G4_T_SLOT,uint32(uint16(bank)),uint32(r)); return r; }
// CVehicle::GetComponentWorldPosition (virtual, vtable +0x64, ret 8)
inline void __fastcall G4_CompPos(void* self,void*,int32 comp,CVector* out){ if(!Mine())return; Push(G4_T_COMP,Id(self),uint32(comp)); *out=g4.comp; }

inline void SavesG4(){
    Save(0xB6F028,0x40);   // TheCamera: vtable, placement (posicion en 0xB6F02C) y puntero a la matriz (0xB6F03C)
    Save(0xC8132C,4);      // CWeather::UnderWaterness
}
inline void StubsG4(){
    AddHookStub(G_X4,0x4F5080,"CAEVehicleAudioEntity","GetAccelAndBrake",FnPtr(&G4_GetAB));
    AddHookStub(G_X4,0x4FD0B0,"CAEVehicleAudioEntity","JustWreckedVehicle",FnPtr(&G4_Wrecked));
    AddHookStub(G_X4,0x4F55C0,"CAEVehicleAudioEntity","CancelVehicleEngineSound",FnPtr(&G4_Cancel));
    AddHookStub(G_X4,0x4F4D10,"CAEVehicleAudioEntity","RequestBankSlot",FnPtr(&G4_ReqSlot));
    g4Vt[25]=reinterpret_cast<uintptr_t>(FnPtr(&G4_CompPos));
}

inline const std::vector<Fn> kFnsG4{
    {"CAEVehicleAudioEntity","PlayAircraftSound",0x4F93C0,0,-1,256},
    {"CAEVehicleAudioEntity","GetAircraftNearPosition",0x4F96A0,0,-1,128},
    {"CAEVehicleAudioEntity","ProcessAircraft",0x501C50,0,-1,384},
    {"CAEVehicleAudioEntity","ProcessPlayerProp",0x4FD290,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessDummyProp",0x4FD8F0,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessAIProp",0x4FDFD0,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessPlayerHeli",0x4FE420,0,-1,192},
    {"CAEVehicleAudioEntity","ProcessDummyHeli",0x4FE940,0,-1,192},
    {"CAEVehicleAudioEntity","ProcessAIHeli",0x4FEE20,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessPlayerSeaPlane",0x4FF320,0,-1,192},
    {"CAEVehicleAudioEntity","ProcessDummySeaPlane",0x4FF7C0,0,-1,128},
    {"CAEVehicleAudioEntity","ProcessPlayerJet",0x501650,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessDummyJet",0x501960,0,-1,256},
    {"CAEVehicleAudioEntity","ProcessDummyRCPlane",0x4FA7C0,0,-1,192},
    {"CAEVehicleAudioEntity","ProcessDummyRCHeli",0x4FAA80,0,-1,192},
};

// ---------------------------------------------------------------------------------------------------------------------
// Fixtures
// ---------------------------------------------------------------------------------------------------------------------
struct G4Env { uint8* eb; CAEVehicleAudioEntity* e; uint8* v; uint8* vp; CVector pos; };

// multiplo de 0.25 (exacto en float: las distancias 48 y 128 a la camara son exactas)
inline float G4Q(Ctx& c,float lo,float hi){ return std::floor(c.uni(lo,hi)*4.f)/4.f; }
inline CVector G4Unit(Ctx& c){
    for(;;){ const CVector v{c.uni(-1.f,1.f),c.uni(-1.f,1.f),c.uni(-1.f,1.f)}; const float l2=v.x*v.x+v.y*v.y+v.z*v.z;
        if(l2>0.05f&&l2<=1.f){ const float l=std::sqrt(l2); return CVector{v.x/l,v.y/l,v.z/l}; } }
}
// Matriz del vehiculo: ejes (derecha +0, delante +0x10, arriba +0x20) y posicion (+0x30); el resto, aleatorio
inline void G4Mat(Ctx& c,uint8* m,const CVector& pos){
    c.fill(m,0x60);
    CVector r{1.f,0.f,0.f}, f{0.f,1.f,0.f}, u{0.f,0.f,1.f};
    const uint32 mode=c.next()%8;
    if(mode==1){ r=CVector{0.f,1.f,0.f}; f=CVector{-1.f,0.f,0.f}; }
    else if(mode>=2){
        f=G4Unit(c); const CVector t=G4Unit(c);
        CVector x{f.y*t.z-f.z*t.y,f.z*t.x-f.x*t.z,f.x*t.y-f.y*t.x};
        const float l2=x.x*x.x+x.y*x.y+x.z*x.z;
        if(l2>1e-4f){ const float l=std::sqrt(l2); r=CVector{x.x/l,x.y/l,x.z/l}; u=CVector{r.y*f.z-r.z*f.y,r.z*f.x-r.x*f.z,r.x*f.y-r.y*f.x}; }
        else f=CVector{0.f,1.f,0.f};
    }
    auto set=[&](size_t o,const CVector& v){ At<float>(m,o)=v.x; At<float>(m,o+4)=v.y; At<float>(m,o+8)=v.z; };
    set(0,r); set(0x10,f); set(0x20,u); set(0x30,pos);
}
// Camara del juego: posicion a una distancia dada de la entidad (0, <48, 48, entre 48 y 128, 128, >128), en el placement o en una matriz
inline void G4Cam(Ctx& c,const CVector& ent,uint32 mode){
    float d;
    switch(mode%9){ case 0:d=0.f;break; case 1:d=c.uni(0.5f,47.5f);break; case 2:d=48.f;break; case 3:d=c.uni(48.01f,127.9f);break;
                    case 4:d=128.f;break; case 5:d=c.uni(128.1f,700.f);break; case 6:d=48.0004f;break; case 7:d=c.uni(60.f,120.f);break; default:d=c.uni(5.f,300.f); }
    const CVector dir=(mode%9==2||mode%9==4) ? CVector{1.f,0.f,0.f} : G4Unit(c);
    const CVector cam{ent.x-dir.x*d,ent.y-dir.y*d,ent.z-dir.z*d};
    uint8* const tc=reinterpret_cast<uint8*>(0xB6F028);
    if(c.chance(3)){ G4Mat(c,w.mat[1],cam); At<uint32>(tc,0x14)=uint32(reinterpret_cast<uintptr_t>(w.mat[1])); }
    else { At<uint32>(tc,0x14)=0; At<float>(tc,4)=cam.x; At<float>(tc,8)=cam.y; At<float>(tc,12)=cam.z; }
}
inline float G4Pedal(Ctx& c){ const uint32 k=c.next()%10; return k<3 ? 0.f : (k<5 ? 1.f : (k==5 ? c.uni(-1.f,0.f) : c.uni(0.f,1.f))); }
inline float G4Health(Ctx& c){
    static const float h[]{249.99f,250.f,389.99f,390.f,459.99f,460.f,649.99f,650.f,0.f,1000.f,100.f,800.f};
    return c.chance(3) ? h[c.next()%std::size(h)] : c.uni(0.f,1000.f);
}
// velocidades (helice, rotor): 0, justo el tope, por encima y valores intermedios
inline float G4Spd(Ctx& c,float top){
    const uint32 k=c.next()%10;
    return k==0 ? 0.f : (k==1 ? top : (k==2 ? c.uni(top,top*1.8f) : (k==3 ? top*0.2f : c.uni(0.f,top))));
}
inline int16 G4Model(Ctx& c,int kind){
    static const int16 jets[]{519,520,577,592}, props[]{476,511,512,513,553,593,539,464,501}, helis[]{417,425,447,469,487,488,497,548,563,501}, seas[]{460,511,476};
    switch(kind){
    case 3: return c.chance(10) ? int16(400+c.next()%212) : jets[c.next()%4];
    case 1: case 5: return helis[c.next()%std::size(helis)];
    case 2: return seas[c.next()%std::size(seas)];
    case 6: return int16(400+c.next()%212);
    default: return props[c.next()%std::size(props)];
    }
}

// kind: 0 helice, 1 helicoptero, 2 hidroavion, 3 jet, 4 RC avion, 5 RC helicoptero, 6 generico
inline G4Env G4Prep(Ctx& c,int kind,bool nullMat){
    G4Env g{};
    c.SetTime(); c.FreshPool(); c.FillEntity(0,0);
    g.eb=c.E(0); g.e=c.Ent(0); g.v=c.V(0); g.vp=w.params;
    { const uint32 m=c.next()%10; fk.bankMode= m<7 ? 0 : (m==7 ? 1 : 2); fk.bankBits=c.next(); }
    const uint32 now=G<uint32>(TIMER_MS);

    // --- entidad de audio
    const uint32 at= (kind==1||kind==5) ? 4u : (kind==2 ? 6u : (kind==6 ? 4u+c.next()%3u : 5u));
    g.e->m_AuSettings.VehicleAudioType=static_cast<eAEVehicleSoundType>(at);
    At<int16>(&g.e->m_PlayerEngineBank,0)= (kind==1 && c.chance(3)) ? int16(12) : (c.chance(10) ? int16(-1) : int16(c.next()%100));
    At<int16>(&g.e->m_DummyEngineBank,0)= c.chance(8) ? int16(-1) : int16(c.next()%100);
    At<int16>(&g.e->m_DummySlot,0)= c.chance(4) ? int16(-1) : int16(7+c.next()%10);
    {   // tabla de ranuras dummy (0xB6B9A0): banco y contador de cada una
        auto* tab=reinterpret_cast<int16*>(0xB6B9A0);
        for(int i=0;i<10;++i){ tab[2*i]= c.chance(3) ? At<int16>(&g.e->m_DummyEngineBank,0) : int16(c.next()%100); tab[2*i+1]=int16(c.next()%4); }
        const int sl=At<int16>(&g.e->m_DummySlot,0);
        if(sl>=7&&sl<17&&c.chance(2))tab[2*(sl-7)]=At<int16>(&g.e->m_DummyEngineBank,0);
    }
    G<uintptr_t>(0xB6B990)= c.chance(5) ? uintptr_t(0) : reinterpret_cast<uintptr_t>(w.ped[0]);   // s_pPlayerDriver
    g.e->m_StalledCount=int16(c.next()%18);
    g.e->m_SwapStalledTime= c.chance(4) ? now : now+uint32(int32(c.next()%2400u)-1200);
    g.e->m_IsSilentStalled=c.chance(2);
    g.e->m_CurrentRotorFrequency= c.chance(5) ? -1.f : c.uni(0.2f,1.4f);
    At<float>(g.eb,0x230)=c.uni(-150.f,30.f);
    g.e->m_IsWreckedVehicle=c.chance(10);

    // --- vehiculo
    g.pos=CVector{G4Q(c,-2000.f,2000.f),G4Q(c,-2000.f,2000.f),G4Q(c,0.f,100.f)};
    c.fill(g.v,VEH_SIZE);
    G4Mat(c,w.mat[0],g.pos);
    At<uint32>(g.v,0)=uint32(reinterpret_cast<uintptr_t>(g4Vt.data()));
    if(nullMat){ At<uint32>(g.v,0x14)=0; At<float>(g.v,4)=g.pos.x; At<float>(g.v,8)=g.pos.y; At<float>(g.v,12)=g.pos.z; }
    else { At<uint32>(g.v,0x14)=uint32(reinterpret_cast<uintptr_t>(w.mat[0])); At<float>(g.v,4)=G4Q(c,-500.f,500.f); At<float>(g.v,8)=G4Q(c,-500.f,500.f); At<float>(g.v,12)=G4Q(c,0.f,50.f); }
    At<int16>(g.v,0x22)=G4Model(c,kind);
    { const uint32 st= c.chance(3) ? c.next()%8 : 3u; g.v[0x36]=uint8((st<<3)|(c.next()&7)); }
    {   // identificador de grabacion (byte bajo en +0x424): negativo, valido, y valores de 32 bits positivos con el byte bajo negativo
        int32 rec;
        switch(c.next()%6){ case 0:rec=-1;break; case 1:rec=int32(c.next()%100);break; case 2:rec=0;break; case 3:rec=int32(128+c.next()%128);break;
                            case 4:rec=int32(256+c.next()%127);break; default:rec=int32(c.next()); }
        At<int32>(g.v,0x424)=rec;
    }
    { uint8 fl=uint8(c.next()); fl= c.chance(5) ? uint8(fl&~0x10u) : uint8(fl|0x10u); g.v[0x428]=fl; }
    At<float>(g.v,0x49C)=G4Pedal(c); At<float>(g.v,0x4A0)=G4Pedal(c);
    At<float>(g.v,0x4C0)=G4Health(c);
    At<float>(g.v,0x5A4)=G4Spd(c,0.22f);
    At<float>(g.v,0x84C)=G4Spd(c,0.22f);
    g.v[0x960]=uint8(c.chance(2) ? 1 : 0);
    At<float>(g.v,0x9C4)=G4Spd(c,0.34f);

    // --- parametros del vehiculo, camara y entorno
    c.fill(g.vp,0x60);
    At<uint32>(g.vp,0x10)=uint32(reinterpret_cast<uintptr_t>(g.v));
    G4Cam(c,g.pos,c.next());
    { static const float uw[]{0.f,0.3f,0.49999f,0.5f,0.6f,1.f}; G<float>(0xC8132C)=uw[c.next()%std::size(uw)]; }

    // --- falsos propios
    g4.accel=int16(c.chance(3) ? 0 : int(c.next()%256)); g4.brake=int16(c.chance(3) ? 0 : int(c.next()%256)); if(c.chance(20))g4.accel=int16(-int(c.next()%256));
    for(auto& s:g4.slotSeq)s= c.chance(4) ? -1 : int32(7+c.next()%10);
    g4.slotIdx=0;
    g4.comp=CVector{c.uni(-500.f,500.f),c.uni(-500.f,500.f),c.uni(0.f,100.f)};
    return g;
}
inline void G4Out(Ctx& c,const G4Env& g){ c.out["ent"]=c.Bytes(g.eb+0x7C,ENT_SIZE-0x7C); c.out["vp"]=c.Bytes(g.vp,0x4C); }

inline bool RunG4(Ctx& c){
    const std::string& nm=c.nm;

    if(nm=="PlayAircraftSound"){
        static const int st_[]{1,2,3,4,5,7}; static const int slots[]{0x13,0x28,0x12};
        const int st=st_[c.next()%6];
        auto g=G4Prep(c,6,c.chance(4));
        const int slot= c.chance(4) ? int(7+c.next()%10) : slots[c.next()%3];
        const int sfx=int(c.next()%40);
        const float vol= c.chance(8) ? c.uni(-100.f,-80.f) : c.uni(-30.f,20.f);
        const float spd= c.chance(10) ? 1.f : c.uni(0.1f,2.5f);
        const bool has=(c.next()%10)<4;   // el sonido de ese tipo ya existe (actualizacion) o no (creacion)
        g.e->m_EngineSounds[size_t(st)].Sound= has ? c.Pre(c.next()%NPRE) : nullptr;
        { auto P=MakePatches(G_STD|G_X4); CallThis<void>(c.a,g.eb,int32(st),int32(slot),int32(sfx),vol,spd); }
        c.out["snd"]=Id(g.e->m_EngineSounds[size_t(st)].Sound); G4Out(c,g); return true; }

    if(nm=="GetAircraftNearPosition"){
        auto g=G4Prep(c,6,c.chance(4));
        if(c.chance(2))g.e->m_AuSettings.VehicleAudioType=static_cast<eAEVehicleSoundType>(c.next()%11);
        CVector r{9.f,8.f,7.f};
        CVector* res;
        res=CallThis<CVector*>(c.a,g.eb,&r);
        c.out["pos"]=c.Bytes(&r,12); c.ret=(res==&r) ? 1 : 0; G4Out(c,g); return true; }

    if(nm=="ProcessAircraft"){
        const uint32 k=c.next()%20;
        const int kind= k<4 ? 1 : (k<9 ? 0 : (k<14 ? 3 : (k<16 ? 2 : 6)));
        auto g=G4Prep(c,kind,false);
        if(kind==6){ static const int other[]{0,1,2,3,7,8,9,10}; g.e->m_AuSettings.VehicleAudioType=static_cast<eAEVehicleSoundType>(other[c.next()%8]); }
        G<uint8>(0xB6B994)=uint8(c.chance(10) ? 1 : 0);   // s_HelicoptorsDisabled
        g.e->m_IsHelicopterDisabled=c.chance(8);
        g.e->m_IsWreckedVehicle=c.chance(8);
        g.e->m_IsPlayerDriver=c.chance(2);
        { auto P=MakePatches(G_STD|G_X4); CallThis<void>(c.a,g.eb,static_cast<void*>(g.vp)); }
        G4Out(c,g); return true; }

    // Funciones Process* de una sola aeronave: todas son thiscall con tVehicleParams& (ret 4)
    struct Proc { const char* name; int kind; };
    static const Proc procs[]{ {"ProcessPlayerProp",0},{"ProcessDummyProp",0},{"ProcessAIProp",0},{"ProcessPlayerHeli",1},{"ProcessDummyHeli",1},{"ProcessAIHeli",1},
                               {"ProcessPlayerSeaPlane",2},{"ProcessDummySeaPlane",2},{"ProcessPlayerJet",3},{"ProcessDummyJet",3},{"ProcessDummyRCPlane",4},{"ProcessDummyRCHeli",5} };
    for(const auto& p:procs){
        if(nm!=p.name)continue;
        auto g=G4Prep(c,p.kind,false);
        if(p.kind==4||p.kind==5)g.e->m_IsWreckedVehicle=c.chance(6);
        else g.e->m_IsWreckedVehicle=false;
        { auto P=MakePatches(G_STD|G_X4); CallThis<void>(c.a,g.eb,static_cast<void*>(g.vp)); }
        G4Out(c,g); return true;
    }
    return false;
}
}
