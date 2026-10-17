#pragma once
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "Camera.h"
#include "Entity/Ped/PlayerPed.h"
#include "CutsceneMgr.h"
#include "Hud.h"
#include "Text/Text.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
// Banco Dependencies21B: CCamera::ProcessObbeCinemaCameraCar/Train/Heli/Plane/Boat (0x5267C0, 0x526950, 0x526AE0, 0x526C80, 0x526E20).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con el hook activo. Los cinco colaboradores externos son falsos y se
// parchean con EntryPatch en la direccion del EXE y, cuando la DLL los llama directamente, tambien en su entrada de la DLL.
namespace Dependencies21BLive {
inline bool done{};inline std::ofstream log;
struct Fn{const char* cls;const char* name;uintptr_t address,vt;int slot,cases;};
inline constexpr int kCases=2048;
inline constexpr Fn fns[]{{"CCamera","ProcessObbeCinemaCameraCar",0x5267C0,0,-1,kCases},{"CCamera","ProcessObbeCinemaCameraTrain",0x526950,0,-1,kCases},{"CCamera","ProcessObbeCinemaCameraHeli",0x526AE0,0,-1,kCases},{"CCamera","ProcessObbeCinemaCameraPlane",0x526C80,0,-1,kCases},{"CCamera","ProcessObbeCinemaCameraBoat",0x526E20,0,-1,kCases}};
static_assert(sizeof(CCamera)==0xD78&&sizeof(CCam)==0x238);
struct Kind{int count;uintptr_t idx,tim,tbl;};
inline constexpr Kind kinds[]{{11,0x8CCEEC,0xB70120,0x8CC828},{6,0x8CCEF0,0xB70124,0x8CC858},{7,0x8CCEF4,0xB70128,0x8CC874},{6,0x8CCEF8,0xB7012C,0x8CC894},{3,0x8CCEFC,0xB70130,0x8CC8B0}};
inline uintptr_t Resolve(uintptr_t p){for(int i=0;i<16&&*reinterpret_cast<uint8*>(p)==0xE9;++i)p+=5+*reinterpret_cast<int32*>(p+1);return p;}
inline HMODULE ModuleOf(uintptr_t p){HMODULE m{};GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(p),&m);return m;}
inline void ModuleMarker(){}
struct Fpu {std::array<uint8,108> state;uint32 mxcsr;bool operator==(const Fpu&)const=default;};
inline Fpu CaptureFP(){Fpu f{};auto* p=f.state.data();auto* m=&f.mxcsr;__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]} __asm {mov eax,m} __asm {stmxcsr [eax]}return f;}
inline void RestoreFP(const Fpu& f){const auto* p=f.state.data();const auto m=f.mxcsr;__asm {mov eax,p} __asm {frstor [eax]} __asm {ldmxcsr m}}
inline bool writeFPActive{};inline Fpu writeFP{};
inline void Write(const json& r){const auto enteringFP=CaptureFP();const auto& baseline=writeFPActive?writeFP:enteringFP;RestoreFP(baseline);ReversingProbe::PumpMessages();RestoreFP(baseline);log<<r.dump()<<'\n';log.flush();RestoreFP(baseline);}
// ---- aleatorio determinista (identico para original y candidato)
struct Rng{uint64_t s;uint32 next(){s=s*6364136223846793005ull+1442695040888963407ull;return uint32(s>>33);}uint32 below(uint32 n){return next()%n;}};
inline std::string Hex(const std::vector<uint32>& v){static constexpr char d[]="0123456789abcdef";std::string s(v.size()*8,'0');for(size_t i=0;i<v.size();++i)for(int k=0;k<8;++k)s[i*8+k]=d[(v[i]>>(28-4*k))&15];return s;}
// ---- colaboradores falsos
inline std::vector<uint32> trace;inline uint8* cameraBytes{};inline Rng faker{1};inline uint32 tryMask,busyMask,timeMask,callNo[4],pokeRate,helpPoke;
template<class T> T& At(uintptr_t a){return *reinterpret_cast<T*>(a);}
inline void Snap(){auto push=[&](uint32 v){trace.push_back(v);};push(At<uint8>(0xB6EC2D));push(At<uint8>(0xB6EC34));push(uint8(At<int8>(0x8CC381)));for(int i=0;i<5;++i){push(At<uint32>(0x8CCEEC+4*i));push(At<uint32>(0xB70120+4*i));}push(At<uint32>(0xB7CB84));push(At<uint32>(uintptr_t(cameraBytes)+0xC34));}
// Los colaboradores pueden modificar el estado compartido (un ciclo posterior debe releerlo, como el EXE).
inline void Poke(){if(!pokeRate)return;if(faker.next()%pokeRate)return;const uint32 w=faker.next()%14,v=faker.next();
 switch(w){case 0:At<uint8>(0xB6EC2D)=uint8(v%3);break;case 1:At<uint8>(0xB6EC34)=uint8(v);break;case 2:At<int8>(0x8CC381)=int8(v);break;
  case 3:case 4:case 5:case 6:case 7:At<int32>(0x8CCEEC+4*(w-3))=int32(v%19)-3;break;case 8:case 9:case 10:case 11:case 12:At<uint32>(0xB70120+4*(w-8))=v;break;default:At<uint32>(0xB7CB84)=v;}}
