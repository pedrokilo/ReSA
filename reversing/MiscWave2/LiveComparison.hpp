#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1): CCarEnterExit (22 funciones) y CRunningScript (22 funciones) del EXE
// GTA SA 1.0 US. Original (hook desactivado) frente a candidato (hook activo), llamando SIEMPRE a la direccion del EXE.
// Todo se hace sobre objetos falsos en memoria propia (vehiculo, peds, matriz, model info, colmodel, scripts y bytecode
// falsos); nunca se toca el script real ni ningun singleton. Colaboradores sustituidos con EntryPatch en la direccion del
// EXE y en el simbolo de la DLL (mismo stub, misma traza): CWorld::ProcessLineOfSight, CAnimManager::BlendAnimation,
// RpAnimBlendClumpGetFirstAssociation/GetNextAssociation/GetAssociation, CPed::StopNonPartialAnims/RestartNonPartialAnims,
// CMessages::ClearSmallMessagesOnly y CCutsceneMgr::IsCutsceneSkipButtonBeingPressed. Las llamadas virtuales del vehiculo
// falso (IsDoor*U32, GetRideAnimData) van por una vtable falsa con trazas por ranura. Los stubs solo actuan en el hilo del
// banco (otros hilos del juego los pueden llamar). Todo el estado global tocado se guarda y se restaura en cada ejecucion.
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../ProbeRuntime.hpp"

namespace MiscWave2Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 H(uint32 a,uint32 b=0){ uint32 x=(a+0x9E3779B9u)*2654435761u; x^=(b+0x7F4A7C15u)*40503u; x^=x>>15; x*=2246822519u; x^=x>>13; x*=3266489917u; x^=x>>16; return x; }
inline uint32 F(float f){ return std::bit_cast<uint32>(f); }
inline std::vector<uint8> B(const void* p,size_t n){ return ReversingProbe::Bytes(p,n); }
inline void Fill(void* p,size_t n,uint32 seed){ auto* b=static_cast<uint8*>(p); uint32 s=seed*2654435761u+12345u; for(size_t i=0;i<n;++i){ s^=s<<13; s^=s>>17; s^=s<<5; b[i]=uint8(s>>11); } }
template<class T> inline T& Raw(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }   // hilo del banco (los stubs ignoran hilos ajenos)

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j)a+=5+*reinterpret_cast<int32*>(a+1); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
// Orden = indices de CarCall (0..21) y ScrCall (22..43)
inline const std::array<Fn,44> fns{{
    {"CCarEnterExit","ComputeDoorFlag",0x64E550,0,-1,512},
    {"CCarEnterExit","ComputeOppositeDoorFlag",0x64E610,0,-1,512},
    {"CCarEnterExit","ComputePassengerIndexFromCarDoor",0x64F1E0,0,-1,384},
    {"CCarEnterExit","ComputeSlowJackedPed",0x64F070,0,-1,384},
    {"CCarEnterExit","ComputeTargetDoorToEnterAsPassenger",0x64F190,0,-1,256},
    {"CCarEnterExit","ComputeTargetDoorToExit",0x64F110,0,-1,512},
    {"CCarEnterExit","IsCarDoorInUse",0x64EC90,0,-1,1024},
    {"CCarEnterExit","IsCarSlowJackRequired",0x64EF70,0,-1,512},
    {"CCarEnterExit","IsVehicleHealthy",0x64EEC0,0,-1,128},
    {"CCarEnterExit","IsPedHealthy",0x64EEE0,0,-1,128},
    {"CCarEnterExit","CarHasDoorToOpen",0x64EDD0,0,-1,256},
    {"CCarEnterExit","CarHasDoorToClose",0x64EE10,0,-1,256},
    {"CCarEnterExit","CarHasPartiallyOpenDoor",0x64EE70,0,-1,256},
    {"CCarEnterExit","IsCarDoorReady",0x64ED90,0,-1,256},
    {"CCarEnterExit","CarHasOpenableDoor",0x64EE50,0,-1,256},
    {"CCarEnterExit","IsCarQuickJackPossible",0x64EF00,0,-1,256},
    {"CCarEnterExit","IsClearToDriveAway",0x6509B0,0,-1,512},
    {"CCarEnterExit","IsPathToDoorBlockedByVehicleCollisionModel",0x651210,0,-1,1024},
    {"CCarEnterExit","RemoveGetInAnims",0x64F6E0,0,-1,128},
    {"CCarEnterExit","RemoveCarSitAnim",0x64F680,0,-1,128},
    {"CCarEnterExit","AddInCarAnim",0x64F720,0,-1,512},
    {"CCarEnterExit","QuitEnteringCar",0x650130,0,-1,1024},
    {"CRunningScript","Init",0x4648E0,0,-1,64},
    {"CRunningScript","AddScriptToList",0x464C00,0,-1,64},
    {"CRunningScript","RemoveScriptFromList",0x464BD0,0,-1,128},
    {"CRunningScript","UpdatePC",0x464DA0,0,-1,128},
    {"CRunningScript","UpdateCompareFlag",0x4859D0,0,-1,256},
    {"CRunningScript","GetPointerToLocalVariable",0x463CA0,0,-1,256},
    {"CRunningScript","GetPointerToLocalArrayElement",0x463CC0,0,-1,512},
    {"CRunningScript","ReadArrayInformation",0x463CF0,0,-1,512},
    {"CRunningScript","GetPointerToScriptVariable",0x464790,0,-1,1024},
    {"CRunningScript","GetIndexOfGlobalVariable",0x464700,0,-1,512},
    {"CRunningScript","CollectParameters",0x464080,0,-1,1024},
    {"CRunningScript","CollectNextParameterWithoutIncreasingPC",0x464250,0,-1,512},
    {"CRunningScript","StoreParameters",0x464370,0,-1,1024},
    {"CRunningScript","ReadParametersForNewlyStartedScript",0x464500,0,-1,1024},
    {"CRunningScript","ReadTextLabelFromScript",0x463D50,0,-1,1024},
    {"CRunningScript","GetCorrectPedModelIndexForEmergencyServiceType",0x464F50,0,-1,256},
    {"CRunningScript","ThisIsAValidRandomPed",0x489490,0,-1,256},
    {"CRunningScript","IsPedDead",0x464D70,0,-1,128},
    {"CRunningScript","GetPadState",0x485B10,0,-1,64},
    {"CRunningScript","DoDeathArrestCheck",0x485A50,0,-1,512},
    {"CRunningScript","ProcessOneCommand",0x469EB0,0,-1,256},
    {"CRunningScript","Process",0x469F00,0,-1,512},
}};
static constexpr size_t kFirstScript=22;

