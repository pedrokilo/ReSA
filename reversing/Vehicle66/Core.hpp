#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstdio>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include "Fixtures.hpp"
// Nucleo del banco Vehicle66 (lote 66). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute. Plantilla: reversing/SmallHelpers64A (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).
//
// Banco par a par de CINCO metodos de CVehicle: RemoveVehicleUpgrade (0x6DF930), AddUpgrade (0x6DFA20), AddVehicleUpgrade (0x6E3290), PossiblyDropFreeFallBombForPlayer (0x6E07E0)
// y ProcessSirenAndHorn (0x6E0950). Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con SOLO ese hook encendido.
// Colaboradores sustituidos por falsos identicos en ambos lados (parche en la entrada del EXE y, si la DLL tiene hook, en el cuerpo de la DLL), por funcion:
//   RemoveVehicleUpgrade: ClearVehicleUpgradeFlags (0x6D3210), FindOtherUpgrade (0x4C74D0), RemoveReplacementUpgrade (0x6D39E0), RpClumpForAllAtomics (0x749B70)
//   AddUpgrade: GetFrameFromId (0x4C53C0), RpClumpForAllAtomics, CreateUpgradeAtomic (0x6D3510), RwFrameForAllObjects (0x7F1200), RwFrameForAllChildren (0x7F0DC0), SetupClumpAfterVehicleUpgrade (0x6FDFE0)
//   AddVehicleUpgrade: SetVehicleUpgradeFlags (0x6D30E0), FindOtherUpgrade, GetReplacementUpgrade (0x6D3A50), AddReplacementUpgrade (0x6D3830), GetUpgrade (0x6D3650), AddUpgrade (0x6DFA20)
//   PossiblyDropFreeFallBombForPlayer: GetPlaneOrdnanceRateOfFire (0x6D4590), GetPlaneOrdnancePosition (0x6D46E0), CProjectileInfo::AddProjectile (0x737C80), CPad::StartShake (0x53F920)
//   ProcessSirenAndHorn: UsesSiren (0x6D8470), CPad::GetHorn (0x53FEE0)
// CPed::IsPlayer (0x5DF8F0) y CPad::GetPad (0x53FB70) se ejecutan de verdad en ambos lados. Los model infos de mejoras y del vehiculo falsos se instalan en ms_modelInfoPtrs[15..19].
// Registros de salida: AddVehicleUpgrade devuelve la mejora sustituida en EAX y su llamador 0x4985DA la guarda en la global 0xA43C78: se EXIGE EAX. Los demas (RemoveVehicleUpgrade,
// AddUpgrade, PossiblyDrop..., ProcessSirenAndHorn): ningun llamador del EXE lee EAX/ECX/EDX (residue_scan.py): solo registrados.

