#pragma once
// Infraestructura del banco VisPluginsFinal4 (CVisibilityPlugins, 84 funciones): mundo RW falso estatico, registro de trazas sin
// reservas, stubs trazados (direccion del EXE + simbolo de la DLL), listas de alfa falsas y utilidades. Ver README.md.
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
#include "VisibilityPlugins.h"
#include "Models/BaseModelInfo.h"
#include "Models/ModelInfo.h"
#include "Entity/Entity.h"
#include "Entity/Ped/Ped.h"
#include "WeaponInfo.h"
#include "Renderer.h"
#include "World.h"
#include "PlayerInfo.h"
#include "ModelIndices.h"
#include "RwHelper.h"
#include "../ProbeRuntime.hpp"

namespace VisPluginsFinal4Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }

// Hilo del banco: otros hilos del juego (audio) llaman a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine() { return GetCurrentThreadId()==owner; }

inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline float Fl(uint32 u) { return std::bit_cast<float>(u); }
template<class T=uint32> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv64(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// ---------------------------------------------------------------------------------------------------------------------
// Mundo falso. Todo es estatico (direcciones estables); Id() convierte punteros en (region, desplazamiento) para las trazas.
// ---------------------------------------------------------------------------------------------------------------------
constexpr int NAT=10, NFR=16, NCL=4, NGEO=10, NMAT=14, NTX=4, NMI=6, NENT=10, NPED=3, NWC=3, NHX=48, NWI=4;
constexpr size_t AT_SIZE=0x200, FR_SIZE=0x200, CL_SIZE=0x100, GEO_SIZE=0x100, MAT_SIZE=0x80, TX_SIZE=0x80, MI_SIZE=0x100, COL_SIZE=0x80,
                 ENT_SIZE=0x200, PED_SIZE=0x800, WC_SIZE=0x100, WI_SIZE=0x80;
constexpr int NLISTS=5; constexpr int LCAP[NLISTS]{20,20,200,100,50}; constexpr int WCAP=100;
constexpr uintptr_t LADDR[NLISTS]{0xC88070,0xC880C8,0xC88120,0xC88178,0xC881D0};  // alfa, barcos, entidades, bajo el agua, "dibujar al final"
constexpr uintptr_t WLADDR=0xC88224;                                               // peds con arma
enum : int { L_ALPHA=0, L_BOAT=1, L_ENT=2, L_UNDER=3, L_LAST=4 };
constexpr uintptr_t GLOB_BASE=0xC88000; constexpr size_t GLOB_SIZE=0x260;           // variables y listas de la clase

struct World {
    alignas(16) uint8 at[NAT][AT_SIZE];          // atomics
    alignas(16) uint8 fr[NFR][FR_SIZE];          // frames (modelling en 0x10, LTM en 0x50)
    alignas(16) uint8 cl[NCL][CL_SIZE];          // clumps
    alignas(16) uint8 geo[NGEO][GEO_SIZE];       // geometrias
    alignas(16) uint32 geoMats[NGEO][12];        // tablas de materiales
    alignas(16) uint8 mat[NMAT][MAT_SIZE];       // materiales
    alignas(16) uint8 tex[NTX][TX_SIZE];         // texturas
    alignas(16) uint8 mi[NMI][MI_SIZE];          // model infos
    alignas(16) uint8 col[NMI][COL_SIZE];        // CColModel
    alignas(16) uint8 ent[NENT][ENT_SIZE];       // entidades
    alignas(16) uint8 ped[NPED][PED_SIZE];       // peds
    alignas(16) uint8 wc[NWC][WC_SIZE];          // clumps de armas
    alignas(16) uint8 wfr[NWC][FR_SIZE];         // frames de los clumps de armas
    alignas(16) uint8 wi[NWI][WI_SIZE];          // CWeaponInfo
    alignas(16) uint8 hmx[NHX][64];              // matrices de la jerarquia de animacion
    alignas(16) uint8 hier[0x40];                // jerarquia de animacion falsa
    alignas(16) uint8 cam[0x100];                // RwCamera falsa
    alignas(16) uint8 camFr[FR_SIZE];            // su frame (la posicion de la camara es camFr+0x40)
    alignas(16) uint8 mx[4][0x40];               // matrices sueltas
    alignas(16) uint8 vec[0x40];                 // vectores sueltos
    alignas(16) uint8 ln0[LCAP[0]*20], ln1[LCAP[1]*20], ln2[LCAP[2]*20], ln3[LCAP[3]*20], ln4[LCAP[4]*20];  // nodos de las listas de alfa
    alignas(16) uint8 lnw[WCAP*12];              // nodos de la lista de peds
    alignas(16) uint8 guard[16];                 // relleno final (detecta desbordes)
    alignas(16) uint8 scratch[2][0x200];         // objetos de plugin para PluginAttach
    alignas(16) uint32 vtEnt[32];                // vtable de entidad falsa
    alignas(16) uint32 vtPed[32];                // vtable de ped falso
    alignas(16) uint32 vtMi[17];                 // vtable de model info falso
};
inline World w;
alignas(16) inline std::array<uint8,0x20000> arena{}; inline size_t arenaTop{};
inline uint8* Nodes(int li){ switch(li){ case 0: return w.ln0; case 1: return w.ln1; case 2: return w.ln2; case 3: return w.ln3; default: return w.ln4; } }

// Offsets de plugin leidos del EXE en tiempo de ejecucion
inline int32 atomOff{}, frameOff{}, clumpOff{};

// Registro de regiones para Id()
inline std::array<std::pair<uintptr_t,size_t>,8> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }

inline bool InStack(uintptr_t p){ const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb()); return p>=uintptr_t(tib->StackLimit)&&p<uintptr_t(tib->StackBase); }
inline bool IsCode(uintptr_t a){ MEMORY_BASIC_INFORMATION mi{}; return a>=0x10000&&VirtualQuery(reinterpret_cast<void*>(a),&mi,sizeof(mi))&&mi.State==MEM_COMMIT&&(mi.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)); }
inline uintptr_t Final(uintptr_t a){ for(int d=0;d<6&&IsCode(a)&&*reinterpret_cast<uint8*>(a)==0xE9;++d)a=a+5+*reinterpret_cast<int32*>(a+1); return a; }
// Sigue la cadena de saltos E9 (entrada incremental -> cuerpo) SIN entrar en el EXE (un hook desactivado deja un jmp al EXE).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j){ const auto t=a+5+*reinterpret_cast<int32*>(a+1); if(t>=0x400000&&t<0x1000000)break; a=t; } return a; }

// Funciones de la clase (direccion del EXE <-> destino de la DLL): la DLL pasa punteros a sus propias funciones donde el EXE
// pasa la direccion del original. Canon() devuelve siempre la direccion del EXE.
struct CbPair { uintptr_t exe, dll, dllFinal, exeFinal; };
inline std::array<CbPair,128> cbs{}; inline size_t ncbs{};
inline void AddCb(uintptr_t exe,uintptr_t dll){ if(ncbs<cbs.size())cbs[ncbs++]={exe,dll,dll?Final(dll):0,0}; }
inline void FinishCbs(){ for(size_t k=0;k<ncbs;++k){ cbs[k].exeFinal=Final(cbs[k].exe); cbs[k].dllFinal=cbs[k].dll ? Final(cbs[k].dll) : 0; } }   // se recalcula tras cada cambio de estado de los hooks
inline uintptr_t Canon(uintptr_t u){
    if(u<0x10000)return u;
    for(size_t k=0;k<ncbs;++k)if(u==cbs[k].exe||(cbs[k].dll&&u==cbs[k].dll))return cbs[k].exe;
    if(IsCode(u)){ const auto fu=Final(u); for(size_t k=0;k<ncbs;++k){ if(cbs[k].dllFinal&&fu==cbs[k].dllFinal)return cbs[k].exe; if(fu==cbs[k].exeFinal)return cbs[k].exe; } }
    return u;
}
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first));
    if(InStack(u))return 0xD0000000u;
    if(u<0x1000000){ for(size_t k=0;k<ncbs;++k)if(u==cbs[k].exe)return uint32(u); return uint32(u); }
    const auto cu=Canon(u); if(cu!=u)return uint32(cu);
    return 0xE0000000u;
}
// Valor de 32 bits que puede ser un puntero del mundo falso o un dato (color, banderas...)
inline uint32 Val(uint32 v){ const auto u=uintptr_t(v); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32((k+1)<<20|(u-regs[k].first)); if(InStack(u))return 0xD0000000u; return v; }
inline bool Known(const void* p){ const auto u=uintptr_t(p); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return true; return false; }

