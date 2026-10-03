#pragma once
// Banco dentro del motor: clases pequenas del lote 4 (74 funciones): CMaths, CMatrixLink, CMatrixLinkList,
// CLodTimeModelInfo, CMissionTextOffsets, CAEAudioEntity, CAEAudioUtility, CAEAudioEnvironment,
// CAEExplosionAudioEntity, CAEDoorAudioEntity y CModelInfo. Original (hooks del lote desactivados) frente a
// candidato con fixtures deterministas. Se simulan y trazan: rand, QueryPerformanceCounter/Frequency,
// operator new/delete (solo durante la llamada y sin reservar memoria), CFileMgr::Read, FindPlayerPed/Vehicle,
// el banco de sonidos y el gestor de sonidos (peticiones, cancelaciones y consultas), las primitivas RW de
// liberacion, CMemoryMgr::Free, Init2dEffects, SetColModel y SetTexDictionary. El estado global tocado
// (tablas, stores de model infos, zonas de audio, camara, reloj) se guarda y se restaura en cada ejecucion.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Maths.h"
#include "MatrixLink.h"
#include "MatrixLinkList.h"
#include "ModelInfo.h"
#include "FileMgr.h"
#include "MemoryMgr.h"
#include "AESound.h"
#include "AEAudioHardware.h"
#include "AESoundManager.h"
#include "AudioZones.h"
#include "../ProbeRuntime.hpp"

