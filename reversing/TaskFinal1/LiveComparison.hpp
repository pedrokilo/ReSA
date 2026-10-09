#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1, partida cargada via FastLoader): constructores y destructores de las
// cinco clases de tarea que cierran la lista final:
//   CTaskUtilityLineUpPedWithCar (0x64FBB0 / 0x64FC00, no es CTask: sin vptr), CTaskGoToVehicleAndLean (0x660E60 / 0x660EE0),
//   CTaskGangHassleVehicle (0x65FAC0 / 0x65FB60), CTaskGangHasslePed (0x65FED0 / 0x65FF60) y
//   CTaskComplexPlayHandSignalAnim (0x61B2B0 / 0x61BDF0).
// Original (hooks del banco desactivados) frente a candidato con fixtures identicos y deterministas:
//  - Constructores: se llaman en la direccion del EXE sobre un buffer rellenado con 0xCD (tambien los bytes sobrantes tras
//    el objeto, mas guardas de 16 bytes) y se compara el buffer COMPLETO (sin la vptr; en su lugar se compara el tipo de
//    tarea, GetTaskType, con los hooks apagados), el retorno (this), las referencias registradas en el peaton / vehiculo
//    falsos y el estado de esas entidades.
//  - Destructores: objeto con campos aleatorios, referencias registradas a veces, subtarea falsa con vtable trazada,
//    contadores de bloques de animacion (0xB5D4A0, 32 bytes por bloque), de los cuatro modelos de manos (CBaseModelInfo
//    +8) y ms_animBlock (0x8D2E38) restaurados antes de cada ejecucion.
// Colaboradores simulados y trazados: CBaseModelInfo::RemoveRef (0x4C4BB0; decrementa el contador de +8 y anota el modelo,
// el contador del TXD no se toca) y CStreaming::RemoveModel (0x4089A0). RemoveAnimBlockRef, GetAnimationBlockIndex,
// SetHasslePosId y las referencias (RegisterReference / CleanUpOldReference) se ejecutan de verdad.
// El pool de tareas no se usa (los objetos viven en un buffer propio): se comprueba que su ocupacion no cambia y se
// restaura m_LastFreeSlot y el relleno 0xCD de los huecos libres por si algun dia se reservara una tarea.
// Nada del estado global tocado se deja modificado: se guarda y se restaura por ejecucion y al final.
#include <fstream>
#include <unordered_map>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskUtilityLineUpPedWithCar.h"
#include "TaskGoToVehicleAndLean.h"
#include "TaskGangHassleVehicle.h"
#include "TaskGangHasslePed.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "Entity/Vehicle/Automobile.h"
#include "Models/BaseModelInfo.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace TaskFinal1Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const std::string& s,uint64 h=1469598103934665603ull){ for(unsigned char c:s){h^=c;h*=1099511628211ull;} return h; }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
// Hilo del banco: otros hilos del juego (audio...) no deben tocar la traza ni los colaboradores simulados.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

constexpr size_t OBJ=0x80;                         // buffer del objeto probado (la mayor clase mide 0x44)
constexpr uintptr_t ADDR_BLOCKS=0xB5D4A0;          // CAnimManager::ms_aAnimBlocks (32 bytes por bloque)
constexpr uintptr_t ADDR_ANIMBLOCK=0x8D2E38;       // CTaskComplexPlayHandSignalAnim::ms_animBlock
constexpr uintptr_t ADDR_CLOCK=0xB7CB84;           // CTimer::m_snTimeInMilliseconds
constexpr int FIRST_HAND_MODEL=394;                // SHANDL, SHANDR, FHANDL, FHANDR (0xA9B6F0 + 4 * k = ms_modelInfoPtrs[394 + k])
static_assert(sizeof(CAnimBlock)==0x20);
static_assert(sizeof(CTaskUtilityLineUpPedWithCar)==0x1C && sizeof(CTaskGoToVehicleAndLean)==0x24 && sizeof(CTaskGangHassleVehicle)==0x44 &&
              sizeof(CTaskGangHasslePed)==0x2C && sizeof(CTaskComplexPlayHandSignalAnim)==0x18);

alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> veh{};
alignas(16) inline std::array<uint8,16+OBJ+16> objBuf{};              // guarda + objeto + guarda
alignas(16) inline std::array<std::array<uint8,0x40>,2> fakes{};      // 0 = subtarea falsa, 1 = reserva
inline CPed* Ped() { return reinterpret_cast<CPed*>(ped.data()); }
inline CAutomobile* Veh() { return reinterpret_cast<CAutomobile*>(veh.data()); }
inline uint8* Obj() { return objBuf.data()+16; }
inline CTask* Fake(int n) { return reinterpret_cast<CTask*>(fakes[n].data()); }
inline uint8* Blocks() { return reinterpret_cast<uint8*>(ADDR_BLOCKS); }
inline size_t BlocksBytes() { return size_t(StaticRef<uint32>(0xB4EA30))*sizeof(CAnimBlock); }
inline uint16& BlockRefs(uint32 idx) { return *reinterpret_cast<uint16*>(Blocks()+idx*sizeof(CAnimBlock)+0x12); }
inline CBaseModelInfo* Hand(int k) { return CModelInfo::GetModelInfo(FIRST_HAND_MODEL+k); }
inline uint16& HandRefs(int k) { return *reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(Hand(k))+8); }
inline int HandIndex(const void* m) { for(int k=0;k<4;++k)if(Hand(k)==m)return k; return -1; }
inline uint32& AnimBlockStatic() { return *reinterpret_cast<uint32*>(ADDR_ANIMBLOCK); }
inline uint32& Clock() { return *reinterpret_cast<uint32*>(ADDR_CLOCK); }

inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(ped.data(),ped.size(),"ped"))return *s; if(auto s=in(veh.data(),veh.size(),"veh"))return *s;
    if(auto s=in(Obj(),OBJ,"obj"))return *s;
    for(int n=0;n<2;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    const auto refs=uintptr_t(&CReferences::aRefs[0]); if(v>=refs&&v<refs+sizeof(CReferences::aRefs))return "ref";
    return v;
}
inline json Region(const void* p,size_t n,bool skipVptr=false) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=skipVptr ? 1:0;k<n/4;++k)r.push_back(Name(w[k])); return r; }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
// el nodo de referencias sale de la lista libre global (su direccion no es comparable): se enmascara
template<size_t N> std::string EntHash(const std::array<uint8,N>& buf) {
    auto copy=buf; const auto off=uintptr_t(&reinterpret_cast<const CEntity*>(buf.data())->m_pReferences)-uintptr_t(buf.data());
    std::memset(copy.data()+off,0,4); return Hash(copy.data(),copy.size());
}

// subtarea falsa: solo importa su destructor (hueco 0 de la vtable); el resto de huecos anotan un acceso inesperado
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { if(Mine())trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
template<int N> void* __fastcall FTrap(CTask*,void*) { if(Mine())trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// colaboradores simulados
inline void __fastcall RemoveRefStub(CBaseModelInfo* self,void*) {   // 0x4C4BB0: --m_nRefCount (+8) y RemoveRef del TXD (omitido)
    auto& rc=*reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(self)+8); rc=uint16(rc-1);
    if(Mine())trace.push_back({"remove_ref",HandIndex(self),rc});
}
inline void __cdecl RemoveModelStub(int32 id) { if(Mine())trace.push_back({"remove_model",id}); }   // 0x4089A0

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j)a+=5+*reinterpret_cast<int32*>(a+1); return a; }

enum Kind { LIN, GTL, GHV, GHP, PHS };
inline Kind KindOf(const std::string& c) {
    return c=="CTaskUtilityLineUpPedWithCar" ? LIN : c=="CTaskGoToVehicleAndLean" ? GTL : c=="CTaskGangHassleVehicle" ? GHV : c=="CTaskGangHasslePed" ? GHP : PHS;
}
constexpr uintptr_t VT[]{0,0x86FAC8,0x86F9D4,0x86FA00,0x86D5DC};
constexpr size_t SZ[]{0x1C,0x24,0x44,0x2C,0x18};

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=10;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskUtilityLineUpPedWithCar","Constructor",0x64FBB0,0,-1,512},{"CTaskUtilityLineUpPedWithCar","Destructor",0x64FC00,0,-1,128},
    {"CTaskGoToVehicleAndLean","Constructor",0x660E60,0,-1,256},{"CTaskGoToVehicleAndLean","Destructor",0x660EE0,0,-1,256},
    {"CTaskGangHassleVehicle","Constructor",0x65FAC0,0,-1,256},{"CTaskGangHassleVehicle","Destructor",0x65FB60,0,-1,512},
    {"CTaskGangHasslePed","Constructor",0x65FED0,0,-1,256},{"CTaskGangHasslePed","Destructor",0x65FF60,0,-1,256},
    {"CTaskComplexPlayHandSignalAnim","Constructor",0x61B2B0,0,-1,256},{"CTaskComplexPlayHandSignalAnim","Destructor",0x61BDF0,0,-1,512},
}};

inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(Ped()); clean(Veh());
}
inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"taskfinal1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"taskfinal1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("taskfinal1");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"taskfinal1-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    for(int k=0;k<4;++k)if(!Hand(k)){Write({{"event","error"},{"reason","hand model info missing"},{"model",FIRST_HAND_MODEL+k}});return;}
    const int32 gangs=CAnimManager::GetAnimationBlockIndex("gangs"); const uint32 nBlocks=StaticRef<uint32>(0xB4EA30);
    if(gangs<0||uint32(gangs)>=nBlocks||nBlocks==0){Write({{"event","error"},{"reason","gangs anim block missing"},{"index",gangs},{"blocks",nBlocks}});return;}
    FillTraps(std::make_integer_sequence<int,12>{}); fakeVtbl[0]=reinterpret_cast<void*>(&FDtor);

    // Estado global que se toca: instantanea para restaurar tras cada ejecucion y al final.
    const std::vector<uint8> savedBlocks=ReversingProbe::Bytes(Blocks(),BlocksBytes());
    std::array<uint16,4> savedHands{}; for(int k=0;k<4;++k)savedHands[k]=HandRefs(k);
    const uint32 savedAnimBlock=AnimBlockStatic(), savedClock=Clock();
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // Direcciones de la DLL de los colaboradores simulados
    const uintptr_t dllRemoveRef=std::bit_cast<uintptr_t>(&CBaseModelInfo::RemoveRef), dllRemoveModel=std::bit_cast<uintptr_t>(&CStreaming::RemoveModel);
    struct Site { const char* name; uintptr_t exeAddr, dll, dllBody; uintptr_t stub; };
    const std::array<Site,2> sites{{
        {"CBaseModelInfo::RemoveRef",0x4C4BB0,dllRemoveRef,Body(dllRemoveRef),reinterpret_cast<uintptr_t>(&RemoveRefStub)},
        {"CStreaming::RemoveModel",0x4089A0,dllRemoveModel,Body(dllRemoveModel),reinterpret_cast<uintptr_t>(&RemoveModelStub)}}};
    bool known=true; json siteInfo=json::array();
    for(auto& s:sites){ const bool ok=s.exeAddr>=0x401000&&s.exeAddr<0x1000000&&s.dll>=0x1000000&&s.dllBody>=0x1000000; known&=ok;
        siteInfo.push_back({{"name",s.name},{"exe",s.exeAddr},{"dll",s.dll},{"body",s.dllBody},{"ok",ok}}); }
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entryBefore;
    for(auto& s:sites)for(auto a:{s.exeAddr,s.dll,s.dllBody})entryBefore.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),8)});
    for(auto& fn:fns)entryBefore.push_back({fn.address,ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address),8)});
    const unsigned savedFP=_controlfp(0,0);
    Write({{"event","begin"},{"sites",siteInfo},{"known",known},{"fp_control",savedFP},{"gangs_block",gangs},{"anim_blocks",nBlocks}});

    bool passed=known; int pair=0;
    if(known) {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        for(auto& s:sites){ patches.push_back(std::make_unique<EntryPatch>(s.exeAddr,reinterpret_cast<void*>(s.stub))); patches.push_back(std::make_unique<EntryPatch>(s.dll,reinterpret_cast<void*>(s.stub)));
            if(s.dllBody!=s.dll)patches.push_back(std::make_unique<EntryPatch>(s.dllBody,reinterpret_cast<void*>(s.stub))); }
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm; const Kind kind=KindOf(fn.cls);
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                // flotantes que el EXE copia sin interpretar (nunca sNaN: el x87 los silencia)
                auto fl=[&]()->float{ static const uint32 sp[]{0,0x80000000u,0x7F800000u,0xFF800000u,0x7FC00000u,0x3F800000u,0xC479FF5Cu,0x00000001u,0x7F7FFFFFu};
                    return next()%3==0 ? std::bit_cast<float>(sp[next()%9]) : uni(-1000.f,1000.f); };
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura; huecos libres con 0xCD (como los otros bancos de tareas)
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sl=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sl[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F1Au; if(!st)st=1; trace=json::array();
                    CleanRefs();
                    std::memcpy(Blocks(),savedBlocks.data(),savedBlocks.size());
                    for(int k=0;k<4;++k)HandRefs(k)=savedHands[k];
                    AnimBlockStatic()=savedAnimBlock; Clock()=savedClock;
                    // entidades falsas
                    std::memset(ped.data(),0,ped.size()); Ped()->SetType(ENTITY_TYPE_PED);
                    std::memset(veh.data(),0,veh.size()); Veh()->SetType(ENTITY_TYPE_VEHICLE); veh[0x51B]=uint8(next());   // 0x51B = bits de posiciones de acoso
                    // subtarea falsa
                    for(auto& fk:fakes){ std::memset(fk.data(),0,fk.size()); *reinterpret_cast<void**>(fk.data())=fakeVtbl.data(); }
                    // objeto probado
                    objBuf.fill(0xCD); uint8* o=Obj(); auto W=[&](size_t off)->uint32&{return *reinterpret_cast<uint32*>(o+off);};
                    const bool isCtor=nm=="Constructor";
                    uint32 clk=savedClock;
                    // argumentos de los constructores
                    CEntity* entArg=nullptr; int32 ai[3]{}; uint32 rawByte=0; float af[2]{}; float vec[3]{};
                    if(isCtor){
                        switch(kind){
                        case LIN: { static const uint32 ck[]{0u,1u,0x7FFFFFFFu,0x80000000u,0xFFFFFFFFu,0xFFFFFF00u,0x1234ABCDu};
                            clk=next()%3==0 ? ck[next()%7] : next(); Clock()=clk;
                            for(auto& c:vec)c=fl();
                            { static const int32 tm[]{0,1,-1,600,0x7FFFFFFF,int32(0x80000000u),-600}; ai[0]=next()%2 ? tm[next()%7] : int32(next()); }
                            ai[1]=next()%4 ? int32(next()%8) : int32(next()); ai[2]=next()%4 ? int32(next()%24) : int32(next());
                            break; }
                        case GTL: entArg=next()%5 ? static_cast<CEntity*>(Veh()) : nullptr; ai[0]=next()%2 ? int32(next()%100000) : int32(next()); break;
                        case GHV: entArg=next()%5 ? static_cast<CEntity*>(Veh()) : nullptr; ai[0]=int32(next()); rawByte=next(); af[0]=fl(); af[1]=fl(); break;
                        case GHP: entArg=next()%5 ? static_cast<CEntity*>(Ped()) : nullptr; for(auto& v:ai)v=next()%2 ? int32(next()%100) : int32(next()); break;
                        case PHS: { static const int32 ids[]{-1,0,1,0x140,0x141,0x142,0x144,0x7FFFFFFF,int32(0x80000000u)}; ai[0]=next()%2 ? ids[next()%9] : int32(next()%1000); af[0]=fl(); break; }
                        }
                    } else {
                        // destructores: campos aleatorios; vptr del EXE; punteros y banderas canonicos
                        for(size_t k=0;k<SZ[kind];++k)o[k]=uint8(next());
                        if(kind!=LIN){ W(0)=uint32(VT[kind]); W(8)=next()%4 ? uint32(uintptr_t(fakes[0].data())) : 0; }
                        auto reg=[&](CEntity* e,size_t off){ W(off)=uint32(uintptr_t(e)); if(e&&next()%4)e->RegisterReference(reinterpret_cast<CEntity**>(o+off)); };
                        switch(kind){
                        case LIN: break;
                        case GTL: o[0x14]=uint8(next()%2); reg(next()%5 ? static_cast<CEntity*>(Veh()) : nullptr,0xC); BlockRefs(uint32(gangs))=uint16(next()%3==0 ? 0 : next()%5==0 ? 0xFFFF : 1+next()%4); break;
                        case GHV: {
                            static const int32 ids[]{-1,-1,-2,int32(0x80000000u),0,1,2,3,4,5,6,7,8,15,31};
                            reg(next()%5 ? static_cast<CEntity*>(Veh()) : nullptr,0x10); W(0x14)=uint32(ids[next()%15]);
                            { const uint32 m=next()%3; reg(m==0 ? nullptr : m==1 ? static_cast<CEntity*>(Ped()) : static_cast<CEntity*>(Veh()),0x34); }
                            o[0x30]=uint8(next()%2); BlockRefs(uint32(gangs))=uint16(1+next()%4);
                            break; }
                        case GHP: reg(next()%5 ? static_cast<CEntity*>(Ped()) : nullptr,0xC); o[0x1C]=uint8(next()%2); BlockRefs(uint32(gangs))=uint16(1+next()%4); break;
                        case PHS: {
                            o[0x14]=uint8(next());
                            uint32 ab= next()%4==0 ? 0xFFFFFFFFu : next()%nBlocks;
                            if((o[0x14]&4)&&ab==0xFFFFFFFFu)ab=next()%nBlocks;
                            AnimBlockStatic()=ab;
                            static const uint16 rc[]{0,0,1,2,3,0xFFFF};
                            if(ab!=0xFFFFFFFFu)BlockRefs(ab)=rc[next()%6];
                            const bool allNonZero=next()%3==0;
                            for(int k=0;k<4;++k)HandRefs(k)=allNonZero ? uint16(1+next()%3) : rc[next()%6];
                            break; }
                        }
                    }
                    const auto occ=Occupancy(); const auto blocksBefore=Hash(Blocks(),BlocksBytes());
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                    void* self=o; uintptr_t rawRet=0;
                    if(isCtor){
                        switch(kind){
                        case LIN: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const CVector*,int32,int32,int32)>(fn.address)(self,reinterpret_cast<const CVector*>(vec),ai[0],ai[1],ai[2])); break;
                        case GTL: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32)>(fn.address)(self,static_cast<CVehicle*>(entArg),ai[0])); break;
                        case GHV: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CVehicle*,int32,uint32,float,float)>(fn.address)(self,static_cast<CVehicle*>(entArg),ai[0],rawByte,af[0],af[1])); break;
                        case GHP: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,CPed*,int32,int32,int32)>(fn.address)(self,static_cast<CPed*>(entArg),ai[0],ai[1],ai[2])); break;
                        case PHS: rawRet=uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32,float)>(fn.address)(self,ai[0],af[0])); break;
                        }
                    } else reinterpret_cast<void*(__thiscall*)(void*)>(fn.address)(self);   // el retorno del destructor del EXE no esta definido
                    for(auto* h:hooks)h->State(false);
                    json type=nullptr; if(isCtor&&kind!=LIN)type=int(reinterpret_cast<CTask*>(self)->GetTaskType());   // vptr EXE o DLL: se compara el tipo
                    json refList=json::array();
                    for(CEntity* e:{static_cast<CEntity*>(Ped()),static_cast<CEntity*>(Veh())}){ json l=json::array(); for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); refList.push_back(l); }
                    json hands=json::array(); for(int k=0;k<4;++k)hands.push_back(HandRefs(k));
                    const bool guardsOk=std::all_of(objBuf.begin(),objBuf.begin()+16,[](uint8 b){return b==0xCD;})&&std::all_of(objBuf.end()-16,objBuf.end(),[](uint8 b){return b==0xCD;});
                    json r={{"ret",isCtor ? Name(rawRet) : json(nullptr)},{"type",type},{"object",Region(o,OBJ,kind!=LIN)},{"guards",guardsOk},
                            {"refs",refList},{"ped",EntHash(ped)},{"veh",EntHash(veh)},{"fakes",Region(fakes[0].data(),fakes[0].size(),true)},
                            {"blocks_before",blocksBefore},{"blocks",Hash(Blocks(),BlocksBytes())},{"hands",hands},{"anim_block",AnimBlockStatic()},{"clock",Clock()},
                            {"trace",trace},{"clean",Occupancy()==occ},{"switches",switches}};
                    CleanRefs();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true&&original["guards"]==true;
                passed&=ok; failures+=!ok;
                auto digest=[](const json& o){ return Fnv(o["object"].dump(),Fnv(o["blocks"].dump())); };
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},
                             {"original",{{"ret",original["ret"]},{"type",original["type"]},{"trace",original["trace"]},{"state_fnv",digest(original)}}},
                             {"candidate",{{"ret",candidate["ret"]},{"type",candidate["type"]},{"trace",candidate["trace"]},{"state_fnv",digest(candidate)}}},
                             {"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        patches.clear();   // restaura las entradas
    }
    CleanRefs();
    std::memcpy(Blocks(),savedBlocks.data(),savedBlocks.size());
    for(int k=0;k<4;++k)HandRefs(k)=savedHands[k];
    AnimBlockStatic()=savedAnimBlock; Clock()=savedClock;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    _controlfp(savedFP,_MCW_PC|_MCW_RC);
    bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&_controlfp(0,0)==savedFP;
    restored&=savedBlocks==ReversingProbe::Bytes(Blocks(),BlocksBytes());
    for(int k=0;k<4;++k)restored&=HandRefs(k)==savedHands[k];
    restored&=AnimBlockStatic()==savedAnimBlock&&Clock()==savedClock;
    for(auto& [a,b]:entryBefore)restored&=b==ReversingProbe::Bytes(reinterpret_cast<void*>(a),8);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
