#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace PFLive {
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline int Report(int t,char* s,int*){if(t!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",s?s:""}});log.flush();ExitProcess(91);}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
struct Saved {void* p;std::vector<uint8> bytes;template<class T>Saved(T& v):p(&v),bytes(ReversingProbe::Bytes(&v,sizeof(v))){}void Restore(){std::memcpy(p,bytes.data(),bytes.size());}bool Same(){return bytes==ReversingProbe::Bytes(p,bytes.size());}~Saved(){Restore();}};
inline uint32 seed{},rngCalls{};
inline int Random(){++rngCalls;seed=seed*1664525u+1013904223u;return int((seed>>8)&32767);}
inline json trace;inline bool allocationFails{};alignas(16) inline std::array<uint8,0xA40> vehicleBytes;
inline void* Alloc(unsigned n){trace.push_back({"alloc",n});return allocationFails?nullptr:vehicleBytes.data();}
template<int N>inline void* __fastcall Ctor(void* self,void*,int32 model,eVehicleCreatedBy by){trace.push_back({"ctor",N,self==vehicleBytes.data(),model,int(by)});return self;}
inline void* __fastcall AutoCtor(void* self,void*,int32 model,eVehicleCreatedBy by,bool suspension){trace.push_back({"ctor",0,self==vehicleBytes.data(),model,int(by),suspension});return self;}
inline DWORD exceptionCode{};inline uintptr_t exceptionAccess{};
inline int Capture(EXCEPTION_POINTERS* p){exceptionCode=p->ExceptionRecord->ExceptionCode;exceptionAccess=p->ExceptionRecord->NumberParameters>=2?p->ExceptionRecord->ExceptionInformation[1]:0;return EXCEPTION_EXECUTE_HANDLER;}
__declspec(noinline) inline CVehicle* Factory(int model,uint8 by){__try{return reinterpret_cast<CVehicle*(__cdecl*)(int,uint8)>(0x421440)(model,by);}__except(Capture(GetExceptionInformation())){return nullptr;}}
inline uintptr_t LocalAddress(HMODULE module,const char* name){auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name));if(p&&*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1);return p;}
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"population-factory-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"population-factory-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;auto report=_CrtSetReportHook(Report);
    using Hook=ReversibleHooks::ReversibleHook::Base;
    std::array<Hook*,3> hooks{};const std::array classes{"CPopCycle","CCarCtrl","CTaskAllocatorKillThreatsBasic"};const std::array names{"PickPedMIToStreamInForCurrentZone","GetNewVehicleDependingOnCarModel","ComputeClosestPeds"};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())for(int n=0;n<3;++n)if(c.Name()==classes[n]&&h->Name()==names[n])hooks[n]=h.get();});
    if(std::ranges::any_of(hooks,[](auto* h){return !h;})){Write({{"event","error"},{"reason","missing hook"}});log.flush();_CrtSetReportHook(report);return;}
    auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));const auto fpBefore=_controlfp(0,0);
    std::vector<Saved> saved;
    saved.emplace_back(CPopCycle::m_pCurrZoneInfo);saved.emplace_back(CPopCycle::m_nCurrentTimeIndex);saved.emplace_back(CPopCycle::m_nCurrentTimeOfWeek);saved.emplace_back(CPopCycle::m_nPercTypeGroup);
    saved.emplace_back(CPopulation::CurrentWorldZone);saved.emplace_back(CPopulation::m_TranslationArray);saved.emplace_back(CPopulation::m_nNumPedsInGroup);saved.emplace_back(CPopulation::m_PedGroups);
    saved.emplace_back(CStreaming::ms_NextPedToLoadFromGroup);saved.emplace_back(CStreaming::ms_pedsLoaded);saved.emplace_back(CModelInfo::ms_modelInfoPtrs);
    // No vector growth after this point: Saved owns restoration of its original region.
    bool passed=true;int pairs=0;const auto startup=PFProbe::calls;
    {
    ReversingProbe::EntryPatch rn(0x821B1E,Random),rl(reinterpret_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
    HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
    const std::array<uintptr_t,10> native{0x6B0A90,0x6C8D60,0x6CE370,0x6C4190,0x6C8E20,0x6F2940,0x6F6030,0x6BF430,0x6BF820,0x6D03A0};
    const std::array<const char*,10> symbols{"??0CAutomobile@@QAE@HW4eVehicleCreatedBy@@_N@Z","??0CMonsterTruck@@QAE@HW4eVehicleCreatedBy@@@Z","??0CQuadBike@@QAE@HW4eVehicleCreatedBy@@@Z","??0CHeli@@QAE@HW4eVehicleCreatedBy@@@Z","??0CPlane@@QAE@HW4eVehicleCreatedBy@@@Z","??0CBoat@@QAE@HW4eVehicleCreatedBy@@@Z","??0CTrain@@QAE@HW4eVehicleCreatedBy@@@Z","??0CBike@@QAE@HW4eVehicleCreatedBy@@@Z","??0CBmx@@QAE@HW4eVehicleCreatedBy@@@Z","??0CTrailer@@QAE@HW4eVehicleCreatedBy@@@Z"};
    const std::array<uintptr_t,10> stubs{reinterpret_cast<uintptr_t>(AutoCtor),reinterpret_cast<uintptr_t>(Ctor<1>),reinterpret_cast<uintptr_t>(Ctor<2>),reinterpret_cast<uintptr_t>(Ctor<3>),reinterpret_cast<uintptr_t>(Ctor<4>),reinterpret_cast<uintptr_t>(Ctor<5>),reinterpret_cast<uintptr_t>(Ctor<6>),reinterpret_cast<uintptr_t>(Ctor<7>),reinterpret_cast<uintptr_t>(Ctor<8>),reinterpret_cast<uintptr_t>(Ctor<9>)};
    for(int j=0;j<10;++j){auto address=LocalAddress(module,symbols[j]);if(!address){Write({{"event","error"},{"reason",symbols[j]}});log.flush();_CrtSetReportHook(report);return;}patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(address,reinterpret_cast<void*>(stubs[j])));patches.emplace_back(std::make_unique<ReversingProbe::EntryPatch>(native[j],reinterpret_cast<void*>(stubs[j])));}
    ReversingProbe::EntryPatch an(0x6E2D50,Alloc),al(reinterpret_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CVehicle::operator new)),Alloc);
    Raw<CVehicleModelInfo> vehicleInfo;std::memcpy(vehicleInfo.b.data(),CModelInfo::ms_modelInfoPtrs[400],vehicleInfo.b.size());
    std::array<Raw<CPedModelInfo>,21> models;for(auto& m:models)std::memcpy(m.b.data(),CModelInfo::ms_modelInfoPtrs[7],m.b.size());
    CZoneInfo zone;
    Raw<CPedGroup> group1,group2;std::array<Raw<CPed>,16> peds;std::array<Raw<CMatrixLink>,16> matrices;
    for(int fn=0;fn<3&&passed;++fn){
        auto* h=hooks[fn];bool enabled=h->Hooked(),locked=h->Locked();h->LockState(false);Write({{"event","hook_state"},{"function",fn},{"enabled",enabled},{"locked",locked},{"startup_calls",startup[fn]}});
        const int count=fn==0?8192:fn==1?2048:8192;
        for(int i=0;i<count&&passed;++i){
            auto run=[&]()->json{
                seed=uint32(i*7919);rngCalls=0;trace=json::array();
                if(fn==0){
                    CPopCycle::m_pCurrZoneInfo=&zone;zone.PopType=i%20;zone.PopRaces=(i/20)%16;CPopCycle::m_nCurrentTimeIndex=(i/320)%12;CPopCycle::m_nCurrentTimeOfWeek=(i/3840)%2;CPopulation::CurrentWorldZone=i%3;
                    auto& percentages=CPopCycle::m_nPercTypeGroup[CPopCycle::m_nCurrentTimeIndex][CPopCycle::m_nCurrentTimeOfWeek][zone.PopType];
                    std::fill_n(percentages,18,uint8(0));if(i%3==0)percentages[(i/3)%18]=100;else for(int j=0;j<18;++j)percentages[j]=uint8(j<10?6:5);
                    for(int g=0;g<18;++g){for(int w=0;w<3;++w)CPopulation::m_TranslationArray[g].pedGroupIds[w]=ePopcyclePedGroup(g*3+w);CStreaming::ms_NextPedToLoadFromGroup[g]=(i/7+g)%21;}
                    for(int g=0;g<54;++g){CPopulation::m_nNumPedsInGroup[g]=std::array{0,1,8,21}[(g+i/11)%4];for(int k=0;k<21;++k)CPopulation::m_PedGroups[g][k]=int16(7+(g*3+k)%21);}
                    for(int k=0;k<21;++k){models[k].get()->m_nRace=uint8((k+i/17)%5);CModelInfo::ms_modelInfoPtrs[k+7]=models[k].get();}
                    for(int k=0;k<8;++k)CStreaming::ms_pedsLoaded[k]=eModelID(i%4==0?7+k:MODEL_INVALID);
                    int result=int(reinterpret_cast<eModelID(__cdecl*)()>(0x60FFD0)());
                    return {{"result",result},{"rng_calls",rngCalls},{"rng_state",seed},{"indices",CStreaming::ms_NextPedToLoadFromGroup}};
                }
                if(fn==1){
                    allocationFails=i%2;exceptionCode=0;exceptionAccess=0;for(size_t j=0;j<vehicleBytes.size();++j)vehicleBytes[j]=uint8(i*7+j);
                    vehicleInfo.get()->m_nVehicleType=eVehicleType((i/2)%16-2);CModelInfo::ms_modelInfoPtrs[400]=vehicleInfo.get();
                    PFProbe::expectedAllocationFailure=allocationFails;auto* result=Factory(400,uint8((i/32)%256));PFProbe::expectedAllocationFailure=false;
                    return {{"result",result==reinterpret_cast<CVehicle*>(vehicleBytes.data())},{"null",result==nullptr},{"exception",exceptionCode},{"access",exceptionAccess},{"trace",trace},{"bytes",vehicleBytes}};
                }
                group1.b.fill(0);group2.b.fill(0);
                for(int j=0;j<16;++j){
                    peds[j].b.fill(0);matrices[j].b.fill(0);auto* p=peds[j].get();p->SetTypePed();p->m_nPedState=std::array{PEDSTATE_IDLE,PEDSTATE_DIE,PEDSTATE_DEAD,PEDSTATE_DRIVING}[(i/13+j)%4];p->m_nPedType=(i/7+j)%5?PED_TYPE_CIVMALE:PED_TYPE_PLAYER1;
                    if(i%3==0)p->m_nPedState=PEDSTATE_IDLE;
                    if(j%2)p->m_matrix=matrices[j].get();
                    auto coord=[&](int k){seed=seed*1664525u+1013904223u;return i%7==0?0.f:float(int(seed%10000)-5000)*std::array{1.f,.125f,.0000001f,1000000.f}[(i/19+k)%4];};
                    p->GetPosition()=CVector{coord(0),coord(1),coord(2)};
                    bool present=((uint32(i*37)>>(j%8))&1)!=0;if(i%11==0)present=true;if(i%17==0&&j>=8)present=false;
                    auto* group=j<8?group1.get():group2.get();*reinterpret_cast<CPed**>(reinterpret_cast<uint8*>(group)+0xC+(j%8)*4)=present?p:nullptr;
                }
                auto before1=group1.b,before2=group2.b;std::array<CPed*,10> result;result.fill(reinterpret_cast<CPed*>(0x12345678));
                reinterpret_cast<void(__cdecl*)(CPedGroup&,CPedGroup&,CPed**)>(0x69C850)(*group1.get(),*group2.get(),result.data()+1);
                std::array<int,8> ids;for(int j=0;j<8;++j){ids[j]=-1;for(int k=0;k<16;++k)if(result[j+1]==peds[k].get())ids[j]=k;}
                return {{"targets",ids},{"guards",result.front()==reinterpret_cast<CPed*>(0x12345678)&&result.back()==reinterpret_cast<CPed*>(0x12345678)},{"groups_unchanged",group1.b==before1&&group2.b==before2}};
            };
            h->State(false);auto a=PFProbe::calls[fn];auto original=run();auto b=PFProbe::calls[fn];h->State(true);auto candidate=run();auto c=PFProbe::calls[fn];
            passed=original==candidate&&a==b&&c==b+1;if(fn==2)passed=passed&&original["guards"]==true&&original["groups_unchanged"]==true;
            Write({{"event","comparison"},{"pair",pairs++},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
        h->State(enabled);h->LockState(locked);
    }
    }
    bool restored=true;for(auto& s:saved){s.Restore();restored=restored&&s.Same();}
    restored=restored&&playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&fpBefore==_controlfp(0,0);
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();_CrtSetReportHook(report);
}
}
