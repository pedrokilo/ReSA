#pragma once
// Banco dentro del motor: modelos atomicos y de peaton, colision, carteles y textos (lista de 9, 45 funciones).
// CAtomicModelInfo (VMT), CColModel, CCustomRoadsignMgr, CPedModelInfo (VMT) y CText. Original (hooks del lote
// desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// - Model infos falsos fuera de ms_modelInfoPtrs (ninguna funcion los busca por indice) con la vtable del EXE;
//   CAtomicModelInfo usa una copia de la vtable con GetAnimFileIndex trazado.
// - Objetos RW falsos (atomic, frame, clump, geometria, materiales, raster, textura, jerarquia HAnim). Las primitivas
//   RW que crean, destruyen o registran se simulan con trazas; la matematica RW (invertir, transformar matrices y
//   puntos, buscar nodos HAnim) corre de verdad sobre matrices propias (se cambian CGame::m_pWorkingMatrix1/2).
// - CColModel usa un pool falso (se cambia el puntero 0xB744A4); operator new/delete del EXE y CMemoryMgr van a
//   arenas fijas. Las trazas usan un registro fijo (sin reservas) porque operator new esta sustituido.
// - CText trabaja sobre la memoria de TheText intercambiada (se guarda y se restaura en cada ejecucion) con un
//   fichero GXT falso en memoria: CFileMgr (SetDir, SetDirMyDocuments, OpenFile, Read, Seek, CloseFile),
//   CTimer::Suspend/Resume y CMessages::ClearAllMessagesDisplayedByGame se simulan. Nunca se tocan las tablas reales.
// Todos los globales tocados se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Models/AtomicModelInfo.h"
#include "Models/ClumpModelInfo.h"
#include "Models/PedModelInfo.h"
#include "Collision/ColModel.h"
#include "Collision/CollisionData.h"
#include "Collision/Collision.h"
#include "CustomRoadsignMgr.h"
#include "Text/Text.h"
#include "Core/KeyGen.h"
#include "Messages.h"
#include "FileMgr.h"
#include "MemoryMgr.h"
#include "TxdStore.h"
#include "TagManager.h"
#include "Animation/AnimManager.h"
#include "Pipelines/CustomBuilding/CustomBuildingRenderer.h"
#include "Fx/CarFXRenderer.h"
#include "RwHelper.h"
#include "Timer.h"
#include "Game.h"
#include "../ProbeRuntime.hpp"

namespace Models9Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 H(const void* p,size_t n){ return uint32(Fnv(p,n)); }
inline uint32 S(const char* s){ return s ? H(s,strlen(s))|1u : 0u; }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }

// --- registro de buffers propios (Id legible y deteccion de punteros de pila) ---
inline std::array<std::pair<uintptr_t,size_t>,96> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }
inline bool Known(const void* p){ const auto u=uintptr_t(p); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return true; return false; }
inline uint32 Id(const void* p){ const auto u=uintptr_t(p); if(!u)return 0; for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first)); return uint32(u); }

// --- registro sin reservas: los stubs corren con operator new sustituido ---
struct Rec { uint32 tag,a,b,c,d; };
inline std::array<Rec,8192> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(nrec==recs.size())r.push_back("overflow"); return r; }

// --- mascaras de fallo por recurso ---
inline uint32 failMask[8]{}, failCalls[8]{};
inline bool Fail(int k){ return (failMask[k]>>(failCalls[k]++%32))&1; }

// --- operator new/delete del EXE: arena fija ---
alignas(16) inline std::array<uint8,0x10000> newArena{}; inline size_t newTop{}; inline bool recordNew{};
alignas(16) inline std::array<std::array<uint8,0x200>,4> fixArr{}; // arrays de fixture que liberan las funciones
inline void* __cdecl FakeNew(size_t n){ const size_t off=newTop; if(n>newArena.size()-off){ if(recordNew)Push(1,uint32(n),0xFFFFFFFFu); return nullptr; } newTop=(off+n+15)&~size_t(15); if(recordNew)Push(1,uint32(n),uint32(off)); return newArena.data()+off; }
inline void RealFree(void* p){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); } // free del CRT del EXE (destino de 0x8214BD)
// Con recordNew=false (CText) operator new no se sustituye: lo que no sea un array de fixture se libera de verdad
// (p. ej. las cadenas de spdlog de CheckFileEncoding, NOTSA, que viven en el heap real).
inline void __cdecl FakeDelete(void* p){ if(!p)return; const auto u=uintptr_t(p); const bool fix=u>=uintptr_t(fixArr.data())&&u<uintptr_t(fixArr.data())+sizeof(fixArr);
    if(fix||recordNew)Push(2,Id(p)); else RealFree(p); }
struct NewStubs { EntryPatch n1{0x82119A,FakeNew},n2{0x821195,FakeNew},d1{0x8214BD,FakeDelete},d2{0x8213AE,FakeDelete}; };
struct DelStubs { EntryPatch d1{0x8214BD,FakeDelete},d2{0x8213AE,FakeDelete}; };

// --- CMemoryMgr ---
alignas(16) inline std::array<uint8,0x2000> mArena{}; inline size_t mTop{};
inline void* __cdecl FakeMalloc(uint32 n,uint32 hint){ const size_t off=mTop; const bool ok=n<=mArena.size()-off; Push(14,n,hint,ok ? uint32(off) : 0xFFFFFFFFu); if(!ok)return nullptr; mTop=(off+n+15)&~size_t(15); return mArena.data()+off; }
inline void __cdecl FakeFree(void* p){ Push(15,Id(p)); }

// --- colision ---
inline void* __fastcall FakeCdCopy(void* self,void*,const void* src){ Push(10,Id(self),Id(src)); return self; }
inline void __fastcall FakeCdRemoveVolumes(void* self,void*){ Push(11,Id(self)); }
inline void __fastcall FakeCdCalcPlanes(void* self,void*){ Push(12,Id(self)); }
inline void __cdecl FakeColRemovePlanes(void* cd){ Push(13,Id(cd)); }
struct ColStubs {
    EntryPatch c1,c1e,c2,c2e,c3,c3e,c4,c4e,m1,m1e,m2,m2e;
    ColStubs() : c1(bit_cast<uintptr_t>(&CCollisionData::Copy),FakeCdCopy), c1e(0x40F120,FakeCdCopy),
        c2(bit_cast<uintptr_t>(&CCollisionData::RemoveCollisionVolumes),FakeCdRemoveVolumes), c2e(0x40F070,FakeCdRemoveVolumes),
        c3(bit_cast<uintptr_t>(&CCollisionData::CalculateTrianglePlanes),FakeCdCalcPlanes), c3e(0x40F590,FakeCdCalcPlanes),
        c4(bit_cast<uintptr_t>(static_cast<void(*)(CCollisionData*)>(&CCollision::RemoveTrianglePlanes)),FakeColRemovePlanes), c4e(0x416400,FakeColRemovePlanes),
        m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc), m1e(0x72F420,FakeMalloc),
        m2(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), m2e(0x72F430,FakeFree) {}
};

