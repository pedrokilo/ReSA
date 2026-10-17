#pragma once
#include <fstream>
#include "World.h"
#include "CutsceneMgr.h"
#include "Sprite.h"
#include "Camera.h"
#include "Weather.h"
#include "Timer.h"
#include "Scene.h"
#include "Clouds.h"
#include "PointLights.h"
#include "app_light.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "Fixtures.hpp"

// Banco par a par de las cuatro funciones de CPointLights (hook RH apagado = EXE, encendido = DLL), dentro del juego:
//   GenerateLightsAffectingObject 0x6FFBB0 (cdecl, 3 args, float en ST0)       2048 pares
//   GetLightMultiplier            0x6FFE70 (cdecl, 1 arg, float en ST0)        1536 pares
//   ProcessVerticalLineUsingCache 0x6FFFF0 (cdecl, CVector por valor + puntero) 2048 pares
//   RenderFogEffect               0x7002D0 (cdecl, sin args)                   2048 pares
// Colaboradores sustituidos en AMBOS lados por el mismo falso trazado (EntryPatch en la direccion del EXE y en el cuerpo de la DLL, resuelto con Resolve()):
//   AddAnExtraDirectionalLight 0x735840, CWorld::ProcessVerticalLine 0x5674E0, CSprite::{InitSpriteBuffer 0x70CFB0, FlushSpriteBuffer 0x70CF20, CalcScreenCoors 0x70CE30,
//   RenderBufferedOneXLUSprite_Rotate_Aspect 0x70E780}, CPointLights::ProcessVerticalLineUsingCache 0x6FFFF0 (solo en los pares de RenderFogEffect) y fpRenderStateSet del motor.
namespace Dependencies22ALive {
using namespace Dependencies22AFix;
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
inline void PrimeX87() {
    static const double stale[8]{1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8};const double* p=stale;
    __asm {mov eax,p} __asm {fld qword ptr [eax]} __asm {fld qword ptr [eax+8]} __asm {fld qword ptr [eax+16]} __asm {fld qword ptr [eax+24]}
    __asm {fld qword ptr [eax+32]} __asm {fld qword ptr [eax+40]} __asm {fld qword ptr [eax+48]} __asm {fld qword ptr [eax+56]}
    __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)} __asm {fstp st(0)}
}
// Los diez bytes de cada registro x87 VACIO (residuo) se comparan enteros (el FNSAVE completo, salvo FIP): los ayudantes de X87Math plantan al final de cada tramo los ultimos valores que
// el EXE deja en los registros por debajo de la cima (PointLightsShadow / PointLightsPlant*, mismo metodo que X87Plant* de CTaskSimpleGangDriveBy y Ftol2RegsCore). No se relaja nunca.
inline constexpr bool kCompareDeadX87Registers=true;

