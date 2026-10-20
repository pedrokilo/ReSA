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

// Nucleo del banco PedEntityCollision60. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (LiveComparison.hpp los incluye). Plantilla: reversing/PedWrappers60 (hook Virtual con falsos).
//
// Una funcion virtual de CPed: CPed::ProcessEntityCollision 0x5E2530 (hook Virtual; parchea tambien la entrada del EXE con un salto al cuerpo de la DLL).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Todo lo que la funcion toca es memoria FALSA del banco (arenas con guardas rellenas de basura
// determinista) o globales que el banco fija por caso y restaura. Los colaboradores se sustituyen por el mismo falso en ambos lados (parche de entrada en el EXE y en el cuerpo de la DLL, que se
// localiza por la tabla de hooks). El CEventDamage y el calculador de dano son los REALES (constructor, destructor): sus campos se comparan en los falsos de AffectsPed/ComputeDamageResponse/Add.
namespace PedEntityCollision60Live {
using namespace PedEntityCollision60Fix;
using HookRoute = OriginalDependencies499::HookRoute;
using u8 = std::uint8_t;
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
// La funcion usa x87 en el EXE (y SSE en la DLL): se compara lo observable (FCW completa, TOP, etiquetas y ST validos); no los registros marcados vacios, ni las banderas de FSW, ni FIP/FDP.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Los llamadores del EXE (CPhysical::ProcessCollision, llamada virtual) leen EAX (numero de puntos): EAX se compara siempre como retorno;
// ECX/EDX solo se registran (ver README.md).
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
inline Ar<kPedArena> aPed;inline Ar<kEntArena> aEnt;inline Ar<kMatArena> aMatPed,aMatEnt;inline Ar<kCpArena> aCp;inline Ar<kMiArena> aMi;
inline Ar<kCmArena> aPcm,aTcm,aEcm;inline Ar<kCdArena> aCd;inline Ar<kLnArena> aLn;inline Ar<kSpArena> aSp;inline Ar<kTaskArena> aTask;inline Ar<kWiArena> aWi;
inline Ar<kTokArena> aIntel,aClump,aAssoc;

struct Slot {u8* now;u8* ref;u32 bytes,off,arena,label;};
inline std::array<Slot,L_COUNT> slots{};
template<size_t N> inline Slot Mk(Ar<N>& a,u32 bytes,u32 off,u32 label){return Slot{a.now.data(),a.ref.data(),bytes,off,u32(N),label};}
inline void InitSlots() {
    slots={};
    auto set=[](Slot s){slots[s.label]=s;};
    set(Mk(aPed,kPedBytes,kPedOff,L_PED));set(Mk(aEnt,kEntBytes,kEntOff,L_ENT));set(Mk(aMatPed,kMatBytes,kMatOff,L_MATPED));set(Mk(aMatEnt,kMatBytes,kMatOff,L_MATENT));
    set(Mk(aCp,kCpBytes,kCpOff,L_CP));set(Mk(aMi,kMiBytes,kMiOff,L_MI));set(Mk(aPcm,kCmBytes,kCmOff,L_PCM));set(Mk(aTcm,kCmBytes,kCmOff,L_TCM));set(Mk(aEcm,kCmBytes,kCmOff,L_ECM));
    set(Mk(aCd,kCdBytes,kCdOff,L_CD));set(Mk(aLn,kLnBytes,kLnOff,L_LN));set(Mk(aSp,kSpBytes,kSpOff,L_SP));set(Mk(aTask,kTaskBytes,kTaskOff,L_TASK));set(Mk(aWi,kWiBytes,kWiOff,L_WI));
    set(Mk(aIntel,kTokBytes,kTokOff,L_INTEL));set(Mk(aClump,kTokBytes,kTokOff,L_CLUMP));set(Mk(aAssoc,kTokBytes,kTokOff,L_ASSOC));
}
inline u32 AddrOf(u32 label){return label>0&&label<L_COUNT?u32(uintptr_t(slots[label].now+slots[label].off)):0u;}
// etiqueta de un puntero: la del objeto cuya ventana empieza ahi; 0x100 + desplazamiento para direcciones dentro del ped; 0 nulo; 0x200 + desplazamiento dentro de la entidad; 0xBAD otro (p. ej. pila).
// El colmodel temporal del EXE (0x968E20) se etiqueta L_TCM.
inline u32 Label(const void* p){
    if(!p)return 0u;const u8* q=static_cast<const u8*>(p);
    if(uintptr_t(q)==kGTempCm)return L_TCM;
    const u8* ped=slots[L_PED].now+kPedOff;
    if(q>=ped&&q<ped+kPedBytes)return q==ped?u32(L_PED):0x100u+u32(q-ped);
    const u8* ent=slots[L_ENT].now+kEntOff;
    if(q>=ent&&q<ent+kEntBytes)return q==ent?u32(L_ENT):0x1000u+u32(q-ent);
    for(u32 l=1;l<L_COUNT;++l)if(l!=L_PED&&l!=L_ENT&&q==slots[l].now+slots[l].off)return l;
    return 0xBADu;
}
inline u32 LabelU(u32 v){return v?Label(reinterpret_cast<const void*>(uintptr_t(v))):0u;}
inline u32 Bits(const void* p){u32 v;std::memcpy(&v,p,4);return v;}

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
constexpr u32 kEvWidth=20;
struct Ev {u32 w[kEvWidth];};
inline std::array<Ev,160> evs{};inline unsigned nev{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0,u32 e=0,u32 f=0,u32 g=0,u32 h=0,u32 i=0,u32 j=0,u32 k2=0,u32 l=0,u32 m=0,u32 n=0,u32 o=0,u32 p=0){
    if(nev<evs.size()){evs[nev]=Ev{{k,a,b,c,d,e,f,g,h,i,j,k2,l,m,n,o,p,0,0,0}};++nev;}
}
// tipos de evento
enum : u32 {K_TASK=1,K_GETHEAD,K_SETHEAD,K_ALLOC,K_UPDMAT,K_GETCOL,K_PCM,K_REGREF,K_CLEANREF,K_STEEP,K_SOFT,K_ASSOC,K_LOCALDIR,K_AFFECTS,K_COMPUTE,K_ADD,K_ADDREC,K_ROTZ};

inline const Case* gC{};
inline unsigned gAffectCall{},gPcmCall{};
inline const u8* gLastEvent{};
inline u64 Fnv(const u8* p,size_t n){u64 h=0xCBF29CE484222325ull;for(size_t i=0;i<n;++i)h=(h^p[i])*0x100000001B3ull;return h;}
// CEventDamage (0x44 bytes): se comparan solo los bytes que el constructor inicializa (sin vtable ni relleno)
inline u64 EventHash(const void* e){
    u8 m[0x44];std::memcpy(m,e,sizeof(m));
    for(int i:{0,1,2,3,9,10,11,13,0x12,0x13,0x26,0x27})m[i]=0;
    m[0x25]&=0x1F;
    return Fnv(m,sizeof(m));
}

// ---- falsos (thiscall = __fastcall (this, edx, args...) con el mismo `ret N`; cdecl = __cdecl)
inline u32 __fastcall FkTaskUseGun(void* intel,void*){Push(K_TASK,Label(intel));return gC->aimSel?AddrOf(L_TASK):0u;}
inline float __fastcall FkGetHeading(void* self,void*){Push(K_GETHEAD,Label(self));return U2F(gC->hdgBits);}
inline void __fastcall FkSetHeading(void* self,void*,u32 bits){Push(K_SETHEAD,Label(self),bits);}
inline void __fastcall FkAlloc(void* self,void*){Push(K_ALLOC,Label(self));*reinterpret_cast<u32*>(static_cast<u8*>(self)+0x14)=AddrOf(L_MATENT);}
inline void __fastcall FkUpdMat(void* self,void*,u32 mat){Push(K_UPDMAT,u32(uintptr_t(self))-AddrOf(L_ENT),LabelU(mat));}
inline u32 __fastcall FkGetCol(void* self,void*){Push(K_GETCOL,Label(self));return AddrOf(L_ECM);}
inline u32 __cdecl FkPcm(u32 matPed,u32 colPed,u32 matEnt,u32 colEnt,u32 points,u32 lines,u32 touch,u32 flag){
    const bool second=gPcmCall++!=0;
    // lo que la funcion deja en el colmodel y los datos de colision del ped en el momento de la llamada
    u64 hc=0,hd=0;
    if(colPed)hc=Fnv(reinterpret_cast<const u8*>(uintptr_t(colPed)),0x30);
    const u32 data=colPed?*reinterpret_cast<const u32*>(uintptr_t(colPed)+0x2C):0u;
    if(data){const u8* d=reinterpret_cast<const u8*>(uintptr_t(data));hd=Fnv(d,0x18);const u32 pl=*reinterpret_cast<const u32*>(d+0x10);if(pl)hd^=Fnv(reinterpret_cast<const u8*>(uintptr_t(pl)),0x40);}
    Push(K_PCM,LabelU(matPed),LabelU(colPed),LabelU(matEnt),LabelU(colEnt),LabelU(points),lines?1u:0u,touch?1u:0u,flag&0xFFu,u32(hc),u32(hc>>32),u32(hd),u32(hd>>32),second?1u:0u);
    const Case& c=*gC;
    const unsigned n=second?c.n2:c.n1;
    u8* cps=reinterpret_cast<u8*>(uintptr_t(points));
    for(unsigned i=0;i<n&&i<32;++i)MakeCp(c,second?1u:0u,i,cps+0x2C*i);
    if(lines&&touch&&!second){
        float* t=reinterpret_cast<float*>(uintptr_t(touch));
        u8* ln=reinterpret_cast<u8*>(uintptr_t(lines));
        if(c.w0){std::memcpy(&t[0],&c.touch0,4);MakeCp(c,2,0,ln);}
        if(c.w1){std::memcpy(&t[1],&c.touch1,4);MakeCp(c,3,0,ln+0x2C);}
    }
    return second?c.n2:c.n1;
}
inline void __fastcall FkRegRef(void* self,void*,u32 ref){Push(K_REGREF,Label(self),LabelU(ref));}
inline void __fastcall FkCleanRef(void* self,void*,u32 ref){Push(K_CLEANREF,Label(self),LabelU(ref));}
inline u32 __fastcall FkSteep(void* self,void*,u32 surf){Push(K_STEEP,surf);return (gC->junk&~0xFFu)|((gC->steepSeed&&(Rand(gC->steepSeed,surf&0xFFu)&1))?1u:0u);}
inline u32 __fastcall FkSoft(void* self,void*,u32 surf){Push(K_SOFT,surf);return (gC->junk&~0xFFu)|((gC->softSeed&&(Rand(gC->softSeed,surf&0xFFu)&1))?1u:0u);}
inline u32 __cdecl FkAssoc(u32 clump,u32 id){Push(K_ASSOC,LabelU(clump),id);return gC->assocSel?AddrOf(L_ASSOC):0u;}
inline u32 __fastcall FkLocalDir(void* self,void*,const u8* v){Push(K_LOCALDIR,Label(self),Bits(v),Bits(v+4));return gC->localDir;}
inline u32 __fastcall FkAffects(void* ev,void*,void* ped){
    gLastEvent=static_cast<const u8*>(ev);const u64 h=EventHash(ev);
    Push(K_AFFECTS,u32(h),u32(h>>32),Label(ped));return (gC->junk&~0xFFu)|gC->affects[(gAffectCall++)&1];}
inline void __fastcall FkCompute(void* calc,void*,void* ped,u8* resp,u32 speak){
    const u8* q=static_cast<const u8*>(calc);
    Push(K_COMPUTE,LabelU(Bits(q)),Bits(q+4),Bits(q+8),Bits(q+0xC),q[0x10],Label(ped),u32(resp-gLastEvent),speak&0xFFu);
    std::memcpy(resp,&gC->resp[0],4);std::memcpy(resp+4,&gC->resp[1],4);std::memcpy(resp+8,&gC->resp[2],4);
}
inline u32 __fastcall FkGroupAdd(void* group,void*,void* ev,u32 flag){
    const u64 h=EventHash(ev);
    Push(K_ADD,u32(uintptr_t(group))-AddrOf(L_INTEL),u32(h),u32(h>>32),flag&0xFFu,u32(static_cast<const u8*>(ev)-gLastEvent));return gC->junk;}
inline void __fastcall FkAddRec(void* self,void*,void* other){Push(K_ADDREC,Label(self),Label(other));}
inline void __fastcall FkRotZ(void* self,void*,u32 bits){Push(K_ROTZ,Label(self),bits);}

// Entrada del EXE y cuerpo de la DLL de un colaborador. El cuerpo de la DLL se localiza por la direccion del EXE recorriendo la tabla de hooks (Simple o Virtual): es la direccion "nuestra" del hook.
// Con un hook APAGADO la entrada de la DLL es un `jmp` al EXE: solo se siguen saltos mientras caigan dentro del ASI (si no, el cuerpo no se resuelve). Se resuelve ANTES de escribir ningun parche.
struct Pair {uintptr_t exe,dll;};
inline uintptr_t gAsiBase{};
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(ModuleOf(t)!=gAsiBase)break;p=t;}return p;}
inline uintptr_t OurByExe(uintptr_t exe){
    uintptr_t found=0;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){
        for(auto& hook:category.Items()){
            if(!strcmp(hook->Symbol(),"S")){auto* s=static_cast<ReversibleHooks::ReversibleHook::Simple*>(hook.get());if(uintptr_t(s->GetHookGTAAddress())==exe&&s->GetHookOurAddress())found=uintptr_t(s->GetHookOurAddress());}
            else if(!strcmp(hook->Symbol(),"V")){auto* v=static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook.get());if(uintptr_t(v->GetHookGTAAddress())==exe&&v->GetHookOurAddress())found=uintptr_t(v->GetHookOurAddress());}
        }
    });
    return found;
}
inline Pair MakePair(uintptr_t exe,uintptr_t fallback=0){uintptr_t our=OurByExe(exe);if(!our)our=fallback;return Pair{exe,our?Body(our):0};}
inline uintptr_t gUpdMatFallback{},gRotZFallback{};      // CSimpleTransform::UpdateMatrix no tiene hook: LiveComparison aporta su direccion en la DLL

struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,160> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    void Reset(){for(int i=n-1;i>=0;--i)p[i].reset();n=0;}
    ~Patches(){Reset();}
};
struct Stubs {
    Pair tuse,gethead,sethead,alloc,updmat,getcol,pcm,regref,cleanref,steep,soft,assoc,localdir,affects,compute,gadd,addrec,rotz;
    std::vector<const Pair*> All()const{return {&tuse,&gethead,&sethead,&alloc,&updmat,&getcol,&pcm,&regref,&cleanref,&steep,&soft,&assoc,&localdir,&affects,&compute,&gadd,&addrec,&rotz};}
    void Resolve(){
        tuse=MakePair(0x600F70);gethead=MakePair(0x441DB0);sethead=MakePair(0x43E0C0);alloc=MakePair(0x54F560);updmat=MakePair(0x54F1B0,gUpdMatFallback);getcol=MakePair(0x535300);pcm=MakePair(0x4185C0);
        regref=MakePair(0x571B70);cleanref=MakePair(0x571A00);steep=MakePair(0x55E770);soft=MakePair(0x55E690);assoc=MakePair(0x4D68B0);localdir=MakePair(0x5DEF60);affects=MakePair(0x4B35A0);
        compute=MakePair(0x4B5AC0);gadd=MakePair(0x4AB420);addrec=MakePair(0x543490);rotz=MakePair(0x59B020,gRotZFallback);
    }
    void Install(Patches& p)const{
        p.Add(tuse,&FkTaskUseGun);p.Add(gethead,&FkGetHeading);p.Add(sethead,&FkSetHeading);p.Add(alloc,&FkAlloc);p.Add(updmat,&FkUpdMat);p.Add(getcol,&FkGetCol);p.Add(pcm,&FkPcm);p.Add(regref,&FkRegRef);
        p.Add(cleanref,&FkCleanRef);p.Add(steep,&FkSteep);p.Add(soft,&FkSoft);p.Add(assoc,&FkAssoc);p.Add(localdir,&FkLocalDir);p.Add(affects,&FkAffects);p.Add(compute,&FkCompute);p.Add(gadd,&FkGroupAdd);
        p.Add(addrec,&FkAddRec);p.Add(rotz,&FkRotZ);
    }
};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> route;
    Stubs stubs;
    std::vector<Watch> watches;       // entradas del EXE (parcheadas solo mientras dura una seccion, con RAII): deben seguir siendo lo que eran
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline unsigned gPairs{};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// ---- globales: se fijan por caso y se restauran (la funcion NO debe cambiarlos; el banco comprueba que siguen como los dejo)
struct Globals {
    float timestep{},mix{},bright{};u32 timeMs{};
    std::array<u32,kModelSlots> models{};
    std::array<u8,0x30> tempCm{};
    u8 camActive{};std::array<u16,3> camMode{};std::array<u32,6> camFront{};
    bool saved{};
    void Save(){
        std::memcpy(&timestep,reinterpret_cast<void*>(kGTimestep),4);timeMs=*reinterpret_cast<u32*>(kGTimeMs);std::memcpy(&mix,reinterpret_cast<void*>(kGMix),4);std::memcpy(&bright,reinterpret_cast<void*>(kGBright),4);
        for(u32 i=0;i<kModelSlots;++i)models[i]=*reinterpret_cast<u32*>(kGModelInfo+4*i);
        std::memcpy(tempCm.data(),reinterpret_cast<void*>(kGTempCm),0x30);
        camActive=*reinterpret_cast<u8*>(kGCamActive);
        for(u32 i=0;i<3;++i){camMode[i]=*reinterpret_cast<u16*>(kGCamMode+kGCamStride*i);std::memcpy(&camFront[2*i],reinterpret_cast<void*>(kGCamFront+kGCamStride*i),8);}
        saved=true;
    }
    void RestoreAll()const{
        if(!saved)return;
        std::memcpy(reinterpret_cast<void*>(kGTimestep),&timestep,4);*reinterpret_cast<u32*>(kGTimeMs)=timeMs;std::memcpy(reinterpret_cast<void*>(kGMix),&mix,4);std::memcpy(reinterpret_cast<void*>(kGBright),&bright,4);
        for(u32 i=0;i<kModelSlots;++i)*reinterpret_cast<u32*>(kGModelInfo+4*i)=models[i];
        std::memcpy(reinterpret_cast<void*>(kGTempCm),tempCm.data(),0x30);
        *reinterpret_cast<u8*>(kGCamActive)=camActive;
        for(u32 i=0;i<3;++i){*reinterpret_cast<u16*>(kGCamMode+kGCamStride*i)=camMode[i];std::memcpy(reinterpret_cast<void*>(kGCamFront+kGCamStride*i),&camFront[2*i],8);}
    }
    bool Intact()const{
        if(!saved)return true;
        bool ok=std::memcmp(reinterpret_cast<void*>(kGTimestep),&timestep,4)==0&&*reinterpret_cast<u32*>(kGTimeMs)==timeMs&&std::memcmp(reinterpret_cast<void*>(kGMix),&mix,4)==0&&std::memcmp(reinterpret_cast<void*>(kGBright),&bright,4)==0;
        for(u32 i=0;i<kModelSlots;++i)ok&=*reinterpret_cast<u32*>(kGModelInfo+4*i)==models[i];
        ok&=std::memcmp(reinterpret_cast<void*>(kGTempCm),tempCm.data(),0x30)==0;
        ok&=*reinterpret_cast<u8*>(kGCamActive)==camActive;
        for(u32 i=0;i<3;++i){ok&=*reinterpret_cast<u16*>(kGCamMode+kGCamStride*i)==camMode[i];ok&=std::memcmp(reinterpret_cast<void*>(kGCamFront+kGCamStride*i),&camFront[2*i],8)==0;}
        return ok;
    }
};
inline Globals gSaved;
// la funcion NO debe cambiar ningun global (salvo el colmodel temporal, que se compara como arena): comprueba que siguen como los dejo SetGlobals
inline bool GlobalsAsSet(const Case& c) {
    bool ok=std::memcmp(reinterpret_cast<void*>(kGTimestep),&c.timestep,4)==0&&*reinterpret_cast<u32*>(kGTimeMs)==c.timeMs&&std::memcmp(reinterpret_cast<void*>(kGMix),&c.gMix,4)==0&&std::memcmp(reinterpret_cast<void*>(kGBright),&c.gBright,4)==0;
    for(u32 i=0;i<kModelSlots;++i)ok&=*reinterpret_cast<u32*>(kGModelInfo+4*i)==AddrOf(L_MI);
    ok&=*reinterpret_cast<u8*>(kGCamActive)==u8(c.camActive);
    for(u32 i=0;i<3;++i){ok&=*reinterpret_cast<u16*>(kGCamMode+kGCamStride*i)==u16(c.camMode[i]);ok&=std::memcmp(reinterpret_cast<void*>(kGCamFront+kGCamStride*i),&c.camFront[2*i],8)==0;}
    return ok;
}

