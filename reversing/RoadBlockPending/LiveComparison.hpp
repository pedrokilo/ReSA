#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RoadBlocks.h"
#include "../ProbeRuntime.hpp"

namespace RoadBlockPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& v) { ReversingProbe::PumpMessages(); log << v.dump() << '\n'; log.flush(); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
template<class T> inline T& At(void* p,size_t n){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+n);}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,1> fns{{{"CRoadBlocks","CreateRoadBlockBetween2Points",0x4619C0,0,-1,1024}}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* result{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)result=h.get();});
    return result;
}
alignas(16) inline std::array<std::array<uint8,0x988>,16> entities;
inline std::array<CMatrix,16> matrices;
inline std::array<std::array<uint8,0x28>,16> fires;
inline std::array<uintptr_t,64> vt;
inline uint32 caseId{}, seed{}, calls{}, allocated{}, clearCalls{}, groundCalls{};
inline int32 model{}, defaultModel{}; inline CVector playerPosition;
inline bool army{}, fbi{}, swat{}, law{};
inline int Id(const void* p){for(int i=0;i<int(entities.size());++i)if(p==entities[i].data())return i;return -1;}
inline json V(CVector p){return json::array({F(p.x),F(p.y),F(p.z)});}
inline int __cdecl Rand(){seed=seed*1664525u+1013904223u;++calls;const int value=int(seed>>16);trace.push_back({"rand",value});return value;}
inline uint16 Random(){return uint16(Rand());}
inline CVector Coors(int32 id){trace.push_back({"coors",id});return playerPosition;}
inline CWanted* Wanted(int32 id){trace.push_back({"wanted",id});return reinterpret_cast<CWanted*>(uintptr_t(0x12345000));}
inline bool __fastcall Army(void*,void*){trace.push_back({"army"});return army;}
inline bool __fastcall Fbi(void*,void*){trace.push_back({"fbi"});return fbi;}
inline bool __fastcall Swat(void*,void*){trace.push_back({"swat"});return swat;}
inline eModelID Default(bool flag){trace.push_back({"default",flag,defaultModel});return eModelID(defaultModel);}
inline eModelID Pick(){trace.push_back({"pick",model});return eModelID(model);}
inline bool Clear(CVector a,CVector b){const bool result=caseId%4>=2 || clearCalls%2!=0; ++clearCalls; trace.push_back({"clear",V(a),V(b),result});return result;}
inline float Ground(CVector p,bool* found,CEntity** out){*found=(caseId+groundCalls)%5!=0;++groundCalls;trace.push_back({"ground",V(p),*found,out==nullptr});return float(int(caseId%21)-10)*0.25f;}
inline void DeleteArea(CVector p,float radius){trace.push_back({"delete_area",V(p),F(radius)});}
inline void* Alloc(size_t amount){trace.push_back({"allocate",amount});if(allocated>=entities.size())std::abort();return entities[allocated++].data();}
inline void* __fastcall CarCtor(void* self,void*,int32 m,eVehicleCreatedBy cb,bool setup){
    trace.push_back({"car_ctor",Id(self),m,int(cb),setup});const auto id=Id(self);
    At<uintptr_t>(self,0)=uintptr_t(vt.data());At<CMatrix*>(self,0x14)=&matrices[id];At<void*>(self,0x18)=nullptr;
    At<void*>(self,0x490)=fires[id].data();At<uint16>(self,0x22)=uint16(m);
    for(int k=0;k<25;++k)At<uintptr_t>(self,0x648+4*k)=caseId%3 ? 0x1000u+k*16u : 0;
    return self;
}
inline void* __fastcall ObjCtor(void* self,void*,int32 m,bool create){
    trace.push_back({"object_ctor",Id(self),m,create});const auto id=Id(self);
    At<uintptr_t>(self,0)=uintptr_t(vt.data());At<CMatrix*>(self,0x14)=&matrices[id];At<void*>(self,0x18)=nullptr;
    At<void*>(self,0x164)=fires[id].data();At<uint16>(self,0x22)=uint16(m);return self;
}
inline void* __fastcall Destroy(void* self,void*,uint32 flags){trace.push_back({"destroy",Id(self),flags});return self;}
inline void __fastcall Static(void* self,void*,bool value){trace.push_back({"static",Id(self),value});}
inline float __fastcall Height(void* self,void*){trace.push_back({"height",Id(self)});return float(caseId%7)*0.25f;}
inline void __fastcall Door(void* self,void*,CPed* ped,int32 node,eDoors door,float ratio,bool sound){trace.push_back({"door",Id(self),ped==nullptr,node,int(door),F(ratio),sound});}
inline void __fastcall Explode(void* self,void*,bool a,bool b,bool c,bool d){trace.push_back({"explode",Id(self),a,b,c,d});}
inline void __fastcall Road(void* self,void*){trace.push_back({"road",Id(self)});if(caseId%8==0)At<CMatrix*>(self,0x14)->GetUp().z=0.94f;else if(caseId%8==1)At<CMatrix*>(self,0x14)->GetUp().z=0.941f;}
inline void __fastcall Damage(void* self,void*,bool value){trace.push_back({"damage",Id(self),value});}
inline bool __fastcall Law(void* self,void*){trace.push_back({"law",Id(self)});return law;}
inline void Physics(CVehicle* self){trace.push_back({"physics",Id(self)});}
inline void Alpha(RpClump* clump,int32 value){trace.push_back({"alpha",clump==nullptr,value});}
inline void Add(CEntity* self){trace.push_back({"add",Id(self)});}
inline CFire* __fastcall Fire(void*,void*,CEntity* target,CEntity* creator,float amount,uint8 unknown,uint32 time,int8 generation){trace.push_back({"fire",Id(target),creator==nullptr,F(amount),unknown,time,generation});return nullptr;}
inline uintptr_t Symbol(const char* name){ HMODULE module{}; if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Symbol),&module)){if(const auto exported=GetProcAddress(module,name))return uintptr_t(exported);} alignas(SYMBOL_INFO) char buffer[sizeof(SYMBOL_INFO)+512]{};auto* s=reinterpret_cast<SYMBOL_INFO*>(buffer);s->SizeOfStruct=sizeof(SYMBOL_INFO);s->MaxNameLen=511;return SymFromName(GetCurrentProcess(),name,s)?uintptr_t(s->Address):0;}
// FNV-1a 64-bit sobre dump() UTF-8 completo de json (claves ordenadas de nlohmann::json).
inline uint64 Hash(const json& value){uint64 h=14695981039346656037ull;for(const unsigned char c:value.dump()){h^=c;h*=1099511628211ull;}return h;}
inline json Digest(const json& value){return json{{"fnv1a64_json_utf8",Hash(value)},{"events",value["trace"].size()},{"objects",value["objects"].size()},{"temporary",value["temporary"]},{"switches",value["switches"]}};}

