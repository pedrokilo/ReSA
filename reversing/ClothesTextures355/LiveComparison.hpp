#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ClothesBuilder.h"
#include "TxdStore.h"
#include "../ProbeRuntime.hpp"
namespace ClothesTextures355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},requested{},pipeline{};
inline std::array<TxdDef,64> definitions{};inline std::array<uint8,64> flags{};inline std::array<uintptr_t,6> pool{};
inline uint32 Bits(float f){return std::bit_cast<uint32>(f);}
inline uintptr_t Id(const void* p){return uintptr_t(p);}
inline RwTexture* Tex(uintptr_t id){return reinterpret_cast<RwTexture*>(id);}
inline int32 __cdecl Request(uint32 key){int result=key?++requested:-1;trace.push_back({"request",key,result});return result;}
inline void __cdecl Load(){trace.push_back({"load_requested"});}
inline void __cdecl LoadAll(bool priority){trace.push_back({"load_all",priority});}
inline void __cdecl Remove(int id){trace.push_back({"remove",id});}
inline RwTexture* __cdecl Pipe(RwTexture* dest,int current,uint32 key,int* next){*next=key?++requested:-1;trace.push_back({"pipe",Id(dest),current,key,*next});++pipeline;if(dest)return dest;if(current==-1)return nullptr;return Tex(0x210000+uintptr_t(pipeline)*0x100);}
inline RwTexture* __cdecl Lookup(RwTexDictionary* dict,const char* name){const bool absent=(scenario%3==0&&!strcmp(name,StaticRef<const char*>(0x8D0A74)))||(scenario%5==0&&!strcmp(name,StaticRef<const char*>(0x8D0A78)));const auto key=CKeyGen::GetUppercaseKey(name);auto* t=absent?nullptr:Tex(0x310000+(key&0xffff));trace.push_back({"lookup",Id(dict),name,Id(t)});return t;}
inline RwTexture* __cdecl First(RwTexDictionary* dict){trace.push_back({"first",Id(dict)});return Tex(0x410000+(Id(dict)&0xffff));}
inline RwTexture* __cdecl Copy(RwTexture* src){auto* t=src?Tex(Id(src)+0x100000):nullptr;trace.push_back({"copy",Id(src),Id(t)});return t;}
inline void __cdecl Blend2(RwTexture* dst,RwTexture* src,float a,float b,int n){trace.push_back({"blend2",Id(dst),Id(src),Bits(a),Bits(b),n});}
inline void __cdecl Blend3(RwTexture* dst,RwTexture* s1,RwTexture* s2,float a,float b,float c,int n){trace.push_back({"blend3",Id(dst),Id(s1),Id(s2),Bits(a),Bits(b),Bits(c),n});}
inline void __cdecl BlendTattoo(RwTexture* dst,RwTexture* s1,RwTexture* s2,float a,float b,float c,int n,RwTexture* tattoos){trace.push_back({"blend_tattoo",Id(dst),Id(s1),Id(s2),Bits(a),Bits(b),Bits(c),n,Id(tattoos)});}
inline RwBool __cdecl Destroy(RwTexture* t){trace.push_back({"destroy",Id(t)});return true;}
inline RwTexture* __cdecl Name(RwTexture* t,const char* n){trace.push_back({"name",Id(t),n});return t;}
inline RwTexture* __cdecl Add(RwTexDictionary* d,RwTexture* t){trace.push_back({"add",Id(d),Id(t)});return t;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CClothesBuilder","ConstructTextures",0x5A6040,0,-1,2048}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CClothesBuilder")for(auto& x:c.Items())if(x->Name()=="ConstructTextures")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"clothes-textures355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"clothes-textures355-runtime.jsonl");ReversingProbe::InstallFailureCapture("clothes-textures355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* savedPool=CTxdStore::ms_pTxdPool;for(int k=0;k<64;++k)definitions[k].m_pRwDictionary=reinterpret_cast<RwTexDictionary*>(uintptr_t(0x510000+k*0x100));flags.fill(0);pool={uintptr_t(definitions.data()),uintptr_t(flags.data()),64,0,0,0};CTxdStore::ms_pTxdPool=reinterpret_cast<CTxdPool*>(pool.data());int pairs=0,failures=0,positive=0,tattoos=0,fallback=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch rq(0x5A4220,Request),rqd(bit_cast<uintptr_t>(&CClothesBuilder::RequestTexture),Request),load(0x40E3A0,Load),loadd(bit_cast<uintptr_t>(&CStreaming::LoadRequestedModels),Load),all(0x40EA10,LoadAll),alld(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),LoadAll),rem(0x4089A0,Remove),remd(bit_cast<uintptr_t>(&CStreaming::RemoveModel),Remove),p(0x5A5F70,Pipe),pd(bit_cast<uintptr_t>(&GetTextureFromTxdAndLoadNextTxd),Pipe),lookup(0x7F39F0,Lookup),first(0x734940,First),copy(0x5A5730,Copy),copyd(bit_cast<uintptr_t>(&CClothesBuilder::CopyTexture),Copy),b2(0x5A5820,Blend2),b2d(bit_cast<uintptr_t>(static_cast<void(*)(RwTexture*,RwTexture*,float,float,int32)>(&CClothesBuilder::BlendTextures)),Blend2),b3(0x5A59C0,Blend3),b3d(bit_cast<uintptr_t>(static_cast<void(*)(RwTexture*,RwTexture*,RwTexture*,float,float,float,int32)>(&CClothesBuilder::BlendTextures)),Blend3),bt(0x5A5BC0,BlendTattoo),btd(bit_cast<uintptr_t>(static_cast<void(*)(RwTexture*,RwTexture*,RwTexture*,float,float,float,int32,RwTexture*)>(&CClothesBuilder::BlendTextures)),BlendTattoo),destroy(0x7F3820,Destroy),name(0x7F38A0,Name),add(0x7F3980,Add);
auto* h=FindHook();if(!h){CTxdStore::ms_pTxdPool=savedPool;Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){scenario=i;requested=0;pipeline=0;trace=json::array();std::array<uint32,18> hashes{};for(int k=0;k<18;++k)hashes[k]=(i&(1<<(k%10)))?0:uint32(100+k);float a=float(i%9)*.125f,b=float((i/9)%13-3)*.125f,c=float((i/117)%9)*.125f;if(i%67==0)b=std::bit_cast<float>(0x7FC00123u);h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x5A6040)==candidate;reinterpret_cast<void(__cdecl*)(RwTexDictionary*,uint32*,float,float,float)>(0x5A6040)(reinterpret_cast<RwTexDictionary*>(uintptr_t(0x610000)),hashes.data(),a,b,c);return json{{"trace",trace},{"requested",requested},{"hashes",hashes},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=!a["trace"].empty();for(const auto& t:a["trace"]){tattoos+=t[0]=="blend_tattoo";fallback+=t[0]=="first";}Write({{"event","comparison"},{"function","CClothesBuilder::ConstructTextures"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
CTxdStore::ms_pTxdPool=savedPool;pass&=positive>0&&tattoos>0&&fallback>0;Write({{"event","function_summary"},{"function","CClothesBuilder::ConstructTextures"},{"cases",pairs},{"failures",failures},{"positive",positive},{"tattoos",tattoos},{"fallback",fallback}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
