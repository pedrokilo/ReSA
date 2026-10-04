#pragma once
// Banco dentro del motor: gestor de efectos FxManager_c (lista de 25; 23 funciones). Original (hooks de FxManager_c
// desactivados) frente a candidato con fixtures deterministas. Cada caso trabaja sobre un FxManager_c falso o sobre
// g_fxMan con su contenido sustituido (las dos variantes: el EXE usa el gestor global en linea en ShouldCreate,
// CreateFxSystem-3, DestroyFxSystem y en los operator new de FxEmitterPrt_c/FxSystemBP_c) y se restaura tras cada
// ejecucion. Blueprints, primitivas, sistemas, particulas, matrices, camara y pools FX son buferes falsos de direccion
// fija. Se simulan y trazan sin reservar memoria (registro fijo convertido despues): rand, FxMemoryPool_c::Init/Exit/
// Optimise, RwMatrixCreate/Destroy, operator new/delete (solo durante la llamada; otros tamanos se reenvian),
// FxSystem_c::Init/Exit/SetRateMult, FxSystemBP_c::Load/Render/FreePrtFromSystem, CTxdStore, CFileMgr (Open/ReadLine/
// Close con guion de lineas) y el estado de render (puntero fpRenderStateSet). Nunca se crean efectos ni se leen
// ficheros reales. Ya validadas aparte: DestroyAllFxSystems y Update (banco FxManager).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "../../source/game_sa/Fx/Fx.h"
#include "../../source/game_sa/Fx/FxManager.h"
#include "../../source/game_sa/Fx/FxSystem.h"
#include "../../source/game_sa/Fx/FxSystemBP.h"
#include "../../source/game_sa/Fx/FxPrimBP.h"
#include "../../source/game_sa/Fx/FxEmitterPrt.h"
#include "../../source/game_sa/Fx/FxMemoryPool.h"
#include "../../source/game_sa/TxdStore.h"
#include "../../source/game_sa/FileMgr.h"
#include "../ProbeRuntime.hpp"

namespace Fx25Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> inline T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }
inline uint64 Fnv(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 Hs(const char* s){ return s ? uint32(Fnv(s,std::strlen(s))) : 0u; }

constexpr uintptr_t FXMAN=0xA9AE80; constexpr size_t MGR_N=0xBC; constexpr size_t POOL_N=0x10400;

// --- buferes falsos (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,0x100> fakeMgr{};
alignas(16) inline std::array<uint8,POOL_N> poolA{}, poolB{};          // pool de g_fxMan / pool del gestor falso
alignas(16) inline std::array<std::array<uint8,0x40>,24> mats{};      // 0-7 pila de g_fxMan, 8-15 pila falsa, 16-23 RwMatrixCreate
alignas(16) inline std::array<std::array<uint8,0x40>,8> bps{};
alignas(16) inline std::array<std::array<uint8,0x40>,16> prims{};
alignas(16) inline std::array<std::array<void*,4>,8> primArr{};
alignas(16) inline std::array<std::array<uint8,0x110>,12> systems{};  // 11 = sistema ajeno (fuera de las listas)
alignas(16) inline std::array<std::array<uint8,0x40>,64> prts{};      // 48-63 libres (ReturnParticle)
alignas(16) inline std::array<std::array<uint8,0x20>,3> spheres{};
alignas(16) inline std::array<uint8,0x200> camBuf{};
alignas(16) inline std::array<uint8,0x100> frameBuf{};
alignas(16) inline std::array<uint8,0x110> arena{};                   // operator new(0x104) de FxSystem_c
alignas(16) inline std::array<uint8,0x40> xform{}, objMat{};
alignas(16) inline std::array<float,4> windSpeed{};

// --- registro sin reservas (los stubs pueden correr con operator new sustituido) ---
struct Rec { uint32 t, a, b, c; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c}); return r; }

