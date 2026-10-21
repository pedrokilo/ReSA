#pragma once
// Casos del banco SmallHelpers64B (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y las tres funciones de caso.
#include "Core.hpp"

namespace Small64Live {
// ================================================================================================ estado de los falsos
struct FakeState {
    // 1: FlushWaterRenderBuffer
    u32 retT{}, retR{};
    // 2: destructor de CTaskComplex
    uintptr_t obj{}, subA{}, subB{};
};
inline FakeState g;
inline u32 DrawCounter() {return *reinterpret_cast<volatile u32*>(kAddrDraw);}

// ================================================================================================ 0: GetPopcycleGroupPedModel
inline void ApplyA0(const A0& c) {
    uint8* t=Mem(kRTrans);
    for(size_t i=0;i<kTransBytes/4;++i){const u32 v=u32(i32(Rand(c.seedT,u32(i))%60)-1);memcpy(t+4*i,&v,4);}
    FillWords(Mem(kRPed),kPedBytes,c.seedP);
}
inline void CaseModel(Env& env,Totals& t,unsigned n) {
    const A0 c=MakeA0(n);
    const json input=json::parse(JsonA0(c));
    const u32 args[3]{c.group,c.slot,c.zone};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ApplyA0(c);
        const auto refT=ReversingProbe::Bytes(Mem(kRTrans),kTransBytes);
        const auto refP=ReversingProbe::Bytes(Mem(kRPed),kPedBytes);
        sides[s]=Execute(env,0,0x406E80,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,c.ecx,c.edx,c.stack,12);});
        json obs;
        obs["ret_eax"]=sides[s].reg.eax;   // el valor devuelto: se compara entero (movzx de 16 bits)
        obs["tables_unchanged"]=refT==ReversingProbe::Bytes(Mem(kRTrans),kTransBytes)&&refP==ReversingProbe::Bytes(Mem(kRPed),kPedBytes);
        if(kCompareVolatileRegisters){obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        RestoreRegions();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: FlushWaterRenderBuffer
inline u32 __cdecl FakeTransform(u32 verts,u32 count,u32 ltm,u32 flags) {
    Ev& e=PushEv(1,5);e.v[0]=verts;e.v[1]=count;e.v[2]=ltm;e.v[3]=flags;e.v[4]=DrawCounter();
    return g.retT;
}
inline u32 __cdecl FakeRender(u32 prim,u32 indices,u32 count) {
    Ev& e=PushEv(2,4);e.v[0]=prim;e.v[1]=indices;e.v[2]=count;e.v[3]=DrawCounter();
    return g.retR;
}
inline u32 __cdecl FakeEnd() {
    Ev& e=PushEv(3,1);e.v[0]=DrawCounter();
    return g.retR;
}
inline void CaseFlush(Env& env,Totals& t,unsigned n) {
    const B1 c=MakeB1(n);
    g.retT=c.retT;g.retR=c.retR;
    const json input=json::parse(JsonB1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetCount(kRCounters,0,c.nVtx);SetCount(kRCounters,4,c.nIdx);SetCount(kRDraw,0,c.draw);
        nev=0;
        sides[s]=Execute(env,1,0x6E7680,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,c.ecx,c.edx,c.stack,0);});
        json obs;
        obs["events"]=EvsJson();obs["nevents"]=nev;
        obs["n_vtx_after"]=GetCount(kRCounters,0);obs["n_idx_after"]=GetCount(kRCounters,4);obs["draw_after"]=GetCount(kRDraw,0);
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        RestoreRegions();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2: cuerpo de ~CTaskComplex (0x61A3D0)
alignas(16) inline std::array<uint8,kTaskArena> taskArena{},taskRef{};
alignas(16) inline std::array<u32,4> subObjA{},subObjB{};
inline void* fakeVtable[1]{};
// Destructor virtual con borrado falso: `push 1; call [vtable]` con ECX = objeto; lo llama el original y el candidato, ambos con el objeto dueno en un estado observable.
inline void __fastcall FakeDeletingDtor(void* self,void* /*edx*/,u32 flag) {
    Ev& e=PushEv(1,5);
    e.v[0]=uintptr_t(self)==g.subA?1u:(uintptr_t(self)==g.subB?2u:3u);
    e.v[1]=flag;
    e.v[2]=At<u32>(reinterpret_cast<void*>(g.obj),0);                       // vtable del dueno en ese momento (debe ser la de CTaskComplex del EXE)
    e.v[3]=At<u32>(reinterpret_cast<void*>(g.obj),8)==u32(uintptr_t(self))?1u:0u;   // el dueno aun apunta a la subtarea
    e.v[4]=At<u32>(reinterpret_cast<void*>(g.obj),4);
}
inline void CaseTaskDtor(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    uint8* obj=taskArena.data()+kTaskObj;
    g.obj=uintptr_t(obj);g.subA=uintptr_t(subObjA.data());g.subB=uintptr_t(subObjB.data());
    fakeVtable[0]=reinterpret_cast<void*>(&FakeDeletingDtor);
    FillWords(taskRef.data(),kTaskArena,c.guardSeed);
    uint8* ro=taskRef.data()+kTaskObj;
    const u32 subPtr=c.sub==0?0u:(c.sub==1?u32(g.subA):u32(g.subB));
    At<u32>(ro,0)=c.vtable;At<u32>(ro,4)=c.parent;At<u32>(ro,8)=subPtr;At<u32>(ro,12)=c.tail;
    const json input=json::parse(JsonC2(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetArena(taskArena,taskRef);nev=0;
        subObjA={u32(uintptr_t(fakeVtable)),0x11111111u,0x22222222u,0x33333333u};subObjB={u32(uintptr_t(fakeVtable)),0x44444444u,0x55555555u,0x66666666u};
        const auto subRefA=subObjA;const auto subRefB=subObjB;
        sides[s]=Execute(env,2,0x61A3D0,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(uintptr_t(obj)),c.edx,c.stack,0);});
        json obs;
        obs["events"]=EvsJson();obs["nevents"]=nev;
        json words=json::array();for(u32 i=0;i<kTaskObjBytes/4;++i)words.push_back(At<u32>(obj,4*i));
        obs["obj"]=words;
        bool outside=true;
        for(u32 i=0;i<kTaskArena;++i)if((i<kTaskObj||i>=kTaskObj+kTaskObjBytes)&&taskArena[i]!=taskRef[i])outside=false;
        obs["outside_unchanged"]=outside;
        obs["sub_unchanged"]=subObjA==subRefA&&subObjB==subRefB;
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    g.obj=0;
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFunctions;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunctions},{"pairs",kFunctions*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFunctions;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c) {
                switch(f) {
                case 0: CaseModel(env,totals,c); break;
                case 1: CaseFlush(env,totals,c); break;
                default: CaseTaskDtor(env,totals,c); break;
                }
            }
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&RegionsIntact()}});
        }
    }
    RestoreRegions();
    const bool entriesIntact=env.WatchesIntact()&&RegionsIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
