#pragma once
#include <fstream>
#include "World.h"
#include "CutsceneMgr.h"
#include "Cam.h"
#include "Weather.h"
#include "FxEmitter.h"
#include "FxManager.h"
#include "FxPrimBP.h"
#include "FxSystem.h"
#include "FxSystemBP.h"
#include "FxInfoManager.h"
#include "FxPrtMult.h"
#include "EmissionInfo.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"
#include "Arenas.hpp"

// Banco par a par de dos entradas del EXE contra su reconstruccion en C++ (hook RH apagado frente a encendido, dentro del juego):
//   CCam::WrapAngle                 0x509BE0  (cdecl, float*; el EXE conserva EDX y ECX = el puntero)               2048 pares
//   FxEmitter_c::CreateParticles    0x4A41E0  (thiscall, float, float; ret 8)                                         4096 pares
// Los objetos son arenas propios del banco (Arenas.hpp). Colaboradores de CreateParticles (sustituidos en AMBOS lados por los mismos stubs trazados con EntryPatch en la direccion
// del EXE y en la entrada del envoltorio de la DLL): FxInfoManager_c::ProcessEmissionInfo (0x4A4960), FxManager_c::FxRwMatrixCreate (0x4A9440) y FxRwMatrixDestroy (0x4A9460),
// FxPrimBP_c::GetRWMatrix (0x4A9DC0), RwMatrixUpdate (0x7F18A0) y RwMatrixMultiply (0x7F18B0); FxEmitter_c::CreateParticle (0x4A2580) es una funcion del EXE sin reconstruir que la DLL
// llama por direccion (un solo parche). El constructor de FxPrtMult_c (0x4AB270 en el EXE, el de la DLL en el candidato) y _ftol2 (0x821B40 en el EXE; el del CRT de la DLL mas los
// ayudantes de X87Math en el candidato) se ejecutan de verdad. La traza (tipo, argumentos como desplazamientos en el arena, bits de los floats y hashes de las estructuras) forma
// parte de la comparacion. Las globales del juego que lee el EXE (CWeather::Wind/Rain, 0xC812F0 y 0xC81324) se guardan y restauran.
namespace Dependencies22CLive {
using namespace Dependencies22CFix;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(address),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t address) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
// Deja en los ocho registros ST un residuo conocido y distinto (carga ocho doubles y los descarta): el FNSAVE posterior compara tambien el contenido de los registros vacios.
inline void PrimeX87() {
    static const double stale[8]{1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};
    const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE (salvo FIP), incluido el contenido crudo de los registros ST marcados como vacios en FTW.
// false: esos registros vacios se ponen a cero en ambos lados antes de comparar. Relajarla es una decision de politica del propietario del proyecto: no se ha tomado.
inline constexpr bool kCompareDeadX87Registers=true;

struct Reg {uint32 eax,ecx,edx;bool stack,nonvolatile;};
// cdecl con un argumento, con la semilla de EDX por caso (EAX/ECX con semillas fijas); se capturan justo tras la llamada
inline Reg GateCdecl1Edx(uintptr_t entry,uint32 a1,uint32 edxSeed){uint32 before{},after{},ra{},rc{},rd{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {push a1} __asm {mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,edxSeed} __asm {call entry}
 __asm {mov ra,eax} __asm {mov rc,ecx} __asm {mov rd,edx} __asm {add esp,4} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {ra,rc,rd,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}
// thiscall con dos argumentos de pila (el llamado los retira: ret 8); ECX = this
inline Reg GateThis2(uintptr_t entry,uint32 self,uint32 a1,uint32 a2){uint32 before{},after{},ra{},rc{},rd{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {push a2} __asm {push a1} __asm {mov eax,0A5123456h} __asm {mov edx,0C0DE5921h} __asm {mov ecx,self} __asm {call entry}
 __asm {mov ra,eax} __asm {mov rc,ecx} __asm {mov rd,edx} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {ra,rc,rd,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}

inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
// Vigila bytes de codigo (entradas parcheadas temporalmente): deben volver a su contenido inicial tras cada llamada.
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

// ---- FNSAVE (108 bytes, modo protegido de 32 bits): +0 FCW, +4 FSW, +8 FTW (2 bits por registro fisico, 3 = vacio), +12 FIP, +16 FCS|FOP<<16, +20 FDP, +24 FDS, +28 ST0..ST7 (10 bytes, orden de pila).
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
 json out=json::array();
 const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw",4,2},{"ftw",8,2},{"fip",12,4},{"fcs_fop",16,4},{"fdp",20,4},{"fds",24,4}};
 for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
 for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
 return out;
}
// Clase de la ultima instruccion x87 (por los bytes en FIP): 1 = fcomp m32 (D8 /3), 2 = fstp m32 (D9 /3), 3 = otra, 0 = sin FIP.
inline unsigned FpOpcodeClass(uint32 ip){
 if(!ip)return 0;const auto* code=reinterpret_cast<const uint8*>(ip);
 if((code[1]&0xC0)==0xC0)return 3;
 if(code[0]==0xD8&&(code[1]&0x38)==0x18)return 1;
 if(code[0]==0xD9&&(code[1]&0x38)==0x18)return 2;
 return 3;
}

struct Run {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{},ip{};bool routeOK{};};
template<class G> Run Execute(OriginalDependencies499::HookRoute& route,bool candidate,uintptr_t entry,G gate){
 Run run{};
 route.Invoke(candidate,[&]{
  run.routeOK=candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(entry);
  run.reg=gate();SaveFP(run.fp.data());run.mxcsr=ReadMXCSR();return 0;});
 memcpy(&run.ip,run.fp.data()+12,4);return run;
}

// ---- Stubs trazados de los colaboradores de CreateParticles ----
inline std::vector<uint32> trace;
inline uint8* traceBase{};
inline std::array<uint32,kEmissionWords> emissionSource{};
inline int matrixCount{};
inline bool createCalled{};
inline uint32 Off(const void* p){return p?uint32(reinterpret_cast<const uint8*>(p)-traceBase):0xFFFFFFFFu;}
inline uint32 Fnv(const void* p,size_t n){uint32 h=2166136261u;for(size_t i=0;i<n;++i){h^=reinterpret_cast<const uint8*>(p)[i];h*=16777619u;}return h;}
inline uint32 Bits(float f){uint32 v;memcpy(&v,&f,4);return v;}
inline void __fastcall StubProcessEmissionInfo(FxInfoManager_c* self,void*,float currentTime,float mult,float totalTime,bool useConst,EmissionInfo_t* out){
 trace.insert(trace.end(),{1u,Off(self),Bits(currentTime),Bits(mult),Bits(totalTime),uint32(useConst?1u:0u)});memcpy(out,emissionSource.data(),sizeof(EmissionInfo_t));}
inline RwMatrix* __fastcall StubCreate(void*,void*){createCalled=true;auto* m=reinterpret_cast<RwMatrix*>(traceBase+CpMatrix(uint32(matrixCount++)%3));trace.insert(trace.end(),{2u,Off(m)});return m;}
inline void __fastcall StubDestroy(void*,void*,RwMatrix* m){trace.insert(trace.end(),{3u,Off(m)});}
inline void __fastcall StubGetRWMatrix(FxPrimBP_c* self,void*,RwMatrix* m){
 trace.insert(trace.end(),{4u,Off(self),Off(m)});auto* w=reinterpret_cast<uint32*>(m);for(uint32 i=0;i<16;++i)w[i]=0x600DF000u+i*7u+uint32(matrixCount);}
inline RwMatrix* __cdecl StubRwMatrixUpdate(RwMatrix* m){trace.insert(trace.end(),{5u,Off(m),Fnv(m,0x40)});return m;}
inline RwMatrix* __cdecl StubRwMatrixMultiply(RwMatrix* d,const RwMatrix* a,const RwMatrix* b){
 trace.insert(trace.end(),{6u,Off(d),Off(a),Off(b),Fnv(a,0x40),Fnv(b,0x40)});auto* w=reinterpret_cast<uint32*>(d);const uint32 h=Fnv(a,0x40)^(Fnv(b,0x40)*31u);for(uint32 i=0;i<16;++i)w[i]=h+i;return d;}
inline void* __fastcall StubCreateParticle(FxEmitter_c* self,void*,const EmissionInfo_t* em,RwMatrix* wld,const CVector* vel,float timeSince,const FxPrtMult_c* mult,float brightness,bool createLocal){
 trace.insert(trace.end(),{8u,Off(self),Fnv(em,sizeof(EmissionInfo_t)),Off(wld),Fnv(wld,0x40),uint32(vel?1u:0u),Bits(timeSince),Fnv(mult,sizeof(FxPrtMult_c)),Bits(brightness),uint32(createLocal?1u:0u)});return nullptr;}
inline uintptr_t Entry(auto member){return Resolve(std::bit_cast<uintptr_t>(member));}

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies22c-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies22c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies22c");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
 // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado.
 OriginalDependencies499::HookRoute wrap("CCam","WrapAngle",0x509BE0,OriginalDependencies499::HookRoute::RegisteredTarget{});
 OriginalDependencies499::HookRoute create("FxEmitter_c","CreateParticles",0x4A41E0,&FxEmitter_c::CreateParticles);
 // Entradas de la DLL de los colaboradores (se resuelven saltando los E9 de enlace incremental) y direcciones del EXE.
 const uintptr_t peiDll=Entry(&FxInfoManager_c::ProcessEmissionInfo),createMatDll=Entry(&FxManager_c::FxRwMatrixCreate),destroyMatDll=Entry(&FxManager_c::FxRwMatrixDestroy),
  getRwDll=Entry(&FxPrimBP_c::GetRWMatrix),updateDll=Resolve(uintptr_t(&RwMatrixUpdate)),multiplyDll=Resolve(uintptr_t(&RwMatrixMultiply));
 const uintptr_t exeEntries[]{0x4A4960,0x4A9440,0x4A9460,0x4A9DC0,0x7F18A0,0x7F18B0,0x4A2580};
 const uintptr_t dllEntries[]{peiDll,createMatDll,destroyMatDll,getRwDll,updateDll,multiplyDll};
 bool collaboratorsOK=true;for(uintptr_t at:dllEntries)collaboratorsOK&=ModuleOf(at)==asi;
 // CreateParticle (0x4A2580) debe ser, hoy, el codigo del EXE sin hook (la DLL lo llama por direccion).
 collaboratorsOK&=!ReversingProbe::JumpsToDll(0x4A2580)&&ModuleOf(Resolve(0x4A2580))==gameExe;
 bool ready=wrap.Registered()&&create.Registered()&&collaboratorsOK&&ModuleOf(wrap.Body())==asi&&ModuleOf(create.Body())==asi;
 std::vector<Watch> watches;
 for(uintptr_t at:{uintptr_t(0x509BE0),uintptr_t(0x4A41E0)})watches.emplace_back(at,16);
 for(uintptr_t at:exeEntries)watches.emplace_back(at,16);
 for(uintptr_t at:dllEntries)watches.emplace_back(at,16);
 auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
 // Globales del juego que lee CreateParticles.
 const uint32 savedWind=*reinterpret_cast<uint32*>(&CWeather::Wind),savedRain=*reinterpret_cast<uint32*>(&CWeather::Rain);
 auto globalsRestored=[&]{return *reinterpret_cast<uint32*>(&CWeather::Wind)==savedWind&&*reinterpret_cast<uint32*>(&CWeather::Rain)==savedRain;};
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 json watched=json::array();for(const auto& w:watches)watched.push_back({{"address",w.address},{"bytes",Hex(w.address,16)}});
 constexpr unsigned total=kWrapCases+kCreateCases;
 Write({{"event","begin"},{"registered_hook",ready},{"functions",2},{"pairs",total},{"cases",{{"Wrap",kWrapCases},{"Create",kCreateCases}}},
  {"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},
  {"collaborators",{{"exe",std::vector<uintptr_t>(std::begin(exeEntries),std::end(exeEntries))},{"dll",std::vector<uintptr_t>(std::begin(dllEntries),std::end(dllEntries))}}},
  {"watched_entries",watched},{"bodies",{{"Wrap",wrap.Body()},{"Create",create.Body()}}},{"asi_module",asi},{"original_module",gameExe},
  {"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"initial_wind",savedWind},{"initial_rain",savedRain}});
 int pairs{};bool passed=ready;
 auto prepare=[&]{RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);};
 auto finishRun=[&](json& out,const Run& run,bool fpIpRouteOK){
  auto fp=run.fp;memset(fp.data()+12,0,4);const auto logged=kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);
  out["post_floating_state"]=Hex(uintptr_t(logged.data()),108);out["post_mxcsr"]=run.mxcsr;out["fp_ip_route_ok"]=fpIpRouteOK;
  out["stack_restored"]=run.reg.stack;out["nonvolatile_preserved"]=run.reg.nonvolatile;out["route_ok"]=run.routeOK;
  RestoreFloatingState();out["globals_restored"]=watchesIntact()&&globalsRestored();out["floating_state_restored"]=FloatingStateRestored();
  out["last_x87_class"]=FpOpcodeClass(run.ip);
 };
 auto emitPair=[&](const char* function,unsigned scenario,const json& a,const json& b,const Run& ra,const Run& rb,int& failures){
  bool ok=a==b;
  for(const char* k:{"fp_ip_route_ok","stack_restored","nonvolatile_preserved","route_ok","globals_restored","floating_state_restored"})ok&=a[k]==true&&b[k]==true;
  auto fa=ra.fp,fb=rb.fp;memset(fa.data()+12,0,4);memset(fb.data()+12,0,4);
  passed&=ok;failures+=!ok;
  Write({{"event","comparison"},{"function",function},{"case",scenario},{"pair",pairs++},{"original",a},{"candidate",b},{"fp_difference",FpDifference(fa,fb)},
   {"original_residue",{{"eax",ra.reg.eax},{"ecx",ra.reg.ecx},{"edx",ra.reg.edx}}},{"candidate_residue",{{"eax",rb.reg.eax},{"ecx",rb.reg.ecx},{"edx",rb.reg.edx}}},
   {"original_fp_ip",ra.ip},{"candidate_fp_ip",rb.ip},{"equal",ok},{"accepted",ok}});
 };

 // ============================================================ CCam::WrapAngle (0x509BE0)
 int failuresWrap{};
 alignas(16) static std::array<uint8,kWrapArena+32> wrapBytes{};
 for(unsigned scenario=0;scenario<kWrapCases&&ready;++scenario){
  const auto fx=Wrap(scenario);Run runs[2];json rows[2];
  for(int side=0;side<2;++side){
   const bool candidate=side==1;prepare();
   uint8* base=wrapBytes.data()+16;memset(wrapBytes.data(),0xA5,16);memset(base+kWrapArena,0xA5,16);BuildWrap(base,fx);
   const uint32 pointer=Ptr(base,WrapAngleOffset(fx));
   runs[side]=Execute(wrap,candidate,0x509BE0,[&]{return GateCdecl1Edx(0x509BE0,pointer,fx.edx);});
   // Ultima instruccion x87: fcomp m32 (la comparacion con -pi) o fstp m32 (el bucle que suma 2pi), en el EXE (original) o en el ASI (candidato).
   const uint32 cls=FpOpcodeClass(runs[side].ip);const uintptr_t where=ModuleOf(runs[side].ip);
   const bool ipOK=(cls==1||cls==2)&&where==(candidate?asi:gameExe);
   json out={{"input",{{"case",scenario},{"angle",fx.angle},{"kind",fx.kind},{"slot",fx.slot},{"fill",fx.fill},{"edx",fx.edx}}},{"base",uintptr_t(base)},
    {"state",Hex(uintptr_t(wrapBytes.data()),wrapBytes.size())},{"edx_after",runs[side].reg.edx},{"edx_preserved",runs[side].reg.edx==fx.edx},
    {"ecx_after",runs[side].reg.ecx},{"ecx_is_pointer",runs[side].reg.ecx==pointer}};
   finishRun(out,runs[side],ipOK);rows[side]=out;
  }
  emitPair("CCam::WrapAngle",scenario,rows[0],rows[1],runs[0],runs[1],failuresWrap);
 }
 Write({{"event","function_summary"},{"function","CCam::WrapAngle"},{"cases",int(kWrapCases)},{"failures",failuresWrap},{"state_restored",watchesIntact()}});

 // ============================================================ FxEmitter_c::CreateParticles (0x4A41E0)
 int failuresCreate{};
 alignas(16) static std::array<uint8,kCpArena+32> createBytes{};
 for(unsigned scenario=0;scenario<kCreateCases&&ready;++scenario){
  const auto fx=Create(scenario);Run runs[2];json rows[2];
  for(int side=0;side<2;++side){
   const bool candidate=side==1;prepare();
   uint8* base=createBytes.data()+16;memset(createBytes.data(),0xA5,16);memset(base+kCpArena,0xA5,16);BuildCreate(base,fx);
   BuildEmission(emissionSource.data(),fx);trace.clear();traceBase=base;matrixCount=0;createCalled=false;
   *reinterpret_cast<uint32*>(&CWeather::Wind)=fx.wind;*reinterpret_cast<uint32*>(&CWeather::Rain)=fx.rain;
   const uint32 emitter=Ptr(base,kCpEmitter);float currentTime,deltaTime;memcpy(&currentTime,&fx.time,4);memcpy(&deltaTime,&fx.delta,4);
   uint32 timeBits,deltaBits;memcpy(&timeBits,&currentTime,4);memcpy(&deltaBits,&deltaTime,4);
   runs[side]=Execute(create,candidate,0x4A41E0,[&]{
    ReversingProbe::EntryPatch pei0(0x4A4960,&StubProcessEmissionInfo),createExe(0x4A9440,&StubCreate),destroyExe(0x4A9460,&StubDestroy),getRwExe(0x4A9DC0,&StubGetRWMatrix),
     updateExe(0x7F18A0,&StubRwMatrixUpdate),multiplyExe(0x7F18B0,&StubRwMatrixMultiply),particleExe(0x4A2580,&StubCreateParticle),
     pei1(peiDll,&StubProcessEmissionInfo),createDll(createMatDll,&StubCreate),destroyDll(destroyMatDll,&StubDestroy),getRwDll2(getRwDll,&StubGetRWMatrix),
     updateDll2(updateDll,&StubRwMatrixUpdate),multiplyDll2(multiplyDll,&StubRwMatrixMultiply);
    return GateThis2(0x4A41E0,emitter,timeBits,deltaBits);});
   *reinterpret_cast<uint32*>(&CWeather::Wind)=savedWind;*reinterpret_cast<uint32*>(&CWeather::Rain)=savedRain;
   const uint32 cls=FpOpcodeClass(runs[side].ip);const uintptr_t where=ModuleOf(runs[side].ip);
   // Salida pronta (sin matrices): la ultima x87 es una comparacion fcomp m32; si llega al final, el fstp m32 de la intensidad.
   const bool continued=createCalled;
   const bool ipOK=(cls==1||cls==2)&&where==(candidate?asi:gameExe);
   json out={{"input",{{"case",scenario},{"dist",fx.dist},{"rate",fx.rate},{"lod_start",fx.lodStart},{"lod_end",fx.lodEnd},{"count",fx.count},{"init",fx.init},{"wind",fx.wind},{"rain",fx.rain},
     {"min_wind",fx.minWind},{"max_wind",fx.maxWind},{"min_rain",fx.minRain},{"max_rain",fx.maxRain},{"delta",fx.delta},{"time",fx.time},{"parent",fx.parent},{"create_local",fx.createLocal},
     {"flags_other",fx.flagsOther},{"emission_fill",fx.emissionFill},{"benign",fx.benign},{"guarded",fx.guarded}}},
    {"base",uintptr_t(base)},{"state",Hex(uintptr_t(createBytes.data()),createBytes.size())},{"trace",trace},{"intensity",Get32(base,kCpEmitter+0x10)},{"continued",continued}};
   finishRun(out,runs[side],ipOK);rows[side]=out;
  }
  emitPair("FxEmitter_c::CreateParticles",scenario,rows[0],rows[1],runs[0],runs[1],failuresCreate);
 }
 Write({{"event","function_summary"},{"function","FxEmitter_c::CreateParticles"},{"cases",int(kCreateCases)},{"failures",failuresCreate},{"state_restored",watchesIntact()}});

 // Las vigilancias se tomaron con los dos hooks apagados (el constructor de HookRoute los apaga): se comprueban ANTES de Restore(), que vuelve a encenderlos.
 const bool entriesIntactBefore=watchesIntact();
 bool stateRestored=entriesIntactBefore&&globalsRestored();stateRestored&=wrap.Restore();stateRestored&=create.Restore();
 for(size_t i=2;i<watches.size();++i)stateRestored&=watches[i].Intact();
 RestoreFloatingState();stateRestored&=FloatingStateRestored();
 Write({{"event","global_state_final"},{"floating_state",Hex(uintptr_t(savedFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore},
  {"wind",*reinterpret_cast<uint32*>(&CWeather::Wind)},{"rain",*reinterpret_cast<uint32*>(&CWeather::Rain)}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
 RestoreFloatingState();savedFPActive=false;
}
}
