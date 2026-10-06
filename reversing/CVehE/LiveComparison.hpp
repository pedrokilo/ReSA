#pragma once
// Banco dentro del motor: CVehicle, grupo E (armas de avion, barco, cabrestante, cuchillas y colision; 26 funciones):
// ApplyBoatWaterResistance, ProcessBoatControl, BladeColSectorList<CPtrListSingleLink<CEntity*>>, DoBladeCollision,
// DoFixedMachineGuns, FirePlaneGuns, FireUnguidedMissile, Get/SetFiringRateMultiplier, GetPlaneGunsAutoAimAngle,
// GetPlaneGunsPosition, GetPlaneGunsRateOfFire, GetPlaneNumGuns, GetPlaneOrdnancePosition, GetPlaneOrdnanceRateOfFire,
// SelectPlaneWeapon, InitWinch, PickUpEntityWithWinch, QueryPickedUpEntityWithWinch, ReleasePickedUpEntityWithWinch,
// RemoveWinch, UpdateWinch, Get/SetRopeHeightForHeli, SpecialEntityCalcCollisionSteps (vtable 0x871E80, ranura 16) y
// SpecialEntityPreCollisionStuff (ranura 15).
// Original (hooks del grupo desactivados) frente a candidato con fixtures identicos y comparacion bit a bit.
// Vehiculo FALSO (bufer de 0xA40 bytes con 0xCD y solo los campos que el EXE lee), peatones, entidades, matrices, modelos de
// colision, informacion de modelo (se intercambian temporalmente las entradas de ms_modelInfoPtrs usadas), cuerdas (se guarda
// y restaura todo el array de CRopes) y nodos de lista de CPtrNodeSingleLinkPool (se guarda y restaura el pool): nunca
// vehiculos, peatones ni pools reales. Se simulan y trazan (en el EXE y en la DLL): fuerzas (ApplyMoveForce/ApplyTurnForce/
// ApplySoftCollision/SetDamagedPieceRecord), GetColModel, AnimatePedColModelSkinned, CCollision::ProcessColModels, GetLocalDirection,
// Fx (AddSparks/AddBlood), eventos (AffectsPed, ComputeDamageResponse, CEventGroup::Add; el constructor y destructor de
// CEventDamage corren de verdad), CLocalisation::Blood, GetIsOnScreen, camara, audio (ReportCollision/ReportWeaponEvent/
// AddAudioEvent), rand, ClearScanCodes, cBuoyancy::ProcessBuoyancyBoat, GetWaterLevel, AddWakePoint, pads (GetHandBrake/
// GetCarGunFired/StartShake), HandlePlayerBreath, GetPadFromPlayer, CWeapon::FireInstantHit/FireProjectile, DoPlaneGunFireFX,
// FireFixedMachineGuns, DoesNotCollideWithFlyers, ApplyBoatWaterResistance (solo en ProcessBoatControl), CRopes::RegisterRope y
// CRope::ReleasePickedUpObject/PickUpObject/Remove. FindRope, el constructor de CWeapon, CMatrix (Invert, operator=) y fsin/fcos/
// pow corren de verdad. Se guardan y restauran el reloj, el paso, el codigo de escaneo, planeRotorDmgTimeMS, los puntos de
// colision temporales, la camara activa, las entradas de model info, las cuerdas, el pool de nodos y la zona de la malla de aspas.
#include <fstream>
#include <set>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "Entity/Vehicle/Boat.h"
#include "Entity/Vehicle/Automobile.h"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Heli.h"
#include "Entity/Ped/Ped.h"
#include "Entity/Ped/PlayerPed.h"
#include "Entity/Object/Object.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "Ropes.h"
#include "Rope.h"
#include "Buoyancy.h"
#include "Audio/Entities/AEVehicleAudioEntity.h"
#include "../ProbeRuntime.hpp"

namespace CVehELive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
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
// sigue los saltos de la ILT de la DLL sin entrar en la imagen del EXE
inline uintptr_t Body(uintptr_t p) {
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p;
}
inline bool InExe(uintptr_t a) { return a>=0x400000&&a<0x1000000; }

