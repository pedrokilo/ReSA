#pragma once
#include <fstream>
#include "Entity/Vehicle/Vehicle.h"
#include "CutsceneMgr.h"
#include "Pools/VehiclePool.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperRoutes.hpp"

namespace DependenciesVehicle12Live {
inline bool done{};
inline std::ofstream log;
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
struct Fn { const char* cls;const char* name;uintptr_t address;int reserved,unused,count; };
inline constexpr Fn fns[]{
    {"CVehicle","operator new-anon",0x6E2D50,0,-1,2048}
};

struct Result {uint32 eax,ecx,edx,arg;bool stack,nonvolatile;};
inline Result Gate(uintptr_t entry,bool helper,uintptr_t pool,uint32 allocationSize){
    uint32 before{},after{},a{},c{},d{},arg{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
    __asm {
        mov before,esp
        mov b0,ebx
        mov s0,esi
        mov i0,edi
        mov p0,ebp
        cmp helper,0
        jne noSize
        push allocationSize
    noSize:
        mov eax,0A5123456h
        mov edx,0C0DE5921h
        mov ecx,pool
        call entry
        mov a,eax
        mov c,ecx
        mov d,edx
        mov b1,ebx
        mov s1,esi
        mov i1,edi
        mov p1,ebp
        cmp helper,0
        jne finished
        mov eax,[esp]
        mov arg,eax
        add esp,4
    finished:
        mov after,esp
    }
    return {a,c,d,arg,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};
}
inline uint32 ReadInt(uintptr_t a){uint32 v;memcpy(&v,reinterpret_cast<void*>(a),4);return v;}
inline void PutInt(uintptr_t a,uint32 v){memcpy(reinterpret_cast<void*>(a),&v,4);}
inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string s;s.reserve(n*2);for(size_t i=0;i<n;++i){s+=digits[b[i]>>4];s+=digits[b[i]&15];}return s;}
struct Block {
    uintptr_t address;std::vector<uint8> saved;
    Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}
    void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}
    bool Restored()const{return !memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size());}
    std::string Observe()const{return Hex(address,saved.size());}
};
inline bool AllocatorModeOK(bool on,uintptr_t body){
    ReversibleHooks::ReversibleHook::Simple* allocator{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& cat){for(auto& h:cat.Items())if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple){auto* hook=static_cast<ReversibleHooks::ReversibleHook::Simple*>(h.get());if(uintptr_t(hook->GetHookGTAAddress())==0x6E2A50)allocator=hook;}});
    if(!allocator||!allocator->Reversed()||allocator->Hooked()!=on)return false;
    const auto library=uintptr_t(allocator->GetHookOurAddress());
    const auto* gta=on?reinterpret_cast<const uint8*>(&allocator->m_HookContent):allocator->m_OriginalFunctionContent;
    const auto* lib=on?allocator->m_LibOriginalFunctionContent:reinterpret_cast<const uint8*>(&allocator->m_LibHookContent);
    bool ok=!memcmp(reinterpret_cast<void*>(0x6E2A50),gta,allocator->m_iHookedBytes)&&!memcmp(reinterpret_cast<void*>(library),lib,allocator->m_iLibHookedBytes);
    if(on)ok&=Resolve(0x6E2A50)==body&&ModuleOf(body)==ModuleOf(uintptr_t(&ModuleMarker))&&Resolve(library)==body;
    else ok&=ModuleOf(Resolve(0x6E2A50))==uintptr_t(GetModuleHandleA(nullptr))&&Resolve(library)==Resolve(0x6E2A50);
    return ok;
}
inline void Update(){
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesvehicle12-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesvehicle12-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesvehicle12");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;
    static_assert(sizeof(CVehiclePool)==0x14&&sizeof(CHeli)==0xA18);
    auto isolated=std::make_unique<CVehiclePool>(4,"DependenciesVehicle12 isolated");
    const uintptr_t pool=uintptr_t(isolated.get()),storage=ReadInt(pool),flags=ReadInt(pool+4),gamePool=ReadInt(0xB74494);
    if(!gamePool){Write({{"event","error"},{"reason","loaded_game_vehicle_pool_missing"}});return;}
    bool layoutOK=ReadInt(pool+8)==4&&*reinterpret_cast<uint8*>(pool+16)==1;
    const auto gamePoolBefore=Hex(gamePool,0x14);const uintptr_t gameStorage=ReadInt(gamePool),gameFlags=ReadInt(gamePool+4);const uint32 gameCapacity=ReadInt(gamePool+8);
    if(!gameStorage||!gameFlags||!gameCapacity||gameCapacity>10000){Write({{"event","error"},{"reason","loaded_game_vehicle_pool_layout_invalid"}});return;}
    const auto gameStorageBefore=Hex(gameStorage,size_t(gameCapacity)*0xA18),gameFlagsBefore=Hex(gameFlags,gameCapacity);
    auto gameUntouched=[&]{return Hex(gamePool,0x14)==gamePoolBefore&&Hex(gameStorage,size_t(gameCapacity)*0xA18)==gameStorageBefore&&Hex(gameFlags,gameCapacity)==gameFlagsBefore;};
    const auto raw=uintptr_t(static_cast<void*(*)(unsigned)>(&CVehicle::operator new));const auto member=&CVehiclePool::New;const auto helperRaw=uintptr_t(FunctionToVoidPtr(member));
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initialCode;
    for(auto at:{uintptr_t(0x6E2D50),raw,Resolve(raw),uintptr_t(0x6E2A50),helperRaw,Resolve(helperRaw)})initialCode.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute target("CVehicle","operator new-anon",0x6E2D50,static_cast<void*(*)(unsigned)>(&CVehicle::operator new));
    const auto targetOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x6E2D50),16);
    HelperRoute allocator(0x6E2A50);const auto allocatorMetadata=allocator.Metadata();
    bool ready=layoutOK&&target.Registered()&&ModuleOf(target.Body())==ModuleOf(uintptr_t(&ModuleMarker))&&allocator.Check();
    for(const char* field:{"registered","enabled","reconstructed"})ready&=allocatorMetadata[field]==true;
    std::vector<Block> blocks;blocks.emplace_back(0xB74494,4);blocks.emplace_back(pool,0x14);blocks.emplace_back(flags,4);blocks.emplace_back(storage,4*0xA18);
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};
    auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
    Write({{"event","begin"},{"registered_hook",ready},{"allocator_route",allocatorMetadata},{"allocator_route_ok",allocator.Check()},{"pool_layout_ok",layoutOK},{"floating_state_restored",FloatingStateRestored()},{"functions",1},{"pairs",2048},{"allocator_equivalence_cases",2048},{"real_owned_pool",true},{"game_pool_pointer",gamePool},{"game_pool_capacity",gameCapacity},{"game_pool_snapshot",json::array({gamePoolBefore,gameFlagsBefore,gameStorageBefore})},{"capacity",4},{"pool_address",pool},{"initial_pool_bytes",blocks[1].Observe()},{"storage_address",storage},{"flags_address",flags},{"slot_stride",0xA18},{"collaborators_stubbed",false}});
    const uint32 sizes[]{sizeof(CVehicle),sizeof(CHeli),0x100,0,0xFFFFFFFF};bool passed=ready;int pairs=0,helperPairs=0,failures=0;
    auto run=[&](OriginalDependencies499::HookRoute& route,uintptr_t entry,bool helper,bool candidate,int scenario){
        restore();const unsigned profile=scenario%32,value=scenario/32,mask=profile%16;const int cursor=int(value%5)-1;
        const unsigned ref=(value/5)%4;const uint8 generations[]{0,1,126,127};const uint32 size=sizes[(value/20+profile/16)%5];
        PutInt(0xB74494,uint32(pool));PutInt(pool+12,uint32(cursor));memset(reinterpret_cast<void*>(storage),uint8(0x30+value%32),4*0xA18);
        json beforeFlags=json::array();for(unsigned n=0;n<4;++n){const uint8 state=uint8(generations[(ref+n)%4]|((mask&(1u<<n))?0x80:0));*reinterpret_cast<uint8*>(flags+n)=state;beforeFlags.push_back(state);}
        const auto storageBefore=Hex(storage,4*0xA18);bool routeOK=false,allocatorOK=helper||allocator.Check();
        const auto result=route.Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(entry)==candidate;if(helper)allocatorOK&=AllocatorModeOK(candidate,route.Body());return Gate(entry,helper,pool,size);});allocatorOK&=helper?AllocatorModeOK(false,route.Body()):allocator.Check();
        const bool redirectOK=ReadInt(0xB74494)==pool;const uint32 cursorAfter=ReadInt(pool+12);
        const int index=result.eax?int((result.eax-storage)/0xA18):-1;
        const bool allocatedPointerOK=!result.eax||(result.eax>=storage&&result.eax<storage+4*0xA18&&(result.eax-storage)%0xA18==0);
        json after=json::array();for(const auto& b:blocks)after.push_back(b.Observe());
        bool releaseOK=true;if(result.eax&&allocatedPointerOK){isolated->Delete(reinterpret_cast<CVehicle*>(result.eax));releaseOK=isolated->IsFreeSlotAtIndex(size_t(index));}
        json cleanup=json::array();for(const auto& b:blocks)cleanup.push_back(b.Observe());
        json input={{"profile",profile},{"value",value},{"empty_mask",mask},{"cursor",cursor},{"size",size},{"flags",beforeFlags},{"storage_pattern",0x30+value%32}};
        json out={{"input",input},{"eax",result.eax},{"returned_index",index},{"cursor_after",cursorAfter},{"state",after},{"cleanup_state",cleanup},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"allocator_route_ok",allocatorOK},{"pool_redirect_ok",redirectOK},{"allocated_pointer_ok",allocatedPointerOK},{"storage_unchanged",Hex(storage,4*0xA18)==storageBefore},{"release_balanced",releaseOK}};
        if(!helper){out["ecx"]=result.ecx;out["edx"]=result.edx;out["argument_after"]=result.arg;out["argument_unchanged"]=result.arg==size;}
        restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
    };
    auto gates=[&](const json& a,const json& b,bool helper){bool ok=a==b;for(const char* field:{"stack_restored","nonvolatile_preserved","route_ok","allocator_route_ok","pool_redirect_ok","allocated_pointer_ok","storage_unchanged","release_balanced","globals_restored","floating_state_restored"})ok&=a[field]==true&&b[field]==true;if(!helper)ok&=a["argument_unchanged"]==true&&b["argument_unchanged"]==true;return ok;};
    if(ready){
        OriginalDependencies499::HookRoute helper("CVehiclePool","New",0x6E2A50,FunctionToVoidPtr(member));
        for(int scenario=0;scenario<2048;++scenario){const auto a=run(helper,0x6E2A50,true,false,scenario),b=run(helper,0x6E2A50,true,true,scenario);const bool ok=gates(a,b,true);passed&=ok;
            Write({{"event","allocator_equivalence"},{"case",scenario},{"original",a},{"candidate",b},{"equal",ok},{"accepted",ok}});++helperPairs;}
        ready=helper.Restore()&&allocator.Check();passed&=ready;
        Write({{"event","allocator_summary"},{"cases",helperPairs},{"state_restored",ready},{"floating_state_restored",FloatingStateRestored()}});
        for(int scenario=0;scenario<2048&&ready;++scenario){const auto a=run(target,0x6E2D50,false,false,scenario),b=run(target,0x6E2D50,false,true,scenario);const bool ok=gates(a,b,false);passed&=ok;failures+=!ok;
            Write({{"event","comparison"},{"function","CVehicle::operator new-anon"},{"pair",pairs++},{"case",scenario},{"original",a},{"candidate",b},{"equal",ok},{"accepted",ok}});}
    }
    Write({{"event","function_summary"},{"function","CVehicle::operator new-anon"},{"cases",pairs},{"failures",failures}});
    restore();bool stateRestored=restored()&&allocator.Check()&&targetOff==ReversingProbe::Bytes(reinterpret_cast<void*>(0x6E2D50),16);stateRestored&=target.Restore();
    for(const auto& [at,bytes]:initialCode)stateRestored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    const bool actualPoolUntouched=gameUntouched();stateRestored&=actualPoolUntouched;
    isolated.reset(); // Real storage/state allocations are freed by the owning CPool destructor.
    Write({{"event","game_pool_final"},{"bytes",json::array({Hex(gamePool,0x14),Hex(gameFlags,gameCapacity),Hex(gameStorage,size_t(gameCapacity)*0xA18)})}});
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"allocator_equivalence_cases",helperPairs},{"state_restored",stateRestored},{"game_pool_untouched",actualPoolUntouched},{"owned_pool_freed",isolated==nullptr},{"floating_state_restored",FloatingStateRestored()},{"allocator_route_ok",allocator.Check()}});
    RestoreFloatingState();savedFPActive=false;
}
}
