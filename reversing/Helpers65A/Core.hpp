#pragma once
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>
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
// Nucleo del banco Helpers65A (lote 65). Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute. Plantilla: reversing/D3DResource63 (Gate de ABI, falsos identicos en ambos lados con parche de entrada en el EXE y, si el colaborador esta
// reconstruido en la DLL, tambien en el cuerpo de la DLL).
//
// Banco par a par de CINCO ayudantes del EXE: FloorFloat (0x406CE0), GetAnimHierarchyFromFrameCallback (0x734A70), CDecisionMaker::MakeDecision (0x606060),
// CDecisionMakerTypes::ReadEventIndices (0x5BB9F0) y UserTracksLegacyVector::PushBack (0x4F4620). Original = bytes del EXE con el hook RH de la funcion apagado; candidato = DLL con SOLO
// ese hook encendido. Los colaboradores (CRT floor 0x8219F0; RpHAnimFrameGetHierarchy 0x7C5160 y RwFrameForAllChildren 0x7F0DC0; CDecision::MakeDecision 0x6040D0; CFileMgr::SetDir
// 0x5387D0 / OpenFile 0x538900 / ReadLine 0x5389B0 / CloseFile 0x5389D0 y el sscanf del EXE 0x8220AD; 0x4F3B30 y 0x4F4320 del vector VC6) se sustituyen por falsos identicos en ambos lados
// que registran EN ORDEN sus argumentos (los punteros a la arena como etiquetas) y devuelven/escriben lo que dicta el caso. Ningun llamador del EXE lee EAX/ECX/EDX de salida
// (ver README.md): solo se registran. No se ejecuta E/S real: ningun archivo se abre ni se lee.

