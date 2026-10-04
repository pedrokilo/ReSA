#pragma once
// Banco dentro del motor: scripts en streaming, Game y árbol cuaternario (lista de 12). 34 funciones de
// CStreamedScripts, Game y CQuadTreeNode<T>. Original (hooks del lote desactivados) frente a candidato con
// fixtures deterministas:
// - CStreamedScripts trabaja sobre un objeto falso (con 0x20 bytes delante para ver las escrituras en la entrada
//   -1). ReadStreamedScriptData lee la cabecera real del SCM cargado (solo lectura). Se simulan
//   CheckStreamedScriptVersion, RwStreamRead, CTheScripts::StartNewScript, SetMissionDoesntRequireModel y
//   operator new[]/delete[] (solo durante la llamada, registro sin reservas).
// - CQuadTreeNode trabaja con pools falsos de nodos del árbol (256) y de nodos de lista (1024): los punteros
//   globales 0xB745BC y 0xB74484 se cambian solo durante la llamada. InitPool usa un operator new falso.
// - Game: funciones de orquestación. Se interceptan todas sus dependencias (render, cámara RW, sombras, HUD,
//   menú, audio, inicialización y RenderWare) y se compara la secuencia exacta de llamadas, argumentos y estados
//   de render. Escena, cámara, espejos, agua, pads, réplica, menú y resolución se fijan y se restauran.
//   Idle y FrontendIdle quedan fuera (alojan el propio banco y llevan añadidos NOTSA de ImGui/carga rápida).
//   Silenciosos (sin traza, NOTSA o sin efecto observable): printf del EXE en TheGame y DebugModules::Render3D (lo único que hacía UIRenderer::Render3D; desde el 04/10/2026 RenderEffects ya no la llama).
#include <fstream>
#include <set>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "StreamedScripts.h"
#include "TheScripts.h"
#include "RunningScript.h"
#include "QuadTreeNode.h"
#include "app.h"
#include "app_game.h"
#include "platform.h"
#include "Gamma.h"
#include "LoadingScreen.h"
#include "PlantMgr.h"
#include "Shadows.h"
#include "Clouds.h"
#include "MovingThings.h"
#include "BreakManager_c.h"
#include "Credits.h"
#include "UserDisplay.h"
#include "Garages.h"
#include "UIRenderer.h"
#include "Birds.h"
#include "Skidmarks.h"
#include "Ropes.h"
#include "Glass.h"
#include "WaterCannons.h"
#include "VehicleRecording.h"
#include "PostEffects.h"
#include "CarFXRenderer.h"
#include "Mirrors.h"
#include "StencilShadows.h"
#include "Replay.h"
#include "Pickups.h"
#include "Hud.h"
#include "Darkel.h"
#include "Messages.h"
#include "OnscreenTimer.h"
#include "Sprite2d.h"
#include "Font.h"
#include "../ProbeRuntime.hpp"

namespace Sys12Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; } float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; } };
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
inline const float QNAN=std::numeric_limits<float>::quiet_NaN();

// --- registro sin reservas (stubs activos mientras operator new/delete están sustituidos) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// ======================= CStreamedScripts =======================
constexpr size_t SS_PRE=0x20, SS_SIZE=sizeof(CStreamedScripts);
alignas(16) inline std::array<uint8,SS_PRE+SS_SIZE> ssBuf{};
alignas(16) inline std::array<uint8,0x1100> arena{};
alignas(16) inline std::array<uint8,0x400> ipBuf{};
alignas(16) inline std::array<uint8,sizeof(CRunningScript)> fakeScript{};
inline uint32 Off(const void* p){
    const auto u=uintptr_t(p);
    if(!u)return 0xFFFFFFFFu;
    auto in=[&](const auto& a){ return u>=uintptr_t(a.data())&&u<uintptr_t(a.data())+a.size(); };
    if(in(arena))return 0x10000u+uint32(u-uintptr_t(arena.data()));
    if(in(ipBuf))return 0x20000u+uint32(u-uintptr_t(ipBuf.data()));
    if(in(fakeScript))return 0x30000u+uint32(u-uintptr_t(fakeScript.data()));
    if(in(ssBuf))return 0x40000u+uint32(u-uintptr_t(ssBuf.data()));
    return 0xEEEEEEEEu; }
inline void* __cdecl FakeNew(size_t n){ Push(1,uint32(n)); return n<=arena.size() ? arena.data() : nullptr; }
inline void __cdecl FakeDelete(void* p){ Push(2,Off(p)); }
inline bool checkResult{};
inline RwStream* const FAKE_STREAM=reinterpret_cast<RwStream*>(0x7E200000);
inline bool __cdecl FakeCheckVersion(RwStream* s,const char* name){ Push(3,s==FAKE_STREAM,uint32(Fnv(name,name ? strlen(name):0))); return checkResult; }
inline RwUInt32 __cdecl FakeStreamRead(RwStream* s,void* buf,RwUInt32 n){ Push(4,Off(buf),n); Push(5,s==FAKE_STREAM); return n; }
inline CRunningScript* __cdecl FakeStartNewScript(uint8* ip){ trace.push_back({"StartNewScript",Off(ip)}); return reinterpret_cast<CRunningScript*>(fakeScript.data()); }
inline void __cdecl FakeDoesntRequire(int32 id){ trace.push_back({"SetMissionDoesntRequireModel",id}); }
inline const uint8* realEntries{}; inline int realCount{};

inline void FillName(Rng& r,char* dst){
    static const char cs[]="abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_";
    const int len=1+int(r.next()%18); for(int k=0;k<len;++k)dst[k]=cs[r.next()%63]; dst[len]=0; for(int k=len+1;k<20;++k)dst[k]=char(1+r.next()%255); }

