#pragma once
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
#include "UIRenderer.h"
#include "SDLWrapper.hpp"
#include <SDL3/SDL.h>
namespace PlatformLive {
inline void __fastcall SaveSettings(CMenuManager*,void*){}
inline bool __fastcall Retuning(CAudioEngine*,void*){return false;}
inline bool entered{};
inline void Update(){
 using namespace PlatformProbe;
 if(!active||finished||entered)return;
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;entered=true;inTest=true;
 std::array<bool,7> enabled{};for(size_t i=0;i<7;++i)enabled[i]=hooks[i]->Hooked();
 const auto psBefore=ReversingProbe::Bytes(RsGlobal.ps,40);const bool uiBefore=notsa::ui::UIRenderer::GetSingleton().IsActive();
 Write({{"event","startup"},{"enabled",enabled},{"calls",calls},{"ps_static",RsGlobal.ps==reinterpret_cast<psGlobalType*>(0xC8CF88)},{"ps_size",sizeof(psGlobalType)},{"sdl_attached",notsa::SDLWrapper::GetWindow()!=nullptr},{"rw_camera",Scene.m_pRwCamera&&RwCameraGetRaster(Scene.m_pRwCamera)!=nullptr}});
 bool passed=true;int pairs=0;
 for(int index:{4,6}){
  for(int i=0;i<16&&passed;++i){
   auto run=[&]{if(index==4){auto* wnd=notsa::SDLWrapper::GetWindow();reinterpret_cast<void(__cdecl*)()>(0x7458A0)();return json{{"ps",ReversingProbe::Bytes(RsGlobal.ps,40)},{"sdl_unchanged",wnd==notsa::SDLWrapper::GetWindow()}};}
    RsGlobal.quit=false;PostThreadMessageA(GetCurrentThreadId(),i%2?WM_QUIT:WM_NULL,0,0);reinterpret_cast<void(__cdecl*)()>(0x746870)();const bool quit=RsGlobal.quit;RsGlobal.quit=false;return json{{"quit",quit}};
   };
   hooks[index]->State(false);auto a=calls[index];auto original=run();auto b=calls[index];hooks[index]->State(true);auto candidate=run();auto c=calls[index];passed=original==candidate&&b==a&&c==b+1;
   Write({{"event","comparison"},{"function",index},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});++pairs;
  }
  hooks[index]->State(enabled[index]);
 }
 {
 ReversingProbe::EntryPatch sn(0x57C660,SaveSettings),sl(std::bit_cast<uintptr_t>(&CMenuManager::SaveSettings),SaveSettings),rn(0x506FF0,Retuning),rl(std::bit_cast<uintptr_t>(&CAudioEngine::IsRadioRetuneInProgress),Retuning);
 for(int i=0;i<4&&passed;++i){
  const auto run=[&]{
   auto& menu=FrontEndMenuManager;menu.m_bMenuActive=false;menu.m_bDontDrawFrontEnd=false;menu.m_bActivateMenuNextFrame=true;menu.m_bLoadingData=false;menu.m_bIsSaveDone=false;menu.m_isPreInitialised=false;menu.m_nPlayerNumber=i%2;menu.m_nCurrentScreen=SCREEN_PAUSE_MENU;
   reinterpret_cast<void(__thiscall*)(CMenuManager*)>(0x576B70)(&menu);const bool opened=menu.m_bMenuActive;
   menu.m_bActivateMenuNextFrame=false;menu.m_bDontDrawFrontEnd=true;
   reinterpret_cast<void(__thiscall*)(CMenuManager*)>(0x576B70)(&menu);const bool closed=!menu.m_bMenuActive;
   return json{{"opened",opened},{"closed",closed},{"dont_draw",menu.m_bDontDrawFrontEnd},{"activate_next",menu.m_bActivateMenuNextFrame},{"preinitialised",menu.m_isPreInitialised}};
  };
  hooks[2]->State(false);auto a=calls[2];auto original=run();auto b=calls[2];hooks[2]->State(true);auto candidate=run();auto c=calls[2];passed=original==candidate&&original["opened"]==true&&original["closed"]==true&&a==b&&c==b+2;
  Write({{"event","comparison"},{"function",2},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});++pairs;
 }
 hooks[2]->State(enabled[2]);
 }
 // Exercise the Windows -> SDL -> ImGui path with input confined to this HWND.
 auto& ui=notsa::ui::UIRenderer::GetSingleton();
 for(int mode=0;mode<2&&passed;++mode){ui.SetIsActive(mode!=0);CPad::TempKeyState.standardKeys['J']=0;const auto events=uiEvents;
  SendMessageA(PSGLOBAL(window),WM_KEYDOWN,'J',1|(MapVirtualKeyA('J',MAPVK_VK_TO_VSC)<<16));
  SendMessageA(PSGLOBAL(window),WM_MOUSEMOVE,0,MAKELPARAM(31,37));notsa::SDLWrapper::ProcessEvents();
  const bool gameReceived=CPad::TempKeyState.standardKeys['J']!=0;const bool uiReceived=uiEvents>events;
  SendMessageA(PSGLOBAL(window),WM_KEYUP,'J',0xC0000001|(MapVirtualKeyA('J',MAPVK_VK_TO_VSC)<<16));notsa::SDLWrapper::ProcessEvents();
  passed=gameReceived==(mode==0)&&uiReceived;Write({{"event","input_route"},{"ui_active",mode!=0},{"game_received",gameReceived},{"ui_received",uiReceived},{"passed",passed}});
 }
 ui.SetIsActive(uiBefore);for(size_t i=0;i<7;++i)hooks[i]->State(enabled[i]);
 const bool restored=std::ranges::equal(enabled,hooks,{},[](bool e){return e;},[](Hook* h){return h->Hooked();})&&RsGlobal.ps==reinterpret_cast<psGlobalType*>(0xC8CF88)&&!FrontEndMenuManager.m_bMenuActive;
 finished=true;Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored},{"calls",calls}});inTest=false;
}
}