// --- buferes con nombre para identificar punteros en las trazas ---
struct Named { const void* p; size_t n; const char* name; };
inline std::vector<Named> named;
inline void Nm(const void* p,size_t n,const char* name){ named.push_back({p,n,name}); }
inline json Name(uintptr_t v){
    if(!v)return nullptr;
    for(auto& g:named)if(v>=uintptr_t(g.p)&&v<uintptr_t(g.p)+g.n)return std::string(g.name)+"+"+std::to_string(v-uintptr_t(g.p));
    if(v>=0xB7D0B8&&v<0xB7D0B8+8*120*120)return json::array({"sector",(v-0xB7D0B8)/8,(v-0xB7D0B8)%8});
    if(v>=0xB992B8&&v<0xB992B8+12*256)return json::array({"repeat_sector",(v-0xB992B8)/12,(v-0xB992B8)%12});
    if(v>=0xB9ACD0&&v<0xB9ACD0+0x2C*32)return json::array({"col_point",(v-0xB9ACD0)/0x2C,(v-0xB9ACD0)%0x2C});
    if(v>=0xB768B8&&v<0xB768B8+0x328*8)return json::array({"rope",(v-0xB768B8)/0x328,(v-0xB768B8)%0x328});
    if(v>=0xB73458&&v<0xB73458+0x140*2)return json::array({"pad",(v-0xB73458)/0x134});
    return "other";
}
inline json V3(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }
template<class V> inline json Jv(V v){
    if constexpr(std::is_same_v<V,json>) return v;
    else if constexpr(std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<V>) return Name(reinterpret_cast<uintptr_t>(v));
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    // direccion del EXE y cuerpo de la DLL (se sigue la ILT)
    template<class Fp> void Both(uintptr_t exeAddr,Fp f,uintptr_t dllSym){ Add(exeAddr,f); if(dllSym)Add(Body(dllSym),f); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

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
    return a ? Body(a) : 0; }

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=26;
inline const std::array<Fn,NFNS> fns{{
    {"CVehicle","ApplyBoatWaterResistance",0x6D2740,0,-1,512},
    {"CVehicle","ProcessBoatControl",0x6DBCE0,0,-1,1024},
    {"CVehicle","BladeColSectorList<CPtrListSingleLink<CEntity*>>",0x6DAF00,0,-1,512},
    {"CVehicle","DoBladeCollision",0x6E2E50,0,-1,512},
    {"CVehicle","DoFixedMachineGuns",0x73F400,0,-1,256},
    {"CVehicle","FirePlaneGuns",0x6D4D30,0,-1,512},
    {"CVehicle","FireUnguidedMissile",0x6D5110,0,-1,512},
    {"CVehicle","GetFiringRateMultiplier",0x6D4090,0,-1,128},
    {"CVehicle","SetFiringRateMultiplier",0x6D4010,0,-1,256},
    {"CVehicle","GetPlaneGunsAutoAimAngle",0x6D3E00,0,-1,128},
    {"CVehicle","GetPlaneGunsPosition",0x6D4290,0,-1,256},
    {"CVehicle","GetPlaneGunsRateOfFire",0x6D40E0,0,-1,256},
    {"CVehicle","GetPlaneNumGuns",0x6D3F30,0,-1,128},
    {"CVehicle","GetPlaneOrdnancePosition",0x6D46E0,0,-1,256},
    {"CVehicle","GetPlaneOrdnanceRateOfFire",0x6D4590,0,-1,256},
    {"CVehicle","SelectPlaneWeapon",0x6D4900,0,-1,256},
    {"CVehicle","InitWinch",0x6D3B60,0,-1,128},
    {"CVehicle","PickUpEntityWithWinch",0x6D3CD0,0,-1,64},
    {"CVehicle","QueryPickedUpEntityWithWinch",0x6D3CF0,0,-1,64},
    {"CVehicle","ReleasePickedUpEntityWithWinch",0x6D3CB0,0,-1,64},
    {"CVehicle","RemoveWinch",0x6D3C70,0,-1,128},
    {"CVehicle","UpdateWinch",0x6D3B80,0,-1,256},
    {"CVehicle","GetRopeHeightForHeli",0x6D3D10,0,-1,64},
    {"CVehicle","SetRopeHeightForHeli",0x6D3D30,0,-1,64},
    {"CVehicle","SpecialEntityCalcCollisionSteps",0,0x871E80,16,512},
    {"CVehicle","SpecialEntityPreCollisionStuff",0,0x871E80,15,1024},
}};

// ======================= objetos falsos =======================
constexpr size_t VEHSZ=0xA40, ENTSZ=0x700, MATSZ=0x60, NENT=5;
alignas(16) inline std::array<uint8,VEHSZ> veh{}, otherVeh{};
alignas(16) inline std::array<uint8,MATSZ> vehMat{}, colMat{}, bladeMat{};
alignas(16) inline std::array<uint8,0x100> handling{};
alignas(16) inline std::array<uint8,0x40> boatHandling{}, vehCM{}, bladeCM{}, pedCM{}, colCM{};
alignas(16) inline std::array<uint8,ENTSZ> drv{}, col{};
alignas(16) inline std::array<uint8,0x100> intel{}, clumpBuf{};
alignas(16) inline std::array<uint8,0x140> fakePad{};
alignas(16) inline std::array<std::array<uint8,ENTSZ>,NENT> ents{};
alignas(16) inline std::array<std::array<uint8,MATSZ>,NENT> entMats{};
alignas(16) inline std::array<std::array<uint8,0x40>,NENT> entCMs{};
inline std::array<void*,100> trapVt{};
inline std::array<float,4> fakeCamPos{};
inline CVector camPos{};
inline float lastDepth{};
inline float outBools[1]{};

// desplazamientos crudos del EXE
constexpr size_t E_MATRIX=0x14, E_CLUMP=0x18, E_FLAGS=0x1C, E_MODEL=0x22, E_REFS=0x24, E_SCAN=0x2C, E_TYPE=0x36, E_PHYS=0x40, E_MOVE=0x44, E_TURN=0x50,
    E_MASS=0x8C, E_TURNMASS=0x90, E_ELAST=0x9C, E_BUOY=0xA0, E_COM=0xA4, E_ATTACHED=0xFC, E_IGNORED=0x128, E_BRIGHT=0x12C,
    V_AUDIO=0x138, V_HANDLING=0x384, V_STEER=0x494, V_STEER2=0x498, V_GAS=0x49C, V_ROPEBYTE=0x4A8, V_GUNBYTE=0x4A9, V_AMMOCLIP=0x4AB, V_TOWING=0x4C4, V_TOWED=0x4C8,
    V_FIRETIME=0x4D4, V_PROJTIME=0x4FC, V_PROJTIME2=0x500, V_WEAPONINUSE=0x513, V_FLAGS=0x428, V_DRIVER=0x460, V_PASS=0x464, V_TYPE=0x590, V_SUBTYPE=0x594,
    V_BOATFLAGS=0x5AC, V_BOATFORCE=0x630, V_ROTORSPEED=0x84C, V_DOOM=0x950, V_PLANEFIRE=0x9D8, V_HELIFIRE=0xA10,
    P_FLAGS1=0x46C, P_FLAGS2=0x470, P_INTEL=0x47C, P_VEHICLE=0x58C, P_PEDTYPE=0x598, O_TYPE=0x13C, O_FLAGS=0x140, O_COLDMG=0x144;
constexpr uintptr_t ADDR_STEP=0xB7CB5C, ADDR_TIME=0xB7CB84, ADDR_SCAN=0xB7CD78, ADDR_ROTORTIME=0xC1CC1C, ADDR_TEMPCOL=0xB9ACD0, ADDR_ACTIVECAM=0xB6F081,
    ADDR_CAMDIR=0xB6F1C4, ADDR_ROPES=0xB768B8, ADDR_BUOYIMM=0xC1C94C, ADDR_BLADECOL=0xC1CD38, ADDR_MODELINFOS=0xA9B0C8, ADDR_PADS=0xB73458;
constexpr size_t BLADECOL_SIZE=0x88;
// modelos usados (se sustituyen sus entradas de ms_modelInfoPtrs por informacion falsa)
inline constexpr std::array<int32,16> USED_MODELS{425,447,464,476,487,497,520,548,576,460,411,500,19990,345,441,586};
alignas(16) inline std::array<std::array<uint8,0x80>,USED_MODELS.size()> fakeMi{};
alignas(16) inline std::array<std::array<uint8,0x100>,USED_MODELS.size()> fakeVs{};

// configuracion del caso consultada por los falsos
struct Cfg {
    bool affects{}, bloodOn{}, onScreen{}, handBrake{}, gunFired{}, waterRet{}, waterWrite{}, buoyRet{}, noFlyers{}, softRet{};
    int32 numColls{}, localDir{}; float softDmg{};
    void* animRet{};
};
inline Cfg cfg;
inline Rng sr{1};
inline int32 colMapCount{}; inline std::array<void*,NENT+2> colMapEnt{}, colMapCM{};

// ======================= falsos =======================
inline void __fastcall V_Trap(void* self,void*){ T("virtual_inesperada",self); }
inline int __cdecl S_Rand(){ T("rand"); return int(sr.next()>>9)&0x7FFF; }
inline uint16 S_RandomNumber(){ T("random_number"); return uint16(sr.next()>>9)&0x7FFF; }
inline void __cdecl S_ClearScan(){ T("clear_scan_codes"); }
inline void __fastcall S_MoveForce(void* s,void*,CVector f){ T("apply_move_force",s,V3(f)); }
inline void __fastcall S_TurnForce(void* s,void*,CVector f,CVector p){ T("apply_turn_force",s,V3(f),V3(p)); }
inline bool __fastcall S_SoftCol(void* s,void*,void* e,const void* cp,float* dmg){ T("apply_soft_collision",s,e,cp); *dmg=cfg.softDmg; return cfg.softRet; }
inline void __fastcall S_DamagedPiece(void* s,void*,float dmg,void* e,const void* cp,float m){ T("set_damaged_piece_record",s,dmg,e,cp,m); }
inline void* __fastcall S_GetColModel(void* e,void*){
    T("get_col_model",e);
    for(int32 k=0;k<colMapCount;++k)if(colMapEnt[size_t(k)]==e)return colMapCM[size_t(k)];
    return vehCM.data();
}
inline void* __fastcall S_AnimCM(void* mi,void*,void* clump){ T("animate_ped_col_model_skinned",mi,clump); return cfg.animRet; }
inline int32 __cdecl S_ProcessCol(const void* ma,void* ca,const void* mb,void* cb,void* pts,void* lines,void* touch,uint32 flag){
    T("process_col_models",ma,ca,mb,cb,pts,lines,touch,flag&0xFF);
    if(uintptr_t(pts)==ADDR_TEMPCOL)for(int32 k=0;k<cfg.numColls&&k<32;++k){ auto* p=reinterpret_cast<float*>(ADDR_TEMPCOL+size_t(k)*0x2C);
        for(int j=0;j<11;++j)p[j]=sr.uni(-2.f,2.f); }
    return cfg.numColls; }
inline int32 __fastcall S_LocalDir(void* ped,void*,const CVector2D* v){ T("get_local_direction",ped,F(v->x),F(v->y)); return cfg.localDir; }
inline void __fastcall S_Sparks(void* fx,void*,const CVector* o,const CVector* d,float force,int32 amt,CVector across,int32 type,float spread,float life){
    T("add_sparks",V3(*o),V3(*d),force,amt,V3(across),type,spread,life); }
inline void __fastcall S_Blood(void* fx,void*,const CVector* o,const CVector* d,int32 amt,float f){ T("add_blood",V3(*o),V3(*d),amt,f); }
inline bool __fastcall S_Affects(void* ev,void*,void* ped){ T("event_affects_ped",ped); return cfg.affects; }
inline void __fastcall S_ComputeResp(void* calc,void*,void* ped,void* resp,uint32 speak){ T("compute_damage_response",ped,speak&0xFF); }
inline void* __fastcall S_EvAdd(void* grp,void*,void* ev,uint32 valid){
    auto* e=reinterpret_cast<CEventDamage*>(ev);
    T("event_group_add",grp,valid&0xFF,e->m_pSourceEntity,int32(e->m_weaponType),int32(e->m_pedPieceType),uint32(e->m_ucDirection),uint32(e->m_bWitnessedInVehicle),uint32(e->m_damageResponse.m_bDamageCalculated));
    return nullptr; }
inline bool __cdecl S_LocBlood(){ T("localisation_blood"); return cfg.bloodOn; }
inline bool __fastcall S_OnScreen(void* e,void*){ T("get_is_on_screen",e); return cfg.onScreen; }
inline CVector* __fastcall S_CamPos(void* cam,void*){ T("get_game_cam_position"); return &camPos; }
inline void __fastcall S_ReportCol(void* audio,void*,void* e1,void* e2,int32 s1,int32 s2,const CVector* pos,const CVector* normal,float i1,float i2,uint32 b1,uint32 b2){
    T("report_collision",e1,e2,s1,s2,V3(*pos),normal,i1,i2,b1&0xFF,b2&0xFF); }
inline void __fastcall S_ReportWeapon(void* audio,void*,int32 ev,int32 wt,void* phys){ T("report_weapon_event",ev,wt,phys); }
inline bool __fastcall S_Buoy(void* b,void*,void* v,float c,CVector* tp,CVector* fo,uint32 uw){
    T("process_buoyancy_boat",v,c,uw&0xFF);
    if(!cfg.buoyRet)return false;
    *tp=CVector{sr.uni(-3.f,3.f),sr.uni(-5.f,5.f),sr.uni(-2.f,2.f)};
    const float mass=At<float>(veh.data(),E_MASS), step=AtA<float>(ADDR_STEP), thr=step*mass*0.0008f;
    const float z=sr.next()%3==0 ? thr*sr.uni(0.2f,1.5f) : sr.uni(0.f,mass*0.01f+1.f);
    *fo=CVector{sr.uni(-50.f,50.f),sr.uni(-50.f,50.f),z};
    AtA<float>(ADDR_BUOYIMM)=sr.next()%8==0 ? 0.f : sr.uni(0.f,1.5f);
    return true; }
inline bool __cdecl S_Water(float x,float y,float z,float* out,uint32 touching,CVector* normals){
    T("get_water_level",x,y,z,touching&0xFF,normals);
    if(cfg.waterWrite)*out=z+sr.uni(-1.2f,1.5f);
    return cfg.waterRet; }
inline void __fastcall S_Wake(void* b,void*,CVector p){ T("add_wake_point",b,V3(p)); }
inline void* __fastcall S_PadFromPlayer(void* p,void*){ T("get_pad_from_player",p); return fakePad.data(); }
inline int16 __fastcall S_HandBrake(void* pad,void*){ T("get_hand_brake",pad); return int16(cfg.handBrake ? 1 : 0); }
inline int16 __fastcall S_GunFired(void* pad,void*){ T("get_car_gun_fired",pad); return int16(cfg.gunFired ? 1 : 0); }
inline void __fastcall S_Breath(void* p,void*,uint32 b,float m){ T("handle_player_breath",p,b&0xFF,m); }
inline void __fastcall S_AudioEvent(void* a,void*,int32 ev,float v){ T("add_audio_event",a,ev,v); }
inline void __fastcall S_BoatWater(void* v,void*,void* bh,float d){ T("apply_boat_water_resistance",v,bh,d); }
inline json WInfo(void* w){ return json::array({At<uint32>(w,0),At<uint32>(w,4),At<uint32>(w,8),At<uint32>(w,0xC)}); }
inline bool __fastcall S_FireHit(void* w,void*,void* e,CVector* o,CVector* m,void* te,void* t,void* dv,uint32 a6,uint32 m2){
    T("fire_instant_hit",WInfo(w),e,V3(*o),V3(*m),o==m,te,t,dv,a6&0xFF,m2&0xFF); return true; }
inline bool __fastcall S_FireProj(void* w,void*,void* e,const CVector* o,void* te,const void* tp,float force){
    T("fire_projectile",WInfo(w),e,V3(*o),te,tp,force); return true; }
inline void __fastcall S_PlaneFx(void* v,void*,void* w,CVector* pp,CVector* sp,int32 idx){ T("do_plane_gun_fire_fx",v,WInfo(w),V3(*pp),V3(*sp),idx); }
inline void __fastcall S_StartShake(void* pad,void*,int32 t,uint32 f,uint32 a){ T("start_shake",pad,int32(int16(t)),f&0xFF,a); }
inline void __fastcall S_FireFixed(void* v,void*){ T("fire_fixed_machine_guns",v); }
inline bool __fastcall S_NoFlyers(void* e,void*){ T("does_not_collide_with_flyers",e); return cfg.noFlyers; }
inline bool __cdecl S_RegRope(uint32 id,uint32 type,CVector pos,uint32 expires,uint32 seg,uint32 flags,void* holder,uint32 t){
    T("register_rope",int64(id)-int64(uintptr_t(veh.data())),type&0xFF,V3(pos),expires&0xFF,seg&0xFF,flags&0xFF,holder,t); return true; }
inline void __fastcall S_RopeRelease(void* rope,void*){ T("rope_release_picked_up_object",rope); }
inline void __fastcall S_RopePick(void* rope,void*,void* e){ T("rope_pick_up_object",rope,e); }
inline void __fastcall S_RopeRemove(void* rope,void*){ T("rope_remove",rope); }
// aspas: la lista y el vehiculo se simulan sin tocar el mundo (solo se anotan los parametros)
inline bool __fastcall S_BladeList(void* self,void*,void* list,void* cm,void* mat,int32 rotor,float dmg){
    T("blade_col_sector_list",self,list,cm==reinterpret_cast<void*>(ADDR_BLADECOL),mat,int32(int16(rotor)),dmg,Fnv(reinterpret_cast<void*>(ADDR_BLADECOL),BLADECOL_SIZE));
    return (sr.next()&7)==0; }

// ======================= fixtures =======================
// valor "interesante": 0, +-1, pequeno, o uniforme
inline float SP(Rng& r,float lo,float hi){ const uint32 m=r.below(10); return m==0 ? 0.f : m==1 ? 1.f : m==2 ? -1.f : m==3 ? r.uni(-0.01f,0.01f) : r.uni(lo,hi); }
// matriz de rotacion (right, forward, up) con posicion; el modo 4 son valores sin ortogonalizar
inline void RotMat(void* m,Rng& r,int mode,float posRange=100.f){
    double yaw=r.uni(-3.14159f,3.14159f), pitch=0, roll=0;
    switch(mode){
    case 0: pitch=r.uni(-0.3f,0.3f); roll=r.uni(-0.3f,0.3f); break;
    case 1: pitch=r.uni(-1.5f,1.5f); roll=r.uni(-3.14f,3.14f); break;
    case 2: pitch=r.uni(-0.4f,0.4f); roll=3.14159+r.uni(-0.5f,0.5f); break; // boca abajo
    case 3: pitch=r.uni(-1.3f,-0.6f); roll=r.uni(-0.4f,0.4f); break;       // morro abajo
    default: break;
    }
    auto* f=static_cast<float*>(m);
    if(mode==4){ for(int row=0;row<3;++row)for(int c=0;c<3;++c)f[row*4+c]=r.uni(-1.5f,1.5f); }
    else {
        const double cy=std::cos(yaw),sy=std::sin(yaw),cp=std::cos(pitch),sp=std::sin(pitch),cr=std::cos(roll),sr_=std::sin(roll);
        const double fx=-sy*cp, fy=cy*cp, fz=sp, rx0=cy, ry0=sy, rz0=0;
        const double ux0=ry0*fz-rz0*fy, uy0=rz0*fx-rx0*fz, uz0=rx0*fy-ry0*fx;
        const double rx=rx0*cr+ux0*sr_, ry=ry0*cr+uy0*sr_, rz=rz0*cr+uz0*sr_, ux=ux0*cr-rx0*sr_, uy=uy0*cr-ry0*sr_, uz=uz0*cr-rz0*sr_;
        f[0]=float(rx); f[1]=float(ry); f[2]=float(rz); f[4]=float(fx); f[5]=float(fy); f[6]=float(fz); f[8]=float(ux); f[9]=float(uy); f[10]=float(uz);
    }
    f[3]=f[7]=f[11]=f[15]=0.f; f[12]=r.uni(-posRange,posRange); f[13]=r.uni(-posRange,posRange); f[14]=r.uni(-posRange,posRange);
}
inline std::string Hash(const void* p,size_t n){ return std::format("{:016x}",Fnv(p,n)); }
// la lista de referencias de una entidad falsa sale del pool global: su direccion no es comparable y se enmascara
inline std::string EntHash(const void* p,size_t n){
    std::vector<uint8> c(static_cast<const uint8*>(p),static_cast<const uint8*>(p)+n); std::memset(c.data()+E_REFS,0,4); return Hash(c.data(),c.size()); }
inline void CleanRefs(){
    auto clean=[](void* b){ auto* e=reinterpret_cast<CEntity*>(b); while(auto* rf=e->m_pReferences)e->CleanUpOldReference(rf->m_ppEntity); };
    clean(veh.data()); clean(otherVeh.data()); clean(drv.data()); clean(col.data()); for(auto& e:ents)clean(e.data());
}
inline json Key(){
    json k=json::array(); auto* v=veh.data();
    for(size_t o:{E_PHYS,E_MOVE,E_MOVE+4,E_MOVE+8,E_TURN,E_TURN+4,E_TURN+8,E_ELAST,V_STEER2,V_ROTORSPEED,V_DOOM,V_FLAGS,V_FLAGS+4,V_BOATFLAGS,V_BOATFORCE,V_BOATFORCE+4,V_BOATFORCE+8})k.push_back(At<uint32>(v,o));
    return k; }
inline json VehOut(){ return {{"veh",EntHash(veh.data(),veh.size())},{"key",Key()}}; }

inline void Models(Rng& r){
    for(size_t k=0;k<USED_MODELS.size();++k){
        auto& mi=fakeMi[k]; auto& vs=fakeVs[k]; std::memset(mi.data(),0xCD,mi.size()); std::memset(vs.data(),0,vs.size());
        At<void*>(mi.data(),0x5C)=vs.data();
        const uint32 m=r.below(10);
        if(m>=4){ At<float>(vs.data(),0x9C)=m==9 ? 0.f : r.uni(-4.f,4.f); At<float>(vs.data(),0xA0)=m==8 ? 0.f : r.uni(-6.f,6.f); At<float>(vs.data(),0xA4)=m==7 ? 0.f : r.uni(-3.f,3.f); }
        else if(m==3){ At<uint32>(vs.data(),0x9C)=0x80000000u; At<uint32>(vs.data(),0xA0)=0; At<uint32>(vs.data(),0xA4)=0x80000000u; } // -0, 0, -0: sigue siendo cero
        AtA<void*>(ADDR_MODELINFOS+4*size_t(USED_MODELS[k]))=mi.data();
    }
}
inline void Handling(Rng& r){
    std::memset(handling.data(),0xCD,handling.size());
    for(size_t o=0;o<0xE0;o+=4)At<float>(handling.data(),o)=SP(r,-1.f,3.f);
    At<float>(handling.data(),0xC0)=r.pct(25) ? 0.f : r.uni(-0.8f,0.8f);
    At<float>(handling.data(),0xA4)=r.uni(-1.f,2.f); At<float>(handling.data(),0xA8)=r.uni(-1.f,2.f); At<float>(handling.data(),0x7C)=r.uni(0.f,3.f);
    At<float>(handling.data(),0x94)=r.uni(0.f,5.f); At<float>(handling.data(),0x98)=r.uni(0.f,1.f); At<float>(handling.data(),0xAC)=r.uni(0.1f,2.f);
}
inline void BoatHandling(Rng& r){
    std::memset(boatHandling.data(),0xCD,boatHandling.size());
    At<int32>(boatHandling.data(),0)=int32(r.below(5));
    for(size_t o=4;o<=0x20;o+=4)At<float>(boatHandling.data(),o)=SP(r,-1.f,3.f);
    for(size_t o=0x24;o<=0x38;o+=4)At<float>(boatHandling.data(),o)=r.pct(10) ? r.pick({0.f,1.f}) : r.uni(0.3f,1.2f);
}
// modelo de colision falso: caja (min en 0, max en 0xC) y esfera (centro en 0x18)
inline void ColModel(void* cm,Rng& r,bool tiny){
    std::memset(cm,0xCD,0x40);
    for(size_t k=0;k<3;++k){ At<float>(cm,4*k)=-r.uni(0.f,tiny?1.f:4.f); At<float>(cm,0xC+4*k)=r.uni(tiny&&r.pct(60)?0.1f:0.f,tiny?1.4f:4.f); }
    for(size_t k=0;k<4;++k)At<float>(cm,0x18+4*k)=r.uni(-3.f,3.f);
}
inline void Globals(Rng& r,bool wideTime=true){
    AtA<float>(ADDR_STEP)=r.pct(60) ? 1.f : r.pick({0.f,0.005f,0.02f,2.5f,0.1f,3.f}); if(r.pct(25))AtA<float>(ADDR_STEP)=r.uni(0.05f,3.f);
    AtA<uint32>(ADDR_TIME)=wideTime&&r.pct(30) ? r.next() : r.below(2000000);
}
// vehiculo falso comun
inline void PrepVeh(Rng& r,int32 type,int32 sub,int32 model,int32 status,int matMode){
    auto* v=veh.data(); std::memset(v,0xCD,veh.size());
    At<void*>(v,0)=trapVt.data();
    RotMat(vehMat.data(),r,matMode);
    At<void*>(v,E_MATRIX)=vehMat.data();
    At<uint32>(v,E_FLAGS)=r.next(); At<uint16>(v,0x20)=uint16(r.next());
    At<int16>(v,E_MODEL)=int16(model);
    At<void*>(v,E_REFS)=nullptr;
    At<uint8>(v,E_TYPE)=uint8(2|(status<<3));
    At<uint32>(v,E_PHYS)=r.next();
    for(size_t k=0;k<3;++k){ At<float>(v,E_MOVE+4*k)=SP(r,-2.f,2.f); At<float>(v,E_TURN+4*k)=SP(r,-2.f,2.f); At<float>(v,E_COM+4*k)=r.uni(-1.f,1.f); }
    At<float>(v,E_MASS)=r.uni(200.f,4000.f); At<float>(v,E_TURNMASS)=r.uni(300.f,9000.f); At<float>(v,E_ELAST)=r.uni(0.f,1.f); At<float>(v,E_BUOY)=r.uni(0.f,2.f);
    At<void*>(v,V_HANDLING)=handling.data();
    At<void*>(v,V_DRIVER)=nullptr; for(size_t k=0;k<8;++k)At<void*>(v,V_PASS+4*k)=nullptr;
    At<void*>(v,V_TOWING)=nullptr; At<void*>(v,V_TOWED)=nullptr; At<void*>(v,E_ATTACHED)=nullptr; At<void*>(v,E_IGNORED)=nullptr;
    At<int32>(v,V_TYPE)=type; At<int32>(v,V_SUBTYPE)=sub;
    for(size_t k=0;k<8;++k)At<uint8>(v,V_FLAGS+k)=uint8(r.next());
}
inline void PrepPed(void* p,Rng& r,int32 pedType){
    std::memset(p,0xCD,ENTSZ);
    At<void*>(p,0)=trapVt.data();
    At<uint8>(p,E_TYPE)=uint8(3|(r.below(13)<<3)); At<uint32>(p,E_FLAGS)=r.next(); At<uint32>(p,E_PHYS)=r.next(); At<void*>(p,E_REFS)=nullptr;
    At<int32>(p,P_PEDTYPE)=pedType; At<uint32>(p,P_FLAGS1)=r.next(); At<uint32>(p,P_FLAGS2)=r.next();
    At<void*>(p,P_INTEL)=intel.data(); At<void*>(p,E_ATTACHED)=nullptr; At<void*>(p,P_VEHICLE)=nullptr; At<void*>(p,E_IGNORED)=nullptr;
    At<float>(p,E_BRIGHT)=r.uni(0.f,1.f);
}

// ======================= funciones =======================
inline json RunApplyBoat(uintptr_t a,Rng& r){
    Patches P; P.Both(0x542A50,S_TurnForce,Addr(&CPhysical::ApplyTurnForce));
    PrepVeh(r,5,5,r.pct(30)?460:411,int32(r.below(13)),int(r.below(5))); Globals(r); Handling(r); BoatHandling(r);
    const float depth=r.pct(15) ? r.pick({0.f,1.f,-0.5f}) : r.uni(-0.5f,2.5f);
    reinterpret_cast<void(__thiscall*)(void*,void*,float)>(a)(veh.data(),boatHandling.data(),depth);
    json o=VehOut(); o["trace"]=trace; return o;
}

inline json RunProcessBoat(uintptr_t a,Rng& r){
    Patches P;
    P.Both(0x5429F0,S_MoveForce,Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)));
    P.Both(0x542A50,S_TurnForce,Addr(&CPhysical::ApplyTurnForce));
    P.Both(0x6D2740,S_BoatWater,Addr(&CVehicle::ApplyBoatWaterResistance));
    P.Both(0x535300,S_GetColModel,Addr(&CEntity::GetColModel));
    P.Both(0x6EB690,S_Water,Addr(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel)));
    P.Both(0x6F2550,S_Wake,Addr(&CBoat::AddWakePoint));
    P.Both(0x609560,S_PadFromPlayer,Addr(&CPlayerPed::GetPadFromPlayer));
    P.Both(0x540040,S_HandBrake,Addr(&CPad::GetHandBrake));
    P.Both(0x60A8D0,S_Breath,Addr(&CPlayerPed::HandlePlayerBreath));
    P.Both(0x4F6420,S_AudioEvent,Addr(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)));
    P.Add(0x4B35A0,S_Affects); P.Add(DllBody("CEventDamage","AffectsPed"),S_Affects);
    P.Add(0x4B5AC0,S_ComputeResp);
    P.Both(0x4AB420,S_EvAdd,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)));
    P.Both(0x6C3030,S_Buoy,Addr(&cBuoyancy::ProcessBuoyancyBoat));
    const uint32 kind=r.below(10);
    const int32 type=kind<6 ? 5 : kind<9 ? 0 : 4, sub=kind<6 ? 5 : kind<8 ? 0 : kind<9 ? 4 : 5;
    const int32 model=r.pct(25) ? 460 : r.pickI({411,430,500});
    const int32 status=r.pct(35) ? 0 : r.pickI({3,8,2,12,4});
    const int mode=r.pct(25) ? 2 : r.pct(15) ? 3 : int(r.below(2));
    PrepVeh(r,type,sub,model,status,mode); Globals(r,false); Handling(r); BoatHandling(r);
    auto* v=veh.data();
    if(r.pct(40))for(size_t k=0;k<2;++k)At<float>(v,E_MOVE+4*k)=r.uni(-0.06f,0.06f);
    At<float>(v,V_STEER)=SP(r,-1.f,1.f); At<float>(v,V_STEER2)=r.pct(50) ? 0.f : r.pct(50) ? float(AtA<uint32>(ADDR_TIME))+r.uni(-500.f,500.f) : r.uni(-2.f,2.f);
    At<float>(v,V_GAS)=r.pct(60) ? r.pick({0.f,0.005f,-0.005f,0.03f,-0.03f,0.3f,-0.3f,0.7f,-0.7f,1.f,-1.f,0.05f,-0.05f,0.01f,0.5f}) : r.uni(-1.f,1.f);
    At<uint8>(v,V_BOATFLAGS)=uint8(r.next()); At<float>(v,V_BOATFORCE)=0.f; At<float>(v,V_BOATFORCE+4)=0.f; At<float>(v,V_BOATFORCE+8)=0.f; At<float>(v,V_DOOM)=0.f;
    ColModel(vehCM.data(),r,false); colMapCount=1; colMapEnt[0]=veh.data(); colMapCM[0]=vehCM.data();
    const uint32 dk=r.below(10);
    if(dk>=3){ PrepPed(drv.data(),r,dk<6 ? int32(r.below(2)) : 2+int32(r.below(18))); At<void*>(v,V_DRIVER)=drv.data(); }
    std::memset(intel.data(),0xCD,intel.size());
    cfg={}; cfg.buoyRet=!r.pct(8); cfg.waterRet=r.pct(70); cfg.waterWrite=r.pct(65); cfg.affects=r.pct(50); cfg.handBrake=r.pct(50);
    lastDepth=r.pct(20) ? 0.f : r.uni(0.f,1.5f);
    reinterpret_cast<void(__thiscall*)(void*,void*,float*,uint32,uint32)>(a)(v,boatHandling.data(),&lastDepth,r.below(2),r.below(2));
    json o=VehOut(); o["drv"]=EntHash(drv.data(),drv.size()); o["last"]=F(lastDepth); o["trace"]=trace; return o;
}

