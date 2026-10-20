#pragma once
// Casos del banco WaterLevel62 (continua el espacio de nombres de Core.hpp): falsos de los colaboradores y las cinco funciones de caso.
#include "Core.hpp"

namespace Water62Live {
// ================================================================================================ estado de los falsos
struct FakeState {
    // 0: AddToQuadsAndTrianglesList
    u32 trigKind{}, trigAt{}, trigVal{}, calls{}, count{};
    u64 hash{};
    std::array<std::array<u32,4>,24> first{};
    // 1: GetWaterDepth
    u32 retA{}, retG{}, writeLevel{}, level{}, ground{};
    uintptr_t obj{};
    // 2 y 3: IsSphereVisible
    u32 ret1{}, ret2{}, sphereCalls{};
};
inline FakeState g;
inline u32 NearStack(const void* p, const void* local) {const auto d=intptr_t(uintptr_t(p))-intptr_t(uintptr_t(local));return d>-0x4000&&d<0x4000;}
// CRenPar por valor tal como viaja en la pila: z, bigWaves, smallWaves y la palabra flowX | flowY << 8 | relleno << 16 (el relleno no se compara)
struct RP {u32 z,big,sm,flow;};
inline void LogRP(Ev& e,unsigned at,const RP& p) {e.v[at]=p.z;e.v[at+1]=p.big;e.v[at+2]=p.sm;e.v[at+3]=p.flow&0xFFFFu;}

// ================================================================================================ 0: FillQuadsAndTrianglesList
inline void __cdecl FakeAdd(i32 a,i32 b,i32 idx,i32 type) {
    const u32 vals[4]{u32(a),u32(b),u32(idx),u32(type)};
    for(u32 v:vals)for(int k=0;k<4;++k){g.hash^=(v>>(8*k))&0xFFu;g.hash*=0x100000001B3ull;}
    if(g.count<g.first.size())g.first[g.count]={vals[0],vals[1],vals[2],vals[3]};
    ++g.count;++g.calls;
    if(g.trigKind&&g.calls==g.trigAt)SetCount(g.trigKind==1?kRNumQ:kRNumT,g.trigVal);
}
inline void ApplyA0(const A0& c) {
    memset(Mem(kRVerts),0x5A,regions[kRVerts].bytes);memset(Mem(kRQuads),0xA5,regions[kRQuads].bytes);memset(Mem(kRTris),0xA5,regions[kRTris].bytes);
    for(size_t v=0;v<c.vx.size();++v)PutVertex(i32(v),c.vx[v],c.vy[v]);
    PutVertex(-1,c.vm1x,c.vm1y);
    for(size_t i=0;i<c.quads.size();++i)PutWord(kRQuads,i,c.quads[i]);
    for(size_t i=0;i<c.tris.size();++i)PutWord(kRTris,i,c.tris[i]);
    SetCount(kRNumQ,c.numQ);SetCount(kRNumT,c.numT);SetCount(kRNumV,u32(c.vx.size()));
}
inline void CaseFill(Env& env,Totals& t,unsigned n) {
    const A0 c=MakeA0(n);
    const json input=json::parse(JsonA0(c));
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        ApplyA0(c);
        const AtlasRef ref=SnapAtlas();
        g.trigKind=c.trigKind;g.trigAt=c.trigAt;g.trigVal=c.trigVal;g.calls=0;g.count=0;g.hash=0xCBF29CE484222325ull;g.first={};
        sides[s]=Execute(env,0,0x6E7B30,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,c.ecx,c.edx,c.stack,0);});
        json obs;
        json first=json::array();for(u32 i=0;i<g.count&&i<g.first.size();++i)first.push_back(json::array({g.first[i][0],g.first[i][1],g.first[i][2],g.first[i][3]}));
        char hex[17];snprintf(hex,sizeof(hex),"%016llx",static_cast<unsigned long long>(g.hash));
        obs["count"]=g.count;obs["hash"]=std::string(hex);obs["first"]=first;
        obs["num_q_after"]=GetCount(kRNumQ);obs["num_t_after"]=GetCount(kRNumT);obs["atlas_unchanged"]=AtlasUnchanged(ref);
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        RestoreRegions();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