// ---------------------------------------------------------------------------------------------------------------------
// Registro de trazas sin reservas
// ---------------------------------------------------------------------------------------------------------------------
struct Rec { uint32 t,a,b,c,d; };
inline std::array<Rec,8192> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(recOverflow)r.push_back("overflow"); return r; }

enum : uint32 {
    T_CFA=1,T_GFM,T_LTM,T_UPD,T_MTR,T_MROT,T_CREND,T_HID,T_HMX,T_GAH,T_ADRC,T_RESYNC,T_V3D,T_FRUST,T_MATTEX,T_REGA,T_REGF,T_REGC,
    T_RS,T_RENDONE,T_AMB,T_DEACT,T_GFA,T_RGFA,T_WSK,T_WINF,T_ESET,T_EREM,T_PSET,T_ALLOC,T_DELETE,T_CBA,T_CBB,T_VISCB,T_GMT,T_TRAP,
    T_CTOR,T_DTOR,T_COPY };

// ---------------------------------------------------------------------------------------------------------------------
// Estado de fixtures leido por los stubs
// ---------------------------------------------------------------------------------------------------------------------
struct Tables {
    int clumpAtomN[NCL]; void* clumpAtom[NCL][NAT];
    int32 reg[3];            // valores que devuelven los RegisterPlugin
    int visRet;              // resultado de la funcion de visibilidad de prueba
    uint8 miType;            // GetModelType de los model infos falsos
    int plugOff[3];          // offsets usados al llamar a los constructores de plugin
};
inline Tables tb;

inline int IdxOf(const void* p,const void* base,size_t stride,int n){ const auto u=reinterpret_cast<const uint8*>(p),b=reinterpret_cast<const uint8*>(base); if(u<b||u>=b+stride*size_t(n))return -1; return int((u-b)/stride); }
inline int ClIdx(const void* p){ return IdxOf(p,w.cl,CL_SIZE,NCL); }

// Arena de operator new/delete (sin reservas durante la llamada probada)
inline void* ArenaAlloc(size_t n){ const size_t off=arenaTop; if(n>arena.size()-off){ Push(T_ALLOC,uint32(n),0xFFFFFFFFu); return nullptr; } arenaTop=(off+n+15)&~size_t(15); Push(T_ALLOC,uint32(n),uint32(off)); return arena.data()+off; }

// Hash de las alfas de los materiales de un atomic y de las banderas de su geometria (lo que ve el render real)
inline uint32 AlphaHash(void* atomic){
    auto* geo=At<uint8*>(atomic,0x18); if(!Known(geo))return 0xBAD0BAD0u;
    const int32 n=At<int32>(geo,0x24); auto* arr=At<uint32*>(geo,0x20);
    uint32 h=Fnv32(geo+8,4)^uint32(n)*2654435761u;
    if(n<0||n>12||!Known(arr))return h^0xDEADu;
    for(int32 k=0;k<n;++k){ auto* m=reinterpret_cast<uint8*>(arr[k]); if(!Known(m)){ h=h*31u+0xFFu; continue; } h=(h*31u)+m[7]; }
    return h;
}

// ---------------------------------------------------------------------------------------------------------------------
// Stubs (todos respetan la convencion y el 'ret N' del original)
// ---------------------------------------------------------------------------------------------------------------------
using Cb2 = void*(__cdecl*)(void*,void*);
inline void* __cdecl S_ClumpForAllAtomics(void* c,void* cb,void* d){
    if(!Mine())return c; Push(T_CFA,Id(c),uint32(Canon(uintptr_t(cb))),Val(uint32(uintptr_t(d))));
    const int ci=ClIdx(c);
    if(ci>=0&&cb){ const auto fn=reinterpret_cast<Cb2>(cb); const int n=tb.clumpAtomN[ci]; for(int k=0;k<n;++k)if(!fn(tb.clumpAtom[ci][k],d))break; }
    return c; }
