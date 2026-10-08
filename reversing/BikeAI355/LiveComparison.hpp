#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Bike.h"
#include "Entity/Ped/Ped.h"
#include "Models/VehicleModelInfo.h"
#include "CarAI.h"
#include "CarCtrl.h"
#include "cTransmission.h"
#include "Stats.h"
#include "Pad.h"
#include "../ProbeRuntime.hpp"
namespace BikeAI355Live {
template<class T> T& At(void* p,size_t n){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+n);}
inline json Bytes(const void* p,size_t n){json value=json::array();auto* bytes=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)value.push_back(bytes[i]);return value;}
inline uint32 F(float x){return std::bit_cast<uint32>(x);}
inline uint32 scenario;inline json trace;inline CPed* fixturePed;
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return reinterpret_cast<CPlayerPed*>(fixturePed);}
inline CPad* __cdecl Pad(int32 id){return &CPad::Pads[id];}
inline void __fastcall Inputs(CBike* bike,void*,uint8 player){trace.push_back({"inputs",player});At<float>(bike,0x4A0)=float(int32(scenario%5)-2)*.25f;}
inline void __fastcall Horn(CBike*,void*){trace.push_back({"horn"});}
inline float __cdecl Modifier(eStatModAbilities mod){trace.push_back({"modifier",int(mod)});return 1.f+float(scenario%5)*.125f;}
inline void __fastcall Turn(CPhysical*,void*,CVector force,CVector point){trace.push_back({"turn",Bytes(&force,sizeof(force)),Bytes(&point,sizeof(point))});}
inline void __fastcall Prune(CEntity*,void*){trace.push_back({"prune"});}
inline void __fastcall Drive(CVehicle*,void*){trace.push_back({"drive"});}
inline void __fastcall Soft(CBike*,void*,uint32& flags){trace.push_back({"soft",flags});flags|=0x20;}
inline int16 __fastcall Gun(const CPad*,void*){trace.push_back({"gun"});return scenario%3==0?1:0;}
inline void __fastcall Bomb(CVehicle*,void*){trace.push_back({"bomb"});}
inline void __cdecl AI(CVehicle* v){trace.push_back({"AI"});At<uint8>(v,0x3DB)|=1;}
inline void __fastcall Physics(CPhysical*,void*){trace.push_back({"physics"});}
inline void __cdecl Rails(CVehicle*){trace.push_back({"rails"});}
inline void __cdecl Steer(CVehicle*){trace.push_back({"steer"});}
inline void __fastcall Gear(cTransmission*,void*,float speed,uint8& gear){trace.push_back({"gear",F(speed),gear});gear=uint8((gear+1)%6);}
inline float __fastcall Wheel(CVehicle*,void*,tWheelState state,const CVector& forward,const CVector& speed,float radius){trace.push_back({"wheel",int(state),Bytes(&forward,sizeof(forward)),Bytes(&speed,sizeof(speed)),F(radius)});return .125f+float(scenario%5)*.0625f;}
inline bool done;inline std::ofstream log;
inline void Write(const json& value){ReversingProbe::PumpMessages();log<<value.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CBike","ProcessAI",0,0x871360,66,1024}}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CBike")for(auto& h:c.Items())if(h->Name()=="ProcessAI")found=h.get();});return found;}
inline uintptr_t PhysicsAddress(){ReversibleHooks::ReversibleHook::Base* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPhysical")for(auto& h:c.Items())if(h->Name()=="ProcessControl")found=h.get();});return found?uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(found)->GetHookOurAddress()):0;}
inline void Update(){
 if(done)return;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);auto directory=std::filesystem::path(executable).parent_path();
 if(!std::filesystem::exists(directory/"bike-ai355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(directory/"bike-ai355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-ai355");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p0(0x56E210,Player),p1(bit_cast<uintptr_t>(&FindPlayerPed),Player),pad(0x53FB70,Pad),mod0(0x559AF0,Modifier),mod1(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),Modifier);
 EntryPatch turn0(0x542A50,Turn),turn1(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),Turn),pr0(0x571A90,Prune),pr1(bit_cast<uintptr_t>(&CEntity::PruneReferences),Prune),drive0(0x741FD0,Drive),drive1(bit_cast<uintptr_t>(&CVehicle::DoDriveByShootings),Drive);
 EntryPatch soft0(0x6B6D40,Soft),soft1(bit_cast<uintptr_t>(&CBike::DoSoftGroundResistance),Soft),gun0(0x53FFE0,Gun),gun1(bit_cast<uintptr_t>(&CPad::CarGunJustDown),Gun),bomb0(0x6D24F0,Bomb),bomb1(bit_cast<uintptr_t>(&CVehicle::ActivateBomb),Bomb);
 EntryPatch ai0(0x41DA30,AI),ai1(bit_cast<uintptr_t>(&CCarAI::UpdateCarAI),AI),phys0(0x5485E0,Physics),phys1(PhysicsAddress(),Physics),rails0(0x436540,Rails),rails1(bit_cast<uintptr_t>(&CCarCtrl::UpdateCarOnRails),Rails),st0(0x437C20,Steer),st1(bit_cast<uintptr_t>(&CCarCtrl::SteerAICarWithPhysics),Steer);
 EntryPatch gear0(0x6D0530,Gear),gear1(bit_cast<uintptr_t>(&cTransmission::CalculateGearForSimpleCar),Gear),wheel0(0x6D1230,Wheel),wheel1(bit_cast<uintptr_t>(&CVehicle::ProcessWheelRotation),Wheel);
 struct Region{uintptr_t address;size_t count;std::vector<uint8> saved;};std::array<Region,5> regions{{{0xA9B0C8+522*4,4},{0x97D6C0,16},{0xB7CD98,800},{0xB7CB5C,4},{0x8712FC,4}}};for(auto& r:regions){r.saved.resize(r.count);std::memcpy(r.saved.data(),reinterpret_cast<void*>(r.address),r.count);}
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
 for(int test=0;test<1024;++test){auto run=[&](bool candidate){
 hook->State(candidate);scenario=test;trace=json::array();for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);
 alignas(CBike) std::array<uint8,sizeof(CBike)> bikeBytes{};auto* bike=reinterpret_cast<CBike*>(bikeBytes.data());std::array<uintptr_t,70> bikeVT{};bikeVT[24]=bit_cast<uintptr_t>(&Inputs);bikeVT[54]=bit_cast<uintptr_t>(&Horn);At<uintptr_t>(bike,0)=uintptr_t(bikeVT.data());
 CMatrix matrix{};std::memset(&matrix,0,sizeof(matrix));matrix.GetRight()=CVector{1,0,0};matrix.GetForward()=CVector{.125f,1,0};matrix.GetUp()=CVector{0,0,1};At<CMatrix*>(bike,0x14)=&matrix;At<int16>(bike,0x22)=522;At<uint8>(bike,0x36)=uint8(2|((test/64)%16)<<3);At<int8>(bike,0x424)=test%7==0?-1:int8(test%4);At<uint8>(bike,0x3DB)=0xFF;At<uint8>(bike,0x614)=uint8(test%256);At<uint8>(bike,0x428)=uint8((test&32)?0x20:0);At<uint8>(bike,0x42A)=(test&8)?8:0;At<uint8>(bike,0x42B)=0xFF;At<uint8>(bike,0x804)=uint8(test%3);
 alignas(CVehicleModelInfo) std::array<uint8,sizeof(CVehicleModelInfo)> modelBytes{};At<float>(modelBytes.data(),0x40)=.75f;At<float>(modelBytes.data(),0x44)=.875f;StaticRef<uintptr_t>(0xA9B0C8+522*4)=uintptr_t(modelBytes.data());
 alignas(float) std::array<uint8,256> handlingBytes{},bikeHandlingBytes{};At<uintptr_t>(bike,0x384)=uintptr_t(handlingBytes.data());At<uintptr_t>(bike,0x63C)=uintptr_t(bikeHandlingBytes.data());At<float>(handlingBytes.data(),24)=.25f;At<float>(bikeHandlingBytes.data(),4)=.375f;At<float>(bikeHandlingBytes.data(),8)=.5f;At<float>(bikeHandlingBytes.data(),12)=.625f;At<float>(bikeHandlingBytes.data(),16)=.75f;
 alignas(CPed) std::array<uint8,sizeof(CPed)> pedBytes{},otherPedBytes{};fixturePed=reinterpret_cast<CPed*>(pedBytes.data());auto* other=reinterpret_cast<CPed*>(otherPedBytes.data());fixturePed->m_nPedState=static_cast<ePedState>(test%7==0?61:test%7==1?58:0);bike->m_pDriver=(test&2)?fixturePed:(test&4)?other:nullptr;bike->m_apPassengers[0]=(test&8)?other:nullptr;StaticRef<CPed*>(0xB7CD98)=fixturePed;StaticRef<CPed*>(0xB7CF28)=other;
 bike->m_vecMoveSpeed=CVector{float(int32(test%9)-4)*.03125f,float(int32((test/9)%9)-4)*.03125f,0};At<float>(bike,0x90)=2.f;At<CVector>(bike,0xA4)=CVector{.125f,.25f,.375f};At<float>(bike,0x650)=float(int32(test%7)-3)*.125f;At<float>(bike,0x654)=.5f;At<float>(bike,0x658)=.25f;At<float>(bike,0x4A0)=float(int32(test%5)-2)*.25f;At<float>(bike,0x49C)=float(test%4)*.25f;At<float>(bike,0x3C8)=float(test%13)*.125f;At<uint32>(bike,0x514)=test%5;At<uint8>(bike,0x4B4)=uint8(test%6);
 StaticRef<float>(0xB7CB5C)=float(1+test%5)*.25f;for(size_t n=0;n<16;++n)StaticRef<uint8>(0x97D6C0+n)=test%5==0?0:1;
 uint32 flags=uint32(test%8);const uintptr_t entry=reinterpret_cast<uintptr_t*>(0x871360)[66];const bool switched=(entry!=0x6BC930)==candidate;const bool result=reinterpret_cast<bool(__thiscall*)(CBike*,uint32&)>(entry)(bike,flags);
 json globals=json::array();for(auto& r:regions)globals.push_back(Bytes(reinterpret_cast<void*>(r.address),r.count));return json{{"result",result},{"flags",flags},{"bike",Bytes(bikeBytes.data(),bikeBytes.size())},{"ped",Bytes(pedBytes.data(),pedBytes.size())},{"otherPed",Bytes(otherPedBytes.data(),otherPedBytes.size())},{"model",Bytes(modelBytes.data(),modelBytes.size())},{"handling",Bytes(handlingBytes.data(),handlingBytes.size())},{"bikeHandling",Bytes(bikeHandlingBytes.data(),bikeHandlingBytes.size())},{"matrix",Bytes(&matrix,sizeof(matrix))},{"globals",globals},{"trace",trace},{"switches",switched}};
 };auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true;failures+=!equal;Write({{"event","comparison"},{"pair",test},{"function","CBike::ProcessAI"},{"case",test},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});}
 hook->State(enabled);hook->LockState(locked);for(auto& r:regions)std::memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.count);Write({{"event","function_summary"},{"function","CBike::ProcessAI"},{"cases",1024},{"failures",failures}});Write({{"event",failures?"error":"complete"},{"pairs",1024},{"state_restored",true}});
}
}



