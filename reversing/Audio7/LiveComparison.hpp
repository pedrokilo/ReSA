#pragma once
// Banco dentro del motor: audio, pipeline de edificios y decisiones (lista de 7). 42 funciones de
// CAEFireAudioEntity, CAEMP3TrackLoader, CAETwinLoopSoundEntity, CAEWaterCannonAudioEntity,
// CCustomBuildingPipeline y CDecisionMakerTypesFileLoader. Original (hooks del lote desactivados) frente a
// candidato con fixtures deterministas. Nada toca el hardware de audio, los bancos, los streams ni ficheros:
// se simulan y trazan el banco y el gestor de sonidos, CAESound (posicion, paradas), FxSystem_c, CFileMgr,
// CMemoryMgr, operator new/delete (solo durante la llamada), la API Win32 de ficheros (solo hilo principal,
// por IAT), CAEDataStream::Initialise/FillBuffer, getDvdGamePath, las primitivas RW/RxPipeline, el estado D3D9
// (dispositivo falso y cachés) y CDecisionMakerTypes (alta, baja e indices). El estado global tocado se
// guarda y se restaura en cada ejecucion.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/AESound.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/Managers/AESoundManager.h"
#include "Audio/Entities/AEFireAudioEntity.h"
#include "Audio/Entities/AETwinLoopSoundEntity.h"
#include "Audio/Entities/AEWaterCannonAudioEntity.h"
#include "Audio/Loaders/AEMP3TrackLoader.h"
#include "Audio/Loaders/AEDataStream.h"
#include "Fx/FxSystem.h"
#include "FileMgr.h"
#include "MemoryMgr.h"
#include "Pipelines/CustomBuilding/CustomBuildingPipeline.h"
#include "Pipelines/CustomBuilding/CustomBuildingDNPipeline.h"
#include "Pipelines/CustomCarEnvMap/CustomCarEnvMapPipeline.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "DecisionMakers/DecisionMakerTypes.h"
#include "../ProbeRuntime.hpp"

char* getDvdGamePath();

namespace Audio7Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- registro sin reservas (operator new/delete sustituidos) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- Win32 por IAT (solo el hilo principal; los demas hilos van a la API real) ---
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }
inline HMODULE SelfModule(){ HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&SelfModule),&m); return m; }
inline uintptr_t RealApi(const char* fn){ if(auto m=GetModuleHandleA("kernel32.dll"))if(auto p=GetProcAddress(m,fn))return uintptr_t(p); return 0; }
struct IatPatch {
    std::vector<std::pair<uintptr_t*,uintptr_t>> slots;
    static void Set(uintptr_t* t,uintptr_t v){ DWORD o{}; VirtualProtect(t,4,PAGE_READWRITE,&o); *t=v; VirtualProtect(t,4,o,&o); }
    template<class Fp> IatPatch(const char* fn,Fp fakeFn) {
        const auto fake=reinterpret_cast<uintptr_t>(fakeFn);
        std::array<uintptr_t,2> reals{RealApi(fn),0};
        if(auto kb=GetModuleHandleA("kernelbase.dll"))reals[1]=uintptr_t(GetProcAddress(kb,fn));
        for(HMODULE m:{GetModuleHandleA(nullptr),SelfModule()}) {
            auto* base=reinterpret_cast<uint8*>(m); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
            const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)continue;
            for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
                for(auto* t=reinterpret_cast<uintptr_t*>(base+d->FirstThunk);*t;++t)
                    if(*t==reals[0]||(reals[1]&&*t==reals[1])){ slots.push_back({t,*t}); Set(t,fake); }
        }
    }
    ~IatPatch(){ for(auto& [t,v]:slots)Set(t,v); }
};
inline uint32 fileState{}, fileMode{};
inline HANDLE WINAPI FakeCreateFileA(LPCSTR n,DWORD acc,DWORD share,LPSECURITY_ATTRIBUTES sa,DWORD disp,DWORD fl,HANDLE tpl){
    if(!Main())return reinterpret_cast<decltype(&CreateFileA)>(RealApi("CreateFileA"))(n,acc,share,sa,disp,fl,tpl);
    const bool ok=(fileMode>>(std::strstr(n,"TRAKLKUP") ? 1 : 0))&1;
    Push(20,uint32(Fnv(n,std::strlen(n))),acc^(share<<8)^(disp<<16)^(fl<<24)^uint32(uintptr_t(sa))^uint32(uintptr_t(tpl))); return ok ? reinterpret_cast<HANDLE>(0x7770) : INVALID_HANDLE_VALUE; }
inline uint32 fileSize{}, fileShort{}, fileKind{};
inline DWORD WINAPI FakeGetFileSize(HANDLE h,LPDWORD hi){
    if(!Main())return reinterpret_cast<decltype(&GetFileSize)>(RealApi("GetFileSize"))(h,hi);
    Push(21,uint32(uintptr_t(h)),uint32(uintptr_t(hi))); return fileSize; }
inline BOOL WINAPI FakeReadFile(HANDLE h,LPVOID buf,DWORD n,LPDWORD rd,LPOVERLAPPED ov){
    if(!Main())return reinterpret_cast<decltype(&ReadFile)>(RealApi("ReadFile"))(h,buf,n,rd,ov);
    auto* b=static_cast<uint8*>(buf); const DWORD got=n>fileShort ? n-fileShort : n;
    for(DWORD i=0;i<got;++i){ fileState=fileState*1103515245u+12345u; b[i]=uint8(fileState>>16); }
    if(fileKind==0)for(DWORD i=0;i+16<=got;i+=16){ char tmp[17]{}; std::snprintf(tmp,sizeof(tmp),"PK%03u",(i/16+fileState)%1000u); std::memcpy(b+i,tmp,16); }
    if(rd)*rd=got; Push(22,uint32(uintptr_t(h))^uint32(uintptr_t(ov)),n); return TRUE; }
inline BOOL WINAPI FakeCloseHandle(HANDLE h){
    if(!Main())return reinterpret_cast<decltype(&CloseHandle)>(RealApi("CloseHandle"))(h);
    Push(23,uint32(uintptr_t(h))); return TRUE; }

// --- operator new/delete y CMemoryMgr: arenas fijas con asignacion secuencial ---
alignas(16) inline std::array<uint8,0x8000> newArena{}; inline size_t newTop{}; inline uint32 newNullSize{};
inline void* __cdecl FakeNew(size_t n){ const size_t off=newTop; Push(1,uint32(n),uint32(off)); if(n==newNullSize||off+n>newArena.size())return nullptr; newTop=(off+n+15)&~size_t(15); return newArena.data()+off; }
inline void __cdecl FakeDelete(void* p){ Push(2,p ? uint32(intptr_t(p)-intptr_t(newArena.data())) : 0xFFFFFFFFu); }
alignas(16) inline std::array<uint8,0x4000> mallocArena{}; inline size_t mallocTop{};
inline void* __cdecl FakeMalloc(uint32 n,uint32 hint){ const size_t off=mallocTop; Push(24,n,hint); if(off+n>mallocArena.size())return nullptr; mallocTop=(off+n+15)&~size_t(15); return mallocArena.data()+off; }
inline void __cdecl FakeFree(void* p){ Push(3,uint32(uintptr_t(p))); }

