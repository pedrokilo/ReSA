#pragma once
// Banco dentro del motor (flujo viejo, simple): CPad::UpdateMouse (EXE 0x53F3C0, GTA SA 1.0 US).
// Original (hook desactivado) frente a candidato sobre un CPad falso y el estado global del raton
// (0xB73404 temp, 0xB73418 nuevo, 0xB7342C viejo), con 9216 casos deterministas.
// Colaboradores simulados y trazados con argumentos: IsForegroundApp (0x746060), IsVideoModeExclusive (0x745CA0),
// diMouseInit (0x7469A0, hook bloqueado: se parchea la direccion del EXE y el simbolo de la DLL con EntryPatch) y
// GetMouseState (0x746ED0: cdecl(buffer) en el EXE, miembro thiscall en la DLL: dos stubs distintos con la misma traza).
// CheckForInput (0x53F270) y SetTouched se ejecutan de verdad. Sin COM ni dispositivo real: el puntero de dispositivo
// (0xC8CFA4) es un token opaco que nadie desreferencia. Todo el estado global tocado se guarda y se restaura.
#include <fstream>
#include <float.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Pad.h"
#include "MouseControllerState.h"
#include "WinInput.h"
#include "../ProbeRuntime.hpp"

bool IsForegroundApp();
bool IsVideoModeExclusive();

namespace PadMouse1Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const std::string& s,uint64 h=1469598103934665603ull){ for(unsigned char c:s){h^=c;h*=1099511628211ull;} return h; }

static_assert(sizeof(CPad)==308 && sizeof(CMouseControllerState)==20 && sizeof(DIMOUSESTATE2)==20);
static_assert(offsetof(CPad,LastTimeTouched)==0x120);

// --- estado de la ejecucion actual ---
inline unsigned route{}, profile{}; inline bool valid=true;
// Hilo del banco. Otros hilos del juego llaman a los mismos colaboradores parcheados (p. ej. AEStreamThread llama a
// IsForegroundApp 0x746060): sus llamadas no se trazan ni tocan el estado compartido (carrera sobre 'trace' = traza
// espuria y corrupcion de memoria en push_back).
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }
alignas(16) inline std::array<uint8,16+308+16> padBuf{};     // CPad falso con guardas de 16 bytes
alignas(16) inline std::array<uint8,48> opaque{};            // token de dispositivo (no se desreferencia)
inline CPad* FakePad(){ return reinterpret_cast<CPad*>(padBuf.data()+16); }
inline uintptr_t Token(){ return uintptr_t(opaque.data()+16); }
inline uintptr_t& Device(){ return *reinterpret_cast<uintptr_t*>(0xC8CFA4); }   // PSGLOBAL(diMouse)

// Los tres estados del raton (60 bytes) con el relleno enmascarado: CMouseControllerState mide 0x14 y su byte 7 (tras
// isMouseSecondXPressed) es relleno que el EXE nunca escribe pero copia con rep movsd (5 dwords), y que el compilador
// no copia al hacer la asignacion miembro a miembro. Se pone a 0 en los bytes 7, 27 y 47 (temp/nuevo/viejo).
inline std::vector<uint8> MouseBytes(){ auto b=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73404),60); for(size_t k:{7u,27u,47u})b[k]=0; return b; }

// 12 estados de DIMOUSESTATE2 (sin INT_MIN; los flotantes salen de enteros: nunca NaN/Inf)
inline const std::array<DIMOUSESTATE2,12> states{{
    {0,0,0,{0,0,0,0,0,0,0,0}},{1,-1,0,{0,0,0,0,0,0,0,0}},{-1,1,0,{0,0,0,0,0,0,0,0}},
    {0,0,1,{0,0,0,0,0,0,0,0}},{0,0,-1,{0,0,0,0,0,0,0,0}},
    {0,0,0,{128,0,0,0,0,1,127,255}},{0,0,0,{0,255,0,0,0,128,0,0}},{0,0,0,{0,0,129,0,0,0,0,0}},{0,0,0,{0,0,0,128,255,0,0,0}},
    {INT_MAX,-INT_MAX,INT_MAX,{1,127,128,129,255,0,0,0}},{16777217,-16777217,1,{255,255,255,255,255,255,255,255}},{-16777219,16777219,-INT_MAX,{127,127,127,127,127,127,127,127}}
}};

