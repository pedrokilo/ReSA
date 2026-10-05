#pragma once
// Banco dentro del motor: CPhysical (56 funciones de la lista de 34; Add y RemoveAndAdd tienen casos propios desde el
// 05/10/2026, con la tecnica de UnlockPhysical: rectangulo de limites inyectado con valores en los bordes de sector y
// listas de repeticion/nodos reales). Original (hooks de CPhysical desactivados) frente a candidato con fixtures
// deterministas por caso.
// Fisicos FALSOS con desplazamientos crudos (8 bloques de 0xA20 bytes: vehiculos, peatones, objetos y edificios) con
// matriz, masas, velocidades, banderas, registros de colision y datos derivados (tipo/subtipo de vehiculo, alarma,
// estado y banderas de peaton, banderas y CObjectData de objeto). Todos comparten una vtable FALSA cuyas ranuras
// usadas (SetIsStatic, GetBoundRect, SpecialEntityPreCollisionStuff, SpecialEntityCalcCollisionSteps,
// ProcessEntityCollision, AddMovingCollisionSpeed, GetHeightAboveRoad) son stubs trazados; el resto aborta.
// Model info y modelo de colision FALSOS (indice 1 y los indices especiales que el codigo compara: bola blanca,
// boca de incendios, parquimetros, montacargas, dumper y satchel).
// Mundo FALSO: 3x3 sectores (109..111) y sus celdas de repeticion se vacian y se rellenan con listas construidas con
// pools de nodos LOCALES (simple, doble y CEntryInfoNode) y pools de entidades FALSOS (vehiculos, peatones, objetos y
// edificios cubren el bloque de entidades: la DLL de depuracion valida nodos e Items contra esos pools).
// Se simulan y trazan: rand (EXE, CRT de la DLL y CGeneral::GetRandomNumber), CCollision::ProcessColModels,
// AudioEngine (ReportCollision y ReportMissionAudioEvent), CObject::ObjectDamage, CGlass (IsObjectGlass y las dos
// respuestas de ventana), g_fx (TriggerWaterHydrant y AddSparks), CPickups::CreateSomeMoney, CStats::IncrementStat,
// FindPlayerPed/FindPlayerVehicle, CCullZones::DoExtraAirResistanceForPlayer, CWorld::Add/Remove/ClearScanCodes,
// CCarCtrl::SwitchVehicleToRealPhysics/FindGhostRoadHeight, CPed::KillPedWithCar, CPedIntelligence::GetTaskClimb,
// CBike::CalculateLeanMatrix, CEntity::Add/CEntity::Remove (edificios grandes), y para constructor/destructor
// CPlaceable::AllocateStaticMatrix, CRealTimeShadowManager::ReturnRealTimeShadow, CEntryInfoList::Flush y ~CEntity.
// Estado global que se toca y se restaura en cada ejecucion: paso de tiempo, reloj, CGame::currArea, jugador en foco
// y CPlayerInfo, bNoMoreCollisionTorque, bSecondShift, mesa de billar, codigo de busqueda, los tres arrays de
// CColPoint estaticos (y sus guardas), la lista de entidades en movimiento, los punteros de los pools, las entradas
// de ms_modelInfoPtrs, CReferences (aRefs + pEmptyList) y el bloque de sectores.
// Fuera de la comparacion: el byte 0xBA del constructor (NOTSA conservado: la DLL inicializa m_nContactSurface) y los
// punteros de vtable de constructor y destructor (cada lado instala la suya).
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Physical.h"
#include "Automobile.h"
#include "Bike.h"
#include "Object.h"
#include "ObjectData.h"
#include "Ped.h"
#include "PlayerPedData.h"
#include "PedIntelligence.h"
#include "References.h"
#include "Glass.h"
#include "Pickups.h"
#include "Stats.h"
#include "CullZones.h"
#include "CarCtrl.h"
#include "RealTimeShadowManager.h"
#include "Collision/Collision.h"
#include "Collision/CollisionData.h"
#include "Fx/Fx.h"
#include "Pools/EntryInfoNodePool.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "../ProbeRuntime.hpp"

