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
#include "Arenas.hpp"

// Nucleo del banco PedWrappers60. Requiere, declarado antes: json (nlohmann), uint8/uint32/int32, ReversingProbe::{EntryPatch,Bytes,JumpsToDll,PumpMessages},
// OriginalDependencies499::HookRoute y los tipos del juego (LiveComparison.hpp los incluye). Plantilla: reversing/FightProcessPed31 (hook Virtual con falsos).
//
// Tres funciones virtuales de CPed (hooks Virtual; el hook Virtual parchea tambien la entrada del EXE con un salto al cuerpo de la DLL):
//   CPed::SpecialEntityCalcCollisionSteps 0x5E3E90 (sin colaboradores), CPed::SpecialEntityPreCollisionStuff 0x5E3C30 (falso: CPedIntelligence::GetTaskClimb 0x601180), CPed::ProcessControl 0x5E8CD0 (32 falsos).
// Original = bytes del EXE con el hook RH apagado; candidato = DLL con SOLO ese hook encendido. Todo lo que las funciones tocan es memoria FALSA del banco (arenas con guardas, rellenas con basura
// determinista) o globales que el banco fija por caso y restaura. Los colaboradores se sustituyen por el mismo falso en ambos lados: parche de entrada en el EXE y, si existe, en el cuerpo de la
// DLL (se localiza recorriendo la tabla de hooks por la direccion del EXE: es la funcion que llama el codigo de la DLL). Los falsos registran sus argumentos en orden en una traza.
// La rand del EXE (0x821B1E) es la real, sembrada con srand del EXE (0x821B11); el estado se restaura tras cada caso.
namespace PedWrappers60Live {
using namespace PedWrappers60Fix;
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
// Las funciones usan x87 (y la DLL puede usar SSE): se compara lo observable (FCW completa, TOP, etiquetas y ST validos); no los registros marcados vacios, ni las banderas de FSW, ni FIP/FDP.
inline constexpr bool kCompareDeadX87Registers=false;
// true: EAX/ECX/EDX de salida forman parte de la igualdad. Ningun llamador del EXE los lee (ver README.md): por defecto solo se registran (en CalcCollisionSteps se compara AL como retorno).
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
// `seh`: la cadena de manejadores (fs:[0]) es la misma tras la llamada.
struct Reg {u32 eax,ecx,edx,eflags;bool stack,nonvolatile,seh;};
// Siembra 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX; empuja los argumentos (argsPtr[0] queda en la cima); captura los registros y EFLAGS justo tras el `call`. `popBytes` = lo que el LLAMADOR debe quitar
// (0 para thiscall con `ret N`: el llamado limpia su pila y ESP debe coincidir).
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

// ------------------------------------------------------------------------------------------------ RNG del EXE
inline void ExeSrand(u32 seed) {reinterpret_cast<void(__cdecl*)(u32)>(0x821B11)(seed);}
inline u32* ExeRandState() {return reinterpret_cast<u32*>(reinterpret_cast<uint8*(__cdecl*)()>(0x827B3D)()+0x14);}
inline u32 NextRand(u32 s) {return s*0x343FDu+0x269EC3u;}

// Entrada del EXE y cuerpo de la DLL de un colaborador. El cuerpo de la DLL (la funcion que llama el codigo de la DLL) se localiza por la direccion del EXE recorriendo la tabla de hooks
// (Simple o Virtual): es la direccion "nuestra" del hook; se sigue cualquier enlace de salto (E9) hacia fuera del EXE hasta el cuerpo real. Se resuelve ANTES de escribir ningun parche.
struct Pair {uintptr_t exe,dll;};
inline uintptr_t Body(uintptr_t p){for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);if(t>=0x400000&&t<0x1000000)break;p=t;}return p;}
inline uintptr_t OurByExe(uintptr_t exe){
    uintptr_t found=0;
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& category){
        for(auto& hook:category.Items()){
            if(!strcmp(hook->Symbol(),"S")){auto* s=static_cast<ReversibleHooks::ReversibleHook::Simple*>(hook.get());if(uintptr_t(s->GetHookGTAAddress())==exe)found=uintptr_t(s->GetHookOurAddress());}
            else if(!strcmp(hook->Symbol(),"V")){auto* v=static_cast<ReversibleHooks::ReversibleHook::Virtual*>(hook.get());if(uintptr_t(v->GetHookGTAAddress())==exe)found=uintptr_t(v->GetHookOurAddress());}
        }
    });
    return found;
}
inline Pair MakePair(uintptr_t exe,bool hasDll){const uintptr_t our=hasDll?OurByExe(exe):0;return Pair{exe,our?Body(our):0};}

