#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "SurfaceInfos_c.h"
#include "AEVehicleAudioEntity.h"
#include "CarCtrl.h"
#include "World.h"
#include "Camera.h"
#include "Explosion.h"
#include "Darkel.h"
#include "VisibilityPlugins.h"
#include "../ProbeRuntime.hpp"
namespace BikeBlow355Live {
inline bool done{};inline std::ofstream log;inline json trace;
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline CBike* current{};inline CEntity* damager{};
inline void __cdecl Clump(RpClump*,int32 flags){trace.push_back({"clump",flags});}
inline void __fastcall Shake(void*,void*,float strength,CVector position){trace.push_back({"shake",std::bit_cast<uint32>(strength),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z)});current->GetPosition().z+=.5f;}
inline void __fastcall Kill(void*,void*){trace.push_back({"kill"});reinterpret_cast<uint8*>(current)[0x428]=0xFF;reinterpret_cast<uint8*>(current)[0x4A8]=0xFF;}
inline void __fastcall Law(void*,void*,bool state){trace.push_back({"law",state});current->GetPosition().x+=.25f;}
inline void __cdecl Explosion(CEntity* victim,CEntity* creator,eExplosionType type,CVector position,uint32 lifetime,uint8 sound,float shake,uint8 visible){trace.push_back({"explosion",victim==current,creator==damager,int(type),std::bit_cast<uint32>(position.x),std::bit_cast<uint32>(position.y),std::bit_cast<uint32>(position.z),lifetime,sound,std::bit_cast<uint32>(shake),visible});}
inline void __cdecl Darkel(CVehicle& vehicle,int32 player){trace.push_back({"darkel",&vehicle==current,player});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","BlowUpCar",0,0x871360,41,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"bike-blow355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-blow355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-blow355");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream f(dir/"bike-blow355-only.txt");if(f)std::getline(f,only);}
 std::array<ReversibleHooks::ReversibleHook::Base*,1> hooks{};std::array<bool,1> enabled{},locked{};
 for(size_t n=0;n<1;++n){hooks[n]=FindHook(fns[n].cls,fns[n].name);if(!hooks[n]){Write({{"event","error"},{"reason","missing hook"},{"function",fns[n].name}});return;}enabled[n]=hooks[n]->Hooked();locked[n]=hooks[n]->Locked();hooks[n]->LockState(false);}
 uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch cl(0x7322F0,Clump),cld(bit_cast<uintptr_t>(&CVisibilityPlugins::SetClumpForAllAtomicsFlag),Clump);
 EntryPatch sh(0x50A9F0,Shake),shd(bit_cast<uintptr_t>(&CCamera::CamShake),Shake);
 EntryPatch ki(0x6D1C80,Kill),kid(bit_cast<uintptr_t>(&CVehicle::KillPedsInVehicle),Kill);
 EntryPatch la(0x6D2330,Law),lad(bit_cast<uintptr_t>(&CVehicle::ChangeLawEnforcerState),Law);
 EntryPatch ex(0x736A50,Explosion),exd(bit_cast<uintptr_t>(&CExplosion::AddExplosion),Explosion);
 EntryPatch da(0x43DF20,Darkel),dad(bit_cast<uintptr_t>(&CDarkel::RegisterCarBlownUpByPlayer),Darkel);
 for(size_t n=0;n<1;++n){const auto& fn=fns[n];const std::string full=std::string(fn.cls)+"::"+fn.name;if(!only.empty()&&only!=full)continue;uint32 failures=0,positiveCases=0;
  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(CEntity)> offender{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(offender.data(),0,offender.size());current=reinterpret_cast<CBike*>(object.data());damager=i%3==0?nullptr:reinterpret_cast<CEntity*>(offender.data());*reinterpret_cast<uintptr_t*>(current)=0x871360;current->m_matrix=i%2?reinterpret_cast<CMatrixLink*>(matrix.data()):nullptr;*reinterpret_cast<CVector*>(matrix.data()+0x30)=CVector{float(i%7),float(i%11),float(i%13)*.25f};current->GetPosition()=CVector{float(i%7),float(i%11),float(i%13)*.25f};current->m_vecMoveSpeed=CVector{.25f,-.5f,float(i%17-8)*.125f};object[0x42A]=uint8((i%5==0?0:0x20)|uint8(i%8));object[0x36]=uint8(i%64);current->m_nPhysicalFlags=uint32(i*123);current->m_fHealth=float(i%19+1);current->m_wBombTimer=uint16(i+1);*reinterpret_cast<void**>(object.data()+0x18)=nullptr;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6BEA10)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*,CEntity*,bool)>(address)(current,damager,i%7!=0);current->m_matrix=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"trace",trace}};
   };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",pairs++},{"function",full},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }if(!positiveCases)++failures;totalFailures+=failures;Write({{"event","function_summary"},{"function",full},{"cases",fn.cases},{"failures",failures},{"positive_cases",positiveCases}});
 }
 }
 bool restored=true;
 for(size_t n=0;n<1;++n){hooks[n]->State(enabled[n]);hooks[n]->LockState(locked[n]);restored=restored&&hooks[n]->Hooked()==enabled[n]&&hooks[n]->Locked()==locked[n];}
 Write({{"event",totalFailures?"error":"complete"},{"pairs",pairs},{"failures",totalFailures},{"state_restored",restored}});log.close();
}
}




