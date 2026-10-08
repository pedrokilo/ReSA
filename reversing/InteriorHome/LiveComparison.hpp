#pragma once
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/Interior_c.h"
#include "Interior/Furniture_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorHomeLive {
using ReversingProbe::EntryPatch;
inline json trace;
inline uint32 scenario{}, rngCalls{}, placements{};
alignas(16) inline std::array<uint8,0x794> interior{};
inline std::array<uint8,64> box{};
inline std::array<std::array<uint8,0x14>,32> furniture{};
alignas(16) inline std::array<std::array<uint8,0x400>,128> objects{};
template<class T=uint32> T& At(void* p,size_t offset){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+offset);}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64_t Digest(const void* p,size_t n){uint64_t h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
inline uintptr_t Identity(const void* p){auto a=uintptr_t(p);if(!a)return 0;const auto start=uintptr_t(objects.data()),end=start+sizeof(objects);if(a>=start&&a<end)return 0x10000+(a-start);const auto fs=uintptr_t(furniture.data());if(a>=fs&&a<fs+sizeof(furniture))return 0x20000+a-fs;return a;}
inline int32 __cdecl Random(){const auto v=(scenario*811+rngCalls++*3571)%32768;trace.push_back({"rand",v});return int32(v);}
inline int32 __cdecl Range(int32 lo,int32 hi){const auto r=(scenario*17+rngCalls++*23)&0x7fff;const auto v=hi>lo?lo+r%(hi-lo):lo;trace.push_back({"range",lo,hi,v});return v;}
inline Furniture_c* __fastcall Furniture(void*,void*,int32 group,int32 sub,int16 id,uint8 wealth){const auto k=uint32(group*7+sub*3+int32(id)+32)%32;trace.push_back({"furniture",group,sub,id,wealth,k});return reinterpret_cast<Furniture_c*>(furniture[k].data());}
inline int32 __fastcall RandomId(void*,void*,int32 group,int32 sub,uint8 wealth){const int32 v=(scenario+group+sub)%5;trace.push_back({"random_id",group,sub,wealth,v});return v;}
inline void __fastcall Tiles(Interior_c* self,void*,int32 x,int32 y,int32 w,int32 h,int32 direction,int8 status){trace.push_back({"tiles",x,y,w,h,direction,status});for(int a=0;a<w&&a<30;++a)for(int b=0;b<h&&b<30;++b)if(x+a>=0&&x+a<30&&y+b>=0&&y+b<30)At<uint8>(self,0x68+(x+a)*30+y+b)=uint8(status);}
inline void __fastcall Corner(Interior_c*,void*,int32 a,int32 b,int32 c,uint8 d){trace.push_back({"corner",a,b,c,d});}
inline int32 __fastcall Tile(Interior_c* self,void*,int32 x,int32 y){const auto value=x>=0&&y>=0&&x<30&&y<30?At<uint8>(self,0x68+x*30+y):uint8(3);trace.push_back({"tile",x,y,value});return value;}
inline bool __fastcall Empty(Interior_c*,void*,int32 w,int32 h,int32* x,int32* y){*x=1+scenario%2;*y=1+(scenario/2)%2;const bool found=scenario%4!=0;trace.push_back({"empty",w,h,*x,*y,found});return found;}
inline void __fastcall Goto(Interior_c*,void*,int32 a,int32 b,float x,float y){trace.push_back({"goto",a,b,F(x),F(y)});}
inline bool __fastcall Info(Interior_c* self,void*,int32 type,float x,float y,int32 direction,CEntity* entity){trace.push_back({"info",type,F(x),F(y),direction,Identity(entity)});auto& count=At<uint8>(self,0x40D);if(count>=16)return false;auto* out=reinterpret_cast<uint8*>(self)+0x590+count++*0x20;At<uint8>(out,0)=uint8(type);At<float>(out,4)=x;At<float>(out,8)=y;At<int32>(out,0x10)=direction;At<CEntity*>(out,0x1C)=entity;return true;}
inline CEntity* ObjectResult(){const auto n=placements++;return (scenario+n)%5==0?nullptr:reinterpret_cast<CEntity*>(objects[n%128].data());}
inline CObject* __fastcall Object(Interior_c*,void*,uint8 flags,Furniture_c* f,float x,float y,float z,float rot){auto* result=reinterpret_cast<CObject*>(ObjectResult());trace.push_back({"object",flags,Identity(f),F(x),F(y),F(z),F(rot),Identity(result)});return result;}
inline CEntity* __fastcall Place(Interior_c*,void*,Furniture_c* f,int32 x,int32 y,float z,int32 a,int32 b,int32* outX,int32* outY,uint8 flag){if(outX)*outX=x;if(outY)*outY=y;auto* result=ObjectResult();trace.push_back({"place",Identity(f),x,y,F(z),a,b,bool(outX),bool(outY),flag,Identity(result)});return result;}
inline CEntity* __fastcall Wall(Interior_c*,void*,int32 g,int32 s,int32 id,float z,int32 a,int32 dir,int32 offset,int32 d,int32* outDir,int32* outOffset,int32* outX,int32* outY,int32* outW,int32* outH){const int32 direction=dir<0?int32(scenario%4):dir;const int32 pos=offset<0?1:offset;if(outDir)*outDir=direction;if(outOffset)*outOffset=pos;if(outX)*outX=1;if(outY)*outY=1;if(outW)*outW=1;if(outH)*outH=1;auto* result=ObjectResult();trace.push_back({"wall",g,s,id,F(z),a,dir,offset,d,bool(outDir),bool(outOffset),bool(outX),bool(outY),bool(outW),bool(outH),Identity(result)});return result;}
inline CEntity* __fastcall InCorner(Interior_c*,void*,int32 g,int32 s,int32 id,float z,int32 a,int32 dir,int32 orientation,int32* outDir,int32* outX,int32* outY,int32* outW,int32* outH){if(outDir)*outDir=dir<0?int32(scenario%4):dir;if(outX)*outX=1;if(outY)*outY=1;if(outW)*outW=1;if(outH)*outH=1;auto* result=ObjectResult();trace.push_back({"in_corner",g,s,id,F(z),a,dir,orientation,bool(outDir),bool(outX),bool(outY),bool(outW),bool(outH),Identity(result)});return result;}
inline CObject* __fastcall Table(Interior_c*,void*,int32 a,int32 b,int32 c,int32 d,int32 e,int32 f){auto* result=reinterpret_cast<CObject*>(ObjectResult());trace.push_back({"table",a,b,c,d,e,f,Identity(result)});return result;}
inline CObject* __fastcall Edges(Interior_c*,void*){auto* result=reinterpret_cast<CObject*>(ObjectResult());trace.push_back({"edges",Identity(result)});return result;}
inline CObject* __fastcall TV(Interior_c*,void*,int32 a,int32 b,int32 c,int32 d){auto* result=reinterpret_cast<CObject*>(ObjectResult());trace.push_back({"tv",a,b,c,d,Identity(result)});return result;}
inline CObject* __fastcall Hifi(Interior_c*,void*,int32 a,int32 b,int32 c,int32 d){auto* result=reinterpret_cast<CObject*>(ObjectResult());trace.push_back({"hifi",a,b,c,d,Identity(result)});return result;}
inline void __fastcall Chair(Interior_c*,void*,int32 a,int32 b,CEntity* c){trace.push_back({"chair",a,b,Identity(c)});}
inline void __fastcall Sofa(Interior_c*,void*,int32 a,int32 b,CEntity* c){trace.push_back({"sofa",a,b,Identity(c)});}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,9> fns{{
{"Interior_c","Bedroom_AddTableItem",0x593F10,0,-1,1024},
{"Interior_c","FurnishBedroom",0x593FC0,0,-1,1024},
{"Interior_c","Kitchen_FurnishEdges",0x596930,0,-1,1024},
{"Interior_c","FurnishKitchen",0x5970B0,0,-1,1024},
{"Interior_c","Lounge_AddTV",0x597240,0,-1,1024},
{"Interior_c","Lounge_AddHifi",0x597430,0,-1,1024},
{"Interior_c","Lounge_AddChairInfo",0x5974E0,0,-1,1024},
{"Interior_c","Lounge_AddSofaInfo",0x5975C0,0,-1,1024},
{"Interior_c","FurnishLounge",0x597740,0,-1,1024},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-home-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-home-runtime.jsonl");ReversingProbe::InstallFailureCapture("interior-home");Write({{"event","waiting_for_player"}});}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;bool passed=true;bool restored=true;uint32 pairs{};
 for(const auto& fn:fns){auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}const bool oldEnabled=hook->Hooked(),oldLocked=hook->Locked();hook->LockState(false);
  std::vector<std::unique_ptr<EntryPatch>> patches;auto patch=[&](uintptr_t addr,auto callback){if(addr!=fn.address)patches.emplace_back(std::make_unique<EntryPatch>(addr,callback));};
  patch(0x407180,Range);patch(0x821B1E,Random);patch(0x5911E0,Furniture);patch(0x591220,RandomId);patch(0x591700,Tiles);patch(0x5917C0,Corner);patch(0x5918E0,Tile);patch(0x591C50,Empty);patch(0x591D20,Goto);patch(0x591E40,Info);patch(0x592AA0,Place);patch(0x593120,Wall);patch(0x593340,InCorner);patch(0x5934E0,Object);patch(0x593F10,Table);patch(0x596930,Edges);patch(0x597240,TV);patch(0x597430,Hifi);patch(0x5974E0,Chair);patch(0x5975C0,Sofa);
  int failures{},nulls{},nonnull{},positive{},dirMask{};const std::string full=std::string(fn.cls)+"::"+fn.name;
  for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);scenario=uint32(i);rngCalls=placements=0;trace=json::array();interior.fill(0);box.fill(0);for(size_t k=0;k<objects.size();++k)objects[k].fill(uint8(k));
   for(size_t k=0;k<furniture.size();++k){furniture[k].fill(0);At<int16>(furniture[k].data(),8)=int16(k);furniture[k][0xC]=uint8(1+(i+k)%2);furniture[k][0xD]=uint8(1+(i+k/2)%2);}
   box[2]=uint8(5+i%4);box[3]=uint8(5+(i/4)%4);box[5]=uint8(1+i%3);box[7]=uint8(i%3==0?-1:1);box[9]=uint8(i%4==0?-1:2);box[0xD]=uint8(i%5==0?-1:1);box[0xF]=uint8(i%6==0?-1:2);box[0x10]=uint8(i%2==0?-1:2);box[0x1F]=uint8(i%101);
   auto* self=reinterpret_cast<Interior_c*>(interior.data());At<void*>(self,0x14)=box.data();At<int16>(self,0x3EC)=box[2];At<int16>(self,0x3EE)=box[3];At<uint8>(self,0x790)=uint8(i%3);At<uint8>(self,0x791)=uint8(i%7);
   for(int x=0;x<30;++x)for(int y=0;y<30;++y)At<uint8>(self,0x68+x*30+y)=uint8((x+y+i)%7==0?1:0);
   const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;uintptr_t result{};const int32 direction=i%4,pos=1+(i/4)%3;
   if(fn.address==0x593F10)result=uintptr_t(reinterpret_cast<CObject*(__thiscall*)(Interior_c*,int32,int32,int32,int32,int32,int32)>(fn.address)(self,i%9,i%4,direction,pos,1+i%3,i%4));
   else if(fn.address==0x597240||fn.address==0x597430)result=uintptr_t(reinterpret_cast<CObject*(__thiscall*)(Interior_c*,int32,int32,int32,int32)>(fn.address)(self,direction,pos,1+i%3,1+(i/7)%3));
   else if(fn.address==0x5974E0||fn.address==0x5975C0)reinterpret_cast<void(__thiscall*)(Interior_c*,int32,int32,CEntity*)>(fn.address)(self,direction,pos,i%5==0?nullptr:reinterpret_cast<CEntity*>(objects[127].data()));
   else if(fn.address==0x596930)result=uintptr_t(reinterpret_cast<CObject*(__thiscall*)(Interior_c*)>(fn.address)(self));
   else reinterpret_cast<void(__thiscall*)(Interior_c*)>(fn.address)(self);
   return json{{"interior",Digest(interior.data(),interior.size())},{"box",Digest(box.data(),box.size())},{"furniture",Digest(furniture.data(),sizeof(furniture))},{"objects",Digest(objects.data(),sizeof(objects))},{"result",Identity(reinterpret_cast<void*>(result))},{"trace",trace},{"switches",switched}};
  };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true;passed&=equal;failures+=!equal;positive+=!original["trace"].empty();nulls+=original["result"]==0;nonnull+=original["result"]!=0;dirMask|=1<<(i%4);
  Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }
  std::vector<std::pair<uintptr_t,std::array<uint8,5>>> codeBefore;for(const auto& patch:patches)codeBefore.emplace_back(patch->address,patch->saved);patches.clear();for(const auto& entry:codeBefore)restored&=std::memcmp(reinterpret_cast<const void*>(entry.first),entry.second.data(),5)==0;
  hook->State(oldEnabled);hook->LockState(oldLocked);restored&=hook->Hooked()==oldEnabled&&hook->Locked()==oldLocked;
  const bool pointerResult=fn.address==0x593F10||fn.address==0x596930||fn.address==0x597240||fn.address==0x597430;
  const bool coverage=positive==fn.cases&&dirMask==15&&(!pointerResult||(nulls>0&&nonnull>0));passed&=coverage;
  Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",{{"positive",positive},{"directions",dirMask},{"null",nulls},{"nonnull",nonnull},{"required",coverage}}}});
 }
 Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
}
}
