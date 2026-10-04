#pragma once
// Banco dentro del motor: gestor de entradas y salidas (lista de 18; 17 funciones de CEntryExitManager).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures deterministas sobre un
// pool falso de 400 entradas (tamano del EXE; Save recorre 400 slots fijos) y un arbol cuaternario cuya raiz y nodos
// salen del pool real de nodos (operator new de la clase) y cuyas listas usan nodos reales de CPtrNodeSingleLink.
// Los punteros globales (pool, arbol, pila, entrada activa, lista de entidades visibles) se sustituyen en cada
// ejecucion y se restauran, igual que m_LastFreeSlot de los pools reales de nodos. Se simulan y trazan: rand,
// FindPlayerPed, Load/SaveDataToWorkBuffer (nunca se tocan partidas), CCamera::IsSphereVisible,
// CClock::GetIsTimeInRange, C3dMarkers::PlaceMarkerCone, CGameLogic::IsCoopGameGoingOn,
// CEntryExit::TransitionStarted/TransitionFinished, CRenderer::GetObjectsInFrustum, CWorld::FindObjectsInRange y
// operator delete/delete[] (solo durante Shutdown, sin liberar). Init queda fuera: reserva 512 entradas (NOTSA).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "EntryExit.h"
#include "EntryExitManager.h"
#include "Core/QuadTreeNode.h"
#include "GenericGameStorage.h"
#include "World.h"
#include "Renderer.h"
#include "Camera.h"
#include "Clock.h"
#include "GameLogic.h"
#include "3dMarkers.h"
#include "CutsceneMgr.h"
#include "Replay.h"
#include "Pad.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"

namespace World18Live {
using TreeNode = CQuadTreeNode<CEntryExit*>;
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr int32 POOL_SIZE=400; constexpr size_t ENEX=0x3C;
constexpr uintptr_t GLOBALS=0x96A720, GLOBALS_SIZE=0xC0; // pila .. ms_numVisibleEntities (incluye los estaticos de CEntryExit)
constexpr uintptr_t CAMERA=0xB6F028, CAM_MIRROR=0xB6F998, CAM_FWD=0xB6F9AC, CUTSCENE=0xB5F852, REPLAY_MODE=0xA43088, AREA=0xB72914;

// --- fixtures ---
alignas(16) inline std::array<uint8,POOL_SIZE*ENEX> storage{};
inline std::array<uint8,POOL_SIZE> states{};
alignas(16) inline std::array<uint8,0x20> poolHdr{};
alignas(16) inline std::array<std::array<uint8,0x200>,40> ents{};
alignas(16) inline std::array<std::array<float,18>,48> mats{}; // matrices falsas: posicion en [12..14]
alignas(16) inline std::array<uint8,0x7A0> ped{}, veh{};
inline TreeNode* root{};
inline std::vector<int32> usedSlots;
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
    for(size_t k=0;k<ents.size();++k) if(u==uintptr_t(ents[k].data())) return "ent"+std::to_string(k);
    if(u==uintptr_t(ped.data())) return "ped";
    if(u==uintptr_t(veh.data())) return "veh";
    if(root&&u==uintptr_t(root)) return "root";
    if(u>=GLOBALS&&u<GLOBALS+GLOBALS_SIZE) return "g"+std::to_string(u-GLOBALS);
    if(u==uintptr_t(&PC_Scratch[256])) return "scratch";
    return "other";
}

// --- rand ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- trabajo de guardado (sin ficheros) ---
inline std::vector<uint8> loadStream; inline size_t loadPos{};
inline bool __cdecl FakeLoadWB(void* p,int32 n) {
    for(int32 k=0;k<n;++k){ static_cast<uint8*>(p)[k]=loadPos<loadStream.size() ? loadStream[loadPos] : 0xFF; ++loadPos; }
    trace.push_back({"load_wb",Name(p),n}); return true; }
inline bool __cdecl FakeSaveWB(void* p,int32 n) { trace.push_back({"save_wb",Name(p),ReversingProbe::Bytes(p,size_t(n))}); return true; }

