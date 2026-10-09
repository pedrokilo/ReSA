#pragma once
// Banco dentro del motor (estilo Small2/PadMouse1, partida cargada via FastLoader): las funciones que faltaban para dejar
// COMPLETAS CTaskSimpleFightingControl y CTaskComplexProstituteSolicit (constructores/destructores y el resto de virtuales ya
// estaban validados en otros bancos):
//   CTaskSimpleFightingControl::CalcMoveCommand (EXE 0x624B50), Clone (vtable 0x86D700 hueco 1 -> 0x622EB0) y
//   MakeAbortable (hueco 6 -> 0x61DD00);
//   CTaskComplexProstituteSolicit::Clone (vtable 0x86FB88 hueco 1 -> 0x6622F0), GetRidOfPlayerProstitute (0x661D30),
//   IsTaskValid (0x661BB0) y MakeAbortable (hueco 6 -> 0x661B80).
// Original (hooks del banco desactivados) frente a candidato con fixtures identicos y deterministas. Peatones, entidad objetivo,
// vehiculos, inteligencia, datos de jugador, tareas falsas (vtable trazada: GetSubTask, GetTaskType, MakeAbortable) y evento falso
// (vtable trazada: GetEventType, GetEventPriority) viven en buffers propios con desplazamientos crudos del EXE para los miembros
// protegidos (CPed 0x47C inteligencia, 0x480 datos de jugador, 0x58C vehiculo, 0x46C/0x474 banderas).
// Colaboradores simulados y trazados (mismo stub en la direccion del EXE y en el simbolo de la DLL, solo en el hilo del banco):
// rand del EXE (0x821B1E), FindPlayerPed (0x56E210), ComputeEntityBoundingBoxPlanesUncachedAll (0x5F3660), ComputeEntityHitSide
// (0x5F3AC0), CTheScripts::IsPlayerOnAMission (0x464D50) y CGameLogic::IsCoopGameGoingOn (0x441390). Se ejecutan de verdad:
// CTaskManager::FindTaskByType/GetSimplestActiveTask, CVehicle::GetVehicleAppearance/IsUpsideDown/IsOnItsSide, CVector::Normalise,
// CTimer, el pool de tareas (Clone: operator new real; las tareas nuevas se detectan en el pool y se borran), las referencias.
// Se compara: retorno, memoria de la tarea probada, tareas creadas, tareas falsas, hash de todos los buffers (referencias
// enmascaradas), listas de referencias antes/despues del borrado, traza completa (incluye el numero y orden de llamadas virtuales
// y de rand), palabra de control x87 y pila x87 vacia. El x87 corre a 24 bits como el juego.
#include <fstream>
#include <unordered_map>
#include <map>
#include <float.h>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TaskSimpleFightingControl.h"
#include "TaskSimpleFight.h"
#include "TaskComplexProstituteSolicit.h"
#include "PlayerPedData.h"
#include "PedIntelligence.h"
#include "PedGeometryAnalyser.h"
#include "GameLogic.h"
#include "Scripts/TheScripts.h"
#include "Entity/Vehicle/Automobile.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace TaskFinal2Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const std::string& s,uint64 h=1469598103934665603ull){ for(unsigned char c:s){h^=c;h*=1099511628211ull;} return h; }
// Hilo del banco: otros hilos del juego (audio, streaming) llaman a funciones parcheadas; no se trazan ni tocan el estado compartido.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

