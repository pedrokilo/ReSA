#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Fx/FxEmitterBP.h"
#include "Fx/FxEmitterPrt.h"
#include "Fx/RenderInfo.h"
#include "Fx/Fx.h"
#include "Fx/FxPrtMult.h"
#include "../ProbeRuntime.hpp"
namespace FxEmitterRender355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},matrixIndex{};
alignas(16)inline std::array<uint8,sizeof(FxEmitterBP_c)>object;
inline std::array<FxEmitterPrt_c,2>particles;
alignas(16)inline std::array<uint8,sizeof(FxSystem_c)>system;
alignas(16)inline std::array<uint8,sizeof(FxSystemBP_c)>systemBP;
inline std::array<RwMatrix,4>matrices;
inline RwCamera camera;inline RwFrame frame;inline std::array<RwTexture,4>textures;inline std::array<RwRaster,4>rasters;
inline std::array<uint8,12>infoItem;inline std::array<void*,1>infos;
inline uint32 B(float x){return std::bit_cast<uint32>(x);}inline json V(const RwV3d&v){return json::array({B(v.x),B(v.y),B(v.z)});}
inline json Ptr(const void*p){if(!p)return nullptr;if(p==object.data())return "emitter";if(p==system.data())return "system";for(int i=0;i<4;++i){if(p==&matrices[i])return json::array({"matrix",i});if(p==&rasters[i])return json::array({"raster",i});}return uintptr_t(p);}
inline json Words(const void*p,size_t n){json j=json::array();for(size_t i=0;i<n/4;++i)j.push_back(static_cast<const uint32*>(p)[i]);return j;}
inline void Write(const json&j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline RwBool __cdecl State(RwRenderState state,void*value){trace.push_back({"state",int(state),uintptr_t(value)});return TRUE;}
inline void __cdecl Begin(RwRaster*r,RwMatrix*m,uint32 flags){trace.push_back({"begin",Ptr(r),Ptr(m),flags});}
inline void __cdecl End(){trace.push_back({"end"});}
inline void __cdecl Triangle(float x1,float y1,float z1,float x2,float y2,float z2,float x3,float y3,float z3,float u1,float v1,float u2,float v2,float u3,float v3,int r1,int g1,int b1,int a1,int r2,int g2,int b2,int a2,int r3,int g3,int b3,int a3){trace.push_back({"triangle",json::array({B(x1),B(y1),B(z1),B(x2),B(y2),B(z2),B(x3),B(y3),B(z3),B(u1),B(v1),B(u2),B(v2),B(u3),B(v3)}),json::array({r1,g1,b1,a1,r2,g2,b2,a2,r3,g3,b3,a3})});}
inline RwMatrix* __fastcall Create(FxManager_c*,void*){auto*p=&matrices[matrixIndex++%4];trace.push_back({"matrix_new",Ptr(p)});return p;}
inline void __fastcall Destroy(FxManager_c*,void*,RwMatrix*p){trace.push_back({"matrix_free",Ptr(p)});}
inline void __fastcall Composite(const FxSystem_c*,void*,RwMatrix*m){RwMatrixSetIdentity(m);m->pos={.5f,1.f,1.5f};trace.push_back({"composite",Ptr(m)});}
inline RwV3d* __cdecl Transform(RwV3d*out,const RwV3d*in,RwInt32 n,const RwMatrix*m){trace.push_back({"transform",V(*in),n,Ptr(m)});for(int i=0;i<n;++i)out[i]=RwV3d{in[i].x+m->pos.x,in[i].y+m->pos.y,in[i].z+m->pos.z};return out;}
inline RwReal __cdecl Normalize(RwV3d*out,const RwV3d*in){trace.push_back({"normalise",V(*in)});auto length=std::sqrt((in->x*in->x+in->y*in->y)+in->z*in->z);if(length==0.f){*out={0.f,0.f,0.f};return 0.f;}*out={in->x/length,in->y/length,in->z/length};return length;}
inline RwReal __cdecl Length(const RwV3d*p){trace.push_back({"length",V(*p)});return std::sqrt((p->x*p->x+p->y*p->y)+p->z*p->z);}
inline void __cdecl Rotate(RwV3d&out,const RwV3d&in,const RwV3d&axis,float angle){trace.push_back({"rotate",V(in),V(axis),B(angle)});out=RwV3d{-in.y,in.x,in.z};}
inline int __cdecl Random(){trace.push_back({"rand"});return (scenario*701)%32768;}
inline void __fastcall Add(FxSystem_c*s,void*,const CVector&p,const CVector&v,float age,const FxPrtMult_c&m,float rotation,float light,float limit,bool local){trace.push_back({"smoke",Ptr(s),V(p),V(v),B(age),Words(&m,sizeof(m)),B(rotation),B(light),B(limit),local});}
inline void __fastcall RenderInfo(FxInfoManager_c*,void*,float now,float age,float time,float length,bool constant,RenderInfo_t*out){trace.push_back({"info",B(now),B(age),B(time),B(length),constant});std::memset(out,0,sizeof(*out));out->Process();out->m_Color1={120,160,200,192};out->m_Color2={200,100,80,32};out->m_nColorType=static_cast<ERenderColorType>((scenario/16)%3);out->m_fSizeX=1.f;out->m_fSizeY=.5f;out->m_fSizeXBias=.25f;out->m_fSizeYBias=.125f;out->m_bHasAnimTextures=(scenario&4)!=0;out->m_nCurrentTexId=int8((scenario/8)%4+1);out->m_bIsFlat=(scenario&16)!=0;out->m_bHasDir=(scenario&32)!=0;out->m_bUseVel=(scenario&64)!=0;out->m_bSelfLit=(scenario&128)!=0;out->m_Direction=CVector{0.f,1.f,0.f};out->m_nTrailScreenMode=(scenario/256)%3;out->m_fTrailTime=.5f;out->m_SmokeType=(scenario&512)?0:-1;out->m_SmokeColor={120.f,100.f,80.f,128.f};out->m_SmokeBrightness=10.f;out->m_SmokeSize=.5f;out->m_SmokeLife=1.f;RwMatrixSetIdentity(&out->m_FlatMatrix);out->m_FlatMatrix.right={0.f,1.f,0.f};out->m_FlatMatrix.up={1.f,0.f,0.f};}
struct Fn{const char*cls;const char*name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"FxEmitterBP_c","RenderHeatHaze",0x4A1940,0,0,1024},{"FxEmitterBP_c","Render",0x4A2C40,0x85A788,5,2048}};
inline auto*FindHook(const char*n){ReversibleHooks::ReversibleHook::Base*r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&cat){if(cat.Name()=="FxEmitterBP_c")for(auto&h:cat.Items())if(h->Name()==n)r=h.get();});return r;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"fx-emitter-render355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"fx-emitter-render355-runtime.jsonl");ReversingProbe::InstallFailureCapture("fx-emitter-render355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;bool pass=true;int total=0;
for(auto&fn:fns){auto*h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto hs=h->Hooked(),hl=h->Locked();h->LockState(false);int failures=0;json coverage=json::object();{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch be(0x4A13B0,Begin),bd(bit_cast<uintptr_t>(&RenderBegin),Begin),ee(0x4A1600,End),ed(bit_cast<uintptr_t>(&RenderEnd),End),te(0x4A1410,Triangle),td(bit_cast<uintptr_t>(&RenderAddTri_),Triangle);
EntryPatch ce(0x4A9440,Create),cd(bit_cast<uintptr_t>(&FxManager_c::FxRwMatrixCreate),Create),de(0x4A9460,Destroy),dd(bit_cast<uintptr_t>(&FxManager_c::FxRwMatrixDestroy),Destroy);
EntryPatch cme(0x4AA8C0,Composite),cmd(bit_cast<uintptr_t>(&FxSystem_c::GetCompositeMatrix),Composite),tfe(0x7EDD90,Transform),tfd(bit_cast<uintptr_t>(&RwV3dTransformPoints),Transform);
EntryPatch ne(0x7ED9B0,Normalize),nd(bit_cast<uintptr_t>(&RwV3dNormalize),Normalize),lne(0x7EDAC0,Length),lnd(bit_cast<uintptr_t>(&RwV3dLength),Length);
EntryPatch ro(0x4A1780,Rotate),rod(bit_cast<uintptr_t>(&RotateVecAboutVec),Rotate),rne(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
EntryPatch ie(0x4A4A80,RenderInfo),id(bit_cast<uintptr_t>(&FxInfoManager_c::ProcessRenderInfo),RenderInfo);
EntryPatch ae(0x4AA440,Add),ad(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),Add);
auto* engine=*reinterpret_cast<uintptr_t**>(0xC97B24);auto savedState=engine[8];engine[8]=bit_cast<uintptr_t>(&State);auto savedHeat=g_fxMan.m_bHeatHazeEnabled;auto savedSmoke=g_fx.m_SmokeHuge;g_fx.m_SmokeHuge=reinterpret_cast<FxSystem_c*>(system.data());auto savedStep=CTimer::ms_fTimeStep;auto savedPrev=*reinterpret_cast<CVector*>(0xB6FA14);
for(int i=0;i<fn.cases;++i){scenario=i;json results[2];for(int candidate=0;candidate<2;++candidate){h->State(candidate!=0);bool switches=fn.slot==0?ReversingProbe::JumpsToDll(fn.address)==(candidate!=0):(reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]!=fn.address)==(candidate!=0);pass&=switches;object.fill(0);system.fill(0);systemBP.fill(0);std::memset(particles.data(),0,sizeof(particles));std::memset(&camera,0,sizeof(camera));std::memset(&frame,0,sizeof(frame));std::memset(textures.data(),0,sizeof(textures));std::memset(rasters.data(),0,sizeof(rasters));std::memset(matrices.data(),0xCD,sizeof(matrices));infoItem.fill(0);trace=json::array();matrixIndex=0;g_fxMan.m_bHeatHazeEnabled=false;CTimer::ms_fTimeStep=1.f;*reinterpret_cast<CVector*>(0xB6FA14)=CVector{0.f,.5f,0.f};auto*self=reinterpret_cast<FxEmitterBP_c*>(object.data());auto*s=reinterpret_cast<FxSystem_c*>(system.data());auto*bp=reinterpret_cast<FxSystemBP_c*>(systemBP.data());s->m_SystemBP=bp;s->m_fCurrentTime=.5f;bp->m_fLength=2.f;self->m_Particles.RemoveAll();self->m_bAlphaOn=(i&1)!=0;self->m_nSrcBlendId=uint8(i%11);self->m_nDstBlendId=uint8((i/11)%11);self->m_FxInfoManager.m_bHasHeatHazeParticleEmitter=(i&64)!=0;self->m_FxInfoManager.m_nNumInfos=(i&1024)?1:0;self->m_FxInfoManager.m_pInfos=reinterpret_cast<FxInfo_c**>(infos.data());infos[0]=infoItem.data();uint16 heatType=0x4008;std::memcpy(infoItem.data()+4,&heatType,2);camera.object.object.parent=&frame;RwMatrixSetIdentity(&frame.modelling);for(int k=0;k<4;++k){textures[k].raster=&rasters[k];self->m_apTextures[k]=(k&&(i&8))?nullptr:&textures[k];}
for(int k=0;k<i%3;++k){auto&p=particles[k];p.m_System=s;p.m_fTotalLife=2.f;p.m_fCurrentLife=.5f;p.m_Pos=CVector{float(k)+1.f,2.f,3.f};p.m_Velocity=CVector{.5f,1.f,.25f};auto*bytes=reinterpret_cast<uint8*>(&p);bytes[0x2C]=uint8((i%4)?255:128);bytes[0x2D]=200;bytes[0x2E]=128;bytes[0x2F]=192;bytes[0x30]=uint8((i&4)?128:255);bytes[0x32]=uint8(i%256);bytes[0x33]=uint8((i*3)%256);bytes[0x34]=uint8((i*7)%256);bytes[0x35]=uint8((i%3)?100:101);bytes[0x36]=uint8((i&16)?255:20);p.m_bLocalToSystem=(i&2)!=0;p.m_CurrentRotation=float((i/32)%5)*180.f-360.f;self->m_Particles.AppendItem(&p);}
if(fn.slot==0)reinterpret_cast<void(__thiscall*)(FxEmitterBP_c*,RwCamera*,uint32,float)>(fn.address)(self,&camera,0x1234,.75f);else{auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];reinterpret_cast<void(__thiscall*)(FxEmitterBP_c*,RwCamera*,uint32,float,bool)>(address)(self,&camera,0x1234,.75f,(i&128)!=0);}
if(candidate)for(auto&t:trace){auto n=t[0].get<std::string>();coverage[n]=coverage.value(n,0)+1;}
results[candidate]={{"switch",switches},{"object",Words(object.data()+4,object.size()-4)},{"particles",Words(particles.data(),sizeof(particles))},{"system",Words(system.data(),system.size())},{"matrix_arena",Words(matrices.data(),sizeof(matrices))},{"heat",g_fxMan.m_bHeatHazeEnabled},{"trace",trace}};
}bool equal=results[0]==results[1];pass&=equal;failures+=!equal;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"pair",total++},{"equal",equal},{"accepted",equal},{"original",results[0]},{"candidate",results[1]}});}
engine[8]=savedState;g_fxMan.m_bHeatHazeEnabled=savedHeat;g_fx.m_SmokeHuge=savedSmoke;CTimer::ms_fTimeStep=savedStep;*reinterpret_cast<CVector*>(0xB6FA14)=savedPrev;
}h->State(hs);h->LockState(hl);for(auto n:{"triangle","info","begin","end","rotate","composite"})if(!coverage.value(n,0)){pass=false;Write({{"event","error"},{"function",std::string(fn.cls)+"::"+fn.name},{"reason","missing coverage"},{"branch",n}});}Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"coverage",coverage}});}
Write({{"event",pass?"complete":"error"},{"pairs",total},{"state_restored",true}});}
}
