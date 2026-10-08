#pragma once
#include <bit>
#include <cfenv>
#include <float.h>
#include <xmmintrin.h>
#include <fstream>
#include <map>
#include "Fixtures.hpp"
#include "HexTransport.hpp"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
#include <reversiblehooks/ReversibleHook/Virtual.h>
namespace RequestedPadDistanceTrainNopHexV2 {
using Simple=ReversibleHooks::ReversibleHook::Simple;
struct Range{void* ptr;std::vector<uint8> bytes;};
inline json trace;
inline CVehicle* returnedVehicle{};
inline int vehicleRole{};
inline bool collaboratorValid{};
inline CVehicle* __cdecl FindVehicle(int32 player,bool remote){const bool valid=player==-1&&!remote;collaboratorValid&=valid;trace.push_back({{"event","find_player_vehicle"},{"player",player},{"remote",remote},{"role",vehicleRole},{"arguments_valid",valid}});return returnedVehicle;}
// Full 32-bit x87 environment, including condition codes, TOP, tags and pointers.
using X87Environment=std::array<uint8,28>;
static_assert(sizeof(X87Environment)==28);
__declspec(noinline) inline X87Environment ReadX87Environment(){X87Environment image{};auto* environmentPointer=image.data();__asm {
 mov eax,environmentPointer
 fnstenv [eax]
 fldenv [eax]
 }return image;}
__declspec(noinline) inline void RestoreX87Environment(const X87Environment& image){const auto* environmentPointer=image.data();__asm {
 mov eax,environmentPointer
 fldenv [eax]
 }}
inline json FP(){unsigned control{};_controlfp_s(&control,0,0);return {{"control",control},{"exceptions",_statusfp()},{"mxcsr",_mm_getcsr()}};}
inline void TestFP(){_clearfp();unsigned control{};_controlfp_s(&control,_PC_24|_RC_NEAR|_MCW_EM,_MCW_PC|_MCW_RC|_MCW_EM);_mm_setcsr(0x1F80);}
// Independent arithmetic model transcribed from native x87 order (z+y+x or y+x), not CVector code.
__declspec(noinline) inline std::pair<float,float> Measure(CVector camera,CVector point,bool two){
 float cameraX=camera.x,cy=camera.y,cz=camera.z,px=point.x,py=point.y,pz=point.z,sum,mag;
 if(two){__asm {
  fld cy
  fsub py
  fmul st(0),st(0)
  fld cameraX
  fsub px
  fmul st(0),st(0)
  faddp st(1),st(0)
  fst sum
  fsqrt
  fstp mag
 }}else{__asm {
  fld cz
  fsub pz
  fmul st(0),st(0)
  fld cy
  fsub py
  fmul st(0),st(0)
  faddp st(1),st(0)
  fld cameraX
  fsub px
  fmul st(0),st(0)
  faddp st(1),st(0)
  fst sum
  fsqrt
  fstp mag
 }}return {sum,mag};
}
__declspec(noinline) inline uint8 TrainFrequency(float sum){float value;const float seventy=70,thirty=30;__asm {
 fld sum
 fsqrt
 fsubr seventy
 fadd thirty
 fstp value
 }return uint8(int32(value));}
inline void Update(){
 static bool done{};if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"pad-distance-train-nop-hex-v2-runtime.start"))return;done=true;
 std::ofstream log(dir/"pad-distance-train-nop-hex-v2-runtime.jsonl");auto write=[&](const json& r){log<<r.dump()<<'\n';if(r.value("event","")!="comparison")log.flush();};
 const auto flags=SmallPendingSupport::SnapshotHooks();std::map<std::string,Simple*> targets;std::vector<Range> entries;
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& holder:cat.Items()){auto* base=holder.get();void* gta{};void* ours{};using namespace ReversibleHooks::ReversibleHook;
  if(base->Type()==Base::HookType::Simple){auto* h=static_cast<Simple*>(base);gta=h->GetHookGTAAddress();ours=h->GetHookOurAddress();targets[std::string(cat.Name())+"::"+h->Name()]=h;}
  else if(base->Type()==Base::HookType::Virtual){auto* h=static_cast<Virtual*>(base);gta=h->GetHookGTAAddress();ours=h->GetHookOurAddress();}if(gta)entries.push_back({gta,ReversingProbe::Bytes(gta,52)});if(ours)entries.push_back({ours,ReversingProbe::Bytes(ours,52)});}});
 for(size_t n=0;n<3;++n)if(!targets.contains(keys[n])||uintptr_t(targets.at(keys[n])->GetHookGTAAddress())!=addresses[n]){write({{"event","error"},{"reason","missing/wrong target"},{"function",keys[n]}});return;}
 if(uintptr_t(&TheCamera)!=0xB6F028||uintptr_t(&FrontEndMenuManager.m_PrefsUseVibration)!=0xBA6768||uintptr_t(&CCutsceneMgr::ms_running)!=0xB5F851){write({{"event","error"},{"reason","global alias layout"}});return;}
 std::vector<Range> globals{{&TheCamera,ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera))},{reinterpret_cast<void*>(0xBA6758),ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA6758),33)},{reinterpret_cast<void*>(0xB5F841),ReversingProbe::Bytes(reinterpret_cast<void*>(0xB5F841),33)},{&CPad::Pads,ReversingProbe::Bytes(&CPad::Pads,sizeof(CPad::Pads))}};
 const auto x87Before=ReadX87Environment();const auto mxcsrBefore=_mm_getcsr();uint16 statusBefore{};__asm fnstsw statusBefore
 unsigned controlBefore{};_controlfp_s(&controlBefore,0,0);
 const uintptr_t findDll=std::bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle));const std::array<uintptr_t,2> patchAddresses{0x56E0D0,findDll};std::vector<Range> patchBefore;for(auto at:patchAddresses)patchBefore.push_back({reinterpret_cast<void*>(at),ReversingProbe::Bytes(reinterpret_cast<void*>(at),52)});
 for(const auto& state:flags)if(state.hook->Name()!="NOTSA_WinMain"){state.hook->LockState(false);state.hook->State(false);}
 bool passed=true;size_t pairs=0,differences=0;
 {
  ReversingProbe::EntryPatch nativeFind(0x56E0D0,FindVehicle),dllFind(findDll,FindVehicle);
  GuardedMatrix matrix;VehicleImage vehicle;
  const bool layout=uintptr_t(&matrix.value.GetPosition())-uintptr_t(&matrix.value)==0x30&&uintptr_t(&vehicle.Get()->m_nVehicleType)-uintptr_t(vehicle.Get())==0x590&&uintptr_t(&TheCamera.m_matrix)==0xB6F03C&&uintptr_t(&TheCamera.m_placement.m_vPosn)==0xB6F02C;
  if(!layout){write({{"event","error"},{"reason","private/camera layout"}});passed=false;}
  for(int bank=0;layout&&bank<3;++bank)for(size_t seed=0;seed<counts[bank];++seed){const auto f=Case(bank,seed);const auto padSeed=PadSeed(bank,seed,f.oldDuration);const auto initialPad=ReversingProbe::Bytes(&padSeed,sizeof(padSeed));auto modelPad=initialPad;json modelTrace=json::array();float sum=0,mag=0;uint8 expectedFrequency=initialPad[0x110];int16 expectedDuration=f.oldDuration;
   TestFP();if(bank!=0){const auto measured=Measure(f.camera,f.point,bank==2);sum=measured.first;mag=measured.second;}
   const bool enabled=f.gates&1,running=f.gates&2;
   if(bank==1&&enabled&&!running&&mag<70.f){if(!f.requestFrequency){expectedDuration=0;expectedFrequency=0;}else if(f.requestDuration>f.oldDuration){expectedDuration=f.requestDuration;expectedFrequency=f.requestFrequency;}}
   if(bank==2&&enabled&&!running){const auto call=json{{"event","find_player_vehicle"},{"player",-1},{"remote",false},{"role",f.vehicle},{"arguments_valid",true}};modelTrace.push_back(call);if(f.vehicle)modelTrace.push_back(call);if(f.vehicle!=2&&sum<4900.f&&f.oldDuration<100){expectedDuration=100;expectedFrequency=TrainFrequency(sum);}}
   std::memcpy(modelPad.data()+0x10C,&expectedDuration,2);modelPad[0x110]=expectedFrequency;
   json results[2],initialCamera,initialMatrix,initialVehicle,initialGlobals;bool guards=true;
   for(int route=0;route<2;++route){for(const auto& r:globals)std::memcpy(r.ptr,r.bytes.data(),r.bytes.size());TheCamera.m_placement.m_vPosn=f.camera;matrix.value.GetPosition()=f.camera;TheCamera.m_matrix=f.mode?&matrix.value:nullptr;FrontEndMenuManager.m_PrefsUseVibration=enabled;CCutsceneMgr::ms_running=running?1:0;vehicle.Fill(uint8(seed*37),f.vehicle==2?6:0);returnedVehicle=f.vehicle?vehicle.Get():nullptr;vehicleRole=int(f.vehicle);trace=json::array();collaboratorValid=true;
    const auto camBefore=HexBytes(&TheCamera,sizeof(TheCamera)),matBefore=HexBytes(&matrix.value,sizeof(matrix.value)),vehBefore=HexBytes(vehicle.bytes.data(),vehicle.bytes.size());json globalsBefore=json::array();for(const auto& r:globals)globalsBefore.push_back(HexBytes(r.ptr,r.bytes.size()));if(route==0){initialCamera=camBefore;initialMatrix=matBefore;initialVehicle=vehBefore;initialGlobals=globalsBefore;}
    GuardedPad pad(padSeed);TestFP();auto* h=targets.at(keys[bank]);h->State(route!=0);const bool gate=SmallPendingSupport::ExactSwitch(*h,route!=0);const bool helperProof=*reinterpret_cast<const uint8*>(0x56E0D0)==0xE9&&*reinterpret_cast<const uint8*>(findDll)==0xE9&&uint32(0x56E0D0+5+*reinterpret_cast<const int32*>(0x56E0D1))==uint32(std::bit_cast<uintptr_t>(&FindVehicle))&&uint32(findDll+5+*reinterpret_cast<const int32*>(findDll+1))==uint32(std::bit_cast<uintptr_t>(&FindVehicle));
    if(bank==0)reinterpret_cast<void(__thiscall*)(CPad*)>(addresses[bank])(&pad.value);else if(bank==1)reinterpret_cast<void(__thiscall*)(CPad*,int16,uint8,CVector)>(addresses[bank])(&pad.value,f.requestDuration,f.requestFrequency,f.point);else reinterpret_cast<void(__thiscall*)(CPad*,float,float)>(addresses[bank])(&pad.value,f.point.x,f.point.y);
    const auto fp=FP(); // Capture before any transport allocation.
    json globalsAfter=json::array();for(const auto& r:globals)globalsAfter.push_back(HexBytes(r.ptr,r.bytes.size()));const bool protectedGuards=pad.Guards()&&matrix.Guards()&&vehicle.Guards();guards&=protectedGuards;
    results[route]={{"gate",gate},{"helper_patch_exact",helperProof},{"collaborator_valid",collaboratorValid},{"pad",HexBytes(&pad.value,sizeof(pad.value))},{"camera",HexBytes(&TheCamera,sizeof(TheCamera))},{"matrix",HexBytes(&matrix.value,sizeof(matrix.value))},{"vehicle",HexBytes(vehicle.bytes.data(),vehicle.bytes.size())},{"globals",globalsAfter},{"trace",trace},{"guards",protectedGuards},{"fp",fp}};h->State(false);
   }
   const bool equal=results[0]==results[1]&&results[0]["gate"]==true&&results[0]["helper_patch_exact"]==true&&results[0]["collaborator_valid"]==true&&guards&&results[0]["pad"]==HexBytes(modelPad.data(),modelPad.size())&&results[0]["camera"]==initialCamera&&results[0]["matrix"]==initialMatrix&&results[0]["vehicle"]==initialVehicle&&results[0]["globals"]==initialGlobals&&results[0]["trace"]==modelTrace;passed&=equal;differences+=!equal;
   write({{"event","comparison"},{"function",keys[bank]},{"pair",pairs++},{"case",seed},{"bank",bank},{"equal",equal},{"geometry",f.geometry},{"mode",f.mode},{"gates",f.gates},{"vehicle_role",f.vehicle},{"duration_role",f.durationRole},{"old_duration",f.oldDuration},{"request_duration",f.requestDuration},{"request_frequency",f.requestFrequency},{"camera_bits",{std::bit_cast<uint32>(f.camera.x),std::bit_cast<uint32>(f.camera.y),std::bit_cast<uint32>(f.camera.z)}},{"point_bits",{std::bit_cast<uint32>(f.point.x),std::bit_cast<uint32>(f.point.y),std::bit_cast<uint32>(f.point.z)}},{"matrix_address",uintptr_t(&matrix.value)},{"initial_pad",HexBytes(initialPad.data(),initialPad.size())},{"initial_camera",initialCamera},{"initial_matrix",initialMatrix},{"initial_vehicle",initialVehicle},{"initial_globals",initialGlobals},{"model_pad",HexBytes(modelPad.data(),modelPad.size())},{"model_trace",modelTrace},{"model_sum_bits",std::bit_cast<uint32>(sum)},{"model_magnitude_bits",std::bit_cast<uint32>(mag)},{"original",results[0]},{"candidate",results[1]}});if(seed%64==63){log.flush();ReversingProbe::PumpMessages();}
  }
  for(const auto& r:globals)std::memcpy(r.ptr,r.bytes.data(),r.bytes.size());
 }
 for(const auto& r:globals)std::memcpy(r.ptr,r.bytes.data(),r.bytes.size());for(const auto& state:flags){state.hook->LockState(false);state.hook->State(state.enabled);state.hook->LockState(state.locked);}
 const auto rangesRestored=[](const auto& ranges){return std::ranges::all_of(ranges,[](const auto& r){return r.bytes==ReversingProbe::Bytes(r.ptr,r.bytes.size());});};const bool entriesRestored=rangesRestored(entries),patchesRestored=rangesRestored(patchBefore),globalsRestored=rangesRestored(globals),flagsRestored=SmallPendingSupport::HooksUnchanged(flags);
 RestoreX87Environment(x87Before);_mm_setcsr(mxcsrBefore);const auto x87After=ReadX87Environment();const bool x87Restored=x87Before==x87After;uint16 statusAfter{};__asm fnstsw statusAfter
 unsigned controlAfter{};_controlfp_s(&controlAfter,0,0);const bool fpRestored=x87Restored&&statusBefore==statusAfter&&controlBefore==controlAfter&&_mm_getcsr()==mxcsrBefore;const bool restored=entriesRestored&&patchesRestored&&globalsRestored&&flagsRestored&&fpRestored;
 write({{"event",passed&&restored&&pairs==13952?"complete":"error"},{"pairs",pairs},{"differences",differences},{"state_restored",restored},{"entries_restored",entriesRestored},{"collaborator_entries_restored",patchesRestored},{"globals_restored",globalsRestored},{"flags_restored",flagsRestored},{"fp_restored",fpRestored},{"x87_environment_restored",x87Restored},{"x87_environment_before",HexBytes(x87Before.data(),x87Before.size())},{"x87_environment_after",HexBytes(x87After.data(),x87After.size())},{"fp_status_before",statusBefore},{"fp_status_after",statusAfter},{"fp_control_before",controlBefore},{"fp_control_after",controlAfter},{"mxcsr_before",mxcsrBefore},{"mxcsr_after",_mm_getcsr()},{"scope","PC24 round-nearest finite CPU fixtures; FindPlayerVehicle isolated exact multiplicity; real camera borrowed matrix restored; no resources/gameplay/helper credit"}});
 returnedVehicle=nullptr;trace=nullptr;
}
}
