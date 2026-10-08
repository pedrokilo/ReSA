#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PedDamageResponseCalculator.h"
#include "PedDamageResponse.h"
#include "PedStats.h"
#include "Tasks/TaskTypes/TaskSimpleFight.h"
#include "Events/EventDamage.h"
#include "../ProbeRuntime.hpp"
namespace PedDamagePendingLive {
inline bool done{};inline std::ofstream log;inline json trace;
alignas(16) inline std::array<uint8,sizeof(CPlayerPed)> ped{}, attacker{};
alignas(16) inline std::array<uint8,sizeof(CPedIntelligence)> intel{};
alignas(16) inline std::array<uint8,sizeof(CPlayerPedData)> data{};
alignas(16) inline std::array<uint8,sizeof(CTaskSimpleFight)> fight{};
inline CPedStat stats{};inline int scenario{},rngIndex{};
template<class T=uint32> T& At(void* p,size_t offset){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+offset);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;for(size_t i=0;i<n;++i){h^=static_cast<const uint8*>(p)[i];h*=1099511628211ull;}return h;}
inline uint32 F(float f){return std::bit_cast<uint32>(f);}
inline json Ptr(const void* p){if(!p)return nullptr;const auto v=uintptr_t(p);
#define P(x) if(v>=uintptr_t(x.data())&&v<uintptr_t(x.data())+x.size())return json::array({#x,v-uintptr_t(x.data())});
P(ped) P(attacker) P(intel) P(data) P(fight)
#undef P
return v;}
inline void Write(const json& row){ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();}
inline bool __fastcall Player(const CPed* p,void*){const bool b=p->m_nPedType==PED_TYPE_PLAYER1||p->m_nPedType==PED_TYPE_PLAYER2;trace.push_back({"player",Ptr(p),b});return b;}
inline CPlayerPed* __cdecl FindPlayer(int id){trace.push_back({"find_player",id});return reinterpret_cast<CPlayerPed*>(scenario&1?attacker.data():ped.data());}
inline CVehicle* __cdecl FindVehicle(int id,bool remote){trace.push_back({"find_vehicle",id,remote});return scenario%5==0?reinterpret_cast<CVehicle*>(attacker.data()):nullptr;}
inline bool __cdecl Same(const CPed* a,const CPed* b){const bool yes=(scenario&4)!=0;trace.push_back({"same_group",Ptr(a),Ptr(b),yes});return yes;}
inline bool __cdecl Friends(const CPed& a,const CPed& b){const bool yes=(scenario&8)!=0;trace.push_back({"friends",Ptr(&a),Ptr(&b),yes});return yes;}
inline bool __cdecl KillAllowed(){const bool b=(scenario&16)==0;trace.push_back({"kill_allowed",b});return b;}
inline int __cdecl Random(){const int r=(scenario*73+17*rngIndex++)&0xffff;trace.push_back({"random",r});return r;}
inline CTaskSimpleFight* __fastcall Fight(CPedIntelligence* p,void*){const bool b=(scenario&32)==0;trace.push_back({"fight",Ptr(p),b});return b?reinterpret_cast<CTaskSimpleFight*>(fight.data()):nullptr;}
inline void __fastcall Reference(CEntity* p,void*,CEntity**){trace.push_back({"reference",Ptr(p)});}
inline void __fastcall Cleanup(CEntity* p,void*,CEntity**){trace.push_back({"cleanup",Ptr(p)});}
inline void __fastcall Annoy(CPlayerPed* p,void*,bool b){trace.push_back({"annoy",Ptr(p),b});}
inline void __fastcall GroupDamage(CPlayerPed* p,void*,CEventDamage& e){trace.push_back({"group_damage",Ptr(p),Ptr(e.m_pSourceEntity),int(e.m_weaponType),int(e.m_pedPieceType),bool(e.m_bWitnessedInVehicle)});}
inline bool __fastcall Member(const CPedGroupMembership*,void*,const CPed* p){trace.push_back({"member",Ptr(p)});return false;}
inline void __fastcall Stats(CPedDamageResponseCalculator* c,void*,CPed*,CPedDamageResponse&){trace.push_back({"stats"});c->m_fDamageFactor*=0.75f;}
inline void __fastcall Armour(CPedDamageResponseCalculator*,void*,CPed*,CPedDamageResponse& r){trace.push_back({"armour"});r.m_fDamageArmor=float(scenario%7);}
inline void __fastcall Kill(CPedDamageResponseCalculator* c,void*,CPed* p,CPedDamageResponse& r,bool speak){trace.push_back({"kill",speak});r.m_fDamageHealth=c->m_fDamageFactor;p->m_fHealth-=c->m_fDamageFactor;r.m_bHealthZero=p->m_fHealth<=0;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline const std::array<Fn,4> fns{{
{"CPedDamageResponseCalculator","AccountForPedDamageStats",0x4AD430,0,-1,1024},
{"CPedDamageResponseCalculator","AccountForPedArmour",0x4AD550,0,-1,1024},
{"CPedDamageResponseCalculator","ComputeWillForceDeath",0x4AD610,0,-1,2048},
{"CPedDamageResponseCalculator","ComputeDamageResponse",0x4B5AC0,0,-1,2048}
}};
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* out{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CPedDamageResponseCalculator")for(auto& h:c.Items())if(h->Name()==name)out=h.get();});return out;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"ped-damage-pending-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"ped-damage-pending-runtime.jsonl");ReversingProbe::InstallFailureCapture("ped-damage-pending");Write({{"event","waiting_for_player"}});}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 std::array<uint8,sizeof(CWorld::Players)> playersSaved{};std::memcpy(playersSaved.data(),&CWorld::Players,playersSaved.size());
 const auto savedFocus=CWorld::PlayerInFocus;const auto savedTime=CTimer::m_snTimeInMilliseconds;const auto savedFactor=CPedDamageResponseCalculator::ms_damageFactor;
 uint32 pairs=0;bool passed=true;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch p(0x5DF8F0,Player),pd(bit_cast<uintptr_t>(&CPed::IsPlayer),Player);
 EntryPatch fp(0x56E210,FindPlayer),fpd(bit_cast<uintptr_t>(&FindPlayerPed),FindPlayer);
 EntryPatch fv(0x56E0D0,FindVehicle),fvd(bit_cast<uintptr_t>(&FindPlayerVehicle),FindVehicle);
 EntryPatch sg(0x5F7F40,Same),sgd(bit_cast<uintptr_t>(&CPedGroups::AreInSameGroup),Same);
 EntryPatch fr(0x601D10,Friends),frd(bit_cast<uintptr_t>(&CPedIntelligence::AreFriends),Friends);
 EntryPatch kl(0x56D2C0,KillAllowed),kld(bit_cast<uintptr_t>(&CLocalisation::KillPeds),KillAllowed);
 EntryPatch rn(0x821B1E,Random),rnd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch ft(0x600F30,Fight),ftd(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskFighting),Fight);
 EntryPatch rf(0x571B70,Reference),rfd(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),Reference);
 EntryPatch cl(0x571A00,Cleanup),cld(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),Cleanup);
 EntryPatch an(0x60A040,Annoy),andll(bit_cast<uintptr_t>(&CPlayerPed::AnnoyPlayerPed),Annoy);
 EntryPatch gd(0x60A110,GroupDamage),gdd(bit_cast<uintptr_t>(&CPlayerPed::MakeGroupRespondToPlayerTakingDamage),GroupDamage);
 EntryPatch mb(0x5F6A10,Member),mbd(bit_cast<uintptr_t>(&CPedGroupMembership::IsMember),Member);
 EntryPatch ki(0x4B3210,Kill),kid(bit_cast<uintptr_t>(&CPedDamageResponseCalculator::ComputeWillKillPed),Kill);
 for(const auto& fn:fns){
  auto* hook=FindHook(fn.name);if(!hook){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}
  const bool old=hook->Hooked(),locked=hook->Locked();hook->LockState(false);uint32 failures=0,positive=0,bleeds=0;
  for(int i=0;i<fn.cases;++i){
   auto run=[&](bool candidate){
    scenario=i;rngIndex=0;trace=json::array();ped.fill(0);attacker.fill(0);intel.fill(0);data.fill(0);fight.fill(0);stats={};
    std::memcpy(&CWorld::Players,playersSaved.data(),playersSaved.size());CWorld::PlayerInFocus=i%2;CTimer::m_snTimeInMilliseconds=0x80000000u+uint32(i)*17;CPedDamageResponseCalculator::ms_damageFactor=25.0f;
    auto* victim=reinterpret_cast<CPed*>(ped.data());auto* damager=reinterpret_cast<CPed*>(attacker.data());
    At<uint8>(ped.data(),0x36)=3;At<uint8>(attacker.data(),0x36)=i%4==0?2:3;
    At<CPedIntelligence*>(attacker.data(),0x47C)=reinterpret_cast<CPedIntelligence*>(intel.data());At<CPlayerPedData*>(attacker.data(),0x480)=reinterpret_cast<CPlayerPedData*>(data.data());
    At<CPlayerPedData*>(ped.data(),0x480)=reinterpret_cast<CPlayerPedData*>(data.data());
    victim->m_nPedType=i%4==0?PED_TYPE_PLAYER1:PED_TYPE_CIVMALE;damager->m_nPedType=(i/3)%3==0?PED_TYPE_PLAYER2:PED_TYPE_CIVFEMALE;
    At<uint8>(ped.data(),0x484)=uint8(i%3);victim->m_pStats=&stats;stats.m_fDefendWeakness=float(i%9)*0.25f;
    At<uint32>(ped.data(),0x470)=i%7==0?0x1000:0;At<uint32>(ped.data(),0x46C)=i%5==0?0x100:0;At<void*>(ped.data(),0x58C)=i%2?attacker.data():nullptr;
    At<int32>(data.data(),0x70)=i%5;At<uint8>(data.data(),0x34)=i%4==0?8:0;At<uint32>(data.data(),0x38)=0;
    At<int8>(fight.data(),0x24)=(i/7)%3==0?11:7;At<int8>(fight.data(),0x25)=(i/11)%2==0?2:1;
    constexpr uint32 edges[]{0,0x80000000,0x3f800000,0xbf800000,0x41c80000,0x42c80000,0x7fc00000,0x7f800000,0xff800000};
    victim->m_fHealth=float(1+i%101);victim->m_fArmour=i%3==0?std::bit_cast<float>(edges[(i/3)%std::size(edges)]):float(i%75);
    alignas(CPedDamageResponseCalculator) std::array<uint8,sizeof(CPedDamageResponseCalculator)> calculator{};calculator.fill(0xCD);
    auto* c=reinterpret_cast<CPedDamageResponseCalculator*>(calculator.data());c->m_pDamager=i%7==0?nullptr:damager;c->m_fDamageFactor=i%3==0?std::bit_cast<float>(edges[(i/9)%std::size(edges)]):float(i%127)*0.5f;c->m_weaponType=static_cast<eWeaponType>(i%60);c->m_bodyPart=static_cast<ePedPieceTypes>(i%11);c->m_bSpeak=(i&8)!=0;
    alignas(CPedDamageResponse) std::array<uint8,sizeof(CPedDamageResponse)> response{};response.fill(0xCD);auto* r=reinterpret_cast<CPedDamageResponse*>(response.data());r->m_bDamageCalculated=fn.address==0x4B5AC0&&i%13==0;r->m_bCheckIfAffectsPed=false;r->m_bHealthZero=false;r->m_bForceDeath=false;
    hook->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;bool result=false;
    if(fn.address==0x4B5AC0){
      EntryPatch st(0x4AD430,Stats),stdll(bit_cast<uintptr_t>(&CPedDamageResponseCalculator::AccountForPedDamageStats),Stats);
      EntryPatch ar(0x4AD550,Armour),ardll(bit_cast<uintptr_t>(&CPedDamageResponseCalculator::AccountForPedArmour),Armour);
      reinterpret_cast<void(__thiscall*)(CPedDamageResponseCalculator*,CPed*,CPedDamageResponse&,bool)>(fn.address)(c,victim,*r,(i&2)!=0);
    }else if(fn.address==0x4AD610)result=reinterpret_cast<bool(__thiscall*)(CPedDamageResponseCalculator*,CPed*,CPedDamageResponse&)>(fn.address)(c,victim,*r);
    else reinterpret_cast<void(__thiscall*)(CPedDamageResponseCalculator*,CPed*,CPedDamageResponse&)>(fn.address)(c,victim,*r);
    return json{{"calculator",Hash(calculator.data(),calculator.size())},{"response",Hash(response.data(),response.size())},{"ped",Hash(ped.data(),ped.size())},{"attacker",Hash(attacker.data(),attacker.size())},{"players",Hash(&CWorld::Players,sizeof(CWorld::Players))},{"returned",result},{"trace",trace},{"switches",switched},{"bleeding",At<uint8>(ped.data(),0x72F)}};
   };
   const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original["switches"]==true;passed&=equal;failures+=!equal;positive+=!original["trace"].empty();bleeds+=original["bleeding"]==200;
   Write({{"event","comparison"},{"pair",pairs++},{"function",std::string(fn.cls)+"::"+fn.name},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
  }
  hook->State(old);hook->LockState(locked);passed&=positive>0;if(fn.address==0x4B5AC0)passed&=bleeds>0;
  Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive",positive},{"bleeding_cases",bleeds}});
 }
 }
 std::memcpy(&CWorld::Players,playersSaved.data(),playersSaved.size());CWorld::PlayerInFocus=savedFocus;CTimer::m_snTimeInMilliseconds=savedTime;CPedDamageResponseCalculator::ms_damageFactor=savedFactor;
 Write({{"event",passed?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
