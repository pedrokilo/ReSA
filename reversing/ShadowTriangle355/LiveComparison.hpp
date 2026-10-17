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
inline uint32 Rnd(uint32 i,uint32 s){uint32 x=i*2654435761u+s*40503u+0x9E3779B9u;x^=x<<13;x^=x>>17;x^=x<<5;x*=2246822519u;x^=x>>15;return x;}
inline float Rf(uint32 i,uint32 s,float lo,float hi){return lo+(hi-lo)*(float(Rnd(i,s)>>8)*(1.0f/16777216.0f));}
inline float Special(uint32 i,uint32 s,float normal){switch(Rnd(i,s+7777u)%24u){case 0:return std::bit_cast<float>(0x7FC00123u);case 1:return std::bit_cast<float>(0x7F800000u);case 2:return -0.0f;case 3:return 1.0e-40f;case 4:return 1.0e30f;case 5:return -1.0e30f;default:return normal;}}
inline void Stress(int i,CVector& normal,_ProjectionParam& param){const uint32 n=uint32(i);
for(int k=0;k<3;++k){projection[k]={Rf(n,k*8+1,-.5f,1.5f),Rf(n,k*8+2,-.5f,1.5f),Rf(n,k*8+3,-1.6f,1.6f)};world[k]={Rf(n,k*8+4,-4.f,4.f),Rf(n,k*8+5,-4.f,4.f),Rf(n,k*8+6,-4.f,4.f)};}
projection[Rnd(n,50)%3].z=Special(n,51,projection[0].z);constexpr float edge[]{1.0f,-1.0f,1.0000001f,0.99999994f,0.0f};if(Rnd(n,52)%6==0)projection[Rnd(n,53)%3].z=edge[Rnd(n,54)%5];
normal=CVector{Special(n,60,Rf(n,61,-1.f,1.f)),Rf(n,62,-1.f,1.f),Rf(n,63,-1.f,1.f)};param.at={Rf(n,64,-1.f,1.f),Rf(n,65,-1.f,1.f),Rf(n,66,-1.f,1.f)};param.shadowValue=uint8(Rnd(n,67));param.fade=(Rnd(n,68)&1)?-1:0;}
inline RwV3d* __cdecl Transform(RwV3d* out,const RwV3d* input,int count,const RwMatrix* matrix){trace.push_back({"transform",count,Hash(input,size_t(count)*sizeof(RwV3d)),Hash(matrix,sizeof(*matrix))});const auto& source=transforms++?projection:world;memcpy(out,source.data(),size_t(count)*sizeof(RwV3d));return out;}
inline void __cdecl Start(int ni,int nv,RwImVertexIndex*& index,RwIm3DVertex*& vertex){trace.push_back({"start",ni,nv});++stores;index=indices.data();vertex=vertices.data();}
inline void __cdecl Stop(){trace.push_back({"stop"});}
inline CVector* Call(CVector* normal,CVector* triangle,_ProjectionParam* param){CVector* result;__asm {mov eax,normal}__asm {mov ebx,triangle}__asm {mov edi,param}__asm {mov ecx,0709CF0h}__asm {call ecx}__asm {mov result,eax}return result;}
inline void Write(const json& j){ReversingProbe::PumpMessages();log<<j.dump()<<'\n';log.flush();}
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};inline Fn fns[]{{"CShadows","ShadowRenderTriangleCB",0x709CF0,0,-1,3072}};
inline auto* FindHook(){ReversibleHooks::ReversibleHook::Base* h{};ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()=="CShadows")for(auto& x:c.Items())if(x->Name()=="ShadowRenderTriangleCB")h=x.get();});return h;}
inline void Update(){if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"shadow-triangle355-runtime.start")){done=true;return;}if(!log.is_open()){log.open(dir/"shadow-triangle355-runtime.jsonl");ReversingProbe::InstallFailureCapture("shadow-triangle355");Write({{"event","waiting_for_player"}});}if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;int pairs=0,failures=0,positive=0,inactive=0,faded=0;bool pass=true;
{
using ReversingProbe::EntryPatch;using std::bit_cast;
EntryPatch t(0x7EDD90,Transform),s(0x708290,Start),sd(bit_cast<uintptr_t>(&RenderBuffer::StartStoring),Start),e(0x7077A0,Stop),ed(bit_cast<uintptr_t>(&RenderBuffer::StopStoring),Stop);

for(int i=0;i<fns[0].cases;++i){auto run=[&](bool candidate){scenario=i;transforms=stores=0;trace=json::array();memset(vertices.data(),0xCD,sizeof(vertices));memset(indices.data(),0xCD,sizeof(indices));_ProjectionParam param{};memset(&param,0xCD,sizeof(param));param.at={0,0,1};param.shadowValue=uint8(i);param.fade=(i&8)?-1:0;std::array<CVector,3> triangle{CVector{1,2,3},CVector{4,5,6},CVector{7,8,9}};CVector normal{float(i%3-1),float(i%5-2),float(i%7-3)};
for(int k=0;k<3;++k){world[k]={float((i+k)%13-6)*.125f,float(i%7),float(i%19-9)*.125f};projection[k]={float((i/7+k)%9-4)*.25f,float((i/63+k)%9-4)*.25f,float((i/567+k)%9-4)*.5f};}if(i%37==0)projection[0].x=std::bit_cast<float>(0x7FC00123u);if(i%41==0)projection[1].z=std::bit_cast<float>(0x7FC00123u);if(i%43==0)normal.z=std::bit_cast<float>(0x7FC00123u);
// Sin hook en 0x709CF0 (EAX/EBX/EDI no se leen en C++): el original es el EXE por direccion con el adaptador de ensamblador de Call y el candidato la funcion de la DLL por simbolo.
if(i>=2048)Stress(i,normal,param);
bool switched=!ReversingProbe::JumpsToDll(0x709CF0);auto* result=candidate?ShadowRenderTriangleCB(&normal,triangle.data(),&param):Call(&normal,triangle.data(),&param);return json{{"trace",trace},{"vertices",Hash(vertices.data(),sizeof(vertices))},{"indices",indices},{"returned_triangle",result==triangle.data()},{"param",Hash(&param,sizeof(param))},{"triangle",Hash(triangle.data(),sizeof(triangle))},{"stores",stores},{"fade",param.fade},{"switches",switched}};};auto a=run(false),b=run(true);bool eq=a==b&&a["switches"]==true;pass&=eq;failures+=!eq;positive+=a["stores"].get<int>()>0;inactive+=a["stores"].get<int>()==0;faded+=a["stores"].get<int>()>0&&a["fade"].get<int>()!=0;Write({{"event","comparison"},{"function","CShadows::ShadowRenderTriangleCB"},{"pair",pairs++},{"case",i},{"equal",eq},{"accepted",eq},{"original",a},{"candidate",b}});}
}
pass&=positive>0&&inactive>0&&faded>0;Write({{"event","function_summary"},{"function","CShadows::ShadowRenderTriangleCB"},{"cases",pairs},{"failures",failures},{"positive",positive},{"inactive",inactive},{"faded",faded}});Write({{"event",pass?"complete":"error"},{"pairs",pairs},{"state_restored",true}});
}
}