inline json RunScripts(const std::string& nm,uintptr_t a,Rng& r,json& out) {
    for(auto& b:ssBuf)b=uint8(r.next());
    uint8* const o=ssBuf.data()+SS_PRE; auto E=[&](int k){ return o+k*0x20; };
    const int cnt=r.next()%8==0 ? 0 : int(r.next()%83);
    At<int16>(o,0xA44)=int16(cnt);
    for(int k=0;k<82;++k)if(k<cnt||r.next()%2)FillName(r,reinterpret_cast<char*>(E(k)+8));
    json ret=nullptr;
    if(nm=="Initialise"||nm=="ReInitialise") reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
    else if(nm=="FindStreamedScript"||nm=="FindStreamedScriptQuiet"){
        char q[24]{}; const int mode=int(r.next()%8);
        if(cnt>0&&mode<5){ std::memcpy(q,E(int(r.next()%cnt))+8,20); for(int k=0;q[k];++k)if(isalpha(uint8(q[k]))&&r.next()%2)q[k]=char(q[k]^0x20); }
        else if(cnt>0&&mode==5){ E(int(r.next()%cnt))[8]=0; }
        else FillName(r,q);
        ret=reinterpret_cast<int32(__thiscall*)(void*,const char*)>(a)(o,q); out["query"]=std::string(q); }
    else if(nm=="GetProperIndexFromIndexUsedByScript"){
        for(int k=0;k<82;++k)At<int16>(E(k),6)=int16(int(r.next()%45)-3);
        const int16 q=int16(int(r.next()%50)-3); ret=int(reinterpret_cast<int16(__thiscall*)(void*,int16)>(a)(o,q)); }
    else if(nm=="GetStreamedScriptFilename"){ const uint16 idx=uint16(r.next()%82); ret=Off(reinterpret_cast<const char*(__thiscall*)(void*,uint16)>(a)(o,idx)); }
    else if(nm=="GetStreamedScriptWithThisStartAddress"){
        for(int k=0;k<82;++k)At<uint8*>(E(k),0)=r.next()%4==0 ? nullptr : ipBuf.data()+(r.next()%48)*0x10;
        uint8* q=r.next()%4 ? At<uint8*>(E(int(r.next()%82)),0) : (r.next()%4==0 ? nullptr : ipBuf.data()+(r.next()%64)*0x10);
        ret=reinterpret_cast<uint32(__thiscall*)(void*,uint8*)>(a)(o,q); }
    else if(nm=="RegisterScript"){ At<int16>(o,0xA44)=int16(r.next()%82); char q[24]{}; FillName(r,q);
        ret=reinterpret_cast<int32(__thiscall*)(void*,const char*)>(a)(o,q); }
    else if(nm=="RemoveStreamedScriptFromMemory"){ const int32 idx=int32(r.next()%82); At<uint8*>(E(idx),0)=r.next()%4==0 ? nullptr : ipBuf.data()+(r.next()%64)*0x10;
        nrec=0;
        { ReversingProbe::EntryPatch d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(o,idx); }
        out["recs"]=Recs(); }
    else if(nm=="LoadStreamedScript"){ const int32 idx=int32(r.next()%82); At<int32>(E(idx),0x1C)=r.next()%6==0 ? int32(0x2000+r.next()%0x1000) : int32(r.next()%0x1000);
        checkResult=r.next()%4!=0; nrec=0; bool b;
        { ReversingProbe::EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete);
          b=reinterpret_cast<bool(__thiscall*)(void*,RwStream*,int32)>(a)(o,FAKE_STREAM,idx); }
        ret=b; out["recs"]=Recs(); }
    else if(nm=="StartNewStreamedScript"){ const int32 idx=int32(r.next()%82); At<uint8*>(E(idx),0)=r.next()%4==0 ? nullptr : ipBuf.data()+(r.next()%64)*0x10;
        for(auto& b:fakeScript)b=uint8(r.next());
        ret=Off(reinterpret_cast<CRunningScript*(__thiscall*)(void*,int32)>(a)(o,idx)); out["script"]=ReversingProbe::Bytes(fakeScript.data(),fakeScript.size()); }
    else if(nm=="ReadStreamedScriptData"){ const int c=int(r.next()%(uint32(realCount)+1)); At<int16>(o,0xA44)=int16(c);
        for(int k=0;k<c;++k){ char* dst=reinterpret_cast<char*>(E(k)+8); if(realCount&&r.next()%5){ std::memcpy(dst,realEntries+(r.next()%realCount)*0x1C,20); dst[19]=0; } else FillName(r,dst); }
        reinterpret_cast<void(__thiscall*)(void*)>(a)(o); }
    out["obj"]=Fnv(ssBuf.data(),ssBuf.size()); out["tail"]=ReversingProbe::Bytes(o+0xA40,8); out["pre"]=ReversingProbe::Bytes(ssBuf.data(),SS_PRE);
    return ret;
}

// ======================= CQuadTreeNode =======================
constexpr int NQ=256, NP=1024;
alignas(16) inline std::array<uint8,NQ*0x28> qStore{}; inline std::array<uint8,NQ> qFlags{};
alignas(16) inline std::array<uint8,NP*8> pStore{}; inline std::array<uint8,NP> pFlags{};
alignas(16) inline std::array<uint8,0x14> qHdr{}, pHdr{};
alignas(16) inline std::array<uint8,0x28> soloNode{};
alignas(16) inline std::array<uint8,0x4400> ipArena{};
inline size_t ipUsed{}; inline int ipCalls{}; inline bool ipFail{};
inline void* __cdecl FakeSeqNew(size_t n){ Push(1,uint32(n)); if(ipFail&&ipCalls==0){ ++ipCalls; return nullptr; } ++ipCalls;
    if(ipUsed+n>ipArena.size())return nullptr; void* p=ipArena.data()+ipUsed; ipUsed=(ipUsed+n+15)&~size_t(15); return p; }
struct QRect { float l,t,r,b; }; // orden en memoria de CRect: left, top, right, bottom (top = Y máxima)
inline void ResetPool(Rng& r,uint8* hdr,uint8* store,size_t storeSize,uint8* flags,int n){
    for(size_t k=0;k<storeSize;++k)store[k]=uint8(r.next());
    for(int k=0;k<n;++k){ const uint8 id=uint8(r.next()&0x7F); flags[k]=r.next()%4==0 ? id : uint8(0x80|id); }
    At<uint8*>(hdr,0)=store; At<uint8*>(hdr,4)=flags; At<int32>(hdr,8)=n; At<int32>(hdr,0xC)=int32(r.next()%uint32(n))-1; At<uint32>(hdr,0x10)=0; }
