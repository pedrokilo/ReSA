#pragma once
// Banco dentro del motor: FIX_BUGS quitados en funciones validadas (21 funciones). Cada caso entra por la ruta que antes
// solo tomaba la DLL con FIX_BUGS (o por la ruta del EXE que la DLL desviaba) y compara el EXE (hooks del banco
// desactivados) con la DLL. Todo trabaja sobre buferes propios o sobre globales que se guardan y se restauran en cada
// ejecucion: dirección de la cámara (0xB6F9AC), tablas de CPopCycle, puntos de cobertura, gCdStreams/gLastCdStreamPosn,
// pool de entradas (puntero sustituido por un pool lleno falso), campos de frecuencia/clase/referencias de los model
// infos de vehículos, m_LastFreeSlot del pool de tareas y referencias. Se simulan y trazan: rand (EXE y DLL),
// C3dMarkers::PlaceMarker, las consultas de supresión/bloqueo/descarga de vehículos y el printf de depuración del EXE.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Glass.h"
#include "Cover.h"
#include "CoverPoint.h"
#include "PopCycle.h"
#include "Directory.h"
#include "EntryExitManager.h"
#include "LoadedCarGroup.h"
#include "Checkpoint.h"
#include "3dMarkers.h"
#include "Streaming.h"
#include "Scripts/TheScripts.h"
#include "PedGroup.h"
#include "Tasks/Allocators/PedGroup/PedGroupDefaultTaskAllocators.h"
#include "Pools/TaskPool.h"
#include "../ProbeRuntime.hpp"

namespace FixBugsLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- buferes de los fixtures ---
alignas(16) inline std::array<uint8,0x40> taskMgr{};                 // CTaskManager falso
alignas(16) inline std::array<uint8,0x20> zone{};               // CZoneInfo falso (PopType en +0xF & 0x1F)
alignas(16) inline std::array<uint8,0x20> coverPt{};            // CCoverPoint (0x1C)
alignas(16) inline std::array<uint8,0x300> interp{};            // RtAnimInterpolator + frames interpolados
alignas(16) inline std::array<uint8,0x20> anim{};               // RtAnimAnimation
alignas(16) inline std::array<uint8,0x40> info{};               // RtAnimInterpolatorInfo
alignas(16) inline std::array<uint8,0x400> frames{};            // keyframes de RtAnim
alignas(16) inline std::array<uint8,0x20> custom{};
alignas(16) inline std::array<uint8,0x20> hier{};               // CAnimBlendHierarchy falso
alignas(16) inline std::array<uint8,6*0xC> seqs{};              // CAnimBlendSequence x6
alignas(16) inline std::array<uint8,0x400> kf{};                // keyframes (64 bytes de margen delante)
alignas(16) inline std::array<uint8,4*0x30> streams{};          // CdStream x4
alignas(16) inline std::array<uint8,0x10> dir{};                // CDirectory
alignas(16) inline std::array<uint8,0x40+9*0x20> dirEntries{};  // entradas (2 de margen delante)
alignas(16) inline std::array<uint8,0x20> dirInfo{};
alignas(16) inline std::array<uint8,0x14> pool{};               // CPool<CEntryExit> lleno
alignas(16) inline std::array<uint8,8> poolFlags{};
alignas(16) inline std::array<uint8,8*0x3C> poolStore{};
alignas(16) inline std::array<uint8,0x30> carGroup{};           // CLoadedCarGroup (23 int16)
alignas(16) inline std::array<uint8,0x40> cp{};                 // CCheckpoint (0x38)
alignas(16) inline std::array<uint8,0x80> object{};             // tarea construida / destruida
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};        // entidad falsa (peatón)
alignas(16) inline std::array<uint8,sizeof(CAutomobile)> veh{}; // entidad falsa (vehículo)
alignas(16) inline std::array<uint8,0x60> assoc{};              // CAnimBlendAssociation falsa
alignas(16) inline std::array<uint8,sizeof(CPedGroup)> group{};
alignas(16) inline std::array<std::array<uint8,0x10>,4> dummyPeds{}; // punteros de peatón que no se desreferencian
alignas(16) inline std::array<uint8,0x40> intInfo{};

