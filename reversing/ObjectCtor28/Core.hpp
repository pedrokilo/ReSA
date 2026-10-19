#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco ObjectCtor28. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, CObject (Object.h), ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y las cabeceras de ReversibleHook (Base/Simple/Virtual). Plantillas: reversing/FileLoaderLoad2dEffect27 y reversing/DecisionAdd27.
//
// Banco par a par de UNA funcion: CObject::CObject(int32 modelId, bool bCreate) (0x5A1D70, thiscall, `ret 8`, devuelve this).
// Original = bytes del EXE con el hook RH (CObject::Constructor) apagado; candidato = DLL con el hook encendido (envoltura `Constructor` -> constructor C++ de la DLL).
//
// Colaboradores SUSTITUIDOS por falsos identicos en ambos lados (parche de entrada de 5 bytes en la direccion del EXE y en el cuerpo de la DLL, solo durante la llamada):
//   CPhysical::CPhysical (0x542260), CEntity::SetModelIndex (0x532AE0), CEntity::SetModelIndexNoCreate (0x533700), CObject::Init (0x59F840).
// Los falsos dejan el objeto en un estado deterministico (ruido en [0,0x138) con un vptr marcador; palabra de modelo/marcador en +0x22/+0x24; ruido en los dwords impares de [0x138,0x17C))
// y registran la traza (tipo, this relativo al objeto, argumento, vptr observado, +0x170). Con eso el objeto final (0x17C bytes) y las guardas se comparan ENTEROS: cualquier store
// de mas, de menos o fuera de orden en el constructor (p. ej. m_pDummyObject = nullptr despues de Init, o el vptr sin escribir) cambia el resultado.
// El vptr propio es distinto por construccion (EXE 0x866F60, DLL su vtable): cada lado lo sustituye por la etiqueta kVtLabel SOLO si es el valor esperado de ese lado.
//
// ABI: ESP, EBX/ESI/EDI/EBP, DF, cadena SEH (fs:[0]) y x87 (FCW 24 bits, pila vacia). EAX debe ser this en AMBOS lados (los llamadores del EXE lo leen). ECX/EDX de salida: solo
// registrados (original_residue/candidate_residue): ningun llamador del EXE los lee (ver README.md).
namespace ObjectCtor28Live {
using namespace ObjectCtor28Fix;
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
// La funcion no usa x87 (los falsos tampoco): el estado x87 debe quedar IDENTICO al de entrada en ambos lados. Se compara lo observable igual que en los demas bancos:
//   FCW completa, TOP de la pila (bits 11-13 de FSW), la palabra de etiquetas y los ST VALIDOS (ninguno debe quedar: pila vacia). Se ignoran banderas de FSW, FIP/FDP y registros vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE lee ECX/EDX (ver README.md); EAX = this se exige APARTE (observed.eax_is_this).
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
// Igual que DecisionAdd27::Gate (siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`),
// mas la cadena SEH: fs:[0] antes de empujar nada y despues de la llamada deben coincidir (el constructor del EXE y el de la DLL enlazan y desenlazan su registro de excepciones).
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
        add esp,popBytes
        mov espAfter,esp
        mov ebxAfter,ebx
        mov esiAfter,esi
        mov ediAfter,edi
        mov ebpAfter,ebp
        mov eax,dword ptr fs:[0]
        mov sehAfter,eax
        pop edi
        pop esi
        pop ebx
    }
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter,sehBefore==sehAfter};
}

// ------------------------------------------------------------------------------------------------ memoria de los casos y falsos
alignas(16) inline std::array<u8,kArena> arena{};
inline u8* curObj{};                 // inicio del objeto dentro de la arena (para `this` relativo)
inline bool curCandidate{};          // lado que se ejecuta: decide el vptr propio esperado
inline u32 curSeedP{},curSeedI{};    // semillas del ruido de los falsos
inline u32 dllVptr{};                // vptr que escribe el constructor de la DLL (capturado una vez, ver CaptureDllVptr)
inline constexpr u32 kExeVptr=0x866F60u;
inline u32 ExpectedVptr(bool candidate){return candidate?dllVptr:kExeVptr;}
inline u32 VtClass(u32 v){return v==ExpectedVptr(curCandidate)?kVtLabel:v;}

