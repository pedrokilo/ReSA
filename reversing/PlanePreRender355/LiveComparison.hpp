#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Plane.h"
#include "ColModel.h"
#include "VehicleModelInfo.h"
#include "BouncingPanel.h"
#include "FxSystem.h"
#include "FxManager.h"
#include "FxPrtMult.h"
#include "Shadows.h"
#include "MotionBlurStreaks.h"
#include "../ProbeRuntime.hpp"
namespace PlanePreRender355Live {
inline json trace;inline uint32 scenario{},created{};
inline uintptr_t planeBase{};inline CColModel* collision{};inline uint8* frameBase{};inline uint8* fxBase{};inline RpAtomic* atomic{};
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline json V(const CVector& v){return json::array({Bits(v.x),Bits(v.y),Bits(v.z)});}
inline json M(const void* matrix){const auto* b=static_cast<const uint8*>(matrix);return json::array({V(*reinterpret_cast<const CVector*>(b)),V(*reinterpret_cast<const CVector*>(b+0x10)),V(*reinterpret_cast<const CVector*>(b+0x20)),V(*reinterpret_cast<const CVector*>(b+0x30))});}
inline uint64_t Digest(const void* data,size_t n){uint64_t h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
template<class T=uint32>T& At(void* data,size_t offset){return *reinterpret_cast<T*>(static_cast<uint8*>(data)+offset);}
inline int FrameID(const RwFrame* frame){if(!frame)return -1;const auto p=reinterpret_cast<uintptr_t>(frame),base=reinterpret_cast<uintptr_t>(frameBase);return p>=base&&p<base+25*256?int((p-base)/256):-2;}
inline int FxID(const FxSystem_c* fx){if(!fx)return -1;const auto p=reinterpret_cast<uintptr_t>(fx),base=reinterpret_cast<uintptr_t>(fxBase);return p>=base&&p<base+8*512?int((p-base)/512):-2;}
inline CColModel* __fastcall Col(CEntity*,void*){trace.push_back({"col"});return collision;}
inline void __fastcall Base(CVehicle*,void*){trace.push_back({"base_prerender"});}
inline void __fastcall Suspension(CAutomobile*,void*){trace.push_back({"suspension"});}
inline void __fastcall Hover(CAutomobile*,void*){trace.push_back({"hover"});}
inline void __fastcall Wheel(const CVehicleModelInfo*,void*,int32 wheel,CVector& out,bool local){trace.push_back({"wheel",wheel,local});out=CVector{float(wheel)*.25f,float(wheel)*.125f,.5f};}
inline bool __fastcall OriginalPosition(CVehicleModelInfo*,void*,CVector& out,int32 component){trace.push_back({"original_position",component});out=CVector{.5f,1,1.5f};return scenario%5!=0;}
inline void __fastcall SetFlags(CAutomobile*,void*,int32 node,int32 flags){trace.push_back({"component_flags",node,flags});}
inline void __fastcall Rotation(CVehicle*,void*,RwFrame* frame,eRotationAxis axis,float angle,bool reset){trace.push_back({"rotation",FrameID(frame),int32(axis),Bits(angle),reset});if(frame)At<float>(frame,0x10+0x30+size_t(axis)*4)=angle;}
inline RwFrame* __cdecl Objects(RwFrame* frame,RwObjectCallBack,void* output){trace.push_back({"frame_objects",FrameID(frame)});*static_cast<RpAtomic**>(output)=scenario%3==0?nullptr:atomic;return frame;}
inline void __cdecl Alpha(RpAtomic* object,int32 alpha){trace.push_back({"alpha",alpha});At<int32>(object,4)=alpha;}
inline void __fastcall Dust(CAutomobile*,void*,float time,float height){trace.push_back({"dust",Bits(time),Bits(height)});}
inline FxSystem_c* __fastcall Create(FxManager_c*,void*,const char* name,const CVector& position,RwMatrix* parent,bool ignore){trace.push_back({"create",name,V(position),parent!=nullptr,ignore});const auto ordinal=created++;return scenario%7==0?nullptr:reinterpret_cast<FxSystem_c*>(fxBase+(ordinal%4)*512);}
inline void __fastcall Play(FxSystem_c* fx,void*){trace.push_back({"play",FxID(fx)});if(FxID(fx)>=0)At<uint32>(fx,0)=1;}
inline void __fastcall Kill(FxSystem_c* fx,void*){trace.push_back({"kill",FxID(fx)});if(FxID(fx)>=0)At<uint32>(fx,0)=2;}
inline void __fastcall Local(FxSystem_c* fx,void*,bool local){trace.push_back({"local",FxID(fx),local});if(FxID(fx)>=0)At<uint32>(fx,4)=local;}
inline void __fastcall Copy(FxSystem_c* fx,void*){trace.push_back({"copy_parent",FxID(fx)});}
inline void __fastcall FxMatrix(FxSystem_c* fx,void*,RwMatrix* matrix){trace.push_back({"fx_matrix",FxID(fx),M(matrix)});if(FxID(fx)>=0){for(size_t off: {size_t(0),size_t(0x10),size_t(0x20),size_t(0x30)})At<CVector>(fx,0x20+off)=At<CVector>(matrix,off);}}
inline void __fastcall Offset(FxSystem_c* fx,void*,const CVector& position){trace.push_back({"fx_offset",FxID(fx),V(position)});if(FxID(fx)>=0)At<CVector>(fx,0x70)=position;}
inline void __fastcall Time(FxSystem_c* fx,void*,bool on,float time){trace.push_back({"fx_time",FxID(fx),on,Bits(time)});if(FxID(fx)>=0){At<uint32>(fx,8)=on;At<float>(fx,0xC)=time;}}
inline void __fastcall Particle(FxSystem_c* fx,void*,const CVector& position,const CVector& velocity,float elapsed,const FxPrtMult_c& multiplier,float rotation,float light,float limit,bool local){trace.push_back({"particle",FxID(fx),V(position),V(velocity),Bits(elapsed),Digest(&multiplier,sizeof(multiplier)),Bits(rotation),Bits(light),Bits(limit),local});}
inline void __fastcall Panel(CBouncingPanel* panel,void*,CVehicle*,RwFrame* frame,CVector speed,CVector turn,float multiplier,float limit){trace.push_back({"panel",panel->m_nFrameId,FrameID(frame),V(speed),V(turn),Bits(multiplier),Bits(limit)});panel->m_vecRotation.x+=multiplier;panel->m_vecPos.y+=limit;}
inline void __cdecl Shadow(CVehicle*,VEH_SHD_TYPE type){trace.push_back({"shadow",uint32(type)});}
inline void __cdecl Streak(uint32 id,uint8 red,uint8 green,uint8 blue,uint8 alpha,CVector left,CVector right){trace.push_back({"streak",uint32(id-uint32(planeBase)),red,green,blue,alpha,V(left),V(right)});}
inline void __fastcall Boat(CVehicle*,void*,float damping){trace.push_back({"boat",Bits(damping)});}
inline bool done{};inline std::ofstream log;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,1> fns{{{"CPlane","PreRender",0x6C94A0,0x871948,17,16384}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* found{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();});return found;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"plane-prerender355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"plane-prerender355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plane-prerender355");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;bool passed=true;uint32 pairCount=0;
 using ReversingProbe::EntryPatch;
 EntryPatch col(0x535300,Col),base(0x6D6480,Base),wheel(0x4C7D20,Wheel),position(0x4C7DD0,OriginalPosition),flags(0x6AA290,SetFlags),rotation(0x6DBA30,Rotation),objects(0x7F1200,Objects),alpha(0x6D2960,Alpha),dust(0x6B0690,Dust);
 EntryPatch create(0x4A9BE0,Create),play(0x4AA2F0,Play),kill(0x4AA3F0,Kill),local(0x4AA910,Local),copy(0x4AA890,Copy),fxMatrix(0x4AA630,FxMatrix),offset(0x4AA660,Offset),time(0x4AA6C0,Time),particle(0x4AA440,Particle);
 EntryPatch panel(0x6F49A0,Panel),shadow(0x70BDA0,Shadow),streak(0x721DC0,Streak),boat(0x6DD130,Boat);
 const auto savedStep=CTimer::GetTimeStep();
 constexpr std::array<int16,16> models{460,464,476,511,512,513,519,520,539,553,577,592,2000,463,518,591};
 std::array<uintptr_t,models.size()> savedModels;for(size_t n=0;n<models.size();++n)savedModels[n]=StaticRef<uintptr_t[20000]>(0xA9B0C8)[size_t(models[n])];
 for(const auto& fn:fns){auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);scenario=uint32(i);created=0;trace=json::array();
 alignas(CPlane) std::array<uint8,sizeof(CPlane)> vehicleBytes{};auto* self=reinterpret_cast<CPlane*>(vehicleBytes.data());planeBase=uintptr_t(self);
 alignas(CMatrix) std::array<uint8,sizeof(CMatrix)> matrixBytes{};alignas(16) std::array<uint8,25*256> frames{};alignas(16) std::array<uint8,8*512> effects{};alignas(16) std::array<uint8,256> atomicBytes{},clump{};
 alignas(16) std::array<uint8,512> modelInfo{},modelData{},handling{},flyingHandling{},lineData{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> colModel{};alignas(16) std::array<uint8,128> colData{};
 std::array<uintptr_t,71> vtable;std::memcpy(vtable.data(),reinterpret_cast<void*>(fn.vt),sizeof(vtable));vtable[0xD0/4]=std::bit_cast<uintptr_t>(&Suspension);vtable[0x114/4]=std::bit_cast<uintptr_t>(&Hover);At<uintptr_t>(self,0)=uintptr_t(vtable.data());At<uintptr_t>(self,0x14)=uintptr_t(matrixBytes.data());At<uintptr_t>(self,0x18)=uintptr_t(clump.data());frameBase=frames.data();fxBase=effects.data();atomic=reinterpret_cast<RpAtomic*>(atomicBytes.data());collision=reinterpret_cast<CColModel*>(colModel.data());
 At<CVector>(matrixBytes.data(),0)=CVector{1,0,0};At<CVector>(matrixBytes.data(),0x10)=CVector{0,1,.125f};At<CVector>(matrixBytes.data(),0x20)=CVector{0,0,1};At<CVector>(matrixBytes.data(),0x30)=CVector{4,5,6};
 for(size_t n=0;n<25;++n){auto* frame=frames.data()+n*256;At<CVector>(frame,0x10)=CVector{1,0,0};At<CVector>(frame,0x20)=CVector{0,1,0};At<CVector>(frame,0x30)=CVector{0,0,1};At<CVector>(frame,0x40)=CVector{float(n)*.125f,1.f+float(n)*.25f,2.f+float(n)*.125f};At<uint32>(frame,0x1C)=2;
 const bool exists=i%5!=0||(n%3!=size_t((i/16)%3));At<uintptr_t>(self,0x648+n*4)=exists?uintptr_t(frame):0;}
 const int16 model=models[size_t(i%16)];At<int16>(self,0x22)=model;At<uint8>(self,0x36)=uint8(2+((i/16)%4)*8);At<uint32>(self,0x40)=uint32(i%3==0?0x4000100:0x100);At<uint8>(self,0x42B)=uint8((i%2?1:0)|(i%7==0?0x40:0));At<uint8>(self,0x428)=uint8(i%3?0x10:0);At<uintptr_t>(self,0x384)=uintptr_t(handling.data());At<uintptr_t>(self,0x388)=uintptr_t(flyingHandling.data());At<uint32>(self,0x38C)=i%7==0?0x20000:0;At<float>(handling.data(),0x10)=2;At<float>(handling.data(),0x40)=3;At<float>(handling.data(),0xB8)=.25f;At<float>(flyingHandling.data(),0x34)=i%3==0?.5f:2.f;
 At<uintptr_t>(modelInfo.data(),0x5C)=uintptr_t(modelData.data());for(size_t off:{size_t(0x6C),size_t(0x78),size_t(0x84),size_t(0x90)})At<CVector>(modelData.data(),off)=i%3==0?CVector{0,0,0}:CVector{.25f,1.f,2.f};StaticRef<uintptr_t[20000]>(0xA9B0C8)[size_t(model)]=uintptr_t(modelInfo.data());At<uintptr_t>(colModel.data(),0x2C)=uintptr_t(colData.data());At<uintptr_t>(colData.data(),0x10)=uintptr_t(lineData.data());for(size_t n=0;n<128;++n)At<float>(lineData.data(),n*4)=float(n)*.125f+1.f;
 At<uintptr_t>(clump.data(),4)=uintptr_t(frames.data());
 for(size_t n=0;n<4;++n){At<float>(self,0x7D4+n*4)=float((i/16+n)%5)*.25f;At<float>(self,0x838+n*4)=.25f+float(n)*.125f;At<float>(self,0x828+n*4)=float(n)*.125f;At<float>(self,0x878+n*4)=.5f;At<float>(self,0x888+n*4)=1.f;At<uintptr_t>(self,0x9E8+n*4)=i%3==0?uintptr_t(effects.data()+n*512):0;}
 At<CVector>(self,0x44)=CVector{.125f,float((i/16)%5)*.25f-.5f,.0625f};At<CVector>(self,0x50)=CVector{.03125f,.0625f,.125f};At<CVector>(self,0x5C)=CVector{.015625f,.03125f,.0625f};At<CVector>(self,0x68)=CVector{.0078125f,.015625f,.03125f};for(size_t n=0;n<4;++n)At<CVector>(self,0x734+n*0x2C)=CVector{.125f,1,.25f};
 At<float>(self,0x988)=float((i/16)%7)*.125f-.375f;At<float>(self,0x98C)=float((i/32)%7)*.125f-.375f;At<float>(self,0x990)=float((i/64)%7)*.125f-.375f;At<float>(self,0x994)=float((i/16)%3)*.25f;At<float>(self,0x9C4)=float((i/16)%5)*.5f;At<float>(self,0x9C8)=i%3==0?6.2f:i%3==1?13.f:.125f;At<float>(self,0x9CC)=float(int32((i/32)%7)-3)*.5f;
 At<uint16>(self,0x86C)=uint16((i/16)%5==0?0:(i/16)%5==1?CPlane::HARRIER_NOZZLE_SWITCH_LIMIT-1:(i/16)%5==2?CPlane::HARRIER_NOZZLE_SWITCH_LIMIT:CPlane::HARRIER_NOZZLE_ROTATE_LIMIT);At<uintptr_t>(self,0x580)=i%3==0?uintptr_t(effects.data()+4*512):0;At<float>(self,0x984)=.5f;At<uint8>(self,0xA00)=uint8(i%3!=0);At<float>(self,0x950)=.375f;
 for(size_t n=0;n<3;++n){At<int16>(self,0x6AC+n*0x20)=i%5==0?int16{-1}:int16(16+n);At<CVector>(self,0x6AC+n*0x20+8)=CVector{.125f,.25f,.375f};}At<CVector>(self,0x8B4)=CVector{.125f,.25f,.375f};At<CVector>(self,0x8C0)=CVector{.03125f,.0625f,.125f};
 if(model==520){At<uintptr_t>(self,0x660)=uintptr_t(frames.data()+6*256);At<uintptr_t>(self,0x654)=uintptr_t(frames.data()+3*256);}StaticRef<float>(0xB7CB5C)=i%3==0?.5f:i%3==1?1.f:1.5f;
 const auto target=*reinterpret_cast<uintptr_t*>(fn.vt+size_t(fn.slot)*4);const bool switched=(target!=fn.address)==candidate;reinterpret_cast<void(__thiscall*)(CPlane*)>(target)(self);
 return json{{"vehicle",Digest(vehicleBytes.data(),vehicleBytes.size())},{"matrix",Digest(matrixBytes.data(),matrixBytes.size())},{"frames",Digest(frames.data(),frames.size())},{"effects",Digest(effects.data(),effects.size())},{"atomic",Digest(atomicBytes.data(),atomicBytes.size())},{"prop",Bits(At<float>(self,0x9C8))},{"wheel_heights",json::array({Bits(At<float>(self,0x838)),Bits(At<float>(self,0x83C)),Bits(At<float>(self,0x840)),Bits(At<float>(self,0x844))})},{"trace",trace},{"switches",switched}};
 };const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",pairCount++},{"function",full},{"case",i},{"model",models[size_t(i%16)]},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }StaticRef<float>(0xB7CB5C)=savedStep;for(size_t n=0;n<models.size();++n)StaticRef<uintptr_t[20000]>(0xA9B0C8)[size_t(models[n])]=savedModels[n];Write({{"event",passed?"complete":"error"},{"pairs",pairCount},{"state_restored",true}});
}
}
