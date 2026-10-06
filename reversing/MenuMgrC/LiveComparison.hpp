#pragma once
// Banco dentro del motor: CMenuManager, grupo C (flujo y opciones): Process, ProcessMenuOptions, ProcessPCMenuOptions,
// ProcessMissionPackNewGame, CheckMissionPackValidMenu, SwitchToNewScreen, JumpToGenericMessageScreen,
// SaveLoadFileError_SetUpErrorScreen, GetNumberOfMenuOptions, ProcessFileActions, DoSettingsBeforeStartingAGame,
// ProcessStreaming, HasLanguageChanged e InitialiseChangedLanguageSettings.
// Original (hook del EXE desactivado) frente a candidato (hook activado) con fixtures deterministas por caso.
// NUNCA se toca el singleton real: se usa un CMenuManager FALSO (bufer del tamano de la clase con guardas de 64 bytes,
// relleno aleatorio y despues los campos que cada funcion lee fijados por caso). Solo se conmuta el hook de la funcion
// bajo prueba; todo lo que ella llama (el resto del grupo, audio, camara, radar, estadisticas, streaming, ficheros,
// partidas guardadas, texto, controles, modos de video, RNG, hilos de mensajes...) se sustituye en el EXE y en la DLL
// por falsos que anotan la llamada y sus argumentos. Los ficheros son falsos (manejadores y contenido en memoria) y las
// partidas guardadas son trazas: no se abre ningun fichero real ni se carga ninguna partida.
// Detalle importante: el EXE usa direcciones ABSOLUTAS del singleton para m_CurrentlyLoading/Deleting/Saving
// (0xBA8284/85/86) y m_nNumberOfMenuOptions (0xBA8269); la DLL usa this. Para la ejecucion "original" se copian esos 4
// bytes del falso al singleton real justo antes de la llamada y se devuelven (y se restauran) justo despues.
// Se compara: retorno, bytes del falso (con guardas), tabla aScreens, globales tocados, singleton real intacto y la traza.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MenuManager.h"
#include "GenericGameStorage.h"
#include "C_PcSave.h"
#include "Radar.h"
#include "PostEffects.h"
#include "VideoMode.h"
#include "ControllerConfigManager.h"
#include "Cheat.h"
#include "FileMgr.h"
#include "../ProbeRuntime.hpp"

namespace MenuMgrCLive {
using M = CMenuManager;
using ReversingProbe::EntryPatch;
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
// Sigue los saltos de la ILT de la DLL, pero nunca entra en la imagen del EXE.
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }
template<class T=uint8> T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }

// --- CMenuManager falso: guardas + objeto + guardas ---
constexpr size_t GUARD=64, SZ=sizeof(M);
static_assert(SZ==0x1B78);
alignas(16) inline std::array<uint8,GUARD+SZ+GUARD> raw{};
inline M* Fake(){ return reinterpret_cast<M*>(raw.data()+GUARD); }
inline int Self(const void* p){ return p==static_cast<const void*>(Fake()) ? 1 : p==static_cast<const void*>(&FrontEndMenuManager) ? 2 : 0; }

template<class... A> inline void Tr(const char* n,A... a){ trace.push_back(json::array({n,json(a)...})); }
inline json Txt(const void* p){
    if(!p)return nullptr;
    auto* s=static_cast<const uint8*>(p); size_t n=0; while(n<512&&s[n])++n;
    bool ascii=true; for(size_t i=0;i<n;++i)ascii&=s[i]<0x80;
    if(ascii)return std::string(reinterpret_cast<const char*>(s),n);
    return ReversingProbe::Bytes(s,n);
}

