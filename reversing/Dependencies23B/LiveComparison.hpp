#pragma once
#include <fstream>
#include "Vehicle.h"
#include "Physical.h"
#include "Ped.h"
#include "PlayerPed.h"
#include "Pad.h"
#include "Timer.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"

// Banco par a par de CVehicle::FlyingControl 0x6D85F0 (hook RH apagado = EXE, encendido = DLL), dentro del juego.
//   __thiscall, 5 argumentos en pila (eFlightModel, leftRightSkid, steeringUpDown, steeringLeftRight, accelerationBreakStatus), ret 0x14, sin valor de retorno.
//   11 rutas x 1536 pares = 16896 pares: ruta 0..9 = flightModel 0..9 (0 dirigible a mano; 1,3,4,5 ala; 2,6,8 rotor; 7 y 9 solo prologo y cola) y ruta 10 = flightModel 40 (>8).
// Colaboradores sustituidos en AMBOS lados por el mismo falso trazado (EntryPatch en la direccion del EXE y en el cuerpo de la DLL, resuelto con Resolve() ANTES de crear parches):
//   CPhysical::ApplyTurnForce 0x542A50, ApplyMoveForce 0x5429F0, GetSpeed 0x542CE0, CVehicle::HeightAboveCeiling 0x6D2600 (conserva EDX), los ocho lectores de CPad
//   (0x53FB80 0x53FBD0 0x53FC10 0x53FC50 0x5403F0 0x540080 0x53FE10 0x53FDD0), CEntity::GetColModel 0x535300, CPed::IsPlayer 0x5DF8F0, CPlayerPed::GetPadFromPlayer 0x609560.
// El CRT (pow/asin/acos/_ftol2) NO se sustituye: la DLL llama al mismo nucleo del EXE (0x829B99 ...); errno se compara via _errno del EXE (0x82B181).
namespace Dependencies23BLive {
using namespace Dependencies23BFix;
inline bool done{};
inline std::ofstream log;
inline void ModuleMarker() {}
inline uintptr_t ModuleOf(uintptr_t a) {MEMORY_BASIC_INFORMATION i{};return VirtualQuery(reinterpret_cast<void*>(a),&i,sizeof(i))==sizeof(i)?uintptr_t(i.AllocationBase):0;}
inline uintptr_t Resolve(uintptr_t a) {for(unsigned i=0;i<16&&*reinterpret_cast<uint8*>(a)==0xE9;++i)a+=5+*reinterpret_cast<int32*>(a+1);return a;}
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> c{};SaveFP(c.data());return c==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}
// Los diez bytes de cada registro x87 VACIO se comparan enteros (FNSAVE completo salvo FIP). No se relaja nunca.
inline constexpr bool kCompareDeadX87Registers=true;

inline std::string Hex(const void* a,size_t n){constexpr char d[]="0123456789abcdef";const auto* b=static_cast<const uint8*>(a);std::string o;o.reserve(n*2);for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];}return o;}
inline uint64 Fnv(const void* a,size_t n){uint64 h=1469598103934665603ull;const auto* b=static_cast<const uint8*>(a);for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;}return h;}
struct Watch {uintptr_t address;std::vector<uint8> saved;Watch(uintptr_t a,size_t n):address(a),saved(ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)){}bool Intact()const{return saved==ReversingProbe::Bytes(reinterpret_cast<void*>(address),saved.size());}};

// ---- FNSAVE: +0 FCW, +4 FSW, +8 FTW, +12 FIP, +16 FCS|FOP, +20 FDP, +24 FDS, +28 ST0..ST7 (10 bytes cada uno) ----
using Fp=std::array<uint8,108>;
inline uint32 W(const Fp& f,size_t at,size_t n=4){uint32 v{};memcpy(&v,f.data()+at,n);return v;}
inline bool Dead(const Fp& f,unsigned st){const uint32 ftw=W(f,8,2),top=(W(f,4,2)>>11)&7;return ((ftw>>(2*((top+st)&7)))&3)==3;}
inline json FpDifference(const Fp& a,const Fp& b){json o=json::array();
 const struct {const char* n;size_t at,sz;} fl[]{{"fcw",0,2},{"fsw",4,2},{"ftw",8,2},{"fcs_fop",16,4},{"fdp",20,4},{"fds",24,4}};
 for(const auto& f:fl)if(memcmp(a.data()+f.at,b.data()+f.at,f.sz))o.push_back(f.n);
 for(unsigned st=0;st<8;++st){const bool dead=Dead(a,st);if((!dead||kCompareDeadX87Registers)&&memcmp(a.data()+28+10*st,b.data()+28+10*st,10))o.push_back("st"+std::to_string(st)+(dead?"(dead)":"(valid)"));}
 return o;}
