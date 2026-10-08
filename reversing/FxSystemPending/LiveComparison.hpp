#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Fx/FxSystem.h"
#include "Fx/FxSystemBP.h"
#include "Fx/FxPrim.h"
#include "Fx/Particle.h"
#include "Fx/FxBox.h"
#include "Fx/FxPrtMult.h"
#include "../ProbeRuntime.hpp"
namespace FxSystemPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,4> fns{{
 {"FxSystem_c","AddParticle-v3d",0x4AA440,0,-1,1024},
 {"FxSystem_c","AddParticle-mat",0x4AA540,0,-1,1024},
 {"FxSystem_c","UpdateBoundingBoxCB",0x4AA9A0,0,-1,1024},
 {"FxSystem_c","Update",0x4AAF70,0,-1,2048},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}


inline RwMatrix matrix{};inline int rng{},quality{};inline bool visible{};
alignas(16) inline std::array<std::array<uint8,sizeof(FxPrim_c)>,8> prims{};inline std::array<FxPrim_c*,8> pointers{};inline std::array<uintptr_t,6> vt{};
inline int ID(void* p){return int((uintptr_t(p)-uintptr_t(prims.data()))/sizeof(prims[0]));}
inline int __cdecl Random(){trace.push_back({"rand",rng});return rng;}
inline FxQuality_e __fastcall Quality(const Fx_c*,void*){trace.push_back({"quality",quality});return FxQuality_e(quality);}
inline RwMatrix* __fastcall Create(FxManager_c*,void*){trace.push_back("matrix_new");return &matrix;}
inline void __fastcall Destroy(FxManager_c*,void*,RwMatrix* m){trace.push_back({"matrix_free",m==&matrix});}
inline bool __fastcall Visible(const FxSystem_c*,void*){trace.push_back({"visible",visible});return visible;}
inline void __fastcall Audio(FxSystem_c*,void*,CVector p){trace.push_back({"audio",F(p.x),F(p.y),F(p.z)});}
inline void __fastcall Stop(FxSystem_c* self,void*){trace.push_back("stop");self->m_nPlayStatus=eFxSystemPlayStatus::FX_STOPPED;self->m_fCurrentTime=0;}
inline void __fastcall Play(FxSystem_c* self,void*){trace.push_back("play");self->m_nPlayStatus=eFxSystemPlayStatus::FX_PLAYING;self->m_prevCulled=false;}
inline void __fastcall PrimUpdate(FxPrim_c* p,void*,float time,float delta){trace.push_back({"update",ID(p),F(time),F(delta)});}
inline void __fastcall AddMat(FxPrim_c* p,void*,const RwMatrix& mat,const CVector& vel,float time,const FxPrtMult_c& mult,float rot,float brightness,bool local){trace.push_back({"mat",ID(p),ReversingProbe::Bytes(&mat,sizeof(mat)),ReversingProbe::Bytes(&vel,sizeof(vel)),F(time),ReversingProbe::Bytes(&mult,sizeof(mult)),F(rot),F(brightness),local});}
inline void __fastcall AddVec(FxPrim_c* p,void*,const CVector& pos,const CVector& vel,float time,const FxPrtMult_c& mult,float rot,float brightness,bool local){trace.push_back({"vec",ID(p),ReversingProbe::Bytes(&pos,sizeof(pos)),ReversingProbe::Bytes(&vel,sizeof(vel)),F(time),ReversingProbe::Bytes(&mult,sizeof(mult)),F(rot),F(brightness),local});}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"fx-system-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"fx-system-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fx-system-pending");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;bool passed=true;uint32 pairs=0;
 {
 using ReversingProbe::EntryPatch;
 EntryPatch e0(0x821B1E,Random),d0(std::bit_cast<uintptr_t>(&rand),Random),d00(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random),e1(0x49EA50,Quality),d1(std::bit_cast<uintptr_t>(&Fx_c::GetFxQuality),Quality),e2(0x4A9440,Create),d2(std::bit_cast<uintptr_t>(&FxManager_c::FxRwMatrixCreate),Create),e3(0x4A9460,Destroy),d3(std::bit_cast<uintptr_t>(&FxManager_c::FxRwMatrixDestroy),Destroy),e4(0x4AAF30,Visible),d4(std::bit_cast<uintptr_t>(&FxSystem_c::IsVisible),Visible),e5(0x4AAC90,Audio),d5(std::bit_cast<uintptr_t>(&FxSystem_c::DoFxAudio),Audio),e6(0x4AA390,Stop),d6(std::bit_cast<uintptr_t>(&FxSystem_c::Stop),Stop),e7(0x4AA2F0,Play),d7(std::bit_cast<uintptr_t>(&FxSystem_c::Play),Play);
 vt.fill(uintptr_t(&PrimUpdate));vt[2]=uintptr_t(&PrimUpdate);vt[4]=uintptr_t(&AddMat);vt[5]=uintptr_t(&AddVec);
 for(size_t f=0;f<fns.size();++f){const auto& fn=fns[f];auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0,active=0,empty=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);trace=json::array();rng=(i*37)%32768;quality=i%4;visible=i%3!=0;
 alignas(16) std::array<uint8,sizeof(FxSystem_c)> raw{};alignas(16) std::array<uint8,sizeof(FxSystemBP_c)> bpraw{};auto* self=reinterpret_cast<FxSystem_c*>(raw.data());auto* bp=reinterpret_cast<FxSystemBP_c*>(bpraw.data());RwMatrix parent{};RwMatrixSetIdentity(&parent);parent.pos={float(i%9),float(i%11),float(i%5)};matrix={};RwMatrixSetIdentity(&self->m_LocalMatrix);self->m_LocalMatrix.pos={float(i%13),float(i%7),float(i%3)};self->m_ParentMatrix=i%2?&parent:nullptr;
 self->m_SystemBP=bp;self->m_Prims=pointers.data();bp->m_nNumPrims=i%17==0?255:uint8(i%9);bp->m_nPlayMode=uint8(i%5);bp->m_nCullDist=uint16((i%13)*256);bp->m_fLength=float(i%7)*0.5f;bp->m_fLoopIntervalMin=i%3==0?0.0f:0.75f;bp->m_fLoopLength=1.5f;
 self->m_nPlayStatus=eFxSystemPlayStatus((i/5)%3);self->m_nKillStatus=eFxSystemKillStatus((i/15)%4);self->m_fCurrentTime=float(i%19)*0.25f;self->m_fCameraDistance=float(i%9)*2.0f;self->m_UseConstTime=i%4==0;self->m_nConstTime=uint16(i%1200);self->m_nTimeMult=uint16((i%6)*500);self->m_LoopInterval=0.5f;self->m_prevCulled=i%2;self->m_stopParticleCreation=i%3==0;
 for(int k=0;k<8;++k){prims[k].fill(0);pointers[k]=reinterpret_cast<FxPrim_c*>(prims[k].data());At<uintptr_t>(prims[k].data(),0)=uintptr_t(vt.data());pointers[k]->m_bEnabled=(i+k)%3!=0;}
 RwCamera camera{};RwFrame frame{};frame.root=&frame;camera.object.object.parent=&frame;RwMatrixSetIdentity(&frame.ltm);frame.ltm.pos={float(i%17),float(i%23),float(i%11)};
 CVector pos{float(i%11),float(i%7),float(i%5)},vel{0.25f,-0.5f,0.75f};alignas(16) std::array<uint8,sizeof(FxPrtMult_c)> mult{};mult.fill(uint8(i));auto& fxmult=*reinterpret_cast<FxPrtMult_c*>(mult.data());const float light=float(i%7)*0.25f,limit=float(i%5)*0.25f;
 Particle_c particle{};particle.m_Pos=pos;FxBox_c box;box.minX=box.minY=box.minZ=float(i%5);box.maxX=box.maxY=box.maxZ=float(i%7);if(i%13==0)particle.m_Pos.x=std::bit_cast<float>(0x7fc01234u);auto* boxptr=&box;bool result=false;const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
 if(f==0)reinterpret_cast<void(__thiscall*)(FxSystem_c*,const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(fn.address)(self,pos,vel,0.125f,fxmult,-1.0f,light,limit,i%2!=0);
 else if(f==1)reinterpret_cast<void(__thiscall*)(FxSystem_c*,const RwMatrix&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(fn.address)(self,parent,vel,0.125f,fxmult,-1.0f,light,limit,i%2!=0);
 else if(f==2)reinterpret_cast<void(__cdecl*)(Particle_c*,int32,FxBox_c**)>(fn.address)(&particle,i%5==0?1:0,&boxptr);
 else result=reinterpret_cast<bool(__thiscall*)(FxSystem_c*,RwCamera*,float)>(fn.address)(self,&camera,float(i%7)*0.125f);
 return json{{"system",ReversingProbe::Bytes(raw.data(),raw.size())},{"bp",ReversingProbe::Bytes(bpraw.data(),bpraw.size())},{"matrix",ReversingProbe::Bytes(&matrix,sizeof(matrix))},{"prims",ReversingProbe::Bytes(prims.data(),sizeof(prims))},{"box",ReversingProbe::Bytes(&box,sizeof(box))},{"result",result},{"trace",trace},{"switches",switched}};
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;active+=original["trace"].size()>2;empty+=original["trace"].size()<=2;
 Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases},{"active",active},{"empty",empty}});
 }
 }
 Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
