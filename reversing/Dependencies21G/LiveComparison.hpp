#pragma once
#include <fstream>
#include "World.h"
#include "CutsceneMgr.h"
#include "Core/Quaternion.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"
#include "Arenas.hpp"

// Banco par a par de tres entradas del EXE contra su reconstruccion en C++ (hook RH apagado frente a encendido, dentro del juego):
//   RwHelper::GetFirstTexture                                  0x734940  (cdecl, 1 argumento)      1024 pares
//   RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial      0x734DE0  (cdecl, 3 argumentos)     1536 pares
//   CQuaternion::Slerp (halftheta, sintheta_inv, t)            0x59C300  (thiscall, ret 0x14)      4096 pares
// Los objetos son arenas propios del banco (Arenas.hpp): no se toca ningun objeto del motor RenderWare ni del juego.
// Colaboradores de ReplaceOld: _rpMaterialListFindMaterialIndex (0x74E420) y RpMaterialDestroy (0x74DA20) se sustituyen en AMBOS lados por el mismo
// par de stubs trazados (EntryPatch en la direccion del EXE y en la entrada del envoltorio de la DLL): el stub de FindIndex repite el recorrido hacia atras del
// EXE y el de Destroy decrementa refCount sin liberar (el camino refCount == 1 libera memoria del motor y no se ejecuta). La traza forma parte de la comparacion.
namespace Dependencies21GLive {
using namespace Dependencies21GFix;
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
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE (salvo FIP en Slerp), incluido el contenido crudo de los registros ST marcados como vacios en FTW.
// false: esos registros vacios se ponen a cero en ambos lados antes de comparar (FCW, FSW, FTW, TOP y los registros con datos siguen comparandose enteros).
// Relajarla es una decision de politica del propietario del proyecto: no se ha tomado.
inline constexpr bool kCompareDeadX87Registers=true;

struct Reg {uint32 eax,ecx,edx;bool stack,nonvolatile;};
// cdecl con un argumento (EAX/ECX/EDX con semillas distintas; se capturan justo tras la llamada)
inline Reg GateCdecl1(uintptr_t entry,uint32 a1){uint32 before{},after{},ra{},rc{},rd{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {push a1} __asm {mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,0C0DE5921h} __asm {call entry}
 __asm {mov ra,eax} __asm {mov rc,ecx} __asm {mov rd,edx} __asm {add esp,4} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {ra,rc,rd,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}
// cdecl con tres argumentos
inline Reg GateCdecl3(uintptr_t entry,uint32 a1,uint32 a2,uint32 a3){uint32 before{},after{},ra{},rc{},rd{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {push a3} __asm {push a2} __asm {push a1} __asm {mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,0C0DE5921h} __asm {call entry}
 __asm {mov ra,eax} __asm {mov rc,ecx} __asm {mov rd,edx} __asm {add esp,12} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {ra,rc,rd,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}
// thiscall con cinco argumentos de pila (el llamado los retira: ret 0x14); ECX = this
inline Reg GateThis5(uintptr_t entry,uint32 self,uint32 a1,uint32 a2,uint32 a3,uint32 a4,uint32 a5){uint32 before{},after{},ra{},rc{},rd{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {push a5} __asm {push a4} __asm {push a3} __asm {push a2} __asm {push a1} __asm {mov eax,0A5123456h} __asm {mov edx,0C0DE5921h} __asm {mov ecx,self} __asm {call entry}
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

// ---- Stubs trazados de los colaboradores de ReplaceOld ----
inline std::vector<uint32> trace;
inline uintptr_t traceBase{};
inline int32 __cdecl FindIndexStub(const RpMaterialList* list,const RpMaterial* material){
 trace.push_back(1);trace.push_back(uint32(reinterpret_cast<uintptr_t>(list)-traceBase));trace.push_back(uint32(reinterpret_cast<uintptr_t>(material)-traceBase));
 const int32 count=list->numMaterials;if(count<=0)return count-1;   // 0x74E442: jle -> dec eax
 for(int32 i=count-1;i>=0;--i)if(list->materials[i]==material)return i;
 return -1;
}
inline RwBool __cdecl DestroyStub(RpMaterial* material){
 trace.push_back(2);trace.push_back(uint32(reinterpret_cast<uintptr_t>(material)-traceBase));trace.push_back(0);
 if(uint16(material->refCount)!=1)material->refCount=int16(material->refCount-1);   // 0x74DA25..0x74DA73; con refCount == 1 el original libera: el stub no lo hace
 return 1;
}
inline uint16 ReadWord(uintptr_t at){uint16 v{};memcpy(&v,reinterpret_cast<void*>(at),2);return v;}

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies21g-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies21g-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies21g");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
 // Rutas RH: se construyen antes que nada; cada constructor deja su hook apagado.
 OriginalDependencies499::HookRoute firstTexture("RwHelper","GetFirstTexture",0x734940,&GetFirstTexture);
 OriginalDependencies499::HookRoute replace("RwHelper","RpGeometryReplaceOldMaterialWithNewMaterial",0x734DE0,&RpGeometryReplaceOldMaterialWithNewMaterial);
 OriginalDependencies499::HookRoute slerp("CQuaternion","Slerp-halftheta",0x59C300,static_cast<void (CQuaternion::*)(const CQuaternion&,const CQuaternion&,float,float,float)>(&CQuaternion::Slerp));
 // _rpMaterialListFindMaterialIndex (0x74E420) tiene ahora su propio hook RH (Dependencies25B): se apaga aqui para que el colaborador del lado original sea el codigo del EXE.
// La entrada de la DLL de un colaborador enganchado se resuelve ANTES de apagar su hook: con el hook apagado RH deja en esa entrada un JMP al EXE y Resolve() devolveria 0x74E420
 // (ModuleOf(findDll) != asi, ready=false, 0 comparaciones). Con el hook encendido Resolve() salta el thunk y da el cuerpo real de la DLL.
 const uintptr_t findDll=Resolve(uintptr_t(&_rpMaterialListFindMaterialIndex));
 OriginalDependencies499::HookRoute findIndexHook("RpWorld","_rpMaterialListFindMaterialIndex",0x74E420,&_rpMaterialListFindMaterialIndex);
 const uintptr_t destroyDll=Resolve(uintptr_t(&RpMaterialDestroy));
 // Los colaboradores deben ser, hoy, el codigo del EXE (sin hook) y los envoltorios de la DLL que llaman a esas direcciones.
 bool collaboratorsOK=ModuleOf(findDll)==asi&&ModuleOf(destroyDll)==asi&&!ReversingProbe::JumpsToDll(0x74E420)&&!ReversingProbe::JumpsToDll(0x74DA20)&&ModuleOf(Resolve(0x74E420))==gameExe&&ModuleOf(Resolve(0x74DA20))==gameExe;
 bool ready=firstTexture.Registered()&&replace.Registered()&&slerp.Registered()&&findIndexHook.Registered()&&collaboratorsOK;
 ready=ready&&ModuleOf(firstTexture.Body())==asi&&ModuleOf(replace.Body())==asi&&ModuleOf(slerp.Body())==asi;
 std::vector<Watch> watches;
 for(uintptr_t at:{uintptr_t(0x734940),uintptr_t(0x734DE0),uintptr_t(0x59C300),uintptr_t(0x74E420),uintptr_t(0x74DA20),findDll,destroyDll})watches.emplace_back(at,16);
 auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 json watched=json::array();for(const auto& w:watches)watched.push_back({{"address",w.address},{"bytes",Hex(w.address,16)}});
 constexpr unsigned total=kFirstTextureCases+kReplaceCases+kSlerpCases;
 Write({{"event","begin"},{"registered_hook",ready},{"functions",3},{"pairs",total},{"cases",{{"GetFirstTexture",kFirstTextureCases},{"Replace",kReplaceCases},{"Slerp",kSlerpCases}}},
  {"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},{"collaborators",{{"find_index_exe",0x74E420},{"destroy_exe",0x74DA20},{"find_index_dll",findDll},{"destroy_dll",destroyDll}}},
  {"watched_entries",watched},{"bodies",{{"GetFirstTexture",firstTexture.Body()},{"Replace",replace.Body()},{"Slerp",slerp.Body()}}},{"asi_module",asi},{"original_module",gameExe},
  {"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR}});
 int pairs{};bool passed=ready;
 auto prepare=[&]{RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);};
 auto finishRun=[&](json& out,const Run& run,bool candidate,bool fpIpRouteOK,bool zeroFip){
  auto fp=run.fp;if(zeroFip)memset(fp.data()+12,0,4);const auto logged=kCompareDeadX87Registers?fp:MaskDeadRegisters(fp);
  out["post_floating_state"]=Hex(uintptr_t(logged.data()),108);out["post_mxcsr"]=run.mxcsr;out["fp_ip_route_ok"]=fpIpRouteOK;
  out["stack_restored"]=run.reg.stack;out["nonvolatile_preserved"]=run.reg.nonvolatile;out["route_ok"]=run.routeOK;
  RestoreFloatingState();out["globals_restored"]=watchesIntact();out["floating_state_restored"]=FloatingStateRestored();(void)candidate;
 };
 auto emitPair=[&](const char* function,unsigned scenario,const json& a,const json& b,const Run& ra,const Run& rb,bool zeroFip,int& failures){
  bool ok=a==b;
  for(const char* k:{"fp_ip_route_ok","stack_restored","nonvolatile_preserved","route_ok","globals_restored","floating_state_restored"})ok&=a[k]==true&&b[k]==true;
  auto fa=ra.fp,fb=rb.fp;if(zeroFip){memset(fa.data()+12,0,4);memset(fb.data()+12,0,4);}
  passed&=ok;failures+=!ok;
  Write({{"event","comparison"},{"function",function},{"case",scenario},{"pair",pairs++},{"original",a},{"candidate",b},{"fp_difference",FpDifference(fa,fb)},
   {"original_residue",{{"eax",ra.reg.eax},{"ecx",ra.reg.ecx},{"edx",ra.reg.edx}}},{"candidate_residue",{{"eax",rb.reg.eax},{"ecx",rb.reg.ecx},{"edx",rb.reg.edx}}},
   {"original_fp_ip",ra.ip},{"candidate_fp_ip",rb.ip},{"equal",ok},{"accepted",ok}});
 };

 // ============================================================ GetFirstTexture (0x734940)
 int failuresFirst{};
 alignas(16) static std::array<uint8,kTexArena+32> texBytes{};
 for(unsigned scenario=0;scenario<kFirstTextureCases&&ready;++scenario){
  const auto fx=FirstTexture(scenario);Run runs[2];json rows[2];
  for(int side=0;side<2;++side){
   const bool candidate=side==1;prepare();
   uint8* base=texBytes.data()+16;memset(texBytes.data(),0xA5,16);memset(base+kTexArena,0xA5,16);BuildFirstTexture(base,fx);
   const uint32 dict=Ptr(base,DictOffset(fx));
   runs[side]=Execute(firstTexture,candidate,0x734940,[&]{return GateCdecl1(0x734940,dict);});
   const long expected=ExpectedFirstTexture(fx);const uint32 eax=runs[side].reg.eax;
   json out={{"input",{{"case",scenario},{"count",fx.count},{"pad",fx.pad},{"order",fx.order},{"junk",fx.junk},{"prev",fx.prev}}},{"base",uintptr_t(base)},{"state",Hex(uintptr_t(texBytes.data()),texBytes.size())},
    {"eax_offset",eax?long(eax)-long(uintptr_t(base)):-1L},{"expected_offset",expected}};
   // Ninguna de las dos rutas ejecuta x87: FIP es el del ultimo fstp de PrimeX87 (en el ASI) y debe coincidir.
   finishRun(out,runs[side],candidate,ModuleOf(runs[side].ip)==asi,false);rows[side]=out;
  }
  emitPair("RwHelper::GetFirstTexture",scenario,rows[0],rows[1],runs[0],runs[1],false,failuresFirst);
 }
 Write({{"event","function_summary"},{"function","RwHelper::GetFirstTexture"},{"cases",int(kFirstTextureCases)},{"failures",failuresFirst},{"state_restored",watchesIntact()}});

 // ============================================================ RpGeometryReplaceOldMaterialWithNewMaterial (0x734DE0)
 int failuresReplace{};
 alignas(16) static std::array<uint8,kRepArena+32> repBytes{};
 for(unsigned scenario=0;scenario<kReplaceCases&&ready;++scenario){
  const auto fx=Replace(scenario);Run runs[2];json rows[2];
  for(int side=0;side<2;++side){
   const bool candidate=side==1;prepare();
   uint8* base=repBytes.data()+16;memset(repBytes.data(),0xA5,16);memset(base+kRepArena,0xA5,16);BuildReplace(base,fx);
   trace.clear();traceBase=uintptr_t(base);
   const uint32 geometry=Ptr(base,kRepGeometry),oldMaterial=Ptr(base,PoolOffset(fx.oldMat)),newMaterial=Ptr(base,PoolOffset(fx.newMat));
   runs[side]=Execute(replace,candidate,0x734DE0,[&]{
    ReversingProbe::EntryPatch findExe(0x74E420,&FindIndexStub),destroyExe(0x74DA20,&DestroyStub),findLib(findDll,&FindIndexStub),destroyLib(destroyDll,&DestroyStub);
    return GateCdecl3(0x734DE0,geometry,oldMaterial,newMaterial);});
   json out={{"input",{{"case",scenario},{"meshes",fx.meshes},{"materials",fx.materials},{"old",fx.oldMat},{"new",fx.newMat},{"absent",fx.absent},{"first_mesh_offset",fx.firstMeshOffset},{"serial",fx.serial},
     {"mesh_mat",std::vector<unsigned>(fx.meshMat,fx.meshMat+8)},{"list_mat",std::vector<unsigned>(fx.listMat,fx.listMat+6)},{"ref_counts",std::vector<unsigned>(fx.refCount,fx.refCount+8)}}},
    {"base",uintptr_t(base)},{"state",Hex(uintptr_t(repBytes.data()),repBytes.size())},{"returned",int(runs[side].reg.eax&0xFF)},{"trace",trace}};
   finishRun(out,runs[side],candidate,ModuleOf(runs[side].ip)==asi,false);rows[side]=out;
  }
  emitPair("RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial",scenario,rows[0],rows[1],runs[0],runs[1],false,failuresReplace);
 }
 Write({{"event","function_summary"},{"function","RwHelper::RpGeometryReplaceOldMaterialWithNewMaterial"},{"cases",int(kReplaceCases)},{"failures",failuresReplace},{"state_restored",watchesIntact()}});

 // ============================================================ CQuaternion::Slerp (0x59C300)
 int failuresSlerp{};
 alignas(16) static std::array<uint8,0x50> quatBytes{};
 for(unsigned scenario=0;scenario<kSlerpCases&&ready;++scenario){
  const auto fx=Slerp(scenario);Run runs[2];json rows[2];
  const bool copyPath=(fx.theta&0x7FFFFFFFu)==0;
  for(int side=0;side<2;++side){
   const bool candidate=side==1;prepare();
   uint8* base=quatBytes.data()+16;memset(quatBytes.data(),0xA5,quatBytes.size());
   memcpy(base,fx.from,16);memcpy(base+16,fx.to,16);memset(base+32,0x6B,16);   // from @0, to @16, destino propio @32
   const uintptr_t target=fx.alias==0?uintptr_t(base+32):fx.alias==1?uintptr_t(base):uintptr_t(base+16);
   runs[side]=Execute(slerp,candidate,0x59C300,[&]{return GateThis5(0x59C300,uint32(target),uint32(uintptr_t(base)),uint32(uintptr_t(base+16)),fx.theta,fx.inv,fx.t);});
   json out={{"input",{{"case",scenario},{"theta",fx.theta},{"t",fx.t},{"inv",fx.inv},{"quat",fx.quat},{"alias",fx.alias},{"from",std::vector<uint32>(fx.from,fx.from+4)},{"to",std::vector<uint32>(fx.to,fx.to+4)}}},
    {"state",Hex(uintptr_t(quatBytes.data()),quatBytes.size())},{"path",copyPath?"copy":"math"}};
   // Ultima instruccion x87: fcomp m32 (rama que copia) o fstp m32 (rama de calculo), en el EXE (original) o en el ASI (candidato).
   const unsigned cls=FpOpcodeClass(runs[side].ip);const uintptr_t where=ModuleOf(runs[side].ip);
   const bool ipOK=cls==(copyPath?1u:2u)&&where==(candidate?asi:gameExe);
   finishRun(out,runs[side],candidate,ipOK,true);rows[side]=out;
  }
  emitPair("CQuaternion::Slerp",scenario,rows[0],rows[1],runs[0],runs[1],true,failuresSlerp);
 }
 Write({{"event","function_summary"},{"function","CQuaternion::Slerp"},{"cases",int(kSlerpCases)},{"failures",failuresSlerp},{"state_restored",watchesIntact()}});

 // Las vigilancias se tomaron con los tres hooks apagados (el constructor de HookRoute los apaga): se comprueban ANTES de Restore(), que vuelve a encender los hooks
 // (entradas 0x734940/0x734DE0/0x59C300 pasan a ser JMP a la DLL, que es su estado inicial correcto). Restore() verifica por su cuenta que esas 5 bytes y el estado
 // Hooked/Locked vuelven a lo inicial; los colaboradores (0x74E420, 0x74DA20 y los envoltorios de la DLL) no cambian con Restore() y se vuelven a comprobar despues.
 const bool entriesIntactBefore=watchesIntact();
 bool stateRestored=entriesIntactBefore;stateRestored&=firstTexture.Restore();stateRestored&=replace.Restore();stateRestored&=slerp.Restore();
 for(size_t i=3;i<watches.size();++i)stateRestored&=watches[i].Intact();
 RestoreFloatingState();stateRestored&=FloatingStateRestored();
 Write({{"event","global_state_final"},{"floating_state",Hex(uintptr_t(savedFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
 RestoreFloatingState();savedFPActive=false;
}
}