struct Ev {u32 kind,a,b,c,d;};
inline std::array<Ev,16> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c,d};++nev;}}
inline u32 Rel(const void* self){return u32(static_cast<const u8*>(self)-curObj);}
inline u32 Dw(const void* self,size_t at){u32 v{};std::memcpy(&v,static_cast<const u8*>(self)+at,4);return v;}
inline u32 W16(const void* self,size_t at){u16 v{};std::memcpy(&v,static_cast<const u8*>(self)+at,2);return v;}
inline void PutW16(void* self,size_t at,u16 v){std::memcpy(static_cast<u8*>(self)+at,&v,2);}

// Falsos. thiscall sin argumentos en pila: __fastcall(self, edx) (`ret`); thiscall con un argumento: __fastcall(self, edx, arg) (`ret 4`).
// CPhysical::CPhysical: ruido en [0,0x138) y el vptr marcador en +0 (el constructor derivado debe sobrescribirlo). Devuelve this como el real.
inline void* __fastcall FakePhys(void* self,void*){
    Push(1,Rel(self));
    u8* p=static_cast<u8*>(self);
    FillWords(p,kPhysBytes,curSeedP);
    std::memcpy(p,&kBaseVptrMarker,4);
    return self;
}
inline void __fastcall FakeSetModel(void* self,void*,u32 index){
    Push(2,Rel(self),index,VtClass(Dw(self,0)),Dw(self,kDummyAt));
    PutW16(self,kModelAt,u16(index));PutW16(self,kMarkAt,0x5E01);
}
inline void __fastcall FakeNoCreate(void* self,void*,u32 index){
    Push(3,Rel(self),index,VtClass(Dw(self,0)),Dw(self,kDummyAt));
    PutW16(self,kModelAt,u16(index));PutW16(self,kMarkAt,0x5E02);
}
inline void __fastcall FakeInit(void* self,void*){
    Push(4,Rel(self),VtClass(Dw(self,0)),Dw(self,kDummyAt),W16(self,kModelAt));
    // Ruido solo en los dwords de indice IMPAR de [0x138,0x17C) (k = (offset-0x138)/4): los pares (incluido +0x170, k=14) conservan lo que haya, asi un store propio del constructor en
    // ellos (m_pDummyObject = 0) o uno ajeno anterior a Init sigue siendo visible en el objeto final.
    u8* p=static_cast<u8*>(self);
    for(u32 k=1;k<(kObjBytes-kInitAt)/4;k+=2){const u32 w=Rand(curSeedI,k);std::memcpy(p+kInitAt+4*k,&w,4);}
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Cuerpo de la DLL de una funcion con hook S (Simple) o V (virtual: el simbolo de una virtual es un thunk, el hook guarda la direccion real).
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn);if(!h)return 0;uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a?Body(a):0;
}
inline bool Prologue(uintptr_t f){const auto* b=reinterpret_cast<const uint8*>(f);if(b[0]==0x8B&&b[1]==0xFF)b+=2;return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC;}
// Primera llamada (E8) de `fn` (dentro del modulo `asi`) a una funcion de la DLL con prologo push ebp / mov ebp,esp: el cuerpo real de un constructor/colaborador no virtual.
inline uintptr_t FirstDllCall(uintptr_t asi,uintptr_t fn){
    fn=Body(fn);if(!fn||ModuleOf(fn)!=asi)return 0;
    for(uintptr_t p=fn;p<fn+0x300;++p){
        if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue;
        const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(ModuleOf(t)==asi&&Prologue(Body(t)))return Body(t);
    }
    return 0;
}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x5A1D70 y las cuatro entradas del EXE y los cuatro cuerpos de la DLL de los colaboradores: deben seguir intactos al volver
    Pair pPhys{},pSet{},pNoCreate{},pInit{};   // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    uintptr_t dllCtor{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,12> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){
    p.Add(e.pPhys,&FakePhys);p.Add(e.pSet,&FakeSetModel);p.Add(e.pNoCreate,&FakeNoCreate);p.Add(e.pInit,&FakeInit);
}

struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los parches de los colaboradores se instalan despues de comprobar la
// ruta y se quitan antes de volver.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        {
            Patches patches;
            InstallPatches(env,patches);
            side.reg=gate(entry);
        }
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"seh_chain_restored",side.reg.seh},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK}};
}
inline json Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","seh_chain_restored","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    return row;
}

