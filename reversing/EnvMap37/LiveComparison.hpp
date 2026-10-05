#pragma once
// Banco dentro del motor: CCustomCarEnvMapPipeline (lista de 34, 37 funciones). Original (hooks de la clase
// desactivados) frente a candidato sobre un mundo RW falso y determinista: materiales, atomics, geometrías, clump,
// frames, LTM, luz, texturas, datos MatFX y una RwResEntry con hasta 4 mallas; los offsets de los plugins se leen
// del EXE en tiempo de ejecución. Los tres pools del pipeline se sustituyen temporalmente por pools falsos pequeños
// (formato crudo de CPool) para poder llenarlos. Se simulan y trazan sin reservar memoria: RwStreamRead/Write,
// RwTextureRead/Destroy, el registro de plugins de material/atomic, RxPipeline*, RwD3D9GetCaps, RpMatFXMaterialGetEffects,
// RpGeometryForAllMaterials, SetPipelineID (EXE + DLL), RwFrameGetLTM, _rwD3D9EnableClippingIfNeeded (EXE + DLL),
// estados/TSS/texturas/luces/transformaciones/material/superficie D3D9, el alfa por vértice (0x7FE0A0), el vaciado de
// estados, fpRenderStateSet del motor, un dispositivo D3D9 falso (solo durante el render) y operator new/delete
// (0x82119A/0x8214BD, solo durante CreatePipe/DestroyPipe y sobre una arena fija). Nunca se toca el dispositivo real
// ni materiales/texturas reales. Se guardan y restauran en cada ejecución: offsets de plugins, datos falsos 0xC02D18,
// pipeline y pools, g_GameLight, matrices de textura, material D3D9 global, intensidad especular, AmbientSaturated,
// cachés de D3D9, pDirect, la calidad de efectos, el puntero del dispositivo y fpRenderStateSet.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h"
#include "../ProbeRuntime.hpp"

namespace EnvMap37Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }
inline uint64 Fnv(const void* p,size_t n){ uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 F(float f){ return std::bit_cast<uint32>(f); }

// --- registro sin reservas (los stubs corren también con operator new sustituido) ---
struct Rec { uint32 t, a, b, c, d; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c,d}; else recOverflow=true; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].t,recs[i].a,recs[i].b,recs[i].c,recs[i].d}); if(recOverflow)r.push_back("overflow"); return r; }

// --- mundo RW falso ---
constexpr size_t NM=6, NA=3, NF=3, NT=4, NPM=16, NPA=8, NPS=16, NMESH=4;
struct World {
    alignas(16) uint8 mats[NM][0x200];
    alignas(16) uint8 atomics[NA][0x200];
    alignas(16) uint8 geoms[2][0x40];
    alignas(16) uint8 clump[0x40];
    alignas(16) uint8 frames[NF][0x80];
    alignas(16) uint8 ltms[NF][0x40];
    alignas(16) uint8 light[0x20];
    alignas(16) uint8 texs[NT][0x60];
    alignas(16) uint8 matfx[NM][0x20];
    alignas(16) uint8 matObjs[NPM][12];
    alignas(16) uint8 matFlags[NPM];
    alignas(16) uint8 atmObjs[NPA][12];
    alignas(16) uint8 atmFlags[NPA];
    alignas(16) uint8 specObjs[NPS][8];
    alignas(16) uint8 specFlags[NPS];
    alignas(16) uint8 pools[3][0x20];
    alignas(16) uint8 res[0x18+0x40+0x24*NMESH];
    alignas(16) uint8 caps[0x140];
    alignas(16) uint8 pipe[0x40];
    alignas(16) uint8 nodeDef[0x40];
    alignas(16) uint8 node[0x40];
    alignas(16) uint8 stream[0x20];
    alignas(16) uint8 buf12[0x10];
};
inline World w;
alignas(16) inline std::array<uint8,0x20000> arena{}; inline size_t arenaTop{}; inline int32 newCount{}, newFail{-1};
template<class T=void*> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

inline bool IsCode(uintptr_t a){ MEMORY_BASIC_INFORMATION mi{}; return a>=0x10000&&VirtualQuery(reinterpret_cast<void*>(a),&mi,sizeof(mi))&&mi.State==MEM_COMMIT&&(mi.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)); }
inline uintptr_t Final(uintptr_t a){ for(int d=0;d<4&&IsCode(a)&&*reinterpret_cast<uint8*>(a)==0xE9;++d)a=a+5+*reinterpret_cast<int32*>(a+1); return a; }
// Callbacks del EXE que la DLL pasa como punteros propios: se comparan por destino final.
constexpr uintptr_t kCb[]{0x5D8BD0,0x5D95B0,0x5D9600,0x5D9660,0x5D8CD0,0x5D8D10,0x5D8D30,0x5D9730,0x5D9780,0x5D8D40,0x5D97D0,0x5D9830,0x5D9880,0x5D8D60,0x5D8DD0,0x5D8490,0x5D9900,0x5DA560};
inline uint32 Id(const void* p){
    const auto u=uintptr_t(p); if(!u)return 0;
    const auto wb=uintptr_t(&w); if(u>=wb&&u<wb+sizeof(World))return 0x40000000u|uint32(u-wb);
    const auto ab=uintptr_t(arena.data()); if(u>=ab&&u<ab+arena.size())return 0x50000000u|uint32(u-ab);
    for(auto e:kCb)if(u==e)return uint32(e);
    if(u<0x1000000)return uint32(u);
    const auto f=Final(u); for(auto e:kCb)if(f==Final(e))return uint32(e);
    return 0xD1100000u;
}

