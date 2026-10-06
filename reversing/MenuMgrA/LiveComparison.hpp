#pragma once
// Banco dentro del motor: CMenuManager, grupo A (dibujo; 19 funciones).
//   DrawBackground, DrawFrontEnd, DrawStandardMenus, DrawWindow, DrawWindowedText, DrawQuitGameScreen, DrawControllerBound,
//   DrawControllerScreenExtraText, DrawControllerSetupScreen, DisplaySlider, DisplayHelperText, PrintBriefs, PrintMap,
//   PrintRadioStationList, PrintStats, MessageScreen, SmallMessageScreen, StretchX, StretchY.
// Original (hooks de las 19 desactivados a la vez) frente a candidato (activados a la vez): como las funciones del grupo se llaman
// entre si, cada caso ejecuta toda la cadena por el lado EXE o por el lado DLL.
// Se trabaja sobre un CMenuManager FALSO (0x1B78 bytes con guardas de 64 bytes, nunca el singleton como `this`). El EXE lee y escribe
// ademas por direccion absoluta unos campos del singleton real 0xBA6748 (0x1B24..0x1B63: m_StatsScrollTime, m_bViewRadar,
// m_RadarVisibilityChangeTime, m_BriefsArrowBlinkTime, m_UserTrackScanningTime, m_ErrorPendingReset, m_ErrorStartTime, bitfield 0x1B4C,
// m_BackgroundSprite, m_InputWaitBlink, m_LastBlinkTime, m_HelperTextUpdatedTime, m_OptionFlashColorState, m_LastHighlightToggleTime) y
// CRadar::TransformRadarPointToScreenSpace lee m_bDrawingMap/m_vMapOrigin/m_fMapZoom del singleton: antes de cada ejecucion se copian
// del objeto falso al singleton; despues, para el original el estado efectivo de esos bytes es el del singleton y para el candidato el
// del objeto falso (se comprueba ademas que la otra copia no se toca) y el singleton se restaura entero.
// Se fijan por caso, se comparan y se restauran: resolucion (RsGlobal), reloj, contador de fotogramas, pad/teclado/raton, tabla aScreens,
// estaticos de pantalla (0x8CDFF4..0x8CE003), gString/gGxtString/gGxtString2, ranuras de guardado (estado y nombres), PreviousBriefs,
// estadisticas, zonas y leyenda del mapa, banderas de scripts, estado de CRadar (origen/rango/seno/coseno), sensibilidad del raton,
// direccion del raton invertida, direccion de direccion con raton, estado del escaneo de pistas de usuario y texto de error de CD.
// Se simulan y trazan: CFont (todo), CSprite2d (DrawRect, Draw, Draw2DPolygon, DrawBarChart, SetRecipNearClip, InitPerFrame), CText::Get,
// RsCameraBeginUpdate, DefinedState2d, CMenuManager::{SetHelperText, ResetHelperText, CheckHover, SaveSettings, SetFrontEndRenderStates},
// CAudioEngine::{IsRadioRetuneInProgress, GetRadioStationName}, CAEUserRadioTrackManager::{ScanUserTracks, Initialise},
// CControllerConfigManager::{GetControllerSettingTextWithOrderNumber, GetButtonComboText}, Fx_c::GetFxQuality, GetVideoModeList,
// CMemoryMgr::{Calloc, Free}, CStats::{ConstructStatLine, GetStatValue, FindCriminalRatingNumber, FindCriminalRatingString},
// CMessages::{InsertNumberInString, InsertStringInString, InsertPlayerControlKeysInString}, GetSavedGameDateAndTime, CMenuSystem::{Process,
// SwitchOffMenu}, CRadar::{InitFrontEndMap, DrawRadarSectionMap, DrawRadarGangOverlay, DrawBlips, DrawLegend}, CPlaceName::GetForMap,
// CTheZones::GetCurrentZoneLockedOrUnlocked, FindPlayerCentreOfWorld_NoSniperShift y RsEventHandler. CMenuManager::DrawBuildInfo (NOTSA,
// solo DLL; dibuja la version) se anula en la DLL para que no aparezca en la traza. CRadar::Transform* y LimitRadarPoint se ejecutan de verdad.
// Se compara: retorno, bytes del objeto falso (con guardas), estado efectivo de los campos absolutos, singleton, bloques de memoria global
// tocados y la traza.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MenuManager.h"
#include "MenuSystem.h"
#include "ControllerConfigManager.h"
#include "Radar.h"
#include "TheZones.h"
#include "UserDisplay.h"
#include "Messages.h"
#include "Stats.h"
#include "GenericGameStorage.h"
#include "AudioEngine.h"
#include "AEUserRadioTrackManager.h"
#include "VideoMode.h"
#include "app/platform/platform.h"
#include "../ProbeRuntime.hpp"

