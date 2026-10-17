#pragma once
#include <fstream>
#include "World.h"
#include "CutsceneMgr.h"
#include "RenderWare/D3DResourceSystem.h"
#include "../ProbeRuntime.hpp"
#include "../OriginalDependencies499/HookRoute.hpp"
#include "HelperRoutes.hpp"
// Banco par a par de D3DResourceSystem::TidyUpD3DTextures (0x7305E0) y TidyUpD3DIndexBuffers (0x730740): EXE con el hook apagado frente a la DLL con el hook activo.
// Ningun recurso D3D real se libera: las cabeceras de las caches (0xC87C60..0xC87FE8) apuntan a listas de punteros del banco cuyos objetos son COM falsos
// (vtable de tres ranuras; solo Release, la ranura 2, es legitima). Se compara en bits el estado resultante (region de globales, listas y contadores de los objetos),
// la traza de Release (objeto y hash de la region en el momento de la llamada, en orden), ESP, registros no volatiles, x87 (FNSAVE completo) y MXCSR.
namespace Dependencies21FLive {
inline bool done{};
inline std::ofstream log;
inline void SaveFP(void* p) {__asm {mov eax,p} __asm {fnsave [eax]} __asm {frstor [eax]}}
inline void RestoreFP(void* p) {__asm {mov eax,p} __asm {frstor [eax]}}
inline uint32 ReadMXCSR() {uint32 v{};__asm {stmxcsr v}return v;}
inline void SetMXCSR(uint32 v) {__asm {ldmxcsr v}}
alignas(16) inline std::array<uint8,108> savedFP{};
inline uint32 savedMXCSR{};inline bool savedFPActive{};
inline void RestoreFloatingState() {if(savedFPActive) {RestoreFP(savedFP.data());SetMXCSR(savedMXCSR);}}
inline bool FloatingStateRestored() {alignas(16)std::array<uint8,108> current{};SaveFP(current.data());return current==savedFP&&ReadMXCSR()==savedMXCSR;}
inline void Write(const json& row) {RestoreFloatingState();ReversingProbe::PumpMessages();log<<row.dump()<<'\n';log.flush();RestoreFloatingState();}

inline constexpr unsigned kCasesPerFunction=2048;   // 512 combinaciones de (cuenta, llenado, mutacion) x 4 variantes aleatorias
inline constexpr unsigned kTexSlots=17,kIbSlots=16,kArray=64,kObjects=2048,kTrace=4096;
inline constexpr uintptr_t kRegion=0xC87C60,kRegionEnd=0xC87FE8;     // NumTextureBuffers .. FreeIndexBufferIndex (0x388 bytes)
inline constexpr uintptr_t kTexBase=0xC87C68,kIbBase=0xC87E48,kGenBase=0xC87FC8,kNumTex=0xC87C60,kFreeTex=0xC87FE0,kFreeIb=0xC87FE4;
inline constexpr size_t kRegionSize=kRegionEnd-kRegion;
struct Fn {const char* cls;const char* name;uintptr_t address;const char* full;};
inline constexpr Fn fns[]{
    {"D3DResourceSystem","TidyUpD3DTextures",0x7305E0,"D3DResourceSystem::TidyUpD3DTextures"},
    {"D3DResourceSystem","TidyUpD3DIndexBuffers",0x730740,"D3DResourceSystem::TidyUpD3DIndexBuffers"}
};

inline uint32 ReadInt(uintptr_t a){uint32 v{};memcpy(&v,reinterpret_cast<void*>(a),4);return v;}
inline void PutInt(uintptr_t a,uint32 v){memcpy(reinterpret_cast<void*>(a),&v,4);}
inline std::string Hex(uintptr_t a,size_t n){constexpr char digits[]="0123456789abcdef";const auto* b=reinterpret_cast<const uint8*>(a);std::string out;out.reserve(n*2);for(size_t i=0;i<n;++i){out+=digits[b[i]>>4];out+=digits[b[i]&15];}return out;}
inline uint32 Fnv32(const void* p,size_t n){uint32 h=2166136261u;const auto* b=static_cast<const uint8*>(p);for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;}return h;}
inline uint64_t Fnv64(const std::vector<uint8>& v){uint64_t h=1469598103934665603ull;for(auto c:v){h^=c;h*=1099511628211ull;}return h;}
struct Block {uintptr_t address;std::vector<uint8> saved;Block(uintptr_t a,size_t n):address(a),saved(n){memcpy(saved.data(),reinterpret_cast<void*>(a),n);}void Restore()const{memcpy(reinterpret_cast<void*>(address),saved.data(),saved.size());}bool Restored()const{return !memcmp(reinterpret_cast<void*>(address),saved.data(),saved.size());}};

