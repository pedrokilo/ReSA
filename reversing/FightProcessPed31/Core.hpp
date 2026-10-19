#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <initializer_list>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <utility>
#include "Fixtures.hpp"
// Nucleo del banco FightProcessPed31: CTaskSimpleFight::ProcessPed (0x629920, hook VIRTUAL, ranura 7 de la vtable 0x86D684). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32,
// ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}, OriginalDependencies499::HookRoute y los tipos del juego. Plantillas: reversing/FightAbort31 y reversing/PartnerChatAbort30.
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Todo lo que la funcion toca es FALSO y comun a ambos lados: la tarea (`this`), las asociaciones
// de animacion (golpe, reposo, reposo nuevo), la jerarquia, el ped (ventana de 0x730 bytes), las matrices, el objetivo, los bloques y la tabla de grupos de animacion, el CWeaponInfo y el CPad;
// las globales que lee (tabla de combos y vectores de nivel 0xC17090..0xC17940, 0xB7CB5C, 0xB6EC2E, 0xB6F32C, 0xB4EA34) se fijan por caso y se restauran. Los colaboradores se sustituyen por el
// mismo falso en ambos lados (parche de entrada en el EXE y, si existe, en el cuerpo de la DLL resuelto antes de parchear): SetDeleteCallback, BlendAnimation, SetCurrentTime, IsPlayer,
// SetMoveState, Add/RemoveAnimBlockRef, GetAnimBlockName, GetAnimationBlock, RequestModel, GetWeaponInfo, CAEPedAudioEntity::AddAudioEvent, CAEPedWeaponAudioEntity::AddAudioEvent, FightStrike,
// GetPadFromPlayer, CPad::GetTarget (todos en C++ y enganchados) y, solo EXE, 0x61C7F0 (GetAvailableComboSet), 0x61C9B0, 0x624710, 0x624A40 y 0x623B10. Los falsos registran sus argumentos en orden.

