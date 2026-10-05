#pragma once
// Banco dentro del motor: CControllerConfigManager (58 funciones: asignaciones de teclado/ratón/joystick,
// AffectPadFromKeyBoard/Mouse, Clear*/Set*/Get*, DeleteMatching*, textos de las acciones, Load/SaveSettings,
// ReinitControls). Original (hooks de la clase desbloqueados y desactivados) frente a candidato (activados),
// llamando siempre a la dirección del EXE. Nunca se tocan dispositivos reales:
//  - el gestor es un objeto falso de 0x12E4 bytes con tablas deterministas; SaveSettings, LoadSettings y
//    ReinitControls usan el ControlsManager global (0xB70198), cuyo contenido se sustituye durante la llamada;
//  - los pads (CPad::Pads), los estados de teclado/ratón, padNumber, campos del menú, AllValidWinJoys, la cámara
//    activa y los búferes de texto 0xB7147C..0xB714EE se rellenan con fixtures y se restauran en cada ejecución;
//  - se simulan y trazan CFileMgr::Read/Write/Seek (fichero en memoria), FindPlayerPed/FindPlayerVehicle,
//    GTATranslateShiftKey, WinInput::GetMouseSetUp (EXE: thiscall ret 4; DLL: cdecl con retorno oculto) y el
//    dispositivo DirectInput del joystick (PSGLOBAL(diDevice1) apunta a un objeto falso con vtable trazada).
// Los stubs de fichero/jugador solo trazan en el hilo principal; otros hilos reciben fread/fwrite/fseek reales.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ControllerConfigManager.h"
#include "Pad.h"
#include "FileMgr.h"
#include "MenuManager.h"
#include "WinPlatform.h"
#include "WinInput.h"
#include "../ProbeRuntime.hpp"