inline bool DeadEqual(const Fp& a,const Fp& b){for(unsigned st=0;st<8;++st)if(Dead(a,st)&&memcmp(a.data()+28+10*st,b.data()+28+10*st,10))return false;return true;}

// ---- Arena (direcciones reales: los punteros del vehiculo apuntan dentro) y falsos ----
alignas(4096) inline uint8 gArena[kArenaSize]{};
inline uint32 ArenaBase() {return uint32(uintptr_t(gArena));}
inline Cfg G{};
inline std::vector<uint32> trace;
inline void T(std::initializer_list<uint32> v) {trace.insert(trace.end(),v);}
inline uint32 TagOf(const void* p) {const uintptr_t u=reinterpret_cast<uintptr_t>(p);return (u>=uintptr_t(gArena)&&u<uintptr_t(gArena)+kArenaSize)?uint32(u-uintptr_t(gArena)):0xFFFFFFFFu;}
inline void __fastcall FApplyTurn(void* self,void*,CVector force,CVector point) {T({1u,TagOf(self),Bits(force.x),Bits(force.y),Bits(force.z),Bits(point.x),Bits(point.y),Bits(point.z)});}
inline void __fastcall FApplyMove(void* self,void*,CVector force) {T({2u,TagOf(self),Bits(force.x),Bits(force.y),Bits(force.z)});}
inline CVector* __fastcall FGetSpeed(void* self,void*,CVector* out,CVector point) {T({3u,TagOf(self),Bits(point.x),Bits(point.y),Bits(point.z)});std::memcpy(out,G.speed,12);return out;}
inline void __cdecl RecHeight(uint32 self,uint32 z,uint32 model) {T({4u,TagOf(reinterpret_cast<void*>(uintptr_t(self))),z,model});}
inline float gCeiling{};
// 0x6D2600: thiscall, ret 8, float en ST0, no toca EDX en el EXE: el falso conserva ECX y EDX
__declspec(naked) inline void HeightHook() {
    __asm {
        push edx
        push ecx
        push dword ptr [esp + 16]
        push dword ptr [esp + 16]
        push ecx
        call RecHeight
        add esp, 12
        pop ecx
        pop edx
        fld dword ptr [gCeiling]
        ret 8
    }
}
inline int __fastcall FSteerLR(void* self,void*) {T({5u,TagOf(self)});return G.steerLR;}
inline int __fastcall FSteerUD(void* self,void*) {T({6u,TagOf(self)});return G.steerUD;}
inline int __fastcall FGunUD(void* self,void*) {T({7u,TagOf(self)});return G.gunUD;}
inline int __fastcall FGunLR(void* self,void*) {T({8u,TagOf(self)});return G.gunLR;}
inline int __fastcall FAccel(void* self,void*) {T({9u,TagOf(self)});return G.accel;}
inline int __fastcall FBrake(void* self,void*) {T({10u,TagOf(self)});return G.brake;}
inline bool __fastcall FLookL(void* self,void*) {T({11u,TagOf(self)});return G.lookL;}
inline bool __fastcall FLookR(void* self,void*) {T({12u,TagOf(self)});return G.lookR;}
inline void* __fastcall FColModel(void* self,void*) {T({13u,TagOf(self)});return G.colNull?nullptr:static_cast<void*>(gArena+L::col);}
inline bool __fastcall FIsPlayer(void* self,void*) {T({14u,TagOf(self)});return G.isPlayer;}
inline void* __fastcall FPadFromPlayer(void* self,void*) {T({15u,TagOf(self)});return gArena+L::pad;}
inline int* Errno() {return reinterpret_cast<int*(__cdecl*)()>(0x82B181)();}
// recuento de eventos por tipo (longitudes fijas por tipo)
inline json TraceJson(const std::vector<uint32>& t,bool full){
    static const int len[16]{0,8,5,5,4,2,2,2,2,2,2,2,2,2,2,2};json counts=json::object();bool ok=true;size_t i=0;
    while(i<t.size()){const uint32 k=t[i];if(k==0||k>=16||i+len[k]>t.size()){ok=false;break;}counts[std::to_string(k)]=counts.value(std::to_string(k),0)+1;i+=len[k];}
    json o={{"len",t.size()},{"hash",Fnv(t.data(),t.size()*4)},{"counts",counts},{"well_formed",ok}};
    if(full)o["words"]=t;
    return o;}

