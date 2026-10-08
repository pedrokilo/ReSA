#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "PostEffects.h"
#include "Sprite.h"
#include "../ProbeRuntime.hpp"
namespace PickupsPendingLive {
inline bool done{};inline std::ofstream log;inline json trace;inline int fixture{},calls{},randomCalls{};inline CPickup pickup;
inline void Write(const json&r){ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();}
inline uint32 F(float x){return std::bit_cast<uint32>(x);}inline json V(const CVector&v){return {F(v.x),F(v.y),F(v.z)};}
inline uint64 Hash(const void*p,size_t n){uint64 h=1469598103934665603ull;auto*b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
inline int __cdecl Rand(){int r=(fixture*37+randomCalls++*997)&32767;trace.push_back({"rand",r});return r;}inline uint16 Random(){return uint16(Rand());}
inline CVector playerPos;
inline CVector* __cdecl Coors(CVector*out,int32 id){trace.push_back({"coors",id});*out=playerPos;return out;}
inline float __cdecl Ground(CVector pos,bool*found,CEntity**ent){trace.push_back({"ground",V(pos),ent!=nullptr});++calls;*found=fixture%8!=0&&(fixture%8!=1||calls>20);return pos.z-1.0f+float(fixture%5)*0.25f;}
inline bool __cdecl Bubble(CVector p,float r){trace.push_back({"bubble",V(p),F(r)});return fixture%8==2;}
inline bool __cdecl Line(const CVector&a,const CVector&b,bool c,bool d,bool e,bool f,bool g,bool h,bool i){trace.push_back({"line",V(a),V(b),c,d,e,f,g,h,i});return fixture%8!=3;}
inline CEntity* __cdecl Sphere(CVector p,float radius,CEntity*ignore,bool a,bool b,bool c,bool d,bool e,bool f){trace.push_back({"sphere",V(p),F(radius),ignore!=nullptr,a,b,c,d,e,f});return fixture%8==4?reinterpret_cast<CEntity*>(1):nullptr;}
inline CPickup* __cdecl Find(CObject*){trace.push_back({"pickup"});return &pickup;}
inline bool __fastcall Invisible(void*,void*){trace.push_back({"invisible"});return fixture%8==0;}
inline eWeaponType __cdecl Weapon(int32 model){trace.push_back({"weapon",model});return static_cast<eWeaponType>(10+fixture%30);}
inline bool __cdecl Vision(){trace.push_back({"vision"});return fixture%2!=0;}
inline bool __cdecl Screen(const RwV3d*p,RwV3d*out,float*w,float*h,bool a,bool b){trace.push_back({"screen",{F(p->x),F(p->y),F(p->z)},a,b});*out={320.0f,240.0f,0.5f};*w=2.0f;*h=3.0f;return fixture%8!=1;}
inline const char* __cdecl Label(ePickupPropertyText i){trace.push_back({"label",int(i)});return "FIXTURE";}
inline GxtChar words[]{'x',0};
inline const GxtChar* __fastcall Text(void*,void*,const char*s){trace.push_back({"text",s});return words;}
inline void __cdecl Corona(uint32,CEntity*at,uint8 r,uint8 g,uint8 b,uint8 alpha,const CVector&p,float sz,float range,eCoronaType type,eCoronaFlareType flare,eCoronaReflType refl,eCoronaLOSCheck los,eCoronaTrail trail,float angle,bool neon,float pull,bool full,float fade,bool below,bool white){trace.push_back({"corona",at!=nullptr,r,g,b,alpha,V(p),F(sz),F(range),int(type),int(flare),int(refl),int(los),int(trail),F(angle),neon,F(pull),full,F(fade),below,white});}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char*c,const char*n){ReversibleHooks::ReversibleHook::Base*r{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&x){if(x.Name()==c)for(auto&h:x.Items())if(h->Name()==n)r=h.get();});return r;}
struct Fn{const char*cls,*name;uintptr_t address,vt;int slot,cases;};inline const std::array<Fn,2> fns{{{"CPickups","CreatePickupCoorsCloseToCoors-",0x458A80,0,-1,1024},{"CPickups","DoPickUpEffects",0x455720,0,-1,1024}}};
alignas(16) inline std::array<uint8,sizeof(CObject)> object;alignas(16) inline std::array<uint8,0x50> matrix,cameraMatrix;alignas(16) inline std::array<uint8,0x40> model;alignas(16) inline std::array<uint8,0x30> collision;
inline json Run(int f,int k,bool candidate){fixture=k;calls=randomCalls=0;trace=json::array();float x=-123,y=-456,z=-789;
 if(f==0){playerPos=CVector{k%3==0?0.0f:100.0f,0,0};if(candidate)CPickups::CreatePickupCoorsCloseToCoors(float(k%11),float(k%7),3.0f,x,y,z);else plugin::Call<0x458A80>(float(k%11),float(k%7),3.0f,&x,&y,&z);return {{"out",{F(x),F(y),F(z)}},{"trace",trace}};}
 memset(object.data(),0,object.size());memset(matrix.data(),0xCD,matrix.size());memset(cameraMatrix.data(),0,cameraMatrix.size());memset(model.data(),0,model.size());memset(collision.data(),0,collision.size());memset(&pickup,0,sizeof(pickup));
 *reinterpret_cast<void**>(object.data()+0x14)=matrix.data();*reinterpret_cast<uint8*>(object.data()+0x36)=3;int id=k%16==15?362:k%16==14?400:500+k%14;*reinterpret_cast<int16*>(object.data()+0x22)=int16(id);
 auto*pos=reinterpret_cast<CVector*>(matrix.data()+0x30);*pos=CVector{float(k%20),float(k%4),3.0f};*reinterpret_cast<CVector*>(cameraMatrix.data()+0x30)=CVector{0,0,0};
 *reinterpret_cast<void**>(model.data()+0x14)=collision.data();auto*box=reinterpret_cast<float*>(collision.data());box[0]=box[1]=box[2]=0;box[3]=0.2f+float(k%3);box[4]=0.4f+float(k%5);box[5]=0.7f+float(k%7);
 *reinterpret_cast<void**>(0xA9B0C8+id*4)=model.data();*reinterpret_cast<void**>(0xB6F03C)=cameraMatrix.data();
 auto*models=reinterpret_cast<uint16*>(0x8CD5BC);for(int i=0;i<16;++i)models[i*2]=uint16(500+i);
 *reinterpret_cast<uint32*>(0xB7CB84)=uint32(k*113);*reinterpret_cast<uint8*>(0xB70153)=uint8(k%8);*reinterpret_cast<uint8*>(0xB6F081)=0;*reinterpret_cast<uint16*>(0xB6F1A8)=uint16(k%3==0?46:4);
 uint32 flags=0x40000000u|uint32(k%4)*4;*reinterpret_cast<uint32*>(object.data()+0x140)=flags;object[0x13D]=uint8(k%2);*reinterpret_cast<uint16*>(object.data()+0x13E)=uint16(k%50);pickup.m_nFlags.nPropertyTextIndex=uint8(k%3);
 /* El EXE conserva el argumento como indice de color en estos modelos: fixtures sin texto. */
 if(id==502||id==505||id==506||id==511||id==510){*reinterpret_cast<uint32*>(object.data()+0x140)=0x40000000u;object[0x13D]=0;*reinterpret_cast<uint16*>(object.data()+0x13E)=0;}
 CPickups::NumMessages=uint16(k%17);memset(CPickups::aMessages.data(),0xCD,sizeof(CPickups::aMessages));
 if(candidate)CPickups::DoPickUpEffects(reinterpret_cast<CEntity*>(object.data()));else plugin::Call<0x455720>(reinterpret_cast<CEntity*>(object.data()));
 json rawMessages=json::array();for(auto byte:std::span(reinterpret_cast<const uint8*>(CPickups::aMessages.data()),sizeof(CPickups::aMessages)))rawMessages.push_back(byte);return {{"message_bytes",rawMessages},{"object",Hash(object.data(),object.size())},{"matrix",Hash(matrix.data(),matrix.size())},{"messages",Hash(CPickups::aMessages.data(),sizeof(CPickups::aMessages))},{"count",CPickups::NumMessages},{"trace",trace}};
}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"pickups-pending-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"pickups-pending-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pickups-pending");}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;std::string only;{std::ifstream file(dir/"pickups-pending-only.txt");std::getline(file,only);}std::array<ReversibleHooks::ReversibleHook::Base*,2>hooks;std::array<bool,2>enabled,locked;for(int i=0;i<2;++i){hooks[i]=FindHook(fns[i].cls,fns[i].name);if(!hooks[i]){Write({{"event","error"},{"reason","missing hook"}});return;}enabled[i]=hooks[i]->Hooked();locked[i]=hooks[i]->Locked();hooks[i]->LockState(false);}
 struct Saved{uintptr_t at;std::vector<uint8>bytes;};std::vector<Saved>saved;auto save=[&](uintptr_t at,size_t n){auto*p=reinterpret_cast<uint8*>(at);saved.push_back({at,{p,p+n}});};save(0x8CD5BC,64);save(0xB6F03C,4);save(0xB6F081,1);save(0xB6F1A8,2);save(0xB70153,1);save(0xB7CB84,4);save(0x978678,2);save(0x978680,sizeof(CPickups::aMessages));for(int id=500;id<516;++id)save(0xA9B0C8+id*4,4);save(0xA9B0C8+400*4,4);save(0xA9B0C8+362*4,4);
 bool passed=true;int pairs=0;
 {using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r0(0x821B1E,Rand),r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch c0(0x56E010,Coors),c1(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),Coors);
 EntryPatch g0(0x5696C0,Ground),g1(bit_cast<uintptr_t>(&CWorld::FindGroundZFor3DCoord),Ground);
 EntryPatch b0(0x456450,Bubble),b1(bit_cast<uintptr_t>(&CPickups::TestForPickupsInBubble),Bubble);
 EntryPatch l0(0x56A490,Line),l1(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Line);
 EntryPatch s0(0x569E20,Sphere),s1(bit_cast<uintptr_t>(&CWorld::TestSphereAgainstWorld),Sphere);
 EntryPatch p0(0x4551C0,Find),p1(bit_cast<uintptr_t>(&CPickups::FindPickUpForThisObject),Find);
 EntryPatch i0(0x454D20,Invisible),i1(bit_cast<uintptr_t>(&CPickup::PickUpShouldBeInvisible),Invisible);
 EntryPatch w0(0x454AE0,Weapon),w1(bit_cast<uintptr_t>(&CPickups::WeaponForModel),Weapon);
 EntryPatch v0(0x7034F0,Vision),v1(bit_cast<uintptr_t>(&CPostEffects::IsVisionFXActive),Vision);
 EntryPatch q0(0x70CE30,Screen),q1(bit_cast<uintptr_t>(&CSprite::CalcScreenCoors),Screen);
 EntryPatch t0(0x455540,Label),t1(bit_cast<uintptr_t>(&CPickup::FindStringForTextIndex),Label),t2(0x6A0050,Text),t3(bit_cast<uintptr_t>(&CText::Get),Text);
 EntryPatch a0(0x6FC580,Corona),a1(bit_cast<uintptr_t>(static_cast<void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,eCoronaType,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool)>(&CCoronas::RegisterCorona)),Corona);
 for(int f=0;f<2;++f){std::string full=std::string(fns[f].cls)+"::"+fns[f].name;if(!only.empty()&&only!=full)continue;int failures=0,success=0,fallback=0,messages=0,coronas=0;for(int k=0;k<fns[f].cases;++k){json out[2];for(int side=0;side<2;++side){hooks[f]->State(side!=0);if(ReversingProbe::JumpsToDll(fns[f].address)!=(side!=0)){Write({{"event","error"},{"reason","hook switch failed"}});passed=false;break;}out[side]=Run(f,k,side!=0);}bool equal=out[0]==out[1];failures+=!equal;for(auto&r:out[0]["trace"]){if(r[0]=="line")++success;if(r[0]=="ground")++fallback;if(r[0]=="screen")++messages;if(r[0]=="corona")++coronas;}Write({{"event","comparison"},{"pair",pairs},{"function",full},{"case",k},{"equal",equal},{"accepted",equal},{"original",out[0]},{"candidate",out[1]}});++pairs;}bool coverage=f==0?success>0&&fallback>0:messages>0&&coronas>0;passed&=failures==0&&coverage;Write({{"event","function_summary"},{"function",full},{"cases",fns[f].cases},{"failures",failures},{"coverage",coverage},{"line",success},{"ground",fallback},{"screen",messages},{"corona",coronas}});}}
 for(auto&s:saved)memcpy(reinterpret_cast<void*>(s.at),s.bytes.data(),s.bytes.size());for(int i=0;i<2;++i){hooks[i]->State(enabled[i]);hooks[i]->LockState(locked[i]);}Write({{"event",passed?"complete":"error"},{"passed",passed},{"pairs",pairs},{"state_restored",true}});
}
}
