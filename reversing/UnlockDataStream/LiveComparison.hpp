#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEDataStream.h"
#include "AEStreamTransformer.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"

namespace DSLive {
inline bool done{};
inline std::ofstream log;
inline constexpr std::array names{"Constructor", "Destructor", "Initialise", "FillBuffer", "GetCurrentPosition", "Seek-OG", "Stat"};
inline constexpr std::array counts{256,256,512,2048,512,2048,512};
inline void Write(const json& row) { log << row.dump() << '\n'; log.flush(); }
using Constructor = CAEDataStream*(__thiscall*)(CAEDataStream*,int,char*,uint32,uint32,int);
using Destructor = void(__thiscall*)(CAEDataStream*);
using Initialise = bool(__thiscall*)(CAEDataStream*);
using Fill = size_t(__thiscall*)(CAEDataStream*,void*,size_t);
using Position = uint32(__thiscall*)(CAEDataStream*);
using Seek = uint32(__thiscall*)(CAEDataStream*,long,int);
using Stat = HRESULT(__stdcall*)(CAEDataStream*,STATSTG*,DWORD);
inline json ObjectState(CAEDataStream* stream, bool preserveHandle=false) {
    auto bytes=ReversingProbe::Bytes(stream,sizeof(*stream));
    std::fill_n(bytes.begin(),4,0); // Original and C++ IStream vtables have different addresses.
    if(!preserveHandle) std::fill_n(bytes.begin()+4,4,0);
    std::fill_n(bytes.begin()+8,4,0); // Owned filename allocations are independent.
    return json{{"bytes",bytes},{"filename_null",stream->m_pszFilename==nullptr},
        {"handle_invalid",stream->m_pFileHandle==INVALID_HANDLE_VALUE}};
}
inline HANDLE Open(const std::filesystem::path& path) {
    return CreateFileW(path.c_str(),GENERIC_READ,FILE_SHARE_READ,nullptr,OPEN_EXISTING,0,nullptr);
}
inline bool HandleAlive(HANDLE handle) {
    DWORD flags{};return GetHandleInformation(handle,&flags)!=FALSE;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);
    const auto game=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(game/"unlock-data-stream-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(game/"unlock-data-stream-runtime.jsonl");Write({{"event","waiting_for_player"}});}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
    using Hook=ReversibleHooks::ReversibleHook::Base;
    std::array<Hook*,7> hooks{};std::array<bool,7> enabled{},locked{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){if(category.Name()=="CAEDataStream")for(auto& hook:category.Items())for(size_t i=0;i<names.size();++i)if(hook->Name()==names[i])hooks[i]=hook.get();});
    for(auto* hook:hooks)if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}
    const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed));
    const auto transformerBefore=ReversingProbe::Bytes(&AEStreamTransformer,sizeof(AEStreamTransformer));
    const auto cwd=std::filesystem::current_path();
    const auto fixture=std::filesystem::path(__FILE__).parent_path()/"artifacts/input.bin";
    {std::ofstream file(fixture,std::ios::binary);for(unsigned i=0;i<512;++i)file.put(char((i*73+29)&255));}
    for(size_t i=0;i<hooks.size();++i){enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();hooks[i]->LockState(false);Write({{"event","hook_state"},{"function",i},{"enabled",enabled[i]},{"locked",locked[i]},{"startup_calls",DSProbe::calls[i]}});}
    bool passed=true;int pairs=0;
    for(int fn=0;fn<7&&passed;++fn)for(int i=0;i<counts[fn]&&passed;++i){
        auto run=[&]{
            alignas(CAEDataStream) std::array<uint8,sizeof(CAEDataStream)> storage;storage.fill(uint8(0x80+i%73));
            auto* stream=reinterpret_cast<CAEDataStream*>(storage.data());
            const auto path=(fn==2 && i%7==0)?fixture.string()+".missing":fixture.string();
            auto* filename=new char[path.size()+1];std::memcpy(filename,path.c_str(),path.size()+1);
            const auto start=uint32(std::array{0,1,31,128,511,512,513,7}[i%8]);
            const auto length=uint32(std::array{0,1,16,128,256,512}[i/8%6]);
            HANDLE owned=INVALID_HANDLE_VALUE;
            if(fn==0){
                stream->m_pFileHandle=reinterpret_cast<HANDLE>(0x12345678);
                auto* result=reinterpret_cast<Constructor>(0x4DC620)(stream,i-100,filename,start,length,i&1);
                auto state=ObjectState(stream,true);state["returned_this"]=result==stream;delete[] filename;return state;
            }
            new(stream) CAEDataStream(i-100,filename,start,length,i&1);
            stream->m_pFileHandle=reinterpret_cast<HANDLE>(0x12345678);
            stream->m_bIsOpen=fn==2 ? i%11==0 : i%3!=0;
            if(fn!=2 || stream->m_bIsOpen){owned=Open(fixture);assert(owned!=INVALID_HANDLE_VALUE);stream->m_pFileHandle=owned;SetFilePointer(owned,start,nullptr,FILE_BEGIN);}
            stream->m_nCurrentPosition=start+uint32(i%13);
            json result;
            if(fn==1){
                reinterpret_cast<Destructor>(0x4DC490)(stream);
                result=ObjectState(stream);result["handle_alive"]=HandleAlive(owned);
                if(HandleAlive(owned))CloseHandle(owned);
                return result;
            }
            if(fn==2)result["return"]=reinterpret_cast<Initialise>(0x4DC2B0)(stream);
            if(fn==3){
                std::array<uint8,1536> buffer;buffer.fill(0xA5);
                const auto request=size_t(std::array{0,1,2,15,128,511,512,513,1024}[i%9]);
                result["return"]=reinterpret_cast<Fill>(0x4DC1C0)(stream,buffer.data(),request);result["buffer"]=buffer;
            }
            if(fn==4)result["return"]=reinterpret_cast<Position>(0x4DC230)(stream);
            if(fn==5){const auto delta=long(std::array{-1024,-512,-1,0,1,17,128,512,1024}[i%9]);result["return"]=reinterpret_cast<Seek>(0x4DC250)(stream,delta,i/9%4);}
            if(fn==6){
                STATSTG status;std::memset(&status,0xA7,sizeof(status));
                if(i%8==0){CloseHandle(owned);owned=INVALID_HANDLE_VALUE;stream->m_pFileHandle=INVALID_HANDLE_VALUE;}
                result["return"]=reinterpret_cast<Stat>(0x4DC3A0)(stream,i%7?&status:nullptr,i%3);result["stat"]=ReversingProbe::Bytes(&status,sizeof(status));
            }
            result["object"]=ObjectState(stream);
            result["file_position"]=SetFilePointer(stream->m_pFileHandle,0,nullptr,FILE_CURRENT);
            if(HandleAlive(stream->m_pFileHandle))CloseHandle(stream->m_pFileHandle);
            delete[] filename;
            return result;
        };
        hooks[fn]->State(false);const auto a=DSProbe::calls[fn];const auto original=run();const auto b=DSProbe::calls[fn];
        hooks[fn]->State(true);const auto candidate=run();const auto c=DSProbe::calls[fn];
        passed=original==candidate&&a==b&&c==b+1;
        Write({{"event","comparison"},{"function",fn},{"case",i},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});++pairs;
        hooks[fn]->State(enabled[fn]);
    }
    json baseline;
    for(int mask=0;mask<64&&passed;++mask){
        for(int i=0;i<6;++i)hooks[i]->State((mask&(1<<i))!=0);
        alignas(CAEDataStream) std::array<uint8,sizeof(CAEDataStream)> storage{};auto* stream=reinterpret_cast<CAEDataStream*>(storage.data());
        const auto path=fixture.string();auto* filename=new char[path.size()+1];std::memcpy(filename,path.c_str(),path.size()+1);
        reinterpret_cast<Constructor>(0x4DC620)(stream,17,filename,31,128,1);
        const bool open=reinterpret_cast<Initialise>(0x4DC2B0)(stream);auto handle=stream->m_pFileHandle;
        auto position=reinterpret_cast<Seek>(0x4DC250)(stream,7,SEEK_SET);
        std::array<uint8,32> data{};const auto count=reinterpret_cast<Fill>(0x4DC1C0)(stream,data.data(),data.size());
        const auto end=reinterpret_cast<Position>(0x4DC230)(stream);
        reinterpret_cast<Destructor>(0x4DC490)(stream);
        json state{{"opened",open},{"seek",position},{"count",count},{"data",data},{"position",end},{"closed",!HandleAlive(handle)}};
        if(mask==0)baseline=state;
        passed=state==baseline&&open&&count==data.size()&&!HandleAlive(handle);
        Write({{"event","interop"},{"mask",mask},{"result",state},{"passed",passed}});
    }
    for(size_t i=0;i<hooks.size();++i){hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);}
    const bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&transformerBefore==ReversingProbe::Bytes(&AEStreamTransformer,sizeof(AEStreamTransformer))&&cwd==std::filesystem::current_path();
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});
}
}
