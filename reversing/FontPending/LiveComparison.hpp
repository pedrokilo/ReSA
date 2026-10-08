#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Font.h"
#include "Sprite.h"
#include "Sprite2d.h"
#include "../ProbeRuntime.hpp"
namespace FontPendingLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log << r.dump() << '\n'; log.flush(); }
template<class T=uint32> T& At(void* b, size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint32 F(float v) { return std::bit_cast<uint32>(v); }
struct Fn { const char* cls; const char* name; uintptr_t address, vt; int slot, cases; };
inline const std::array<Fn,4> fns{{
 {"CFont","PrintChar",0x718A10,0,-1,2048},
 {"CFont","FindSubFontCharacter",0x7192C0,0,-1,1024},
 {"CFont","RenderFontBuffer",0x719840,0,-1,1024},
 {"CFont","ProcessCurrentString",0x71A220,0,-1,2048},
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* name) {
    ReversibleHooks::ReversibleHook::Base* found{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==name)found=h.get();}); return found;
}


inline uint64 Hash(const void* data,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(data);for(size_t i=0;i<n;++i)h=(h^b[i])*1099511628211ull;return h;}
inline float __cdecl Property(uint8 ch){trace.push_back({"prop",ch});return float(ch%27+4);}
inline void __cdecl Add(const CRect& rect,const CRGBA& color,float u1,float v1,float u2,float v2,float u3,float v3,float u4,float v4){trace.push_back({"glyph",ReversingProbe::Bytes(&rect,sizeof(rect)),ReversingProbe::Bytes(&color,sizeof(color)),F(u1),F(v1),F(u2),F(v2),F(u3),F(v3),F(u4),F(v4)});}
inline void __fastcall Draw(CSprite2d* s,void*,const CRect& rect,const CRGBA& color){trace.push_back({"button",int((uintptr_t(s)-0xC71AD8)/4),ReversingProbe::Bytes(&rect,sizeof(rect)),ReversingProbe::Bytes(&color,sizeof(color))});}
inline void __fastcall SpriteState(CSprite2d* s,void*){trace.push_back({"sprite",int((uintptr_t(s)-0xC71AD0)/4)});}
inline void __cdecl Flush(){trace.push_back("flush");}inline void __cdecl Render(){trace.push_back("render");}
inline RwBool __cdecl State(RwRenderState state,void* value){trace.push_back({"state",int(state),uintptr_t(value)});return true;}
inline void __cdecl Char(float x,float y,char c){trace.push_back({"char",F(x),F(y),uint8(c),int(CFont::PS2Symbol)});}
inline float __cdecl Width(const GxtChar* string,bool full,bool script){const auto* s=reinterpret_cast<const char*>(string);int n=0;while(s[n]&&s[n]!=' '&&s[n]!='~')++n;trace.push_back({"width",std::string(s),full,script});return float(n)*7.25f;}
inline float __cdecl Size(uint8 c){trace.push_back({"size",c});return 3.5f;}
inline char* __cdecl Token(char* s,CRGBA& color,bool images,char* tag){const char c=s[1];char* end=s+1;while(*end&&*end!='~')++end;if(*end)++end;trace.push_back({"token",c,images,tag!=nullptr});if(c=='n')CFont::m_bNewLine=true;else if(c=='x')CFont::PS2Symbol=EXSYMBOL_CROSS;else{color={uint8(c),0x12,0x34,0x56};if(tag)*tag=c;}return end;}
inline void __cdecl Line(float x,float y,const char* begin,const char* end,float spacing){trace.push_back({"line",F(x),F(y),std::string(begin,end),F(spacing)});}
inline void __cdecl Color(CRGBA c){trace.push_back({"color",ReversingProbe::Bytes(&c,sizeof(c))});CFont::m_Color=c;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"font-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"font-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("font-pending");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 std::array<uint8,0x500> saved{};memcpy(saved.data(),reinterpret_cast<void*>(0xC716A8),saved.size());std::array<char,352> savedString{};memcpy(savedString.data(),gString,sizeof(gString));const auto savedWidth=RsGlobal.maximumWidth,savedHeight=RsGlobal.maximumHeight;bool passed=true;uint32 pairs=0;
 {
 using ReversingProbe::EntryPatch;
 EntryPatch e1(0x718770,Property),d1(std::bit_cast<uintptr_t>(&GetLetterIdPropValue),Property),e2(0x728200,Add),d2(std::bit_cast<uintptr_t>(&CSprite2d::AddToBuffer),Add),e3(0x728350,Draw),d3(std::bit_cast<uintptr_t>(static_cast<void(CSprite2d::*)(const CRect&,const CRGBA&)>(&CSprite2d::Draw)),Draw),e4(0x727B30,SpriteState),d4(std::bit_cast<uintptr_t>(&CSprite2d::SetRenderState),SpriteState),e5(0x70CF20,Flush),d5(std::bit_cast<uintptr_t>(&CSprite::FlushSpriteBuffer),Flush),e6(0x7273D0,Render),d6(std::bit_cast<uintptr_t>(&CSprite2d::RenderVertexBuffer),Render),e7(0x71A0E0,Width),d7(std::bit_cast<uintptr_t>(&CFont::GetStringWidth),Width),e8(0x719750,Size),d8(std::bit_cast<uintptr_t>(&CFont::GetCharacterSize),Size),e9(0x718F00,Token),d9(std::bit_cast<uintptr_t>(&CFont::ParseToken),Token),e10(0x719B40,Line),e11(0x719430,Color),d11(std::bit_cast<uintptr_t>(&CFont::SetColor),Color);
 const auto engine=*reinterpret_cast<uintptr_t*>(0xC97B24);EntryPatch rs(At<uintptr_t>(reinterpret_cast<void*>(engine),0x20),State);
 for(size_t f=0;f<fns.size();++f){const auto& fn=fns[f];auto* hook=FindHook(fn.cls,fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);int failures=0,active=0;const std::string full=std::string(fn.cls)+"::"+fn.name;
 std::unique_ptr<EntryPatch> printExe,printDll;if(f==2){printExe=std::make_unique<EntryPatch>(0x718A10,Char);printDll=std::make_unique<EntryPatch>(std::bit_cast<uintptr_t>(&CFont::PrintChar),Char);}
 for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){hook->State(candidate);trace=json::array();memcpy(reinterpret_cast<void*>(0xC716A8),saved.data(),saved.size());memset(gString,0x12,sizeof(gString));RsGlobal.maximumWidth=640;RsGlobal.maximumHeight=480;
 auto& state=CFont::RenderState;memset(&state,0x35,sizeof(state));state.m_fWidth=float(i%7+1)*0.125f;state.m_fHeight=float(i%5+1)*0.25f;state.m_fSlant=i%3==0?0.0f:i%3==1?0.25f:std::bit_cast<float>(0x80000000u);state.m_wFontTexture=uint16(i%3);state.m_nFontStyle=uint8(i%4);state.m_bPropOn=i%2;state.m_vPosn={23.25f,41.5f};state.m_vSlanRefPoint={100.0f,20.0f};state.m_fWrap=3.25f;state.m_nOutline=uint8(i%5==0?255:i%4);CFont::PS2Symbol=i%13==0?EXSYMBOL_CROSS:EXSYMBOL_NONE;
 auto* data=reinterpret_cast<uint8*>(0xC718B0);for(int k=0;k<420;++k)data[k]=uint8(k*13+i);CFont::m_Color={0x11,0x22,0x33,0x44};CFont::m_bNewLine=false;CFont::m_bFontCentreAlign=i%4==1;CFont::m_bFontRightAlign=i%4==2;CFont::m_bFontJustify=i%4==3;CFont::m_fWrapx=50.0f+float(i%17);CFont::m_fFontCentreSize=47.0f+float(i%13);CFont::m_fRightJustifyWrap=3.0f;CFont::m_Scale={0.75f,float(i%5+1)*0.125f};
 const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;int result=0;
 if(f==0){constexpr std::array<uint32,9> xs{0,0x80000000,0x3f800000,0x44200000,0x44200001,0xbf800000,0x7fc00000,0x7f800000,0x42140000};const float x=i%3==0?std::bit_cast<float>(xs[(i/3)%xs.size()]):float(i%600);const float y=float(i%500);reinterpret_cast<void(__cdecl*)(float,float,char)>(fn.address)(x,y,char(i%256));}
 else if(f==1)result=reinterpret_cast<uint8(__cdecl*)(uint8,uint8)>(fn.address)(uint8(i),uint8(i/256));
 else if(f==2){state.m_wFontTexture=i%2;state.m_bContainImages=i%3==0;auto* begin=reinterpret_cast<uint8*>(0xC716B0);memset(begin,0,512);memcpy(begin,&state,sizeof(state));const std::array<const char*,8> strings{"ABC", "A B", "~r~ABC", "~x~ABC", "~n~A", "~r~~b~Hi", "?", "\xb1\xbc\xc0"};const auto* text=strings[i%8];memcpy(begin+48,text,strlen(text)+1);uintptr_t end=uintptr_t(begin+48+strlen(text)+1);end=(end+3)&~uintptr_t(3);*reinterpret_cast<void**>(0xC716A8)=i%11==0?begin:reinterpret_cast<void*>(end);reinterpret_cast<void(__cdecl*)()>(fn.address)();}
 else{const std::array<const char*,8> strings{"", "one", "one two three", "~r~one two", "one ~n~two", "~x~two words", "a b c d e f g", "a~r~b c"};result=reinterpret_cast<int16(__cdecl*)(bool,float,float,const GxtChar*)>(fn.address)(i%2!=0,21.25f,17.0f,reinterpret_cast<const GxtChar*>(strings[i%8]));}
 return json{{"globals",Hash(reinterpret_cast<void*>(0xC716A8),saved.size())},{"string",Hash(gString,sizeof(gString))},{"result",result},{"trace",trace},{"switches",switched}};
 };
 const auto original=run(false),candidate=run(true);const bool ok=original==candidate&&original["switches"]==true;passed&=ok;failures+=!ok;active+=!original["trace"].empty();Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",ok},{"accepted",ok},{"original",original},{"candidate",candidate}});
 }
 printDll.reset();printExe.reset();hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases},{"active",active}});
 }
 }
 memcpy(reinterpret_cast<void*>(0xC716A8),saved.data(),saved.size());memcpy(gString,savedString.data(),sizeof(gString));RsGlobal.maximumWidth=savedWidth;RsGlobal.maximumHeight=savedHeight;Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",memcmp(reinterpret_cast<void*>(0xC716A8),saved.data(),saved.size())==0&&memcmp(gString,savedString.data(),sizeof(gString))==0&&RsGlobal.maximumWidth==savedWidth&&RsGlobal.maximumHeight==savedHeight}});
}
}