// lista de nodos del pool real (el pool se restaura al empezar cada ejecucion)
inline void MakeList(const std::vector<void*>& items,uint32& head){
    auto* pool=GetPtrNodeSingleLinkPool(); void* prev=nullptr; head=0;
    for(size_t k=items.size();k-->0;){
        void* n=pool->New(); At<void*>(n,0)=items[k]; At<void*>(n,4)=prev; prev=n; }
    head=uint32(reinterpret_cast<uintptr_t>(prev));
}
inline json RunBlade(uintptr_t a,Rng& r){
    Patches P;
    P.Both(0x535300,S_GetColModel,Addr(&CEntity::GetColModel));
    P.Both(0x4C6F70,S_AnimCM,Addr(&CPedModelInfo::AnimatePedColModelSkinned));
    P.Both(0x4185C0,S_ProcessCol,Addr(&CCollision::ProcessColModels));
    P.Both(0x5DEF60,S_LocalDir,Addr(&CPed::GetLocalDirection));
    P.Both(0x5429F0,S_MoveForce,Addr(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)));
    P.Both(0x542A50,S_TurnForce,Addr(&CPhysical::ApplyTurnForce));
    P.Both(0x543890,S_SoftCol,Addr(static_cast<bool(CPhysical::*)(CEntity*,const CColPoint&,float&)>(&CPhysical::ApplySoftCollision)));
    P.Both(0x5428C0,S_DamagedPiece,Addr(&CPhysical::SetDamagedPieceRecord));
    P.Both(0x49F040,S_Sparks,Addr(&Fx_c::AddSparks));
    P.Both(0x49EB00,S_Blood,Addr(&Fx_c::AddBlood));
    P.Add(0x4B35A0,S_Affects); P.Add(DllBody("CEventDamage","AffectsPed"),S_Affects);
    P.Add(0x4B5AC0,S_ComputeResp);
    P.Both(0x4AB420,S_EvAdd,Addr(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)));
    P.Both(0x56D230,S_LocBlood,Addr(&CLocalisation::Blood));
    P.Both(0x534540,S_OnScreen,Addr(&CEntity::GetIsOnScreen));
    P.Both(0x50AE50,S_CamPos,Addr(&CCamera::GetGameCamPosition));
    P.Both(0x506EB0,S_ReportCol,Addr(&CAudioEngine::ReportCollision));
    P.Add(0x821B1E,S_Rand); P.Add(Body(Addr(&CGeneral::GetRandomNumber)),S_RandomNumber);
    PrepVeh(r,r.pct(60)?0:r.pickI({9,5,4}),0,r.pickI({411,500}),r.pct(30)?0:r.pct(30)?8:int32(r.below(13)),int(r.below(2))); Globals(r);
    auto* v=veh.data();
    At<float>(v,V_ROTORSPEED)=r.pct(70) ? r.pick({0.f,0.05f,0.07f,0.074f,0.075f,0.0751f,0.1f,0.15f,0.1501f,0.3f,-0.05f,-0.1f}) : r.uni(-0.2f,0.4f);
    AtA<uint32>(ADDR_ROTORTIME)=r.pct(20) ? r.next() : AtA<uint32>(ADDR_TIME)+uint32(r.pickI({-2001,-2000,-1999,-1000,-1,0,1,1000,3000}));
    AtA<uint16>(ADDR_SCAN)=uint16(1+r.below(60000));
    RotMat(bladeMat.data(),r,r.pct(60)?0:int(r.below(5))); ColModel(bladeCM.data(),r,false); ColModel(pedCM.data(),r,false);
    const int16 rotor=int16(r.pickI({-3,-3,-3,-3,-2,-1,1,2,3}));
    const float dmg=r.pct(30) ? r.pick({0.f,1.f,2.f}) : r.uni(0.f,3.f);
    camPos=CVector{r.uni(-5.f,5.f),r.uni(-5.f,5.f),r.uni(-5.f,5.f)};
    cfg={}; cfg.affects=r.pct(50); cfg.bloodOn=r.pct(60); cfg.onScreen=r.pct(60); cfg.softRet=r.pct(50); cfg.softDmg=r.pct(30) ? r.uni(0.f,50.f) : r.uni(0.f,2000.f);
    cfg.numColls=r.pickI({-1,0,1,1,2,3,5,32}); cfg.localDir=r.below(4); cfg.animRet=r.pct(80) ? pedCM.data() : nullptr;
    sr.s=r.next()|1u;
    std::memset(intel.data(),0xCD,intel.size());
    const uint32 n=1+r.below(NENT);
    std::vector<void*> items; colMapCount=0;
    for(uint32 k=0;k<n;++k){
        auto* e=ents[k].data(); std::memset(e,0xCD,ENTSZ);
        const uint32 kk=r.below(100); const int32 kind=kk<40 ? 3 : kk<65 ? 2 : kk<85 ? 4 : 1;
        At<void*>(e,0)=trapVt.data();
        At<uint8>(e,E_TYPE)=uint8(kind|(r.below(13)<<3));
        At<uint32>(e,E_FLAGS)=(r.next()&~1u)|(r.pct(85)?1u:0u);
        At<uint16>(e,E_SCAN)=r.pct(15) ? AtA<uint16>(ADDR_SCAN) : uint16(r.next());
        RotMat(entMats[k].data(),r,int(r.below(2))); At<void*>(e,E_MATRIX)=entMats[k].data();
        At<void*>(e,E_CLUMP)=clumpBuf.data();
        At<int16>(e,E_MODEL)=int16(kind==3 ? 19990 : r.pct(15) ? 345 : r.pickI({411,500}));
        At<void*>(e,E_REFS)=nullptr; At<uint32>(e,E_PHYS)=r.next(); At<void*>(e,E_ATTACHED)=r.pct(50) ? nullptr : veh.data(); At<void*>(e,E_IGNORED)=nullptr;
        At<int32>(e,P_PEDTYPE)=2+int32(r.below(18)); At<uint32>(e,P_FLAGS1)=r.next(); At<uint32>(e,P_FLAGS2)=r.next(); At<void*>(e,P_INTEL)=intel.data(); At<void*>(e,P_VEHICLE)=nullptr;
        At<float>(e,E_BRIGHT)=r.uni(0.f,1.f);
        At<uint8>(e,O_TYPE)=uint8(r.pct(30) ? 3 : r.below(8)); At<uint8>(e,O_COLDMG)=uint8(r.below(30));
        ColModel(entCMs[k].data(),r,false); colMapEnt[k]=e; colMapCM[k]=r.pct(88) ? entCMs[k].data() : nullptr; colMapCount=int32(k+1);
        items.push_back(e);
    }
    if(r.pct(10))items.insert(items.begin()+int(r.below(uint32(items.size()+1))),veh.data());
    uint32 head{}; MakeList(items,head);
    // Como el original, la funcion devuelve siempre false
    const bool ret=reinterpret_cast<bool(__thiscall*)(void*,void*,void*,void*,int32,float)>(a)(v,&head,bladeCM.data(),bladeMat.data(),rotor,dmg);
    json o=VehOut(); o["ret"]=ret; for(uint32 k=0;k<n;++k)o["e"+std::to_string(k)]=EntHash(ents[k].data(),ENTSZ);
    o["tempcol"]=Hash(reinterpret_cast<void*>(ADDR_TEMPCOL),0x2C*32); o["rotortime"]=AtA<uint32>(ADDR_ROTORTIME); o["scan"]=AtA<uint16>(ADDR_SCAN);
    o["trace"]=trace; return o;
}

