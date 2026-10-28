#pragma once
// Banco dentro del motor: audio, interfaz y render (lista de 70), 24 funciones de 12 clases: CAEStreamingDecoder,
// CAEStreamTransformer, CColourSet, CCrime, CCurrentVehicle, CFallingGlassPane, CGamma, COnscreenCounterEntry,
// CPlaceName, CRegisteredMotionBlurStreak, CUserDisplay y Securom. Original (hooks del grupo desactivados) frente a
// candidato con fixtures deterministas por caso. Se simulan y trazan: rand, CText::Get, CFont (setters y
// PrintString), CHud::SetVehicleName/SetZoneName, zonas (FindSmallestZoneForPosition), entradas/salidas
// (GetPositionRelativeToOutsideWorld), COnscreenTimer::Init/Process, CAEDataStream::Initialise, operator delete,
// el dispositivo Direct3D (caps y rampas de gamma falsas: la gamma real no se toca nunca), las capacidades de RW,
// Im3D (direcciones del EXE), LittleTest, CGlass (alfa por normal y vaciado de lotes), audio de cristales,
// Fx_c::AddGlass, FindPlayerPed y el sistema de búsqueda (CWanted, CPlayerPed, IsPedPointerValid, CPedType,
// GermanGame, mensajes grandes). El estado global tocado se guarda y se restaura en cada ejecución.
#include <fstream>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEStreamingDecoder.h"
#include "AEStreamTransformer.h"
#include "AEDataStream.h"
#include "ColourSet.h"
#include "Crime.h"
#include "CurrentVehicle.h"
#include "FallingGlassPane.h"
#include "Gamma.h"
#include "OnscreenCounterEntry.h"
#include "OnscreenTimer.h"
#include "PlaceName.h"
#include "RegisteredMotionBlurStreak.h"
#include "UserDisplay.h"
#include "Securom.h"
#include "Glass.h"
#include "Hud.h"
#include "TheZones.h"
#include "EntryExitManager.h"
#include "Wanted.h"
#include "PedType.h"
#include "Localisation.h"
#include "Messages.h"
#include "Font.h"
#include "ModelInfo.h"
#include "TheScripts.h"
#include "Fx/Fx.h"
#include "../ProbeRuntime.hpp"

namespace Misc70Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- etiquetas de punteros (fixtures del caso) ---
inline std::vector<std::pair<const void*,std::string>> labels;
inline json Label(const void* p){ if(!p)return nullptr; for(auto& [q,n]:labels)if(q==p)return n; return "unk"; }

// --- rand ---
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- textos ---
alignas(4) inline std::array<std::array<GxtChar,16>,64> texts{};
inline json TextLabel(const void* p){ if(!p)return nullptr; const auto d=intptr_t(p)-intptr_t(texts.data()); if(d>=0&&d<intptr_t(sizeof(texts)))return "T"+std::to_string(d/16)+"+"+std::to_string(d%16); return Label(p); }
inline const GxtChar* __fastcall FakeTextGet(void*,void*,const char* key){ trace.push_back({"text_get",std::string(key)}); return texts[Fnv(key,strlen(key))%64].data(); }

// --- fuentes ---
inline void __cdecl FakeSetScale(float w,float h){ trace.push_back({"font_scale",F(w),F(h)}); }
inline void __cdecl FakeSetColor(uint32 c){ trace.push_back({"font_color",c}); }
inline void __cdecl FakeSetFontStyle(uint32 s){ trace.push_back({"font_style",s&0xFF}); }
inline void __cdecl FakeSetWrapx(float v){ trace.push_back({"font_wrapx",F(v)}); }
inline void __cdecl FakeSetRightJustifyWrap(float v){ trace.push_back({"font_right_wrap",F(v)}); }
inline void __cdecl FakeSetDropColor(uint32 c){ trace.push_back({"font_drop_color",c}); }
inline void __cdecl FakeSetEdge(uint32 v){ trace.push_back({"font_edge",v&0xFF}); }
inline void __cdecl FakeSetProportional(uint32 v){ trace.push_back({"font_proportional",v&0xFF}); }
inline void __cdecl FakeSetOrientation(uint32 v){ trace.push_back({"font_orientation",v&0xFF}); }
inline void __cdecl FakePrintString(float x,float y,const GxtChar* t){ trace.push_back({"print_string",F(x),F(y),std::string(reinterpret_cast<const char*>(t))}); }

// --- HUD, zonas, entradas/salidas, temporizador ---
inline void __cdecl FakeSetVehicleName(const GxtChar* n){ trace.push_back({"set_vehicle_name",TextLabel(n)}); }
// sin zona el EXE pasa un valor de pila sin inicializar (0x572016) y la DLL nullptr: ambos se registran como nulo
inline void __cdecl FakeSetZoneName(const GxtChar* n,bool imm){ json l=TextLabel(n); if(l=="unk")l=nullptr; trace.push_back({"set_zone_name",l,imm}); }
inline void* zoneResult{};
inline CZone* __cdecl FakeFindZone(const CVector& p,bool type0){ trace.push_back({"find_smallest_zone",F(p.x),F(p.y),F(p.z),type0}); return static_cast<CZone*>(zoneResult); }
inline bool relShift{};
inline void __cdecl FakeRelOutside(CVector& p){ trace.push_back({"relative_to_outside",F(p.x),F(p.y),F(p.z)}); if(relShift){p.x+=1000.f;p.y-=500.f;p.z+=20.f;} }
inline void __fastcall FakeTimerInit(void* self,void*){ trace.push_back({"onscreen_timer_init",intptr_t(self)-0xBA1788}); }
inline void __fastcall FakeTimerProcess(void* self,void*){ trace.push_back({"onscreen_timer_process",intptr_t(self)-0xBA1788}); }

