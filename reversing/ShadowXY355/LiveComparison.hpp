#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shadows.h"
#include "Collision/CollisionData.h"
#include "Collision/ColHelpers.h"
#include "../ProbeRuntime.hpp"
namespace ShadowXY355Live {
inline bool done{};inline std::ofstream log;inline json trace;
alignas(16) inline std::array<uint8,sizeof(CEntity)> rawEntity{};
alignas(16) inline std::array<uint8,sizeof(CCollisionData)> rawData{};
inline CMatrixLink matrix{};inline CColModel model{};
inline std::array<CompressedVector,24> points{};struct TriStorage{std::array<ColHelpers::TFaceGroup,2> groups;uint32 count;std::array<CColTriangle,8> triangles;};inline TriStorage triangleStorage{};inline auto& triangles=triangleStorage.triangles;
alignas(4) inline std::array<uint8,80> rawPlanes{};
inline std::array<CPolyBunch,8> bunches{};
inline std::array<RwIm3DVertex,7> outVertices{};inline std::array<RwImVertexIndex,15> outIndices{};
inline int scenario{},stored{},vertexCount{},indexCount{};
inline uint64 Hash(const void*p,size_t n){uint64 h=14695981039346656037ull;auto*b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline uint32 Rnd(uint32 i,uint32 s){uint32 x=i*2654435761u+s*40503u+0x9E3779B9u;x^=x<<13;x^=x>>17;x^=x<<5;x*=2246822519u;x^=x>>15;return x;}
inline float Rf(uint32 i,uint32 s,float lo,float hi){return lo+(hi-lo)*(float(Rnd(i,s)>>8)*(1.0f/16777216.0f));}
inline float Special(uint32 i,uint32 s,float normal){switch(Rnd(i,s+7777u)%24u){case 0:return std::bit_cast<float>(0x7FC00123u);case 1:return std::bit_cast<float>(0x7F800000u);case 2:return -0.0f;case 3:return 1.0e-40f;case 4:return 1.0e30f;case 5:return -1.0e30f;default:return normal;}}
inline void StressXY(int i,CVector& pos,float& ax,float& ay,float& bx,float& by){const uint32 n=uint32(i);
matrix.GetRight()=CVector{Rf(n,1,.3f,1.f),Rf(n,2,-.7f,.7f),0};matrix.GetForward()=CVector{Rf(n,3,-.7f,.7f),Rf(n,4,.3f,1.f),0};matrix.GetUp()=CVector{0,0,1};
pos=CVector{matrix.GetPosition().x+Rf(n,5,-1.5f,1.5f),matrix.GetPosition().y+Rf(n,6,-1.5f,1.5f),matrix.GetPosition().z+Rf(n,7,0.f,1.5f)};
ax=Rf(n,8,-1.2f,1.2f);ay=Rf(n,9,-1.2f,1.2f);bx=Rf(n,10,-1.2f,1.2f);by=Rf(n,11,-1.2f,1.2f);if(Rnd(n,12)%40==0)ax=std::bit_cast<float>(0x7FC00123u);}
inline void __cdecl Planes(CColModel*){trace.push_back({"planes"});}
inline void __cdecl Start(int ni,int nv,RwImVertexIndex*&indices,RwIm3DVertex*&vertices){trace.push_back({"start",ni,nv});++stored;indexCount=ni;vertexCount=nv;indices=outIndices.data();vertices=outVertices.data();}
inline void __cdecl Stop(){trace.push_back({"stop",Hash(outVertices.data(),sizeof(outVertices)),Hash(outIndices.data(),sizeof(outIndices))});}
inline void __cdecl Colour(eShadowType type,uint8 lighting,uint8 r,uint8 g,uint8 b,uint8&ro,uint8&go,uint8&bo){trace.push_back({"colour",int(type),lighting,r,g,b});ro=r^lighting;go=g+lighting;bo=b-lighting;}
inline void Write(const json&j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char*cls;const char*name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CShadows","CastShadowEntityXY",0x7086B0,0,-1,6144}};
inline auto*FindHook(){ReversibleHooks::ReversibleHook::Base*h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto&c){if(c.Name()=="CShadows")for(auto&x:c.Items())if(x->Name()=="CastShadowEntityXY")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"shadow-xy355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"shadow-xy355-runtime.jsonl");ReversingProbe::InstallFailureCapture("shadow-xy355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
std::array<uint8,0x218> savedScratch{};memcpy(savedScratch.data(),reinterpret_cast<void*>(0xC4B6B8),savedScratch.size());auto savedVertices=TempBufferVertices;std::array<RxVertexIndex,TOTAL_TEMP_BUFFER_INDICES> savedIndices{};memcpy(savedIndices.data(),aTempBufferIndices,sizeof(aTempBufferIndices));auto savedVtx=uiTempBufferVerticesStored;auto savedIdx=uiTempBufferIndicesStored;auto savedNeededIdx=*reinterpret_cast<int32*>(0xC40410);auto savedNeededVtx=*reinterpret_cast<int32*>(0xC40414);auto*savedEmpty=CShadows::pEmptyBunchList;auto*savedModel=CModelInfo::ms_modelInfoPtrs[100];alignas(16)std::array<uint8,0x40> modelInfo{};*reinterpret_cast<CColModel**>(modelInfo.data()+0x14)=&model;CModelInfo::ms_modelInfoPtrs[100]=reinterpret_cast<CBaseModelInfo*>(modelInfo.data());
int pairs=0,failures=0,positive=0,inactive=0,polys=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;EntryPatch p(0x418580,Planes),pd(bit_cast<uintptr_t>(static_cast<void(__cdecl*)(CColModel*)>(&CCollision::CalculateTrianglePlanes)),Planes),c(0x707850,Colour),cd(bit_cast<uintptr_t>(&CShadows::AffectColourWithLighting),Colour);
auto*h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<6144;++i){auto run=[&](bool candidate){scenario=i;trace=json::array();stored=vertexCount=indexCount=0;uiTempBufferVerticesStored=uiTempBufferIndicesStored=0;*reinterpret_cast<int32*>(0xC40410)=*reinterpret_cast<int32*>(0xC40414)=0;memset(&TempBufferVertices,0xCD,sizeof(TempBufferVertices));memset(aTempBufferIndices,0xCD,sizeof(aTempBufferIndices));memset(outVertices.data(),0xCD,sizeof(outVertices));memset(outIndices.data(),0xCD,sizeof(outIndices));memset(reinterpret_cast<void*>(0xC4B6B8),0xCD,0x218);memset(rawEntity.data(),0xCD,rawEntity.size());memset(rawData.data(),0,rawData.size());memset(rawPlanes.data(),0,rawPlanes.size());memset(bunches.data(),0xCD,sizeof(bunches));
auto*entity=reinterpret_cast<CEntity*>(rawEntity.data());entity->m_nModelIndex=100;entity->m_matrix=&matrix;matrix.GetRight()=CVector{1,0,0};matrix.GetForward()=CVector{0,1,0};matrix.GetUp()=CVector{0,0,1};matrix.GetPosition()=CVector{float(i%5),float(i%7),float(i%3)};
auto*data=reinterpret_cast<CCollisionData*>(rawData.data());data->m_nNumTriangles=uint16(i%9);data->m_pVertices=points.data();data->m_pTriangles=triangles.data();data->m_pTrianglePlanes=reinterpret_cast<CColTrianglePlane*>(rawPlanes.data());model.m_pColData=i%31?data:nullptr;data->bHasFaceGroups=(i&2)!=0;triangleStorage.count=2;for(int group=0;group<2;++group){auto&g=triangleStorage.groups[group];g.bb.m_vecMin=CVector{(i%23==0)?100.0f:-100.0f,-100,-100};g.bb.m_vecMax=CVector{101,100,100};g.first=uint16(group*4);g.last=uint16(std::min<int>(int(data->m_nNumTriangles)-1,group*4+3));if(int(data->m_nNumTriangles)<=group*4){g.first=1;g.last=0;}}
for(int k=0;k<8;++k){const float span=float((i+k)%7+1)*.5f;points[k*3]=CompressedVector{CVector{-span,-span,float(k)*.125f}};points[k*3+1]=CompressedVector{CVector{-span,span,float(k)*.125f}};points[k*3+2]=CompressedVector{CVector{span,-span,float(k)*.125f}};triangles[k]=CColTriangle(uint16(k*3),uint16(k*3+1),uint16(k*3+2),static_cast<eSurfaceType>(0),tColLighting{uint8(i+k)});constexpr int16 normalZ[]{0,409,410,4096,-4096,2000,8192};auto*normal=reinterpret_cast<int16*>(rawPlanes.data()+k*10);normal[0]=int16((i%9-4)*127);normal[1]=int16((k%7-3)*91);normal[2]=normalZ[(i+k)%7];bunches[k].m_pNext=k<7?&bunches[k+1]:nullptr;}
CShadows::pEmptyBunchList=i%17?bunches.data():nullptr;CPolyBunch*list=nullptr;uint8 lighting=0xAB;CVector pos{matrix.GetPosition().x+float(i%11-5)*.25f,matrix.GetPosition().y+float((i/11)%11-5)*.25f,matrix.GetPosition().z+float(i%7)*.25f};
float ax=.5f,ay=0,bx=0,by=.75f;if(i>=4096)StressXY(i,pos,ax,ay,bx,by);
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x7086B0)==candidate;reinterpret_cast<void(__cdecl*)(CEntity*,float,float,float,float,CVector*,float,float,float,float,int16,uint8,uint8,uint8,float,float,CPolyBunch**,uint8*,int32)>(0x7086B0)(entity,-100,-100,100,100,&pos,ax,ay,bx,by,int16(i*17),uint8(i),uint8(i*3),uint8(i*7),float(i%9)*.25f,float(i%5)*.5f,i&1?&list:nullptr,&lighting,i%4);
return json{{"trace",trace},{"scratch",Hash(reinterpret_cast<void*>(0xC4B6B8),0x218)},{"bunches",Hash(bunches.data(),sizeof(bunches))},{"head",uintptr_t(list)},{"empty",uintptr_t(CShadows::pEmptyBunchList)},{"entity",Hash(rawEntity.data(),rawEntity.size())},{"lighting",lighting},{"stored",uiTempBufferVerticesStored},{"indices_count",uiTempBufferIndicesStored},{"vertices",Hash(&TempBufferVertices,sizeof(TempBufferVertices))},{"indices",Hash(aTempBufferIndices,sizeof(aTempBufferIndices))},{"needed_indices",*reinterpret_cast<int32*>(0xC40410)},{"needed_vertices",*reinterpret_cast<int32*>(0xC40414)},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=a["stored"].get<int>()>0;polys+=a["head"].get<uintptr_t>()!=0;inactive+=a["stored"].get<int>()==0&&a["head"].get<uintptr_t>()==0;Write({{"event","comparison"},{"function","CShadows::CastShadowEntityXY"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
TempBufferVertices=savedVertices;memcpy(aTempBufferIndices,savedIndices.data(),sizeof(aTempBufferIndices));uiTempBufferVerticesStored=savedVtx;uiTempBufferIndicesStored=savedIdx;*reinterpret_cast<int32*>(0xC40410)=savedNeededIdx;*reinterpret_cast<int32*>(0xC40414)=savedNeededVtx;model.m_pColData=nullptr;CModelInfo::ms_modelInfoPtrs[100]=savedModel;CShadows::pEmptyBunchList=savedEmpty;memcpy(reinterpret_cast<void*>(0xC4B6B8),savedScratch.data(),savedScratch.size());pass&=positive>0&&polys>0&&inactive>0;Write({{"event","function_summary"},{"function","CShadows::CastShadowEntityXY"},{"cases",pairs},{"failures",failures},{"positive",positive},{"polys",polys},{"inactive",inactive}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
