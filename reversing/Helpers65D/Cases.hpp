#pragma once
// Casos del banco Helpers65D (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y una funcion de caso por funcion.
#include "Core.hpp"
#include <utility>

namespace Hp65Live {
// ================================================================================================ estado de los falsos y arenas
struct FakeState {
    const C0* c0{}; const C1* c1{}; const C2* c2{}; const C3* c3{};
    uintptr_t newTaskObj{}, newRouteObj{}, pedObj{};
    u32 texIdx{};
};
inline FakeState g;
alignas(16) inline std::array<uint8,kMatArena> matArena{},matRef{};
alignas(16) inline std::array<uint8,kVecArena> ptArena{},ptRef{},outArena{},outRef{};
alignas(16) inline std::array<uint8,kWalkArena> walkArena{},walkRef{};
alignas(16) inline std::array<uint8,kRouteArena> routeArena{},routeRef{};
alignas(16) inline std::array<uint8,kNewTaskArena> newTaskArena{},newTaskRef{};
alignas(16) inline std::array<uint8,kNewRouteArena> newRouteArena{},newRouteRef{};
alignas(16) inline std::array<uint8,kFightArena> fightArena{},fightRef{};
alignas(16) inline std::array<uint8,kSmallArena> playerArena{},playerRef{},statsArena{},statsRef{};
alignas(16) inline std::array<uint8,kPedArena> pedArena{},pedRef{};

template<size_t N> inline json Changes(const std::array<uint8,N>& mem,const std::array<uint8,N>& ref,size_t objOff) {return ArenaChanges(mem.data(),ref.data(),N,objOff);}
template<size_t N> inline void Put32At(std::array<uint8,N>& a,size_t objOff,size_t off,u32 v) {memcpy(a.data()+objOff+off,&v,4);}
template<size_t N> inline void Put8At(std::array<uint8,N>& a,size_t objOff,size_t off,u32 v) {a[objOff+off]=uint8(v);}
template<size_t N> inline uintptr_t ObjAt(const std::array<uint8,N>& a,size_t objOff) {return uintptr_t(a.data())+objOff;}

// Cierra el lado s de un caso (fila comun + restauracion de regiones y estado x87)
inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}
// Valor de un puntero que devuelve la funcion, expresado respecto a un objeto de la arena (si cae dentro de [obj - 0x40, obj + 0x80)) o en crudo
inline u32 RelOrRaw(u32 value,uintptr_t obj) {return value-u32(obj)<0x80u||u32(obj)-value<=0x40u?0x10000000u+(value-u32(obj)):value;}

