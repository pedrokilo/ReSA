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
#include "Font.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace HudPlayerInfo355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline json trace;inline int scenario{};alignas(16) inline std::array<uint8,sizeof(CWanted)> wantedBytes{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intelligence{};
inline CPlayerPed* __cdecl Player(int id){trace.push_back({"player",id});return reinterpret_cast<CPlayerPed*>(anchor.data());}
inline CTaskSimpleSwim* __fastcall Swim(CPedIntelligence*,void*){trace.push_back({"swim",bool(scenario&16)});return (scenario&16)?reinterpret_cast<CTaskSimpleSwim*>(targetTokens.data()):nullptr;}
inline float __cdecl Stat(eStats id){trace.push_back({"stat",int(id)});return float((scenario%6)*250);}
inline eStats __cdecl Skill(eWeaponType id){trace.push_back({"skill",int(id)});return static_cast<eStats>(69+scenario%8);}
inline float __cdecl Round(float v){trace.push_back({"round",std::bit_cast<uint32>(v)});return std::floor(v);}
inline void __fastcall Window(CMenuManager*,void*,const CRect& r,const char* key,uint8 color,CRGBA back,bool unused,bool background){trace.push_back({"window",ReversingProbe::Bytes(&r,16),key,color,back.ToInt(),unused,background});}
alignas(16) inline std::array<uint8,sizeof(CPed)> secondPed{};alignas(16) inline std::array<uint8,sizeof(CVehicle)> vehicle{};alignas(16) inline std::array<uint8,0xAC> data{},otherData{};alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> otherIntel{};
inline float __cdecl Modifier(eStatModAbilities id){trace.push_back({"modifier",int(id)});return 1000.f;}
inline void __cdecl Health(int id,int x,int y){trace.push_back({"health",id,x,y});}
inline void __cdecl Armor(int id,int x,int y){trace.push_back({"armor",id,x,y});}
inline void __cdecl Breath(int id,int x,int y){trace.push_back({"breath",id,x,y});}
inline void __cdecl WeaponIcon(CPed*p,int x,int y,float a){trace.push_back({"weapon_icon",p==reinterpret_cast<CPed*>(anchor.data())?0:1,x,y,std::bit_cast<uint32>(a)});}
inline void __cdecl Ammo(CPed*p,int x,int y,float a){trace.push_back({"ammo",p==reinterpret_cast<CPed*>(anchor.data())?0:1,x,y,std::bit_cast<uint32>(a)});}
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
{"CHud","DrawPlayerInfo",0x58EAF0,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CHud")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"hud-player-info355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"hud-player-info355-runtime.jsonl");ReversingProbe::InstallFailureCapture("hud-player-info355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* globals=reinterpret_cast<uint8*>(0xBAA388);std::array<uint8,0xEB0> saved{};std::memcpy(saved.data(),globals,saved.size());auto cameraWide=TheCamera.m_bWideScreenOn;auto cutscene=CCutsceneMgr::ms_running;auto subtitles=StaticRef<bool>(0xBA678C);std::array<uint8,8> formatting{};std::memcpy(formatting.data(),reinterpret_cast<void*>(0xA44B60),8);auto hoursSaved=StaticRef<uint8>(0xB70153),minutesSaved=StaticRef<uint8>(0xB70152);auto coopPedSaved=StaticRef<CPlayerPed*>(0xB7CF28);auto replaySaved=StaticRef<uint8>(0xA43088);auto daySaved=StaticRef<uint8>(0xB7014E);std::array<uint8,200> stringSaved{};std::memcpy(stringSaved.data(),gString,200);std::array<uint8,0x154> timerSaved{};std::memcpy(timerSaved.data(),reinterpret_cast<void*>(0xBA1788),timerSaved.size());auto disableSaved=StaticRef<bool>(0x96C014);std::array<uint8,800> playersSaved{};std::memcpy(playersSaved.data(),reinterpret_cast<void*>(0xB7CD98),800);auto focusSaved=CWorld::PlayerInFocus;auto frameSaved=CTimer::GetFrameCounter();auto clockSaved=CTimer::GetTimeInMS();const auto timeSaved=CTimer::GetTimeStep();const auto widthSaved=RsGlobal.maximumWidth,heightSaved=RsGlobal.maximumHeight;
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch b1(0x7195C0,Background),b2(bit_cast<uintptr_t>(&CFont::SetBackground),Background),o1(0x719610,Orientation),o2(bit_cast<uintptr_t>(&CFont::SetOrientation),Orientation),p1(0x7195B0,Proportional),p2(bit_cast<uintptr_t>(&CFont::SetProportional),Proportional),s1(0x719490,Style),s2(bit_cast<uintptr_t>(&CFont::SetFontStyle),Style),co1(0x719430,Color),co2(bit_cast<uintptr_t>(&CFont::SetColor),Color),dc1(0x719510,DropColor),dc2(bit_cast<uintptr_t>(&CFont::SetDropColor),DropColor),sc1(0x719380,Scale),sc2(bit_cast<uintptr_t>(&CFont::SetScale),Scale),ce1(0x7194E0,Centre),ce2(bit_cast<uintptr_t>(&CFont::SetCentreSize),Centre),pr1(0x71A700,Print),pr2(bit_cast<uintptr_t>(&CFont::PrintString),Print),j1(0x719600,Justify),j2(bit_cast<uintptr_t>(&CFont::SetJustify),Justify),e1(0x719590,Edge),e2(bit_cast<uintptr_t>(&CFont::SetEdge),Edge),l1(0x71A5E0,Lines),l2(bit_cast<uintptr_t>(&CFont::GetNumberLines),Lines),w1(0x56E230,Wanted),w2(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted),a1(0x718600,Gxt),a2(bit_cast<uintptr_t>(&AsciiToGxtChar),Gxt),hy1(0x588B60,HealthY),hy2(bit_cast<uintptr_t>(&CHud::GetYPosBasedOnHealth),HealthY),tg1(0x6A0050,TextGet),tg2(bit_cast<uintptr_t>(&CText::Get),TextGet),wr1(0x7194D0,Wrap),wr2(bit_cast<uintptr_t>(&CFont::SetWrapx),Wrap),rw1(0x7194F0,RightWrap),rw2(bit_cast<uintptr_t>(&CFont::SetRightJustifyWrap),RightWrap),bar1(0x728640,Bar),bar2(bit_cast<uintptr_t>(&CSprite2d::DrawBarChart),Bar),pl1(0x56E210,Player),pl2(bit_cast<uintptr_t>(&FindPlayerPed),Player),sw1(0x601070,Swim),sw2(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),Swim),st1(0x558E40,Stat),st2(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),Stat),sk1(0x743CD0,Skill),sk2(bit_cast<uintptr_t>(&CWeaponInfo::GetSkillStatIndex),Skill),ro1(0x406CE0,Round),win1(0x573EE0,Window),win2(bit_cast<uintptr_t>(&CMenuManager::DrawWindow),Window),mo1(0x559AF0,Modifier),mo2(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier),Modifier),he1(0x589270,Health),he2(bit_cast<uintptr_t>(&CHud::RenderHealthBar),Health),ar1(0x5890A0,Armor),ar2(bit_cast<uintptr_t>(&CHud::RenderArmorBar),Armor),br1(0x589190,Breath),br2(bit_cast<uintptr_t>(&CHud::RenderBreathBar),Breath),wi1(0x58D7D0,WeaponIcon),wi2(bit_cast<uintptr_t>(&CHud::DrawWeaponIcon),WeaponIcon),am1(0x5893B0,Ammo),am2(bit_cast<uintptr_t>(&CHud::DrawAmmo),Ammo);
int pairs=0;bool pass=true;
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(globals,0xCD,saved.size());anchor.fill(0);secondPed.fill(0);intelligence.fill(0);otherIntel.fill(0);vehicle.fill(0);data.fill(0);otherData.fill(0);auto*p=reinterpret_cast<CPed*>(anchor.data());auto*q=reinterpret_cast<CPed*>(secondPed.data());*reinterpret_cast<void**>(anchor.data()+0x47C)=intelligence.data();*reinterpret_cast<void**>(secondPed.data()+0x47C)=otherIntel.data();*reinterpret_cast<void**>(anchor.data()+0x480)=data.data();*reinterpret_cast<void**>(secondPed.data()+0x480)=otherData.data();*reinterpret_cast<float*>(data.data()+0x44)=(i&32)?500.f:1000.f;*reinterpret_cast<float*>(otherData.data()+0x44)=(i&64)?500.f:1000.f;p->bInVehicle=(i&8)!=0;q->bInVehicle=(i&8)!=0;p->m_pVehicle=reinterpret_cast<CVehicle*>(vehicle.data());q->m_pVehicle=p->m_pVehicle;*reinterpret_cast<uint32*>(vehicle.data()+0x40)=(i&4)?0x100:0;vehicle[0x42B]=uint8((i&2)?0x40:0);constexpr int weapons[]{0,22,24,28,32,35};*reinterpret_cast<int8*>(anchor.data()+0x718)=0;*reinterpret_cast<int32*>(anchor.data()+0x5A0)=weapons[(i/3)%6];CWorld::PlayerInFocus=uint8((i/3)%2);*reinterpret_cast<void**>(0xB7CD98+400*CWorld::PlayerInFocus)=anchor.data();StaticRef<CPlayerPed*>(0xB7CD98)=reinterpret_cast<CPlayerPed*>(anchor.data());StaticRef<CPlayerPed*>(0xB7CF28)=((i&1)||CWorld::PlayerInFocus==1)?reinterpret_cast<CPlayerPed*>(secondPed.data()):nullptr;StaticRef<uint8>(0xB70153)=uint8(i%24);StaticRef<uint8>(0xB70152)=uint8(i%60);StaticRef<uint32>(0xBAA400)=uint32((i&128)!=0);StaticRef<int32>(0xB7CE7C+400*CWorld::PlayerInFocus)=100;StaticRef<int32>(0xB7CE54+400*CWorld::PlayerInFocus)=(i&256)?-123456:123456;for(uintptr_t a:{uintptr_t(0xBAA434),uintptr_t(0xBAA424),uintptr_t(0xBAA404)}){StaticRef<int32>(a)=int32((i/7)%6);StaticRef<int32>(a+4)=int32((i/5)%5*750);StaticRef<int32>(a+8)=int32((i/3)%4*5000);StaticRef<int32>(a+12)=(i&512)?(a==0xBAA404?weapons[(i/3)%6]:a==0xBAA424?((i&256)?-123456:123456):100):-1;}CHud::m_LastBreathTime=(i&64)?3900:0;StaticRef<uint32>(0xB7CB84)=4000;StaticRef<float>(0xB7CB5C)=float(1+i%4)*.5f;constexpr int widths[]{640,800,1024,1920},heights[]{448,600,768,1080};RsGlobal.maximumWidth=widths[(i/3)%4];RsGlobal.maximumHeight=heights[(i/5)%4];h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)()>(fn.address)();return json{{"timer",ReversingProbe::Bytes(anchor.data(),anchor.size())},{"globals",ReversingProbe::Bytes(globals,saved.size())},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=!a["trace"].empty();yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
std::memcpy(globals,saved.data(),saved.size());TheCamera.m_bWideScreenOn=cameraWide;CCutsceneMgr::ms_running=cutscene;StaticRef<bool>(0xBA678C)=subtitles;std::memcpy(reinterpret_cast<void*>(0xA44B60),formatting.data(),8);StaticRef<uint8>(0xB70153)=hoursSaved;StaticRef<uint8>(0xB70152)=minutesSaved;StaticRef<CPlayerPed*>(0xB7CF28)=coopPedSaved;StaticRef<uint8>(0xA43088)=replaySaved;StaticRef<uint8>(0xB7014E)=daySaved;std::memcpy(gString,stringSaved.data(),200);std::memcpy(reinterpret_cast<void*>(0xBA1788),timerSaved.data(),timerSaved.size());StaticRef<bool>(0x96C014)=disableSaved;std::memcpy(reinterpret_cast<void*>(0xB7CD98),playersSaved.data(),800);CWorld::PlayerInFocus=focusSaved;StaticRef<uint32>(0xB7CB4C)=frameSaved;StaticRef<uint32>(0xB7CB84)=clockSaved;StaticRef<float>(0xB7CB5C)=timeSaved;RsGlobal.maximumWidth=widthSaved;RsGlobal.maximumHeight=heightSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}

