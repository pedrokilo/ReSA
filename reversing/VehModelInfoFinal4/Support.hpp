#pragma once
// Infraestructura del banco VehModelInfoFinal4 (CVehicleModelInfo, 75 funciones): mundo RW falso, registro de trazas sin
// reservas, stubs trazados (direccion del EXE + simbolo de la DLL) y utilidades. Ver README.md.
#include <fstream>
#include <array>
#include <vector>
#include <memory>
#include <bit>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Models/VehicleModelInfo.h"
#include "Models/AtomicModelInfo.h"
#include "Models/ModelInfo.h"
#include "Animation/AnimManager.h"
#include "Fx/CarFXRenderer.h"
#include "VisibilityPlugins.h"
#include "CustomCarPlateMgr.h"
#include "LoadingScreen.h"
#include "TxdStore.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "Timer.h"
#include "General.h"
#include "RwHelper.h"
#include "Entity/Vehicle/Vehicle.h"
#include "../ProbeRuntime.hpp"

namespace VehModelInfoFinal4Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId()==owner; }

inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv64(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 S(const char* s){ return s ? (Fnv32(s,strlen(s))|1u) : 0u; }

// ---------------------------------------------------------------------------------------------------------------------
// Mundo falso. Direcciones estables (estaticas); Id() las convierte en (region, desplazamiento) para las trazas.
// ---------------------------------------------------------------------------------------------------------------------
constexpr int NMI=3, NVS=8, NCL=3, NFR=16, NNF=8, NAT=10, NNA=8, NGEO=10, NMAT=14, NTEX=8, NMX=4;
constexpr size_t MI_SIZE=0x500, VS_SIZE=0x330, FR_SIZE=0x200, AT_SIZE=0x200, GEO_SIZE=0x100, MAT_SIZE=0x80, TEX_SIZE=0x80, MESH_SIZE=0x10+0xC*8+0x10;
struct World {
    alignas(16) uint8 mi[NMI][MI_SIZE];
    alignas(16) uint8 vsPool[NVS*0x314+0x40];   // almacenamiento del pool falso de CVehicleStructure (0x314 por objeto)
    alignas(16) uint8 poolFlags[NVS+8];         // estados del pool falso
    alignas(16) uint8 poolObj[0x20];            // CPool<CVehicleStructure> falso (0x14 bytes)
    alignas(16) uint8 clump[NCL][0x40];
    alignas(16) uint8 fr[NFR][FR_SIZE];         // frames del arbol
    alignas(16) uint8 nfr[NNF][FR_SIZE];        // frames que devuelve RwFrameCreate
    alignas(16) uint8 at[NAT][AT_SIZE];         // atomics
    alignas(16) uint8 nat[NNA][AT_SIZE];        // clones que devuelve RpAtomicClone
    alignas(16) uint8 geo[NGEO][GEO_SIZE];
    alignas(16) uint8 mesh[NGEO][MESH_SIZE];
    alignas(16) uint8 mat[NMAT][MAT_SIZE];
    alignas(16) uint8 tex[NTEX][TEX_SIZE];
    alignas(16) uint8 mx[NMX][0x50];            // matrices
    alignas(16) uint8 veh[0x600];               // CVehicle falso
    alignas(16) uint8 matList[0x20];
    alignas(16) uint8 entries[16*8];
    alignas(16) uint8 dict[2][0x20];
    alignas(16) uint8 misc[0x100];
    alignas(16) uint8 txdPool[0x20];            // CPool<TxdDef> falso
    alignas(16) uint8 txdObj[4][0x10];
    alignas(16) uint8 txdFlags[8];
    alignas(16) uint8 fileTok[0x10];
    alignas(16) char names[16][40];             // nombres de frames/texturas/modelos
    alignas(16) char lineBuf[512];
    alignas(16) uint8 outVec[0x20];
    alignas(16) uint8 search[0x10];
    alignas(16) uint8 plate[0x20];
};
inline World w;
alignas(16) inline std::array<uint8,0x20000> arena{}; inline size_t arenaTop{};

inline std::array<std::array<uintptr_t,17>,NMI> vtc{};
inline uint8* Vs(int k){ return w.vsPool+0x314*size_t(k); }

// Registro de regiones para Id()
inline std::array<std::pair<uintptr_t,size_t>,48> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }

inline bool InStack(uintptr_t p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=uintptr_t(tib->StackLimit)&&p<uintptr_t(tib->StackBase); }
inline bool IsCode(uintptr_t a){ MEMORY_BASIC_INFORMATION mi{}; return a>=0x10000&&VirtualQuery(reinterpret_cast<void*>(a),&mi,sizeof(mi))&&mi.State==MEM_COMMIT&&(mi.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)); }
inline uintptr_t Final(uintptr_t a){ for(int d=0;d<4&&IsCode(a)&&*reinterpret_cast<uint8*>(a)==0xE9;++d)a=a+5+*reinterpret_cast<int32*>(a+1); return a; }
// Sigue la cadena de saltos E9 (entrada incremental -> cuerpo) SIN entrar en el EXE (un hook desactivado deja un jmp al EXE).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const auto t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; } return a; }

// Callbacks del EXE que la DLL pasa como punteros propios: se comparan por destino final.
constexpr uintptr_t kCb[]{
    0x4C7BD0,0x4C7B90,0x4C7BB0,0x4C8E30,0x4C7700,0x4C5350,0x4C52F0,0x4C77E0,0x4C7870,0x4C7930,0x4C79A0,0x4C7A30,0x4C7AA0,
    0x4C83E0,0x4C8220,0x4C83B0,0x4C77C0,0x4C8840,0x4C88B0,0x4C9410,0x4C9430,0x4C8B60,0x4C88F0,0x4C7510,
    0x7323C0,0x7331E0,0x733F80,0x733240,0x732820,0x734370,0x733420,0x7334F0,0x7344A0,0x733550,0x734240,0x733330,0x7340B0,0x734170};
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first));
    if(InStack(u))return 0xD0000000u;
    for(auto e:kCb)if(u==e)return uint32(e);
    if(u<0x1000000)return uint32(u);
    const auto fu=Final(u); for(auto e:kCb)if(fu==Final(e))return uint32(e);
    return 0xE0000000u;
}
// Valor de 32 bits que puede ser un puntero del mundo falso o un dato (color, banderas...)
inline uint32 Val(uint32 v){ const auto u=uintptr_t(v); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first)); return v; }
inline bool Known(const void* p){ const auto u=uintptr_t(p); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return true; return false; }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de trazas sin reservas
// ---------------------------------------------------------------------------------------------------------------------
struct Rec { uint32 t,a,b,c,d; };
inline std::array<Rec,8192> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(recOverflow)r.push_back("overflow"); return r; }

enum : uint32 {
    T_CFA=1,T_FFC,T_FFO,T_GFM,T_GFO,T_GFC,T_FCREATE,T_FDESTROY,T_FADD,T_FREMOVE,T_FXFORM,T_FUPD,T_FLTM,T_MCREATE,T_MDESTROY,T_MXFORM,T_MROT,T_V3D,
    T_CREMOVE,T_CADD,T_ACLONE,T_ADESTROY,T_ASETFRAME,T_MFXGET,T_MFXSET,T_MFXFRAME,T_MFXCOEF,T_MATTEX,T_MLAPP,T_MLDEINIT,
    T_TFIND,T_TCUR,T_TGETCB,T_TSETCB,T_TDESTROY,T_TREAD,T_ARCB,T_CLRCLUMP,T_FXSETUP,T_PGEN,T_PSETUP,T_QSET,
    T_CHOOSE1,T_CHOOSE2,T_ARCBS,T_RPIPE,T_PRE,T_REDUCE,T_CPLATE,T_OPEN,T_READ,T_CLOSE,T_LOADLINE,T_GMI,T_GMIR,T_UPFLAGS,T_CHUNK,
    T_FTXD,T_ATXD,T_PUSH,T_POP,T_SETTXD,T_LTXD,T_TXDREF,T_CYC,T_CYCMS,T_DIRT,T_SCR,T_ALLOC,T_DELETE,T_RAND,
    T_CINIT,T_CSETCLUMP,T_CDELETE,T_CCREATE,T_CSETIDS,T_GFID,T_LCOLS,T_LUPG,T_LENV,T_FPV,T_ANIMIDX,T_GMT,T_SCRLOCK };

