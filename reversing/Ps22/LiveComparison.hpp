#pragma once
// Banco dentro del motor: capa de plataforma Ps (22 funciones de WinPs.cpp). Original (hooks del lote
// desactivados) frente a candidato (activados) con fixtures deterministas.
// Nunca se toca la ventana, el modo de video, el cursor, el dispositivo ni la camara reales:
// - API de Windows/DirectX: se parchean solo durante la llamada las entradas de la IAT del EXE (fijas; ddraw,
//   dsound y d3d9 son importaciones diferidas en 0x8E3EBC/CC/D4) y las de la DLL (buscadas por valor). Los otros
//   hilos pasan a la funcion real. Si falta alguna entrada de la DLL el banco se detiene sin ejecutar nada.
// - Objetos COM falsos (IDirectDraw7, IDirectSound8, IDirect3D9, IDirect3DDevice9, IDirect3DSurface9).
// - RW y juego: EntryPatch en la direccion del EXE y en el simbolo de la DLL (RwCameraBeginUpdate/ShowRaster,
//   RsEventHandler, RwD3D9GetCurrentD3DDevice, RwImageCreate/AllocatePixels, RwD3D9DeviceSupportsDXTTexture,
//   CMemoryMgr::Malloc, RwEngine* de subsistemas y modos, RwD3D9EngineSet*, GetVideoModeList, CText::Get/Load/
//   Unload, CFileMgr::Initialise, InitUserDirectories, C_PcSave::SetSaveDirectory, CMenuManager::LoadSettings,
//   GetDXVersion 0x745650). rwfree se sustituye en RwEngineInstance (+0x138).
// - RsGlobal.ps apunta a un bloque Ps falso (salvo psInitialize, que escribe el real y se restaura).
// - NOTSA mantenidos fuera de la comparacion: SetProcessDPIAware (psInitialize) y SDLWrapper::Initialize +
//   SDL_SetWindowSize (psSelectDevice, NOTSA_USE_SDL3) se simulan sin traza; SkipDeviceSelection se fuerza a falso.
// Se guardan y restauran en cada ejecucion y al final: RsGlobal, PsGlobal, s_OSStatus, el fabricante de CPU,
// gGameState, CMenuManager (0x100 bytes), CLoadingScreen::m_bActive, Scene.m_pRwCamera, GsubSysInfo y los globals
// de seleccion de video (incluido el rdtsc de 0xC92104/08, que solo se compara como "escrito").
#include <fstream>
#include <optional>
#include <intrin.h>
#include <ddraw.h>
#include <dsound.h>
#include <d3d9.h>
#include <mmsystem.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <platform/platform.h>
#include <platform/win/WinPlatform.h>
#include <platform/win/VideoMode.h>
#include "FileMgr.h"
#include "MemoryMgr.h"
#include "C_PcSave.h"
#include "MenuManager.h"
#include "LoadingScreen.h"
#include "extensions/Configs/Miscellaneous.hpp"
#ifdef NOTSA_USE_SDL3
#include "SDLWrapper.hpp"
#include <SDL3/SDL.h>
#endif
#include "../ProbeRuntime.hpp"

namespace Ps22Live {
inline bool done{}; inline std::ofstream log; inline json trace; inline DWORD mainTid{};
inline bool Mine() { return GetCurrentThreadId()==mainTid; }
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }
inline json Str(const char* s) { return s ? json(std::string(s)) : json(nullptr); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// ---------------------------------------------------------------------------------------------------
// IAT
// ---------------------------------------------------------------------------------------------------
inline void** FindIat(HMODULE mod,const void* target) {
    if(!mod||!target)return nullptr;
    auto* base=reinterpret_cast<uint8*>(mod); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
    const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)return nullptr;
    for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
        for(auto* t=reinterpret_cast<void**>(base+d->FirstThunk);*t;++t)if(*t==target)return t;
    return nullptr;
}
struct IatPatch {
    void** slot{}; void* saved{};
    IatPatch(void** s,void* r) : slot(s) { if(slot){saved=*slot;Set(r);} }
    void Set(void* v) const { DWORD o{},ign{}; VirtualProtect(slot,4,PAGE_READWRITE,&o); *slot=v; VirtualProtect(slot,4,o,&ign); }
    ~IatPatch() { if(slot)Set(saved); }
    IatPatch(const IatPatch&)=delete; IatPatch& operator=(const IatPatch&)=delete;
};

enum Api { A_SetWindowTextA, A_MessageBoxA, A_ShowCursor, A_ClientToScreen, A_SetCursorPos, A_GetWindowRect, A_SetWindowPos,
           A_OutputDebugStringA, A_timeGetDevCaps, A_timeBeginPeriod, A_timeGetTime, A_timeEndPeriod, A_GetSystemDefaultLCID,
           A_GetUserDefaultLCID, A_GetKeyboardLayout, A_GetVersionExA, A_GlobalMemoryStatus, A_CoInitialize, A_DirectDrawCreateEx,
           A_DirectSoundCreate8, A_Direct3DCreate9, A_DialogBoxParamA, A_COUNT };
inline std::array<void*,A_COUNT> realFn{}; inline std::array<void**,A_COUNT> dllSlot{};
template<class T> T Real(int id) { return reinterpret_cast<T>(realFn[id]); }

// --- fixtures (rellenados antes de cada llamada, iguales en ambas ejecuciones) ---
inline int32 retSetText{}, retMsgBox{}, retShowCursor{}, retGetRect{}, retSetPos{}, offX{}, offY{};
inline RECT fixRect{};
inline UINT capsMin{}, capsMax{}; inline MMRESULT capsRes{}; inline DWORD timeValue{};
inline std::array<DWORD,2> sysLcid{}, usrLcid{}, kbLayout{}; inline int nSys{}, nUsr{}, nKb{};
inline DWORD verPlatform{}, verMajor{}, verMinor{};
inline std::array<SIZE_T,4> memFix{};
inline HRESULT coHr{}, ddHr{}, vidHr{}, dsHr{}, capsHr{}; inline DWORD vidTotal{}, vidFree{};
inline INT_PTR dialogRet{}; inline int32 dialogVM{}, dialogSS{};
inline uint32 d3dModeCount{}; inline std::array<D3DDISPLAYMODE,16> d3dModes{};
inline UINT dmW{}, dmH{}; inline int32 lockPitch{}; inline uint32 dpiCalls{}, sdlCalls{};