// --- colaboradores simulados ---
inline bool __cdecl Foreground(){ if(!Mine())return *reinterpret_cast<uint32*>(0x8D621C)!=0; const bool r=route!=0; trace.push_back({"foreground",r}); return r; }
inline bool __cdecl Exclusive(){ if(!Mine())return false; const bool r=((profile+route)&1)!=0; trace.push_back({"exclusive",r}); return r; }
// El EXE empuja un dword cuyo byte alto es basura de pila: solo cuenta el byte bajo.
inline void __cdecl Init(uint32 raw){ if(!Mine())return; trace.push_back({"init",raw&0xFF}); Device()= route==1 ? 0 : Token(); }
inline HRESULT Read(DIMOUSESTATE2* st){
    if(!Mine())return HRESULT(0x80004005L);
    const auto* tib=reinterpret_cast<const NT_TIB*>(NtCurrentTeb());
    const uintptr_t lo=uintptr_t(tib->StackLimit), hi=uintptr_t(tib->StackBase), p=uintptr_t(st);
    const bool inStack=p>=lo && p<=hi-20;
    valid&=st && inStack && Device()==Token();
    if(st&&inStack)std::memcpy(st,&states[profile],20);
    const HRESULT status= route==2 ? HRESULT(0x80004005L) : route==5 ? HRESULT(0x80070005L) : route==4 ? HRESULT(1) : HRESULT(0);
    trace.push_back({"read",int32(status),ReversingProbe::Bytes(&states[profile],20),valid});
    return status;
}
inline HRESULT __cdecl ReadExe(DIMOUSESTATE2* st){ return Read(st); }                                            // 0x746ED0: cdecl(buffer)
inline HRESULT __fastcall ReadDll(CPad* self,void*,DIMOUSESTATE2* st){ if(Mine())valid&=self==FakePad(); return Read(st); } // miembro: thiscall(self,buffer)

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Sigue la cadena de saltos E9 (thunk incremental -> cuerpo).
inline uintptr_t Body(uintptr_t a){ for(int j=0;j<16&&*reinterpret_cast<uint8*>(a)==0xE9;++j)a+=5+*reinterpret_cast<int32*>(a+1); return a; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,1> fns{{
    {"CPad","UpdateMouse",0x53F3C0,0,-1,9216},
}};

// globales tocados: {direccion, bytes}
inline const std::array<std::pair<uintptr_t,size_t>,7> globals{{
    {0xB733F4,0x64},   // guarda (16) + temp/nuevo/viejo estado del raton (60) + guarda (24)
    {0xC8CFA4,4},      // dispositivo
    {0xBA67A4,1},      // m_bMenuActive
    {0xBA6744,2},      // bInvertMouseX/Y
    {0xB7CB84,4},      // reloj del juego
    {0xB73458,616},    // CPad globales (inmutables)
    {0x8D621C,4},      // ForegroundApp
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"padmouse1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"padmouse1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("padmouse1");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"padmouse1-only.txt"); if(f)std::getline(f,only); }
    auto* hook=FindHook(fns[0].cls,fns[0].name);
    if(!hook){Write({{"event","error"},{"reason","hook not found"},{"function","CPad::UpdateMouse"}});return;}
    const bool wasEnabled=hook->Hooked(), wasLocked=hook->Locked(); hook->LockState(false);

    // Direcciones de la DLL (entrada incremental y cuerpo) y stubs que las sustituyen.
    static_assert(sizeof(&CPad::GetMouseState)==sizeof(uintptr_t));
    struct Site { const char* name; uintptr_t exeAddr, dll, dllBody; uintptr_t exeStub, dllStub; };
    const uintptr_t dllFg=std::bit_cast<uintptr_t>(&IsForegroundApp), dllEx=std::bit_cast<uintptr_t>(&IsVideoModeExclusive);
    const uintptr_t dllInit=std::bit_cast<uintptr_t>(static_cast<void(*)(bool)>(&WinInput::diMouseInit));
    const uintptr_t dllRead=std::bit_cast<uintptr_t>(&CPad::GetMouseState);
    const std::array<Site,4> sites{{
        {"IsForegroundApp",0x746060,dllFg,Body(dllFg),reinterpret_cast<uintptr_t>(&Foreground),reinterpret_cast<uintptr_t>(&Foreground)},
        {"IsVideoModeExclusive",0x745CA0,dllEx,Body(dllEx),reinterpret_cast<uintptr_t>(&Exclusive),reinterpret_cast<uintptr_t>(&Exclusive)},
        {"diMouseInit",0x7469A0,dllInit,Body(dllInit),reinterpret_cast<uintptr_t>(&Init),reinterpret_cast<uintptr_t>(&Init)},
        {"GetMouseState",0x746ED0,dllRead,Body(dllRead),reinterpret_cast<uintptr_t>(&ReadExe),reinterpret_cast<uintptr_t>(&ReadDll)}}};
    bool known=true; json siteInfo=json::array();
    for(auto& s:sites){ const bool ok=s.exeAddr>=0x401000&&s.exeAddr<0x1000000&&s.dll>=0x1000000&&s.dllBody>=0x1000000; known&=ok;
        siteInfo.push_back({{"name",s.name},{"exe",s.exeAddr},{"dll",s.dll},{"body",s.dllBody},{"ok",ok}}); }
    // bytes iniciales de todas las entradas parcheables, para comprobar la restauracion
    std::vector<std::pair<uintptr_t,std::vector<uint8>>> entryBefore;
    for(auto& s:sites)for(auto a:{s.exeAddr,s.dll,s.dllBody})entryBefore.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),8)});
    entryBefore.push_back({0x53F3C0,ReversingProbe::Bytes(reinterpret_cast<void*>(0x53F3C0),8)});
    const auto checkBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0x53F270),16);
    std::vector<std::vector<uint8>> saved; for(auto [p,n]:globals)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(p),n));
    const unsigned savedFP=_controlfp(0,0);
    Write({{"event","begin"},{"sites",siteInfo},{"known",known},{"fp_control",savedFP},{"pad",uintptr_t(FakePad())},{"hook_enabled",wasEnabled},{"hook_locked",wasLocked}});

    bool passed=known; int pair=0;
    if(known) {
        using ReversingProbe::EntryPatch;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto put=[&](uintptr_t at,uintptr_t stub){ patches.push_back(std::make_unique<EntryPatch>(at,reinterpret_cast<void*>(stub))); };
        for(auto& s:sites){ put(s.exeAddr,s.exeStub); put(s.dll,s.dllStub); if(s.dllBody!=s.dll)put(s.dllBody,s.dllStub); }

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                const unsigned I=unsigned(i);
                const unsigned prof=I%12, rt=(I/12)%6, flags=(I/72)%8, pidx=(I/576)%4, devStart=(I/2304)%2, clk=(I/4608)%2;
                const uint8 poison=uint8(pidx*85); const uint32 clock= clk ? 0xFFFFFFFFu : 0x1234ABCDu;
                auto run=[&](bool candidate)->json {
                    profile=prof; route=rt; valid=true; trace=json::array();
                    padBuf.fill(0xCD); opaque.fill(0xA7);
                    for(unsigned k=0;k<308;++k)padBuf[16+k]=uint8(poison+k*7+(I&0xFF));
                    uint8 seeds[60]; for(unsigned j=0;j<60;++j)seeds[j]=uint8(poison+j*17);
                    for(unsigned s=0;s<3;++s){ seeds[s*20+3]=uint8(pidx%2); seeds[s*20+4]=uint8((pidx+1)%2);   // bool canonicos
                        const float v= poison ? float(poison)*float(s+1)*(s==1?-0.5f:1.f) : 0.f; for(unsigned off:{8u,12u,16u})std::memcpy(seeds+s*20+off,&v,4); }
                    std::memcpy(reinterpret_cast<void*>(0xB73404),seeds,60);
                    Device()= devStart ? Token() : 0;
                    *reinterpret_cast<uint8*>(0xBA67A4)=uint8(flags&1); *reinterpret_cast<uint8*>(0xBA6744)=uint8((flags>>1)&1); *reinterpret_cast<uint8*>(0xBA6745)=uint8((flags>>2)&1);
                    std::memcpy(reinterpret_cast<void*>(0xB7CB84),&clock,4);
                    const auto guardsBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB733F4),16), guardsAfter=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73440),24);
                    const auto globalPads=ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73458),616);
                    hook->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                    _controlfp(_PC_24,_MCW_PC);                                               // x87 a 24 bits como el juego
                    reinterpret_cast<void(__thiscall*)(CPad*)>(fn.address)(FakePad());
                    const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                    json out={{"switches",switches},{"pad",padBuf},{"opaque",opaque},
                              {"mouse",MouseBytes()},
                              {"guards",json::array({guardsBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB733F4),16),guardsAfter==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73440),24)})},
                              {"global_pads",globalPads==ReversingProbe::Bytes(reinterpret_cast<void*>(0xB73458),616)},
                              {"device",Device()==Token() ? 1 : Device()==0 ? 0 : 2},
                              {"menu",*reinterpret_cast<uint8*>(0xBA67A4)},{"invert",ReversingProbe::Bytes(reinterpret_cast<void*>(0xBA6744),2)},
                              {"clock",*reinterpret_cast<uint32*>(0xB7CB84)},{"fp",fp},{"abi_valid",valid},{"trace",trace}};
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["abi_valid"]==true&&original["global_pads"]==true&&original["guards"]==json::array({true,true});
                passed&=ok; failures+=!ok;
                auto digest=[](const json& o){ return Fnv(o["pad"].dump(),Fnv(o["mouse"].dump())); };
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},
                             {"original",{{"trace",original["trace"]},{"state_fnv",digest(original)}}},{"candidate",{{"trace",candidate["trace"]},{"state_fnv",digest(candidate)}}},
                             {"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"fixture",{{"profile",prof},{"route",rt},{"flags",flags},{"poison",poison},{"dev_start",devStart},{"clock",clock}}},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        patches.clear();   // restaura las entradas
    }
    hook->State(wasEnabled); hook->LockState(wasLocked);
    for(size_t j=0;j<globals.size();++j)std::memcpy(reinterpret_cast<void*>(globals[j].first),saved[j].data(),saved[j].size());
    _controlfp(savedFP,_MCW_PC|_MCW_RC);
    bool restored=_controlfp(0,0)==savedFP && checkBefore==ReversingProbe::Bytes(reinterpret_cast<void*>(0x53F270),16);
    for(size_t j=0;j<globals.size();++j)restored&=saved[j]==ReversingProbe::Bytes(reinterpret_cast<void*>(globals[j].first),globals[j].second);
    for(auto& [a,b]:entryBefore)restored&=b==ReversingProbe::Bytes(reinterpret_cast<void*>(a),8);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
