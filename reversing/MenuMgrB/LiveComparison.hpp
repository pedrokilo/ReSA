#pragma once
// Banco dentro del motor: CMenuManager, grupo B (entrada; 15 funciones).
//   AdditionalOptionInput, CentreMousePointer, CheckCodesForControls, CheckForMenuClosing, CheckFrontEndDownInput/LeftInput/
//   RightInput/UpInput, CheckHover, CheckRedefineControlInput, CheckSliderMovement, ProcessUserInput, RedefineScreenUserInput,
//   ScrollRadioStations, UserInput.
// Original (hooks de estas 15 desactivados) frente a candidato (activados) con fixtures deterministas por caso.
// Se trabaja sobre un CMenuManager FALSO (0x1B78 bytes con guardas de 64 bytes, nunca el singleton). El EXE y la DLL leen/escriben
// ademas por direccion absoluta unos pocos campos del singleton real (0xBA6748: m_SlideLeft/RightMoveTime, m_LastActionTime,
// m_LastTransitionTime, m_bDrawingMap, m_nMouseHoverScreen, bMouseHoverInitialised, m_StatusDisablePlayerControls y, a traves de
// CRadar, m_fMapZoom/m_vMapOrigin): se instantanea el singleton completo, se rellenan esos campos por caso, se compara el
// singleton entero (salvo la posicion de raton de Windows 0xE0..0xE7, que escribe WndProc) y se restaura al terminar.
// Se fijan por caso, se comparan y se restauran: pad 0/1, teclado (nuevo/viejo), raton (nuevo/viejo), historial de sticks
// analogicos, reloj, RsGlobal.maximumWidth/Height, lastMousePos, estado de CRadar (origen/rango/seno/coseno), scripts, replay,
// camara (sensibilidad del raton, ms_lodDistScale), ranuras de guardado, PreviousBriefs, tabla aScreens (solo lectura) y estaticos
// de UserInput (0x8CE004/5).
// Se simulan y trazan: audio (ReportFrontendAudioEvent, Service, StopRadio, RetuneRadio, volumenes, IsRadioRetuneInProgress,
// IsCutsceneTrackActive), CText::Get, CFont::GetStringWidth, CGamma::SetGamma, CMenuSystem (CreateNewMenu, InsertMenu,
// CheckForAccept/Selected, SetActiveMenuItem, SwitchOffMenu), CRadar (ClearBlip, SetCoordBlip, SetBlipSprite),
// CControllerConfigManager, CMenuManager (SaveSettings, SwitchToNewScreen, GetNumberOfMenuOptions, ProcessMenuOptions,
// DrawControllerBound, SaveStatsToFile, Initialise, LoadAllTextures, UnloadTextures), CPad::Clear/ClearKeyBoardHistory/
// ClearMouseHistory, DoRWStuff*, DIReleaseMouse, InitialiseMouse, IsVideoModeExclusive, CTimer::EndUserPause/Update,
// CCamera::SetFadeColour/Fade/ProcessFade y FindPlayerPed (ped falso).
// CentreMousePointer llama de verdad a ClientToScreen/SetCursorPos (el cursor se restaura al terminar).
// Se compara: retorno, bytes del objeto falso (con guardas), singleton real, memoria global tocada y la traza.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MenuManager.h"
#include "MenuSystem.h"
#include "ControllerConfigManager.h"
#include "Radar.h"
#include "GenericGameStorage.h"
#include "VideoMode.h"
#include "Gamma.h"
#include "app/app.h"
#include "reversiblebugfixes/Bugs.hpp"
#include "extensions/Configs/FastLoader.hpp"
#include "../ProbeRuntime.hpp"

namespace MenuMgrBLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- objeto falso: [guarda][CMenuManager][guarda] ---
constexpr size_t GUARD=64, OBJ=sizeof(CMenuManager);
static_assert(sizeof(CMenuManager)==0x1B78);
alignas(16) inline std::array<uint8,GUARD+OBJ+GUARD> raw{};
inline uint8* OB() { return raw.data()+GUARD; }
inline CMenuManager* Obj() { return reinterpret_cast<CMenuManager*>(OB()); }
inline CMenuManager* Real() { return &FrontEndMenuManager; }
// --- jugador falso (solo se leen m_nActiveWeaponSlot 0x718 y las armas 0x5A0 + 0x1C*i) ---
alignas(16) inline std::array<uint8,sizeof(CPlayerPed)+64> pedRaw{};
inline CPlayerPed* FakePed() { return reinterpret_cast<CPlayerPed*>(pedRaw.data()); }

inline json Tag(const void* p) {
    if(!p)return nullptr;
    const auto* b=static_cast<const uint8*>(p);
    if(b>=OB()&&b<OB()+OBJ)return json::array({"obj",int(b-OB())});
    if(p==&FrontEndMenuManager)return "singleton"; if(p==&AudioEngine)return "audio"; if(p==&gamma)return "gamma";
    if(p==&ControlsManager)return "controls"; if(p==&TheCamera)return "camera"; if(p==&TheText)return "text";
    return "other";
}
inline json Txt(const void* p){
    if(!p)return nullptr;
    auto* s=static_cast<const uint8*>(p); size_t n=0; while(n<512&&s[n])++n;
    bool ascii=true; for(size_t i=0;i<n;++i)ascii&=s[i]<0x80;
    if(ascii)return std::string(reinterpret_cast<const char*>(s),n);
    return ReversingProbe::Bytes(s,n);
}

// --- mandos por caso de los stubs ---
inline uint32 numOptions{}; inline int cancelMode{-1};
inline bool retuneBusy{}, cutsceneActive{}, videoExclusive{};
inline MenuId createRet{}; inline int8 acceptRet{-1}, selectedRet{-1}; inline int32 blipRet{};
inline uint32 joyRet{}; inline bool comboRet{}; inline int assocMode{}; inline int32 curKey{};