// --- stubs ---
enum : uint32 { T_SREAD=1,T_SWRITE,T_TEXREAD,T_TEXDESTROY,T_MATREG,T_MATSTREAM,T_ATMREG,T_REGCB,T_PCREATE,T_NODEDEF,T_LOCK,T_ADDFRAG,T_UNLOCK,T_FIND,
    T_GETICB,T_SETICB,T_SETRICB,T_SETRCB,T_PDESTROY,T_CAPS,T_NEW,T_DELETE,T_EFFECTS,T_FORALL,T_PIPEID,T_CLIP,T_GETRS,T_SETRS,T_TSS,T_TEXTURE,T_STREAMS,
    T_LIGHT,T_ENLIGHT,T_TRANSFORM,T_MATERIAL,T_SURFACE,T_VALPHA,T_FLUSH,T_LTM,T_RWRS,T_DEV,T_INSTCB };
alignas(16) inline std::array<uint8,0x20> streamSrc{};
inline RwUInt32 __cdecl FakeStreamRead(RwStream* s,void* buf,RwUInt32 len){ Push(T_SREAD,Id(s),len,Id(buf)!=0); std::memcpy(buf,streamSrc.data(),std::min<size_t>(len,streamSrc.size())); return len; }
inline RwStream* __cdecl FakeStreamWrite(RwStream* s,const void* buf,RwUInt32 len){ Push(T_SWRITE,Id(s),len,Fnv32(buf,std::min<size_t>(len,0x20))); return s; }
inline RwTexture* texReadResult{};
inline RwTexture* __cdecl FakeTexRead(const char* name,const char* mask){ Push(T_TEXREAD,Fnv32(name,strnlen(name,24)),Id(mask)); return texReadResult; }
inline RwBool __cdecl FakeTexDestroy(RwTexture* t){ Push(T_TEXDESTROY,Id(t)); return TRUE; }
inline std::array<int32,5> regResults{}; inline size_t regCall{};
inline int32 NextReg(){ return regCall<regResults.size() ? regResults[regCall++] : -7; }
inline RwInt32 __cdecl FakeMatReg(RwInt32 size,RwUInt32 id,void* c,void* d,void* cp){ Push(T_MATREG,size,id); Push(T_REGCB,Id(c),Id(d),Id(cp)); return NextReg(); }
inline RwInt32 __cdecl FakeMatStream(RwUInt32 id,void* r,void* wr,void* g){ Push(T_MATSTREAM,id,Id(r),Id(wr),Id(g)); return NextReg(); }
inline RwInt32 __cdecl FakeAtmReg(RwInt32 size,RwUInt32 id,void* c,void* d,void* cp){ Push(T_ATMREG,size,id); Push(T_REGCB,Id(c),Id(d),Id(cp)); return NextReg(); }
inline uint32 rxMask{};
inline void* __cdecl FakePipelineCreate(){ Push(T_PCREATE); return rxMask&1 ? w.pipe : nullptr; }
inline void* __cdecl FakeNodeDef(){ Push(T_NODEDEF); return w.nodeDef; }
inline void* __cdecl FakePipeLock(void* p){ Push(T_LOCK,Id(p)); return rxMask&2 ? p : nullptr; }
inline void* __cdecl FakeAddFragment(void* p,void* first,void* def,void* end){ Push(T_ADDFRAG,Id(p),Id(first),Id(def),Id(end)); return rxMask&4 ? p : nullptr; }
inline void* __cdecl FakeUnlock(void* p){ Push(T_UNLOCK,Id(p)); return rxMask&8 ? p : nullptr; }
inline void* __cdecl FakeFindNode(void* p,const char* name,void* start,void* idx){ Push(T_FIND,Id(p),uintptr_t(name)==At<uintptr_t>(w.nodeDef,0),Id(start),Id(idx)); return rxMask&16 ? w.node : nullptr; }
inline void* __cdecl FakeGetInstanceCB(void* n){ Push(T_GETICB,Id(n)); return reinterpret_cast<void*>(0x1111); }
inline void __cdecl FakeSetInstanceCB(void* n,void* cb){ Push(T_SETICB,Id(n),Id(cb)); }
inline void __cdecl FakeSetReinstanceCB(void* n,void* cb){ Push(T_SETRICB,Id(n),Id(cb)); }
inline void __cdecl FakeSetRenderCB(void* n,void* cb){ Push(T_SETRCB,Id(n),Id(cb)); }
inline void __cdecl FakePipeDestroy(void* p){ Push(T_PDESTROY,Id(p)); }
inline const void* __cdecl FakeGetCaps(){ Push(T_CAPS); return w.caps; }
inline void* __cdecl FakeNew(size_t n){ const int32 k=newCount++; Push(T_NEW,uint32(n),uint32(k)); if(k==newFail||arenaTop+n>arena.size())return nullptr; void* p=arena.data()+arenaTop; arenaTop=(arenaTop+n+15)&~size_t(15); return p; }
inline void __cdecl FakeDelete(void* p){ Push(T_DELETE,Id(p)); }
inline uint32 matfxEffect{};
inline RpMatFXMaterialFlags __cdecl FakeGetEffects(const RpMaterial* m){ Push(T_EFFECTS,Id(m)); return RpMatFXMaterialFlags(matfxEffect); }
inline void* __cdecl FakeForAllMaterials(void* g,void* cb,void* d){ Push(T_FORALL,Id(g),Id(cb),Id(d)); return g; }
inline void __cdecl FakeSetPipelineID(RpAtomic* a,RwUInt32 id){ Push(T_PIPEID,Id(a),id); }
inline uint32 lightingValue{};
inline void __cdecl FakeClipping(void* o,RwUInt32 t){ Push(T_CLIP,Id(o),t); }
inline void __cdecl FakeGetRenderState(RwUInt32 s,void* v){ Push(T_GETRS,s); *static_cast<uint32*>(v)=lightingValue; }
inline void __cdecl FakeSetRenderState(RwUInt32 s,RwUInt32 v){ Push(T_SETRS,s,v); }
inline void __cdecl FakeSetTSS(RwUInt32 st,RwUInt32 t,RwUInt32 v){ Push(T_TSS,st,t,v); }
inline RwBool __cdecl FakeSetTexture(RwTexture* t,RwUInt32 st){ Push(T_TEXTURE,Id(t),st); return TRUE; }
inline void __cdecl FakeSetStreams(const void* s,RwBool off){ Push(T_STREAMS,Id(s),uint32(off)); }
inline RwBool __cdecl FakeSetLight(RwInt32 i,const void* l){ Push(T_LIGHT,uint32(i),Id(l)); return TRUE; }
inline RwBool __cdecl FakeEnableLight(RwInt32 i,RwBool e){ Push(T_ENLIGHT,uint32(i),uint32(e)); return TRUE; }
inline RwBool __cdecl FakeSetTransform(RwUInt32 s,const void* m){ Push(T_TRANSFORM,s,Id(m),Fnv32(m,0x40)); return TRUE; }
inline RwBool __cdecl FakeSetMaterial(const void* m){ Push(T_MATERIAL,Id(m),Fnv32(m,0x44)); return TRUE; }
inline RwBool __cdecl FakeSurface(const void* sp,const void* c,RwUInt32 fl){ Push(T_SURFACE,Id(sp),Id(c),fl,Fnv32(sp,12)^Fnv32(c,4)); return TRUE; }
inline void __cdecl FakeVertexAlpha(RwBool e){ Push(T_VALPHA,uint32(e)); }
inline void __cdecl FakeFlush(){ Push(T_FLUSH); }
inline RwMatrix* __cdecl FakeLTM(RwFrame* f){ Push(T_LTM,Id(f)); for(size_t k=0;k<NF;++k)if(reinterpret_cast<uint8*>(f)==w.frames[k])return reinterpret_cast<RwMatrix*>(w.ltms[k]); return reinterpret_cast<RwMatrix*>(w.ltms[0]); }
inline RwBool __cdecl FakeRenderStateSet(RwRenderState s,void* v){ Push(T_RWRS,uint32(s),uint32(uintptr_t(v))); return TRUE; }
inline HRESULT __stdcall DevTrap(void*){ Push(T_DEV,0xFFFF); return 0; }
inline HRESULT __stdcall DevSetIndices(void*,void* p){ Push(T_DEV,104,uint32(uintptr_t(p))); return 0; }
inline HRESULT __stdcall DevSetDecl(void*,void* p){ Push(T_DEV,87,uint32(uintptr_t(p))); return 0; }
inline HRESULT __stdcall DevSetVS(void*,void* p){ Push(T_DEV,92,uint32(uintptr_t(p))); return 0; }
inline HRESULT __stdcall DevSetPS(void*,void* p){ Push(T_DEV,107,uint32(uintptr_t(p))); return 0; }
inline HRESULT __stdcall DevDrawIndexed(void*,uint32 t,int32 b,uint32 mn,uint32 nv,uint32 st,uint32 c){ Push(T_DEV,82,t,uint32(b)^(mn<<8),nv^(st<<12)^(c<<20)); return 0; }
inline HRESULT __stdcall DevDraw(void*,uint32 t,uint32 st,uint32 c){ Push(T_DEV,81,t,st,c); return 0; }
inline std::array<void*,119> devVt{}; inline void* fakeDevice[2]{devVt.data(),nullptr};
inline int32 instanceResult{};
inline RwBool __cdecl FakeInstanceCB(void* o,void* h,RwBool r){ Push(T_INSTCB,Id(o),Id(h),uint32(r)); return instanceResult; }

