#pragma once
// Banco dentro del motor: flotabilidad cBuoyancy (lista 9b, 9 funciones). Original (hooks de cBuoyancy
// desactivados) frente a candidato con fixtures deterministas. Las funciones se llaman sobre un cBuoyancy
// falso (no sobre mod_Buoyancy) y entidades falsas (CPhysical/CVehicle/CBoat/CPed con offsets crudos y su propia
// matriz). Se simulan y trazan, en la direccion del EXE y en el simbolo de la DLL: CWaterLevel::GetWaterLevel,
// CEntity::GetColModel, CPhysical::GetSpeed/ApplyTurnForce, FxSystem_c::AddParticle, CPedIntelligence::GetTaskSwim,
// CAEPedAudioEntity::AddAudioEvent, CAudioEngine::ReportWaterSplash, CGeneral::GetAngleBetweenPoints/LimitAngle y
// rand. Se guardan y restauran calcStruct, fPointVolMultiplier, CTimer::ms_fTimeStep, los sistemas de g_fx y
// mod_Buoyancy.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Buoyancy.h"
#include "WaterLevel.h"
#include "FxSystem.h"
#include "PedIntelligence.h"
#include "AudioEngine.h"
#include "../ProbeRuntime.hpp"

namespace Buoy9Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- objetos falsos ---
alignas(16) inline std::array<uint8,0x800> ent{};      // entidad (CPhysical/CVehicle/CPed)
alignas(16) inline std::array<uint8,0x48> entMat{};    // su matriz
alignas(16) inline std::array<uint8,0x100> handling{}; // tHandlingData
alignas(16) inline std::array<uint8,0xB0> playerData{};
alignas(16) inline std::array<uint8,0x40> intel{}, swimTask{};
alignas(16) inline std::array<uint8,0x100> colModel{};
alignas(16) inline std::array<uint8,0x40> fxSplash{}, fxWake{};
inline json Off(const void* p){ const auto d=intptr_t(p)-intptr_t(ent.data()); if(d>=0&&d<intptr_t(ent.size()))return d; return p ? json("foreign") : json(nullptr); }

// --- agua: nivel = base + sx*x + sy*y; normal opcional ---
inline float waterBase{}, waterSx{}, waterSy{}; inline uint32 waterFailMask{}, waterNormalMask{}, waterCalls{};
inline bool __cdecl FakeWaterLevel(float x,float y,float z,float* out,uint32 touch,CVector* normal){
    const uint32 k=waterCalls++%32; trace.push_back({"water",F(x),F(y),F(z),touch&0xFF,normal!=nullptr});
    if((waterFailMask>>k)&1)return false;
    *out=waterBase+waterSx*x+waterSy*y;
    if(normal&&((waterNormalMask>>k)&1)){ normal->x=waterSx*3.f+x*0.001f; normal->y=waterSy*3.f-y*0.001f; normal->z=0.9f+waterSx; }
    return true; }
// --- entidad ---
inline CColModel* __fastcall FakeGetColModel(void* self,void*){ trace.push_back({"col_model",Off(self)}); return reinterpret_cast<CColModel*>(colModel.data()); }
inline CVector* __fastcall FakeGetSpeed(void* self,void*,CVector* out,float px,float py,float pz){
    trace.push_back({"get_speed",Off(self),F(px),F(py),F(pz)});
    const auto* ms=reinterpret_cast<const float*>(static_cast<uint8*>(self)+0x44);
    *out=CVector{ms[0]+0.05f*py-0.01f*pz, ms[1]-0.05f*px, ms[2]+0.02f*px-0.03f*py}; return out; }