inline uint8* Alloc(Rng& r,uint8* store,uint8* flags,int n,size_t stride){
    int k=int(r.next()%uint32(n)); for(int t=0;t<n;++t,k=(k+1)%n)if(flags[k]&0x80){ flags[k]=uint8((flags[k]+1)&0x7F); return store+k*stride; }
    return nullptr; }
inline uintptr_t Item(int k){ return 0x10000u+uintptr_t(k)*0x10; }
inline QRect Sector(const QRect& p,int s){ const float cx=(p.r+p.l)*0.5f, cy=(p.b+p.t)*0.5f; QRect c=p;
    switch(s){ case 0:c.r=cx;c.b=cy;break; case 1:c.l=cx;c.b=cy;break; case 2:c.r=cx;c.t=cy;break; default:c.l=cx;c.t=cy; } return c; }
inline void SetNode(uint8* n,const QRect& rc,uint32 level){ std::memcpy(n,&rc,16); At<void*>(n,0x10)=nullptr; for(int s=0;s<4;++s)At<void*>(n,0x14+4*s)=nullptr; At<uint32>(n,0x24)=level; }
inline void AddItems(Rng& r,void*& head,int n){ for(int k=0;k<n;++k){ uint8* p=Alloc(r,pStore.data(),pFlags.data(),NP,8); At<uintptr_t>(p,0)=Item(int(r.next()%8)); At<void*>(p,4)=head; head=p; } }
inline void Build(Rng& r,uint8* node,const QRect& rc,uint32 level,uint32 childPct){
    SetNode(node,rc,level); AddItems(r,At<void*>(node,0x10),int(r.next()%4));
    if(level)for(int s=0;s<4;++s)if(r.next()%100<childPct){ uint8* c=Alloc(r,qStore.data(),qFlags.data(),NQ,0x28); At<uint8*>(node,0x14+4*s)=c; Build(r,c,Sector(rc,s),level-1,childPct); } }
inline QRect RootRect(Rng& r){ if(r.next()%3==0)return {-3000.f,3000.f,3000.f,-3000.f};
    const float l=r.uni(-3000.f,0.f), b=r.uni(-3000.f,0.f); return {l,b+r.uni(50.f,6000.f),l+r.uni(50.f,6000.f),b}; }
inline QRect QueryRect(Rng& r,const QRect& root){
    auto pick=[&](float lo,float hi){ const float m=(hi-lo)*0.1f; return r.uni(lo-m,hi+m); };
    float x0=pick(root.l,root.r), x1=pick(root.l,root.r), y0=pick(root.b,root.t), y1=pick(root.b,root.t);
    if(r.next()%3){ const float w=r.uni(0.f,(root.r-root.l)*0.2f), h=r.uni(0.f,(root.t-root.b)*0.2f); x1=x0+w; y1=y0+h; }
    const float cx=(root.r+root.l)*0.5f, cy=(root.b+root.t)*0.5f;
    if(r.next()%10==0)x0=cx; if(r.next()%10==0)x1=cx; if(r.next()%10==0)y0=cy; if(r.next()%10==0)y1=cy;
    QRect q{std::min(x0,x1),std::max(y0,y1),std::max(x0,x1),std::min(y0,y1)};
    if(r.next()%8==0)std::swap(q.l,q.r); if(r.next()%8==0)std::swap(q.t,q.b);
    if(r.next()%16==0)(&q.l)[r.next()%4]=QNAN;
    return q; }
inline CVector2D QueryVec(Rng& r,const QRect& root){
    const float cx=(root.r+root.l)*0.5f, cy=(root.b+root.t)*0.5f;
    CVector2D v{r.uni(root.l-50.f,root.r+50.f),r.uni(root.b-50.f,root.t+50.f)};
    if(r.next()%8==0)v.x=cx; if(r.next()%8==0)v.y=cy; if(r.next()%16==0)(r.next()%2 ? v.x : v.y)=QNAN;
    return v; }
inline const void* cbQuery{};
inline void __cdecl CbRect(const void* q,uintptr_t item){ trace.push_back({"cb",q==cbQuery,item}); }
inline json ListOut(void* head){ json l=json::array(); for(auto* n=static_cast<uint8*>(head);n;n=At<uint8*>(n,4)){ l.push_back({At<uintptr_t>(n,0),(n-pStore.data())/8}); if(l.size()>4096)break; } return l; }

