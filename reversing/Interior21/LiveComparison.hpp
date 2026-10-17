#pragma once
// Banco dentro del motor: gestor de interiores (lista de 21), InteriorManager_c (21 funciones).
// Original (hooks de la clase desactivados) frente a candidato con fixtures deterministas sobre el g_interiorMan real
// (0xBAF670: Init devuelve los interiores al pool del global), guardado y restaurado en cada ejecucion y al final junto
// con el bloque de g_furnitureMan (0xBAB378..0xBAD3F8), CGame::currArea y las entradas de ms_modelInfoPtrs prestadas.
// Nunca se crean objetos ni peatones: InteriorGroup_c (Init/Update/AddInterior/Setup/Exit), Interior_c (Init,
// IsPtInside, GetFurnitureEntity, GetBoundingBox), FurnitureManager_c::Exit/LoadFurniture, CWorld::FindObjectsInRange,
// CBaseModelInfo::Get2dEffect, CAnimManager::GetAnimationBlockIndex y FindPlayerPed se simulan con traza.
// Entidades, modelos, colisiones, efectos 2DFX, peatones, inteligencia, tarea y entradas/salidas son falsos.
#include <fstream>
#include <unordered_map>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../../source/game_sa/Interior/FurnitureManager_c.h"
#include "../../source/game_sa/Interior/Interior_c.h"
#include "../../source/game_sa/Interior/InteriorGroup_c.h"
#include "../../source/game_sa/Interior/InteriorManager_c.h"
#include "../../source/game_sa/Animation/AnimManager.h"
#include "../ProbeRuntime.hpp"

namespace Interior21Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline std::string Hash(const void* p,size_t n){ return std::to_string(Fnv(p,n)); }
inline uint32 Str(const char* s){ return s ? uint32(Fnv(s,strlen(s))) : 0u; }
template<class M> uintptr_t MemberAddress(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }
inline uintptr_t Body(uintptr_t p){ while(*reinterpret_cast<const uint8*>(p)==0xE9)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }
template<class T> T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- fixtures (direcciones fijas: iguales en los dos lados) ---
constexpr int NE=6, NMI=4, NFX=16, NP=4;
constexpr int32 MI_BASE=19990;                    // entradas de ms_modelInfoPtrs prestadas (se restauran)
constexpr uintptr_t IM=0xBAF670; constexpr size_t IM_N=0x4758;
constexpr uintptr_t FM_BEGIN=0xBAB378; constexpr size_t FM_N=0xBAD3F8-0xBAB378;
alignas(16) inline std::array<std::array<uint8,0x40>,NE> ents{};
alignas(16) inline std::array<std::array<uint8,sizeof(CMatrixLink)>,NE+NP> mats{};
alignas(16) inline std::array<std::array<uint8,0x40>,NMI> mis{};
alignas(16) inline std::array<std::array<uint8,0x30>,NMI> cols{};
alignas(16) inline std::array<std::array<uint8,0x40>,NMI*NFX> fxs{};
alignas(16) inline std::array<std::array<uint8,sizeof(CPed)>,NP> peds{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,0x40> task{};
alignas(16) inline std::array<std::array<uint8,0x3C>,2> enexs{};
alignas(16) inline std::array<uint8,0x40> fakeInterior{};
alignas(16) inline std::array<uint8,0x40> furnEnts{};
alignas(16) inline std::array<uint8,33*0x50> infos{};
alignas(16) inline CVector posOut{}, ptArg{};
alignas(16) inline std::array<uint8,8> rw{};
inline uint8* Ent(int k){ return ents[k].data(); }
inline uint8* Fx(int m,int i){ return fxs[(m%NMI)*NFX+(i%NFX)].data(); }
inline uint8* Ped(int k){ return peds[k].data(); }
inline uint8* Info(int k){ return infos.data()+k*0x50; }

struct Buf { const void* p; size_t n; const char* nm; };
inline std::vector<Buf> bufs;
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& b:bufs)if(v>=uintptr_t(b.p)&&v<uintptr_t(b.p)+b.n)return std::string(b.nm)+"+"+std::to_string(v-uintptr_t(b.p));
    return v;
}
inline json Region(const void* p,size_t n){ json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=0;k<n/4;++k)r.push_back(Name(w[k])); return r; }

