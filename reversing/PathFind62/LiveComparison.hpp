#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "PathFind.h"
#include "MemoryMgr.h"
#include "World.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par PathFind62 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.
// Seis rutas Simple: CPathFind::FindNodeClosestToCoors (0x44F460), FindNthNodeClosestToCoors (0x44F8C0), FindNextNodeWandering (0x451B70), CompleteNewInterior (0x452270),
// FindParkingNodeInArea (0x4513F0) y GeneratePedCreationCoors_Interior (0x44ECA0).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp.
namespace PathFind62Layout {
static_assert(sizeof(CPathFind) == PathFind62Fix::kThisBytes, "sizeof(CPathFind)");
static_assert(sizeof(CPathNode) == PathFind62Fix::kNodeBytes && sizeof(CNodeAddress) == 4, "CPathNode / CNodeAddress");
static_assert(offsetof(CPathFind, m_pPathNodes) == PathFind62Fix::kOffPathNodes && offsetof(CPathFind, m_pNodeLinks) == PathFind62Fix::kOffNodeLinks
              && offsetof(CPathFind, m_pLinkLengths) == PathFind62Fix::kOffLinkLengths && offsetof(CPathFind, m_pPathIntersections) == PathFind62Fix::kOffIntersections, "CPathFind: punteros por area");
static_assert(offsetof(CPathFind, m_anNumNodes) == PathFind62Fix::kOffNumNodes && offsetof(CPathFind, m_anNumVehicleNodes) == PathFind62Fix::kOffNumVeh && offsetof(CPathFind, m_anNumPedNodes) == PathFind62Fix::kOffNumPed
              && offsetof(CPathFind, m_anNumCarPathLinks) == PathFind62Fix::kOffNumCarLinks && offsetof(CPathFind, m_anNumAddresses) == PathFind62Fix::kOffNumAddresses
              && offsetof(CPathFind, m_interiorIDs) == PathFind62Fix::kOffInteriorIds, "CPathFind: contadores por area");
static_assert(offsetof(CPathNode, m_vPos) == 8 && offsetof(CPathNode, m_totalDistFromOrigin) == 0xE && offsetof(CPathNode, m_wBaseLinkId) == 0x10 && offsetof(CPathNode, m_wAreaId) == 0x12
              && offsetof(CPathNode, m_wNodeId) == 0x14 && offsetof(CPathNode, m_nPathWidth) == 0x16 && offsetof(CPathNode, m_nFloodFill) == 0x17, "CPathNode: campos");
static_assert(NUM_PATH_MAP_AREAS == 64 && NUM_PATH_INTERIOR_AREAS == 8 && PathFind62Fix::kNumAreas == 72, "areas");
}

namespace PathFind62Live {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pathfind62-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pathfind62-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pathfind62");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores con hook: se resuelven ANTES de crear las rutas (que dejan su hook apagado) y con todos los hooks encendidos.
    const uintptr_t bodyClosest=DllBody(&CPathFind::FindNodeClosestToCoors);
    const uintptr_t bodyMalloc=DllBody(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc));
    const uintptr_t bodyAddDyn=DllBody(&CPathFind::AddDynamicLinkBetween2Nodes_For1Node);
    const uintptr_t bodyGround=DllBody(&CWorld::FindGroundZFor3DCoord);
    // Rutas RH: el constructor deja cada hook apagado.
    const struct {unsigned idx;const char* fn;uintptr_t entry;} routes[kFns]{
        {0,"FindNodeClosestToCoors",0x44F460},{1,"FindNthNodeClosestToCoors",0x44F8C0},{2,"FindNextNodeWandering",0x451B70},
        {3,"CompleteNewInterior",0x452270},{4,"FindParkingNodeInArea",0x4513F0},{5,"GeneratePedCreationCoors_Interior",0x44ECA0}};
    for(const auto& r:routes) {
        env.route[r.idx].reset(new HookRoute("CPathFind",r.fn,r.entry,HookRoute::RegisteredTarget{}));
        registered[kFn[r.idx]]=env.route[r.idx]->Registered();bodies[kFn[r.idx]]=env.route[r.idx]->Body()!=0;
        ready&=env.route[r.idx]->Registered()&&env.route[r.idx]->Body()&&ModuleOf(env.route[r.idx]->Body())==env.asi;
    }
    env.stubs[0]={{Pair{0x44F2C0,0,false},reinterpret_cast<void*>(&FakeRegion)}};      // 0x44F2C0 sin reconstruir: la DLL llama a la direccion del EXE
    env.stubs[1]={{Pair{0x44F460,bodyClosest,true},reinterpret_cast<void*>(&FakeFind)}};
    env.stubs[2]={{Pair{0x44F460,bodyClosest,true},reinterpret_cast<void*>(&FakeFind)}};
    env.stubs[3]={{Pair{0x72F420,bodyMalloc,true},reinterpret_cast<void*>(&FakeMalloc)},
                  {Pair{0x450FB0,0,false},reinterpret_cast<void*>(&FakeLink)},               // 0x450FB0 sin reconstruir
                  {Pair{0x44E000,bodyAddDyn,true},reinterpret_cast<void*>(&FakeAddDyn)}};
    env.stubs[5]={{Pair{0x821B1E,0,false},reinterpret_cast<void*>(&FakeRand)},                // rand() del CRT del EXE
                  {Pair{0x5696C0,bodyGround,true},reinterpret_cast<void*>(&FakeGround)}};
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            if(s.pair.exe!=0x44F460)env.watches.emplace_back(s.pair.exe,16);   // la entrada de 0x44F460 es la de una funcion bajo prueba: la vigilan las rutas
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    env.globals.emplace_back(0x96F050,kThisBytes);                      // ThePaths: ninguna funcion lo toca (this es una arena propia)
    env.globals.emplace_back(kGlobAAddr,kGlobABytes);
    env.globals.emplace_back(kGlobBAddr,kGlobBBytes);
    Run(env,ready,registered,bodies,collaborators);
}
}
