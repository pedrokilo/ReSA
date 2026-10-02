#pragma once
// Banco dentro del motor: CAEAmbienceTrackManager (Reset, IsAmbienceTrackActive, IsAmbienceRadioActive,
// PlaySpecialMissionAmbienceTrack, StopSpecialMissionAmbienceTrack, Service). Original (hooks del lote y
// el de CAEAudioUtility::GetRandomNumberInRange<int> desactivados) frente a candidato sobre un gestor propio;
// AudioEngine/AEAudioHardware, UpdateAmbienceTrackAndVolume, StartTrackPlayback, CheckForPause y el RNG
// base controlados; pausas, menu y tiempo fijados por caso. Se compara el gestor y la traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/managers/AEAmbienceTrackManager.h"
#include "Audio/Hardware/AEAudioHardware.h"
#include "Audio/AudioEngine.h"
#include "../ProbeRuntime.hpp"

namespace AmbienceLive {
inline bool done{}; inline std::ofstream log; inline json trace; inline int seed{}, rngCalls{};
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
alignas(16) inline std::array<uint8,sizeof(CAEAmbienceTrackManager)+16> manager{};
inline bool cutscene{};
inline json Self(const void* p) { return p==manager.data() ? json("manager") : p==&AEAudioHardware ? json("hardware") : json(uintptr_t(p)); }
inline bool __cdecl IsCutscene() { trace.push_back({"is_cutscene_track_active"}); return cutscene; }
inline void __fastcall UpdateAmbience(CAEAmbienceTrackManager* m,void*) { trace.push_back({"update_ambience",Self(m)}); }
inline void __fastcall StartPlayback(const CAEAmbienceTrackManager* m,void*) { trace.push_back({"start_track_playback",Self(m)}); }
inline void __fastcall CheckPause(const CAEAmbienceTrackManager* m,void*) { trace.push_back({"check_for_pause",Self(m)}); }
inline void __fastcall PlayTrack(CAEAudioHardware* h,void*,uint32 t,int n,uint32 off,uint8 f,bool u,bool un) { trace.push_back({"play_track",Self(h),t,n,off,f,u,un}); }
inline void __fastcall SetVolume(CAEAudioHardware* h,void*,int16 c,uint16 id,float v,uint8 x) { trace.push_back({"set_channel_volume",Self(h),c,id,std::bit_cast<uint32>(v),x}); }
inline void __fastcall StopTrack(CAEAudioHardware* h,void*) { trace.push_back({"stop_track",Self(h)}); }
inline void __fastcall StopSound(const CAEAudioHardware* h,void*,int16 c,uint16 s) { trace.push_back({"stop_sound",Self(h),c,s}); }
inline int __cdecl FakeRand() { return int((uint32(seed)*2654435761u+uint32(rngCalls++)*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,6> fns{{{"Reset",0x4D6CA0,64},{"IsAmbienceTrackActive",0x4D6CF0,64},{"IsAmbienceRadioActive",0x4D6D40,64},
                                    {"PlaySpecialMissionAmbienceTrack",0x4D6D50,64},{"StopSpecialMissionAmbienceTrack",0x4D6D60,64},{"Service",0x4D76C0,4096}}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ambience-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ambience-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ambience");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,7> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook("CAEAmbienceTrackManager",fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    if(!(hooks[6]=FindHook("CAEAudioUtility","GetRandomNumberInRange<int32>-int"))&&!(hooks[6]=FindHook("CAEAudioUtility","GetRandomNumberInRange-int"))){
        // nombre exacto segun el registro
        ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CAEAudioUtility")for(auto& h:c.Items())if(h->Name().find("int")!=std::string::npos&&h->Name().find("GetRandom")!=std::string::npos)hooks[6]=h.get();});
        if(!hooks[6]){Write({{"event","error"},{"reason","hook not found"},{"function","CAEAudioUtility::GetRandomNumberInRange<int>"}});return;}
    }
    const auto savedUser=CTimer::m_UserPause, savedCode=CTimer::m_CodePause, savedSwitch=FrontEndMenuManager.m_bMainMenuSwitch; const auto savedTime=CTimer::m_snTimeInMilliseconds;
    std::array<bool,7> enabled{},locked{};
    for(size_t f=0;f<hooks.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    {
        using namespace ReversingProbe;
        EntryPatch p1(std::bit_cast<uintptr_t>(&CAudioEngine::IsCutsceneTrackActive),IsCutscene), p1e(0x507150,IsCutscene);
        EntryPatch p2(std::bit_cast<uintptr_t>(&CAEAmbienceTrackManager::UpdateAmbienceTrackAndVolume),UpdateAmbience), p2e(0x4D6E60,UpdateAmbience);
        EntryPatch p3(std::bit_cast<uintptr_t>(&CAEAmbienceTrackManager::StartTrackPlayback),StartPlayback), p3e(0x4D6DA0,StartPlayback);
        EntryPatch p4(std::bit_cast<uintptr_t>(&CAEAmbienceTrackManager::CheckForPause),CheckPause), p4e(0x4D6DF0,CheckPause);
        EntryPatch p5(std::bit_cast<uintptr_t>(&CAEAudioHardware::PlayTrack),PlayTrack), p5e(0x4D8F10,PlayTrack);
        EntryPatch p6(std::bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelVolume),SetVolume), p6e(0x4D8870,SetVolume);
        EntryPatch p7(std::bit_cast<uintptr_t>(&CAEAudioHardware::StopTrack),StopTrack), p7e(0x4D8F50,StopTrack);
        EntryPatch p8(std::bit_cast<uintptr_t>(&CAEAudioHardware::StopSound),StopSound), p8e(0x4D88E0,StopSound);
        EntryPatch r1(std::bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA3B1u; seed=i; rngCalls=0; trace=json::array();
                    for(auto& b:manager)b=uint8(next());
                    auto* m=reinterpret_cast<CAEAmbienceTrackManager*>(manager.data());
                    m->m_AmbienceStatus=CAEAmbienceTrackManager::AmbienceStatus(next()%11); m->m_StartAmbienceAtBeginning=next()%3==0;
                    m->m_PrevAmbiencePlayTimeMs=next()%3==0 ? -1 : int32(next()%700000); m->m_PrevAmbienceStopTimeMs=next()%3==0 ? 0 : int32(next()%900000);
                    m->m_SpecialMissionAmbienceTrackID=next()%2 ? -1 : int32(next()%5); m->m_RequestedSettings.TrackID=next()%2 ? m->m_SpecialMissionAmbienceTrackID : int32(next()%5);
                    m->m_RequestedSettings.TrackFlags=uint8(next()); m->m_HwClientHandle=int32(next()%40); m->m_Volume=float(int(next()%200)-150)/3.f;
                    m->m_AmbienceRadioStation=next()%2 ? RADIO_INVALID : eRadioID(next()%13); m->m_OverrideRadio=next()%2; m->m_LastAmbienceOverrodeRadio=next()%2;
                    cutscene=next()%5==0; CTimer::m_UserPause=next()%6==0; CTimer::m_CodePause=next()%6==0; FrontEndMenuManager.m_bMainMenuSwitch=next()%6==0;
                    CTimer::m_snTimeInMilliseconds=1000000+next()%500000;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm=fn.name; json ret=nullptr;
                    if(nm.starts_with("Is"))ret=reinterpret_cast<bool(__thiscall*)(void*)>(fn.address)(manager.data());
                    else if(nm=="PlaySpecialMissionAmbienceTrack")reinterpret_cast<void(__thiscall*)(void*,int)>(fn.address)(manager.data(),int(next()%400));
                    else if(nm=="Service"){ static const int times[]{-1,-2,-6,-7,0,1234,999999}; reinterpret_cast<void(__thiscall*)(void*,int32)>(fn.address)(manager.data(),next()%2 ? times[next()%7] : int32(next()%800000)); }
                    else reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(manager.data());
                    return {{"ret",ret},{"manager",ReversingProbe::Bytes(manager.data(),sizeof(CAEAmbienceTrackManager))},{"trace",trace},{"rng",rngCalls},{"switches",switches}};
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CAEAmbienceTrackManager::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
                else Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CAEAmbienceTrackManager::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}});
            }
            Write({{"event","function_summary"},{"function",std::string("CAEAmbienceTrackManager::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<hooks.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    CTimer::m_UserPause=savedUser; CTimer::m_CodePause=savedCode; FrontEndMenuManager.m_bMainMenuSwitch=savedSwitch; CTimer::m_snTimeInMilliseconds=savedTime;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",true}});
}
}
