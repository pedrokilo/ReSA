#pragma once
#include <fstream>
#include <cfenv>
#include <xmmintrin.h>
#include "Pools/PedPool.h"
#include "Pools/VehiclePool.h"
#include "Pools/ObjectPool.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedPoolsSixGettersLive {
inline std::ofstream log;
inline bool passed=true;inline int failures{},pair{};
inline void Write(const json& r){log<<r.dump()<<'\n';log.flush();}
inline std::string Hex(const void* p,size_t n){static constexpr char alphabet[]="0123456789abcdef";std::string out(n*2,'0');const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){out[2*i]=alphabet[b[i]>>4];out[2*i+1]=alphabet[b[i]&15];}return out;}
template<class Pool,class Storage>
inline void Run(uintptr_t slot,uintptr_t refTarget,uintptr_t getTarget,const char* refName,const char* getName,int family,const std::vector<SmallPendingSupport::HookSnapshot>& flags){
    using namespace ReversibleHooks::ReversibleHook;
    static_assert(sizeof(Pool)==20);
    constexpr size_t stride=sizeof(Storage),capacity=2;
    alignas(Storage)std::array<uint8,capacity*stride+32> storage{};storage.fill(0xA5);
    std::array<uint8,capacity+32> states{};states.fill(0xD3);
    alignas(Pool)std::array<uint8,sizeof(Pool)+32> header{};header.fill(0xB6);
    auto* pool=::new(header.data()+16)Pool(capacity,storage.data()+16,states.data()+16);
    // This constructor owns no allocations and creates no actor objects.
    const auto storageInert=storage;const auto statesInert=states;const auto headerInert=header;
    const auto globals=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB74484),68);
    auto& installed=*reinterpret_cast<Pool**>(slot);const auto saved=installed;installed=pool;
    Simple* hooks[2]{};const uintptr_t targets[]{refTarget,getTarget};
    for(const auto& f:flags)if(f.hook->Type()==Base::HookType::Simple){auto*h=static_cast<Simple*>(f.hook);for(int k=0;k<2;++k)if(h->GetHookGTAAddress()==reinterpret_cast<void*>(targets[k]))hooks[k]=h;}
    if(!hooks[0]||!hooks[1]){installed=saved;passed=false;Write({{"event","error"},{"reason","pool wrapper identity missing"}});return;}
    std::array<std::vector<uint8>,2> nativeBefore,dllBefore;for(int k=0;k<2;++k){nativeBefore[k]=ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52);dllBefore[k]=ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);hooks[k]->LockState(false);}
    std::fenv_t initialFP{};std::fegetenv(&initialFP);const auto initialMXCSR=_mm_getcsr();
    Write({{"event","fixture"},{"family",family},{"stride",stride},{"capacity",capacity},{"storage_address",reinterpret_cast<uintptr_t>(storage.data()+16)},{"state_address",reinterpret_cast<uintptr_t>(states.data()+16)},{"pool_address",reinterpret_cast<uintptr_t>(pool)},{"storage",Hex(storage.data(),storage.size())},{"states",Hex(states.data(),states.size())},{"header",Hex(header.data(),header.size())},{"globals",Hex(reinterpret_cast<void*>(0xB74484),68)},{"fp",{{"rounding",std::fegetround()},{"exceptions",std::fetestexcept(FE_ALL_EXCEPT)},{"mxcsr",initialMXCSR}}}});
    for(int fn=0;fn<2;++fn){const int total=fn?768:256;
        for(int seed=0;seed<total;++seed){const int index=seed%2,id=(seed/2)%128,variant=fn?seed/256:0;const uint8 requested=variant==0?uint8(id):variant==1?uint8(id^1):uint8(id^128);const uint32 handle=(index<<8)|requested;
            const uint32 address=uint32(reinterpret_cast<uintptr_t>(storage.data()+16+index*stride));const uint32 argument=fn?handle:address;const uint32 expected=fn?(requested==id?address:0):uint32((index<<8)|id);json legs[2];
            for(int side=0;side<2;++side){storage=storageInert;states=statesInert;header=headerInert;for(size_t i=0;i<capacity*stride;++i)storage[16+i]=uint8(seed*47+i*29);states[16]=uint8((id+17)&127);states[17]=uint8((id+31)&127);states[16+index]=uint8(id);
                const auto storageBefore=storage;const auto statesBefore=states;const auto headerBefore=header;const auto globalBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB74484),68);
                std::fesetenv(&initialFP);_mm_setcsr(initialMXCSR);hooks[fn]->State(side!=0);const bool exact=SmallPendingSupport::ExactSwitch(*hooks[fn],side!=0);
                const uint32 result=reinterpret_cast<uint32(__cdecl*)(uint32)>(targets[fn])(argument);
                const int rounding=std::fegetround(),exceptions=std::fetestexcept(FE_ALL_EXCEPT);const auto mxcsr=_mm_getcsr();
                legs[side]={{"returned",result},{"switch_exact",exact},{"storage_unchanged",storage==storageBefore},{"states_unchanged",states==statesBefore},{"header_unchanged",header==headerBefore},{"globals_unchanged",globalBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB74484),68)},{"storage",Hex(storage.data(),storage.size())},{"states",Hex(states.data(),states.size())},{"header",Hex(header.data(),header.size())},{"globals",Hex(reinterpret_cast<void*>(0xB74484),68)},{"fp",{{"rounding",rounding},{"exceptions",exceptions},{"mxcsr",mxcsr}}}};
            }
            const bool accepted=legs[0]==legs[1]&&legs[0]["returned"]==expected&&legs[0]["switch_exact"]==true&&legs[0]["storage_unchanged"]==true&&legs[0]["states_unchanged"]==true&&legs[0]["header_unchanged"]==true&&legs[0]["globals_unchanged"]==true;passed&=accepted;failures+=!accepted;
            Write({{"event","comparison"},{"function",std::string("CPools::")+(fn?getName:refName)},{"family",family},{"fn",fn},{"pair",pair++},{"case",seed},{"index",index},{"id",id},{"variant",variant},{"argument",argument},{"expected",expected},{"equal",legs[0]==legs[1]},{"accepted",accepted},{"original",legs[0]},{"candidate",legs[1]}});
        }
    }
    std::fesetenv(&initialFP);_mm_setcsr(initialMXCSR);installed=saved;storage=storageInert;states=statesInert;header=headerInert;
    bool restored=globals==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB74484),68);
    for(int k=0;k<2;++k){const auto it=std::find_if(flags.begin(),flags.end(),[&](const auto&v){return v.hook==hooks[k];});hooks[k]->LockState(false);hooks[k]->State(it->enabled);hooks[k]->LockState(it->locked);restored&=nativeBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookGTAAddress(),52)&&dllBefore[k]==ReversingProbe::Bytes(hooks[k]->GetHookOurAddress(),52);}
    passed&=restored;Write({{"event","family_restore"},{"family",family},{"state_restored",restored}});
    // No real actor lifetime began. Borrowed header/storage end without destruction callbacks.
}
inline void Update(){char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);log.open(std::filesystem::path(exe).parent_path()/"requested-pools-six-getters-runtime.jsonl");const auto flags=SmallPendingSupport::SnapshotHooks();Write({{"event","start"},{"cases",3072},{"functions",6},{"domain","two in-bounds slots with active seven-bit IDs; mismatch handles may flip ID/free bit; no invalid indices"}});
    Run<CPedPool,CCopPed>(0xB74490,0x54FF60,0x54FF90,"GetPedRef","GetPed",0,flags);
    Run<CVehiclePool,CHeli>(0xB74494,0x54FFC0,0x54FFF0,"GetVehicleRef","GetVehicle",1,flags);
    Run<CObjectPool,CCutsceneObject>(0xB7449C,0x550020,0x550050,"GetObjectRef","GetObject",2,flags);
    const bool restored=SmallPendingSupport::HooksUnchanged(flags);Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"failures",failures},{"passed",passed&&restored},{"state_restored",restored}});
}
}
