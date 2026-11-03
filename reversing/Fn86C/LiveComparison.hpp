#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include "PathFind.h"
#include "MemoryMgr.h"
#include "World.h"
#include "Camera.h"
#include "Entity/Ped/Ped.h"
#include "CutsceneMgr.h"
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par Fn86C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.
// Una funcion (hook S): CPathFind::GeneratePedCreationCoors 0x44E790 (thiscall, `ret 30h`, 12 argumentos; devuelve AL). El objeto `this` es una arena propia.
// Colaboradores sustituidos por falsos identicos en la direccion del EXE y en el cuerpo de la DLL: rand() del CRT del EXE 0x821B1E (sin hook: la DLL lo llama por su direccion),
// CWorld::FindGroundZFor3DCoord 0x5696C0 y CCamera::IsSphereVisible 0x420C40 (con matriz) / 0x420D40 (sin ella). FindRegionForCoors 0x44D830 y CPathNode::GetPosition 0x420A10 son codigo real (con hook en la DLL, ambos lados usan el de la DLL).

// Disposicion de los objetos reales frente a las constantes de Fixtures.hpp.
namespace Fn86CLayout {
static_assert(sizeof(CPathFind) == Fn86CFix::kThisBytes, "sizeof(CPathFind)");
static_assert(sizeof(CPathNode) == Fn86CFix::kNodeBytes && sizeof(CNodeAddress) == 4, "CPathNode / CNodeAddress");
static_assert(offsetof(CPathFind, m_pPathNodes) == Fn86CFix::kOffPathNodes && offsetof(CPathFind, m_pNodeLinks) == Fn86CFix::kOffNodeLinks
              && offsetof(CPathFind, m_pPathIntersections) == Fn86CFix::kOffIntersections, "CPathFind: punteros por area");
static_assert(offsetof(CPathFind, m_anNumNodes) == Fn86CFix::kOffNumNodes && offsetof(CPathFind, m_anNumVehicleNodes) == Fn86CFix::kOffNumVeh && offsetof(CPathFind, m_anNumPedNodes) == Fn86CFix::kOffNumPed, "CPathFind: contadores por area");
static_assert(offsetof(CPathNode, m_vPos) == 8 && offsetof(CPathNode, m_wBaseLinkId) == 0x10 && offsetof(CPathNode, m_wAreaId) == 0x12 && offsetof(CPathNode, m_wNodeId) == 0x14, "CPathNode: campos");
static_assert(NUM_PATH_MAP_AREAS == 64 && NUM_PATH_INTERIOR_AREAS == 8 && Fn86CFix::kNumAreas == 72, "areas");
}

namespace Fn86CLive {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fn86c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fn86c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fn86c");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();
    // Cuerpos de la DLL de los colaboradores con hook: se resuelven ANTES de crear las rutas (que dejan su hook apagado) y con todos los hooks encendidos.
    const uintptr_t bodyGround=DllBody(&CWorld::FindGroundZFor3DCoord);
    const uintptr_t bodyCamMatrix=DllBody(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible));
    const uintptr_t bodyCamMirror=DllBody(static_cast<bool(CCamera::*)(const CVector&,float)>(&CCamera::IsSphereVisible));
    // Ruta RH de la funcion probada: el constructor deja el hook apagado.
    env.route[0].reset(new HookRoute("CPathFind","GeneratePedCreationCoors",0x44E790,HookRoute::RegisteredTarget{}));
    registered[kFn[0]]=env.route[0]->Registered();bodies[kFn[0]]=env.route[0]->Body()!=0;
    ready&=env.route[0]->Registered()&&env.route[0]->Body()&&ModuleOf(env.route[0]->Body())==env.asi;
    env.stubs[0]={{Pair{0x821B1E,0,false},reinterpret_cast<void*>(&FakeRand)},                       // rand() del CRT del EXE
                  {Pair{0x5696C0,bodyGround,true},reinterpret_cast<void*>(&FakeGround)},
                  {Pair{0x420C40,bodyCamMatrix,true},reinterpret_cast<void*>(&FakeSphereMatrix)},
                  {Pair{0x420D40,bodyCamMirror,true},reinterpret_cast<void*>(&FakeSphereMirror)}};
    for(unsigned f=0;f<kFns;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array({"FindRegionForCoors 0x44D830 y CPathNode::GetPosition 0x420A10 (codigo real con hook en la DLL), aritmetica float, _ftol2"})},{"stubbed",stubbed}};
    }
    env.globals.emplace_back(0x96F050,kThisBytes);                      // ThePaths: la funcion no lo toca (this es una arena propia)
    Run(env,ready,registered,bodies,collaborators);
}
}
