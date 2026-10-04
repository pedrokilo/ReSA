#pragma once
// Banco dentro del motor: capa Rs del esqueleto de RenderWare (27 funciones de platform.cpp). Original (hooks
// de la clase desactivados) frente a candidato (activados) con fixtures deterministas.
// Nunca se toca la ventana, el dispositivo, la camara, la entrada ni el motor RW reales:
// - Las funciones Ps*, AppEventHandler (0x53EC10) y RwEngineInit/Open/Start/Stop/Close/Term,
//   RwTextureSetMipmapping/SetAutoMipmapping y psGetMemoryFunctions se sustituyen con EntryPatch solo durante la
//   llamada: en la direccion del EXE y en el cuerpo de la DLL (los envoltorios RW de la DLL llaman al EXE).
//   Los cuerpos de la DLL se resuelven una sola vez antes de escribir ningun parche (siguiendo el ILT) y se
//   comprueba que no coinciden con una direccion del EXE ni con un sustituto.
// - Sustitutos sin reservas de memoria: la traza va a un array fijo y se convierte a json despues de la llamada.
// - Los manejadores de teclado/raton/mando de RsGlobal apuntan a un manejador falso con traza.
// Se guardan y restauran en cada ejecucion y al final: RsGlobal (0xC17040, 0x3C bytes) y DefaultVM (0x8D2E34).
// rsCommandLine y rsPreInitCommandLine reciben el argumento en eax (convencion del EXE); se llaman con asm.
#include <fstream>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <platform/platform.h>
#include <app/app.h>
#include "../ProbeRuntime.hpp"

namespace Rs27Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }

// ---------------------------------------------------------------------------------------------------
// Traza sin reservas
// ---------------------------------------------------------------------------------------------------
enum EvId : uint8 { E_TIMER, E_SETTEXT, E_PATHSEP, E_CAMBEGIN, E_ONTOP, E_MOUSEVIS, E_MOUSEPOS, E_SELECT, E_PATHDESTROY,
    E_PATHCREATE, E_PSTERM, E_PSINIT, E_GRAB, E_ERRMSG, E_WARNMSG, E_MEMFUNCS, E_ENGINIT, E_APPEVENT, E_INSTALLFS,
    E_ENGOPEN, E_ENGSTART, E_ENGSTOP, E_ENGCLOSE, E_ENGTERM, E_NATIVETEX, E_MIP, E_AUTOMIP, E_HANDLER, E_COUNT };
inline constexpr const char* evNames[E_COUNT]{"ps_timer","ps_set_text","ps_path_sep","ps_cam_begin","ps_on_top","ps_mouse_vis",
    "ps_mouse_pos","ps_select_device","ps_path_destroy","ps_path_create","ps_terminate","ps_initialize","ps_grab","ps_error",
    "ps_warning","ps_mem_funcs","rw_engine_init","app_event","ps_install_fs","rw_engine_open","rw_engine_start","rw_engine_stop",
    "rw_engine_close","rw_engine_term","ps_native_tex","rw_mipmapping","rw_auto_mipmapping","input_handler"};
struct Ev { EvId id; uint32 a, b, c; };
inline std::array<Ev,64> evs{}; inline uint32 nEv{}; inline bool overflow{};
inline void Push(EvId id,uint32 a=0,uint32 b=0,uint32 c=0) { if(nEv<evs.size())evs[nEv++]={id,a,b,c}; else overflow=true; }
inline json TraceJson() { json t=json::array(); for(uint32 k=0;k<nEv;++k)t.push_back({evNames[evs[k].id],evs[k].a,evs[k].b,evs[k].c}); if(overflow)t.push_back("overflow"); return t; }

// --- fixtures (iguales en ambas ejecuciones) ---
struct Fixture { uint32 timer; char sep; int32 ret1, select, init, open, start, handler; std::array<int32,8> app; char* pathRet; RwImage* image; };
inline Fixture fx{}; inline uint32 nApp{}; inline bool derefParam{};
alignas(16) inline std::array<uint8,0x40> fakeMem{};

