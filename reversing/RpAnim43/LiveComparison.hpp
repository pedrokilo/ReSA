#pragma once
// Banco dentro del motor: plugin RpAnimBlend (lista de 43; las 42 funciones pendientes de RpAnimBlend).
// Original (hooks de la clase desactivados, codigo del EXE) frente a candidato con fixtures deterministas por caso.
// Nunca se toca un clump real: clump (byte de tipo 2 y puntero del plugin en el offset real de 0xB5F878),
// CAnimBlendClumpData, asociaciones (vtable falsa: el `delete` se traza y desenlaza sin liberar), jerarquias,
// frame datas, RwFrames falsos (nombre en el offset real del plugin NodeName, hijos/hermanos enlazados a mano),
// keyframes de interpolacion, nodos, secuencias y keyframes, jerarquia HAnim, interpolador, atomic/skin y arena
// son buferes estaticos del banco: los punteros se comparan en crudo. Los globales 0xB5F878 (offset del plugin),
// 0xB5F87C (frame encontrado) y 0xB4EA0C (gpAnimBlendClump) se guardan y restauran en cada ejecucion; el offset
// del plugin se restaura justo despues de la llamada a RpAnimBlendPluginAttach.
// Se simulan y trazan sin reservar memoria (solo durante la llamada y solo en el hilo principal; arena rellena con
// 0xCD): operator new/delete, CMemoryMgr::MallocAlign/FreeAlign, GetFirstAtomic, RpSkinGeometryGetSkin,
// GetAnimHierarchyFromSkinClump, SkinGetBonePositionsToTable, CVisibilityPlugins::GetFrameHierarchyId,
// CAnimBlendAssociation::Start, RtAnimAnimationCreate, RpClumpRegisterPlugin y RtAnimRegisterInterpolationScheme
// (los punteros a callbacks de la DLL se leen como las direcciones del EXE). Corren de verdad (ya validadas):
// CAnimBlendClumpData (constructor, destructor, SetNumberOfBones, ForAllFrames), CAnimBlendNode::Update/
// UpdateCompressed/CalcDeltas, GetFrameNodeName/SetFrameNodeName, CKeyGen::GetUppercaseKey, RpHAnimIDGetIndex,
// RwFrameForAllChildren, RwMatrixUpdate y las extracciones de velocidad del EXE (0x4D1680/0x4D1A50/0x4D1DB0/
// 0x4D2100/0x4D2450/0x4D27F0), a las que llaman las dos versiones.
#include <fstream>
#include <optional>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MemoryMgr.h"
#include "AnimBlendAssociation.h"
#include "AnimBlendClumpData.h"
#include "AnimBlendNode.h"
#include "AnimBlendSequence.h"
#include "KeyGen.h"
#include "RwHelper.h"
#include "VisibilityPlugins.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

// Funciones de la DLL sin declaracion en cabeceras (solo para leer sus punteros como los del EXE).
void* ClumpAnimConstruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
void* ClumpAnimDestruct(void* object, RwInt32 offsetInObject, RwInt32 sizeInObject);
void* ClumpAnimCopy(void* dstObject, const void* srcObject, RwInt32 offsetInObject, RwInt32 sizeInObject);
void RtAnimBlendKeyFrameApply(void* voidMat, void* voidFrame);
void RpAnimBlendKeyFrameInterpolate(void* voidOut, void* voidIn1, void* voidIn2, float time, void* customData);

namespace RpAnim43Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }
// Sigue los saltos E9 de la DLL (thunks incrementales) hasta el cuerpo; se detiene si el salto va al EXE.
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> inline T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }

constexpr uintptr_t CLUMPOFF=0xB5F878, FOUNDFRAME=0xB5F87C, GPCLUMP=0xB4EA0C, NODENAMEOFF=0xC87C5C;

// --- traza sin reservas ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(!Main())return; if(nrec<recs.size())recs[nrec]={t,a,b}; ++nrec; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<std::min(nrec,recs.size());++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); r.push_back(nrec); return r; }

// --- buferes de los fixtures ---
constexpr size_t ASZ=0x40, NASSOC=12, HSZ=0x18, NHIER=8, FDSZ=0x18, NFD=32, FRSZ=0x400, NFR=20, KFSZ=0x1C, NKF=8;
constexpr size_t NARR=11, NODESZ=0x18, SEQSZ=0xC, MAXKF=6, KFB=32;
alignas(16) inline std::array<uint8,0x400> clump{};
alignas(16) inline std::array<uint8,0x20> cdata{}, vdata{};
alignas(16) inline std::array<uint8,NASSOC*ASZ> assocs{};
alignas(16) inline std::array<uint8,NHIER*HSZ> hiers{};
alignas(16) inline std::array<uint8,NFD*FDSZ> frameDatas{};
alignas(16) inline std::array<uint8,NFR*FRSZ> frames{};
alignas(16) inline std::array<uint8,NKF*KFSZ> kfs{};
alignas(16) inline std::array<uint8,0x40> atomic{}, geometry{}, skin{}, hanim{}, matrix{}, interpFrame{}, anim{}, updData{}, outCells{}, pedPos{}, interpInfo{};
alignas(16) inline std::array<uint8,0x4C+0x40> interp{};
alignas(16) inline std::array<uint8,64*16> nodeInfo{};
alignas(16) inline std::array<uint8,64*4> outArr{};
alignas(16) inline std::array<uint8,NARR*2*NODESZ> nodes{};
alignas(16) inline std::array<uint8,NARR*SEQSZ> seqs{};
alignas(16) inline std::array<uint8,NARR*MAXKF*KFB> keyframes{};
alignas(64) inline std::array<uint8,0x8000> arena{}; inline size_t arenaTop{};
inline std::array<int32,NFR> hierIds{};
inline std::array<void*,4> fakeVt{};
inline uint8* A(size_t k){ return assocs.data()+k*ASZ; }
inline uint8* L(size_t k){ return A(k)+4; }
inline uint8* H(size_t k){ return hiers.data()+k*HSZ; }
inline uint8* FD(size_t k){ return frameDatas.data()+k*FDSZ; }
inline uint8* FR(size_t k){ return frames.data()+k*FRSZ; }
inline uint8* ND(size_t a){ return nodes.data()+a*2*NODESZ; }
inline uint8* SQ(size_t a){ return seqs.data()+a*SEQSZ; }
inline uint8* KB(size_t a){ return keyframes.data()+a*MAXKF*KFB; }