struct Reg {uint32 eax,ecx,edx;bool stack,nonvolatile;};
// cdecl con n argumentos de 32 bits (EAX/ECX/EDX sembrados; se capturan justo tras la llamada)
inline Reg GateCdecl(uintptr_t entry,const uint32* args,int n){uint32 before{},after{},ra{},rc{},rd{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {mov ecx,n} __asm {mov eax,args}
 __asm {pushloop: test ecx,ecx} __asm {jz pushed} __asm {push dword ptr [eax+ecx*4-4]} __asm {dec ecx} __asm {jmp pushloop}
 __asm {pushed: mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,0C0DE5921h} __asm {call entry}
 __asm {mov ra,eax} __asm {mov rc,ecx} __asm {mov rd,edx} __asm {mov eax,n} __asm {lea esp,[esp+eax*4]} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {ra,rc,rd,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}

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

// ---- Trazas de los colaboradores (bits crudos) ----
struct Ev {uint32 kind;uint32 a[12];};
inline std::vector<Ev> trace;
inline void Rec(uint32 kind,std::initializer_list<uint32> v){Ev e{};e.kind=kind;int i=0;for(uint32 x:v)e.a[i++]=x;trace.push_back(e);}
inline uint32 B(float f){return std::bit_cast<uint32>(f);}
inline uint32 Q(float f){return B(f);}
// comportamiento de los falsos del caso actual
inline unsigned fSeed{},fCalc{},fPvl{},fCache{};inline unsigned cCalc{},cPvl{},cCache{};
inline bool pvlFixedOk{};inline uint32 pvlFixedZ{};inline bool useFixedPvl{};   // ProcessVerticalLineUsingCache: el falso de CWorld::ProcessVerticalLine devuelve esto
inline uint32 FakeValue(uint32 h,int range,uint32 per){return Value(h,1,range,per);}
inline void __cdecl FakeAddLight(RpWorld* w,float x,float y,float z,float r,float g,float b){Rec(1,{uint32(uintptr_t(w)),B(x),B(y),B(z),B(r),B(g),B(b)});}
inline bool __cdecl FakePvl(const CVector& o,float d,CColPoint& cp,CEntity*& e,bool b,bool v,bool p,bool ob,bool dm,bool st,CStoredCollPoly* poly){
    Rec(2,{B(o.x),B(o.y),B(o.z),B(d),uint32(b),uint32(v),uint32(p),uint32(ob),uint32(dm),uint32(st),uint32(uintptr_t(poly))});
    bool ok;uint32 z;
    if(useFixedPvl){ok=pvlFixedOk;z=pvlFixedZ;}else{const uint32 h=Rand(fSeed,200+cPvl++);ok=(h%10)<fPvl;z=FakeValue(h,480,60);}
    if(!ok)return false;
    std::memset(&cp,0,sizeof(cp));std::memcpy(reinterpret_cast<uint8*>(&cp)+8,&z,4);e=nullptr;return true;}
inline void __cdecl FakeInit(){Rec(11,{});}
inline void __cdecl FakeFlush(){Rec(12,{});}
inline bool __cdecl FakeCalc(const RwV3d& pos,RwV3d* out,float* w,float* h,bool mx,bool mn){
    const uint32 k=Rand(fSeed,100+cCalc++);Rec(13,{B(pos.x),B(pos.y),B(pos.z),uint32(mx),uint32(mn)});if((k%10)>=fCalc)return false;
    const uint32 v[5]{FakeValue(k*7+1,10000,25),FakeValue(k*7+2,7500,25),FakeValue(k*7+3,1440,40),FakeValue(k*7+4,640,40),FakeValue(k*7+5,640,40)};
    std::memcpy(&out->x,&v[0],4);std::memcpy(&out->y,&v[1],4);std::memcpy(&out->z,&v[2],4);std::memcpy(w,&v[3],4);std::memcpy(h,&v[4],4);return true;}
inline void __cdecl FakeSprite(float x,float y,float z,float w,float h,uint8 r,uint8 g,uint8 b,int16 i,float rcp,float ang,uint8 a){Rec(16,{B(x),B(y),B(z),B(w),B(h),r,g,b,uint16(i),B(rcp),B(ang),a});}
inline bool __cdecl FakeCache(CVector p,float* outZ){const uint32 h=Rand(fSeed,300+cCache++);Rec(15,{B(p.x),B(p.y),B(p.z)});const bool ok=(h%10)<fCache;if(ok){const uint32 z=FakeValue(h,480,60);std::memcpy(outZ,&z,4);}return ok;}
inline RwBool __cdecl FakeState(RwRenderState s,void* v){Rec(10,{uint32(s),uint32(uintptr_t(v))});return 1;}

// Los parches se deshacen en orden INVERSO: dos entradas de DLL contiguas (cuerpos de menos de 5 bytes) se solapan y restaurar en orden directo dejaba un salto escrito en el codigo.
struct PatchSet {std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v;void Add(uintptr_t at,void* f){v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f));}~PatchSet(){while(!v.empty())v.pop_back();}};
struct Run {Reg reg{};Fp fp{};uint32 ip{};bool routeOK{};std::vector<Ev> trace;};
template<class G> Run Execute(OriginalDependencies499::HookRoute& route,bool candidate,uintptr_t entry,G gate){
    Run run{};
    route.Invoke(candidate,[&]{
        run.routeOK=candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(entry);
        run.reg=gate();SaveFP(run.fp.data());return 0;});
    run.ip=W(run.fp,12);run.trace=trace;return run;
}
inline json TraceJson(const std::vector<Ev>& t,bool full){
    json counts=json::object();uint64 h=1469598103934665603ull;
    for(const auto& e:t){counts[std::to_string(e.kind)]=counts.value(std::to_string(e.kind),0)+1;h=(h^Fnv(&e,sizeof(e)))*1099511628211ull;}
    json o={{"len",t.size()},{"hash",h},{"counts",counts}};
    if(full){json a=json::array();for(const auto& e:t){json r=json::array({e.kind});for(int i=0;i<12;++i)r.push_back(e.a[i]);a.push_back(r);}o["events"]=a;}
    return o;}