inline json RunQuad(const std::string& nm,uintptr_t a,Rng& r,json& out) {
    auto& gP=*reinterpret_cast<uint8**>(0xB74484); auto& gQ=*reinterpret_cast<uint8**>(0xB745BC);
    uint8* const savedP=gP; uint8* const savedQ=gQ;
    ResetPool(r,qHdr.data(),qStore.data(),qStore.size(),qFlags.data(),NQ);
    ResetPool(r,pHdr.data(),pStore.data(),pStore.size(),pFlags.data(),NP);
    json ret=nullptr;
    if(nm=="InitPool"){
        for(auto& b:ipArena)b=uint8(r.next()); ipUsed=0; ipCalls=0; ipFail=r.next()%8==0; nrec=0;
        const bool present=r.next()%4==0; gQ=present ? qHdr.data() : nullptr;
        { ReversingProbe::EntryPatch n1(0x82119A,FakeSeqNew),n2(0x821195,FakeSeqNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__cdecl*)()>(a)(); }
        out["pool"]=gQ==qHdr.data() ? json("fake") : gQ ? json(uintptr_t(gQ)-uintptr_t(ipArena.data())) : json(nullptr);
        out["arena"]=Fnv(ipArena.data(),ipArena.size()); out["recs"]=Recs();
        if(gQ&&gQ!=qHdr.data())out["hdr"]=ReversingProbe::Bytes(gQ,0x14);
        gQ=savedQ; out["q"]=Fnv(qHdr.data(),qHdr.size()); return ret;
    }
    const QRect root=RootRect(r);
    if(nm=="FindSector-rect"||nm=="FindSector-vec"||nm=="InSector"){
        uint8* n=soloNode.data(); for(auto& b:soloNode)b=uint8(r.next()); std::memcpy(n,&root,16); At<uint32>(n,0x24)=r.next()%4==0 ? 0 : 1+r.next()%6;
        if(nm=="FindSector-rect"){ const QRect q=QueryRect(r,root); ret=reinterpret_cast<int32(__thiscall*)(void*,const void*)>(a)(n,&q); }
        else if(nm=="FindSector-vec"){ const CVector2D v=QueryVec(r,root); ret=reinterpret_cast<int32(__thiscall*)(void*,const void*)>(a)(n,&v); }
        else { const QRect q=QueryRect(r,root); const int32 s=int32(r.next()%4); ret=reinterpret_cast<bool(__thiscall*)(void*,const void*,int32)>(a)(n,&q,s); }
        out["node"]=ReversingProbe::Bytes(n,0x28); return ret;
    }
    // árbol en el pool falso
    const uint32 level=r.next()%4;
    const uint32 pct=nm=="DeleteItem-rect" ? 100 : nm=="AddItem" ? 30 : 55;
    uint8* const rootNode=Alloc(r,qStore.data(),qFlags.data(),NQ,0x28); Build(r,rootNode,root,level,pct);
    const QRect q=QueryRect(r,root); const CVector2D v=QueryVec(r,root); const uintptr_t item=Item(int(r.next()%10));
    void* listHead=nullptr; AddItems(r,listHead,int(r.next()%3));
    cbQuery=nm.ends_with("-vec") ? static_cast<const void*>(&v) : static_cast<const void*>(&q);
    gP=pHdr.data(); gQ=qHdr.data();
    if(nm=="AddItem") reinterpret_cast<void(__thiscall*)(void*,uintptr_t,const void*)>(a)(rootNode,item,&q);
    else if(nm=="DeleteItem-") reinterpret_cast<void(__thiscall*)(void*,uintptr_t)>(a)(rootNode,item);
    else if(nm=="DeleteItem-rect") reinterpret_cast<void(__thiscall*)(void*,uintptr_t,const void*)>(a)(rootNode,item,&q);
    else if(nm=="GetAll") reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(rootNode,&listHead);
    else if(nm=="GetAllMatching-rect") reinterpret_cast<void(__thiscall*)(void*,const void*,void*)>(a)(rootNode,&q,&listHead);
    else if(nm=="GetAllMatching-vec") reinterpret_cast<void(__thiscall*)(void*,const void*,void*)>(a)(rootNode,&v,&listHead);
    else if(nm=="ForAllMatching-rect") reinterpret_cast<void(__thiscall*)(void*,const void*,void*)>(a)(rootNode,&q,reinterpret_cast<void*>(&CbRect));
    else if(nm=="ForAllMatching-vec") reinterpret_cast<void(__thiscall*)(void*,const void*,void*)>(a)(rootNode,&v,reinterpret_cast<void*>(&CbRect));
    gP=savedP; gQ=savedQ;
    out["list"]=ListOut(listHead); out["root"]=(rootNode-qStore.data())/0x28;
    out["q"]={Fnv(qStore.data(),qStore.size()),Fnv(qFlags.data(),qFlags.size()),ReversingProbe::Bytes(qHdr.data(),qHdr.size())};
    out["p"]={Fnv(pStore.data(),pStore.size()),Fnv(pFlags.data(),pFlags.size()),ReversingProbe::Bytes(pHdr.data(),pHdr.size())};
    return ret;
}

// ======================= Game =======================
alignas(16) inline std::array<uint8,0x40> param{};
alignas(16) inline std::array<uint8,0x400> fakeCam{};
alignas(16) inline std::array<uint8,0x800> fakePed{};
inline std::array<void*,64> fakeDeviceVt{};
struct FakeDevice { void** vt; }; inline FakeDevice fakeDevice{};
inline json Arg(uint32 v) {
    if(!v)return 0;
    if(v==uintptr_t(fakeCam.data()))return "fakeCamera";
    if(v==uintptr_t(TheCamera.m_pRwCamera))return "TheCamera.m_pRwCamera";
    if(v==uintptr_t(&AudioEngine))return "AudioEngine";
    if(v==uintptr_t(&TheCamera))return "TheCamera";
    if(v==uintptr_t(&FrontEndMenuManager))return "FrontEndMenuManager";
    if(v==uintptr_t(&CUserDisplay::OnscnTimer))return "OnscnTimer";
    if(v==uintptr_t(&g_breakMan))return "g_breakMan";
    if(v==uintptr_t(&g_fx))return "g_fx";
    if(v==uintptr_t(fakePed.data()))return "fakePed";
    if(v==uintptr_t(&fakeDevice))return "fakeDevice";
    if(v>=uintptr_t(param.data())&&v<uintptr_t(param.data())+param.size())return "param+"+std::to_string(v-uintptr_t(param.data()));
    return v; }
// dependencias genéricas: cdecl (hasta 5 argumentos leídos) o thiscall sin argumentos (se registra `this`)
struct Dep { const char* name; uintptr_t exe; uintptr_t dll; int nargs; uint32 ret; bool thiscall; uint8 str; };
inline std::vector<Dep> deps;
inline Dep& D(const char* n){ for(auto& d:deps)if(!strcmp(d.name,n))return d; std::abort(); }
inline json Args(int id,const uint32* a) {
    json r=json::array();
    for(int k=0;k<deps[id].nargs;++k){ if(deps[id].str>>k&1)r.push_back(a[k] ? json(std::string(reinterpret_cast<const char*>(a[k]))) : json(nullptr)); else r.push_back(Arg(a[k])); }
    return r; }
template<int ID> uint32 __cdecl Stub(uint32 a0,uint32 a1,uint32 a2,uint32 a3,uint32 a4) { const uint32 a[5]{a0,a1,a2,a3,a4}; trace.push_back({deps[ID].name,Args(ID,a)}); return deps[ID].ret; }
template<int ID> uint32 __fastcall StubT(void* self,void*) { trace.push_back({deps[ID].name,Arg(uint32(uintptr_t(self)))}); return deps[ID].ret; }
template<int... I> void RegisterStubs(std::integer_sequence<int,I...>,Patches& p) {
    ([&]{ if(I<int(deps.size())){ const auto& d=deps[I];
        if(d.thiscall){ p.Add(d.exe,&StubT<I>); p.Add(d.dll,&StubT<I>); } else { p.Add(d.exe,&Stub<I>); p.Add(d.dll,&Stub<I>); } } }(),...); }
