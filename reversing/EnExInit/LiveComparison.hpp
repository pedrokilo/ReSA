#pragma once
// Banco dentro del motor: CEntryExitManager::Init como el EXE (pool de 400 entradas) y las funciones que dependen
// del tamano del pool y que se cambiaron con el: Save (recorre 400 slots fijos con indices crudos) y Shutdown
// (destruye el pool en linea, sin ~CPool). Original (hooks desactivados, codigo del EXE) frente a candidato.
// Init: los punteros globales del gestor se rellenan con basura antes de la llamada y se restauran; operator new y
// new[] (0x82119A/0x821195) se parchean SOLO durante la llamada con un asignador de arena fijo (trazas sin
// reservas) cuya memoria se rellena con un patron por caso (se ve que el almacenamiento no se rellena); el nodo raiz
// sale del pool real de nodos del arbol, cuyo m_LastFreeSlot se elige por caso (a veces con el pool lleno) y cuyo
// almacenamiento y mapa se restauran tras cada ejecucion. Save/Shutdown usan los fixtures de World18 (pool falso
// de 400 entradas, arbol con nodos reales, Save/delete simulados) con indices y enlaces fuera de rango o desalineados.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "EntryExit.h"
#include "EntryExitManager.h"
#include "Core/QuadTreeNode.h"
#include "GenericGameStorage.h"
#include "CutsceneMgr.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"

