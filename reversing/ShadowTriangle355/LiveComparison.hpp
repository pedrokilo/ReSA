#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Shadows.h"
#include "../ProbeRuntime.hpp"
namespace ShadowTriangle355Live {
inline bool done{};inline std::ofstream log;inline json trace;inline int scenario{},transforms{},stores{};
inline std::array<RwIm3DVertex,3> vertices{};inline std::array<RwImVertexIndex,3> indices{};
inline std::array<RwV3d,3> world{},projection{};
inline uint64 Hash(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline RwV3d* __cdecl Transform(RwV3d* out,const RwV3d* input,int count,const RwMatrix* matrix){trace.push_back({"transform",count,Hash(input,size_t(count)*sizeof(RwV3d)),Hash(matrix,sizeof(*matrix))});const auto& source=transforms++?projection:world;memcpy(out,source.data(),size_t(count)*sizeof(RwV3d));return out;}
inline void __cdecl Start(int ni,int nv,RwImVertexIndex*& index,RwIm3DVertex*& vertex){trace.push_back({"start",ni,nv});++stores;index=indices.data();vertex=vertices.data();}
inline void __cdecl Stop(){trace.push_back({"stop"});}
inline CVector* Call(CVector* normal,CVector* triangle,_ProjectionParam* param){CVector* result;__asm {mov eax,normal}__asm {mov ebx,triangle}__asm {mov edi,param}__asm {mov ecx,0709CF0h}__asm {call ecx}__asm {mov result,eax}return result;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CShadows","ShadowRenderTriangleCB",0x709CF0,0,-1,2048}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CShadows")for(auto& x:c.Items())if(x->Name()=="ShadowRenderTriangleCB")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"shadow-triangle355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"shadow-triangle355-runtime.jsonl");ReversingProbe::InstallFailureCapture("shadow-triangle355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;int pairs=0,failures=0,positive=0,inactive=0,faded=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch t(0x7EDD90,Transform),s(0x708290,Start),sd(bit_cast<uintptr_t>(&RenderBuffer::StartStoring),Start),e(0x7077A0,Stop),ed(bit_cast<uintptr_t>(&RenderBuffer::StopStoring),Stop);
auto* h=FindHook();if(!h){Write({{"event","error"},{"reason","missing hook"}});return;}bool old=h->Hooked(),locked=h->Locked();h->LockState(false);
for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){scenario=i;transforms=stores=0;trace=json::array();memset(vertices.data(),0xCD,sizeof(vertices));memset(indices.data(),0xCD,sizeof(indices));_ProjectionParam param{};memset(&param,0xCD,sizeof(param));param.at={0,0,1};param.shadowValue=uint8(i);param.fade=(i&8)?-1:0;std::array<CVector,3> triangle{CVector{1,2,3},CVector{4,5,6},CVector{7,8,9}};CVector normal{float(i%3-1),float(i%5-2),float(i%7-3)};
for(int k=0;k<3;++k){world[k]={float((i+k)%13-6)*.125f,float(i%7),float(i%19-9)*.125f};projection[k]={float((i/7+k)%9-4)*.25f,float((i/63+k)%9-4)*.25f,float((i/567+k)%9-4)*.5f};}if(i%37==0)projection[0].x=std::bit_cast<float>(0x7FC00123u);if(i%41==0)projection[1].z=std::bit_cast<float>(0x7FC00123u);if(i%43==0)normal.z=std::bit_cast<float>(0x7FC00123u);
h->State(candidate);bool switched=ReversingProbe::JumpsToDll(0x709CF0)==candidate;auto* result=Call(&normal,triangle.data(),&param);return json{{"trace",trace},{"vertices",Hash(vertices.data(),sizeof(vertices))},{"indices",indices},{"returned_triangle",result==triangle.data()},{"param",Hash(&param,sizeof(param))},{"triangle",Hash(triangle.data(),sizeof(triangle))},{"stores",stores},{"fade",param.fade},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=a["stores"].get<int>()>0;inactive+=a["stores"].get<int>()==0;faded+=a["stores"].get<int>()>0&&a["fade"].get<int>()!=0;Write({{"event","comparison"},{"function","CShadows::ShadowRenderTriangleCB"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
h->State(old);h->LockState(locked);}
pass&=positive>0&&inactive>0&&faded>0;Write({{"event","function_summary"},{"function","CShadows::ShadowRenderTriangleCB"},{"cases",pairs},{"failures",failures},{"positive",positive},{"inactive",inactive},{"faded",faded}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
