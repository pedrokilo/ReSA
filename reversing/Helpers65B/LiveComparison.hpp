#pragma once
// Banco dentro del motor (lote 65B): seis ayudantes pequenos del EXE reconstruidos en C++.
//   CCamera::IsActiveCamUnderWater          0x50B830  cdecl sin argumentos, resultado en AL
//   CCam::ResetDWHeliChaseParams            0x50E180  thiscall (ECX = parametros, 0x9C bytes); deja EAX = 0, ECX = this, EDX = 0x3C (el thunk 0x5173F0 devuelve ECX)
//   CCam::RandomiseDWHeliChaseParams        0x50E090  thiscall (ECX = parametros); cinco rand() del EXE (0x821B1E) y x87
//   CHeli::FindSwatPositionRelativeToHeli   0x6C4760  thiscall, puntero oculto + indice, ret 8; EDX intacto (el llamador 0x6C7844 lo conserva), EAX = puntero oculto
//   CTaskComplexGoPickUpEntity::Constructor 0x6919C0  thiscall (entidad, grupo de animacion), ret 8; EAX = this
//   CTaskComplexGoPickUpEntity::Destructor  0x691A50  thiscall sin argumentos
// Cada funcion se llama en la direccion del EXE con una sola puerta de ABI (Gate): una vez con el hook apagado (codigo del EXE) y otra con el hook
// encendido (el EXE salta al envoltorio de la DLL). Se siembran 64 dwords bajo ESP, EBX/ESI/EDI, EAX/ECX/EDX y se exige ESP, no volatiles, DF, cadena SEH,
// FCW x87 = 0x7F (precision de 24 bits), pila x87 vacia y MXCSR = 0x9F80 (FTZ, el del juego) tras la llamada. Se compara la memoria completa (arena con guardas de 0x20
// bytes rellenada con bytes deterministas por caso), el resultado y la traza de colaboradores. Colaboradores sustituidos por falsos identicos en ambos lados
// (parche en la direccion del EXE y en la entrada de la DLL): CWaterLevel::GetWaterLevel (0x6EB690), rand del CRT del EXE (0x821B1E),
// CEntity::RegisterReference (0x571B70), CEntity::CleanUpOldReference (0x571A00), CAnimManager::RemoveAnimBlockRef (0x4D3FD0) y el destructor virtual de la
// subtarea falsa del destructor. EAX/ECX/EDX solo se registran (info) salvo donde la funcion los reproduce a proposito (Reset: EAX/ECX/EDX; Swat: EAX/ECX/EDX).
// Se tocan globales del juego (camara activa y fuentes de camara, tabla de grupos de animacion): se guardan y se restauran por caso.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "WaterLevel.h"
#include "Entity/Entity.h"
#include "Entity/Vehicle/Heli.h"
#include "Animation/AnimManager.h"
#include "Tasks/TaskTypes/TaskComplexGoPickUpEntity.h"
#include "../ProbeRuntime.hpp"

namespace Helpers65BLive {
using u32 = uint32;
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
inline std::string Hex(const void* p,size_t n){ constexpr char d[]="0123456789abcdef"; const auto* b=static_cast<const uint8*>(p); std::string o; o.reserve(n*2); for(size_t i=0;i<n;++i){o+=d[b[i]>>4];o+=d[b[i]&15];} return o; }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    template<class V> V pick(std::initializer_list<V> l){ return l.begin()[next()%l.size()]; } };

// --- buferes con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    return "other";
}
template<class V> inline json Jv(V v){
    if constexpr(std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    void Reset(){ while(!v.empty())v.pop_back(); seen.clear(); }
    ~Patches(){ Reset(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }
// entradas del EXE de los colaboradores sustituidos: tras quitar los parches deben ser lo que eran al empezar (solo se comparan, nunca se restauran)
inline std::vector<Region> watches;
inline void Watch(uintptr_t p,size_t n){ watches.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline bool WatchesIntact(){ for(auto& g:watches)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// cuerpo de la DLL de una funcion enganchada (con el hook apagado la entrada del EXE salta a la DLL solo cuando esta encendido)
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }

// direcciones de la DLL de los colaboradores (CEntity tiene sobrecargas de plantilla estaticas con el mismo nombre)
inline uintptr_t DllRegisterReference(){ return Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)); }
inline uintptr_t DllCleanUpOldReference(){ return Addr(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)); }
inline uintptr_t DllRemoveAnimBlockRef(){ return Addr(static_cast<void(*)(int32)>(&CAnimManager::RemoveAnimBlockRef)); }
inline uintptr_t DllGetWaterLevel(){ return Addr(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)); }

