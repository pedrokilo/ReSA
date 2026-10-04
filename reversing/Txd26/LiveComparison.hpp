#pragma once
// Banco dentro del motor: almacen de diccionarios de texturas (lista de 25; las 24 funciones pendientes de CTxdStore).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures deterministas sobre un
// pool falso de TxdDef (8-64 slots; 5000 en GameShutdown, cuyo bucle del EXE recorre 5000 slots fijos) cuyo puntero
// global (0xC8800C) se cambia solo durante el caso y se restaura, y sobre diccionarios RwTexDictionary y texturas
// falsos (el padre se guarda en el desplazamiento del plugin, ms_txdPluginOffset 0xC88018). Nunca se carga un TXD
// real ni se destruye un diccionario real: se simulan y trazan (sin reservas, en la direccion del EXE: los simbolos
// RW de la DLL son envoltorios de esas direcciones) RwTexDictionaryGetCurrent/SetCurrent/FindNamedTexture/Create/
// Destroy/ForAllTextures, RwTextureDestroy, RwTexDictionaryRemoveTexture, RwStreamFindChunk/Open/Close, las lecturas
// RwTexDictionaryGtaStreamRead/1/2, RwTextureSetFindCallBack/SetReadCallBack, CStreaming::RemoveModel (EXE + DLL) y
// operator new/delete (solo en Initialise y Shutdown). Se guardan y restauran los globales de CTxdStore (pool,
// diccionario guardado, ultimo slot encontrado, defaultTxds). Las retrollamadas se normalizan (direccion del EXE o
// simbolo de la DLL de TxdStoreFindCB/TxdStoreLoadCB/RemoveIfRefCountIsGreaterThanOne): con hooks son equivalentes.
// Los slots usados por las funciones que el EXE no comprueba (desreferencia nullptr) son siempre slots ocupados,
// salvo en StartLoadTxd/LoadTxd-stream con el chunk ausente, donde el EXE no llega a escribir.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "TxdStore.h"
#include "KeyGen.h"
#include "Streaming.h"
#include "CutsceneMgr.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"

namespace Txd26Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- direcciones del EXE ---
constexpr uintptr_t TXDPOOL=0xC8800C, STORED=0xC88010, LASTFOUND=0xC88014, DEFAULTS=0xC88004, PLUGOFF=0xC88018;
constexpr int32 NSLOT=5000; constexpr size_t DEFSZ=0xC;
constexpr int32 NDICT=16, NTEX=16; constexpr size_t DICTSZ=0x100, TEXSZ=0x60;

// --- fixtures ---
alignas(16) inline std::array<uint8,NSLOT*DEFSZ> storage{};
inline std::array<uint8,NSLOT> states{};
alignas(16) inline std::array<uint8,0x14> poolHdr{};
alignas(16) inline std::array<std::array<uint8,DICTSZ>,NDICT> dicts{};
alignas(16) inline std::array<std::array<uint8,TEXSZ>,NTEX> texs{};
alignas(16) inline std::array<uint8,0x40> stream{};
alignas(16) inline std::array<uint8,0x11000> arena{}; inline size_t arenaTop{};
inline std::vector<int32> used, freeSlots;
inline int32 cap{}; inline int32 plugOff{};
inline uint8* Slot(int32 i) { return storage.data()+size_t(i)*DEFSZ; }
inline TxdDef* Def(int32 i) { return reinterpret_cast<TxdDef*>(Slot(i)); }
inline RwTexDictionary* Dict(int32 k) { return reinterpret_cast<RwTexDictionary*>(dicts[k].data()); }
inline RwTexDictionary*& Parent(int32 k) { return At<RwTexDictionary*>(dicts[k].data(),size_t(plugOff)); }

