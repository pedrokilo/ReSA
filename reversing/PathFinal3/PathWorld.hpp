#pragma once
// Infraestructura del banco PathFinal3 (CPathFind): utilidades, memoria falsa, constructor de mundos falsos y estado global.
// Todo el contenido real de ThePaths (0x96F050, 0x3C80 bytes) y de las globales vecinas se guarda antes de empezar y se restaura tras
// cada ejecucion; los nodos/enlaces de las pruebas viven en un arena propio del banco (nunca se tocan las areas reales).
#include <fstream>
#include <set>
#include <cmath>
#include <cstring>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "PathFind.h"
#include "Timer.h"
#include "Streaming.h"
#include "GenericGameStorage.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace PathFinal3Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return n ? next()%n : 0; }
    int32 range(int32 a,int32 b){ return a+int32(below(uint32(b-a+1))); }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };

// Hilo del banco: otros hilos del juego pueden llamar a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

// --- buferes con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    return "other";
}
template<class V> inline json Jv(V v){
    if constexpr(std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos (como WorldFinal1) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// cuerpo de la DLL de un puntero a funcion/miembro (sigue los E9 dentro de la DLL; si salta al EXE se queda en el salto)
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(Addr(f)); }

// ======================= desplazamientos del EXE dentro de CPathFind (ThePaths = 0x96F050) =======================
constexpr uintptr_t kPaths=0x96F050; constexpr size_t kPathsSize=0x3C80;
constexpr size_t oHash=4, oPathNodes=0x804, oNaviNodes=0x924, oNodeLinks=0xA44, oLinkLengths=0xB64, oIntersections=0xC84, oNaviLinks=0xDA4, oTempNodes=0xEA4,
    oNumNodes=0xFA4, oNumVeh=0x10C4, oNumPed=0x11E4, oNumNavi=0x1304, oNumAddr=0x1424, oDynBase=0x1544, oDynIds=0x2544, oHashCount=0x3544,
    oInteriorIds=0x3548, oNumSwitches=0x3568, oSwitches=0x356C, oPending=0x3C6C, oReqMinX=0x3C70, oReqMaxX=0x3C74, oReqMinY=0x3C78, oReqMaxY=0x3C7C;
inline uint32 Off2(const void* m,const void* base){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); }
inline uint8* PathsP(){ return reinterpret_cast<uint8*>(kPaths); }
inline CPathFind* Paths(){ return reinterpret_cast<CPathFind*>(kPaths); }
inline uint32& P32(size_t off){ return *reinterpret_cast<uint32*>(PathsP()+off); }
inline int32& PI32(size_t off){ return *reinterpret_cast<int32*>(PathsP()+off); }
inline float& PF(size_t off){ return *reinterpret_cast<float*>(PathsP()+off); }

// globales vecinas de CPathFind (rango contiguo 0x96EA98..0x96F050) y otras tocadas
constexpr uintptr_t gConnects=0x96EAC0, gDontWander=0x96EC40, gZ=0x96EC80, gY=0x96ED80, gX=0x96EE80, gNumNodesGiven=0x96EF80, gNewIntSlot=0x96EF84,
    gIntIdBuilt=0x96EF88, gForScript=0x96EF90, gToBeStreamed=0x96EFD0, gTempExt=0x96F02C, gLoadNeeded=0x96F030, gIntBuilding=0x96F031, gNumLinksExt=0x96EAB8,
    gIntLinkedExt=0x96EA98, gPathsNeededPos=0x977B70, gExtLinkedTo=0x977B7C, gString=0xB71670, gTimeMs=0xB7CB78, gTimePrevMs=0xB7CB84, gVehPool=0xB74494;
constexpr uintptr_t kGlobBegin=0x96EA98, kGlobEnd=0x96F050;   // 0x5B8 bytes