// ---------------------------------------------------------------------------------------------------------------------
// Estado de fixtures leido por los stubs
// ---------------------------------------------------------------------------------------------------------------------
struct Tables {
    int clumpAtomN[NCL]; void* clumpAtom[NCL][NAT];
    int fobjN[NFR]; void* fobj[NFR][3];
    int geoMatN[NGEO]; void* geoMat[NGEO][6];
    uint32 matEff[NMAT];
    int texDict[NTEX];            // 0 = ninguno, 1 = dict[0], 2 = dict[1]
    void* currentDict; void* findCb;
    void* frameForId[8]; int frameForIdCount;
    void* cloneQueue[NNA]; int cloneCount;
    void* newFrameQueue[NNF]; int newFrameCount;
    int nextMatrix;
    void* playerVehicle;
    void* nextCreateInstanceClump;
    int32 chooseRet[2];
    void* plateMaterial;
    int plateCalls;
    uint32 modelInfoIdBase;
};
inline Tables tb;
inline uint32 rngState{1};

inline int IdxOf(const void* p,const void* base,size_t stride,int n){ const auto u=reinterpret_cast<const uint8*>(p),b=reinterpret_cast<const uint8*>(base); if(u<b||u>=b+stride*size_t(n))return -1; return int((u-b)/stride); }
inline int ClIdx(const void* p){ return IdxOf(p,w.clump,0x40,NCL); }
inline int FrIdx(const void* p){ return IdxOf(p,w.fr,FR_SIZE,NFR); }
inline int GeoIdx(const void* p){ return IdxOf(p,w.geo,GEO_SIZE,NGEO); }
inline int MatIdx(const void* p){ return IdxOf(p,w.mat,MAT_SIZE,NMAT); }
inline int TexIdx(const void* p){ return IdxOf(p,w.tex,TEX_SIZE,NTEX); }

// Arena de operator new/delete (sin reservas durante la llamada probada)
inline void* ArenaAlloc(size_t n){ const size_t off=arenaTop; if(n>arena.size()-off){ Push(T_ALLOC,uint32(n),0xFFFFFFFFu); return nullptr; } arenaTop=(off+n+15)&~size_t(15); Push(T_ALLOC,uint32(n),uint32(off)); return arena.data()+off; }

// Fichero falso (LoadVehicleColours / LoadVehicleUpgrades)
inline std::array<const char*,48> fileLines{}; inline size_t fileLineCount{}, fileLinePos{};
inline std::array<char,128> fileNames{};

// ---------------------------------------------------------------------------------------------------------------------
// Stubs (todos respetan la convencion y el 'ret N' del original)
// ---------------------------------------------------------------------------------------------------------------------
using Cb2 = void*(__cdecl*)(void*,void*);
inline void* __cdecl S_ClumpForAllAtomics(void* c,void* cb,void* d){
    if(!Mine())return c; Push(T_CFA,Id(c),Id(cb),Id(d));
    const int ci=ClIdx(c);
    if(ci>=0&&cb){ const auto fn=reinterpret_cast<Cb2>(cb); const int n=tb.clumpAtomN[ci]; for(int k=0;k<n;++k)if(!fn(tb.clumpAtom[ci][k],d))break; }
    return c; }
inline void* __cdecl S_FrameForAllChildren(void* f,void* cb,void* d){
    if(!Mine())return f; Push(T_FFC,Id(f),Id(cb),Id(d));
    if(f&&cb){ const auto fn=reinterpret_cast<Cb2>(cb); void* cur=At<void*>(f,0x98); while(cur){ void* nx=At<void*>(cur,0x9C); if(!fn(cur,d))break; cur=nx; } }
    return f; }
