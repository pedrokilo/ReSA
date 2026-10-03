#pragma once
#include <fstream>
#include <crtdbg.h>
#include <rtcapi.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "WinInput.h"
#include "ControllerConfigManager.h"
#include "platform.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace UILive {
inline bool done{};inline std::ofstream log;inline int currentCase=-1,currentFunction=-1;
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline int __cdecl Error(int,const wchar_t*,int,const wchar_t*,const wchar_t*,...){Write({{"event","error"},{"reason","runtime check"},{"case",currentCase},{"function",currentFunction}});log.flush();ExitProcess(92);}
inline void __cdecl Assertion(const wchar_t* expression,const wchar_t* file,unsigned line){char expr[1024]{},path[1024]{};WideCharToMultiByte(CP_UTF8,0,expression,-1,expr,1024,nullptr,nullptr);WideCharToMultiByte(CP_UTF8,0,file,-1,path,1024,nullptr,nullptr);Write({{"event","assertion"},{"expression",expr},{"file",path},{"line",line},{"case",currentCase},{"function",currentFunction}});log.flush();ExitProcess(93);}
inline std::array<uintptr_t,32> vtable;inline uintptr_t* device=vtable.data();inline json trace;inline DIMOUSESTATE2 mouse;inline DIJOYSTATE2 joy;inline int reads,acquires,polls;
inline HRESULT __stdcall Acquire(void*){trace.push_back("acquire");++acquires;if(currentCase%19==0)return E_FAIL;if(currentCase%7==0&&acquires<3)return DIERR_INPUTLOST;return S_OK;}
inline HRESULT __stdcall Poll(void*){trace.push_back("poll");++polls;return currentCase%13==0||(currentCase%5==0&&polls==1)?E_FAIL:S_OK;}
inline HRESULT __stdcall State(void*,DWORD size,void* out){trace.push_back(json::array({"state",size}));++reads;if(currentCase%23==0||(currentCase%11==0&&reads==1))return E_FAIL;if(size==sizeof(mouse))std::memcpy(out,&mouse,size);else{assert(size==sizeof(joy));std::memcpy(out,&joy,size);}return S_OK;}
inline void InitMouse(bool exclusive){trace.push_back(json::array({"init_mouse",exclusive}));if(currentCase%3)PSGLOBAL(diMouse)=reinterpret_cast<IDirectInputDevice8*>(&device);}
inline RsEventStatus PadEvent(RsEvent event,void* data){trace.push_back(json::array({"pad_event",int(event),*static_cast<int*>(data)}));return rsEVENTPROCESSED;}
inline void Update(){
 if(done)return;_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"unlock-input-runtime.start")){done=true;return;}
 if(!log.is_open()){_RTC_SetErrorFuncW(Error);_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE);log.open(dir/"unlock-input-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
 ReversingProbe::EntryPatch ap(reinterpret_cast<uintptr_t>(&_wassert),Assertion);
 using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,2> hooks{};
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPad")for(auto& h:c.Items()){if(h->Name()=="UpdateMouse")hooks[0]=h.get();if(h->Name()=="ProcessPad")hooks[1]=h.get();}});
 if(!hooks[0]||!hooks[1]){Write({{"event","error"},{"reason","missing hook"}});log.flush();return;}
 const auto padsBefore=ReversingProbe::Bytes(&CPad::Pads,sizeof(CPad::Pads)),menuBefore=ReversingProbe::Bytes(&FrontEndMenuManager,sizeof(FrontEndMenuManager)),controlsBefore=ReversingProbe::Bytes(&ControlsManager,sizeof(ControlsManager)),joysBefore=ReversingProbe::Bytes(&AllValidWinJoys,sizeof(AllValidWinJoys)),psBefore=ReversingProbe::Bytes(RsGlobal.ps,40),mouseBefore=ReversingProbe::Bytes(&CPad::TempMouseControllerState,60),axesBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC8CFB0),16);
 const auto foregroundBefore=*reinterpret_cast<uint8*>(0x8D621C);const auto numberBefore=CPad::padNumber;const auto timeBefore=CTimer::GetTimeInMS();
 vtable[7]=reinterpret_cast<uintptr_t>(Acquire);vtable[9]=reinterpret_cast<uintptr_t>(State);vtable[25]=reinterpret_cast<uintptr_t>(Poll);
 std::array<bool,2> enabled{},locked{};for(int fn=0;fn<2;++fn){enabled[fn]=hooks[fn]->Hooked();locked[fn]=hooks[fn]->Locked();hooks[fn]->LockState(false);Write({{"event","hook_state"},{"function",fn},{"enabled",enabled[fn]},{"locked",locked[fn]},{"startup_calls",UIProbe::calls[fn]}});}
 bool passed=true;int pairs=0;
 {
 ReversingProbe::EntryPatch mn(0x7469A0,InitMouse),ml(reinterpret_cast<uintptr_t>(&WinInput::diMouseInit),InitMouse),en(0x6195A0,PadEvent),el(reinterpret_cast<uintptr_t>(&RsPadEventHandler),PadEvent);
 for(int fn=0;fn<2&&passed;++fn){currentFunction=fn;for(int i=0;i<(fn?4096:2048)&&passed;++i){currentCase=i;
  const auto run=[&]{
   trace=json::array();reads=acquires=polls=0;std::memcpy(&CPad::Pads,padsBefore.data(),padsBefore.size());std::memcpy(&ControlsManager,controlsBefore.data(),controlsBefore.size());std::memcpy(&AllValidWinJoys,joysBefore.data(),joysBefore.size());std::memcpy(&FrontEndMenuManager,menuBefore.data(),menuBefore.size());std::memcpy(&CPad::TempMouseControllerState,mouseBefore.data(),60);std::memcpy(reinterpret_cast<void*>(0xC8CFB0),axesBefore.data(),16);
   auto* dev=reinterpret_cast<IDirectInputDevice8*>(&device);PSGLOBAL(diMouse)=i%17?dev:nullptr;PSGLOBAL(diDevice1)=PSGLOBAL(diDevice2)=i%29?dev:nullptr;CPad::padNumber=(i/2)%2;*reinterpret_cast<uint8*>(0x8D621C)=i%31!=0;
   FrontEndMenuManager.m_bMenuActive=(i/3)%2;FrontEndMenuManager.bInvertMouseX=(i/6)%2;FrontEndMenuManager.bInvertMouseY=(i/12)%2;
   FrontEndMenuManager.m_bInvertPadX1=(i/4)%2;FrontEndMenuManager.m_bInvertPadY1=(i/8)%2;FrontEndMenuManager.m_bInvertPadX2=(i/16)%2;FrontEndMenuManager.m_bInvertPadY2=(i/32)%2;FrontEndMenuManager.m_bSwapPadAxis1=(i/64)%2;FrontEndMenuManager.m_bSwapPadAxis2=(i/128)%2;
   mouse={};mouse.lX=(i%37-18)*17;mouse.lY=(i%29-14)*13;mouse.lZ=(i%3-1)*120;for(int b=0;b<8;++b)mouse.rgbButtons[b]=(i>>b)&1?0x81:0x01;
   joy={};joy.lX=std::array{-2000,-601,-600,-599,0,599,600,601,2000}[i%9];joy.lY=std::array{-2000,-701,0,701,2000}[(i/9)%5];joy.lZ=(i%31-15)*111;joy.lRz=(i%17-8)*173;joy.rgdwPOV[0]=i%5?0xFFFFFFFFu:uint32((i/5)%8)*4500;for(int b=0;b<128;++b)joy.rgbButtons[b]=(i>>b%12)&1?0x80:0;
   ControlsManager.m_bJoyJustInitialised=(i/3)%2;for(auto& j:AllValidWinJoys.JoyStickNum){j.bZAxisPresent=(i/4)%2;j.bZRotPresent=(i/8)%2;}
   if(fn)reinterpret_cast<void(__cdecl*)(ePadID)>(0x746A10)(static_cast<ePadID>(i%2));else reinterpret_cast<void(__thiscall*)(CPad*)>(0x53F3C0)(CPad::GetPad(i%2));
   return json{{"trace",trace},{"pads",ReversingProbe::Bytes(&CPad::Pads,sizeof(CPad::Pads))},{"mouse",ReversingProbe::Bytes(&CPad::TempMouseControllerState,60)},{"controls",ReversingProbe::Bytes(&ControlsManager,4+2*sizeof(DIJOYSTATE2))},{"axes",ReversingProbe::Bytes(reinterpret_cast<void*>(0xC8CFB0),16)}};
  };
  hooks[fn]->State(false);auto a=UIProbe::calls[fn];auto original=run();auto b=UIProbe::calls[fn];hooks[fn]->State(true);auto candidate=run();auto c=UIProbe::calls[fn];passed=original==candidate&&a==b&&c==b+1;Write({{"event","comparison"},{"function",fn},{"case",i},{"pair",pairs++},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});if(i%64==0||!passed)log.flush();
 }}
 }
 std::memcpy(&CPad::Pads,padsBefore.data(),padsBefore.size());std::memcpy(&ControlsManager,controlsBefore.data(),controlsBefore.size());std::memcpy(&AllValidWinJoys,joysBefore.data(),joysBefore.size());std::memcpy(&FrontEndMenuManager,menuBefore.data(),menuBefore.size());std::memcpy(&CPad::TempMouseControllerState,mouseBefore.data(),60);std::memcpy(reinterpret_cast<void*>(0xC8CFB0),axesBefore.data(),16);std::memcpy(RsGlobal.ps,psBefore.data(),40);*reinterpret_cast<uint8*>(0x8D621C)=foregroundBefore;CPad::padNumber=numberBefore;
 for(int fn=0;fn<2;++fn){hooks[fn]->State(enabled[fn]);hooks[fn]->LockState(locked[fn]);}
 bool restored=padsBefore==ReversingProbe::Bytes(&CPad::Pads,sizeof(CPad::Pads))&&controlsBefore==ReversingProbe::Bytes(&ControlsManager,sizeof(ControlsManager))&&psBefore==ReversingProbe::Bytes(RsGlobal.ps,40)&&timeBefore==CTimer::GetTimeInMS();Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();
}
}