inline void* __cdecl S_GeometryForAllMaterials(void* g,void* cb,void* d){
    if(!Mine())return g; const int32 n=Known(g) ? At<int32>(g,0x24) : 0; Push(T_GFM,Id(g),uint32(Canon(uintptr_t(cb))),Val(uint32(uintptr_t(d))),uint32(n));
    if(Known(g)&&cb&&n>0&&n<=12){ const auto fn=reinterpret_cast<Cb2>(cb); auto* arr=At<uint32*>(g,0x20); for(int32 k=0;k<n;++k)if(!fn(reinterpret_cast<void*>(arr[k]),d))break; }
    return g; }
inline void* __cdecl S_FrameGetLTM(void* f){ if(!Mine())return nullptr; Push(T_LTM,Id(f)); return static_cast<uint8*>(f)+0x50; }
inline void* __cdecl S_FrameUpdateObjects(void* f){ if(!Mine())return f; Push(T_UPD,Id(f)); return f; }
inline void* __cdecl S_MatrixTranslate(void* m,const void* t,uint32 op){
    if(!Mine())return m; Push(T_MTR,Id(m),Fnv32(m,0x40),Fnv32(t,12),op);
    for(int k=0;k<3;++k)At<float>(m,0x30+4*k)+=At<float>(const_cast<void*>(t),4*k);
    return m; }
inline void* __cdecl S_MatrixRotate(void* m,const void* axis,float angle,uint32 op){
    if(!Mine())return m; Push(T_MROT,Id(m),Fnv32(m,0x40),Fnv32(axis,12),F(angle)); (void)op;
    At<float>(m,0x30)+=angle*0.01f; return m; }
inline void* __cdecl S_ClumpRender(void* c){
    if(!Mine())return c; auto* fr=At<uint8*>(c,4); Push(T_CREND,Id(c),Known(fr) ? Fnv32(fr+0x10,0x40) : 0u); return c; }
inline int32 __cdecl S_HAnimIndex(void* h,int32 id){ if(!Mine())return 0; Push(T_HID,Id(h),uint32(id)); return (id*7+3)%40; }
inline void* __cdecl S_HAnimMatrices(void* h){ if(!Mine())return nullptr; Push(T_HMX,Id(h)); return w.hmx; }
inline void* __cdecl S_AnimHierarchy(void* clump){ if(!Mine())return nullptr; Push(T_GAH,Id(clump)); return w.hier; }
inline void* __cdecl S_AtomicDefaultRender(void* a){ if(!Mine())return a; Push(T_ADRC,Id(a),AlphaHash(a),Known(a) ? uint32(At<uint16>(a,atomOff+2)) : 0u); return a; }
inline void __cdecl S_Resync(void* a){ if(!Mine())return; Push(T_RESYNC,Id(a)); At<float>(a,0x1C)+=1.0f; At<uint32>(a,0x4C)&=~2u; }
inline void* __cdecl S_V3dTransformPoints(void* out,const void* in,int32 n,const void* m){
    if(!Mine())return out; Push(T_V3D,Id(out),Id(in),uint32(n),Fnv32(m,0x40));
    float tmp[12]; const int32 nn= n>4 ? 4 : (n<0 ? 0 : n);
    for(int32 i=0;i<nn;++i)for(int k=0;k<3;++k)tmp[3*i+k]=At<float>(const_cast<void*>(in),12*i+4*k)+At<float>(const_cast<void*>(m),0x30+4*k);
    for(int32 i=0;i<nn;++i)for(int k=0;k<3;++k)At<float>(out,12*i+4*k)=tmp[3*i+k];
    return out; }