namespace H65Live {
using namespace H65Fix;
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
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): por defecto solo se registran.
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
// exe = entrada del EXE; dll = cuerpo de la DLL si el colaborador esta reconstruido y la DLL lo llama directamente (0 = la DLL lo llama por la direccion del EXE).
struct Pair {uintptr_t exe,dll;bool hooked{false};};
// Salta los enlaces de salto (E9) hasta el cuerpo real de la funcion de la DLL (solo mientras caigan dentro del ASI).
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
struct StubSpec {Pair pair;void* fake;};
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> p;
    Patches(){p.reserve(32);}
    void Add(const Pair& pr,void* fake){if(pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.exe,fake));if(pr.dll&&pr.dll!=pr.exe)p.emplace_back(new ReversingProbe::EntryPatch(pr.dll,fake));}
    ~Patches(){while(!p.empty())p.pop_back();}
};
struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};bool collabOK{};};
struct Env {
    uintptr_t asi{},game{};
    std::array<std::unique_ptr<HookRoute>,kFns> route;
    std::array<std::vector<StubSpec>,kFns> stubs;                  // colaboradores sustituidos por funcion
    std::vector<Watch> watches;                                     // entradas del EXE y cuerpos de la DLL de los colaboradores: deben seguir siendo lo que eran al empezar
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// Ruta: original = hook apagado; candidato = SOLO el hook de la ruta encendido (cuerpo en el modulo ASI). Los falsos se instalan DESPUES de comprobar rutas y colaboradores y solo
// mientras dura la llamada.
template<class Fn> Side Execute(Env& env,unsigned fnIdx,uintptr_t entry,bool candidate,Fn gate) {
    Side side{};
    HookRoute& route=*env.route[fnIdx];
    auto call=[&]{
        side.routeOK=candidate?(ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==env.asi):!ReversingProbe::JumpsToDll(entry);
        bool collab=true;
        for(const auto& s:env.stubs[fnIdx]) {
            const auto& pr=s.pair;
            collab&=pr.dll==0?!ReversingProbe::JumpsToDll(pr.exe):ModuleOf(pr.dll)==env.asi;   // reconstruido: su cuerpo esta en la DLL; si no, sigue siendo codigo original del EXE
        }
        side.collabOK=collab;
        {
            Patches patches;
            for(const auto& s:env.stubs[fnIdx])patches.Add(s.pair,s.fake);
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

// ================================================================================================ arena, eventos y falsos
alignas(16) inline std::array<uint8,kArenaBytes> arena{};
inline uintptr_t ArenaBase() {return uintptr_t(arena.data());}
inline u32& ArenaWord(u32 byteOffset) {return *reinterpret_cast<u32*>(arena.data()+byteOffset);}
inline DWORD benchThread{};

// Evento: n valores de 32 bits (los punteros a la arena van como etiquetas, ver cada falso). Nada aqui asigna memoria: se usa dentro de la ventana de parches.
struct Ev {u32 n{};u32 v[10]{};};
inline std::array<Ev,kMaxEvents+8> evs{};inline unsigned nev{};inline Ev evSink{};
template<class... A> inline void Log(A... a) {
    static_assert(sizeof...(A)<=10);
    Ev& e=nev<kMaxEvents?evs[nev]:evSink;
    e=Ev{};
    const u32 values[]{u32(a)...};
    for(unsigned i=0;i<sizeof...(A);++i)e.v[i]=values[i];
    e.n=sizeof...(A);
    ++nev;
}
inline json EventsJson() {
    json out=json::array();
    for(unsigned i=0;i<nev&&i<kMaxEvents;++i){json e=json::array();for(u32 k=0;k<evs[i].n;++k)e.push_back(evs[i].v[k]);out.push_back(e);}
    return out;
}
inline u32 Fnv(const void* p,u32 n){u32 h=2166136261u;const auto* b=static_cast<const u8*>(p);for(u32 i=0;i<n;++i){h^=b[i];h*=16777619u;}return h;}
inline u32 StrLen(const char* s){u32 n=0;while(n<512&&s[n])++n;return n;}
inline u32 StrHash(const char* s){return Fnv(s,StrLen(s));}

// ---- caso en curso (los falsos leen de aqui)
inline SpecFloor curFloor;inline SpecCallback curCb;inline SpecDecision curDec;inline SpecRead curRead;inline SpecPush curPush;
inline uintptr_t cbBodyDll{};   // cuerpo de la DLL de GetAnimHierarchyFromFrameCallback (para etiquetar el callback)

// ---- F0: floor del CRT (0x8219F0): cdecl con un double (dos palabras de pila); devuelve el double del caso en ST0. Un hilo ajeno al banco (audio, streaming) que llegue a
// pasar por el parche recibe el floor de la DLL.
inline double __cdecl FkFloor(u32 lo,u32 hi) {
    if(GetCurrentThreadId()!=benchThread){const u64 b=(u64(hi)<<32)|lo;double d;memcpy(&d,&b,8);return std::floor(d);}
    Log(1,lo,hi);
    const u64 bits=FloorRetBits(curFloor.mode,curFloor.seed,lo,hi);
    double d;memcpy(&d,&bits,8);
    return d;
}

// ---- F1: RpHAnimFrameGetHierarchy (0x7C5160) y RwFrameForAllChildren (0x7F0DC0): arbol de marcos falsos
inline u32 NodeOf(u32 frame) {const u32 off=frame-u32(ArenaBase()+kFrames);return (frame>=ArenaBase()+kFrames&&off<kNodes*kFrameStride&&off%kFrameStride==0)?off/kFrameStride:0xFFu;}
inline u32 CbLabel(u32 cb) {return (cb==0x734A70u||Resolve(cb)==cbBodyDll)?1u:2u;}
inline int cbDepth{};inline unsigned cbTraps{};
inline u32 __cdecl FkHier(u32 frame) {
    const u32 node=NodeOf(frame);
    Log(1,node);
    return node<kNodes?curCb.hier[node]:0xBADBAD01u;
}
inline u32 __cdecl FkChildren(u32 frame,u32 callback,u32 data) {
    const u32 node=NodeOf(frame);
    Log(2,node,CbLabel(callback),data==u32(ArenaBase()+kFound)?1u:2u);
    if(node>=kNodes||++cbDepth>40){++cbTraps;return frame;}
    for(u32 child=1;child<curCb.n;++child) {
        if(curCb.parent[child]!=int(node))continue;
        const u32 childFrame=u32(ArenaBase()+kFrames+child*kFrameStride);
        if(!reinterpret_cast<u32(__cdecl*)(u32,u32)>(callback)(childFrame,data))break;
    }
    --cbDepth;
    return curCb.rg;   // el iterador real devuelve el marco; la funcion no debe usar este valor
}

// ---- F2: CDecision::MakeDecision (0x6040D0): thiscall, 8 argumentos de pila (ret 20h)
inline u32 PtrLabel2(u32 p) {return p==u32(ArenaBase()+kTaskSlot)?1u:p==u32(ArenaBase()+kFacialSlot)?2u:3u;}
inline void __fastcall FkMakeDecision(u32 self,u32,i32 src,i32 veh,i32 a1,i32 a2,i32 a3,i32 seek,u32 taskPtr,u32 facialPtr) {
    Log(1,self-u32(ArenaBase()+kThisZone),src,(veh&0xFF)!=0,a1,a2,a3,seek,PtrLabel2(taskPtr),PtrLabel2(facialPtr));
    if((curDec.wmode&1)&&PtrLabel2(taskPtr)!=3)*reinterpret_cast<i16*>(taskPtr)=i16(curDec.wt);
    if((curDec.wmode&2)&&PtrLabel2(facialPtr)!=3)*reinterpret_cast<i16*>(facialPtr)=i16(curDec.wf);
}

// ---- F3: CFileMgr (SetDir 0x5387D0, OpenFile 0x538900, ReadLine 0x5389B0, CloseFile 0x5389D0) y sscanf del EXE (0x8220AD); cdecl
inline unsigned lineIndex{};
inline i32 __cdecl FkSetDir(const char* path) {Log(1,StrHash(path),StrLen(path));return 0x7A5Bu;}
inline u32 __cdecl FkOpenFile(const char* path,const char* mode) {Log(2,StrHash(path),StrLen(path),StrHash(mode),StrLen(mode));return curRead.tok;}
inline u32 __cdecl FkReadLine(u32 file,char* buf,i32 num) {
    const unsigned k=lineIndex;
    const bool served=k<curRead.lines.size();
    Log(3,file,u32(num),served?u32(k):0xFFFFFFFFu);
    if(!served)return 0xABCDEF00u;                                  // AL = 0 con basura en los bits altos
    ++lineIndex;
    const std::string& t=curRead.lines[k].text;
    memcpy(buf,t.data(),t.size());buf[t.size()]=0;
    return 0x12345601u;                                              // AL = 1 con basura en los bits altos
}
inline i32 __cdecl FkSscanf(const char* buf,const char* fmt,char* word,i32* index) {
    const unsigned k=lineIndex-1;
    const uintptr_t b=uintptr_t(buf),w=uintptr_t(word),p=uintptr_t(index);
    const bool distinct=!(w>=b&&w<b+256)&&!(p>=b&&p<b+256)&&(p+4<=w||w+4<=p);
    Log(4,StrHash(buf),StrLen(buf),StrHash(fmt),StrLen(fmt),distinct);
    if(k>=curRead.lines.size())return 0;
    const Line& l=curRead.lines[k];
    if(l.write){word[0]='E';word[1]='V';word[2]=0;*index=l.idx;}
    return i32(l.ret);
}
inline i32 __cdecl FkCloseFile(u32 file) {Log(5,file);return 0;}

// ---- F4: 0x4F3B30 (cdecl, 5 argumentos) y 0x4F4320 (thiscall, 3 argumentos de pila, ret 0Ch) del vector VC6
inline u32 PtrLabel4(u32 p) {return p==u32(ArenaBase()+kVector)?1u:p==u32(ArenaBase()+kValueSlot)?2u:3u;}
inline void PushMutate() {
    switch(curPush.fk) {
    case 1: ArenaWord(kVector+8)=curPush.fv;break;     // last
    case 2: ArenaWord(kVector+4)=curPush.fv;break;     // first
    case 3: ArenaWord(kVector+12)=curPush.fv;break;    // end
    default: break;
    }
}
inline void __cdecl Fk3B30(u32 dest,u32 n,u32 src,u32 a4,u32 a5) {Log(1,dest,n,PtrLabel4(src),PtrLabel4(a4),PtrLabel4(a5));PushMutate();}
inline void __fastcall Fk4320(u32 self,u32,u32 pos,u32 n,u32 src) {Log(2,PtrLabel4(self),pos,n,PtrLabel4(src));PushMutate();}

// ================================================================================================ ejecucion de los casos
inline constexpr const char* kFn[kFns]{"CHud::FloorFloat","RwHelper::GetAnimHierarchyFromFrameCallback","CDecisionMaker::MakeDecision","CDecisionMakerTypes::ReadEventIndices","UserTracksLegacyVector::PushBack"};
inline constexpr uintptr_t kEntry[kFns]{0x406CE0,0x734A70,0x606060,0x5BB9F0,0x4F4620};

inline json ChangesJson(const std::array<u32,kArenaWords>& pre) {
    json out=json::array();
    for(u32 w=0;w<kArenaWords;++w){const u32 v=ArenaWord(w*4);if(v!=pre[w])out.push_back(json::array({w,v}));}
    return out;
}
inline json X87Json(const std::array<uint8,108>& fp) {
    unsigned depth=0;for(unsigned st=0;st<8;++st)depth+=!FpRegisterIsDead(fp,st);
    return {{"depth",depth},{"st0",depth&&!FpRegisterIsDead(fp,0)?HexBytes(fp.data()+28,10):std::string()}};
}

inline void RunCase(Env& env,Totals& t,u32 fn,unsigned n) {
    json input=json::parse(JsonCase(fn,n));
    std::array<u32,kArenaWords> img{};
    BuildImage(fn,n,img);
    Side sides[2];json rows[2];
    for(int sd=0;sd<2;++sd) {
        const bool candidate=sd==1;
        PrepareFloatingState();
        nev=0;lineIndex=0;cbDepth=0;cbTraps=0;
        memcpy(arena.data(),img.data(),kArenaBytes);
        const uintptr_t A=ArenaBase();
        u32 args[10]{};u32 argc=0,popBytes=0,eax=0,ecx=0,edx=0,stack=0;
        switch(fn) {
        case kFloor: {curFloor=MakeFloor(n);args[0]=curFloor.x;argc=1;popBytes=4;eax=curFloor.eax;ecx=curFloor.ecx;edx=curFloor.edx;stack=curFloor.stack;break;}
        case kCallback: {curCb=MakeCallback(n);args[0]=u32(A+kFrames+curCb.root*kFrameStride);args[1]=u32(A+kFound);argc=2;popBytes=8;eax=curCb.eax;ecx=curCb.ecx;edx=curCb.edx;stack=curCb.stack;break;}
        case kDecisionMaker: {
            curDec=MakeDecision(n);
            args[0]=u32(curDec.idx);args[1]=u32(curDec.src);args[2]=curDec.veh;args[3]=u32(curDec.a1);args[4]=u32(curDec.a2);args[5]=u32(curDec.a3);args[6]=u32(curDec.seek);
            args[7]=u32(A+kTaskSlot);args[8]=u32(A+kFacialSlot);argc=9;popBytes=0;ecx=u32(A+kThisZone);eax=curDec.eax;edx=curDec.edx;stack=curDec.stack;break;
        }
        case kReadEvents: {curRead=MakeRead(n);args[0]=u32(A+kIndices);args[1]=u32(A+kNameZone);argc=2;popBytes=0;eax=curRead.eax;ecx=curRead.ecx;edx=curRead.edx;stack=curRead.stack;break;}
        default: {curPush=MakePush(n);args[0]=u32(A+kValueSlot);argc=1;popBytes=0;ecx=u32(A+kVector);eax=curPush.eax;edx=curPush.edx;stack=curPush.stack;break;}
        }
        sides[sd]=Execute(env,fn,kEntry[fn],candidate,[&](uintptr_t entry){return Gate(entry,args,argc,eax,ecx,edx,stack,popBytes);});
        json obs;
        obs["events"]=EventsJson();obs["nevents"]=nev;
        if(fn==kFloor)obs["x87"]=X87Json(sides[sd].fp);
        if(fn==kCallback) {
            const u32 r=sides[sd].reg.eax;
            if(r==0)obs["ret"]=-1;else if(NodeOf(r)<kNodes)obs["ret"]=NodeOf(r);else obs["ret"]=1000000000ull+r;
            obs["traps"]=cbTraps;
        }
        obs["changes"]=ChangesJson(img);
        auto fp=NormalizeFp(sides[sd].fp);
        const bool watchesIntact=env.WatchesIntact();
        json out=CommonRow(sides[sd],fp);out["observed"]=obs;
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact();
        out["floating_state_restored"]=FloatingStateRestored();
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
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",kFns},{"pairs",kFns*kCases},{"cases",cases},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",collaborators},{"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},{"arena_bytes",kArenaBytes}});
    Totals totals;
    if(ready) {
        for(u32 fn=0;fn<kFns;++fn) {
            const unsigned before=totals.failures;
            for(unsigned c=0;c<kCases;++c)RunCase(env,totals,fn,c);
            Write({{"event","function_summary"},{"function",kFn[fn]},{"cases",kCases},{"failures",totals.failures-before},{"state_restored",env.WatchesIntact()}});
        }
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact;
    for(auto& r:env.route)if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