// Region de CPointLights (cache de resultados 0xC3F050, NumLights 0xC3F0D0, indice 0xC3F0D4, luces 0xC3F0E0 y cache de lecturas 0xC3F6E0..0xC3F868)
constexpr uintptr_t kRegion=0xC3F050;constexpr size_t kRegionSize=0xC3F868-0xC3F050;
inline void SetLight(int i,const LightFx& l){auto& L=CPointLights::aLights[i];uint32 w[12]{l.pos[0],l.pos[1],l.pos[2],l.dir[0],l.dir[1],l.dir[2],l.radius,l.color[0],l.color[1],l.color[2],l.entity,0};
    std::memcpy(&L,w,0x2C);reinterpret_cast<uint8*>(&L)[0x2C]=uint8(l.type);reinterpret_cast<uint8*>(&L)[0x2D]=uint8(l.fog);reinterpret_cast<uint8*>(&L)[0x2E]=uint8(l.shadows);}
inline json LightJson(const LightFx& l){return {{"pos",{l.pos[0],l.pos[1],l.pos[2]}},{"dir",{l.dir[0],l.dir[1],l.dir[2]}},{"radius",l.radius},{"color",{l.color[0],l.color[1],l.color[2]}},{"entity",l.entity},{"type",l.type},{"fog",l.fog}};}

