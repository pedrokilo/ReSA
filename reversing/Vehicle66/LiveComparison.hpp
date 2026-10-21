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

// Banco par a par Vehicle66 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Cinco rutas: RemoveVehicleUpgrade (0x6DF930), AddUpgrade (0x6DFA20), AddVehicleUpgrade (0x6E3290), PossiblyDropFreeFallBombForPlayer (0x6E07E0) y ProcessSirenAndHorn (0x6E0950).
// Los colaboradores (ver Core.hpp) se sustituyen por falsos identicos en ambos lados.
namespace V66Live {
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
    if(!std::filesystem::exists(dir/"vehicle66-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vehicle66-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vehicle66");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json ownHooks=json::object();
    dllRemoveCb=uintptr_t(&RemoveUpgradeCB);
    dllVisObj=uintptr_t(static_cast<RwObject*(*)(RwObject*,void*)>(&SetVehicleAtomicVisibilityCB));
    dllVisFrame=uintptr_t(static_cast<RwFrame*(*)(RwFrame*,void*)>(&SetVehicleAtomicVisibilityCB));
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
    if(!ready){ // no continuar sin las cinco rutas
        Run(env,false,registered,bodies,collaborators);return;
    }
    const Pair pClear=MakeCollab(env,kClear,hooksFound),pFindOther=MakeCollab(env,kFindOther,hooksFound),pRemoveRepl=MakeCollab(env,kRemoveRepl,hooksFound),pForAll=MakeCollab(env,kForAllAtomics,hooksFound);
    const Pair pGetFrame=MakeCollab(env,kGetFrame,hooksFound),pCreate=MakeCollab(env,kCreateAtomic,hooksFound),pObjects=MakeCollab(env,kFrameObjects,hooksFound),
        pChildren=MakeCollab(env,kFrameChildren,hooksFound),pPlate=MakeCollab(env,kPlate,hooksFound);
    const Pair pSetFlags=MakeCollab(env,kSetFlags,hooksFound),pGetRepl=MakeCollab(env,kGetRepl,hooksFound),pAddRepl=MakeCollab(env,kAddRepl,hooksFound),pGetUp=MakeCollab(env,kGetUpgrade,hooksFound);
    // AddUpgrade es una de las funciones probadas: su ruta ya existe (hook apagado); su cuerpo en la DLL es el de esa ruta
    const Pair pAddUp{kEntry[1],Body(env.route[1]->Body()),false};
    const Pair pRate=MakeCollab(env,kRate,hooksFound),pOrd=MakeCollab(env,kOrdPos,hooksFound),pProj=MakeCollab(env,kAddProjectile,hooksFound),pShake=MakeCollab(env,kStartShake,hooksFound);
    const Pair pSiren=MakeCollab(env,kUsesSiren,hooksFound),pHorn=MakeCollab(env,kGetHorn,hooksFound);
    auto S=[](const Pair& p,void* fake){return StubSpec{p,fake};};
    env.stubs[kRemove]={S(pClear,reinterpret_cast<void*>(&FkClear)),S(pFindOther,reinterpret_cast<void*>(&FkFindOther)),S(pRemoveRepl,reinterpret_cast<void*>(&FkRemoveRepl)),S(pForAll,reinterpret_cast<void*>(&FkForAll))};
    env.stubs[kAddUp]={S(pGetFrame,reinterpret_cast<void*>(&FkGetFrame)),S(pForAll,reinterpret_cast<void*>(&FkForAll)),S(pCreate,reinterpret_cast<void*>(&FkCreateAtomic)),
        S(pObjects,reinterpret_cast<void*>(&FkFrameObjects)),S(pChildren,reinterpret_cast<void*>(&FkFrameChildren)),S(pPlate,reinterpret_cast<void*>(&FkPlate))};
    env.stubs[kAddVeh]={S(pSetFlags,reinterpret_cast<void*>(&FkSetFlags)),S(pFindOther,reinterpret_cast<void*>(&FkFindOther)),S(pGetRepl,reinterpret_cast<void*>(&FkGetRepl)),
        S(pAddRepl,reinterpret_cast<void*>(&FkAddRepl)),S(pGetUp,reinterpret_cast<void*>(&FkGetUpgrade)),S(pAddUp,reinterpret_cast<void*>(&FkAddUpgrade))};
    env.stubs[kDrop]={S(pRate,reinterpret_cast<void*>(&FkRate)),S(pOrd,reinterpret_cast<void*>(&FkOrdPos)),S(pProj,reinterpret_cast<void*>(&FkProjectile)),S(pShake,reinterpret_cast<void*>(&FkShake))};
    env.stubs[kSiren]={S(pSiren,reinterpret_cast<void*>(&FkUsesSiren)),S(pHorn,reinterpret_cast<void*>(&FkGetHorn))};
    std::set<uintptr_t> seen;
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            if(seen.insert(s.pair.exe).second)env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"CPed::IsPlayer 0x5DF8F0","CPad::GetPad 0x53FB70"})},{"stubbed",stubbed},{"hooks_found",hooksFound},{"own_hook",ownHooks[kFn[f]]}};
    }
    Run(env,ready,registered,bodies,collaborators);
}
}
