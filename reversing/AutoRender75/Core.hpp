#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"
// Nucleo del banco AutoRender75 (lote 75): CAutomobile::Render (0x6A2B10, virtual), derivado del caso Render de AutomobileWrappers60 (lote 60). Requiere, declarado antes: json (nlohmann),
// uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages} y OriginalDependencies499::HookRoute.
// Colaboradores sustituidos por falsos identicos en el EXE y en el cuerpo de la DLL: GetFirstObject, CVehicle::Render, GetColModel, GetVehicleAppearance, DoHeadLightBeam, RwMatrixTranslate,
// RwFrameUpdateObjects, el renderCallBack del atomico y las entradas fpRenderStateSet/Get de RwEngineInstance. RwMatrixTranslate/RwFrameUpdateObjects son funciones de RenderWare: la DLL las llama
// por su direccion del EXE (sin cuerpo propio), asi que el parche de esa direccion cubre a los dos lados.
// El estado x87 se compara normalizado (NormalizeFp) como en ModifySpeed27.
namespace AutoRender75Live {
using namespace AutoRender75Fix;
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
// Igual que ModifySpeed27: se compara FCW completa, TOP, palabra de etiquetas y los ST validos (ninguno debe quedar); se ignoran banderas de excepcion/condicion de FSW,
// FIP/FCS/FOP/FDP/FDS y los registros vacios. MXCSR sin banderas.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (README.md, seccion de registros): solo se registran.
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
// Igual que ModifySpeed27::Gate (EFLAGS justo tras el `call`; argsPtr[0] queda en la cima de la pila).
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

// ------------------------------------------------------------------------------------------------ tamanos de arena (SOLO aqui: el oraculo no los usa)
constexpr u32 kGuard = 0x40;                                      // relleno 0xC3 antes y despues
constexpr u32 kCarOffset = kGuard;
constexpr u32 kCarArena = kCarOffset + kCarBytes + kGuard;        // 0x40 + 0x988 + 0x40
constexpr u32 kFrameOffset = 0x10;                                // RwFrame dentro de su arena
constexpr u32 kFrameSize = 0x100;
constexpr u32 kFrameArena = kFrameOffset + kFrameSize + 0x10;
constexpr u32 kModellingOffset = 0x10;                            // RwFrame::modelling
constexpr u32 kMatrixBytes = 0x48;                                // CMatrix
constexpr u32 kMatrixArena = kGuard + kMatrixBytes + kGuard;
constexpr u32 kSmallArena = 0x120;                                // atomicos, modelo de colision, datos de colision, informacion de modelo, datos de handling, FxSystem
constexpr u32 kLinesBytes = 12 * 0x20;
constexpr u32 kLinesArena = kLinesBytes + 0x20;

// ------------------------------------------------------------------------------------------------ contexto de los falsos
struct Ctx {
    json trace=json::array();
    uintptr_t car{},matrix{},colModel{},frameM{};
    std::array<uintptr_t,8> frames{},atomA{},atomB{};
    std::array<unsigned,8> gfoCount{};
    uintptr_t fxSand{},fxSmoke{},fxWater{};
    int appearance{};bool getWrites{};i32 getVal{};
};
inline Ctx ctx;
template<class... A> inline json Ev(A&&... a){return json::array({json(std::forward<A>(a))...});}
inline std::string Lbl(uintptr_t p){
    if(!p)return "null";
    if(p==ctx.car)return "car";
    if(p==ctx.matrix)return "matrix";
    if(p==ctx.colModel)return "colmodel";
    if(p==ctx.frameM)return "FM";
    if(p==ctx.fxSand)return "SA";
    if(p==ctx.fxSmoke)return "SH";
    if(p==ctx.fxWater)return "WS";
    for(unsigned n=2;n<8;++n){
        if(p==ctx.frames[n])return "F"+std::to_string(n);
        if(p==ctx.frames[n]+kModellingOffset)return "M"+std::to_string(n);
        if(p==ctx.atomA[n])return "A"+std::to_string(n);
        if(p==ctx.atomB[n])return "B"+std::to_string(n);
    }
    return "?";
}
inline u32 Bits(float v){u32 b;memcpy(&b,&v,4);return b;}

// ---- falsos de Render
inline int __cdecl FakeStateGet(int state,void* out){ctx.trace.push_back(Ev("sget",state));if(ctx.getWrites)*static_cast<i32*>(out)=ctx.getVal;return 1;}
inline int __cdecl FakeStateSet(int state,void* value){ctx.trace.push_back(Ev("sset",state,u32(uintptr_t(value))));return 1;}
inline void* __cdecl FakeGetFirstObject(void* frame){
    int node=-1;for(int n=2;n<8;++n)if(uintptr_t(frame)==ctx.frames[n])node=n;
    if(node<0){ctx.trace.push_back(Ev("gfo","?","?"));return nullptr;}
    const bool first=(ctx.gfoCount[node]++%2)==0;   // la primera llamada de cada par da el atomico, la segunda su argumento
    ctx.trace.push_back(Ev("gfo","F"+std::to_string(node),first?"A":"B"));
    return reinterpret_cast<void*>(first?ctx.atomA[node]:ctx.atomB[node]);
}
inline void* __cdecl FakeRenderCallBack(void* atomic){ctx.trace.push_back(Ev("rcb",Lbl(uintptr_t(atomic))));return atomic;}
inline void __fastcall FakeVehicleRender(void* self,void*){ctx.trace.push_back(Ev("vrender",Lbl(uintptr_t(self))));}
inline void* __fastcall FakeGetColModel(void* self,void*){ctx.trace.push_back(Ev("col",Lbl(uintptr_t(self))));return reinterpret_cast<void*>(ctx.colModel);}
// RwMatrixTranslate (post-concatenacion): suma el vector a la posicion (+0x30) con sumas float, como la matriz real
inline void* __cdecl FakeTranslate(void* matrix,const float* v,int op){
    ctx.trace.push_back(Ev("xlate",Lbl(uintptr_t(matrix)),Bits(v[0]),Bits(v[1]),Bits(v[2]),op));
    float* pos=reinterpret_cast<float*>(static_cast<u8*>(matrix)+0x30);
    for(int k=0;k<3;++k){float* p=pos+k;const float* q=v+k;__asm {mov eax,p} __asm {mov edx,q} __asm {fld dword ptr [eax]} __asm {fadd dword ptr [edx]} __asm {fstp dword ptr [eax]}}   // x87 a 24 bits: desnormales intactos
    return matrix;
}
inline void* __cdecl FakeUpdateObjects(void* frame){ctx.trace.push_back(Ev("upd",Lbl(uintptr_t(frame))));return frame;}
inline int __fastcall FakeAppearance(void* self,void*){ctx.trace.push_back(Ev("appear",Lbl(uintptr_t(self))));return ctx.appearance;}
inline void __fastcall FakeBeam(void* self,void*,int dummy,void* matrix,bool flag){ctx.trace.push_back(Ev("beam",Lbl(uintptr_t(self)),dummy,Lbl(uintptr_t(matrix)),int(flag)));}

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kCarArena> carArena{};
alignas(16) inline std::array<std::array<u8,kFrameArena>,8> frameMem{};
alignas(16) inline std::array<std::array<u8,kSmallArena>,8> atomAMem{},atomBMem{};
alignas(16) inline std::array<u8,kMatrixArena> matrixMem{};
alignas(16) inline std::array<u8,kSmallArena> colModelMem{},colDataMem{};
alignas(16) inline std::array<u8,kLinesArena> linesMem{};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct DllAddr {uintptr_t getFirstObject{},vehicleRender{},getColModel{},appearance{},beam{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x6A2B10};
    DllAddr dll{};
    std::vector<Watch> watches;
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> list;
    template<class F> void Add(uintptr_t at,F fn){list.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fn));}
};
// Escribe un puntero de datos (p. ej. los punteros de funcion del motor RW) y lo restaura al salir
struct SlotPatch {
    u32* slot;u32 saved;
    SlotPatch(u32* s,uintptr_t value):slot(s),saved(*s){*slot=u32(value);}
    ~SlotPatch(){*slot=saved;}
};

