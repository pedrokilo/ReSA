#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
namespace PlatformProbe {
inline std::array<unsigned,7> calls{};
using Hook=ReversibleHooks::ReversibleHook::Base;
inline std::array<Hook*,7> hooks{};
inline std::array<bool,7> sourceLocked{};
inline constexpr std::array names{"UpdateMouse","ProcessPad","CheckForMenuClosing","psInitialize","psTerminate","NOTSA_WinMain","MessageLoop"};
inline std::ofstream log;inline bool active{},started{},finished{},inTest{};inline unsigned uiEvents{};inline int lastState=-1;
inline void Write(const json& r){log<<r.dump()<<'\n';log.flush();}
inline void Configure(){char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();active=std::filesystem::exists(dir/"unlock-platform-runtime.start");if(!active)return;log.open(dir/"unlock-platform-runtime.jsonl");
 ReversibleHooks::GetRootCategory().ForEachCategory([](auto& c){for(auto& h:c.Items())for(size_t i=0;i<names.size();++i)if(h->Name()==names[i]&&(c.Name()=="CPad"||c.Name()=="CMenuManager"||c.Name()=="Ps"||c.Name()=="Win")){hooks[i]=h.get();sourceLocked[i]=h->Locked();h->LockState(false);}});
 for(auto* h:hooks)if(!h){Write({{"event","error"},{"reason","missing platform hook"}});ExitProcess(95);}
 const auto* maskText=std::getenv("GTA_REVERSED_PLATFORM_MASK");const int mask=maskText?std::atoi(maskText):127;
 for(size_t i=0;i<hooks.size();++i)hooks[i]->State((mask&(1<<i))!=0);
 Write({{"event","configured"},{"source_locked",sourceLocked},{"mask",mask}});
}
inline void Tick(){if(!active||finished||inTest)return;const int state=int(gGameState);if(state!=lastState){lastState=state;Write({{"event","game_state"},{"state",state},{"calls",calls}});}
 // Native WinMain does not implement FastLoader. Simulate only this test's
 // own input at the intro and main menu, without changing a saved game.
 if(!hooks[5]->Hooked()){
  if(state==2||state==4){PostMessageA(PSGLOBAL(window),WM_KEYDOWN,VK_RETURN,1);PostMessageA(PSGLOBAL(window),WM_KEYUP,VK_RETURN,0xC0000001);}
  if(state==7&&!started){started=true;FrontEndMenuManager.SimulateGameLoad(true,0);}
 }
}
}