// --- simulaciones ---
inline uint32 ptMask{}, ptCalls{}, feMask{}, feCalls{}, bbMask{}, bbCalls{}; inline int32 animIdx{}; inline bool ped1Present{};
inline int16 nObjs{}; inline std::array<int,16> objIdx{};
inline CPlayerPed* __cdecl FakeFindPed(int32 id){ trace.push_back({"find_ped",id}); return id==1 ? (ped1Present ? reinterpret_cast<CPlayerPed*>(Ped(1)) : nullptr) : reinterpret_cast<CPlayerPed*>(Ped(0)); }
inline void __cdecl FakeFindObjects(const CVector& p,float r,bool b2D,int16* outCount,int16 maxCount,CEntity** out,bool b,bool v,bool pd,bool o,bool d){
    trace.push_back({"find_objects",F(p.x),F(p.y),F(p.z),F(r),b2D,maxCount,b,v,pd,o,d});
    for(int16 k=0;k<nObjs;++k)out[k]=reinterpret_cast<CEntity*>(Ent(objIdx[k]%NE)); *outCount=nObjs; }
inline C2dEffect* __fastcall FakeGet2dEffect(void* mi,void*,int32 i){ trace.push_back({"get_2dfx",Name(uintptr_t(mi)),i});
    const int m=int((uintptr_t(mi)-uintptr_t(mis.data()))/0x40); return reinterpret_cast<C2dEffect*>(Fx(m,i)); }
inline int32 __cdecl FakeAnimBlockIndex(const char* n){ trace.push_back({"anim_block_index",Str(n)}); return animIdx; }
inline void __fastcall FakeGrpInit(void* g,void*,CEntity* e,int32 id){ trace.push_back({"grp_init",Name(uintptr_t(g)),Name(uintptr_t(e)),id}); }
inline void __fastcall FakeGrpUpdate(void* g,void*){ trace.push_back({"grp_update",Name(uintptr_t(g))}); }
inline void __fastcall FakeGrpSetup(void* g,void*){ trace.push_back({"grp_setup",Name(uintptr_t(g))}); }
inline int8 __fastcall FakeGrpExit(void* g,void*){ trace.push_back({"grp_exit",Name(uintptr_t(g))}); return 1; }
inline int32 __fastcall FakeGrpAddInterior(void* g,void*,void* i){ trace.push_back({"grp_add_interior",Name(uintptr_t(g)),Name(uintptr_t(i))}); return 1; }
inline int32 __fastcall FakeIntInit(void* i,void*,const CVector* p){ trace.push_back({"int_init",Name(uintptr_t(i)),Name(uintptr_t(p))}); return 0; }
inline bool __fastcall FakePtInside(void* i,void*,const CVector* p,CVector bias){
    trace.push_back({"pt_inside",Name(uintptr_t(i)),F(p->x),F(p->y),F(p->z),F(bias.x),F(bias.y),F(bias.z)}); return (ptMask>>(ptCalls++%32))&1; }
inline void* __fastcall FakeGetFurnEnt(void* i,void*,CEntity* e){ trace.push_back({"get_furn_entity",Name(uintptr_t(i)),Name(uintptr_t(e))});
    const uint32 c=feCalls++; return (feMask>>(c%32))&1 ? furnEnts.data()+(c%8)*4 : nullptr; }
inline bool __fastcall FakeIntBBox(void* i,void*,void* fe,CVector* pos){ trace.push_back({"int_bbox",Name(uintptr_t(i)),Name(uintptr_t(fe)),Name(uintptr_t(pos))});
    const uint32 c=bbCalls++; if(pos)*pos=CVector{float(c),float(c*2),float(c*3)}; return (bbMask>>(c%32))&1; }
