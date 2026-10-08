#pragma once
#include "StdInc.h"
#include <fstream>
#include <cfenv>
#include <xmmintrin.h>
#include "GenericGameStorage.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedStorageWorkBufferLive {
inline std::ofstream log;
inline void Write(const json& value){log<<value.dump()<<'\n';log.flush();}
inline std::string Hex(const void* ptr,size_t n){const auto*b=static_cast<const uint8*>(ptr);constexpr char digits[]="0123456789abcdef";std::string s(n*2,'0');for(size_t i=0;i<n;++i){s[2*i]=digits[b[i]>>4];s[2*i+1]=digits[b[i]&15];}return s;}
inline uint8 Call(uintptr_t target,void* data,int32 size){uint32 out{};
    __asm {
        push size
        push data
        call target
        add esp,8
        mov out,eax
    }
    return uint8(out);
}
inline void Update(){
    using namespace ReversibleHooks::ReversibleHook;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);log.open(std::filesystem::path(exe).parent_path()/"requested-storage-work-buffer-runtime.jsonl");
    const auto flags=SmallPendingSupport::SnapshotHooks();const uintptr_t targets[]{0x5D1270,0x5D1300};Simple* hooks[2]{};
    for(const auto&f:flags)if(f.hook->Type()==Base::HookType::Simple){auto*h=static_cast<Simple*>(f.hook);for(int k=0;k<2;++k)if(h->GetHookGTAAddress()==reinterpret_cast<void*>(targets[k]))hooks[k]=h;}
    if(!hooks[0]||!hooks[1]){Write({{"event","error"},{"reason","missing exact work-buffer hook"}});return;}
    std::array<std::vector<uint8>,2> nativeBefore,dllBefore;for(int k=0;k<2;++k){nativeBefore[k]=ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52);dllBefore[k]=ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);hooks[k]->LockState(false);}
    const auto globalsBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC16EE8),24);const auto sizeBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D2BD0),32);
    std::fenv_t fp{};std::fegetenv(&fp);const auto csr=_mm_getcsr();
    std::array<uint8,51232> work{};std::array<uint8,288> caller{};
    auto& pointer=*reinterpret_cast<uint8**>(0xC16EE8);auto& position=*reinterpret_cast<int32*>(0xC16EEC);auto& failed=*reinterpret_cast<bool*>(0xC16EFC);auto& capacity=*reinterpret_cast<uint32*>(0x8D2BE0);
    const int32 sizes[]{-1,0,1,2,3,4,7,16},positions[]{0,1,2,3,16,51168,51176,51184};
    Write({{"event","start"},{"pairs",2048},{"work_address",uintptr_t(work.data()+16)},{"caller_address",uintptr_t(caller.data()+16)},{"globals",Hex(globalsBefore.data(),globalsBefore.size())},{"size_globals",Hex(sizeBefore.data(),sizeBefore.size())},{"fp",{{"rounding",std::fegetround()},{"exceptions",std::fetestexcept(FE_ALL_EXCEPT)},{"mxcsr",csr}}}});
    int pair{},failures{};
    for(int fn=0;fn<2;++fn)for(int seed=0;seed<1024;++seed){const int pattern=seed%8,sizeIndex=(seed/8)%8,posIndex=(seed/64)%8;const bool priorFailed=(seed/512)!=0;const int32 size=sizes[sizeIndex],pos=positions[posIndex];json legs[2];
        for(int side=0;side<2;++side){
            std::memcpy(reinterpret_cast<void*>(0xC16EE8),globalsBefore.data(),24);std::memcpy(reinterpret_cast<void*>(0x8D2BD0),sizeBefore.data(),32);pointer=work.data()+16;position=pos;failed=priorFailed;capacity=51200;
            work.fill(0xA5);caller.fill(0xD3);for(size_t i=0;i<51200;++i)work[16+i]=uint8(pattern*37+i*13);for(size_t i=0;i<256;++i)caller[16+i]=uint8(pattern*53+i*29);
            std::fesetenv(&fp);_mm_setcsr(csr);hooks[fn]->State(side!=0);const bool exact=SmallPendingSupport::ExactSwitch(*hooks[fn],side!=0);
            const uint8 returned=Call(targets[fn],caller.data()+16,size);const int rounding=std::fegetround(),exceptions=std::fetestexcept(FE_ALL_EXCEPT);const auto mxcsr=_mm_getcsr();
            legs[side]={{"returned_al",returned},{"switch_exact",exact},{"work",Hex(work.data(),work.size())},{"caller",Hex(caller.data(),caller.size())},{"globals",Hex(reinterpret_cast<void*>(0xC16EE8),24)},{"size_globals",Hex(reinterpret_cast<void*>(0x8D2BD0),32)},{"fp",{{"rounding",rounding},{"exceptions",exceptions},{"mxcsr",mxcsr}}}};
        }
        const bool equal=legs[0]==legs[1],accepted=equal&&legs[0]["returned_al"]==int(!priorFailed)&&legs[0]["switch_exact"]==true;failures+=!accepted;
        Write({{"event","comparison"},{"fn",fn},{"pair",pair++},{"case",seed},{"pattern",pattern},{"size",size},{"position",pos},{"prior_failed",priorFailed},{"equal",equal},{"accepted",accepted},{"original",legs[0]},{"candidate",legs[1]}});
    }
    std::memcpy(reinterpret_cast<void*>(0xC16EE8),globalsBefore.data(),24);std::memcpy(reinterpret_cast<void*>(0x8D2BD0),sizeBefore.data(),32);std::fesetenv(&fp);_mm_setcsr(csr);
    for(int k=0;k<2;++k){const auto it=std::find_if(flags.begin(),flags.end(),[&](const auto&f){return f.hook==hooks[k];});hooks[k]->LockState(false);hooks[k]->State(it->enabled);hooks[k]->LockState(it->locked);}
    bool restored=SmallPendingSupport::HooksUnchanged(flags)&&globalsBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xC16EE8),24)&&sizeBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D2BD0),32);
    for(int k=0;k<2;++k)restored&=nativeBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52)&&dllBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);
    Write({{"event",failures==0&&restored?"complete":"error"},{"pairs",pair},{"failures",failures},{"passed",failures==0&&restored},{"state_restored",restored}});
}
}
