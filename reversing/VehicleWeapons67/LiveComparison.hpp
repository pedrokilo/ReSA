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

// Banco par a par VehicleWeapons67 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CVehicle::ProcessWeapons (0x6E3950).
// Los colaboradores (ver Core.hpp) se sustituyen por falsos identicos en ambos lados.
namespace V67Live {
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
    if(!std::filesystem::exists(dir/"vehicleweapons67-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vehicleweapons67-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vehicleweapons67");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    // Ruta RH de la funcion probada: se construye ANTES que nada; el constructor deja el hook apagado. El cuerpo de la DLL se calcula aqui, antes de crear las rutas de los
    // colaboradores y los parches de entrada.
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
    if(!ready){ // no continuar sin la ruta
        Run(env,false,registered,bodies,collaborators);return;
    }
    const Pair pFindVeh=MakeCollab(env,aFindVeh,hooksFound),pClear=MakeCollab(env,aClearCross,hooksFound),pFiring=MakeCollab(env,aFiringStatus,hooksFound),pSelect=MakeCollab(env,aSelect,hooksFound),
        pGuns=MakeCollab(env,aFireGuns,hooksFound),pBomb=MakeCollab(env,aBomb,hooksFound),pHeat=MakeCollab(env,aHeat,hooksFound),pUnguided=MakeCollab(env,aUnguided,hooksFound),
        pNumGuns=MakeCollab(env,aNumGuns,hooksFound),pSetMatrix=MakeCollab(env,aSetMatrix,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[0]={S(pFindVeh,reinterpret_cast<void*>(&FkFindVeh)),S(pClear,reinterpret_cast<void*>(&FkClearCross)),S(pFiring,reinterpret_cast<void*>(&FkFiring)),
        S(pSelect,reinterpret_cast<void*>(&FkSelect)),S(pGuns,reinterpret_cast<void*>(&FkFireGuns)),S(pBomb,reinterpret_cast<void*>(&FkBomb)),S(pHeat,reinterpret_cast<void*>(&FkHeat)),
        S(pUnguided,reinterpret_cast<void*>(&FkUnguided)),S(pNumGuns,reinterpret_cast<void*>(&FkNumGuns)),S(pSetMatrix,reinterpret_cast<void*>(&FkSetMatrix))};
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
