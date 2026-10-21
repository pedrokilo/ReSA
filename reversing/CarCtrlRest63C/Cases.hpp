#pragma once
// Casos del banco CarCtrlRest63C (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y una funcion de caso por funcion.
#include "Core.hpp"
#include <utility>

namespace Cc63cLive {
// ================================================================================================ estado de los falsos y arenas
struct FakeState {
    uintptr_t vehObj{}, matObj{}, targetObj{};
    const C0* c0{}; const C1* c1{}; const C2* c2{};
    u32 coordsCall{}, startDword{};
};
inline FakeState g;
alignas(16) inline std::array<uint8,kVehArena> vehArena{},vehRef{};
alignas(16) inline std::array<uint8,kMatArena> matArena{},matRef{};
alignas(16) inline std::array<uint8,kTargetArena> targetArena{};
alignas(16) inline std::array<uint8,kMatArena> targetMat{};
alignas(16) inline std::array<uint8,kNodeArena> nodeArena{},nodeRef{};
alignas(16) inline std::array<uint8,kLinkArena> linkArena{},linkRef{};
alignas(16) inline std::array<uint8,16> posnArena{};

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
// Posicion del vehiculo como la lee la funcion: matriz nula -> +4/+8; matriz -> matriz+0x30/+0x34 (la otra queda como senuelo)
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
// Escribe los nodos y enlaces del juego (arenas) y los punteros de ThePaths (regiones, restauradas tras el caso)
inline void SetupNodes(const NodeSet& ns,u32 seed) {
    FillWords(nodeArena.data(),kNodeArena,seed*13u+1u);FillWords(linkArena.data(),kLinkArena,seed*17u+2u);
    const uintptr_t nodeBase=uintptr_t(nodeArena.data())+kNodeGuard,linkBase=uintptr_t(linkArena.data())+kLinkGuard;
    for(u32 i=0;i<kPoolAreas;++i){SetPtr(kRPathNodes,i,0);SetPtr(kRNodeLinks,i,0);}
    for(u32 sl=0;sl<kNodeAreas;++sl){SetPtr(kRPathNodes,ns.areaIds[sl],nodeBase+sl*kNodesPerArea*kNodeBytes);SetPtr(kRNodeLinks,ns.areaIds[sl],linkBase+sl*kLinksPerArea*4);}
    for(u32 i=0;i<kNodeAreas*kNodesPerArea;++i) {
        uint8* node=nodeArena.data()+kNodeGuard+i*kNodeBytes;
        memcpy(node+8,&ns.x[i],2);memcpy(node+0xA,&ns.y[i],2);memcpy(node+0xC,&ns.z[i],2);memcpy(node+0x10,&ns.base[i],2);
        node[0x18]=ns.b18[i];node[0x19]=ns.b19[i];node[0x1A]=ns.b1A[i];
    }
    for(u32 a=0;a<kNodeAreas;++a)memcpy(linkArena.data()+kLinkGuard+a*kLinksPerArea*4,ns.link[a],kLinksPerArea*4);
}

// ================================================================================================ 0: PickNextNodeAccordingStrategy
inline float* __cdecl FakeFindCoors(float* out,u32 playerId) {
    const u32 call=g.coordsCall++;
    Ev& e=PushEv(60,2);e.v[0]=playerId;e.v[1]=call;
    const u32* c=g.c0->coords[call<3?call:2];
    memcpy(out,c,12);
    return out;
}
inline void* __cdecl FakeFindPlayerVeh(u32 playerId,u32 includeRemote) {
    Ev& e=PushEv(61,2);e.v[0]=playerId;e.v[1]=includeRemote&0xFFu;
    return reinterpret_cast<void*>(uintptr_t(g.c0->playerVeh));
}
inline u32 __cdecl FakeChase(void* veh,u32 x,u32 y,u32 z) {Ev& e=PushEv(62,4);e.v[0]=Rel(veh);e.v[1]=x;e.v[2]=y;e.v[3]=z;return g.c0->retChase;}
inline u32 __cdecl FakeFollow(void* veh) {Ev& e=PushEv(63,1);e.v[0]=Rel(veh);return g.c0->retFollow;}
inline u32 __cdecl FakeRandomly(void* veh) {Ev& e=PushEv(64,1);e.v[0]=Rel(veh);return g.c0->retRandom;}
inline void CasePickStrategy(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);SetupNodes(c.ns,n);
        const u32 next=c.ns.areaIds[c.slot]|(c.node<<16);
        Put32(vehArena,0x394,next);Put8(vehArena,0x3BA,c.mission);Put8(vehArena,0x3D1,c.field41);
        // objetivo (+0x41C): entidad con matriz (posicion en +0x30) o sin ella (posicion en +4)
        FillWords(targetArena.data(),kTargetArena,n*5u+3u);FillWords(targetMat.data(),kMatArena,n*7u+5u);
        {
            auto putT=[&](size_t off,u32 v){memcpy(targetArena.data()+kGuard+off,&v,4);};
            if(c.targetMat) {
                putT(0x14,u32(TargetMatObj()));putT(4,c.targetDecoy[0]);putT(8,c.targetDecoy[1]);putT(0xC,c.targetDecoy[0]);
                for(u32 i=0;i<3;++i)memcpy(targetMat.data()+kMatGuard+0x30+4*i,&c.targetPos[i],4);
            } else {
                putT(0x14,0);for(u32 i=0;i<3;++i)putT(4+4*i,c.targetPos[i]);
                memcpy(targetMat.data()+kMatGuard+0x30,&c.targetDecoy[0],4);memcpy(targetMat.data()+kMatGuard+0x34,&c.targetDecoy[1],4);
            }
            Put32(vehArena,0x41C,u32(TargetObj()));
        }
        vehRef=vehArena;nodeRef=nodeArena;
        g.vehObj=VehObj();g.c0=&c;g.coordsCall=0;nev=0;
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,0,0x432B10,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(nodeArena,nodeRef,kNodeGuard)});obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: JoinCarWithRoadSystem
inline u32* __fastcall FakeFavour(void* self,void*,u32* ret,u32 px,u32 py,u32 pz,u32 type,u32 dx,u32 dy) {
    Ev& e=PushEv(50,7);e.v[0]=self==reinterpret_cast<void*>(0x96F050)?1u:0u;e.v[1]=px;e.v[2]=py;e.v[3]=pz;e.v[4]=type&0xFFu;e.v[5]=dx;e.v[6]=dy;
    *ret=g.startDword;
    return ret;
}
template<u32 K> inline void __cdecl FakeFindLinks(void* veh) {
    Ev& e=PushEv(K,4);e.v[0]=Rel(veh);e.v[1]=R32(uintptr_t(veh)+0x390);e.v[2]=R32(uintptr_t(veh)+0x394);e.v[3]=R32(uintptr_t(veh)+0x398)&0xFFFFu;
}
inline void CaseJoinRoad(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);SetupNodes(c.ns,n);
        FillWords(matArena.data(),kMatArena,0x777u);
        Put32(vehArena,0x14,u32(MatObj()));
        memcpy(matArena.data()+kMatGuard+0x10,&c.fwd[0],4);memcpy(matArena.data()+kMatGuard+0x14,&c.fwd[1],4);
        for(u32 i=0;i<3;++i)memcpy(matArena.data()+kMatGuard+0x30+4*i,&c.pos[i],4);
        const u32 area=c.startSel==5?0xFFFFu:c.startSel==4?c.ns.unloadedId:c.ns.areaIds[c.startSlot];
        g.startDword=area|(c.startNode<<16);
        vehRef=vehArena;matRef=matArena;nodeRef=nodeArena;linkRef=linkArena;
        g.vehObj=VehObj();g.c1=&c;nev=0;
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,1,0x42F5A0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard),Changes(nodeArena,nodeRef,kNodeGuard),Changes(linkArena,linkRef,kLinkGuard)});obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2: JoinCarWithRoadSystemGotoCoors
inline void __fastcall FakeDoPathSearch(void* self,void*,u32 a0,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9,u32 a10,u32 a11,u32 a12,u32 a13,u32 a14,u32 a15,u32 a16,u32 a17,u32 a18) {
    Ev& e=PushEv(70,20);
    e.v[0]=self==reinterpret_cast<void*>(0x96F050)?1u:0u;
    e.v[1]=a0;e.v[2]=a1;e.v[3]=a2;e.v[4]=a3;e.v[5]=a4;e.v[6]=a5;e.v[7]=a6;e.v[8]=a7;e.v[9]=u32(a8-u32(g.vehObj));e.v[10]=u32(a9-u32(g.vehObj));
    e.v[11]=a10;e.v[12]=a11;e.v[13]=a12;e.v[14]=a13;e.v[15]=a14;e.v[16]=a15&0xFFu;e.v[17]=a16;e.v[18]=a17&0xFFu;e.v[19]=a18&0xFFu;
    const u16 count=u16(g.c2->searchCount);
    memcpy(reinterpret_cast<void*>(uintptr_t(a9)),&count,2);
    memcpy(reinterpret_cast<void*>(uintptr_t(a8)),g.c2->nodes,32);
}
inline void __fastcall FakeRemoveBad(void* self,void*,u32 x,u32 y,u32 z,u32 nodes,u32 count) {
    Ev& e=PushEv(71,6);e.v[0]=self==reinterpret_cast<void*>(0x96F050)?1u:0u;e.v[1]=x;e.v[2]=y;e.v[3]=z;e.v[4]=nodes-u32(g.vehObj);e.v[5]=count-u32(g.vehObj);
    if(g.c2->badModify) {
        const u16 bc=u16(g.c2->badCount);
        memcpy(reinterpret_cast<void*>(uintptr_t(count)),&bc,2);memcpy(reinterpret_cast<void*>(uintptr_t(nodes)),&g.c2->badNode0,4);
    }
}
inline void __fastcall FakeRemoveOne(void* self,void*) {
    // como CAutoPilot::RemoveOnePathNode (0x41B950), con la copia acotada a las 8 ranuras del vector
    uint8* ap=static_cast<uint8*>(self);
    i16 count;memcpy(&count,ap+0x88,2);
    Ev& e=PushEv(72,3);e.v[0]=Rel(self);e.v[1]=u32(u16(count));e.v[2]=R32(uintptr_t(ap)+0x68);
    --count;memcpy(ap+0x88,&count,2);
    if(count>0) {
        const i16 lim=count<8?count:i16(7);
        for(i16 i=0;i<lim;++i){u32 v;memcpy(&v,ap+0x6C+4*i,4);memcpy(ap+0x68+4*i,&v,4);}
    }
}
inline void __cdecl FakeJoin(void* veh) {Ev& e=PushEv(74,1);e.v[0]=Rel(veh);}
inline void CaseGotoCoors(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    const json input=json::parse(JsonC2(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);
        PlacePosition(c.matMode,c.pos,c.decoy);
        Put16(vehArena,0x22,c.model);Put8(vehArena,0x3DB,c.flags3DB);
        uintptr_t posnPtr;
        if(c.alias) {
            for(u32 i=0;i<3;++i)Put32(vehArena,0x3EC+4*i,c.posn[i]);
            posnPtr=VehObj()+0x3EC;
        } else {
            FillWords(posnArena.data(),16,n+9u);
            for(u32 i=0;i<3;++i)memcpy(posnArena.data()+4*i,&c.posn[i],4);
            posnPtr=uintptr_t(posnArena.data());
        }
        vehRef=vehArena;matRef=matArena;
        g.vehObj=VehObj();g.c2=&c;nev=0;
        const u32 args[4]{u32(VehObj()),u32(posnPtr),c.arg3,c.arg4};
        sides[s]=Execute(env,2,0x42F870,candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,c.ecx,c.edx,c.stack,16);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard)});obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[2],n,input,rows,sides);
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
                case 0: CasePickStrategy(env,totals,c); break;
                case 1: CaseJoinRoad(env,totals,c); break;
                default: CaseGotoCoors(env,totals,c); break;
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
