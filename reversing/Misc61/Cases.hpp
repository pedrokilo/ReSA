#pragma once
// Casos de IsSpottedEntity, GetLightingFromCollision y ProcessPed, y la ejecucion del banco Misc61 (continua el espacio de nombres de Core.hpp).
#include "Core.hpp"

namespace Misc61Live {
// ================================================================================================ globales de los falsos
struct FakeState {
    u32 getRet{}, cRet{};                       // 1: GetActualScriptThingIndex y 0x493280
    u32 pvlRet{}, cpLight{}, cpSeed{};          // 3: CWorld::ProcessVerticalLine
    u32 animRet{}, limRet{};                    // 2: RunNamedAnim::ProcessPed y LimitRadianAngle
    uintptr_t arenaLo[6]{}, arenaHi[6]{};       // rangos de las arenas del caso en curso (para etiquetar punteros)
    uintptr_t taskObj{}, pedObj{}, primObj{};
};
inline FakeState g;
inline u32 InAny(const void* p) {const auto a=uintptr_t(p);for(int i=0;i<6;++i)if(g.arenaLo[i]&&a>=g.arenaLo[i]&&a<g.arenaHi[i])return 1;return 0;}
inline u32 NearStack(const void* p, const void* local) {const auto d=intptr_t(uintptr_t(p))-intptr_t(uintptr_t(local));return d>-0x4000&&d<0x4000;}
inline json Label(u32 ptr) {
    if(ptr==0)return 0;
    if(ptr==g.taskObj)return "task";
    if(ptr==g.pedObj)return "ped";
    if(ptr==g.primObj)return "primary";
    return "other";
}
template<class T> inline T& At(void* base,size_t off) {return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off);}
template<size_t N> inline void SetArena(std::array<uint8,N>& arena,const std::array<uint8,N>& ref) {memcpy(arena.data(),ref.data(),N);}