inline std::unordered_map<uintptr_t,int> freshIds;
inline json Name(const void* p) {
    const auto v=uintptr_t(p); if(!v)return nullptr;
    for(auto& [t,id]:freshIds)if(v>=t&&v<t+0x80)return "T"+std::to_string(id)+"+"+std::to_string(v-t);
    const std::pair<const void*,size_t> bufs[]{{object.data(),object.size()},{ped.data(),ped.size()},{veh.data(),veh.size()},{assoc.data(),assoc.size()},
        {group.data(),group.size()},{interp.data(),interp.size()},{anim.data(),anim.size()},{frames.data(),frames.size()},{custom.data(),custom.size()},
        {cp.data(),cp.size()},{intInfo.data(),intInfo.size()},{dummyPeds.data(),sizeof(dummyPeds)}};
    static const char* names[]{"object","ped","veh","assoc","group","interp","anim","frames","custom","cp","int_info","dummy"};
    for(size_t k=0;k<std::size(bufs);++k)if(v>=uintptr_t(bufs[k].first)&&v<uintptr_t(bufs[k].first)+bufs[k].second)return std::string(names[k])+"+"+std::to_string(v-uintptr_t(bufs[k].first));
    return "other"; // pila (el EXE y la DLL usan marcos distintos) u otra memoria
}

// --- dependencias simuladas ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline int __cdecl FakePrintf(const char*,...) { return 0; } // 0x821982: printf de depuración del EXE (la DLL usa su propio registro)
inline C3dMarker* __cdecl FakePlace(uint32 id,e3dMarkerType type,CVector& pos,float size,uint8 r,uint8 g,uint8 b,uint8 a,uint16 period,float frac,int16 rot,CVector dir,bool z){
    trace.push_back({"place_marker",id,int(type),Name(&pos),F(pos.x),F(pos.y),F(pos.z),F(size),r,g,b,a,period,F(frac),rot,F(dir.x),F(dir.y),F(dir.z),z}); return nullptr; }
inline uint32 stubMask{}, stubCalls{};
inline bool NextStub() { return (stubMask>>(stubCalls++%32))&1; }
inline bool __cdecl FakeSuppressed(int32 m) { trace.push_back({"suppressed",m}); return NextStub(); }
inline bool __cdecl FakeBlocked(int32 m) { trace.push_back({"blocked",m}); return NextStub(); }
inline bool __cdecl FakePhaseOut(int32 m) { trace.push_back({"phase_out",m}); return NextStub(); }
inline void __cdecl FakeInterp(void* out,void* in1,void* in2,float t,void* cd){
    trace.push_back({"interpolate",Name(out),Name(in1),Name(in2),F(t),Name(cd)});
    const int32 isz=*reinterpret_cast<const int32*>(interp.data()+0x24); if(isz>=12)*reinterpret_cast<uint32*>(static_cast<uint8*>(out)+8)=*static_cast<const uint32*>(in1)^(*static_cast<const uint32*>(in2)*3u); }