inline void Update(){
    if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"dependencies22a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"dependencies22a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies22a");}
    if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
    const uintptr_t asi=ModuleOf(uintptr_t(&ModuleMarker)),gameExe=uintptr_t(GetModuleHandleA(nullptr));
    OriginalDependencies499::HookRoute gen("CPointLights","GenerateLightsAffectingObject",0x6FFBB0,&CPointLights::GenerateLightsAffectingObject);
    OriginalDependencies499::HookRoute mult("CPointLights","GetLightMultiplier",0x6FFE70,&CPointLights::GetLightMultiplier);
    OriginalDependencies499::HookRoute cache("CPointLights","ProcessVerticalLineUsingCache",0x6FFFF0,&CPointLights::ProcessVerticalLineUsingCache);
    OriginalDependencies499::HookRoute fogRoute("CPointLights","RenderFogEffect",0x7002D0,&CPointLights::RenderFogEffect);
    bool ready=gen.Registered()&&mult.Registered()&&cache.Registered()&&fogRoute.Registered();
    ready=ready&&ModuleOf(gen.Body())==asi&&ModuleOf(mult.Body())==asi&&ModuleOf(cache.Body())==asi&&ModuleOf(fogRoute.Body())==asi;
    // Cuerpos de los colaboradores de la DLL (se calculan antes de crear los parches de entrada)
    const uintptr_t dAdd=Resolve(uintptr_t(&AddAnExtraDirectionalLight)),dPvl=Resolve(uintptr_t(&CWorld::ProcessVerticalLine)),dInit=Resolve(uintptr_t(&CSprite::InitSpriteBuffer)),
        dFlush=Resolve(uintptr_t(&CSprite::FlushSpriteBuffer)),dCalc=Resolve(uintptr_t(&CSprite::CalcScreenCoors)),dSprite=Resolve(uintptr_t(&CSprite::RenderBufferedOneXLUSprite_Rotate_Aspect)),
        dCache=cache.Body();   // NO Resolve(): con el hook de la ruta apagado el cuerpo de la DLL empieza por un JMP al EXE (RH::Simple::Switch) y Resolve() devolveria 0x6FFFF0
    for(uintptr_t a:{dAdd,dPvl,dInit,dFlush,dCalc,dSprite})ready=ready&&ModuleOf(a)==asi;
    std::vector<Watch> watches;
    for(uintptr_t at:{uintptr_t(0x6FFBB0),uintptr_t(0x6FFE70),uintptr_t(0x6FFFF0),uintptr_t(0x7002D0),uintptr_t(0x735840),uintptr_t(0x5674E0),uintptr_t(0x70CFB0),uintptr_t(0x70CF20),uintptr_t(0x70CE30),uintptr_t(0x70E780),dAdd,dPvl,dInit,dFlush,dCalc,dSprite})watches.emplace_back(at,16);   // el cuerpo de la DLL de la ruta (dCache) cambia con el estado del hook: lo comprueba HookRoute::Restore
    auto watchesIntact=[&]{return std::ranges::all_of(watches,[](const auto& w){return w.Intact();});};
    auto brokenWatches=[&]{json o=json::array();for(const auto& w:watches)if(!w.Intact())o.push_back({{"address",w.address},{"expected",Hex(w.saved.data(),w.saved.size())},{"actual",Hex(ReversingProbe::Bytes(reinterpret_cast<void*>(w.address),w.saved.size()).data(),w.saved.size())}});return o;};
    SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
    // estado global guardado y restaurado
    struct Saved {uintptr_t a;std::vector<uint8> v;};std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{{kRegion,kRegionSize},{0xC81300,4},{0xB5F851,1},{0xB7CB84,4},{0xB6F02C,0x14},{0xC6AA78,4},{0xC17038,4}})saved.push_back({a,ReversingProbe::Bytes(reinterpret_cast<void*>(a),n)});
    auto RestoreAll=[&]{for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size());};
    auto GlobalsIntact=[&]{for(auto& s:saved)if(ReversingProbe::Bytes(reinterpret_cast<void*>(s.a),s.v.size())!=s.v)return false;return true;};
    constexpr unsigned total=kGenerateCases+kMultiplierCases+kCacheCases+kFogCases;
    Write({{"event","begin"},{"registered_hook",ready},{"functions",4},{"pairs",total},{"cases",{{"Generate",kGenerateCases},{"Multiplier",kMultiplierCases},{"Cache",kCacheCases},{"Fog",kFogCases}}},
        {"compare_dead_x87_registers",kCompareDeadX87Registers},{"collaborators_stubbed",true},{"bodies",{{"Generate",gen.Body()},{"Multiplier",mult.Body()},{"Cache",cache.Body()},{"Fog",fogRoute.Body()}}},
        {"asi_module",asi},{"original_module",gameExe},{"initial_fp",Hex(savedFP.data(),108)},{"initial_mxcsr",savedMXCSR}});
    int pairs{};bool passed=ready;
    auto prepare=[&]{RestoreFloatingState();PrimeX87();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);trace.clear();cCalc=cPvl=cCache=0;};
    // un par: `setup` deja el caso listo (se llama antes de cada lado), `call` ejecuta la funcion, `observe` serializa el resultado propio de la funcion.
    auto Pair=[&](OriginalDependencies499::HookRoute& route,const char* name,unsigned scenario,uintptr_t entry,const json& input,auto setup,auto gateCall,auto observe,auto patches,int& failures){
        Run runs[2];json rows[2];
        for(int side=0;side<2;++side){
            const bool candidate=side==1;setup();prepare();
            runs[side]=Execute(route,candidate,entry,[&]{auto p=patches();return gateCall();});
            json out={{"input",input},{"trace",TraceJson(runs[side].trace,false)},{"observed",observe()}};
            Fp fp=runs[side].fp;memset(fp.data()+12,0,4);   // FIP: cada lado ejecuta su ultima instruccion x87 en otra direccion (se valida el modulo aparte)
            out["state_hash"]=Fnv(reinterpret_cast<void*>(kRegion),kRegionSize);
            out["post_floating_state"]=Hex(fp.data(),108);
            const uintptr_t where=ModuleOf(runs[side].ip);
            out["fp_ip_route_ok"]=runs[side].ip==0||where==(candidate?asi:gameExe)||where==asi;
            out["stack_restored"]=runs[side].reg.stack;out["nonvolatile_preserved"]=runs[side].reg.nonvolatile;out["route_ok"]=runs[side].routeOK;
            RestoreFloatingState();out["globals_restored"]=watchesIntact();out["floating_state_restored"]=FloatingStateRestored();rows[side]=out;
            if(side==1&&rows[0]!=rows[1]){rows[0]["trace_full"]=TraceJson(runs[0].trace,true);rows[1]["trace_full"]=TraceJson(runs[1].trace,true);}
        }
        Fp fa=runs[0].fp,fb=runs[1].fp;memset(fa.data()+12,0,4);memset(fb.data()+12,0,4);
        bool ok=rows[0]==rows[1];for(const char* k:{"fp_ip_route_ok","stack_restored","nonvolatile_preserved","route_ok","globals_restored","floating_state_restored"})ok&=rows[0][k]==true&&rows[1][k]==true;
        const json diff=FpDifference(fa,fb);ok&=diff.empty();passed&=ok;failures+=!ok;
        Write({{"event","comparison"},{"function",name},{"case",scenario},{"pair",pairs++},{"original",rows[0]},{"candidate",rows[1]},{"fp_difference",diff},{"dead_registers_equal",DeadEqual(fa,fb)},
            {"original_residue",{{"eax",runs[0].reg.eax},{"ecx",runs[0].reg.ecx},{"edx",runs[0].reg.edx}}},{"candidate_residue",{{"eax",runs[1].reg.eax},{"ecx",runs[1].reg.ecx},{"edx",runs[1].reg.edx}}},{"equal",ok},{"accepted",ok}});
    };
    CVector pointArena[2]{};alignas(16) static std::array<uint8,48> bufs{};float* totalArena=reinterpret_cast<float*>(bufs.data()+16);float* outZArena=reinterpret_cast<float*>(bufs.data()+32);
    auto patchAdd=[&]{auto p=std::make_unique<PatchSet>();p->Add(0x735840,reinterpret_cast<void*>(&FakeAddLight));p->Add(dAdd,reinterpret_cast<void*>(&FakeAddLight));return p;};
    auto patchPvl=[&]{auto p=std::make_unique<PatchSet>();p->Add(0x5674E0,reinterpret_cast<void*>(&FakePvl));p->Add(dPvl,reinterpret_cast<void*>(&FakePvl));return p;};

    // ============================================================ GenerateLightsAffectingObject
    int failuresGen{};
    for(unsigned c=0;c<kGenerateCases&&ready;++c){
        const unsigned n=LightCount(c);const u32 pt[3]{PointCoord(c,0),PointCoord(c,1),PointCoord(c,2)};const bool useTotal=UseTotal(c);const u32 totalInit=TotalInit(c),entity=EntityArg(c);
        json input={{"case",c},{"count",n},{"point",{pt[0],pt[1],pt[2]}},{"use_total",useTotal},{"total_init",totalInit},{"entity",entity},{"lights",json::array()}};
        for(unsigned i=0;i<(n>32?0:n);++i)input["lights"].push_back(LightJson(Light(c,i)));
        auto setup=[&]{RestoreAll();CPointLights::NumLights=n;for(int i=0;i<32;++i)SetLight(i,Light(c,i));std::memcpy(&pointArena[0],pt,12);std::memcpy(totalArena,&totalInit,4);trace.clear();};
        auto call=[&]{const uint32 a[3]{uint32(uintptr_t(&pointArena[0])),uint32(useTotal?uintptr_t(totalArena):0),entity};return GateCdecl(0x6FFBB0,a,3);};
        Pair(gen,"CPointLights::GenerateLightsAffectingObject",c,0x6FFBB0,input,setup,call,[&]{return json{{"total",*reinterpret_cast<uint32*>(totalArena)}};},patchAdd,failuresGen);
    }
    Write({{"event","function_summary"},{"function","CPointLights::GenerateLightsAffectingObject"},{"cases",int(kGenerateCases)},{"failures",failuresGen},{"state_restored",watchesIntact()},{"broken_watches",brokenWatches()}});
    // ============================================================ GetLightMultiplier
    int failuresMult{};
    for(unsigned c=0;c<kMultiplierCases&&ready;++c){
        const unsigned n=LightCount(c+100000);const u32 pt[3]{PointCoord(c,0),PointCoord(c,1),PointCoord(c,2)};
        json input={{"case",c},{"count",n},{"point",{pt[0],pt[1],pt[2]}},{"lights",json::array()}};
        for(unsigned i=0;i<(n>32?0:n);++i)input["lights"].push_back(LightJson(Light(c,i)));
        auto setup=[&]{RestoreAll();CPointLights::NumLights=n;for(int i=0;i<32;++i)SetLight(i,Light(c,i));std::memcpy(&pointArena[0],pt,12);trace.clear();};
        auto call=[&]{const uint32 a[1]{uint32(uintptr_t(&pointArena[0]))};return GateCdecl(0x6FFE70,a,1);};
        Pair(mult,"CPointLights::GetLightMultiplier",c,0x6FFE70,input,setup,call,[&]{return json{};},[&]{return std::make_unique<PatchSet>();},failuresMult);
    }
    Write({{"event","function_summary"},{"function","CPointLights::GetLightMultiplier"},{"cases",int(kMultiplierCases)},{"failures",failuresMult},{"state_restored",watchesIntact()},{"broken_watches",brokenWatches()}});
    // ============================================================ ProcessVerticalLineUsingCache
    int failuresCache{};
    for(unsigned c=0;c<kCacheCases&&ready;++c){
        const CacheFx f=Cache(c);
        json input={{"case",c},{"point",{f.point[0],f.point[1],f.point[2]}},{"next",f.next},{"pvl_ok",f.pvlOk},{"pvl_z",f.pvlZ},{"hit",f.hit}};
        auto setup=[&]{RestoreAll();std::memcpy(CPointLights::aCachedMapReads,f.entry,sizeof(f.entry));std::memcpy(CPointLights::aCachedMapReadResults,f.result,sizeof(f.result));CPointLights::NextCachedValue=f.next;
            useFixedPvl=true;pvlFixedOk=f.pvlOk;pvlFixedZ=f.pvlZ;*outZArena=7.0f;trace.clear();};
        auto call=[&]{const uint32 a[4]{f.point[0],f.point[1],f.point[2],uint32(uintptr_t(outZArena))};return GateCdecl(0x6FFFF0,a,4);};
        Pair(cache,"CPointLights::ProcessVerticalLineUsingCache",c,0x6FFFF0,input,setup,call,[&]{return json{{"out_z",*reinterpret_cast<uint32*>(outZArena)}};},patchPvl,failuresCache);
        useFixedPvl=false;
    }
    Write({{"event","function_summary"},{"function","CPointLights::ProcessVerticalLineUsingCache"},{"cases",int(kCacheCases)},{"failures",failuresCache},{"state_restored",watchesIntact()},{"broken_watches",brokenWatches()}});
    // ============================================================ RenderFogEffect
    int failuresFog{};
    {
        auto& slot=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet;const auto savedSlot=slot;static RwTexture fakeTexture{};static uint8 fakeCamera[0x100]{};
        for(unsigned c=0;c<kFogCases&&ready;++c){
            const FogFx f=Fog(c);const unsigned n=f.count;
            json input={{"case",c},{"count",n},{"foggy",f.foggy},{"timer",f.timer},{"cutscene",f.cutscene},{"camera_matrix",f.cameraMatrix},{"camera",{f.camera[0],f.camera[1],f.camera[2]}},
                {"p_calc",f.pCalc},{"p_pvl",f.pPvl},{"p_cache",f.pCache},{"lights",json::array()}};
            for(unsigned i=0;i<n;++i)input["lights"].push_back(LightJson(FogLight(c,i)));
            auto setup=[&]{RestoreAll();CPointLights::NumLights=n;for(int i=0;i<32;++i)SetLight(i,FogLight(c,i));
                *reinterpret_cast<uint32*>(0xC81300)=f.foggy;*reinterpret_cast<uint8*>(0xB5F851)=uint8(f.cutscene);*reinterpret_cast<uint32*>(0xB7CB84)=f.timer;gpCloudTex=&fakeTexture;
                float* cp;if(f.cameraMatrix){*reinterpret_cast<void**>(0xB6F03C)=fakeCamera;cp=reinterpret_cast<float*>(fakeCamera+0x30);}else{*reinterpret_cast<void**>(0xB6F03C)=nullptr;cp=reinterpret_cast<float*>(0xB6F02C);}
                std::memcpy(cp,f.camera,12);fSeed=f.seed;fCalc=f.pCalc;fPvl=f.pPvl;fCache=f.pCache;useFixedPvl=false;slot=&FakeState;trace.clear();};
            auto call=[&]{return GateCdecl(0x7002D0,nullptr,0);};
            auto patches=[&]{auto p=std::make_unique<PatchSet>();
                p->Add(0x5674E0,reinterpret_cast<void*>(&FakePvl));p->Add(dPvl,reinterpret_cast<void*>(&FakePvl));p->Add(0x70CFB0,reinterpret_cast<void*>(&FakeInit));p->Add(dInit,reinterpret_cast<void*>(&FakeInit));
                p->Add(0x70CF20,reinterpret_cast<void*>(&FakeFlush));p->Add(dFlush,reinterpret_cast<void*>(&FakeFlush));p->Add(0x70CE30,reinterpret_cast<void*>(&FakeCalc));p->Add(dCalc,reinterpret_cast<void*>(&FakeCalc));
                p->Add(0x70E780,reinterpret_cast<void*>(&FakeSprite));p->Add(dSprite,reinterpret_cast<void*>(&FakeSprite));p->Add(0x6FFFF0,reinterpret_cast<void*>(&FakeCache));p->Add(dCache,reinterpret_cast<void*>(&FakeCache));return p;};
            Pair(fogRoute,"CPointLights::RenderFogEffect",c,0x7002D0,input,setup,call,[&]{return json{};},patches,failuresFog);
        }
        slot=savedSlot;
    }
    Write({{"event","function_summary"},{"function","CPointLights::RenderFogEffect"},{"cases",int(kFogCases)},{"failures",failuresFog},{"state_restored",watchesIntact()},{"broken_watches",brokenWatches()}});

    // Las entradas se vigilan ANTES de Restore(): Restore() vuelve a encender los hooks (las cuatro primeras vigilancias pasan a ser JMP a la DLL, su estado inicial correcto, que cada
    // HookRoute comprueba por su cuenta). Las demas vigilancias (colaboradores del EXE y cuerpos de la DLL) deben seguir intactas tambien despues.
    const bool entriesIntactBefore=watchesIntact();const json brokenBefore=brokenWatches();
    bool stateRestored=entriesIntactBefore;RestoreAll();stateRestored&=GlobalsIntact();
    stateRestored&=gen.Restore();stateRestored&=mult.Restore();stateRestored&=cache.Restore();stateRestored&=fogRoute.Restore();
    bool othersIntact=true;json brokenAfter=json::array();for(size_t i=4;i<watches.size();++i){if(!watches[i].Intact()){othersIntact=false;brokenAfter.push_back({{"index",i},{"address",watches[i].address},{"expected",Hex(watches[i].saved.data(),watches[i].saved.size())},{"actual",Hex(ReversingProbe::Bytes(reinterpret_cast<void*>(watches[i].address),watches[i].saved.size()).data(),watches[i].saved.size())}});}}stateRestored&=othersIntact;
    RestoreFloatingState();stateRestored&=FloatingStateRestored();
    Write({{"event","global_state_final"},{"state_restored",stateRestored},{"watched_intact",entriesIntactBefore},{"broken_watches_before_restore",brokenBefore},{"other_watches_intact_after_restore",othersIntact},{"broken_watches_after_restore",brokenAfter}});
    Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"entries_intact",entriesIntactBefore}});
    RestoreFloatingState();savedFPActive=false;
}
}