inline BOOL WINAPI XSetWindowTextA(HWND h,LPCSTR s){ if(!Mine())return Real<decltype(&XSetWindowTextA)>(A_SetWindowTextA)(h,s); trace.push_back({"set_text",U(h),Str(s)}); return retSetText; }
inline int WINAPI XMessageBoxA(HWND h,LPCSTR t,LPCSTR c,UINT ty){ if(!Mine())return Real<decltype(&XMessageBoxA)>(A_MessageBoxA)(h,t,c,ty); trace.push_back({"msgbox",U(h),Str(t),Str(c),ty}); return retMsgBox; }
inline int WINAPI XShowCursor(BOOL b){ if(!Mine())return Real<decltype(&XShowCursor)>(A_ShowCursor)(b); trace.push_back({"show_cursor",b}); return retShowCursor; }
inline BOOL WINAPI XClientToScreen(HWND h,LPPOINT p){ if(!Mine())return Real<decltype(&XClientToScreen)>(A_ClientToScreen)(h,p); trace.push_back({"client_to_screen",U(h),p->x,p->y}); p->x+=offX; p->y+=offY; return TRUE; }
inline BOOL WINAPI XSetCursorPos(int x,int y){ if(!Mine())return Real<decltype(&XSetCursorPos)>(A_SetCursorPos)(x,y); trace.push_back({"set_cursor_pos",x,y}); return TRUE; }
inline BOOL WINAPI XGetWindowRect(HWND h,LPRECT r){ if(!Mine())return Real<decltype(&XGetWindowRect)>(A_GetWindowRect)(h,r); trace.push_back({"get_window_rect",U(h)}); *r=fixRect; return retGetRect; }
inline BOOL WINAPI XSetWindowPos(HWND h,HWND after,int x,int y,int cx,int cy,UINT fl){ if(!Mine())return Real<decltype(&XSetWindowPos)>(A_SetWindowPos)(h,after,x,y,cx,cy,fl); trace.push_back({"set_window_pos",U(h),int32(intptr_t(after)),x,y,cx,cy,fl}); return retSetPos; }
inline void WINAPI XOutputDebugStringA(LPCSTR s){ if(!Mine())return Real<decltype(&XOutputDebugStringA)>(A_OutputDebugStringA)(s); trace.push_back({"ods",Str(s)}); }
inline MMRESULT WINAPI XtimeGetDevCaps(LPTIMECAPS tc,UINT sz){ if(!Mine())return Real<decltype(&XtimeGetDevCaps)>(A_timeGetDevCaps)(tc,sz); trace.push_back({"time_devcaps",sz}); tc->wPeriodMin=capsMin; tc->wPeriodMax=capsMax; return capsRes; }
inline MMRESULT WINAPI XtimeBeginPeriod(UINT p){ if(!Mine())return Real<decltype(&XtimeBeginPeriod)>(A_timeBeginPeriod)(p); trace.push_back({"time_begin",p}); return 0; }
inline DWORD WINAPI XtimeGetTime(){ if(!Mine())return Real<decltype(&XtimeGetTime)>(A_timeGetTime)(); trace.push_back({"time_get"}); return timeValue; }
inline MMRESULT WINAPI XtimeEndPeriod(UINT p){ if(!Mine())return Real<decltype(&XtimeEndPeriod)>(A_timeEndPeriod)(p); trace.push_back({"time_end",p}); return 0; }
inline LCID WINAPI XGetSystemDefaultLCID(){ if(!Mine())return Real<decltype(&XGetSystemDefaultLCID)>(A_GetSystemDefaultLCID)(); trace.push_back({"sys_lcid"}); return sysLcid[nSys++%2]; }
inline LCID WINAPI XGetUserDefaultLCID(){ if(!Mine())return Real<decltype(&XGetUserDefaultLCID)>(A_GetUserDefaultLCID)(); trace.push_back({"usr_lcid"}); return usrLcid[nUsr++%2]; }
inline HKL WINAPI XGetKeyboardLayout(DWORD t){ if(!Mine())return Real<decltype(&XGetKeyboardLayout)>(A_GetKeyboardLayout)(t); trace.push_back({"kb_layout",t}); return reinterpret_cast<HKL>(uintptr_t(kbLayout[nKb++%2])); }
inline BOOL WINAPI XGetVersionExA(LPOSVERSIONINFOA v){ if(!Mine())return Real<decltype(&XGetVersionExA)>(A_GetVersionExA)(v); trace.push_back({"version",v->dwOSVersionInfoSize});
    v->dwMajorVersion=verMajor; v->dwMinorVersion=verMinor; v->dwBuildNumber=1234; v->dwPlatformId=verPlatform; v->szCSDVersion[0]=0; return TRUE; }
inline void WINAPI XGlobalMemoryStatus(LPMEMORYSTATUS m){ if(!Mine())return Real<decltype(&XGlobalMemoryStatus)>(A_GlobalMemoryStatus)(m); trace.push_back({"memstatus"});
    m->dwLength=sizeof(MEMORYSTATUS); m->dwMemoryLoad=50; m->dwTotalPhys=memFix[0]; m->dwAvailPhys=memFix[1]; m->dwTotalPageFile=7; m->dwAvailPageFile=8; m->dwTotalVirtual=memFix[2]; m->dwAvailVirtual=memFix[3]; }
inline HRESULT WINAPI XCoInitialize(LPVOID p){ if(!Mine())return Real<decltype(&XCoInitialize)>(A_CoInitialize)(p); trace.push_back({"coinit",U(p)}); return coHr; }

// --- objetos COM falsos ---
struct FakeObj { void** vt; };
inline ULONG __stdcall XTrap(void*){ trace.push_back({"trap"}); return 0; }
inline ULONG __stdcall XRelease(FakeObj* o){ trace.push_back({"release",U(o->vt)&0xFFF}); return 0; }
inline std::array<void*,128> ddVt{}, dsVt{}, d3dVt{}, devVt{}, surfVt{};
inline FakeObj fakeDD{ddVt.data()}, fakeDS{dsVt.data()}, fakeD3D{d3dVt.data()}, fakeDev{devVt.data()}, fakeSurf{surfVt.data()};
inline HRESULT __stdcall XGetAvailableVidMem(FakeObj*,LPDDSCAPS2 c,LPDWORD total,LPDWORD avail){
    trace.push_back({"vidmem",c->dwCaps,c->dwCaps2,c->dwCaps3,c->dwCaps4}); *total=vidTotal; *avail=vidFree; return vidHr; }