// --- audio ---
alignas(16) inline std::array<std::array<uint8,0x74>,8> fakeSnd{}; alignas(16) inline std::array<uint8,0x74> updSnd{};
inline uint8* entityBase{}; inline size_t entitySize{};
inline json P(const void* p){
    if(!p)return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<fakeSnd.size();++k)if(u==uintptr_t(fakeSnd[k].data()))return json::array({"snd",k});
    if(u==uintptr_t(updSnd.data()))return "upd";
    if(entityBase&&u>=uintptr_t(entityBase)&&u<uintptr_t(entityBase)+entitySize)return json::array({"obj",u-uintptr_t(entityBase)});
    return "other"; }
inline uint32 bankLoadedMask{}, bankCalls{}, requestMask{}, requestCalls{}, nextSnd{}, playMask{}, playCalls{};
inline bool __fastcall FakeIsBankLoaded(void*,void*,eSoundBank b,eSoundBankSlot s){ trace.push_back({"is_bank_loaded",int(b),int(s)}); return (bankLoadedMask>>(bankCalls++%32))&1; }
inline void __fastcall FakeLoadBank(void*,void*,eSoundBank b,eSoundBankSlot s){ trace.push_back({"load_bank",int(b),int(s)}); }
inline CAESound* __fastcall FakeRequestNewSound(void*,void*,CAESound* s){
    trace.push_back({"request",P(s),int(s->m_BankSlot),int(s->m_SoundID),P(s->m_AudioEntity),s->m_Event,F(s->m_ClientVariable),F(s->m_CurrPos.x),F(s->m_CurrPos.y),F(s->m_CurrPos.z),
                     F(s->m_Volume),F(s->m_RollOffFactor),F(s->m_Speed),F(s->m_SpeedVariance),F(s->m_Doppler),s->m_FrameDelay,s->m_Flags,s->m_PlayTime});
    return (requestMask>>(requestCalls++%32))&1 ? reinterpret_cast<CAESound*>(fakeSnd[nextSnd++%fakeSnd.size()].data()) : nullptr; }
inline void __fastcall FakeCancelOwned(void*,void*,CAEAudioEntity* e,bool full){ trace.push_back({"cancel_owned",P(e),full}); }
inline void __fastcall FakeStopAndForget(CAESound* s,void*){ trace.push_back({"stop_forget",P(s)}); }
inline void __fastcall FakeStopSound(CAESound* s,void*){ trace.push_back({"stop",P(s)}); }
// m_tempSound y las CAESound locales/temporales se registran igual: depende de si CAESound::Initialise esta enganchado
inline void __fastcall FakeSetPosition(CAESound* s,void*,CVector pos){ json who=P(s); if(who=="other"||(who.is_array()&&who[0]=="obj"))who="tmp"; trace.push_back({"set_pos",who,F(pos.x),F(pos.y),F(pos.z)}); s->m_CurrPos=pos; }
inline void __fastcall FakeCompositeMatrix(void* fx,void*,RwMatrix* m){ trace.push_back({"composite",uintptr_t(fx)&0xFFF}); auto* f=reinterpret_cast<float*>(m); for(int k=0;k<16;++k)f[k]=float(int((uintptr_t(fx)>>4)&0xFF))+float(k)*0.5f+float(playCalls); }
inline int32 __fastcall FakePlayStatus(void* fx,void*){ trace.push_back({"play_status",uintptr_t(fx)&0xFFF}); return int32((playMask>>(2*(playCalls++%16)))&3); }

// --- CFileMgr / getDvdGamePath / CAEDataStream ---
// Estos stubs y los de Win32/CMemoryMgr usan Push (sin reservas): pueden correr con operator new sustituido.
inline uint32 openMask{}, openCalls{}, lineState{}, linesLeft{}, lineNo{}; inline char* dvdResult{};
inline std::vector<int32> validEvents;
inline int32 __cdecl FakeSetDir(const char* p){ Push(25,uint32(Fnv(p,std::strlen(p)))); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){
    const uint32 mode=(openMask>>(2*(openCalls%16)))&3; const uintptr_t h=mode==0 ? 0 : mode==1 ? 0x80000040u : 0x5000u+openCalls*16u; ++openCalls;
    Push(26,uint32(Fnv(p,std::strlen(p),Fnv(m,std::strlen(m)))),uint32(h)); linesLeft=1+lineState%6; lineNo=0; return reinterpret_cast<FILESTREAM>(h); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ Push(27,uint32(uintptr_t(f))); return 0; }
inline bool __cdecl FakeReadLine(FILESTREAM f,char* str,int32 num){
    Push(28,uint32(uintptr_t(f)),uint32(num)); if(lineNo>linesLeft)return false;
    auto next=[&]{ lineState^=lineState<<13; lineState^=lineState>>17; lineState^=lineState<<5; return lineState; };
    char buf[1024]; int len=0;
    if(lineNo==0) len=std::snprintf(buf,sizeof(buf),"data values2:\n");
    else { const int32 ev=validEvents.empty() ? 0 : validEvents[next()%validEvents.size()]; len=std::snprintf(buf,sizeof(buf),"%d,%u,",ev,next()%8);
        for(int c=0;c<6;++c){ const int32 task=next()%4 ? int32(200+next()%1200) : -1;
            len+=std::snprintf(buf+len,sizeof(buf)-len,"%d,%u.%06u,%u.%06u,%u.%06u,%u.%06u,%u,%u,%u,%u,%u,%u.5,%u,%u,",task,next()%101,next()%1000000,next()%101,next()%1000000,next()%101,next()%1000000,
                             next()%101,next()%1000000,next()%2,next()%2,next()%3,next()%3,next()%3,next()%3,next()%3,next()%3); }
        len+=std::snprintf(buf+len,sizeof(buf)-len,"\n"); }
    ++lineNo; const int n=std::min(len,num-1); std::memcpy(str,buf,n); str[n]=0; return true; }
inline char* __cdecl FakeDvdPath(){ Push(29); return dvdResult; }
inline bool __fastcall FakeStreamInit(CAEDataStream* s,void*){
    if(!s){ Push(30); return false; }
    auto* b=reinterpret_cast<uint8*>(s); const char* name=*reinterpret_cast<char**>(b+8);
    Push(31,*reinterpret_cast<uint32*>(b+0x1C),name ? uint32(Fnv(name,std::strlen(name))) : 0u); Push(32,*reinterpret_cast<uint32*>(b+0x14),*reinterpret_cast<uint32*>(b+0x18)); Push(33,b[0x20]|(b[0xC]<<8),*reinterpret_cast<uint32*>(b+0x10)^*reinterpret_cast<uint32*>(b+0x24));
    return true; }
inline size_t __fastcall FakeFillBuffer(CAEDataStream* s,void*,void* dst,size_t n){
    Push(34,uint32(intptr_t(dst)-intptr_t(newArena.data()))|(s ? 0x80000000u : 0u),uint32(n)); auto* d=static_cast<uint8*>(dst); for(size_t i=0;i<n;++i)d[i]=uint8(i*7+n); return n; }

