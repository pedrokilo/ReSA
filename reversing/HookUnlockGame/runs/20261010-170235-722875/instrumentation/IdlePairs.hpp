#pragma once
// Banco dentro del motor: Game::Idle (0x53E920) y Game::FrontendIdle (0x53E770). Original (cuerpo del EXE, hooks
// desbloqueados y desactivados durante la llamada) frente a candidato (hooks activados) llamando a la dirección del EXE
// desde aquí (el banco corre dentro de CGame::Process, que Idle llama: Process está sustituido y no hay reentrada).
// Se sustituye CADA función llamada (dirección del EXE y cuerpo de la DLL, resuelto antes de escribir ningún parche)
// por un thunk trazado genérico ("push índice / call Común / ret N") que guarda nombre, ECX (thiscall) y argumentos en
// un registro fijo sin reservas (el json se arma después de la llamada) y devuelve valores de fixture: ciclos del
// temporizador (limitador de 14 ms), ciclos/ms, estado del fundido, retornos de BeginUpdate/DoRWStuff y el float en ST0
// de CDraw::CalculateAspectRatio. El estado de render se traza cambiando fpRenderStateSet. Nada real se ejecuta salvo el
// vaciado de ms_weaponPedsForPC (0x53E8D0 en el EXE, en línea en la DLL), que corre sobre una lista falsa.
// El menú ImGui (UIRenderer::DrawLoop) ya no existe: la traza del candidato debe ser exactamente la del EXE.
// Única guarda NOTSA restante: ForegroundApp (0x8D621C), que se fija a 1 durante la llamada.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "app.h"
#include "app_debug.h"
#include "platform.h"
#include "Credits.h"
#include "Securom.h"
#include "RealTimeShadowManager.h"
#include "Mirrors.h"
#include "Renderer.h"
#include "VisibilityPlugins.h"
#include "Draw.h"
#include "Hud.h"
#include "Messages.h"
#include "MenuManager.h"
#include "../ProbeRuntime.hpp"