inline void* __cdecl S_FrameForAllObjects(void* f,void* cb,void* d){
    if(!Mine())return f; Push(T_FFO,Id(f),Id(cb),Id(d));
    const int fi=FrIdx(f);
    if(fi>=0&&cb){ const auto fn=reinterpret_cast<Cb2>(cb); const int n=tb.fobjN[fi]; void* objs[3]{tb.fobj[fi][0],tb.fobj[fi][1],tb.fobj[fi][2]}; for(int k=0;k<n;++k)if(!fn(objs[k],d))break; }
    return f; }
inline void* __cdecl S_GeometryForAllMaterials(void* g,void* cb,void* d){
    if(!Mine())return g; Push(T_GFM,Id(g),Id(cb),Id(d));
    const int gi=GeoIdx(g);
    if(gi>=0&&cb){ const auto fn=reinterpret_cast<Cb2>(cb); const int n=tb.geoMatN[gi]; for(int k=0;k<n;++k)if(!fn(tb.geoMat[gi][k],d))break; }
    return g; }
inline void* __cdecl S_GetFirstObject(void* f){
    if(!Mine())return nullptr; Push(T_GFO,Id(f)); const int fi=FrIdx(f); return fi>=0&&tb.fobjN[fi]>0 ? tb.fobj[fi][0] : nullptr; }
inline void* __cdecl S_GetFirstChild(void* f){
    if(!Mine())return nullptr; Push(T_GFC,Id(f)); return f ? At<void*>(f,0x98) : nullptr; }

inline void* __cdecl S_FrameCreate(){
    if(!Mine())return nullptr; void* r=tb.newFrameCount<NNF ? tb.newFrameQueue[tb.newFrameCount] : nullptr; ++tb.newFrameCount; Push(T_FCREATE,Id(r)); return r; }
inline int __cdecl S_FrameDestroy(void* f){ if(!Mine())return 1; Push(T_FDESTROY,Id(f)); return 1; }
inline void* __cdecl S_FrameAddChild(void* p,void* c){ if(!Mine())return p; Push(T_FADD,Id(p),Id(c)); return p; }
inline void* __cdecl S_FrameRemoveChild(void* c){ if(!Mine())return c; Push(T_FREMOVE,Id(c)); return c; }
inline void* __cdecl S_FrameTransform(void* f,const void* m,uint32 op){ if(!Mine())return f; Push(T_FXFORM,Id(f),Fnv32(m,0x40),op); return f; }
inline void* __cdecl S_FrameUpdateObjects(void* f){ if(!Mine())return f; Push(T_FUPD,Id(f)); return f; }
inline void* __cdecl S_FrameGetLTM(void* f){ if(!Mine())return nullptr; Push(T_FLTM,Id(f)); return w.mx[3]; }
inline void* __cdecl S_MatrixCreate(){ if(!Mine())return nullptr; void* r=w.mx[tb.nextMatrix&1]; ++tb.nextMatrix; Push(T_MCREATE,Id(r)); return r; }
inline int __cdecl S_MatrixDestroy(void* m){ if(!Mine())return 1; Push(T_MDESTROY,Id(m)); return 1; }
// Efecto determinista: pos += transform.pos * 0.5 (la composicion real no importa, solo que dependa de la cadena)
inline void* __cdecl S_MatrixTransform(void* m,const void* t,uint32 op){
    if(!Mine())return m; Push(T_MXFORM,Id(m),Fnv32(t,0x40),op);
    for(int k=0;k<3;++k){ const float v=At<float>(m,0x30+4*k)+At<float>(const_cast<void*>(t),0x30+4*k)*0.5f; At<float>(m,0x30+4*k)=v; }
    return m; }
