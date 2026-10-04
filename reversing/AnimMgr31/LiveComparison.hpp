#pragma once
// Banco dentro del motor: gestor de animaciones (lista de 31; las 31 funciones pendientes de CAnimManager).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures deterministas por caso.
// Nunca se carga un IFP real ni se toca el almacen real: las tablas globales del EXE (definiciones 0x8AA5A8 con sus
// 145 huecos, contadores 0xB4EA28-0xB4EA34, bloques 0xB5D4A0, jerarquias 2440-2499 de 0xB4EA40, cache LRU 0xB5EB20 y
// ms_modelInfoPtrs[7]) se guardan, se rellenan con fixtures durante el caso y se restauran tras cada ejecucion.
// Grupos, asociaciones estaticas, asociaciones, clump (byte de tipo 2 y plugin en el offset real 0xB5F878), enlaces y
// jerarquias de la cache, el fichero IFP y las lineas de ANIMGRP.DAT son buferes estaticos del banco (mismas
// direcciones en las dos ejecuciones, punteros comparados en crudo). Se simulan y trazan sin reservar memoria (solo
// durante la llamada; arena fija rellena con 0xCD): operator new/delete, CMemoryMgr::Malloc/MallocAlign/FreeAlign,
// RwStreamOpen/Read/Close, CFileMgr::SetDir/OpenFile/CloseFile, CFileLoader::LoadLine, CStreaming::RemoveModel,
// CAnimBlendHierarchy::Shutdown/CalcTotalTime/CalcTotalTimeCompressed/RemoveQuaternionFlips/RemoveUncompressedData/
// Uncompress, CAnimBlendAssocGroup::CopyAnimation/CreateAssociations/DestroyAssociations, CAnimBlendAssociation::Start/
// SyncAnimation/ReferenceAnimBlock, CAnimManager::UncompressAnimation (salvo cuando es la probada), CreateInstance de un
// model info falso, RpAnimBlendClumpInit, GetFirstAtomic, RpSkinGeometryGetSkin, RpClumpForAllAtomics y RpClumpDestroy.
// El constructor de asociaciones, los de grupos y secuencias, SetName/SetBoneTag/SetNumFrames y GetAnimation de los
// grupos corren de verdad (ya validados). La vtable y el callback por defecto de la DLL se leen como los del EXE y el
// puntero aStdAnimDescs de la DLL como 0x8A7788.
#include <fstream>
#include <optional>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MemoryMgr.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "AnimBlendAssocGroup.h"
#include "AnimBlendHierarchy.h"
#include "AnimBlendSequence.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "Streaming.h"
#include "KeyGen.h"
#include "RwHelper.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace AnimMgr31Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 Hs(const char* s){ return s ? uint32(Fnv(s,std::strlen(s))) : 0u; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }
// Sigue los saltos E9 de la DLL (thunks incrementales) hasta el cuerpo; se detiene si el salto va al EXE.
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> inline T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }

// --- direcciones del EXE ---
constexpr uintptr_t DEFS=0x8AA5A8, NUMDEFS=0xB4EA28, NUMANIMS=0xB4EA2C, NUMBLOCKS=0xB4EA30, GROUPS=0xB4EA34;
constexpr uintptr_t ANIMS=0xB4EA40, BLOCKS=0xB5D4A0, CACHE=0xB5EB20, MIPTR=0xA9B0C8+7*4, STDDESCS=0x8A7788;
constexpr size_t NDEF=145, DEFSZ=0x30, NBLK=180, BLKSZ=0x20, HSZ=0x18, CACHESZ=0x34, GSZ=0x14, SSZ=0x14, ASZ=0x3C;
constexpr int32 ABASE=2440, ACOUNT=60;
constexpr int32 NGD=40;                                    // grupos/definiciones con fixtures propios
inline uint8* Def(size_t d){ return reinterpret_cast<uint8*>(DEFS+d*DEFSZ); }
inline uint8* Blk(size_t k){ return reinterpret_cast<uint8*>(BLOCKS+k*BLKSZ); }
inline uint8* Hier(int32 i){ return reinterpret_cast<uint8*>(ANIMS+size_t(i)*HSZ); }

// --- traza sin reservas ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(!Main())return; if(nrec<recs.size())recs[nrec]={t,a,b}; ++nrec; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<std::min(nrec,recs.size());++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); r.push_back(nrec); return r; }
inline uint32 masks[4]{}, calls[4]{};
inline bool Bit(int m){ return (masks[m]>>(calls[m]++%32))&1; }

// --- buferes de los fixtures ---
alignas(16) inline std::array<uint8,4+NDEF*GSZ> groupsMem{};      // cookie de new[] + grupos
alignas(16) inline std::array<uint8,NGD*8*SSZ> statics{};          // asociaciones estaticas (8 por grupo)
alignas(16) inline std::array<uint8,NGD*8*8> descrs{};             // AnimDescriptor (8 por definicion)
alignas(16) inline std::array<uint8,NGD*8*4> namePtrs{};           // AnimNames (8 por definicion)
alignas(16) inline std::array<uint8,NGD*8*24> nameBufs{};
alignas(16) inline std::array<uint8,8*0x40> assocs{};              // asociaciones de la lista del clump
alignas(16) inline std::array<uint8,0x40> na{}, extra{};           // asociacion de CopyAnimation y una ajena
alignas(16) inline std::array<uint8,0x400> clump{}, clumpI{};      // clump probado y el de CreateInstance
alignas(16) inline std::array<uint8,0x20> cdata{};
alignas(16) inline std::array<uint8,4*0x18> frameDatas{};
alignas(16) inline std::array<uint8,8*12> cacheLinks{};
alignas(16) inline std::array<uint8,10*HSZ> cacheHiers{};
alignas(16) inline std::array<uint8,0x40> fakeMi{}, atomic{}, geometry{}, skin{}, stream{}, fileTok{};
alignas(16) inline std::array<uint8,0x6000> ifp{}; inline size_t ifpSize{}, ifpPos{};
inline std::array<std::array<char,48>,32> lines{}; inline size_t nLines{}, lineIdx{}; inline std::array<char,64> lineCur{};
alignas(16) inline std::array<char,4*32> uncomp{};
alignas(64) inline std::array<uint8,0x18000> arena{}; inline size_t arenaTop{};
alignas(64) inline std::array<uint8,0x18000> normBuf{};
inline uint8* Static(int32 g,int32 s){ return statics.data()+(size_t(g)*8+s)*SSZ; }
inline uint8* Assoc(int32 k){ return assocs.data()+size_t(k)*0x40; }
inline uint8* Group(int32 g){ return groupsMem.data()+4+size_t(g)*GSZ; }

inline bool In(const void* p,const void* b,size_t n){ return uintptr_t(p)>=uintptr_t(b)&&uintptr_t(p)<uintptr_t(b)+n; }
inline bool InFixtures(const void* p){
    return In(p,arena.data(),arena.size())||In(p,groupsMem.data(),groupsMem.size())||In(p,cacheLinks.data(),cacheLinks.size())
        ||In(p,namePtrs.data(),namePtrs.size())||In(p,nameBufs.data(),nameBufs.size())||In(p,statics.data(),statics.size()); }