namespace Physical56Live {
inline bool done{}; inline std::ofstream log; inline json trace; inline DWORD mainThread{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T> inline T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline std::string Hex(const void* p,size_t n) { std::string s; for(size_t k=0;k<n;++k)s+=std::format("{:02x}",static_cast<const uint8*>(p)[k]); return s; }
inline std::string Hex80(const uint8* p) { std::string s; for(size_t k=10;k-->0;)s+=std::format("{:02x}",p[k]); return s; }

// --- direcciones del EXE ---
constexpr uintptr_t VT_PHYS=0x863BA0, TIMESTEP=0xB7CB5C, TIMEMS=0xB7CB84, CURR_AREA=0xB72914, PLAYER_FOCUS=0xB7CD74, PLAYERS=0xB7CD98,
    NO_TORQUE=0xB7CD72, SECOND_SHIFT=0xB7CD6C, SNOOKER=0x8CDEF4, SCAN_CODE=0xB7CD78, CPS_A=0xB73710, CPS_A_GUARD=0xB73C90, CPS_B=0xB73C98,
    CPS_B_GUARD=0xB74218, TEMP_CPS=0xB9ACD0, MOVING_LIST=0xB9ACC8, POOLS=0xB74484, MI_PTRS=0xA9B0C8, SECTORS=0xB7D0B8, REPEAT=0xB992B8,
    MI_CUE=0x8CD778, MI_HYDRANT=0x8CD594, MI_METER1=0x8CD548, MI_METER2=0x8CD54C;
constexpr size_t PLAYERS_SIZE=0x190*2, CPS_SIZE=0x2C*32, POOLS_SIZE=0x1C;
constexpr size_t ES=0xA20; constexpr int NE=8, SEC_LO=109, SEC_HI=111, NVT=80;
constexpr size_t KNOB=0xA18; // bytes 0xA18..0xA1F: mandos del fixture fuera de cualquier clase (CHeli mide 0xA18)
constexpr int NDBL=256, NSGL=64, NENTRY=128;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,ES*NE> entMem{};
inline uint8* E(int k) { return entMem.data()+size_t(k)*ES; }
alignas(16) inline std::array<std::array<uint8,0x48>,NE+2> mats{};
alignas(16) inline std::array<std::array<uint8,0x50>,NE> objInfo{};
alignas(16) inline std::array<uint8,0xAC> plrData{};
alignas(16) inline std::array<uint8,0x400> miBuf{}; alignas(16) inline std::array<uint8,0x30> colBuf{}; alignas(16) inline std::array<uint8,0x30> colData{};
alignas(16) inline std::array<uint8,0x20*4> colLines{};
alignas(16) inline std::array<uint8,0x2C*2> cpBuf{}; alignas(16) inline std::array<uint8,0x2C*32> cpArr{};
alignas(16) inline std::array<uint8,0x40> misc{};   // salidas por referencia (floats, vectores, CRect, cuaternion)
alignas(16) inline std::array<uint8,0x10> shadowBuf{};
inline std::array<void*,NVT> fvt{}; inline std::array<void*,32> miVt{};
// pools de nodos LOCALES (cabecera construida una vez; almacenamiento y estados se reinician en cada ejecucion)
alignas(16) inline std::array<uint8,NDBL*12> dblStore{}; inline std::array<uint8,NDBL> dblStates{};
alignas(16) inline std::array<uint8,NSGL*8> sglStore{}; inline std::array<uint8,NSGL> sglStates{};
alignas(16) inline std::array<uint8,NENTRY*20> entryStore{}; inline std::array<uint8,NENTRY> entryStates{};
alignas(16) inline std::array<uint8,0x20> dblHdr{}, sglHdr{}, entryHdr{}, dblImg{}, sglImg{}, entryImg{};
// pools de entidades FALSOS que cubren entMem (todas las ranuras ocupadas)
alignas(16) inline std::array<uint8,0x14> vehHdr{}, pedHdr{}, objHdr{}, bldHdr{};
inline std::array<uint8,ES*NE/0x38+1> entStates{};

inline std::string Name(const void* p) {
    const auto u=uintptr_t(p); if(!u)return "null";
    struct R { uintptr_t base; size_t size, stride; const char* tag; };
    const R regions[]{
        {uintptr_t(entMem.data()),entMem.size(),ES,"e"},{uintptr_t(mats.data()),sizeof(mats),0x48,"m"},{uintptr_t(objInfo.data()),sizeof(objInfo),0x50,"oi"},
        {uintptr_t(plrData.data()),plrData.size(),plrData.size(),"plr"},{uintptr_t(miBuf.data()),miBuf.size(),miBuf.size(),"mi"},{uintptr_t(colBuf.data()),colBuf.size(),colBuf.size(),"col"},
        {uintptr_t(colData.data()),colData.size(),colData.size(),"cd"},{uintptr_t(cpBuf.data()),cpBuf.size(),0x2C,"cp"},{uintptr_t(cpArr.data()),cpArr.size(),0x2C,"cpa"},
        {uintptr_t(misc.data()),misc.size(),misc.size(),"misc"},{uintptr_t(shadowBuf.data()),shadowBuf.size(),shadowBuf.size(),"shadow"},
        {CPS_A,CPS_SIZE,0x2C,"cpsA"},{CPS_B,CPS_SIZE,0x2C,"cpsB"},{TEMP_CPS,CPS_SIZE,0x2C,"tmpcp"},{0xB73720,4,4,"fxdir"},
        {uintptr_t(dblStore.data()),dblStore.size(),12,"dn"},{uintptr_t(sglStore.data()),sglStore.size(),8,"sn"},{uintptr_t(entryStore.data()),entryStore.size(),20,"en"},
        {REPEAT,16*16*12,12,"rs"},{SECTORS,120*120*8,8,"sec"},{MOVING_LIST,4,4,"moving"}};
    for(auto& r:regions)if(u>=r.base&&u<r.base+r.size)return std::format("{}#{}+{:x}",r.tag,(u-r.base)/r.stride,(u-r.base)%r.stride);
    const auto* tib=reinterpret_cast<NT_TIB*>(NtCurrentTeb()); if(u>=uintptr_t(tib->StackLimit)&&u<uintptr_t(tib->StackBase))return "stack";
    return std::format("{:08x}",u);
}

// --- generador de los stubs (mismo estado al empezar cada ejecucion) ---
inline uint32 sst{};
inline uint32 SNext() { sst^=sst<<13; sst^=sst>>17; sst^=sst<<5; return sst; }
inline float SUni(float a,float b) { return a+(b-a)*float(SNext()%1000000)/1000000.f; }
inline bool OnMain() { return GetCurrentThreadId()==mainThread; }
inline void T(json j) { if(OnMain())trace.push_back(std::move(j)); }
inline json V(const CVector* v) { return v ? json{F(v->x),F(v->y),F(v->z)} : json(nullptr); }

// --- dependencias simuladas ---
inline int __cdecl FakeRand() { if(!OnMain())return 0; const int r=int(SNext()>>9)&0x7FFF; T({"rand",r}); return r; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }
inline void __fastcall FakeReportCollision(void*,void*,CEntity* a,CEntity* b,uint32 s1,uint32 s2,const CVector& pos,const CVector* n,float i1,float i2,uint32 b1,uint32 b2) {
    T({"report_collision",Name(a),Name(b),s1&0xFF,s2&0xFF,V(&pos),V(n),F(i1),F(i2),b1&0xFF,b2&0xFF}); }
inline void __fastcall FakeMissionAudio(void*,void*,uint32 id,void* obj) { T({"mission_audio",id&0xFFFF,Name(obj)}); }
inline void __fastcall FakeObjectDamage(void* obj,void*,float d,const CVector* o,const CVector* dir,CEntity* dmg,uint32 w) { T({"object_damage",Name(obj),F(d),V(o),V(dir),Name(dmg),w}); }
inline bool __cdecl FakeIsObjectGlass(uint8* e) { const bool r=e&&(At<uint8>(e,KNOB)&1); T({"is_glass",Name(e),r}); return r; }
inline void __cdecl FakeWindowCol(CEntity* e,float d,CVector ms,CVector pt,uint32 b) { T({"window_col",Name(e),F(d),V(&ms),V(&pt),b&0xFF}); }
inline void __cdecl FakeWindowSoft(CEntity* e,float d) { T({"window_soft",Name(e),F(d)}); }
inline void __fastcall FakeHydrant(void*,void*,const CVector& p) { T({"hydrant",V(&p)}); }
inline void __fastcall FakeSparks(void*,void*,const CVector& o,const CVector& d,float f,int32 n,CVector ac,uint32 type,float spread,float life) { T({"sparks",V(&o),V(&d),F(f),n,V(&ac),type,F(spread),F(life)}); }
inline void __cdecl FakeMoney(CVector p,int32 n) { T({"money",V(&p),n}); }
inline void __cdecl FakeStat(uint32 st,float v) { T({"stat",st&0xFFFF,F(v)}); }
inline void* fakePed{}; inline void* fakeVeh{}; inline bool extraAir{};
inline CPlayerPed* __cdecl FakeFindPlayerPed(int32 id) { T({"find_player_ped",id}); return static_cast<CPlayerPed*>(fakePed); }
inline CVehicle* __cdecl FakeFindPlayerVehicle(int32 id,bool r) { T({"find_player_vehicle",id,r}); return static_cast<CVehicle*>(fakeVeh); }
inline bool __cdecl FakeExtraAir() { T({"extra_air",extraAir}); return extraAir; }
inline void __cdecl FakeWorldAdd(CEntity* e) { T({"world_add",Name(e)}); }
inline void __cdecl FakeWorldRemove(CEntity* e) { T({"world_remove",Name(e)}); }
inline void __cdecl FakeClearScanCodes() { T({"clear_scan_codes"}); }
inline void __cdecl FakeSwitchReal(CVehicle* v) { T({"switch_real_physics",Name(v)}); }
inline float ghostHeight{};
inline float __cdecl FakeGhostHeight(CVehicle* v) { T({"ghost_height",Name(v)}); return ghostHeight; }
inline void __fastcall FakeKillPed(void* ped,void*,void* car,float d,uint32 b) { T({"kill_ped_with_car",Name(ped),Name(car),F(d),b&0xFF}); }
inline void* __fastcall FakeTaskClimb(void* intel,void*) { T({"task_climb",Name(intel)}); return nullptr; }
inline void __fastcall FakeLean(void* bike,void*) { T({"lean_matrix",Name(bike)}); }
inline void __fastcall FakeEntityRemove(void* e,void*) { T({"entity_remove",Name(e)}); }
inline void __fastcall FakeEntityAdd(void* e,void*) { T({"entity_add",Name(e)}); }
inline void __fastcall FakeReturnShadow(void*,void*,void* s) { T({"return_shadow",Name(s)}); }
inline void __fastcall FakeFlush(void* l,void*) { T({"flush",Name(l)}); }
inline void __fastcall FakeEntityDtor(void* e,void*) { T({"entity_dtor",Name(e)}); }
/* El CColPoint local de ApplyFriction()/ApplySpeed en el EXE solo tiene punto y normal: sus superficies (+0x20/+0x23) son pila sin
   inicializar (los 12 bytes justo bajo la direccion de retorno de ApplySpeed) y la ruta de chispas las lee. Se pone a cero la pila
   bajo la llamada para que el EXE lea ceros como el `{}` de la DLL. Naked y sin marco propio: una version en C++ dejaba sin poner a
   cero el tope de su propio marco (ebp guardado, cookie, relleno), que es justo donde cae ese CColPoint. */
__declspec(naked) inline void ScrubStack() {
    __asm {
        mov edx, edi
        lea edi, [esp-4000h]
        mov ecx, 1000h
        xor eax, eax
        rep stosd
        mov edi, edx
        ret
    }
}
inline void __fastcall FakeAllocStatic(void* e,void*) { T({"alloc_static_matrix",Name(e)}); At<void*>(e,0x14)=mats[NE].data(); }
// CCollision::ProcessColModels: rellena n puntos de colision deterministas
inline void FillColPoint(uint8* c,const CVector& at) {
    std::memset(c,0,0x2C); At<CVector>(c,0)=at+CVector{SUni(-1.f,1.f),SUni(-1.f,1.f),SUni(-0.5f,0.5f)};
    CVector n{SUni(-1.f,1.f),SUni(-1.f,1.f),SUni(-1.f,1.f)}; if(SNext()%4==0)n=CVector{0.f,0.f,1.f}; else if(SNext()%6==0)n=CVector{0.f,0.f,-1.f};
    const float m=std::sqrt(n.x*n.x+n.y*n.y+n.z*n.z); if(m>0.01f)n=CVector{n.x/m,n.y/m,n.z/m}; else n=CVector{0.f,0.f,1.f};
    At<CVector>(c,0x10)=n;
    static const uint8 surf[]{0,1,4,9,10,16,19,40,63,65,70,74,100,150,178};
    static const uint8 piece[]{0,0,0,3,4,13,14,15,16};
    c[0x20]=surf[SNext()%std::size(surf)]; c[0x21]=piece[SNext()%std::size(piece)]; c[0x22]=uint8(SNext());
    c[0x23]=surf[SNext()%std::size(surf)]; c[0x24]=piece[SNext()%std::size(piece)]; c[0x25]=uint8(SNext());
    At<float>(c,0x28)=SNext()%5==0 ? 0.f : SUni(0.f,0.6f);
}
inline int32 __cdecl FakeColModels(const CMatrix& ma,CColModel& ca,const CMatrix& mb,CColModel& cb,CColPoint* cps,CColPoint* lines,float* ratios,bool b) {
    const int32 n=SNext()%3==0 ? 0 : int32(1+SNext()%4);
    const CVector at=(ma.GetPosition()+mb.GetPosition())*0.5f;
    for(int32 i=0;i<n;++i)FillColPoint(reinterpret_cast<uint8*>(cps)+i*0x2C,at);
    T({"col_models",Name(&ma),Name(&ca),Name(&mb),Name(&cb),Name(cps),Name(lines),Name(ratios),b,n}); return n;
}

// --- vtable FALSA de las entidades ---
template<int K> void __fastcall VTrap(void* self,void*) {
    Write({{"event","error"},{"reason","unexpected virtual call"},{"slot",K},{"self",Name(self)}}); log.flush(); ExitProcess(95);
}
template<int... K> std::array<void*,sizeof...(K)> TrapTable(std::integer_sequence<int,K...>) { return {reinterpret_cast<void*>(&VTrap<K>)...}; }
inline void __fastcall VSetIsStatic(uint8* e,void*,uint32 s) { T({"set_is_static",Name(e),s&0xFF}); At<uint32>(e,0x1C)=(At<uint32>(e,0x1C)&~4u)|((s&0xFF) ? 4u : 0u); }
inline bool boundsOn{}; inline float bnd[4]{}; // Add/RemoveAndAdd: rectangulo de limites inyectado (izq, arriba, der, abajo en memoria)
inline float* __fastcall VGetBoundRect(uint8* e,void*,float* out) {
    if(boundsOn){ out[0]=bnd[0]; out[1]=bnd[1]; out[2]=bnd[2]; out[3]=bnd[3]; T({"bound_rect",Name(e)}); return out; }
    const auto& p=At<CVector>(At<uint8*>(e,0x14),0x30); const float r=1.f+float(At<uint8>(e,KNOB+1)%5);
    out[0]=p.x-r; out[1]=p.y+r; out[2]=p.x+r; out[3]=p.y-r; T({"bound_rect",Name(e)}); return out; }
inline void __fastcall VPreCol(uint8* e,void*,CPhysical* other,uint32 shift,bool& skipAll,bool& skipCol,bool& forceBld,bool& forceSoft) {
    const uint32 r=SNext(); if(r%9==0)skipAll=true; if((r>>4)%7==0)skipCol=true; if((r>>8)%6==0)forceBld=true; if((r>>12)%6==0)forceSoft=true;
    T({"pre_col",Name(e),Name(other),shift&0xFF,skipAll,skipCol,forceBld,forceSoft}); }
inline uint8 __fastcall VCalcSteps(uint8* e,void*,bool& full,bool& half) {
    full=SNext()%4==0; half=SNext()%4==0; const uint8 n=uint8(1+SNext()%3); T({"calc_steps",Name(e),full,half,n}); return n; }
inline int32 __fastcall VProcessEntityCollision(uint8* e,void*,uint8* other,CColPoint* cps) {
    const int32 n=SNext()%3==0 ? 0 : int32(1+SNext()%3);
    const CVector at=(At<CVector>(At<uint8*>(e,0x14),0x30)+At<CVector>(At<uint8*>(other,0x14),0x30))*0.5f;
    for(int32 i=0;i<n;++i)FillColPoint(reinterpret_cast<uint8*>(cps)+i*0x2C,at);
    T({"process_entity_collision",Name(e),Name(other),Name(cps),n}); return n; }
inline CVector* __fastcall VMovingColSpeed(uint8* e,void*,CVector* out,CVector& pt) {
    *out=CVector{SUni(-0.2f,0.2f),SUni(-0.2f,0.2f),SUni(-0.05f,0.05f)}; T({"moving_col_speed",Name(e),V(&pt)}); return out; }
inline float __fastcall VHeightAboveRoad(uint8* e,void*) { T({"height_above_road",Name(e)}); return 0.5f+float(At<uint8>(e,KNOB+2)%8)*0.125f; }
inline void* __fastcall MiAsAtomic(uint8* mi,void*) { const bool r=At<uint8>(mi,0x3FF)&1; T({"as_atomic",r}); return r ? mi : nullptr; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=56;
inline const std::array<Fn,NFNS> fns{{
    {"CPhysical","ApplyMoveForce-vec",0x5429F0,0,-1,256},{"CPhysical","ApplyTurnForce",0x542A50,0,-1,256},
    {"CPhysical","ApplyForce",0x542B50,0,-1,256},{"CPhysical","GetSpeed",0x542CE0,0,-1,256},
    {"CPhysical","ApplyMoveSpeed",0x542DD0,0,-1,256},{"CPhysical","ApplyTurnSpeed",0x542E20,0,-1,256},
    {"CPhysical","ApplyGravity",0x542FE0,0,-1,256},{"CPhysical","ApplyFrictionMoveForce",0x5430A0,0,-1,256},
    {"CPhysical","ApplyFrictionForce",0x543220,0,-1,256},{"CPhysical","SkipPhysics",0x5433B0,0,-1,256},
    {"CPhysical","AddCollisionRecord",0x543490,0,-1,256},{"CPhysical","GetHasCollidedWith",0x543540,0,-1,256},
    {"CPhysical","GetHasCollidedWithAnyObject",0x543580,0,-1,256},{"CPhysical","ApplySpringCollision",0x543C90,0,-1,256},
    {"CPhysical","ApplySpringCollisionAlt",0x543D60,0,-1,256},{"CPhysical","ApplySpringDampening",0x543E90,0,-1,256},
    {"CPhysical","RemoveRefsToEntity",0x544280,0,-1,256},{"CPhysical","GetLightingFromCol",0x5447B0,0,-1,256},
    {"CPhysical","GetLightingTotal",0x544850,0,-1,256},{"CPhysical","CanPhysicalBeDamaged",0x5448B0,0,-1,256},
    {"CPhysical","ApplyAirResistance",0x544C40,0,-1,256},{"CPhysical","SetDamagedPieceRecord",0x5428C0,0,-1,256},
    {"CPhysical","ApplyCollision-1",0x5435C0,0,-1,512},{"CPhysical","ApplySoftCollision-1",0x543890,0,-1,512},
    {"CPhysical","ApplyCollisionAlt",0x544D50,0,-1,1024},{"CPhysical","ApplyFriction-self",0x5454C0,0,-1,512},
    {"CPhysical","ApplyFriction-other",0x545980,0,-1,512},{"CPhysical","ApplyFriction-void",0x5483D0,0,-1,512},
    {"CPhysical","ApplySpeed",0x547B80,0,-1,512},{"CPhysical","UnsetIsInSafePosition",0x548320,0,-1,256},
    {"CPhysical","ProcessControl",0,0x863BA0,10,512},{"CPhysical","ApplyCollision-2",0x548680,0,-1,1024},
    {"CPhysical","ApplySoftCollision-2",0x54A2C0,0,-1,1024},{"CPhysical","GetBoundRect",0,0x863BA0,9,256},
    {"CPhysical","Constructor",0x542260,0,-1,128},{"CPhysical","Destructor",0x542450,0,-1,128},
    {"CPhysical","Remove",0,0x863BA0,3,256},{"CPhysical","AddToMovingList",0x542800,0,-1,256},
    {"CPhysical","RemoveFromMovingList",0x542860,0,-1,256},{"CPhysical","DettachEntityFromEntity",0x5442F0,0,-1,512},
    {"CPhysical","DettachAutoAttachedEntity",0x5446A0,0,-1,256},{"CPhysical","AttachEntityToEntity-vec",0x54D570,0,-1,512},
    {"CPhysical","AttachEntityToEntity-quat",0x54D690,0,-1,512},{"CPhysical","PositionAttachedEntity",0x546FF0,0,-1,1024},
    {"CPhysical","PlacePhysicalRelativeToOtherPhysical",0x546DB0,0,-1,256},{"CPhysical","ProcessEntityCollision",0,0x863BA0,22,512},
    {"CPhysical","CheckCollision",0x54D920,0,-1,512},{"CPhysical","CheckCollision_SimpleCar",0x54DAB0,0,-1,256},
    {"CPhysical","ProcessCollisionSectorList",0x54BA60,0,-1,1024},{"CPhysical","ProcessCollisionSectorList_SimpleCar",0x54CFF0,0,-1,512},
    {"CPhysical","ProcessShiftSectorList",0x546670,0,-1,1024},{"CPhysical","TestCollision",0,0x863BA0,13,512},
    {"CPhysical","ProcessCollision",0,0x863BA0,11,512},{"CPhysical","ProcessShift",0,0x863BA0,12,512},
    // Add (virtual, ranura 2) y RemoveAndAdd: casos propios, con la tecnica de UnlockPhysical
    {"CPhysical","Add",0,0x863BA0,2,512},{"CPhysical","RemoveAndAdd",0x542560,0,-1,1024},
}};

// --- llamadas que devuelven un float en ST0 (se guardan los 80 bits) ---
inline std::string CallThisF0(uintptr_t fnAddr,void* self) {
    uint8 raw[10]{};
    __asm { mov ecx, self
            mov eax, fnAddr
            call eax
            fstp tbyte ptr raw }
    return Hex80(raw);
}
inline std::string CallThisF1(uintptr_t fnAddr,void* self,uint32 arg) {
    uint8 raw[10]{};
    __asm { push arg
            mov ecx, self
            mov eax, fnAddr
            call eax
            fstp tbyte ptr raw }
    return Hex80(raw);
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"physical56-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"physical56-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("physical56");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainThread=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"physical56-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    { int freeRefs=0; for(auto* r=CReferences::pEmptyList;r&&freeRefs<1000;r=r->m_pNext)++freeRefs;
      if(freeRefs<200){Write({{"event","error"},{"reason","not enough free references"},{"free",freeRefs}});return;} }

    // --- cuerpos de la DLL exportados (se resuelven ANTES de escribir ningun parche) ---
    HMODULE module{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&module);
    auto Local=[&](const char* name)->uintptr_t{ auto p=reinterpret_cast<uintptr_t>(GetProcAddress(module,name)); if(!p)return 0; if(*reinterpret_cast<uint8*>(p)==0xE9)p+=5+*reinterpret_cast<int32*>(p+1); return p; };
    const uintptr_t dllEntityDtor=Local("??1CEntity@@UAE@XZ"), dllEntityRemove=Local("?Remove@CEntity@@UAEXXZ"), dllEntityAdd=Local("?Add@CEntity@@UAEXXZ");
    if(!dllEntityDtor||!dllEntityRemove||!dllEntityAdd){Write({{"event","error"},{"reason","dll export not found"}});return;}
    const uintptr_t allocStatic=std::bit_cast<uintptr_t>(&CPlaceable::AllocateStaticMatrix), flushFn=std::bit_cast<uintptr_t>(&CEntryInfoList::Flush),
                    returnShadow=std::bit_cast<uintptr_t>(&CRealTimeShadowManager::ReturnRealTimeShadow);

    // --- tablas falsas ---
    { const auto traps=TrapTable(std::make_integer_sequence<int,NVT>{}); for(int k=0;k<NVT;++k)fvt[k]=traps[k];
      fvt[4]=reinterpret_cast<void*>(&VSetIsStatic); fvt[9]=reinterpret_cast<void*>(&VGetBoundRect); fvt[15]=reinterpret_cast<void*>(&VPreCol);
      fvt[16]=reinterpret_cast<void*>(&VCalcSteps); fvt[22]=reinterpret_cast<void*>(&VProcessEntityCollision); fvt[49]=reinterpret_cast<void*>(&VMovingColSpeed);
      fvt[53]=reinterpret_cast<void*>(&VHeightAboveRoad);
      for(int k=0;k<32;++k)miVt[k]=traps[k]; miVt[1]=reinterpret_cast<void*>(&MiAsAtomic); }

    // --- estado global guardado ---
    struct Blob { uintptr_t at; std::vector<uint8> data; };
    std::vector<Blob> blobs;
    auto Save=[&](uintptr_t at,size_t n){ blobs.push_back({at,std::vector<uint8>(reinterpret_cast<uint8*>(at),reinterpret_cast<uint8*>(at)+n)}); };
    Save(TIMESTEP,4); Save(TIMEMS,4); Save(CURR_AREA,4); Save(PLAYER_FOCUS,1); Save(PLAYERS,PLAYERS_SIZE); Save(NO_TORQUE,1); Save(SECOND_SHIFT,1);
    Save(SNOOKER,24); Save(SCAN_CODE,2); Save(CPS_A,CPS_SIZE); Save(CPS_A_GUARD,4); Save(CPS_B,CPS_SIZE); Save(CPS_B_GUARD,4); Save(TEMP_CPS,CPS_SIZE);
    Save(MOVING_LIST,4); Save(POOLS,POOLS_SIZE); Save(uintptr_t(&CReferences::aRefs[0]),sizeof(CReferences::aRefs)); Save(uintptr_t(&CReferences::pEmptyList),4);
    Save(REPEAT,16*16*12);
    for(int y=SEC_LO;y<=SEC_HI;++y)Save(SECTORS+(size_t(y)*120+SEC_LO)*8,size_t(SEC_HI-SEC_LO+1)*8);
    // indices de modelo que el codigo compara (variables del EXE) y constantes
    std::vector<int32> models{1};
    for(uintptr_t v:{MI_CUE,MI_HYDRANT,MI_METER1,MI_METER2}){ const int32 m=*reinterpret_cast<int16*>(v); if(m>1&&m<20000)models.push_back(m); }
    for(int32 m:{530,406,363})models.push_back(m);
    std::sort(models.begin(),models.end()); models.erase(std::unique(models.begin(),models.end()),models.end());
    for(int32 m:models)Save(MI_PTRS+size_t(m)*4,4);
    auto RestoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size()); };
    auto Restored=[&]{ bool ok=true; for(auto& b:blobs)ok&=std::memcmp(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size())==0; return ok; };

