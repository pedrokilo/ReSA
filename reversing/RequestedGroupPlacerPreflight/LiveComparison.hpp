#pragma once
#include <fstream>
#include <new>
#include "PedGroupPlacer.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedGroupPlacerPreflightLive {
using namespace ReversibleHooks::ReversibleHook;
inline bool done{},candidateEntered{},terminalWritten{};
inline std::ofstream log;
inline Simple* target{};inline bool enabled{},locked{};
inline std::vector<SmallPendingSupport::HookSnapshot> flags;
inline std::vector<uint8> gtaBefore,dllBefore;
inline ULONG_PTR* messageBoxSlot{};inline ULONG_PTR messageBoxOriginal{};
inline bool iatPatched{};
inline void Write(const json&r){log<<r.dump()<<'\n';log.flush();}
inline void SetIat(ULONG_PTR value){DWORD old{},ignored{};if(!VirtualProtect(messageBoxSlot,sizeof(value),PAGE_READWRITE,&old))std::abort();*messageBoxSlot=value;VirtualProtect(messageBoxSlot,sizeof(value),old,&ignored);}
inline bool Restore(){
 target->LockState(false);target->State(enabled);target->LockState(locked);
 if(iatPatched){SetIat(messageBoxOriginal);iatPatched=false;}
 return SmallPendingSupport::HooksUnchanged(flags)&&gtaBefore==ReversingProbe::Bytes(target->GetHookGTAAddress(),52)&&dllBefore==ReversingProbe::Bytes(target->GetHookOurAddress(),52)&&*messageBoxSlot==messageBoxOriginal;
}
inline int WINAPI CaptureMessageBoxA(HWND,const char*text,const char*caption,UINT type){
 // A candidate fatal path is evidence, never a successful comparator return.
 const bool expected=candidateEntered&&caption&&std::strstr(caption,"Unreachable code reached")&&text&&std::strstr(text,"PlaceGroup");
 const bool restored=Restore();terminalWritten=true;
 Write({{"event","error"},{"function","CPedGroupPlacer::PlaceGroup"},{"allocator",4},{"candidate_returned",false},{"unreachable_guard_observed",expected},{"caption",caption?caption:""},{"details",text?text:""},{"messagebox_flags",type},{"state_restored",restored},{"iat_restored",*messageBoxSlot==messageBoxOriginal},{"reason","baseline candidate unreachable path captured before modal; fatal specimen, no accreditation"}});
 log.close();ExitProcess(3);
}
inline ULONG_PTR* FindOwnMessageBoxA(HMODULE module){
 auto*base=reinterpret_cast<uint8*>(module);auto*dos=reinterpret_cast<IMAGE_DOS_HEADER*>(base);if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return nullptr;
 auto*nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+dos->e_lfanew);if(nt->Signature!=IMAGE_NT_SIGNATURE)return nullptr;
 const auto&dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT];if(!dir.VirtualAddress)return nullptr;
 auto*desc=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);
 for(;desc->Name;++desc){if(!desc->OriginalFirstThunk)continue;auto*name=reinterpret_cast<IMAGE_THUNK_DATA*>(base+desc->OriginalFirstThunk);auto*slot=reinterpret_cast<IMAGE_THUNK_DATA*>(base+desc->FirstThunk);
  for(;name->u1.AddressOfData;++name,++slot){if(IMAGE_SNAP_BY_ORDINAL(name->u1.Ordinal))continue;auto*item=reinterpret_cast<IMAGE_IMPORT_BY_NAME*>(base+name->u1.AddressOfData);if(!std::strcmp(reinterpret_cast<char*>(item->Name),"MessageBoxA"))return &slot->u1.Function;}
 }
 return nullptr;
}
inline void Update(){
 if(done)return;done=true;char executable[MAX_PATH]{};GetModuleFileNameA(nullptr,executable,MAX_PATH);const auto dir=std::filesystem::path(executable).parent_path();if(!std::filesystem::exists(dir/"requested-group-placer-preflight-runtime.start"))return;
 log.open(dir/"requested-group-placer-preflight-runtime.jsonl");flags=SmallPendingSupport::SnapshotHooks();
 for(const auto&f:flags)if(f.hook->Type()==Base::HookType::Simple){auto*h=static_cast<Simple*>(f.hook);if(h->GetHookGTAAddress()==reinterpret_cast<void*>(0x5FD810)&&h->Name()=="PlaceGroup")target=h;}
 if(!target){Write({{"event","error"},{"reason","exact PlaceGroup identity missing"}});return;}
 HMODULE module{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(target->GetHookOurAddress()),&module);messageBoxSlot=FindOwnMessageBoxA(module);
 // Abort setup before target call if capture cannot guarantee no modal UI.
 if(!messageBoxSlot||!*messageBoxSlot){Write({{"event","error"},{"reason","own ASI MessageBoxA IAT missing; candidate not called"},{"candidate_returned",false},{"state_restored",true}});return;}
 messageBoxOriginal=*messageBoxSlot;enabled=target->Hooked();locked=target->Locked();gtaBefore=ReversingProbe::Bytes(target->GetHookGTAAddress(),52);dllBefore=ReversingProbe::Bytes(target->GetHookOurAddress(),52);
 Write({{"event","fixture"},{"iat_slot",reinterpret_cast<uintptr_t>(messageBoxSlot)},{"iat_original",messageBoxOriginal},{"scope","valid enum4 dispatcher only; no placement helpers, actor or pool integration"}});
 alignas(256)static std::array<uint8,512>origins{};CPedGroupPlacer placer{};
 using Fn=uint32(__thiscall*)(CPedGroupPlacer*,ePedType,uint32,const CVector&,ePedGroupDefaultTaskAllocatorType);
 target->LockState(false);target->State(false);
 for(int offset:{0,4,128,252}){
  origins.fill(0xCD);auto*origin=::new(origins.data()+offset)CVector{1.f,2.f,3.f};const auto originBefore=origins;const auto placerBefore=ReversingProbe::Bytes(&placer,sizeof(placer));const bool gate=SmallPendingSupport::ExactSwitch(*target,false);
  const uint32 eax=reinterpret_cast<Fn>(0x5FD810)(&placer,PED_TYPE_CIVMALE,3,*origin,ePedGroupDefaultTaskAllocatorType::SIT_IN_LEADER_CAR);
  const uint32 expected=uint32(reinterpret_cast<uintptr_t>(origin)&255);const bool correct=gate&&eax==expected&&originBefore==origins&&placerBefore==ReversingProbe::Bytes(&placer,sizeof(placer));
  Write({{"event","characterization"},{"allocator",4},{"origin_low_byte",expected},{"raw_eax",eax},{"raw_al",eax&255},{"expected_raw_eax",expected},{"exact_abi_and_state",correct}});
  if(!correct){const bool restored=Restore();Write({{"event","error"},{"reason","original characterization failed; candidate not called"},{"state_restored",restored}});return;}
 }
 origins.fill(0xCD);auto*origin=::new(origins.data()+128)CVector{1.f,2.f,3.f};target->State(true);const bool gate=SmallPendingSupport::ExactSwitch(*target,true);
 if(!gate){const bool restored=Restore();Write({{"event","error"},{"reason","candidate mode gate failed"},{"state_restored",restored}});return;}
 SetIat(reinterpret_cast<ULONG_PTR>(&CaptureMessageBoxA));iatPatched=true;candidateEntered=true;
 Write({{"event","before_candidate"},{"allocator",4},{"expected_raw_al",128},{"candidate_gate",gate},{"guard_installed",*messageBoxSlot==reinterpret_cast<ULONG_PTR>(&CaptureMessageBoxA)}});
 const uint32 returned=reinterpret_cast<Fn>(0x5FD810)(&placer,PED_TYPE_CIVMALE,3,*origin,ePedGroupDefaultTaskAllocatorType::SIT_IN_LEADER_CAR);
 const bool restored=Restore();Write({{"event","error"},{"reason","baseline candidate returned; inspect possible source change before corrected bank"},{"raw_eax",returned},{"raw_al",returned&255},{"candidate_returned",true},{"state_restored",restored}});
}
}