inline HRESULT __stdcall XGetCaps(FakeObj*,LPDSCAPS c){ trace.push_back({"ds_caps",c->dwSize}); return capsHr; }
inline UINT __stdcall XGetAdapterModeCount(FakeObj*,UINT ad,D3DFORMAT f){ trace.push_back({"mode_count",ad,int(f)}); return d3dModeCount; }
inline HRESULT __stdcall XEnumAdapterModes(FakeObj*,UINT ad,D3DFORMAT f,UINT i,D3DDISPLAYMODE* m){ trace.push_back({"enum_mode",ad,int(f),i}); *m=d3dModes[i%16]; return 0; }
inline HRESULT __stdcall XGetDisplayMode(FakeObj*,UINT sw,D3DDISPLAYMODE* m){ trace.push_back({"display_mode",sw}); m->Width=dmW; m->Height=dmH; m->RefreshRate=60; m->Format=D3DFMT_X8R8G8B8; return 0; }
inline HRESULT __stdcall XCreateOffscreen(FakeObj*,UINT w,UINT h,D3DFORMAT f,D3DPOOL pool,IDirect3DSurface9** pp,HANDLE* sh){
    trace.push_back({"create_offscreen",w,h,int(f),int(pool),U(sh)}); *pp=reinterpret_cast<IDirect3DSurface9*>(&fakeSurf); return 0; }
inline HRESULT __stdcall XGetFrontBufferData(FakeObj*,UINT sw,void* s){ trace.push_back({"front_buffer",sw,s==&fakeSurf}); return 0; }
alignas(16) inline std::array<uint8,8192> srcPixels{};
inline HRESULT __stdcall XLockRect(FakeObj*,D3DLOCKED_RECT* lr,const RECT* r,DWORD fl){
    trace.push_back({"lock",r ? json::array({r->left,r->top,r->right,r->bottom}) : json(nullptr),fl}); lr->Pitch=lockPitch; lr->pBits=srcPixels.data(); return 0; }
inline HRESULT __stdcall XUnlockRect(FakeObj*){ trace.push_back({"unlock"}); return 0; }

inline HRESULT WINAPI XDirectDrawCreateEx(GUID* g,LPVOID* out,REFIID iid,IUnknown* outer){ if(!Mine())return Real<decltype(&XDirectDrawCreateEx)>(A_DirectDrawCreateEx)(g,out,iid,outer);
    trace.push_back({"dd_create",U(g),std::memcmp(&iid,&IID_IDirectDraw7,sizeof(GUID))==0,U(outer)}); if(SUCCEEDED(ddHr))*out=&fakeDD; return ddHr; }
inline HRESULT WINAPI XDirectSoundCreate8(LPCGUID g,LPDIRECTSOUND8* out,LPUNKNOWN outer){ if(!Mine())return Real<decltype(&XDirectSoundCreate8)>(A_DirectSoundCreate8)(g,out,outer);
    trace.push_back({"ds_create",U(g),U(outer)}); if(SUCCEEDED(dsHr))*out=reinterpret_cast<LPDIRECTSOUND8>(&fakeDS); return dsHr; }
inline IDirect3D9* WINAPI XDirect3DCreate9(UINT sdk){ if(!Mine())return Real<decltype(&XDirect3DCreate9)>(A_Direct3DCreate9)(sdk); trace.push_back({"d3d_create",sdk}); return reinterpret_cast<IDirect3D9*>(&fakeD3D); }
inline INT_PTR WINAPI XDialogBoxParamA(HINSTANCE inst,LPCSTR tmpl,HWND wnd,DLGPROC proc,LPARAM lp){ if(!Mine())return Real<decltype(&XDialogBoxParamA)>(A_DialogBoxParamA)(inst,tmpl,wnd,proc,lp);
    trace.push_back({"dialog",U(inst),U(tmpl),U(wnd),proc!=nullptr,int32(lp)}); GcurSelVM=dialogVM; GcurSelSS=dialogSS; return dialogRet; }
inline BOOL WINAPI XSetProcessDPIAware(){ ++dpiCalls; return TRUE; }
#ifdef NOTSA_USE_SDL3
inline bool __cdecl XSdlSetWindowSize(SDL_Window*,int,int){ ++sdlCalls; return true; }
inline bool __cdecl XSdlInitialize(){ ++sdlCalls; return true; }
#endif

struct ApiDesc { Api id; const char* mod; const char* name; uintptr_t exeSlot; void* fake; };
inline const std::array<ApiDesc,A_COUNT> apis{{
    {A_SetWindowTextA,"user32.dll","SetWindowTextA",0x8582F8,reinterpret_cast<void*>(&XSetWindowTextA)},
    {A_MessageBoxA,"user32.dll","MessageBoxA",0x8582F4,reinterpret_cast<void*>(&XMessageBoxA)},
    {A_ShowCursor,"user32.dll","ShowCursor",0x8582EC,reinterpret_cast<void*>(&XShowCursor)},
    {A_ClientToScreen,"user32.dll","ClientToScreen",0x8582FC,reinterpret_cast<void*>(&XClientToScreen)},
    {A_SetCursorPos,"user32.dll","SetCursorPos",0x858300,reinterpret_cast<void*>(&XSetCursorPos)},
    {A_GetWindowRect,"user32.dll","GetWindowRect",0x8582F0,reinterpret_cast<void*>(&XGetWindowRect)},
    {A_SetWindowPos,"user32.dll","SetWindowPos",0x8582E0,reinterpret_cast<void*>(&XSetWindowPos)},
    {A_OutputDebugStringA,"kernel32.dll","OutputDebugStringA",0x858118,reinterpret_cast<void*>(&XOutputDebugStringA)},
    {A_timeGetDevCaps,"winmm.dll","timeGetDevCaps",0x858318,reinterpret_cast<void*>(&XtimeGetDevCaps)},
    {A_timeBeginPeriod,"winmm.dll","timeBeginPeriod",0x858314,reinterpret_cast<void*>(&XtimeBeginPeriod)},
    {A_timeGetTime,"winmm.dll","timeGetTime",0x858310,reinterpret_cast<void*>(&XtimeGetTime)},
    {A_timeEndPeriod,"winmm.dll","timeEndPeriod",0x85830C,reinterpret_cast<void*>(&XtimeEndPeriod)},
    {A_GetSystemDefaultLCID,"kernel32.dll","GetSystemDefaultLCID",0x858248,reinterpret_cast<void*>(&XGetSystemDefaultLCID)},
    {A_GetUserDefaultLCID,"kernel32.dll","GetUserDefaultLCID",0x858124,reinterpret_cast<void*>(&XGetUserDefaultLCID)},
    {A_GetKeyboardLayout,"user32.dll","GetKeyboardLayout",0x8582C8,reinterpret_cast<void*>(&XGetKeyboardLayout)},
    {A_GetVersionExA,"kernel32.dll","GetVersionExA",0x858144,reinterpret_cast<void*>(&XGetVersionExA)},
    {A_GlobalMemoryStatus,"kernel32.dll","GlobalMemoryStatus",0x858140,reinterpret_cast<void*>(&XGlobalMemoryStatus)},
    {A_CoInitialize,"ole32.dll","CoInitialize",0x858350,reinterpret_cast<void*>(&XCoInitialize)},
    {A_DirectDrawCreateEx,"ddraw.dll","DirectDrawCreateEx",0x8E3EBC,reinterpret_cast<void*>(&XDirectDrawCreateEx)},
    {A_DirectSoundCreate8,"dsound.dll","DirectSoundCreate8",0x8E3ECC,reinterpret_cast<void*>(&XDirectSoundCreate8)},
    {A_Direct3DCreate9,"d3d9.dll","Direct3DCreate9",0x8E3ED4,reinterpret_cast<void*>(&XDirect3DCreate9)},
    {A_DialogBoxParamA,"user32.dll","DialogBoxParamA",0x8582CC,reinterpret_cast<void*>(&XDialogBoxParamA)},
}};
// Parchea todas las entradas (EXE y DLL) mientras exista
struct ApiPatches {
    std::array<std::optional<IatPatch>,2*A_COUNT> p;
    ApiPatches() { for(size_t i=0;i<apis.size();++i){ p[2*i].emplace(reinterpret_cast<void**>(apis[i].exeSlot),apis[i].fake); p[2*i+1].emplace(dllSlot[i],apis[i].fake); } }
};