inline void* __cdecl S_MatrixRotate(void* m,const void* axis,float angle,uint32 op){ if(!Mine())return m; Push(T_MROT,Id(m),Fnv32(axis,12),F(angle),op); return m; }
inline void* __cdecl S_V3dTransformPoints(void* out,const void* in,int32 n,const void* m){
    if(!Mine())return out; Push(T_V3D,Id(out),Id(in),uint32(n),Fnv32(m,0x40));
    for(int32 i=0;i<n;++i)for(int k=0;k<3;++k){ At<float>(out,12*i+4*k)=At<float>(const_cast<void*>(in),12*i+4*k)+At<float>(const_cast<void*>(m),0x30+4*k); }
    return out; }
inline void* __cdecl S_ClumpRemoveAtomic(void* c,void* a){ if(!Mine())return c; Push(T_CREMOVE,Id(c),Id(a)); return c; }
inline void* __cdecl S_ClumpAddAtomic(void* c,void* a){ if(!Mine())return c; Push(T_CADD,Id(c),Id(a)); return c; }
inline void* __cdecl S_AtomicClone(void* a){ if(!Mine())return nullptr; void* r=tb.cloneCount<NNA ? tb.cloneQueue[tb.cloneCount] : nullptr; ++tb.cloneCount; Push(T_ACLONE,Id(a),Id(r)); return r; }
inline int __cdecl S_AtomicDestroy(void* a){ if(!Mine())return 1; Push(T_ADESTROY,Id(a)); return 1; }
inline void* __cdecl S_AtomicSetFrame(void* a,void* f){ if(!Mine())return a; Push(T_ASETFRAME,Id(a),Id(f)); if(a)At<void*>(a,4)=f; return a; }

inline uint32 __cdecl S_MatFXGetEffects(void* m){ if(!Mine())return 0; const int k=MatIdx(m); Push(T_MFXGET,Id(m)); return k>=0 ? tb.matEff[k] : 0u; }
inline void* __cdecl S_MatFXSetEffects(void* m,uint32 e){ if(!Mine())return m; Push(T_MFXSET,Id(m),e); const int k=MatIdx(m); if(k>=0)tb.matEff[k]=e; return m; }
inline void* __cdecl S_MatFXSetEnvMapFrame(void* m,void* f){ if(!Mine())return m; Push(T_MFXFRAME,Id(m),Id(f)); return m; }
inline void* __cdecl S_MatFXSetEnvMapCoeff(void* m,float c){ if(!Mine())return m; Push(T_MFXCOEF,Id(m),F(c)); return m; }
inline void* __cdecl S_MaterialSetTexture(void* m,void* t){ if(!Mine())return m; Push(T_MATTEX,Id(m),Id(t)); if(m)At<void*>(m,0)=t; return m; }
inline int __cdecl S_MatListAppend(void* l,void* m){ if(!Mine())return 0; const int32 n=At<int32>(l,4); Push(T_MLAPP,Id(m),uint32(n)); At<int32>(l,4)=n+1; return n; }
inline void* __cdecl S_MatListDeinit(void* l){ if(!Mine())return l; Push(T_MLDEINIT,uint32(At<int32>(l,4)),uint32(At<int32>(l,8))); return l; }

inline void* __cdecl S_TexDictFind(void* d,const char* name){
    if(!Mine())return nullptr; Push(T_TFIND,Id(d),S(name));
    const int di=d==w.dict[0] ? 1 : d==w.dict[1] ? 2 : 0;
    if(di)for(int k=0;k<NTEX;++k)if(tb.texDict[k]==di&&!std::strncmp(reinterpret_cast<const char*>(w.tex[k])+0x10,name,32))return w.tex[k];
    return nullptr; }
inline void* __cdecl S_TexDictGetCurrent(){ if(!Mine())return nullptr; Push(T_TCUR); return tb.currentDict; }
inline void* __cdecl S_TexGetFindCB(){ if(!Mine())return nullptr; Push(T_TGETCB); return tb.findCb; }
inline int __cdecl S_TexSetFindCB(void* cb){ if(!Mine())return 1; Push(T_TSETCB,Id(cb)); return 1; }
inline int __cdecl S_TexDestroy(void* t){ if(!Mine())return 1; Push(T_TDESTROY,Id(t)); return 1; }
inline void* __cdecl S_TexRead(const char* name,const char* mask){ if(!Mine())return nullptr; Push(T_TREAD,S(name),Id(mask)); return w.tex[7]; }