inline bool In(const void* p,const void* b,size_t n){ return uintptr_t(p)>=uintptr_t(b)&&uintptr_t(p)<uintptr_t(b)+n; }
inline bool InFixtures(const void* p){ return In(p,arena.data(),arena.size())||In(p,cdata.data(),cdata.size())||In(p,assocs.data(),assocs.size())||In(p,frameDatas.data(),frameDatas.size()); }
inline uint32 InArena(const void* p){ return In(p,arena.data(),arena.size()) ? uint32(uintptr_t(p)-uintptr_t(arena.data())) : 0xFFFFFFFFu; }
inline void* Alloc(size_t n,size_t align){
    const size_t off=(arenaTop+align-1)&~(align-1);
    if(n>arena.size()-std::min(off,arena.size()))return nullptr;
    arenaTop=off+n; return arena.data()+off; }

// --- punteros de la DLL leidos como los del EXE ---
inline std::array<std::pair<uint32,uint32>,32> normTable{}; inline size_t nNorm{};
inline void AddNorm(uintptr_t dll,uint32 exe){ if(nNorm+2<=normTable.size()){ normTable[nNorm++]={uint32(dll),exe}; normTable[nNorm++]={uint32(Body(dll)),exe}; } }
inline uint32 Norm(uint32 p){ for(size_t k=0;k<nNorm;++k)if(normTable[k].first==p)return normTable[k].second; return p; }

// --- dependencias simuladas (sin reservas; fuera del hilo principal se reenvian al CRT del EXE) ---
enum : uint32 { T_NEW=1,T_DEL,T_MALIGN,T_FALIGN,T_DTOR,T_START,T_FIRSTATOMIC,T_SKIN,T_SKINHIER,T_BONEPOS,T_HIERID,
    T_ANIMCREATE,T_ANIMCREATE2,T_REGPLUGIN,T_REGPLUGIN2,T_REGPLUGIN3,T_REGSCHEME };
inline int32 skinMode{}, regRet{}, numBonesTable{}; inline uint32 boneSeed{}, animMask{};
inline void* __cdecl FakeNew(uint32 n){
    if(!Main())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    void* p=Alloc(n,16); Push(T_NEW,n,InArena(p)); return p; }