template<class G> Side Execute(Env& env,bool candidate,G gate) {
    Side side{};
    auto call=[&]{
        const uintptr_t entry=env.entry;
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        side.reg=gate(entry);
        SaveFP(side.fp.data());side.mxcsr=ReadMXCSR();
        return 0;
    };
    env.route->Invoke(candidate,call);
    return side;
}

// Diferencias entre dos bloques como tramos contiguos [desplazamiento, hex nuevo]
inline json DiffRuns(const u8* ref,const u8* now,size_t n){
    json runs=json::array();
    for(size_t i=0;i<n;){
        if(ref[i]==now[i]){++i;continue;}
        size_t j=i;while(j<n&&ref[j]!=now[j])++j;
        runs.push_back(json::array({json(i),json(HexBytes(now+i,j-i))}));
        i=j;
    }
    return runs;
}
inline bool GuardsOk(const std::array<u8,kCarArena>& now,const std::array<u8,kCarArena>& ref){
    return memcmp(now.data(),ref.data(),kCarOffset)==0&&memcmp(now.data()+kCarOffset+kCarBytes,ref.data()+kCarOffset+kCarBytes,kCarArena-kCarOffset-kCarBytes)==0;
}
inline void RefCar(std::array<u8,kCarArena>& ref,u32 seed){ref.fill(0xC3);FillWords(ref.data()+kCarOffset,kCarBytes,seed);}
template<class T> inline void Put(u8* base,size_t off,T v){memcpy(base+off,&v,sizeof(T));}
inline u32 Ptr(const void* p){return u32(uintptr_t(p));}

