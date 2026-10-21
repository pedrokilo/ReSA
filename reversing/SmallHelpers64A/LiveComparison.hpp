#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "World.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par SmallHelpers64A dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cuatro rutas: GetMillisecondTime (0x5617C0), DIReleaseMouse (0x746F70), RemoveUpgradeCB (0x6D3300) y FindUpgradeCB (0x6D3370).
// Los colaboradores (RsTimer, GetAtomicId, GetModelInfo, RpClumpRemoveAtomic, RpAtomicDestroy, RwFrameDestroy, CBaseModelInfo::RemoveRef) se sustituyen por falsos identicos en ambos lados.
namespace SH64Live {
// Busca el hook Simple registrado para una direccion del EXE (categoria y nombre), para crear su ruta (apagada) y obtener el cuerpo de la DLL.
inline bool FindHook(uintptr_t exe, std::string& cat, std::string& name) {
    bool found = false;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        for (auto& h : c.Items())
            if (h->Type() == ReversibleHooks::ReversibleHook::Base::HookType::Simple
                && reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get())->GetHookGTAAddress()) == exe) {
                cat = std::string(c.Name()); name = std::string(h->Name()); found = true;
            }
    });
    return found;
}
// Colaborador: si la DLL tiene hook (apagado) se parchea tambien el cuerpo de la DLL; si no (dll = 0), la DLL llama a la direccion del EXE.
inline Pair MakeCollab(Env& env, uintptr_t exe, json& hooksFound) {
    Pair p{exe, 0, false};
    std::string cat, name;
    if (FindHook(exe, cat, name)) {
        env.collab.emplace_back(new HookRoute(cat.c_str(), name.c_str(), exe, HookRoute::RegisteredTarget{}));
        hooksFound.push_back({{"exe", exe}, {"class", cat}, {"name", name}});
        if (env.collab.back()->Registered()) p.dll = Body(env.collab.back()->Body());
    }
    return p;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"smallhelpers64a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"smallhelpers64a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("smallhelpers64a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Rutas RH de las funciones probadas: se construyen ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear
    // las rutas de los colaboradores y los parches de entrada.
    for(unsigned f=0;f<kFns;++f) {
        std::string cat,name;
        const bool found=FindHook(kEntry[f],cat,name);
        if(found)env.route[f].reset(new HookRoute(cat.c_str(),name.c_str(),kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=found&&env.route[f]->Registered();bodies[kFn[f]]=found&&env.route[f]->Body()!=0;
        ready&=found&&env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
        if(!found)ready=false;
        ownHooks[kFn[f]]=found?json({{"class",cat},{"name",name},{"address",kEntry[f]}}):json(nullptr);
    }
    json hooksFound=json::array();
    if(!ready){ // no continuar sin las cuatro rutas
        Run(env,false,registered,bodies,collaborators);return;
    }
    const Pair pTimer=MakeCollab(env,kRsTimer,hooksFound);
    // RsTimer del EXE es `jmp psTimer` (0x745270), que tiene hook en la DLL: se apaga (ruta de apoyo, sin falso) para que la entrada del EXE siga siendo codigo original
    MakeCollab(env,0x745270,hooksFound);
    const Pair pAtomId=MakeCollab(env,kGetAtomicId,hooksFound);
    const Pair pModel=MakeCollab(env,kGetModelInfo,hooksFound);
    const Pair pClump=MakeCollab(env,kClumpRemoveAtomic,hooksFound);
    const Pair pAtomDestroy=MakeCollab(env,kAtomicDestroy,hooksFound);
    const Pair pFrame=MakeCollab(env,kFrameDestroy,hooksFound);
    const Pair pRef=MakeCollab(env,kRemoveRef,hooksFound);
    env.stubs[kTimer]={StubSpec{pTimer,reinterpret_cast<void*>(&FkRsTimer)}};
    env.stubs[kRemove]={StubSpec{pAtomId,reinterpret_cast<void*>(&FkGetAtomicId)},StubSpec{pModel,reinterpret_cast<void*>(&FkGetModelInfo)},
        StubSpec{pClump,reinterpret_cast<void*>(&FkClumpRemove)},StubSpec{pAtomDestroy,reinterpret_cast<void*>(&FkAtomicDestroy)},
        StubSpec{pFrame,reinterpret_cast<void*>(&FkFrameDestroy)},StubSpec{pRef,reinterpret_cast<void*>(&FkRemoveRef)}};
    env.stubs[kFind]={StubSpec{pAtomId,reinterpret_cast<void*>(&FkGetAtomicId)},StubSpec{pModel,reinterpret_cast<void*>(&FkGetModelInfo)}};
    std::set<uintptr_t> seen;
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            if(seen.insert(s.pair.exe).second)env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