    // pools de nodos locales: cabecera construida una sola vez
    new (dblHdr.data()) CPtrNodeDoubleLinkPool(NDBL,dblStore.data(),dblStates.data()); dblImg=dblHdr;
    new (sglHdr.data()) CPtrNodeSingleLinkPool(NSGL,sglStore.data(),sglStates.data()); sglImg=sglHdr;
    new (entryHdr.data()) CEntryInfoNodePool(NENTRY,entryStore.data(),entryStates.data()); entryImg=entryHdr;
    auto PoolHdr=[&](std::array<uint8,0x14>& h,size_t stride){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=entMem.data(); At<void*>(h.data(),4)=entStates.data();
                                                             At<int32>(h.data(),8)=int32(entMem.size()/stride); At<int32>(h.data(),0xC)=-1; };
    // la capacidad se calcula con el paso mas pequeño (0x38): cubre todo entMem sea cual sea el tamaño real del tipo almacenado
    PoolHdr(vehHdr,0x38); PoolHdr(pedHdr,0x38); PoolHdr(objHdr,0x38); PoolHdr(bldHdr,0x38);
    auto* const poolPtrs=reinterpret_cast<void**>(POOLS); // 0 simple, 1 doble, 2 entry, 3 peatones, 4 vehiculos, 5 edificios, 6 objetos

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch r0(bit_cast<uintptr_t>(&rand),FakeRand), r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch a1(bit_cast<uintptr_t>(&CAudioEngine::ReportCollision),FakeReportCollision), a1e(0x506EB0,FakeReportCollision);
        EntryPatch a2(bit_cast<uintptr_t>(static_cast<void(CAudioEngine::*)(uint16,CObject*)>(&CAudioEngine::ReportMissionAudioEvent)),FakeMissionAudio), a2e(0x507350,FakeMissionAudio);
        EntryPatch o1(bit_cast<uintptr_t>(&CObject::ObjectDamage),FakeObjectDamage), o1e(0x5A0D90,FakeObjectDamage);
        EntryPatch g1(bit_cast<uintptr_t>(&CGlass::IsObjectGlass),FakeIsObjectGlass), g1e(0x46A760,FakeIsObjectGlass);
        EntryPatch g2(bit_cast<uintptr_t>(&CGlass::WindowRespondsToCollision),FakeWindowCol), g2e(0x71BC40,FakeWindowCol);
        EntryPatch g3(bit_cast<uintptr_t>(&CGlass::WindowRespondsToSoftCollision),FakeWindowSoft), g3e(0x71AF70,FakeWindowSoft);
        EntryPatch x1(bit_cast<uintptr_t>(&Fx_c::TriggerWaterHydrant),FakeHydrant), x1e(0x4A0D70,FakeHydrant);
        EntryPatch x2(bit_cast<uintptr_t>(&Fx_c::AddSparks),FakeSparks), x2e(0x49F040,FakeSparks);
        EntryPatch k1(bit_cast<uintptr_t>(&CPickups::CreateSomeMoney),FakeMoney), k1e(0x458970,FakeMoney);
        EntryPatch k2(bit_cast<uintptr_t>(&CStats::IncrementStat),FakeStat), k2e(0x55C180,FakeStat);
        EntryPatch p1(bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),FakeFindPlayerPed), p1e(0x56E210,FakeFindPlayerPed);
        EntryPatch p2(bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),FakeFindPlayerVehicle), p2e(0x56E0D0,FakeFindPlayerVehicle);
        EntryPatch c1(bit_cast<uintptr_t>(&CCullZones::DoExtraAirResistanceForPlayer),FakeExtraAir), c1e(0x72DDD0,FakeExtraAir);
        EntryPatch w1(bit_cast<uintptr_t>(&CWorld::Add),FakeWorldAdd), w1e(0x563220,FakeWorldAdd);
        EntryPatch w2(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove), w2e(0x563280,FakeWorldRemove);
        EntryPatch w3(bit_cast<uintptr_t>(&CWorld::ClearScanCodes),FakeClearScanCodes), w3e(0x563470,FakeClearScanCodes);
        EntryPatch cc1(bit_cast<uintptr_t>(&CCarCtrl::SwitchVehicleToRealPhysics),FakeSwitchReal), cc1e(0x423FC0,FakeSwitchReal);
        EntryPatch cc2(bit_cast<uintptr_t>(&CCarCtrl::FindGhostRoadHeight),FakeGhostHeight), cc2e(0x422370,FakeGhostHeight);
        EntryPatch kp(bit_cast<uintptr_t>(&CPed::KillPedWithCar),FakeKillPed), kpe(0x5F0360,FakeKillPed);
        EntryPatch tc(bit_cast<uintptr_t>(&CPedIntelligence::GetTaskClimb),FakeTaskClimb), tce(0x601180,FakeTaskClimb);
        EntryPatch lm(bit_cast<uintptr_t>(&CBike::CalculateLeanMatrix),FakeLean), lme(0x6B7150,FakeLean);
        EntryPatch er(dllEntityRemove,FakeEntityRemove), ere(0x534AE0,FakeEntityRemove);
        EntryPatch ea(dllEntityAdd,FakeEntityAdd), eae(0x533020,FakeEntityAdd);
        EntryPatch cm(bit_cast<uintptr_t>(&CCollision::ProcessColModels),FakeColModels), cme(0x4185C0,FakeColModels);
        const CVector C0{2525.f,2525.f,20.f};

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(fn.cases==0||(!only.empty()&&only!=full))continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5656u; if(!stt)stt=1; sst=stt^0xA5A5A5A5u; if(!sst)sst=7;
                    trace=json::array(); fakePed=nullptr; fakeVeh=nullptr; extraAir=false; ghostHeight=0.f; boundsOn=false;
                    json out; json ret=nullptr, extra=nullptr;

                    // ---------- mundo y pools (identicos en las dos ejecuciones) ----------
                    std::memcpy(dblHdr.data(),dblImg.data(),dblImg.size()); dblStore.fill(0xFD); dblStates.fill(0x80);
                    std::memcpy(sglHdr.data(),sglImg.data(),sglImg.size()); sglStore.fill(0xFD); sglStates.fill(0x80);
                    std::memcpy(entryHdr.data(),entryImg.data(),entryImg.size()); entryStore.fill(0xFD); entryStates.fill(0x80);
                    entStates.fill(0);
                    poolPtrs[0]=sglHdr.data(); poolPtrs[1]=dblHdr.data(); poolPtrs[2]=entryHdr.data(); poolPtrs[3]=pedHdr.data(); poolPtrs[4]=vehHdr.data(); poolPtrs[5]=bldHdr.data(); poolPtrs[6]=objHdr.data();
                    std::memset(reinterpret_cast<void*>(REPEAT),0,16*16*12);
                    for(int y=SEC_LO;y<=SEC_HI;++y)std::memset(reinterpret_cast<void*>(SECTORS+(size_t(y)*120+SEC_LO)*8),0,size_t(SEC_HI-SEC_LO+1)*8);
                    At<void*>(reinterpret_cast<void*>(MOVING_LIST),0)=nullptr;
                    const CVector C=C0+(next()%3==0 ? CVector{uni(-30.f,30.f),uni(-30.f,30.f),0.f} : CVector{0.f,0.f,0.f});
                    static const float steps[]{1.f,1.f,0.5f,2.f,3.f,3.5f,0.25f,1.6666666f};
                    CTimer::ms_fTimeStep=next()%3 ? steps[next()%std::size(steps)] : uni(0.05f,4.f);
                    CTimer::m_snTimeInMilliseconds=next();
                    *reinterpret_cast<int32*>(CURR_AREA)=next()%4==0 ? int32(1+next()%5) : 0;
                    *reinterpret_cast<uint8*>(PLAYER_FOCUS)=uint8(next()%2);
                    *reinterpret_cast<uint8*>(NO_TORQUE)=uint8(next()%4==0); *reinterpret_cast<uint8*>(SECOND_SHIFT)=uint8(next()%3==0);
                    *reinterpret_cast<uint16*>(SCAN_CODE)=uint16(100+next()%50000);
                    {   auto& mn=At<CVector>(reinterpret_cast<void*>(SNOOKER),0xC); auto& mx=At<CVector>(reinterpret_cast<void*>(SNOOKER),0);
                        mn=C+CVector{uni(-3.f,-0.5f),uni(-3.f,-0.5f),uni(-1.f,0.f)}; mx=C+CVector{uni(0.5f,3.f),uni(0.5f,3.f),uni(0.f,1.f)}; }

                    // ---------- model info y colision falsas ----------
                    miBuf.fill(0); At<void*>(miBuf.data(),0)=miVt.data(); At<void*>(miBuf.data(),0x14)=colBuf.data(); At<uint16>(miBuf.data(),0x12)=uint16(next()); miBuf[0x3FF]=uint8(next()%3==0);
                    colBuf.fill(0); At<CVector>(colBuf.data(),0)=CVector{uni(-3.f,-0.3f),uni(-3.f,-0.3f),uni(-2.f,-0.1f)}; At<CVector>(colBuf.data(),0xC)=CVector{uni(0.3f,3.f),uni(0.3f,3.f),uni(0.1f,2.f)};
                    At<CVector>(colBuf.data(),0x18)=CVector{uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-0.5f,0.5f)}; At<float>(colBuf.data(),0x24)=uni(0.2f,4.f);
                    colData.fill(0); colLines.fill(0);
                    {   auto* cd=reinterpret_cast<CCollisionData*>(colData.data()); cd->m_nNumLines=uint8(next()%5); cd->m_pLines=reinterpret_cast<CColLine*>(colLines.data());
                        for(int k=0;k<4;++k){ At<CVector>(colLines.data(),k*0x20)=CVector{uni(-1.f,1.f),uni(-2.f,2.f),uni(0.f,0.5f)}; At<CVector>(colLines.data(),k*0x20+0x10)=CVector{uni(-1.f,1.f),uni(-2.f,2.f),uni(-1.5f,-0.5f)}; }
                        At<void*>(colBuf.data(),0x2C)=next()%2 ? colData.data() : nullptr; }
                    for(int32 m:models)reinterpret_cast<void**>(MI_PTRS)[m]=miBuf.data();
                    auto PickModel=[&]()->int16{ return int16(next()%3 ? 1 : models[next()%models.size()]); };

                    // ---------- entidades ----------
                    auto MakeMat=[&](uint8* m,const CVector& pos){ std::memset(m,0,0x48); auto* v=reinterpret_cast<float*>(m);
                        const float h=uni(-3.2f,3.2f), c=std::cos(h), s=std::sin(h);
                        v[0]=c; v[1]=s; v[4]=-s; v[5]=c; v[10]=1.f; v[12]=pos.x; v[13]=pos.y; v[14]=pos.z;
                        if(next()%3==0){ const float t=uni(-1.2f,1.2f), ct=std::cos(t), st=std::sin(t); v[1]*=ct; v[5]*=ct; v[2]=st*(next()%2 ? 1.f : -1.f); v[9]=st*0.5f; v[10]=ct; }
                        if(next()%5==0)for(int k:{0,1,2,4,5,6,8,9,10})v[k]+=uni(-0.05f,0.05f); };
                    // billar: el CColPoint local de ApplyFriction()/ApplySpeed solo tiene punto y normal en el EXE (resto de pila sin
                    // inicializar); con velocidad relativa > 0,1 la ruta de chispas leeria esas superficies: velocidades pequeñas
                    auto SnookerSpeeds=[&](CPhysical* ph){ ph->m_vecMoveSpeed=CVector{uni(-0.04f,0.04f),uni(-0.04f,0.04f),uni(-0.01f,0.01f)};
                        ph->m_vecTurnSpeed=CVector{uni(-0.004f,0.004f),uni(-0.004f,0.004f),uni(-0.004f,0.004f)}; ph->m_vecFrictionMoveSpeed=CVector{0.f,0.f,0.f}; ph->m_vecFrictionTurnSpeed=CVector{0.f,0.f,0.f}; };
                    static const float massList[]{0.5f,1.f,5.f,20.f,70.f,100.f,800.f,1500.f,3000.f,50000.f,99999.f};
                    std::array<int,NE> types{};
                    auto InitEnt=[&](int k,int type){
                        uint8* e=E(k); std::memset(e,0,ES); types[k]=type;
                        At<void*>(e,0)=fvt.data();
                        const CVector pos=C+CVector{uni(-8.f,8.f),uni(-8.f,8.f),uni(-2.f,2.f)};
                        MakeMat(mats[k].data(),pos); At<void*>(e,0x14)=mats[k].data(); At<CVector>(e,4)=pos; At<float>(e,0x10)=uni(-3.2f,3.2f);
                        At<uint32>(e,0x1C)=(next()&~0x100u)|(next()%4 ? 1u : 0u);
                        At<uint16>(e,0x20)=uint16(next()); At<int16>(e,0x22)=PickModel(); At<uint16>(e,0x2C)=uint16(next()%3 ? next() : *reinterpret_cast<uint16*>(SCAN_CODE)+next()%2);
                        At<uint8>(e,0x2F)=uint8(next()%4==0 ? 1+next()%3 : 0);
                        static const uint8 statuses[]{0,2,3,3,3,3,4,4,5};
                        At<uint8>(e,0x36)=uint8(type|(statuses[next()%std::size(statuses)]<<3));
                        for(size_t b=KNOB;b<ES;++b)e[b]=uint8(next());
                        if(type==ENTITY_TYPE_BUILDING)return;
                        auto* ph=reinterpret_cast<CPhysical*>(e);
                        ph->field_38=uni(0.f,200.f); ph->m_nLastCollisionTime=next();
                        ph->m_nPhysicalFlags=(next()&next())|(next()%3==0 ? 2u : 0u);
                        // bDisableZ (billar) solo en objetos: con un vehiculo BMX ApplyFriction lee superficies de un CColPoint
                        // de pila sin inicializar en el EXE (ruta de chispas)
                        if(type!=ENTITY_TYPE_OBJECT)ph->physicalFlags.bDisableZ=false;
                        ph->m_vecMoveSpeed=next()%6==0 ? CVector{0.f,0.f,0.f} : CVector{uni(-0.3f,0.3f),uni(-0.3f,0.3f),uni(-0.1f,0.1f)};
                        ph->m_vecTurnSpeed=next()%6==0 ? CVector{0.f,0.f,0.f} : CVector{uni(-0.05f,0.05f),uni(-0.05f,0.05f),uni(-0.08f,0.08f)};
                        ph->m_vecFrictionMoveSpeed=next()%3 ? CVector{0.f,0.f,0.f} : CVector{uni(-0.02f,0.02f),uni(-0.02f,0.02f),uni(-0.02f,0.02f)};
                        ph->m_vecFrictionTurnSpeed=next()%3 ? CVector{0.f,0.f,0.f} : CVector{uni(-0.01f,0.01f),uni(-0.01f,0.01f),uni(-0.01f,0.01f)};
                        ph->m_vecForce=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-5.f,5.f)}; ph->m_vecTorque=CVector{uni(-5.f,5.f),uni(-5.f,5.f),uni(-5.f,5.f)};
                        ph->m_fMass=massList[next()%std::size(massList)]*(next()%2 ? 1.f : uni(0.8f,1.2f)); ph->m_fTurnMass=ph->m_fMass*uni(0.3f,8.f);
                        ph->m_fVelocityFrequency=uni(0.5f,1.5f);
                        switch(next()%4){case 0:ph->m_fAirResistance=0.1f;break;case 1:ph->m_fAirResistance=0.99f;break;case 2:ph->m_fAirResistance=uni(0.f,0.2f);break;default:ph->m_fAirResistance=uni(0.9f,1.f);break;}
                        ph->m_fElasticity=uni(0.f,1.f); ph->m_fBuoyancyConstant=uni(0.f,2.f);
                        ph->m_vecCentreOfMass=next()%3 ? CVector{0.f,0.f,0.f} : CVector{uni(-0.5f,0.5f),uni(-0.5f,0.5f),uni(-0.5f,0.5f)};
                        ph->m_nContactSurface=eSurfaceType(next()%179);
                        ph->m_fMovingSpeed=uni(0.f,1.f); ph->m_fDamageIntensity=next()%3 ? 0.f : uni(0.f,600.f); ph->m_nPieceType=uint16(next()%20);
                        ph->m_vecLastCollisionImpactVelocity=CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; ph->m_vecLastCollisionPosn=pos+CVector{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)};
                        ph->m_vecAttachOffset=CVector{uni(-2.f,2.f),uni(-2.f,2.f),uni(-1.f,2.f)}; ph->m_vecAttachedEntityRotation=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-3.f,3.f)};
                        if(next()%2){ ph->m_qAttachedEntityRotation.imag=CVector{0.f,0.f,0.f}; ph->m_qAttachedEntityRotation.real=0.f; }
                        else { CVector q{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; float w=uni(-1.f,1.f); const float m=std::sqrt(q.x*q.x+q.y*q.y+q.z*q.z+w*w); ph->m_qAttachedEntityRotation.imag=CVector{q.x/m,q.y/m,q.z/m}; ph->m_qAttachedEntityRotation.real=w/m; }
                        ph->m_fContactSurfaceBrightness=uni(0.f,1.5f); ph->m_fDynamicLighting=uni(0.f,1.f);
                        if(ph->physicalFlags.bDisableZ)SnookerSpeeds(ph);
                        if(type==ENTITY_TYPE_VEHICLE){ auto* v=reinterpret_cast<CVehicle*>(e);
                            static const std::pair<int,int> vt[]{{0,0},{0,0},{0,0},{0,1},{0,2},{9,9},{9,10},{5,5},{6,6},{3,3},{4,4},{0,11}};
                            const auto& t=vt[next()%std::size(vt)]; At<int32>(e,0x590)=t.first; At<int32>(e,0x594)=t.second;
                            v->m_nAlarmState=uint16(next()%3==0 ? 0xFFFF : next()%2 ? 0 : next()); v->m_vehicleSpecialColIndex=-1; At<int32>(e,0x4A4)=int32(1+next()%4);
                            v->vehicleFlags.bIsLawEnforcer=next()%4==0; v->vehicleFlags.bIsCarParkVehicle=next()%4==0; v->vehicleFlags.bVehicleColProcessed=next()%2; At<uint8>(e,0x42C)=uint8(At<uint8>(e,0x42C)|(next()%4==0 ? 2 : 0)); }
                        else if(type==ENTITY_TYPE_PED){ auto* p=reinterpret_cast<CPed*>(e);
                            static const int32 states[]{1,1,7,11,12,41,50,54,55,56,62};
                            At<int32>(e,0x530)=states[next()%std::size(states)]; At<int32>(e,0x598)=int32(next()%8);
                            p->bIsStanding=next()%2; p->bWasStanding=next()%2; p->bPushOtherPeds=next()%3==0; p->bTestForBlockedPositions=next()%3==0; p->bFallenDown=next()%4==0;
                            if(next()%3==0){ plrData.fill(0); plrData[0x86]=uint8(next()%2); At<void*>(e,0x480)=plrData.data(); } }
                        else if(type==ENTITY_TYPE_OBJECT){ auto* o=reinterpret_cast<CObject*>(e);
                            o->m_nObjectFlags=next()&next(); o->m_nColDamageEffect=uint8(next()%4 ? 0 : next()%220); o->m_nSpecialColResponseCase=uint8(next()%8);
                            o->m_nLastWeaponDamage=uint8(next()%2 ? 0xFF : next()); o->m_fDoorStartAngle=next()%2 ? -1001.f : uni(-3.f,3.f);
                            uint8* oi=objInfo[k].data(); std::memset(oi,0,0x50); auto* d=reinterpret_cast<CObjectData*>(oi);
                            d->m_fMass=next()%4==0 ? 99999.f : uni(1.f,500.f); d->m_fTurnMass=uni(1.f,800.f); d->m_fAirResistance=uni(0.9f,1.f); d->m_fElasticity=uni(0.f,1.f);
                            d->m_fBuoyancyConstant=uni(0.f,2.f); d->m_fUprootLimit=next()%3==0 ? 10000.f : uni(0.f,300.f); d->m_fColDamageMultiplier=uni(0.f,5.f);
                            d->m_nColDamageEffect=uint8(next()%4); d->m_bCausesExplosion=uint8(next()%4==0); o->m_pObjectInfo=d; }
                    };
                    auto RandType=[&]{ static const int t[]{2,2,3,3,4,4,4}; return t[next()%std::size(t)]; };
                    InitEnt(0,RandType()); for(int k=1;k<NE;++k)InitEnt(k,next()%5==0 ? int(ENTITY_TYPE_BUILDING) : RandType());
                    auto Phys=[&](int k){ return types[k]!=ENTITY_TYPE_BUILDING; };
                    auto AnyEnt=[&](bool allowNull)->uint8*{ if(allowNull&&next()%6==0)return nullptr; return E(int(1+next()%(NE-1))); };
                    auto PhysEnt=[&](int from)->int{ for(int t=0;t<16;++t){ const int k=int(from+next()%(NE-from)); if(Phys(k))return k; } InitEnt(from,RandType()); return from; };
                    // registros de colision, daño, entidad ignorada (con referencias registradas a veces)
                    for(int k=0;k<NE;++k)if(Phys(k)){ auto* ph=reinterpret_cast<CPhysical*>(E(k));
                        ph->m_nNumEntitiesCollided=uint8(next()%7); for(int j=0;j<6;++j)ph->m_apCollidedEntities[j]=j<ph->m_nNumEntitiesCollided ? reinterpret_cast<CEntity*>(next()%5==0 ? nullptr : E(int(next()%NE))) : nullptr;
                        if(next()%3==0){ ph->m_pDamageEntity=reinterpret_cast<CEntity*>(AnyEnt(false)); if(next()%2)ph->m_pDamageEntity->RegisterReference(&ph->m_pDamageEntity); }
                        if(next()%4==0)ph->m_pEntityIgnoredCollision=reinterpret_cast<CEntity*>(AnyEnt(false)); }
                    // mundo: listas de los 3x3 sectores con nodos de los pools locales
                    auto SecX=[&](float v){ return std::clamp(int(std::floor(v*0.02f+60.f)),SEC_LO,SEC_HI); };
                    auto BuildWorld=[&]{
                        for(int k=0;k<NE;++k){ uint8* e=E(k); const auto& p=At<CVector>(At<uint8*>(e,0x14),0x30); const int sx=SecX(p.x), sy=SecX(p.y);
                            if(types[k]==ENTITY_TYPE_BUILDING){ CWorld::ms_aSectors[sy][sx].Buildings.AddItem(reinterpret_cast<CBuilding*>(e)); continue; }
                            auto& rs=CWorld::ms_aRepeatSectors[sy&15][sx&15];
                            if(types[k]==ENTITY_TYPE_VEHICLE)rs.Vehicles.AddItem(reinterpret_cast<CVehicle*>(e));
                            else if(types[k]==ENTITY_TYPE_PED)rs.Peds.AddItem(reinterpret_cast<CPed*>(e));
                            else rs.Objects.AddItem(reinterpret_cast<CObject*>(e)); }
                    };
                    auto AddCollisionNodes=[&](int k,int n){ // nodos CEntryInfoNode de la entidad k en celdas de repeticion
                        auto* ph=reinterpret_cast<CPhysical*>(E(k));
                        for(int j=0;j<n;++j){ auto* sector=&CWorld::ms_aRepeatSectors[13+next()%3][13+next()%3];
                            auto* list=reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(sector)+next()%3;
                            auto* node=new CEntryInfoNode; node->m_doubleLink=list->AddItem(ph); node->m_doubleLinkList=list; node->m_repeatSector=sector;
                            node->AddToList(ph->m_pCollisionList.m_node); ph->m_pCollisionList.m_node=node; } };
                    auto CP=[&](int idx,const CVector& at){ uint8* c=cpBuf.data()+idx*0x2C; const uint32 s=sst; sst=next()|1; FillColPoint(c,at); sst=s; return c; };
                    auto SelfPos=[&]{ return At<CVector>(mats[0].data(),0x30); };
                    misc.fill(0);

                    // ---------- fixture por funcion (identico en las dos ejecuciones) ----------
                    uint8* const self=E(0); auto* const sp=reinterpret_cast<CPhysical*>(self);
                    auto RandVec=[&](float r){ return CVector{uni(-r,r),uni(-r,r),uni(-r,r)}; };
                    auto& f0=At<float>(misc.data(),0); auto& f1=At<float>(misc.data(),4); auto& v0=At<CVector>(misc.data(),8); auto& v1=At<CVector>(misc.data(),0x14); auto& v2=At<CVector>(misc.data(),0x20);
                    std::function<void(uintptr_t)> call; bool world=false; bool ctorDtor=false;
                    if(nm=="ApplyMoveForce-vec"){ const CVector fo=RandVec(next()%2 ? 5.f : 5000.f); call=[&,fo](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,CVector)>(a)(self,fo); }; }
                    else if(nm=="ApplyTurnForce"){ const CVector fo=RandVec(next()%2 ? 5.f : 5000.f), pt=next()%4 ? RandVec(3.f) : SelfPos()+RandVec(3.f);
                        call=[&,fo,pt](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,CVector,CVector)>(a)(self,fo,pt); }; }
                    else if(nm=="ApplyForce"){ const CVector fo=RandVec(next()%2 ? 5.f : 5000.f), pt=RandVec(3.f); const bool b=next()%4!=0;
                        call=[&,fo,pt,b](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,CVector,CVector,bool)>(a)(self,fo,pt,b); }; }
                    else if(nm=="GetSpeed"){ const CVector pt=RandVec(3.f);
                        call=[&,pt](uintptr_t a){ CVector o{-1234.f,-1234.f,-1234.f}; auto* r=reinterpret_cast<CVector*(__thiscall*)(void*,CVector*,CVector)>(a)(self,&o,pt); ret={F(o.x),F(o.y),F(o.z),r==&o}; }; }
                    else if(nm=="ApplyMoveSpeed"||nm=="ApplyTurnSpeed"||nm=="ApplyGravity"||nm=="SkipPhysics"||nm=="ApplyAirResistance"||nm=="ApplyFriction-void"
                            ||nm=="ApplySpeed"||nm=="UnsetIsInSafePosition"||nm=="AddToMovingList"||nm=="RemoveFromMovingList"||nm=="DettachAutoAttachedEntity"
                            ||nm=="PositionAttachedEntity"||nm=="CheckCollision"||nm=="CheckCollision_SimpleCar"){
                        extraAir=next()%2;
                        if(nm=="ApplySpeed"||nm=="UnsetIsInSafePosition"||nm=="ApplyFriction-void"){
                            if(next()%3==0){ InitEnt(0,ENTITY_TYPE_OBJECT); sp->physicalFlags.bDisableZ=true; SnookerSpeeds(sp); At<CVector>(mats[0].data(),0x30)=C+CVector{uni(-3.5f,3.5f),uni(-3.5f,3.5f),uni(-1.f,1.f)}; }
                            else if(next()%2){ InitEnt(0,ENTITY_TYPE_OBJECT); sp->physicalFlags.bDisableMoveForce=true; reinterpret_cast<CObject*>(self)->m_fDoorStartAngle=uni(-3.2f,3.2f);
                                sp->m_vecTurnSpeed.z=uni(-0.3f,0.3f); reinterpret_cast<CObject*>(self)->objectFlags.bIsDoorMoving=next()%2; } }
                        if(nm=="AddToMovingList"||nm=="RemoveFromMovingList"){
                            auto& ml=CWorld::ms_listMovingEntityPtrs; for(int k=1;k<NE;++k)if(Phys(k)&&next()%2)ml.AddItem(reinterpret_cast<CPhysical*>(E(k)));
                            if(next()%2){ sp->m_pMovingList=ml.AddItem(sp); if(next()%2)for(int k=1;k<NE;++k)if(Phys(k)&&next()%3==0)ml.AddItem(reinterpret_cast<CPhysical*>(E(k))); } }
                        if(nm=="DettachAutoAttachedEntity"||nm=="PositionAttachedEntity"){ const int k=PhysEnt(1); sp->m_pAttachedTo=reinterpret_cast<CPhysical*>(E(k));
                            static const int16 att[]{530,530,406,406,1,1}; if(next()%2)At<int16>(E(k),0x22)=att[next()%std::size(att)];
                            if(types[k]==ENTITY_TYPE_VEHICLE){ At<uint16>(E(k),0x86C)=uint16(next()%3 ? 0 : next()); At<uint16>(E(k),0x86E)=uint16(next()%3 ? 0 : next()); }
                            if(next()%5==0)At<int16>(self,0x22)=363; if(next()%3==0)sp->physicalFlags.bInfiniteMass=true;
                            if(next()%4==0)At<uint8>(E(k),0x1C)=uint8(At<uint8>(E(k),0x1C)&~1u); }
                        if(nm=="CheckCollision"||nm=="CheckCollision_SimpleCar"){ world=true; if(nm=="CheckCollision_SimpleCar")AddCollisionNodes(0,int(next()%4)); }
                        call=[&](uintptr_t a){ ScrubStack(); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); };
                        if(nm=="CheckCollision"||nm=="CheckCollision_SimpleCar")call=[&](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(self); };
                        fakePed=next()%4==0 ? E(int(next()%NE)) : nullptr; }
                    else if(nm=="ApplyFrictionMoveForce"){ const CVector fo=RandVec(next()%2 ? 1.f : 500.f); call=[&,fo](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,CVector)>(a)(self,fo); }; }
                    else if(nm=="ApplyFrictionForce"){ const CVector fo=RandVec(next()%2 ? 1.f : 500.f), pt=RandVec(3.f); call=[&,fo,pt](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,CVector,CVector)>(a)(self,fo,pt); }; }
                    else if(nm=="AddCollisionRecord"||nm=="GetHasCollidedWith"||nm=="RemoveRefsToEntity"){
                        uint8* other=next()%3==0&&sp->m_nNumEntitiesCollided ? reinterpret_cast<uint8*>(sp->m_apCollidedEntities[next()%sp->m_nNumEntitiesCollided]) : AnyEnt(nm!="AddCollisionRecord");
                        // AddCollisionRecord: el EXE lee el tipo (+0x36) de la otra entidad sin comprobar nulo (0x5434AF)
                        if(nm=="AddCollisionRecord"&&!other)other=AnyEnt(false);
                        if(nm=="RemoveRefsToEntity"&&other&&next()%2)for(int j=0;j<sp->m_nNumEntitiesCollided;++j)if(next()%2)sp->m_apCollidedEntities[j]=reinterpret_cast<CEntity*>(other);
                        if(nm=="GetHasCollidedWith")call=[&,other](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(self,other); };
                        else call=[&,other](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(self,other); }; }
                    else if(nm=="GetHasCollidedWithAnyObject"){ call=[&](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(self); }; }
                    else if(nm=="ApplySpringCollision"||nm=="ApplySpringCollisionAlt"||nm=="ApplySpringDampening"){
                        v0=next()%3 ? CVector{0.f,0.f,-1.f} : RandVec(1.f); v1=SelfPos()+RandVec(2.f); v2=next()%2 ? CVector{0.f,0.f,1.f} : RandVec(1.f);
                        const float level=uni(0.f,3.f), len=next()%4==0 ? uni(1.f,1.5f) : uni(-0.2f,1.f), bias=uni(0.f,1.f); f0=uni(-100.f,100.f);
                        if(nm=="ApplySpringCollision")call=[&,level,len,bias](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,float,CVector&,CVector&,float,float,float&)>(a)(self,level,v0,v1,len,bias,f0); };
                        else if(nm=="ApplySpringCollisionAlt")call=[&,level,len,bias](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,float,CVector&,CVector&,float,float,CVector&,float&)>(a)(self,level,v0,v1,len,bias,v2,f0); };
                        else { const float damp=uni(-0.5f,0.5f), lim=next()%4==0 ? 0.f : uni(-300.f,300.f);
                            call=[&,damp,lim](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,float,float,CVector&,CVector&,CVector&)>(a)(self,damp,lim,v0,v1,v2); }; } }
                    else if(nm=="GetLightingFromCol"){ const uint32 b=next()%2; call=[&,b](uintptr_t a){ ret=CallThisF1(a,self,b); }; }
                    else if(nm=="GetLightingTotal"){ call=[&](uintptr_t a){ ret=CallThisF0(a,self); }; }
                    else if(nm=="CanPhysicalBeDamaged"){ const int32 w=next()%16==0 ? int32(next()%200)-20 : int32(next()%60); const bool withFlag=next()%4!=0; At<uint8>(misc.data(),0x3C)=0xCC;
                        call=[&,w,withFlag](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,int32,bool*)>(a)(self,w,withFlag ? reinterpret_cast<bool*>(misc.data()+0x3C) : nullptr); }; }
                    else if(nm=="SetDamagedPieceRecord"){ uint8* other=AnyEnt(false); if(next()%4==0)At<int16>(other,0x22)=*reinterpret_cast<int16*>(MI_CUE); uint8* c=CP(0,SelfPos());
                        const float d=uni(0.f,800.f), m=uni(-2.f,2.f); call=[&,other,c,d,m](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,float,void*,void*,float)>(a)(self,d,other,c,m); }; }
                    else if(nm=="ApplyCollision-1"||nm=="ApplySoftCollision-1"){ uint8* other=AnyEnt(false); uint8* c=CP(0,SelfPos()); f0=-77.f;
                        call=[&,other,c](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*,void*,float&)>(a)(self,other,c,f0); }; }
                    else if(nm=="ApplyCollisionAlt"){ const int k=PhysEnt(1); uint8* c=CP(0,SelfPos()); f0=-77.f; v0=CVector{-5.f,-5.f,-5.f}; v1=CVector{-6.f,-6.f,-6.f};
                        if(next()%3==0){ const int j=PhysEnt(2); sp->m_pAttachedTo=reinterpret_cast<CPhysical*>(E(j)); }
                        call=[&,k,c](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*,void*,float&,CVector&,CVector&)>(a)(self,E(k),c,f0,v0,v1); }; }
                    else if(nm=="ApplyFriction-self"){ uint8* c=CP(0,SelfPos()); const float fr=uni(0.f,next()%2 ? 0.1f : 3.f);
                        call=[&,c,fr](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,float,void*)>(a)(self,fr,c); }; }
                    else if(nm=="ApplyFriction-other"){ const int k=PhysEnt(1); uint8* c=CP(0,SelfPos()); const float fr=uni(0.f,next()%2 ? 0.1f : 3.f);
                        call=[&,k,c,fr](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*,float,void*)>(a)(self,E(k),fr,c); }; }
                    else if(nm=="ProcessControl"){ if(next()%3==0){ const int k=PhysEnt(1); sp->m_pAttachedTo=reinterpret_cast<CPhysical*>(E(k)); } extraAir=next()%2;
                        call=[&](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }; }
                    else if(nm=="ApplyCollision-2"||nm=="ApplySoftCollision-2"){
                        const int k=nm=="ApplySoftCollision-2" ? PhysEnt(1) : int(1+next()%(NE-1)); uint8* c=CP(0,SelfPos()); f0=-77.f; f1=-88.f;
                        if(next()%4==0)At<int16>(E(k),0x22)=*reinterpret_cast<int16*>(next()%2 ? MI_HYDRANT : MI_METER1);
                        if(types[0]==ENTITY_TYPE_PED&&next()%3==0)At<void*>(self,0x568)=E(k);
                        if(types[k]==ENTITY_TYPE_PED&&next()%3==0)At<void*>(E(k),0x568)=self;
                        fakeVeh=next()%4==0 ? (types[0]==ENTITY_TYPE_VEHICLE ? self : E(k)) : nullptr;
                        if(next()%4==0)sp->m_pAttachedTo=reinterpret_cast<CPhysical*>(E(PhysEnt(1)));
                        call=[&,k,c](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*,void*,float&,float&)>(a)(self,E(k),c,f0,f1); }; }
                    else if(nm=="GetBoundRect"){ call=[&](uintptr_t a){ float o[4]{-1.f,-1.f,-1.f,-1.f}; auto* r=reinterpret_cast<float*(__thiscall*)(void*,float*)>(a)(self,o); ret={F(o[0]),F(o[1]),F(o[2]),F(o[3]),r==o}; }; }
                    else if(nm=="Constructor"){ ctorDtor=true; std::memset(self,0xCD,ES); std::memset(mats[NE].data(),0xCD,0x48);
                        call=[&](uintptr_t a){ EntryPatch s1(allocStatic,FakeAllocStatic), s1e(0x54F4C0,FakeAllocStatic);
                                               ret=Name(reinterpret_cast<void*(__thiscall*)(void*)>(a)(self)); }; }
                    else if(nm=="Destructor"){ ctorDtor=true; if(next()%2)sp->m_pShadowData=reinterpret_cast<CRealTimeShadow*>(shadowBuf.data()); if(next()%2)AddCollisionNodes(0,int(1+next()%3));
                        call=[&](uintptr_t a){ EntryPatch d1(dllEntityDtor,FakeEntityDtor), d1e(0x535E90,FakeEntityDtor), d2(flushFn,FakeFlush), d2e(0x536E10,FakeFlush),
                                                           d3(returnShadow,FakeReturnShadow), d3e(0x705B30,FakeReturnShadow);
                                               reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }; }
                    else if(nm=="Remove"){ AddCollisionNodes(0,int(next()%5)); for(int k=1;k<NE;++k)if(Phys(k)&&next()%2)AddCollisionNodes(k,int(next()%3));
                        if(next()%6==0)At<uint32>(self,0x1C)|=0x100u; call=[&](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }; }
                    else if(nm=="DettachEntityFromEntity"){ const int k=PhysEnt(1); sp->m_pAttachedTo=reinterpret_cast<CPhysical*>(E(k));
                        if(next()%2)sp->physicalFlags.bDisableCollisionForce=true; const float x=uni(-3.2f,3.2f), y=uni(-3.2f,3.2f), z=uni(-2.f,2.f); const bool b=next()%2;
                        call=[&,x,y,z,b](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,float,float,float,bool)>(a)(self,x,y,z,b); }; }
                    else if(nm=="AttachEntityToEntity-vec"||nm=="AttachEntityToEntity-quat"){
                        const int k=PhysEnt(1); uint8* target=next()%10==0 ? nullptr : E(k);
                        if(next()%3==0){ const int j=PhysEnt(1); sp->m_pAttachedTo=reinterpret_cast<CPhysical*>(E(j)); if(next()%2)sp->m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&sp->m_pAttachedTo)); }
                        if(target&&next()%3==0)At<int16>(target,0x22)=530;
                        if(nm=="AttachEntityToEntity-vec"){ const CVector off=RandVec(2.f), rot=RandVec(3.2f); call=[&,target,off,rot](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,void*,CVector,CVector)>(a)(self,target,off,rot); }; }
                        else { v0=RandVec(2.f); auto& q=At<CQuaternion>(misc.data(),0x20); { CVector qi=RandVec(1.f); float w=uni(-1.f,1.f); const float m=std::sqrt(qi.x*qi.x+qi.y*qi.y+qi.z*qi.z+w*w); q.imag=CVector{qi.x/m,qi.y/m,qi.z/m}; q.real=w/m; }
                            const bool withOff=next()%3!=0, withRot=next()%3!=0;
                            call=[&,target,withOff,withRot](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*,void*,CVector*,CQuaternion*)>(a)(self,target,withOff ? &v0 : nullptr,withRot ? &At<CQuaternion>(misc.data(),0x20) : nullptr); }; } }
                    else if(nm=="PlacePhysicalRelativeToOtherPhysical"){ const int k=PhysEnt(1); const CVector off=RandVec(3.f);
                        call=[&,k,off](uintptr_t a){ reinterpret_cast<void(__cdecl*)(void*,void*,CVector)>(a)(E(k),self,off); }; }
                    else if(nm=="ProcessEntityCollision"){ uint8* other=AnyEnt(false); for(auto& b:cpArr)b=0xEE;
                        call=[&,other](uintptr_t a){ ret=reinterpret_cast<int32(__thiscall*)(void*,void*,void*)>(a)(self,other,cpArr.data()); }; }
                    else if(nm=="ProcessCollisionSectorList"||nm=="ProcessShiftSectorList"){ world=true; const int32 x=SEC_LO+int32(next()%3), y=SEC_LO+int32(next()%3);
                        const int32 sx=next()%4 ? SecX(SelfPos().x) : x, sy=next()%4 ? SecX(SelfPos().y) : y;
                        if(nm=="ProcessShiftSectorList"&&types[0]==ENTITY_TYPE_VEHICLE&&next()%2)sp->physicalFlags.bProcessingShift=true;
                        call=[&,sx,sy](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,int32,int32)>(a)(self,sx,sy); }; }
                    else if(nm=="ProcessCollisionSectorList_SimpleCar"){ world=true; const int rx=13+int(next()%3), ry=13+int(next()%3);
                        call=[&,rx,ry](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(self,&CWorld::ms_aRepeatSectors[ry][rx]); }; }
                    else if(nm=="TestCollision"){ world=true; const uint32 b=next()%2; call=[&,b](uintptr_t a){ ret=reinterpret_cast<bool(__thiscall*)(void*,uint32)>(a)(self,b); }; }
                    else if(nm=="ProcessCollision"||nm=="ProcessShift"){ world=true;
                        if(nm=="ProcessCollision"&&types[0]==ENTITY_TYPE_VEHICLE&&next()%4==0){ At<uint8>(self,0x36)=uint8((At<uint8>(self,0x36)&7)|(STATUS_GHOST<<3)); ghostHeight=uni(15.f,25.f); At<void*>(colBuf.data(),0x2C)=colData.data(); }
                        if(next()%3==0)AddCollisionNodes(0,int(1+next()%3));
                        call=[&](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }; }
                    else if(nm=="Add"||nm=="RemoveAndAdd"){
                        /* Add (0x544A30) y RemoveAndAdd (0x542560): rectangulo inyectado (GetBoundRect trazado) con coordenadas en los bordes de
                           sector (multiplos de 50 y sus vecinos de 1 ulp: x87 coordenada*0,02+60, floor y _ftol), extremos negativos del mapa, rangos
                           vacios y rangos de mas de 16 celdas (se solapan por la mascara &15). Listas de repeticion reales con entidades de fondo,
                           nodos CEntryInfoNode previos (tambien en las celdas destino y en la misma lista), nodos reutilizados, sobrantes retirados y
                           edificio grande (CEntity::Add/Remove trazados; la lista de colision no se toca). */
                        const bool redo=nm=="RemoveAndAdd", big=next()%9==0;
                        auto Snap=[&]()->float{ const float x=float(int(next()%161)-80)*50.f; switch(next()%4){ case 0:return std::nextafter(x,-INFINITY); case 1:return x; case 2:return std::nextafter(x,INFINITY); default:return x+uni(-25.f,25.f); } };
                        float l,b;
                        if(next()%40==0){ l=uni(-1.0e7f,1.0e7f); b=uni(-1.0e7f,1.0e7f); } else if(next()%7==0){ l=uni(-4500.f,4500.f); b=uni(-4500.f,4500.f); } else { l=Snap(); b=Snap(); }
                        float w=next()%5==0 ? 0.f : float(next()%6)*50.f+(next()%3 ? 0.f : uni(0.f,50.f));
                        float h=next()%5==0 ? 0.f : float(next()%6)*50.f+(next()%3 ? 0.f : uni(0.f,50.f));
                        if(next()%14==0)w=-uni(1.f,120.f);
                        if(next()%18==0)h=-uni(1.f,120.f);
                        boundsOn=true; bnd[0]=l; bnd[1]=b+h; bnd[2]=l+w; bnd[3]=b;
                        const int bx=int(std::floor(l*0.02f+60.f)), by=int(std::floor(b*0.02f+60.f));
                        const int ownList=types[0]>=2&&types[0]<=4 ? types[0]-2 : int(next()%3);
                        auto Cell=[&](int c)->CRepeatSector*{ return &CWorld::ms_aRepeatSectors[unsigned(c>>4)&15u][unsigned(c&15)]; };
                        auto Background=[&](int n){ for(int j=0;j<n;++j){ const int cx=next()%2 ? bx+int(next()%4) : int(next()%16), cy=next()%2 ? by+int(next()%4) : int(next()%16);
                            auto* list=reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(Cell((cy&15)*16+(cx&15)))+next()%3; const int k=int(next()%NE);
                            list->AddItem(reinterpret_cast<CPhysical*>(Phys(k)&&next()%4 ? E(k) : self)); } };
                        auto MakeNodes=[&](int n){ for(int j=0;j<n;++j){ const int cx=next()%3==0 ? bx+int(next()%4) : int(next()%16), cy=next()%3==0 ? by+int(next()%4) : int(next()%16);
                            auto* sector=Cell((cy&15)*16+(cx&15)); auto* list=reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(sector)+(next()%2 ? ownList : int(next()%3));
                            auto* node=new CEntryInfoNode; node->m_doubleLink=list->AddItem(sp); node->m_doubleLinkList=list; node->m_repeatSector=sector;
                            node->AddToList(sp->m_pCollisionList.m_node); sp->m_pCollisionList.m_node=node; } };
                        Background(int(next()%10));
                        MakeNodes(redo ? (next()%4==0 ? 0 : int(next()%21)) : (next()%2 ? 0 : int(next()%4)));
                        Background(int(next()%6));
                        if(big){ At<uint32>(self,0x1C)|=0x100u; At<uint8>(self,0x36)=uint8((At<uint8>(self,0x36)&~7u)|(1u+next()%5)); }
                        call=[&,redo](uintptr_t a){ reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                            json en=json::array(); int g=0;
                            for(auto* n=At<uint8*>(self,0xB0);n&&g<400;n=At<uint8*>(n,0x10),++g){ auto* lk=At<uint8*>(n,4);
                                en.push_back({Name(n),Name(At<void*>(n,0)),Name(lk),Name(lk ? At<void*>(lk,0) : nullptr),Name(At<void*>(n,8)),Name(At<void*>(n,0xC)),Name(At<void*>(n,0x10))}); }
                            ret={{"entries",en},{"redo",redo}};
                            json cells=json::array();
                            for(int c=0;c<256;++c)for(int t=0;t<3;++t){ auto* hd=*reinterpret_cast<uint8**>(REPEAT+size_t(c)*12+size_t(t)*4); if(!hd)continue;
                                json ch=json::array(); int q=0; for(auto* n=hd;n&&q<600;n=At<uint8*>(n,4),++q)ch.push_back({Name(n),Name(At<void*>(n,0)),Name(At<void*>(n,8))});
                                cells.push_back({c,t,ch}); }
                            extra=cells; }; }
                    if(world){ BuildWorld(); fakePed=next()%4==0 ? E(int(next()%NE)) : nullptr; }

                    // ---------- llamada ----------
                    for(auto* h:hooks)h->State(candidate);
                    uintptr_t target=fn.address; bool switches;
                    if(fn.vt){ target=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot]; switches=(target<0x400000||target>=0x1000000)==candidate; }
                    else switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    out["switches"]=switches;
                    call(target);

                    // ---------- resultado ----------
                    {   json ents=json::array();
                        for(int k=0;k<NE;++k){ std::array<uint8,ES> c; std::memcpy(c.data(),E(k),ES);
                            if(ctorDtor&&k==0){ std::memset(c.data(),0,4); c[0xBA]=0; }
                            ents.push_back({Hex(c.data()+4,0x134),Hash(c.data(),ES)}); }
                        out["ents"]=ents; }
                    /* Las palabras de relleno (+0x0C/+0x1C/+0x2C/+0x3C) de una matriz copiada por el EXE vienen de CMatrix::operator* (0x59BE30), que deja
                       esos huecos con basura de pila (operator*= 0x411A80, cuaternion->matriz): no se comparan */
                    {   auto mm=mats; for(auto& m:mm)for(size_t o:{0x0Cu,0x1Cu,0x2Cu,0x3Cu})std::memset(m.data()+o,0,4);
                        out["mats"]=Hash(mm.data(),sizeof(mm)); } out["misc"]=Hex(misc.data(),misc.size()); out["cp"]=Hex(cpBuf.data(),cpBuf.size()); out["cpa"]=Hash(cpArr.data(),cpArr.size());
                    out["col"]=Hash(colBuf.data(),colBuf.size())+Hash(colData.data(),colData.size());
                    out["globals"]={F(CTimer::ms_fTimeStep),Hash(reinterpret_cast<void*>(PLAYERS),PLAYERS_SIZE),*reinterpret_cast<uint16*>(SCAN_CODE),*reinterpret_cast<uint8*>(SECOND_SHIFT),
                                    Hash(reinterpret_cast<void*>(CPS_A),CPS_SIZE),*reinterpret_cast<uint32*>(CPS_A_GUARD),Hash(reinterpret_cast<void*>(CPS_B),CPS_SIZE),*reinterpret_cast<uint32*>(CPS_B_GUARD),
                                    Hash(reinterpret_cast<void*>(TEMP_CPS),CPS_SIZE)};
                    out["refs"]={Hash(&CReferences::aRefs[0],sizeof(CReferences::aRefs)),Name(CReferences::pEmptyList)};
                    out["nodes"]={Hash(dblHdr.data(),0x14),Hash(dblStore.data(),dblStore.size()),Hash(dblStates.data(),dblStates.size()),Hash(sglHdr.data(),0x14),Hash(sglStore.data(),sglStore.size()),
                                  Hash(sglStates.data(),sglStates.size()),Hash(entryHdr.data(),0x14),Hash(entryStore.data(),entryStore.size()),Hash(entryStates.data(),entryStates.size()),
                                  Name(At<void*>(reinterpret_cast<void*>(MOVING_LIST),0))};
                    {   json w=json::array(); w.push_back(Hash(reinterpret_cast<void*>(REPEAT),16*16*12));
                        for(int y=SEC_LO;y<=SEC_HI;++y)w.push_back(Hash(reinterpret_cast<void*>(SECTORS+(size_t(y)*120+SEC_LO)*8),size_t(SEC_HI-SEC_LO+1)*8)); out["world"]=w; }
                    out["ret"]=ret; out["extra"]=extra; out["trace"]=trace;
                    RestoreAll();
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",Restored()}});
}
}