inline bool Bit(uint32 mask,uint32 n){return (mask>>(n%32))&1;}
inline bool __fastcall FakeIsItTime(CCamera* self,void*,int32 mode,int32 time){trace.push_back(1);trace.push_back(uint32(reinterpret_cast<uint8*>(self)==cameraBytes));trace.push_back(uint32(mode));trace.push_back(uint32(time));Snap();const bool r=Bit(timeMask,callNo[0]++);Poke();return r;}
inline bool __fastcall FakeTry(CCamera* self,void*,int32 mode){trace.push_back(2);trace.push_back(uint32(reinterpret_cast<uint8*>(self)==cameraBytes));trace.push_back(uint32(mode));Snap();const bool r=Bit(tryMask,callNo[1]++);Poke();return r;}
inline bool __cdecl FakeBusy(){trace.push_back(3);Snap();const bool r=Bit(busyMask,callNo[2]++);Poke();return r;}
inline const GxtChar* __fastcall FakeGet(CText* self,void*,const char* key){trace.push_back(4);trace.push_back(uint32(uintptr_t(self)));for(int i=0;i<8;++i){const uint8 c=uint8(key[i]);trace.push_back(c);if(!c){for(++i;i<8;++i)trace.push_back(0);break;}}Snap();return reinterpret_cast<const GxtChar*>(uintptr_t(0x12340000u+callNo[3]));}
inline void __cdecl FakeHelp(uint32 text,uint32 quick,uint32 permanent,uint32 brief){trace.push_back(5);trace.push_back(text);trace.push_back(quick);trace.push_back(permanent);trace.push_back(brief);Snap();
 if(helpPoke==1)At<uint8>(0xB6EC2D)=uint8(callNo[3]&1);else if(helpPoke==2)At<uint8>(0xB6EC2D)=1;else if(helpPoke==3)At<uint8>(0xB6EC2D)=0;++callNo[3];Poke();}
// ---- llamada con medicion de ABI
struct Result{bool stack,nv,seh,fpu;};
inline Result Call(uintptr_t entry,void* self){Result result{};uint32 sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},h0{},h1{};uint8 env0[28]{},env1[28]{};auto* e0=env0;auto* e1=env1;
 __asm{
  mov eax,e0
  fnstenv [eax]
  fldenv [eax]
  mov sp0,esp
  mov b0,ebx
  mov s0,esi
  mov d0,edi
  mov p0,ebp
  mov eax,fs:[0]
  mov h0,eax
  mov ecx,self
  mov eax,0A5123456h
  mov edx,0C0DE5921h
  call entry
  mov sp1,esp
  mov b1,ebx
  mov s1,esi
  mov d1,edi
  mov p1,ebp
  mov eax,fs:[0]
  mov h1,eax
  mov eax,e1
  fnstenv [eax]
  fldenv [eax]
 }
 result.stack=sp0==sp1;result.nv=b0==b1&&s0==s1&&d0==d1&&p0==p1;result.seh=h0==h1;
 // fcw (0..1), fsw con TOP (4..5) y ftw (8..9) iguales antes y despues
 result.fpu=std::memcmp(env0,env1,2)==0&&std::memcmp(env0+4,env1+4,2)==0&&std::memcmp(env0+8,env1+8,2)==0;return result;}
