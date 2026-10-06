#pragma once
// Banco dentro del motor: constructores de CBmx (0x6BF820) y CQuadBike (0x6CE370). Cada constructor se ejecuta sobre un bufer
// del tamano del objeto (0x838 / 0x9BC bytes) rellenado con 0xCD mas una franja de guarda: una vez con el codigo del EXE (hook
// desactivado) y otra a traves del hook (envoltorio de la DLL -> constructor de la DLL), ambas llamando a la direccion del EXE.
// Se compara el bufer COMPLETO byte a byte (vptr normalizado: 0x871528 / 0x871AE8 en el EXE o vtable de la DLL) y la traza de
// llamadas salientes.
// Los constructores de las bases (CBike 0x6BF430 / CAutomobile 0x6B0A90, que a su vez construyen CVehicle y CPhysical y tocan
// pools, mundo, RwObjects y handling) no se ejecutan de verdad: se sustituyen, en el EXE y en el constructor de la DLL localizado
// por el banco, por un doble que traza (this, modelo, creationBy) y rellena la parte de la base con bytes deterministas por caso
// (no con 0xCD) y deja en el sitio los punteros a los nodos de la bici que lee CBmx. Asi se valida SOLO la parte propia de cada
// constructor, pero con todos los campos de la base presentes, de modo que una puesta a cero de mas (`{}`) o un campo no escrito
// aparece como diferencia.
// Tambien se simulan: la tabla de model infos (ms_modelInfoPtrs 0xA9B0C8: se guarda y se sustituye por un modelo falso con vtable
// trazada; GetAnimFileIndex, hueco 14) y la entrada del bloque de animacion (GroupId en 0xB5D4A0+0x20*i+0x1C). CAutomobile::
// SetupSuspensionLines (0x6A65D0 / cuerpo de la DLL) es un doble trazado que anota el hash del objeto en ese instante (orden de
// escrituras). RwMatrixTransform (0x7F25A0) se ejecuta de verdad en las dos versiones (es matematica pura sobre matrices de marcos
// falsos). No se asigna memoria ni se toca ningun pool: no hay que parchear operator new/delete.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Bmx.h"
#include "Entity/Vehicle/QuadBike.h"
#include "Entity/Vehicle/Bike.h"
#include "cHandlingDataMgr.h"
#include "../ProbeRuntime.hpp"

namespace VehCtor62Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; }
    float pick(std::initializer_list<float> l){ return l.begin()[next()%l.size()]; }
    int32 pickI(std::initializer_list<int32> l){ return l.begin()[next()%l.size()]; } };

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
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos (como Sys13 / Boat23) ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
inline bool Prologue(uintptr_t f){ const auto* b=reinterpret_cast<const uint8*>(f); if(b[0]==0x8B&&b[1]==0xFF)b+=2; return b[0]==0x55&&b[1]==0x8B&&b[2]==0xEC; }
// primera llamada (E8) a una funcion de la DLL con prologo push ebp / mov ebp,esp
inline uintptr_t FirstDllCall(uintptr_t fn){ fn=FollowIn(fn); if(!InDll(fn))return 0;
    for(uintptr_t p=fn;p<fn+0x300;++p){ if(*reinterpret_cast<const uint8*>(p)!=0xE8)continue; const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1);
        if(InDll(t)&&Prologue(FollowIn(t)))return FollowIn(t); }
    return 0; }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