// --- Objetos COM falsos ---------------------------------------------------------------------------------------------------------------------------------
struct FakeCom {const void* vtbl;uint32 id;uint32 releases;uint32 reserved;};
inline std::array<FakeCom,kObjects> pool{};
inline const void* fakeVtable[3]{};
struct Mutation {unsigned at{};uint32 n{};int32 texIndex{-1};int32 ibIndex{-1};};    // se aplica dentro del Release numero `at` (1 = el primero)
inline Mutation mutation;
inline unsigned releaseNumber{},traceLength{},unexpectedCalls{};
inline bool traceOverflow{};
inline std::array<uint32,2*kTrace> trace{};
inline unsigned long __stdcall FakeQueryInterface(FakeCom*,const void*,void**) {++unexpectedCalls;return 0x80004002;}
inline unsigned long __stdcall FakeAddRef(FakeCom*) {++unexpectedCalls;return 2;}
inline unsigned long __stdcall FakeRelease(FakeCom* self) {
    if(self<pool.data()||self>=pool.data()+kObjects) {++unexpectedCalls;return 0;}
    ++self->releases;++releaseNumber;
    if(traceLength<kTrace) {trace[2*traceLength]=self->id;trace[2*traceLength+1]=Fnv32(reinterpret_cast<void*>(kRegion),kRegionSize);++traceLength;} else traceOverflow=true;
    if(mutation.at&&releaseNumber==mutation.at) {     // la llamada cambia los globales que el EXE vuelve a leer tras Release
        if(mutation.n)PutInt(kNumTex,mutation.n);
        if(mutation.texIndex>=0)PutInt(kFreeTex,uint32(mutation.texIndex));
        if(mutation.ibIndex>=0)PutInt(kFreeIb,uint32(mutation.ibIndex));
    }
    return 0x1000+self->id;
}