inline uint32 Name(const void* p) {
    // direcciones de los fixtures como desplazamiento etiquetado (estable entre ejecuciones)
    const auto v=uintptr_t(p); if(!v) return 0;
    auto in=[&](const void* b,size_t n,uint32 tag)->std::optional<uint32>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return (tag<<24)|uint32(v-uintptr_t(b)); return std::nullopt; };
    if(auto r=in(storage.data(),storage.size(),1))return *r; if(auto r=in(states.data(),states.size(),2))return *r; if(auto r=in(poolHdr.data(),poolHdr.size(),3))return *r;
    if(auto r=in(arena.data(),arena.size(),4))return *r; if(auto r=in(dicts.data(),sizeof(dicts),5))return *r; if(auto r=in(texs.data(),sizeof(texs),6))return *r;
    if(auto r=in(stream.data(),stream.size(),7))return *r;
    return 0xEEEEEEEEu; // puntero ajeno a los fixtures
}
// retrollamadas: direccion del EXE o simbolo de la DLL (con hooks, equivalentes)
inline uint32 Callback(const void* p) {
    using std::bit_cast; const auto v=uintptr_t(p);
    if(v==0x731720||v==bit_cast<uintptr_t>(&CTxdStore::TxdStoreFindCB)) return 1;
    if(v==0x731710||v==bit_cast<uintptr_t>(&CTxdStore::TxdStoreLoadCB)) return 2;
    if(v==0x731680||v==bit_cast<uintptr_t>(&RemoveIfRefCountIsGreaterThanOne)) return 3;
    return Name(p);
}

// --- traza sin reservas ---
struct Ev { uint32 kind, a; }; inline std::array<Ev,16384> evs{}; inline int nev{};
inline void Push(uint32 k,uint32 a) { if(nev<int(evs.size()))evs[nev++]={k,a}; }
inline void Tag(uint32 t) { Push(0,t); }
inline void PushP(const void* p) { Push(1,Name(p)); }
inline void PushV(uint32 v) { Push(2,v); }
inline void PushS(const char* s) { Push(3,s ? uint32(Fnv(s,std::strlen(s))) : 0u); }
inline uint32 masks[4]{}, calls[4]{};
inline bool Bit(int m) { return (masks[m]>>(calls[m]++%32))&1; }

// --- stubs ---
inline RwTexDictionary* current{}; inline RwTexDictionary* createResult{}; inline RwTexDictionary* readResult{};
inline bool chunkAllowed{}; inline int32 openFails{};
inline RwTexDictionary* __cdecl FakeGetCurrent() { Tag(100); return current; }
inline RwTexDictionary* __cdecl FakeSetCurrent(RwTexDictionary* d) { Tag(101); PushP(d); current=d; return d; }
inline RwTexture* __cdecl FakeFindNamed(RwTexDictionary* d,const char* n) { Tag(102); PushP(d); PushS(n);
    if(!Bit(0)) return nullptr; const auto k=(uintptr_t(d)-uintptr_t(dicts.data()))/DICTSZ; return reinterpret_cast<RwTexture*>(texs[k%NTEX].data()); }
inline RwTexDictionary* __cdecl FakeDictCreate() { Tag(103); return createResult; }
inline RwBool __cdecl FakeDictDestroy(RwTexDictionary* d) { Tag(104); PushP(d); return TRUE; }
inline const RwTexDictionary* __cdecl FakeForAll(const RwTexDictionary* d,void* cb,void* data) { Tag(105); PushP(d); PushV(Callback(cb)); PushP(data); return d; }
inline RwBool __cdecl FakeTexDestroy(RwTexture* t) { Tag(106); PushP(t); return TRUE; }
inline RwTexture* __cdecl FakeRemoveTex(RwTexture* t) { Tag(107); PushP(t); return t; }
inline RwBool __cdecl FakeFindChunk(RwStream* s,RwUInt32 type,RwUInt32* len,RwUInt32* ver) { Tag(108); PushP(s); PushV(type); PushP(len); PushP(ver); return chunkAllowed&&Bit(1); }
inline RwStream* __cdecl FakeStreamOpen(int32 type,int32 access,const void* data) { Tag(109); PushV(uint32(type)); PushV(uint32(access)); PushS(static_cast<const char*>(data));
    if(openFails>0){ --openFails; return nullptr; } return reinterpret_cast<RwStream*>(stream.data()); }
