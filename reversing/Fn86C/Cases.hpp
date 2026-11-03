#pragma once
// Caso, falsos y ejecucion del banco Fn86C (continua el espacio de nombres de Core.hpp).
#include "Core.hpp"

namespace Fn86CLive {
// ================================================================================================ arenas y estado de los falsos
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};
alignas(16) inline std::array<std::array<u8,kNodesArena>,kMaxSlots> nodeArena{},nodeRef{};
alignas(16) inline std::array<std::array<u8,kLinksArena>,kMaxSlots> linkArena{},linkRef{};
alignas(16) inline std::array<std::array<u8,kInterArena>,kMaxSlots> interArena{},interRef{};
inline u8* ThisObj() {return thisArena.data()+kThisOff;}
template<class T> inline T& At(void* base,size_t off) {return *reinterpret_cast<T*>(static_cast<u8*>(base)+off);}
// Arena pequena (guarda | objeto | guarda) para parametros de salida
struct Small {
    alignas(16) std::array<u8,kSmallArena> mem{},ref{};
    u8* obj() {return mem.data()+kSmallObj;}
    u8* robj() {return ref.data()+kSmallObj;}
    void Seed(u32 s) {FillWords(ref.data(),kSmallArena,s);}
    void Reset() {mem=ref;}
    // cambios ignorando los primeros `skip` bytes del objeto (campos que el caso observa aparte)
    json ChangesExcept(size_t skip) const {auto tmp=mem;memcpy(tmp.data()+kSmallObj,ref.data()+kSmallObj,skip);return Runs(tmp.data(),ref.data(),kSmallArena,kSmallObj);}
};
inline Small coordsA,oriA,addr1A,addr2A,matA;
inline u32 PtrOf(const void* p) {return u32(uintptr_t(p));}
inline constexpr uintptr_t kTheCamera=0xB6F028;

struct FakeState {
    const P* p{};
    u32 randIdx{},groundIdx{},camIdx{};
};
inline FakeState g;

// Mundo: imagen de referencia de `this` y de las arenas de nodos/enlaces/intersecciones (las arenas vivas se restauran desde ella antes de cada lado)
inline void LoadWorld(const World& w,u32 salt) {
    FillWords(thisRef.data(),kThisArena,Rand(salt,9001));
    u8* t=thisRef.data()+kThisOff;
    std::memset(t+kOffPathNodes,0,kNumAreas*4);
    std::memset(t+kOffNodeLinks,0,kNumAreas*4);
    std::memset(t+kOffIntersections,0,kNumAreas*4);
    for(u32 s=0;s<kMaxSlots;++s) {
        FillWords(nodeRef[s].data(),kNodesArena,Rand(salt,9100+s));
        FillWords(linkRef[s].data(),kLinksArena,Rand(salt,9200+s));
        FillWords(interRef[s].data(),kInterArena,Rand(salt,9300+s));
        if(s>=w.nAreas)continue;
        memcpy(nodeRef[s].data()+kNodesOff,w.nodes[s].data(),w.nodes[s].size()*4);
        memcpy(linkRef[s].data()+kLinksOff,w.links[s].data(),w.links[s].size()*4);
        memcpy(interRef[s].data()+kInterOff,w.inter[s].data(),w.inter[s].size()*4);
        const u32 a=w.area[s];
        At<u32>(t,kOffPathNodes+4*a)=PtrOf(nodeArena[s].data()+kNodesOff);
        At<u32>(t,kOffNodeLinks+4*a)=PtrOf(linkArena[s].data()+kLinksOff);
        At<u32>(t,kOffIntersections+4*a)=PtrOf(interArena[s].data()+kInterOff);
        At<u32>(t,kOffNumNodes+4*a)=w.Total(s);At<u32>(t,kOffNumVeh+4*a)=w.nVeh[s];At<u32>(t,kOffNumPed+4*a)=w.nPed[s];
    }
}
inline void ResetWorld() {thisArena=thisRef;nodeArena=nodeRef;linkArena=linkRef;interArena=interRef;}
inline json WorldChanges(const World& w) {
    json o;
    o["this"]=Runs(thisArena.data(),thisRef.data(),kThisArena,kThisOff);
    json nodes=json::array(),links=json::array(),inter=json::array();
    for(u32 s=0;s<w.nAreas;++s) {
        nodes.push_back(Runs(nodeArena[s].data(),nodeRef[s].data(),kNodesArena,kNodesOff));
        links.push_back(Runs(linkArena[s].data(),linkRef[s].data(),kLinksArena,kLinksOff));
        inter.push_back(Runs(interArena[s].data(),interRef[s].data(),kInterArena,kInterOff));
    }
    o["nodes"]=nodes;o["links"]=links;o["inter"]=inter;
    return o;
}

