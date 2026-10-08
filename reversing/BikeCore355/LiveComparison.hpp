#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Bike.h"
#include "SurfaceInfos_c.h"
#include "../ProbeRuntime.hpp"
namespace BikeCore355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},associationCount{};
alignas(16) inline std::array<std::array<uint8,sizeof(CAnimBlendAssociation)>,5> associations{};
alignas(16) inline std::array<uint8,0x80> matrix{},clump{};alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};alignas(16) inline std::array<uint8,sizeof(CColModel)> col{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline CAnimBlendAssociation* __cdecl First(RpClump*,uint32 flags){trace.push_back({"first",flags});return associationCount?reinterpret_cast<CAnimBlendAssociation*>(associations[0].data()):nullptr;}
inline CAnimBlendAssociation* __cdecl Next(CAnimBlendAssociation* item,uint32 flags){const auto index=int32((reinterpret_cast<uint8*>(item)-associations[0].data())/sizeof(CAnimBlendAssociation));trace.push_back({"next",index,flags});return index+1<associationCount?reinterpret_cast<CAnimBlendAssociation*>(associations[index+1].data()):nullptr;}
inline int32 __fastcall Adhesion(void*,void*,SurfaceId id){trace.push_back({"adhesion",int(id)});return int(id)%7==4?4:2;}
inline void __fastcall Force(void*,void*,CVector force){trace.push_back({"force",std::bit_cast<uint32>(force.x),std::bit_cast<uint32>(force.y),std::bit_cast<uint32>(force.z)});}
inline CColModel* __fastcall Col(void*,void*){trace.push_back({"col"});return reinterpret_cast<CColModel*>(col.data());}
inline int32 __cdecl Random(){trace.push_back({"random"});return scenario*43+7;}
inline int16 __fastcall Say(void*,void*,eGlobalSpeechContext ctx,uint32 delay,float probability,bool overrideSilence,bool audible,bool frontend){trace.push_back({"say",int(ctx),delay,std::bit_cast<uint32>(probability),overrideSilence,audible,frontend});return 1;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,5> fns{{{"CBike","ResetSuspension",0x6B6740,0,-1,1024},{"CBike","SetRemoveAnimFlags",0x6B5F50,0,-1,1024},{"CBike","DoSoftGroundResistance",0x6B6D40,0,-1,1024},{"CBike","GetCorrectedWorldDoorPosition",0x6BF230,0,-1,1024},{"CBike","PlayCarHorn",0,0x871360,54,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-core355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-core355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-core355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-core355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,5> hooks{};std::array<bool,5> enabled{},locked{};
 for(size_t n=0;n<5;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 const auto oldStep=CTimer::GetTimeStep();uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch a(0x4D6A70,First),ad(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation)),First);
 EntryPatch b(0x4D6AD0,Next),bd(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(CAnimBlendAssociation*,uint32)>(&RpAnimBlendGetNextAssociation)),Next);
 EntryPatch c(0x55E5C0,Adhesion),cd(bit_cast<uintptr_t>(&SurfaceInfos_c::GetAdhesionGroup),Adhesion);
 EntryPatch d(0x5429F0,Force),dd(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),Force);
 EntryPatch e(0x535300,Col),ed(bit_cast<uintptr_t>(&CEntity::GetColModel),Col);
 EntryPatch r(0x821B1E,Random),rd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch s(0x5EFFE0,Say),sd(bit_cast<uintptr_t>(&CPed::Say),Say);
 for(size_t n=0;n<5;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};CVector output{-123.f,-124.f,-125.f};uint32 flags=uint32(i)*0x1020304u;
   auto run=[&](bool candidate){scenario=i;associationCount=i%6;trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(ped.data(),0,ped.size());memset(clump.data(),0,clump.size());memset(col.data(),0,col.size());memset(associations.data(),0xCD,sizeof(associations));output=CVector{-123.f,-124.f,-125.f};flags=uint32(i)*0x1020304u;
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(object.data())=0x871360;*reinterpret_cast<uintptr_t*>(object.data()+0x14)=uintptr_t(matrix.data());
    *reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{float(i%9-4)*.25f,1.f,float(i%7-3)*.125f};*reinterpret_cast<CVector*>(matrix.data()+0x20)=CVector{.125f,float(i%3)*.25f,1.f};*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{10.f,-20.f,30.f};
    *reinterpret_cast<uintptr_t*>(ped.data()+0x18)=uintptr_t(clump.data());ped[0x36]=uint8(i%7==0?2:3);clump[0]=2;for(int aidx=0;aidx<5;++aidx)*reinterpret_cast<uint16*>(associations[aidx].data()+0x2E)=uint16(i+aidx*7);
    self->m_vecMoveSpeed=CVector{float(i%13-6)*.0625f,float((i/3)%9-4)*.0625f,float(i%7-3)*.0625f};self->m_fMass=100.f+float(i%11)*25.f;self->m_GasPedal=float(i%7)*.125f;CTimer::ms_fTimeStep=float(i%5)*.25f;
    for(int w=0;w<4;++w){self->m_aWheelRatios[w]=float((i+w)%7)*.25f;self->m_aWheelColPoints[w].m_nSurfaceTypeB=static_cast<eSurfaceType>((i/7+w)%5==0?178:(i+w)%13);}
    self->m_nAlarmState=uint16(i%3==0?0:i%3==1?0xFFFF:2);self->SetStatus((i/3)%2?STATUS_WRECKED:STATUS_PHYSICS);self->m_HornCounter=i%13==0?4:0;self->m_HornPattern=int8(i%7==0?2:0);self->m_nCarHornTimer=char(std::array<uint8,4>{0,2,0x80,0xFF}[(i/2)%4]);self->m_pDriver=(i/7)%2?reinterpret_cast<CPed*>(ped.data()):nullptr;self->m_autoPilot.carCtrlFlags.bHonkAtCar=(i/11)%2;
    reinterpret_cast<CColModel*>(col.data())->m_boundBox.m_vecMax=CVector{float(i%5-2),3.f,float(i%7-2)};
    hooks[n]->State(candidate);const auto address=fn.address?fn.address:reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];const bool switched=fn.address?ReversingProbe::JumpsToDll(address)==candidate:(address!=0x6B7080)==candidate;if(!switched)return json{{"switch",false}};
    if(n==1)reinterpret_cast<void(__thiscall*)(void*,CPed*)>(address)(self,reinterpret_cast<CPed*>(ped.data()));
    else if(n==2)reinterpret_cast<void(__thiscall*)(void*,uint32&)>(address)(self,flags);
    else if(n==3)reinterpret_cast<void(__thiscall*)(void*,CVector&,CVector,CVector)>(address)(self,output,CVector{float(i%9)*.125f,-.5f,1.f},CVector{-.25f,float(i%11)*.25f,2.f});
    else reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"ped",Hash(ped.data(),ped.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"col",Hash(col.data(),col.size())},{"associations",Hash(associations.data(),sizeof(associations))},{"output",{std::bit_cast<uint32>(output.x),std::bit_cast<uint32>(output.y),std::bit_cast<uint32>(output.z)}},{"flags",flags},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(n&&!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 CTimer::ms_fTimeStep=oldStep;bool restored=CTimer::GetTimeStep()==oldStep;
 for(size_t n=0;n<5;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}


