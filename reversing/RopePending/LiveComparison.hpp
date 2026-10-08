#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Rope.h"
#include "Matrix.h"
#include "AudioEngine.h"
#include "../ProbeRuntime.hpp"
namespace RopePendingLive {
inline bool done{};inline std::ofstream log;inline json trace;inline uint32 scenario{},positive{},rng{};inline uintptr_t base{};
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float v){return std::bit_cast<uint32>(v);}
inline uint64 Hash(const void* p,size_t n){uint64 h=1469598103934665603ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline void __fastcall Normalise(CVector* v,void*){++positive;trace.push_back({"normalise",F(v->x),F(v->y),F(v->z)});const auto inverse=1.f/std::sqrt((v->x*v->x+v->y*v->y)+v->z*v->z);v->x*=inverse;v->y*=inverse;v->z*=inverse;}
alignas(16) inline std::array<uint8,0xA18> holder{},attached{},current{},vehicle{};alignas(16) inline std::array<uint8,0x7C4> ped{};alignas(16) inline std::array<uint8,0x19C> pickup{};
alignas(16) inline std::array<std::array<uint8,0x80>,6> matrices{};inline std::array<uintptr_t,64> vt{};inline std::array<std::array<uintptr_t,4>,3> pools{};inline std::array<int8,3> map{};
alignas(16) inline std::array<std::array<uint8,0x40>,20> infos{},cols{};
inline int32 Id(const void* p){if(!p)return 0;const std::array<const void*,6> entities{holder.data(),attached.data(),current.data(),ped.data(),vehicle.data(),pickup.data()};for(int i=0;i<6;++i)if(p==entities[i])return i+1;for(int i=0;i<6;++i)if(p==matrices[i].data())return 20+i;return 99;}
inline CVector& Pos(void* p){auto* matrix=*reinterpret_cast<CMatrix**>(static_cast<uint8*>(p)+0x14);return matrix?matrix->GetPosition():*reinterpret_cast<CVector*>(static_cast<uint8*>(p)+4);}
inline int32 __cdecl Random(){const auto value=int32((scenario*719+ ++rng*1381)%32768);trace.push_back({"rand",value});return value;}
inline float __cdecl Ground(CVector v,bool* result,CEntity** entity){++positive;trace.push_back({"ground",F(v.x),F(v.y),F(v.z),result==nullptr,entity==nullptr});return -.5f;}
inline int16 __fastcall Axis(void*,void*){trace.push_back({"axis"});return int16(scenario%3==0?-128:scenario%3==1?128:0);}
inline bool __fastcall Button(void*,void*){trace.push_back({"button"});return scenario%7==0;}
inline void __fastcall Mission(void*,void*,uint16 event,CPhysical* physical,float a,float b){++positive;trace.push_back({"mission",event,Id(physical),F(a),F(b)});}
inline int8 __fastcall Weight(CRope* self,void*,float x,float y,float z,int32 flags,float* output){++positive;trace.push_back({"weight",F(x),F(y),F(z),flags});output[0]=x+.125f;output[1]=y-.25f;output[2]=z+.5f;self->m_aSegments[31]=CVector{x,y,z};return scenario%3!=0;}
inline CVector* __fastcall Speed(void* self,void*,CVector* output,CVector point){trace.push_back({"speed",Id(self),F(point.x),F(point.y),F(point.z)});*output=CVector{point.x*.125f+.25f,point.y*.25f-.125f,point.z*.125f};return output;}
inline void __fastcall Force(void* self,void*,CVector up){++positive;trace.push_back({"force_up",Id(self),F(up.x),F(up.y),F(up.z)});auto* matrix=static_cast<CMatrix*>(self);matrix->GetUp()=up;matrix->GetRight()=CVector{1.f+up.z*.125f,up.x*.25f,up.y*.125f};matrix->GetForward()=CVector{up.y*.25f,1.f,up.x*.125f};}
inline void __fastcall Copy(CMatrix* self,void*,const CMatrix& other){trace.push_back({"copy",Id(self),Id(&other)});memcpy(self,&other,0x40);}
inline CVector* __cdecl Transform(CVector* output,const CMatrix& matrix,const CVector& point){trace.push_back({"transform",Id(&matrix),F(point.x),F(point.y),F(point.z)});*output=CVector{(point.x*matrix.GetRight().x+point.y*matrix.GetForward().x)+point.z*matrix.GetUp().x+matrix.GetPosition().x,(point.x*matrix.GetRight().y+point.y*matrix.GetForward().y)+point.z*matrix.GetUp().y+matrix.GetPosition().y,(point.x*matrix.GetRight().z+point.y*matrix.GetForward().z)+point.z*matrix.GetUp().z+matrix.GetPosition().z};return output;}
inline void __fastcall Ref(void* self,void*,CEntity** ref){++positive;trace.push_back({"reference",Id(self),uintptr_t(ref)-base});}
inline void __fastcall SetPos(void* self,void*,const CVector& position){trace.push_back({"set_pos",Id(self),F(position.x),F(position.y),F(position.z)});Pos(self)=position;}
inline bool __fastcall IsPlayer(void* self,void*){trace.push_back({"is_player",Id(self)});return scenario%9==0;}
inline void __fastcall SetStatic(void* self,void*,bool value){trace.push_back({"static",Id(self),value});auto& f=*reinterpret_cast<uint32*>(static_cast<uint8*>(self)+0x1C);f=value?f|4:f&~4u;}
inline void __fastcall Moving(void* self,void*){++positive;trace.push_back({"moving",Id(self)});}
inline void __fastcall Release(CRope* self,void*){++positive;trace.push_back({"release"});self->m_pRopeAttachObject=nullptr;self->m_nFlags1=60;}
inline void __fastcall Remove(CRope* self,void*){++positive;trace.push_back({"remove"});self->m_nType=eRopeType::NONE;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,2> fns{{{"CRope","UpdateWeightInRope",0x5561B0,0,-1,1024},{"CRope","Update",0x557530,0,-1,1024}}};
struct Region{uintptr_t address;size_t length;std::vector<uint8> saved;};
inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"rope-pending-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"rope-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("rope-pending");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"rope-pending-only.txt");if(f)std::getline(f,only);}
    std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};std::array<bool,2> enabled{},locked{};
    for(size_t n=0;n<2;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
    std::array<Region,13> regions{{{0xB6F02C,0x14,{}},{0xB7CB84,4,{}},{0xB7CB4C,4,{}},{0xB7CB5C,4,{}},{0xB76898,4,{}},{0xA44494,3,{}},{0xB74490,0x10,{}},{0x8CD6C8,0x78,{}},{0xA9B0C8+300*4,80,{}},{0xA9B0C8+428*4,4,{}},{0xA9B0C8+564*4,4,{}},{uintptr_t(CPad::GetPad(0))+0x1C,2,{}},{uintptr_t(CPad::GetPad(0))+0x20,2,{}}}};
    for(auto& region:regions){region.saved.resize(region.length);memcpy(region.saved.data(),reinterpret_cast<void*>(region.address),region.length);}uint32 pairs=0,totalFailures=0;
    {
        using ReversingProbe::EntryPatch;using std::bit_cast;
        EntryPatch a(0x59C910,Normalise),ad(bit_cast<uintptr_t>(&CVector::Normalise),Normalise);
        EntryPatch b(0x821B1E,Random),bd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
        EntryPatch c(0x5696C0,Ground),cd(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),Ground);
        EntryPatch d(0x53FC10,Axis),dd(bit_cast<uintptr_t>(&CPad::GetCarGunUpDown),Axis);
        EntryPatch e(0x53FFE0,Button),ed(bit_cast<uintptr_t>(&CPad::CarGunJustDown),Button);
        EntryPatch f(0x5073B0,Mission),fd(bit_cast<uintptr_t>(static_cast<void(CAudioEngine::*)(uint16,CPhysical*,float,float)>(&CAudioEngine::ReportMissionAudioEvent)),Mission);
        EntryPatch g(0x542CE0,Speed),gd(bit_cast<uintptr_t>(&CPhysical::GetSpeed),Speed);
        EntryPatch h(0x59B7E0,Force),hd(bit_cast<uintptr_t>(&CMatrix::ForceUpVector),Force);
        EntryPatch j(0x59BBC0,Copy),jd(bit_cast<uintptr_t>(&CMatrix::operator=),Copy);
        EntryPatch k(0x59C890,Transform),kd(bit_cast<uintptr_t>(static_cast<CVector(*)(const CMatrix&,const CVector&)>(&operator*)),Transform);
        EntryPatch l(0x571B70,Ref),ld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Ref);
        EntryPatch m(0x4241C0,SetPos),md(bit_cast<uintptr_t>(static_cast<void(CPlaceable::*)(const CVector&)>(&CPlaceable::SetPosn)),SetPos);
        EntryPatch o(0x5DF8F0,IsPlayer),od(bit_cast<uintptr_t>(&CPed::IsPlayer),IsPlayer);
        EntryPatch p(0x542800,Moving),pd(bit_cast<uintptr_t>(&CPhysical::AddToMovingList),Moving);
        EntryPatch q(0x556030,Release),qd(bit_cast<uintptr_t>(&CRope::ReleasePickedUpObject),Release);
        EntryPatch r(0x556780,Remove),rd(bit_cast<uintptr_t>(&CRope::Remove),Remove);
        vt.fill(uintptr_t(&SetStatic));vt[4]=uintptr_t(&SetStatic);
        for(size_t n=0;n<2;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;std::unique_ptr<EntryPatch> weightExe,weightDll;if(n==1){weightExe=std::make_unique<EntryPatch>(0x5561B0,Weight);weightDll=std::make_unique<EntryPatch>(bit_cast<uintptr_t>(&CRope::UpdateWeightInRope),Weight);}
            for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CRope)> object{};auto run=[&](bool candidate){scenario=uint32(i);positive=rng=0;base=uintptr_t(object.data());trace=json::array();for(auto& region:regions)memcpy(reinterpret_cast<void*>(region.address),region.saved.data(),region.length);memset(object.data(),0xCD,object.size());auto* self=reinterpret_cast<CRope*>(object.data());self->m_nSegments=uint8(n?i%31:i%32);self->m_fTotalLength=.125f+float(i%7)*.125f;self->m_fSegmentLength=.5f;self->m_fMass=10.f;CTimer::ms_fTimeStep=float(i%5+1)*.25f;for(int s=0;s<32;++s){self->m_aSegments[s]=CVector{float(s)*.25f,float((s+i)%9)*.125f,float(s%5)*.25f};self->m_aSpeed[s]=CVector{float(s)*.0625f,-float(s)*.03125f,float(i%3)*.25f};}
                    if(n){
                        memset(reinterpret_cast<void*>(0xB6F02C),0,0x14);StaticRef<uint32>(0xB7CB84)=1000;StaticRef<uint32>(0xB7CB4C)=i%8;StaticRef<int32>(0xB76898)=i%5;StaticRef<uint8>(0xA44494)=uint8((i/3)%2);StaticRef<uint8>(0xA44495)=uint8((i/5)%2);StaticRef<uint8>(0xA44496)=uint8((i/7)%2);*reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0))+0x1C)=int16((i%3-1)*128);*reinterpret_cast<int16*>(reinterpret_cast<uint8*>(CPad::GetPad(0))+0x20)=int16((i%5-2)*64);
                        for(int s=0;s<20;++s){memset(infos[s].data(),0,infos[s].size());memset(cols[s].data(),0,cols[s].size());*reinterpret_cast<uintptr_t*>(infos[s].data()+0x14)=uintptr_t(cols[s].data());*reinterpret_cast<float*>(cols[s].data()+0x14)=.5f;StaticRef<uintptr_t>(0xA9B0C8+(300+s)*4)=uintptr_t(infos[s].data());}StaticRef<uintptr_t>(0xA9B0C8+428*4)=StaticRef<uintptr_t>(0xA9B0C8+564*4)=uintptr_t(infos[0].data());for(int s=0;s<30;++s)StaticRef<int32>(0x8CD6C8+s*4)=300+s%20;
                        const std::array<void*,6> entities{holder.data(),attached.data(),current.data(),ped.data(),vehicle.data(),pickup.data()};const auto end=self->m_aSegments[31];for(int s=0;s<6;++s){const auto bytes=s==3?ped.size():s==5?pickup.size():holder.size();memset(entities[s],0,bytes);memset(matrices[s].data(),0,matrices[s].size());auto* raw=static_cast<uint8*>(entities[s]);*reinterpret_cast<uintptr_t*>(raw)=uintptr_t(vt.data());*reinterpret_cast<uintptr_t*>(raw+0x14)=uintptr_t(matrices[s].data());*reinterpret_cast<int16*>(raw+0x22)=300;raw[0x36]=uint8(s==3?3:s==5?4:2);*reinterpret_cast<float*>(raw+0x8C)=100.f;*reinterpret_cast<CVector*>(raw+0x44)=CVector{.25f,-.125f,.0625f};auto* mat=reinterpret_cast<CMatrix*>(matrices[s].data());mat->GetRight()=CVector{1.f,0.f,0.f};mat->GetForward()=CVector{0.f,1.f,0.f};mat->GetUp()=CVector{0.f,0.f,1.f};mat->GetPosition()=CVector{end.x,end.y,end.z-.5f};}
                        *reinterpret_cast<uint32*>(current.data()+0x40)=i%13==0?0x20000000:0;*reinterpret_cast<int16*>(current.data()+0x22)=i%7==0?428:300;*reinterpret_cast<int32*>(current.data()+0x594)=i%11==0?9:0;*reinterpret_cast<uintptr_t*>(current.data()+0x460)=i%11==0?1:0;*reinterpret_cast<int32*>(ped.data()+0x530)=i%7==0?55:1;*reinterpret_cast<uint32*>(ped.data()+0x46C)=i%11==0?0x100:0;vehicle[0x42D]=i%9?0x10:0;vehicle[0x4A8]=i%11==0?0x20:0;*reinterpret_cast<int16*>(vehicle.data()+0x22)=i%8==2?564:300;*reinterpret_cast<uint32*>(pickup.data()+0x140)=0x40000;*reinterpret_cast<uint32*>(pickup.data()+0x1C)=4;
                        const std::array<void*,3> storage{ped.data(),vehicle.data(),pickup.data()};for(int s=0;s<3;++s){map[s]=i%9==0?-1:0;pools[s]={uintptr_t(storage[s]),uintptr_t(&map[s]),1,0};}StaticRef<uintptr_t>(0xB74490)=uintptr_t(pools[0].data());StaticRef<uintptr_t>(0xB74494)=uintptr_t(pools[1].data());StaticRef<uintptr_t>(0xB7449C)=uintptr_t(pools[2].data());
                        self->m_nType=static_cast<eRopeType>(1+i%8);self->m_nFlags1=i%6==0?2:0;self->m_nFlags2=uint8(i%8);self->m_nTime=i%2?900:1100;self->m_fGroundZ=-1.f;self->m_fSegmentLength=float(i%13)*.1f;self->m_fMass=10.f;self->m_pRopeHolder=reinterpret_cast<CEntity*>(holder.data());self->m_pAttachedEntity=reinterpret_cast<CEntity*>(attached.data());self->m_pRopeAttachObject=i%3==0?nullptr:reinterpret_cast<CEntity*>(current.data());if(i%17==0)self->m_aSegments[0].x=300.f;if(i%23==0)self->m_aSegments[0].z=-60.f;
                    }
                    std::array<float,3> out;memset(out.data(),0xCD,sizeof(out));const auto anchor=self->m_aSegments[self->m_nSegments];const auto x=anchor.x+(i%3==0?100.f:.25f+float(i%7)*.125f),y=anchor.y+float(i%9)*.125f,z=anchor.z+float(i%11)*.125f;
                    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};const auto before=Hash(object.data(),object.size());int8 ret=0;if(!n)ret=reinterpret_cast<int8(__thiscall*)(void*,float,float,float,int32,float*)>(fn.address)(self,x,y,z,i,out.data());else reinterpret_cast<void(__thiscall*)(void*)>(fn.address)(self);if(!candidate&&(Hash(object.data(),object.size())!=before||positive))++positiveCases;json hashes=json::array();for(const auto& region:regions)hashes.push_back(Hash(reinterpret_cast<void*>(region.address),region.length));return json{{"switch",true},{"return",ret},{"object",Hash(object.data(),object.size())},{"out",Hash(out.data(),sizeof(out))},{"entities",n?json{Hash(holder.data(),holder.size()),Hash(attached.data(),attached.size()),Hash(current.data(),current.size()),Hash(ped.data(),ped.size()),Hash(vehicle.data(),vehicle.size()),Hash(pickup.data(),pickup.size()),Hash(matrices.data(),sizeof(matrices)),Hash(pools.data(),sizeof(pools))}:json{}},{"globals",hashes},{"trace",trace}};
                };
                const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
            }
            if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
        }
    }
    bool restored=true;for(auto& region:regions){memcpy(reinterpret_cast<void*>(region.address),region.saved.data(),region.length);restored=restored&&memcmp(reinterpret_cast<void*>(region.address),region.saved.data(),region.length)==0;}for(size_t n=0;n<2;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
    Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}