// --- Fixtures ---------------------------------------------------------------------------------------------------------------------------------------
struct Slot {uint32 num{},size{};std::array<int32,kArray> ids{};};            // ids: indice del objeto, -1 = puntero nulo, -2 = relleno de veneno (0xDEAD0000)
struct Fixture {uint32 count{},n{},freeTex{},freeIb{};std::array<Slot,kTexSlots> tex{};std::array<Slot,kIbSlots> ib{};Slot gen{};Mutation mut{};};
struct Rng {uint32 s;uint32 Next(){s^=s<<13;s^=s>>17;s^=s<<5;return s;}uint32 Below(uint32 n){return Next()%n;}};
inline Fixture MakeFixture(unsigned function,unsigned scenario) {
    Fixture f;Rng r{0x9E3779B9u*(scenario+1)^0x85EBCA6Bu*(function+1)};for(int i=0;i<4;++i)r.Next();
    constexpr uint32 counts[16]{0,1,2,3,4,7,100,200,0x7FFFFFFF,0x80000000,0xFFFFFFFF,0xFFFFFFFE,5,17,33,64};
    const unsigned fill=scenario/16%4,mutationMode=scenario/64%8,variant=scenario/512;
    f.count=counts[scenario%16];
    constexpr uint32 sizes[8]{1,2,3,5,9,16,17,0};
    f.n=sizes[scenario/8%8];if(!f.n)f.n=1+r.Below(17);
    switch(scenario/3%5) {case 0:f.freeTex=0;break;case 1:f.freeTex=f.n-1;break;case 2:f.freeTex=f.n<17?f.n+r.Below(17-f.n):16;break;case 3:f.freeTex=r.Below(f.n);break;default:f.freeTex=r.Below(17);}
    switch(scenario/5%5) {case 0:f.freeIb=0;break;case 1:f.freeIb=15;break;case 2:f.freeIb=16;break;case 3:f.freeIb=r.Below(16);break;default:f.freeIb=r.Below(17);}
    int32 nextId=0;
    auto fillSlot=[&](Slot& s) {
        uint32 size=fill==0?0:fill==1?r.Below(3):fill==2?r.Below(13):r.Below(49);
        if(fill&&r.Below(8)==0)size=0;
        if(fill==3&&r.Below(16)==0)size=60;
        s.size=size;s.ids.fill(-2);
        for(uint32 i=0;i<size;++i) {
            if(r.Below(8)==0)s.ids[i]=-1;
            else if(nextId>0&&r.Below(6)==0)s.ids[i]=int32(r.Below(uint32(nextId)));
            else s.ids[i]=nextId++;
        }
        constexpr uint32 extremes[8]{0,1,0x7FFFFFFF,0x80000000,0xFFFFFFFF,0xFFFFFFFE,2,0x10000};
        switch(r.Below(4)) {case 0:s.num=size;break;case 1:s.num=size+r.Below(4);break;case 2:s.num=extremes[r.Below(8)];break;default:s.num=r.Next();}
    };
    for(auto& s:f.tex)fillSlot(s);
    for(auto& s:f.ib)fillSlot(s);
    fillSlot(f.gen);
    switch(mutationMode) {
        case 2:f.mut.at=1+r.Below(3);f.mut.n=1+r.Below(17);break;
        case 3:f.mut.at=1+r.Below(4);f.mut.texIndex=int32(r.Below(17));f.mut.ibIndex=int32(r.Below(17));break;
        case 4:f.mut.at=1+r.Below(3);f.mut.n=1+r.Below(17);f.mut.texIndex=int32(r.Below(17));f.mut.ibIndex=int32(r.Below(17));break;
        case 5:f.mut.at=2+r.Below(6);f.mut.texIndex=int32(r.Below(17));break;
        case 6:f.mut.at=1+r.Below(5);f.mut.ibIndex=int32(r.Below(17));break;
        case 7:f.mut.at=1+r.Below(2);f.mut.n=1;break;
        default:break;
    }
    (void)variant;
    return f;
}
inline json SlotJson(const Slot& s) {json ids=json::array();for(uint32 i=0;i<s.size;++i)ids.push_back(s.ids[i]);return {s.num,s.size,ids};}
inline json FixtureJson(const Fixture& f) {
    json tex=json::array(),ib=json::array();for(const auto& s:f.tex)tex.push_back(SlotJson(s));for(const auto& s:f.ib)ib.push_back(SlotJson(s));
    return {{"count",f.count},{"n",f.n},{"free_tex",f.freeTex},{"free_ib",f.freeIb},{"tex",tex},{"ib",ib},{"gen",SlotJson(f.gen)},
            {"mutation",{{"at",f.mut.at},{"n",f.mut.n},{"tex_index",f.mut.texIndex},{"ib_index",f.mut.ibIndex}}}};
}
// Listas de punteros del banco (las que las cabeceras de las caches senalan durante el par) y su relleno.
inline std::array<std::array<void*,kArray>,kTexSlots> texLists{};
inline std::array<std::array<void*,kArray>,kIbSlots> ibLists{};
inline std::array<void*,kArray> genList{};
inline void FillList(std::array<void*,kArray>& list,const Slot& s) {
    for(unsigned i=0;i<kArray;++i)list[i]=s.ids[i]==-2?reinterpret_cast<void*>(0xDEAD0000):s.ids[i]==-1?nullptr:static_cast<void*>(&pool[size_t(s.ids[i])]);
}
inline void Apply(const Fixture& f) {
    for(unsigned i=0;i<kObjects;++i)pool[i]={fakeVtable,i,0,0};
    traceLength=0;releaseNumber=0;unexpectedCalls=0;traceOverflow=false;trace.fill(0);mutation=f.mut;
    memset(reinterpret_cast<void*>(kRegion),0xA5,kRegionSize);
    PutInt(kNumTex,f.n);PutInt(kNumTex+4,0x55AA55AAu);PutInt(kFreeTex,f.freeTex);PutInt(kFreeIb,f.freeIb);
    for(unsigned i=0;i<kTexSlots;++i) {
        auto* b=reinterpret_cast<D3DTextureBuffer*>(kTexBase+i*sizeof(D3DTextureBuffer));
        FillList(texLists[i],f.tex[i]);
        b->m_nFormat=0x1000+i;b->m_nWidth=0x20<<(i%4);b->m_nLevels=-1;b->m_nCapcacity=kArray;b->m_nNumTexturesInBuffer=f.tex[i].num;b->m_nSize=f.tex[i].size;
        b->m_apTextures=reinterpret_cast<decltype(b->m_apTextures)>(texLists[i].data());
    }
    for(unsigned i=0;i<kIbSlots+1;++i) {
        auto* b=reinterpret_cast<D3DIndexDataBuffer*>(kIbBase+i*sizeof(D3DIndexDataBuffer));      // i == 16 es el buffer generico (0xC87FC8)
        const Slot& s=i<kIbSlots?f.ib[i]:f.gen;auto& list=i<kIbSlots?ibLists[i]:genList;
        FillList(list,s);
        b->m_nFormat=0x2000+i;b->field_4=7;b->m_nCapcacity=kArray;b->m_nNumDatasInBuffer=s.num;b->m_nSize=s.size;
        b->m_apIndexData=reinterpret_cast<decltype(b->m_apIndexData)>(list.data());
    }
}
// Memoria observable tras una llamada: region de globales, las 35 listas y el pool de objetos (contadores de Release).
inline std::vector<uint8> Observe() {
    std::vector<uint8> mem;mem.reserve(kRegionSize+sizeof(texLists)+sizeof(ibLists)+sizeof(genList)+sizeof(pool));
    auto add=[&](const void* p,size_t n){mem.insert(mem.end(),static_cast<const uint8*>(p),static_cast<const uint8*>(p)+n);};
    add(reinterpret_cast<void*>(kRegion),kRegionSize);add(texLists.data(),sizeof(texLists));add(ibLists.data(),sizeof(ibLists));add(genList.data(),sizeof(genList));add(pool.data(),sizeof(pool));
    return mem;
}