namespace FightProcessPed31Live {
using namespace FightProcessPed31Fix;
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
// La funcion no usa x87 (empuja 4.0f como entero), pero sus colaboradores (constructores) pueden: se compara lo observable (FCW completa, TOP, etiquetas y ST validos).
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. EAX es el retorno (se compara siempre, como etiqueta, en `observed.ret`); ECX/EDX no los lee ningun llamador
// (CTaskComplexPartner::CreateNextSubTask 0x683B46 los sobrescribe: `mov ecx,[esp+30h]` en 0x6840AC), por defecto solo se registran.
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada (el EXE instala un marco SEH en 0x683120..0x683228 y lo retira).
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Igual que DecisionAdd27::Gate (sin argumentos: la funcion es thiscall sin parametros y `ret`): siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; captura los registros y EFLAGS justo tras el `call`.
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

// ------------------------------------------------------------------------------------------------ memoria de los casos
template<size_t N> struct Ar { alignas(16) std::array<u8,N> now{},ref{}; };
inline Ar<kThisArena> aThis;inline Ar<kAnimArena> aAnim,aIdle,aNew;inline Ar<kHierArena> aHier;inline Ar<kPedArena> aPed;inline Ar<kMatArena> aMat,aTMat;inline Ar<kTgtArena> aTgt;
inline Ar<kBlocksArena> aBlocks;inline Ar<kGroupsArena> aGroups;inline Ar<kWinfoArena> aWinfo;inline Ar<kPadArena> aPad;
inline const C* gC{};
inline u8 *pThis{},*pAnim{},*pIdle{},*pNew{},*pHier{},*pPed{},*pMat{},*pTMat{},*pTgt{},*pBlocks{},*pGroups{},*pWinfo{},*pPad{};
inline u32 gRet{};

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
enum : u32 {K_SETDEL=1,K_BLEND,K_SETTIME,K_ISPLAYER,K_MOVESTATE,K_ADDREF,K_REMREF,K_BLOCKNAME,K_GETBLOCK,K_REQUEST,K_WINFO,K_PEDAE,K_WEAPAE,K_STRIKE,K_AVAIL,K_FIND,K_CHOOSEP,K_CHOOSEAI,K_STARTANIM,K_GETPAD,K_GETTARGET};
inline constexpr u32 kNameToken=0x4E414D45;
struct Ev {u32 w[9];};
inline std::array<Ev,96> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0,u32 f=0,u32 g=0,u32 h=0){if(nev<evs.size()){evs[nev]=Ev{{k,a,b,c,d,e,f,g,h}};++nev;}}
// etiquetas (el oraculo no conoce direcciones): 0 nulo, 1 tarea, 2 anim, 3 reposo, 4 reposo nuevo, 5 ped, 6 objetivo, 7 pad, 0x100 + desplazamiento dentro del ped, 0xBAD otro
inline u32 Label(const void* p){
    if(!p)return 0u;const u8* q=static_cast<const u8*>(p);
    if(q==pThis)return 1u;if(q==pAnim)return 2u;if(q==pIdle)return 3u;if(q==pNew)return 4u;if(q==pPed)return 5u;if(q==pTgt)return 6u;if(q==pPad)return 7u;
    if(q>pPed&&q<pPed+kPedBytes)return 0x100u+u32(q-pPed);
    return 0xBADu;
}
inline u32 LabelU(u32 v){return v?Label(reinterpret_cast<const void*>(uintptr_t(v))):0u;}
inline u32 Bits(const void* p){u32 v;std::memcpy(&v,p,4);return v;}
// ---- falsos (thiscall = __fastcall (this, edx, args...) con el mismo `ret N`)
inline u32 __fastcall FakeSetDel(void* assoc,void*,u32 cb,u32 data){Push(K_SETDEL,Label(assoc),cb,LabelU(data));return gRet;}
inline u32 __cdecl FakeBlend(u32 clump,u32 group,u32 anim,u32 delta){Push(K_BLEND,clump,group,anim,delta);return u32(uintptr_t(pNew));}
inline u32 __fastcall FakeSetTime(void* assoc,void*,u32 bits){Push(K_SETTIME,Label(assoc),bits);return gRet;}
inline u32 __fastcall FakeIsPlayer(void* ped,void*){Push(K_ISPLAYER,Label(ped));return (gRet&~0xFFu)|(gC->isPlayer?1u:0u);}
inline u32 __fastcall FakeMoveState(void* ped,void*,u32 st){Push(K_MOVESTATE,Label(ped),st);return gRet;}
inline u32 __cdecl FakeAddRef(u32 idx){Push(K_ADDREF,idx);return gRet;}
inline u32 __cdecl FakeRemRef(u32 idx){Push(K_REMREF,idx);return gRet;}
inline u32 __cdecl FakeBlockName(u32 group){Push(K_BLOCKNAME,group);return kNameToken;}
inline u32 __cdecl FakeGetBlock(u32 name){Push(K_GETBLOCK,name);return u32(uintptr_t(pBlocks+0x20*(gC->nameBlock-1)));}
inline u32 __cdecl FakeRequest(u32 id,u32 flags){Push(K_REQUEST,id,flags);return gRet;}
inline u32 __cdecl FakeWinfo(u32 type,u32 skill){Push(K_WINFO,type,skill&0xFFu);return u32(uintptr_t(pWinfo));}
inline u32 __fastcall FakePedAE(void* self,void*,u32 ev,u32 vol,u32 speed,u32 phys,u32 surf,u32 a7,u32 maxVol){Push(K_PEDAE,Label(self),ev,vol,speed,LabelU(phys),surf,a7,maxVol);return gRet;}
inline u32 __fastcall FakeWeapAE(void* self,void*,u32 ev){Push(K_WEAPAE,Label(self),ev);return gRet;}
inline u32 __fastcall FakeStrike(void* task,void*,void* ped,const u8* pos){Push(K_STRIKE,Label(task),Label(ped),Bits(pos),Bits(pos+4),Bits(pos+8));return gRet;}
inline u32 __fastcall FakeAvail(void* task,void*,void* ped,u32 cmd){Push(K_AVAIL,Label(task),Label(ped),cmd&0xFFu);return gC->rAvail;}
inline u32 __fastcall FakeFind(void* task,void*,void* ped){Push(K_FIND,Label(task),Label(ped));return gRet;}
inline u32 __fastcall FakeChooseP(void* task,void*,void* ped){Push(K_CHOOSEP,Label(task),Label(ped));return gC->rAttack;}
inline u32 __fastcall FakeChooseAI(void* task,void*,void* ped){Push(K_CHOOSEAI,Label(task),Label(ped));return gC->rAttack;}
inline u32 __fastcall FakeStartAnim(void* task,void*,void* ped,u32 attack){Push(K_STARTANIM,Label(task),Label(ped),attack);return gRet;}
inline u32 __fastcall FakeGetPad(void* ped,void*){Push(K_GETPAD,Label(ped));return u32(uintptr_t(pPad));}
inline u32 __fastcall FakeGetTarget(void* pad,void*){Push(K_GETTARGET,Label(pad));return (gRet&~0xFFu)|(gC->getTarget&0xFFu);}

