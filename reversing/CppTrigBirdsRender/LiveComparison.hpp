#pragma once
// Banco dentro del motor: pajaros, creditos, curvas, cronometros, horarios, muebles y ventana (lista de 6):
// CBirds, CCredits, CCurves, COnscreenTimer, CTimeInfo, FurnitureGroup_c y Win (41 funciones). Original (hooks
// del lote desactivados) frente a candidato con fixtures deterministas. Se simulan y trazan: rand/GetRandomNumber,
// la linea de vision, el nivel del agua, CalcScreenCoors, el estado de render (fpRenderStateSet), el volcado de
// geometria, CFont, CText::Get, DefinedState2d, los bordes panoramicos, las funciones de FurnitureSubGroup_c,
// RsKeyboardEventHandler y las API de Win32 (IAT del EXE y de la DLL: PeekMessageA, TranslateMessage,
// DispatchMessageA, CreateEventA, GetLastError, FindWindowA, SetForegroundWindow, GetKeyState, MapVirtualKeyA/W).
// Nunca se toca la ventana ni la cola de mensajes reales. El estado global tocado se guarda y restaura.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Birds.h"
#include "Credits.h"
#include "Curves.h"
#include "OnscreenTimer.h"
#include "TimeInfo.h"
#include "Interior/FurnitureGroup_c.h"
#include "Interior/FurnitureSubGroup_c.h"
#include "Sprite.h"
#include "BrightLights.h"
#include "WaterLevel.h"
#include "Font.h"
#include "Text/Text.h"
#include "app.h"
#include "platform.h"
#include "../ProbeRuntime.hpp"

namespace CppTrigBirdsRenderLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 Str(const char* s){ return s ? uint32(Fnv(s,strlen(s))) : 0u; }

// --- registro sin reservas (los stubs de Win32 y de CFont no reservan memoria) ---
struct Rec { uint32 tag, a, b, c; };
inline std::array<Rec,8192> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b,recs[i].c}); return r; }

// --- RNG ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
// Cuerpo real de una funcion de la DLL (salta la tabla ILT): el GetRandomNumberInRange<float> desnudo la llama sin ILT.
inline uintptr_t Body(uintptr_t p){ while(*reinterpret_cast<const uint8*>(p)==0xE9)p=p+5+*reinterpret_cast<const int32*>(p+1); return p; }

// --- CBirds: linea de vision, agua, pantalla, render ---
inline uint32 losMask{}, losCalls{}, waterMask{}, waterCalls{}, screenMask{}, screenCalls{}; inline float waterLevel{};
inline bool __cdecl FakeLos(const CVector& o,const CVector& t,bool b,bool v,bool p,bool ob,bool d,bool s,bool c){
    trace.push_back({"los",F(o.x),F(o.y),F(o.z),F(t.x),F(t.y),F(t.z),b,v,p,ob,d,s,c}); return (losMask>>(losCalls++%32))&1; }
inline bool __cdecl FakeWater(CVector p,float* lvl,float* u1,float* u2){
    trace.push_back({"water",F(p.x),F(p.y),F(p.z),lvl!=nullptr,u1!=nullptr,u2!=nullptr}); const bool r=(waterMask>>(waterCalls++%32))&1; if(r&&lvl)*lvl=waterLevel; return r; }
inline bool __cdecl FakeScreen(const RwV3d& p,RwV3d* out,float* w,float* h,bool mx,bool mn){
    trace.push_back({"screen",F(p.x),F(p.y),F(p.z),mx,mn}); out->x=out->y=out->z=1.f; *w=*h=2.f; return (screenMask>>(screenCalls++%32))&1; }
inline RwBool __cdecl FakeRenderState(RwRenderState s,void* v){ trace.push_back({"rs",int(s),uintptr_t(v)}); return TRUE; }
inline void __cdecl FakeFlush(){ trace.push_back({"flush"}); }

// --- CCredits: CFont, CText, 2D ---
alignas(16) inline std::array<GxtChar,4096> textBuf{};
inline void __cdecl FakeDefinedState2d(){ Push(1); }
inline void __cdecl FakeSetBackground(bool a,bool b){ Push(2,uint8(a),uint8(b)); }
inline void __cdecl FakeSetCentreSize(float v){ Push(3,F(v)); }
inline void __cdecl FakeSetOrientation(int8 v){ Push(4,uint32(uint8(v))); }
inline void __cdecl FakeSetProportional(bool v){ Push(5,uint8(v)); }
inline void __cdecl FakeSetFontStyle(uint8 v){ Push(6,uint32(v)); }
inline void __cdecl FakeSetEdge(int8 v){ Push(7,uint32(int32(v))); }
inline void __cdecl FakeSetDropColor(uint32 c){ Push(8,c); }
inline void __cdecl FakeSetScale(float w,float h){ Push(9,F(w),F(h)); }
inline void __cdecl FakeSetColor(uint32 c){ Push(10,c); }
inline void __cdecl FakePrintString(float x,float y,const GxtChar* t){ Push(11,F(x),F(y),uint32(intptr_t(t)-intptr_t(textBuf.data()))); }
inline void __cdecl FakeDrawFonts(){ Push(12); }
inline void __fastcall FakeBorders(void* self,void*){ Push(13,uint32(uintptr_t(self))); }
inline const GxtChar* __fastcall FakeTextGet(void* self,void*,const char* key){ const uint32 h=Str(key); Push(14,h,uint32(uintptr_t(self))); return textBuf.data()+(h%1024)*4; }
inline void __cdecl FakeRenderCredits(){ Push(15); }