inline void __cdecl FakeDelete(void* p){
    if(!Main()||(p&&!InFixtures(p))){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    Push(T_DEL,U(p)); }
inline void* __cdecl FakeMallocAlign(uint32 size,uint32 align,uint32 hint){
    if(!Main()){ // como 0x72F4C0
        auto* m=reinterpret_cast<uint8*(__cdecl*)(size_t)>(0x824257)(size+align);
        auto* r=reinterpret_cast<uint8*>(uintptr_t(m+align)&~uintptr_t(align-1)); reinterpret_cast<void**>(r)[-1]=m; return r; }
    void* p=Alloc(size,64); Push(T_MALIGN,size,align); Push(T_MALIGN,hint,InArena(p)); return p; }
inline void __cdecl FakeFreeAlign(void* p){
    if(!Main()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(reinterpret_cast<void**>(p)[-1]); return; }
    Push(T_FALIGN,U(p)); }
// Destructor virtual de las asociaciones falsas: traza y desenlaza (como el real), sin liberar.
inline void* __fastcall FakeAssocDtor(uint8* self,void*,int32 flag){
    Push(T_DTOR,U(self),uint32(flag));
    uint8* next=At<uint8*>(self,4); uint8* prev=At<uint8*>(self,8);
    if(prev)At<uint8*>(prev,0)=next;
    if(next)At<uint8*>(next,4)=prev;
    At<uint8*>(self,4)=nullptr; At<uint8*>(self,8)=nullptr;
    return self; }
inline void __fastcall FakeStart(void* a,void*,float t){ Push(T_START,U(a),F(t)); }
inline void* __cdecl FakeFirstAtomic(void* c){ void* r=skinMode>=1 ? atomic.data() : nullptr; Push(T_FIRSTATOMIC,U(c),U(r)); return r; }
inline void* __cdecl FakeSkin(void* g){ void* r=skinMode==2 ? skin.data() : nullptr; Push(T_SKIN,U(g),U(r)); return r; }
inline void* __cdecl FakeSkinHier(void* c){ Push(T_SKINHIER,U(c)); return hanim.data(); }
// La tabla esta en la pila del llamador: no se traza su direccion.
inline void __cdecl FakeBonePos(void* c,RwV3d* table){
    Push(T_BONEPOS,U(c));
    uint32 s=boneSeed;
    for(int32 k=0;k<std::min(numBonesTable,64);++k){
        float* v=&table[k].x;
        for(int q=0;q<3;++q){ s^=s<<13; s^=s>>17; s^=s<<5; v[q]=float(int32(s%20001)-10000)/4096.f; } } }
inline int32 __cdecl FakeHierId(void* f){
    const int32 k=In(f,frames.data(),frames.size()) ? int32((uintptr_t(f)-uintptr_t(frames.data()))/FRSZ) : -1;
    const int32 r=k>=0 ? hierIds[size_t(k)] : 0; Push(T_HIERID,U(f),uint32(r)); return r; }
inline void* __cdecl FakeAnimCreate(int32 type,int32 nf,int32 flags,float dur){
    Push(T_ANIMCREATE,uint32(type),uint32(nf)); Push(T_ANIMCREATE2,uint32(flags),F(dur));
    void* r=(animMask&1) ? anim.data() : nullptr; animMask>>=1; return r; }
inline int32 __cdecl FakeRegPlugin(int32 size,uint32 id,void* c,void* d,void* cp){
    Push(T_REGPLUGIN,uint32(size),id); Push(T_REGPLUGIN2,Norm(U(c)),Norm(U(d))); Push(T_REGPLUGIN3,Norm(U(cp))); return regRet; }
// La estructura esta en la pila del llamador: se copia (normalizada al tomar la instantanea).
inline int32 __cdecl FakeRegScheme(void* info){ Push(T_REGSCHEME); std::memcpy(interpInfo.data(),info,0x30); return TRUE; }

// Parches de una ejecucion: direcciones ya resueltas antes de escribir, sin duplicados, nunca sobre un stub,
// en un array fijo (sin reservas) y deshechos en orden inverso.
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,40> P; size_t n{};
    std::array<std::pair<uintptr_t,void*>,40> req{}; size_t nr{};
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
inline const std::array<Fn,42> fns{{
    {"RpAnimBlend","ClumpAnimConstruct",0x4D5F40,0,-1,64},{"RpAnimBlend","ClumpAnimCopy",0x4D5F90,0,-1,64},
    {"RpAnimBlend","ClumpAnimDestruct",0x4D6110,0,-1,256},
    {"RpAnimBlend","FrameUpdateCallBackCompressedNonSkinned",0x4D32D0,0,-1,2048},{"RpAnimBlend","FrameUpdateCallBackCompressedSkinned",0x4D2E40,0,-1,2048},
    {"RpAnimBlend","FrameUpdateCallBackNonSkinned",0x4D30A0,0,-1,2048},{"RpAnimBlend","FrameUpdateCallBackSkinned",0x4D2B90,0,-1,2048},
    {"RpAnimBlend","RpAnimBlendAllocateData",0x4D5F50,0,-1,64},{"RpAnimBlend","RpAnimBlendClumpAddAssociation",0x4D6790,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpExtractAssociations",0x4D6BE0,0,-1,128},{"RpAnimBlend","RpAnimBlendClumpFillFrameArray",0x4D64A0,0,-1,512},
    {"RpAnimBlend","RpAnimBlendClumpFindBone",0x4D6400,0,-1,256},{"RpAnimBlend","RpAnimBlendClumpFindFrame",0x4D62A0,0,-1,512},
    {"RpAnimBlend","RpAnimBlendClumpFindFrameFromHashKey",0x4D6370,0,-1,512},
    {"RpAnimBlend","RpAnimBlendClumpGetAssociation-AnimId",0x4D68B0,0,-1,256},{"RpAnimBlend","RpAnimBlendClumpGetAssociation-AnimName",0x4D6870,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpGetAssociation-Hier",0x4D68E0,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpGetFirstAssociation-",0x4D15E0,0,-1,128},{"RpAnimBlend","RpAnimBlendClumpGetFirstAssociation-Flags",0x4D6A70,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpGetMainAssociation",0x4D6910,0,-1,512},{"RpAnimBlend","RpAnimBlendClumpGetMainAssociation_N",0x4D69F0,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpGetMainPartialAssociation",0x4D69A0,0,-1,256},{"RpAnimBlend","RpAnimBlendClumpGetMainPartialAssociation_N",0x4D6A30,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpGetNumAssociations",0x4D6B60,0,-1,128},{"RpAnimBlend","RpAnimBlendClumpGetNumNonPartialAssociations",0x4D6BB0,0,-1,128},
    {"RpAnimBlend","RpAnimBlendClumpGetNumPartialAssociations",0x4D6B80,0,-1,128},{"RpAnimBlend","RpAnimBlendClumpGiveAssociations",0x4D6C30,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpInit",0x4D6720,0,-1,512},{"RpAnimBlend","RpAnimBlendClumpIsInitialized",0x4D6760,0,-1,64},
    {"RpAnimBlend","RpAnimBlendClumpPauseAllAnimations",0x4D6B00,0,-1,128},{"RpAnimBlend","RpAnimBlendClumpRemoveAllAssociations",0x4D6C00,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpRemoveAssociations",0x4D6820,0,-1,256},{"RpAnimBlend","RpAnimBlendClumpSetBlendDeltas",0x4D67E0,0,-1,256},
    {"RpAnimBlend","RpAnimBlendClumpUnPauseAllAnimations",0x4D6B30,0,-1,128},{"RpAnimBlend","RpAnimBlendCreateAnimationForHierarchy",0x4D60E0,0,-1,128},
    {"RpAnimBlend","RpAnimBlendFrameGetName",0x4D5EF0,0,-1,64},{"RpAnimBlend","RpAnimBlendFrameSetName",0x4D5F00,0,-1,128},
    {"RpAnimBlend","RpAnimBlendGetNextAssociation-Any",0x4D6AB0,0,-1,128},{"RpAnimBlend","RpAnimBlendGetNextAssociation-Flags",0x4D6AD0,0,-1,256},
    {"RpAnimBlend","RpAnimBlendKeyFrameInterpolate",0x4D60C0,0,-1,64},{"RpAnimBlend","RpAnimBlendPluginAttach",0x4D6150,0,-1,64},
    {"RpAnimBlend","RtAnimBlendKeyFrameApply",0x4D5FA0,0,-1,1024},
}};

// --- nombres de los fixtures ---
inline const char* const animNames[]{"walk_civi","run_civi","idle_stance","WALK_CIVI","sprint_civi","fall_back","handsup","Run_Player"};
inline std::array<uint32,std::size(animNames)> animHashes{};
inline const char* const frameNames[]{"body","Head","wheel_lf_dummy","chassis","door_rf_dummy","BODY","bonnet_dummy","Pelvis","r upperarm","", "a_name_of_exactly_23_ch"};
inline const char* const needles[]{"Root","Pelvis","Spine","Spine1","Neck","Head","Jaw","R UpperArm","L Hand","Bip01 R Clavicle","r upperarm","HEAD","L Toe","Belly",
    "pelvis","bip01 l clavicle","R Fingers","L Finger01","x","","body","Body","WHEEL_LF_DUMMY","chassis","door_rf_dummy","bonnet_dummy","a_name_of_exactly_23_ch"};
inline const char* const setNames[]{"","a","Bip01","wheel_rb_dummy","a_name_of_exactly_23_ch","a_name_longer_than_twenty_three","Head"};
inline const int32 boneTags[]{0,1,2,3,4,5,6,7,8,21,22,23,24,25,26,31,32,33,34,35,36,41,42,43,44,51,52,53,54,201,301,302,999,-1};
inline const int32 pedTags[]{3,5,0x20,0x16,0x22,0x18,0x29,0x33,0x2B,0x35,0x2A,0x34,0x21,0x17,0x1F,0x15,4,8};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"rpanim43-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"rpanim43-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("rpanim43");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"rpanim43-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,42> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,42> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    using std::bit_cast;
    for(size_t k=0;k<std::size(animNames);++k)animHashes[k]=CKeyGen::GetUppercaseKey(animNames[k]);
    fakeVt={reinterpret_cast<void*>(&FakeAssocDtor),nullptr,nullptr,nullptr};
    nNorm=0;
    AddNorm(bit_cast<uintptr_t>(&ClumpAnimConstruct),0x4D5F40); AddNorm(bit_cast<uintptr_t>(&ClumpAnimDestruct),0x4D6110);
    AddNorm(bit_cast<uintptr_t>(&ClumpAnimCopy),0x4D5F90); AddNorm(bit_cast<uintptr_t>(&RtAnimBlendKeyFrameApply),0x4D5FA0);
    AddNorm(bit_cast<uintptr_t>(&RpAnimBlendKeyFrameInterpolate),0x4D60C0); AddNorm(bit_cast<uintptr_t>(&RpHAnimKeyFrameBlend),0x7C60C0);
    AddNorm(bit_cast<uintptr_t>(&RpHAnimKeyFrameAdd),0x7C6720); AddNorm(bit_cast<uintptr_t>(&RpHAnimKeyFrameMulRecip),0x7C65C0);
    AddNorm(bit_cast<uintptr_t>(&RpHAnimKeyFrameStreamRead),0x7C64C0); AddNorm(bit_cast<uintptr_t>(&RpHAnimKeyFrameStreamWrite),0x7C6540);
    AddNorm(bit_cast<uintptr_t>(&RpHAnimKeyFrameStreamGetSize),0x7C65B0);

    const uint32 clumpOffset=G<uint32>(CLUMPOFF);
    if(clumpOffset+4>clump.size()||clumpOffset<8){Write({{"event","error"},{"reason","clump plugin offset out of fixture"},{"offset",clumpOffset}});return;}
    const int32 nameOff=G<int32>(NODENAMEOFF);
    if(nameOff<=0||size_t(nameOff)+32>FRSZ||nameOff<0xA4){Write({{"event","error"},{"reason","node name offset out of fixture"},{"offset",nameOff}});return;}

    // Cuerpos de la DLL resueltos antes de escribir ningun parche.
    struct Stub { uintptr_t exe, dll; void* fake; };
    const auto B=[](uintptr_t p){ return p ? Body(p) : uintptr_t(0); };
    const std::array<Stub,13> stubs{{
        {0x82119A,0,reinterpret_cast<void*>(&FakeNew)},{0x8214BD,0,reinterpret_cast<void*>(&FakeDelete)},
        {0x72F4C0,0,reinterpret_cast<void*>(&FakeMallocAlign)},{0x72F4F0,0,reinterpret_cast<void*>(&FakeFreeAlign)},
        {0x734820,B(bit_cast<uintptr_t>(&GetFirstAtomic)),reinterpret_cast<void*>(&FakeFirstAtomic)},
        {0x7C7550,0,reinterpret_cast<void*>(&FakeSkin)},
        {0x734A40,B(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump)),reinterpret_cast<void*>(&FakeSkinHier)},
        {0x735360,0,reinterpret_cast<void*>(&FakeBonePos)},
        {0x732A20,B(bit_cast<uintptr_t>(&CVisibilityPlugins::GetFrameHierarchyId)),reinterpret_cast<void*>(&FakeHierId)},
        {0x4CEB70,B(bit_cast<uintptr_t>(&CAnimBlendAssociation::Start)),reinterpret_cast<void*>(&FakeStart)},
        {0x7CCE40,0,reinterpret_cast<void*>(&FakeAnimCreate)},{0x74BDD0,0,reinterpret_cast<void*>(&FakeRegPlugin)},
        {0x7CCD40,0,reinterpret_cast<void*>(&FakeRegScheme)},
    }};
    std::vector<uintptr_t> patchedExe; for(auto& s:stubs)patchedExe.push_back(s.exe);
    std::vector<std::array<uint8,5>> savedEntries(patchedExe.size());
    for(size_t k=0;k<patchedExe.size();++k)std::memcpy(savedEntries[k].data(),reinterpret_cast<void*>(patchedExe[k]),5);

    // estado real guardado (globales del EXE que tocan las funciones)
    struct Blob { uintptr_t p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{CLUMPOFF,4},{FOUNDFRAME,4},{GPCLUMP,4}};
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
            auto RndF=[&](uint8* p,size_t n,float lo,float hi){ for(size_t k=0;k+4<=n;k+=4)At<float>(p,k)=uni(lo,hi); };
            constexpr float kQNaN=std::numeric_limits<float>::quiet_NaN();
            float lastBlend=0.5f;
            // Blends con 0, negativos, NaN silencioso, 1 y empates exactos.
            auto blendVal=[&]{ const uint32 r=next()%10; const float v=r==0 ? 0.f : r==1 ? -uni(0.f,1.f) : r==2 ? kQNaN : r==3 ? 1.f : r==4 ? lastBlend : uni(0.01f,1.f); lastBlend=v; return v; };

            // ---- mundo de fixtures comun ----
            auto buildBase=[&]{
                Rnd(clump.data(),clump.size()); clump[0]=2; At<void*>(clump.data(),clumpOffset)=cdata.data(); At<void*>(clump.data(),4)=FR(0);
                cdata.fill(0); At<uint32>(cdata.data(),8)=1+next()%8; At<void*>(cdata.data(),0xC)=next()%2 ? pedPos.data() : nullptr; At<void*>(cdata.data(),0x10)=frameDatas.data();
                for(size_t h=0;h<NHIER;++h){ Rnd(H(h),HSZ); At<uint32>(H(h),0)=next()%4 ? animHashes[next()%animHashes.size()] : next(); }
                for(size_t k=0;k<NASSOC;++k){ uint8* a=A(k); Rnd(a,ASZ);
                    At<void*>(a,0)=fakeVt.data(); At<void*>(a,4)=nullptr; At<void*>(a,8)=nullptr; At<void*>(a,0x14)=H(next()%NHIER);
                    At<float>(a,0x18)=blendVal(); RndF(a+0x1C,0x10,-2.f,2.f);
                    At<int16>(a,0x2C)=int16(next()%4==0 ? -int32(next()%3) : int32(next()%300)); At<uint16>(a,0x2E)=uint16(next()); }
                for(size_t k=0;k<NFR;++k){ uint8* fr=FR(k); std::memset(fr,0,FRSZ); Rnd(fr,8); RndF(fr+0x10,0x40,-5.f,5.f); At<uint32>(fr,0x1C)=next();
                    const char* s=frameNames[next()%std::size(frameNames)]; std::strcpy(reinterpret_cast<char*>(fr+nameOff),s); hierIds[k]=int32(next()%19); }
                Rnd(frameDatas.data(),frameDatas.size());
                for(size_t k=0;k<NFD;++k){ uint8* fd=FD(k); RndF(fd+4,12,-3.f,3.f); At<void*>(fd,0x10)=FR(k%NFR); At<int32>(fd,0x14)=boneTags[next()%std::size(boneTags)]; }
                RndF(kfs.data(),kfs.size(),-2.f,2.f); Rnd(outArr.data(),outArr.size()); Rnd(outCells.data(),outCells.size()); RndF(pedPos.data(),pedPos.size(),-4.f,4.f);
                Rnd(matrix.data(),matrix.size()); Rnd(interpFrame.data(),interpFrame.size()); Rnd(anim.data(),anim.size()); interpInfo.fill(0);
                atomic.fill(0); At<void*>(atomic.data(),0x18)=geometry.data(); Rnd(geometry.data(),geometry.size());
                skin.fill(0); hanim.fill(0); Rnd(interp.data(),interp.size()); Rnd(nodeInfo.data(),nodeInfo.size());
                Rnd(nodes.data(),nodes.size()); Rnd(seqs.data(),seqs.size()); Rnd(keyframes.data(),keyframes.size()); updData.fill(0); vdata.fill(0);
                G<uint32>(FOUNDFRAME)=next(); G<void*>(GPCLUMP)=vdata.data();
                skinMode=0; regRet=0; numBonesTable=0; boneSeed=next()|1; animMask=next();
            };
            // Lista de la clump: asociaciones 0..n-1 en orden aleatorio.
            int32 nList{}; std::array<int32,8> perm{0,1,2,3,4,5,6,7};
            auto buildList=[&](int32 minimum){
                nList=minimum+int32(next()%uint32(9-minimum));
                for(int32 k=7;k>0;--k)std::swap(perm[size_t(k)],perm[next()%uint32(k+1)]);
                uint8* prev=cdata.data(); At<uint8*>(prev,0)=nullptr;
                for(int32 k=0;k<nList;++k){ uint8* l=L(size_t(perm[size_t(k)])); At<uint8*>(prev,0)=l; At<uint8*>(l,4)=prev; At<uint8*>(l,0)=nullptr; prev=l; } };
            auto listAssoc=[&]{ return nList ? A(size_t(perm[next()%uint32(nList)])) : A(8); };
            auto anyFlags=[&]{ const uint32 r=next()%6; return r==0 ? 0u : r==1 ? next() : r==2 ? 0x10u : 1u<<(next()%16); };
            // Fixture de los callbacks de actualizacion: arrays de nodos con secuencias y keyframes reales.
            auto buildUpdate=[&](bool compressed,bool skinned){
                // Estres numerico en un caso de cada tres (i%3==2): NaN con signo y carga distintos (tambien senalizantes), +-inf, +-0, subnormales,
                // magnitudes de 1e30/1e-30 y quaternions sin normalizar o nulos. Generador PROPIO (sn): el principal (next) no se toca, asi que
                // los casos normales quedan identicos a los de antes. Se reinicia en cada ejecucion (original y candidata ven la misma secuencia).
                // Para que los falsos no rompan el flujo: el tiempo de los keyframes sigue siendo positivo y no diminuto (o NaN/+inf/1e30, que salen
                // del bucle de NextKeyFrame de inmediato); el paso de tiempo nunca es +inf ni grande y positivo, y el tiempo restante nunca es -inf ni
                // muy negativo (con un asociado en bucle el `while (restante <= 0)` no terminaria).
                const bool stress=i%3==2;
                uint32 sst=uint32(i)*2246822519u^uint32(f)*3266489917u^0x5712E55u; if(!sst)sst=1;
                auto sn=[&]{sst^=sst<<13;sst^=sst>>17;sst^=sst<<5;return sst;};
                const uint32 smode=stress ? sn()%8 : 0; // 0 NaN, 1 inf/0, 2 subnormales, 3 extremos, 4 sin normalizar, 5 todo, 6 solo blends/tiempos, 7 NaN+inf+extremos
                const bool dataMode=smode!=4&&smode!=6;
                auto SF=[](uint32 b){return std::bit_cast<float>(b);};
                const uint32 kNaNs[]{0x7FC00000u,0xFFC00000u,0x7FC00001u,0xFFC12345u,0x7FA00000u,0xFFA00000u,0x7FFFFFFFu,0xFFFFFFFFu,0x7FC54321u};
                const uint32 kInfs[]{0x7F800000u,0xFF800000u,0u,0x80000000u,0x7F800000u,0xFF800000u};
                const uint32 kSubs[]{1u,0x80000001u,0x007FFFFFu,0x807FFFFFu,0x00400000u,0x80400000u,0x000116C2u,0x800116C2u,0x00800000u,0x80800000u};
                const float  kBigs[]{1e30f,-1e30f,1e-30f,-1e-30f,3.4028235e38f,-3.4028235e38f,1e20f,-1e20f,1e-20f};
                const uint32 poolMask=smode==0 ? 1u : smode==1 ? 2u : smode==2 ? 4u : smode==3 ? 8u : smode==4 ? 0u : smode==7 ? (1u|2u|8u) : 15u;
                auto sp=[&]()->float{
                    uint32 pick=sn()%4; const uint32 mask=poolMask ? poolMask : 15u;
                    while(!((mask>>pick)&1u))pick=(pick+1)%4;
                    switch(pick){ case 0: return SF(kNaNs[sn()%std::size(kNaNs)]); case 1: return SF(kInfs[sn()%std::size(kInfs)]);
                                  case 2: return SF(kSubs[sn()%std::size(kSubs)]); default: return kBigs[sn()%std::size(kBigs)]; } };
                // Paso de tiempo (nunca +inf ni positivo grande) y tiempo restante (nunca -inf ni muy negativo).
                const float kTimeSteps[]{SF(0x7FC00000u),SF(0xFFC12345u),SF(0x7FA00000u),0.f,SF(0x80000000u),SF(1u),SF(0x80000001u),SF(0xFF800000u),-1e30f,1e-30f,-1e-30f};
                const float kRemain[]{SF(0x7FC00000u),SF(0xFFC12345u),SF(0x7FA00000u),SF(0x7F800000u),1e30f,0.f,SF(0x80000000u),SF(1u),SF(0x80000001u),1e-30f,-1e-30f};
                const float kKfTimes[]{SF(0x7FC00000u),SF(0xFFC12345u),SF(0x7F800000u),1e30f};
                const int32 kExtI16[]{32767,-32768,-32767,0,1,-1};
                const int32 nArr=next()%8==0 ? int32(NARR) : 1+int32(next()%4);
                At<uint32>(updData.data(),0)=next()%3==0 ? 0u : next()%4==0 ? 2u : 1u;
                bool anyValid=false;
                for(int32 a=0;a<nArr;++a){
                    uint8* nd=ND(size_t(a)); uint8* as=A(size_t(a)); uint8* sq=SQ(size_t(a)); uint8* kb=KB(size_t(a));
                    uint16 fl=uint16(next()); fl=uint16(fl&~0x13u); if(next()%4)fl|=1; if(next()%2)fl|=2; if(next()%3==0)fl|=0x10;
                    At<uint16>(as,0x2E)=fl; const uint32 r=next()%6;
                    At<float>(as,0x18)=r==0 ? 0.f : r==1 ? 1.f : uni(0.05f,1.f); At<float>(as,0x28)=next()%4==0 ? 0.f : uni(0.f,0.25f);
                    if(stress){
                        if(smode!=4&&sn()%(smode==6 ? 2u : 4u)==0)At<float>(as,0x18)=sp();
                        if(smode==6 ? sn()%2==0 : sn()%6==0)At<float>(as,0x28)=kTimeSteps[sn()%std::size(kTimeSteps)];
                    }
                    const bool trans=next()%2!=0; const int32 nf=1+int32(next()%5);
                    At<uint32>(sq,0)=next(); At<uint16>(sq,4)=uint16((next()%8 ? 1 : 0)|(trans ? 2 : 0)|(compressed ? 4 : 0)); At<uint16>(sq,6)=uint16(nf); At<void*>(sq,8)=kb;
                    const size_t stride=compressed ? (trans ? 16 : 10) : (trans ? 32 : 20);
                    for(int32 k=0;k<nf;++k){ uint8* kf=kb+size_t(k)*stride;
                        float q[4]; float len=0.f; for(auto& c:q){ c=uni(-1.f,1.f); len+=c*c; } len=std::sqrt(std::max(len,0.01f)); for(auto& c:q)c/=len;
                        if(compressed){ for(int c=0;c<4;++c)At<int16>(kf,size_t(c)*2)=int16(q[c]*4096.f); At<int16>(kf,8)=int16(1+next()%30);
                            if(trans)for(int c=0;c<3;++c)At<int16>(kf,10+size_t(c)*2)=int16(int32(next()%4096)-2048); }
                        else { for(int c=0;c<4;++c)At<float>(kf,size_t(c)*4)=q[c]; At<float>(kf,16)=uni(0.05f,0.5f);
                            if(trans)for(int c=0;c<3;++c)At<float>(kf,20+size_t(c)*4)=uni(-2.f,2.f); } }
                    if(stress)for(int32 k=0;k<nf;++k){ uint8* kf=kb+size_t(k)*stride;
                        if(compressed){
                            const int32 sc=(smode==3||smode==4||smode==5||smode==7) ? 1+int32(sn()%8) : 1; const bool zero=smode==4&&sn()%4==0;
                            for(size_t c=0;c<4;++c){ int32 v=zero ? 0 : int32(At<int16>(kf,c*2))*sc; if(dataMode&&sn()%6==0)v=kExtI16[sn()%std::size(kExtI16)];
                                At<int16>(kf,c*2)=int16(std::clamp(v,-32768,32767)); }
                            if(trans)for(size_t c=0;c<3;++c){ int32 v=At<int16>(kf,10+c*2); if(dataMode&&sn()%6==0)v=kExtI16[sn()%std::size(kExtI16)];
                                At<int16>(kf,10+c*2)=int16(v); }
                        } else {
                            const float s=smode==3 ? ((sn()&1) ? 1e30f : 1e-30f) : (smode==4||smode==5||smode==7) ? float(1+sn()%2000)/20.f : 1.f; const bool zero=smode==4&&sn()%4==0;
                            for(size_t c=0;c<4;++c){ float v=zero ? 0.f : At<float>(kf,c*4)*s; if(dataMode&&sn()%4==0)v=sp(); At<float>(kf,c*4)=v; }
                            if(dataMode&&smode!=2&&sn()%12==0)At<float>(kf,16)=kKfTimes[sn()%std::size(kKfTimes)];
                            if(trans)for(size_t c=0;c<3;++c){ float v=At<float>(kf,20+c*4); if(smode==3&&sn()%2==0)v*=(sn()&1) ? 1e30f : 1e-30f; if(dataMode&&sn()%4==0)v=sp(); At<float>(kf,20+c*4)=v; }
                        } }
                    const bool valid=next()%5!=0; anyValid|=valid;
                    const int32 cur=nf>1 ? 1+int32(next()%uint32(nf-1)) : 0;
                    At<int16>(nd,8)=int16(cur); At<int16>(nd,0xA)=int16(cur ? cur-1 : nf-1); At<float>(nd,0xC)=uni(-0.2f,0.5f);
                    if(stress&&(smode==6 ? sn()%2==0 : sn()%6==0))At<float>(nd,0xC)=kRemain[sn()%std::size(kRemain)];
                    At<void*>(nd,0x10)=valid ? sq : nullptr; At<void*>(nd,0x14)=as;
                    if(valid){ auto* node=reinterpret_cast<CAnimBlendNode*>(nd); if(compressed)node->CalcDeltasCompressed(); else node->CalcDeltas(); }
                    At<void*>(updData.data(),4+size_t(a)*4)=nd;
                }
                if(!anyValid){ uint8* nd=ND(0); At<void*>(nd,0x10)=SQ(0); auto* node=reinterpret_cast<CAnimBlendNode*>(nd); if(compressed)node->CalcDeltasCompressed(); else node->CalcDeltas(); }
                uint8* fd=FD(0); fd[0]=uint8(next()); RndF(fd+4,12,-1.f,1.f);
                if(stress&&dataMode)for(size_t c=0;c<3;++c)if(sn()%3==0)At<float>(fd,4+c*4)=sp();
                At<void*>(fd,0x10)=skinned ? static_cast<void*>(kfs.data()) : static_cast<void*>(FR(1));
                At<void*>(vdata.data(),0xC)=next()%2 ? pedPos.data() : nullptr; G<void*>(GPCLUMP)=vdata.data();
            };

            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x43A5u; if(!stt)stt=1; nrec=0;
                lastBlend=0.5f; nList=0; perm={0,1,2,3,4,5,6,7};
                arena.fill(0xCD); arenaTop=0x40;
                buildBase();

                const uintptr_t a=fn.address;
                json out=json::object(); json ret=nullptr;
                auto call=[&](auto&& body){
                    for(auto* h:hooks)h->State(candidate);
                    out["switches"]=ReversingProbe::JumpsToDll(a)==candidate;
                    { Patches p;
                      for(auto& s:stubs){ p.Want(s.exe,s.fake); p.Want(s.dll,s.fake); }
                      p.Apply(); body(); }
                    for(auto* h:hooks)h->State(false); };
                void* clumpP=clump.data();

                if(nm=="ClumpAnimConstruct"){
                    const int32 off=next()%2 ? int32(clumpOffset) : int32(8+4*(next()%250)); const int32 sz=int32(next()%64);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(clumpP,off,sz); }); ret=U(r); }
                else if(nm=="ClumpAnimCopy"){
                    const int32 off=int32(next()%0x100), sz=int32(next()%64);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,const void*,int32,int32)>(a)(clumpP,cdata.data(),off,sz); }); ret=U(r); }
                else if(nm=="ClumpAnimDestruct"){
                    buildList(0);
                    const uint32 m=next()%4; if(m==0)At<void*>(clumpP,clumpOffset)=nullptr;
                    else if(m==1)At<void*>(cdata.data(),0x10)=nullptr; else At<void*>(cdata.data(),0x10)=arena.data()+0x40*(1+next()%8);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(clumpP,int32(next()%0x100),int32(next()%64)); }); ret=U(r); }
                else if(nm=="RpAnimBlendAllocateData"){
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(clumpP); }); ret=U(r); }
                else if(nm=="RtAnimBlendKeyFrameApply"){
                    const uint32 m=next()%4;
                    for(int c=0;c<4;++c)At<float>(interpFrame.data(),size_t(c)*4)=m==0 ? uni(-1.f,1.f)*0.5f : m==1 ? (next()%3 ? uni(-1.f,1.f) : 0.f) : uni(-1.5f,1.5f);
                    if(m==3){ float len=0.f; for(int c=0;c<4;++c)len+=At<float>(interpFrame.data(),size_t(c)*4)*At<float>(interpFrame.data(),size_t(c)*4);
                        len=std::sqrt(std::max(len,0.01f)); for(int c=0;c<4;++c)At<float>(interpFrame.data(),size_t(c)*4)/=len; }
                    RndF(interpFrame.data()+0x10,12,-50.f,50.f);
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(matrix.data(),interpFrame.data()); }); }
                else if(nm=="RpAnimBlendKeyFrameInterpolate"){
                    const float t=uni(0.f,1.f); void* cd=next()%2 ? anim.data() : nullptr;
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,void*,void*,float,void*)>(a)(interpFrame.data(),kfs.data(),kfs.data()+KFSZ,t,cd); }); }
                else if(nm=="RpAnimBlendCreateAnimationForHierarchy"){
                    At<int32>(hanim.data(),4)=next()%8==0 ? -int32(next()%5) : int32(next()%70);
                    void* h=next()%6==0 ? nullptr : hanim.data();
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(h); }); ret=U(r); }
                else if(nm=="RpAnimBlendPluginAttach"){
                    const int32 rv[]{0x10,0x1F0,0x2A8,-1,-2,0,0x7FFFFFF0,int32(0x80000000)}; regRet=rv[next()%std::size(rv)];
                    int32 r{}; uint32 written{};
                    call([&]{ r=reinterpret_cast<int32(__cdecl*)()>(a)(); written=G<uint32>(CLUMPOFF); G<uint32>(CLUMPOFF)=clumpOffset; });
                    ret=r; out["offset"]=written;
                    json info=json::array(); for(size_t k=0;k<0x30;k+=4)info.push_back(Norm(At<uint32>(interpInfo.data(),k))); out["info"]=info; }
                else if(nm=="RpAnimBlendClumpInit"){
                    skinMode=int32(next()%3);
                    At<void*>(clumpP,clumpOffset)=reinterpret_cast<void*>(uintptr_t(next()));
                    if(skinMode==2){
                        const int32 nb=next()%8==0 ? int32(next()%3) : 1+int32(next()%24); At<int32>(skin.data(),0)=nb; numBonesTable=nb;
                        At<int32>(hanim.data(),4)=nb; At<void*>(hanim.data(),0x10)=nodeInfo.data(); At<void*>(hanim.data(),0x20)=interp.data();
                        At<int32>(interp.data(),0x24)=next()%3==0 ? int32(next()%64) : (next()%2 ? 0x1C : 0x24);
                        for(int32 k=0;k<64;++k)At<int32>(nodeInfo.data(),size_t(k)*16)=boneTags[next()%std::size(boneTags)];
                    } else {
                        const int32 nDesc=int32(next()%15);
                        for(int32 k=1;k<=nDesc;++k){ uint8* parent=FR(next()%uint32(k)); At<uint8*>(FR(size_t(k)),0x9C)=At<uint8*>(parent,0x98); At<uint8*>(parent,0x98)=FR(size_t(k)); }
                    }
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(clumpP); });
                    out["slot"]=U(At<void*>(clumpP,clumpOffset)); }
                else if(nm=="RpAnimBlendClumpIsInitialized"){
                    const uint32 m=next()%4; if(m==0)At<void*>(clumpP,clumpOffset)=nullptr; else At<uint32>(cdata.data(),8)=m==1 ? 0u : m==2 ? next() : 1u+next()%30;
                    uint32 r{}; call([&]{ r=reinterpret_cast<uint32(__cdecl*)(void*)>(a)(clumpP); }); ret=r&0xFFu; } // los llamadores del EXE miran AL
                else if(nm=="RpAnimBlendClumpAddAssociation"){
                    buildList(0); uint8* as=A(11); const uint32 fl=next()%3==0 ? next() : next()&0xFFFF; const float st=uni(0.f,3.f), bl=next()%5==0 ? kQNaN : uni(-1.f,2.f);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,void*,uint32,float,float)>(a)(clumpP,as,fl,st,bl); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpExtractAssociations"){
                    buildList(1); void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(clumpP); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGiveAssociations"){
                    buildList(0);
                    const int32 nNew=1+int32(next()%3);
                    for(int32 k=0;k<nNew;++k){ uint8* l=L(size_t(8+k)); At<uint8*>(l,0)=k+1<nNew ? L(size_t(9+k)) : nullptr; At<uint8*>(l,4)=k ? L(size_t(7+k)) : reinterpret_cast<uint8*>(uintptr_t(next())); }
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(clumpP,A(8)); }); }
                else if(nm=="RpAnimBlendClumpFillFrameArray"){
                    skinMode=next()%2 ? 2 : int32(next()%2);
                    At<uint32>(cdata.data(),8)=next()%13;
                    At<int32>(hanim.data(),4)=next()%8==0 ? -int32(next()%3) : int32(next()%25); At<void*>(hanim.data(),0x10)=nodeInfo.data();
                    for(int32 k=0;k<64;++k)At<int32>(nodeInfo.data(),size_t(k)*16)=next()%4 ? pedTags[next()%std::size(pedTags)] : boneTags[next()%std::size(boneTags)];
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(clumpP,outArr.data()); }); }
                else if(nm=="RpAnimBlendClumpFindBone"){
                    At<uint32>(cdata.data(),8)=next()%13; const int32 tag=next()%5==0 ? int32(next()) : boneTags[next()%std::size(boneTags)];
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32)>(a)(clumpP,tag); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpFindFrame"||nm=="RpAnimBlendClumpFindFrameFromHashKey"){
                    skinMode=next()%2 ? 2 : int32(next()%2);
                    At<uint32>(cdata.data(),8)=next()%13;
                    const char* s=needles[next()%std::size(needles)];
                    void* r{};
                    if(nm=="RpAnimBlendClumpFindFrame")call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,const char*)>(a)(clumpP,s); });
                    else { const uint32 key=next()%6==0 ? next() : CKeyGen::GetUppercaseKey(s); out["key"]=key;
                        call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,uint32)>(a)(clumpP,key); }); }
                    ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetAssociation-AnimId"){
                    buildList(0); const int32 id=next()%3==0 ? At<int16>(listAssoc(),0x2C) : next()%4==0 ? -int32(next()%3) : int32(next()%300);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32)>(a)(clumpP,id); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetAssociation-AnimName"){
                    buildList(0); const char* s=animNames[next()%std::size(animNames)];
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,const char*)>(a)(clumpP,s); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetAssociation-Hier"){
                    buildList(0); void* h=next()%6==0 ? nullptr : H(next()%NHIER); const int32 b=int32(next()%2);
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32,void*)>(a)(clumpP,b,h); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetFirstAssociation-"){
                    buildList(0); const uint32 m=next()%4; if(m==0)At<void*>(clumpP,clumpOffset)=nullptr; else if(m==1)At<uint32>(cdata.data(),8)=0;
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(clumpP); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetFirstAssociation-Flags"){
                    buildList(0); const uint32 fl=anyFlags();
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,uint32)>(a)(clumpP,fl); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetMainAssociation"){
                    buildList(0); void* p2=next()%3 ? outCells.data() : nullptr; void* pb=next()%3 ? outCells.data()+4 : nullptr;
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,void*,void*)>(a)(clumpP,p2,pb); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetMainAssociation_N"||nm=="RpAnimBlendClumpGetMainPartialAssociation_N"){
                    buildList(0); const int32 n=int32(next()%11)-1;
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,int32)>(a)(clumpP,n); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetMainPartialAssociation"){
                    buildList(0); void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(clumpP); }); ret=U(r); }
                else if(nm=="RpAnimBlendClumpGetNumAssociations"||nm=="RpAnimBlendClumpGetNumNonPartialAssociations"||nm=="RpAnimBlendClumpGetNumPartialAssociations"){
                    buildList(0); uint32 r{}; call([&]{ r=reinterpret_cast<uint32(__cdecl*)(void*)>(a)(clumpP); }); ret=r; }
                else if(nm=="RpAnimBlendClumpPauseAllAnimations"||nm=="RpAnimBlendClumpUnPauseAllAnimations"||nm=="RpAnimBlendClumpRemoveAllAssociations"){
                    buildList(0); call([&]{ reinterpret_cast<void(__cdecl*)(void*)>(a)(clumpP); }); }
                else if(nm=="RpAnimBlendClumpRemoveAssociations"){
                    buildList(0); const uint32 fl=anyFlags(); call([&]{ reinterpret_cast<void(__cdecl*)(void*,uint32)>(a)(clumpP,fl); }); }
                else if(nm=="RpAnimBlendClumpSetBlendDeltas"){
                    buildList(0); const uint32 fl=anyFlags(); const float d=next()%6==0 ? kQNaN : uni(-8.f,8.f);
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,uint32,float)>(a)(clumpP,fl,d); }); }
                else if(nm=="RpAnimBlendGetNextAssociation-Any"||nm=="RpAnimBlendGetNextAssociation-Flags"){
                    buildList(0); void* as=listAssoc(); void* r{};
                    if(nm=="RpAnimBlendGetNextAssociation-Any")call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(as); });
                    else { const uint32 fl=anyFlags(); call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,uint32)>(a)(as,fl); }); }
                    ret=U(r); }
                else if(nm=="RpAnimBlendFrameGetName"){
                    void* fr=FR(next()%NFR); void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*)>(a)(fr); }); ret=U(r); }
                else if(nm=="RpAnimBlendFrameSetName"){
                    void* fr=FR(next()%NFR); const char* s=setNames[next()%std::size(setNames)];
                    void* r{}; call([&]{ r=reinterpret_cast<void*(__cdecl*)(void*,const char*)>(a)(fr,s); }); ret=U(r); }
                else if(nm.starts_with("FrameUpdateCallBack")){
                    const bool compressed=nm.find("Compressed")!=std::string::npos, skinned=nm.find("NonSkinned")==std::string::npos;
                    buildUpdate(compressed,skinned);
                    call([&]{ reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(FD(0),updData.data()); }); }

                out["ret"]=ret; out["recs"]=Recs();
                out["clump"]=ReversingProbe::Bytes(clump.data(),clump.size()); out["cdata"]=ReversingProbe::Bytes(cdata.data(),cdata.size());
                out["assocs"]=ReversingProbe::Bytes(assocs.data(),assocs.size()); out["hiers"]=Fnv(hiers.data(),hiers.size());
                out["frameDatas"]=Fnv(frameDatas.data(),frameDatas.size()); out["frames"]=Fnv(frames.data(),frames.size());
                out["kfs"]=ReversingProbe::Bytes(kfs.data(),kfs.size()); out["out"]=Fnv(outArr.data(),outArr.size()); out["cells"]=ReversingProbe::Bytes(outCells.data(),outCells.size());
                out["matrix"]=ReversingProbe::Bytes(matrix.data(),matrix.size()); out["interpFrame"]=ReversingProbe::Bytes(interpFrame.data(),interpFrame.size());
                out["anim"]=ReversingProbe::Bytes(anim.data(),anim.size()); out["ped"]=ReversingProbe::Bytes(pedPos.data(),pedPos.size());
                out["nodes"]=ReversingProbe::Bytes(nodes.data(),nodes.size()); out["upd"]=ReversingProbe::Bytes(updData.data(),updData.size());
                out["seqs"]=Fnv(seqs.data(),seqs.size())^Fnv(keyframes.data(),keyframes.size()); out["interp"]=Fnv(interp.data(),interp.size())^Fnv(nodeInfo.data(),nodeInfo.size());
                out["fakes"]=Fnv(atomic.data(),atomic.size())^Fnv(geometry.data(),geometry.size())^Fnv(skin.data(),skin.size())^Fnv(hanim.data(),hanim.size())^Fnv(vdata.data(),vdata.size());
                out["arena"]=Fnv(arena.data(),arena.size()); out["arenaTop"]=arenaTop;
                out["globals"]={G<uint32>(FOUNDFRAME),G<uint32>(CLUMPOFF),U(G<void*>(GPCLUMP))};
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