// --- audio ---
inline void __fastcall FakeReportFrontend(CAudioEngine* self,void*,int32 ev,float vol,float speed){ trace.push_back({"frontend_audio",self==&AudioEngine,ev,F(vol),F(speed)}); }
inline void __fastcall FakeAudioService(CAudioEngine* a,void*){ trace.push_back({"audio_service",a==&AudioEngine}); }
inline void __fastcall FakeStopRadio(CAudioEngine* a,void*,void* settings,bool during){ trace.push_back({"stop_radio",a==&AudioEngine,uint32(uintptr_t(settings)),during}); }
inline bool __fastcall FakeRetuneBusy(CAudioEngine* a,void*){ trace.push_back({"retune_in_progress",a==&AudioEngine}); return retuneBusy; }
inline bool __cdecl FakeCutsceneActive(){ trace.push_back({"cutscene_track_active"}); return cutsceneActive; }
inline void __fastcall FakeRetune(CAudioEngine* a,void*,int8 id){ trace.push_back({"retune_radio",a==&AudioEngine,int(id)}); }
inline void __fastcall FakeMusicVol(CAudioEngine* a,void*,int8 v){ trace.push_back({"music_volume",a==&AudioEngine,int(v)}); }
inline void __fastcall FakeEffectsVol(CAudioEngine* a,void*,int8 v){ trace.push_back({"effects_volume",a==&AudioEngine,int(v)}); }
inline void __fastcall FakeSetGamma(CGamma* g,void*,float v,bool fade){ trace.push_back({"gamma",g==&gamma,F(v),fade}); }
// --- texto / fuente ---
inline float __cdecl FakeGetStringWidth(const GxtChar* s,bool full,bool script){ trace.push_back({"string_width",Txt(s),full,script}); return 0.0f; }
inline const GxtChar* __fastcall FakeTextGet(CText* self,void*,const char* key){ trace.push_back({"text_get",self==&TheText,Txt(key)}); return reinterpret_cast<const GxtChar*>(key); }
// --- CMenuManager (this = objeto falso) ---
inline void __fastcall FakeSaveSettings(CMenuManager* m,void*){ trace.push_back({"save_settings",Tag(m)}); }
inline void __fastcall FakeSaveStats(CMenuManager* m,void*){ trace.push_back({"save_stats",Tag(m)}); }
inline void __fastcall FakeInitialise(CMenuManager* m,void*){ trace.push_back({"initialise",Tag(m)}); }
inline void __fastcall FakeLoadAllTextures(CMenuManager* m,void*){ trace.push_back({"load_all_textures",Tag(m)}); }
inline void __fastcall FakeUnloadTextures(CMenuManager* m,void*){ trace.push_back({"unload_textures",Tag(m)}); }
inline void __fastcall FakeSwitchScreen(CMenuManager* m,void*,int32 s){ trace.push_back({"switch_screen",Tag(m),int(int8(s))}); }
inline uint32 __fastcall FakeNumOptions(CMenuManager* m,void*){ trace.push_back({"num_options",Tag(m)}); return numOptions; }
inline void __fastcall FakeProcessMenuOptions(CMenuManager* m,void*,int32 lr,bool* cancel,int32 accept){
    trace.push_back({"process_menu_options",Tag(m),int(int8(lr)),(accept&0xFF)!=0,*reinterpret_cast<uint8*>(cancel)});
    if(cancelMode>=0)*cancel=cancelMode!=0; }
inline void __fastcall FakeDrawControllerBound(CMenuManager* m,void*,uint16 v,bool opp){ trace.push_back({"draw_controller_bound",Tag(m),int(v),opp}); }
// --- CMenuSystem ---
inline MenuId __cdecl FakeCreateNewMenu(CMenuSystem::eMenuType type,const char* title,float x,float y,float w,uint8 cols,bool inter,bool back,eFontAlignment al){
    trace.push_back({"create_new_menu",int(type),Txt(title),F(x),F(y),F(w),cols,inter,back,int(al)}); return createRet; }
inline void __cdecl FakeInsertMenu(MenuId id,uint8 col,const char* h,const char* r0,const char* r1,const char* r2,const char* r3,const char* r4,const char* r5,
                                    const char* r6,const char* r7,const char* r8,const char* r9,const char* r10,const char* r11){
    trace.push_back({"insert_menu",id,col,Txt(h),Txt(r0),Txt(r1),Txt(r2),Txt(r3),Txt(r4),Txt(r5),Txt(r6),Txt(r7),Txt(r8),Txt(r9),Txt(r10),Txt(r11)}); }
inline int8 __cdecl FakeCheckForAccept(MenuId id){ trace.push_back({"check_accept",id}); return acceptRet; }
inline int8 __cdecl FakeCheckForSelected(MenuId id){ trace.push_back({"check_selected",id}); return selectedRet; }
inline void __cdecl FakeSetActiveMenuItem(MenuId id,int8 item){ trace.push_back({"set_active_item",id,int(item)}); }
inline void __cdecl FakeSwitchOffMenu(MenuId id){ trace.push_back({"switch_off_menu",id}); }
// --- CRadar ---
inline void __cdecl FakeClearBlip(int32 b){ trace.push_back({"clear_blip",b}); }
inline int32 __cdecl FakeSetCoordBlip(int32 type,CVector p,int32 color,int32 display,const char* name){ trace.push_back({"set_coord_blip",type,F(p.x),F(p.y),F(p.z),color,display,Txt(name)}); return blipRet; }
inline void __cdecl FakeSetBlipSprite(int32 b,int32 sprite){ trace.push_back({"set_blip_sprite",b,sprite}); }
// --- CControllerConfigManager (this = ControlsManager) ---
inline int32 __fastcall FakeGetKeyAssoc(CControllerConfigManager* c,void*,int32 action,int32 type){
    const int32 r= assocMode==0 ? 1056 : assocMode==1 ? curKey : (curKey^1);
    trace.push_back({"get_key_assoc",c==&ControlsManager,action,type,r}); return r; }