namespace CtrlConfig58Live {
inline bool done{}; inline std::ofstream log; inline json trace; inline DWORD mainThread{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class Fn> uintptr_t A(Fn f){ static_assert(sizeof(Fn)>=sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&f,sizeof(a)); return a; }
inline bool InExe(uintptr_t a){ return a>=0x400000&&a<0x1000000; }
inline uintptr_t Body(uintptr_t a){
    for(int k=0;k<8&&*reinterpret_cast<const uint8*>(a)==0xE9;++k){ const uintptr_t t=a+5+*reinterpret_cast<const int32*>(a+1); if(InExe(t))break; a=t; }
    return a;
}
inline bool Main(){ return GetCurrentThreadId()==mainThread; }

// --- fichero en memoria (CFileMgr) ---
inline std::array<uint8,4096> fileData{}; inline size_t fileLen{}, filePos{};
inline std::array<uint8,4096> outData{}; inline size_t outLen{};
inline size_t __cdecl FakeRead(FILESTREAM f,void* buf,size_t size){
    if(!Main())return fread(buf,1,size,f);
    const size_t avail=filePos<fileLen ? fileLen-filePos : 0, n=std::min(size,avail);
    trace.push_back({"read",size,filePos,n}); std::memcpy(buf,fileData.data()+filePos,n); filePos+=n; return n; }
inline size_t __cdecl FakeWrite(FILESTREAM f,const void* buf,size_t size){
    if(!Main())return fwrite(buf,1,size,f);
    trace.push_back({"write",size,Fnv(buf,size)}); for(size_t i=0;i<size&&outLen<outData.size();++i)outData[outLen++]=static_cast<const uint8*>(buf)[i]; return size; }
inline bool __cdecl FakeSeek(FILESTREAM f,long off,int32 origin){
    if(!Main())return fseek(f,off,origin)!=0;
    trace.push_back({"seek",off,origin});
    const long base=origin==0 ? 0 : origin==1 ? long(filePos) : long(fileLen); const long p=base+off; filePos=p<0 ? 0 : size_t(p); return false; }

// --- jugador ---
inline void* fakePedResult{}; inline void* fakeVehResult{}; inline void* realPlayer{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ if(!Main())return static_cast<CPlayerPed*>(realPlayer); trace.push_back({"find_player_ped",id}); return static_cast<CPlayerPed*>(fakePedResult); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote){ if(!Main())return nullptr; trace.push_back({"find_player_vehicle",id,remote}); return static_cast<CVehicle*>(fakeVehResult); }

// --- entrada de Windows / DirectInput ---
inline BOOL __cdecl FakeTranslateShiftKey(RsKeyCodes*){ trace.push_back({"translate_shift_key"}); return TRUE; }
inline std::array<uint8,0x14> mouseSetUp{};
inline CMouseControllerState* __fastcall FakeMouseSetUpExe(void*,void*,CMouseControllerState* out){ trace.push_back({"mouse_setup"}); std::memcpy(out,mouseSetUp.data(),0x14); return out; }
inline CMouseControllerState FakeMouseSetUpDll(){ trace.push_back({"mouse_setup"}); CMouseControllerState s; std::memcpy(&s,mouseSetUp.data(),0x14); return s; }
inline uint32 capsButtons{}; inline HRESULT capsResult{};
inline HRESULT __stdcall FakeGetCaps(void*,DIDEVCAPS* caps){ trace.push_back({"get_caps",caps->dwSize}); caps->dwButtons=capsButtons; return capsResult; }
inline HRESULT __stdcall FakeDevTrap(void*){ trace.push_back({"device_trap"}); return E_FAIL; }
inline std::array<void*,40> devVt{}; inline void* fakeDev[2]{};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,58> fns{{
    {"CControllerConfigManager","Constructor",0x531EE0,0,-1,64},
    {"CControllerConfigManager","LoadSettings",0x530530,0,-1,256},
    {"CControllerConfigManager","SaveSettings",0x52D200,0,-1,64},
    {"CControllerConfigManager","InitDefaultControlConfiguration",0x530640,0,-1,128},
    {"CControllerConfigManager","InitDefaultControlConfigJoyPad",0x530B00,0,-1,256},
    {"CControllerConfigManager","InitDefaultControlConfigMouse",0x52F6F0,0,-1,256},
    {"CControllerConfigManager","InitialiseControllerActionNameArray",0x52D260,0,-1,64},
    {"CControllerConfigManager","ReinitControls",0x531F20,0,-1,128},
    {"CControllerConfigManager","SetMouseButtonAssociatedWithAction",0x52F590,0,-1,256},
    {"CControllerConfigManager","StoreMouseButtonState",0x52DA30,0,-1,256},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonUp",0x531070,0,-1,512},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonDown_DebugStuff",0x52DC10,0,-1,64},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonUp_DebugStuff",0x52DD80,0,-1,64},
    {"CControllerConfigManager","ClearSimButtonPressCheckers",0x52DD90,0,-1,64},
    {"CControllerConfigManager","GetIsKeyboardKeyDown",0x52DDB0,0,-1,1024},
    {"CControllerConfigManager","GetIsKeyboardKeyJustDown",0x52E450,0,-1,1024},
    {"CControllerConfigManager","GetIsMouseButtonDown",0x52EF30,0,-1,256},
    {"CControllerConfigManager","GetIsMouseButtonUp",0x52F020,0,-1,256},
    {"CControllerConfigManager","GetIsMouseButtonJustUp",0x52F110,0,-1,256},
    {"CControllerConfigManager","GetIsKeyBlank",0x52F2A0,0,-1,256},
    {"CControllerConfigManager","GetActionType",0x52F2F0,0,-1,256},
    {"CControllerConfigManager","GetControllerKeyAssociatedWithAction",0x52F4F0,0,-1,256},
    {"CControllerConfigManager","ResetSettingOrder",0x52F5F0,0,-1,512},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonDown_Driving",0x52F7B0,0,-1,512},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonDown_FirstPersonOnly",0x52F9E0,0,-1,256},
    {"CControllerConfigManager","GetControllerSettingTextMouse",0x52F390,0,-1,256},
    {"CControllerConfigManager","GetControllerSettingTextJoystick",0x52F450,0,-1,256},
    {"CControllerConfigManager","GetNumOfSettingsForAction",0x52F4A0,0,-1,256},
    {"CControllerConfigManager","GetIsActionAButtonCombo",0x52F550,0,-1,64},
    {"CControllerConfigManager","GetButtonComboText",0x52F560,0,-1,128},
    {"CControllerConfigManager","GetMouseButtonAssociatedWithAction",0x52F580,0,-1,128},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonDown_ThirdPersonOnly",0x52FA20,0,-1,256},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonDown_FirstAndThirdPersonOnly",0x52FAB0,0,-1,512},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonDown_AllStates",0x52FCA0,0,-1,256},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonDown_VehicleAndThirdPersonOnly",0x52FD20,0,-1,128},
    {"CControllerConfigManager","AffectControllerStateOn_ButtonUp_All_Player_States",0x52FD40,0,-1,128},
    {"CControllerConfigManager","ClearSettingsAssociatedWithAction",0x52FD70,0,-1,256},
    {"CControllerConfigManager","GetControllerSettingTextKeyBoard",0x52FE10,0,-1,1024},
    {"CControllerConfigManager","DeleteMatchingVehicle_3rdPersonControls",0x531BC0,0,-1,256},
    {"CControllerConfigManager","DeleteMatching1rstPersonControls",0x531C20,0,-1,256},
    {"CControllerConfigManager","DeleteMatchingActionInitiators",0x531C90,0,-1,512},
    {"CControllerConfigManager","GetControllerSettingTextWithOrderNumber",0x531E20,0,-1,256},
    {"CControllerConfigManager","GetFirstKeyboardControllerText",0x531E90,0,-1,256},
    {"CControllerConfigManager","GetGxtStringOfCommandKeys",0x5303D0,0,-1,512},
    {"CControllerConfigManager","AffectPadFromMouse",0x5314A0,0,-1,256},
    {"CControllerConfigManager","DeleteMatching1rst3rdPersonControls",0x5318C0,0,-1,256},
    {"CControllerConfigManager","DeleteMatchingVehicleControls",0x5319D0,0,-1,256},
    {"CControllerConfigManager","SetControllerKeyAssociatedWithAction",0x530490,0,-1,256},
    {"CControllerConfigManager","MakeControllerActionsBlank",0x530500,0,-1,64},
    {"CControllerConfigManager","AffectPadFromKeyBoard",0x531140,0,-1,256},
    {"CControllerConfigManager","DeleteMatchingCommonControls",0x531670,0,-1,256},
    {"CControllerConfigManager","DeleteMatching3rdPersonControls",0x531730,0,-1,256},
    {"CControllerConfigManager","UpdateJoyInConfigMenus_ButtonDown",0x52DAB0,0,-1,256},
    {"CControllerConfigManager","UpdateJoyInConfigMenus_ButtonUp",0x52DC20,0,-1,256},
    {"CControllerConfigManager","GetJoyButtonJustUp",0x52D1C0,0,-1,256},
    {"CControllerConfigManager","GetJoyButtonJustDown",0x52D1E0,0,-1,256},
    {"CControllerConfigManager","UpdateJoy_ButtonDown",0x530ED0,0,-1,512},
    {"CControllerConfigManager","UpdateJoyButtonState",0x52F510,0,-1,128},
}};

