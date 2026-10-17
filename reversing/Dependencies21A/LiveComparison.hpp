#pragma once
#include <fstream>
#include "World.h"
#include "CutsceneMgr.h"
#include "Entity/Ped/PlayerPed.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "FixtureTables.hpp"
namespace Dependencies21ALive {
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t address) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(address),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t address) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(address)==0xE9;++i)address+=5+*reinterpret_cast<int32*>(address+1);return address;}
inline bool done{};
inline std::ofstream log;
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
// 32 formatos x 2 modos x 64 variantes. Ver fixture_oracle.py (fuente unica de las tablas de FixtureTables.hpp).
inline constexpr unsigned kCases=4096;
// true: la igualdad EXE/DLL exige los 108 bytes del FNSAVE (los dos lados ejecutan el mismo codigo x87 del CRT del EXE, asi que FIP/FDP tambien coinciden).
inline constexpr bool kCompareDeadX87Registers=true;
struct Fn { const char* cls;const char* name;uintptr_t address;int reserved,unused,count; };
inline constexpr Fn fns[]{
    {"CRT","CrtPrintf",0x821982,0,-1,int(kCases)}
};
inline constexpr uintptr_t kStdout=0x8E2C50, kWriteEntry=0x82B867, kGetPtd=0x827B3D, kLockTable=0x8E31C0, kLockIndex=0x11, kTempBufferCount=0xC9AC4C, kTempBufferPointers=0xC9AC6C;
inline constexpr uint32 kStreamBufferBytes=4096;

// Salida de _write doblada (solo durante la llamada, en ambos lados): registra lo que el CRT volcaria al descriptor 1 y devuelve la cuenta pedida. SIN CREDITO como colaborador.
inline std::string captured;
inline int writeCalls{};
inline int __cdecl WriteStub(int fd,const void* buffer,unsigned count){++writeCalls;if(fd==1&&buffer&&count)captured.append(reinterpret_cast<const char*>(buffer),count);return int(count);}