inline RwUInt32 __cdecl XTimer() { Push(E_TIMER); return fx.timer; }
inline void __cdecl XSetText(const char* s) { Push(E_SETTEXT,U(s)); }
inline char __cdecl XPathSep() { Push(E_PATHSEP); return fx.sep; }
inline RwBool __cdecl XCamBegin(RwCamera* c) { Push(E_CAMBEGIN,U(c)); return fx.ret1; }
inline RwBool __cdecl XOnTop(RwBool v) { Push(E_ONTOP,uint32(v)); return fx.ret1; }
inline void __cdecl XMouseVis(RwBool v) { Push(E_MOUSEVIS,uint32(v)); }
inline void __cdecl XMousePos(RwV2d* p) { Push(E_MOUSEPOS,U(p)); }
inline RwBool __cdecl XSelect() { Push(E_SELECT); return fx.select; }
inline void __cdecl XPathDestroy(char* p) { Push(E_PATHDESTROY,U(p)); }
inline char* __cdecl XPathCreate(const char* p) { Push(E_PATHCREATE,U(p)); return fx.pathRet; }
inline void __cdecl XPsTerm() { Push(E_PSTERM); }
inline RwBool __cdecl XPsInit() { Push(E_PSINIT); return fx.ret1; }
inline RwImage* __cdecl XGrab(RwCamera* c) { Push(E_GRAB,U(c)); return fx.image; }
inline void __cdecl XErr(const char* s) { Push(E_ERRMSG,U(s)); }
inline void __cdecl XWarn(const char* s) { Push(E_WARNMSG,U(s)); }
inline RwMemoryFunctions* __cdecl XMemFuncs() { Push(E_MEMFUNCS); return reinterpret_cast<RwMemoryFunctions*>(fakeMem.data()); }
inline RwBool __cdecl XEngInit(const RwMemoryFunctions* m,RwUInt32 f,RwUInt32 arena) { Push(E_ENGINIT,U(m),f,arena); return fx.init; }
inline RsEventStatus __cdecl XApp(RsEvent e,void* p) { Push(E_APPEVENT,uint32(e),U(p)); return RsEventStatus(fx.app[nApp++%8]); }
inline RwBool __cdecl XInstallFS() { Push(E_INSTALLFS); return fx.ret1; }
inline RwBool __cdecl XEngOpen(RwEngineOpenParams* p) { Push(E_ENGOPEN,U(p->displayID)); return fx.open; }
inline RwBool __cdecl XEngStart() { Push(E_ENGSTART); return fx.start; }
inline RwBool __cdecl XEngStop() { Push(E_ENGSTOP); return 1; }
inline RwBool __cdecl XEngClose() { Push(E_ENGCLOSE); return 1; }
inline RwBool __cdecl XEngTerm() { Push(E_ENGTERM); return 1; }
inline RwBool __cdecl XNativeTex() { Push(E_NATIVETEX); return fx.ret1; }
inline RwBool __cdecl XMip(RwBool v) { Push(E_MIP,uint32(v)); return 1; }
inline RwBool __cdecl XAutoMip(RwBool v) { Push(E_AUTOMIP,uint32(v)); return 1; }
// manejador de entrada falso (RsGlobal.keyboard/mouse/pad); en Rs*Message el parametro es un dword de la pila
inline RsEventStatus __cdecl XHandler(RsEvent e,void* p) { Push(E_HANDLER,uint32(e),derefParam ? *reinterpret_cast<uint32*>(p) : U(p)); return RsEventStatus(fx.handler); }