struct Pair {uintptr_t exe,dll;};
// Salta los enlaces de salto (E9) hacia fuera del EXE hasta el cuerpo real de la funcion de la DLL.
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    std::vector<Watch> watches;       // las entradas del EXE (parcheadas solo durante la llamada, con RAII): deben seguir siendo lo que eran
    Pair pSetDel{},pBlend{},pSetTime{},pIsPlayer{},pMoveState{},pAddRef{},pRemRef{},pBlockName{},pGetBlock{},pRequest{},pWinfo{},pPedAE{},pWeapAE{},pStrike{},pAvail{},pFind{},pChooseP{},pChooseAI{},pStart{},pGetPad{},pGetTarget{};
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,48> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    ~Patches(){for(int i=n-1;i>=0;--i)p[i].reset();}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// ---- globales que lee la funcion: se fijan por caso y se restauran (la region de la tabla de combos y los niveles, el paso de tiempo, la camara y el puntero a la tabla de grupos)
inline constexpr uintptr_t kTimeStep=0xB7CB5C,kCamFlag=0xB6EC2E,kCamFront=0xB6F32C,kGroupsPtr=0xB4EA34;
struct Globals {
    std::array<u8,kRegion> region{};u32 timestep{},groups{};u8 cam{};std::array<u8,12> front{};bool saved{};
    void Save(){std::memcpy(region.data(),reinterpret_cast<void*>(kRegionBase),kRegion);std::memcpy(&timestep,reinterpret_cast<void*>(kTimeStep),4);std::memcpy(&groups,reinterpret_cast<void*>(kGroupsPtr),4);
        cam=*reinterpret_cast<u8*>(kCamFlag);std::memcpy(front.data(),reinterpret_cast<void*>(kCamFront),12);saved=true;}
    void Restore()const{if(!saved)return;std::memcpy(reinterpret_cast<void*>(kRegionBase),region.data(),kRegion);std::memcpy(reinterpret_cast<void*>(kTimeStep),&timestep,4);std::memcpy(reinterpret_cast<void*>(kGroupsPtr),&groups,4);
        *reinterpret_cast<u8*>(kCamFlag)=cam;std::memcpy(reinterpret_cast<void*>(kCamFront),front.data(),12);}
    bool Intact()const{if(!saved)return true;return std::memcmp(reinterpret_cast<void*>(kRegionBase),region.data(),kRegion)==0&&std::memcmp(reinterpret_cast<void*>(kTimeStep),&timestep,4)==0&&std::memcmp(reinterpret_cast<void*>(kGroupsPtr),&groups,4)==0
        &&*reinterpret_cast<u8*>(kCamFlag)==cam&&std::memcmp(reinterpret_cast<void*>(kCamFront),front.data(),12)==0;}
};
inline Globals gSaved;

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los falsos se instalan DESPUES de comprobar la ruta.
template<class Fn> Side Execute(Env& env,HookRoute& route,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        {
            Patches patches;
            patches.Add(env.pSetDel,&FakeSetDel);patches.Add(env.pBlend,&FakeBlend);patches.Add(env.pSetTime,&FakeSetTime);patches.Add(env.pIsPlayer,&FakeIsPlayer);patches.Add(env.pMoveState,&FakeMoveState);
            patches.Add(env.pAddRef,&FakeAddRef);patches.Add(env.pRemRef,&FakeRemRef);patches.Add(env.pBlockName,&FakeBlockName);patches.Add(env.pGetBlock,&FakeGetBlock);patches.Add(env.pRequest,&FakeRequest);
            patches.Add(env.pWinfo,&FakeWinfo);patches.Add(env.pPedAE,&FakePedAE);patches.Add(env.pWeapAE,&FakeWeapAE);patches.Add(env.pStrike,&FakeStrike);patches.Add(env.pAvail,&FakeAvail);
            patches.Add(env.pFind,&FakeFind);patches.Add(env.pChooseP,&FakeChooseP);patches.Add(env.pChooseAI,&FakeChooseAI);patches.Add(env.pStart,&FakeStartAnim);patches.Add(env.pGetPad,&FakeGetPad);
            patches.Add(env.pGetTarget,&FakeGetTarget);
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
            {"nonvolatile_preserved",side.reg.nonvolatile},{"direction_flag_clear",(side.reg.eflags&0x400u)==0},{"seh_restored",side.reg.seh},{"route_ok",side.routeOK}};
}
inline json Finish(Env& env,Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    json row={{"event","comparison"},{"function",function},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
              {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}};
    Write(row);
    (void)env;
    return row;
}

