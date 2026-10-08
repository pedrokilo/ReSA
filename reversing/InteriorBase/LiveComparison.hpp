#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Interior/InteriorGroup_c.h"
#include "Interior/Interior_c.h"
#include "../ProbeRuntime.hpp"
namespace InteriorBaseLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
inline std::string Bytes(const void* ptr, size_t size) { const auto* p = static_cast<const uint8*>(ptr); std::string out(size*2,'0'); constexpr char digits[]="0123456789abcdef"; for(size_t i=0;i<size;++i){out[2*i]=digits[p[i]>>4];out[2*i+1]=digits[p[i]&15];}return out; }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,6> fns{{
{"Interior_c","GetFurnitureEntity",0x5913B0,0,-1,1024},
{"Interior_c","CheckTilesEmpty",0x591680,0,-1,4096},
{"Interior_c","SetTilesStatus",0x591700,0,-1,8192},
{"Interior_c","SetCornerTiles",0x5917C0,0,-1,4096},
{"Interior_c","GetTileStatus",0x5918E0,0,-1,4096},
{"Interior_c","GetNumEmptyTiles",0x591920,0,-1,4096},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
 ReversibleHooks::ReversibleHook::Base* found{}; ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();});return found;
}
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
inline void Update() {
 if(done)return; char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"interior-base-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"interior-base-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior-base");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 bool passed=true;uint32 pairs=0;
 RestorationProof restoration;
 for(const auto& target:fns){auto* targetHook=FindHook(target.cls,target.name);if(!targetHook){Write({{"event","error"},{"reason","missing hook before probe"},{"state_restored",false}});return;}restoration.SaveHook(targetHook,target.address);}

 for(const auto& fn:fns){
 auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
 const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 // Subordinate tile entry follows the tested side, so helper calls use matching implementations.
 auto* tileHook=FindHook("Interior_c","GetTileStatus");auto* setHook=FindHook("Interior_c","SetTilesStatus");
 const bool tileEnabled=tileHook->Hooked(),tileLocked=tileHook->Locked(),setEnabled=setHook->Hooked(),setLocked=setHook->Locked();tileHook->LockState(false);setHook->LockState(false);
 for(int i=0;i<fn.cases;++i){
 auto run=[&](bool candidate){
 tileHook->State(candidate);setHook->State(candidate);hook->State(candidate);
 alignas(Interior_c) std::array<uint8,sizeof(Interior_c)> raw{};raw.fill(uint8(i*37));auto* self=reinterpret_cast<Interior_c*>(raw.data());
 std::array<uint8,64> box{};box[2]=uint8(i%30+1);box[3]=uint8((i/30)%30+1);self->m_box=reinterpret_cast<tEffectInterior*>(box.data());
 for(int j=0;j<900;++j)self->field_68[j]=char((i+j*17)%13);
 if(i%4==0)std::fill(std::begin(self->field_68),std::end(self->field_68),0);
 std::array<FurnitureEntity_c,6> nodes{};for(size_t j=0;j<nodes.size();++j){nodes[j].m_pNext=j+1<nodes.size()?&nodes[j+1]:nullptr;nodes[j].m_entity=reinterpret_cast<CEntity*>(0x100000+j*16);}
 *reinterpret_cast<FurnitureEntity_c**>(raw.data()+0x5C)=i%7==6?nullptr:&nodes[0];
 const int32 x=i%35-2,y=(i/35)%35-2,width=(i/11)%8-1,height=(i/7)%8-1,status=(i/3)%16-2,direction=(i/5)%7-1;uint32 result=0;
 const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;
 if(fn.address==0x5913B0){auto* ptr=reinterpret_cast<FurnitureEntity_c*(__thiscall*)(Interior_c*,CEntity*)>(fn.address)(self,reinterpret_cast<CEntity*>(0x100000+(i%8)*16));result=ptr?uint32(ptr-nodes.data()+1):0;}
 else if(fn.address==0x591680)result=uint8(reinterpret_cast<int8(__thiscall*)(Interior_c*,int32,int32,int32,int32,uint8)>(fn.address)(self,x,y,width,height,uint8(i%3)));
 else if(fn.address==0x591700)reinterpret_cast<void(__thiscall*)(Interior_c*,int32,int32,int32,int32,int32,int8)>(fn.address)(self,x,y,width,height,status,int8(i%3));
 else if(fn.address==0x5917C0)reinterpret_cast<void(__thiscall*)(Interior_c*,int32,int32,int32,uint8)>(fn.address)(self,direction,width,status,uint8(i%3));
 else if(fn.address==0x5918E0)result=uint32(reinterpret_cast<int32(__thiscall*)(Interior_c*,int32,int32)>(fn.address)(self,x,y));
 else result=uint32(reinterpret_cast<int32(__thiscall*)(Interior_c*,int32,int32,int32,int32)>(fn.address)(self,x,y,direction,i%7+1));
 // Verify actual receiver bindings before replacing addresses by deterministic markers.
 const bool bindingsPreserved = self->m_box == reinterpret_cast<tEffectInterior*>(box.data())
     && self->m_list.GetHead() == (i%7==6 ? nullptr : &nodes[0]);
 self->m_box=reinterpret_cast<tEffectInterior*>(0x11223344);*reinterpret_cast<FurnitureEntity_c**>(raw.data()+0x5C)=reinterpret_cast<FurnitureEntity_c*>(0x55667788);
 return json{{"bytes_hex",Bytes(raw.data(),raw.size())},{"result",result},{"bindings_preserved",bindingsPreserved},{"switches",switched}};
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true&&original["bindings_preserved"]==true;passed&=ok;failures+=!ok;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 tileHook->State(tileEnabled);tileHook->LockState(tileLocked);setHook->State(setEnabled);setHook->LockState(setLocked);hook->State(enabled);hook->LockState(locked);
 Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
 }
 const bool restored=restoration.Verify();passed&=restored;
 Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
}
}