// ======================= memoria falsa del banco =======================
constexpr size_t kArena=0x10000, kMallocArena=0x4000;
alignas(16) inline std::array<uint8,kArena> arena{};            // areas falsas (nodos, enlaces...)
alignas(16) inline std::array<uint8,kMallocArena> mallocArena{}; // memoria que devuelve CMemoryMgr::Malloc (doble)
inline size_t arenaTop{}, mallocTop{};
inline uint8* ArenaAlloc(size_t n){ arenaTop=(arenaTop+15)&~size_t(15); uint8* p=arena.data()+arenaTop; arenaTop+=n; if(arenaTop>arena.size())std::abort(); return p; }
alignas(16) inline std::array<uint8,32> fakeStream{};
alignas(16) inline std::array<uint8,0x40> fakePlayer{};
inline std::vector<uint8> streamData; inline size_t streamPos{};
inline bool playerPresent{}; inline CVector playerPos{};
inline CNodeAddress findResult{};

inline void FillBytes(void* p,size_t n,Rng& r){ auto* b=static_cast<uint8*>(p); for(size_t i=0;i<n;++i)b[i]=uint8(r.next()); }
inline float RandF(Rng& r,float lim=300.f){ const uint32 k=r.below(100); if(k<6)return 0.f; if(k<10)return r.pick({1.f,-1.f,0.5f,120.f,119.99f,1e-30f,-1e-30f}); return r.uni(-lim,lim); }
// flotante finito aleatorio (nunca sNaN: el x87 los silencia al copiarlos y la DLL no)
inline float FiniteF(Rng& r,float lim=3000.f){ return r.uni(-lim,lim); }

// ======================= dobles (se llaman en la direccion del EXE y en el cuerpo de la DLL) =======================
// CMemoryMgr::Malloc (0x72F420, cdecl; los llamadores del EXE empujan 2 argumentos y la DLL 1)
inline void* __cdecl S_Malloc(size_t n){
    if(!Mine()) return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);   // el CRT que usa 0x72F420
    mallocTop=(mallocTop+15)&~size_t(15); if(mallocTop+n>mallocArena.size())std::abort();
    void* p=mallocArena.data()+mallocTop; mallocTop+=n; T("malloc",uint32(n),p); return p; }
