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
#include <utility>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco Helpers65C. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, CProjectile (Projectile.h), ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y las cabeceras de ReversibleHook (Base/Simple/Virtual). Plantillas: reversing/ObjectCtor28 (constructor, Gate con SEH, vptr propio de cada lado)
// y reversing/CameraShake30 (falsos __fastcall, traza, globales del juego guardados y restaurados).
//
// Banco par a par de CINCO funciones pequenas (4096 casos cada una). Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con el hook encendido.
//   0 Fx_c::TriggerWaterHydrant 0x4A0D70   1 Fx_c::TriggerTankFire 0x4A0FA0   2 Fx_c::AddTyreBurst 0x49F300
//   3 CProjectile::CProjectile 0x5A4030     4 CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB 0x63C840 (cdecl; el llamador retira los 8 bytes de argumentos)
// Colaboradores SUSTITUIDOS por falsos identicos en ambos lados (parche de entrada de 5 bytes en la direccion del EXE y en el cuerpo de la DLL, solo durante la llamada); todos registran en
// una traza plana (tipo de evento + argumentos leidos en el momento de la llamada) que se compara entera con el oraculo:
//   Fx:   FxManager_c::CreateFxSystem(name,CVector) 0x4A9BE0, FxSystem_c::PlayAndKill 0x4AA3D0, FxManager_c::FxRwMatrixCreate 0x4A9440, FxManager_c::FxRwMatrixDestroy 0x4A9460,
//         Fx_c::CreateMatFromVec 0x49E950 (escribe un patron en la matriz M), FxSystem_c::CopyParentMatrix 0x4AA890, FxSystem_c::AddParticle(v3d) 0x4AA440.
//   Proy: CObject::CObject() 0x5A1D10 (ruido + vptr marcador), CObject::SetIsStatic 0x5A0760 (invierte la bandera +0x1C bit 2 y borra/invierte bits de +0x40), CEntity::SetModelIndex 0x532AE0.
//   Rod:  CVehicle::ClearGettingOutFlags 0x6D3060 y CVehicleAnimGroup::GetGroup 0x6E3B00 (pueden re-apuntar la tarea a otro vehiculo o cambiar el conductor), y la ranura 28 de la vtable del
//         vehiculo (ProcessOpenDoor): una vtable de arena con 64 falsos distintos que registran su indice.
// ABI: ESP, EBX/ESI/EDI/EBP, DF, cadena SEH (fs:[0]) y x87 (FCW 24 bits, pila vacia). EAX/ECX/EDX de salida: solo registrados, salvo EAX = this en el constructor (lo leen los 3 llamadores).
namespace Helpers65CLive {
using namespace Helpers65CFix;
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
// El EXE de las funciones Fx usa x87 (distancia a la camara) y la DLL puede usar SSE: el estado x87 se compara normalizado (FCW completa, TOP, etiquetas y ST VALIDOS: ninguno debe quedar).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md); EAX = this del constructor se exige APARTE (observed.eax_is_this).
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
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`, retira popBytes (0 en `ret n`, 8 en
// la funcion cdecl) y comprueba ESP, EBX/ESI/EDI/EBP y la cadena SEH (fs:[0]).
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

// ------------------------------------------------------------------------------------------------ estado de los falsos
alignas(16) inline std::array<u8,0x1300> arena{};      // la mayor es la del callback (0x1250)
inline u32 arenaSize{};
inline std::vector<u32> tr;                            // traza plana de los colaboradores
inline bool curCandidate{};                            // lado que se ejecuta: decide el vptr propio esperado del constructor
inline u32 dllVptr{};                                  // vptr que escribe el constructor de la DLL (capturado una vez)
inline u32 curCreateRet{},curMSeed{},curSeedP{};       // resultado de CreateFxSystem, semilla del patron de CreateMatFromVec, semilla del ruido de CObject()
inline const RollCase* curRoll{};
inline u8* AB(){return arena.data();}
inline u32 Tag(const void* p){const auto a=uintptr_t(p);if(!a)return 0;const auto b=uintptr_t(AB());if(a>=b&&a<b+arenaSize)return 1u+u32(a-b);return 0xFFFFFFFEu;}
inline u32 Dw(const void* self,size_t at){u32 v{};std::memcpy(&v,static_cast<const u8*>(self)+at,4);return v;}
inline void PutDw(void* self,size_t at,u32 v){std::memcpy(static_cast<u8*>(self)+at,&v,4);}
inline u32 W16(const void* self,size_t at){u16 v{};std::memcpy(&v,static_cast<const u8*>(self)+at,2);return v;}
inline void PutW16(void* self,size_t at,u16 v){std::memcpy(static_cast<u8*>(self)+at,&v,2);}
inline void P(std::initializer_list<u32> v){if(tr.size()<256)for(const u32 x:v)tr.push_back(x);}
inline u32 Fnv(const char* s){u32 h=2166136261u;for(;*s;++s){h^=u8(*s);h*=16777619u;}return h;}
inline u32 ExpectedVptr(bool candidate){return candidate?dllVptr:kExeVptr;}
inline u32 VtClass(u32 v){return v==ExpectedVptr(curCandidate)?kVtLabel:v;}

// Falsos. thiscall con N argumentos en pila: __fastcall(self, edx ficticio, args...) (el llamado retira N*4 bytes, igual que `ret N*4`).
// --- Fx
inline void* __fastcall FakeCreateFx(void* self,void*,const char* name,const void* point,void* mat,u32 flag){        // 0x4A9BE0, `ret 10h`
    P({1,u32(uintptr_t(self)),Fnv(name),Dw(point,0),Dw(point,4),Dw(point,8),Tag(mat),flag});
    return curCreateRet?AB()+kFxSystem:nullptr;
}
inline void __fastcall FakePlayAndKill(void* self,void*){P({2,Tag(self)});}                                          // 0x4AA3D0, `ret`
inline void* __fastcall FakeMatCreate(void* self,void*){P({3,u32(uintptr_t(self))});return AB()+kFxMatM;}           // 0x4A9440, `ret`
inline void __fastcall FakeMatDestroy(void* self,void*,void* mat){P({6,u32(uintptr_t(self)),Tag(mat)});}            // 0x4A9460, `ret 4`
inline void __fastcall FakeCreateMat(void* self,void*,void* mat,const void* pos,const void* dir){                    // 0x49E950, `ret 0Ch`
    P({4,Tag(self),Tag(mat),Dw(pos,0),Dw(pos,4),Dw(pos,8),Dw(dir,0),Dw(dir,4),Dw(dir,8)});
    for(u32 i=0;i<16;++i)PutDw(mat,4*i,curMSeed*0x9E3779B1u+i*0x85EBCA6Bu);
}
inline void __fastcall FakeCopyParent(void* self,void*){P({5,Tag(self)});}                                          // 0x4AA890, `ret`
inline void __fastcall FakeAddParticle(void* self,void*,const void* pos,const void* vel,u32 timeSince,const void* mult,u32 rotZ,u32 light,u32 limit,u32 createLocal){   // 0x4AA440, `ret 20h`
    P({7,Tag(self),Dw(pos,0),Dw(pos,4),Dw(pos,8),Dw(vel,0),Dw(vel,4),Dw(vel,8),timeSince,Dw(mult,0),Dw(mult,4),Dw(mult,8),Dw(mult,12),Dw(mult,16),Dw(mult,20),Dw(mult,24),rotZ,light,limit,createLocal});
}
// --- CProjectile
inline void* __fastcall FakeObjectCtor(void* self,void*){                                                            // 0x5A1D10, `ret`: ruido + vptr marcador (el derivado debe sobrescribirlo)
    P({8,Tag(self)});
    FillWords(static_cast<u8*>(self),kObjBytes,curSeedP);
    PutDw(self,0,kBaseVptrMarker);
    return self;
}
inline void __fastcall FakeSetIsStatic(void* self,void*,u32 arg){                                                    // 0x5A0760, `ret 4`
    P({9,Tag(self),arg,VtClass(Dw(self,0)),Dw(self,kOffFlags1C),Dw(self,kOffFlags40),Dw(self,kOffMass),Dw(self,kOffTurnMass),Dw(self,kOffAirRes),Dw(self,kOffElasticity),Dw(self,kOffBuoyancy)});
    PutDw(self,kOffFlags1C,Dw(self,kOffFlags1C)^4u);
    PutDw(self,kOffFlags40,(Dw(self,kOffFlags40)&~0x00800000u)^0x1000u);
}
inline void __fastcall FakeSetModel(void* self,void*,u32 index){                                                     // 0x532AE0, `ret 4`
    P({10,Tag(self),index,VtClass(Dw(self,0)),Dw(self,kOffFlags1C),Dw(self,kOffFlags40),Dw(self,kOffObjType)});
    PutW16(self,kOffModel,u16(index));PutW16(self,kOffMark,0x5E01);
}
// --- callback
inline u8* TaskP(){return AB()+kTaskOff;}
inline u8* VehP(u32 i){return AB()+kVehOff+i*kVehBytes;}
inline void __fastcall FakeClearGettingOut(void* self,void*,u32 arg){                                                // 0x6D3060, `ret 4`
    P({11,Tag(self),arg});
    if(curRoll&&curRoll->mutA)PutDw(TaskP(),kTaskVehicle,u32(uintptr_t(VehP(curRoll->idxA))));
}
inline u32 __fastcall FakeGetGroup(void* self,void*,u32 animId){                                                     // 0x6E3B00, `ret 4`, devuelve el grupo en EAX
    P({12,u32(uintptr_t(self))-kGroupTable,animId});
    if(curRoll){
        if(curRoll->mutB)PutDw(TaskP(),kTaskVehicle,u32(uintptr_t(VehP(curRoll->idxB))));
        if(curRoll->mutC)PutDw(VehP(curRoll->idxC),kVehDriver,curRoll->newDriver);
        return curRoll->groupRet;
    }
    return 0;
}
template<unsigned K> inline void __fastcall FakeVirt(void* self,void*,u32 a1,u32 a2,u32 a3,u32 a4,u32 a5){          // ranura K de la vtable del vehiculo, `ret 14h`
    P({13,K,Tag(self),a1,a2,a3,a4,a5});
}
template<size_t... I> inline std::array<uintptr_t,sizeof...(I)> MakeVtable(std::index_sequence<I...>){return {reinterpret_cast<uintptr_t>(&FakeVirt<unsigned(I)>)...};}
inline const std::array<uintptr_t,kVtabSlots> kFakeVtable=MakeVtable(std::make_index_sequence<kVtabSlots>{});

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI: leccion 2 de BRIEF60).
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
inline constexpr unsigned kFunc=kFunctions;
inline constexpr const char* kNames[kFunc]{"Fx_c::TriggerWaterHydrant","Fx_c::TriggerTankFire","Fx_c::AddTyreBurst","CProjectile::CProjectile","CTaskSimpleCarDrive::FinishAnimCloseDoorRollingCB"};
inline constexpr uintptr_t kEntries[kFunc]{0x4A0D70,0x4A0FA0,0x49F300,0x5A4030,0x63C840};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route[kFunc];
    std::vector<Watch> watches;          // entradas del EXE y cuerpos de la DLL de las funciones y de los colaboradores: deben seguir intactos al volver
    // colaboradores (cuerpos de la DLL resueltos ANTES de escribir ningun parche)
    Pair create{},play{},matCreate{},matDestroy{},createMat{},copyParent{},addParticle{};
    Pair objCtor{},setStatic{},setModel{};
    Pair clear{},group{};
    uintptr_t dllProjCtor{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,24> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,unsigned func,Patches& p){
    switch(func){
    case kHydrant:p.Add(e.create,&FakeCreateFx);p.Add(e.play,&FakePlayAndKill);break;
    case kTankFire:p.Add(e.create,&FakeCreateFx);p.Add(e.play,&FakePlayAndKill);p.Add(e.matCreate,&FakeMatCreate);p.Add(e.matDestroy,&FakeMatDestroy);p.Add(e.createMat,&FakeCreateMat);p.Add(e.copyParent,&FakeCopyParent);break;
    case kTyreBurst:p.Add(e.addParticle,&FakeAddParticle);break;
    case kProjCtor:p.Add(e.objCtor,&FakeObjectCtor);p.Add(e.setStatic,&FakeSetIsStatic);p.Add(e.setModel,&FakeSetModel);break;
    default:p.Add(e.clear,&FakeClearGettingOut);p.Add(e.group,&FakeGetGroup);break;
    }
}

struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los parches de los colaboradores se instalan despues de comprobar la
// ruta y se quitan antes de volver (los globales se comprueban DESPUES de quitarlos: leccion 3 de BRIEF60).
template<class Fn> Side Execute(Env& env,unsigned func,bool candidate,Fn gate) {
    Side side{};
    const uintptr_t entry=kEntries[func];
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        {
            Patches patches;
            InstallPatches(env,func,patches);
            side.reg=gate(entry);
        }
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route[func]->Invoke(candidate,call);
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
inline json Trace(){json a=json::array();for(const u32 v:tr)a.push_back(v);return a;}

// ------------------------------------------------------------------------------------------------ globales del juego que tocan las funciones Fx
struct FxGlobals {
    std::array<u8,12> placement{};u32 matrixPtr{};u32 smoke{};
    void Save(){std::memcpy(placement.data(),reinterpret_cast<void*>(uintptr_t(kCamPlacement)),12);matrixPtr=*reinterpret_cast<u32*>(uintptr_t(kCamMatrixPtr));smoke=*reinterpret_cast<u32*>(uintptr_t(kSmokeSlot));}
    void Restore()const{std::memcpy(reinterpret_cast<void*>(uintptr_t(kCamPlacement)),placement.data(),12);*reinterpret_cast<u32*>(uintptr_t(kCamMatrixPtr))=matrixPtr;*reinterpret_cast<u32*>(uintptr_t(kSmokeSlot))=smoke;}
    bool Equal(const FxGlobals& o)const{return placement==o.placement&&matrixPtr==o.matrixPtr&&smoke==o.smoke;}
};
inline FxGlobals fxInitial;   // valores del juego antes del banco

// ------------------------------------------------------------------------------------------------ casos Fx (funciones 0..2)
inline json FxInput(const FxCase& f){
    return {{"func",f.func},{"n",f.n},{"family",f.family},{"cam_mode",f.camMode},{"cam",std::vector<u32>{f.cam[0],f.cam[1],f.cam[2]}},{"other",std::vector<u32>{f.other[0],f.other[1],f.other[2]}},
            {"pos",std::vector<u32>{f.pos[0],f.pos[1],f.pos[2]}},{"dir",std::vector<u32>{f.dir[0],f.dir[1],f.dir[2]}},{"create_kind",f.createKind},{"this_kind",f.thisKind},{"mseed",f.mseed},
            {"mode",f.mode},{"seed",f.seedArena},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseFx(Env& env,Totals& t,unsigned func,unsigned n) {
    const FxCase f=MakeFx(func,n);
    arenaSize=kFxArena;
    std::array<u8,kFxArena> ref{};
    FillArena(ref.data(),kFxArena,f.mode,f.seedArena);
    for(int i=0;i<3;++i){PutDw(ref.data(),kFxPos+4*i,f.pos[i]);PutDw(ref.data(),kFxDir+4*i,f.dir[i]);}
    // matriz de la camara de la arena: la posicion usada en el modo 1 (+0x30); en el modo 0 el +0x30 lleva la `other` que nadie lee
    for(int i=0;i<3;++i)PutDw(ref.data(),kFxCamMat+0x30+4*i,f.camMode?f.cam[i]:f.other[i]);
    const u32 thisPtr=f.thisKind==0?u32(uintptr_t(AB()+kFxThis)):f.thisKind==1?0xA9AE00u:(0xDEAD0000u|(Rand(f.n,77)&0xFFFFu));
    const json input=FxInput(f);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        arena=decltype(arena){};std::memcpy(AB(),ref.data(),kFxArena);
        tr.clear();curCandidate=candidate;curCreateRet=f.createKind;curMSeed=f.mseed;curRoll=nullptr;
        // globales: camara (m_matrix / posicion de 0xB6F02C) y el sistema de humo
        FxGlobals set=fxInitial;
        for(int i=0;i<3;++i)std::memcpy(set.placement.data()+4*i,&(f.camMode?f.other[i]:f.cam[i]),4);
        set.matrixPtr=f.camMode?u32(uintptr_t(AB()+kFxCamMat)):0u;
        set.smoke=u32(uintptr_t(AB()+kFxSystem));
        set.Restore();
        const u32 posP=u32(uintptr_t(AB()+kFxPos)),dirP=u32(uintptr_t(AB()+kFxDir));
        sides[s]=Execute(env,func,candidate,[&](uintptr_t entry){
            if(func==kHydrant){const u32 args[1]{posP};return Gate(entry,args,1,f.eax,thisPtr,f.edx,f.stack,0);}
            const u32 args[2]{posP,dirP};return Gate(entry,args,2,f.eax,thisPtr,f.edx,f.stack,0);
        });
        FxGlobals after;after.Save();
        const bool globalsSame=after.Equal(set);
        bool guards=std::memcmp(AB(),ref.data(),kFxPos)==0&&std::memcmp(AB()+kFxEnd,ref.data()+kFxEnd,kGuard)==0;
        std::array<u8,kFxArena> a{},b{};std::memcpy(a.data(),AB(),kFxArena);b=ref;
        std::memset(a.data()+kFxMatM,0,0x40);std::memset(b.data()+kFxMatM,0,0x40);
        const bool rest=a==b;
        json observed={{"trace",Trace()},{"mat",HexBytes(AB()+kFxMatM,0x40)},{"rest_ok",rest},{"guard_ok",guards},{"globals_unchanged",globalsSame}};
        if(kCompareVolatileRegisters){observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        fxInitial.Restore();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        FxGlobals now;now.Save();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&now.Equal(fxInitial);
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kNames[func],n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ CProjectile (funcion 3)
inline json PrjInput(const PrjCase& f,const std::array<u8,kObjBytes>& pre){
    return {{"func",f.func},{"n",f.n},{"family",f.family},{"model_kind",f.modelKind},{"mode",f.mode},{"obj_off",f.objOff},{"model_id",f.modelId},{"seed_arena",f.seedArena},{"seed_p",f.seedP},
            {"pre",HexBytes(pre.data(),kObjBytes)},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseProj(Env& env,Totals& t,unsigned n) {
    const PrjCase f=MakePrj(n);
    arenaSize=kPrjArena;
    std::array<u8,kPrjArena> ref{};
    FillArena(ref.data(),kPrjArena,f.mode,f.seedArena);
    u8* const obj=AB()+f.objOff;
    std::array<u8,kObjBytes> pre{};std::memcpy(pre.data(),ref.data()+f.objOff,kObjBytes);
    const json input=PrjInput(f,pre);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        arena=decltype(arena){};std::memcpy(AB(),ref.data(),kPrjArena);
        tr.clear();curCandidate=candidate;curSeedP=f.seedP;curRoll=nullptr;
        sides[s]=Execute(env,kProjCtor,candidate,[&](uintptr_t entry){
            const u32 args[1]{f.modelId};
            return Gate(entry,args,1,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);       // `ret 4`: el constructor retira el argumento
        });
        std::array<u8,kObjBytes> ob{};std::memcpy(ob.data(),obj,kObjBytes);
        u32 vptr{};std::memcpy(&vptr,ob.data(),4);
        const bool vptrOwn=vptr==ExpectedVptr(candidate);
        if(vptrOwn){const u32 label=kVtLabel;std::memcpy(ob.data(),&label,4);}
        bool guards=true;
        for(size_t i=0;i<kPrjArena;++i){if(i>=f.objOff&&i<f.objOff+kObjBytes)continue;if(AB()[i]!=ref[i]){guards=false;break;}}
        json observed={{"obj",HexBytes(ob.data(),kObjBytes)},{"vptr_own",vptrOwn},{"guard_ok",guards},{"trace",Trace()},{"eax_is_this",sides[s].reg.eax==u32(uintptr_t(obj))}};
        if(kCompareVolatileRegisters){observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kNames[kProjCtor],n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ callback (funcion 4)
inline json RollInput(const RollCase& f){
    return {{"func",f.func},{"n",f.n},{"family",f.family},{"mode",f.mode},{"seed_arena",f.seedArena},{"task_veh",f.taskVeh},{"flags",f.flags},
            {"driver",std::vector<u32>{f.driver[0],f.driver[1],f.driver[2]}},{"handling",std::vector<u32>{f.handling[0],f.handling[1],f.handling[2]}},
            {"anim_group",std::vector<u32>{f.animGroup[0],f.animGroup[1],f.animGroup[2]}},{"mut_a",f.mutA},{"idx_a",f.idxA},{"mut_b",f.mutB},{"idx_b",f.idxB},{"mut_c",f.mutC},{"idx_c",f.idxC},
            {"new_driver",f.newDriver},{"group_ret",f.groupRet},{"assoc",f.assoc},{"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseRoll(Env& env,Totals& t,unsigned n) {
    const RollCase f=MakeRoll(n);
    arenaSize=kRollArena;
    std::array<u8,kRollArena> ref{};
    FillArena(ref.data(),kRollArena,f.mode,f.seedArena);
    // estructuras: handling (byte +0xDE), vehiculos (vtable, handling, conductor), vtable de arena, tarea (vehiculo, bandera)
    const u32 base=u32(uintptr_t(AB()));
    for(u32 j=0;j<3;++j)ref[kHandOff+j*kHandBytes+kHandAnimGroup]=u8(f.animGroup[j]);
    for(u32 i=0;i<3;++i){
        PutDw(ref.data(),kVehOff+i*kVehBytes,base+kVtabOff);
        PutDw(ref.data(),kVehOff+i*kVehBytes+kVehHandling,base+kHandOff+f.handling[i]*kHandBytes);
        PutDw(ref.data(),kVehOff+i*kVehBytes+kVehDriver,f.driver[i]);
    }
    for(u32 k=0;k<kVtabSlots;++k)PutDw(ref.data(),kVtabOff+4*k,u32(kFakeVtable[k]));
    PutDw(ref.data(),kTaskOff+kTaskVehicle,f.taskVeh==3?0u:base+kVehOff+f.taskVeh*kVehBytes);
    ref[kTaskOff+kTaskFlags]=u8(f.flags);
    const json input=RollInput(f);
    std::array<u8,kTaskBytes> taskRef{};std::memcpy(taskRef.data(),ref.data()+kTaskOff,kTaskBytes);
    PutDw(taskRef.data(),kTaskVehicle,Tag(reinterpret_cast<const void*>(uintptr_t(Dw(taskRef.data(),kTaskVehicle)))));      // punteros de la arena como etiquetas (la direccion de la arena no es estable)
    json inputFull=input;inputFull["task_pre"]=HexBytes(taskRef.data(),kTaskBytes);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        arena=decltype(arena){};std::memcpy(AB(),ref.data(),kRollArena);
        tr.clear();curCandidate=candidate;curRoll=&f;
        const u32 taskP=u32(uintptr_t(TaskP()));
        sides[s]=Execute(env,kRollCb,candidate,[&](uintptr_t entry){
            const u32 args[2]{f.assoc,taskP};
            return Gate(entry,args,2,f.eax,f.ecx,f.edx,f.stack,8);                      // cdecl: el llamador retira los 8 bytes de argumentos
        });
        // lo que puede cambiar: la tarea (flags, vehiculo, animacion) y los campos de vehiculo tocados por los falsos (conductor)
        json veh=json::array();
        auto TagDw=[](u32 v){return Tag(reinterpret_cast<const void*>(uintptr_t(v)));};
        for(u32 i=0;i<3;++i)veh.push_back({TagDw(Dw(VehP(i),0)),TagDw(Dw(VehP(i),kVehHandling)),Dw(VehP(i),kVehDriver)});
        std::array<u8,kTaskBytes> taskOut{};std::memcpy(taskOut.data(),TaskP(),kTaskBytes);
        PutDw(taskOut.data(),kTaskVehicle,TagDw(Dw(taskOut.data(),kTaskVehicle)));
        std::array<u8,kRollArena> a{},b{};std::memcpy(a.data(),AB(),kRollArena);b=ref;
        std::memset(a.data()+kTaskOff,0,kTaskBytes);std::memset(b.data()+kTaskOff,0,kTaskBytes);
        for(u32 i=0;i<3;++i){PutDw(a.data(),kVehOff+i*kVehBytes+kVehDriver,0);PutDw(b.data(),kVehOff+i*kVehBytes+kVehDriver,0);}
        const bool rest=a==b;
        bool guards=std::memcmp(AB(),ref.data(),kGuard)==0&&std::memcmp(AB()+kRollArena-kGuard,ref.data()+kRollArena-kGuard,kGuard)==0;
        json observed={{"task",HexBytes(taskOut.data(),kTaskBytes)},{"veh",veh},{"rest_ok",rest},{"guard_ok",guards},{"trace",Trace()}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    curRoll=nullptr;
    Finish(t,kNames[kRollCb],n,inputFull,rows,sides);
}

// ------------------------------------------------------------------------------------------------ vptr de la DLL
// Construye (en un buffer propio, con los falsos instalados y sin tocar ningun pool) un CProjectile con el constructor de la DLL y comprueba: la traza es [CObject(), SetIsStatic, SetModelIndex],
// el vptr final esta en el modulo ASI, no es el del EXE ni el marcador del falso base.
inline bool CaptureDllVptr(Env& env,json& info) {
    alignas(16) static u8 buf[0x200]{};
    u32 v{};
    {
        Patches patches;
        InstallPatches(env,kProjCtor,patches);
        std::memset(AB(),0,0x20);arenaSize=sizeof buf;
        // Tag/AB usan `arena`: el buffer de captura se trata como una arena independiente solo para la traza
        curCandidate=true;curSeedP=1;curRoll=nullptr;tr.clear();
        std::memset(buf,0,sizeof buf);
        (void)::new(static_cast<void*>(buf)) CProjectile(0);
        std::memcpy(&v,buf,4);
    }
    const bool shape=tr.size()>=3&&tr[0]==8;
    info={{"vptr",v},{"in_asi",ModuleOf(v)==env.asi},{"trace_calls",tr.size()},{"first_event",tr.empty()?0u:tr[0]}};
    tr.clear();
    dllVptr=v;
    return shape&&v!=0&&v!=kExeVptr&&v!=kBaseVptrMarker&&ModuleOf(v)==env.asi;
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& resolved) {
    for(unsigned f=0;f<kFunc;++f){const uintptr_t body=env.route[f]?env.route[f]->Body():0;ready&=body&&ModuleOf(body)==env.asi;}
    std::vector<uintptr_t> exeCollab,dllCollab;
    for(const Pair* p:{&env.create,&env.play,&env.matCreate,&env.matDestroy,&env.createMat,&env.copyParent,&env.addParticle,&env.objCtor,&env.setStatic,&env.setModel,&env.clear,&env.group}){
        exeCollab.push_back(p->exe);dllCollab.push_back(p->dll);
        env.watches.emplace_back(p->exe,16);env.watches.emplace_back(p->dll,16);
    }
    for(unsigned f=0;f<kFunc;++f){env.watches.emplace_back(kEntries[f],16);if(env.route[f])env.watches.emplace_back(env.route[f]->Body(),16);}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    PrepareFloatingState();
    fxInitial.Save();
    json vptrInfo=json::object();
    if(ready)ready&=CaptureDllVptr(env,vptrInfo);
    RestoreFloatingState();
    json cases=json::object();for(unsigned f=0;f<kFunc;++f)cases[kNames[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunc},{"pairs",kFunc*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",exeCollab},{"dll",dllCollab}}},{"resolved",resolved},{"dll_vptr",vptrInfo},{"exe_vptr",kExeVptr},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},
        {"initial_globals",{{"placement",HexBytes(fxInitial.placement.data(),12)},{"matrix_ptr",fxInitial.matrixPtr},{"smoke",fxInitial.smoke}}},
        {"layout",{{"fx_arena",kFxArena},{"prj_arena",kPrjArena},{"roll_arena",kRollArena},{"obj_bytes",kObjBytes},{"task_bytes",kTaskBytes},{"veh_bytes",kVehBytes},{"hand_bytes",kHandBytes}}}});
    Totals totals;
    if(ready) {
        for(unsigned func=0;func<kFunc;++func){
            Totals ft;ft.pairs=totals.pairs;
            for(unsigned c=0;c<kCases;++c){
                if(func<=kTyreBurst)CaseFx(env,ft,func,c);
                else if(func==kProjCtor)CaseProj(env,ft,c);
                else CaseRoll(env,ft,c);
            }
            totals.pairs=ft.pairs;totals.failures+=ft.failures;totals.passed&=ft.passed;
            Write({{"event","function_summary"},{"function",kNames[func]},{"cases",kCases},{"failures",ft.failures},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    FxGlobals fin;fin.Save();
    bool stateRestored=entriesIntact&&fin.Equal(fxInitial);
    for(unsigned f=0;f<kFunc;++f)if(env.route[f])stateRestored&=env.route[f]->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"globals",{{"placement",HexBytes(fin.placement.data(),12)},{"matrix_ptr",fin.matrixPtr},{"smoke",fin.smoke}}}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