// ================================================================================================ 1: GetWaterDepth
alignas(16) inline std::array<uint8,kDepthArena> depthArena{},depthRef{};
inline u32 __cdecl FakeLevel(u32 x,u32 y,u32 z,u32* level,void* big,void* smallw) {
    Ev& e=PushEv(1,6);e.v[0]=x;e.v[1]=y;e.v[2]=z;const char local=0;e.v[3]=NearStack(level,&local);e.v[4]=big?1:0;e.v[5]=smallw?1:0;
    if(g.writeLevel)*level=g.level;   // copia de 32 bits: conserva un SNaN
    return g.retA;
}
inline u32 __cdecl FakeGround(const u32* pos,u32* out,void* col,u32 dist) {
    Ev& e=PushEv(2,4);const char local=0;e.v[0]=uintptr_t(pos)==g.obj?1:0;e.v[1]=NearStack(out,&local);e.v[2]=col?1:0;e.v[3]=dist;
    *out=g.ground;
    return g.retG;
}
inline void CaseDepth(Env& env,Totals& t,unsigned n) {
    const B1 c=MakeB1(n);
    FillWords(depthRef.data(),kDepthArena,Rand(n,50));
    uint8* obj=depthArena.data()+kDepthObj;uint8* ro=depthRef.data()+kDepthObj;
    for(u32 i=0;i<3;++i)At<u32>(ro,4*i)=c.pos[i];
    g.obj=uintptr_t(obj);g.retA=c.retA;g.retG=c.retG;g.writeLevel=c.writeLevel;g.level=c.level;g.ground=c.ground;
    const json input=json::parse(JsonB1(c));
    u32 args[4]{u32(uintptr_t(obj)),0,0,0};
    for(u32 i=0;i<3;++i)args[1+i]=c.out[i]==kNullOut?0u:u32(uintptr_t(obj)+c.out[i]);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetArena(depthArena,depthRef);nev=0;
        sides[s]=Execute(env,1,0x6EA960,candidate,[&](uintptr_t entry){return Gate(entry,args,4,c.eax,c.ecx,c.edx,c.stack,16);});
        json obs;
        obs["events"]=EvsJson();obs["nevents"]=nev;obs["ret_al"]=sides[s].reg.eax&0xFFu;
        json words=json::array();for(u32 i=0;i<kDepthObjBytes/4;++i)words.push_back(At<u32>(obj,4*i));
        obs["obj"]=words;obs["obj_ptr"]=u32(uintptr_t(obj));
        bool outside=true;
        for(u32 i=0;i<kDepthArena;++i)if((i<kDepthObj||i>=kDepthObj+kDepthObjBytes)&&depthArena[i]!=depthRef[i])outside=false;
        obs["outside_unchanged"]=outside;
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    g.obj=0;
    Finish(t,kFn[1],n,input,rows,sides);
}

