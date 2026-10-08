#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Hud.h"
#include "Wanted.h"
#include "Text/Text.h"
#include "Sprite2d.h"
#include "Font.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace HudTimers355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline json trace;inline int scenario{};alignas(16) inline std::array<uint8,sizeof(CWanted)> wantedBytes{};
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
{"CHud","DrawMissionTimers",0x58B180,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CHud")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"hud-timers355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"hud-timers355-runtime.jsonl");ReversingProbe::InstallFailureCapture("hud-timers355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* globals=reinterpret_cast<uint8*>(0xBAA388);std::array<uint8,0xEB0> saved{};std::memcpy(saved.data(),globals,saved.size());auto cameraWide=TheCamera.m_bWideScreenOn;auto cutscene=CCutsceneMgr::ms_running;auto subtitles=StaticRef<bool>(0xBA678C);std::array<uint8,8> formatting{};std::memcpy(formatting.data(),reinterpret_cast<void*>(0xA44B60),8);std::array<uint8,0x154> timerSaved{};std::memcpy(timerSaved.data(),reinterpret_cast<void*>(0xBA1788),timerSaved.size());auto disableSaved=StaticRef<bool>(0x96C014);std::array<uint8,800> playersSaved{};std::memcpy(playersSaved.data(),reinterpret_cast<void*>(0xB7CD98),800);auto focusSaved=CWorld::PlayerInFocus;auto frameSaved=CTimer::GetFrameCounter();auto clockSaved=CTimer::GetTimeInMS();const auto timeSaved=CTimer::GetTimeStep();const auto widthSaved=RsGlobal.maximumWidth,heightSaved=RsGlobal.maximumHeight;
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch b1(0x7195C0,Background),b2(bit_cast<uintptr_t>(&CFont::SetBackground),Background),o1(0x719610,Orientation),o2(bit_cast<uintptr_t>(&CFont::SetOrientation),Orientation),p1(0x7195B0,Proportional),p2(bit_cast<uintptr_t>(&CFont::SetProportional),Proportional),s1(0x719490,Style),s2(bit_cast<uintptr_t>(&CFont::SetFontStyle),Style),co1(0x719430,Color),co2(bit_cast<uintptr_t>(&CFont::SetColor),Color),dc1(0x719510,DropColor),dc2(bit_cast<uintptr_t>(&CFont::SetDropColor),DropColor),sc1(0x719380,Scale),sc2(bit_cast<uintptr_t>(&CFont::SetScale),Scale),ce1(0x7194E0,Centre),ce2(bit_cast<uintptr_t>(&CFont::SetCentreSize),Centre),pr1(0x71A700,Print),pr2(bit_cast<uintptr_t>(&CFont::PrintString),Print),j1(0x719600,Justify),j2(bit_cast<uintptr_t>(&CFont::SetJustify),Justify),e1(0x719590,Edge),e2(bit_cast<uintptr_t>(&CFont::SetEdge),Edge),l1(0x71A5E0,Lines),l2(bit_cast<uintptr_t>(&CFont::GetNumberLines),Lines),w1(0x56E230,Wanted),w2(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted),a1(0x718600,Gxt),a2(bit_cast<uintptr_t>(&AsciiToGxtChar),Gxt),hy1(0x588B60,HealthY),hy2(bit_cast<uintptr_t>(&CHud::GetYPosBasedOnHealth),HealthY),tg1(0x6A0050,TextGet),tg2(bit_cast<uintptr_t>(&CText::Get),TextGet),wr1(0x7194D0,Wrap),wr2(bit_cast<uintptr_t>(&CFont::SetWrapx),Wrap),rw1(0x7194F0,RightWrap),rw2(bit_cast<uintptr_t>(&CFont::SetRightJustifyWrap),RightWrap),bar1(0x728640,Bar),bar2(bit_cast<uintptr_t>(&CSprite2d::DrawBarChart),Bar);
int pairs=0;bool pass=true;
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(globals,0xCD,saved.size());std::memset(CHud::m_BigMessage,0,sizeof(CHud::m_BigMessage));CHud::m_BigMessage[4][0]=GxtChar((i&1)?'X':0);StaticRef<bool>(0xBAA3FA)=(i&2)!=0;StaticRef<bool>(0x96C014)=(i&256)!=0;StaticRef<uint32>(0xB7CB4C)=uint32((i&4)?4:0);StaticRef<uint32>(0xB7CF28)=uint32((i&8)!=0);CWorld::PlayerInFocus=uint8((i/3)%2);StaticRef<uint8>(0xB7D077)=uint8((i&16)?100:101);auto*timer=reinterpret_cast<uint8*>(0xBA1788);std::memset(timer,0,0x154);std::memcpy(timer+4,"CLOCK",6);std::memcpy(timer+0xE,"01:23",6);timer[0x38]=uint8((i&32)!=0);timer[0x150]=uint8((i&64)!=0);StaticRef<int16>(0xBAA388)=int16((i/3)%4*20);StaticRef<uint8>(0xBAA38A)=uint8((i&128)!=0);for(int k=0;k<4;++k){auto*c=timer+0x40+k*0x44;std::memcpy(c+8,"COUNTER",8);if((i+k)%3==0)c[8]=0;*reinterpret_cast<int16*>(c+0x12)=int16((i+k)%2);std::memcpy(c+0x14,(i&512)?"75":"123",4);c[0x3E]=uint8((i+k)%3!=0);c[0x3F]=uint8((i+k)%2);c[0x40]=uint8((i+k)%15);*reinterpret_cast<uint8*>(0xBAA394+k)=uint8((i&128)!=0);*reinterpret_cast<int16*>(0xBAA38C+2*k)=int16((i/5+k)%4*20);}constexpr int widths[]{640,800,1024,1920},heights[]{448,600,768,1080};RsGlobal.maximumWidth=widths[(i/3)%4];RsGlobal.maximumHeight=heights[(i/5)%4];h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)()>(fn.address)();return json{{"timer",ReversingProbe::Bytes(timer,0x154)},{"globals",ReversingProbe::Bytes(globals,saved.size())},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=!a["trace"].empty();yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
std::memcpy(globals,saved.data(),saved.size());TheCamera.m_bWideScreenOn=cameraWide;CCutsceneMgr::ms_running=cutscene;StaticRef<bool>(0xBA678C)=subtitles;std::memcpy(reinterpret_cast<void*>(0xA44B60),formatting.data(),8);std::memcpy(reinterpret_cast<void*>(0xBA1788),timerSaved.data(),timerSaved.size());StaticRef<bool>(0x96C014)=disableSaved;std::memcpy(reinterpret_cast<void*>(0xB7CD98),playersSaved.data(),800);CWorld::PlayerInFocus=focusSaved;StaticRef<uint32>(0xB7CB4C)=frameSaved;StaticRef<uint32>(0xB7CB84)=clockSaved;StaticRef<float>(0xB7CB5C)=timeSaved;RsGlobal.maximumWidth=widthSaved;RsGlobal.maximumHeight=heightSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
