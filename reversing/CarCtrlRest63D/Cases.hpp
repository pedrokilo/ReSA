#pragma once
// Casos del banco CarCtrlRest63D (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y una funcion de caso por funcion.
#include "Core.hpp"
#include <utility>

namespace Cc63dLive {
// ================================================================================================ estado de los falsos y arenas
struct FakeState {
    uintptr_t vehObj{}, matObj{}, targetObj{};
    const D0* d0{}; const D1* d1{};
    u32 searchCall{}, distCall{};
};
inline FakeState g;
alignas(16) inline std::array<uint8,kVehArena> vehArena{},vehRef{};
alignas(16) inline std::array<uint8,kMatArena> matArena{},matRef{};
alignas(16) inline std::array<uint8,kVehArena> targetArena{};
alignas(16) inline std::array<uint8,kMatArena> targetMat{};
alignas(16) inline std::array<uint8,kNodeArena> nodeArena{},nodeRef{};
alignas(16) inline std::array<uint8,kLinkArena> linkArena{},linkRef{};
alignas(16) inline std::array<uint8,kNaviArena> naviArena{},naviRef{};

inline uintptr_t VehObj() {return uintptr_t(vehArena.data())+kGuard;}
inline uintptr_t MatObj() {return uintptr_t(matArena.data())+kMatGuard;}
inline uintptr_t TargetObj() {return uintptr_t(targetArena.data())+kGuard;}
inline uintptr_t TargetMatObj() {return uintptr_t(targetMat.data())+kMatGuard;}
inline void Put32(std::array<uint8,kVehArena>& a,size_t off,u32 v) {memcpy(a.data()+kGuard+off,&v,4);}
inline void Put16(std::array<uint8,kVehArena>& a,size_t off,u32 v) {const u16 w=u16(v);memcpy(a.data()+kGuard+off,&w,2);}
inline void Put8(std::array<uint8,kVehArena>& a,size_t off,u32 v) {a[kGuard+off]=uint8(v);}
inline void ResetVehicle(u32 seed) {FillWords(vehArena.data(),kVehArena,seed*3u+11u);}
template<size_t N> inline json Changes(const std::array<uint8,N>& mem,const std::array<uint8,N>& ref,size_t objOff) {return ArenaChanges(mem.data(),ref.data(),N,objOff);}
inline u32 Rel(const void* p) {return u32(uintptr_t(p)-g.vehObj);}
inline u32 R32(uintptr_t at) {u32 v;memcpy(&v,reinterpret_cast<const void*>(at),4);return v;}
// Un puntero de pila / nulo / dentro del vehiculo, sin registrar direcciones absolutas: 0 nulo, 0x80000000 | desplazamiento dentro del vehiculo, 1 otro
inline u32 PtrTag(u32 p) {return p==0?0u:(p>=u32(g.vehObj)&&p<u32(g.vehObj)+kVehWindow)?(0x80000000u|(p-u32(g.vehObj))):1u;}

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
inline void PlacePosition(u32 matMode,const u32* pos,const u32* decoy) {
    FillWords(matArena.data(),kMatArena,0x777u+matMode);
    if(matMode==0) {
        Put32(vehArena,0x14,0);Put32(vehArena,4,pos[0]);Put32(vehArena,8,pos[1]);
        memcpy(matArena.data()+kMatGuard+0x30,&decoy[0],4);memcpy(matArena.data()+kMatGuard+0x34,&decoy[1],4);
    } else {
        Put32(vehArena,0x14,u32(MatObj()));Put32(vehArena,4,decoy[0]);Put32(vehArena,8,decoy[1]);
        memcpy(matArena.data()+kMatGuard+0x30,&pos[0],4);memcpy(matArena.data()+kMatGuard+0x34,&pos[1],4);
    }
}
inline void SetupNodes(const NodeSet& ns,u32 seed,const u32 (*navi)[kNaviPerArea]) {
    FillWords(nodeArena.data(),kNodeArena,seed*13u+1u);FillWords(linkArena.data(),kLinkArena,seed*17u+2u);FillWords(naviArena.data(),kNaviArena,seed*19u+4u);
    const uintptr_t nodeBase=uintptr_t(nodeArena.data())+kNodeGuard,linkBase=uintptr_t(linkArena.data())+kLinkGuard,naviBase=uintptr_t(naviArena.data())+kNaviGuard;
    for(u32 i=0;i<kPoolAreas;++i){SetPtr(kRPathNodes,i,0);SetPtr(kRNodeLinks,i,0);SetPtr(kRNaviLinks,i,0);}
    for(u32 sl=0;sl<kNodeAreas;++sl){
        SetPtr(kRPathNodes,ns.areaIds[sl],nodeBase+sl*kNodesPerArea*kNodeBytes);SetPtr(kRNodeLinks,ns.areaIds[sl],linkBase+sl*kLinksPerArea*4);SetPtr(kRNaviLinks,ns.areaIds[sl],naviBase+sl*kNaviPerArea*2);
    }
    for(u32 i=0;i<kNodeAreas*kNodesPerArea;++i) {
        uint8* node=nodeArena.data()+kNodeGuard+i*kNodeBytes;
        memcpy(node+8,&ns.x[i],2);memcpy(node+0xA,&ns.y[i],2);memcpy(node+0xC,&ns.z[i],2);memcpy(node+0x10,&ns.base[i],2);
        node[0x18]=ns.b18[i];node[0x19]=ns.b19[i];node[0x1A]=ns.b1A[i];
    }
    for(u32 a=0;a<kNodeAreas;++a)memcpy(linkArena.data()+kLinkGuard+a*kLinksPerArea*4,ns.link[a],kLinksPerArea*4);
    if(navi)for(u32 a=0;a<kNodeAreas;++a)for(u32 l=0;l<kNaviPerArea;++l){const u16 w=u16(navi[a][l]);memcpy(naviArena.data()+kNaviGuard+(a*kNaviPerArea+l)*2,&w,2);}
}

// ================================================================================================ 0: ReconsiderRoute
inline void __cdecl FakeNearest(void* veh,u32* a1,u32* a2) {
    Ev& e=PushEv(80,3);e.v[0]=Rel(veh);e.v[1]=*a1&0xFFFFu;e.v[2]=*a2&0xFFFFu;   // las palabras de nodo iniciales no se registran (el EXE no las inicializa)
    *a1=g.d0->first;*a2=g.d0->second;
}
inline float* __cdecl FakeCoors(float* out,u32 playerId) {
    Ev& e=PushEv(81,1);e.v[0]=playerId;
    memcpy(out,g.d0->playerCoords,12);
    return out;
}
inline void __fastcall FakeDoPathSearch(void* self,void*,u32 a0,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10,u32 a11,u32 a12,u32 a13,u32 a14,u32 a15,u32 a16,u32 a17,u32 a18) {
    const u32 call=g.searchCall++;
    Ev& e=PushEv(82,21);
    e.v[0]=self==reinterpret_cast<void*>(0x96F050)?1u:0u;e.v[1]=call;
    e.v[2]=a0;e.v[3]=a1;e.v[4]=a2;e.v[5]=a3;e.v[6]=a4;e.v[7]=a5;e.v[8]=a6;e.v[9]=a7;e.v[10]=PtrTag(a8);e.v[11]=PtrTag(a9);e.v[12]=a10;e.v[13]=PtrTag(a11);e.v[14]=a12;
    e.v[15]=PtrTag(a13);e.v[16]=a14;e.v[17]=a15&0xFFu;e.v[18]=a16;e.v[19]=a17&0xFFu;e.v[20]=a18&0xFFu;
    if(call==0) {
        if(a11)memcpy(reinterpret_cast<void*>(uintptr_t(a11)),&g.d0->dist,4);
        const u16 c=u16(g.d0->count1);
        if(a9)memcpy(reinterpret_cast<void*>(uintptr_t(a9)),&c,2);
    } else {
        const u16 c=u16(g.d0->count2);
        if(a9)memcpy(reinterpret_cast<void*>(uintptr_t(a9)),&c,2);
        if(a8)memcpy(reinterpret_cast<void*>(uintptr_t(a8)),g.d0->nodes,32);
    }
}
template<u32 K> inline void __cdecl FakeFindLinks(void* veh) {
    Ev& e=PushEv(K,4);e.v[0]=Rel(veh);e.v[1]=R32(uintptr_t(veh)+0x390);e.v[2]=R32(uintptr_t(veh)+0x394);e.v[3]=R32(uintptr_t(veh)+0x398)&0xFFFFu;
}
inline void __fastcall FakeRemoveOne(void* self,void*) {
    uint8* ap=static_cast<uint8*>(self);
    i16 count;memcpy(&count,ap+0x88,2);
    Ev& e=PushEv(84,3);e.v[0]=Rel(self);e.v[1]=u32(u16(count));e.v[2]=R32(uintptr_t(ap)+0x68);
}
inline void CaseReconsider(Env& env,Totals& t,unsigned n) {
    const D0 c=MakeD0(n);
    const json input=json::parse(JsonD0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);
        PlacePosition(c.matMode,c.pos,c.vdecoy);
        Put16(vehArena,0x20,c.seed);Put16(vehArena,0x22,c.model);Put8(vehArena,0x3BA,c.mission);Put32(vehArena,0x390,c.cur);Put32(vehArena,0x394,c.start);Put32(vehArena,0x398,c.ending);
        Put8(vehArena,0x3C5,c.counter);Put8(vehArena,0x3DB,c.flags3DB);
        for(u32 i=0;i<3;++i)Put32(vehArena,0x3EC+4*i,c.dest[i]);
        FillWords(targetArena.data(),kVehArena,n*5u+3u);FillWords(targetMat.data(),kMatArena,n*7u+5u);
        {
            auto putT=[&](size_t off,u32 v){memcpy(targetArena.data()+kGuard+off,&v,4);};
            if(c.targetMat) {
                putT(0x14,u32(TargetMatObj()));putT(4,c.decoy[0]);putT(8,c.decoy[1]);
                for(u32 i=0;i<3;++i)memcpy(targetMat.data()+kMatGuard+0x30+4*i,&c.targetPos[i],4);
            } else {
                putT(0x14,0);for(u32 i=0;i<3;++i)putT(4+4*i,c.targetPos[i]);
            }
            Put32(vehArena,0x41C,u32(TargetObj()));
        }
        memcpy(Mem(kRTimerPrev),&c.prev,4);memcpy(Mem(kRTimerNow),&c.now,4);
        vehRef=vehArena;matRef=matArena;
        g.vehObj=VehObj();g.d0=&c;g.searchCall=0;nev=0;
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,0,0x42FC40,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard)});obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: FindLinksToGoWithTheseNodes
inline void __cdecl FakeSrand(u32 seed) {Ev& e=PushEv(90,1);e.v[0]=seed;}
inline float __cdecl FakeDistToLine(const float* a,const float* b,const float* p) {
    const u32 call=g.distCall++;
    Ev& e=PushEv(91,10);
    for(u32 i=0;i<3;++i){e.v[i]=BitsOf(a+i);e.v[3+i]=BitsOf(b+i);e.v[6+i]=BitsOf(p+i);}
    e.v[9]=call;
    return FloatOfBits(g.d1->dists[call%kDistList]);
}
inline void CaseFindLinks(Env& env,Totals& t,unsigned n) {
    const D1 c=MakeD1(n);
    const json input=json::parse(JsonD1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);SetupNodes(c.ns,n,c.navi);
        PlacePosition(c.matMode,c.pos,c.vdecoy);
        const u32 curIdx=c.curSlot*kNodesPerArea+c.curNode;
        const u32 cur=c.ns.areaIds[c.curSlot]|(c.curNode<<16);
        u32 start;
        switch(c.startSel) {
        case 0: start=c.ns.link[c.curSlot][u32(i32(c.ns.base[curIdx]))+c.startIdx]; break;
        case 1: start=c.ns.areaIds[c.startSlot]|(c.startNode<<16); break;
        case 2: start=cur; break;
        case 3: start=c.ns.areaIds[c.curSlot]|((((c.curNode+(c.startIdx&1?1u:7u))&7u))<<16); break;
        default: start=c.ns.areaIds[(c.curSlot+1)%kNodeAreas]|(c.startNode<<16); break;
        }
        Put32(vehArena,0x390,cur);Put32(vehArena,0x394,start);Put16(vehArena,0x45E,c.seed);
        vehRef=vehArena;matRef=matArena;nodeRef=nodeArena;linkRef=linkArena;naviRef=naviArena;
        g.vehObj=VehObj();g.d1=&c;g.distCall=0;nev=0;
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,1,0x42B470,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard),Changes(nodeArena,nodeRef,kNodeGuard),Changes(linkArena,linkRef,kLinkGuard),Changes(naviArena,naviRef,kNaviGuard)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
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
                case 0: CaseReconsider(env,totals,c); break;
                default: CaseFindLinks(env,totals,c); break;
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