// --- RW / RxPipeline / D3D9 ---
alignas(16) inline std::array<uint8,0x40> fakePipe{}, fakeNodeDef{}, fakeNode{}; inline uint32 rxMask{};
inline bool IsCode(uintptr_t a){ MEMORY_BASIC_INFORMATION mi{}; return a>=0x10000&&VirtualQuery(reinterpret_cast<void*>(a),&mi,sizeof(mi))&&mi.State==MEM_COMMIT&&(mi.Protect&(PAGE_EXECUTE|PAGE_EXECUTE_READ|PAGE_EXECUTE_READWRITE|PAGE_EXECUTE_WRITECOPY)); }
inline uintptr_t Final(uintptr_t a){ for(int d=0;d<4&&IsCode(a)&&*reinterpret_cast<uint8*>(a)==0xE9;++d)a=a+5+*reinterpret_cast<int32*>(a+1); return a; }
inline json Norm(const void* p){ const auto u=uintptr_t(p); if(!u)return nullptr; for(uintptr_t e:{0x5D77A0u,0x5D77D0u,0x5D7DB0u})if(u==e||Final(u)==Final(e))return e; return u<0x1000000 ? json(u) : json("dll"); }
inline json PipeName(const void* p){ const auto u=uintptr_t(p); if(u==uintptr_t(fakePipe.data()))return "pipe"; if(u==uintptr_t(fakeNode.data()))return "node"; if(u==uintptr_t(fakeNodeDef.data()))return "nodedef"; return u; }
inline RxPipeline* __cdecl FakePipelineCreate(){ trace.push_back({"pipe_create"}); return rxMask&1 ? reinterpret_cast<RxPipeline*>(fakePipe.data()) : nullptr; }
inline RxNodeDefinition* __cdecl FakeNodeDef(){ trace.push_back({"nodedef"}); return reinterpret_cast<RxNodeDefinition*>(fakeNodeDef.data()); }
inline void* __cdecl FakePipeLock(void* p){ trace.push_back({"lock",PipeName(p)}); return rxMask&2 ? p : nullptr; }
inline void* __cdecl FakeAddFragment(void* p,void* first,void* def,void* end){ trace.push_back({"add_fragment",PipeName(p),uintptr_t(first),PipeName(def),uintptr_t(end)}); return rxMask&4 ? p : nullptr; }
inline void* __cdecl FakeUnlock(void* p){ trace.push_back({"unlock",PipeName(p)}); return rxMask&8 ? p : nullptr; }
inline void* __cdecl FakeFindNode(void* p,const char* name,void* start,void* idx){ trace.push_back({"find_node",PipeName(p),uintptr_t(name)==*reinterpret_cast<uintptr_t*>(fakeNodeDef.data()),uintptr_t(start),uintptr_t(idx)}); return rxMask&16 ? fakeNode.data() : nullptr; }
inline void* __cdecl FakeGetInstanceCB(void* n){ trace.push_back({"get_instance_cb",PipeName(n)}); return reinterpret_cast<void*>(0x1111); }
inline void __cdecl FakeSetInstanceCB(void* n,void* cb){ trace.push_back({"set_instance_cb",PipeName(n),Norm(cb)}); }
inline void __cdecl FakeSetReinstanceCB(void* n,void* cb){ trace.push_back({"set_reinstance_cb",PipeName(n),Norm(cb)}); }
inline void __cdecl FakeSetRenderCB(void* n,void* cb){ trace.push_back({"set_render_cb",PipeName(n),Norm(cb)}); }
inline void __cdecl FakePipeDestroy(void* p){ trace.push_back({"pipe_destroy",PipeName(p)}); }
inline void* __cdecl FakeForAllMaterials(void* g,void* cb,void* d){ trace.push_back({"for_all_materials",uintptr_t(g)&0xFFFF,Norm(cb),uintptr_t(d)}); return g; }
inline void __cdecl FakeSetPipelineID(RpAtomic* a,RwUInt32 id){ trace.push_back({"set_pipeline_id",P(a),id}); }
// materiales y datos del mapa de entorno
alignas(16) inline std::array<std::array<uint8,0x200>,4> mats{}; alignas(16) inline std::array<std::array<uint8,0x10>,4> envs{}; alignas(16) inline std::array<std::array<uint8,0x60>,3> texs{};
alignas(16) inline std::array<std::array<uint8,0x40>,4> matfx{};
inline json M(const void* p){ const auto u=uintptr_t(p); if(!u)return nullptr;
    for(size_t k=0;k<mats.size();++k)if(u>=uintptr_t(mats[k].data())&&u<uintptr_t(mats[k].data())+0x200)return json::array({"mat",k,u-uintptr_t(mats[k].data())});
    for(size_t k=0;k<envs.size();++k)if(u==uintptr_t(envs[k].data()))return json::array({"env",k});
    for(size_t k=0;k<texs.size();++k)if(u==uintptr_t(texs[k].data()))return json::array({"tex",k});
    return u; }
inline uint32 matfxEffect{}, envMode{};
inline RpMatFXMaterialFlags __cdecl FakeGetEffects(const RpMaterial* m){ trace.push_back({"get_effects",M(m)}); return RpMatFXMaterialFlags(matfxEffect); }
inline CustomEnvMapPipeMaterialData* __cdecl FakeSetFxEnvTexture(CustomEnvMapPipeMaterialData** pp){
    trace.push_back({"set_fx_env_texture",M(pp),M(*pp)});
    if(envMode==1)*pp=reinterpret_cast<CustomEnvMapPipeMaterialData*>(envs[3].data()); else if(envMode==2)return nullptr; return *pp; }
// estado D3D9
inline uint32 lightingValue{};
inline void __cdecl FakeClipping(void* o,RwUInt32 t){ trace.push_back({"clipping",P(o),t}); }
inline void __cdecl FakeGetRenderState(RwUInt32 s,void* v){ trace.push_back({"get_rs",s}); *static_cast<uint32*>(v)=lightingValue; }
inline void __cdecl FakeSetRenderState(RwUInt32 s,RwUInt32 v){ trace.push_back({"set_rs",s,v}); }
inline void __cdecl FakeSetTSS(RwUInt32 st,RwUInt32 t,RwUInt32 v){ trace.push_back({"tss",st,t,v}); }
inline RwBool __cdecl FakeSetTexture(RwTexture* t,RwUInt32 st){ trace.push_back({"texture",M(t),st}); return TRUE; }
inline void __cdecl FakeSetStreams(const void* s,RwBool off){ trace.push_back({"streams",P(s),off}); }
inline RwBool __cdecl FakeSurface(const void* sp,const void* c,RwUInt32 fl){ trace.push_back({"surface",M(sp),M(c),fl}); return TRUE; }
inline void __cdecl FakeVertexAlpha(RwBool e){ trace.push_back({"vertex_alpha",e}); }
inline void __cdecl FakeFlush(){ trace.push_back({"flush"}); }
inline RwBool __cdecl FakeRenderStateSet(RwRenderState s,void* v){ trace.push_back({"rw_rs",int(s),uintptr_t(v)}); return TRUE; }
inline HRESULT __stdcall DevTrap(void*){ trace.push_back({"dev_trap"}); return 0; }
inline HRESULT __stdcall DevSetIndices(void*,void* p){ trace.push_back({"dev_indices",uintptr_t(p)}); return 0; }
inline HRESULT __stdcall DevSetDecl(void*,void* p){ trace.push_back({"dev_decl",uintptr_t(p)}); return 0; }
inline HRESULT __stdcall DevSetVS(void*,void* p){ trace.push_back({"dev_vs",uintptr_t(p)}); return 0; }
inline HRESULT __stdcall DevSetPS(void*,void* p){ trace.push_back({"dev_ps",uintptr_t(p)}); return 0; }
inline HRESULT __stdcall DevDrawIndexed(void*,uint32 t,int32 b,uint32 mn,uint32 nv,uint32 st,uint32 c){ trace.push_back({"dev_draw_indexed",t,b,mn,nv,st,c}); return 0; }
inline HRESULT __stdcall DevDraw(void*,uint32 t,uint32 st,uint32 c){ trace.push_back({"dev_draw",t,st,c}); return 0; }
inline std::array<void*,119> devVt{}; inline void* fakeDevice[2]{devVt.data(),nullptr};

