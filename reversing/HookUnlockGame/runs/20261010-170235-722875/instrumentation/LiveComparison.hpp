#pragma once
#include "IdlePairs.hpp"
#include "RenderPairs.hpp"
#include <reversiblehooks/ReversibleHook/Simple.h>
extern bool ProcessGameLogic(INT nCmdShow);
#ifdef NOTSA_USE_SDL3
#include <SDL3/SDL.h>
#include "app/platform/SDLWrapper.hpp"
#endif

namespace HookUnlockGameLive {
inline bool initialized{}, done{}, pairsPassed{}, ready{};
inline int frames{}, idleFrames{}, frontendFrames{}, backgroundSkipped{};
inline bool inDispatch{};
inline bool enabled[3]{}, locked[3]{};
inline ReversibleHooks::ReversibleHook::Base* hooks[3]{};
inline std::array<std::vector<uint8>,3> exeBytes{}, dllBytes{};
inline uintptr_t dllAddress[3]{};
inline const char* names[]{"Idle","FrontendIdle","Render2dStuff"};
inline uintptr_t addresses[]{0x53E920,0x53E770,0x53E230};
inline int dispatchFunction{-1};
inline bool dispatchState{}, dispatchForeground{}, dispatchRoute{};
inline DWORD dispatchStarted{};
inline bool savedMenu{},savedActivate{},savedLoading{},savedStartLoading{};
inline ReversibleHooks::ReversibleHook::Base* appHook{};
inline uint32 savedQuit{};
inline volatile LONG renderCalls{};
inline std::array<uint8,5> renderCallBytes{};
inline bool renderCallPatched{};
inline void Write(const json& row);
inline bool Restore();
__declspec(naked) __declspec(noinline) inline void RenderForward() {
    __asm {
        pushfd
        inc dword ptr [renderCalls]
        popfd
        push 053E230h
        ret
    }
}
inline void PutCall(const std::array<uint8,5>& bytes) {
    DWORD old{},ignored{};void* p=reinterpret_cast<void*>(0x53EB12);
    VirtualProtect(p,5,PAGE_EXECUTE_READWRITE,&old);std::memcpy(p,bytes.data(),5);
    VirtualProtect(p,5,old,&ignored);FlushInstructionCache(GetCurrentProcess(),p,5);
}
inline bool PatchRenderCall() {
    std::memcpy(renderCallBytes.data(),reinterpret_cast<void*>(0x53EB12),5);
    if(renderCallBytes[0]!=0xE8)return false;
    const auto target=0x53EB17+*reinterpret_cast<const int32*>(renderCallBytes.data()+1);
    if(target!=0x53E230)return false;
    auto bytes=renderCallBytes;const int32 displacement=int32(uintptr_t(&RenderForward)-0x53EB17);
    std::memcpy(bytes.data()+1,&displacement,4);PutCall(bytes);renderCallPatched=true;return true;
}
inline bool BackgroundCheck(int mask,bool renderParam,bool appActive) {
    ReversingProbe::PumpMessages();
    const uint32 fg=*reinterpret_cast<const uint32*>(0x8D621C);const auto frame=CTimer::m_FrameCounter;
    const uint8 foregroundTracking=*reinterpret_cast<const uint8*>(0xC920EC);
    POINT before{},after{};const bool cursorBefore=GetCursorPos(&before)!=0;
    hooks[0]->LockState(false);hooks[0]->State(false);
    const bool appEnabled=appHook->Hooked(),appLocked=appHook->Locked();
    const auto appDllAddress=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(appHook)->GetHookOurAddress());
    const auto appExeBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x53EC10),5);
    const auto appDllBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(appDllAddress),5);
    appHook->LockState(false);appHook->State(appActive);
    *reinterpret_cast<uint32*>(0x8D621C)=0;inDispatch=true;
    const bool predicate=ShouldPauseOriginalIdleInBackground();
    const bool loading=FrontEndMenuManager.m_bStartGameLoading;const auto quit=RsGlobal.quit;
    FrontEndMenuManager.m_bStartGameLoading=false;RsGlobal.quit=false;
    bool messagePump=true,messageApplicable=false;
