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
// Nucleo del banco ReplUpgrade72 (lote 72). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute. Plantilla: reversing/Vehicle66 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y en el cuerpo de la DLL).
//
// Banco par a par de CVehicle::AddReplacementUpgrade (0x6D3830, thiscall ret 8, sin valor de retorno). Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido.
// Colaboradores sustituidos por falsos identicos en ambos lados (parche en la entrada del EXE y, si la DLL tiene hook, en el cuerpo de la DLL):
//   GetFrameFromId (0x4C53C0), RwFrameForAllObjects (0x7F1200), RwFrameForAllChildren (0x7F0DC0), CBaseModelInfo::AddRef (0x4C4BA0), RpClumpAddAtomic (0x74A490), RpAtomicSetFrame (0x74BF20),
//   RwFrameDestroy (0x7F05A0), SetAtomicId (0x732290), SetAtomicFlag (0x7322B0), RpGeometryForAllMaterials (0x74C790), SetAtomicRenderCallback (0x7328A0), SetRenderPipelinesCB (0x4C88F0),
//   CreateReplacementAtomic (0x6D3700), RwMatrixUpdate (0x7F18A0), SetupClumpAfterVehicleUpgrade (0x6FDFE0).
// CreateInstance (vtable +0x2C) y AsDamageAtomicModelInfoPtr (vtable +8) del model info de la mejora son virtuales de una vtable falsa (la misma para ambos lados).
// Los model infos de mejoras y del vehiculo falsos se instalan en ms_modelInfoPtrs[15..19]. Ningun llamador del EXE (todos en AddVehicleUpgrade 0x6E3290) lee EAX/ECX/EDX tras la llamada
// (solo conservan ESI/EDI/EBX/EBP): ECX/EDX/EAX solo se registran.

