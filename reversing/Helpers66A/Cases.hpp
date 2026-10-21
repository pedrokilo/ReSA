#pragma once
// Casos del banco Helpers66A (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y una funcion de caso por funcion.
#include "Core.hpp"
#include <utility>

namespace Hp66Live {
// ================================================================================================ arenas
template<size_t N> struct Arena {
    alignas(16) std::array<uint8,N> m{},r{};
    uintptr_t Obj(size_t off=kGuard) const {return uintptr_t(m.data())+off;}
    void Fill(u32 seed) {FillWords(m.data(),N,seed);}
    void Snap() {r=m;}
    void Put32(size_t off,u32 v) {memcpy(m.data()+kGuard+off,&v,4);}
    void Put8(size_t off,u32 v) {m[kGuard+off]=uint8(v);}
    u32 Get32(size_t off) const {u32 v;memcpy(&v,m.data()+kGuard+off,4);return v;}
    json Changes() const {return ArenaChanges(m.data(),r.data(),N,kGuard);}
    void Tagged() const {TagAdd(m.data(),N);}
};
inline Arena<kPedArena> pedA;
inline Arena<kPlayerArena> playerA;
inline Arena<kEntArena> entA,entB,entC;
inline Arena<kMatArena> matSelf,matTgt,matCam;
inline Arena<kVecArena> vecA,vecB,vecC;
inline Arena<kTaskArena> taskA;
inline Arena<kCamArena> camA;
inline Arena<kSmokeArena> smokeA;

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
inline void SetCamera(bool hasMat,const u32* used,const u32* other) {
    for(unsigned i=0;i<3;++i) {G32(kCamPlacement+4*i)=hasMat?other[i]:used[i];matCam.Put32(0x30+4*i,hasMat?used[i]:other[i]);}
    G32(kCamMatrixPtr)=hasMat?u32(matCam.Obj()):0u;
}

// ================================================================================================ 0: PlayerWantsToAttack (0x60CC50)
struct State {const C0* c0{};const C1* c1{};const C2* c2{};const C3* c3{};const C4* c4{};};
inline State g;
inline u32 __fastcall FakeCount(void* self,void*) {Ev& e=PushEv(1,1);e.v[0]=Tag(self);return g.c0->count;}                                     // 0x5F6AA0, `ret`
inline void __fastcall FakeReport(void* self,void*) {Ev& e=PushEv(2,1);e.v[0]=Tag(self);}                                                      // 0x5F8780, `ret`
inline u32 __cdecl FakeIsTag(void* entity) {Ev& e=PushEv(3,1);e.v[0]=Tag(entity);return g.c0->isTagRet;}                                        // 0x49CCE0, cdecl
inline u32 __fastcall FakeCanTarget(void* self,void*) {Ev& e=PushEv(4,1);e.v[0]=Tag(self);return g.c0->canRet;}                                // 0x59F320, `ret`
inline u32 __fastcall FakeFindPed(void* self,void*) {                                                                                            // 0x60C5F0, `ret`
    Ev& e=PushEv(5,1);e.v[0]=Tag(self);
    return g.c0->findKind==0?0u:g.c0->findKind==1?u32(entC.Obj()):g.c0->findRaw;
}
inline void __fastcall FakeAttack(void* self,void*,void* player,void* ped) {Ev& e=PushEv(6,3);e.v[0]=Tag(self);e.v[1]=Tag(player);e.v[2]=Tag(ped);}   // 0x5F7CC0, `ret 8`
inline void CasePlayerWants(Env& env,Totals& t,unsigned n) {
    const C0 c=MakeC0(n);
    const json input=json::parse(JsonC0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        pedA.Fill(c.seed*5u+1u);playerA.Fill(c.seed*5u+2u);entA.Fill(c.seed*5u+3u);entB.Fill(c.seed*5u+4u);entC.Fill(c.seed*5u+5u);
        pedA.Put32(0x480,u32(playerA.Obj()));pedA.Put32(0x71C,c.tKind?u32(entA.Obj()):0u);pedA.Put32(0x79C,c.mKind?u32(entB.Obj()):0u);
        playerA.Put32(0x38,c.group);
        entA.Put8(0x36,c.tType);entB.Put8(0x36,c.mType);
        *reinterpret_cast<uint8*>(kGroupsBase+c.group*kGroupStride+4)=uint8(c.flagByte);       // m_bMembersEnterLeadersVehicle del grupo del jugador
        *reinterpret_cast<uint8*>(kMouseFlag)=uint8(c.camMouse);
        pedA.Snap();playerA.Snap();entA.Snap();entB.Snap();entC.Snap();
        TagClear();pedA.Tagged();playerA.Tagged();entA.Tagged();entB.Tagged();entC.Tagged();TagAdd(reinterpret_cast<void*>(kGroupsBase),kGroupsBytes);
        g.c0=&c;nev=0;
        sides[s]=Execute(env,0,0x60CC50,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,u32(pedA.Obj()),c.edx,c.stack,0);});
        json obs;   // EAX no se compara: la funcion no deja un valor definido en EAX (nadie lo lee)
        obs["changes"]=json::array({pedA.Changes(),playerA.Changes(),entA.Changes(),entB.Changes(),entC.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: EvaluateNeighbouringTarget (0x60D1C0)
inline u32 __fastcall FakeBroken(void* self,void*,void* target,void* weapon) {                                                                    // 0x60C0C0, `ret 8`
    Ev& e=PushEv(11,3);e.v[0]=Tag(self);e.v[1]=Tag(target);e.v[2]=u32(uintptr_t(weapon)-uintptr_t(self));
    return g.c1->brokenRet;
}
inline float __cdecl FakeAtan(u32 xBits,u32 yBits) {Ev& e=PushEv(12,2);e.v[0]=xBits;e.v[1]=yBits;return F(g.c1->atanRet);}                        // 0x53CC70, cdecl, resultado float en ST0
inline void CaseNeighbour(Env& env,Totals& t,unsigned n) {
    const C1 c=MakeC1(n);
    const json input=json::parse(JsonC1(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        pedA.Fill(c.seed*7u+1u);entA.Fill(c.seed*7u+2u);matSelf.Fill(c.seed*7u+3u);matTgt.Fill(c.seed*7u+4u);matCam.Fill(c.seed*7u+5u);vecA.Fill(c.seed*7u+6u);vecB.Fill(c.seed*7u+7u);
        PlaceEntity(pedA,matSelf,c.selfMat,c.self,c.selfOther);
        PlaceEntity(entA,matTgt,c.tgtMat,c.tgt,c.tgtOther);
        pedA.Put8(0x718,c.slot);
        vecA.Put32(0,c.outTargetRaw);vecB.Put32(0,c.outPrio);
        SetCamera(c.camMat,c.cam,c.camOther);
        pedA.Snap();entA.Snap();matSelf.Snap();matTgt.Snap();matCam.Snap();vecA.Snap();vecB.Snap();
        TagClear();pedA.Tagged();entA.Tagged();matSelf.Tagged();matTgt.Tagged();matCam.Tagged();vecA.Tagged();vecB.Tagged();
        g.c1=&c;nev=0;
        const u32 args[6]{u32(entA.Obj()),u32(vecA.Obj()),u32(vecB.Obj()),c.maxDist,c.comp,c.flag};
        sides[s]=Execute(env,1,0x60D1C0,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,u32(pedA.Obj()),c.edx,c.stack,0);});
        json obs;   // void: EAX/ECX/EDX solo registrados
        obs["changes"]=json::array({pedA.Changes(),entA.Changes(),matSelf.Changes(),matTgt.Changes(),matCam.Changes(),vecA.Changes(),vecB.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2: HasClearLineOfSight (0x621B10)
inline u32 __cdecl FakeLos(const void* origin,const void* target,u32 a3,u32 a4,u32 a5,u32 a6,u32 a7,u32 a8,u32 a9) {                              // 0x56A490, cdecl 9 argumentos
    Ev& e=PushEv(21,15);
    for(unsigned i=0;i<3;++i) {e.v[i]=BitsOf(static_cast<const u8*>(origin)+4*i);e.v[3+i]=BitsOf(static_cast<const u8*>(target)+4*i);}
    e.v[6]=Tag(target);e.v[7]=a3;e.v[8]=a4;e.v[9]=a5;e.v[10]=a6;e.v[11]=a7;e.v[12]=a8;e.v[13]=a9;e.v[14]=Tag(uintptr_t(G32(kIgnoreEntity)));
    return g.c2->losRet;
}
inline u32 __cdecl FakeRand() {PushEv(22,0);return g.c2->randRet;}                                                                               // 0x821B1E, cdecl
inline void CaseLineOfSight(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    const json input=json::parse(JsonC2(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        taskA.Fill(c.seed*9u+1u);entA.Fill(c.seed*9u+2u);matSelf.Fill(c.seed*9u+3u);vecA.Fill(c.seed*9u+4u);entB.Fill(c.seed*9u+5u);
        PlaceEntity(entA,matSelf,c.hasMat,c.pos,c.other);      // el ped
        for(unsigned i=0;i<3;++i) vecA.Put32(4*i,c.aim[i]);
        taskA.Put32(0x18,c.losTime);taskA.Put8(0x1C,c.blocked);
        taskA.Put32(0x34,c.tKind==0?0u:c.tKind==1?u32(entB.Obj()):c.tRaw);
        G32(kTimerMs)=c.timer;G32(kIgnoreEntity)=c.ignoreInit;
        taskA.Snap();entA.Snap();matSelf.Snap();vecA.Snap();entB.Snap();
        TagClear();taskA.Tagged();entA.Tagged();matSelf.Tagged();vecA.Tagged();entB.Tagged();
        g.c2=&c;nev=0;
        const u32 args[2]{u32(entA.Obj()),u32(vecA.Obj())};
        sides[s]=Execute(env,2,0x621B10,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,u32(taskA.Obj()),c.edx,c.stack,0);});
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;   // el unico llamador del EXE (0x62780F) lee solo AL (test al,al; mov [esi+0Bh],al); ECX/EDX solo registrados
        obs["changes"]=json::array({taskA.Changes(),entA.Changes(),matSelf.Changes(),vecA.Changes(),entB.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[2],n,input,rows,sides);
}

// ================================================================================================ 3: AddPunchImpact (0x49F670)
inline void __fastcall FakeAddParticle(void* self,void*,const void* pos,const void* vel,u32 timeSince,const void* mult,u32 rotZ,u32 light,u32 limit,u32 createLocal) {   // 0x4AA440, `ret 20h`
    Ev& e=PushEv(31,19);
    e.v[0]=Tag(self);
    for(unsigned i=0;i<3;++i) {e.v[1+i]=BitsOf(static_cast<const u8*>(pos)+4*i);e.v[4+i]=BitsOf(static_cast<const u8*>(vel)+4*i);}
    e.v[7]=timeSince;
    for(unsigned i=0;i<7;++i) e.v[8+i]=BitsOf(static_cast<const u8*>(mult)+4*i);
    e.v[15]=rotZ;e.v[16]=light;e.v[17]=limit;e.v[18]=createLocal;
}
inline void CasePunch(Env& env,Totals& t,unsigned n) {
    const C3 c=MakeC3(n);
    const json input=json::parse(JsonC3(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        vecA.Fill(c.seed*3u+1u);vecB.Fill(c.seed*3u+2u);matCam.Fill(c.seed*3u+3u);smokeA.Fill(c.seed*3u+4u);entA.Fill(c.seed*3u+5u);
        for(unsigned i=0;i<3;++i) {vecA.Put32(4*i,c.pos[i]);vecB.Put32(4*i,c.vel[i]);}
        SetCamera(c.camMode,c.cam,c.other);
        G32(kSmokeSlot)=u32(smokeA.Obj());
        vecA.Snap();vecB.Snap();matCam.Snap();smokeA.Snap();entA.Snap();
        TagClear();vecA.Tagged();vecB.Tagged();matCam.Tagged();smokeA.Tagged();entA.Tagged();
        g.c3=&c;nev=0;
        const u32 thisPtr=c.thisKind==0?u32(entA.Obj()):c.thisKind==1?0xA9AE00u:c.thisKind==2?(0xDEAD0000u|(Rand(n,77)&0xFFFFu)):0u;
        const u32 args[3]{u32(vecA.Obj()),u32(vecB.Obj()),c.num};
        sides[s]=Execute(env,3,0x49F670,candidate,[&](uintptr_t entry){return Gate(entry,args,3,c.eax,thisPtr,c.edx,c.stack,0);});
        json obs;   // void
        obs["changes"]=json::array({vecA.Changes(),vecB.Changes(),matCam.Changes(),smokeA.Changes(),entA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[3],n,input,rows,sides);
}

// ================================================================================================ 4: RsSavePresetView (0x619FA0)
inline char pathBuf[16]{'v','i','e','w','s','.','t','x','t',0};
inline u32 __cdecl FakePathCreate(const char* s) {Ev& e=PushEv(41,1);e.v[0]=FnvStr(s);return g.c4->createKind?0u:u32(uintptr_t(pathBuf));}              // 0x745470, cdecl
inline void __cdecl FakePathDestroy(char* p) {Ev& e=PushEv(42,1);e.v[0]=p==pathBuf?1u:(p?2u:0u);}                                                  // 0x7454E0, cdecl
inline u32 __cdecl FakeFopen(const char* path,const char* mode) {                                                                                    // RwFileFunctions+4
    Ev& e=PushEv(43,3);e.v[0]=path==pathBuf?1u:(path?2u:0u);e.v[1]=FnvStr(path);e.v[2]=FnvStr(mode);
    return g.c4->fopenKind==0?0u:g.c4->fopenKind==1?0x55u:0x7FFFE000u+(g.c4->seed&0xFFF0u);
}
inline u32 __cdecl FakeFclose(u32 file) {Ev& e=PushEv(44,1);e.v[0]=file;return g.c4->closeRet;}                                                    // RwFileFunctions+8
inline u32 __cdecl FakeFputs(const char* text,u32 file) {Ev& e=PushEv(45,3);e.v[0]=FnvStr(text);e.v[1]=u32(strlen(text));e.v[2]=file;return g.c4->fputsRet;}   // RwFileFunctions+0x18
inline int __cdecl FakeSprintf(char* buf,const char* fmt,...) {                                                                                      // RwGlobals+0xF0
    va_list ap;va_start(ap,fmt);
    Ev& e=PushEv(46,17);e.v[0]=FnvStr(fmt);
    for(unsigned i=0;i<7;++i) {const double d=va_arg(ap,double);u32 w[2];memcpy(w,&d,8);e.v[1+2*i]=w[0];e.v[2+2*i]=w[1];}
    const char* name=va_arg(ap,const char*);const int num=va_arg(ap,int);va_end(ap);
    e.v[15]=FnvStr(name);e.v[16]=u32(num);
    return sprintf_s(buf,0x400,"line-%08x-%d",e.v[1]^e.v[2]^e.v[3]^e.v[4]^e.v[5]^e.v[6]^e.v[11]^e.v[12]^e.v[13]^e.v[14],num);
}
inline u32 __cdecl FakeStrlen(const char* s) {Ev& e=PushEv(47,1);e.v[0]=FnvStr(s);return u32(strlen(s));}                                          // RwGlobals+0x120
inline u32 __cdecl FakeLoad() {Ev& e=PushEv(48,1);e.v[0]=G32(kNumViews);G32(kNumViews)=g.c4->numAfter;return g.c4->loadRet;}                         // 0x619D60 (RsLoadPresetViews)
inline u32 __cdecl FakeSetView(void* camera,u32 view) {Ev& e=PushEv(49,2);e.v[0]=Tag(camera);e.v[1]=view;return g.c4->kbRet;}                    // 0x619780 (RsSetPresetView)
inline void* __cdecl FakeGetLtm(void* frame) {Ev& e=PushEv(50,1);e.v[0]=Tag(frame);return reinterpret_cast<void*>(matSelf.Obj());}               // 0x7F0990 (RwFrameGetLTM)
inline float __cdecl FakeNormalize(float* out,const float* in) {                                                                                     // 0x7ED9B0 (RwV3dNormalize)
    Ev& e=PushEv(51,3);for(unsigned i=0;i<3;++i)e.v[i]=BitsOf(in+i);
    const float k=F(g.c4->normScale);
    const float v0=in[0]*k,v1=in[1]*k,v2=in[2]*k;
    out[0]=v0;out[1]=v1;out[2]=v2;
    return 1.0f;
}
inline void __cdecl FakeErrorBox(const char* msg) {Ev& e=PushEv(52,1);e.v[0]=FnvStr(msg);}                                                         // 0x7451D0 (psErrorMessage)
inline u32 __cdecl FakeKbHandler(u32 event,const u32* param) {Ev& e=PushEv(53,2);e.v[0]=event;e.v[1]=param?*param:0xFFFFFFFFu;return g.c4->kbRet;}   // RsGlobal.keyboard.inputEventHandler
inline void CaseSavePreset(Env& env,Totals& t,unsigned n) {
    const C4 c=MakeC4(n);
    const json input=json::parse(JsonC4(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        camA.Fill(c.seed*11u+1u);matSelf.Fill(c.seed*11u+2u);entA.Fill(c.seed*11u+3u);
        camA.Put32(4,c.frameKind?(0x7FFF0000u|(c.seed&0xFFFFu)):u32(entA.Obj()));camA.Put32(0x80,c.nearPl);camA.Put32(0x84,c.farPl);
        for(unsigned i=0;i<3;++i) {matSelf.Put32(0x20+4*i,c.at[i]);matSelf.Put32(0x30+4*i,c.pos[i]);}
        G32(kNumViews)=c.numBefore;G32(kKbUsed)=c.kbUsed;G32(kKbHandler)=u32(reinterpret_cast<uintptr_t>(&FakeKbHandler));
        camA.Snap();matSelf.Snap();entA.Snap();
        TagClear();camA.Tagged();matSelf.Tagged();entA.Tagged();
        g.c4=&c;nev=0;
        const u32 args[1]{u32(camA.Obj())};
        sides[s]=Execute(env,4,0x619FA0,candidate,[&](uintptr_t entry){
            TablePatches tp;   // funciones de RenderWare de fichero y de cadenas: se restauran justo al volver (antes de comprobar las regiones)
            tp.Set(engineBase+0xC4+4,u32(reinterpret_cast<uintptr_t>(&FakeFopen)));
            tp.Set(engineBase+0xC4+8,u32(reinterpret_cast<uintptr_t>(&FakeFclose)));
            tp.Set(engineBase+0xC4+0x18,u32(reinterpret_cast<uintptr_t>(&FakeFputs)));
            tp.Set(engineBase+0xF0,u32(reinterpret_cast<uintptr_t>(&FakeSprintf)));
            tp.Set(engineBase+0x120,u32(reinterpret_cast<uintptr_t>(&FakeStrlen)));
            return Gate(entry,args,1,c.eax,c.ecx,c.edx,c.stack,4);
        });
        json obs;
        obs["al"]=sides[s].reg.eax&0xFFu;   // ningun llamador del EXE (la funcion no tiene llamadas ni punteros): AL solo como valor de retorno
        obs["changes"]=json::array({camA.Changes(),matSelf.Changes(),entA.Changes()});
        obs["events"]=EvsJson();
        rows[s]=CloseSide(env,sides[s],obs);
    }
    Finish(t,kFn[4],n,input,rows,sides);
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
                case 0: CasePlayerWants(env,totals,c); break;
                case 1: CaseNeighbour(env,totals,c); break;
                case 2: CaseLineOfSight(env,totals,c); break;
                case 3: CasePunch(env,totals,c); break;
                default: CaseSavePreset(env,totals,c); break;
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