namespace HookUnlockGamePairs {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class Fn> uintptr_t A(Fn f){ static_assert(sizeof(Fn)>=sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&f,sizeof(a)); return a; }
inline bool InExe(uintptr_t a){ return a>=0x400000&&a<0x1000000; }
inline uintptr_t Body(uintptr_t a){
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}

// --- tabla de stubs: dirección del EXE, símbolo de la DLL, nombre, convención ('c' cdecl / 't' thiscall),
//     argumentos ('i' dword, 'b' byte, 'h' 16 bits, 'p' RwV2d* por valor), retorno
//     ('0' cero, '1' uno, 'C' primer argumento, 'T' ciclos, 'M' ciclos/ms, 'B' bool de fixture, 'S' estado del fundido,
//      'f' float en ST0)
struct Stub { uintptr_t exe, dll; const char* name; char conv; const char* kinds; char ret; };
inline std::vector<Stub>& Stubs() {
    static std::vector<Stub> s{
        {0x561A80,A(&CTimer::GetCurrentTimeInCycles),"GetCurrentTimeInCycles",'c',"",'T'},
        {0x561A40,A(&CTimer::GetCyclesPerMillisecond),"GetCyclesPerMillisecond",'c',"",'M'},
        {0x561B10,A(&CTimer::Update),"Timer::Update",'c',"",'0'},
        {0x727350,A(&CSprite2d::InitPerFrame),"Sprite2d::InitPerFrame",'c',"",'0'},
        {0x727260,A(&CSprite2d::SetRecipNearClip),"Sprite2d::SetRecipNearClip",'c',"",'0'},
        {0x719800,A(&CFont::InitPerFrame),"Font::InitPerFrame",'c',"",'0'},
        {0x53BEE0,A(&CGame::Process),"Game::Process",'c',"",'0'},
        {0x507750,A(&CAudioEngine::Service),"AudioEngine::Service",'t',"",'0'},
        {0x7354E0,A(&SetLightsWithTimeOfDayColour),"SetLightsWithTimeOfDayColour",'c',"i",'0'},
        {0x50AE20,A(&CCamera::GetScreenFadeStatus),"Camera::GetScreenFadeStatus",'t',"",'S'},
        {0x6194A0,A(&RsMouseSetPos),"RsMouseSetPos",'c',"p",'0'},
        {0x5556E0,A(&CRenderer::ConstructRenderList),"Renderer::ConstructRenderList",'c',"",'0'},
        {0x553910,A(&CRenderer::PreRender),"Renderer::PreRender",'c',"",'0'},
        {0x563430,A(&CWorld::ProcessPedsAfterPreRender),"World::ProcessPedsAfterPreRender",'c',"",'0'},
        {0x706AB0,A(&CRealTimeShadowManager::Update),"RealTimeShadowManager::Update",'t',"",'0'},
        {0x727140,A(&CMirrors::BeforeMainRender),"Mirrors::BeforeMainRender",'c',"",'0'},
        {0x53D7A0,A(&DoRWStuffStartOfFrame_Horizon),"DoRWStuffStartOfFrame_Horizon",'c',"hhhhhhh",'B'},
        {0x734650,A(&DefinedState),"DefinedState",'c',"",'0'},
        {0x7EE2A0,A(&RwCameraSetFarClipPlane),"RwCameraSetFarClipPlane",'c',"ii",'C'},
        {0x726090,A(&CMirrors::RenderMirrorBuffer),"Mirrors::RenderMirrorBuffer",'c',"",'0'},
        {0x53DF40,A(&RenderScene),"RenderScene",'c',"",'0'},
        {0x732F30,A(&CVisibilityPlugins::RenderWeaponPedsForPC),"RenderWeaponPedsForPC",'c',"",'0'},
        {0x53E170,A(&RenderEffects),"RenderEffects",'c',"",'0'},
        {0x50BF80,A(&CCamera::SetMotionBlurAlpha),"Camera::SetMotionBlurAlpha",'t',"i",'0'},
        {0x50B8F0,A(&CCamera::RenderMotionBlur),"Camera::RenderMotionBlur",'t',"",'0'},
        {0x53E230,A(&Render2dStuff),"Render2dStuff",'c',"",'0'},
        {0x6FF420,A(&CDraw::CalculateAspectRatio),"Draw::CalculateAspectRatio",'c',"",'f'},
        {0x72FC70,A(&CameraSize),"CameraSize",'c',"iiii",'0'},
        {0x7328C0,A(&CVisibilityPlugins::SetRenderWareCamera),"SetRenderWareCamera",'c',"i",'0'},
        {0x7EE340,A(&RwCameraClear),"RwCameraClear",'c',"iii",'C'},
        {0x619450,A(&RsCameraBeginUpdate),"RsCameraBeginUpdate",'c',"i",'B'},
        {0x57C290,A(&CMenuManager::DrawFrontEnd),"MenuManager::DrawFrontEnd",'t',"",'0'},
        {0x53E600,A(&DoFade),"DoFade",'c',"",'0'},
        {0x58D490,A(&CHud::DrawAfterFade),"Hud::DrawAfterFade",'c',"",'0'},
        {0x69EFC0,A(&CMessages::Display),"Messages::Display",'c',"b",'0'},
        {0x71A210,A(&CFont::DrawFonts),"Font::DrawFonts",'c',"",'0'},
        {0x5A87F0,A(&CCredits::RenderCredits),"Credits::RenderCredits",'c',"",'0'},
        {0x532260,A(&CDebug::DebugDisplayTextBuffer),"DebugDisplayTextBuffer",'c',"",'0'},
        {0x734640,A(&FlushObrsPrintfs),"FlushObrsPrintfs",'c',"",'0'},
        {0x7EE180,A(&RwCameraEndUpdate),"RwCameraEndUpdate",'c',"i",'C'},
        {0x619440,A(&RsCameraShowRaster),"RsCameraShowRaster",'c',"i",'C'},
        {0x541DD0,A(&CPad::UpdatePads),"Pad::UpdatePads",'c',"",'0'},
        {0x57B440,A(&CMenuManager::Process),"MenuManager::Process",'t',"",'0'},
        {0x744AE0,A(&SecuromStateDisplay),"SecuromStateDisplay",'c',"",'0'},
        // ni el EXE ni la DLL deben llamarlas (las versiones anteriores de la DLL sí): aparecerían en la traza
        {0x57C520,A(&CMenuManager::CentreMousePointer),"MenuManager::CentreMousePointer",'t',"",'0'},
        {0x53E530,A(&RenderMenus),"RenderMenus",'c',"",'0'},
        {0x53D5B0,A(&CCredits::Render),"Credits::Render",'c',"",'0'},
        {0x746060,A(&IsForegroundApp),"IsForegroundApp",'c',"",'1'},
        // estado de render: thunk instalado en RwEngineInstance->dOpenDevice.fpRenderStateSet
        {0,0,"RwRenderStateSet",'c',"ii",'1'},
    };
    return s;
}

// --- registro fijo (sin reservas) ---
struct Rec { uint16 stub; uint8 n; uint8 pad; uint32 v[8]; };
inline std::array<Rec,2048> recs{}; inline size_t nrec{}; inline bool overflow{};

// --- fixtures de los stubs ---
inline uint64 boolMask{}; inline uint32 boolCalls{};
inline uint32 cycBase{}, cycStep{1}, cycCalls{}, cpms{1};
inline uint32 fadeStatus{};
inline uint8 floatRet{}; inline float floatVal{1.25f};

inline DWORD mainTid{};
inline uint32 __cdecl Dispatch(uint32 idx,uint32 ecx,const uint32* args) {
    const auto& s=Stubs()[idx];
    // Otros hilos (p. ej. CAEStreamThread llama a IsForegroundApp en su bucle) no se trazan ni consumen fixtures:
    // reciben el valor real de ForegroundApp, el primer argumento o 0/1 según el retorno del stub.
    if(GetCurrentThreadId()!=mainTid){
        if(s.exe==0x746060)return uint32(*reinterpret_cast<const uint32*>(0x8D621C)!=0);
        switch(s.ret){ case '1': return 1; case 'C': return args[0]; case 'M': return cpms; default: return 0; }
    }
    Rec r{}; r.stub=uint16(idx);
    auto put=[&](uint32 v){ if(r.n<8)r.v[r.n++]=v; else overflow=true; };
    if(s.conv=='t')put(ecx);
    for(size_t k=0;s.kinds[k];++k){
        const uint32 a=args[k];
        switch(s.kinds[k]){
        case 'i': put(a); break;
        case 'b': put(a&0xFFu); break;
        case 'h': put(a&0xFFFFu); break;
        case 'p': { auto* p=reinterpret_cast<const uint32*>(uintptr_t(a)); put(p ? p[0] : 0xFFFFFFFFu); put(p ? p[1] : 0xFFFFFFFFu); break; }
        default: break;
        }
    }
    if(nrec<recs.size())recs[nrec++]=r; else overflow=true;
    switch(s.ret){
    case '1': return 1;
    case 'C': return args[0];
    case 'T': { const uint32 k=cycCalls++; if(k>600){ overflow=true; return 0xFFFFFFFFu; } return cycBase+k*cycStep; }
    case 'M': return cpms;
    case 'B': return uint32((boolMask>>(boolCalls++&63))&1);
    case 'S': return fadeStatus;
    case 'f': floatRet=1; return 0;
    default: return 0;
    }
}
// Entrada común de los thunks: [esp]=vuelta al thunk, [esp+4]=índice, [esp+8]=vuelta al llamador, [esp+12]=argumentos.
// Conserva ECX/EDX; si el stub devuelve float lo deja en ST0. El thunk termina con "ret N" (thiscall) o "ret" (cdecl).
__declspec(naked) inline void Common() {
    __asm {
        push ebp
        mov ebp, esp
        push ecx
        push edx
        lea eax, [ebp+16]
        push eax
        push ecx
        push dword ptr [ebp+8]
        call Dispatch
        add esp, 12
        cmp byte ptr [floatRet], 0
        je noFloatResult
        mov byte ptr [floatRet], 0
        fld dword ptr [floatVal]
    noFloatResult:
        pop edx
        pop ecx
        pop ebp
        ret 4
    }
}
inline uint8* thunks{};
inline void* Thunk(size_t i){ return thunks+i*16; }
inline bool BuildThunks() {
    const auto& s=Stubs();
    thunks=static_cast<uint8*>(VirtualAlloc(nullptr,s.size()*16,MEM_COMMIT|MEM_RESERVE,PAGE_EXECUTE_READWRITE));
    if(!thunks)return false;
    const auto common=reinterpret_cast<uintptr_t>(&Common);
    for(size_t i=0;i<s.size();++i){
        uint8* t=thunks+i*16; const uint32 idx=uint32(i);
        t[0]=0x68; std::memcpy(t+1,&idx,4);
        t[5]=0xE8; const int32 rel=int32(common-(uintptr_t(t)+10)); std::memcpy(t+6,&rel,4);
        const uint16 n=s[i].conv=='t' ? uint16(4*std::strlen(s[i].kinds)) : 0;
        if(n){ t[10]=0xC2; std::memcpy(t+11,&n,2); } else t[10]=0xC3;
    }
    FlushInstructionCache(GetCurrentProcess(),thunks,s.size()*16);
    return true;
}
// Cuerpos de la DLL resueltos ANTES de escribir ningún parche; se rechazan direcciones repetidas o que sean thunks.
inline json ResolveStubs() {
    json bad=json::array(); auto& s=Stubs();
    for(auto& t:s)if(t.dll)t.dll=Body(t.dll);
    const uintptr_t lo=uintptr_t(thunks), hi=lo+s.size()*16;
    for(auto& t:s){
        if(t.dll&&InExe(t.dll))bad.push_back(std::string(t.name)+": cuerpo de la DLL dentro del EXE");
        if((t.dll>=lo&&t.dll<hi)||(t.exe>=lo&&t.exe<hi))bad.push_back(std::string(t.name)+": dirección de un thunk");
        if(t.exe==0x53E920||t.exe==0x53E770)bad.push_back(std::string(t.name)+": función probada");
        for(auto& o:s)if(&o!=&t&&((t.dll&&(o.dll==t.dll||o.exe==t.dll))||(t.exe&&o.exe==t.exe)))bad.push_back(std::string(t.name)+": dirección repetida");
    }
    return bad;
}
inline json Trace() {
    json t=json::array();
    for(size_t i=0;i<nrec;++i){
        const auto& r=recs[i]; json e=json::array({Stubs()[r.stub].name});
        for(int k=0;k<r.n;++k)e.push_back(r.v[k]);
        t.push_back(std::move(e));
    }
    if(overflow)t.push_back("overflow");
    return t;
}

// --- objetos falsos ---
alignas(16) inline std::array<uint8,0x400> fakeCam{};
alignas(16) inline std::array<uint8,0x100> fakeWorld{};
struct FakeLink { uint32 data; FakeLink* prev; FakeLink* next; };
alignas(16) inline std::array<FakeLink,8> fakeLinks{};
inline constexpr uintptr_t LIST=0xC88224; // CVisibilityPlugins::ms_weaponPedsForPC (CLinkList<CPed*>, 0x34 bytes)

// --- rangos globales guardados, fijados por caso, comparados por hash y restaurados ---
// (ForegroundApp 0x8D621C se fija a 1 solo durante la llamada: WndProc puede cambiarlo entre casos)
struct Range { uintptr_t a; size_t n; };
inline const std::array<Range,17> ranges{{
    {0xBA677B,1},{0xBA67A4,1},{0xC17050,4},{0x8D5038,4},{0xC3EFA4,4},{0xC17038,8},{0xC6E97C,1},{0xB72CA8,4},{0xC3F0D0,4},
    {0xC17044,8},{0xC812CC,1},{0xB7C4C4,12},{0xB7C4F0,8},{0xB6F0B8,4},{0xB6F16C,4},{LIST,0x34},{0xB72CA0,4},
}};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,2> fns{{
    {"Game","Idle",0x53E920,0,-1,1024},{"Game","FrontendIdle",0x53E770,0,-1,1024},
}};

inline void Run() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"hookunlockgame-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"hookunlockgame-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("hookunlockgame");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"hookunlockgame-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    if(!BuildThunks()){Write({{"event","error"},{"reason","VirtualAlloc"}});return;}
    if(const auto bad=ResolveStubs(); !bad.empty()){Write({{"event","error"},{"reason","stubs"},{"details",bad}});return;}
    const auto& stubs=Stubs();
    size_t stateIdx=0; for(size_t k=0;k<stubs.size();++k)if(std::strcmp(stubs[k].name,"RwRenderStateSet")==0)stateIdx=k;
    std::array<bool,2> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    std::vector<std::vector<uint8>> saved;
    for(auto& r:ranges){ auto* p=reinterpret_cast<uint8*>(r.a); saved.emplace_back(p,p+r.n); }
    auto restore=[&]{ for(size_t k=0;k<ranges.size();++k)std::memcpy(reinterpret_cast<void*>(ranges[k].a),saved[k].data(),ranges[k].n); };
    auto B=[](uintptr_t a)->uint8&{ return *reinterpret_cast<uint8*>(a); };
    auto D=[](uintptr_t a)->uint32&{ return *reinterpret_cast<uint32*>(a); };
    auto Fl=[](uintptr_t a)->float&{ return *reinterpret_cast<float*>(a); };

