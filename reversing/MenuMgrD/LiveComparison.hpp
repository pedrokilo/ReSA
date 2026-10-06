#pragma once
// Banco dentro del motor: CMenuManager, grupo D (estado, texturas y ajustes; 13 funciones).
//   Constructor, Destructor, Initialise, LoadAllTextures, UnloadTextures, SwapTexturesRound, SetFrontEndRenderStates,
//   SetDefaultPreferences, LoadSettings, SaveSettings, SaveStatsToFile, ResetHelperText, SetHelperText.
// Original (hooks desactivados) frente a candidato (activados) con fixtures deterministas por caso.
// Nunca se toca el singleton real salvo los pocos bytes que el EXE lee de FrontEndMenuManager por direccion absoluta
// (0xBA6795..0xBA6799 y 0xBA8268), que se fijan por caso, se comparan y se restauran. El resto trabaja sobre un
// CMenuManager FALSO (0x1B78 bytes con guardas de 64 bytes; Constructor/Destructor sobre un bufer relleno de 0xCD).
// Se simulan y trazan: CTxdStore (Push/Pop/Find/Add/Load/AddRef/SetCurrent/RemoveTxd), CSprite2d (ctor/dtor/Delete/
// SetTexture/SetAddressing), CStreaming, CGame::TidyUpMemory, CTimer, CAudioEngine, Fx_c, CGamma, CFileMgr, CText,
// CControllerConfigManager, CStats, RwRenderStateSet (puntero fpRenderStateSet), RNG (0x821B1E + CGeneral::GetRandomNumber).
// Ficheros: gta_sa.set se simula en memoria (imagen de lectura / imagen escrita); stats.html se escribe en un fichero
// temporal del CRT de la DLL (candidato) o se captura con fopen/fprintf/fclose del EXE sustituidos (original).
// Se compara: retorno, bytes del objeto (con guardas), globales tocados, traza, imagen escrita y texto de stats.html.
#include <cstdarg>
#include <cstdio>
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MenuManager.h"
#include "Gamma.h"
#include "GenericGameStorage.h"
#include "C_PcSave.h"
#include "Radar.h"
#include "PostEffects.h"
#include "VideoMode.h"
#include "ControllerConfigManager.h"
#include "Cheat.h"
#include "FileMgr.h"
#include "../ProbeRuntime.hpp"

