#pragma once
// Banco dentro del motor: CDamageManager (lista de 27; sus 27 funciones pendientes).
// Original (hooks de la clase desactivados, código del EXE) frente a candidato (hooks activados) sobre un
// CDamageManager falso (0x18 bytes dentro de un búfer de 0x200 con bytes aleatorios alrededor, para ver los
// índices sin límites del EXE: ruedas con índice dword o byte, desplazamientos de luces/paneles/avión sobre un
// byte) y un CAutomobile falso de 0xA00 bytes (flag 0x868 bit 4, puertas en 0x5B8). Se simulan y trazan, sin
// reservas: rand (EXE 0x821B1E + CGeneral::GetRandomNumber), CDoor::Open (0x6F4790) y
// CAutomobile::SetBumperDamage/SetPanelDamage/SetDoorDamage (0x6B1350/0x6B1480/0x6B1600), en el EXE y en la DLL.
// Se compara el retorno (EAX completo donde el EXE lo define, AL en los bool), los 0x18 bytes del gestor, el
// hash de todo el búfer y del vehículo, las salidas de GetComponentGroup y la traza. No se toca ningún global.
// Fuera de los fixtures: componentes negativos grandes en ApplyDamage (el EXE indexa la tabla 0x8D32A0 con el
// grupo sin inicializar = el argumento) e índices de rueda que salgan del búfer.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "DamageManager.h"
#include "Automobile.h"
#include "Door.h"
#include "General.h"
#include "CutsceneMgr.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"

namespace Damage27Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- fixtures ---
constexpr size_t DMOFF=0x80;
alignas(16) inline std::array<uint8,0x200> buf{};
alignas(16) inline std::array<uint8,0xA00> veh{};
alignas(16) inline std::array<uint8,0x10> outs{};
inline uint8* Dm() { return buf.data()+DMOFF; }
inline uint32 Off(const void* p) {
    const auto v=uintptr_t(p);
    if(v>=uintptr_t(veh.data())&&v<uintptr_t(veh.data())+veh.size()) return 0x01000000u|uint32(v-uintptr_t(veh.data()));
    if(v>=uintptr_t(buf.data())&&v<uintptr_t(buf.data())+buf.size()) return 0x02000000u|uint32(v-uintptr_t(buf.data()));
    return 0xEEEEEEEEu;
}

