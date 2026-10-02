#pragma once
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Probe.hpp"

namespace AudioCompletionLive {
using namespace AudioCompletionProbe;
inline bool done{};
inline std::ofstream log;
inline void Write(const json& row) { log<<row.dump()<<'\n';log.flush(); }
inline int Report(int type,char* message,int*) { if(type!=_CRT_ASSERT)return FALSE;Write({{"event","assertion"},{"message",message ? message : ""}});ExitProcess(91); }
inline std::vector<Spec> Cases() {
    std::vector<Spec> cases;
    auto add=[&](Spec s,const char* name) { s.name=std::string(name)+"-"+std::to_string(cases.size());cases.push_back(s); };
    // Each defined zone, sphere priority and box selection; distance zones are
    // sphere-only because the original box path leaves its position uninitialized.
    for(int zone=-2;zone<70;++zone)for(int kind : {1,2,3})for(int flags=0;flags<4;++flags) {
        if(kind==2 && (zone==5 || zone==10 || zone==13))continue;
        Spec s;s.zone=zone;s.zoneKind=kind;s.radio=flags&1;s.status=flags&2 ? 3 : 8;s.track=157;add(s,"audio-zone");
    }
    for(int zone : {-1,4,5,10,13,37,41,52})for(int state=0;state<9;++state)
        for(int track : {143,140,139,157,144,169,174,166,-1})for(float vol : {-41.f,-40.f,-39.9f,-6.f,0.f}) {
        Spec s;s.zone=zone;s.zoneKind=1;s.status=state;s.track=track;s.volume=vol;add(s,"zone-track-transition");
    }
    for(int previous : {-128,-2,-1,0,3,11})for(int current : {-1,0,3,11})for(int flags=0;flags<8;++flags) {
        Spec s;s.previousRadio=previous;s.station=current;s.radio=flags&1;s.special=flags&2 ? 151 : -1;
        s.zoneKind=1;s.zone=flags&4 ? 30 : -1;s.favorite=current;add(s,"radio-handoff");
    }
    for(float sand : {-1.f,0.f,0.001f,0.5f,1.f,2.f})for(int area : {0,1})for(int gates=0;gates<4;++gates)
        for(int zone : {-1,5,13,52})for(int special : {-1,0,157}) {
        Spec s;s.sandstorm=sand;s.area=area;s.playerNoRain=gates&1;s.cameraNoRain=gates&2;s.zone=zone;s.zoneKind=1;
        s.special=special;s.status=3;s.track=167;add(s,"weather-zone-priority");
    }
    for(float underwater : {0.f,0.49999f,0.5f,1.f})for(float atten : {-50.f,-28.001f,-28.f,-22.001f,-22.f,-21.999f,0.f})
        for(int flags=0;flags<8;++flags)for(int state : {0,3,4,8})for(int track : {174,166,167}) {
        Spec s;s.underwater=underwater;s.attenuation=atten;s.riots=flags&1;s.area=flags&2 ? 1 : 0;s.vehicleRadio=flags&4;
        s.status=state;s.track=track;add(s,"water-riot-stop");
    }
    for(float freq : {-1.f,0.f,0.0625f,0.1f,0.25f,0.3f,0.3125f,0.5f,0.75f,1.f,1.1f})
        for(float vol : {-100.f,-40.f,-34.f,-13.f,0.f,14.f,15.f})for(float underwater : {0.f,1.f}) {
        Spec s;s.status=3;s.track=174;s.attenuation=0.f;s.underwater=underwater;s.volume=vol;s.frequency=freq;add(s,"water-fade-pitch");
    }
    for(float z : {1000.f,1371.f,1372.f,1373.f})for(float volume : {-100.f,-40.f,-34.f,-6.f,0.f}) {
        Spec s;s.zone=4;s.zoneKind=1;s.status=3;s.track=143;s.cameraZ=z;s.volume=volume;add(s,"dam-height");
    }
    for(float distance : {0.f,0.01f,123.45f,10000.f})for(float z : {-100.f,0.f,10.f,1372.f}) {
        Spec s;s.waterDistance=distance;s.cameraZ=z;s.waterZ=3.456f;add(s,"water-distance");
    }
    for(int special : {0,138,174})for(int state=0;state<9;++state)for(int track : {-1,0,138,174}) {
        Spec s;s.special=special;s.status=state;s.track=track;add(s,"mission-track");
    }
    for(int gates=0;gates<16;++gates)for(int event : {-1,0,140,141,142,32767}) {
        Spec s;s.function=2;s.event=event;s.area=gates&1;s.playerNoRain=gates&2;s.cameraNoRain=gates&4;s.loaded=gates&8;add(s,"thunder-gates");
    }
    for(int counter : {-1,0,1,2,3,126,127})for(uint32 duration : {0u,1u,10u,20u,100u,0x7FFFFFFFu,0x80000000u,0xFFFFFFFFu})
        for(int volume : {-128,-20,-3,0,127})for(bool request : {false,true}) {
        Spec s;s.function=2;s.counter=counter;s.lightning=duration;s.defaultVolume=volume;s.requestSucceeds=request;add(s,"thunder-voices");
    }
    for(uint32 size : {0u,1u,8000u,65536u,100001u})for(uint16 freq : {uint16(0),uint16(1),uint16(22050),uint16(65535)})
        for(int old=0;old<4;++old)for(bool hardware : {false,true}) {
        Spec s;s.function=1;s.size=size;s.sampleFrequency=freq;s.oldBuffers=old;s.hardware=hardware;add(s,"pcm-size-frequency");
    }
    for(uint32 size : {64u,7999u,24000u,24001u,70000u})for(int16 loop : {int16(-1),int16(0),int16(1),int16(15),int16(30)})
        for(int failure=0;failure<7;++failure)for(int old=0;old<4;++old)for(bool hardware : {false,true}) {
        Spec s;s.function=1;s.size=size;s.loop=loop;s.failure=failure;s.oldBuffers=old;s.hardware=hardware;add(s,"pcm-loop-failures");
    }
    for(uint32 ms : {0u,32767u,32768u,65535u,65536u,0xFFFFFFFFu})for(int16 loop : {int16(100),int16(2000),int16(3000),int16(3999)}) {
        Spec s;s.function=1;s.loop=loop;s.convertResult=ms;s.f88=32767;s.f8c=-32768;add(s,"pcm-loop-layout");
    }
    return cases;
}
struct Fixture {
    CAEAmbienceTrackManager manager{};
    alignas(CAEWeatherAudioEntity) std::array<uint8,sizeof(CAEWeatherAudioEntity)> weatherBytes{};
    alignas(CAEStaticChannel) std::array<uint8,sizeof(CAEStaticChannel)> channelBytes{};
    std::array<int8,142> volumes{};
    std::vector<uint8> input, destination;
    ComObject device{deviceTable.data(),1},oldBuffer{bufferTable.data(),2},old3D{bufferTable.data(),3},buffer{bufferTable.data(),4},buffer3D{bufferTable.data(),5};
    void SetUp() {
        trace=json::array();
        CGame::currArea=eAreaCodes(spec.area);CWeather::Sandstorm=spec.sandstorm;CWeather::UnderWaterness=spec.underwater;CWeather::LightningDuration=spec.lightning;
        TheCamera.m_matrix=nullptr;TheCamera.m_placement.m_vPosn=CVector{0.f,0.f,spec.cameraZ};TheCamera.m_fHeightOfNearestWater=spec.waterZ;TheCamera.m_fDistanceToWater=spec.waterDistance;
        CAudioZones::m_NumActiveSpheres=spec.zoneKind==1 || spec.zoneKind==3 ? 1 : 0;
        CAudioZones::m_NumActiveBoxes=spec.zoneKind==2 || spec.zoneKind==3 ? 1 : 0;
        CAudioZones::m_aActiveSpheres[0]=0;CAudioZones::m_aActiveBoxes[0]=0;
        CAudioZones::m_aSpheres[0].m_nAudioZone=int16(spec.zone);CAudioZones::m_aSpheres[0].m_Sphere.m_vecCenter=spec.spherePosition;
        CAudioZones::m_aBoxes[0].m_nAudioZone=int16(spec.zoneKind==3 ? 52 : spec.zone);
        manager={};ambience=&manager;manager.m_AmbienceStatus=CAEAmbienceTrackManager::AmbienceStatus(spec.status);
        manager.m_AmbienceRadioStation=eRadioID(spec.previousRadio);manager.m_SpecialMissionAmbienceTrackID=spec.special;
        manager.m_RequestedSettings.TrackID=spec.track;manager.m_RequestedSettings.PlayingTrackID=989;manager.m_RequestedSettings.TrackFlags=7;
        manager.m_Volume=spec.volume;manager.m_FreqFactor=spec.frequency;manager.m_LastAmbienceOverrodeRadio=spec.lastOverride;
        manager.m_OverrideRadio=spec.overrideRadio;manager.m_StartAmbienceAtBeginning=spec.startAtBeginning;
        manager.m_HwClientHandle=123;manager.m_PrevAmbiencePlayTimeMs=987;manager.m_PrevAmbienceStopTimeMs=654;
        weatherBytes.fill(0x5A);weather=reinterpret_cast<CAEWeatherAudioEntity*>(weatherBytes.data());weather->m_nThunderFrequencyVariationCounter=int8(spec.counter);
        volumes.fill(0);volumes[141]=int8(spec.defaultVolume);StaticRef<int8*>(0xBD00F8)=volumes.data();
        channelBytes.fill(0xA5);channel=reinterpret_cast<CAEStaticChannel*>(channelBytes.data());
        channel->m_pDirectSound=reinterpret_cast<IDirectSound*>(&device);
        channel->m_pDirectSoundBuffer=spec.oldBuffers&1 ? reinterpret_cast<IDirectSoundBuffer*>(&oldBuffer) : nullptr;
        channel->m_pDirectSound3DBuffer=spec.oldBuffers&2 ? reinterpret_cast<IDirectSound3DBuffer*>(&old3D) : nullptr;
        channel->m_IsHardwareMixAvailable=spec.hardware;
        input.resize(100033);for(size_t i=0;i<input.size();++i)input[i]=uint8(i*71+13);
        channel->m_pBuffer=input.data();newBuffer=&buffer;new3D=&buffer3D;output=&destination;destination.clear();g_numSoundChannelsUsed=spec.channels;
    }
    json Run() {
        SetUp();const auto originalInput=input;const auto originalWeather=weatherBytes;
        bool result{};json state;
        if(spec.function==0) {
            reinterpret_cast<void(__thiscall*)(CAEAmbienceTrackManager*)>(0x4D6E60)(&manager);
            state=ReversingProbe::Bytes(&manager,sizeof(manager));
        } else if(spec.function==1) {
            result=reinterpret_cast<bool(__thiscall*)(CAEStaticChannel*,const void*,uint32,int16,int16,int16,uint16)>(0x4F0C40)(channel,input.data(),spec.size,spec.f88,spec.f8c,spec.loop,spec.sampleFrequency);
            auto bytes=channelBytes;
            for(int offset : {4,8,12,132})std::memset(bytes.data()+offset,0,4);
            state={{"bytes",bytes},{"buffer",channel->m_pDirectSoundBuffer==reinterpret_cast<IDirectSoundBuffer*>(&oldBuffer) ? 2 : channel->m_pDirectSoundBuffer==reinterpret_cast<IDirectSoundBuffer*>(&buffer) ? 4 : 0},
                {"buffer3D",channel->m_pDirectSound3DBuffer==reinterpret_cast<IDirectSound3DBuffer*>(&old3D) ? 3 : channel->m_pDirectSound3DBuffer==reinterpret_cast<IDirectSound3DBuffer*>(&buffer3D) ? 5 : 0},
                {"source",channel->m_pBuffer==input.data()},{"output",destination},{"channels",g_numSoundChannelsUsed}};
        } else {
            reinterpret_cast<void(__thiscall*)(CAEWeatherAudioEntity*,eAudioEvents)>(0x506800)(weather,eAudioEvents(spec.event));
            state=weatherBytes;
        }
        auto expectedWeather=originalWeather;expectedWeather[0x7C]=weatherBytes[0x7C];
        return {{"result",result},{"state",state},{"calls",trace},{"inputs_intact",input==originalInput && weatherBytes==expectedWeather}};
    }
};
struct SavedRegion {
    void* pointer;std::vector<uint8> bytes;
    SavedRegion(void* p,size_t n):pointer(p),bytes(ReversingProbe::Bytes(p,n)){}
    void Restore() const {std::memcpy(pointer,bytes.data(),bytes.size());}
    bool Intact() const {return bytes==ReversingProbe::Bytes(pointer,bytes.size());}
};
inline BOOL CALLBACK CloseOwnWindow(HWND hwnd,LPARAM) {
    DWORD pid{};GetWindowThreadProcessId(hwnd,&pid);char name[128]{};GetClassNameA(hwnd,name,128);
    if(pid==GetCurrentProcessId() && std::strcmp(name,"Grand theft auto San Andreas")==0)PostMessage(hwnd,WM_CLOSE,0,0);return TRUE;
}
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio-completion-runtime.start")){done=true;return;}
    if(!FindPlayerPed())return;
    done=true;log.open(dir/"audio-completion-runtime.jsonl");auto oldReport=_CrtSetReportHook(Report);
    const std::array<const char*,3> names{"UpdateAmbienceTrackAndVolume","SetAudioBuffer","AddAudioEvent"};
    const std::array<const char*,3> classes{"CAEAmbienceTrackManager","CAEStaticChannel","CAEWeatherAudioEntity"};
    const std::array<uintptr_t,3> addresses{0x4D6E60,0x4F0C40,0x506800};
    const std::array<std::vector<uint8>,3> entries{{{0x83,0xEC,0x24,0x53,0x55},{0x83,0xEC,0x30,0x53,0x55},{0x6A,0xFF,0x68,0x48,0xC5}}};
    std::array<ReversibleHooks::ReversibleHook::Base*,3> hooks{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){for(size_t i=0;i<3;++i)if(category.Name()==classes[i])for(auto& h:category.Items())if(h->Name()==names[i])hooks[i]=h.get();});
    if(std::ranges::any_of(hooks,[](auto p){return p==nullptr;})){Write({{"event","error"},{"reason","hook missing"}});_CrtSetReportHook(oldReport);return;}
    std::array<bool,3> enabled{},locked{};for(size_t i=0;i<3;++i){enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();hooks[i]->LockState(false);hooks[i]->State(false);}
    std::vector<SavedRegion> saved;
    auto save=[&](auto& value){saved.emplace_back(&value,sizeof(value));};
    save(TheCamera);save(CGame::currArea);save(CWeather::Sandstorm);save(CWeather::UnderWaterness);save(CWeather::LightningDuration);
    save(CAudioZones::m_NumActiveSpheres);save(CAudioZones::m_NumActiveBoxes);save(CAudioZones::m_aActiveSpheres);save(CAudioZones::m_aActiveBoxes);
    save(CAudioZones::m_aSpheres);save(CAudioZones::m_aBoxes);save(StaticRef<int8*>(0xBD00F8));save(g_numSoundChannelsUsed);
    bool passed=true;int count{};json counts={{names[0],0},{names[1],0},{names[2],0}};
    active=true;SetTables();const auto cases=Cases();
    {
        std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> patches;
        auto patch=[&](uintptr_t address,auto provider){patches.push_back(std::make_unique<ReversingProbe::EntryPatch>(address,provider));};
        patch(0x72DDC0,&PlayerNoRain);patch(0x72DDB0,&CamNoRain);patch(0x506FD0,&RadioOnABI);patch(0x507050,&VehicleRadioABI);
        patch(0x507040,&StationABI);patch(0x507DC0,&StartABI);patch(0x506F70,&StopABI);patch(0x506EA0,&FrontendABI);
        patch(0x558FA0,&Favorite);patch(0x441C10,&Riots);patch(0x4D7F20,&Attenuation);patch(0x4D80B0,&RelativeABI);patch(0x4D6CC0,&StopAmbienceABI);
        patch(0x4D88C0,&LoadedABI);patch(0x4D88A0,&LoadABI);patch(0x4EFE50,&InitialiseABI);patch(0x4EFB10,&RequestABI);patch(0x4D79D0,&BytesToMSABI);
        Fixture a,b;
        for(const auto& test:cases) {
            spec=test;const auto i=size_t(spec.function);auto* hook=hooks[i];auto& calls=i==0 ? ambienceCalls : i==1 ? staticCalls : weatherCalls;
            hook->State(false);const auto before=calls;
            const bool originalEntry=ReversingProbe::Bytes(reinterpret_cast<void*>(addresses[i]),5)==entries[i];
            Write({{"event","begin"},{"pair",count},{"function",names[i]},{"case",spec.name}});
            const auto original=a.Run();const auto middle=calls;hook->State(true);const auto candidate=b.Run();const auto after=calls;hook->State(false);
            passed=original==candidate && original["inputs_intact"]==true && middle==before && after==middle+1 && originalEntry;
            Write({{"event","comparison"},{"pair",count++},{"function",names[i]},{"case",spec.name},{"equal",passed},{"original",original},{"candidate",candidate},
                {"original_candidate_calls",middle-before},{"candidate_calls",after-middle},{"original_entry",originalEntry}});
            counts[names[i]]=counts[names[i]].get<int>()+1;if(!passed)break;
        }
    }
    for(const auto& region:saved)region.Restore();
    bool restored=std::ranges::all_of(saved,[](const auto& region){return region.Intact();});
    for(size_t i=0;i<3;++i){hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);restored=restored && hooks[i]->Hooked()==enabled[i] && hooks[i]->Locked()==locked[i];}
    active=false;Write({{"event",passed&&restored ? "complete" : "error"},{"pairs",count},{"expected",cases.size()},{"function_pairs",counts},{"state_restored",restored}});
    _CrtSetReportHook(oldReport);EnumWindows(CloseOwnWindow,0);
}
}
