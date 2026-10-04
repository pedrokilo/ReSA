#pragma once
// Banco dentro del motor: constructores de CAEWeaponAudioEntity (0x507560, vtable 0x862E68) y de
// CAEPedWeaponAudioEntity (0x5DE990, vtable 0x86C2AC). Original (hooks desactivados) frente a candidato (envoltorios
// de la DLL que construyen en el sitio): cada constructor se llama por su entrada del EXE sobre un búfer relleno con
// 0xCD (o con bytes pseudoaleatorios en parte de los casos, para ver qué bytes no se escriben), con el objeto en
// distintas alineaciones dentro del búfer. Se compara el búfer completo (objeto + guardas antes y después), el valor
// devuelto (relativo al búfer) y la vptr por tipo (vtable del EXE o la exportada por la DLL).
// Los constructores no llaman a nada: no hace falta simular funciones. Se vigilan las vtables del EXE y las entradas
// parcheadas; el estado de los hooks se restaura al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Entities/AEWeaponAudioEntity.h"
#include "Audio/Entities/AEPedWeaponAudioEntity.h"
#include "../ProbeRuntime.hpp"

namespace WeapAudioLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }

constexpr uintptr_t WEAPON_VT=0x862E68, PED_WEAPON_VT=0x86C2AC;
constexpr size_t PED_SIZE=0xA8, GUARD=16, BUF_SIZE=GUARD+8+PED_SIZE+GUARD;
alignas(16) inline std::array<uint8,BUF_SIZE> buf{};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=2;
inline const std::array<Fn,NFN> fns{{
    {"CAEWeaponAudioEntity","Constructor",0x507560,0,-1,512},
    {"CAEPedWeaponAudioEntity","Constructor",0x5DE990,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"weapaudio-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"weapaudio-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weapaudio");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dirPath/"weapaudio-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f){
        hooks[f]=FindHook(fns[f].cls,fns[f].name);
        const bool vmt=fns[f].slot>=0;
        if(!hooks[f]||(hooks[f]->Symbol()[0]=='V')!=vmt){Write({{"event","error"},{"reason","hook not found or wrong kind"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    }
    // vtable de la DLL (exportada) de CAEWeaponAudioEntity; CAEPedWeaponAudioEntity no exporta la suya.
    const uintptr_t dllWeaponVt=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CAEWeaponAudioEntity"));
    auto vptrType=[&](uintptr_t v)->json{
        if(v==WEAPON_VT||v==dllWeaponVt)return "CAEWeaponAudioEntity";
        if(v==PED_WEAPON_VT)return "CAEPedWeaponAudioEntity";
        return json(uint32(v)); };

    std::vector<Saved> savedAll{ Save(WEAPON_VT,4), Save(PED_WEAPON_VT,8), Save(0x507560,5), Save(0x5DE990,5) };
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;

    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5EAu; if(!stt)stt=1;
                for(auto* h:hooks)h->State(candidate);
                // Como pide el banco: el conmutador se comprueba después de fijar el estado de la ejecución
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                json out={{"switches",switches}};
                // relleno: 0xCD en la mitad de los casos, bytes pseudoaleatorios en el resto
                const uint32 mode=next()%4;
                if(mode<2) buf.fill(0xCD);
                else for(auto& b:buf)b=uint8(next()>>11);
                // el objeto empieza en GUARD + 0, 1, 2, 4 u 8 bytes (alineaciones distintas)
                static constexpr size_t shifts[]{0,1,2,4,8};
                const size_t shift=shifts[next()%5];
                uint8* obj=buf.data()+GUARD+shift;
                void* r=reinterpret_cast<void*(__thiscall*)(void*)>(fn.address)(obj);
                out["ret"]=r==obj ? json("this") : json(uint32(uintptr_t(r)));
                out["shift"]=shift; out["mode"]=mode;
                out["vptr"]=vptrType(At<uintptr_t>(obj,0));
                auto bytes=ReversingProbe::Bytes(buf.data(),buf.size());
                std::fill_n(bytes.begin()+GUARD+shift,4,0); // vptr comparada por tipo
                out["buf"]=bytes;
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