// --- traza sin reservas ---
struct Ev { uint32 k,a,b; }; inline std::array<Ev,64> evs{}; inline int nev{};
inline void Push(uint32 k,uint32 a,uint32 b) { if(nev<int(evs.size()))evs[nev++]={k,a,b}; }
inline uint32 rs{};
inline int __cdecl FakeRand() { rs^=rs<<13; rs^=rs>>17; rs^=rs<<5; const int r=int(rs>>7)&0x7FFF; Push(1,uint32(r),0); return r; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline void __fastcall FakeOpen(void* door,void*,float ratio) { Push(2,Off(door),std::bit_cast<uint32>(ratio)); }
inline void __fastcall FakeBumper(void* v,void*,uint32 a,uint32 b) { Push(3,Off(v),(a&0xFF)|((b&0xFF)<<8)); }
inline void __fastcall FakePanel(void* v,void*,uint32 a,uint32 b) { Push(4,Off(v),(a&0xFF)|((b&0xFF)<<8)); }
inline void __fastcall FakeDoorDmg(void* v,void*,uint32 a,uint32 b) { Push(5,Off(v),(a&0xFF)|((b&0xFF)<<8)); }

// --- hooks y parches ---
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
// cuerpo de una función de la DLL: sigue los saltos del ILT mientras no vuelvan al EXE
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class M> inline uintptr_t MAddr(M m) { static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=27;
inline const std::array<Fn,NFNS> fns{{
    {"CDamageManager","ApplyDamage",0x6C24B0,0,-1,1024},{"CDamageManager","FuckCarCompletely",0x6C25D0,0,-1,512},
    {"CDamageManager","GetAeroplaneCompStatus",0x6C2300,0,-1,256},{"CDamageManager","GetCarNodeIndexFromDoor",0x6C26F0,0,-1,128},
    {"CDamageManager","GetCarNodeIndexFromPanel",0x6C26A0,0,-1,128},{"CDamageManager","GetComponentGroup",0x6C2040,0,-1,512},
    {"CDamageManager","GetDoorStatus",0x6C2230,0,-1,256},{"CDamageManager","GetDoorStatus_Component",0x6C2250,0,-1,256},
    {"CDamageManager","GetEngineStatus",0x6C22C0,0,-1,64},{"CDamageManager","GetLightStatus",0x6C2130,0,-1,256},
    {"CDamageManager","GetPanelStatus",0x6C2180,0,-1,256},{"CDamageManager","GetWheelStatus",0x6C21B0,0,-1,256},
    {"CDamageManager","Init",0x6A0520,0,-1,64},{"CDamageManager","ProgressAeroplaneDamage",0x6C2460,0,-1,512},
    {"CDamageManager","ProgressDoorDamage",0x6C2320,0,-1,512},{"CDamageManager","ProgressEngineDamage",0x6C23B0,0,-1,64},
    {"CDamageManager","ProgressPanelDamage",0x6C23C0,0,-1,512},{"CDamageManager","ProgressWheelDamage",0x6C2440,0,-1,256},
    {"CDamageManager","ResetDamageStatus",0x6C20E0,0,-1,64},{"CDamageManager","ResetDamageStatusAndWheelDamage",0x6A04E0,0,-1,64},
    {"CDamageManager","SetAeroplaneCompStatus",0x6C22D0,0,-1,256},{"CDamageManager","SetDoorStatus-",0x6C21C0,0,-1,256},
    {"CDamageManager","SetDoorStatus_Component",0x6C21E0,0,-1,256},{"CDamageManager","SetEngineStatus",0x6C22A0,0,-1,256},
    {"CDamageManager","SetLightStatus",0x6C2100,0,-1,256},{"CDamageManager","SetPanelStatus",0x6C2150,0,-1,256},
    {"CDamageManager","SetWheelStatus",0x6C21A0,0,-1,256},
}};

template<class R,class... A> inline R Th(uintptr_t a,void* self,A... args) { return reinterpret_cast<R(__thiscall*)(void*,A...)>(a)(self,args...); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"damage27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"damage27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("damage27");}
    if(!FindPlayerPed() || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"damage27-only.txt"); if(f)std::getline(f,only); }
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(auto& fn:fns){ auto* h=FindHook(fn.cls,fn.name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fn.cls)+"::"+fn.name}});return;} hooks.push_back(h); }

    // parches: primero se resuelven todos los cuerpos de la DLL, después se escribe (nunca sobre un stub)
    struct P { uintptr_t at; void* stub; };
    std::vector<P> plan;
    auto add=[&](uintptr_t exeAddr,uintptr_t dllSym,void* stub){ if(exeAddr)plan.push_back({exeAddr,stub}); if(dllSym){ const auto b=Body(dllSym); if(!InExe(b))plan.push_back({b,stub}); } };
    add(0x821B1E,0,reinterpret_cast<void*>(&FakeRand));
    add(0,uintptr_t(&CGeneral::GetRandomNumber),reinterpret_cast<void*>(&FakeRandomNumber));
    add(0x6F4790,MAddr(&CDoor::Open),reinterpret_cast<void*>(&FakeOpen));
    add(0x6B1350,MAddr(&CAutomobile::SetBumperDamage),reinterpret_cast<void*>(&FakeBumper));
    add(0x6B1480,MAddr(&CAutomobile::SetPanelDamage),reinterpret_cast<void*>(&FakePanel));
    add(0x6B1600,MAddr(&CAutomobile::SetDoorDamage),reinterpret_cast<void*>(&FakeDoorDmg));
    { std::vector<uintptr_t> stubs; for(auto& p:plan)stubs.push_back(Body(uintptr_t(p.stub)));
      std::vector<P> clean; for(auto& p:plan){ bool dup=false; for(auto& q:clean)dup|=q.at==p.at; for(auto s:stubs)dup|=s==p.at||uintptr_t(p.stub)==p.at; if(!dup)clean.push_back(p); } plan=clean; }

    std::vector<bool> enabled(hooks.size()),locked(hooks.size());
    for(size_t f=0;f<hooks.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);hooks[f]->State(false);}
    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        for(auto& p:plan)patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(p.at,p.stub));
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/16777216.f; };
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xDA27u; if(!st)st=1;
                    nev=0; rs=next()|1u;
                    for(auto& b:buf)b=uint8(next());
                    for(auto& b:veh)b=uint8(next());
                    for(auto& b:outs)b=uint8(next());
                    uint8* dm=Dm();
                    if(next()%4){ // estado coherente; si no, bytes aleatorios
                        dm[4]=uint8(next()%4==0 ? next() : next()%251);
                        for(int k=0;k<4;++k)dm[5+k]=uint8(next()%5==0 ? next()%8 : next()%3);
                        for(int k=0;k<6;++k)dm[9+k]=uint8(next()%6==0 ? next()%8 : next()%5);
                        uint32 l=next(); if(next()%2)l&=0x55u; At<uint32>(dm,0x10)=l;
                        uint32 p=0; for(int k=0;k<8;++k)p|=uint32(next()%6==0 ? next()%16 : next()%5)<<(4*k); At<uint32>(dm,0x14)=p;
                    }
                    if(next()%5) veh[0x868]|=0x10; else veh[0x868]&=uint8(~0x10);
                    void* v=veh.data();

                    // argumentos (deterministas, iguales en las dos ejecuciones)
                    auto upper=[&](uint32 lo){ return next()%3==0 ? (next()&0xFFFFFF00u)|(lo&0xFFu) : lo; };
                    auto smallIdx=[&](uint32 n){ const uint32 r=next()%8; return r<5 ? next()%n : r==5 ? next()%(n+6) : r==6 ? next() : 0x100u+next()%n; };

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(nm=="ApplyDamage"){
                        int32 comp; const uint32 r=next()%8;
                        if(r<5)comp=int32(next()%18); else if(r==5)comp=int32(next()%33)-8; else if(r==6)comp=int32(16+next()%0x10000);
                        else { static const int32 cs[]{0x7FFFFFFF,0x10F,0x100,15,11,16}; comp=cs[next()%std::size(cs)]; }
                        float inten; const uint32 q=next()%10;
                        if(q<6)inten=uni(0.f,400.f); else if(q==6)inten=uni(40.f,130.f); else if(q==7)inten=std::bit_cast<float>(next()%2 ? 0x7FC00000u : 0xFFC00000u);
                        else if(q==8){ static const uint32 fs[]{0x7F800000u,0xFF800000u,0u,0x80000000u,0x43160000u,0x42700000u}; inten=std::bit_cast<float>(fs[next()%std::size(fs)]); }
                        else inten=uni(-500.f,2000.f);
                        const float mult=uni(0.f,2.f);
                        out["args"]={comp,std::bit_cast<uint32>(inten)};
                        ret=Th<uint8>(a,dm,v,comp,inten,mult);
                    }
                    else if(nm=="FuckCarCompletely"){ const uint32 lo=next()%4==0 ? next()&0xFF : next()%2; const uint32 arg=upper(lo); out["args"]=arg; Th<void>(a,dm,arg); }
                    else if(nm=="GetComponentGroup"){
                        const uint32 r=next()%6; const int32 comp=r<4 ? int32(next()%33)-8 : r==4 ? int32(next()) : int32(next()%0x200);
                        out["args"]=comp;
                        ret=Th<uint8>(a,dm,comp,reinterpret_cast<int32*>(outs.data()),outs.data()+8);
                    }
                    else if(nm=="Init"||nm=="ResetDamageStatusAndWheelDamage"){ ret=Off(Th<void*>(a,dm)); }
                    else if(nm=="ResetDamageStatus"){ Th<void>(a,dm); }
                    else if(nm=="GetEngineStatus"){ ret=Th<uint32>(a,dm); }
                    else if(nm=="SetEngineStatus"){ const uint32 r=next()%5; const uint32 s=r<2 ? next()%300 : r==2 ? next() : r==3 ? 249u+next()%3 : 0x100u+next()%0x100; out["args"]=s; Th<void>(a,dm,s); }
                    else if(nm=="ProgressEngineDamage"){ const uint32 s=next(); out["args"]=s; ret=Th<uint8>(a,dm,s); }
                    else if(nm=="GetCarNodeIndexFromPanel"||nm=="GetCarNodeIndexFromDoor"){
                        const uint32 x=smallIdx(nm=="GetCarNodeIndexFromPanel" ? 7 : 6); out["args"]=x;
                        ret=reinterpret_cast<int32(__cdecl*)(uint32)>(a)(x);
                    }
                    else if(nm=="GetDoorStatus"){ const uint32 r=next()%6; const int32 d=r<4 ? int32(next()%12)-3 : r==4 ? int32(next()) : int32(0x100+next()%6); out["args"]=d; ret=Th<uint32>(a,dm,d); }
                    else if(nm=="SetDoorStatus-"){ const uint32 r=next()%6; const int32 d=r<4 ? int32(next()%12)-3 : r==4 ? int32(next()) : int32(0x100+next()%6);
                        const uint32 s=next()%3 ? next()%5 : next(); out["args"]={d,s}; Th<void>(a,dm,d,s); }
                    else if(nm=="GetDoorStatus_Component"){ const uint32 r=next()%6; const uint32 c=r<4 ? next()%20 : r==4 ? next() : 0x100u+next()%20; out["args"]=c; ret=Th<uint32>(a,dm,c); }
                    else if(nm=="SetDoorStatus_Component"){ const uint32 r=next()%6; const uint32 c=r<4 ? next()%20 : r==4 ? next() : 0x100u+next()%20;
                        const uint32 s=next()%3 ? next()%5 : next(); out["args"]={c,s}; Th<void>(a,dm,c,s); }
                    else if(nm=="GetWheelStatus"||nm=="SetWheelStatus"){
                        // índice dword sin límites: dentro del búfer (dm-0x80+5 .. dm+0x17B)
                        const uint32 r=next()%4; const int32 w=r<2 ? int32(next()%4) : r==2 ? int32(next()%8)-2 : int32(next()%0x1C0)-0x80;
                        if(nm=="GetWheelStatus"){ out["args"]=w; ret=Th<uint32>(a,dm,w); }
                        else { const uint32 s=next()%3 ? next()%4 : next(); out["args"]={w,s}; Th<void>(a,dm,w,s); }
                    }
                    else if(nm=="ProgressWheelDamage"){ const uint32 w=upper(next()%3 ? next()%4 : next()&0xFF); out["args"]=w; ret=Th<uint8>(a,dm,w); }
                    else if(nm=="GetPanelStatus"||nm=="GetLightStatus"){ const uint32 x=upper(next()%3 ? next()%8 : next()&0xFF); out["args"]=x; ret=Th<uint32>(a,dm,x); }
                    else if(nm=="SetPanelStatus"||nm=="SetLightStatus"){ const uint32 x=upper(next()%3 ? next()%8 : next()&0xFF);
                        const uint32 r=next()%4; const uint32 s=r<2 ? next()%5 : r==2 ? next()%20 : next(); out["args"]={x,s}; Th<void>(a,dm,x,s); }
                    else if(nm=="ProgressPanelDamage"){ const uint32 x=upper(next()%4 ? next()%7 : next()&0xFF); out["args"]=x; ret=Th<uint8>(a,dm,x); }
                    else if(nm=="ProgressDoorDamage"){ const uint32 d=upper(next()%4 ? next()%6 : next()&0xFF); out["args"]=d; ret=Th<uint8>(a,dm,d,v); }
                    else if(nm=="GetAeroplaneCompStatus"||nm=="ProgressAeroplaneDamage"){ const uint32 x=upper(next()%4 ? 12+next()%16 : next()&0xFF); out["args"]=x;
                        if(nm=="GetAeroplaneCompStatus")ret=Th<uint32>(a,dm,x); else ret=Th<uint8>(a,dm,x); }
                    else if(nm=="SetAeroplaneCompStatus"){ const uint32 x=upper(next()%4 ? 12+next()%16 : next()&0xFF);
                        const uint32 r=next()%4; const uint32 s=r<2 ? next()%4 : r==2 ? next()%16 : next(); out["args"]={x,s}; Th<void>(a,dm,x,s); }

                    for(auto* h:hooks)h->State(false);
                    out["dm"]=ReversingProbe::Bytes(dm,0x18);
                    out["buf"]=Fnv(buf.data(),buf.size()); out["veh"]=Fnv(veh.data(),veh.size());
                    out["outs"]=ReversingProbe::Bytes(outs.data(),outs.size());
                    out["ret"]=ret;
                    { json t=json::array(); for(int k=0;k<nev;++k)t.push_back({evs[k].k,evs[k].a,evs[k].b}); out["trace"]=t; }
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,val]:original.items())if(candidate[k]!=val)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t k=0;k<hooks.size();++k){hooks[k]->State(enabled[k]);hooks[k]->LockState(locked[k]);}
    bool restored=true; for(size_t k=0;k<hooks.size();++k)restored&=hooks[k]->Hooked()==enabled[k];
    for(auto& p:plan)restored&=*reinterpret_cast<const uint8*>(p.at)!=0xE9||!(uintptr_t(p.at)+5+*reinterpret_cast<const int32*>(p.at+1)==Body(uintptr_t(p.stub))||uintptr_t(p.at)+5+*reinterpret_cast<const int32*>(p.at+1)==uintptr_t(p.stub));
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