inline std::vector<bool> Occupancy() { auto* p=GetTaskPool(); std::vector<bool> r(p->GetSize()); for(size_t k=0;k<r.size();++k)r[k]=!p->IsFreeSlotAtIndex(k); return r; }
inline void CleanRefs() {
    auto clean=[](void* p){ auto* e=static_cast<CEntity*>(p); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); };
    clean(ped.data()); clean(veh.data());
}
inline json RefList(void* p) { json l=json::array(); for(auto* r=static_cast<CEntity*>(p)->m_pReferences;r;r=r->m_pNext)l.push_back(Name(r->m_ppEntity)); return l; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=21;
inline const std::array<Fn,NFNS> fns{{
    {"CGlass","CalcAlphaWithNormal",0x71ACF0,0,-1,1024},
    {"CTaskManager","GetTaskSecondary",0x681810,0,-1,512},
    {"CPopCycle","PickARandomGroupOfOtherPeds",0x610420,0,-1,1024},
    {"CCoverPoint","CanAccommodateAnotherPed",0x698E70,0,-1,256},
    {"CCover","AddCoverPoint",0x698F30,0,-1,1024},
    {"RtAnim","RtAnimInterpolatorSetCurrentAnim",0x7CD5A0,0,-1,512},
    {"CAnimBlendHierarchy","CalcTotalTime",0x4CF2F0,0,-1,512},
    {"CAnimBlendHierarchy","CalcTotalTimeCompressed",0x4CF3E0,0,-1,512},
    {"CdStream","CdStreamRead",0x406A20,0,-1,512},
    {"CDirectory","AddItem-",0x532310,0,-1,512},
    {"CEntryExitManager","AddOne",0x43FA00,0,-1,64},
    {"CLoadedCarGroup","PickRandomCar",0x611C50,0,-1,512},
    {"CCheckpoint","Render",0x725C00,0,-1,512},
    {"CTaskSimpleCarFallOut","Destructor",0x648DE0,0,-1,256},
    {"CTaskComplexFleeAnyMeans","Destructor",0x65CD10,0,-1,256},
    {"CTaskComplexPassObject","Destructor",0x661230,0,-1,256},
    {"CTaskComplexDestroyCarArmed","Constructor",0x621F50,0,-1,256},
    {"CTaskInteriorLieInBed","Constructor",0x675E20,0,-1,256},
    {"CPedGroupDefaultTaskAllocatorStandStill","AllocateDefaultTasks",0,0x86C76C,0,512},
    {"CPedGroupDefaultTaskAllocatorFollowLimited","AllocateDefaultTasks",0,0x86C764,0,512},
    {"CPedGroupDefaultTaskAllocatorFollowAnyMeans","AllocateDefaultTasks",0,0x86C75C,0,512},
}};
// instancias estáticas de los asignadores por defecto (0xC09908..0xC0991C) según su vtable
inline uintptr_t AllocatorInstance(uintptr_t vt) {
    switch(vt){ case 0x86C75C: return 0xC09908; case 0x86C764: return 0xC0990C; case 0x86C76C: return 0xC09910; default: return 0; }
}
// globales tocados (dirección, tamaño)
constexpr uintptr_t CAM_FWD=0xB6F9AC, POP_BEGIN=0xC0BC68, COVER_NUM=0xC197A4, COVER_PTS=0xC197C8, CD_STREAMS=0x8E3FFC, CD_INIT=0x8E3FE4,
                    CD_POSN=0x8E4898, CD_COUNT=0x8E4090, CD_QHEAD=0x8E3FF0, CD_QTAIL=0x8E3FF4, ENEX_POOL=0x96A7D8;
constexpr size_t POP_BYTES=0x10+sizeof(CPopCycle::m_nPercTypeGroup), COVER_BYTES=100*0x1C;
constexpr uint32 ANIMBLEND_ID=MAKECHUNKID(rwVENDORID_DEVELOPER,0xFB); // rwID_RPANIMBLENDPLUGIN

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"fixbugs-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"fixbugs-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fixbugs");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dirPath/"fixbugs-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // estado global guardado
    auto Raw=[](uintptr_t a,size_t n){ auto* p=reinterpret_cast<const uint8*>(a); return std::vector<uint8>(p,p+n); };
    auto Put=[](uintptr_t a,const std::vector<uint8>& v){ std::memcpy(reinterpret_cast<void*>(a),v.data(),v.size()); };
    auto Same=[](uintptr_t a,const std::vector<uint8>& v){ return std::memcmp(reinterpret_cast<const void*>(a),v.data(),v.size())==0; };
    const auto savedCam=Raw(CAM_FWD,12), savedPop=Raw(POP_BEGIN,POP_BYTES), savedCoverNum=Raw(COVER_NUM,4), savedCover=Raw(COVER_PTS,COVER_BYTES);
    const auto savedStreams=Raw(CD_STREAMS,4), savedInit=Raw(CD_INIT,4), savedPosn=Raw(CD_POSN,4), savedEnex=Raw(ENEX_POOL,4);
    // model infos de vehículo reales: clase (+0x4D), frecuencia (+0x52) y referencias (+8)
    std::vector<int32> vehIds; std::vector<std::array<uint8,5>> vehFields;
    for(int32 id=400;id<612;++id){ auto* mi=CModelInfo::ms_modelInfoPtrs[id]; if(!mi||mi->GetModelType()!=MODEL_INFO_VEHICLE)continue;
        auto* b=reinterpret_cast<uint8*>(mi); vehIds.push_back(id); vehFields.push_back({b[0x4D],b[0x52],b[0x53],b[8],b[9]}); }
    auto restoreVeh=[&]{ for(size_t k=0;k<vehIds.size();++k){ auto* b=reinterpret_cast<uint8*>(CModelInfo::ms_modelInfoPtrs[vehIds[k]]); b[0x4D]=vehFields[k][0]; b[0x52]=vehFields[k][1]; b[0x53]=vehFields[k][2]; b[8]=vehFields[k][3]; b[9]=vehFields[k][4]; } };
    auto vehIntact=[&]{ for(size_t k=0;k<vehIds.size();++k){ auto* b=reinterpret_cast<uint8*>(CModelInfo::ms_modelInfoPtrs[vehIds[k]]); if(b[0x4D]!=vehFields[k][0]||b[0x52]!=vehFields[k][1]||b[0x53]!=vehFields[k][2]||b[8]!=vehFields[k][3]||b[9]!=vehFields[k][4])return false; } return true; };
    auto restoreAll=[&]{ Put(CAM_FWD,savedCam); Put(POP_BEGIN,savedPop); Put(COVER_NUM,savedCoverNum); Put(COVER_PTS,savedCover);
        Put(CD_STREAMS,savedStreams); Put(CD_INIT,savedInit); Put(CD_POSN,savedPosn); Put(ENEX_POOL,savedEnex); restoreVeh(); };
    const auto occupancyStart=Occupancy(); auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch pf(0x821982,FakePrintf);
        EntryPatch k1(bit_cast<uintptr_t>(&C3dMarkers::PlaceMarker),FakePlace), k1e(0x725120,FakePlace);
        EntryPatch s1(bit_cast<uintptr_t>(&CTheScripts::HasCarModelBeenSuppressed),FakeSuppressed), s1e(0x46A810,FakeSuppressed);
        EntryPatch s2(bit_cast<uintptr_t>(&CTheScripts::HasVehicleModelBeenBlockedByScript),FakeBlocked), s2e(0x46A890,FakeBlocked);
        EntryPatch s3(bit_cast<uintptr_t>(&CStreaming::WeAreTryingToPhaseVehicleOut),FakePhaseOut), s3e(0x407F80,FakePhaseOut);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if(cls=="CdStream") {
                // el hilo de streaming solo lee gCdStreams tras una petición: se espera a que la cola y los canales estén libres
                bool idle=false;
                for(int w=0;w<5000&&!idle;++w){
                    const auto* real=*reinterpret_cast<uint8* const*>(CD_STREAMS); const int32 n=*reinterpret_cast<const int32*>(CD_COUNT);
                    idle=*reinterpret_cast<const int32*>(CD_QHEAD)==*reinterpret_cast<const int32*>(CD_QTAIL);
                    for(int32 k=0;k<n&&idle;++k)idle=*reinterpret_cast<const uint32*>(real+k*0x30+4)==0&&real[k*0x30+0xE]==0;
                    if(!idle)Sleep(1);
                }
                if(!idle){Write({{"event","error"},{"reason","streaming not idle"}});passed=false;break;}
            }
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](auto& buf){ for(auto& b:buf)b=uint8(next()); };
                auto W=[](void* base,size_t off)->uint32&{ return *reinterpret_cast<uint32*>(static_cast<uint8*>(base)+off); };
                auto Fl=[](void* base,size_t off)->float&{ return *reinterpret_cast<float*>(static_cast<uint8*>(base)+off); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xF1B5u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; stubCalls=0; stubMask=0;
                    freshIds.clear(); CleanRefs();
                    // el New del EXE busca desde m_LastFreeSlot+1: se restaura y los huecos libres se rellenan con 0xCD
                    { auto& lf=*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(GetTaskPool())+0xC); static const int32 lf0=lf; lf=lf0;
                      auto* pb=reinterpret_cast<uint8*>(GetTaskPool()); auto* stor=*reinterpret_cast<uint8**>(pb); auto* st=*reinterpret_cast<uint8**>(pb+4); const int32 cap=*reinterpret_cast<int32*>(pb+8);
                      for(int32 k=0;k<cap;++k)if(st[k]&0x80)std::memset(stor+k*0x80,0xCD,0x80); }
                    // entidades falsas
                    std::memset(ped.data(),0,ped.size()); reinterpret_cast<CEntity*>(ped.data())->SetType(ENTITY_TYPE_PED);
                    std::memset(veh.data(),0,veh.size()); reinterpret_cast<CEntity*>(veh.data())->SetType(ENTITY_TYPE_VEHICLE);
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CGlass") {
                        // dirección de la cámara: normalizada casi siempre; normales unitarias o no
                        CVector c{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; if(next()%4)c.Normalise();
                        CVector n{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; if(next()%4)n.Normalise(); if(next()%16==0)n=CVector{0.f,0.f,next()%2 ? 1.f : -1.f};
                        Fl(reinterpret_cast<void*>(CAM_FWD),0)=c.x; Fl(reinterpret_cast<void*>(CAM_FWD),4)=c.y; Fl(reinterpret_cast<void*>(CAM_FWD),8)=c.z;
                        ret=int(reinterpret_cast<uint8(__cdecl*)(const CVector&)>(a)(n));
                        Put(CAM_FWD,savedCam);
                    }
                    else if(cls=="CTaskManager") {
                        // índice -1 (TASK_SECONDARY_INVALID) y fuera de rango: el EXE lee la palabra que toque del gestor
                        fill(taskMgr); const int32 idx=next()%3==0 ? -1 : int32(next()%11);
                        ret=uint32(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(taskMgr.data(),idx))); out["idx"]=idx;
                    }
                    else if(cls=="CPopCycle") {
                        // tabla de porcentajes con sumas por encima y por debajo de 100 (el rango del sorteo es siempre 100)
                        auto* table=reinterpret_cast<uint8*>(POP_BEGIN+0x10); const bool low=next()%2;
                        for(size_t k=0;k<sizeof(CPopCycle::m_nPercTypeGroup);++k)table[k]=uint8(low ? next()%9 : next()%31);
                        fill(zone); zone[0xF]=uint8((zone[0xF]&0xE0)|(next()%20));
                        *reinterpret_cast<void**>(POP_BEGIN)=zone.data(); W(reinterpret_cast<void*>(POP_BEGIN),8)=next()%2; W(reinterpret_cast<void*>(POP_BEGIN),0xC)=next()%12;
                        ret=reinterpret_cast<int32(__cdecl*)()>(a)();
                        Put(POP_BEGIN,savedPop);
                    }
                    else if(cls=="CCoverPoint") {
                        fill(coverPt); coverPt[0]=uint8(next()%5);
                        W(coverPt.data(),0x14)=next()%2 ? 0 : next()|1; W(coverPt.data(),0x18)=next()%2 ? 0 : next()|1;
                        ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(coverPt.data());
                    }
                    else if(cls=="CCover") {
                        // puntos de los tipos 1..3 alrededor de una base: los de objeto/vehículo ahora cuentan por distancia
                        auto* pts=reinterpret_cast<uint8*>(COVER_PTS); const CVector base{uni(-100.f,100.f),uni(-100.f,100.f),uni(0.f,30.f)}; uint32 count=0;
                        const float spread=next()%2 ? 2.5f : 6.f;
                        for(int k=0;k<100;++k){ auto* p=pts+k*0x1C; for(int b=0;b<0x1C;++b)p[b]=uint8(next());
                            const int8 type=next()%3==0 ? int8(1+next()%3) : 0; p[0]=uint8(type); count+=type!=0;
                            Fl(p,4)=base.x+uni(-spread,spread); Fl(p,8)=base.y+uni(-spread,spread); Fl(p,0xC)=base.z+uni(-spread,spread);
                            W(p,0x10)=(type==1||type==2) ? (next()%2 ? 0 : 0x1000u+next()%4*0x100) : next(); }
                        *reinterpret_cast<uint32*>(COVER_NUM)=count;
                        const int32 type=int32(1+next()%3); CVector pos=base+CVector{uni(-spread,spread),uni(-spread,spread),uni(-spread,spread)};
                        if(next()%6==0){ const int k=int(next()%100); pos=CVector{Fl(pts+k*0x1C,4),Fl(pts+k*0x1C,8),Fl(pts+k*0x1C,0xC)}; }
                        const int32 usage=int32(next()%4); const uint32 dirB=next()%256;
                        // el EXE (0x698F30) es void: eax al salir es basura, así que solo se comparan los puntos y el contador
                        reinterpret_cast<void(__cdecl*)(int32,void*,const CVector*,int32,uint32)>(a)(type,nullptr,&pos,usage,dirB);
                        out["points"]=ReversingProbe::Bytes(pts,COVER_BYTES); out["num"]=*reinterpret_cast<uint32*>(COVER_NUM);
                        Put(COVER_NUM,savedCoverNum); Put(COVER_PTS,savedCover);
                    }
                    else if(cls=="RtAnim") {
                        // casi siempre animaciones de RpAnimBlend: el EXE (y ahora la DLL) también reescribe sus cabeceras
                        fill(interp); fill(anim); fill(info); fill(frames); fill(custom);
                        static const uint32 typeIds[]{ANIMBLEND_ID,ANIMBLEND_ID,ANIMBLEND_ID,1,0x1234};
                        const int32 isz=int32(8+4*(next()%15)), asz=int32(8+4*(next()%15));
                        W(info.data(),0)=typeIds[next()%std::size(typeIds)]; W(info.data(),4)=uint32(isz); W(info.data(),8)=uint32(asz);
                        W(info.data(),0xC)=0x1000+next()%0x100; W(info.data(),0x10)=0x2000+next()%0x100;
                        *reinterpret_cast<void**>(info.data()+0x14)=reinterpret_cast<void*>(&FakeInterp); W(info.data(),0x18)=0x3000+next()%0x100;
                        const int32 nodes=next()%12==0 ? -int32(next()%3) : int32(next()%9);
                        W(interp.data(),0x2C)=uint32(nodes); W(interp.data(),0x20)=0x40;
                        *reinterpret_cast<void**>(anim.data())=info.data(); W(anim.data(),4)=uint32(std::max(nodes,0)*2);
                        *reinterpret_cast<void**>(anim.data()+0x10)=frames.data(); *reinterpret_cast<void**>(anim.data()+0x14)=next()%4 ? custom.data() : nullptr;
                        ret=reinterpret_cast<RwBool(__cdecl*)(void*,void*)>(a)(interp.data(),anim.data());
                        out["interp"]=ReversingProbe::Bytes(interp.data(),interp.size()); out["anim"]=Fnv(anim.data(),anim.size()); out["frames"]=Fnv(frames.data(),frames.size());
                    }
                    else if(cls=="CAnimBlendHierarchy") {
                        // secuencias sin keyframes: se lee el keyframe -1 (dentro del margen de kf) como el EXE
                        const bool compressed=nm=="CalcTotalTimeCompressed";
                        if(compressed){ for(size_t k=0;k<kf.size();k+=2)*reinterpret_cast<int16*>(kf.data()+k)=int16(int32(next()%4001)-2000); }
                        else { for(size_t k=0;k<kf.size();k+=4)Fl(kf.data(),k)=uni(0.f,8.f); }
                        fill(hier); fill(seqs); const int16 count=int16(next()%8==0 ? 0 : 1+next()%6);
                        *reinterpret_cast<void**>(hier.data()+4)=seqs.data(); *reinterpret_cast<int16*>(hier.data()+8)=count; Fl(hier.data(),0x10)=uni(-1.f,9.f);
                        for(int j=0;j<6;++j){ auto* s=seqs.data()+j*0xC; *reinterpret_cast<int16*>(s+6)=int16(next()%3==0 ? 0 : 1+next()%5);
                            *reinterpret_cast<void**>(s+8)=kf.data()+0x40+j*0xA0; }
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(hier.data());
                        out["hier"]=ReversingProbe::Bytes(hier.data(),hier.size()); out["seqs"]=Fnv(seqs.data(),seqs.size()); out["kf"]=Fnv(kf.data(),kf.size());
                    }
                    else if(cls=="CdStream") {
                        // canales ocupados: la función solo actualiza gLastCdStreamPosn (posición con FileID) y hFile y sale
                        fill(streams);
                        for(int k=0;k<4;++k){ auto* s=streams.data()+k*0x30; if(next()%2){ W(s,4)=1+next()%100; } else { W(s,4)=0; s[0xE]=uint8(1+next()%255); } }
                        *reinterpret_cast<void**>(CD_STREAMS)=streams.data(); W(reinterpret_cast<void*>(CD_INIT),0)=1;
                        const int32 id=int32(next()%4); const uint32 rawPos=(next()%4==0 ? 0u : (next()%8)<<24)|(next()&0xFFFFFF); const int32 cnt=int32(next()%3==0 ? next()%0x10000 : 1+next()%64);
                        CdStreamPos pos; std::memcpy(&pos,&rawPos,4); SetLastError(0x1234);
                        ret=reinterpret_cast<bool(__cdecl*)(int32,void*,CdStreamPos,int32)>(a)(id,frames.data(),pos,cnt);
                        out["last_error"]=uint32(GetLastError()); out["posn"]=*reinterpret_cast<uint32*>(CD_POSN); out["streams"]=ReversingProbe::Bytes(streams.data(),streams.size());
                        Put(CD_STREAMS,savedStreams); Put(CD_INIT,savedInit); Put(CD_POSN,savedPosn);
                    }
                    else if(cls=="CDirectory") {
                        // entradas repetidas: el EXE las añade igualmente
                        fill(dirEntries); fill(dir);
                        const int32 cap=next()%8==0 ? -1 : int32(next()%9);
                        const int32 num=next()%6==0 ? -int32(1+next()%2) : int32(next()%(uint32(std::max(cap,0))+1));
                        *reinterpret_cast<void**>(dir.data())=dirEntries.data()+0x40; W(dir.data(),4)=uint32(cap); W(dir.data(),8)=uint32(num);
                        if(next()%2&&cap>0){ const int k=int(next()%uint32(cap)); std::memcpy(dirInfo.data(),dirEntries.data()+0x40+k*0x20,0x20); } else fill(dirInfo);
                        reinterpret_cast<void(__thiscall*)(void*,const void*)>(a)(dir.data(),dirInfo.data());
                        out["dir"]=ReversingProbe::Bytes(dir.data(),dir.size()); out["entries"]=ReversingProbe::Bytes(dirEntries.data(),dirEntries.size());
                    }
                    else if(cls=="CEntryExitManager") {
                        // pool lleno (bit 7 a cero en todos los huecos): el EXE devuelve 0
                        const int32 size=int32(1+next()%8); for(auto& b:poolFlags)b=uint8(next()&0x7F); fill(poolStore); fill(pool);
                        *reinterpret_cast<void**>(pool.data())=poolStore.data(); *reinterpret_cast<void**>(pool.data()+4)=poolFlags.data();
                        W(pool.data(),8)=uint32(size); W(pool.data(),0xC)=uint32(int32(next()%uint32(size+1))-1);
                        *reinterpret_cast<void**>(ENEX_POOL)=pool.data();
                        static const char name[]="FIXBUGS";
                        ret=reinterpret_cast<int32(__cdecl*)(float,float,float,float,float,float,float,float,float,float,float,int32,int32,int32,int32,int32,int32,const char*)>(a)(
                            uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f),uni(-180.f,180.f),uni(0.f,5.f),uni(0.f,5.f),uni(0.f,5.f),
                            uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f),uni(-180.f,180.f),int32(next()%18),int32(next()&0xFFFF),int32(next()%4),int32(next()%24),int32(next()%24),int32(next()%5),name);
                        out["pool"]=ReversingProbe::Bytes(pool.data()+8,0xC); out["flags"]=ReversingProbe::Bytes(poolFlags.data(),poolFlags.size()); out["store"]=Fnv(poolStore.data(),poolStore.size());
                        Put(ENEX_POOL,savedEnex);
                    }
                    else if(cls=="CLoadedCarGroup") {
                        // pesos totales nulos (el EXE devuelve -1 sin sortear) y pesos normales
                        std::fill_n(reinterpret_cast<int16*>(carGroup.data()),carGroup.size()/2,int16(-1));
                        const uint32 n=next()%24; const bool zeroSum=next()%3==0;
                        static const int8 classes[]{-1,0,1,2,3,4,5,6,7,8,9,10};
                        for(uint32 k=0;k<n;++k){ const int32 id=vehIds[next()%vehIds.size()]; reinterpret_cast<int16*>(carGroup.data())[k]=int16(id);
                            auto* b=reinterpret_cast<uint8*>(CModelInfo::ms_modelInfoPtrs[id]); b[0x4D]=uint8(classes[next()%std::size(classes)]);
                            *reinterpret_cast<uint16*>(b+0x52)=uint16(zeroSum||next()%6==0 ? 0 : 1+next()%20); *reinterpret_cast<uint16*>(b+8)=uint16(next()%5); }
                        stubMask=next()&next();
                        ret=reinterpret_cast<int32(__thiscall*)(void*,bool,bool)>(a)(carGroup.data(),next()%2,next()%2);
                        restoreVeh();
                    }
                    else if(cls=="CCheckpoint") {
                        // toro TORUSTHROUGH con alfa < 48: el EXE dibuja los cuatro con el alfa desbordado en un byte
                        fill(cp); *reinterpret_cast<uint16*>(cp.data())=6; cp[0xB]=uint8(next()%2 ? next()%48 : next()%256);
                        for(int k=0;k<3;++k){ Fl(cp.data(),0x10+4*k)=uni(-3000.f,3000.f); Fl(cp.data(),0x1C+4*k)=next()%10==0 ? 0.f : uni(-1.f,1.f); }
                        Fl(cp.data(),0x28)=uni(0.f,1.f); Fl(cp.data(),0x2C)=next()%10==0 ? 0.f : uni(0.1f,12.f); Fl(cp.data(),0x30)=uni(0.f,500.f); Fl(cp.data(),0x34)=uni(-1.f,1.f);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(cp.data());
                        out["cp"]=ReversingProbe::Bytes(cp.data(),0x38);
                    }
                    else if(nm=="Destructor") {
                        // referencias que el EXE no limpia (vehículo/entidad a pasar) y animación cuyo callback no se toca
                        std::memset(object.data(),0,object.size()); auto* o=object.data(); size_t size=0;
                        auto Reg=[&](void* ent,size_t off){ *reinterpret_cast<void**>(o+off)=ent; if(ent)static_cast<CEntity*>(ent)->RegisterReference(reinterpret_cast<CEntity**>(o+off)); };
                        if(cls=="CTaskSimpleCarFallOut"){ size=0x1C; W(o,0)=0x86EFD0; o[8]=uint8(next()%2); fill(assoc);
                            *reinterpret_cast<void**>(o+0xC)=next()%4 ? assoc.data() : nullptr; Reg(next()%4 ? veh.data() : nullptr,0x10);
                            W(o,0x14)=next()%4; W(o,0x18)=next()%2 ? 0 : 0x5000+next()%0x100; }
                        else if(cls=="CTaskComplexFleeAnyMeans"){ size=0x54; for(size_t k=0x10;k<0x50;++k)o[k]=uint8(next()); W(o,0)=0x86F7F0;
                            Reg(next()%3 ? ped.data() : nullptr,0xC); Reg(next()%4 ? veh.data() : nullptr,0x50); }
                        else { size=0x30; for(size_t k=0x10;k<0x2C;++k)o[k]=uint8(next()); W(o,0)=0x86FB18;
                            Reg(next()%3 ? ped.data() : nullptr,0xC); Reg(next()%4 ? veh.data() : nullptr,0x2C); }
                        const auto assocBefore=assoc;
                        reinterpret_cast<void*(__thiscall*)(void*)>(a)(o);
                        out["object"]=ReversingProbe::Bytes(o+4,size-4); out["assoc_intact"]=assoc==assocBefore;
                        out["refs"]=json::array({RefList(ped.data()),RefList(veh.data())});
                    }
                    else if(nm=="Constructor") {
                        // objeto con 0xCD: DestroyCarArmed registra la dirección de su parámetro en la pila; LieInBed deja sin escribir m_bRghtHandSide
                        std::memset(object.data(),0xCD,object.size()); auto* o=object.data(); size_t size=0;
                        if(cls=="CTaskComplexDestroyCarArmed"){ size=0x64; void* v=next()%5 ? veh.data() : nullptr;
                            reinterpret_cast<void*(__thiscall*)(void*,void*,uint32,uint32,uint32)>(a)(o,v,next(),next(),next()); }
                        else { size=0x30; fill(intInfo); /* CTaskInteriorLieInBed: el constructor escribe hasta 0x2C */
                            reinterpret_cast<void*(__thiscall*)(void*,int32,void*,uint32,uint32)>(a)(o,int32(next()%20000)-1000,next()%4 ? intInfo.data() : nullptr,next()%2,next()%2); }
                        out["object"]=ReversingProbe::Bytes(o+4,size-4); out["type"]=int(reinterpret_cast<CTask*>(o)->GetTaskType());
                        out["refs"]=json::array({RefList(ped.data()),RefList(veh.data())});
                    }
                    else { // asignadores por defecto del grupo: pares con el mismo peatón repetido
                        std::memset(group.data(),0,group.size()); auto* g=group.data();
                        auto* pairs=reinterpret_cast<CPedTaskPair*>(g+0x21C);
                        *reinterpret_cast<void**>(g+0xC+7*4)=next()%4==0 ? ped.data() : nullptr; // líder (miembro 7)
                        for(int k=0;k<8;++k){ const uint32 r=next()%6; pairs[k].Ped=r<4 ? reinterpret_cast<CPed*>(dummyPeds[r].data()) : nullptr; pairs[k].Task=nullptr; }
                        CPed* argPed=next()%3==0 ? nullptr : reinterpret_cast<CPed*>(dummyPeds[next()%4].data());
                        const auto occ=Occupancy();
                        reinterpret_cast<void(__thiscall*)(uintptr_t,CPedGroup*,CPed*)>(a)(AllocatorInstance(fn.vt),reinterpret_cast<CPedGroup*>(g),argPed);
                        for(auto* h:hooks)h->State(false);
                        auto* tp=GetTaskPool(); std::vector<CTask*> fresh;
                        for(size_t k=0;k<occ.size();++k)if(!occ[k]&&!tp->IsFreeSlotAtIndex(k)){fresh.push_back(tp->GetAt(k));freshIds[uintptr_t(tp->GetAt(k))]=int(fresh.size()-1);}
                        json ps=json::array(); for(int k=0;k<8;++k)ps.push_back({Name(pairs[k].Ped),Name(pairs[k].Task),pairs[k].Task ? json(int(pairs[k].Task->GetTaskType())) : json(nullptr)});
                        out["pairs"]=ps; out["arg"]=Name(argPed); out["refs"]=RefList(ped.data()); ret=int(fresh.size());
                        for(auto* t:fresh)delete t;
                        out["clean"]=Occupancy()==occ; freshIds.clear();
                    }
                    for(auto* h:hooks)h->State(false);
                    CleanRefs(); restoreAll();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    CleanRefs(); restoreAll();
    const bool restored=Same(CAM_FWD,savedCam)&&Same(POP_BEGIN,savedPop)&&Same(COVER_NUM,savedCoverNum)&&Same(COVER_PTS,savedCover)&&Same(CD_STREAMS,savedStreams)
        &&Same(CD_INIT,savedInit)&&Same(CD_POSN,savedPosn)&&Same(ENEX_POOL,savedEnex)&&vehIntact()&&Occupancy()==occupancyStart&&freeRefs()==savedFree;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