inline json RunDoBlade(uintptr_t a,Rng& r){
    Patches P;
    P.Add(0x6DAF00,S_BladeList);
    P.Add(Body(Addr(static_cast<bool(CVehicle::*)(CPtrListSingleLink<CBuilding*>&,CColModel&,CMatrix&,int16,float)>(&CVehicle::BladeColSectorList<CPtrListSingleLink<CBuilding*>>))),S_BladeList);
    P.Add(Body(Addr(static_cast<bool(CVehicle::*)(CPtrListDoubleLink<CVehicle*>&,CColModel&,CMatrix&,int16,float)>(&CVehicle::BladeColSectorList<CPtrListDoubleLink<CVehicle*>>))),S_BladeList);
    P.Add(Body(Addr(static_cast<bool(CVehicle::*)(CPtrListDoubleLink<CPed*>&,CColModel&,CMatrix&,int16,float)>(&CVehicle::BladeColSectorList<CPtrListDoubleLink<CPed*>>))),S_BladeList);
    P.Add(Body(Addr(static_cast<bool(CVehicle::*)(CPtrListDoubleLink<CObject*>&,CColModel&,CMatrix&,int16,float)>(&CVehicle::BladeColSectorList<CPtrListDoubleLink<CObject*>>))),S_BladeList);
    P.Add(0x563470,S_ClearScan); P.Add(Body(Addr(&CWorld::ClearScanCodes)),S_ClearScan);
    PrepVeh(r,0,0,411,int32(r.below(13)),int(r.below(2))); Globals(r); sr.s=r.next()|1u;
    AtA<uint16>(ADDR_SCAN)=r.pct(20) ? uint16(0xFFFF) : uint16(r.below(65535));
    // la matriz recibida (no la del vehiculo) decide el sector; posiciones cerca de los bordes del mundo recortan el rango
    const float range=r.pct(20) ? 3300.f : r.pct(20) ? 3050.f : 200.f;
    RotMat(bladeMat.data(),r,r.pct(70)?0:int(r.below(5)),range);
    const CVector pos{r.uni(-8.f,8.f),r.uni(-8.f,8.f),r.uni(-8.f,8.f)};
    const float radius=r.pct(15) ? r.pick({0.f,0.4f,1.f}) : r.pct(15) ? r.uni(60.f,200.f) : r.uni(0.2f,45.f);
    const int32 rotor=r.pickI({-3,-3,-2,-1,0,1,2,3,7,-5});
    const float dmg=r.pct(30) ? r.pick({0.f,1.f,2.f}) : r.uni(0.f,3.f);
    const bool ret=reinterpret_cast<bool(__thiscall*)(void*,CVector,void*,int32,float,float)>(a)(veh.data(),pos,bladeMat.data(),rotor,radius,dmg);
    json o=VehOut(); o["ret"]=ret; o["scan"]=AtA<uint16>(ADDR_SCAN); o["blade"]=Hash(reinterpret_cast<void*>(ADDR_BLADECOL),BLADECOL_SIZE);
    o["mat"]=Hash(bladeMat.data(),0x40); o["trace"]=trace; return o;
}