// ---- montaje del caso
inline void Put32(Slot& s,u32 off,u32 v){std::memcpy(s.ref+s.off+off,&v,4);}
inline void Put16(Slot& s,u32 off,u32 v){const u16 x=u16(v);std::memcpy(s.ref+s.off+off,&x,2);}
inline void Put8(Slot& s,u32 off,u32 v){s.ref[s.off+off]=u8(v);}
inline u32 PtrSel(u32 sel){return sel==1?AddrOf(L_ENT):(sel==2?kAnyPtr:0u);}
inline void BuildImages(const Case& c,unsigned n) {
    for(u32 l=1;l<L_COUNT;++l)FillWords(slots[l].ref,slots[l].arena,Rand(n,300+l));
    auto& ped=slots[L_PED];
    Put32(ped,0x10,c.pedHeading);Put32(ped,0x14,AddrOf(L_MATPED));Put32(ped,0x18,(Rand(c.cpSeed,270)%7==0)?0u:AddrOf(L_CLUMP));Put32(ped,0x1C,c.pedF1C);Put16(ped,0x22,c.modelIdx);
    Put32(ped,0x40,c.pedPhys);Put32(ped,0x44,c.speed[0]);Put32(ped,0x48,c.speed[1]);Put32(ped,0x4C,c.speed[2]);
    Put32(ped,0xFC,c.attachedSel?kAnyPtr:0u);Put32(ped,0x128,PtrSel(c.ignoredSel));Put32(ped,0x12C,c.bright);
    Put32(ped,0x46C,c.ped46C);Put32(ped,0x470,c.ped470);Put32(ped,0x474,c.ped474);Put32(ped,0x478,c.ped478);Put32(ped,0x47C,AddrOf(L_INTEL));
    Put32(ped,0x530,c.pedState);Put32(ped,0x588,c.f588);Put32(ped,0x58C,PtrSel(c.vehSel));Put32(ped,0x598,c.pedType);
    auto& mp=slots[L_MATPED];Put32(mp,0x30,c.pedPos[0]);Put32(mp,0x34,c.pedPos[1]);Put32(mp,0x38,c.pedPos[2]);
    auto& ent=slots[L_ENT];
    Put32(ent,0x10,c.entPlHeading);Put32(ent,0x14,c.entMatNull?0u:AddrOf(L_MATENT));Put32(ent,0x1C,c.entF1C);Put8(ent,0x36,c.entType);Put32(ent,0x40,c.entPhys);
    Put32(ent,0x44,c.entSpeed[0]);Put32(ent,0x48,c.entSpeed[1]);Put32(ent,0x4C,c.entSpeed[2]);Put32(ent,0x12C,c.entBright);Put32(ent,0x590,c.entVType);Put32(ent,0x594,c.entSub);
    Put32(ent,4,c.entPlPos[0]);Put32(ent,8,c.entPlPos[1]);Put32(ent,12,c.entPlPos[2]);
    auto& me=slots[L_MATENT];Put32(me,0x30,c.entMatPos[0]);Put32(me,0x34,c.entMatPos[1]);Put32(me,0x38,c.entMatPos[2]);
    Put32(slots[L_MI],0x14,AddrOf(L_PCM));
    Put32(slots[L_PCM],0x2C,AddrOf(L_CD));Put32(slots[L_TCM],0x2C,AddrOf(L_CD));
    auto& cd=slots[L_CD];Put32(cd,8,AddrOf(L_SP));Put32(cd,0x10,AddrOf(L_LN));
    auto& ln=slots[L_LN];for(int i=0;i<2;++i)for(int j=0;j<8;++j)Put32(ln,0x20*i+4*j,c.line[i][j]);
    auto& sp=slots[L_SP];for(int i=0;i<3;++i)for(int j=0;j<5;++j)Put32(sp,0x14*i+4*j,c.sph[i][j]);
    Put32(slots[L_TASK],0x30,c.aimSel==2?AddrOf(L_WI):0u);
    Put32(slots[L_WI],0x18,c.winfoFlags);
    Put8(slots[L_CLUMP],0,2);   // RwObject tipo rpCLUMP (2) (CEntity::GetRpClump lo comprueba)
}
inline void SetGlobals(const Case& c) {
    std::memcpy(reinterpret_cast<void*>(kGTimestep),&c.timestep,4);*reinterpret_cast<u32*>(kGTimeMs)=c.timeMs;std::memcpy(reinterpret_cast<void*>(kGMix),&c.gMix,4);std::memcpy(reinterpret_cast<void*>(kGBright),&c.gBright,4);
    for(u32 i=0;i<kModelSlots;++i)*reinterpret_cast<u32*>(kGModelInfo+4*i)=AddrOf(L_MI);
    std::memcpy(reinterpret_cast<void*>(kGTempCm),slots[L_TCM].ref+slots[L_TCM].off,0x30);
    *reinterpret_cast<u8*>(kGCamActive)=u8(c.camActive);
    for(u32 i=0;i<3;++i){*reinterpret_cast<u16*>(kGCamMode+kGCamStride*i)=u16(c.camMode[i]);std::memcpy(reinterpret_cast<void*>(kGCamFront+kGCamStride*i),&c.camFront[2*i],8);}
}
// cambios de una arena respecto a su imagen previa: [arena, desplazamiento respecto al objeto (negativo = guarda delantera), byte nuevo]
template<class Out> inline void Diff(Out& out,unsigned arena,const Slot& s){
    for(u32 i=0;i<s.arena;++i)if(s.now[i]!=s.ref[i])out.push_back(std::array<int,3>{int(arena),int(i)-int(s.off),int(s.now[i])});
}