struct Patches {
    std::array<std::optional<ReversingProbe::EntryPatch>,160> p;int n{};
    template<class F> void Add(const Pair& pr,F stub){if(pr.exe)p[n++].emplace(pr.exe,stub);if(pr.dll&&pr.dll!=pr.exe)p[n++].emplace(pr.dll,stub);}
    void Reset(){for(int i=n-1;i>=0;--i)p[i].reset();n=0;}
    ~Patches(){Reset();}
};
struct Stubs {
    Pair aeService,swim,climb,getHier,boneIdx,matArr,createFx,fxLocal,fxCopy,fxPlay,fxDestroy,coors,prune,getAlpha,setAlpha,colModels,findPlayer,isMember,buoy,pFirst,pProc,skip,physPc,delW,foot,newVel,updPos,blood,shadow,pedCount,say,waeAdd,waeService;
    std::vector<const Pair*> All(bool climbOnly)const{
        if(climbOnly)return {&climb};
        return {&aeService,&swim,&climb,&getHier,&boneIdx,&matArr,&createFx,&fxLocal,&fxCopy,&fxPlay,&fxDestroy,&coors,&prune,&getAlpha,&setAlpha,&colModels,&findPlayer,&isMember,&buoy,&pFirst,&pProc,&skip,&physPc,
                &delW,&foot,&newVel,&updPos,&blood,&shadow,&pedCount,&say,&waeAdd,&waeService};
    }
    void Resolve(){
        aeService=MakePair(0x4E2EE0,true);swim=MakePair(0x601070,true);climb=MakePair(0x601180,true);getHier=MakePair(0x734A40,true);boneIdx=MakePair(0x7C51A0,false);matArr=MakePair(0x7C5120,false);createFx=MakePair(0x4A9BE0,true);
        fxLocal=MakePair(0x4AA910,true);fxCopy=MakePair(0x4AA890,true);fxPlay=MakePair(0x4AA2F0,true);fxDestroy=MakePair(0x4A9810,true);coors=MakePair(0x56E010,true);prune=MakePair(0x571A90,true);
        getAlpha=MakePair(0x732B20,true);setAlpha=MakePair(0x732B00,true);colModels=MakePair(0x4185C0,true);findPlayer=MakePair(0x56E210,true);isMember=MakePair(0x5F6A10,true);buoy=MakePair(0x5E1FA0,true);
        pFirst=MakePair(0x6073A0,true);pProc=MakePair(0x608260,true);skip=MakePair(0x5433B0,true);physPc=MakePair(0x5485E0,true);delW=MakePair(0x5E8910,true);foot=MakePair(0x5E57F0,true);
        newVel=MakePair(0x5E4C50,true);updPos=MakePair(0x5E1B10,true);blood=MakePair(0x56D230,true);shadow=MakePair(0x706F60,true);pedCount=MakePair(0x611570,true);say=MakePair(0x5EFFE0,true);
        waeAdd=MakePair(0x4E69F0,true);waeService=MakePair(0x4E6AE0,true);
    }
    void Install(Patches& p,bool climbOnly)const{
        p.Add(climb,&FakeClimb);
        if(climbOnly)return;
        p.Add(swim,&FakeSwim);
        p.Add(aeService,&FakeAeService);p.Add(getHier,&FakeGetHier);p.Add(boneIdx,&FakeBoneIdx);p.Add(matArr,&FakeMatArr);p.Add(createFx,&FakeCreateFx);p.Add(fxLocal,&FakeFxLocal);p.Add(fxCopy,&FakeFxCopy);
        p.Add(fxPlay,&FakeFxPlay);p.Add(fxDestroy,&FakeFxDestroy);p.Add(coors,&FakeCoors);p.Add(prune,&FakePrune);p.Add(getAlpha,&FakeGetAlpha);p.Add(setAlpha,&FakeSetAlpha);p.Add(colModels,&FakeColModels);
        p.Add(findPlayer,&FakeFindPlayer);p.Add(isMember,&FakeIsMember);p.Add(buoy,&FakeBuoy);p.Add(pFirst,&FakePFirst);p.Add(pProc,&FakePProc);p.Add(skip,&FakeSkip);p.Add(physPc,&FakePhysPc);p.Add(delW,&FakeDelW);
        p.Add(foot,&FakeFoot);p.Add(newVel,&FakeNewVel);p.Add(updPos,&FakeUpdPos);p.Add(blood,&FakeBlood);p.Add(shadow,&FakeShadow);p.Add(pedCount,&FakePedCount);p.Add(say,&FakeSay);p.Add(waeAdd,&FakeWaeAdd);
        p.Add(waeService,&FakeWaeService);
    }
};