namespace R72Live {
using namespace R72Fix;
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
inline constexpr const char* kFn[kFns]{"AddReplacementUpgrade"};
inline constexpr uintptr_t kEntry[kFns]{0x6D3830};
// colaboradores (direcciones del EXE)
inline constexpr uintptr_t kGetFrame = 0x4C53C0, kFrameObjects = 0x7F1200, kFrameChildren = 0x7F0DC0, kAddRef = 0x4C4BA0, kClumpAddAtomic = 0x74A490, kAtomicSetFrame = 0x74BF20,
    kFrameDestroy = 0x7F05A0, kSetAtomicId = 0x732290, kSetAtomicFlag = 0x7322B0, kGeomForAll = 0x74C790, kSetRenderCB = 0x7328A0, kSetPipelines = 0x4C88F0, kCreateRepl = 0x6D3700,
    kMatrixUpdate = 0x7F18A0, kPlate = 0x6FDFE0;

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

// ---- eventos de los falsos (nada aqui asigna memoria: se usan dentro de la ventana de parches)
// 1 GetFrameFromId(clump,id); 2 RwFrameForAllObjects(marco,cb,data); 3 RwFrameForAllChildren(marco,cb,data); 4 CreateInstance(model info, ms_bCreateDamagedVersion); 5 AddRef(model info);
// 6 RpClumpAddAtomic(clump,atomo); 7 RpAtomicSetFrame(atomo,marco); 8 RwFrameDestroy(marco); 9 SetAtomicId(atomo,id); 10 SetAtomicFlag(atomo,bandera); 11 RpGeometryForAllMaterials(geometria,cb);
// 12 SetAtomicRenderCallback(atomo,cb); 13 SetRenderPipelinesCB(atomo,arg); 14 CreateReplacementAtomic(this,info,marco,flags,ms_bCreateDamagedVersion,danado,rueda); 15 RwMatrixUpdate(matriz);
// 16 SetupClumpAfterVehicleUpgrade(clump,material,tipo); 17 AsDamageAtomicModelInfoPtr(info); 99 virtual inesperado de la vtable falsa
struct Ev {u32 kind{};std::array<u64,kEvArgs> a{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
inline void Push(u32 kind,u64 a0=0,u64 a1=0,u64 a2=0,u64 a3=0,u64 a4=0,u64 a5=0,u64 a6=0,u64 a7=0){Ev& e=nev<kMaxEvents?evs[nev]:evSink;e=Ev{kind,{a0,a1,a2,a3,a4,a5,a6,a7}};++nev;}
inline Spec cur{};
inline uintptr_t dllRemoveObj{},dllRemoveFrame{},dllVisObj{},dllVisFrame{},dllHasAlpha{},dllHiDetail{},dllHiDetailAlpha{};
inline constexpr u64 kStackMark=0xDA7A;

// Identificador normalizado de un callback (el del EXE y el de la DLL dan el mismo): 1 RemoveObjectsCB (objeto), 2 (marco), 3 SetVehicleAtomicVisibilityCB (objeto), 4 (marco),
// 5 HasAlphaMaterialCB, 6 RenderVehicleHiDetailCB, 7 RenderVehicleHiDetailAlphaCB, 99 otro
inline u32 CbId(void* cb){
    const uintptr_t a=uintptr_t(cb);
    const struct {uintptr_t exe,dll;u32 id;} t[]{{0x6D33B0,dllRemoveObj,1},{0x6D3420,dllRemoveFrame,2},{0x6D2690,dllVisObj,3},{0x6D26D0,dllVisFrame,4},{0x4C77C0,dllHasAlpha,5},{0x733240,dllHiDetail,6},{0x733F80,dllHiDetailAlpha,7}};
    for(const auto& e:t)if(a==e.exe||(e.dll&&Body(a)==Body(e.dll)))return e.id;
    return 99;
}
inline u32 __cdecl FkGetFrame(void* clump,u32 id){
    Push(1,Lab(clump),id);
    const u32 kind=cur.v[kFrameKind];
    return kind==0?0u:(kind==1?A(oF0):cur.v[kFrameOther]);
}
inline void* __cdecl FkFrameObjects(void* frame,void* cb,void* data){
    const u32 id=CbId(cb);
    Push(2,Lab(frame),id,id==1?kStackMark:Lab(data));
    if(id==1&&cur.v[kObjWrites])*static_cast<u32*>(data)=cur.v[kObjValue];
    return frame;
}
inline void* __cdecl FkFrameChildren(void* frame,void* cb,void* data){
    const u32 id=CbId(cb);
    Push(3,Lab(frame),id,id==2?kStackMark:Lab(data));
    if(id==2&&cur.v[kChWrites])*static_cast<u32*>(data)=cur.v[kChValue];
    return frame;
}
inline void __fastcall FkAddRef(void* self,void*){Push(5,Lab(self));}
inline void* __cdecl FkClumpAddAtomic(void* clump,void* atomic){Push(6,Lab(clump),Lab(atomic));return clump;}
inline void* __cdecl FkAtomicSetFrame(void* atomic,void* frame){Push(7,Lab(atomic),Lab(frame));reinterpret_cast<u32*>(atomic)[1]=u32(uintptr_t(frame));return atomic;}
inline u32 __cdecl FkFrameDestroy(void* frame){Push(8,Lab(frame));return 1;}
inline void __cdecl FkSetAtomicId(void* atomic,u32 id){Push(9,Lab(atomic),id);}
inline void __cdecl FkSetAtomicFlag(void* atomic,u32 flag){Push(10,Lab(atomic),flag&0xFFFFu);}
inline void* __cdecl FkGeomForAll(void* geometry,void* cb,void* data){Push(11,Lab(geometry),CbId(cb));*static_cast<u8*>(data)=u8(cur.v[kAlphaByte]);return geometry;}
inline void __cdecl FkSetRenderCB(void* atomic,void* cb){Push(12,Lab(atomic),CbId(cb));}
inline void* __cdecl FkSetPipelines(void* atomic,void* arg){Push(13,Lab(atomic),Lab(arg));return atomic;}
inline u32 __fastcall FkCreateRepl(void* self,void*,void* info,void* frame,u32 flags,u32 damaged,u32 wheel){
    Push(14,Lab(self),Lab(info),Lab(frame),flags,*reinterpret_cast<u8*>(kDamagedFlag),damaged&0xFFu,wheel&0xFFu);
    return A(oAT1);
}
inline void* __cdecl FkMatrixUpdate(void* matrix){Push(15,Lab(matrix));return matrix;}
inline u32 __cdecl FkPlate(void* clump,void* material,u32 type){Push(16,Lab(clump),Lab(material),type&0xFFu);return 0;}
// vtable falsa de los model infos de mejoras (el objeto guarda su direccion en +0)
inline u32 __fastcall FkCreateInstance(void* self,void*){Push(4,Lab(self),*reinterpret_cast<u8*>(kDamagedFlag));return A(oAT0);}
inline u32 __fastcall FkAsDamage(void* self,void*){Push(17,Lab(self));return cur.v[kDamageRet];}
inline u32 __fastcall FkTrap(void* self,void*){Push(99,Lab(self));return 0;}
inline std::array<u32,16> FakeVtbl{};
inline void InitVtbl(){
    for(auto& s:FakeVtbl)s=u32(uintptr_t(&FkTrap));
    FakeVtbl[2]=u32(uintptr_t(&FkAsDamage));
    FakeVtbl[10]=FakeVtbl[11]=u32(uintptr_t(&FkCreateInstance));
}

inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){json e=json::array({evs[i].kind});for(u64 x:evs[i].a)e.push_back(x);out.push_back(e);}
    return out;
}

// Imagen inicial de la arena de un caso (relleno + campos de la funcion). Debe coincidir con lo que modela fixture_oracle.py.
inline void BuildArena(const Spec& s,unsigned n) {
    for(u32 i=0;i<kArenaWords;++i)Arena[i]=FillWord(s.fn,n,i);
    S8(oClump,2);   // RwObject.type = rpCLUMP
    S16(oVeh+kVModel,s.v[kVehIdx]);   // indice del model info del vehiculo (int16 con signo en el EXE; aqui siempre 0..19999)
    W(oVeh+kVHandling)=A(oHnd);
    W(oVeh+kVClump)=s.v[kClumpOn]?A(oClump):0u;
    W(oVMi+kMiPlateMat)=s.v[kPlateMat];
    S8(oVMi+kMiPlateType,s.v[kPlateType]);
    W(oHnd+kHndFlags)=s.v[kHnd];
    for(u32 i=0;i<4;++i)W(oU0+0x40*i)=u32(uintptr_t(FakeVtbl.data()));
    W(oAT0+kAtParent)=s.v[kOldFrame];
    W(oAT0+kAtGeometry)=s.v[kGeom];
    W(oAT1+kAtParent)=A(oF1);
    W(oF0+kFrPosX)=s.v[kPosX];
    ArenaInit=Arena;
}

// Globales que el banco escribe durante un caso: las ranuras del pool de ms_modelInfoPtrs (kSlotPool: 415 entradas DENTRO de la tabla de 20000; en cada caso se instalan los model infos falsos
// en las ranuras v[kModel] y v[kVehIdx] y las demas conservan su valor) y ms_bCreateDamagedVersion (0xA9B0B0, que la funcion deja a 0: se registra y se repone).
struct Globals {
    std::array<u32,kPoolSize> slots{};u8 dmg{};
    static Globals Capture(){Globals g;for(u32 i=0;i<kPoolSize;++i)g.slots[i]=reinterpret_cast<u32*>(kModelInfoPtrs)[kSlotPool[i]];g.dmg=*reinterpret_cast<u8*>(kDamagedFlag);return g;}
    void Apply()const{for(u32 i=0;i<kPoolSize;++i)reinterpret_cast<u32*>(kModelInfoPtrs)[kSlotPool[i]]=slots[i];*reinterpret_cast<u8*>(kDamagedFlag)=dmg;}
    bool operator==(const Globals& o)const{return slots==o.slots&&dmg==o.dmg;}
};
inline unsigned PoolPos(u32 index){
    const auto it=std::lower_bound(kSlotPool.begin(),kSlotPool.end(),index);
    return it!=kSlotPool.end()&&*it==index?unsigned(it-kSlotPool.begin()):kPoolSize;
}
inline Globals CaseGlobals(const Spec& s,const Globals& base){
    Globals g=base;
    const unsigned pm=PoolPos(s.v[kModel]),pv=PoolPos(s.v[kVehIdx]);
    if(pm<kPoolSize&&pv<kPoolSize&&pm!=pv){
        g.slots[pv]=A(oVMi);                                // model info del vehiculo en [this+0x22]
        g.slots[pm]=A(oU0+0x40*(s.v[kModel]&3));            // model info de la mejora en modelIndex
    }
    g.dmg=u8(s.v[kDmgInit]);
    return g;
}

inline void RunCase(Env& env,Totals& t,const Globals& base,u32 fn,unsigned n) {
    const Spec s=MakeSpec(fn,n);
    const json input=json::parse(JsonSpec(s));
    Side sides[2];json rows[2];
    const Globals want=CaseGlobals(s,base);
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        nev=0;cur=s;
        BuildArena(s,n);
        want.Apply();
        auto gate=[&](uintptr_t entry){
            const u32 args[2]{s.v[kModel],s.v[kNode]};
            return Gate(entry,args,2,s.eax,A(oVeh),s.edx,s.stack,0);
        };
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,gate,[]{});
        json mem=json::array();
        for(u32 i=0;i<kArenaWords;++i)if(Arena[i]!=ArenaInit[i])mem.push_back(json::array({i,Lab(Arena[i])}));
        json obs;
        obs["ret"]=json::array();
        obs["events"]=EventsJson();obs["nevents"]=nev;obs["mem"]=mem;
        obs["dmg"]=*reinterpret_cast<u8*>(kDamagedFlag);
        *reinterpret_cast<u8*>(kDamagedFlag)=want.dmg;   // la funcion lo deja a 0: se repone antes de comparar el resto de las globales
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        const bool globalsSame=Globals::Capture()==want;
        out["globals_restored"]=globalsSame&&watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        base.Apply();
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
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"mxcsr_ftz",(savedMXCSR&0x8000u)!=0},{"slot_pool",kPoolSize},{"num_model_infos",kNumModelInfos}});
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