// cuerpo de la DLL de una funcion enganchada
inline uintptr_t DllBody(const char* cls,const char* fn){
    auto* h=FindHook(cls,fn); if(!h)return 0; uintptr_t a=0;
    if(!strcmp(h->Symbol(),"V"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    else if(!strcmp(h->Symbol(),"S"))a=uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return a ? FollowIn(a) : 0; }

// ======================= objetos falsos =======================
constexpr size_t kBmxSize=0x838, kQuadSize=0x9BC, kGuard=32;
alignas(16) inline std::array<uint8,kBmxSize+kGuard> bmx{};
alignas(16) inline std::array<uint8,kQuadSize+kGuard> quad{};
struct alignas(16) FrameBuf { uint8 b[0xB0]; };          // RwFrame falso (0xA8 reales; matriz de modelado en +0x10, padre en +4)
alignas(16) inline std::array<FrameBuf,8> frames{};
alignas(16) inline std::array<uint8,0x80> mi{};           // CVehicleModelInfo falso (m_nHandlingId en +0x4A)
inline std::array<void*,40> miVt{};

// estado del caso actual compartido con los dobles
inline uint8* curObj{}; inline size_t curSize{};
inline uint32 baseSeed{}; inline int32 animIdx{};
inline void* nodeFront{}; inline void* nodeRear{};

inline uint64 ObjHash(){ return Fnv(curObj+4,curSize-4); }   // sin la vptr (distinta en el EXE y la DLL)
inline void FillBase(void* self,size_t n){ Rng r{baseSeed}; size_t k=4; for(;k+4<=n;k+=4)At<uint32>(self,k)=r.next(); for(;k<n;++k)At<uint8>(self,k)=uint8(r.next()); }

// --- dobles de las bases (se llaman en la direccion del EXE y en el constructor de la DLL) ---
// CBike::CBike(int32 modelIndex, eVehicleCreatedBy) : thiscall, ret 8
inline void* __fastcall S_BikeCtor(void* self,void*,int32 model,uint32 createdBy){
    T("CBike_ctor",self,model,createdBy);
    FillBase(self,0x814);                          // CBike entero (0x814 bytes) con bytes deterministas
    At<void*>(self,0x5B0)=nodeFront; At<void*>(self,0x5B4)=nodeRear;   // m_aBikeNodes[BMX_WHEEL_FRONT/REAR]
    return self; }
// CAutomobile::CAutomobile(int32, eVehicleCreatedBy, bool) : thiscall, ret 0xC
inline void* __fastcall S_AutoCtor(void* self,void*,int32 model,uint32 createdBy,uint32 flag){
    T("CAutomobile_ctor",self,model,createdBy,uint8(flag));
    FillBase(self,0x988);                          // CAutomobile entero (0x988 bytes)
    return self; }
// CAutomobile::SetupSuspensionLines (0x6A65D0 y cuerpo de la DLL): se anota el hash del objeto en ese instante
inline void __fastcall S_AutoSetup(void* self,void*){ T("CAutomobile_SetupSuspensionLines",self,ObjHash()); }
// vtable del model info falso
inline void __fastcall V_Trap(void* self,void*){ T("virtual_inesperada",self); }
inline int32 __fastcall V_AnimIdx(void* self,void*){ T("GetAnimFileIndex",self,ObjHash()); return animIdx; }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,2> fns{{
    {"CBmx","Constructor",0x6BF820,0,-1,1024},
    {"CQuadBike","Constructor",0x6CE370,0,-1,512},
}};

// ======================= fixtures =======================
inline void FillFrameMatrix(uint8* f,Rng& r){
    for(size_t k=0;k<sizeof(FrameBuf);++k)f[k]=uint8(r.next());
    uint8* m=f+0x10;                                // RwMatrix: right 0, flags 0xC, up 0x10, at 0x20, pos 0x30
    const bool ident=r.pct(12);
    for(int row=0;row<3;++row)for(int c=0;c<3;++c)At<float>(m,0x10*row+4*c)=ident ? (row==c?1.f:0.f) : r.uni(-1.5f,1.5f);
    At<uint32>(m,0xC)=ident ? 0x20003u : uint32(r.pickI({3,2,1,0x20003}));
    At<uint32>(m,0x1C)=0; At<uint32>(m,0x2C)=0; At<uint32>(m,0x3C)=0x3F800000u;
    for(int c=0;c<3;++c)At<float>(m,0x30+4*c)=r.pct(5) ? r.uni(-500.f,500.f) : r.uni(-4.f,4.f);
    if(r.pct(8))At<float>(m,0x34)=0.f;
    At<void*>(f,4)=nullptr; }

// cadena de padres: front = 0 -> 2 -> 3 -> 4, rear = 1 -> 5 -> 6 -> 7; con ciclos hacia el propio nodo y/o cola compartida
inline void SetupFrames(Rng& r){
    for(auto& f:frames)FillFrameMatrix(f.b,r);
    auto P=[&](int i,int parent){ At<void*>(frames[i].b,4)=parent<0?nullptr:frames[parent].b; };
    const int A[4]{0,2,3,4}, B[4]{1,5,6,7};
    int lenA=int(r.below(4)), lenB=int(r.below(4));
    const bool aliasRear=r.pct(3);
    const bool eqY=r.pct(6);
    if(eqY){ lenA=lenB=0; }
    const bool cycleA=!eqY&&lenA>=1&&r.pct(20);
    const bool join=!eqY&&!cycleA&&lenB>=1&&r.pct(40);
    const bool cycleB=!eqY&&!join&&lenB>=1&&r.pct(20);
    for(int k=0;k<lenA;++k)P(A[k],A[k+1]);
    if(cycleA)P(A[lenA],0);
    for(int k=0;k<lenB;++k)P(B[k],B[k+1]);
    if(join)P(B[lenB],A[1+int(r.below(3))]);
    else if(cycleB)P(B[lenB],1);
    if(eqY)At<float>(frames[1].b,0x10+0x34)=At<float>(frames[0].b,0x10+0x34);
    nodeFront=frames[0].b; nodeRear=aliasRear ? frames[0].b : frames[1].b; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"vehctor62-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"vehctor62-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("vehctor62");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    InitModule();
    std::string only; { std::ifstream f(dir/"vehctor62-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,2> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // constructores de las bases en la DLL (primer llamado de la DLL dentro del constructor derivado, contrastado con el de la base)
    uintptr_t dllBikeCtor{}, dllAutoCtor{}, dllAutoSetup{};
    { const uintptr_t bmxCtor=FirstDllCall(DllBody("CBmx","Constructor")); const uintptr_t a=bmxCtor ? FirstDllCall(bmxCtor) : 0; const uintptr_t b=FirstDllCall(DllBody("CBike","Constructor"));
      if(!a||a!=b){ Write({{"event","error"},{"reason","CBike constructor not found"},{"from_bmx",a},{"from_wrapper",b}}); return; } dllBikeCtor=a; }
    { const uintptr_t quadCtor=FirstDllCall(DllBody("CQuadBike","Constructor")); const uintptr_t a=quadCtor ? FirstDllCall(quadCtor) : 0; const uintptr_t b=FirstDllCall(DllBody("CAutomobile","Constructor"));
      if(!a||a!=b){ Write({{"event","error"},{"reason","CAutomobile constructor not found"},{"from_quad",a},{"from_wrapper",b}}); return; } dllAutoCtor=a; }
    dllAutoSetup=DllBody("CAutomobile","SetupSuspensionLines");
    if(!dllAutoSetup){ Write({{"event","error"},{"reason","CAutomobile::SetupSuspensionLines body not found"}}); return; }
    // ningun doble puede coincidir con una direccion del propio banco
    if(dllBikeCtor==dllAutoCtor||dllBikeCtor==dllAutoSetup||dllAutoCtor==dllAutoSetup){ Write({{"event","error"},{"reason","base bodies overlap"}}); return; }

    std::array<bool,2> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    const uint32 numBlocks=std::min<uint32>(AtA<uint32>(0xB4EA30),180);
    if(numBlocks==0){ Write({{"event","error"},{"reason","no animation blocks loaded"}}); for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);} return; }

    // vtable falsa del model info y nombres
    for(auto& v:miVt)v=reinterpret_cast<void*>(&V_Trap); miVt[14]=reinterpret_cast<void*>(&V_AnimIdx);
    if(named.empty()){
        Nm(bmx.data(),bmx.size(),"obj"); Nm(quad.data(),quad.size(),"obj"); Nm(frames.data(),sizeof(frames),"frames"); Nm(mi.data(),mi.size(),"mi"); }

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { auto* B=reinterpret_cast<CBmx*>(bmx.data()); auto* Q=reinterpret_cast<CQuadBike*>(quad.data());
      auto Off=[&](const void* base,const void* m){ return uint32(static_cast<const uint8*>(m)-static_cast<const uint8*>(base)); };
      json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      Chk("CBmx.m_nVehicleSubType",Off(B,&B->m_nVehicleSubType),0x594); Chk("CBmx.m_aBikeNodes[4]",Off(B,B->m_aBikeNodes.data()+4),0x5B0); Chk("CBmx.m_aBikeNodes[5]",Off(B,B->m_aBikeNodes.data()+5),0x5B4);
      Chk("CBmx.m_RideAnimData",Off(B,&B->m_RideAnimData),0x640); Chk("CBmx.m_nFixLeftHand",Off(B,&B->m_nFixLeftHand),0x7B4); Chk("CBmx.m_nFixRightHand",Off(B,&B->m_nFixRightHand),0x7B5);
      Chk("CBmx.m_fControlJump",Off(B,&B->m_fControlJump),0x814); Chk("CBmx.m_fControlPedaling",Off(B,&B->m_fControlPedaling),0x818); Chk("CBmx.m_fSprintLeanAngle",Off(B,&B->m_fSprintLeanAngle),0x81C);
      Chk("CBmx.m_fCrankAngle",Off(B,&B->m_fCrankAngle),0x820); Chk("CBmx.m_fPedalAngleL",Off(B,&B->m_fPedalAngleL),0x824); Chk("CBmx.m_fPedalAngleR",Off(B,&B->m_fPedalAngleR),0x828);
      Chk("CBmx.m_fMidWheelDistY",Off(B,&B->m_fMidWheelDistY),0x82C); Chk("CBmx.m_fMidWheelFracY",Off(B,&B->m_fMidWheelFracY),0x830); Chk("CBmx.m_bIsFreewheeling",Off(B,&B->m_bIsFreewheeling),0x834);
      Chk("CQuadBike.m_nVehicleSubType",Off(Q,&Q->m_nVehicleSubType),0x594); Chk("CQuadBike.m_fSteerAngle",Off(Q,&Q->m_fSteerAngle),0x494); Chk("CQuadBike.m_pHandling",Off(Q,&Q->m_pHandling),0x988);
      Chk("CQuadBike.m_sRideAnimData",Off(Q,&Q->m_sRideAnimData),0x98C); Chk("CQuadBike.field_9A8",Off(Q,&Q->field_9A8),0x9A8); Chk("CQuadBike.m_nQuadFlags",Off(Q,&Q->m_nQuadFlags),0x9B8);
      Chk("sizeof(CBmx)",uint32(sizeof(CBmx)),uint32(kBmxSize)); Chk("sizeof(CQuadBike)",uint32(sizeof(CQuadBike)),uint32(kQuadSize));
      Chk("sizeof(tBikeHandlingData)",uint32(sizeof(tBikeHandlingData)),0x40);
      Chk("bikeHandlingBase-162*0x40",uint32(reinterpret_cast<uintptr_t>(gHandlingDataMgr.m_aBikeHandling.data())-162*0x40),0xC3491Cu);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado: model infos 15..18, bloques de animacion
    if(regions.empty()){ Snap(0xA9B0C8+4*15,16); Snap(0xB5D4A0,180*0x20); }

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        const bool isBmx=cls=="CBmx";
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x62C7u; if(!stt)stt=1; Rng r{stt};
                trace=json::array();
                hooks[f]->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                json out={{"switches",switches}};
                Patches P;
                // dobles de la base (EXE + constructor de la DLL)
                if(isBmx){ P.Add(0x6BF430,S_BikeCtor); P.Add(dllBikeCtor,S_BikeCtor); }
                else { P.Add(0x6B0A90,S_AutoCtor); P.Add(dllAutoCtor,S_AutoCtor); P.Add(0x6A65D0,S_AutoSetup); P.Add(dllAutoSetup,S_AutoSetup); }

                // modelo falso en una de las posiciones 15..18
                const int32 idx=int32(15+r.below(4)); const uint32 createdBy=r.below(5);
                for(auto& b:mi)b=uint8(r.next());
                At<void*>(mi.data(),0)=miVt.data(); AtA<void*>(0xA9B0C8+4*idx)=mi.data();
                baseSeed=r.next()|1u;
                uint8* obj=isBmx ? bmx.data() : quad.data(); const size_t size=isBmx ? kBmxSize : kQuadSize; const size_t total=size+kGuard;
                std::memset(obj,0xCD,total); curObj=obj; curSize=size;
                void* ret{};
                if(isBmx){
                    animIdx=int32(r.below(numBlocks));
                    // GroupId del bloque de animacion elegido (fuera de 7..9 el constructor lo sustituye por 7)
                    const int32 gid=r.pct(60) ? int32(r.below(18))-3 : r.pickI({-1,0,6,7,8,9,10,11,0x7FFFFFFF,int32(0x80000000),int32(0xFFFFFFF0),1000});
                    AtA<int32>(0xB5D4A0+0x20*uint32(animIdx)+0x1C)=gid;
                    SetupFrames(r);
                    ret=reinterpret_cast<void*(__thiscall*)(void*,int32,uint32)>(fn.address)(obj,idx,createdBy);
                    out["gid"]=gid; out["frames"]=Fnv(frames.data(),sizeof(frames));
                } else {
                    // m_nHandlingId (uint16 en +0x4A): el EXE usa solo el byte bajo
                    const uint32 pk=r.below(10);
                    const uint16 hid=pk<4 ? uint16(r.below(256)) : pk<7 ? uint16(162+r.below(14)) : uint16(r.next());
                    At<uint16>(mi.data(),0x4A)=hid;
                    ret=reinterpret_cast<void*(__thiscall*)(void*,int32,uint32)>(fn.address)(obj,idx,createdBy);
                    out["hid"]=hid;
                }
                out["ret_is_this"]=(ret==obj);
                const uint32 vp=At<uint32>(obj,0); const uint32 vtExe=isBmx ? 0x871528u : 0x871AE8u;
                out["vptr_ok"]=(vp==vtExe)||InDll(vp);
                At<uint32>(obj,0)=0;                               // vptr normalizada
                out["obj"]=ReversingProbe::Bytes(obj,size);
                out["guard"]=Fnv(obj+size,kGuard);
                out["trace"]=trace; return out;
            };
            const auto original=run(false), candidate=run(true);
            RestoreRegions();
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["vptr_ok"]==true&&original["ret_is_this"]==true&&original["guard"]==candidate["guard"];
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",RegionsRestored()}});
}
}
