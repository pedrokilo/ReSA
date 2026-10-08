#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ClothesBuilder.h"
#include "../ProbeRuntime.hpp"
namespace ClothesSkin355Live {
struct Mesh {RpGeometry geometry{};RpMorphTarget morph{};std::array<RwV3d,128> vertices{},normals{};std::array<RwTexCoords,128> uv{};std::array<RpTriangle,128> triangles{};std::array<uint32,128> bones{};std::array<RwMatrixWeights,128> weights{};};
inline std::array<Mesh,11> meshes{};inline std::array<RwMatrixWeights,128> allocatedWeights{};inline std::array<uint32,128> allocatedBones{};
struct Record {uint32 tag,a,b,c,d;};inline std::array<Record,1024> records{};inline int recordCount{},allocations{},scenario{};inline bool overflow{};inline std::array<size_t,2> sizes{};
inline void RecordCall(uint32 tag,uint32 a=0,uint32 b=0,uint32 c=0,uint32 d=0){if(recordCount<int(records.size()))records[recordCount++]={tag,a,b,c,d};else overflow=true;}
inline int Index(const RpGeometry* g){for(int k=0;k<11;++k)if(g==&meshes[k].geometry)return k;return -1;}
inline RpGeometry* __cdecl Create(int vertices,int triangles,uint32 flags){RecordCall(1,uint32(vertices),uint32(triangles),flags);auto& m=meshes[10];m.geometry.numVertices=vertices;m.geometry.numTriangles=triangles;m.geometry.flags=flags;return &m.geometry;}
inline RpGeometry* __cdecl SetIndices(RpGeometry* g,RpTriangle* triangle,uint16 a,uint16 b,uint16 c){RecordCall(2,uint32(triangle-g->triangles),a,b,c);triangle->vertIndex[0]=a;triangle->vertIndex[1]=b;triangle->vertIndex[2]=c;return g;}
inline RpGeometry* __cdecl SetMaterial(RpGeometry* g,RpTriangle* triangle,RpMaterial* material){RecordCall(3,uint32(triangle-g->triangles),uint32(uintptr_t(material)));triangle->matIndex=uint16(uintptr_t(material)&15);return g;}
inline const RpMorphTarget* __cdecl Sphere(const RpMorphTarget* morph,RwSphere* out){RecordCall(4);out->center={.25f,.5f,.75f};out->radius=1.5f;return morph;}
inline RpGeometry* __cdecl Unlock(RpGeometry* g){RecordCall(5,uint32(g->numVertices));return g;}
inline RpSkin* __cdecl Skin(RpGeometry* g){RecordCall(6,uint32(Index(g)));return reinterpret_cast<RpSkin*>(g);}
inline const uint32* __cdecl Bones(RpSkin* s){int k=Index(reinterpret_cast<RpGeometry*>(s));RecordCall(7,uint32(k));return meshes[k].bones.data();}
inline RwMatrixWeights* __cdecl Weights(RpSkin* s){int k=Index(reinterpret_cast<RpGeometry*>(s));RecordCall(8,uint32(k));return meshes[k].weights.data();}
inline void __cdecl Conversion(uint8* out,RpHAnimHierarchy*,int model){RecordCall(9,uint32(model));for(int k=0;k<64;++k)out[k]=uint8((k+model*7+scenario)%32);}
inline void* __cdecl New(size_t n){int index=allocations++;RecordCall(10,uint32(n));if(index<2)sizes[index]=n;else overflow=true;return index==0?static_cast<void*>(allocatedWeights.data()):static_cast<void*>(allocatedBones.data());}
inline json OutputIdentity355(const void* value){if(!value)return nullptr;const auto pointer=uintptr_t(value);for(size_t i=0;i<meshes.size();++i)if(value==&meshes[i].geometry)return json::array({"mesh",i});
    if(pointer>=uintptr_t(allocatedWeights.data())&&pointer<uintptr_t(allocatedWeights.data())+sizeof(allocatedWeights))return json::array({"weights",pointer-uintptr_t(allocatedWeights.data())});
    if(pointer>=uintptr_t(allocatedBones.data())&&pointer<uintptr_t(allocatedBones.data())+sizeof(allocatedBones))return json::array({"bones",pointer-uintptr_t(allocatedBones.data())});return pointer;}
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline bool done{};inline std::ofstream log;inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CClothesBuilder","ConstructGeometryAndSkinArrays",0x5A6530,0,-1,2048}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CClothesBuilder")for(auto& x:c.Items())if(x->Name()=="ConstructGeometryAndSkinArrays")h=x.get();});return h;}
inline void Setup(int i){scenario=i;recordCount=allocations=0;overflow=false;sizes.fill(0);records.fill({});memset(meshes.data(),0xCD,sizeof(meshes));memset(allocatedWeights.data(),0xCD,sizeof(allocatedWeights));memset(allocatedBones.data(),0xCD,sizeof(allocatedBones));for(int k=0;k<11;++k){auto& m=meshes[k];m.geometry.numVertices=(i+k)%9;m.geometry.numTriangles=(i/9+k)%4;m.geometry.morphTarget=&m.morph;m.geometry.texCoords[0]=m.uv.data();m.geometry.triangles=m.triangles.data();m.morph.parentGeom=&m.geometry;m.morph.verts=m.vertices.data();m.morph.normals=m.normals.data();for(int v=0;v<128;++v){m.vertices[v]={float(v+k),float(k),float(i%7)};m.normals[v]={1,0,0};m.uv[v]={float(v)*.125f,float(k)*.25f};m.triangles[v]={{uint16(v%8),uint16((v+1)%8),uint16((v+2)%8)},uint16(k)};m.bones[v]=0x03020100u+uint32(k)*0x01010101u;m.weights[v]={.4f,.3f,.2f,.1f};}}}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"clothes-skin355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"clothes-skin355-runtime.jsonl");ReversingProbe::InstallFailureCapture("clothes-skin355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;int pairs=0,failures=0,positive=0,empty=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch cr(0x74CA90,Create),si(0x74C690,SetIndices),sm(0x74C6C0,SetMaterial),sp(0x74C200,Sphere),un(0x74C800,Unlock),sk(0x7C7550,Skin),bo(0x7C7800,Bones),we(0x7C77F0,Weights),co(0x5A56E0,Conversion),cod(bit_cast<uintptr_t>(&CClothesBuilder::BuildBoneIndexConversionTable),Conversion);
auto* h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){Setup(i);std::array<RpGeometry*,10> input{};std::array<RpMaterial*,10> materials{};const uint32 count=uint32(i%11);for(int k=0;k<10;++k){input[k]=(i&(1<<k))?&meshes[k].geometry:nullptr;materials[k]=reinterpret_cast<RpMaterial*>(uintptr_t(0x123400+k));}RpGeometry* result=nullptr;RwMatrixWeights* weights=nullptr;uint32* bones=nullptr;h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x5A6530)==candidate;
{
EntryPatch n(0x82119A,New),nd(bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&::operator new)),New);
reinterpret_cast<void(__cdecl*)(RpHAnimHierarchy*,RpGeometry**,RwMatrixWeights**,uint32**,uint32,RpGeometry**,RpMaterial**)>(0x5A6530)(reinterpret_cast<RpHAnimHierarchy*>(uintptr_t(0x234500)),&result,&weights,&bones,count,input.data(),materials.data());
}
json events=json::array();for(int k=0;k<recordCount;++k){const auto& r=records[k];events.push_back({r.tag,r.a,r.b,r.c,r.d});}return json{{"trace",events},{"meshes",Hash(meshes.data(),sizeof(meshes))},{"weights",Hash(allocatedWeights.data(),sizeof(allocatedWeights))},{"bones",Hash(allocatedBones.data(),sizeof(allocatedBones))},{"sizes",sizes},{"allocations",allocations},{"outputs",json::array({result==&meshes[10].geometry,weights==allocatedWeights.data(),bones==allocatedBones.data()})},{"output_identities",json::array({OutputIdentity355(result),OutputIdentity355(weights),OutputIdentity355(bones)})},{"overflow",overflow},{"vertices",meshes[10].geometry.numVertices},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true&&!a["overflow"].get<bool>();pass&=eq;failures+=!eq;positive+=a["vertices"].get<int>()>0;empty+=a["vertices"].get<int>()==0;Write({{"event","comparison"},{"function","CClothesBuilder::ConstructGeometryAndSkinArrays"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
pass&=positive>0&&empty>0;Write({{"event","function_summary"},{"function","CClothesBuilder::ConstructGeometryAndSkinArrays"},{"cases",pairs},{"failures",failures},{"positive",positive},{"empty",empty}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
