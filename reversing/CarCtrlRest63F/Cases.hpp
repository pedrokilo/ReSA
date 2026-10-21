#pragma once
// Casos del banco CarCtrlRest63F (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y la funcion de caso de IsThisAnAppropriateNode.
#include "Core.hpp"
#include <utility>

namespace Cc63fLive {
struct FakeState {
    uintptr_t vehObj{};
    const F0* c{};
};
inline FakeState g;
alignas(16) inline std::array<uint8,kVehArena> vehArena{},vehRef{};
alignas(16) inline std::array<uint8,kMatArena> matArena{},matRef{};
alignas(16) inline std::array<uint8,kNodeArena> nodeArena{},nodeRef{};
alignas(16) inline std::array<uint8,kDriverBytes> driverArena{};
alignas(16) inline std::array<uint8,kInfoBytes> infoArena{};
alignas(16) inline std::array<uint8,kColBytes> colArena{};

inline uintptr_t VehObj() {return uintptr_t(vehArena.data())+kGuard;}
inline uintptr_t MatObj() {return uintptr_t(matArena.data())+kMatGuard;}
inline void Put32(std::array<uint8,kVehArena>& a,size_t off,u32 v) {memcpy(a.data()+kGuard+off,&v,4);}
inline void Put16(std::array<uint8,kVehArena>& a,size_t off,u32 v) {const u16 w=u16(v);memcpy(a.data()+kGuard+off,&w,2);}
inline void Put8(std::array<uint8,kVehArena>& a,size_t off,u32 v) {a[kGuard+off]=uint8(v);}
inline void ResetVehicle(u32 seed) {FillWords(vehArena.data(),kVehArena,seed*3u+11u);}
template<size_t N> inline json Changes(const std::array<uint8,N>& mem,const std::array<uint8,N>& ref,size_t objOff) {return ArenaChanges(mem.data(),ref.data(),N,objOff);}
inline u32 Rel(const void* p) {return u32(uintptr_t(p)-g.vehObj);}
inline u32 PtrTag(u32 p) {return p==0?0u:(p>=u32(g.vehObj)&&p<u32(g.vehObj)+kVehWindow)?(0x80000000u|(p-u32(g.vehObj))):1u;}

inline json CloseSide(Env& env,Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    RestoreRegions();
    out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ 0: IsThisAnAppropriateNode
inline u32 __fastcall FakeLaw(void* self,void*) {Ev& e=PushEv(110,1);e.v[0]=Rel(self);return g.c->lawRet;}
inline u32 __cdecl FakeParking() {Ev& e=PushEv(111,1);e.v[0]=0;return g.c->parkRet;}
inline void __cdecl FakeFindObjects(const u32* point,u32 radius,u32 b2D,u16* outCount,u32 maxCount,u32 outEntities,u32 buildings,u32 vehicles,u32 peds,u32 objects,u32 dummies) {
    Ev& e=PushEv(112,13);
    e.v[0]=point[0];e.v[1]=point[1];e.v[2]=point[2];e.v[3]=radius;e.v[4]=b2D&0xFFu;e.v[5]=PtrTag(u32(uintptr_t(outCount)))?1u:0u;e.v[6]=maxCount&0xFFFFu;e.v[7]=outEntities;
    e.v[8]=buildings&0xFFu;e.v[9]=vehicles&0xFFu;e.v[10]=peds&0xFFu;e.v[11]=objects&0xFFu;e.v[12]=dummies&0xFFu;
    *outCount=u16(g.c->count);
}
inline void CaseAppropriate(Env& env,Totals& t,unsigned n) {
    const F0 c=MakeF0(n);
    const json input=json::parse(JsonF0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);
        // nodos de ruta sinteticos
        FillWords(nodeArena.data(),kNodeArena,n*13u+1u);
        const uintptr_t nodeBase=uintptr_t(nodeArena.data())+kNodeGuard;
        for(u32 i=0;i<kPoolAreas;++i)SetPtr(kRPathNodes,i,0);
        for(u32 sl=0;sl<kNodeAreas;++sl)SetPtr(kRPathNodes,c.ns.areaIds[sl],nodeBase+sl*kNodesPerArea*kNodeBytes);
        for(u32 i=0;i<kNodeAreas*kNodesPerArea;++i) {
            uint8* node=nodeArena.data()+kNodeGuard+i*kNodeBytes;
            memcpy(node+8,&c.ns.x[i],2);memcpy(node+0xA,&c.ns.y[i],2);memcpy(node+0xC,&c.ns.z[i],2);
            node[0x18]=c.ns.b18[i];node[0x19]=c.ns.b19[i];node[0x1A]=c.ns.b1A[i];
        }
        auto addr=[&](const u32* a){return (a[0]==99u?c.ns.unloadedId:c.ns.areaIds[a[0]%kNodeAreas])|(a[1]<<16);};
        const u32 n1=addr(c.n1),n2=addr(c.n2),n3=addr(c.n3);
        // vehiculo: matriz valida (derecha en +0, posicion en +0x30), modelo, creador, mision, conductor
        FillWords(matArena.data(),kMatArena,n*7u+5u);
        for(u32 i=0;i<3;++i){memcpy(matArena.data()+kMatGuard+4*i,&c.right[i],4);memcpy(matArena.data()+kMatGuard+0x30+4*i,&c.pos[i],4);}
        Put32(vehArena,0x14,u32(MatObj()));Put32(vehArena,4,c.decoy[0]);Put32(vehArena,8,c.decoy[1]);
        Put16(vehArena,0x22,c.model);Put8(vehArena,0x4A4,c.createdBy);Put8(vehArena,0x3BA,c.mission);
        FillWords(driverArena.data(),kDriverBytes,n*3u+9u);
        driverArena[0x484]=uint8(c.driver484);memcpy(driverArena.data()+0x598,&c.driver598,4);
        Put32(vehArena,0x460,c.hasDriver?u32(uintptr_t(driverArena.data())):0u);
        // informacion del modelo falsa
        FillWords(infoArena.data(),kInfoBytes,n*5u+7u);FillWords(colArena.data(),kColBytes,n*11u+3u);
        infoArena[0x4D]=uint8(c.info4D);
        const u32 colPtr=u32(uintptr_t(colArena.data()));memcpy(infoArena.data()+0x14,&colPtr,4);
        memcpy(colArena.data()+0x0C,&c.col[0],4);memcpy(colArena.data()+0x10,&c.col[1],4);memcpy(colArena.data()+0x14,&c.col[2],4);
        const u32 infoPtr=u32(uintptr_t(infoArena.data()));memcpy(Mem(kRModelInfos)+size_t(c.model)*4,&infoPtr,4);
        vehRef=vehArena;matRef=matArena;nodeRef=nodeArena;
        g.vehObj=VehObj();g.c=&c;nev=0;
        const u32 args[5]{u32(VehObj()),n1,n2,n3,c.arg5};
        sides[s]=Execute(env,0,0x42DAB0,candidate,[&](uintptr_t entry){return Gate(entry,args,5,c.eax,c.ecx,c.edx,c.stack,20);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard),Changes(nodeArena,nodeRef,kNodeGuard)});obs["events"]=EvsJson();
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
            for(unsigned c=0;c<kCases;++c)CaseAppropriate(env,totals,c);
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