inline json ParamsJson(const Case& c) {
    json j;
    j["family"]=c.family;j["index"]=c.index;
    j["ped"]={{"model",c.modelIdx},{"f1C",c.pedF1C},{"phys",c.pedPhys},{"f46C",c.ped46C},{"f470",c.ped470},{"f474",c.ped474},{"f478",c.ped478},{"type",c.pedType},{"state",c.pedState},
              {"attached",c.attachedSel},{"ignored",c.ignoredSel},{"veh",c.vehSel},{"f588",c.f588},{"speed",c.speed},{"bright",c.bright},{"pos",c.pedPos},{"heading",c.pedHeading}};
    j["gun"]={{"aim",c.aimSel},{"winfo",c.winfoFlags},{"cam_active",c.camActive},{"cam_mode",c.camMode},{"cam_front",c.camFront}};
    j["ent"]={{"f1C",c.entF1C},{"type",c.entType},{"phys",c.entPhys},{"vtype",c.entVType},{"sub",c.entSub},{"speed",c.entSpeed},{"bright",c.entBright},{"mat_null",c.entMatNull},{"mat_pos",c.entMatPos}};
    json sph=json::array(),ln=json::array();
    for(int i=0;i<3;++i)sph.push_back(c.sph[i]);for(int i=0;i<2;++i)ln.push_back(c.line[i]);
    j["col"]={{"spheres",sph},{"lines",ln}};
    j["pcm"]={{"n1",c.n1},{"n2",c.n2},{"touch0",c.touch0},{"touch1",c.touch1},{"w0",c.w0},{"w1",c.w1},{"cp_seed",c.cpSeed},{"lpz",c.lpz}};
    j["fakes"]={{"steep",c.steepSeed},{"soft",c.softSeed},{"assoc",c.assocSel},{"local_dir",c.localDir},{"affects",c.affects},{"heading",c.hdgBits},{"junk",c.junk},{"resp",c.resp}};
    j["globals"]={{"timestep",c.timestep},{"time_ms",c.timeMs},{"mix",c.gMix},{"bright",c.gBright}};
    j["abi"]={{"eax",c.eax},{"edx",c.edx},{"stack",c.stack}};
    return j;
}