// --- CDecisionMakerTypes ---
inline int32 addResult{};
inline void __fastcall FakeLoadEventIndices(void* self,void*){ trace.push_back({"load_event_indices",self==CDecisionMakerTypes::GetInstance()}); }
inline void __fastcall FakeRemoveDM(void* self,void*,int32 dm){ trace.push_back({"remove_dm",self==CDecisionMakerTypes::GetInstance(),dm}); }
inline int32 __fastcall FakeAddDM(void* self,void*,CDecisionMaker* dm,int32 type,bool mission){ trace.push_back({"add_dm",self==CDecisionMakerTypes::GetInstance(),Fnv(dm,sizeof(CDecisionMaker)),type,mission}); return addResult; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,42> fns{{
    {"CAEFireAudioEntity","Initialise",0x4DCF20,0,-1,64},{"CAEFireAudioEntity","StaticInitialise",0x5B9A90,0,-1,64},{"CAEFireAudioEntity","Terminate",0x4DCF40,0,-1,64},
    {"CAEFireAudioEntity","AddAudioEvent",0x4DD3C0,0,-1,512},{"CAEFireAudioEntity","PlayFireSounds",0x4DD0D0,0,-1,512},{"CAEFireAudioEntity","PlayWaterSounds",0x4DD270,0,-1,256},
    {"CAEFireAudioEntity","UpdateParameters",0,0x85AA94,0,1024},
    {"CAETwinLoopSoundEntity","Initialise",0x4F28A0,0,-1,128},{"CAETwinLoopSoundEntity","UpdateTwinLoopSound",0x4F29A0,0,-1,256},{"CAETwinLoopSoundEntity","UpdateParameters",0,0x85F438,0,512},
    {"CAETwinLoopSoundEntity","SwapSounds",0x4F2C10,0,-1,256},{"CAETwinLoopSoundEntity","StopSoundAndForget",0x4F2960,0,-1,128},{"CAETwinLoopSoundEntity","DoSoundsSwitchThisFrame",0x4F2CA0,0,-1,256},
    {"CAETwinLoopSoundEntity","PlayTwinLoopSound",0x4F2CB0,0,-1,512},
    {"CAEWaterCannonAudioEntity","Constructor",0x728A90,0,-1,64},{"CAEWaterCannonAudioEntity","Initialise",0x503060,0,-1,64},{"CAEWaterCannonAudioEntity","Terminate",0x5030A0,0,-1,128},
    {"CAEWaterCannonAudioEntity","SetSplashInfo",0x502F50,0,-1,128},{"CAEWaterCannonAudioEntity","ClearSplashInfo",0x502F20,0,-1,64},{"CAEWaterCannonAudioEntity","Service",0x5030D0,0,-1,1024},
    {"CAEWaterCannonAudioEntity","UpdateParameters",0,0x872A60,0,256},
    {"CAEMP3TrackLoader","Constructor",0x4E0930,0,-1,64},{"CAEMP3TrackLoader","Deconstructor",0x4E0940,0,-1,128},{"CAEMP3TrackLoader","Initialise",0x4E0C50,0,-1,512},
    {"CAEMP3TrackLoader","LoadStreamPackTable",0x4E0970,0,-1,256},{"CAEMP3TrackLoader","LoadTrackLookupTable",0x4E09F0,0,-1,256},{"CAEMP3TrackLoader","GetTrackInfo",0x4E0A70,0,-1,512},
    {"CAEMP3TrackLoader","IsCurrentAudioStreamAvailable",0x4E0BD0,0,-1,256},
    {"CCustomBuildingPipeline","CreatePipe",0x5D7D90,0,-1,128},{"CCustomBuildingPipeline","DestroyPipe",0x5D7380,0,-1,64},{"CCustomBuildingPipeline","CustomPipeAtomicSetup",0x5D7E50,0,-1,128},
    {"CCustomBuildingPipeline","CreateCustomObjPipe",0x5D7AA0,0,-1,256},{"CCustomBuildingPipeline","CustomPipeMaterialSetup",0x5D7DB0,0,-1,512},{"CCustomBuildingPipeline","CustomPipeRenderCB",0x5D77D0,0,-1,1024},
    {"CCustomBuildingPipeline","CustomPipeReinstanceCB",0x5D77A0,0,-1,256},
    {"CDecisionMakerTypesFileLoader","ReStart",0x607D00,0,-1,64},{"CDecisionMakerTypesFileLoader","GetPedDMName",0x600860,0,-1,64},{"CDecisionMakerTypesFileLoader","GetGrpDMName",0x600880,0,-1,64},
    {"CDecisionMakerTypesFileLoader","UnloadDecisionMaker",0x607A70,0,-1,128},{"CDecisionMakerTypesFileLoader","LoadDefaultDecisionMaker",0x5BF400,0,-1,64},
    {"CDecisionMakerTypesFileLoader","LoadDecisionMaker-enum",0x607D30,0,-1,256},{"CDecisionMakerTypesFileLoader","LoadDecisionMaker-ptr",0x6076B0,0,-1,512},
}};

inline int32 __cdecl FakeReinstanceInstanceCB(void* o,void* h,RwBool r){ trace.push_back({"instance_cb",uintptr_t(o)&0xFFFF,uintptr_t(h)&0xFFFF,r}); return int32(addResult); }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio7-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audio7-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio7");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"audio7-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,42> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,42> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto& fireIdx=*reinterpret_cast<uint8*>(0xB612EC); const uint8 savedFireIdx=fireIdx;
    auto& pauseTime=*reinterpret_cast<uint32*>(0xB7CB7C); const uint32 savedPause=pauseTime;
    auto& objPipe=*reinterpret_cast<void**>(0xC02C68); void* const savedObjPipe=objPipe;
    auto* texMat=reinterpret_cast<uint8*>(0xC02C70); std::array<uint8,0x40> savedTexMat{}; std::memcpy(savedTexMat.data(),texMat,0x40);
    auto* caches=reinterpret_cast<uint8*>(0x8E2440); std::array<uint8,0x14> savedCaches{}; std::memcpy(savedCaches.data(),caches,0x14);
    auto& device=*reinterpret_cast<void**>(0xC97C28);
    auto* inst=CDecisionMakerTypes::GetInstance(); auto* defDm=reinterpret_cast<uint8*>(&inst->m_DefaultRandomPedDecisionMaker); constexpr size_t NDEF=5*sizeof(CDecisionMaker);
    const std::vector<uint8> savedDefDm(defDm,defDm+NDEF);
    validEvents.clear(); for(size_t e=0;e<inst->m_EventIndices.size();++e)if(inst->m_EventIndices[e]>=0&&inst->m_EventIndices[e]<CDecision::MAX_NUM_DECISIONS)validEvents.push_back(int32(e));
    for(auto& v:devVt)v=reinterpret_cast<void*>(&DevTrap);
    devVt[81]=reinterpret_cast<void*>(&DevDraw); devVt[82]=reinterpret_cast<void*>(&DevDrawIndexed); devVt[87]=reinterpret_cast<void*>(&DevSetDecl);
    devVt[92]=reinterpret_cast<void*>(&DevSetVS); devVt[104]=reinterpret_cast<void*>(&DevSetIndices); devVt[107]=reinterpret_cast<void*>(&DevSetPS);
    const int32 envOff=CCustomCarEnvMapPipeline::ms_envMapPluginOffset, fxOff=MatFXMaterialDataOffset;
    const bool offsetsOk=envOff>=0x1C&&envOff<=0x1F8&&fxOff>=0x1C&&fxOff<=0x1F8&&std::abs(envOff-fxOff)>=4;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),FakeIsBankLoaded), a1e(0x4D88C0,FakeIsBankLoaded);
        EntryPatch a2(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),FakeLoadBank), a2e(0x4D88A0,FakeLoadBank);
        EntryPatch a3(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),FakeRequestNewSound), a3e(0x4EFB10,FakeRequestNewSound);
        EntryPatch a5(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOwnedByAudioEntity),FakeCancelOwned), a5e(0x4EFCD0,FakeCancelOwned);
        EntryPatch a6(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),FakeStopAndForget), a6e(0x4EF850,FakeStopAndForget);
        EntryPatch a7(bit_cast<uintptr_t>(&CAESound::StopSound),FakeStopSound), a7e(0x4EF1C0,FakeStopSound);
        EntryPatch a8(bit_cast<uintptr_t>(&CAESound::SetPosition),FakeSetPosition), a8e(0x4EF880,FakeSetPosition);
        EntryPatch x1(bit_cast<uintptr_t>(&FxSystem_c::GetCompositeMatrix),FakeCompositeMatrix), x1e(0x4AA8C0,FakeCompositeMatrix);
        EntryPatch x2(bit_cast<uintptr_t>(&FxSystem_c::GetPlayStatus),FakePlayStatus), x2e(0x4AA900,FakePlayStatus);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), f1e(0x5387D0,FakeSetDir);
        EntryPatch f2(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), f2e(0x538900,FakeOpenFile);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), f3e(0x5389D0,FakeCloseFile);
        EntryPatch f4(bit_cast<uintptr_t>(&CFileMgr::ReadLine),FakeReadLine), f4e(0x5389B0,FakeReadLine);
        EntryPatch f5(bit_cast<uintptr_t>(static_cast<char*(*)()>(&getDvdGamePath)),FakeDvdPath), f5e(0x747300,FakeDvdPath);
        EntryPatch m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc), m1e(0x72F420,FakeMalloc);
        EntryPatch m2(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), m2e(0x72F430,FakeFree);
        EntryPatch p1(0x8057B0,FakePipelineCreate), p2(0x7582E0,FakeNodeDef), p3(0x806990,FakePipeLock), p4(0x806BE0,FakeAddFragment), p5(0x805F40,FakeUnlock), p6(0x806B30,FakeFindNode);
        EntryPatch p7(0x757390,FakeGetInstanceCB), p8(0x757380,FakeSetInstanceCB), p9(0x7573A0,FakeSetReinstanceCB), p10(0x7573E0,FakeSetRenderCB), p11(0x805820,FakePipeDestroy);
        EntryPatch p12(0x74C790,FakeForAllMaterials), p13(0x72FC50,FakeSetPipelineID), p13d(bit_cast<uintptr_t>(static_cast<void(*)(RpAtomic*,RwUInt32)>(&SetPipelineID)),FakeSetPipelineID);
        EntryPatch p14(0x812140,FakeGetEffects), p15(0x5D9570,FakeSetFxEnvTexture), p15d(bit_cast<uintptr_t>(&CCustomBuildingDNPipeline::SetFxEnvTexture),FakeSetFxEnvTexture);
        EntryPatch d1(0x756D90,FakeClipping), d1d(bit_cast<uintptr_t>(&_rwD3D9EnableClippingIfNeeded),FakeClipping), d2(0x7FC320,FakeGetRenderState), d3(0x7FC2D0,FakeSetRenderState), d4(0x7FC340,FakeSetTSS);
        EntryPatch d5(0x7FDE70,FakeSetTexture), d6(0x7FA090,FakeSetStreams), d7(0x7FC4D0,FakeSurface), d8(0x7FE0A0,FakeVertexAlpha), d9(0x7FC200,FakeFlush);
        EntryPatch e1(0x600840,FakeLoadEventIndices), e2(0x6043A0,FakeRemoveDM), e3(0x607050,FakeAddDM);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if(cls=="CCustomBuildingPipeline"&&(nm=="CustomPipeMaterialSetup"||nm=="CustomPipeRenderCB")&&!offsetsOk){ Write({{"event","error"},{"reason","plugin offsets out of fixture range"},{"env",envOff},{"matfx",fxOff}}); passed=false; continue; }
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA7u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; nrec=0;
                    bankCalls=requestCalls=nextSnd=playCalls=openCalls=0; bankLoadedMask=next(); requestMask=next(); playMask=next(); openMask=next(); lineState=next()|1; entityBase=nullptr; entitySize=0;
                    for(auto& s:fakeSnd){ for(auto& b:s)b=uint8(next()); *reinterpret_cast<float*>(s.data()+0x14)=uni(-100.f,10.f); *reinterpret_cast<float*>(s.data()+0x1C)=uni(0.f,2.f); }
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto randPos=[&]{ return CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-50.f,200.f)}; };
                    auto sndOrNull=[&]{ return next()%3 ? static_cast<void*>(fakeSnd[next()%fakeSnd.size()].data()) : nullptr; };

                    if(cls=="CAEFireAudioEntity") {
                        alignas(16) std::array<uint8,0x88> obj{}; for(auto& b:obj)b=uint8(next()); entityBase=obj.data(); entitySize=obj.size();
                        *reinterpret_cast<uintptr_t*>(obj.data())=0x85AA94; *reinterpret_cast<uint32*>(obj.data()+0x10)=0;
                        alignas(16) static std::array<uint8,0x100> fx{};
                        *reinterpret_cast<void**>(obj.data()+0x7C)=sndOrNull(); *reinterpret_cast<void**>(obj.data()+0x80)=sndOrNull(); *reinterpret_cast<void**>(obj.data()+0x84)=next()%4 ? fx.data() : nullptr;
                        fireIdx=uint8(next()%4 ? next()%5 : next()%256);
                        if(nm=="Initialise")reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(obj.data(),next()%4 ? fx.data() : nullptr);
                        else if(nm=="StaticInitialise")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Terminate")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="UpdateParameters"){ for(auto& b:updSnd)b=uint8(next()); static const int32 evs[]{1,2,3,4,0,7,-1};
                            *reinterpret_cast<int32*>(updSnd.data()+0xC)=evs[next()%7];
                            auto fv=[&](float lo,float hi){ const uint32 k=next()%16; return k==0 ? std::bit_cast<float>(0x7FC00000u) : k==1 ? -30.f : k==2 ? -100.f : uni(lo,hi); };
                            *reinterpret_cast<float*>(updSnd.data()+0x10)=fv(-40.f,20.f); *reinterpret_cast<float*>(updSnd.data()+0x14)=fv(-120.f,20.f);
                            if(next()%3==0)*reinterpret_cast<void**>(obj.data()+0x7C+4*(next()%2))=updSnd.data();
                            static const int16 poss[]{-1,0,-5,1,250,3000}; const int16 pos=poss[next()%6];
                            reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(a)(obj.data(),reinterpret_cast<CAESound*>(updSnd.data()),pos);
                            out["sound"]=ReversingProbe::Bytes(updSnd.data(),updSnd.size()); }
                        else { CVector p=randPos(); int32 ev; switch(next()%5){case 0:ev=0x89;break;case 1:ev=int32(0x88+next()%6);break;case 2:ev=int32(next()%200);break;default:ev=next()%2 ? 0x82 : 0x8A;}
                            reinterpret_cast<void(__thiscall*)(void*,int32,CVector*)>(a)(obj.data(),ev,&p); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["snd"]=Fnv(fakeSnd.data(),sizeof(fakeSnd)); out["idx"]=fireIdx; fireIdx=savedFireIdx;
                    }
                    else if(cls=="CAETwinLoopSoundEntity") {
                        alignas(16) std::array<uint8,0xA8> obj{}; for(auto& b:obj)b=uint8(next()); entityBase=obj.data(); entitySize=obj.size();
                        *reinterpret_cast<uintptr_t*>(obj.data())=0x85F438; *reinterpret_cast<uint32*>(obj.data()+0x10)=0;
                        *reinterpret_cast<void**>(obj.data()+0xA0)=sndOrNull(); *reinterpret_cast<void**>(obj.data()+0xA4)=sndOrNull(); obj[0x98]=uint8(next()%2);
                        auto& mn=*reinterpret_cast<uint16*>(obj.data()+0x8E); auto& mx=*reinterpret_cast<uint16*>(obj.data()+0x90);
                        if(next()%4){ mn=uint16(next()%3000); mx=uint16(mn+next()%5000); }
                        for(int k=0;k<2;++k){ auto& pc=*reinterpret_cast<int16*>(obj.data()+0x9A+2*k); pc=next()%3==0 ? int16(-1) : next()%4 ? int16(next()%100) : int16(next()); }
                        pauseTime=next()%2 ? next()%1000000 : next(); auto& swap=*reinterpret_cast<uint32*>(obj.data()+0x94); swap=next()%3 ? pauseTime+next()%2000-1000 : next();
                        if(nm=="Initialise"){ reinterpret_cast<void(__thiscall*)(void*,int16,int16,int16,void*,uint16,uint16,int16,int16)>(a)(obj.data(),int16(next()%40),int16(next()%300),int16(next()%300),
                                next()%2 ? obj.data() : nullptr,uint16(next()%3000),uint16(next()%8000),next()%3 ? int16(next()%100) : int16(-1),next()%3 ? int16(next()%100) : int16(-1)); }
                        else if(nm=="UpdateTwinLoopSound")reinterpret_cast<void(__thiscall*)(void*,CVector,float,float)>(a)(obj.data(),randPos(),uni(-60.f,10.f),uni(0.f,2.f));
                        else if(nm=="UpdateParameters"){ static const int16 poss[]{-1,0,1,500}; void* s=next()%2 ? *reinterpret_cast<void**>(obj.data()+0xA0+4*(next()%2)) : sndOrNull();
                            reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(a)(obj.data(),static_cast<CAESound*>(s),poss[next()%4]); }
                        else if(nm=="SwapSounds")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="StopSoundAndForget")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="DoSoundsSwitchThisFrame")ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(obj.data());
                        else { static const int32 flags[]{0,1,0x10,0x80,0x200,0x1004};
                            reinterpret_cast<void(__thiscall*)(void*,CVector,float,float,float,float,int32)>(a)(obj.data(),randPos(),uni(-60.f,10.f),uni(0.f,2.f),uni(0.5f,4.f),uni(0.f,1.5f),flags[next()%6]); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["snd"]=Fnv(fakeSnd.data(),sizeof(fakeSnd)); pauseTime=savedPause;
                    }
                    else if(cls=="CAEWaterCannonAudioEntity") {
                        alignas(16) std::array<uint8,0xA0> obj{}; for(auto& b:obj)b=uint8(next()); entityBase=obj.data(); entitySize=obj.size();
                        *reinterpret_cast<uintptr_t*>(obj.data())=0x872A60; *reinterpret_cast<uint32*>(obj.data()+0x10)=0;
                        for(int k=0;k<4;++k)*reinterpret_cast<void**>(obj.data()+0x90+4*k)=sndOrNull();
                        alignas(16) static std::array<uint8,0x340> cannon{}; for(auto& b:cannon)b=uint8(next());
                        *reinterpret_cast<uint32*>(cannon.data())=next()%4 ? next() : 0; *reinterpret_cast<int16*>(cannon.data()+4)=int16(next()%32);
                        for(int k=0;k<32;++k){ cannon[0x30C+k]=uint8(next()%2); *reinterpret_cast<CVector*>(cannon.data()+0xC+12*k)=randPos(); }
                        bool maskTemp=false;
                        if(nm=="Constructor"){ obj.fill(0xCD); ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(obj.data()))-intptr_t(obj.data()); }
                        else if(nm=="Initialise")reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(obj.data(),next()%4 ? cannon.data() : nullptr);
                        else if(nm=="Terminate")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="ClearSplashInfo")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="SetSplashInfo")reinterpret_cast<void(__thiscall*)(void*,CVector,float)>(a)(obj.data(),randPos(),uni(0.f,5.f));
                        else if(nm=="UpdateParameters"){ static const int16 poss[]{-1,0,1,500}; void* s=next()%2 ? *reinterpret_cast<void**>(obj.data()+0x90+4*(next()%4)) : sndOrNull();
                            reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(a)(obj.data(),static_cast<CAESound*>(s),poss[next()%4]); }
                        else { obj[0x7C]=uint8(next()%5 ? 1 : 0); obj[0x7D]=uint8(next()%2); *reinterpret_cast<void**>(obj.data()+4)=cannon.data();
                            *reinterpret_cast<CVector*>(obj.data()+0x80)=randPos(); *reinterpret_cast<float*>(obj.data()+0x8C)=uni(0.f,3.f);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); maskTemp=true; }
                        if(maskTemp)std::fill(obj.begin()+8,obj.begin()+0x7C,uint8(0)); // m_tempSound: sus campos se comparan en la traza de la peticion
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["snd"]=Fnv(fakeSnd.data(),sizeof(fakeSnd));
                    }
                    else if(cls=="CAEMP3TrackLoader") {
                        alignas(16) std::array<uint8,0x1C> obj{}; for(auto& b:obj)b=uint8(next());
                        alignas(16) static std::array<uint8,12*64> lookups{}; alignas(16) static std::array<uint8,16*260> packs{};
                        static char dvdA[]="E:\\", dvdB[]="D:\\GAMES\\SA\\";
                        for(auto& b:lookups)b=uint8(next());
                        for(size_t k=0;k<260;++k){ std::memset(packs.data()+16*k,0,16); std::snprintf(reinterpret_cast<char*>(packs.data()+16*k),16,next()%8 ? "PK%03u.DAT" : "STREAM_%07u",unsigned(next()%1000)); }
                        newTop=0; mallocTop=0; newArena.fill(0xCD); mallocArena.fill(0xAB); newNullSize=0xFFFFFFFFu; fileState=next(); fileMode=next()%4 ? 3 : next()%4; fileShort=next()%4 ? 0 : next()%20; fileKind=0;
                        const uint32 dv=next()%3; dvdResult=dv==0 ? nullptr : dv==1 ? dvdA : dvdB;
                        auto& count=*reinterpret_cast<uint32*>(obj.data()+4); auto& packCount=*reinterpret_cast<uint32*>(obj.data()+8);
                        *reinterpret_cast<void**>(obj.data()+0xC)=lookups.data(); *reinterpret_cast<void**>(obj.data()+0x10)=packs.data();
                        count=next()%60; packCount=next()%256; obj[0x14]=uint8(next()%2); *reinterpret_cast<char**>(obj.data()+0x18)=next()%3 ? dvdB : dvdA;
                        for(size_t k=0;k<64;++k)lookups[12*k]=uint8(next()%4 ? next()%(packCount+3) : next()%256);
                        auto withNew=[&](auto&& call){ EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),dd1(0x8214BD,FakeDelete),dd2(0x8213AE,FakeDelete); call(); };
                        if(nm=="Constructor"){ obj.fill(0xCD); ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(obj.data()))-intptr_t(obj.data()); }
                        else if(nm=="Deconstructor"){ obj[0]=uint8(next()%2); if(next()%3==0)*reinterpret_cast<void**>(obj.data()+0x18)=nullptr; else *reinterpret_cast<void**>(obj.data()+0x18)=newArena.data()+0x100;
                            withNew([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); }); }
                        else if(nm=="LoadStreamPackTable"||nm=="LoadTrackLookupTable"){ fileKind=nm=="LoadStreamPackTable" ? 0 : 1; fileSize=next()%4 ? (fileKind ? 12u : 16u)*(next()%64) : next()%0x500;
                            { IatPatch i1("CreateFileA",FakeCreateFileA), i2("GetFileSize",FakeGetFileSize), i3("ReadFile",FakeReadFile), i4("CloseHandle",FakeCloseHandle);
                              ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(obj.data()); }
                            out["arena"]=Fnv(mallocArena.data(),mallocTop); }
                        else if(nm=="Initialise"){ fileSize=16u*(1+next()%32); fileKind=0;
                            { IatPatch i1("CreateFileA",FakeCreateFileA), i2("GetFileSize",FakeGetFileSize), i3("ReadFile",FakeReadFile), i4("CloseHandle",FakeCloseHandle);
                              bool r{}; withNew([&]{ r=reinterpret_cast<bool(__thiscall*)(void*)>(a)(obj.data()); }); ret=r; }
                            out["arena"]=Fnv(mallocArena.data(),mallocTop); out["new"]=Fnv(newArena.data(),newTop); }
                        else if(nm=="IsCurrentAudioStreamAvailable"){ *reinterpret_cast<void**>(obj.data()+0x10)=packs.data()+16*(next()%4); ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(obj.data()); }
                        else { const uint32 id=next()%5==0 ? next() : next()%(count+2); if(next()%16==0)newNullSize=0x28; uint8* r{};
                            { EntryPatch s1(bit_cast<uintptr_t>(&CAEDataStream::Initialise),FakeStreamInit), s1e(0x4DC2B0,FakeStreamInit), s2(bit_cast<uintptr_t>(&CAEDataStream::FillBuffer),FakeFillBuffer), s2e(0x4DC1C0,FakeFillBuffer);
                              withNew([&]{ r=reinterpret_cast<uint8*(__thiscall*)(void*,uint32)>(a)(obj.data(),id); }); } // json fuera: con operator new sustituido no se reserva nada
                            ret=r ? json(intptr_t(r-newArena.data())) : json(nullptr); if(r)out["info"]=Fnv(r,0x1F84);
                            if(nrec&&recs[0].tag==1&&recs[0].b<newArena.size())out["path"]=std::string(reinterpret_cast<char*>(newArena.data()+recs[0].b),strnlen(reinterpret_cast<char*>(newArena.data()+recs[0].b),64)); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["recs"]=Recs();
                    }
                    else if(cls=="CCustomBuildingPipeline") {
                        rxMask=next()%4 ? 0x1F : next(); for(auto& b:fakePipe)b=uint8(next()); static const char nodeName[]="nodeD3D9AtomicAllInOne.csl"; *reinterpret_cast<const char**>(fakeNodeDef.data())=nodeName;
                        for(auto& m:mats)for(auto& b:m)b=uint8(next()); for(auto& e:envs)for(auto& b:e)b=uint8(next()); for(auto& t:texs)for(auto& b:t)b=uint8(next()); for(auto& x:matfx)for(auto& b:x)b=uint8(next());
                        for(size_t k=0;k<mats.size();++k){ auto* m=mats[k].data(); *reinterpret_cast<void**>(m+envOff)=next()%5 ? envs[next()%3].data() : nullptr; *reinterpret_cast<void**>(m+fxOff)=matfx[k].data();
                            *reinterpret_cast<void**>(matfx[k].data()+4)=next()%4 ? texs[next()%3].data() : nullptr; *reinterpret_cast<void**>(m)=next()%3 ? texs[next()%3].data() : nullptr;
                            if(next()%3==0)m[7]=0xFF; }
                        for(auto& e:envs){ *reinterpret_cast<void**>(e.data()+8)=next()%4 ? texs[next()%3].data() : nullptr; if(next()%4==0)e[4]=0; }
                        objPipe=next()%4 ? reinterpret_cast<void*>(fakePipe.data()) : nullptr;
                        if(nm=="CreatePipe"){ ret=reinterpret_cast<bool(__cdecl*)()>(a)(); out["pipe"]=PipeName(objPipe); }
                        else if(nm=="DestroyPipe"){ reinterpret_cast<void(__cdecl*)()>(a)(); out["pipe"]=PipeName(objPipe); }
                        else if(nm=="CreateCustomObjPipe"){ ret=PipeName(reinterpret_cast<void*(__cdecl*)()>(a)()); out["fake"]=ReversingProbe::Bytes(fakePipe.data(),fakePipe.size()); }
                        else if(nm=="CustomPipeAtomicSetup"){ alignas(16) std::array<uint8,0x80> at{}; for(auto& b:at)b=uint8(next()); entityBase=at.data(); entitySize=at.size();
                            objPipe=reinterpret_cast<void*>(uintptr_t(next())); ret=P(reinterpret_cast<void*(__cdecl*)(void*)>(a)(at.data())); out["atomic"]=ReversingProbe::Bytes(at.data(),at.size()); }
                        else if(nm=="CustomPipeMaterialSetup"){ static const uint32 effects[]{0,1,2,2,3,4}; matfxEffect=effects[next()%6]; envMode=next()%3; const size_t k=next()%mats.size();
                            ret=M(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(mats[k].data(),reinterpret_cast<void*>(uintptr_t(next()%2 ? 0 : next()))));
                            out["mat"]=ReversingProbe::Bytes(mats[k].data(),mats[k].size()); out["envs"]=Fnv(envs.data(),sizeof(envs)); out["texs"]=Fnv(texs.data(),sizeof(texs)); }
                        else if(nm=="CustomPipeReinstanceCB"){ addResult=next()%3 ? 0 : int32(next()%3); alignas(16) std::array<uint8,0x80> res{};
                            ret=reinterpret_cast<int32(__cdecl*)(void*,void*,void*)>(a)(reinterpret_cast<void*>(0x4321),res.data(),next()%4 ? reinterpret_cast<void*>(&FakeReinstanceInstanceCB) : nullptr); }
                        else { alignas(16) std::array<uint8,0x18+0x40+0x24*4> res{}; for(auto& b:res)b=uint8(next()); entityBase=res.data(); entitySize=res.size();
                            uint8* h=res.data()+0x18; const uint32 nMesh=next()%8==0 ? 0x80000001u : next()%5;
                            *reinterpret_cast<uint32*>(h+4)=nMesh; *reinterpret_cast<uintptr_t*>(h+8)=next()%3 ? 0x2000+next()%16 : 0;
                            *reinterpret_cast<uint32*>(h+0x30)=next()%2; *reinterpret_cast<uintptr_t*>(h+0x34)=0x3000+next()%16; *reinterpret_cast<uint32*>(h+0xC)=1+next()%6;
                            for(int k=0;k<4;++k){ uint8* ms=h+0x40+0x24*k; *reinterpret_cast<void**>(ms+8)=mats[next()%mats.size()].data(); *reinterpret_cast<uint32*>(ms+0xC)=next()%3==0;
                                *reinterpret_cast<uintptr_t*>(ms+0x10)=0x4000+next()%16; for(int q=0x14;q<0x24;q+=4)*reinterpret_cast<uint32*>(ms+q)=next()%1000; }
                            for(auto& m:mats){ auto& fl=*reinterpret_cast<uint32*>(m.data()+0x10); fl=next()%3 ? (fl&~1u)|1u : fl&~1u; if(!*reinterpret_cast<void**>(m.data()+envOff))fl&=~1u; }
                            lightingValue=next()%3 ? next()%2 : next(); for(int k=0;k<0x40;++k)texMat[k]=uint8(next());
                            for(int k=0;k<5;++k)*reinterpret_cast<uint32*>(caches+4*k)=0xDEAD0000u+k;
                            auto& fp=*reinterpret_cast<void**>(*reinterpret_cast<uintptr_t*>(0xC97B24)+0x20); void* const savedFp=fp; void* const savedDev=device;
                            fp=reinterpret_cast<void*>(&FakeRenderStateSet); device=fakeDevice;
                            static const uint32 flagsSet[]{0,8,4,0x84,0x80,0x8C,0x10,0x2C};
                            reinterpret_cast<void(__cdecl*)(void*,void*,uint32,uint32)>(a)(res.data(),reinterpret_cast<void*>(uintptr_t(0x6000+next()%16)),uint32(1+next()%2)|(next()%4==0 ? 0xFF00u : 0u),flagsSet[next()%8]);
                            device=savedDev; fp=savedFp;
                            out["texmat"]=ReversingProbe::Bytes(texMat,0x40); out["caches"]=ReversingProbe::Bytes(caches,0x14);
                            std::memcpy(texMat,savedTexMat.data(),0x40); std::memcpy(caches,savedCaches.data(),0x14); }
                        objPipe=savedObjPipe;
                    }
                    else if(cls=="CDecisionMakerTypesFileLoader") {
                        openMask=next()%6 ? (next()|0xAAAAAAAAu) : next(); addResult=int32(next()%40);
                        alignas(16) static std::array<uint8,sizeof(CDecisionMaker)> dm{}; for(auto& b:dm)b=uint8(next());
                        static const char* names[]{"RANDOM.ped","m_norm.ped","Cop.ped","MISSION.grp","RANDOM2.grp","nonexistent.ped"};
                        if(nm=="ReStart")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="GetPedDMName"||nm=="GetGrpDMName"){ char buf[32]; std::memset(buf,'#',sizeof(buf)); buf[31]=0; const int32 idx=nm=="GetPedDMName" ? int32(next()%6) : 0;
                            reinterpret_cast<void(__cdecl*)(int32,char*)>(a)(idx,buf); buf[31]=0; ret=std::string(buf); }
                        else if(nm=="UnloadDecisionMaker")reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(next()%20));
                        else if(nm=="LoadDecisionMaker-enum")ret=reinterpret_cast<int32(__cdecl*)(const char*,int32,bool)>(a)(names[next()%6],int32(next()%2),next()%2!=0);
                        else if(nm=="LoadDecisionMaker-ptr"){ ret=reinterpret_cast<bool(__cdecl*)(const char*,void*)>(a)(names[next()%6],dm.data()); out["dm"]=ReversingProbe::Bytes(dm.data(),dm.size()); }
                        else { reinterpret_cast<void(__cdecl*)()>(a)(); out["defaults"]=Fnv(defDm,NDEF); std::memcpy(defDm,savedDefDm.data(),NDEF); }
                    }
                    out["ret"]=ret; out["trace"]=trace; out["recs"]=Recs(); return out;
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
    fireIdx=savedFireIdx; pauseTime=savedPause; objPipe=savedObjPipe; std::memcpy(texMat,savedTexMat.data(),0x40); std::memcpy(caches,savedCaches.data(),0x14); std::memcpy(defDm,savedDefDm.data(),NDEF);
    const bool restored=fireIdx==savedFireIdx&&pauseTime==savedPause&&objPipe==savedObjPipe&&std::memcmp(texMat,savedTexMat.data(),0x40)==0&&std::memcmp(caches,savedCaches.data(),0x14)==0&&std::memcmp(defDm,savedDefDm.data(),NDEF)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