// --- objetos falsos ---
constexpr int NPOOL=8;
alignas(16) inline std::array<uint8,0x20> poolHdr{}; alignas(16) inline std::array<uint8,NPOOL*0x30> poolStorage{}; alignas(16) inline std::array<uint8,NPOOL> poolStates{};
alignas(16) inline std::array<uint8,0x30> cmA{}, cmB{}, cdA{}, cdB{};
alignas(16) inline std::array<uint8,12*0x14> spheres{};
alignas(16) inline std::array<uint8,0x60> obj{};
inline std::array<uintptr_t,20> vtCopy{};
alignas(16) inline std::array<std::array<uint8,0x80>,4> atomics{};
alignas(16) inline std::array<std::array<uint8,0xA0>,4> frames{};
alignas(16) inline std::array<std::array<uint8,0x40>,2> clumps{};
alignas(16) inline std::array<std::array<uint8,0x200>,4> geoms{};
alignas(16) inline std::array<std::array<uint8,0x10>,4> plugins{};
alignas(16) inline std::array<std::array<uint8,0x20>,6> mats{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> rasters{};
alignas(16) inline std::array<std::array<uint8,0x2800>,3> pixels{};
alignas(16) inline std::array<uint8,0x10000> charset{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> textures{};
alignas(16) inline std::array<uint8,0x30> morph{};
alignas(16) inline std::array<uint8,16*12> verts{}; alignas(16) inline std::array<uint8,8*8> tris{};
alignas(16) inline std::array<uint8,16*4> prelit{}; alignas(16) inline std::array<uint8,16*8> uvs{};
alignas(16) inline std::array<uint8,0x40> hier{}; alignas(16) inline std::array<uint8,32*16> nodes{};
alignas(16) inline std::array<uint8,64+32*64> matArr{}; alignas(16) inline std::array<uint8,64> wm1{}, wm2{};
alignas(16) inline std::array<std::array<uint8,20>,4> lines{};
inline std::array<int,4> geomMatCount{};

// --- model infos ---
inline int32 animIndex{};
inline int32 __fastcall FakeGetAnimFileIndex(void* self,void*){ Push(20,Id(self)); return animIndex; }
inline void __fastcall FakeAddTxdRef(void* self,void*){ Push(21,Id(self)); }
inline void __fastcall FakeRemoveTxdRef(void* self,void*){ Push(22,Id(self)); }
inline void __cdecl FakeAddAnimRef(int32 i){ Push(23,uint32(i)); }
inline void __cdecl FakeRemoveAnimRef(int32 i){ Push(24,uint32(i)); }
inline RwBool __cdecl FakeAtomicDestroy(RpAtomic* a){ Push(25,Id(a)); return TRUE; }
inline RwBool __cdecl FakeFrameDestroy(RwFrame* f){ Push(26,Id(f)); return TRUE; }
inline uint32 cbpc{}, ccpc{};
inline uint32 __cdecl FakeIsCBPC(RpAtomic* a){ Push(27,Id(a),cbpc); return cbpc; }
inline void __cdecl FakeAtomicSetup(RpAtomic* a){ Push(28,Id(a)); }
inline uint32 __cdecl FakeIsCCPC(RpAtomic* a){ Push(29,Id(a),ccpc); return ccpc; }
inline RpAtomic* __cdecl FakeSetCustomFX(RpAtomic* a,void* d){ Push(30,Id(a),U(d)); return a; }
inline void __cdecl FakeTagSetup(RpAtomic* a){ Push(31,Id(a)); }
inline void __fastcall FakeClumpDelete(void* self,void*){ Push(40,Id(self)); }
inline void __fastcall FakeClumpSetClump(void* self,void*,RpClump* c){ Push(41,Id(self),Id(c)); }
inline void __fastcall FakeSetFrameIds(void* self,void*,void* ids){ Push(42,Id(self),U(ids)); }
inline RpClump* __cdecl FakeForAllAtomics(RpClump* c,void*,void*){ Push(43,Id(c)); return c; }
inline void* hierResult{};
inline void* __cdecl FakeHierFromClump(RpClump* c){ Push(44,Id(c)); return hierResult; }
inline void* __cdecl FakeHierFromSkin(RpClump* c){ Push(45,Id(c)); return hierResult; }

// --- carteles ---
inline int32 txdSlotRet{};
inline int32 __cdecl FakeFindTxdSlot(const char* n){ Push(50,S(n)); return txdSlotRet; }
inline void __cdecl FakePushTxd(){ Push(51); }
inline void __cdecl FakePopTxd(){ Push(52); }
inline void __cdecl FakeSetTxd(int32 i){ Push(53,uint32(i)); }
inline RwTexture* __cdecl FakeTextureRead(const char* n,const char* m){ Push(54,S(n),S(m)); return reinterpret_cast<RwTexture*>(textures[3].data()); }
inline uint8* PixelsOf(const void* r){ for(int k=0;k<3;++k)if(r==rasters[k].data())return pixels[k].data(); return r==rasters[3].data() ? charset.data() : nullptr; }
inline RwUInt8* __cdecl FakeRasterLock(RwRaster* r,RwUInt8 level,RwInt32 mode){ const bool f=Fail(1); Push(55,Id(r),level,uint32(mode),f); return f ? nullptr : PixelsOf(r); }
inline RwRaster* __cdecl FakeRasterUnlock(RwRaster* r){ Push(56,Id(r),Id(*reinterpret_cast<void**>(0xC3EF88))); return r; }
inline RwRaster* __cdecl FakeRasterUnlockPalette(RwRaster* r){ Push(57,Id(r),Id(*reinterpret_cast<void**>(0xC3EF8C))); return r; }
inline RwBool __cdecl FakeTextureDestroy(RwTexture* t){ Push(58,Id(t),Id(*reinterpret_cast<void**>(0xC3EF84))); return TRUE; }
inline uint32 strideExtra{}, rasterNext{};
inline RwRaster* __cdecl FakeRasterCreate(RwInt32 w,RwInt32 h,RwInt32 d,RwInt32 fl){ const bool f=Fail(0); Push(59,uint32(w),uint32(h),uint32(d),uint32(fl)|(f ? 0x80000000u : 0));
    if(f)return nullptr; auto* r=rasters[rasterNext++%3].data(); At<int32>(r,0x18)=strideExtra==99 ? 0 : w*4+int32(strideExtra); return reinterpret_cast<RwRaster*>(r); }
inline RwBool __cdecl FakeRasterDestroy(RwRaster* r){ Push(60,Id(r)); return TRUE; }
inline uint32 texNext{};
inline RwTexture* __cdecl FakeTextureCreate(RwRaster* r){ const bool f=Fail(2); Push(61,Id(r),f); if(f)return nullptr; auto* t=textures[texNext++%3].data(); At<void*>(t,0)=r; return reinterpret_cast<RwTexture*>(t); }
inline RwTexture* __cdecl FakeTextureSetName(RwTexture* t,const char* n){ Push(62,Id(t),S(n),n ? uint32(strlen(n)) : 0); return t; }
inline RpGeometry* __cdecl FakeForAllMaterials(RpGeometry* g,RpMaterialCallBack cb,void* data){
    Push(63,Id(g),Id(data)); int k=0; for(;k<4;++k)if(g==reinterpret_cast<void*>(geoms[k].data()))break;
    if(k<4)for(int m=0;m<geomMatCount[k];++m)if(!cb(reinterpret_cast<RpMaterial*>(mats[m].data()),data))break; return g; }
inline RpMaterial* __cdecl FakeMatSetTexture(RpMaterial* m,RwTexture* t){ Push(64,Id(m),Id(t)); At<void*>(m,0)=t; return m; }
inline uint32 matNext{};
inline RpMaterial* __cdecl FakeMatCreate(){ const bool f=Fail(3); Push(65,f); return f ? nullptr : reinterpret_cast<RpMaterial*>(mats[matNext++%6].data()); }
inline RwBool __cdecl FakeMatDestroy(RpMaterial* m){ Push(66,Id(m)); return TRUE; }
inline RpGeometry* __cdecl FakeGeomCreate(RwInt32 nv,RwInt32 nt,RwUInt32 fl){ const bool f=Fail(4); Push(67,uint32(nv),uint32(nt),fl,f); return f ? nullptr : reinterpret_cast<RpGeometry*>(geoms[2].data()); }
inline const RpGeometry* __cdecl FakeTriSetVerts(const RpGeometry* g,RpTriangle* t,RwUInt16 a,RwUInt16 b,RwUInt16 c){ Push(68,Id(t),a,b,c); return g; }
inline RpGeometry* __cdecl FakeTriSetMat(RpGeometry* g,RpTriangle* t,RpMaterial* m){ Push(69,Id(t),Id(m)); return g; }
inline const RpMorphTarget* __cdecl FakeCalcSphere(const RpMorphTarget* mt,RwSphere* s){ Push(70,Id(mt),H(verts.data(),verts.size())); s->center={std::bit_cast<float>(At<uint32>(verts.data(),0)),1.f,2.f}; s->radius=1.5f; return mt; }
inline RpGeometry* __cdecl FakeGeomUnlock(RpGeometry* g){ Push(71,Id(g)); return g; }
inline RpAtomic* __cdecl FakeAtomicCreate(){ const bool f=Fail(5); Push(72,f); return f ? nullptr : reinterpret_cast<RpAtomic*>(atomics[2].data()); }
inline RpAtomic* __cdecl FakeAtomicSetGeom(RpAtomic* a,RpGeometry* g,RwUInt32 fl){ const bool f=Fail(6); Push(73,Id(a),Id(g),fl,f); return f ? nullptr : a; }
inline RwBool __cdecl FakeGeomDestroy(RpGeometry* g){ Push(74,Id(g)); return TRUE; }
inline RwFrame* __cdecl FakeFrameCreate(){ Push(75); return reinterpret_cast<RwFrame*>(frames[1].data()); }
inline RwFrame* __cdecl FakeFrameSetIdentity(RwFrame* f){ Push(76,Id(f)); return f; }
inline RpAtomic* __cdecl FakeAtomicSetFrame(RpAtomic* a,RwFrame* f){ Push(77,Id(a),Id(f)); return a; }
inline RpAtomic* __cdecl FakeAtomicRender(RpAtomic* a){ Push(78,Id(a),mats[0][7],mats[1][7]); return a; }
inline RwBool __cdecl FakeRenderState(int32 s,void* v){ Push(79,uint32(s),U(v)); return TRUE; }
inline RpAtomic* __cdecl FakeCreateA(float w,float h,int32 n,char* l1,char* l2,char* l3,char* l4,int32 letters,uint8 pal){
    Push(80,F(w),F(h),uint32(n),uint32(letters)); Push(81,pal);
    for(char* l:{l1,l2,l3,l4})Push(82,Known(l) ? Id(l) : 0xD0D0D0D0u,l ? H(l,16) : 0); return reinterpret_cast<RpAtomic*>(atomics[3].data()); }

// --- textos ---
inline std::vector<uint8> file; inline size_t filePos{}; inline uint32 readCalls{}; inline uint64 readHash{};
alignas(16) inline std::array<uint8,16> fileTagObj{};
inline FILESTREAM FileTag(){ return reinterpret_cast<FILESTREAM>(fileTagObj.data()); }
inline FILESTREAM openRet{};
inline void __cdecl FakeClearMessages(bool b){ Push(90,b); }
inline int32 __cdecl FakeSetDir(const char* p){ Push(91,S(p)); return 0; }
inline int32 __cdecl FakeSetDirMyDocuments(){ Push(92); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ Push(93,S(p),S(m)); filePos=0; return openRet; }
inline size_t __cdecl FakeRead(FILESTREAM f,void* buf,size_t n){
    size_t got=0; if(f==FileTag()){ got=std::min(n,file.size()>filePos ? file.size()-filePos : size_t(0)); std::memcpy(buf,file.data()+filePos,got); filePos+=got; }
    ++readCalls; const uint32 v[3]{uint32(n),uint32(filePos),uint32(got)}; readHash=Fnv(v,sizeof(v),readHash); return got; }
inline bool __cdecl FakeSeek(FILESTREAM f,long off,int32 origin){ Push(94,f==FileTag(),uint32(off),uint32(origin)); if(f==FileTag()){ const long base=origin==1 ? long(filePos) : origin==2 ? long(file.size()) : 0; filePos=size_t(std::max(0L,base+off)); } return true; }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ Push(95,f==FileTag() ? 1u : U(f)); return 0; }
inline void __cdecl FakeSuspend(){ Push(96); }
inline void __cdecl FakeResume(){ Push(97); }
alignas(16) inline std::array<uint8,0x400> tdatMain{}, tdatMission{};
alignas(16) inline std::array<uint8,64*8> keysMain{}, keysMission{};
alignas(16) inline std::array<uint8,300> outName{};
inline uint32 offsetVar{};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurAddress(ReversibleHooks::ReversibleHook::Base* h) {
    if(!h)return 0;
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return 0;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,45> fns{{
    {"CAtomicModelInfo","AsAtomicModelInfoPtr",0,0x85BBF0,1,16},{"CAtomicModelInfo","GetModelType",0,0x85BBF0,4,16},
    {"CAtomicModelInfo","Init",0,0x85BBF0,6,64},{"CAtomicModelInfo","DeleteRwObject",0,0x85BBF0,8,256},
    {"CAtomicModelInfo","GetRwModelType",0,0x85BBF0,9,16},{"CAtomicModelInfo","SetAtomic",0,0x85BBF0,15,512},
    {"CAtomicModelInfo","GetAtomicFromDistance",0x4C44B0,0,-1,1024},{"CAtomicModelInfo","SetupVehicleUpgradeFlags",0x4C4570,0,-1,1024},
    {"CAtomicModelInfo","SetAtomicModelInfoFlags",0x5B3B20,0,-1,1024},
    {"CColModel","operator new",0x40FC30,0,-1,256},{"CColModel","operator delete",0x40FC40,0,-1,256},{"CColModel","operator=",0x40F7C0,0,-1,256},
    {"CColModel","MakeMultipleAlloc",0x40F740,0,-1,128},{"CColModel","AllocateData-void",0x40F810,0,-1,64},{"CColModel","AllocateData-params",0x40F870,0,-1,512},
    {"CColModel","RemoveCollisionVolumes",0x40F9E0,0,-1,256},{"CColModel","CalculateTrianglePlanes",0x40FA30,0,-1,64},{"CColModel","RemoveTrianglePlanes",0x40FA40,0,-1,128},
    {"CCustomRoadsignMgr","Initialise",0x6FE120,0,-1,64},{"CCustomRoadsignMgr","Shutdown",0x6FE180,0,-1,64},{"CCustomRoadsignMgr","RoadsignGenerateTextRaster",0x6FEB70,0,-1,512},
    {"CCustomRoadsignMgr","CreateRoadsignTexture",0x6FECA0,0,-1,256},{"CCustomRoadsignMgr","SetupRoadsignAtomic",0x6FED60,0,-1,128},{"CCustomRoadsignMgr","SetAtomicAlpha",0x6FE240,0,-1,128},
    {"CCustomRoadsignMgr","CreateRoadsignAtomicA",0x6FEDA0,0,-1,512},{"CCustomRoadsignMgr","CreateRoadsignAtomic",0x6FF2D0,0,-1,256},{"CCustomRoadsignMgr","RenderRoadsignAtomic",0x6FF350,0,-1,1024},
    {"CPedModelInfo","GetModelType",0,0x85BDC0,4,16},{"CPedModelInfo","DeleteRwObject",0,0x85BDC0,8,256},{"CPedModelInfo","SetClump",0,0x85BDC0,16,256},
    {"CPedModelInfo","AddXtraAtomics",0x4C6D40,0,-1,64},{"CPedModelInfo","SetFaceTexture",0x4C6D50,0,-1,64},{"CPedModelInfo","CreateHitColModelSkinned",0x4C6D90,0,-1,256},
    {"CPedModelInfo","AnimatePedColModelSkinned",0x4C6F70,0,-1,256},{"CPedModelInfo","AnimatePedColModelSkinnedWorld",0x4C7170,0,-1,256},{"CPedModelInfo","IncrementVoice",0x4C7300,0,-1,1024},
    {"CText","Constructor",0x6A00F0,0,-1,64},{"CText","Destructor",0x6A0140,0,-1,128},{"CText","Unload",0x69FF20,0,-1,128},
    {"CText","Get",0x6A0050,0,-1,1024},{"CText","GetNameOfLoadedMissionText",0x69FBD0,0,-1,256},{"CText","ReadChunkHeader",0x69F940,0,-1,512},
    {"CText","Load",0x6A01A0,0,-1,256},{"CText","LoadMissionText",0x69FBF0,0,-1,512},{"CText","LoadMissionPackText",0x69F9A0,0,-1,256},
}};

constexpr uintptr_t TEXT_ADDR=0xC1B340, TEXT_LEN=0xA90, GXT_ERR=0xC1AEB8, ASCII_BUF=0xC1BDD0, MPG_ADDR=0xB72910, LANG_ADDR=0xBA67CC;
constexpr uintptr_t LOD_MULT=0xB6F118, WMAT_ADDR=0xB72920, COL_POOL=0xB744A4, RS_GLOBALS=0xC3EF84, PLUGIN_OFF=0xC3A1E0, ENGINE=0xC97B24;

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"models9-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"models9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("models9");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"models9-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,45> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t clumpDeleteOur=OurAddress(FindHook("CClumpModelInfo","DeleteRwObject")), clumpSetOur=OurAddress(FindHook("CClumpModelInfo","SetClump"));
    if(!clumpDeleteOur||!clumpSetOur){Write({{"event","error"},{"reason","dependency hook not found"},{"clump_delete",clumpDeleteOur},{"clump_set",clumpSetOur}});return;}
    // CBaseModelInfo::SetBaseModelInfoFlags se registra con RH_ScopedInstall sobre la funcion miembro (thiscall) para
    // una funcion cdecl del EXE que ademas debe preservar EDX: el SetAtomicModelInfoFlags original se rompe si ese hook
    // esta activo. Se desactiva solo durante esas llamadas (las dos ejecuciones usan la funcion del EXE o la de la DLL directa).
    auto* baseFlagsHook=FindHook("CBaseModelInfo","SetBaseModelInfoFlags");
    const int32 pluginOff=*reinterpret_cast<int32*>(PLUGIN_OFF);
    if(pluginOff<0||pluginOff>0x1F0){Write({{"event","error"},{"reason","2dfx plugin offset out of range"},{"offset",pluginOff}});return;}
    std::array<bool,45> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // registro de buffers
    nregs=0; Reg(newArena.data(),newArena.size()); Reg(fixArr.data(),sizeof(fixArr)); Reg(mArena.data(),mArena.size()); Reg(poolHdr.data(),poolHdr.size());
    Reg(poolStorage.data(),poolStorage.size()); Reg(poolStates.data(),poolStates.size()); Reg(cmA.data(),0x30); Reg(cmB.data(),0x30); Reg(cdA.data(),0x30); Reg(cdB.data(),0x30);
    Reg(spheres.data(),spheres.size()); Reg(obj.data(),obj.size());
    for(auto& b:atomics)Reg(b.data(),b.size()); for(auto& b:frames)Reg(b.data(),b.size()); for(auto& b:clumps)Reg(b.data(),b.size()); for(auto& b:geoms)Reg(b.data(),b.size());
    for(auto& b:plugins)Reg(b.data(),b.size()); for(auto& b:mats)Reg(b.data(),b.size()); for(auto& b:rasters)Reg(b.data(),b.size()); for(auto& b:pixels)Reg(b.data(),b.size());
    Reg(charset.data(),charset.size()); for(auto& b:textures)Reg(b.data(),b.size()); Reg(morph.data(),morph.size()); Reg(verts.data(),verts.size()); Reg(tris.data(),tris.size());
    Reg(prelit.data(),prelit.size()); Reg(uvs.data(),uvs.size()); Reg(hier.data(),hier.size()); Reg(nodes.data(),nodes.size()); Reg(matArr.data(),matArr.size());
    Reg(wm1.data(),64); Reg(wm2.data(),64); for(auto& b:lines)Reg(b.data(),b.size()); Reg(tdatMain.data(),tdatMain.size()); Reg(tdatMission.data(),tdatMission.size());
    Reg(keysMain.data(),keysMain.size()); Reg(keysMission.data(),keysMission.size()); Reg(outName.data(),outName.size()); Reg(&offsetVar,4); Reg(fileTagObj.data(),16);
    Reg(reinterpret_cast<void*>(TEXT_ADDR),TEXT_LEN); Reg(reinterpret_cast<void*>(GXT_ERR),32);

    // regiones globales guardadas
    const std::vector<std::pair<uintptr_t,size_t>> regions{{TEXT_ADDR,TEXT_LEN},{GXT_ERR,32},{ASCII_BUF,256},{MPG_ADDR,1},{LANG_ADDR,1},{LOD_MULT,4},{WMAT_ADDR,8},{COL_POOL,4},{RS_GLOBALS,12}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.emplace_back(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n);
    auto* engine=*reinterpret_cast<uint8**>(ENGINE); auto& rsSlot=At<void*>(engine,0x20); void* const rsSaved=rsSlot;
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); rsSlot=rsSaved; };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return rsSlot==rsSaved; };

    // nodos de colision de peaton del EXE (para la jerarquia falsa)
    std::array<int32,13> boneIds{}; for(int k=0;k<12;++k)boneIds[k]=*reinterpret_cast<int32*>(0x8A630C+0x1C*k); boneIds[12]=3;

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
            auto rotm=[&](uint8* m,float spread){ const double a=uni(-3.14f,3.14f), b=uni(-1.5f,1.5f), c=uni(-3.14f,3.14f);
                const double ca=std::cos(a),sa=std::sin(a),cb=std::cos(b),sb=std::sin(b),cc=std::cos(c),sc=std::sin(c);
                const double r[3][3]{{ca*cb,ca*sb*sc-sa*cc,ca*sb*cc+sa*sc},{sa*cb,sa*sb*sc+ca*cc,sa*sb*cc-ca*sc},{-sb,cb*sc,cb*cc}};
                for(int q=0;q<3;++q)for(int k=0;k<3;++k)At<float>(m,0x10*q+4*k)=float(r[q][k]);
                At<uint32>(m,0xC)=next()%2 ? 3u : 0u; At<uint32>(m,0x1C)=0; At<uint32>(m,0x2C)=0; At<uint32>(m,0x3C)=0;
                for(int k=0;k<3;++k)At<float>(m,0x30+4*k)=uni(-spread,spread); };
            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x99u; if(!stt)stt=1;
                nrec=0; newTop=0; mTop=0; recordNew=true; readCalls=0; readHash=0; filePos=0;
                for(int k=0;k<8;++k){ failMask[k]=next()%3==0 ? next()&next() : 0; failCalls[k]=0; }
                rasterNext=texNext=matNext=0;
                for(auto* h:hooks)h->State(candidate);
                const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                json out={{"switches",switches}}; json ret=nullptr;
                fill(newArena.data(),0x100); fill(mArena.data(),0x200);

                // pool falso de CColModel (se cambia el puntero del EXE)
                auto setupPool=[&](bool needFree){
                    fill(poolStorage.data(),poolStorage.size());
                    for(auto& s:poolStates)s=uint8((next()&0x7F)|(next()%5<2 ? 0x80 : 0));
                    if(needFree)poolStates[next()%NPOOL]|=0x80;
                    At<void*>(poolHdr.data(),0)=poolStorage.data(); At<void*>(poolHdr.data(),4)=poolStates.data(); At<int32>(poolHdr.data(),8)=NPOOL;
                    At<int32>(poolHdr.data(),0xC)=int32(next()%(NPOOL+1))-1; poolHdr[0x10]=0; poolHdr[0x11]=0;
                    *reinterpret_cast<void**>(COL_POOL)=poolHdr.data(); };
                auto poolOut=[&]{ out["pool_hdr"]=At<int32>(poolHdr.data(),0xC); out["pool_states"]=ReversingProbe::Bytes(poolStates.data(),NPOOL); out["pool_storage"]=ReversingProbe::Bytes(poolStorage.data(),poolStorage.size()); };

                if(cls=="CColModel") {
                    ColStubs cs;
                    fill(cmA.data(),0x30); fill(cmB.data(),0x30); fill(cdA.data(),0x30); fill(cdB.data(),0x30);
                    auto* cm=cmA.data();
                    if(nm=="operator new"){ setupPool(next()%4!=0); void* r; { NewStubs ns; r=reinterpret_cast<void*(__cdecl*)(uint32)>(a)(0x30); } ret=Id(r); poolOut(); }
                    else if(nm=="operator delete"){ setupPool(false); void* p=poolStorage.data()+0x30*(next()%NPOOL); { NewStubs ns; reinterpret_cast<void(__cdecl*)(void*)>(a)(p); } poolOut(); }
                    else if(nm=="operator="){ At<void*>(cm,0x2C)=next()%4 ? cdA.data() : nullptr; At<void*>(cmB.data(),0x2C)=cdB.data();
                        void* r; { NewStubs ns; r=reinterpret_cast<void*(__thiscall*)(void*,const void*)>(a)(cm,cmB.data()); } ret=Id(r); }
                    else if(nm=="MakeMultipleAlloc"){ cm[0x29]=uint8(next()%4 ? cm[0x29]|2 : cm[0x29]&~2); At<void*>(cm,0x2C)=cdA.data();
                        { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(cm); } out["arena"]=ReversingProbe::Bytes(newArena.data(),0x40); }
                    else if(nm=="AllocateData-void"){ { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(cm); } out["arena"]=ReversingProbe::Bytes(newArena.data(),0x40); }
                    else if(nm=="AllocateData-params"){ auto cnt=[&](uint32 m){ return next()%5==0 ? 0 : int32(next()%m); };
                        const int32 ns_=cnt(21), nb=cnt(11), nl=cnt(11), nv=cnt(41), nt=cnt(31); const bool disks=next()%2;
                        { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*,int32,int32,int32,int32,int32,bool)>(a)(cm,ns_,nb,nl,nv,nt,disks); }
                        out["malloc"]=ReversingProbe::Bytes(mArena.data(),0x40); }
                    else if(nm=="RemoveCollisionVolumes"){ At<void*>(cm,0x2C)=next()%4 ? cdA.data() : nullptr; { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(cm); } }
                    else if(nm=="CalculateTrianglePlanes"){ At<void*>(cm,0x2C)=next()%4 ? cdA.data() : nullptr; { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(cm); } }
                    else { At<void*>(cm,0x2C)=next()%4 ? cdA.data() : nullptr; At<void*>(cdA.data(),0x1C)=next()%3 ? mArena.data()+0x100 : nullptr;
                        { NewStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(cm); } }
                    out["cm"]=ReversingProbe::Bytes(cmA.data(),0x30); out["cd"]=ReversingProbe::Bytes(cdA.data(),0x30);
                }
                else if(cls=="CAtomicModelInfo") {
                    EntryPatch p1(bit_cast<uintptr_t>(&CBaseModelInfo::AddTexDictionaryRef),FakeAddTxdRef), p1e(0x4C4B80,FakeAddTxdRef);
                    EntryPatch p2(bit_cast<uintptr_t>(&CBaseModelInfo::RemoveTexDictionaryRef),FakeRemoveTxdRef), p2e(0x4C4B90,FakeRemoveTxdRef);
                    EntryPatch p3(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef),FakeAddAnimRef), p3e(0x4D3FB0,FakeAddAnimRef);
                    EntryPatch p4(bit_cast<uintptr_t>(&CAnimManager::RemoveAnimBlockRef),FakeRemoveAnimRef), p4e(0x4D3FD0,FakeRemoveAnimRef);
                    EntryPatch p5(0x749DC0,FakeAtomicDestroy), p6(0x7F05A0,FakeFrameDestroy);
                    EntryPatch p7(bit_cast<uintptr_t>(&CCustomBuildingRenderer::IsCBPCPipelineAttached),FakeIsCBPC), p7e(0x5D7F40,FakeIsCBPC);
                    EntryPatch p8(bit_cast<uintptr_t>(&CCustomBuildingRenderer::AtomicSetup),FakeAtomicSetup), p8e(0x5D7F00,FakeAtomicSetup);
                    EntryPatch p9(bit_cast<uintptr_t>(&CCarFXRenderer::IsCCPCPipelineAttached),FakeIsCCPC), p9e(0x5D5B80,FakeIsCCPC);
                    EntryPatch p10(bit_cast<uintptr_t>(&CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB),FakeSetCustomFX), p10e(0x5D5B60,FakeSetCustomFX);
                    EntryPatch p11(bit_cast<uintptr_t>(&CTagManager::SetupAtomic),FakeTagSetup), p11e(0x49CE10,FakeTagSetup);
                    // vtable: copia de la del EXE (ya con el estado de hooks de esta ejecucion) con GetAnimFileIndex trazado
                    std::memcpy(vtCopy.data(),reinterpret_cast<void*>(0x85BBF0),16*4); vtCopy[14]=uintptr_t(&FakeGetAnimFileIndex);
                    fill(obj.data(),obj.size()); At<void*>(obj.data(),0)=vtCopy.data();
                    animIndex=next()%3 ? -1 : int32(next()%200); cbpc=next()%3==0; ccpc=next()%3==0;
                    for(int k=0;k<2;++k){ auto* at=atomics[k].data(); fill(at,0x80); at[0]=1; At<void*>(at,4)=frames[k].data(); At<void*>(at,0x18)=geoms[k].data();
                        fill(geoms[k].data(),0x200); fill(plugins[k].data(),0x10); At<uint32>(plugins[k].data(),0)=next()%21;
                        At<void*>(geoms[k].data(),size_t(pluginOff))=next()%5 ? plugins[k].data() : nullptr; }
                    At<void*>(obj.data(),0x1C)=next()%4 ? atomics[0].data() : nullptr;
                    if(nm=="AsAtomicModelInfoPtr")ret=Id(reinterpret_cast<void*(__thiscall*)(void*)>(a)(obj.data()));
                    else if(nm=="GetModelType")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(obj.data());
                    else if(nm=="GetRwModelType")ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(obj.data());
                    else if(nm=="Init")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                    else if(nm=="DeleteRwObject")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                    else if(nm=="SetAtomic")reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(obj.data(),atomics[1].data());
                    else if(nm=="GetAtomicFromDistance"){ auto& lod=*reinterpret_cast<float*>(LOD_MULT); lod=next()%16==0 ? std::bit_cast<float>(0x7FC00000u) : uni(0.3f,3.f);
                        At<float>(obj.data(),0x18)=next()%16==0 ? std::bit_cast<float>(0x7FC00000u) : uni(0.f,600.f);
                        float d=next()%16==0 ? std::bit_cast<float>(0x7FC00000u) : uni(0.f,1200.f); if(next()%8==0)d=lod*At<float>(obj.data(),0x18);
                        ret=Id(reinterpret_cast<void*(__thiscall*)(void*,float)>(a)(obj.data(),d)); }
                    else if(nm=="SetupVehicleUpgradeFlags"){ static const char* pre[]{"bnt_","bntl_","bntr_","spl_","wg_l_","wg_r_","fbb_","bbb_","lgt_","rf_","nto_","hydralics","stereo",
                            "chss_","wheel_","exh_","fbmp_","rbmp_","misc_a_","misc_b_","misc_c_","wg_r","bnt","exh","door_lf","boot_dummy",""};
                        static char nameBuf[32]; const char* p=pre[next()%std::size(pre)]; std::snprintf(nameBuf,sizeof(nameBuf),"%s%s",p,next()%2 ? "" : (next()%2 ? "ok" : "_1"));
                        if(next()%10==0){ for(int k=0;k<6;++k)nameBuf[k]=char('a'+next()%26); nameBuf[6]=0; }
                        At<uint16>(obj.data(),0x12)=uint16(next()); if(next()%3)At<uint16>(obj.data(),0x12)&=0x7FFF;
                        reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(obj.data(),nameBuf); }
                    else { uint32 fl=next(); if(next()%2)fl&=next();
                        const bool bh=baseFlagsHook&&baseFlagsHook->Hooked(), bl=baseFlagsHook&&baseFlagsHook->Locked();
                        if(baseFlagsHook){ baseFlagsHook->LockState(false); baseFlagsHook->State(false); }
                        reinterpret_cast<void(__cdecl*)(void*,uint32)>(a)(obj.data(),fl);
                        if(baseFlagsHook){ baseFlagsHook->State(bh); baseFlagsHook->LockState(bl); } }
                    out["obj"]=ReversingProbe::Bytes(obj.data()+4,0x1C);
                }
                else if(cls=="CPedModelInfo") {
                    ColStubs cs;
                    EntryPatch q1(clumpDeleteOur,FakeClumpDelete), q1e(0x4C4E70,FakeClumpDelete);
                    EntryPatch q2(clumpSetOur,FakeClumpSetClump), q2e(0x4C4F70,FakeClumpSetClump);
                    EntryPatch q3(bit_cast<uintptr_t>(&CClumpModelInfo::SetFrameIds),FakeSetFrameIds), q3e(0x4C5460,FakeSetFrameIds);
                    EntryPatch q4(0x749B70,FakeForAllAtomics);
                    EntryPatch q5(bit_cast<uintptr_t>(&GetAnimHierarchyFromClump),FakeHierFromClump), q5e(0x734B10,FakeHierFromClump);
                    EntryPatch q6(bit_cast<uintptr_t>(&GetAnimHierarchyFromSkinClump),FakeHierFromSkin), q6e(0x734A40,FakeHierFromSkin);
                    fill(obj.data(),obj.size()); At<uintptr_t>(obj.data(),0)=0x85BDC0;
                    auto* cl=clumps[0].data(); fill(cl,0x40); cl[0]=2; At<void*>(cl,4)=frames[0].data(); fill(frames[0].data(),0xA0); rotm(frames[0].data()+0x10,2000.f);
                    At<void*>(obj.data(),0x1C)=next()%4 ? cl : nullptr;
                    // jerarquia HAnim falsa con matrices propias
                    fill(hier.data(),hier.size()); const int32 nNodes=16+int32(next()%9); At<int32>(hier.data(),4)=nNodes;
                    At<void*>(hier.data(),8)=matArr.data()+64; At<void*>(hier.data(),0x10)=nodes.data(); hierResult=hier.data();
                    { std::array<int32,32> ids{}; for(int k=0;k<32;++k)ids[k]=k<13 ? boneIds[k] : int32(100+next()%400);
                      for(int k=nNodes-1;k>0;--k)std::swap(ids[k],ids[next()%(k+1)]);
                      if(next()%8==0)for(int k=0;k<nNodes;++k)if(ids[k]==boneIds[next()%13])ids[k]=999;
                      for(int k=0;k<32;++k){ At<int32>(nodes.data(),16*k)=ids[k]; At<int32>(nodes.data(),16*k+4)=k; At<int32>(nodes.data(),16*k+8)=0; At<void*>(nodes.data(),16*k+12)=nullptr; } }
                    for(int k=0;k<33;++k)rotm(matArr.data()+64*k,80.f);
                    fill(wm1.data(),64); fill(wm2.data(),64); rotm(wm1.data(),5.f); rotm(wm2.data(),5.f);
                    reinterpret_cast<void**>(WMAT_ADDR)[0]=wm1.data(); reinterpret_cast<void**>(WMAT_ADDR)[1]=wm2.data();
                    const bool create=nm=="CreateHitColModelSkinned"; setupPool(true);
                    // modelo de colision existente (en el pool falso) con 12 esferas
                    auto* hit=poolStorage.data()+0x30*(next()%NPOOL); fill(cdA.data(),0x30); fill(spheres.data(),spheres.size()); At<void*>(cdA.data(),8)=spheres.data();
                    At<void*>(hit,0x2C)=next()%5 ? cdA.data() : nullptr;
                    At<void*>(obj.data(),0x34)=create||next()%4==0 ? nullptr : hit;
                    if(nm=="DeleteRwObject"&&next()%2)At<void*>(obj.data(),0x34)=hit;
                    if(nm=="DeleteRwObject"&&At<void*>(hit,0x2C)==nullptr&&next()%2)At<void*>(hit,0x2C)=cdA.data();
                    if(nm=="AnimatePedColModelSkinned"||nm=="AnimatePedColModelSkinnedWorld"||nm=="SetClump")if(At<void*>(obj.data(),0x34))At<void*>(hit,0x2C)=cdA.data();
                    if(nm=="GetModelType")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(obj.data());
                    else if(nm=="IncrementVoice"){ auto v=[&]{ return next()%6==0 ? int16(-int(next()%3)-1) : int16(next()%40); };
                        At<int16>(obj.data(),0x3E)=v(); At<int16>(obj.data(),0x40)=next()%5==0 ? v() : int16(At<int16>(obj.data(),0x3E)+int16(next()%10)); At<int16>(obj.data(),0x42)=next()%8==0 ? int16(next()) : v();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); }
                    else if(nm=="AddXtraAtomics"||nm=="SetFaceTexture")reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(obj.data(),next()%2 ? cl : nullptr);
                    else { void* r=nullptr; { NewStubs ns;
                        if(nm=="DeleteRwObject")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="SetClump")reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(obj.data(),cl);
                        else if(create)reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(obj.data(),cl);
                        else r=reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(obj.data(),cl); }
                        ret=Id(r); poolOut(); out["cd"]=ReversingProbe::Bytes(cdA.data(),0x30); out["spheres"]=ReversingProbe::Bytes(spheres.data(),spheres.size());
                        out["malloc"]=ReversingProbe::Bytes(mArena.data(),0x40+12*0x14); for(auto* m:{wm1.data(),wm2.data()})for(size_t pd:{0x1Cu,0x2Cu,0x3Cu})At<uint32>(m,pd)=0; /* relleno de RwMatrix: basura de pila de las funciones RW */
                        out["wm"]=ReversingProbe::Bytes(wm1.data(),64); out["wm2"]=ReversingProbe::Bytes(wm2.data(),64); }
                    out["obj"]=ReversingProbe::Bytes(obj.data()+4,0x40);
                }
                else if(cls=="CCustomRoadsignMgr") {
                    EntryPatch r1(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FakeFindTxdSlot), r1e(0x731850,FakeFindTxdSlot);
                    EntryPatch r2(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), r2e(0x7316A0,FakePushTxd);
                    EntryPatch r3(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), r3e(0x7316B0,FakePopTxd);
                    EntryPatch r4(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), r4e(0x7319C0,FakeSetTxd);
                    EntryPatch r5(0x7F3AC0,FakeTextureRead), r6(0x7FB2D0,FakeRasterLock), r7(0x7FAEC0,FakeRasterUnlock), r8(0x7FAFF0,FakeRasterUnlockPalette), r9(0x7F3820,FakeTextureDestroy);
                    EntryPatch r10(0x7FB230,FakeRasterCreate), r11(0x7FB020,FakeRasterDestroy), r12(0x7F37C0,FakeTextureCreate), r13(0x7F38A0,FakeTextureSetName);
                    EntryPatch r14(0x74C790,FakeForAllMaterials), r15(0x74DBC0,FakeMatSetTexture), r16(0x74D990,FakeMatCreate), r17(0x74DA20,FakeMatDestroy), r18(0x74CA90,FakeGeomCreate);
                    EntryPatch r19(0x74C690,FakeTriSetVerts), r20(0x74C6C0,FakeTriSetMat), r21(0x74C200,FakeCalcSphere), r22(0x74C800,FakeGeomUnlock), r23(0x749C50,FakeAtomicCreate);
                    EntryPatch r24(0x749D40,FakeAtomicSetGeom), r25(0x74CCC0,FakeGeomDestroy), r26(0x749DC0,FakeAtomicDestroy), r27(0x7F0410,FakeFrameCreate), r28(0x7F10B0,FakeFrameSetIdentity);
                    EntryPatch r29(0x74BF20,FakeAtomicSetFrame);
                    auto** gl=reinterpret_cast<void**>(RS_GLOBALS);
                    for(auto& t:textures)fill(t.data(),t.size()); for(auto& r:rasters)fill(r.data(),r.size()); for(auto& m:mats)fill(m.data(),m.size());
                    for(auto& p:pixels)fill(p.data(),p.size()); fill(charset.data(),charset.size());
                    static const int32 strides[]{128,128,128,64,132,256,0};
                    At<void*>(textures[3].data(),0)=next()%10 ? rasters[3].data() : nullptr; At<int32>(rasters[3].data(),0x18)=strides[next()%7];
                    strideExtra=next()%6==0 ? 99 : (next()%3==0 ? 4*(next()%20) : 0);
                    gl[0]=next()%8||(nm!="Shutdown"&&nm!="Initialise") ? textures[3].data() : nullptr; /* el EXE lee pCharsetTex->raster sin comprobar */ gl[1]=next()%8 ? charset.data() : nullptr; gl[2]=next()%3==0 ? charset.data()+0x40 : nullptr;
                    txdSlotRet=int32(next()%3000)-1;
                    static const char alphabet[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789 !#$%&()*+-.:;<>?@[]^{|}~,'\"_/\\=\x80\xAA\xF0";
                    for(auto& l:lines){ const uint32 len=next()%17; for(uint32 k=0;k<20;++k)l[k]=k<len ? uint8(alphabet[next()%(sizeof(alphabet)-1)]) : 0; }
                    for(int k=0;k<4;++k){ auto* at=atomics[k].data(); fill(at,0x80); at[0]=1; At<void*>(at,4)=frames[k].data(); At<void*>(at,0x18)=geoms[k].data(); At<void*>(at,0x48)=reinterpret_cast<void*>(&FakeAtomicRender);
                        fill(frames[k].data(),0xA0); geomMatCount[k]=int(next()%4); }
                    auto* g=geoms[2].data(); fill(g,0x200); fill(morph.data(),morph.size()); fill(verts.data(),verts.size()); fill(tris.data(),tris.size()); fill(prelit.data(),prelit.size()); fill(uvs.data(),uvs.size());
                    At<void*>(g,0x2C)=tris.data(); At<void*>(g,0x30)=prelit.data(); At<void*>(g,0x34)=uvs.data(); At<void*>(g,0x5C)=morph.data(); At<void*>(morph.data(),0x14)=verts.data();
                    const auto letters=[&]{ return next()%10==0 ? int32(next()%3)-2 : int32(next()%17); };
                    if(nm=="Initialise")ret=reinterpret_cast<uint8(__cdecl*)()>(a)();
                    else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)()>(a)();
                    else if(nm=="RoadsignGenerateTextRaster"){ At<int32>(rasters[0].data(),0x18)=next()%8==0 ? 0 : int32(16*32+4*(next()%40));
                        ret=reinterpret_cast<uint8(__cdecl*)(char*,int32,void*,int32,void*)>(a)(reinterpret_cast<char*>(lines[0].data()),letters(),rasters[3].data(),int32(next()),rasters[0].data()); }
                    else if(nm=="CreateRoadsignTexture")ret=Id(reinterpret_cast<void*(__cdecl*)(char*,int32)>(a)(reinterpret_cast<char*>(lines[0].data()),letters()));
                    else if(nm=="SetupRoadsignAtomic")ret=Id(reinterpret_cast<void*(__cdecl*)(void*,char*,int32)>(a)(atomics[0].data(),reinterpret_cast<char*>(lines[1].data()),letters()));
                    else if(nm=="SetAtomicAlpha")ret=Id(reinterpret_cast<void*(__cdecl*)(void*,uint8)>(a)(atomics[0].data(),uint8(next())));
                    else if(nm=="CreateRoadsignAtomicA"||nm=="CreateRoadsignAtomic"){
                        float w=next()%12==0 ? 0.f : uni(-2.f,12.f), h=next()%12==0 ? 0.f : uni(-2.f,6.f); if(next()%40==0)w=std::bit_cast<float>(0x7FC00000u); if(next()%40==0)h=-0.f;
                        const int32 n=int32(next()%5); char* l[4]; for(int k=0;k<4;++k)l[k]=reinterpret_cast<char*>(lines[k].data());
                        if(nm=="CreateRoadsignAtomic"){ for(auto& x:l)if(next()%3==0)x=nullptr;
                            EntryPatch c1(bit_cast<uintptr_t>(&CCustomRoadsignMgr::CreateRoadsignAtomicA),FakeCreateA), c1e(0x6FEDA0,FakeCreateA);
                            ret=Id(reinterpret_cast<void*(__cdecl*)(float,float,int32,char*,char*,char*,char*,int32,uint8)>(a)(w,h,n,l[0],l[1],l[2],l[3],letters(),uint8(next()%6))); }
                        else ret=Id(reinterpret_cast<void*(__cdecl*)(float,float,int32,char*,char*,char*,char*,int32,uint8)>(a)(w,h,n,l[0],l[1],l[2],l[3],letters(),uint8(next()%6))); }
                    else { // RenderRoadsignAtomic
                        rsSlot=reinterpret_cast<void*>(&FakeRenderState); auto* fr=frames[0].data(); const CVector fp{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                        At<float>(fr,0x40)=fp.x; At<float>(fr,0x44)=fp.y; At<float>(fr,0x48)=fp.z;
                        float dist; switch(next()%6){ case 0: dist=uni(0.f,40.f); break; case 1: dist=uni(39.5f,40.5f); break; case 2: dist=uni(499.f,501.f); break; case 3: dist=uni(500.f,900.f); break; default: dist=uni(0.f,520.f); }
                        CVector dir{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float m=std::sqrt(dir.x*dir.x+dir.y*dir.y+dir.z*dir.z)+1e-6f;
                        CVector pos{fp.x+dir.x/m*dist,fp.y+dir.y/m*dist,fp.z+dir.z/m*dist}; if(next()%32==0)pos.y=std::bit_cast<float>(0x7FC00000u);
                        ret=Id(reinterpret_cast<void*(__cdecl*)(void*,const CVector&)>(a)(atomics[0].data(),pos)); rsSlot=rsSaved; }
                    out["globals"]={Id(gl[0]),Id(gl[1]),Id(gl[2])}; out["textures"]=H(textures.data(),sizeof(textures)); out["tex3"]=ReversingProbe::Bytes(textures[3].data(),0x60);
                    out["rasters"]=ReversingProbe::Bytes(rasters.data(),sizeof(rasters)); out["pixels"]={H(pixels[0].data(),pixels[0].size()),H(pixels[1].data(),pixels[1].size()),H(pixels[2].data(),pixels[2].size())};
                    out["mats"]=ReversingProbe::Bytes(mats.data(),sizeof(mats)); out["verts"]=ReversingProbe::Bytes(verts.data(),verts.size()); out["uvs"]=ReversingProbe::Bytes(uvs.data(),uvs.size());
                    out["prelit"]=ReversingProbe::Bytes(prelit.data(),prelit.size()); out["tris"]=ReversingProbe::Bytes(tris.data(),tris.size()); out["morph"]=ReversingProbe::Bytes(morph.data(),morph.size());
                    out["atomics"]=ReversingProbe::Bytes(atomics.data(),sizeof(atomics));
                }
                else if(cls=="CText") {
                    EntryPatch t1(bit_cast<uintptr_t>(&CMessages::ClearAllMessagesDisplayedByGame),FakeClearMessages), t1e(0x69EDC0,FakeClearMessages);
                    EntryPatch t2(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), t2e(0x5387D0,FakeSetDir);
                    EntryPatch t3(bit_cast<uintptr_t>(&CFileMgr::SetDirMyDocuments),FakeSetDirMyDocuments), t3e(0x538860,FakeSetDirMyDocuments);
                    EntryPatch t4(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), t4e(0x538900,FakeOpenFile);
                    EntryPatch t5(bit_cast<uintptr_t>(&CFileMgr::Read),FakeRead), t5e(0x538950,FakeRead);
                    EntryPatch t6(bit_cast<uintptr_t>(&CFileMgr::Seek),FakeSeek), t6e(0x538990,FakeSeek);
                    EntryPatch t7(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), t7e(0x5389D0,FakeCloseFile);
                    EntryPatch t8(bit_cast<uintptr_t>(&CTimer::Suspend),FakeSuspend), t8e(0x5619D0,FakeSuspend);
                    EntryPatch t9(bit_cast<uintptr_t>(&CTimer::Resume),FakeResume), t9e(0x561A00,FakeResume);
                    recordNew=false; // los registros de CheckFileEncoding (NOTSA) pueden reservar: se compara el contenido
                    auto* tt=reinterpret_cast<uint8*>(TEXT_ADDR); auto* err=reinterpret_cast<uint8*>(GXT_ERR);
                    fill(tt,TEXT_LEN); fill(err,32); fill(reinterpret_cast<void*>(ASCII_BUF),256);
                    for(auto& b:fixArr)fill(b.data(),b.size()); fill(tdatMain.data(),tdatMain.size()); fill(tdatMission.data(),tdatMission.size());
                    *reinterpret_cast<uint8*>(MPG_ADDR)=0; *reinterpret_cast<uint8*>(LANG_ADDR)=uint8(next()%5);
                    openRet=FileTag(); file.clear();
                    tt[0x20]=uint8("egisf"[next()%5]); tt[0x21]=uint8(next()%2); tt[0x22]=uint8(next()%2); tt[0x23]=uint8(next()%2);
                    // arrays iniciales de fixture (los libera Unload/Destructor/LoadMission*)
                    auto initArrays=[&]{ for(int k=0;k<4;++k){ const bool has=next()%4!=0; At<void*>(tt,8*k)=has ? fixArr[k].data() : nullptr; At<uint32>(tt,8*k+4)=has ? next()%40 : 0; } }; /* sin datos => tamano 0 (Get busca en la tabla de mision) */
                    initArrays(); At<uint16>(tt,0xA8C)=0;
                    static const char* keyNames[]{"CDERROR","FEM_OK","FEC_RED","INTRO1","SWE1_A","RYD2_01","FEH_STA","FES_DAT","GXT_X","AB","Z","LAX_BRG","CASINO9","MPNAME","LONGKEYNAME"};
                    static const char* missions[]{"INTRO1","SWEET1","RYDER2","CASINO9","LAX_BRG","A","LONGNAM8","LONGNAME10"};
                    // tabla de claves ordenada por hash y textos (bytes GXT, incluidos acentos de un byte)
                    auto buildKeys=[&](std::vector<uint8>& tkey,std::vector<uint8>& tdat,bool cderr){
                        std::vector<std::pair<uint32,uint32>> e; std::vector<std::string> used;
                        const uint32 n=1+next()%10; for(uint32 k=0;k<n;++k){ std::string key=keyNames[next()%std::size(keyNames)]; if(k==0&&cderr)key="CDERROR"; if(!cderr&&key=="CDERROR")continue;
                            if(std::find(used.begin(),used.end(),key)!=used.end())continue; used.push_back(key);
                            const uint32 off=uint32(tdat.size()); const uint32 len=next()%14; for(uint32 c=0;c<len;++c)tdat.push_back(next()%6==0 ? uint8(0x80+next()%0x7F) : uint8(' '+next()%95)); tdat.push_back(0);
                            e.push_back({CKeyGen::GetUppercaseKey(key.c_str()),off}); }
                        std::sort(e.begin(),e.end());
                        for(auto& [hash,off]:e){ for(int b=0;b<4;++b)tkey.push_back(uint8(off>>(8*b))); for(int b=0;b<4;++b)tkey.push_back(uint8(hash>>(8*b))); } };
                    auto put32=[&](std::vector<uint8>& v,uint32 x){ for(int b=0;b<4;++b)v.push_back(uint8(x>>(8*b))); };
                    auto chunk=[&](std::vector<uint8>& v,const char* magic,const std::vector<uint8>& body){ v.insert(v.end(),magic,magic+4); put32(v,uint32(body.size())); v.insert(v.end(),body.begin(),body.end()); };
                    auto junk=[&](std::vector<uint8>& v){ static const char* mg[]{"JUNK","ABCD","TKEZ","tkey","DATA"}; std::vector<uint8> b(next()%25); for(auto& x:b)x=uint8(next()); chunk(v,mg[next()%5],b); };
                    auto zero=[&](std::vector<uint8>& v){ static const char* mg[]{"TKEY","TDAT","TABL","NONE"}; chunk(v,mg[next()%4],{}); };
                    auto keyBlock=[&](std::vector<uint8>& v,bool cderr){ std::vector<uint8> tkey,tdat; buildKeys(tkey,tdat,cderr);
                        std::vector<int> order{0,1}; if(next()%3==0)order.push_back(2); if(next()%4==0)order.push_back(3); for(int k=int(order.size())-1;k>0;--k)std::swap(order[k],order[next()%(k+1)]);
                        for(int o:order){ if(o==0)chunk(v,"TKEY",tkey); else if(o==1)chunk(v,"TDAT",tdat); else if(o==2)junk(v); else zero(v); } };
                    // fixture de arrays de claves para Get
                    auto fixKeys=[&](std::array<uint8,64*8>& keys,std::array<uint8,0x400>& tdat,size_t arrOff){
                        std::vector<uint8> tkey,data; buildKeys(tkey,data,next()%2); std::memcpy(tdat.data(),data.data(),std::min(data.size(),tdat.size()));
                        const uint32 n=uint32(tkey.size()/8); for(uint32 k=0;k<n;++k){ At<void*>(keys.data(),8*k)=tdat.data()+At<uint32>(tkey.data(),8*k); At<uint32>(keys.data(),8*k+4)=At<uint32>(tkey.data(),8*k+4); }
                        At<void*>(tt,arrOff)=keys.data(); At<uint32>(tt,arrOff+4)=n; };
                    uint32 recordedHash=0; bool truncated=false;
                    if(nm=="Constructor"){ void* r; { DelStubs ns; r=reinterpret_cast<void*(__thiscall*)(void*)>(a)(tt); } ret=Id(r); }
                    else if(nm=="Destructor"){ DelStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(tt); }
                    else if(nm=="Unload"){ DelStubs ns; reinterpret_cast<void(__thiscall*)(void*,bool)>(a)(tt,next()%2!=0); }
                    else if(nm=="Get"){ fixKeys(keysMain,tdatMain,0); fixKeys(keysMission,tdatMission,0x10); *reinterpret_cast<uint8*>(MPG_ADDR)=uint8(next()%3==0 ? 1+next()%3 : 0);
                        static char key[24]; const uint32 kk=next()%10; if(kk==0)key[0]=0; else if(kk==1)std::strcpy(key," FEM_OK"); else std::strcpy(key,keyNames[next()%std::size(keyNames)]);
                        if(next()%8==0)for(char* c=key;*c;++c)*c=char(std::tolower(uint8(*c)));
                        const uint8* r; { DelStubs ns; r=reinterpret_cast<const uint8*(__thiscall*)(void*,const char*)>(a)(tt,key); }
                        ret=r==err ? json("err") : json(Id(r)); }
                    else if(nm=="GetNameOfLoadedMissionText"){ for(int k=0;k<8;++k)tt[0x24+k]=uint8('A'+next()%26); if(next()%4)tt[0x24+next()%8]=0;
                        tt[0x2C+next()%200]=0; outName.fill(0xAB); { DelStubs ns; reinterpret_cast<void(__thiscall*)(void*,char*)>(a)(tt,reinterpret_cast<char*>(outName.data())); }
                        out["out"]=ReversingProbe::Bytes(outName.data(),outName.size()); }
                    else if(nm=="ReadChunkHeader"){ file.resize(next()%13); for(auto& x:file)x=uint8(next()); filePos=0; offsetVar=next();
                        alignas(4) static std::array<uint8,8> header; header.fill(0xEE); const uint32 unk=next()%6==0 ? 1+next()%2 : 0;
                        { DelStubs ns; ret=reinterpret_cast<uint8(__thiscall*)(void*,void*,FILESTREAM,uint32*,uint32)>(a)(tt,header.data(),next()%8 ? FileTag() : nullptr,&offsetVar,unk); }
                        out["header"]=ReversingProbe::Bytes(header.data(),8); out["offset"]=offsetVar; }
                    else if(nm=="Load"){ const bool keep=next()%2; if(keep)*reinterpret_cast<uint8*>(MPG_ADDR)=uint8(next()%3==0 ? 1+next()%3 : 0);
                        file={uint8(next()),uint8(next()),8,0}; std::vector<uint8> tabl; const uint32 nt=next()%5; for(uint32 k=0;k<nt;++k){ const char* m=missions[next()%std::size(missions)]; char nb[8]{}; std::memcpy(nb,m,std::min<size_t>(8,strlen(m))); tabl.insert(tabl.end(),nb,nb+8); put32(tabl,next()%5000); }
                        std::vector<uint8> body; keyBlock(body,next()%4!=0);
                        if(next()%2){ if(next()%2)chunk(file,"TABL",tabl); file.insert(file.end(),body.begin(),body.end()); } else { file.insert(file.end(),body.begin(),body.end()); chunk(file,"TABL",tabl); }
                        { DelStubs ns; reinterpret_cast<void(__thiscall*)(void*,bool)>(a)(tt,keep); } }
                    else if(nm=="LoadMissionText"){ if(next()%10==0)*reinterpret_cast<uint8*>(MPG_ADDR)=uint8(1+next()%3);
                        file={uint8(next()),uint8(next()),8,0}; const uint32 n=next()%7; std::array<std::string,8> names; std::array<uint32,8> offs{};
                        for(uint32 k=0;k<n;++k){ names[k]=missions[next()%std::size(missions)]; offs[k]=uint32(file.size());
                            char nb[8]{}; std::memcpy(nb,names[k].data(),std::min<size_t>(8,names[k].size())); file.insert(file.end(),nb,nb+8); keyBlock(file,true); }
                        for(uint32 k=0;k<n;++k){ auto* e=tt+0x12C+12*k; std::memset(e,0,8); std::memcpy(e,names[k].data(),std::min<size_t>(8,names[k].size())); At<uint32>(e,8)=offs[k]; }
                        At<uint16>(tt,0xA8C)=uint16(n);
                        static char mission[16]; std::strcpy(mission,next()%4 ? missions[next()%std::size(missions)] : "NOSUCH");
                        { DelStubs ns; reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(tt,mission); } }
                    else { // LoadMissionPackText
                        *reinterpret_cast<uint8*>(MPG_ADDR)=uint8(1+next()%3); const uint32 o=next()%8; openRet=o==0 ? nullptr : o==1 ? reinterpret_cast<FILESTREAM>(0x80001000u) : FileTag();
                        file={uint8(next()),uint8(next()),8,0}; keyBlock(file,next()%2); if(next()%4==0){ file.resize(next()%(file.size()+1)); truncated=true; }
                        { DelStubs ns; reinterpret_cast<void(__thiscall*)(void*)>(a)(tt); } }
                    (void)recordedHash;
                    // estado de TheText: los punteros de los arrays se comparan por contenido
                    json arrays=json::array(); const uint8* datas[2]{At<uint8*>(tt,8),At<uint8*>(tt,0x18)};
                    for(int k=0;k<4;++k){ auto* p=At<uint8*>(tt,8*k); const uint32 n=At<uint32>(tt,8*k+4);
                        if(!p){ arrays.push_back(nullptr); continue; }
                        if(uintptr_t(p)>=uintptr_t(fixArr.data())&&uintptr_t(p)<uintptr_t(fixArr.data())+sizeof(fixArr)){ arrays.push_back(json::array({"fix",Id(p)})); continue; }
                        if(truncated&&!Known(p)){ arrays.push_back(json::array({"heap",n})); continue; } /* fichero truncado: el resto del array es heap sin inicializar */
                        if(k%2==0){ json e=json::array(); const uint8* d=datas[k/2]; for(uint32 q=0;q<std::min<uint32>(n,64);++q)e.push_back({int64(At<uintptr_t>(p,8*q))-int64(uintptr_t(d)),At<uint32>(p,8*q+4)}); arrays.push_back(e); }
                        else arrays.push_back(json::array({"data",H(p,std::min<uint32>(n,0x1000))})); }
                    std::vector<uint8> rest(tt+0x20,tt+TEXT_LEN);
                    out["arrays"]=arrays; out["sizes"]={At<uint32>(tt,4),At<uint32>(tt,0xC),At<uint32>(tt,0x14),At<uint32>(tt,0x1C)}; out["text"]=H(rest.data(),rest.size());
                    out["text_head"]=ReversingProbe::Bytes(tt+0x20,0x30); out["offsets_size"]=At<uint32>(tt,0xA8C);
                    out["gxt_err"]={err[0],ReversingProbe::Bytes(err+25,7)}; /* bytes 1..24: pila sin inicializar en el EXE (Get) */
                    out["ascii"]=H(reinterpret_cast<void*>(ASCII_BUF),256); out["reads"]=readCalls; out["read_hash"]=std::to_string(readHash);
                    if(nm=="Constructor")out["text_full"]=ReversingProbe::Bytes(tt,TEXT_LEN);
                    for(int k=0;k<4;++k){ void* p=At<void*>(tt,8*k); if(p&&!Known(p))RealFree(p); } // arrays reservados por Load* en el heap real
                }
                out["ret"]=ret; out["trace"]=Recs(); restoreGlobals(); return out;
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreGlobals();
    const bool restored=globalsIntact();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
