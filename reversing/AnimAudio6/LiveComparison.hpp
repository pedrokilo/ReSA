#pragma once
// Banco dentro del motor: audio, animacion, objetos de cinematica, puertas y video (lista de 6; 42 funciones):
// CAEBankLoader, CAEGlobalWeaponAudioEntity, CAnimBlendClumpData, CAnimBlendStaticAssociation, CCutsceneObject,
// CDoor y VideoPlayer. Original (hooks del grupo desactivados) frente a candidato con fixtures deterministas sobre
// objetos falsos. Nunca se tocan bancos, ficheros, CdStream, hardware de audio ni DirectShow reales: se simulan y
// trazan CFileMgr (Open/GetTotalSize/Read/Close), CdStreamOpen, operator new/delete (solo durante la llamada),
// CMemoryMgr (Malloc/Free/MallocAlign/FreeAlign), el hardware y el gestor de sonidos, CAESound (Initialise,
// RegisterWithPhysicalEntity, StopSound), CAudioEngine, FindPlayerCoors, rand, las busquedas de frames del plugin de
// animacion, RW (jerarquias, LTM, atomics, skin), sombras, luces, CWorld::ProcessVerticalLine, GetSpeed, las APIs de
// Win32/COM que usa VideoPlayer (GetClientRect, SetFocus, UpdateWindow, CoInitialize, CoCreateInstance) y objetos COM
// falsos con tablas virtuales trazadas. Todo el estado global tocado se guarda y se restaura en cada ejecucion.
// Todas las trazas usan un registro fijo sin reservas (las llamadas pueden tener operator new sustituido).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "FileMgr.h"
#include "MemoryMgr.h"
#include "AESound.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AEWeaponAudioEntity.h"
#include "AudioEngine.h"
#include "RealTimeShadowManager.h"
#include "Shadows.h"
#include "../ProbeRuntime.hpp"

namespace AnimAudio6Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- registro sin reservas ---
enum : uint32 { T_RAND=1, T_NEW, T_DELETE, T_MALLOC, T_MALLOCALIGN, T_FREE, T_FREEALIGN, T_OPEN, T_SIZE, T_READ, T_CLOSE, T_CDOPEN,
    T_BANKLOADED, T_LOADBANK, T_LOADTUNE, T_INIT, T_REGISTER, T_REQUEST, T_STOP, T_REPORT, T_WFIRE, T_WRELOAD, T_PLAYERCOORS, T_CLEANUP, T_PHYSDTOR,
    T_FINDBONE, T_FINDHASH, T_FRAMECB, T_ENTSETMODEL, T_CLUMPINIT, T_FORALLATOMICS, T_PHYSPC, T_RPHANIM, T_GETHIER, T_MATARRAY, T_LTM, T_FIRSTATOMIC,
    T_GETSKIN, T_DOSHADOW, T_STORESHADOW, T_SHOULDER, T_VERTICAL, T_ACTDIR, T_DEACTDIR, T_AMBIENT, T_REMOVELIGHTS, T_SETUPLIGHT, T_GETSPEED,
    T_CLIENTRECT, T_SETFOCUS, T_UPDATEWIN, T_COINIT, T_COCREATE, T_QI, T_RELEASE, T_RENDERFILE, T_RUN, T_STOPC, T_GETSTATE, T_GETEVENT, T_FREEEVENT,
    T_NOTIFY, T_OWNER, T_STYLE, T_DRAIN, T_WINPOS, T_COMTRAP };
inline std::array<uint32,1<<17> rb{}; inline size_t rn{};
inline void R(uint32 t,std::initializer_list<uint32> v={}){ if(rn+2+v.size()>rb.size())return; rb[rn++]=t; rb[rn++]=uint32(v.size()); for(auto x:v)rb[rn++]=x; }
inline json Recs(){ json r=json::array(); size_t k=0; while(k<rn){ json e=json::array(); e.push_back(rb[k]); const uint32 n=rb[k+1]; k+=2; for(uint32 j=0;j<n;++j)e.push_back(rb[k++]); r.push_back(e); } return r; }

// --- regiones con nombre (punteros como region<<24 | desplazamiento) ---
inline std::array<std::pair<uintptr_t,size_t>,64> regs{}; inline size_t nregs{};
inline void Reg(const void* p,size_t n){ if(nregs<regs.size())regs[nregs++]={uintptr_t(p),n}; }
inline uint32 Id(const void* p){ if(!p)return 0xFFFFFFFFu; const auto u=uintptr_t(p); for(size_t k=0;k<nregs;++k)if(u>=regs[k].first&&u<regs[k].first+regs[k].second)return uint32(k<<24|(u-regs[k].first)); return 0xFFFFFFFEu; }