inline bool __fastcall FakeIsCombo(CControllerConfigManager* c,void*,int32 action){ trace.push_back({"is_combo",c==&ControlsManager,action}); return comboRet; }
inline void __fastcall FakeClearAssoc(CControllerConfigManager* c,void*,int32 action,int32 type){ trace.push_back({"clear_assoc",c==&ControlsManager,action,type}); }
inline void __fastcall FakeDeleteMatching(CControllerConfigManager* c,void*,int32 action,int32 button,int32 type){ trace.push_back({"delete_matching",c==&ControlsManager,action,button,type}); }
inline void __fastcall FakeSetAssoc(CControllerConfigManager* c,void*,int32 action,int32 button,int32 type){ trace.push_back({"set_assoc",c==&ControlsManager,action,button,type}); }
inline uint32 __fastcall FakeJoyJustDown(CControllerConfigManager* c,void*){ trace.push_back({"joy_just_down",c==&ControlsManager}); return joyRet; }
// --- pad / entrada ---
inline void __fastcall FakePadClear(CPad* p,void*,bool a,bool b){ trace.push_back({"pad_clear",int(p-&CPad::Pads[0]),a,b}); }
inline void __cdecl FakeClearKeyboardHistory(){ trace.push_back({"clear_keyboard_history"}); }
inline void __cdecl FakeClearMouseHistory(){ trace.push_back({"clear_mouse_history"}); }
inline bool __cdecl FakeVideoExclusive(){ trace.push_back({"video_mode_exclusive"}); return videoExclusive; }
inline void* __cdecl FakeDIReleaseMouse(){ trace.push_back({"di_release_mouse"}); return nullptr; }
// La DLL con FIX_BUGS llama InitialiseMouse(false) al cerrar el menu; el EXE llama InitialiseMouse(true): el argumento se enmascara
// en la rama de cierre (m_bMenuActive falso en el objeto falso).
inline void __cdecl FakeInitialiseMouse(bool excl){ trace.push_back({"initialise_mouse",Obj()->m_bMenuActive ? json(excl) : json("masked")}); }
// --- varios ---
inline bool __cdecl FakeStartOfFrame(int16 a,int16 b,int16 c,int16 d,int16 e,int16 f,int16 g){ trace.push_back({"rw_start_of_frame",a,b,c,d,e,f,g}); return true; }
inline void __cdecl FakeEndOfFrame(){ trace.push_back({"rw_end_of_frame"}); }
inline void __cdecl FakeEndUserPause(){ trace.push_back({"timer_end_user_pause"}); }
inline void __cdecl FakeTimerUpdate(){ trace.push_back({"timer_update"}); }
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); return FakePed(); }
inline void __fastcall FakeSetFadeColour(CCamera* c,void*,uint8 r,uint8 g,uint8 b){ trace.push_back({"camera_fade_colour",c==&TheCamera,r,g,b}); }
inline void __fastcall FakeFade(CCamera* c,void*,float d,uint16 flag){ trace.push_back({"camera_fade",c==&TheCamera,F(d),int(flag)}); }
inline void __fastcall FakeProcessFade(CCamera* c,void*){ trace.push_back({"camera_process_fade",c==&TheCamera}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,15> fns{{
    {"CMenuManager","AdditionalOptionInput",0x5773D0,0,-1,1024},{"CMenuManager","CentreMousePointer",0x57C520,0,-1,64},
    {"CMenuManager","CheckCodesForControls",0x57DB20,0,-1,1024},{"CMenuManager","CheckForMenuClosing",0x576B70,0,-1,1024},
    {"CMenuManager","CheckFrontEndDownInput",0x5738B0,0,-1,256},{"CMenuManager","CheckFrontEndLeftInput",0x573920,0,-1,256},
    {"CMenuManager","CheckFrontEndRightInput",0x573990,0,-1,256},{"CMenuManager","CheckFrontEndUpInput",0x573840,0,-1,256},
    {"CMenuManager","CheckHover",0x57C4F0,0,-1,512},{"CMenuManager","CheckRedefineControlInput",0x57E4D0,0,-1,1024},
    {"CMenuManager","CheckSliderMovement",0x573440,0,-1,512},{"CMenuManager","ProcessUserInput",0x57B480,0,-1,1024},
    {"CMenuManager","RedefineScreenUserInput",0x57EF50,0,-1,1024},{"CMenuManager","ScrollRadioStations",0x573A00,0,-1,256},
    {"CMenuManager","UserInput",0x57FD70,0,-1,2048},
}};

// bloque de memoria global instantaneado/restaurado/comparado
struct Blob { const char* name; uintptr_t addr; size_t n; std::vector<uint8> saved; bool compare; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"menumgrb-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"menumgrb-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("menumgrb");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"menumgrb-only.txt"); if(f)std::getline(f,only); }

    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(size_t f=0;f<fns.size();++f){ auto* h=FindHook(fns[f].cls,fns[f].name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} hooks.push_back(h); }
    std::vector<char> enabled(hooks.size()),locked(hooks.size());
    for(size_t k=0;k<hooks.size();++k){enabled[k]=hooks[k]->Hooked();locked[k]=hooks[k]->Locked();hooks[k]->LockState(false);}
    Write({{"event","player_ready"},{"generic_frame_rate_bugfix",bool(notsa::bugfixes::GenericFrameRate)},
           {"fix_bugs",
#ifdef FIX_BUGS
            true
#else
            false
#endif
           }});

    // --- listas de (pantalla, elemento) para sesgar los fixtures ---
    std::vector<std::pair<int,int>> sliderList, optionList;
    for(int s=0;s<SCREEN_COUNT;++s)for(int i=0;i<12;++i){
        const auto& it=aScreens[s].m_aItems[i];
        const int a=it.m_nActionType;
        if(a==MENU_ACTION_BRIGHTNESS||a==MENU_ACTION_RADIO_VOL||a==MENU_ACTION_SFX_VOL||a==MENU_ACTION_DRAW_DIST||a==MENU_ACTION_MOUSE_SENS)sliderList.push_back({s,i});
        if(it.m_nType==TI_OPTION)optionList.push_back({s,i});
    }

    // --- estado global guardado ---
    constexpr size_t NKEY=sizeof(CKeyboardState), NMOUSE=sizeof(CMouseControllerState), NPAD=sizeof(CPad)*2;
    constexpr size_t NBRIEF=sizeof(CMessages::PreviousBriefs);
    std::vector<Blob> blobs;
    auto AddBlob=[&](const char* name,void* p,size_t n,bool compare=true){ blobs.push_back({name,uintptr_t(p),n,std::vector<uint8>(static_cast<uint8*>(p),static_cast<uint8*>(p)+n),compare}); };
    AddBlob("pads",&CPad::Pads[0],NPAD); AddBlob("new_keys",&CPad::NewKeyState,NKEY); AddBlob("old_keys",&CPad::OldKeyState,NKEY);
    AddBlob("new_mouse",&CPad::NewMouseControllerState,NMOUSE); AddBlob("old_mouse",&CPad::OldMouseControllerState,NMOUSE);
    AddBlob("sticks",reinterpret_cast<void*>(0xB736F0),16); AddBlob("timer",reinterpret_cast<void*>(0xB7CB7C),12);
    AddBlob("rsglobal",&RsGlobal.maximumWidth,8); AddBlob("last_mouse",&PSGLOBAL(lastMousePos),sizeof(RwV2d));
    AddBlob("old_option",reinterpret_cast<void*>(0x8CE004),2); AddBlob("camera_accel_vertical",reinterpret_cast<void*>(0xB6EC18),4,false); /* FIX_BUGS: la DLL tambien escribe la sensibilidad vertical (el EXE no) */ AddBlob("camera_accel_horizontal",reinterpret_cast<void*>(0xB6EC1C),4);
    AddBlob("lod_scale",reinterpret_cast<void*>(0x8CD800),4); AddBlob("min_mouse_accel",reinterpret_cast<void*>(0xBA672C),4);
    AddBlob("slots",&CGenericGameStorage::ms_Slots,sizeof(CGenericGameStorage::ms_Slots)); AddBlob("briefs",&CMessages::PreviousBriefs,NBRIEF);
    AddBlob("scripts",reinterpret_cast<void*>(0xA444A1),4); AddBlob("replay",reinterpret_cast<void*>(0xA43088),1);
    AddBlob("widescreen",reinterpret_cast<void*>(0xB6F065),1); AddBlob("cheat",reinterpret_cast<void*>(0x96918C),1);
    AddBlob("radar_origin",reinterpret_cast<void*>(0xBAA248),8); AddBlob("radar_sincos",reinterpret_cast<void*>(0xBA8308),8);
    AddBlob("radar_range",reinterpret_cast<void*>(0xBA8314),4); AddBlob("foreground",reinterpret_cast<void*>(0x8D621C),4);
    AddBlob("screens",&aScreens[0],sizeof(tMenuScreen)*SCREEN_COUNT);
    const std::vector<uint8> savedReal(reinterpret_cast<uint8*>(Real()),reinterpret_cast<uint8*>(Real())+OBJ);
    const bool savedTried=g_FastLoaderConfig.TriedLoadingSaveGame;
    POINT savedCursor{}; GetCursorPos(&savedCursor);
    const auto cursorBefore=savedCursor;
    auto restoreBlobs=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.addr),b.saved.data(),b.n); };
    auto restoreReal=[&]{ auto* r=reinterpret_cast<uint8*>(Real()); for(size_t i=0;i<OBJ;++i) if(i<0xE0||i>=0xE8) r[i]=savedReal[i]; }; // 0xE0..0xE7 = posicion de raton de Windows (la escribe WndProc)
    auto realHash=[&]{ std::vector<uint8> c(reinterpret_cast<uint8*>(Real()),reinterpret_cast<uint8*>(Real())+OBJ); std::memset(c.data()+0xE0,0,8); return Fnv(c.data(),c.size()); };

    g_FastLoaderConfig.TriedLoadingSaveGame=true; // ShouldLoadSaveGame() falso: la DLL llama a AudioEngine.Service() como el EXE

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto P=[&](uintptr_t dll,uintptr_t exeAddr,auto fn){ if(dll)patches.push_back(std::make_unique<EntryPatch>(dll,fn)); if(exeAddr)patches.push_back(std::make_unique<EntryPatch>(exeAddr,fn)); };
        // audio
        P(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),0x506EA0,FakeReportFrontend);
        P(bit_cast<uintptr_t>(&CAudioEngine::Service),0x507750,FakeAudioService);
        P(bit_cast<uintptr_t>(&CAudioEngine::StopRadio),0x506F70,FakeStopRadio);
        P(bit_cast<uintptr_t>(&CAudioEngine::IsRadioRetuneInProgress),0x506FF0,FakeRetuneBusy);
        P(bit_cast<uintptr_t>(&CAudioEngine::IsCutsceneTrackActive),0x507150,FakeCutsceneActive);
        P(bit_cast<uintptr_t>(&CAudioEngine::RetuneRadio),0x507E10,FakeRetune);
        P(bit_cast<uintptr_t>(&CAudioEngine::SetMusicMasterVolume),0x506DE0,FakeMusicVol);
        P(bit_cast<uintptr_t>(&CAudioEngine::SetEffectsMasterVolume),0x506E10,FakeEffectsVol);
        P(bit_cast<uintptr_t>(&CGamma::SetGamma),0x747200,FakeSetGamma);
        // texto / fuente
        P(bit_cast<uintptr_t>(&CFont::GetStringWidth),0x71A0E0,FakeGetStringWidth);
        P(bit_cast<uintptr_t>(&CText::Get),0x6A0050,FakeTextGet);
        // CMenuManager
        P(bit_cast<uintptr_t>(&CMenuManager::SaveSettings),0x57C660,FakeSaveSettings);
        P(bit_cast<uintptr_t>(&CMenuManager::SaveStatsToFile),0x57DDE0,FakeSaveStats);
        P(bit_cast<uintptr_t>(&CMenuManager::Initialise),0x5744D0,FakeInitialise);
        P(bit_cast<uintptr_t>(&CMenuManager::LoadAllTextures),0x572EC0,FakeLoadAllTextures);
        P(bit_cast<uintptr_t>(&CMenuManager::UnloadTextures),0x574630,FakeUnloadTextures);
        P(bit_cast<uintptr_t>(&CMenuManager::SwitchToNewScreen),0x573680,FakeSwitchScreen);
        P(bit_cast<uintptr_t>(&CMenuManager::GetNumberOfMenuOptions),0x573E70,FakeNumOptions);
        P(bit_cast<uintptr_t>(&CMenuManager::ProcessMenuOptions),0x576FE0,FakeProcessMenuOptions);
        P(bit_cast<uintptr_t>(&CMenuManager::DrawControllerBound),0x57E6E0,FakeDrawControllerBound);
        // CMenuSystem
        P(bit_cast<uintptr_t>(&CMenuSystem::CreateNewMenu),0x582300,FakeCreateNewMenu);
        P(bit_cast<uintptr_t>(&CMenuSystem::InsertMenu),0x581E00,FakeInsertMenu);
        P(bit_cast<uintptr_t>(&CMenuSystem::CheckForAccept),0x5807C0,FakeCheckForAccept);
        P(bit_cast<uintptr_t>(&CMenuSystem::CheckForSelected),0x5807E0,FakeCheckForSelected);
        P(bit_cast<uintptr_t>(&CMenuSystem::SetActiveMenuItem),0x5820C0,FakeSetActiveMenuItem);
        P(bit_cast<uintptr_t>(&CMenuSystem::SwitchOffMenu),0x580750,FakeSwitchOffMenu);
        // CRadar
        P(bit_cast<uintptr_t>(&CRadar::ClearBlip),0x587CE0,FakeClearBlip);
        P(bit_cast<uintptr_t>(&CRadar::SetCoordBlip),0x583820,FakeSetCoordBlip);
        P(bit_cast<uintptr_t>(&CRadar::SetBlipSprite),0x583D70,FakeSetBlipSprite);
        // CControllerConfigManager
        P(bit_cast<uintptr_t>(&CControllerConfigManager::GetControllerKeyAssociatedWithAction),0x52F4F0,FakeGetKeyAssoc);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::GetIsActionAButtonCombo),0x52F550,FakeIsCombo);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::ClearSettingsAssociatedWithAction),0x52FD70,FakeClearAssoc);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::DeleteMatchingActionInitiators),0x531C90,FakeDeleteMatching);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::SetControllerKeyAssociatedWithAction),0x530490,FakeSetAssoc);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::GetJoyButtonJustDown),0x52D1E0,FakeJoyJustDown);
        // pad / entrada
        P(bit_cast<uintptr_t>(&CPad::Clear),0x541A70,FakePadClear);
        P(bit_cast<uintptr_t>(&CPad::ClearKeyBoardHistory),0x53F1E0,FakeClearKeyboardHistory);
        P(bit_cast<uintptr_t>(&CPad::ClearMouseHistory),0x541BD0,FakeClearMouseHistory);
        P(bit_cast<uintptr_t>(&IsVideoModeExclusive),0x745CA0,FakeVideoExclusive);
        P(bit_cast<uintptr_t>(&DIReleaseMouse),0x746F70,FakeDIReleaseMouse);
        P(bit_cast<uintptr_t>(&InitialiseMouse),0x7469A0,FakeInitialiseMouse);
        // varios
        P(bit_cast<uintptr_t>(&DoRWStuffStartOfFrame),0x53D690,FakeStartOfFrame);
        P(bit_cast<uintptr_t>(&DoRWStuffEndOfFrame),0x53D840,FakeEndOfFrame);
        P(bit_cast<uintptr_t>(&CTimer::EndUserPause),0x561B00,FakeEndUserPause);
        P(bit_cast<uintptr_t>(&CTimer::Update),0x561B10,FakeTimerUpdate);
        P(bit_cast<uintptr_t>(&FindPlayerPed),0x56E210,FakeFindPlayerPed);
        P(bit_cast<uintptr_t>(&CCamera::SetFadeColour),0x50BF00,FakeSetFadeColour);
        P(bit_cast<uintptr_t>(&CCamera::Fade),0x50AC20,FakeFade);
        P(bit_cast<uintptr_t>(&CCamera::ProcessFade),0x50B5D0,FakeProcessFade);

        static const int16 stickValues[]{0,0,0,0,-128,127,-16,-15,-14,16,15,14,-6,-5,-4,4,5,6,-100,100,1,-1,64,-64};
        static const int32 keyCodes[]{1056,1000,1001,1002,1003,1009,1053,1032,1054,1052,1004,1005,1006,1007,1008,1010,1011,1012,1013,1014,1015,1016,1017,1018,1019,1020,
                                      1021,1022,1023,1024,1025,1026,1027,1028,1029,1030,1031,1033,1034,1035,1036,1037,1038,1039,1040,1041,1042,1043,1044,1045,1046,1047,1048,1049,1050,1051,1055,
                                      0,65,66,97,32,13,9};
        static const uint32 deltas[]{0,1,5,19,20,21,100,150,199,200,201,250,1000,100000};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto chance=[&](uint32 n){return next()%n==0;};
                auto fmix=[&](float v){ // flotante con casos especiales (limites y NaN silencioso)
                    return v; };
                (void)fmix;
                std::vector<uint8> rawO, rawC;
                auto run=[&](bool candidate,std::vector<uint8>& rawOut)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7B21u; if(!stt)stt=1; trace=json::array();
                    restoreBlobs(); restoreReal();
                    for(auto& b:raw)b=uint8(next());
                    for(auto& b:pedRaw)b=uint8(next());
                    CMenuManager* self=Obj();
                    // ---------------- entrada comun ----------------
                    const uint32 dens=chance(3) ? 2u : chance(2) ? 4u : 16u;
                    auto zero=[&](){ return next()%dens==0 ? int16(255) : int16(0); };
                    auto randState=[&](CControllerState& n,CControllerState& o){
                        auto* wn=reinterpret_cast<int16*>(&n); auto* wo=reinterpret_cast<int16*>(&o);
                        for(size_t k=0;k<sizeof(CControllerState)/2;++k){ int16 v=zero(); if(chance(5))v=int16(int(next()%255)-127); wn[k]=v; wo[k]= chance(2) ? v : zero(); }
                        n.LeftStickX=stickValues[next()%std::size(stickValues)]; n.LeftStickY=stickValues[next()%std::size(stickValues)];
                        o.LeftStickX=stickValues[next()%std::size(stickValues)]; o.LeftStickY=stickValues[next()%std::size(stickValues)]; };
                    for(int k=0;k<2;++k){
                        auto* pb=reinterpret_cast<uint8*>(&CPad::Pads[k]); for(size_t j=0;j<sizeof(CPad);++j)pb[j]=uint8(next());
                        randState(CPad::Pads[k].NewState,CPad::Pads[k].OldState);
                        *reinterpret_cast<uint16*>(pb+0x10A)=uint16(next()%4); *reinterpret_cast<uint16*>(pb+0x10E)= chance(5) ? uint16(next()) : uint16(0);
                    }
                    { auto* kn=reinterpret_cast<int16*>(&CPad::NewKeyState); auto* ko=reinterpret_cast<int16*>(&CPad::OldKeyState);
                      for(size_t j=0;j<NKEY/2;++j){ const int16 v=zero(); kn[j]=v; ko[j]= chance(2) ? v : zero(); } }
                    { auto* mn=reinterpret_cast<uint8*>(&CPad::NewMouseControllerState); auto* mo=reinterpret_cast<uint8*>(&CPad::OldMouseControllerState);
                      static const uint8 mouseVals[]{0,0,0x80,0x80,1,255};
                      for(size_t j=0;j<7;++j){ const bool wheel=j==3||j==4; const uint8 v= chance(dens) ? (wheel ? uint8(1) : mouseVals[2+next()%4]) : uint8(0); mn[j]=v;
                          const uint8 w= chance(dens) ? (wheel ? uint8(1) : mouseVals[2+next()%4]) : uint8(0); mo[j]= chance(2) ? v : w; } }
                    for(int k=0;k<4;++k)*reinterpret_cast<int16*>(0xB736F0+4*k)=stickValues[next()%std::size(stickValues)];
                    // reloj y tiempos del singleton real
                    const uint32 now= chance(8) ? next() : 5000u+next()%1000000u;
                    CTimer::m_snTimeInMillisecondsPauseMode=now; CTimer::m_snTimeInMilliseconds=now^(chance(2) ? 0u : next()%2000u);
                    auto past=[&]{ const uint32 d=deltas[next()%std::size(deltas)]; return chance(6) ? now+d : now-d; };
                    auto* R=Real();
                    R->m_LastActionTime=past(); R->m_LastTransitionTime=past(); R->m_SlideLeftMoveTime=past(); R->m_SlideRightMoveTime=past();
                    R->m_nMouseHoverScreen=int32(next()%50)-3; R->field_1B74=int32(next()); R->m_bDrawingMap=chance(2); R->m_StatusDisablePlayerControls=uint16(next());
                    // pantalla (global, la lee el EXE por RsGlobal)
                    static const int widths[]{640,800,1024,1280,1366,1920,2560}, heights[]{448,480,600,768,1024,1080,1440};
                    RsGlobal.maximumWidth=widths[next()%std::size(widths)]; RsGlobal.maximumHeight=heights[next()%std::size(heights)];
                    // estado de CRadar para el mapa del menu (giro nulo para que los bucles converjan)
                    StaticRef<float>(0xBAA248)=0.0f; StaticRef<float>(0xBAA24C)=0.0f; StaticRef<float>(0xBA830C)=0.0f; StaticRef<float>(0xBA8308)=1.0f; StaticRef<float>(0xBA8314)=3000.0f;
                    *reinterpret_cast<uint8*>(0xA43088)=uint8(next()%3); *reinterpret_cast<uint8*>(0xB6F065)=chance(3); *reinterpret_cast<uint8*>(0x96918C)=chance(3);
                    *reinterpret_cast<uint8*>(0xA444A1)=chance(4); *reinterpret_cast<uint8*>(0xA444A4)=chance(4);
                    StaticRef<uint8>(0x8CE004)=uint8(next()); StaticRef<uint8>(0x8CE005)=uint8(next());
                    for(auto& s:CGenericGameStorage::ms_Slots)s=static_cast<eSlotState>(next()%4);
                    for(auto& b:CMessages::PreviousBriefs)b.Text= chance(3) ? nullptr : reinterpret_cast<const GxtChar*>(&b);
                    // mandos de los stubs
                    static const uint32 optionCounts[]{0,1,2,3,5,8,12,13};
                    numOptions=optionCounts[next()%std::size(optionCounts)]; cancelMode= chance(3) ? int(next()%2) : -1;
                    retuneBusy=chance(4); cutsceneActive=chance(4); videoExclusive=chance(2);
                    createRet=MenuId(next()%4); acceptRet=int8(int(next()%7)-2); selectedRet=int8(int(next()%7)-2); blipRet=int32(next()%50);
                    joyRet= chance(2) ? 0u : next()%20u; comboRet=chance(2); assocMode=int(next()%3); curKey=keyCodes[next()%std::size(keyCodes)];
                    // ---------------- campos comunes del objeto falso ----------------
                    auto pick=[&](int& screen,int& item){
                        const uint32 r=next()%10;
                        if(r<4&&!sliderList.empty()){ const auto& pr=sliderList[next()%sliderList.size()]; screen=pr.first; item=pr.second; }
                        else if(r<6&&!optionList.empty()){ const auto& pr=optionList[next()%optionList.size()]; screen=pr.first; item=pr.second; }
                        else { screen=int(next()%SCREEN_COUNT); item=int(next()%12); } };
                    self->m_nPlayerNumber= chance(4) ? 1 : 0;
                    self->m_pPressedKey= chance(6) ? &self->m_KeyPressedCode : nullptr;
                    self->m_KeyPressedCode=static_cast<RsKeyCodes>(keyCodes[next()%std::size(keyCodes)]);
                    self->field_EC= chance(8) ? int32(next()) : 0;
                    self->m_EditingControlOptions=chance(6); self->m_bJustOpenedControlRedefWindow=chance(4);
                    self->m_ControllerError=static_cast<eControllerError>(chance(12) ? next()%4 : 0); self->m_bScanningUserTracks=chance(12);
                    self->m_nSysMenu= chance(2) ? int8(-99) : chance(2) ? int8(next()%5) : int8(next());
                    self->m_RadioAvailable=chance(2)==false; self->m_isPreInitialised=chance(3); self->m_DisplayTheMouse=chance(2);
                    self->m_nMousePosX=int32(next()%2400)-100; self->m_nMousePosY=int32(next()%1600)-100;
                    self->m_nMousePosWinX= chance(2) ? int32(next()%2400)-200 : self->m_nMousePosX; self->m_nMousePosWinY= chance(2) ? int32(next()%1600)-200 : self->m_nMousePosY;
                    self->m_nOldMousePosX= chance(4) ? 0 : int32(next()%2200)+1; self->m_nOldMousePosY= chance(4) ? 0 : int32(next()%1500)+1;
                    if(chance(3)){ self->m_nOldMousePosX=self->m_nMousePosX; self->m_nOldMousePosY=self->m_nMousePosY; }
                    static const int bounds[]{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,16,0,-1,17};
                    self->m_MouseInBounds=static_cast<eMouseInBounds>(bounds[next()%std::size(bounds)]);
                    self->m_CurrentMouseOption=int32(next()%14)-1;
                    { int s,it; pick(s,it); self->m_nCurrentScreen=static_cast<eMenuScreen>(s); self->m_nCurrentScreenItem=it; }
                    for(auto& k:self->m_KeyPressed)k=chance(2);
                    self->m_ListSelection=int32(next()%40)-6; self->m_JustExitedRedefine=chance(3);
                    self->m_RedefiningControls=static_cast<eControlMode>(next()%2); self->m_ControlMethod=static_cast<eController>(next()%2);
                    self->m_CanBeDefined=chance(3)==false; self->m_DeleteAllBoundControls=chance(4); self->m_DeleteAllNextDefine=chance(4);
                    self->m_OptionToChange=int32(next()%64); self->m_nPressedMouseButton=static_cast<RsKeyCodes>(chance(2) ? 0 : 1+next()%7);
                    self->m_nJustDownJoyButton= chance(2) ? 0 : int32(next()%20); self->m_MenuIsAbleToQuit=chance(3);
                    // mapa
                    self->m_bAllStreamingStuffLoaded=chance(10);
                    { static const float zooms[]{300.f,300.f,299.f,301.f,350.f,500.f,800.f,1093.f,1100.f,1101.f,1099.5f,1000.f,700.f};
                      self->m_fMapZoom= chance(3) ? zooms[next()%std::size(zooms)] : uni(290.f,1110.f);
                      self->m_vMapOrigin.x=uni(580.f-self->m_fMapZoom-40.f,60.f+self->m_fMapZoom+40.f); self->m_vMapOrigin.y=uni(388.f-self->m_fMapZoom-40.f,60.f+self->m_fMapZoom+40.f);
                      if(chance(5)){ self->m_vMapOrigin.x=580.f-self->m_fMapZoom; self->m_vMapOrigin.y=388.f-self->m_fMapZoom; }
                      self->m_vMousePos.x= chance(8) ? uni(-3100.f,3100.f) : uni(-300.f,300.f); self->m_vMousePos.y= chance(8) ? uni(-3100.f,3100.f) : uni(-300.f,300.f); }
                    self->m_nTargetBlipIndex= chance(2) ? 0 : int32(next()%30)+1;
                    for(auto& b:self->m_abPrefsMapBlips)b=chance(2); self->m_bMapLegend=chance(2);
                    self->m_nSelectedRow=uint8(chance(3) ? next()%30 : 3+next()%17);
                    // ajustes de deslizadores en rangos normales (los casos extremos solo en CheckSliderMovement; |brillo| < 2^24 para que fiadd y (float)int coincidan)
                    self->m_PrefsBrightness=int32(next()%420)-10; self->m_fDrawDistance=uni(0.8f,2.0f); self->m_nRadioVolume=int8(next()%70); self->m_nSfxVolume=int8(next()%70);
                    TheCamera.m_fMouseAccelHorzntl=uni(0.f,0.006f); TheCamera.m_fMouseAccelVertical=uni(0.f,0.01f); StaticRef<float>(0xBA672C)=1.0f/3200.f; CRenderer::ms_lodDistScale=uni(0.5f,2.0f);
                    // el singleton real: CRadar lee zoom y origen de el (con m_bDrawingMap activo)
                    R->m_fMapZoom=self->m_fMapZoom; R->m_vMapOrigin=self->m_vMapOrigin;
                    // jugador falso y armas
                    { const int slot=int(next()%13); pedRaw[0x718]=uint8(slot);
                      *reinterpret_cast<int32*>(pedRaw.data()+0x5A0+slot*0x1C)= chance(2) ? 43 : int32(next()%47);
                      *reinterpret_cast<uint32*>(pedRaw.data()+0x5A0+slot*0x1C+0x10)=CTimer::m_snTimeInMilliseconds+uint32(int(next()%2000)-1000); }
                    self->m_bMenuActive=chance(2); self->m_bDontDrawFrontEnd=chance(5); self->m_bActivateMenuNextFrame=chance(5); self->m_bMenuAccessWidescreen=chance(3);
                    self->field_35=chance(3); self->m_bLoadingData=chance(5); self->m_bIsSaveDone=chance(5);

                    const MenuId dummyId=0; (void)dummyId;
                    json ret=nullptr;
                    const uintptr_t a=fn.address;
                    using Self=CMenuManager*;

                    // ---------------- fixtures y llamada por funcion ----------------
                    // (se fijan antes de conmutar los hooks para que ambas ejecuciones vean lo mismo)
                    auto callAfterHooks=[&](auto&& call){
                        for(auto* h:hooks)h->State(candidate);
                        const bool sw=ReversingProbe::JumpsToDll(fn.address)==candidate;
                        call();
                        return sw; };
                    bool switches=false;

                    if(nm=="UserInput"){
                        { int s,it; const uint32 r=next()%10; if(r<3){ s=38; it=int(next()%12); } else if(r<5){ s=5; it=int(next()%12); } else pick(s,it);
                          self->m_nCurrentScreen=static_cast<eMenuScreen>(s); self->m_nCurrentScreenItem=it; }
                        self->m_ControllerError=static_cast<eControllerError>(chance(15) ? 1+next()%3 : 0); self->m_bScanningUserTracks=chance(15);
                        self->m_EditingControlOptions=chance(10);
                        if(!self->m_EditingControlOptions&&chance(2)==false)self->m_pPressedKey=nullptr; if(self->m_EditingControlOptions)self->m_pPressedKey=&self->m_KeyPressedCode; /* el EXE desreferencia el puntero al editar */
                        self->field_EC= chance(8) ? int32(next()) : 0;
                        // ratón apuntando a un elemento del menu la mitad de las veces
                        if(chance(2)){ const auto& items=aScreens[int(self->m_nCurrentScreen)].m_aItems; const int row=int(next()%12);
                            const float y=float(int16(items[row].m_Y))*float(RsGlobal.maximumHeight)/448.f; self->m_nMousePosWinY=int32(y)+int32(next()%24)+1; self->m_nMousePosY=self->m_nMousePosWinY;
                            if(chance(2)){ self->m_nOldMousePosX=self->m_nMousePosX+1; self->m_nOldMousePosY=self->m_nOldMousePosY ? self->m_nOldMousePosY : 5; } }
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self)>(a)(self); });
                    }
                    else if(nm=="ProcessUserInput"){
                        const bool b0=chance(3), b1=chance(3), b2=chance(3), b3=chance(4); const uint32 lr=next()%6; const int8 lrv= lr<3 ? int8(0) : lr==3 ? int8(1) : lr==4 ? int8(-1) : int8(1);
                        self->m_EditingControlOptions=chance(10); self->m_ControllerError=static_cast<eControllerError>(0);
                        if(chance(8)==false)self->m_pPressedKey=nullptr; if(self->m_EditingControlOptions)self->m_pPressedKey=&self->m_KeyPressedCode; /* el EXE desreferencia el puntero al editar */
                        self->field_EC= chance(10) ? int32(next()) : 0;
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self,bool,bool,bool,bool,int8)>(a)(self,b0,b1,b2,b3,lrv); });
                    }
                    else if(nm=="AdditionalOptionInput"){
                        const uint32 r=next()%10; const int s= r<6 ? 5 : r<7 ? 0 : r<9 ? 2 : int(next()%SCREEN_COUNT); self->m_nCurrentScreen=static_cast<eMenuScreen>(s);
                        bool up=chance(2), down=chance(2);
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self,bool*,bool*)>(a)(self,&up,&down); });
                        ret=json::array({up,down});
                    }
                    else if(nm=="RedefineScreenUserInput"){
                        self->m_nCurrentScreen=SCREEN_CONTROLS_DEFINITION; self->m_EditingControlOptions=chance(8);
                        bool acc=chance(2), can=chance(2);
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self,bool*,bool*)>(a)(self,&acc,&can); });
                        ret=json::array({acc,can});
                    }
                    else if(nm=="CheckRedefineControlInput"){
                        self->m_EditingControlOptions=chance(4)==false; self->m_pPressedKey= chance(5) ? nullptr : &self->m_KeyPressedCode;
                        if(self->m_pPressedKey==nullptr&&self->m_EditingControlOptions&&!self->m_bJustOpenedControlRedefWindow)self->m_pPressedKey=&self->m_KeyPressedCode; // el EXE desreferencia el puntero
                        self->m_nCurrentScreen=SCREEN_CONTROLS_DEFINITION;
                        bool r=false;
                        switches=callAfterHooks([&]{ r=reinterpret_cast<bool(__thiscall*)(Self)>(a)(self); });
                        ret=r;
                    }
                    else if(nm=="CheckCodesForControls"){
                        const int32 type= chance(20) ? int32(next()%6) : int32(next()%4);
                        self->m_pPressedKey=&self->m_KeyPressedCode; self->m_KeyPressedCode=static_cast<RsKeyCodes>(curKey);
                        if(type>3){ /* tipos inexistentes: solo se prueban 0..3 */ }
                        const int32 t=type>3 ? int32(next()%4) : type;
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self,int32)>(a)(self,t); });
                    }
                    else if(nm=="CheckForMenuClosing"){
                        static const int screens[]{41,16,22,18,14,0,5,34,38,43};
                        self->m_nCurrentScreen=static_cast<eMenuScreen>(chance(3) ? int(next()%SCREEN_COUNT) : screens[next()%std::size(screens)]);
                        self->m_nCurrentScreenItem=int32(next()%12);
                        if(chance(2))*reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&CPad::NewKeyState)+0x218)=255; // esc (nuevo)
                        if(chance(2))*reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&CPad::OldKeyState)+0x218)=0;
                        self->m_nPlayerNumber=uint8(chance(5) ? 1 : 0);
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self)>(a)(self); });
                    }
                    else if(nm=="CheckSliderMovement"){
                        { int s,it; if(chance(8)==false&&!sliderList.empty()){ const auto& pr=sliderList[next()%sliderList.size()]; s=pr.first; it=pr.second; } else pick(s,it);
                          self->m_nCurrentScreen=static_cast<eMenuScreen>(s); self->m_nCurrentScreenItem=it; }
                        const int32 lr= chance(2) ? 1 : -1;
                        self->m_PrefsBrightness= chance(3) ? int32(next()%1000)-300 : int32(next()%385);
                        { static const int32 edge[]{0,1,23,24,25,359,360,361,383,384,385,-1}; if(chance(4))self->m_PrefsBrightness=edge[next()%std::size(edge)]; }
                        self->m_nRadioVolume=int8(chance(3) ? int(next()&0xFF) : int(next()%70)-3); self->m_nSfxVolume=int8(chance(3) ? int(next()&0xFF) : int(next()%70)-3);
                        { static const float dists[]{0.925f,1.8f,0.9f,1.85f,1.2f,1.77f,0.95f,0.925f+0.0546875f,1.8f-0.0546875f,1.0f};
                          self->m_fDrawDistance= chance(3) ? dists[next()%std::size(dists)] : uni(0.8f,2.0f); if(chance(40))self->m_fDrawDistance=std::bit_cast<float>(0x7FC00000u); }
                        { static const float acc[]{0.005f,1.0f/3200.f,0.0f,0.0049f,0.00001f,0.004f,0.0045f};
                          TheCamera.m_fMouseAccelHorzntl= chance(3) ? acc[next()%std::size(acc)] : uni(0.f,0.0065f); if(chance(40))TheCamera.m_fMouseAccelHorzntl=std::bit_cast<float>(0x7FC00000u);
                          StaticRef<float>(0xBA672C)= chance(2) ? 1.0f/3200.f : uni(0.f,0.003f); TheCamera.m_fMouseAccelVertical=uni(0.f,0.01f); }
                        CRenderer::ms_lodDistScale=uni(0.5f,2.0f);
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self,int32)>(a)(self,lr); });
                    }
                    else if(nm=="ScrollRadioStations"){
                        static const int8 stations[]{0,1,2,12,13,14,-1,-128,127,100,3,6,7,9,10,11};
                        self->m_nRadioStation=static_cast<eRadioID>(stations[next()%std::size(stations)]);
                        static const int8 steps[]{1,-1,1,-1,2,-2,0,127,-128,5,-13,13};
                        const int8 step=steps[next()%std::size(steps)];
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self,int8)>(a)(self,step); });
                    }
                    else if(nm=="CheckHover"){
                        const int32 base=self->m_nMousePosX; auto edge=[&](int32 v){ return chance(3) ? v+int32(next()%3)-1 : int32(next()%2400)-100; };
                        const int32 l=edge(base-int32(next()%50)), r=edge(base+int32(next()%50)), t=edge(self->m_nMousePosY-int32(next()%50)), b=edge(self->m_nMousePosY+int32(next()%50));
                        bool res=false;
                        switches=callAfterHooks([&]{ res=reinterpret_cast<bool(__thiscall*)(Self,int32,int32,int32,int32)>(a)(self,l,r,t,b); });
                        ret=res;
                    }
                    else if(nm=="CheckFrontEndUpInput"||nm=="CheckFrontEndDownInput"||nm=="CheckFrontEndLeftInput"||nm=="CheckFrontEndRightInput"){
                        bool res=false;
                        switches=callAfterHooks([&]{ res=reinterpret_cast<bool(__thiscall*)(Self)>(a)(self); });
                        ret=res;
                    }
                    else if(nm=="CentreMousePointer"){
                        static const int dims[]{0,1,2,3,640,641,800,1023,1024,1280,1366,1919,1920};
                        RsGlobal.maximumWidth=dims[next()%std::size(dims)]; RsGlobal.maximumHeight=dims[next()%std::size(dims)];
                        StaticRef<uint32>(0x8D621C)=1; // IsForegroundApp() verdadero (con FIX_BUGS la DLL no hace nada en segundo plano)
                        PSGLOBAL(lastMousePos).x=uni(-5.f,5.f); PSGLOBAL(lastMousePos).y=uni(-5.f,5.f);
                        SetCursorPos(10+int(next()%20),10+int(next()%20));
                        switches=callAfterHooks([&]{ reinterpret_cast<void(__thiscall*)(Self)>(a)(self); });
                        /* el cursor real depende del usuario (ratón en movimiento): solo se compara lastMousePos */
                        ret=json::array({F(PSGLOBAL(lastMousePos).x),F(PSGLOBAL(lastMousePos).y)});
                    }

                    rawOut.assign(raw.begin(),raw.end());
                    json blobHashes=json::array(); for(auto& b:blobs)blobHashes.push_back(b.compare ? Fnv(reinterpret_cast<void*>(b.addr),b.n) : uint64(0));
                    json out={{"switches",switches},{"ret",ret},{"trace",trace},{"object",Fnv(raw.data(),raw.size())},{"real",realHash()},{"blobs",blobHashes}};
                    // CentreMousePointer: el cursor real de Windows ya va en ret; el resto de funciones no lo tocan
                    return out;
                };
                const auto original=run(false,rawO), candidate=run(true,rawC);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json names=json::array(); if(original["blobs"]!=candidate["blobs"])for(size_t k=0;k<blobs.size();++k)if(original["blobs"][k]!=candidate["blobs"][k])names.push_back(blobs[k].name);
                    json bytes=json::array(); // [desplazamiento respecto al objeto (negativo = guarda previa), original, candidato]
                    for(size_t k=0;k<rawO.size()&&bytes.size()<64;++k)if(rawO[k]!=rawC[k])bytes.push_back({int(k)-int(GUARD),rawO[k],rawC[k]});
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"blobs_differ",names},{"byte_diffs",bytes},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t k=0;k<hooks.size();++k){hooks[k]->State(enabled[k]);hooks[k]->LockState(locked[k]);}
    restoreBlobs(); restoreReal(); g_FastLoaderConfig.TriedLoadingSaveGame=savedTried; SetCursorPos(cursorBefore.x,cursorBefore.y);
    bool restored=std::memcmp(Real(),savedReal.data(),0xE0)==0&&std::memcmp(reinterpret_cast<uint8*>(Real())+0xE8,savedReal.data()+0xE8,OBJ-0xE8)==0&&g_FastLoaderConfig.TriedLoadingSaveGame==savedTried;
    for(auto& b:blobs)restored&=std::memcmp(reinterpret_cast<void*>(b.addr),b.saved.data(),b.n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