// ======================= estado x87 / SSE =======================
inline constexpr uint16 kFcw=0x007F; inline constexpr uint32 kMxcsr=0x9F80;
struct FpSaved { alignas(16) uint8 x87[108]; uint32 mxcsr; };
inline void FpEnter(FpSaved& s){ void* p=s.x87; uint32 m{}; __asm { mov eax,p } __asm { fnsave [eax] } __asm { stmxcsr m } s.mxcsr=m;
    const uint16 cw=kFcw; const uint32 mx=kMxcsr; __asm { fldcw cw } __asm { ldmxcsr mx } }
// true si tras la llamada: FCW = 0x27F, TOP = 0, pila vacia y MXCSR = 0x9F80 (salvo banderas pegajosas). Siempre restaura el estado previo.
inline bool FpLeave(FpSaved& s){ alignas(16) uint8 env[28]{}; void* p=env; uint32 m{}; __asm { mov eax,p } __asm { fnstenv [eax] } __asm { stmxcsr m }
    const uint16 fcw=*reinterpret_cast<uint16*>(env), fsw=*reinterpret_cast<uint16*>(env+4), ftw=*reinterpret_cast<uint16*>(env+8);
    const bool ok=(fcw&0xFFFF)==kFcw && ((fsw>>11)&7)==0 && ftw==0xFFFF && (m&0xFFC0)==(kMxcsr&0xFFC0);
    void* q=s.x87; const uint32 mx=s.mxcsr; __asm { mov eax,q } __asm { frstor [eax] } __asm { ldmxcsr mx }
    return ok; }

// ======================= puerta de ABI =======================
struct Reg { u32 eax,ecx,edx,eflags; bool stack,nonvolatile,seh; };
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

// ======================= arenas =======================
constexpr size_t kGuard=0x20;
struct Arena { std::vector<uint8> mem; size_t size{}; uint8* obj() { return mem.data()+kGuard; }
    void Init(size_t n,Rng& r){ size=n; mem.assign(n+2*kGuard,0); for(auto& b:mem)b=uint8(r.next()); }
    std::string Hex() { return Helpers65BLive::Hex(mem.data(),mem.size()); } };
inline Arena arena;
inline uint8 entityBuf[0x40]; inline uint8 fakeTaskBuf[0x40]; inline void* fakeTaskVt[4];
struct alignas(8) GroupRaw { uint32 w[5]; };
inline std::array<GroupRaw,64> fakeGroups{};

// ======================= falsos (colaboradores) =======================
inline bool waterFound{}; inline float waterLevel{};
inline bool __cdecl S_Water(float x,float y,float z,float* out,uint32 touch,CVector* normal){
    T("water",x,y,z,touch&0xFF,normal!=nullptr);
    if(!waterFound)return false;
    *out=waterLevel; return true; }
inline std::array<int32,8> randQueue{}; inline uint32 randPos{};
inline int __cdecl S_Rand(){ const int32 v=randQueue[randPos++%randQueue.size()]; T("rand",v); return v; }
inline void __fastcall S_RegisterReference(void* self,void*,void* pp){ T("RegisterReference",self,pp); }
inline void __fastcall S_CleanUpOldReference(void* self,void*,void* pp){ T("CleanUpOldReference",self,pp); }
inline void __cdecl S_RemoveAnimBlockRef(int32 index){ T("RemoveAnimBlockRef",index); }
inline void* __fastcall S_SubtaskDtor(void* self,void*,uint32 flags){ T("subtask_dtor",self,flags); return self; }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,6> fns{{
    {"CCamera","IsActiveCamUnderWater",0x50B830,0,-1,4608},
    {"CCam","ResetDWHeliChaseParams",0x50E180,0,-1,4096},
    {"CCam","RandomiseDWHeliChaseParams",0x50E090,0,-1,6144},
    {"CHeli","FindSwatPositionRelativeToHeli",0x6C4760,0,-1,4096},
    {"CTaskComplexGoPickUpEntity","Constructor",0x6919C0,0,-1,4096},
    {"CTaskComplexGoPickUpEntity","Destructor",0x691A50,0,-1,4096},
}};

