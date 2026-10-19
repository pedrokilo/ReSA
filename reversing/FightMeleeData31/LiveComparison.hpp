#pragma once
#include <bit>
#include <fstream>
#include <memory>
#include <cstdio>
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "Animation/AnimManager.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Core.hpp"

// Banco par a par FightMeleeData31 dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp.
// Una ruta: CTaskSimpleFight::LoadMeleeData (0x5BEDC0), hook Simple (RH_ScopedInstall): HookRoute::RegisteredTarget toma el cuerpo de la DLL del propio hook registrado.
static_assert(sizeof(CMeleeInfo) == 0x88, "CMeleeInfo mide 0x88 (la tabla de combos es de filas de 0x88)");
static_assert(sizeof(CTaskSimpleFight::m_aComboData) == 12 * 0x88, "12 filas en la tabla de combos (0xC170D0)");
static_assert(FightMeleeData31Fix::kTableAt + 12 * 0x88 == 0xC17730u, "fin de la tabla de 12 filas (12 * 0x88 = 0x660)");
static_assert(FightMeleeData31Fix::kRegionBase + FightMeleeData31Fix::kRegionOff == FightMeleeData31Fix::kTableAt, "la region empieza 0x40 antes de la tabla");
static_assert(FightMeleeData31Fix::kRegionOff + FightMeleeData31Fix::kRegionBytes + 0x40 == FightMeleeData31Fix::kRegion, "guarda + ventana + guarda");
static_assert(FightMeleeData31Fix::kRegionBase + FightMeleeData31Fix::kRegion == 0xC17940u, "fin de la region");
namespace FightMeleeData31Live {
static void Dbg(const char* m){std::ofstream f("E:/JUEGOS/GTA San Andreas - CVehicle Test/fm31-debug.txt",std::ios::app);f<<m<<std::endl;}
inline void Update() {
    static int dbgCalls=0;if(dbgCalls<3||dbgCalls%600==0){Dbg(dbgCalls==0?"1 Update llamada":"1b Update sigue llamandose");}++dbgCalls;
    if(done){if(dbgCalls<5)Dbg("2 done=true: salgo");return;}char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fightmeleedata31-runtime.start")){Dbg("3 no existe .start: done=true");done=true;return;}Dbg("4 existe .start");
    if(!log.is_open()){Dbg("5 abro registro");log.open(dir/"fightmeleedata31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fightmeleedata31");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    // Ruta RH: se construye antes que nada; el constructor deja el hook apagado.
    bool ready=true;json registered=json::object();json bodies=json::object();
    env.route.reset(new HookRoute("CTaskSimpleFight","LoadMeleeData",0x5BEDC0,HookRoute::RegisteredTarget{}));
    registered["CTaskSimpleFight::LoadMeleeData"]=env.route->Registered();bodies["CTaskSimpleFight::LoadMeleeData"]=env.route->Body();
    ready&=env.route->Registered()&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    // Cuerpos de la DLL de los colaboradores: se resuelven ANTES de escribir ningun parche (PROCESO.md, trampas). Con sus hooks encendidos (estado normal del ASI) la entrada del EXE
    // salta a la DLL; Body() sigue el salto.
    env.pOpen={0x538900,Body(std::bit_cast<uintptr_t>(&CFileMgr::OpenFile))};
    env.pRead={0x536F80,Body(std::bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)))};
    env.pClose={0x5389D0,Body(std::bit_cast<uintptr_t>(&CFileMgr::CloseFile))};
    env.pName={0x4D3A20,Body(std::bit_cast<uintptr_t>(&CAnimManager::GetAnimGroupName))};
    Run(env,ready,registered,bodies);
}
}
