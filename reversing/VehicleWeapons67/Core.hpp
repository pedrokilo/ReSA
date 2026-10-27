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
// Nucleo del banco VehicleWeapons67 (lote 67). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute. Plantilla: reversing/Vehicle66 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).
//
// Banco par a par de CVehicle::ProcessWeapons (0x6E3950). Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido.
// Colaboradores sustituidos por falsos identicos en ambos lados (parche en la entrada del EXE y, si la DLL tiene hook, en el cuerpo de la DLL):
//   FindPlayerVehicle (0x56E0D0), CWeaponEffects::ClearCrossHairImmediately (0x742CA0), GetPlaneWeaponFiringStatus (0x6E3440, sin hook en la DLL), SelectPlaneWeapon (0x6D4900),
//   FirePlaneGuns (0x6D4D30), PossiblyDropFreeFallBombForPlayer (0x6E07E0), FireHeatSeakingMissile (0x6E05C0, con hook en la DLL desde el lote 73: el mismo falso en su cuerpo), FireUnguidedMissile (0x6D5110),
//   GetPlaneNumGuns (0x6D3F30) y FxSystem_c::SetMatrix (0x4AA630).
// SelectPlaneWeapon del EXE NO toca EDX y ProcessWeapons lo aprovecha (`mov cl,dl` tras la llamada): su falso es un envoltorio desnudo (solo del banco) que conserva EDX.
// Registros de salida: los dos llamadores del EXE (0x6C7917, 0x6C9348) no leen EAX/ECX/EDX (residue_scan.py): solo se registran.

