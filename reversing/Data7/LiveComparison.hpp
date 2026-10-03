#pragma once
// Banco dentro del motor: policia, ropa, listas, plantas, marcas, efectos y streaming (lista de 7, 45 funciones).
// CCopPed (constructor, destructor, compañero, lista de criminales y ProcessControl), CPedClothesDesc, CPedList
// (Empty, BuildListFromGroup_NoLeader, ExtractPedsWithGuns), CPPTriPlantBuffer, CSkidmarks, CSpecialFX y CStreamingInfo.
// Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures identicos en buffers propios:
// policias y peatones falsos (nunca peatones reales de los pools), inteligencia, CWanted, vehiculos, grupo, buffer de
// plantas, materiales, arrays falsos de CStreamingInfo (se cambia ms_pArrayBase temporalmente) y matrices RW.
// Se simulan y trazan: CPed::CPed/~CPed/SetModelIndex/ProcessControl/FlagToDestroyWhenNextProcessed/GiveWeapon/
// GiveDelayedWeapon/SetCurrentWeapon/Say, CAEPedSpeechAudioEntity::Initialise, CEntity::RegisterReference/
// CleanUpOldReference/GetIsOnScreen, CStreaming::GetDefaultCopModel, FindPlayerPed/FindPlayerWanted,
// CWanted::RemovePursuitCop, CPedIntelligence::SetPedDecisionMakerType, CWeapon::Update/IsTypeMelee,
// CWorld::ProcessLineOfSight, CGrassRenderer::DrawTriPlants, RpMaterialSetTexture, CTxdStore, RwTextureRead/Destroy,
// RwRasterDestroy, RwIm3D*, RwFrameGetLTM, RwRenderStateSet (puntero del motor), LittleTest, CCamera::IsSphereVisible,
// C3dMarkers/CCheckpoints/CBulletTraces/CMotionBlurStreaks/CBrightLights/CShinyTexts (Init/Update/Render/Shutdown),
// CRegisteredMotionBlurStreak::Update, RegisterStreak, CFont, AsciiToGxtChar, CSprite2d y el RNG (rand/srand del EXE
// y de la DLL). Los globales tocados (marcas, efectos, espejos, reloj, contador de frames, escala de tiempo, modo de
// repeticion, cámara, resolucion, buffers temporales) se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity/Ped/CopPed.h"
#include "PedClothesDesc.h"
#include "PedList.h"
#include "PPTriPlantBuffer.h"
#include "GrassRenderer.h"
#include "Skidmarks.h"
#include "SpecialFX.h"
#include "StreamingInfo.h"
#include "Streaming.h"
#include "MotionBlurStreaks.h"
#include "RegisteredMotionBlurStreak.h"
#include "BulletTraces.h"
#include "Checkpoints.h"
#include "3dMarkers.h"
#include "BrightLights.h"
#include "ShinyTexts.h"
#include "Sprite2d.h"
#include "Font.h"
#include "TxdStore.h"
#include "Wanted.h"
#include "World.h"
#include "Weapon.h"
#include "PedIntelligence.h"
#include "Camera.h"
#include "GxtChar.h"
#include "../ProbeRuntime.hpp"

namespace Data7Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

constexpr uintptr_t COP_VT=0x86C120, TIMER=0xB7CB84, FRAMES=0xB7CB4C, TIMESCALE=0xB7CB64, ARRAY_BASE=0x9654B4, GRASS_TEX=0xC02DC0;
constexpr uintptr_t SKID=0xC79AA8, SKID_N=32*0x158, SKID_IDX=0xC799C8, SKID_TEX=0xC79A88, TEMPV=0xC4D958, REPLAY=0xA43088, DN=0x8D12C0;
constexpr uintptr_t FX_A=0xC7C600, FX_A_N=0x560, FX_B=0xC80250, FX_B_N=0x198, GSTRING=0xB71670, ENGINE=0xC97B24;
constexpr uintptr_t RS_W=0xC17044, RS_H=0xC17048, CAMIDX=0xB6F081;
constexpr size_t P_INTEL=0x47C, P_CREATED=0x484, P_STATE=0x530, P_HEALTH=0x540, P_VEH=0x58C, P_TYPE=0x598, P_WEAPONS=0x5A0, P_SLOT=0x718, P_TARGET=0x71C;
constexpr size_t COP_SIZE=sizeof(CCopPed), SI=0x14, NSI=64, SIPAD=4;

// --- buffers propios y nombres ---
alignas(16) inline std::array<std::array<uint8,COP_SIZE>,3> cops{};
alignas(16) inline std::array<std::array<uint8,COP_SIZE>,6> peds{};
alignas(16) inline std::array<std::array<uint8,0x600>,2> vehs{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,0x300> wanted{}, ent{}, weaponObj{}, rwMat{}, membership{};
alignas(16) inline std::array<uint8,0x80> clothes{};
alignas(16) inline std::array<uint8,0x7C> list1{}, list2{};
alignas(16) inline std::array<uint8,sizeof(CPPTriPlantBuffer)> plants{};
alignas(16) inline std::array<uint8,0x40> material{}, rgba{}, texture{}, raster{};
alignas(16) inline std::array<uint8,SI*(NSI+2*SIPAD)> infos{};
inline uint8* InfoBase() { return infos.data()+SI*SIPAD; }
inline uintptr_t dllCopVt{};
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v==COP_VT||(dllCopVt&&v==dllCopVt))return "cop_vt";
    for(auto& r:named)if(v>=uintptr_t(r.p)&&v<uintptr_t(r.p)+r.n)return std::string(r.name)+"+"+std::to_string(v-uintptr_t(r.p));
    if(v>=0x7000&&v<0x8000)return std::format("fake{:x}",v);
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t from,size_t to) { json r=json::array(); for(size_t k=from;k+4<=to;k+=4)r.push_back(Name(*reinterpret_cast<const uint32*>(static_cast<const uint8*>(p)+k))); return r; }
inline json Str(const char* s,size_t max=64) { std::string r; for(size_t k=0;k<max&&s[k];++k)r+=std::format("{:02x}",uint8(s[k])); return r; }

// --- configuracion por caso ---
inline uint32 rngState{}, maskA{}, callsA{}, maskB{}, callsB{};
inline uintptr_t ctorTarget{}, playerRet{}, losEntity{}; inline int32 defaultCopModel{}, findTxdRet{};
inline uint32 postpone{}; inline bool onScreen{};
inline bool NextA() { return (maskA>>(callsA++%32))&1; }
inline bool NextB() { return (maskB>>(callsB++%32))&1; }