namespace V66Live {
using namespace V66Fix;
using HookRoute = OriginalDependencies499::HookRoute;
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
inline void PrimeX87() {
    static const double stale[8]{1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};
    const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// Se compara lo observable de la x87 (FCW completa, TOP, etiquetas y ST validos); los registros vacios y las banderas de FSW no.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Aqui NO: lo que se exige (EAX/EDX de GetMillisecondTime, EAX de los callbacks) va en `observed`; el resto solo se registra.
inline constexpr bool kCompareVolatileRegisters=false;

inline std::string HexBytes(const void* p,size_t n) {constexpr char d[]="0123456789abcdef";const auto* b=static_cast<const u8*>(p);std::string o;o.reserve(n*2);for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];}return o;}
inline uint32 FpWord(const std::array<uint8,108>& fp,size_t at){uint32 v{};memcpy(&v,fp.data()+at,4);return v;}
inline bool FpRegisterIsDead(const std::array<uint8,108>& fp,unsigned st){const uint32 ftw=FpWord(fp,8)&0xFFFF,top=(FpWord(fp,4)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline std::array<uint8,108> MaskDeadRegisters(std::array<uint8,108> fp){for(unsigned st=0;st<8;++st)if(FpRegisterIsDead(fp,st))memset(fp.data()+28+10*st,0,10);return fp;}
inline std::array<uint8,108> NormalizeFp(std::array<uint8,108> fp){
    uint16 fsw{};memcpy(&fsw,fp.data()+4,2);fsw&=0x3800;memcpy(fp.data()+4,&fsw,2);   // solo TOP
    memset(fp.data()+2,0,2);memset(fp.data()+6,0,2);memset(fp.data()+10,0,2);          // mitades reservadas
    memset(fp.data()+12,0,16);                                                         // FIP, FCS/FOP, FDP, FDS
    return MaskDeadRegisters(fp);
}
inline json FpDifference(const std::array<uint8,108>& a,const std::array<uint8,108>& b){
    json out=json::array();
    const struct {const char* name;size_t at,size;} fields[]{{"fcw",0,2},{"fsw_top",4,2},{"ftw",8,2}};
    for(const auto& f:fields)if(memcmp(a.data()+f.at,b.data()+f.at,f.size))out.push_back(f.name);
    for(unsigned st=0;st<8;++st)if(memcmp(a.data()+28+10*st,b.data()+28+10*st,10))out.push_back("st"+std::to_string(st)+(FpRegisterIsDead(a,st)?"(dead)":"(valid)"));
    return out;
}
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

// ------------------------------------------------------------------------------------------------ llamada con medicion de ABI
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; empuja los argumentos (args[0] arriba), llama y captura registros y EFLAGS justo tras el `call`;
// popBytes = bytes que limpia el LLAMADOR (cdecl: 4*n; `ret n` del callee: 0).
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{},sehBefore{},sehAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore,ebp
        mov eax,dword ptr fs:[0]
        mov sehBefore,eax
        mov edx,stackSeed
        xor esi,esi
        mov eax,esp
        mov ecx,64
    seedloop:
        sub eax,4
        add esi,9E3779B1h
        mov edi,edx
        xor edi,esi
        mov [eax],edi
        dec ecx
        jnz seedloop
        mov espBefore,esp
        mov ebx,13579BDFh
        mov esi,2468ACE0h
        mov edi,10293847h
        mov ecx,argCount
        mov edx,argsPtr
    pushloop:
        test ecx,ecx
        jz pushed
        push dword ptr [edx+ecx*4-4]
        dec ecx
        jmp pushloop
    pushed:
        mov eax,seedEax
        mov ecx,seedEcx
        mov edx,seedEdx
        call entryAddress
        mov outEax,eax
        mov outEcx,ecx
        mov outEdx,edx
        pushfd
        pop eax
        mov outFlags,eax
        mov eax,dword ptr fs:[0]
        mov sehAfter,eax
        add esp,popBytes
        mov espAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        pop edi
        pop esi
        pop ebx
    }
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter,sehBefore==sehAfter};
}

// ------------------------------------------------------------------------------------------------ colaboradores, parches y rutas
struct Pair {uintptr_t exe,dll;bool hooked{true};};   // hooked=false: sin RH_ScopedInstall activo (la entrada del EXE sigue siendo codigo original); dll=0: la DLL llama a la direccion del EXE
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
struct StubSpec {Pair pair;void* fake;};
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    Patches(){p.reserve(32);}
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    void Reset(){while(!p.empty())p.pop_back();}   // orden inverso al de instalacion
    ~Patches(){Reset();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,kFns> route;              // una por funcion
    std::vector<std::unique_ptr<HookRoute>> collab;                 // rutas de los colaboradores con hook (apagados durante el banco)
    std::array<std::vector<StubSpec>,kFns> stubs;                   // colaboradores sustituidos por funcion
    std::vector<Watch> watches;                                     // entradas del EXE de todos los colaboradores: deben seguir siendo lo que eran al empezar
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los colaboradores sustituidos tienen su hook apagado: su entrada del EXE debe seguir
// siendo codigo original en ambos lados; los falsos se instalan DESPUES de comprobar rutas y colaboradores y solo mientras dura la llamada.
// Los parches de falsos se quitan (Reset) ANTES de leer el estado final: `after` corre ya sin parches.
template<class Fn,class After> Side Execute(Env& env,unsigned fnIdx,uintptr_t entry,bool candidate,Fn gate,After after) {
    Side side{};
    HookRoute& route=*env.route[fnIdx];
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& s:env.stubs[fnIdx]) {
            const auto& pr=s.pair;
            collab&=!ReversingProbe::JumpsToDll(pr.exe)&&ModuleOf(Resolve(pr.exe))==env.game&&(pr.dll==0||ModuleOf(pr.dll)==env.asi);
        }
        side.collabOK=collab;
        {
            Patches patches;
            for(const auto& s:env.stubs[fnIdx])patches.Add(s.pair,s.fake);
            side.reg=gate(entry);
            patches.Reset();
        }
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        after();
        return 0;
    };
    route.Invoke(candidate,call);
    return side;
}