// --- jugador, camara, reloj, marcadores, transiciones ---
inline void* pedResult{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { trace.push_back({"find_player_ped",id}); return static_cast<CPlayerPed*>(pedResult); }
inline uint32 sphereMask{}, sphereCalls{}, timeMask{}, timeCalls{}, startMask{}, startCalls{}, finishMask{}, finishCalls{}; inline bool coopResult{};
inline bool Bit(uint32 m,uint32& c) { return (m>>(c++%32))&1; }
inline bool __fastcall FakeSphereVisible(void* cam,void*,const CVector& c,float r,RwMatrix* m) {
    const bool v=Bit(sphereMask,sphereCalls); trace.push_back({"sphere_visible",intptr_t(cam)-intptr_t(CAMERA),F(c.x),F(c.y),F(c.z),F(r),intptr_t(m)-intptr_t(CAMERA),v}); return v; }
inline bool __cdecl FakeTimeInRange(uint8 a,uint8 b) { const bool v=Bit(timeMask,timeCalls); trace.push_back({"time_in_range",a,b,v}); return v; }
inline void __cdecl FakeMarkerCone(uint32 id,CVector& p,float size,uint8 r,uint8 g,uint8 b,uint8 a,uint16 period,float frac,int16 rot,bool col) {
    trace.push_back({"marker_cone",Name(reinterpret_cast<void*>(id)),F(p.x),F(p.y),F(p.z),F(size),r,g,b,a,period,F(frac),rot,col}); }
inline bool __cdecl FakeCoop() { trace.push_back({"coop"}); return coopResult; }
inline bool __fastcall FakeTransitionStarted(CEntryExit* self,void*,CPed* p) { const bool v=Bit(startMask,startCalls); trace.push_back({"transition_started",Name(self),Name(p),v}); return v; }
inline bool __fastcall FakeTransitionFinished(CEntryExit* self,void*,CPed* p) { const bool v=Bit(finishMask,finishCalls); trace.push_back({"transition_finished",Name(self),Name(p),v}); return v; }

// --- mundo ---
inline int32 frustumCount{}; inline std::array<CEntity*,64> frustumList{};
inline int32 __cdecl FakeFrustum(CEntity** out,float farPlane,RwMatrix* m) {
    trace.push_back({"objects_in_frustum",Name(out),F(farPlane),uintptr_t(m)}); for(int32 k=0;k<frustumCount;++k)out[k]=frustumList[k]; return frustumCount; }
inline int16 rangeCount{}; inline std::array<CEntity*,100> rangeList{};
inline void __cdecl FakeObjectsInRange(const CVector& p,float r,bool b2D,int16* outCount,int16 maxCount,CEntity** out,bool b,bool v,bool pd,bool o,bool d) {
    trace.push_back({"objects_in_range",F(p.x),F(p.y),F(p.z),F(r),b2D,maxCount,b,v,pd,o,d}); for(int16 k=0;k<rangeCount;++k)out[k]=rangeList[k]; *outCount=rangeCount; }

// --- operator delete/delete[] durante Shutdown: sin liberar ni reservar ---
struct Rec { uint32 tag; uintptr_t p; };
inline std::array<Rec,64> recs{}; inline size_t nrec{};
inline void __cdecl FakeDelete(void* p) { if(nrec<recs.size())recs[nrec++]={1,uintptr_t(p)}; }
inline void __cdecl FakeDeleteArr(void* p) { if(nrec<recs.size())recs[nrec++]={2,uintptr_t(p)}; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,17> fns{{
    {"CEntryExitManager","AddEntryExitToStack",0x43E410,0,-1,512},{"CEntryExitManager","AddOne",0x43FA00,0,-1,768},
    {"CEntryExitManager","DeleteOne",0x43FD50,0,-1,512},{"CEntryExitManager","EnableBurglaryHouses",0x43F180,0,-1,256},
    {"CEntryExitManager","FindNearestDoor",0x43F630,0,-1,512},{"CEntryExitManager","FindNearestEntryExit",0x43F4B0,0,-1,512},
    {"CEntryExitManager","GetEntryExitIndex",0x43EFD0,0,-1,768},{"CEntryExitManager","GetPositionRelativeToOutsideWorld",0x43F150,0,-1,512},
    {"CEntryExitManager","LinkEntryExit",0x43F050,0,-1,512},{"CEntryExitManager","Load",0x5D55C0,0,-1,512},
    {"CEntryExitManager","PostEntryExitsCreation",0x43F0A0,0,-1,256},{"CEntryExitManager","ResetAreaCodeForVisibleObjects",0x43ED80,0,-1,256},
    {"CEntryExitManager","Save",0x5D5970,0,-1,512},{"CEntryExitManager","SetAreaCodeForVisibleObjects",0x43ECF0,0,-1,256},
    {"CEntryExitManager","Shutdown",0x440B90,0,-1,128},{"CEntryExitManager","ShutdownForRestart",0x440C40,0,-1,256},
    {"CEntryExitManager","Update",0x440D10,0,-1,768},
}};

// Pool falso: todos los slots con campos finitos (los enlaces de slots libres tambien se pueden seguir), minUsed..maxUsed ocupados
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
inline json DumpTree(TreeNode* n) {
    if(!n) return nullptr;
    json items=json::array(); for(auto* it:n->m_ItemList) items.push_back(Name(it));
    json ch=json::array(); for(auto* c:n->m_apChildren) ch.push_back(DumpTree(c));
    return {{"rect",{F(n->m_Rect.left),F(n->m_Rect.top),F(n->m_Rect.right),F(n->m_Rect.bottom)}},{"level",n->m_nLevel},{"items",items},{"children",ch}};
}
inline json PoolDiff(const std::vector<uint8>& s0,const std::vector<uint8>& st0) {
    json d=json::array();
    for(int32 i=0;i<POOL_SIZE;++i) if(states[i]!=st0[i]||std::memcmp(Slot(i),&s0[size_t(i)*ENEX],ENEX)!=0) d.push_back({i,states[i],ReversingProbe::Bytes(Slot(i),ENEX)});
    return d;
}
inline json Globals() {
    uint8* g=G();
    json stack=json::array(); for(int k=0;k<4;++k) stack.push_back(Name(At<void*>(g,k*4)));
    json vis=json::array(); for(int k=0;k<32;++k) vis.push_back(Name(At<void*>(g,0x18+k*4)));
    return {{"stack",stack},{"burglary",ReversingProbe::Bytes(g+0x10,4)},{"old_area",At<int32>(g,0x14)},{"visible",vis},{"enex_statics",Fnv(g+0x98,0xC)},
            {"posn",At<int32>(g,0xA4)},{"disabled",ReversingProbe::Bytes(g+0xA8,4)},{"state",At<int32>(g,0xAC)},{"tree",Name(At<void*>(g,0xB0))},
            {"active",Name(At<void*>(g,0xB4))},{"pool",Name(At<void*>(g,0xB8))},{"num_visible",At<int32>(g,0xBC)},{"raw",Fnv(g,GLOBALS_SIZE)}};
}
inline json Ents() { json a=json::array(); for(auto& e:ents)a.push_back(e[0x2F]); return {{"areas",a},{"hash",Fnv(ents.data(),sizeof(ents))}}; }
// Entidad falsa (objeto): bytes aleatorios, tipo objeto, posicion por matriz o colocacion
template<class N> inline void SetEntityPos(N& next,size_t k,const CVector& p) {
    auto* en=ents[k].data();
    if(next()%2){ auto& m=mats[k]; m[12]=p.x; m[13]=p.y; m[14]=p.z; At<void*>(en,0x14)=m.data(); }
    else { At<void*>(en,0x14)=nullptr; At<float>(en,4)=p.x; At<float>(en,8)=p.y; At<float>(en,0xC)=p.z; }
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"world18-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"world18-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("world18");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"world18-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,17> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,17> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // pools reales de nodos (arbol y listas): solo se restaura m_LastFreeSlot
    auto* quadPool=TreeNode::GetPool(); auto* ptrPool=GetPtrNodeSingleLinkPool();
    auto& quadLast=At<int32>(quadPool,0xC); auto& ptrLast=At<int32>(ptrPool,0xC); const int32 savedQuadLast=quadLast, savedPtrLast=ptrLast;
    const size_t quadFree=quadPool->GetNoOfFreeSpaces();
    if(quadFree<64){Write({{"event","error"},{"reason","quadtree node pool almost full"},{"free",quadFree}});return;}

    // estado global guardado
    const std::vector<uint8> savedGlobals(G(),G()+GLOBALS_SIZE);
    auto* cam=reinterpret_cast<uint8*>(CAMERA); const std::vector<uint8> savedCam(cam,cam+0x18), savedCamFwd(reinterpret_cast<uint8*>(CAM_FWD),reinterpret_cast<uint8*>(CAM_FWD)+12);
    auto& mirror=*reinterpret_cast<uint8*>(CAM_MIRROR); const uint8 savedMirror=mirror;
    auto& cutscene=*reinterpret_cast<uint8*>(CUTSCENE); const uint8 savedCutscene=cutscene;
    auto& replay=*reinterpret_cast<uint8*>(REPLAY_MODE); const uint8 savedReplay=replay;
    auto& area=*reinterpret_cast<int32*>(AREA); const int32 savedArea=area;
    auto& padDisable=CPad::GetPad(0)->DisablePlayerControls; const uint16 savedPad=padDisable;
    auto* scratch=reinterpret_cast<uint8*>(&PC_Scratch[256]); const std::vector<uint8> savedScratch(scratch,scratch+64*4);
    auto restoreAll=[&]{
        std::memcpy(G(),savedGlobals.data(),GLOBALS_SIZE); std::memcpy(cam,savedCam.data(),savedCam.size()); std::memcpy(reinterpret_cast<uint8*>(CAM_FWD),savedCamFwd.data(),12);
        mirror=savedMirror; cutscene=savedCutscene; replay=savedReplay; area=savedArea; padDisable=savedPad; std::memcpy(scratch,savedScratch.data(),savedScratch.size());
        quadLast=savedQuadLast; ptrLast=savedPtrLast; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand), r1d(bit_cast<uintptr_t>(&rand),FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch w1(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWB), w1e(0x5D1300,FakeLoadWB);
        EntryPatch w2(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWB), w2e(0x5D1270,FakeSaveWB);
        EntryPatch c1(bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible)),FakeSphereVisible), c1e(0x420C40,FakeSphereVisible);
        EntryPatch c2(bit_cast<uintptr_t>(&CClock::GetIsTimeInRange),FakeTimeInRange), c2e(0x52CEE0,FakeTimeInRange);
        EntryPatch c3(bit_cast<uintptr_t>(&C3dMarkers::PlaceMarkerCone),FakeMarkerCone), c3e(0x726D40,FakeMarkerCone);
        EntryPatch c4(bit_cast<uintptr_t>(&CGameLogic::IsCoopGameGoingOn),FakeCoop), c4e(0x441390,FakeCoop);
        EntryPatch t1(bit_cast<uintptr_t>(&CEntryExit::TransitionStarted),FakeTransitionStarted), t1e(0x43FFD0,FakeTransitionStarted);
        EntryPatch t2(bit_cast<uintptr_t>(&CEntryExit::TransitionFinished),FakeTransitionFinished), t2e(0x4404A0,FakeTransitionFinished);
        EntryPatch o1(bit_cast<uintptr_t>(&CRenderer::GetObjectsInFrustum),FakeFrustum), o1e(0x554C60,FakeFrustum);
        EntryPatch o2(bit_cast<uintptr_t>(&CWorld::FindObjectsInRange),FakeObjectsInRange), o2e(0x564A20,FakeObjectsInRange);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x18E1u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; nrec=0;
                    sphereCalls=timeCalls=startCalls=finishCalls=0; sphereMask=next(); timeMask=next(); startMask=next(); finishMask=next(); coopResult=false;
                    pedResult=player; frustumCount=0; rangeCount=0; loadStream.clear(); loadPos=0;
                    restoreAll();
                    const bool cluster=nm=="Update"||nm=="FindNearestEntryExit";
                    BuildPool(next,uni,nm=="Update"||nm=="AddOne" ? 0 : 1,nm=="AddOne" ? 60 : 30,cluster ? 60.f : 480.f);
                    if(!BuildTree(next)) return {{"error","no tree node"}};
                    for(auto& e:ents)for(auto& b:e)b=uint8(next());
                    ped.fill(0); veh.fill(0);
                    uint8* g=G(); At<void*>(g,0xB0)=root; At<void*>(g,0xB8)=poolHdr.data();
                    const std::vector<uint8> s0(storage.begin(),storage.end()), st0(states.begin(),states.end());

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    bool poolOut=true, treeOut=true;

                    if(nm=="AddEntryExitToStack"){
                        const int32 posn=int32(next()%4); At<int32>(g,0xA4)=posn;
                        for(int k=0;k<4;++k) At<void*>(g,k*4)=Enex(next()%2 ? PickUsed(next) : int32(next()%POOL_SIZE));
                        auto* e=Enex(PickUsed(next));
                        if(posn>0&&next()%3==0) At<void*>(g,(posn-1)*4)=e->m_pLink ? e->m_pLink : e;
                        reinterpret_cast<decltype(&CEntryExitManager::AddEntryExitToStack)>(a)(e);
                    }
                    else if(nm=="AddOne"){
                        g[0x10]=uint8(next()%2);
                        static const char* const names[]{"BARBERS","barbers","GYM1","PDOMES","CSCHP","LONGNAME12","BAR","8BALL","AMMUN1",""};
                        const float cx=uni(-480.f,480.f), cy=uni(-480.f,480.f), cz=uni(-10.f,40.f);
                        float ang; switch(next()%4){case 0:ang=0.f;break;case 1:ang=float(int(next()%8)-4)*90.f;break;default:ang=uni(-360.f,360.f);}
                        float rx=uni(0.f,20.f), ry=uni(0.f,20.f); if(next()%10==0)rx=-rx; if(next()%10==0)ry=-ry; if(next()%20==0)rx=0.f;
                        uint32 flags=next()&0x1FFFF; if(next()%3==0)flags|=0x4; if(next()%3==0)flags|=0x400; if(next()%3==0)flags|=0x1000;
                        const int32 areaArg=next()%6 ? int32(next()%19) : int32(next()%300);
                        const char* name=next()%7==0 ? nullptr : names[next()%std::size(names)];
                        ret=reinterpret_cast<decltype(&CEntryExitManager::AddOne)>(a)(cx,cy,cz,ang,rx,ry,uni(-5.f,5.f),uni(-480.f,480.f),uni(-480.f,480.f),uni(-10.f,40.f),uni(-360.f,360.f),
                            eAreaCodes(areaArg),CEntryExit::eFlags(flags),int32(next()%300),int32(next()%30),int32(next()%30),int32(next()%300),name);
                        out["header"]=ReversingProbe::Bytes(poolHdr.data(),0x14);
                    }
                    else if(nm=="DeleteOne"){
                        const int32 idx=next()%3 ? PickUsed(next) : int32(next()%POOL_SIZE);
                        reinterpret_cast<decltype(&CEntryExitManager::DeleteOne)>(a)(idx);
                        out["header"]=ReversingProbe::Bytes(poolHdr.data(),0x14);
                    }
                    else if(nm=="EnableBurglaryHouses"){
                        g[0x10]=uint8(next()%2); reinterpret_cast<decltype(&CEntryExitManager::EnableBurglaryHouses)>(a)(next()%2!=0);
                    }
                    else if(nm=="FindNearestDoor"){
                        auto* e=Enex(next()%4 ? PickUsed(next) : int32(next()%POOL_SIZE));
                        float radius=uni(0.f,30.f); if(next()%20==0)radius=0.f; else if(next()%30==0)radius=-radius;
                        const CVector c=e->GetPosition(); rangeCount=int16(next()%13);
                        const float spreadR=std::abs(radius)*1.2f+0.5f; CVector prev=c;
                        for(int16 k=0;k<rangeCount;++k){
                            const CVector p=next()%7==0 ? prev : c+CVector{uni(-spreadR,spreadR),uni(-spreadR,spreadR),uni(-spreadR,spreadR)}; prev=p;
                            SetEntityPos(next,size_t(k),p); auto* en=ents[k].data(); At<uint32>(en,0x40)=next(); en[0x36]=uint8((en[0x36]&~7)|4);
                            rangeList[k]=reinterpret_cast<CEntity*>(en); }
                        ret=Name(reinterpret_cast<decltype(&CEntryExitManager::FindNearestDoor)>(a)(*e,radius));
                        poolOut=false;
                    }
                    else if(nm=="FindNearestEntryExit"){
                        CVector2D p{uni(-70.f,70.f),uni(-70.f,70.f)}; if(next()%10==0){ const auto c=Enex(PickUsed(next))->GetPosition(); p=CVector2D{c.x,c.y}; }
                        float range=uni(0.f,60.f); if(next()%20==0)range=0.f; else if(next()%20==0)range=-range;
                        static const int32 ignores[]{0,1,2,3,13,200,-1}; const int32 ign=ignores[next()%std::size(ignores)];
                        ret=reinterpret_cast<decltype(&CEntryExitManager::FindNearestEntryExit)>(a)(p,range,ign);
                        poolOut=false;
                    }
                    else if(nm=="GetEntryExitIndex"){
                        static const char* const names[]{"BARBERS","BarBers","GYM1","gym1","PDOMES","CSCHP","LONGNAM8","longnam8x","BAR","8BALL","AMMUN1","NONE",""};
                        const char* name=names[next()%std::size(names)];
                        const uint16 en=next()%3==0 ? uint16(0) : uint16(next()&next());
                        const uint16 dis=next()%3==0 ? uint16(4) : uint16(next()&next()&next());
                        ret=reinterpret_cast<decltype(&CEntryExitManager::GetEntryExitIndex)>(a)(name,en,dis);
                        poolOut=false; treeOut=false;
                    }
                    else if(nm=="GetPositionRelativeToOutsideWorld"){
                        const int32 posn=int32(next()%5); At<int32>(g,0xA4)=posn;
                        for(int k=0;k<4;++k) At<void*>(g,k*4)=Enex(PickUsed(next));
                        CVector pos{uni(-500.f,500.f),uni(-500.f,500.f),uni(-50.f,50.f)};
                        reinterpret_cast<decltype(&CEntryExitManager::GetPositionRelativeToOutsideWorld)>(a)(pos);
                        ret=json::array({F(pos.x),F(pos.y),F(pos.z)}); poolOut=false; treeOut=false;
                    }
                    else if(nm=="LinkEntryExit"){
                        reinterpret_cast<decltype(&CEntryExitManager::LinkEntryExit)>(a)(Enex(PickUsed(next)));
                    }
                    else if(nm=="Load"){
                        auto put=[&](const void* p,size_t n){ auto* b=static_cast<const uint8*>(p); loadStream.insert(loadStream.end(),b,b+n); };
                        auto idx=[&]{ return int16(next()%4 ? PickUsed(next) : int32(next()%POOL_SIZE)); };
                        const int32 posn=next()%8==0 ? -1 : int32(next()%5); put(&posn,4);
                        for(int32 k=0;k<posn;++k){ const int16 v=idx(); put(&v,2); }
                        const int32 nrecs=int32(next()%13);
                        for(int32 k=0;k<nrecs;++k){ const int16 v=idx(); const uint16 fl=uint16(next()); const int16 li=next()%3==0 ? int16(-1) : idx(); put(&v,2); put(&fl,2); put(&li,2); }
                        const int16 end=-1; put(&end,2);
                        ret=reinterpret_cast<decltype(&CEntryExitManager::Load)>(a)(); out["consumed"]=loadPos;
                    }
                    else if(nm=="PostEntryExitsCreation"){
                        reinterpret_cast<decltype(&CEntryExitManager::PostEntryExitsCreation)>(a)();
                    }
                    else if(nm=="ResetAreaCodeForVisibleObjects"){
                        const int32 num=int32(next()%33); At<int32>(g,0xBC)=num; At<int32>(g,0x14)=next()%4 ? int32(next()%20) : int32(next());
                        for(int k=0;k<32;++k) At<void*>(g,0x18+k*4)=ents[next()%ents.size()].data();
                        reinterpret_cast<decltype(&CEntryExitManager::ResetAreaCodeForVisibleObjects)>(a)();
                        out["ents"]=Ents(); poolOut=false; treeOut=false;
                    }
                    else if(nm=="Save"){
                        const int32 posn=int32(next()%6)-1; At<int32>(g,0xA4)=posn;
                        for(int k=0;k<4;++k) At<void*>(g,k*4)=Enex(next()%3 ? PickUsed(next) : int32(next()%POOL_SIZE));
                        for(int32 k:usedSlots) if(next()%8==0) At<uintptr_t>(Slot(k),0x38)=0x1000+next()%0x10000; // punteros ajenos al pool
                        ret=reinterpret_cast<decltype(&CEntryExitManager::Save)>(a)();
                    }
                    else if(nm=="SetAreaCodeForVisibleObjects"){
                        area=next()%5 ? int32(next()%4) : 200;
                        for(auto& e:ents){ static const uint8 others[]{0,1,2,3,13,200}; e[0x2F]=next()%3 ? uint8(area) : others[next()%std::size(others)]; }
                        frustumCount=int32(next()%41);
                        for(int32 k=0;k<frustumCount;++k) frustumList[k]=next()%20==0 ? reinterpret_cast<CEntity*>(ped.data()) : reinterpret_cast<CEntity*>(ents[next()%ents.size()].data());
                        pedResult=next()%3 ? static_cast<void*>(ents[next()%ents.size()].data()) : static_cast<void*>(ped.data());
                        At<int32>(g,0xBC)=int32(next()%40); At<int32>(g,0x14)=int32(next()%20);
                        reinterpret_cast<decltype(&CEntryExitManager::SetAreaCodeForVisibleObjects)>(a)();
                        out["ents"]=Ents(); out["ped_area"]=ped[0x2F]; out["scratch"]=Fnv(scratch,savedScratch.size()); poolOut=false; treeOut=false;
                    }
                    else if(nm=="Shutdown"){
                        At<int32>(g,0xA4)=int32(next()%5);
                        { EntryPatch d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDeleteArr); reinterpret_cast<decltype(&CEntryExitManager::Shutdown)>(a)(); }
                        json r=json::array(); for(size_t k=0;k<nrec;++k)r.push_back({recs[k].tag,Name(reinterpret_cast<void*>(recs[k].p))}); out["deletes"]=r;
                        out["quad_used"]=quadPool->GetNoOfUsedSpaces();
                        poolOut=false; treeOut=false; // pool y arbol liberados (el DLL rellena la memoria liberada en ~CPool)
                    }
                    else if(nm=="ShutdownForRestart"){
                        g[0x10]=uint8(next()%2); g[0xA8]=uint8(next()%2); At<int32>(g,0xAC)=int32(next()%5); At<int32>(g,0xA4)=int32(next()%5);
                        At<void*>(g,0xB4)=next()%2 ? static_cast<void*>(Enex(PickUsed(next))) : nullptr;
                        At<int32>(g,0xBC)=int32(next()%9); At<int32>(g,0x14)=int32(next()%20);
                        for(int k=0;k<32;++k) At<void*>(g,0x18+k*4)=ents[next()%ents.size()].data();
                        for(int32 k:usedSlots) if(next()%2) At<uint16>(Slot(k),0x30)|=0x8000;
                        reinterpret_cast<decltype(&CEntryExitManager::ShutdownForRestart)>(a)();
                        out["ents"]=Ents(); out["header"]=ReversingProbe::Bytes(poolHdr.data(),0x14);
                    }
                    else if(nm=="Update"){
                        static const int32 statesE[]{0,0,0,3,1,4}; At<int32>(g,0xAC)=statesE[next()%std::size(statesE)];
                        cutscene=uint8(next()%10==0); padDisable=uint16(next()%10==0); coopResult=next()%10==0; replay=uint8(next()%10==0 ? 1+next()%2 : 0); g[0xA8]=uint8(next()%10==0);
                        At<void*>(g,0xB4)=next()%4==0 ? static_cast<void*>(Enex(PickUsed(next))) : nullptr;
                        area=int32(next()%4);
                        const CVector camPos{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,30.f)};
                        if(next()%2){ auto& m=mats[40]; m[12]=camPos.x; m[13]=camPos.y; m[14]=camPos.z; At<void*>(cam,0x14)=m.data(); }
                        else { At<void*>(cam,0x14)=nullptr; At<float>(cam,4)=camPos.x; At<float>(cam,8)=camPos.y; At<float>(cam,0xC)=camPos.z; }
                        At<float>(reinterpret_cast<void*>(CAM_FWD),0)=uni(-1.f,1.f); At<float>(reinterpret_cast<void*>(CAM_FWD),4)=uni(-1.f,1.f); At<float>(reinterpret_cast<void*>(CAM_FWD),8)=uni(-0.5f,0.5f);
                        mirror=uint8(next()%2);
                        for(int32 k:usedSlots) if(next()%2) At<uint16>(Slot(k),0x30)|=0x4000;
                        ped.fill(0); veh.fill(0); pedResult=ped.data();
                        auto placePos=[&]{ if(!usedSlots.empty()&&next()%5<3){ const auto c=Enex(PickUsed(next))->GetPosition(); return CVector{c.x+uni(-1.f,1.f),c.y+uni(-1.f,1.f),c.z+uni(-0.9f,0.9f)}; }
                                           return CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,30.f)}; };
                        auto setPos=[&](uint8* ent,size_t mi,const CVector& p){ if(next()%2){ auto& m=mats[mi]; m[12]=p.x; m[13]=p.y; m[14]=p.z; At<void*>(ent,0x14)=m.data(); }
                                                                              else { At<void*>(ent,0x14)=nullptr; At<float>(ent,4)=p.x; At<float>(ent,8)=p.y; At<float>(ent,0xC)=p.z; } };
                        setPos(ped.data(),41,placePos()); setPos(veh.data(),42,placePos());
                        if(next()%10<3) At<uint32>(ped.data(),0x46C)|=0x100;
                        At<void*>(ped.data(),0x58C)=veh.data();
                        reinterpret_cast<decltype(&CEntryExitManager::Update)>(a)();
                        out["ptr_last"]=ptrLast;
                    }

                    out["globals"]=Globals();
                    if(poolOut){ out["pool"]=PoolDiff(s0,st0); }
                    if(treeOut&&At<void*>(g,0xB0)==root){ out["tree"]=DumpTree(root); out["quad_used"]=quadPool->GetNoOfUsedSpaces(); out["quad_last"]=quadLast; }
                    out["ret"]=ret; out["trace"]=trace;
                    // desmontaje: el arbol vuelve al pool real de nodos (salvo que Shutdown ya lo liberara)
                    for(auto* h:hooks)h->State(false);
                    if(At<void*>(g,0xB0)==root) delete root;
                    root=nullptr;
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
    const bool restored=std::memcmp(G(),savedGlobals.data(),GLOBALS_SIZE)==0&&std::memcmp(cam,savedCam.data(),savedCam.size())==0&&quadLast==savedQuadLast&&ptrLast==savedPtrLast
        &&quadPool->GetNoOfFreeSpaces()==quadFree&&area==savedArea&&replay==savedReplay&&cutscene==savedCutscene&&padDisable==savedPad;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