// --- memoria global tocada: dirección y tamaño ---
struct Region { uintptr_t at; size_t size; };
inline const std::array<Region,13> regions{{
    {0xB70198,0x12E4},  // ControlsManager
    {0xB73458,0x134*2}, // CPad::Pads
    {0xB73190,0x270},   // CPad::NewKeyState
    {0xB72F20,0x270},   // CPad::OldKeyState
    {0xB73400,1},       // CPad::padNumber
    {0xB73404,0x14*3},  // Temp/New/OldMouseControllerState
    {0xBA6818,1},       // FrontEndMenuManager.m_ControlMethod
    {0xBA67A4,1},       // FrontEndMenuManager.m_bMenuActive
    {0xBA67C8,4},       // FrontEndMenuManager.m_nTextLanguage
    {0xC92144,0x20},    // AllValidWinJoys
    {0xB6F081,1},       // TheCamera.m_nActiveCam
    {0xB6F1A8,2},       // TheCamera.m_aCams[0].m_nMode
    {0xB7147C,0x72},    // búferes de texto 0xB7147C / 0xB7149C / 0xB714BC
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ctrlconfig58-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ctrlconfig58-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ctrlconfig58");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainThread=GetCurrentThreadId(); realPlayer=player;
    std::string only; { std::ifstream f(dir/"ctrlconfig58-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,58> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,58> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    std::vector<std::vector<uint8>> saved; for(auto& r:regions)saved.emplace_back(reinterpret_cast<uint8*>(r.at),reinterpret_cast<uint8*>(r.at)+r.size);
    auto restoreAll=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].at),saved[k].data(),regions[k].size); };
    auto& diDevice=PSGLOBAL(diDevice1); auto* const savedDevice=diDevice;

    // cuerpos de la DLL resueltos antes de escribir ningún parche
    const uintptr_t dRead=Body(A(&CFileMgr::Read)), dWrite=Body(A(&CFileMgr::Write)), dSeek=Body(A(&CFileMgr::Seek));
    const uintptr_t dPed=Body(A(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed))), dVeh=Body(A(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)));
    const uintptr_t dShift=Body(A(&GTATranslateShiftKey)), dMouse=Body(A(&WinInput::GetMouseSetUp));
    const std::array<uintptr_t,7> dll{dRead,dWrite,dSeek,dPed,dVeh,dShift,dMouse};
    const std::array<uintptr_t,7> stubs{A(&FakeRead),A(&FakeWrite),A(&FakeSeek),A(&FakeFindPlayerPed),A(&FakeFindPlayerVehicle),A(&FakeTranslateShiftKey),A(&FakeMouseSetUpDll)};
    for(size_t k=0;k<dll.size();++k){ if(InExe(dll[k])||Body(stubs[k])==dll[k]){Write({{"event","error"},{"reason","bad dll body"},{"index",k}});return;}
        for(size_t j=0;j<k;++j)if(dll[j]==dll[k]){Write({{"event","error"},{"reason","duplicate dll body"},{"index",k}});return;} }
    for(auto& v:devVt)v=reinterpret_cast<void*>(&FakeDevTrap); devVt[3]=reinterpret_cast<void*>(&FakeGetCaps); fakeDev[0]=devVt.data();

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch fr(dRead,FakeRead), fre(0x538950,FakeRead), fw(dWrite,FakeWrite), fwe(0x538970,FakeWrite), fs(dSeek,FakeSeek), fse(0x538990,FakeSeek);
        EntryPatch p1(dPed,FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed), p2(dVeh,FakeFindPlayerVehicle), p2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch sk(dShift,FakeTranslateShiftKey), ske(0x747CD0,FakeTranslateShiftKey), ms(dMouse,FakeMouseSetUpDll), mse(0x53F2D0,FakeMouseSetUpExe);
        alignas(16) static std::array<uint8,0x800> fakePed{}; alignas(16) static std::array<uint8,0x40> fakeVeh{};
        auto* const pads=reinterpret_cast<uint8*>(0xB73458);
        auto* const cm=reinterpret_cast<uint8*>(0xB70198);
        auto* const textBufs=reinterpret_cast<uint8*>(0xB7147C);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC0F58u; if(!stt)stt=1; trace=json::array();
                    alignas(16) static std::array<uint8,0x12E4> mgr{}; uint8* const m=mgr.data();
                    auto K=[&](uint8* b,int a,int t)->uint32&{ return *reinterpret_cast<uint32*>(b+0xB70+a*0x20+t*8); };
                    auto O=[&](uint8* b,int a,int t)->uint32&{ return *reinterpret_cast<uint32*>(b+0xB74+a*0x20+t*8); };
                    auto keyPool=[&](int t)->uint32{ const uint32 r=next()%10; if(r==0)return 0x420u; if(r==1)return 0u;
                        if(t==2)return r<7 ? 1+next()%7 : next()%12; if(t==3)return r<7 ? 1+next()%16 : next()%20;
                        if(r<5)return next()%256; if(r<8)return 1000+next()%57; return next()%2000; };
                    auto orderPool=[&]()->uint32{ return next()%8==0 ? next() : next()%6; };
                    auto fillMgr=[&](uint8* b){ for(size_t k=0;k<0xB70;++k)b[k]=uint8(next());
                        for(int a=0;a<59;++a)for(int t=0;t<4;++t){K(b,a,t)=keyPool(t);O(b,a,t)=orderPool();}
                        for(size_t k=0x12D0;k<0x12E0;++k)b[k]=next()%3==0 ? 1 : 0; b[0x12E0]=uint8(next()%2); for(size_t k=0x12E1;k<0x12E4;++k)b[k]=uint8(next()); };
                    auto stateVal=[&]()->int16{ static const int16 vals[]{0,0,0,255,128,-128,1,-1}; return next()%6 ? vals[next()%8] : int16(next()); };
                    auto fillState=[&](uint8* s){ for(int k=0;k<24;++k)reinterpret_cast<int16*>(s)[k]=stateVal(); };
                    auto fillKeys=[&](uint8* s){ for(int k=0;k<0x138;++k)reinterpret_cast<int16*>(s)[k]=next()%5==0 ? int16(next()%2 ? 255 : 0x80) : int16(0); };
                    auto mouseByte=[&]()->uint8{ static const uint8 vals[]{0,0,1,0x80,0xFF}; return vals[next()%5]; };
                    auto fillMouse=[&](uint8* s){ for(int k=0;k<7;++k)s[k]=mouseByte(); s[3]=uint8(next()%2); s[4]=uint8(next()%2); };
                    auto action=[&]()->int{ return next()%10 ? int(next()%59) : int(next()%64); };
                    auto type=[&]()->int{ return int(next()%4); };
                    auto Txt=[&](const GxtChar* p)->json{ if(!p)return nullptr; const uintptr_t u=uintptr_t(p); json r=json::array();
                        if(u>=0xB7147C&&u<0xB714EE)r.push_back(u-0xB7147C); else r.push_back("text");
                        std::vector<uint8> s; for(auto* q=p;*q&&s.size()<200;++q)s.push_back(*q); r.push_back(s); return r; };

                    // estado global de fixture
                    restoreAll(); diDevice=savedDevice;
                    fillMgr(m);
                    for(int pd=0;pd<2;++pd){ uint8* p=pads+pd*0x134; for(int k=0x78;k<0x108;k+=2)*reinterpret_cast<int16*>(p+k)=stateVal();
                        *reinterpret_cast<uint16*>(p+0x10E)=next()%4==0 ? uint16(next()) : uint16(0); }
                    fillKeys(reinterpret_cast<uint8*>(0xB73190)); fillKeys(reinterpret_cast<uint8*>(0xB72F20));
                    *reinterpret_cast<uint8*>(0xB73400)=next()%5==0 ? uint8(1) : uint8(0);
                    for(int s=0;s<3;++s)fillMouse(reinterpret_cast<uint8*>(0xB73404+s*0x14));
                    *reinterpret_cast<uint8*>(0xBA6818)=next()%10==0 ? uint8(2) : uint8(next()%2);
                    *reinterpret_cast<uint8*>(0xBA67A4)=next()%4==0 ? uint8(1) : uint8(0);
                    *reinterpret_cast<int32*>(0xBA67C8)=int32(next()%5);
                    { auto* j=reinterpret_cast<uint8*>(0xC92144); for(int k=0;k<0x20;++k)j[k]=uint8(next());
                      if(next()%2){*reinterpret_cast<uint32*>(j+8)=0x3427;*reinterpret_cast<uint32*>(j+0xC)=0x1190;}
                      else if(next()%2){*reinterpret_cast<uint32*>(j+8)=0x3427;} j[4]=next()%3 ? uint8(1) : uint8(0); }
                    { static const int16 modes[]{16,7,8,51,34,46,4,18,53,1}; *reinterpret_cast<uint8*>(0xB6F081)=0; *reinterpret_cast<int16*>(0xB6F1A8)=modes[next()%10]; }
                    for(int k=0;k<0x72;++k)textBufs[k]=uint8(0x41+next()%26);
                    std::memset(fakePed.data(),0,fakePed.size()); *reinterpret_cast<int32*>(fakePed.data()+0x530)=next()%2 ? 50 : int32(next()%60);
                    fakePedResult=next()%6 ? fakePed.data() : nullptr; fakeVehResult=next()%3 ? fakeVeh.data() : nullptr;
                    fileLen=filePos=outLen=0;

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr; bool padsOut=false, cmOut=false, mouseOut=false;

                    // botón que casi siempre coincide con alguna asignación del gestor
                    auto pickButton=[&](uint8* b,int t)->uint32{ return next()%4 ? K(b,action()%59,t) : keyPool(t); };

                    if(nm=="Constructor"){ std::memset(m,0xCD,0x12E4); auto* r=reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(m); ret=intptr_t(r-m); }
                    else if(nm=="LoadSettings"||nm=="SaveSettings"){
                        fillMgr(cm); cmOut=true;
                        if(nm=="SaveSettings"){ FILESTREAM file=next()%8==0 ? nullptr : reinterpret_cast<FILESTREAM>(0x1234);
                            reinterpret_cast<void(__thiscall*)(void*,FILESTREAM)>(a)(m,file);
                            out["written"]=ReversingProbe::Bytes(outData.data(),outLen); }
                        else { const uint32 variant=next()%7; FILESTREAM file=reinterpret_cast<FILESTREAM>(0x1234);
                            if(variant==0)file=nullptr;
                            else if(variant==1){ const char* hdr="THIS FILE IS NOT VALID YET"; fileLen=29+next()%40; for(size_t k=0;k<fileLen;++k)fileData[k]=uint8(next()); std::memcpy(fileData.data(),hdr,26+next()%2); }
                            else { fileLen=4+4*59*12; size_t p=0; const int32 ver=variant==2 ? int32(next()%6)-int32(next()%2)*100 : (next()%3 ? 6 : 6+int32(next()%100));
                                std::memcpy(&fileData[p],&ver,4); p+=4;
                                for(int t=0;t<4;++t)for(int ac=0;ac<59;++ac){ uint32 id=uint32(ac), key=keyPool(t), ord=orderPool(); std::memcpy(&fileData[p],&id,4); std::memcpy(&fileData[p+4],&key,4); std::memcpy(&fileData[p+8],&ord,4); p+=12; }
                                if(variant==5){ const size_t e=next()%(4*59); *reinterpret_cast<uint32*>(&fileData[4+e*12])^=1u+next()%7; }
                                if(variant==6) fileLen=29+next()%(fileLen-29); }
                            ret=reinterpret_cast<bool(__thiscall*)(void*,FILESTREAM)>(a)(m,file); out["pos"]=filePos; }
                    }
                    else if(nm=="InitDefaultControlConfiguration"||nm=="InitialiseControllerActionNameArray"||nm=="MakeControllerActionsBlank"||nm=="ClearSimButtonPressCheckers")
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(m);
                    else if(nm=="UpdateJoyButtonState") reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(m,int32(next()%3));
                    else if(nm=="InitDefaultControlConfigJoyPad"){ const uint32 n=next()%5 ? next()%21 : next(); reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(m,n); }
                    else if(nm=="InitDefaultControlConfigMouse"){ alignas(4) std::array<uint8,0x14> st{}; for(auto& b:st)b=uint8(next()); for(int k=0;k<3;++k)st[k]=next()%2 ? mouseByte() : 0; st[3]=uint8(next()%2); st[4]=uint8(next()%2);
                        reinterpret_cast<void(__thiscall*)(void*,const void*,bool)>(a)(m,st.data(),next()%2!=0); }
                    else if(nm=="ReinitControls"){
                        fillMgr(cm); cmOut=true; for(auto& b:mouseSetUp)b=uint8(next()); for(int k=0;k<3;++k)mouseSetUp[k]=next()%2 ? mouseByte() : 0; mouseSetUp[3]=uint8(next()%2); mouseSetUp[4]=uint8(next()%2);
                        capsButtons=next()%5 ? next()%21 : next(); capsResult=next()%4 ? S_OK : E_FAIL; diDevice=reinterpret_cast<LPDIRECTINPUTDEVICE8>(fakeDev);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(m); diDevice=savedDevice; }
                    else if(nm=="SetMouseButtonAssociatedWithAction") reinterpret_cast<void(__thiscall*)(void*,int,uint32)>(a)(m,action()%59,keyPool(2));
                    else if(nm=="SetControllerKeyAssociatedWithAction"){ const int t=type(); reinterpret_cast<void(__thiscall*)(void*,int,uint32,int)>(a)(m,action()%59,keyPool(t),t); }
                    else if(nm=="StoreMouseButtonState"){ const uint32 b=next()%5 ? next()%9 : next(); reinterpret_cast<void(__thiscall*)(void*,uint32,bool)>(a)(m,b,next()%2!=0); mouseOut=true; }
                    else if(nm=="AffectControllerStateOn_ButtonUp"||nm=="UpdateJoy_ButtonDown"){ const int t=type(); reinterpret_cast<void(__thiscall*)(void*,uint32,int)>(a)(m,pickButton(m,t),t); padsOut=true; }
                    else if(nm=="AffectControllerStateOn_ButtonDown_DebugStuff"||nm=="AffectControllerStateOn_ButtonUp_DebugStuff"){ const int t=type(); reinterpret_cast<void(__thiscall*)(void*,uint32,int)>(a)(m,keyPool(t),t); }
                    else if(nm=="GetIsKeyboardKeyDown"||nm=="GetIsKeyboardKeyJustDown"){ const uint32 k=next()%8 ? keyPool(0) : uint32(next()%1100);
                        ret=reinterpret_cast<bool(__thiscall*)(void*,uint32)>(a)(m,k); }
                    else if(nm=="GetIsMouseButtonDown"||nm=="GetIsMouseButtonUp"||nm=="GetIsMouseButtonJustUp"){ const uint32 k=next()%6 ? next()%9 : next(); ret=reinterpret_cast<bool(__thiscall*)(void*,uint32)>(a)(m,k); }
                    else if(nm=="GetIsKeyBlank"){ const int t=next()%8 ? type() : int(next()%8); const uint32 k=next()%3 ? keyPool(t&3) : (next()%2 ? 0x420u : 0u); ret=reinterpret_cast<bool(__thiscall*)(void*,uint32,int)>(a)(m,k,t); }
                    else if(nm=="GetActionType"){ const int ac=next()%8 ? int(next()%59) : int(next()%2 ? next()%80 : next()); ret=reinterpret_cast<int(__thiscall*)(void*,int)>(a)(m,ac); }
                    else if(nm=="GetControllerKeyAssociatedWithAction"){ const int ac=action()%59, t=type(); ret=reinterpret_cast<uint32(__thiscall*)(void*,int,int)>(a)(m,ac,t); }
                    else if(nm=="GetMouseButtonAssociatedWithAction") ret=reinterpret_cast<uint32(__thiscall*)(void*,int)>(a)(m,action()%59);
                    else if(nm=="GetNumOfSettingsForAction") ret=reinterpret_cast<uint32(__thiscall*)(void*,int)>(a)(m,action()%59);
                    else if(nm=="ResetSettingOrder"){ const int ac=action()%59; if(next()%3==0)for(int t=0;t<4;++t)O(m,ac,t)=next()%6; reinterpret_cast<void(__thiscall*)(void*,int)>(a)(m,ac); }
                    else if(nm.rfind("AffectControllerStateOn_ButtonDown_",0)==0||nm=="AffectControllerStateOn_ButtonUp_All_Player_States"){
                        alignas(4) std::array<uint8,0x30> st{}; fillState(st.data()); const int t=type();
                        reinterpret_cast<void(__thiscall*)(void*,uint32,int,void*)>(a)(m,pickButton(m,t),t,st.data()); out["state"]=ReversingProbe::Bytes(st.data(),st.size()); }
                    else if(nm=="GetControllerSettingTextMouse"||nm=="GetControllerSettingTextJoystick"||nm=="GetButtonComboText"||nm=="GetFirstKeyboardControllerText"){
                        const int ac=nm=="GetButtonComboText" ? (next()%3==0 ? 36 : int(next()%59)) : action()%59; ret=Txt(reinterpret_cast<const GxtChar*(__thiscall*)(void*,int)>(a)(m,ac)); }
                    else if(nm=="GetIsActionAButtonCombo") ret=reinterpret_cast<bool(__thiscall*)(void*,int)>(a)(m,action());
                    else if(nm=="GetControllerSettingTextKeyBoard"){ const int ac=action()%59, t=next()%6 ? int(next()%2) : type();
                        if(next()%4==0)K(m,ac,t)=next()%3 ? uint32(next()%256) : uint32(next()%2 ? '^' : 178);
                        ret=Txt(reinterpret_cast<const GxtChar*(__thiscall*)(void*,int,int)>(a)(m,ac,t)); }
                    else if(nm=="GetControllerSettingTextWithOrderNumber"){ const int ac=action()%59; const uint32 ord=next()%6; ret=Txt(reinterpret_cast<const GxtChar*(__thiscall*)(void*,int,uint32)>(a)(m,ac,ord)); }
                    else if(nm=="GetGxtStringOfCommandKeys"){ const int ac=action()%59; K(m,ac,0)=next()%256; /* siempre hay texto de teclado: sin él el EXE hace strlen(nullptr) */
                        std::array<GxtChar,256> dst{}; for(auto& b:dst)b=GxtChar(0x61+next()%26); const uint16 len=uint16(next()%4 ? 1+next()%64 : next()%300);
                        reinterpret_cast<void(__thiscall*)(void*,int,GxtChar*,uint32)>(a)(m,ac,dst.data(),len); out["dst"]=ReversingProbe::Bytes(dst.data(),dst.size()); }
                    else if(nm=="ClearSettingsAssociatedWithAction"){ const int t=next()%10 ? type() : 4+int(next()%4); reinterpret_cast<void(__thiscall*)(void*,int,int)>(a)(m,action()%59,t); }
                    else if(nm.rfind("DeleteMatching",0)==0){ const int t=next()%12 ? type() : 4+int(next()%4); const int ac=next()%10 ? int(next()%59) : int(next()%64);
                        const uint32 b=pickButton(m,t&3); reinterpret_cast<void(__thiscall*)(void*,int,uint32,int)>(a)(m,ac,b,t); }
                    else if(nm=="AffectPadFromMouse"||nm=="AffectPadFromKeyBoard"){ reinterpret_cast<void(__thiscall*)(void*)>(a)(m); padsOut=true; }
                    else if(nm=="UpdateJoyInConfigMenus_ButtonDown"||nm=="UpdateJoyInConfigMenus_ButtonUp"){ const uint32 b=next()%6 ? next()%18 : next(); const int32 pn=int32(next()%2);
                        reinterpret_cast<void(__thiscall*)(void*,uint32,int32)>(a)(m,b,pn); padsOut=true; }
                    else if(nm=="GetJoyButtonJustUp"||nm=="GetJoyButtonJustDown"){
                        for(int k=0;k<17;++k){ m[0x34+k]=next()%3==0 ? 0x80 : 0; m[0x144+k]=next()%3==0 ? 0x80 : 0; }
                        ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(m); }
                    else { out["unknown"]=true; }

                    out["mgr_actions"]=ReversingProbe::Bytes(m+0xB5C,0x12E4-0xB5C); out["mgr_rest"]=Fnv(m,0xB5C);
                    if(nm=="Constructor"||nm=="InitialiseControllerActionNameArray")out["names"]=ReversingProbe::Bytes(m+0x224,0x938);
                    if(cmOut){ out["cm_actions"]=ReversingProbe::Bytes(cm+0xB70,0x774); out["cm_rest"]=Fnv(cm,0xB70); }
                    if(padsOut)out["pads"]=ReversingProbe::Bytes(pads,0x134*2);
                    if(mouseOut)out["temp_mouse"]=ReversingProbe::Bytes(reinterpret_cast<uint8*>(0xB73404),0x14);
                    out["text"]=ReversingProbe::Bytes(textBufs,0x72);
                    out["ret"]=ret; out["trace"]=trace;
                    restoreAll(); diDevice=savedDevice;
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&!original.contains("unknown");
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
    restoreAll(); diDevice=savedDevice;
    bool restored=diDevice==savedDevice; for(size_t k=0;k<regions.size();++k)restored&=std::memcmp(reinterpret_cast<void*>(regions[k].at),saved[k].data(),regions[k].size)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