struct Side {Reg reg{};std::array<uint8,108> fp{};uint32 mxcsr{};bool routeOK{};u32 rngState{};};
struct Env {
    uintptr_t asi{},game{};
    std::unique_ptr<HookRoute> routeCS,routePS,routePC;
    Stubs stubs;
    std::vector<Watch> watches;       // entradas del EXE (parcheadas solo mientras dura una seccion, con RAII): deben seguir siendo lo que eran
    bool WatchesIntact()const{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});}
};
struct Totals {unsigned pairs{};unsigned failures{};bool passed{true};};
inline unsigned gPairs{};      // numero de par global (el analizador comun exige 0..N-1 sin repeticiones entre las tres funciones)
inline json ResidueJson(const Reg& r){return {{"eax",r.eax},{"ecx",r.ecx},{"edx",r.edx},{"eflags",r.eflags}};}

// ---- globales de ProcessControl: se fijan por caso y se restauran
struct Globals {
    u32 frame{},timestep{},area{},camMat{},bloodTex{},rngState{};u8 force{};std::array<u32,3> camPos{};std::vector<std::pair<u32,u32>> modelTable;bool saved{};
    void Save(){frame=*reinterpret_cast<u32*>(kGFrame);timestep=*reinterpret_cast<u32*>(kGTimestep);area=*reinterpret_cast<u32*>(kGArea);force=*reinterpret_cast<u8*>(kGForce);camMat=*reinterpret_cast<u32*>(kGCamMat);
        std::memcpy(camPos.data(),reinterpret_cast<void*>(kGCamPos),12);bloodTex=*reinterpret_cast<u32*>(kGBloodTex);rngState=*ExeRandState();saved=true;}
    void RestoreAll()const{if(!saved)return;*reinterpret_cast<u32*>(kGFrame)=frame;*reinterpret_cast<u32*>(kGTimestep)=timestep;*reinterpret_cast<u32*>(kGArea)=area;*reinterpret_cast<u8*>(kGForce)=force;
        *reinterpret_cast<u32*>(kGCamMat)=camMat;std::memcpy(reinterpret_cast<void*>(kGCamPos),camPos.data(),12);*reinterpret_cast<u32*>(kGBloodTex)=bloodTex;*ExeRandState()=rngState;}
    bool Intact()const{if(!saved)return true;return *reinterpret_cast<u32*>(kGFrame)==frame&&*reinterpret_cast<u32*>(kGTimestep)==timestep&&*reinterpret_cast<u32*>(kGArea)==area&&*reinterpret_cast<u8*>(kGForce)==force
        &&*reinterpret_cast<u32*>(kGCamMat)==camMat&&std::memcmp(reinterpret_cast<void*>(kGCamPos),camPos.data(),12)==0&&*reinterpret_cast<u32*>(kGBloodTex)==bloodTex&&*ExeRandState()==rngState;}
};
inline Globals gSaved;

// Ruta: original = hook apagado; candidato = SOLO el hook de la funcion encendido (cuerpo en el modulo ASI).
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

