#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/Interior_c.h"
#include "Interior/Furniture_c.h"
#include "Interior/FurnitureManager_c.h"
#include "Interior/InteriorManager_c.h"
#include "AtomicModelInfo.h"
#include "Building.h"
#include "../ProbeRuntime.hpp"
namespace InteriorPlacementLive {
inline json trace;
inline int scenario{}, attempt{};
alignas(16) inline std::array<uint8,sizeof(Interior_c)> interiorBytes;
alignas(16) inline std::array<uint8,sizeof(CObject)> entityBytes;
alignas(16) inline std::array<uint8,sizeof(CAtomicModelInfo)> modelBytes;
alignas(16) inline std::array<uint8,sizeof(CColModel)> colBytes;
inline std::array<uintptr_t,22> virtuals;
inline FurnitureEntity_c node;
inline Furniture_c furniture;
inline CMatrix constructorMatrix;
inline std::array<uint32,12> placedMatrix;
inline std::array<uint8,64> boxBytes;
template<class T> T& At(void* p,size_t off){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+off);}
inline uint32 Bits(float x){return std::bit_cast<uint32>(x);}
inline uintptr_t PointerId(const void* p){const auto value=uintptr_t(p);if(!value)return 0;const auto in=uintptr_t(interiorBytes.data()),entity=uintptr_t(entityBytes.data()),furn=uintptr_t(&furniture),item=uintptr_t(&node);if(value>=in&&value<in+sizeof(Interior_c))return 0x10000+value-in;if(value>=entity&&value<entity+sizeof(CObject))return 0x20000+value-entity;if(value>=furn&&value<furn+sizeof(Furniture_c))return 0x30000+value-furn;if(value>=item&&value<item+sizeof(FurnitureEntity_c))return 0x40000+value-item;return value;}
inline void MatrixRecord(CMatrix& m){const CVector vectors[]{m.GetRight(),m.GetForward(),m.GetUp(),m.GetPosition()};int k=0;for(const auto& v:vectors){placedMatrix[k++]=Bits(v.x);placedMatrix[k++]=Bits(v.y);placedMatrix[k++]=Bits(v.z);}}
inline int32 __cdecl Random(){return (scenario*1877+8191)&32767;}
inline int32 __cdecl Range(int32 lo,int32 hi){trace.push_back({"range",lo,hi});return lo+(hi>lo?(scenario*17)%(hi-lo):0);}
inline void* __cdecl Allocate(unsigned size){trace.push_back({"allocate",size});return entityBytes.data();}
inline void __fastcall StaticMatrix(CPlaceable* self,void*){At<CMatrix*>(self,0x14)=&constructorMatrix;}
inline void __fastcall ModelNoCreate(CEntity* self,void*,int32 model){At<int16>(self,0x22)=int16(model);}
inline void __fastcall Model(CEntity* self,void*,int32 model){trace.push_back({"model",model});std::memset(self,0x5A,sizeof(CBuilding));At<uintptr_t>(self,0)=uintptr_t(virtuals.data());At<uint32>(self,0x1C)=0x201008;At<int16>(self,0x22)=int16(model);}
inline void __fastcall ObjectInit(CObject* self,void*){std::memset(self,0x5A,sizeof(CObject));At<uintptr_t>(self,0)=uintptr_t(virtuals.data());At<uint32>(self,0x1C)=0x201008;At<uint32>(self,0x140)=0x41000;At<int16>(self,0x22)=2000;}
inline void __fastcall SetStatic(CEntity* self,void*,bool state){trace.push_back({"static",state});self->m_bIsStatic=state;}
inline void* __fastcall Delete(CEntity* self,void*,uint32 flags){trace.push_back({"delete",flags});return self;}
inline void __fastcall Matrix(CPlaceable* self,void*,CMatrix& m){MatrixRecord(m);trace.push_back({"matrix",placedMatrix,PointerId(self)});}
inline void __cdecl Add(CEntity* entity){trace.push_back({"add",PointerId(entity)});}
inline void __cdecl Remove(CEntity* entity){trace.push_back({"remove",PointerId(entity)});}
inline int8 __fastcall Empty(Interior_c*,void*,int32 x,int32 y,int32 w,int32 d,uint8 window){trace.push_back({"empty",x,y,w,d,window});return int8(scenario%5!=0);}
inline void __fastcall Tiles(Interior_c*,void*,int32 x,int32 y,int32 w,int32 d,int32 state,int8 replace){trace.push_back({"tiles",x,y,w,d,state,replace});}
inline Furniture_c* __fastcall Furniture(FurnitureManager_c*,void*,int32 group,int32 subgroup,int16 id,uint8 wealth){trace.push_back({"furniture",group,subgroup,id,wealth});return scenario%7==0?nullptr:&furniture;}
inline CEntity* __fastcall Place(Interior_c* self,void*,Furniture_c* item,int32 x,int32 y,float height,int32 mode,int32 direction,int32* w,int32* d,uint8 skip){trace.push_back({"place",x,y,Bits(height),mode,direction,skip,attempt,PointerId(self),PointerId(item)});++attempt;if(scenario%5==0||attempt<scenario%4)return nullptr;*w=17;*d=23;return reinterpret_cast<CEntity*>(entityBytes.data());}
inline bool __fastcall Setup(InteriorManager_c* manager,void*,Interior_c* self){trace.push_back({"setup",uintptr_t(manager),PointerId(self)});return scenario%4!=0;}
inline int32 __fastcall Find(InteriorManager_c* manager,void*,int32 interior,int32 model,CVector pos){trace.push_back({"find",interior,model,Bits(pos.x),Bits(pos.y),Bits(pos.z),uintptr_t(manager)});return scenario%4==1?-1:scenario%3;}
inline ReversibleHooks::ReversibleHook::Base* Hook(const char* name){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="Interior_c")for(auto& p:c.Items())if(p->Name()==name)h=p.get();});return h;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,4> fns{{{"Interior_c","PlaceFurniture",0x592AA0,0,-1,1024},{"Interior_c","PlaceFurnitureOnWall",0x593120,0,-1,1024},{"Interior_c","PlaceFurnitureInCorner",0x593340,0,-1,1024},{"Interior_c","PlaceObject",0x5934E0,0,-1,1024}}};
inline bool done{};inline std::ofstream log;
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
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
 if(done)return;
 char path[MAX_PATH]{};GetModuleFileNameA(nullptr,path,MAX_PATH);const auto dir=std::filesystem::path(path).parent_path();
 if(!std::filesystem::exists(dir/"interior-placement-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-placement-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-placement");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 bool passed=true;uint32 pair=0;
 RestorationProof restoration;
 for(const auto& target:fns){auto* targetHook=Hook(target.name);if(!targetHook){Write({{"event","error"},{"reason","missing hook before probe"},{"state_restored",false}});return;}restoration.SaveHook(targetHook,target.address);}

 auto& pool=StaticRef<TList_c<FurnitureEntity_c>>(0xBAD3EC);const auto savedPool=ReversingProbe::Bytes(&pool,12);const auto savedModel=StaticRef<uintptr_t>(0xA9B0C8+2000*4);const auto savedObjects=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3A18),900);
 {
 virtuals.fill(0);virtuals[0]=uintptr_t(&Delete);virtuals[4]=uintptr_t(&SetStatic);virtuals[6]=uintptr_t(&Model);
 EntryPatch random(0x821B1E,Random),randomDll(bit_cast<uintptr_t>(&std::rand),Random),range(0x407180,Range),rangeDll(bit_cast<uintptr_t>(static_cast<int32(*)(int32,int32)>(&CGeneral::GetRandomIntInRange)),Range);
 EntryPatch allocateBuilding(0x404090,Allocate),allocateBuildingDll(bit_cast<uintptr_t>(&CBuilding::operator new),Allocate);
 EntryPatch allocateObject(0x5A1EE0,Allocate),allocateObjectDll(bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CObject::operator new)),Allocate);
 EntryPatch allocateMatrix(0x54F4C0,StaticMatrix),allocateMatrixDll(bit_cast<uintptr_t>(&CPlaceable::AllocateStaticMatrix),StaticMatrix);
 // Obtain the DLL method bodies from a constructed DLL building. The EXE's
 // inherited building VMT may still point to native entries, and virtual member
 // pointers may name dispatch thunks rather than the implementation.
 alignas(CBuilding) std::array<uint8,sizeof(CBuilding)> vtableProbe{};
 auto* buildingProbe=::new(vtableProbe.data()) CBuilding;
 const auto* dllVtable=*reinterpret_cast<const uintptr_t* const*>(buildingProbe);
 EntryPatch modelDll(dllVtable[6],Model),model(0x533700,Model);
 EntryPatch objectInit(0x59F840,ObjectInit),objectInitDll(bit_cast<uintptr_t>(&CObject::Init),ObjectInit);
 EntryPatch matrix(0x54F610,Matrix),matrixDll(bit_cast<uintptr_t>(&CPlaceable::SetMatrix),Matrix);
 EntryPatch add(0x563220,Add),addDll(bit_cast<uintptr_t>(&CWorld::Add),Add),remove(0x563280,Remove),removeDll(bit_cast<uintptr_t>(&CWorld::Remove),Remove);
 EntryPatch empty(0x591680,Empty),emptyDll(bit_cast<uintptr_t>(&Interior_c::CheckTilesEmpty),Empty),tiles(0x591700,Tiles),tilesDll(bit_cast<uintptr_t>(&Interior_c::SetTilesStatus),Tiles);
 EntryPatch furn(0x5911E0,Furniture),furnDll(bit_cast<uintptr_t>(&FurnitureManager_c::GetFurniture),Furniture);
 EntryPatch setup(0x5982B0,Setup),setupDll(bit_cast<uintptr_t>(&InteriorManager_c::HasInteriorHadStealDataSetup),Setup);
 EntryPatch find(0x5982F0,Find),findDll(bit_cast<uintptr_t>(static_cast<int32(InteriorManager_c::*)(int32,int32,CVector) const>(&InteriorManager_c::FindStealableObjectId)),Find);
 restoration.SavePatch(random);restoration.SavePatch(randomDll);restoration.SavePatch(range);restoration.SavePatch(rangeDll);restoration.SavePatch(allocateBuilding);restoration.SavePatch(allocateBuildingDll);restoration.SavePatch(allocateObject);restoration.SavePatch(allocateObjectDll);restoration.SavePatch(allocateMatrix);restoration.SavePatch(allocateMatrixDll);restoration.SavePatch(modelDll);restoration.SavePatch(model);restoration.SavePatch(objectInit);restoration.SavePatch(objectInitDll);restoration.SavePatch(matrix);restoration.SavePatch(matrixDll);restoration.SavePatch(add);restoration.SavePatch(addDll);restoration.SavePatch(remove);restoration.SavePatch(removeDll);restoration.SavePatch(empty);restoration.SavePatch(emptyDll);restoration.SavePatch(tiles);restoration.SavePatch(tilesDll);restoration.SavePatch(furn);restoration.SavePatch(furnDll);restoration.SavePatch(setup);restoration.SavePatch(setupDll);restoration.SavePatch(find);restoration.SavePatch(findDll);
 for(const auto& fn:fns){auto* hook=Hook(fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const auto old=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;
  // Only wall/corner fixtures replace PlaceFurniture, so the placement entry itself remains the target in its own bank cases.
  std::unique_ptr<EntryPatch> place,placeDll;
  if(fn.address==0x593120||fn.address==0x593340){place=std::make_unique<EntryPatch>(0x592AA0,Place);placeDll=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&Interior_c::PlaceFurniture),Place);}
  if(place)restoration.SavePatch(*place);if(placeDll)restoration.SavePatch(*placeDll);
  for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);scenario=i;attempt=0;trace=json::array();placedMatrix.fill(0xDEADBEEF);
    interiorBytes.fill(uint8(i*11));entityBytes.fill(0x5A);boxBytes.fill(0);auto* self=reinterpret_cast<Interior_c*>(interiorBytes.data());
    self->m_box=reinterpret_cast<tEffectInterior*>(boxBytes.data());boxBytes[2]=uint8(3+i%22);boxBytes[3]=uint8(4+i%19);boxBytes[4]=uint8(2+i%9);boxBytes[31]=uint8(i%256);
    self->m_interiorId=i-513;self->m_areaCode=i%16;self->m_list.RemoveAll();
    std::memset(&self->m_matrix,0,sizeof(RwMatrix));self->m_matrix.right={1,0,0};self->m_matrix.up={0,1,0};self->m_matrix.at={0,0,1};self->m_matrix.pos={float(i%13)-7.0f,float(i%17)-8.0f,float(i%9)*.25f};
    pool.RemoveAll();node={};node.m_entity=reinterpret_cast<CEntity*>(0x1234);node.m_tileX=0xCAFE;node.m_tileY=0xBEEF;
    if(i%8!=0)pool.AddItem(&node);if(i%8==1)At<int32>(&pool,8)=0;
    if(i%8==2){At<uintptr_t>(&pool,0)=0;At<uintptr_t>(&pool,4)=0;At<int32>(&pool,8)=1;}
    if(i>=896 && i<960){pool.RemoveAll();pool.AddItem(&node);At<int32>(&pool,8)=-1;}
    std::memset(&furniture,0,sizeof(furniture));furniture.m_nModelId=2000;furniture.m_nWidthX=int8(1+i%8);furniture.m_nWidthY=int8(1+(i/8)%8);furniture.m_bCanPlaceInFrontOfWindow=i%2!=0;furniture.m_bIsTall=(i/2)%2!=0;furniture.m_nMaxAng=int8((i/4)%3==0?0:(i/4)%3==1?12:130);
    modelBytes.fill(0);colBytes.fill(0);At<uintptr_t>(modelBytes.data(),0x14)=uintptr_t(colBytes.data());At<float>(colBytes.data(),8)=i>=960?.3f:float(i%5-2)*.25f;StaticRef<uintptr_t>(0xA9B0C8+2000*4)=uintptr_t(modelBytes.data());
    std::memset(reinterpret_cast<void*>(0xBB3A18),0x37,900);StaticRef<int32>(0xBB3A18)=0;for(int n=0;n<3;++n)StaticRef<uint8>(0xBB3A34+n*28)=uint8(i%4==2);
    const int32 direction=i%9-2,placement=(i/9)%5-1;int32 outputs[6]{-99,-98,-97,-96,-95,-94};uintptr_t result=0;
    const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
    if(fn.address==0x592AA0)result=uintptr_t(reinterpret_cast<CEntity*(__thiscall*)(Interior_c*,Furniture_c*,int32,int32,float,int32,int32,int32*,int32*,uint8)>(fn.address)(self,&furniture,i>=960?(i%2?0x1000001:-0x1000001):i%17-3,i>=960?(i%2?-0x1000001:0x1000001):i%13-2,i>=960?.1f:float(i%9)*.125f,placement,direction,&outputs[0],&outputs[1],uint8(i>=960?1:i%2)));
    else if(fn.address==0x593120)result=uintptr_t(reinterpret_cast<CEntity*(__thiscall*)(Interior_c*,int32,int32,int32,float,int32,int32,int32,int32,int32*,int32*,int32*,int32*,int32*,int32*)>(fn.address)(self,i%9,i%11,i%29,float(i%9)*.125f,placement,i%3?-1:direction,i%4?-1:i%17,i%5, i%2?outputs:nullptr,i%4?outputs+1:nullptr,i%8?outputs+2:nullptr,i%16?outputs+3:nullptr,i%32?outputs+4:nullptr,i%64?outputs+5:nullptr));
    else if(fn.address==0x593340)result=uintptr_t(reinterpret_cast<CEntity*(__thiscall*)(Interior_c*,int32,int32,int32,float,int32,int32,int32,int32*,int32*,int32*,int32*,int32*)>(fn.address)(self,i%9,i%11,i%29,float(i%9)*.125f,placement,i%3?-1:direction,i%5,i%2?outputs:nullptr,i%4?outputs+1:nullptr,i%8?outputs+2:nullptr,i%16?outputs+3:nullptr,i%32?outputs+4:nullptr));
    else result=uintptr_t(reinterpret_cast<CObject*(__thiscall*)(Interior_c*,uint8,Furniture_c*,float,float,float,float)>(fn.address)(self,uint8((i/4)%2),&furniture,float(i%17)-3.5f,float(i%13)-2.75f,i>=960?.1f:float(i%9)*.125f,float(i%7)*31.0f));
    return json{{"interior",ReversingProbe::Bytes(interiorBytes.data(),0x794)},{"switched",switched},{"result",result},{"outputs",outputs},{"trace",trace},{"pool",ReversingProbe::Bytes(&pool,12)},{"list",ReversingProbe::Bytes(&self->m_list,12)},{"node",ReversingProbe::Bytes(&node,sizeof(node))},{"objects",ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3A18),900)},{"entity",ReversingProbe::Bytes(entityBytes.data(),fn.address==0x5934E0?sizeof(CObject):sizeof(CBuilding))}};
   };const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switched"]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",pair++},{"function",std::string("Interior_c::")+fn.name},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});}
  hook->State(old);hook->LockState(locked);Write({{"event","function_summary"},{"function",std::string("Interior_c::")+fn.name},{"failures",failures},{"cases",fn.cases}});
 }
 std::memcpy(&pool,savedPool.data(),12);StaticRef<uintptr_t>(0xA9B0C8+2000*4)=savedModel;std::memcpy(reinterpret_cast<void*>(0xBB3A18),savedObjects.data(),900);
 } // All collaborator EntryPatch destructors have restored their saved entries.
 const bool restored=restoration.Verify()&&ReversingProbe::Bytes(&pool,12)==savedPool&&StaticRef<uintptr_t>(0xA9B0C8+2000*4)==savedModel&&ReversingProbe::Bytes(reinterpret_cast<void*>(0xBB3A18),900)==savedObjects;
 passed&=restored;
 Write({{"event",passed?"complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