inline uint32 InArena(const void* p){ return In(p,arena.data(),arena.size()) ? uint32(uintptr_t(p)-uintptr_t(arena.data())) : 0xFFFFFFFFu; }
inline void* Alloc(size_t n,size_t align){
    const size_t off=(arenaTop+align-1)&~(align-1);
    if(n>arena.size()-std::min(off,arena.size()))return nullptr;
    arenaTop=off+n; return arena.data()+off; }

// --- dependencias simuladas (sin reservas; fuera del hilo principal se reenvian al CRT del EXE) ---
enum : uint32 { T_NEW=1,T_DEL,T_MALLOC,T_MALIGN,T_FALIGN,T_READ,T_SOPEN,T_SCLOSE,T_SETDIR,T_FOPEN,T_FCLOSE,T_LINE,T_REMOVEMODEL,
    T_HSHUT,T_HTOTALC,T_HREMOVEU,T_HUNCOMP,T_HFLIPS,T_HTOTAL,T_COPY,T_CREATE,T_CREATE2,T_CREATE3,T_DESTROY,T_START,T_SYNC,T_REF,
    T_UNCOMP,T_CLUMPINIT,T_FIRSTATOMIC,T_SKIN,T_FORALL,T_CLUMPDESTROY,T_INSTANCE,T_TRAP };
inline void* __cdecl FakeNew(uint32 n){
    if(!Main())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    void* p=Alloc(n,16); Push(T_NEW,n,U(p)); return p; }
inline void __cdecl FakeDelete(void* p){
    if(!Main()||(p&&!InFixtures(p))){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    Push(T_DEL,U(p)); }
inline void* __cdecl FakeMalloc(uint32 n,uint32 hint){
    if(!Main())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    void* p=Alloc(n,16); Push(T_MALLOC,n,hint); Push(T_MALLOC,U(p)); return p; }
inline void* __cdecl FakeMallocAlign(uint32 size,uint32 align,uint32 hint){
    if(!Main()){ // como 0x72F4C0
        auto* m=reinterpret_cast<uint8*(__cdecl*)(size_t)>(0x824257)(size+align);
        auto* r=reinterpret_cast<uint8*>(uintptr_t(m+align)&~uintptr_t(align-1)); reinterpret_cast<void**>(r)[-1]=m; return r; }
    void* p=Alloc(size,64); Push(T_MALIGN,size,align); Push(T_MALIGN,hint,U(p)); return p; }
inline void __cdecl FakeFreeAlign(void* p){
    if(!Main()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(reinterpret_cast<void**>(p)[-1]); return; }
    Push(T_FALIGN,U(p)); }
inline RwUInt32 __cdecl FakeStreamRead(RwStream* s,void* buf,RwUInt32 len){
    if(!Main())return 0;
    const size_t left=ifpSize>ifpPos ? ifpSize-ifpPos : 0; const size_t n=std::min<size_t>(len,left);
    std::memcpy(buf,ifp.data()+ifpPos,n); ifpPos+=n; Push(T_READ,len,InArena(buf)); Push(T_READ,U(s),uint32(n)); return RwUInt32(n); }
inline RwStream* __cdecl FakeStreamOpen(int32 type,int32 access,const void* data){ Push(T_SOPEN,uint32(type)|(uint32(access)<<8),Hs(static_cast<const char*>(data))); return reinterpret_cast<RwStream*>(stream.data()); }
inline RwBool __cdecl FakeStreamClose(RwStream* s,void* data){ Push(T_SCLOSE,U(s),U(data)); return TRUE; }
inline int32 __cdecl FakeSetDir(const char* p){ Push(T_SETDIR,Hs(p)); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ Push(T_FOPEN,Hs(p),Hs(m)); return reinterpret_cast<FILESTREAM>(fileTok.data()); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ Push(T_FCLOSE,U(f)); return 0; }
inline char* __cdecl FakeLoadLine(FILESTREAM f){
    Push(T_LINE,U(f),uint32(lineIdx)); if(lineIdx>=nLines)return nullptr;
    std::memcpy(lineCur.data(),lines[lineIdx++].data(),48); lineCur[48]=0; return lineCur.data(); }
inline void __cdecl FakeRemoveModel(int32 id){ Push(T_REMOVEMODEL,uint32(id)); }
inline void __fastcall FakeHierShutdown(void* h,void*){ Push(T_HSHUT,U(h)); }
inline void __fastcall FakeHierTotalC(void* h,void*){ Push(T_HTOTALC,U(h)); }
inline void __fastcall FakeHierRemoveU(void* h,void*){ Push(T_HREMOVEU,U(h)); }
inline void __fastcall FakeHierUncomp(void* h,void*){ Push(T_HUNCOMP,U(h)); }
inline void __fastcall FakeHierFlips(void* h,void*){ Push(T_HFLIPS,U(h)); }
inline void __fastcall FakeHierTotal(void* h,void*){ Push(T_HTOTAL,U(h)); }
inline void* __fastcall FakeCopyAnim(void* g,void*,int32 id){ Push(T_COPY,U(g),uint32(id)); return na.data(); }
// CreateAssociations: deja m_Anims/m_NumAnims apuntando a las estaticas del grupo (el bucle de flags las usa).
inline void __fastcall FakeCreateAssocs(void* g,void*,const char* block,void* c,const char** names,uint32 cnt){
    Push(T_CREATE,U(g),Hs(block)); Push(T_CREATE2,U(c),U(names)); Push(T_CREATE3,cnt);
    const int32 gi=int32((uintptr_t(g)-uintptr_t(G<void*>(GROUPS)))/GSZ);
    At<void*>(g,4)=Static(((gi%NGD)+NGD)%NGD,0); At<uint32>(g,8)=std::min<uint32>(cnt,8); }
inline void __fastcall FakeDestroyAssocs(void* g,void*){ Push(T_DESTROY,U(g)); }
inline void __fastcall FakeStart(void* a,void*,float t){ Push(T_START,U(a),F(t)); }
inline void __fastcall FakeSync(void* a,void*,void* o){ Push(T_SYNC,U(a),U(o)); }
inline void __fastcall FakeRefBlock(void* a,void*){ Push(T_REF,U(a)); }
inline void __cdecl FakeUncompress(void* h){ Push(T_UNCOMP,U(h)); }
inline void __cdecl FakeClumpInit(void* c){ Push(T_CLUMPINIT,U(c)); }
inline void* __cdecl FakeFirstAtomic(void* c){ void* r=Bit(1) ? atomic.data() : nullptr; Push(T_FIRSTATOMIC,U(c),U(r)); return r; }
inline void* __cdecl FakeSkin(void* g){ void* r=Bit(2) ? skin.data() : nullptr; Push(T_SKIN,U(g),U(r)); return r; }
inline uintptr_t dllSkinCb{}, dllSkinCbBody{};
inline void* __cdecl FakeForAll(void* c,void* cb,void* d){
    const uintptr_t v=uintptr_t(cb); Push(T_FORALL,U(c),v==dllSkinCb||v==dllSkinCbBody ? 0x734B90u : U(cb)); Push(T_FORALL,U(d)); return c; }
inline int32 __cdecl FakeClumpDestroy(void* c){ Push(T_CLUMPDESTROY,U(c)); return TRUE; }
inline void* __fastcall FakeCreateInstance(void* mi,void*){ void* r=Bit(0)||Bit(0) ? clumpI.data() : nullptr; Push(T_INSTANCE,U(mi),U(r)); return r; }
template<int N> inline void* __fastcall Trap(void* mi,void*){ Push(T_TRAP,U(mi),uint32(N)); return nullptr; }
inline std::array<void*,16> fakeVt{};

// Parches de una ejecucion: direcciones ya resueltas antes de escribir, sin duplicados, nunca sobre un stub,
// en un array fijo (sin reservas) y deshechos en orden inverso.
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,64> P; size_t n{};
    std::array<std::pair<uintptr_t,void*>,64> req{}; size_t nr{};
    void Want(uintptr_t at,void* fake){ if(at&&nr<req.size())req[nr++]={at,fake}; }
    void Apply(){
        for(size_t i=0;i<nr;++i){
            const auto [at,fake]=req[i]; bool skip=false;
            for(size_t j=0;j<i;++j)skip|=req[j].first==at;
            for(size_t j=0;j<nr;++j)skip|=uintptr_t(req[j].second)==at||Body(uintptr_t(req[j].second))==at;
            if(!skip)P[n++].emplace(at,fake);
        } }
    ~Patches(){ while(n)P[--n].reset(); }
};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,31> fns{{
    {"CAnimManager","AddAnimAssocDefinition",0x4D3BA0,0,-1,512},{"CAnimManager","AddAnimBlockRef",0x4D3FB0,0,-1,128},
    {"CAnimManager","AddAnimToAssocDefinition",0x4D3C80,0,-1,256},{"CAnimManager","AddAnimation-hier",0x4D4330,0,-1,512},
    {"CAnimManager","AddAnimation-id",0x4D3AA0,0,-1,512},{"CAnimManager","AddAnimationAndSync",0x4D3B30,0,-1,512},
    {"CAnimManager","BlendAnimation-hier",0x4D4410,0,-1,1024},{"CAnimManager","BlendAnimation-id",0x4D4610,0,-1,1024},
    {"CAnimManager","CreateAnimAssocGroups",0x4D3CC0,0,-1,512},{"CAnimManager","CreateAnimAssociation",0x4D3A40,0,-1,128},
    {"CAnimManager","GetAnimAssociation-by-id",0x4D3A60,0,-1,128},{"CAnimManager","GetAnimAssociation-by-name",0x4D3A80,0,-1,256},
    {"CAnimManager","GetAnimBlockName",0x4D3A30,0,-1,64},{"CAnimManager","GetAnimGroupName",0x4D3A20,0,-1,64},
    {"CAnimManager","GetAnimation-0",0x4D39F0,0,-1,256},{"CAnimManager","GetAnimation-1",0x4D42F0,0,-1,256},
    {"CAnimManager","GetAnimationBlock-",0x4D3940,0,-1,256},{"CAnimManager","GetAnimationBlockIndex-by-name",0x4D3990,0,-1,256},
    {"CAnimManager","GetFirstAssocGroup",0x4D39B0,0,-1,256},{"CAnimManager","GetNumRefsToAnimBlock",0x4D4010,0,-1,128},
    {"CAnimManager","Initialise",0x5BF6B0,0,-1,256},{"CAnimManager","LoadAnimFile",0x4D47F0,0,-1,1024},
    {"CAnimManager","LoadAnimFiles",0x4D5620,0,-1,256},{"CAnimManager","ReadAnimAssociationDefinitions",0x5BC910,0,-1,256},
    {"CAnimManager","RegisterAnimBlock",0x4D3E50,0,-1,256},{"CAnimManager","RemoveAnimBlock",0x4D3F40,0,-1,256},
    {"CAnimManager","RemoveAnimBlockRefWithoutDelete",0x4D3FF0,0,-1,128},{"CAnimManager","RemoveFromUncompressedCache",0x4D42A0,0,-1,256},
    {"CAnimManager","RemoveLastAnimFile",0x4D3ED0,0,-1,256},{"CAnimManager","Shutdown",0x4D4130,0,-1,64},
    {"CAnimManager","UncompressAnimation",0x4D41C0,0,-1,1024},
}};

