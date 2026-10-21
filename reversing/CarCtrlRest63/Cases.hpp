#pragma once
// Casos del banco CarCtrlRest63 (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y una funcion de caso por funcion.
#include "Core.hpp"

namespace Cc63Live {
// ================================================================================================ estado de los falsos y arenas
struct FakeState {
    // 4: CPathNode::GetPosition
    u32 calls{}, perturbed{};
    uintptr_t nodeBase{};
    // 6: colaboradores de SwitchBetweenPhysicsAndGhost
    u32 loadedRet{};
    uintptr_t vehObj{}, matObj{};
};
inline FakeState g;
alignas(16) inline std::array<uint8,kVehArena> vehArena{},vehRef{};
alignas(16) inline std::array<uint8,kMatArena> matArena{},matRef{};
alignas(16) inline std::array<uint8,kOutArena> outArena{},outRef{};
alignas(16) inline std::array<uint8,kLinkArena> linkArena{},linkRef{};
alignas(16) inline std::array<uint8,kNodeArena> nodeArena{},nodeRef{};
alignas(16) inline std::array<uint8,kFlagArena> flagArena{},flagRef{};

inline uintptr_t VehObj() {return uintptr_t(vehArena.data())+kGuard;}
inline uintptr_t MatObj() {return uintptr_t(matArena.data())+kMatGuard;}
inline void Put32(std::array<uint8,kVehArena>& a,size_t off,u32 v) {memcpy(a.data()+kGuard+off,&v,4);}
inline void Put16(std::array<uint8,kVehArena>& a,size_t off,u32 v) {const u16 w=u16(v);memcpy(a.data()+kGuard+off,&w,2);}
inline void Put8(std::array<uint8,kVehArena>& a,size_t off,u32 v) {a[kGuard+off]=uint8(v);}
inline void ResetVehicle(u32 seed) {FillWords(vehArena.data(),kVehArena,seed*3u+11u);}
// Cierra un caso: observados comunes + restauracion
template<size_t N> inline json Changes(const std::array<uint8,N>& mem,const std::array<uint8,N>& ref,size_t objOff) {return ArenaChanges(mem.data(),ref.data(),N,objOff);}

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

// ================================================================================================ 0: ThisVehicleShouldTryNotToTurn
inline void CaseNoTurn(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);Put16(vehArena,0x22,c.model);
        vehRef=vehArena;
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,0,0x421FE0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;obs["changes"]=Changes(vehArena,vehRef,kGuard);
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: FindIntersection2Lines (los llamadores del EXE leen EDX tras la llamada: se compara)
inline void CaseIntersection(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(outArena.data(),kOutArena,n*5u+3u);
        memcpy(outArena.data()+kOutSlot1,&c.stale[0],4);memcpy(outArena.data()+kOutSlot2,&c.stale[1],4);
        outRef=outArena;
        const u32 out1=u32(uintptr_t(outArena.data())+kOutSlot1);
        const u32 out2=c.outMode==0?u32(uintptr_t(outArena.data())+kOutSlot2):c.outMode==1?out1:out1+4;
        const u32 args[10]{c.a[0],c.a[1],c.a[2],c.a[3],c.a[4],c.a[5],c.a[6],c.a[7],out1,out2};
        sides[s]=Execute(env,1,0x4226F0,candidate,[&](uintptr_t entry){return Gate(entry,args,10,c.eax,c.ecx,c.edx,c.stack,40);});
        json obs;
        obs["edx"]=sides[s].reg.edx;   // el llamador 0x4287C6 (DealWithBend_Racing) lee [edx] tras la llamada
        obs["out"]=json::array({BitsOf(outArena.data()+kOutSlot1),BitsOf(outArena.data()+kOutSlot2),BitsOf(outArena.data()+kOutSlot1+4)});
        obs["changes"]=Changes(outArena,outRef,kOutSlot1);
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2: FindSpeedMultiplier (resultado en ST0: va en post_floating_state)
inline void CaseSpeedMultiplier(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    const json input=json::parse(JsonC2(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        const u32 args[4]{c.a[0],c.a[1],c.a[2],c.a[3]};
        sides[s]=Execute(env,2,0x4224E0,candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,c.ecx,c.edx,c.stack,16);});
        json obs;
        obs["st0"]=HexBytes(sides[s].fp.data()+28,10);   // ST(0) de la imagen de FNSAVE (orden de pila): el valor que la funcion deja como resultado
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ 3: FindPercDependingOnDistToLink
inline void CasePercDist(Env& env,Totals& t,unsigned n) {
    const C3 c=MakeC3(n);
    const json input=json::parse(JsonC3(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);FillWords(matArena.data(),kMatArena,n*7u+5u);FillWords(linkArena.data(),kLinkArena,n*11u+9u);
        // posicion del coche: la que lee la funcion va en +4/+8 (m_matrix nulo) o en matriz+0x30/+0x34; la otra queda como señuelo
        const uintptr_t mat=MatObj();
        if(c.matMode==0) {
            Put32(vehArena,0x14,0);Put32(vehArena,4,c.pos[0]);Put32(vehArena,8,c.pos[1]);
            memcpy(matArena.data()+kMatGuard+0x30,&c.decoy[0],4);memcpy(matArena.data()+kMatGuard+0x34,&c.decoy[1],4);
        } else {
            Put32(vehArena,0x14,u32(mat));Put32(vehArena,4,c.decoy[0]);Put32(vehArena,8,c.decoy[1]);
            memcpy(matArena.data()+kMatGuard+0x30,&c.pos[0],4);memcpy(matArena.data()+kMatGuard+0x34,&c.pos[1],4);
        }
        const u32 slot=Rand(n,50)%4;
        uint8* link=linkArena.data()+kLinkGuard+slot*kLinkBytes;
        const i16 lx=i16(c.lx),ly=i16(c.ly);memcpy(link,&lx,2);memcpy(link+2,&ly,2);
        SetPtr(kRNaviNodes,c.area,uintptr_t(link)-uintptr_t(c.id)*kLinkBytes);
        vehRef=vehArena;matRef=matArena;linkRef=linkArena;
        const u32 addr=(c.area<<10|c.id)|(c.junkHi<<16);
        const u32 args[2]{u32(VehObj()),addr};
        sides[s]=Execute(env,3,0x422620,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,c.ecx,c.edx,c.stack,8);});
        json obs;
        obs["st0"]=HexBytes(sides[s].fp.data()+28,10);
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard),Changes(linkArena,linkRef,kLinkGuard)});
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[3],n,input,rows,sides);
}