// --- FurnitureSubGroup_c ---
alignas(16) inline std::array<std::array<uint8,0x1C>,8> subGroups{}; alignas(16) inline std::array<uint8,64> furnitures{};
inline uint32 SgIndex(const void* p){ return uint32((intptr_t(p)-intptr_t(subGroups.data()))/0x1C); }
inline Furniture_c* __fastcall FakeSgGetFurniture(void* self,void*,int16 id,uint8 w){ trace.push_back({"sg_get",SgIndex(self),id,w}); return reinterpret_cast<Furniture_c*>(furnitures.data()+SgIndex(self)%8*4); }
inline int32 __fastcall FakeSgGetRandomId(void* self,void*,uint8 w){ trace.push_back({"sg_rand",SgIndex(self),w}); return int32(SgIndex(self)*1000+w); }
inline bool __fastcall FakeSgAddFurniture(void* self,void*,uint16 m,int16 id,uint8 a,uint8 b,uint8 c){ trace.push_back({"sg_add",SgIndex(self),m,id,a,b,c}); return (SgIndex(self)&1)!=0; }

// --- CTimeInfo: model infos falsos ---
struct FakeModel { void* vt; uint32 key; uint8 timeInfo[4]; bool hasTime; uint8 pad[0x34]; };
alignas(16) inline std::array<FakeModel,8> fakeModels{};
inline CTimeInfo* __fastcall FakeGetTimeInfo(void* self,void*){ auto* m=static_cast<FakeModel*>(self); trace.push_back({"time_info",int(m-fakeModels.data())}); return m->hasTime ? reinterpret_cast<CTimeInfo*>(m->timeInfo) : nullptr; }
inline void __fastcall FakeModelTrap(void* self,void*){ trace.push_back({"model_trap"}); }

// --- Win32 (solo el hilo principal; los demas hilos van a la API real) ---
inline DWORD mainTid{};
inline HMODULE SelfModule(){ HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&SelfModule),&m); return m; }
inline uintptr_t RealApi(const char* fn){ for(const char* dll:{"user32.dll","kernel32.dll"})if(auto m=GetModuleHandleA(dll))if(auto p=GetProcAddress(m,fn))return uintptr_t(p); return 0; }
// Sustituye las entradas de la IAT del EXE y de la DLL que apuntan a la API (y a su copia en kernelbase).
struct IatPatch {
    std::vector<std::pair<uintptr_t*,uintptr_t>> slots;
    static void Set(uintptr_t* t,uintptr_t v){ DWORD o{}; VirtualProtect(t,4,PAGE_READWRITE,&o); *t=v; VirtualProtect(t,4,o,&o); }
    template<class Fp> IatPatch(const char* fn,Fp fakeFn) {
        const auto fake=reinterpret_cast<uintptr_t>(fakeFn);
        std::array<uintptr_t,3> reals{RealApi(fn),0,0};
        if(auto kb=GetModuleHandleA("kernelbase.dll"))reals[1]=uintptr_t(GetProcAddress(kb,fn));
        for(HMODULE m:{GetModuleHandleA(nullptr),SelfModule()}) {
            auto* base=reinterpret_cast<uint8*>(m); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
            const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)continue;
            for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
                for(auto* t=reinterpret_cast<uintptr_t*>(base+d->FirstThunk);*t;++t)
                    if(*t&&(*t==reals[0]||*t==reals[1])){ slots.push_back({t,*t}); Set(t,fake); }
        }
    }
    ~IatPatch(){ for(auto& [t,v]:slots)Set(t,v); }
};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }
struct FakeMsg { uint32 hwnd, message, wParam, lParam; };
inline std::array<FakeMsg,8> msgQueue{}; inline size_t msgCount{}, msgPos{};
inline uint32 lastError{}, findResult{}, keyMask{}, keyCalls{}, mapResult{};
inline BOOL WINAPI FakePeekMessageA(LPMSG msg,HWND h,UINT mn,UINT mx,UINT rm){
    if(!Main())return reinterpret_cast<decltype(&PeekMessageA)>(RealApi("PeekMessageA"))(msg,h,mn,mx,rm);
    Push(20,uint32(uintptr_t(h)),mn|(mx<<16),rm); if(msgPos>=msgCount)return FALSE;
    const auto& q=msgQueue[msgPos++]; msg->hwnd=reinterpret_cast<HWND>(uintptr_t(q.hwnd)); msg->message=q.message; msg->wParam=q.wParam; msg->lParam=LPARAM(q.lParam); msg->time=0x1234; msg->pt={5,6}; return TRUE; }
inline BOOL WINAPI FakeTranslateMessage(const MSG* m){
    if(!Main())return reinterpret_cast<decltype(&TranslateMessage)>(RealApi("TranslateMessage"))(m);
    Push(21,m->message,uint32(m->wParam),uint32(m->lParam)); return TRUE; }
inline LRESULT WINAPI FakeDispatchMessageA(const MSG* m){
    if(!Main())return reinterpret_cast<decltype(&DispatchMessageA)>(RealApi("DispatchMessageA"))(m);
    Push(22,m->message,uint32(m->wParam),uint32(uintptr_t(m->hwnd))); return 0; }
inline HANDLE WINAPI FakeCreateEventA(LPSECURITY_ATTRIBUTES a,BOOL manual,BOOL initial,LPCSTR name){
    if(!Main())return reinterpret_cast<decltype(&CreateEventA)>(RealApi("CreateEventA"))(a,manual,initial,name);
    Push(23,uint32(uintptr_t(a)),uint32(manual)|(uint32(initial)<<8),Str(name)); return reinterpret_cast<HANDLE>(0x5151); }
inline DWORD WINAPI FakeGetLastError(){
    if(!Main())return reinterpret_cast<decltype(&GetLastError)>(RealApi("GetLastError"))();
    return lastError; }
inline HWND WINAPI FakeFindWindowA(LPCSTR cls,LPCSTR name){
    if(!Main())return reinterpret_cast<decltype(&FindWindowA)>(RealApi("FindWindowA"))(cls,name);
    Push(24,Str(cls),Str(name)); return reinterpret_cast<HWND>(uintptr_t(findResult)); }
inline BOOL WINAPI FakeSetForegroundWindow(HWND h){
    if(!Main())return reinterpret_cast<decltype(&SetForegroundWindow)>(RealApi("SetForegroundWindow"))(h);
    Push(25,uint32(uintptr_t(h))); return TRUE; }
