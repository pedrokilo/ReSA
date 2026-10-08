#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ClothesBuilder.h"
#include "../ProbeRuntime.hpp"
namespace ClothesGeometry355Live {
inline bool done{}; inline std::ofstream log; inline json trace;
struct Mesh { RpGeometry geometry{}; RpAtomic atomic{}; RpMorphTarget morph{}; std::array<RwV3d,16> vertices{},normals{}; std::array<RwTexCoords,16> uv{}; std::array<uint32,16> bones{}; std::array<RwMatrixWeights,16> weights{}; };
inline Mesh meshes[3];
inline int Index(const char* name){return !strcmp(name,"normal")?0:!strcmp(name,"fat")?1:2;}
inline RpAtomic* __cdecl Find(RpClump*,const char* name){const int k=Index(name);trace.push_back({"find",k});return &meshes[k].atomic;}
inline RpClump* __cdecl Enumerate(RpClump* c,RpAtomicCallBack,void* context){auto** p=static_cast<void**>(context);p[1]=Find(c,static_cast<const char*>(p[0]));return c;}
inline RpSkin* __cdecl Skin(RpGeometry* g){return reinterpret_cast<RpSkin*>(g);}
inline const RwUInt32* __cdecl Bones(RpSkin* skin){for(auto& m:meshes)if(&m.geometry==reinterpret_cast<RpGeometry*>(skin))return m.bones.data();return nullptr;}
inline RwMatrixWeights* __cdecl Weights(RpSkin* skin){for(auto& m:meshes)if(&m.geometry==reinterpret_cast<RpGeometry*>(skin))return m.weights.data();return nullptr;}
inline RpGeometry* __cdecl Lock(RpGeometry* g,int flags){trace.push_back({"lock",flags});g->lockedSinceLastInst|=uint16(flags);return g;}
inline RpGeometry* __cdecl Unlock(RpGeometry* g){trace.push_back({"unlock"});return g;}
inline RpGeometry* __cdecl AddRef(RpGeometry* g){++g->refCount;return g;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
inline json GeometryIdentity355(const RpGeometry* value){if(!value)return nullptr;for(size_t i=0;i<std::size(meshes);++i)if(value==&meshes[i].geometry)return json::array({"mesh",i});return uintptr_t(value);}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{
{"CClothesBuilder","CopyGeometry",0x5A5340,0,-1,1024},
{"CClothesBuilder","BlendGeometry-2",0x5A4F10,0,-1,2048},
{"CClothesBuilder","BlendGeometry-3",0x5A4940,0,-1,2048}};
inline auto* FindHook(const char* name){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CClothesBuilder")for(auto& x:c.Items())if(x->Name()==name)h=x.get();});return h;}
inline void Setup(int scenario){
for(int k=0;k<3;++k){auto& m=meshes[k];memset(&m,0xCD,sizeof(m));m.geometry.numVertices=scenario%17;m.geometry.morphTarget=&m.morph;m.geometry.texCoords[0]=m.uv.data();m.geometry.refCount=7;m.geometry.lockedSinceLastInst=0;m.atomic.geometry=&m.geometry;m.morph.parentGeom=&m.geometry;m.morph.verts=m.vertices.data();m.morph.normals=m.normals.data();
for(int i=0;i<16;++i){m.vertices[i]={float((scenario+i*3+k*7)%59-29)*.125f,float(i-k)*.3f,float(scenario%7-k)*.2f};m.normals[i]={float(i%3+1),float(k+1),float(i%5-2)};m.uv[i]={float(i)*.0625f,float(k)*.2f};const uint8 base=uint8((scenario/17+k)%6);m.bones[i]=uint32(base)|(uint32((base+1)%6)<<8)|(uint32((base+2)%6)<<16)|(uint32((base+3)%6)<<24);m.weights[i]={.4f,.3f,.2f,.1f};if(scenario%11==0)m.weights[i]={0,0,0,0};}}
}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"clothes-geometry355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"clothes-geometry355-runtime.jsonl");ReversingProbe::InstallFailureCapture("clothes-geometry355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;int pairs=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch e(0x749B70,Enumerate),d(bit_cast<uintptr_t>(&GetAtomicWithName),Find),s(0x7C7550,Skin),b(0x7C7800,Bones),w(0x7C77F0,Weights),l(0x74C7D0,Lock),u(0x74C800,Unlock),r(0x74CCB0,AddRef);
for(const auto& fn:fns){auto* h=FindHook(fn.name);if(!h){Write({{"event","error"},{"reason","missing hook"},{"function",fn.name}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);int failures=0,positive=0,normalised=0;
for(int i=0;i<fn.cases;++i){auto run=[&](bool candidate){Setup(i);trace=json::array();h->State(candidate);const bool switched=ReversingProbe::JumpsToDll(fn.address)==candidate;RpGeometry* result{};auto* clump=reinterpret_cast<RpClump*>(uintptr_t(0x12340000));const float ratio0=float(i%9)*.125f,ratio1=float((i/9)%9)*.125f,ratio2=float((i/81)%9)*.125f;
if(fn.address==0x5A5340)result=reinterpret_cast<RpGeometry*(__cdecl*)(RpClump*,const char*,const char*)>(fn.address)(clump,"normal","fat");else if(fn.address==0x5A4F10)result=reinterpret_cast<RpGeometry*(__cdecl*)(RpClump*,const char*,const char*,float,float)>(fn.address)(clump,"normal","fat",ratio0,ratio1);else result=reinterpret_cast<RpGeometry*(__cdecl*)(RpClump*,const char*,const char*,const char*,float,float,float)>(fn.address)(clump,"normal","fat","ripped",ratio0,ratio1,ratio2);
return json{{"state",Hash(meshes,sizeof(meshes))},{"returned_destination",result==&meshes[0].geometry},{"returned_identity",GeometryIdentity355(result)},{"trace",trace},{"vertices",meshes[0].geometry.numVertices},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=a["vertices"].get<int>()>0;normalised+=i%17>0&&i%11!=0;Write({{"event","comparison"},{"function",std::string(fn.cls)+"::"+fn.name},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);pass&=positive>0&&normalised>0;Write({{"event","function_summary"},{"function",std::string(fn.cls)+"::"+fn.name},{"cases",fn.cases},{"failures",failures},{"positive",positive},{"weighted",normalised}});}}
Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
