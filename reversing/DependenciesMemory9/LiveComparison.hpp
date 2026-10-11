#pragma once
#include <fstream>
#include "MemoryMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "CrtProof.hpp"
#ifdef MEMORY_MGR_USE_MEMORY_HEAP
#error This bank requires the standard PC CRT allocator route
#endif

namespace DependenciesMemory9Live {
inline bool done{};
inline std::ofstream log;
struct Fn {const char* cls;const char* name;uintptr_t address;int reserved,slot,count;};
inline constexpr Fn fns[]{
    {"CMemoryMgr","Malloc-SizeHint",0x72F420,0,-1,2048},
    {"CMemoryMgr","Realloc-MemorySizeHint",0x72F440,0,-1,2048},
    {"CMemoryMgr","MallocAlign-SizeAlignHint",0x72F4C0,0,-1,2048},
    {"CMemoryMgr","FreeAlign",0x72F4F0,0,-1,2048}
};
inline uintptr_t Resolve(uintptr_t p) {for(int i=0;i<16&&*reinterpret_cast<uint8*>(p)==0xE9;++i)p+=5+*reinterpret_cast<int32*>(p+1);return p;}
inline HMODULE ModuleOf(uintptr_t p) {HMODULE m{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(p),&m);return m;}
inline void ModuleMarker() {}
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};
inline bool fpActive{};
inline void RestoreFloating() {if(fpActive){RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingRestored() {alignas(16)std::array<uint8,108> now{};SaveFP(now.data());return now==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloating();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloating();}
struct Result {uintptr_t eax,argAfter;bool stack,nonvolatile;};
inline Result Gate(uintptr_t entry,const std::array<uint32,3>& args,unsigned arity) {
    const auto* pArgs=args.data();const unsigned cleanup=arity*4;
    uint32 a{},arg{},before{},after{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
    __asm {
        mov before,esp
        mov b0,ebx
        mov s0,esi
        mov i0,edi
        mov p0,ebp
        mov eax,pArgs
        mov edx,arity
    pushArg:
        push dword ptr [eax+edx*4-4]
        dec edx
        jnz pushArg
        mov eax,0A5123456h
        mov ecx,137AFF01h
        mov edx,0C0DE5921h
        call entry
        mov a,eax
        mov b1,ebx
        mov s1,esi
        mov i1,edi
        mov p1,ebp
        mov eax,[esp]
        mov arg,eax
        add esp,cleanup
        mov after,esp
    }
    return {a,arg,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};
}
inline uint8* NativeMalloc(uint32 size) {return reinterpret_cast<uint8*(__cdecl*)(uint32)>(0x824257)(size);}
inline void NativeFree(void* p) {reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p);}
inline uint8 Pattern(unsigned i,unsigned scenario) {return uint8(i*37u+scenario*13u+0x5Au);}
inline void Fill(uint8* p,unsigned n,unsigned scenario) {for(unsigned i=0;i<n;++i)p[i]=Pattern(i,scenario);}
inline bool Matches(const uint8* p,unsigned n,unsigned scenario) {for(unsigned i=0;i<n;++i)if(p[i]!=Pattern(i,scenario))return false;return true;}
struct Helper {
    uintptr_t entry;std::vector<uint8> bytes;
    template<size_t N> Helper(uintptr_t at,const std::array<uint8,N>& canonical):entry(at),bytes(canonical.begin(),canonical.end()){}
    bool Check()const{return ModuleOf(entry)==GetModuleHandleA(nullptr)&&Resolve(entry)==entry&&ModuleOf(Resolve(entry))==GetModuleHandleA(nullptr)&&!ReversingProbe::JumpsToDll(entry)&&bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(entry),bytes.size());}
    json Metadata()const{return {{"entry",entry},{"body",Resolve(entry)},{"core",entry},{"core_bytes",std::vector<uint8>(bytes.begin(),bytes.begin()+16)},{"bytes",bytes},{"route_ok",Check()}};}
};
struct BodyProof {
    uintptr_t originalEntry{},candidateBody{},crt{},branch{},bridge{};
    unsigned length{},opcodeOffset{};
    std::vector<uint8> original,candidate,bridgeBytes;
    bool proven{};
    static uintptr_t Destination(uintptr_t at,const std::vector<uint8>& bytes,unsigned offset) {
        int32 displacement{};memcpy(&displacement,bytes.data()+offset+1,4);
        return uintptr_t(intptr_t(at+offset+5)+displacement);
    }
    void Capture(uintptr_t entry,uintptr_t body,unsigned n,unsigned offset,uintptr_t crtEntry) {
        originalEntry=entry;candidateBody=body;length=n;opcodeOffset=offset;crt=crtEntry;
        candidate=ReversingProbe::Bytes(reinterpret_cast<void*>(body),n);
        // original must have been captured while RH was OFF, before Capture is called ON.
        const auto opcode=offset==11?uint8(0xE9):uint8(0xE8);
        proven=original.size()==n&&candidate.size()==n&&original[offset]==opcode&&candidate[offset]==opcode;
        if(!proven)return;
        proven&=Destination(entry,original,offset)==crt;
        for(unsigned i=0;i<n;++i)if(i<=offset||i>offset+4)proven&=original[i]==candidate[i];
        branch=Destination(body,candidate,offset);bridge=Resolve(branch);
        bridgeBytes=ReversingProbe::Bytes(reinterpret_cast<void*>(bridge),6);
        uint32 pushed{};memcpy(&pushed,bridgeBytes.data()+1,4);
        proven&=ModuleOf(body)==ModuleOf(uintptr_t(&ModuleMarker))&&ModuleOf(branch)==ModuleOf(body)
            &&ModuleOf(bridge)==ModuleOf(body)&&bridgeBytes[0]==0x68&&bridgeBytes[5]==0xC3&&pushed==crt;
    }
    bool Check(bool candidateActive)const {
        if(!proven||Resolve(branch)!=bridge||bridgeBytes!=ReversingProbe::Bytes(reinterpret_cast<void*>(bridge),6))return false;
        return candidateActive ? candidate==ReversingProbe::Bytes(reinterpret_cast<void*>(candidateBody),length)
            : original==ReversingProbe::Bytes(reinterpret_cast<void*>(originalEntry),length);
    }
    json Metadata()const{return {{"entry",originalEntry},{"candidate_body",candidateBody},{"length",length},
        {"branch_opcode_offset",opcodeOffset},{"original",original},{"candidate",candidate},{"branch",branch},
        {"bridge",bridge},{"bridge_bytes",bridgeBytes},{"crt",crt},{"candidate_module",uintptr_t(ModuleOf(candidateBody))},
        {"bridge_module",uintptr_t(ModuleOf(bridge))},{"proven",proven}};}
};
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependenciesmemory9-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependenciesmemory9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependenciesmemory9");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    done=true;SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();fpActive=true;
    using MallocFn=void*(*)(uint32,uint32);
    using ReallocFn=uint8*(*)(void*,uint32,uint32);
    using AlignFn=uint8*(*)(uint32,uint32,uint32);
    const std::array<uintptr_t,4> symbols{uintptr_t(static_cast<MallocFn>(&CMemoryMgr::Malloc)),uintptr_t(static_cast<ReallocFn>(&CMemoryMgr::Realloc)),uintptr_t(static_cast<AlignFn>(&CMemoryMgr::MallocAlign)),uintptr_t(&CMemoryMgr::FreeAlign)};
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> initial;
    for(unsigned i=0;i<4;++i)for(auto at:{fns[i].address,symbols[i],Resolve(symbols[i])})initial.push_back({at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),16)});
    OriginalDependencies499::HookRoute r0("CMemoryMgr",fns[0].name,fns[0].address,static_cast<MallocFn>(&CMemoryMgr::Malloc));
    OriginalDependencies499::HookRoute r1("CMemoryMgr",fns[1].name,fns[1].address,static_cast<ReallocFn>(&CMemoryMgr::Realloc));
    OriginalDependencies499::HookRoute r2("CMemoryMgr",fns[2].name,fns[2].address,static_cast<AlignFn>(&CMemoryMgr::MallocAlign));
    OriginalDependencies499::HookRoute r3("CMemoryMgr",fns[3].name,fns[3].address,&CMemoryMgr::FreeAlign);
    const std::array routes{&r0,&r1,&r2,&r3};
    std::array<std::vector<uint8>,4> offCode;
    bool ready=true;
    for(unsigned i=0;i<4;++i){offCode[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);ready&=routes[i]->Registered()&&ModuleOf(routes[i]->Body())==ModuleOf(uintptr_t(&ModuleMarker));}
    std::array<Helper,3> helpers{Helper(0x824257,crt_824257),Helper(0x824269,crt_824269),Helper(0x82413F,crt_82413F)};
    auto helpersOK=[&]{return std::ranges::all_of(helpers,[](const auto& h){return h.Check();});};
    ready&=helpersOK();json helperRows=json::array();for(const auto& h:helpers)helperRows.push_back(h.Metadata());
    constexpr unsigned lengths[]{14,19,35,16},offsets[]{5,10,12,11};
    constexpr uintptr_t destinations[]{0x824257,0x824269,0x824257,0x82413F};
    std::array<BodyProof,4> bodyProofs;
    json bodyRows=json::array();
    for(unsigned i=0;i<4;++i){
        auto& proof=bodyProofs[i];proof.original=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),lengths[i]);
        routes[i]->Invoke(true,[&]{proof.Capture(fns[i].address,routes[i]->Body(),lengths[i],offsets[i],destinations[i]);return proof.proven;});
        ready&=proof.Check(false);bodyRows.push_back(proof.Metadata());
    }
    Write({{"event","begin"},{"registered_hook",ready},{"helper_routes_ok",helpersOK()},{"helper_routes",helperRows},
        {"candidate_body_contract",ready},{"body_proofs",bodyRows},{"normalization","owned block identity only; heap addresses and undefined bytes excluded"},{"crt_entries_modified",false}});
    const unsigned sizes[]{0,1,2,3,4,7,8,15,16,17,31,32,33,63,64,65,127,128,129,255,256,257,511,512,513,1023,1024,1025,2047,2048,2049,4096};
    const unsigned aligns[]{4,8,16,32,64,128,256};
    bool passed=ready;int pair=0;
    for(unsigned fn=0;fn<4&&ready;++fn){int failures=0;
        for(unsigned scenario=0;scenario<2048;++scenario){
            const unsigned size=sizes[scenario%std::size(sizes)],align=aligns[(scenario/std::size(sizes))%std::size(aligns)];
            const unsigned oldSize=(scenario/32%8==0)?0:sizes[1+(scenario/8)%31];
            const uint32 hint=scenario*2654435761u;
            auto invoke=[&](bool candidate){
                RestoreFloating();bool contract=true,balanced=true,helperOK=helpersOK(),routeOK=false,bodyOK=true;
                uint8* base=nullptr;uint8* input=nullptr;unsigned requested=0,allocated=0,freed=0;
                auto* canary=NativeMalloc(64);if(!canary)std::abort();++allocated;Fill(canary,64,scenario+1);
                std::array<uint32,3> args{size,hint,0};unsigned arity=2;
                if(fn==1){if(oldSize){base=NativeMalloc(oldSize);if(!base)std::abort();++allocated;Fill(base,oldSize,scenario);}input=base;args={uint32(uintptr_t(input)),size,hint};arity=3;}
                if(fn==2){args={size,align,hint};arity=3;}
                if(fn==3){requested=size+align;base=NativeMalloc(requested);if(!base)std::abort();++allocated;input=reinterpret_cast<uint8*>((uintptr_t(base)+align)&~uintptr_t(align-1));memcpy(input-4,&base,4);Fill(input,size,scenario);contract&=Matches(input,size,scenario);args={uint32(uintptr_t(input)),0,0};arity=1;}
                const auto result=routes[fn]->Invoke(candidate,[&]{routeOK=ReversingProbe::JumpsToDll(fns[fn].address)==candidate;
                    if(candidate)routeOK&=Resolve(fns[fn].address)==routes[fn]->Body();
                    else routeOK&=offCode[fn]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[fn].address),16);
                    bodyOK&=bodyProofs[fn].Check(candidate);
                    const auto called=Gate(fns[fn].address,args,arity);
                    bodyOK&=bodyProofs[fn].Check(candidate);
                    return called;});
                helperOK&=helpersOK();
                uint8* output=reinterpret_cast<uint8*>(result.eax);json payload=json::array();unsigned kept=0;
                bool argOK=fn==3?result.argAfter==uintptr_t(base):result.argAfter==args[0];
                const bool releaseOnly=fn==1&&input&&size==0;
                bool nullOK=fn==3||releaseOnly?true:output!=nullptr;
                if(fn==0){requested=size;base=output;if(base){++allocated;Fill(base,size,scenario);contract&=Matches(base,size,scenario);payload=ReversingProbe::Bytes(base,size);NativeFree(base);++freed;}else contract=false;}
                if(fn==1){requested=size;
                    if(releaseOnly){contract&=output==nullptr;++freed;base=nullptr;}
                    else if(output){kept=std::min(oldSize,size);contract&=Matches(output,kept,scenario);payload=ReversingProbe::Bytes(output,kept);
                        Fill(output,size,scenario);contract&=Matches(output,size,scenario);if(!input)++allocated;NativeFree(output);++freed;base=nullptr;
                    }else{contract=false;if(base){NativeFree(base);++freed;base=nullptr;}}
                }
                bool alignedOK=true,backpointerOK=true;
                if(fn==2){requested=size+align;
                    if(output){memcpy(&base,output-4,4);++allocated;alignedOK=(uintptr_t(output)&(align-1))==0;
                        backpointerOK=base&&uintptr_t(output)>=uintptr_t(base)+4&&uintptr_t(output)<=uintptr_t(base)+align;
                        contract&=alignedOK&&backpointerOK;if(!contract)std::abort();
                        Fill(output,size,scenario);payload=ReversingProbe::Bytes(output,size);contract&=Matches(output,size,scenario);NativeFree(base);++freed;
                    }else contract=false;
                }
                if(fn==3){++freed;output=nullptr;}
                const bool canariesOK=Matches(canary,64,scenario+1);NativeFree(canary);++freed;
                balanced=allocated==freed;helperOK&=helpersOK();RestoreFloating();
                return json{{"requested_size",requested},{"payload_size",size},{"old_size",fn==1?oldSize:0},{"alignment",fn>=2?align:0},{"hint",hint},{"prefix_preserved_bytes",kept},{"defined_payload",payload},
                    {"return_kind",fn==3?"void":releaseOnly?"null_after_free":"owned_block"},{"argument_after_matches",argOK},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},
                    {"route_ok",routeOK},{"cand_body_contract",bodyOK},{"helper_route_ok",helperOK},{"fixture_contract",contract},{"return_contract",nullOK},{"alignment_ok",alignedOK},{"backpointer_ok",backpointerOK},
                    {"canaries_preserved",canariesOK},{"allocations",allocated},{"frees",freed},{"resources_balanced",balanced},{"floating_state_restored",FloatingRestored()}};
            };
            const auto original=invoke(false);const auto candidate=invoke(true);
            bool ok=original==candidate;
            for(auto key:{"argument_after_matches","stack_restored","nonvolatile_preserved","route_ok","cand_body_contract","helper_route_ok","fixture_contract","return_contract","alignment_ok","backpointer_ok","resources_balanced","canaries_preserved","floating_state_restored"})ok&=original[key]==true&&candidate[key]==true;
            failures+=!ok;passed&=ok;
            Write({{"event","comparison"},{"function",std::string("CMemoryMgr::")+fns[fn].name},{"pair",pair++},{"case",scenario},{"original",original},{"candidate",candidate},{"equal",ok},{"accepted",ok}});
        }
        Write({{"event","function_summary"},{"function",std::string("CMemoryMgr::")+fns[fn].name},{"cases",2048},{"failures",failures}});
    }
    RestoreFloating();bool restored=helpersOK()&&FloatingRestored();
    for(unsigned i=0;i<4;++i)restored&=offCode[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),16);
    for(unsigned i=0;i<4;++i){restored&=bodyProofs[i].Check(false);const bool checked=routes[i]->Invoke(true,[&]{return bodyProofs[i].Check(true);});restored&=checked;}
    for(auto* route:routes){const bool r=route->Restore();restored&=r;}
    for(const auto& [at,bytes]:initial)restored&=bytes==ReversingProbe::Bytes(reinterpret_cast<void*>(at),16);
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pair},{"state_restored",restored},{"helper_route_ok",helpersOK()},{"floating_state_restored",FloatingRestored()}});
    RestoreFloating();fpActive=false;
}
}