// ================================================================================================ 2 y 3: RenderHighDetailWaterRectangle / Triangle
inline u32 __fastcall FakeSphere(void* self,void*,const u32* center,u32 radius,void* matrix) {
    Ev& e=PushEv(1,6);e.v[0]=u32(uintptr_t(self));e.v[1]=center[0];e.v[2]=center[1];e.v[3]=center[2];e.v[4]=radius;e.v[5]=u32(uintptr_t(matrix));
    return g.sphereCalls++==0?g.ret1:g.ret2;
}
inline void __cdecl FakeFog(i32 a,i32 b,i32 c,i32 d) {Ev& e=PushEv(2,4);e.v[0]=u32(a);e.v[1]=u32(b);e.v[2]=u32(c);e.v[3]=u32(d);}
inline void __cdecl FakeRectLayer(i32 a,i32 b,i32 c,i32 d,RP p1,RP p2,RP p3,RP p4,i32 layer,i32 w,i32 x,i32 y,i32 z) {
    Ev& e=PushEv(3,25);e.v[0]=u32(a);e.v[1]=u32(b);e.v[2]=u32(c);e.v[3]=u32(d);LogRP(e,4,p1);LogRP(e,8,p2);LogRP(e,12,p3);LogRP(e,16,p4);
    e.v[20]=u32(layer);e.v[21]=u32(w);e.v[22]=u32(x);e.v[23]=u32(y);e.v[24]=u32(z);
}
inline void __cdecl FakeSplitX(i32 split,i32 a,i32 b,i32 c,i32 d,RP p1,RP p2,RP p3,RP p4) {
    Ev& e=PushEv(4,21);e.v[0]=u32(split);e.v[1]=u32(a);e.v[2]=u32(b);e.v[3]=u32(c);e.v[4]=u32(d);LogRP(e,5,p1);LogRP(e,9,p2);LogRP(e,13,p3);LogRP(e,17,p4);
}
inline void __cdecl FakeRenderRect(i32 a,i32 b,i32 c,i32 d,RP p1,RP p2,RP p3,RP p4) {
    Ev& e=PushEv(5,20);e.v[0]=u32(a);e.v[1]=u32(b);e.v[2]=u32(c);e.v[3]=u32(d);LogRP(e,4,p1);LogRP(e,8,p2);LogRP(e,12,p3);LogRP(e,16,p4);
}
inline void __cdecl FakeTriLayer(i32 x1,i32 y1,RP p1,i32 x2,i32 y2,RP p2,i32 x3,i32 y3,RP p3,i32 layer,i32 a,i32 b,i32 c) {
    Ev& e=PushEv(6,22);e.v[0]=u32(x1);e.v[1]=u32(y1);LogRP(e,2,p1);e.v[6]=u32(x2);e.v[7]=u32(y2);LogRP(e,8,p2);e.v[12]=u32(x3);e.v[13]=u32(y3);LogRP(e,14,p3);
    e.v[18]=u32(layer);e.v[19]=u32(a);e.v[20]=u32(b);e.v[21]=u32(c);
}
inline void __cdecl FakeSplitTri(i32 split,i32 x1,i32 y1,RP p1,i32 x2,i32 y2,RP p2,i32 x3,i32 y3,RP p3) {
    Ev& e=PushEv(7,19);e.v[0]=u32(split);e.v[1]=u32(x1);e.v[2]=u32(y1);LogRP(e,3,p1);e.v[7]=u32(x2);e.v[8]=u32(y2);LogRP(e,9,p2);e.v[13]=u32(x3);e.v[14]=u32(y3);LogRP(e,15,p3);
}
// CRenPar completo (16 bytes) para la pila: el relleno lleva un valor fijo reconocible
inline void PushRen(std::vector<u32>& args,const Ren& r) {args.push_back(r.z);args.push_back(r.big);args.push_back(r.sm);args.push_back((r.flow&0xFFFFu)|0xCDCD0000u);}
inline void CaseRect(Env& env,Totals& t,unsigned n) {
    const C2 c=MakeC2(n);
    g.ret1=c.ret1;g.ret2=c.ret2;
    const json input=json::parse(JsonC2(c));
    std::vector<u32> args{u32(c.ints[0]),u32(c.ints[1]),u32(c.ints[2]),u32(c.ints[3])};
    for(const Ren& r:c.p)PushRen(args,r);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        *Mem(kRMirror)=uint8(c.mirror);g.sphereCalls=0;nev=0;
        sides[s]=Execute(env,2,0x6EB810,candidate,[&](uintptr_t entry){return Gate(entry,args.data(),u32(args.size()),c.eax,c.ecx,c.edx,c.stack,u32(args.size()*4));});
        json obs;
        obs["events"]=EvsJson();obs["nevents"]=nev;
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        const bool mirrorKept=*Mem(kRMirror)==uint8(c.mirror);
        RestoreRegions();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact()&&mirrorKept;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[2],n,input,rows,sides);
}
inline void CaseTri(Env& env,Totals& t,unsigned n) {
    const C3 c=MakeC3(n);
    g.ret1=c.ret1;g.ret2=c.ret2;
    const json input=json::parse(JsonC3(c));
    std::vector<u32> args;
    for(u32 i=0;i<3;++i){args.push_back(u32(c.ints[2*i]));args.push_back(u32(c.ints[2*i+1]));PushRen(args,c.p[i]);}
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        *Mem(kRMirror)=uint8(c.mirror);g.sphereCalls=0;nev=0;
        sides[s]=Execute(env,3,0x6EDDC0,candidate,[&](uintptr_t entry){return Gate(entry,args.data(),u32(args.size()),c.eax,c.ecx,c.edx,c.stack,u32(args.size()*4));});
        json obs;
        obs["events"]=EvsJson();obs["nevents"]=nev;
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        const bool mirrorKept=*Mem(kRMirror)==uint8(c.mirror);
        RestoreRegions();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact()&&mirrorKept;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[3],n,input,rows,sides);
}