namespace Small2Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- registro sin reservas (para stubs activos mientras operator new esta sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- reloj de alto rendimiento (sin traza: puede llamarlo otro hilo) ---
inline volatile int64 qpcValue{}, qpfValue{};
inline BOOL WINAPI FakeQPC(LARGE_INTEGER* p){ p->QuadPart=qpcValue; return TRUE; }
inline BOOL WINAPI FakeQPF(LARGE_INTEGER* p){ p->QuadPart=qpfValue; return TRUE; }

// --- operator new/delete: arena fija ---
alignas(16) inline std::array<uint8,4+0x54*16> arena{};
inline void* __cdecl FakeNew(size_t n){ Push(1,uint32(n)); return n<=arena.size() ? arena.data() : nullptr; }
inline void __cdecl FakeDelete(void* p){ Push(2,uint32(intptr_t(p)-intptr_t(arena.data()))); }

// --- CFileMgr::Read ---
inline uint32 readState{}, readCalls{}; inline uint32 readFailMask{};
inline size_t __cdecl FakeRead(FILESTREAM,void* buf,size_t size){ auto* b=static_cast<uint8*>(buf); for(size_t i=0;i<size;++i){readState=readState*1103515245u+12345u;b[i]=uint8(readState>>16);} const bool fail=(readFailMask>>(readCalls++%32))&1; trace.push_back({"read",size}); return fail ? 0 : size; }

// --- jugador ---
inline void* fakePedResult{}; inline void* fakeVehResult{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id){ trace.push_back({"find_player_ped",id}); return static_cast<CPlayerPed*>(fakePedResult); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool remote){ trace.push_back({"find_player_vehicle",id,remote}); return static_cast<CVehicle*>(fakeVehResult); }

// --- audio ---
inline uint32 bankLoadedMask{}, bankCalls{}, playingMask{}, playingCalls{};
inline void* entityBase{};
inline bool __fastcall FakeIsBankLoaded(void*,void*,eSoundBank b,eSoundBankSlot s){ trace.push_back({"is_bank_loaded",int(b),int(s)}); return (bankLoadedMask>>(bankCalls++%32))&1; }
inline void __fastcall FakeLoadBank(void*,void*,eSoundBank b,eSoundBankSlot s){ trace.push_back({"load_bank",int(b),int(s)}); }
inline bool includeEvent{};
inline CAESound* __fastcall FakeRequestNewSound(void*,void*,CAESound* s){
    json r={"request",int(s->m_BankSlot),int(s->m_SoundID),intptr_t(s->m_AudioEntity)-intptr_t(entityBase),F(s->m_CurrPos.x),F(s->m_CurrPos.y),F(s->m_CurrPos.z),F(s->m_Volume),F(s->m_RollOffFactor),
            F(s->m_Speed),F(s->m_Doppler),s->m_FrameDelay,s->m_Flags,F(s->m_SpeedVariance),s->m_PlayTime};
    if(includeEvent)r.push_back(s->m_Event);
    trace.push_back(r); return nullptr; }
inline int16 __fastcall FakeArePlaying(void*,void*,int16 ev,CAEAudioEntity* e){ trace.push_back({"are_playing",ev,intptr_t(e)-intptr_t(entityBase)}); return int16((playingMask>>(playingCalls++%32))&1); }
inline void __fastcall FakeCancelOwned(void*,void*,CAEAudioEntity* e,bool full){ trace.push_back({"cancel_owned",intptr_t(e)-intptr_t(entityBase),full}); }
inline void __fastcall FakeStopAndForget(CAESound* s,void*){ trace.push_back({"stop_and_forget"}); }

// --- CModelInfo ---
inline uint64 init2dHash{}; inline uint32 init2dCount{};
inline void __fastcall FakeInit2dEffects(void* self,void*){ ++init2dCount; init2dHash=Fnv(&self,4,init2dHash); }
inline void __fastcall FakeShutdown(void* self,void*){ Push(10,uint32(uintptr_t(self))); }
inline void __fastcall FakeTrap(void* self,void*){ Push(99,uint32(uintptr_t(self))); }
inline RwBool __cdecl FakeTextureDestroy(RwTexture* t){ Push(11,uint32(uintptr_t(t))); return TRUE; }
inline void __cdecl FakeFree(void* p){ Push(12,uint32(uintptr_t(p))); }
inline RpAtomic* __cdecl FakeAtomicSetFrame(RpAtomic* a,RwFrame* f){ Push(13,uint32(uintptr_t(a)),uint32(uintptr_t(f))); return a; }
inline RwBool __cdecl FakeFrameDestroy(RwFrame* f){ Push(14,uint32(uintptr_t(f))); return TRUE; }
inline RwBool __cdecl FakeAtomicDestroy(RpAtomic* a){ Push(15,uint32(uintptr_t(a))); return TRUE; }
inline void __fastcall FakeSetColModel(void* self,void*,CColModel* c,bool lod){ Push(16,uint32(uintptr_t(self)),uint32(uintptr_t(c))|(lod?1u<<31:0)); }
inline void __fastcall FakeSetTexDictionary(void* self,void*,const char* n){ Push(17,uint32(uintptr_t(self)),uint32(Fnv(n,strlen(n)))); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,74> fns{{
    {"CMaths","InitMathsTables",0x59AC90,0,-1,16},{"CMaths","GetSinFast",0x4A1340,0,-1,1024},{"CMaths","GetCosFast",0x4A1360,0,-1,1024},
    {"CMatrixLink","Insert",0x54E8F0,0,-1,256},{"CMatrixLink","Remove",0x54E910,0,-1,256},
    {"CMatrixLinkList","Init",0x54F0D0,0,-1,128},{"CMatrixLinkList","Shutdown",0x54E990,0,-1,128},{"CMatrixLinkList","AddToList1",0x54E9D0,0,-1,256},
    {"CMatrixLinkList","AddToList2",0x54EA10,0,-1,256},{"CMatrixLinkList","MoveToList1",0x54EA60,0,-1,256},{"CMatrixLinkList","MoveToList2",0x54EA90,0,-1,256},
    {"CMatrixLinkList","MoveToFreeList",0x54EAD0,0,-1,256},{"CMatrixLinkList","GetNumFree",0x54EB10,0,-1,128},{"CMatrixLinkList","GetNumUsed1",0x54EB30,0,-1,128},
    {"CMatrixLinkList","GetNumUsed2",0x54EB50,0,-1,128},
    {"CLodTimeModelInfo","GetModelType",0,0x85BCF0,4,16},{"CLodTimeModelInfo","GetTimeInfo",0,0x85BCF0,5,16},
    {"CMissionTextOffsets","Load",0x69F670,0,-1,256},
    {"CAEAudioEntity","Destructor",0x5DE9E0,0,-1,64},{"CAEAudioEntity","UpdateParameters",0,0x859988,0,16},
    {"CAEAudioUtility","GetRandomNumberInRange<int32>-int",0x4D9C10,0,-1,1024},{"CAEAudioUtility","GetRandomNumberInRange<float>-float",0x4D9C50,0,-1,1024},
    {"CAEAudioUtility","ResolveProbability",0x4D9C80,0,-1,1024},{"CAEAudioUtility","GetPiecewiseLinear",0x4D9D90,0,-1,1024},{"CAEAudioUtility","AudioLog10",0x4D9E50,0,-1,1024},
    {"CAEAudioUtility","ConvertFromBytesToMS",0x4D9EF0,0,-1,1024},{"CAEAudioUtility","ConvertFromMSToBytes",0x4D9F40,0,-1,1024},
    {"CAEAudioUtility","GetBankAndSoundFromScriptSlotAudioEvent",0x4D9CC0,0,-1,1024},{"CAEAudioUtility","FindVehicleOfPlayer",0x4D9E10,0,-1,256},
    {"CAEAudioUtility","GetCurrentTimeInMS",0x4D9E80,0,-1,512},{"CAEAudioUtility","StaticInitialise",0x5B97F0,0,-1,32},
    {"CAEAudioEnvironment","GetDopplerRelativeFrequency",0x4D7E40,0,-1,2048},{"CAEAudioEnvironment","GetDistanceAttenuation",0x4D7F20,0,-1,2048},
    {"CAEAudioEnvironment","GetDirectionalMikeAttenuation",0x4D7F60,0,-1,2048},{"CAEAudioEnvironment","GetReverbEnvironmentAndDepth",0x4D8010,0,-1,1024},
    {"CAEAudioEnvironment","GetPositionRelativeToCamera-vec",0x4D80B0,0,-1,1024},{"CAEAudioEnvironment","GetPositionRelativeToCamera-placeable",0x4D8340,0,-1,512},
    {"CAEExplosionAudioEntity","StaticInitialise",0x5B9A60,0,-1,16},{"CAEExplosionAudioEntity","AddAudioEvent",0x4DCBE0,0,-1,512},{"CAEExplosionAudioEntity","UpdateParameters",0,0x862E60,0,512},
    {"CAEDoorAudioEntity","StaticInitialise",0x5B9A80,0,-1,16},{"CAEDoorAudioEntity","Reset",0x4DC6B0,0,-1,64},{"CAEDoorAudioEntity","AddAudioEvent-1",0x4DC9F0,0,-1,512},
    {"CAEDoorAudioEntity","AddAudioEvent-2",0x4DC860,0,-1,1024},{"CAEDoorAudioEntity","PlayDoorSound",0x4DC6D0,0,-1,512},{"CAEDoorAudioEntity","UpdateParameters",0,0x85998C,0,1024},
    {"CModelInfo","Initialise",0x4C6810,0,-1,8},{"CModelInfo","ShutDown",0x4C63E0,0,-1,64},{"CModelInfo","ReInit2dEffects",0x4C63B0,0,-1,8},
    {"CModelInfo","GetModelInfoUInt16",0x4C59F0,0,-1,256},{"CModelInfo","GetModelInfoFromHashKey",0x4C59B0,0,-1,256},{"CModelInfo","GetModelInfo-full",0x4C5940,0,-1,512},
    {"CModelInfo","GetModelInfo-minmax",0x4C5A20,0,-1,512},
    {"CModelInfo","AddAtomicModel",0x4C6620,0,-1,64},{"CModelInfo","AddDamageAtomicModel",0x4C6650,0,-1,64},{"CModelInfo","AddLodAtomicModel",0x4C6680,0,-1,64},
    {"CModelInfo","AddTimeModel",0x4C66B0,0,-1,64},{"CModelInfo","AddLodTimeModel",0x4C66E0,0,-1,64},{"CModelInfo","AddWeaponModel",0x4C6710,0,-1,64},
    {"CModelInfo","AddClumpModel",0x4C6740,0,-1,64},{"CModelInfo","AddVehicleModel",0x4C6770,0,-1,64},{"CModelInfo","AddPedModel",0x4C67A0,0,-1,64},
    {"CModelInfo","IsBoatModel",0x4C5A70,0,-1,256},{"CModelInfo","IsCarModel",0x4C5AA0,0,-1,256},{"CModelInfo","IsTrainModel",0x4C5AD0,0,-1,256},
    {"CModelInfo","IsHeliModel",0x4C5B00,0,-1,256},{"CModelInfo","IsPlaneModel",0x4C5B30,0,-1,256},{"CModelInfo","IsBikeModel",0x4C5B60,0,-1,256},
    {"CModelInfo","IsFakePlaneModel",0x4C5B90,0,-1,256},{"CModelInfo","IsMonsterTruckModel",0x4C5BC0,0,-1,256},{"CModelInfo","IsQuadBikeModel",0x4C5BF0,0,-1,256},
    {"CModelInfo","IsBmxModel",0x4C5C20,0,-1,256},{"CModelInfo","IsTrailerModel",0x4C5C50,0,-1,256},{"CModelInfo","IsVehicleModelType",0x4C5C80,0,-1,256},
}};

// stores de CModelInfo: contador, primer elemento, tamano de elemento, capacidad
struct StoreDesc { uintptr_t count, items; size_t stride; int32 cap; };
inline const std::array<StoreDesc,10> stores{{
    {0xAAE950,0xAAE954,0x20,14000},{0xB1BF58,0xB1BF5C,0x24,70},{0xB1C934,0xB1C938,0x24,1},{0xB1C960,0xB1C964,0x24,169},{0xB1E128,0xB1E12C,0x28,1},
    {0xB1E158,0xB1E15C,0x28,51},{0xB1E958,0xB1E95C,0x24,92},{0xB1F650,0xB1F654,0x308,212},{0xB478F8,0xB478FC,0x44,278},{0xB4C2D8,0xB4C2DC,0x40,100},
}};
constexpr uintptr_t MI_BEGIN=0xA9B0C8, MI_END=0xB4C2DC+0x40*100;

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"small2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"small2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("small2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"small2-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,74> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,74> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto* lut=reinterpret_cast<uint8*>(0xBB3E00); const std::vector<uint8> savedLut(lut,lut+1024);
    auto* audioTime=reinterpret_cast<uint8*>(0xB610F0); constexpr size_t NAT=0x10+400; const std::vector<uint8> savedAudioTime(audioTime,audioTime+NAT);
    auto* zones=reinterpret_cast<uint8*>(0xB6DC6C); const size_t NZ=0xB6EBA8+3*0x1C-0xB6DC6C; const std::vector<uint8> savedZones(zones,zones+NZ);
    auto& underWater=*reinterpret_cast<float*>(0xC8132C); const float savedUnderWater=underWater;
    auto& area=*reinterpret_cast<int32*>(0xB72914); const int32 savedArea=area;
    auto& focus=*reinterpret_cast<uint8*>(0xB7CD74); const uint8 savedFocus=focus;
    auto& camMode=*reinterpret_cast<int16*>(0xB6F1A8+*reinterpret_cast<uint8*>(0xB6F081)*0x238); const int16 savedMode=camMode;
    auto* mi=reinterpret_cast<uint8*>(MI_BEGIN); const size_t NMI=MI_END-MI_BEGIN; const std::vector<uint8> savedMi(mi,mi+NMI);

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), p2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch fr(bit_cast<uintptr_t>(&CFileMgr::Read),FakeRead), fre(0x538950,FakeRead);
        EntryPatch a1(bit_cast<uintptr_t>(&CAEAudioHardware::IsSoundBankLoaded),FakeIsBankLoaded), a1e(0x4D88C0,FakeIsBankLoaded);
        EntryPatch a2(bit_cast<uintptr_t>(&CAEAudioHardware::LoadSoundBank),FakeLoadBank), a2e(0x4D88A0,FakeLoadBank);
        EntryPatch a3(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),FakeRequestNewSound), a3e(0x4EFB10,FakeRequestNewSound);
        EntryPatch a4(bit_cast<uintptr_t>(&CAESoundManager::AreSoundsOfThisEventPlayingForThisEntity),FakeArePlaying), a4e(0x4EF570,FakeArePlaying);
        EntryPatch a5(bit_cast<uintptr_t>(&CAESoundManager::CancelSoundsOwnedByAudioEntity),FakeCancelOwned), a5e(0x4EFCD0,FakeCancelOwned);
        EntryPatch a6(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),FakeStopAndForget), a6e(0x4EF850,FakeStopAndForget);
        EntryPatch m1(bit_cast<uintptr_t>(&CBaseModelInfo::Init2dEffects),FakeInit2dEffects), m1e(0x4C4C20,FakeInit2dEffects);
        EntryPatch m2(bit_cast<uintptr_t>(&CBaseModelInfo::SetColModel),FakeSetColModel), m2e(0x4C4BC0,FakeSetColModel);
        EntryPatch m3(bit_cast<uintptr_t>(&CBaseModelInfo::SetTexDictionary),FakeSetTexDictionary), m3e(0x4C4B40,FakeSetTexDictionary);
        EntryPatch m4(0x7F3820,FakeTextureDestroy), m5(0x74BF20,FakeAtomicSetFrame), m6(0x7F05A0,FakeFrameDestroy), m7(0x749DC0,FakeAtomicDestroy);
        EntryPatch m8(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), m8e(0x72F430,FakeFree);
        static std::array<void*,64> fakeVt{}; for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap); fakeVt[7]=reinterpret_cast<void*>(&FakeShutdown);
        void* const realPlayer=player;

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A11u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; nrec=0;
                    bankCalls=playingCalls=0; bankLoadedMask=next(); playingMask=next(); includeEvent=false; fakePedResult=realPlayer; fakeVehResult=nullptr;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CMaths") {
                        if(nm=="InitMathsTables"){ for(int k=0;k<1024;++k)lut[k]=uint8(next()); reinterpret_cast<void(__cdecl*)()>(a)(); out["lut"]=ReversingProbe::Bytes(lut,1024); std::memcpy(lut,savedLut.data(),1024); }
                        else { float x; switch(next()%5){case 0:x=uni(-7.f,7.f);break;case 1:x=uni(-1000.f,1000.f);break;case 2:x=float(int(next()%512)-256)*0.024543693f;break;case 3:x=uni(-1e6f,1e6f);break;default:x=uni(-0.05f,0.05f);}
                            ret=F(reinterpret_cast<float(__cdecl*)(float)>(a)(x)); }
                    }
                    else if(cls=="CMatrixLink"||(cls=="CMatrixLinkList"&&nm!="Init"&&nm!="Shutdown")) {
                        alignas(16) static std::array<uint8,0x1FC> list{}; alignas(16) static std::array<std::array<uint8,0x54>,16> links{};
                        for(auto& b:list)b=uint8(next()); for(auto& l:links)for(auto& b:l)b=uint8(next());
                        auto P=[&](void* base,size_t off)->uint8*&{ return *reinterpret_cast<uint8**>(static_cast<uint8*>(base)+off); };
                        uint8* L=list.data(); constexpr size_t heads[3]{0,0xA8,0x150}, tails[3]{0x54,0xFC,0x1A4};
                        std::array<std::vector<uint8*>,3> lists; for(auto& l:links)lists[next()%3].push_back(l.data());
                        for(int k=0;k<3;++k){ uint8* prev=L+heads[k]; for(auto* n:lists[k]){P(prev,0x50)=n;P(n,0x4C)=prev;prev=n;} P(prev,0x50)=L+tails[k]; P(L+tails[k],0x4C)=prev; }
                        uint8* pick=links[next()%16].data();
                        if(nm=="Insert"){ uint8* other=next()%4 ? L+heads[next()%3] : links[next()%16].data(); reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(other,pick); }
                        else if(nm=="Remove")reinterpret_cast<void(__thiscall*)(void*)>(a)(pick);
                        else if(nm=="AddToList1"||nm=="AddToList2"){ auto* r=reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(L); ret=r ? json(intptr_t(r-links[0].data())) : json(nullptr); }
                        else if(nm.rfind("MoveTo",0)==0)reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(L,pick);
                        else ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(L);
                        out["list"]=ReversingProbe::Bytes(list.data(),list.size()); out["links"]=Fnv(links.data(),sizeof(links));
                    }
                    else if(cls=="CMatrixLinkList") {
                        alignas(16) static std::array<uint8,0x1FC> list{}; for(auto& b:list)b=uint8(next()); for(auto& b:arena)b=uint8(next());
                        auto masked=[&]{ auto c=arena; const uint32 cnt=*reinterpret_cast<uint32*>(c.data()); for(uint32 e=0;e<std::min<uint32>(cnt,16);++e)for(int k=0x45;k<0x48;++k)c[4+e*0x54+k]=0; return Fnv(c.data(),c.size()); };
                        if(nm=="Init"){ const int32 count=int32(next()%17);
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(list.data(),count); }
                            out["arena"]=masked(); out["cookie"]=*reinterpret_cast<uint32*>(arena.data()); }
                        else { auto& objs=*reinterpret_cast<uint8**>(list.data()+0x1F8); const uint32 cnt=next()%17; *reinterpret_cast<uint32*>(arena.data())=cnt;
                            for(uint32 e=0;e<cnt;++e){ *reinterpret_cast<uint32*>(&arena[4+e*0x54+0x40])=0; arena[4+e*0x54+0x44]=0; }
                            objs=next()%4 ? arena.data()+4 : nullptr;
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(a)(list.data()); }
                            out["arena"]=masked(); }
                        out["list"]=ReversingProbe::Bytes(list.data(),list.size()); out["recs"]=Recs();
                    }
                    else if(cls=="CLodTimeModelInfo") {
                        alignas(16) std::array<uint8,0x30> obj{}; for(auto& b:obj)b=uint8(next()); *reinterpret_cast<uintptr_t*>(obj.data())=0x85BCF0;
                        if(nm=="GetModelType")ret=int(reinterpret_cast<uint8(__thiscall*)(void*)>(a)(obj.data()));
                        else ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(obj.data()))-intptr_t(obj.data());
                    }
                    else if(cls=="CMissionTextOffsets") {
                        static std::vector<uint8> obj(sizeof(CMissionTextOffsets)); for(auto& b:obj)b=uint8(next());
                        readState=next(); readCalls=0; readFailMask=next()%3==0 ? next() : 0;
                        const uint32 maxLen=uint32(obj.size()-4); uint32 len=next()%4==0 ? (next()%20)*12 : next()%(maxLen+1); if(next()%8==0)len=0;
                        uint32 offset=next();
                        ret=reinterpret_cast<bool(__thiscall*)(void*,uint32,FILESTREAM,uint32*,int32)>(a)(obj.data(),len,reinterpret_cast<FILESTREAM>(0x1234),&offset,int32(next()));
                        out["obj"]=Fnv(obj.data(),obj.size()); out["size"]=*reinterpret_cast<uint32*>(obj.data()+obj.size()-4); out["offset"]=offset; out["reads"]=readCalls;
                    }
                    else if(cls=="CAEAudioEntity") {
                        alignas(16) std::array<uint8,0x7C> obj{}; static const uintptr_t vts[]{0x859988,0x862E60,0x85998C};
                        *reinterpret_cast<uintptr_t*>(obj.data())=vts[next()%3]; *reinterpret_cast<uint32*>(obj.data()+4)=next(); for(int k=0x14;k<0x30;++k)obj[k]=uint8(next()); // m_tempSound.m_PhysicalEntity (+0x10) queda nulo
                        entityBase=obj.data();
                        if(nm=="Destructor")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else { CAESound s{}; s.m_Volume=uni(-10.f,10.f); reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(a)(obj.data(),&s,int16(next())); out["sound"]=F(s.m_Volume); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if(cls=="CAEAudioUtility") {
                        if(nm=="GetRandomNumberInRange<int32>-int"){ int32 lo,hi; if(next()%4){lo=int32(next()%2001)-1000;hi=lo+int32(next()%2001);}else{lo=int32(next()%200001)-100000;hi=int32(next()%200001)-100000;}
                            ret=reinterpret_cast<int32(__cdecl*)(int32,int32)>(a)(lo,hi); }
                        else if(nm=="GetRandomNumberInRange<float>-float"){ const float lo=uni(-100.f,100.f), hi=next()%4 ? lo+uni(0.f,200.f) : uni(-1e5f,1e5f); ret=F(reinterpret_cast<float(__cdecl*)(float,float)>(a)(lo,hi)); }
                        else if(nm=="ResolveProbability"){ float p=uni(-0.5f,1.5f); if(next()%8==0)p=float(next()%2); ret=reinterpret_cast<bool(__cdecl*)(float)>(a)(p); }
                        else if(nm=="GetPiecewiseLinear"){ const int16 n=int16(1+next()%8); float data[8][2]; float x0=uni(-50.f,50.f);
                            for(int k=0;k<n;++k){ data[k][0]=x0; x0+=next()%6==0 ? 0.f : uni(0.01f,30.f); data[k][1]=uni(-100.f,100.f); }
                            float x=uni(data[0][0]-20.f,data[n-1][0]+20.f); if(next()%5==0)x=data[next()%n][0];
                            ret=F(reinterpret_cast<float(__cdecl*)(float,int16,const float(*)[2])>(a)(x,n,data)); }
                        else if(nm=="AudioLog10"){ float p; switch(next()%5){case 0:p=uni(0.f,0.0001f);break;case 1:p=uni(-1.f,1.f);break;case 2:p=0.00001f;break;default:p=uni(0.f,1000.f);} ret=F(reinterpret_cast<float(__cdecl*)(float)>(a)(p)); }
                        else if(nm=="ConvertFromBytesToMS"){ static const uint32 rates[]{8000,11025,22050,24000,32000,44100,48000}; const uint32 bytes=next()%3 ? next()%10000000 : next();
                            const uint32 rate=next()%4 ? rates[next()%7] : 1+next()%0x7FFFFFFF; const uint16 ch=uint16(next()%4 ? 1+next()%2 : 1+next()%8);
                            ret=reinterpret_cast<uint32(__cdecl*)(uint32,uint32,uint16)>(a)(bytes,rate,ch); }
                        else if(nm=="ConvertFromMSToBytes"){ static const uint32 rates[]{8000,11025,22050,24000,32000,44100,48000}; const uint32 ms=next()%3 ? next()%600000 : next();
                            const uint32 rate=next()%4 ? rates[next()%7] : 1+next()%0x7FFFFF; const uint16 mult=uint16(next()%4 ? 1+next()%2 : 1+next()%8);
                            ret=reinterpret_cast<uint32(__cdecl*)(uint32,uint32,uint16)>(a)(ms,rate,mult); }
                        else if(nm=="GetBankAndSoundFromScriptSlotAudioEvent"){ int32 id; switch(next()%6){case 0:id=int32(next()%1800);break;case 1:case 2:id=1800+int32(next()%200);break;case 3:id=0xFFFF;break;default:id=2000+int32(next()%60000);}
                            const eAudioEvents ev=eAudioEvents(id); int32 bank=0x5A5A, snd=0x7B7B; const int32 slot=int32(next()%9)-2;
                            ret=reinterpret_cast<bool(__cdecl*)(const eAudioEvents&,int32&,int32&,int32)>(a)(ev,bank,snd,slot); out["bank"]=bank; out["sound"]=snd; }
                        else if(nm=="FindVehicleOfPlayer"){ alignas(16) static std::array<uint8,0x800> ped{}, ent{}, veh{}; for(auto& b:ped)b=uint8(next()); for(auto& b:ent)b=uint8(next());
                            *reinterpret_cast<void**>(ped.data()+0xFC)=next()%3 ? ent.data() : nullptr; ent[0x36]=uint8(next());
                            fakePedResult=next()%5 ? ped.data() : nullptr; fakeVehResult=next()%3==0 ? veh.data() : nullptr; focus=uint8(next()%2);
                            auto* r=reinterpret_cast<uint8*(__cdecl*)()>(a)(); ret=r==veh.data() ? "veh" : r==ent.data() ? "ent" : r ? "other" : "null"; focus=savedFocus; }
                        else if(nm=="GetCurrentTimeInMS"){ qpcValue=int64(next())*int64(1+next()%4096)+next(); auto& freq=*reinterpret_cast<int64*>(0xB610F0); auto& start=*reinterpret_cast<uint64*>(0xB610F8);
                            freq=next()%4 ? 1000000+next()%9000000 : 3579545; start=next()%3 ? uint64(next()%100000000) : (uint64(next())<<32|next());
                            uint64 r; { EntryPatch q(bit_cast<uintptr_t>(&QueryPerformanceCounter),FakeQPC); r=reinterpret_cast<uint64(__cdecl*)()>(a)(); }
                            ret=std::to_string(r); std::memcpy(audioTime,savedAudioTime.data(),NAT); }
                        else if(nm=="StaticInitialise"){ for(size_t k=0;k<NAT;++k)audioTime[k]=uint8(next()); qpcValue=int64(next())*int64(1+next()%4096); qpfValue=next()%4 ? 1000000+next()%9000000 : 3579545;
                            { EntryPatch q(bit_cast<uintptr_t>(&QueryPerformanceCounter),FakeQPC), q2(bit_cast<uintptr_t>(&QueryPerformanceFrequency),FakeQPF); reinterpret_cast<void(__cdecl*)()>(a)(); }
                            out["state"]=ReversingProbe::Bytes(audioTime,NAT); std::memcpy(audioTime,savedAudioTime.data(),NAT); }
                    }
                    else if(cls=="CAEAudioEnvironment") {
                        if(nm=="GetDopplerRelativeFrequency"){ const float p=uni(0.f,200.f); const float c=next()%6==0 ? p : p+uni(-30.f,30.f); const uint32 t0=next(); const uint32 t1=next()%6==0 ? t0-next()%100 : t0+next()%200;
                            const float sc=next()%6==0 ? 0.f : uni(0.f,2.f); ret=F(reinterpret_cast<float(__cdecl*)(float,float,uint32,uint32,float)>(a)(p,c,t0,t1,sc)); }
                        else if(nm=="GetDistanceAttenuation"){ float d=next()%10==0 ? uni(-1.f,0.f) : uni(0.f,140.f); if(next()%8==0)d=float(next()%1300)*0.1f; ret=F(reinterpret_cast<float(__cdecl*)(float)>(a)(d)); }
                        else if(nm=="GetDirectionalMikeAttenuation"){ CVector v{uni(-50.f,50.f),uni(-50.f,50.f),uni(-50.f,50.f)}; if(next()%10==0)v=CVector{0.f,0.f,0.f}; if(next()%6==0)v.y=0.f;
                            ret=F(reinterpret_cast<float(__cdecl*)(const CVector&)>(a)(v)); }
                        else if(nm=="GetReverbEnvironmentAndDepth"){ underWater=next()%4==0 ? uni(0.5f,1.f) : uni(0.f,0.5f); if(next()%10==0)underWater=0.5f;
                            CAudioZones::m_NumActiveSpheres=next()%3; CAudioZones::m_NumActiveBoxes=next()%3; CAudioZones::m_aActiveSpheres[0]=int32(next()%3); CAudioZones::m_aActiveBoxes[0]=int32(next()%158);
                            CAudioZones::m_aSpheres[CAudioZones::m_aActiveSpheres[0]].m_nAudioZone=next()%4==0 ? -1 : int16(next()%68);
                            CAudioZones::m_aBoxes[CAudioZones::m_aActiveBoxes[0]].m_nAudioZone=next()%4==0 ? -1 : int16(next()%68);
                            area=next()%2 ? 0 : int32(next()%18);
                            int8 env=0x55; int32 depth=0x5555; reinterpret_cast<void(__cdecl*)(int8*,int32*)>(a)(&env,&depth); out["env"]=env; out["depth"]=depth;
                            std::memcpy(zones,savedZones.data(),NZ); underWater=savedUnderWater; area=savedArea; }
                        else { static const int16 modes[]{4,7,8,16,18,22,1,53}; camMode=modes[next()%8];
                            alignas(16) static std::array<uint8,0x200> ped{}; alignas(16) static CMatrix mat; const CVector cam=TheCamera.GetPosition();
                            for(auto& b:ped)b=0; const CVector pp=cam+CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-2.f,2.f)};
                            if(next()%2){ *reinterpret_cast<CMatrix**>(ped.data()+0x14)=&mat; mat.GetPosition()=pp; } else *reinterpret_cast<CVector*>(ped.data()+4)=pp;
                            switch(next()%3){case 0:fakePedResult=realPlayer;break;case 1:fakePedResult=ped.data();break;default:fakePedResult=nullptr;}
                            CVector pos=cam+CVector{uni(-80.f,80.f),uni(-80.f,80.f),uni(-30.f,30.f)}; CVector o{-1234.f,-1234.f,-1234.f};
                            if(nm=="GetPositionRelativeToCamera-vec") reinterpret_cast<void(__cdecl*)(CVector*,const CVector*)>(a)(&o,next()%10==0 ? nullptr : &pos);
                            else { alignas(16) static std::array<uint8,0x40> pl{}; alignas(16) static CMatrix pm; for(auto& b:pl)b=0;
                                if(next()%2){ *reinterpret_cast<CMatrix**>(pl.data()+0x14)=&pm; pm.GetPosition()=pos; } else *reinterpret_cast<CVector*>(pl.data()+4)=pos;
                                reinterpret_cast<void(__cdecl*)(CVector*,void*)>(a)(&o,pl.data()); }
                            ret=json::array({F(o.x),F(o.y),F(o.z)}); camMode=savedMode; }
                    }
                    else if(cls=="CAEExplosionAudioEntity"||cls=="CAEDoorAudioEntity") {
                        const bool door=cls=="CAEDoorAudioEntity"; includeEvent=door;
                        alignas(16) std::array<uint8,0x88> obj{}; *reinterpret_cast<uintptr_t*>(obj.data())=door ? 0x85998C : 0x862E60; entityBase=obj.data();
                        timeMs=next()%2 ? next()%100000 : next();
                        if(door){ *reinterpret_cast<uint32*>(obj.data()+0x7C)=next()%3 ? timeMs-next()%12000 : next(); *reinterpret_cast<uint32*>(obj.data()+0x80)=next()%3 ? timeMs-next()%6000 : next(); *reinterpret_cast<uint32*>(obj.data()+0x84)=next(); }
                        else { obj[0x7C]=uint8(next()%3); if(next()%4==0){obj[0x7D]=uint8(next());obj[0x7E]=uint8(next());obj[0x7F]=uint8(next());} }
                        auto randPos=[&]{ const CVector cam=TheCamera.GetPosition(); switch(next()%6){case 0:return CVector{-1000.f,-1000.f,-1000.f};case 1:return CVector{0.f,0.f,0.f};case 2:return CVector{-1000.f,-1000.f,0.f};default:return cam+CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(-10.f,10.f)};} };
                        if(nm=="StaticInitialise")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Reset")reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        else if(nm=="UpdateParameters"){ alignas(16) static std::array<uint8,sizeof(CAESound)> sb{}; for(auto& b:sb)b=uint8(next()); auto* s=reinterpret_cast<CAESound*>(sb.data());
                            s->m_Volume=next()%5==0 ? float(int(next()%3)-1) : uni(-3.f,5.f); static const int32 evs[]{0x9F,0xA1,0xA0,0xA2,0x81}; s->m_Event=evs[next()%5]; s->m_CurrPos=randPos();
                            const int16 pp=next()%4==0 ? int16(-int(next()%5)) : int16(next()%3000); reinterpret_cast<void(__thiscall*)(void*,CAESound*,int16)>(a)(obj.data(),s,pp);
                            out["sound"]=ReversingProbe::Bytes(sb.data(),sb.size()); }
                        else if(!door){ CVector p=randPos(); const int32 ev=next()%4 ? 0x81 : int32(next()%0x100); const float vol=uni(-20.f,10.f);
                            reinterpret_cast<void(__thiscall*)(void*,int32,CVector&,float)>(a)(obj.data(),ev,p,vol); }
                        else if(nm=="AddAudioEvent-1"){ alignas(16) static std::array<uint8,0x200> ph{}; alignas(16) static CMatrix pm; for(auto& b:ph)b=0; const CVector p=randPos();
                            if(next()%2){ *reinterpret_cast<CMatrix**>(ph.data()+0x14)=&pm; pm.GetPosition()=p; } else *reinterpret_cast<CVector*>(ph.data()+4)=p;
                            const int32 ev=next()%4 ? 0xA3 : int32(0x9F+next()%6); reinterpret_cast<void(__thiscall*)(void*,int32,void*)>(a)(obj.data(),ev,ph.data()); }
                        else if(nm=="AddAudioEvent-2"){ CVector p=randPos(); const int32 ev=next()%6 ? int32(0x9F+next()%4) : int32(0x9B+next()%10);
                            reinterpret_cast<void(__thiscall*)(void*,int32,CVector&,float,float)>(a)(obj.data(),ev,p,uni(-10.f,10.f),uni(0.5f,1.5f)); }
                        else { CVector p=randPos(); const int16 sfx=int16(next()%4); const int32 ev=int32(0x9F+next()%5);
                            reinterpret_cast<void(__thiscall*)(void*,int16,int32,CVector&,float,float)>(a)(obj.data(),sfx,ev,p,uni(-10.f,10.f),uni(0.5f,1.5f)); }
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); timeMs=savedTime;
                    }
                    else if(cls=="CModelInfo") {
                        auto& ptrs=CModelInfo::ms_modelInfoPtrs; auto& last=CModelInfo::ms_lastPositionSearched;
                        auto Name=[&](void* p)->json{ if(!p)return nullptr; const auto u=uintptr_t(p); for(auto& s:stores)if(u>=s.items&&u<s.items+s.stride*size_t(s.cap))return json::array({int(&s-stores.data()),(u-s.items)/s.stride,(u-s.items)%s.stride}); return "foreign"; };
                        auto randomIndex=[&]{ return next()%5<3 ? int32(400+next()%212) : int32(next()%20000); };
                        bool restore=false;
                        if(nm=="GetModelInfo-full"||nm=="GetModelInfoUInt16"||nm=="GetModelInfo-minmax"){
                            static const char* names[]{"infernus","cheetah","banshee","pcj600","bmx","rhino","hydra","maverick","admiral","male01","bfori","wmybar","colt45","ak47","m4","sniper",
                                                       "nonexistent","","INFERNUS","Ped","lamppost1","trafficlight1","nt_roadblockci","airtrain_vlo","dyn_roadbarrier_3","fire_hydrant","bench","cj_bag_reclaim"};
                            const char* n=names[next()%std::size(names)]; last=int32(next()%3 ? next()%20000 : next()%700);
                            if(nm=="GetModelInfo-minmax"){ int32 lo=int32(next()%20000); int32 hi=std::min<int32>(19999,lo+int32(next()%4000)-200); if(next()%3==0){lo=int32(next()%700);hi=int32(lo+next()%20000);hi=std::min(hi,19999);}
                                ret=Name(reinterpret_cast<void*(__cdecl*)(const char*,int32,int32)>(a)(n,lo,hi)); }
                            else if(nm=="GetModelInfo-full"){ int32 idx=-7; const bool useIdx=next()%4!=0; ret=Name(reinterpret_cast<void*(__cdecl*)(const char*,int32*)>(a)(n,useIdx ? &idx : nullptr)); out["index"]=idx; }
                            else { uint16 idx=0x7777; const bool useIdx=next()%4!=0; ret=Name(reinterpret_cast<void*(__cdecl*)(const char*,uint16*)>(a)(n,useIdx ? &idx : nullptr)); out["index"]=idx; }
                            out["last"]=last; last=*reinterpret_cast<const int32*>(savedMi.data()+(0xAAE948-MI_BEGIN));
                        }
                        else if(nm=="GetModelInfoFromHashKey"){ uint32 key=next(); if(next()%4){ for(int k=0;k<50;++k){ auto* m=ptrs[next()%20000]; if(m){key=m->m_nKey;break;} } }
                            int32 idx=-7; const bool useIdx=next()%4!=0; ret=Name(reinterpret_cast<void*(__cdecl*)(uint32,int32*)>(a)(key,useIdx ? &idx : nullptr)); out["index"]=idx; }
                        else if(nm.rfind("Is",0)==0){ int32 idx=randomIndex(); if(nm=="IsVehicleModelType"&&next()%6==0)idx=20000+int32(next()%10000);
                            ret=nm=="IsVehicleModelType" ? json(reinterpret_cast<int32(__cdecl*)(int32)>(a)(idx)) : json(reinterpret_cast<bool(__cdecl*)(int32)>(a)(idx)); }
                        else if(nm=="ReInit2dEffects"){ init2dCount=0; init2dHash=0; reinterpret_cast<void(__cdecl*)()>(a)(); out["count"]=init2dCount; out["hash"]=init2dHash; out["fx"]=*reinterpret_cast<int32*>(0xB4C2D8); restore=true; }
                        else if(nm.rfind("Add",0)==0){ static const char* order[]{"AddAtomicModel","AddDamageAtomicModel","AddLodAtomicModel","AddTimeModel","AddLodTimeModel","AddWeaponModel","AddClumpModel","AddVehicleModel","AddPedModel"};
                            size_t s=0; while(nm!=order[s])++s; auto& cnt=*reinterpret_cast<int32*>(stores[s].count); cnt=int32(next()%uint32(stores[s].cap));
                            const int32 idx=int32(next()%20000); ret=Name(reinterpret_cast<void*(__cdecl*)(int32)>(a)(idx)); out["count"]=cnt; out["slot"]=Name(ptrs[idx]); restore=true; }
                        else if(nm=="Initialise"){ reinterpret_cast<void(__cdecl*)()>(a)(); out["recs"]=Recs(); restore=true; }
                        else if(nm=="ShutDown"){ alignas(16) static std::array<std::array<uint8,0x40>,8> atomics{};
                            for(size_t s=0;s<stores.size();++s){ auto& cnt=*reinterpret_cast<int32*>(stores[s].count); cnt=int32(next()%(std::min(stores[s].cap,4)+1));
                                for(int32 k=0;k<cnt;++k){ auto* item=reinterpret_cast<uint8*>(stores[s].items+stores[s].stride*k);
                                    if(s<9) *reinterpret_cast<void**>(item)=fakeVt.data();
                                    else { static const uint8 types[]{0,7,3,1}; item[0x0C]=types[next()%4]; auto& at=atomics[next()%8]; *reinterpret_cast<void**>(at.data()+4)=next()%2 ? reinterpret_cast<void*>(0x7000+next()%16*4) : nullptr;
                                        *reinterpret_cast<uint32*>(item+0x28)=next()%3 ? 0x5000+next()%256 : 0; *reinterpret_cast<void**>(item+0x2C)=next()%3 ? at.data() : nullptr;
                                        *reinterpret_cast<uint32*>(item+0x30)=next()%3 ? 0x6000+next()%256 : 0; *reinterpret_cast<uint32*>(item+0x34)=next()%3 ? 0x6800+next()%256 : 0; } } }
                            reinterpret_cast<void(__cdecl*)()>(a)(); out["recs"]=Recs(); restore=true; }
                        if(restore){ out["mi"]=Fnv(mi,NMI); std::memcpy(mi,savedMi.data(),NMI); }
                    }
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    timeMs=savedTime; std::memcpy(lut,savedLut.data(),1024); std::memcpy(audioTime,savedAudioTime.data(),NAT); std::memcpy(zones,savedZones.data(),NZ);
    underWater=savedUnderWater; area=savedArea; focus=savedFocus; camMode=savedMode; std::memcpy(mi,savedMi.data(),NMI);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",std::memcmp(mi,savedMi.data(),NMI)==0&&std::memcmp(lut,savedLut.data(),1024)==0}});
}
}
