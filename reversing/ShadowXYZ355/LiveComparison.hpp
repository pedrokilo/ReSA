#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shadows.h"
#include "Collision/CollisionData.h"
#include "../ProbeRuntime.hpp"
namespace ShadowXYZ355Live {
inline bool done{};inline std::ofstream log;inline json trace;
alignas(16)inline std::array<uint8,sizeof(CEntity)> rawEntity{};
alignas(16)inline std::array<uint8,sizeof(CCollisionData)> rawData{};
alignas(16)inline std::array<uint8,sizeof(CRealTimeShadow)> rawShadow{};
inline CMatrixLink matrix{};inline CColModel model{};inline RwCamera camera{};inline RwFrame frame{};
inline std::array<CompressedVector,24> points{};inline std::array<CColTriangle,8> triangles{};
alignas(4)inline std::array<uint8,80> rawPlanes{};inline int scenario{},tested{},callbacks{};
inline uint32 Bits(float f){return std::bit_cast<uint32>(f);}
inline uint64 Hash(const void*p,size_t n){uint64 h=14695981039346656037ull;auto*b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline json Vec(const RwV3d&v){return json::array({Bits(v.x),Bits(v.y),Bits(v.z)});}
inline json Mat(const RwMatrix&m){return json::array({Vec(m.right),Vec(m.up),Vec(m.at),Vec(m.pos)});}
inline void __cdecl Planes(CColModel*){trace.push_back({"planes"});}
inline bool __cdecl Test(const CColSphere&s,const CompressedVector*,const CColTriangle&t,const CColTrianglePlane&p){trace.push_back({"test",tested,Hash(&s,16),int(s.m_Surface.m_nMaterial),s.m_Surface.m_nPiece,s.m_Surface.m_nLighting.value,t.vA,t.vB,t.vC,Hash(&p,sizeof(p))});return ((tested+++scenario)%3)!=0;}
inline CVector* __cdecl Triangle(CVector*n,CVector*v,_ProjectionParam*p){++callbacks;trace.push_back({"triangle",Hash(n,sizeof(*n)),Hash(v,sizeof(*v)*3),Vec(p->at),Mat(p->invMatrix),p->shadowValue,p->fade,Mat(p->entityMatrix)});return scenario%7==0?nullptr:v;}
__declspec(naked) inline void TriangleEntry(){__asm { push edi }__asm {push ebx}__asm {push eax}__asm {call Triangle}__asm {add esp,12}__asm {ret}}
inline void Write(const json&j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char*cls;const char*name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CShadows","CastShadowEntityXYZ",0x70A040,0,-1,4096}};
inline auto*FindHook(){ReversibleHooks::ReversibleHook::Base*h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&c){if(c.Name()=="CShadows")for(auto&x:c.Items())if(x->Name()=="CastShadowEntityXYZ")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"shadow-xyz355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"shadow-xyz355-runtime.jsonl");ReversingProbe::InstallFailureCapture("shadow-xyz355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
auto*savedModel=CModelInfo::ms_modelInfoPtrs[100];alignas(16)std::array<uint8,0x40> modelInfo{};*reinterpret_cast<CColModel**>(modelInfo.data()+0x14)=&model;CModelInfo::ms_modelInfoPtrs[100]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());int pairs=0,failures=0,positive=0,inactive=0,early=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;EntryPatch p(0x418580,Planes),pd(bit_cast<uintptr_t>(static_cast<void(__cdecl*)(CColModel*)>(&CCollision::CalculateTrianglePlanes)),Planes),t(0x4165B0,Test),td(bit_cast<uintptr_t>(&CCollision::TestSphereTriangle),Test),c(0x709CF0,TriangleEntry),cd(bit_cast<uintptr_t>(&ShadowRenderTriangleCB),Triangle);
auto*h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<4096;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();tested=callbacks=0;memset(rawEntity.data(),0xCD,rawEntity.size());memset(rawData.data(),0,rawData.size());memset(rawShadow.data(),0xCD,rawShadow.size());memset(rawPlanes.data(),0,rawPlanes.size());auto*entity=reinterpret_cast<CEntity*>(rawEntity.data());entity->m_nModelIndex=100;entity->m_matrix=&matrix;matrix.GetRight()=CVector{1,0,0};matrix.GetForward()=CVector{0,1,0};matrix.GetUp()=CVector{0,0,1};matrix.GetPosition()=CVector{float(i%5),float(i%7),float(i%3)};
auto*shadow=reinterpret_cast<CRealTimeShadow*>(rawShadow.data());shadow->m_camera.m_pRwCamera=&camera;shadow->m_bBlurred=(i&1)!=0;shadow->m_blurCamera.m_pRwCamera=nullptr;shadow->m_baseSphere.m_vecCenter=CVector{float(i%17)*.25f,float(i%13)*.5f,float(i%11)};camera.object.object.parent=&frame;camera.viewWindow={float(i%9+1)*.125f,float(i%7+1)*.25f};RwMatrixSetIdentity(&frame.modelling);frame.modelling.pos={float(i%3),float(i%5),float(i%7)};
auto*data=reinterpret_cast<CCollisionData*>(rawData.data());data->m_nNumTriangles=uint16(i%9);data->m_pVertices=points.data();data->m_pTriangles=triangles.data();data->m_pTrianglePlanes=reinterpret_cast<CColTrianglePlane*>(rawPlanes.data());model.m_pColData=i%31?data:nullptr;
for(int k=0;k<8;++k){points[k*3]=CompressedVector{CVector{float(k),0,float(i%7)}};points[k*3+1]=CompressedVector{CVector{0,float(k),1}};points[k*3+2]=CompressedVector{CVector{1,2,float(k)}};triangles[k]=CColTriangle(uint16(k*3),uint16(k*3+1),uint16(k*3+2),static_cast<eSurfaceType>(0),tColLighting{uint8(i+k)});auto*normal=reinterpret_cast<int16*>(rawPlanes.data()+k*10);normal[0]=int16(i*3);normal[1]=int16(k*117);normal[2]=int16(i%13==0?0:4096);}
CVector pos{1,2,3};CPolyBunch*poly=nullptr;h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x70A040)==candidate;reinterpret_cast<void(__cdecl*)(CEntity*,CVector*,float,float,float,float,int16,uint8,uint8,uint8,float,float,CPolyBunch**,CRealTimeShadow*)>(0x70A040)(entity,&pos,float(i%7-3)*.125f,float(i%5-2)*.25f,.5f,.75f,int16(i*17),uint8(i),uint8(i*3),uint8(i*7),4.5f,1.0f,&poly,i%19?shadow:nullptr);
return json{{"trace",trace},{"callbacks",callbacks},{"tested",tested},{"entity",Hash(rawEntity.data(),rawEntity.size())},{"shadow",Hash(rawShadow.data(),rawShadow.size())},{"poly",uintptr_t(poly)},{"pos",Hash(&pos,sizeof(pos))},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=a["callbacks"].get<int>()>0;inactive+=a["tested"].get<int>()==0;early+=i%7==0&&a["callbacks"].get<int>()==1;Write({{"event","comparison"},{"function","CShadows::CastShadowEntityXYZ"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
model.m_pColData=nullptr;CModelInfo::ms_modelInfoPtrs[100]=savedModel;pass&=positive>0&&inactive>0&&early>0;Write({{"event","function_summary"},{"function","CShadows::CastShadowEntityXYZ"},{"cases",pairs},{"failures",failures},{"positive",positive},{"inactive",inactive},{"early",early}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