struct Result {uint32 eax,ecx,edx;bool stack,nonvolatile;};
// Llama a entry con `n` palabras apiladas (la primera de `args` queda en la cima), como un llamador cdecl con varargs del EXE.
__declspec(noinline) inline Result Gate(uintptr_t entry,const uint32* args,uint32 n){uint32 before{},after{},a{},c{},d{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {mov ecx,n} __asm {mov edx,args}
 push_loop: __asm {test ecx,ecx} __asm {jz push_end} __asm {push dword ptr [edx+ecx*4-4]} __asm {dec ecx} __asm {jmp push_loop}
 push_end: __asm {mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,0C0DE5921h} __asm {call entry}
 __asm {mov a,eax} __asm {mov c,ecx} __asm {mov d,edx} __asm {mov ecx,n} __asm {lea esp,[esp+ecx*4]}
 __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {a,c,d,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}
inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
inline std::string HexOf(const std::string& s){return Hex(uintptr_t(s.data()),s.size());}
struct Block {uintptr_t address;std::vector<uint8> saved;Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}bool Restored()const{return !memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size());}std::string Observe()const{return Hex(address,saved.size());}};
inline uint32 ReadInt(uintptr_t a){uint32 v{};memcpy(&v,reinterpret_cast<void*>(a),4);return v;}
inline void PutInt(uintptr_t a,uint32 v){memcpy(reinterpret_cast<void*>(a),&v,4);}
inline uintptr_t GetPtd(){uintptr_t value{};const auto fn=kGetPtd;__asm {call fn} __asm {mov value,eax}return value;}
// Entrada de la tabla de descriptores (ioinfo) del fd 1: byte de banderas _osfile en +4 (0x40 = FDEV, tty). Misma cuenta que 0x8334B2 (_isatty).
inline uintptr_t Fd1Flags(){const uintptr_t block=ReadInt(0xC9C300+4*(1>>5));return block+(1&0x1F)*0x24+4;}
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
 json out=json::array();
 const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw",4,2},{"ftw",8,2},{"fip",12,4},{"fcs_fop",16,4},{"fdp",20,4},{"fds",24,4}};
 for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
 for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
 return out;
}
// Memoria que sostiene los argumentos puntero de cada caso (direcciones estables, mismas en ambos lados).
struct ArgStorage {
 std::array<std::array<char,512>,16> narrow{};std::array<std::array<wchar_t,256>,8> wide{};
 ArgStorage(){for(size_t i=0;i<std::size(kStrings);++i){memset(narrow[i].data(),0,narrow[i].size());if(!kStrings[i].null)memcpy(narrow[i].data(),kStrings[i].text,kStrings[i].size);}
  for(size_t i=0;i<std::size(kWides);++i){wide[i].fill(0);if(!kWides[i].null)wcscpy_s(wide[i].data(),wide[i].size(),kWides[i].text);}}
};
struct Fixture {unsigned fmt,mode,variant;std::vector<uint32> words;json logical;};
inline Fixture MakeFixture(unsigned scenario,ArgStorage& storage){
 Fixture f{scenario%32,(scenario/32)%2,scenario/64,{},json::array()};const char* kinds=kFormats[f.fmt].kinds;
 for(unsigned k=0;kinds[k];++k){const char kind=kinds[k];
  if(kind=='i'||kind=='p'){const auto x=kInts[(f.variant+7*k)%std::size(kInts)];f.words.push_back(x);f.logical.push_back({"i",x});}
  else if(kind=='c'){const auto x=kChars[(f.variant+3*k)%std::size(kChars)];f.words.push_back(x);f.logical.push_back({"c",x});}
  else if(kind=='*'){const auto x=kStars[(f.variant+5*k)%std::size(kStars)];f.words.push_back(x);f.logical.push_back({"*",x});}
  else if(kind=='q'){const auto x=kQwords[(f.variant+5*k)%std::size(kQwords)];f.words.push_back(uint32(x));f.words.push_back(uint32(x>>32));f.logical.push_back({"q",x});}
  else if(kind=='d'){const auto x=kDoubles[(f.variant+11*k)%std::size(kDoubles)];f.words.push_back(uint32(x));f.words.push_back(uint32(x>>32));f.logical.push_back({"d",x});}
  else if(kind=='s'){const auto i=(f.variant+3*k)%std::size(kStrings);f.words.push_back(kStrings[i].null?0:uint32(uintptr_t(storage.narrow[i].data())));f.logical.push_back({"s",i});}
  else if(kind=='w'){const auto i=(f.variant+3*k)%std::size(kWides);f.words.push_back(kWides[i].null?0:uint32(uintptr_t(storage.wide[i].data())));f.logical.push_back({"w",i});}
 }
 return f;
}
inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies21a-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies21a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies21a");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 static ArgStorage storage;
 alignas(16) static std::array<char,kStreamBufferBytes+64> ownBuffer{};   // buffer propio del modo 0 (con 32 bytes de guarda a cada lado)
 char* const own=ownBuffer.data()+32;
 // Sin HookRoute::Invoke los dos lados serian el mismo: el hook RH de 0x821982 se enciende/apaga por caso (destino vivo registrado).
 OriginalDependencies499::HookRoute route("CRT","CrtPrintf",0x821982,OriginalDependencies499::HookRoute::RegisteredTarget{});
 const uintptr_t body=route.Body();
 bool ready=route.Registered()&&body&&ModuleOf(body)==ModuleOf(uintptr_t(&ModuleMarker));
 auto prepare=[&](const Fixture& fx,uintptr_t ptdAddress){
  memset(ownBuffer.data(),0xA5,ownBuffer.size());memset(own,0,kStreamBufferBytes);
  // Preparacion del flujo (mismos campos que deja el CRT del EXE; _file = 1):
  //  modo 0: flujo con buffer propio (_flag = _IOWRT|_IOYOURBUF 0x102): _stbuf devuelve 0 y _ftbuf no hace nada.
  //  modo 1: sin buffer + descriptor tty: _stbuf asigna su buffer temporal (0x1000) y _ftbuf lo vuelca con _write (doblado) y lo desarma.
  PutInt(kStdout+0,fx.mode==0?uint32(uintptr_t(own)):0);PutInt(kStdout+4,fx.mode==0?kStreamBufferBytes:0);PutInt(kStdout+8,fx.mode==0?uint32(uintptr_t(own)):0);
  PutInt(kStdout+0xC,fx.mode==0?0x102u:0x2u);PutInt(kStdout+0x10,1);PutInt(kStdout+0x14,0);PutInt(kStdout+0x18,fx.mode==0?kStreamBufferBytes:0);PutInt(kStdout+0x1C,0);
  *reinterpret_cast<uint8*>(Fd1Flags())|=0x40;if(ptdAddress)PutInt(ptdAddress+8,0);
 };
 // Calentamiento (fuera de la comparacion, con el hook apagado): crea la seccion critica del cerrojo 0x11 y el buffer temporal de _stbuf (modo 1), que quedan fijos en el proceso.
 const uintptr_t ptd=GetPtd();
 if(ready&&ptd){const Block warmFile(kStdout,0x20),warmFlags(Fd1Flags(),1);ArgStorage& st=storage;const auto fx=MakeFixture(32,st);prepare(fx,ptd);
  std::vector<uint32> stack{uint32(uintptr_t(kFormats[fx.fmt].text))};
  {ReversingProbe::EntryPatch writePatch(kWriteEntry,WriteStub);route.Invoke(false,[&]{return Gate(0x821982,stack.data(),uint32(stack.size()));});}
  RestoreFloatingState();warmFile.Restore();warmFlags.Restore();captured.clear();writeCalls=0;}
 std::vector<Block> blocks;
 blocks.emplace_back(kStdout,0x20);                                                     // FILE de stdout del EXE
 blocks.emplace_back(uintptr_t(ownBuffer.data()),ownBuffer.size());                      // buffer propio + guardas
 blocks.emplace_back(kTempBufferCount,4);blocks.emplace_back(kTempBufferPointers,8);    // contador y punteros del buffer temporal de _stbuf (stdout, stderr)
 if(ready&&ReadInt(kLockTable+8*kLockIndex))blocks.emplace_back(ReadInt(kLockTable+8*kLockIndex),sizeof(CRITICAL_SECTION));  // estado del cerrojo del flujo
 blocks.emplace_back(kLockTable+8*kLockIndex,8);
 blocks.emplace_back(Fd1Flags(),1);                                                      // banderas _osfile del descriptor 1 (se fuerza FDEV en el modo 1)
 if(ptd)blocks.emplace_back(ptd+8,4);                       // errno del hilo (_getptd()->_terrno)
 ready&=ptd!=0&&ReadInt(kLockTable+8*kLockIndex)!=0;
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();captured.clear();writeCalls=0;};
 auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
 json layout=json::array(),initial=json::array();for(const auto& b:blocks){layout.push_back({{"address",b.address},{"bytes",b.saved.size()}});initial.push_back(b.Observe());}
 Write({{"event","begin"},{"registered_hook",ready},{"functions",1},{"pairs",kCases},{"cases",kCases},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},{"stubbed_collaborators",json::array({"_write 0x82B867 (doblado solo durante la llamada, en ambos lados, sin credito)"})},{"snapshot_layout",layout},{"initial_state",initial},{"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},{"body",body},{"asi_module",ModuleOf(uintptr_t(&ModuleMarker))},{"original_module",uintptr_t(GetModuleHandleA(nullptr))}});
 int pairs{},failures{};bool passed=ready;
 for(unsigned scenario=0;scenario<kCases&&ready;++scenario){
  std::array<std::array<uint8,108>,2> fpRaw{};
  auto invoke=[&](bool candidate){restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
   const auto fx=MakeFixture(scenario,storage);
   prepare(fx,ptd);
   alignas(16)std::array<uint8,108> fp{};uint32 mx{};bool routeOK{};
   std::vector<uint32> stack;stack.push_back(uint32(uintptr_t(kFormats[fx.fmt].text)));stack.insert(stack.end(),fx.words.begin(),fx.words.end());
   Result result{};
   {ReversingProbe::EntryPatch writePatch(kWriteEntry,WriteStub);
    result=route.Invoke(candidate,[&]{routeOK=candidate?ReversingProbe::JumpsToDll(0x821982)&&ModuleOf(Resolve(0x821982))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(0x821982);const auto value=Gate(0x821982,stack.data(),uint32(stack.size()));SaveFP(fp.data());mx=ReadMXCSR();return value;});}
   fpRaw[candidate?1:0]=fp;
   // Texto producido: modo 0 = bytes entre _base y _ptr del flujo (no se vuelca nada); modo 1 = lo capturado en _write.
   const uint32 base=ReadInt(kStdout+8),ptr=ReadInt(kStdout+0);std::string text;
   if(fx.mode==0&&ptr>=base&&ptr-base<=kStreamBufferBytes)text.assign(reinterpret_cast<const char*>(base),ptr-base);else text=captured;
   const bool guardsOK=[&]{for(int i=0;i<32;++i)if(uint8(ownBuffer[i])!=0xA5||uint8(ownBuffer[ownBuffer.size()-1-i])!=0xA5)return false;return true;}();
   json state=json::array();for(const auto& b:blocks)state.push_back(b.Observe());
   // ECX y EDX al volver: ningun llamador del EXE los lee (29 llamadores desensamblados, ver classification); se registran sin exigir igualdad entre lados.
   json out={{"format",kFormats[fx.fmt].text},{"mode",fx.mode},{"variant",fx.variant},{"words",fx.words.size()},{"logical",fx.logical},{"state",state},{"eax",result.eax},{"text_hex",HexOf(text)},{"write_calls",writeCalls},
    {"post_floating_state",Hex(uintptr_t(fp.data()),108)},{"post_mxcsr",mx},{"stack_restored",result.stack},{"nonvolatile_preserved",result.nonvolatile},{"route_ok",routeOK},{"guards_ok",guardsOK}};
   out["residue_ecx_edx_informative"]={result.ecx,result.edx};
   restore();out["globals_restored"]=restored();out["floating_state_restored"]=FloatingStateRestored();return out;
  };
  auto a=invoke(false),b=invoke(true);
  const json ra=a["residue_ecx_edx_informative"],rb=b["residue_ecx_edx_informative"];a.erase("residue_ecx_edx_informative");b.erase("residue_ecx_edx_informative");
  bool ok=a==b;const auto fpDifference=FpDifference(fpRaw[0],fpRaw[1]);for(const char* k:{"stack_restored","nonvolatile_preserved","route_ok","guards_ok","globals_restored","floating_state_restored"})ok&=a[k]==true&&b[k]==true;
  passed&=ok;failures+=!ok;Write({{"event","comparison"},{"function","CRT::CrtPrintf"},{"case",scenario},{"pair",pairs++},{"original",a},{"candidate",b},{"residue_original",ra},{"residue_candidate",rb},{"fp_difference",fpDifference},{"equal",ok},{"accepted",ok}});
 }
 restore();bool stateRestored=restored();stateRestored&=route.Restore();
 json final=json::array();for(const auto& b:blocks)final.push_back(b.Observe());alignas(16)std::array<uint8,108> finalFP{};SaveFP(finalFP.data());
 Write({{"event","function_summary"},{"function","CRT::CrtPrintf"},{"cases",pairs},{"failures",failures},{"state_restored",stateRestored}});
 Write({{"event","global_state_final"},{"state",final},{"floating_state",Hex(uintptr_t(finalFP.data()),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()}});restore();savedFPActive=false;
}
}