inline json RunDoFixed(uintptr_t a,Rng& r){
    Patches P; P.Both(0x53FF90,S_GunFired,Addr(&CPad::GetCarGunFired)); P.Both(0x73DF00,S_FireFixed,Addr(&CVehicle::FireFixedMachineGuns));
    PrepVeh(r,0,0,411,int32(r.below(13)),0); Globals(r);
    AtA<int32>(ADDR_CAMDIR+size_t(AtA<uint8>(ADDR_ACTIVECAM))*0x238)=r.pct(65) ? 3 : int32(r.below(8));
    const uint32 dk=r.below(10);
    if(dk>=4){ PrepPed(drv.data(),r,dk<7 ? 1 : dk<9 ? 0 : 2+int32(r.below(18))); At<void*>(veh.data(),V_DRIVER)=drv.data(); }
    const uint32 now=AtA<uint32>(ADDR_TIME);
    At<uint32>(veh.data(),V_FIRETIME)=r.pct(50) ? now-uint32(r.pickI({1400,1401,1399,0,1,5000,100})) : r.next();
    cfg={}; cfg.gunFired=r.pct(50);
    reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
    json o=VehOut(); o["trace"]=trace; return o;
}

// armas del avion: Hunter, Seasparrow, RC Baron, Rustler, Maverick, Polmav, Hydra, Cargobob, Tornado (+ otros)
inline int32 PlaneModel(Rng& r){ return r.pct(88) ? r.pickI({425,447,464,476,487,497,520,548,576}) : r.pickI({460,411,500,19990,345,441,586}); }
inline void PrepPlane(Rng& r,int32 status){
    PrepVeh(r,0,r.pickI({3,4,4,0}),PlaneModel(r),status,int(r.below(3))); Globals(r); Models(r);
    At<uint8>(veh.data(),V_PLANEFIRE)=r.pct(10) ? 0 : uint8(r.below(256)); At<uint8>(veh.data(),V_HELIFIRE)=r.pct(10) ? 0 : uint8(r.below(256));
    At<uint8>(veh.data(),V_GUNBYTE)=uint8(r.next()); At<uint8>(veh.data(),V_WEAPONINUSE)=uint8(r.below(6));
}
inline json RunFirePlane(uintptr_t a,Rng& r){
    Patches P;
    P.Both(0x73FB10,S_FireHit,Addr(&CWeapon::FireInstantHit));
    P.Both(0x6D4AD0,S_PlaneFx,Addr(&CVehicle::DoPlaneGunFireFX));
    P.Both(0x53F920,S_StartShake,Addr(&CPad::StartShake));
    P.Both(0x506F40,S_ReportWeapon,Addr(&CAudioEngine::ReportWeaponEvent));
    PrepPlane(r,r.pct(25) ? 8 : int32(r.below(13)));
    const uint32 now=AtA<uint32>(ADDR_TIME);
    At<uint32>(veh.data(),V_FIRETIME)=r.pct(60) ? now-uint32(r.pickI({0,1,40,41,50,60,61,80,81,100,101,150,226,500,5000})) : r.next();
    const uint32 dk=r.below(10);
    if(dk>=3){ PrepPed(drv.data(),r,dk<6 ? int32(r.below(2)) : 2+int32(r.below(18))); At<void*>(veh.data(),V_DRIVER)=drv.data(); }
    cfg={};
    reinterpret_cast<void(__thiscall*)(void*)>(a)(veh.data());
    json o=VehOut(); o["drv"]=EntHash(drv.data(),drv.size()); o["trace"]=trace; return o;
}
inline json RunFireMissile(uintptr_t a,Rng& r){
    Patches P;
    P.Both(0x741360,S_FireProj,Addr(&CWeapon::FireProjectile));
    P.Both(0x53F920,S_StartShake,Addr(&CPad::StartShake));
    PrepPlane(r,int32(r.below(13)));
    const uint32 now=AtA<uint32>(ADDR_TIME);
    for(size_t o:{V_PROJTIME,V_PROJTIME2})At<uint32>(veh.data(),o)=r.pct(60) ? now-uint32(r.pickI({0,1,350,351,500,501,1000,1001,2000,5000})) : r.next();
    const uint32 dk=r.below(10);
    if(dk>=3){ PrepPed(drv.data(),r,dk<6 ? int32(r.below(2)) : 2+int32(r.below(18))); At<void*>(veh.data(),V_DRIVER)=drv.data(); }
    cfg={};
    reinterpret_cast<void(__thiscall*)(void*,int32,uint32)>(a)(veh.data(),r.pickI({1,2,0,3,1,2}),r.below(2));
    json o=VehOut(); o["drv"]=EntHash(drv.data(),drv.size()); o["trace"]=trace; return o;
}
inline json RunPlaneSimple(const std::string& nm,uintptr_t a,Rng& r){
    PrepPlane(r,int32(r.below(13)));
    json o; auto* v=veh.data();
    if(nm=="GetPlaneGunsAutoAimAngle")o["ret"]=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(v));
    else if(nm=="GetPlaneNumGuns")o["ret"]=reinterpret_cast<int32(__thiscall*)(void*)>(a)(v);
    else if(nm=="GetFiringRateMultiplier")o["ret"]=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(v));
    else if(nm=="SetFiringRateMultiplier"){ const float m=r.pct(50) ? r.pick({0.f,-0.f,0.01f,0.5f,1.f,15.9375f,16.f,100.f,-5.f,0.0624f,0.0625f,15.93f}) : r.uni(-3.f,20.f); reinterpret_cast<void(__thiscall*)(void*,float)>(a)(v,m); }
    else if(nm=="GetPlaneGunsRateOfFire")o["ret"]=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(v);
    else if(nm=="GetPlaneGunsPosition"||nm=="GetPlaneOrdnancePosition"){
        CVector out; At<uint32>(&out,0)=0xCDCDCDCD; At<uint32>(&out,4)=0xCDCDCDCD; At<uint32>(&out,8)=0xCDCDCDCD;
        const int32 arg=nm=="GetPlaneGunsPosition" ? int32(r.below(6))-1 : r.pickI({0,1,2,3,-1});
        reinterpret_cast<void(__thiscall*)(void*,CVector*,int32)>(a)(v,&out,arg); o["ret"]=V3(out); }
    else if(nm=="GetPlaneOrdnanceRateOfFire")o["ret"]=reinterpret_cast<uint32(__thiscall*)(void*,int32)>(a)(v,r.pickI({0,1,2,3,-1}));
    else if(nm=="SelectPlaneWeapon")reinterpret_cast<void(__thiscall*)(void*,uint32,int32)>(a)(v,r.below(2),r.pickI({0,1,2,3,-1}));
    json k=VehOut(); o["veh"]=k["veh"]; o["trace"]=trace; return o;
}

