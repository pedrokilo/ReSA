#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEStreamingChannel.h"
#include "../ProbeRuntime.hpp"
namespace AudioStreamingPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},positive{};inline uintptr_t base{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0x60098> object{};alignas(16) inline std::array<uint8,0x60000> destination{},source{};
inline std::array<uintptr_t,32> comVT{};inline uintptr_t comObject{};inline std::array<uintptr_t,8> decoderVT{};inline std::array<uintptr_t,2> decoderA{},decoderB{};inline std::array<uintptr_t,9> channelVT{};
inline HRESULT __stdcall ComStop(void*){++positive;trace.push_back({"stop"});return S_OK;}
inline HRESULT __stdcall ComPosition(void*,DWORD* cursor,DWORD* write){++positive;if(cursor)*cursor=scenario%3*0x60000u+uint32(scenario%200);if(write)*write=0;trace.push_back({"position",cursor!=nullptr,write!=nullptr});return S_OK;}
inline HRESULT __stdcall ComLock(void*,DWORD offset,DWORD bytes,void** ptr,DWORD* size,void**,DWORD*,DWORD flags){++positive;trace.push_back({"lock",offset,bytes,flags});if(scenario%7==0)return E_FAIL;*ptr=destination.data();*size=bytes ? 0x60000u : 32u+uint32(scenario%9)*4;return S_OK;}
inline HRESULT __stdcall ComUnlock(void*,void*,DWORD size,void*,DWORD rest){++positive;trace.push_back({"unlock",size,rest,Hash(destination.data(),destination.size())});return S_OK;}
inline HRESULT __stdcall ComSetPosition(void*,DWORD value){++positive;trace.push_back({"set_position",value});return S_OK;}
inline int32 __fastcall Rate(void* self,void*){trace.push_back({"rate",uintptr_t(self)==uintptr_t(decoderA.data())?0:1});return scenario%3==0?44100:48000;}
inline void* __fastcall Destroy(void* self,void*,uint32 flags){++positive;trace.push_back({"destroy",uintptr_t(self)==uintptr_t(decoderA.data())?0:1,flags});return self;}
inline uint32 __fastcall Fill(void*,void*,void* buffer,uint32 size){++positive;trace.push_back({"fill",buffer==destination.data(),size});const uint32 written=scenario%4 ? size : size/2;auto* b=static_cast<uint8*>(buffer);for(uint32 i=0;i<written;++i)b[i]=uint8(i*17+scenario);return written;}
inline bool __fastcall Fade(void*,void*,IDirectSoundBuffer*,float volume,int16 duration,bool stop){++positive;trace.push_back({"fade",F(volume),duration,stop});return scenario%2;}
inline void __fastcall Frequency(CAEAudioChannel* self,void*,uint32 rate){++positive;trace.push_back({"frequency",rate});self->m_nOriginalFrequency=rate;}
inline void __fastcall Status(CAEAudioChannel* self,void*){if(uintptr_t(self)!=base)return;++positive;trace.push_back({"status"});self->m_nBufferStatus=(scenario/7)%3;}
inline void __fastcall ChannelStop(void*,void*,bool update){++positive;trace.push_back({"channel_stop",update});}
inline int32 __fastcall Prepare(void*,void*,CAEStreamingDecoder* decoder,int8 flags,bool stop){++positive;trace.push_back({"prepare",decoder==reinterpret_cast<CAEStreamingDecoder*>(decoderB.data()),int(flags),stop});return 0;}
inline void __fastcall Play(void*,void*,int16 offset,int8 flags,float speed){++positive;trace.push_back({"play",offset,int(flags),F(speed)});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,3> fns{{{"CAEStreamingChannel","Constructor",0x4F1800,0,-1,512},{"CAEStreamingChannel","PrepareStream",0x4F23D0,0,-1,1024},{"CAEStreamingChannel","Service",0,0x85F3F0,1,1024}}};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio-streaming-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audio-streaming-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio-streaming-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"audio-streaming-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks{};std::array<bool,3> enabled{},locked{};
    for(size_t n=0;n<3;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    uint32 pairs=0,totalFailures=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch a(0x4F1E20,Fill),ad(bit_cast<uintptr_t>(&CAEStreamingChannel::FillBuffer),Fill);
        EntryPatch b(0x4EEAA0,Fade),bd(bit_cast<uintptr_t>(&CAESmoothFadeThread::RequestFade),Fade);
        EntryPatch c(0x4D7A70,Frequency),cd(bit_cast<uintptr_t>(&CAEAudioChannel::SetOriginalFrequency),Frequency);
        EntryPatch d(0x4D7BD0,Status),dd(bit_cast<uintptr_t>(&CAEAudioChannel::UpdateStatus),Status);
        EntryPatch e(0x4F1A90,ChannelStop),ed(bit_cast<uintptr_t>(static_cast<void(CAEStreamingChannel::*)(bool)>(&CAEStreamingChannel::Stop)),ChannelStop);
        comVT.fill(reinterpret_cast<uintptr_t>(&ComStop));comVT[4]=reinterpret_cast<uintptr_t>(&ComPosition);comVT[11]=reinterpret_cast<uintptr_t>(&ComLock);comVT[13]=reinterpret_cast<uintptr_t>(&ComSetPosition);comVT[18]=reinterpret_cast<uintptr_t>(&ComStop);comVT[19]=reinterpret_cast<uintptr_t>(&ComUnlock);comObject=uintptr_t(comVT.data());decoderVT.fill(reinterpret_cast<uintptr_t>(&Rate));decoderVT[6]=reinterpret_cast<uintptr_t>(&Destroy);decoderA[0]=decoderB[0]=uintptr_t(decoderVT.data());channelVT.fill(reinterpret_cast<uintptr_t>(&Play));
        for(size_t n=0;n<3;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;std::unique_ptr<EntryPatch> prepareExe,prepareDll;
            if(n==2){prepareExe=std::make_unique<EntryPatch>(0x4F23D0,Prepare);prepareDll=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CAEStreamingChannel::PrepareStream),Prepare);}
            for(int i=0;i<fn.cases;++i){base=uintptr_t(object.data());
                auto run=[&](bool candidate){scenario=uint32(i);positive=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(destination.data(),0x5A,destination.size());memset(source.data(),0x9B,source.size());auto* self=reinterpret_cast<CAEStreamingChannel*>(object.data());
                    if(n){*reinterpret_cast<uintptr_t*>(object.data())=uintptr_t(channelVT.data());self->m_pDirectSoundBuffer=n==1&&i%13==0?nullptr:reinterpret_cast<IDirectSoundBuffer*>(&comObject);self->m_pStreamingDecoder=i%5?reinterpret_cast<CAEStreamingDecoder*>(decoderA.data()):nullptr;self->m_pNextStreamingDecoder=reinterpret_cast<CAEStreamingDecoder*>(decoderB.data());self->m_nState=static_cast<StreamingChannelState>(-7+i%7);self->m_nOriginalFrequency=48000;self->m_bNeedSwitch=(i/21)%2;self->m_bSilenced=(i/42)%2;self->m_bNeedToFinish=(i/84)%2;self->m_bShouldStop=(i/168)%2;self->m_lastSlot=uint8((i/7)%2);self->m_lastWrittenSlot=0;self->m_lStoppingFrameCount=(i/7)%9;self->m_pBuffer=source.data();if(n==2)self->m_pStreamingDecoder=reinterpret_cast<CAEStreamingDecoder*>(decoderA.data());}
                    hooks[n]->State(candidate);const uintptr_t address=n==2?reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]:fn.address;const bool switches=n==2?((address<0x400000||address>=0x1000000)==candidate):ReversingProbe::JumpsToDll(fn.address)==candidate;if(!switches)return json{{"switch",false}};
                    int32 ret=0;
                    if(n==0){const auto r=reinterpret_cast<void*(__thiscall*)(void*,IDirectSound*,uint16)>(address)(self,reinterpret_cast<IDirectSound*>(0x10000000u+uint32(i)*4),uint16(i*131));ret=int32(uintptr_t(r)-base);*reinterpret_cast<uintptr_t*>(object.data())=0x85F3F0;++positive;}
                    else if(n==1)ret=reinterpret_cast<int32(__thiscall*)(void*,CAEStreamingDecoder*,int8,bool)>(address)(self,i%17==0?nullptr:reinterpret_cast<CAEStreamingDecoder*>(decoderB.data()),int8(i%8),i%2);
                    else reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
                    if(!candidate&&positive)++positiveCases;
                    return json{{"switch",true},{"ret",ret},{"object",Hash(object.data(),object.size())},{"destination",Hash(destination.data(),destination.size())},{"source",Hash(source.data(),source.size())},{"trace",trace}};
                };
                const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;
                Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            }
            if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
        }
    }
    bool restored=true;for(size_t n=0;n<3;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
    Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}