namespace MenuMgrDLive {
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

// --- RNG del EXE (0x821B1E) y de la DLL (CGeneral::GetRandomNumber), deterministas por caso ---
inline uint32 seed{}; inline uint32 rngCalls{};
inline int __cdecl FakeRand() { return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- etiquetas ---
inline uint8 fileTag{}, statsTag{};
inline FILE* statsTmp{};
inline json Tag(const void* p) {
    if(!p)return nullptr;
    const auto* b=static_cast<const uint8*>(p);
    if(b>=OB()&&b<OB()+OBJ)return json::array({"obj",int(b-OB())});
    if(p==&AudioEngine)return "audio"; if(p==&TheText)return "text"; if(p==&g_fx)return "fx"; if(p==&gamma)return "gamma";
    if(p==&ControlsManager)return "controls"; if(p==&s_PcSaveHelper)return "pcsave"; if(p==&FrontEndMenuManager)return "singleton";
    if(p==&fileTag)return "file"; if(p==&statsTag||(statsTmp&&p==statsTmp))return "stats";
    return "other";
}
inline json Txt(const void* p) {
    if(!p)return nullptr;
    auto* s=static_cast<const uint8*>(p); size_t n=0; while(n<512&&s[n])++n;
    bool ascii=true; for(size_t i=0;i<n;++i)ascii&=s[i]<0x80;
    if(ascii)return std::string(reinterpret_cast<const char*>(s),n);
    return ReversingProbe::Bytes(s,n);
}
inline int SprIdx(const void* p) {
    const auto d=static_cast<const uint8*>(p)-reinterpret_cast<const uint8*>(&Obj()->m_aFrontEndSprites[0]);
    return (d>=0&&d<int(25*sizeof(CSprite2d))) ? int(d/sizeof(CSprite2d)) : -1;
}

// --- CTxdStore: tabla de ranuras falsa ---
struct Slot { char name[16]; int32 id; };
inline std::array<Slot,16> slots{}; inline int nslots{}; inline int32 nextSlot{100};
inline void AddSlotEntry(const char* n,int32 id){ if(nslots<int(slots.size())){ std::strncpy(slots[nslots].name,n,15); slots[nslots].name[15]=0; slots[nslots].id=id; ++nslots; } }
inline int32 __cdecl FakeFindTxdSlot(const char* n){ int32 r=-1; for(int i=0;i<nslots;++i)if(!std::strcmp(slots[i].name,n)){r=slots[i].id;break;} trace.push_back({"txd_find",Txt(n),r}); return r; }
inline int32 __cdecl FakeAddTxdSlot(const char* n){ const int32 id=nextSlot++; AddSlotEntry(n,id); trace.push_back({"txd_add",Txt(n),id}); return id; }
inline bool __cdecl FakeLoadTxd(int32 s,const char* n){ trace.push_back({"txd_load",s,Txt(n)}); return true; }
inline void __cdecl FakeAddRef(int32 s){ trace.push_back({"txd_addref",s}); }
inline void __cdecl FakeSetCurrentTxd(int32 s){ trace.push_back({"txd_set_current",s}); }
inline void __cdecl FakeRemoveTxd(int32 s){ trace.push_back({"txd_remove",s}); }
inline void __cdecl FakePushTxd(){ trace.push_back({"txd_push"}); }
inline void __cdecl FakePopTxd(){ trace.push_back({"txd_pop"}); }

// --- streaming / juego / reloj ---
inline void __cdecl FakeMakeSpaceFor(size_t n){ trace.push_back({"make_space",uint32(n)}); }
inline void __cdecl FakeImGonna(){ trace.push_back({"streaming_use"}); }
inline void __cdecl FakeIHaveUsed(){ trace.push_back({"streaming_used"}); }
inline void __cdecl FakeTidyUp(bool a,bool b){ trace.push_back({"tidy_up",a,b}); }
inline void __cdecl FakeTimerStop(){ trace.push_back({"timer_stop"}); }
inline void __cdecl FakeTimerUpdate(){ trace.push_back({"timer_update"}); }
inline void __cdecl FakeStartUserPause(){ trace.push_back({"timer_start_user_pause"}); }

// --- CSprite2d ---
inline CSprite2d* __fastcall FakeSprCtor(CSprite2d* s,void*){ trace.push_back({"sprite_ctor",SprIdx(s)}); *reinterpret_cast<uint32*>(s)=0; return s; }
inline CSprite2d* __fastcall FakeSprDtor(CSprite2d* s,void*){ trace.push_back({"sprite_dtor",SprIdx(s)}); *reinterpret_cast<uint32*>(s)=0; return s; }
inline void __fastcall FakeSprDelete(CSprite2d* s,void*){ trace.push_back({"sprite_delete",SprIdx(s),*reinterpret_cast<uint32*>(s)}); *reinterpret_cast<uint32*>(s)=0; }
inline void __fastcall FakeSprSetTexture(CSprite2d* s,void*,const char* n,const char* m){
    trace.push_back({"sprite_set_texture",SprIdx(s),Txt(n),Txt(m)});
    *reinterpret_cast<uint32*>(s)=uint32(Fnv(n,std::strlen(n)))|1u; }
inline void __fastcall FakeSprSetAddressing(CSprite2d* s,void*,int32 mode){ trace.push_back({"sprite_set_addressing",SprIdx(s),mode}); }

// --- audio, Fx, gamma ---
inline int8 stationRet{};
inline void __fastcall FakeMusicVol(CAudioEngine* a,void*,int8 v){ trace.push_back({"music_volume",Tag(a),int(v)}); }
inline void __fastcall FakeEffectsVol(CAudioEngine* a,void*,int8 v){ trace.push_back({"effects_volume",Tag(a),int(v)}); }
inline void __fastcall FakeBass(CAudioEngine* a,void*,bool v){ trace.push_back({"bass_enhance",Tag(a),v}); }
inline void __fastcall FakeAutoRetune(CAudioEngine* a,void*,bool v){ trace.push_back({"radio_auto_retune",Tag(a),v}); }
inline void __fastcall FakeRetune(CAudioEngine* a,void*,int8 v){ trace.push_back({"retune_radio",Tag(a),int(uint8(v))}); }
inline int8 __fastcall FakeGetStation(CAudioEngine* a,void*){ trace.push_back({"get_radio_station",Tag(a)}); return stationRet; }
inline int32 fxQualityRet{};
inline void __fastcall FakeSetFxQuality(Fx_c* f,void*,int32 q){ trace.push_back({"fx_set_quality",Tag(f),q}); }
inline int32 __fastcall FakeGetFxQuality(Fx_c* f,void*){ trace.push_back({"fx_get_quality",Tag(f)}); return fxQualityRet; }
inline void __fastcall FakeSetGamma(CGamma* g,void*,float v,bool fade){ trace.push_back({"gamma",Tag(g),F(v),fade}); }

// --- Initialise ---
inline int32 __cdecl FakeChangeMsaa(uint32 n){ trace.push_back({"change_multisampling",n}); return 1; }
inline void __cdecl FakeSetVideoMode(int32 m){ trace.push_back({"set_video_mode",m}); }
inline void __fastcall FakeCentreMouse(CMenuManager* m,void*){ trace.push_back({"centre_mouse_pointer",Tag(m)}); }
inline void __cdecl FakeSetMapCentre(){ trace.push_back({"radar_set_map_centre"}); }
inline void __cdecl FakeStopPadsShaking(){ trace.push_back({"stop_pads_shaking"}); }
inline void __fastcall FakePopulateSlotInfo(C_PcSave* s,void*){ trace.push_back({"populate_slot_info",Tag(s)}); }

// --- ficheros ---
inline bool statsMode{}, fileOpen{}, ctrlLoadRet{};
inline std::array<uint8,512> image{}; inline size_t cursor{};
inline std::array<uint8,512> outImage{}; inline size_t outLen{};
inline std::string captured;
inline int32 rwSubsystemRet{};
inline json FileTag(const void* f){ return Tag(f); }
inline int32 __cdecl FakeSetDir(const char* p){ trace.push_back({"set_dir",Txt(p)}); return 0; }
inline int32 __cdecl FakeSetDirMyDocs(){ trace.push_back({"set_dir_my_documents"}); return 0; }
inline FILE* __cdecl FakeOpenFile(const char* path,const char* mode){
    trace.push_back({"open_file",Txt(path),Txt(mode)});
    if(!fileOpen)return nullptr;
    if(statsMode){ statsTmp=std::tmpfile(); return statsTmp; } // DLL: FILE* real del CRT de la DLL
    return reinterpret_cast<FILE*>(&fileTag);
}
inline size_t __cdecl FakeRead(FILE* f,void* buf,size_t n){
    const size_t avail=cursor<image.size() ? image.size()-cursor : 0, got=std::min(n,avail);
    trace.push_back({"read",FileTag(f),uint32(n),uint32(got),uint32(cursor)});
    if(got)std::memcpy(buf,image.data()+cursor,got);
    cursor+=got; return got;
}
inline size_t __cdecl FakeWrite(FILE* f,const void* buf,size_t n){
    trace.push_back({"write",FileTag(f),uint32(n),uint32(outLen)});
    const size_t room=outLen<outImage.size() ? outImage.size()-outLen : 0, put=std::min(n,room);
    if(put)std::memcpy(outImage.data()+outLen,buf,put);
    outLen+=put; return n;
}
inline bool __cdecl FakeSeek(FILE* f,long off,int32 origin){ trace.push_back({"seek",FileTag(f),off,origin}); if(origin==0)cursor=size_t(off); return false; }
inline int32 __cdecl FakeCloseFile(FILE* f){
    trace.push_back({"close_file",FileTag(f)});
    if(statsMode&&f&&f==statsTmp){ // DLL: se recoge el texto escrito en el fichero temporal antes de cerrarlo
        std::fflush(f); std::fseek(f,0,SEEK_SET);
        char b[4096]; size_t n; while((n=std::fread(b,1,sizeof b,f))>0)captured.append(b,n);
        std::fclose(f); statsTmp=nullptr;
    }
    return 0;
}
inline bool __fastcall FakeCtrlLoad(CControllerConfigManager* c,void*,FILE* f){ trace.push_back({"controls_load",Tag(c),FileTag(f)}); return ctrlLoadRet; }
inline bool __fastcall FakeCtrlSave(CControllerConfigManager* c,void*,FILE* f){ trace.push_back({"controls_save",Tag(c),FileTag(f)}); return true; }
inline int32 __cdecl FakeGetSubSystem(){ trace.push_back({"rw_get_current_subsystem"}); return rwSubsystemRet; }
inline void __fastcall FakeTextLoad(CText* t,void*,bool keep){ trace.push_back({"text_load",Tag(t),keep}); }
inline void __fastcall FakeInitChangedLanguage(CMenuManager* m,void*,bool reinit){ trace.push_back({"init_changed_language",Tag(m),reinit}); }

// --- CRT del EXE para SaveStatsToFile (solo mientras se prueba esa funcion) ---
inline bool statsOpenOk{};
inline FILE* __cdecl ExeFopen(const char* path,const char* mode){ trace.push_back({"open_file",Txt(path),Txt(mode)}); return statsOpenOk ? reinterpret_cast<FILE*>(&statsTag) : nullptr; }
inline int __cdecl ExeFprintf(FILE*,const char* fmt,...){
    char b[16384]; va_list ap; va_start(ap,fmt); const int n=std::vsnprintf(b,sizeof b,fmt,ap); va_end(ap);
    if(n>0)captured.append(b,size_t(std::min<int>(n,int(sizeof b)-1)));
    return n;
}
inline int __cdecl ExeFclose(FILE* f){ trace.push_back({"close_file",FileTag(f)}); return 0; }

// --- textos y estadisticas ---
inline uint32 textSalt{}; inline std::array<std::array<GxtChar,160>,64> pool{}; inline uint32 poolNext{};
inline const GxtChar* __fastcall FakeTextGet(CText* self,void*,const char* key){
    size_t n=0; while(key&&n<64&&key[n])++n;
    const std::string k(key ? key : "",n);
    trace.push_back({"text_get",Tag(self),Txt(key)});
    std::string s;
    switch((uint32(Fnv(k.data(),k.size()))^textSalt)%7){
    case 0: s=k; break;
    case 1: s="Mission "+k; break;
    case 2: s=k+" | "+k; break;
    case 3: s="mixed Case 123 "+k; break;
    case 4: s=""; break;
    case 5: s="alpha-beta_gamma "+k; break;
    default: s=k+" ok"; break;
    }
    auto& out=pool[poolNext++%pool.size()];
    std::memcpy(out.data(),s.data(),s.size()); out[s.size()]=0;
    return out.data();
}
inline uint32 statSalt{}; inline std::array<int32,8> statLines{}; inline int32 ratingNumber{};
inline GxtChar* __cdecl FakeRatingString(){ static GxtChar b[3][16]{}; const int k=int(statSalt%3); const char* s[]{"PUNK","BIG TIME","gangsta"}; std::strcpy(reinterpret_cast<char*>(b[k]),s[k]); trace.push_back({"rating_string"}); return b[k]; }
inline int32 __cdecl FakeRatingNumber(){ trace.push_back({"rating_number"}); return ratingNumber; }
inline int32 __cdecl FakeStatLine(int32 a,uint8 item){
    trace.push_back({"stat_line",a,int(item)});
    if(a==99999)return statLines[item&7];
    const uint32 h=uint32(Fnv(&a,4,Fnv(&item,1)))^statSalt;
    static const char* const names[]{"","Money earned","Distance run","Pie|Count","Respect 100","Time 3|4 Mins","x","Gym visits"};
    static const char* const values[]{"0","5","15","42","999","1234","-7","$12345|","100|50","2147483647","abc","3000","16777217","","12 hours","73"};
    std::strcpy(reinterpret_cast<char*>(gGxtString),names[(h>>3)%std::size(names)]);
    std::strcpy(reinterpret_cast<char*>(gGxtString2),values[(h>>9)%std::size(values)]);
    StaticRef<uint16>(0xB794CC)=uint16(h%3==0 ? 1+(h>>20)%5 : 0);
    return 0;
}

// --- RwRenderStateSet ---
inline RwBool __cdecl FakeRenderStateSet(RwRenderState s,void* v){ trace.push_back({"render_state",int(s),uint32(reinterpret_cast<uintptr_t>(v))}); return 1; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,13> fns{{
    {"CMenuManager","Constructor",0x574350,0,-1,256},{"CMenuManager","Destructor",0x579440,0,-1,256},
    {"CMenuManager","Initialise",0x5744D0,0,-1,1024},{"CMenuManager","LoadAllTextures",0x572EC0,0,-1,256},
    {"CMenuManager","UnloadTextures",0x574630,0,-1,256},{"CMenuManager","SwapTexturesRound",0x5730A0,0,-1,512},
    {"CMenuManager","SetFrontEndRenderStates",0x573A60,0,-1,64},{"CMenuManager","SetDefaultPreferences",0x573AE0,0,-1,1024},
    {"CMenuManager","LoadSettings",0x57C8F0,0,-1,2048},{"CMenuManager","SaveSettings",0x57C660,0,-1,512},
    {"CMenuManager","SaveStatsToFile",0x57DDE0,0,-1,256},{"CMenuManager","ResetHelperText",0x57CD30,0,-1,64},
    {"CMenuManager","SetHelperText",0x57CD10,0,-1,256},
}};

// Globales que el EXE/DLL pueden tocar (se guardan, se comparan por caso y se restauran).
struct Region { uintptr_t a; size_t n; };
inline const std::array<Region,10> regions{{
    {0xBA6795,5},   // FrontEndMenuManager +0x4D..+0x51: autoseleccion, modo de salida, SFX, radio, ecualizador
    {0xBA8268,1},   // FrontEndMenuManager.m_TexturesSwapped
    {0xB6EC1C,4},   // CCamera::m_fMouseAccelHorzntl
    {0xB6EC2E,1},   // CCamera::m_bUseMouse3rdPerson
    {0xBA6745,1},   // CMenuManager::bInvertMouseY
    {0xC1CC02,2},   // CVehicle::m_bEnableMouseSteering / m_bEnableMouseFlying
    {0x8CD800,4},   // CRenderer::ms_lodDistScale
    {0xC1AED8,1104},// gGxtString2 + gGxtString (contiguos)
    {0xB794CC,2},   // indicador de porcentaje de CStats::ConstructStatLine
    {0xB78A00,8},   // CStats::LastMissionPassedName
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"menumgrd-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"menumgrd-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("menumgrd");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"menumgrd-only.txt"); if(f)std::getline(f,only); }

    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(size_t f=0;f<fns.size();++f){ auto* h=FindHook(fns[f].cls,fns[f].name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} hooks.push_back(h); }
    std::vector<char> enabled(hooks.size()),locked(hooks.size());
    for(size_t k=0;k<hooks.size();++k){enabled[k]=hooks[k]->Hooked();locked[k]=hooks[k]->Locked();hooks[k]->LockState(false);}

    // estado global guardado
    std::array<std::vector<uint8>,regions.size()> saved;
    for(size_t k=0;k<regions.size();++k){ auto* p=reinterpret_cast<uint8*>(regions[k].a); saved[k].assign(p,p+regions[k].n); }
    auto& fpState=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedFp=fpState;

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        using TxdFind=int32(*)(const char*); using TxdLoad=bool(*)(int32,const char*);
        using SprTex2=void(CSprite2d::*)(const char*,const char*);
        std::vector<std::unique_ptr<EntryPatch>> patches;
        auto P=[&](uintptr_t dll,uintptr_t exeAddr,auto fn){ if(dll)patches.push_back(std::make_unique<EntryPatch>(dll,fn)); if(exeAddr)patches.push_back(std::make_unique<EntryPatch>(exeAddr,fn)); };
        // CTxdStore
        P(bit_cast<uintptr_t>(static_cast<TxdFind>(&CTxdStore::FindTxdSlot)),0x731850,FakeFindTxdSlot);
        P(bit_cast<uintptr_t>(&CTxdStore::AddTxdSlot),0x731C80,FakeAddTxdSlot);
        P(bit_cast<uintptr_t>(static_cast<TxdLoad>(&CTxdStore::LoadTxd)),0x7320B0,FakeLoadTxd);
        P(bit_cast<uintptr_t>(&CTxdStore::AddRef),0x731A00,FakeAddRef);
        P(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),0x7319C0,FakeSetCurrentTxd);
        P(bit_cast<uintptr_t>(&CTxdStore::RemoveTxd),0x731E90,FakeRemoveTxd);
        P(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),0x7316A0,FakePushTxd);
        P(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),0x7316B0,FakePopTxd);
        // streaming, juego, reloj
        P(bit_cast<uintptr_t>(&CStreaming::MakeSpaceFor),0x40E120,FakeMakeSpaceFor);
        P(bit_cast<uintptr_t>(&CStreaming::ImGonnaUseStreamingMemory),0x407BE0,FakeImGonna);
        P(bit_cast<uintptr_t>(&CStreaming::IHaveUsedStreamingMemory),0x407BF0,FakeIHaveUsed);
        P(bit_cast<uintptr_t>(&CGame::TidyUpMemory),0x53C500,FakeTidyUp);
        P(bit_cast<uintptr_t>(&CTimer::Stop),0x561AA0,FakeTimerStop);
        P(bit_cast<uintptr_t>(&CTimer::Update),0x561B10,FakeTimerUpdate);
        P(bit_cast<uintptr_t>(&CTimer::StartUserPause),0x561AF0,FakeStartUserPause);
        // CSprite2d
        P(bit_cast<uintptr_t>(&CSprite2d::Constructor),0x727230,FakeSprCtor);
        P(bit_cast<uintptr_t>(&CSprite2d::Destructor),0x7281E0,FakeSprDtor);
        P(bit_cast<uintptr_t>(&CSprite2d::Delete),0x727240,FakeSprDelete);
        P(bit_cast<uintptr_t>(static_cast<SprTex2>(&CSprite2d::SetTexture)),0x7272B0,FakeSprSetTexture);
        P(bit_cast<uintptr_t>(&CSprite2d::SetAddressing),0x727320,FakeSprSetAddressing);
        // audio, Fx, gamma
        P(bit_cast<uintptr_t>(&CAudioEngine::SetMusicMasterVolume),0x506DE0,FakeMusicVol);
        P(bit_cast<uintptr_t>(&CAudioEngine::SetEffectsMasterVolume),0x506E10,FakeEffectsVol);
        P(bit_cast<uintptr_t>(&CAudioEngine::SetBassEnhanceOnOff),0x506F90,FakeBass);
        P(bit_cast<uintptr_t>(&CAudioEngine::SetRadioAutoRetuneOnOff),0x506F80,FakeAutoRetune);
        P(bit_cast<uintptr_t>(&CAudioEngine::RetuneRadio),0x507E10,FakeRetune);
        P(bit_cast<uintptr_t>(&CAudioEngine::GetCurrentRadioStationID),0x507040,FakeGetStation);
        P(bit_cast<uintptr_t>(&Fx_c::SetFxQuality),0x49EA40,FakeSetFxQuality);
        P(bit_cast<uintptr_t>(&Fx_c::GetFxQuality),0x49EA50,FakeGetFxQuality);
        P(bit_cast<uintptr_t>(&CGamma::SetGamma),0x747200,FakeSetGamma);
        // Initialise
        P(bit_cast<uintptr_t>(&RwD3D9ChangeMultiSamplingLevels),0x7F8A90,FakeChangeMsaa);
        P(bit_cast<uintptr_t>(&SetVideoMode),0x745C70,FakeSetVideoMode);
        P(bit_cast<uintptr_t>(&CMenuManager::CentreMousePointer),0x57C520,FakeCentreMouse);
        P(bit_cast<uintptr_t>(&CRadar::SetMapCentreToPlayerCoords),0x585B20,FakeSetMapCentre);
        P(bit_cast<uintptr_t>(&CPad::StopPadsShaking),0x541D70,FakeStopPadsShaking);
        P(bit_cast<uintptr_t>(&C_PcSave::PopulateSlotInfo),0x619140,FakePopulateSlotInfo);
        // ajustes y ficheros
        P(bit_cast<uintptr_t>(&CFileMgr::SetDir),0x5387D0,FakeSetDir);
        P(bit_cast<uintptr_t>(&CFileMgr::SetDirMyDocuments),0x538860,FakeSetDirMyDocs);
        P(bit_cast<uintptr_t>(&CFileMgr::OpenFile),0x538900,FakeOpenFile);
        P(bit_cast<uintptr_t>(&CFileMgr::CloseFile),0x5389D0,FakeCloseFile);
        P(bit_cast<uintptr_t>(&CFileMgr::Read),0x538950,FakeRead);
        P(bit_cast<uintptr_t>(&CFileMgr::Write),0x538970,FakeWrite);
        P(bit_cast<uintptr_t>(&CFileMgr::Seek),0x538990,FakeSeek);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::LoadSettings),0x530530,FakeCtrlLoad);
        P(bit_cast<uintptr_t>(&CControllerConfigManager::SaveSettings),0x52D200,FakeCtrlSave);
        P(bit_cast<uintptr_t>(&RwEngineGetCurrentSubSystem),0x7F2C60,FakeGetSubSystem);
        P(bit_cast<uintptr_t>(&CText::Load),0x6A01A0,FakeTextLoad);
        P(bit_cast<uintptr_t>(&CMenuManager::InitialiseChangedLanguageSettings),0x573260,FakeInitChangedLanguage);
        // textos, estadisticas, RNG
        P(bit_cast<uintptr_t>(&CText::Get),0x6A0050,FakeTextGet);
        P(bit_cast<uintptr_t>(&CStats::FindCriminalRatingString),0x55A210,FakeRatingString);
        P(bit_cast<uintptr_t>(&CStats::FindCriminalRatingNumber),0x559080,FakeRatingNumber);
        P(bit_cast<uintptr_t>(&CStats::ConstructStatLine),0x55A780,FakeStatLine);
        patches.push_back(std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber));
        patches.push_back(std::make_unique<EntryPatch>(0x821B1E,FakeRand));
        std::vector<std::unique_ptr<EntryPatch>> crt; // CRT del EXE, solo durante SaveStatsToFile

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            if(nm=="SaveStatsToFile"){
                crt.push_back(std::make_unique<EntryPatch>(0x8232D8,ExeFopen));
                crt.push_back(std::make_unique<EntryPatch>(0x823A30,ExeFprintf));
                crt.push_back(std::make_unique<EntryPatch>(0x82318B,ExeFclose));
            }
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                std::string capO, capC; std::array<uint8,512> imgO{}, imgC{}; size_t lenO{}, lenC{};
                std::vector<uint8> rawO, rawC;
                auto run=[&](bool candidate,std::vector<uint8>& rawOut,std::string& capOut,std::array<uint8,512>& imgOut,size_t& lenOut)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4D4Du; if(!stt)stt=1;
                    trace=json::array(); rngCalls=0; poolNext=0; captured.clear(); cursor=0; outLen=0; statsTmp=nullptr;
                    std::fill(outImage.begin(),outImage.end(),uint8(0xEE)); std::fill(image.begin(),image.end(),uint8(0));
                    statsMode=nm=="SaveStatsToFile"; fileOpen=true; statsOpenOk=true;
                    const bool cd=nm=="Constructor" || (nm=="Destructor" && i%2==0);
                    if(cd)std::fill(raw.begin(),raw.end(),uint8(0xCD)); else for(auto& b:raw)b=uint8(next());
                    seed=next(); textSalt=next(); statSalt=next();
                    auto& m=*Obj();
                    // singleton: solo los bytes que el EXE lee por direccion absoluta
                    auto* single=reinterpret_cast<uint8*>(0xBA6795);
                    single[0]=uint8(next()&1); single[1]=uint8(next()&1); single[2]=uint8(next()%128); single[3]=uint8(next()%128); single[4]=uint8(next()&1);
                    *reinterpret_cast<uint8*>(0xBA8268)=uint8(next()&1);
                    // globales de ajustes
                    *reinterpret_cast<float*>(0xB6EC1C)=uni(0.f,0.01f); *reinterpret_cast<uint8*>(0xB6EC2E)=uint8(next()&1);
                    *reinterpret_cast<uint8*>(0xBA6745)=uint8(next()&1); *reinterpret_cast<uint8*>(0xC1CC02)=uint8(next()&1); *reinterpret_cast<uint8*>(0xC1CC03)=uint8(next()&1);
                    *reinterpret_cast<float*>(0x8CD800)=uni(0.5f,3.f);
                    // ranuras de TXD (frontend3 con id 0 activa el camino "!slot" del EXE)
                    nslots=0; nextSlot=100+int32(next()%50);
                    for(const char* s:{"frontend1","frontend_pc","frontend2","frontend3"}){ const uint32 r=next()%6; if(r==0)continue; AddSlotEntry(s,r==1 ? 0 : int32(1+next()%200)); }
                    stationRet=int8(int(next()%20)-3); fxQualityRet=next()%4==0 ? int32(next()) : int32(next()%4); rwSubsystemRet=int32(next()%4);
                    fpState=FakeRenderStateSet;

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json ret=nullptr;

                    if(nm=="Constructor"){ ret=Tag(reinterpret_cast<CMenuManager*(__fastcall*)(void*,void*)>(a)(Obj(),nullptr)); }
                    else if(nm=="Destructor"){ if(!cd)m.m_bTexturesLoaded=next()&1; reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr); }
                    else if(nm=="Initialise"){
                        m.m_bDoVideoModeUpdate=next()&1; m.m_nPrefsAntialiasing=next()%4==0 ? int32(next()) : int32(next()%9); m.m_nPrefsVideoMode=int32(next()%40);
                        m.m_isPreInitialised=next()&1;
                        static const float ov[]{0.f,1100.f,-1100.f,640.f,1740.f,448.f,1548.f,320.f,224.f,1099.f,1101.f,539.f,-0.f,2200.f};
                        m.m_vMapOrigin.x=next()%3==0 ? ov[next()%std::size(ov)] : uni(-3000.f,3000.f);
                        m.m_vMapOrigin.y=next()%3==0 ? ov[next()%std::size(ov)] : uni(-3000.f,3000.f);
                        reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr);
                    }
                    else if(nm=="LoadAllTextures"||nm=="UnloadTextures"){ m.m_bTexturesLoaded=next()&1; reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr); }
                    else if(nm=="SwapTexturesRound"){ const uint32 round=next()&1; reinterpret_cast<void(__fastcall*)(void*,void*,uint32)>(a)(Obj(),nullptr,round); }
                    else if(nm=="SetFrontEndRenderStates"){ reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr); }
                    else if(nm=="SetDefaultPreferences"){
                        static const int8 screens[]{3,4,27,36};
                        const int32 screen=next()%4!=0 ? int32(screens[next()%4]) : int32(int8(next()));
                        m.m_nPrefsVideoMode=int32(next()%40);
                        reinterpret_cast<void(__fastcall*)(void*,void*,int32)>(a)(Obj(),nullptr,screen);
                    }
                    else if(nm=="LoadSettings"){
                        m.m_nPrefsLanguage=static_cast<eLanguage>(next()%7); fileOpen=next()%8!=0; ctrlLoadRet=next()%6!=0;
                        for(auto& b:image)b=uint8(next());
                        const uint32 mode=next()%10;
                        if(mode==0){ std::memcpy(image.data(),"THIS FILE IS NOT VALID YET",26); }
                        else {
                            const int32 version=mode==1 ? int32(next()%6) : mode==2 ? -int32(next()%5+1) : int32(6+next()%3);
                            std::memcpy(image.data(),&version,4); size_t o=4;
                            auto u8=[&](uint8 v){ image[o++]=v; };
                            auto i32=[&](int32 v){ std::memcpy(image.data()+o,&v,4); o+=4; };
                            auto f32=[&](float v){ std::memcpy(image.data()+o,&v,4); o+=4; };
                            f32(uni(0.f,0.01f)); u8(uint8(next()&1)); u8(uint8(next()&1)); u8(uint8(next()&1));           // acel., invertir Y, direccion, vuelo
                            u8(uint8(next()%128)); u8(uint8(next()%128)); u8(next()%4==0 ? uint8(next()) : uint8(1+next()%13)); u8(uint8(next()&1)); u8(uint8(next()&1)); // SFX, radio, emisora, auto, EQ
                            i32(next()%3 ? int32(next()%513) : int32(next())); u8(uint8(next()&1)); u8(uint8(next()&1)); i32(int32(next()%9)); // brillo, mip, pistas, AA
                            i32(next()%4==0 ? int32(next()) : int32(next()%4)); u8(next()%8==0 ? uint8(next()) : uint8('T'));            // calidad Fx, 'T'
                            f32(uni(0.5f,3.f)); u8(uint8(next()&1)); u8(uint8(next()&1)); u8(uint8(next()&1));                         // distancia, subtitulos, pantalla ancha, limitador
                            i32(int32(next()%30)); u8(next()%4==0 ? uint8(next()) : uint8(next()&1));                                   // modo de video, control
                            u8(next()%3==0 ? uint8(next()) : uint8(next()%7));                                                         // idioma
                            u8(uint8(next()&1)); i32(next()%4==0 ? int32(next()) : int32(next()%3)); u8(next()%4==0 ? uint8(next()) : uint8(next()%3)); u8(uint8(next()&1)); // HUD, radar, modo radio, fotos
                            u8(next()%8==0 ? uint8(next()) : uint8(0x1D));
                            for(int k=0;k<6;++k)u8(uint8(next()&1));
                            u8(uint8(next()&1)); i32(int32(next())); i32(int32(next()%4)); u8(next()%8==0 ? uint8(next()) : uint8('_'));
                        }
                        reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr);
                    }
                    else if(nm=="SaveSettings"){ fileOpen=next()%8!=0; reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr); }
                    else if(nm=="SaveStatsToFile"){
                        statsOpenOk=fileOpen=next()%8!=0;
                        auto* name=reinterpret_cast<char*>(0xB78A00);
                        if(next()%3){ for(int k=0;k<7;++k)name[k]=char('A'+next()%26); name[7]=0; } else std::memset(name,0,8);
                        for(auto& l:statLines)l=int32(next()%5); ratingNumber=int32(next()%300);
                        reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr);
                    }
                    else if(nm=="ResetHelperText"){ reinterpret_cast<void(__fastcall*)(void*,void*)>(a)(Obj(),nullptr); }
                    else if(nm=="SetHelperText"){ const int32 id=next()%3 ? int32(next()%9)-1 : int32(next()); reinterpret_cast<void(__fastcall*)(void*,void*,int32)>(a)(Obj(),nullptr,id); }
                    if(ret.is_null()&&nm!="Constructor")ret=trace.size();
                    if(nm=="SaveStatsToFile")ret=json::array({uint32(Fnv(captured.data(),captured.size())),captured.size()});

                    rawOut.assign(raw.begin(),raw.end()); capOut=captured; imgOut=outImage; lenOut=outLen;
                    json regs=json::array(); for(const auto& r:regions)regs.push_back(Fnv(reinterpret_cast<const void*>(r.a),r.n));
                    json out={{"switches",switches},{"ret",ret},{"trace",trace},{"memory",Fnv(raw.data(),raw.size())},{"globals",regs},
                              {"rng",rngCalls},{"written",json::array({uint32(Fnv(outImage.data(),outImage.size())),outLen})},
                              {"stats",json::array({uint32(Fnv(captured.data(),captured.size())),captured.size()})}};
                    return out;
                };
                const auto original=run(false,rawO,capO,imgO,lenO), candidate=run(true,rawC,capC,imgC,lenC);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json bytes=json::array(); // [desplazamiento respecto al objeto (negativo = guarda previa), original, candidato]
                    for(size_t k=0;k<rawO.size()&&bytes.size()<64;++k)if(rawO[k]!=rawC[k])bytes.push_back({int(k)-int(GUARD),rawO[k],rawC[k]});
                    json text=nullptr; // primera diferencia del texto de stats.html: [indice, original, candidato]
                    if(capO!=capC){ size_t k=0; while(k<capO.size()&&k<capC.size()&&capO[k]==capC[k])++k;
                        auto win=[&](const std::string& s){ return ReversingProbe::Bytes(s.data()+std::min(k,s.size()),std::min<size_t>(80,s.size()-std::min(k,s.size()))); };
                        text=json::array({k,win(capO),win(capC)}); }
                    json written=nullptr;
                    if(imgO!=imgC||lenO!=lenC){ size_t k=0; while(k<imgO.size()&&imgO[k]==imgC[k])++k; written=json::array({k,k<imgO.size()?int(imgO[k]):-1,k<imgC.size()?int(imgC[k]):-1,lenO,lenC}); }
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"byte_diffs",bytes},{"stats_diff",text},{"written_diff",written},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
            crt.clear();
        }
    }
    for(size_t k=0;k<hooks.size();++k){hooks[k]->State(enabled[k]);hooks[k]->LockState(locked[k]);}
    for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].a),saved[k].data(),regions[k].n);
    fpState=savedFp;
    bool restored=fpState==savedFp;
    for(size_t k=0;k<regions.size();++k)restored&=std::memcmp(reinterpret_cast<const void*>(regions[k].a),saved[k].data(),regions[k].n)==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