// Cierre comun de un lado: filas de ABI, globales y estado x87
inline json SideRow(Env& env,const Side& side,json obs) {
    auto fp=NormalizeFp(side.fp);
    if(kCompareVolatileRegisters){obs["eax"]=side.reg.eax;obs["ecx"]=side.reg.ecx;obs["edx"]=side.reg.edx;}
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    out["globals_restored"]=env.WatchesIntact()&&env.GlobalsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ falsos
// 6: rand() del CRT del EXE (cdecl, sin argumentos)
inline int __cdecl FakeRand() {
    Ev& e=PushEv(6);
    e.v[0]=g.randIdx;
    CommitEv(e);
    return int(ScriptRand(g.p->randMode,g.p->randSeed,g.randIdx++));
}
// 7: CWorld::FindGroundZFor3DCoord 0x5696C0 (cdecl: x, y, z, bool*, CEntity**; devuelve float en ST0)
inline float __cdecl FakeGround(u32 x,u32 y,u32 z,u8* found,void* ent) {
    Ev& e=PushEv(7);
    const char local=0;
    e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=NearStack(found,&local);e.v[4]=ent!=nullptr;e.v[5]=g.groundIdx;
    CommitEv(e);
    const u32 k=g.groundIdx++;
    *found=k<kGroundN?u8(g.p->groundFound[k]):u8(0);
    return Fadd(FloatOfBits(z),FloatOfBits(k<kGroundN?g.p->groundDelta[k]:0u));
}
// 8: CCamera::IsSphereVisible(origen, radio, RwMatrix*) 0x420C40 (thiscall, `ret 0xC`); 9: IsSphereVisible(origen, radio) 0x420D40 (thiscall, `ret 8`). Devuelven AL (resto de EAX basura).
inline u32 __fastcall FakeSphereMatrix(void* self,void*,const u32* origin,u32 radius,void* matrix) {
    Ev& e=PushEv(8);
    e.v[0]=origin[0];e.v[1]=origin[1];e.v[2]=origin[2];e.v[3]=radius;e.v[4]=matrix==matA.obj();e.v[5]=uintptr_t(self)==kTheCamera;e.v[6]=g.camIdx;
    CommitEv(e);
    return CamResult(g.p->camMode,g.p->camSeed,g.camIdx++);
}
inline u32 __fastcall FakeSphereMirror(void* self,void*,const u32* origin,u32 radius) {
    Ev& e=PushEv(9);
    e.v[0]=origin[0];e.v[1]=origin[1];e.v[2]=origin[2];e.v[3]=radius;e.v[4]=uintptr_t(self)==kTheCamera;e.v[5]=g.camIdx;
    CommitEv(e);
    return CamResult(g.p->camMode,g.p->camSeed,g.camIdx++);
}

// ================================================================================================ 0: CPathFind::GeneratePedCreationCoors
inline void CaseGenerate(Env& env,Totals& t,unsigned n) {
    const P c=MakeP(n);
    LoadWorld(c.w,n);
    coordsA.Seed(Rand(n,200));oriA.Seed(Rand(n,201));addr1A.Seed(Rand(n,202));addr2A.Seed(Rand(n,203));matA.Seed(Rand(n,204));
    for(u32 i=0;i<3;++i)At<u32>(coordsA.robj(),4*i)=c.outCoordsInit[i];
    At<u32>(oriA.robj(),0)=c.outOriInit;At<u32>(addr1A.robj(),0)=c.outAddrInit[0];At<u32>(addr2A.robj(),0)=c.outAddrInit[1];
    g=FakeState{};g.p=&c;
    const u32 args[12]{c.x,c.y,c.minD1,c.maxD1,c.minD2,c.maxD2,PtrOf(coordsA.obj()),PtrOf(addr1A.obj()),PtrOf(addr2A.obj()),PtrOf(oriA.obj()),c.lowTraffic,c.matrixMode?PtrOf(matA.obj()):0u};
    const json input=json::parse(JsonP(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetWorld();coordsA.Reset();oriA.Reset();addr1A.Reset();addr2A.Reset();matA.Reset();ResetEvs();g.randIdx=0;g.groundIdx=0;g.camIdx=0;
        sides[s]=Execute(env,0,0x44E790,candidate,[&](uintptr_t entry){return Gate(entry,args,12,c.eax,PtrOf(ThisObj()),c.edx,c.stack,0);});
        json obs;
        obs["events"]=EventsJson([](const Ev& e){
            switch(e.kind){
            case 6: return json::array({"rand",e.v[0]});
            case 7: return json::array({"ground",e.v[0],e.v[1],e.v[2],e.v[3],e.v[4],e.v[5]});
            case 8: return json::array({"cam_m",e.v[0],e.v[1],e.v[2],e.v[3],e.v[4],e.v[5],e.v[6]});
            case 9: return json::array({"cam_s",e.v[0],e.v[1],e.v[2],e.v[3],e.v[4],e.v[5]});
            default: return json::array({"unexpected",e.kind});
            }});
        obs["nevents"]=nev;obs["ev_hash"]=evHash;obs["rand_calls"]=g.randIdx;obs["ground_calls"]=g.groundIdx;obs["cam_calls"]=g.camIdx;
        obs["ret_al"]=sides[s].reg.eax&0xFFu;
        obs["coords"]=json::array({BitsOf(coordsA.obj()),BitsOf(coordsA.obj()+4),BitsOf(coordsA.obj()+8)});
        obs["coords_other"]=coordsA.ChangesExcept(12);
        obs["ori"]=BitsOf(oriA.obj());obs["ori_other"]=oriA.ChangesExcept(4);
        obs["addr"]=json::array({BitsOf(addr1A.obj()),BitsOf(addr2A.obj())});
        obs["addr_other"]=json::array({addr1A.ChangesExcept(4),addr2A.ChangesExcept(4)});
        obs["matrix_changes"]=matA.ChangesExcept(0);
        obs["world"]=WorldChanges(c.w);
        rows[s]=SideRow(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFns;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
        for(const auto& s:env.stubs[f]) {
            if(s.pair.hooked)ready&=s.pair.dll!=0&&ModuleOf(s.pair.dll)==env.asi;
            else ready&=s.pair.dll==0||ModuleOf(s.pair.dll)==env.asi;
        }
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    json globalsJ=json::array();for(const auto& w:env.globals)globalsJ.push_back({{"address",w.address},{"bytes",w.saved.size()}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"watched_globals",globalsJ},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned f=0;f<kFns;++f) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)CaseGenerate(env,totals,c);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&env.GlobalsIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    const bool globalsIntact=env.GlobalsIntact();
    bool stateRestored=entriesIntact&&globalsIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"globals_intact",globalsIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