namespace MenuMgrALive {
using M = CMenuManager;
using ReversingProbe::EntryPatch;
inline bool done{}; inline std::ofstream log; inline json trace; inline uint32 traceOver{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline void T(json&& j){ if(trace.size()<6000)trace.push_back(std::move(j)); else ++traceOver; }

// --- objeto falso: [guarda][CMenuManager][guarda] ---
constexpr size_t GUARD=64, OBJ=sizeof(CMenuManager);
static_assert(sizeof(CMenuManager)==0x1B78);
alignas(16) inline std::array<uint8,GUARD+OBJ+GUARD> raw{};
inline uint8* OB() { return raw.data()+GUARD; }
inline M* Obj() { return reinterpret_cast<M*>(OB()); }
inline M* Real() { return &FrontEndMenuManager; }
// campos del singleton que el EXE toca por direccion absoluta (desplazamiento, longitud)
inline constexpr std::array<std::pair<size_t,size_t>,14> absMask{{{0x1B24,4},{0x1B28,1},{0x1B2C,4},{0x1B30,4},{0x1B40,4},{0x1B44,1},{0x1B48,4},
    {0x1B4C,4},{0x1B50,1},{0x1B51,1},{0x1B54,4},{0x1B58,4},{0x1B5C,1},{0x1B60,4}}};
inline bool InMask(size_t off){ for(auto& m:absMask)if(off>=m.first&&off<m.first+m.second)return true; return false; }
inline void CopyMask(uint8* dst,const uint8* src){ for(auto& m:absMask)std::memcpy(dst+m.first,src+m.first,m.second); }

// --- texto ---
inline json Txt(const void* p){
    if(!p)return nullptr;
    auto* s=static_cast<const uint8*>(p); size_t n=0; while(n<512&&s[n])++n;
    bool ascii=true; for(size_t i=0;i<n;++i)ascii&=s[i]<0x80;
    if(ascii)return std::string(reinterpret_cast<const char*>(s),n);
    return ReversingProbe::Bytes(s,n);
}
inline json Rect(const CRect& r){ auto* f=reinterpret_cast<const float*>(&r); return json::array({F(f[0]),F(f[1]),F(f[2]),F(f[3])}); } // orden de memoria
inline json Col(const CRGBA& c){ return json::array({c.r,c.g,c.b,c.a}); }
inline int SpriteIdx(const CSprite2d* s){ const auto d=reinterpret_cast<const uint8*>(s)-OB(); return (d>=0&&d<intptr_t(OBJ)) ? int(d) : -1; }
inline std::array<std::array<GxtChar,300>,128> pool{}; inline uint32 poolNext{};
inline const GxtChar* Pool(const std::string& s){ auto& out=pool[poolNext++%pool.size()]; const size_t n=std::min<size_t>(s.size(),280); std::memcpy(out.data(),s.data(),n); out[n]=0; return out.data(); }

// --- mandos por caso de los stubs ---
inline float curScaleX{1.f};
inline bool cameraOk{true}, retuneBusy{}, scanRet{}, zoneLocked{}; inline int fxQuality{2}; inline uint32 hoverMode{}, ctrlSeed{}; inline int statCount{};
inline CVector playerCentre{};
inline std::array<std::array<char,48>,10> videoStrings{}; inline std::array<char*,16> videoPtrs{};
inline std::array<std::array<GxtChar,70>,8> dateText{};
inline std::array<uint8,128> callocBuf{};
inline std::array<std::array<GxtChar,64>,20> briefText{}; inline std::array<std::array<GxtChar,32>,20> briefStr{};

// --- CFont ---
inline void __cdecl FSetBackground(bool a,bool b){ T({"font_background",a,b}); }
inline void __cdecl FSetProportional(bool a){ T({"font_proportional",a}); }
inline void __cdecl FSetFontStyle(eFontStyle s){ T({"font_style",int(s)}); }
inline void __cdecl FSetScale(float w,float h){ curScaleX=w; T({"font_scale",F(w),F(h)}); }
inline void __cdecl FSetScaleLang(float w,float h){ curScaleX=w; T({"font_scale_lang",F(w),F(h)}); }
inline void __cdecl FSetOrientation(eFontAlignment a){ T({"font_orientation",int(a)}); }
inline void __cdecl FSetColor(CRGBA c){ T({"font_color",Col(c)}); }
inline void __cdecl FSetDropColor(CRGBA c){ T({"font_drop_color",Col(c)}); }
inline void __cdecl FSetEdge(int8 v){ T({"font_edge",int(v)}); }
inline void __cdecl FSetDropShadowPosition(int16 v){ T({"font_shadow",int(v)}); }
inline void __cdecl FSetWrapx(float v){ T({"font_wrapx",F(v)}); }
inline void __cdecl FSetCentreSize(float v){ T({"font_centre_size",F(v)}); }
inline void __cdecl FSetRightJustifyWrap(float v){ T({"font_right_justify_wrap",F(v)}); }
inline void __cdecl FSetJustify(bool a){ T({"font_justify",a}); }
inline void __cdecl FSetAlphaFade(float v){ T({"font_alpha_fade",F(v)}); }
inline void __cdecl FInitPerFrame(){ T({"font_init_per_frame"}); }
inline void __cdecl FRenderFontBuffer(){ T({"font_render_buffer"}); }
inline void __cdecl FDrawFonts(){ T({"font_draw_fonts"}); }
inline void __cdecl FPrintString(float x,float y,const GxtChar* s){ T({"print_string",F(x),F(y),Txt(s)}); }
inline void __cdecl FPrintStringFromBottom(float x,float y,const GxtChar* s){ T({"print_string_bottom",F(x),F(y),Txt(s)}); }
// ancho y numero de lineas deterministas a partir del contenido y de la escala vigente
inline float __cdecl FGetStringWidth(const GxtChar* s,bool full,bool script){
    size_t n=0; while(s&&n<512&&s[n])++n;
    const float w=float(n+1)*curScaleX*9.f;
    T({"string_width",Txt(s),full,script,F(w)}); return w; }
inline int16 __cdecl FGetNumberLines(float x,float y,const GxtChar* s){
    size_t n=0; uint32 h=7; while(s&&n<512&&s[n]){h=h*31+s[n];++n;}
    const int16 r=int16((h+uint32(F(x))+uint32(F(y)))%4u);
    T({"number_lines",F(x),F(y),Txt(s),int(r)}); return r; }
inline void __cdecl FGetTextRect(CRect* r,float x,float y,const GxtChar* s){
    size_t n=0; while(s&&n<512&&s[n])++n;
    auto* f=reinterpret_cast<float*>(r); f[0]=x-2.5f; f[1]=y-3.f; f[2]=x+float(n)*5.f+7.f; f[3]=y+20.f+float(n%3)*4.f; // izquierda, arriba(+4), derecha, abajo(+C)
    T({"text_rect",F(x),F(y),Txt(s),Rect(*r)}); }
// --- CSprite2d ---
inline void __cdecl SDrawRect(const CRect& r,const CRGBA& c){ T({"draw_rect",Rect(r),Col(c)}); }
inline void __fastcall SDrawSpriteRect(CSprite2d* self,void*,const CRect& r,const CRGBA& c){ T({"sprite_draw",SpriteIdx(self),Rect(r),Col(c)}); }
inline void __fastcall SDrawSpriteXY(CSprite2d* self,void*,float x,float y,float w,float h,const CRGBA& c){ T({"sprite_draw_xywh",SpriteIdx(self),F(x),F(y),F(w),F(h),Col(c)}); }
inline void __cdecl SDraw2DPolygon(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4,const CRGBA& c){
    T({"polygon",F(x1),F(y1),F(x2),F(y2),F(x3),F(y3),F(x4),F(y4),Col(c)}); }
inline void __cdecl SDrawBarChart(float x,float y,uint16 w,uint8 h,float progress,int8 add,uint8 pct,uint8 border,CRGBA c,CRGBA c2){
    T({"bar_chart",F(x),F(y),int(w),int(h),F(progress),int(add),int(pct),int(border),Col(c),Col(c2)}); }
inline void __cdecl SSetRecipNearClip(){ T({"sprite_recip_near_clip"}); }
inline void __cdecl SInitPerFrame(){ T({"sprite_init_per_frame"}); }
// --- texto ---
inline const GxtChar* __fastcall FakeTextGet(CText* self,void*,const char* key){
    size_t n=0; while(key&&n<64&&key[n])++n;
    const std::string k(key ? key : "",n);
    T({"text_get",self==&TheText,Txt(key)});
    std::string s;
    switch(uint32(Fnv(k.data(),k.size()))%5){
    case 0: s=k; break;
    case 1: s=k+" ~1~"; break;
    case 2: s="~1~ "+k+" ~1~"; break;
    case 3: s=k+" "+k+" "+k+" "+k; break;
    default: s="$~1~ "+k+" OK"; break;
    }
    return Pool(s);
}
// --- motor / audio / varios ---
inline BOOL __cdecl FakeCameraBegin(RwCamera*){ T({"camera_begin",cameraOk}); return cameraOk ? 1 : 0; }
inline void __cdecl FakeDefinedState2d(){ T({"defined_state_2d"}); }
inline void __fastcall FakeSetHelperText(M* self,void*,eHelperText id){ T({"set_helper_text",self==Obj(),int(id)}); self->m_HelperText=id; self->m_nHelperTextFadingAlpha=300; }
inline void __fastcall FakeResetHelperText(M* self,void*){ T({"reset_helper_text",self==Obj()}); self->m_HelperText=HELPER_NONE; self->m_nHelperTextFadingAlpha=300; }
inline bool __fastcall FakeCheckHover(M* self,void*,int32 l,int32 r,int32 t,int32 b){
    const uint32 h=uint32(l)*73856093u^uint32(r)*19349663u^uint32(t)*83492791u^uint32(b)*2654435761u^hoverMode*0x9E3779B9u;
    const bool res= hoverMode==1 ? true : hoverMode==2 ? false : ((h>>7)%3)==0;
    T({"check_hover",self==Obj(),l,r,t,b,res}); return res; }
inline void __fastcall FakeSaveSettings(M* self,void*){ T({"save_settings",self==Obj()}); }
inline void __fastcall FakeSetFrontEndRenderStates(M* self,void*){ T({"set_front_end_render_states",self==Obj()}); }
inline void __fastcall FakeDrawBuildInfo(M*,void*){}
inline bool __fastcall FakeRetuneBusy(CAudioEngine* self,void*){ T({"radio_retune_busy",self==&AudioEngine,retuneBusy}); return retuneBusy; }
inline const GxtChar* __fastcall FakeRadioName(CAudioEngine* self,void*,eRadioID id){ T({"radio_station_name",self==&AudioEngine,int(id)}); return Pool("RS"+std::to_string(int(id))); }
inline bool __fastcall FakeScanUserTracks(CAEUserRadioTrackManager*,void*){ T({"scan_user_tracks",scanRet}); return scanRet; }
inline bool __fastcall FakeUserTracksInit(CAEUserRadioTrackManager*,void*){ T({"user_tracks_initialise"}); return true; }
inline const GxtChar* __fastcall FakeCtrlText(CControllerConfigManager* self,void*,eControllerAction a,eContSetOrder o){
    const uint32 h=uint32(int(a))*7u+uint32(int(o))*3u+ctrlSeed;
    const GxtChar* r= h%3==0 ? nullptr : Pool("B"+std::to_string(int(a))+"_"+std::to_string(int(o)));
    T({"ctrl_text",self==&ControlsManager,int(a),int(o),Txt(r)}); return r; }
inline const GxtChar* __fastcall FakeComboText(CControllerConfigManager* self,void*,eControllerAction a){
    const GxtChar* r= (uint32(int(a))+ctrlSeed)%4==0 ? nullptr : Pool("COMBO"+std::to_string(int(a)));
    T({"combo_text",self==&ControlsManager,int(a),Txt(r)}); return r; }
inline FxQuality_e __fastcall FakeFxQuality(Fx_c* self,void*){ T({"fx_quality",self==&g_fx,fxQuality}); return static_cast<FxQuality_e>(fxQuality); }
inline char** __cdecl FakeVideoModes(){ T({"video_mode_list"}); return videoPtrs.data(); }
inline uint8* __cdecl FakeCalloc(uint32 n,uint32 sz,uint32 hint){ T({"calloc",n,sz,hint}); std::memset(callocBuf.data(),0,callocBuf.size()); return callocBuf.data(); }
inline void __cdecl FakeFree(void* p){ T({"free",p==callocBuf.data()}); }
// --- estadisticas / mensajes ---
inline int32 __cdecl FakeConstructStatLine(int32 line,uint8 stat){
    char b1[64]{}, b2[64]{};
    if((uint32(line)+uint32(stat))%5!=0) std::snprintf(b1,sizeof(b1),"STAT%d_%d",line,int(stat)); // a veces vacio
    switch((uint32(line)+uint32(stat))%4){ case 0: std::snprintf(b2,sizeof(b2),"%d",line*37+int(stat)); break; case 1: std::snprintf(b2,sizeof(b2),"%d|%d",line,int(stat)); break;
        case 2: std::snprintf(b2,sizeof(b2),"%d",(line*11)%1000); break; default: std::snprintf(b2,sizeof(b2),"A|B%d",int(stat)); break; }
    std::memcpy(gGxtString,b1,sizeof(b1)); std::memcpy(gGxtString2,b2,sizeof(b2));
    T({"construct_stat_line",line,int(stat),Txt(gGxtString),Txt(gGxtString2),statCount}); return statCount; }
inline float __cdecl FakeGetStatValue(eStats s){ const float v=float(int(uint32(s)*37u%2500u))-100.f; T({"get_stat_value",int(s),F(v)}); return v; }
inline int32 __cdecl FakeCriminalRatingNumber(){ T({"criminal_rating_number"}); return int32(ctrlSeed%100u); }
inline GxtChar* __cdecl FakeCriminalRatingString(){ T({"criminal_rating_string"}); return const_cast<GxtChar*>(Pool("RATING"+std::to_string(ctrlSeed%7u))); }
inline void __cdecl FakeInsertNumber(const GxtChar* src,int32 a,int32 b,int32 c,int32 d,int32 e,int32 f,GxtChar* dst){
    T({"insert_number",Txt(src),a,b,c,d,e,f});
    std::string s=src ? std::string(reinterpret_cast<const char*>(src)) : std::string();
    s+="#"+std::to_string(a)+","+std::to_string(f); if(s.size()>200)s.resize(200); std::memcpy(dst,s.c_str(),s.size()+1); }
inline void __cdecl FakeInsertString(GxtChar* into,const GxtChar* str){
    T({"insert_string",Txt(into),Txt(str)});
    std::string s(reinterpret_cast<const char*>(into)); s+="+"; if(str)s+=reinterpret_cast<const char*>(str); if(s.size()>200)s.resize(200); std::memcpy(into,s.c_str(),s.size()+1); }
inline void __cdecl FakeInsertKeys(GxtChar* str){
    T({"insert_keys",Txt(str)});
    std::string s(reinterpret_cast<const char*>(str)); s+="*K"; if(s.size()>200)s.resize(200); std::memcpy(str,s.c_str(),s.size()+1); }
inline const GxtChar* __cdecl FakeSavedDate(int32 slot){ T({"saved_date",slot}); return dateText[uint32(slot)&7u].data(); }
// --- mapa ---
inline void __cdecl FakeInitFrontEndMap(){ T({"init_front_end_map"}); }
inline void __cdecl FakeDrawRadarSection(int32 x,int32 y,float l,float t,float r,float b){ T({"radar_section",x,y,F(l),F(t),F(r),F(b)}); }
inline void __cdecl FakeDrawGangOverlay(bool a){ T({"gang_overlay",a}); }
inline void __cdecl FakeDrawBlips(){ T({"draw_blips"}); }
inline void __cdecl FakeDrawLegend(int32 x,int32 y,int8 sprite){ T({"draw_legend",x,y,int(sprite)}); }
inline const GxtChar* __fastcall FakePlaceName(CPlaceName*,void*,float x,float y){ T({"place_name",F(x),F(y)}); return Pool("PLACE"+std::to_string(F(x)%97u)); }
inline bool __cdecl FakeZoneLocked(float x,float y){ T({"zone_locked",F(x),F(y),zoneLocked}); return zoneLocked; }
inline const CVector& __cdecl FakePlayerCentre(int32 id){ T({"player_centre",id}); return playerCentre; }
inline void __cdecl FakeMenuSystemProcess(int8 m){ T({"menu_system_process",int(m)}); }
inline void __cdecl FakeMenuSystemSwitchOff(MenuId m){ T({"menu_system_switch_off",int(m)}); }
inline RsEventStatus __cdecl FakeRsEvent(RsEvent e,void* p){ T({"rs_event",int(e),p==nullptr}); return rsEVENTPROCESSED; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,19> fns{{
    {"CMenuManager","DrawBackground",0x57B750,0,-1,1024},{"CMenuManager","DrawFrontEnd",0x57C290,0,-1,512},
    {"CMenuManager","DrawStandardMenus",0x5794A0,0,-1,2048},{"CMenuManager","DrawWindow",0x573EE0,0,-1,512},
    {"CMenuManager","DrawWindowedText",0x578F50,0,-1,512},{"CMenuManager","DrawQuitGameScreen",0x57D860,0,-1,128},
    {"CMenuManager","DrawControllerBound",0x57E6E0,0,-1,1024},{"CMenuManager","DrawControllerScreenExtraText",0x57D8D0,0,-1,512},
    {"CMenuManager","DrawControllerSetupScreen",0x57F300,0,-1,1024},{"CMenuManager","DisplaySlider",0x576860,0,-1,1024},
    {"CMenuManager","DisplayHelperText",0x57E240,0,-1,1024},{"CMenuManager","PrintBriefs",0x576320,0,-1,512},
    {"CMenuManager","PrintMap",0x575130,0,-1,2048},{"CMenuManager","PrintRadioStationList",0x5746F0,0,-1,256},
    {"CMenuManager","PrintStats",0x574900,0,-1,1024},{"CMenuManager","MessageScreen",0x579330,0,-1,256},
    {"CMenuManager","SmallMessageScreen",0x574010,0,-1,512},{"CMenuManager","StretchX",0x5733E0,0,-1,256},
    {"CMenuManager","StretchY",0x573410,0,-1,256},
}};

struct Blob { const char* name; uintptr_t addr; size_t size; std::vector<uint8> saved; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"menumgra-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"menumgra-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("menumgra");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"menumgra-only.txt"); if(f)std::getline(f,only); }

    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(size_t f=0;f<fns.size();++f){ auto* h=FindHook(fns[f].cls,fns[f].name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} hooks.push_back(h); }
    std::vector<char> enabled(hooks.size()),locked(hooks.size());
    for(size_t k=0;k<hooks.size();++k){enabled[k]=hooks[k]->Hooked();locked[k]=hooks[k]->Locked();hooks[k]->LockState(false);}

    // desplazamientos de los campos usados (se comprueban en tiempo de ejecucion contra los del EXE)
    {
        #define MA_OFF(m) size_t(reinterpret_cast<const uint8*>(&Obj()->m)-OB())
        struct Off { const char* n; size_t have; size_t want; };
        const Off offs[]{
            {"m_nCurrentScreenItem",MA_OFF(m_nCurrentScreenItem),0x54},{"m_bDrawingMap",MA_OFF(m_bDrawingMap),0x59},{"m_fMapZoom",MA_OFF(m_fMapZoom),0x64},
            {"m_vMapOrigin",MA_OFF(m_vMapOrigin),0x68},{"m_vMousePos",MA_OFF(m_vMousePos),0x70},{"m_bMapLoaded",MA_OFF(m_bMapLoaded),0x78},
            {"m_nPrefsLanguage",MA_OFF(m_nPrefsLanguage),0x84},{"m_ListSelection",MA_OFF(m_ListSelection),0x90},{"m_RadioMode",MA_OFF(m_RadioMode),0xB0},
            {"m_RedefiningControls",MA_OFF(m_RedefiningControls),0xB7},{"m_nMousePosX",MA_OFF(m_nMousePosX),0xBC},{"m_bPrefsMipMapping",MA_OFF(m_bPrefsMipMapping),0xC4},
            {"m_nDisplayAntialiasing",MA_OFF(m_nDisplayAntialiasing),0xCC},{"m_ControlMethod",MA_OFF(m_ControlMethod),0xD0},{"m_nDisplayVideoMode",MA_OFF(m_nDisplayVideoMode),0xD8},
            {"m_bMainMenuSwitch",MA_OFF(m_bMainMenuSwitch),0xE9},{"m_nPlayerNumber",MA_OFF(m_nPlayerNumber),0xEA},{"m_pPressedKey",MA_OFF(m_pPressedKey),0xF0},
            {"m_aFrontEndSprites",MA_OFF(m_aFrontEndSprites),0xF8},{"m_bTexturesLoaded",MA_OFF(m_bTexturesLoaded),0x15C},{"m_nCurrentScreen",MA_OFF(m_nCurrentScreen),0x15D},
            {"m_SelectedSlot",MA_OFF(m_SelectedSlot),0x15F},{"m_MissionPacks",MA_OFF(m_MissionPacks),0x161},{"m_MenuIsAbleToQuit",MA_OFF(m_MenuIsAbleToQuit),0x1AE8},
            {"m_RadioAvailable",MA_OFF(m_RadioAvailable),0x1AE9},{"m_ControllerError",MA_OFF(m_ControllerError),0x1AEA},{"m_bScanningUserTracks",MA_OFF(m_bScanningUserTracks),0x1AEB},
            {"m_nHelperTextFadingAlpha",MA_OFF(m_nHelperTextFadingAlpha),0x1AEC},{"m_nOldMousePosX",MA_OFF(m_nOldMousePosX),0x1AF8},{"m_MouseInBounds",MA_OFF(m_MouseInBounds),0x1B00},
            {"m_CurrentMouseOption",MA_OFF(m_CurrentMouseOption),0x1B04},{"m_EditingControlOptions",MA_OFF(m_EditingControlOptions),0x1B09},{"m_DeleteAllNextDefine",MA_OFF(m_DeleteAllNextDefine),0x1B0B},
            {"m_OptionToChange",MA_OFF(m_OptionToChange),0x1B0C},{"m_CanBeDefined",MA_OFF(m_CanBeDefined),0x1B14},{"m_HelperText",MA_OFF(m_HelperText),0x1B18},
            {"m_StatsScrollTime",MA_OFF(m_StatsScrollTime),0x1B24},{"m_bViewRadar",MA_OFF(m_bViewRadar),0x1B28},{"m_BriefsArrowBlinkTime",MA_OFF(m_BriefsArrowBlinkTime),0x1B30},
            {"m_UserTrackScanningTime",MA_OFF(m_UserTrackScanningTime),0x1B40},{"m_ErrorStartTime",MA_OFF(m_ErrorStartTime),0x1B48},{"m_BackgroundSprite",MA_OFF(m_BackgroundSprite),0x1B50},
            {"m_LastBlinkTime",MA_OFF(m_LastBlinkTime),0x1B54},{"m_HelperTextUpdatedTime",MA_OFF(m_HelperTextUpdatedTime),0x1B58},{"m_LastHighlightToggleTime",MA_OFF(m_LastHighlightToggleTime),0x1B60},
        };
        json bad=json::array(); for(auto& o:offs)if(o.have!=o.want)bad.push_back({o.n,o.have,o.want});
        Write({{"event","layout"},{"checked",std::size(offs)},{"mismatches",bad}});
#undef MA_OFF
    }

    // estado global guardado
    constexpr size_t NKEY=sizeof(CKeyboardState), NMOUSE=sizeof(CMouseControllerState), NPAD=sizeof(CPad)*2;
    std::vector<Blob> blobs;
    auto AddBlob=[&](const char* name,void* p,size_t n){ blobs.push_back({name,uintptr_t(p),n,std::vector<uint8>(static_cast<uint8*>(p),static_cast<uint8*>(p)+n)}); };
    AddBlob("pads",&CPad::Pads[0],NPAD); AddBlob("new_keys",&CPad::NewKeyState,NKEY); AddBlob("old_keys",&CPad::OldKeyState,NKEY);
    AddBlob("new_mouse",&CPad::NewMouseControllerState,NMOUSE); AddBlob("old_mouse",&CPad::OldMouseControllerState,NMOUSE);
    AddBlob("rsglobal",&RsGlobal.maximumWidth,8); AddBlob("timer",reinterpret_cast<void*>(0xB7CB7C),12); AddBlob("frame_counter",reinterpret_cast<void*>(0xB7CB4C),4);
    AddBlob("gstring",&gString[0],sizeof(gString)); AddBlob("ggxtstring",&gGxtString[0],sizeof(gGxtString)); AddBlob("ggxtstring2",&gGxtString2[0],sizeof(gGxtString2));
    AddBlob("screens",&aScreens[0],sizeof(tMenuScreen)*SCREEN_COUNT);
    AddBlob("statics",reinterpret_cast<void*>(0x8CDFF4),16);
    AddBlob("camera_accel_horizontal",reinterpret_cast<void*>(0xB6EC1C),4); AddBlob("mouse_steering_flying",reinterpret_cast<void*>(0xC1CC02),2); AddBlob("invert_mouse",reinterpret_cast<void*>(0xBA6744),2);
    AddBlob("stat_id",reinterpret_cast<void*>(0xB794CC),4);
    AddBlob("slots",&CGenericGameStorage::ms_Slots,sizeof(CGenericGameStorage::ms_Slots)); AddBlob("slot_names",&CGenericGameStorage::ms_SlotFileName,sizeof(CGenericGameStorage::ms_SlotFileName));
    AddBlob("briefs",&CMessages::PreviousBriefs,sizeof(CMessages::PreviousBriefs));
    AddBlob("zones_revealed",reinterpret_cast<void*>(0xBA372C),4); AddBlob("zones_visited",reinterpret_cast<void*>(0xBA3730),100);
    AddBlob("legend_counter",reinterpret_cast<void*>(0xBA86B8),2); AddBlob("legend_list",&CRadar::MapLegendList,sizeof(CRadar::MapLegendList));
    AddBlob("scripts",reinterpret_cast<void*>(0xA444A1),4);
    AddBlob("radar_sincos",reinterpret_cast<void*>(0xBA8308),8); AddBlob("radar_range",reinterpret_cast<void*>(0xBA8314),4); AddBlob("radar_origin",reinterpret_cast<void*>(0xBAA248),8);
    AddBlob("user_tracks_state",reinterpret_cast<void*>(0xB6B988),4);
    AddBlob("cd_error_loaded",reinterpret_cast<void*>(0xC1B362),1); AddBlob("cd_error_text",reinterpret_cast<void*>(0xC1B36C),256);
    const std::vector<uint8> savedReal(reinterpret_cast<uint8*>(Real()),reinterpret_cast<uint8*>(Real())+OBJ);
    auto restoreBlobs=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.addr),b.saved.data(),b.size); };
    auto restoreReal=[&]{ std::memcpy(reinterpret_cast<void*>(Real()),savedReal.data(),OBJ); };
    // datos fijos de los stubs
    for(size_t i=0;i<videoStrings.size();++i){
        static const char* const modes[]{"640 x 480 (32 bit)","800 x 600","1024 x 768   (60 Hz)","1280 x 720 (32 bit) (16:9)","1920 x 1080  ","(x)","1366 x 768 (32 bit)","2560 x 1440 (60 Hz)","320 x 200","1600 x 900 (32 bit)"};
        std::snprintf(videoStrings[i].data(),videoStrings[i].size(),"%s",modes[i%std::size(modes)]); }
    for(size_t i=0;i<videoPtrs.size();++i)videoPtrs[i]=videoStrings[i%videoStrings.size()].data();
    for(size_t i=0;i<dateText.size();++i){ std::snprintf(reinterpret_cast<char*>(dateText[i].data()),dateText[i].size(),"%02zu/10/2026 12:%02zu",i+1,i*7); }

    bool passed=true; int pair=0;
    {
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto P=[&](uintptr_t dll,uintptr_t exeAddr,auto fn){ patches.push_back(std::make_unique<EntryPatch>(dll,fn)); patches.push_back(std::make_unique<EntryPatch>(exeAddr,fn)); };
        using std::bit_cast;
        // CFont
        P(bit_cast<uintptr_t>(&CFont::SetBackground),0x7195C0,FSetBackground); P(bit_cast<uintptr_t>(&CFont::SetProportional),0x7195B0,FSetProportional);
        P(bit_cast<uintptr_t>(&CFont::SetFontStyle),0x719490,FSetFontStyle); P(bit_cast<uintptr_t>(&CFont::SetScale),0x719380,FSetScale);
        P(bit_cast<uintptr_t>(&CFont::SetScaleForCurrentLanguage),0x7193A0,FSetScaleLang); P(bit_cast<uintptr_t>(&CFont::SetOrientation),0x719610,FSetOrientation);
        P(bit_cast<uintptr_t>(&CFont::SetColor),0x719430,FSetColor); P(bit_cast<uintptr_t>(&CFont::SetDropColor),0x719510,FSetDropColor);
        P(bit_cast<uintptr_t>(&CFont::SetEdge),0x719590,FSetEdge); P(bit_cast<uintptr_t>(&CFont::SetDropShadowPosition),0x719570,FSetDropShadowPosition);
        P(bit_cast<uintptr_t>(&CFont::SetWrapx),0x7194D0,FSetWrapx); P(bit_cast<uintptr_t>(&CFont::SetCentreSize),0x7194E0,FSetCentreSize);
        P(bit_cast<uintptr_t>(&CFont::SetRightJustifyWrap),0x7194F0,FSetRightJustifyWrap); P(bit_cast<uintptr_t>(&CFont::SetJustify),0x719600,FSetJustify);
        P(bit_cast<uintptr_t>(&CFont::SetAlphaFade),0x719500,FSetAlphaFade); P(bit_cast<uintptr_t>(&CFont::InitPerFrame),0x719800,FInitPerFrame);
        P(bit_cast<uintptr_t>(&CFont::RenderFontBuffer),0x719840,FRenderFontBuffer); P(bit_cast<uintptr_t>(&CFont::DrawFonts),0x71A210,FDrawFonts);
        P(bit_cast<uintptr_t>(&CFont::PrintString),0x71A700,FPrintString); P(bit_cast<uintptr_t>(&CFont::PrintStringFromBottom),0x71A820,FPrintStringFromBottom);
        P(bit_cast<uintptr_t>(&CFont::GetStringWidth),0x71A0E0,FGetStringWidth); P(bit_cast<uintptr_t>(&CFont::GetNumberLines),0x71A5E0,FGetNumberLines);
        P(bit_cast<uintptr_t>(&CFont::GetTextRect),0x71A620,FGetTextRect);
        // CSprite2d
        P(bit_cast<uintptr_t>(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),0x727B60,SDrawRect);
        P(bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const CRect&,const CRGBA&)>(&CSprite2d::Draw)),0x728350,SDrawSpriteRect);
        P(bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(float,float,float,float,const CRGBA&)>(&CSprite2d::Draw)),0x7282C0,SDrawSpriteXY);
        P(bit_cast<uintptr_t>(&CSprite2d::Draw2DPolygon),0x7285B0,SDraw2DPolygon); P(bit_cast<uintptr_t>(&CSprite2d::DrawBarChart),0x728640,SDrawBarChart);
        P(bit_cast<uintptr_t>(&CSprite2d::SetRecipNearClip),0x727260,SSetRecipNearClip); P(bit_cast<uintptr_t>(&CSprite2d::InitPerFrame),0x727350,SInitPerFrame);
        // texto, motor, audio y varios
        P(bit_cast<uintptr_t>(&CText::Get),0x6A0050,FakeTextGet);
        P(bit_cast<uintptr_t>(&RsCameraBeginUpdate),0x619450,FakeCameraBegin); P(bit_cast<uintptr_t>(&DefinedState2d),0x734750,FakeDefinedState2d);
        P(bit_cast<uintptr_t>(&CMenuManager::SetHelperText),0x57CD10,FakeSetHelperText); P(bit_cast<uintptr_t>(&CMenuManager::ResetHelperText),0x57CD30,FakeResetHelperText);
        P(bit_cast<uintptr_t>(&CMenuManager::CheckHover),0x57C4F0,FakeCheckHover); P(bit_cast<uintptr_t>(&CMenuManager::SaveSettings),0x57C660,FakeSaveSettings);
        P(bit_cast<uintptr_t>(&CMenuManager::SetFrontEndRenderStates),0x573A60,FakeSetFrontEndRenderStates);
        patches.push_back(std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CMenuManager::DrawBuildInfo),FakeDrawBuildInfo));
        P(bit_cast<uintptr_t>(&CAudioEngine::IsRadioRetuneInProgress),0x506FF0,FakeRetuneBusy); P(bit_cast<uintptr_t>(&CAudioEngine::GetRadioStationName),0x507000,FakeRadioName);
        P(bit_cast<uintptr_t>(&CAEUserRadioTrackManager::ScanUserTracks),0x4F4BA0,FakeScanUserTracks);
        P(bit_cast<uintptr_t>(static_cast<bool(CAEUserRadioTrackManager::*)()>(&CAEUserRadioTrackManager::Initialise)),0x4F35B0,FakeUserTracksInit);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::GetControllerSettingTextWithOrderNumber),0x531E20,FakeCtrlText);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::GetButtonComboText),0x52F560,FakeComboText);
        P(bit_cast<uintptr_t>(&Fx_c::GetFxQuality),0x49EA50,FakeFxQuality); P(bit_cast<uintptr_t>(&GetVideoModeList),0x745AF0,FakeVideoModes);
        P(bit_cast<uintptr_t>(static_cast<uint8*(*)(uint32,uint32,uint32)>(&CMemoryMgr::Calloc)),0x72F460,FakeCalloc); P(bit_cast<uintptr_t>(&CMemoryMgr::Free),0x72F430,FakeFree);
        P(bit_cast<uintptr_t>(&CStats::ConstructStatLine),0x55A780,FakeConstructStatLine); P(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),0x558E40,FakeGetStatValue);
        P(bit_cast<uintptr_t>(&CStats::FindCriminalRatingNumber),0x559080,FakeCriminalRatingNumber); P(bit_cast<uintptr_t>(&CStats::FindCriminalRatingString),0x55A210,FakeCriminalRatingString);
        P(bit_cast<uintptr_t>(&CMessages::InsertNumberInString),0x69DE90,FakeInsertNumber); P(bit_cast<uintptr_t>(&CMessages::InsertStringInString),0x69E040,FakeInsertString);
        P(bit_cast<uintptr_t>(&CMessages::InsertPlayerControlKeysInString),0x69E160,FakeInsertKeys); P(bit_cast<uintptr_t>(&GetSavedGameDateAndTime),0x618D00,FakeSavedDate);
        P(bit_cast<uintptr_t>(&CRadar::InitFrontEndMap),0x585960,FakeInitFrontEndMap); P(bit_cast<uintptr_t>(&CRadar::DrawRadarSectionMap),0x586520,FakeDrawRadarSection);
        P(bit_cast<uintptr_t>(&CRadar::DrawRadarGangOverlay),0x586650,FakeDrawGangOverlay); P(bit_cast<uintptr_t>(&CRadar::DrawBlips),0x588050,FakeDrawBlips);
        P(bit_cast<uintptr_t>(&CRadar::DrawLegend),0x5828A0,FakeDrawLegend); P(bit_cast<uintptr_t>(&CPlaceName::GetForMap),0x571D90,FakePlaceName);
        P(bit_cast<uintptr_t>(&CTheZones::GetCurrentZoneLockedOrUnlocked),0x572130,FakeZoneLocked); P(bit_cast<uintptr_t>(&FindPlayerCentreOfWorld_NoSniperShift),0x56E320,FakePlayerCentre);
        P(bit_cast<uintptr_t>(&CMenuSystem::Process),0x582630,FakeMenuSystemProcess); P(bit_cast<uintptr_t>(&CMenuSystem::SwitchOffMenu),0x580750,FakeMenuSystemSwitchOff);
        P(bit_cast<uintptr_t>(&RsEventHandler),0x619B60,FakeRsEvent);

        static const char* const keys[]{"FEM_OK","FEM_ON","FEM_OFF","FED_RES","FED_AAS","FEC_ERI","FEA_SMP","NOCD","OPENCD","WRONGCD","CDERROR","FEH_MPB","FET_APP","FEL_ENG","BB_19","MOD_01","XX","A","FESZ_QQ","FEA_PR1","DUMMY1","IE_N1"};
        static const int16 mActions[]{0,2,3,4,5,6,7,8,9,10,11,12,13,14,15,18,20,21,22,24,25,26,27,28,29,30,31,32,33,34,35,36,42,43,44,46,47,48,49,50,51,52,53,56,57,58,59,60,61,62,63,64,65};
        static const float timeDeltas[]{0,1,5,10,11,40,41,150,151,400,401,700,701,1000,1001,3000,3001,7000,7001,100000};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto chance=[&](uint32 n){return next()%n==0;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto key=[&]()->const char*{ return keys[next()%std::size(keys)]; };
                auto keyOrNull=[&]()->const char*{ const uint32 r=next()%8; return r==0 ? nullptr : r==1 ? "" : key(); };
                auto put=[&](char* dst,const char* s){ if(!s)s=""; std::memcpy(dst,s,std::strlen(s)+1); };
                std::vector<uint8> rawO, rawC;
                auto run=[&](bool candidate,std::vector<uint8>& rawOut)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x3E25u; if(!stt)stt=1; trace=json::array(); traceOver=0; poolNext=0; curScaleX=1.f;
                    restoreBlobs(); restoreReal();
                    auto* self=Obj();
                    for(auto& b:raw)b=uint8(next());
                    // ---------------- pantalla y reloj ----------------
                    static const int widths[]{640,640,800,1024,1280,1366,1920,2560}, heights[]{448,448,480,600,768,1024,1080,1440};
                    RsGlobal.maximumWidth=widths[next()%std::size(widths)]; RsGlobal.maximumHeight=heights[next()%std::size(heights)];
                    const uint32 now= chance(10) ? 0xFFFFFF00u+next()%0x100u : 100000u+next()%2000000u;
                    CTimer::m_snTimeInMillisecondsPauseMode=now; CTimer::m_FrameCounter=next();
                    auto past=[&]{ const uint32 d=uint32(timeDeltas[next()%std::size(timeDeltas)]); return chance(12) ? now+d : now-d; };
                    // ---------------- estado de mando, teclado y raton ----------------
                    auto& nk=CPad::NewKeyState; auto& ok=CPad::OldKeyState;
                    nk.esc= chance(3) ? 255 : 0; ok.esc= chance(2) ? 255 : 0; nk.standardKeys['Z']= chance(10) ? 255 : 0; nk.standardKeys['z']= chance(10) ? 255 : 0;
                    CPad::NewMouseControllerState.isMouseLeftButtonPressed= chance(2) ? 1 : 0;
                    // ---------------- objeto falso ----------------
                    self->m_nPlayerNumber= chance(4) ? 1 : 0;
                    self->m_nStatsScrollDirection= chance(2) ? 1 : 0; self->m_fStatsScrollSpeed= chance(6) ? (chance(2) ? 0.f : -uni(0.f,200.f)) : uni(10.f,400.f);
                    self->m_nSelectedRow=uint8(3+next()%17);
                    self->m_bHudOn=chance(2); self->m_nRadarMode=static_cast<eRadarMode>(chance(12) ? 3+next()%3 : next()%3); self->m_nTargetBlipIndex=int32(next()%30);
                    self->m_nSysMenu= chance(2) ? int8(-99) : int8(next()%3); self->m_bDontDrawFrontEnd=chance(5); self->m_KeyPressedCode=static_cast<RsKeyCodes>(int(next()%200)-1);
                    self->m_PrefsBrightness=int32(next()%520)-5; self->m_fDrawDistance=uni(0.7f,2.2f);
                    self->m_bShowSubtitles=chance(2); for(auto& b:self->m_abPrefsMapBlips)b=chance(2);
                    self->m_bMapLegend=chance(2); self->m_bWidescreenOn=chance(2); self->m_bPrefsFrameLimiter=chance(2); self->m_bRadioAutoSelect=chance(2); self->m_PrefsAudioOutputMode=chance(2);
                    self->m_nSfxVolume=int8(int(next()%75)-3); self->m_nRadioVolume=int8(int(next()%75)-3); self->m_bRadioEq=chance(2);
                    self->m_nRadioStation=static_cast<eRadioID>(int(next()%18)-3); self->m_nCurrentScreenItem=int32(next()%12);
                    self->m_bDrawingMap=chance(3); self->m_bStreamingDisabled=chance(3); self->m_bAllStreamingStuffLoaded=chance(3);
                    self->m_fMapZoom=uni(70.f,1200.f); self->m_vMapOrigin.x=uni(0.f,640.f); self->m_vMapOrigin.y=uni(0.f,448.f);
                    self->m_vMousePos.x=uni(-3100.f,3100.f); self->m_vMousePos.y=uni(-3100.f,3100.f); self->m_bMapLoaded=chance(4)==false;
                    self->m_nPrefsLanguage=static_cast<eLanguage>(chance(10) ? 5+next()%3 : next()%5);
                    self->m_ListSelection=int32(next()%30)-2; self->m_pPressedKey= chance(2) ? &self->m_KeyPressedCode : nullptr; self->m_RadioMode=static_cast<eRadioMode>(int(next()%5)-1);
                    self->m_bInvertPadX1=chance(2); self->m_bInvertPadY1=chance(2); self->m_bInvertPadX2=chance(2); self->m_bInvertPadY2=chance(2); self->m_bSwapPadAxis1=chance(2); self->m_bSwapPadAxis2=chance(2);
                    self->m_RedefiningControls= chance(2) ? eControlMode::FOOT : eControlMode::VEHICLE; self->m_DisplayTheMouse=chance(2);
                    self->m_nMousePosX=int32(next()%2800)-100; self->m_nMousePosY=int32(next()%1700)-100;
                    self->m_bPrefsMipMapping=chance(2); self->m_bTracksAutoScan=chance(2); self->m_nPrefsAntialiasing=int32(next()%5); self->m_nDisplayAntialiasing= chance(2) ? self->m_nPrefsAntialiasing : int32(next()%5);
                    self->m_ControlMethod= chance(2) ? eController::MOUSE_PLUS_KEYS : eController::JOYPAD; self->m_nPrefsVideoMode=int32(next()%10); self->m_nDisplayVideoMode= chance(2) ? self->m_nPrefsVideoMode : int32(next()%10);
                    self->m_bSavePhotos=chance(2); self->m_bMainMenuSwitch=chance(2);
                    self->m_bTexturesLoaded= chance(10)==false; self->m_nCurrentScreen=static_cast<eMenuScreen>(next()%SCREEN_COUNT);
                    self->m_SelectedSlot=uint8(next()%8); self->m_nMissionPackGameId=uint8(next()%4);
                    for(int k=0;k<25;++k){ self->m_MissionPacks[k].m_Id=uint8(chance(3) ? 0 : 1+next()%3); char nameBuf[24]{}; std::snprintf(nameBuf,sizeof(nameBuf),"PACK%d",k); put(self->m_MissionPacks[k].m_Name,nameBuf); }
                    self->m_nPressedMouseButton=static_cast<RsKeyCodes>(next()%8); self->m_nJustDownJoyButton=int32(next()%20);
                    self->m_MenuIsAbleToQuit=chance(3); self->m_RadioAvailable=chance(2); self->m_ControllerError=static_cast<eControllerError>(chance(3) ? 1+next()%4 : 0); self->m_bScanningUserTracks=chance(6);
                    self->m_nHelperTextFadingAlpha=int32(next()%420)-10; for(auto& k:self->m_KeyPressed)k=chance(2);
                    self->m_nOldMousePosX= chance(3) ? self->m_nMousePosX : int32(next()%2800)-100; self->m_nOldMousePosY= chance(3) ? self->m_nMousePosY : int32(next()%1700)-100;
                    { static const int bounds[]{2,3,4,5,6,7,8,9,10,11,12,13,14,15,16}; self->m_MouseInBounds=static_cast<eMouseInBounds>(bounds[next()%std::size(bounds)]); }
                    self->m_CurrentMouseOption=int32(next()%14)-1; self->m_bJustOpenedControlRedefWindow=chance(3); self->m_EditingControlOptions=chance(3);
                    self->m_DeleteAllBoundControls=chance(3); self->m_DeleteAllNextDefine=chance(3); self->m_OptionToChange=int32(next()%64); self->m_CanBeDefined=chance(2); self->m_JustExitedRedefine=chance(3);
                    self->m_HelperText=static_cast<eHelperText>(next()%9);
                    // campos del singleton que el EXE toca por direccion absoluta (tambien en el objeto falso para el candidato)
                    self->m_StatsScrollTime=past(); self->m_bViewRadar=chance(2); self->m_RadarVisibilityChangeTime=past(); self->m_BriefsArrowBlinkTime=past(); self->m_UserTrackScanningTime=past();
                    self->m_ErrorPendingReset=chance(3); self->m_ErrorStartTime=past(); self->field_1B4C=int32(next()%4); self->m_BackgroundSprite=static_cast<eFrontend>(next()%25);
                    self->m_InputWaitBlink=chance(2); self->m_LastBlinkTime=past(); self->m_HelperTextUpdatedTime=past(); self->m_OptionFlashColorState=chance(2); self->m_LastHighlightToggleTime=past();
                    // ---------------- globales ----------------
                    StaticRef<float>(0xB6EC1C)=uni(0.f,0.01f); StaticRef<uint8>(0xC1CC02)=chance(2); StaticRef<uint8>(0xC1CC03)=chance(2); StaticRef<uint8>(0xBA6745)=chance(2);
                    StaticRef<uint16>(0xB794CC)= chance(2) ? 0 : uint16(1+next()%300);
                    for(auto& s:CGenericGameStorage::ms_Slots)s=static_cast<eSlotState>(next()%4);
                    for(auto& n:CGenericGameStorage::ms_SlotFileName){ const uint32 len= chance(6) ? 254+next()%3 : next()%30; for(uint32 k=0;k<len;++k)n[k]=GxtChar('A'+(next()%26)); n[len]=0; }
                    StaticRef<uint8>(0x8CDFF9)=chance(2); StaticRef<uint8>(0x8CDFFA)=chance(2); StaticRef<int32>(0x8CDFFC)= chance(2) ? -1 : 1; StaticRef<int32>(0x8CE000)=int32(250+next()%140);
                    StaticRef<float>(0x8CDFF4)=uni(-300.f,300.f); StaticRef<int8>(0x8CDFF8)=int8(next()%12);
                    StaticRef<int32>(0xB6B988)=int32(next()%5);
                    TheText.m_bCdErrorLoaded=chance(2); { char cd[64]; std::snprintf(cd,sizeof(cd),"CDERR%u",next()%100u); std::memcpy(TheText.m_szCdErrorText,cd,std::strlen(cd)+1); }
                    ctrlSeed=next()%1000u; hoverMode=chance(3) ? 1+next()%2 : 0; fxQuality=int(next()%5); statCount=int(next()%26); cameraOk=chance(8)==false; retuneBusy=chance(3); scanRet=chance(2);
                    zoneLocked=chance(2); playerCentre=CVector{uni(-3100.f,3100.f),uni(-3100.f,3100.f),uni(0.f,50.f)};
                    StaticRef<float>(0xBA8308)=1.f; StaticRef<float>(0xBA830C)=0.f; StaticRef<float>(0xBA8314)=uni(500.f,3500.f); StaticRef<CVector2D>(0xBAA248)=CVector2D{uni(-500.f,500.f),uni(-500.f,500.f)};
                    if(chance(3)){ const float a=uni(0.f,6.28f); StaticRef<float>(0xBA8308)=std::cos(a); StaticRef<float>(0xBA830C)=std::sin(a); }
                    for(auto& b:*reinterpret_cast<std::array<bool,100>*>(0xBA3730))b=chance(3)!=0;
                    StaticRef<int32>(0xBA372C)= chance(2) ? int32(next()%80) : int32(80+next()%40);
                    StaticRef<uint16>(0xBA86B8)=uint16(chance(10) ? 0 : next()%36); for(auto& s:CRadar::MapLegendList)s=int16(next()%64);
                    StaticRef<bool>(0xA444A1)=chance(5); StaticRef<bool>(0xA444A4)=chance(5);
                    for(size_t k=0;k<briefText.size();++k){
                        std::snprintf(reinterpret_cast<char*>(briefText[k].data()),briefText[k].size(),"BRIEF%zu",k); std::snprintf(reinterpret_cast<char*>(briefStr[k].data()),briefStr[k].size(),"S%zu",k);
                        auto& b=CMessages::PreviousBriefs[k]; b.Text= chance(4) ? nullptr : briefText[k].data(); for(auto& n:b.NumbersToInsert)n=int32(next()%2000)-5; b.StringToInsert= chance(3) ? nullptr : briefStr[k].data(); }
                    { static const int special[]{0,2,3,4,5,6,9,10,11,12,16,17,26,27,28,33,34,35,36,38,39,40,42,43};
                      if((nm=="DrawStandardMenus"||nm=="DrawBackground"||nm=="DrawFrontEnd"||nm=="PrintMap"||nm=="DisplayHelperText")&&chance(2))self->m_nCurrentScreen=static_cast<eMenuScreen>(special[next()%std::size(special)]); }
                    // mutaciones de la pantalla actual (la tabla se restaura al terminar cada ejecucion)
                    if(nm=="DrawStandardMenus"||nm=="DrawBackground"||nm=="DrawFrontEnd"||nm=="PrintMap"){
                        const int cur= int(self->m_nCurrentScreen);
                        auto& sc=aScreens[cur];
                        if(chance(3)){ if(chance(3))put(sc.m_szTitleName,keyOrNull()); }
                        if(chance(2)){
                            for(int k=0;k<12;++k){ auto& it=sc.m_aItems[k]; const uint32 r=next()%9;
                                if(r==0||r==1){
                                    it.m_nActionType=static_cast<eMenuAction>(chance(8) ? 1 : mActions[next()%std::size(mActions)]);
                                    put(it.m_szName,keyOrNull());
                                    const uint32 t=next()%6; it.m_nType= (t==0 && k>=1 && k<=8) ? static_cast<eMenuEntryType>(1+next()%8) : (t==1 && k>=2) ? TI_MPACK : t==2 ? TI_MOUSEJOYPAD : TI_STRING;
                                    it.m_X= chance(3) ? 0 : uint16(next()%700); it.m_Y= chance(3) ? 0 : uint16(next()%500); it.m_nAlign=static_cast<eMenuAlign>(next()%4);
                                } else if(r==2){ it.m_X=0; it.m_Y=0; }
                                else if(r==3){ it.m_nActionType=static_cast<eMenuAction>(mActions[next()%std::size(mActions)]); }
                                else if(r==4){ it.m_nAlign=static_cast<eMenuAlign>(next()%4); } }
                        }
                    }
                    // el singleton real: lo que el EXE lee por direccion absoluta y lo que lee CRadar
                    std::vector<uint8> initMask(absMask.size()*4); { size_t o=0; for(auto& m:absMask){ std::memcpy(initMask.data()+o,OB()+m.first,m.second); o+=4; } }
                    CopyMask(reinterpret_cast<uint8*>(Real()),OB());
                    Real()->m_bDrawingMap=self->m_bDrawingMap; Real()->m_fMapZoom=self->m_fMapZoom; Real()->m_vMapOrigin=self->m_vMapOrigin;
                    if(nm=="PrintMap"||nm=="DrawBackground"||nm=="DrawFrontEnd"){ Real()->m_bDrawingMap=chance(8)==false; }
                    const std::vector<uint8> singleBefore(reinterpret_cast<uint8*>(Real()),reinterpret_cast<uint8*>(Real())+OBJ);
                    const std::vector<uint8> objBefore(OB(),OB()+OBJ);

                    // ---------------- argumentos y llamada por funcion ----------------
                    json ret=nullptr; const uintptr_t a=fn.address;
                    float fa=0,fb=0,fc=0,fd=0,fe=0,ff=0; int32 ia=0; const char* k1=nullptr; const char* k2=nullptr; bool b1=false,b2=false,b3=false; uint8 u8=0; uint16 u16=0; CRGBA col{0,0,0,0};
                    CRect rc;
                    if(nm=="DisplaySlider"){ fa=uni(0.f,1500.f); fb=uni(0.f,1200.f); fc= chance(10) ? 0.f : uni(0.f,60.f); fd= chance(10) ? fc : uni(0.f,60.f); fe=uni(0.f,400.f); /* h1, h2 >= 0 y separacion >= 0: con otros valores el CRect de la DLL aborta (assert) */
                        ff= chance(4) ? float(next()%17)/16.f+0.03125f : uni(-0.2f,1.2f); if(chance(30))ff=std::numeric_limits<float>::quiet_NaN(); ia=int32(next()%34); }
                    else if(nm=="DisplayHelperText"){ k1= chance(2) ? nullptr : key(); }
                    else if(nm=="SmallMessageScreen"||nm=="MessageScreen"){ k1=keyOrNull(); if(!k1)k1="FEM_OK"; b1=chance(2); b2=chance(2); }
                    else if(nm=="StretchX"||nm=="StretchY"){ fa= chance(8) ? 0.f : chance(8) ? std::numeric_limits<float>::quiet_NaN() : uni(-100.f,2000.f); }
                    else if(nm=="DrawWindow"){ auto* rf=reinterpret_cast<float*>(&rc); rf[0]=uni(-50.f,700.f); rf[1]=uni(-50.f,500.f); rf[2]=uni(-50.f,900.f); rf[3]=uni(-50.f,600.f);
                        if(chance(6))rf[1]=rf[3]; if(chance(12))rf[1]=std::numeric_limits<float>::quiet_NaN(); if(chance(6))rf[3]=-0.f, rf[1]=0.f;
                        k1=keyOrNull(); u8=uint8(chance(2) ? 0 : next()); col=CRGBA(uint8(next()),uint8(next()),uint8(next()),uint8(next())); b1=chance(2); b2=chance(2); }
                    else if(nm=="DrawWindowedText"){ fa=uni(0.f,700.f); fb=uni(0.f,500.f); fc= chance(6) ? uni(0.f,30.f) : uni(30.f,700.f); k1=keyOrNull(); k2=keyOrNull(); ia=int32(next()%4); }
                    else if(nm=="DrawControllerBound"){ u16= chance(2) ? 69 : uint16(next()%300); b1=chance(4)==false ? false : true; self->m_ControlMethod= chance(2) ? eController::MOUSE_PLUS_KEYS : eController::JOYPAD; }
                    else if(nm=="DrawControllerScreenExtraText"){ ia=int32(next()%400)-30; }
                    else if(nm=="DrawStandardMenus"){ b1=chance(5)==false; }
                    else if(nm=="PrintStats"){ self->m_nCurrentScreenItem=int32(next()%19)-4; }
                    else if(nm=="PrintRadioStationList"){ self->m_nRadioStation=static_cast<eRadioID>(int(next()%18)-3); }
                    else if(nm=="PrintMap"){ self->m_nSysMenu= chance(2) ? int8(-99) : int8(next()%3); }
                    else if(nm=="DrawFrontEnd"){ self->m_nCurrentScreen= chance(4) ? SCREEN_INITIAL : self->m_nCurrentScreen; if(chance(3))self->m_nCurrentScreenItem=0; }
                    else if(nm=="DrawQuitGameScreen"){ self->m_DisplayTheMouse=chance(2); }
                    // los campos del objeto falso cambiados arriba (p. ej. m_ControlMethod) deben verlos ambos lados: ya estan fijados antes de conmutar

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    if(nm=="DrawBackground"||nm=="DrawFrontEnd"||nm=="DrawQuitGameScreen"||nm=="PrintRadioStationList"||nm=="PrintBriefs"||nm=="PrintMap"||nm=="PrintStats"||nm=="DrawControllerSetupScreen") reinterpret_cast<void(__thiscall*)(M*)>(a)(self);
                    else if(nm=="DrawStandardMenus") reinterpret_cast<void(__thiscall*)(M*,bool)>(a)(self,b1);
                    else if(nm=="DrawWindow") reinterpret_cast<void(__thiscall*)(M*,const CRect&,const char*,uint8,CRGBA,bool,bool)>(a)(self,rc,k1,u8,col,b1,b2);
                    else if(nm=="DrawWindowedText") reinterpret_cast<void(__thiscall*)(M*,float,float,float,const char*,const char*,eFontAlignment)>(a)(self,fa,fb,fc,k1,k2,static_cast<eFontAlignment>(ia));
                    else if(nm=="DrawControllerBound") reinterpret_cast<void(__thiscall*)(M*,uint16,bool)>(a)(self,u16,b1);
                    else if(nm=="DrawControllerScreenExtraText") reinterpret_cast<void(__thiscall*)(M*,int32)>(a)(self,ia);
                    else if(nm=="DisplaySlider") ret=int(reinterpret_cast<int32(__thiscall*)(M*,float,float,float,float,float,float,int32)>(a)(self,fa,fb,fc,fd,fe,ff,ia));
                    else if(nm=="DisplayHelperText") reinterpret_cast<void(__thiscall*)(M*,const char*)>(a)(self,k1);
                    else if(nm=="MessageScreen") reinterpret_cast<void(__thiscall*)(M*,const char*,bool,bool)>(a)(self,k1,b1,b2);
                    else if(nm=="SmallMessageScreen") reinterpret_cast<void(__thiscall*)(M*,const char*)>(a)(self,k1);
                    else if(nm=="StretchX"||nm=="StretchY") ret=F(reinterpret_cast<float(__thiscall*)(M*,float)>(a)(self,fa));

                    // ---------------- estado efectivo ----------------
                    rawOut.assign(raw.begin(),raw.end());
                    uint8* singleNow=reinterpret_cast<uint8*>(Real());
                    bool leak=true;
                    { size_t o=0; for(auto& m:absMask){
                        if(!candidate){ leak&=std::memcmp(OB()+m.first,initMask.data()+o,m.second)==0; std::memcpy(rawOut.data()+GUARD+m.first,singleNow+m.first,m.second); }
                        else leak&=std::memcmp(singleNow+m.first,initMask.data()+o,m.second)==0;
                        o+=4; } }
                    bool singleOk=true; for(size_t k=0;k<OBJ;++k){ if(InMask(k)||(k>=0x59&&k<0x5A)||(k>=0x64&&k<0x70))continue; singleOk&=singleNow[k]==singleBefore[k]; }
                    const uint8* effObj=rawOut.data()+GUARD;
                    json blobsJson=json::array(); for(auto& b:blobs)blobsJson.push_back(Fnv(reinterpret_cast<void*>(b.addr),b.size));
                    json out={{"switches",switches},{"ret",ret},{"trace",trace},{"trace_over",traceOver},{"memory",Fnv(rawOut.data(),rawOut.size())},{"leak",leak},{"single_ok",singleOk},{"blobs",blobsJson}};
                    (void)effObj; (void)objBefore;
                    restoreReal(); restoreBlobs();
                    return out;
                };
                const auto original=run(false,rawO), candidate=run(true,rawC);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["leak"]==true&&original["single_ok"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json bytes=json::array(); for(size_t k=0;k<rawO.size()&&bytes.size()<64;++k)if(rawO[k]!=rawC[k])bytes.push_back({int(k)-int(GUARD),rawO[k],rawC[k]});
                    json firstTrace=nullptr; { const auto& to=original["trace"]; const auto& tc=candidate["trace"]; const size_t n=std::min(to.size(),tc.size()); size_t k=0; while(k<n&&to[k]==tc[k])++k;
                        firstTrace=json::array({int(k),int(to.size()),int(tc.size()),k<to.size() ? to[k] : json(nullptr),k<tc.size() ? tc[k] : json(nullptr)}); }
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"byte_diffs",bytes},{"first_trace_diff",firstTrace},
                           {"original",failures<=2 ? json({{"ret",original["ret"]},{"leak",original["leak"]},{"single_ok",original["single_ok"]},{"switches",original["switches"]}}) : json(nullptr)},
                           {"candidate",failures<=2 ? json({{"ret",candidate["ret"]},{"leak",candidate["leak"]},{"single_ok",candidate["single_ok"]},{"switches",candidate["switches"]}}) : json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        patches.clear();
    }
    for(size_t k=0;k<hooks.size();++k){hooks[k]->State(enabled[k]);hooks[k]->LockState(locked[k]);}
    restoreBlobs(); restoreReal();
    bool restored=std::memcmp(reinterpret_cast<void*>(Real()),savedReal.data(),OBJ)==0;
    for(auto& b:blobs)restored&=std::memcmp(reinterpret_cast<void*>(b.addr),b.saved.data(),b.size)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
