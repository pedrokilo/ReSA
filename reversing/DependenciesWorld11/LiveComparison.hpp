#pragma once
#include <fstream>
#include "RwHelper.h"
#include "TxdStore.h"
#include "Core/KeyGen.h"
#include "NativeProof.hpp"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace DependenciesWorld11Live {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"RwHelper","RwTexDictionaryFindHashNamedTexture",0x734E50,0,-1,2048}
};
inline bool done{};inline std::ofstream log;
inline void Write(const json& r);
struct Result{uintptr_t eax;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uintptr_t dict,uint32 hash){
 uint32 before{},after{},rb{},rs{},rd{},fb{},fa{},eaxCaptured{};
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  push hash
  push dict
  call entry
  mov eaxCaptured,eax
  add esp,8
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {eaxCaptured,before==after&&fb==fa&&rb==0x13579BDF&&rs==0x2468ACE0&&rd==0x10293847};
}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
alignas(16) inline std::array<uint8,108> canonicalFP{};inline uint32 canonicalMXCSR{};inline bool canonicalActive{};
inline void RestoreCanonical(){if(canonicalActive){RestoreFP(canonicalFP.data());WriteMXCSR(canonicalMXCSR);}}
inline bool CanonicalRestored(){alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==canonicalFP&&ReadMXCSR()==canonicalMXCSR;}
inline void Write(const json& r){alignas(16)std::array<uint8,108> current{};const auto mx=ReadMXCSR();if(!canonicalActive)SaveFP(current.data());RestoreCanonical();ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();if(canonicalActive)RestoreCanonical();else{RestoreFP(current.data());WriteMXCSR(mx);}}
inline uintptr_t Resolve(uintptr_t e){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(e)==0xE9;++n)e+=5+*reinterpret_cast<const int32*>(e+1);return e;}
inline bool ModuleIs(uintptr_t e,HMODULE expected){HMODULE owner{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(e),&owner)&&owner==expected;}
inline HMODULE ProbeModule(){HMODULE p{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Call),&p);return p;}
inline bool Libraries(){return Resolve(0x7F3600)==0x7F3600&&Resolve(0x7F36A0)==0x7F36A0&&ModuleIs(0x7F3600,GetModuleHandleA(nullptr))&&ModuleIs(0x7F36A0,GetModuleHandleA(nullptr))&&std::equal(native_7F3600.begin(),native_7F3600.end(),reinterpret_cast<const uint8*>(0x7F3600))&&std::equal(native_7F36A0.begin(),native_7F36A0.end(),reinterpret_cast<const uint8*>(0x7F36A0));}
struct Dictionary {RwTexDictionary* pointer;std::vector<RwTexture*> textures;std::vector<uint32> hashes;uint32 missing;};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependenciesworld11-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependenciesworld11-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesworld11");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 static_assert(offsetof(RwTexture,lInDictionary)==8);static_assert(offsetof(RwTexture,name)==16);static_assert(offsetof(RwTexDictionary,texturesInDict)==8);
 std::vector<Dictionary> dicts;auto* pool=CTxdStore::ms_pTxdPool;if(pool)for(uint32 i=0;i<pool->GetSize();++i){auto* slot=pool->GetAt(i);if(!slot||!slot->m_pRwDictionary)continue;Dictionary row{slot->m_pRwDictionary,{},{},0xDEADBEEF};auto* head=&row.pointer->texturesInDict.link;auto* link=head->next;bool valid=true;for(int n=0;link!=head&&n<4096;++n){auto* t=reinterpret_cast<RwTexture*>(reinterpret_cast<uint8*>(link)-8);if(t->dict!=row.pointer||!std::memchr(t->name,0,32)){valid=false;break;}row.textures.push_back(t);row.hashes.push_back(CKeyGen::GetUppercaseKey(t->name));link=link->next;}valid&=link==head;
  if(valid&&row.textures.size()>1&&row.hashes.back()!=row.hashes.front()&&std::find(row.hashes.begin(),row.hashes.end()-1,row.hashes.back())==row.hashes.end()-1){while(std::find(row.hashes.begin(),row.hashes.end(),row.missing)!=row.hashes.end())++row.missing;dicts.push_back(std::move(row));}}
 if(dicts.empty()||!Libraries()){RestoreCanonical();Write({{"event","error"},{"reason","No loaded real dictionary with distinct first/last texture or original RW unavailable"},{"state_restored",CanonicalRestored()}});return;}
 const uintptr_t globals=*reinterpret_cast<uint32*>(0xC97B24),offset=*reinterpret_cast<uint32*>(0xC97B4C),module=globals+offset;const auto moduleBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(module),20);const uintptr_t first=*reinterpret_cast<const uintptr_t*>(module);const auto firstBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(first),8);
 std::vector<std::pair<uintptr_t,std::vector<uint8>>> beforeLive;for(const auto& d:dicts){beforeLive.push_back({uintptr_t(d.pointer),ReversingProbe::Bytes(d.pointer,sizeof(RwTexDictionary))});for(auto* t:d.textures)beforeLive.push_back({uintptr_t(t),ReversingProbe::Bytes(t,sizeof(RwTexture))});}
 auto* empty=reinterpret_cast<RwTexDictionary*(__cdecl*)()>(0x7F3600)();if(!empty){RestoreCanonical();Write({{"event","error"},{"reason","Real RW empty dictionary allocation failed"},{"state_restored",CanonicalRestored()}});return;}
 std::vector<std::pair<uintptr_t,std::vector<uint8>>> readonlyLive;for(const auto& [at,bytes]:beforeLive)readonlyLive.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),bytes.size())});readonlyLive.push_back({uintptr_t(empty),ReversingProbe::Bytes(empty,sizeof(RwTexDictionary))});const auto moduleAllocated=ReversingProbe::Bytes(reinterpret_cast<void*>(module),20);
 auto readonly=[&]{if(moduleAllocated!=ReversingProbe::Bytes(reinterpret_cast<void*>(module),20))return false;for(const auto&[at,bytes]:readonlyLive)if(bytes!=ReversingProbe::Bytes(reinterpret_cast<void*>(at),bytes.size()))return false;return true;};
 using HashFn=uint32(*)(const char*);const auto hashPointer=uintptr_t(static_cast<HashFn>(&CKeyGen::GetUppercaseKey));OriginalDependencies499::HookRoute route("RwHelper","RwTexDictionaryFindHashNamedTexture",0x734E50,&RwTexDictionaryFindHashNamedTexture),helper("CKeyGen","GetUppercaseKey-",0x53CF30,static_cast<HashFn>(&CKeyGen::GetUppercaseKey));const auto helperOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x53CF30),32);std::vector<uint8>helperOn;const bool helperReady=helper.Invoke(true,[&]{helperOn=ReversingProbe::Bytes(reinterpret_cast<void*>(helper.Body()),32);return helper.Registered()&&Resolve(hashPointer)==helper.Body()&&ModuleIs(helper.Body(),ProbeModule())&&ReversingProbe::JumpsToDll(0x53CF30);});
 RestoreCanonical();Write({{"event","begin"},{"registered_hook",route.Registered()},{"helper_registered",helper.Registered()},{"helper_route_ok",helperReady},{"helper_entry",0x53CF30},{"helper_body",helper.Body()},{"helper_off_bytes",helperOff},{"helper_on_bytes",helperOn},{"library_route_ok",Libraries()},{"real_dictionaries",dicts.size()},{"owned_empty_created",true},{"collaborators_stubbed",false},{"fp_state_restored",CanonicalRestored()}});
 bool passed=helperReady;uint32 failures=0;std::array<uint32,4> kinds{};
 for(int scenario=0;scenario<2048;++scenario){const auto& d=dicts[uint32(scenario/4)%dicts.size()];const uint32 kind=scenario%4,hash=kind<2?d.missing:kind==2?d.hashes.front():d.hashes.back();auto* dictionary=kind==0?empty:d.pointer;auto* expected=kind<2?nullptr:kind==2?d.textures.front():d.textures.back();
  auto run=[&](bool candidate){RestoreCanonical();bool helperOK=false,routeOK=false;const bool libraryBefore=Libraries();const auto result=helper.Invoke(candidate,[&]{auto check=[&]{return helper.Registered()&&ReversingProbe::JumpsToDll(0x53CF30)==candidate&&(candidate?(Resolve(hashPointer)==helper.Body()&&ModuleIs(helper.Body(),ProbeModule())&&helperOn==ReversingProbe::Bytes(reinterpret_cast<void*>(helper.Body()),32)):helperOff==ReversingProbe::Bytes(reinterpret_cast<void*>(0x53CF30),32));};helperOK=check();const auto r=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x734E50)==candidate;if(candidate)routeOK&=Resolve(0x734E50)==route.Body()&&ModuleIs(route.Body(),ProbeModule());return Call(0x734E50,uintptr_t(dictionary),hash);});helperOK&=check();return r;});const bool ro=readonly();RestoreCanonical();json value={{"return_bits",result.eax},{"expected_pointer",uintptr_t(expected)},{"fixture_kind",kind},{"hash",hash},{"texture_count",kind==0?0:d.textures.size()},{"route_ok",routeOK},{"helper_route_ok",helperOK},{"library_route_ok",libraryBefore&&Libraries()},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"contract_ok",result.eax==uintptr_t(expected)},{"readonly_ok",ro},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return value;};
  const auto original=run(false),candidate=run(true);bool equal=route.Registered()&&original==candidate;for(auto k:{"route_ok","helper_route_ok","library_route_ok","stack_restored","registers_preserved","contract_ok","readonly_ok","fp_state_restored"})equal&=original[k]==true;passed&=equal;failures+=!equal;++kinds[kind];Write({{"event","comparison"},{"pair",scenario},{"case",scenario},{"function","RwHelper::RwTexDictionaryFindHashNamedTexture"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 bool coverage=true;for(auto n:kinds)coverage&=n==512;passed&=coverage;Write({{"event","function_summary"},{"function","RwHelper::RwTexDictionaryFindHashNamedTexture"},{"cases",2048},{"failures",failures},{"complete_paths",coverage},{"fixture_counts",kinds}});
 const bool mainRestore=route.Restore(),helperRestore=helper.Restore();const bool destroyed=reinterpret_cast<RwBool(__cdecl*)(RwTexDictionary*)>(0x7F36A0)(empty)!=0;bool resourceRestored=destroyed&&moduleBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(module),20)&&firstBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(first),8);for(const auto&[at,bytes]:beforeLive)resourceRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),bytes.size());RestoreCanonical();const bool fp=CanonicalRestored();const bool restored=mainRestore&&helperRestore&&resourceRestored&&fp&&Libraries();Write({{"event",passed&&restored?"complete":"error"},{"pairs",2048},{"state_restored",restored},{"resources_restored",resourceRestored},{"owned_empty_destroyed",destroyed},{"library_route_ok",Libraries()},{"fp_state_restored",fp},{"helper_route_ok",helperRestore}});
}
}
