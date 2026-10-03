#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "FileMgr.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace FMLive {
inline bool done{};inline std::ofstream log;
inline constexpr std::array names{"SetDirMyDocuments","LoadFile","OpenFile","OpenFileForWriting","OpenFileForAppending","Read","Write","Seek","ReadLine","CloseFile","GetTotalSize","Tell","GetErrorReadWrite"};
inline constexpr std::array<uintptr_t,13> addresses{0x538860,0x538890,0x538900,0x538910,0x538930,0x538950,0x538970,0x538990,0x5389B0,0x5389D0,0x5389E0,0x538A20,0x538A50};
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline void Fixture(const std::filesystem::path& p){std::ofstream f(p,std::ios::binary|std::ios::trunc);for(int i=0;i<256;++i)f.put(char(i%29==0?'\n':i%29==28?'\r':32+i%91));}
inline json Disk(const std::filesystem::path& p){std::ifstream f(p,std::ios::binary);return std::vector<uint8>(std::istreambuf_iterator<char>(f),{});}
inline json State(FILESTREAM f){const auto* d=reinterpret_cast<const uint32*>(f);return json{{"position",CFileMgr::Tell(f)},{"count",d[1]},{"flags",d[3]},{"buffer_size",d[6]}};}
using Open=FILESTREAM(__cdecl*)(const char*,const char*);using OpenOne=FILESTREAM(__cdecl*)(const char*);
using Read=size_t(__cdecl*)(FILESTREAM,void*,size_t);using WriteFile=size_t(__cdecl*)(FILESTREAM,const void*,size_t);
using Seek=bool(__cdecl*)(FILESTREAM,long,int);using Tell=int(__cdecl*)(FILESTREAM);using Close=int(__cdecl*)(FILESTREAM);
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto game=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(game/"unlock-file-mgr-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(game/"unlock-file-mgr-runtime.jsonl");Write({{"event","waiting_for_player"}});}
 auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
 using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,13> hooks{};std::array<bool,13> enabled{},locked{};
 ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CFileMgr")for(auto& h:c.Items())for(int i=0;i<13;++i)if(h->Name()==names[i])hooks[i]=h.get();});
 for(auto* h:hooks)if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}
 const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));const auto cwd=std::filesystem::current_path();
 const auto dirBefore=ReversingProbe::Bytes(CFileMgr::ms_dirName,128);const auto userBefore=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC92368),256);
 const auto folder=std::filesystem::path(__FILE__).parent_path()/"artifacts";const auto file=folder/"fixture.txt";const auto path=file.string();
 const auto tempUser=folder.string();assert(tempUser.size()<128);std::strcpy(reinterpret_cast<char*>(0xC92368),tempUser.c_str());
 for(int i=0;i<13;++i){enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();hooks[i]->LockState(false);Write({{"event","hook_state"},{"function",i},{"enabled",enabled[i]},{"locked",locked[i]}});}
 bool passed=true;int pairs=0;
 for(int fn=0;fn<13&&passed;++fn)for(int i=0;i<128&&passed;++i){
  auto run=[&]{
   std::filesystem::current_path(cwd);std::memcpy(CFileMgr::ms_dirName,dirBefore.data(),128);Fixture(file);
   std::array<uint8,1024> bytes;bytes.fill(0xA5);json r;FILESTREAM stream{};
   if(fn==0){using F=int(__cdecl*)();r["return"]=reinterpret_cast<F>(addresses[fn])();r["cwd"]=std::filesystem::current_path().string();r["dir"]=ReversingProbe::Bytes(CFileMgr::ms_dirName,128);return r;}
   if(fn==1){using F=size_t(__cdecl*)(const char*,uint8*,size_t,const char*);const auto p=i%7?path:path+".missing";r["return"]=reinterpret_cast<F>(addresses[fn])(p.c_str(),bytes.data(),bytes.size(),i&1?"rb":"r");r["bytes"]=bytes;return r;}
   if(fn==2){const auto p=i%7?path:path+".missing";stream=reinterpret_cast<Open>(addresses[fn])(p.c_str(),std::array{"rb","r","r+b","wb","a","a+b","w+b","rb"}[i%8]);r["opened"]=stream!=nullptr;}
   else if(fn==3||fn==4){stream=reinterpret_cast<OpenOne>(addresses[fn])(path.c_str());r["opened"]=stream!=nullptr;}
   else {stream=CFileMgr::OpenFile(path.c_str(),i%5==0?"rb":"r+b");assert(stream);CFileMgr::Seek(stream,long(i%19),SEEK_SET);}
   if(stream){
    if(fn==5)r["return"]=reinterpret_cast<Read>(addresses[fn])(stream,bytes.data(),size_t(std::array{0,1,15,255,256,257,511,1024}[i%8]));
    if(fn==6)r["return"]=reinterpret_cast<WriteFile>(addresses[fn])(stream,bytes.data(),size_t(std::array{0,1,15,255}[i%4]));
    if(fn==7)r["return"]=reinterpret_cast<Seek>(addresses[fn])(stream,long(std::array{-512,-1,0,1,15,256,512,1024}[i%8]),i/8%4);
    if(fn==8){using F=bool(__cdecl*)(FILESTREAM,char*,int);r["return"]=reinterpret_cast<F>(addresses[fn])(stream,reinterpret_cast<char*>(bytes.data()),std::array{0,1,2,16,256,512,1024,8}[i%8]);}
    if(fn==10||fn==11)r["return"]=reinterpret_cast<Tell>(addresses[fn])(stream);
    if(fn==12){auto* flags=reinterpret_cast<uint32*>(stream)+3;const auto before=*flags;*flags=(before&~0x20)|(i&1?0x20:0);using F=uint32(__cdecl*)(FILESTREAM);r["return"]=reinterpret_cast<F>(addresses[fn])(stream);*flags=before;}
    if(fn!=9)r["stream"]=State(stream);
    r["bytes"]=bytes;
    if(fn==9)r["return"]=reinterpret_cast<Close>(addresses[fn])(stream);else CFileMgr::CloseFile(stream);
   }
   r["disk"]=Disk(file);return r;
  };
  hooks[fn]->State(false);const auto a=FMProbe::calls[fn];const auto original=run();const auto b=FMProbe::calls[fn];
  hooks[fn]->State(true);const auto candidate=run();const auto c=FMProbe::calls[fn];
  // Tell is used for the common state snapshot; subtract that single shared call.
  const unsigned extra=fn==11?1:0;
  passed=original==candidate&&b==a+extra&&c==b+1+extra;
  Write({{"event","comparison"},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a-extra},{"candidate_calls",c-b-extra}});++pairs;
  hooks[fn]->State(enabled[fn]);
 }
 json baseline;
 for(int mask=0;mask<64&&passed;++mask){
  constexpr std::array indices{2,5,6,7,11,9};for(int bit=0;bit<6;++bit)hooks[indices[bit]]->State((mask&(1<<bit))!=0);
  Fixture(file);auto* stream=reinterpret_cast<Open>(0x538900)(path.c_str(),"r+b");assert(stream);
  std::array<uint8,32> bytes{};const auto read=reinterpret_cast<Read>(0x538950)(stream,bytes.data(),bytes.size());
  const auto seek=reinterpret_cast<Seek>(0x538990)(stream,7,SEEK_SET);const auto written=reinterpret_cast<WriteFile>(0x538970)(stream,bytes.data(),bytes.size());
  const auto pos=reinterpret_cast<Tell>(0x538A20)(stream);const auto closed=reinterpret_cast<Close>(0x5389D0)(stream);
  json state{{"read",read},{"seek",seek},{"written",written},{"pos",pos},{"closed",closed},{"bytes",bytes},{"disk",Disk(file)}};
  if(!mask)baseline=state;passed=state==baseline&&read==32&&written==32&&closed==0;Write({{"event","interop"},{"mask",mask},{"passed",passed},{"state",state}});
 }
 if(passed){
  const auto unicode=folder/std::filesystem::path(L"prueba-\u00f1-\u4e2d.txt");const auto utf8=UnicodeToUTF8(unicode.wstring());auto* f=CFileMgr::OpenFile(utf8.c_str(),"w+b");
  const std::array<uint8,4> expected{0,10,128,255};std::array<uint8,4> got{};bool unicodePass=f!=nullptr;
  if(f){unicodePass=reinterpret_cast<WriteFile>(0x538970)(f,expected.data(),4)==4&&!reinterpret_cast<Seek>(0x538990)(f,0,SEEK_SET)&&reinterpret_cast<Read>(0x538950)(f,got.data(),4)==4&&got==expected;reinterpret_cast<Close>(0x5389D0)(f);}
  Fixture(file);std::array<uint8,32> bounded;bounded.fill(0xA5);const auto n=CFileMgr::LoadFile(path.c_str(),bounded.data(),16,"rb");const bool boundedPass=n==16&&std::ranges::all_of(bounded|std::views::drop(16),[](auto b){return b==0xA5;});
  passed=unicodePass&&boundedPass;Write({{"event","extensions"},{"unicode",unicodePass},{"bounded_load",boundedPass},{"passed",passed}});
 }
 for(int i=0;i<13;++i){hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);}
 std::filesystem::current_path(cwd);std::memcpy(CFileMgr::ms_dirName,dirBefore.data(),128);std::memcpy(reinterpret_cast<void*>(0xC92368),userBefore.data(),256);
 const bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&std::filesystem::current_path()==cwd;Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
}
}
