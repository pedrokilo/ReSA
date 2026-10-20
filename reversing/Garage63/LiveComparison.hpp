#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Garage.h"
#include "World.h"
#include "X87Math.h"
#include "SimpleTransform.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par Garage63 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Nueve rutas de CGarage; todos los colaboradores se sustituyen por falsos identicos en ambos lados (parche en la entrada del EXE y en el cuerpo de la DLL).
namespace G63Layout {
static_assert(sizeof(CGarage) == 0xD8 && offsetof(CGarage, m_fLeftCoord) == 0x28 && offsetof(CGarage, m_fRightCoord) == 0x2C && offsetof(CGarage, m_fFrontCoord) == 0x30 && offsetof(CGarage, m_fBackCoord) == 0x34
              && offsetof(CGarage, m_vPosn) == 0 && offsetof(CGarage, m_fTopZ) == 0x1C && offsetof(CGarage, m_fDoorPosition) == 0x38 && offsetof(CGarage, m_nTimeToOpen) == 0x3C
              && offsetof(CGarage, m_nType) == 0x4C && offsetof(CGarage, m_nDoorState) == 0x4D && offsetof(CGarage, m_nFlags) == 0x4E, "CGarage");
}

namespace G63Live {
inline uintptr_t PointBody() {return Body(std::bit_cast<uintptr_t>(static_cast<bool (CGarage::*)(CVector)>(&CGarage::IsPointInsideGarage)));}
inline uintptr_t RadiusBody() {return Body(std::bit_cast<uintptr_t>(static_cast<bool (CGarage::*)(CVector,float)>(&CGarage::IsPointInsideGarage)));}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"garage63-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"garage63-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("garage63");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Write({{"event","stage"},{"n",1}});
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Rutas RH de las funciones bajo prueba: el constructor deja el hook apagado.
    for(unsigned f=0;f<kFns;++f) {
        env.route[f].reset(new HookRoute("CGarage",kName[f],kEntry[f],HookRoute::RegisteredTarget{}));
        registered[kFn[f]]=env.route[f]->Registered();bodies[kFn[f]]=env.route[f]->Body()!=0;
        ready&=env.route[f]->Registered()&&env.route[f]->Body()&&ModuleOf(env.route[f]->Body())==env.asi;
    }
    Write({{"event","stage"},{"n",2}});
    // Colaboradores: cada uno con su ruta RH (apagada) si tiene hook; el cuerpo de la DLL se parchea ademas de la entrada del EXE.
    struct C {const char* cls;const char* fn;uintptr_t exe;void* fake;uintptr_t body;};
    auto mk=[&](const C& c){
        Write({{"event","stage"},{"n",3},{"fn",c.fn?c.fn:"-"}});
        Pair p{c.exe,c.body,false};
        if(c.cls) {
            env.collab.emplace_back(new HookRoute(c.cls,c.fn,c.exe,HookRoute::RegisteredTarget{}));
            if(!p.dll&&env.collab.back()->Registered())p.dll=Body(env.collab.back()->Body());
            else if(!env.collab.back()->Registered()&&!c.body)p.dll=0;
        }
        return StubSpec{p,c.fake};
    };
    const StubSpec col=mk({"CEntity","GetColModel",0x535300,reinterpret_cast<void*>(&FkCol),0});
    const StubSpec mat=mk({"CPlaceable","GetMatrix",0x411990,reinterpret_cast<void*>(&FkMat),0});
    const StubSpec alloc=mk({"CPlaceable","AllocateMatrix",0x54F560,reinterpret_cast<void*>(&FkAlloc),0});
    const StubSpec upd=mk({"CSimpleTransform","UpdateMatrix",0x54F1B0,reinterpret_cast<void*>(&FkUpd),Body(std::bit_cast<uintptr_t>(&CSimpleTransform::UpdateMatrix))});
    const StubSpec xform=mk({nullptr,nullptr,0x59C890,reinterpret_cast<void*>(&FkXform),0});
    const StubSpec ipr=mk({"CGarage","IsPointInsideGarage-radius",0x4487D0,reinterpret_cast<void*>(&FkIpr),RadiusBody()});
    const StubSpec ipp=mk({"CGarage","IsPointInsideGarage-point",0x448740,reinterpret_cast<void*>(&FkIpp),PointBody()});
    const StubSpec touch=mk({"CGarage","IsEntityTouching3D",0x448EE0,reinterpret_cast<void*>(&FkTouch),0});
    const StubSpec find=mk({"CWorld","FindObjectsIntersectingCube",0x568DD0,reinterpret_cast<void*>(&FkFind),0});
    const StubSpec fpv=mk({"CWorld","FindPlayerVehicle",0x56E0D0,reinterpret_cast<void*>(&FkFpv),0});
    const StubSpec fpp=mk({"CWorld","FindPlayerPed",0x56E210,reinterpret_cast<void*>(&FkFpp),0});
    const StubSpec task=mk({"CTaskManager","FindActiveTaskByType",0x681740,reinterpret_cast<void*>(&FkTask),0});
    const StubSpec del=mk({"CVehicle","CanBeDeleted",0x6D1180,reinterpret_cast<void*>(&FkDel),0});
    const StubSpec rem=mk({"CWorld","Remove",0x563280,reinterpret_cast<void*>(&FkRemove),0});
    const StubSpec in3d=mk({"CGarage","IsEntityEntirelyInside3D",0x448BE0,reinterpret_cast<void*>(&FkIn3d),0});
    Write({{"event","stage"},{"n",4}});
    env.stubs[kInside3D]={col,mat,xform,ipr};
    Write({{"event","stage"},{"n",43}});
    env.stubs[kOutside]={col,alloc,upd,xform,ipr};
    env.stubs[kStatic]={fpv,fpp,task,in3d};
    env.stubs[kEmpty]={find,touch};
    env.stubs[kRemove]={touch,ipp,del,rem};
    env.stubs[kTidy]={ipp,rem};
    Write({{"event","stage"},{"n",44}});
    env.stubs[kTidyClose]={touch,col,xform,ipr,rem};
    Write({{"event","stage"},{"n",45}});
    for(const StubSpec* s:{&col,&mat,&alloc,&upd,&xform,&ipr,&ipp,&touch,&find,&fpv,&fpp,&task,&del,&rem,&in3d})env.watches.emplace_back(s->pair.exe,16);
    Write({{"event","stage"},{"n",41}});
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f])stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll!=0},{"hooked",s.pair.hooked}});
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    Write({{"event","stage"},{"n",42}});
    {
        const bool arena=uintptr_t(&W)+sizeof(W)<0x80000000u&&uintptr_t(&WRef)+sizeof(WRef)<0x80000000u;
        ready&=arena;
        if(!arena)Write({{"event","layout_error"},{"reason","arena del banco por encima de 0x80000000"}});
    }
    Write({{"event","stage"},{"n",5}});
    Run(env,ready,registered,bodies,collaborators);
}
}