inline void PrepareFloatingState() {
    RestoreFloatingState();PrimeX87();
    _controlfp(_PC_24|_RC_NEAR|_MCW_EM,_MCW_PC|_MCW_RC|_MCW_EM);   // 24 bits, al mas cercano, todas las excepciones enmascaradas (estado del juego)
}
inline json CommonRow(const Side& side,const std::array<uint8,108>& fp) {
    return {{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",side.mxcsr&0xFFC0u},{"stack_restored",side.reg.stack},
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"seh_restored",side.reg.seh},{"route_ok",side.routeOK},{"collaborators_dll",side.collabOK}};
}
inline json Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    return row;
}

// ================================================================================================ arena, falsos, fixtures y casos
inline constexpr const char* kFn[kFns]{"RemoveVehicleUpgrade","AddUpgrade","AddVehicleUpgrade","PossiblyDropFreeFallBombForPlayer","ProcessSirenAndHorn"};
inline constexpr uintptr_t kEntry[kFns]{0x6DF930,0x6DFA20,0x6E3290,0x6E07E0,0x6E0950};
// colaboradores (direcciones del EXE)
inline constexpr uintptr_t kClear=0x6D3210,kFindOther=0x4C74D0,kRemoveRepl=0x6D39E0,kForAllAtomics=0x749B70,kGetFrame=0x4C53C0,kCreateAtomic=0x6D3510,kFrameObjects=0x7F1200,kFrameChildren=0x7F0DC0,
    kPlate=0x6FDFE0,kSetFlags=0x6D30E0,kGetRepl=0x6D3A50,kAddRepl=0x6D3830,kGetUpgrade=0x6D3650,kRate=0x6D4590,kOrdPos=0x6D46E0,kAddProjectile=0x737C80,kStartShake=0x53F920,
    kUsesSiren=0x6D8470,kGetHorn=0x53FEE0;

alignas(16) inline std::array<u32,kArenaWords> Arena{};
alignas(16) inline std::array<u32,kArenaWords> ArenaInit{};
inline u32 AB(){return u32(uintptr_t(Arena.data()));}
inline u32 A(u32 off){return AB()+off;}
inline u64 Lab(u32 v){return (v>=AB()&&v<AB()+kArenaSize)?kLabel+(v-AB()):u64(v);}
inline u64 Lab(const void* p){return Lab(u32(uintptr_t(p)));}
inline u32& W(u32 off){return Arena[off/4];}
inline u8* Bytes(){return reinterpret_cast<u8*>(Arena.data());}
inline void S8(u32 off,u32 v){Bytes()[off]=u8(v);}
inline void S16(u32 off,u32 v){const u16 w=u16(v);memcpy(Bytes()+off,&w,2);}
inline u32 G16(u32 off){u16 w;memcpy(&w,Bytes()+off,2);return w;}

