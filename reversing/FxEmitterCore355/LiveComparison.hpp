#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Fx/FxEmitterBP.h"
#include "Fx/FxEmitter.h"
#include "Fx/FxEmitterPrt.h"
#include "../ProbeRuntime.hpp"
namespace FxEmitterCore355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{};
inline FxEmitter_c prototype;alignas(16) inline std::array<uint8,sizeof(FxEmitterBP_c)> object{};
alignas(16) inline std::array<uint8,sizeof(FxEmitter_c)> arena{};
alignas(16) inline std::array<FxEmitterPrt_c,8> particles{};
alignas(16) inline std::array<std::array<uint8,sizeof(FxSystem_c)>,3> systems{};
alignas(16) inline std::array<uint8,4096> fxPool{};
inline size_t allocated{};
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline json Ptr(const void* p){if(!p)return nullptr;if(p==arena.data())return "arena";for(int i=0;i<8;++i)if(p==&particles[i])return json::array({"particle",i});for(int i=0;i<3;++i)if(p==systems[i].data())return json::array({"system",i});return uintptr_t(p);}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline void* __cdecl New(size_t n){allocated=n;return scenario%7==0?nullptr:arena.data();}
inline bool __fastcall Particle(FxEmitterBP_c*,void*,float dt,FxEmitterPrt_c* p){const bool b=(scenario+int(p-particles.data()))%3==0;trace.push_back({"particle",Ptr(p),std::bit_cast<uint32>(dt),b});return b;}
inline void __fastcall Return(FxManager_c*,void*,FxEmitterPrt_c* p){trace.push_back({"return",Ptr(p)});p->m_pNext=reinterpret_cast<Particle_c*>(uintptr_t(0x11111111));p->m_pPrev=reinterpret_cast<Particle_c*>(uintptr_t(0x22222222));}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"FxEmitterBP_c","CreateInstance",0,0x85A788,3,1024},
{"FxEmitterBP_c","Update",0,0x85A788,4,2048},
{"FxEmitterBP_c","FreePrtFromPrim",0,0x85A788,6,2048},
{"FxEmitterBP_c","Load",0,0x85A788,1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="FxEmitterBP_c")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"fx-emitter-core355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"fx-emitter-core355-runtime.jsonl");ReversingProbe::InstallFailureCapture("fx-emitter-core355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
const auto poolSaved=g_fxMan.GetMemPool();int pairs=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch pe(0x4A21D0,Particle),pd(bit_cast<uintptr_t>(&FxEmitterBP_c::UpdateParticle),Particle);
EntryPatch re(0x4A93B0,Return),rd(bit_cast<uintptr_t>(&FxManager_c::ReturnParticle),Return);
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,positive=0,zero=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();allocated=0;object.fill(0xCD);arena.fill(0xCD);std::memset(particles.data(),0,sizeof(particles));for(auto& s:systems)s.fill(0);fxPool.fill(0xCD);auto* self=reinterpret_cast<FxEmitterBP_c*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x85A788;self->m_Particles.RemoveAll();for(int k=0;k<3;++k){auto* s=reinterpret_cast<FxSystem_c*>(systems[k].data());s->m_nKillStatus=static_cast<eFxSystemKillStatus>((i+k)%4);s->m_nPlayStatus=static_cast<eFxSystemPlayStatus>((i/4+k)%3);}for(int k=0;k<i%9;++k){particles[k].m_System=reinterpret_cast<FxSystem_c*>(systems[(i+k)%3].data());self->m_Particles.AppendItem(&particles[k]);}if(fn.slot==4&&i%17==0)*reinterpret_cast<size_t*>(object.data()+0x28)=0;
h->State(candidate);const auto addr=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];constexpr uintptr_t originals[]{0x4A2B40,0x4A2BC0,0x4A2510,0x5C25F0};const int n=fn.slot==3?0:fn.slot==4?1:fn.slot==6?2:3;const bool switched=(addr!=originals[n])==candidate;bool ret=false;void* made{};int consumed=-1;std::array<uint8,160> names{};names.fill(0xA5);
if(fn.slot==3){using NewFn=void*(*)(size_t);{EntryPatch ne(0x82119A,New),nd(bit_cast<uintptr_t>(static_cast<NewFn>(&::operator new)),New);made=reinterpret_cast<void*(__thiscall*)(FxEmitterBP_c*)>(addr)(self);}trace.push_back({"new",allocated,Ptr(made)});}
else if(fn.slot==4)reinterpret_cast<void(__thiscall*)(FxEmitterBP_c*,float)>(addr)(self,float(i%11-5)*.125f);
else if(fn.slot==6)ret=reinterpret_cast<bool(__thiscall*)(FxEmitterBP_c*,FxSystem_c*)>(addr)(self,i%5==0?nullptr:reinterpret_cast<FxSystem_c*>(systems[(i/8)%3].data()));
else {g_fxMan.GetMemPool().m_pData=fxPool.data();g_fxMan.GetMemPool().m_nSize=fxPool.size();g_fxMan.GetMemPool().m_nPosition=i%8;int version=std::array{0,100,101,102,103,104,109,110}[i%8];std::string body="FX_PRIM_BASE_DATA:\nNAME: prueba\nMATRIX: 1 0 0 0 1 0 0 0 1 0 0 0\nTEXTURE: principal\n";if(version>101)body+="TEXTURE2: secundaria\nTEXTURE3: NULL\nTEXTURE4: cuarta\n";body+=std::format("ALPHAON: {}\nSRCBLENDID: {}\nDSTBLENDID: {}\n\nNUM_INFOS: 0\nLODSTART: {:.8f}\nLODEND: {:.8f}\nEND_PROBE\n",i%4,i%11,(i/11)%11,float(i%33-16)*.125f,float(i%1025)*.25f);FILE* file{};tmpfile_s(&file);if(!file)return json{{"file_error",true}};fwrite(body.data(),1,body.size(),file);rewind(file);ret=reinterpret_cast<bool(__thiscall*)(FxEmitterBP_c*,FILESTREAM,int,FxName32_t*)>(addr)(self,file,version,reinterpret_cast<FxName32_t*>(names.data()+16));consumed=ftell(file);fclose(file);}
const bool vt=!made||(*reinterpret_cast<uintptr_t*>(made)==(candidate?*reinterpret_cast<uintptr_t*>(&prototype):uintptr_t(0x85A7A4)));
return json{{"returned",ret},{"made",Ptr(made)},{"vtable",vt},{"arena",Hash(arena.data()+4,arena.size()-4)},{"object",Hash(object.data()+4,object.size()-4)},{"particles",Hash(particles.data(),sizeof(particles))},{"systems",Hash(systems.data(),sizeof(systems))},{"pool",Hash(fxPool.data(),fxPool.size())},{"pool_position",fn.slot==1?json(g_fxMan.GetMemPool().m_nPosition):json(nullptr)},{"names",names},{"consumed",consumed},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true&&a["vtable"]==true;pass&=eq;failures+=!eq;positive+=!a["trace"].empty();zero+=a["trace"].empty();Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);if(fn.slot!=1)pass&=positive>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive",positive},{"inactive",zero}});}}
g_fxMan.GetMemPool()=poolSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