inline void __fastcall FakeApplyTurnForce(void* self,void*,float fx,float fy,float fz,float px,float py,float pz){ trace.push_back({"turn_force",Off(self),F(fx),F(fy),F(fz),F(px),F(py),F(pz)}); }
// --- efectos y audio ---
inline void __fastcall FakeAddParticle(void* self,void*,const CVector* pos,const CVector* vel,float t,const FxPrtMult_c* m,float rz,float lm,float lml,uint32 local){
    trace.push_back({"particle",self==fxSplash.data() ? "splash" : self==fxWake.data() ? "wake" : "other",F(pos->x),F(pos->y),F(pos->z),F(vel->x),F(vel->y),F(vel->z),F(t),
                     ReversingProbe::Bytes(m,sizeof(FxPrtMult_c)),F(rz),F(lm),F(lml),local&0xFF}); }
inline void* swimResult{};
inline void* __fastcall FakeGetTaskSwim(void* self,void*){ trace.push_back({"task_swim",self==intel.data()}); return swimResult; }
inline void __fastcall FakeAddAudioEvent(void* self,void*,int32 ev,float vol,float speed,void* ped,uint32 surf,int32 a7,uint32 maxVol){
    trace.push_back({"ped_audio",Off(self),ev,F(vol),F(speed),Off(ped),surf&0xFF,a7,maxVol}); }