// ---- eventos de los falsos (nada aqui asigna memoria: se usan dentro de la ventana de parches)
// 1 ClearVehicleUpgradeFlags(this,idx,carMod); 2 FindOtherUpgrade(lista,idx); 3 RemoveReplacementUpgrade(this,nodo); 4 RpClumpForAllAtomics(clump,cb,data); 5 GetFrameFromId(clump,id);
// 6 CreateUpgradeAtomic(this,info,desc,marco,danado); 7 RwFrameForAllObjects(marco,cb,data); 8 RwFrameForAllChildren(marco,cb,data); 9 SetupClumpAfterVehicleUpgrade(clump,material,tipo);
// 10 SetVehicleUpgradeFlags(this,idx,mod,*salida); 11 GetReplacementUpgrade(this,nodo); 12 AddReplacementUpgrade(this,modelo,nodo); 13 GetUpgrade(this,id); 14 AddUpgrade(this,modelo,indice);
// 15 GetPlaneOrdnanceRateOfFire(this,tipo); 16 GetPlaneOrdnancePosition(this,tipo); 17 AddProjectile(creador,arma,x,y,z,fuerza,direccion,victima); 18 StartShake(pad,tiempo,frecuencia,arg);
// 19 UsesSiren(this); 20 GetHorn(pad)
struct Ev {u32 kind{};std::array<u64,kEvArgs> a{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline void Push(u32 kind,u64 a0=0,u64 a1=0,u64 a2=0,u64 a3=0,u64 a4=0,u64 a5=0,u64 a6=0,u64 a7=0){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{kind,{a0,a1,a2,a3,a4,a5,a6,a7}};++nev;}
inline Spec cur{};
inline unsigned createCalls{};
inline uintptr_t dllRemoveCb{},dllVisObj{},dllVisFrame{};

// Identificador normalizado de un callback: el del EXE y el de la DLL dan el mismo (1 RemoveUpgradeCB, 2 SetVehicleAtomicVisibilityCB (objeto), 3 (marco), 99 otro)
inline u32 CbId(void* cb){
    const uintptr_t a=uintptr_t(cb);
    if(a==0x6D3300||(dllRemoveCb&&Body(a)==Body(dllRemoveCb)))return 1;
    if(a==0x6D2690||(dllVisObj&&Body(a)==Body(dllVisObj)))return 2;
    if(a==0x6D26D0||(dllVisFrame&&Body(a)==Body(dllVisFrame)))return 3;
    return 99;
}
inline u32 __fastcall FkClear(void* self,void*,u32 a,u32 b){Push(1,Lab(self),a,b);return cur.v[kRmClear];}
inline u32 __fastcall FkFindOther(void* self,void*,u32 a){Push(2,Lab(self),a);return cur.v[cur.fn==kRemove?kRmLinked:kAvLinked];}
inline void __fastcall FkRemoveRepl(void* self,void*,u32 node){Push(3,Lab(self),node);}
inline void* __cdecl FkForAll(void* clump,void* cb,void* data){Push(4,Lab(clump),CbId(cb),Lab(data));return clump;}
inline u32 __cdecl FkGetFrame(void* clump,u32 id){Push(5,Lab(clump),id);return cur.v[kAuFrame];}
inline u32 __fastcall FkCreateAtomic(void* self,void*,void* info,void* desc,void* frame,u32 damaged){
    const unsigned k=createCalls++;
    Push(6,Lab(self),Lab(info),Lab(desc),Lab(frame),damaged&0xFFu);
    return cur.v[k==0?kAuRet1:kAuRet2];
}
inline void* __cdecl FkFrameObjects(void* frame,void* cb,void* data){Push(7,Lab(frame),CbId(cb),Lab(data));return frame;}
inline void* __cdecl FkFrameChildren(void* frame,void* cb,void* data){Push(8,Lab(frame),CbId(cb),Lab(data));return frame;}
inline u32 __cdecl FkPlate(void* clump,void* material,u32 type){Push(9,Lab(clump),Lab(material),type);return 0;}
inline u32 __fastcall FkSetFlags(void* self,void*,u32 a,u32 b,u32* out){
    Push(10,Lab(self),a,b,*out);
    if(cur.v[kAvSetWrites])*out=cur.v[kAvSetValue];
    return cur.v[kAvSetRet];
}
inline u32 __fastcall FkGetRepl(void* self,void*,u32 node){Push(11,Lab(self),node);return cur.v[kAvGetRepl];}
inline void __fastcall FkAddRepl(void* self,void*,u32 model,u32 node){Push(12,Lab(self),model,node);}
inline u32 __fastcall FkGetUpgrade(void* self,void*,u32 id){Push(13,Lab(self),id);return cur.v[kAvGetUp];}
inline void __fastcall FkAddUpgrade(void* self,void*,u32 model,u32 idx){Push(14,Lab(self),model,idx);}
inline u32 __fastcall FkRate(void* self,void*,u32 type){Push(15,Lab(self),type);return cur.v[kDrRate];}
inline void* __fastcall FkOrdPos(void* self,void*,u32* out,u32 type){Push(16,Lab(self),type);out[0]=cur.v[kDrOrd];out[1]=cur.v[kDrOrd+1];out[2]=cur.v[kDrOrd+2];return out;}
inline u32 __cdecl FkProjectile(void* creator,u32 weapon,u32 x,u32 y,u32 z,u32 force,void* dir,void* victim){Push(17,Lab(creator),weapon,x,y,z,force,Lab(dir),Lab(victim));return cur.v[kDrProj]&0xFFu;}
inline void __fastcall FkShake(void* self,void*,u32 time,u32 freq,u32 arg){Push(18,u32(uintptr_t(self))-kPads0,time,freq,arg);}
inline u32 __fastcall FkUsesSiren(void* self,void*){Push(19,Lab(self));return cur.v[kSiSiren];}
inline u32 __fastcall FkGetHorn(void* self,void*){Push(20,u32(uintptr_t(self))-kPads0);return cur.v[kSiHorn];}

inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){json e=json::array({evs[i].kind});for(u64 x:evs[i].a)e.push_back(x);out.push_back(e);}
    return out;
}