inline RwBool __cdecl FakeStreamClose(RwStream* s,void* data) { Tag(110); PushP(s); PushP(data); return TRUE; }
inline RwTexDictionary* __cdecl FakeRead(RwStream* s) { Tag(111); PushP(s); return readResult; }
inline RwTexDictionary* __cdecl FakeRead1(RwStream* s) { Tag(112); PushP(s); return readResult; }
inline RwTexDictionary* __cdecl FakeRead2(RwStream* s,RwTexDictionary* d) { Tag(113); PushP(s); PushP(d); return readResult; }
inline RwBool __cdecl FakeSetFindCB(void* cb) { Tag(114); PushV(Callback(cb)); return TRUE; }
inline RwBool __cdecl FakeSetReadCB(void* cb) { Tag(115); PushV(Callback(cb)); return TRUE; }
inline void __cdecl FakeRemoveModel(int32 id) { Tag(116); PushV(uint32(id)); }
inline void* __cdecl FakeNew(size_t n) { Tag(117); PushV(uint32(n)); arenaTop=(arenaTop+15)&~size_t(15); void* r=arena.data()+arenaTop; arenaTop+=n; if(arenaTop>arena.size())arenaTop=0; return r; }
inline void* __cdecl FakeNewArr(size_t n) { Tag(118); PushV(uint32(n)); arenaTop=(arenaTop+15)&~size_t(15); void* r=arena.data()+arenaTop; arenaTop+=n; if(arenaTop>arena.size())arenaTop=0; return r; }
inline void __cdecl FakeDelete(void* p) { Tag(119); PushP(p); }
inline void __cdecl FakeDeleteArr(void* p) { Tag(120); PushP(p); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,24> fns{{
    {"CTxdStore","AddRef",0x731A00,0,-1,256},{"CTxdStore","Create",0x731990,0,-1,256},
    {"CTxdStore","FindTxdSlot-hash",0x7318E0,0,-1,512},{"CTxdStore","FindTxdSlot-name",0x731850,0,-1,1024},
    {"CTxdStore","FinishLoadTxd",0x731E40,0,-1,512},{"CTxdStore","GameShutdown",0x732060,0,-1,64},
    {"CTxdStore","GetNumRefs",0x731AA0,0,-1,256},{"CTxdStore","GetParentTxdSlot",0x408370,0,-1,256},
    {"CTxdStore","GetTxd",0x408340,0,-1,256},{"CTxdStore","Initialise",0x731F20,0,-1,128},
    {"CTxdStore","LoadTxd-filename",0x7320B0,0,-1,256},{"CTxdStore","LoadTxd-stream",0x731DD0,0,-1,512},
    {"CTxdStore","PopCurrentTxd",0x7316B0,0,-1,128},{"CTxdStore","PushCurrentTxd",0x7316A0,0,-1,128},
    {"CTxdStore","RemoveIfRefCountIsGreaterThanOne",0x731680,0,-1,256},{"CTxdStore","RemoveRef",0x731A30,0,-1,512},
    {"CTxdStore","RemoveRefWithoutDelete",0x731A70,0,-1,256},{"CTxdStore","RemoveTxd",0x731E90,0,-1,512},
    {"CTxdStore","SetCurrentTxd",0x7319C0,0,-1,256},{"CTxdStore","SetupTxdParent",0x731D50,0,-1,512},
    {"CTxdStore","Shutdown",0x732000,0,-1,128},{"CTxdStore","StartLoadTxd",0x731930,0,-1,512},
    {"CTxdStore","TxdStoreFindCB",0x731720,0,-1,512},{"CTxdStore","TxdStoreLoadCB",0x731710,0,-1,64},
}};
// hooks ya validados que tambien se conmutan (el original queda entero en el EXE)
inline const char* const extraHooks[]{"AddTxdSlot","RemoveTxdSlot"};

inline const char* const names[]{"particle","PARTICLE","hud","generic","vehicle","fronten1","fronten2","LOADSCS","infernus","CJ_Tex","*","","txd","Particle","fonts","radar00"};
inline std::array<uint32,std::size(names)> hashes{};

// Pool falso: ~1/2 de slots ocupados, diccionarios falsos o nulos, padres -1 u ocupados, hashes de la lista o aleatorios
template<class N> inline void BuildPool(N& next,int32 capacity,int needFree) {
    cap=capacity; used.clear(); freeSlots.clear();
    static const int16 refs[]{0,0,0,1,1,2,5,-1,0x7FFF,-0x8000,100};
    for(int32 i=0;i<cap;++i){
        uint8* e=Slot(i); for(size_t k=0;k<DEFSZ;k+=4){ const uint32 v=next(); std::memcpy(e+k,&v,4); }
        auto* d=Def(i);
        d->m_pRwDictionary=next()%4==0 ? nullptr : Dict(int32(next()%NDICT));
        d->m_wRefsCount=refs[next()%std::size(refs)];
        d->m_hash=next()%3 ? hashes[next()%hashes.size()] : next();
        const bool isUsed=next()%2==0;
        states[i]=uint8(isUsed ? (next()&0x7F) : (0x80|(next()&0x7F)));
    }
    if(cap>NSLOT/2){ for(int32 i=0;i<cap;++i)(states[i]&0x80 ? freeSlots : used).push_back(i); }
    else {
        auto count=[&]{ used.clear(); freeSlots.clear(); for(int32 i=0;i<cap;++i)(states[i]&0x80 ? freeSlots : used).push_back(i); };
        count(); while(used.size()<2){ states[next()%uint32(cap)]&=0x7F; count(); }
        while(int(freeSlots.size())<needFree){ states[used[next()%used.size()]]|=0x80; count(); }
    }
    // padres: -1 u otro slot ocupado (el EXE no comprueba el slot del padre)
    for(int32 i=0;i<cap;++i) Def(i)->m_wParentIndex=next()%3==0 ? int16(-1) : int16(used[next()%used.size()]);
    poolHdr.fill(0);
    At<void*>(poolHdr.data(),0)=storage.data(); At<void*>(poolHdr.data(),4)=states.data(); At<int32>(poolHdr.data(),8)=cap;
    At<int32>(poolHdr.data(),0xC)=int32(next()%uint32(cap+1))-1; poolHdr[0x10]=uint8(next()%2); poolHdr[0x11]=uint8(next()%2);
}
template<class N> inline int32 PickUsed(N& next) { return used[next()%used.size()]; }
// Diccionarios falsos: bytes aleatorios, padre nulo o un diccionario de indice mayor (sin ciclos)
template<class N> inline void BuildDicts(N& next) {
    for(int32 k=0;k<NDICT;++k){ for(auto& b:dicts[k])b=uint8(next()); Parent(k)=k+1<NDICT&&next()%3 ? Dict(k+1+int32(next()%uint32(NDICT-k-1))) : nullptr; }
    for(auto& t:texs)for(auto& b:t)b=uint8(next());
    for(auto& b:stream)b=uint8(next());
}
inline json PoolDiff(const std::vector<uint8>& s0,const std::vector<uint8>& st0,int32 n) {
    json d=json::array();
    for(int32 i=0;i<n;++i) if(states[i]!=st0[i]||std::memcmp(Slot(i),&s0[size_t(i)*DEFSZ],DEFSZ)!=0) d.push_back({i,states[i],ReversingProbe::Bytes(Slot(i),DEFSZ)});
    return d;
}
inline json Header(const void* h) {
    return json::array({Name(At<void*>(const_cast<void*>(h),0)),Name(At<void*>(const_cast<void*>(h),4)),At<int32>(const_cast<void*>(h),8),At<int32>(const_cast<void*>(h),0xC),
                        static_cast<const uint8*>(h)[0x10],static_cast<const uint8*>(h)[0x11]});
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"txd26-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"txd26-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("txd26");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"txd26-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,24> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::vector<ReversibleHooks::ReversibleHook::Base*> all(hooks.begin(),hooks.end());
    for(auto* e:extraHooks){ auto* h=FindHook("CTxdStore",e); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string("CTxdStore::")+e}});return;} all.push_back(h); }
    std::vector<bool> enabled,locked; for(auto* h:all){enabled.push_back(h->Hooked());locked.push_back(h->Locked());h->LockState(false);}

    plugOff=At<int32>(reinterpret_cast<void*>(PLUGOFF),0);
    if(plugOff<0||size_t(plugOff)+4>DICTSZ){Write({{"event","error"},{"reason","txd plugin offset out of fixture range"},{"offset",plugOff}});return;}
    for(size_t k=0;k<std::size(names);++k)hashes[k]=CKeyGen::GetUppercaseKey(names[k]);

    // estado real guardado (globales de CTxdStore)
    struct Blob { uintptr_t p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{TXDPOOL,4},{STORED,4},{LASTFOUND,4},{DEFAULTS,8}};
    for(auto& b:blobs)b.saved.assign(reinterpret_cast<uint8*>(b.p),reinterpret_cast<uint8*>(b.p)+b.n);
    auto restoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.p),b.saved.data(),b.n); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(0x7F3A90,FakeGetCurrent), r2(0x7F3A70,FakeSetCurrent), r3(0x7F39F0,FakeFindNamed), r4(0x7F3600,FakeDictCreate);
        EntryPatch r5(0x7F36A0,FakeDictDestroy), r6(0x7F3730,FakeForAll), r7(0x7F3820,FakeTexDestroy), r8(0x7F39C0,FakeRemoveTex);
        EntryPatch r9(0x7ED2D0,FakeFindChunk), r10(0x7ECEF0,FakeStreamOpen), r11(0x7ECE20,FakeStreamClose);
        EntryPatch g1(0x730FC0,FakeRead), g2(0x731070,FakeRead1), g3(0x731150,FakeRead2);
        EntryPatch c1(0x7F34D0,FakeSetFindCB), c2(0x7F3500,FakeSetReadCB);
        EntryPatch s1(bit_cast<uintptr_t>(&CStreaming::RemoveModel),FakeRemoveModel), s1e(0x4089A0,FakeRemoveModel);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7D26u; if(!stt)stt=1; nev=0; arenaTop=0;
                    for(int k=0;k<4;++k){ masks[k]=next(); calls[k]=0; }
                    restoreAll();
                    const bool big=nm=="GameShutdown";
                    BuildPool(next,big ? NSLOT : int32(8+next()%57),nm=="Initialise" ? 4 : 1);
                    BuildDicts(next);
                    current=next()%4==0 ? nullptr : Dict(int32(next()%NDICT));
                    createResult=next()%5==0 ? nullptr : Dict(int32(next()%NDICT));
                    readResult=next()%4==0 ? nullptr : Dict(int32(next()%NDICT));
                    chunkAllowed=true; openFails=next()%4==0 ? int32(1+next()%3) : 0;
                    At<RwTexDictionary*>(reinterpret_cast<void*>(STORED),0)=next()%4==0 ? nullptr : Dict(int32(next()%NDICT));
                    At<int32>(reinterpret_cast<void*>(LASTFOUND),0)=int32(next()%uint32(cap+1))-1;
                    for(int k=0;k<4;++k)At<int16>(reinterpret_cast<void*>(DEFAULTS),size_t(k)*2)=int16(next());
                    At<void*>(reinterpret_cast<void*>(TXDPOOL),0)=poolHdr.data();
                    const std::vector<uint8> s0(storage.begin(),storage.begin()+size_t(cap)*DEFSZ), st0(states.begin(),states.begin()+cap);
                    const int32 cap0=cap;

                    for(auto* h:all)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    using VoidI=void(__cdecl*)(int32);

                    if(nm=="AddRef"||nm=="RemoveRef"||nm=="RemoveRefWithoutDelete"||nm=="SetCurrentTxd"||nm=="RemoveTxd"||nm=="SetupTxdParent"||nm=="Create"){
                        const int32 k=PickUsed(next); out["index"]=k;
                        if(nm=="SetupTxdParent"&&!Def(k)->m_pRwDictionary) Def(k)->m_pRwDictionary=Dict(int32(next()%NDICT)); // el EXE escribe en el diccionario
                        reinterpret_cast<VoidI>(a)(k);
                    }
                    else if(nm=="GetNumRefs"||nm=="GetParentTxdSlot"){ const int32 k=PickUsed(next); out["index"]=k; ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(k); }
                    else if(nm=="GetTxd"){ const int32 k=PickUsed(next); out["index"]=k; ret=Name(reinterpret_cast<RwTexDictionary*(__cdecl*)(int32)>(a)(k)); }
                    else if(nm=="FindTxdSlot-hash"){
                        const uint32 r=next()%4; const uint32 h=r==0 ? next() : r==1 ? Def(int32(next()%uint32(cap)))->m_hash : hashes[next()%hashes.size()];
                        ret=reinterpret_cast<int32(__cdecl*)(uint32)>(a)(h);
                    }
                    else if(nm=="FindTxdSlot-name"){ ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(names[next()%std::size(names)]); }
                    else if(nm=="StartLoadTxd"||nm=="LoadTxd-stream"){
                        // slot libre: el EXE solo no escribe si falta el chunk
                        const bool freeSlot=!freeSlots.empty()&&next()%4==0; const int32 k=freeSlot ? freeSlots[next()%freeSlots.size()] : PickUsed(next);
                        out["index"]=k; chunkAllowed=!freeSlot;
                        ret=reinterpret_cast<bool(__cdecl*)(int32,RwStream*)>(a)(k,reinterpret_cast<RwStream*>(stream.data()));
                    }
                    else if(nm=="FinishLoadTxd"){ const int32 k=PickUsed(next); out["index"]=k; ret=reinterpret_cast<bool(__cdecl*)(int32,RwStream*)>(a)(k,reinterpret_cast<RwStream*>(stream.data())); }
                    else if(nm=="LoadTxd-filename"){
                        static const char* const files[]{"MODELS\\GENERIC\\VEHICLE.TXD","models\\fronten1.txd","MODELS\\PARTICLE.TXD","models\\hud.txd","x.txd","",
                            "MODELS\\TXD\\A_VERY_LONG_DIRECTORY_NAME_FOR_THE_TEXTURE_DICTIONARY_LOADER\\AND_ANOTHER_ONE\\file_with_a_long_name_too.txd"};
                        const int32 k=PickUsed(next); out["index"]=k;
                        ret=reinterpret_cast<bool(__cdecl*)(int32,const char*)>(a)(k,files[next()%std::size(files)]);
                    }
                    else if(nm=="PushCurrentTxd"||nm=="PopCurrentTxd"||nm=="GameShutdown"){ reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="RemoveIfRefCountIsGreaterThanOne"){
                        auto& t=texs[next()%NTEX]; static const int32 rc[]{0,1,1,2,3,-1,0x7FFFFFFF,INT32_MIN,100};
                        At<int32>(t.data(),0x54)=rc[next()%std::size(rc)];
                        void* data=next()%2 ? nullptr : static_cast<void*>(dicts[0].data());
                        ret=Name(reinterpret_cast<RwTexture*(__cdecl*)(RwTexture*,void*)>(a)(reinterpret_cast<RwTexture*>(t.data()),data));
                    }
                    else if(nm=="TxdStoreFindCB"){ ret=Name(reinterpret_cast<RwTexture*(__cdecl*)(const char*)>(a)(names[next()%std::size(names)])); }
                    else if(nm=="TxdStoreLoadCB"){ ret=Name(reinterpret_cast<RwTexture*(__cdecl*)(const char*,const char*)>(a)(names[next()%std::size(names)],next()%2 ? nullptr : "mask")); }
                    else if(nm=="Initialise"){
                        const bool keep=next()%4==0; At<void*>(reinterpret_cast<void*>(TXDPOOL),0)=keep ? static_cast<void*>(poolHdr.data()) : nullptr;
                        arena.fill(0x5A);
                        { EntryPatch n1(0x82119A,FakeNew), n2(0x821195,FakeNewArr), d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDeleteArr); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        auto* np=At<uint8*>(reinterpret_cast<void*>(TXDPOOL),0);
                        if(np&&np!=poolHdr.data()){ out["new_pool"]=Header(np); auto* objs=At<uint8*>(np,0); auto* mp=At<uint8*>(np,4);
                            out["new_map"]=Fnv(mp,NSLOT); out["slots0"]=ReversingProbe::Bytes(objs,4*DEFSZ); }
                        out["arena"]=Fnv(arena.data(),arena.size());
                    }
                    else if(nm=="Shutdown"){
                        if(next()%4==0) At<int32>(poolHdr.data(),8)=next()%2 ? 0 : -1;
                        { EntryPatch d1(0x8214BD,FakeDelete), d2(0x8213AE,FakeDeleteArr); reinterpret_cast<void(__cdecl*)()>(a)(); }
                    }

                    for(auto* h:all)h->State(false);
                    out["pool"]=PoolDiff(s0,st0,cap0);
                    out["header"]=Header(poolHdr.data());
                    { json p=json::array(); for(int32 k=0;k<NDICT;++k)p.push_back(Name(Parent(k))); out["parents"]=p; }
                    out["dicts"]=Fnv(dicts.data(),sizeof(dicts)); out["texs"]=Fnv(texs.data(),sizeof(texs)); out["stream"]=Fnv(stream.data(),stream.size());
                    out["globals"]={Name(At<void*>(reinterpret_cast<void*>(TXDPOOL),0)),Name(At<void*>(reinterpret_cast<void*>(STORED),0)),At<int32>(reinterpret_cast<void*>(LASTFOUND),0),
                                    ReversingProbe::Bytes(reinterpret_cast<void*>(DEFAULTS),8),Name(current),openFails};
                    out["ret"]=ret;
                    { json t=json::array(); for(int k=0;k<nev;++k)t.push_back({evs[k].kind,evs[k].a}); out["trace"]=t; }
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
    for(size_t k=0;k<all.size();++k){all[k]->State(enabled[k]);all[k]->LockState(locked[k]);}
    restoreAll();
    bool restored=true; for(size_t k=0;k<all.size();++k)restored&=all[k]->Hooked()==enabled[k];
    for(auto& b:blobs)restored&=std::memcmp(reinterpret_cast<void*>(b.p),b.saved.data(),b.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
