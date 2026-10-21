#pragma once
// Casos del banco CarCtrlRest63B (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y una funcion de caso por funcion.
#include "Core.hpp"
#include <utility>

namespace Cc63bLive {
// ================================================================================================ estado de los falsos y arenas
struct FakeState {
    uintptr_t vehObj{}, matObj{}, infoObj{};
    const C0* c0{}; const C1* c1{}; const CS* cs{};
    u32 randIdx{}, listCall{};
};
inline FakeState g;
alignas(16) inline std::array<uint8,kVehArena> vehArena{},vehRef{};
alignas(16) inline std::array<uint8,kMatArena> matArena{},matRef{};
alignas(16) inline std::array<uint8,kFlagsArena> flagsArena{},flagsRef{};
alignas(16) inline std::array<u32,4> poolObj{};
alignas(16) inline std::array<u32,4> infoObject{};           // objeto de CBaseModelInfo falso: vptr + 3 palabras
alignas(16) inline std::array<void*,kVtableSlots> infoVtable{};
alignas(16) inline std::array<void*,kVtableSlots> vehVtable{};

inline uintptr_t VehObj() {return uintptr_t(vehArena.data())+kGuard;}
inline uintptr_t MatObj() {return uintptr_t(matArena.data())+kMatGuard;}
inline void Put32(std::array<uint8,kVehArena>& a,size_t off,u32 v) {memcpy(a.data()+kGuard+off,&v,4);}
inline void Put16(std::array<uint8,kVehArena>& a,size_t off,u32 v) {const u16 w=u16(v);memcpy(a.data()+kGuard+off,&w,2);}
inline void Put8(std::array<uint8,kVehArena>& a,size_t off,u32 v) {a[kGuard+off]=uint8(v);}
inline void ResetVehicle(u32 seed) {FillWords(vehArena.data(),kVehArena,seed*3u+11u);}
template<size_t N> inline json Changes(const std::array<uint8,N>& mem,const std::array<uint8,N>& ref,size_t objOff) {return ArenaChanges(mem.data(),ref.data(),N,objOff);}
inline u32 Rel(const void* p) {return u32(uintptr_t(p)-g.vehObj);}

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

// ================================================================================================ 0: SetUpDriverAndPassengersForVehicle
inline u32 __fastcall FakeSetUpDriver(void* self,void*,u32 a,u32 b,u32 c) {
    Ev& e=PushEv(1,4);e.v[0]=Rel(self);e.v[1]=a;e.v[2]=b&0xFFu;e.v[3]=c&0xFFu;
    return g.c0->drvRet;
}
inline u32 __cdecl FakeRand() {
    const u32 idx=g.randIdx++;
    const u32 value=g.c0->rnd[idx%kRandSeq];
    Ev& e=PushEv(7,2);e.v[0]=idx;e.v[1]=value;
    return value;
}
inline void* __fastcall FakeGive(void* self,void*,u32 model,u32 replace) {
    Ev& e=PushEv(2,3);e.v[0]=u32(uintptr_t(self));e.v[1]=model;e.v[2]=replace&0xFFu;
    return nullptr;
}
inline u32 __cdecl FakeIsCar(u32 model) {
    Ev& e=PushEv(3,1);e.v[0]=model;
    return g.c0->isCarRet;
}
inline u32 __cdecl FakeAnimBlock(const char* name) {
    Ev& e=PushEv(4,2);
    u32 w[2]{};
    for(unsigned i=0;i<7&&name[i];++i)reinterpret_cast<char*>(w)[i]=name[i];   // solo hasta el NUL (el texto posterior no se conoce)
    e.v[0]=w[0];e.v[1]=w[1];
    return g.c0->animBlock;
}
inline void* __fastcall FakeSetupPassenger(void* self,void*,u32 seat,u32 pedType,u32 b4,u32 b5) {
    Ev& e=PushEv(5,5);e.v[0]=Rel(self);e.v[1]=seat;e.v[2]=pedType;e.v[3]=b4&0xFFu;e.v[4]=b5&0xFFu;
    return reinterpret_cast<void*>(uintptr_t(seat<kPassTokens?g.c0->passTok[seat]:0u));
}
inline void __fastcall FakeUpdateStat(void* self,void*) {Ev& e=PushEv(6,1);e.v[0]=u32(uintptr_t(self));}
template<unsigned S> inline u32 __fastcall InfoSlot(void* self,void*) {
    Ev& e=PushEv(8,2);e.v[0]=S;e.v[1]=self==reinterpret_cast<void*>(g.infoObj)?1u:0u;
    return g.c0->animFile;
}
template<size_t... I> inline void FillInfoVtable(std::index_sequence<I...>) {((infoVtable[I]=reinterpret_cast<void*>(&InfoSlot<unsigned(I)>)),...);}
inline void CaseSetUpDriver(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);
        Put16(vehArena,0x22,c.model);Put32(vehArena,0x460,c.driver);Put8(vehArena,0x488,c.maxPass);
        FillInfoVtable(std::make_index_sequence<kVtableSlots>{});
        infoObject={u32(uintptr_t(infoVtable.data())),0x11111111u,0x22222222u,0x33333333u};
        g.infoObj=uintptr_t(infoObject.data());
        if(c.model<kModelInfoSlots)memcpy(Mem(kRModelInfos)+size_t(c.model)*4,&g.infoObj,4);
        vehRef=vehArena;
        g.vehObj=VehObj();g.c0=&c;g.randIdx=0;nev=0;
        const u32 args[6]{u32(VehObj()),c.pedType,c.minPass,c.b4,c.b5,c.passNum};
        sides[s]=Execute(env,0,0x4217C0,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,c.ecx,c.edx,c.stack,24);});
        json obs;
        obs["changes"]=Changes(vehArena,vehRef,kGuard);obs["events"]=EvsJson();obs["rand_calls"]=g.randIdx;
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: SetCoordsOfScriptCar
inline void __fastcall FakeSetIsStatic(void* self,void*,u32 v) {Ev& e=PushEv(20,2);e.v[0]=Rel(self);e.v[1]=v&0xFFu;}
inline void __fastcall FakeTeleport(void* self,void*,u32 x,u32 y,u32 z,u32 b) {Ev& e=PushEv(21,6);e.v[0]=Rel(self);e.v[1]=x;e.v[2]=y;e.v[3]=z;e.v[4]=b&0xFFu;}
template<unsigned S> inline void __fastcall VehSlot(void* self,void*) {Ev& e=PushEv(22,2);e.v[0]=S;e.v[1]=Rel(self);}
template<size_t... I> inline void FillVehVtable(std::index_sequence<I...>) {((vehVtable[I]=reinterpret_cast<void*>(&VehSlot<unsigned(I)>)),...);}
inline float __cdecl FakeGround(u32 x,u32 y) {Ev& e=PushEv(23,2);e.v[0]=x;e.v[1]=y;return FloatOfBits(g.c1->groundRet);}
inline float __fastcall FakeDist(void* self,void*) {Ev& e=PushEv(24,1);e.v[0]=Rel(self);return FloatOfBits(g.c1->distRet);}
inline void __fastcall FakeStuck(void* self,void*,u32 handle) {Ev& e=PushEv(25,2);e.v[0]=self==reinterpret_cast<void*>(0xA90AB0)?1u:0u;e.v[1]=handle;}
inline void __cdecl FakeClearSpace(const u32* vec,void* entity) {Ev& e=PushEv(26,4);e.v[0]=vec[0];e.v[1]=vec[1];e.v[2]=vec[2];e.v[3]=Rel(entity);}
inline void __fastcall FakePlaceAuto(void* self,void*) {Ev& e=PushEv(27,1);e.v[0]=Rel(self);}
inline void __fastcall FakePlaceBike(void* self,void*) {Ev& e=PushEv(28,1);e.v[0]=Rel(self);}
inline void __cdecl FakeJoin(void* veh) {Ev& e=PushEv(29,1);e.v[0]=Rel(veh);}
inline void CaseSetCoords(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);FillWords(flagsArena.data(),kFlagsArena,c.flagsSeed);
        FillVehVtable(std::make_index_sequence<kVtableSlots>{});
        vehVtable[4]=reinterpret_cast<void*>(&FakeSetIsStatic);vehVtable[14]=reinterpret_cast<void*>(&FakeTeleport);
        Put32(vehArena,0,u32(uintptr_t(vehVtable.data())));Put32(vehArena,0x590,c.vtype);Put8(vehArena,0x3BB,c.tempAction);
        // pool falso: indice = (vehiculo - almacenamiento) / 0xA18 truncado hacia cero; el delta lleva el signo del indice
        const int32 k=int32(c.poolIndex);
        const int32 delta=k>0?k*0xA18+int32(c.poolRem):k<0?k*0xA18-int32(c.poolRem):int32(c.poolRem)*(Rand(n,55)%2?1:-1);
        poolObj={u32(VehObj())-u32(delta),u32(uintptr_t(flagsArena.data())+kFlagsBefore),0,0};
        const u32 poolPtr=u32(uintptr_t(poolObj.data()));
        memcpy(Mem(kRVehiclePool),&poolPtr,4);
        vehRef=vehArena;flagsRef=flagsArena;
        g.vehObj=VehObj();g.c1=&c;nev=0;
        const u32 args[6]{u32(VehObj()),c.pos[0],c.pos[1],c.pos[2],c.arg5,c.arg6};
        sides[s]=Execute(env,1,0x4342A0,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,c.ecx,c.edx,c.stack,24);});
        json obs;
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(flagsArena,flagsRef,kFlagsBefore)});obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2 y 3: ScanForPedDanger / FindMaximumSpeedForThisCarInTraffic
inline void* __cdecl FakeFindPlayer(u32 playerId,u32 includeRemote) {
    Ev& e=PushEv(30,2);e.v[0]=playerId;e.v[1]=includeRemote&0xFFu;
    return g.cs->playerMode==1?reinterpret_cast<void*>(g.vehObj):g.cs->playerMode==2?nullptr:reinterpret_cast<void*>(g.vehObj+0x10);
}
inline void __cdecl FakeClearScan() {Ev& e=PushEv(31,1);e.v[0]=*reinterpret_cast<const u16*>(0xB7CD78);}
template<unsigned T> inline void __cdecl FakeList(u32 list,void* veh,u32 x1,u32 y1,u32 x2,u32 y2,u32* speed,u32 arg8) {
    const u32 call=g.listCall++;
    Ev& e=PushEv(40+T,9);e.v[0]=list-0xB992B8u;e.v[1]=Rel(veh);e.v[2]=x1;e.v[3]=y1;e.v[4]=x2;e.v[5]=y2;e.v[6]=*speed;e.v[7]=arg8;e.v[8]=call;
    const u32 w=g.cs->speedWrites[call%kSpeedWrites];
    if(w)*speed=w;
    if(g.cs->mutateAt==call) {
        *reinterpret_cast<uint8*>(g.vehObj+0x3D0)=uint8(g.cs->mutCruise);*reinterpret_cast<u32*>(g.vehObj+0x3D4)=g.cs->mutMult;
    }
}
inline void SetupScanVehicle(const CS& c,u32 n) {
    ResetVehicle(n);
    PlacePosition(c.matMode,c.pos,c.decoy);
    Put8(vehArena,0x3B9,c.style);Put8(vehArena,0x3D0,c.cruise);Put32(vehArena,0x3D4,c.mult);Put8(vehArena,0x3DB,c.honkByte);Put8(vehArena,0x429,c.flags429);
    const u16 sc=u16(c.scanCode);memcpy(Mem(kRScanCode),&sc,2);
    vehRef=vehArena;matRef=matArena;
    g.vehObj=VehObj();g.matObj=MatObj();g.cs=&c;g.listCall=0;nev=0;
}
inline void CaseScanPed(Env& env,Totals& t,unsigned n) {
    const CS c=MakeCS(n,false);
    const json input=json::parse(JsonCS(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetupScanVehicle(c,n);
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,2,0x42CE40,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard)});obs["events"]=EvsJson();obs["scan_code"]=*reinterpret_cast<const u16*>(0xB7CD78);
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[2],n,input,rows,sides);
}
inline void CaseFindMax(Env& env,Totals& t,unsigned n) {
    const CS c=MakeCS(n,true);
    const json input=json::parse(JsonCS(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetupScanVehicle(c,n);
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,3,0x434400,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["st0"]=HexBytes(sides[s].fp.data()+28,10);   // ST(0) de la imagen de FNSAVE: el resultado float de la funcion
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard)});obs["events"]=EvsJson();obs["scan_code"]=*reinterpret_cast<const u16*>(0xB7CD78);
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
                case 0: CaseSetUpDriver(env,totals,c); break;
                case 1: CaseSetCoords(env,totals,c); break;
                case 2: CaseScanPed(env,totals,c); break;
                default: CaseFindMax(env,totals,c); break;
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