inline int32 __cdecl S_FrustumTest(const void* camera,const void* sphere){ if(!Mine())return 0; const uint32 h=Fnv32(sphere,16); Push(T_FRUST,Id(camera),h); return int32(h%3u); }
inline void* __cdecl S_MaterialSetTexture(void* m,void* t){ if(!Mine())return m; Push(T_MATTEX,Id(m),Id(t)); if(Known(m))At<void*>(m,0)=t; return m; }

// Registro de plugins: se invocan los tres callbacks (ctor/dtor/copia) sobre objetos de prueba y se traza el resultado
inline uint32 PlugId(void* fn,uintptr_t ctorExe){ const auto u=uintptr_t(fn); if(!u)return 0; if(u>=0x400000&&u<0x1000000)return uint32(u); const auto c=Canon(u); return c!=u ? uint32(c) : uint32(ctorExe); }
inline int32 RegCommon(int k,uint32 tag,int32 size,uint32 id,void* ctor,void* dtor,void* copy,uintptr_t ctorExe,uintptr_t dtorExe,uintptr_t copyExe){
    if(!Mine())return -1;
    Push(tag,uint32(size),id,PlugId(ctor,ctorExe),PlugId(dtor,dtorExe)); Push(tag,PlugId(copy,copyExe));
    const int off=tb.plugOff[k];
    using Ctor=void*(__cdecl*)(void*,int32,int32); using Copy=void*(__cdecl*)(void*,const void*,int32,int32);
    uint8* A=w.scratch[0]; uint8* B=w.scratch[1];
    // El plugin de clump guarda un puntero a funcion (la DLL el suyo, el EXE 0x732A30): se normaliza antes de resumir
    auto norm=[&](uint8* X){ if(k==2&&off>=0&&off+8<=0x200)At<uint32>(X,size_t(off))=uint32(Canon(At<uint32>(X,size_t(off)))); };
    if(ctor){ const auto r=reinterpret_cast<Ctor>(ctor)(A,off,size); norm(A); Push(T_CTOR,Id(r),Fnv32(A,0x200)); }
    if(copy){ const auto r=reinterpret_cast<Copy>(copy)(B,A,off,size); norm(B); Push(T_COPY,Id(r),Fnv32(B,0x200)); }
    if(dtor){ const auto r=reinterpret_cast<Ctor>(dtor)(B,off,size); norm(B); Push(T_DTOR,Id(r),Fnv32(B,0x200)); }
    return tb.reg[k];
}
inline int32 __cdecl S_RegAtomic(int32 size,uint32 id,void* ctor,void* dtor,void* copy){ return RegCommon(0,T_REGA,size,id,ctor,dtor,copy,0x732150,0x7321A0,0x732170); }
inline int32 __cdecl S_RegFrame(int32 size,uint32 id,void* ctor,void* dtor,void* copy){ return RegCommon(1,T_REGF,size,id,ctor,dtor,copy,0x7321B0,0x7321F0,0x7321D0); }
inline int32 __cdecl S_RegClump(int32 size,uint32 id,void* ctor,void* dtor,void* copy){ return RegCommon(2,T_REGC,size,id,ctor,dtor,copy,0x732E10,0x732220,0x732200); }

// Estados de render (puntero de funcion del dispositivo RW: lo usan igual el EXE y la DLL)
inline int32 __cdecl S_RenderState(uint32 state,uint32 value){ if(!Mine())return 1; Push(T_RS,state,Val(value)); return 1; }
inline uintptr_t rsSlot{}; inline uint32 rsOld{};
struct RsGuard { RsGuard(){ rsOld=*reinterpret_cast<uint32*>(rsSlot); *reinterpret_cast<uint32*>(rsSlot)=uint32(uintptr_t(&S_RenderState)); }
                 ~RsGuard(){ *reinterpret_cast<uint32*>(rsSlot)=rsOld; } };