// --- nombres de los fixtures (todos con terminador dentro de 16 bytes salvo el largo de RegisterAnimBlock) ---
inline const char* const blockNames[]{"ped","PED","gangs","fat","muscular","shop","Bike","colt45","car","weapons","dancing","police","int_house","sunbathe","","verylongblocknm"};
inline const char* const groupNames[]{"man","shuffle","oldman","gang1","fatman","ped","default","bmx","colt45","biked","dance1","police","sexy","woman"};
inline const char* const animNames[]{"walk_civi","run_civi","idle_stance","WALK_CIVI","sprint_civi","fall_back","handsup","Run_Player","dance_loop","shot_partial","KO_skid_front","idle_gang1"};
inline std::array<uint32,std::size(animNames)> animHashes{};

// Normaliza la vtable / callback por defecto de la DLL (como los del EXE) en un bloque de bytes.
inline uint32 dllVt{}, dllDefaultCb{}, dllDefaultCbBody{};
inline uint64 NormHash(const uint8* p,size_t n){
    std::memcpy(normBuf.data(),p,n);
    for(size_t k=0;k+4<=n;k+=4){ uint32 w; std::memcpy(&w,normBuf.data()+k,4);
        if(w==dllVt)w=0x85C6D0; else if(w==dllDefaultCb||w==dllDefaultCbBody)w=0x4D1390; std::memcpy(normBuf.data()+k,&w,4); }
    return Fnv(normBuf.data(),n); }
inline json NormBytes(const uint8* p,size_t n){
    auto v=ReversingProbe::Bytes(p,n);
    for(size_t k=0;k+4<=n;k+=4){ uint32 w; std::memcpy(&w,v.data()+k,4);
        if(w==dllVt)w=0x85C6D0; else if(w==dllDefaultCb||w==dllDefaultCbBody)w=0x4D1390; std::memcpy(v.data()+k,&w,4); }
    return v; }