// ================================================================================================ 4: TestLineAgainstWater
alignas(16) inline std::array<uint8,kLineArena> lineArena{},lineRef{};
inline void ApplyE4(const E4& c) {
    memset(Mem(kRVerts),0x5A,regions[kRVerts].bytes);memset(Mem(kRQuads),0xA5,regions[kRQuads].bytes);memset(Mem(kRTris),0xA5,regions[kRTris].bytes);
    memset(Mem(kRBlocks),0,regions[kRBlocks].bytes);memset(Mem(kRCombos),0,regions[kRCombos].bytes);
    for(size_t v=0;v<c.vx.size();++v)PutVertex(i32(v),c.vx[v],c.vy[v]);
    PutVertex(-1,c.vm1x,c.vm1y);
    for(size_t i=0;i<c.quads.size();++i)PutWord(kRQuads,i,c.quads[i]);
    for(size_t i=0;i<c.tris.size();++i)PutWord(kRTris,i,c.tris[i]);
    for(size_t i=0;i<c.blockIdx.size();++i)PutWord(kRBlocks,c.blockIdx[i],c.blockVal[i]);
    for(size_t i=0;i<c.comboVal.size();++i)PutWord(kRCombos,i,c.comboVal[i]);
    SetCount(kRNumQ,u32(c.quads.size()/5));SetCount(kRNumT,u32(c.tris.size()/4));SetCount(kRNumV,u32(c.vx.size()));
}
inline void CaseLine(Env& env,Totals& t,unsigned n) {
    const E4 c=MakeE4(n);
    FillWords(lineRef.data(),kLineArena,Rand(n,53));
    uint8* obj=lineArena.data()+kLineObj;uint8* ro=lineRef.data()+kLineObj;
    for(u32 i=0;i<3;++i)At<u32>(ro,4*i)=c.outInit[i];
    const json input=json::parse(JsonE4(c));
    const u32 args[7]{c.from[0],c.from[1],c.from[2],c.to[0],c.to[1],c.to[2],u32(uintptr_t(obj))};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        SetArena(lineArena,lineRef);ApplyE4(c);
        const AtlasRef ref=SnapAtlas();
        sides[s]=Execute(env,4,0x6E61B0,candidate,[&](uintptr_t entry){return Gate(entry,args,7,c.eax,c.ecx,c.edx,c.stack,28);});
        json obs;
        obs["ret_al"]=sides[s].reg.eax&0xFFu;
        obs["out"]=json::array({At<u32>(obj,0),At<u32>(obj,4),At<u32>(obj,8)});
        bool outside=At<u32>(obj,12)==At<u32>(ro,12);
        for(u32 i=0;i<kLineArena;++i)if((i<kLineObj||i>=kLineObj+kLineObjBytes)&&lineArena[i]!=lineRef[i])outside=false;
        obs["outside_unchanged"]=outside;
        obs["atlas_unchanged"]=AtlasUnchanged(ref);
        if(kCompareVolatileRegisters){obs["eax"]=sides[s].reg.eax;obs["ecx"]=sides[s].reg.ecx;obs["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        RestoreFloatingState();
        RestoreRegions();
        out["globals_restored"]=env.WatchesIntact()&&RegionsIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kFn[4],n,input,rows,sides);
}

// ================================================================================================ ejecucion del banco
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
                case 0: CaseFill(env,totals,c); break;
                case 1: CaseDepth(env,totals,c); break;
                case 2: CaseRect(env,totals,c); break;
                case 3: CaseTri(env,totals,c); break;
                default: CaseLine(env,totals,c); break;
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