// =====================================================================================================================
// Fixtures del vehiculo (CCarEnterExit). Offsets del EXE: vptr 0, matriz 0x14, modelo 0x22, estado/tipo 0x36, handling 0x384,
// banderas 0x429/0x42A, conductor 0x460, pasajeros 0x464/468/46C, 0x485 numGettingIn, 0x486 in, 0x487 out, 0x488 maxPas,
// 0x48B colmodel propio, 0x4A4 createdBy, 0x4C0 vida, 0x4F8 cerradura, 0x590 tipo, 0x594 subtipo. Peds: +4 posicion, +0x14
// matriz, +0x18 clump, +0x1C banderas, +0x484 creadoPor, +0x4D4 grupo de anim, +0x530 estado, +0x540 vida, +0x598 tipo.
// =====================================================================================================================
alignas(16) inline std::array<uint8,0x800> vehBuf{};
alignas(16) inline std::array<uint8,0x100> hdlBuf{};
alignas(16) inline std::array<std::array<uint8,0x700>,5> pedBuf{};   // 0 conductor, 1..3 pasajeros, 4 sujeto
alignas(16) inline std::array<uint8,0x40> clumpBuf{};
alignas(16) inline std::array<uint8,0x60> matBuf{};
alignas(16) inline std::array<uint8,0x40> miBuf{}, colBuf{}, cdBuf{}, rideBuf{};
alignas(16) inline std::array<std::array<uint8,0x14>,6> sphBuf{};
alignas(16) inline std::array<std::array<uint8,0x60>,3> assocBuf{};
inline void* fakeVtbl[64]{};
inline uint32 vseed{}, assocCount{}; inline bool hasIdle{}, traceVirt{true}, rideOn{};
inline uintptr_t plosEntity{}; inline bool plosResult{};
inline CVehicle* Veh(){ return reinterpret_cast<CVehicle*>(vehBuf.data()); }
inline CPed* Ped(size_t k){ return reinterpret_cast<CPed*>(pedBuf[k].data()); }
inline uint32 Off(const void* p,const void* base){ return p ? uint32(uintptr_t(p)-uintptr_t(base)) : 0xFFFFFFFFu; }

// Ranuras virtuales de CVehicle: devuelven pseudoaleatorio estable por (ranura, argumento). La ranura 47 (0xBC, GetRideAnimData)
// devuelve un puntero. Firma thiscall con 1 argumento de pila (ret 4); las ranuras sin argumento solo se usan con la 47.
template<int N> inline uintptr_t __fastcall VSlot(void*,void*,uint32 a) {
    if(Mine()&&traceVirt)trace.push_back({"v",N,a}); return H(vseed,uint32(N)*977u+a)&1;
}
// Ranura 47 (GetRideAnimData): thiscall SIN argumentos de pila (ret simple); con "ret 4" desequilibraba la pila del EXE.
inline uintptr_t __fastcall VSlotRide(void*,void*) { if(Mine()&&traceVirt)trace.push_back({"ride"}); return rideOn ? uintptr_t(rideBuf.data()) : 0; }
template<int... Ns> inline void FillV(std::integer_sequence<int,Ns...>){ ((fakeVtbl[Ns]=reinterpret_cast<void*>(&VSlot<Ns>)),...); fakeVtbl[47]=reinterpret_cast<void*>(&VSlotRide); }

// --- colaboradores sustituidos (misma funcion en el EXE y en la DLL) ---
inline bool __cdecl StubPlos(const CVector& o,const CVector& t,CColPoint&,CEntity*& ent,bool a,bool b,bool c,bool d,bool e,bool f,bool g,bool h) {
    if(!Mine())return false;
    trace.push_back({"plos",F(o.x),F(o.y),F(o.z),F(t.x),F(t.y),F(t.z),int(a)|int(b)<<1|int(c)<<2|int(d)<<3|int(e)<<4|int(f)<<5|int(g)<<6|int(h)<<7});
    ent=reinterpret_cast<CEntity*>(plosEntity); return plosResult; }
inline void* __cdecl StubBlend(void* clump,uint32 grp,uint32 anim,float t){ if(Mine())trace.push_back({"blend",Off(clump,clumpBuf.data()),grp,anim,F(t)}); return nullptr; }
inline void* __cdecl StubFirst(void* clump,uint32 flags){ if(Mine())trace.push_back({"first",Off(clump,clumpBuf.data()),flags}); return assocCount ? assocBuf[0].data() : nullptr; }
inline void* __cdecl StubNext(void* a,uint32 flags){
    uint32 idx=99; for(uint32 k=0;k<3;++k)if(a==assocBuf[k].data())idx=k;
    if(Mine())trace.push_back({"next",idx,flags}); return idx+1<assocCount ? assocBuf[idx+1].data() : nullptr; }
inline void* __cdecl StubGetAssoc(void* clump,uint32 id){ if(Mine())trace.push_back({"getassoc",Off(clump,clumpBuf.data()),id}); return hasIdle ? assocBuf[0].data() : nullptr; }
inline void __fastcall StubStop(void* self,void*){ if(Mine())trace.push_back({"stop",Off(self,pedBuf[4].data())}); }
inline void __fastcall StubRestart(void* self,void*){ if(Mine())trace.push_back({"restart",Off(self,pedBuf[4].data())}); }
inline void __cdecl StubClear(){ if(Mine())trace.push_back({"clear"}); }
inline bool skipPressed{};
inline bool __cdecl StubSkip(){ if(!Mine())return false; trace.push_back({"skip",skipPressed}); return skipPressed; }

