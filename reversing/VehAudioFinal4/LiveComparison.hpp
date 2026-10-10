#pragma once
// Banco dentro del motor (flujo Small2/PadMouse1/VehModelInfoFinal4): CAEVehicleAudioEntity, 102 funciones (lista final 4).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato (hooks activos) sobre un mundo falso y determinista:
// entidades de audio falsas (vptr 0x862CEC), vehiculos falsos de cualquier tipo, matrices, transmisiones, peatones y sonidos en
// buffers estaticos (Support.hpp). El gestor de sonidos (RequestNewSound y consultas), el hardware (IsSoundBankLoaded/LoadSoundBank),
// el azar (0x821B1E + CGeneral::GetRandomNumber), el pad, el entorno de audio, las referencias de entidad y la radio se simulan con
// trazas (direccion del EXE + simbolo de la DLL). No hay E/S de audio real. Cada global tocado se guarda y se restaura en cada
// ejecucion. Funciones repartidas en seis grupos (CasesG1..G6.hpp). Ver README.md.
#include "CasesG1.hpp"
#include "CasesG2.hpp"
#include "CasesG3.hpp"
#include "CasesG4.hpp"
#include "CasesG5.hpp"
#include "CasesG6.hpp"

namespace VehAudioFinal4Live {
inline std::vector<Fn> AllFns(){
    std::vector<Fn> v;
    for(const auto* g:{&kFnsG1,&kFnsG2,&kFnsG3,&kFnsG4,&kFnsG5,&kFnsG6})v.insert(v.end(),g->begin(),g->end());
    return v;
}

// Resumen compacto y determinista (nunca nulo, tambien para funciones void): retorno, nº de eventos y hashes de traza, peticiones de
// sonido, mundo, globales y campos propios de la funcion.
inline json Summary(const json& o){
    const auto tr=o["trace"].dump(), all=o.dump();
    return json{{"ret",o["ret"]},{"events",o["trace"].size()},{"trace_fnv",Fnv64(tr.data(),tr.size())},{"requests",o["nreq"]},{"req_fnv",o["req_fnv"]},
                {"world",o["world"]},{"globals",o["globals"]},{"fields_fnv",Fnv64(all.data(),all.size())}};
}

inline bool Layout(){
    using E=CAEVehicleAudioEntity;
    bool ok=true; json l=json::object();
    auto chk=[&](const char* n,size_t got,size_t want){ const bool g=got==want; ok&=g; l[n]=g; };
    chk("sizeof_ent",sizeof(E),0x24C); chk("sizeof_snd",sizeof(CAESound),0x74); chk("sizeof_params",sizeof(E::tVehicleParams),0x4C); chk("sizeof_settings",sizeof(tVehicleAudioSettings),0x24);
    chk("sizeof_trans",sizeof(cTransmission),0x68);
    chk("tempSound",offsetof(E,m_tempSound),TEMP_SOUND_OFF); chk("doCountStalls",offsetof(E,m_DoCountStalls),0x7C); chk("auSettings",offsetof(E,m_AuSettings),0x80);
    chk("isInit",offsetof(E,m_IsInitialized),0xA4); chk("skid",offsetof(E,m_SkidSound),0x184);
    chk("params_accel",offsetof(E::tVehicleParams,ThisAccel),0x20); chk("params_brake",offsetof(E::tVehicleParams,ThisBrake),0x22);
    chk("snd_bank",offsetof(CAESound,m_BankSlot),0); chk("snd_len",offsetof(CAESound,m_Length),0x70);
    chk("snd_flags",offsetof(CAESound,m_Flags),0x56); chk("snd_vol",offsetof(CAESound,m_Volume),0x14);
    Write({{"event","layout"},{"ok",ok},{"checks",l}});
    return ok;
}

inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"vehaudiofinal4-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vehaudiofinal4-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vehaudiofinal4");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"vehaudiofinal4-only.txt"); if(f)std::getline(f,only); }

    const std::vector<Fn> fns=AllFns();
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(fns.size(),nullptr);
    for(size_t f=0;f<fns.size();++f){
        hooks[f]=FindHook(fns[f].cls,fns[f].name);
        const bool vmt=fns[f].slot>=0;
        if(!hooks[f]||(hooks[f]->Symbol()[0]=='V')!=vmt){Write({{"event","error"},{"reason","hook not found or wrong kind"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    }
    if(!Layout()){ Write({{"event","error"},{"reason","layout"}}); return; }

    // falsos: se registran (Body() de cada simbolo de la DLL) ANTES de crear ningun EntryPatch
    RegisterStdStubs(); StubsG1(); StubsG2(); StubsG3(); StubsG4(); StubsG5(); StubsG6();

    nregs=0; Reg(&w,sizeof(World));
    for(size_t k=0;k<NENT;++k)Reg(w.ent[k],ENT_BUF);
    for(size_t k=0;k<NVEH;++k)Reg(w.veh[k],VEH_SIZE);
    for(size_t k=0;k<NSND;++k)Reg(w.snd[k],SND_BUF);
    // Instantanea de globales (restauradas al final de cada ejecucion)
    saved.clear();
    Save(0xB6B980,0x180);         // estaticos de CAEVehicleAudioEntity (radio, ranuras dummy, contadores, suavizados)
    Save(0x8CBC00,0x600);                      // configuracion de audio de vehiculos (.data del EXE)
    Save(0xB7CB40,0x60);                       // reloj del juego
    SavesG1(); SavesG2(); SavesG3(); SavesG4(); SavesG5(); SavesG6();
    const uint32 vtSlot0=G<uint32>(VTABLE_VEH_AUDIO);
    std::vector<bool> enabled(fns.size()),locked(fns.size());
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    const unsigned savedFP=_controlfp(0,0);
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i}});
            auto run=[&](bool candidate)->json {
                Ctx c; c.fn=&fn; c.nm=fn.name; c.i=i; c.f=uint32(f); c.candidate=candidate;
                c.st=uint32(i+1)*2654435761u^uint32(f+1)*0x9E3779B9u^0xA4E1u; if(!c.st)c.st=1;
                std::memset(&w,0,sizeof(w));
                c.ResetFakes();
                for(auto* h:hooks)h->State(candidate);
                const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                const bool switches=fn.slot>=0 ? (InExe(vtTarget)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                c.a= fn.slot>=0 ? vtTarget : fn.address;
                c.out=json::object(); c.ret=nullptr;
                _controlfp(_PC_24,_MCW_PC);
                const bool handled=RunG1(c)||RunG2(c)||RunG3(c)||RunG4(c)||RunG5(c)||RunG6(c);
                _controlfp(savedFP,_MCW_PC|_MCW_RC);
                json out=c.out; out["switches"]=switches; out["handled"]=handled; out["ret"]=c.ret; out["trace"]=Recs();
                out["nreq"]=nreq; out["req_fnv"]=ReqsHash(); out["requests"]=Reqs();
                out["world"]=WorldHash(); out["globals"]=SavedHash();
                { json gr=json::array(); for(auto& sv:saved)gr.push_back(Fnv32(reinterpret_cast<void*>(sv.addr),sv.bytes.size())); out["gregions"]=gr; }
                RestoreAll();
                return out;
            };
            const auto original=run(false);
            static World originalWorld;
            std::memcpy(&originalWorld,&w,sizeof(w));
            const auto candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["handled"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(), vals=json::object();
                json dg=json::array(); if(original.contains("gregions")&&candidate.contains("gregions"))for(size_t q=0;q<saved.size()&&q<original["gregions"].size();++q)if(original["gregions"][q]!=candidate["gregions"][q])dg.push_back({{"addr",saved[q].addr},{"bytes",saved[q].bytes.size()}}); for(auto& [k,v]:original.items())if(candidate[k]!=v){ diff.push_back(k);
                       const auto so=v.dump(), sc=candidate[k].dump(); vals[k]={{"o",so.substr(0,600)},{"c",sc.substr(0,600)}}; }
                // Diagnostico de bytes sin cambiar el criterio de igualdad. Omite solo el escritorio ya excluido por WorldHash.
                json dw=json::array();
                if(original["world"]!=candidate["world"]){
                    const auto* ob=reinterpret_cast<const uint8*>(&originalWorld);
                    const auto* cb=reinterpret_cast<const uint8*>(&w);
                    for(size_t q=0;q<sizeof(w)&&dw.size()<64;++q){
                        const bool scratch=q<sizeof(w.ent)&&q%ENT_BUF>=TEMP_SOUND_OFF&&q%ENT_BUF<TEMP_SOUND_END;
                        if(!scratch&&ob[q]!=cb[q])dw.push_back({{"offset",q},{"o",ob[q]},{"c",cb[q]}});
                    }
                }
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"diff_values",vals},{"diff_globals",dg},{"diff_world",dw},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    const bool restored=AllRestored()&&_controlfp(0,0)==savedFP&&G<uint32>(VTABLE_VEH_AUDIO)==vtSlot0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
