#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Pools/PedPool.h"
#include "MatrixLink.h"
#include "PlayerPedData.h"
#include "References.h"
#include "Probe.hpp"

namespace P2PLive {
using namespace P2PProbe;
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { log << row.dump() << '\n'; log.flush(); }
inline int Report(int type,char* message,int*) { if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",message ? message : ""}});ExitProcess(91); }
inline std::vector<Spec> Cases() {
    std::vector<Spec> cases;
    auto add=[&](Spec s,const char* family) { s.name=std::string(family)+"-"+std::to_string(cases.size());cases.push_back(s); };
    for(uint32 state : {1u,2u,3u,4u,0xFFFFFFFFu})
        for(uint32 start : {60000u,0xFFFFFF00u})
            for(uint32 delta : {0u,2499u,2500u,2501u,2999u,3000u,3001u,3999u,4000u,4001u})
                for(int flags=0;flags<16;++flags) {
                    Spec s;s.state=state;s.start=start;s.time=start+delta;s.participant=flags&1;s.positive=flags&2;s.lookPlayer=flags&4;s.lookPed=flags&8;s.talking=flags&4;add(s,"timer");
                }
    for(int topic=-1;topic<=10;++topic)
        for(int type : {int(PED_TYPE_CIVMALE),int(PED_TYPE_CIVFEMALE),int(PED_TYPE_PROSTITUTE),int(PED_TYPE_GANG2)})
            for(int random : {0,39,40,74,75,99})
                for(int flags=0;flags<8;++flags) {
                    Spec s;s.state=1;s.topic=topic;s.type=type;s.random=random;s.positive=flags&1;s.yes=flags&2;s.no=flags&4;add(s,"reply");
                }
    for(int topic=-1;topic<=10;++topic)
        for(int seed : {0,127,256,511,1023})
            for(float value : {0.f,149.f,150.f,151.f,299.f,300.f,301.f,399.f,400.f,401.f,499.f,500.f,501.f,699.f,700.f,701.f,1000.f}) {
                Spec s;s.topic=topic;s.seed=seed;s.respect=value;s.appeal=value;s.muscle=value;s.fat=1000.f-value;s.random=static_cast<int>(value);add(s,"opening-stat");
            }
    for(int topic=0;topic<10;++topic)
        for(int flags=0;flags<64;++flags) {
            Spec s;s.topic=topic;s.request=flags&1;s.chat=flags&2;s.hair=flags&4;s.tattoo=flags&8;s.fallback=(flags&16) ? 3 : 4;s.hasCar=flags&32;s.budget=0;add(s,"opening-gate");
        }
    for(int vc=-1;vc<=12;++vc)
        for(float health : {799.f,800.f,801.f})
            for(float d : {19.999f,20.f,20.001f}) {
                Spec s;s.topic=0;s.vehicleClass=vc;s.health=health;s.carDistance=d;add(s,"vehicle");
            }
    for(float wind : {0.099f,0.1f,0.101f,0.499f,0.5f,0.501f})
        for(int hour : {0,6,7,12,19,20,23}) {
            Spec s;s.topic=7;s.wind=wind;s.hour=hour;add(s,"weather-forced-topic");
        }
    for(int slot : {0,7,8,15})for(int idx : {-1,0,7,15})for(int gate=0;gate<9;++gate) {
        Spec s;s.slot=slot;s.index=idx;s.gate=gate;add(s,"scan");
    }
    for(float distance : {0.f,0.001f,6.999f,7.f,7.001f})for(int flags=0;flags<4;++flags) {
        Spec s;s.distance=distance;s.frontPlayer=flags&1;s.frontPed=flags&2;add(s,"geometry");
    }
    for(uint32 last : {0u,60000u,0xFFFFFF00u})for(uint32 delta : {29999u,30000u,30001u}) {
        Spec s;s.last=last;s.time=last+delta;add(s,"cooldown");
    }
    for(uint32 state=0;state<5;++state) { Spec s;s.state=state;s.inVehicle=true;add(s,"vehicle-reference"); }
    for(int topic : {1,2,3,4,5})for(int seed : {0,127,256,511,1023})
        for(float boundary : {150.f,250.f,300.f,400.f,500.f,700.f})for(float direction : {-1.f,1.f}) {
            Spec s;s.topic=topic;s.seed=seed;s.respect=std::nextafter(boundary,direction<0 ? 0.f : 1001.f);s.muscle=s.respect;s.fat=1000.f-s.respect;s.appeal=s.respect;add(s,"fractional-stat");
        }
    for(int type : {4,5,7,8,9,16,17,22})for(int topic : {0,6,7,8,9}) {
        Spec s;s.type=type;s.topic=topic;add(s,"ped-type");
    }
    for(int slot : {0,7,8,15})for(int idx : {-1,0,7,15}) {
        Spec s;s.slot=slot;s.index=idx;s.emptySlots=true;add(s,"empty-slots");
    }
    return cases;
}
struct Fixture {
    CPedPool pool{16,"p2p-fixture"};
    alignas(CPlayerPed) std::array<uint8,sizeof(CPlayerPed)> playerBytes{};
    alignas(CVehicle) std::array<uint8,sizeof(CVehicle)> carBytes{};
    alignas(CVehicleModelInfo) std::array<uint8,sizeof(CVehicleModelInfo)> modelBytes{};
    CMatrixLink playerMatrix{},pedMatrix{},carMatrix{};
    CPlayerPedData data{};
    CPedClothesDesc clothes{};
    std::array<void*,6> audioVtable{};
    std::array<CPed*,16> peds{};
    Fixture() { for(auto& p:peds)p=pool.New();audioVtable[5]=reinterpret_cast<void*>(&TopicABI); }
    void SetUp() {
        player=reinterpret_cast<CPlayerPed*>(playerBytes.data());
        car=reinterpret_cast<CVehicle*>(carBytes.data());
        ped=peds[spec.slot];
        playerBytes.fill(0);carBytes.fill(0);modelBytes.fill(0);
        // Segundo puntero del pool: array de SlotState de un byte.
        auto* flags=*reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(&pool)+4);
        for(size_t i=0;i<peds.size();++i) {
            flags[i]=spec.emptySlots && i!=static_cast<size_t>(spec.slot) ? 0x80 : 1;
            std::memset(peds[i],0,sizeof(CCopPed));peds[i]->SetCreatedBy(PED_MISSION);
        }
        ped->SetCreatedBy(spec.gate==6 ? PED_MISSION : PED_GAME);
        ped->bHasAScriptBrain=spec.gate==7;
        if(spec.gate==8)ped->SetCreatedBy(PED_UNKNOWN);
        ped->m_nPedType=static_cast<ePedType>(spec.type);ped->m_nRandomSeed=static_cast<uint16>(spec.seed);
        playerMatrix.SetUnity();pedMatrix.SetUnity();carMatrix.SetUnity();
        playerMatrix.GetForward()=CVector{0.f,spec.frontPlayer ? 1.f : -1.f,0.f};
        pedMatrix.GetForward()=CVector{0.f,spec.frontPed ? -1.f : 1.f,0.f};
        pedMatrix.GetPosition()=CVector{0.f,spec.distance,0.f};
        carMatrix.GetPosition()=CVector{spec.carDistance,spec.distance,0.f};
        player->m_matrix=&playerMatrix;ped->m_matrix=&pedMatrix;car->m_matrix=&carMatrix;
        *reinterpret_cast<void***>(reinterpret_cast<uint8*>(ped)+0x294)=audioVtable.data();
        // ABI comprobada contra Ped.h; sólo los proveedores de prueba leen estos enlaces.
        *reinterpret_cast<CPlayerPedData**>(playerBytes.data()+0x480)=&data;
        data.m_pPedClothesDesc=&clothes;
        *reinterpret_cast<void**>(reinterpret_cast<uint8*>(ped)+0x47C)=reinterpret_cast<void*>(0x1000);
        car->m_nModelIndex=400;car->m_fHealth=spec.health;
        reinterpret_cast<CVehicleModelInfo*>(modelBytes.data())->m_nVehicleClass=static_cast<eVehicleClass>(spec.vehicleClass);
        CModelInfo::ms_modelInfoPtrs[400]=reinterpret_cast<CVehicleModelInfo*>(modelBytes.data());
        StaticRef<CPedPool*>(0xB74490)=&pool;
        C::m_State=static_cast<C::eP2pState>(spec.state);C::m_Topic=spec.topic;
        C::m_pPed=spec.participant ? ped : nullptr;C::m_LastPlayerVehicle=spec.hasCar ? car : nullptr;
        C::m_StartTime=spec.start;C::m_TimeOfLastPlayerConversation=spec.last;
        C::m_bPositiveOpening=spec.positive;C::m_bPositiveReply=true;C::m_PedPoolIndex=spec.index;
        CTimer::m_snTimeInMilliseconds=spec.time;CWeather::Wind=spec.wind;CClock::ms_nGameClockHours=static_cast<uint8>(spec.hour);
        trace=json::array();
    }
    json Run() {
        SetUp();
        const auto beforePlayer=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
        const auto beforePed=ReversingProbe::Bytes(ped,sizeof(CCopPed));
        const auto beforeCar=ReversingProbe::Bytes(car,sizeof(CVehicle));
        reinterpret_cast<void(__cdecl*)()>(0x43B0F0)();
        json result={
            {"state",int(C::m_State)},{"topic",C::m_Topic},{"ped",Id(C::m_pPed)},{"vehicle",Id(C::m_LastPlayerVehicle)},
            {"start",C::m_StartTime},{"last",C::m_TimeOfLastPlayerConversation},{"positive",C::m_bPositiveOpening},
            {"reply",C::m_bPositiveReply},{"index",C::m_PedPoolIndex},
            {"ped_reference",ped->m_pReferences!=nullptr},{"car_reference",car->m_pReferences!=nullptr},{"calls",trace}
        };
        ped->CleanUpOldReference(reinterpret_cast<CEntity**>(&C::m_pPed));
        car->CleanUpOldReference(reinterpret_cast<CEntity**>(&C::m_LastPlayerVehicle));
        result["inputs_intact"]=beforePlayer==ReversingProbe::Bytes(player,sizeof(CPlayerPed)) && beforePed==ReversingProbe::Bytes(ped,sizeof(CCopPed)) && beforeCar==ReversingProbe::Bytes(car,sizeof(CVehicle));
        return result;
    }
};
inline BOOL CALLBACK CloseOwnWindow(HWND hwnd,LPARAM) {
    DWORD pid{};GetWindowThreadProcessId(hwnd,&pid);
    char name[128]{};GetClassNameA(hwnd,name,128);
    if(pid==GetCurrentProcessId() && std::strcmp(name,"Grand theft auto San Andreas")==0)PostMessage(hwnd,WM_CLOSE,0,0);
    return TRUE;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ped-to-player-runtime.start")){done=true;return;}
    auto* livePlayer=FindPlayerPed();if(!livePlayer)return;
    done=true;log.open(dir/"ped-to-player-runtime.jsonl");auto oldReport=_CrtSetReportHook(Report);
    ReversibleHooks::ReversibleHook::Base* hook{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPedToPlayerConversations")for(auto& h:c.Items())if(h->Name()=="Update")hook=h.get();});
    if(!hook){Write({{"event","error"},{"reason","hook missing"}});_CrtSetReportHook(oldReport);return;}
    const auto globals=ReversingProbe::Bytes(reinterpret_cast<void*>(0x9691B0),20);
    const auto tail=ReversingProbe::Bytes(reinterpret_cast<void*>(0x969A20),36);
    const auto liveBytes=ReversingProbe::Bytes(livePlayer,sizeof(CPlayerPed));
    const auto oldTime=CTimer::GetTimeInMS();const auto oldWind=CWeather::Wind;const auto oldHour=CClock::ms_nGameClockHours;
    auto* oldPool=GetPedPool();auto* oldModel=CModelInfo::ms_modelInfoPtrs[400];
    const auto refs=CReferences::ListSize(CReferences::pEmptyList);
    const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    bool passed=true;int count=0;active=true;
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        auto patch=[&](uintptr_t address,auto fn){patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,fn));};
        patch(0x56E210,&Player);patch(0x56E0D0,&Vehicle);patch(0x56E010,&Coors);
        patch(0x41BE60,&WantedABI);patch(0x43AAC0,&Conversation);patch(0x43ABA0,&ReadyABI);patch(0x534540,&ScreenABI);
        patch(0x407180,&RandomRange);patch(0x821B1E,&Random);patch(0x558E40,&Stat);
        patch(0x5A7970,&HairABI);patch(0x5A79D0,&TattooABI);patch(0x4E38C0,&Request);patch(0x4E3960,&Release);
        patch(0x5EFFE0,&SayABI);patch(0x5EFF50,&TalkingABI);patch(0x5EFF60,&DisableABI);patch(0x5EFF70,&EnableABI);
        patch(0x6181A0,&LookingABI);patch(0x618280,&AbortABI);patch(0x618970,&LookAtABI);
        patch(0x5411D0,&YesABI);patch(0x541200,&NoABI);patch(0x4AB420,&EventABI);
        Fixture a,b;
        for(const auto& s:Cases()) {
            spec=s;hook->State(false);auto begin=calls;
            const auto prolog=ReversingProbe::Bytes(reinterpret_cast<void*>(0x43B0F0),6);
            Write({{"event","begin"},{"pair",count},{"case",s.name}});
            auto original=a.Run();auto middle=calls;hook->State(true);auto candidate=b.Run();auto end=calls;
            passed=original==candidate && original["inputs_intact"]==true && middle==begin && end==middle+1 && prolog==std::vector<uint8>{0x64,0xA1,0,0,0,0};
            Write({{"event","comparison"},{"pair",count++},{"case",s.name},{"equal",passed},{"original",original},{"candidate",candidate},{"original_candidate_calls",middle-begin},{"candidate_calls",end-middle}});
            if(!passed)break;
        }
        hook->State(enabled);hook->LockState(locked);
        StaticRef<CPedPool*>(0xB74490)=oldPool;CModelInfo::ms_modelInfoPtrs[400]=oldModel;
        std::memcpy(reinterpret_cast<void*>(0x9691B0),globals.data(),globals.size());
        std::memcpy(reinterpret_cast<void*>(0x969A20),tail.data(),tail.size());
        CTimer::m_snTimeInMilliseconds=oldTime;CWeather::Wind=oldWind;CClock::ms_nGameClockHours=oldHour;
    }
    active=false;
    const bool restored=globals==ReversingProbe::Bytes(reinterpret_cast<void*>(0x9691B0),20) && tail==ReversingProbe::Bytes(reinterpret_cast<void*>(0x969A20),36)
        && liveBytes==ReversingProbe::Bytes(livePlayer,sizeof(CPlayerPed)) && GetPedPool()==oldPool && CModelInfo::ms_modelInfoPtrs[400]==oldModel
        && CReferences::ListSize(CReferences::pEmptyList)==refs && hook->Hooked()==enabled && hook->Locked()==locked;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",count},{"expected",Cases().size()},{"state_restored",restored}});
    _CrtSetReportHook(oldReport);EnumWindows(CloseOwnWindow,0);
}
}