// Imagen inicial de la arena de un caso (relleno + campos de la funcion). Debe coincidir con lo que modela fixture_oracle.py.
inline void BuildArena(const Spec& s,unsigned n) {
    for(u32 i=0;i<kArenaWords;++i)Arena[i]=FillWord(s.fn,n,i);
    // comun: el vehiculo falso (modelo kVehModelSlot), su model info, su estructura y su manejabilidad
    S8(oClump,2);   // RwObject.type = rpCLUMP
    S16(oVeh+kVModel,kVehModelSlot);
    W(oVeh+kVHandling)=A(oHnd);
    W(oVMi+kMiStruct)=A(oVSt);
    const u32 vehFlagBase=oVeh;
    (void)vehFlagBase;
    switch(s.fn) {
    case kRemove:
        for(u32 i=0;i<4;++i)S16(oU0+0x40*i+kMiFlags,s.v[kRmFlags+i]);
        for(u32 i=0;i<15;++i)S16(oVeh+kVUpgrades+2*i,s.v[kRmSlots+i]);
        W(oVeh+kVWheelScale)=s.v[kRmScale];
        W(oVeh+kVClump)=s.v[kRmClump]?A(oClump):0u;
        break;
    case kAddUp:
        W(oVMi+kMiPlateMat)=s.v[kAuPlateMat];
        S8(oVMi+kMiPlateType,s.v[kAuPlateType]);
        for(u32 i=0;i<15;++i)W(oVSt+kStUpgrades+0x20*i+kDescParent)=s.v[kAuParents+i];
        W(oVeh+kVClump)=s.v[kAuClump]?A(oClump):0u;
        break;
    case kAddVeh:
        for(u32 i=0;i<4;++i)S16(oU0+0x40*i+kMiFlags,s.v[kAvFlags+i]);
        W(oVMi+kMiWheelFront)=s.v[kAvWheelFront];
        W(oVeh+kVSubType)=s.v[kAvSubType];
        W(oHnd+kHndFlags)=s.v[kAvHnd];
        W(oVeh+kVFlags38C)=s.v[kAvFlags38C];
        W(oVeh+kVWheelScale)=s.v[kAvScale];
        for(u32 i=0;i<15;++i)S16(oVeh+kVUpgrades+2*i,s.v[kAvSlots+i]);
        break;
    case kDrop:
        W(oVeh+kVMatrix)=s.v[kDrHasMat]?A(oMat):0u;
        for(u32 i=0;i<3;++i){W(oMat+kMatPos+4*i)=s.v[kDrMatPos+i];W(oVeh+kVPos+4*i)=s.v[kDrPlPos+i];}
        W(oVeh+kVTime1)=s.v[kDrT1];
        W(oVeh+kVTime2)=s.v[kDrT2];
        S8(oVeh+kVOrdn,s.v[kDrByte]);
        W(oVeh+kVDriver)=s.v[kDrDriver]?A(oPed):0u;
        W(oPed+kPedType)=s.v[kDrPedType];
        break;
    default:
        S16(oVeh+kVAlarm,s.v[kSiAlarm]);
        S8(oVeh+kVStatus,s.v[kSiStatus]);
        S8(oVeh+kVFlags42D,s.v[kSiFlags]);
        W(oVeh+kVHorn)=s.v[kSiCounter];
        break;
    }
    ArenaInit=Arena;
}
inline u32 HornByte(const Spec& s,unsigned n,u32 i){return i<5?(s.v[kSiHist+i]&0xFFu):(i==5?(s.v[kSiIndex]&0xFFu):HornFill(n,i));}

