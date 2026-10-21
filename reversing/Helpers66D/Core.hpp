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

// Nucleo del banco Helpers66D. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, CHeli, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y las cabeceras de ReversibleHook (Base/Simple/Virtual). Plantillas: reversing/Helpers65C (constructor con vptr propio, falsos en el EXE y en la DLL,
// trazas planas) y reversing/Helpers65A (Gate de ABI).
//
// Banco par a par de CUATRO funciones (4096 casos cada una). Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con el hook encendido.
//   0 RwHelper::RpClumpGetBoundingSphere 0x734FC0 (cdecl)       1 CGarage::FindDoorsWithGarage 0x449FF0 (thiscall, ret 8)
//   2 CCam::GetArrestCamCopSourceSide 0x516010 (thiscall, ret 10h)   3 CHeli::CHeli 0x6C4190 (constructor thiscall, ret 8)
// Colaboradores SUSTITUIDOS por falsos identicos en ambos lados (parche de entrada de 5 bytes en la direccion del EXE y en el cuerpo de la DLL, solo durante la llamada); todos registran en
// una traza plana (tipo de evento + argumentos leidos en el momento de la llamada) que se compara entera con el oraculo:
//   Clump:  RpClumpGetNumAtomics 0x7498E0, RpClumpForAllAtomics 0x749B70 (los dos callbacks 0x734970 y 0x734ED0 se reconocen por direccion y NO se ejecutan: el falso escribe lo que dicta el caso),
//           RwFrameGetLTM 0x7F0990, RwMatrixInvert 0x7F2070, RwV3dTransformPoints 0x7EDD90. El global 0x8D60BC (byte bUseLTM) se guarda y se restaura.
//   Garage: ninguno; el pool de objetos (puntero 0xB7449C) se sustituye por uno de la arena durante la llamada y se restaura.
//   Cop:    CVector::Normalise 0x59C910, CrossProduct(out) 0x59C730, CWorld::FindGroundZFor3DCoord 0x5696C0.
//   Heli:   CAutomobile::CAutomobile 0x6B0A90 (ruido + vptr marcador), CDamageManager::SetDoorStatus 0x6C21C0 (cuerpo de la DLL: el metodo), rand() del CRT del EXE 0x821B1E. El
//           global CTimer::m_snTimeInMilliseconds 0xB7CB84 se fija por caso y se restaura.
// ABI: ESP, EBX/ESI/EDI/EBP, DF, cadena SEH (fs:[0]) y x87 (FCW 24 bits, pila vacia). EAX/ECX/EDX de salida: solo registrados, salvo EAX = this en el constructor y AL en la funcion de la
// camara (los leen los llamadores del EXE) y EAX = clump en la esfera (se compara el valor devuelto).
namespace H66Live {
using namespace H66Fix;
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
// El estado x87 se compara normalizado (FCW completa, TOP, etiquetas y ST VALIDOS: ninguno debe quedar). Los registros muertos no se comparan.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Los llamadores del EXE solo leen EAX (constructor: this; esfera: clump) y AL (camara): se exigen aparte; el resto solo se registra.
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
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; argsPtr[0] queda en la cima de la pila; captura los registros y EFLAGS justo tras el `call`, retira popBytes (0 en `ret n`, 4*n en
// una funcion cdecl) y comprueba ESP, EBX/ESI/EDI/EBP y la cadena SEH (fs:[0]).
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
alignas(16) inline std::array<u8,0x1740> arena{};      // la mayor es la del garaje (0x1740)
inline u32 arenaSize{};
inline std::vector<u32> tr;                            // traza plana de los colaboradores
inline u8* AB(){return arena.data();}
inline u32 Tag(const void* p){const auto a=uintptr_t(p);if(!a)return 0;const auto b=uintptr_t(AB());if(a>=b&&a<b+arenaSize)return 1u+u32(a-b);return 0xFFFFFFFEu;}
inline u32 Dw(const void* self,size_t at){u32 v{};std::memcpy(&v,static_cast<const u8*>(self)+at,4);return v;}
inline void PutDw(void* self,size_t at,u32 v){std::memcpy(static_cast<u8*>(self)+at,&v,4);}
inline void P(std::initializer_list<u32> v){if(tr.size()<512)for(const u32 x:v)tr.push_back(x);}
inline u32 Fnv(const void* p,u32 n){u32 h=2166136261u;const auto* b=static_cast<const u8*>(p);for(u32 i=0;i<n;++i){h^=b[i];h*=16777619u;}return h;}
inline u32 FpAddr(u32 a){return a;}
inline bool curCandidate{};
inline u32 dllHeliVptr{};                              // vptr que escribe el constructor de la DLL (capturado una vez)
inline u32 ExpectedVptr(bool candidate){return candidate?dllHeliVptr:kExeHeliVptr;}
inline u32 VtClass(u32 v){return v==ExpectedVptr(curCandidate)?kVtLabel:v;}

// ---- 0: RpClumpGetBoundingSphere
inline const ClumpCase* curClump{};
inline int __cdecl FkGetNum(void* clump){                                                              // 0x7498E0
    P({1,Tag(clump)});
    return curClump->count;
}
inline void* __cdecl FkForAll(void* clump,u32 cb,void* data){                                          // 0x749B70
    const u32 label=cb==0x734970u?1u:cb==0x734ED0u?2u:3u;
    u32 w[4]{};
    std::memcpy(w,data,label==2?16:12);
    P({2,Tag(clump),label,Tag(data),u32(*reinterpret_cast<const u8*>(0x8D60BC)),w[0],w[1],w[2],w[3]});
    if(label==1){for(int i=0;i<3;++i)PutDw(data,4*i,curClump->sum[i]);}
    else if(label==2){
        if(curClump->radMode)PutDw(data,12,curClump->rad);
        if(curClump->cmodMode)for(int i=0;i<3;++i)PutDw(data,4*i,curClump->cmod[i]);
    }
    return reinterpret_cast<void*>(uintptr_t(0xDEAD0000u|label));
}
inline void* __cdecl FkGetLtm(void* frame){                                                            // 0x7F0990
    P({3,Tag(frame)});
    return AB()+kLtmOff;
}
inline u32* __cdecl FkInvert(u32* out,const u32* in){                                                  // 0x7F2070
    P({4,Tag(out),Tag(in),Fnv(in,0x40)});
    for(u32 i=0;i<16;++i)out[i]=in[i]^(curClump->invMask^(0x01010101u*i));
    return out;
}
inline u32* __cdecl FkTransform(u32* out,const u32* in,u32 n,const u32* mat){                          // 0x7EDD90
    P({5,u32(out==in),n,in[0],in[1],in[2],Fnv(mat,0x40)});
    const u32 t[3]{in[0]^curClump->trMask[0],in[1]^curClump->trMask[1],in[2]^curClump->trMask[2]};
    out[0]=t[0];out[1]=t[1];out[2]=t[2];
    return out;
}

// ---- 2: CCam::GetArrestCamCopSourceSide
inline const CopCase* curCop{};
inline void __fastcall FkNormalise(u32* self,void*){                                                   // 0x59C910 (thiscall, ret)
    P({1,Tag(self),self[0],self[1],self[2]});
    if(curCop->nrmMode){self[0]=curCop->nrm[0];self[1]=curCop->nrm[1];self[2]=curCop->nrm[2];}
}
inline u32* __cdecl FkCross(u32* out,const u32* a,const u32* b){                                       // 0x59C730
    P({2,Tag(out),a[0],a[1],a[2],b[0],b[1],b[2]});
    out[0]=curCop->cross[0];out[1]=curCop->cross[1];out[2]=curCop->cross[2];
    return out;
}
inline float __cdecl FkGround(u32 x,u32 y,u32 z,u8* found,void* entity){                              // 0x5696C0 (float en ST0)
    P({3,x,y,z,Tag(found),u32(uintptr_t(entity))});
    if(curCop->foundMode)*found=u8(curCop->foundByte);
    return BitsF(curCop->ground);
}

// ---- 3: CHeli::CHeli
inline const HeliCase* curHeli{};
inline u8* curObj{};
inline u32 ObjHash(){
    std::array<u8,kHeliBytes> copy{};
    std::memcpy(copy.data(),curObj,kHeliBytes);
    const u32 v=Dw(copy.data(),0);
    if(v==ExpectedVptr(curCandidate))PutDw(copy.data(),0,kVtLabel);
    return Fnv(copy.data(),kHeliBytes);
}
inline void* __fastcall FkAutoCtor(void* self,void*,u32 model,u32 createdBy,u32 flag){                // 0x6B0A90 (thiscall, ret 0Ch)
    P({1,Tag(self),model,createdBy,flag});
    FillWords(static_cast<u8*>(self),kHeliBytes,curHeli->seedP);
    PutDw(self,0,kBaseVptrMarker);
    return self;
}
inline void __fastcall FkSetDoor(void* self,void*,u32 door,u32 status){                               // 0x6C21C0 / metodo de la DLL (thiscall, ret 8)
    P({2,Tag(self),door,status,ObjHash()});
    PutDw(self,0,kSetDoorPattern0);PutDw(self,4,kSetDoorPattern1);
}
inline u32 __cdecl FkRand(){                                                                           // 0x821B1E
    P({3,ObjHash()});
    return curHeli->randRet;
}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI: leccion 2 de BRIEF60).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn){
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// Cuerpo de la DLL de una funcion con hook S (Simple) o V (virtual).
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

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
inline constexpr unsigned kFunc=kFunctions;
inline constexpr const char* kNames[kFunc]{"RwHelper::RpClumpGetBoundingSphere","CGarage::FindDoorsWithGarage","CCam::GetArrestCamCopSourceSide","CHeli::CHeli"};
inline constexpr uintptr_t kEntries[kFunc]{0x734FC0,0x449FF0,0x516010,0x6C4190};
struct Collab {Pair pair;void* fake;bool hooked;};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route[kFunc];
    std::vector<Collab> collabs[kFunc];                // colaboradores sustituidos por funcion
    std::vector<Watch> watches;                         // entradas del EXE y cuerpos de la DLL de las funciones y de los colaboradores: deben seguir intactos al volver
    uintptr_t dllHeliCtor{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
// Parches de entrada activos SOLO durante la llamada (RAII, orden inverso).
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,24> p;int n{};
    void Add(const Pair& pr,void* stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};

struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los falsos se instalan DESPUES de comprobar rutas y colaboradores y solo mientras
// dura la llamada; los globales se comprueban DESPUES de quitar los parches (leccion 3 de BRIEF60).
template<class Fn> Side Execute(Env& env,unsigned func,bool candidate,Fn gate) {
    Side side{};
    const uintptr_t entry=kEntries[func];
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& c:env.collabs[func])collab&=c.pair.dll==0?!ReversingProbe::JumpsToDll(c.pair.exe):ModuleOf(c.pair.dll)==env.asi;
        side.collabOK=collab;
        {
            Patches patches;
            for(const auto& c:env.collabs[func])patches.Add(c.pair,c.fake);
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"seh_chain_restored",side.reg.seh},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"route_ok",side.routeOK},{"collaborators_dll",side.collabOK}};
}
inline json Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","seh_chain_restored","direction_flag_clear","route_ok","collaborators_dll","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    return row;
}
inline json Trace(){json a=json::array();for(const u32 v:tr)a.push_back(v);return a;}
// El resto de la arena (todo menos los rangos [lo,hi) indicados) debe quedar igual que la imagen de referencia.
inline bool RestOk(const u8* ref,u32 size,std::initializer_list<std::pair<u32,u32>> ranges){
    for(u32 i=0;i<size;++i){
        bool skip=false;for(const auto& r:ranges)skip|=i>=r.first&&i<r.second;
        if(!skip&&AB()[i]!=ref[i])return false;
    }
    return true;
}
inline bool GuardsOk(const u8* ref,u32 size){return std::memcmp(AB(),ref,kGuard)==0&&std::memcmp(AB()+size-kGuard,ref+size-kGuard,kGuard)==0;}