inline void __cdecl S_RenderOneNonRoad(void* e){ if(!Mine())return; Push(T_RENDONE,Id(e),Known(e) ? At<uint32>(e,0x1C) : 0u); }
inline void __cdecl S_AmbientColours(){ if(!Mine())return; Push(T_AMB); }
inline void __cdecl S_DeActivateDirectional(){ if(!Mine())return; Push(T_DEACT); }
inline void __fastcall S_SetGunFlash(void* self,void*,uint32 flag){ if(!Mine())return; Push(T_GFA,Id(self),flag&0xFFu); }
inline void __fastcall S_ResetGunFlash(void* self,void*){ if(!Mine())return; Push(T_RGFA,Id(self)); }
inline int32 __fastcall S_WeaponSkill(void* self,void*){ if(!Mine())return 1; Push(T_WSK,Id(self)); const int slot=At<uint8>(self,0x718)%13; return int32((At<uint32>(self,0x5A0+0x1C*slot)*5u+uint32(slot))&3u); }
inline void* __cdecl S_WeaponInfo(int32 type,int32 skill){ if(!Mine())return w.wi[0]; Push(T_WINF,uint32(type),uint32(skill)); return w.wi[(uint32(type)+uint32(skill))&3u]; }
inline bool __fastcall S_EntSetup(void* self,void*){ if(!Mine())return false; Push(T_ESET,Id(self)); return (Fnv32(self,0x20)&1u)!=0; }
inline void __fastcall S_EntRemove(void* self,void*,uint32 flag){ if(!Mine())return; Push(T_EREM,Id(self),flag&0xFFu); }
inline bool __fastcall S_PedSetup(void* self,void*){ if(!Mine())return false; Push(T_PSET,Id(self)); return true; }
inline uint8 __fastcall S_ModelType(void* self,void*){ if(!Mine())return 0; Push(T_GMT,Id(self)); return tb.miType; }
inline void __fastcall S_Trap(void* self,void*){ if(!Mine())return; Push(T_TRAP,Id(self)); }

// Callbacks de prueba para las listas de alfa y la visibilidad del clump
inline void __cdecl CbA(void* obj,float d){ if(!Mine())return; Push(T_CBA,Id(obj),F(d)); }
inline void __cdecl CbB(void* obj,float d){ if(!Mine())return; Push(T_CBB,Id(obj),F(d)); }
inline int __cdecl VisCb(void* clump){ if(!Mine())return 0; Push(T_VISCB,Id(clump)); return tb.visRet; }

// Memoria
inline void* __cdecl S_New(size_t n){ if(!Mine())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n); return ArenaAlloc(n); }
inline void __cdecl S_Delete(void* p){ if(!Mine()){ if(p)reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; } if(!p)return; /* delete[] de nulo: el compilador puede omitir la llamada, no se traza */ Push(T_DELETE,Id(p)); }

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
enum : uint32 { G_RW=1, G_PLUG=2, G_NEW=4 };   // G_RW: primitivas RW, peds, luces y entidades; G_PLUG: registro de plugins; G_NEW: operator new/delete
struct Stub { uint32 group; uintptr_t exe,dll,body; void* fn; };
inline std::vector<Stub> stubs;
inline void AddStub(uint32 group,uintptr_t exe,uintptr_t dllSym,void* fn){ stubs.push_back({group,exe,dllSym,dllSym ? Body(dllSym) : 0,fn}); }
template<class P> inline void* FnPtr(P f){ return reinterpret_cast<void*>(f); }
inline bool AddHookStub(uint32 group,uintptr_t exe,const char* cls,const char* name,void* fn){
    auto* h=FindHook(cls,name); const auto our=OurAddress(h); if(!h||!our)return false;
    AddStub(group,exe,our,fn); return true;
}
// Conjunto de parches SIN reservas de memoria: los EntryPatch viven en almacenamiento estatico (con operator new parcheado
// un vector/unique_ptr consumiria la arena con bytes que dependen del lado y ensuciaria la traza).
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

// Instantanea de globales (se restaura al terminar cada ejecucion)
struct Saved { uintptr_t addr; std::vector<uint8> bytes; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }

template<class R,class... A> inline R CallThis(uintptr_t a,void* self,A... args){ return reinterpret_cast<R(__thiscall*)(void*,A...)>(a)(self,args...); }
template<class R,class... A> inline R CallCdecl(uintptr_t a,A... args){ return reinterpret_cast<R(__cdecl*)(A...)>(a)(args...); }
}