inline void __fastcall FakeReportWaterSplash(void* self,void*,void* e,float vol,uint32 force){ trace.push_back({"water_splash",self==&AudioEngine,Off(e),F(vol),force&0xFF}); }
inline float __cdecl FakeAngle(float x1,float y1,float x2,float y2){ trace.push_back({"angle",F(x1),F(y1),F(x2),F(y2)}); return x1*37.f+y1*11.f-x2+y2; }
inline float __cdecl FakeLimitAngle(float a){ trace.push_back({"limit_angle",F(a)}); return a*0.5f-3.f; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,9> fns{{
    {"cBuoyancy","FindWaterLevel",0x6C2810,0,-1,512},{"cBuoyancy","FindWaterLevelNorm",0x6C28C0,0,-1,512},
    {"cBuoyancy","SimpleSumBuoyancyData",0x6C2970,0,-1,1024},{"cBuoyancy","CalcBuoyancyForce",0x6C2750,0,-1,1024},
    {"cBuoyancy","PreCalcSetup",0x6C2B90,0,-1,1024},{"cBuoyancy","AddSplashParticles",0x6C34E0,0,-1,1024},
    {"cBuoyancy","SimpleCalcBuoyancy",0x6C3B00,0,-1,512},{"cBuoyancy","ProcessBuoyancyBoat",0x6C3030,0,-1,512},
    {"cBuoyancy","ProcessBuoyancy",0x6C3EF0,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"buoy9-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"buoy9-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("buoy9");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"buoy9-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,9> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,9> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto* calc=reinterpret_cast<uint8*>(0xC1C858); std::array<uint8,0x1C> savedCalc; std::memcpy(savedCalc.data(),calc,0x1C);
    auto& volMult=cBuoyancy::fPointVolMultiplier; const float savedVol=volMult;
    auto& timeStep=CTimer::ms_fTimeStep; const float savedStep=timeStep;
    auto& fxWakePtr=*reinterpret_cast<void**>(0xA9AE34); auto& fxSplashPtr=*reinterpret_cast<void**>(0xA9AE38);
    void* const savedWake=fxWakePtr; void* const savedSplash=fxSplashPtr;
    auto* mod=reinterpret_cast<uint8*>(0xC1C890); std::array<uint8,0xD0> savedMod; std::memcpy(savedMod.data(),mod,0xD0);
    auto restore=[&]{ std::memcpy(calc,savedCalc.data(),0x1C); volMult=savedVol; timeStep=savedStep; fxWakePtr=savedWake; fxSplashPtr=savedSplash; };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch w1(bit_cast<uintptr_t>(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)),FakeWaterLevel), w1e(0x6EB690,FakeWaterLevel);
        EntryPatch c1(bit_cast<uintptr_t>(&CEntity::GetColModel),FakeGetColModel), c1e(0x535300,FakeGetColModel);
        EntryPatch s1(bit_cast<uintptr_t>(&CPhysical::GetSpeed),FakeGetSpeed), s1e(0x542CE0,FakeGetSpeed);
        EntryPatch t1(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),FakeApplyTurnForce), t1e(0x542A50,FakeApplyTurnForce);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),FakeAddParticle), p1e(0x4AA440,FakeAddParticle);
        EntryPatch g1(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskSwim),FakeGetTaskSwim), g1e(0x601070,FakeGetTaskSwim);
        EntryPatch a1(bit_cast<uintptr_t>(static_cast<void(CAEPedAudioEntity::*)(eAudioEvents,float,float,CPhysical*,eSurfaceType,int32,uint32)>(&CAEPedAudioEntity::AddAudioEvent)),FakeAddAudioEvent), a1e(0x4E2BB0,FakeAddAudioEvent);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<void(CAudioEngine::*)(CPhysical*,float,bool)>(&CAudioEngine::ReportWaterSplash)),FakeReportWaterSplash), a2e(0x506F00,FakeReportWaterSplash);
        EntryPatch m1(bit_cast<uintptr_t>(static_cast<float(*)(float,float,float,float)>(&CGeneral::GetAngleBetweenPoints)),FakeAngle), m1e(0x53CEA0,FakeAngle);
        EntryPatch m2(bit_cast<uintptr_t>(static_cast<float(*)(float)>(&CGeneral::LimitAngle)),FakeLimitAngle), m2e(0x53CB00,FakeLimitAngle);
        alignas(16) static std::array<uint8,0xD0> buoy{};
        static const int16 boatModels[]{446,452,453,454,472,473,484,493,595,430,539};
        static const int16 otherModels[]{417,460,487,497,400,500,425,520};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto B=[&](size_t off)->float&{ return *reinterpret_cast<float*>(&buoy[off]); };
                auto V=[&](uint8* base,size_t off)->CVector&{ return *reinterpret_cast<CVector*>(base+off); };
                auto fillMat=[&](uint8* m,const CVector& pos){
                    auto* fl=reinterpret_cast<float*>(m); const float c=uni(-1.f,1.f), s=uni(-1.f,1.f);
                    fl[0]=c; fl[1]=s; fl[2]=uni(-0.2f,0.2f); fl[4]=-s; fl[5]=c; fl[6]=uni(-0.2f,0.2f);
                    fl[8]=uni(-0.2f,0.2f); fl[9]=uni(-0.2f,0.2f); fl[10]=uni(0.8f,1.f); fl[12]=pos.x; fl[13]=pos.y; fl[14]=pos.z;
                    for(int k:{3,7,11,15})reinterpret_cast<uint32*>(m)[k]=next()%2 ? 0 : next();
                    *reinterpret_cast<uint32*>(m+0x40)=0; *reinterpret_cast<uint32*>(m+0x44)=0; };
                auto fillBuoy=[&]{ for(size_t k=0;k<buoy.size();k+=4)B(k)=uni(-4.f,4.f);
                    fillMat(&buoy[0x0C],CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,5.f)});
                    buoy[0x98]=uint8(next()%2); buoy[0xB8]=uint8(next()%4==0); buoy[0xBA]=uint8(next()%2); };
                auto bounds=[&](CVector& mn,CVector& mx){ mn=CVector{-uni(0.3f,3.f),-uni(0.5f,8.f),-uni(0.2f,2.f)}; mx=CVector{uni(0.3f,3.f),uni(0.5f,8.f),uni(0.2f,2.f)};
                    switch(next()%10){case 0:mx.x=-mn.x;mx.y=-mn.y;mx.z=-mn.z;mx.y=mx.x;mn.y=mn.x;break;case 1:mx.z=mx.y;mn.z=mn.y;break;case 2:mx=CVector{1.f,1.f,1.f};mn=CVector{-1.f,-1.f,-1.f};break;default:break;} };
                auto setCol=[&]{ std::fill(colModel.begin(),colModel.end(),uint8(0)); auto* cm=reinterpret_cast<CColModel*>(colModel.data()); bounds(cm->m_boundBox.m_vecMin,cm->m_boundBox.m_vecMax); };
                auto makeEntity=[&](int type){
                    std::fill(ent.begin(),ent.end(),uint8(0)); fillMat(entMat.data(),CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,5.f)});
                    *reinterpret_cast<void**>(ent.data()+0x14)=entMat.data(); ent[0x36]=uint8(type|((next()%32)<<3));
                    int16 model; if(type==2&&next()%2) model=boatModels[next()%std::size(boatModels)]; else if(next()%2) model=otherModels[next()%std::size(otherModels)]; else model=int16(next()%20000);
                    *reinterpret_cast<int16*>(ent.data()+0x22)=model; *reinterpret_cast<uint32*>(ent.data()+0x40)=next();
                    switch(next()%4){case 0:V(ent.data(),0x44)=CVector{uni(-0.03f,0.03f),uni(-0.03f,0.03f),uni(-0.03f,0.03f)};break;
                                     case 1:V(ent.data(),0x44)=CVector{uni(-0.08f,0.08f),uni(-0.08f,0.08f),uni(-0.02f,0.02f)};break;
                                     default:V(ent.data(),0x44)=CVector{uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-0.3f,0.3f)};}
                    if(next()%12==0)V(ent.data(),0x44)=CVector{0.f,0.f,0.f};
                    V(ent.data(),0x50)=CVector{uni(-0.05f,0.05f),uni(-0.05f,0.05f),uni(-0.05f,0.05f)};
                    *reinterpret_cast<float*>(ent.data()+0x8C)=uni(10.f,5000.f);
                    static const uint32 vtypes[]{5,5,5,0,3,4,9}; *reinterpret_cast<uint32*>(ent.data()+0x590)=vtypes[next()%7];
                    static const uint32 stypes[]{0,3,3,4,5}; *reinterpret_cast<uint32*>(ent.data()+0x594)=stypes[next()%5];
                    for(auto& b:handling)b=0; *reinterpret_cast<float*>(handling.data()+0xB0)=uni(0.f,0.3f); *reinterpret_cast<void**>(ent.data()+0x384)=handling.data();
                    for(auto& b:playerData)b=0; *reinterpret_cast<float*>(playerData.data()+0x90)=uni(-5.f,5.f);
                    *reinterpret_cast<void**>(ent.data()+0x47C)=intel.data(); *reinterpret_cast<void**>(ent.data()+0x480)=next()%2 ? playerData.data() : nullptr; };
                auto entPos=[&]{ return V(entMat.data(),0x30); };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xB0A7u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; waterCalls=0;
                    timeStep=uni(0.3f,2.f); volMult=uni(0.f,1.1f);
                    for(int k=0;k<6;++k)reinterpret_cast<float*>(calc)[k]=uni(-3.f,3.f);
                    { static const uint8 flags[]{0,1,2,3,0xFE,0xFF}; calc[0x18]=flags[next()%6]; calc[0x19]=uint8(next()); calc[0x1A]=uint8(next()); calc[0x1B]=uint8(next()); }
                    fxSplashPtr=fxSplash.data(); fxWakePtr=fxWake.data(); swimResult=next()%2 ? swimTask.data() : nullptr;
                    waterFailMask=next()%8==0 ? next() : 0; waterNormalMask=next()%4 ? 0xFFFFFFFFu : next(); waterSx=uni(-0.1f,0.1f); waterSy=uni(-0.1f,0.1f);
                    fillBuoy(); setCol(); makeEntity(2);
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address; void* const self=buoy.data();
                    json out={{"switches",switches}}; json ret=nullptr;
                    // nivel del agua alrededor de una z de referencia (relativo para cubrir los tres estados)
                    auto waterAround=[&](float z,float lo,float hi){ const CVector p=V(&buoy[0],0); waterBase=z+uni(lo,hi)-waterSx*p.x-waterSy*p.y; };

                    if(nm=="FindWaterLevel"||nm=="FindWaterLevelNorm") {
                        V(&buoy[0],0)=CVector{uni(-50.f,50.f),uni(-50.f,50.f),uni(-5.f,5.f)};
                        B(0x84)=-uni(0.f,2.f); B(0x78)=uni(0.f,2.f); if(next()%10==0)B(0x78)=B(0x84);
                        V(&buoy[0],0x58)=CVector{0.f,0.f,B(8)}; CVector other{uni(-2.f,2.f),uni(-2.f,2.f),uni(-5.f,5.f)};
                        const CVector* initial=next()%3 ? reinterpret_cast<CVector*>(&buoy[0x58]) : &other;
                        waterAround(B(8),-4.f,4.f);
                        CVector off{uni(-5.f,5.f),uni(-5.f,5.f),uni(-3.f,3.f)}; int32 state=0x77777777; CVector normal{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        if(nm=="FindWaterLevel") reinterpret_cast<void(__thiscall*)(void*,const CVector*,CVector*,int32*)>(a)(self,initial,&off,&state);
                        else reinterpret_cast<void(__thiscall*)(void*,const CVector*,CVector*,int32*,CVector*)>(a)(self,initial,&off,&state,&normal);
                        out["off"]={F(off.x),F(off.y),F(off.z)}; out["state"]=state; out["normal"]={F(normal.x),F(normal.y),F(normal.z)};
                    }
                    else if(nm=="SimpleSumBuoyancyData") {
                        volMult=uni(0.2f,1.1f); B(0x84)=-uni(0.f,2.f); B(0x88)=next()%4==0 ? 1.f : uni(1.f,9.f); B(0xBC)=uni(0.f,5.f);
                        CVector p{uni(-4.f,4.f),uni(-4.f,4.f),uni(B(0x84)-1.f,B(0x84)+3.f)}; const int32 st=next()%8==0 ? 0 : int32(1+next()%2);
                        ret=F(reinterpret_cast<float(__thiscall*)(void*,CVector*,int32)>(a)(self,&p,st)); out["p"]={F(p.x),F(p.y),F(p.z)};
                    }
                    else if(nm=="CalcBuoyancyForce") {
                        const int type=int(2+next()%3); makeEntity(type);
                        B(0xBC)=uni(0.f,1.f); B(0x6C)=uni(0.f,500.f); if(next()%8==0)B(0xBC)=0.f;
                        V(ent.data(),0x44).z=uni(-0.3f,0.3f); *reinterpret_cast<float*>(ent.data()+0x8C)=uni(0.f,3000.f);
                        if(next()%10==0){ *reinterpret_cast<float*>(ent.data()+0x8C)=0.f; V(ent.data(),0x44).z=-0.f; B(0xBC)=-0.f; }
                        CVector turn{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)}, force{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)};
                        ret=reinterpret_cast<bool(__thiscall*)(void*,void*,CVector*,CVector*)>(a)(self,ent.data(),&turn,&force);
                        out["turn"]={F(turn.x),F(turn.y),F(turn.z)}; out["force"]={F(force.x),F(force.y),F(force.z)};
                    }
                    else if(nm=="PreCalcSetup") {
                        const int type=next()%10<7 ? 2 : int(3+next()%2); makeEntity(type);
                        reinterpret_cast<void(__thiscall*)(void*,void*,float)>(a)(self,ent.data(),uni(0.f,3000.f));
                    }
                    else if(nm=="AddSplashParticles") {
                        const int type=int(2+next()%3); makeEntity(type);
                        CVector from{uni(-3.f,3.f),uni(-3.f,3.f),uni(-2.f,2.f)}, to{uni(-3.f,3.f),uni(-3.f,3.f),uni(-2.f,2.f)}, dir{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        if(next()%6==0)to=from;
                        if(next()%10==0){ from=to=CVector{0.f,0.f,0.f}; }
                        const uint8 reduce=uint8(next()%2);
                        reinterpret_cast<void(__thiscall*)(void*,void*,CVector,CVector,CVector,uint8)>(a)(self,ent.data(),from,to,dir,reduce);
                    }
                    else if(nm=="SimpleCalcBuoyancy") {
                        const int type=next()%4==0 ? 3 : (next()%2 ? 2 : 4); makeEntity(type);
                        CVector mn,mx; bounds(mn,mx); V(&buoy[0],0x7C)=mn; V(&buoy[0],0x70)=mx;
                        const CVector c=(mx-mn)*0.5f; V(&buoy[0],0x9C)=c; V(&buoy[0],0xA8)=CVector{uni(0.f,1.f),uni(0.f,1.f),uni(0.f,1.f)};
                        B(0x88)=1.f; V(&buoy[0],0xC0)=CVector{0.f,0.f,0.f}; B(0xBC)=0.f; buoy[0x98]=0; buoy[0xBA]=uint8(next()%4==0);
                        V(&buoy[0],0)=entPos(); std::memcpy(&buoy[0x0C],entMat.data(),0x40); V(&buoy[0],0x58)=CVector{0.f,0.f,B(8)};
                        waterAround(B(8),mn.z-0.5f,mx.z+0.5f);
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,ent.data());
                    }
                    else if(nm=="ProcessBuoyancyBoat") {
                        makeEntity(2); if(next()%5) *reinterpret_cast<uint32*>(ent.data()+0x590)=5;
                        if(next()%4) *reinterpret_cast<int16*>(ent.data()+0x22)=boatModels[next()%std::size(boatModels)];
                        auto* cm=reinterpret_cast<CColModel*>(colModel.data());
                        waterBase=entPos().z+uni(cm->m_boundBox.m_vecMin.z-1.f,cm->m_boundBox.m_vecMax.z+1.f)-waterSx*entPos().x-waterSy*entPos().y;
                        CVector turn{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)}, force{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)};
                        ret=reinterpret_cast<bool(__thiscall*)(void*,void*,float,CVector*,CVector*,bool)>(a)(self,ent.data(),uni(0.f,3000.f),&turn,&force,next()%3==0);
                        out["turn"]={F(turn.x),F(turn.y),F(turn.z)}; out["force"]={F(force.x),F(force.y),F(force.z)};
                    }
                    else if(nm=="ProcessBuoyancy") {
                        const int type=next()%2 ? 3 : (next()%2 ? 2 : 4); makeEntity(type);
                        auto* cm=reinterpret_cast<CColModel*>(colModel.data());
                        const float rel=type==3 ? uni(-2.5f,2.5f) : uni(cm->m_boundBox.m_vecMin.z-1.f,cm->m_boundBox.m_vecMax.z+1.f);
                        waterBase=entPos().z+rel-waterSx*entPos().x-waterSy*entPos().y-B(0x68);
                        CVector turn{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)}, force{uni(-9.f,9.f),uni(-9.f,9.f),uni(-9.f,9.f)};
                        ret=reinterpret_cast<bool(__thiscall*)(void*,void*,float,CVector*,CVector*)>(a)(self,ent.data(),uni(0.f,2000.f),&turn,&force);
                        out["turn"]={F(turn.x),F(turn.y),F(turn.z)}; out["force"]={F(force.x),F(force.y),F(force.z)};
                    }
                    out["buoy"]=ReversingProbe::Bytes(buoy.data(),buoy.size()); out["calc"]=ReversingProbe::Bytes(calc,0x1C); out["vol"]=F(volMult);
                    out["ent"]=Fnv(ent.data(),ent.size()); out["mat"]=Fnv(entMat.data(),entMat.size());
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
                restore();
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restore();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",std::memcmp(calc,savedCalc.data(),0x1C)==0&&std::memcmp(mod,savedMod.data(),0xD0)==0
        &&F(volMult)==F(savedVol)&&F(timeStep)==F(savedStep)&&fxWakePtr==savedWake&&fxSplashPtr==savedSplash}});
}
}