inline void BuildPeds(unsigned I,unsigned salt) {
    static constexpr float kHealth[8]{-5.f,0.f,0.5f,1.f,100.f,2000.f,0.f,0.f};
    for(size_t k=0;k<pedBuf.size();++k){
        auto* p=pedBuf[k].data(); Fill(p,pedBuf[k].size(),I*7u+salt+uint32(k));
        Raw<uintptr_t>(p,0x14)=0; Raw<uintptr_t>(p,0x18)=uintptr_t(clumpBuf.data());
        Raw<CVector>(p,4)=CVector{float(H(I,uint32(k)+20u)%200)-100.f,float(H(I,uint32(k)+30u)%200)-100.f,float(H(I,uint32(k)+40u)%20)};
        const uint32 h=H(I+salt,uint32(k)+50u);
        Raw<float>(p,0x540)= (h%11==0) ? std::bit_cast<float>(0x7FC00000u) : kHealth[h%8];
        Raw<uint32>(p,0x598)=uint32((h>>4)%7);                 // tipo de ped (0,1 = jugador)
        Raw<uint32>(p,0x530)=(h>>8)%64u;
        Raw<uint8>(p,0x484)=uint8((h>>16)%4u);
        Raw<uint32>(p,0x4D4)=(h>>10)%30u;
        Raw<uint32>(p,0x1C)=H(I,60u+uint32(k));
    }
}
inline void BuildCar(unsigned I,unsigned salt) {
    static constexpr uint32 kTypes[8]{0,9,0,9,2,5,3,10}, kSub[8]{0,9,2,10,4,3,0,1}, kGroups[8]{0,14,15,16,17,100,101,5};
    clumpBuf.fill(0); clumpBuf[0]=2;   // rpCLUMP
    Fill(vehBuf.data(),vehBuf.size(),I*13u+salt); Fill(hdlBuf.data(),hdlBuf.size(),I*17u+salt+1u);
    BuildPeds(I,salt);
    auto* v=vehBuf.data(); auto* h=hdlBuf.data();
    const uint32 r=H(I+salt,1u);
    Raw<uintptr_t>(v,0)=uintptr_t(fakeVtbl);
    Raw<uintptr_t>(v,0x14)=0;
    Raw<int16>(v,0x22)=400;
    Raw<uint8>(v,0x36)=uint8(((r>>3)%8u)<<3|2u);   // estado << 3 | tipo de entidad (2 = vehiculo)
    Raw<uint8>(v,0x48B)=0xFF;
    Raw<uintptr_t>(v,0x384)=uintptr_t(h);
    uint32 flags=H(I+salt,2u)&~(0x200u|0x400u);
    flags|=((r>>8)&1u)<<9; flags|=((r>>9)&1u)<<10;   // tandem, sit in boat
    Raw<uint32>(h,0xCC)=flags; Raw<uint8>(h,0xDE)=uint8(kGroups[(r>>12)%8u]);
    Raw<uint8>(v,0x429)=uint8(H(I+salt,3u));
    Raw<uint8>(v,0x42A)=uint8(H(I+salt,4u));
    const uint32 occ=(r>>16)&15u;
    Raw<uintptr_t>(v,0x460)=(occ&1u)?uintptr_t(pedBuf[0].data()):0;
    for(unsigned k=0;k<3;++k)Raw<uintptr_t>(v,0x464+4*k)=(occ&(2u<<k))?uintptr_t(pedBuf[1+k].data()):0;
    Raw<uint8>(v,0x485)=uint8((r>>20)%5u);        // 0 incluido: la DLL debe saturar en 0
    Raw<uint8>(v,0x486)=uint8(H(I+salt,5u)); Raw<uint8>(v,0x487)=uint8(H(I+salt,6u));
    Raw<uint8>(v,0x488)=uint8((r>>24)%4u);
    Raw<uint8>(v,0x4A4)=uint8((r>>26)%5u);
    Raw<uint32>(v,0x4F8)=(H(I+salt,7u)%9u);
    Raw<uint32>(v,0x590)=kTypes[(r>>2)%8u]; Raw<uint32>(v,0x594)=kSub[(r>>5)%8u];
    vseed=H(I,salt+99u); rideOn=(r>>28)&1; Fill(rideBuf.data(),rideBuf.size(),I+salt+5u);
    assocCount=(r>>29)%4u; hasIdle=(H(I,8u)&1)!=0;
    for(size_t k=0;k<assocBuf.size();++k){ Fill(assocBuf[k].data(),assocBuf[k].size(),I*3u+uint32(k)+salt); Raw<uint16>(assocBuf[k].data(),0x2E)=uint16(H(I,uint32(k)+70u)); }
}
// Geometria para IsClearToDriveAway / IsPathToDoorBlocked: matriz, model info (ranura 400), ColModel y esferas.
inline void BuildGeometry(unsigned I,uint32 modelIndex) {
    matBuf.fill(0);
    static constexpr float kYaw[6]{0.f,0.5235988f,1.5707964f,3.1415927f,4.3633232f,-0.7f};
    const float yaw=kYaw[H(I,11u)%6u], c=std::cos(yaw), s=std::sin(yaw), upz=(H(I,12u)%5==0)?-1.f:1.f;
    Raw<CVector>(matBuf.data(),0x00)=CVector{c,s,0.f};
    Raw<CVector>(matBuf.data(),0x10)=CVector{-s,c,0.f};
    Raw<CVector>(matBuf.data(),0x20)=CVector{0.f,0.f,upz};
    Raw<CVector>(matBuf.data(),0x30)=CVector{float(H(I,13u)%400)-200.f+0.25f,float(H(I,14u)%400)-200.f-0.5f,float(H(I,15u)%30)+0.125f};
    Raw<uintptr_t>(vehBuf.data(),0x14)=uintptr_t(matBuf.data());
    Raw<int16>(vehBuf.data(),0x22)=int16(modelIndex);
    Raw<uint8>(vehBuf.data(),0x36)=uint8((Raw<uint8>(vehBuf.data(),0x36)&0xF8)|2);
    miBuf.fill(0); colBuf.fill(0); cdBuf.fill(0);
    *reinterpret_cast<uintptr_t*>(0xA9B0C8+400*4)=uintptr_t(miBuf.data());   // ranura 400 de ms_modelInfoPtrs (se restaura por ejecucion)
    Raw<uintptr_t>(miBuf.data(),0x14)=uintptr_t(colBuf.data());
    const float hx=1.f+float(H(I,16u)%30)*0.1f, hy=1.5f+float(H(I,17u)%40)*0.2f;
    Raw<CVector>(colBuf.data(),0)=CVector{-hx,-hy*0.9f,-0.5f}; Raw<CVector>(colBuf.data(),0xC)=CVector{hx,hy,0.75f+float(H(I,18u)%10)*0.1f};
    Raw<uintptr_t>(colBuf.data(),0x2C)=uintptr_t(cdBuf.data());
    const unsigned n=H(I,19u)%6u;
    Raw<uint16>(cdBuf.data(),0)=uint16(n); Raw<uintptr_t>(cdBuf.data(),8)=uintptr_t(sphBuf.data());
    for(unsigned k=0;k<6;++k){ Fill(sphBuf[k].data(),0x14,I+k); auto* sp=sphBuf[k].data();
        Raw<CVector>(sp,0)=CVector{float(int(H(I,uint32(100+k))%81)-40)*0.2f,float(int(H(I,uint32(110+k))%81)-40)*0.2f,float(int(H(I,uint32(120+k))%21)-10)*0.1f};
        Raw<float>(sp,0xC)=0.3f+float(H(I,uint32(130+k))%20)*0.15f; }
    plosResult=(H(I,21u)&1)!=0;
    switch(H(I,22u)%3u){ case 0: plosEntity=0; break; case 1: plosEntity=uintptr_t(vehBuf.data()); break; default: plosEntity=uintptr_t(pedBuf[4].data()); }
}