// --- CAEDataStream y operator delete ---
inline bool initResult{};
inline bool __fastcall FakeStreamInit(void* self,void*){ trace.push_back({"data_stream_initialise",Label(self)}); return initResult; }
// sin reservas: mientras operator delete esta parcheado, el json de la traza no puede crecer (liberaria con el stub)
inline std::array<void*,16> deleted{}; inline size_t ndeleted{};
inline void __cdecl FakeDelete(void* p){ if(ndeleted<deleted.size())deleted[ndeleted++]=p; }

// --- Direct3D y RW (falsos) ---
inline D3DCAPS9 fakeCaps{}; inline uint32 devCaps2{}; inline bool deviceNull{};
inline std::array<void*,128> devVt{}; struct FakeDevice { void** vt; }; inline FakeDevice fakeDevice{};
inline std::array<uint16,768> rampPattern{};
inline HRESULT __stdcall FakeGetDeviceCaps(void* self,D3DCAPS9* c){ trace.push_back({"get_device_caps",self==&fakeDevice}); std::memset(c,0,sizeof(*c)); c->Caps2=devCaps2; c->DeclTypes=0xFFFFFFFF; return 0; }
inline void __stdcall FakeSetGammaRamp(void* self,UINT sc,DWORD flags,const D3DGAMMARAMP* r){ trace.push_back({"set_gamma_ramp",self==&fakeDevice,sc,flags,intptr_t(r)-0xC8D4C8,Fnv(r,sizeof(*r))}); }
inline void __stdcall FakeGetGammaRamp(void* self,UINT sc,D3DGAMMARAMP* r){ trace.push_back({"get_gamma_ramp",self==&fakeDevice,sc,intptr_t(r)-0xC8D4C8}); std::memcpy(r,rampPattern.data(),sizeof(*r)); }
inline void __stdcall FakeDevTrap(void*){ Write({{"event","error"},{"reason","unexpected device call"}}); std::abort(); }
inline void* __cdecl FakeCurrentDevice(){ trace.push_back({"current_d3d_device"}); return deviceNull ? nullptr : &fakeDevice; }
inline const void* __cdecl FakeGetCaps(){ trace.push_back({"rw_d3d9_caps"}); return &fakeCaps; }