#ifdef NOTSA_USE_SDL3
    // SDLWrapper consume estos eventos pero no los convierte en WM_ACTIVATE ni actualiza ForegroundApp.
    // Prueba sólo la cola del backend mientras el bucle está pausado; no simula foco del escritorio.
    auto* sdlWindow=notsa::SDLWrapper::GetWindow();
    const SDL_WindowID sdlWindowId=sdlWindow?SDL_GetWindowID(sdlWindow):0;
    SDL_Event lost{},gained{};lost.type=SDL_EVENT_WINDOW_FOCUS_LOST;gained.type=SDL_EVENT_WINDOW_FOCUS_GAINED;
    lost.window.windowID=gained.window.windowID=sdlWindowId;
    const bool lostQueued=sdlWindowId&&SDL_PushEvent(&lost);
    const bool gainedQueued=sdlWindowId&&SDL_PushEvent(&gained);
    const bool focusEventsPresent=SDL_HasEvent(SDL_EVENT_WINDOW_FOCUS_LOST)&&SDL_HasEvent(SDL_EVENT_WINDOW_FOCUS_GAINED);
#endif
#ifndef NOTSA_USE_SDL3
    constexpr UINT probeMessage=WM_APP+0x451;messageApplicable=true;
    messagePump=PostThreadMessage(GetCurrentThreadId(),probeMessage,0x51E,0)!=0;
#endif
    const DWORD loopStart=GetTickCount();bool loopStatus=true;
    for(int attempt=0;attempt<16;++attempt){
        loopStatus&=ProcessGameLogic(SW_SHOW);
#ifndef NOTSA_USE_SDL3
        MSG pending{};const bool remains=PeekMessage(&pending,nullptr,probeMessage,probeMessage,PM_NOREMOVE)!=0;
        if(!remains&&GetTickCount()-loopStart>=90)break;
#else
        break;
#endif
    }
    const DWORD loopTime=GetTickCount()-loopStart;
#ifdef NOTSA_USE_SDL3
    const bool lostConsumed=!SDL_HasEvent(SDL_EVENT_WINDOW_FOCUS_LOST);
    const bool gainedConsumed=!SDL_HasEvent(SDL_EVENT_WINDOW_FOCUS_GAINED);
    const bool sdlQueuePassed=lostQueued&&gainedQueued&&focusEventsPresent&&lostConsumed&&gainedConsumed&&
        *reinterpret_cast<const uint32*>(0x8D621C)==0&&frame==CTimer::m_FrameCounter&&loopStatus&&loopTime>=90;
    messagePump&=sdlQueuePassed;
#endif
#ifndef NOTSA_USE_SDL3
    MSG remaining{};messagePump&=PeekMessage(&remaining,nullptr,probeMessage,probeMessage,PM_NOREMOVE)==0;
    if(!messagePump)PeekMessage(&remaining,nullptr,probeMessage,probeMessage,PM_REMOVE);
#endif
    const auto status=appActive?AppEventHandler(rsIDLE,renderParam?reinterpret_cast<void*>(1):nullptr):rsEVENTPROCESSED;
    FrontEndMenuManager.m_bStartGameLoading=loading;RsGlobal.quit=quit;
    inDispatch=false;*reinterpret_cast<uint32*>(0x8D621C)=fg;
    *reinterpret_cast<uint8*>(0xC920EC)=foregroundTracking;
    appHook->State(appEnabled);appHook->LockState(appLocked);
    const bool cursorAfter=GetCursorPos(&after)!=0;
    const bool callOk=messagePump&&predicate&&loopTime>=90&&loopStatus&&status==rsEVENTPROCESSED&&frame==CTimer::m_FrameCounter&&cursorBefore&&cursorAfter&&before.x==after.x&&before.y==after.y;
    const bool gameRestored=Restore();
    const bool appRestored=appHook->Hooked()==appEnabled&&appHook->Locked()==appLocked&&
        appExeBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x53EC10),5)&&
        appDllBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(appDllAddress),5);
    const bool stateRestored=gameRestored&&appRestored&&
        *reinterpret_cast<const uint32*>(0x8D621C)==fg&&
        *reinterpret_cast<const uint8*>(0xC920EC)==foregroundTracking&&
        FrontEndMenuManager.m_bStartGameLoading==loading&&RsGlobal.quit==quit;
    const bool ok=callOk&&stateRestored;