inline void __fastcall FakeFurnExit(void* s,void*){ trace.push_back({"furn_exit",Name(uintptr_t(s))}); }
inline void __fastcall FakeLoadFurniture(void* s,void*){ trace.push_back({"load_furniture",Name(uintptr_t(s))}); }
// tarea falsa: GetTaskType (slot 4) trazada
inline int32 taskType{};
inline int __fastcall FType(void* t,void*){ trace.push_back({"get_task_type",Name(uintptr_t(t))}); return taskType; }
template<int N> void* __fastcall FTrap(void*,void*){ trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,12> fakeVtbl{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>){ ((fakeVtbl[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,21> fns{{
    {"InteriorManager_c","ActivatePeds",0x598080,0,-1,64},{"InteriorManager_c","AddSameGroupEffectInfos",0x598430,0,-1,1024},
    {"InteriorManager_c","AreAnimsLoaded",0x5980F0,0,-1,256},{"InteriorManager_c","Exit",0x598010,0,-1,256},
    {"InteriorManager_c","FindStealableObjectId-1",0x598360,0,-1,256},{"InteriorManager_c","FindStealableObjectId-2",0x5982F0,0,-1,512},
    {"InteriorManager_c","GetBoundingBox",0x598090,0,-1,512},{"InteriorManager_c","GetInteriorFromPool",0x5984A0,0,-1,128},
    {"InteriorManager_c","GetPedsInterior",0x598620,0,-1,512},{"InteriorManager_c","GetPedsInteriorGroup",0x598240,0,-1,512},
    {"InteriorManager_c","GetVectorsInterior",0x5983D0,0,-1,512},{"InteriorManager_c","GetVisibleEffects",0x598D80,0,-1,1024},
    {"InteriorManager_c","HasInteriorHadStealDataSetup",0x5982B0,0,-1,256},{"InteriorManager_c","Init",0x5C0500,0,-1,64},
    {"InteriorManager_c","IsGroupActive",0x598280,0,-1,256},{"InteriorManager_c","IsInteriorEffectVisible",0x598690,0,-1,1024},
    {"InteriorManager_c","PruneVisibleEffects",0x598A60,0,-1,1024},{"InteriorManager_c","ReturnInteriorToPool",0x5984B0,0,-1,128},
    {"InteriorManager_c","SetEntryExitPtr",0x598180,0,-1,512},{"InteriorManager_c","SetStealableObjectStolen",0x598390,0,-1,256},
    {"InteriorManager_c","Update",0x598F50,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"interior21-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"interior21-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("interior21");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"interior21-only.txt"); if(f)std::getline(f,only); }
    const auto& all=fns;
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(all.size());
    for(size_t f=0;f<all.size();++f)if(!(hooks[f]=FindHook(all[f].cls,all[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(all[f].cls)+"::"+all[f].name}});return;}
    std::vector<uint8> enabled(all.size()),locked(all.size());
    for(size_t f=0;f<all.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    FillTraps(std::make_integer_sequence<int,12>{}); fakeVtbl[4]=reinterpret_cast<void*>(&FType);
    bufs={{reinterpret_cast<void*>(IM),IM_N,"im"},{reinterpret_cast<void*>(FM_BEGIN),FM_N,"fm"},{ents.data(),sizeof(ents),"ent"},{mats.data(),sizeof(mats),"mat"},
          {mis.data(),sizeof(mis),"mi"},{cols.data(),sizeof(cols),"col"},{fxs.data(),sizeof(fxs),"fx"},{peds.data(),sizeof(peds),"ped"},{intel.data(),intel.size(),"intel"},
          {task.data(),task.size(),"task"},{enexs.data(),sizeof(enexs),"enex"},{fakeInterior.data(),fakeInterior.size(),"fake_interior"},{furnEnts.data(),furnEnts.size(),"furn_ent"},
          {infos.data(),infos.size(),"info"},{&posOut,sizeof(posOut),"pos_out"},{&ptArg,sizeof(ptArg),"pt_arg"},{rw.data(),rw.size(),"rw"},{&TheCamera,sizeof(CCamera),"camera"}};

    // estado global guardado
    auto* M=reinterpret_cast<uint8*>(IM); const std::vector<uint8> savedIm(M,M+IM_N);
    auto* FMb=reinterpret_cast<uint8*>(FM_BEGIN); const std::vector<uint8> savedFm(FMb,FMb+FM_N);
    auto& currArea=*reinterpret_cast<int32*>(0xB72914); const int32 savedArea=currArea;
    auto** miPtrs=reinterpret_cast<CBaseModelInfo**>(0xA9B0C8); std::array<CBaseModelInfo*,NMI> savedMi{}; for(int k=0;k<NMI;++k)savedMi[k]=miPtrs[MI_BASE+k];
    auto stateOk=[&]{ bool ok=std::memcmp(M,savedIm.data(),IM_N)==0&&std::memcmp(FMb,savedFm.data(),FM_N)==0&&currArea==savedArea;
        for(int k=0;k<NMI;++k)ok&=miPtrs[MI_BASE+k]==savedMi[k]; return ok; };
    auto restore=[&]{ std::memcpy(M,savedIm.data(),IM_N); std::memcpy(FMb,savedFm.data(),FM_N); currArea=savedArea; for(int k=0;k<NMI;++k)miPtrs[MI_BASE+k]=savedMi[k]; };
    const CVector cam=TheCamera.GetPosition();

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        for(size_t f=0;f<all.size();++f) {
            const auto& fn=all[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1A21u; if(!stt)stt=1; trace=json::array();
                    ptCalls=feCalls=bbCalls=0; ptMask=next(); feMask=next(); bbMask=next(); ped1Present=next()%3!=0;
                    const bool upd=nm=="Update";
                    // --- area actual, modelos, colisiones y efectos ---
                    static const int32 areas[]{0,1,5,13,18};
                    currArea=areas[next()%5];
                    for(int m=0;m<NMI;++m){ auto* mi=mis[m].data(); std::memset(mi,0,0x40); mi[0xD]=uint8(next()%11); At<void*>(mi,0x14)=cols[m].data(); miPtrs[MI_BASE+m]=reinterpret_cast<CBaseModelInfo*>(mi);
                        auto* c=cols[m].data(); std::memset(c,0,0x30);
                        const bool grid=next()%4==0;
                        for(int a=0;a<3;++a){ At<float>(c,4*a)=grid ? -float(1+next()%4) : uni(-10.f,-1.f); At<float>(c,0xC+4*a)=grid ? float(1+next()%4) : uni(1.f,10.f); }
                        for(int k=0;k<NFX;++k){ auto* x=Fx(m,k); std::memset(x,0,0x40);
                            for(int a=0;a<3;++a)At<float>(x,4*a)=next()%3==0 ? float(int(next()%5)-2) : uni(-5.f,5.f);
                            x[0xC]=uint8(next()%4 ? 5 : next()%10); x[0x10]=uint8(next()%3); x[0x11]=uint8(next()%5==0 ? next() : next()%3); } }
                    // --- entidades ---
                    for(int k=0;k<NE;++k){ auto* e=Ent(k); std::memset(e,0,0x40); At<int16>(e,0x22)=int16(MI_BASE+int(next()%NMI)); At<void*>(e,0x18)=next()%6 ? rw.data() : nullptr;
                        e[0x2F]=uint8(next()%3 ? currArea : areas[next()%5]); if(next()%12==0)e[0x2F]=0xFF;
                        auto* mt=mats[k].data(); std::memset(mt,0,sizeof(CMatrixLink)); CMatrix mm{};
                        if(next()%4==0){ mm.SetRotate(0.f,0.f,0.f); mm.GetPosition()=CVector{std::floor(cam.x)+float(int(next()%5)-2),std::floor(cam.y)+float(int(next()%5)-2),std::floor(cam.z)}; }
                        else { mm.SetRotate(next()%3==0 ? uni(-0.3f,0.3f) : 0.f,next()%3==0 ? uni(-0.3f,0.3f) : 0.f,uni(-3.f,3.f));
                            const float r=next()%4 ? uni(0.f,4.f) : uni(20.f,60.f); mm.GetPosition()=CVector{cam.x+uni(-1.f,1.f)*r,cam.y+uni(-1.f,1.f)*r,cam.z+uni(-2.f,2.f)}; }
                        std::memcpy(mt,&mm,sizeof(CMatrix)); At<void*>(e,0x14)=mt; At<float>(e,4)=uni(-5.f,5.f); }
                    // --- peatones (0 = jugador) ---
                    std::memset(intel.data(),0,intel.size()); std::memset(task.data(),0,task.size()); At<void*>(task.data(),0)=fakeVtbl.data();
                    for(int k=0;k<NP;++k){ auto* p=Ped(k); std::memset(p,0,sizeof(CPed)); At<void*>(p,0x47C)=intel.data(); p[0x2F]=uint8(next()%5 ? 1+next()%4 : 0);
                        CVector pos{cam.x+uni(-6.f,6.f),cam.y+uni(-6.f,6.f),cam.z+uni(-3.f,3.f)};
                        if(next()%5==0)pos=CVector{std::floor(cam.x)+float(int(next()%11)-5),std::floor(cam.y)+float(int(next()%11)-5),std::floor(cam.z)+float(int(next()%5)-2)};
                        if(next()%3){ auto* mt=mats[NE+k].data(); CMatrix mm{}; mm.SetRotate(0.f,0.f,uni(-3.f,3.f)); mm.GetPosition()=pos; std::memcpy(mt,&mm,sizeof(CMatrix)); At<void*>(p,0x14)=mt; }
                        else { At<float>(p,4)=pos.x; At<float>(p,8)=pos.y; At<float>(p,0xC)=pos.z; } }
                    { const uint32 t=next()%6; if(t){ At<void*>(intel.data(),4+4*(next()%5))=task.data(); } taskType=t==1 ? TASK_SIMPLE_CAR_DRIVE : int32(next()%5==0 ? next()%1000 : 700+next()%20); }
                    // --- gestor de interiores ---
                    for(size_t k=0;k<IM_N;++k)M[k]=uint8(next());
                    auto Interior=[&](int k){ return M+k*0x794; }; auto Group=[&](int k){ return M+0x3CAC+k*0xBC; };
                    for(int k=0;k<8;++k)At<int32>(Interior(k),8)=int32(next()%7);
                    auto build=[&](size_t off,const std::vector<uint8*>& nodes){ uint8* prev=nullptr;
                        for(auto* n:nodes){ At<uint8*>(n,0)=prev; At<uint8*>(n,4)=nullptr; if(prev)At<uint8*>(prev,4)=n; prev=n; }
                        At<uint8*>(M,off)=nodes.empty() ? nullptr : nodes.front(); At<uint8*>(M,off+4)=prev; At<uint32>(M,off+8)=uint32(nodes.size()); };
                    std::array<int,8> go{0,1,2,3,4,5,6,7}, io{0,1,2,3,4,5,6,7};
                    for(int k=7;k>0;--k){ std::swap(go[k],go[next()%(k+1)]); std::swap(io[k],io[next()%(k+1)]); }
                    const int nl=int(next()%9), np=upd ? 8-nl : int(next()%(9-nl)), nip=int(next()%9);
                    { std::vector<uint8*> a,b,c; for(int k=0;k<nl;++k)a.push_back(Group(go[k])); for(int k=nl;k<nl+np;++k)b.push_back(Group(go[k]));
                      for(int k=0;k<nip;++k)c.push_back(Interior(io[k])); build(0x428C,a); build(0x4298,b); build(0x3CA0,c); }
                    static const uint8 gtypes[]{0,1,2,3,0x80,0xFF};
                    for(int k=0;k<8;++k){ auto* g=Group(go[k]);
                        At<void*>(g,8)=upd ? Ent(k%NE) : Ent(int(next()%NE)); g[0xC]=uint8(go[k]); g[0xD]=uint8(upd ? k/NE : next()%3); g[0xE]=gtypes[next()%6]; g[0xF]=uint8(next()%9);
                        for(int s=0;s<8;++s)At<uint8*>(g,0x10+4*s)=next()%3==0 ? nullptr : Interior(nip<8 ? io[nip+int(next()%(8-nip))] : int(next()%8));
                        g[0x36]=uint8(next()%8==0 ? int8(-1-int(next()%3)) : int8(next()%17));
                        for(int s=0;s<16;++s)At<uint8*>(g,0x38+4*s)=next()%3==0 ? nullptr : Ped(int(next()%NP)); }
                    At<int32>(M,0x42A4)=next()%8==0 ? -int32(next()%3)-1 : int32(next()%65);
                    for(int k=0;k<64;++k)At<int32>(M,0x42A8+4*k)=int32(next()%9);
                    At<int32>(M,0x43A8)=next()%8==0 ? -int32(next()%3)-1 : int32(next()%33);
                    static const float grid[]{0.f,1.f,-2.5f,100.f};
                    for(int k=0;k<32;++k){ auto* o=M+0x43AC+k*0x1C; At<void*>(o,0)=next()%4 ? Ent(int(next()%NE)) : (void*)Ped(int(next()%NP)); At<int32>(o,4)=int32(next()%3); At<int32>(o,8)=int32(next()%3);
                        for(int a=0;a<3;++a)At<float>(o,0xC+4*a)=next()%6==0 ? std::numeric_limits<float>::quiet_NaN() : grid[next()%4]; }
                    for(int a=0;a<4;++a)At<float>(M,0x4740+4*a)=float(int(next()%7)-3);
                    M[0x4750]=uint8(upd ? next()%6==0 : next()%2); M[0x4751]=uint8(next()%6!=0); M[0x4752]=uint8(next()%2);

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    {
                        EntryPatch s1(Body(bit_cast<uintptr_t>(&FindPlayerPed)),FakeFindPed), s1e(0x56E210,FakeFindPed);
                        EntryPatch s2(Body(bit_cast<uintptr_t>(&CWorld::FindObjectsInRange)),FakeFindObjects), s2e(0x564A20,FakeFindObjects);
                        EntryPatch s3(Body(MemberAddress(&CBaseModelInfo::Get2dEffect)),FakeGet2dEffect), s3e(0x4C4C70,FakeGet2dEffect);
                        EntryPatch s4(Body(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CAnimManager::GetAnimationBlockIndex))),FakeAnimBlockIndex), s4e(0x4D3990,FakeAnimBlockIndex);
                        EntryPatch s5(Body(MemberAddress(&FurnitureManager_c::Exit)),FakeFurnExit), s5e(0x5911A0,FakeFurnExit);
                        EntryPatch s6(Body(MemberAddress(&FurnitureManager_c::LoadFurniture)),FakeLoadFurniture), s6e(0x5C0280,FakeLoadFurniture);
                        // direcciones del EXE (las de la DLL se interceptan abajo)
                        EntryPatch g1(0x5947E0,FakeGrpInit), g2(0x5968E0,FakeGrpUpdate), g3(0x595320,FakeGrpSetup), g4(0x595290,FakeGrpExit), g5(0x594840,FakeGrpAddInterior);
                        EntryPatch n1(0x593BF0,FakeIntInit), n2(0x5913E0,FakePtInside), n3(0x5913B0,FakeGetFurnEnt), n4(0x593DB0,FakeIntBBox);
                        // InteriorGroup_c e Interior_c ya son C++ en la DLL: InteriorManager_c los llama directamente (no por el EXE), asi que tambien
                        // hay que interceptar sus entradas de DLL con los mismos falsos (de lo contrario el candidato ejecuta Exit/Unfurnish reales sobre el fixture aleatorio).
                        EntryPatch gd1(Body(MemberAddress(&InteriorGroup_c::Init)),FakeGrpInit), gd2(Body(MemberAddress(&InteriorGroup_c::Update)),FakeGrpUpdate),
                                   gd3(Body(MemberAddress(&InteriorGroup_c::Setup)),FakeGrpSetup), gd4(Body(MemberAddress(&InteriorGroup_c::Exit)),FakeGrpExit),
                                   gd5(Body(MemberAddress(&InteriorGroup_c::AddInterior)),FakeGrpAddInterior);
                        EntryPatch nd1(Body(MemberAddress(&Interior_c::Init)),FakeIntInit), nd2(Body(MemberAddress(&Interior_c::IsPtInside)),FakePtInside),
                                   nd3(Body(MemberAddress(&Interior_c::GetFurnitureEntity)),FakeGetFurnEnt), nd4(Body(MemberAddress(&Interior_c::GetBoundingBox)),FakeIntBBox);
                        void* const self=M;
                        if(nm=="ActivatePeds")reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(self,(next()&0xFFFFFF00u)|(next()%2));
                        else if(nm=="AddSameGroupEffectInfos"||nm=="PruneVisibleEffects"||nm=="GetVisibleEffects"){
                            std::memset(infos.data(),0xCD,infos.size());
                            const int n=nm=="AddSameGroupEffectInfos" ? 1 : int(next()%13);
                            for(int k=0;k<n;++k){ auto* in=Info(k); const int e=int(next()%NE); At<void*>(in,0)=Ent(e);
                                const int m=At<int16>(Ent(e),0x22)-MI_BASE;
                                At<int32>(in,4)=nm=="PruneVisibleEffects"&&next()%12==0 ? -1 : int32(next()%9);
                                for(int s=0;s<8;++s){ At<void*>(in,8+4*s)=Fx(next()%3 ? m : int(next()%NMI),int(next()%NFX)); At<uint32>(in,0x28+4*s)=next()%12; }
                                At<float>(in,0x48)=uni(0.f,500.f); in[0x4C]=uint8(next()%2); }
                            if(nm=="AddSameGroupEffectInfos")reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(self,Info(0),int32(next()%40));
                            else if(nm=="PruneVisibleEffects"){ const uint32 req=next()%10==0 ? 0x80000000u|next() : next()%12;
                                float md=uni(0.f,40.f); if(next()%10==0)md=std::numeric_limits<float>::quiet_NaN(); else if(next()%10==0)md=-md;
                                reinterpret_cast<void(__thiscall*)(void*,void*,uint32,uint32,float)>(a)(self,infos.data(),uint32(n),req,md); out["n"]=n; }
                            else { nObjs=int16(next()%8==0 ? 0 : next()%10); for(auto& x:objIdx)x=int(next()%NE);
                                const uint32 mx=next()%6==0 ? next()%3 : next()%10==0 ? 32 : next()%12;
                                ret=reinterpret_cast<uint32(__thiscall*)(void*,void*,uint32)>(a)(self,infos.data(),mx); }
                            out["infos"]=Region(infos.data(),infos.size()); }
                        else if(nm=="AreAnimsLoaded"){ animIdx=next()%10==0 ? -1 : int32(next()%180); const int32 arg=next()%4 ? int32(next()%3) : int32(next()%180);
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,arg))); }
                        else if(nm=="Exit")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="FindStealableObjectId-1"){ void* e=next()%4 ? At<void*>(M,0x43AC+(next()%32)*0x1C) : (void*)Ent(int(next()%NE));
                            ret=reinterpret_cast<int32(__thiscall*)(void*,void*)>(a)(self,e); }
                        else if(nm=="FindStealableObjectId-2"){ auto* o=M+0x43AC+(next()%32)*0x1C; int32 iid=At<int32>(o,8), mid=At<int32>(o,4);
                            CVector p{At<float>(o,0xC),At<float>(o,0x10),At<float>(o,0x14)};
                            if(next()%4==0)iid=int32(next()%3); if(next()%4==0)mid=int32(next()%3); if(next()%4==0)(&p.x)[next()%3]=grid[next()%4];
                            ret=reinterpret_cast<int32(__thiscall*)(void*,int32,int32,CVector)>(a)(self,iid,mid,p); }
                        else if(nm=="GetBoundingBox"){ posOut=CVector{-1.f,-1.f,-1.f};
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,void*,CVector*)>(a)(self,Ent(int(next()%NE)),&posOut))); out["pos"]={F(posOut.x),F(posOut.y),F(posOut.z)}; }
                        else if(nm=="GetInteriorFromPool")ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)));
                        else if(nm=="GetPedsInterior"||nm=="GetPedsInteriorGroup")ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(self,Ped(int(next()%NP)))));
                        else if(nm=="GetVectorsInterior"){ ptArg=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,5.f)};
                            ret=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*,const CVector*)>(a)(self,&ptArg))); }
                        else if(nm=="HasInteriorHadStealDataSetup"){ std::memset(fakeInterior.data(),0,fakeInterior.size()); At<int32>(fakeInterior.data(),8)=int32(next()%10);
                            void* in=next()%4 ? (void*)Interior(int(next()%8)) : (void*)fakeInterior.data();
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,void*)>(a)(self,in))); }
                        else if(nm=="Init"){ for(size_t off:{size_t(0x3CA0),size_t(0x428C),size_t(0x4298)}){ At<void*>(M,off)=nullptr; At<void*>(M,off+4)=nullptr; At<uint32>(M,off+8)=0; }
                            auto* fl=reinterpret_cast<uint8*>(0xBAD3EC); std::memset(fl,0,0xC); At<uint64>(FMb,0)=(uint64(next())<<32)|next();
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*)>(a)(self))); }
                        else if(nm=="IsGroupActive"){ static const int32 gt[]{0,1,2,3,-1,-128,128,255}; ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(self,gt[next()%8]))); }
                        else if(nm=="IsInteriorEffectVisible")ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*,void*,void*)>(a)(self,Fx(int(next()%NMI),int(next()%NFX)),Ent(int(next()%NE)))));
                        else if(nm=="ReturnInteriorToPool"){ std::vector<void*> c; for(int k=nip;k<8;++k)c.push_back(Interior(io[k]));
                            void* in=c.empty() ? (void*)fakeInterior.data() : c[next()%c.size()]; if(in==fakeInterior.data())std::memset(fakeInterior.data(),0xCD,fakeInterior.size());
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,in); out["fake_interior"]=Region(fakeInterior.data(),8); }
                        else if(nm=="SetEntryExitPtr"){ for(auto& e:enexs){ std::memset(e.data(),0,e.size()); for(int k=0;k<4;++k)At<float>(e.data(),8+4*k)=At<float>(M,0x4740+4*k);
                                if(next()%2)At<float>(e.data(),8+4*(next()%4))=float(int(next()%7)-3); if(next()%10==0)At<float>(e.data(),8+4*(next()%4))=std::numeric_limits<float>::quiet_NaN();
                                e[0x32]=uint8(next()%3 ? 1+next()%18 : 0); }
                            At<void*>(enexs[0].data(),0x38)=next()%3==0 ? enexs[1].data() : nullptr;
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,enexs[0].data()); }
                        else if(nm=="SetStealableObjectStolen"){ void* e=next()%4 ? At<void*>(M,0x43AC+(next()%32)*0x1C) : (void*)Ent(int(next()%NE));
                            reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(a)(self,e,next()); }
                        else { // Update
                            nObjs=int16(next()%8==0 ? 0 : next()%10); for(auto& x:objIdx)x=int(next()%NE);
                            ret=int(uint8(reinterpret_cast<uint32(__thiscall*)(void*)>(a)(self))); }
                    }
                    for(auto* h:hooks)h->State(false);
                    out["im"]=Hash(M,IM_N); out["lists"]=json::array({Region(M+0x3CA0,0xC),Region(M+0x428C,0x18)});
                    { json g=json::array(); for(int k=0;k<8;++k)g.push_back(Region(Group(k),0x38)); out["groups"]=g; }
                    { json n=json::array(); for(int k=0;k<8;++k)n.push_back(Region(Interior(k),0x18)); out["interiors"]=n; }
                    out["tail"]=Region(M+0x472C,IM_N-0x472C); out["counts"]={At<int32>(M,0x42A4),At<int32>(M,0x43A8)};
                    out["objects_stolen"]=json::array(); for(int k=0;k<32;++k)out["objects_stolen"].push_back(M[0x43AC+k*0x1C+0x18]);
                    out["fm"]=Hash(FMb,FM_N); out["fm_head"]=Region(FMb,0x14); out["fm_list"]=Region(reinterpret_cast<void*>(0xBAD3EC),0xC);
                    restore();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<all.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restore();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateOk()}});
}
}