inline SHORT WINAPI FakeGetKeyState(int vk){
    if(!Main())return reinterpret_cast<decltype(&GetKeyState)>(RealApi("GetKeyState"))(vk);
    const uint32 k=keyCalls++; Push(26,uint32(vk)); const bool down=(keyMask>>(k%32))&1; return SHORT(down ? 0x8000|(k&0x7F) : (k&0x7F)); }
inline UINT WINAPI FakeMapVirtualKey(UINT code,UINT type){
    if(!Main())return reinterpret_cast<decltype(&MapVirtualKeyA)>(RealApi("MapVirtualKeyA"))(code,type);
    Push(27,code,type); return mapResult; }
inline RsKeyCodes* curKeyCode{};
inline RsEventStatus __cdecl FakeKeyboardHandler(RsEvent e,void* p){ Push(28,uint32(e),p ? uint32(*static_cast<int32*>(p)) : 0xDEAD,p==curKeyCode); return rsEVENTPROCESSED; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,41> fns{{
    {"CBirds","Init",0x711EC0,0,-1,64},{"CBirds","Shutdown",0x712300,0,-1,128},{"CBirds","HandleGunShot",0x712E40,0,-1,512},
    {"CBirds","CreateNumberOfBirds",0x711EF0,0,-1,1024},{"CBirds","Update",0x712330,0,-1,1024},{"CBirds","Render",0x712810,0,-1,512},
    {"CCredits","Start",0x7170E0,0,-1,64},{"CCredits","Stop",0x717100,0,-1,64},{"CCredits","Render",0x53D5B0,0,-1,128},
    {"CCredits","PrintCreditText",0x5A8660,0,-1,1024},{"CCredits","PrintCreditSpace",0x5A87C0,0,-1,512},{"CCredits","RenderCredits",0x5A87F0,0,-1,64},
    {"CCurves","TestCurves",0x43C600,0,-1,64},{"CCurves","DistForLineToCrossOtherLine",0x43C610,0,-1,1024},{"CCurves","CalcSpeedVariationInBend",0x43C660,0,-1,1024},
    {"CCurves","CalcSpeedScaleFactor",0x43C710,0,-1,1024},{"CCurves","CalcCorrectedDist",0x43C880,0,-1,1024},{"CCurves","CalcCurvePoint",0x43C900,0,-1,1024},
    {"COnscreenTimer","AddClock",0x44CD50,0,-1,512},{"COnscreenTimer","AddCounter",0x44CDA0,0,-1,512},{"COnscreenTimer","ClearClock",0x44CE60,0,-1,512},
    {"COnscreenTimer","ClearCounter",0x44CE80,0,-1,512},{"COnscreenTimer","SetClockBeepCountdownSecs",0x44CEE0,0,-1,512},{"COnscreenTimer","SetCounterFlashWhenFirstDisplayed",0x44CEB0,0,-1,512},
    {"CTimeInfo","FindOtherTimeModel",0x4C47E0,0,-1,256},{"CTimeInfo","GetOtherTimeModel",0x4C4A30,0,-1,256},{"CTimeInfo","GetTimeOff",0x407330,0,-1,256},
    {"CTimeInfo","GetTimeOn",0x407320,0,-1,256},{"CTimeInfo","SetOtherTimeModel",0x5B3440,0,-1,256},{"CTimeInfo","SetTimes",0x5B3430,0,-1,256},
    {"FurnitureGroup_c","Init",0x5910A0,0,-1,64},{"FurnitureGroup_c","Exit",0x5910B0,0,-1,512},{"FurnitureGroup_c","AddSubGroup",0x5910E0,0,-1,512},
    {"FurnitureGroup_c","GetFurniture",0x591130,0,-1,512},{"FurnitureGroup_c","GetRandomId",0x591170,0,-1,512},{"FurnitureGroup_c","AddFurniture",0x5C0230,0,-1,512},
    {"Win","GTATranslateKey",0x747820,0,-1,1024},{"Win","GTATranslateShiftKey",0x747CD0,0,-1,256},{"Win","IsAlreadyRunning",0x7468E0,0,-1,256},
    {"Win","IsForegroundApp",0x746060,0,-1,128},{"Win","MessageLoop",0x746870,0,-1,256},
}};

constexpr uintptr_t BIRDS_BEGIN=0xC6A8A0, BIRDS_END=0xC6AA54;      // bHasBirdBeenShot .. vecBirdShotAt
constexpr uintptr_t TB_VERTS=0xC4D958, TB_IDX=0xC4B958, TB_CNT=0xC4B950;
constexpr size_t NTBV=96*0x24, NTBI=360*2;

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cpptrigbirdsrender-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cpptrigbirdsrender-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cpptrigbirdsrender");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"cpptrigbirdsrender-only.txt"); if(f)std::getline(f,only); }
    const auto& all=fns;
    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks(all.size());
    for(size_t f=0;f<all.size();++f)if(!(hooks[f]=FindHook(all[f].cls,all[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(all[f].cls)+"::"+all[f].name}});return;}
    std::vector<uint8> enabled(all.size()),locked(all.size());
    for(size_t f=0;f<all.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); const float savedStep=timeStep;
    auto& frameCounter=*reinterpret_cast<uint32*>(0xB7CB4C); const uint32 savedFrame=frameCounter;
    auto& area=*reinterpret_cast<int32*>(0xB72914); const int32 savedArea=area;
    auto& hours=*reinterpret_cast<uint8*>(0xB70153); const uint8 savedHours=hours;
    auto& region=*reinterpret_cast<int16*>(0xC81314); const int16 savedRegion=region;
    auto& distWater=TheCamera.m_fDistanceToWater; const float savedDistWater=distWater;
    auto& camFwd=TheCamera.m_mCameraMatrix.GetForward(); const CVector savedFwd=camFwd;
    auto& wide=TheCamera.m_bWideScreenOn; const bool savedWide=wide;
    auto& cloudTex=*reinterpret_cast<RwTexture**>(0xC6AA7C); RwTexture* const savedCloudTex=cloudTex;
    auto& fpState=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedFp=fpState;
    auto* birds=reinterpret_cast<uint8*>(BIRDS_BEGIN); const size_t NB=BIRDS_END-BIRDS_BEGIN; const std::vector<uint8> savedBirds(birds,birds+NB);
    auto* tbv=reinterpret_cast<uint8*>(TB_VERTS); const std::vector<uint8> savedTbv(tbv,tbv+NTBV);
    auto* tbi=reinterpret_cast<uint8*>(TB_IDX); const std::vector<uint8> savedTbi(tbi,tbi+NTBI);
    auto* tbc=reinterpret_cast<uint8*>(TB_CNT); const std::vector<uint8> savedTbc(tbc,tbc+8);
    auto& credStart=*reinterpret_cast<uint32*>(0xC6E978); const uint32 savedCredStart=credStart;
    auto& credGoing=*reinterpret_cast<uint8*>(0xC6E97C); const uint8 savedCredGoing=credGoing;
    auto& menuActive=*reinterpret_cast<uint8*>(0xBA67A4); const uint8 savedMenu=menuActive;
    auto& scrW=RsGlobal.maximumWidth; auto& scrH=RsGlobal.maximumHeight; const int32 savedW=scrW, savedH=scrH;
    auto& quit=RsGlobal.quit; const RwBool savedQuit=quit;
    auto& osVer=*reinterpret_cast<int32*>(0xC8CF68); const int32 savedOs=osVer;
    auto& foreground=*reinterpret_cast<uint32*>(0x8D621C); const uint32 savedFg=foreground;
    auto* sgStore=reinterpret_cast<uint8*>(0xBAD3F8); constexpr size_t NSG=128*0x1C; const std::vector<uint8> savedSgStore(sgStore,sgStore+NSG);
    auto& sgCount=*reinterpret_cast<uint32*>(0xBAB37C); const uint32 savedSgCount=sgCount;
    auto& miPtrs=CModelInfo::ms_modelInfoPtrs;
    std::vector<int32> freeSlots; for(int32 k=0;k<CModelInfo::NUM_MODEL_INFOS&&freeSlots.size()<64;k+=97)if(!miPtrs[k])freeSlots.push_back(k);
    if(freeSlots.size()<16){Write({{"event","error"},{"reason","not enough free model info slots"}});return;}
    auto stateOk=[&]{ return std::memcmp(birds,savedBirds.data(),NB)==0&&std::memcmp(tbv,savedTbv.data(),NTBV)==0&&std::memcmp(tbi,savedTbi.data(),NTBI)==0
        &&std::memcmp(tbc,savedTbc.data(),8)==0&&std::memcmp(sgStore,savedSgStore.data(),NSG)==0&&sgCount==savedSgCount&&timeMs==savedTime&&credGoing==savedCredGoing
        &&credStart==savedCredStart&&scrW==savedW&&scrH==savedH&&quit==savedQuit&&osVer==savedOs&&foreground==savedFg&&fpState==savedFp&&cloudTex==savedCloudTex; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        const CVector cam=TheCamera.GetPosition();
        static std::array<void*,32> fakeVt{}; for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeModelTrap); fakeVt[5]=reinterpret_cast<void*>(&FakeGetTimeInfo);
        alignas(16) static std::array<uint8,0x60> fakeTex{}; *reinterpret_cast<uintptr_t*>(fakeTex.data())=0x7A57E0;

        for(size_t f=0;f<all.size();++f) {
            const auto& fn=all[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(full!="CBirds::Render")continue;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x6A15u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; nrec=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CBirds") {
                        // pajaros aleatorios alrededor de la camara
                        for(size_t k=0;k<NB;++k)birds[k]=uint8(next());
                        for(auto& b:CBirds::aBirds){
                            b.m_vecPosn=cam+CVector{uni(-90.f,90.f),uni(-90.f,90.f),uni(-20.f,30.f)};
                            b.m_vecCurrentVelocity=CVector{uni(-6.f,6.f),uni(-6.f,6.f),uni(-1.f,1.f)}; b.m_vecTargetVelocity=CVector{uni(-6.f,6.f),uni(-6.f,6.f),uni(-1.f,1.f)};
                            b.m_fAngle=uni(-4.f,4.f); b.m_nUpdateAfterMS=next()%4 ? 0 : next()%200000;
                            static const uint32 ws[]{320,500,640,1000,1140,1500,1000000,7};
                            b.m_nWingStillness=next()%6 ? ws[next()%8] : 1+next()%20000; b.m_fSize=uni(0.4f,2.6f);
                            static const float md[]{40.f,45.f,80.f}; b.m_fMaxBirdDistance=next()%8 ? md[next()%3] : uni(1.f,120.f);
                            static const uint8 modes[]{1,1,1,2,0,3}; b.m_eBirdMode=eBirdMode(modes[next()%6]);
                            reinterpret_cast<uint8&>(b.m_bCreated)=uint8(next()%3 ? 1 : 0); reinterpret_cast<uint8&>(b.m_bMustDoCurves)=uint8(next()%3==0);
                        }
                        auto& numBirds=CBirds::uiNumberOfBirds; numBirds=0; for(auto& b:CBirds::aBirds)numBirds+=b.m_bCreated; if(next()%8==0)numBirds=next()%9; // sin negativos: con n < 0 Update crearia 7 pajaros y saltaria el assert NOTSA de i < 6
                        reinterpret_cast<uint8&>(CBirds::bHasBirdBeenShot)=uint8(next()%2);
                        losMask=next(); losCalls=0; waterMask=next(); waterCalls=0; screenMask=next()|next(); screenCalls=0; waterLevel=uni(-10.f,30.f);
                        timeMs=next()%2 ? next()%1000000 : next();
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="HandleGunShot"){ const auto& tb=CBirds::aBirds[next()%6]; CVector A=cam+CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-2.f,2.f)};
                            CVector B=next()%3 ? tb.m_vecPosn+(tb.m_vecPosn-A)*uni(0.f,1.f)+CVector{uni(-0.6f,0.6f),uni(-0.6f,0.6f),uni(-0.6f,0.6f)} : cam+CVector{uni(-90.f,90.f),uni(-90.f,90.f),uni(-20.f,30.f)};
                            reinterpret_cast<void(__cdecl*)(const CVector*,const CVector*)>(a)(&A,&B); }
                        else if(nm=="CreateNumberOfBirds"){ const CVector s=cam+CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,30.f)}; CVector t=cam+CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(0.f,30.f)};
                            if(next()%10==0)t=s+CVector{0.f,0.f,uni(0.1f,5.f)};
                            const int32 count=next()%10==0 ? -int32(next()%3) : int32(next()%7); const int32 biome=int32(next()%7==0 ? 3+next()%3 : next()%3);
                            EntryPatch l1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLos), l1e(0x56A490,FakeLos);
                            reinterpret_cast<void(__cdecl*)(CVector,CVector,int32,int32,bool)>(a)(s,t,count,biome,next()%4!=0); }
                        else if(nm=="Update"){ area=next()%6==0 ? int32(1+next()%5) : 0; hours=uint8(next()%5==0 ? next()%24 : 5+next()%18);
                            frameCounter=next()%3 ? (next()&~0x1FFu)|6u : next(); if(next()%6==0)frameCounter=(next()&~0x1FFu)|uint32(next()%12);
                            distWater=next()%8==0 ? 30.f : uni(0.f,60.f); region=int16(next()%3==0 ? 4 : next()%6);
                            timeStep=next()%10==0 ? 0.f : uni(0.2f,3.5f);
                            camFwd=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.5f,0.5f)}; if(next()%6==0)camFwd.x=0.f;
                            EntryPatch l1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),FakeLos), l1e(0x56A490,FakeLos);
                            EntryPatch w1(bit_cast<uintptr_t>(&CWaterLevel::GetWaterLevelNoWaves),FakeWater), w1e(0x6E8580,FakeWater);
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            area=savedArea; hours=savedHours; frameCounter=savedFrame; distWater=savedDistWater; region=savedRegion; timeStep=savedStep; camFwd=savedFwd; }
                        else if(nm=="Render"){ for(size_t k=0;k<NTBV;++k)tbv[k]=uint8(next()); for(size_t k=0;k<NTBI;++k)tbi[k]=uint8(next()); for(size_t k=0;k<8;++k)tbc[k]=uint8(next());
                            for(auto& b:CBirds::aBirds)if(b.m_nWingStillness>(1u<<24))b.m_nWingStillness=1000;
                            cloudTex=reinterpret_cast<RwTexture*>(fakeTex.data()); fpState=FakeRenderState;
                            EntryPatch s1(bit_cast<uintptr_t>(static_cast<bool(*)(const RwV3d&,RwV3d*,float*,float*,bool,bool)>(&CSprite::CalcScreenCoors)),FakeScreen), s1e(0x70CE30,FakeScreen);
                            EntryPatch g1(bit_cast<uintptr_t>(&CBrightLights::RenderOutGeometryBuffer),FakeFlush), g1e(0x722150,FakeFlush);
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            fpState=savedFp; cloudTex=savedCloudTex;
                            out["tbc"]=ReversingProbe::Bytes(tbc,8); out["tbv"]=Fnv(tbv,NTBV); out["tbi"]=Fnv(tbi,NTBI);
                            std::memcpy(tbv,savedTbv.data(),NTBV); std::memcpy(tbi,savedTbi.data(),NTBI); std::memcpy(tbc,savedTbc.data(),8); }
                        out["birds"]=ReversingProbe::Bytes(birds,NB); std::memcpy(birds,savedBirds.data(),NB); timeMs=savedTime;
                    }
                    else if(cls=="CCredits") {
                        const uint8 goingIn=uint8(next()%3 ? next()%2 : next()); credGoing=goingIn; credStart=next()%2 ? next()%100000 : next();
                        timeMs=credStart+(next()%4 ? next()%400000 : next()%(1u<<24)); menuActive=uint8(next()%3==0);
                        static const int32 ws[]{640,800,1024,1280,1366,1600,1920,1023,1281,961}; static const int32 hs[]{480,600,768,720,1080,1050,767,1081};
                        scrW=ws[next()%10]; scrH=hs[next()%8]; wide=next()%2;
                        EntryPatch f1(bit_cast<uintptr_t>(&CFont::SetScale),FakeSetScale), f1e(0x719380,FakeSetScale);
                        EntryPatch f2(bit_cast<uintptr_t>(&CFont::SetColor),FakeSetColor), f2e(0x719430,FakeSetColor);
                        EntryPatch f3(bit_cast<uintptr_t>(&CFont::PrintString),FakePrintString), f3e(0x71A700,FakePrintString);
                        if(nm=="Start")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Stop")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Render"){ EntryPatch rc(bit_cast<uintptr_t>(&CCredits::RenderCredits),FakeRenderCredits), rce(0x5A87F0,FakeRenderCredits); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="PrintCreditText"){ static const float sc[]{1.1f,0.78f,1.0f,0.5f,2.0f}; const float sx=next()%5 ? sc[next()%5] : uni(0.f,3.f), sy=next()%4 ? sx : uni(0.f,3.f);
                            uint32 pos=next()%4 ? next()%30000 : next()%(1u<<24); const float off=next()%4 ? float(pos)+uni(-200.f,float(scrH)+200.f) : uni(-1000.f,40000.f);
                            reinterpret_cast<void(__cdecl*)(float,float,const GxtChar*,uint32&,float,bool)>(a)(sx,sy,textBuf.data()+next()%1000*4,pos,off,next()%2); out["pos"]=pos; }
                        else if(nm=="PrintCreditSpace"){ static const float sp[]{0.5f,1.0f,1.5f}; const float s=next()%3 ? sp[next()%3] : uni(-2.f,4.f); uint32 pos=next()%4 ? next()%30000 : next()%(1u<<24);
                            reinterpret_cast<void(__cdecl*)(float,uint32&)>(a)(s,pos); ret=pos; }
                        else { EntryPatch d1(bit_cast<uintptr_t>(&DefinedState2d),FakeDefinedState2d), d1e(0x734750,FakeDefinedState2d);
                            EntryPatch g1(bit_cast<uintptr_t>(&CFont::SetBackground),FakeSetBackground), g1e(0x7195C0,FakeSetBackground);
                            EntryPatch g2(bit_cast<uintptr_t>(&CFont::SetCentreSize),FakeSetCentreSize), g2e(0x7194E0,FakeSetCentreSize);
                            EntryPatch g3(bit_cast<uintptr_t>(&CFont::SetOrientation),FakeSetOrientation), g3e(0x719610,FakeSetOrientation);
                            EntryPatch g4(bit_cast<uintptr_t>(&CFont::SetProportional),FakeSetProportional), g4e(0x7195B0,FakeSetProportional);
                            EntryPatch g5(bit_cast<uintptr_t>(&CFont::SetFontStyle),FakeSetFontStyle), g5e(0x719490,FakeSetFontStyle);
                            EntryPatch g6(bit_cast<uintptr_t>(&CFont::SetEdge),FakeSetEdge), g6e(0x719590,FakeSetEdge);
                            EntryPatch g7(bit_cast<uintptr_t>(&CFont::SetDropColor),FakeSetDropColor), g7e(0x719510,FakeSetDropColor);
                            EntryPatch g8(bit_cast<uintptr_t>(&CFont::DrawFonts),FakeDrawFonts), g8e(0x71A210,FakeDrawFonts);
                            EntryPatch g9(bit_cast<uintptr_t>(&CCamera::DrawBordersForWideScreen),FakeBorders), g9e(0x514860,FakeBorders);
                            EntryPatch t1(bit_cast<uintptr_t>(&CText::Get),FakeTextGet), t1e(0x6A0050,FakeTextGet);
                            if(next()%2==0)timeMs=credStart+next()%700000; // recorre todo el rodillo, final incluido
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["going"]=credGoing; out["start"]=credStart; out["recs_n"]=nrec; out["recs"]=std::to_string(Fnv(recs.data(),nrec*sizeof(Rec)));
                        if(nrec<=64)out["rec_list"]=Recs();
                        credGoing=savedCredGoing; credStart=savedCredStart; timeMs=savedTime; menuActive=savedMenu; scrW=savedW; scrH=savedH; wide=savedWide;
                    }
                    else if(cls=="CCurves") {
                        auto vec=[&](float r){ return CVector{uni(-r,r),uni(-r,r),uni(-r,r)}; };
                        auto dirv=[&]{ CVector d{uni(-1.f,1.f),uni(-1.f,1.f),uni(-0.2f,0.2f)}; switch(next()%8){case 0:d.Normalise();break;case 1:d=CVector{1.f,0.f,0.f};break;case 2:d=CVector{0.f,0.f,0.f};break;default:break;} return d; };
                        if(nm=="TestCurves")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="DistForLineToCrossOtherLine"){ float v[8]; for(auto& x:v)x=uni(-100.f,100.f); if(next()%5==0){const float k=uni(-3.f,3.f);v[6]=v[2]*k;v[7]=v[3]*k;} if(next()%10==0){v[2]=v[3]=0.f;}
                            ret=F(reinterpret_cast<float(__cdecl*)(float,float,float,float,float,float,float,float)>(a)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7])); }
                        else if(nm=="CalcSpeedVariationInBend"||nm=="CalcSpeedScaleFactor"){ const CVector s=vec(200.f); CVector e=next()%12==0 ? s : s+vec(40.f); const CVector d1=dirv(); CVector d2=dirv();
                            if(next()%4==0){d2=d1; d2.x+=uni(-0.3f,0.3f); d2.y+=uni(-0.3f,0.3f);} if(next()%10==0)d2=CVector{-d1.x,-d1.y,0.f};
                            ret=F(reinterpret_cast<float(__cdecl*)(const CVector&,const CVector&,float,float,float,float)>(a)(s,e,d1.x,d1.y,d2.x,d2.y)); }
                        else if(nm=="CalcCorrectedDist"){ float tot; switch(next()%6){case 0:tot=uni(-1.f,0.00002f);break;case 1:tot=0.00001f;break;default:tot=uni(0.f,300.f);} const float cur=next()%5 ? uni(0.f,1.f)*tot : uni(-50.f,400.f);
                            const float sv=next()%4 ? uni(0.f,0.3334f) : uni(-1.f,2.f); float t=-777.f; ret=F(reinterpret_cast<float(__cdecl*)(float,float,float,float*)>(a)(cur,tot,sv,&t)); out["t"]=F(t); }
                        else { const CVector s=vec(200.f); CVector e=next()%16==0 ? s : s+vec(40.f); const CVector d1=dirv(); CVector d2=dirv(); if(next()%4==0){d2=d1;d2.x+=uni(-0.4f,0.4f);d2.y+=uni(-0.4f,0.4f);}
                            float time; switch(next()%10){case 0:time=0.f;break;case 1:time=1.f;break;case 2:time=uni(-0.5f,0.f);break;case 3:time=uni(1.f,1.5f);break;case 4:time=std::bit_cast<float>(0x7FC00000u);break;default:time=uni(0.f,1.f);}
                            const int32 ms=next()%12==0 ? 0 : int32(100+next()%30000); CVector rc{-5.f,-5.f,-5.f}, rs{-6.f,-6.f,-6.f};
                            reinterpret_cast<void(__cdecl*)(const CVector&,const CVector&,const CVector&,const CVector&,float,int32,CVector&,CVector&)>(a)(s,e,d1,d2,time,ms,rc,rs);
                            ret=json::array({F(rc.x),F(rc.y),F(rc.z),F(rs.x),F(rs.y),F(rs.z)}); }
                    }
                    else if(cls=="COnscreenTimer") {
                        alignas(16) std::array<uint8,0x154> obj{}; for(auto& b:obj)b=uint8(next());
                        const uint32 ids[]{0u,0x10u,0x20u,0x30u}; auto rid=[&]{ return next()%8 ? ids[next()%4] : next(); };
                        *reinterpret_cast<uint32*>(obj.data())=rid(); for(int k=0;k<4;++k)*reinterpret_cast<uint32*>(obj.data()+0x40+k*0x44)=rid();
                        for(int k=1;k<4;++k)*reinterpret_cast<uint32*>(obj.data()+k*0x40)=next()%3==0 ? rid() : *reinterpret_cast<uint32*>(obj.data()+k*0x40);
                        char gxt[24]{}; const int len=int(next()%15); for(int k=0;k<len;++k)gxt[k]=char('A'+next()%26); char* g=next()%8 ? gxt : nullptr;
                        const uint32 id=rid();
                        if(nm=="AddClock")reinterpret_cast<void(__thiscall*)(void*,uint32,char*,uint8)>(a)(obj.data(),id,g,uint8(next()%4 ? next()%2 : next()));
                        else if(nm=="AddCounter")reinterpret_cast<void(__thiscall*)(void*,uint32,uint16,char*,uint16)>(a)(obj.data(),id,uint16(next()%4 ? next()%3 : next()),g,uint16(next()%4));
                        else if(nm=="ClearClock")reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(obj.data(),id);
                        else if(nm=="ClearCounter")reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(obj.data(),id);
                        else if(nm=="SetClockBeepCountdownSecs")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(a)(obj.data(),id,next());
                        else reinterpret_cast<void(__thiscall*)(void*,uint32,bool)>(a)(obj.data(),id,next()%2!=0);
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if(cls=="CTimeInfo") {
                        alignas(4) std::array<uint8,4> obj{}; for(auto& b:obj)b=uint8(next());
                        if(nm=="GetTimeOn"||nm=="GetTimeOff")ret=int(reinterpret_cast<uint8(__thiscall*)(void*)>(a)(obj.data()));
                        else if(nm=="GetOtherTimeModel")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="SetTimes")reinterpret_cast<void(__thiscall*)(void*,uint8,uint8)>(a)(obj.data(),uint8(next()),uint8(next()));
                        else if(nm=="SetOtherTimeModel")reinterpret_cast<void(__thiscall*)(void*,int16)>(a)(obj.data(),int16(next()));
                        else { static const char* bases[]{"m6zqa","m6zq_b","lamp6zq","m6zqcj_x","q6zz9"}; static const char* sufs[]{"_nt","_dy","","_NT","_dyx"};
                            char names[8][24]{}; std::array<int32,8> slots{}; const int nf=int(next()%7);
                            for(int k=0;k<nf;++k){ auto& m=fakeModels[k]; std::memset(&m,0,sizeof(m)); m.vt=fakeVt.data(); snprintf(names[k],24,"%s%s",bases[next()%5],sufs[next()%3]);
                                m.key=CKeyGen::GetUppercaseKey(names[k]); m.hasTime=next()%4!=0; for(auto& b:m.timeInfo)b=uint8(next());
                                slots[k]=freeSlots[(next()%(freeSlots.size()/8))*8+k%8]; }
                            for(int k=0;k<nf;++k)miPtrs[slots[k]]=reinterpret_cast<CBaseModelInfo*>(&fakeModels[k]);
                            char query[24]{}; snprintf(query,24,"%s%s",bases[next()%5],sufs[next()%5]); if(next()%6==0)snprintf(query,24,"ab_ntcd%u",next()%10);
                            auto* r=reinterpret_cast<uint8*(__thiscall*)(void*,const char*)>(a)(obj.data(),query);
                            for(int k=0;k<nf;++k)miPtrs[slots[k]]=nullptr;
                            int which=-1; for(int k=0;k<nf;++k)if(r==fakeModels[k].timeInfo)which=k; ret=r ? json(which) : json("null");
                            json sl=json::array(); for(int k=0;k<nf;++k)sl.push_back(slots[k]); out["slots"]=sl; }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if(cls=="FurnitureGroup_c") {
                        alignas(16) std::array<uint8,0xC> group{}; for(auto& g:subGroups)for(auto& b:g)b=uint8(next());
                        const int n=int(next()%7); std::array<int,8> order{0,1,2,3,4,5,6,7}; for(int k=7;k>0;--k)std::swap(order[k],order[next()%(k+1)]);
                        auto P=[&](void* base,size_t off)->uint8*&{ return *reinterpret_cast<uint8**>(static_cast<uint8*>(base)+off); };
                        uint8* prev=nullptr; for(int k=0;k<n;++k){ uint8* s=subGroups[order[k]].data(); P(s,0)=prev; P(s,4)=nullptr; if(prev)P(prev,4)=s; prev=s; *reinterpret_cast<int32*>(s+8)=int32(next()%5); }
                        P(group.data(),0)=n ? subGroups[order[0]].data() : nullptr; P(group.data(),4)=prev; *reinterpret_cast<uint32*>(group.data()+8)=uint32(n);
                        const int32 id=int32(next()%6);
                        EntryPatch s1(bit_cast<uintptr_t>(&FurnitureSubGroup_c::GetFurniture),FakeSgGetFurniture), s1e(0x590EE0,FakeSgGetFurniture);
                        EntryPatch s2(bit_cast<uintptr_t>(&FurnitureSubGroup_c::GetRandomId),FakeSgGetRandomId), s2e(0x590FD0,FakeSgGetRandomId);
                        EntryPatch s3(bit_cast<uintptr_t>(&FurnitureSubGroup_c::AddFurniture),FakeSgAddFurniture), s3e(0x5C00C0,FakeSgAddFurniture);
                        auto Name=[&](const void* p)->json{ if(!p)return nullptr; const auto u=intptr_t(p); if(u>=intptr_t(furnitures.data())&&u<intptr_t(furnitures.data()+64))return u-intptr_t(furnitures.data()); return "foreign"; };
                        if(nm=="Init")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(group.data());
                        else if(nm=="Exit")reinterpret_cast<void(__thiscall*)(void*)>(a)(group.data());
                        else if(nm=="AddSubGroup"){ for(size_t k=0;k<NSG;++k)sgStore[k]=uint8(next()); sgCount=(next()%3==0 ? next()<<16 : 0u)|(next()%4 ? next()%128 : 120+next()%16);
                            ret=int(reinterpret_cast<uint8(__thiscall*)(void*,int32,int32,int32,int32,int32,uint8,bool,bool)>(a)(group.data(),int32(next()%100),int32(next()),int32(next()),int32(next()),int32(next()),uint8(next()),next()%2!=0,next()%2!=0));
                            out["count"]=sgCount; out["store"]=std::to_string(Fnv(sgStore,NSG)); std::memcpy(sgStore,savedSgStore.data(),NSG); sgCount=savedSgCount; }
                        else if(nm=="GetFurniture")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32,int16,uint8)>(a)(group.data(),id,int16(next()),uint8(next())));
                        else if(nm=="GetRandomId")ret=reinterpret_cast<int32(__thiscall*)(void*,int32,uint8)>(a)(group.data(),id,uint8(next()));
                        else ret=int(reinterpret_cast<uint8(__thiscall*)(void*,int32,uint16,int16,uint8,uint8,uint8)>(a)(group.data(),id,uint16(next()),int16(next()),uint8(next()),uint8(next()),uint8(next())));
                        // punteros como desplazamientos relativos a los subgrupos falsos
                        json g=json::array(); for(int k=0;k<2;++k){ const auto v=intptr_t(P(group.data(),k*4)); g.push_back(v ? v-intptr_t(subGroups.data()) : 0); } g.push_back(*reinterpret_cast<uint32*>(group.data()+8));
                        out["group"]=g; out["subgroups"]=ReversingProbe::Bytes(subGroups.data(),sizeof(subGroups));
                    }
                    else if(cls=="Win") {
                        osVer=next()%4==0 ? 1 : int32(next()%5);
                        if(nm=="GTATranslateKey"){ static const uint32 maps[]{0x41,0x7A,0x20,0xFE,0xFF,0x100,0x80000041,0x8000FF00,0,0x30};
                            mapResult=next()%4 ? 0x20+next()%0x60 : maps[next()%10]; const UINT vk=next()%6==0 ? next()%256 : 8+next()%0x8C; const LPARAM lp=LPARAM(next()%2 ? next()&0x01FFFFFF : next());
                            IatPatch m1("MapVirtualKeyA",FakeMapVirtualKey), m2("MapVirtualKeyW",FakeMapVirtualKey);
                            int32 ck=int32(0x5A5A5A5A); ret=int(uint8(reinterpret_cast<BOOL(__cdecl*)(int32*,LPARAM,UINT)>(a)(&ck,lp,vk))); out["ck"]=ck; }
                        else if(nm=="GTATranslateShiftKey"){ keyMask=next(); keyCalls=0; int32 ck=int32(next()%2 ? 0x41 : next()); curKeyCode=reinterpret_cast<RsKeyCodes*>(&ck);
                            IatPatch k1("GetKeyState",FakeGetKeyState);
                            EntryPatch h1(bit_cast<uintptr_t>(&RsKeyboardEventHandler),FakeKeyboardHandler), h1e(0x619560,FakeKeyboardHandler);
                            reinterpret_cast<BOOL(__cdecl*)(int32*)>(a)(&ck); out["ck"]=ck; } // el EXE no fija el valor de retorno
                        else if(nm=="IsAlreadyRunning"){ lastError=next()%2 ? 0xB7 : next()%300; findResult=next()%3 ? 0x10000+next()%0x1000 : 0;
                            IatPatch c1("CreateEventA",FakeCreateEventA), c2("GetLastError",FakeGetLastError), c3("FindWindowA",FakeFindWindowA), c4("SetForegroundWindow",FakeSetForegroundWindow);
                            ret=int(uint8(reinterpret_cast<uint32(__cdecl*)()>(a)())); }
                        else if(nm=="IsForegroundApp"){ static const uint32 v[]{0,1,0x100,0x10000,0x1000000,0xFFFFFFFF}; foreground=next()%2 ? v[next()%6] : next();
                            ret=int(uint8(reinterpret_cast<uint32(__cdecl*)()>(a)())); }
                        else { msgCount=next()%6; msgPos=0; static const uint32 msgs[]{0x12,0x100,0x101,0x200,0x1C,0x0F,0x113}; for(size_t k=0;k<msgCount;++k)msgQueue[k]={0x20000u+next()%16,next()%3 ? msgs[next()%7] : next()%0x400,next(),next()};
                            quit=RwBool(next()%3==0 ? 1 : 0);
                            IatPatch p1("PeekMessageA",FakePeekMessageA), p2("TranslateMessage",FakeTranslateMessage), p3("DispatchMessageA",FakeDispatchMessageA);
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["quit"]=quit; }
                        out["recs"]=Recs();
                        osVer=savedOs; quit=savedQuit; foreground=savedFg;
                    }
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",original},{"candidate",candidate}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<all.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    timeMs=savedTime; timeStep=savedStep; frameCounter=savedFrame; area=savedArea; hours=savedHours; region=savedRegion; distWater=savedDistWater; camFwd=savedFwd; wide=savedWide;
    cloudTex=savedCloudTex; fpState=savedFp; std::memcpy(birds,savedBirds.data(),NB); std::memcpy(tbv,savedTbv.data(),NTBV); std::memcpy(tbi,savedTbi.data(),NTBI); std::memcpy(tbc,savedTbc.data(),8);
    credStart=savedCredStart; credGoing=savedCredGoing; menuActive=savedMenu; scrW=savedW; scrH=savedH; quit=savedQuit; osVer=savedOs; foreground=savedFg;
    std::memcpy(sgStore,savedSgStore.data(),NSG); sgCount=savedSgCount;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateOk()}});
}
}
