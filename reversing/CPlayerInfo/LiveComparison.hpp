#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Cranes.h"
#include "CarEnterExit.h"
#include "PedGeometryAnalyser.h"
#include "CullZones.h"
#include "TaskSimpleSwim.h"
#include "TaskSimpleJetPack.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace PlayerInfoLive {
inline bool done{}; inline std::ofstream log; inline int seed{}; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline int Report(int type,char* msg,int*) { if(type!=_CRT_ASSERT)return FALSE; Write({{"event","assertion"},{"message",msg ? msg:""}}); ExitProcess(91); }
template<class T> struct Raw {
    alignas(T) std::array<uint8,sizeof(T)> data{};
    T* get() {return reinterpret_cast<T*>(data.data());}
};
template<class F> uintptr_t Address(F f) { return std::bit_cast<uintptr_t>(f); }
struct Patch {
    ReversingProbe::EntryPatch dll,exe;
    template<class F,class G> Patch(F f,uintptr_t at,G stub):dll(Address(f),stub),exe(at,stub){}
};
inline void __cdecl ClearScan() { trace.push_back({"clear_scans"}); }
inline void __fastcall Bound(const CEntity* e,void*,CVector& out) { trace.push_back({"bound",e->m_nModelIndex});out=CVector{1,2,3}; }
inline float __fastcall Height(CVehicle* e,void*) { trace.push_back({"height",e->m_nModelIndex});return float(seed%4)*0.5f; }
inline bool __cdecl Door(const CPed* ped,const CVehicle* car,CVector& pos,int32& door) { trace.push_back({"door",car->m_nModelIndex,door});pos=car->GetPosition();pos.z+=float(seed%5);door=10;return (seed+car->m_nModelIndex)%4!=0; }
inline bool __cdecl Inside(const CPed& ped,const CVector& pos,CEntity& car) { trace.push_back({"inside",car.m_nModelIndex,pos.x,pos.y,pos.z});return (seed+car.m_nModelIndex)%2!=0; }
inline bool __cdecl Crane(CVehicle* car) { trace.push_back({"crane",car->m_nModelIndex});return (seed+car->m_nModelIndex)%7==0; }
inline bool Equal(const json& a,const json& b) {
    if(a.is_number_float()||b.is_number_float())return a.is_number()&&b.is_number()&&std::abs(a.get<double>()-b.get<double>())<=0.00001;
    if(a.type()!=b.type()||a.size()!=b.size())return false;
    if(a.is_object()){for(auto it=a.begin();it!=a.end();++it)if(!b.contains(it.key())||!Equal(it.value(),b[it.key()]))return false;return true;}
    if(a.is_array()){for(size_t n=0;n<a.size();++n)if(!Equal(a[n],b[n]))return false;return true;}
    return a==b;
}
inline void SetMatrix(CPlaceable* p,CMatrix* m) { *reinterpret_cast<CMatrix**>(reinterpret_cast<uint8*>(p)+0x14)=m; }
inline void SetIntelligence(CPed* p,CPedIntelligence* i) { *reinterpret_cast<CPedIntelligence**>(reinterpret_cast<uint8*>(p)+0x47C)=i; }
inline bool SearchTests(std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks,int& pair) {
    auto savedSectors=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB992B8),3072);
    const auto savedScan=CWorld::ms_nCurrentScanCode;
    Raw<CPlayerInfo> info;Raw<CPlayerPed> ped;CMatrix matrix;matrix.SetUnity();SetMatrix(ped.get(),&matrix);
    std::array<Raw<CObject>,16> objects;
    std::array<Raw<CAutomobile>,16> cars;
    std::array<CMatrix,16> carMatrices;
    std::array<Raw<CPtrNodeDoubleLink<CObject*>>,16> objectNodes;
    std::array<Raw<CPtrNodeDoubleLink<CVehicle*>>,16> carNodes;
    Raw<CPtrListDoubleLink<CVehicle*>> list;
    std::array<uintptr_t,64> vtable{};vtable[0xD4/4]=Address(Height);
    bool passed=true;
    struct WorldAccess : CWorld { using CWorld::ClearScanCodes; };
    Patch clear(WorldAccess::ClearScanCodes,0x563470,ClearScan);
    Patch bound(static_cast<void(CEntity::*)(CVector&)const>(&CEntity::GetBoundCentre),0x534250,Bound);
    Patch door(CCarEnterExit::GetNearestCarDoor,0x6528F0,Door);
    Patch inside(CPedGeometryAnalyser::LiesInsideBoundingBox,0x5F3880,Inside);
    Patch crane(CCranes::IsThisCarBeingCarriedByAnyCrane,0x6F3FF0,Crane);
    for(int fn=0;fn<2&&passed;++fn) {
        auto* hook=hooks[fn];bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
        for(int i=0;i<4096&&passed;++i) {
            seed=i;matrix.SetUnity();matrix.GetPosition()=CVector{float((i%17)-8)*50,float((i%19)-9)*50,float(i%3)};
            if(i&256)matrix.GetPosition().x=(i&512)?2999.0f:-2999.0f;
            if(i&1024){matrix.GetForward()=CVector{1,0,0};matrix.GetRight()=CVector{0,-1,0};}
            const auto centre=matrix.GetPosition()+(matrix.GetForward()*0.5f)*3.0f;
            const auto setup=[&] {
                trace=json::array();std::memset(reinterpret_cast<void*>(0xB992B8),0,3072);
                CWorld::ms_nCurrentScanCode=(i%31==0)?65535:uint16(i+2);
                *reinterpret_cast<void**>(list.data.data())=i%17?carNodes[0].get():nullptr;
                for(int n=0;n<16;++n) {
                    objects[n].data.fill(0);cars[n].data.fill(0);
                    auto* obj=objects[n].get();obj->GetPosition()=centre+CVector{float((n+i)%7-3)*0.5f,float((n*3+i)%9-4)*0.5f,float((n+i)%3)*0.5f};
                    obj->objectFlags.bIsLiftable=(n+i)%3!=0;obj->SetScanCode((n+i)%4==0?uint16(CWorld::ms_nCurrentScanCode==65535?1:CWorld::ms_nCurrentScanCode+1):0);
                    auto* on=objectNodes[n].get();on->Item=obj;on->Next=n+1<i%17?objectNodes[n+1].get():nullptr;on->Prev=n?objectNodes[n-1].get():nullptr;
                    auto* car=cars[n].get();*reinterpret_cast<uintptr_t**>(car)=vtable.data();car->SetType(ENTITY_TYPE_VEHICLE);car->SetStatus(eEntityStatus((i+n)%9));
                    car->m_bUsesCollision=(i+n)%5!=0;car->SetScanCode((i+n)%6==0?CWorld::ms_nCurrentScanCode:0);
                    car->m_nVehicleType=eVehicleType((i+n)%3==0?9:(i+n)%3==1?5:0);
                    car->m_nModelIndex=(i&16)?MODEL_AT400:eModelID(400+n);
                    car->vehicleFlags.bConsideredByPlayer=(i+n)%7!=0;
                    carMatrices[n].SetUnity();carMatrices[n].GetUp().z=float((i+n)%5)*0.2f;
                    carMatrices[n].GetPosition()=matrix.GetPosition()+CVector{float(n-7)*1.5f,float((i+n)%4),float((i+n)%9-4)};SetMatrix(car,&carMatrices[n]);
                    auto* cn=carNodes[n].get();cn->Item=car;cn->Next=n+1<i%17?carNodes[n+1].get():nullptr;cn->Prev=n?carNodes[n-1].get():nullptr;
                }
                ped.get()->m_pContactEntity=(i&32)?cars[i%16].get():nullptr;
                // Same list may be visited in adjoining sectors; original object search does not mark scans.
                for(int y=0;y<16;++y)for(int x=0;x<16;++x)*reinterpret_cast<void**>(&CWorld::GetRepeatSector(x,y).Objects)=i%17?objectNodes[0].get():nullptr;
            };
            auto run=[&] {
                setup();auto inputs=ped.data;auto beforeInfo=info.data;
                json out;
                if(fn==0) {
                    auto* result=reinterpret_cast<CEntity*(__thiscall*)(CPlayerInfo*,CPed*)>(0x56DBD0)(info.get(),ped.get());
                    int selected=-1;for(int n=0;n<16;++n)if(result==objects[n].get())selected=n;
                    out={{"selected",selected},{"scan",CWorld::ms_nCurrentScanCode}};
                } else {
                    float distance=float(i%5);CVehicle* result=nullptr;
                    reinterpret_cast<void(__thiscall*)(CPlayerInfo*,CPtrListDoubleLink<CVehicle*>&,CPed*,float,float,float,float,float*,CVehicle**)>(0x56F4E0)(info.get(),*list.get(),ped.get(),-1,-2,3,4,&distance,&result);
                    int selected=-1;json scans=json::array();for(int n=0;n<16;++n){if(result==cars[n].get())selected=n;scans.push_back(cars[n].get()->GetScanCode());}
                    out={{"selected",selected},{"distance",distance},{"scans",scans}};
                }
                out["trace"]=trace;out["inputs_restored"]=inputs==ped.data&&beforeInfo==info.data;return out;
            };
            hook->State(false);auto start=PlayerInfoProbe::calls;auto original=run();auto middle=PlayerInfoProbe::calls;
            hook->State(true);auto candidate=run();auto end=PlayerInfoProbe::calls;
            passed=Equal(original,candidate)&&original["inputs_restored"]==true&&candidate["inputs_restored"]==true&&start==middle&&end[fn]==middle[fn]+1;
            Write({{"event","comparison"},{"pair",pair++},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",middle[fn]-start[fn]},{"candidate_calls",end[fn]-middle[fn]}});
        }
        hook->State(enabled);hook->LockState(locked);
    }
    std::memcpy(reinterpret_cast<void*>(0xB992B8),savedSectors.data(),savedSectors.size());CWorld::ms_nCurrentScanCode=savedScan;
    return passed && ReversingProbe::Bytes(reinterpret_cast<void*>(0xB992B8),3072)==savedSectors && CWorld::ms_nCurrentScanCode==savedScan;
}
}
#include "ProcessComparison.hpp"
namespace PlayerInfoLive {
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"playerinfo-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"playerinfo-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    const std::array names{"FindObjectToSteal","FindClosestCarSectorList","Process"};
    std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPlayerInfo")for(auto& h:c.Items())for(int n=0;n<3;++n)if(h->Name()==names[n])hooks[n]=h.get();});
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"}});_CrtSetReportHook(report);return;}
    auto before=ReversingProbe::Bytes(player,sizeof(CPlayerPed));int pair=0;
    bool passed=SearchTests(hooks,pair);
    if(passed)passed=ProcessTests(hooks[2],pair);
    const bool restored=before==ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored}});_CrtSetReportHook(report);
}
}