// ================================================================================================ 0: matriz * punto (0x59C890)
inline void CaseMatPoint(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(matArena.data(),kMatArena,n*7u+1u);FillWords(ptArena.data(),kVecArena,n*7u+2u);FillWords(outArena.data(),kVecArena,n*7u+3u);
        memcpy(matArena.data()+kMatGuard,c.m,sizeof(c.m));
        memcpy(ptArena.data()+kVecGuard,c.p,sizeof(c.p));
        matRef=matArena;ptRef=ptArena;outRef=outArena;
        const uintptr_t mat=ObjAt(matArena,kMatGuard),pt=ObjAt(ptArena,kVecGuard);
        uintptr_t out=ObjAt(outArena,kVecGuard);
        switch(c.alias) {
        case 1: out=pt;break;
        case 2: out=mat+0x30;break;
        case 3: out=mat;break;
        case 4: out=mat+0x10;break;
        case 5: out=mat+0x28;break;
        default: break;
        }
        const u32 args[3]{u32(out),u32(mat),u32(pt)};
        sides[s]=Execute(env,0,0x59C890,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,c.ecx,c.edx,c.stack,12);});
        json obs;
        obs["eax_rel"]=i32(sides[s].reg.eax-u32(out));   // 0 = devuelve el destino; ningun llamador del EXE lee ECX/EDX (ver README.md)
        obs["changes"]=json::array({Changes(matArena,matRef,kMatGuard),Changes(ptArena,ptRef,kVecGuard),Changes(outArena,outRef,kVecGuard)});
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: CreateRouteTask (0x655140)
inline void* __cdecl FakeTaskNew(size_t size) {Ev& e=PushEv(1,1);e.v[0]=u32(size);return g.c1->taskNull?nullptr:reinterpret_cast<void*>(g.newTaskObj);}
inline void* __cdecl FakeRouteNew(u32 size) {Ev& e=PushEv(2,1);e.v[0]=size;return g.c1->routeNull?nullptr:reinterpret_cast<void*>(g.newRouteObj);}
inline void CaseCreateRoute(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(walkArena.data(),kWalkArena,c.seed*3u+1u);FillWords(routeArena.data(),kRouteArena,c.seed*3u+2u);
        FillWords(newTaskArena.data(),kNewTaskArena,c.seed*3u+3u);FillWords(newRouteArena.data(),kNewRouteArena,c.seed*3u+4u);
        const uintptr_t walk=ObjAt(walkArena,kGuard),route=ObjAt(routeArena,kGuard);
        Put32At(walkArena,kGuard,0xC,c.moveState);Put32At(walkArena,kGuard,0x20,u32(route));
        Put32At(routeArena,kGuard,0,c.numEntries);
        memcpy(routeArena.data()+kGuard+4,c.entries,8*12);
        walkRef=walkArena;routeRef=routeArena;newTaskRef=newTaskArena;newRouteRef=newRouteArena;
        g.c1=&c;g.newTaskObj=ObjAt(newTaskArena,kGuard);g.newRouteObj=ObjAt(newRouteArena,kGuard);nev=0;
        const u32 args[1]{c.ped};
        sides[s]=Execute(env,1,0x655140,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(walk),c.edx,c.stack,0);});
        json obs;
        obs["eax"]=RelOrRaw(sides[s].reg.eax,g.newTaskObj);
        obs["changes"]=json::array({Changes(walkArena,walkRef,kGuard),Changes(routeArena,routeRef,kGuard),Changes(newTaskArena,newTaskRef,kGuard),Changes(newRouteArena,newRouteRef,kGuard)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2: RwTexDictionaryGtaStreamRead (0x730FC0)
inline u32 __cdecl FakeFindChunk(u32 stream,u32 type,u32* length,u32* version) {
    Ev& e=PushEv(10,2);e.v[0]=stream;e.v[1]=type;
    if(g.c2->findRet){*length=g.c2->chunkLen;*version=g.c2->version;}
    return g.c2->findRet;
}
inline u32 __cdecl FakeStreamRead(u32 stream,void* buffer,u32 length) {
    Ev& e=PushEv(11,2);e.v[0]=stream;e.v[1]=length;
    memcpy(buffer,g.c2->payload,length<12u?length:12u);   // como mucho el marco de 12 bytes del EXE
    return g.c2->readRet;
}
inline u32 __cdecl FakeCreateDict() {PushEv(12,0);return g.c2->createRet;}
inline u32 __cdecl FakeTexRead(u32 stream) {
    const u32 idx=g.texIdx++;
    Ev& e=PushEv(13,2);e.v[0]=stream;e.v[1]=idx;
    return idx==g.c2->texFail?0u:g.c2->texTok[idx%kTexTokens];
}
inline u32 __cdecl FakeAddTex(u32 dict,u32 tex) {Ev& e=PushEv(14,2);e.v[0]=dict;e.v[1]=tex;return g.c2->addRet[(g.texIdx?g.texIdx-1:0)%kTexTokens];}
inline u32 __cdecl FakeForAll(u32 dict,u32 callback,u32 data) {Ev& e=PushEv(15,3);e.v[0]=dict;e.v[1]=callback;e.v[2]=data;return g.c2->forAllRet;}
inline u32 __cdecl FakeDestroyDict(u32 dict) {Ev& e=PushEv(16,1);e.v[0]=dict;return g.c2->destroyRet;}
inline void CaseTexDict(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    const json input=json::parse(JsonC2(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        g.c2=&c;g.texIdx=0;nev=0;
        const u32 args[1]{c.stream};
        sides[s]=Execute(env,2,0x730FC0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["eax"]=sides[s].reg.eax;
        obs["events"]=EvsJson();obs["tex_reads"]=g.texIdx;
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ 3: FightStrikeDamageExe (0x61C740)
inline u32 __fastcall FakeIsPlayer(void* self,void*) {Ev& e=PushEv(20,1);e.v[0]=u32(uintptr_t(self)-g.pedObj);return g.c3->isPlayerRet;}
inline float __cdecl FakeStat(u32 which) {Ev& e=PushEv(21,1);e.v[0]=which;return F(g.c3->stat);}
inline void CaseFightDamage(Env& env,Totals& t,unsigned n) {
    const C3 c=MakeC3(n);
    const json input=json::parse(JsonC3(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(fightArena.data(),kFightArena,c.seed*5u+1u);FillWords(playerArena.data(),kSmallArena,c.seed*5u+2u);FillWords(statsArena.data(),kSmallArena,c.seed*5u+3u);
        FillWords(pedArena.data(),kPedArena,c.seed*5u+4u);
        const uintptr_t task=ObjAt(fightArena,kGuard),ped=ObjAt(pedArena,kGuard+kPedBefore),player=ObjAt(playerArena,kGuard),stats=ObjAt(statsArena,kGuard);
        Put8At(fightArena,kGuard,0x24,c.combo);Put8At(fightArena,kGuard,0x25,c.move);
        Put8At(playerArena,kGuard,0x34,c.playerByte);Put32At(statsArena,kGuard,0x28,c.attack);
        const size_t pedOff=kGuard+kPedBefore;
        Put32At(pedArena,pedOff,0x480,u32(player));Put32At(pedArena,pedOff,0x59C,u32(stats));Put8At(pedArena,pedOff,0x718,c.slot);
        Put32At(pedArena,pedOff,size_t(ptrdiff_t(0x5A0)+ptrdiff_t(i8(c.slot))*0x1C),c.weaponType);
        // tabla de combos: bytes del caso y el byte de danio de la fila (combo - 4) * 0x88 + 0x55 + movimiento (ambos con signo)
        FillWords(Mem(kRTable),kTableBytes,c.tableSeed);
        *reinterpret_cast<uint8*>(uintptr_t(kTableBase)+ptrdiff_t(i8(c.combo)-4)*0x88+0x55+ptrdiff_t(i8(c.move)))=uint8(c.tableByte);
        fightRef=fightArena;playerRef=playerArena;statsRef=statsArena;pedRef=pedArena;
        g.c3=&c;g.pedObj=ped;nev=0;
        const u32 args[1]{u32(ped)};
        sides[s]=Execute(env,3,0x61C740,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(task),c.edx,c.stack,0);});
        json obs;
        obs["st0"]=HexBytes(sides[s].fp.data()+28,10);   // ST(0) de la imagen de FNSAVE: el resultado float de la funcion
        obs["changes"]=json::array({Changes(fightArena,fightRef,kGuard),Changes(playerArena,playerRef,kGuard),Changes(statsArena,statsRef,kGuard),Changes(pedArena,pedRef,pedOff)});
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
                case 0: CaseMatPoint(env,totals,c); break;
                case 1: CaseCreateRoute(env,totals,c); break;
                case 2: CaseTexDict(env,totals,c); break;
                default: CaseFightDamage(env,totals,c); break;
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
