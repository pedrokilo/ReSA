#pragma once
#include <fstream>
#include <memory>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/Interior_c.h"
#include "Interior/InteriorGroup_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorSpatialLive {
using ReversingProbe::EntryPatch;
inline json trace;inline uint32 scenario{},randomCalls{},checkCalls{};
alignas(16) inline std::array<uint8,0x794> raw{};
alignas(16) inline std::array<uint8,0x100> entity{};
alignas(16) inline std::array<uint8,0x1000> group{};
alignas(16) inline std::array<uint8,0x80> frame{};
inline std::array<uint8,64> box{};inline std::array<uint8,32> rw{};
inline std::array<int32,900> visited{};inline std::array<int32,6> outputs{};inline CVector point{},bias{},translation{};
template<class T=uint32>T& At(void* p,size_t o){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+o);}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64_t Digest(const void* p,size_t n){uint64_t h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
inline json V(const RwV3d* p){return json::array({F(p->x),F(p->y),F(p->z)});}
inline json M(const RwMatrix* p){if(!p)return nullptr;json result=json::array();const auto* q=reinterpret_cast<const uint32*>(p);for(int i=0;i<16;++i)result.push_back(q[i]);return result;}
inline uintptr_t MatrixId(const RwMatrix* p){if(!p)return 0;if(uintptr_t(p)==uintptr_t(raw.data()+0x18))return 1;if(uintptr_t(p)==uintptr_t(frame.data()+0x10))return 2;return uintptr_t(p);}
inline int32 __cdecl Random(){const auto call=randomCalls++;const int32 value=call<2?0:int32(16384+(scenario*97)%12000);trace.push_back({"rand",value});return value;}
inline uint8 __fastcall Tile(Interior_c* self,void*,int32 x,int32 y){const auto value=x>=0&&x<30&&y>=0&&y<30?At<uint8>(self,0x68+x*30+y):uint8(1);trace.push_back({"tile",x,y,value});return value;}
inline int8 __fastcall Check(Interior_c*,void*,int32 x,int32 y,int32 w,int32 h,uint8 window){const auto call=checkCalls++;const bool result=scenario%4!=0&&call>=scenario%5;trace.push_back({"check",x,y,w,h,window,result});return int8(result);}
inline void __fastcall SetTiles(Interior_c* self,void*,int32 x,int32 y,int32 w,int32 h,int32 status,int8 force){trace.push_back({"set_tiles",x,y,w,h,status,force});for(int a=0;a<w&&a<30;++a)for(int b=0;b<h&&b<30;++b)if(x+a>=0&&x+a<30&&y+b>=0&&y+b<30)At<uint8>(self,0x68+(x+a)*30+y+b)=uint8(status);}
inline CEntity* __fastcall Entity(InteriorGroup_c* self,void*){trace.push_back({"entity",uintptr_t(self)==uintptr_t(group.data())?uintptr_t(1):uintptr_t(self)});return reinterpret_cast<CEntity*>(entity.data());}
inline RwV3d* __cdecl Transform(RwV3d* out,const RwV3d* in,RwInt32 count,const RwMatrix* matrix){trace.push_back({"transform",V(in),count,M(matrix),out==in,MatrixId(matrix)});const auto input=*in;out->x=input.x+matrix->pos.x;out->y=input.y+matrix->pos.y;out->z=input.z+matrix->pos.z;return out;}
inline RwMatrix* __cdecl Rotate(RwMatrix* matrix,const RwV3d* axis,RwReal angle,RwOpCombineType combine){trace.push_back({"rotate",M(matrix),V(axis),F(angle),int(combine),MatrixId(matrix)});matrix->right.x+=angle*0.125f;matrix->up.y-=angle*0.0625f;return matrix;}
inline RwMatrix* __cdecl Translate(RwMatrix* matrix,const RwV3d* t,RwOpCombineType combine){trace.push_back({"translate",M(matrix),V(t),int(combine),MatrixId(matrix)});matrix->pos=*t;return matrix;}
inline RwMatrix* __cdecl Multiply(RwMatrix* out,const RwMatrix* a,const RwMatrix* b){trace.push_back({"multiply",M(a),M(b),MatrixId(out),MatrixId(a),MatrixId(b),out==a,out==b});if(b){out->pos.x+=b->pos.x;out->pos.y+=b->pos.y;out->pos.z+=b->pos.z;}else out->flags^=0x8000;return out;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,7> fns{{
{"Interior_c","GetTileCentre",0x591BD0,0,-1,1024},
{"Interior_c","GetRandomTile",0x591B20,0,-1,1024},
{"Interior_c","FindEmptyTiles",0x591C50,0,-1,1024},
{"Interior_c","FindBoundingBox",0x5922C0,0,-1,1024},
{"Interior_c","ResetTiles",0x593910,0,-1,1024},
{"Interior_c","IsPtInside",0x5913E0,0,-1,1024},
{"Interior_c","CalcMatrix",0x5914D0,0,-1,1024},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name){ReversibleHooks::ReversibleHook::Base* result{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)result=h.get();});return result;}
inline bool done{};inline std::ofstream log;
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
struct RestorationProof {
    struct Entry { uintptr_t address; std::array<uint8,5> bytes; };
    struct HookState { ReversibleHooks::ReversibleHook::Base* hook; bool enabled,locked; };
    std::vector<Entry> entries;
    std::vector<HookState> hooks;
    void SaveEntry(uintptr_t address,const std::array<uint8,5>& bytes){for(const auto& entry:entries)if(entry.address==address)return;entries.push_back({address,bytes});}
    void SaveHook(ReversibleHooks::ReversibleHook::Base* hook,uintptr_t address){std::array<uint8,5> bytes{};std::memcpy(bytes.data(),reinterpret_cast<void*>(address),5);SaveEntry(address,bytes);hooks.push_back({hook,hook->Hooked(),hook->Locked()});}
    void SavePatch(const ReversingProbe::EntryPatch& patch){SaveEntry(patch.address,patch.saved);}
    bool Verify()const{bool restored=true;for(const auto& state:hooks)restored&=state.hook->Hooked()==state.enabled&&state.hook->Locked()==state.locked;for(const auto& entry:entries)restored&=std::memcmp(reinterpret_cast<void*>(entry.address),entry.bytes.data(),5)==0;return restored;}
};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"interior-spatial-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"interior-spatial-runtime.jsonl");ReversingProbe::InstallFailureCapture("interior-spatial");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;bool passed=true;uint32 pairs{};
 RestorationProof restoration;
 for(const auto& target:fns){auto* targetHook=FindHook(target.cls,target.name);if(!targetHook){Write({{"event","error"},{"reason","missing hook before probe"},{"state_restored",false}});return;}restoration.SaveHook(targetHook,target.address);}

 for(const auto& fn:fns){auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}const bool oldEnabled=hook->Hooked(),oldLocked=hook->Locked();hook->LockState(false);
  std::vector<std::unique_ptr<EntryPatch>> patches;auto patch=[&](uintptr_t addr,auto callback){patches.emplace_back(std::make_unique<EntryPatch>(addr,callback));restoration.SavePatch(*patches.back());};auto both=[&](uintptr_t addr,uintptr_t dll,auto callback){patch(addr,callback);if(dll!=addr)patch(dll,callback);};
  if(fn.address==0x591B20||fn.address==0x591C50){both(0x821B1E,std::bit_cast<uintptr_t>(&std::rand),Random);const auto general=std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber);if(general!=0x821B1E&&general!=std::bit_cast<uintptr_t>(&std::rand))patch(general,Random);}
  if(fn.address==0x591C50)both(0x591680,std::bit_cast<uintptr_t>(&Interior_c::CheckTilesEmpty),Check);
  if(fn.address==0x591BD0)both(0x7EDD90,uintptr_t(&RwV3dTransformPoints),Transform);
  if(fn.address==0x5914D0){both(0x594BD0,std::bit_cast<uintptr_t>(&InteriorGroup_c::GetEntity),Entity);both(0x7F1FD0,uintptr_t(&RwMatrixRotate),Rotate);both(0x7F2450,uintptr_t(&RwMatrixTranslate),Translate);both(0x7F18B0,uintptr_t(&RwMatrixMultiply),Multiply);}
  int failures{},trueResults{},falseResults{},changed{},retry{},nullMatrix{},nonnullMatrix{},visitedGrowth{},pointChanges{};const std::string full=std::string(fn.cls)+"::"+fn.name;
  for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);scenario=uint32(i);randomCalls=checkCalls=0;trace=json::array();raw.fill(0xA7);box.fill(0);entity.fill(0);rw.fill(0);frame.fill(0);group.fill(0);visited.fill(0);outputs.fill(-12345);
   box[2]=uint8(5+i%5);box[3]=uint8(5+(i/5)%5);box[4]=uint8(2+i%7);At<float>(box.data(),0x20)=float(i%361-180)*0.5f;
   auto* self=reinterpret_cast<Interior_c*>(raw.data());At<void*>(self,0x14)=box.data();At<void*>(self,0xC)=group.data();At<int16>(self,0x3EC)=box[2];At<int16>(self,0x3EE)=box[3];
   auto* matrix=reinterpret_cast<RwMatrix*>(raw.data()+0x18);std::memset(matrix,0,sizeof(*matrix));matrix->right={1.0f,0.0f,0.0f};matrix->up={0.0f,1.0f,0.0f};matrix->at={0.0f,0.0f,1.0f};matrix->pos={float(i%13)*0.125f,float(i%17)*-0.25f,float(i%19)*0.0625f};if(i%4==2){matrix->right={0.75f,-0.125f,0.375f};matrix->up={-0.5f,1.25f,0.25f};matrix->at={0.125f,0.375f,0.625f};}if(i%4==3){matrix->right={-1.0f,0.25f,0.125f};matrix->up={0.125f,-0.75f,-0.25f};matrix->at={-0.25f,0.125f,-1.5f};}matrix->flags=0xAA008000|uint32(i%8);matrix->pad1=0x11223344;matrix->pad2=0x55667788;matrix->pad3=0x99AABBCC;
   At<void*>(entity.data(),0x18)=i%3==0?nullptr:rw.data();At<void*>(rw.data(),4)=frame.data();auto* fm=reinterpret_cast<RwMatrix*>(frame.data()+0x10);fm->pos={float(i%7),float(i%11),float(i%3)};
   std::fill(raw.begin()+0x68,raw.begin()+0x3EC,uint8(i%12));At<uint8>(self,0x68)=uint8((i%12+1)%12);
   if(fn.address==0x5922C0){box[2]=box[3]=20;At<int16>(self,0x3EC)=At<int16>(self,0x3EE)=20;std::fill(raw.begin()+0x68,raw.begin()+0x3EC,uint8(0));for(int x=11;x<=15;++x)for(int y=11;y<=15;++y)if(i%3!=0&&((x+y+i)%5!=0||x==13||y==13))At<uint8>(self,0x68+x*30+y)=5;visited[13*30+13]=1;outputs[0]=outputs[1]=outputs[2]=outputs[3]=13;}
   if(fn.address==0x593910){for(int k=6;k<=16;k+=2){box[k]=uint8((i+k)%4==0?-1:1);box[k+1]=uint8(2+(i+k)%3);}for(int k=0;k<3;++k){box[0x12+k]=uint8((i+k)%4==0?-1:1+k);box[0x15+k]=uint8((i+k)%5==0?-1:1+k);box[0x18+k]=uint8(1+(i+k)%2);box[0x1B+k]=uint8(1+(i+k/2)%2);}}
   point=CVector{matrix->pos.x+float(i%11-5),matrix->pos.y+float((i/11)%11-5),matrix->pos.z+float((i/121)%7-3)};bias=CVector{float(i%3)*0.125f,float(i%5)*-0.125f,float(i%7)*0.0625f};translation=CVector{float(i%17)-8.25f,float(i%23)*0.1f,float(i%13)*-0.2f};
   if(fn.address==0x5913E0){if(i%16==0)point.x=std::bit_cast<float>(uint32(0x7FC00001));if(i%16==1)point.y=std::bit_cast<float>(uint32(0x7F800000));if(i%16==2)point.z=std::bit_cast<float>(uint32(0xFF800000));if(i%16==3)point.x=matrix->pos.x+float(box[2])*0.5f+bias.x;}
   const auto before=Digest(raw.data(),raw.size());const auto pointBefore=std::array<uint32,3>{F(point.x),F(point.y),F(point.z)};const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;uintptr_t result{};
   if(fn.address==0x591BD0)result=uintptr_t(reinterpret_cast<CVector*(__thiscall*)(Interior_c*,float,float,CVector*)>(fn.address)(self,float(i%37)*0.1f-1.0f,float(i%41)*-0.125f,&point));
   else if(fn.address==0x591B20)result=uint32(reinterpret_cast<int32(__thiscall*)(Interior_c*,int32,int32*,int32*)>(fn.address)(self,i%12,&outputs[0],&outputs[1]));
   else if(fn.address==0x591C50)result=reinterpret_cast<bool(__thiscall*)(Interior_c*,int32,int32,int32*,int32*)>(fn.address)(self,1+i%3,1+(i/3)%3,&outputs[0],&outputs[1]);
   else if(fn.address==0x5922C0)reinterpret_cast<void(__thiscall*)(Interior_c*,int32,int32,int32*,int32*,int32*,int32*,int32*)>(fn.address)(self,13,13,&outputs[0],&outputs[1],&outputs[2],&outputs[3],visited.data());
   else if(fn.address==0x5913E0)result=reinterpret_cast<bool(__thiscall*)(Interior_c*,const CVector&,CVector)>(fn.address)(self,point,bias);
   else if(fn.address==0x5914D0)reinterpret_cast<void(__thiscall*)(Interior_c*,CVector*)>(fn.address)(self,&translation);
   else reinterpret_cast<void(__thiscall*)(Interior_c*)>(fn.address)(self);
   return json{{"interior",Digest(raw.data(),raw.size())},{"box",Digest(box.data(),box.size())},{"group",Digest(group.data(),group.size())},{"rw",Digest(rw.data(),rw.size())},{"entity",Digest(entity.data(),entity.size())},{"frame",Digest(frame.data(),frame.size())},{"visited",Digest(visited.data(),sizeof(visited))},{"outputs",outputs},{"point",{F(point.x),F(point.y),F(point.z)}},{"bias",{F(bias.x),F(bias.y),F(bias.z)}},{"translation",{F(translation.x),F(translation.y),F(translation.z)}},{"result",result},{"return_contract",fn.address==0x591BD0?result==uintptr_t(&point):fn.address==0x591B20?result==uint32(outputs[1]):true},{"trace",trace},{"point_changed",pointBefore!=std::array<uint32,3>{F(point.x),F(point.y),F(point.z)}},{"visited_count",std::count(visited.begin(),visited.end(),1)},{"random_calls",randomCalls},{"check_calls",checkCalls},{"changed",before!=Digest(raw.data(),raw.size())},{"switches",switched}};
  };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true&&original["return_contract"]==true&&candidate["return_contract"]==true;passed&=equal;failures+=!equal;trueResults+=original["result"]==1;falseResults+=original["result"]==0;changed+=original["changed"]==true;retry+=original["random_calls"].get<uint32>()>2;nullMatrix+=i%3==0;nonnullMatrix+=i%3!=0;visitedGrowth+=original["visited_count"].get<int>()>1;pointChanges+=original["point_changed"]==true;
   Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }
  hook->State(oldEnabled);hook->LockState(oldLocked);bool coverage=true;if(fn.address==0x5922C0)coverage=visitedGrowth>0;if(fn.address==0x591BD0)coverage=pointChanges>0;if(fn.address==0x591C50||fn.address==0x5913E0)coverage=trueResults>0&&falseResults>0;if(fn.address==0x591B20)coverage=retry>0;if(fn.address==0x593910||fn.address==0x5914D0)coverage=changed>0;if(fn.address==0x5914D0)coverage&=nullMatrix>0&&nonnullMatrix>0;passed&=coverage;
  Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"coverage",{{"true",trueResults},{"false",falseResults},{"changed",changed},{"retry",retry},{"visited_growth",visitedGrowth},{"point_changes",pointChanges},{"null_matrix",nullMatrix},{"nonnull_matrix",nonnullMatrix},{"required",coverage}}}});
 }
 const bool restored=restoration.Verify();passed&=restored;
 Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
}
}