// ======================= fixtures =======================
inline float RandFloat(Rng& r){
    switch(r.below(14)){
        case 0: return 0.f; case 1: return -0.f;
        case 2: return std::numeric_limits<float>::infinity(); case 3: return -std::numeric_limits<float>::infinity();
        case 4: return std::bit_cast<float>(0x7FC00000u|(r.next()&0x3FFFFFu));              // NaN silencioso
        case 5: return std::bit_cast<float>(0x7F800001u|(r.next()&0x3FFFFFu));              // NaN senalizador
        case 6: return std::bit_cast<float>(r.next()&0x807FFFFFu);                          // subnormal / cero
        case 7: return std::bit_cast<float>((r.next()&0x80000000u)|(0x7F000000u+(r.next()&0x7FFFFFu))); // enorme
        case 8: return std::bit_cast<float>((r.next()&0x80000000u)|(0x00800000u+(r.next()&0xFFFFFu)));  // diminuto normal
        case 9: return r.uni(-3000.f,3000.f);
        case 10: return r.uni(-1.f,1.f);
        case 11: return r.uni(-100.f,100.f);
        case 12: return float(int32(r.below(2001))-1000);
        default: return std::bit_cast<float>(r.next()); }
}
inline float FiniteFloat(Rng& r){ float f; do f=RandFloat(r); while(!std::isfinite(f)); return f; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"helpers65b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"helpers65b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("helpers65b");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    InitModule();
    { uint16 fcw0{}; uint32 mx0{}; __asm { fnstcw fcw0 } __asm { stmxcsr mx0 } Write({{"event","fp_environment"},{"fcw",fcw0},{"mxcsr",mx0}}); }
    std::string only; { std::ifstream f(dir/"helpers65b-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,6> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,6> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // cuerpos de la DLL de las funciones probadas: no pueden coincidir con ningun falso
    std::array<uintptr_t,6> body{};
    for(size_t f=0;f<fns.size();++f){ body[f]=DllBody(fns[f].cls,fns[f].name); if(!body[f]||!InDll(body[f])){ Write({{"event","error"},{"reason","DLL body not found"},{"function",fns[f].name}}); return; } }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    {
      alignas(16) static uint8 tmp[0x60]{}; auto* P=reinterpret_cast<CTaskComplexGoPickUpEntity*>(tmp);
      auto Off=[&](const void* m){ return uint32(static_cast<const uint8*>(m)-tmp); };
      json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      Chk("m_pEntity",Off(&P->m_pEntity),0xC); Chk("m_vecPosition",Off(&P->m_vecPosition),0x10); Chk("m_vecPickupPosition",Off(&P->m_vecPickupPosition),0x1C);
      Chk("m_nTimePassed",Off(&P->m_nTimePassedSinceLastSubTaskCreatedInMs),0x28); Chk("m_nAnimGroupId",Off(&P->m_nAnimGroupId),0x2C); Chk("m_bAnimBlockReferenced",Off(&P->m_bAnimBlockReferenced),0x30);
      Chk("sizeof(CTaskComplexGoPickUpEntity)",uint32(sizeof(CTaskComplexGoPickUpEntity)),0x34);
      Chk("sizeof(CAnimBlendAssocGroup)",uint32(sizeof(CAnimBlendAssocGroup)),0x14); Chk("sizeof(CAnimBlock)",uint32(sizeof(CAnimBlock)),0x20);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    if(named.empty()){ Nm(entityBuf,sizeof(entityBuf),"entity"); Nm(fakeTaskBuf,sizeof(fakeTaskBuf),"subtask"); }
    for(auto& v:fakeTaskVt)v=reinterpret_cast<void*>(&S_SubtaskDtor);

    // estado global guardado: camara activa y fuente de cada camara, tabla de grupos de animacion (puntero y contador)
    if(watches.empty()){ for(uintptr_t a:{0x6EB690u,0x821B1Eu,0x571B70u,0x571A00u,0x4D3FD0u})Watch(a,8); for(auto f:fns)Watch(f.address,8);
        Watch(DllGetWaterLevel(),8); Watch(DllRegisterReference(),8); Watch(DllCleanUpOldReference(),8); Watch(DllRemoveAnimBlockRef(),8); }
    if(regions.empty()){ Snap(0xB6F081,1); for(int k=0;k<3;++k)Snap(0xB6F338+0x238*k,12); Snap(0xB4EA34,4); Snap(0xB4EA28,4); }
    // estado de los hooks de los colaboradores sustituidos: la entrada del EXE se parchea de todos modos, no hace falta tocarlos
    const uint32 numBlocks=std::min<uint32>(AtA<uint32>(0xB4EA30),180);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            std::string info0, info1;
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x65B1u; if(!stt)stt=1; Rng r{stt};
                trace=json::array();
                hooks[f]->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                json out={{"switches",switches}};
                Patches P;
                const u32 seedEax=r.next(), seedEdx=r.next(), stackSeed=r.next();
                FpSaved fp; Reg reg{}; bool fpOk=false;
                json info=json::object();
                switch(f){
                case 0: {   // ---------------- CCamera::IsActiveCamUnderWater ----------------
                    P.Add(0x6EB690,S_Water); P.Add(DllGetWaterLevel(),S_Water);
                    const uint32 active=r.below(3);
                    json cams=json::array();
                    for(uint32 k=0;k<3;++k){
                        float x=RandFloat(r),y=RandFloat(r),z=RandFloat(r);
                        if(r.pct(50)){ x=r.uni(-3000.f,3000.f); y=r.uni(-3000.f,3000.f); }
                        if(r.pct(35))z=r.uni(-30.f,30.f);
                        if(k!=active&&r.pct(50)){ x=0.f; y=0.f; }
                        AtA<float>(0xB6F338+0x238*k+0)=x; AtA<float>(0xB6F338+0x238*k+4)=y; AtA<float>(0xB6F338+0x238*k+8)=z;
                    }
                    AtA<uint8>(0xB6F081)=uint8(active);
                    const float z=AtA<float>(0xB6F338+0x238*active+8);
                    waterFound=r.pct(85);
                    switch(r.below(12)){
                        case 0: waterLevel=z; break;
                        case 1: waterLevel=std::nextafter(z,std::numeric_limits<float>::infinity()); break;
                        case 2: waterLevel=std::nextafter(z,-std::numeric_limits<float>::infinity()); break;
                        case 3: waterLevel=z+r.uni(-50.f,50.f); break;
                        case 4: waterLevel=std::bit_cast<float>(0x7FC00000u|(r.next()&0x3FFFFFu)); break;
                        case 5: waterLevel=std::numeric_limits<float>::infinity(); break;
                        case 6: waterLevel=-std::numeric_limits<float>::infinity(); break;
                        case 7: waterLevel=std::bit_cast<float>(r.next()&0x807FFFFFu); break;
                        case 8: waterLevel=r.pct(50)?0.f:-0.f; break;
                        case 9: waterLevel=std::bit_cast<float>(r.next()); break;
                        case 10: waterLevel=z; break;
                        default: waterLevel=r.uni(-10.f,10.f); }
                    FpEnter(fp);
                    reg=Gate(fn.address,nullptr,0,seedEax,r.next(),seedEdx,stackSeed,0);
                    fpOk=FpLeave(fp);
                    out["cams"]=Fnv(reinterpret_cast<void*>(0xB6F338),12)^Fnv(reinterpret_cast<void*>(0xB6F338+0x238),12)^Fnv(reinterpret_cast<void*>(0xB6F338+0x238*2),12);
                    out["active"]=AtA<uint8>(0xB6F081);
                    out["al"]=reg.eax&0xFF;
                    info={{"eax",reg.eax},{"ecx",reg.ecx},{"edx",reg.edx}};
                    break; }
                case 1: {   // ---------------- CCam::ResetDWHeliChaseParams ----------------
                    arena.Init(0x9C,r);
                    FpEnter(fp);
                    reg=Gate(fn.address,nullptr,0,seedEax,u32(reinterpret_cast<uintptr_t>(arena.obj())),seedEdx,stackSeed,0);
                    fpOk=FpLeave(fp);
                    out["mem"]=arena.Hex();
                    out["eax"]=reg.eax; out["ecx_is_this"]=(reg.ecx==u32(reinterpret_cast<uintptr_t>(arena.obj()))); out["edx"]=reg.edx;
                    break; }
                case 2: {   // ---------------- CCam::RandomiseDWHeliChaseParams ----------------
                    P.Add(0x821B1E,S_Rand);
                    arena.Init(0x9C,r);
                    for(uint32 off=0x18;off<=0x28;off+=4)At<float>(arena.obj(),off)=RandFloat(r);
                    for(auto& q:randQueue)q=r.pct(15)?r.pick<int32>({0,1,32766,32767,16383,16384}):int32(r.below(32768));
                    randPos=0;
                    FpEnter(fp);
                    reg=Gate(fn.address,nullptr,0,seedEax,u32(reinterpret_cast<uintptr_t>(arena.obj())),seedEdx,stackSeed,0);
                    fpOk=FpLeave(fp);
                    out["mem"]=arena.Hex();
                    info={{"eax",reg.eax},{"ecx_is_this",reg.ecx==u32(reinterpret_cast<uintptr_t>(arena.obj()))},{"edx",reg.edx}};
                    break; }
                case 3: {   // ---------------- CHeli::FindSwatPositionRelativeToHeli ----------------
                    arena.Init(12,r);
                    const int32 swat=r.pct(50) ? int32(r.below(4)) : r.pick<int32>({-1,4,5,7,8,100,int32(0x7FFFFFFF),int32(0x80000000),int32(0xFFFFFFFFu),int32(r.next()),-2,int32(0x80000001)});
                    const u32 self=r.pct(30)?0u:r.next();
                    const u32 args[2]{u32(reinterpret_cast<uintptr_t>(arena.obj())),u32(swat)};
                    FpEnter(fp);
                    reg=Gate(fn.address,args,2,seedEax,self,seedEdx,stackSeed,0);
                    fpOk=FpLeave(fp);
                    out["swat"]=swat; out["mem"]=arena.Hex();
                    out["eax_is_out"]=(reg.eax==args[0]); out["ecx"]=reg.ecx; out["self"]=self; out["edx_kept"]=(reg.edx==seedEdx);
                    break; }
                case 4: {   // ---------------- CTaskComplexGoPickUpEntity::Constructor ----------------
                    P.Add(0x571B70,S_RegisterReference); P.Add(DllRegisterReference(),S_RegisterReference);
                    arena.Init(0x34,r);
                    uint8* obj=arena.obj();
                    void* entity= r.pct(30) ? nullptr : entityBuf+r.below(0x30);
                    const int32 group= r.pct(30) ? 81 : r.pct(50) ? int32(r.below(64)) : r.pick<int32>({-1,0,int32(0x7FFFFFFF),int32(0x80000000),255,256,int32(r.next())});
                    const u32 args[2]{u32(reinterpret_cast<uintptr_t>(entity)),u32(group)};
                    FpEnter(fp);
                    reg=Gate(fn.address,args,2,seedEax,u32(reinterpret_cast<uintptr_t>(obj)),seedEdx,stackSeed,0);
                    fpOk=FpLeave(fp);
                    const uint32 vp=At<uint32>(obj,0); const bool vptrOk= candidate ? InDll(vp) : vp==0x870B98u;
                    At<uint32>(obj,0)=0;
                    out["vptr_ok"]=vptrOk; out["mem"]=arena.Hex(); out["group"]=group;
                    out["eax_is_this"]=(reg.eax==u32(reinterpret_cast<uintptr_t>(obj)));
                    info={{"ecx_is_this",reg.ecx==u32(reinterpret_cast<uintptr_t>(obj))},{"edx",reg.edx}};
                    break; }
                default: {  // ---------------- CTaskComplexGoPickUpEntity::Destructor ----------------
                    P.Add(0x571A00,S_CleanUpOldReference); P.Add(DllCleanUpOldReference(),S_CleanUpOldReference);
                    P.Add(0x4D3FD0,S_RemoveAnimBlockRef); P.Add(DllRemoveAnimBlockRef(),S_RemoveAnimBlockRef);
                    arena.Init(0x34,r);
                    uint8* obj=arena.obj();
                    // tabla de grupos falsa: cada entrada apunta a un bloque real de animaciones (o nulo / desalineado)
                    for(size_t g=0;g<fakeGroups.size();++g){ fakeGroups[g]={}; const uint32 k=r.below(numBlocks?numBlocks:1);
                        const uint32 mode=r.below(20);
                        fakeGroups[g].w[0]= mode==0 ? 0u : mode==1 ? 0xB5D4A0u+0x20*k+0x10 : 0xB5D4A0u+0x20*k; }
                    AtA<uint32>(0xB4EA34)=u32(reinterpret_cast<uintptr_t>(fakeGroups.data())); AtA<uint32>(0xB4EA28)=u32(fakeGroups.size());
                    At<uint32>(obj,0)=r.next(); At<uint32>(obj,4)=r.next();
                    At<void*>(obj,8)= r.pct(40) ? static_cast<void*>(fakeTaskBuf) : nullptr;
                    if(At<void*>(obj,8))At<void*>(fakeTaskBuf,0)=fakeTaskVt;
                    At<void*>(obj,0xC)= r.pct(60) ? static_cast<void*>(entityBuf+r.below(0x30)) : nullptr;
                    At<int32>(obj,0x2C)= int32(r.below(fakeGroups.size()));
                    At<uint8>(obj,0x30)= r.pct(50) ? uint8(1) : r.pct(30) ? uint8(0) : uint8(r.next());
                    FpEnter(fp);
                    reg=Gate(fn.address,nullptr,0,seedEax,u32(reinterpret_cast<uintptr_t>(obj)),seedEdx,stackSeed,0);
                    fpOk=FpLeave(fp);
                    const uint32 vp=At<uint32>(obj,0); const bool vptrOk= candidate ? InDll(vp) : vp==0x86D48Cu;
                    At<uint32>(obj,0)=0;
                    out["vptr_ok"]=vptrOk; out["mem"]=arena.Hex();
                    info={{"eax",reg.eax},{"ecx_is_this",reg.ecx==u32(reinterpret_cast<uintptr_t>(obj))},{"edx",reg.edx}};
                    break; }
                }
                out["abi"]={{"stack",reg.stack},{"nonvolatile",reg.nonvolatile},{"seh",reg.seh},{"df_clear",(reg.eflags&0x400)==0},{"fp_ok",fpOk}};
                out["trace"]=trace;
                (candidate ? info1 : info0)=info.dump();
                return out;
            };
            const auto original=run(false);
            RestoreRegions();
            const auto candidate=run(true);
            RestoreRegions();
            const bool abiGood=[&](const json& r){ return r["abi"]["stack"]==true&&r["abi"]["nonvolatile"]==true&&r["abi"]["seh"]==true&&r["abi"]["df_clear"]==true&&r["abi"]["fp_ok"]==true; }(original);
            const bool vptrGood=!original.contains("vptr_ok")||(original["vptr_ok"]==true&&candidate["vptr_ok"]==true);
            // normaliza el campo vptr_ok (distinto por construccion en cada lado: ya se exigio arriba)
            json o2=original, c2=candidate; o2.erase("vptr_ok"); c2.erase("vptr_ok");
            const bool ok=o2==c2&&original["switches"]==true&&candidate["switches"]==true&&abiGood&&vptrGood;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(o2)},{"candidate",Summary(c2)},{"events",original["trace"].size()},{"regs_original",info0},{"regs_candidate",info1}});
            else { json diff=json::array(); for(auto& [k,v]:o2.items())if(c2[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"abi_good",abiGood},{"vptr_good",vptrGood},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)},{"regs_original",info0},{"regs_candidate",info1}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",RegionsRestored()&&WatchesIntact()},{"regions_restored",RegionsRestored()},{"entries_intact",WatchesIntact()}});
}
}