inline RwBool __cdecl RenderState(RwRenderState s,void* v) { trace.push_back({"render_state",int(s),uintptr_t(v)}); return TRUE; }
inline void __fastcall BreakRender(void* self,void*,uint8 state) { trace.push_back({"BreakManager_c::Render",Arg(uint32(uintptr_t(self))),state}); }
inline void __fastcall FxRender(void* self,void*,RwCamera* cam,bool heatHaze) { trace.push_back({"Fx_c::Render",Arg(uint32(uintptr_t(self))),Arg(uint32(uintptr_t(cam))),heatHaze}); }
inline void __cdecl DrawRect(const CRect* rc,const CRGBA* col) { trace.push_back({"CSprite2d::DrawRect",ReversingProbe::Bytes(rc,16),ReversingProbe::Bytes(col,4)}); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"FindPlayerPed",id}); return reinterpret_cast<CPlayerPed*>(fakePed.data()); }
inline void __stdcall FakeSetGammaRamp(void* self,UINT sw,DWORD flags,const void* ramp) { trace.push_back({"SetGammaRamp",self==&fakeDevice,sw,flags,ramp==&CGamma::ms_SavedGamma}); }
inline void __stdcall FakeDeviceTrap(void* self) { trace.push_back({"device_trap"}); }
inline int __cdecl SilentPrintf(const char*,...) { return 0; }
inline void __fastcall SilentThis(void*,void*) {}
// Render2dStuff (0x53E230) tiene además un HookInstall permanente (InjectHooksMain.cpp, ImGui) al mismo destino de la
// DLL: si se desactiva su hook reversible, la entrada de la DLL salta al EXE y el EXE salta a la DLL (bucle infinito).
// Por eso su hook se deja siempre activo y el original se ejecuta reponiendo durante la llamada los 5 bytes originales
// de la entrada del EXE (mov eax,[0xC97B24]).
struct ExeEntryBytes {
    uintptr_t at; std::array<uint8,5> saved{};
    ExeEntryBytes(uintptr_t a,std::array<uint8,5> bytes) : at(a) { std::memcpy(saved.data(),reinterpret_cast<void*>(at),5); Put(bytes); }
    void Put(const std::array<uint8,5>& b) const { void* p=reinterpret_cast<void*>(at); DWORD old{},ignored{};
        if(!VirtualProtect(p,5,PAGE_EXECUTE_READWRITE,&old))std::abort(); std::memcpy(p,b.data(),5); VirtualProtect(p,5,old,&ignored); FlushInstructionCache(GetCurrentProcess(),p,5); }
    ~ExeEntryBytes(){ Put(saved); }
};
constexpr uintptr_t RENDER2D=0x53E230;
inline const std::array<uint8,5> RENDER2D_ORIGINAL{0xA1,0x24,0x7B,0xC9,0x00};

struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(void* p,size_t n){ regions.push_back({p,n,ReversingProbe::Bytes(p,n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

inline void SetupGameDeps() {
    using RS=CPlayerPed;
    deps={
        // RenderEffects
        {"CBirds::Render",0x712810,Addr(&CBirds::Render),0,0,false,0},{"CSkidmarks::Render",0x720640,Addr(&CSkidmarks::Render),0,0,false,0},
        {"CRopes::Render",0x556AE0,Addr(&CRopes::Render),0,0,false,0},{"CGlass::Render",0x71CE20,Addr(&CGlass::Render),0,0,false,0},
        {"CMovingThings::Render",0x717150,Addr(&CMovingThings::Render),0,0,false,0},
        {"CVisibilityPlugins::RenderReallyDrawLastObjects",0x733800,Addr(&CVisibilityPlugins::RenderReallyDrawLastObjects),0,0,false,0},
        {"CCoronas::Render",0x6FAEC0,Addr(&CCoronas::Render),0,0,false,0},{"CWaterCannons::Render",0x729B30,Addr(&CWaterCannons::Render),0,0,false,0},
        {"CWaterLevel::RenderWaterFog",0x6E7760,Addr(&CWaterLevel::RenderWaterFog),0,0,false,0},
        {"CClouds::MovingFogRender",0x716C90,Addr(&CClouds::MovingFogRender),0,0,false,0},{"CClouds::VolumetricCloudsRender",0x716380,Addr(&CClouds::VolumetricCloudsRender),0,0,false,0},
        {"CHeli::Pre_SearchLightCone",0x6C4650,Addr(&CHeli::Pre_SearchLightCone),0,0,false,0},{"CHeli::RenderAllHeliSearchLights",0x6C7C50,Addr(&CHeli::RenderAllHeliSearchLights),0,0,false,0},
        {"CTheScripts::RenderAllSearchLights",0x493E30,Addr(&CTheScripts::RenderAllSearchLights),0,0,false,0},{"CHeli::Post_SearchLightCone",0x6C46E0,Addr(&CHeli::Post_SearchLightCone),0,0,false,0},
        {"CWeaponEffects::Render",0x742CF0,Addr(&CWeaponEffects::Render),0,0,false,0},
        {"CPlayerPed::DrawTriangleForMouseRecruitPed",0x60BA80,Addr(&RS::DrawTriangleForMouseRecruitPed),0,0,true,0},
        {"CSpecialFX::Render",0x726AD0,Addr(&CSpecialFX::Render),0,0,false,0},{"CVehicleRecording::Render",0x459F70,Addr(&CVehicleRecording::Render),0,0,false,0},
        {"CPointLights::RenderFogEffect",0x7002D0,Addr(&CPointLights::RenderFogEffect),0,0,false,0},
        {"CRenderer::RenderFirstPersonVehicle",0x553D00,Addr(&CRenderer::RenderFirstPersonVehicle),0,0,false,0},{"CPostEffects::Render",0x7046E0,Addr(&CPostEffects::Render),0,0,false,0},
        // RenderScene
        {"CMovingThings::Render_BeforeClouds",0x7178F0,Addr(&CMovingThings::Render_BeforeClouds),0,0,false,0},{"CClouds::Render",0x713950,Addr(&CClouds::Render),0,0,false,0},
        {"CCarFXRenderer::PreRenderUpdate",0x5D5B10,Addr(&CCarFXRenderer::PreRenderUpdate),0,0,false,0},{"CRenderer::RenderRoads",0x553A10,Addr(&CRenderer::RenderRoads),0,0,false,0},
        {"CCoronas::RenderReflections",0x6FB630,Addr(&CCoronas::RenderReflections),0,0,false,0},{"CRenderer::RenderEverythingBarRoads",0x553AA0,Addr(&CRenderer::RenderEverythingBarRoads),0,0,false,0},
        {"CRenderer::RenderFadingInUnderwaterEntities",0x553220,Addr(&CRenderer::RenderFadingInUnderwaterEntities),0,0,false,0},
        {"CWaterLevel::RenderWater",0x6EF650,Addr(&CWaterLevel::RenderWater),0,0,false,0},{"CRenderer::RenderFadingInEntities",0x5531E0,Addr(&CRenderer::RenderFadingInEntities),0,0,false,0},
        {"RwCameraEndUpdate",0x7EE180,Addr(&RwCameraEndUpdate),1,0,false,0},{"RwCameraSetNearClipPlane",0x7EE1D0,Addr(&RwCameraSetNearClipPlane),2,0,false,0},
        {"RwCameraBeginUpdate",0x7EE190,Addr(&RwCameraBeginUpdate),1,0,false,0},
        {"CShadows::UpdateStaticShadows",0x707F40,Addr(&CShadows::UpdateStaticShadows),0,0,false,0},{"CShadows::RenderStaticShadows",0x708300,Addr(&CShadows::RenderStaticShadows),0,0,false,0},
        {"CShadows::RenderStoredShadows",0x70A960,Addr(&CShadows::RenderStoredShadows),0,0,false,0},{"CPlantMgr::Render",0x5DBAE0,Addr(&CPlantMgr::Render),0,0,false,0},
        {"CClouds::RenderBottomFromHeight",0x7154B0,Addr(&CClouds::RenderBottomFromHeight),0,0,false,0},{"CWeather::RenderRainStreaks",0x72AF70,Addr(&CWeather::RenderRainStreaks),0,0,false,0},
        {"CCoronas::RenderSunReflection",0x6FBAA0,Addr(&CCoronas::RenderSunReflection),0,0,false,0},
        {"CStencilShadows::RenderStencilShadows",0x7113B0,Addr(&CStencilShadows::RenderStencilShadows),0,0,false,0},
        // Render2dStuff
        {"CReplay::Display",0x45C210,Addr(&CReplay::Display),0,0,false,0},{"CPickups::RenderPickUpText",0x455000,Addr(&CPickups::RenderPickUpText),0,0,false,0},
        {"CCamera::DrawBordersForWideScreen",0x514860,Addr(&CCamera::DrawBordersForWideScreen),0,0,true,0},
        {"CAudioEngine::DisplayRadioStationName",0x507030,Addr(&CAudioEngine::DisplayRadioStationName),0,0,true,0},
        {"CHud::Draw",0x58FAE0,Addr(&CHud::Draw),0,0,false,0},{"CSpecialFX::Render2DFXs",0x721660,Addr(&CSpecialFX::Render2DFXs),0,0,false,0},
        {"COnscreenTimer::ProcessForDisplay",0x44CC20,Addr(&COnscreenTimer::ProcessForDisplay),0,0,true,0},
        {"CMessages::Display",0x69EFC0,Addr(&CMessages::Display),1,0,false,0},{"CDarkel::DrawMessages",0x43CEC0,Addr(&CDarkel::DrawMessages),0,0,false,0},
        {"CGarages::PrintMessages",0x447790,Addr(&CGarages::PrintMessages),0,0,false,0},{"CFont::DrawFonts",0x71A210,Addr(&CFont::DrawFonts),0,0,false,0},
        // RenderMenus / RenderDebugShit
        {"CMenuManager::DrawFrontEnd",0x57C290,Addr(&CMenuManager::DrawFrontEnd),0,0,true,0},
        {"CTheScripts::RenderTheScriptDebugLines",0,Addr(&CTheScripts::RenderTheScriptDebugLines),0,0,false,0},{"DefinedState",0x734650,Addr(&DefinedState),0,0,false,0},
        // InitialiseGame / TheGame
        {"CGame::Initialise",0x53BC80,Addr(&CGame::Initialise),1,0,false,1},{"CAudioEngine::Restart",0x506DB0,Addr(&CAudioEngine::Restart),0,0,true,0},
        {"CTimer::Initialise",0x5617E0,Addr(&CTimer::Initialise),0,0,false,0},{"CLoadingScreen::SetLoadingBarMsg",0x590240,Addr(&CLoadingScreen::SetLoadingBarMsg),2,0,false,3},
        {"CLoadingScreen::NewChunkLoaded",0x590D00,Addr(&CLoadingScreen::NewChunkLoaded),0,0,false,0},
        // GameInit / RwInitialize / RwTerminate
        {"RsRwInitialize",0x619C90,Addr(&RsRwInitialize),1,1,false,0},{"CGame::InitialiseRenderWare",0x5BD600,Addr(&CGame::InitialiseRenderWare),0,1,false,0},
        {"RsTimer",0x619410,Addr(&RsTimer),0,0,false,0},{"srand",0x821B11,Addr(&srand),1,0,false,0},
        {"CGame::ShutdownRenderWare",0x53BB80,Addr(&CGame::ShutdownRenderWare),0,0,false,0},{"RwD3D9GetCurrentD3DDevice",0x7F9D50,Addr(&RwD3D9GetCurrentD3DDevice),0,0,false,0},
        {"RsRwTerminate",0x6195E0,Addr(&RsRwTerminate),0,0,false,0},
    };
}

inline json RunGame(const std::string& nm,uintptr_t a,Rng& r,json& out) {
    auto& fpState=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; fpState=RenderState;
    json ret=nullptr;
    if(nm=="GameInit"||nm=="RwInitialize"){
        D("RsRwInitialize").ret=r.next()%4!=0; D("CGame::InitialiseRenderWare").ret=r.next()%2; D("RsTimer").ret=r.next();
        if(nm=="GameInit") reinterpret_cast<void(__cdecl*)()>(a)();
        else { void* p=r.next()%3 ? param.data()+r.next()%16 : nullptr; ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(p); } }
    else if(nm=="RwTerminate"){ gbGammaChanged=r.next()%2; D("RwD3D9GetCurrentD3DDevice").ret=r.next()%4 ? uint32(uintptr_t(&fakeDevice)) : 0;
        reinterpret_cast<void(__cdecl*)()>(a)(); out["gamma"]=gbGammaChanged; }
    else if(nm=="InitialiseGame"){ auto& version=*reinterpret_cast<int16*>(0xB72C68); version=int16(r.next());
        FrontEndMenuManager.m_bLoadingData=r.next()%2; FrontEndMenuManager.m_bStartGameLoading=r.next()%2;
        reinterpret_cast<void(__cdecl*)()>(a)(); out["state"]={version,FrontEndMenuManager.m_bLoadingData,FrontEndMenuManager.m_bStartGameLoading}; }
    else if(nm=="TheGame"||nm=="RenderDebugShit") reinterpret_cast<void(__cdecl*)()>(a)();
    else if(nm=="RenderMenus"){ FrontEndMenuManager.m_bMenuActive=r.next()%2; reinterpret_cast<void(__cdecl*)()>(a)(); }
    else if(nm=="RenderEffects"){
        CHeli::NumberOfSearchLights=r.next()%3==0 ? 1+r.next()%3 : 0; CTheScripts::NumberOfScriptSearchLights=uint16(r.next()%3==0 ? 1+r.next()%3 : 0);
        CReplay::Mode=eReplayMode(r.next()%3); static const uint16 dpc[]{0,0,1,0x100,0xFF};
        CPad::GetPad(0)->DisablePlayerControls=dpc[r.next()%5];
        reinterpret_cast<void(__cdecl*)()>(a)(); }
    else if(nm=="RenderScene"){
        static const float uw[]{0.f,-0.f,0.5f,1.f,-0.25f,1e-30f}; CWeather::UnderWaterness=r.next()%8==0 ? QNAN : r.next()%3 ? uw[r.next()%6] : r.uni(-1.f,1.f);
        CMirrors::TypeOfMirror=eMirrorType(r.next()%3==0 ? 1+r.next()%2 : 0); CMirrors::bRenderingReflection=r.next()%4==0;
        for(auto& b:fakeCam)b=uint8(r.next()); At<float>(fakeCam.data(),0x80)=r.uni(0.05f,3.f); At<float>(fakeCam.data(),0x84)=r.uni(50.f,3000.f);
        Scene.m_pRwCamera=reinterpret_cast<RwCamera*>(fakeCam.data());
        static const float fz[]{0.f,-0.f,1.f,-1.f,0.25f}; CCamera::GetActiveCamera().m_vecFront.z=r.next()%10==0 ? QNAN : r.next()%3==0 ? fz[r.next()%5] : r.uni(-1.f,1.f);
        reinterpret_cast<void(__cdecl*)()>(a)(); }
    else if(nm=="Render2dStuff"){
        TheCamera.m_bWideScreenOn=r.next()%2; FrontEndMenuManager.m_bWidescreenOn=r.next()%2;
        static const int32 ws[]{640,800,1024,1280,1366,1600,1920,1081,767,2561}, hs[]{480,600,768,720,1080,1081,601,1440,999,767};
        RsGlobal.maximumWidth=ws[r.next()%10]; RsGlobal.maximumHeight=hs[r.next()%10];
        static const uint16 modes[]{7,39,8,40,46,45,4,53,16,18};
        CCamera::GetActiveCamera().m_nMode=eCamMode(modes[r.next()%10]);
        for(auto& b:fakePed)b=uint8(r.next()); const uint8 slot=uint8(r.next()%13); fakePed[0x718]=slot;
        static const int32 wt[]{19,34,19,34,35,0,22,36};
        At<int32>(fakePed.data(),0x5A0+slot*0x1C)=wt[r.next()%8];
        reinterpret_cast<void(__cdecl*)()>(a)(); }
    RestoreRegions();
    return ret;
}

// ======================= tabla =======================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    const std::string cat=!strcmp(cls,"CQuadTreeNode") ? "CQuadTreeNode<T>" : cls;
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cat)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=34;
inline const std::array<Fn,NFN> fns{{
    {"CStreamedScripts","Initialise",0x470660,0,-1,64},{"CStreamedScripts","ReInitialise",0x4706A0,0,-1,64},
    {"CStreamedScripts","FindStreamedScript",0x470740,0,-1,512},{"CStreamedScripts","FindStreamedScriptQuiet",0x4706F0,0,-1,512},
    {"CStreamedScripts","GetProperIndexFromIndexUsedByScript",0x470810,0,-1,512},{"CStreamedScripts","GetStreamedScriptFilename",0x470900,0,-1,256},
    {"CStreamedScripts","GetStreamedScriptWithThisStartAddress",0x470910,0,-1,512},{"CStreamedScripts","RegisterScript",0x4706C0,0,-1,256},
    {"CStreamedScripts","RemoveStreamedScriptFromMemory",0x4708E0,0,-1,256},{"CStreamedScripts","LoadStreamedScript",0x470840,0,-1,256},
    {"CStreamedScripts","StartNewStreamedScript",0x470890,0,-1,256},{"CStreamedScripts","ReadStreamedScriptData",0x470750,0,-1,256},
    {"CQuadTreeNode","InitPool",0x552C00,0,-1,64},{"CQuadTreeNode","FindSector-rect",0x5525A0,0,-1,1024},{"CQuadTreeNode","FindSector-vec",0x552640,0,-1,1024},
    {"CQuadTreeNode","InSector",0x5526A0,0,-1,1024},{"CQuadTreeNode","AddItem",0x552CD0,0,-1,512},{"CQuadTreeNode","DeleteItem-",0x552A40,0,-1,512},
    {"CQuadTreeNode","DeleteItem-rect",0x552A90,0,-1,512},{"CQuadTreeNode","GetAll",0x552870,0,-1,256},
    {"CQuadTreeNode","GetAllMatching-rect",0x5528C0,0,-1,512},{"CQuadTreeNode","GetAllMatching-vec",0x552930,0,-1,512},
    {"CQuadTreeNode","ForAllMatching-rect",0x552980,0,-1,512},{"CQuadTreeNode","ForAllMatching-vec",0x5529F0,0,-1,512},
    {"Game","GameInit",0x5BF3B0,0,-1,64},{"Game","RwInitialize",0x5BF390,0,-1,64},{"Game","RwTerminate",0x53D910,0,-1,64},
    {"Game","InitialiseGame",0x53E580,0,-1,64},{"Game","TheGame",0x53DF10,0,-1,16},{"Game","RenderMenus",0x53E530,0,-1,64},
    {"Game","RenderDebugShit",0x53E160,0,-1,16},{"Game","RenderEffects",0x53E170,0,-1,256},{"Game","RenderScene",0x53DF40,0,-1,512},
    {"Game","Render2dStuff",0x53E230,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"sys12-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"sys12-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("sys12");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"sys12-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // bloque de scripts en streaming del SCM cargado (solo lectura): tres saltos fijos, como el EXE
    { const uint8* base=CTheScripts::ScriptSpace.data(); auto J=[&](uint32 o){ return *reinterpret_cast<const uint32*>(base+3+o); };
      const uint8* data=base+J(J(J(0)))+8; realEntries=data+8; realCount=std::clamp(*reinterpret_cast<const int32*>(data+4),0,82); }

    // estado global guardado
    regions.clear();
    Snap(&RWSRCGLOBAL(dOpenDevice).fpRenderStateSet,4); Snap(&Scene.m_pRwCamera,4); Snap(&CWeather::UnderWaterness,4);
    Snap(&CMirrors::TypeOfMirror,4); Snap(&CMirrors::bRenderingReflection,1);
    Snap(&CCamera::GetActiveCamera().m_vecFront.z,4); Snap(&CCamera::GetActiveCamera().m_nMode,sizeof(eCamMode));
    Snap(&CHeli::NumberOfSearchLights,4); Snap(&CTheScripts::NumberOfScriptSearchLights,2); Snap(&CReplay::Mode,1);
    Snap(&CPad::GetPad(0)->DisablePlayerControls,2);
    Snap(&FrontEndMenuManager.m_bMenuActive,1); Snap(&FrontEndMenuManager.m_bWidescreenOn,1); Snap(&FrontEndMenuManager.m_bLoadingData,1);
    Snap(&FrontEndMenuManager.m_bStartGameLoading,1); Snap(&TheCamera.m_bWideScreenOn,1);
    Snap(&RsGlobal.maximumWidth,4); Snap(&RsGlobal.maximumHeight,4); Snap(reinterpret_cast<void*>(0xB72C68),2); Snap(&gbGammaChanged,1);
    Snap(reinterpret_cast<void*>(0xB74484),4); Snap(reinterpret_cast<void*>(0xB745BC),4);

    for(auto& v:fakeDeviceVt)v=reinterpret_cast<void*>(&FakeDeviceTrap); fakeDeviceVt[21]=reinterpret_cast<void*>(&FakeSetGammaRamp); fakeDevice.vt=fakeDeviceVt.data();
    SetupGameDeps();

    bool passed=true; int pair=0;
    {
        using std::bit_cast;
        Patches patches;
        RegisterStubs(std::make_integer_sequence<int,80>{},patches);
        patches.Add(0x59E6A0,BreakRender); patches.Add(Addr(&BreakManager_c::Render),BreakRender);
        patches.Add(0x49E650,FxRender); patches.Add(Addr(&Fx_c::Render),FxRender);
        patches.Add(0x727B60,DrawRect); patches.Add(Addr(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),DrawRect);
        patches.Add(0x56E210,FakeFindPlayerPed); patches.Add(Addr(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed);
        patches.Add(0x821982,SilentPrintf); patches.Add(Addr(&DebugModules::Render3D),SilentThis); // ya no se llama desde RenderEffects (menú ImGui retirado); el parche queda inerte
        patches.Add(0x464FF0,FakeCheckVersion); patches.Add(Addr(&CTheScripts::CheckStreamedScriptVersion),FakeCheckVersion);
        patches.Add(0x7EC9D0,FakeStreamRead); patches.Add(Addr(static_cast<RwUInt32(*)(RwStream*,void*,RwUInt32)>(&RwStreamRead)),FakeStreamRead);
        patches.Add(0x464C20,FakeStartNewScript); patches.Add(Addr(static_cast<CRunningScript*(*)(uint8*)>(&CTheScripts::StartNewScript)),FakeStartNewScript);
        patches.Add(0x409C90,FakeDoesntRequire); patches.Add(Addr(&CStreaming::SetMissionDoesntRequireModel),FakeDoesntRequire);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    Rng r{uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A12u}; if(!r.s)r.s=1;
                    trace=json::array(); nrec=0;
                    for(size_t k=0;k<hooks.size();++k)hooks[k]->State(fns[k].address==RENDER2D ? true : candidate);
                    std::optional<ExeEntryBytes> exeEntry; if(fn.address==RENDER2D&&!candidate)exeEntry.emplace(RENDER2D,RENDER2D_ORIGINAL);
                    const bool switches=fn.address==RENDER2D ? (candidate ? ReversingProbe::JumpsToDll(RENDER2D) : *reinterpret_cast<const uint8*>(RENDER2D)==0xA1)
                                                             : ReversingProbe::JumpsToDll(fn.address)==candidate;
                    json out={{"switches",switches}}; json ret=nullptr;
                    if(cls=="CStreamedScripts") ret=RunScripts(nm,fn.address,r,out);
                    else if(cls=="CQuadTreeNode") ret=RunQuad(nm,fn.address,r,out);
                    else ret=RunGame(nm,fn.address,r,out);
                    exeEntry.reset();
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()}});
}
}