struct Region{uintptr_t address;size_t size;};
// Zonas del EXE que la funcion lee o escribe (se guardan, se rellenan por caso y se restauran) y zonas vecinas que solo se vigilan.
// Las zonas vigiladas NO deben solapar las modificables (contador 0x8CC381, indices 0x8CCEEC..0x8CCEFF): la funcion las escribe legitimamente.
inline constexpr Region mutated[]{{0x8CC380,0x10},{0x8CC470,0x10},{0x8CC820,0xB0},{0x8CCEE8,0x20},{0xB6EC28,0x14},{0xB70120,0x20},{0xB7CB80,8}};
inline constexpr Region watched[]{{0x8CC000,0x380},{0x8CC390,0xE0},{0x8CC480,0x380},{0x8CCB00,0x3E8},{0x8CCF08,0xF8},{0xB6EC00,0x28},{0xB70100,0x20},{0xB70140,0x40},{0xB7CB60,0x20}};
inline json Diff(const std::vector<uint8>& before,const std::vector<uint8>& after,uintptr_t base){json d=json::array();for(size_t i=0;i<before.size();++i)if(before[i]!=after[i])d.push_back(json::array({base+i,after[i]}));return d;}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();if(!std::filesystem::exists(dir/"dependencies21b-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies21b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies21b");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto fp=CaptureFP();writeFP=fp;writeFPActive=true;
 // Rutas: el original es el EXE (hook apagado), el candidato el cuerpo de la DLL (hook encendido).
 std::vector<std::unique_ptr<OriginalDependencies499::HookRoute>> routes;
 routes.emplace_back(new OriginalDependencies499::HookRoute("CCamera","ProcessObbeCinemaCameraCar",0x5267C0,FunctionToVoidPtr(&CCamera::ProcessObbeCinemaCameraCar)));
 routes.emplace_back(new OriginalDependencies499::HookRoute("CCamera","ProcessObbeCinemaCameraTrain",0x526950,FunctionToVoidPtr(&CCamera::ProcessObbeCinemaCameraTrain)));
 routes.emplace_back(new OriginalDependencies499::HookRoute("CCamera","ProcessObbeCinemaCameraHeli",0x526AE0,FunctionToVoidPtr(&CCamera::ProcessObbeCinemaCameraHeli)));
 routes.emplace_back(new OriginalDependencies499::HookRoute("CCamera","ProcessObbeCinemaCameraPlane",0x526C80,FunctionToVoidPtr(&CCamera::ProcessObbeCinemaCameraPlane)));
 routes.emplace_back(new OriginalDependencies499::HookRoute("CCamera","ProcessObbeCinemaCameraBoat",0x526E20,FunctionToVoidPtr(&CCamera::ProcessObbeCinemaCameraBoat)));
 std::array<std::vector<uint8>,5> mainOff;for(size_t i=0;i<5;++i)mainOff[i]=ReversingProbe::Bytes(reinterpret_cast<void*>(fns[i].address),64);
 auto routeProof=[&](size_t i,bool candidate){const auto at=fns[i].address;return ReversingProbe::JumpsToDll(at)==candidate&&(candidate?(Resolve(at)==routes[i]->Body()&&ModuleOf(routes[i]->Body())==ModuleOf(uintptr_t(&ModuleMarker))):(Resolve(at)==at&&ModuleOf(at)==GetModuleHandleA(nullptr)&&mainOff[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(at),64)));};
 bool routesReady=true;for(size_t i=0;i<5;++i)routesReady&=routes[i]->Registered()&&routes[i]->Invoke(true,[&]{return routeProof(i,true);})&&routeProof(i,false);
 // Colaboradores falsos. IsItTimeForNewcam (0x51D770) y la comprobacion 0x50B830 no estan reconstruidos: la DLL los llama por la direccion del EXE.
 const auto tryBody=Resolve(bit_cast<uintptr_t>(&CCamera::TryToStartNewCamMode));const auto getBody=Resolve(bit_cast<uintptr_t>(&CText::Get));const auto helpBody=Resolve(bit_cast<uintptr_t>(&CHud::SetHelpMessage));
 EntryPatch time0(0x51D770,FakeIsItTime),try0(0x51E560,FakeTry),try1(tryBody,FakeTry),busy0(0x50B830,FakeBusy),get0(0x6A0050,FakeGet),get1(getBody,FakeGet),help0(0x588BE0,FakeHelp),help1(helpBody,FakeHelp);
 // Estado a restaurar
 std::vector<std::vector<uint8>> saved;for(const auto& r:mutated)saved.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(r.address),r.size));
 const auto stringSaved=ReversingProbe::Bytes(reinterpret_cast<void*>(0x863248),8);
 auto restore=[&]{for(size_t i=0;i<std::size(mutated);++i)std::memcpy(reinterpret_cast<void*>(mutated[i].address),saved[i].data(),saved[i].size());RestoreFP(fp);};
 auto restored=[&]{for(size_t i=0;i<std::size(mutated);++i)if(saved[i]!=ReversingProbe::Bytes(reinterpret_cast<void*>(mutated[i].address),saved[i].size()))return false;return stringSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x863248),8)&&CaptureFP()==fp;};
 const bool stringOk=std::memcmp(reinterpret_cast<void*>(0x863248),"CINCAM",7)==0;
 const bool ready=routesReady&&stringOk&&restored();
 json collaborators=json::array({json::array({0x51D770,0}),json::array({0x51E560,uintptr_t(tryBody)}),json::array({0x50B830,0}),json::array({0x6A0050,uintptr_t(getBody)}),json::array({0x588BE0,uintptr_t(helpBody)})});
 RestoreFP(fp);Write({{"event","begin"},{"ready",ready},{"routes_ready",routesReady},{"string_ok",stringOk},{"collaborators",collaborators},{"camera_bytes",sizeof(CCamera)},{"candidate_module",uintptr_t(ModuleOf(routes[0]->Body()))},{"probe_module",uintptr_t(ModuleOf(uintptr_t(&ModuleMarker)))},{"scope","private CCamera buffer; five collaborators replaced by traced fakes; live EXE statics saved/filled/restored per case"}});
 if(!ready){restore();bool hooks=true;for(auto& r:routes)hooks&=r->Restore();Write({{"event","error"},{"reason","fixture/routes preflight failed"},{"pairs",0},{"state_restored",hooks&&restored()}});return;}
 bool passed=true;unsigned pair=0;
 for(size_t fi=0;fi<5;++fi){
  const auto& k=kinds[fi];unsigned failures{};
  for(int n=0;n<kCases;++n,++pair){
   auto invoke=[&](bool candidate){
    // ---- fixture (derivado solo de fi y n)
    Rng r{uint64_t(n)*0x9E3779B97F4A7C15ull+fi+1};for(int i=0;i<4;++i)r.next();
    restore();
    static const uint32 ext[]{0,1,0x12,0x1D,0x7FFFFFFFu,0x80000000u,0xFFFFFFFFu,0xFFFFFFFEu};
    for(uintptr_t a=0x8CC820;a<0x8CC8C8;a+=4)At<uint32>(a)=r.below(3)==0?ext[r.below(8)]:r.below(70);
    alignas(16) static std::array<uint8,sizeof(CCamera)+64> arena;arena.fill(0xA5);auto* cam=arena.data()+16;cameraBytes=cam;for(size_t i=0;i<sizeof(CCamera);++i)cam[i]=uint8(r.next());
    const auto guardInitial=arena;
    cam[0x59]=uint8(r.below(6));cam[0xCF0]=uint8(r.below(4)==0);
    {const uint32 v=r.next();std::memcpy(cam+0xC34,&v,4);}
    for(int i=0;i<6;++i){uint16 m=r.below(3)==0?0x12:uint16(r.below(70));if(r.below(5)==0)m|=uint16(r.below(2)<<15);std::memcpy(cam+0x180+0x238*i,&m,2);}
    static const int32 idxs[]{-1,-1,0,1,2,3,4,5,6,7,8,9,10,11,12,-2,-1000,1000,15,0x40000000,int32(0x80000000),int32(0xC0000000),0x7FFFFFFF,0x3FFFFFFF};
    const uint32 sel=r.below(100);int32 idx=sel<70?int32(r.below(k.count+3))-1:idxs[r.below(24)];if(sel>=95)idx=-1;
    for(int i=0;i<5;++i){At<int32>(0x8CCEEC+4*i)=int32(r.below(k.count+3))-1;At<uint32>(0xB70120+4*i)=r.below(5)==0?ext[r.below(8)]:r.next();}
    At<int32>(k.idx)=idx;static const uint8 bd[]{0,0,1,1,2,0xFF,0x80};At<uint8>(0xB6EC2D)=bd[r.below(7)];At<uint8>(0xB6EC34)=r.below(3)==0?uint8(r.next()):uint8(r.below(2));
    static const int8 msgs[]{0,1,2,3,-1,127,-128,5};At<int8>(0x8CC381)=r.below(4)?msgs[r.below(8)]:int8(r.next());
    static const int8 steps[]{0,1,-1,2,-2,7,-7,127,-128,11,10,12,-11,5,3,-3,33,-33};At<int8>(0x8CC471)=r.below(4)?steps[r.below(18)]:int8(r.next());
    At<uint32>(0xB7CB84)=r.below(4)?r.below(1000000):(r.below(2)?ext[r.below(8)]:r.next());
    static const uint32 masks[]{0u,0xFFFFFFFFu,0xAAAAAAAAu,0x55555555u};
    const uint32 mm=r.below(10);tryMask=mm<2?masks[mm]:mm<7?(1u<<r.below(16)):r.next();timeMask=r.below(3)==0?masks[r.below(4)]:r.next();busyMask=r.below(3)==0?masks[r.below(4)]:r.next();
    pokeRate=r.below(3)==0?1+r.below(4):0;helpPoke=r.below(4);faker=Rng{r.next()|1};trace.clear();std::memset(callNo,0,sizeof callNo);
    // ---- entrada capturada
    std::vector<std::vector<uint8>> inputs,watchedIn;for(const auto& m:mutated)inputs.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(m.address),m.size));for(const auto& w:watched)watchedIn.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(w.address),w.size));
    const auto camIn=ReversingProbe::Bytes(cam,sizeof(CCamera));const auto fpBefore=CaptureFP();
    bool routeOk=false;RestoreFP(fp);Result abi{};
    routes[fi]->Invoke(candidate,[&]{routeOk=routeProof(fi,candidate);RestoreFP(fp);abi=Call(fns[fi].address,cam);routeOk&=routeProof(fi,candidate);return 0;});
    const bool fpuBalanced=CaptureFP()==fpBefore;
    // ---- salida
    json memDiff=json::array();for(size_t i=0;i<std::size(mutated);++i){auto d=Diff(inputs[i],ReversingProbe::Bytes(reinterpret_cast<void*>(mutated[i].address),mutated[i].size),mutated[i].address);for(auto& e:d)memDiff.push_back(e);}
    bool watchedOk=true;for(size_t i=0;i<std::size(watched);++i)watchedOk&=watchedIn[i]==ReversingProbe::Bytes(reinterpret_cast<void*>(watched[i].address),watched[i].size);
    const auto camDiff=Diff(camIn,ReversingProbe::Bytes(cam,sizeof(CCamera)),0);
    const bool guards=std::equal(arena.begin(),arena.begin()+16,guardInitial.begin())&&std::equal(arena.end()-48,arena.end(),guardInitial.end()-48);
    const bool string=stringSaved==ReversingProbe::Bytes(reinterpret_cast<void*>(0x863248),8);
    const auto traceCopy=trace;restore();const bool memory=restored();RestoreFP(fp);
    const uint32 finalMode=At<uint32>(uintptr_t(cam)+0xC34);
    return json{{"final_mode",finalMode},{"trace",Hex(traceCopy)},{"memory_diff",memDiff},{"camera_diff",camDiff},{"route_ok",routeOk},{"stack_restored",abi.stack},{"nonvolatile_preserved",abi.nv},{"seh_restored",abi.seh},{"fpu_env_preserved",abi.fpu},{"fpu_state_balanced",fpuBalanced},{"watched_unchanged",watchedOk},{"guards_preserved",guards},{"string_unchanged",string},{"state_restored",memory}};
   };
   const auto original=invoke(false);const auto candidate=invoke(true);
   // perfil (de la traza del original): llamadas a cada colaborador y destino final
   int calls[6]{};{auto hex=original["trace"].get<std::string>();std::vector<uint32> words;for(size_t i=0;i+8<=hex.size();i+=8)words.push_back(uint32(std::stoul(hex.substr(i,8),nullptr,16)));static const size_t lens[6]{0,19,18,16,25,20};for(size_t i=0;i<words.size();){const auto t=words[i];if(t<1||t>5)break;++calls[t];i+=lens[t];}}
   const uint32 finalMode=original["final_mode"].get<uint32>();
   bool equal=original==candidate;for(auto key:{"route_ok","stack_restored","nonvolatile_preserved","seh_restored","fpu_env_preserved","fpu_state_balanced","watched_unchanged","guards_preserved","string_unchanged","state_restored"})equal&=original[key]==true&&candidate[key]==true;
   passed&=equal;failures+=!equal;
   Write({{"event","comparison"},{"function",std::string(fns[fi].cls)+"::"+fns[fi].name},{"pair",pair},{"case",n},{"profile",{{"is_it_time",calls[1]},{"try_start",calls[2]},{"busy",calls[3]},{"get",calls[4]},{"help",calls[5]},{"final_mode",finalMode}}},{"original",original},{"candidate",candidate},{"equal",equal},{"accepted",equal}});
  }
  Write({{"event","function_summary"},{"function",std::string(fns[fi].cls)+"::"+fns[fi].name},{"cases",kCases},{"failures",failures}});
 }
 restore();bool hooks=true;for(auto& r:routes)hooks&=r->Restore();const bool memory=restored()&&hooks;RestoreFP(fp);
 Write({{"event",passed&&memory?"complete":"error"},{"pairs",pair},{"state_restored",memory},{"floating_state_restored",CaptureFP()==fp}});RestoreFP(fp);
}
}
