#pragma once
// Casos de las seis funciones y ejecucion del banco PathFind62 (continua el espacio de nombres de Core.hpp).
#include "Core.hpp"

namespace PathFind62Live {
// ================================================================================================ arenas y estado de los falsos
alignas(16) inline std::array<u8,kThisArena> thisArena{},thisRef{};
alignas(16) inline std::array<std::array<u8,kNodesArena>,kMaxSlots> nodeArena{},nodeRef{};
alignas(16) inline std::array<std::array<u8,kLinksArena>,kMaxSlots> linkArena{},linkRef{};
alignas(16) inline std::array<u8,kMallocArena> mallocArena{},mallocRef{};
inline u8* ThisObj() {return thisArena.data()+kThisOff;}
template<class T> inline T& At(void* base,size_t off) {return *reinterpret_cast<T*>(static_cast<u8*>(base)+off);}
// Arena pequena (guarda | objeto | guarda) para parametros de salida
struct Small {
    alignas(16) std::array<u8,kSmallArena> mem{},ref{};
    u8* obj() {return mem.data()+kSmallObj;}
    u8* robj() {return ref.data()+kSmallObj;}
    void Seed(u32 s) {FillWords(ref.data(),kSmallArena,s);}
    void Reset() {mem=ref;}
    json Changes() const {return Runs(mem.data(),ref.data(),kSmallArena,kSmallObj);}
    // cambios ignorando los primeros `skip` bytes del objeto (campos que el caso observa aparte)
    json ChangesExcept(size_t skip) const {auto tmp=mem;memcpy(tmp.data()+kSmallObj,ref.data()+kSmallObj,skip);return Runs(tmp.data(),ref.data(),kSmallArena,kSmallObj);}
};
inline Small retA,outA,tgtA,oriA,dirA,u1A,u2A,ocA,oriF;
inline u32 PtrOf(const void* p) {return u32(uintptr_t(p));}

struct FakeState {
    const P1* p1{};const P2* p2{};const P3* p3{};const P6* p6{};
    u32 callIdx{},randIdx{},groundIdx{},mallocTop{};
};
inline FakeState g;

// Mundo: imagen de referencia de `this` y de las arenas de nodos/enlaces (las arenas vivas se restauran desde ella antes de cada lado)
inline void LoadWorld(const World& w,u32 salt) {
    FillWords(thisRef.data(),kThisArena,Rand(salt,9001));
    u8* t=thisRef.data()+kThisOff;
    std::memset(t+kOffPathNodes,0,kNumAreas*4);
    std::memset(t+kOffNodeLinks,0,kNumAreas*4);
    for(u32 s=0;s<kMaxSlots;++s) {
        FillWords(nodeRef[s].data(),kNodesArena,Rand(salt,9100+s));
        FillWords(linkRef[s].data(),kLinksArena,Rand(salt,9200+s));
        if(s>=w.nAreas)continue;
        memcpy(nodeRef[s].data()+kNodesOff,w.nodes[s].data(),w.nodes[s].size()*4);
        memcpy(linkRef[s].data()+kLinksOff,w.links[s].data(),w.links[s].size()*4);
        const u32 a=w.area[s];
        At<u32>(t,kOffPathNodes+4*a)=PtrOf(nodeArena[s].data()+kNodesOff);
        At<u32>(t,kOffNodeLinks+4*a)=PtrOf(linkArena[s].data()+kLinksOff);
        At<u32>(t,kOffNumNodes+4*a)=w.Total(s);At<u32>(t,kOffNumVeh+4*a)=w.nVeh[s];At<u32>(t,kOffNumPed+4*a)=w.nPed[s];
    }
}
inline void ResetWorld() {thisArena=thisRef;nodeArena=nodeRef;linkArena=linkRef;}
inline json WorldChanges(const World& w) {
    json o;
    o["this"]=Runs(thisArena.data(),thisRef.data(),kThisArena,kThisOff);
    json nodes=json::array(),links=json::array();
    for(u32 s=0;s<w.nAreas;++s) {
        nodes.push_back(Runs(nodeArena[s].data(),nodeRef[s].data(),kNodesArena,kNodesOff));
        links.push_back(Runs(linkArena[s].data(),linkRef[s].data(),kLinksArena,kLinksOff));
    }
    o["nodes"]=nodes;o["links"]=links;
    return o;
}
// Globales reales que tocan las funciones: se fijan a lo que pide el caso y se restauran siempre desde la copia inicial
inline std::array<u8,kGlobABytes> snapA{};inline std::array<u8,kGlobBBytes> snapB{};
inline std::array<u8,kGlobABytes> fixA{};inline std::array<u8,kGlobBBytes> fixB{};
inline u8* GlobA() {return reinterpret_cast<u8*>(kGlobAAddr);}
inline u8* GlobB() {return reinterpret_cast<u8*>(kGlobBAddr);}
inline void RestoreGlobals() {memcpy(GlobA(),snapA.data(),kGlobABytes);memcpy(GlobB(),snapB.data(),kGlobBBytes);}

// Cierre comun de un lado: filas de ABI, globales y estado x87
inline json SideRow(Env& env,const Side& side,json obs,bool retPtrExpected,u32 retPtr) {
    auto fp=NormalizeFp(side.fp);
    obs["ret_ptr_ok"]=retPtrExpected?json(side.reg.eax==retPtr):json(nullptr);
    if(kCompareVolatileRegisters){obs["eax"]=side.reg.eax;obs["ecx"]=side.reg.ecx;obs["edx"]=side.reg.edx;}
    json out=CommonRow(side,fp);out["observed"]=obs;
    RestoreFloatingState();
    out["globals_restored"]=env.WatchesIntact()&&env.GlobalsIntact();
    out["floating_state_restored"]=FloatingStateRestored();
    return out;
}

// ================================================================================================ falsos
// 1: busqueda por region 0x44F2C0 (thiscall, 11 argumentos en la pila, `ret 2Ch`)
inline u32 __fastcall FakeRegion(void* self,void*,u32 retp,u32 area,u32 x,u32 y,u32 z,u32 nt,u32 distp,u32 u2,u32 u4,u32 boats,u32 u6) {
    Ev& e=PushEv(1);
    const char local=0;
    e.v[0]=area;e.v[1]=x;e.v[2]=y;e.v[3]=z;e.v[4]=nt&0xFFu;e.v[5]=BitsOf(reinterpret_cast<void*>(distp));e.v[6]=u2;e.v[7]=u4;e.v[8]=boats;e.v[9]=u6;
    e.v[10]=NearStack(reinterpret_cast<void*>(distp),&local);e.v[11]=0;e.v[12]=self==ThisObj();   // v[11] reservado: la DLL puede devolver por RVO directamente en el puntero del llamador (arena), el EXE usa un local
    CommitEv(e);
    for(u32 i=0;i<g.p1->ns;++i) {
        if(g.p1->sArea[i]!=(area&0xFFFFu))continue;
        const float cand=FloatOfBits(g.p1->sDist[i]),cur=FloatOfBits(BitsOf(reinterpret_cast<void*>(distp)));
        if(cand<cur) {
            *reinterpret_cast<u32*>(retp)=(area&0xFFFFu)|(g.p1->sNode[i]<<16);
            *reinterpret_cast<u32*>(distp)=g.p1->sDist[i];
        }
        break;
    }
    return 0;
}
// 2: FindNodeClosestToCoors 0x44F460 (thiscall, 11 argumentos en la pila, `ret 2Ch`, EAX = puntero oculto)
inline u32 __fastcall FakeFind(void* self,void*,u32 retp,u32 x,u32 y,u32 z,u32 nt,u32 maxd,u32 u2,u32 u3,u32 u4,u32 boats,u32 u6) {
    Ev& e=PushEv(2);
    e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=nt&0xFFu;e.v[4]=maxd;e.v[5]=u2;e.v[6]=u3;e.v[7]=u4;e.v[8]=boats;e.v[9]=u6;e.v[10]=self==ThisObj();e.v[11]=g.callIdx;
    CommitEv(e);
    u32 res=0xFFFFu|(0xBEEFu<<16);
    const u32 k=g.callIdx++;
    if(g.p2){if(k<g.p2->ns)res=g.p2->rArea[k]|(g.p2->rNode[k]<<16);}
    else if(g.p3){if(k==0)res=g.p3->result;}
    *reinterpret_cast<u32*>(retp)=res;
    return retp;
}
// 3: 0x450FB0 (cdecl, 6 argumentos): suma uno al contador
inline void __cdecl FakeLink(i32 a1,i32 a2,i32 a3,i32 a4,i32 a5,i32* counter) {
    Ev& e=PushEv(3);
    const char local=0;
    e.v[0]=u32(a1);e.v[1]=u32(a2);e.v[2]=u32(a3);e.v[3]=u32(a4);e.v[4]=u32(a5);e.v[5]=u32(*counter);e.v[6]=NearStack(counter,&local);
    CommitEv(e);
    ++*counter;
}
// 4: CMemoryMgr::Malloc (cdecl, 2 argumentos) sobre una arena propia
inline void* __cdecl FakeMalloc(u32 size,u32 hint) {
    Ev& e=PushEv(4);
    e.v[0]=size;e.v[1]=hint;
    const u32 aligned=(g.mallocTop+15u)&~15u;
    void* p=nullptr;
    if(aligned+size<=kMallocBytes){p=mallocArena.data()+kMallocOff+aligned;g.mallocTop=aligned+size;e.v[2]=0;e.v[3]=aligned;}
    else e.v[2]=1;
    CommitEv(e);
    return p;
}
// 5: AddDynamicLinkBetween2Nodes_For1Node 0x44E000 (thiscall, 2 dwords, `ret 8`)
inline void __fastcall FakeAddDyn(void* self,void*,u32 first,u32 second) {
    Ev& e=PushEv(5);
    e.v[0]=first;e.v[1]=second;e.v[2]=self==ThisObj();
    CommitEv(e);
}
// 6: rand() del CRT del EXE (cdecl, sin argumentos)
inline int __cdecl FakeRand() {
    Ev& e=PushEv(6);
    e.v[0]=g.randIdx;
    CommitEv(e);
    return int(ScriptRand(g.p6->randMode,g.p6->randSeed,g.randIdx++));
}
// 7: CWorld::FindGroundZFor3DCoord 0x5696C0 (cdecl: x, y, z, bool*, CEntity**; devuelve float en ST0)
inline float __cdecl FakeGround(u32 x,u32 y,u32 z,u8* found,void* ent) {
    Ev& e=PushEv(7);
    const char local=0;
    e.v[0]=x;e.v[1]=y;e.v[2]=z;e.v[3]=NearStack(found,&local);e.v[4]=ent!=nullptr;e.v[5]=g.groundIdx;
    CommitEv(e);
    const u32 k=g.groundIdx++;
    *found=k<kGroundN?u8(g.p6->groundFound[k]):u8(0);
    return Fadd(FloatOfBits(z),FloatOfBits(k<kGroundN?g.p6->groundDelta[k]:0u));
}

// ================================================================================================ 0: CPathFind::FindNodeClosestToCoors
inline void CaseClosest(Env& env,Totals& t,unsigned n) {
    const P1 c=MakeP1(n);
    LoadWorld(World{},n);
    retA.Seed(Rand(n,200));
    g=FakeState{};g.p1=&c;
    const u32 args[11]{PtrOf(retA.obj()),c.pos[0],c.pos[1],c.pos[2],c.nodeType,c.maxDist,c.unk2,c.unk3,c.unk4,c.boats,c.unk6};
    const json input=json::parse(JsonP1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetWorld();retA.Reset();ResetEvs();g.callIdx=0;
        sides[s]=Execute(env,0,0x44F460,candidate,[&](uintptr_t entry){return Gate(entry,args,11,c.eax,PtrOf(ThisObj()),c.edx,c.stack,0);});
        json obs;
        obs["events"]=EventsJson([](const Ev& e){json r=json::array({"region"});for(int k=0;k<13;++k)r.push_back(e.v[k]);return r;});
        obs["nevents"]=nev;obs["ev_hash"]=evHash;
        const u32 res=BitsOf(retA.obj());const bool inv=(res&0xFFFFu)==0xFFFFu;
        obs["area"]=res&0xFFFFu;obs["node"]=inv?0xFFFFu:res>>16;   // la mitad de nodo de un resultado invalido es residuo de pila del EXE (la DLL deja -1)
        obs["ret_other"]=retA.ChangesExcept(4);
        obs["world"]=WorldChanges(World{});
        rows[s]=SideRow(env,sides[s],obs,true,args[0]);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: CPathFind::FindNthNodeClosestToCoors
inline void CaseNth(Env& env,Totals& t,unsigned n) {
    const P2 c=MakeP2(n);
    LoadWorld(c.w,n);
    retA.Seed(Rand(n,200));outA.Seed(Rand(n,201));
    g=FakeState{};g.p2=&c;
    const u32 args[12]{PtrOf(retA.obj()),c.pos[0],c.pos[1],c.pos[2],c.nodeType,c.maxDist,c.lowTraffic,c.unkn,c.nth,c.boats,c.ignore,c.hasOut?PtrOf(outA.obj()):0u};
    const json input=json::parse(JsonP2(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetWorld();retA.Reset();outA.Reset();ResetEvs();g.callIdx=0;
        sides[s]=Execute(env,1,0x44F8C0,candidate,[&](uintptr_t entry){return Gate(entry,args,12,c.eax,PtrOf(ThisObj()),c.edx,c.stack,0);});
        json obs;
        obs["events"]=EventsJson([](const Ev& e){json r=json::array({"find"});for(int k=0;k<12;++k)r.push_back(e.v[k]);return r;});
        obs["nevents"]=nev;obs["ev_hash"]=evHash;
        obs["ret"]=BitsOf(retA.obj());
        obs["ret_other"]=retA.ChangesExcept(4);
        obs["out"]=outA.Changes();
        obs["world"]=WorldChanges(c.w);
        rows[s]=SideRow(env,sides[s],obs,true,args[0]);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2: CPathFind::FindNextNodeWandering
inline void CaseWander(Env& env,Totals& t,unsigned n) {
    const P3 c=MakeP3(n);
    LoadWorld(c.w,n);
    oriA.Seed(Rand(n,200));tgtA.Seed(Rand(n,201));dirA.Seed(Rand(n,202));
    At<u32>(oriA.robj(),0)=c.origin;At<u32>(tgtA.robj(),0)=c.targetInit;At<u8>(dirA.robj(),0)=u8(c.outDirInit);
    g=FakeState{};g.p3=&c;
    const u32 args[8]{c.nodeType,c.pos[0],c.pos[1],c.pos[2],c.originKind==0?0u:PtrOf(oriA.obj()),PtrOf(tgtA.obj()),c.dir,PtrOf(dirA.obj())};
    const json input=json::parse(JsonP3(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetWorld();oriA.Reset();tgtA.Reset();dirA.Reset();ResetEvs();g.callIdx=0;
        sides[s]=Execute(env,2,0x451B70,candidate,[&](uintptr_t entry){return Gate(entry,args,8,c.eax,PtrOf(ThisObj()),c.edx,c.stack,0);});
        json obs;
        obs["events"]=EventsJson([](const Ev& e){json r=json::array({"find"});for(int k=0;k<12;++k)r.push_back(e.v[k]);return r;});
        obs["nevents"]=nev;obs["ev_hash"]=evHash;
        obs["origin"]=BitsOf(oriA.obj());obs["origin_other"]=oriA.ChangesExcept(4);
        obs["target"]=BitsOf(tgtA.obj());obs["target_other"]=tgtA.ChangesExcept(4);
        obs["out_dir"]=*dirA.obj();obs["out_dir_other"]=dirA.ChangesExcept(1);
        obs["world"]=WorldChanges(c.w);
        rows[s]=SideRow(env,sides[s],obs,false,0);
    }
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ 3: CPathFind::CompleteNewInterior
inline void SetInteriorGlobals(const P4& c) {
    u8* A=GlobA();u8* B=GlobB();
    for(u32 i=0;i<8;++i)At<u32>(A,i*4)=c.intLinked[i];
    At<u32>(A,kGAExtLinks)=c.numExt;
    memcpy(A+kGAConnects,c.connects,sizeof(c.connects));memcpy(A+kGADontWander,c.dontWander,sizeof(c.dontWander));
    for(u32 i=0;i<64;++i){At<u32>(A,kGAZ+4*i)=c.z[i];At<u32>(A,kGAY+4*i)=c.y[i];At<u32>(A,kGAX+4*i)=c.x[i];}
    At<u32>(A,kGANumNodes)=c.numNodes;At<u32>(A,kGASlot)=c.slot;At<u32>(A,kGAInteriorId)=c.interiorId;A[kGABuilding]=u8(c.building);
    for(u32 i=0;i<8;++i)At<u32>(B,kGBExtNodes+4*i)=c.extLinked[i];
}
// puntero de la arena de Malloc como desplazamiento (o el valor crudo si no cae dentro)
inline json MallocOff(u32 p) {
    const u32 lo=PtrOf(mallocArena.data()+kMallocOff);
    if(p==0)return 0;
    return p>=lo&&p<lo+kMallocBytes?json(p-lo):json(0xFFFFFFFFu);
}
inline void CaseInterior(Env& env,Totals& t,unsigned n) {
    const P4 c=MakeP4(n);
    LoadWorld(c.w,n);
    outA.Seed(Rand(n,200));At<u32>(outA.robj(),0)=c.outInit;
    FillWords(mallocRef.data(),kMallocArena,c.fill);
    g=FakeState{};
    const u32 args[1]{c.outKind?PtrOf(outA.obj()):0u};
    const json input=json::parse(JsonP4(c));
    Side sides[2];json rows[2];
    const u32 a=64u+c.slot;
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetWorld();outA.Reset();mallocArena=mallocRef;ResetEvs();g.mallocTop=0;
        RestoreGlobals();SetInteriorGlobals(c);memcpy(fixA.data(),GlobA(),kGlobABytes);memcpy(fixB.data(),GlobB(),kGlobBBytes);
        sides[s]=Execute(env,3,0x452270,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,PtrOf(ThisObj()),c.edx,c.stack,0);});
        json obs;
        obs["events"]=EventsJson([](const Ev& e){
            switch(e.kind){
            case 3: return json::array({"link",e.v[0],e.v[1],e.v[2],e.v[3],e.v[4],e.v[5],e.v[6]});
            case 4: return json::array({"malloc",e.v[0],e.v[1],e.v[2],e.v[3]});
            case 5: return json::array({"adddyn",e.v[0],e.v[1],e.v[2]});
            default: return json::array({"unexpected",e.kind});
            }});
        obs["nevents"]=nev;obs["ev_hash"]=evHash;obs["malloc_top"]=g.mallocTop;
        obs["out"]=outA.Changes();
        {   // campos de `this` que escribe la funcion: punteros como desplazamientos de la arena de Malloc, el resto de valores crudos; solo si cambian
            const u8* tt=ThisObj();const u8* rr=thisRef.data()+kThisOff;
            json f=json::object();
            auto ptrField=[&](const char* nm,size_t off){const u32 v=At<u32>(const_cast<u8*>(tt),off),r=At<u32>(const_cast<u8*>(rr),off);f[nm]=v==r?json(nullptr):MallocOff(v);};
            auto valField=[&](const char* nm,size_t off){const u32 v=At<u32>(const_cast<u8*>(tt),off),r=At<u32>(const_cast<u8*>(rr),off);f[nm]=v==r?json(nullptr):json(v);};
            ptrField("path_nodes",kOffPathNodes+4*a);ptrField("node_links",kOffNodeLinks+4*a);ptrField("link_lengths",kOffLinkLengths+4*a);ptrField("intersections",kOffIntersections+4*a);
            valField("num_nodes",kOffNumNodes+4*a);valField("num_veh",kOffNumVeh+4*a);valField("num_ped",kOffNumPed+4*a);valField("num_car_links",kOffNumCarLinks+4*a);
            valField("num_addresses",kOffNumAddresses+4*a);valField("interior_id",kOffInteriorIds+4*c.slot);
            obs["this_fields"]=f;
            auto tmp=thisArena;
            for(size_t off:{kOffPathNodes+4*a,kOffNodeLinks+4*a,kOffLinkLengths+4*a,kOffIntersections+4*a,kOffNumNodes+4*a,kOffNumVeh+4*a,kOffNumPed+4*a,kOffNumCarLinks+4*a,kOffNumAddresses+4*a,size_t(kOffInteriorIds+4*c.slot)})
                memcpy(tmp.data()+kThisOff+off,thisRef.data()+kThisOff+off,4);
            obs["this_other"]=Runs(tmp.data(),thisRef.data(),kThisArena,kThisOff);
        }
        obs["malloc"]=Runs(mallocArena.data(),mallocRef.data(),kMallocArena,kMallocOff);
        obs["glob_a"]=Runs(GlobA(),fixA.data(),kGlobABytes,0);
        obs["glob_b"]=Runs(GlobB(),fixB.data(),kGlobBBytes,0);
        obs["nodes"]=json::array();for(u32 q=0;q<c.w.nAreas;++q)obs["nodes"].push_back(Runs(nodeArena[q].data(),nodeRef[q].data(),kNodesArena,kNodesOff));
        RestoreGlobals();
        rows[s]=SideRow(env,sides[s],obs,false,0);
    }
    Finish(t,kFn[3],n,input,rows,sides);
}

// ================================================================================================ 4: CPathFind::FindParkingNodeInArea
inline void CaseParking(Env& env,Totals& t,unsigned n) {
    const P5 c=MakeP5(n);
    LoadWorld(c.w,n);
    retA.Seed(Rand(n,200));
    g=FakeState{};
    const u32 args[7]{PtrOf(retA.obj()),c.box[0],c.box[1],c.box[2],c.box[3],c.box[4],c.box[5]};
    const json input=json::parse(JsonP5(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetWorld();retA.Reset();ResetEvs();
        RestoreGlobals();At<u32>(GlobB(),0x977B9Cu-kGlobBAddr)=c.cursor;
        sides[s]=Execute(env,4,0x4513F0,candidate,[&](uintptr_t entry){return Gate(entry,args,7,c.eax,PtrOf(ThisObj()),c.edx,c.stack,0);});
        json obs;
        obs["ret"]=json::array({BitsOf(retA.obj()),BitsOf(retA.obj()+4),BitsOf(retA.obj()+8)});
        obs["ret_other"]=retA.ChangesExcept(12);
        obs["cursor"]=At<u32>(GlobB(),0x977B9Cu-kGlobBAddr);
        obs["world"]=WorldChanges(c.w);
        {   // el resto de la zona B no cambia
            auto tmp=snapB;memcpy(tmp.data()+(0x977B9Cu-kGlobBAddr),GlobB()+(0x977B9Cu-kGlobBAddr),4);
            obs["glob_b_other"]=Runs(GlobB(),tmp.data(),kGlobBBytes,0);
        }
        RestoreGlobals();
        rows[s]=SideRow(env,sides[s],obs,true,args[0]);
    }
    Finish(t,kFn[4],n,input,rows,sides);
}

// ================================================================================================ 5: CPathFind::GeneratePedCreationCoors_Interior
inline void CaseGenerate(Env& env,Totals& t,unsigned n) {
    const P6 c=MakeP6(n);
    LoadWorld(c.w,n);
    ocA.Seed(Rand(n,200));u1A.Seed(Rand(n,201));u2A.Seed(Rand(n,202));oriF.Seed(Rand(n,203));
    for(u32 i=0;i<3;++i)At<u32>(ocA.robj(),4*i)=c.outCoordsInit[i];
    At<u32>(oriF.robj(),0)=c.outOriInit;
    g=FakeState{};g.p6=&c;
    const u32 args[6]{c.x,c.y,PtrOf(ocA.obj()),PtrOf(u1A.obj()),PtrOf(u2A.obj()),PtrOf(oriF.obj())};
    const json input=json::parse(JsonP6(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ResetWorld();ocA.Reset();u1A.Reset();u2A.Reset();oriF.Reset();ResetEvs();g.randIdx=0;g.groundIdx=0;
        sides[s]=Execute(env,5,0x44ECA0,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,PtrOf(ThisObj()),c.edx,c.stack,0);});
        json obs;
        obs["events"]=EventsJson([](const Ev& e){
            switch(e.kind){
            case 6: return json::array({"rand",e.v[0]});
            case 7: return json::array({"ground",e.v[0],e.v[1],e.v[2],e.v[3],e.v[4],e.v[5]});
            default: return json::array({"unexpected",e.kind});
            }});
        obs["nevents"]=nev;obs["ev_hash"]=evHash;obs["rand_calls"]=g.randIdx;obs["ground_calls"]=g.groundIdx;
        obs["ret_al"]=sides[s].reg.eax&0xFFu;
        obs["coords"]=json::array({BitsOf(ocA.obj()),BitsOf(ocA.obj()+4),BitsOf(ocA.obj()+8)});
        obs["coords_other"]=ocA.ChangesExcept(12);
        obs["ori"]=BitsOf(oriF.obj());obs["ori_other"]=oriF.ChangesExcept(4);
        obs["unused"]=json::array({u1A.Changes(),u2A.Changes()});
        obs["world"]=WorldChanges(c.w);
        rows[s]=SideRow(env,sides[s],obs,false,0);
    }
    Finish(t,kFn[5],n,input,rows,sides);
}

// ================================================================================================ ejecucion del banco
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// fsin/fcos del x87 (control de precision de 24 bits) frente a (float)sin((double)x) de la DLL para los 256 valores de `dir` de FindNextNodeWandering (angulo = dir * 0.7853982f)
inline void X87SinCos(float angle,float* s,float* c) {
    __asm {
        fld dword ptr angle
        fld st(0)
        fsin
        mov eax,s
        fstp dword ptr [eax]
        fcos
        mov eax,c
        fstp dword ptr [eax]
    }
}
inline unsigned SinCosMismatches() {
    unsigned bad=0;
    PrepareFloatingState();
    for(unsigned d=0;d<256;++d) {
        volatile float angle=float(d);angle=angle*0.7853982f;
        float s{},c{};
        X87SinCos(angle,&s,&c);
        const float ds=float(std::sin(double(angle))),dc=float(std::cos(double(angle)));
        bad+=BitsOf(&s)!=BitsOf(&ds);bad+=BitsOf(&c)!=BitsOf(&dc);
    }
    RestoreFloatingState();
    return bad;
}

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
    const unsigned sinCos=SinCosMismatches();
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"watched_globals",globalsJ},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"sincos_mismatches",sinCos}});
    Totals totals;
    if(ready) {
        memcpy(snapA.data(),GlobA(),kGlobABytes);memcpy(snapB.data(),GlobB(),kGlobBBytes);
        // F2 y F3 sustituyen FindNodeClosestToCoors con su hook ENCENDIDO (los falsos se parchean tambien en el cuerpo de la DLL, resuelto antes de crear las rutas)
        auto* closest=FindHook("CPathFind","FindNodeClosestToCoors");
        const unsigned order[kFns]{1,2,3,4,5,0};
        for(unsigned f:order) {
            const unsigned before=totals.failures;
            const bool needsClosest=f==1||f==2;
            if(needsClosest&&closest)closest->State(true);
            for(unsigned c=0;c<kCases;++c) {
                switch(f) {
                case 0: CaseClosest(env,totals,c); break;
                case 1: CaseNth(env,totals,c); break;
                case 2: CaseWander(env,totals,c); break;
                case 3: CaseInterior(env,totals,c); break;
                case 4: CaseParking(env,totals,c); break;
                default: CaseGenerate(env,totals,c); break;
                }
            }
            if(needsClosest&&closest)closest->State(false);
            Write({{"event","function_summary"},{"function",kFn[f]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()&&env.GlobalsIntact()}});
        }
        RestoreGlobals();
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
