#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Hud.h"
#include "Wanted.h"
#include "Text/Text.h"
#include "Sprite2d.h"
#include "Entity/Ped/PlayerPed.h"
#include "PedIntelligence.h"
#include "Stats.h"
#include "WeaponInfo.h"
#include "MenuManager.h"
#include "Sprite.h"
#include "TaskSimpleUseGun.h"
#include "TxdStore.h"
#include "Font.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace HudCrosshairs355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline json trace;inline int scenario{};alignas(16) inline std::array<uint8,sizeof(CWanted)> wantedBytes{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelligence{};
alignas(16) inline std::array<uint8,sizeof(CVehicle)> vehicle{};alignas(16) inline std::array<uint8,0xAC> data{};alignas(16) inline std::array<uint8,sizeof(CTaskSimpleUseGun)> gun{};inline CWeaponInfo weaponInfo;inline RwTexture texture;inline TxdDef txd;alignas(16) inline std::array<uint8,20> pool{};inline std::array<uint8,1> poolFlags{};alignas(16) inline std::array<uint8,0x20> model{};
inline CVehicle* __cdecl Vehicle(int id,bool remote){trace.push_back({"vehicle",id,remote});return (scenario&64)?nullptr:reinterpret_cast<CVehicle*>(vehicle.data());}
inline bool __fastcall Melee(CWeapon*,void*){trace.push_back({"melee",bool(scenario&16)});return (scenario&16)!=0;}
inline CTaskSimpleUseGun* __fastcall Gun(CPedIntelligence*,void*){trace.push_back({"gun",bool(scenario&32)});return (scenario&32)?reinterpret_cast<CTaskSimpleUseGun*>(gun.data()):nullptr;}
inline float __fastcall Radius(CPlayerPed*,void*){float v=(scenario&128)?std::bit_cast<float>(0x3E4CCCCDu):.5f;trace.push_back({"radius",std::bit_cast<uint32>(v)});return v;}
inline CWeaponInfo* __cdecl WeaponInfo(eWeaponType type,eWeaponSkill skill){trace.push_back({"weapon_info",int(type),int(skill)});return &weaponInfo;}
inline RwTexture* __cdecl Texture(RwTexDictionary*,uint32 key){trace.push_back({"texture",key});return (scenario&512)?nullptr:&texture;}
inline void __fastcall Draw(CSprite2d*self,void*,const CRect&r,const CRGBA&c){trace.push_back({"draw",int(self-CHud::Sprites.data()),ReversingProbe::Bytes(&r,16),c.ToInt()});}
inline void __cdecl Rect(const CRect&r,const CRGBA&c){trace.push_back({"rect",ReversingProbe::Bytes(&r,16),c.ToInt()});}
inline void __cdecl Sprite(CVector p,CVector2D size,uint8 r,uint8 g,uint8 b,int16 intensity,float rhw,uint8 a,uint8 u,uint8 v){trace.push_back({"sprite",ReversingProbe::Bytes(&p,12),ReversingProbe::Bytes(&size,8),r,g,b,intensity,std::bit_cast<uint32>(rhw),a,u,v});}
inline RwBool __cdecl RenderState(RwRenderState state,void* value){trace.push_back({"renderstate",int(state),state==rwRENDERSTATETEXTURERASTER?json("raster"):json(uintptr_t(value))});return TRUE;}
inline CPlayerPed* __cdecl Player(int id){trace.push_back({"player",id});return reinterpret_cast<CPlayerPed*>(anchor.data());}
inline CTaskSimpleSwim* __fastcall Swim(CPedIntelligence*,void*){trace.push_back({"swim",bool(scenario&16)});return (scenario&16)?reinterpret_cast<CTaskSimpleSwim*>(targetTokens.data()):nullptr;}
inline float __cdecl Stat(eStats id){trace.push_back({"stat",int(id)});return float((scenario%6)*250);}
inline eStats __cdecl Skill(eWeaponType id){trace.push_back({"skill",int(id)});return static_cast<eStats>(69+scenario%8);}
inline float __cdecl Round(float v){trace.push_back({"round",std::bit_cast<uint32>(v)});return std::floor(v);}
inline void __fastcall Window(CMenuManager*,void*,const CRect& r,const char* key,uint8 color,CRGBA back,bool unused,bool background){trace.push_back({"window",ReversingProbe::Bytes(&r,16),key,color,back.ToInt(),unused,background});}
inline std::array<GxtChar,40> translated{};
inline const GxtChar* __fastcall TextGet(CText*,void*,const char* key){trace.push_back({"text_get",key});std::memset(translated.data(),0,40);std::memcpy(translated.data(),key,std::min<size_t>(std::strlen(key),39));return translated.data();}
inline void __cdecl Wrap(float v){trace.push_back({"wrap",std::bit_cast<uint32>(v)});}
inline void __cdecl RightWrap(float v){trace.push_back({"right_wrap",std::bit_cast<uint32>(v)});}
inline void __cdecl Bar(float x,float y,uint16 width,uint8 height,float progress,int8 add,uint8 percent,uint8 border,CRGBA color,CRGBA extra){trace.push_back({"bar",std::bit_cast<uint32>(x),std::bit_cast<uint32>(y),width,height,std::bit_cast<uint32>(progress),int(add),percent,border,color.ToInt(),extra.ToInt()});}
inline CWanted* __cdecl Wanted(int id){trace.push_back({"wanted",id});return reinterpret_cast<CWanted*>(wantedBytes.data());}
inline float __cdecl HealthY(uint8 id,float y,int8 offset){trace.push_back({"health_y",id,std::bit_cast<uint32>(y),int(offset)});return y-3.f;}
inline void __cdecl Gxt(const char* a,GxtChar* b){trace.push_back({"gxt",a});std::memcpy(b,a,std::strlen(a)+1);}
inline uint32 Bits(float v){return std::bit_cast<uint32>(v);}
inline bool __cdecl Coop(){trace.push_back({"coop",(scenario&4)!=0});return (scenario&4)!=0;}
inline int16 __cdecl Lines(float x,float y,const GxtChar* text){trace.push_back({"lines",Bits(x),Bits(y),ReversingProbe::Bytes(text,std::strlen(reinterpret_cast<const char*>(text))+1)});return int16(1+scenario%3);}
inline void __cdecl Edge(int8 value){trace.push_back({"edge",int(value)});}
inline void __cdecl Justify(bool value){trace.push_back({"justify",value});}
inline void __cdecl Background(bool enabled,bool wrap){trace.push_back({"background",enabled,wrap});}
inline void __cdecl BackgroundColor(CRGBA color){trace.push_back({"background_color",color.ToInt()});}
inline void __cdecl Orientation(eFontAlignment value){trace.push_back({"orientation",int(value)});}
inline void __cdecl Proportional(bool value){trace.push_back({"proportional",value});}
inline void __cdecl DropShadow(int16 value){trace.push_back({"drop_shadow",value});}
inline void __cdecl Style(eFontStyle value){trace.push_back({"style",int(value)});}
inline void __cdecl Color(CRGBA color){trace.push_back({"color",color.ToInt()});}
inline void __cdecl DropColor(CRGBA color){trace.push_back({"drop_color",color.ToInt()});}
inline void __cdecl LanguageScale(float x,float y){trace.push_back({"language_scale",Bits(x),Bits(y)});}
inline void __cdecl Scale(float x,float y){trace.push_back({"scale",Bits(x),Bits(y)});}
inline void __cdecl Centre(float value){trace.push_back({"centre",Bits(value)});}
inline void __cdecl Print(float x,float y,const GxtChar* text){trace.push_back({"print",Bits(x),Bits(y),ReversingProbe::Bytes(text,std::strlen(reinterpret_cast<const char*>(text))+1)});}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json Ptr(const void* p){if(!p)return nullptr;auto a=uintptr_t(p);if(a>=uintptr_t(targetTokens.data())&&a<uintptr_t(targetTokens.data())+sizeof(targetTokens))return json::array({"target",(a-uintptr_t(targetTokens.data()))/4});return a;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CHud","DrawCrossHairs",0x58E020,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CHud")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"hud-crosshairs355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"hud-crosshairs355-runtime.jsonl");ReversingProbe::InstallFailureCapture("hud-crosshairs355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* globals=reinterpret_cast<uint8*>(0xBAA388);std::array<uint8,0xEB0> saved{};std::memcpy(saved.data(),globals,saved.size());auto cameraWide=TheCamera.m_bWideScreenOn;auto cutscene=CCutsceneMgr::ms_running;auto subtitles=StaticRef<bool>(0xBA678C);std::array<uint8,8> formatting{};std::memcpy(formatting.data(),reinterpret_cast<void*>(0xA44B60),8);auto chairXSaved=CCamera::m_f3rdPersonCHairMultX,chairYSaved=CCamera::m_f3rdPersonCHairMultY;auto poolSaved=CTxdStore::ms_pTxdPool;auto modelSaved=StaticRef<void*>(0xA9B0CC);auto scriptSaved=CTheScripts::bDrawCrossHair;std::array<uint8,sizeof(CCamera)> cameraSaved{};std::memcpy(cameraSaved.data(),&TheCamera,cameraSaved.size());auto spritesSaved=CHud::Sprites;auto replaySaved=StaticRef<uint8>(0xA43088);auto daySaved=StaticRef<uint8>(0xB7014E);std::array<uint8,200> stringSaved{};std::memcpy(stringSaved.data(),gString,200);std::array<uint8,0x154> timerSaved{};std::memcpy(timerSaved.data(),reinterpret_cast<void*>(0xBA1788),timerSaved.size());auto disableSaved=StaticRef<bool>(0x96C014);std::array<uint8,800> playersSaved{};std::memcpy(playersSaved.data(),reinterpret_cast<void*>(0xB7CD98),800);auto focusSaved=CWorld::PlayerInFocus;auto frameSaved=CTimer::GetFrameCounter();auto clockSaved=CTimer::GetTimeInMS();const auto timeSaved=CTimer::GetTimeStep();const auto widthSaved=RsGlobal.maximumWidth,heightSaved=RsGlobal.maximumHeight;
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch pl1(0x56E210,Player),pl2(bit_cast<uintptr_t>(&FindPlayerPed),Player),v1(0x56E0D0,Vehicle),v2(bit_cast<uintptr_t>(&FindPlayerVehicle),Vehicle),m1(0x73B1C0,Melee),m2(bit_cast<uintptr_t>(&CWeapon::IsTypeMelee),Melee),g1(0x600F70,Gun),g2(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskUseGun),Gun),r1(0x609CD0,Radius),r2(bit_cast<uintptr_t>(&CPlayerPed::GetWeaponRadiusOnScreen),Radius),wi1(0x743C60,WeaponInfo),wi2(bit_cast<uintptr_t>(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),WeaponInfo),te1(0x734E50,Texture),te2(bit_cast<uintptr_t>(&RwTexDictionaryFindHashNamedTexture),Texture),d1(0x728350,Draw),d2(bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const CRect&,const CRGBA&)>(&CSprite2d::Draw)),Draw),re1(0x727B60,Rect),re2(bit_cast<uintptr_t>(static_cast<void(*)(const CRect&,const CRGBA&)>(&CSprite2d::DrawRect)),Rect),sp1(0x70D000,Sprite),sp2(bit_cast<uintptr_t>(&CSprite::RenderOneXLUSprite),Sprite);
 auto*driver=*reinterpret_cast<uintptr_t**>(0xC97B24);auto renderSaved=driver[8];driver[8]=bit_cast<uintptr_t>(&RenderState);