inline constexpr int32 kDoors[13]{-1,0,1,7,8,9,10,11,12,17,18,19,100};
inline json CarState() {
    json s=json::object();
    s["veh"]=Fnv(vehBuf.data(),vehBuf.size()); s["hdl"]=Fnv(hdlBuf.data(),hdlBuf.size());
    uint64 hp=1469598103934665603ull; for(auto& p:pedBuf)hp=Fnv(p.data(),p.size(),hp); s["peds"]=hp;
    s["assoc"]=Fnv(assocBuf.data(),sizeof(assocBuf)); s["clump"]=Fnv(clumpBuf.data(),clumpBuf.size());
    s["trace"]=trace; return s;
}
inline json CarCall(size_t f,unsigned I,uintptr_t addr) {
    BuildCar(I,uint32(f)*101u);
    traceVirt=true; trace=json::array();
    CVehicle* v=Veh(); json out=json::object();
    const int32 door=kDoors[H(I,200u)%13u], door2=kDoors[H(I,201u)%13u];
    auto pedPick=[&](unsigned k)->CPed*{ const unsigned c=H(I,300u+k)%7u; return c<5?Ped(c):nullptr; };
    switch(f){
    case 0: out["ret"]=reinterpret_cast<int32(__cdecl*)(CVehicle*,int32,bool)>(addr)(v,door,(H(I,210u)&1)!=0); break;
    case 1: out["ret"]=reinterpret_cast<int32(__cdecl*)(CVehicle*,int32,bool)>(addr)(v,door,(H(I,210u)&1)!=0); break;
    case 2: out["ret"]=reinterpret_cast<int32(__cdecl*)(CVehicle*,int32)>(addr)(v,door); break;
    case 3: out["ret"]=Off(reinterpret_cast<CPed*(__cdecl*)(CVehicle*,int32)>(addr)(v,door),pedBuf[0].data()-0); break;
    case 4: out["ret"]=reinterpret_cast<int32(__cdecl*)(CVehicle*,int32)>(addr)(v,int32(H(I,220u)%7u)-2); break;
    case 5: out["ret"]=reinterpret_cast<int32(__cdecl*)(CVehicle*,CPed*)>(addr)(v,pedPick(0)); break;
    case 6: out["ret"]=reinterpret_cast<bool(__cdecl*)(CVehicle*,int32,int32)>(addr)(v,door,door2); break;
    case 7: out["ret"]=reinterpret_cast<bool(__cdecl*)(CVehicle*,int32)>(addr)(v,door); break;
    case 8: out["ret"]=reinterpret_cast<bool(__cdecl*)(CVehicle*)>(addr)(v); break;
    case 9: out["ret"]=reinterpret_cast<bool(__cdecl*)(CPed*)>(addr)(Ped(H(I,230u)%5u)); break;
    case 10: case 11: case 12: case 13: out["ret"]=reinterpret_cast<bool(__cdecl*)(CVehicle*,int32)>(addr)(v,door); break;
    case 14: out["ret"]=reinterpret_cast<bool(__cdecl*)(CVehicle*,int32,CPed*)>(addr)(v,door,Ped(H(I,231u)%5u)); break;
    case 15: traceVirt=false;   // el EXE hace consultas puras que la DLL omite: solo cuentan el resultado y la memoria
             out["ret"]=reinterpret_cast<bool(__cdecl*)(CVehicle*,int32,CPed*)>(addr)(v,(H(I,232u)%3u==0)?10:door,Ped(H(I,233u)%5u)); break;
    case 16: BuildGeometry(I,400); out["ret"]=reinterpret_cast<bool(__cdecl*)(CVehicle*)>(addr)(v); break;
    case 17: { BuildGeometry(I,(H(I,24u)%8u==0)?577u:400u); const CVector pos{float(H(I,25u)%100)-50.f,float(H(I,26u)%100)-50.f,float(H(I,27u)%10)};
               out["ret"]=reinterpret_cast<bool(__cdecl*)(CPed*,CVehicle*,const CVector*)>(addr)(Ped(4),v,&pos); break; }
    case 18: out["ret"]=0; reinterpret_cast<void(__cdecl*)(CPed*)>(addr)(Ped(4)); break;
    case 19: out["ret"]=0; reinterpret_cast<void(__cdecl*)(CPed*)>(addr)(Ped(4)); break;
    case 20: out["ret"]=0; reinterpret_cast<void(__cdecl*)(CVehicle*,CPed*,bool)>(addr)(v,Ped(4),(H(I,240u)&1)!=0); break;
    case 21: out["ret"]=0; reinterpret_cast<void(__cdecl*)(CPed*,CVehicle*,int32,bool)>(addr)(Ped(4),v,door,(H(I,241u)&1)!=0); break;
    }
    out["state"]=CarState(); return out;
}

// =====================================================================================================================
// Fixtures de CRunningScript: scripts y bytecode falsos en memoria propia. Offsets del EXE: pNext 0, pPrev 4, nombre 8,
// BaseIP 0x10, IP 0x14, pila de IP 0x18 (8), profundidad 0x38 (u16), variables locales 0x3C (34), activo 0xC4, condicion
// 0xC5, limpieza de mision 0xC6, externo 0xC7, texto 0xC8, tipo externo 0xC9, despertar 0xCC, AndOr 0xD0 (u16), NOT 0xD2,
// muerte/arresto habilitado 0xD3, hecho 0xD4, saltar escena 0xD8, "unica mision" 0xDC. Espacio global 0xA49960, locales de
// mision 0xA48960, ScriptParams 0xA43C78. Las variables de indice de arrays son las locales 30..33 y las globales
// 0x20..0x3C (siempre valores 0..3); el resto de variables son datos aleatorios.
// =====================================================================================================================
alignas(16) inline std::array<std::array<uint8,0x100>,3> sBuf{};
alignas(16) inline std::array<uint8,0x200> codeBuf{};
alignas(16) inline std::array<uint8,0x300> txtBuf{};
inline uintptr_t listHead{};
static_assert(sizeof(CRunningScript)<=0x100);
inline CRunningScript* S(size_t k){ return reinterpret_cast<CRunningScript*>(sBuf[k].data()); }
inline uint8* SS(){ return reinterpret_cast<uint8*>(0xA49960); }
inline uint8* ML(){ return reinterpret_cast<uint8*>(0xA48960); }
inline uint32* SP(){ return reinterpret_cast<uint32*>(0xA43C78); }