struct Result {uint32 eax,ecx,edx;bool stack,nonvolatile;};
// Llamada cdecl con un argumento de pila; EAX/ECX/EDX se siembran y se devuelven solo como informacion (no se comparan: ningun llamador los lee, ver README).
inline Result Gate(uintptr_t entry,uint32 argument){uint32 before{},after{},a{},c{},d{},b0{},s0{},i0{},p0{},b1{},s1{},i1{},p1{};
 __asm {mov before,esp} __asm {mov b0,ebx} __asm {mov s0,esi} __asm {mov i0,edi} __asm {mov p0,ebp}
 __asm {push argument} __asm {mov eax,0A5123456h} __asm {mov ecx,0B1234567h} __asm {mov edx,0C0DE5921h} __asm {call entry} __asm {add esp,4}
 __asm {mov a,eax} __asm {mov c,ecx} __asm {mov d,edx} __asm {mov after,esp} __asm {mov b1,ebx} __asm {mov s1,esi} __asm {mov i1,edi} __asm {mov p1,ebp}
 return {a,c,d,before==after,b0==b1&&s0==s1&&i0==i1&&p0==p1};}

inline void Update(){
 if(done)return;char exe[MAX_PATH]{};GetModuleFileNameA(nullptr,exe,MAX_PATH);const auto dir=std::filesystem::path(exe).parent_path();
 if(!std::filesystem::exists(dir/"dependencies21f-runtime.start")){done=true;return;}
 if(!log.is_open()){log.open(dir/"dependencies21f-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("dependencies21f");}
 if(!FindPlayerPed()||CCutsceneMgr::IsRunning())return;done=true;
 fakeVtable[0]=reinterpret_cast<const void*>(&FakeQueryInterface);fakeVtable[1]=reinterpret_cast<const void*>(&FakeAddRef);fakeVtable[2]=reinterpret_cast<const void*>(&FakeRelease);
 // Estado inicial de los objetivos antes de que HookRoute apague/encienda los hooks.
 std::array<HelperRoute,2> targetSnapshots{HelperRoute(fns[0].address),HelperRoute(fns[1].address)};
 json targetInitial=json::array();for(const auto& t:targetSnapshots)targetInitial.push_back(t.Metadata());
 OriginalDependencies499::HookRoute routeTex(fns[0].cls,fns[0].name,fns[0].address,&D3DResourceSystem::TidyUpD3DTextures);
 OriginalDependencies499::HookRoute routeIb(fns[1].cls,fns[1].name,fns[1].address,&D3DResourceSystem::TidyUpD3DIndexBuffers);
 OriginalDependencies499::HookRoute* routes[2]{&routeTex,&routeIb};
 bool ready=true;for(auto* route:routes)ready&=route->Registered()&&ModuleOf(route->Body())==ModuleOf(uintptr_t(&ModuleMarker));
 // Bloques de la memoria real del juego que se guardan y se comprueban (y restauran): la region de las cabeceras y las listas reales a las que apuntan
 // (si el par llegase a escribir en ellas, o a liberar un recurso real, se vería aqui). Las listas reales solo se leen para guardarlas.
 std::vector<Block> blocks;blocks.emplace_back(kRegion,kRegionSize);
 unsigned realLists=0;const uint32 realN=ReadInt(kNumTex);
 if(realN>=1&&realN<=kTexSlots) {
     auto protect=[&](uintptr_t listAt,uintptr_t capAt){const uint32 list=ReadInt(listAt),cap=ReadInt(capAt);if(list&&cap&&cap<=65536){blocks.emplace_back(list,size_t(cap)*4);++realLists;}};
     for(unsigned i=0;i<kTexSlots;++i)protect(kTexBase+i*0x1C+0x18,kTexBase+i*0x1C+0xC);
     for(unsigned i=0;i<kIbSlots+1;++i)protect(kIbBase+i*0x18+0x14,kIbBase+i*0x18+0x8);
 }
 SaveFP(savedFP.data());savedMXCSR=ReadMXCSR();savedFPActive=true;
 auto restore=[&]{for(const auto& b:blocks)b.Restore();RestoreFloatingState();};
 auto restored=[&]{return std::ranges::all_of(blocks,[](const auto& b){return b.Restored();})&&FloatingStateRestored();};
 json layout=json::array();for(const auto& b:blocks)layout.push_back({{"address",b.address},{"bytes",b.saved.size()}});
 Write({{"event","begin"},{"registered_hook",ready},{"functions",2},{"pairs",2*kCasesPerFunction},{"cases",kCasesPerFunction},{"collaborators_stubbed",true},{"com_objects_fake",true},
        {"real_resources_released",false},{"helper_routes",json::array()},{"target_initial",targetInitial},{"snapshot_layout",layout},{"real_lists_protected",realLists},
        {"initial_region",Hex(kRegion,kRegionSize)},{"initial_fp",Hex(uintptr_t(savedFP.data()),108)},{"initial_mxcsr",savedMXCSR},
        {"bodies",{routeTex.Body(),routeIb.Body()}},{"asi_module",ModuleOf(uintptr_t(&ModuleMarker))},{"original_module",uintptr_t(GetModuleHandleA(nullptr))}});
 int pairs{};bool passed=ready;std::array<int,2> failures{};
 for(unsigned function=0;function<2&&ready;++function) {
  auto& route=*routes[function];const auto entry=fns[function].address;
  for(unsigned scenario=0;scenario<kCasesPerFunction;++scenario) {
   const Fixture fx=MakeFixture(function,scenario);
   struct Observation {json out;std::vector<uint8> memory;Result regs{};};
   auto invoke=[&](bool candidate){
    restore();_controlfp(_PC_24|_RC_NEAR,_MCW_PC|_MCW_RC);Apply(fx);
    alignas(16)std::array<uint8,108> fp{};uint32 mx{};bool routeOK{};Result regs{};
    route.Invoke(candidate,[&]{routeOK=candidate?ReversingProbe::JumpsToDll(entry)&&ModuleOf(Resolve(entry))==ModuleOf(uintptr_t(&ModuleMarker)):!ReversingProbe::JumpsToDll(entry);regs=Gate(entry,fx.count);SaveFP(fp.data());mx=ReadMXCSR();return 0;});
    memset(fp.data()+12,0,4);     // FIP: direccion de la ultima instruccion x87 (ninguna de las dos funciones usa x87; se normaliza como en los hermanos)
    Observation o;o.regs=regs;o.memory=Observe();
    json traceJson=json::array();for(unsigned i=0;i<traceLength;++i)traceJson.push_back({trace[2*i],trace[2*i+1]});
    o.out={{"globals",Hex(kRegion,kRegionSize)},{"trace",traceJson},{"releases",releaseNumber},{"memory_len",o.memory.size()},{"memory_fnv64",Fnv64(o.memory)},
           {"post_floating_state",Hex(uintptr_t(fp.data()),108)},{"post_mxcsr",mx},{"stack_restored",regs.stack},{"nonvolatile_preserved",regs.nonvolatile},{"route_ok",routeOK},
           {"com_calls_ok",unexpectedCalls==0&&!traceOverflow}};
    restore();o.out["globals_restored"]=restored();o.out["floating_state_restored"]=FloatingStateRestored();return o;
   };
   const auto a=invoke(false),b=invoke(true);
   bool ok=a.out==b.out&&a.memory==b.memory;
   for(const char* k:{"stack_restored","nonvolatile_preserved","route_ok","com_calls_ok","globals_restored","floating_state_restored"})ok&=a.out[k]==true&&b.out[k]==true;
   passed&=ok;failures[function]+=!ok;
   Write({{"event","comparison"},{"function",fns[function].full},{"case",scenario},{"pair",pairs++},{"fixture",FixtureJson(fx)},{"original",a.out},{"candidate",b.out},
          {"memory_bytes_equal",a.memory==b.memory},
          {"registers_not_compared",{{"original",{a.regs.eax,a.regs.ecx,a.regs.edx}},{"candidate",{b.regs.eax,b.regs.ecx,b.regs.edx}}}},
          {"equal",ok},{"accepted",ok}});
  }
  Write({{"event","function_summary"},{"function",fns[function].full},{"cases",kCasesPerFunction},{"failures",failures[function]},{"state_restored",route.Restore()}});
 }
 restore();bool stateRestored=restored();stateRestored&=routeTex.Restore();stateRestored&=routeIb.Restore();for(const auto& t:targetSnapshots)stateRestored&=t.Check();
 Write({{"event","global_state_final"},{"region",Hex(kRegion,kRegionSize)},{"mxcsr",ReadMXCSR()},{"state_restored",stateRestored}});
 Write({{"event",passed&&stateRestored?"complete":"error"},{"pairs",pairs},{"state_restored",stateRestored},{"floating_state_restored",FloatingStateRestored()},{"helper_routes_ok",true},{"target_restored",stateRestored}});
 restore();savedFPActive=false;
}
}