// ------------------------------------------------------------------------------------------------ CPed::SpecialEntityCalcCollisionSteps (0x5E3E90)
inline void CaseCS(Env& env,Totals& t,unsigned n) {
    const KCase c=MakeK(n);
    gP=nullptr;gS=nullptr;gRet=Rand(n,59);nev=0;
    InitSlots();
    ApplyPokes(slots[L_PED],c.ped,Rand(n,50));
    FillWords(slots[L_BOOLS].ref,slots[L_BOOLS].arena,Rand(n,51));
    for(int i=0;i<2;++i)slots[L_BOOLS].ref[kBoolsOff+i]=u8(c.bools[i]);
    json input=json::parse(JsonK(c));
    {json addrs=json::array();for(u32 l=0;l<L_COUNT;++l)addrs.push_back(AddrOf(l));input["addrs"]=addrs;}      // direcciones reales de las arenas (el oraculo informa los punteros resueltos)
    Side sides[2];json rows[2];
    const u32 thisPtr=AddrOf(L_PED);
    const u32 args[2]{AddrOf(L_BOOLS),AddrOf(L_BOOLS)+1};
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        for(auto& sl:slots)if(sl.now)std::memcpy(sl.now,sl.ref,sl.arena);
        *reinterpret_cast<u32*>(kGTimestep)=c.timestep;
        sides[s]=Execute(*env.routeCS,0x5E3E90,env.asi,candidate,[&](uintptr_t entry){return Gate(entry,args,2,c.eax,thisPtr,c.edx,c.stack,0);});
        json obs;
        obs["ret"]=sides[s].reg.eax&0xFFu;
        json changes=json::array();Diff(changes,0,slots[L_PED]);
        obs["changes"]=changes;
        obs["bools"]={slots[L_BOOLS].now[kBoolsOff],slots[L_BOOLS].now[kBoolsOff+1]};
        obs["bools_unchanged"]=SameArena(slots[L_BOOLS]);
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        const bool tsNow=*reinterpret_cast<u32*>(kGTimestep)==c.timestep;
        gSaved.RestoreAll();
        RestoreFloatingState();
        out["globals_restored"]=env.WatchesIntact()&&gSaved.Intact()&&tsNow;
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    Finish(t,"CPed::SpecialEntityCalcCollisionSteps",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ CPed::SpecialEntityPreCollisionStuff (0x5E3C30)
inline void CasePS(Env& env,Totals& t,unsigned n) {
    const SCase c=MakeS(n);
    gP=nullptr;gS=&c;gRet=Rand(n,59);nev=0;
    InitSlots();
    ApplyPokes(slots[L_PED],c.ped,Rand(n,50));ApplyPokes(slots[L_ENT],c.ent,Rand(n,51));ApplyPokes(slots[L_OBJINFO],c.objinfo,Rand(n,52));
    ApplyPokes(slots[L_MATPED],c.matPed,Rand(n,53));ApplyPokes(slots[L_MATOTHER],c.matEnt,Rand(n,54));
    FillWords(slots[L_BOOLS].ref,slots[L_BOOLS].arena,Rand(n,55));
    for(int i=0;i<4;++i)slots[L_BOOLS].ref[kBoolsOff+i]=u8(c.bools[i]);
    FillWords(slots[L_INTEL].ref,slots[L_INTEL].arena,Rand(n,56));
    json input=json::parse(JsonS(c));
    {json addrs=json::array();for(u32 l=0;l<L_COUNT;++l)addrs.push_back(AddrOf(l));input["addrs"]=addrs;}      // direcciones reales de las arenas (el oraculo informa los punteros resueltos)
    Side sides[2];json rows[2];
    const u32 thisPtr=AddrOf(L_PED);
    // argumentos: colPhysical, bIgnoreStuckCheck (dword completo), cuatro referencias a bool
    const u32 args[6]{AddrOf(L_ENT),c.ignoreArg,AddrOf(L_BOOLS),AddrOf(L_BOOLS)+1,AddrOf(L_BOOLS)+2,AddrOf(L_BOOLS)+3};
    Patches patches;
    env.stubs.Install(patches,true);
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        for(auto& sl:slots)if(sl.now)std::memcpy(sl.now,sl.ref,sl.arena);
        nev=0;
        sides[s]=Execute(*env.routePS,0x5E3C30,env.asi,candidate,[&](uintptr_t entry){return Gate(entry,args,6,c.eax,thisPtr,c.edx,c.stack,0);});
        json obs;
        obs["ret"]=nullptr;
        json evj=json::array();
        for(unsigned i=0;i<nev;++i){json e=json::array();for(u32 w:evs[i].w)e.push_back(w);evj.push_back(e);}
        obs["events"]=evj;
        json changes=json::array();Diff(changes,0,slots[L_PED]);Diff(changes,1,slots[L_ENT]);
        obs["changes"]=changes;
        obs["bools"]={slots[L_BOOLS].now[kBoolsOff],slots[L_BOOLS].now[kBoolsOff+1],slots[L_BOOLS].now[kBoolsOff+2],slots[L_BOOLS].now[kBoolsOff+3]};
        bool others=true;for(u32 l:{u32(L_OBJINFO),u32(L_MATPED),u32(L_MATOTHER),u32(L_INTEL)})others&=SameArena(slots[l]);
        // la arena de bools: solo pueden cambiar los cuatro bytes de la ventana (a 1) y nada de las guardas
        const auto& bs=slots[L_BOOLS];bool guards=std::memcmp(bs.now,bs.ref,kBoolsOff)==0&&std::memcmp(bs.now+kBoolsOff+4,bs.ref+kBoolsOff+4,bs.arena-kBoolsOff-4)==0;
        obs["others_unchanged"]=others&&guards;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        gSaved.RestoreAll();
        RestoreFloatingState();
        out["globals_restored"]=gSaved.Intact();   // las entradas vigiladas se comprueban fuera del bucle, con los parches de los falsos ya retirados
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    gS=nullptr;
    patches.Reset();   // los falsos parchean entradas vigiladas (p. ej. 0x601180): comprobarlas sólo con ellos retirados
    {const bool w=env.WatchesIntact();for(auto& r:rows)r["globals_restored"]=r["globals_restored"]==true&&w;}
    Finish(t,"CPed::SpecialEntityPreCollisionStuff",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ CPed::ProcessControl (0x5E8CD0)
inline void CasePC(Env& env,Totals& t,unsigned n) {
    const PCase c=MakeP(n);
    gP=&c;gS=nullptr;gRet=Rand(n,59);nev=0;
    InitSlots();
    ApplyPokes(slots[L_PED],c.ped,Rand(n,50));ApplyPokes(slots[L_PDATA],c.pdata,Rand(n,51));ApplyPokes(slots[L_PDATA2],c.pdata2,Rand(n,52));ApplyPokes(slots[L_PPED],c.pped,Rand(n,53));
    ApplyPokes(slots[L_ENT],c.ent,Rand(n,54));ApplyPokes(slots[L_VEH],c.veh,Rand(n,55));ApplyPokes(slots[L_MATPED],c.matPed,Rand(n,56));ApplyPokes(slots[L_MATOTHER],c.matVeh,Rand(n,57));
    ApplyPokes(slots[L_MATCAM],c.matCam,Rand(n,58));ApplyPokes(slots[L_CLUMP],c.clump,Rand(n,60));
    for(u32 l:{u32(L_MI0),u32(L_MI1),u32(L_OBJINFO),u32(L_FX),u32(L_HIER),u32(L_INTEL),u32(L_WOBJ),u32(L_CM0),u32(L_CM1),u32(L_TEX),u32(L_MATARR)})FillWords(slots[l].ref,slots[l].arena,Rand(n,61+l));
    const auto mi=MiTable(c);
    for(const auto& e:mi){const u32 cm=AddrOf(e.cm);std::memcpy(slots[e.mi].ref+slots[e.mi].off+0x14,&cm,4);}
    // vtable falsa: todas las ranuras a FakeBadVirt salvo SetMoveAnim
    {auto& v=slots[L_VTBL];FillWords(v.ref,v.arena,Rand(n,80));for(u32 i=0;i<26;++i){const u32 a=u32(uintptr_t(&FakeBadVirt));std::memcpy(v.ref+v.off+4*i,&a,4);}
     const u32 a=u32(uintptr_t(&FakeMoveAnim));std::memcpy(v.ref+v.off+4*kSlotMoveAnim,&a,4);}
    json input=json::parse(JsonP(c));
    {json addrs=json::array();for(u32 l=0;l<L_COUNT;++l)addrs.push_back(AddrOf(l));input["addrs"]=addrs;}      // direcciones reales de las arenas (el oraculo informa los punteros resueltos)
    Side sides[2];json rows[2];
    const u32 thisPtr=AddrOf(L_PED);
    Patches patches;
    env.stubs.Install(patches,false);
    for(int s=0;s<2;++s) {
        const bool candidate=s==1;
        PrepareFloatingState();
        for(auto& sl:slots)if(sl.now)std::memcpy(sl.now,sl.ref,sl.arena);
        nev=0;
        // globales
        *reinterpret_cast<u32*>(kGFrame)=c.frame;*reinterpret_cast<u32*>(kGTimestep)=c.timestep;*reinterpret_cast<u32*>(kGArea)=c.area;*reinterpret_cast<u8*>(kGForce)=u8(c.force);
        *reinterpret_cast<u32*>(kGCamMat)=ResolvePtr(c.camMat);std::memcpy(reinterpret_cast<void*>(kGCamPos),c.camPos.data(),12);*reinterpret_cast<u32*>(kGBloodTex)=AddrOf(L_TEX);
        std::vector<std::pair<u32,u32>> savedTable;
        for(const auto& e:mi){u32* at=reinterpret_cast<u32*>(kGModelInfo+4*e.idx);savedTable.emplace_back(e.idx,*at);*at=AddrOf(e.mi);}
        ExeSrand(c.seed);
        sides[s]=Execute(*env.routePC,0x5E8CD0,env.asi,candidate,[&](uintptr_t entry){return Gate(entry,nullptr,0,c.eax,thisPtr,c.edx,c.stack,0);});
        const u32 rngState=*ExeRandState();
        int randCalls=-1;{u32 st=c.seed;for(int k=0;k<=8;++k){if(rngState==st){randCalls=k;break;}st=NextRand(st);}}
        json obs;
        obs["ret"]=nullptr;
        json evj=json::array();
        for(unsigned i=0;i<nev;++i){json e=json::array();for(u32 w:evs[i].w)e.push_back(w);evj.push_back(e);}
        obs["events"]=evj;
        json changes=json::array();Diff(changes,0,slots[L_PED],0x44,0x50);Diff(changes,1,slots[L_PDATA]);
        obs["changes"]=changes;
        obs["speed"]={Bits(slots[L_PED].now+kPedOff+0x44),Bits(slots[L_PED].now+kPedOff+0x48),Bits(slots[L_PED].now+kPedOff+0x4C)};
        obs["rand_calls"]=randCalls;
        bool others=true;for(u32 l:{u32(L_PDATA2),u32(L_PPED),u32(L_ENT),u32(L_VEH),u32(L_MATPED),u32(L_MATOTHER),u32(L_MATCAM),u32(L_MI0),u32(L_MI1),u32(L_FX),u32(L_HIER),u32(L_CLUMP),u32(L_INTEL),u32(L_WOBJ),
                                    u32(L_CM0),u32(L_CM1),u32(L_TEX),u32(L_MATARR),u32(L_VTBL)})others&=SameArena(slots[l]);
        obs["others_unchanged"]=others;
        const bool globalsNow=*reinterpret_cast<u32*>(kGFrame)==c.frame&&*reinterpret_cast<u32*>(kGTimestep)==c.timestep&&*reinterpret_cast<u32*>(kGArea)==c.area&&*reinterpret_cast<u8*>(kGForce)==u8(c.force)
            &&*reinterpret_cast<u32*>(kGCamMat)==ResolvePtr(c.camMat)&&std::memcmp(reinterpret_cast<void*>(kGCamPos),c.camPos.data(),12)==0&&*reinterpret_cast<u32*>(kGBloodTex)==AddrOf(L_TEX);
        obs["globals_unchanged"]=globalsNow;
        for(const auto& e:savedTable)*reinterpret_cast<u32*>(kGModelInfo+4*e.first)=e.second;
        auto fp=NormalizeFp(sides[s].fp);
        json out=CommonRow(sides[s],fp);out["observed"]=obs;
        gSaved.RestoreAll();
        RestoreFloatingState();
        out["globals_restored"]=gSaved.Intact();   // las entradas vigiladas se comprueban fuera del bucle, con los parches de los falsos ya retirados
        out["floating_state_restored"]=FloatingStateRestored();
        rows[s]=out;
    }
    gP=nullptr;
    patches.Reset();   // los falsos parchean entradas vigiladas (p. ej. 0x601180): comprobarlas sólo con ellos retirados
    {const bool w=env.WatchesIntact();for(auto& r:rows)r["globals_restored"]=r["globals_restored"]==true&&w;}
    Finish(t,"CPed::ProcessControl",n,input,rows,sides);
}

// ------------------------------------------------------------------------------------------------ ejecucion del banco
inline void Run(Env& env,bool ready,const json& registered,const json& bodies) {
    ready&=env.routeCS&&env.routePS&&env.routePC;
    for(const HookRoute* r:{env.routeCS.get(),env.routePS.get(),env.routePC.get()})ready&=r&&r->Body()&&ModuleOf(r->Body())==env.asi;
    env.stubs.Resolve();
    json stubbed=json::array();
    for(const Pair* p:env.stubs.All(false)){env.watches.emplace_back(p->exe,16);stubbed.push_back({{"exe",p->exe},{"dll",p->dll}});}
    // todos los colaboradores con cuerpo en la DLL deben haberse localizado; solo los dos de RenderWare (hueso/array de matrices) existen unicamente en el EXE
    bool stubsOk=true;for(const Pair* p:env.stubs.All(false)){const bool exeOnly=p->exe==0x7C51A0||p->exe==0x7C5120;stubsOk&=exeOnly?p->dll==0:(p->dll!=0&&ModuleOf(p->dll)==env.asi);}
    ready&=stubsOk;
    for(uintptr_t at:{uintptr_t(0x5E3E90),uintptr_t(0x5E3C30),uintptr_t(0x5E8CD0),uintptr_t(0x821B1E),uintptr_t(0x821B11),uintptr_t(0x827B3D)})env.watches.emplace_back(at,16);
    json watched=json::array();for(const auto& w:env.watches)watched.push_back({{"address",w.address},{"bytes",HexBytes(reinterpret_cast<void*>(w.address),16)}});
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    gSaved.Save();
    Write({{"event","begin"},{"registered_hook",ready},{"registered",registered},{"functions",3},{"pairs",kCasesCS+kCasesPS+kCasesPC},
        {"cases",{{"CPed::SpecialEntityCalcCollisionSteps",kCasesCS},{"CPed::SpecialEntityPreCollisionStuff",kCasesPS},{"CPed::ProcessControl",kCasesPC}}},
        {"direct_functions",json::array()},{"compare_dead_x87_registers",kCompareDeadX87Registers},{"compare_volatile_registers",kCompareVolatileRegisters},
        {"x87_comparison","fcw+top+ftw+valid_st; ignora FSW banderas/CC, FIP/FDP y registros vacios; MXCSR sin banderas"},{"collaborators_stubbed",true},
        {"collaborators",{{"exe_real",json::array({0x406DA0,0x40FE60,0x40FEC0,0x821B1E})},{"stubbed",stubbed}}},
        {"watched_entries",watched},{"bodies",bodies},{"asi_module",env.asi},{"original_module",env.game},{"initial_fp",HexBytes(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR},
        {"initial_rand_state",gSaved.rngState}});
    Totals totalsCS,totalsPS,totalsPC;
    if(ready) {
        for(unsigned c=0;c<kCasesCS;++c)CaseCS(env,totalsCS,c);
        Write({{"event","function_summary"},{"function","CPed::SpecialEntityCalcCollisionSteps"},{"cases",kCasesCS},{"failures",totalsCS.failures},{"state_restored",env.WatchesIntact()&&gSaved.Intact()}});
        for(unsigned c=0;c<kCasesPS;++c)CasePS(env,totalsPS,c);
        Write({{"event","function_summary"},{"function","CPed::SpecialEntityPreCollisionStuff"},{"cases",kCasesPS},{"failures",totalsPS.failures},{"state_restored",env.WatchesIntact()&&gSaved.Intact()}});
        for(unsigned c=0;c<kCasesPC;++c)CasePC(env,totalsPC,c);
        Write({{"event","function_summary"},{"function","CPed::ProcessControl"},{"cases",kCasesPC},{"failures",totalsPC.failures},{"state_restored",env.WatchesIntact()&&gSaved.Intact()}});
    }
    gSaved.RestoreAll();
    const bool entriesIntact=env.WatchesIntact();
    bool stateRestored=entriesIntact&&gSaved.Intact();
    for(HookRoute* r:{env.routeCS.get(),env.routePS.get(),env.routePC.get()})if(r)stateRestored&=r->Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    const bool passed=totalsCS.passed&&totalsPS.passed&&totalsPC.passed;
    Write({{"event","global_state_final"},{"floating_state",HexBytes(savedFP.data(),108)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored},{"watched_intact",entriesIntact},{"globals_intact",gSaved.Intact()},
        {"rand_state",*ExeRandState()}});
    Write({{"event",ready&&passed&&stateRestored?"complete":"error"},{"pairs",totalsCS.pairs+totalsPS.pairs+totalsPC.pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntact}});
    RestoreFloatingState();savedFPActive=false;
}
}