inline const std::array<std::pair<uintptr_t,size_t>,12> globals{{
    {0xA43C78,0x80},          // ScriptParams
    {0xA48960,0x2000},        // locales de mision (0x1000) + primeros 0x1000 bytes de ScriptSpace
    {0xA447F4,2},             // CommandsExecuted
    {0xA43F50,0x14},          // datos de sentencias switch (ReinitialiseSwitchStatementData)
    {0xA476AC,4},             // OnAMissionFlag
    {0xA444B0,1},             // FailCurrentMission
    {0xB7CB84,4},             // reloj del juego
    {0xBAAD40,1},             // CHud::m_BigMessage[1][0]
    {0xB7CD98,0x320},         // CWorld::Players (2 x CPlayerInfo)
    {0xB7CD74,1},             // PlayerInFocus
    {0xA9B0C8+400*4,4},       // ranura 400 de ms_modelInfoPtrs
    {0x859D44,8},             // cadena "noname" del EXE
}};
inline std::vector<std::vector<uint8>> savedGlobals;
inline void RestoreGlobals(){ for(size_t j=0;j<globals.size();++j)std::memcpy(reinterpret_cast<void*>(globals[j].first),savedGlobals[j].data(),savedGlobals[j].size()); }
inline uint64 GlobDigest(bool withParams){ uint64 h=1469598103934665603ull; for(size_t j=withParams?0:1;j<globals.size();++j)h=Fnv(reinterpret_cast<void*>(globals[j].first),globals[j].second,h); return h; }

struct Code {
    size_t n{};
    void P8(uint32 v){ codeBuf[n++]=uint8(v); }
    void P16(uint32 v){ P8(v); P8(v>>8); }
    void P32(uint32 v){ P16(v); P16(v>>16); }
};
inline void PutSeq(uint8* d,unsigned nops,bool notFlag,uint32 waitType,uint32 waitVal){   // NOP*nops + WAIT(valor)
    size_t n=0; auto p8=[&](uint32 v){ d[n++]=uint8(v); };
    for(unsigned k=0;k<nops;++k){ p8(0); p8(notFlag?0x80:0); }
    p8(1); p8(notFlag?0x80:0); p8(waitType);
    if(waitType==4)p8(waitVal); else if(waitType==5){ p8(waitVal); p8(waitVal>>8); } else if(waitType==1){ p8(waitVal); p8(waitVal>>8); p8(waitVal>>16); p8(waitVal>>24); }
    else if(waitType==2||waitType==3){ p8(waitVal); p8(waitVal>>8); }
    p8(0); p8(0);   /* NOP final: la DLL (NOTSA) hace VERIFY de que el comando siguiente sea valido; el EXE no lo mira */
}
inline void PutArray(Code& c,bool global,unsigned I,unsigned& q){
    const uint32 h=H(I,600u+q++); const bool gidx=((h>>8)&1)!=0;
    c.P16(global ? 0x40+(h%0x40)*4 : (h%8));
    c.P16(gidx ? 0x20+((h>>9)%8)*4 : 30+((h>>9)%4));
    c.P8(4+h%8); c.P8(gidx?0x80:0x00);   // tamano; bit 7 = variable de indice global
}
inline void PutOperand(Code& c,uint32 type,unsigned I,unsigned& q){
    const uint32 h=H(I,700u+q++); c.P8(type);
    switch(type){
    case 1: c.P32(h); break;
    case 2: c.P16(0x40+(h%0x1C0)*4); break;
    case 3: c.P16(h%30); break;
    case 4: c.P8(h); break;
    case 5: c.P16(h); break;
    case 6: c.P32(F(float(int(h%2001)-1000)*0.37f)); break;
    case 7: PutArray(c,true,I,q); break;
    case 8: PutArray(c,false,I,q); break;
    default: break;
    }
}
inline void BuildScripts(unsigned I,uint32 salt,bool mustBe) {
    for(size_t k=0;k<3;++k){
        auto* b=sBuf[k].data(); Fill(b,0x100,I*5u+salt+uint32(k));
        Raw<uintptr_t>(b,0x10)=uintptr_t(codeBuf.data()); Raw<uintptr_t>(b,0x14)=uintptr_t(codeBuf.data());
        for(unsigned j=0;j<8;++j)Raw<uintptr_t>(b,0x18+4*j)=uintptr_t(codeBuf.data())+0x80+8*j;
        Raw<uint16>(b,0x38)=uint16(H(I,uint32(k)+800u)%9u);
        for(size_t o:{0xC4u,0xC5u,0xC6u,0xC7u,0xC8u,0xD2u,0xD3u,0xD4u})Raw<uint8>(b,o)=uint8(H(I,uint32(o)+uint32(k)*7u)&1);
        Raw<uint8>(b,0xDC)=mustBe?1:0; Raw<int8>(b,0xC9)=int8(int(H(I,900u+uint32(k))%8)-1);
        Raw<uint16>(b,0xD0)=uint16(H(I,910u+uint32(k))%32u);
        Raw<int32>(b,0xCC)=int32(H(I,920u+uint32(k))%100000u); Raw<int32>(b,0xD8)=0;
        Raw<uintptr_t>(b,0)=0; Raw<uintptr_t>(b,4)=0;
    }
    Fill(codeBuf.data(),codeBuf.size(),I*3u+salt+1u); Fill(txtBuf.data(),txtBuf.size(),I*9u+salt+2u);
    for(unsigned j=0;j<8;++j)PutSeq(codeBuf.data()+0x80+8*j,H(I,930u+j)%2,false,4,5);
    Fill(SS(),0x1000,I*11u+salt+3u); Fill(ML(),0x1000,I*19u+salt+4u);
    for(size_t k=0;k<0x1000;++k){ if((SS()[k]&7)==0)SS()[k]=0; if((ML()[k]&7)==0)ML()[k]=0; }
    for(unsigned j=0;j<8;++j)Raw<int32>(SS(),0x20+4*j)=int32(H(I,940u+j)%4u);       // indices globales 0..3
    for(unsigned j=30;j<34;++j){ Raw<int32>(ML(),4*j)=int32(H(I,950u+j)%4u); for(size_t k=0;k<3;++k)Raw<int32>(sBuf[k].data(),0x3C+4*j)=int32(H(I,960u+j+uint32(k))%4u); }
    for(unsigned j=0;j<32;++j)SP()[j]=H(I,970u+j)%4u;                                // ScriptParams pequenos (pueden ser indices)
}
inline json PtrOut(const void* p){
    const auto a=uintptr_t(p);
    for(size_t k=0;k<3;++k)if(a>=uintptr_t(sBuf[k].data())&&a<uintptr_t(sBuf[k].data())+0x100)return json::array({"s",k,a-uintptr_t(sBuf[k].data())});
    if(a>=0xA48960&&a<0xA49960)return json::array({"ml",a-0xA48960});
    if(a>=0xA49960&&a<0xA4A960)return json::array({"ss",a-0xA49960});
    if(a>=uintptr_t(codeBuf.data())&&a<uintptr_t(codeBuf.data())+codeBuf.size())return json::array({"code",a-uintptr_t(codeBuf.data())});
    return json::array({"raw",a});
}
inline json ScrState(bool withParams) {
    json s=json::object(); json sc=json::array(); for(auto& b:sBuf)sc.push_back(Fnv(b.data(),b.size()));
    s["scripts"]=sc; s["code"]=Fnv(codeBuf.data(),codeBuf.size()); s["txt"]=Fnv(txtBuf.data(),txtBuf.size());
    s["glob"]=GlobDigest(withParams); s["list"]=PtrOut(reinterpret_cast<void*>(listHead)); s["ip"]=PtrOut(reinterpret_cast<void*>(Raw<uintptr_t>(sBuf[0].data(),0x14)));
    s["trace"]=trace; return s;
}
inline constexpr uint32 kTextTypes[14]{0,8,9,10,11,12,13,14,15,16,17,18,19,20};
inline constexpr uint32 kBufLens[10]{0,1,7,8,9,15,16,17,24,64};