inline void __cdecl S_SetAtomicRenderCB(void* a,void* cb){ if(!Mine())return; Push(T_ARCB,Id(a),Id(cb)); }
inline void __cdecl S_ClearClumpFlag(void* c,int32 id){ if(!Mine())return; Push(T_CLRCLUMP,Id(c),uint32(id)); }
inline void __cdecl S_FxClumpSetup(void* c){ if(!Mine())return; Push(T_FXSETUP,Id(c)); }
inline bool __cdecl S_PlateGenerate(char* buf,uint32 len){ if(!Mine())return true; Push(T_PGEN,S(buf),len); static const char t[]="PLATE_X"; for(uint32 k=0;k<len&&k<8;++k)buf[k]=t[k]; return true; }
inline void* __cdecl S_PlateSetupClump(void* c,char* text,uint32 type){ if(!Mine())return nullptr; Push(T_PSETUP,Id(c),S(text),type&0xFFu); ++tb.plateCalls; return tb.plateMaterial; }
inline void __fastcall S_QuatSet(void* self,void*,const void* m){ if(!Mine())return; Push(T_QSET,Id(self),Fnv32(m,0x40)); At<float>(self,0)=At<float>(const_cast<void*>(m),0x10)*0.25f; }

// Subfunciones de la propia clase (para CreateInstance / SetClump)
inline int32 __fastcall S_Choose1(void* self,void*){ if(!Mine())return -1; Push(T_CHOOSE1,Id(self)); return tb.chooseRet[0]; }
inline int32 __fastcall S_Choose2(void* self,void*){ if(!Mine())return -1; Push(T_CHOOSE2,Id(self)); return tb.chooseRet[1]; }
inline void __fastcall S_SetARCBs(void* self,void*){ if(!Mine())return; Push(T_ARCBS,Id(self)); }
inline void __fastcall S_SetRPipes(void* self,void*){ if(!Mine())return; Push(T_RPIPE,Id(self)); }
inline void __fastcall S_Preprocess(void* self,void*){ if(!Mine())return; Push(T_PRE,Id(self)); }
inline void __fastcall S_Reduce(void* self,void*){ if(!Mine())return; Push(T_REDUCE,Id(self)); }
inline void __fastcall S_SetCCPlate(void* self,void*){ if(!Mine())return; Push(T_CPLATE,Id(self)); }
// CClumpModelInfo (base)
inline void __fastcall S_ClumpInit(void* self,void*){ if(!Mine())return; Push(T_CINIT,Id(self)); }
inline void __fastcall S_ClumpSetClump(void* self,void*,void* clump){ if(!Mine())return; Push(T_CSETCLUMP,Id(self),Id(clump)); }
inline void __fastcall S_ClumpDelete(void* self,void*){ if(!Mine())return; Push(T_CDELETE,Id(self)); }
inline void* __fastcall S_ClumpCreate(void* self,void*){ if(!Mine())return nullptr; Push(T_CCREATE,Id(self)); return tb.nextCreateInstanceClump; }
inline void __fastcall S_ClumpSetIds(void* self,void*,void* table){ if(!Mine())return; Push(T_CSETIDS,Id(self),Id(table)); }
inline void* __cdecl S_GetFrameFromId(void* clump,int32 id){
    if(!Mine())return nullptr; Push(T_GFID,Id(clump),uint32(id));
    if(tb.frameForIdCount<=0)return nullptr; void* r=tb.frameForId[uint32(id)%uint32(tb.frameForIdCount)]; return r; }