int pairs=0;bool pass=true;
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(globals,0xCD,saved.size());anchor.fill(0);intelligence.fill(0);vehicle.fill(0);gun.fill(0);data.fill(0);auto*p=reinterpret_cast<CPed*>(anchor.data());*reinterpret_cast<void**>(anchor.data()+0x47C)=intelligence.data();*reinterpret_cast<void**>(anchor.data()+0x480)=data.data();data[0x34]=uint8((i&8)?8:0);gun[0xE]=uint8((i&4)!=0);constexpr int weapons[]{0,22,24,28,32,33,34,37,38,43};*reinterpret_cast<int8*>(anchor.data()+0x718)=0;*reinterpret_cast<int32*>(anchor.data()+0x5A0)=weapons[(i/3)%10];p->m_nPedState=static_cast<ePedState>((i&256)?59:0);p->m_pTargetedObject=nullptr;CWorld::PlayerInFocus=uint8((i/3)%2);*reinterpret_cast<void**>(0xB7CD98+400*CWorld::PlayerInFocus)=anchor.data();reinterpret_cast<CVehicle*>(vehicle.data())->m_nModelIndex=int16((i&1)?520:425);constexpr int modes[]{7,8,16,34,39,40,41,42,45,46,51,52,53,55,65,4};TheCamera.m_nActiveCam=0;TheCamera.GetActiveCam().m_nMode=static_cast<eCamMode>(modes[(i/5)%16]);TheCamera.m_bTransitionState=(i&2)!=0;CTheScripts::bDrawCrossHair=static_cast<eCrossHairType>((i/7)%3);CCamera::m_f3rdPersonCHairMultX=.5f;CCamera::m_f3rdPersonCHairMultY=.5f;std::memset(&weaponInfo,0,sizeof(weaponInfo));weaponInfo.m_nModelId1=(i&128)?0:1;model.fill(0);*reinterpret_cast<uint32*>(model.data()+4)=123;*reinterpret_cast<int16*>(model.data()+10)=0;StaticRef<void*>(0xA9B0CC)=model.data();txd.m_pRwDictionary=(i&256)?nullptr:reinterpret_cast<RwTexDictionary*>(targetTokens.data());pool.fill(0);*reinterpret_cast<void**>(pool.data())=&txd;*reinterpret_cast<void**>(pool.data()+4)=poolFlags.data();*reinterpret_cast<int32*>(pool.data()+8)=1;poolFlags[0]=0;CTxdStore::ms_pTxdPool=reinterpret_cast<CTxdPool*>(pool.data());std::memset(&texture,0,sizeof(texture));texture.raster=reinterpret_cast<RwRaster*>(pedTokens.data());CHud::Sprites[SPRITE_SITE_ROCKET].m_pTexture=&texture;constexpr int widths[]{640,800,1024,1920},heights[]{448,600,768,1080};RsGlobal.maximumWidth=widths[(i/3)%4];RsGlobal.maximumHeight=heights[(i/5)%4];h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)()>(fn.address)();return json{{"timer",ReversingProbe::Bytes(anchor.data(),anchor.size())},{"globals",ReversingProbe::Bytes(globals,saved.size())},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=!a["trace"].empty();yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
std::memcpy(globals,saved.data(),saved.size());TheCamera.m_bWideScreenOn=cameraWide;CCutsceneMgr::ms_running=cutscene;StaticRef<bool>(0xBA678C)=subtitles;std::memcpy(reinterpret_cast<void*>(0xA44B60),formatting.data(),8);CCamera::m_f3rdPersonCHairMultX=chairXSaved;CCamera::m_f3rdPersonCHairMultY=chairYSaved;driver[8]=renderSaved;CTxdStore::ms_pTxdPool=poolSaved;StaticRef<void*>(0xA9B0CC)=modelSaved;CTheScripts::bDrawCrossHair=scriptSaved;std::memcpy(&TheCamera,cameraSaved.data(),cameraSaved.size());CHud::Sprites=spritesSaved;StaticRef<uint8>(0xA43088)=replaySaved;StaticRef<uint8>(0xB7014E)=daySaved;std::memcpy(gString,stringSaved.data(),200);std::memcpy(reinterpret_cast<void*>(0xBA1788),timerSaved.data(),timerSaved.size());StaticRef<bool>(0x96C014)=disableSaved;std::memcpy(reinterpret_cast<void*>(0xB7CD98),playersSaved.data(),800);CWorld::PlayerInFocus=focusSaved;StaticRef<uint32>(0xB7CB4C)=frameSaved;StaticRef<uint32>(0xB7CB84)=clockSaved;StaticRef<float>(0xB7CB5C)=timeSaved;RsGlobal.maximumWidth=widthSaved;RsGlobal.maximumHeight=heightSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