// ================================================================================================ 1: CSearchLight::IsSpottedEntity
alignas(16) inline std::array<uint8,kEntityArena> entArena{},entRef{};
alignas(16) inline std::array<uint8,kMatrixArena> matArena{},matRef{};
inline int32 __cdecl FakeScriptIndex(int32 ref,int32 type) {Ev& e=PushEv(1);e.v[0]=u32(ref);e.v[1]=u32(type);return int32(g.getRet);}
inline u32 __cdecl FakeSpotted(const float* pos,int32 idx) {
    Ev& e=PushEv(2);e.v[0]=BitsOf(pos);e.v[1]=BitsOf(pos+1);e.v[2]=BitsOf(pos+2);e.v[3]=u32(idx);e.v[4]=InAny(pos);
    return g.cRet;
}
inline void CaseSpot(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    uint8* ent=entArena.data()+kEntityOff;uint8* mat=matArena.data()+kMatrixOff;
    FillWords(entRef.data(),kEntityArena,Rand(n,50));FillWords(matRef.data(),kMatrixArena,Rand(n,51));
    uint8* refEnt=entRef.data()+kEntityOff;uint8* refMat=matRef.data()+kMatrixOff;
    for(u32 i=0;i<3;++i) {
        At<u32>(refEnt,kEntityPlacementAt+4*i)=c.hasMatrix?c.decoy[i]:c.pos[i];
        At<u32>(refMat,kMatrixPosAt+4*i)=c.hasMatrix?c.pos[i]:c.decoy[i];
    }
    At<uintptr_t>(refEnt,kEntityMatrixAt)=c.hasMatrix?uintptr_t(mat):0;
    g.getRet=c.getRet;g.cRet=c.cRet;
    g.arenaLo[0]=uintptr_t(entArena.data());g.arenaHi[0]=g.arenaLo[0]+kEntityArena;g.arenaLo[1]=uintptr_t(matArena.data());g.arenaHi[1]=g.arenaLo[1]+kMatrixArena;
    const json input=json::parse(JsonC2(c));
    const u32 args[2]{c.index,u32(uintptr_t(ent))};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetArena(entArena,entRef);SetArena(matArena,matRef);nev=0;
        sides[s]=Execute(env,1,0x493900,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,c.ecx,c.edx,c.stack,8);});
        json obs;json ej=json::array();
        for(unsigned i=0;i<nev&&i<evs.size();++i) {
            const Ev& e=evs[i];
            if(e.kind==1)ej.push_back(json::array({"script_index",e.v[0],e.v[1]}));
            else ej.push_back(json::array({"spotted",e.v[0],e.v[1],e.v[2],e.v[3],e.v[4]}));
        }
        obs["events"]=ej;obs["nevents"]=nev;obs["ret_al"]=sides[s].reg.eax&0xFFu;
        obs["entity_changes"]=ArenaChanges(entArena.data(),entRef.data(),kEntityArena,kEntityOff);
        obs["matrix_changes"]=ArenaChanges(matArena.data(),matRef.data(),kMatrixArena,kMatrixOff);
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    memset(g.arenaLo,0,sizeof(g.arenaLo));memset(g.arenaHi,0,sizeof(g.arenaHi));
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 3: CTrainNode::GetLightingFromCollision
alignas(16) inline std::array<uint8,kNodeArena> nodeArena{},nodeRef{};
inline u32 __cdecl FakePvl(const float* origin,float dist,uint8* cp,void** ent,u32 b0,u32 b1,u32 b2,u32 b3,u32 b4,u32 b5,void* poly) {
    Ev& e=PushEv(3);
    e.v[0]=BitsOf(origin);e.v[1]=BitsOf(origin+1);e.v[2]=BitsOf(origin+2);e.v[3]=BitsOf(&dist);
    e.v[4]=b0&0xFFu;e.v[5]=b1&0xFFu;e.v[6]=b2&0xFFu;e.v[7]=b3&0xFFu;e.v[8]=b4&0xFFu;e.v[9]=b5&0xFFu;
    const char local=0;e.v[10]=poly?1u:0u;e.v[11]=NearStack(cp,&local);e.v[12]=NearStack(ent,&local);
    FillWords(cp,0x2C,g.cpSeed);cp[0x25]=uint8(g.cpLight);   // el EXE/DLL solo deben leer el byte +0x25 (m_nLightingB)
    *ent=reinterpret_cast<void*>(uintptr_t(0x5A5A5A00));
    return g.pvlRet;
}
inline void CaseNode(Env& env,Totals& t,unsigned n) {
    const C3 c=MakeC3(n);
    FillWords(nodeRef.data(),kNodeArena,Rand(n,52));
    uint8* refNode=nodeRef.data()+kNodeOff;uint8* node=nodeArena.data()+kNodeOff;
    At<u16>(refNode,0)=u16(c.x);At<u16>(refNode,2)=u16(c.y);At<u16>(refNode,4)=u16(c.z);
    refNode[kNodeLightAt]=uint8(c.cached);refNode[kNodeFoundAt]=uint8(c.found);
    g.pvlRet=c.pvlRet;g.cpLight=c.cpLight;g.cpSeed=c.cpSeed;
    const json input=json::parse(JsonC3(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetArena(nodeArena,nodeRef);nev=0;
        sides[s]=Execute(env,3,0x6F5F80,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(uintptr_t(node)),c.edx,c.stack,0);});
        json obs;json ej=json::array();
        for(unsigned i=0;i<nev&&i<evs.size();++i) {
            const Ev& e=evs[i];json row=json::array({"pvl"});
            for(int k=0;k<13;++k)row.push_back(e.v[k]);
            ej.push_back(row);
        }
        obs["events"]=ej;obs["nevents"]=nev;obs["ret_al"]=sides[s].reg.eax&0xFFu;
        obs["node_changes"]=ArenaChanges(nodeArena.data(),nodeRef.data(),kNodeArena,kNodeOff);
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[3],n,input,rows,sides);
}

