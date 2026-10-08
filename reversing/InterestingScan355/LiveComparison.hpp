#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "InterestingEvents.h"
#include "RepeatSector.h"
#include "../ProbeRuntime.hpp"
namespace InterestingScan355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
alignas(16) inline std::array<uint8,sizeof(CInterestingEvents)> object{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> player{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,6> peds{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAutomobile)>,2> vehicles{};
inline CMatrixLink matrix{};inline std::array<std::array<void*,3>,6> pedNodes{};inline std::array<std::array<void*,3>,2> vehicleNodes{};
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline json Ptr(const void* p){if(!p)return nullptr;if(p==player.data())return "player";for(int i=0;i<6;++i)if(p==peds[i].data())return json::array({"ped",i});for(int i=0;i<2;++i)if(p==vehicles[i].data())return json::array({"vehicle",i});return uintptr_t(p);}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline CPlayerPed* __cdecl Player(int){trace.push_back({"player"});return reinterpret_cast<CPlayerPed*>(player.data());}
inline void __fastcall Add(CInterestingEvents*,void*,CInterestingEvents::EType t,CEntity* e){trace.push_back({"add",int(t),Ptr(e)});}
inline void __cdecl Clear(){trace.push_back({"clear_scan"});reinterpret_cast<CPed*>(player.data())->SetScanCode(0);for(auto& p:peds)reinterpret_cast<CPed*>(p.data())->SetScanCode(0);for(auto& v:vehicles)reinterpret_cast<CVehicle*>(v.data())->SetScanCode(0);}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CInterestingEvents","ScanForNearbyEntities",0x605A30,0,-1,2048}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CInterestingEvents")for(auto& x:c.Items())if(x->Name()=="ScanForNearbyEntities")h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"interesting-scan355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"interesting-scan355-runtime.jsonl");ReversingProbe::InstallFailureCapture("interesting-scan355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,sizeof(CWorld::ms_aRepeatSectors)> sectorsSaved{};std::memcpy(sectorsSaved.data(),&CWorld::ms_aRepeatSectors,sectorsSaved.size());std::array<uint8,sizeof(TheCamera)> cameraSaved{};std::memcpy(cameraSaved.data(),&TheCamera,cameraSaved.size());const auto oldFlags=g_InterestingEvents.m_nFlags;const auto oldTime=CTimer::m_snTimeInMilliseconds,oldFrame=CTimer::m_FrameCounter;const auto oldScan=CWorld::ms_nCurrentScanCode;auto* h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}auto old=h->Hooked(),locked=h->Locked();h->LockState(false);int pairs=0,failures=0,adds=0,clears=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch pe(0x56E210,Player),pd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
EntryPatch ae(0x602590,Add),ad(bit_cast<uintptr_t>(&CInterestingEvents::Add),Add);
EntryPatch ce(0x563470,Clear),cd(bit_cast<uintptr_t>(&CWorld::ClearScanCodes),Clear);
for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();object.fill(0);player.fill(0);for(auto& p:peds)p.fill(0);for(auto& v:vehicles)v.fill(0);matrix.SetUnity();matrix.GetPosition()=CVector{float(i%9-4)*10.f,float(i%7-3)*5.f,2.f};auto* pl=reinterpret_cast<CPlayerPed*>(player.data());pl->m_matrix=&matrix;TheCamera.m_nActiveCam=0;TheCamera.m_aCams[0].m_vecSource=i%4==0?matrix.GetPosition():CVector{0.f,0.f,4.f};g_InterestingEvents.m_nFlags=uint8(i);CTimer::m_FrameCounter=77;CTimer::m_snTimeInMilliseconds=i%13==0?0xFFFFFE00u:1000u;CWorld::ms_nCurrentScanCode=i%7==0?65535:uint16(i%3);auto* t=reinterpret_cast<CInterestingEvents*>(object.data());t->m_nFlags=uint8(i^1);t->m_CurrentFrameCounter=i%2?77:76;constexpr float centers[]{-3000.f,-.01f,0.f,2999.99f,3000.f,100000.f};t->m_vecCenter=CVector{centers[(i/32)%6],centers[(i/192)%6],2.f};t->vec148=CVector{1.f,0.f,0.f};constexpr float radii[]{0.f,30.f,100.f,-10.f};t->m_fRadius=radii[(i/8)%4];constexpr uint32 elapsed[]{0,499,500,501,0xFFFFFFFFu};t->m_nLastScanTimeUpdate=CTimer::GetTimeInMS()-elapsed[(i/3)%5];
for(int k=0;k<6;++k){auto* p=reinterpret_cast<CPed*>(peds[k].data());constexpr int types[]{6,20,22,7,16,4};p->m_nPedType=static_cast<ePedType>(types[(i+k)%6]);p->m_nPedState=static_cast<ePedState>((i+k)%7==0?55:0);p->bInVehicle=(i+k)%3==0;p->m_pVehicle=(i+k)%2?reinterpret_cast<CVehicle*>(vehicles[k%2].data()):nullptr;p->SetScanCode(uint16((i+k)%3));pedNodes[k]={p,k<5?pedNodes[k+1].data():nullptr,k?pedNodes[k-1].data():nullptr};}
for(int k=0;k<2;++k){auto* v=reinterpret_cast<CVehicle*>(vehicles[k].data());v->SetScanCode(uint16((i+k)%3));v->physicalFlags.bRenderScorched=(i+k)%7==0;v->m_pDriver=(i+k)%3?reinterpret_cast<CPed*>(peds[k].data()):nullptr;v->m_autoPilot.m_nCarDrivingStyle=static_cast<eCarDrivingStyle>((i+k)%8);vehicleNodes[k]={v,k==0?vehicleNodes[1].data():nullptr,k?vehicleNodes[0].data():nullptr};}
auto** heads=reinterpret_cast<void**>(&CWorld::ms_aRepeatSectors);for(int k=0;k<256;++k){heads[k*3]=vehicleNodes.data();heads[k*3+1]=pedNodes.data();heads[k*3+2]=nullptr;}
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x605A30)==candidate;reinterpret_cast<void(__thiscall*)(CInterestingEvents*)>(0x605A30)(t);
return json{{"object",Hash(object.data(),object.size())},{"player",Hash(player.data(),player.size())},{"peds",Hash(peds.data(),sizeof(peds))},{"vehicles",Hash(vehicles.data(),sizeof(vehicles))},{"scan",CWorld::ms_nCurrentScanCode},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;for(auto& e:a["trace"]){adds+=e[0]=="add";clears+=e[0]=="clear_scan";}Write({{"event","comparison"},{"function","CInterestingEvents::ScanForNearbyEntities"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}}
h->State(old);h->LockState(locked);std::memcpy(&CWorld::ms_aRepeatSectors,sectorsSaved.data(),sectorsSaved.size());std::memcpy(&TheCamera,cameraSaved.data(),cameraSaved.size());g_InterestingEvents.m_nFlags=oldFlags;CTimer::m_snTimeInMilliseconds=oldTime;CTimer::m_FrameCounter=oldFrame;CWorld::ms_nCurrentScanCode=oldScan;pass&=adds>0&&clears>0;Write({{"event","function_summary"},{"function","CInterestingEvents::ScanForNearbyEntities"},{"cases",pairs},{"failures",failures},{"adds",adds},{"clears",clears}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
