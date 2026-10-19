#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cstring>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco GaragesImpound30. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/PickupsGenNew28 (Core.hpp).
//
// Banco par a par de UNA funcion: CGarages::StoreCarInNearestImpoundingGarage (0x44A3C0, cdecl, un dword = CVehicle*, `ret` normal, void). El hook RH se llama "StoreCarInNearestImpoundingGarage"
// (clase CGarages). Original = bytes del EXE con el hook apagado; candidato = DLL con SOLO ese hook encendido.
// Estado global: CGarages::aGarages (0x96C048, 50 x 0xD8) y NumGarages (0x96C024), y CGarages::aCarsInSafeHouse (0x96ABD8, 20 x 4 x 0x40) se SUSTITUYEN por los del caso en cada lado (se guardan los
// reales y se restauran al final). El "vehiculo" y su "matriz" son bloques de 0x40 bytes del banco. En ambos lados se sustituyen, SOLO durante la llamada y con el MISMO falso (parche de entrada
// en la direccion del EXE Y en el cuerpo de la DLL, calculado ANTES de parchear; la DLL llama a su FindSafeHouseIndexForGarageType, con el hook por defecto encendido, y al envoltorio de
// CStoredCar::StoreCar, que llama a 0x449760): CGarages::FindSafeHouseIndexForGarageType (0x4489F0) y CStoredCar::StoreCar (0x449760). Cada falso anota (tipo, argumentos) y StoreCar deja una marca
// en la ranura, asi que el orden de las llamadas y la ranura elegida quedan comparados. NO hay colaboradores reales. La palabra de control x87 se fija a 24 bits, al mas cercano y con TODAS las
// excepciones enmascaradas (estado del juego). EAX/ECX/EDX de salida: solo se registran (ningun llamador del EXE los lee, ver README.md).
namespace GaragesImpound30Live {
using namespace GaragesImpound30Fix;
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
// La funcion usa x87 (la DLL tambien, desde X87Math.cpp): los registros ST vacios guardan restos distintos, la palabra de estado lleva banderas de excepcion y codigos de condicion distintos y los
// punteros de instruccion/operando difieren. Se compara lo observable: FCW completa (precision/redondeo/mascaras), TOP de la pila (bits 11-13 de FSW), la palabra de etiquetas y los ST VALIDOS
// (ninguno debe quedar: pila vacia). Se ignoran: banderas de excepcion/condicion de FSW, FIP/FCS/FOP/FDP/FDS y el contenido de los registros marcados como vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): por defecto solo se registran en original_residue/candidate_residue.
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
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile;};
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`.
__declspec(noinline) inline Reg Gate(uintptr_t entryAddress,const u32* argsPtr,u32 argCount,u32 seedEax,u32 seedEcx,u32 seedEdx,u32 stackSeed,u32 popBytes){
    u32 espBefore{},espAfter{},outEax{},outEcx{},outEdx{},outFlags{},ebxAfter{},esiAfter{},ediAfter{},ebpBefore{},ebpAfter{};
    __asm {
        push ebx
        push esi
        push edi
        mov ebpBefore,ebp
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
        pop edi
        pop esi
        pop ebx
    }
    return {outEax,outEcx,outEdx,outFlags,espBefore==espAfter,ebxAfter==0x13579BDFu&&esiAfter==0x2468ACE0u&&ediAfter==0x10293847u&&ebpBefore==ebpAfter};
}
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// ------------------------------------------------------------------------------------------------ memoria de los casos
inline u8* LiveGarages() {return reinterpret_cast<u8*>(uintptr_t(kGarageAddress));}
inline u8* LiveTable() {return reinterpret_cast<u8*>(uintptr_t(kTableAddress));}
inline i32* NumPtr() {return reinterpret_cast<i32*>(uintptr_t(kNumAddress));}
alignas(16) inline std::array<u8,kGarageBytes> refGarages{};    // array de garajes inicial del caso
alignas(16) inline std::array<u8,kGarageBytes> savedGarages{};  // array real del juego, restaurado al final
alignas(16) inline std::array<u8,kTableBytes> refTable{};       // tabla de coches guardados inicial del caso
alignas(16) inline std::array<u8,kTableBytes> savedTable{};     // tabla real del juego, restaurada al final
inline i32 savedNum{};
alignas(16) inline std::array<u8,kVehicleBytes> vehBlock{};
alignas(16) inline std::array<u8,kVehicleBytes> vehRef{};
alignas(16) inline std::array<u8,kMatrixBytes> matBlock{};
alignas(16) inline std::array<u8,kMatrixBytes> matRef{};
inline u32 findMap[3]{};

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
struct Ev {u32 kind,a,b;};
inline std::array<Ev,16> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0) {if(nev<evs.size())evs[nev++]=Ev{k,a,b};}

// Falsos. FindSafeHouseIndexForGarageType (0x4489F0): cdecl, un dword (el EXE lo empuja con movzx del byte de tipo); el original solo toca EAX y el EXE depende de que ECX (= 0) se conserve despues,
// asi que el falso conserva ECX y EDX. CStoredCar::StoreCar (0x449760): thiscall (ECX = ranura), un argumento, `ret 4`.
inline u32 __cdecl RecordFind(u32 type) {
    const u32 t=type&0xFFu;
    Push(1,t,0);
    return t>=0x21u&&t<=0x23u?findMap[t-0x21u]:0u;
}
__declspec(naked) inline int __cdecl FakeFind() {
    __asm {
        push ecx
        push edx
        mov eax,[esp+12]
        push eax
        call RecordFind
        add esp,4
        pop edx
        pop ecx
        ret
    }
}
inline void __fastcall FakeStore(void* self,void*,void* vehicle) {
    const uintptr_t d=uintptr_t(self)-kTableAddress;
    const u32 chunk=d<kTableBytes&&d%kSlotBytes==0?u32(d/kSlotBytes):0xFFFFFFFFu;
    Push(2,chunk,vehicle==static_cast<void*>(vehBlock.data())?1u:u32(uintptr_t(vehicle)));
    if(chunk<kChunks) {
        u8* p=static_cast<u8*>(self);
        const u32 mark=0xC0DE0000u|chunk;const u16 model=0x7777;
        std::memcpy(p,&mark,4);std::memcpy(p+0x12,&model,2);
    }
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;          // 0x44A3C0 y las dos entradas parcheadas: deben seguir siendo el codigo original al volver
    Pair pFind{},pStore{};               // cuerpos de la DLL resueltos ANTES de escribir ningun parche
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,8> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
inline void InstallPatches(Env& e,Patches& p){p.Add(e.pFind,&FakeFind);p.Add(e.pStore,&FakeStore);}
// Ruta: original = hook de la ruta apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los parches de los colaboradores se instalan despues de comprobar la ruta
// y se quitan antes de volver.
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK}};
}
inline void Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ casos
inline void BuildReference(const Fx& f) {
    const u32 n=f.n;
    const size_t N=f.garages.size();
    for(u32 i=0;i<kMaxGarages;++i) {
        u8* g=refGarages.data()+size_t(i)*kEntryBytes;
        FillWords(g,kEntryBytes,Rand(n,1000+i));
        u32 type=g[0x4C],x,y;std::memcpy(&x,g,4);std::memcpy(&y,g+4,4);
        if(i<N) {type=f.garages[i].type;x=f.garages[i].x;y=f.garages[i].y;}
        else if(Rand(n,2000+i)%2) {type=0x22;x=0;y=0;}     // ranuras por encima de NumGarages: tipo de embargo en el origen (si se iteraran, ganarian)
        g[0x4C]=u8(type);std::memcpy(g,&x,4);std::memcpy(g+4,&y,4);
    }
    std::memset(refTable.data(),f.fill,kTableBytes);
    for(const auto& h:f.houses)std::memcpy(refTable.data()+size_t(h.index)*kHouseBytes,h.bytes.data(),kHouseBytes);
    FillWords(vehRef.data(),kVehicleBytes,Rand(n,3000));
    FillWords(matRef.data(),kMatrixBytes,Rand(n,3001));
    const u32* placement=f.vehMatrix?f.vother:f.vpos;
    const u32* matrixPos=f.vehMatrix?f.vpos:f.vother;
    std::memcpy(vehRef.data()+4,placement,12);
    const u32 matrixPtr=f.vehMatrix?u32(uintptr_t(matBlock.data())):0u;
    std::memcpy(vehRef.data()+0x14,&matrixPtr,4);
    std::memcpy(matRef.data()+0x30,matrixPos,12);
    findMap[0]=f.find[0];findMap[1]=f.find[1];findMap[2]=f.find[2];
}
inline json ChangedJson() {
    json out=json::array();
    for(u32 ch=0;ch<kChunks;++ch)
        if(std::memcmp(LiveTable()+size_t(ch)*kSlotBytes,refTable.data()+size_t(ch)*kSlotBytes,kSlotBytes)!=0)
            out.push_back(json::array({ch,HexBytes(LiveTable()+size_t(ch)*kSlotBytes,kSlotBytes)}));
    return out;
}
inline void CaseImpound(Env& env,Totals& t,unsigned n) {
    const Fx f=Make(n);
    const json input=json::parse(InputLine(f));
    BuildReference(f);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(LiveGarages(),refGarages.data(),kGarageBytes);
        std::memcpy(LiveTable(),refTable.data(),kTableBytes);
        *NumPtr()=i32(f.garages.size());
        vehBlock=vehRef;matBlock=matRef;
        nev=0;
        sides[s]=Execute(env,*env.route,0x44A3C0,candidate,[&](uintptr_t entry){
            const u32 args[1]{u32(uintptr_t(vehBlock.data()))};
            return Gate(entry,args,1,f.eax,f.ecx,f.edx,f.stack,4);       // cdecl: el llamador retira el dword
        });
        json trace=json::array();
        for(unsigned i=0;i<nev;++i)trace.push_back(json::array({evs[i].kind,evs[i].a,evs[i].b}));
        const json observed={{"changed",ChangedJson()},{"trace",trace},{"garages_ok",std::memcmp(LiveGarages(),refGarages.data(),kGarageBytes)==0},{"num",*NumPtr()},
                             {"veh_ok",vehBlock==vehRef&&matBlock==matRef}};
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);
        out["observed"]=observed;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,"CGarages::StoreCarInNearestImpoundingGarage",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    ready&=env.pFind.dll&&env.pStore.dll&&ModuleOf(env.pFind.dll)==env.asi&&ModuleOf(env.pStore.dll)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x44A3C0),uintptr_t(0x4489F0),uintptr_t(0x449760)})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    std::memcpy(savedGarages.data(),LiveGarages(),kGarageBytes);std::memcpy(savedTable.data(),LiveTable(),kTableBytes);savedNum=*NumPtr();
    PrepareFloatingState();
    RestoreFloatingState();
    json stubbed=json::array();
    for(const Pair* p:{&env.pFind,&env.pStore})stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},{"cases",{{"CGarages::StoreCarInNearestImpoundingGarage",kCases}}},
           {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
           {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
           {"collaborators",{{"exe_real",json::array()},{"stubbed",stubbed}}},
           {"constants",{{"entry_bytes",kEntryBytes},{"max_garages",kMaxGarages},{"garage_bytes",kGarageBytes},{"garage_address",kGarageAddress},{"num_address",kNumAddress},{"safehouses",kSafehouses},
                         {"slots_per_house",kSlotsPerHouse},{"slot_bytes",kSlotBytes},{"house_bytes",kHouseBytes},{"table_bytes",kTableBytes},{"table_address",kTableAddress},{"chunks",kChunks},
                         {"vehicle_bytes",kVehicleBytes},{"matrix_bytes",kMatrixBytes},{"initial_min",kInitialMin},{"cases",kCases}}},
           {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseImpound(env,totals,c);
        Write({{"event","function_summary"},{"function","CGarages::StoreCarInNearestImpoundingGarage"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    std::memcpy(LiveGarages(),savedGarages.data(),kGarageBytes);std::memcpy(LiveTable(),savedTable.data(),kTableBytes);*NumPtr()=savedNum;   // el estado del juego vuelve a como estaba
    const bool stateBack=std::memcmp(LiveGarages(),savedGarages.data(),kGarageBytes)==0&&std::memcmp(LiveTable(),savedTable.data(),kTableBytes)==0&&*NumPtr()==savedNum;
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&stateBack;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"tables_restored",stateBack}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