// ================================================================================================ 2: CTaskSimpleSlideToCoord::ProcessPed
alignas(16) inline std::array<uint8,kTaskArena> taskArena{},taskRef{};
alignas(16) inline std::array<uint8,kPedArena> pedArena{},pedRef{};
alignas(16) inline std::array<uint8,kMatrixArena> pmatArena{},pmatRef{};
alignas(16) inline std::array<uint8,kIntelArena> intelArena{},intelRef{};
alignas(16) inline std::array<uint8,kPrimArena> primArena{},primRef{};
inline std::array<void*,12> fakeVtbl{};
// colaboradores (todos reciben ped en la pila; thiscall con `ret 4` = __fastcall con un argumento)
inline u32 __fastcall FakeAnim(void* self,void*,void* ped) {Ev& e=PushEv(1);e.v[0]=u32(uintptr_t(self));e.v[1]=u32(uintptr_t(ped));return g.animRet;}
inline u32 __fastcall FakeStand(void* self,void*,void* ped) {
    Ev& e=PushEv(2);
    const uint8* o=static_cast<const uint8*>(self);
    const char local=0;e.v[0]=NearStack(self,&local);e.v[1]=u32(uintptr_t(ped));
    e.v[2]=BitsOf(o+4);e.v[3]=BitsOf(o+8);e.v[4]=BitsOf(o+0xC);e.v[5]=BitsOf(o+0x10);e.v[6]=o[0x14];e.v[7]=o[0x15];e.v[8]=o[0x18];e.v[9]=o[0x19];e.v[10]=BitsOf(o+0x1C);
    return 0;
}
inline u32 __fastcall FakeAbort(void* self,void*,void* ped,int prio,void* event) {Ev& e=PushEv(3);e.v[0]=u32(uintptr_t(self));e.v[1]=u32(uintptr_t(ped));e.v[2]=u32(prio);e.v[3]=u32(uintptr_t(event));return 0;}
template<int N> inline u32 __fastcall FakeTrap(void* self,void*) {Ev& e=PushEv(9);e.v[0]=N;e.v[1]=u32(uintptr_t(self));return 0;}
template<int... N> inline void FillTraps(std::integer_sequence<int,N...>) {((fakeVtbl[N]=reinterpret_cast<void*>(&FakeTrap<N>)),...);}
inline float __cdecl FakeLimit(float a) {Ev& e=PushEv(4);e.v[0]=BitsOf(&a);return FloatOfBits(g.limRet);}
inline void CaseSlide(Env& env,Totals& t,unsigned n,u32 timerSaved) {
    const C4 c=MakeC4(n);
    FillWords(taskRef.data(),kTaskArena,Rand(n,53));FillWords(pedRef.data(),kPedArena,Rand(n,54));FillWords(pmatRef.data(),kMatrixArena,Rand(n,55));
    FillWords(intelRef.data(),kIntelArena,Rand(n,56));FillWords(primRef.data(),kPrimArena,Rand(n,57));
    uint8* task=taskArena.data()+kTaskOff;uint8* ped=pedArena.data()+kPedOff;uint8* mat=pmatArena.data()+kMatrixOff;uint8* intel=intelArena.data()+kIntelOff;uint8* prim=primArena.data()+kPrimOff;
    uint8* rt=taskRef.data()+kTaskOff;uint8* rp=pedRef.data()+kPedOff;uint8* rm=pmatRef.data()+kMatrixOff;uint8* ri=intelRef.data()+kIntelOff;uint8* rq=primRef.data()+kPrimOff;
    for(u32 i=0;i<3;++i) {
        At<u32>(rt,kTaskPosAt+4*i)=c.tgt[i];
        At<u32>(rp,kPedPlacementAt+4*i)=c.hasMatrix?c.decoy[i]:c.pos[i];
        At<u32>(rm,kMatrixPosAt+4*i)=c.hasMatrix?c.pos[i]:c.decoy[i];
        At<u32>(rm,0x00+4*i)=c.right[i];At<u32>(rm,0x10+4*i)=c.fwd[i];
    }
    At<u32>(rt,kTaskAimAt)=c.taskAim;At<u32>(rt,kTaskSpeedAt)=c.speed;rt[kTaskBitsAt]=uint8(c.bits78);At<u32>(rt,kTaskTimerAt)=c.timer;
    At<uintptr_t>(rp,kPedMatrixAt)=c.hasMatrix?uintptr_t(mat):0;
    At<uintptr_t>(rp,kPedIntelAt)=c.intelValid?uintptr_t(intel):uintptr_t(0xDEAD1230u+(n&0xF)*4u);
    At<u32>(rp,kPedShiftAt)=c.shiftInit[0];At<u32>(rp,kPedShiftAt+4)=c.shiftInit[1];
    At<u32>(rp,kPedCurRotAt)=c.cur;At<u32>(rp,kPedAimRotAt)=c.aim;At<u32>(rp,kPedTypeAt)=c.pedType;
    At<uintptr_t>(ri,kIntelPrimaryDefaultAt)=uintptr_t(prim);
    At<uintptr_t>(rq,0)=uintptr_t(fakeVtbl.data());
    g.animRet=c.animRet;g.limRet=c.limRet;
    g.taskObj=uintptr_t(task);g.pedObj=uintptr_t(ped);g.primObj=uintptr_t(prim);
    const json input=json::parse(JsonC4(c));
    const u32 args[1]{u32(uintptr_t(ped))};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetArena(taskArena,taskRef);SetArena(pedArena,pedRef);SetArena(pmatArena,pmatRef);SetArena(intelArena,intelRef);SetArena(primArena,primRef);nev=0;
        CTimer::m_snTimeInMilliseconds=c.now;
        sides[s]=Execute(env,2,0x66C4E0,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,u32(uintptr_t(task)),c.edx,c.stack,0);});
        CTimer::m_snTimeInMilliseconds=timerSaved;
        json obs;json ej=json::array();
        for(unsigned i=0;i<nev&&i<evs.size();++i) {
            const Ev& e=evs[i];
            switch(e.kind) {
            case 1: ej.push_back(json::array({"anim",Label(e.v[0]),Label(e.v[1])})); break;
            case 2: ej.push_back(json::array({"stand",e.v[0],Label(e.v[1]),json::array({e.v[2],e.v[3],e.v[4],e.v[5],e.v[6],e.v[7],e.v[8],e.v[9],e.v[10]})})); break;
            case 3: ej.push_back(json::array({"abort",Label(e.v[0]),Label(e.v[1]),e.v[2],e.v[3]})); break;
            case 4: ej.push_back(json::array({"limit",e.v[0]})); break;
            default: ej.push_back(json::array({"unexpected",e.kind,e.v[0],e.v[1]})); break;
            }
        }
        obs["events"]=ej;obs["nevents"]=nev;obs["ret_al"]=sides[s].reg.eax&0xFFu;
        obs["task_changes"]=ArenaChanges(taskArena.data(),taskRef.data(),kTaskArena,kTaskOff);
        obs["ped_changes"]=ArenaChanges(pedArena.data(),pedRef.data(),kPedArena,kPedOff);
        obs["aux_unchanged"]=pmatArena==pmatRef&&intelArena==intelRef&&primArena==primRef;
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&CTimer::m_snTimeInMilliseconds==timerSaved;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    memset(g.arenaLo,0,sizeof(g.arenaLo));
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<4;++f) {
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
    const u32 timerSaved=CTimer::m_snTimeInMilliseconds;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",4},{"pairs",4*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_timer",timerSaved}});
    Totals totals;
    if(ready) {
        // arena de paginas del monton y manejador de PAGE_GUARD
        gHeapBase=static_cast<uint8*>(VirtualAlloc(nullptr,size_t(kHeapPages)*kPageBytes,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE));
        void* veh=gHeapBase?AddVectoredExceptionHandler(1,GuardHandler):nullptr;
        if(!gHeapBase||!veh) {
            Write({{"event","error"},{"reason","no se pudo preparar la arena de paginas"}});
            ready=false;
        } else {
            FillTraps(std::make_integer_sequence<int,12>{});
            fakeVtbl[6]=reinterpret_cast<void*>(&FakeAbort);
            for(unsigned f=0;f<4;++f) {
                const unsigned before=totals.failures;
                for(unsigned c=0;c<kCases;++c) {
                    switch(f) {
                    case 0: CaseHeap(env,totals,c); break;
                    case 1: CaseSpot(env,totals,c); break;
                    case 2: CaseSlide(env,totals,c,timerSaved); break;
                    default: CaseNode(env,totals,c); break;
                    }
                }
                Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
            }
            RemoveVectoredExceptionHandler(veh);
            VirtualFree(gHeapBase,0,MEM_RELEASE);gHeapBase=nullptr;
            ready&=CTimer::m_snTimeInMilliseconds==timerSaved;
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