inline json ScrCall(size_t f,unsigned I,uintptr_t addr) {
    const unsigned fi=unsigned(f-kFirstScript);
    const bool mustBe=(H(I,400u)%3u==0);
    BuildScripts(I,uint32(f)*131u,mustBe);
    BuildPeds(I,uint32(f)*17u);
    trace=json::array(); json out=json::object();
    unsigned q=0; auto R=[&](unsigned mod){ return H(I,500u+q++)%mod; };
    Code c; CRunningScript* s=S(0); const bool withParams=!(fi==20||fi==21);
    auto invalidType=[&]()->uint32{ static constexpr uint32 t[6]{0,9,10,13,20,255}; return t[R(6)]; };
    switch(fi){
    case 0: reinterpret_cast<void(__thiscall*)(CRunningScript*)>(addr)(s); break;
    case 1: listHead=R(2)?uintptr_t(S(1)):0; Raw<uintptr_t>(S(1),4)=R(2)?uintptr_t(S(2)):0;
            reinterpret_cast<void(__thiscall*)(CRunningScript*,CRunningScript**)>(addr)(s,reinterpret_cast<CRunningScript**>(&listHead)); break;
    case 2: { CRunningScript* t=S(1); const bool hasPrev=R(2)!=0, hasNext=R(2)!=0;
            Raw<uintptr_t>(t,4)=hasPrev?uintptr_t(S(0)):0; Raw<uintptr_t>(t,0)=hasNext?uintptr_t(S(2)):0;
            Raw<uintptr_t>(S(0),0)=uintptr_t(t); Raw<uintptr_t>(S(2),4)=uintptr_t(t);
            listHead=hasPrev?uintptr_t(S(0)):uintptr_t(t);
            reinterpret_cast<void(__thiscall*)(CRunningScript*,CRunningScript**)>(addr)(t,reinterpret_cast<CRunningScript**>(&listHead)); break; }
    case 3: { static constexpr int32 v[9]{0,1,0x10,0x7FF,0xFFF,-1,-8,-0x100,-0x1FF}; reinterpret_cast<void(__thiscall*)(CRunningScript*,int32)>(addr)(s,v[R(9)]); break; }
    case 4: { Raw<uint8>(s,0xD2)=I&1; Raw<uint8>(s,0xC5)=(I>>1)&1; const bool arg=((I>>2)&1)!=0; Raw<uint16>(s,0xD0)=uint16((I>>3)%33u);
            reinterpret_cast<void(__thiscall*)(CRunningScript*,bool)>(addr)(s,arg); break; }
    case 5: { static constexpr int32 v[8]{0,1,2,17,29,33,34,100}; out["ret"]=PtrOut(reinterpret_cast<tScriptParam*(__thiscall*)(CRunningScript*,int32)>(addr)(s,v[R(8)])); break; }
    case 6: { static constexpr int32 b[6]{0,1,5,17,33,40}; static constexpr uint16 x[7]{0,1,2,3,7,100,65535}; static constexpr uint8 z[4]{1,2,4,8};
            out["ret"]=PtrOut(reinterpret_cast<tScriptParam*(__thiscall*)(CRunningScript*,int32,uint16,uint8)>(addr)(s,b[R(6)],x[R(7)],z[R(4)])); break; }
    case 7: { PutArray(c,R(2)!=0,I,q); uint16 base=0xAAAA; int32 idx=0x55555555; const int32 upd=R(3)==0?0:(R(2)?1:5);
            reinterpret_cast<void(__thiscall*)(CRunningScript*,int32,uint16*,int32*)>(addr)(s,upd,&base,&idx); out["base"]=base; out["idx"]=idx; break; }
    case 8: { const uint32 t=R(24); c.P8(t);
            switch(t){ case 2: case 10: case 16: c.P16(0x40+R(0x1C0)*4); break; case 3: case 11: case 17: c.P16(R(30)); break;
                       case 7: case 12: case 18: PutArray(c,true,I,q); break; case 8: case 13: case 19: PutArray(c,false,I,q); break; default: break; }
            out["ret"]=PtrOut(reinterpret_cast<tScriptParam*(__thiscall*)(CRunningScript*,int32)>(addr)(s,0)); break; }
    case 9: { if(R(2)){ c.P8(2); c.P16(0x40+R(0x1C0)*4); } else { c.P8(7); PutArray(c,true,I,q); }
            out["ret"]=reinterpret_cast<uint16(__thiscall*)(CRunningScript*)>(addr)(s); break; }
    case 10: { const int16 n=int16(R(9)); for(int k=0;k<n;++k){ if(R(10)==0)c.P8(invalidType()); else PutOperand(c,1+R(8),I,q); }
            reinterpret_cast<void(__thiscall*)(CRunningScript*,int16)>(addr)(s,n); break; }
    case 11: { if(R(10)==0)c.P8(invalidType()); else PutOperand(c,1+R(8),I,q);
            out["ret"]=reinterpret_cast<int32(__thiscall*)(CRunningScript*)>(addr)(s); break; }
    case 12: { const int16 n=int16(R(7)); static constexpr uint32 t[8]{2,3,7,8,2,3,7,8};
            for(int k=0;k<n;++k){ if(R(8)==0){ static constexpr uint32 bad[5]{1,4,5,6,0}; c.P8(bad[R(5)]); } else PutOperand(c,t[R(8)],I,q); }
            reinterpret_cast<void(__thiscall*)(CRunningScript*,int16)>(addr)(s,n); break; }
    case 13: { const unsigned n=R(9); for(unsigned k=0;k<n;++k){ if(R(12)==0)c.P8(9); else PutOperand(c,1+R(8),I,q); } c.P8(0);
            reinterpret_cast<void(__thiscall*)(CRunningScript*,CRunningScript*)>(addr)(s,S(1)); break; }
    case 14: { const uint32 t=kTextTypes[R(14)]; const uint8 len=uint8(kBufLens[R(10)]); c.P8(t);
            switch(t){
            case 9: for(int k=0;k<8;++k)c.P8(0x41+R(26)); break;
            case 10: case 16: c.P16(0x40+R(0x100)*4); break;
            case 11: case 17: c.P16(R(27)); break;
            case 12: case 18: PutArray(c,true,I,q); break;
            case 13: case 19: PutArray(c,false,I,q); break;
            case 14: { static constexpr uint32 pl[9]{0,1,5,8,16,40,0x7F,0x80,0xFF}; const uint32 n=pl[R(9)]; c.P8(n); if(n<0x80)for(uint32 k=0;k<n;++k)c.P8(0x61+R(26)); break; }
            case 15: for(int k=0;k<16;++k)c.P8(R(5)==0?0:0x41+R(26)); break;
            default: break; }
            reinterpret_cast<void(__thiscall*)(CRunningScript*,char*,uint8)>(addr)(s,reinterpret_cast<char*>(txtBuf.data()+0x40),len); break; }
    case 15: { static constexpr uint32 pt[11]{0xFFFFFFFFu,0,1,4,5,6,7,15,20,22,48}; static constexpr uint32 md[14]{270,279,280,281,282,283,284,285,286,287,288,0,1,1000};
            uint32 m=md[R(14)]; const uint32 p=pt[R(11)];
            reinterpret_cast<void(__thiscall*)(CRunningScript*,uint32,uint32*)>(addr)(s,p,&m); out["model"]=m; break; }
    case 16: { static constexpr uint32 pt[27]{0xFFFFFFFFu,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,40};
            const uint32 t=pt[R(27)]; const bool a=R(2)!=0, b=R(2)!=0, d=R(2)!=0;
            out["ret"]=reinterpret_cast<bool(__thiscall*)(CRunningScript*,uint32,bool,bool,bool)>(addr)(s,t,a,b,d); break; }
    case 17: Raw<uint32>(pedBuf[4].data(),0x530)=R(80); out["ret"]=reinterpret_cast<bool(__thiscall*)(CRunningScript*,CPed*)>(addr)(s,Ped(4)); break;
    case 18: { const uint16 pi=uint16(R(2)), bt=uint16(R(25)); out["ret"]=reinterpret_cast<int16(__thiscall*)(CRunningScript*,uint16,uint16)>(addr)(s,pi,bt); break; }
    default: break;   // 19..21 se montan abajo
    }
    if(fi==19){   // DoDeathArrestCheck (siempre profundidad >= 1: con 0 el EXE lee la pila fuera de rango)
        Raw<uint16>(s,0x38)=uint16(1+R(8)); Raw<uint8>(s,0xD3)=R(4)!=0;
        const bool onMission=R(4)!=0; *reinterpret_cast<uint32*>(0xA476AC)=onMission?0x700u:0u; Raw<uint32>(SS(),0x700)=R(4)==0?2u:1u;
        Raw<uint8>(reinterpret_cast<void*>(0xB7CD98+0x190*size_t(*reinterpret_cast<uint8*>(0xB7CD74))),0xDC)=uint8(R(4));
        reinterpret_cast<void(__thiscall*)(CRunningScript*)>(addr)(s);
    }
    if(fi==20){   // ProcessOneCommand
        static constexpr uint32 wt[5]{4,5,1,2,3}; const uint32 t=wt[R(5)]; const bool nf=R(2)!=0;
        *reinterpret_cast<uint32*>(0xB7CB84)=100000u+R(1000);
        const unsigned nops=R(2);
        PutSeq(codeBuf.data(),nops,nf,t,t==2?(0x40+R(0x100)*4):(t==3?R(30):(t==4?R(128):R(300))));   // int8 <128: la DLL (NOTSA_DEBUG) hace assert in_range<uint32>(int8 negativo) que el EXE no tiene
        out["ret"]=reinterpret_cast<int8(__thiscall*)(CRunningScript*)>(addr)(s);
    }
    if(fi==21){   // Process
        *reinterpret_cast<uint32*>(0xB7CB84)=100000u+R(1000); const uint32 now=*reinterpret_cast<uint32*>(0xB7CB84);
        const unsigned nops=R(4); const bool nf=R(4)==0; const unsigned wv=5+R(20);
        PutSeq(codeBuf.data(),nops,nf,4,wv);
        const unsigned wk=R(3); Raw<int32>(s,0xCC)=int32(wk==0?now+5000u:(wk==1?now-1u:0u));
        const bool cleanup=R(2)!=0; Raw<uint8>(s,0xC6)=cleanup; Raw<uint8>(s,0xD3)=R(4)!=0;
        Raw<uint16>(s,0x38)=uint16(cleanup?1+R(8):R(9));
        *reinterpret_cast<int8*>(0xA444B0)=int8(R(3)==0?1:0);
        const bool onMission=R(3)!=0; *reinterpret_cast<uint32*>(0xA476AC)=onMission?0x700u:0u; Raw<uint32>(SS(),0x700)=R(4)==0?2u:1u;
        Raw<uint8>(reinterpret_cast<void*>(0xB7CD98+0x190*size_t(*reinterpret_cast<uint8*>(0xB7CD74))),0xDC)=uint8(R(3));
        const unsigned sk=R(4); skipPressed=R(2)!=0;
        if(sk==1){ const uint32 n=0x800+8*R(4); Raw<int32>(s,0xD8)=int32(n); PutSeq(SS()+n,R(2),false,4,7); }
        else if(sk==2){ const uint32 n=0x40+8*R(4); Raw<int32>(s,0xD8)=-int32(n); PutSeq(codeBuf.data()+n,R(2),false,4,9); }
        else Raw<int32>(s,0xD8)=0;
        out["ret"]=reinterpret_cast<int8(__thiscall*)(CRunningScript*)>(addr)(s);
    }
    // Estado comun: los 3 scripts, bytecode, texto, globales tocados y traza. La IP de S(0) y el puntero de lista se clasifican.
    out["state"]=ScrState(withParams);
    return out;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"miscwave2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"miscwave2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("miscwave2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"miscwave2-only.txt"); if(f)std::getline(f,only); }
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(fns.size(),nullptr); std::vector<bool> wasEnabled(fns.size()), wasLocked(fns.size());
    for(size_t f=0;f<fns.size();++f){
        hooks[f]=FindHook(fns[f].cls,fns[f].name);
        if(!hooks[f]){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
        wasEnabled[f]=hooks[f]->Hooked(); wasLocked[f]=hooks[f]->Locked();
    }
    if(*reinterpret_cast<uint8*>(0xB7CD74)>1){Write({{"event","error"},{"reason","PlayerInFocus fuera de rango"}});return;}
    for(size_t f=0;f<fns.size();++f)hooks[f]->LockState(false);

    // Direcciones de la DLL (entrada incremental y cuerpo) de los colaboradores sustituidos. TODAS se calculan ANTES de crear
    // ningun EntryPatch (regla 11).
    static_assert(sizeof(&CPed::StopNonPartialAnims)==sizeof(uintptr_t));
    struct Site { const char* name; uintptr_t exeAddr, dll, dllBody, stub; };
    std::array<Site,9> sites{{
        {"CWorld::ProcessLineOfSight",0x56BA00,std::bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),0,reinterpret_cast<uintptr_t>(&StubPlos)},
        {"CAnimManager::BlendAnimation",0x4D4610,std::bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),0,reinterpret_cast<uintptr_t>(&StubBlend)},
        {"RpAnimBlendClumpGetFirstAssociation",0x4D6A70,std::bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation)),0,reinterpret_cast<uintptr_t>(&StubFirst)},
        {"RpAnimBlendGetNextAssociation",0x4D6AD0,std::bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(CAnimBlendAssociation*,uint32)>(&RpAnimBlendGetNextAssociation)),0,reinterpret_cast<uintptr_t>(&StubNext)},
        {"RpAnimBlendClumpGetAssociation",0x4D68B0,std::bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),0,reinterpret_cast<uintptr_t>(&StubGetAssoc)},
        {"CPed::StopNonPartialAnims",0x5DED10,std::bit_cast<uintptr_t>(&CPed::StopNonPartialAnims),0,reinterpret_cast<uintptr_t>(&StubStop)},
        {"CPed::RestartNonPartialAnims",0x5DED50,std::bit_cast<uintptr_t>(&CPed::RestartNonPartialAnims),0,reinterpret_cast<uintptr_t>(&StubRestart)},
        {"CMessages::ClearSmallMessagesOnly",0x69DD30,std::bit_cast<uintptr_t>(&CMessages::ClearSmallMessagesOnly),0,reinterpret_cast<uintptr_t>(&StubClear)},
        {"CCutsceneMgr::IsCutsceneSkipButtonBeingPressed",0x4D5D10,std::bit_cast<uintptr_t>(&CCutsceneMgr::IsCutsceneSkipButtonBeingPressed),0,reinterpret_cast<uintptr_t>(&StubSkip)},
    }};
    bool known=true; json siteInfo=json::array();
    for(auto& s:sites){ s.dllBody=Body(s.dll); const bool ok=s.exeAddr>=0x401000&&s.exeAddr<0x1000000&&s.dll>=0x1000000&&s.dllBody>=0x1000000; known&=ok;
        siteInfo.push_back({{"name",s.name},{"exe",s.exeAddr},{"dll",s.dll},{"body",s.dllBody},{"ok",ok}}); }
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entryBefore;
    for(auto& s:sites)for(auto a:{s.exeAddr,s.dll,s.dllBody})entryBefore.push_back({a,B(reinterpret_cast<void*>(a),8)});
    for(auto& fn:fns)entryBefore.push_back({fn.address,B(reinterpret_cast<void*>(fn.address),8)});
    for(auto& g:globals)savedGlobals.push_back(B(reinterpret_cast<void*>(g.first),g.second));
    FillV(std::make_integer_sequence<int,64>{});
    const unsigned savedFP=_controlfp(0,0);
    Write({{"event","begin"},{"sites",siteInfo},{"known",known},{"fp_control",savedFP},{"functions",fns.size()}});

    bool passed=known; int pair=0;
    if(known) {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        for(auto& s:sites){ patches.push_back(std::make_unique<EntryPatch>(s.exeAddr,reinterpret_cast<void*>(s.stub)));
            patches.push_back(std::make_unique<EntryPatch>(s.dll,reinterpret_cast<void*>(s.stub)));
            if(s.dllBody!=s.dll)patches.push_back(std::make_unique<EntryPatch>(s.dllBody,reinterpret_cast<void*>(s.stub))); }

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                const unsigned I=unsigned(i);
                auto run=[&](bool candidate)->json {
                    RestoreGlobals(); hooks[f]->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                    _controlfp(_PC_24,_MCW_PC);                                               // x87 a 24 bits como el juego
                    json o=f<kFirstScript ? CarCall(f,I,fn.address) : ScrCall(f,I,fn.address);
                    _controlfp(savedFP,_MCW_PC|_MCW_RC);
                    o["switches"]=switches; return o;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok){ const std::string od=original.dump(), cd=candidate.dump();
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},
                           {"original",{{"digest",Fnv(od.data(),od.size())},{"ret",original.value("ret",json(nullptr))}}},
                           {"candidate",{{"digest",Fnv(cd.data(),cd.size())},{"ret",candidate.value("ret",json(nullptr))}}}}); }
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        patches.clear();   // restaura las entradas
    }
    for(size_t f=0;f<fns.size();++f){ hooks[f]->State(wasEnabled[f]); hooks[f]->LockState(wasLocked[f]); }
    RestoreGlobals(); _controlfp(savedFP,_MCW_PC|_MCW_RC);
    bool restored=_controlfp(0,0)==savedFP;
    for(size_t j=0;j<globals.size();++j)restored&=savedGlobals[j]==B(reinterpret_cast<void*>(globals[j].first),globals[j].second);
    for(auto& [a,b]:entryBefore)restored&=b==B(reinterpret_cast<void*>(a),8);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