// --- RNG ---
inline int __cdecl FakeRand(){ rngState=rngState*214013u+2531011u; const int r=int(rngState>>16)&0x7FFF; trace.push_back({"rand",r}); return r; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline void __cdecl FakeSrand(unsigned int s){ trace.push_back({"srand",s}); rngState=s; }
// --- peatones / policias ---
inline void* __fastcall FakePedCtor(uint8* self,void*,int32 type) { trace.push_back({"ped_ctor",Name(self),type}); At<void*>(self,P_INTEL)=intel.data(); At<uintptr_t>(self,P_TARGET)=ctorTarget; return self; }
inline void __fastcall FakePedDtor(void* self,void*) { trace.push_back({"ped_dtor",Name(self),Name(At<uint32>(self,0))}); }
inline void __fastcall FakeSetModel(void* self,void*,uint32 idx) { trace.push_back({"set_model_index",Name(self),idx}); }
inline void __fastcall FakeSpeechInit(void* self,void*,void* e) { trace.push_back({"speech_initialise",Name(self),Name(e)}); }
inline int32 __fastcall FakeGiveWeapon(void* self,void*,int32 type,uint32 ammo,uint32 b) { trace.push_back({"give_weapon",Name(self),type,ammo,uint8(b)}); return 1; }
inline void __fastcall FakeGiveDelayed(void* self,void*,int32 type,uint32 ammo) { trace.push_back({"give_delayed_weapon",Name(self),type,ammo}); }
inline void __fastcall FakeSetCurrentWeapon(void* self,void*,int32 type) { trace.push_back({"set_current_weapon",Name(self),type}); }
inline int32 __cdecl FakeDefaultCopModel() { trace.push_back({"default_cop_model"}); return defaultCopModel; }
inline void __fastcall FakeRegisterRef(void* self,void*,void* ref) { trace.push_back({"register_ref",Name(self),Name(ref)}); }
inline void __fastcall FakeCleanUpRef(void* self,void*,void* ref) { trace.push_back({"clean_up_ref",Name(self),Name(ref)}); }
inline CPlayerPed* __cdecl FakeFindPed(int32 id) { trace.push_back({"find_player_ped",id}); return reinterpret_cast<CPlayerPed*>(playerRet); }
inline CWanted* __cdecl FakeFindWanted(int32 id) { trace.push_back({"find_player_wanted",id}); return reinterpret_cast<CWanted*>(wanted.data()); }
inline void __fastcall FakeRemovePursuit(void* self,void*,void* cop) { trace.push_back({"remove_pursuit_cop",Name(self),Name(cop)}); }
inline void __fastcall FakeSetDm(void* self,void*,int32 t) { trace.push_back({"set_dm_type",Name(self),t}); }
inline void __fastcall FakePedProcess(uint8* self,void*) { trace.push_back({"ped_process_control",Name(self)}); auto& f=At<uint32>(self,0x1C); if(postpone==1)f|=0x40u; else if(postpone==2)f&=~0x40u; }
inline void __fastcall FakeWeaponUpdate(void* w,void*,void* p) { trace.push_back({"weapon_update",Name(w),Name(p)}); }
inline int16 __fastcall FakeSay(void* self,void*,uint32 ctx,uint32 delay,float prob,uint32 a,uint32 b,uint32 c) { trace.push_back({"say",Name(self),ctx&0xFFFF,delay,F(prob),uint8(a),uint8(b),uint8(c)}); return 0; }
inline bool __fastcall FakeOnScreen(void* self,void*) { trace.push_back({"is_on_screen",Name(self),onScreen}); return onScreen; }
inline void __fastcall FakeFlagDestroy(void* self,void*) { trace.push_back({"flag_to_destroy",Name(self)}); }
inline bool __cdecl FakeLos(const CVector& o,const CVector& t,CColPoint&,CEntity*& e,bool b,bool v,bool p,bool ob,bool d,bool see,bool cam,bool shoot) {
    trace.push_back({"line_of_sight",F(o.x),F(o.y),F(o.z),F(t.x),F(t.y),F(t.z),b,v,p,ob,d,see,cam,shoot}); e=reinterpret_cast<CEntity*>(losEntity); return losEntity!=0; }
inline bool __fastcall FakeIsMelee(void* w,void*) { const bool r=NextA(); trace.push_back({"is_type_melee",Name(w),r}); return r; }
// --- plantas ---
inline void __cdecl FakeDrawTriPlants(void* buf,int32 n,void* tab) { trace.push_back({"draw_tri_plants",Name(buf),n,Name(tab)}); }
inline void* __cdecl FakeMatSetTexture(void* m,void* t) { trace.push_back({"material_set_texture",Name(m),Name(t)}); return m; }
// --- TXD / RW ---
inline void __cdecl FakePushTxd() { trace.push_back({"push_txd"}); }
inline void __cdecl FakePopTxd() { trace.push_back({"pop_txd"}); }
inline void __cdecl FakeSetTxd(int32 i) { trace.push_back({"set_txd",i}); }
inline int32 __cdecl FakeFindTxd(const char* n) { trace.push_back({"find_txd",Str(n)}); return findTxdRet; }
inline void* __cdecl FakeTexRead(const char* n,const char* m) { trace.push_back({"texture_read",Str(n),m ? json(Str(m)) : json(nullptr)}); return texture.data(); }
inline RwBool __cdecl FakeTexDestroy(void* t) { trace.push_back({"texture_destroy",Name(t)}); return TRUE; }
inline RwBool __cdecl FakeRasterDestroy(void* r) { trace.push_back({"raster_destroy",Name(r)}); return TRUE; }
inline void* __cdecl FakeIm3DTransform(void* v,uint32 n,void* ltm,uint32 flags) { const bool r=NextB(); trace.push_back({"im3d_transform",Name(v),n,Name(ltm),flags,Hash(v,size_t(std::min<uint32>(n,64))*0x24),r}); return r ? v : nullptr; }
inline RwBool __cdecl FakeIm3DRender(int32 type,void* idx,int32 n) { trace.push_back({"im3d_render_indexed",type,Name(idx),n}); return TRUE; }
inline RwBool __cdecl FakeIm3DEnd() { trace.push_back({"im3d_end"}); return TRUE; }
inline RwBool __cdecl FakeRenderState(int32 s,void* v) { trace.push_back({"render_state",s,Name(v)}); return TRUE; }
inline void* __cdecl FakeFrameLTM(void* f) { trace.push_back({"frame_get_ltm",Name(f)}); return rwMat.data(); }
inline void __cdecl FakeLittleTest() { trace.push_back({"little_test"}); }
inline bool __fastcall FakeSphereVisible(void* cam,void*,const CVector& c,float r) { const bool v=NextA(); trace.push_back({"sphere_visible",F(c.x),F(c.y),F(c.z),F(r),v}); return v; }
// --- efectos especiales ---
template<int Tag> void __cdecl FakeSub() { static const char* names[]{"markers_init","checkpoints_init","markers_update","checkpoints_update","bullet_traces_update",
    "markers_render","checkpoints_render","bullet_traces_render","mbs_render","bright_lights_render","shiny_texts_render","markers_shutdown"}; trace.push_back({names[Tag]}); }
inline void __fastcall FakeStreakUpdate(void* s,void*) { trace.push_back({"streak_update",Name(s)}); }
inline void __cdecl FakeRegisterStreak(uint32 id,uint32 r,uint32 g,uint32 b,uint32 a,CVector l,CVector rr) {
    trace.push_back({"register_streak",Name(id),uint8(r),uint8(g),uint8(b),uint8(a),F(l.x),F(l.y),F(l.z),F(rr.x),F(rr.y),F(rr.z)}); }
inline void __cdecl FakeSetScale(float w,float h) { trace.push_back({"font_scale",F(w),F(h)}); }
inline void __cdecl FakeSetBackground(uint32 a,uint32 b) { trace.push_back({"font_background",uint8(a),uint8(b)}); }
inline void __cdecl FakeSetProportional(uint32 a) { trace.push_back({"font_proportional",uint8(a)}); }
inline void __cdecl FakeSetOrientation(uint32 a) { trace.push_back({"font_orientation",uint8(a)}); }
inline void __cdecl FakeSetColor(uint32 c) { trace.push_back({"font_color",c}); }
inline void __cdecl FakeSetFontStyle(uint32 s) { trace.push_back({"font_style",uint8(s)}); }
inline void __cdecl FakePrintString(float x,float y,const char* s) { trace.push_back({"print_string",F(x),F(y),Name(s)}); }
inline void __cdecl FakeAsciiToGxt(const char* src,void* dst) { trace.push_back({"ascii_to_gxt",Str(src),Name(dst)}); }
inline void __cdecl FakePolygon(float x1,float y1,float x2,float y2,float x3,float y3,float x4,float y4,const CRGBA& c) {
    trace.push_back({"polygon",F(x1),F(y1),F(x2),F(y2),F(x3),F(y3),F(x4),F(y4),*reinterpret_cast<const uint32*>(&c)}); }
inline void __cdecl FakeDrawRect(const CRect& r,const CRGBA& c) { const auto* w=reinterpret_cast<const uint32*>(&r); trace.push_back({"draw_rect",w[0],w[1],w[2],w[3],*reinterpret_cast<const uint32*>(&c)}); }

inline uintptr_t LocalAddress(HMODULE module,const char* name) { auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name)); if(p&&*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1); return p; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=45;
inline const std::array<Fn,NFNS> fns{{
    {"CCopPed","Constructor",0x5DDC60,0,-1,512},{"CCopPed","Destructor",0x5DE0D0,0,-1,256},{"CCopPed","SetPartner",0x5DDE80,0,-1,256},
    {"CCopPed","AddCriminalToKill",0x5DDEB0,0,-1,1024},{"CCopPed","RemoveCriminalToKill",0x5DE040,0,-1,256},{"CCopPed","ClearCriminalsToKill",0x5DE070,0,-1,256},
    {"CCopPed","ProcessControl",0,0x86C120,10,512},
    {"CPedClothesDesc","Constructor",0x5A8020,0,-1,64},{"CPedClothesDesc","Initialise",0x5A78F0,0,-1,64},{"CPedClothesDesc","GetIsWearingBalaclava",0x5A7950,0,-1,256},
    {"CPedClothesDesc","HasVisibleNewHairCut",0x5A7970,0,-1,512},{"CPedClothesDesc","HasVisibleTattoo",0x5A79D0,0,-1,256},
    {"CPedClothesDesc","SetModel-modelHash",0x5A7910,0,-1,256},{"CPedClothesDesc","SetModel-modelName",0x5A7920,0,-1,256},
    {"CPedList","Empty",0x699DB0,0,-1,64},{"CPedList","BuildListFromGroup_NoLeader",0x699DD0,0,-1,256},{"CPedList","ExtractPedsWithGuns",0x69A4C0,0,-1,512},
    {"CPPTriPlantBuffer","Flush",0x5DB0C0,0,-1,512},{"CPPTriPlantBuffer","GetPPTriPlantPtr",0x5DB140,0,-1,512},{"CPPTriPlantBuffer","ChangeCurrentPlantModelsSet",0x5DB170,0,-1,256},
    {"CPPTriPlantBuffer","IncreaseBufferIndex",0x5DB1A0,0,-1,512},{"CPPTriPlantBuffer","GetPlantModelsTab",0x5DACA0,0,-1,256},{"CPPTriPlantBuffer","SetPlantModelsTab",0x5DAC80,0,-1,256},
    {"CPPTriPlantBuffer","SetGrassMaterialCB",0x5DAC10,0,-1,256},
    {"CSkidmarks","Init",0x7204E0,0,-1,64},{"CSkidmarks","Shutdown",0x720570,0,-1,64},{"CSkidmarks","Clear",0x720590,0,-1,64},{"CSkidmarks","Update",0x7205C0,0,-1,512},
    {"CSkidmarks","Render",0x720640,0,-1,256},{"CSkidmarks","RegisterOne-",0x720930,0,-1,1024},{"CSkidmarks","RegisterOne-EC0",0x720EC0,0,-1,512},
    {"CSpecialFX","Init",0x7268F0,0,-1,64},{"CSpecialFX","Update",0x726AA0,0,-1,128},{"CSpecialFX","Shutdown",0x723390,0,-1,128},{"CSpecialFX","AddWeaponStreak",0x7233F0,0,-1,512},
    {"CSpecialFX","Render",0x726AD0,0,-1,64},{"CSpecialFX","Render2DFXs",0x721660,0,-1,256},{"CSpecialFX","ReplayStarted",0x721D30,0,-1,64},
    {"CStreamingInfo","Init",0x407460,0,-1,128},{"CStreamingInfo","AddToList",0x407480,0,-1,512},{"CStreamingInfo","GetCdPosn",0x407570,0,-1,256},
    {"CStreamingInfo","SetCdPosnAndSize",0x4075E0,0,-1,128},{"CStreamingInfo","GetCdPosnAndSize",0x4075A0,0,-1,256},{"CStreamingInfo","InList",0x407560,0,-1,128},
    {"CStreamingInfo","RemoveFromList",0x4074E0,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"data7-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"data7-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("data7");}
    auto* realPlayer=FindPlayerPed(); if(!realPlayer || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"data7-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    HMODULE module{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    const std::array<const char*,6> exports{{
        "??0CPed@@QAE@W4ePedType@@@Z","?SetModelIndex@CPed@@UAEXI@Z","?Initialise@CAEPedSpeechAudioEntity@@QAEXPAVCEntity@@@Z",
        "??1CPed@@UAE@XZ","?ProcessControl@CPed@@UAEXXZ","?FlagToDestroyWhenNextProcessed@CPed@@UAEXXZ",
    }};
    std::array<uintptr_t,6> ex{}; json missing=json::array();
    for(size_t k=0;k<exports.size();++k)if(!(ex[k]=LocalAddress(module,exports[k])))missing.push_back(exports[k]);
    dllCopVt=reinterpret_cast<uintptr_t>(GetProcAddress(module,"??_7CCopPed@@6B@"));
    if(!missing.empty()){Write({{"event","error"},{"reason","missing dll export"},{"exports",missing}});return;}
    named.clear();
    for(int n=0;n<3;++n)named.push_back({cops[n].data(),cops[n].size(),n==0?"cop#0":n==1?"cop#1":"cop#2"});
    { static const char* pn[]{"ped#0","ped#1","ped#2","ped#3","ped#4","ped#5"}; for(int n=0;n<6;++n)named.push_back({peds[n].data(),peds[n].size(),pn[n]}); }
    named.push_back({vehs[0].data(),vehs[0].size(),"veh#0"}); named.push_back({vehs[1].data(),vehs[1].size(),"veh#1"});
    named.push_back({intel.data(),intel.size(),"intel"}); named.push_back({wanted.data(),wanted.size(),"wanted"}); named.push_back({ent.data(),ent.size(),"ent"});
    named.push_back({weaponObj.data(),weaponObj.size(),"weapon_obj"}); named.push_back({rwMat.data(),rwMat.size(),"rw_matrix"}); named.push_back({membership.data(),membership.size(),"membership"});
    named.push_back({clothes.data(),clothes.size(),"clothes"}); named.push_back({list1.data(),list1.size(),"list1"}); named.push_back({list2.data(),list2.size(),"list2"});
    named.push_back({plants.data(),plants.size(),"plants"}); named.push_back({material.data(),material.size(),"material"}); named.push_back({rgba.data(),rgba.size(),"rgba"});
    named.push_back({texture.data(),texture.size(),"texture"}); named.push_back({raster.data(),raster.size(),"raster"}); named.push_back({infos.data(),infos.size(),"infos"});
    named.push_back({reinterpret_cast<void*>(SKID),SKID_N,"skid"}); named.push_back({reinterpret_cast<void*>(SKID_IDX),0xC0,"skid_idx"}); named.push_back({reinterpret_cast<void*>(TEMPV),0x24*64,"tempv"});
    named.push_back({reinterpret_cast<void*>(FX_A),FX_A_N,"fx"}); named.push_back({reinterpret_cast<void*>(GSTRING),0x100,"gstring"}); named.push_back({reinterpret_cast<void*>(0xC1B100),0x400,"ggxt"});

    // estado global tocado
    auto* engine=*reinterpret_cast<uint8**>(ENGINE); auto& rsSlot=At<void*>(engine,0x20); void* const rsSaved=rsSlot;
    const uint8 camIdx=*reinterpret_cast<uint8*>(CAMIDX);
    std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{FRAMES,4},{TIMESCALE,4},{ARRAY_BASE,4},{GRASS_TEX,4},{SKID,SKID_N},{SKID_IDX,0xC0},{SKID_TEX,4},{TEMPV,0x24*64},
        {REPLAY,4},{DN,4},{FX_A,FX_A_N},{FX_B,FX_B_N},{0xC803E8,0x4C},{GSTRING,0x100},{RS_W,4},{RS_H,4},{0xB6F24C+camIdx*0x238u,4},{0xB6F25C+camIdx*0x238u,4},{0xB73708,4}};
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n));
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); rsSlot=rsSaved; };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return rsSlot==rsSaved; };
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand), r1d(bit_cast<uintptr_t>(&rand),FakeRand);
        EntryPatch s1d(bit_cast<uintptr_t>(&srand),FakeSrand), s1e(0x821B11,FakeSrand);
        EntryPatch c1(ex[0],FakePedCtor), c1e(0x5E8030,FakePedCtor);
        EntryPatch c2(ex[1],FakeSetModel), c2e(0x5E4880,FakeSetModel);
        EntryPatch c3(ex[2],FakeSpeechInit), c3e(0x4E68D0,FakeSpeechInit);
        EntryPatch c4(ex[3],FakePedDtor), c4e(0x5E8620,FakePedDtor);
        EntryPatch c5(ex[4],FakePedProcess), c5e(0x5E8CD0,FakePedProcess);
        EntryPatch c6(ex[5],FakeFlagDestroy), c6e(0x5E7B70,FakeFlagDestroy);
        EntryPatch w1(bit_cast<uintptr_t>(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),FakeGiveWeapon), w1e(0x5E6080,FakeGiveWeapon);
        EntryPatch w2(bit_cast<uintptr_t>(&CPed::GiveDelayedWeapon),FakeGiveDelayed), w2e(0x5E89B0,FakeGiveDelayed);
        EntryPatch w3(bit_cast<uintptr_t>(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon)),FakeSetCurrentWeapon), w3e(0x5E6280,FakeSetCurrentWeapon);
        EntryPatch w4(bit_cast<uintptr_t>(&CWeapon::Update),FakeWeaponUpdate), w4e(0x73DB40,FakeWeaponUpdate);
        EntryPatch w5(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),FakeIsMelee), w5e(0x73B1C0,FakeIsMelee);
        EntryPatch m1(bit_cast<uintptr_t>(&CStreaming::GetDefaultCopModel),FakeDefaultCopModel), m1e(0x407C00,FakeDefaultCopModel);
        EntryPatch e1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterRef), e1e(0x571B70,FakeRegisterRef);
        EntryPatch e2(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUpRef), e2e(0x571A00,FakeCleanUpRef);
        EntryPatch e3(bit_cast<uintptr_t>(&CEntity::GetIsOnScreen),FakeOnScreen), e3e(0x534540,FakeOnScreen);
        EntryPatch e4(bit_cast<uintptr_t>(&CPed::Say),FakeSay), e4e(0x5EFFE0,FakeSay);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPed), p1e(0x56E210,FakeFindPed);
        EntryPatch p2(bit_cast<uintptr_t>(&FindPlayerWanted),FakeFindWanted), p2e(0x56E230,FakeFindWanted);
        EntryPatch p3(bit_cast<uintptr_t>(static_cast<void(CWanted::*)(CCopPed*)>(&CWanted::RemovePursuitCop)),FakeRemovePursuit), p3e(0x562C10,FakeRemovePursuit);
        EntryPatch p4(bit_cast<uintptr_t>(&CPedIntelligence::SetPedDecisionMakerType),FakeSetDm), p4e(0x600B50,FakeSetDm);
        EntryPatch p5(bit_cast<uintptr_t>(&CWorld::ProcessLineOfSight),FakeLos), p5e(0x56BA00,FakeLos);
        EntryPatch g1(bit_cast<uintptr_t>(&CGrassRenderer::DrawTriPlants),FakeDrawTriPlants), g1e(0x5DAD00,FakeDrawTriPlants), g2(0x74DBC0,FakeMatSetTexture);
        EntryPatch t1(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), t1e(0x7316A0,FakePushTxd);
        EntryPatch t2(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), t2e(0x7316B0,FakePopTxd);
        EntryPatch t3(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), t3e(0x7319C0,FakeSetTxd);
        EntryPatch t4(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FakeFindTxd), t4e(0x731850,FakeFindTxd);
        EntryPatch rw1(0x7F3AC0,FakeTexRead), rw2(0x7F3820,FakeTexDestroy), rw3(0x7FB020,FakeRasterDestroy), rw4(0x7EF450,FakeIm3DTransform), rw5(0x7EF550,FakeIm3DRender);
        EntryPatch rw6(0x7EF520,FakeIm3DEnd), rw7(0x7F0990,FakeFrameLTM);
        EntryPatch l1(bit_cast<uintptr_t>(&LittleTest),FakeLittleTest), l1e(0x541330,FakeLittleTest);
        EntryPatch v1(bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float)>(&CCamera::IsSphereVisible)),FakeSphereVisible), v1e(0x420D40,FakeSphereVisible);
        EntryPatch x1(bit_cast<uintptr_t>(&C3dMarkers::Init),FakeSub<0>), x1e(0x724E40,FakeSub<0>), x2(bit_cast<uintptr_t>(&CCheckpoints::Init),FakeSub<1>), x2e(0x722880,FakeSub<1>);
        EntryPatch x3(bit_cast<uintptr_t>(&C3dMarkers::Update),FakeSub<2>), x3e(0x7227B0,FakeSub<2>), x4(bit_cast<uintptr_t>(&CCheckpoints::Update),FakeSub<3>), x4e(0x7229C0,FakeSub<3>);
        EntryPatch x5(bit_cast<uintptr_t>(&CBulletTraces::Update),FakeSub<4>), x5e(0x723FB0,FakeSub<4>), x6(bit_cast<uintptr_t>(&C3dMarkers::Render),FakeSub<5>), x6e(0x725040,FakeSub<5>);
        EntryPatch x7(bit_cast<uintptr_t>(&CCheckpoints::Render),FakeSub<6>), x7e(0x726060,FakeSub<6>), x8(bit_cast<uintptr_t>(&CBulletTraces::Render),FakeSub<7>), x8e(0x723C10,FakeSub<7>);
        EntryPatch x9(bit_cast<uintptr_t>(&CMotionBlurStreaks::Render),FakeSub<8>), x9e(0x7240E0,FakeSub<8>), x10(bit_cast<uintptr_t>(&CBrightLights::Render),FakeSub<9>), x10e(0x7241C0,FakeSub<9>);
        EntryPatch x11(bit_cast<uintptr_t>(&CShinyTexts::Render),FakeSub<10>), x11e(0x724890,FakeSub<10>), x12(bit_cast<uintptr_t>(&C3dMarkers::Shutdown),FakeSub<11>), x12e(0x722710,FakeSub<11>);
        EntryPatch x13(bit_cast<uintptr_t>(&CRegisteredMotionBlurStreak::Update),FakeStreakUpdate), x13e(0x721EE0,FakeStreakUpdate);
        EntryPatch x14(bit_cast<uintptr_t>(&CMotionBlurStreaks::RegisterStreak),FakeRegisterStreak), x14e(0x721DC0,FakeRegisterStreak);
        EntryPatch f1(bit_cast<uintptr_t>(&CFont::SetScale),FakeSetScale), f1e(0x719380,FakeSetScale), f2(bit_cast<uintptr_t>(&CFont::SetBackground),FakeSetBackground), f2e(0x7195C0,FakeSetBackground);
        EntryPatch f3(bit_cast<uintptr_t>(&CFont::SetProportional),FakeSetProportional), f3e(0x7195B0,FakeSetProportional), f4(bit_cast<uintptr_t>(&CFont::SetOrientation),FakeSetOrientation), f4e(0x719610,FakeSetOrientation);
        EntryPatch f5(bit_cast<uintptr_t>(&CFont::SetColor),FakeSetColor), f5e(0x719430,FakeSetColor), f6(bit_cast<uintptr_t>(&CFont::SetFontStyle),FakeSetFontStyle), f6e(0x719490,FakeSetFontStyle);
        EntryPatch f7(bit_cast<uintptr_t>(&CFont::PrintString),FakePrintString), f7e(0x71A700,FakePrintString), f8(bit_cast<uintptr_t>(&AsciiToGxtChar),FakeAsciiToGxt), f8e(0x718600,FakeAsciiToGxt);
        EntryPatch f9(bit_cast<uintptr_t>(&CSprite2d::Draw2DPolygon),FakePolygon), f9e(0x7285B0,FakePolygon);
        EntryPatch f10(bit_cast<uintptr_t>(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),FakeDrawRect), f10e(0x727B60,FakeDrawRect);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xD47Au; if(!st)st=1; trace=json::array();
                    rngState=next(); maskA=next(); maskB=next(); callsA=callsB=0; postpone=0; onScreen=false; losEntity=0; ctorTarget=0; playerRet=0; defaultCopModel=280; findTxdRet=int32(next()%40);
                    restoreGlobals();
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto& timeMs=*reinterpret_cast<uint32*>(TIMER); timeMs=next()%3 ? 100000+next()%1000000 : next();

                    if(cls=="CCopPed") {
                        for(auto& c:cops)for(auto& b:c)b=uint8(next());
                        for(auto& p:peds)for(auto& b:p)b=uint8(next());
                        for(auto& v:vehs)for(auto& b:v)b=uint8(next());
                        for(auto& b:intel)b=uint8(next()); for(auto& b:wanted)b=uint8(next());
                        auto* c=cops[0].data();
                        auto pickPed=[&]()->void*{ const uint32 m=next()%8; return m<6 ? peds[m].data() : nullptr; };
                        auto setupList=[&](uint8* cop){ for(int k=0;k<5;++k)At<void*>(cop,0x7AC+4*k)=next()%3 ? pickPed() : nullptr; };
                        auto setupPed=[&](uint8* p){ static const int32 types[]{0,1,4,5,6,7,8,12,18,19,20,23,24,25,31,32,40};
                            At<int32>(p,P_TYPE)=next()%3 ? int32(4+next()%4) : types[next()%std::size(types)]; p[P_CREATED]=uint8(next()%4==0 ? 2 : next()%4);
                            const uint32 h=next()%6; At<float>(p,P_HEALTH)=h==0 ? 0.f : h==1 ? -uni(0.f,50.f) : uni(1.f,200.f);
                            At<void*>(p,P_VEH)=next()%2 ? vehs[next()%2].data() : nullptr; };
                        for(auto& p:peds)setupPed(p.data());
                        for(auto& cp:cops){ At<uintptr_t>(cp.data(),0)=COP_VT; setupList(cp.data()); At<void*>(cp.data(),0x7A8)=nullptr; At<void*>(cp.data(),P_INTEL)=intel.data(); } // ProcessControl lee la inteligencia (0x5DE177)
                        if(nm=="Constructor"){
                            std::memset(c,0xCD,COP_SIZE); ctorTarget=next()%2 ? uintptr_t(ent.data()) : 0;
                            static const uint32 types[]{0,1,2,3,4,5,6,7,8,280,284,285,286,287,0x7FFFFFFF};
                            const uint32 t=next()%4 ? next()%8 : types[next()%std::size(types)]; defaultCopModel=int32(280+next()%4);
                            ret=Name(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(a)(c,t)); out["type"]=t;
                        } else if(nm=="Destructor"){
                            At<void*>(c,0x7A8)=next()%2 ? cops[1].data() : nullptr; playerRet=next()%4 ? uintptr_t(peds[5].data()) : 0;
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(c);
                        } else if(nm=="SetPartner"){
                            At<void*>(c,0x7A8)=next()%2 ? cops[1].data() : nullptr; void* np=next()%3 ? cops[next()%3].data() : nullptr;
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(c,np);
                        } else if(nm=="AddCriminalToKill"){
                            if(next()%3==0){ At<void*>(c,0x7A8)=cops[1].data(); if(next()%3==0)At<void*>(cops[1].data(),0x7A8)=next()%2 ? c : cops[2].data(); }
                            if(next()%5==0)for(int k=0;k<5;++k)At<void*>(c,0x7AC+4*k)=peds[next()%5].data();
                            void* crim=next()%10 ? peds[next()%6].data() : nullptr; if(crim&&next()%3==0)At<float>(crim,P_HEALTH)=uni(10.f,100.f);
                            ret=reinterpret_cast<int32(__thiscall*)(void*,void*)>(a)(c,crim);
                        } else if(nm=="RemoveCriminalToKill"){
                            const int32 unk=int32(next()), idx=int32(next()%5);
                            reinterpret_cast<void(__thiscall*)(void*,int32,int32)>(a)(c,unk,idx);
                        } else if(nm=="ClearCriminalsToKill"){
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(c);
                        } else {
                            At<int32>(wanted.data(),0x2C)=next()%2 ? 0 : int32(1+next()%6);
                            At<int32>(intel.data(),0xB4)=next()%2 ? 7 : int32(next()%20);
                            postpone=next()%3; At<uint32>(c,0x1C)=next();
                            { static const int32 states[]{1,54,55,0,55,54,7}; At<int32>(c,P_STATE)=states[next()%7]; }
                            c[P_SLOT]=uint8(next()%13); At<void*>(c,P_TARGET)=next()%2 ? ent.data() : nullptr;
                            c[0x79D]=uint8(next()%3==0 ? 1+next()%2 : 0); onScreen=next()%2;
                            At<uint32>(reinterpret_cast<void*>(FRAMES),0)=next(); At<uint16>(c,0x20)=uint16(next());
                            if(next()%2)At<uint16>(c,0x20)=uint16((17u-*reinterpret_cast<uint32*>(FRAMES))&0x1F | (next()&0xFFE0));
                            losEntity=next()%2 ? uintptr_t(ent.data()) : 0;
                            At<CVector>(c,4)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)}; At<CVector>(vehs[1].data(),0x30)=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                            if(next()%2){ At<void*>(c,0x14)=nullptr; } else { At<void*>(c,0x14)=vehs[1].data(); }
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(c);
                        }
                        out["vt"]=Name(At<uint32>(c,0)); out["cop"]=Region(c,4,COP_SIZE); out["cop1"]=Region(cops[1].data(),0x790,COP_SIZE);
                        { json ps=json::array(); for(auto& p:peds)ps.push_back({At<uint32>(p.data(),0x470),At<uint32>(p.data(),0x478),At<uint32>(p.data(),0x54C),At<uint32>(p.data(),0x790)}); out["peds"]=ps; }
                        { json vs=json::array(); for(auto& v:vehs)vs.push_back({At<uint16>(v.data(),0x4A6),v[0x42C]}); out["vehs"]=vs; }
                        out["intel"]=Hash(intel.data(),intel.size());
                    }
                    else if(cls=="CPedClothesDesc") {
                        auto* d=clothes.data(); for(auto& b:clothes)b=uint8(next());
                        const uint32 keys[]{0,CKeyGen::GetUppercaseKey("balaclava"),CKeyGen::GetUppercaseKey("head"),CKeyGen::GetUppercaseKey("afro"),next()};
                        for(int k=0;k<10;++k)if(next()%2)At<uint32>(d,4*k)=keys[next()%5];
                        for(int k=0;k<18;++k)At<uint32>(d,0x28+4*k)=next()%4 ? 0 : next();
                        if(nm=="Constructor")ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(a)(d));
                        else if(nm=="Initialise")reinterpret_cast<void(__thiscall*)(void*)>(a)(d);
                        else if(nm=="GetIsWearingBalaclava"||nm=="HasVisibleTattoo")ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(d);
                        else if(nm=="HasVisibleNewHairCut"){ const int32 t=next()%2 ? 1 : int32(next()%4)-1; ret=reinterpret_cast<bool(__thiscall*)(void*,int32)>(a)(d,t); }
                        else if(nm=="SetModel-modelHash")reinterpret_cast<void(__thiscall*)(void*,uint32,int32)>(a)(d,next(),int32(next()%10));
                        else { static const char* names[]{"balaclava","head","afro","player_torso","Hat","",nullptr,"glasses01","BALACLAVA"};
                            reinterpret_cast<void(__thiscall*)(void*,const char*,int32)>(a)(d,names[next()%std::size(names)],int32(next()%10)); }
                        out["desc"]=ReversingProbe::Bytes(d,0x80);
                    }
                    else if(cls=="CPedList") {
                        for(auto& b:list1)b=uint8(next()); for(auto& b:list2)b=uint8(next()); for(auto& b:membership)b=uint8(next());
                        for(auto& p:peds){ for(int k=0;k<0x20;++k)p[P_SLOT-8+k]=uint8(next()); p[P_SLOT]=uint8(next()%13); }
                        auto fill=[&](uint8* l,uint32 cnt){ At<uint32>(l,0)=cnt; for(uint32 k=0;k<30;++k)At<void*>(l,4+4*k)=k<cnt ? peds[next()%6].data() : (next()%2 ? peds[next()%6].data() : nullptr); };
                        if(nm=="Empty"){ fill(list1.data(),next()%31); reinterpret_cast<void(__thiscall*)(void*)>(a)(list1.data()); }
                        else if(nm=="BuildListFromGroup_NoLeader"){ for(int k=0;k<8;++k)At<void*>(membership.data(),4+4*k)=next()%3 ? peds[next()%6].data() : nullptr;
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(list1.data(),membership.data()); }
                        else { fill(list1.data(),next()%4==0 ? 25+next()%6 : next()%10); fill(list2.data(),next()%31);
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(list1.data(),list2.data()); }
                        out["list1"]=Region(list1.data(),0,0x7C); out["list2"]=Region(list2.data(),0,0x7C);
                    }
                    else if(cls=="CPPTriPlantBuffer") {
                        auto* b=plants.data(); for(auto& x:plants)x=uint8(next());
                        const int32 counts[]{0,1,5,16,31,32,33,-1,-5}; At<int32>(b,0)=next()%2 ? int32(next()%33) : counts[next()%9];
                        auto& set=At<int32>(b,0xB84); set=next()%4 ? int32(next()%4) : int32(next()%8)-2;
                        for(int k=0;k<4;++k)At<uintptr_t>(b,0xB88+4*k)=next()%4 ? 0x7000+k*0x10+next()%8 : 0;
                        if(nm=="Flush")reinterpret_cast<void(__thiscall*)(void*)>(a)(b);
                        else if(nm=="GetPPTriPlantPtr"){ const int32 n=next()%4 ? int32(next()%33) : int32(next()%8)-4; ret=Name(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(b,n)); }
                        else if(nm=="ChangeCurrentPlantModelsSet"){ const int32 s=next()%2 ? set : int32(next()%6)-1; reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(b,s); }
                        else if(nm=="IncreaseBufferIndex"){ const int32 s=next()%3 ? set : int32(next()%6)-1, n=next()%4 ? int32(next()%34) : int32(next()%8)-4;
                            reinterpret_cast<void(__thiscall*)(void*,int32,int32)>(a)(b,s,n); }
                        else if(nm=="GetPlantModelsTab"){ const uint32 k=next()%4 ? next()%4 : next(); ret=Name(reinterpret_cast<void*(__thiscall*)(void*,uint32)>(a)(b,k)); }
                        else if(nm=="SetPlantModelsTab"){ const uint32 k=next()%4 ? next()%4 : next()%10; ret=reinterpret_cast<bool(__thiscall*)(void*,uint32,uintptr_t)>(a)(b,k,0x7800+next()%64); }
                        else { for(auto& x:material)x=uint8(next()); for(auto& x:rgba)x=uint8(next());
                            At<uintptr_t>(reinterpret_cast<void*>(GRASS_TEX),0)=next()%2 ? uintptr_t(texture.data()) : 0x7400+next()%16;
                            if(next()%3==0)At<uintptr_t>(material.data(),0)=At<uintptr_t>(reinterpret_cast<void*>(GRASS_TEX),0);
                            ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(material.data(),rgba.data())); out["material"]=Region(material.data(),0,0x10); }
                        out["plants"]=Hash(b,plants.size()); out["head"]=At<int32>(b,0); out["set"]=At<int32>(b,0xB84); out["tabs"]=Region(b,0xB88,0xB98);
                    }
                    else if(cls=="CSkidmarks") {
                        auto* sk=reinterpret_cast<uint8*>(SKID); for(size_t k=0;k<SKID_N;++k)sk[k]=uint8(next());
                        auto* tv=reinterpret_cast<uint8*>(TEMPV); for(size_t k=0;k<0x24*64;++k)tv[k]=uint8(next());
                        *reinterpret_cast<float*>(DN)=next()%4 ? uni(0.f,1.f) : float(next()%2);
                        *reinterpret_cast<uint8*>(REPLAY)=uint8(next()%8==0 ? 1 : next()%8==0 ? 2 : 0);
                        At<void*>(reinterpret_cast<void*>(SKID_TEX),0)=texture.data(); At<void*>(texture.data(),0)=raster.data();
                        const uint32 ids[]{0x1000,0x2000,0x3000,0x4000};
                        for(uint32 m=0;m<32;++m){ auto* mk=sk+m*0x158;
                            for(int k=0;k<16;++k){ At<CVector>(mk,12*k)=CVector{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,50.f)}; At<float>(mk,0xC0+4*k)=uni(-1.f,1.f); At<float>(mk,0x100+4*k)=uni(-1.f,1.f); }
                            At<uint32>(mk,0x140)=next()%2 ? ids[next()%4] : next(); At<uint32>(mk,0x144)=timeMs-next()%400;
                            At<uint32>(mk,0x148)=timeMs-2000+next()%4000; At<uint32>(mk,0x14C)=At<uint32>(mk,0x148)+1+next()%6000;
                            At<uint32>(mk,0x150)=next()%4; At<uint16>(mk,0x154)=uint16(next()%16); mk[0x156]=uint8(next()%3); mk[0x157]=uint8(next()%2);
                            if(nm=="Render"&&mk[0x156]==2&&next()%2)At<uint32>(mk,0x14C)=timeMs-next()%100;
                            if(At<uint32>(mk,0x14C)==At<uint32>(mk,0x148))++At<uint32>(mk,0x14C); } // sin division por cero (el EXE se cae)
                        if(nm=="RegisterOne-"||nm=="RegisterOne-EC0"){ const uint32 mode=next()%3; for(uint32 m=0;m<32;++m){ auto* mk=sk+m*0x158;
                            if(mode>=1&&mk[0x156]==0)mk[0x156]=uint8(1+next()%2); if(mode==2&&next()%2)mk[0x157]=1; } }
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Clear")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Update")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Render"){ rsSlot=reinterpret_cast<void*>(&FakeRenderState); reinterpret_cast<void(__cdecl*)()>(a)(); rsSlot=rsSaved; }
                        else { const CVector pos{uni(-2000.f,2000.f),uni(-2000.f,2000.f),uni(0.f,50.f)}; const uint32 id=next()%2 ? ids[next()%4] : next();
                            float dx=uni(-1.f,1.f), dy=uni(-1.f,1.f); if(next()%8==0){dx=0.f;dy=0.f;} const float len=uni(0.1f,3.f);
                            uint8 blood=uint8(next()%2), sandy=uint8(next()%2);
                            if(nm=="RegisterOne-")reinterpret_cast<void(__cdecl*)(uint32,const CVector&,float,float,uint32,uint8*,float)>(a)(id,pos,dx,dy,next()%4,&blood,len);
                            else reinterpret_cast<void(__cdecl*)(uint32,const CVector&,float,float,uint8*,uint8*,float)>(a)(id,pos,dx,dy,&sandy,&blood,len);
                            out["blood"]=blood; out["sandy"]=sandy; }
                        out["skid"]=Hash(sk,SKID_N); { json h=json::array(); for(uint32 m=0;m<32;++m)h.push_back(Hash(sk+m*0x158,0x158)); out["marks"]=h; }
                        out["idx"]=Hash(reinterpret_cast<void*>(SKID_IDX),0xC0); out["tex"]=Name(At<uint32>(reinterpret_cast<void*>(SKID_TEX),0)); out["tempv"]=Hash(tv,0x24*64);
                    }
                    else if(cls=="CSpecialFX") {
                        auto* fa=reinterpret_cast<uint8*>(FX_A); auto* fb=reinterpret_cast<uint8*>(FX_B);
                        for(size_t k=0;k<FX_A_N;++k)fa[k]=uint8(next()); for(size_t k=0;k<FX_B_N;++k)fb[k]=uint8(next());
                        auto& flagTex=At<uintptr_t>(reinterpret_cast<void*>(0xC7C718),0); flagTex=next()%2 ? uintptr_t(texture.data()) : 0;
                        At<uintptr_t>(reinterpret_cast<void*>(0xC7C71C),0)=next()%2 ? 0x7100+next()%16 : 0; At<uintptr_t>(reinterpret_cast<void*>(0xC7C720),0)=next()%2 ? 0x7200+next()%16 : 0;
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Update"){ for(int k=0;k<4;++k)At<uint32>(reinterpret_cast<void*>(0xC7CA08+0x54*k),0)=next()%2 ? next() : 0; reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Shutdown"||nm=="Render")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="AddWeaponStreak"){
                            auto* guard=reinterpret_cast<uint32*>(0xC80430); *guard=1|(next()&~1u); auto* mat=reinterpret_cast<uint8*>(0xC803E8);
                            for(int k=0;k<0x40;++k)mat[k]=uint8(next()); At<uint32>(mat,0x40)=0; At<uint32>(mat,0x44)=0;
                            for(auto& p:peds[0])p=uint8(next()); for(auto& x:weaponObj)x=uint8(next()); for(auto& x:rwMat)x=uint8(next());
                            for(int k=0;k<16;++k)if(k%4!=3)At<float>(rwMat.data(),4*k)=k>=12 ? uni(-3000.f,3000.f) : uni(-1.f,1.f);
                            At<void*>(peds[0].data(),0x4F4)=next()%6 ? weaponObj.data() : nullptr; At<uintptr_t>(weaponObj.data(),4)=0x7300+next()%16;
                            playerRet=next()%8 ? uintptr_t(peds[0].data()) : 0;
                            static const int32 types[]{5,2,8,5,2,8,0,1,3,4,6,7,9,22,46};
                            reinterpret_cast<void(__cdecl*)(int32)>(a)(types[next()%std::size(types)]);
                            { auto m=ReversingProbe::Bytes(mat,0x48); for(size_t o:{12u,28u,44u,60u})std::fill_n(m.begin()+o,4,uint8(0)); out["mat"]=m; } // relleno: el EXE copia con rep movs basura de la pila del temporal out["guard"]=*guard;
                        }
                        else if(nm=="Render2DFXs"){
                            static const int32 ws[]{640,800,1024,1920,0,3}, hs[]{480,600,768,1080,0,5};
                            if(next()%3){ At<int32>(reinterpret_cast<void*>(RS_W),0)=ws[next()%6]; At<int32>(reinterpret_cast<void*>(RS_H),0)=hs[next()%6]; }
                            if(At<int32>(reinterpret_cast<void*>(RS_W),0)==0)At<int32>(reinterpret_cast<void*>(RS_W),0)=320;
                            if(At<int32>(reinterpret_cast<void*>(RS_H),0)==0)At<int32>(reinterpret_cast<void*>(RS_H),0)=240;
                            fa[0x10C]=uint8(next()%3==0); fa[0x10D]=uint8(next()%3==0); fa[0x114]=uint8(next()%2); At<uint32>(fa,0x110)=next()%25;
                            *reinterpret_cast<uint32*>(FRAMES)=next(); *reinterpret_cast<float*>(TIMESCALE)=uni(0.f,2.f);
                            rsSlot=reinterpret_cast<void*>(&FakeRenderState); reinterpret_cast<void(__cdecl*)()>(a)(); rsSlot=rsSaved;
                            out["gstring"]=ReversingProbe::Bytes(reinterpret_cast<void*>(GSTRING),16); out["timescale"]=F(*reinterpret_cast<float*>(TIMESCALE));
                            out["cam"]={*reinterpret_cast<uint32*>(0xB6F24C+camIdx*0x238u),*reinterpret_cast<uint32*>(0xB6F25C+camIdx*0x238u)};
                        }
                        else { fa[0x114]=uint8(next()%2); *reinterpret_cast<float*>(TIMESCALE)=uni(0.f,2.f); reinterpret_cast<void(__cdecl*)()>(a)(); out["timescale"]=F(*reinterpret_cast<float*>(TIMESCALE)); }
                        out["fx"]=ReversingProbe::Bytes(fa,FX_A_N); out["fx_markers"]=Hash(fb,FX_B_N);
                    }
                    else if(cls=="CStreamingInfo") {
                        for(auto& x:infos)x=uint8(next()); At<void*>(reinterpret_cast<void*>(ARRAY_BASE),0)=InfoBase();
                        auto idx=[&]{ return int16(next()%8==0 ? -1 : int16(next()%NSI)); };
                        for(size_t k=0;k<NSI;++k){ auto* e=InfoBase()+SI*k; At<int16>(e,0)=idx(); At<int16>(e,2)=idx(); e[7]=uint8(next()%8); }
                        auto* e=InfoBase()+SI*(next()%NSI);
                        if(nm=="Init")reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        else if(nm=="AddToList"){ auto* after=InfoBase()+SI*(next()%NSI); reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(e,after); }
                        else if(nm=="GetCdPosn"){ if(next()%2)At<uint32>(e,8)=next()%0x1000000; ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(e); }
                        else if(nm=="SetCdPosnAndSize")reinterpret_cast<void(__thiscall*)(void*,uint32,uint32)>(a)(e,next(),next());
                        else if(nm=="GetCdPosnAndSize"){ if(next()%3==0)At<uint32>(e,0xC)=0; uint32 pos=0x5A5A5A5A, size=0xA5A5A5A5;
                            ret=reinterpret_cast<bool(__thiscall*)(void*,uint32*,uint32*)>(a)(e,&pos,&size); out["pos"]=pos; out["size"]=size; }
                        else if(nm=="InList"){ if(next()%2)At<int16>(e,0)=-1; ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(e); }
                        else reinterpret_cast<void(__thiscall*)(void*)>(a)(e);
                        out["infos"]=Hash(infos.data(),infos.size()); out["elem"]=ReversingProbe::Bytes(e,SI);
                    }
                    for(auto* h:hooks)h->State(false);
                    rsSlot=rsSaved; restoreGlobals();
                    out["ret"]=ret; out["trace"]=trace; return out;
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
    restoreGlobals();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",globalsIntact()}});
}
}