    bool passed=true; int pair=0;
    using ReversingProbe::EntryPatch;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto run=[&](bool candidate)->json {
                restore();
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1D1Eu; if(!stt)stt=1;
                // fixtures de los stubs
                boolMask=0; for(int b=0;b<64;++b)if(next()%8)boolMask|=1ull<<b;
                boolCalls=cycCalls=0; floatRet=0;
                cpms=1000+next()%3000;
                const uint32 lastTick=100+next()%900000;
                const int32 d0=int32(next()%24)-8;                       // <0: la resta sin signo da la vuelta y sale enseguida
                cycBase=uint32(int32(lastTick)+d0)*cpms+next()%cpms; cycStep=cpms/4+next()%(cpms*2);
                switch(next()%4){ case 0: case 1: fadeStatus=2; break; case 2: fadeStatus=next()%4; break; default: fadeStatus=next()%6==0 ? next() : next()%3; }
                // globales
                B(0xBA677B)=uint8(next()%3==0); B(0xBA67A4)=uint8(next()%3==0);
                D(0xC17050)=next()%5==0 ? (next()%2 ? 1u : next()) : 0u;                                   // RsGlobal.quit
                Fl(0x8D5038)=next()%4==0 ? 70.f : uni(20.f,120.f); Fl(0xC3EFA4)=next()%4==0 ? 4.f/3.f : uni(1.f,2.4f);
                for(auto& b:fakeCam)b=uint8(next()); for(auto& b:fakeWorld)b=uint8(next());
                D(0xC1703C)=uint32(uintptr_t(fakeCam.data())); D(0xC17038)=uint32(uintptr_t(fakeWorld.data()));
                B(0xC6E97C)=uint8(next()%3==0);                                                               // bCreditsGoing
                D(0xB72CA8)=lastTick; D(0xC3F0D0)=next();
                D(0xC17044)=next()%8==0 ? next()%4 : 320+next()%3000; D(0xC17048)=next()%8==0 ? next()%4 : 240+next()%2000;
                B(0xC812CC)=uint8(next()%4==0);                                                               // LightningFlash
                for(int k=0;k<6;++k)*reinterpret_cast<uint16*>(0xB7C4C4+2*k)=uint16(next()%4 ? next()%256 : next());
                Fl(0xB7C4F0)=uni(10.f,3000.f); Fl(0xB7C4F4)=uni(-100.f,1000.f);
                D(0xB6F0B8)=next()%3==0 ? next()%9 : (next()%2 ? 0u : 2u);                                   // m_nBlurType
                switch(next()%5){ case 0: Fl(0xB6F16C)=0.f; break; case 1: Fl(0xB6F16C)=-uni(0.f,5.f); break; case 2: D(0xB6F16C)=0x7FC00000u; break; default: Fl(0xB6F16C)=uni(0.001f,60.f); }
                // lista falsa en el lugar de ms_weaponPedsForPC (cabecera en 0xC88224, eslabones propios)
                {
                    auto* L=reinterpret_cast<FakeLink*>(LIST); FakeLink* usedHead=L; FakeLink* usedTail=L+1; FakeLink* freeHead=L+2; FakeLink* freeTail=L+3;
                    for(int k=0;k<4;++k)L[k].data=next();
                    usedHead->next=usedTail; usedTail->prev=usedHead; freeHead->next=freeTail; freeTail->prev=freeHead;
                    usedHead->prev=nullptr; usedTail->next=nullptr; freeHead->prev=nullptr; freeTail->next=nullptr;
                    *reinterpret_cast<FakeLink**>(LIST+0x30)=fakeLinks.data();
                    for(auto& l:fakeLinks){
                        l.data=next(); FakeLink* after=next()%2 ? usedHead : freeHead;
                        l.next=after->next; l.next->prev=&l; l.prev=after; after->next=&l;
                    }
                }
                const uint32 param=next()%5 ? 1+next()%0x7FFF : 0u;
                // Fuera del dominio del EXE: el EXE solo llama a Idle con ForegroundApp != 0.
                // Se exige ForegroundApp = 1 durante la llamada (más abajo).
                nrec=0; overflow=false;
                bool fgWasOne=false;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                {
                    std::vector<std::unique_ptr<EntryPatch>> P; P.reserve(2*stubs.size());
                    for(size_t k=0;k<stubs.size();++k){ const auto& s=stubs[k]; if(s.exe)P.push_back(std::make_unique<EntryPatch>(s.exe,Thunk(k))); if(s.dll)P.push_back(std::make_unique<EntryPatch>(s.dll,Thunk(k))); }
                    auto& fpState=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedFp=fpState;
                    fpState=reinterpret_cast<RwRenderStateSetFunction>(Thunk(stateIdx));
                    const uint32 fg=D(0x8D621C); D(0x8D621C)=1; fgWasOne=D(0x8D621C)==1;                      // ForegroundApp: siempre 1 durante la llamada
                    if(f==0)reinterpret_cast<void(__cdecl*)(void*)>(fn.address)(reinterpret_cast<void*>(uintptr_t(param)));
                    else reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    D(0x8D621C)=fg;
                    fpState=savedFp;
                    for(size_t k=P.size();k-->0;)P[k].reset();
                }
                const bool notsaOk=fgWasOne;
                json out={{"switches",switches},{"trace",Trace()}};
                json st=json::array(); for(auto& r:ranges)st.push_back(Fnv(reinterpret_cast<void*>(r.a),r.n)); out["state"]=st;
                out["cam"]=Fnv(fakeCam.data(),fakeCam.size()); out["links"]=Fnv(fakeLinks.data(),sizeof(fakeLinks));
                out["notsa"]={{"ok",notsaOk},{"foreground",fgWasOne}};
                restore();
                return out;
            };
            auto original=run(false), candidate=run(true);
            const bool notsaOk=original["notsa"]["ok"]==true&&candidate["notsa"]["ok"]==true;
            const json notsaCand=candidate["notsa"]; original.erase("notsa"); candidate.erase("notsa");
            const bool ok=notsaOk&&original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k); if(!notsaOk)diff.push_back("notsa");
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"notsa",notsaCand},
                       {"original",original},{"candidate",candidate}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restore();
    bool restored=true; for(size_t k=0;k<ranges.size();++k)restored&=std::memcmp(reinterpret_cast<void*>(ranges[k].a),saved[k].data(),ranges[k].n)==0;
    restored&=ReversingProbe::JumpsToDll(0x53E920)==enabled[0]&&ReversingProbe::JumpsToDll(0x53E770)==enabled[1];
    Write({{"event",passed ? "idle_pairs_complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