// --- buferes falsos ---
alignas(64) inline std::array<uint8,0x40000> arena{}; inline size_t arenaTop{};
alignas(16) inline std::array<std::array<uint8,0x1000>,4> ent{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> mats{};
alignas(16) inline std::array<std::array<uint8,0x100>,2> frames{};
alignas(16) inline std::array<std::array<uint8,0x400>,2> clumps{};
alignas(16) inline std::array<uint8,0x40> cdata{};
alignas(16) inline std::array<uint8,64*0x18> fdatas{};
alignas(16) inline std::array<uint8,64*0x0C> seqs{};
alignas(16) inline std::array<uint8,0x20> hierBuf{};
alignas(16) inline std::array<uint8,64*0x40> matArr{};
alignas(16) inline std::array<std::array<uint8,0x80>,2> atomics{};
alignas(16) inline std::array<uint8,0x80> geom{}, morph{}, interp{};
alignas(16) inline std::array<std::array<uint8,0x80>,6> sounds{};
alignas(16) inline std::array<std::array<uint8,0x20>,2> links{};
alignas(16) inline std::array<uint8,0x700> loader{};
alignas(16) inline std::array<uint8,0x20> rbuf{};
struct FakeCom { void** vt; uint32 tag; };
inline std::array<FakeCom,5> coms{}; inline std::array<std::array<void*,48>,5> comVt{};
inline std::array<void*,16> physVt{};

inline void* Alloc(size_t n){ if(n>arena.size()-arenaTop)return nullptr; void* p=arena.data()+arenaTop; arenaTop+=(n+15)&~size_t(15); return p; }
inline bool InArena(const void* p){ const auto u=uintptr_t(p); return u>=uintptr_t(arena.data())&&u<uintptr_t(arena.data())+arena.size(); }

// --- dependencias simuladas ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand(){ R(T_RAND); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline uint32 maskA{}, callsA{}, maskB{}, callsB{}, maskC{}, callsC{};
inline bool BitA(){ return (maskA>>(callsA++%32))&1; }
inline bool BitB(){ return (maskB>>(callsB++%32))&1; }
inline bool BitC(){ return (maskC>>(callsC++%32))&1; }

inline void* __cdecl FakeNew(size_t n){ R(T_NEW,{uint32(n)}); return Alloc(n); }
inline void __cdecl FakeDelete(void* p){ R(T_DELETE,{Id(p)}); }
inline void* __cdecl FakeMalloc1(uint32 n){ R(T_MALLOC,{n,0}); return Alloc(n); }
inline void* __cdecl FakeMalloc2(uint32 n,uint32 h){ R(T_MALLOC,{n,h}); return Alloc(n); }
inline void* __cdecl FakeMallocAlign(uint32 n,uint32 a){ R(T_MALLOCALIGN,{n,a}); return Alloc(n); }
inline void __cdecl FakeFree(void* p){ R(T_FREE,{Id(p)}); }
inline void __cdecl FakeFreeAlign(void* p){ R(T_FREEALIGN,{Id(p)}); }

// CFileMgr y CdStream
inline uint32 openResult{}, totalSize{}, readState{}, readCalls{}, readFailMask{}, cdResult{}; inline int16 slotCount{}; inline bool pakMode{};
inline uint32 Str(const char* s){ return uint32(Fnv(s,strlen(s))); }
inline FILESTREAM __cdecl FakeOpen(const char* p,const char* m){ R(T_OPEN,{Str(p),Str(m)}); return reinterpret_cast<FILESTREAM>(uintptr_t(openResult)); }
inline int32 __cdecl FakeSize(FILESTREAM f){ R(T_SIZE,{uint32(uintptr_t(f))}); return int32(totalSize); }
inline size_t __cdecl FakeRead(FILESTREAM f,void* buf,size_t size){
    R(T_READ,{uint32(uintptr_t(f)),Id(buf),uint32(size)});
    auto* b=static_cast<uint8*>(buf);
    if(b==loader.data()+0xC&&size==2) std::memcpy(b,&slotCount,2);
    else if(InArena(b)){ const size_t n=std::min(size,size_t(uintptr_t(arena.data())+arena.size()-uintptr_t(b)));
        for(size_t k=0;k<n;++k){ readState=readState*1103515245u+12345u; b[k]=uint8(readState>>16); }
        if(pakMode) for(size_t r=0;r+52<=n;r+=52){ const uint32 len=1+b[r]%11; for(uint32 k=0;k<len;++k)b[r+k]=uint8('A'+b[r+k]%26); b[r+len]=0; } }
    const bool fail=(readFailMask>>(readCalls++%32))&1; return fail ? size/2 : size; }
inline int32 __cdecl FakeClose(FILESTREAM f){ R(T_CLOSE,{uint32(uintptr_t(f))}); return 0; }
inline CdStreamHandle __cdecl FakeCdOpen(const char* p){ R(T_CDOPEN,{Str(p),uint32(strlen(p))}); return cdResult++; }

// audio
inline bool __fastcall FakeIsBankLoaded(void*,void*,int32 b,int32 s){ R(T_BANKLOADED,{uint32(b)&0xFFFF,uint32(s)&0xFFFF}); return BitA(); }
inline void __fastcall FakeLoadBank(void*,void*,int32 b,int32 s){ R(T_LOADBANK,{uint32(b)&0xFFFF,uint32(s)&0xFFFF}); }
inline bool __fastcall FakeLoadingTune(void*,void*){ const bool r=BitB(); R(T_LOADTUNE,{r}); return r; }
inline void __fastcall FakeInitialise(void* s,void*,int32 slot,int32 sfx,void* e,CVector pos,float vol,float roll,float speed,float dop,uint32 fd,uint32 fl,float sv,uint32 pt){
    R(T_INIT,{Id(s),uint32(slot)&0xFFFF,uint32(sfx)&0xFFFF,Id(e),F(pos.x),F(pos.y),F(pos.z),F(vol),F(roll),F(speed),F(dop),fd&0xFF,fl&0xFFFF,F(sv),pt&0xFFFF}); }
inline void __fastcall FakeRegister(void* s,void*,void* e){ R(T_REGISTER,{Id(s),Id(e)}); }
inline std::array<void*,4> requestSeq{}; inline uint32 requestCalls{};
inline void* __fastcall FakeRequest(void* mgr,void*,void* s){ R(T_REQUEST,{uint32(uintptr_t(mgr)-0xB62CB0),Id(s),At<uint32>(s,0x18),At<uint32>(s,0x1C),At<uint16>(s,0x56),At<uint32>(s,8)}); return requestSeq[requestCalls++%4]; }
inline void __fastcall FakeStopSound(void* s,void*){ R(T_STOP,{Id(s)}); }
inline void __fastcall FakeReport(void* eng,void*,int32 ev,int32 w,void* p){ R(T_REPORT,{uint32(uintptr_t(eng)-0xB6BC90),uint32(ev),uint32(w),Id(p)}); }
inline void __fastcall FakeWeaponFire(void* self,void*,int32 w,void* e,int32 ev){ R(T_WFIRE,{Id(self),uint32(w),Id(e),uint32(ev)}); }
inline void __fastcall FakeWeaponReload(void* self,void*,int32 w,void* e,int32 ev){ R(T_WRELOAD,{Id(self),uint32(w),Id(e),uint32(ev)}); }
inline CVector playerCoors{};
inline CVector* __cdecl FakePlayerCoors(CVector* out,int32 id){ R(T_PLAYERCOORS,{uint32(id)}); *out=playerCoors; return out; }
inline void __fastcall FakeCleanUp(void* self,void*,void** ref){ R(T_CLEANUP,{Id(self),Id(ref)}); }
inline void* __fastcall FakePhysDtor(void* self,void*,uint32 flags){ R(T_PHYSDTOR,{Id(self),flags&0xFF}); return self; }

// plugin de animacion
inline uint32 frameMask{}, frameCalls{}, frameCount{};
inline void* FramePick(uint32 k){ const bool hit=(frameMask>>(frameCalls++%32))&1; return hit&&frameCount ? fdatas.data()+0x18*(k%frameCount) : nullptr; }
inline void* __cdecl FakeFindBone(void* c,int32 tag){ void* r=FramePick(uint32(tag)*7u+3u); R(T_FINDBONE,{Id(c),uint32(tag),Id(r)}); return r; }
inline void* __cdecl FakeFindHash(void* c,uint32 key){ void* r=FramePick(key*13u+1u); R(T_FINDHASH,{Id(c),key,Id(r)}); return r; }
inline uint32 cbChangeAt{}, cbNewCount{}, cbCalls{};
inline void __cdecl FrameCb(AnimBlendFrameData* fd,void* data){ R(T_FRAMECB,{Id(fd),Id(data)}); if(++cbCalls==cbChangeAt)At<uint32>(cdata.data(),8)=cbNewCount; }

// RW, entidades y render
inline void* rwObjResult{};
inline void __fastcall FakeEntSetModel(void* self,void*,uint32 idx){ R(T_ENTSETMODEL,{Id(self),idx}); At<void*>(self,0x18)=rwObjResult; At<int16>(self,0x22)=int16(idx); }
inline void __cdecl FakeClumpInit(void* c){ R(T_CLUMPINIT,{Id(c)}); }
inline RpClump* __cdecl FakeForAllAtomics(RpClump* c,void* cb,void* data){ const auto u=uintptr_t(cb); R(T_FORALLATOMICS,{Id(c),(u==0x5B1A90||u<0x400000||u>=0x1000000) ? 1u : uint32(u),Id(data)}); return c; }
inline void __fastcall FakePhysPC(void* self,void*){ R(T_PHYSPC,{Id(self)}); }
inline void __fastcall FakeRpHAnim(void* self,void*){ R(T_RPHANIM,{Id(self)}); }
inline void* __cdecl FakeGetHier(void* c){ R(T_GETHIER,{Id(c)}); return hierBuf.data(); }
inline void* __cdecl FakeMatArray(void* h){ R(T_MATARRAY,{Id(h)}); return matArr.data(); }
inline void* __cdecl FakeLTM(void* f){ R(T_LTM,{Id(f)}); return static_cast<uint8*>(f)+0x10; }
inline std::array<void*,2> firstAtomics{}; inline uint32 faCalls{};
inline void* __cdecl FakeFirstAtomic(void* c){ void* r=firstAtomics[faCalls++%2]; R(T_FIRSTATOMIC,{Id(c),Id(r)}); return r; }
inline void* skinResult{};
inline void* __cdecl FakeGetSkin(void* g){ R(T_GETSKIN,{Id(g)}); return skinResult; }
inline void __fastcall FakeDoShadow(void* mgr,void*,void* p){ R(T_DOSHADOW,{uint32(uintptr_t(mgr)-0xC40350),Id(p)}); }
inline void __cdecl FakeStoreShadow(void* ped,float a,float b,float c,float d,float e,float f){ R(T_STORESHADOW,{Id(ped),F(a),F(b),F(c),F(d),F(e),F(f)}); }
inline void __cdecl FakeShoulder(void* c){ R(T_SHOULDER,{Id(c)}); }
inline uint8 lightB{};
inline bool __cdecl FakeVertical(const CVector& o,float d,CColPoint& cp,CEntity*& e,bool b0,bool b1,bool b2,bool b3,bool b4,bool b5,CStoredCollPoly* poly){
    const bool r=BitC(); R(T_VERTICAL,{F(o.x),F(o.y),F(o.z),F(d),b0,b1,b2,b3,b4,b5,poly!=nullptr,r}); if(r){ reinterpret_cast<uint8*>(&cp)[0x25]=lightB; e=nullptr; } return r; }
inline void __cdecl FakeActDir(){ R(T_ACTDIR); }
inline void __cdecl FakeDeActDir(){ R(T_DEACTDIR); }
inline void __cdecl FakeAmbient(){ R(T_AMBIENT); }
inline void __cdecl FakeRemoveLights(){ R(T_REMOVELIGHTS); }
inline bool __cdecl FakeSetupLight(void* e){ const bool r=BitA(); R(T_SETUPLIGHT,{Id(e),r}); return r; }
inline CVector* __fastcall FakeGetSpeed(void* self,void*,CVector* out,CVector pt){ R(T_GETSPEED,{Id(self),F(pt.x),F(pt.y),F(pt.z)}); *out=CVector{pt.x*0.375f+0.125f,pt.y*-0.25f+0.5f,pt.z*0.5f+0.0625f}; return out; }

// Win32 y COM (VideoPlayer)
inline HWND gameWnd{};
inline uint32 W(const void* h){ return h==static_cast<const void*>(gameWnd) ? 1u : h ? 2u : 0u; }
inline RECT rectResult{};
inline uint32 callNo{}, failAt{};
inline HRESULT Hr(){ return ++callNo==failAt ? HRESULT(0x80004005) : (BitA() ? S_OK : S_FALSE); }
inline BOOL WINAPI FakeClientRect(HWND h,LPRECT r){ R(T_CLIENTRECT,{W(h)}); *r=rectResult; return TRUE; }
inline HWND WINAPI FakeSetFocus(HWND h){ R(T_SETFOCUS,{W(h)}); return h; }
inline BOOL WINAPI FakeUpdateWindowApi(HWND h){ R(T_UPDATEWIN,{W(h)}); return TRUE; }
inline HRESULT WINAPI FakeCoInit(LPVOID p){ const HRESULT hr=Hr(); R(T_COINIT,{p!=nullptr,uint32(hr)}); return hr; }
inline HRESULT WINAPI FakeCoCreate(const GUID& c,void* o,DWORD ctx,const GUID& i,void** pp){ const HRESULT hr=Hr(); R(T_COCREATE,{uint32(c.Data1),o!=nullptr,uint32(ctx),uint32(i.Data1),Id(pp),uint32(hr)});
    *pp=SUCCEEDED(hr) ? static_cast<void*>(&coms[0]) : nullptr; return hr; }
inline uint32 iidCtl{}, iidEvt{}, iidSeek{}, iidWin{};
inline HRESULT __stdcall FakeQI(void* self,const GUID& iid,void** out){ const HRESULT hr=Hr(); R(T_QI,{Id(self),uint32(iid.Data1),Id(out),uint32(hr)}); void* r=nullptr;
    if(SUCCEEDED(hr)){ const auto d=uint32(iid.Data1); if(d==iidCtl)r=&coms[1]; else if(d==iidEvt)r=&coms[2]; else if(d==iidSeek)r=&coms[4]; else if(d==iidWin)r=&coms[3]; } *out=r; return hr; }
inline ULONG __stdcall FakeComRelease(void* self){ R(T_RELEASE,{Id(self)}); return 0; }
inline HRESULT __stdcall FakeRenderFile(void* self,const wchar_t* f,const wchar_t* pl){ size_t n=0; while(n<400&&f[n])++n; const HRESULT hr=Hr(); R(T_RENDERFILE,{Id(self),uint32(Fnv(f,n*2)),uint32(n),pl!=nullptr,uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeRun(void* self){ const HRESULT hr=Hr(); R(T_RUN,{Id(self),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeStopC(void* self){ const HRESULT hr=Hr(); R(T_STOPC,{Id(self),uint32(hr)}); return hr; }
inline uint32 stateTarget{}, stateAfter{}, stateCalls{};
inline HRESULT __stdcall FakeGetState(void* self,LONG ms,LONG* st){ const uint32 k=stateCalls++; *st=k>=stateAfter ? LONG(stateTarget) : LONG((stateTarget+1+k)%3);
    const HRESULT hr=BitA() ? S_OK : HRESULT(0x40237); R(T_GETSTATE,{Id(self),uint32(ms),uint32(*st),uint32(hr)}); return hr; }
inline uint32 eventsLeft{};
inline HRESULT __stdcall FakeGetEvent(void* self,LONG* code,LONG_PTR* p1,LONG_PTR* p2,LONG ms){ HRESULT hr=HRESULT(0x80004004);
    if(eventsLeft){ --eventsLeft; *code=BitB() ? 1 : 0x0D; *p1=LONG_PTR(0x1000+eventsLeft); *p2=LONG_PTR(0x2000+eventsLeft*3); hr=BitC() ? S_OK : S_FALSE; }
    R(T_GETEVENT,{Id(self),uint32(ms),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeFreeEvent(void* self,LONG c,LONG_PTR a,LONG_PTR b){ const HRESULT hr=BitA() ? S_OK : HRESULT(0x80070057); R(T_FREEEVENT,{Id(self),uint32(c),uint32(a),uint32(b),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeNotify(void* self,LONG_PTR h,LONG msg,LONG_PTR inst){ const HRESULT hr=Hr(); R(T_NOTIFY,{Id(self),W(reinterpret_cast<void*>(h)),uint32(msg),uint32(inst),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeOwner(void* self,LONG_PTR h){ const HRESULT hr=Hr(); R(T_OWNER,{Id(self),W(reinterpret_cast<void*>(h)),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeStyle(void* self,long s){ const HRESULT hr=Hr(); R(T_STYLE,{Id(self),uint32(s),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeDrain(void* self,LONG_PTR h){ const HRESULT hr=Hr(); R(T_DRAIN,{Id(self),W(reinterpret_cast<void*>(h)),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeWinPos(void* self,long l,long t,long w,long h){ const HRESULT hr=Hr(); R(T_WINPOS,{Id(self),uint32(l),uint32(t),uint32(w),uint32(h),uint32(hr)}); return hr; }
inline HRESULT __stdcall FakeComTrap(void* self){ R(T_COMTRAP,{Id(self)}); return S_OK; }

// operator new/delete del EXE sustituidos solo durante la llamada (sin reservar memoria)
struct NewGuard { ReversingProbe::EntryPatch n1{0x82119A,FakeNew}, n2{0x821195,FakeNew}, d1{0x8214BD,FakeDelete}, d2{0x8213AE,FakeDelete}; };

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Direccion de la funcion de la DLL instalada por un hook virtual (para simularla tambien en llamadas directas)
inline uintptr_t OurVirtual(const char* cls,const char* fn){ auto* h=FindHook(cls,fn); if(!h||h->Symbol()[0]!='V')return 0; return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress()); }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,42> fns{{
    {"CAEBankLoader","CalculateBankSlotsInfosOffsets",0x4DFBA0,0,-1,256},{"CAEBankLoader","Deconstructor",0x4DFB20,0,-1,128},{"CAEBankLoader","GetBankLookup",0x4E01B0,0,-1,512},
    {"CAEBankLoader","LoadBankLookupFile",0x4DFBD0,0,-1,256},{"CAEBankLoader","LoadBankSlotFile",0x4E0590,0,-1,256},{"CAEBankLoader","LoadSFXPakLookupFile",0x4DFC70,0,-1,256},
    {"CAEGlobalWeaponAudioEntity","AddAudioEvent",0x4DFAA0,0,-1,256},{"CAEGlobalWeaponAudioEntity","Constructor",0x5075B0,0,-1,64},{"CAEGlobalWeaponAudioEntity","Destructor",0x506C50,0,-1,64},
    {"CAEGlobalWeaponAudioEntity","ProjectileFire",0x4DF060,0,-1,512},{"CAEGlobalWeaponAudioEntity","ServiceAmbientGunFire",0x4DF210,0,-1,1024},{"CAEGlobalWeaponAudioEntity","UpdateParameters",0,0x862E5C,0,256},
    {"CAnimBlendClumpData","Constructor",0x4CF0E0,0,-1,64},{"CAnimBlendClumpData","Destructor",0x4CF100,0,-1,128},{"CAnimBlendClumpData","ForAllFrames",0x4CF190,0,-1,256},
    {"CAnimBlendClumpData","ForAllFramesInSPR",0x4CF1E0,0,-1,64},{"CAnimBlendClumpData","LoadFramesIntoSPR",0x4CF1D0,0,-1,64},{"CAnimBlendClumpData","SetNumberOfBones",0x4CF140,0,-1,256},
    {"CAnimBlendStaticAssociation","AllocateSequenceArray",0x4CE960,0,-1,256},{"CAnimBlendStaticAssociation","Constructor1",0x4CE940,0,-1,64},{"CAnimBlendStaticAssociation","Constructor2",0x4CEF60,0,-1,256},
    {"CAnimBlendStaticAssociation","Destructor",0x4CDF50,0,-1,256},{"CAnimBlendStaticAssociation","FreeSequenceArray",0x4CE9A0,0,-1,128},{"CAnimBlendStaticAssociation","Init",0x4CEC20,0,-1,512},
    {"CCutsceneObject","PreRender",0,0x868A60,17,512},{"CCutsceneObject","ProcessControl",0,0x868A60,10,512},{"CCutsceneObject","RemoveLighting",0,0x868A60,20,128},
    {"CCutsceneObject","SetModelIndex",0,0x868A60,5,256},{"CCutsceneObject","SetupCarPipeAtomicsForClump",0x5B1AB0,0,-1,256},{"CCutsceneObject","SetupLighting",0,0x868A60,19,128},
    {"CDoor","GetAngleOpenRatio",0x6F47E0,0,-1,512},{"CDoor","IsClosed",0x6F4800,0,-1,512},{"CDoor","IsFullyOpen",0x6F4820,0,-1,512},
    {"CDoor","Open",0x6F4790,0,-1,512},{"CDoor","Process",0x6F4040,0,-1,1024},{"CDoor","ProcessImpact",0x6F4540,0,-1,1024},
    {"VideoPlayer","OnGraphNotify",0x745FB0,0,-1,256},{"VideoPlayer","Play",0x747660,0,-1,512},{"VideoPlayer","Release",0x746740,0,-1,128},
    {"VideoPlayer","Shutdown",0x7467B0,0,-1,128},{"VideoPlayer","UpdateWindow",0x7466D0,0,-1,256},{"VideoPlayer","WaitState",0x745F90,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"animaudio6-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"animaudio6-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("animaudio6");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"animaudio6-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,42> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    using std::bit_cast;
    const uintptr_t dllEntSetModel=OurVirtual("CEntity","SetModelIndex"), dllPhysPC=OurVirtual("CPhysical","ProcessControl");
    if(!dllEntSetModel||!dllPhysPC){Write({{"event","error"},{"reason","virtual dependency not found"}});return;}
    const auto dllVtGW=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CAEGlobalWeaponAudioEntity"));
    const auto dllVtSA=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CAnimBlendStaticAssociation"));
    const uint32 clumpOffset=*reinterpret_cast<uint32*>(0xB5F878);
    if(clumpOffset+4>clumps[0].size()){Write({{"event","error"},{"reason","clump plugin offset"},{"offset",clumpOffset}});return;}
    std::array<bool,42> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(!nregs){ Reg(arena.data(),arena.size()); for(auto& b:ent)Reg(b.data(),b.size()); for(auto& b:mats)Reg(b.data(),b.size()); for(auto& b:frames)Reg(b.data(),b.size());
        for(auto& b:clumps)Reg(b.data(),b.size()); Reg(cdata.data(),cdata.size()); Reg(fdatas.data(),fdatas.size()); Reg(seqs.data(),seqs.size()); Reg(hierBuf.data(),hierBuf.size());
        Reg(matArr.data(),matArr.size()); for(auto& b:atomics)Reg(b.data(),b.size()); Reg(geom.data(),geom.size()); Reg(morph.data(),morph.size()); Reg(interp.data(),interp.size());
        for(auto& b:sounds)Reg(b.data(),b.size()); for(auto& b:links)Reg(b.data(),b.size()); Reg(loader.data(),loader.size()); Reg(coms.data(),sizeof(coms));
        Reg(reinterpret_cast<void*>(0xC920D4),20); Reg(reinterpret_cast<void*>(0xB612F0),16); Reg(rbuf.data(),rbuf.size()); }
    // objetos COM falsos: QueryInterface, Release y los metodos que usa VideoPlayer (indices de la vtable del SDK)
    for(size_t k=0;k<coms.size();++k){ for(auto& v:comVt[k])v=reinterpret_cast<void*>(&FakeComTrap); comVt[k][0]=reinterpret_cast<void*>(&FakeQI); comVt[k][2]=reinterpret_cast<void*>(&FakeComRelease); coms[k]={comVt[k].data(),uint32(k)}; }
    comVt[0][13]=reinterpret_cast<void*>(&FakeRenderFile);
    comVt[1][7]=reinterpret_cast<void*>(&FakeRun); comVt[1][9]=reinterpret_cast<void*>(&FakeStopC); comVt[1][10]=reinterpret_cast<void*>(&FakeGetState);
    comVt[2][8]=reinterpret_cast<void*>(&FakeGetEvent); comVt[2][12]=reinterpret_cast<void*>(&FakeFreeEvent); comVt[2][13]=reinterpret_cast<void*>(&FakeNotify);
    comVt[3][9]=reinterpret_cast<void*>(&FakeStyle); comVt[3][29]=reinterpret_cast<void*>(&FakeOwner); comVt[3][31]=reinterpret_cast<void*>(&FakeDrain); comVt[3][39]=reinterpret_cast<void*>(&FakeWinPos);
    iidCtl=*reinterpret_cast<uint32*>(0x874708); iidEvt=*reinterpret_cast<uint32*>(0x8746E8); iidSeek=*reinterpret_cast<uint32*>(0x8870DC); iidWin=*reinterpret_cast<uint32*>(0x8746B8);
    for(auto& v:physVt)v=reinterpret_cast<void*>(&FakeComTrap); physVt[0]=reinterpret_cast<void*>(&FakePhysDtor);
    gameWnd=PSGLOBAL(window);

    // modelos para CCutsceneObject: los seis vehiculos de cinematica y modelos validos cualesquiera
    std::vector<int32> modelIds;
    for(int k=0;k<6;++k){ int32 idx=-1; const char* n=reinterpret_cast<const char**>(0x8D0F68)[k]; if(CModelInfo::GetModelInfo(n,&idx)&&idx>=0)modelIds.push_back(idx); }
    for(int32 k=1;k<20000&&modelIds.size()<40;k+=487)if(CModelInfo::GetModelInfo(k))modelIds.push_back(k);
    if(modelIds.empty()){Write({{"event","error"},{"reason","no models"}});return;}

    // estado global guardado
    auto Snap=[](uintptr_t a,size_t n){ auto* p=reinterpret_cast<uint8*>(a); return std::vector<uint8>(p,p+n); };
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {0xB7CB84,4},{0xB7CB4C,4},{0xB7CB5C,4},{0xB6F065,1},{0xB6F03C,4},{0xB612F0,16},{0xB61318,0x54},{0xC81314,2},{0xC812FC,4},
            {0xC8D4C0,4},{0xC920D4,20},{0xBC4040,0x1C}})
        saved.push_back({a,Snap(a,n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };
    auto& mlist=*reinterpret_cast<uint8*>(0xB74288); uint8* const mobjs=*reinterpret_cast<uint8**>(&mlist+0x1F8);
    const size_t mcount=mobjs ? *reinterpret_cast<uint32*>(mobjs-4) : 0;
    const std::vector<uint8> savedList(&mlist,&mlist+0x1FC); const std::vector<uint8> savedLinks(mobjs,mobjs+mcount*0x54);
    auto RestoreList=[&]{ std::memcpy(&mlist,savedList.data(),0x1FC); if(mcount)std::memcpy(mobjs,savedLinks.data(),mcount*0x54); };
    const bool listFull=*reinterpret_cast<uint8**>(&mlist+0x150+0x50)==&mlist+0x1A4;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch f1(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpen), f1e(0x538900,FakeOpen), f2(bit_cast<uintptr_t>(&CFileMgr::GetTotalSize),FakeSize), f2e(0x5389E0,FakeSize);
        EntryPatch f3(bit_cast<uintptr_t>(&CFileMgr::Read),FakeRead), f3e(0x538950,FakeRead), f4(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeClose), f4e(0x5389D0,FakeClose);
        EntryPatch cd(bit_cast<uintptr_t>(&CdStreamOpen),FakeCdOpen), cde(0x4067B0,FakeCdOpen);
        EntryPatch m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)),FakeMalloc1), m2(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc2), m2e(0x72F420,FakeMalloc2);
        EntryPatch m3(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), m3e(0x72F430,FakeFree), m4e(0x72F4C0,FakeMallocAlign), m5e(0x72F4F0,FakeFreeAlign);
        EntryPatch m5(bit_cast<uintptr_t>(&CMemoryMgr::FreeAlign),FakeFreeAlign); // entrada de la DLL (naked desde d26e3a5d): ~CAnimBlendClumpData/SetNumberOfBones la llaman directo, no por 0x72F4F0
        EntryPatch a1(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),FakeIsBankLoaded), a1e(0x4D88C0,FakeIsBankLoaded);
        EntryPatch a2(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),FakeLoadBank), a2e(0x4D88A0,FakeLoadBank);
        EntryPatch a3(bit_cast<uintptr_t>(&CAudioEngine::IsLoadingTuneActive),FakeLoadingTune), a3e(0x506D90,FakeLoadingTune);
        EntryPatch a4(bit_cast<uintptr_t>(&CAESound::Initialise),FakeInitialise), a4e(0x4EFE50,FakeInitialise);
        EntryPatch a5(bit_cast<uintptr_t>(&CAESound::RegisterWithPhysicalEntity),FakeRegister), a5e(0x4EF820,FakeRegister);
        EntryPatch a6(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),FakeRequest), a6e(0x4EFB10,FakeRequest);
        EntryPatch a7(bit_cast<uintptr_t>(&CAESound::StopSound),FakeStopSound), a7e(0x4EF1C0,FakeStopSound);
        EntryPatch a8(bit_cast<uintptr_t>(&CAudioEngine::ReportWeaponEvent),FakeReport), a8e(0x506F40,FakeReport);
        EntryPatch a9(bit_cast<uintptr_t>(&CAEWeaponAudioEntity::WeaponFire),FakeWeaponFire), a9e(0x504F80,FakeWeaponFire);
        EntryPatch aa(bit_cast<uintptr_t>(&CAEWeaponAudioEntity::WeaponReload),FakeWeaponReload), aae(0x503690,FakeWeaponReload);
        EntryPatch pc(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakePlayerCoors), pce(0x56E010,FakePlayerCoors);
        EntryPatch cu(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUp), cue(0x571A00,FakeCleanUp);
        EntryPatch b1(bit_cast<uintptr_t>(&RpAnimBlendClumpFindBone),FakeFindBone), b1e(0x4D6400,FakeFindBone);
        EntryPatch b2(bit_cast<uintptr_t>(&RpAnimBlendClumpFindFrameFromHashKey),FakeFindHash), b2e(0x4D6370,FakeFindHash);
        EntryPatch b3(bit_cast<uintptr_t>(&RpAnimBlendClumpInit),FakeClumpInit), b3e(0x4D6720,FakeClumpInit);
        EntryPatch e1(dllEntSetModel,FakeEntSetModel), e1e(0x532AE0,FakeEntSetModel), e2(dllPhysPC,FakePhysPC), e2e(0x5485E0,FakePhysPC);
        EntryPatch e3(bit_cast<uintptr_t>(&CEntity::UpdateRpHAnim),FakeRpHAnim), e3e(0x532B20,FakeRpHAnim);
        EntryPatch w1(bit_cast<uintptr_t>(&GetAnimHierarchyFromClump),FakeGetHier), w1e(0x734B10,FakeGetHier), w2e(0x7C5120,FakeMatArray), w3e(0x7F0990,FakeLTM);
        EntryPatch w4(bit_cast<uintptr_t>(&GetFirstAtomic),FakeFirstAtomic), w4e(0x734820,FakeFirstAtomic), w5e(0x7C7550,FakeGetSkin), w6e(0x749B70,FakeForAllAtomics);
        EntryPatch s1(bit_cast<uintptr_t>(&CRealTimeShadowManager::DoShadowThisFrame),FakeDoShadow), s1e(0x706BA0,FakeDoShadow);
        EntryPatch s2(bit_cast<uintptr_t>(&CShadows::StoreShadowForPedObject),FakeStoreShadow), s2e(0x707B40,FakeStoreShadow);
        EntryPatch s3(bit_cast<uintptr_t>(&CPed::ShoulderBoneRotation),FakeShoulder), s3e(0x5DF560,FakeShoulder);
        EntryPatch s4(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVertical), s4e(0x5674E0,FakeVertical);
        EntryPatch l1(bit_cast<uintptr_t>(&ActivateDirectional),FakeActDir), l1e(0x735C80,FakeActDir), l2(bit_cast<uintptr_t>(&DeActivateDirectional),FakeDeActDir), l2e(0x735C70,FakeDeActDir);
        EntryPatch l3(bit_cast<uintptr_t>(static_cast<void(*)()>(&SetAmbientColours)),FakeAmbient), l3e(0x735D30,FakeAmbient);
        EntryPatch l4(bit_cast<uintptr_t>(&CPointLights::RemoveLightsAffectingObject),FakeRemoveLights), l4e(0x6FFFE0,FakeRemoveLights);
        EntryPatch l5(bit_cast<uintptr_t>(&CRenderer::SetupLightingForEntity),FakeSetupLight), l5e(0x553E40,FakeSetupLight);
        EntryPatch gs(bit_cast<uintptr_t>(&CPhysical::GetSpeed),FakeGetSpeed), gse(0x542CE0,FakeGetSpeed);
        // APIs de Win32/COM: destino del IAT del EXE y direccion que usa la DLL (si es distinta); solo durante VideoPlayer
        struct Api { uintptr_t exeIat, dll; void* fake; };
        auto Proc=[](const char* mod,const char* name){ HMODULE h=GetModuleHandleA(mod); if(!h)h=LoadLibraryA(mod); return h ? reinterpret_cast<uintptr_t>(GetProcAddress(h,name)) : uintptr_t(0); };
        const std::array<Api,5> apis{{ {0x858304,Proc("user32.dll","GetClientRect"),reinterpret_cast<void*>(&FakeClientRect)},
            {0x8582D8,Proc("user32.dll","SetFocus"),reinterpret_cast<void*>(&FakeSetFocus)}, {0x8582AC,Proc("user32.dll","UpdateWindow"),reinterpret_cast<void*>(&FakeUpdateWindowApi)},
            {0x858350,Proc("ole32.dll","CoInitialize"),reinterpret_cast<void*>(&FakeCoInit)}, {0x85834C,Proc("ole32.dll","CoCreateInstance"),reinterpret_cast<void*>(&FakeCoCreate)} }};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if(full=="CAEGlobalWeaponAudioEntity::Constructor"&&(listFull||!mobjs)){Write({{"event","error"},{"reason","matrix list full"}});passed=false;continue;}
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                // CMatrix falsa con rotacion en Z, posicion dada y sin matriz adjunta
                auto mkMat=[&](void* m,const CVector& pos){ const float h=uni(-3.2f,3.2f), s=std::sin(h), c=std::cos(h); auto* q=static_cast<float*>(m);
                    std::fill(q,q+0x15,0.f); q[0]=c;q[1]=s;q[2]=uni(-0.05f,0.05f); q[4]=-s;q[5]=c;q[6]=uni(-0.05f,0.05f); q[8]=uni(-0.05f,0.05f);q[9]=uni(-0.05f,0.05f);q[10]=1.f;
                    q[12]=pos.x;q[13]=pos.y;q[14]=pos.z; At<void*>(m,0x40)=nullptr; At<uint8>(m,0x44)=0; };
                auto mkRwMat=[&](void* m){ auto* q=static_cast<float*>(m); for(int k=0;k<16;++k)q[k]=uni(-1.5f,1.5f); q[12]=uni(-500.f,500.f);q[13]=uni(-500.f,500.f);q[14]=uni(-50.f,50.f);
                    At<uint32>(m,0xC)=next()%4; At<uint32>(m,0x1C)=0; At<uint32>(m,0x2C)=0; At<uint32>(m,0x3C)=0; };
                auto masked=[&](void* m){ auto v=ReversingProbe::Bytes(m,0x48); for(size_t pad:{0xC,0x1C,0x2C,0x3C})std::fill_n(v.begin()+pad,4,uint8(0)); return v; };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA6A6u; if(!stt)stt=1; rn=0; seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    maskA=next(); callsA=0; maskB=next(); callsB=0; maskC=next(); callsC=0; if(next()%6==0)maskA=~0u; if(next()%7==0)maskB=0;
                    arena.fill(0xCD); arenaTop=0x2000; requestCalls=0; faCalls=0; frameCalls=0; cbCalls=0; callNo=0; failAt=0; stateCalls=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); auto& frameCounter=*reinterpret_cast<uint32*>(0xB7CB4C); auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C);
                    auto& camMatrix=*reinterpret_cast<void**>(0xB6F03C);

                    if(cls=="CAEBankLoader") {
                        void* o=loader.data(); fill(o,loader.size()); readState=next(); readCalls=0; readFailMask=next()%3==0 ? next() : 0; pakMode=false; cdResult=0x1000+next()%16;
                        static const uint32 opens[]{0,1,0x1234,0x7FFF0000u,0x80000010u,0xFFFFFFF0u}; openResult=next()%4 ? 0x1234+next()%64 : opens[next()%6];
                        if(nm=="CalculateBankSlotsInfosOffsets"){ const int16 cnt=next()%8==0 ? int16(-int(next()%5)) : int16(next()%7); At<int16>(o,0xC)=cnt;
                            void* slots=Alloc(7*0x12D4); fill(slots,7*0x12D4); At<void*>(o,0)=slots; reinterpret_cast<void(__thiscall*)(void*)>(a)(o); out["slots"]=Fnv(slots,7*0x12D4); }
                        else if(nm=="Deconstructor"){ At<uint8>(o,0x14)=uint8(next()%3 ? 1+next()%255 : 0);
                            for(size_t off:{size_t(0),size_t(4),size_t(0x1C),size_t(0x20)})At<void*>(o,off)=next()%4 ? Alloc(16+next()%64) : nullptr;
                            { NewGuard np; reinterpret_cast<void(__thiscall*)(void*)>(a)(o); } }
                        else if(nm=="GetBankLookup"){ At<uint8>(o,0x14)=uint8(next()%5 ? 1 : 0); At<int16>(o,0xE)=next()%6==0 ? int16(-int(next()%100)) : int16(next()%600);
                            At<void*>(o,4)=Alloc(16); const uint16 id=next()%5==0 ? uint16(next()) : uint16(next()%700);
                            ret=Id(reinterpret_cast<void*(__thiscall*)(void*,uint16)>(a)(o,id)); }
                        else { if(nm=="LoadBankLookupFile")totalSize=next()%6==0 ? next()%3 : next()%10==0 ? 0x60000+next()%0x10000 : (next()%300)*12+(next()%4 ? 0 : next()%12);
                            else if(nm=="LoadBankSlotFile"){ slotCount=next()%8==0 ? int16(-int(1+next()%5)) : int16(next()%6); const uint32 k=uint32(std::max<int>(slotCount,0));
                                totalSize=next()%6==0 ? next()%4 : 2+(next()%4 ? k : next()%(k+1))*0x12D4+(next()%4 ? 0 : next()%64); }
                            else { pakMode=true; totalSize=next()%6==0 ? next()%3 : (1+next()%40)*52+(next()%4 ? 0 : next()%52); }
                            bool r; { NewGuard np; r=reinterpret_cast<bool(__thiscall*)(void*)>(a)(o); } ret=r; out["arena"]=Fnv(arena.data(),arena.size()); }
                        out["obj"]=ReversingProbe::Bytes(o,0x6E4);
                    }
                    else if(cls=="CAEGlobalWeaponAudioEntity") {
                        void* o=ent[0].data(); void* ph=ent[1].data(); fill(o,0xA8); At<uintptr_t>(o,0)=0x862E5C; At<uint32>(o,0x10)=0; At<void*>(o,0xA0)=ph;
                        fill(ph,0x140); At<uintptr_t>(ph,0)=uintptr_t(physVt.data()); At<void*>(ph,0x18)=nullptr;
                        if(next()%2){ mkMat(mats[1].data(),CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-10.f,60.f)}); At<void*>(ph,0x14)=mats[1].data(); }
                        else { At<void*>(ph,0x14)=nullptr; At<CVector>(ph,4)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-10.f,60.f)}; }
                        for(auto& s:sounds){ fill(s.data(),s.size()); At<float>(s.data(),0x14)=uni(-20.f,20.f); }
                        auto P=[&]()->void*{ switch(next()%4){case 0:return nullptr;default:return sounds[next()%6].data();} };
                        for(auto& p:requestSeq)p=P();
                        auto& pw=*reinterpret_cast<std::array<void*,4>*>(0xB612F0);
                        if(nm=="Constructor"){ std::fill_n(static_cast<uint8*>(o),0xA8,uint8(0xCD));
                            { NewGuard np; reinterpret_cast<void*(__thiscall*)(void*)>(a)(o); }
                            const auto vt=At<uintptr_t>(o,0); out["vt"]=vt==0x862E5C||vt==dllVtGW; std::vector<uint8> w; // solo los bytes que escribe el EXE
                            for(size_t k=0x10;k<0x14;++k)w.push_back(At<uint8>(o,k)); for(size_t k=0x7C;k<0x7F;++k)w.push_back(At<uint8>(o,k)); for(size_t k=0x84;k<0xA0;++k)w.push_back(At<uint8>(o,k));
                            w.push_back(At<uint8>(o,0xA4)); out["obj"]=w; void* p=At<void*>(o,0xA0); out["physical"]=Id(p);
                            if(InArena(p))out["physical_bytes"]=Fnv(static_cast<uint8*>(p)+4,0x134); out["links"]=Fnv(mobjs,mcount*0x54); out["list"]=Fnv(&mlist,0x1FC);
                            RestoreList(); }
                        else if(nm=="Destructor"){ if(next()%5==0)At<void*>(o,0xA0)=nullptr; reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                            auto v=ReversingProbe::Bytes(o,0xA8); std::fill_n(v.begin(),4,uint8(0)); out["obj"]=v; }
                        else if(nm=="UpdateParameters"){ for(auto& p:pw)p=P(); void* s=next()%5 ? pw[next()%4] : sounds[next()%6].data(); if(next()%6==0)s=nullptr;
                            const int16 pos=next()%4 ? int16(-1) : int16(next()%3000-100);
                            reinterpret_cast<void(__thiscall*)(void*,void*,int16)>(a)(o,s,pos); out["sounds"]=json::array({Id(pw[0]),Id(pw[1]),Id(pw[2]),Id(pw[3])}); }
                        else if(nm=="AddAudioEvent"||nm=="ProjectileFire"){ At<int8>(o,0xA4)=int8(int(next()%129)-1);
                            static const int32 weapons[]{19,20,19,20,22,30,0,46,35};
                            const int32 w=next()%5==0 ? int32(next()%60) : weapons[next()%9]; const int32 ev=next()%3 ? 0x94 : int32(next()%400);
                            if(nm=="AddAudioEvent"){ const int32 id=next()%4 ? int32(0x8F+next()%9) : int32(next()%400); void* e=next()%6 ? ph : nullptr;
                                reinterpret_cast<void(__thiscall*)(void*,int32,int32,void*)>(a)(o,id,w,e); }
                            else reinterpret_cast<void(__thiscall*)(void*,int32,void*,int32)>(a)(o,w,ph,ev);
                            out["obj"]=ReversingProbe::Bytes(o,0xA8); }
                        else { // ServiceAmbientGunFire
                            auto* st=reinterpret_cast<uint8*>(0xB61318); auto& state=At<int32>(st,0xC); auto& shots=At<int32>(st,0); auto& weapon=At<int32>(st,4); auto& delay=At<uint32>(st,8);
                            auto& lastTime=At<uint32>(st,0x4C); auto& guard=At<uint32>(st,0x50);
                            // vectores estaticos del EXE ya construidos (guardas 2..32 puestas): cascadas 2, 1, 0 (0xB61328) y sirenas 1, 0 (0xB6134C)
                            static const float statics[15]{2091.5f,1888.9f,11.1f, 2085.8f,1905.5f,11.3f, 2075.3f,1925.9f,13.0f, -2683.f,1804.f,69.f, -2683.f,1445.f,37.f};
                            std::memcpy(st+0x10,statics,sizeof(statics));
                            guard=(guard&~0x3Fu)|0x3Eu|(next()%4 ? 1u : 0u);
                            state=next()%10==0 ? int32(6+next()%4) : int32(next()%6); shots=int32(next()%5)-1; static const int32 wt[]{22,30,22,30,25}; weapon=wt[next()%5];
                            timeMs=next()%2 ? 100000+next()%100000 : next(); lastTime=timeMs-next()%8000; delay=next()%4 ? timeMs-lastTime+int32(next()%5)-2 : next()%2000;
                            if(next()%5==0){ lastTime=timeMs-uint32(next()%3)-5000; } if(next()%7==0){ lastTime=timeMs-uint32(next()%3)-6500; }
                            frameCounter=next()%3 ? (next()%1000)*1024 : next();
                            static const int16 regions[]{0,1,2,3,4,1,3,0,5,-1}; At<int16>(reinterpret_cast<void*>(0xC81314),0)=regions[next()%10];
                            At<float>(reinterpret_cast<void*>(0xC812FC),0)=next()%8==0 ? 0.5f : uni(0.f,1.f);
                            CVector cam; switch(next()%4){case 0:cam=CVector{-2683.f+uni(-2500.f,2500.f),1445.f+uni(-2500.f,2500.f),uni(0.f,100.f)};break;
                                case 1:cam=CVector{2085.8f+uni(-140.f,140.f),1905.5f+uni(-140.f,140.f),uni(0.f,40.f)};break;case 2:cam=CVector{-2683.f+uni(-50.f,50.f),1445.f+uni(-50.f,50.f),37.f};break;
                                default:cam=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,80.f)};}
                            mkMat(mats[2].data(),cam); camMatrix=mats[2].data();
                            const CVector pp=At<void*>(ph,0x14) ? At<CVector>(mats[1].data(),0x30) : At<CVector>(ph,4);
                            playerCoors=pp+CVector{uni(-40.f,40.f),uni(-40.f,40.f),uni(-5.f,5.f)}; if(next()%6==0)playerCoors=pp+CVector{24.f,0.f,0.f};
                            for(auto& p:pw)p=next()%3 ? nullptr : sounds[next()%6].data();
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                            out["state"]=ReversingProbe::Bytes(st,0x54); out["sounds"]=json::array({Id(pw[0]),Id(pw[1]),Id(pw[2]),Id(pw[3])});
                            out["physical"]=ReversingProbe::Bytes(ph,0x40); out["mat"]=ReversingProbe::Bytes(mats[1].data()+0x30,12);
                            json vols=json::array(); for(auto& s:sounds)vols.push_back(At<uint32>(s.data(),0x14)); out["volumes"]=vols; }
                    }
                    else if(cls=="CAnimBlendClumpData") {
                        void* o=cdata.data(); fill(o,cdata.size());
                        if(nm=="Constructor"){ std::fill_n(static_cast<uint8*>(o),0x14,uint8(0xCD)); reinterpret_cast<void*(__thiscall*)(void*)>(a)(o); }
                        else if(nm=="Destructor"){ for(auto& l:links)fill(l.data(),l.size()); At<void*>(o,0)=next()%3 ? links[0].data() : nullptr; At<void*>(o,4)=next()%3 ? links[1].data() : nullptr;
                            At<void*>(o,0x10)=next()%4 ? fdatas.data() : nullptr; reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                            out["links"]=json::array({ReversingProbe::Bytes(links[0].data(),8),ReversingProbe::Bytes(links[1].data(),8)}); }
                        else if(nm=="SetNumberOfBones"){ At<void*>(o,0x10)=next()%3 ? fdatas.data() : nullptr; static const uint32 big[]{0,1,0x0AAAAAAB,0x7FFFFFFF,0x80000000u,0xFFFFFFFFu,10923,2731};
                            const uint32 n=next()%4 ? next()%200 : big[next()%8]; reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(o,n); }
                        else if(nm=="ForAllFrames"){ At<void*>(o,0x10)=fdatas.data(); At<uint32>(o,8)=next()%8==0 ? 0x80000000u+next()%5 : next()%30;
                            cbChangeAt=next()%3==0 ? 1+next()%10 : 0; cbNewCount=next()%4==0 ? 0x80000001u : next()%40;
                            void* data=next()%3 ? static_cast<void*>(rbuf.data()) : nullptr; reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(a)(o,reinterpret_cast<void*>(&FrameCb),data); }
                        else if(nm=="LoadFramesIntoSPR") reinterpret_cast<void(__thiscall*)(void*)>(a)(o);
                        else reinterpret_cast<void(__thiscall*)(void*,void*,void*,uint32)>(a)(o,reinterpret_cast<void*>(&FrameCb),rbuf.data(),next());
                        out["obj"]=ReversingProbe::Bytes(o,0x14);
                    }
                    else if(cls=="CAnimBlendStaticAssociation") {
                        uint8* block=static_cast<uint8*>(Alloc(4+8*0x14)); fill(block,4+8*0x14); void* o=block+4;
                        auto Vt=[&](void* p)->json{ const auto v=At<uintptr_t>(p,0); return v==0x85C6CC||v==dllVtSA ? json("CAnimBlendStaticAssociation") : json(v); };
                        auto Body=[&](void* p){ auto v=ReversingProbe::Bytes(p,0x14); std::fill_n(v.begin(),4,uint8(0)); return v; };
                        // clump falso con datos de animacion, jerarquia y secuencias
                        void* clump=clumps[0].data(); fill(clump,clumps[0].size()); At<uint8>(clump,0)=2; At<void*>(clump,clumpOffset)=cdata.data();
                        fill(cdata.data(),cdata.size()); fill(fdatas.data(),fdatas.size()); At<void*>(cdata.data(),0x10)=fdatas.data();
                        const uint32 nf=next()%10==0 ? 0x8000u+next()%8 : next()%41; At<uint32>(cdata.data(),8)=nf | (next()%6==0 ? 0x10000u : 0u);
                        frameCount=nf>=0x8000 ? 0 : std::min<uint32>(nf,40); frameMask=next();
                        fill(seqs.data(),seqs.size()); const uint16 sc=next()%10==0 ? uint16(0x8000+next()%8) : uint16(next()%40);
                        for(uint32 k=0;k<40;++k){ uint8* s=seqs.data()+k*12; At<int16>(s,6)=next()%6==0 ? int16(-int(next()%4)) : int16(next()%5==0 ? 0 : 1+next()%50); }
                        fill(hierBuf.data(),hierBuf.size()); At<void*>(hierBuf.data(),4)=seqs.data(); At<uint16>(hierBuf.data(),8)=sc;
                        void* hier=hierBuf.data();
                        auto seqsOut=[&]{ json r=json::array(); auto** bs=At<void**>(o,0xC); if(InArena(bs))for(uint32 k=0;k<frameCount;++k)r.push_back(Id(bs[k])); return r; };
                        if(nm=="Constructor1"){ std::fill_n(block+4,0x14,uint8(0xCD)); reinterpret_cast<void*(__thiscall*)(void*)>(a)(o); out["vt"]=Vt(o); out["obj"]=Body(o); }
                        else if(nm=="Constructor2"||nm=="Init"){ if(nm=="Constructor2")std::fill_n(block+4,0x14,uint8(0xCD)); void* c=next()%6 ? clump : nullptr;
                            if(nm=="Constructor2"){ reinterpret_cast<void*(__thiscall*)(void*,void*,void*)>(a)(o,c,hier); out["vt"]=Vt(o); }
                            else reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(a)(o,c,hier);
                            out["obj"]=Body(o); out["seq"]=seqsOut(); }
                        else if(nm=="AllocateSequenceArray"){ static const int32 odd[]{0,-1,-5,-100000,int32(0x80000000)}; const int32 n=next()%5 ? int32(next()%65) : odd[next()%5]; /* sin positivos enormes: el EXE escribe sin limite tras un Malloc desbordado o nulo */
                            reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(o,n); out["obj"]=Body(o); json r=json::array(); auto** bs=At<void**>(o,0xC);
                            if(InArena(bs))for(int32 k=0;k<std::min(n,64);++k)r.push_back(Id(bs[k])); out["seq"]=r; }
                        else if(nm=="FreeSequenceArray"){ At<void*>(o,0xC)=next()%4 ? static_cast<void*>(seqs.data()+next()%64*4) : nullptr; reinterpret_cast<void(__thiscall*)(void*)>(a)(o); out["obj"]=Body(o); }
                        else { // Destructor (con borrado; bit 1: array con contador en this-4)
                            const uint8 flags=uint8(next()%4); const uint32 count=1+next()%4; At<uint32>(block,0)=count;
                            for(uint32 k=0;k<8;++k){ void* e=block+4+k*0x14; At<uintptr_t>(e,0)=next()%2 ? 0x85C6CC : next(); At<void*>(e,0xC)=next()%4 ? static_cast<void*>(seqs.data()+k*8) : nullptr; }
                            void* r; { NewGuard np; r=reinterpret_cast<void*(__thiscall*)(void*,uint8)>(a)(o,flags); } ret=Id(r);
                            json es=json::array(); for(uint32 k=0;k<8;++k){ void* e=block+4+k*0x14; es.push_back({Vt(e),Body(e)}); } out["elements"]=es; }
                    }
                    else if(cls=="CCutsceneObject") {
                        void* o=ent[0].data(); void* att=ent[1].data(); fill(o,0x1A0); fill(att,0x40); At<uintptr_t>(o,0)=0x868A60;
                        for(auto& b:clumps){ fill(b.data(),b.size()); b[0]=2; At<void*>(b.data(),clumpOffset)=cdata.data(); }
                        fill(cdata.data(),cdata.size()); fill(fdatas.data(),fdatas.size()); At<void*>(cdata.data(),0x10)=fdatas.data(); At<void*>(fdatas.data(),0x10)=interp.data();
                        fill(interp.data(),interp.size()); for(auto& b:frames){ fill(b.data(),b.size()); mkRwMat(b.data()+0x10); }
                        for(int k=0;k<64;++k)mkRwMat(matArr.data()+k*0x40);
                        for(auto& b:atomics){ fill(b.data(),b.size()); At<void*>(b.data(),0x18)=geom.data(); } fill(geom.data(),geom.size()); At<void*>(geom.data(),0x5C)=morph.data(); fill(morph.data(),morph.size());
                        At<void*>(att,0x18)=clumps[1].data();
                        mkMat(mats[0].data(),CVector{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,60.f)}); At<void*>(o,0x14)=mats[0].data();
                        At<void*>(o,0x18)=next()%5 ? static_cast<void*>(clumps[0].data()) : static_cast<void*>(atomics[1].data()); At<uint8>(atomics[1].data(),0)=1;
                        At<int16>(o,0x22)=next()%4==0 ? int16(1) : int16(next()%600+2);
                        for(size_t k=0x44;k<0x50;k+=4)At<float>(o,k)=uni(-2.f,2.f); At<float>(o,0x12C)=uni(0.f,1.f); At<void*>(o,0x134)=next()%3 ? nullptr : static_cast<void*>(links[0].data());
                        for(size_t k=0x184;k<0x19C;k+=4)At<float>(o,k)=uni(-500.f,500.f);
                        const bool hasAtt=next()%2; At<void*>(o,0x180)=hasAtt ? att : nullptr;
                        if(hasAtt) At<uint32>(o,0x17C)=next()%4 ? 1+next()%63 : 0; else At<void*>(o,0x17C)=next()%3 ? static_cast<void*>(frames[next()%2].data()) : nullptr;
                        if(At<int16>(o,0x22)==1)At<void*>(o,0x18)=clumps[0].data(); // ShoulderBoneRotation exige un clump (GetRpClump)
                        timeStep=next()%5==0 ? uni(0.f,0.0105f) : uni(0.2f,3.f); if(next()%12==0)timeStep=0.01f;
                        firstAtomics={next()%5 ? static_cast<void*>(atomics[0].data()) : nullptr, atomics[next()%2].data()}; skinResult=next()%3 ? static_cast<void*>(links[1].data()) : nullptr;
                        lightB=uint8(next()); rwObjResult=next()%4 ? static_cast<void*>(clumps[0].data()) : static_cast<void*>(atomics[1].data());
                        auto* keys=reinterpret_cast<uint8*>(0xBC4040);
                        if(nm=="SetModelIndex"||nm=="SetupCarPipeAtomicsForClump"){ keys[0x18]=uint8(next()%3 ? 1 : 0); if(next()%3==0)fill(keys,0x18); }
                        if(nm=="SetModelIndex"){ const int32 idx=modelIds[next()%modelIds.size()]; auto* mi=CModelInfo::GetModelInfo(idx); const uint8 alpha=mi->m_nAlpha; mi->m_nAlpha=uint8(next());
                            reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(o,uint32(idx)); out["alpha"]=mi->m_nAlpha; mi->m_nAlpha=alpha; out["keys"]=ReversingProbe::Bytes(keys,0x19);
                            out["cdata"]=ReversingProbe::Bytes(cdata.data(),0x14); out["frame0"]=fdatas[0]; }
                        else if(nm=="SetupCarPipeAtomicsForClump"){ const int32 idx=modelIds[next()%modelIds.size()]; reinterpret_cast<void(__cdecl*)(uint32,void*)>(a)(uint32(idx),next()%4 ? clumps[0].data() : nullptr);
                            out["keys"]=ReversingProbe::Bytes(keys,0x19); }
                        else if(nm=="ProcessControl"){ reinterpret_cast<void(__thiscall*)(void*)>(a)(o); out["mat"]=masked(mats[0].data()); }
                        else if(nm=="PreRender"){ reinterpret_cast<void(__thiscall*)(void*)>(a)(o); out["mat"]=masked(mats[0].data()); out["morph"]=ReversingProbe::Bytes(morph.data(),0x14); }
                        else if(nm=="SetupLighting") ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(o);
                        else reinterpret_cast<void(__thiscall*)(void*,bool)>(a)(o,next()%2);
                        out["obj"]=ReversingProbe::Bytes(o,0x19C);
                    }
                    else if(cls=="CDoor") {
                        void* d=ent[2].data(); void* v=ent[3].data(); fill(d,0x18); fill(v,0x980);
                        At<float>(d,0)=next()%8==0 ? 0.f : uni(-2.f,2.f); At<float>(d,4)=next()%3 ? 0.f : uni(-0.5f,0.5f);
                        const uint16 dirMax=nm=="Process" ? 16 : 6; At<uint16>(d,8)=uint16((next()%10==0 ? next()%dirMax : next()%6)|(next()&0x1F0)|(next()%8==0 ? next()&0xFE00 : 0));
                        At<uint8>(d,0xA)=uint8(next()%6==0 ? next()%8 : (next()%2 ? 0 : 2)); At<uint8>(d,0xB)=uint8(next()%5);
                        At<float>(d,0xC)=next()%6==0 ? At<float>(d,next()%2 ? 0 : 4) : uni(-2.2f,2.2f); At<float>(d,0x10)=uni(-2.f,2.f); At<float>(d,0x14)=next()%6==0 ? 0.f : uni(-0.6f,0.6f);
                        if(nm=="Open"){ const float r=next()%6==0 ? 1.f : next()%6==0 ? 0.f : uni(-0.5f,1.5f); reinterpret_cast<void(__thiscall*)(void*,float)>(a)(d,r); }
                        else if(nm=="GetAngleOpenRatio") ret=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(d));
                        else if(nm=="IsClosed"){ if(next()%3==0)At<float>(d,0xC)=At<float>(d,4); ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(d); }
                        else if(nm=="IsFullyOpen"){ if(next()%4==0)At<float>(d,0xC)=std::fabs(At<float>(d,0))-0.5f; ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(d); }
                        else { mkMat(mats[3].data(),CVector{uni(-1000.f,1000.f),uni(-1000.f,1000.f),uni(0.f,40.f)}); At<void*>(v,0x14)=mats[3].data();
                            static const uint32 types[]{0,0,5,9,0,3}; At<uint32>(v,0x590)=types[next()%6]; At<uint8>(v,0x960)=uint8(next()%3 ? 0 : 1+next()%4);
                            At<int16>(v,0x22)=next()%4==0 ? int16(544) : int16(400+next()%211); At<uint8>(reinterpret_cast<void*>(0xB6F065),0)=uint8(next()%4==0);
                            timeStep=uni(0.2f,3.f);
                            auto rv=[&](float s){ return CVector{uni(-s,s),uni(-s,s),uni(-s,s)}; };
                            const CVector om=rv(0.5f), ot=rv(0.05f), off=rv(2.5f);
                            ret=nm=="Process" ? reinterpret_cast<bool(__thiscall*)(void*,void*,const CVector*,const CVector*,const CVector*)>(a)(d,v,&om,&ot,&off)
                                              : reinterpret_cast<bool(__thiscall*)(void*,void*,const CVector*,const CVector*,const CVector*)>(a)(d,v,&om,&ot,&off); }
                        out["door"]=ReversingProbe::Bytes(d,0x18);
                    }
                    else if(cls=="VideoPlayer") {
                        auto& vp=*reinterpret_cast<std::array<void*,5>*>(0xC920D4); // graph, control, event, window, seeking
                        static const uint32 slotOf[5]{0,1,2,3,4};
                        for(size_t k=0;k<5;++k)vp[k]=next()%4 ? static_cast<void*>(&coms[slotOf[k]]) : nullptr;
                        rectResult={LONG(next()%50),LONG(next()%50),LONG(640+next()%1280),LONG(480+next()%720)};
                        failAt=next()%3 ? 0 : 1+next()%16; stateTarget=next()%3; stateAfter=next()%6; eventsLeft=next()%5;
                        auto& gstate=*reinterpret_cast<int32*>(0xC8D4C0); gstate=int32(next()%10);
                        std::vector<std::unique_ptr<EntryPatch>> ap; for(auto& api:apis){ const auto t=*reinterpret_cast<uintptr_t*>(api.exeIat); ap.push_back(std::make_unique<EntryPatch>(t,api.fake)); if(api.dll&&api.dll!=t)ap.push_back(std::make_unique<EntryPatch>(api.dll,api.fake)); }
                        if(nm=="OnGraphNotify"){ vp[2]=&coms[2]; ret=int32(reinterpret_cast<long(__cdecl*)()>(a)()); }
                        else if(nm=="UpdateWindow"){ vp[3]=&coms[3]; ret=int32(reinterpret_cast<long(__cdecl*)()>(a)()); }
                        else if(nm=="WaitState"){ vp[1]=&coms[1]; reinterpret_cast<void(__cdecl*)(int32)>(a)(int32(stateTarget)); }
                        else if(nm=="Release") reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Shutdown") reinterpret_cast<void(__cdecl*)()>(a)();
                        else { static const char* paths[]{"movies\\Logo.mpg","movies\\GTAtitles.mpg","movies\\GTAtitlesGER.mpg",""}; std::array<char,220> p{};
                            if(next()%3){ const char* s=paths[next()%4]; std::memcpy(p.data(),s,strlen(s)); } else { const uint32 len=next()%200; for(uint32 k=0;k<len;++k)p[k]=char('a'+next()%26); }
                            reinterpret_cast<void(__cdecl*)(int32,const char*)>(a)(int32(next()%11),p.data()); }
                        ap.clear();
                        out["globals"]=json::array({Id(vp[0]),Id(vp[1]),Id(vp[2]),Id(vp[3]),Id(vp[4])}); out["game_state"]=gstate;
                    }
                    out["ret"]=ret; out["recs"]=Recs(); RestoreAll(); return out;
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll(); RestoreList();
    bool restored=std::memcmp(&mlist,savedList.data(),0x1FC)==0; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