// cuerdas: 8 cuerdas con bytes aleatorios; la del vehiculo (id = this + 0x1D, o this + 0xA320 en RemoveWinch) puede existir o no
inline void PrepRopes(Rng& r,uint32 id,bool found,bool inactiveCopies){
    for(size_t i=0;i<8;++i){ auto* rp=reinterpret_cast<uint8*>(ADDR_ROPES+i*0x328); for(size_t k=0;k<0x328;++k)rp[k]=uint8(r.next());
        At<uint32>(rp,0x300)=r.pct(30) ? id+uint32(r.below(3)) : r.next(); At<uint8>(rp,0x325)=uint8(r.below(9));
        At<float>(rp,0x308)=r.uni(0.f,5.f); At<float>(rp,0x30C)=r.uni(0.f,0.2f); At<float>(rp,0x31C)=SP(r,-10.f,12.f);
        At<void*>(rp,0x318)=r.pct(50) ? nullptr : r.pct(50) ? veh.data() : otherVeh.data(); }
    if(found){ auto* rp=reinterpret_cast<uint8*>(ADDR_ROPES+size_t(r.below(8))*0x328); At<uint32>(rp,0x300)=id; At<uint8>(rp,0x325)=uint8(1+r.below(8)); }
    if(inactiveCopies){ auto* rp=reinterpret_cast<uint8*>(ADDR_ROPES+size_t(r.below(8))*0x328); if(At<uint32>(rp,0x300)!=id){ At<uint32>(rp,0x300)=id; At<uint8>(rp,0x325)=0; } }
}
inline json RunWinch(const std::string& nm,uintptr_t a,Rng& r){
    Patches P;
    P.Both(0x556B40,S_RegRope,Addr(&CRopes::RegisterRope));
    P.Both(0x556030,S_RopeRelease,Addr(&CRope::ReleasePickedUpObject));
    P.Both(0x5569C0,S_RopePick,Addr(&CRope::PickUpObject));
    P.Both(0x556780,S_RopeRemove,Addr(&CRope::Remove));
    PrepVeh(r,0,0,411,int32(r.below(13)),int(r.below(5))); Globals(r);
    std::memset(otherVeh.data(),0xCD,otherVeh.size()); At<void*>(otherVeh.data(),E_REFS)=nullptr;
    auto* v=veh.data(); json o;
    const uint32 ropeId=uint32(reinterpret_cast<uintptr_t>(v))+0x1D;
    if(nm=="InitWinch"){ PrepRopes(r,ropeId,false,false); reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(v,r.pickI({0,1,2,3,4,5,-1,7})); }
    else if(nm=="UpdateWinch"){ PrepRopes(r,ropeId,r.pct(60),r.pct(30)); At<uint8>(v,V_ROPEBYTE)=uint8(r.next()); reinterpret_cast<void(__thiscall*)(void*)>(a)(v); }
    else if(nm=="RemoveWinch"){ PrepRopes(r,uint32(reinterpret_cast<uintptr_t>(v))+0xA320,r.pct(60),r.pct(30)); At<uint8>(v,V_ROPEBYTE)=uint8(r.next()); reinterpret_cast<void(__thiscall*)(void*)>(a)(v); }
    else { // las funciones restantes buscan la cuerda sin comprobar el resultado: siempre existe
        PrepRopes(r,ropeId,true,r.pct(30));
        if(nm=="PickUpEntityWithWinch")reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(v,r.pct(25) ? nullptr : r.pct(50) ? static_cast<void*>(otherVeh.data()) : static_cast<void*>(drv.data()));
        else if(nm=="QueryPickedUpEntityWithWinch")o["ret"]=Name(uintptr_t(reinterpret_cast<void*(__thiscall*)(void*)>(a)(v)));
        else if(nm=="ReleasePickedUpEntityWithWinch")reinterpret_cast<void(__thiscall*)(void*)>(a)(v);
        else if(nm=="GetRopeHeightForHeli")o["ret"]=F(reinterpret_cast<float(__thiscall*)(void*)>(a)(v));
        else if(nm=="SetRopeHeightForHeli")reinterpret_cast<void(__thiscall*)(void*,float)>(a)(v,SP(r,-10.f,12.f));
    }
    json k=VehOut(); o["veh"]=k["veh"]; o["ropes"]=Hash(reinterpret_cast<void*>(ADDR_ROPES),0x328*8); o["trace"]=trace; return o;
}