namespace EnExInitLive {
using TreeNode = CQuadTreeNode<CEntryExit*>;
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr int32 POOL_SIZE=400; constexpr size_t ENEX=0x3C, NODE=0x28;
constexpr uintptr_t GLOBALS=0x96A720, GLOBALS_SIZE=0xC0; // pila .. ms_numVisibleEntities (incluye los estaticos de CEntryExit)

// --- fixtures ---
alignas(16) inline std::array<uint8,POOL_SIZE*ENEX> storage{};
inline std::array<uint8,POOL_SIZE> states{};
alignas(16) inline std::array<uint8,0x20> poolHdr{};
alignas(16) inline std::array<uint8,0x6400> arena{}; // memoria de operator new/new[] durante Init
inline TreeNode* root{};
inline std::vector<int32> usedSlots;
inline uint8* quadStorage{}; inline uint8* quadStates{}; inline int32 quadCap{};
inline uint8* G() { return reinterpret_cast<uint8*>(GLOBALS); }
inline uint8* Slot(int32 i) { return storage.data()+size_t(i)*ENEX; }
inline CEntryExit* Enex(int32 i) { return reinterpret_cast<CEntryExit*>(Slot(i)); }

inline std::string Name(const void* p) {
    const auto u=uintptr_t(p);
    if(!u) return "null";
    const auto s=uintptr_t(storage.data());
    if(u>=s&&u<s+storage.size()){ const auto o=u-s; return "e"+std::to_string(o/ENEX)+(o%ENEX ? "+"+std::to_string(o%ENEX) : std::string()); }
    if(u==uintptr_t(states.data())) return "states";
    if(u==uintptr_t(poolHdr.data())) return "pool";
    const auto a=uintptr_t(arena.data());
    if(u>=a&&u<a+arena.size()) return "arena+"+std::to_string(u-a);
    const auto q=uintptr_t(quadStorage);
    if(quadStorage&&u>=q&&u<q+size_t(quadCap)*NODE){ const auto o=u-q; return "node"+std::to_string(o/NODE)+(o%NODE ? "+"+std::to_string(o%NODE) : std::string()); }
    if(u>=GLOBALS&&u<GLOBALS+GLOBALS_SIZE) return "g"+std::to_string(u-GLOBALS);
    if(u<s&&s-u<0x1000) return "e-"+std::to_string(s-u);
    return "other";
}

// --- operator new/new[] durante Init: arena fija, sin reservas; las trazas van a un array fijo ---
struct Rec { uint32 tag; uint32 a; uintptr_t p; };
inline std::array<Rec,64> recs{}; inline size_t nrec{};
inline size_t bump{}; inline uint32 newCalls{}, failMask{};
inline void* Alloc(uint32 tag,size_t n) {
    const uint32 k=newCalls++;
    void* p=nullptr;
    if(!((failMask>>k)&1)&&bump+n<=arena.size()){ p=arena.data()+bump; bump=(bump+n+16+15)&~size_t(15); }
    if(nrec<recs.size()) recs[nrec++]={tag,uint32(n),uintptr_t(p)};
    return p;
}
inline void* __cdecl FakeNew(size_t n) { return Alloc(1,n); }
inline void* __cdecl FakeNewArr(size_t n) { return Alloc(2,n); }
// --- operator delete/delete[] durante Shutdown: sin liberar ---
inline void __cdecl FakeDelete(void* p) { if(nrec<recs.size())recs[nrec++]={3,0,uintptr_t(p)}; }
inline void __cdecl FakeDeleteArr(void* p) { if(nrec<recs.size())recs[nrec++]={4,0,uintptr_t(p)}; }
inline json Recs() { json r=json::array(); for(size_t k=0;k<nrec;++k) r.push_back({recs[k].tag,recs[k].a,Name(reinterpret_cast<void*>(recs[k].p))}); return r; }

// --- trabajo de guardado (sin ficheros) ---
inline bool __cdecl FakeSaveWB(void* p,int32 n) { trace.push_back({"save_wb",Name(p),ReversingProbe::Bytes(p,size_t(n))}); return true; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,3> fns{{
    {"CEntryExitManager","Init",0x43F880,0,-1,1024},
    {"CEntryExitManager","Save",0x5D5970,0,-1,768},
    {"CEntryExitManager","Shutdown",0x440B90,0,-1,256},
}};

// Pool falso (como World18): todos los slots con campos finitos, minUsed..maxUsed ocupados
static const char* const kNames[]{"BARBERS","barbers","GYM1","Gym1","PDOMES","CSCHP","LONGNAM8","BAR","bar","8BALL","AMMUN1","ammun1"};
template<class N,class U> inline void BuildPool(N& next,U& uni,int32 minUsed,int32 maxUsed,float spread) {
    for(int32 i=0;i<POOL_SIZE;++i){
        uint8* e=Slot(i);
        for(size_t k=0;k<ENEX;k+=4){ const uint32 v=next(); std::memcpy(e+k,&v,4); }
        const char* nm=kNames[next()%std::size(kNames)]; const size_t len=std::min<size_t>(std::strlen(nm),8); std::memcpy(e,nm,len); if(len<8)e[len]=0;
        const float cx=uni(-spread,spread), cy=uni(-spread,spread), w=uni(0.5f,12.f), h=uni(0.5f,12.f);
        At<float>(e,0x08)=cx-w*0.5f; At<float>(e,0x0C)=cy+h*0.5f; At<float>(e,0x10)=cx+w*0.5f; At<float>(e,0x14)=cy-h*0.5f;
        At<float>(e,0x18)=uni(0.f,30.f); At<float>(e,0x1C)=next()%2 ? 0.f : uni(-3.2f,3.2f);
        At<float>(e,0x20)=uni(-spread,spread); At<float>(e,0x24)=uni(-spread,spread); At<float>(e,0x28)=uni(0.f,30.f); At<float>(e,0x2C)=uni(-360.f,360.f);
        At<uint16>(e,0x30)=uint16(next());
        e[0x32]=next()%8 ? uint8(next()%4) : (next()%2 ? uint8(200) : uint8(13));
        e[0x34]=uint8(next()%25); e[0x35]=uint8(next()%25);
        states[i]=uint8(0x80|(next()&0x7F));
    }
    usedSlots.clear();
    const int32 n=minUsed+int32(next()%uint32(maxUsed-minUsed+1));
    while(int32(usedSlots.size())<n){ const int32 i=next()%3 ? int32(next()%48) : int32(next()%POOL_SIZE); if(states[i]&0x80){ states[i]&=0x7F; usedSlots.push_back(i); } }
    for(int32 i=0;i<POOL_SIZE;++i){ void* l=nullptr; const uint32 r=next()%5;
        if((r==2||r==3)&&!usedSlots.empty())l=Enex(usedSlots[next()%usedSlots.size()]); else if(r==4)l=Enex(int32(next()%POOL_SIZE));
        At<void*>(Slot(i),0x38)=l; }
    poolHdr.fill(0);
    At<void*>(poolHdr.data(),0)=storage.data(); At<void*>(poolHdr.data(),4)=states.data(); At<int32>(poolHdr.data(),8)=POOL_SIZE;
    At<int32>(poolHdr.data(),0xC)=int32(next()%(POOL_SIZE+1))-1; poolHdr[0x10]=uint8(next()%2); poolHdr[0x11]=uint8(next()%2);
}
template<class N> inline int32 PickUsed(N& next) { return usedSlots.empty() ? int32(next()%POOL_SIZE) : usedSlots[next()%usedSlots.size()]; }
// Raiz del arbol desde el pool real de nodos; las entradas ocupadas se anaden casi todas con su rectangulo
template<class N> inline bool BuildTree(N& next) {
    root=new TreeNode(CRect{-512.f,-512.f,512.f,512.f},int32(next()%4));
    if(!root) return false;
    for(int32 i:usedSlots) if(next()%8){ auto* e=Enex(i); const CRect r=e->m_recEntrance; root->AddItem(e,r); }
    return true;
}
// Puntero "raro" para la pila o un enlace de Save: el EXE solo calcula (p - base) / 0x3C con signo, sin desreferenciar
template<class N> inline void* OddPtr(N& next) {
    const auto s=uintptr_t(storage.data());
    switch(next()%6){
    case 0:  return nullptr;
    case 1:  return reinterpret_cast<void*>(s-1-next()%(ENEX-1));                  // justo antes del almacenamiento: indice 0
    case 2:  return reinterpret_cast<void*>(s-ENEX*(1+next()%4)-next()%ENEX);       // antes del almacenamiento: indice negativo
    case 3:  return Slot(int32(next()%POOL_SIZE))+1+next()%(ENEX-1);              // desalineado dentro del pool
    case 4:  return reinterpret_cast<void*>(s+storage.size()+next()%(4*ENEX));       // pasado el final
    default: return reinterpret_cast<void*>(uintptr_t(0x1000+next()%0x10000));     // ajeno al pool
    }
}
inline json Globals() {
    uint8* g=G();
    json stack=json::array(); for(int k=0;k<4;++k) stack.push_back(Name(At<void*>(g,k*4)));
    return {{"stack",stack},{"burglary",ReversingProbe::Bytes(g+0x10,4)},{"posn",At<int32>(g,0xA4)},{"disabled",ReversingProbe::Bytes(g+0xA8,4)},
            {"state",At<int32>(g,0xAC)},{"tree",Name(At<void*>(g,0xB0))},{"active",Name(At<void*>(g,0xB4))},{"pool",Name(At<void*>(g,0xB8))},
            {"raw",ReversingProbe::Bytes(g,GLOBALS_SIZE)}};
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"enexinit-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"enexinit-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("enexinit");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"enexinit-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,3> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // pools reales de nodos (arbol y listas): se guarda el pool de nodos del arbol completo (almacenamiento, mapa y
    // m_LastFreeSlot) y el m_LastFreeSlot del pool de CPtrNodeSingleLink
    auto* quadPool=TreeNode::GetPool(); auto* ptrPool=GetPtrNodeSingleLinkPool();
    auto& quadLast=At<int32>(quadPool,0xC); auto& ptrLast=At<int32>(ptrPool,0xC); const int32 savedQuadLast=quadLast, savedPtrLast=ptrLast;
    quadStorage=At<uint8*>(quadPool,0); quadStates=At<uint8*>(quadPool,4); quadCap=At<int32>(quadPool,8);
    const size_t quadFree=quadPool->GetNoOfFreeSpaces();
    if(quadFree<64||!quadStorage||!quadStates||quadCap<=0){Write({{"event","error"},{"reason","quadtree node pool unusable"},{"free",quadFree}});return;}
    const std::vector<uint8> savedQuadStorage(quadStorage,quadStorage+size_t(quadCap)*NODE), savedQuadStates(quadStates,quadStates+quadCap);

    const std::vector<uint8> savedGlobals(G(),G()+GLOBALS_SIZE);
    auto restoreAll=[&]{
        std::memcpy(G(),savedGlobals.data(),GLOBALS_SIZE);
        std::memcpy(quadStorage,savedQuadStorage.data(),savedQuadStorage.size()); std::memcpy(quadStates,savedQuadStates.data(),savedQuadStates.size());
        quadLast=savedQuadLast; ptrLast=savedPtrLast; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWB), w2e(0x5D1270,FakeSaveWB);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xE4E1u; if(!stt)stt=1; trace=json::array(); nrec=0; bump=0; newCalls=0; failMask=0;
                    restoreAll();
                    uint8* g=G();
                    json out; json ret=nullptr;
                    const uintptr_t a=fn.address;

                    if(nm=="Init"){
                        // globals previos con basura (Init no los lee) y arena con un patron por caso
                        for(size_t k=0;k<GLOBALS_SIZE;k+=4){ const uint32 v=next(); std::memcpy(g+k,&v,4); }
                        for(size_t k=0;k<arena.size();k+=4){ const uint32 v=next(); std::memcpy(arena.data()+k,&v,4); }
                        if(next()%16==0) failMask=2; // el new[] del almacenamiento devuelve nulo (el EXE sigue)
                        // cursor del pool de nodos del arbol; a veces el pool lleno (Init deja el arbol nulo)
                        switch(next()%4){
                        case 0:  break;
                        case 1:  quadLast=-1; break;
                        default: quadLast=int32(next()%uint32(quadCap)); break;
                        }
                        if(next()%16==0) for(int32 k=0;k<quadCap;++k) quadStates[k]&=0x7F;
                        const std::vector<uint8> qs0(quadStates,quadStates+quadCap);

                        for(auto* h:hooks)h->State(candidate);
                        out["switches"]=ReversingProbe::JumpsToDll(a)==candidate;
                        { EntryPatch n1(0x82119A,FakeNew), n2(0x821195,FakeNewArr); reinterpret_cast<decltype(&CEntryExitManager::Init)>(a)(); }
                        out["allocs"]=Recs();
                        out["header"]=ReversingProbe::Bytes(arena.data(),0x20);
                        out["arena"]=Fnv(arena.data(),arena.size());
                        { json chunks=json::array(); for(size_t k=0;k<nrec;++k) if(recs[k].p) chunks.push_back(Fnv(reinterpret_cast<void*>(recs[k].p),recs[k].a+16)); out["chunks"]=chunks; }
                        auto* tree=At<uint8*>(g,0xB0);
                        const auto q=uintptr_t(quadStorage);
                        if(tree&&uintptr_t(tree)>=q&&uintptr_t(tree)<q+size_t(quadCap)*NODE){ out["node"]=ReversingProbe::Bytes(tree,NODE); out["node_state"]=quadStates[(uintptr_t(tree)-q)/NODE]; }
                        json qd=json::array(); for(int32 k=0;k<quadCap;++k) if(quadStates[k]!=qs0[k]) qd.push_back({k,qs0[k],quadStates[k]}); out["quad_states"]=qd;
                        out["quad_last"]=quadLast;
                        out["globals"]=Globals();
                        // desmontaje: el pool de nodos y los globales se restauran en restoreAll
                    }
                    else {
                        BuildPool(next,uni,nm=="Save" ? 0 : 1,nm=="Save" ? 60 : 30,480.f);
                        if(!BuildTree(next)) return {{"error","no tree node"}};
                        At<void*>(g,0xB0)=root; At<void*>(g,0xB8)=poolHdr.data();
                        if(nm=="Save"){
                            const int32 posn=int32(next()%8)-1; At<int32>(g,0xA4)=posn; // hasta 6: el EXE lee mas alla de la pila (sin desreferenciar)
                            for(int k=0;k<4;++k) At<void*>(g,k*4)=next()%4==0 ? OddPtr(next) : Enex(next()%3 ? PickUsed(next) : int32(next()%POOL_SIZE));
                            for(int32 k:usedSlots) if(next()%6==0) At<void*>(Slot(k),0x38)=OddPtr(next);
                            if(next()%4==0) At<int32>(poolHdr.data(),8)=1+int32(next()%POOL_SIZE); // capacidad menor: el bucle sigue siendo de 400
                            for(auto* h:hooks)h->State(candidate);
                            out["switches"]=ReversingProbe::JumpsToDll(a)==candidate;
                            ret=reinterpret_cast<decltype(&CEntryExitManager::Save)>(a)();
                        }
                        else { // Shutdown
                            At<int32>(g,0xA4)=int32(next()%5);
                            if(next()%8==0) At<int32>(poolHdr.data(),8)=0; // capacidad 0: no se liberan los arrays
                            for(auto* h:hooks)h->State(candidate);
                            out["switches"]=ReversingProbe::JumpsToDll(a)==candidate;
                            { EntryPatch d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDeleteArr); reinterpret_cast<decltype(&CEntryExitManager::Shutdown)>(a)(); }
                            out["deletes"]=Recs();
                            out["header"]=ReversingProbe::Bytes(poolHdr.data(),0x14); // el pool no se libera de verdad: se ve lo que queda escrito
                            out["storage"]=Fnv(storage.data(),storage.size()); out["states"]=Fnv(states.data(),states.size());
                            out["quad_used"]=quadPool->GetNoOfUsedSpaces();
                        }
                        out["globals"]=Globals();
                        for(auto* h:hooks)h->State(false);
                        if(At<void*>(g,0xB0)==root) delete root; // el arbol vuelve al pool real de nodos (salvo que Shutdown ya lo liberara)
                        root=nullptr;
                    }
                    out["ret"]=ret; out["trace"]=trace;
                    for(auto* h:hooks)h->State(false);
                    restoreAll();
                    return out;
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
    restoreAll();
    const bool restored=std::memcmp(G(),savedGlobals.data(),GLOBALS_SIZE)==0&&quadLast==savedQuadLast&&ptrLast==savedPtrLast
        &&std::memcmp(quadStates,savedQuadStates.data(),savedQuadStates.size())==0&&quadPool->GetNoOfFreeSpaces()==quadFree;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
