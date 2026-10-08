#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AEPoliceScannerAudioEntity.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AEAudioUtility.h"
#include "PedIntelligence.h"
#include "TheZones.h"
#include "../ProbeRuntime.hpp"
namespace AudioScannerPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},positive{};inline uintptr_t base{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
alignas(16) inline std::array<uint8,0x800> fakePed{},fakeVehicle{};inline CZone fakeZone{};alignas(16) inline std::array<uint8,0x7C> outputSound{};
inline bool __cdecl Busy(){trace.push_back({"busy"});return scenario%3==1;}
inline bool __fastcall Loaded(void*,void*,eSoundBank bank,eSoundID id,eSoundBankSlot slot){++positive;trace.push_back({"loaded",int(bank),int(id),int(slot)});return scenario%3!=1;}
inline void __fastcall Load(void*,void*,eSoundBank bank,eSoundID id,eSoundBankSlot slot){++positive;trace.push_back({"load",int(bank),int(id),int(slot)});}
inline void __fastcall Frontend(void*,void*,eAudioEvents event,float vol,float speed){++positive;trace.push_back({"frontend",int(event),F(vol),F(speed)});}
inline void __cdecl Finish(){++positive;trace.push_back({"finish"});CAEPoliceScannerAudioEntity::s_nScannerPlaybackState=CAEPoliceScannerAudioEntity::STATE_INITIAL;}
inline void __cdecl LoadSlots(){++positive;trace.push_back({"load_slots"});}
inline void __fastcall PlayLoaded(void* self,void*){++positive;trace.push_back({"play_loaded",uintptr_t(self)-base});}
inline int32 __cdecl Random(int32 low,int32 high){trace.push_back({"random",low,high});return low+int32(scenario%uint32(high-low+1));}
inline CZone* __cdecl Zone(const CVector& p,bool check){trace.push_back({"zone",F(p.x),F(p.y),F(p.z),check});return scenario%17==0?nullptr:&fakeZone;}
inline CWanted* __cdecl Wanted(int32 player){trace.push_back({"wanted",player});return reinterpret_cast<CWanted*>(base-0x21C+(player?0x1000:0));}
inline CPlayerPed* __cdecl Player(int32 player){trace.push_back({"player",player});return reinterpret_cast<CPlayerPed*>(fakePed.data());}
inline CTaskSimpleSwim* __fastcall Swim(void*,void*){trace.push_back({"swim"});return scenario%3==0?reinterpret_cast<CTaskSimpleSwim*>(1):nullptr;}
inline CTaskSimpleJetPack* __fastcall Jet(void*,void*){trace.push_back({"jet"});return scenario%3==1?reinterpret_cast<CTaskSimpleJetPack*>(1):nullptr;}
inline int32 __fastcall Remap(void*,void*){trace.push_back({"remap"});return scenario%3==0?1:-1;}
inline void __fastcall Dialogue(void* self,void*,tScannerSlot* first,tScannerSlot* second){++positive;json a=json::array(),b=json::array();for(int i=0;i<5;++i){a.push_back({int(first[i].Bank),int(first[i].SoundID)});b.push_back({int(second[i].Bank),int(second[i].SoundID)});}trace.push_back({"dialogue",uintptr_t(self)-base,a,b});}
inline CAESound* __fastcall Request(void*,void*,CAESound* s){++positive;trace.push_back({"request",int(s->m_BankSlot),int(s->m_SoundID),uintptr_t(s->m_AudioEntity)-base,F(s->m_CurrPos.x),F(s->m_CurrPos.y),F(s->m_CurrPos.z),F(s->m_Volume),F(s->m_RollOffFactor),F(s->m_Speed),F(s->m_Doppler),int(s->m_FrameDelay),int(s->m_Flags),F(s->m_SpeedVariance),int(s->m_PlayTime),int(s->m_Event),F(s->m_ClientVariable)});return scenario%3?reinterpret_cast<CAESound*>(outputSound.data()):nullptr;}
inline void __fastcall Position(CAESound* s,void*,CVector p){s->m_CurrPos=p;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,4> fns{{{"CAEPoliceScannerAudioEntity","AddAudioEvent",0x4E71E0,0,-1,1024},{"CAEPoliceScannerAudioEntity","LoadSlots",0x4E6CD0,0,-1,1024},{"CAEPoliceScannerAudioEntity","PlayLoadedDialogue",0x4E6F60,0,-1,1024},{"CAEPoliceScannerAudioEntity","Service",0x4E7630,0,-1,1024}}};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"audio-scanner-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"audio-scanner-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio-scanner-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"audio-scanner-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,4> hooks{};std::array<bool,4> enabled{},locked{};
    for(size_t n=0;n<4;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    auto* state=reinterpret_cast<uint8*>(0xB61CF8);std::array<uint8,0x60> saved{};memcpy(saved.data(),state,saved.size());
    auto& clock=*reinterpret_cast<uint32*>(0xB7CB84);const auto savedClock=clock;auto& wide=*reinterpret_cast<uint8*>(0xB6F065);const auto savedWide=wide;auto& radio=*reinterpret_cast<uintptr_t*>(0xB6B98C);const auto savedRadio=radio;
    auto& volumes=*reinterpret_cast<int8**>(0xBD00F8);auto* savedVolumes=volumes;std::array<int8,45401> fakeVolumes{};fakeVolumes[164]=-12;volumes=fakeVolumes.data();
    uint32 pairs=0,totalFailures=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch a(0x4076A0,Busy),ad(bit_cast<uintptr_t>(&CStreaming::IsVeryBusy),Busy);
        EntryPatch b(0x4D8EF0,Loaded),bd(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundLoaded),Loaded);
        EntryPatch c(0x4D8ED0,Load),cd(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSound),Load);
        EntryPatch d(0x506EA0,Frontend),dd(bit_cast<uintptr_t>(&CAudioEngine::ReportFrontendAudioEvent),Frontend);
        EntryPatch e(0x4E6C30,Finish),ed(bit_cast<uintptr_t>(&CAEPoliceScannerAudioEntity::FinishedPlayingScannerDialogue),Finish);
        EntryPatch f(0x4D9C10,Random),fd(bit_cast<uintptr_t>(&CAEAudioUtility::GetRandomNumberInRange<int32>),Random);
        EntryPatch g(0x572360,Zone),gd(bit_cast<uintptr_t>(&CTheZones::FindSmallestZoneForPosition),Zone);
        EntryPatch h(0x56E230,Wanted),hd(bit_cast<uintptr_t>(&FindPlayerWanted),Wanted);
        EntryPatch j(0x56E210,Player),jd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
        EntryPatch k(0x601070,Swim),kd(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),Swim);
        EntryPatch l(0x601110,Jet),ld(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskJetPack),Jet);
        EntryPatch m(0x6D0B70,Remap),md(bit_cast<uintptr_t>(&CVehicle::GetRemapIndex),Remap);
        EntryPatch o(0x4E6ED0,Dialogue),od(bit_cast<uintptr_t>(&CAEPoliceScannerAudioEntity::PlayPoliceScannerDialogue),Dialogue);
        EntryPatch p(0x4EFB10,Request),pd(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),Request);
        EntryPatch q(0x4EF880,Position),qd(bit_cast<uintptr_t>(&CAESound::SetPosition),Position);
        for(size_t n=0;n<4;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
            std::unique_ptr<EntryPatch> loadExe,loadDll,playExe,playDll;
            if(n==3){loadExe=std::make_unique<EntryPatch>(0x4E6CD0,LoadSlots);loadDll=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CAEPoliceScannerAudioEntity::LoadSlots),LoadSlots);playExe=std::make_unique<EntryPatch>(0x4E6F60,PlayLoaded);playDll=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CAEPoliceScannerAudioEntity::PlayLoadedDialogue),PlayLoaded);}
            for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,0x7C> object{};base=uintptr_t(object.data());
                auto run=[&](bool candidate){scenario=uint32(i);positive=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(fakePed.data(),0,fakePed.size());memset(fakeVehicle.data(),0,fakeVehicle.size());memset(state,0xCD,saved.size());clock=3000+uint32(i%100);wide=uint8((i/16)%2);radio=i%2?1:0;
                    using S=CAEPoliceScannerAudioEntity;S::s_fVolumeOffset=float(i%9)-8.f;S::s_bStoppingScanner=(i/32)%2;S::s_pSound=nullptr;S::s_nAbortPlaybackTime=2950u+uint32((i/8)%5)*50;S::s_nPlaybackStartTime=2950u+uint32((i/4)%5)*50;S::s_nSectionPlaying=int16(i%2);S::s_nScannerPlaybackState=static_cast<S::State>(i%8);S::s_bScannerDisabled=(i/8)%2;S::s_NextNewScannerDialogueTime=4000;S::s_pPSControlling=i%3?reinterpret_cast<S*>(object.data()):nullptr;
                    for(int s=0;s<5;++s){S::s_SlotState[s]=int16((i+s)%7-1);S::s_ScannerSlotFirst[s]={static_cast<eSoundBank>(i%7==0?-1:140+s),static_cast<eSoundID>(i%11==0?-1:s+1)};S::s_ScannerSlotSecond[s]={static_cast<eSoundBank>(i%7==0?-1:150+s),static_cast<eSoundID>(i%11==0?-1:s+4)};}
                    if(n==2&&i%4==0)for(auto& value:S::s_SlotState)value=3;
                    S::s_pCurrentSlots=i%13==0&&n!=2?nullptr:S::s_ScannerSlotFirst;*reinterpret_cast<float*>(0xB61D54)=float(i%5)*0.25f;
                    int zoneIndex=int(i%194);for(int seek=0;seek<194&&reinterpret_cast<const int16*>(0x8C87A8)[zoneIndex]<0;++seek)zoneIndex=(zoneIndex+1)%194;
                    memcpy(fakeZone.m_TextLabel,reinterpret_cast<const uint8*>(0x8C8198)+zoneIndex*8,8);fakeZone.m_fX1=-100;fakeZone.m_fX2=100;fakeZone.m_fY1=-200;fakeZone.m_fY2=200;
                    *reinterpret_cast<uint32*>(fakePed.data()+0x46C)=i%2?0x100:0;*reinterpret_cast<uintptr_t*>(fakePed.data()+0x47C)=uintptr_t(fakePed.data());*reinterpret_cast<uintptr_t*>(fakePed.data()+0x58C)=i%19?uintptr_t(fakeVehicle.data()):0;fakeVehicle[0x1D4]=uint8(i%48);fakeVehicle[0x434]=uint8(i%129);
                    hooks[n]->State(candidate);const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;if(!switches)return json{{"switch",false}};
                    if(n==0){CVector point{float(i%7-3)*75.f,float((i/7)%7-3)*150.f,1.f};reinterpret_cast<void(__thiscall*)(void*,eAudioEvents,eCrimeType,const CVector&)>(fn.address)(object.data(),static_cast<eAudioEvents>(i%23?164:163),static_cast<eCrimeType>(i%25),point);}
                    else if(n==2)reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(object.data());else reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    if(!candidate&&positive)++positiveCases;
                    return json{{"switch",true},{"state",Hash(state,saved.size())},{"object",Hash(object.data(),object.size())},{"ped",Hash(fakePed.data(),fakePed.size())},{"vehicle",Hash(fakeVehicle.data(),fakeVehicle.size())},{"trace",trace}};
                };
                const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;
                Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            }
            if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
        }
    }
    for(size_t n=0;n<4;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);}memcpy(state,saved.data(),saved.size());clock=savedClock;wide=savedWide;radio=savedRadio;volumes=savedVolumes;
    Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",memcmp(state,saved.data(),saved.size())==0&&clock==savedClock&&wide==savedWide&&radio==savedRadio&&volumes==savedVolumes}});log.close();
}
}
