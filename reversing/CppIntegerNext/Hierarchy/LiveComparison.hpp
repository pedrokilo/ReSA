#pragma once
#include <fstream>
#include <set>
#include "RwHelper.h"
#include "Pools/Pools.h"
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "RenderWare/rw/rphanim.h"
#include "NativeProof.hpp"
#include "../../ProbeRuntime.hpp"
#include "../../OriginalDependencies499/HookRoute.hpp"
namespace CppIntegerHierarchyLive {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"RwHelper","GetAnimHierarchyFromFrame",0x734AB0,0,-1,2048}
};
inline bool done{};inline std::ofstream log;inline DWORD resourceWaitStarted{},lastResourceDiagnostic{};
inline void Write(const json& r);
struct Result{uintptr_t eax;bool abi;};
__declspec(noinline) inline Result Call(uintptr_t entry,uintptr_t frame){
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
  push frame
  call entry
  mov eaxCaptured,eax
  add esp,4
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
template<size_t N> inline bool Native(uintptr_t at,const std::array<uint8,N>& bytes){return Resolve(at)==at&&ModuleIs(at,GetModuleHandleA(nullptr))&&std::equal(bytes.begin(),bytes.end(),reinterpret_cast<const uint8*>(at));}
inline bool Libraries(){return Native(0x734A70,native_734A70)&&Native(0x7C5160,native_7C5160)&&Native(0x7F0DC0,native_7F0DC0)&&Native(0x7F0410,native_7F0410)&&Native(0x7F05A0,native_7F05A0)&&Native(0x7F0360,native_7F0360);}
inline RpHAnimHierarchy* Direct(RwFrame* f,int32 offset){return *reinterpret_cast<RpHAnimHierarchy**>(reinterpret_cast<uint8*>(f)+offset+4);}
// Matches the actual callback: nested success does not stop its caller's sibling loop.
inline void Children(RwFrame* f,int32 offset,RpHAnimHierarchy*& result,int depth=0){if(depth>256)return;for(auto* c=f->child;c;c=c->next){if(auto* h=Direct(c,offset)){result=h;break;}Children(c,offset,result,depth+1);}}
struct Fixture{RwFrame* frame;RpHAnimHierarchy* expected;uint32 kind;};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cppintegerhierarchy-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cppintegerhierarchy-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cppintegerhierarchy");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 // g25b engancha RpHAnimFrameGetHierarchy (0x7C5160): este banco exige las bibliotecas nativas, asi que el hook queda apagado mientras corre (HookRoute lo restaura al salir).
 OriginalDependencies499::HookRoute libraryRoute("RpHAnim","RpHAnimFrameGetHierarchy",0x7C5160,OriginalDependencies499::HookRoute::RegisteredTarget{});
 static_assert(offsetof(RwFrame,child)==0x98);static_assert(offsetof(RwFrame,next)==0x9C);
 const int32 offset=*reinterpret_cast<const int32*>(0xC9B8C8);if(offset<int32(sizeof(RwFrame))||offset>4096||!Libraries()){RestoreCanonical();Write({{"event","error"},{"reason","Native libraries or HAnim plugin offset unavailable"}});done=true;return;}
 std::vector<RwFrame*> frames;std::set<RwFrame*> seen;bool treeLegal=true;
 auto collect=[&](RwFrame* root){std::vector<std::pair<RwFrame*,int>> pending{{root,0}};while(!pending.empty()){auto [f,depth]=pending.back();pending.pop_back();if(!f)continue;if(depth>256||seen.size()>8192){treeLegal=false;break;}if(!seen.insert(f).second)continue;frames.push_back(f);std::set<RwFrame*> siblings;for(auto* c=f->child;c;c=c->next){if(c->object.parent!=f||!siblings.insert(c).second||siblings.size()>8192){treeLegal=false;break;}pending.push_back({c,depth+1});}}};
 auto entity=[&](CEntity* e){if(!e||!e->GetRwObject()||RwObjectGetType(e->GetRwObject())!=rpCLUMP)return;collect(RpClumpGetFrame(e->GetRpClump()));};
 if(auto* pool=GetPedPool())for(uint32 i=0;i<pool->GetSize();++i)entity(pool->GetAt(i));if(auto* pool=GetVehiclePool())for(uint32 i=0;i<pool->GetSize();++i)entity(pool->GetAt(i));
 if(!treeLegal){RestoreCanonical();Write({{"event","error"},{"reason","Real frame topology exceeded legal bound"}});done=true;return;}
 std::array<std::vector<Fixture>,4> fixtures;for(auto* f:frames){auto* h=Direct(f,offset);if(h)fixtures[2].push_back({f,h,2});else{RpHAnimHierarchy* expected{};Children(f,offset,expected);if(expected)fixtures[3].push_back({f,expected,3});else if(f->child)fixtures[1].push_back({f,nullptr,1});}}
 if(fixtures[1].empty()||fixtures[2].empty()||fixtures[3].empty()||FindPlayerPed()->GetAreaCode()!=0){
  const auto now=GetTickCount();if(!resourceWaitStarted)resourceWaitStarted=now;const auto elapsed=now-resourceWaitStarted;
  RestoreCanonical();if(!lastResourceDiagnostic||now-lastResourceDiagnostic>=5000||elapsed>=60000){lastResourceDiagnostic=now;Write({{"event",elapsed>=60000?"error":"waiting_for_real_frame_resources"},{"reason","Required exterior real hierarchy/child branches missing"},{"elapsed_ms",elapsed},{"player_area",FindPlayerPed()->GetAreaCode()},{"real_frames",frames.size()},{"fixture_counts",{0,fixtures[1].size(),fixtures[2].size(),fixtures[3].size()}}});}
  RestoreCanonical();canonicalActive=false;if(elapsed>=60000)done=true;return;
 }
 done=true;
 const uintptr_t globals=*reinterpret_cast<const uintptr_t*>(0xC97B24),frameModule=globals+*reinterpret_cast<const uint32*>(0xC9796C),dirty=globals+0xBC,first=*reinterpret_cast<const uintptr_t*>(dirty);const auto moduleBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(frameModule),4),dirtyBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(dirty),8),firstBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(first),8);
 std::vector<std::pair<uintptr_t,std::vector<uint8>>> snapshots;for(auto* f:frames)snapshots.push_back({uintptr_t(f),ReversingProbe::Bytes(f,offset+sizeof(RpHAnimFrameExtension))});
 auto* empty=reinterpret_cast<RwFrame*(__cdecl*)()>(0x7F0410)();if(!empty){RestoreCanonical();Write({{"event","error"},{"reason","Native frame allocation failed"}});return;}
 const bool emptyLegal=!empty->child&&!empty->next&&!Direct(empty,offset);fixtures[0].push_back({empty,nullptr,0});const auto emptyBefore=ReversingProbe::Bytes(empty,offset+sizeof(RpHAnimFrameExtension));
 auto globalsOK=[&]{return moduleBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(frameModule),4)&&dirtyBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(dirty),8)&&firstBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(first),8);};auto borrowedOK=[&]{for(const auto&[at,bytes]:snapshots)if(bytes!=ReversingProbe::Bytes(reinterpret_cast<void*>(at),bytes.size()))return false;return true;};
 OriginalDependencies499::HookRoute route("RwHelper","GetAnimHierarchyFromFrame",0x734AB0,OriginalDependencies499::HookRoute::RegisteredTarget{});const uintptr_t dllCallback=Resolve(reinterpret_cast<uintptr_t>(&GetAnimHierarchyFromFrameCallback));const bool callbackDllOk=dllCallback!=0x734A70&&ModuleIs(dllCallback,ProbeModule());RestoreCanonical();Write({{"event","begin"},{"registered_hook",route.Registered()},{"callback_dll_ok",callbackDllOk},{"callback_dll",dllCallback},{"library_route_ok",Libraries()},{"real_frames",frames.size()},{"player_area",FindPlayerPed()->GetAreaCode()},{"fixture_configuration","FastLoader.SaveGameToLoad=0"},{"owned_empty_created",true},{"empty_frame_legal",emptyLegal},{"collaborators_stubbed",false},{"fp_state_restored",CanonicalRestored()},{"hanim_plugin_offset",offset},{"native_callback_entry",0x734A70},{"native_callback_bytes",native_734A70},{"native_getter_bytes",native_7C5160},{"native_children_bytes",native_7F0DC0}});
 bool passed=emptyLegal;uint32 failures=0;std::array<uint32,4> kinds{};
 for(int scenario=0;scenario<2048;++scenario){const uint32 kind=scenario%4;const auto& fixture=fixtures[kind][uint32(scenario/4)%fixtures[kind].size()];auto run=[&](bool candidate){RestoreCanonical();bool routeOK=false;const bool nativeBefore=Libraries();const auto result=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(0x734AB0)==candidate;if(candidate)routeOK&=Resolve(0x734AB0)==route.Body()&&ModuleIs(route.Body(),ProbeModule());return Call(0x734AB0,uintptr_t(fixture.frame));});const bool ro=borrowedOK()&&globalsOK()&&emptyBefore==ReversingProbe::Bytes(empty,emptyBefore.size());RestoreCanonical();json value={{"return_bits",result.eax},{"expected_pointer",uintptr_t(fixture.expected)},{"fixture_kind",kind},{"has_children",fixture.frame->child!=nullptr},{"direct_hierarchy",Direct(fixture.frame,offset)!=nullptr},{"route_ok",routeOK},{"library_route_ok",nativeBefore&&Libraries()},{"stack_restored",result.abi},{"registers_preserved",result.abi},{"contract_ok",result.eax==uintptr_t(fixture.expected)},{"readonly_ok",ro},{"fp_state_restored",CanonicalRestored()}};RestoreCanonical();return value;};
 const auto original=run(false),candidate=run(true);bool equal=route.Registered()&&original==candidate;for(auto k:{"route_ok","library_route_ok","stack_restored","registers_preserved","contract_ok","readonly_ok","fp_state_restored"})equal&=original[k]==true;passed&=equal;failures+=!equal;++kinds[kind];Write({{"event","comparison"},{"pair",scenario},{"case",scenario},{"function","RwHelper::GetAnimHierarchyFromFrame"},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});}
 bool coverage=true;for(auto n:kinds)coverage&=n==512;passed&=coverage;Write({{"event","function_summary"},{"function","RwHelper::GetAnimHierarchyFromFrame"},{"cases",2048},{"failures",failures},{"complete_paths",coverage},{"fixture_counts",kinds}});
 const bool mainRestore=route.Restore();const bool destroyed=reinterpret_cast<RwBool(__cdecl*)(RwFrame*)>(0x7F05A0)(empty)!=0;const bool resources=destroyed&&borrowedOK()&&globalsOK();RestoreCanonical();const bool fp=CanonicalRestored(),restored=mainRestore&&resources&&fp&&Libraries();Write({{"event",passed&&restored?"complete":"error"},{"pairs",2048},{"state_restored",restored},{"resources_restored",resources},{"owned_empty_destroyed",destroyed},{"library_route_ok",Libraries()},{"fp_state_restored",fp}});
}
}
