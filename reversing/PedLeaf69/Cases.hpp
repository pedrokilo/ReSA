#pragma once
// Casos del banco PedLeaf69 (continua el espacio de nombres de Core.hpp): falso del colaborador y una funcion de caso por funcion.
#include "Core.hpp"
#include <utility>

namespace Pl69Live {
// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    bool Contains(uintptr_t p) const {return p>=uintptr_t(m.data())&&p<uintptr_t(m.data())+N;}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kIntelArena> intelA;
inline Arena<kEntArena> entA,entB,entC;
inline Arena<kMatArena> matSelf;
inline Arena<kVecArena> vecA;
inline Arena<kStatsArena> statsA;

inline uint32& G32(uintptr_t a) {return *reinterpret_cast<uint32*>(a);}
// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    obs["globals_hash"]=RegionsHash();
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}
// Posicion de una entidad / matriz: la usada va donde la lee la funcion (matriz +0x30 si hay matriz, +4 si no) y la otra es ruido que nadie debe leer
template<size_t N,size_t M> inline void PlaceEntity(Arena<N>& ent,Arena<M>& mat,bool hasMat,const u32* used,const u32* other) {
    ent.Put32(0x14,hasMat?u32(mat.Obj()):0u);
    for(unsigned i=0;i<3;++i) {ent.Put32(4+4*i,hasMat?other[i]:used[i]);mat.Put32(0x30+4*i,hasMat?used[i]:other[i]);}
}

// ================================================================================================ 0: IsInHearingRange (0x600C00)
inline void CaseHearing(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        intelA.Fill(c.seed*7u+1u);entA.Fill(c.seed*7u+2u);matSelf.Fill(c.seed*7u+3u);vecA.Fill(c.seed*7u+4u);
        intelA.Put32(0,u32(entA.Obj()));intelA.Put32(kIntelRange,c.range);
        PlaceEntity(entA,matSelf,c.hasMat,c.ped,c.other);
        for(unsigned i=0;i<3;++i) vecA.Put32(4*i,c.posn[i]);
        intelA.Snap();entA.Snap();matSelf.Snap();vecA.Snap();
        TagClear();intelA.Tagged();entA.Tagged();matSelf.Tagged();vecA.Tagged();nev=0;
        const u32 args[1]{u32(vecA.Obj())};
        sides[s]=Execute(env,0,0x600C00,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(intelA.Obj()),c.edx,c.stack,0);});   // `ret 4`: limpia el callee
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;   // sin llamadores en el EXE: solo el valor de retorno (AL)
        obs["changes"]=json::array({intelA.Changes(),entA.Changes(),matSelf.Changes(),vecA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1 y 2: entidades interesantes
inline u32 SlotValue(u32 kind,u32 raw) {
    switch(kind) {
    case 0: return 0u;
    case 1: return u32(entA.Obj());
    case 2: return u32(entB.Obj());
    case 3: return u32(entC.Obj());
    default: return raw;
    }
}
inline void SetupInteresting(const CS& c) {
    intelA.Fill(c.seed*7u+1u);entA.Fill(c.seed*7u+2u);entB.Fill(c.seed*7u+3u);entC.Fill(c.seed*7u+4u);
    for(unsigned i=0;i<3;++i) intelA.Put32(kIntelEntities+4*i,SlotValue(c.kind[i],c.raw[i]));
    if(c.before!=1) intelA.Put32(kIntelEntities-4,c.before==0?0u:0xA5A5A5A5u);        // palabra vecina anterior
    if(c.after!=1) intelA.Put32(kIntelEntities+12,c.after==0?0u:0xA5A5A5A5u);          // palabra vecina posterior
    intelA.Snap();entA.Snap();entB.Snap();entC.Snap();
    TagClear();intelA.Tagged();entA.Tagged();entB.Tagged();entC.Tagged();nev=0;
}
inline void CaseHasInteresting(Env& env,Totals& t,unsigned n) {
    const CS c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetupInteresting(c);
        sides[s]=Execute(env,1,0x602080,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(intelA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;
        obs["changes"]=json::array({intelA.Changes(),entA.Changes(),entB.Changes(),entC.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// 0x571A00 CEntity::CleanUpOldReference(CEntity** ref): thiscall, ECX = entidad, `ret 4`. Registra la entidad, la direccion de la ranura y su valor en ese instante (aun no puesto a cero).
inline void __fastcall FakeCleanUp(void* self,void*,void** ref) {
    Ev& e=PushEv(71,3);
    e.v[0]=Tag(self);e.v[1]=Tag(uintptr_t(ref));
    e.v[2]=intelA.Contains(uintptr_t(ref))?Tag(uintptr_t(*ref)):0xBADBAD00u;
}
inline void CaseRemoveAll(Env& env,Totals& t,unsigned n) {
    const CS c=MakeC2(n);
    const json input=json::parse(JsonC2(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetupInteresting(c);
        sides[s]=Execute(env,2,0x602320,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(intelA.Obj()),c.edx,c.stack,0);});
        json obs;   // void
        json changes=intelA.Changes(),others=json::array();
        for(const auto& ch:changes) if(ch[0].get<int64>()<int64(kIntelEntities)||ch[0].get<int64>()>=int64(kIntelEntities+12)) others.push_back(ch);
        obs["slots_after"]=json::array({intelA.Get32(kIntelEntities),intelA.Get32(kIntelEntities+4),intelA.Get32(kIntelEntities+8)});
        obs["other_changes"]=others;   // todo salvo las tres ranuras: debe quedar intacto
        obs["changes"]=json::array({changes,entA.Changes(),entB.Changes(),entC.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ 3: FindIndexWithPedStat (0x608940)
inline void CaseFindIndex(Env& env,Totals& t,unsigned n) {
    const C3 c=MakeC3(n);
    const json input=json::parse(JsonC3(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        statsA.Fill(c.seed*7u+1u);statsA.Snap();
        const u32 base=c.baseKind==0?u32(statsA.Obj()):c.baseKind==1?0u:c.baseRaw;
        G32(kPedStatsPtr)=base;
        const u32 arg=c.argKind==0?base+c.delta:c.argRaw;
        TagClear();statsA.Tagged();nev=0;
        const u32 args[1]{arg};
        sides[s]=Execute(env,3,0x608940,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});   // cdecl: limpia el llamador
        json obs;
        obs["eax"]=sides[s].reg.eax;   // sin llamadores en el EXE: solo el valor de retorno (EAX completo)
        obs["changes"]=json::array({statsA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[3],n,input,rows,sides);
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
            for(unsigned c=0;c<kCases;++c) {
                switch(f) {
                case 0: CaseHearing(env,totals,c); break;
                case 1: CaseHasInteresting(env,totals,c); break;
                case 2: CaseRemoveAll(env,totals,c); break;
                default: CaseFindIndex(env,totals,c); break;
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