namespace V67Live {
using namespace V67Fix;
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
inline constexpr const char* kFn[kFns]{"ProcessWeapons"};
inline constexpr uintptr_t kEntry[kFns]{0x6E3950};
// colaboradores (direcciones del EXE)
inline constexpr uintptr_t aFindVeh=0x56E0D0,aClearCross=0x742CA0,aFiringStatus=0x6E3440,aSelect=0x6D4900,aFireGuns=0x6D4D30,aBomb=0x6E07E0,aHeat=0x6E05C0,aUnguided=0x6D5110,
    aNumGuns=0x6D3F30,aSetMatrix=0x4AA630;

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
// codigo de puntero -> valor (ver Fixtures.hpp)
inline u32 Ptr(u32 c){
    switch(c) {
    case 0: return 0u;
    case 1: return A(oOther);
    case 2: return A(oVeh);
    case 3: return A(oFrame);
    case 4: return A(oClump);
    default: return (c>=5&&c<=12)?A(oFx+8*(c-5)):c;
    }
}

// ---- eventos de los falsos (nada aqui asigna memoria: se usan dentro de la ventana de parches)
// 1 FindPlayerVehicle(id,bool); 2 ClearCrossHairImmediately(id); 3 GetPlaneWeaponFiringStatus(this,*bool,*tipo) (valores ANTES de escribir); 4 SelectPlaneWeapon(this,bool,tipo);
// 5 FirePlaneGuns(this); 6 PossiblyDropFreeFallBombForPlayer(this,tipo,bool); 7 FireHeatSeakingMissile(this,objetivo,tipo,bool); 8 FireUnguidedMissile(this,tipo,bool);
// 9 GetPlaneNumGuns(this); 10 FxSystem_c::SetMatrix(fx,matriz)
struct Ev {u32 kind{};std::array<u64,kEvArgs> a{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline void Push(u32 kind,u64 a0=0,u64 a1=0,u64 a2=0,u64 a3=0){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{kind,{a0,a1,a2,a3}};++nev;}
inline Spec cur{};
inline unsigned fpvCalls{};

inline u32 __cdecl FkFindVeh(u32 id,u32 flag){Push(1,id,flag&0xFFu);return Ptr(cur.v[fpvCalls++==0?kFpv1:kFpv2]);}
inline void __cdecl FkClearCross(u32 id){Push(2,id);}
inline u32 __fastcall FkFiring(void* self,void*,u8* status,u32* ordnance){
    Push(3,Lab(self),*status,*ordnance);
    if(cur.v[kStWrite])*status=u8(cur.v[kStByte]);
    if(cur.v[kOrdWrite])*ordnance=cur.v[kOrdVal];
    return Ptr(cur.v[kStRet]);
}
inline void __cdecl SelectImpl(void* self,u32 change,u32 type){
    Push(4,Lab(self),change&0xFFu,type);
    if(cur.v[kSelWrite])Bytes()[oVeh+kVWeapon]=u8(cur.v[kSelVal]);
}
// thiscall `ret 8` que CONSERVA EDX, como 0x6D4900 (no lo escribe en ningun camino): ProcessWeapons lo usa tras la llamada (`mov cl,dl`)
__declspec(naked) inline void FkSelect() {
    __asm {
        push edx
        push dword ptr [esp+12]
        push dword ptr [esp+12]
        push ecx
        call SelectImpl
        add esp,12
        pop edx
        ret 8
    }
}
inline void __fastcall FkFireGuns(void* self,void*){Push(5,Lab(self));}
inline void __fastcall FkBomb(void* self,void*,u32 type,u32 flag){Push(6,Lab(self),type,flag&0xFFu);}
inline void __fastcall FkHeat(void* self,void*,void* target,u32 type,u32 flag){Push(7,Lab(self),Lab(target),type,flag&0xFFu);}
inline void __fastcall FkUnguided(void* self,void*,u32 type,u32 flag){Push(8,Lab(self),type,flag&0xFFu);}
inline u32 __fastcall FkNumGuns(void* self,void*){Push(9,Lab(self));return cur.v[kNumGuns];}
inline void __fastcall FkSetMatrix(void* self,void*,void* matrix){Push(10,Lab(self),Lab(matrix));}

inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){json e=json::array({evs[i].kind});for(u64 x:evs[i].a)e.push_back(x);out.push_back(e);}
    return out;
}

// Imagen inicial de la arena de un caso. Debe coincidir con el significado de v[] (Fixtures.hpp).
inline void BuildArena(const Spec& s,unsigned n) {
    for(u32 i=0;i<kArenaWords;++i)Arena[i]=FillWord(n,i);
    S8(oClump,2);   // RwObject.type = rpCLUMP
    W(oVeh+kVSubType)=s.v[kSub];
    W(oVeh+kVPhysFlags)=s.v[kPhys];
    S8(oVeh+kVStatus,(s.v[kStat]<<3)|(s.v[kStLow]&7));
    W(oVeh+kVFlags42C)=s.v[kF42C];
    S16(oVeh+kVModel,s.v[kModel]);
    S8(oVeh+kVWeapon,s.v[kWeapon]);
    S8(oVeh+kVMission,s.v[kMission]);
    W(oVeh+kVAiTarget)=Ptr(s.v[kAiTarget]);
    W(oVeh+kVPlaneFlag)=s.v[kPlaneFlag];
    W(oVeh+kVHeliFx)=s.v[kHeliFx]?A(oFxArr):0u;
    W(oVeh+kVPlaneFx)=s.v[kPlaneFx]?A(oFxArr):0u;
    W(oVeh+kVClump)=s.v[kClump]?A(oClump):0u;
    W(oClump+4)=Ptr(s.v[kParent]);
    for(u32 i=0;i<8;++i)W(oFxArr+4*i)=Ptr(s.v[kFxEntries+i]);
    ArenaInit=Arena;
}

inline void RunCase(Env& env,Totals& t,unsigned n) {
    const Spec s=MakeSpec(n);
    const json input=json::parse(JsonSpec(s));
    Side sides[2];json rows[2];
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        nev=0;fpvCalls=0;cur=s;
        BuildArena(s,n);
        auto gate=[&](uintptr_t entry){return Gate(entry,nullptr,0,s.eax,A(oVeh),s.edx,s.stack,0);};
        sides[sd]=Execute(env,0,kEntry[0],candidate,gate,[]{});
        json mem=json::array();
        for(u32 i=0;i<kArenaWords;++i)if(Arena[i]!=ArenaInit[i])mem.push_back(json::array({i,Lab(Arena[i])}));
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["mem"]=mem;
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact;   // el banco no escribe ninguna global del juego: solo las entradas vigiladas
        out["floating_state_restored"]=FloatingStateRestored();
        rows[sd]=out;
    }
    Finish(t,kFn[0],n,input,rows,sides);
}

inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& collaborators) {
    {
        const uintptr_t body=env.route[0]?env.route[0]->Body():0;
        ready&=body&&ModuleOf(body)==env.asi;
    }
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();for(const char* nm:kFn)cases[nm]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"arena_size",kArenaSize},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)RunCase(env,totals,c);
        Write({{"event","function_summary"},{"function",kFn[0]},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.collab)if(r)stateRestored&=r->Restore();
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"region_restored",true}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact},{"region_restored",true}});
    RestoreFloatingState();savedFPActive=false;
}
}
