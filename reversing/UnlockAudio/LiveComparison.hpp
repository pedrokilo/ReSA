#pragma once
#include <fstream>
#include <crtdbg.h>
#include <rtcapi.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEAudioEnvironment.h"
#include "Probe.hpp"
#include "../ProbeSupport.hpp"
namespace UALive {
inline bool done{};inline std::ofstream log;inline CPlayerPed* fakePlayer{};
inline void Write(const json& r){ ReversingProbe::PumpMessages();log<<r.dump()<<'\n';}
inline CPlayerPed* Player(int32){return fakePlayer;}
template<class T>struct Raw{alignas(T) std::array<uint8,sizeof(T)> b{};T* get(){return reinterpret_cast<T*>(b.data());}};
inline int __cdecl Error(int,const wchar_t*,int,const wchar_t*,const wchar_t*,...){Write({{"event","error"},{"reason","runtime check"}});log.flush();ExitProcess(92);}
inline void Update(){
    if(done)return;_set_error_mode(_OUT_TO_STDERR);_set_abort_behavior(0,_WRITE_ABORT_MSG|_CALL_REPORTFAULT);
    char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"unlock-audio-runtime.start")){done=true;return;}
    if(!log.is_open()){_RTC_SetErrorFuncW(Error);log.open(dir/"unlock-audio-runtime.jsonl");Write({{"event","waiting_for_player"}});log.flush();}
    auto* player=FindPlayerPed();if(!player||CCutsceneMgr::IsRunning())return;done=true;
    using Hook=ReversibleHooks::ReversibleHook::Base;std::array<Hook*,2> hooks{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CAEAudioEnvironment")for(auto& h:c.Items()){if(h->Name()=="GetPositionRelativeToCamera-vec")hooks[0]=h.get();if(h->Name()=="GetPositionRelativeToCamera-placeable")hooks[1]=h.get();}});
    if(!hooks[0]||!hooks[1]){Write({{"event","error"},{"reason","missing hook"}});log.flush();return;}
    const auto playerBefore=ReversingProbe::Bytes(player,sizeof(CPlayerPed)),cameraBefore=ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera));
    std::array<bool,2> enabled{},locked{};for(int n=0;n<2;++n){enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    Raw<CPlayerPed> ped;Raw<CMatrixLink> camMatrix,pedMatrix;Raw<CPlaceable> place;bool passed=true;int pairs=0;
    {
    ReversingProbe::EntryPatch pn(0x56E210,Player),pl(reinterpret_cast<uintptr_t>(&FindPlayerPed),Player);
    for(int fn=0;fn<2&&passed;++fn){
        Write({{"event","hook_state"},{"function",fn},{"enabled",enabled[fn]},{"locked",locked[fn]},{"startup_calls",UAProbe::calls[fn]}});
        for(int i=0;i<(fn?1024:8192)&&passed;++i){
            const auto run=[&]{
                ped.b.fill(0);camMatrix.b.fill(0);pedMatrix.b.fill(0);place.b.fill(0);
                TheCamera.m_nActiveCam=0;CCamera::GetActiveCamera().m_nMode=static_cast<eCamMode>(std::array{0,7,8,16,4}[i%5]);
                *reinterpret_cast<CMatrixLink**>(reinterpret_cast<uint8*>(&TheCamera)+0x14)=nullptr;
                TheCamera.SetPosn(CVector{float(i%71)*.125f,float(i%37)*.25f,-float(i%11)*.5f});
                if(i&1){camMatrix.get()->GetPosition()=TheCamera.GetPosition();*reinterpret_cast<CMatrixLink**>(reinterpret_cast<uint8*>(&TheCamera)+0x14)=camMatrix.get();}
                ped.get()->SetPosn(TheCamera.GetPosition()+CVector{float(i%13)*.125f,float(i%3)*.125f,float(i%5)*.0625f});
                if(i&2){pedMatrix.get()->GetPosition()=ped.get()->GetPosition();*reinterpret_cast<CMatrixLink**>(ped.b.data()+0x14)=pedMatrix.get();}
                fakePlayer=i%7?ped.get():nullptr;
                auto& m=TheCamera.m_mCameraMatrix;m.GetPosition()=CVector{100.f,200.f,300.f};
                const auto a=float(i%61)*.1f,c=std::cos(a),s=std::sin(a);
                m.GetRight()=CVector{c,s,0.f};m.GetForward()=CVector{-s*.8f,c*.8f,.6f};m.GetUp()=CVector{s*.6f,-c*.6f,.8f};
                CVector input{float(i%137)*.0625f,float(i%83)*-.375f,float(i%41)*.125f},output{31.f,32.f,33.f};
                if(fn){
                    place.get()->SetPosn(input);
                    if(i%2)output=CAEAudioEnvironment::GetPositionRelativeToCamera(place.get());
                    else reinterpret_cast<void(__cdecl*)(CVector*,CPlaceable*)>(0x4D8340)(&output,place.get());
                }else if(i%31==0){reinterpret_cast<void(__cdecl*)(CVector*,const CVector*)>(0x4D80B0)(&output,nullptr);
                }else if(i%17==0){output=input;reinterpret_cast<void(__cdecl*)(CVector*,const CVector*)>(0x4D80B0)(&output,&output);
                }else if(i%2){output=CAEAudioEnvironment::GetPositionRelativeToCamera(input);
                }else reinterpret_cast<void(__cdecl*)(CVector*,const CVector*)>(0x4D80B0)(&output,&input);
                return json{{"result",ReversingProbe::Bytes(&output,sizeof(output))},{"input",ReversingProbe::Bytes(&input,sizeof(input))}};
            };
            hooks[fn]->State(false);if(fn)hooks[0]->State(false);const auto a=UAProbe::calls[fn];const auto original=run();const auto b=UAProbe::calls[fn];
            hooks[fn]->State(true);if(fn)hooks[0]->State(true);const auto candidate=run();const auto c=UAProbe::calls[fn];
            passed=original==candidate&&a==b&&c==b+1;
            Write({{"event","comparison"},{"function",fn},{"case",i},{"pair",pairs++},{"original",original},{"candidate",candidate},{"equal",passed},{"original_candidate_calls",b-a},{"candidate_calls",c-b}});
        }
        log.flush();
    }
    }
    std::memcpy(&TheCamera,cameraBefore.data(),sizeof(TheCamera));for(int n=0;n<2;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);}
    const bool restored=playerBefore==ReversingProbe::Bytes(player,sizeof(CPlayerPed))&&cameraBefore==ReversingProbe::Bytes(&TheCamera,sizeof(TheCamera));
    Write({{"event",passed&&restored?"complete":"error"},{"pairs",pairs},{"state_restored",restored}});log.flush();
}
}