// Cambios de un arena respecto a su imagen previa: [arena, desplazamiento respecto al objeto (negativo = guarda delantera), byte nuevo]; `skip0..skip1` (relativos al arena) se omiten
template<size_t N> inline void Diff(json& out,unsigned arena,const std::array<u8,N>& now,const std::array<u8,N>& ref,u32 off,u32 skip0=0,u32 skip1=0){
    for(u32 i=0;i<N;++i)if(now[i]!=ref[i]&&!(i>=skip0&&i<skip1))out.push_back({arena,int(i)-int(off),now[i]});
}
template<size_t N> inline void Put32(std::array<u8,N>& a,u32 at,u32 v){std::memcpy(a.data()+at,&v,4);}
template<size_t N> inline void Put16(std::array<u8,N>& a,u32 at,u32 v){const u16 w=u16(v);std::memcpy(a.data()+at,&w,2);}

// ------------------------------------------------------------------------------------------------ CTaskSimpleFight::ProcessPed
inline void CaseProc(Env& env,Totals& t,unsigned n) {
    const C c=MakeC(n);
    gC=&c;gRet=Rand(n,59);
    pThis=aThis.now.data()+kThisOff;pAnim=aAnim.now.data()+kAnimOff;pIdle=aIdle.now.data()+kAnimOff;pNew=aNew.now.data()+kAnimOff;pHier=aHier.now.data()+kHierOff;pPed=aPed.now.data()+kPedOff;
    pMat=aMat.now.data()+kMatOff;pTMat=aTMat.now.data()+kMatOff;pTgt=aTgt.now.data()+kTgtOff;pBlocks=aBlocks.now.data()+kBlocksOff;pGroups=aGroups.now.data()+kGroupsOff;pWinfo=aWinfo.now.data()+kWinfoOff;pPad=aPad.now.data()+kPadOff;
    auto P=[](const u8* p){return u32(uintptr_t(p));};

    // ---- imagenes previas (aleatorias) y campos del caso
    FillWords(aThis.ref.data(),kThisArena,Rand(n,50));FillWords(aAnim.ref.data(),kAnimArena,Rand(n,51));FillWords(aIdle.ref.data(),kAnimArena,Rand(n,52));FillWords(aNew.ref.data(),kAnimArena,Rand(n,53));
    FillWords(aHier.ref.data(),kHierArena,Rand(n,54));FillWords(aPed.ref.data(),kPedArena,Rand(n,55));FillWords(aMat.ref.data(),kMatArena,Rand(n,56));FillWords(aTMat.ref.data(),kMatArena,Rand(n,57));
    FillWords(aTgt.ref.data(),kTgtArena,Rand(n,58));FillWords(aBlocks.ref.data(),kBlocksArena,Rand(n,60));FillWords(aGroups.ref.data(),kGroupsArena,Rand(n,61));FillWords(aWinfo.ref.data(),kWinfoArena,Rand(n,62));
    FillWords(aPad.ref.data(),kPadArena,Rand(n,63));
    const u32 animPtr=c.hasAnim?P(pAnim):0u,idlePtr=c.hasIdle?P(pIdle):0u,newPtr=P(pNew),tgtPtr=c.hasTarget?P(pTgt):0u;
    {   // this
        auto& a=aThis.ref;const u32 o=kThisOff;
        a[o+8]=u8(c.finished);a[o+9]=u8(c.inControl);a[o+0xA]=u8(c.animsRef);Put32(a,o+0xC,c.reqGroup);Put16(a,o+0x10,c.idlePeriod);Put16(a,o+0x12,c.idleCounter);Put32(a,o+0x18,tgtPtr);Put32(a,o+0x1C,animPtr);Put32(a,o+0x20,idlePtr);
        a[o+0x24]=u8(c.combo);a[o+0x25]=u8(c.move);a[o+0x26]=u8(c.next);a[o+0x27]=u8(c.last);
    }
    {   // asociaciones y jerarquia
        auto& a=aAnim.ref;const u32 o=kAnimOff;Put32(a,o+kAnimHier,c.hasHier?P(pHier):0u);Put32(a,o+kAnimWeight,c.animW);Put32(a,o+kAnimDelta,c.animD);Put32(a,o+kAnimTime,c.animTime);Put32(a,o+kAnimStep,c.animStep);
        Put16(a,o+kAnimId,c.animId);Put16(a,o+kAnimFlags,c.animFlags);
        auto& d=aIdle.ref;Put32(d,kAnimOff+kAnimWeight,c.idleW);Put32(d,kAnimOff+kAnimDelta,c.idleD);
        Put32(aHier.ref,kHierOff+kHierTotal,c.hierTotal);
    }
    {   // ped
        auto& a=aPed.ref;const u32 o=kPedOff;
        for(u32 i=0;i<3;++i){Put32(a,o+kPedPos+4*i,c.pedPos[i]);Put32(a,o+kPedSpeed+4*i,c.speed[i]);}
        Put32(a,o+kPedMatrix,c.hasMatrix?P(pMat):0u);Put32(a,o+kPedClump,c.clump);Put32(a,o+kPedGroup,c.group);Put32(a,o+kPedMoveState,c.moveState);Put32(a,o+kPedSwim,c.swim0);Put32(a,o+kPedAim,kAimInit);
        a[o+kPedSlot]=u8(c.slot);a[o+kPedStyle]=u8(c.style);Put32(a,o+kPedWeapons+kWeaponStride*c.slot,c.wtype);
        auto& m=aMat.ref;for(u32 i=0;i<3;++i){Put32(m,kMatOff+4*i,c.mat[i]);Put32(m,kMatOff+0x10+4*i,c.mat[3+i]);Put32(m,kMatOff+0x20+4*i,c.mat[6+i]);Put32(m,kMatOff+0x30+4*i,c.mat[9+i]);}
        auto& g=aTgt.ref;for(u32 i=0;i<3;++i)Put32(g,kTgtOff+4+4*i,c.tgtPos[i]);Put32(g,kTgtOff+kPedMatrix,c.tgtHasMatrix?P(pTMat):0u);
        auto& tm=aTMat.ref;for(u32 i=0;i<3;++i)Put32(tm,kMatOff+0x30+4*i,c.tgtMatPos[i]);
    }
    u32 blockAddr[3];for(u32 k=0;k<3;++k){blockAddr[k]=P(pBlocks+0x20*k);aBlocks.ref[kBlocksOff+0x20*k+0x10]=u8(c.blockLoaded[k]);}
    u32 groupAddr[4];for(u32 k=0;k<4;++k){groupAddr[k]=c.groupBlock[k]?blockAddr[c.groupBlock[k]-1]:0u;Put32(aGroups.ref,kGroupsOff+0x14*k,groupAddr[k]);}
    const u32 nameAddr=blockAddr[c.nameBlock-1];
    aWinfo.ref[kWinfoOff+0x6E]=u8(c.baseCombo);
    std::array<u8,kRegion> table{};FillWords(table.data(),kRegion,c.tableSeed);
    for(const auto& w:c.writes)std::memcpy(table.data()+(w.addr-kRegionBase),&w.value,4);

    json wr=json::array();for(const auto& w:c.writes)wr.push_back({w.addr,w.value});
    json mat=json::array(),pp=json::array(),spd=json::array(),tp=json::array(),tmp=json::array(),cf=json::array(),gba=json::array(),ba=json::array(),bl=json::array();
    for(u32 v:c.mat)mat.push_back(v);for(u32 v:c.pedPos)pp.push_back(v);for(u32 v:c.speed)spd.push_back(v);for(u32 v:c.tgtPos)tp.push_back(v);for(u32 v:c.tgtMatPos)tmp.push_back(v);for(u32 v:c.camFront)cf.push_back(v);
    for(u32 v:groupAddr)gba.push_back(v);for(u32 v:blockAddr)ba.push_back(v);for(u32 v:c.blockLoaded)bl.push_back(v);
    const json input={{"family",c.family},{"finished",c.finished},{"in_control",c.inControl},{"anims_ref",c.animsRef},{"req_group",c.reqGroup},{"idle_period",c.idlePeriod},{"idle_counter",c.idleCounter},{"combo",c.combo},
        {"move",c.move},{"next",c.next},{"last",c.last},{"has_target",c.hasTarget},{"has_anim",c.hasAnim},{"has_idle",c.hasIdle},{"has_hier",c.hasHier},{"anim_w",c.animW},{"anim_d",c.animD},{"anim_time",c.animTime},
        {"anim_step",c.animStep},{"anim_flags",c.animFlags},{"anim_id",c.animId},{"hier_total",c.hierTotal},{"idle_w",c.idleW},{"idle_d",c.idleD},{"is_player",c.isPlayer},{"move_state",c.moveState},{"wslot",c.slot},
        {"wtype",c.wtype},{"style",c.style},{"group",c.group},{"clump",c.clump},{"has_matrix",c.hasMatrix},{"swim0",c.swim0},{"mat",mat},{"ped_pos",pp},{"speed",spd},{"tgt_has_matrix",c.tgtHasMatrix},{"tgt_pos",tp},
        {"tgt_mat_pos",tmp},{"cam_flag",c.camFlag},{"get_target",c.getTarget},{"cam_front",cf},{"timestep",c.timestep},{"base_combo",c.baseCombo},{"r_avail",c.rAvail},{"r_attack",c.rAttack},{"group_block_addr",gba},
        {"block_addr",ba},{"block_loaded",bl},{"name_block_addr",nameAddr},{"table_seed",c.tableSeed},{"table_writes",wr},{"anim_ptr",animPtr},{"idle_ptr",idlePtr},{"newidle_ptr",newPtr},{"eax",c.eax},{"edx",c.edx},
        {"stack_seed",c.stack},{"this_off",kThisOff},{"vslot",kProcessPedSlot}};

    Side sides[2];json rows[2];
    const u32 args[1]{P(pPed)};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        aThis.now=aThis.ref;aAnim.now=aAnim.ref;aIdle.now=aIdle.ref;aNew.now=aNew.ref;aHier.now=aHier.ref;aPed.now=aPed.ref;aMat.now=aMat.ref;aTMat.now=aTMat.ref;aTgt.now=aTgt.ref;
        aBlocks.now=aBlocks.ref;aGroups.now=aGroups.ref;aWinfo.now=aWinfo.ref;aPad.now=aPad.ref;nev=0;
        std::memcpy(reinterpret_cast<void*>(kRegionBase),table.data(),kRegion);std::memcpy(reinterpret_cast<void*>(kTimeStep),&c.timestep,4);*reinterpret_cast<u8*>(kCamFlag)=u8(c.camFlag);
        std::memcpy(reinterpret_cast<void*>(kCamFront),c.camFront.data(),12);const u32 gp=P(pGroups);std::memcpy(reinterpret_cast<void*>(kGroupsPtr),&gp,4);
        std::array<u8,kRegion> tableNow{};
        sides[s]=Execute(env,*env.route,0x629920,candidate,[&](uintptr_t entry){return Gate(entry,args,1,c.eax,P(pThis),c.edx,c.stack,0);});
        std::memcpy(tableNow.data(),reinterpret_cast<void*>(kRegionBase),kRegion);
        u32 tsNow,gpNow;u8 camNow;std::array<u8,12> frontNow;std::memcpy(&tsNow,reinterpret_cast<void*>(kTimeStep),4);std::memcpy(&gpNow,reinterpret_cast<void*>(kGroupsPtr),4);camNow=*reinterpret_cast<u8*>(kCamFlag);
        std::memcpy(frontNow.data(),reinterpret_cast<void*>(kCamFront),12);

        json obs;
        obs["ret"]=sides[s].reg.eax&0xFFu;
        json evj=json::array();
        for(unsigned i=0;i<nev;++i){json e=json::array();for(u32 w:evs[i].w)e.push_back(w);evj.push_back(e);}
        obs["events"]=evj;
        json changes=json::array();
        Diff(changes,0,aThis.now,aThis.ref,kThisOff);Diff(changes,1,aAnim.now,aAnim.ref,kAnimOff);Diff(changes,2,aIdle.now,aIdle.ref,kAnimOff);
        Diff(changes,3,aPed.now,aPed.ref,kPedOff,kPedOff+kPedAim,kPedOff+kPedAim+4);Diff(changes,4,aNew.now,aNew.ref,kAnimOff);
        obs["changes"]=changes;
        u32 aim;std::memcpy(&aim,aPed.now.data()+kPedOff+kPedAim,4);
        obs["aim"]=aim==kAimInit?json(nullptr):json(aim);
        obs["others_unchanged"]=aHier.now==aHier.ref&&aMat.now==aMat.ref&&aTMat.now==aTMat.ref&&aTgt.now==aTgt.ref&&aBlocks.now==aBlocks.ref&&aGroups.now==aGroups.ref&&aWinfo.now==aWinfo.ref&&aPad.now==aPad.ref;
        obs["globals_unchanged"]=tableNow==table&&tsNow==c.timestep&&gpNow==gp&&camNow==u8(c.camFlag)&&std::memcmp(frontNow.data(),c.camFront.data(),12)==0;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        gSaved.Restore();
        RestoreFloatingState();
        out["globals_restored"]=env.WatchesIntact()&&gSaved.Intact();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    gC=nullptr;
    Finish(env,t,"CTaskSimpleFight::ProcessPed",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    ready&=body&&ModuleOf(body)==env.asi;
    const Pair* all[]{&env.pSetDel,&env.pBlend,&env.pSetTime,&env.pIsPlayer,&env.pMoveState,&env.pAddRef,&env.pRemRef,&env.pBlockName,&env.pGetBlock,&env.pRequest,&env.pWinfo,&env.pPedAE,&env.pWeapAE,&env.pStrike,
                      &env.pAvail,&env.pFind,&env.pChooseP,&env.pChooseAI,&env.pStart,&env.pGetPad,&env.pGetTarget};
    for(const Pair* p:all)env.watches.emplace_back(p->exe,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    gSaved.Save();
    json stubbed=json::array();
    for(const Pair* p:all)stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},
        {"cases",{{"CTaskSimpleFight::ProcessPed",kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe_real",json::array()},{"stubbed",stubbed}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CaseProc(env,totals,c);
        Write({{"event","function_summary"},{"function","CTaskSimpleFight::ProcessPed"},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()&&gSaved.Intact()}});
    }
    gSaved.Restore();
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&gSaved.Intact();
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"globals_intact",gSaved.Intact()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
