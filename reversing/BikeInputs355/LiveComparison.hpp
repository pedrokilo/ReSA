#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "SurfaceInfos_c.h"
#include "AEVehicleAudioEntity.h"
#include "CarCtrl.h"
#include "World.h"
#include "Camera.h"
#include "Explosion.h"
#include "Darkel.h"
#include "VisibilityPlugins.h"
#include "Stats.h"
#include "PlayerPed.h"
#include "Pad.h"
#include "../ProbeRuntime.hpp"
namespace BikeInputs355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int scenario{},padCalls{};inline CPad pad{};inline CPlayerPed* player{};
inline CPad* __cdecl GetPad(int32 index){trace.push_back({"pad",index,padCalls++});if(scenario%13==0){CTimer::UpdateTimeStep(CTimer::GetTimeStep()+.125f);StaticRef<float>(0xB73424)+=.25f;CTimer::SetTimeInMS(CTimer::GetTimeInMS()+0x100);}return &pad;}
inline bool __fastcall Exit(void*,void*){trace.push_back({"exit"});return scenario%7==0;}
inline int16 __fastcall Hand(void*,void*){trace.push_back({"hand"});return scenario%3==0?128:0;}
inline int16 __fastcall Left(void*,void*){static constexpr int16 values[]{-128,0,64,128,255};const auto result=values[(scenario/3)%5];trace.push_back({"left",result});return result;}
inline int16 __fastcall Up(void*,void*){static constexpr int16 values[]{-128,0,64,128,255};const auto result=values[(scenario/7)%5];trace.push_back({"up",result});return result;}
inline int16 __fastcall Accelerate(void*,void*){static constexpr int16 values[]{0,128,200,255};const auto result=values[(scenario/5)%4];trace.push_back({"accelerate",result});return result;}
inline int16 __fastcall Brake(void*,void*){static constexpr int16 values[]{0,128,200,255};const auto result=values[(scenario/11)%4];trace.push_back({"brake",result});return result;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline void __fastcall Keep(void*,void*){trace.push_back({"keep"});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","ProcessControlInputs",0,0x871360,24,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-inputs355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-inputs355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-inputs355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-inputs355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 std::array<uint8,16> savedVehicle;std::array<uint8,8> savedMouse;std::array<uint8,64> savedTimer;std::array<uint8,3> savedActive;const auto savedCamera=StaticRef<uint8>(0xB6EC2E);const auto savedSteer=StaticRef<float>(0xC1C804);const auto savedGame=StaticRef<int32>(0x96A8A0);memcpy(savedVehicle.data(),reinterpret_cast<void*>(0xC1CC00),16);memcpy(savedMouse.data(),reinterpret_cast<void*>(0xB73424),8);memcpy(savedTimer.data(),reinterpret_cast<void*>(0xB7CB48),64);memcpy(savedActive.data(),reinterpret_cast<void*>(0x97D6C0),3);uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch gp(0x53FB70,GetPad),gpd(bit_cast<uintptr_t>(&CPad::GetPad),GetPad);
 EntryPatch ex(0x5400D0,Exit),exd(bit_cast<uintptr_t>(&CPad::GetExitVehicle),Exit);
 EntryPatch hb(0x540040,Hand),hbd(bit_cast<uintptr_t>(&CPad::GetHandBrake),Hand);
 EntryPatch le(0x53FB80,Left),led(bit_cast<uintptr_t>(&CPad::GetSteeringLeftRight),Left);
 EntryPatch up(0x53FBD0,Up),upd(bit_cast<uintptr_t>(&CPad::GetSteeringUpDown),Up);
 EntryPatch ac(0x5403F0,Accelerate),acd(bit_cast<uintptr_t>(&CPad::GetAccelerate),Accelerate);
 EntryPatch br(0x540080,Brake),brd(bit_cast<uintptr_t>(&CPad::GetBrake),Brake);
 EntryPatch pp(0x56E210,Player),ppd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch ke(0x60C1E0,Keep),ked(bit_cast<uintptr_t>(&CPlayerPed::KeepAreaAroundPlayerClear),Keep);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CPlayerPed)> playerPed{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};
   auto run=[&](bool candidate){scenario=i;padCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(playerPed.data(),0,playerPed.size());memset(matrix.data(),0,matrix.size());memset(handling.data(),0,handling.size());memset(&pad,0,sizeof(pad));auto* self=reinterpret_cast<CBike*>(object.data());player=reinterpret_cast<CPlayerPed*>(playerPed.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{0.f,1.f,0.f};self->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());self->m_pHandlingData->m_fSteeringLock=float(i%9+1)*5.f;
    static constexpr float steering[]{-1.5f,-1.f,-.4f,0.f,.4f,1.f,1.5f,std::bit_cast<float>(0x7FC00000u)};self->m_fRawSteerAngle=steering[(i/3)%8];self->m_RideAnimData.LeanFwd=steering[(i/7)%8];self->m_vecMoveSpeed=CVector{float(i%5)*.125f,float(i%9-4)*.125f,float(i%3)*.125f};self->m_GasPedal=-.25f;self->m_BrakePedal=.5f;self->m_nVehicleSubType=static_cast<eVehicleType>(i%5==0?10:9);object[0x424]=uint8(i%4-1);object[0x428]=uint8(i%2);object[0x429]=i%7==0?0x10:0;
    memset(reinterpret_cast<void*>(0xC1CC00),0,16);CVehicle::m_bEnableMouseSteering=i%3!=0;CVehicle::m_nLastControlInput=static_cast<eControllerType>(i%4);StaticRef<uint8>(0xB6EC2E)=uint8(i%2);StaticRef<float>(0xB73424)=i%5==0?0.f:float(i%11-5);StaticRef<float>(0xB73428)=i%7==0?0.f:float(i%9-4);StaticRef<float>(0xC1C804)=.125f;StaticRef<int32>(0x96A8A0)=i%3;CTimer::UpdateTimeStep(float(i%3));CTimer::SetTimeInMS(uint32(i*1024));for(size_t j=0;j<3;++j)StaticRef<uint8>(0x97D6C0+j)=uint8((i+int(j))%2);pad.NewState.m_bVehicleMouseLook=i%5==0?128:0;*reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(&pad)+0x10E)=i%17==0?1:0;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6BE310)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*,uint8)>(address)(self,uint8(i%2));self->m_matrix=nullptr;self->m_pHandlingData=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"mouse",Hash(reinterpret_cast<void*>(0xB73424),8)},{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),64)},{"vehicle",Hash(reinterpret_cast<void*>(0xC1CC00),16)},{"steer",std::bit_cast<uint32>(StaticRef<float>(0xC1C804))},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xC1CC00),savedVehicle.data(),16);memcpy(reinterpret_cast<void*>(0xB73424),savedMouse.data(),8);memcpy(reinterpret_cast<void*>(0xB7CB48),savedTimer.data(),64);memcpy(reinterpret_cast<void*>(0x97D6C0),savedActive.data(),3);StaticRef<uint8>(0xB6EC2E)=savedCamera;StaticRef<float>(0xC1C804)=savedSteer;StaticRef<int32>(0x96A8A0)=savedGame;
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}