// Memoria / azar / jugador
inline int __cdecl S_RandExe(){ if(!Mine())return 0x1234; rngState^=rngState<<13; rngState^=rngState>>17; rngState^=rngState<<5; const uint32 v=rngState&0x7FFF; Push(T_RAND,v); return int(v); }
inline uint16 __cdecl S_RandDll(){ return uint16(S_RandExe()); }
inline void* __cdecl S_New(size_t n){ if(!Mine())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n); return ArenaAlloc(n); }
inline void __cdecl S_Delete(void* p){ if(!Mine()){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; } Push(T_DELETE,Id(p)); }
inline void* __cdecl S_Malloc72(size_t n,uint32 hint){ (void)hint; if(!Mine())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n); return ArenaAlloc(n); }
inline void* __cdecl S_FindPlayerVehicle(int32 pid,bool remote){ if(!Mine())return nullptr; Push(T_FPV,uint32(pid),remote?1u:0u); return tb.playerVehicle; }
inline int32 __cdecl S_AnimBlockIndex(const char* n){ if(!Mine())return -1; const uint32 h= Known(n) ? S(n) : uint32(uintptr_t(n)); Push(T_ANIMIDX,h); return int32(h&0xFF)-1; }
inline void __cdecl S_InitScratch(){ if(!Mine())return; Push(T_SCR); }
inline uint32 __cdecl S_Cycles(){ if(!Mine())return 0; Push(T_CYC); return 12345u; }
inline uint32 __cdecl S_CyclesMs(){ if(!Mine())return 0; Push(T_CYCMS); return 99u; }
inline bool __cdecl S_InitDirt(){ if(!Mine())return true; Push(T_DIRT); return true; }
inline void __cdecl S_NewChunk(){ if(!Mine())return; Push(T_CHUNK); }
inline void __cdecl S_LoadColours(){ if(!Mine())return; Push(T_LCOLS); }
inline void __cdecl S_LoadUpgrades(){ if(!Mine())return; Push(T_LUPG); }
inline void __cdecl S_LoadEnv(){ if(!Mine())return; Push(T_LENV); }

// Ficheros / modelos / TXD
inline void* __cdecl S_OpenFile(const char* path,const char* mode){ if(!Mine())return nullptr; Push(T_OPEN,S(path),S(mode)); fileLinePos=0; return w.fileTok; }
inline bool __cdecl S_ReadLine(void* file,char* buf,int32 n){
    if(!Mine())return false; Push(T_READ,Id(file),uint32(n));
    if(fileLinePos>=fileLineCount)return false;
    const char* l=fileLines[fileLinePos++]; const size_t len=strlen(l); std::memcpy(buf,l,len+1); return true; }
inline int32 __cdecl S_CloseFile(void* file){ if(!Mine())return 0; Push(T_CLOSE,Id(file)); return 0; }
inline char* __cdecl S_LoadLine(void* file){
    if(!Mine())return nullptr; Push(T_LOADLINE,Id(file));
    if(fileLinePos>=fileLineCount)return nullptr;
    const char* l=fileLines[fileLinePos++]; const size_t len=strlen(l); std::memcpy(w.lineBuf,l,len+1); return w.lineBuf; }
inline uint8* ModelByName(const char* name){ return w.mi[S(name)%NMI]; }
inline void* __cdecl S_GetModelInfoName(const char* name,int32* id){ if(!Mine())return nullptr; Push(T_GMI,S(name),Id(id)); if(id)*id=int32(400+(S(name)>>4)%100); return ModelByName(name); }
inline int gmiMode{1}; inline uint8 gmiType{6};
inline void* __cdecl S_GetModelInfoRange(const char* name,int32 a,int32 b){ if(!Mine())return nullptr; Push(T_GMIR,S(name),uint32(a),uint32(b)); return gmiMode ? static_cast<void*>(w.mi[1]) : nullptr; }
inline uint8 __fastcall S_GetModelTypeV(void* self,void*){ if(!Mine())return 0; Push(T_GMT,Id(self)); return gmiType; }
inline void __fastcall S_UpgradeFlags(void* self,void*,const char* name){ if(!Mine())return; Push(T_UPFLAGS,Id(self),S(name)); }
inline int32 __cdecl S_FindTxdSlot(const char* name){ if(!Mine())return -1; Push(T_FTXD,S(name)); return std::strcmp(name,"vehicle")==0 ? (At<int32>(w.misc,0)) : std::strcmp(name,"particle")==0 ? 2 : -1; }
inline int32 __cdecl S_AddTxdSlot(const char* name){ if(!Mine())return -1; Push(T_ATXD,S(name)); return 1; }
inline void __cdecl S_PushTxd(){ if(!Mine())return; Push(T_PUSH); }
inline void __cdecl S_PopTxd(){ if(!Mine())return; Push(T_POP); }
inline void __cdecl S_SetCurrentTxd(int32 s){ if(!Mine())return; Push(T_SETTXD,uint32(s)); }
inline bool __cdecl S_LoadTxd(int32 s,const char* f){ if(!Mine())return true; Push(T_LTXD,uint32(s),S(f)); return true; }
inline void __cdecl S_TxdAddRef(int32 s){ if(!Mine())return; Push(T_TXDREF,uint32(s)); }