template<class Fn> Side Execute(HookRoute& route,uintptr_t entry,uintptr_t asi,bool candidate,Fn gate) {
    Side side{};
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==asi):!ReversingProbe::JumpsToDll(entry);
        side.reg=gate(entry);
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
inline void Finish(Totals& t,const char* function,unsigned n,const json& input,json rows[2],Side sides[2]) {
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","seh_restored","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function",function},{"case",n},{"pair",gPairs++},{"fn_pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ CPed::ProcessEntityCollision (0x5E2530)
inline constexpr const char* kName="CPed::ProcessEntityCollision";
inline void CasePE(Env& env,Totals& t,unsigned n) {
    const Case c=MakeCase(n);
    gC=&c;nev=0;gAffectCall=0;gPcmCall=0;gLastEvent=nullptr;
    InitSlots();
    BuildImages(c,n);
    json input=ParamsJson(c);
    {json addrs=json::array();for(u32 l=0;l<L_COUNT;++l)addrs.push_back(AddrOf(l));input["addrs"]=addrs;}
    Side sides[2];json rows[2];
    const u32 thisPtr=AddrOf(L_PED);
    const u32 args[2]{AddrOf(L_ENT),AddrOf(L_CP)};
    Patches patches;
    env.stubs.Install(patches);
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        for(auto& sl:slots)if(sl.now)std::memcpy(sl.now,sl.ref,sl.arena);
        nev=0;gAffectCall=0;gPcmCall=0;gLastEvent=nullptr;
        SetGlobals(c);
        sides[s]=Execute(*env.route,0x5E2530,env.asi,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,thisPtr,c.edx,c.stack,0);});
        // el colmodel temporal del EXE (global) se copia a su arena espejo para compararlo como cualquier otra arena
        std::memcpy(slots[L_TCM].now+slots[L_TCM].off,reinterpret_cast<void*>(kGTempCm),0x30);
        json obs;
        obs["ret"]=sides[s].reg.eax;
        json evj=json::array();
        for(unsigned i=0;i<nev;++i){json e=json::array();for(u32 w:evs[i].w)e.push_back(w);evj.push_back(e);}
        obs["events"]=evj;
        obs["event_overflow"]=nev>=evs.size();
        obs["globals_unchanged"]=GlobalsAsSet(c);
        json changes=json::array();
        for(u32 l=1;l<L_COUNT;++l)Diff(changes,l-1,slots[l]);
        obs["changes"]=changes;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        gSaved.RestoreAll();
        RestoreFloatingState();
        out["globals_restored"]=gSaved.Intact();   // las entradas vigiladas se comprueban fuera del bucle, con los parches de los falsos ya retirados
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    gC=nullptr;
    patches.Reset();   // los falsos parchean entradas vigiladas: comprobarlas solo con ellos retirados
    {const bool w=env.WatchesIntact();for(auto& r:rows)r["globals_restored"]=r["globals_restored"]==true&&w;}
    Finish(t,kName,n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies,const json& layout) {
    ready&=env.route&&env.route->Body()&&ModuleOf(env.route->Body())==env.asi;
    env.stubs.Resolve();
    json stubbed=json::array();
    for(const Pair* p:env.stubs.All()){env.watches.emplace_back(p->exe,16);stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});}
    // todos los colaboradores deben tener cuerpo en la DLL (el banco los parchea en ambos lados)
    bool stubsOk=true;for(const Pair* p:env.stubs.All())stubsOk&=p->dll!=0&&ModuleOf(p->dll)==env.asi;
    ready&=stubsOk;
    env.watches.emplace_back(0x5E2530,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    gSaved.Save();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",kCases},{"cases",{{kName,kCases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe_real",json::array({0x59C910,0x40FC80})},{"stubbed",stubbed}}},{"layout",layout},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<kCases;++c)CasePE(env,totals,c);
        Write({{"event","function_summary"},{"function",kName},{"cases",kCases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()&&gSaved.Intact()}});
    }
    gSaved.RestoreAll();
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&gSaved.Intact();
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool passed=totals.passed;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"globals_intact",gSaved.Intact()}});
    Write({{"event",ready&&passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
