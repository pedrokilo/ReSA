#pragma once
#include <fstream>
#include <crtdbg.h>
#include <rtcapi.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Managers/AEUserRadioTrackManager.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace URLive {
inline bool done{};inline std::ofstream log;inline int currentCase=-1,currentFunction=-1;inline std::filesystem::path fixture,docs;
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline int __cdecl Error(int,const wchar_t*,int,const wchar_t*,const wchar_t*,...){Write({{"event","error"},{"reason","runtime check"},{"case",currentCase},{"function",currentFunction}});log.flush();ExitProcess(92);}
inline void __cdecl Assertion(const wchar_t* expression,const wchar_t* file,unsigned line){char expr[1024]{},path[1024]{};WideCharToMultiByte(CP_UTF8,0,expression,-1,expr,1024,nullptr,nullptr);WideCharToMultiByte(CP_UTF8,0,file,-1,path,1024,nullptr,nullptr);Write({{"event","assertion"},{"expression",expr},{"file",path},{"line",line},{"case",currentCase},{"function",currentFunction}});log.flush();ExitProcess(93);}
inline int Documents(){const auto s=docs.string();assert(s.size()<128);strcpy_s(CFileMgr::ms_dirName,s.c_str());return SetCurrentDirectoryW(docs.c_str())?0:-1;}
inline int Root(const char*){return Documents();}
inline std::string Read(const std::filesystem::path& p){std::ifstream f(p,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
inline json Tracks(const std::vector<tUserTracksInfo>& records,const std::string& data){
 json out=json::array();for(const auto& r:records){assert(r.startUfilesPath<=data.size()&&r.pathLength<=data.size()-r.startUfilesPath);auto path=data.substr(r.startUfilesPath,r.pathLength);std::replace(path.begin(),path.end(),'\\','/');out.push_back(json::array({std::filesystem::path(path).lexically_normal().generic_string(),int(r.fileType)}));}return out;
}
inline void Free(UserTracksLegacyVector& v){for(auto** p=v.first;p!=v.last;++p)plugin::Call<0x8214BD>(*p);plugin::Call<0x8214BD>(v.first);v={};}
inline void Update(){
 if(done)return;_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
 char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"unlock-radio-runtime.start")){done=true;return;}
 if(!log.is_open()){_RTC_SetErrorFuncW(Error);_CrtSetReportMode(_CRT_ASSERT,_CRTDBG_MODE_FILE);_CrtSetReportMode(_CRT_ERROR,_CRTDBG_MODE_FILE);log.open(dir/"unlock-radio-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
 ReversingProbe::EntryPatch ap(reinterpret_cast<uintptr_t>(&_wassert),Assertion);
 fixture=std::filesystem::path(__FILE__).parent_path()/"artifacts/fixtures";
 using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,2> hooks{};
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CAEUserRadioTrackManager")for(auto& h:c.Items()){if(h->Name()=="WriteUserTracksFile-")hooks[0]=h.get();if(h->Name()=="WriteUserTracksThread")hooks[1]=h.get();}});
 if(!hooks[0]||!hooks[1]){Write({{"event","error"},{"reason","missing hook"}});log.flush();return;}
 const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed)),managerBefore=ReversingProbe::Bytes(&AEUserRadioTrackManager,sizeof(AEUserRadioTrackManager));
 const auto cwd=std::filesystem::current_path();const auto dirBefore=ReversingProbe::Bytes(CFileMgr::ms_dirName,128);const auto indexBefore=FrontEndMenuManager.m_UserTrackIndex;
 std::array<bool,2> enabled{},locked{};for(int fn=0;fn<2;++fn){enabled[fn]=hooks[fn]->Hooked();locked[fn]=hooks[fn]->Locked();hooks[fn]->LockState(false);Write({{"event","hook_state"},{"function",fn},{"enabled",enabled[fn]},{"locked",locked[fn]},{"startup_calls",URProbe::calls[fn]}});}
 bool passed=true;int pairs=0;
 {
 ReversingProbe::EntryPatch dn(0x538860,Documents),dl(reinterpret_cast<uintptr_t>(&CFileMgr::SetDirMyDocuments),Documents),rn(0x5387D0,Root),rl(reinterpret_cast<uintptr_t>(&CFileMgr::SetDir),Root);
 const auto hr=CoInitialize(nullptr);assert(SUCCEEDED(hr));
 for(int fn=0;fn<2&&passed;++fn){currentFunction=fn;
  for(int i=0;i<(fn?32:192)&&passed;++i){currentCase=i;docs=fixture/("case"+std::to_string(i%8));if(i%8==7)std::filesystem::create_directories(docs);
   const auto run=[&](bool candidate){
    CAEUserRadioTrackManager manager{};manager.m_nUserTracksScanState=USER_TRACK_SCAN_IN_PROGRESS;FrontEndMenuManager.m_UserTrackIndex=53;
    if(fn==0){
     auto* file=CFileMgr::OpenFile((fixture/"scan.dat").string().c_str(),"w+b");assert(file);size_t length=std::array<size_t,6>{0,1,4,17,31,123}[(i/32)%6];std::string padding(length,'#');CFileMgr::Write(file,padding.data(),padding.size());
     UserTracksLegacyVector vec;const auto startLength=length;const auto folder=(docs/"User Tracks").string();const int depth=std::array{0,1,14,15}[(i/8)%4];
     auto count=reinterpret_cast<int(__thiscall*)(CAEUserRadioTrackManager*,const char*,UserTracksLegacyVector&,FILE*,size_t&,int)>(0x4F4690)(&manager,folder.c_str(),vec,file,length,depth);
     CFileMgr::CloseFile(file);const auto data=Read(fixture/"scan.dat");assert(data.size()==length&&data.substr(0,startLength)==padding);std::vector<tUserTracksInfo> records;for(auto** p=vec.first;p!=vec.last;++p)records.push_back(**p);assert(count==int(records.size()));auto tracks=Tracks(records,data);Free(vec);return json{{"count",count},{"tracks",tracks},{"padding_intact",true}};
    }
    // Exercise all four worker/scanner combinations using a real bounded thread.
    hooks[0]->State((i/8)%2!=0);Documents();std::filesystem::remove(docs/"sa-ufiles.dat");std::filesystem::remove(docs/"sa-utrax.dat");
    auto thread=CreateThread(nullptr,0,reinterpret_cast<LPTHREAD_START_ROUTINE>(0x4F4A20),&manager,0,nullptr);assert(thread);const auto wait=WaitForSingleObject(thread,10000);if(wait!=WAIT_OBJECT_0){Write({{"event","error"},{"reason","worker timeout"},{"case",i}});log.flush();ExitProcess(94);}DWORD code{};assert(GetExitCodeThread(thread,&code));CloseHandle(thread);assert(code==0);
    auto data=Read(docs/"sa-ufiles.dat"),raw=Read(docs/"sa-utrax.dat");assert(raw.size()%sizeof(tUserTracksInfo)==0);std::vector<tUserTracksInfo> records(raw.size()/sizeof(tUserTracksInfo));if(!raw.empty())std::memcpy(records.data(),raw.data(),raw.size());auto tracks=Tracks(records,data);
    if(candidate){assert(manager.m_bUserTracksLoaded==!records.empty()&&manager.m_bUserTracksLoadedCopy==!records.empty());if(!records.empty()){assert(manager.m_nUserTracksCount==records.size());assert(std::memcmp(manager.m_pUserTracksInfo,records.data(),raw.size())==0);}if(manager.m_pUserTracksInfo)CMemoryMgr::Free(manager.m_pUserTracksInfo);}
    else assert(!manager.m_bUserTracksLoaded&&!manager.m_pUserTracksInfo);
    return json{{"return",code},{"state",int(manager.m_nUserTracksScanState)},{"index",FrontEndMenuManager.m_UserTrackIndex},{"tracks",tracks},{"refresh_contract",true}};
   };
   hooks[fn]->State(false);auto a=URProbe::calls[fn];auto original=run(false);auto b=URProbe::calls[fn];hooks[fn]->State(true);auto candidate=run(true);auto c=URProbe::calls[fn];passed=original==candidate&&a==b&&c==b+1;
   Write({{"event","comparison"},{"function",fn},{"case",i},{"pair",pairs++},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});log.flush();
  }
 }
 CoUninitialize();
 }
 std::filesystem::current_path(cwd);std::memcpy(CFileMgr::ms_dirName,dirBefore.data(),128);FrontEndMenuManager.m_UserTrackIndex=indexBefore;
 for(int fn=0;fn<2;++fn){hooks[fn]->State(enabled[fn]);hooks[fn]->LockState(locked[fn]);}
 bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&managerBefore==ReversingProbe::Bytes(&AEUserRadioTrackManager,sizeof(AEUserRadioTrackManager))&&std::filesystem::current_path()==cwd&&dirBefore==ReversingProbe::Bytes(CFileMgr::ms_dirName,128);
 Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();
}
}