// ================================================================================================ 4: FindPathDirection (falso de CPathNode::GetPosition, 0x420A10)
inline float* __fastcall FakeGetPos(void* self,void*,float* out) {
    Ev& e=PushEv(1,3);
    const u32 index=g.calls++;
    e.v[0]=u32(uintptr_t(self)-g.nodeBase);e.v[1]=index;
    const uint8* p=static_cast<const uint8*>(self);
    float v[3];memcpy(v,p,12);
    e.v[2]=BitsOf(&v[2]);
    if(g.perturbed) {   // la posicion depende del orden de la llamada: delata una lectura de una llamada equivocada
        v[0]+=float(index+1)*0.03125f;v[1]+=float(index+1)*0.0625f;
    }
    memcpy(out,v,12);
    return out;
}
inline void CasePathDir(Env& env,Totals& t,unsigned n) {
    const C4 c=MakeC4(n);
    const json input=json::parse(JsonC4(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        FillWords(nodeArena.data(),kNodeArena,n*13u+1u);FillWords(flagArena.data(),kFlagArena,n*17u+2u);
        flagArena[kFlagAt]=uint8(c.flagInit);
        const uintptr_t nodeBase=uintptr_t(nodeArena.data())+kNodeGuard;
        for(u32 i=0;i<kPoolAreas;++i)SetPtr(kRPathNodes,i,0);
        for(u32 sl=0;sl<kNodeAreas;++sl)SetPtr(kRPathNodes,c.areaIds[sl],nodeBase+sl*kNodesPerArea*kNodeBytes);
        for(u32 r=0;r<3;++r) {
            if(c.refSlot[r]>=4)continue;
            uint8* node=nodeArena.data()+kNodeGuard+(c.refSlot[r]*kNodesPerArea+c.ref[r][1])*kNodeBytes;
            memcpy(node,c.pos[r],12);
        }
        nodeRef=nodeArena;flagRef=flagArena;
        g.calls=0;g.perturbed=c.perturbed;g.nodeBase=nodeBase;nev=0;
        const u32 args[4]{c.ref[0][0]|(c.ref[0][1]<<16),c.ref[1][0]|(c.ref[1][1]<<16),c.ref[2][0]|(c.ref[2][1]<<16),u32(uintptr_t(flagArena.data())+kFlagAt)};
        sides[s]=Execute(env,4,0x422090,candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,c.ecx,c.edx,c.stack,16);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;obs["flag"]=flagArena[kFlagAt];
        obs["changes"]=json::array({Changes(flagArena,flagRef,kFlagAt),Changes(nodeArena,nodeRef,kNodeGuard)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[4],n,input,rows,sides);
}

// ================================================================================================ 5: StopCarIfNodesAreInvalid
inline void CaseStopCar(Env& env,Totals& t,unsigned n) {
    const C5 c=MakeC5(n);
    const json input=json::parse(JsonC5(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);
        Put16(vehArena,0x3A4,c.w[0]);Put16(vehArena,0x3A6,c.w[1]);Put16(vehArena,0x390,c.w[2]);Put16(vehArena,0x394,c.w[3]);Put8(vehArena,0x3DC,c.flagByte);
        for(u32 i=0;i<kPoolAreas;++i)SetPtr(kRPathNodes,i,c.loaded[i]?0x1000u+16u*i:0u);
        vehRef=vehArena;
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,5,0x422590,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;obs["changes"]=Changes(vehArena,vehRef,kGuard);
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[5],n,input,rows,sides);
}

// ================================================================================================ 6: SwitchBetweenPhysicsAndGhost
inline u32 __cdecl FakeColLoaded(const void* pos,u32 area) {
    Ev& e=PushEv(10,2);
    const uintptr_t p=uintptr_t(pos);
    e.v[0]=p==g.vehObj+4?1u:p==g.matObj+0x30?2u:0u;e.v[1]=area;
    return g.loadedRet;
}
inline u32 __fastcall FakeSetWheel(void* self,void*,u32 wheel,u32 status) {
    Ev& e=PushEv(11,3);e.v[0]=u32(uintptr_t(self)-g.vehObj);e.v[1]=wheel;e.v[2]=status;
    return 0;
}
inline void __fastcall FakePlaceAuto(void* self,void*) {Ev& e=PushEv(12,1);e.v[0]=u32(uintptr_t(self)-g.vehObj);}
inline void __fastcall FakePlaceBike(void* self,void*) {Ev& e=PushEv(13,1);e.v[0]=u32(uintptr_t(self)-g.vehObj);}
inline void CaseSwitchGhost(Env& env,Totals& t,unsigned n) {
    const C6 c=MakeC6(n);
    const json input=json::parse(JsonC6(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetVehicle(n);FillWords(matArena.data(),kMatArena,n*7u+5u);
        Put32(vehArena,0x40,c.flags40);Put8(vehArena,0x4A4,c.creator);Put32(vehArena,0x594,c.sub594);Put32(vehArena,0x590,c.type590);Put8(vehArena,0x36,c.statusByte);
        Put32(vehArena,0x14,c.matMode?u32(MatObj()):0u);
        vehRef=vehArena;matRef=matArena;
        g.vehObj=VehObj();g.matObj=MatObj();g.loadedRet=c.loadedRet;nev=0;
        const u32 args[1]{u32(VehObj())};
        sides[s]=Execute(env,6,0x4222A0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);});
        json obs;
        obs["changes"]=json::array({Changes(vehArena,vehRef,kGuard),Changes(matArena,matRef,kMatGuard)});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[6],n,input,rows,sides);
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
                case 0: CaseNoTurn(env,totals,c); break;
                case 1: CaseIntersection(env,totals,c); break;
                case 2: CaseSpeedMultiplier(env,totals,c); break;
                case 3: CasePercDist(env,totals,c); break;
                case 4: CasePathDir(env,totals,c); break;
                case 5: CaseStopCar(env,totals,c); break;
                default: CaseSwitchGhost(env,totals,c); break;
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