// ---------------------------------------------------------------------------------------------------
// RW y juego
// ---------------------------------------------------------------------------------------------------
alignas(16) inline std::array<uint8,0x80> fakeCam{}, otherCam{};
inline RwCamera* camBeginRet{}; inline RwCamera* showRet{};
inline RwCamera* __cdecl XCamBegin(RwCamera* c){ trace.push_back({"cam_begin",U(c)==U(fakeCam.data()) ? "scene" : U(c)==U(otherCam.data()) ? "other" : "?"}); return camBeginRet; }
inline RsEventStatus __cdecl XRsEvent(RsEvent e,void* p){ trace.push_back({"rs_event",int(e),U(p)}); return rsEVENTPROCESSED; }
inline RwCamera* __cdecl XShowRaster(RwCamera* c,void* dev,RwUInt32 fl){ trace.push_back({"show_raster",U(c)==U(fakeCam.data()) ? "cam" : U(c) ? "?" : "null",U(dev),fl}); return showRet; }
inline void* __cdecl XGetDevice(){ trace.push_back({"get_device"}); return &fakeDev; }
alignas(16) inline std::array<uint8,0x40> fakeImage{}; alignas(16) inline std::array<uint8,2048> dstPixels{}; inline bool imageNull{};
inline RwImage* __cdecl XImageCreate(RwInt32 w,RwInt32 h,RwInt32 d){ trace.push_back({"image_create",w,h,d}); return imageNull ? nullptr : reinterpret_cast<RwImage*>(fakeImage.data()); }
inline RwImage* __cdecl XImageAlloc(RwImage* i){ trace.push_back({"image_alloc",reinterpret_cast<uint8*>(i)==fakeImage.data()}); *reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(i)+0x14)=dstPixels.data(); return i; }
inline RwBool dxtRet{};
inline RwBool __cdecl XDxt(){ trace.push_back({"dxt"}); return dxtRet; }
alignas(16) inline std::array<uint8,512> arena{}; inline bool mallocNull{};
inline void* __cdecl XMalloc(uint32 size,uint32 hint){ if(!Mine())return reinterpret_cast<void*(__cdecl*)(uint32)>(0x824257)(size); trace.push_back({"malloc",size,hint}); return mallocNull ? nullptr : arena.data(); }
inline void(__cdecl* realRwFree)(void*){};
inline void __cdecl XRwFree(void* p){ if(!Mine())return realRwFree(p); trace.push_back({"rwfree",p==arena.data() ? json("arena") : json(U(p))}); }
// CText
inline std::array<std::array<char,32>,8> gxt{}; inline uint32 nGxt{};
inline const GxtChar* __fastcall XTextGet(void* self,void*,const char* key){ trace.push_back({"text_get",U(self),Str(key)}); auto& b=gxt[nGxt++%8]; std::snprintf(b.data(),b.size(),"gxt:%s",key); return reinterpret_cast<const GxtChar*>(b.data()); }
inline void __fastcall XTextUnload(void* self,void*,uint32 a){ trace.push_back({"text_unload",U(self),uint8(a)}); }
inline void __fastcall XTextLoad(void* self,void*,uint32 a){ trace.push_back({"text_load",U(self),uint8(a)}); }
// psInitialize
inline std::array<char,64> userDir{};
inline void __cdecl XFmInit(){ trace.push_back({"filemgr_init"}); }
inline char* __cdecl XUserDirs(){ trace.push_back({"user_dirs"}); return userDir.data(); }
inline void __cdecl XSetSaveDir(const char* p){ trace.push_back({"set_save_dir",p==userDir.data(),Str(p)}); }
inline void __fastcall XLoadSettings(void* self,void*){ trace.push_back({"load_settings",U(self)}); }
inline DWORD dxVer{};
inline DWORD __cdecl XDxVersion(){ trace.push_back({"dx_version"}); return dxVer; }
// psSelectDevice
inline std::array<char*,32> vmList{}; inline char vmName[]="800 X 600 X 32";
inline char** __cdecl XVmList(){ trace.push_back({"vm_list"}); return vmList.data(); }
inline int32 numSub{}, curSub{}, setSubRet{}, numVM{}, setVMRet{};
inline std::array<std::array<int32,6>,16> vmTable{};
inline RwInt32 __cdecl XNumSub(){ trace.push_back({"num_sub"}); return numSub; }
inline RwSubSystemInfo* __cdecl XSubInfo(RwSubSystemInfo* i,RwInt32 idx){ trace.push_back({"sub_info",int32(U(i)-0xC8CFC0),idx}); auto* b=reinterpret_cast<uint8*>(i); for(int k=0;k<0x50;++k)b[k]=uint8(idx*31+k); return i; }
inline RwInt32 __cdecl XCurSub(){ trace.push_back({"cur_sub"}); return curSub; }
inline RwBool __cdecl XSetSub(RwInt32 s){ trace.push_back({"set_sub",s}); return setSubRet; }
inline RwInt32 __cdecl XNumVM(){ trace.push_back({"num_vm"}); return numVM; }
inline RwVideoMode* __cdecl XVmInfo(RwVideoMode* v,RwInt32 idx){ trace.push_back({"vm_info",idx}); std::memcpy(v,vmTable[uint32(idx)%16].data(),24); return v; }
inline RwBool __cdecl XSetVM(RwInt32 idx){ trace.push_back({"set_vm",idx}); return setVMRet; }
inline void __cdecl XSetRefresh(RwUInt32 r){ trace.push_back({"set_refresh",r}); }
inline void __cdecl XSetMS(RwUInt32 l){ trace.push_back({"set_ms",l}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,22> fns{{
    {"Ps","psWindowSetText",0x7451B0,0,-1,128},{"Ps","psErrorMessage",0x7451D0,0,-1,128},{"Ps","psWarningMessage",0x7451F0,0,-1,128},
    {"Ps","psCameraBeginUpdate",0x745210,0,-1,256},{"Ps","psCameraShowRaster",0x745240,0,-1,256},{"Ps","psTimer",0x745270,0,-1,256},
    {"Ps","psGrabScreen",0x7452B0,0,-1,512},{"Ps","psMouseSetVisibility",0x7453E0,0,-1,128},{"Ps","psMouseSetPos",0x7453F0,0,-1,1024},
    {"Ps","psPathnameCreate",0x745470,0,-1,512},{"Ps","psPathnameDestroy",0x7454E0,0,-1,128},{"Ps","psPathGetSeparator",0x745500,0,-1,64},
    {"Ps","psInstallFileSystem",0x745520,0,-1,64},{"Ps","psNativeTextureSupport",0x745530,0,-1,128},{"Ps","psDebugMessageHandler",0x745540,0,-1,128},
    {"Ps","GetVideoMemInfo",0x7455E0,0,-1,256},{"Ps","CheckDirectSound",0x745840,0,-1,256},{"Ps","psTerminate",0x7458A0,0,-1,64},
    {"Ps","psAlwaysOnTop",0x7458B0,0,-1,256},{"Ps","psSelectDevice",0x746190,0,-1,1024},{"Ps","InitialiseLanguage",0x7465B0,0,-1,512},
    {"Ps","psInitialize",0x747420,0,-1,512},
}};

struct Snap { uintptr_t a; std::vector<uint8> b;
    void Restore() const { std::memcpy(reinterpret_cast<void*>(a),b.data(),b.size()); }
    bool Same() const { return std::memcmp(reinterpret_cast<const void*>(a),b.data(),b.size())==0; } };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ps22-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ps22-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ps22");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"ps22-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,22> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // IAT: funciones reales y entradas de la DLL
    HMODULE self{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&done),&self);
    json missing=json::array();
    for(size_t i=0;i<apis.size();++i){ const HMODULE m=GetModuleHandleA(apis[i].mod); realFn[i]=m ? reinterpret_cast<void*>(GetProcAddress(m,apis[i].name)) : nullptr; dllSlot[i]=FindIat(self,realFn[i]); if(!realFn[i]||!dllSlot[i])missing.push_back(apis[i].name); }
    Write({{"event","info"},{"dll_iat_missing",missing}});
    if(!missing.empty()){Write({{"event","error"},{"reason","dll iat not found"},{"apis",missing}});return;}

    std::array<bool,22> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtables falsas
    for(auto* vt:{&ddVt,&dsVt,&d3dVt,&devVt,&surfVt})for(auto& v:*vt)v=reinterpret_cast<void*>(&XTrap);
    for(auto* vt:{&ddVt,&dsVt,&d3dVt,&surfVt})(*vt)[2]=reinterpret_cast<void*>(&XRelease);
    ddVt[23]=reinterpret_cast<void*>(&XGetAvailableVidMem); dsVt[4]=reinterpret_cast<void*>(&XGetCaps);
    d3dVt[6]=reinterpret_cast<void*>(&XGetAdapterModeCount); d3dVt[7]=reinterpret_cast<void*>(&XEnumAdapterModes);
    devVt[8]=reinterpret_cast<void*>(&XGetDisplayMode); devVt[33]=reinterpret_cast<void*>(&XGetFrontBufferData); devVt[36]=reinterpret_cast<void*>(&XCreateOffscreen);
    surfVt[13]=reinterpret_cast<void*>(&XLockRect); surfVt[14]=reinterpret_cast<void*>(&XUnlockRect);

    // estado global guardado
    std::vector<Snap> snaps; auto snap=[&](uintptr_t a,size_t n){ snaps.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)}); };
    snap(0xC17040,0x18);   // RsGlobal: appName, maximumWidth/Height, frameLimit, quit, ps
    snap(0xC8CF88,0x28);   // PsGlobal
    snap(0xC8CF68,0x20);   // s_OSStatus
    snap(0x8D6238,13);     // fabricante de CPU
    snap(0xC8D4C0,4);      // gGameState
    snap(0xBA6748,0x100);  // FrontEndMenuManager (idiomas, modo de video, antialias, limitador)
    snap(0xBAB318,1);      // CLoadingScreen::m_bActive
    snap(0xC1703C,4);      // Scene.m_pRwCamera
    snap(0xC8CFC0,0x500);  // GsubSysInfo
    snap(0xC920F0,0x2C);   // GnumSubSystems, GcurSelSS, UseDefaultVM, rdtsc, MultipleSubSystems
    snap(0x8D6218,0xC);    // IsVMNotSelected, GcurSelVM
    auto restoreAll=[&]{ for(auto& s:snaps)s.Restore(); };
    const bool savedSkip=g_MiscConfig.SkipDeviceSelection; g_MiscConfig.SkipDeviceSelection=false;
    auto** rwEngine=reinterpret_cast<uint8**>(0xC97B24);
    auto& rsPs=*reinterpret_cast<void**>(0xC17054);
    alignas(16) static std::array<uint8,0x28> fakePs{};
    static std::array<char,64> appName{}, text{};

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto pick=[&](std::initializer_list<int32> l){ return *(l.begin()+next()%l.size()); };
                auto randStr=[&](std::array<char,64>& b,const char* cs){ const size_t n=next()%48; const size_t m=std::strlen(cs); for(size_t k=0;k<n;++k)b[k]=cs[next()%m]; b[n]=0; return b.data(); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5022u; if(!stt)stt=1;
                    trace=json::array(); nSys=nUsr=nKb=0; nGxt=0; dpiCalls=sdlCalls=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // bloque Ps falso
                    for(auto& b:fakePs)b=uint8(next()); *reinterpret_cast<uint32*>(fakePs.data())=0x0A0B0C01; *reinterpret_cast<uint32*>(fakePs.data()+4)=0x0A0B0C02;
                    *reinterpret_cast<uint32*>(fakePs.data()+8)=uint32(pick({0,1,1,2,-1}));
                    retSetText=pick({0,1,7}); retMsgBox=pick({0,1,2,6,7}); retShowCursor=int32(next()%7)-3; retGetRect=pick({0,1,1,3}); retSetPos=pick({0,1,1,5,-1});
                    offX=int32(next()%2001)-1000; offY=int32(next()%2001)-1000;
                    { const LONG l=LONG(next()%201)-100, t=LONG(next()%201)-100; fixRect={l,t,l+LONG(next()%21),t+LONG(next()%21)}; }

                    if(nm=="psWindowSetText"||nm=="psErrorMessage"||nm=="psWarningMessage") {
                        const char* s=next()%8 ? randStr(text,"abc XYZ/\\:._019") : nullptr;
                        const char* app=next()%6 ? randStr(appName,"GTA San Andreas") : nullptr;
                        rsPs=fakePs.data(); *reinterpret_cast<const char**>(0xC17040)=app;
                        { ApiPatches ap; reinterpret_cast<void(__cdecl*)(const char*)>(a)(s); }
                    }
                    else if(nm=="psCameraBeginUpdate") {
                        *reinterpret_cast<void**>(0xC1703C)=next()%8 ? fakeCam.data() : nullptr;
                        camBeginRet=next()%3 ? reinterpret_cast<RwCamera*>(next()%2 ? fakeCam.data() : otherCam.data()) : nullptr;
                        RwCamera* arg=next()%2 ? reinterpret_cast<RwCamera*>(otherCam.data()) : nullptr;
                        EntryPatch c1(0x7EE190,XCamBegin), c2(bit_cast<uintptr_t>(&RwCameraBeginUpdate),XCamBegin), e1(0x619B60,XRsEvent), e2(bit_cast<uintptr_t>(&RsEventHandler),XRsEvent);
                        { ApiPatches ap; ret=reinterpret_cast<int32(__cdecl*)(RwCamera*)>(a)(arg); }
                    }
                    else if(nm=="psCameraShowRaster") {
                        *reinterpret_cast<uint8*>(0xBA6794)=uint8(next()%2); *reinterpret_cast<uint8*>(0xBAB318)=uint8(next()%2);
                        showRet=next()%3 ? reinterpret_cast<RwCamera*>(fakeCam.data()) : nullptr; rsPs=fakePs.data();
                        RwCamera* arg=next()%6 ? reinterpret_cast<RwCamera*>(fakeCam.data()) : nullptr;
                        EntryPatch s1(0x7EE370,XShowRaster), s2(bit_cast<uintptr_t>(&RwCameraShowRaster),XShowRaster);
                        RwCamera* r; { ApiPatches ap; r=reinterpret_cast<RwCamera*(__cdecl*)(RwCamera*)>(a)(arg); }
                        ret=r==reinterpret_cast<RwCamera*>(fakeCam.data()) ? json("cam") : r ? json("?") : json(nullptr);
                    }
                    else if(nm=="psTimer") {
                        capsMin=next()%4 ? 1+next()%16 : next()%0x10000; capsMax=1000000; capsRes=next()%5 ? 0 : 97; timeValue=next();
                        { ApiPatches ap; ret=reinterpret_cast<uint32(__cdecl*)()>(a)(); }
                    }
                    else if(nm=="psGrabScreen") {
                        dmW=next()%21; dmH=next()%21; lockPitch=int32(4*(next()%24)+4*(next()%3)); imageNull=next()%8==0;
                        for(auto& b:srcPixels)b=uint8(next()); for(auto& b:fakeImage)b=0; for(auto& b:dstPixels)b=0xCD;
                        rsPs=fakePs.data();
                        EntryPatch d1(0x7F9D50,XGetDevice), d2(bit_cast<uintptr_t>(&RwD3D9GetCurrentD3DDevice),XGetDevice);
                        EntryPatch i1(0x8026E0,XImageCreate), i2(bit_cast<uintptr_t>(&RwImageCreate),XImageCreate), i3(0x8027A0,XImageAlloc), i4(bit_cast<uintptr_t>(&RwImageAllocatePixels),XImageAlloc);
                        RwImage* r; { ApiPatches ap; r=reinterpret_cast<RwImage*(__cdecl*)(RwCamera*)>(a)(reinterpret_cast<RwCamera*>(fakeCam.data())); }
                        ret=r ? json("image") : json(nullptr); out["pixels"]=Fnv(dstPixels.data(),dstPixels.size()); out["head"]=ReversingProbe::Bytes(dstPixels.data(),64);
                    }
                    else if(nm=="psMouseSetVisibility") {
                        const int32 v=pick({0,1,1,0,2,-1,0x100,int32(next())});
                        { ApiPatches ap; reinterpret_cast<void(__cdecl*)(int32)>(a)(v); }
                    }
                    else if(nm=="psMouseSetPos") {
                        auto val=[&]()->float{ switch(next()%7){case 0:return float(int32(next()%2001)-1000);case 1:return float(int32(next()%4000001)-2000000)/1000.f;
                            case 2:return float(int32(next()%200001)-100000)*0.37f;case 3:return float(int64(next())*int64(next()%5+1)-int64(0x100000000))*(next()%2?1.f:-1.f);
                            case 4:return float(int32(next()%11)-5)*0.25f;case 5:return float(next()%5000)+0.5f;default:return float(int32(next()%1281));} };
                        RwV2d pos{val(),val()}; rsPs=fakePs.data();
                        { ApiPatches ap; reinterpret_cast<void(__cdecl*)(RwV2d*)>(a)(&pos); }
                        out["pos"]={F(pos.x),F(pos.y)};
                    }
                    else if(nm=="psPathnameCreate") {
                        const char* s=randStr(text,"ab/\\/c.:_ /X/");
                        mallocNull=next()%10==0; for(auto& b:arena)b=0xCD;
                        EntryPatch m1(0x72F420,XMalloc), m2(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),XMalloc);
                        char* r; { ApiPatches ap; r=reinterpret_cast<char*(__cdecl*)(const char*)>(a)(s); }
                        ret=r==reinterpret_cast<char*>(arena.data()) ? json("arena") : r ? json("?") : json(nullptr);
                        out["arena"]=ReversingProbe::Bytes(arena.data(),std::strlen(s)+4); out["src"]=Str(s);
                    }
                    else if(nm=="psPathnameDestroy") {
                        char* p=next()%4==0 ? nullptr : next()%2 ? reinterpret_cast<char*>(arena.data()) : reinterpret_cast<char*>(uintptr_t(0x12340000+next()%0x1000));
                        auto& slot=*reinterpret_cast<void(__cdecl**)(void*)>(*rwEngine+0x138); realRwFree=slot;
                        { DWORD o{},ign{}; VirtualProtect(&slot,4,PAGE_READWRITE,&o); slot=XRwFree; VirtualProtect(&slot,4,o,&ign); }
                        { ApiPatches ap; reinterpret_cast<void(__cdecl*)(char*)>(a)(p); }
                        { DWORD o{},ign{}; VirtualProtect(&slot,4,PAGE_READWRITE,&o); slot=realRwFree; VirtualProtect(&slot,4,o,&ign); }
                    }
                    else if(nm=="psPathGetSeparator") { ApiPatches ap; ret=int(reinterpret_cast<char(__cdecl*)()>(a)()); }
                    else if(nm=="psInstallFileSystem") { ApiPatches ap; ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                    else if(nm=="psNativeTextureSupport") {
                        dxtRet=pick({0,1,1,2,-1,int32(next())});
                        EntryPatch x1(0x7F9C30,XDxt), x2(bit_cast<uintptr_t>(&RwD3D9DeviceSupportsDXTTexture),XDxt);
                        { ApiPatches ap; ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                    }
                    else if(nm=="psDebugMessageHandler") {
                        const int32 type=int32(next()%5); const char* s=randStr(text,"RW debug: x=1 ");
                        { ApiPatches ap; reinterpret_cast<void(__cdecl*)(int32,const char*)>(a)(type,s); }
                    }
                    else if(nm=="psTerminate") { ApiPatches ap; reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="psAlwaysOnTop") {
                        const int32 v=pick({0,1,1,0,2,-1,0x100}); rsPs=fakePs.data();
                        { ApiPatches ap; ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(v); }
                    }
                    else if(nm=="GetVideoMemInfo") {
                        ddHr=pick({0,0,0,int32(E_FAIL),int32(DDERR_NODIRECTDRAWHW),1}); vidHr=pick({0,0,int32(DDERR_INVALIDPARAMS),1,int32(E_FAIL)}); vidTotal=next(); vidFree=next();
                        DWORD total=0x11111111, avail=0x22222222;
                        { ApiPatches ap; ret=int32(reinterpret_cast<HRESULT(__cdecl*)(LPDWORD,LPDWORD)>(a)(&total,&avail)); }
                        out["total"]=total; out["avail"]=avail;
                    }
                    else if(nm=="CheckDirectSound") {
                        coHr=pick({0,1,int32(RPC_E_CHANGED_MODE)}); dsHr=pick({0,0,0,int32(E_FAIL),int32(DSERR_NODRIVER),1}); capsHr=pick({0,0,0,1,int32(E_FAIL),int32(DSERR_INVALIDPARAM)});
                        { ApiPatches ap; ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                    }
                    else if(nm=="InitialiseLanguage"||nm=="psInitialize") {
                        auto lang=[&]()->DWORD{ const DWORD l=DWORD(pick({7,9,0xA,0xC,0x10,0x10,int32(next()%0x400)})); return l|(next()%3 ? DWORD(next()%64)<<10 : 0)|(next()%4==0 ? DWORD(next())<<16 : 0); };
                        for(auto& v:sysLcid)v=lang(); for(auto& v:usrLcid)v=lang(); for(auto& v:kbLayout)v=lang();
                        auto* menu=reinterpret_cast<uint8*>(0xBA67C4); for(int k=0;k<16;++k)menu[k]=uint8(next());
                        EntryPatch t1(0x69FF20,XTextUnload), t2(bit_cast<uintptr_t>(&CText::Unload),XTextUnload), t3(0x6A01A0,XTextLoad), t4(bit_cast<uintptr_t>(&CText::Load),XTextLoad);
                        if(nm=="InitialiseLanguage") { ApiPatches ap; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else {
                            verPlatform=DWORD(pick({2,2,2,1,1,0,3})); verMajor=DWORD(pick({3,4,4,5,5,6,10,int32(next()%20)})); verMinor=DWORD(pick({0,0,1,10,int32(next()%5)}));
                            dxVer=DWORD(pick({0x900,0x900,0x900,0x901,0x800,0x8FF,0,0x700}));
                            coHr=pick({0,1}); ddHr=pick({0,0,int32(E_FAIL)}); vidHr=pick({0,1,int32(E_FAIL)}); vidTotal=next(); vidFree=next();
                            dsHr=pick({0,0,0,int32(E_FAIL),1}); capsHr=pick({0,0,0,1,int32(E_FAIL)});
                            for(auto& m:memFix)m=SIZE_T(next());
                            randStr(userDir,"C:/Users/x/Docs");
                            auto* ps=reinterpret_cast<uint8*>(0xC8CF88); for(int k=8;k<0x28;++k)ps[k]=uint8(next());
                            auto* os=reinterpret_cast<uint8*>(0xC8CF68); for(int k=0;k<0x20;++k)os[k]=uint8(next());
                            auto* vendor=reinterpret_cast<uint8*>(0x8D6238); for(int k=0;k<13;++k)vendor[k]=uint8('a'+next()%26);
                            *reinterpret_cast<int32*>(0xC8D4C0)=int32(next()%10)+1; rsPs=fakePs.data();
                            EntryPatch g1(0x6A0050,XTextGet), g2(bit_cast<uintptr_t>(&CText::Get),XTextGet);
                            EntryPatch f1(0x5386F0,XFmInit), f2(bit_cast<uintptr_t>(&CFileMgr::Initialise),XFmInit), u1(0x744FB0,XUserDirs), u2(bit_cast<uintptr_t>(&InitUserDirectories),XUserDirs);
                            EntryPatch s1(0x619040,XSetSaveDir), s2(bit_cast<uintptr_t>(&C_PcSave::SetSaveDirectory),XSetSaveDir);
                            EntryPatch l1(0x57C8F0,XLoadSettings), l2(bit_cast<uintptr_t>(&CMenuManager::LoadSettings),XLoadSettings), dx(0x745650,XDxVersion);
                            EntryPatch dpi(bit_cast<uintptr_t>(&SetProcessDPIAware),XSetProcessDPIAware); // NOTSA, sin traza
                            { ApiPatches ap; ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                            out["ps"]=ReversingProbe::Bytes(ps,0x28); out["rs_ps_static"]=rsPs==ps; out["os"]=ReversingProbe::Bytes(os,0x20);
                            out["vendor"]=ReversingProbe::Bytes(vendor,13); out["game_state"]=*reinterpret_cast<int32*>(0xC8D4C0);
                        }
                        out["menu"]=ReversingProbe::Bytes(menu,16);
                    }
                    else if(nm=="psSelectDevice") {
                        const int32 useDef=next()%4==0; *reinterpret_cast<int32*>(0xC920FC)=useDef;
                        *reinterpret_cast<int32*>(0xC920F0)=useDef ? int32(next()%4) : 0x7777; *reinterpret_cast<int32*>(0xC920F4)=useDef ? int32(next()%3) : 0x5555;
                        *reinterpret_cast<int32*>(0xC92118)=0x66; *reinterpret_cast<uint32*>(0xC92104)=0; *reinterpret_cast<uint32*>(0xC92108)=0;
                        numSub=pick({0,1,1,1,2,2,3,17,-1}); curSub=int32(next()%3); setSubRet=next()%8 ? 1 : 0; setVMRet=next()%8 ? 1 : 0;
                        dialogRet=next()%5 ? 1 : 0; dialogVM=int32(next()%12); dialogSS=int32(next()%3);
                        numVM=int32(next()%13); GcurSelVM=next()%4==0 ? int32(next()%12) : 0; IsVMNotSelected=next()%3 ? 1 : 0;
                        *reinterpret_cast<int32*>(0xBA6820)=int32(next()%10); *reinterpret_cast<int32*>(0xBA681C)=0x4444; *reinterpret_cast<int32*>(0xBA679C)=7; *reinterpret_cast<int32*>(0xBA6810)=int32(next()%5);
                        for(auto& v:vmList)v=next()%3 ? vmName : nullptr;
                        static const int32 modes[][4]{{800,600,32,1},{800,600,32,1},{800,600,32,0},{800,600,16,1},{640,480,32,1},{1024,768,32,9},{800,601,32,1},{1024,768,24,1}};
                        for(auto& v:vmTable){ const auto& m=modes[next()%8]; v={m[0],m[1],m[2],m[3],60,int32(next()%3)}; }
                        d3dModeCount=next()%13;
                        for(auto& m:d3dModes){ const auto& v=vmTable[next()%16]; m.Width=UINT(v[0]); m.Height=UINT(next()%6 ? v[1] : v[1]+1);
                            m.Format=D3DFORMAT(pick({22,22,22,23,20,21})); m.RefreshRate=UINT(pick({0,59,60,60,75,85,120,144,int32(next()%300)})); if(next()%5==0&&m.RefreshRate==0)m.RefreshRate=60; }
                        for(int k=0;k<0x500;++k)reinterpret_cast<uint8*>(0xC8CFC0)[k]=0xEE;
                        *reinterpret_cast<int32*>(0xC17044)=0x1111; *reinterpret_cast<int32*>(0xC17048)=0x2222; rsPs=fakePs.data();
                        EntryPatch v1(0x745AF0,XVmList), v2(bit_cast<uintptr_t>(&GetVideoModeList),XVmList);
                        EntryPatch r1(0x7F2C00,XNumSub), r1d(bit_cast<uintptr_t>(&RwEngineGetNumSubSystems),XNumSub), r2(0x7F2C30,XSubInfo), r2d(bit_cast<uintptr_t>(&RwEngineGetSubSystemInfo),XSubInfo);
                        EntryPatch r3(0x7F2C60,XCurSub), r3d(bit_cast<uintptr_t>(&RwEngineGetCurrentSubSystem),XCurSub), r4(0x7F2C90,XSetSub), r4d(bit_cast<uintptr_t>(&RwEngineSetSubSystem),XSetSub);
                        EntryPatch r5(0x7F2CC0,XNumVM), r5d(bit_cast<uintptr_t>(&RwEngineGetNumVideoModes),XNumVM), r6(0x7F2CF0,XVmInfo), r6d(bit_cast<uintptr_t>(&RwEngineGetVideoModeInfo),XVmInfo);
                        EntryPatch r7(0x7F2D50,XSetVM), r7d(bit_cast<uintptr_t>(&RwEngineSetVideoMode),XSetVM), r8(0x7F8580,XSetRefresh), r8d(bit_cast<uintptr_t>(&RwD3D9EngineSetRefreshRate),XSetRefresh);
                        EntryPatch r9(0x7F84F0,XSetMS), r9d(bit_cast<uintptr_t>(&RwD3D9EngineSetMultiSamplingLevels),XSetMS);
#ifdef NOTSA_USE_SDL3
                        EntryPatch sd1(bit_cast<uintptr_t>(&notsa::SDLWrapper::Initialize),XSdlInitialize), sd2(bit_cast<uintptr_t>(&SDL_SetWindowSize),XSdlSetWindowSize); // NOTSA, sin traza
#endif
                        { ApiPatches ap; ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                        auto* g=reinterpret_cast<uint8*>(0xC920F0); std::vector<uint8> gb(g,g+0x2C);
                        out["tsc_written"]=*reinterpret_cast<uint32*>(0xC92104)!=0||*reinterpret_cast<uint32*>(0xC92108)!=0; for(int k=0x14;k<0x1C;++k)gb[k]=0;
                        out["globals"]=gb; out["vm"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0x8D6218),0xC);
                        out["screen_item"]=*reinterpret_cast<int32*>(0xBA679C); out["prefs_vm"]=*reinterpret_cast<int32*>(0xBA681C);
                        out["max"]={*reinterpret_cast<int32*>(0xC17044),*reinterpret_cast<int32*>(0xC17048)}; out["subinfo"]=Fnv(reinterpret_cast<void*>(0xC8CFC0),0x500);
                    }
                    out["fake_ps"]=ReversingProbe::Bytes(fakePs.data(),fakePs.size());
                    restoreAll();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false); const uint32 notsaOrig=dpiCalls+sdlCalls;
                const auto candidate=run(true); const uint32 notsaCand=dpiCalls+sdlCalls;
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},
                             {"events",original["trace"].size()},{"notsa_calls",json::array({notsaOrig,notsaCand})}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreAll(); g_MiscConfig.SkipDeviceSelection=savedSkip;
    bool restored=true; for(auto& s:snaps)restored&=s.Same();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