// Los parches se deshacen en orden INVERSO
struct PatchSet {std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v;void Add(uintptr_t at,void* f){v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f));}~PatchSet(){while(!v.empty())v.pop_back();}};

// ---- Compuerta: estado x87 inicial determinista (FCW 0x7F = 24 bits, FSW del caso, ocho residuos), MXCSR 0x1F80, thiscall con 5 argumentos de pila ----
alignas(16) inline uint8 gPrime[108]{};
inline void MakePrime(uint32 fsw) {
    std::memset(gPrime,0,108);gPrime[0]=0x7F;const uint16 s=uint16(fsw);std::memcpy(gPrime+4,&s,2);
    for(int i=0;i<8;++i){uint8* r=gPrime+28+10*i;for(int k=0;k<8;++k)r[k]=uint8(0x11*(i+1)+k);r[7]|=0x80;r[8]=uint8(i);r[9]=0x40;}
}
struct Gate {bool stack{},nonvolatile{};uint32 mxcsr{};alignas(16) std::array<uint8,108> img{};int err{};};
inline Gate GateThis5(uintptr_t entry,uint32 self,uint32 model,uint32 a2,uint32 a3,uint32 a4,uint32 a5) {
    Gate g{};uint32 sp0{},sp1{},b0{},b1{},s0{},s1{},d0{},d1{},p0{},p1{},mx0=0x1F80,mx1{};uint8* img=g.img.data();const uint8* prime=gPrime;
    *Errno()=0;
    __asm {
        ldmxcsr mx0
        mov eax, prime
        frstor [eax]
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        fstp st(0)
        mov sp0, esp
        mov b0, ebx
        mov s0, esi
        mov d0, edi
        mov p0, ebp
        push a5
        push a4
        push a3
        push a2
        push model
        mov ecx, self
        mov eax, 0A5123456h
        mov edx, 0C0DE5921h
        call entry
        mov sp1, esp
        stmxcsr mx1
        mov b1, ebx
        mov s1, esi
        mov d1, edi
        mov p1, ebp
        mov eax, img
        fnsave [eax]
        frstor [eax]
    }
    g.stack=sp0==sp1;g.nonvolatile=b0==b1&&s0==s1&&d0==d1&&p0==p1;g.mxcsr=mx1;g.err=*Errno();return g;
}
struct Run {Gate gate{};uint32 ip{};bool routeOK{};std::vector<uint32> trace;std::vector<uint8> arena;};
template<class F> Run Execute(OriginalDependencies499::HookRoute& route,bool candidate,uintptr_t entry,F gate){
    Run run{};
    route.Invoke(candidate,[&]{
        run.routeOK=candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(entry);
        run.gate=gate();return 0;});
    run.ip=W(run.gate.img,12);run.trace=trace;run.arena.assign(gArena,gArena+kArenaSize);return run;
}

inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependencies23b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependencies23b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies23b");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
    constexpr uintptr_t kEntry=0x6D85F0;
    OriginalDependencies499::HookRoute route("CVehicle","FlyingControl",kEntry,&CVehicle::FlyingControl);
    bool ready=route.Registered()&&ModuleOf(route.Body())==asi;
    // Cuerpos de los colaboradores de la DLL: se calculan ANTES de crear los parches de entrada
    struct Col {uintptr_t exe,dll;void* fake;};
    std::vector<Col> cols;
    auto Add=[&](uintptr_t exeAt,uintptr_t dllFn,void* fake){cols.push_back({exeAt,Resolve(dllFn),fake});};
    Add(0x542A50,std::bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),reinterpret_cast<void*>(&FApplyTurn));
    Add(0x5429F0,std::bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),reinterpret_cast<void*>(&FApplyMove));
    Add(0x542CE0,std::bit_cast<uintptr_t>(&CPhysical::GetSpeed),reinterpret_cast<void*>(&FGetSpeed));
    Add(0x6D2600,std::bit_cast<uintptr_t>(&CVehicle::HeightAboveCeiling),reinterpret_cast<void*>(&HeightHook));
    Add(0x53FB80,std::bit_cast<uintptr_t>(&CPad::GetSteeringLeftRight),reinterpret_cast<void*>(&FSteerLR));
    Add(0x53FBD0,std::bit_cast<uintptr_t>(&CPad::GetSteeringUpDown),reinterpret_cast<void*>(&FSteerUD));
    Add(0x53FC10,std::bit_cast<uintptr_t>(&CPad::GetCarGunUpDown),reinterpret_cast<void*>(&FGunUD));
    Add(0x53FC50,std::bit_cast<uintptr_t>(&CPad::GetCarGunLeftRight),reinterpret_cast<void*>(&FGunLR));
    Add(0x5403F0,std::bit_cast<uintptr_t>(&CPad::GetAccelerate),reinterpret_cast<void*>(&FAccel));
    Add(0x540080,std::bit_cast<uintptr_t>(&CPad::GetBrake),reinterpret_cast<void*>(&FBrake));
    Add(0x53FE10,std::bit_cast<uintptr_t>(&CPad::GetLookRight),reinterpret_cast<void*>(&FLookR));
    Add(0x53FDD0,std::bit_cast<uintptr_t>(&CPad::GetLookLeft),reinterpret_cast<void*>(&FLookL));
    Add(0x535300,std::bit_cast<uintptr_t>(&CEntity::GetColModel),reinterpret_cast<void*>(&FColModel));
    Add(0x5DF8F0,std::bit_cast<uintptr_t>(&CPed::IsPlayer),reinterpret_cast<void*>(&FIsPlayer));
    Add(0x609560,std::bit_cast<uintptr_t>(&CPlayerPed::GetPadFromPlayer),reinterpret_cast<void*>(&FPadFromPlayer));
    std::vector<Watch> watches;
    for(const auto& c:cols){watches.emplace_back(c.exe,16);if(c.dll!=c.exe)watches.emplace_back(c.dll,16);}
    auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
    auto brokenWatches=[&]{json o=json::array();for(const auto& w:watches)if(!w.Intact())o.push_back({{"address",w.address},{"expected",Hex(w.saved.data(),w.saved.size())}});return o;};
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    // estado global guardado y restaurado: paso de tiempo, viento, byte 0xC1C970 y los dos indicadores del CRT del EXE que usa el nucleo de pow
    struct Saved {uintptr_t a;std::vector<uint8> v;};std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{{0xB7CB5C,4},{0xC813E0,12},{0xC1C970,1},{0xC9C400,8},{0xC9ABFC,4}})saved.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)});
    auto RestoreAll=[&]{for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size());};
    auto GlobalsIntact=[&]{for(auto& s:saved)if(ReversingProbe::Bytes(reinterpret_cast<void*>(s.a),s.v.size())!=s.v)return false;return true;};
    constexpr unsigned total=kRoutes*kCasesPerRoute;
    json bodies=json::array();for(const auto& c:cols)bodies.push_back({c.exe,c.dll});
    Write({{"event","begin"},{"registered_hook",ready},{"functions",kRoutes},{"pairs",total},{"cases_per_route",kCasesPerRoute},{"routes",kRoutes},
        {"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},{"bodies",{{"FlyingControl",route.Body()}}},{"collaborator_bodies",bodies},
        {"crt_c9c400",*reinterpret_cast<uint32*>(0xC9C400)},{"crt_c9c404",*reinterpret_cast<uint32*>(0xC9C404)},{"crt_c9abfc",*reinterpret_cast<uint32*>(0xC9ABFC)},
        {"asi_module",asi},{"original_module",gameExe},{"initial_fp",Hex(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    int pairs{};bool passed=ready;
    static const char* const names[kRoutes]{"CVehicle::FlyingControl[ruta 0]","CVehicle::FlyingControl[ruta 1]","CVehicle::FlyingControl[ruta 2]","CVehicle::FlyingControl[ruta 3]","CVehicle::FlyingControl[ruta 4]",
        "CVehicle::FlyingControl[ruta 5]","CVehicle::FlyingControl[ruta 6]","CVehicle::FlyingControl[ruta 7]","CVehicle::FlyingControl[ruta 8]","CVehicle::FlyingControl[ruta 9]","CVehicle::FlyingControl[ruta 10]"};
    for(int r=0;r<kRoutes&&ready;++r){
        int failures{};
        for(unsigned n=0;n<kCasesPerRoute;++n){
            const Case c=MakeCase(r,uint64(r)*1000003ull+n,uintptr_t(gArena));
            Run runs[2];json rows[2];
            for(int side=0;side<2;++side){
                const bool candidate=side==1;
                RestoreAll();std::memcpy(gArena,c.arena.data(),kArenaSize);G=c.cfg;gCeiling=c.cfg.ceiling;
                *reinterpret_cast<float*>(0xB7CB5C)=c.ts;for(int i=0;i<3;++i)*reinterpret_cast<uint32*>(0xC813E0+4*i)=c.wind[i];
                *reinterpret_cast<uint8*>(0xC1C970)=uint8((c.ts_bits>>3)&1);
                *reinterpret_cast<uint32*>(0xC9C400)=1;*reinterpret_cast<uint32*>(0xC9C404)=1;*reinterpret_cast<uint32*>(0xC9ABFC)=0;
                MakePrime(c.fsw);RestoreFloatingState();trace.clear();
                runs[side]=Execute(route,candidate,kEntry,[&]{PatchSet p;for(const auto& k:cols){p.Add(k.exe,k.fake);if(k.dll!=k.exe)p.Add(k.dll,k.fake);}
                    return GateThis5(kEntry,uint32(uintptr_t(gArena+L::veh)),uint32(c.model),c.args[0],c.args[1],c.args[2],c.args[3]);});
                json out={{"input",{{"route",r},{"case",n},{"model",c.model},{"args",{c.args[0],c.args[1],c.args[2],c.args[3]}},{"fsw",c.fsw},{"ts",Bits(c.ts)},{"wind",{c.wind[0],c.wind[1],c.wind[2]}},{"arena_in",Fnv(c.arena.data(),kArenaSize)}}},
                    {"trace",TraceJson(runs[side].trace,false)},{"observed",{{"arena_hash",Fnv(runs[side].arena.data(),kArenaSize)},{"errno",runs[side].gate.err},{"mxcsr",runs[side].gate.mxcsr&0x3F}}}};
                Fp fp=runs[side].gate.img;memset(fp.data()+12,0,4);   // FIP: cada lado ejecuta su ultima instruccion x87 en otra direccion (se valida el modulo aparte)
                out["post_floating_state"]=Hex(fp.data(),108);
                const uintptr_t where=ModuleOf(runs[side].ip);
                out["fp_ip_route_ok"]=runs[side].ip==0||where==(candidate?asi:gameExe)||where==asi;
                out["stack_restored"]=runs[side].gate.stack;out["nonvolatile_preserved"]=runs[side].gate.nonvolatile;out["route_ok"]=runs[side].routeOK;
                RestoreFloatingState();out["globals_restored"]=watchesIntact();out["floating_state_restored"]=FloatingStateRestored();rows[side]=out;
                if(side==1&&rows[0]!=rows[1]){rows[0]["trace_full"]=TraceJson(runs[0].trace,true);rows[1]["trace_full"]=TraceJson(runs[1].trace,true);
                    size_t i=0;while(i<runs[0].arena.size()&&runs[0].arena[i]==runs[1].arena[i])++i;rows[1]["first_arena_difference"]=i;}
            }
            Fp fa=runs[0].gate.img,fb=runs[1].gate.img;memset(fa.data()+12,0,4);memset(fb.data()+12,0,4);
            bool ok=rows[0]==rows[1]&&runs[0].arena==runs[1].arena&&runs[0].trace==runs[1].trace;
            for(const char* k:{"fp_ip_route_ok","stack_restored","nonvolatile_preserved","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
            ok&=rows[0]["trace"]["well_formed"]==true;
            const json diff=FpDifference(fa,fb);ok&=diff.empty();passed&=ok;failures+=!ok;
            Write({{"event","comparison"},{"function",names[r]},{"case",n},{"pair",pairs++},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",diff},{"dead_registers_equal",DeadEqual(fa,fb)},{"equal",ok},{"accepted",ok}});
        }
        Write({{"event","function_summary"},{"function",names[r]},{"cases",int(kCasesPerRoute)},{"failures",failures},{"state_restored",watchesIntact()},{"broken_watches",brokenWatches()}});
    }
    // Las entradas se vigilan ANTES de Restore(); despues se restauran el hook y los globales
    const bool entriesIntactBefore=watchesIntact();const json brokenBefore=brokenWatches();
    bool stateRestored=entriesIntactBefore;RestoreAll();stateRestored&=GlobalsIntact();stateRestored&=route.Restore();
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore},{"broken_watches_before_restore",brokenBefore}});
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
    RestoreFloatingState();savedFPActive=false;
}
}
