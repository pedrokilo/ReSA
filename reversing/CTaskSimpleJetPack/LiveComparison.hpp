#pragma once
#include <fstream>
#include <optional>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimpleJetPack.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace JetLive {
using ReversingProbe::Bytes;
inline bool done{},realGeneration{};
inline uint32 randomState{};
inline int32 generated=-1;
inline json events=json::array();
inline std::ofstream log;
inline bool JumpsToExe(uintptr_t at){ if(*reinterpret_cast<const uint8*>(at)!=0xE9) return false; const auto t=at+5+*reinterpret_cast<const int32*>(at+1); return t>=0x400000&&t<0x1000000; }
inline ReversingProbe::EntryPatch *generatePatch{},*generateDllPatch{},*coordsPatch{},*coordsDllPatch{};
// Cuerpo C++ real de CPickups::GenerateNewOne_ExeAbi (resuelto antes de parchear) y contadores por lado: [0] EXE original (0x456F20), [1] cuerpo de la DLL.
inline uintptr_t generateDllBody{};
inline int generateCalls[2]{},generateReal[2]{};
// Diagnostico: orden de entrada a los falsos y anidamiento (el falso del EXE dentro del de la DLL, o al reves).
inline json genTrace=json::array();
inline bool inDllFake{},inExeFake{};
// Cuerpo real de una funcion de la DLL (salta los thunks ILT que apuntan dentro de la DLL).
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline int Report(int type,char* msg,int*) {
    if(type!=_CRT_ASSERT) return FALSE;
    Write({{"event","assertion"},{"message",msg ? msg : ""}}); ExitProcess(91);
}
inline int __cdecl Random() { randomState=randomState*214013u+2531011u; return int((randomState>>16)&32767); }
inline uint16 __cdecl RandomNumber() { return uint16(Random()); }
inline void __cdecl Coords(float x,float y,float z,float& ox,float& oy,float& oz) {
    // Se sueltan las dos entradas (EXE y DLL) para llamar al cuerpo real sin recursion.
    coordsPatch->Enable(false); coordsDllPatch->Enable(false);
    reinterpret_cast<void(__cdecl*)(float,float,float,float&,float&,float&)>(0x458A80)(x,y,z,ox,oy,oz);
    coordsDllPatch->Enable(true); coordsPatch->Enable(true); events.push_back({"coords",x,y,z,ox,oy,oz});
}
inline int32 __cdecl Generate(CVector pos,uint32 model,ePickupType type,uint32 ammo,uint32 money,bool empty,char* message) {
    // Lado original: el EXE llama a 0x456F20. El hook GenerateNewOne esta apagado durante todo el banco, asi que al soltar el parche queda el cuerpo real del EXE.
    int32 result=-1; ++generateCalls[0]; genTrace.push_back(inDllFake ? "exe(dentro de dll)" : "exe"); const bool prevExe=inExeFake; inExeFake=true;
    if(realGeneration) {
        generatePatch->Enable(false); ++generateReal[0];
        result=reinterpret_cast<int32(__cdecl*)(CVector,uint32,ePickupType,uint32,uint32,bool,char*)>(0x456F20)(pos,model,type,ammo,money,empty,message);
        generatePatch->Enable(true);
    }
    inExeFake=prevExe;
    generated=result; events.push_back({"generate",pos.x,pos.y,pos.z,model,int(type),ammo,money,empty,message==nullptr,result}); return result;
}
inline int32 __cdecl GenerateDll(CVector pos,uint32 model,ePickupType type,uint32 ammo,uint32 money,uint8 empty,char* message) {
    // Lado candidato: la envoltura CPickups::GenerateNewOne de la DLL llama a GenerateNewOne_ExeAbi (cuerpo C++ del hook 0x456F20); se parchea su cuerpo con el mismo falso.
    int32 result=-1; ++generateCalls[1]; genTrace.push_back(inExeFake ? "dll(dentro de exe)" : "dll"); const bool prevDll=inDllFake; inDllFake=true;
    if(realGeneration) {
        generateDllPatch->Enable(false); ++generateReal[1];
        result=reinterpret_cast<int32(__cdecl*)(CVector,uint32,ePickupType,uint32,uint32,uint8,char*)>(generateDllBody)(pos,model,type,ammo,money,empty,message);
        generateDllPatch->Enable(true);
    }
    inDllFake=prevDll;
    generated=result; events.push_back({"generate",pos.x,pos.y,pos.z,model,int(type),ammo,money,bool(empty),message==nullptr,result}); return result;
}
struct Jet : CTaskSimpleJetPack {
    bool abortResult;
    explicit Jet(int i) : abortResult((i&2)!=0) {
        auto* p=reinterpret_cast<uint8*>(this); p[8]=uint8(i&1); p[15]=0; p[13]=1; p[14]=uint8(i%3-1);
        float strafe=float(i%9-4)*.5f; std::memcpy(p+16,&strafe,4);
    }
    bool MakeAbortable(CPed*,eAbortPriority priority,const CEvent* event) override {
        events.push_back({"abort",int(priority),event==nullptr,abortResult});
        if(abortResult) reinterpret_cast<uint8*>(this)[8]=1;
        return abortResult;
    }
};
inline json PickupState() {
    json list=json::array();
    for(size_t i=0;i<CPickups::aPickUps.size();++i) {
        auto& p=CPickups::aPickUps[i];
        if(p.m_nPickupType==PICKUP_NONE) continue;
        auto position=p.GetPosn();
        list.push_back({i,p.m_nModelIndex,int(p.m_nPickupType),p.m_nAmmo,p.m_nRegenerationTime,p.m_nReferenceIndex,position.x,position.y,position.z,p.m_pObject!=nullptr});
    }
    return list;
}
inline void RestorePickups(const std::array<CPickup,MAX_NUM_PICKUPS>& saved) {
    for(size_t i=0;i<saved.size();++i) {
        auto& p=CPickups::aPickUps[i];
        if(p.m_pObject && p.m_pObject!=saved[i].m_pObject) p.GetRidOfObjects();
    }
    std::memcpy(CPickups::aPickUps.data(),saved.data(),sizeof(saved));
}
inline void Update() {
    if(done) return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"jetpack-runtime.start")) { done=true; return; }
    if(!log.is_open()) { log.open(dir/"jetpack-runtime.jsonl"); Write({{"event","waiting_for_player"}}); }
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning()) return;
    done=true; auto previousReport=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()=="CTaskSimpleJetPack") for(auto& h:c.Items()) if(h->Name()=="DropJetPack") hook=h.get();
    });
    ReversibleHooks::ReversibleHook::Base* genHook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c) {
        if(c.Name()=="CPickups") for(auto& h:c.Items()) if(h->Name()=="GenerateNewOne") genHook=h.get();
    });
    if(!hook || !genHook) { Write({{"event","error"},{"reason","missing hook"}}); _CrtSetReportHook(previousReport); return; }
    const auto savedPickups=CPickups::aPickUps; const auto savedPlayer=Bytes(player,sizeof(CPlayerPed));
    bool enabled=hook->Hooked(),locked=hook->Locked(); hook->LockState(false);
    // Hook GenerateNewOne (0x456F20 -> GenerateNewOne_ExeAbi): State(x) reescribe los 5 bytes de 0x456F20 Y los de la entrada de la DLL (apagado: 0x456F20 = EXE real y la entrada de ExeAbi salta a 0x456F20;
    // encendido: 0x456F20 salta a la DLL y la entrada de ExeAbi es su cuerpo real). Por eso: el cuerpo se resuelve con el hook ENCENDIDO y antes de crear parches, cada EntryPatch se construye con el hook
    // en el estado de su lado (guarda bytes reales), y por par se cambia el estado del hook y DESPUES se activa el parche de ese lado.
    const bool genEnabled=genHook->Hooked(),genLocked=genHook->Locked(); genHook->LockState(false); genHook->State(true);
    generateDllBody=Body(std::bit_cast<uintptr_t>(&CPickups::GenerateNewOne_ExeAbi));
    Write({{"event","gen_diagnostic"},{"exe_entry",Bytes(reinterpret_cast<void*>(0x456F20),8)},{"dll_body",generateDllBody},{"dll_body_bytes",Bytes(reinterpret_cast<void*>(generateDllBody),8)},{"dll_symbol_bytes",Bytes(reinterpret_cast<void*>(std::bit_cast<uintptr_t>(&CPickups::GenerateNewOne_ExeAbi)),8)},{"gen_hook_on",genHook->Hooked()}});
    int exeReal=0,dllReal=0; bool gateAll=generateDllBody>=0x1000000 && !JumpsToExe(generateDllBody) && ReversingProbe::JumpsToDll(0x456F20);
    bool passed=true; int pairs=0,created=0;
    {
        // CreatePickupCoorsCloseToCoors y CGeneral::GetRandomNumber estan reconstruidas y la DLL las llama directamente (la conversion de DropJetPack
        // ya no pasa por 0x458A80): mismas sustituciones en la entrada de la DLL que en la del EXE; el RNG de la DLL usa rand() de su propia CRT.
        ReversingProbe::EntryPatch coords(0x458A80,Coords),random(0x821B1E,Random);
        ReversingProbe::EntryPatch coordsDll(Body(std::bit_cast<uintptr_t>(static_cast<void(*)(float,float,float,float&,float&,float&)>(&CPickups::CreatePickupCoorsCloseToCoors))),Coords);
        ReversingProbe::EntryPatch randomDll(Body(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),RandomNumber);
        // genDll: hook encendido (bytes reales de la entrada de la DLL). gen: hook apagado (bytes reales del EXE en 0x456F20). Ambos quedan desactivados hasta su lado.
        std::optional<ReversingProbe::EntryPatch> genDll,gen;
        genHook->State(true); genDll.emplace(generateDllBody,GenerateDll); genDll->Enable(false);
        genHook->State(false); gen.emplace(0x456F20,Generate); gen->Enable(false);
        generatePatch=&*gen; generateDllPatch=&*genDll; coordsPatch=&coords; coordsDllPatch=&coordsDll;
        auto entry=reinterpret_cast<void(__thiscall*)(CTaskSimpleJetPack*,CPed*)>(0x67B660);
        for(int i=0;i<128 && passed;++i) {
            realGeneration=i<64;
            alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> a{},b{};
            std::memcpy(a.data(),player,a.size()); auto* pa=reinterpret_cast<CPlayerPed*>(a.data());
            CMatrixLink matrix=*player->m_matrix; pa->m_matrix=&matrix;
            if(i&4) { pa->m_matrix=nullptr; pa->SetPosn(player->GetPosition()); }
            b=a; auto* pb=reinterpret_cast<CPlayerPed*>(b.data()); CMatrixLink matrixB=matrix;
            if(pb->m_matrix) pb->m_matrix=&matrixB;
            const auto beforeA=a,beforeB=b;
            Jet ja(i),jb(i); const auto start=JetProbe::calls;
            genHook->State(false); const bool gateO=!ReversingProbe::JumpsToDll(0x456F20); gen->Enable(true); hook->State(false); randomState=uint32(i+1); events=json::array(); generated=-1; genTrace=json::array(); inDllFake=inExeFake=false; generateCalls[0]=generateCalls[1]=generateReal[0]=generateReal[1]=0; entry(&ja,pa);
            const auto original=json{{"task",Bytes(reinterpret_cast<uint8*>(&ja)+8,0x68)},{"events",events},{"pickups",PickupState()},{"rng",randomState}};
            gen->Enable(false);
            const auto middle=JetProbe::calls; if(generated!=-1) ++created; const json origTrace=genTrace; const int origCalls[2]{generateCalls[0],generateCalls[1]},origReal[2]{generateReal[0],generateReal[1]};
            RestorePickups(savedPickups);
            genHook->State(true); const bool gateC=generateDllBody>=0x1000000 && !JumpsToExe(generateDllBody) && ReversingProbe::JumpsToDll(0x456F20); genDll->Enable(true); hook->State(true); randomState=uint32(i+1); events=json::array(); generated=-1; genTrace=json::array(); inDllFake=inExeFake=false; generateCalls[0]=generateCalls[1]=generateReal[0]=generateReal[1]=0; entry(&jb,pb);
            const auto candidate=json{{"task",Bytes(reinterpret_cast<uint8*>(&jb)+8,0x68)},{"events",events},{"pickups",PickupState()},{"rng",randomState}};
            genDll->Enable(false);
            const auto end=JetProbe::calls; if(generated!=-1) ++dllReal; exeReal+=origReal[0]; RestorePickups(savedPickups);
            // Rutas: el original llama una vez al falso del EXE y ninguna al de la DLL; el candidato al reves. La generacion real ejecuta el cuerpo propio de cada lado.
            const int nGen=origCalls[0]; /* 0 si la tarea aborta antes de generar, 1 si genera */ const bool routes=(nGen==0||nGen==1) && origCalls[1]==0 && origReal[0]==(realGeneration?nGen:0) && origReal[1]==0 && generateCalls[0]==0 && generateCalls[1]==nGen && generateReal[0]==0 && generateReal[1]==(realGeneration?nGen:0);
            const bool unchanged=a==beforeA && b==beforeB;
            const bool gates=gateO && gateC && gateAll; passed=gates && routes && original==candidate && unchanged && middle==start && end==middle+1;
            Write({{"event","comparison"},{"pair",pairs++},{"real_generation",realGeneration},{"original",original},{"candidate",candidate},{"equal",passed},{"inputs_unchanged",unchanged},{"original_candidate_calls",middle-start},{"routes_ok",routes},{"gen_route_ok",gates},{"orig_trace",origTrace},{"cand_trace",genTrace},{"orig_generate_calls",{origCalls[0],origCalls[1]}},{"cand_generate_calls",{generateCalls[0],generateCalls[1]}},{"orig_real_exe",origReal[0]},{"cand_real_body",generateReal[1]},{"candidate_calls",end-middle}});
        }
        // Cierre: con el hook apagado se libera gen (bytes EXE); con el encendido, genDll (bytes reales de la DLL).
        genHook->State(false); gen.reset(); genHook->State(true); genDll.reset();
    }
    genHook->State(genEnabled); genHook->LockState(genLocked);
    hook->State(enabled); hook->LockState(locked);
    const bool restored=Bytes(player,sizeof(CPlayerPed))==savedPlayer && std::memcmp(CPickups::aPickUps.data(),savedPickups.data(),sizeof(savedPickups))==0 && hook->Hooked()==enabled && hook->Locked()==locked && genHook->Hooked()==genEnabled && genHook->Locked()==genLocked;
    Write({{"event",passed && restored ? "complete":"error"},{"pairs",pairs},{"created",created},{"created_candidate",dllReal},{"real_exe_calls",exeReal},{"state_restored",restored}});
    _CrtSetReportHook(previousReport);
}
}
