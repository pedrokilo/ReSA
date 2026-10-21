#pragma once
// Casos del banco CarCtrlRest63E (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y la funcion de caso de ChooseModel.
#include "Core.hpp"
#include <utility>

namespace Cc63eLive {
struct FakeState {
    const E0* c{};
    u32 randIdx{};
};
inline FakeState g;
alignas(16) inline std::array<u32,kCatArenaWords> catArena{},catRef{};
alignas(16) inline std::array<uint8,16> zoneArena{};

template<size_t N> inline json WordChanges(const std::array<u32,N>& mem,const std::array<u32,N>& ref,size_t objWord) {
    json out=json::array();
    for(size_t i=0;i<N;++i)if(mem[i]!=ref[i])out.push_back(json::array({int64(i)-int64(objWord),mem[i]}));
    return out;
}
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ 0: ChooseModel
inline u32 __cdecl FakeRand() {
    const u32 idx=g.randIdx++;
    const u32 value=g.c->rnd[idx%kRandSeq];
    Ev& e=PushEv(100,2);e.v[0]=idx;e.v[1]=value;
    return value;
}
inline u32 __cdecl FakeGangWar() {Ev& e=PushEv(101,1);e.v[0]=0;return g.c->gangWarRet;}
inline u32 __cdecl FakePolice(u32 ignore) {Ev& e=PushEv(102,1);e.v[0]=ignore;return g.c->policeRet;}
inline u32 __fastcall FakePick(void* self,void*,u32 a,u32 b) {Ev& e=PushEv(103,3);e.v[0]=self==reinterpret_cast<void*>(0xC0E9F8)?1u:0u;e.v[1]=a&0xFFu;e.v[2]=b&0xFFu;return g.c->pickRet;}
inline u32 __cdecl FakeGangCar(u32 gang) {Ev& e=PushEv(104,1);e.v[0]=gang;return g.c->gangCarRet;}
inline u32 __cdecl FakeRange(u32 lo,u32 hi) {Ev& e=PushEv(105,2);e.v[0]=lo;e.v[1]=hi;return g.c->rangeRet;}
inline void CaseChooseModel(Env& env,Totals& t,unsigned n) {
    const E0 c=MakeE0(n);
    const json input=json::parse(JsonE0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        memcpy(Mem(kRDensities),c.dens,16);
        Mem(kRFlags)[0]=uint8(c.flagB);Mem(kRFlags)[0x1F]=uint8(c.flagA);
        {const u16 w=u16(c.dealerModel);memcpy(Mem(kRDealerModel),&w,2);}
        for(u32 g2=0;g2<kGroups;++g2)for(u32 w=0;w<kGroupWords;++w){const u16 v=u16(c.table[g2][w]);memcpy(Mem(kRTable)+(g2*kGroupWords+w)*2,&v,2);}
        Mem(kRGangs)[2]=uint8(c.dontGang);Mem(kRGangs)[4]=uint8(c.dontCops);
        for(u32 m=0;m<kModels;++m)Mem(kRInfo)[m*20]=uint8(c.loaded[m]);
        memcpy(Mem(kROverride),&c.forced,4);
        FillWords(zoneArena.data(),16,n+77u);
        for(u32 i=0;i<11;++i)zoneArena[i]=uint8(c.strength[i]);
        const u32 zone=u32(uintptr_t(zoneArena.data()));
        memcpy(Mem(kRZonePtr),&zone,4);
        for(u32 i=0;i<kCatArenaWords;++i)catArena[i]=0xC0DE0000u+i+n;
        catRef=catArena;
        g.c=&c;g.randIdx=0;nev=0;
        const u32 args[1]{u32(uintptr_t(&catArena[kCatGuard]))};
        sides[s]=Execute(env,0,0x424CE0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["eax"]=sides[s].reg.eax;obs["changes"]=WordChanges(catArena,catRef,kCatGuard);obs["events"]=EvsJson();obs["rand_calls"]=g.randIdx;
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion
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
            for(unsigned c=0;c<kCases;++c)CaseChooseModel(env,totals,c);
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