// Definiciones: el puntero AnimDescr de la DLL (copia estatica de aStdAnimDescs) se lee como 0x8A7788.
inline uint64 DefsHash(){
    std::memcpy(normBuf.data(),Def(0),NDEF*DEFSZ);
    for(size_t d=0;d<NDEF;++d){ auto& p=At<uint8*>(normBuf.data(),d*DEFSZ+0x2C);
        if(p&&!In(p,descrs.data(),descrs.size())&&uintptr_t(p)!=STDDESCS&&!IsBadReadPtr(p,16)&&std::memcmp(p,reinterpret_cast<void*>(STDDESCS),16)==0)p=reinterpret_cast<uint8*>(STDDESCS); }
    return Fnv(normBuf.data(),NDEF*DEFSZ); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"animmgr31-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"animmgr31-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("animmgr31");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"animmgr31-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,31> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,31> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    using std::bit_cast;
    // vtable de la DLL: constructor por defecto de la DLL sobre un bufer (no reserva ni llama a nada).
    { alignas(16) static uint8 tmp[0x40]{}; new (tmp) CAnimBlendAssociation(); std::memcpy(&dllVt,tmp,4); }
    dllDefaultCb=bit_cast<uint32>(&CDefaultAnimCallback::DefaultAnimCB); dllDefaultCbBody=uint32(Body(dllDefaultCb));
    dllSkinCb=bit_cast<uintptr_t>(&AtomicRemoveAnimFromSkinCB); dllSkinCbBody=Body(dllSkinCb);
    for(size_t k=0;k<std::size(animNames);++k)animHashes[k]=CKeyGen::GetUppercaseKey(animNames[k]);
    fakeVt={reinterpret_cast<void*>(&Trap<0>),reinterpret_cast<void*>(&Trap<1>),reinterpret_cast<void*>(&Trap<2>),reinterpret_cast<void*>(&Trap<3>),
            reinterpret_cast<void*>(&Trap<4>),reinterpret_cast<void*>(&Trap<5>),reinterpret_cast<void*>(&Trap<6>),reinterpret_cast<void*>(&Trap<7>),
            reinterpret_cast<void*>(&Trap<8>),reinterpret_cast<void*>(&Trap<9>),reinterpret_cast<void*>(&Trap<10>),reinterpret_cast<void*>(&FakeCreateInstance),
            reinterpret_cast<void*>(&Trap<12>),reinterpret_cast<void*>(&Trap<13>),reinterpret_cast<void*>(&Trap<14>),reinterpret_cast<void*>(&Trap<15>)};
    fakeMi.fill(0); At<void*>(fakeMi.data(),0)=fakeVt.data();
    atomic.fill(0); At<void*>(atomic.data(),0x18)=geometry.data();
    const uint32 clumpOffset=G<uint32>(0xB5F878);
    if(clumpOffset+4>clump.size()||clumpOffset<4){Write({{"event","error"},{"reason","clump plugin offset out of fixture"},{"offset",clumpOffset}});return;}

    // Cuerpos de la DLL resueltos antes de escribir ningun parche.
    struct Stub { uintptr_t exe, dll; void* fake; };
    const auto B=[](uintptr_t p){ return p ? Body(p) : uintptr_t(0); };
    const std::array<Stub,33> stubs{{
        {0x82119A,0,reinterpret_cast<void*>(&FakeNew)},{0x8214BD,0,reinterpret_cast<void*>(&FakeDelete)},
        {0x72F420,B(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc))),reinterpret_cast<void*>(&FakeMalloc)},
        {0x72F4C0,0,reinterpret_cast<void*>(&FakeMallocAlign)},{0x72F4F0,0,reinterpret_cast<void*>(&FakeFreeAlign)},
        {0x7EC9D0,0,reinterpret_cast<void*>(&FakeStreamRead)},{0x7ECEF0,0,reinterpret_cast<void*>(&FakeStreamOpen)},{0x7ECE20,0,reinterpret_cast<void*>(&FakeStreamClose)},
        {0x5387D0,B(bit_cast<uintptr_t>(&CFileMgr::SetDir)),reinterpret_cast<void*>(&FakeSetDir)},
        {0x538900,B(bit_cast<uintptr_t>(&CFileMgr::OpenFile)),reinterpret_cast<void*>(&FakeOpenFile)},
        {0x5389D0,B(bit_cast<uintptr_t>(&CFileMgr::CloseFile)),reinterpret_cast<void*>(&FakeCloseFile)},
        {0x536F80,B(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine))),reinterpret_cast<void*>(&FakeLoadLine)},
        {0x4089A0,B(bit_cast<uintptr_t>(&CStreaming::RemoveModel)),reinterpret_cast<void*>(&FakeRemoveModel)},
        {0x4CF980,B(bit_cast<uintptr_t>(&CAnimBlendHierarchy::Shutdown)),reinterpret_cast<void*>(&FakeHierShutdown)},
        {0x4CF3E0,B(bit_cast<uintptr_t>(&CAnimBlendHierarchy::CalcTotalTimeCompressed)),reinterpret_cast<void*>(&FakeHierTotalC)},
        {0x4CF760,B(bit_cast<uintptr_t>(&CAnimBlendHierarchy::RemoveUncompressedData)),reinterpret_cast<void*>(&FakeHierRemoveU)},
        {0x4CF5F0,B(bit_cast<uintptr_t>(&CAnimBlendHierarchy::Uncompress)),reinterpret_cast<void*>(&FakeHierUncomp)},
        {0x4CF4E0,B(bit_cast<uintptr_t>(&CAnimBlendHierarchy::RemoveQuaternionFlips)),reinterpret_cast<void*>(&FakeHierFlips)},
        {0x4CF2F0,B(bit_cast<uintptr_t>(&CAnimBlendHierarchy::CalcTotalTime)),reinterpret_cast<void*>(&FakeHierTotal)},
        {0x4CE130,B(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(AnimationId)>(&CAnimBlendAssocGroup::CopyAnimation))),reinterpret_cast<void*>(&FakeCopyAnim)},
        {0x4CE6E0,B(bit_cast<uintptr_t>(static_cast<void(CAnimBlendAssocGroup::*)(const char*,RpClump*,const char**,uint32)>(&CAnimBlendAssocGroup::CreateAssociations))),reinterpret_cast<void*>(&FakeCreateAssocs)},
        {0x4CDFF0,B(bit_cast<uintptr_t>(&CAnimBlendAssocGroup::DestroyAssociations)),reinterpret_cast<void*>(&FakeDestroyAssocs)},
        {0x4CEB70,B(bit_cast<uintptr_t>(&CAnimBlendAssociation::Start)),reinterpret_cast<void*>(&FakeStart)},
        {0x4CEB40,B(bit_cast<uintptr_t>(&CAnimBlendAssociation::SyncAnimation)),reinterpret_cast<void*>(&FakeSync)},
        {0x4CEA50,B(bit_cast<uintptr_t>(&CAnimBlendAssociation::ReferenceAnimBlock)),reinterpret_cast<void*>(&FakeRefBlock)},
        {0x4D6720,B(bit_cast<uintptr_t>(&RpAnimBlendClumpInit)),reinterpret_cast<void*>(&FakeClumpInit)},
        {0x734820,B(bit_cast<uintptr_t>(&GetFirstAtomic)),reinterpret_cast<void*>(&FakeFirstAtomic)},
        {0x7C7550,0,reinterpret_cast<void*>(&FakeSkin)},{0x749B70,0,reinterpret_cast<void*>(&FakeForAll)},{0x74A310,0,reinterpret_cast<void*>(&FakeClumpDestroy)},
        {0,0,nullptr},{0,0,nullptr},{0,0,nullptr},
    }};
    // UncompressAnimation se simula solo cuando no es la funcion probada.
    const Stub uncompStub{0x4D41C0,B(bit_cast<uintptr_t>(&CAnimManager::UncompressAnimation)),reinterpret_cast<void*>(&FakeUncompress)};
    std::vector<uintptr_t> patchedExe; for(auto& s:stubs)if(s.exe)patchedExe.push_back(s.exe); patchedExe.push_back(uncompStub.exe);
    std::vector<std::array<uint8,5>> savedEntries(patchedExe.size());
    for(size_t k=0;k<patchedExe.size();++k)std::memcpy(savedEntries[k].data(),reinterpret_cast<void*>(patchedExe[k]),5);

    // estado real guardado (globales del EXE que se rellenan con fixtures)
    struct Blob { uintptr_t p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{DEFS,NDEF*DEFSZ},{NUMDEFS,16},{uintptr_t(Hier(ABASE)),size_t(ACOUNT)*HSZ},{BLOCKS,NBLK*BLKSZ},{CACHE,CACHESZ},{MIPTR,4}};
    for(auto& b:blobs)b.saved.assign(reinterpret_cast<uint8*>(b.p),reinterpret_cast<uint8*>(b.p)+b.n);
    auto restoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.p),b.saved.data(),b.n); };

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto Rnd=[&](uint8* p,size_t n){ for(size_t k=0;k<n;++k)p[k]=uint8(next()); };
            auto Name16=[&](uint8* dst,const char* s){ std::memset(dst,0,16); std::memcpy(dst,s,std::min<size_t>(std::strlen(s),15)); };
            constexpr float kQNaN=std::numeric_limits<float>::quiet_NaN();
            int32 nBlocks{}, nDefs{};
            auto pickHier=[&]{ return Hier(ABASE+int32(next()%ACOUNT)); };

            // ---- mundo de fixtures ----
            auto buildWorld=[&]{
                nBlocks=2+int32(next()%20); nDefs=1+int32(next()%30);
                for(size_t k=0;k<NBLK;++k){ uint8* b=Blk(k); std::memset(b,0,BLKSZ);
                    Name16(b,blockNames[next()%std::size(blockNames)]); b[0x10]=uint8(next()%2);
                    At<int16>(b,0x12)=int16(int32(next()%7)-2); At<int32>(b,0x14)=ABASE+int32(next()%(ACOUNT-8));
                    At<uint32>(b,0x18)=next()%5==0 ? 0u : 1u+next()%8; At<int32>(b,0x1C)=int32(next()%120)-1; }
                for(int32 h=0;h<ACOUNT;++h){ uint8* p=Hier(ABASE+h); Rnd(p,HSZ);
                    At<uint32>(p,0)=next()%4 ? animHashes[next()%animHashes.size()] : next(); At<void*>(p,4)=nullptr; At<uint16>(p,8)=0;
                    p[0xA]=uint8(next()%2); p[0xB]=uint8(next()%2); At<int32>(p,0xC)=int32(next()%4);
                    At<float>(p,0x10)=next()%4==0 ? 0.f : uni(0.1f,3.f); At<void*>(p,0x14)=nullptr; }
                for(size_t d=0;d<NDEF;++d){ uint8* p=Def(d); std::memset(p,0,DEFSZ);
                    const bool used=int32(d)<nDefs||(d>=118&&d<118+next()%4);
                    if(used)Name16(p,groupNames[next()%std::size(groupNames)]);
                    Name16(p+0x10,blockNames[next()%std::size(blockNames)]);
                    At<int32>(p,0x20)=next()%3==0 ? 7 : -1; At<int32>(p,0x24)=1+int32(next()%8);
                    At<void*>(p,0x28)=namePtrs.data()+(d%NGD)*8*4; At<void*>(p,0x2C)=descrs.data()+(d%NGD)*8*8; }
                for(int32 d=0;d<NGD;++d){ const int32 cnt=At<int32>(Def(size_t(d)),0x24), base=d*16+int32(next()%8), rot=int32(next()%8);
                    for(int32 j=0;j<8;++j){ uint8* e=descrs.data()+(size_t(d)*8+j)*8;
                        At<int32>(e,0)=j==0||cnt<2 ? base : base+1+(j-1+rot)%(cnt-1); At<int32>(e,4)=int32(next()&0xFFFF); }
                    const int32 filled=int32(next()%8);
                    for(int32 j=0;j<8;++j){ char* nb=reinterpret_cast<char*>(nameBufs.data()+(size_t(d)*8+j)*24); std::memset(nb,0,24);
                        if(j<filled)std::strcpy(nb,animNames[next()%std::size(animNames)]); else nb[1]=char('a'+next()%26);
                        At<char*>(namePtrs.data(),(size_t(d)*8+j)*4)=nb; } }
                G<uint32>(NUMDEFS)=next()%3==0 ? next()%uint32(nDefs) : uint32(nDefs);
                G<int32>(NUMANIMS)=ABASE+int32(next()%(ACOUNT-8)); G<uint32>(NUMBLOCKS)=uint32(nBlocks);
                At<uint32>(groupsMem.data(),0)=uint32(nDefs);
                for(int32 g=0;g<int32(NDEF);++g){ uint8* p=Group(g); Rnd(p,GSZ);
                    At<void*>(p,0)=next()%3==0 ? Blk(next()%uint32(nBlocks)) : next()%2 ? nullptr : Blk(next()%NBLK);
                    At<void*>(p,4)=g<NGD&&next()%4 ? Static(g,0) : nullptr; At<uint32>(p,8)=1+next()%8;
                    At<int32>(p,0xC)=At<int32>(descrs.data(),size_t(g%NGD)*8*8); At<int32>(p,0x10)=g; }
                for(int32 g=0;g<NGD;++g)for(int32 s=0;s<8;++s){ uint8* p=Static(g,s); Rnd(p,SSZ);
                    At<int16>(p,6)=int16(At<int32>(Group(g),0xC)+s); At<int16>(p,8)=int16(g);
                    At<uint16>(p,0xA)=uint16(next()); At<void*>(p,0x10)=pickHier(); }
                G<void*>(GROUPS)=Group(0);
                G<void*>(MIPTR)=fakeMi.data();
            };
            // Clump con lista de asociaciones (cuenta de frames pequena para el constructor real).
            int32 nList{};
            auto buildClump=[&]{
                Rnd(clump.data(),clump.size()); clump[0]=2; At<void*>(clump.data(),clumpOffset)=cdata.data();
                Rnd(cdata.data(),cdata.size()); Rnd(frameDatas.data(),frameDatas.size());
                At<uint32>(cdata.data(),8)=next()%4; At<void*>(cdata.data(),0x10)=frameDatas.data();
                nList=int32(next()%7); At<void*>(cdata.data(),4)=nullptr;
                At<void*>(cdata.data(),0)=nList ? Assoc(0)+4 : nullptr;
                for(int32 k=0;k<8;++k){ uint8* a=Assoc(k); Rnd(a,0x40);
                    At<void*>(a,4)=k+1<nList ? Assoc(k+1)+4 : nullptr; At<void*>(a,8)=k ? Assoc(k-1)+4 : cdata.data();
                    At<void*>(a,0x14)=pickHier(); At<uint16>(a,0x2E)=uint16(next()); }
                Rnd(na.data(),na.size()); At<void*>(na.data(),0x14)=pickHier(); At<uint16>(na.data(),0x2E)=uint16(next());
                Rnd(extra.data(),extra.size()); };
            // Ajusta los blends de la lista (NaN silencioso, 0, negativos, empates exactos de `bd < delta`).
            auto blendFields=[&](float delta){
                for(int32 k=0;k<nList;++k){ uint8* a=Assoc(k);
                    const uint32 r=next()%10; const float amount=r==0 ? 0.f : r==1 ? -uni(0.f,1.f) : r==2 ? kQNaN : r==3 ? 1.f : uni(0.01f,1.f);
                    At<float>(a,0x18)=amount;
                    const uint32 q=next()%6; At<float>(a,0x1C)=q==0 ? -(amount*delta) : q==1 ? -(delta*amount) : q==2 ? kQNaN : q==3 ? -0.05f : uni(-8.f,8.f);
                    At<float>(a,0x20)=next()%3==0 ? At<float>(At<uint8*>(a,0x14),0x10) : uni(0.f,3.f); } };
            // Cache LRU falsa sobre la estructura real (centinelas en 0xB5EB20): 1-8 enlaces, usados y libres.
            auto buildCache=[&]{
                Rnd(cacheLinks.data(),cacheLinks.size()); Rnd(cacheHiers.data(),cacheHiers.size());
                for(int32 h=0;h<10;++h){ uint8* p=cacheHiers.data()+size_t(h)*HSZ; p[0xA]=uint8(next()%2); p[0xB]=uint8(next()%4==0);
                    At<float>(p,0x10)=next()%3==0 ? 0.f : uni(0.1f,2.f); At<void*>(p,0x14)=nullptr; }
                const int32 nLinks=1+int32(next()%8); const int32 nUsed=next()%3==0 ? nLinks : int32(next()%uint32(nLinks+1));
                std::array<int32,8> perm{0,1,2,3,4,5,6,7}; for(int32 k=7;k>0;--k)std::swap(perm[k],perm[next()%uint32(k+1)]);
                uint8* c=reinterpret_cast<uint8*>(CACHE); std::memset(c,0,CACHESZ);
                auto L=[&](int32 k){ return cacheLinks.data()+size_t(perm[k])*12; };
                auto chain=[&](uint8* head,uint8* tail,int32 from,int32 to){
                    uint8* prev=head;
                    for(int32 k=from;k<to;++k){ At<uint8*>(prev,8)=L(k); At<uint8*>(L(k),4)=prev; prev=L(k); }
                    At<uint8*>(prev,8)=tail; At<uint8*>(tail,4)=prev; };
                chain(c,c+0xC,0,nUsed); chain(c+0x18,c+0x24,nUsed,nLinks);
                At<void*>(c,0)=cacheHiers.data()+9*HSZ; At<void*>(c,0x30)=cacheLinks.data();
                for(int32 k=0;k<nLinks;++k){ uint8* h=cacheHiers.data()+size_t(perm[k])*HSZ; At<uint8*>(L(k),0)=h;
                    if(k<nUsed&&next()%8)At<uint8*>(h,0x14)=L(k); } };

            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x31A3u; if(!stt)stt=1; nrec=0;
                for(int k=0;k<4;++k){ masks[k]=next(); calls[k]=0; }
                arena.fill(0xCD); arenaTop=0x40; ifpSize=ifpPos=0; nLines=lineIdx=0;
                buildWorld(); buildClump(); buildCache();

                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                const uintptr_t a=fn.address;
                json out={{"switches",switches}}; json ret=nullptr;
                auto call=[&](auto&& body){
                    Patches p;
                    for(auto& s:stubs){ if(!s.exe)continue; p.Want(s.exe,s.fake); p.Want(s.dll,s.fake); }
                    if(nm!="UncompressAnimation"){ p.Want(uncompStub.exe,uncompStub.fake); p.Want(uncompStub.dll,uncompStub.fake); }
                    p.Apply(); body(); };
                auto anyName=[&]{ const uint32 r=next()%4; return r==0 ? groupNames[next()%std::size(groupNames)] : blockNames[next()%std::size(blockNames)]; };

                if(nm=="GetAnimationBlock-"){ const char* s=anyName(); CAnimBlock* r{}; call([&]{ r=reinterpret_cast<CAnimBlock*(__cdecl*)(const char*)>(a)(s); }); ret=U(r); }
                else if(nm=="GetAnimationBlockIndex-by-name"){ const char* s=anyName(); int32 r{}; call([&]{ r=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(s); }); ret=r; }
                else if(nm=="GetFirstAssocGroup"){ const char* s=anyName(); int32 r{}; call([&]{ r=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(s); }); ret=r; }
                else if(nm=="GetAnimation-0"||nm=="GetAnimation-1"){
                    uint8* b=Blk(next()%NBLK); if(next()%8==0)At<int32>(b,0x18)=-int32(next()%3);
                    void* r{};
                    if(nm=="GetAnimation-0"){ const uint32 h=next()%4 ? animHashes[next()%animHashes.size()] : next(); out["hash"]=h;
                        call([&]{ r=reinterpret_cast<void*(__cdecl*)(uint32,const void*)>(a)(h,b); }); }
                    else { const char* s=animNames[next()%std::size(animNames)]; call([&]{ r=reinterpret_cast<void*(__cdecl*)(const char*,const void*)>(a)(s,b); }); }
                    ret=U(r); }
                else if(nm=="GetAnimGroupName"||nm=="GetAnimBlockName"){ const int32 g=int32(next()%NDEF); const char* r{}; call([&]{ r=reinterpret_cast<const char*(__cdecl*)(int32)>(a)(g); }); ret=U(r); }
                else if(nm=="CreateAnimAssociation"||nm=="GetAnimAssociation-by-id"){
                    const int32 g=int32(next()%NGD), id=At<int32>(Group(g),0xC)+int32(next()%8);
                    if(nm=="GetAnimAssociation-by-id")At<void*>(Group(g),4)=Static(g,0);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(int32,int32)>(a)(g,id); }); ret=U(r); }
                else if(nm=="GetAnimAssociation-by-name"){
                    const int32 g=int32(next()%NGD); At<void*>(Group(g),4)=Static(g,0); if(next()%8==0)At<int32>(Group(g),8)=0;
                    const char* s=animNames[next()%std::size(animNames)];
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(int32,const char*)>(a)(g,s); }); ret=U(r); }
                else if(nm=="AddAnimation-id"||nm=="AddAnimationAndSync"){
                    const int32 g=int32(next()%NGD), id=int32(next()%300);
                    void* r{};
                    if(nm=="AddAnimation-id")call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(clump.data(),g,id); });
                    else { const uint32 w=next()%4; void* sw=w==0 ? nullptr : w==1 ? static_cast<void*>(extra.data()) : nList ? static_cast<void*>(Assoc(int32(next()%uint32(nList)))) : nullptr;
                        call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,void*,int32,int32)>(a)(clump.data(),sw,g,id); }); }
                    ret=U(r); }
                else if(nm=="AddAnimation-hier"){
                    uint8* h=pickHier(); const int32 flags=next()%4==0 ? int32(next()) : int32(next()&0xFFFF);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,void*,int32)>(a)(clump.data(),h,flags); }); ret=U(r);
                    if(r)out["new"]=NormBytes(static_cast<uint8*>(r),ASZ); }
                else if(nm=="BlendAnimation-hier"){
                    uint8* h=pickHier(); const int32 flags=int32(next()&0xFFFF)|(next()%2 ? 0x10 : 0);
                    const float delta=next()%8==0 ? 0.f : next()%4==0 ? 1000.f : uni(0.5f,16.f);
                    for(int32 k=0;k<nList;++k){ uint8* as=Assoc(k); const uint32 r=next()%5;
                        if(r==0)At<uint8*>(as,0x14)=h; else if(r==1){ uint8* o=pickHier(); At<int32>(o,0xC)=At<int32>(h,0xC); At<uint8*>(as,0x14)=o; } }
                    blendFields(delta);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,void*,int32,float)>(a)(clump.data(),h,flags,delta); }); ret=U(r);
                    if(r&&In(r,arena.data(),arena.size()))out["new"]=NormBytes(static_cast<uint8*>(r),ASZ); }
                else if(nm=="BlendAnimation-id"){
                    const int32 g=int32(next()%NGD), id=At<int32>(Group(g),0xC)+int32(next()%8);
                    At<void*>(Group(g),4)=Static(g,0);
                    const float delta=next()%8==0 ? 0.f : next()%4==0 ? 1000.f : uni(0.5f,16.f);
                    for(int32 k=0;k<nList;++k){ uint8* as=Assoc(k);
                        if(next()%4==0){ At<int16>(as,0x2C)=int16(id); At<int16>(as,0xE)=int16(next()%4 ? g : g+1); }
                        else { At<int16>(as,0x2C)=int16(next()%4==0 ? id : int32(next()%300)); At<int16>(as,0xE)=int16(next()%NGD); } }
                    blendFields(delta);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32,int32,float)>(a)(clump.data(),g,id,delta); }); ret=U(r); }
                else if(nm=="AddAnimAssocDefinition"){
                    const char* gname=groupNames[next()%std::size(groupNames)]; const char* bname=blockNames[next()%std::size(blockNames)];
                    const uint32 mi=next()%3==0 ? 7u : next()%2 ? 0xFFFFFFFFu : next()%300; const uint32 cnt=next()%9;
                    void* descr=descrs.data()+size_t(next()%NGD)*64;
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(const char*,const char*,uint32,uint32,void*)>(a)(gname,bname,mi,cnt,descr); }); ret=U(r); }
                else if(nm=="AddAnimToAssocDefinition"){
                    uint8* d=Def(next()%NGD); const char* s=animNames[next()%std::size(animNames)];
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,const char*)>(a)(d,s); }); }
                else if(nm=="CreateAnimAssocGroups"){ call([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }); }
                else if(nm=="RegisterAnimBlock"){
                    const char* s=next()%6==0 ? "averyveryverylongblockname" : anyName();
                    int32 r{}; call([&]{ r=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(s); }); ret=r; }
                else if(nm=="RemoveLastAnimFile"){ if(next()%8==0)At<uint32>(Blk(uint32(nBlocks-1)),0x18)=uint32(-int32(next()%3)); call([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }); }
                else if(nm=="RemoveAnimBlock"){
                    const int32 k=int32(next()%uint32(nBlocks)); if(next()%8==0)At<uint32>(Blk(uint32(k)),0x18)=uint32(-int32(next()%3));
                    for(int32 g=0;g<int32(NDEF);++g)if(next()%4==0)At<void*>(Group(g),0)=Blk(uint32(k));
                    call([&]{ reinterpret_cast<void(__cdecl*)(int32)>(a)(k); }); }
                else if(nm=="AddAnimBlockRef"||nm=="RemoveAnimBlockRefWithoutDelete"||nm=="GetNumRefsToAnimBlock"){
                    const int32 k=int32(next()%NBLK); const int16 rc[]{0,1,-1,0x7FFF,int16(0x8000),5,-3};
                    At<int16>(Blk(uint32(k)),0x12)=rc[next()%std::size(rc)];
                    if(nm=="GetNumRefsToAnimBlock"){ int32 r{}; call([&]{ r=reinterpret_cast<int32(__cdecl*)(int32)>(a)(k); }); ret=r; }
                    else call([&]{ reinterpret_cast<void(__cdecl*)(int32)>(a)(k); }); }
                else if(nm=="UncompressAnimation"||nm=="RemoveFromUncompressedCache"){
                    uint8* h=cacheHiers.data()+size_t(next()%10)*HSZ;
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(h); }); }
                else if(nm=="Shutdown"){
                    G<int32>(NUMANIMS)=int32(next()%40);
                    if(next()%8==0)At<void*>(reinterpret_cast<void*>(CACHE),0x30)=nullptr;
                    if(next()%8==0)G<void*>(GROUPS)=nullptr;
                    call([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }); }
                else if(nm=="Initialise"||nm=="ReadAnimAssociationDefinitions"){
                    // ANIMGRP.DAT falso: cabeceras "%s %s %s %d" y nombres hasta "end" (nunca mas que el contador)
                    const int32 nHeaders=1+int32(next()%3);
                    auto addLine=[&](const char* s){ if(nLines<lines.size()){ std::memset(lines[nLines].data(),0,48); std::strncpy(lines[nLines].data(),s,47); ++nLines; } };
                    for(int32 hd=0;hd<nHeaders;++hd){
                        if(next()%3==0)addLine(next()%2 ? "# comentario" : "");
                        const int32 c=1+int32(next()%5); char buf[48];
                        std::snprintf(buf,sizeof(buf),"%s %s %s %d",groupNames[next()%std::size(groupNames)],blockNames[next()%(std::size(blockNames)-2)],next()%2 ? "PED" : "ped",c);
                        addLine(buf);
                        const int32 k=int32(next()%uint32(c+1));
                        for(int32 j=0;j<k;++j){ if(next()%5==0)addLine(next()%2 ? "#x" : "   "); addLine(animNames[next()%std::size(animNames)]); }
                        addLine(next()%4==0 ? "end   " : "end");
                    }
                    call([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); }); }
                else if(nm=="LoadAnimFile"||nm=="LoadAnimFiles"){
                    // Fichero IFP falso: ANP3, ANP2, ANPK o cabecera desconocida (se lee como ANPK).
                    auto W32=[&](uint32 v){ if(ifpSize+4<=ifp.size()){ std::memcpy(ifp.data()+ifpSize,&v,4); ifpSize+=4; } };
                    auto WF=[&](float v){ W32(F(v)); };
                    auto WS=[&](const char* s,size_t n){ for(size_t k=0;k<n;++k)if(ifpSize<ifp.size())ifp[ifpSize++]=uint8(k<std::strlen(s) ? s[k] : 0); };
                    auto WR=[&](size_t n){ for(size_t k=0;k<n;++k)if(ifpSize<ifp.size())ifp[ifpSize++]=uint8(next()); };
                    auto W4=[&](const char* id){ WS(id,4); };
                    const uint32 kind=next()%4; const char* ids[]{"ANP3","ANP2","ANPK","XXXX"};
                    W4(ids[kind]); W32(next()%4096);
                    const char* bname=next()%3 ? blockNames[next()%(std::size(blockNames)-2)] : "newblock";
                    int32 found=-1; for(int32 k=0;k<nBlocks;++k)if(!_stricmp(reinterpret_cast<const char*>(Blk(uint32(k))),bname)){found=k;break;}
                    const int32 fileAnims=int32(next()%6);
                    const int32 loops=kind<2 ? fileAnims : found>=0&&At<uint32>(Blk(uint32(found)),0x18) ? int32(At<uint32>(Blk(uint32(found)),0x18)) : fileAnims;
                    const bool compress=next()%2!=0; out["compress"]=compress;
                    const char (*list)[32]=nullptr;
                    if(kind>=2&&next()%2){ uncomp.fill(0); const int32 n=int32(next()%4);
                        for(int32 k=0;k<n;++k)std::strcpy(uncomp.data()+k*32,animNames[next()%std::size(animNames)]);
                        list=reinterpret_cast<const char(*)[32]>(uncomp.data()); }
                    if(kind<2){
                        WS(bname,24); W32(uint32(fileAnims));
                        for(int32 an=0;an<loops;++an){
                            WS(animNames[next()%std::size(animNames)],24); const int32 nSeq=int32(next()%5); W32(uint32(nSeq));
                            struct S { int32 type, frames; } seqs[5]{}; uint32 total=0;
                            for(int32 s=0;s<nSeq;++s){ seqs[s].type=next()%10==0 ? int32(next()%2 ? 0 : 5) : 1+int32(next()%4); seqs[s].frames=int32(next()%6);
                                const uint32 sz[]{0,20,32,10,16,0}; total+=sz[seqs[s].type]*uint32(seqs[s].frames); }
                            if(kind==0){ W32(total+(next()%2 ? 0 : 16)); W32(next()%4); }
                            for(int32 s=0;s<nSeq;++s){
                                WS(next()%3 ? "Bip01 Spine" : "root",24); W32(uint32(seqs[s].type)); W32(uint32(seqs[s].frames));
                                const uint32 bt=next()%3; W32(bt==0 ? 0xFFFFFFFFu : bt==1 ? next()%60 : 0x10000u|next()%8);
                                const uint32 sz[]{0,20,32,10,16,0}; WR(sz[seqs[s].type]*uint32(seqs[s].frames)); }
                        }
                    } else {
                        const uint32 infoSz=next()%2 ? 0x14 : 0x1B; W4("INFO"); W32(infoSz); W32(uint32(fileAnims)); WS(bname,((infoSz+3)&~3u)-4);
                        for(int32 an=0;an<loops;++an){
                            const char* an_=animNames[next()%std::size(animNames)]; const uint32 nsz=uint32(std::strlen(an_)+1);
                            W4("NAME"); W32(nsz); WS(an_,(nsz+3)&~3u);
                            W4("DGAN"); W32(next()%512);
                            const int32 nSeq=int32(next()%4); W4("INFO"); W32(8); W32(uint32(nSeq)); W32(next());
                            for(int32 s=0;s<nSeq;++s){
                                W4("CPAN"); W32(next()%256);
                                const uint32 asz=next()%3==0 ? 0x28 : next()%2 ? 0x2C : 0x29; const int32 nf=next()%5==0 ? 0 : 1+int32(next()%5);
                                W4("ANIM"); W32(asz); WS(next()%2 ? "Bip01 L Hand" : "root",28); W32(uint32(nf)); W32(next()); W32(next());
                                if(((asz+3)&~3u)==0x2C)W32(next()%3==0 ? 0xFFFFFFFFu : next()%60);
                                if(!nf)continue;
                                const uint32 kt=next()%8; const char* kid=kt<3 ? "KRTS" : kt<5 ? "KRT0" : kt<7 ? "KR00" : "KRXX";
                                W4(kid); W32(next()%1024);
                                if(kt==7)continue;
                                for(int32 k=0;k<nf;++k){
                                    for(int q=0;q<4;++q)WF(next()%16==0 ? 0.f : uni(-1.f,1.f));
                                    if(kt<5){ for(int q=0;q<3;++q)WF(next()%16==0 ? uni(-40.f,40.f) : uni(-5.f,5.f)); }
                                    if(kt<3){ for(int q=0;q<3;++q)WF(uni(0.5f,1.5f)); }
                                    WF(next()%8==0 ? 0.f : uni(0.f,4.f));
                                }
                            }
                        }
                    }
                    if(nm=="LoadAnimFile")call([&]{ reinterpret_cast<void(__cdecl*)(void*,bool,const char(*)[32])>(a)(stream.data(),compress,list); });
                    else call([&]{ reinterpret_cast<void(__cdecl*)()>(a)(); });
                    out["pos"]=ifpPos; out["size"]=ifpSize;
                }

                for(auto* h:hooks)h->State(false);
                out["globals"]={G<uint32>(NUMDEFS),G<int32>(NUMANIMS),G<uint32>(NUMBLOCKS),U(G<void*>(GROUPS)),U(G<void*>(MIPTR))};
                out["defs"]=DefsHash(); out["blocks"]=Fnv(Blk(0),NBLK*BLKSZ); out["anims"]=Fnv(Hier(ABASE),size_t(ACOUNT)*HSZ);
                out["cache"]=ReversingProbe::Bytes(reinterpret_cast<void*>(CACHE),CACHESZ);
                out["cacheLinks"]=Fnv(cacheLinks.data(),cacheLinks.size()); out["cacheHiers"]=Fnv(cacheHiers.data(),cacheHiers.size());
                out["groups"]=Fnv(groupsMem.data(),groupsMem.size()); out["statics"]=Fnv(statics.data(),statics.size());
                out["names"]=Fnv(nameBufs.data(),nameBufs.size())^Fnv(namePtrs.data(),namePtrs.size());
                out["assocs"]=Fnv(assocs.data(),assocs.size()); out["na"]=ReversingProbe::Bytes(na.data(),ASZ); out["extra"]=Fnv(extra.data(),extra.size());
                out["cdata"]=ReversingProbe::Bytes(cdata.data(),cdata.size()); out["clump"]=Fnv(clump.data(),clump.size());
                out["arena"]=NormHash(arena.data(),arena.size()); out["arenaTop"]=arenaTop; out["lineIdx"]=lineIdx;
                out["ret"]=ret; out["recs"]=Recs();
                restoreAll();
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreAll();
    bool restored=true;
    for(size_t k=0;k<patchedExe.size();++k)restored&=std::memcmp(savedEntries[k].data(),reinterpret_cast<void*>(patchedExe[k]),5)==0;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f];
    for(auto& b:blobs)restored&=std::memcmp(reinterpret_cast<void*>(b.p),b.saved.data(),b.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