inline void Update(){
    if(done)return;
    char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);const auto dir=std::filesystem::path(path).parent_path();
    if(!std::filesystem::exists(dir/"roadblock-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"roadblock-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("roadblock-pending");}
    if(!FindPlayerPed() || CCutsceneMgr::IsRunning())return;done=true;
    std::string only;{std::ifstream f(dir/"roadblock-pending-only.txt");if(f)std::getline(f,only);}
    const std::string full="CRoadBlocks::CreateRoadBlockBetween2Points";
    if(!only.empty() && only!=full){Write({{"event","error"},{"reason","unknown filter"},{"state_restored",true}});return;}
    auto* hook=FindHook("CRoadBlocks","CreateRoadBlockBetween2Points");
    if(!hook){Write({{"event","error"},{"reason","missing hook"},{"state_restored",true}});return;}
    SymInitialize(GetCurrentProcess(),nullptr,TRUE);
    auto carConstructor=Symbol("??0CAutomobile@@QAE@HW4eVehicleCreatedBy@@_N@Z");
    auto objectConstructor=Symbol("??0CObject@@QAE@H_N@Z");
    if(!carConstructor||!objectConstructor){Write({{"event","error"},{"reason","DLL constructor symbol missing"},{"car",carConstructor},{"object",objectConstructor},{"state_restored",true}});return;}
    const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);
    const auto savedModels=ReversingProbe::Bytes(reinterpret_cast<void*>(0xA9B0C8),20000*4);
    const auto savedStreaming=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8E4CC0),26316*20);
    const auto savedTime=At<uint32>(reinterpret_cast<void*>(0xB7CB84),0);
    const auto savedTemporary=CObject::nNoTempObjects;
    const auto savedBarrier=At<uint16>(reinterpret_cast<void*>(0x8CD57C),0),savedWreck=At<uint16>(reinterpret_cast<void*>(0x8CD580),0);
    alignas(16) static std::array<uint8,0x40> modelInfo;
    static CColModel collision;
    At<CColModel*>(modelInfo.data(),0x14)=&collision;
    uint32 failures=0;
    json coverage={{"car",0},{"object",0},{"fire",0},{"doors",0},{"destructor",0},{"space_reject",0},{"gang_space_reject",0},{"gang_model_minus_one",0},{"copbike_exit",0}};
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch eRand(0x821B1E,Rand),dRand(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
        EntryPatch eCoors(0x56E010,Coors),dCoors(bit_cast<uintptr_t>(&FindPlayerCoors),Coors);
        EntryPatch eWanted(0x56E230,Wanted),dWanted(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted);
        EntryPatch eArmy(0x561F80,Army),dArmy(bit_cast<uintptr_t>(&CWanted::AreArmyRequired),Army);
        EntryPatch eFbi(0x561F60,Fbi),dFbi(bit_cast<uintptr_t>(&CWanted::AreFbiRequired),Fbi);
        EntryPatch eSwat(0x561F40,Swat),dSwat(bit_cast<uintptr_t>(&CWanted::AreSwatRequired),Swat);
        EntryPatch eDefault(0x407C50,Default),dDefault(bit_cast<uintptr_t>(&CStreaming::GetDefaultCopCarModel),Default);
        EntryPatch ePick(0x6144B0,Pick),dPick(bit_cast<uintptr_t>(&CPopulation::PickRiotRoadBlockCar),Pick);
        EntryPatch eClear(0x461020,Clear),dClear(bit_cast<uintptr_t>(&CRoadBlocks::ClearSpaceForRoadBlockObject),Clear);
        EntryPatch eGround(0x5696C0,Ground),dGround(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),Ground);
        EntryPatch eDeleteArea(0x5A1980,DeleteArea),dDeleteArea(bit_cast<uintptr_t>(&CObject::DeleteAllTempObjectsInArea),DeleteArea);
        EntryPatch eCarNew(0x6E2D50,Alloc),dCarNew(bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CVehicle::operator new)),Alloc);
        EntryPatch eObjNew(0x5A1EE0,Alloc),dObjNew(bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CObject::operator new)),Alloc);
        EntryPatch eCarCtor(0x6B0A90,CarCtor),dCarCtor(carConstructor,CarCtor),eObjCtor(0x5A1D70,ObjCtor),dObjCtor(objectConstructor,ObjCtor);
        EntryPatch eRoad(0x6AF420,Road),dRoad(bit_cast<uintptr_t>(&CAutomobile::PlaceOnRoadProperly),Road);
        EntryPatch eDamage(0x6A27F0,Damage),dDamage(bit_cast<uintptr_t>(&CAutomobile::SetTotalDamage),Damage);
        EntryPatch eLaw(0x6D8470,Law),dLaw(bit_cast<uintptr_t>(&CVehicle::IsLawEnforcementVehicle),Law);
        EntryPatch ePhysics(0x42F5A0,Physics),dPhysics(bit_cast<uintptr_t>(&CCarCtrl::SwitchVehicleToRealPhysics),Physics);
        EntryPatch eAlpha(0x732B00,Alpha),dAlpha(bit_cast<uintptr_t>(&CVisibilityPlugins::SetClumpAlpha),Alpha);
        EntryPatch eAdd(0x563220,Add),dAdd(bit_cast<uintptr_t>(&CWorld::Add),Add);
        using FireMethod=CFire*(CFireManager::*)(CEntity*,CEntity*,float,uint8,uint32,int8);
        EntryPatch eFire(0x53A050,Fire),dFire(bit_cast<uintptr_t>(static_cast<FireMethod>(&CFireManager::StartFire)),Fire);
        vt.fill(uintptr_t(&Static));vt[0]=uintptr_t(&Destroy);vt[4]=uintptr_t(&Static);vt[27]=uintptr_t(&Door);vt[42]=uintptr_t(&Explode);vt[53]=uintptr_t(&Height);
        for(uint32 i=0;i<1024;++i){
            auto run=[&](bool candidate){
                hook->State(candidate);const bool switched=ReversingProbe::JumpsToDll(0x4619C0)==candidate;
                caseId=i;seed=0xC065A91u+i*31337u;calls=allocated=clearCalls=groundCalls=0;trace=json::array();
                for(auto& e:entities)e.fill(0xCD);for(auto& f:fires)f.fill(0xCD);
                for(auto& m:matrices){m.m_pAttachMatrix=nullptr;m.m_bOwnsAttachedMatrix=false;m.SetUnity();}
                for(int k=0;k<20000;++k)CModelInfo::ms_modelInfoPtrs[k]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
                static const int32 models[]{-1,400,427,433,490,523};model=models[(i/2)%6];defaultModel=i%9==0?523:596;
                army=i%7==0;fbi=i%7==1;swat=i%7==2;law=i%3!=0;
                auto* streaming=reinterpret_cast<uint8*>(0x8E4CC0);for(int k=0;k<20000;++k)streaming[k*20+16]=i%5==0?0:1;
                At<uint16>(reinterpret_cast<void*>(0x8CD57C),0)=1234;At<uint16>(reinterpret_cast<void*>(0x8CD580),0)=1235;
                CObject::nNoTempObjects=uint16(65000+i);At<uint32>(reinterpret_cast<void*>(0xB7CB84),0)=i%2?0xFFFFE000u:i*719u;
                auto& box=collision.GetBoundingBox();box.m_vecMin=CVector{-float(1+i%4)*0.25f,-float(2+i%5)*0.5f,-0.75f};box.m_vecMax=CVector{float(1+i%5)*0.5f,float(2+i%4)*0.5f,1.25f};
                CVector a{float(int(i%23)-11)*1.5f,float(int(i%29)-14)*2.f,float(int(i%9)-4)*0.125f};
                static const CVector directions[]{ {1.f,0.f,0.f},{0.f,1.f,0.f},{0.6f,0.8f,0.f},{-0.8f,0.6f,0.f},{0.f,0.f,1.f},{0.5f,0.5f,0.5f}};
                const float width=i%13==0?0.f:float(i%61)*0.75f;
                const CVector b=a+directions[(i/13)%6]*width;
                playerPosition=a+CVector{float(int(i%7)-3)*4.f,float(int(i%11)-5)*3.f,2.f};
                reinterpret_cast<void(__cdecl*)(CVector,CVector,bool)>(0x4619C0)(a,b,i%2!=0);
                json objects=json::array();for(uint32 k=0;k<allocated;++k){auto bytes=ReversingProbe::Bytes(entities[k].data(),0x988);objects.push_back({{"bytes",bytes},{"matrix",json::array({V(matrices[k].GetRight()),V(matrices[k].GetForward()),V(matrices[k].GetUp()),V(matrices[k].GetPosition())})},{"fire",ReversingProbe::Bytes(fires[k].data(),fires[k].size())}});}
                const auto output=json{{"switches",switched},{"trace",trace},{"objects",objects},{"temporary",CObject::nNoTempObjects}};
                std::memcpy(reinterpret_cast<void*>(0xA9B0C8),savedModels.data(),savedModels.size());
                std::memcpy(reinterpret_cast<void*>(0x8E4CC0),savedStreaming.data(),savedStreaming.size());
                At<uint32>(reinterpret_cast<void*>(0xB7CB84),0)=savedTime;CObject::nNoTempObjects=savedTemporary;
                At<uint16>(reinterpret_cast<void*>(0x8CD57C),0)=savedBarrier;At<uint16>(reinterpret_cast<void*>(0x8CD580),0)=savedWreck;
                return output;
            };
            const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true&&candidate["switches"]==true;failures+=!equal;
            bool picked=false,selectedBike=false,deletedArea=false;
            for(const auto& ev:original["trace"]){const auto name=ev[0].get<std::string>();
                if(name=="car_ctor")coverage["car"]=coverage["car"].get<int>()+1;
                else if(name=="object_ctor")coverage["object"]=coverage["object"].get<int>()+1;
                else if(name=="fire")coverage["fire"]=coverage["fire"].get<int>()+1;
                else if(name=="door")coverage["doors"]=coverage["doors"].get<int>()+1;
                else if(name=="destroy")coverage["destructor"]=coverage["destructor"].get<int>()+1;
                else if(name=="clear"&&ev[3]==false){coverage["space_reject"]=coverage["space_reject"].get<int>()+1;if(i%2)coverage["gang_space_reject"]=coverage["gang_space_reject"].get<int>()+1;}
                else if(name=="pick"&&ev[1]==-1)picked=true;
                else if(name=="default"&&ev[2]==523)selectedBike=true;
                else if(name=="delete_area")deletedArea=true;
            }
            if(picked)coverage["gang_model_minus_one"]=coverage["gang_model_minus_one"].get<int>()+1;
            if(selectedBike&&!deletedArea&&original["objects"].empty())coverage["copbike_exit"]=coverage["copbike_exit"].get<int>()+1;
            Write({{"event","comparison"},{"pair",i},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",equal?Digest(original):original},{"candidate",equal?Digest(candidate):candidate}});
        }
    }
    hook->State(enabled);hook->LockState(locked);
    const bool restored=std::memcmp(reinterpret_cast<void*>(0xA9B0C8),savedModels.data(),savedModels.size())==0&&std::memcmp(reinterpret_cast<void*>(0x8E4CC0),savedStreaming.data(),savedStreaming.size())==0&&At<uint32>(reinterpret_cast<void*>(0xB7CB84),0)==savedTime&&CObject::nNoTempObjects==savedTemporary&&At<uint16>(reinterpret_cast<void*>(0x8CD57C),0)==savedBarrier&&At<uint16>(reinterpret_cast<void*>(0x8CD580),0)==savedWreck;
    bool covered=true;for(const auto& value:coverage)covered&=value.get<int>()>0;
    Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",1024},{"coverage",coverage},{"coverage_gate",covered}});
    Write({{"event",!failures&&restored&&covered?"complete":"error"},{"pairs",1024},{"state_restored",restored},{"coverage_gate",covered},{"coverage",coverage}});
}
}