// --- fixtures que leen los falsos ---
inline uint32 rngSeed{}, rngCalls{};
inline int __cdecl FakeRand(){ return int((uint32(rngSeed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 __cdecl FakeRandomNumber(){ return uint16(FakeRand()); }
inline bool fxPcRet{}, fxCheckSlot{}, fxDeleteOk{}, fxScr{}, fxGxt{};
inline uint32 fxSaveRet{}, fxMaxMS{};
inline int fxFxQuality{}, fxNumModes{};
inline std::string fxMission;
inline std::array<std::string,25> packContent{}; inline std::array<bool,25> packExists{}; inline std::array<uint32,25> packPos{};
inline std::array<char*,24> modesBuf{};
inline int padIter{}, padExitAt{}, padKind{};
inline uint32 mapHash{}, mapCount{};
constexpr uintptr_t H_PACK=0x2000, H_SCR=0x3001, H_GXT=0x3002;

// --- falsos: miembros de CMenuManager (thiscall -> fastcall con ecx/edx) ---
inline void __fastcall FSaveSettings(M* s,void*){ Tr("SaveSettings",Self(s)); }
inline void __fastcall FSwitch(M* s,void*,eMenuScreen scr){ Tr("SwitchToNewScreen",Self(s),int(scr)); if(s==Fake()&&int(scr)>0)s->m_nCurrentScreen=scr; }
inline void __fastcall FDoSettings(M* s,void*){ Tr("DoSettingsBeforeStartingAGame",Self(s)); }
inline void __fastcall FJump(M* s,void*,eMenuScreen scr,const char* a,const char* b){ Tr("JumpToGenericMessageScreen",Self(s),int(scr),Txt(a),Txt(b)); }
inline void __fastcall FResetHelper(M* s,void*){ Tr("ResetHelperText",Self(s)); }
inline void __fastcall FCentreMouse(M* s,void*){ Tr("CentreMousePointer",Self(s)); }
inline void __fastcall FSetDefaults(M* s,void*,eMenuScreen scr){ Tr("SetDefaultPreferences",Self(s),int(uint8(scr))); }
inline void __fastcall FScrollRadio(M* s,void*,int8 n){ Tr("ScrollRadioStations",Self(s),int(n)); }
inline void __fastcall FInitLang(M* s,void*,bool b){ Tr("InitialiseChangedLanguageSettings",Self(s),int(b)); }
inline bool __fastcall FProcessPC(M* s,void*,int8 lr,bool accept){ Tr("ProcessPCMenuOptions",Self(s),int(lr),int(accept)); return fxPcRet; }
inline void __fastcall FNewGame(M* s,void*){ Tr("ProcessMissionPackNewGame",Self(s)); }
inline void __fastcall FProcessStreaming(M* s,void*,bool b){ Tr("ProcessStreaming",Self(s),int(b)); }
inline void __fastcall FUserInput(M* s,void*){ Tr("UserInput",Self(s)); }
inline void __fastcall FProcessFiles(M* s,void*){ Tr("ProcessFileActions",Self(s)); }
inline void __fastcall FCheckClosing(M* s,void*){ Tr("CheckForMenuClosing",Self(s)); }
inline void __fastcall FMessageScreen(M* s,void*,const char* key,bool black,bool cam){ Tr("MessageScreen",Self(s),Txt(key),int(black),int(cam)); }

// --- falsos: tiempo, texto, localizacion, controles, audio, camara, radar, estadisticas, HUD, streaming, D3D ---
inline void __cdecl FTimerStop(){ Tr("CTimer::Stop"); }
inline void __cdecl FTimerUpdate(){ Tr("CTimer::Update"); }
inline void __cdecl FStartUserPause(){ Tr("CTimer::StartUserPause"); }
inline void __cdecl FEndUserPause(){ Tr("CTimer::EndUserPause"); }
inline void __fastcall FGetMissionName(CText* s,void*,char* out){ Tr("GetNameOfLoadedMissionText",int(s==&TheText),Txt(fxMission.c_str())); std::strcpy(out,fxMission.c_str()); }
inline void __fastcall FTextLoad(CText* s,void*,bool keep){ Tr("CText::Load",int(s==&TheText),int(keep)); }
inline void __fastcall FLoadMission(CText* s,void*,const char* m){ Tr("CText::LoadMissionText",int(s==&TheText),Txt(m)); }
inline void __cdecl FSetNormalGame(){ Tr("CLocalisation::SetNormalGame"); }
inline void __cdecl FSetFrenchGame(){ Tr("CLocalisation::SetFrenchGame"); }
inline void __fastcall FReinit(CControllerConfigManager* s,void*){ Tr("ReinitControls",int(s==&ControlsManager)); }
inline void __fastcall FAEReset(CAudioEngine* s,void*){ Tr("AudioEngine::Reset",int(s==&AudioEngine)); }
inline void __fastcall FStopRadio(CAudioEngine* s,void*,tVehicleAudioSettings* v,bool b){ Tr("AudioEngine::StopRadio",int(s==&AudioEngine),int(v!=nullptr),int(b)); }
inline void __fastcall FStartRadio(CAudioEngine* s,void*,uint32 id,uint32 bass){ Tr("AudioEngine::StartRadio",int(s==&AudioEngine),id&0xFF,bass); }
inline void __fastcall FReportFrontend(CAudioEngine* s,void*,eAudioEvents ev,float vol,float speed){ Tr("AudioEngine::ReportFrontendAudioEvent",int(s==&AudioEngine),int(ev),F(vol),F(speed)); }
inline void __fastcall FBass(CAudioEngine* s,void*,bool b){ Tr("AudioEngine::SetBassEnhanceOnOff",int(s==&AudioEngine),int(b)); }
inline void __fastcall FAutoRetune(CAudioEngine* s,void*,bool b){ Tr("AudioEngine::SetRadioAutoRetuneOnOff",int(s==&AudioEngine),int(b)); }
inline void __cdecl FClearBlip(uint32 h){ Tr("CRadar::ClearBlip",h); }
inline void __cdecl FUpdateRespect(uint8 b){ Tr("CStats::UpdateRespectStat",int(b)); }
inline void __cdecl FSetZoneName(const GxtChar* n,bool b){ Tr("CHud::SetZoneName",Txt(n),int(b)); }
inline void __fastcall FSetFadeColour(CCamera* s,void*,uint8 r,uint8 g,uint8 b){ Tr("CCamera::SetFadeColour",int(s==&TheCamera),int(r),int(g),int(b)); }
inline void __fastcall FFade(CCamera* s,void*,float d,eFadeFlag f){ Tr("CCamera::Fade",int(s==&TheCamera),F(d),int(f)); }
inline void __fastcall FProcessFade(CCamera* s,void*){ Tr("CCamera::ProcessFade",int(s==&TheCamera)); }
inline void __cdecl FLoadRequested(){ Tr("CStreaming::LoadRequestedModels"); }
inline void __cdecl FLoadAllRequested(bool b){ Tr("CStreaming::LoadAllRequestedModels",int(b)); }
inline void __cdecl FRequestMap(int32 x,int32 y){ ++mapCount; mapHash=uint32(Fnv(&x,4,Fnv(&y,4,mapHash+1))); if(trace.size()<48)Tr("CRadar::RequestMapSection",x,y); }
inline void __cdecl FRemoveMap(int32 x,int32 y){ ++mapCount; mapHash=uint32(Fnv(&x,4,Fnv(&y,4,mapHash+2))); if(trace.size()<48)Tr("CRadar::RemoveMapSection",x,y); }
inline void __cdecl FTidyIB(uint32 n){ Tr("D3D::TidyUpD3DIndexBuffers",n); }
inline void __cdecl FTidyTex(uint32 n){ Tr("D3D::TidyUpD3DTextures",n); }

// --- falsos: ficheros (manejadores y contenido en memoria) ---
inline int fileFixture(const char* path,const char* tag){ const char* p=std::strstr(path,"MPACK//MPACK"); if(!p||!std::strstr(path,tag))return -1; return std::atoi(p+12); }
inline int32 __cdecl FSetDirMyDocuments(){ Tr("CFileMgr::SetDirMyDocuments"); return 1; }
inline int32 __cdecl FSetDir(const char* p){ Tr("CFileMgr::SetDir",Txt(p)); return 1; }
inline FILESTREAM __cdecl FOpenFile(const char* path,const char* mode){
    uintptr_t h=0;
    if(const int i=fileFixture(path,"MPACK.DAT"); i>=0&&i<25){ if(packExists[i]){ h=H_PACK+i; packPos[i]=0; } }
    else if(std::strstr(path,"SCR.SCM")){ if(fxScr)h=H_SCR; }
    else if(std::strstr(path,"TEXT.GXT")){ if(fxGxt)h=H_GXT; }
    Tr("CFileMgr::OpenFile",Txt(path),Txt(mode),uint32(h)); return reinterpret_cast<FILESTREAM>(h); }
inline int32 __cdecl FCloseFile(FILESTREAM f){ Tr("CFileMgr::CloseFile",uint32(reinterpret_cast<uintptr_t>(f))); return 0; }
inline int32 __cdecl FGetTotalSize(FILESTREAM f){ const auto h=reinterpret_cast<uintptr_t>(f); return h>=H_PACK&&h<H_PACK+25 ? int32(packContent[h-H_PACK].size()) : 0; }
inline size_t __cdecl FRead(FILESTREAM f,void* buf,size_t n){
    const auto h=reinterpret_cast<uintptr_t>(f); if(h<H_PACK||h>=H_PACK+25||n!=1)return 0;
    auto& c=packContent[h-H_PACK]; auto& pos=packPos[h-H_PACK]; if(pos>=c.size())return 0;
    *static_cast<char*>(buf)=c[pos++]; return 1; }

// --- falsos: partidas guardadas (nada se carga ni se escribe) ---
inline bool __cdecl FCheckSlot(int32 slot){ Tr("CGenericGameStorage::CheckSlotDataValid",slot); return fxCheckSlot; }
inline uint32 __fastcall FSaveSlot(C_PcSave* s,void*,int32 slot){ Tr("C_PcSave::SaveSlot",int(s==&s_PcSaveHelper),slot); return fxSaveRet; }
inline bool __fastcall FDeleteSlot(C_PcSave* s,void*,int32 slot){ Tr("C_PcSave::DeleteSlot",int(s==&s_PcSaveHelper),slot); return fxDeleteOk; }
inline void __fastcall FPopulate(C_PcSave* s,void*){ Tr("C_PcSave::PopulateSlotInfo",int(s==&s_PcSaveHelper)); }

// --- falsos: bucle de mensajes, pad, fotograma ---
inline void __cdecl FMessageLoop(){ Tr("MessageLoop"); }
inline void __cdecl FUpdatePads(){
    Tr("CPad::UpdatePads"); ++padIter;
    auto& n=CPad::NewKeyState; auto& o=CPad::OldKeyState; auto& pd=CPad::Pads[0];
    n.enter=o.enter=n.extenter=o.extenter=0; pd.NewState.ButtonCross=pd.OldState.ButtonCross=0;
    const bool hold=(padIter+padKind)%2==0;
    if(padIter<padExitAt){ if(hold){ n.enter=o.enter=1; n.extenter=o.extenter=1; pd.NewState.ButtonCross=pd.OldState.ButtonCross=255; } return; }
    switch(padKind){ case 0: n.enter=1; break; case 1: n.extenter=1; break; default: pd.NewState.ButtonCross=255; break; }
}
inline bool __cdecl FStartFrame(int16 a,int16 b,int16 c,int16 d,int16 e,int16 f,int16 g){ Tr("DoRWStuffStartOfFrame",a,b,c,d,e,f,g); return true; }
inline void __cdecl FEndFrame(){ Tr("DoRWStuffEndOfFrame"); }

// --- falsos: opciones de PC ---
inline FxQuality_e __fastcall FGetFxQuality(Fx_c* s,void*){ return FxQuality_e(fxFxQuality); }
inline void __fastcall FSetFxQuality(Fx_c* s,void*,FxQuality_e q){ Tr("Fx::SetFxQuality",int(s==&g_fx),int(q)); fxFxQuality=int(q); }
inline char** __cdecl FGetVideoModeList(){ return modesBuf.data()+2; }
inline void __cdecl FSetVideoMode(int32 m){ Tr("SetVideoMode",m); }
inline int32 __cdecl FNumVideoModes(){ return fxNumModes; }
inline uint32 __cdecl FMaxMS(){ return fxMaxMS; }
inline int __cdecl FChangeMS(uint32 n){ Tr("RwD3D9ChangeMultiSamplingLevels",n); return 1; }
inline int __cdecl FSetMipmapping(int b){ Tr("RwTextureSetMipmapping",b); return 1; }
inline void __cdecl FScreenModeInit(){ Tr("CPostEffects::DoScreenModeDependentInitializations"); }

template<class R,class... A> inline R CallThis(uintptr_t a,M* m,A... args){ return reinterpret_cast<R(__thiscall*)(M*,A...)>(a)(m,args...); }

// --- parches de una sola ejecucion ---
struct Patches {
    std::array<std::unique_ptr<EntryPatch>,512> P{}; size_t n{}; std::array<uintptr_t,512> seen{};
    template<class Fn> void Add(uintptr_t at,Fn fake){ for(size_t k=0;k<n;++k)if(seen[k]==at)return; seen[n]=at; P[n++]=std::make_unique<EntryPatch>(at,fake); }
    ~Patches(){ while(n)P[--n].reset(); }
};
// Instala todos los falsos salvo el de la funcion bajo prueba.
inline void InstallStubs(Patches& pt,const std::string& nm){
    using std::bit_cast;
    auto add=[&](const char* owner,uintptr_t exe,uintptr_t dll,auto fake){ if(nm==owner)return; pt.Add(exe,fake); pt.Add(Body(dll),fake); };
    add("SaveSettings",0x57C660,bit_cast<uintptr_t>(&M::SaveSettings),FSaveSettings);
    add("SwitchToNewScreen",0x573680,bit_cast<uintptr_t>(&M::SwitchToNewScreen),FSwitch);
    add("DoSettingsBeforeStartingAGame",0x573330,bit_cast<uintptr_t>(&M::DoSettingsBeforeStartingAGame),FDoSettings);
    add("JumpToGenericMessageScreen",0x576AE0,bit_cast<uintptr_t>(&M::JumpToGenericMessageScreen),FJump);
    add("ResetHelperText",0x57CD30,bit_cast<uintptr_t>(&M::ResetHelperText),FResetHelper);
    // SetHelperText NO se sustituye: el EXE lo inlinea en ProcessPCMenuOptions (RESET_CFG), asi que se compara el estado real
    add("CentreMousePointer",0x57C520,bit_cast<uintptr_t>(&M::CentreMousePointer),FCentreMouse);
    add("SetDefaultPreferences",0x573AE0,bit_cast<uintptr_t>(&M::SetDefaultPreferences),FSetDefaults);
    add("ScrollRadioStations",0x573A00,bit_cast<uintptr_t>(&M::ScrollRadioStations),FScrollRadio);
    add("InitialiseChangedLanguageSettings",0x573260,bit_cast<uintptr_t>(&M::InitialiseChangedLanguageSettings),FInitLang);
    add("ProcessPCMenuOptions",0x57CD50,bit_cast<uintptr_t>(&M::ProcessPCMenuOptions),FProcessPC);
    add("ProcessMissionPackNewGame",0x57D520,bit_cast<uintptr_t>(&M::ProcessMissionPackNewGame),FNewGame);
    add("ProcessStreaming",0x573CF0,bit_cast<uintptr_t>(&M::ProcessStreaming),FProcessStreaming);
    add("UserInput",0x57FD70,bit_cast<uintptr_t>(&M::UserInput),FUserInput);
    add("ProcessFileActions",0x578D60,bit_cast<uintptr_t>(&M::ProcessFileActions),FProcessFiles);
    add("CheckForMenuClosing",0x576B70,bit_cast<uintptr_t>(&M::CheckForMenuClosing),FCheckClosing);
    add("MessageScreen",0x579330,bit_cast<uintptr_t>(&M::MessageScreen),FMessageScreen);
    add("-",0x561AA0,bit_cast<uintptr_t>(&CTimer::Stop),FTimerStop);
    add("-",0x561B10,bit_cast<uintptr_t>(&CTimer::Update),FTimerUpdate);
    add("-",0x561AF0,bit_cast<uintptr_t>(&CTimer::StartUserPause),FStartUserPause);
    add("-",0x561B00,bit_cast<uintptr_t>(&CTimer::EndUserPause),FEndUserPause);
    add("-",0x69FBD0,bit_cast<uintptr_t>(&CText::GetNameOfLoadedMissionText),FGetMissionName);
    add("-",0x6A01A0,bit_cast<uintptr_t>(&CText::Load),FTextLoad);
    add("-",0x69FBF0,bit_cast<uintptr_t>(&CText::LoadMissionText),FLoadMission);
    add("-",0x56D1A0,bit_cast<uintptr_t>(&CLocalisation::SetNormalGame),FSetNormalGame);
    add("-",0x56D1C0,bit_cast<uintptr_t>(&CLocalisation::SetFrenchGame),FSetFrenchGame);
    add("-",0x531F20,bit_cast<uintptr_t>(&CControllerConfigManager::ReinitControls),FReinit);
    add("-",0x507A90,bit_cast<uintptr_t>(&CAudioEngine::Reset),FAEReset);
    add("-",0x506F70,bit_cast<uintptr_t>(&CAudioEngine::StopRadio),FStopRadio);
    add("-",0x507DC0,bit_cast<uintptr_t>(static_cast<void(CAudioEngine::*)(eRadioID,eBassSetting)>(&CAudioEngine::StartRadio)),FStartRadio);
    add("-",0x506EA0,bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),FReportFrontend);
    add("-",0x506F90,bit_cast<uintptr_t>(&CAudioEngine::SetBassEnhanceOnOff),FBass);
    add("-",0x506F80,bit_cast<uintptr_t>(&CAudioEngine::SetRadioAutoRetuneOnOff),FAutoRetune);
    add("-",0x587CE0,bit_cast<uintptr_t>(&CRadar::ClearBlip),FClearBlip);
    add("-",0x584B50,bit_cast<uintptr_t>(&CRadar::RequestMapSection),FRequestMap);
    add("-",0x584BB0,bit_cast<uintptr_t>(&CRadar::RemoveMapSection),FRemoveMap);
    add("-",0x55BC50,bit_cast<uintptr_t>(&CStats::UpdateRespectStat),FUpdateRespect);
    add("-",0x588BB0,bit_cast<uintptr_t>(&CHud::SetZoneName),FSetZoneName);
    add("-",0x50BF00,bit_cast<uintptr_t>(&CCamera::SetFadeColour),FSetFadeColour);
    add("-",0x50AC20,bit_cast<uintptr_t>(&CCamera::Fade),FFade);
    add("-",0x50B5D0,bit_cast<uintptr_t>(&CCamera::ProcessFade),FProcessFade);
    add("-",0x40E3A0,bit_cast<uintptr_t>(&CStreaming::LoadRequestedModels),FLoadRequested);
    add("-",0x40EA10,bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),FLoadAllRequested);
    add("-",0x730740,bit_cast<uintptr_t>(&D3DResourceSystem::TidyUpD3DIndexBuffers),FTidyIB);
    add("-",0x7305E0,bit_cast<uintptr_t>(&D3DResourceSystem::TidyUpD3DTextures),FTidyTex);
    add("-",0x538860,bit_cast<uintptr_t>(&CFileMgr::SetDirMyDocuments),FSetDirMyDocuments);
    add("-",0x5387D0,bit_cast<uintptr_t>(&CFileMgr::SetDir),FSetDir);
    add("-",0x538900,bit_cast<uintptr_t>(&CFileMgr::OpenFile),FOpenFile);
    add("-",0x5389D0,bit_cast<uintptr_t>(&CFileMgr::CloseFile),FCloseFile);
    add("-",0x5389E0,bit_cast<uintptr_t>(&CFileMgr::GetTotalSize),FGetTotalSize);
    add("-",0x538950,bit_cast<uintptr_t>(&CFileMgr::Read),FRead);
    add("-",0x5D1380,bit_cast<uintptr_t>(&CGenericGameStorage::CheckSlotDataValid),FCheckSlot);
    add("-",0x619060,bit_cast<uintptr_t>(&C_PcSave::SaveSlot),FSaveSlot);
    add("-",0x6190D0,bit_cast<uintptr_t>(&C_PcSave::DeleteSlot),FDeleteSlot);
    add("-",0x619140,bit_cast<uintptr_t>(&C_PcSave::PopulateSlotInfo),FPopulate);
    add("-",0x746870,bit_cast<uintptr_t>(&MessageLoop),FMessageLoop);
    add("-",0x541DD0,bit_cast<uintptr_t>(&CPad::UpdatePads),FUpdatePads);
    add("-",0x53D690,bit_cast<uintptr_t>(&DoRWStuffStartOfFrame),FStartFrame);
    add("-",0x53D840,bit_cast<uintptr_t>(&DoRWStuffEndOfFrame),FEndFrame);
    add("-",0x49EA50,bit_cast<uintptr_t>(&Fx_c::GetFxQuality),FGetFxQuality);
    add("-",0x49EA40,bit_cast<uintptr_t>(&Fx_c::SetFxQuality),FSetFxQuality);
    add("-",0x745AF0,bit_cast<uintptr_t>(&GetVideoModeList),FGetVideoModeList);
    add("-",0x745C70,bit_cast<uintptr_t>(&SetVideoMode),FSetVideoMode);
    add("-",0x745A70,bit_cast<uintptr_t>(&RwEngineGetNumVideoModes),FNumVideoModes);
    add("-",0x7F84E0,bit_cast<uintptr_t>(&RwD3D9EngineGetMaxMultiSamplingLevels),FMaxMS);
    add("-",0x7F8A90,bit_cast<uintptr_t>(&RwD3D9ChangeMultiSamplingLevels),FChangeMS);
    add("-",0x7F3530,bit_cast<uintptr_t>(&RwTextureSetMipmapping),FSetMipmapping);
    add("-",0x7046D0,bit_cast<uintptr_t>(&CPostEffects::DoScreenModeDependentInitializations),FScreenModeInit);
    pt.Add(Body(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber)),FakeRandomNumber); // DLL
    pt.Add(0x821B1E,FakeRand);                                                        // EXE
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,14> fns{{
    {"CMenuManager","Process",0x57B440,0,-1,256},
    {"CMenuManager","ProcessMenuOptions",0x576FE0,0,-1,2048},
    {"CMenuManager","ProcessPCMenuOptions",0x57CD50,0,-1,4096},
    {"CMenuManager","ProcessMissionPackNewGame",0x57D520,0,-1,512},
    {"CMenuManager","CheckMissionPackValidMenu",0x57D720,0,-1,512},
    {"CMenuManager","SwitchToNewScreen",0x573680,0,-1,2048},
    {"CMenuManager","JumpToGenericMessageScreen",0x576AE0,0,-1,512},
    {"CMenuManager","SaveLoadFileError_SetUpErrorScreen",0x57C490,0,-1,128},
    {"CMenuManager","GetNumberOfMenuOptions",0x573E70,0,-1,1024},
    {"CMenuManager","ProcessFileActions",0x578D60,0,-1,1024},
    {"CMenuManager","DoSettingsBeforeStartingAGame",0x573330,0,-1,512},
    {"CMenuManager","ProcessStreaming",0x573CF0,0,-1,1024},
    {"CMenuManager","HasLanguageChanged",0x573CD0,0,-1,128},
    {"CMenuManager","InitialiseChangedLanguageSettings",0x573260,0,-1,512},
}};

// --- copia de regiones globales ---
struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }
inline void Put(const Saved& s){ std::memcpy(reinterpret_cast<void*>(s.at),s.bytes.data(),s.bytes.size()); }
constexpr uintptr_t SINGLETON=0xBA6748, ASCREENS=0x8CE008, PREVSCREEN=0x8CDFF0, GSTRING=0xB71670, MPACKGAME=0xB72910, INVERTLOOK=0xB73402,
                    INVERTMOUSEY=0xBA6745, MOUSE3RD=0xB6EC2E, MOUSESTEER=0xC1CC02, SLOTS=0xC16EBC, PCERROR=0xC17034, FAVRADIO=0xB78E58, CHEATED=0x96918C;
inline std::vector<Saved> Globals(){
    return { Save(ASCREENS,sizeof(aScreens)), Save(PREVSCREEN,4), Save(GSTRING,352), Save(MPACKGAME,1), Save(INVERTLOOK,1), Save(INVERTMOUSEY,1),
             Save(MOUSE3RD,1), Save(MOUSESTEER,2), Save(SLOTS,32), Save(PCERROR,4), Save(FAVRADIO,56), Save(CHEATED,1) }; }
inline uint64 GlobalsHash(){ uint64 h=1469598103934665603ull; for(auto& s:Globals())h=Fnv(s.bytes.data(),s.bytes.size(),h); return h; }

// --- comprobacion de la disposicion de campos con los desplazamientos que usa el EXE ---
inline void LayoutCheck(bool& passed){
    M* f=Fake(); json bad=json::array();
    auto chk=[&](const char* n,const void* p,int expected){ const int off=int(uintptr_t(p)-uintptr_t(f)); if(off!=expected)bad.push_back({{"field",n},{"offset",off},{"expected",expected}}); };
#define CK(field,off) chk(#field,&f->field,off)
    CK(m_nStatsScrollDirection,0x0); CK(m_fStatsScrollSpeed,0x4); CK(m_bHudOn,0x21); CK(m_nRadarMode,0x24); CK(m_nTargetBlipIndex,0x2C);
    CK(m_DisplayControllerOnFoot,0x31); CK(m_bDontDrawFrontEnd,0x32); CK(m_bActivateMenuNextFrame,0x33); CK(m_KeyPressedCode,0x38);
    CK(m_bShowSubtitles,0x44); CK(m_bMapLegend,0x4A); CK(m_bWidescreenOn,0x4B); CK(m_bPrefsFrameLimiter,0x4C); CK(m_bRadioAutoSelect,0x4D);
    CK(m_bRadioEq,0x51); CK(m_nRadioStation,0x52); CK(m_nCurrentScreenItem,0x54); CK(m_bStreamingDisabled,0x5A); CK(m_bAllStreamingStuffLoaded,0x5B);
    CK(m_bMenuActive,0x5C); CK(m_bStartGameLoading,0x5D); CK(m_bLoadingData,0x60); CK(m_fMapZoom,0x64); CK(m_vMapOrigin,0x68); CK(m_bMapLoaded,0x78);
    CK(m_nPrefsLanguage,0x84); CK(m_nPreviousLanguage,0x85); CK(m_SystemLanguage,0x88); CK(m_ListSelection,0x90); CK(m_RadioMode,0xB0);
    CK(m_bInvertPadX1,0xB1); CK(m_bInvertPadY1,0xB2); CK(m_bInvertPadX2,0xB3); CK(m_bInvertPadY2,0xB4); CK(m_bSwapPadAxis1,0xB5); CK(m_bSwapPadAxis2,0xB6);
    CK(m_RedefiningControls,0xB7); CK(m_DisplayTheMouse,0xB8); CK(m_bPrefsMipMapping,0xC4); CK(m_bTracksAutoScan,0xC5); CK(m_nPrefsAntialiasing,0xC8);
    CK(m_nDisplayAntialiasing,0xCC); CK(m_ControlMethod,0xD0); CK(m_nPrefsVideoMode,0xD4); CK(m_nDisplayVideoMode,0xD8); CK(m_bSavePhotos,0xE8);
    CK(m_bMainMenuSwitch,0xE9); CK(m_bLanguageChanged,0xEB); CK(m_pPressedKey,0xF0); CK(m_nCurrentScreen,0x15D); CK(m_nPrevScreen,0x15E);
    CK(m_SelectedSlot,0x15F); CK(m_nMissionPackGameId,0x160); CK(m_MissionPacks,0x161); CK(m_ControllerError,0x1AEA); CK(m_bScanningUserTracks,0x1AEB);
    CK(m_nHelperTextFadingAlpha,0x1AEC); CK(m_bJustOpenedControlRedefWindow,0x1B08); CK(m_EditingControlOptions,0x1B09); CK(m_OptionToChange,0x1B0C);
    CK(m_CanBeDefined,0x1B14); CK(m_HelperText,0x1B18); CK(m_nNumberOfMenuOptions,0x1B21); CK(m_CurrentlyLoading,0x1B3C); CK(m_CurrentlyDeleting,0x1B3D);
    CK(m_CurrentlySaving,0x1B3E);
#undef CK
    // el singleton real debe ocupar la misma direccion que usa el EXE
    if(uintptr_t(&FrontEndMenuManager)!=SINGLETON)bad.push_back({{"field","FrontEndMenuManager"},{"offset",int(uintptr_t(&FrontEndMenuManager))},{"expected",int(SINGLETON)}});
    if(!bad.empty()){ passed=false; Write({{"event","layout_mismatch"},{"fields",bad}}); }
    else Write({{"event","layout_ok"}});
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"menumgrc-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"menumgrc-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("menumgrc");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"menumgrc-only.txt"); if(f)std::getline(f,only); }

    std::vector<ReversibleHooks::ReversibleHook::Base*> hooks;
    for(size_t f=0;f<fns.size();++f){ auto* h=FindHook(fns[f].cls,fns[f].name); if(!h){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;} hooks.push_back(h); }
    std::vector<char> enabled(hooks.size()),locked(hooks.size());
    for(size_t k=0;k<hooks.size();++k){enabled[k]=hooks[k]->Hooked();locked[k]=hooks[k]->Locked();hooks[k]->LockState(false);}

    // estado global guardado
    const auto savedGlobals=Globals();
    const auto savedSingleton=Save(SINGLETON,SZ);
    auto* padBytes=reinterpret_cast<uint8*>(&CPad::Pads[0]); constexpr size_t NPAD=sizeof(CPad)*2; const std::vector<uint8> savedPads(padBytes,padBytes+NPAD);
    auto* newKeys=reinterpret_cast<uint8*>(&CPad::NewKeyState); auto* oldKeys=reinterpret_cast<uint8*>(&CPad::OldKeyState); constexpr size_t NKEY=sizeof(CKeyboardState);
    const std::vector<uint8> savedNewKeys(newKeys,newKeys+NKEY), savedOldKeys(oldKeys,oldKeys+NKEY);
    const uint64 singletonHash=Fnv(&FrontEndMenuManager,SZ);

    bool passed=true; int pair=0;
    LayoutCheck(passed);
    {
        // acciones "interesantes" para rellenar las pantallas
        static const char* const keys[]{"FEM_OK","FET_LG","FESZ_QR","FEA_MUS","FEN_NGS","","FES_SEL","FED_HUD","X"};
        static const char* const longKeys[]{"","FET_LG","FES_LCE","FEM_OK","FEN_NGS_LONG","ABCDEFGH","ABCDEFGHI","X","MP_1234567890","FES_DEL","FET_SG"};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto rnd=[&](uint32 n){ return n ? next()%n : 0u; };
                auto put=[&](char* dst,const char* s){ std::memcpy(dst,s,std::strlen(s)+1); };

                std::vector<uint8> rawO, rawC; std::vector<uint8> globalsO, globalsC;
                auto run=[&](bool candidate,std::vector<uint8>& rawOut)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x4D43u; if(!stt)stt=1; trace=json::array(); rngCalls=0; rngSeed=next(); mapHash=0; mapCount=0;
                    // memoria del falso (guardas incluidas) aleatoria
                    for(size_t k=0;k+4<=raw.size();k+=4){ const uint32 v=next(); std::memcpy(raw.data()+k,&v,4); }
                    M* m=Fake();
                    // los bool que se conmutan o prueban se dejan en 0/1 (el EXE los normaliza con sete)
                    for(bool* b:{&m->m_bHudOn,&m->m_DisplayControllerOnFoot,&m->m_bDontDrawFrontEnd,&m->m_bActivateMenuNextFrame,&m->m_bShowSubtitles,&m->m_bMapLegend,
                                 &m->m_bWidescreenOn,&m->m_bPrefsFrameLimiter,&m->m_bRadioAutoSelect,&m->m_bRadioEq,&m->m_bStreamingDisabled,&m->m_bAllStreamingStuffLoaded,
                                 &m->m_bMenuActive,&m->m_bStartGameLoading,&m->m_bLoadingData,&m->m_bMapLoaded,&m->m_bInvertPadX1,&m->m_bInvertPadY1,&m->m_bInvertPadX2,
                                 &m->m_bInvertPadY2,&m->m_bSwapPadAxis1,&m->m_bSwapPadAxis2,&m->m_DisplayTheMouse,&m->m_bPrefsMipMapping,&m->m_bTracksAutoScan,
                                 &m->m_bSavePhotos,&m->m_bMainMenuSwitch,&m->m_bLanguageChanged,&m->m_bScanningUserTracks,&m->m_bJustOpenedControlRedefWindow,
                                 &m->m_EditingControlOptions,&m->m_CanBeDefined,&m->m_CurrentlyLoading,&m->m_CurrentlyDeleting,&m->m_CurrentlySaving})
                        *reinterpret_cast<uint8*>(b)=uint8(next()&1);
                    // tabla de pantallas aleatoria pero coherente
                    static const int interesting[]{2,3,4,5,6,7,8,9,10,11,12,13,14,15,18,21,22,23,25,26,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,52,53,54,55,56,57,58,59,60,63,64,65};
                    auto pickAction=[&]()->int{ const uint32 r=next()%10; return r==0 ? 0 : r<8 ? interesting[next()%std::size(interesting)] : int(next()%71); };
                    for(int s=0;s<SCREEN_COUNT;++s){
                        auto& sc=aScreens[s];
                        std::memset(&sc,0,sizeof(sc)); put(sc.m_szTitleName,keys[next()%std::size(keys)]);
                        sc.m_nParentMenu=static_cast<eMenuScreen>(int(next()%46)-2); sc.m_nStartEntry=int8(next()%12);
                        for(auto& it:sc.m_aItems){
                            it.m_nActionType=static_cast<eMenuAction>(pickAction()); put(it.m_szName,keys[next()%std::size(keys)]);
                            it.m_nType=static_cast<eMenuEntryType>(next()%10); it.m_nTargetMenu=static_cast<eMenuScreen>(int(next()%46)-2);
                            it.m_X=uint16(next()%640); it.m_Y=uint16(next()%448); it.m_nAlign=static_cast<eMenuAlign>(next()%4);
                        }
                    }
                    m->m_nCurrentScreen=static_cast<eMenuScreen>(next()%SCREEN_COUNT);
                    m->m_nCurrentScreenItem=int32(next()%12);
                    std::fill(packExists.begin(),packExists.end(),false); for(auto& c:packContent)c.clear(); packPos.fill(0);
                    fxPcRet=false; fxCheckSlot=false; fxDeleteOk=false; fxScr=false; fxGxt=false; fxSaveRet=0; fxMaxMS=0; fxMission.clear();
                    fxFxQuality=0; fxNumModes=0; padIter=0; padExitAt=1; padKind=0;

                    // --- argumentos y fixtures por funcion (se eligen ANTES de conmutar el hook: mismo consumo de RNG) ---
                    int8 pressedLR=0; bool accept=false, cancel=false; bool streamAll=false, reinit=false;
                    eMenuScreen scrArg=SCREEN_NOP; const char* titleKey=""; const char* textKey=""; bool syncReal=false;
                    auto fillStatics=[&]{
                        G<uint8>(MPACKGAME)=uint8(rnd(4)==0 ? next() : rnd(3)); G<uint8>(INVERTLOOK)=uint8(next()&1); G<uint8>(INVERTMOUSEY)=uint8(next()&1);
                        G<uint8>(MOUSE3RD)=uint8(next()&1); G<uint8>(MOUSESTEER)=uint8(next()&1); G<uint8>(MOUSESTEER+1)=uint8(next()&1);
                        for(int k=0;k<8;++k)G<uint32>(SLOTS+4*k)=next()%4; G<uint32>(PCERROR)=rnd(3)==0 ? next() : rnd(14);
                        for(int k=0;k<14;++k)G<uint32>(FAVRADIO+4*k)=next(); G<uint8>(CHEATED)=uint8(next()&1);
                        G<int32>(PREVSCREEN)=rnd(2) ? int32(m->m_nCurrentScreen) : int32(next());
                    };
                    fillStatics();

                    if(nm=="HasLanguageChanged"){
                        m->m_nPrefsLanguage=static_cast<eLanguage>(next()%8); m->m_nPreviousLanguage=rnd(2) ? m->m_nPrefsLanguage : static_cast<eLanguage>(next());
                    }
                    else if(nm=="GetNumberOfMenuOptions"){
                        syncReal=true; const uint32 r=rnd(10); m->m_nCurrentScreen=static_cast<eMenuScreen>(r==0 ? SCREEN_MAP : r==1 ? SCREEN_BRIEF : int(next()%SCREEN_COUNT));
                        G<int32>(PREVSCREEN)=rnd(2) ? int32(m->m_nCurrentScreen) : int32(int(next()%SCREEN_COUNT));
                    }
                    else if(nm=="SaveLoadFileError_SetUpErrorScreen"){ G<uint32>(PCERROR)=rnd(4)==0 ? next() : rnd(14); }
                    else if(nm=="HasLanguageChanged"){}
                    else if(nm=="DoSettingsBeforeStartingAGame"){
                        m->m_nTargetBlipIndex=rnd(2) ? 0 : int32(next());
                    }
                    else if(nm=="InitialiseChangedLanguageSettings"){
                        reinit=next()&1; m->m_bLanguageChanged=rnd(4)!=0;
                        m->m_nPrefsLanguage=static_cast<eLanguage>(rnd(8)==0 ? next() : next()%7);
                        static const int sysLangs[]{0,1,2,3,4,0x101,0x102,0x1000001};
                        m->m_SystemLanguage=rnd(3)==0 ? int32(next()) : sysLangs[next()%std::size(sysLangs)];
                        static const char* const missions[]{"","","M1","MISSION","ABCDEFG"}; fxMission=missions[next()%std::size(missions)];
                    }
                    else if(nm=="ProcessStreaming"){
                        streamAll=next()&1; m->m_bMenuActive=rnd(5)!=0; m->m_nCurrentScreen=rnd(5)==0 ? static_cast<eMenuScreen>(next()%SCREEN_COUNT) : SCREEN_MAP;
                        m->m_bMapLoaded=rnd(6)!=0; m->m_bStreamingDisabled=rnd(6)==0;
                        const uint32 r=rnd(50); m->m_fMapZoom= r==0 ? 0.f : r==1 ? std::bit_cast<float>(0x7FC00000u) : r<4 ? -uni(10.f,500.f) : uni(30.f,1500.f);
                        m->m_vMapOrigin.x=uni(-300.f,800.f); m->m_vMapOrigin.y=uni(-300.f,600.f);
                    }
                    else if(nm=="Process"){
                        m->m_bMenuActive=rnd(4)!=0; m->m_bAllStreamingStuffLoaded=next()&1;
                    }
                    else if(nm=="SwitchToNewScreen"){
                        static const int special[]{0,0,-2,-2,1,3,4,6,8,11,13,16,5,-1,2};
                        const uint32 r=rnd(10);
                        scrArg=static_cast<eMenuScreen>(r<6 ? special[next()%std::size(special)] : r<8 ? int(next()%SCREEN_COUNT) : int(int8(next())));
                        static const int curs[]{4,1,33,35,3,16,0,6,8,11,13};
                        if(rnd(3)==0)m->m_nCurrentScreen=static_cast<eMenuScreen>(curs[next()%std::size(curs)]);
                        m->m_nPrevScreen=static_cast<eMenuScreen>(next()); m->m_nRadioStation=static_cast<eRadioID>(rnd(4)==0 ? next() : next()%14);
                        m->m_nMissionPackGameId=uint8(next());
                    }
                    else if(nm=="JumpToGenericMessageScreen"){
                        const uint32 r=rnd(10); scrArg=static_cast<eMenuScreen>(r<3 ? SCREEN_GAME_SAVED : r<6 ? SCREEN_GAME_LOADED : int(1+next()%43));
                        titleKey=longKeys[next()%std::size(longKeys)]; textKey=longKeys[next()%std::size(longKeys)];
                    }
                    else if(nm=="ProcessFileActions"){
                        syncReal=true; static const int scr[]{SCREEN_LOAD_FIRST_SAVE,SCREEN_DELETE_FINISHED,SCREEN_SAVE_DONE_1,SCREEN_SAVE_DONE_1,SCREEN_LOAD_FIRST_SAVE,SCREEN_DELETE_FINISHED};
                        m->m_nCurrentScreen=static_cast<eMenuScreen>(rnd(8)==0 ? int(next()%SCREEN_COUNT) : scr[next()%std::size(scr)]);
                        m->m_SelectedSlot=uint8(rnd(3)==0 ? next() : next()%8);
                        fxCheckSlot=next()&1; fxDeleteOk=next()&1; fxSaveRet=rnd(3)==0 ? (next()&~0xFFu) : rnd(2); fxScr=rnd(4)!=0;
                        G<uint8>(MPACKGAME)=uint8(rnd(2) ? 0 : 1+rnd(3));
                    }
                    else if(nm=="CheckMissionPackValidMenu"){
                        G<uint8>(MPACKGAME)=uint8(rnd(5)==0 ? next() : 1+rnd(5)); fxScr=rnd(2); fxGxt=rnd(2); padKind=int(rnd(3)); padExitAt=1+int(rnd(3));
                        m->m_bActivateMenuNextFrame=next()&1;
                        // estado inicial del pad/teclado: aleatorio
                        auto& n=CPad::NewKeyState; auto& o=CPad::OldKeyState; n.enter=int16(next()&1); o.enter=int16(next()&1); n.extenter=int16(next()&1); o.extenter=int16(next()&1);
                        CPad::Pads[0].NewState.ButtonCross=uint16(next()&1?255:0); CPad::Pads[0].OldState.ButtonCross=uint16(next()&1?255:0);
                    }
                    else if(nm=="ProcessMissionPackNewGame"){
                        G<uint8>(MPACKGAME)=uint8(rnd(2) ? 0 : 1+rnd(3));
                        static const char* const names[]{"Design Your Own Mission","Pack B","X","Mission Pack Numero Tres","Zz","A#B"};
                        const uint32 nFiles=rnd(4)==0 ? 0 : 1+rnd(8); uint32 totalPacks=0;
                        for(uint32 k=0;k<nFiles;++k){
                            const int idx=int(rnd(25)); if(packExists[idx])continue;
                            std::string c; const uint32 entries=rnd(4);
                            for(uint32 e=0;e<entries;++e){
                                c+=std::to_string(next()%400); c+= rnd(10)==0 ? "##" : "#";
                                std::string nm2=names[next()%std::size(names)]; for(auto& ch:nm2)if(ch=='#')ch='_'; if(rnd(8)==0)nm2.clear();
                                c+=nm2; c+="#"; static const char* const seps[]{"","\r\n","\n","\r"," "," \n"}; c+=seps[next()%std::size(seps)];
                            }
                            if(rnd(6)==0)c+="\n\r";
                            // simula la maquina de estados para no pasar de 25 packs en total (el EXE desborda m_MissionPacks)
                            uint32 packs=0; bool readingId=true; size_t len=0;
                            for(char ch:c){ if(ch=='#'){ if(len>0){ if(!readingId)++packs; readingId=!readingId; len=0; } } else if(ch!='\n'&&ch!='\r')++len; }
                            if(c.size()>240||totalPacks+packs>24)continue;
                            totalPacks+=packs; packExists[idx]=true; packContent[idx]=c;
                        }
                    }
                    else if(nm=="ProcessMenuOptions"||nm=="ProcessPCMenuOptions"){
                        const bool pc=nm=="ProcessPCMenuOptions";
                        int act; { const uint32 r=rnd(20); act= r==0 ? 0 : r==1 ? 1 : r==2 ? int(67+next()%40) : int(2+next()%65); }
                        auto& it=aScreens[m->m_nCurrentScreen].m_aItems[m->m_nCurrentScreenItem]; it.m_nActionType=static_cast<eMenuAction>(act);
                        static const int8 lrs[]{-1,0,1,1,-1,2,-3,127,-128};
                        pressedLR=lrs[next()%std::size(lrs)]; accept=next()&1; cancel=next()&1; fxPcRet=rnd(5)==0;
                        for(int k=0;k<8;++k)G<uint32>(SLOTS+4*k)=next()%4;
                        if(pc){
                            // fuera de las rutas FIX_BUGS/NOTSA de la DLL: RADAR_MODE solo con 0/1, FX_QUALITY sin flecha izquierda, PLAY_MODE con +-1
                            if(act==MENU_ACTION_RADAR_MODE){ pressedLR=int8(next()&1); m->m_nRadarMode=static_cast<eRadarMode>(int(next()%8)-1); }
                            if(act==MENU_ACTION_FX_QUALITY){ pressedLR=int8(next()&1); fxFxQuality=rnd(6)==0 ? int(next()%9)-2 : int(next()%4); }
                            if(act==MENU_ACTION_USER_TRACKS_PLAY_MODE){ pressedLR=next()&1 ? 1 : -1; m->m_RadioMode=static_cast<eRadioMode>(rnd(5)==0 ? next() : next()%3); }
                            m->m_ControlMethod=static_cast<eController>(rnd(8)==0 ? next() : next()&1);
                            m->m_nPrefsLanguage=static_cast<eLanguage>(next()%7);
                            m->m_nPrefsAntialiasing=int32(next()%7)-1; m->m_nDisplayAntialiasing=rnd(3)==0 ? m->m_nPrefsAntialiasing : int32(next()%8)-1; fxMaxMS=rnd(8)==0 ? next()%20 : 1+next()%8;
                            m->m_nPrefsVideoMode=int32(next()%6); fxNumModes=3+int(rnd(14));
                            for(size_t k=0;k<modesBuf.size();++k)modesBuf[k]=reinterpret_cast<char*>(uintptr_t(0x1000+k*16));
                            for(int k=2;k<2+fxNumModes;++k)if(rnd(3)==0)modesBuf[k]=nullptr;
                            { bool any=false; for(int k=2;k<2+fxNumModes;++k)any|=modesBuf[k]!=nullptr; if(!any)modesBuf[2+rnd(fxNumModes)]=reinterpret_cast<char*>(0x4000); }
                            m->m_nDisplayVideoMode=rnd(3)==0 ? m->m_nPrefsVideoMode : int32(rnd(fxNumModes));
                            auto& it2=aScreens[m->m_nCurrentScreen].m_aItems[2]; const uint32 q=rnd(4); it2.m_nTargetMenu= q==0 ? SCREEN_DISPLAY_ADVANCED : q==1 ? SCREEN_CONTROLLER_SETUP : it2.m_nTargetMenu;
                        } else {
                            if(act==MENU_ACTION_SAVE_SLOT||act==MENU_ACTION_SAVE_GAME){
                                m->m_nCurrentScreenItem=1+int32(rnd(8)); aScreens[m->m_nCurrentScreen].m_aItems[m->m_nCurrentScreenItem].m_nActionType=static_cast<eMenuAction>(act);
                                aScreens[m->m_nCurrentScreen].m_aItems[m->m_nCurrentScreenItem].m_nType=static_cast<eMenuEntryType>(rnd(6)==0 ? int8(next()) : 1+rnd(8));
                                static const int scrs[]{SCREEN_LOAD_GAME,SCREEN_DELETE_GAME,SCREEN_LOAD_GAME,SCREEN_DELETE_GAME};
                                if(rnd(5)!=0)m->m_nCurrentScreen=static_cast<eMenuScreen>(scrs[next()%4]);
                            }
                            if(act==MENU_ACTION_STAT){
                                static const uint32 sp[]{0u,0x43160000u,0x41A00000u,0x80000000u,0x7FC00000u,0x3F800000u,0xC2C80000u};
                                m->m_fStatsScrollSpeed=std::bit_cast<float>(sp[next()%std::size(sp)]); m->m_nStatsScrollDirection=int8(rnd(3)==0 ? next() : next()&1);
                            }
                        }
                        // la accion elegida puede haber sido sobrescrita al mover el elemento actual
                        aScreens[m->m_nCurrentScreen].m_aItems[m->m_nCurrentScreenItem].m_nActionType=static_cast<eMenuAction>(act);
                    }

                    // --- conmutar hook, parches y llamada ---
                    hooks[f]->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    Patches pt; InstallStubs(pt,nm);
                    const uint64 realBefore=Fnv(&FrontEndMenuManager,SZ);
                    const uintptr_t a=fn.address;
                    json ret=nullptr;
                    uint8 realSaved[4]{G<uint8>(0xBA8284),G<uint8>(0xBA8285),G<uint8>(0xBA8286),G<uint8>(0xBA8269)};
                    if(!candidate&&syncReal){ G<uint8>(0xBA8284)=m->m_CurrentlyLoading; G<uint8>(0xBA8285)=m->m_CurrentlyDeleting; G<uint8>(0xBA8286)=m->m_CurrentlySaving; G<uint8>(0xBA8269)=m->m_nNumberOfMenuOptions; }
                    if(nm=="Process") CallThis<void>(a,m);
                    else if(nm=="HasLanguageChanged") ret=int(CallThis<bool>(a,m));
                    else if(nm=="GetNumberOfMenuOptions") ret=int(uint8(CallThis<uint32>(a,m))); // el EXE devuelve solo AL
                    else if(nm=="SaveLoadFileError_SetUpErrorScreen") CallThis<void>(a,m);
                    else if(nm=="DoSettingsBeforeStartingAGame") CallThis<void>(a,m);
                    else if(nm=="InitialiseChangedLanguageSettings") CallThis<void>(a,m,reinit);
                    else if(nm=="ProcessStreaming") CallThis<void>(a,m,streamAll);
                    else if(nm=="SwitchToNewScreen") CallThis<void>(a,m,scrArg);
                    else if(nm=="JumpToGenericMessageScreen") CallThis<void>(a,m,scrArg,titleKey,textKey);
                    else if(nm=="ProcessFileActions") CallThis<void>(a,m);
                    else if(nm=="CheckMissionPackValidMenu") ret=int(CallThis<bool>(a,m));
                    else if(nm=="ProcessMissionPackNewGame") CallThis<void>(a,m);
                    else if(nm=="ProcessMenuOptions"){ CallThis<void>(a,m,pressedLR,&cancel,accept); ret=int(cancel); }
                    else if(nm=="ProcessPCMenuOptions") ret=int(CallThis<bool>(a,m,pressedLR,accept));

                    if(!candidate&&syncReal){ m->m_CurrentlyLoading=G<uint8>(0xBA8284); m->m_CurrentlyDeleting=G<uint8>(0xBA8285); m->m_CurrentlySaving=G<uint8>(0xBA8286); m->m_nNumberOfMenuOptions=G<uint8>(0xBA8269);
                        G<uint8>(0xBA8284)=realSaved[0]; G<uint8>(0xBA8285)=realSaved[1]; G<uint8>(0xBA8286)=realSaved[2]; G<uint8>(0xBA8269)=realSaved[3]; }
                    const bool realIntact=Fnv(&FrontEndMenuManager,SZ)==realBefore;
                    if(nm=="Process"||nm=="ProcessStreaming")Tr("map",mapCount,mapHash);
                    // ProcessFileActions: la DLL hace CFileMgr::SetDir("") (FIX_BUGS) que el EXE no hace: no se compara
                    if(nm=="ProcessFileActions"){ json t2=json::array(); for(auto& e:trace)if(e[0]!="CFileMgr::SetDir")t2.push_back(e); trace=t2; }

                    rawOut.assign(raw.begin(),raw.end());
                    json out={{"switches",switches},{"ret",ret},{"trace",trace},{"memory",Fnv(raw.data(),raw.size())},{"globals",GlobalsHash()},
                              {"real_intact",realIntact},{"rng_calls",rngCalls},{"fx_quality",fxFxQuality},
                              {"pad",Fnv(newKeys,NKEY,Fnv(oldKeys,NKEY,Fnv(padBytes,NPAD)))}};
                    return out;
                };
                // el pad se restaura entre las dos ejecuciones para que ambas partan del mismo estado
                const auto savedPadsRun=ReversingProbe::Bytes(padBytes,NPAD), savedNewRun=ReversingProbe::Bytes(newKeys,NKEY), savedOldRun=ReversingProbe::Bytes(oldKeys,NKEY);
                auto restoreGlobals=[&]{ for(auto& s:savedGlobals)Put(s); std::memcpy(padBytes,savedPadsRun.data(),NPAD); std::memcpy(newKeys,savedNewRun.data(),NKEY); std::memcpy(oldKeys,savedOldRun.data(),NKEY); };
                const auto original=run(false,rawO); restoreGlobals();
                const auto candidate=run(true,rawC); restoreGlobals();
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["real_intact"]==true&&candidate["real_intact"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    json bytes=json::array(); // [desplazamiento respecto al falso (negativo = guarda previa), original, candidato]
                    for(size_t k=0;k<rawO.size()&&bytes.size()<64;++k)if(rawO[k]!=rawC[k])bytes.push_back({int(k)-int(GUARD),rawO[k],rawC[k]});
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"byte_diffs",bytes},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t k=0;k<hooks.size();++k){hooks[k]->State(enabled[k]);hooks[k]->LockState(locked[k]);}
    for(auto& s:savedGlobals)Put(s); Put(savedSingleton);
    std::memcpy(padBytes,savedPads.data(),NPAD); std::memcpy(newKeys,savedNewKeys.data(),NKEY); std::memcpy(oldKeys,savedOldKeys.data(),NKEY);
    bool restored=std::memcmp(padBytes,savedPads.data(),NPAD)==0&&std::memcmp(newKeys,savedNewKeys.data(),NKEY)==0&&std::memcmp(oldKeys,savedOldKeys.data(),NKEY)==0
        &&Fnv(&FrontEndMenuManager,SZ)==singletonHash;
    for(auto& s:savedGlobals)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