constexpr int FAKES=10;
constexpr size_t FK_TYPE=0x38, FK_ABORT=0x3C, FK_SUB=0x40, FK_SIZE=0x60;   // configuracion por objeto dentro de cada tarea falsa
constexpr size_t P_FLAGS=0x46C, P_FLAGS3=0x474, P_INTEL=0x47C, P_PDATA=0x480, P_VEH=0x58C, P_ROT=0x558;   // CPed (EXE)
constexpr size_t D_PROSTITUTE=0xA4;                                                                       // CPlayerPedData
constexpr size_t V_MATRIX=0x14, V_HANDLING=0x384, V_DRIVER=0x460, V_NPASS=0x484, V_MAXPASS=0x488, H_FLAGS=0xCC;   // CVehicle / tHandlingData
constexpr size_t I_PRIMARY=0x4, I_SECONDARY=0x18;      // CPedIntelligence: m_TaskMgr en +4 (primarias +4, secundarias +0x18)
constexpr size_t E_NORMAL=0x18;                         // evento de colision: normal de impacto
constexpr size_t SZ_FC=0x24, SZ_PS=0x30;
constexpr uintptr_t TIME_STEP=0xB7CB5C;
alignas(16) inline std::array<uint8,sizeof(CPed)> pedP{}, pedQ{};
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> tgt{}, vehA{}, vehB{};
alignas(16) inline std::array<uint8,sizeof(CMatrixLink)> matP{}, matQ{}, matT{}, matA{}, matB{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelP{}, intelQ{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> pdata{};
alignas(16) inline std::array<uint8,sizeof(tHandlingData)> handA{}, handB{};
alignas(16) inline std::array<std::array<uint8,FK_SIZE>,FAKES> fakes{};
alignas(16) inline std::array<uint8,0x40> event{};
alignas(16) inline std::array<uint8,0x80> object{};
inline CPed* PedP(){ return reinterpret_cast<CPed*>(pedP.data()); }
inline CPed* PedQ(){ return reinterpret_cast<CPed*>(pedQ.data()); }
inline CEntity* Tgt(){ return reinterpret_cast<CEntity*>(tgt.data()); }
inline CTask* Fake(int n){ return reinterpret_cast<CTask*>(fakes[n].data()); }
template<class T=uint32> T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(auto it=freshIds.find(v);it!=freshIds.end())return "T"+std::to_string(it->second);
    auto in=[&](const void* b,size_t n,const char* nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return std::string(nm)+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    if(auto s=in(pedP.data(),pedP.size(),"pedP"))return *s; if(auto s=in(pedQ.data(),pedQ.size(),"pedQ"))return *s;
    if(auto s=in(tgt.data(),tgt.size(),"tgt"))return *s; if(auto s=in(vehA.data(),vehA.size(),"vehA"))return *s; if(auto s=in(vehB.data(),vehB.size(),"vehB"))return *s;
    if(auto s=in(intelP.data(),intelP.size(),"intelP"))return *s; if(auto s=in(intelQ.data(),intelQ.size(),"intelQ"))return *s;
    if(auto s=in(pdata.data(),pdata.size(),"pdata"))return *s; if(auto s=in(handA.data(),handA.size(),"handA"))return *s; if(auto s=in(handB.data(),handB.size(),"handB"))return *s;
    if(auto s=in(matP.data(),matP.size(),"matP"))return *s; if(auto s=in(matQ.data(),matQ.size(),"matQ"))return *s; if(auto s=in(matT.data(),matT.size(),"matT"))return *s;
    if(auto s=in(matA.data(),matA.size(),"matA"))return *s; if(auto s=in(matB.data(),matB.size(),"matB"))return *s;
    for(int n=0;n<FAKES;++n)if(auto s=in(fakes[n].data(),fakes[n].size(),"fk"))return *s+"#"+std::to_string(n);
    if(auto s=in(event.data(),event.size(),"event"))return *s; if(auto s=in(object.data(),object.size(),"object"))return *s;
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
inline void CleanRefs() {
    auto clean=[](CEntity* e){ while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(reinterpret_cast<CEntity*>(pedP.data())); clean(reinterpret_cast<CEntity*>(pedQ.data())); clean(reinterpret_cast<CEntity*>(tgt.data()));
    clean(reinterpret_cast<CEntity*>(vehA.data())); clean(reinterpret_cast<CEntity*>(vehB.data()));
}
inline uint16 FpuTag() {   // pila x87 vacia = 0xFFFF; fnstenv enmascara las excepciones, fldenv restaura el entorno tal cual
    uint8 env[28]{};
    __asm { lea eax, env
            fnstenv [eax]
            fldenv [eax] }
    uint16 tag; std::memcpy(&tag,env+8,2); return tag;
}

// configuracion por caso de las dependencias simuladas
inline int eventType{}, eventPrio{}; inline std::array<int,8> randQ{}; inline unsigned randIdx{};
inline uintptr_t playerRet{}, realPlayer{}; inline bool missionRet{}, coopRet{};
inline std::array<CVector,4> planeVals{}; inline std::array<float,4> dotVals{}; inline int hitSideRet{};

// tareas falsas: cada objeto lleva su tipo, su respuesta a MakeAbortable y su subtarea
inline void* __fastcall FDtor(CTask* t,void*,unsigned f) { if(Mine())trace.push_back({"delete_fake",Name(uintptr_t(t)),f}); return t; }
inline CTask* __fastcall FSub(CTask* t,void*) { if(Mine())trace.push_back({"get_sub_task",Name(uintptr_t(t))}); return *reinterpret_cast<CTask**>(uintptr_t(t)+FK_SUB); }
inline bool __fastcall FSimple(CTask*,void*) { return false; }
inline int __fastcall FType(CTask* t,void*) { if(Mine())trace.push_back({"get_task_type",Name(uintptr_t(t))}); return *reinterpret_cast<int32*>(uintptr_t(t)+FK_TYPE); }
inline bool __fastcall FAbort(CTask* t,void*,CPed* p,int prio,const CEvent* e) { if(Mine())trace.push_back({"make_abortable",Name(uintptr_t(t)),Name(uintptr_t(p)),prio,Name(uintptr_t(e))}); return *reinterpret_cast<uint8*>(uintptr_t(t)+FK_ABORT)!=0; }
template<int N> void* __fastcall FTrap(CTask*,void*) { if(Mine())trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }
// evento falso (GetEventType hueco 1, GetEventPriority hueco 2)
inline int __fastcall EType(void* e,void*) { if(Mine())trace.push_back({"event_get_type",Name(uintptr_t(e))}); return eventType; }
inline int __fastcall EPrio(void* e,void*) { if(Mine())trace.push_back({"event_get_priority",Name(uintptr_t(e))}); return eventPrio; }
template<int N> void* __fastcall ETrap(void*,void*) { if(Mine())trace.push_back({"event_unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> eventVtbl{};
template<int... N> void FillETraps(std::integer_sequence<int,N...>) { ((eventVtbl[N]=reinterpret_cast<void*>(&ETrap<N>)),...); }

// dependencias simuladas
inline int __cdecl FakeRand() { if(!Mine())return 0x1234; const int v=randQ[randIdx++%8]; trace.push_back({"rand",v}); return v; }
inline CPlayerPed* __cdecl FindPlayer(int32 id) { if(!Mine())return reinterpret_cast<CPlayerPed*>(realPlayer); trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
// EXE 0x5F3660 cdecl(float z, CEntity*, CVector* planos[4], float* distancias[4]); la DLL pasa referencias/punteros a arrays (mismo ABI)
inline void __cdecl Planes(float z,CEntity* e,CVector* planes,float* dots) {
    if(!Mine())return; trace.push_back({"planes",F(z),Name(uintptr_t(e)),planes!=nullptr,dots!=nullptr});
    for(int k=0;k<4;++k){ planes[k]=planeVals[k]; dots[k]=dotVals[k]; }
}
// EXE 0x5F3AC0 cdecl(const CVector*, CEntity*) -> eDirection
inline int32 __cdecl HitSide(const CVector* p,CEntity* e) { if(!Mine())return 0; trace.push_back({"hit_side",F(p->x),F(p->y),F(p->z),Name(uintptr_t(e))}); return hitSideRet; }
inline bool __cdecl OnMission() { if(!Mine())return false; trace.push_back({"on_mission",missionRet}); return missionRet; }
inline bool __cdecl Coop() { if(!Mine())return false; trace.push_back({"coop",coopRet}); return coopRet; }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j)a+=5+*reinterpret_cast<int32*>(a+1); return a; }

enum Kind { FC_CALC, FC_CLONE, FC_ABORT, PS_CLONE, PS_GETRID, PS_VALID, PS_ABORT };
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=7;
inline const std::array<Fn,NFNS> fns{{
    {"CTaskSimpleFightingControl","CalcMoveCommand",0x624B50,0,-1,1024},
    {"CTaskSimpleFightingControl","Clone",0,0x86D700,1,128},
    {"CTaskSimpleFightingControl","MakeAbortable",0,0x86D700,6,1024},
    {"CTaskComplexProstituteSolicit","Clone",0,0x86FB88,1,128},
    {"CTaskComplexProstituteSolicit","GetRidOfPlayerProstitute",0x661D30,0,-1,256},
    {"CTaskComplexProstituteSolicit","IsTaskValid",0x661BB0,0,-1,1024},
    {"CTaskComplexProstituteSolicit","MakeAbortable",0,0x86FB88,6,256},
}};
constexpr std::array<Kind,NFNS> KINDS{FC_CALC,FC_CLONE,FC_ABORT,PS_CLONE,PS_GETRID,PS_VALID,PS_ABORT};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"taskfinal2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"taskfinal2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("taskfinal2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId(); realPlayer=uintptr_t(player);
    std::string only; { std::ifstream f(dir/"taskfinal2-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // Comprobacion de la disposicion de miembros publicos frente a los desplazamientos del EXE usados por los fixtures.
    auto off=[](const void* base,const void* member){ return uintptr_t(member)-uintptr_t(base); };
    {
        auto* ped=reinterpret_cast<CPed*>(pedP.data()); auto* veh=reinterpret_cast<CVehicle*>(vehA.data()); auto* h=reinterpret_cast<tHandlingData*>(handA.data());
        auto* fc=reinterpret_cast<CTaskSimpleFightingControl*>(object.data()); auto* ps=reinterpret_cast<CTaskComplexProstituteSolicit*>(object.data());
        const bool layout=off(ped,&ped->m_fCurrentRotation)==P_ROT && off(veh,&veh->m_pHandlingData)==V_HANDLING && off(veh,&veh->m_pDriver)==V_DRIVER &&
            off(veh,&veh->m_nNumPassengers)==V_NPASS && off(veh,&veh->m_nMaxPassengers)==V_MAXPASS && off(h,&h->m_nModelFlags)==H_FLAGS &&
            off(ped,&ped->m_matrix)==V_MATRIX && off(fc,&fc->m_target)==0xC && off(fc,&fc->m_angleRad)==0x10 && off(fc,&fc->m_maxAttackRange)==0x14 &&
            off(fc,&fc->m_nextAttackTime)==0x1C && off(ps,&ps->m_pClient)==0xC && off(ps,&ps->m_nCurrentTimer)==0x2A && sizeof(CTaskSimpleFightingControl)==SZ_FC &&
            sizeof(CTaskComplexProstituteSolicit)==SZ_PS;
        Write({{"event","layout"},{"ok",layout}});
        if(!layout){Write({{"event","error"},{"reason","layout mismatch between DLL members and EXE offsets"}});return;}
    }
    FillTraps(std::make_integer_sequence<int,12>{});
    fakeVtbl[0]=reinterpret_cast<void*>(&FDtor); fakeVtbl[2]=reinterpret_cast<void*>(&FSub); fakeVtbl[3]=reinterpret_cast<void*>(&FSimple);
    fakeVtbl[4]=reinterpret_cast<void*>(&FType); fakeVtbl[6]=reinterpret_cast<void*>(&FAbort);
    FillETraps(std::make_integer_sequence<int,16>{}); eventVtbl[1]=reinterpret_cast<void*>(&EType); eventVtbl[2]=reinterpret_cast<void*>(&EPrio);

    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    auto& timeStep=*reinterpret_cast<float*>(TIME_STEP); const float savedTimeStep=timeStep;
    const int32 savedLastFree=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC);
    const unsigned savedFP=_controlfp(0,0);
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    // bytes iniciales de las entradas parcheables, para comprobar la restauracion
    struct Site { uintptr_t exeAddr, dll; };
    const std::array<Site,6> sites{{
        {0x821B1E,0},
        {0x56E210,std::bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))},
        {0x5F3660,std::bit_cast<uintptr_t>(&CPedGeometryAnalyser::ComputeEntityBoundingBoxPlanes)},
        {0x5F3AC0,std::bit_cast<uintptr_t>(static_cast<int32(*)(const CVector&,CEntity&)>(&CPedGeometryAnalyser::ComputeEntityHitSide))},
        {0x464D50,std::bit_cast<uintptr_t>(&CTheScripts::IsPlayerOnAMission)},
        {0x441390,std::bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn)}}};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entryBefore;
    for(auto& s:sites)for(auto a:{s.exeAddr,s.dll,s.dll ? Body(s.dll) : uintptr_t(0)})if(a)entryBefore.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),8)});
    for(auto& fn:fns)if(fn.address)entryBefore.push_back({fn.address,ReversingProbe::Bytes(reinterpret_cast<void*>(fn.address),8)});
    std::vector<std::pair<uintptr_t,uintptr_t>> vtBefore;
    for(auto& fn:fns)if(fn.vt)vtBefore.push_back({fn.vt+fn.slot*4,*reinterpret_cast<uintptr_t*>(fn.vt+fn.slot*4)});
    Write({{"event","begin"},{"fp_control",savedFP},{"hooks",NFNS}});

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto put=[&](uintptr_t at,auto stub){ patches.push_back(std::make_unique<EntryPatch>(at,reinterpret_cast<void*>(stub))); };
        /* El cuerpo real se calcula ANTES de parchear la entrada: tras parchear s.dll la cadena de E9 pasa por el propio stub y
           parchear "Body(s.dll)" sobreescribiria la entrada del stub con un salto a si mismo (bucle infinito). */
        auto both=[&](const Site& s,auto stub){ const uintptr_t body=s.dll ? Body(s.dll) : 0; put(s.exeAddr,stub); if(s.dll){ put(s.dll,stub); if(body!=s.dll)put(body,stub); } };
        put(0x821B1E,&FakeRand);
        both(sites[1],&FindPlayer); both(sites[2],&Planes); both(sites[3],&HitSide); both(sites[4],&OnMission); both(sites[5],&Coop);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm; const Kind kind=KINDS[f];
            if(!only.empty()&&only!=full)continue;
            std::map<std::string,int> hist;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura para que ambas ejecuciones reciban el mismo slot
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      // slots libres con 0xCD: el New del EXE no escribe en el objeto y los bytes sin usar quedarian con restos
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* sts=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(sts[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7F2Au; if(!st)st=1; trace=json::array(); randIdx=0;
                    for(auto& q:randQ){ uint32 v=next()&0x7FFF; const uint32 m=next()%4; if(m==0)v&=~0x3Fu; else if(m==1)v&=~0xFu; q=int(v); }
                    CleanRefs();
                    for(auto* b:{&pedP,&pedQ})std::memset(b->data(),0,b->size());
                    for(auto* b:{&tgt,&vehA,&vehB})std::memset(b->data(),0,b->size());
                    for(auto* b:{&matP,&matQ,&matT,&matA,&matB})std::memset(b->data(),0,b->size());
                    for(auto* b:{&intelP,&intelQ})std::memset(b->data(),0,b->size());
                    std::memset(pdata.data(),0,pdata.size()); std::memset(handA.data(),0,handA.size()); std::memset(handB.data(),0,handB.size());
                    for(int n=0;n<FAKES;++n){ std::memset(fakes[n].data(),0,FK_SIZE); *reinterpret_cast<void**>(fakes[n].data())=fakeVtbl.data();
                        At<int32>(fakes[n].data(),FK_TYPE)=TASK_SIMPLE_STAND_STILL; At<uint8>(fakes[n].data(),FK_ABORT)=uint8(next()%2); }
                    for(auto& b:event)b=uint8(next()%4==0 ? next() : 0); *reinterpret_cast<void**>(event.data())=eventVtbl.data();
                    planeVals.fill(CVector{0.f,0.f,0.f}); dotVals.fill(0.f); hitSideRet=0; missionRet=false; coopRet=false; playerRet=0;
                    auto* P=PedP(); auto* Q=PedQ(); P->SetType(ENTITY_TYPE_PED); Q->SetType(ENTITY_TYPE_PED);
                    At(P,P_INTEL)=uint32(uintptr_t(intelP.data())); At(Q,P_INTEL)=uint32(uintptr_t(intelQ.data()));
                    auto place=[&](auto& buf,auto& mat,bool useMat,const CVector& pos,int type)->CEntity* {
                        auto* e=reinterpret_cast<CEntity*>(buf.data()); e->SetType(eEntityType(type));
                        if(useMat){ CMatrix m{}; m.SetRotate(0.f,0.f,uni(-3.f,3.f)); m.GetPosition()=pos; e->m_matrix=reinterpret_cast<CMatrixLink*>(mat.data()); std::memcpy(mat.data(),&m,sizeof(CMatrix)); }
                        else e->SetPosn(pos);
                        return e; };
                    // tareas encadenadas: cada nodo apunta a su subtarea (FK_SUB)
                    int fk=0;
                    auto chain=[&](int len,int lastType)->CTask* {
                        CTask* head=nullptr; CTask* prev=nullptr;
                        for(int k=0;k<len&&fk<FAKES;++k){ CTask* t=Fake(fk++);
                            static const int mids[]{TASK_SIMPLE_STAND_STILL,TASK_SIMPLE_PAUSE,TASK_SIMPLE_CAR_DRIVE,TASK_COMPLEX_PROSTITUTE_SOLICIT,TASK_SIMPLE_FIGHT};
                            At<int32>(t,FK_TYPE)=(k==len-1) ? lastType : mids[next()%5];
                            if(prev)At<CTask*>(prev,FK_SUB)=t; else head=t; prev=t; }
                        return head; };
                    std::memset(object.data(),0xCD,object.size());
                    uint8* const o=object.data(); auto W=[&](size_t offs)->uint32&{return *reinterpret_cast<uint32*>(o+offs);};
                    const uintptr_t* vt=fn.vt ? reinterpret_cast<uintptr_t*>(fn.vt) : nullptr;
                    uintptr_t vtObject=0; int prio=0; const void* evArg=nullptr; CPed* pedArg=P; CPed* prostArg=Q; float timeStepNew=1.f;

                    switch(kind){
                    case FC_CALC: {
                        vtObject=0x86D700;
                        float R=next()%5==0 ? 1.f : next()%9==0 ? 0.f : uni(0.2f,3.f);
                        const int tk=int(next()%12);
                        const int ttype= tk<=4 ? ENTITY_TYPE_PED : tk<=7 ? ENTITY_TYPE_VEHICLE : tk==8 ? ENTITY_TYPE_OBJECT : tk==9 ? ENTITY_TYPE_BUILDING : tk==10 ? ENTITY_TYPE_DUMMY : 6;
                        const int planeMode=int(next()%6); hitSideRet=int(next()%4);
                        for(int k=0;k<4;++k){ planeVals[k]=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; dotVals[k]=uni(-5.f,5.f); }
                        switch(planeMode){
                        case 1: planeVals[hitSideRet]=CVector{0.f,0.f,0.f}; dotVals[hitSideRet]=std::bit_cast<float>(0x3F333333u); break;   // justo 0.7f
                        case 2: planeVals[hitSideRet]=CVector{0.f,0.f,0.f}; dotVals[hitSideRet]=std::bit_cast<float>(0x3F333334u); break;
                        case 3: planeVals[hitSideRet]=CVector{0.f,0.f,0.f}; dotVals[hitSideRet]=std::bit_cast<float>(0x3F333332u); break;
                        case 4: for(int k=0;k<4;++k){ planeVals[k]=CVector{1.0e8f,-1.0e8f,1.f}; dotVals[k]=0.f; } break;   // sensible al orden de la suma (z,y,x)
                        default: break;
                        }
                        CVector pp{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        if(next()%8==0)pp=CVector{0.f,0.f,0.f};
                        if(ttype==ENTITY_TYPE_VEHICLE&&planeMode==4)pp=CVector{1.f,1.f,1.f};
                        float m;
                        switch(next()%10){
                        case 0: m=R+0.1f; m=std::nextafter(m,next()%2 ? 100.f : -100.f); break;
                        case 1: m=R+uni(0.1f,5.f); break;
                        case 2: m=R+uni(-0.1f,0.1f); break;
                        case 3: m=std::max(R-uni(0.1f,2.f),0.8f); break;
                        case 4: m=uni(0.05f,0.79f); R=uni(1.f,3.f); break;
                        case 5: m=std::nextafter(0.8f,next()%2 ? 100.f : -100.f); R=m+0.5f; break;
                        case 6: m=R-0.1f; m=std::nextafter(m,next()%2 ? 100.f : -100.f); break;
                        case 7: m=R+0.3f; m=std::nextafter(m,next()%2 ? 100.f : -100.f); break;
                        case 8: m=uni(0.f,10.f); break;
                        default: m=0.f; break;
                        }
                        if(m<0.f)m=0.f;
                        // angulo: casos de frontera (eje +y: el rumbo es -0 y la resta exacta) y casos generales
                        static const float bound[]{0.f,std::bit_cast<float>(0x3E860A92u),std::bit_cast<float>(0x3E860A91u),std::bit_cast<float>(0x3E860A93u),
                                                   -std::bit_cast<float>(0x3E860A92u),-std::bit_cast<float>(0x3E860A91u),-std::bit_cast<float>(0x3E860A93u),0.2617993f,-0.2617993f,0.26179939f};
                        static const float wide[]{0.01f,-0.01f,0.1f,-0.2f,0.25f,-0.26f,0.2618f,-0.2618f,0.3f,-0.5f,1.f,3.14f,-3.14f,6.2831855f,-6.2831855f,6.0f,-6.0f,3.2f,-3.2f,6.5f,-6.5f};
                        const bool boundary=next()%4==0; CVector dir{0.f,1.f,0.f}; float delta;
                        if(boundary){ delta=bound[next()%10]; }
                        else {
                            switch(next()%5){ case 0: break; case 1: dir=CVector{1.f,0.f,0.f}; break; case 2: dir=CVector{-1.f,0.f,0.f}; break; case 3: dir=CVector{0.f,-1.f,0.f}; break;
                                default: { const float a=uni(-3.14f,3.14f); dir=CVector{std::sin(a),std::cos(a),next()%3 ? 0.f : uni(-0.5f,0.5f)}; } }
                            delta= next()%10<7 ? uni(-0.25f,0.25f) : wide[next()%21];
                        }
                        const CVector tp{pp.x+dir.x*m,pp.y+dir.y*m,pp.z+dir.z*m};
                        const float h=float(std::atan2(double(-(tp.x-pp.x)),double(tp.y-pp.y)));
                        place(pedP,matP,next()%2,pp,ENTITY_TYPE_PED);
                        P->m_fCurrentRotation=h+delta;
                        if(tk!=0)place(tgt,matT,next()%2,tp,ttype);
                        std::memset(o,0xCD,object.size()); std::memset(o,0,SZ_FC);
                        At<uint8>(o,8)=uint8(next()); At<uint8>(o,9)=uint8(next()); At<uint16>(o,0xA)=uint16(next());
                        W(0xC)=tk==0 ? 0 : uint32(uintptr_t(tgt.data())); *reinterpret_cast<float*>(o+0x10)=uni(-3.f,3.f); *reinterpret_cast<float*>(o+0x14)=R;
                        *reinterpret_cast<float*>(o+0x18)=1.f; W(0x1C)=next()%5==0 ? 0xFFFFFFF0u : next(); W(0x20)=next();
                        static const float steps[]{0.f,0.5f,1.f,2.5f,3.f,100000.f,5.0e8f,-3.f,0.02f};
                        timeStepNew= next()%3 ? uni(0.2f,3.f) : steps[next()%9];
                        break; }
                    case FC_CLONE: {
                        vtObject=0x86D700;
                        const int ttype=int(next()%8); place(tgt,matT,next()%2,CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,30.f)},ttype);
                        std::memset(o,0xCD,object.size()); for(size_t k=4;k<SZ_FC;k+=4)W(k)=next();
                        W(0xC)=next()%4==0 ? 0 : uint32(uintptr_t(tgt.data()));
                        static const float fl[]{0.f,-0.f,1.f,-1.5f,3.14159f,1.0e30f,-1.0e-30f,std::numeric_limits<float>::infinity(),-std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()};
                        *reinterpret_cast<float*>(o+0x10)=next()%3 ? uni(-50.f,50.f) : fl[next()%10]; *reinterpret_cast<float*>(o+0x14)=next()%3 ? uni(0.f,10.f) : fl[next()%10];
                        break; }
                    case FC_ABORT: {
                        vtObject=0x86D700;
                        const CVector pp{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        place(pedP,matP,next()%2,pp,ENTITY_TYPE_PED);
                        const int dc=int(next()%6); float k=1.f; CVector D;
                        static const float scales[]{1.f,2.f,4.f,0.5f,8.f,3.f};
                        k=scales[next()%6];
                        switch(dc){ case 0: D=CVector{k,0.f,0.f}; break; case 1: D=CVector{-k,0.f,0.f}; break; case 2: D=CVector{0.f,k,0.f}; break; case 3: D=CVector{0.f,0.f,k}; break;
                            case 4: D=CVector{0.f,-k,0.f}; break; default: D=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-5.f,5.f)}; }
                        const int ttype= next()%2 ? ENTITY_TYPE_PED : next()%2 ? ENTITY_TYPE_VEHICLE : ENTITY_TYPE_OBJECT;
                        place(tgt,matT,next()%2,CVector{pp.x+D.x,pp.y+D.y,pp.z+D.z},ttype);
                        std::memset(o,0xCD,object.size()); std::memset(o,0,SZ_FC);
                        W(0xC)=next()%5==0 ? 0 : uint32(uintptr_t(tgt.data())); *reinterpret_cast<float*>(o+0x10)=uni(-3.f,3.f); *reinterpret_cast<float*>(o+0x14)=uni(0.f,3.f);
                        *reinterpret_cast<float*>(o+0x18)=1.f; W(0x1C)=next(); W(0x20)=next();
                        // evento: tipo, prioridad y normal de impacto
                        static const int types[]{EVENT_VEHICLE_COLLISION,EVENT_PED_COLLISION_WITH_PED,EVENT_PED_COLLISION_WITH_PLAYER,EVENT_PLAYER_COLLISION_WITH_PED,EVENT_OBJECT_COLLISION,
                                                 EVENT_BUILDING_COLLISION,EVENT_VEHICLE_COLLISION,EVENT_OBJECT_COLLISION,EVENT_BUILDING_COLLISION,0,7,13,100,EVENT_DAMAGE};
                        eventType=types[next()%14];
                        static const int prios[]{0,21,22,23,57,58,59,-5,100,22,22,23,58,22};
                        eventPrio=prios[next()%14];
                        float nx,ny,nz;
                        if(next()%2){   // normal en la frontera 0.707 para direccion axial
                            static const uint32 bits[]{0x3F34FDF4u,0x3F34FDF3u,0x3F34FDF5u,0x3F350000u,0x3F300000u,0x3F800000u};
                            nx=-std::bit_cast<float>(bits[next()%6])*(next()%4==0 ? -1.f : 1.f); ny=uni(-0.1f,0.1f)*float(next()%2); nz=uni(-0.1f,0.1f)*float(next()%2);
                        } else { nx=uni(-1.f,1.f); ny=uni(-1.f,1.f); nz=uni(-1.f,1.f); }
                        At<float>(event.data(),E_NORMAL)=nx; At<float>(event.data(),E_NORMAL+4)=ny; At<float>(event.data(),E_NORMAL+8)=nz;
                        // tarea de ataque secundaria
                        CTask* sec=next()%4==0 ? nullptr : Fake(0);
                        if(sec)At<int32>(sec,FK_TYPE)=next()%4 ? TASK_SIMPLE_FIGHT : TASK_SIMPLE_PAUSE;
                        At<CTask*>(intelP.data(),I_SECONDARY)=sec;
                        static const int ps[]{0,1,1,1,2,1,1,3,-1,7}; prio=ps[next()%10];
                        evArg= next()%6==0 ? nullptr : event.data();
                        break; }
                    case PS_CLONE: {
                        vtObject=0x86FB88;
                        place(pedQ,matQ,next()%2,CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,30.f)},ENTITY_TYPE_PED);
                        std::memset(o,0xCD,object.size()); for(size_t k=4;k<SZ_PS;k+=4)W(k)=next();
                        W(8)=uint32(uintptr_t(Fake(0))); W(0xC)=uint32(uintptr_t(pedQ.data()));
                        break; }
                    case PS_ABORT: {
                        vtObject=0x86FB88;
                        std::memset(o,0xCD,object.size()); for(size_t k=4;k<SZ_PS;k+=4)W(k)=next();
                        W(8)=uint32(uintptr_t(Fake(0))); W(0xC)=uint32(uintptr_t(pedQ.data()));
                        At<uint8>(o,0x2C)=uint8(next()); At<uint8>(o,0x2D)=uint8(next());
                        static const int ps[]{0,1,2,1,2,0,5,-3}; prio=ps[next()%8];
                        evArg= next()%5==0 ? nullptr : event.data();
                        eventType=EVENT_DAMAGE;
                        break; }
                    case PS_GETRID: {
                        playerRet=uintptr_t(P); At(P,P_PDATA)=uint32(uintptr_t(pdata.data()));
                        place(pedQ,matQ,next()%2,CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(0.f,30.f)},ENTITY_TYPE_PED);
                        At(pdata.data(),D_PROSTITUTE)= next()%5==0 ? 0 : uint32(uintptr_t(pedQ.data()));
                        for(int s=0;s<5;++s){
                            const int len=next()%3==0 ? 0 : 1+int(next()%3);
                            At<CTask*>(intelQ.data(),I_PRIMARY+4*s)=len ? chain(len,next()%2 ? TASK_COMPLEX_PROSTITUTE_SOLICIT : TASK_SIMPLE_STAND_STILL) : nullptr;
                        }
                        for(int n=0;n<FAKES;++n){ At<uint8>(fakes[n].data(),0x2A)=uint8(next()); At<uint8>(fakes[n].data(),0x2B)=uint8(next()); At<uint8>(fakes[n].data(),0x2C)=uint8(next()); At<uint8>(fakes[n].data(),0x2D)=uint8(next());
                                                   if(At<int32>(fakes[n].data(),FK_TYPE)==TASK_COMPLEX_PROSTITUTE_SOLICIT&&next()%2==0)At<uint16>(fakes[n].data(),0x2A)=0; }
                        break; }
                    case PS_VALID: {
                        bool ok[17]; for(auto& b:ok)b=true;
                        const int stage=int(next()%20); if(stage<17)ok[stage]=false;
                        if(next()%6==0)ok[next()%17]=false;
                        pedArg=P; playerRet=uintptr_t(P);
                        if(!ok[0])playerRet= next()%2 ? uintptr_t(Q) : 0;
                        if(!ok[1]){ pedArg=nullptr; playerRet= ok[0] ? 0 : uintptr_t(Q); }
                        const CVector pp{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                        place(pedP,matP,next()%2,pp,ENTITY_TYPE_PED);
                        // posicion del prostituto: distancia al cuadrado <= 100 (frontera exacta) o mayor
                        CVector qp;
                        if(ok[14]){ switch(next()%5){ case 0: qp=CVector{pp.x+10.f,pp.y,pp.z}; break; case 1: qp=CVector{pp.x+6.f,pp.y+8.f,pp.z}; break; case 2: qp=pp; break;
                                                        case 3: qp=CVector{pp.x+uni(-5.f,5.f),pp.y+uni(-5.f,5.f),pp.z+uni(-3.f,3.f)}; break; default: qp=CVector{pp.x,pp.y-9.99f,pp.z}; } }
                        else { switch(next()%4){ case 0: qp=CVector{pp.x+10.001f,pp.y,pp.z}; break; case 1: qp=CVector{pp.x+8.f,pp.y+8.f,pp.z+8.f}; break; case 2: qp=CVector{pp.x+100.f,pp.y,pp.z}; break;
                                                 default: qp=CVector{pp.x,pp.y,pp.z+std::nextafter(10.f,100.f)}; } }
                        place(pedQ,matQ,next()%2,qp,ENTITY_TYPE_PED);
                        At(P,P_FLAGS)=(next()&~0x100u)|(ok[2] ? 0x100u : 0u);
                        At<uint8>(P,P_FLAGS3)=uint8((next()&~0x40u)|(ok[4] ? 0u : 0x40u));
                        At(P,P_VEH)= ok[3] ? uint32(uintptr_t(vehA.data())) : 0;
                        At(P,P_PDATA)=uint32(uintptr_t(pdata.data()));
                        At(pdata.data(),D_PROSTITUTE)= ok[5] ? (next()%2 ? 0u : uint32(uintptr_t(Q))) : uint32(uintptr_t(P));
                        // vehiculo del jugador
                        uint32 hf=next()&~0x0F000000u&~0x200u;
                        if(!ok[6]){ static const uint32 bad[]{0x01000000u,0x02000000u,0x04000000u,0x08000000u,0x03000000u,0x05000000u,0x0F000000u}; hf|=bad[next()%7]; }
                        if(!ok[13])hf|=0x200u;
                        At(handA.data(),H_FLAGS)=hf; At(vehA.data(),V_HANDLING)=uint32(uintptr_t(handA.data()));
                        reinterpret_cast<CEntity*>(vehA.data())->SetType(ENTITY_TYPE_VEHICLE);
                        { float upz,rz;
                          if(ok[7]){ static const float v[]{1.f,0.5f,-0.5f,0.f}; upz= next()%3==0 ? std::nextafter(-0.9f,0.f) : next()%2 ? v[next()%4] : uni(-0.89f,1.f); }
                          else { static const float v[]{-1.f,-0.95f}; upz= next()%3==0 ? -0.9f : next()%3==0 ? std::nextafter(-0.9f,-100.f) : v[next()%2]; }
                          if(ok[8]){ rz= next()%3==0 ? std::nextafter(0.8f,0.f) : next()%4==0 ? std::nextafter(-0.8f,0.f) : uni(-0.79f,0.79f); }
                          else { static const float v[]{0.8f,1.f,-1.f,-0.8f}; rz= next()%3==0 ? std::nextafter(0.8f,100.f) : next()%4==0 ? std::nextafter(-0.8f,-100.f) : v[next()%4]; }
                          CMatrix m{}; m.SetUnity(); m.GetRight()=CVector{uni(-0.2f,0.2f),uni(-0.2f,0.2f),rz}; m.GetForward()=CVector{0.f,1.f,0.f}; m.GetUp()=CVector{uni(-0.2f,0.2f),uni(-0.2f,0.2f),upz};
                          m.GetPosition()=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(0.f,30.f)};
                          std::memcpy(matA.data(),&m,sizeof(CMatrix)); At(vehA.data(),V_MATRIX)=uint32(uintptr_t(matA.data())); }
                        At(vehA.data(),V_DRIVER)= ok[10] ? uint32(uintptr_t(P)) : next()%2 ? 0u : uint32(uintptr_t(Q));
                        At<uint8>(vehA.data(),V_MAXPASS)= ok[12] ? uint8(1+next()%8) : uint8(0);
                        uint32 prosVeh; uint8 npass;
                        if(ok[11]){ if(next()%2){ prosVeh=0; npass=0; } else { prosVeh=uint32(uintptr_t(vehA.data())); npass=1; } }
                        else switch(next()%3){ case 0: prosVeh=uint32(uintptr_t(vehB.data())); npass=uint8(next()%3); break;
                                               case 1: prosVeh=uint32(uintptr_t(vehA.data())); npass=uint8(next()%2 ? 0 : 2+next()%250); break;
                                               default: prosVeh=0; npass=uint8(1+next()%5); }
                        At(Q,P_VEH)=prosVeh; At<uint8>(vehA.data(),V_NPASS)=npass;
                        reinterpret_cast<CEntity*>(vehB.data())->SetType(ENTITY_TYPE_VEHICLE);
                        missionRet=!ok[15]; coopRet=!ok[16];
                        // tareas del jugador: la primera ranura ocupada manda; el ultimo nodo de la cadena decide
                        const int s0=int(next()%5);
                        for(int s=s0;s<5;++s){ if(s>s0&&next()%2)continue;
                            At<CTask*>(intelP.data(),I_PRIMARY+4*s)=chain(1+int(next()%3), s==s0 ? (ok[9] ? TASK_SIMPLE_CAR_DRIVE : (next()%2 ? TASK_SIMPLE_STAND_STILL : TASK_COMPLEX_PROSTITUTE_SOLICIT)) : TASK_SIMPLE_CAR_DRIVE); }
                        break; }
                    }

                    const auto occ=Occupancy();
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t target= fn.vt ? vt[fn.slot] : fn.address;
                    // despues de State(), no antes
                    const bool switches= fn.vt ? ((target>=0x400000&&target<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    if(vtObject)*reinterpret_cast<uintptr_t*>(o)=vtObject;
                    float savedStepRun=timeStep; if(kind==FC_CALC)timeStep=timeStepNew;
                    void* self=o; uintptr_t rawRet=0; json ret=nullptr;
                    uint16 fpuTag=0xFFFF; unsigned fpAfter=0;
                    {
                        _controlfp(_PC_24,_MCW_PC);                                   // x87 a 24 bits como el juego
                        switch(kind){
                        case FC_CALC: ret=int(reinterpret_cast<int16(__thiscall*)(void*,CPed*)>(target)(self,P)); break;
                        case FC_CLONE: case PS_CLONE: rawRet=uintptr_t(reinterpret_cast<CTask*(__thiscall*)(void*)>(target)(self)); break;
                        case FC_ABORT: case PS_ABORT: ret=reinterpret_cast<bool(__thiscall*)(void*,CPed*,int,const void*)>(target)(self,P,prio,evArg); break;
                        case PS_GETRID: reinterpret_cast<void(__cdecl*)()>(target)(); ret="void"; break;
                        case PS_VALID: ret=reinterpret_cast<bool(__cdecl*)(CPed*,CPed*)>(target)(prostArg,pedArg); break;
                        }
                        fpuTag=FpuTag(); fpAfter=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                    }
                    timeStep=savedStepRun;
                    for(auto* h:hooks)h->State(false);
                    freshIds.clear(); std::vector<CTask*> fresh; auto* pool=GetTaskPool();
                    for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!pool->IsFreeSlotAtIndex(k)){fresh.push_back(pool->GetAt(k));freshIds[uintptr_t(pool->GetAt(k))]=int(fresh.size()-1);}
                    if(ret.is_null())ret=Name(rawRet);
                    json tasks=json::array(); for(auto* t:fresh){ tasks.push_back({{"type",t->GetTaskType()},{"words",Region(t,0x70,true)}}); }
                    const json objectWords=Region(o,object.size(),true);   // el objeto probado completo (los bytes tras la tarea siguen en 0xCD)
                    auto refList=[&](CEntity* e){json l=json::array();for(auto* r=e->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity)));return l;};
                    json refs=json::array(); for(auto* e:{reinterpret_cast<CEntity*>(pedP.data()),reinterpret_cast<CEntity*>(pedQ.data()),reinterpret_cast<CEntity*>(tgt.data()),
                                                          reinterpret_cast<CEntity*>(vehA.data()),reinterpret_cast<CEntity*>(vehB.data())})refs.push_back(refList(e));
                    json fakeWords=json::array(); for(int n=0;n<FAKES;++n)fakeWords.push_back(Region(fakes[n].data(),FK_SIZE,true));
                    json ents={EntHash(pedP),EntHash(pedQ),EntHash(tgt),EntHash(vehA),EntHash(vehB),Hash(intelP.data(),intelP.size()),Hash(intelQ.data(),intelQ.size()),
                               Hash(pdata.data(),pdata.size()),Hash(handA.data(),handA.size()),Hash(handB.data(),handB.size()),Hash(matP.data(),matP.size()),Hash(matQ.data(),matQ.size()),
                               Hash(matT.data(),matT.size()),Hash(matA.data(),matA.size()),Hash(matB.data(),matB.size()),Hash(event.data(),event.size())};
                    json r={{"ret",ret},{"object",objectWords},{"tasks",tasks},{"fakes",fakeWords},{"refs",refs},{"buffers",ents},{"time_step",F(timeStep)},
                            {"fp",fpAfter},{"fpu_tag",fpuTag}};
                    // borrado de las tareas creadas
                    const size_t traceBefore=trace.size();
                    playerRet=0;   // el destructor de Prostitute busca al jugador: se deja sin jugador (no toca nada del fixture)
                    for(auto* t:fresh)delete t;
                    json delTrace=json::array(); for(size_t k=traceBefore;k<trace.size();++k)delTrace.push_back(trace[k]);
                    const bool clean=Occupancy()==occ;
                    r["refs_after"]=json::array({refList(reinterpret_cast<CEntity*>(pedP.data())),refList(reinterpret_cast<CEntity*>(pedQ.data())),refList(reinterpret_cast<CEntity*>(tgt.data()))});
                    trace.erase(trace.begin()+ptrdiff_t(traceBefore),trace.end());
                    r["trace"]=trace; r["delete_trace"]=delTrace; r["rng"]=randIdx; r["clean"]=clean; r["switches"]=switches;
                    CleanRefs();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["clean"]==true&&original["fpu_tag"]==0xFFFF;
                passed&=ok; failures+=!ok;
                hist[original["ret"].dump()]++;
                auto digest=[](const json& o){ return Fnv(o.dump()); };
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},
                             {"original",{{"ret",original["ret"]},{"trace",original["trace"]},{"state_fnv",digest(original)}}},{"candidate",{{"ret",candidate["ret"]},{"trace",candidate["trace"]},{"state_fnv",digest(candidate)}}},
                             {"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases},{"return_histogram",hist}});
        }
        patches.clear();   // restaura las entradas
    }
    CleanRefs();
    timeStep=savedTimeStep; *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC)=savedLastFree;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    _controlfp(savedFP,_MCW_PC|_MCW_RC);
    bool restored=Occupancy()==occupancyStart&&freeRefs()==savedFree&&_controlfp(0,0)==savedFP&&timeStep==savedTimeStep;
    for(auto& [a,b]:entryBefore)restored&=b==ReversingProbe::Bytes(reinterpret_cast<void*>(a),8);
    for(auto& [a,v]:vtBefore)restored&=*reinterpret_cast<uintptr_t*>(a)==v;   // huecos de vtable como estaban al empezar
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