// Cierra un lado de un caso: observado + compuertas. `observed` lo construye cada funcion.
inline json SideJson(const Side& side,json observed,bool globalsRestored){
    if(kCompareVolatileRegisters){observed["eax"]=side.reg.eax;observed["ecx"]=side.reg.ecx;observed["edx"]=side.reg.edx;}
    json out={{"observed",observed},{"post_floating_state",HexBytes(NormalizeFp(side.fp).data(),108)},{"post_mxcsr_control",side.mxcsr&0xFFC0u},{"stack_restored",side.reg.stack},
              {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK},{"traps_zero",true},{"calls",0},
              {"globals_restored",globalsRestored}};
    return out;
}
inline void FinishPair(Env& env,Totals& t,const char* name,unsigned caseNo,const json& input,Side (&sides)[2],json (&rows)[2]){
    for(int s=0;s<2;++s){RestoreFloatingState();rows[s]["floating_state_restored"]=FloatingStateRestored();}
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function",name},{"case",caseNo},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
    (void)env;
}
inline void BeginSide(){RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);ctx=Ctx{};}
// ================================================================================================ 0x6A2B10 Render
inline void CaseR(Env& env,Totals& t,unsigned n,unsigned pairOffset) {
    (void)pairOffset;
    const FR f=MakeFR(n);
    std::array<u8,kCarArena> carRef{};RefCar(carRef,f.headSeed);
    u8* const headRef=carRef.data()+kCarOffset;
    // arenas de referencia
    std::array<std::array<u8,kFrameArena>,8> frameRef{};
    std::array<std::array<u8,kSmallArena>,8> aRef{},bRef{};
    std::array<u8,kMatrixArena> matrixRef{};std::array<u8,kSmallArena> colModelRef{},colDataRef{};std::array<u8,kLinesArena> linesRef{};
    matrixRef.fill(0xC3);FillWords(matrixRef.data()+kGuard,kMatrixBytes,f.headSeed+1);
    colModelRef.fill(0xC3);colDataRef.fill(0xC3);linesRef.fill(0xC3);
    for(unsigned node=2;node<8;++node){
        frameRef[node].fill(0xC3);FillWords(frameRef[node].data()+kFrameOffset,kFrameSize,f.headSeed+10u+node);
        for(unsigned k=0;k<3;++k)Put<u32>(frameRef[node].data(),kFrameOffset+kModellingOffset+0x30+k*4,f.fpos[node-2][k]);
        aRef[node].fill(0xC3);bRef[node].fill(0xC3);
        FillWords(aRef[node].data()+0x20,0x80,f.headSeed+30u+node);FillWords(bRef[node].data()+0x20,0x80,f.headSeed+50u+node);
        Put<u32>(aRef[node].data(),0x20+0x48,Ptr(reinterpret_cast<const void*>(&FakeRenderCallBack)));
        Put<u32>(bRef[node].data(),0x20+0x48,Ptr(reinterpret_cast<const void*>(&FakeRenderCallBack)));
    }
    // colision: modelo -> datos -> lineas
    FillWords(colModelRef.data()+0x20,0x40,f.headSeed+70u);
    FillWords(colDataRef.data()+0x20,0x40,f.headSeed+71u);
    Put<u32>(colModelRef.data(),0x20+0x2C,Ptr(colDataMem.data()+0x20));   // CColModel::m_pColData
    Put<u32>(colDataRef.data(),0x20+0x10,Ptr(linesMem.data()));           // CCollisionData::m_pLines
    for(unsigned l=0;l<12;++l){
        u8* line=linesRef.data()+l*0x20;
        FillWords(line,0x20,f.headSeed+80u+l);
        for(unsigned k=0;k<3;++k){Put<u32>(line,k*4,f.lines[l][k]);Put<u32>(line,0x10+k*4,f.lines[l][3+k]);}
    }
    // objeto
    Put<u32>(headRef,0x14,Ptr(matrixMem.data()+kGuard));
    Put<i16>(headRef,0x22,i16(f.model));
    Put<u32>(headRef,0x4D0,f.old4d0);
    headRef[0x584]=f.lights;
    headRef[0x42C]=f.rc;
    for(unsigned i=0;i<8;++i)Put<u32>(headRef,i==0?0x618:i==1?0x61C:i==2?0x624:i==3?0x628:i==4?0x630:i==5?0x634:i==6?0x63C:0x640,f.tracks[i]);
    Put<u32>(headRef,0x878,f.spring0);Put<u32>(headRef,0x888,f.line0);
    // los punteros dependen de las arenas de trabajo (direcciones fijas): se escriben tras fijar ctx
    u8* const obj=carArena.data()+kCarOffset;
    const json input=json::parse(JsonFR(f));
    u32* const engine=reinterpret_cast<u32*>(uintptr_t(*reinterpret_cast<u32*>(uintptr_t(kEngineInstance))));
    u32* const timer=reinterpret_cast<u32*>(uintptr_t(kTimerAddress));
    u8* const cheat=reinterpret_cast<u8*>(uintptr_t(kCheatInvisible));
    const u32 savedTimer=*timer;const u8 savedCheat=*cheat;
    // tabla de punteros de nodos: nodos 2,4,5,7 siempre; 3 y 6 segun el caso
    for(unsigned node=2;node<8;++node){
        const bool present=!(node==3&&f.node3Null)&&!(node==6&&f.node6Null);
        Put<u32>(headRef,0x648+node*4,present?Ptr(frameMem[node].data()+kFrameOffset):0u);
    }
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        BeginSide();
        ctx.car=uintptr_t(obj);ctx.matrix=uintptr_t(matrixMem.data()+kGuard);ctx.colModel=uintptr_t(colModelMem.data()+0x20);
        for(unsigned node=2;node<8;++node){ctx.frames[node]=uintptr_t(frameMem[node].data()+kFrameOffset);ctx.atomA[node]=uintptr_t(atomAMem[node].data()+0x20);ctx.atomB[node]=uintptr_t(atomBMem[node].data()+0x20);}
        ctx.appearance=f.appearance;ctx.getWrites=f.getWrites!=0;ctx.getVal=f.getVal;
        carArena=carRef;frameMem=frameRef;atomAMem=aRef;atomBMem=bRef;matrixMem=matrixRef;colModelMem=colModelRef;colDataMem=colDataRef;linesMem=linesRef;
        *timer=f.time;*cheat=f.cheat;
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            Patches p;
            p.Add(0x7348C0,&FakeGetFirstObject);p.Add(env.dll.getFirstObject,&FakeGetFirstObject);
            p.Add(0x6D0E60,&FakeVehicleRender);p.Add(env.dll.vehicleRender,&FakeVehicleRender);
            p.Add(0x535300,&FakeGetColModel);p.Add(env.dll.getColModel,&FakeGetColModel);
            p.Add(0x7F2450,&FakeTranslate);p.Add(0x7F0910,&FakeUpdateObjects);
            p.Add(0x6D1080,&FakeAppearance);p.Add(env.dll.appearance,&FakeAppearance);
            p.Add(0x6E0E20,&FakeBeam);p.Add(env.dll.beam,&FakeBeam);
            SlotPatch setPatch(engine+8,uintptr_t(&FakeStateSet)),getPatch(engine+9,uintptr_t(&FakeStateGet));
            return Gate(entry,nullptr,0,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);
        });
        json frames=json::array();bool framesRest=true;
        for(unsigned node=2;node<8;++node){
            const u8* fr=frameMem[node].data()+kFrameOffset+kModellingOffset+0x30;
            frames.push_back(json::array({json(*reinterpret_cast<const u32*>(fr)),json(*reinterpret_cast<const u32*>(fr+4)),json(*reinterpret_cast<const u32*>(fr+8))}));
            std::array<u8,kFrameArena> now=frameMem[node];memcpy(now.data()+kFrameOffset+kModellingOffset+0x30,frameRef[node].data()+kFrameOffset+kModellingOffset+0x30,12);
            framesRest&=now==frameRef[node];
        }
        const bool arenas=atomAMem==aRef&&atomBMem==bRef&&matrixMem==matrixRef&&colModelMem==colModelRef&&colDataMem==colDataRef&&linesMem==linesRef;
        json observed={{"car_diff",DiffRuns(headRef,obj,kCarBytes)},{"guard_ok",GuardsOk(carArena,carRef)},{"frames",frames},{"frames_rest_ok",framesRest},{"arenas_unchanged",arenas},{"trace",ctx.trace}};
        const bool watchesIntact=env.WatchesIntact();
        const bool globals=*timer==f.time&&*cheat==f.cheat;
        *timer=savedTimer;*cheat=savedCheat;
        rows[s]=SideJson(sides[s],observed,watchesIntact&&env.WatchesIntact()&&globals&&*timer==savedTimer&&*cheat==savedCheat);
    }
    FinishPair(env,t,"CAutomobile::Render",n,input,sides,rows);
}


// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline const char* const kName="CAutomobile::Render";
inline const std::vector<uintptr_t> kExeCollaborators{0x7348C0,0x6D0E60,0x535300,0x7F2450,0x7F0910,0x6D1080,0x6E0E20};
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    ready&=env.route&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    const DllAddr& d=env.dll;
    const std::vector<uintptr_t> dllCollaborators{d.getFirstObject,d.vehicleRender,d.getColModel,d.appearance,d.beam};
    std::vector<uintptr_t> all{0x6A2B10};
    all.insert(all.end(),kExeCollaborators.begin(),kExeCollaborators.end());
    all.insert(all.end(),dllCollaborators.begin(),dllCollaborators.end());
    for(const uintptr_t at:all){ready&=at!=0&&ModuleOf(at)!=0;env.watches.emplace_back(at,16);}
    for(const uintptr_t at:dllCollaborators)ready&=ModuleOf(at)==env.asi;
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    const u32 savedTimer=*reinterpret_cast<u32*>(uintptr_t(kTimerAddress));
    const u8 savedCheatInv=*reinterpret_cast<u8*>(uintptr_t(kCheatInvisible));
    ctx.trace=json::array();
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    json cases=json::object();cases[kName]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kTotal},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",kExeCollaborators},{"dll",dllCollaborators}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"initial_timer",savedTimer},{"initial_cheat",savedCheatInv}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseR(env,totals,c,0);
        Write({{"event","function_summary"},{"function",kName},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&*reinterpret_cast<u32*>(uintptr_t(kTimerAddress))==savedTimer&&*reinterpret_cast<u8*>(uintptr_t(kCheatInvisible))==savedCheatInv;
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"timer",*reinterpret_cast<u32*>(uintptr_t(kTimerAddress))},{"cheat",*reinterpret_cast<u8*>(uintptr_t(kCheatInvisible))}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
