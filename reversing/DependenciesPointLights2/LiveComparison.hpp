#pragma once
#include <fstream>
#include <float.h>
#include "PointLights.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"

namespace DependenciesPointLights2Live {
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { ReversingProbe::PumpMessages(); log << row.dump() << '\n'; log.flush(); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,2> fns{{
    {"CPointLights","Init",0x6FFB40,0,-1,512},
    {"CPointLights","RemoveLightsAffectingObject",0x6FFFE0,0,-1,512},
}};
inline const std::array<std::pair<uintptr_t,size_t>,3> globals{{
    {0xC3F040,0x830}, // guardas, resultados, contador/lista y posiciones cacheadas completas
    {0xC886E0,0x38}, // guarda, seis punteros de luces extra, contador, guarda
    {0xC17038,8},    // Scene: world y camera (no se desreferencian en Remove)
}};
alignas(16) inline std::array<std::array<uint8,16+sizeof(RpLight)+16>,6> lights{};
alignas(16) inline std::array<uint8,32> worldToken{};
struct ABIResult { bool stack, registers; };
inline ABIResult CallRaw(uintptr_t callAddress) {
    uintptr_t stackBefore{},stackAfter{},ebxBefore{},ebxAfter{},esiBefore{},esiAfter{},ediBefore{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        mov stackBefore,esp
        mov ebxBefore,ebx
        mov esiBefore,esi
        mov ediBefore,edi
        mov ebpBefore,ebp
        mov eax,callAddress
        call eax
        mov stackAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
    }
    return {stackBefore==stackAfter,ebxBefore==ebxAfter&&esiBefore==esiAfter&&ediBefore==ediAfter&&ebpBefore==ebpAfter};
}
inline void Update() {
    if (done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto dir=std::filesystem::path(exe).parent_path();
    if (!std::filesystem::exists(dir/"dependenciespointlights2-runtime.start")) return;
    if (!log.is_open()) {
        log.open(dir/"dependenciespointlights2-runtime.jsonl");
        ReversingProbe::InstallFailureCapture("dependenciespointlights2");
        Write({{"event","waiting_for_player"}});
    }
    if (!FindPlayerPed() || CCutsceneMgr::IsRunning()) return;
    done=true;
    OriginalDependencies499::HookRoute initRoute("CPointLights","Init",0x6FFB40,&CPointLights::Init);
    OriginalDependencies499::HookRoute removeRoute("CPointLights","RemoveLightsAffectingObject",0x6FFFE0,&CPointLights::RemoveLightsAffectingObject);
    std::array<OriginalDependencies499::HookRoute*,2> routes{&initRoute,&removeRoute};
    std::array<std::vector<uint8>,3> saved;
    for (size_t j=0;j<globals.size();++j)
        saved[j]=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);
    const auto savedLights=lights;
    const unsigned savedFP=_controlfp(0,0);
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entries;
    for (size_t f=0;f<fns.size();++f) {
        const uintptr_t symbol=f==0 ? reinterpret_cast<uintptr_t>(&CPointLights::Init) : reinterpret_cast<uintptr_t>(&CPointLights::RemoveLightsAffectingObject);
        for (auto at:{fns[f].address,symbol,routes[f]->Body()})
            entries.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    }
    auto restore=[&] {
        for (size_t j=0;j<globals.size();++j)
            std::memcpy(reinterpret_cast<void*>(globals[j].first),saved[j].data(),saved[j].size());
        lights=savedLights;
        _controlfp(savedFP,_MCW_PC|_MCW_RC);
    };
    auto restored=[&] {
        bool ok=lights==savedLights && _controlfp(0,0)==savedFP;
        for (size_t j=0;j<globals.size();++j)
            ok &= saved[j]==ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);
        for (auto& [at,bytes]:entries)
            ok &= bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
        return ok;
    };
    Write({{"event","begin"},{"functions",2},{"pairs",1024},
           {"registered_hook",initRoute.Registered()&&removeRoute.Registered()},
           {"collaborators_stubbed",false}});
    if (!initRoute.Registered() || !removeRoute.Registered()) {
        restore(); initRoute.Restore(); removeRoute.Restore();
        Write({{"event","error"},{"reason","Falta RH de Init o RemoveLightsAffectingObject"},{"state_restored",true}});
        return;
    }
    unsigned pair=0;
    bool passed=true,allRestored=true;
    for (size_t f=0;f<fns.size();++f) {
        int failures=0;
        const std::string name=std::string(fns[f].cls)+"::"+fns[f].name;
        for (unsigned fixture=0;fixture<512;++fixture,++pair) {
            auto run=[&](bool candidate) {
                restore();
                for (size_t j=0;j<globals.size();++j) {
                    auto* region=reinterpret_cast<uint8*>(globals[j].first);
                    for (size_t k=0;k<globals[j].second;++k) region[k]=uint8(fixture*29+k*11+j*71);
                }
                for (size_t k=0;k<lights.size();++k) {
                    auto& buffer=lights[k];
                    for (size_t b=0;b<buffer.size();++b) buffer[b]=uint8(fixture*7+k*31+b*13);
                    buffer[16]=rpLIGHT;
                    buffer[17]=uint8(fixture%3); // subtipo opaco: el helper sólo escribe flags (+2)
                    buffer[18]=uint8(fixture&255);
                }
                // Todas las seis entradas son válidas; perfiles cubren alias de los mismos RpLight.
                auto& pointers=StaticRef<RpLight*[6]>(0xC886F0);
                for (size_t slot=0;slot<6;++slot) {
                    const size_t actual=(fixture/256)==0 ? slot : slot%((fixture/64)%3+1);
                    pointers[slot]=reinterpret_cast<RpLight*>(lights[actual].data()+16);
                }
                StaticRef<uint32>(0xC88708)=fixture%9;
                StaticRef<uint32>(0xC3F0D0)=fixture%33;
                StaticRef<uintptr_t>(0xC17038)=uintptr_t(worldToken.data());
                std::memcpy(reinterpret_cast<void*>(0xC1703C),saved[2].data()+4,4);
                const auto sceneBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC17038),8);
                const auto initialCache=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[0].first),globals[0].second);
                const auto initialExtra=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[1].first),globals[1].second);
                const auto initialLights=lights;
                _controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
                bool routeOK=false,stackOK=false,registersOK=false;
                const uintptr_t callAddress=fns[f].address;
                routes[f]->Invoke(candidate,[&] {
                    routeOK=ReversingProbe::JumpsToDll(callAddress)==candidate;
                    const auto abi=CallRaw(callAddress);
                    stackOK=abi.stack;
                    registersOK=abi.registers;
                    return true;
                });
                const auto cacheAfter=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[0].first),globals[0].second);
                const auto extraAfter=ReversingProbe::Bytes(reinterpret_cast<void*>(globals[1].first),globals[1].second);
                bool effectOK=true;
                if (f==0) {
                    auto expected=initialCache;
                    std::fill(expected.begin()+0x10,expected.begin()+0x90,0);
                    std::fill(expected.begin()+0x94,expected.begin()+0x98,0);
                    std::fill(expected.begin()+0x6A0,expected.begin()+0x820,0);
                    effectOK=expected==cacheAfter && initialExtra==extraAfter && initialLights==lights;
                } else {
                    auto expectedExtra=initialExtra;
                    std::fill(expectedExtra.begin()+0x28,expectedExtra.begin()+0x2C,0);
                    auto expectedLights=initialLights;
                    for (size_t slot=0;slot<6;++slot) {
                        const size_t actual=(fixture/256)==0 ? slot : slot%((fixture/64)%3+1);
                        expectedLights[actual][18]=0;
                    }
                    effectOK=initialCache==cacheAfter && expectedExtra==extraAfter && expectedLights==lights;
                }
                json out{{"route_ok",routeOK},{"stack_restored",stackOK},{"nonvolatile_registers_preserved",registersOK},
                         {"effect_expected",effectOK},{"scene_unchanged",sceneBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC17038),8)},
                         {"cache_list_guards",cacheAfter},{"extra_pointers_count_guards",extraAfter},{"fake_lights_guards",lights}};
                restore();
                return out;
            };
            const auto original=run(false),candidate=run(true);
            const bool stateOK=restored(),equal=original==candidate;
            const bool accepted=equal&&stateOK&&original["route_ok"]==true&&original["stack_restored"]==true
                &&original["nonvolatile_registers_preserved"]==true&&original["effect_expected"]==true&&original["scene_unchanged"]==true;
            failures+=!accepted;passed&=accepted;allRestored&=stateOK;
            Write({{"event","comparison"},{"function",name},{"pair",pair},{"case",fixture},
                   {"equal",equal},{"accepted",accepted},{"state_restored",stateOK},{"original",original},{"candidate",candidate}});
        }
        Write({{"event","function_summary"},{"function",name},{"cases",512},{"failures",failures}});
    }
    restore();allRestored&=restored();
    allRestored&=initRoute.Restore();allRestored&=removeRoute.Restore();
    Write({{"event",passed&&allRestored ? "complete":"error"},{"pairs",pair},{"state_restored",allRestored}});
}
}
