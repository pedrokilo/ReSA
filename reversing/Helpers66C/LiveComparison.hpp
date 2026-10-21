#pragma once
#include <bit>
#include <cstddef>
#include <fstream>
#include <memory>
#include <winnt.h>
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Cases.hpp"

// Banco par a par Helpers66C dentro del juego: Update() se llama desde CGame::Process (probe_build.py). Todo el nucleo esta en Core.hpp / Cases.hpp.

namespace Hp66Live {
template<class F> inline uintptr_t DllBody(F fn) {return Body(std::bit_cast<uintptr_t>(fn));}

// Ranuras de importacion de la DLL (ASI) cuyo valor coincide con `target` (la API real): se buscan en el directorio IAT del PE.
inline std::vector<uintptr_t> FindImportSlots(uintptr_t module,uintptr_t target) {
    std::vector<uintptr_t> slots;
    const auto* dos=reinterpret_cast<const IMAGE_DOS_HEADER*>(module);
    const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS32*>(module+dos->e_lfanew);
    const auto& iat=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IAT];
    for(uintptr_t at=module+iat.VirtualAddress;at<module+iat.VirtualAddress+iat.Size;at+=4)
        if(*reinterpret_cast<const uintptr_t*>(at)==target)slots.push_back(at);
    return slots;
}
inline void Update() {
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers66c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers66c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers66c");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    Env env;
    env.asi=ModuleOf(uintptr_t(&ModuleMarker));env.game=uintptr_t(GetModuleHandleA(nullptr));
    bool ready=true;json registered=json::object();json bodies=json::object();json collaborators=json::object();json slotInfo=json::object();
    auto simple=[&](unsigned idx,const char* cls,const char* fn,uintptr_t entry) {
        env.route[idx].reset(new HookRoute(cls,fn,entry,HookRoute::RegisteredTarget{}));
        registered[kFn[idx]]=env.route[idx]->Registered();bodies[kFn[idx]]=env.route[idx]->Body()!=0;
        ready&=env.route[idx]->Registered()&&env.route[idx]->Body()&&ModuleOf(env.route[idx]->Body())==env.asi;
    };
    simple(0,"CAEMFDecoder","OpenSoundMedia",0x4E7990);
    simple(1,"CAEMFDecoder","GetMediaSoundFormat",0x4E7A80);
    simple(2,"Ps","GetDXVersion",0x745650);
    simple(3,"RwHelper","RwTextureGtaStreamRead",0x730E60);
    simple(4,"RwHelper","SkinGetBonePositionsToTable",0x735360);
    // Colaboradores sustituidos: cuerpos de la DLL resueltos ANTES de escribir ningun parche (siguiendo los saltos solo dentro del ASI).
    // 0 y 1: las funciones de pegamento QuickTime del EXE (0x818xxx-0x81Dxxx): sin hook ni cuerpo propio, la DLL las llama por la direccion del EXE.
    // 3: RwStreamFindChunk (0x7ED2D0, envoltura sin hook), CTimer::GetCurrentTimeInCycles / GetCyclesPerMillisecond y RpAnisot* (con hook: se parchean la entrada del EXE y el cuerpo de la DLL).
    // 4: RpClumpForAllAtomics, RpSkinGeometryGetSkin, RpSkinGetNumBones, RpSkinGetSkinToBoneMatrices, RwMatrixInvert y RwV3dTransformPoints (envolturas sin hook).
    env.stubs[0]={{Pair{0x818970,0,false},reinterpret_cast<void*>(&F0NativePath)},{Pair{0x81DAA0,0,false},reinterpret_cast<void*>(&F0FInfo)},{Pair{0x81B680,0,false},reinterpret_cast<void*>(&F0Open)},
                  {Pair{0x81AB80,0,false},reinterpret_cast<void*>(&F0NewMovie)},{Pair{0x81A9E0,0,false},reinterpret_cast<void*>(&F0Close)},{Pair{0x81BC60,0,false},reinterpret_cast<void*>(&F0TrackType)},
                  {Pair{0x81A0E0,0,false},reinterpret_cast<void*>(&F0TrackMedia)}};
    env.stubs[1]={{Pair{0x818DB0,0,false},reinterpret_cast<void*>(&F1NewHandle)},{Pair{0x81A300,0,false},reinterpret_cast<void*>(&F1SampleDesc)},{Pair{0x819860,0,false},reinterpret_cast<void*>(&F1MoviesError)},
                  {Pair{0x81C600,0,false},reinterpret_cast<void*>(&F1GetExtension)},{Pair{0x8192B0,0,false},reinterpret_cast<void*>(&F1HandleSize)},{Pair{0x818F90,0,false},reinterpret_cast<void*>(&F1HLock)},
                  {Pair{0x818E70,0,false},reinterpret_cast<void*>(&F1NewPtr)},{Pair{0x818D70,0,false},reinterpret_cast<void*>(&F1MemError)},{Pair{0x819390,0,false},reinterpret_cast<void*>(&F1BlockMove)},
                  {Pair{0x818FB0,0,false},reinterpret_cast<void*>(&F1HUnlock)},{Pair{0x819270,0,false},reinterpret_cast<void*>(&F1Dispose)}};
    env.stubs[3]={{Pair{0x7ED2D0,0,false},reinterpret_cast<void*>(&F3FindChunk)},
                  {Pair{0x561A80,DllBody(static_cast<uint32(*)()>(&CTimer::GetCurrentTimeInCycles)),true},reinterpret_cast<void*>(&F3Time)},
                  {Pair{0x561A40,DllBody(static_cast<uint32(*)()>(&CTimer::GetCyclesPerMillisecond)),true},reinterpret_cast<void*>(&F3Divider)},
                  {Pair{0x748F50,DllBody(&RpAnisotTextureGetMaxAnisotropy),true},reinterpret_cast<void*>(&F3AnisoGet)},
                  {Pair{0x748F20,DllBody(&RpAnisotGetMaxSupportedMaxAnisotropy),true},reinterpret_cast<void*>(&F3AnisoMax)},
                  {Pair{0x748F30,DllBody(&RpAnisotTextureSetMaxAnisotropy),true},reinterpret_cast<void*>(&F3AnisoSet)}};
    env.stubs[4]={{Pair{0x749B70,0,false},reinterpret_cast<void*>(&F4ForAll)},{Pair{0x7C7550,0,false},reinterpret_cast<void*>(&F4GetSkin)},{Pair{0x7C77E0,0,false},reinterpret_cast<void*>(&F4NumBones)},
                  {Pair{0x7C7810,0,false},reinterpret_cast<void*>(&F4Matrices)},{Pair{0x7F2070,0,false},reinterpret_cast<void*>(&F4Invert)},{Pair{0x7EDD90,0,false},reinterpret_cast<void*>(&F4Transform)}};
    // 2: las cuatro APIs de Windows en las ranuras de importacion del EXE (0x858070 LoadLibraryA, 0x85806C GetProcAddress, 0x858110 FreeLibrary, 0x858118 OutputDebugStringA) y en las de la DLL
    // (las que valen lo mismo que la ranura del EXE o la API de kernel32). Se exige al menos una ranura por API en la DLL.
    {
        struct Api {const char* name;uintptr_t exeSlot;void* fake;uintptr_t* real;};
        const Api apis[]{{"LoadLibraryA",0x858070,reinterpret_cast<void*>(&F2LoadLibrary),&g.realLoad},{"GetProcAddress",0x85806C,reinterpret_cast<void*>(&F2GetProcAddress),&g.realProc},
                         {"FreeLibrary",0x858110,reinterpret_cast<void*>(&F2FreeLibrary),&g.realFree},{"OutputDebugStringA",0x858118,reinterpret_cast<void*>(&F2Debug),&g.realDebug}};
        const HMODULE k32=GetModuleHandleA("kernel32.dll");
        for(const auto& a:apis) {
            const uintptr_t exeValue=*reinterpret_cast<const uintptr_t*>(a.exeSlot);
            const uintptr_t apiValue=uintptr_t(GetProcAddress(k32,a.name));
            *a.real=exeValue;
            std::vector<uintptr_t> dllSlots=FindImportSlots(env.asi,exeValue);
            if(apiValue!=exeValue)for(auto s:FindImportSlots(env.asi,apiValue))dllSlots.push_back(s);
            env.slots[2].push_back({a.exeSlot,a.fake});
            for(auto s:dllSlots)env.slots[2].push_back({s,a.fake});
            ready&=!dllSlots.empty()&&exeValue!=0;
            slotInfo[a.name]={{"exe_slot",a.exeSlot},{"exe_value",exeValue},{"api_value",apiValue},{"dll_slots",dllSlots.size()}};
        }
    }
    for(unsigned f=0;f<kFunctions;++f) {
        json stubbed=json::array();
        for(const auto& s:env.stubs[f]) {
            env.watches.emplace_back(s.pair.exe,16);
            stubbed.push_back({{"exe",s.pair.exe},{"dll",s.pair.dll},{"hooked",s.pair.hooked}});
        }
        collaborators[kFn[f]]={{"dll_real",json::array()},{"stubbed",stubbed}};
    }
    SaveRegions();
    Run(env,ready,registered,bodies,collaborators,slotInfo);
}
}
