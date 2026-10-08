#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Hud.h"
#include "Wanted.h"
#include "Font.h"
#include "PointList.h"
#include "PedList.h"
#include "../ProbeRuntime.hpp"
namespace HudWanted355Live {
inline bool done{};inline std::ofstream log;inline std::array<uint32,32> pedTokens{},targetTokens{};
alignas(16) inline std::array<uint8,sizeof(CPed)> anchor{};
inline json trace;inline int scenario{};alignas(16) inline std::array<uint8,sizeof(CWanted)> wantedBytes{};
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
{"CHud","DrawWanted",0x58D9A0,0,-1,1024}};
inline auto* FindHook(const char* n){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CHud")for(auto& x:c.Items())if(x->Name()==n)h=x.get();});return h;}
inline void Update(){
if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"hud-wanted355-runtime.start")){done=true;return;}
if(!log.is_open()){log.open(dir/"hud-wanted355-runtime.jsonl");ReversingProbe::InstallFailureCapture("hud-wanted355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto* globals=reinterpret_cast<uint8*>(0xBAA388);std::array<uint8,0xEB0> saved{};std::memcpy(saved.data(),globals,saved.size());auto cameraWide=TheCamera.m_bWideScreenOn;auto cutscene=CCutsceneMgr::ms_running;auto subtitles=StaticRef<bool>(0xBA678C);std::array<uint8,8> formatting{};std::memcpy(formatting.data(),reinterpret_cast<void*>(0xA44B60),8);std::array<uint8,800> playersSaved{};std::memcpy(playersSaved.data(),reinterpret_cast<void*>(0xB7CD98),800);auto focusSaved=CWorld::PlayerInFocus;auto frameSaved=CTimer::GetFrameCounter();auto clockSaved=CTimer::GetTimeInMS();const auto timeSaved=CTimer::GetTimeStep();const auto widthSaved=RsGlobal.maximumWidth,heightSaved=RsGlobal.maximumHeight;
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch b1(0x7195C0,Background),b2(bit_cast<uintptr_t>(&CFont::SetBackground),Background),o1(0x719610,Orientation),o2(bit_cast<uintptr_t>(&CFont::SetOrientation),Orientation),p1(0x7195B0,Proportional),p2(bit_cast<uintptr_t>(&CFont::SetProportional),Proportional),s1(0x719490,Style),s2(bit_cast<uintptr_t>(&CFont::SetFontStyle),Style),co1(0x719430,Color),co2(bit_cast<uintptr_t>(&CFont::SetColor),Color),dc1(0x719510,DropColor),dc2(bit_cast<uintptr_t>(&CFont::SetDropColor),DropColor),sc1(0x719380,Scale),sc2(bit_cast<uintptr_t>(&CFont::SetScale),Scale),ce1(0x7194E0,Centre),ce2(bit_cast<uintptr_t>(&CFont::SetCentreSize),Centre),pr1(0x71A700,Print),pr2(bit_cast<uintptr_t>(&CFont::PrintString),Print),j1(0x719600,Justify),j2(bit_cast<uintptr_t>(&CFont::SetJustify),Justify),e1(0x719590,Edge),e2(bit_cast<uintptr_t>(&CFont::SetEdge),Edge),l1(0x71A5E0,Lines),l2(bit_cast<uintptr_t>(&CFont::GetNumberLines),Lines),w1(0x56E230,Wanted),w2(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted),a1(0x718600,Gxt),a2(bit_cast<uintptr_t>(&AsciiToGxtChar),Gxt),hy1(0x588B60,HealthY),hy2(bit_cast<uintptr_t>(&CHud::GetYPosBasedOnHealth),HealthY);
int pairs=0;bool pass=true;
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}auto old=h->Hooked(),lock=h->Locked();h->LockState(false);int failures=0,yes=0,no=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();std::memset(globals,0xCD,saved.size());wantedBytes.fill(0);auto*w=reinterpret_cast<CWanted*>(wantedBytes.data());w->m_WantedLevel=static_cast<eWantedLevel>(i%7);w->m_WantedLevelBeforeParole=static_cast<eWantedLevel>((i/7)%7);w->m_LastTimeWantedLevelChanged=(i&16)?0u:3000u;CHud::m_WantedState=uint32((i/7)%6);CHud::m_WantedFadeTimer=uint32((i/3)%5*800);CHud::m_WantedTimer=uint32((i/5)%4*6000);CHud::m_LastWanted=(i&128)?uint32(i%7):uint32((i+1)%7);if(i<64){w->m_WantedLevel=static_cast<eWantedLevel>(0);w->m_WantedLevelBeforeParole=static_cast<eWantedLevel>(0);CHud::m_WantedState=0;CHud::m_LastWanted=0;CHud::m_WantedFadeTimer=0;}StaticRef<float>(0xB7CB5C)=float(1+i%4)*.5f;StaticRef<uint32>(0xB7CB4C)=uint32((i&4)?4:0);StaticRef<uint32>(0xB7CB84)=4000;CWorld::PlayerInFocus=uint8((i/3)%2);*reinterpret_cast<uint8*>(0xB7CEE7+400*CWorld::PlayerInFocus)=uint8((i&8)?100:101);constexpr int widths[]{640,800,1024,1920},heights[]{448,600,768,1080};RsGlobal.maximumWidth=widths[(i/3)%4];RsGlobal.maximumHeight=heights[(i/5)%4];h->State(candidate);bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;reinterpret_cast<void(__cdecl*)()>(fn.address)();return json{{"globals",Hash(globals,saved.size())},{"trace",trace},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;const bool changed=std::any_of(a["trace"].begin(),a["trace"].end(),[](const json& item){return item.is_array() && !item.empty() && item[0]=="background";});yes+=changed;no+=!changed;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(lock);pass&=yes>0&&no>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"found",yes},{"missing",no}});}
std::memcpy(globals,saved.data(),saved.size());TheCamera.m_bWideScreenOn=cameraWide;CCutsceneMgr::ms_running=cutscene;StaticRef<bool>(0xBA678C)=subtitles;std::memcpy(reinterpret_cast<void*>(0xA44B60),formatting.data(),8);std::memcpy(reinterpret_cast<void*>(0xB7CD98),playersSaved.data(),800);CWorld::PlayerInFocus=focusSaved;StaticRef<uint32>(0xB7CB4C)=frameSaved;StaticRef<uint32>(0xB7CB84)=clockSaved;StaticRef<float>(0xB7CB5C)=timeSaved;RsGlobal.maximumWidth=widthSaved;RsGlobal.maximumHeight=heightSaved;Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