inline json RunSteps(uintptr_t a,Rng& r){
    Patches P; P.Both(0x535300,S_GetColModel,Addr(&CEntity::GetColModel));
    PrepVeh(r,r.pct(25)?9:r.pickI({0,5,1}),0,411,r.pct(50)?0:int32(r.below(13)),int(r.below(5))); Globals(r);
    auto* v=veh.data();
    for(size_t k=0;k<3;++k)At<float>(v,E_MOVE+4*k)=r.pct(60) ? r.uni(-2.f,2.f) : SP(r,-4.f,4.f);
    if(r.pct(20))At<uint32>(v,E_PHYS)|=4;
    ColModel(vehCM.data(),r,false); colMapCount=1; colMapEnt[0]=v; colMapCM[0]=vehCM.data();
    std::array<uint8,2> flags{uint8(r.below(2)),uint8(r.below(2))};
    const uint8 ret=reinterpret_cast<uint8(__thiscall*)(void*,uint8*,uint8*)>(a)(v,&flags[0],&flags[1]);
    json o=VehOut(); o["ret"]=int(ret); o["flags"]=json::array({flags[0],flags[1]}); o["trace"]=trace; return o;
}
inline json RunPre(uintptr_t a,Rng& r){
    Patches P;
    P.Both(0x535300,S_GetColModel,Addr(&CEntity::GetColModel));
    P.Both(0x532D40,S_NoFlyers,Addr(&CEntity::DoesNotCollideWithFlyers));
    const int32 vmodel=r.pct(40) ? r.pickI({441,564,594,406,486,530}) : 411;
    PrepVeh(r,r.pickI({0,9,5}),0,vmodel,r.pct(35)?0:r.pickI({8,3,12,2}),int(r.below(2)));
    auto* v=veh.data(); std::memset(otherVeh.data(),0xCD,otherVeh.size()); At<void*>(otherVeh.data(),E_REFS)=nullptr;
    const int32 ctype=r.pickI({3,2,4,4,2,1});
    auto* c=col.data(); std::memset(c,0xCD,ENTSZ);
    At<void*>(c,0)=trapVt.data(); RotMat(colMat.data(),r,int(r.below(2))); At<void*>(c,E_MATRIX)=colMat.data();
    At<uint32>(c,E_FLAGS)=r.next(); At<int16>(c,E_MODEL)=int16(r.pct(40) ? r.pickI({441,564,594,374,377,381,406,486,530,411}) : 411+int32(r.below(20)));
    At<void*>(c,E_REFS)=nullptr; At<uint8>(c,E_TYPE)=uint8(ctype|(r.below(13)<<3)); At<uint32>(c,E_PHYS)=r.next();
    auto ptrPick=[&]()->void*{ switch(r.below(5)){ case 0:return v; case 1:return c; case 2:return otherVeh.data(); default:return nullptr; } };
    At<void*>(c,E_ATTACHED)=ptrPick(); At<void*>(c,E_IGNORED)=ptrPick(); At<void*>(v,E_ATTACHED)=ptrPick(); At<void*>(v,E_IGNORED)=ptrPick();
    At<void*>(v,V_TOWING)=ptrPick(); At<void*>(v,V_TOWED)=ptrPick();
    // bits relevantes con probabilidad alta: 2 desactiva la fuerza de colision, 3 colisionable, 5/6/7 inmovible, 24 no colisiona con voladores
    for(auto* e:{v,c}){ auto& ph=At<uint32>(e,E_PHYS); for(uint32 bit:{2u,3u,5u,6u,7u,24u}){ ph&=~(1u<<bit); if(r.pct(25))ph|=1u<<bit; } }
    for(auto* e:{v,c}){ auto& fl=At<uint32>(e,E_FLAGS); fl&=~0x10u; if(r.pct(35))fl|=0x10u; }
    if(ctype==3){ if(r.pct(50))At<uint32>(c,P_FLAGS2)|=0x80000000u; else At<uint32>(c,P_FLAGS2)&=~0x80000000u; At<void*>(c,P_VEHICLE)=r.pct(60) ? static_cast<void*>(v) : static_cast<void*>(otherVeh.data()); }
    if(ctype==4){ At<uint8>(c,O_TYPE)=uint8(r.pct(30) ? 3 : r.below(8)); auto& of=At<uint32>(c,O_FLAGS); of=r.next(); of&=~0x140u; if(r.pct(40))of|=0x100u; if(r.pct(25))of|=0x40u; At<uint8>(c,O_COLDMG)=uint8(r.pct(30) ? 20 : r.below(30)); }
    ColModel(colCM.data(),r,true); colMapCount=1; colMapEnt[0]=c; colMapCM[0]=colCM.data();
    cfg={}; cfg.noFlyers=r.pct(50);
    std::array<uint8,4> o4{uint8(r.below(2)),uint8(r.below(2)),uint8(r.below(2)),uint8(r.below(2))};
    reinterpret_cast<void(__thiscall*)(void*,void*,uint32,uint8*,uint8*,uint8*,uint8*)>(a)(v,c,r.below(2),&o4[0],&o4[1],&o4[2],&o4[3]);
    json o=VehOut(); o["col"]=EntHash(c,ENTSZ); o["out"]=json::array({o4[0],o4[1],o4[2],o4[3]}); o["colmat"]=Hash(colMat.data(),0x40); o["vehmat"]=Hash(vehMat.data(),0x40); o["trace"]=trace; return o;
}