// ---------------------------------------------------------------------------------------------------
// Parches: EXE + cuerpo de la DLL (resuelto antes de escribir ningun parche)
// ---------------------------------------------------------------------------------------------------
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }
inline uintptr_t Body(uintptr_t a) {
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Target { const char* name; uintptr_t exe; uintptr_t dll; void* stub; };
inline constexpr size_t NTARGETS=27;
inline std::array<Target,NTARGETS> targets{};
inline void FillTargets() {
    auto V=[](auto f){ return reinterpret_cast<void*>(f); };
    targets={{
        {"psTimer",0x745270,Addr(&psTimer),V(&XTimer)},
        {"psWindowSetText",0x7451B0,Addr(&psWindowSetText),V(&XSetText)},
        {"psPathGetSeparator",0x745500,Addr(&psPathGetSeparator),V(&XPathSep)},
        {"psCameraBeginUpdate",0x745210,Addr(&psCameraBeginUpdate),V(&XCamBegin)},
        {"psAlwaysOnTop",0x7458B0,Addr(&psAlwaysOnTop),V(&XOnTop)},
        {"psMouseSetVisibility",0x7453E0,Addr(&psMouseSetVisibility),V(&XMouseVis)},
        {"psMouseSetPos",0x7453F0,Addr(&psMouseSetPos),V(&XMousePos)},
        {"psSelectDevice",0x746190,Addr(&psSelectDevice),V(&XSelect)},
        {"psPathnameDestroy",0x7454E0,Addr(&psPathnameDestroy),V(&XPathDestroy)},
        {"psPathnameCreate",0x745470,Addr(&psPathnameCreate),V(&XPathCreate)},
        {"psTerminate",0x7458A0,Addr(&psTerminate),V(&XPsTerm)},
        {"psInitialize",0x747420,Addr(&psInitialize),V(&XPsInit)},
        {"psGrabScreen",0x7452B0,Addr(&psGrabScreen),V(&XGrab)},
        {"psErrorMessage",0x7451D0,Addr(&psErrorMessage),V(&XErr)},
        {"psWarningMessage",0x7451F0,Addr(&psWarningMessage),V(&XWarn)},
        {"psInstallFileSystem",0x745520,Addr(&psInstallFileSystem),V(&XInstallFS)},
        {"psNativeTextureSupport",0x745530,Addr(&psNativeTextureSupport),V(&XNativeTex)},
        {"AppEventHandler",0x53EC10,Addr(&AppEventHandler),V(&XApp)},
        // envoltorios de la DLL que llaman al EXE: solo la direccion del EXE
        {"psGetMemoryFunctions",0x745510,0,V(&XMemFuncs)},
        {"RwEngineInit",0x7F3170,0,V(&XEngInit)},
        {"RwEngineOpen",0x7F2F70,0,V(&XEngOpen)},
        {"RwEngineStart",0x7F2E70,0,V(&XEngStart)},
        {"RwEngineStop",0x7F2E20,0,V(&XEngStop)},
        {"RwEngineClose",0x7F2F00,0,V(&XEngClose)},
        {"RwEngineTerm",0x7F3130,0,V(&XEngTerm)},
        {"RwTextureSetMipmapping",0x7F3530,0,V(&XMip)},
        {"RwTextureSetAutoMipmapping",0x7F3560,0,V(&XAutoMip)},
    }};
}
// Resuelve los cuerpos de la DLL (una vez, sin parches escritos) y valida las direcciones; devuelve los errores
inline json ResolveTargets() {
    json bad=json::array();
    for(auto& t:targets)if(t.dll)t.dll=Body(t.dll);
    for(auto& t:targets) {
        if(t.dll&&InExe(t.dll))bad.push_back(std::string(t.name)+": dll body in exe");
        for(auto& s:targets){ if(t.dll&&U(s.stub)==t.dll)bad.push_back(std::string(t.name)+": dll body is a stub"); if(U(s.stub)==t.exe)bad.push_back(std::string(t.name)+": exe is a stub"); }
        for(auto& o:targets)if(&o!=&t&&((t.dll&&(o.dll==t.dll||o.exe==t.dll))||o.exe==t.exe))bad.push_back(std::string(t.name)+": duplicate address");
    }
    // ningun cuerpo parcheado puede ser el de una funcion Rs probada (plegado de funciones identicas)
    const uintptr_t tested[]{Addr(&RsKeyFromScanCode),Addr(&RsTimer),Addr(&RsWindowSetText),Addr(&RsPathGetSeparator),Addr(&RsCameraBeginUpdate),
        Addr(&RsAlwaysOnTop),Addr(&RsRegisterImageLoader),Addr(&RsSetDebug),Addr(&RsMouseSetVisibility),Addr(&RsMouseSetPos),Addr(&RsSelectDevice),
        Addr(&RsInputDeviceAttach),Addr(&rsCommandLine),Addr(&rsPreInitCommandLine),Addr(&RsKeyboardEventHandler),Addr(&RsMouseEventHandler),
        Addr(&RsPadEventHandler),Addr(&RsPathnameDestroy),Addr(&RsPathnameCreate),Addr(&RsRwTerminate),Addr(&RsTerminate),Addr(&RsInitialize),
        Addr(&RsGrabScreen),Addr(&RsErrorMessage),Addr(&RsWarningMessage),Addr(&RsEventHandler),Addr(&RsRwInitialize)};
    for(auto& t:targets)for(const uintptr_t r:tested)if(t.dll&&(t.dll==r||t.dll==Body(r)))bad.push_back(std::string(t.name)+": dll body shared with a tested Rs function");
    return bad;
}
struct AllPatches {
    std::array<std::optional<ReversingProbe::EntryPatch>,2*NTARGETS> p;
    AllPatches() { for(size_t i=0;i<targets.size();++i){ p[2*i].emplace(targets[i].exe,targets[i].stub); if(targets[i].dll)p[2*i+1].emplace(targets[i].dll,targets[i].stub); } }
    ~AllPatches() { for(size_t i=p.size();i-->0;)p[i].reset(); }
};

// Llamada con el argumento en eax (rsCommandLine / rsPreInitCommandLine); devuelve eax y edx tras la llamada
inline uint32 CallEax(uintptr_t target,uint32 value,uint32& edxAfter) {
    uint32 r{}, e{};
    __asm {
        mov eax, value
        mov edx, 5A5A1234h
        call target
        mov r, eax
        mov e, edx
    }
    edxAfter=e; return r;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline constexpr size_t NFNS=27;
inline const std::array<Fn,NFNS> fns{{
    {"Rs","RsKeyFromScanCode",0x6193F0,0,-1,512},{"Rs","RsTimer",0x619410,0,-1,64},{"Rs","RsWindowSetText",0x619420,0,-1,64},
    {"Rs","RsPathGetSeparator",0x619430,0,-1,64},{"Rs","RsCameraBeginUpdate",0x619450,0,-1,256},{"Rs","RsAlwaysOnTop",0x619460,0,-1,256},
    {"Rs","RsRegisterImageLoader",0x619470,0,-1,64},{"Rs","RsSetDebug",0x619480,0,-1,64},{"Rs","RsMouseSetVisibility",0x619490,0,-1,128},
    {"Rs","RsMouseSetPos",0x6194A0,0,-1,64},{"Rs","RsSelectDevice",0x6194B0,0,-1,128},{"Rs","RsInputDeviceAttach",0x6194C0,0,-1,512},
    {"Rs","rsCommandLine",0x619510,0,-1,128},{"Rs","rsPreInitCommandLine",0x619530,0,-1,256},{"Rs","RsKeyboardEventHandler",0x619560,0,-1,256},
    {"Rs","RsMouseEventHandler",0x619580,0,-1,256},{"Rs","RsPadEventHandler",0x6195A0,0,-1,256},{"Rs","RsPathnameDestroy",0x6195C0,0,-1,64},
    {"Rs","RsPathnameCreate",0x6195D0,0,-1,64},{"Rs","RsRwTerminate",0x6195E0,0,-1,64},{"Rs","RsTerminate",0x6195F0,0,-1,64},
    {"Rs","RsInitialize",0x619600,0,-1,256},{"Rs","RsGrabScreen",0x619AF0,0,-1,64},{"Rs","RsErrorMessage",0x619B00,0,-1,256},
    {"Rs","RsWarningMessage",0x619B30,0,-1,256},{"Rs","RsEventHandler",0x619B60,0,-1,1024},{"Rs","RsRwInitialize",0x619C90,0,-1,1024},
}};

struct Snap { uintptr_t a; std::vector<uint8> b;
    void Restore() const { std::memcpy(reinterpret_cast<void*>(a),b.data(),b.size()); }
    bool Same() const { return std::memcmp(reinterpret_cast<const void*>(a),b.data(),b.size())==0; } };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"rs27-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"rs27-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("rs27");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"rs27-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    FillTargets();
    if(const auto bad=ResolveTargets(); !bad.empty()){Write({{"event","error"},{"reason","bad patch targets"},{"details",bad}});return;}

    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    std::vector<Snap> snaps; auto snap=[&](uintptr_t a,size_t n){ snaps.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)}); };
    snap(0xC17040,0x3C);  // RsGlobal completo (appName ... pad)
    snap(0x8D2E34,4);     // DefaultVM
    auto restoreAll=[&]{ for(auto& s:snaps)s.Restore(); };
    auto* rs=reinterpret_cast<uint8*>(0xC17040);
    auto& defaultVM=*reinterpret_cast<int32*>(0x8D2E34);
    alignas(16) static std::array<uint8,0x80> fakeCam{}, otherCam{}, fakeImage{}, posBuf{};
    static std::array<char,32> text{}, path{}, created{}, cmd{};

    bool passed=true; int pair=0;
    {
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto pick=[&](std::initializer_list<int32> l){ return *(l.begin()+next()%l.size()); };
                auto randStr=[&](std::array<char,32>& b,const char* cs){ const size_t n=next()%24; const size_t m=std::strlen(cs); for(size_t k=0;k<n;++k)b[k]=cs[next()%m]; b[n]=0; return b.data(); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2527u; if(!stt)stt=1;
                    nEv=0; overflow=false; nApp=0; derefParam=false;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate; // tras State()
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    // RsGlobal: todo aleatorio salvo ps (lo lee el capturador de fallos); manejadores -> falso
                    for(int k=0;k<0x3C;++k)if(k<0x14||k>=0x18)rs[k]=uint8(next());
                    for(int dev=0;dev<3;++dev){ *reinterpret_cast<void**>(rs+0x20+dev*0xC)=reinterpret_cast<void*>(&XHandler); *reinterpret_cast<int32*>(rs+0x1C+dev*0xC)=pick({0,1,1,0x100,-1,2}); }
                    defaultVM=pick({0,1,1,2,-1});
                    fx.timer=next(); fx.sep=char(pick({'\\','/',':',0,int32(next()%256)})); fx.ret1=pick({0,1,1,2,-1,0x100});
                    fx.select=pick({1,1,1,0,2,-1}); fx.init=pick({1,1,1,1,0,2}); fx.open=pick({1,1,1,1,0,3}); fx.start=pick({1,1,1,1,0,-1});
                    fx.handler=pick({0,1,2,2,7}); for(auto& v:fx.app)v=pick({2,2,2,2,2,0,1,3});
                    fx.pathRet=next()%5 ? created.data() : nullptr; fx.image=next()%4 ? reinterpret_cast<RwImage*>(fakeImage.data()) : nullptr;
                    auto anyPtr=[&]()->void*{ switch(next()%5){case 0:return nullptr;case 1:return fakeCam.data();case 2:return otherCam.data();case 3:return cmd.data();default:return reinterpret_cast<void*>(uintptr_t(0x12340000+next()%0x10000));} };

                    if(nm=="RsKeyFromScanCode") {
                        const uint32 scan=next()%256|(next()%3==0 ? next()<<8 : 0); const int32 shift=pick({0,1,0,-1,0x100,0x10000,int32(next())});
                        { AllPatches ap; ret=reinterpret_cast<uint32(__cdecl*)(uint32,int32)>(a)(scan,shift)&0xFF; }
                    }
                    else if(nm=="RsTimer") { AllPatches ap; ret=reinterpret_cast<uint32(__cdecl*)()>(a)(); }
                    else if(nm=="RsWindowSetText") { const char* s=next()%6 ? randStr(text,"abc XYZ:._") : nullptr; AllPatches ap; reinterpret_cast<void(__cdecl*)(const char*)>(a)(s); }
                    else if(nm=="RsPathGetSeparator") { AllPatches ap; ret=reinterpret_cast<uint32(__cdecl*)()>(a)()&0xFF; }
                    else if(nm=="RsCameraBeginUpdate") { void* c=anyPtr(); AllPatches ap; ret=reinterpret_cast<int32(__cdecl*)(void*)>(a)(c); }
                    else if(nm=="RsAlwaysOnTop") { const int32 v=pick({0,1,1,0,2,-1,0x100}); AllPatches ap; ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(v); }
                    else if(nm=="RsRegisterImageLoader"||nm=="RsSetDebug"||nm=="RsSelectDevice"||nm=="RsInitialize") {
                        AllPatches ap; ret=reinterpret_cast<int32(__cdecl*)()>(a)();
                    }
                    else if(nm=="RsMouseSetVisibility") { const int32 v=pick({0,1,1,0,2,-1,0x100,int32(next())}); AllPatches ap; reinterpret_cast<void(__cdecl*)(int32)>(a)(v); }
                    else if(nm=="RsMouseSetPos") { void* p=next()%6 ? posBuf.data() : nullptr; AllPatches ap; reinterpret_cast<void(__cdecl*)(void*)>(a)(p); }
                    else if(nm=="RsInputDeviceAttach") {
                        const int32 type=pick({0,1,2,0,1,2,3,-1,int32(next())}); void* h=anyPtr();
                        AllPatches ap; ret=reinterpret_cast<int32(__cdecl*)(int32,void*)>(a)(type,h);
                    }
                    else if(nm=="rsCommandLine") {
                        void* p=anyPtr(); uint32 edx{};
                        { AllPatches ap; ret=CallEax(a,U(p),edx); } // el EXE no conserva edx aqui
                    }
                    else if(nm=="rsPreInitCommandLine") {
                        static const char* const fixed[]{"-vms","-vm","-vmsx","","-VMS","-v","x-vms","-vms ","-vms","-vms"};
                        const char* s=next()%4 ? fixed[next()%10] : randStr(cmd,"-vms");
                        if(s!=cmd.data()){ std::strcpy(cmd.data(),s); }
                        uint32 edx{};
                        { AllPatches ap; ret=CallEax(a,U(cmd.data()),edx); }
                        out["edx"]=edx; out["arg"]=std::string(cmd.data());
                    }
                    else if(nm=="RsKeyboardEventHandler"||nm=="RsMouseEventHandler"||nm=="RsPadEventHandler") {
                        const int32 e=pick({28,29,5,6,7,31,32,33,0,int32(next()%40)}); void* p=anyPtr();
                        AllPatches ap; ret=reinterpret_cast<int32(__cdecl*)(int32,void*)>(a)(e,p);
                    }
                    else if(nm=="RsPathnameDestroy") { char* p=next()%4 ? path.data() : nullptr; AllPatches ap; reinterpret_cast<void(__cdecl*)(char*)>(a)(p); }
                    else if(nm=="RsPathnameCreate") {
                        const char* s=randStr(path,"ab/\\c.:_");
                        char* r; { AllPatches ap; r=reinterpret_cast<char*(__cdecl*)(const char*)>(a)(s); }
                        ret=U(r);
                    }
                    else if(nm=="RsRwTerminate"||nm=="RsTerminate") { AllPatches ap; reinterpret_cast<void(__cdecl*)()>(a)(); }
                    else if(nm=="RsGrabScreen") { void* c=anyPtr(); RwImage* r; { AllPatches ap; r=reinterpret_cast<RwImage*(__cdecl*)(void*)>(a)(c); } ret=U(r); }
                    else if(nm=="RsErrorMessage"||nm=="RsWarningMessage") {
                        const char* s=next()%6 ? randStr(text,"Error: x=1 ") : nullptr; derefParam=true;
                        { AllPatches ap; reinterpret_cast<void(__cdecl*)(const char*)>(a)(s); }
                        derefParam=false;
                    }
                    else if(nm=="RsEventHandler") {
                        const int32 e=next()%3 ? int32(next()%40) : pick({21,21,23,24,37,37,1,30,-1,40,100,int32(next())});
                        void* p;
                        if(e==37){ static const char* const fixed[]{"-vms","-vm","-vmsx","","x"}; std::strcpy(cmd.data(),fixed[next()%5]); p=cmd.data(); }
                        else p=anyPtr();
                        AllPatches ap; ret=reinterpret_cast<int32(__cdecl*)(int32,void*)>(a)(e,p);
                    }
                    else if(nm=="RsRwInitialize") {
                        void* p=anyPtr();
                        AllPatches ap; ret=reinterpret_cast<int32(__cdecl*)(void*)>(a)(p);
                    }
                    out["rs_global"]=ReversingProbe::Bytes(rs,0x3C); out["default_vm"]=defaultVM;
                    out["ret"]=ret; out["trace"]=TraceJson();
                    restoreAll();
                    return out;
                };
                const auto original=run(false);
                const auto candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},
                             {"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restoreAll();
    bool restored=true; for(auto& s:snaps)restored&=s.Same();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