// El EXE (pluginEnvMatStreamReadCB) compara 12 bytes de un local con el byte de relleno +5 sin inicializar:
// se deja la pila a cero justo antes de la llamada y el falso con +5 = 0 (la DLL toma el +5 del falso).
__declspec(noinline) inline void ClearStack(){ volatile uint8 pad[2048]; for(size_t i=0;i<sizeof(pad);++i)pad[i]=0; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,37> fns{{
    {"CCustomCarEnvMapPipeline","AllocEnvMapPipeAtomicData",0x5D96F0,0,-1,256},{"CCustomCarEnvMapPipeline","CreateCustomObjPipe",0x5D9F80,0,-1,256},
    {"CCustomCarEnvMapPipeline","CreatePipe",0x5DA020,0,-1,128},{"CCustomCarEnvMapPipeline","CustomPipeAtomicSetup",0x5DA610,0,-1,128},
    {"CCustomCarEnvMapPipeline","CustomPipeInstanceCB",0x5D8490,0,-1,256},{"CCustomCarEnvMapPipeline","CustomPipeMaterialSetup",0x5DA560,0,-1,512},
    {"CCustomCarEnvMapPipeline","CustomPipeRenderCB",0x5D9900,0,-1,1024},{"CCustomCarEnvMapPipeline","DestroyPipe",0x5DA130,0,-1,128},
    {"CCustomCarEnvMapPipeline","GetFxEnvScaleX",0x5D6F90,0,-1,256},{"CCustomCarEnvMapPipeline","GetFxEnvScaleY",0x5D6FC0,0,-1,256},
    {"CCustomCarEnvMapPipeline","GetFxEnvShininess",0x5D8AD0,0,-1,256},{"CCustomCarEnvMapPipeline","GetFxEnvTransSclX",0x5D7040,0,-1,256},
    {"CCustomCarEnvMapPipeline","GetFxEnvTransSclY",0x5D7070,0,-1,256},{"CCustomCarEnvMapPipeline","GetFxSpecSpecularity",0x5D8B90,0,-1,256},
    {"CCustomCarEnvMapPipeline","GetFxSpecTexture",0x5D8B50,0,-1,256},{"CCustomCarEnvMapPipeline","PreRenderUpdate",0x5D8870,0,-1,256},
    {"CCustomCarEnvMapPipeline","RegisterPlugin",0x5DA450,0,-1,256},{"CCustomCarEnvMapPipeline","SetCustomEnvMapPipeMaterialDataDefaults",0x5D8BB0,0,-1,128},
    {"CCustomCarEnvMapPipeline","SetFxEnvScale",0x5D6F40,0,-1,512},{"CCustomCarEnvMapPipeline","SetFxEnvShininess",0x5D70A0,0,-1,512},
    {"CCustomCarEnvMapPipeline","SetFxEnvTexture",0x5DA230,0,-1,512},{"CCustomCarEnvMapPipeline","SetFxEnvTransScl",0x5D6FF0,0,-1,512},
    {"CCustomCarEnvMapPipeline","pluginEnvAtmConstructorCB",0x5D8D30,0,-1,128},{"CCustomCarEnvMapPipeline","pluginEnvAtmCopyConstructorCB",0x5D9780,0,-1,256},
    {"CCustomCarEnvMapPipeline","pluginEnvAtmDestructorCB",0x5D9730,0,-1,256},{"CCustomCarEnvMapPipeline","pluginEnvMatConstructorCB",0x5D8BD0,0,-1,128},
    {"CCustomCarEnvMapPipeline","pluginEnvMatCopyConstructorCB",0x5D9600,0,-1,256},{"CCustomCarEnvMapPipeline","pluginEnvMatDestructorCB",0x5D95B0,0,-1,256},
    {"CCustomCarEnvMapPipeline","pluginEnvMatStreamGetSizeCB",0x5D8D10,0,-1,128},{"CCustomCarEnvMapPipeline","pluginEnvMatStreamReadCB",0x5D9660,0,-1,512},
    {"CCustomCarEnvMapPipeline","pluginEnvMatStreamWriteCB",0x5D8CD0,0,-1,256},{"CCustomCarEnvMapPipeline","pluginSpecMatConstructorCB",0x5D8D40,0,-1,128},
    {"CCustomCarEnvMapPipeline","pluginSpecMatCopyConstructorCB",0x5D9830,0,-1,256},{"CCustomCarEnvMapPipeline","pluginSpecMatDestructorCB",0x5D97D0,0,-1,256},
    {"CCustomCarEnvMapPipeline","pluginSpecMatStreamGetSizeCB",0x5D8DD0,0,-1,128},{"CCustomCarEnvMapPipeline","pluginSpecMatStreamReadCB",0x5D9880,0,-1,512},
    {"CCustomCarEnvMapPipeline","pluginSpecMatStreamWriteCB",0x5D8D60,0,-1,256},
}};

// Región de memoria global guardada y restaurada en cada ejecución.
struct Saved { uintptr_t addr; std::vector<uint8> bytes; };
inline std::vector<Saved> saved;
inline void Save(uintptr_t a,size_t n){ saved.push_back({a,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); }
inline void RestoreAll(){ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size()); }
inline bool AllRestored(){ bool ok=true; for(auto& s:saved)ok&=std::memcmp(reinterpret_cast<void*>(s.addr),s.bytes.data(),s.bytes.size())==0; return ok; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"envmap37-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"envmap37-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("envmap37");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"envmap37-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,37> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,37> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    auto restoreHooks=[&]{ for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);} };

    // offsets de plugins (EXE)
    const int32 envOff=*reinterpret_cast<int32*>(0x8D12C4), atmOff=*reinterpret_cast<int32*>(0x8D12C8), specOff=*reinterpret_cast<int32*>(0x8D12CC),
                fxOff=*reinterpret_cast<int32*>(0xC9AB74), visOff=*reinterpret_cast<int32*>(0x8D608C);
    constexpr int32 altOff=0x1F0;
    auto inMat=[](int32 o){ return o>=0x1C&&o+4<=0x1E0; }; auto inAtm=[](int32 o){ return o>=0x70&&o+4<=0x1E0; };
    if(!inMat(envOff)||!inMat(specOff)||!inMat(fxOff)||!inAtm(atmOff)||!inAtm(visOff)||envOff==specOff||envOff==fxOff||specOff==fxOff||atmOff==visOff){
        restoreHooks(); Write({{"event","error"},{"reason","plugin offsets"},{"offsets",{envOff,atmOff,specOff,fxOff,visOff}}}); return; }

    // estado global guardado (se restaura en cada ejecución)
    auto* const engine=*reinterpret_cast<uint8**>(0xC97B24);
    saved.clear();
    Save(0x8D12C4,0x6C);   // offsets, gSpecIntensity, material D3D9 global, superficie/color por defecto
    Save(0xC02CB0,0x68);   // g_GameLight
    Save(0xC02D18,0xA0);   // datos falsos, ObjPipeline, pools, matrices de textura
    Save(0x8E2418,0x10);   // AmbientSaturated
    Save(0x8E2440,0x14);   // cachés de D3D9
    Save(0xC97C28,4);      // dispositivo
    Save(0xC886EC,4);      // pDirect
    Save(0xA9AE00+0x54,4); // calidad de efectos
    Save(uintptr_t(engine+0x20),4); // fpRenderStateSet
    for(auto& v:devVt)v=reinterpret_cast<void*>(&DevTrap);
    devVt[81]=reinterpret_cast<void*>(&DevDraw); devVt[82]=reinterpret_cast<void*>(&DevDrawIndexed); devVt[87]=reinterpret_cast<void*>(&DevSetDecl);
    devVt[92]=reinterpret_cast<void*>(&DevSetVS); devVt[104]=reinterpret_cast<void*>(&DevSetIndices); devVt[107]=reinterpret_cast<void*>(&DevSetPS);

    auto& fakeData=*reinterpret_cast<std::array<uint8,12>*>(0xC02D18);
    auto& objPipe=*reinterpret_cast<void**>(0xC02D24);
    auto& matPoolG=*reinterpret_cast<void**>(0xC02D28); auto& atmPoolG=*reinterpret_cast<void**>(0xC02D2C); auto& specPoolG=*reinterpret_cast<void**>(0xC02D30);
    auto& specOffG=*reinterpret_cast<int32*>(0x8D12CC);
    auto& specIntensity=*reinterpret_cast<float*>(0x8D12D0);
    auto* const ambSat=reinterpret_cast<float*>(0x8E2418);
    auto& fxQuality=*reinterpret_cast<int32*>(0xA9AE00+0x54);
    auto& device=*reinterpret_cast<void**>(0xC97C28);
    auto& pDirectG=*reinterpret_cast<void**>(0xC886EC);
    auto& fpRS=*reinterpret_cast<void**>(engine+0x20);
    static const char nodeName[]="nodeD3D9AtomicAllInOne.csl";
    static const char* texNames[]{"xvehicleenv128","vehicleenvmap128","vehiclespecdot64","x","","vehiclelights128_with_long_name_x","Xvehicle"};

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(0x7EC9D0,FakeStreamRead), s2(0x7ECB30,FakeStreamWrite), s3(0x7F3AC0,FakeTexRead), s4(0x7F3820,FakeTexDestroy);
        EntryPatch g1(0x74DBF0,FakeMatReg), g2(0x74DC20,FakeMatStream), g3(0x74BDA0,FakeAtmReg);
        EntryPatch p1(0x8057B0,FakePipelineCreate), p2(0x7582E0,FakeNodeDef), p3(0x806990,FakePipeLock), p4(0x806BE0,FakeAddFragment), p5(0x805F40,FakeUnlock), p6(0x806B30,FakeFindNode);
        EntryPatch p7(0x757390,FakeGetInstanceCB), p8(0x757380,FakeSetInstanceCB), p9(0x7573A0,FakeSetReinstanceCB), p10(0x7573E0,FakeSetRenderCB), p11(0x805820,FakePipeDestroy);
        EntryPatch p12(0x7FAD20,FakeGetCaps), p13(0x812140,FakeGetEffects), p14(0x74C790,FakeForAllMaterials);
        EntryPatch p15(0x72FC50,FakeSetPipelineID), p15d(bit_cast<uintptr_t>(static_cast<void(*)(RpAtomic*,RwUInt32)>(&SetPipelineID)),FakeSetPipelineID);
        EntryPatch d1(0x756D90,FakeClipping), d1d(bit_cast<uintptr_t>(&_rwD3D9EnableClippingIfNeeded),FakeClipping), d2(0x7FC320,FakeGetRenderState), d3(0x7FC2D0,FakeSetRenderState);
        EntryPatch d4(0x7FC340,FakeSetTSS), d5(0x7FDE70,FakeSetTexture), d6(0x7FA090,FakeSetStreams), d7(0x7FA660,FakeSetLight), d8(0x7FA860,FakeEnableLight);
        EntryPatch d9(0x7FA390,FakeSetTransform), d10(0x7FC430,FakeSetMaterial), d11(0x7FC4D0,FakeSurface), d12(0x7FE0A0,FakeVertexAlpha), d13(0x7FC200,FakeFlush), d14(0x7F0990,FakeLTM);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x37E1u; if(!stt)stt=1;
                    nrec=0; recOverflow=false; regCall=0; newCount=0; newFail=-1; arenaTop=0;
                    // --- construir el mundo ---
                    { auto* b=reinterpret_cast<uint8*>(&w); for(size_t k=0;k<sizeof(World);++k)b[k]=uint8(next()); }
                    const uint16 rf=*reinterpret_cast<uint16*>(engine+8);
                    // texturas
                    for(size_t k=0;k<NT;++k){ auto* t=w.texs[k]; std::memset(t+0x10,0,0x20); const char* nmx=texNames[next()%std::size(texNames)];
                        std::memcpy(t+0x10,nmx,std::min<size_t>(strlen(nmx),0x1F)); At<int32>(t,0x54)=int32(next()%5); }
                    auto tex=[&]()->uint8*{ return next()%4 ? w.texs[next()%NT] : nullptr; };
                    // pools (formato crudo de CPool)
                    auto setupPool=[&](uint8* pool,uint8* objs,uint8* flags,int32 cap){
                        At<uint8*>(pool,0)=objs; At<uint8*>(pool,4)=flags; At<int32>(pool,8)=cap; At<int32>(pool,0xC)=int32(next()%uint32(cap+1))-1; pool[0x10]=uint8(next()%2); pool[0x11]=1;
                        const bool full=next()%6==0; for(int32 k=0;k<cap;++k)flags[k]=uint8(full||next()%3 ? next()&0x7F : 0x80|(next()&0x7F)); };
                    setupPool(w.pools[0],&w.matObjs[0][0],w.matFlags,NPM); setupPool(w.pools[1],&w.atmObjs[0][0],w.atmFlags,NPA); setupPool(w.pools[2],&w.specObjs[0][0],w.specFlags,NPS);
                    matPoolG=w.pools[0]; atmPoolG=w.pools[1]; specPoolG=w.pools[2];
                    auto usedMat=[&]{ const size_t k=next()%NPM; w.matFlags[k]&=0x7F; return w.matObjs[k]; };
                    auto usedAtm=[&]{ const size_t k=next()%NPA; w.atmFlags[k]&=0x7F; return w.atmObjs[k]; };
                    auto usedSpec=[&]{ const size_t k=next()%NPS; w.specFlags[k]&=0x7F; return w.specObjs[k]; };
                    for(auto& o:w.matObjs){ At<uint8*>(o,8)=tex(); if(next()%3==0)At<uint16>(o,6)=rf; if(next()%5==0)o[4]=0; }
                    for(auto& o:w.atmObjs){ At<float>(o,0)=uni(-0.5f,1.5f); At<float>(o,4)=uni(-3000.f,3000.f); At<float>(o,8)=uni(-3000.f,3000.f); }
                    for(auto& o:w.specObjs){ const uint32 k=next()%8; At<float>(o,0)=k==0 ? 0.f : k==1 ? -0.f : k==2 ? std::bit_cast<float>(0x7FC00000u) : k==3 ? uni(1.f,5.f) : uni(0.f,1.f); At<uint8*>(o,4)=tex(); }
                    for(size_t k=0;k<NM;++k)At<uint8*>(w.matfx[k],4)=tex();
                    // datos falsos globales
                    // Como los datos del pool: la textura del falso siempre es una textura falsa o nula (el EXE lee su nombre)
                    for(auto& b:fakeData)b=uint8(next()); if(next()%3==0)fakeData={8,8,8,8,0xFF,fakeData[5],0,0}; At<uint8*>(fakeData.data(),8)=tex();
                    auto envSlot=[&]()->void*{ const uint32 k=next()%6; return k==0 ? nullptr : k==1 ? static_cast<void*>(fakeData.data()) : static_cast<void*>(usedMat()); };
                    // materiales
                    static const uint32 special[]{0xAF00FF,0x00FF3C,0x003CFF,0x00AFFF,0x00FFB9,0xC8FF00,0xFF00FF,0xFFFF00};
                    for(size_t k=0;k<NM;++k){ auto* m=w.mats[k]; At<uint8*>(m,0)=tex();
                        const uint32 cm=next()%5; if(cm==0)At<uint32>(m,4)=special[next()%8]|(next()%2 ? 0xFF000000u : (next()&0xFF)<<24); else if(cm==1)At<uint32>(m,4)=0xFFFFFFFFu;
                        At<float>(m,0xC)=next()%3==0 ? 1.0f : uni(0.f,2.f); At<uint32>(m,0x10)=next(); At<float>(m,0x14)=uni(0.f,2.f);
                        At<void*>(m,envOff)=envSlot(); At<void*>(m,specOff)=next()%3 ? static_cast<void*>(usedSpec()) : nullptr; At<void*>(m,fxOff)=w.matfx[k];
                        At<void*>(m,altOff)=envSlot(); }
                    // frames, LTM, luz, clump, geometrías y atomics
                    for(size_t k=0;k<NF;++k){ auto* l=reinterpret_cast<float*>(w.ltms[k]); for(int q=0;q<12;++q)l[q]=uni(-1.f,1.f);
                        const uint32 pm=next()%8; for(int q=12;q<15;++q)l[q]=pm==0 ? 0.f : pm==1 ? uni(-1e6f,1e6f) : uni(-3000.f,3000.f);
                        if(next()%16==0)l[8]=std::bit_cast<float>(0x7FC00000u);
                        auto* fr=reinterpret_cast<float*>(w.frames[k]+0x10); for(int q=0;q<16;++q)fr[q]=uni(-1.f,1.f); if(next()%8==0)fr[8]=fr[9]=fr[10]=0.f; }
                    At<void*>(w.light,4)=w.frames[next()%NF];
                    w.clump[0]=2; At<void*>(w.clump,4)=w.frames[next()%NF];
                    for(auto& g:w.geoms)At<uint32>(g,8)=next()%2 ? next()|0x80u : next()&~0x80u;
                    for(size_t k=0;k<NA;++k){ auto* a=w.atomics[k]; a[0]=1; At<void*>(a,4)=w.frames[k]; At<void*>(a,0x18)=w.geoms[next()%2];
                        At<void*>(a,0x3C)=next()%3==0 ? static_cast<void*>(w.clump) : nullptr; At<uint16>(a,visOff+2)=uint16(next()%3 ? next()&~0x6000u : next());
                        At<void*>(a,atmOff)=next()%3 ? static_cast<void*>(usedAtm()) : nullptr; At<void*>(a,altOff)=next()%2 ? static_cast<void*>(usedAtm()) : nullptr; }
                    At<const char*>(w.nodeDef,0)=nodeName;

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto* mat=w.mats[next()%NM]; auto* atomic=w.atomics[next()%NA];

                    if(nm=="AllocEnvMapPipeAtomicData") ret=Id(reinterpret_cast<void*(__cdecl*)(void*)>(a)(atomic));
                    else if(nm=="CreateCustomObjPipe"){ rxMask=next()%4 ? 0x1F : next(); ret=Id(reinterpret_cast<void*(__cdecl*)()>(a)()); }
                    else if(nm=="CreatePipe"){
                        rxMask=next()%4 ? 0x1F : next();
                        At<uint32>(w.caps,0x94)=std::array<uint32,5>{0,1,2,8,0x80000002u}[next()%5]; At<uint32>(w.caps,0x98)=next()%4;
                        At<uint32>(w.caps,0x90)=next()%4 ? next()|0x01002000u : next(); At<uint32>(w.caps,0x38)=next()%4 ? next()|0x200u : next();
                        if(next()%3==0){ At<uint32>(w.caps,0x94)=2; At<uint32>(w.caps,0x98)=2; }
                        static const int32 fails[]{-1,-1,-1,0,1,3,4,6,7}; newFail=fails[next()%std::size(fails)];
                        for(size_t k=0;k<arena.size();++k)arena[k]=uint8(k*13+i);
                        RwBool r;
                        { EntryPatch n1(0x82119A,FakeNew), dd1(0x8214BD,FakeDelete); r=reinterpret_cast<RwBool(__cdecl*)()>(a)(); }
                        ret=r; out["pools"]={Id(matPoolG),Id(atmPoolG),Id(specPoolG),Id(objPipe)}; out["light"]=Fnv(reinterpret_cast<void*>(0xC02CB0),0x68);
                        out["arena"]=Fnv(arena.data(),arena.size()); out["top"]=arenaTop;
                    }
                    else if(nm=="DestroyPipe"){
                        for(int k=0;k<3;++k){ At<int32>(w.pools[k],8)=std::array<int32,4>{-1,0,5,16}[next()%4]; }
                        matPoolG=next()%5 ? static_cast<void*>(w.pools[0]) : nullptr; atmPoolG=next()%5 ? static_cast<void*>(w.pools[1]) : nullptr; specPoolG=next()%5 ? static_cast<void*>(w.pools[2]) : nullptr;
                        objPipe=next()%4 ? static_cast<void*>(w.pipe) : nullptr;
                        { EntryPatch n1(0x82119A,FakeNew), dd1(0x8214BD,FakeDelete); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["pools"]={Id(matPoolG),Id(atmPoolG),Id(specPoolG),Id(objPipe)};
                    }
                    else if(nm=="CustomPipeAtomicSetup"){ objPipe=next()%4 ? static_cast<void*>(w.pipe) : reinterpret_cast<void*>(uintptr_t(next()&0xFFFF)); ret=Id(reinterpret_cast<void*(__cdecl*)(void*)>(a)(atomic)); }
                    else if(nm=="CustomPipeInstanceCB"){ static const int32 results[]{0,1,2,-1}; instanceResult=results[next()%4];
                        ret=reinterpret_cast<int32(__cdecl*)(void*,void*,void*)>(a)(reinterpret_cast<void*>(uintptr_t(0x4321+(next()&0xFF))),w.res,next()%4 ? reinterpret_cast<void*>(&FakeInstanceCB) : nullptr); }
                    else if(nm=="CustomPipeMaterialSetup"){ static const uint32 effects[]{0,1,2,2,3,4}; matfxEffect=effects[next()%6];
                        ret=Id(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(mat,reinterpret_cast<void*>(uintptr_t(next()%2 ? 0 : next()&0xFFFF)))); }
                    else if(nm=="CustomPipeRenderCB"){
                        uint8* h=w.res+0x18; const uint32 nMesh=next()%8==0 ? 0x80000001u : next()%(NMESH+1);
                        At<uint32>(h,4)=nMesh; At<uint32>(h,8)=next()%3 ? 0x2000u+next()%16 : 0u; At<uint32>(h,0xC)=1+next()%6;
                        At<uint32>(h,0x30)=next()%2; At<uint32>(h,0x34)=0x3000u+next()%16;
                        for(size_t k=0;k<NMESH;++k){ uint8* ms=h+0x40+0x24*k; At<void*>(ms,8)=w.mats[next()%NM]; At<uint32>(ms,0xC)=next()%3==0;
                            At<uint32>(ms,0x10)=0x4000u+next()%16; for(int q=0x14;q<0x24;q+=4)At<uint32>(ms,q)=next()%1000; }
                        // flags coherentes: brillo solo con datos de entorno, especular solo con datos especulares
                        for(auto& m:w.mats){ uint32 fl=next()%8; if(!At<void*>(m,envOff))fl&=~3u; if(!At<void*>(m,specOff))fl&=~4u; At<uint32>(m,0x10)=fl; }
                        lightingValue=next()%3 ? next()%2 : next(); fxQuality=int32(next()%4);
                        specIntensity=next()%4 ? uni(0.f,1.5f) : uni(1.5f,20.f);
                        for(int k=0;k<4;++k)ambSat[k]=uni(0.f,1.f);
                        { auto* g=reinterpret_cast<uint8*>(0xC02D38); for(int k=0;k<0x80;++k)g[k]=uint8(next()); for(int k=0;k<32;++k)reinterpret_cast<float*>(g)[k]=uni(-2.f,2.f); }
                        { auto* g=reinterpret_cast<float*>(0x8D12D8); for(int k=0;k<17;++k)g[k]=uni(-2.f,2.f); }
                        for(int k=0;k<5;++k)At<uint32>(reinterpret_cast<void*>(0x8E2440),4*k)=0xDEAD0000u+k;
                        static const uint32 flagsSet[]{0,8,4,0x84,0x80,0x8C,0x40,0x48,0x44,0xC8,0x4C,0x2C};
                        const uint32 type=1u|(next()%4==0 ? 0xFF00u : 0u), geoFl=flagsSet[next()%std::size(flagsSet)];
                        void* const savedFp=fpRS; void* const savedDev=device;
                        fpRS=reinterpret_cast<void*>(&FakeRenderStateSet); device=fakeDevice;
                        reinterpret_cast<void(__cdecl*)(void*,void*,uint32,uint32)>(a)(w.res,atomic,type,geoFl);
                        device=savedDev; fpRS=savedFp;   // antes de construir el JSON
                        out["matrices"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC02D38),0x80);
                        out["material"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D12D8),0x44);
                        out["caches"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8E2440),0x14);
                    }
                    else if(nm.rfind("GetFxEnv",0)==0||nm.rfind("GetFxSpec",0)==0){
                        if(nm=="GetFxSpecTexture")ret=Id(reinterpret_cast<void*(__cdecl*)(void*)>(a)(mat));
                        else ret=F(reinterpret_cast<float(__cdecl*)(void*)>(a)(mat));
                    }
                    else if(nm=="PreRenderUpdate"){ pDirectG=w.light; reinterpret_cast<void(__cdecl*)()>(a)(); out["light"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC02CB0),0x68); }
                    else if(nm=="RegisterPlugin"){
                        for(auto& r:regResults)r=next()%6==0 ? -int32(1+next()%3) : int32(0x20+4*(next()%16));
                        ret=reinterpret_cast<int32(__cdecl*)()>(a)();
                        out["offsets"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D12C4),12);
                    }
                    else if(nm=="SetCustomEnvMapPipeMaterialDataDefaults") reinterpret_cast<void(__cdecl*)(void*)>(a)(w.buf12);
                    else if(nm=="SetFxEnvScale"||nm=="SetFxEnvTransScl"||nm=="SetFxEnvShininess"){
                        auto fv=[&]{ const uint32 k=next()%12; return k==0 ? std::bit_cast<float>(0x7FC00000u) : k==1 ? std::bit_cast<float>(0x7F800000u) : k==2 ? -std::bit_cast<float>(0x7F800000u)
                            : k==3 ? uni(-1e10f,1e10f) : k==4 ? uni(-3.f,3.f)*1e38f : k==5 ? float(int32(next()%512)-256)/8.f : uni(-20.f,20.f); };
                        const float x=fv(), y=fv();
                        if(nm=="SetFxEnvShininess")reinterpret_cast<void(__cdecl*)(void*,float)>(a)(mat,x);
                        else reinterpret_cast<void(__cdecl*)(void*,float,float)>(a)(mat,x,y);
                    }
                    else if(nm=="SetFxEnvTexture") reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(mat,next()%3 ? nullptr : static_cast<void*>(w.texs[next()%NT]));
                    else if(nm=="pluginEnvAtmConstructorCB"||nm=="pluginEnvAtmDestructorCB"||nm=="pluginEnvMatConstructorCB"||nm=="pluginEnvMatDestructorCB"||nm=="pluginSpecMatConstructorCB"||nm=="pluginSpecMatDestructorCB"){
                        const bool isAtm=nm.find("Atm")!=std::string::npos, isSpec=nm.find("Spec")!=std::string::npos;
                        const int32 off=next()%4 ? (isAtm ? atmOff : isSpec ? specOff : envOff) : altOff;
                        ret=Id(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(a)(isAtm ? atomic : mat,off,4));
                    }
                    else if(nm=="pluginEnvAtmCopyConstructorCB"||nm=="pluginEnvMatCopyConstructorCB"||nm=="pluginSpecMatCopyConstructorCB"){
                        const bool isAtm=nm.find("Atm")!=std::string::npos, isSpec=nm.find("Spec")!=std::string::npos;
                        uint8* dst=isAtm ? w.atomics[next()%NA] : w.mats[next()%NM]; uint8* src=isAtm ? w.atomics[next()%NA] : w.mats[next()%NM];
                        const int32 off=next()%4 ? (isAtm ? atmOff : isSpec ? specOff : envOff) : altOff;
                        if(next()%4==0)At<void*>(dst,off)=nullptr;
                        static const int32 sizes[]{4,4,4,4,8,12,0,1};
                        ret=Id(reinterpret_cast<void*(__cdecl*)(void*,const void*,int32,int32)>(a)(dst,src,off,sizes[next()%std::size(sizes)]));
                    }
                    else if(nm=="pluginEnvMatStreamGetSizeCB"||nm=="pluginSpecMatStreamGetSizeCB"){
                        if(nm=="pluginSpecMatStreamGetSizeCB"&&next()%3==0)specOffG=-1;
                        ret=reinterpret_cast<int32(__cdecl*)(const void*,int32,int32)>(a)(next()%4 ? static_cast<void*>(mat) : nullptr,int32(next()%0x200),4);
                    }
                    else if(nm=="pluginEnvMatStreamReadCB"){
                        fakeData[5]=0;
                        if(next()%3==0){ // flujo que reproduce exactamente los datos falsos
                            fakeData[6]=fakeData[7]=0; for(int k=0;k<4;++k)At<float>(streamSrc.data(),4*k)=float(int8(fakeData[k]))/8.f;
                            At<float>(streamSrc.data(),16)=(float(fakeData[4])+0.25f)/255.f; At<uint32>(streamSrc.data(),20)=At<uint32>(fakeData.data(),8);
                            if(next()%4==0)streamSrc[next()%20]^=0x40; }
                        else { for(int k=0;k<5;++k)At<float>(streamSrc.data(),4*k)=next()%8==0 ? uni(-1e10f,1e10f) : uni(-16.f,16.f); At<uint8*>(streamSrc.data(),20)=tex(); }
                        using ReadFn=RwStream*(__cdecl*)(RwStream*,int32,void*,int32,int32);
                        const auto fnp=reinterpret_cast<ReadFn>(a); RwStream* const st=reinterpret_cast<RwStream*>(w.stream); void* const obj=mat;
                        const int32 off=next()%4 ? envOff : altOff;
                        ClearStack(); RwStream* const r=fnp(st,24,obj,off,4);
                        ret=Id(r);
                    }
                    else if(nm=="pluginEnvMatStreamWriteCB"){
                        const int32 off=next()%4 ? envOff : altOff; if(!At<void*>(mat,off))At<void*>(mat,off)=fakeData.data();
                        ret=Id(reinterpret_cast<RwStream*(__cdecl*)(RwStream*,int32,const void*,int32,int32)>(a)(reinterpret_cast<RwStream*>(w.stream),24,mat,off,4));
                    }
                    else if(nm=="pluginSpecMatStreamReadCB"){
                        At<void*>(mat,specOff)=next()%4 ? nullptr : static_cast<void*>(usedSpec());
                        const uint32 k=next()%8; At<float>(streamSrc.data(),0)=k==0 ? 0.f : k==1 ? -0.f : k==2 ? std::bit_cast<float>(0x7FC00000u) : uni(0.f,2.f);
                        std::memset(streamSrc.data()+4,0,24); const char* nmx=texNames[next()%std::size(texNames)]; std::memcpy(streamSrc.data()+4,nmx,std::min<size_t>(strlen(nmx),23));
                        texReadResult=reinterpret_cast<RwTexture*>(tex());
                        ret=Id(reinterpret_cast<RwStream*(__cdecl*)(RwStream*,int32,void*,int32,int32)>(a)(reinterpret_cast<RwStream*>(w.stream),28,mat,int32(next()%0x200),4));
                    }
                    else if(nm=="pluginSpecMatStreamWriteCB")
                        ret=Id(reinterpret_cast<RwStream*(__cdecl*)(RwStream*,int32,const void*,int32,int32)>(a)(reinterpret_cast<RwStream*>(w.stream),28,mat,int32(next()%0x200),4));

                    // estado observable
                    out["fake"]=ReversingProbe::Bytes(fakeData.data(),12);
                    out["globals"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC02D24),16);
                    out["world"]=Fnv(&w,sizeof(World)); out["ret"]=ret; out["recs"]=Recs();
                    RestoreAll();
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
    }
    restoreHooks();
    RestoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",AllRestored()}});
}
}