#ifdef NOTSA_USE_SDL3
    Write({{"event","sdl_background_focus_queue"},{"mask",mask},{"app_active",appActive},{"param_nonnull",renderParam},
        {"window_id",sdlWindowId},{"focus_lost_queued",lostQueued},{"focus_gained_queued",gainedQueued},
        {"events_present_before_pump",focusEventsPresent},{"focus_lost_consumed",lostConsumed},{"focus_gained_consumed",gainedConsumed},
        {"foreground_changed_by_sdl_event",false},{"frame_unchanged",frame==CTimer::m_FrameCounter},
        {"scope","Cola SDL consumida durante pausa; no prueba WM_ACTIVATE ni foco del escritorio"},
        {"passed",sdlQueuePassed&&stateRestored},{"state_restored",stateRestored}});
#endif
    Write({{"event","background_message_pump"},{"mask",mask},{"app_active",appActive},{"param_nonnull",renderParam},
        {"applicable",messageApplicable},{"native_message_consumed",messagePump},{"passed",messagePump&&stateRestored},
        {"frame_unchanged",frame==CTimer::m_FrameCounter},{"backend",messageApplicable?"Win32":"SDL3 (prueba WM_APP no aplicable)"}});
    Write({{"event","background_dispatch_guard"},{"mask",mask},{"param_nonnull",renderParam},
        {"app_active",appActive},{"pause_predicate",predicate},{"loop_elapsed_ms",loopTime},{"loop_returned",loopStatus},{"passed",ok},{"frame_unchanged",frame==CTimer::m_FrameCounter},{"cursor_unchanged",before.x==after.x&&before.y==after.y},
        {"state_restored",stateRestored},{"app_bytes_restored",appRestored},{"game_bytes_restored",gameRestored},
        {"foreground_tracking_before",foregroundTracking},{"foreground_tracking_after",*reinterpret_cast<const uint8*>(0xC920EC)},
        {"foreground_tracking_restored",*reinterpret_cast<const uint8*>(0xC920EC)==foregroundTracking}});
    return ok;
}
inline void Write(const json& row){HookUnlockGamePairs::Write(row);}
inline bool Restore() {
    bool ok=true;
    for(int i=0;i<3;++i){hooks[i]->LockState(false);hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);
        ok&=hooks[i]->Hooked()==enabled[i]&&hooks[i]->Locked()==locked[i]&&
            exeBytes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[i]),5)&&
            dllBytes[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(dllAddress[i]),5);}
    return ok;
}
inline void Initialize() {
    if(initialized||done)return;
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;
    HookUnlockGamePairs::Run();
    if(!HookUnlockGamePairs::log.is_open()){done=true;return;}
    initialized=true;
    for(int i=0;i<3;++i){hooks[i]=HookUnlockGamePairs::FindHook("Game",names[i]);
        if(!hooks[i]){Write({{"event","error"},{"reason","missing Game hook"},{"function",names[i]}});done=true;return;}
        enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();
        dllAddress[i]=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(hooks[i])->GetHookOurAddress());
        exeBytes[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[i]),5);
        dllBytes[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(dllAddress[i]),5);
    }
    pairsPassed=HookUnlockRenderPairs::Run();
    appHook=HookUnlockGamePairs::FindHook("App","AppEventHandler");
    if(!appHook){done=true;Write({{"event","error"},{"reason","missing AppEventHandler"},{"state_restored",Restore()}});return;}
    if(!pairsPassed){Restore();done=true;Write({{"event","error"},{"reason","Render pair failure"},{"state_restored",true}});}
}
inline void Update(){
    if(done)return;
    if(!HookUnlockGamePairs::log.is_open()){
        char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
        const auto dir=std::filesystem::path(exe).parent_path();
        if(!std::filesystem::exists(dir/"hookunlockgame-runtime.start")){done=true;return;}
        HookUnlockGamePairs::log.open(dir/"hookunlockgame-runtime.jsonl");
        Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("hookunlockgame");
    }
    if(FindPlayerPed()&&!CCutsceneMgr::IsRunning())ready=true;
}
// Llamar únicamente desde el dispatcher superior de app.cpp, ANTES de entrar al objetivo.
// La espera por jugador se hace en CGame::Process, pero aquí nunca hay un objetivo Game en la pila.
inline void Before(RsEvent& event,void*) {
    if(!initialized&&ready)Initialize();
    if(!initialized||done||inDispatch)return;
    if(event!=rsIDLE&&event!=rsFRONTENDIDLE)return;
    const bool fg=*reinterpret_cast<const uint32*>(0x8D621C)!=0;
    // No desactivar Idle en segundo plano: el EXE secuestra el cursor y la DLL mantiene guarda.
    if(!fg){++backgroundSkipped;return;}
    // Alternar frames completos por el dispatcher sin entrar desde Idle ni cambiar gGameState.
    event=(frames&1)?rsFRONTENDIDLE:rsIDLE;
    dispatchFunction=event==rsIDLE?0:1;
    const int mask=frames/40;
    if(frames%40==0){for(bool appActive:{false,true})for(bool param:{false,true})if(!BackgroundCheck(mask,param,appActive)){done=true;Write({{"event","error"},{"reason","background guard"},{"state_restored",Restore()}});return;}}
    dispatchState=(mask&(1<<dispatchFunction))!=0;
    dispatchForeground=fg;
    savedMenu=FrontEndMenuManager.m_bMenuActive;savedActivate=FrontEndMenuManager.m_bActivateMenuNextFrame;
    savedLoading=FrontEndMenuManager.m_bLoadingData;savedStartLoading=FrontEndMenuManager.m_bStartGameLoading;savedQuit=RsGlobal.quit;
    FrontEndMenuManager.m_bMenuActive=dispatchFunction==1;
    FrontEndMenuManager.m_bActivateMenuNextFrame=false;FrontEndMenuManager.m_bStartGameLoading=false;RsGlobal.quit=false;
    for(int i=0;i<3;++i){hooks[i]->LockState(false);hooks[i]->State((mask&(1<<i))!=0);}
    FlushInstructionCache(GetCurrentProcess(),nullptr,0);
    dispatchRoute=hooks[dispatchFunction]->Hooked()==dispatchState&&ReversingProbe::JumpsToDll(addresses[dispatchFunction])==dispatchState;
    for(int i=0;i<3;++i)dispatchRoute&=hooks[i]->Hooked()==((mask&(1<<i))!=0)&&ReversingProbe::JumpsToDll(addresses[i])==((mask&(1<<i))!=0);
    renderCalls=0;
    if(!PatchRenderCall()){Restore();done=true;Write({{"event","error"},{"reason","EXE render callsite"},{"state_restored",true}});return;}
    inDispatch=true;dispatchStarted=GetTickCount();
}
inline void After(RsEvent event) {
    if(!inDispatch)return;
    inDispatch=false;
    const auto elapsed=GetTickCount()-dispatchStarted;
    if(renderCallPatched){PutCall(renderCallBytes);renderCallPatched=false;}
    const bool restored=Restore();
    const bool covered=dispatchFunction!=0||renderCalls>0;
    if(covered){if(dispatchFunction==0)++idleFrames;else ++frontendFrames;}
    Write({{"event",covered?"game_dispatch_frame":"game_dispatch_wait_render"},{"frame",frames},{"function",names[dispatchFunction]},
        {"active",dispatchState},{"foreground_before",dispatchForeground},{"foreground_after",*reinterpret_cast<const uint32*>(0x8D621C)!=0},
        {"route_ok",dispatchRoute},{"returned",true},{"elapsed_ms",elapsed},{"bytes_restored",restored},
        {"menu_active",FrontEndMenuManager.m_bMenuActive},{"event_value",int(event)},{"mask",frames/40},{"render_calls",LONG(renderCalls)}});
    FrontEndMenuManager.m_bMenuActive=savedMenu;FrontEndMenuManager.m_bActivateMenuNextFrame=savedActivate;
    FrontEndMenuManager.m_bLoadingData=savedLoading;FrontEndMenuManager.m_bStartGameLoading=savedStartLoading;RsGlobal.quit=savedQuit;
    if(!dispatchRoute||!restored){done=true;Write({{"event","error"},{"reason","dispatch route/state"},{"state_restored",restored}});return;}
    if(!covered)return;
    if(++frames>=320){done=true;Write({{"event","complete"},{"pairs",2560},{"state_restored",restored},
        {"idle_frames",idleFrames},{"frontend_frames",frontendFrames},{"background_skipped",backgroundSkipped},
        {"foreground_only",true},{"frontend_unlock_proven",frontendFrames==160},{"idle_background_unlock_proven",false}});}
}
}