// ------------------------------------------------------------------------------------------------ vptr de la DLL
// Construye (en un buffer propio, con los falsos instalados y sin tocar ningun pool) un CObject con el constructor de la DLL y con el constructor por defecto y lee su vptr. Debe ser el mismo,
// estar dentro del modulo ASI y no ser ni el del EXE ni el marcador del falso base. Devuelve false si no.
inline bool CaptureDllVptr(Env& env,json& info) {
    alignas(16) static u8 buf[0x200]{};
    u32 v1{},v2{};
    {
        Patches patches;
        InstallPatches(env,patches);
        curObj=buf;curCandidate=true;curSeedP=1;curSeedI=2;
        std::memset(buf,0,sizeof buf);nev=0;
        (void)::new(static_cast<void*>(buf)) CObject(0,false);
        std::memcpy(&v1,buf,4);
        std::memset(buf,0,sizeof buf);nev=0;
        (void)::new(static_cast<void*>(buf)) CObject();
        std::memcpy(&v2,buf,4);
        nev=0;
    }
    info={{"vptr_int_bool",v1},{"vptr_default",v2},{"in_asi",ModuleOf(v1)==env.asi}};
    dllVptr=v1;
    return v1==v2&&v1!=0&&v1!=kExeVptr&&v1!=kBaseVptrMarker&&ModuleOf(v1)==env.asi;
}

// ------------------------------------------------------------------------------------------------ caso
inline json InputOf(const F& f) {
    return {{"n",f.n},{"model_kind",f.modelKind},{"create_kind",f.createKind},{"mode",f.mode},{"obj_off",f.objOff},{"model_id",f.modelId},{"create",f.create},
            {"seed_p",f.seedP},{"seed_i",f.seedI},{"pre",HexBytes(f.pre,kObjBytes)},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseCtor(Env& env,Totals& t,unsigned n) {
    const F f=Make(n);
    std::array<u8,kArena> ref{};
    FillArena(ref.data(),f.mode,f.seedArena);
    u8* const obj=arena.data()+f.objOff;
    const json input=InputOf(f);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        arena=ref;nev=0;curObj=obj;curCandidate=candidate;curSeedP=f.seedP;curSeedI=f.seedI;
        sides[s]=Execute(env,*env.route,0x5A1D70,candidate,[&](uintptr_t entry){
            const u32 args[2]{f.modelId,f.create};
            return Gate(entry,args,2,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);   // `ret 8`: el constructor retira los argumentos
        });
        // Objeto final con el vptr propio sustituido por la etiqueta SOLO si es el esperado de este lado.
        std::array<u8,kObjBytes> ob{};
        std::memcpy(ob.data(),obj,kObjBytes);
        u32 vptr{};std::memcpy(&vptr,ob.data(),4);
        const bool vptrOwn=vptr==ExpectedVptr(candidate);
        if(vptrOwn){const u32 label=kVtLabel;std::memcpy(ob.data(),&label,4);}
        bool guards=true;
        for(size_t i=0;i<arena.size();++i){if(i>=f.objOff&&i<f.objOff+kObjBytes)continue;if(arena[i]!=ref[i]){guards=false;break;}}
        json trace=json::array();
        for(unsigned i=0;i<nev;++i)trace.push_back(json::array({evs[i].kind,evs[i].a,evs[i].b,evs[i].c,evs[i].d}));
        json observed={{"obj",HexBytes(ob.data(),kObjBytes)},{"vptr_own",vptrOwn},{"guard_ok",guards},{"trace",trace},{"eax_is_this",sides[s].reg.eax==u32(uintptr_t(obj))}};
        if(kCompareVolatileRegisters){observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,"CObject::CObject",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& resolved) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x5A1D70),env.pPhys.exe,env.pSet.exe,env.pNoCreate.exe,env.pInit.exe,env.pPhys.dll,env.pSet.dll,env.pNoCreate.dll,env.pInit.dll})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    PrepareFloatingState();
    json vptrInfo=json::object();
    if(ready)ready&=CaptureDllVptr(env,vptrInfo);
    RestoreFloatingState();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},{"cases",{{"CObject::CObject",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",std::vector<uintptr_t>{0x542260,0x532AE0,0x533700,0x59F840}},{"dll",std::vector<uintptr_t>{env.pPhys.dll,env.pSet.dll,env.pNoCreate.dll,env.pInit.dll}}}},
        {"resolved",resolved},{"dll_vptr",vptrInfo},{"exe_vptr",kExeVptr},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseCtor(env,totals,c);
        Write({{"event","function_summary"},{"function","CObject::CObject"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