// ---------------------------------------------------------------------------------------------------------------------
// Hooks, direcciones de la DLL y conjuntos de parches
// ---------------------------------------------------------------------------------------------------------------------
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
enum : uint32 { G_RNG=1,G_NEW=2,G_TREE=4,G_RWOBJ=8,G_MATFX=0x10,G_TEX=0x20,G_VIS=0x40,G_CLUMP=0x80,G_OWN=0x100,G_CAR=0x200,G_FILE=0x400,G_PLAYER=0x800,G_ANIM=0x1000,
                G_LOADERS=0x2000,G_TXD=0x4000,G_SCRATCH=0x8000,G_TIMER=0x10000,G_UPG=0x20000 };
struct Stub { uint32 group; uintptr_t exe,dll,body; void* fn; };
inline std::vector<Stub> stubs;
inline void AddStub(uint32 group,uintptr_t exe,uintptr_t dllSym,void* fn){ stubs.push_back({group,exe,dllSym,dllSym ? Body(dllSym) : 0,fn}); }
// Conjunto de parches SIN reservas de memoria: los EntryPatch viven en almacenamiento estatico. Con operator new parcheado
// (G_NEW) un vector/unique_ptr consumiria la arena con bytes que dependen del lado (direcciones de salto) y ensuciaria la traza.
struct PatchSet {
    static constexpr size_t MAXP=160;
    alignas(EntryPatch) static inline unsigned char storage[MAXP][sizeof(EntryPatch)];
    static inline uintptr_t used[MAXP];
    size_t n{};
    explicit PatchSet(uint32 mask){ for(auto& s:stubs)if(s.group&mask){ Put(s.exe,s.fn); Put(s.dll,s.fn); Put(s.body,s.fn); } }
    PatchSet(const PatchSet&)=delete; PatchSet& operator=(const PatchSet&)=delete;
    void Put(uintptr_t at,void* fn){ if(!at||n>=MAXP)return; for(size_t k=0;k<n;++k)if(used[k]==at)return; used[n]=at; new(storage[n]) EntryPatch(at,fn); ++n; }
    ~PatchSet(){ while(n>0){ --n; reinterpret_cast<EntryPatch*>(storage[n])->~EntryPatch(); } }
};
inline PatchSet MakePatches(uint32 mask){ return PatchSet(mask); }
template<class P> inline void* FnPtr(P f){ return reinterpret_cast<void*>(f); }
inline bool AddHookStub(uint32 group,uintptr_t exe,const char* cls,const char* name,void* fn){
    auto* h=FindHook(cls,name); const auto our=OurAddress(h); if(!h||!our)return false;
    AddStub(group,exe,our,fn); return true;
}

// Instantanea de globales (se restaura al terminar cada ejecucion)
struct Saved { uintptr_t addr; std::vector<uint8> bytes; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }

template<class R,class... A> inline R CallThis(uintptr_t a,void* self,A... args){ return reinterpret_cast<R(__thiscall*)(void*,A...)>(a)(self,args...); }
template<class R,class... A> inline R CallCdecl(uintptr_t a,A... args){ return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }
}
