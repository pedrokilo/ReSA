#pragma once
// Banco dentro del motor: CLoadMonitor (7). Original (hooks desactivados) frente a candidato sobre un
// objeto propio; CTimer::GetCurrentTimeInCycles/GetCyclesPerMillisecond controlados y el tiempo del
// juego fijado por caso. Se compara el objeto completo (flotantes bit a bit) y el global g_LoadMonitor.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "LoadMonitor.h"
#include "../ProbeRuntime.hpp"

namespace LoadMonLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
alignas(16) inline std::array<uint8,sizeof(CLoadMonitor)+16> object{};
inline uint32 cycles{}, cyclesPerMs{}; inline int cycleCalls{};
inline uint32 __cdecl Cycles() { ++cycleCalls; return cycles+=uint32(cycleCalls)*977u; }
inline uint32 __cdecl CyclesPerMs() { return cyclesPerMs; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CLoadMonitor")for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,7> fns{{{"Constructor",0x53CFA0,64},{"Destructor",0x856430,32},{"BeginFrame",0x53D030,128},{"EndFrame",0x53D0B0,4096},
                                    {"StartTimer",0x53D050,256},{"EndTimer",0x53D070,256},{"Render",0x53D480,32}}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"loadmon-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"loadmon-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("loadmon");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,7> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    auto& global=*reinterpret_cast<CLoadMonitor*>(0xB72978); const auto savedGlobal=ReversingProbe::Bytes(&global,sizeof(CLoadMonitor));
    const auto savedTime=CTimer::m_snTimeInMilliseconds;
    { const unsigned pc=_control87(0,0)&_MCW_PC; Write({{"event","info"},{"precision_control",pc},{"precision_bits",pc==_PC_24 ? 24 : pc==_PC_53 ? 53 : 64}}); }
    std::array<bool,7> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        ReversingProbe::EntryPatch c1(std::bit_cast<uintptr_t>(&CTimer::GetCurrentTimeInCycles),Cycles), c1e(0x561A80,Cycles);
        ReversingProbe::EntryPatch c2(std::bit_cast<uintptr_t>(&CTimer::GetCyclesPerMillisecond),CyclesPerMs), c2e(0x561A40,CyclesPerMs);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x10ADu; cycles=next(); cyclesPerMs=1+next()%5000000; cycleCalls=0;
                    for(auto& b:object)b=uint8(next());
                    auto* w=reinterpret_cast<uint32*>(object.data());
                    // campos con rangos coherentes (los flotantes como floats reales, no bits aleatorios)
                    const size_t words=sizeof(CLoadMonitor)/4;
                    for(size_t k=0;k<words;++k){ const uint32 m=next()%4; if(m==0)w[k]=std::bit_cast<uint32>(float(next()%100000)/37.f); else if(m==1)w[k]=next()%5000; else w[k]&=0x7FFFFFFF; } // ciclos realistas (< 2^31)
                    for(size_t k=0x5C/4;k<0x9C/4;++k)w[k]=std::bit_cast<uint32>(float(next()%2000000)/7.f); // floats reales (sin denormales)
                    CTimer::m_snTimeInMilliseconds=w[8]+(next()%3==0 ? next()%500 : next()%3==0 ? 1000+next()%1000 : 2001+next()%5000); // m_LastTime en +0x20
                    const auto globalBefore=ReversingProbe::Bytes(&global,sizeof(CLoadMonitor));
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm=fn.name; void* self=object.data();
                    if(nm=="StartTimer"||nm=="EndTimer")reinterpret_cast<void(__thiscall*)(void*,int)>(fn.address)(self,int(next()%4));
                    else reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);
                    const auto globalAfter=ReversingProbe::Bytes(&global,sizeof(CLoadMonitor));
                    std::memcpy(&global,globalBefore.data(),globalBefore.size());
                    return {{"object",ReversingProbe::Bytes(object.data(),sizeof(CLoadMonitor))},{"global_changed",globalAfter!=globalBefore},{"cycles",cycleCalls},{"switches",switches}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CLoadMonitor::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",nullptr},{"candidate",nullptr}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CLoadMonitor::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string("CLoadMonitor::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    CTimer::m_snTimeInMilliseconds=savedTime; std::memcpy(&global,savedGlobal.data(),savedGlobal.size());
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",ReversingProbe::Bytes(&global,sizeof(CLoadMonitor))==savedGlobal}});
}
}