// CMemoryMgr::Free (0x72F430 = jmp 0x82413F)
inline void __cdecl S_Free(void* p){
    if(!Mine()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    T("free",p); }
// RwStreamOpen / Read / Close (0x7ECEF0 / 0x7EC9D0 / 0x7ECE20)
inline void* __cdecl S_RwOpen(int type,int access,const void* name){
    if(!Mine()) return nullptr;
    char b[96]{}; std::strncpy(b,static_cast<const char*>(name),sizeof(b)-1); T("rw_open",type,access,std::string(b)); return fakeStream.data(); }
inline uint32 __cdecl S_RwRead(void* stream,void* buf,uint32 len){
    if(!Mine()) return 0;
    T("rw_read",stream,buf,len);
    auto* d=static_cast<uint8*>(buf); for(uint32 i=0;i<len;++i) d[i]= streamPos<streamData.size() ? streamData[streamPos++] : uint8(0xEE);
    return len; }
inline int __cdecl S_RwClose(void* s,void* data){ if(Mine())T("rw_close",s,data); return 1; }
inline void __cdecl S_TimerSuspend(){ if(Mine())T("timer_suspend"); }
inline void __cdecl S_TimerResume(){ if(Mine())T("timer_resume"); }
// CPathFind::FindNodeClosestToCoors (0x44F460): thiscall con puntero de retorno oculto y 10 argumentos, ret 2Ch
inline CNodeAddress* __fastcall S_Closest(CPathFind* self,void*,CNodeAddress* ret,float x,float y,float z,int type,float maxd,int u2,int u3,int u4,int boats,int u6){
    if(Mine())T("find_closest",self,x,y,z,type,maxd,u2,u3,u4,boats,u6);
    *ret=findResult; return ret; }
// CStreaming::RequestModel (0x4087E0) / RemoveModel (0x4089A0), cdecl
inline void __cdecl S_Request(int model,int flags){ if(Mine())T("request_model",model,flags); }
inline void __cdecl S_Remove(int model){ if(Mine())T("remove_model",model); }
// FindPlayerPed (0x56E210) / FindPlayerCoors (0x56E010), cdecl
inline void* __cdecl S_FindPed(int id){ if(!Mine()) return nullptr; T("find_player_ped",id); return playerPresent ? fakePlayer.data() : nullptr; }
inline CVector* __cdecl S_FindCoors(CVector* out,int id){
    if(!Mine()){ *out=CVector{0.f,0.f,0.f}; return out; }
    T("find_player_coors",id); *out=playerPos; return out; }
// CGenericGameStorage::SaveDataToWorkBuffer (0x5D1270) / LoadDataFromWorkBuffer (0x5D1300), cdecl
inline uint32 loadCount{}; inline uint32 loadCall{}; inline Rng loadRng{1};
inline void FillSwitchEntry(void* p,Rng& r){
    for(int i=0;i<6;++i)At<float>(p,4*i)=FiniteF(r,3000.f);
    At<uint8>(p,0x18)=uint8(r.below(2)); At<uint8>(p,0x19)=uint8(r.below(2)); At<uint8>(p,0x1A)=uint8(r.next()); At<uint8>(p,0x1B)=uint8(r.next()); }
inline bool __cdecl S_Save(void* data,int size){ if(Mine())T("save",size,Fnv(data,size_t(size))); return true; }
inline bool __cdecl S_Load(void* data,int size){
    if(!Mine()) return false;
    T("load",size);
    if(loadCall==0&&size==4){ std::memcpy(data,&loadCount,4); }
    else if(size==0x1C){ FillSwitchEntry(data,loadRng); }
    else { std::memset(data,0xAB,size_t(size)); }
    ++loadCall; return true; }

// ======================= constructor de mundos falsos =======================
struct Area {
    int id{-1}, nVeh{}, nPed{}, nNodes{}, nNavi{}, nAddr{};
    uint8* nodes{}; uint8* navi{}; uint32* links{}; uint8* lens{}; uint8* inter{}; uint16* naviLinks{};
    std::vector<int> linkCount;
    uint8* Node(int i) const { return nodes+0x1C*size_t(i); }
};
inline std::vector<Area> world;
inline Area* AreaById(int id){ for(auto& a:world) if(a.id==id) return &a; return nullptr; }

struct Opts {
    int vehMin{0}, vehMax{6}, pedMin{0}, pedMax{6}, maxLinks{4}, naviMax{6};
    uint32 pctUnloadedLink{10}, pctWater{20};
    float cx{0.f}, cy{0.f}, cz{0.f}, half{100.f};
    bool scramble{};     // los campos m_wAreaId/m_wNodeId de cada nodo son aleatorios (el original usa los de la direccion en vez de los del nodo)
    bool symmetric{};    // enlaces bidireccionales (el original revienta si SwitchOffNodeAndNeighbours encuentra dos vecinos y outNext2 es nulo)
};
inline int16 Q8(float v){ return int16(std::lround(v*8.f)); }
inline void SetNodePos(uint8* n,float x,float y,float z){ At<int16>(n,8)=Q8(x); At<int16>(n,0xA)=Q8(y); At<int16>(n,0xC)=Q8(z); }
inline CVector NodePos(const uint8* n){ return CVector{ float(*reinterpret_cast<const int16*>(n+8))*0.125f, float(*reinterpret_cast<const int16*>(n+0xA))*0.125f, float(*reinterpret_cast<const int16*>(n+0xC))*0.125f }; }
inline uint8 RandBehaviour(Rng& r){ return uint8(r.pickI({0,1,1,2,2,3,4,5,8,10,11,12,15})); }
inline uint32 PackAddr(int area,int node){ return uint32(uint16(area))|(uint32(uint16(node))<<16); }

// Rellena las 72 ranuras de punteros y contadores de ThePaths con "ninguna area cargada" (no se tocan las demas zonas del objeto)
inline void ClearAreaTables(){
    for(int i=0;i<72;++i){ P32(oPathNodes+4*size_t(i))=0; P32(oNaviNodes+4*size_t(i))=0; P32(oNodeLinks+4*size_t(i))=0; P32(oLinkLengths+4*size_t(i))=0; P32(oIntersections+4*size_t(i))=0;
        P32(oNumNodes+4*size_t(i))=0; P32(oNumVeh+4*size_t(i))=0; P32(oNumPed+4*size_t(i))=0; P32(oNumNavi+4*size_t(i))=0; P32(oNumAddr+4*size_t(i))=0; }
    for(int i=0;i<64;++i){ P32(oNaviLinks+4*size_t(i))=0; P32(oTempNodes+4*size_t(i))=0; }
}
inline void RegisterArea(const Area& a){
    const size_t i=size_t(a.id);
    P32(oPathNodes+4*i)=uint32(uintptr_t(a.nodes)); P32(oNaviNodes+4*i)=a.nNavi ? uint32(uintptr_t(a.navi)) : 0;
    P32(oNodeLinks+4*i)=a.nAddr ? uint32(uintptr_t(a.links)) : 0; P32(oLinkLengths+4*i)=a.nAddr ? uint32(uintptr_t(a.lens)) : 0;
    P32(oIntersections+4*i)=a.nAddr ? uint32(uintptr_t(a.inter)) : 0; if(a.id<64) P32(oNaviLinks+4*i)=a.nAddr ? uint32(uintptr_t(a.naviLinks)) : 0;
    P32(oNumNodes+4*i)=uint32(a.nNodes); P32(oNumVeh+4*i)=uint32(a.nVeh); P32(oNumPed+4*i)=uint32(a.nPed); P32(oNumNavi+4*i)=uint32(a.nNavi); P32(oNumAddr+4*i)=uint32(a.nAddr);
}
// ids: areas cargadas (0..63 mapa, 64..71 interiores). Reinicia el arena.
inline void BuildWorld(Rng& r,const std::vector<int>& ids,const Opts& o){
    world.clear(); arenaTop=0;
    for(int id:ids){ Area a; a.id=id; a.nVeh=o.vehMin+int(r.below(uint32(o.vehMax-o.vehMin+1))); a.nPed=o.pedMin+int(r.below(uint32(o.pedMax-o.pedMin+1)));
        a.nNodes=a.nVeh+a.nPed; a.nNavi= id<64 ? int(r.below(uint32(o.naviMax+1))) : 0; world.push_back(a); }
    auto unloadedArea=[&]()->int{ for(int t=0;t<64;++t){ const int c=int(r.below(64)); bool used=false; for(auto& w:world) if(w.id==c) used=true; if(!used) return c; } for(int c=0;c<64;++c){ bool used=false; for(auto& w:world) if(w.id==c) used=true; if(!used) return c; } return 63; };
    // listas de adyacencia por area y nodo
    std::vector<std::vector<std::vector<uint32>>> adj(world.size());
    for(size_t w=0;w<world.size();++w) adj[w].resize(size_t(world[w].nNodes));
    if(!o.symmetric){
        for(size_t w=0;w<world.size();++w) for(int i=0;i<world[w].nNodes;++i){ const int nl=int(r.below(uint32(o.maxLinks+1)));
            for(int k=0;k<nl;++k){
                if(r.pct(o.pctUnloadedLink)){ adj[w][size_t(i)].push_back(PackAddr(unloadedArea(),int(r.below(40)))); continue; }
                const size_t tw=r.below(uint32(world.size()));
                if(world[tw].nNodes==0){ adj[w][size_t(i)].push_back(PackAddr(unloadedArea(),int(r.below(40)))); continue; }
                adj[w][size_t(i)].push_back(PackAddr(world[tw].id,int(r.below(uint32(world[tw].nNodes))))); } }
    } else {
        std::vector<std::pair<size_t,int>> all; for(size_t w=0;w<world.size();++w) for(int i=0;i<world[w].nNodes;++i) all.push_back({w,i});
        if(all.size()>=2){ const uint32 edges=r.below(uint32(all.size())*uint32(o.maxLinks)/2+1);
            for(uint32 e=0;e<edges;++e){ auto u=all[r.below(uint32(all.size()))]; auto v=all[r.below(uint32(all.size()))]; if(u==v) continue;
                auto& lu=adj[u.first][size_t(u.second)]; auto& lv=adj[v.first][size_t(v.second)];
                if(int(lu.size())>=o.maxLinks||int(lv.size())>=o.maxLinks) continue;
                lu.push_back(PackAddr(world[v.first].id,v.second)); lv.push_back(PackAddr(world[u.first].id,u.second)); } }
        for(size_t w=0;w<world.size();++w) for(int i=0;i<world[w].nNodes;++i) if(int(adj[w][size_t(i)].size())<o.maxLinks&&r.pct(o.pctUnloadedLink)) adj[w][size_t(i)].push_back(PackAddr(unloadedArea(),int(r.below(40))));
    }
    for(size_t w=0;w<world.size();++w){ Area& a=world[w]; a.linkCount.assign(size_t(a.nNodes),0); int sum=0; for(int i=0;i<a.nNodes;++i){ a.linkCount[size_t(i)]=int(adj[w][size_t(i)].size()); sum+=a.linkCount[size_t(i)]; } a.nAddr=sum; }
    for(size_t w=0;w<world.size();++w){
        Area& a=world[w];
        a.nodes=ArenaAlloc(size_t(std::max(a.nNodes,1))*0x1C);
        a.navi=ArenaAlloc(size_t(std::max(a.nNavi,1))*0xE);
        a.links=reinterpret_cast<uint32*>(ArenaAlloc(size_t(a.nAddr+192)*4));
        a.lens=ArenaAlloc(size_t(a.nAddr+192)); a.inter=ArenaAlloc(size_t(a.nAddr+192));
        a.naviLinks=reinterpret_cast<uint16*>(ArenaAlloc(size_t(std::max(a.nAddr,1))*2));
        for(int i=0;i<a.nAddr+192;++i){ a.links[i]=0xFFFFFFFFu; }
        FillBytes(a.lens,size_t(a.nAddr+192),r); FillBytes(a.inter,size_t(a.nAddr+192),r);
        int base=0;
        for(int i=0;i<a.nNodes;++i){
            uint8* n=a.Node(i); FillBytes(n,0x1C,r);
            At<uint32>(n,0)=0; At<uint32>(n,4)=0;
            SetNodePos(n,o.cx+r.uni(-o.half,o.half),o.cy+r.uni(-o.half,o.half),o.cz+r.uni(-o.half*0.2f,o.half*0.2f));
            At<int16>(n,0xE)= r.pct(80) ? int16(0x7FFE) : int16(r.next());
            At<int16>(n,0x10)=int16(base);
            At<uint16>(n,0x12)= o.scramble ? uint16(r.next()) : uint16(a.id); At<uint16>(n,0x14)= o.scramble ? uint16(r.next()) : uint16(i);
            if(r.pct(20)) At<uint8>(n,0x16)=0;
            n[0x18]=uint8((n[0x18]&0x7F)|(r.pct(o.pctWater)?0x80:0)); n[0x18]=uint8((n[0x18]&0xF0)|a.linkCount[size_t(i)]);
            n[0x1A]=uint8((n[0x1A]&0x0F)|(RandBehaviour(r)<<4));
            for(int k=0;k<a.linkCount[size_t(i)];++k) a.links[base+k]=adj[w][size_t(i)][size_t(k)];
            base+=a.linkCount[size_t(i)];
        }
        for(int i=0;i<a.nNavi;++i){ uint8* l=a.navi+0xE*size_t(i); FillBytes(l,0xE,r); At<int16>(l,0)=Q8(o.cx+r.uni(-o.half,o.half)); At<int16>(l,2)=Q8(o.cy+r.uni(-o.half,o.half)); }
        for(int i=0;i<a.nAddr;++i){ const Area& t=world[r.below(uint32(world.size()))]; a.naviLinks[i]= (t.nNavi>0&&t.id<64&&r.pct(80)) ? uint16((uint32(t.id)<<10)|r.below(uint32(t.nNavi))) : uint16(0xFFFF); }
    }
    ClearAreaTables();
    for(auto& a:world) RegisterArea(a);
}

// ======================= estado global =======================
// Poblacion determinista de ThePaths y las globales vecinas: basura determinista salvo los campos que se leen como flotantes/booleanos
inline void ResetState(Rng& r){
    FillBytes(PathsP(),kPathsSize,r);
    ClearAreaTables();
    for(int i=0;i<512;++i) P32(oHash+4*i)=0;
    for(int i=0;i<64*16;++i){ PI32(oDynBase+4*i)=-1; PI32(oDynIds+4*i)=-1; }
    for(int i=0;i<8;++i) PI32(oInteriorIds+4*i)=-1;
    P32(oNumSwitches)=0; PI32(oHashCount)=0; P32(oPending)=0;
    PF(oReqMinX)=FiniteF(r); PF(oReqMaxX)=FiniteF(r); PF(oReqMinY)=FiniteF(r); PF(oReqMaxY)=FiniteF(r);
    for(int i=0;i<64;++i) FillSwitchEntry(PathsP()+oSwitches+0x1C*size_t(i),r);
    // globales vecinas
    FillBytes(reinterpret_cast<void*>(kGlobBegin),kGlobEnd-kGlobBegin,r);
    for(int i=0;i<64;++i){ AtA<float>(gX+4*i)=FiniteF(r); AtA<float>(gY+4*i)=FiniteF(r); AtA<float>(gZ+4*i)=FiniteF(r); }
    for(int i=0;i<64;++i){ AtA<uint8>(gForScript+i)=uint8(r.below(2)); AtA<uint8>(gToBeStreamed+i)=uint8(r.below(2)); }
    AtA<uint8>(gLoadNeeded)=0; AtA<uint8>(gIntBuilding)=uint8(r.below(2));
    for(int i=0;i<3;++i) AtA<float>(gPathsNeededPos+4*i)=FiniteF(r);
    for(int i=0;i<8;++i) AtA<uint32>(gExtLinkedTo+4*i)=r.next();
    FillBytes(reinterpret_cast<void*>(gString),352,r);
    arenaTop=0; mallocTop=0; std::memset(arena.data(),0xCD,arena.size()); std::memset(mallocArena.data(),0xCD,mallocArena.size());
    streamData.clear(); streamPos=0; playerPresent=false; playerPos=CVector{0.f,0.f,0.f}; findResult=CNodeAddress{};
}

// Copia del estado visible tras una ejecucion (para diagnosticar diferencias) y su resumen
struct Blob { std::vector<uint8> b; std::vector<std::pair<std::string,size_t>> seg; };
inline void AddSeg(Blob& bl,const char* nm,const void* p,size_t n){ bl.seg.push_back({nm,bl.b.size()}); const auto* q=static_cast<const uint8*>(p); bl.b.insert(bl.b.end(),q,q+n); }
inline void Capture(Blob& bl,json& o){
    bl.b.clear(); bl.seg.clear();
    AddSeg(bl,"paths",PathsP(),kPathsSize);
    AddSeg(bl,"globals",reinterpret_cast<void*>(kGlobBegin),kGlobEnd-kGlobBegin);
    AddSeg(bl,"paths_needed",reinterpret_cast<void*>(gPathsNeededPos),12);
    AddSeg(bl,"ext_linked",reinterpret_cast<void*>(gExtLinkedTo),32);
    AddSeg(bl,"gstring",reinterpret_cast<void*>(gString),352);
    AddSeg(bl,"arena",arena.data(),std::min(arenaTop+64,arena.size()));
    AddSeg(bl,"malloc",mallocArena.data(),std::min(mallocTop+64,mallocArena.size()));
    o["paths"]=Fnv(PathsP(),kPathsSize); o["globals"]=Fnv(reinterpret_cast<void*>(kGlobBegin),kGlobEnd-kGlobBegin);
    o["paths_needed"]=Fnv(reinterpret_cast<void*>(gPathsNeededPos),12); o["ext_linked"]=Fnv(reinterpret_cast<void*>(gExtLinkedTo),32);
    o["gstring"]=Fnv(reinterpret_cast<void*>(gString),352); o["arena"]=Fnv(arena.data(),std::min(arenaTop+64,arena.size()));
    o["malloc"]=Fnv(mallocArena.data(),std::min(mallocTop+64,mallocArena.size()));
}
inline json BlobDiff(const Blob& a,const Blob& b){
    json d=json::array(); size_t n=std::min(a.b.size(),b.b.size()); if(a.b.size()!=b.b.size()) d.push_back("size_differs");
    for(size_t i=0;i<n&&d.size()<12;++i) if(a.b[i]!=b.b[i]){ std::string s="?"; size_t off=i; for(auto& sg:a.seg) if(sg.second<=i){ s=sg.first; off=i-sg.second; } d.push_back(s+"+"+std::to_string(off)+":"+std::to_string(a.b[i])+"/"+std::to_string(b.b[i])); }
    return d;
}
}
