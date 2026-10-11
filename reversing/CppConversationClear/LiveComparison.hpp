#pragma once
// Banco por pares dentro del juego: CConversationNode::ClearRecursively (0x43A710), thiscall, ECX=nodo, sin pila.
// Candidato = ConversationNode.cpp (archivo completo /O2 /Oi /arch:IA32 /fp:fast) enganchado DIRECTAMENTE con RH_ScopedInstall.
// Registros crudos: EAX/ECX/EDX/ESI/EDI/EBX/EBP/ESP y EFLAGS capturados inmediatamente despues de la llamada.
// Fixtures SIN CICLOS (un ciclo recursaria sin fin en el original). No compilado ni ejecutado.
#include <fstream>
#include <set>
#include "Conversations/Conversations.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
namespace CppConversationClearLive {
struct Fn{const char* cls;const char* name;uintptr_t address,vtable;int slot,cases;};
inline constexpr Fn fns[]{
{"CConversationNode","ClearRecursively",0x43A710,0,-1,2048}
};
constexpr uintptr_t NODES=0x969570;constexpr int N=50;constexpr size_t SZ=0x18,TOTAL=size_t(N)*SZ,GUARD=64;
constexpr uint32 FLAGS_DEFINED=0x8C5,FLAG_DF=0x400,FLAGS_HYPOTHESIS=0x84; // CF|PF|ZF|SF|OF; AF es indefinida tras OR
inline bool done{};inline std::ofstream log;
inline void Write(const json& r);
struct Result{uint32 eax,ecx,edx,esi,edi,ebx,ebpBefore,ebpAfter,espBefore,espAfter,flagsBefore,flagsAfter;};
__declspec(noinline) inline Result Call(uintptr_t entry,uint32 node,uint32 edxSeed,uint32 eaxSeed){
 uint32 before{},after{},rb{},rs{},rd{},fb{},fa{},ea{},ec{},ed{},flagsBefore{},flagsAfter{};
 __asm {
  push ebx
  push esi
  push edi
  mov before,esp
  mov fb,ebp
  mov ebx,13579BDFh
  mov esi,2468ACE0h
  mov edi,10293847h
  mov ecx,node
  mov edx,edxSeed
  mov eax,eaxSeed
  cmp eax,0
  pushfd
  pop flagsBefore
  call entry
  pushfd
  pop flagsAfter
  mov ea,eax
  mov ec,ecx
  mov ed,edx
  mov after,esp
  mov fa,ebp
  mov rb,ebx
  mov rs,esi
  mov rd,edi
  pop edi
  pop esi
  pop ebx
 }
 return {ea,ec,ed,rs,rd,rb,fb,fa,before,after,flagsBefore,flagsAfter};
}
inline uint16 Top(){uint16 v{};__asm{fnstsw v}return v&0x3800;}
inline void SaveFP(void* p){__asm{mov eax,p}__asm{fnsave [eax]}__asm{frstor [eax]}}
inline void RestoreFP(void* p){__asm{mov eax,p}__asm{frstor [eax]}}
inline uint32 ReadMXCSR(){uint32 v{};__asm{stmxcsr v}return v;}
inline void WriteMXCSR(uint32 v){__asm{ldmxcsr v}}
alignas(16) inline std::array<uint8,108> canonicalFP{};inline uint32 canonicalMXCSR{};inline bool canonicalActive{};
inline void RestoreCanonical(){if(canonicalActive){RestoreFP(canonicalFP.data());WriteMXCSR(canonicalMXCSR);}}
inline bool CanonicalRestored(){alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==canonicalFP&&ReadMXCSR()==canonicalMXCSR;}
inline void Write(const json& r){alignas(16)std::array<uint8,108> current{};const auto mx=ReadMXCSR();if(!canonicalActive)SaveFP(current.data());RestoreCanonical();ReversingProbe::PumpMessages();log<<r.dump()<<'\n';log.flush();if(canonicalActive)RestoreCanonical();else{RestoreFP(current.data());WriteMXCSR(mx);}}
inline uintptr_t Resolve(uintptr_t e){for(int n=0;n<16&&*reinterpret_cast<const uint8*>(e)==0xE9;++n)e+=5+*reinterpret_cast<const int32*>(e+1);return e;}
inline bool ProbeModule(uintptr_t e){HMODULE owner{},probe{};const DWORD flags=GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT;return GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(e),&owner)&&GetModuleHandleExA(flags,reinterpret_cast<LPCSTR>(&Call),&probe)&&owner==probe;}
inline std::string Hex(uint64 v){char b[24];snprintf(b,sizeof b,"%016llx",static_cast<unsigned long long>(v));return b;}
inline uint64 Fnv(const void* p,size_t n){uint64 h=14695981039346656037ull;auto* b=static_cast<const uint8*>(p);while(n--){h^=*b++;h*=1099511628211ull;}return h;}
inline uint32 Rng(uint32& s){s^=s<<13;s^=s>>17;s^=s<<5;return s;}
using NodeBytes=std::array<uint8,TOTAL>;
inline CConversationNode* NodeAt(NodeBytes& b,int i){return reinterpret_cast<CConversationNode*>(b.data()+size_t(i)*SZ);}
// Comprobacion independiente de aciclicidad sobre los bytes reales (indices fuera de [-1,49] => invalido).
struct AcyclicCheck{
 const uint8* raw;std::array<int,N> color{};bool ok{true};
 int Link(int i,int which){int16 v;std::memcpy(&v,raw+size_t(i)*SZ+8+which*2,2);if(v<-1||v>=N)ok=false;return v;}
 void Run(int i){if(color[i]==2)return;if(color[i]==1){ok=false;return;}color[i]=1;for(int w=0;w<2&&ok;++w){const int t=Link(i,w);if(ok&&t>=0)Run(t);}color[i]=2;}
};
inline bool Acyclic(const uint8* raw){AcyclicCheck c{raw};for(int i=0;i<N&&c.ok;++i)c.Run(i);return c.ok;}
struct Fixture{NodeBytes bytes{},expected{};int root{},shape{},n{},reachable{},depth{};bool shared{},valid{};std::array<bool,N> reach{};};
struct ReachWalk{
 const std::array<int,N>& y;const std::array<int,N>& no;std::array<bool,N>& reach;std::array<int,N> deg{},memo{};
 int Go(int p){if(reach[p])return memo[p];reach[p]=true;int d=1;for(int t:{y[p],no[p]})if(t>=0){++deg[t];d=std::max(d,1+Go(t));}return memo[p]=d;}
};
// 16 formas x 128 variantes = 2048. Los hijos de la posicion p solo estan en posiciones mayores => aciclico por construccion.
inline Fixture Build(int c){
 Fixture f;uint32 s=uint32(c)*2654435761u^0x43A71000u;if(!s)s=1;Rng(s);Rng(s);
 const int shape=c%16,variant=c/16;f.shape=shape;
 int n=1+(variant*5+shape)%49;if(shape==0)n=1;if(shape==14)n=1+variant%8;if(shape==15)n=49;f.n=n;
 const int rm=variant%5;const int root=rm==0?0:rm==1?49:int(Rng(s)%N);f.root=root;
 std::array<int,N> order{};std::vector<int> rest;for(int i=0;i<N;++i)if(i!=root)rest.push_back(i);
 for(int i=int(rest.size())-1;i>0;--i)std::swap(rest[i],rest[Rng(s)%uint32(i+1)]);
 order[0]=root;for(int i=0;i<N-1;++i)order[i+1]=rest[i];
 std::array<int,N> yes{},no{};yes.fill(-1);no.fill(-1);
 switch(shape){
  case 0:break;
  case 1:for(int p=0;p+1<n;++p)yes[p]=p+1;break;
  case 2:for(int p=0;p+1<n;++p)no[p]=p+1;break;
  case 3:for(int p=0;p+1<n;++p)(p&1?no:yes)[p]=p+1;break;
  case 4:for(int p=0;p<n;++p){if(2*p+1<n)yes[p]=2*p+1;if(2*p+2<n)no[p]=2*p+2;}break;
  case 5:for(int p=0;p<n;p+=2){if(p+2<n)yes[p]=p+2;if(p+1<n)no[p]=p+1;}break;
  case 6:for(int p=0;p<n;p+=2){if(p+2<n)no[p]=p+2;if(p+1<n)yes[p]=p+1;}break;
  case 7:for(int p=0;p<n;++p){const int k=p%3,l=2*p+1,r=2*p+2;if(k!=1&&l<n)yes[p]=l;if(k!=0&&r<n)no[p]=r;}break;
  case 8:if(n>=2){yes[0]=1;no[0]=1;for(int p=1;p<n;++p){if(2*p<n)yes[p]=2*p;if(2*p+1<n)no[p]=2*p+1;}}break;
  case 9:for(int p=0;p<n;++p){if(p+1<n)yes[p]=p+1;if(p+2<n)no[p]=p+2;}break;
  case 10:for(int p=0;p+1<n;++p){if(Rng(s)&1)yes[p]=p+1+int(Rng(s)%uint32(n-p-1));if(Rng(s)&1)no[p]=p+1+int(Rng(s)%uint32(n-p-1));}break;
  case 11:case 14:for(int p=1;p<n;++p){for(int tries=0;;++tries){const int q=tries<32?int(Rng(s)%uint32(p)):(tries-32)%p;const bool wantYes=(Rng(s)&1)!=0;
    if(wantYes&&yes[q]<0){yes[q]=p;break;}if(!wantYes&&no[q]<0){no[q]=p;break;}if(tries>=32){if(yes[q]<0){yes[q]=p;break;}if(no[q]<0){no[q]=p;break;}}}}break;
  case 12:for(int p=0;p+1<n;++p){yes[p]=p+1;no[p]=p+1;}break;
  case 13:if(n>=4){yes[0]=n-1;no[0]=1;for(int p=1;p<=n-3;++p)yes[p]=p+1;yes[n-2]=n-1;}else for(int p=0;p+1<n;++p)yes[p]=p+1;break;
  case 15:for(int p=0;p+1<n;++p)((Rng(s)&1)?no:yes)[p]=p+1;break;
 }
 // Decoys fuera del arbol: pueden apuntar al arbol o hacia delante; el arbol nunca apunta a ellos.
 for(int d=n;d<N;++d)for(int w=0;w<2;++w){int t=-1;if(Rng(s)&1)t=(Rng(s)&1)?int(Rng(s)%uint32(n)):(d+1<N?d+1+int(Rng(s)%uint32(N-d-1)):-1);(w?no:yes)[d]=t;}
 for(int p=0;p<N;++p){const int idx=order[p];auto* nd=NodeAt(f.bytes,idx);
  nd->m_Name[0]=char('A'+(idx*7+c)%26);for(int k=1;k<8;++k)nd->m_Name[k]=char('a'+(idx+k*3+c)%26);
  nd->m_NodeYes=int16(yes[p]<0?-1:order[yes[p]]);nd->m_NodeNo=int16(no[p]<0?-1:order[no[p]]);
  const int32 base=int32(0x01000000|(c<<12))+idx*3;nd->m_Speech=base;nd->m_SpeechY=base+1;nd->m_SpeechN=base+2;}
 ReachWalk walk{yes,no,f.reach};f.depth=walk.Go(0);
 f.expected=f.bytes;for(int p=0;p<N;++p)if(f.reach[p]){++f.reachable;auto* nd=NodeAt(f.expected,order[p]);nd->m_Name[0]='\0';nd->m_NodeYes=-1;nd->m_NodeNo=-1;nd->m_Speech=0;nd->m_SpeechY=0;nd->m_SpeechN=0;}
 for(int p=0;p<N;++p)if(walk.deg[p]>1)f.shared=true;
 f.valid=Acyclic(f.bytes.data())&&f.reachable>=1&&f.reachable<=49;
 return f;
}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"cppconversationclear-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"cppconversationclear-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cppconversationclear");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 SaveFP(canonicalFP.data());canonicalMXCSR=ReadMXCSR();canonicalActive=true;
 static_assert(sizeof(CConversationNode)==0x18);
 auto* nodes=reinterpret_cast<uint8*>(NODES);const auto clean=ReversingProbe::Bytes(nodes,TOTAL);
 OriginalDependencies499::HookRoute route("CConversationNode","ClearRecursively",0x43A710,FunctionToVoidPtr(&CConversationNode::ClearRecursively));
 const auto entryOff=ReversingProbe::Bytes(reinterpret_cast<void*>(0x43A710),16);const auto bodyBytes=ReversingProbe::Bytes(reinterpret_cast<void*>(route.Body()),48);
 const bool layoutOk=reinterpret_cast<uintptr_t>(&CConversations::m_Nodes[0])==NODES&&CConversations::m_Nodes.size()==size_t(N)&&sizeof(CConversationNode)==SZ;
 RestoreCanonical();
 Write({{"event","begin"},{"registered_hook",route.Registered()},{"nodes_address_ok",layoutOk},{"nodes_address",NODES},{"collaborators_stubbed",false},{"fixture_configuration","clean snapshot of 50 CConversations::m_Nodes restored per run; acyclic graphs only"},
  {"entry_bytes_exe",entryOff},{"candidate_body_bytes",bodyBytes},{"candidate_body_probe_module",ProbeModule(route.Body())},{"fp_state_restored",CanonicalRestored()}});
 const uint32 eaxSeeds[8]{0,0xFFFFFFFF,0x80000000,0x7FFFFFFF,1,0x13579BDF,0x2468ACE0,0x01020304};
 bool passed=route.Registered()&&layoutOk;uint32 failures=0,shapeMask=0,root0=0,root49=0,sharedCases=0,pointerEdx=0,minReach=99,maxReach=0;std::set<uint32> flagValues;
 for(int c=0;c<fns[0].cases;++c){
  const auto fx=Build(c);
  const uint32 nodeAddr=uint32(NODES+SZ*size_t(fx.root));
  const int other=(fx.root+1+(c/2)%49)%N;
  const uint32 edxSeed=(c&1)?uint32(NODES+SZ*size_t(other)):(0xE5A00001u^uint32(c)*0x9E3779B1u);
  const uint32 eaxSeed=eaxSeeds[c%8];
  auto run=[&](bool candidate){
   RestoreCanonical();std::memcpy(nodes,clean.data(),TOTAL);std::memcpy(nodes,fx.bytes.data(),TOTAL);
   const auto before=ReversingProbe::Bytes(nodes,TOTAL);const auto low=ReversingProbe::Bytes(nodes-GUARD,GUARD),high=ReversingProbe::Bytes(nodes+TOTAL,GUARD);
   const bool fixtureOk=fx.valid&&before==std::vector<uint8>(fx.bytes.begin(),fx.bytes.end())&&Acyclic(nodes);
   Result r{};bool routeOk=false,called=false;
   if(fixtureOk){r=route.Invoke(candidate,[&]{routeOk=ReversingProbe::JumpsToDll(0x43A710)==candidate;if(candidate)routeOk&=Resolve(0x43A710)==route.Body()&&ProbeModule(route.Body());called=true;return Call(0x43A710,nodeAddr,edxSeed,eaxSeed);});}
   const auto after=ReversingProbe::Bytes(nodes,TOTAL);int touched=0;for(int i=0;i<N;++i)if(std::memcmp(before.data()+size_t(i)*SZ,after.data()+size_t(i)*SZ,SZ)!=0)++touched;
   const bool oracle=after==std::vector<uint8>(fx.expected.begin(),fx.expected.end())&&touched==fx.reachable;
   const bool guards=low==ReversingProbe::Bytes(nodes-GUARD,GUARD)&&high==ReversingProbe::Bytes(nodes+TOTAL,GUARD);
   std::memcpy(nodes,clean.data(),TOTAL);const bool restored=clean==ReversingProbe::Bytes(nodes,TOTAL);
   const uint32 defined=r.flagsAfter&FLAGS_DEFINED;RestoreCanonical();
   json v={{"root",fx.root},{"reachable",fx.reachable},{"touched",touched},{"nodes_hash_after",Hex(Fnv(after.data(),after.size()))},{"expected_hash",Hex(Fnv(fx.expected.data(),TOTAL))},
    {"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"esi",r.esi},{"edi",r.edi},{"ebx",r.ebx},{"edx_seed",edxSeed},{"eax_seed",eaxSeed},{"ecx_seed",nodeAddr},
    {"flags_before_defined",r.flagsBefore&FLAGS_DEFINED},{"flags_defined",defined},{"flags_df_before",r.flagsBefore&FLAG_DF},{"flags_df_after",r.flagsAfter&FLAG_DF},{"flags_hypothesis_ok",defined==FLAGS_HYPOTHESIS},
    {"called",called},{"fixture_acyclic",fixtureOk},{"route_ok",routeOk},{"stack_restored",called&&r.espBefore==r.espAfter},
    {"registers_preserved",called&&r.ebpBefore==r.ebpAfter&&r.ebx==0x13579BDF&&r.esi==0x2468ACE0&&r.edi==0x10293847},{"edx_preserved",called&&r.edx==edxSeed},
    {"df_preserved",called&&(r.flagsBefore&FLAG_DF)==(r.flagsAfter&FLAG_DF)},{"oracle_ok",oracle},{"guards_ok",guards},{"snapshot_restored",restored},{"fp_state_restored",CanonicalRestored()}};
   RestoreCanonical();return v;
  };
  const auto original=run(false),candidate=run(true);bool equal=route.Registered()&&original==candidate;
  for(auto k:{"called","fixture_acyclic","route_ok","stack_restored","registers_preserved","edx_preserved","df_preserved","oracle_ok","guards_ok","snapshot_restored","fp_state_restored"})equal&=original[k]==true&&candidate[k]==true;
  failures+=!equal;passed&=equal;shapeMask|=1u<<fx.shape;root0+=fx.root==0;root49+=fx.root==49;sharedCases+=fx.shared;pointerEdx+=(c&1);minReach=std::min<uint32>(minReach,fx.reachable);maxReach=std::max<uint32>(maxReach,fx.reachable);flagValues.insert(original["flags_defined"].get<uint32>());
  Write({{"event","comparison"},{"pair",c},{"case",c},{"function","CConversationNode::ClearRecursively"},{"shape",fx.shape},{"root",fx.root},{"nodes_in_shape",fx.n},{"reachable",fx.reachable},{"depth",fx.depth},{"shared",fx.shared},
   {"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
 }
 const bool covered=shapeMask==0xFFFF&&root0&&root49&&sharedCases&&pointerEdx&&minReach==1&&maxReach==49&&flagValues.size()==1;passed&=covered;
 Write({{"event","function_summary"},{"function","CConversationNode::ClearRecursively"},{"cases",fns[0].cases},{"failures",failures},{"complete_paths",covered},{"shape_mask",shapeMask},{"root0_cases",root0},{"root49_cases",root49},
  {"shared_cases",sharedCases},{"edx_pointer_cases",pointerEdx},{"min_reachable",minReach},{"max_reachable",maxReach},{"flags_defined_values",flagValues}});
 std::memcpy(nodes,clean.data(),TOTAL);const bool hooks=route.Restore();RestoreCanonical();const bool fp=CanonicalRestored(),restored=hooks&&fp&&clean==ReversingProbe::Bytes(nodes,TOTAL);
 Write({{"event",passed&&restored?"complete":"error"},{"pairs",fns[0].cases},{"state_restored",restored},{"fp_state_restored",fp},{"hook_restored",hooks}});
}
}