// --- Im3D y LittleTest ---
inline bool transformOk{};
inline json Vtx3(const RxObjSpace3DVertex* v,int n){ json r=json::array(); for(int k=0;k<n;++k){ const auto* w=reinterpret_cast<const uint32*>(&v[k]); json e=json::array(); for(int j=0;j<9;++j)e.push_back(w[j]); r.push_back(e);} return r; }
inline void* __cdecl FakeIm3DTransform(void* v,RwUInt32 n,RwMatrix* m,RwUInt32 f){ trace.push_back({"im3d_transform",Vtx3(static_cast<RxObjSpace3DVertex*>(v),int(n)),uintptr_t(m),f,intptr_t(v)}); return transformOk ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(RwPrimitiveType t,RwImVertexIndex* idx,RwInt32 n){ json is=json::array(); for(int k=0;k<n;++k)is.push_back(idx[k]); trace.push_back({"im3d_render_indexed",int(t),is,intptr_t(idx)}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd(){ trace.push_back({"im3d_end"}); return TRUE; }
inline void __cdecl FakeLittleTest(){ trace.push_back({"little_test"}); }

// --- cristales ---
inline uint8 alphaResult{};
inline uint8 __cdecl FakeCalcAlpha(const CVector& n){ trace.push_back({"calc_alpha_with_normal",F(n.x),F(n.y),F(n.z)}); return alphaResult; }
inline void __cdecl FakeRenderHiLight(){ trace.push_back({"render_hilight_polys",CGlass::H1iLightPolyVerticesIdx,CGlass::HiLightPolyIndicesIdx}); CGlass::H1iLightPolyVerticesIdx=0; CGlass::HiLightPolyIndicesIdx=0; }
inline void __cdecl FakeRenderShattered(){ trace.push_back({"render_shattered_polys",CGlass::ShatteredVerticesBaseIdx,CGlass::ShatteredIndicesBaseIdx}); CGlass::ShatteredVerticesBaseIdx=1024; CGlass::ShatteredIndicesBaseIdx=2048; }
inline void __fastcall FakeGlassAudio(void* self,void*,int32 ev,const CVector& p){ trace.push_back({"glass_collision_audio",self==&AudioEngine,ev,F(p.x),F(p.y),F(p.z)}); }
inline void __fastcall FakeAddGlass(void* self,void*,const CVector& p,const RwRGBA& c,float s,int32 n){ trace.push_back({"add_glass",self==&g_fx,F(p.x),F(p.y),F(p.z),c.red,c.green,c.blue,c.alpha,F(s),n}); }

// --- búsqueda policial ---
inline void* playerResult{};
inline uint32 boolBits{}, boolCalls{}; inline bool NextBool(){ return (boolBits>>(boolCalls++%32))&1; }
inline uint32 wantedResult{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); return static_cast<CPlayerPed*>(playerResult); }
inline bool __cdecl FakePedValid(CPed* p){ const bool r=NextBool(); trace.push_back({"is_ped_pointer_valid",Label(p),r}); return r; }
inline bool __cdecl FakePoliceDontCare(uint32 t){ const bool r=NextBool(); trace.push_back({"police_dont_care",t,r}); return r; }
inline uint32 __fastcall FakeGetWantedLevel(void* self,void*){ trace.push_back({"get_wanted_level",Label(self)}); return wantedResult; }
inline void __fastcall FakePedSetWantedNoDrop(void* self,void*,uint32 l){ trace.push_back({"ped_set_wanted_level_no_drop",Label(self),l}); }
inline void __fastcall FakeWantedSetNoDrop(void* self,void*,uint32 l){ trace.push_back({"wanted_set_wanted_level_no_drop",Label(self),l}); }
inline void __fastcall FakeRegisterCrime(void* self,void*,int32 c,const CVector& p,uint32 id,uint32 dc){ trace.push_back({"register_crime",Label(self),c,F(p.x),F(p.y),F(p.z),Label(reinterpret_cast<void*>(id)),dc&0xFF}); }
inline void __fastcall FakeRegisterCrimeNow(void* self,void*,int32 c,const CVector& p,uint32 id,uint32 dc){ trace.push_back({"register_crime_immediately",Label(self),c,F(p.x),F(p.y),F(p.z),Label(reinterpret_cast<void*>(id)),dc&0xFF}); }
inline int32 __cdecl FakePolicePresence(CVector c,float r){ const bool v=NextBool(); trace.push_back({"work_out_police_presence",F(c.x),F(c.y),F(c.z),F(r),v}); return v ? int32(1+boolCalls%3) : 0; }
inline bool __cdecl FakeGermanGame(){ const bool r=NextBool(); trace.push_back({"german_game",r}); return r; }
inline void __cdecl FakeBigMessage(const GxtChar* t,uint32 time,uint32 style){ trace.push_back({"add_big_message",TextLabel(t),time,style&0xFFFF}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,24> fns{{
    {"CAEStreamingDecoder","Constructor",0x4F2810,0,-1,256},{"CAEStreamingDecoder","Destructor",0x4F2830,0,-1,256},
    {"CAEStreamTransformer","Initialise",0x4F1750,0,-1,64},{"CAEStreamTransformer","TransformBuffer",0x4F17D0,0,-1,1024},
    {"CColourSet","Constructor",0x55F4B0,0,-1,512},{"CColourSet","Interpolate",0x55F870,0,-1,1024},
    {"CCrime","FindImmediateDetectionRange",0x531FC0,0,-1,256},{"CCrime","ReportCrime",0x532010,0,-1,1024},
    {"CCurrentVehicle","Display",0x571EA0,0,-1,256},{"CCurrentVehicle","Process",0x572040,0,-1,256},
    {"CFallingGlassPane","Render",0x71B100,0,-1,1024},{"CFallingGlassPane","Update",0x71AA10,0,-1,1024},
    {"CGamma","Init",0x747180,0,-1,128},{"CGamma","SetGamma",0x747200,0,-1,512},
    {"COnscreenCounterEntry","ProcessForDisplayCounter",0x44CA90,0,-1,1024},{"COnscreenCounterEntry","SetColourID",0x44CB00,0,-1,256},
    {"CPlaceName","GetForMap",0x571D90,0,-1,512},{"CPlaceName","Process",0x571F20,0,-1,1024},
    {"CRegisteredMotionBlurStreak","Render",0x721F70,0,-1,512},{"CRegisteredMotionBlurStreak","Update",0x721EE0,0,-1,512},
    {"CUserDisplay","Init",0x571EE0,0,-1,128},{"CUserDisplay","Process",0x5720A0,0,-1,512},
    {"Securom","SecuromDisplayInit",0x744A70,0,-1,64},{"Securom","SecuromStateDisplay",0x744AE0,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"misc70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"misc70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("misc70");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"misc70-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,24> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uint32 varSpace=CTheScripts::GetSizeOfVariableSpace();
    if(varSpace<8+300){Write({{"event","error"},{"reason","script variable space too small"},{"size",varSpace}});return;}
    std::array<bool,24> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    struct Region { uint8* p; size_t n; std::vector<uint8> saved; };
    std::vector<Region> regions;
    auto Keep=[&](uintptr_t a,size_t n){ auto* p=reinterpret_cast<uint8*>(a); regions.push_back({p,n,std::vector<uint8>(p,p+n)}); };
    Keep(0xB7CB84,4);                       // CTimer::m_snTimeInMilliseconds
    Keep(0xB7CB5C,4);                       // CTimer::ms_fTimeStep
    Keep(0xB72914,4);                       // CGame::currArea
    Keep(0xB7CD98,0x190*2);                 // CWorld::Players
    Keep(0xB79FD8,0xB7C480-0xB79FD8);       // tablas de CTimeCycle
    Keep(0xC71B20,0x10);                    // contadores de lotes de CGlass
    Keep(0xC4B958,0x2000);                  // aTempBufferIndices
    Keep(0xC4D958,0x24*TOTAL_TEMP_BUFFER_3DVERTICES); // TempBufferVertices (3D)
    Keep(0xB6F03C,4);                       // TheCamera.m_matrix
    Keep(0xC7C628,0x24*4);                  // CMotionBlurStreaks::aStreakVertices
    Keep(0xC8D4C8,sizeof(D3DGAMMARAMP)*2);  // CGamma::ms_SavedGamma y ms_GammaTable
    Keep(0xC920C8,1);                       // gbGammaChanged
    Keep(0xBA1788,0xBA1900-0xBA1788);       // CUserDisplay (temporizador, lugar, vehículo)
    Keep(0xC8CDE8,0x100);                   // Securom: trapDisplay y triggerDisplay
    Keep(0xA49960,8+320);                   // variables de script usadas por los contadores
    auto RestoreAll=[&]{ for(auto& r:regions)std::memcpy(r.p,r.saved.data(),r.n); };
    auto Restored=[&]{ for(auto& r:regions)if(std::memcmp(r.p,r.saved.data(),r.n))return false; return true; };

    // vehículos válidos (modelos de vehículo cargados en la tabla de model infos)
    std::vector<int16> vehicleModels;
    for(int32 m=400;m<612;++m){ auto* mi=CModelInfo::GetModelInfo(m); if(mi&&int(mi->GetModelType())==6)vehicleModels.push_back(int16(m)); }
    if(vehicleModels.empty()){Write({{"event","error"},{"reason","no vehicle model infos"}});RestoreAll();return;}

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch t1(bit_cast<uintptr_t>(static_cast<const GxtChar*(CText::*)(const char*)>(&CText::Get)),FakeTextGet), t1e(0x6A0050,FakeTextGet);
        EntryPatch f1(reinterpret_cast<uintptr_t>(&CFont::SetScale),FakeSetScale), f1e(0x719380,FakeSetScale);
        EntryPatch f2(reinterpret_cast<uintptr_t>(&CFont::SetColor),FakeSetColor), f2e(0x719430,FakeSetColor);
        EntryPatch f3(reinterpret_cast<uintptr_t>(&CFont::SetFontStyle),FakeSetFontStyle), f3e(0x719490,FakeSetFontStyle);
        EntryPatch f4(reinterpret_cast<uintptr_t>(&CFont::SetWrapx),FakeSetWrapx), f4e(0x7194D0,FakeSetWrapx);
        EntryPatch f5(reinterpret_cast<uintptr_t>(&CFont::SetRightJustifyWrap),FakeSetRightJustifyWrap), f5e(0x7194F0,FakeSetRightJustifyWrap);
        EntryPatch f6(reinterpret_cast<uintptr_t>(&CFont::SetDropColor),FakeSetDropColor), f6e(0x719510,FakeSetDropColor);
        EntryPatch f7(reinterpret_cast<uintptr_t>(&CFont::SetEdge),FakeSetEdge), f7e(0x719590,FakeSetEdge);
        EntryPatch f8(reinterpret_cast<uintptr_t>(&CFont::SetProportional),FakeSetProportional), f8e(0x7195B0,FakeSetProportional);
        EntryPatch f9(reinterpret_cast<uintptr_t>(&CFont::SetOrientation),FakeSetOrientation), f9e(0x719610,FakeSetOrientation);
        EntryPatch f10(reinterpret_cast<uintptr_t>(&CFont::PrintString),FakePrintString), f10e(0x71A700,FakePrintString);
        EntryPatch h1(reinterpret_cast<uintptr_t>(&CHud::SetVehicleName),FakeSetVehicleName), h1e(0x588F50,FakeSetVehicleName);
        EntryPatch h2(reinterpret_cast<uintptr_t>(&CHud::SetZoneName),FakeSetZoneName), h2e(0x588BB0,FakeSetZoneName);
        EntryPatch z1(reinterpret_cast<uintptr_t>(&CTheZones::FindSmallestZoneForPosition),FakeFindZone), z1e(0x572360,FakeFindZone);
        EntryPatch z2(reinterpret_cast<uintptr_t>(&CEntryExitManager::GetPositionRelativeToOutsideWorld),FakeRelOutside), z2e(0x43F150,FakeRelOutside);
        EntryPatch o1(bit_cast<uintptr_t>(&COnscreenTimer::Init),FakeTimerInit), o1e(0x44CBC0,FakeTimerInit);
        EntryPatch o2(bit_cast<uintptr_t>(&COnscreenTimer::Process),FakeTimerProcess), o2e(0x44CD30,FakeTimerProcess);
        EntryPatch s1(bit_cast<uintptr_t>(&CAEDataStream::Initialise),FakeStreamInit), s1e(0x4DC2B0,FakeStreamInit);
        EntryPatch d1(0x7F9D50,FakeCurrentDevice), d2(0x7FAD20,FakeGetCaps); // las funciones RW de la DLL son envoltorios del EXE
        EntryPatch i1(0x7EF450,FakeIm3DTransform), i2(0x7EF550,FakeIm3DRender), i3(0x7EF520,FakeIm3DEnd);
        EntryPatch l1(reinterpret_cast<uintptr_t>(&LittleTest),FakeLittleTest), l1e(0x541330,FakeLittleTest);
        EntryPatch g1(reinterpret_cast<uintptr_t>(&CGlass::CalcAlphaWithNormal),FakeCalcAlpha), g1e(0x71ACF0,FakeCalcAlpha);
        EntryPatch g2(reinterpret_cast<uintptr_t>(&CGlass::RenderHiLightPolys),FakeRenderHiLight), g2e(0x71ADA0,FakeRenderHiLight);
        EntryPatch g3(reinterpret_cast<uintptr_t>(&CGlass::RenderShatteredPolys),FakeRenderShattered), g3e(0x71AE30,FakeRenderShattered);
        EntryPatch g4(bit_cast<uintptr_t>(&CAudioEngine::ReportGlassCollisionEvent),FakeGlassAudio), g4e(0x506EE0,FakeGlassAudio);
        EntryPatch g5(0x49F970,FakeAddGlass), g5d(bit_cast<uintptr_t>(&Fx_c::AddGlass),FakeAddGlass); // lote 77: Fx_c::AddGlass de la DLL ya es C++ (hook S): se parchea tambien su cuerpo
        for(auto& v:devVt)v=reinterpret_cast<void*>(&FakeDevTrap);
        devVt[7]=reinterpret_cast<void*>(&FakeGetDeviceCaps); devVt[21]=reinterpret_cast<void*>(&FakeSetGammaRamp); devVt[22]=reinterpret_cast<void*>(&FakeGetGammaRamp);
        fakeDevice.vt=devVt.data();
        for(size_t k=0;k<texts.size();++k)for(size_t j=0;j<16;++j)texts[k][j]=0;
        void* const realPlayer=player;
        const uint8 focus=*reinterpret_cast<uint8*>(0xB7CD74);
        auto& focusPed=*reinterpret_cast<void**>(0xB7CD98+focus*0x190);
        auto& focusMoney=*reinterpret_cast<int32*>(0xB7CD98+focus*0x190+0xB8);
        auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C);
        auto& currArea=*reinterpret_cast<int32*>(0xB72914); auto& camMatrix=*reinterpret_cast<CMatrix**>(0xB6F03C);
        auto* trapDisp=reinterpret_cast<int32*>(0xC8CDE8);

        // fixtures reutilizables
        alignas(16) static std::array<uint8,0x800> pedA{}, pedB{}, pedC{}, vehA{};
        alignas(16) static std::array<uint8,0x40> pdata{}, matA{}, matB{};
        alignas(16) static std::array<uint8,0x29C> wanted{};
        alignas(16) static std::array<std::array<uint8,0x20>,4> zones{};
        alignas(16) static CMatrix camMat;

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                auto setF=[&](void* base,size_t off,float v){ std::memcpy(static_cast<uint8*>(base)+off,&v,4); };
                auto set32=[&](void* base,size_t off,uint32 v){ std::memcpy(static_cast<uint8*>(base)+off,&v,4); };
                auto setP=[&](void* base,size_t off,const void* v){ std::memcpy(static_cast<uint8*>(base)+off,&v,4); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x70A1u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0;
                    labels.clear(); boolBits=next(); boolCalls=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CAEStreamingDecoder") {
                        alignas(16) static std::array<uint8,8> obj{}; alignas(16) static std::array<uint8,0x28> stream{};
                        fill(stream.data(),stream.size()); labels={{obj.data(),"decoder"},{stream.data(),"stream"},{reinterpret_cast<void*>(0x7A7A0000),"filename"}};
                        const bool hasStream=next()%4!=0;
                        auto vtKind=[&]{ const uint32 v=*reinterpret_cast<uint32*>(obj.data()); return v==0x85F418||v<0x400000||v>=0x1000000 ? json("decoder_vt") : json(v); };
                        if(nm=="Constructor"){ std::memset(obj.data(),0xCD,obj.size()); initResult=next()%2;
                            auto* r=reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(obj.data(),hasStream ? stream.data() : nullptr); ret=Label(r); }
                        else { fill(obj.data(),obj.size()); setP(obj.data(),4,hasStream ? stream.data() : nullptr);
                            stream[0xC]=0; setP(stream.data(),8,next()%3 ? reinterpret_cast<void*>(0x7A7A0000) : nullptr); // cerrado (+0xC); el nombre (+0x8) solo se libera (operator delete simulado)
                            ndeleted=0; trace.get_ref<json::array_t&>().reserve(trace.size()+64); { EntryPatch n1(0x8214BD,FakeDelete),n2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); }
                            for(size_t k=0;k<ndeleted;++k)trace.push_back({"delete",Label(deleted[k])});
                            out["stream_filename"]=Label(*reinterpret_cast<void**>(stream.data()+8)); out["stream_open"]=stream[0xC]; }
                        out["vt"]=vtKind(); out["field"]=Label(*reinterpret_cast<void**>(obj.data()+4));
                    }
                    else if(cls=="CAEStreamTransformer") {
                        alignas(16) static std::array<uint8,16> obj{}; fill(obj.data(),obj.size());
                        if(nm=="Initialise") reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else { alignas(16) static std::array<uint8,640> buf{}; fill(buf.data(),buf.size());
                            const uint32 off=next()%32; uint32 size=next()%4==0 ? 0 : next()%600; if(next()%8==0)size=1+next()%16; const uint32 pos=next()%3 ? next()%100000 : next();
                            reinterpret_cast<void(__thiscall*)(void*,void*,size_t,uint32)>(a)(obj.data(),buf.data()+off,size,pos); out["buffer"]=Fnv(buf.data(),buf.size()); out["head"]=ReversingProbe::Bytes(buf.data(),64); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if(cls=="CColourSet") {
                        auto randomSet=[&](uint8* s){ fill(s,0xAC);
                            for(size_t o:{0x0,0x4,0x8,0xC,0x10,0x14,0x18,0x1C,0x20})setF(s,o,uni(0.f,255.f));
                            for(size_t o=0x24;o<=0x3A;o+=2){ const uint16 v=uint16(next()%5==0 ? next() : next()%256); std::memcpy(s+o,&v,2); }
                            for(size_t o:{0x3C,0x40,0x44})setF(s,o,uni(-20.f,30.f));
                            for(size_t o=0x48;o<=0x4C;o+=2){ const int16 v=int16(next()%5==0 ? next() : int32(next()%300)-20); std::memcpy(s+o,&v,2); }
                            for(size_t o:{0x50,0x54})setF(s,o,uni(-100.f,2000.f)); setF(s,0x58,uni(0.f,25.f));
                            for(size_t o=0x5C;o<=0x66;o+=2){ const uint16 v=uint16(next()%5==0 ? next() : next()%256); std::memcpy(s+o,&v,2); }
                            for(size_t o=0x68;o<=0x98;o+=4)setF(s,o,uni(-10.f,255.f));
                            set32(s,0x9C,next()%5==0 ? uint32(int32(next()%200001)-100000) : next()%256);
                            { const uint16 v=uint16(next()%5==0 ? next() : next()%256); std::memcpy(s+0xA0,&v,2); }
                            setF(s,0xA4,uni(0.f,2.55f)); setF(s,0xA8,uni(0.5f,2.f)); };
                        alignas(16) static std::array<uint8,0xAC> dst{}, A{}, B{};
                        if(nm=="Constructor"){ auto* tc=reinterpret_cast<uint8*>(0xB79FD8); fill(tc,0xB7C480-0xB79FD8); std::memset(dst.data(),0xCD,dst.size());
                            const int32 t=int32(next()%8), w=int32(next()%23);
                            auto* r=reinterpret_cast<void*(__thiscall*)(void*,int32,int32)>(a)(dst.data(),t,w); ret=r==dst.data(); }
                        else { randomSet(dst.data()); randomSet(A.data()); randomSet(B.data());
                            uint8* pa=A.data(); uint8* pb=B.data(); const uint32 alias=next()%10; if(alias==0)pa=dst.data(); else if(alias==1)pb=dst.data(); else if(alias==2)pb=pa;
                            float ma=uni(-0.5f,1.5f), mb=next()%2 ? 1.f-ma : uni(-0.5f,1.5f); if(next()%8==0){ma=float(next()%2);mb=1.f-ma;} if(next()%10==0){ma=uni(-3.f,3.f);mb=uni(-3.f,3.f);}
                            const uint32 ignore=(next()%2)|(next()%4==0 ? next()&0xFFFFFF00u : 0u);
                            reinterpret_cast<void(__thiscall*)(void*,void*,void*,float,float,uint32)>(a)(dst.data(),pa,pb,ma,mb,ignore);
                            out["A"]=Fnv(A.data(),A.size()); out["B"]=Fnv(B.data(),B.size()); }
                        out["dst"]=ReversingProbe::Bytes(dst.data(),dst.size());
                    }
                    else if(cls=="CCrime" && nm=="FindImmediateDetectionRange") {
                        int32 c=int32(next()%30)-3; if(next()%10==0)c=int32(next());
                        ret=F(reinterpret_cast<float(__cdecl*)(int32)>(a)(c));
                    }
                    else if(cls=="CCrime") {
                        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
                        EntryPatch c1(reinterpret_cast<uintptr_t>(&IsPedPointerValid),FakePedValid), c1e(0x5E8A30,FakePedValid);
                        EntryPatch c2(reinterpret_cast<uintptr_t>(&CPedType::PoliceDontCareAboutCrimesAgainstPedType),FakePoliceDontCare), c2e(0x608A40,FakePoliceDontCare);
                        EntryPatch c3(bit_cast<uintptr_t>(&CPlayerPed::GetWantedLevel),FakeGetWantedLevel), c3e(0x41BE60,FakeGetWantedLevel);
                        EntryPatch c4(bit_cast<uintptr_t>(&CPlayerPed::SetWantedLevelNoDrop),FakePedSetWantedNoDrop), c4e(0x609F30,FakePedSetWantedNoDrop);
                        EntryPatch c5(bit_cast<uintptr_t>(&CWanted::SetWantedLevelNoDrop),FakeWantedSetNoDrop), c5e(0x562570,FakeWantedSetNoDrop);
                        EntryPatch c6(bit_cast<uintptr_t>(&CWanted::RegisterCrime),FakeRegisterCrime), c6e(0x562410,FakeRegisterCrime);
                        EntryPatch c7(bit_cast<uintptr_t>(&CWanted::RegisterCrime_Immediately),FakeRegisterCrimeNow), c7e(0x562430,FakeRegisterCrimeNow);
                        EntryPatch c8(reinterpret_cast<uintptr_t>(&CWanted::WorkOutPolicePresence),FakePolicePresence), c8e(0x5625F0,FakePolicePresence);
                        EntryPatch c9(reinterpret_cast<uintptr_t>(&CLocalisation::GermanGame),FakeGermanGame), c9e(0x56D200,FakeGermanGame);
                        EntryPatch c10(reinterpret_cast<uintptr_t>(&CMessages::AddBigMessage),FakeBigMessage), c10e(0x69F2B0,FakeBigMessage);
                        // jugador devuelto por FindPlayerPed: datos de jugador (+0x480) -> CWanted (+0)
                        fill(pedA.data(),pedA.size()); fill(pedB.data(),pedB.size()); fill(pedC.data(),pedC.size()); fill(wanted.data(),wanted.size()); fill(pdata.data(),pdata.size()); fill(matA.data(),matA.size());
                        setP(pedA.data(),0x480,pdata.data()); setP(pdata.data(),0,wanted.data());
                        { float m=uni(-0.5f,2.f); if(next()%8==0)m=0.f; if(next()%16==0)m=std::numeric_limits<float>::quiet_NaN(); setF(wanted.data(),0x14,m); }
                        set32(pedA.data(),0x598,next()%2); setP(pedA.data(),0x14,nullptr);
                        playerResult=pedA.data(); wantedResult=next()%3==0 ? next()%7 : 0;
                        labels={{pedA.data(),"player"},{pedB.data(),"committer"},{pedC.data(),"victim"},{wanted.data(),"wanted"},{pdata.data(),"pdata"}};
                        const int32 crime=next()%6==0 ? 2 : int32(next()%26);
                        // autor
                        void* by=nullptr; switch(next()%8){case 0:by=nullptr;break;case 1:case 2:case 3:by=pedA.data();break;default:by=pedB.data();}
                        { static const uint32 types[]{0,1,0,1,2,6,7,16}; set32(pedB.data(),0x598,types[next()%8]); }
                        if(next()%2){ setP(pedB.data(),0x14,matA.data()); for(size_t o:{0x30,0x34,0x38})setF(matA.data(),o,uni(-3000.f,3000.f)); } else { setP(pedB.data(),0x14,nullptr); for(size_t o:{4,8,12})setF(pedB.data(),o,uni(-3000.f,3000.f)); }
                        if(by==pedA.data())for(size_t o:{4,8,12})setF(pedA.data(),o,uni(-3000.f,3000.f));
                        // víctima (nunca nula con CRIME_DAMAGED_PED: el EXE la desreferencia sin comprobar)
                        void* victim=(crime!=2&&next()%5==0) ? nullptr : pedC.data();
                        { static const uint8 etypes[]{3,3,3,2,4,1}; pedC[0x36]=uint8((pedC[0x36]&~7)|etypes[next()%6]); }
                        { static const uint32 ptypes[]{4,5,6,7,8,16,17,18,19,20,0,1}; set32(pedC.data(),0x598,ptypes[next()%12]); }
                        { const uint32 st=next()%4==0 ? 54+next()%2 : next()%60; set32(pedC.data(),0x530,st); }
                        if(next()%2)pedC[0x46E]|=0x80; else pedC[0x46E]&=0x7F; // bBeingChasedByPolice (bit 23 de 0x46C)
                        focusMoney=int32(next()%100000);
                        reinterpret_cast<void(__cdecl*)(int32,void*,void*)>(a)(crime,victim,by);
                        out["money"]=focusMoney;
                    }
                    else if(cls=="CCurrentVehicle"||cls=="CPlaceName"||cls=="CUserDisplay") {
                        // jugador en foco falso: bInVehicle (bit 8 de 0x46C) y m_pVehicle (0x58C)
                        fill(pedA.data(),pedA.size()); fill(vehA.data(),vehA.size()); fill(matA.data(),matA.size()); fill(matB.data(),matB.size());
                        labels={{pedA.data(),"player"},{vehA.data(),"vehicle"}};
                        const bool inVeh=next()%2; if(inVeh)pedA[0x46D]|=1; else pedA[0x46D]&=~1;
                        // m_pVehicle nulo solo para CCurrentVehicle: CalcPosition lo desreferencia sin comprobar en el EXE
                        setP(pedA.data(),0x58C,(cls=="CCurrentVehicle"&&next()%8==0) ? nullptr : vehA.data());
                        *reinterpret_cast<int16*>(vehA.data()+0x22)=vehicleModels[next()%vehicleModels.size()];
                        if(next()%2){ setP(pedA.data(),0x14,matA.data()); for(size_t o:{0x30,0x34,0x38})setF(matA.data(),o,uni(-3000.f,3000.f)); } else { setP(pedA.data(),0x14,nullptr); for(size_t o:{4,8,12})setF(pedA.data(),o,uni(-3000.f,3000.f)); }
                        if(next()%2){ setP(vehA.data(),0x14,matB.data()); for(size_t o:{0x30,0x34,0x38})setF(matB.data(),o,uni(-3000.f,3000.f)); } else { setP(vehA.data(),0x14,nullptr); for(size_t o:{4,8,12})setF(vehA.data(),o,uni(-3000.f,3000.f)); }
                        focusPed=pedA.data(); relShift=next()%2;
                        // zonas falsas (la 2 repite la etiqueta de la 0)
                        static const char* names[]{"GAN","LA","SF","VE","BLUEB","MONT","DILLI","CHC"};
                        for(size_t z=0;z<zones.size();++z){ fill(zones[z].data(),0x20); char lab[8]{}; std::strncpy(lab,names[next()%8],7); for(size_t k=strlen(lab)+1;k<8;++k)lab[k]=char(next());
                            std::memcpy(zones[z].data()+8,lab,8); labels.push_back({zones[z].data(),"zone"+std::to_string(z)}); }
                        if(next()%2)std::memcpy(zones[2].data()+8,zones[0].data()+8,8);
                        auto pickZone=[&](bool allowNull)->void*{ const uint32 r=next()%5; return (allowNull&&r==4) ? nullptr : static_cast<void*>(zones[r%4].data()); };
                        zoneResult=pickZone(nm!="GetForMap");
                        { const uint32 r=next()%4; currArea=r==0 ? int32(1+next()%17) : r==1 ? 1 : 0; }
                        alignas(16) static std::array<uint8,8> obj{};
                        auto setupPlace=[&](uint8* p){ fill(p,8); setP(p,0,next()%4==0 ? nullptr : (next()%2 ? zoneResult : pickZone(true)));
                            const uint16 t=uint16(next()%3==0 ? 0 : next()%3==0 ? next() : next()%300); std::memcpy(p+4,&t,2); };
                        if(cls=="CCurrentVehicle"){ fill(obj.data(),obj.size());
                            if(nm=="Display"){ setP(obj.data(),0,next()%4 ? vehA.data() : nullptr); reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); }
                            else reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                            out["obj"]=ReversingProbe::Bytes(obj.data(),4); }
                        else if(cls=="CPlaceName"){ setupPlace(obj.data());
                            if(nm=="GetForMap"){ const float x=uni(-3000.f,3000.f), y=uni(-3000.f,3000.f); ret=TextLabel(reinterpret_cast<const GxtChar*(__thiscall*)(void*,float,float)>(a)(obj.data(),x,y)); }
                            else reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                            out["zone"]=Label(*reinterpret_cast<void**>(obj.data())); out["obj"]=ReversingProbe::Bytes(obj.data()+4,4); }
                        else { auto* ud=reinterpret_cast<uint8*>(0xBA18F4); setupPlace(ud); setP(ud,8,next()%3 ? vehA.data() : nullptr);
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            out["zone"]=Label(*reinterpret_cast<void**>(ud)); out["place"]=ReversingProbe::Bytes(ud+4,4); out["vehicle"]=Label(*reinterpret_cast<void**>(ud+8)); }
                        out["player_ped"]=Label(focusPed);
                    }
                    else if(cls=="CFallingGlassPane") {
                        alignas(16) static std::array<uint8,0x70> pane{}; fill(pane.data(),pane.size());
                        for(size_t o=0;o<0x30;o+=4)if(o%16!=12)setF(pane.data(),o,uni(-1.f,1.f));
                        setF(pane.data(),0x30,uni(-200.f,200.f)); setF(pane.data(),0x34,uni(-200.f,200.f)); setF(pane.data(),0x38,uni(0.f,40.f));
                        set32(pane.data(),0x40,0); pane[0x44]=0;
                        for(size_t o:{0x48,0x4C,0x50})setF(pane.data(),o,uni(-0.3f,0.3f)); for(size_t o:{0x54,0x58,0x5C})setF(pane.data(),o,uni(-0.05f,0.05f));
                        timeMs=next()%2 ? 100000+next()%100000 : next(); timeStep=next()%6==0 ? 1.f : uni(0.05f,3.f);
                        { const uint32 r=next()%4; set32(pane.data(),0x60,r==0 ? timeMs+1+next()%800 : r==1 ? next() : timeMs-next()%1000); }
                        setF(pane.data(),0x64,*reinterpret_cast<float*>(pane.data()+0x38)+uni(-2.f,1.f)); setF(pane.data(),0x68,uni(0.05f,1.f));
                        pane[0x6C]=uint8(next()%4); pane[0x6D]=1; pane[0x6E]=uint8(next()%2); pane[0x6F]=uint8(next()%2);
                        if(nm=="Update") reinterpret_cast<void(__thiscall*)(void*)>(a)(pane.data());
                        else {
                            alphaResult=uint8(next()%5==0 ? 255 : next()); camMatrix=&camMat; camMat.GetPosition()=CVector{
                                *reinterpret_cast<float*>(pane.data()+0x30)+uni(-35.f,35.f),*reinterpret_cast<float*>(pane.data()+0x34)+uni(-35.f,35.f),*reinterpret_cast<float*>(pane.data()+0x38)+uni(-20.f,20.f)};
                            std::memset(reinterpret_cast<void*>(0xC4B958),0xCD,0x2000); std::memset(reinterpret_cast<void*>(0xC4D958),0xCD,0x24*TOTAL_TEMP_BUFFER_3DVERTICES);
                            CGlass::H1iLightPolyVerticesIdx=next()%6==0 ? 1014+next()%12 : next()%1020; CGlass::HiLightPolyIndicesIdx=next()%6==0 ? int32(2036+next()%12) : int32(next()%2041);
                            CGlass::ShatteredVerticesBaseIdx=next()%6==0 ? int32(1526+next()%12) : int32(1024+next()%508); CGlass::ShatteredIndicesBaseIdx=next()%6==0 ? int32(3060+next()%12) : int32(2048+next()%1017);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(pane.data());
                            out["counters"]={CGlass::H1iLightPolyVerticesIdx,CGlass::HiLightPolyIndicesIdx,CGlass::ShatteredVerticesBaseIdx,CGlass::ShatteredIndicesBaseIdx};
                            out["indices"]=Fnv(reinterpret_cast<void*>(0xC4B958),0x2000); out["vertices"]=Fnv(reinterpret_cast<void*>(0xC4D958),0x24*TOTAL_TEMP_BUFFER_3DVERTICES);
                        }
                        out["pane"]=ReversingProbe::Bytes(pane.data(),pane.size());
                    }
                    else if(cls=="CGamma") {
                        alignas(16) static std::array<uint8,16> obj{}; fill(obj.data(),obj.size());
                        for(auto& v:rampPattern)v=uint16(next()); deviceNull=next()%7==0;
                        fill(reinterpret_cast<void*>(0xC8D4C8),sizeof(D3DGAMMARAMP)*2);
                        if(nm=="Init"){ deviceNull=false; std::memset(&fakeCaps,0,sizeof(fakeCaps)); fakeCaps.DeclTypes=next();
                            static const uint32 caps2[]{0,0x20000,0x100000,0x120000}; devCaps2=caps2[next()%4]|(next()&~0x120000u);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); out["decl_types"]=uint32(fakeCaps.DeclTypes); }
                        else { static const uint32 en[]{0,1,1,1,0x100}; set32(obj.data(),0,en[next()%5]);
                            float level=next()%6==0 ? uni(-1.f,2.f) : uni(0.f,1.f); if(next()%10==0)level=float(next()%2);
                            setF(obj.data(),8,next()%3==0 ? level*0.7f : uni(-1.f,1.5f));
                            *reinterpret_cast<uint8*>(0xC920C8)=uint8(next()%2);
                            reinterpret_cast<void(__thiscall*)(void*,float,uint32)>(a)(obj.data(),level,(next()%2)|(next()%4==0 ? next()&0xFFFFFF00u : 0u));
                            out["changed"]=*reinterpret_cast<uint8*>(0xC920C8); out["saved"]=Fnv(reinterpret_cast<void*>(0xC8D4C8),sizeof(D3DGAMMARAMP)); out["table"]=Fnv(reinterpret_cast<void*>(0xC8DAC8),sizeof(D3DGAMMARAMP));
                            out["table_head"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC8DAC8),32); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if(cls=="COnscreenCounterEntry") {
                        alignas(16) static std::array<uint8,0x44> obj{}; fill(obj.data(),obj.size());
                        if(nm=="SetColourID") reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(obj.data(),next());
                        else { auto* vars=reinterpret_cast<uint8*>(0xA49960); for(size_t k=8;k<8+320;k+=4){ const uint32 r=next()%8; const int32 v=r==0 ? int32(0x80000000u) : r==1 ? -int32(next()%1000) : r==2 ? int32(next()) : int32(next()%10000); std::memcpy(vars+k,&v,4); }
                            set32(obj.data(),0,8+(next()%2 ? 4*(next()%75) : next()%297)); set32(obj.data(),4,8+(next()%2 ? 4*(next()%75) : next()%297));
                            uint32 type=next()%3; if(next()%10==0)type=next()%0x10000; type|=next()%4==0 ? next()&0xFFFF0000u : 0u;
                            reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(obj.data(),type); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if(cls=="CRegisteredMotionBlurStreak") {
                        alignas(16) static std::array<uint8,0x54> obj{}; fill(obj.data(),obj.size());
                        for(size_t o=8;o<0x50;o+=4)setF(obj.data(),o,uni(-500.f,500.f));
                        for(size_t k=0;k<3;++k)obj[0x50+k]=uint8(next()%3!=0);
                        if(nm=="Update") reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else { transformOk=next()%5!=0; std::memset(reinterpret_cast<void*>(0xC7C628),0xCD,0x24*4);
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); out["vertices"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC7C628),0x24*4); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if(cls=="Securom") {
                        for(int k=0;k<64;++k){ const uint32 r=next()%6; trapDisp[k]=r<3 ? 0 : r==3 ? int32(1+next()%3) : r==4 ? -int32(next()%5) : int32(next()); }
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["state"]=ReversingProbe::Bytes(trapDisp,0x100);
                    }
                    out["ret"]=ret; out["trace"]=trace; RestoreAll(); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
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
    RestoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Restored()}});
}
}