// --- stubs ---
inline uint32 rngState{}, rngCalls{};
inline int __cdecl FakeRand(){ const int r=rngCalls>=64 ? 0 : int((rngState=rngState*1103515245u+12345u)>>16)&0x7FFF; ++rngCalls; Push(12,uint32(r)); return r; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline bool __fastcall FakePoolInit(FxMemoryPool_c* pool,void*){ Push(1,U(pool));
    pool->m_pData=pool==&g_fxMan.m_Pool ? poolA.data() : poolB.data(); pool->m_nSize=0x10000; pool->m_nPosition=0; return true; }
inline void __fastcall FakePoolExit(FxMemoryPool_c* pool,void*){ Push(2,U(pool)); }
inline void __fastcall FakePoolOptimise(FxMemoryPool_c* pool,void*){ Push(3,U(pool),uint32(pool->m_nPosition)); pool->m_nSize=pool->m_nPosition; }
inline uint32 matCreates{};
inline RwMatrix* __cdecl FakeMatCreate(){ auto* m=reinterpret_cast<RwMatrix*>(mats[16+matCreates++%8].data()); Push(4,U(m)); return m; }
inline RwBool __cdecl FakeMatDestroy(RwMatrix* m){ Push(5,U(m)); return TRUE; }
inline ReversingProbe::EntryPatch* newPatch{}; inline ReversingProbe::EntryPatch* delPatch{};
inline bool Ours(const void* p){ const auto u=uintptr_t(p);
    return (u>=uintptr_t(arena.data())&&u<uintptr_t(arena.data())+arena.size())||(u>=uintptr_t(systems.data())&&u<uintptr_t(systems.data())+sizeof(systems)); }
inline void* __cdecl FakeNew(size_t n){
    if(n==0x104){ Push(6,uint32(n)); return arena.data(); }
    newPatch->Enable(false); void* p=reinterpret_cast<void*(__cdecl*)(size_t)>(0x82119A)(n); newPatch->Enable(true); return p; }
inline void __cdecl FakeDelete(void* p){
    if(Ours(p)){ Push(7,U(p)); return; }
    delPatch->Enable(false); reinterpret_cast<void(__cdecl*)(void*)>(0x8214BD)(p); delPatch->Enable(true); }
inline void __fastcall FakeFxsExit(void* s,void*){ Push(8,U(s)); }
inline bool __fastcall FakeFxsInit(void* s,void*,FxSystemBP_c* bp,const RwMatrix& local,RwMatrix* parent){
    Push(9,U(s),U(bp),U(parent)); Push(90,U(&local),uint32(Fnv(&local,0x40))); return true; }
inline void __fastcall FakeRate(void* s,void*,float m){ Push(10,U(s),std::bit_cast<uint32>(m)); }
inline uint32 freeMask{}, freeCalls{};
inline bool __fastcall FakeFreePrt(void* bp,void*,FxSystem_c* s){ const bool r=freeCalls>=16||((freeMask>>(freeCalls%32))&1); ++freeCalls; Push(11,U(bp),U(s),r); return r; }
inline int32 txdSlot{};
inline int32 __cdecl FakeAddTxdSlot(const char* n){ Push(13,Hs(n),uint32(txdSlot)); return txdSlot; }
inline bool __cdecl FakeLoadTxd(int32 i,const char* f){ Push(14,uint32(i),Hs(f)); return true; }
inline void __cdecl FakeAddRef(int32 i){ Push(15,uint32(i)); }
inline void __cdecl FakePushTxd(){ Push(16); }
inline void __cdecl FakeSetTxd(int32 i){ Push(17,uint32(i)); }
inline void __cdecl FakePopTxd(){ Push(18); }
inline void __cdecl FakeRemoveTxd(int32 i){ Push(19,uint32(i)); }
inline FILESTREAM openRet{};
inline FILESTREAM __cdecl FakeOpen(const char* p,const char* m){ Push(20,Hs(p),Hs(m),U(openRet)); return openRet; }
inline std::array<std::array<char,64>,48> script{}; inline uint32 nScript{}, lineIdx{}, eofMask{};
inline bool __cdecl FakeReadLine(FILESTREAM f,char* s,int32 n){ const uint32 k=lineIdx++; const char* src=k<nScript ? script[k].data() : "END";
    size_t len=std::strlen(src); if(n<=0)return false; if(len>size_t(n-1))len=size_t(n-1); std::memcpy(s,src,len); s[len]=0;
    Push(21,U(f),uint32(n),k); return !((eofMask>>(k%32))&1); }
inline int32 __cdecl FakeClose(FILESTREAM f){ Push(22,U(f)); return 0; }
inline bool __fastcall FakeBpLoad(void* bp,void*,const char* fn,FILESTREAM f,int32 v){ Push(23,U(bp),Hs(fn),U(f)); Push(91,uint32(v)); return false; }
inline void __fastcall FakeBpRender(void* bp,void*,RwCamera* c,float b,bool h){ Push(24,U(bp),U(c),std::bit_cast<uint32>(b)); Push(92,h); }
inline RwBool __cdecl FakeRS(RwRenderState s,void* v){ Push(25,uint32(s),U(v)); return TRUE; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,23> fns{{
    {"FxManager_c","CalcFrustumInfo",0x4A9140,0,-1,1024},{"FxManager_c","Constructor",0x4A9470,0,-1,256},
    {"FxManager_c","CreateFxSystem-1",0x4A95C0,0,-1,1024},{"FxManager_c","CreateFxSystem-2",0x4A9BB0,0,-1,1024},
    {"FxManager_c","CreateFxSystem-3",0x4A96B0,0,-1,1024},{"FxManager_c","CreateFxSystem-4",0x4A9BE0,0,-1,1024},
    {"FxManager_c","DestroyFxSystem",0x4A9810,0,-1,1024},{"FxManager_c","Destructor",0x4A90A0,0,-1,128},
    {"FxManager_c","Exit",0x4A9A10,0,-1,512},{"FxManager_c","FindFxSystemBP",0x4A9360,0,-1,1024},
    {"FxManager_c","FreeUpParticle",0x4A9400,0,-1,1024},{"FxManager_c","FxRwMatrixCreate",0x4A9440,0,-1,256},
    {"FxManager_c","FxRwMatrixDestroy",0x4A9460,0,-1,256},{"FxManager_c","GetFrustumInfo",0x4A9130,0,-1,128},
    {"FxManager_c","GetParticle",0x4A93C0,0,-1,512},{"FxManager_c","Init",0x4A98E0,0,-1,256},
    {"FxManager_c","LoadFxProject",0x5C2420,0,-1,512},{"FxManager_c","LoadFxSystemBP",0x5C1F50,0,-1,512},
    {"FxManager_c","Render",0x4A92A0,0,-1,512},{"FxManager_c","ReturnParticle",0x4A93B0,0,-1,512},
    {"FxManager_c","SetWindData",0x4A93E0,0,-1,256},{"FxManager_c","ShouldCreate",0x4A9500,0,-1,1024},
    {"FxManager_c","UnloadFxProject",0x4A9AE0,0,-1,512},
}};

inline const char* const names[]{"fire","FIRE","explosion_large","smoke_flare","prt_blood","water_splash","","nonexistent_fx"};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fx25-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fx25-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fx25");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"fx25-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,23> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,23> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    using std::bit_cast;

    // estado global guardado: g_fxMan, g_fx (calidad), balance dia/noche y el puntero de estado de render
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{{FXMAN,MGR_N},{0xA9AE00,0x80},{0x8D12C0,4}})
        saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)});
    auto& fpState=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedFp=fpState;
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); fpState=savedFp; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch m1(bit_cast<uintptr_t>(&FxMemoryPool_c::Init),FakePoolInit), m1e(0x4A9C30,FakePoolInit);
        EntryPatch m2(bit_cast<uintptr_t>(&FxMemoryPool_c::Exit),FakePoolExit), m2e(0x4A9C80,FakePoolExit);
        EntryPatch m3(bit_cast<uintptr_t>(&FxMemoryPool_c::Optimise),FakePoolOptimise), m3e(0x4A9CD0,FakePoolOptimise);
        EntryPatch w1(bit_cast<uintptr_t>(&RwMatrixCreate),FakeMatCreate), w1e(0x7F2A50,FakeMatCreate);
        EntryPatch w2(bit_cast<uintptr_t>(&RwMatrixDestroy),FakeMatDestroy), w2e(0x7F2A20,FakeMatDestroy);
        EntryPatch s1(bit_cast<uintptr_t>(&FxSystem_c::Exit),FakeFxsExit), s1e(0x4AA840,FakeFxsExit);
        EntryPatch s2(bit_cast<uintptr_t>(&FxSystem_c::Init),FakeFxsInit), s2e(0x4AA750,FakeFxsInit);
        EntryPatch s3(bit_cast<uintptr_t>(&FxSystem_c::SetRateMult),FakeRate), s3e(0x4AA6F0,FakeRate);
        EntryPatch b1(bit_cast<uintptr_t>(&FxSystemBP_c::FreePrtFromSystem),FakeFreePrt), b1e(0x4AA1B0,FakeFreePrt);
        EntryPatch b2(bit_cast<uintptr_t>(&FxSystemBP_c::Render),FakeBpRender), b2e(0x4AA160,FakeBpRender);
        EntryPatch b3(bit_cast<uintptr_t>(&FxSystemBP_c::Load),FakeBpLoad), b3e(0x5C05F0,FakeBpLoad);
        EntryPatch t1(bit_cast<uintptr_t>(&CTxdStore::AddTxdSlot),FakeAddTxdSlot), t1e(0x731C80,FakeAddTxdSlot);
        EntryPatch t2(bit_cast<uintptr_t>(static_cast<bool(*)(int32,const char*)>(&CTxdStore::LoadTxd)),FakeLoadTxd), t2e(0x7320B0,FakeLoadTxd);
        EntryPatch t3(bit_cast<uintptr_t>(&CTxdStore::AddRef),FakeAddRef), t3e(0x731A00,FakeAddRef);
        EntryPatch t4(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), t4e(0x7316A0,FakePushTxd);
        EntryPatch t5(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), t5e(0x7319C0,FakeSetTxd);
        EntryPatch t6(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), t6e(0x7316B0,FakePopTxd);
        EntryPatch t7(bit_cast<uintptr_t>(&CTxdStore::RemoveTxdSlot),FakeRemoveTxd), t7e(0x731CD0,FakeRemoveTxd);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpen), f1e(0x538900,FakeOpen);
        EntryPatch f2(bit_cast<uintptr_t>(&CFileMgr::ReadLine),FakeReadLine), f2e(0x5389B0,FakeReadLine);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeClose), f3e(0x5389D0,FakeClose);
        auto& quality=G<int32>(0xA9AE54); auto& dnBalance=G<float>(0x8D12C0);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            const bool poolFn=nm=="Init"||nm=="UnloadFxProject"||nm=="Exit"||nm=="LoadFxProject"||nm=="LoadFxSystemBP";
            const bool newFn=nm.rfind("CreateFxSystem",0)==0||nm=="DestroyFxSystem"||nm=="Exit"||nm=="UnloadFxProject";
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto randVec=[&](float lo,float hi){ return CVector{uni(lo,hi),uni(lo,hi),uni(lo,hi)}; };
                auto unitVec=[&]{ CVector v=randVec(-1.f,1.f); const float m=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z)+1e-3f; return CVector{v.x/m,v.y/m,v.z/m}; };
                // listas crudas: cabeza 0, cola 4, contador 8; enlaces del elemento: anterior 0, siguiente 4 (AddItem del EXE)
                auto lpush=[&](uint8* L,uint8* it){ const uint32 head=At<uint32>(L,0); At<uint32>(it,0)=0; At<uint32>(it,4)=head;
                    if(head)At<uint32>(reinterpret_cast<void*>(uintptr_t(head)),0)=U(it); else At<uint32>(L,4)=U(it); At<uint32>(L,0)=U(it); ++At<uint32>(L,8); };
                auto randMatrix=[&](uint8* m,const CVector& pos){ fill(m,0x40); At<CVector>(m,0)=unitVec(); At<CVector>(m,0x10)=unitVec(); At<CVector>(m,0x20)=unitVec(); At<CVector>(m,0x30)=pos;
                    static const uint32 fl[]{0u,3u,0x20003u,0x20000u}; At<uint32>(m,0xC)=next()%5 ? fl[next()%4] : next(); };
                auto word=[&](const char* s){ char w[64]{}; if(std::sscanf(s,"%63s",w)!=1)w[0]=0; return std::string(w); };
                auto addLine=[&](const char* s){ if(nScript<script.size())std::snprintf(script[nScript++].data(),64,"%s",s); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2525u; if(!stt)stt=1;
                    nrec=0; recOverflow=false; rngState=next(); rngCalls=0; freeMask=next(); freeCalls=0; matCreates=0; lineIdx=0; nScript=0; eofMask=next()%4 ? 0u : next();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    auto* gm=reinterpret_cast<uint8*>(FXMAN); auto* fm=fakeMgr.data();
                    const bool fakeSelf=nm=="Constructor"||nm=="Destructor"||next()%3==0;
                    uint8* self=fakeSelf ? fm : gm;

                    // --- pools (solo las funciones que reservan o liberan memoria FX) ---
                    if(poolFn){ for(auto* p:{poolA.data(),poolB.data()}){ std::memset(p,uint8(next()),POOL_N); fill(p,0x100); } }
                    for(auto& m:mats)fill(m.data(),0x40);
                    std::memset(arena.data(),0xCD,arena.size());
                    // --- gestores: g_fxMan y el falso, con el mismo formato ---
                    const CVector base=randVec(-1500.f,1500.f);
                    auto mkMgr=[&](uint8* m,bool global){ fill(m,global ? MGR_N : fakeMgr.size());
                        std::memset(m,0,0x18); std::memset(m+0x1C,0,0xC);
                        At<uint32>(m,0x18)=next()%2 ? U(prts[60].data()) : next();
                        At<uint32>(m,0x88)=next()%5;
                        for(int k=0;k<8;++k)At<uint32>(m,0x8C+4*k)=U(mats[(global?0:8)+k].data());
                        At<uint32>(m,0xAC)=U(global ? poolA.data() : poolB.data()); At<uint32>(m,0xB0)=0x10000; At<uint32>(m,0xB4)=next()%64;
                        At<CVector>(m,0x34)=base+randVec(-20.f,20.f); At<float>(m,0x40)=uni(10.f,150.f);
                        for(int k=0;k<4;++k){ const CVector nrm=unitVec(); At<CVector>(m,0x48+0x10*k)=nrm;
                            At<float>(m,0x54+0x10*k)=nrm.x*base.x+nrm.y*base.y+nrm.z*base.z+uni(-60.f,60.f); } };
                    mkMgr(fm,false); mkMgr(gm,true);
                    auto* bpsL=self; auto* sysL=self+0xC; auto* freeL=self+0x1C; auto* gFreeL=gm+0x1C;
                    // --- blueprints y primitivas ---
                    const int nb=1+int(next()%6);
                    for(auto& p:prims){ fill(p.data(),0x40); std::memset(p.data()+0x20,0,0xC); }
                    for(auto& s:spheres){ fill(s.data(),0x20); At<CVector>(s.data(),0)=randVec(-5.f,5.f); At<float>(s.data(),0xC)=uni(0.1f,30.f); }
                    for(int b=0;b<8;++b){ uint8* bp=bps[b].data(); fill(bp,0x40);
                        At<uint32>(bp,8)=next()%4 ? CKeyGen::GetUppercaseKey(names[next()%6]) : next();
                        static const int8 np[]{0,1,2,3,3,2,1,-1,-128};
                        At<int8>(bp,0x1B)=np[next()%std::size(np)];
                        for(int j=0;j<4;++j)primArr[b][j]=prims[(b*2+j)%16].data();
                        At<uint32>(bp,0x1C)=U(primArr[b].data());
                        At<uint32>(bp,0x20)=next()%4 ? U(spheres[next()%3].data()) : 0u; }
                    for(int b=0;b<nb;++b)lpush(bpsL,bps[b].data());
                    if(fakeSelf&&next()%2)lpush(gm,bps[7].data());
                    // --- sistemas ---
                    const bool needSys=nm=="DestroyFxSystem"||nm=="FreeUpParticle";
                    const int ns=int(next()%9)+(needSys ? 1 : 0);
                    for(int s=0;s<12;++s){ uint8* sy=systems[s].data(); fill(sy,0x110);
                        At<uint32>(sy,8)=U(bps[s<ns ? next()%nb : next()%8].data()); At<uint32>(sy,0x74)=0;
                        At<uint8>(sy,0x62)=uint8(next()%4 ? next()&0xFE : next());
                        std::memset(sy+0x7C,0,0x88); At<uint32>(sy,0x7C)=0x85AA94; At<uint32>(sy,0xC)=U(mats[16+s%8].data()); }
                    for(int s=0;s<ns;++s)lpush(sysL,systems[s].data());
                    if(ns>0&&nm=="FreeUpParticle")At<uint8>(reinterpret_cast<void*>(uintptr_t(At<uint32>(sysL,0))),0x62)&=0xDF;   // cabeza seleccionable
                    // --- particulas ---
                    const int np=int(next()%48);
                    for(auto& p:prts){ fill(p.data(),0x40); std::memset(p.data(),0,8); }
                    for(int k=0;k<np;++k){ uint8* p=prts[k].data();
                        const uint32 o=next()%8; At<uint32>(p,0x28)=o<6&&ns>0 ? U(systems[next()%ns].data()) : o==6 ? U(systems[11].data()) : 0u;
                        const uint32 where=next()%10;
                        if(where<7)lpush(prims[next()%16].data()+0x20,p); else if(where<9)lpush(freeL,p); else lpush(gFreeL,p); }

                    json out={{"switches",switches}}; json ret=nullptr;
                    auto thisCall0=[&](auto r){ return reinterpret_cast<decltype(r)(__thiscall*)(void*)>(a)(self); };
                    if(nm=="CalcFrustumInfo"){
                        fill(camBuf.data(),camBuf.size()); fill(frameBuf.data(),frameBuf.size());
                        At<uint32>(camBuf.data(),4)=U(frameBuf.data());
                        At<float>(camBuf.data(),0x68)=next()%8 ? uni(0.05f,2.5f) : 0.f; At<float>(camBuf.data(),0x6C)=next()%8 ? uni(0.05f,2.5f) : 1.f;
                        static const float farPl[]{0.f,1.f,150.f,800.f,3000.f,0.5f};
                        At<float>(camBuf.data(),0x84)=next()%3 ? uni(0.5f,5000.f) : farPl[next()%std::size(farPl)];
                        for(int k=0;k<6;++k){ At<CVector>(camBuf.data(),0x94+0x14*k)=unitVec(); At<float>(camBuf.data(),0xA0+0x14*k)=uni(-3000.f,3000.f); }
                        randMatrix(frameBuf.data()+0x10,randVec(-3000.f,3000.f));
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,camBuf.data()); }
                    else if(nm=="Constructor"){ if(next()%2)std::memset(fm,0xCD,MGR_N); ret=U(thisCall0((void*)nullptr)); }
                    else if(nm=="Destructor"){ if(next()%2)std::memset(fm,0xCD,MGR_N); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                    else if(nm=="Init"){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(self); }
                    else if(nm=="Exit"||nm=="UnloadFxProject"||nm=="FreeUpParticle"){
                        At<int32>(self,0x28)=next()%4 ? int32(next()%5000) : -1;
                        if(newFn){ EntryPatch n1(0x82119A,FakeNew), d1(0x8214BD,FakeDelete); newPatch=&n1; delPatch=&d1;
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(self); newPatch=delPatch=nullptr; }
                        else reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                    else if(nm=="DestroyFxSystem"){ void* target=systems[next()%ns].data();
                        EntryPatch n1(0x82119A,FakeNew), d1(0x8214BD,FakeDelete); newPatch=&n1; delPatch=&d1;
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,target); newPatch=delPatch=nullptr; }
                    else if(nm=="FindFxSystemBP"){ const char* n=names[next()%std::size(names)];
                        ret=U(reinterpret_cast<void*(__thiscall*)(void*,const char*)>(a)(self,n)); }
                    else if(nm=="FxRwMatrixCreate"){ At<uint32>(self,0x88)=next()%12; ret=U(thisCall0((void*)nullptr)); }
                    else if(nm=="FxRwMatrixDestroy"){ static const uint32 cur[]{0u,1u,7u,8u,0x80000000u,0xFFFFFFFFu};
                        At<uint32>(self,0x88)=next()%2 ? next()%9 : cur[next()%std::size(cur)];
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,next()%3 ? mats[next()%24].data() : nullptr); }
                    else if(nm=="GetFrustumInfo"){ ret=U(thisCall0((void*)nullptr)); }
                    else if(nm=="GetParticle"){ static const int8 pt[]{0,0,0,1,-1,127,-128,2};
                        ret=U(reinterpret_cast<void*(__thiscall*)(void*,int8)>(a)(self,pt[next()%std::size(pt)])); }
                    else if(nm=="ReturnParticle"){ reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,prts[48+next()%16].data()); }
                    else if(nm=="SetWindData"){ void* d=next()%4 ? static_cast<void*>(camBuf.data()+next()%64) : nullptr; void* s=next()%4 ? static_cast<void*>(windSpeed.data()) : reinterpret_cast<void*>(uintptr_t(next()));
                        reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(a)(self,d,s); }
                    else if(nm=="Render"){ static const float dn[]{0.f,1.f,0.5f};
                        dnBalance=next()%3 ? uni(0.f,1.f) : dn[next()%3]; fpState=&FakeRS;
                        reinterpret_cast<void(__thiscall*)(void*,void*,bool)>(a)(self,camBuf.data(),next()%2!=0); fpState=savedFp; }
                    else if(nm=="LoadFxProject"||nm=="LoadFxSystemBP"){
                        static const char* paths[]{"models\\effects.fxp","MODELS\\EFFECTS.FXP","data\\fx\\a.fxp","x.fxp","abcd"};
                        static const FILESTREAM files[]{reinterpret_cast<FILESTREAM>(0x1000),reinterpret_cast<FILESTREAM>(0x7FFFFFF0),nullptr,
                            reinterpret_cast<FILESTREAM>(0x80000010),reinterpret_cast<FILESTREAM>(0xFFFFFFFF)};
                        static const char* vers[]{"101","108","109 x","  -3","0","2147483647"};
                        const char* path=paths[next()%std::size(paths)];
                        openRet=next()%3 ? files[0] : files[next()%std::size(files)];
                        txdSlot=next()%4 ? int32(next()%5000) : -1;
                        if(nm=="LoadFxSystemBP"){ addLine(vers[next()%std::size(vers)]); addLine("FX_SYSTEM_DATA:");
                            ret=U(reinterpret_cast<void*(__thiscall*)(void*,const char*,FILESTREAM)>(a)(self,path,openRet)); }
                        else {
                            // guion simulando el bucle del EXE: cabecera, (linea intermedia, campo, version)* y fin
                            static const char* heads[]{"FX_PROJECT_DATA:","FX_PROJECT_DATA: 1","  FX_PROJECT_DATA:","FX_SYSTEM_DATA:","x"};
                            static const char* mids[]{"","  ","junk line","FX_SYSTEM_DATA:","1 2 3"};
                            static const char* conts[]{"FX_SYSTEM_DATA:","  FX_SYSTEM_DATA:\t","FX_SYSTEM_DATA: extra",""};
                            static const char* ends[]{"FX_PROJECT_END:","FX_SYSTEM_DATA","FX_SYSTEM_DATA:X","fx_system_data:","FX_SYSTEM_DATA;","END"};
                            const char* h=heads[next()%std::size(heads)]; addLine(h); std::string B=word(h);
                            const uint32 target=next()%5; uint32 count=0;
                            for(;;){ addLine(mids[next()%std::size(mids)]);
                                const char* c; if(count<target){ c=conts[next()%std::size(conts)]; if(!*c&&B!="FX_SYSTEM_DATA:")c=conts[0]; } else c=ends[next()%std::size(ends)];
                                addLine(c); if(!word(c).empty())B=word(c);
                                if(B=="FX_SYSTEM_DATA:"){ addLine(vers[next()%std::size(vers)]); ++count; } else break; }
                            ret=reinterpret_cast<bool(__thiscall*)(void*,const char*)>(a)(self,path); }
                        out["lines"]=lineIdx; }
                    else { // ShouldCreate y CreateFxSystem-1..4
                        const CVector pos=base+randVec(-200.f,200.f);
                        randMatrix(xform.data(),pos); randMatrix(objMat.data(),randVec(-3.f,3.f));
                        RwMatrix* om=next()%5<2 ? nullptr : reinterpret_cast<RwMatrix*>(objMat.data());
                        const bool ignore=next()%4==0;
                        static const int32 q[]{0,1,2,3,4,-1,0x7FFFFFFF,int32(0x80000000)};
                        quality=next()%3 ? int32(next()%4) : q[next()%std::size(q)];
                        void* bp=next()%8 ? static_cast<void*>(bps[next()%8].data()) : nullptr;
                        const char* n=names[next()%std::size(names)];
                        if(nm=="ShouldCreate"){ if(!bp)bp=bps[0].data();
                            ret=reinterpret_cast<bool(__thiscall*)(void*,void*,const void*,RwMatrix*,bool)>(a)(self,bp,xform.data(),om,ignore); }
                        else { EntryPatch n1(0x82119A,FakeNew), d1(0x8214BD,FakeDelete); newPatch=&n1; delPatch=&d1;
                            if(nm=="CreateFxSystem-1") ret=U(reinterpret_cast<void*(__thiscall*)(void*,void*,const void*,RwMatrix*,bool)>(a)(self,bp,xform.data(),om,ignore));
                            else if(nm=="CreateFxSystem-2") ret=U(reinterpret_cast<void*(__thiscall*)(void*,const char*,const void*,RwMatrix*,bool)>(a)(self,n,xform.data(),om,ignore));
                            else if(nm=="CreateFxSystem-3") ret=U(reinterpret_cast<void*(__thiscall*)(void*,void*,const CVector*,RwMatrix*,bool)>(a)(self,bp,&pos,om,ignore));
                            else ret=U(reinterpret_cast<void*(__thiscall*)(void*,const char*,const CVector*,RwMatrix*,bool)>(a)(self,n,&pos,om,ignore));
                            newPatch=delPatch=nullptr; }
                        out["arena"]=ReversingProbe::Bytes(arena.data(),0x7C); }

                    out["gfx"]=ReversingProbe::Bytes(gm,MGR_N); out["fake"]=ReversingProbe::Bytes(fm,MGR_N);
                    json h=json::array();
                    for(auto& b:bps)h.push_back(Fnv(b.data(),0x24));
                    for(auto& p:prims)h.push_back(Fnv(p.data(),0x40));
                    for(auto& s:systems)h.push_back(Fnv(s.data(),0x7C));   // la zona de CAEFireAudioEntity (0x7C..) no se compara
                    for(auto& p:prts)h.push_back(Fnv(p.data(),0x3C));
                    for(auto& m:mats)h.push_back(Fnv(m.data(),0x40));
                    out["hashes"]=h;
                    if(poolFn)out["pools"]={Fnv(poolA.data(),POOL_N),Fnv(poolB.data(),POOL_N)};
                    out["ret"]=ret; out["recs"]=Recs(); out["rec_overflow"]=recOverflow;
                    RestoreAll();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["recs"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=fpState==savedFp; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