// Globales que el banco escribe durante un caso: ms_modelInfoPtrs[15..19], MI_OFFROAD_WHEEL (palabra), la region de historial de bocina del pad 0 y CTimer::m_snTimeInMilliseconds.
struct Globals {
    std::array<u32,5> slots{};u16 offroad{};std::array<u8,kPadHistSize> hist{};u32 time{};
    static Globals Capture(){Globals g;for(u32 i=0;i<5;++i)g.slots[i]=reinterpret_cast<u32*>(kModelInfoPtrs)[kVehModelSlot+i];g.offroad=*reinterpret_cast<u16*>(kOffroadWheel);memcpy(g.hist.data(),reinterpret_cast<void*>(kPadHist),kPadHistSize);g.time=*reinterpret_cast<u32*>(kTimeMs);return g;}
    void Apply()const{for(u32 i=0;i<5;++i)reinterpret_cast<u32*>(kModelInfoPtrs)[kVehModelSlot+i]=slots[i];*reinterpret_cast<u16*>(kOffroadWheel)=offroad;memcpy(reinterpret_cast<void*>(kPadHist),hist.data(),kPadHistSize);*reinterpret_cast<u32*>(kTimeMs)=time;}
    bool operator==(const Globals& o)const{return slots==o.slots&&offroad==o.offroad&&hist==o.hist&&time==o.time;}
};
inline Globals CaseGlobals(const Spec& s,unsigned n,const Globals& base){
    Globals g=base;
    g.slots[0]=A(oVMi);for(u32 i=0;i<4;++i)g.slots[1+i]=A(oU0+0x40*i);
    if(s.fn==kAddVeh)g.offroad=u16(s.v[kAvOffroad]);
    if(s.fn==kDrop)g.time=s.v[kDrTime];
    if(s.fn==kSiren)for(u32 i=0;i<kPadHistSize;++i)g.hist[i]=u8(HornByte(s,n,i));
    return g;
}

inline void RunCase(Env& env,Totals& t,const Globals& base,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    Side sides[2];json rows[2];
    const Globals want=CaseGlobals(s,n,base);
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        nev=0;createCalls=0;cur=s;
        BuildArena(s,n);
        want.Apply();
        auto gate=[&](uintptr_t entry){
            const u32 eax=s.eax,edx=s.edx,ecx=A(oVeh);
            switch(fn) {
            case kRemove: {const u32 args[1]{s.v[kRmV0]};return Gate(entry,args,1,eax,ecx,edx,s.stack,0);}
            case kAddUp:  {const u32 args[2]{s.v[kAuModel],s.v[kAuIndex]};return Gate(entry,args,2,eax,ecx,edx,s.stack,0);}
            case kAddVeh: {const u32 args[1]{s.v[kAvModel]};return Gate(entry,args,1,eax,ecx,edx,s.stack,0);}
            case kDrop:   {const u32 args[2]{s.v[kDrType],s.v[kDrArg1]};return Gate(entry,args,2,eax,ecx,edx,s.stack,0);}
            default:      {const u32 args[1]{s.v[kSiArg0]};return Gate(entry,args,1,eax,ecx,edx,s.stack,0);}
            }
        };
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,gate,[]{});
        json mem=json::array();
        for(u32 i=0;i<kArenaWords;++i)if(Arena[i]!=ArenaInit[i])mem.push_back(json::array({i,Lab(Arena[i])}));
        json obs;
        const Reg& rg=sides[sd].reg;
        obs["ret"]=fn==kAddVeh?json::array({Lab(rg.eax)}):json::array();
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["mem"]=mem;
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        const bool globalsSame=Globals::Capture()==want;
        out["globals_restored"]=globalsSame&&watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        base.Apply();   // la region del pad y el resto vuelven a su estado inicial tras cada lado
        rows[sd]=out;
    }
    Finish(t,kFn[fn],n,input,rows,sides);
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    for(unsigned f=0;f<kFns;++f) {
        const uintptr_t body=env.route[f]?env.route[f]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    const Globals base=Globals::Capture();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"arena_size",kArenaSize},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)RunCase(env,totals,base,fn,c);
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    base.Apply();
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool globalsNow=Globals::Capture()==base;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"region_restored",globalsNow}});
    Write({{"event",ready&&totals.passed&&stateRestored&&globalsNow?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"region_restored",globalsNow}});
    RestoreFloatingState();savedFPActive=false;
}
}
