#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Plane.h"
#include "FxSystem.h"
#include "FxPrtMult.h"
#include "cHandlingDataMgr.h"
#include "../ProbeRuntime.hpp"
namespace PlaneFlying355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline CPlane* current{};
inline int scenario{},heightCalls{},randomCalls{},flightCalls{},particleCalls{};
inline std::array<RwFrame,25> frames{};inline std::array<uint8,80> flyingHandling{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline json V(const CVector& v){return {std::bit_cast<uint32>(v.x),std::bit_cast<uint32>(v.y),std::bit_cast<uint32>(v.z)};}
inline int32 __cdecl Random(){const int32 n=(scenario*7919+randomCalls++*113)&32767;trace.push_back({"random",n});return n;}
inline uint32 __fastcall Status(void*,void*,uint8 node){const uint32 result=scenario%8==1?0:uint32((node+scenario/8)%3);trace.push_back({"status",node,result});if(scenario%17==0&&node==13)current->m_aCarNodes[14]=nullptr;return result;}
inline void __fastcall SetStatus(void*,void*,uint8 node,uint32 value){trace.push_back({"set_status",node,value});reinterpret_cast<uint8*>(&current->m_damageManager)[2]^=uint8(node+value);}
inline float __fastcall Roll(void*,void*){const float result=std::array<float,5>{0.f,1.f,2.f,3.f,-2.f}[(scenario+int(trace.size()))%5];trace.push_back({"roll",std::bit_cast<uint32>(result)});return result;}
inline float __fastcall Height(void*,void*,float z,eFlightModel model){const float result=scenario%7==0?-2.f:scenario%7==1?0.f:float(1+heightCalls++);trace.push_back({"height",std::bit_cast<uint32>(z),int(model),std::bit_cast<uint32>(result)});current->GetPosition().z+=.25f;if(scenario%11==0)CTimer::ms_fTimeStep+=.125f;return result;}
inline tFlyingHandlingData* __fastcall Handling(void*,void*,uint8 id){trace.push_back({"handling",id});return reinterpret_cast<tFlyingHandlingData*>(flyingHandling.data());}
inline void __fastcall Flight(void*,void*,eFlightModel model,float skid,float up,float lateral,float acceleration){trace.push_back({"flight",int(model),std::bit_cast<uint32>(skid),std::bit_cast<uint32>(up),std::bit_cast<uint32>(lateral),std::bit_cast<uint32>(acceleration),current->m_pFlyingHandlingData==reinterpret_cast<tFlyingHandlingData*>(flyingHandling.data())});++flightCalls;current->m_vecMoveSpeed.x+=.125f;current->m_pFlyingHandlingData=nullptr;}
inline void __fastcall Particle(void*,void*,const CVector& point,const CVector& velocity,float since,const FxPrtMult_c& mult,float rotation,float light,float limit,bool local){trace.push_back({"particle",V(point),V(velocity),std::bit_cast<uint32>(since),Hash(&mult,sizeof(mult)),std::bit_cast<uint32>(rotation),std::bit_cast<uint32>(light),std::bit_cast<uint32>(limit),local});++particleCalls;current->m_vecMoveSpeed.z+=.03125f;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPlane")for(auto& h:c.Items())if(h->Name()=="ProcessFlyingCarStuff")r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CPlane","ProcessFlyingCarStuff",0,0x871948,68,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"plane-flying355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"plane-flying355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plane-flying355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);uint32 failures=0,positive=0,totalFlights=0,totalParticles=0;
 std::array<uint8,64> savedCamera{};memcpy(savedCamera.data(),reinterpret_cast<void*>(0xB6F028),64);
 const float savedStep=CTimer::ms_fTimeStep;const auto savedSmoke=StaticRef<void*>(0xA9AE20);const auto savedModel=StaticRef<void*>(0xA9B76C);
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch ra(0x821B1E,Random),rad(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch st(0x6C2300,Status),stdp(bit_cast<uintptr_t>(&CDamageManager::GetAeroplaneCompStatus),Status);
 EntryPatch ss(0x6C22D0,SetStatus),ssd(bit_cast<uintptr_t>(&CDamageManager::SetAeroplaneCompStatus),SetStatus);
 EntryPatch ro(0x420B30,Roll),rod(bit_cast<uintptr_t>(&CPlaceable::GetRoll),Roll);
 EntryPatch he(0x6D2600,Height),hed(bit_cast<uintptr_t>(&CVehicle::HeightAboveCeiling),Height);
 EntryPatch ha(0x6F52D0,Handling),had(bit_cast<uintptr_t>(&cHandlingDataMgr::GetFlyingPointer),Handling);
 EntryPatch fl(0x6D85F0,Flight),fld(bit_cast<uintptr_t>(&CVehicle::FlyingControl),Flight);
 EntryPatch pt(0x4AA440,Particle),ptd(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),Particle);
 for(int i=0;i<1024;++i){alignas(16) std::array<uint8,sizeof(CPlane)> object{};alignas(16) std::array<uint8,80> matrix{},oldHandling{},camera{};alignas(16) std::array<uint8,256> model{};
 auto run=[&](bool candidate){scenario=i;heightCalls=randomCalls=flightCalls=particleCalls=0;trace=json::array();memset(object.data(),0,object.size());memset(matrix.data(),0,matrix.size());memset(oldHandling.data(),0,oldHandling.size());memset(camera.data(),0,camera.size());memset(model.data(),0,model.size());memset(frames.data(),0,sizeof(frames));memset(flyingHandling.data(),0,flyingHandling.size());current=reinterpret_cast<CPlane*>(object.data());*reinterpret_cast<uintptr_t*>(current)=0x871948;current->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());current->m_matrix->GetRight()=CVector{1,0,0};current->m_matrix->GetForward()=CVector{0,1,0};current->m_matrix->GetUp()=CVector{0,0,1};current->GetPosition()=CVector{.25f,-.5f,float(i%5)};current->m_vecMoveSpeed=CVector{float((i/3)%5)*.0625f,-.125f,.0625f};
 current->m_nModelIndex=std::array<int16,4>{476,464,520,511}[(i/16)%4];current->SetStatus(static_cast<eEntityStatus>(std::array<uint8,7>{0,8,3,2,4,5,1}[(i/64)%7]));current->m_fHealth=i%3==0?100.f:1000.f;object[0x428]=i%13==0?0:0x10;object[0x42B]=i%19==0?0x40:0;object[0x960]=uint8((i/7)%5);*reinterpret_cast<uint32*>(object.data()+0x40)=i%11==0?0x08000000:0;*reinterpret_cast<uint16*>(object.data()+0x86C)=i%2?10000:0;current->m_pFlyingHandlingData=reinterpret_cast<tFlyingHandlingData*>(oldHandling.data());current->m_fAccelerationBreakStatus=std::array<float,5>{-.5f,0.f,.25f,.5f,1.f}[(i/5)%5];current->m_fLeftRightSkid=.25f;current->m_fSteeringUpDown=-.125f;current->m_fSteeringLeftRight=.5f;current->m_fPropSpeed=std::array<float,5>{0.f,.001f,.1f,.5f,1.f}[(i/9)%5];current->m_planeDamageWave=std::bit_cast<int32>(uint32(i%2?0xFFFFFFF0:100));
 for(int j=0;j<25;++j){frames[j].modelling.pos={float(j)*.125f,float(j%3)*.25f,-.125f};current->m_aCarNodes[j]=i%4==0&&j%3==0?nullptr:&frames[j];}
 for(int j=0;j<4;++j){*reinterpret_cast<int16*>(object.data()+0x6AC+j*0x20)=int16(16+(i+j)%5);*reinterpret_cast<float*>(object.data()+0x6B8+j*0x20)=float(j-2)*.0625f;*reinterpret_cast<float*>(object.data()+0x6C4+j*0x20)=float(j)*.125f;}
 CTimer::ms_fTimeStep=i%31==0?0.f:i%37==0?-1.f:.5f+float(i%5)*.25f;StaticRef<void*>(0xA9AE20)=reinterpret_cast<void*>(0x12340000);StaticRef<void*>(0xA9B76C)=model.data();model[0x4A]=uint8(i%17);memset(reinterpret_cast<void*>(0xB6F028),0,64);StaticRef<CVector>(0xB6F02C)=CVector{i%2?1000.f:0.f,0.f,0.f};StaticRef<void*>(0xB6F03C)=nullptr;
 hook->State(candidate);const uintptr_t address=reinterpret_cast<uintptr_t*>(0x871948)[68];if((address!=0x6CB7C0)!=candidate)return json{{"switch",false}};reinterpret_cast<void(__thiscall*)(CPlane*)>(address)(current);
 const int handling=current->m_pFlyingHandlingData==reinterpret_cast<tFlyingHandlingData*>(oldHandling.data())?1:current->m_pFlyingHandlingData==reinterpret_cast<tFlyingHandlingData*>(flyingHandling.data())?2:0;current->m_pFlyingHandlingData=nullptr;current->m_matrix=nullptr;std::ranges::fill(current->m_aCarNodes,nullptr);if(!candidate){if(!trace.empty())++positive;totalFlights+=flightCalls;totalParticles+=particleCalls;}
 return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"frames",Hash(frames.data(),sizeof(frames))},{"handling",handling},{"step",std::bit_cast<uint32>(CTimer::ms_fTimeStep)},{"trace",trace}};
 };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",i},{"function","CPlane::ProcessFlyingCarStuff"},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xB6F028),savedCamera.data(),64);CTimer::ms_fTimeStep=savedStep;StaticRef<void*>(0xA9AE20)=savedSmoke;StaticRef<void*>(0xA9B76C)=savedModel;if(!positive||!totalFlights||!totalParticles)++failures;hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function","CPlane::ProcessFlyingCarStuff"},{"cases",1024},{"failures",failures},{"positive_cases",positive},{"flight_calls",totalFlights},{"particle_calls",totalParticles}});Write({{"event",failures?"error":"complete"},{"pairs",1024},{"failures",failures},{"state_restored",hook->Hooked()==enabled&&hook->Locked()==locked}});log.close();
}
}
