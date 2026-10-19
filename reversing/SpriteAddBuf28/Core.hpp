#pragma once
#include <array>
#include <bit>
#include <fstream>
#include <filesystem>
#include <memory>
#include <string>
#include <vector>
#include "Fixtures.hpp"

// Nucleo del banco SpriteAddBuf28. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages}
// y OriginalDependencies499::HookRoute. Plantilla: reversing/ModifySpeed27 (Core.hpp).
//
// Banco par a par de CSprite2d::AddToBuffer (0x728200, cdecl, 10 dwords: &posn, &color, u1 v1 u2 v2 u3 v3 u4 v4; `ret` sin limpiar). Original = bytes del EXE con el hook RH
// apagado; candidato = DLL con el hook encendido.
// Colaboradores (se sustituyen por falsos IDENTICOS en el EXE y en el cuerpo de la DLL):
//   0x727A00 CSprite2d::SetVertices(RwD3D9Vertex*, const CRect&, 4 x const CRGBA&, 8 floats): cdecl, 14 dwords. El falso registra TODOS los argumentos (bits; los punteros de color
//            como el dword apuntado + mascara de igualdad de punteros) y escribe 28 dwords derivados de ellos en los 4 vertices (si la ventana cabe en el bufer).
//   0x7273D0 CSprite2d::RenderVertexBuffer(): el EXE lo alcanza con `jmp` (llamada de cola). El falso registra contadores y CRC32 de los bufers (parte usada) y, como el
//            original, pone a 0 ambos contadores solo si nextBufferVertex > 0.
// IsVertexBufferFull (0x7273A0) va en linea en el EXE (cmp/jg) y en C++ en la DLL: es una funcion pura de dos enteros, no se sustituye.
// La funcion no usa x87 (los floats se copian como dwords): el estado x87 se compara normalizado (ver NormalizeFp) y los flags de MXCSR se ignoran.
namespace SpriteAddBuf28Live {
using namespace SpriteAddBuf28Fix;
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
// La funcion del EXE no ejecuta x87 y la DLL puede usar SSE: se compara lo observable (FCW completa, TOP, etiquetas y ST validos); se ignoran banderas de FSW, FIP/FDP y registros vacios.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Los cuatro llamadores del EXE (0x718C2A, 0x718D05, 0x718D7E, 0x718E39, todos en CFont::PrintChar) hacen
// `add esp,28h` justo despues y no leen EAX/ECX/EDX; la funcion es void: por defecto solo se registran en original_residue/candidate_residue.
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
// argsPtr[0] queda en la cima de la pila. EFLAGS justo tras el `call` (para comprobar DF=0).
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

// ------------------------------------------------------------------------------------------------ colaboradores falsos
struct Ctx {std::vector<u32> trace;u32 setCalls{};u32 renderCalls{};};
inline Ctx ctx;
inline u8* VertexBuffer() {return reinterpret_cast<u8*>(uintptr_t(kVertexBufferAddress));}
inline u8* IndexBuffer() {return reinterpret_cast<u8*>(uintptr_t(kIndexBufferAddress));}
inline i32& NextVertex() {return *reinterpret_cast<i32*>(uintptr_t(kNextVertexAddress));}
inline i32& NextIndex() {return *reinterpret_cast<i32*>(uintptr_t(kNextIndexAddress));}
// 0x727A00 (cdecl): traza = [3, off, rect x4, dword de cada color x4, mascara de punteros iguales a color1, u1 v1 u2 v2 u3 v3 u4 v4] (19 dwords) con
// off = (puntero - 0xC5F958) mod 2^32. Si los 4 vertices caben en el bufer, escribe 28 dwords: Mix(h + k*0x9E3779B1), h = cadena Mix(h ^ dword) desde 0x811C9DC5 sobre traza[1:].
inline void __cdecl FakeSetVertices(u8* verts,const u32* rect,const u32* c1,const u32* c2,const u32* c3,const u32* c4,u32 u1,u32 v1,u32 u2,u32 v2,u32 u3,u32 v3,u32 u4,u32 v4) {
    ++ctx.setCalls;
    const u32 off=u32(uintptr_t(verts))-kVertexBufferAddress;
    const u32 words[18]{off,rect[0],rect[1],rect[2],rect[3],*c1,*c2,*c3,*c4,u32(1u|(c2==c1?2u:0u)|(c3==c1?4u:0u)|(c4==c1?8u:0u)),u1,v1,u2,v2,u3,v3,u4,v4};
    ctx.trace.push_back(3);
    u32 h=0x811C9DC5u;
    for(const u32 w:words){ctx.trace.push_back(w);h=Mix(h^w);}
    if(off<=kVertexBufferBytes-kWindowBytes){
        u32 out[28];
        for(u32 k=0;k<28;++k)out[k]=Mix(h+k*0x9E3779B1u);
        memcpy(verts,out,sizeof(out));
    }
}
// 0x7273D0 (cdecl, sin argumentos): traza = [2, nextBufferVertex, nextBufferIndex, crc32(vertices usados), crc32(indices usados)]; reinicia contadores solo si nextBufferVertex > 0.
inline void __cdecl FakeRenderVertexBuffer() {
    ++ctx.renderCalls;
    const i32 v=NextVertex(),i=NextIndex();
    const u32 nv=v<=0?0u:v>i32(kVertexCount)?kVertexCount:u32(v);
    const u32 ni=i<=0?0u:i>i32(kIndexCount)?kIndexCount:u32(i);
    ctx.trace.push_back(2);ctx.trace.push_back(u32(v));ctx.trace.push_back(u32(i));
    ctx.trace.push_back(Crc32(VertexBuffer(),size_t(nv)*kVertexBytes));ctx.trace.push_back(Crc32(IndexBuffer(),size_t(ni)*2));
    if(v>0){NextVertex()=0;NextIndex()=0;}
}
using SetVerticesFn=void(__cdecl*)(u8*,const u32*,const u32*,const u32*,const u32*,const u32*,u32,u32,u32,u32,u32,u32,u32,u32);

// ------------------------------------------------------------------------------------------------ memoria del caso
alignas(16) inline std::array<u8,kArgsArena> argArena{};
struct Saved {std::vector<u8> vertices,indices,guardBefore,guardAfter;i32 nextVertex{},nextIndex{};};
inline Saved saved;

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};};
struct Env {
    uintptr_t asi{},game{};
    unsigned cases{kCases};
    std::unique_ptr<HookRoute> route;
    uintptr_t entry{0x728200};
    uintptr_t dllSetVertices{},dllRender{};   // cuerpos de la DLL de los colaboradores (la DLL los llama directamente)
    std::vector<Watch> watches;
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

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
inline void RestoreGlobals() {
    memcpy(VertexBuffer(),saved.vertices.data(),kVertexBufferBytes);memcpy(IndexBuffer(),saved.indices.data(),kIndexBufferBytes);
    NextVertex()=saved.nextVertex;NextIndex()=saved.nextIndex;
}
inline bool GlobalsRestored() {
    return memcmp(VertexBuffer(),saved.vertices.data(),kVertexBufferBytes)==0&&memcmp(IndexBuffer(),saved.indices.data(),kIndexBufferBytes)==0
        &&NextVertex()==saved.nextVertex&&NextIndex()==saved.nextIndex
        &&memcmp(IndexBuffer()-kGuardBytes,saved.guardBefore.data(),kGuardBytes)==0&&memcmp(IndexBuffer()+kIndexBufferBytes,saved.guardAfter.data(),kGuardBytes)==0;
}

// Un caso: CSprite2d::AddToBuffer(posn, color, uv...) con los bufers del EXE rellenos con un patron conocido y los contadores del caso.
inline void CaseAB(Env& env,Totals& t,unsigned n) {
    const F f=MakeF(n);
    std::array<u8,kArgsArena> argRef{};argRef.fill(0xC3);
    memcpy(argRef.data()+kRectOffset,f.rect,16);memcpy(argRef.data()+kColorOffset,&f.color,4);
    std::vector<u8> vRef(kVertexBufferBytes),iRef(kIndexBufferBytes);
    for(u32 i=0;i<kVertexBufferBytes;++i)vRef[i]=u8(i*7u+f.vseed);
    for(u32 i=0;i<kIndexBufferBytes;++i)iRef[i]=u8(i*13u+f.iseed);
    const u32 voff=u32(f.vertex)*kVertexBytes;                       // desplazamiento (mod 2^32) del primer vertice
    const bool vInBuffer=voff<=kVertexBufferBytes-kWindowBytes;
    const bool iInBuffer=f.index>=0&&f.index<=i32(kIndexCount)-6;
    u8* const rectPtr=argArena.data()+kRectOffset;u8* const colorPtr=argArena.data()+kColorOffset;
    const u32 args[10]{u32(uintptr_t(rectPtr)),u32(uintptr_t(colorPtr)),f.uv[0],f.uv[1],f.uv[2],f.uv[3],f.uv[4],f.uv[5],f.uv[6],f.uv[7]};
    json rectJson=json::array(),uvJson=json::array();
    for(const u32 v:f.rect)rectJson.push_back(v);
    for(const u32 v:f.uv)uvJson.push_back(v);
    const json input={{"kind",f.kind},{"vertex",f.vertex},{"index",f.index},{"rect",rectJson},{"color",f.color},{"uv",uvJson},{"vseed",f.vseed},{"iseed",f.iseed},
                      {"eax",f.eax},{"ecx",f.ecx},{"edx",f.edx},{"stack_seed",f.stack}};
    Side sides[2];json rows[2];
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);
        ctx.trace.clear();ctx.setCalls=0;ctx.renderCalls=0;
        argArena=argRef;memcpy(VertexBuffer(),vRef.data(),kVertexBufferBytes);memcpy(IndexBuffer(),iRef.data(),kIndexBufferBytes);
        NextVertex()=f.vertex;NextIndex()=f.index;
        sides[s]=Execute(env,candidate,[&](uintptr_t entry){
            ReversingProbe::EntryPatch exeSet(0x727A00,&FakeSetVertices),dllSet(env.dllSetVertices,&FakeSetVertices);
            ReversingProbe::EntryPatch exeRender(0x7273D0,&FakeRenderVertexBuffer),dllRender(env.dllRender,&FakeRenderVertexBuffer);
            return Gate(entry,args,10,f.eax,f.ecx,f.edx,f.stack,40);
        });
        // ---- observado
        const i32 vAfter=NextVertex(),iAfter=NextIndex();
        std::vector<u8> vTmp(VertexBuffer(),VertexBuffer()+kVertexBufferBytes),iTmp(IndexBuffer(),IndexBuffer()+kIndexBufferBytes);
        std::string vWin,iWin;
        if(vInBuffer){vWin=HexBytes(vTmp.data()+voff,kWindowBytes);memcpy(vTmp.data()+voff,vRef.data()+voff,kWindowBytes);}
        if(iInBuffer){iWin=HexBytes(iTmp.data()+size_t(f.index)*2,12);memcpy(iTmp.data()+size_t(f.index)*2,iRef.data()+size_t(f.index)*2,12);}
        const bool outsideOk=vTmp==vRef&&iTmp==iRef;
        const bool guardOk=argArena==argRef&&memcmp(IndexBuffer()-kGuardBytes,saved.guardBefore.data(),kGuardBytes)==0&&memcmp(IndexBuffer()+kIndexBufferBytes,saved.guardAfter.data(),kGuardBytes)==0;
        json observed={{"vertex_after",vAfter},{"index_after",iAfter},{"index_window",iWin},{"vertex_window",vWin},{"outside_ok",outsideOk},{"guard_ok",guardOk},{"trace",ctx.trace},
                       {"calls",{ctx.setCalls,ctx.renderCalls}}};
        if(kCompareVolatileRegisters){observed["eax"]=sides[s].reg.eax;observed["ecx"]=sides[s].reg.ecx;observed["edx"]=sides[s].reg.edx;}
        auto fp=NormalizeFp(sides[s].fp);
        const bool watchesIntact=env.WatchesIntact();
        RestoreGlobals();
        json out={{"observed",observed},{"post_floating_state",HexBytes(fp.data(),108)},{"post_mxcsr_control",sides[s].mxcsr&0xFFC0u},{"stack_restored",sides[s].reg.stack},
                  {"nonvolatile_preserved",sides[s].reg.nonvolatile},{"direction_flag_clear",(sides[s].reg.eflags&0x400u)==0},{"route_ok",sides[s].routeOK},
                  {"traps_zero",ctx.setCalls==1&&ctx.renderCalls<=1},{"calls",{ctx.setCalls,ctx.renderCalls}}};
        RestoreFloatingState();
        out["globals_restored"]=watchesIntact&&env.WatchesIntact()&&GlobalsRestored();
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    bool ok=rows[0]==rows[1];
    for(const char* k:{"stack_restored","nonvolatile_preserved","direction_flag_clear","route_ok","traps_zero","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
    const auto fa=NormalizeFp(sides[0].fp),fb=NormalizeFp(sides[1].fp);
    t.passed&=ok;t.failures+=!ok;
    Write({{"event","comparison"},{"function","CSprite2d::AddToBuffer"},{"case",n},{"pair",t.pairs++},{"input",input},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",FpDifference(fa,fb)},
           {"original_residue",ResidueJson(sides[0].reg)},{"candidate_residue",ResidueJson(sides[1].reg)},{"equal",ok},{"accepted",ok}});
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    const uintptr_t body=env.route?env.route->Body():0;
    const bool collaboratorsOK=env.dllSetVertices&&env.dllRender&&ModuleOf(env.dllSetVertices)==env.asi&&ModuleOf(env.dllRender)==env.asi;
    ready&=collaboratorsOK&&body&&ModuleOf(body)==env.asi;
    for(const uintptr_t at:{uintptr_t(0x728200),uintptr_t(0x727A00),uintptr_t(0x7273D0),env.dllSetVertices,env.dllRender})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    saved.vertices.assign(VertexBuffer(),VertexBuffer()+kVertexBufferBytes);saved.indices.assign(IndexBuffer(),IndexBuffer()+kIndexBufferBytes);
    saved.guardBefore.assign(IndexBuffer()-kGuardBytes,IndexBuffer());saved.guardAfter.assign(IndexBuffer()+kIndexBufferBytes,IndexBuffer()+kIndexBufferBytes+kGuardBytes);
    saved.nextVertex=NextVertex();saved.nextIndex=NextIndex();
    ctx.trace.reserve(64);
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",1},{"pairs",env.cases},{"cases",{{"CSprite2d::AddToBuffer",env.cases}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe",std::vector<uintptr_t>{0x727A00,0x7273D0}},{"dll",std::vector<uintptr_t>{env.dllSetVertices,env.dllRender}}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},
        {"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},
        {"initial_vertex_hash",Fnv64(saved.vertices.data(),saved.vertices.size())},{"initial_index_hash",Fnv64(saved.indices.data(),saved.indices.size())},
        {"initial_next_vertex",saved.nextVertex},{"initial_next_index",saved.nextIndex}});
    Totals totals;
    if(ready) {
        for(unsigned c=0;c<env.cases;++c)CaseAB(env,totals,c);
        Write({{"event","function_summary"},{"function","CSprite2d::AddToBuffer"},{"cases",env.cases},{"failures",totals.failures},{"state_restored",env.WatchesIntact()}});
    }
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&GlobalsRestored();
    if(env.route)stateRestored&=env.route->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},
        {"vertex_hash",Fnv64(VertexBuffer(),kVertexBufferBytes)},{"index_hash",Fnv64(IndexBuffer(),kIndexBufferBytes)},{"next_vertex",NextVertex()},{"next_index",NextIndex()}});
    Write({{"event",ready&&totals.passed&&stateRestored?"complete":"error"},{"pairs",totals.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