// comprobacion de la disposicion de campos de la DLL frente a los desplazamientos del EXE
inline std::string LayoutErrors(){
    std::string why;
    auto chk=[&](const char* nm,uintptr_t got,uintptr_t want){ if(got!=want){ why+=nm; why+=" "; } };
    auto off=[](const void* addr,const void* base){ return uintptr_t(addr)-uintptr_t(base); };
    alignas(16) static std::array<uint8,VEHSZ> t{};
    alignas(16) static std::array<uint8,ENTSZ> t2{};
    auto* v=reinterpret_cast<CVehicle*>(t.data()); auto* b=reinterpret_cast<CBoat*>(t.data()); auto* au=reinterpret_cast<CAutomobile*>(t.data());
    auto* pl=reinterpret_cast<CPlane*>(t.data()); auto* he=reinterpret_cast<CHeli*>(t.data());
    chk("m_matrix",off(&v->m_matrix,v),E_MATRIX); chk("m_nModelIndex",off(&v->m_nModelIndex,v),E_MODEL); chk("m_pReferences",off(&v->m_pReferences,v),E_REFS);
    chk("m_vecMoveSpeed",off(&v->m_vecMoveSpeed,v),E_MOVE); chk("m_vecTurnSpeed",off(&v->m_vecTurnSpeed,v),E_TURN); chk("m_fMass",off(&v->m_fMass,v),E_MASS);
    chk("m_fTurnMass",off(&v->m_fTurnMass,v),E_TURNMASS); chk("m_fElasticity",off(&v->m_fElasticity,v),E_ELAST); chk("m_fBuoyancyConstant",off(&v->m_fBuoyancyConstant,v),E_BUOY);
    chk("m_vecCentreOfMass",off(&v->m_vecCentreOfMass,v),E_COM); chk("m_pAttachedTo",off(&v->m_pAttachedTo,v),E_ATTACHED); chk("m_pEntityIgnoredCollision",off(&v->m_pEntityIgnoredCollision,v),E_IGNORED);
    chk("m_fContactSurfaceBrightness",off(&v->m_fContactSurfaceBrightness,v),E_BRIGHT); chk("m_vehicleAudio",off(&v->m_vehicleAudio,v),V_AUDIO);
    chk("m_pHandlingData",off(&v->m_pHandlingData,v),V_HANDLING); chk("m_fSteerAngle",off(&v->m_fSteerAngle,v),V_STEER); chk("m_f2ndSteerAngle",off(&v->m_f2ndSteerAngle,v),V_STEER2);
    chk("m_GasPedal",off(&v->m_GasPedal,v),V_GAS); chk("m_nAmmoInClip",off(&v->m_nAmmoInClip,v),V_AMMOCLIP); chk("m_pTowingVehicle",off(&v->m_pTowingVehicle,v),V_TOWING);
    chk("m_pVehicleBeingTowed",off(&v->m_pVehicleBeingTowed,v),V_TOWED); chk("m_nGunFiringTime",off(&v->m_nGunFiringTime,v),V_FIRETIME);
    chk("m_nProjectileWeaponFiringTime",off(&v->m_nProjectileWeaponFiringTime,v),V_PROJTIME); chk("m_nAdditionalProjectileWeaponFiringTime",off(&v->m_nAdditionalProjectileWeaponFiringTime,v),V_PROJTIME2);
    chk("m_nVehicleWeaponInUse",off(&v->m_nVehicleWeaponInUse,v),V_WEAPONINUSE); chk("m_pDriver",off(&v->m_pDriver,v),V_DRIVER); chk("m_apPassengers",off(&v->m_apPassengers,v),V_PASS);
    chk("m_nVehicleType",off(&v->m_nVehicleType,v),V_TYPE); chk("m_nVehicleSubType",off(&v->m_nVehicleSubType,v),V_SUBTYPE); chk("vehicleFlags",off(&v->vehicleFlags,v),V_FLAGS);
    chk("b.m_nBoatFlags",off(&b->m_nBoatFlags,b),V_BOATFLAGS); chk("b.m_fxBuoyancyForce",off(&b->m_fxBuoyancyForce,b),V_BOATFORCE);
    chk("au.m_fHeliRotorSpeed",off(&au->m_fHeliRotorSpeed,au),V_ROTORSPEED); chk("au.m_fDoomHorizontalRotation",off(&au->m_fDoomHorizontalRotation,au),V_DOOM);
    chk("pl.m_nFiringMultiplier",off(&pl->m_nFiringMultiplier,pl),V_PLANEFIRE); chk("he.m_nFiringMultiplier",off(&he->m_nFiringMultiplier,he),V_HELIFIRE);
    chk("mod_Buoyancy.m_fEntityWaterImmersion",uintptr_t(&mod_Buoyancy.m_fEntityWaterImmersion),ADDR_BUOYIMM);
    chk("sizeof(CColPoint)",sizeof(CColPoint),0x2C); chk("sizeof(CRope)",sizeof(CRope),0x328); chk("sizeof(CVehicle)",sizeof(CVehicle),0x5A0);
    auto* rp=&CRopes::aRopes.data()[0];
    chk("rope.m_nId",off(&rp->m_nId,rp),0x300); chk("rope.m_fMass",off(&rp->m_fMass,rp),0x308); chk("rope.m_fTotalLength",off(&rp->m_fTotalLength,rp),0x30C);
    chk("rope.m_pRopeAttachObject",off(&rp->m_pRopeAttachObject,rp),0x318); chk("rope.m_fSegmentLength",off(&rp->m_fSegmentLength,rp),0x31C); chk("rope.m_nType",off(&rp->m_nType,rp),0x325);
    chk("aRopes",uintptr_t(rp),ADDR_ROPES);
    chk("m_aTempColPts",uintptr_t(CWorld::m_aTempColPts.data()),ADDR_TEMPCOL);
    chk("camera m_nDirectionWasLooking",uintptr_t(&TheCamera.GetActiveCamera().m_nDirectionWasLooking),ADDR_CAMDIR+size_t(AtA<uint8>(ADDR_ACTIVECAM))*0x238);
    // bits de campos con bitfields
    auto bits=[&](auto setter,size_t at,size_t size,uint64 want,const char* nm){ std::memset(t.data(),0,t.size()); setter(); uint64 got{}; std::memcpy(&got,t.data()+at,size); if(got!=want){ why+=nm; why+=" "; } };
    bits([&]{ v->vehicleFlags.bGunSwitchedOff=true; },V_FLAGS+2,1,0x80,"bGunSwitchedOff");
    bits([&]{ v->vehicleFlags.bIsDrowning=true; },V_FLAGS+3,1,0x40,"bIsDrowning");
    bits([&]{ v->m_ropeType=3; },V_ROPEBYTE,1,0x60,"m_ropeType");
    bits([&]{ v->m_nGunsCycleIndex=3; },V_GUNBYTE,1,3,"m_nGunsCycleIndex");
    bits([&]{ v->physicalFlags.bDontCollideWithFlyers=true; },E_PHYS,4,0x1000000,"bDontCollideWithFlyers");
    bits([&]{ v->physicalFlags.bSkipLineCol=true; },E_PHYS,4,0x1000,"bSkipLineCol");
    bits([&]{ v->physicalFlags.bTouchingWater=true; },E_PHYS,4,0x8000000,"bTouchingWater");
    bits([&]{ v->physicalFlags.bSubmergedInWater=true; },E_PHYS,4,0x100,"bSubmergedInWater");
    bits([&]{ v->physicalFlags.bDisableCollisionForce=true; },E_PHYS,4,4,"bDisableCollisionForce");
    bits([&]{ v->physicalFlags.bCollidable=true; },E_PHYS,4,8,"bCollidable");
    bits([&]{ v->SetStatus(STATUS_REMOTE_CONTROLLED); },E_TYPE,1,0x40,"STATUS_REMOTE_CONTROLLED");
    bits([&]{ v->m_bUsesCollision=true; },E_FLAGS,4,1,"m_bUsesCollision");
    bits([&]{ v->m_bIsStuck=true; },E_FLAGS,4,0x10,"m_bIsStuck");
    auto* p=reinterpret_cast<CPed*>(t2.data()); auto* o=reinterpret_cast<CObject*>(t2.data());
    auto bits2=[&](auto setter,size_t at,size_t size,uint64 want,const char* nm){ std::memset(t2.data(),0,t2.size()); setter(); uint64 got{}; std::memcpy(&got,t2.data()+at,size); if(got!=want){ why+=nm; why+=" "; } };
    bits2([&]{ p->bIsStanding=true; },P_FLAGS1,4,1,"bIsStanding");
    bits2([&]{ p->bKnockedOffBike=true; },P_FLAGS2,4,0x80000000u,"bKnockedOffBike");
    bits2([&]{ o->objectFlags.bIsLampPost=true; },O_FLAGS,4,0x100,"bIsLampPost");
    bits2([&]{ o->objectFlags.bIsExploded=true; },O_FLAGS,4,0x40,"bIsExploded");
    chk("p.m_nPedType",off(&p->m_nPedType,p),P_PEDTYPE); chk("p.m_pVehicle",off(&p->m_pVehicle,p),P_VEHICLE); chk("o.m_nObjectType",off(&o->m_nObjectType,o),O_TYPE); chk("o.m_nColDamageEffect",off(&o->m_nColDamageEffect,o),O_COLDMG);
    return why;
}

inline void InitBufs(){
    if(!named.empty())return;
    Nm(veh.data(),veh.size(),"veh"); Nm(otherVeh.data(),otherVeh.size(),"otherVeh"); Nm(vehMat.data(),vehMat.size(),"vehMat"); Nm(colMat.data(),colMat.size(),"colMat"); Nm(bladeMat.data(),bladeMat.size(),"bladeMat");
    Nm(handling.data(),handling.size(),"handling"); Nm(boatHandling.data(),boatHandling.size(),"bh"); Nm(vehCM.data(),vehCM.size(),"vehCM"); Nm(bladeCM.data(),bladeCM.size(),"bladeCM");
    Nm(pedCM.data(),pedCM.size(),"pedCM"); Nm(colCM.data(),colCM.size(),"colCM"); Nm(drv.data(),drv.size(),"drv"); Nm(col.data(),col.size(),"col"); Nm(intel.data(),intel.size(),"intel");
    Nm(clumpBuf.data(),clumpBuf.size(),"clump"); Nm(fakePad.data(),fakePad.size(),"fakePad"); Nm(&camPos,sizeof(camPos),"camPos");
    static const char* entNames[NENT]{"ent0","ent1","ent2","ent3","ent4"}; static const char* matNames[NENT]{"entMat0","entMat1","entMat2","entMat3","entMat4"}; static const char* cmNames[NENT]{"entCM0","entCM1","entCM2","entCM3","entCM4"};
    for(size_t k=0;k<NENT;++k){ Nm(ents[k].data(),ENTSZ,entNames[k]); Nm(entMats[k].data(),MATSZ,matNames[k]); Nm(entCMs[k].data(),0x40,cmNames[k]); }
    for(auto& vt:trapVt)vt=reinterpret_cast<void*>(&V_Trap);
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cvehe-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cvehe-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cvehe");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"cvehe-only.txt"); if(f)std::getline(f,only); }
    InitBufs();
    if(auto bad=LayoutErrors(); !bad.empty()){Write({{"event","error"},{"reason","layout mismatch"},{"fields",bad}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    if(!DllBody("CEventDamage","AffectsPed")){Write({{"event","error"},{"reason","CEventDamage::AffectsPed body not found"}});return;}
    // estado global guardado
    regions.clear();
    Snap(ADDR_STEP,4); Snap(ADDR_TIME,4); Snap(ADDR_SCAN,2); Snap(ADDR_ROTORTIME,4); Snap(ADDR_TEMPCOL,0x2C*32); Snap(ADDR_BUOYIMM,4); Snap(ADDR_BLADECOL,BLADECOL_SIZE);
    Snap(ADDR_ROPES,0x328*8); Snap(ADDR_CAMDIR+size_t(AtA<uint8>(ADDR_ACTIVECAM))*0x238,4);
    for(int32 m:USED_MODELS)Snap(ADDR_MODELINFOS+4*size_t(m),4);
    { auto* pool=GetPtrNodeSingleLinkPool(); Snap(uintptr_t(pool),0x14); Snap(uintptr_t(At<void*>(pool,4)),At<uint32>(pool,8)); }
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->json {
                RestoreRegions(); CleanRefs();
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7E7Eu; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); cfg={}; colMapCount=0; sr.s=r.next()|1u;
                for(auto* h:hooks)h->State(candidate);
                const uintptr_t vtTarget=fn.slot>=0 ? AtA<uintptr_t>(fn.vt+4*size_t(fn.slot)) : 0;
                const bool switches=fn.slot>=0 ? (InExe(vtTarget)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                json out;
                if(nm=="ApplyBoatWaterResistance")out=RunApplyBoat(a,r);
                else if(nm=="ProcessBoatControl")out=RunProcessBoat(a,r);
                else if(nm.rfind("BladeColSectorList",0)==0)out=RunBlade(a,r);
                else if(nm=="DoBladeCollision")out=RunDoBlade(a,r);
                else if(nm=="DoFixedMachineGuns")out=RunDoFixed(a,r);
                else if(nm=="FirePlaneGuns")out=RunFirePlane(a,r);
                else if(nm=="FireUnguidedMissile")out=RunFireMissile(a,r);
                else if(nm=="InitWinch"||nm=="UpdateWinch"||nm=="RemoveWinch"||nm=="PickUpEntityWithWinch"||nm=="QueryPickedUpEntityWithWinch"||nm=="ReleasePickedUpEntityWithWinch"||nm=="GetRopeHeightForHeli"||nm=="SetRopeHeightForHeli")out=RunWinch(nm,a,r);
                else if(nm=="SpecialEntityCalcCollisionSteps")out=RunSteps(a,r);
                else if(nm=="SpecialEntityPreCollisionStuff")out=RunPre(a,r);
                else out=RunPlaneSimple(nm,a,r);
                out["switches"]=switches;
                CleanRefs();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            RestoreRegions();
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["trace"].size()},{"candidate",candidate["trace"].size()},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    CleanRefs(); RestoreRegions();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",RegionsRestored()&&freeRefs()==savedFree}});
}
}