// ------------------------------------------------------------------------------------------------ globales del juego que tocan las funciones
struct Globals {
    u8 useLtm{};u32 pool{};u32 timer{};
    void Save(){useLtm=*reinterpret_cast<u8*>(0x8D60BC);pool=*reinterpret_cast<u32*>(uintptr_t(kObjectPool));timer=*reinterpret_cast<u32*>(uintptr_t(kTimerAddr));}
    void Restore()const{*reinterpret_cast<u8*>(0x8D60BC)=useLtm;*reinterpret_cast<u32*>(uintptr_t(kObjectPool))=pool;*reinterpret_cast<u32*>(uintptr_t(kTimerAddr))=timer;}
    bool Equal(const Globals& o)const{return useLtm==o.useLtm&&pool==o.pool&&timer==o.timer;}
};
inline Globals initialGlobals;   // valores del juego antes del banco

// ------------------------------------------------------------------------------------------------ 0: esfera envolvente
inline json ClumpInput(const ClumpCase& f,const u8* ref){
    return {{"func",kClump},{"n",f.n},{"family",f.family},{"clump_kind",f.clumpKind},{"sphere_kind",f.sphereKind},{"use_ltm",f.useLtm},{"count",f.count},{"sum",std::vector<u32>{f.sum[0],f.sum[1],f.sum[2]}},
            {"rad_mode",f.radMode},{"rad",f.rad},{"cmod_mode",f.cmodMode},{"cmod",std::vector<u32>{f.cmod[0],f.cmod[1],f.cmod[2]}},{"parent_kind",f.parentKind},{"inv_mask",f.invMask},
            {"tr_mask",std::vector<u32>{f.trMask[0],f.trMask[1],f.trMask[2]}},{"pre_sphere",HexBytes(ref+kSphereOff,0x10)},{"frame_mat",HexBytes(ref+kFrameOff+0x10,0x40)},{"ltm",HexBytes(ref+kLtmOff,0x40)},
            {"mode",f.mode},{"seed",f.seedArena},{"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseClump(Env& env,Totals& t,unsigned n) {
    const ClumpCase f=MakeClump(n);
    arenaSize=kClumpArena;
    std::array<u8,kClumpArena> ref{};
    const u32 base=u32(uintptr_t(AB()));
    BuildClumpRef(f,base,ref.data());
    const json input=ClumpInput(f,ref.data());
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(AB(),ref.data(),kClumpArena);
        tr.clear();curCandidate=candidate;curClump=&f;
        initialGlobals.Restore();
        const u32 args[3]{f.clumpKind?base+kClumpOff:0u,f.sphereKind?base+kSphereOff:0u,f.useLtm};
        sides[s]=Execute(env,kClump,candidate,[&](uintptr_t entry){return Gate(entry,args,3,f.eax,f.ecx,f.edx,f.stack,12);});   // cdecl: el llamador retira 12 bytes
        Globals after;after.Save();
        initialGlobals.Restore();
        json observed={{"trace",Trace()},{"sphere",f.sphereKind?HexBytes(AB()+kSphereOff,0x10):std::string()},{"ret",Tag(reinterpret_cast<void*>(uintptr_t(sides[s].reg.eax)))},
                       {"use_ltm_after",u32(after.useLtm)},{"pool_unchanged",after.pool==initialGlobals.pool&&after.timer==initialGlobals.timer},
                       {"rest_ok",RestOk(ref.data(),kClumpArena,{{kSphereOff,kSphereEnd}})},{"guard_ok",GuardsOk(ref.data(),kClumpArena)}};
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        Globals now;now.Save();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&now.Equal(initialGlobals);
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    curClump=nullptr;
    Finish(t,kNames[kClump],n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ 1: puertas del garaje
inline json GarageInput(const GarageCase& f,u32 thisAddr,const u8* garageBytes,u32 gb,const u8* ref){
    json slots=json::array();
    for(u32 j=0;j<kMaxSlots;++j){
        const DoorSlot& s=f.s[j];
        const u8* obj=ref+kObjOff+j*kObjBytes;
        slots.push_back({{"flag",ref[kFlagsOff+j]},{"mat",s.matKind},{"pos",std::vector<u32>{s.pos[0],s.pos[1],s.pos[2]}},{"other",std::vector<u32>{s.other[0],s.other[1],s.other[2]}},{"byte",u32(obj[kObjByte])}});
    }
    return {{"func",kGarage},{"n",f.n},{"family",f.family},{"garage_kind",f.garageKind},{"garage_addr",thisAddr},{"garage_bytes",HexBytes(garageBytes,kGarageReadBytes)},{"gb",gb},{"size",f.size},
            {"base_mode",f.baseMode},{"null_slot",f.nullSlot},{"slots",slots},{"first_pre",f.firstPre},{"second_pre",f.secondPre},{"mode",f.mode},{"seed",f.seedArena},
            {"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseGarage(Env& env,Totals& t,unsigned n) {
    const GarageCase f=MakeGarage(n);
    arenaSize=kGarageArena;
    std::array<u8,kGarageArena> ref{};
    const u32 base=u32(uintptr_t(AB()));
    u32 thisAddr{},gb{};
    BuildGarageRef(f,base,ref.data(),thisAddr,gb);
    u8 garageBytes[kGarageReadBytes];
    if(f.garageKind==0)std::memcpy(garageBytes,ref.data()+kGarageOff+f.garageDelta,kGarageReadBytes);else std::memcpy(garageBytes,reinterpret_cast<const void*>(uintptr_t(f.rdata)),kGarageReadBytes);
    const json input=GarageInput(f,thisAddr,garageBytes,gb,ref.data());
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(AB(),ref.data(),kGarageArena);
        tr.clear();curCandidate=candidate;
        initialGlobals.Restore();
        const u32 args[2]{base+kDoorsOff,base+kDoorsOff+4};
        const u32 poolPtr=base+kPoolOff;
        sides[s]=Execute(env,kGarage,candidate,[&](uintptr_t entry){
            *reinterpret_cast<u32*>(uintptr_t(kObjectPool))=poolPtr;                                   // el pool de objetos de la arena, solo durante la llamada
            const Reg r=Gate(entry,args,2,f.eax,thisAddr,f.edx,f.stack,0);
            *reinterpret_cast<u32*>(uintptr_t(kObjectPool))=initialGlobals.pool;
            return r;
        });
        Globals after;after.Save();
        json observed={{"first",Tag(reinterpret_cast<void*>(uintptr_t(Dw(AB(),kDoorsOff))))},{"second",Tag(reinterpret_cast<void*>(uintptr_t(Dw(AB(),kDoorsOff+4))))},
                       {"globals_unchanged",after.Equal(initialGlobals)},{"rest_ok",RestOk(ref.data(),kGarageArena,{{kDoorsOff,kDoorsOff+8}})},{"guard_ok",GuardsOk(ref.data(),kGarageArena)}};
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        Globals now;now.Save();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&now.Equal(initialGlobals);
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,kNames[kGarage],n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ 2: posicion de la camara de arresto
inline json CopInput(const CopCase& f,const u8* ref){
    auto v3=[](const u32* a){return std::vector<u32>{a[0],a[1],a[2]};};
    return {{"func",kCop},{"n",f.n},{"family",f.family},{"target_kind",f.targetKind},{"cop_kind",f.copKind},{"mat_kind",f.matKind},{"cop_pos",v3(f.copPos)},{"cop_other",v3(f.copOther)},{"dest",v3(f.dest)},
            {"source_pre",v3(f.sourcePre)},{"source_w3",Dw(ref,kSourceOff+12)},{"nrm_mode",f.nrmMode},{"nrm",v3(f.nrm)},{"cross",v3(f.cross)},{"ground",f.ground},{"found_mode",f.foundMode},{"found_byte",f.foundByte},
            {"mode",f.mode},{"seed",f.seedArena},{"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseCop(Env& env,Totals& t,unsigned n) {
    const CopCase f=MakeCop(n);
    arenaSize=kCopArena;
    std::array<u8,kCopArena> ref{};
    const u32 base=u32(uintptr_t(AB()));
    BuildCopRef(f,base,ref.data());
    const json input=CopInput(f,ref.data());
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(AB(),ref.data(),kCopArena);
        tr.clear();curCandidate=candidate;curCop=&f;
        const u32 args[4]{f.targetKind?kTargetFake:0u,f.copKind?base+kCopOff:0u,base+kDestOff,base+kSourceOff};
        sides[s]=Execute(env,kCop,candidate,[&](uintptr_t entry){return Gate(entry,args,4,f.eax,f.ecx,f.edx,f.stack,0);});     // `ret 10h`
        json observed={{"trace",Trace()},{"source",std::vector<u32>{Dw(AB(),kSourceOff),Dw(AB(),kSourceOff+4),Dw(AB(),kSourceOff+8)}},{"ret_al",sides[s].reg.eax&0xFFu},
                       {"rest_ok",RestOk(ref.data(),kCopArena,{{kSourceOff,kSourceOff+12}})},{"guard_ok",GuardsOk(ref.data(),kCopArena)}};
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    curCop=nullptr;
    Finish(t,kNames[kCop],n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ 3: constructor de CHeli
inline json HeliInput(const HeliCase& f){
    return {{"func",kHeli},{"n",f.n},{"family",f.family},{"model",f.model},{"created_by",f.createdBy},{"timer",f.timer},{"rand_ret",f.randRet},{"obj_off",f.objOff},{"mode",f.mode},{"seed",f.seedArena},
            {"seed_p",f.seedP},{"eax",f.eax},{"edx",f.edx},{"stack_seed",f.stack}};
}
inline void CaseHeli(Env& env,Totals& t,unsigned n) {
    const HeliCase f=MakeHeli(n);
    arenaSize=kHeliArena;
    std::array<u8,kHeliArena> ref{};
    BuildHeliRef(f,ref.data());
    const json input=HeliInput(f);
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        std::memcpy(AB(),ref.data(),kHeliArena);
        u8* const obj=AB()+f.objOff;
        tr.clear();curCandidate=candidate;curHeli=&f;curObj=obj;
        initialGlobals.Restore();
        const u32 args[2]{f.model,f.createdBy};
        sides[s]=Execute(env,kHeli,candidate,[&](uintptr_t entry){
            *reinterpret_cast<u32*>(uintptr_t(kTimerAddr))=f.timer;
            const Reg r=Gate(entry,args,2,f.eax,u32(uintptr_t(obj)),f.edx,f.stack,0);              // `ret 8`
            *reinterpret_cast<u32*>(uintptr_t(kTimerAddr))=initialGlobals.timer;
            return r;
        });
        Globals after;after.Save();
        std::array<u8,kHeliBytes> ob{};std::memcpy(ob.data(),obj,kHeliBytes);
        const u32 vptr=Dw(ob.data(),0);
        const bool vptrOwn=vptr==ExpectedVptr(candidate);
        if(vptrOwn)PutDw(ob.data(),0,kVtLabel);
        bool guards=true;
        for(size_t i=0;i<kHeliArena;++i){if(i>=f.objOff&&i<f.objOff+kHeliBytes)continue;if(AB()[i]!=ref[i]){guards=false;break;}}
        json observed={{"obj",HexBytes(ob.data(),kHeliBytes)},{"vptr_own",vptrOwn},{"guard_ok",guards},{"trace",Trace()},{"eax_is_this",sides[s].reg.eax==u32(uintptr_t(obj))},{"globals_unchanged",after.Equal(initialGlobals)}};
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[s],fp);out["observed"]=observed;
        RestoreFloatingState();
        Globals now;now.Save();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&now.Equal(initialGlobals);
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    curHeli=nullptr;curObj=nullptr;
    Finish(t,kNames[kHeli],n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ vptr de la DLL
// Construye (en un buffer propio, con los falsos instalados) un CHeli con el constructor de la DLL y comprueba: la traza es [CAutomobile(), (sin SetDoorStatus: modelo 0), rand], el vptr final
// esta en el modulo ASI, no es el del EXE ni el marcador del falso base.
inline void InstallHeliPatches(Env& e,Patches& p){for(const auto& c:e.collabs[kHeli])p.Add(c.pair,c.fake);}
inline bool CaptureDllVptr(Env& env,json& info) {
    alignas(16) static u8 buf[kHeliBytes+0x40]{};
    u32 v{};
    HeliCase scratch{};scratch.seedP=1;scratch.randRet=0;
    {
        Patches patches;
        InstallHeliPatches(env,patches);
        arenaSize=0;
        curCandidate=true;curHeli=&scratch;curObj=buf;tr.clear();
        std::memset(buf,0,sizeof buf);
        (void)::new(static_cast<void*>(buf)) CHeli(0,eVehicleCreatedBy(1));
        std::memcpy(&v,buf,4);
    }
    const bool shape=tr.size()>=2&&tr[0]==1&&tr[tr.size()-2]==3;
    info={{"vptr",v},{"in_asi",ModuleOf(v)==env.asi},{"trace_words",tr.size()},{"first_event",tr.empty()?0u:tr[0]}};
    tr.clear();curHeli=nullptr;curObj=nullptr;
    dllHeliVptr=v;
    return shape&&v!=0&&v!=kExeHeliVptr&&v!=kBaseVptrMarker&&ModuleOf(v)==env.asi;
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& resolved) {
    for(unsigned f=0;f<kFunc;++f){const uintptr_t body=env.route[f]?env.route[f]->Body():0;ready&=body&&ModuleOf(body)==env.asi;}
    json collaborators=json::object();
    for(unsigned f=0;f<kFunc;++f){
        json list=json::array();
        for(const auto& c:env.collabs[f]){
            list.push_back({{"exe",c.pair.exe},{"dll",c.pair.dll},{"hooked",c.hooked}});
            const bool known=std::ranges::any_of(env.watches,[&](const Watch& w){return w.address==c.pair.exe;});
            if(!known){env.watches.emplace_back(c.pair.exe,16);if(c.pair.dll)env.watches.emplace_back(c.pair.dll,16);}
        }
        collaborators[kNames[f]]=list;
    }
    for(unsigned f=0;f<kFunc;++f){env.watches.emplace_back(kEntries[f],16);if(env.route[f])env.watches.emplace_back(env.route[f]->Body(),16);}
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    PrepareFloatingState();
    initialGlobals.Save();
    json vptrInfo=json::object();
    if(ready)ready&=CaptureDllVptr(env,vptrInfo);
    RestoreFloatingState();
    json cases=json::object();for(unsigned f=0;f<kFunc;++f)cases[kNames[f]]=kCases;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFunc},{"pairs",kFunc*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"resolved",resolved},{"dll_vptr",vptrInfo},{"exe_vptr",kExeHeliVptr},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},
        {"initial_globals",{{"use_ltm",u32(initialGlobals.useLtm)},{"pool",initialGlobals.pool},{"timer",initialGlobals.timer}}},
        {"layout",{{"clump_arena",kClumpArena},{"garage_arena",kGarageArena},{"cop_arena",kCopArena},{"heli_arena",kHeliArena},{"obj_bytes",kObjBytes},{"heli_bytes",kHeliBytes}}}});
    Totals totals;
    if(ready) {
        for(unsigned func=0;func<kFunc;++func){
            Totals ft;ft.pairs=totals.pairs;
            for(unsigned c=0;c<kCases;++c){
                if(func==kClump)CaseClump(env,ft,c);
                else if(func==kGarage)CaseGarage(env,ft,c);
                else if(func==kCop)CaseCop(env,ft,c);
                else CaseHeli(env,ft,c);
            }
            totals.pairs=ft.pairs;totals.failures+=ft.failures;totals.passed&=ft.passed;
            Write({{"event","function_summary"},{"function",kNames[func]},{"cases",kCases},{"failures",ft.failures},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    Globals fin;fin.Save();
    bool stateRestored=entriesIntact&&fin.Equal(initialGlobals);
    for(unsigned f=0;f<kFunc;++f)if(env.route[f])stateRestored&=env.route[f]->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"globals",{{"use_ltm",u32(fin.useLtm)},{"pool",fin.pool},{"timer",fin.timer}}}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
