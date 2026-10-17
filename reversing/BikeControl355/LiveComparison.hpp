#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Bike.h"
#include "cTransmission.h"
#include "CollisionData.h"
#include "ColModel.h"
#include "EventVehicleOnFire.h"
#include "EventGroup.h"
#include "FxSystem.h"
#include "FxManager.h"
#include "Pad.h"
#include "SurfaceInfos_c.h"
#include "CarCtrl.h"
#include "World.h"
#include "RwHelper.h"
#include "PlayerPed.h"
#include "../ProbeRuntime.hpp"
namespace BikeControl355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline CBike* current{};inline CColModel* colModel{};inline int scenario{},physicsCalls{},wheelCalls{},unwindCalls{};inline bool throwing{};
inline std::array<uint8,256> player{},association{},effect{},group{};
template<class T>T& At(void* p,size_t offset){return *reinterpret_cast<T*>(static_cast<uint8*>(p)+offset);}
inline uint32 F(float value){return std::bit_cast<uint32>(value);}
inline json V(const CVector& v){return {F(v.x),F(v.y),F(v.z)};}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline void Write(const json& r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
template<int Tag>void __fastcall Void(void*,void*){trace.push_back({"void",Tag});}
inline CColModel* __fastcall Col(void*,void*){trace.push_back({"col"});return colModel;}
inline bool __fastcall AI(void*,void*,uint32& flags){flags=scenario%9==0?2:0;trace.push_back({"ai",flags});if(scenario%17==0)current->m_vecTurnSpeed.x+=.001f;return false;}
inline bool __fastcall IsPlayer(void*,void*){trace.push_back({"is_player"});return scenario%3==0;}
inline float __cdecl Modifier(int32 id){trace.push_back({"modifier",id});return 1.f+float(scenario%5)*.125f;}
inline void __fastcall Turn(void*,void*,CVector force,CVector point){trace.push_back({"turn",V(force),V(point)});current->m_vecTurnSpeed.y+=.000125f;}
inline void __fastcall Move(void*,void*,CVector force){trace.push_back({"move",V(force)});current->m_vecMoveSpeed.x+=.000125f;}
inline bool __cdecl Restricted(float x,float y){trace.push_back({"restricted",F(x),F(y)});return scenario%7==0;}
inline void __fastcall Damage(void*,void*,float damage,eVehicleCollisionComponent part,CEntity* source,CVector* point,CVector* direction,eWeaponType weapon){trace.push_back({"damage",F(damage),int(part),source!=nullptr,point!=nullptr,direction!=nullptr,int(weapon)});}
inline void __fastcall Collision(void*,void*,bool apply){trace.push_back({"collision",apply});}
inline void __fastcall Physics(void*,void*){trace.push_back({"physics"});++physicsCalls;current->m_vecMoveSpeed.y+=.00025f;}
inline void __fastcall Blow(void*,void*,CEntity* source,bool hide){trace.push_back({"blow",source!=nullptr,hide});current->m_fHealth=-1.f;}
inline float __fastcall Drive(void*,void*,const float& gas,uint8& gear,float& count,float& velocity,float* a,float* b,uint8 grounded,uint8 type){trace.push_back({"drive",F(gas),gear,F(count),F(velocity),grounded,type});gear=uint8((gear+1)%5);count+=.125f;return .03125f;}
inline bool __fastcall Spring(void*,void*,float force,CVector& direction,CVector& point,float length,float bias,float& limit){trace.push_back({"spring",F(force),V(direction),V(point),F(length),F(bias)});limit=.0625f;return true;}
inline bool __fastcall SpringAlt(void*,void*,float force,CVector& direction,CVector& point,float length,float bias,CVector& normal,float& limit){trace.push_back({"spring_alt",F(force),V(direction),V(point),F(length),F(bias),V(normal)});limit=.125f;return true;}
inline bool __fastcall Damping(void*,void*,float force,float limit,CVector& direction,CVector& point,CVector& collision){trace.push_back({"damping",F(force),F(limit),V(direction),V(point),V(collision)});return true;}
inline CVector* __fastcall Speed(void*,void*,CVector* result,CVector point){trace.push_back({"speed",V(point)});*result=current->m_vecMoveSpeed;return result;}
// 0x6BAAD2: el EXE pasa un CColPoint de la pila sin inicializar con solo los tipos de superficie A y B escritos; GetAdhesiveLimit solo lee esos dos bytes.
inline float __fastcall Adhesive(void*,void*,CColPoint* point){trace.push_back({"adhesive",int(point->m_nSurfaceTypeA),int(point->m_nSurfaceTypeB)});return .75f;}
inline uint32 __fastcall Adhesion(void*,void*,uint32 id){trace.push_back({"adhesion",id});return uint32(scenario%6);}
inline float __fastcall Wet(void*,void*,uint32 id){trace.push_back({"wet",id});return .5f;}
inline uint32 __fastcall Skid(void*,void*,uint32 id){trace.push_back({"skid",id});return uint32(scenario%3);}
inline uint32 __fastcall Rough(void*,void*,uint32 id){trace.push_back({"rough",id});return uint32(scenario%4);}
inline void __fastcall Wheel(void*,void*,CVector& forward,CVector& right,CVector& speed,CVector& point,int32 grounded,float thrust,float brake,float adhesion,float destabilization,int8 id,float* rotation,tWheelState* state,eBikeWheelSpecial special,uint16 status){trace.push_back({"wheel",V(forward),V(right),V(speed),V(point),grounded,F(thrust),F(brake),F(adhesion),F(destabilization),id,F(*rotation),int(*state),int(special),status});++wheelCalls;*rotation+=.03125f;*state=tWheelState(scenario%4);}
inline void* __cdecl Anim(void*,uint32 id){trace.push_back({"anim",id});return scenario%4==0?association.data():nullptr;}
inline bool __cdecl Extra(){trace.push_back({"extra"});return scenario%2!=0;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return reinterpret_cast<CPlayerPed*>(player.data());}
// CPad::GetPad es inline en la DLL (lee Pads[n]): no tiene direccion que interceptar, asi que su registro "pad" (indice constante 0) no puede producirse en el candidato.
inline CPad* __cdecl Pad(int32){return reinterpret_cast<CPad*>(group.data());}
inline void __fastcall Shake(void*,void*,int16 duration,uint8 frequency,uint32 delay){trace.push_back({"shake",duration,frequency,delay});}
inline void __cdecl Danger(CVehicle*){trace.push_back({"danger"});}
inline void __fastcall Siren(void*,void*,bool arg){trace.push_back({"siren",arg});}
inline FxSystem_c* __fastcall Create(void*,void*,const char* name,const CVector& position,RwMatrix* matrix,bool ignore){trace.push_back({"create",std::string(name),V(position),matrix!=nullptr,ignore});return scenario%5==0?nullptr:reinterpret_cast<FxSystem_c*>(effect.data());}
// El constructor y el destructor de CEventVehicleOnFire de la DLL no son direccionables (el candidato los ejecuta de verdad): sin registro en la traza; el destructor solo cuenta para la compuerta del original.
inline void __fastcall EventCtor(void* event,void*,CVehicle* vehicle){memset(event,0,sizeof(CEventVehicleOnFire));reinterpret_cast<CEventVehicleOnFire*>(event)->m_vehicle=vehicle;}
inline void __fastcall EventDtor(void*,void*){++unwindCalls;}
inline void* __cdecl Group(){trace.push_back({"group"});return group.data();}
inline void* __fastcall AddEvent(void*,void*,void* event,bool valid){trace.push_back({"event_add",valid});if(throwing)throw 355;return event;}
inline ReversibleHooks::ReversibleHook::Base* FindNamedHook(const char* cls,const char* fn){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});return r;}
// Direccion real de una funcion de la DLL: se saltan los trampolines de enlace incremental (jmp) que no salen hacia el EXE.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(){ReversibleHooks::ReversibleHook::Base* r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CBike")for(auto& h:c.Items())if(h->Name()=="ProcessControl")r=h.get();});return r;}
struct Fn{const char* cls;const char* name;uintptr_t address;uintptr_t vt;int slot;int cases;};
inline const std::array<Fn,1> fns{{{"CBike","ProcessControl",0,0x871360,10,1024}}};
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"bike-control355-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"bike-control355-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("bike-control355");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 auto* hook=FindHook();if(!hook){Write({{"event","error"},{"reason","missing hook"}});return;}const bool enabled=hook->Hooked(),locked=hook->Locked();hook->LockState(false);uint32 failures=0,positivePhysics=0,positiveWheels=0,positiveUnwind=0;
 struct Region{uintptr_t address;size_t length;std::vector<uint8> saved;};std::array<Region,7> regions{{{0xA43088,1},{0xB7CB40,80},{0xB7CD6E,1},{0xC1C260,32},{0xC1C818,4},{0xA9B0C8+522*4,4},{0x97D6C0,32}}};for(auto& r:regions){r.saved.resize(r.length);memcpy(r.saved.data(),reinterpret_cast<void*>(r.address),r.length);}
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch co(0x535300,Col),au(0x502280,Void<1>),al(0x6D21F0,Void<2>),bo(0x6D2570,Void<3>),cl(0x6D2980,Void<4>),ip(0x5DF8F0,IsPlayer),mo(0x559AF0,Modifier),tu(0x542A50,Turn),mv(0x5429F0,Move),re(0x424100,Restricted),ph(0x5485E0,Physics),bu(0x6B5FB0,Void<5>),sk(0x5433B0,Void<6>),dr(0x6D05E0,Drive);
 EntryPatch sp(0x543C90,Spring),sa(0x543D60,SpringAlt),da(0x543E90,Damping),sd(0x542CE0,Speed),ad(0x55EB50,Adhesive),ag(0x55E5C0,Adhesion),we(0x55E600,Wet),sm(0x55E630,Skid),ro(0x55E810,Rough),wh(0x6D73B0,Wheel),an(0x4D68B0,Anim),ex(0x72DDD0,Extra),pl(0x56E210,Player),pa(0x53FB70,Pad),sh(0x53F920,Shake),de(0x42CE40,Danger),si(0x6E0950,Siren),br(0x60A8A0,Void<7>),delex(0x6D1340,Void<8>);
 EntryPatch fc(0x4A9BE0,Create),fp(0x4AA2F0,Void<9>),fk(0x4AA3F0,Void<10>),ec(0x4B10C0,EventCtor),ed(0x4B1170,EventDtor),eg(0x4ABA50,Group),ea(0x4AB420,AddEvent);
 // Entradas de la DLL: CBike::ProcessControl ya es C++ y llama directamente a los colaboradores reconstruidos; las direcciones del EXE de arriba solo cubren a los que reenvian al EXE
 // (CVehicle::ProcessBikeWheel 0x6D73B0, CVehicle::ProcessSirenAndHorn 0x6E0950 y CCarCtrl::ScanForPedDanger 0x42CE40). Cada entrada usa el mismo Fake que su direccion del EXE.
 EntryPatch dco(Body(bit_cast<uintptr_t>(static_cast<CColModel*(CEntity::*)() const>(&CEntity::GetColModel))),Col),dau(Body(bit_cast<uintptr_t>(&CAEVehicleAudioEntity::Service)),Void<1>),dal(Body(bit_cast<uintptr_t>(&CVehicle::ProcessCarAlarm)),Void<2>),dbo(Body(bit_cast<uintptr_t>(&CVehicle::ActivateBombWhenEntered)),Void<3>),dcl(Body(bit_cast<uintptr_t>(&CVehicle::UpdateClumpAlpha)),Void<4>),dip(Body(bit_cast<uintptr_t>(&CPed::IsPlayer)),IsPlayer),dmo(Body(bit_cast<uintptr_t>(&CStats::GetFatAndMuscleModifier)),Modifier);
 EntryPatch dtu(Body(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce)),Turn),dmv(Body(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce))),Move),dre(Body(bit_cast<uintptr_t>(&Native_00424100)),Restricted),dph(uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindNamedHook("CPhysical","ProcessControl"))->GetHookOurAddress()),Physics),dbu(Body(bit_cast<uintptr_t>(&CBike::ProcessBuoyancy)),Void<5>),dsk(Body(bit_cast<uintptr_t>(&CPhysical::SkipPhysics)),Void<6>),ddr(Body(bit_cast<uintptr_t>(&cTransmission::CalculateDriveAcceleration)),Drive);
 EntryPatch dsp(Body(bit_cast<uintptr_t>(&CPhysical::ApplySpringCollision)),Spring),dsa(Body(bit_cast<uintptr_t>(&CPhysical::ApplySpringCollisionAlt)),SpringAlt),dda(Body(bit_cast<uintptr_t>(&CPhysical::ApplySpringDampening)),Damping),dsd(Body(bit_cast<uintptr_t>(&CPhysical::GetSpeed)),Speed),dad(Body(bit_cast<uintptr_t>(&SurfaceInfos_c::GetAdhesiveLimit)),Adhesive),dag(Body(bit_cast<uintptr_t>(&SurfaceInfos_c::GetAdhesionGroup)),Adhesion),dwe(Body(bit_cast<uintptr_t>(&SurfaceInfos_c::GetWetMultiplier)),Wet),dsm(Body(bit_cast<uintptr_t>(&SurfaceInfos_c::GetSkidmarkType)),Skid),dro(Body(bit_cast<uintptr_t>(&SurfaceInfos_c::GetRoughness)),Rough);
 EntryPatch dan(Body(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation))),Anim),dex(Body(bit_cast<uintptr_t>(&CCullZones::DoExtraAirResistanceForPlayer)),Extra),dpl(Body(bit_cast<uintptr_t>(&FindPlayerPed)),Player),dsh(Body(bit_cast<uintptr_t>(&CPad::StartShake)),Shake),dbr(Body(bit_cast<uintptr_t>(&CPlayerPed::ResetPlayerBreath)),Void<7>),ddelex(Body(bit_cast<uintptr_t>(&CVehicle::ProcessDelayedExplosion)),Void<8>);
 EntryPatch dfc(Body(bit_cast<uintptr_t>(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem))),Create),dfp(Body(bit_cast<uintptr_t>(&FxSystem_c::Play)),Void<9>),dfk(Body(bit_cast<uintptr_t>(&FxSystem_c::Kill)),Void<10>),deg(Body(bit_cast<uintptr_t>(&GetEventGlobalGroup)),Group),dea(Body(bit_cast<uintptr_t>(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add))),AddEvent);
 for(int i=0;i<1024;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,80> matrix{};alignas(16) std::array<uint8,256> handling{},bikeHandling{},model{};alignas(CColModel) std::array<uint8,sizeof(CColModel)> col{};alignas(CCollisionData) std::array<uint8,sizeof(CCollisionData)> data{};alignas(CColLine) std::array<CColLine,4> lines{};std::array<uintptr_t,70> vt{};std::array<RwFrame,10> frames{};
 vt[66]=bit_cast<uintptr_t>(&AI);vt[56]=bit_cast<uintptr_t>(&Damage);vt[23]=bit_cast<uintptr_t>(&Collision);vt[41]=bit_cast<uintptr_t>(&Blow);
 auto run=[&](bool candidate){scenario=i;physicsCalls=wheelCalls=unwindCalls=0;throwing=i%32==3;trace=json::array();for(auto& r:regions)memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.length);memset(object.data(),0,object.size());memset(matrix.data(),0,matrix.size());memset(handling.data(),0,handling.size());memset(bikeHandling.data(),0,bikeHandling.size());memset(model.data(),0,model.size());memset(col.data(),0,col.size());memset(data.data(),0,data.size());memset(lines.data(),0,sizeof(lines));memset(frames.data(),0,sizeof(frames));memset(player.data(),0,player.size());memset(association.data(),0,association.size());memset(effect.data(),0,effect.size());memset(group.data(),0,group.size());
 current=reinterpret_cast<CBike*>(object.data());At<uintptr_t>(current,0)=uintptr_t(vt.data());current->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());current->m_matrix->GetRight()=CVector{1,0,.03125f};current->m_matrix->GetForward()=CVector{0,1,.0625f};current->m_matrix->GetUp()=CVector{-.03125f,-.0625f,1};current->GetPosition()=CVector{.25f,-.5f,1.f};current->m_nModelIndex=522;current->SetStatus(static_cast<eEntityStatus>(std::array<uint8,6>{0,3,4,5,2,8}[(i/32)%6]));current->m_vecMoveSpeed=CVector{float(i%5)*.03125f,.125f,.015625f};current->m_vecTurnSpeed=CVector{.001f,-.002f,.001f};current->m_fMass=100.f;current->m_fTurnMass=50.f;current->m_fAirResistance=.01f;current->m_vecCentreOfMass=CVector{0,0,-.125f};current->m_fHealth=i%8==3?100.f:1000.f;current->m_GasPedal=float(i%4)*.25f;current->m_BrakePedal=i%11==0?.5f:0.f;current->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());current->m_BikeHandling=reinterpret_cast<tBikeHandlingData*>(bikeHandling.data());
 for(size_t n=0;n<handling.size();n+=4)At<float>(handling.data(),n)=.5f;for(size_t n=0;n<bikeHandling.size();n+=4)At<float>(bikeHandling.data(),n)=.5f;At<uint32>(handling.data(),0xCC)=0;At<uint32>(handling.data(),0xD0)=0;At<float>(handling.data(),0x14)=.0625f;At<float>(handling.data(),0x18)=.125f;At<float>(handling.data(),0x1C)=-.125f;At<uintptr_t>(current,0x460)=i%6==0?uintptr_t(player.data()):0;At<float>(association.data(),0x18)=.75f;At<float>(association.data(),0x20)=.1f;At<void*>(player.data(),0x18)=group.data();At<float>(current,0x650)=.25f;At<float>(current,0x654)=.125f;At<float>(current,0x658)=1.f;At<float>(current,0x644)=.01f;At<uint8>(current,0x614)=i%13==0?0x18:0;At<uint8>(current,0x42B)=i%15==0?1:0;At<uint8>(current,0x428)=0x10;
 for(int j=0;j<4;++j){current->m_aWheelRatios[j]=i%3==0?1.f:.5f+float(j)*.0625f;current->m_fSuspensionLength[j]=.5f;current->m_fLineLength[j]=1.f;current->m_aWheelColPoints[j].m_vecNormal=CVector{0,0,1};current->m_aWheelColPoints[j].m_vecPoint=CVector{float(j)*.125f,.25f,0};current->m_aRatioHistory[j]=.5f;current->m_WheelCounts[j]=i%2?1.f:-1.f;lines[j].m_vecStart=CVector{float(j)*.125f,0,.5f};lines[j].m_vecEnd=CVector{float(j)*.125f,0,-.5f};}
 for(int j=0;j<10;++j){frames[j].modelling.pos={float(j)*.125f,0,.5f};current->m_aBikeNodes[j]=&frames[j];}current->m_nWheelStatus[0]=i%2;current->m_nWheelStatus[1]=(i/2)%2;current->m_fExtraTractionMult=1.f;current->m_fTyreTemp=1.f;current->m_fBrakingSlide=.125f;
 colModel=reinterpret_cast<CColModel*>(col.data());colModel->m_pColData=reinterpret_cast<CCollisionData*>(data.data());colModel->m_pColData->m_pLines=lines.data();colModel->m_pColData->m_nNumLines=4;StaticRef<uintptr_t>(0xA9B0C8+522*4)=uintptr_t(model.data());At<float>(model.data(),0x40)=.75f;At<float>(model.data(),0x44)=.875f;At<void*>(model.data(),0x5C)=effect.data();At<CVector>(effect.data(),0x54)=CVector{.25f,.5f,.75f};At<void*>(group.data(),4)=&frames[0];At<void*>(current,0x18)=group.data();StaticRef<uint8>(0xA43088)=i%41==0?1:0;StaticRef<uint8>(0xB7CD6E)=0;CTimer::ms_fTimeStep=.25f+float(i%5)*.25f;StaticRef<uint32>(0xB7CB84)=uint32(i);StaticRef<uint32>(0xC1C26C)=0;StaticRef<uint32>(0xC1C270)=0;StaticRef<uint32>(0xC1C27C)=0;
 hook->State(candidate);const uintptr_t entry=reinterpret_cast<uintptr_t*>(0x871360)[10];if((entry!=0x6B9250)!=candidate)return json{{"switch",false}};int exception=0;try{reinterpret_cast<void(__thiscall*)(CBike*)>(entry)(current);}catch(int value){exception=value;trace.push_back({"caught",value});}
 if(!candidate){positivePhysics+=physicsCalls;positiveWheels+=wheelCalls;if(exception==355&&unwindCalls==1)++positiveUnwind;}
 current->m_matrix=nullptr;current->m_pHandlingData=nullptr;current->m_BikeHandling=nullptr;current->m_pDriver=nullptr;std::ranges::fill(current->m_aBikeNodes,nullptr);current->m_pFireParticle=nullptr;At<uintptr_t>(current,0)=0;
 json globals=json::array();for(auto& r:regions)if(r.address!=0xA9B0C8+522*4)globals.push_back(Hash(reinterpret_cast<void*>(r.address),r.length));return json{{"switch",true},{"object",Hash(object.data(),object.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"handling",Hash(handling.data(),handling.size())},{"bike_handling",Hash(bikeHandling.data(),bikeHandling.size())},{"frames",Hash(frames.data(),sizeof(frames))},{"globals",globals},{"exception",exception},{"trace",trace}};
 };const auto original=run(false),candidate=run(true);const bool equal=original==candidate&&original.value("switch",false);if(!equal)++failures;Write({{"event","comparison"},{"pair",i},{"function","CBike::ProcessControl"},{"case",i},{"equal",equal},{"accepted",equal},{"original",original},{"candidate",candidate}});
 }
 }
 for(auto& r:regions)memcpy(reinterpret_cast<void*>(r.address),r.saved.data(),r.length);if(!positivePhysics||!positiveWheels||!positiveUnwind)++failures;hook->State(enabled);hook->LockState(locked);Write({{"event","function_summary"},{"function","CBike::ProcessControl"},{"cases",1024},{"failures",failures},{"physics_calls",positivePhysics},{"wheel_calls",positiveWheels},{"unwind_cases",positiveUnwind}});Write({{"event",failures?"error":"complete"},{"pairs",1024},{"failures",failures},{"state_restored",hook->Hooked()==enabled&&hook->Locked()==locked}});log.close();
}
}
