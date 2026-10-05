#pragma once
// Banco dentro del motor: CCollision, funciones de mundo/camara/ciclo de vida (lista de 59, 20 funciones). Original (hooks del banco
// desactivados) frente a candidato con fixtures deterministas por caso.
// Mundo falso: se vacian los sectores reales (0xB7D0B8 / 0xB992B8; copia completa y restauracion por ejecucion) y se rellenan con nodos de un
// pool falso (los pools de nodos y los de peatones/vehiculos/objetos se intercambian por cabeceras falsas que cubren los fixtures porque la
// DLL de depuracion valida cada nodo y cada item). Entidades falsas con offsets crudos; modelos falsos (ms_modelInfoPtrs, slots 1..NM+1) con
// colision propia (esferas, cajas, triangulos con planos ya calculados, grupos de caras). Se simulan con trazas, sin reservar memoria:
// FindPlayerPed/Vehicle/Speed, IsPlayer, GetBoundCentre, GetTaskHold, RegisterReference/CleanUpOldReference, DoesNotCollideWithFlyers,
// IsExtraEntityToIgnore, CameraToIgnoreThisObject, AnimatePedColModelSkinnedWorld, FindActiveTaskByType, GetCameraAvoidVehicle, ClearScanCodes,
// CalculateTrianglePlanes (los planos ya vienen calculados en el fixture), CColStore::Initialise/Shutdown/LoadCollision,
// CStreaming::LoadAllRequestedModels y CCollisionData::RemoveTrianglePlanes. Los parches solo se activan durante la llamada de cada ejecucion.
// SphereVsEntity (0x41A5A0) queda fuera: el EXE no tiene llamadores y su cuerpo lee una esfera local sin inicializar (ignora el argumento).
#include <fstream>
#include <functional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Collision.h"
#include "ColStore.h"
#include "CollisionData.h"
#include "ColHelpers.h"
#include "ColModel.h"
#include "PedModelInfo.h"
#include "PedIntelligence.h"
#include "TaskManager.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "TaskComplexEnterCarAsPassenger.h"
#include "Camera.h"
#include "Cam.h"
#include "World.h"
#include "Streaming.h"
#include "CutsceneMgr.h"
#include "SurfaceInfos_c.h"
#include "Pools/Pools.h"
#include "../ProbeRuntime.hpp"

namespace ColWorld59cLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* b,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(b)+off); }
template<class T> inline T& G(uintptr_t a){ return *reinterpret_cast<T*>(a); }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,20> fns{{
    {"CCollision","Init",0x416260,0,-1,256},{"CCollision","Shutdown",0x4162E0,0,-1,256},
    {"CCollision","Update",0x411E20,0,-1,256},{"CCollision","SortOutCollisionAfterLoad",0x411E30,0,-1,256},
    {"CCollision","IsThisVehicleSittingOnMe",0x4152C0,0,-1,512},{"CCollision","ResetMadeInvisibleObjects",0x415540,0,-1,256},
    {"CCollision","CheckCameraCollisionPeds",0x415320,0,-1,512},{"CCollision","CheckPeds",0x4154A0,0,-1,512},
    {"CCollision","SphereCastVsBBox",0x415590,0,-1,1024},{"CCollision","SphereCastVersusVsPoly",0x415CF0,0,-1,2048},
    {"CCollision","SphereCastVsSphere",0x417F20,0,-1,1024},{"CCollision","SphereCastVsEntity",0x419F00,0,-1,1024},
    {"CCollision","SphereCastVsCaches",0x4181B0,0,-1,1024},{"CCollision","CheckCameraCollisionBuildings",0x41A820,0,-1,512},
    {"CCollision","CheckCameraCollisionVehicles",0x41A990,0,-1,512},{"CCollision","CheckCameraCollisionObjects",0x41AB20,0,-1,512},
    {"CCollision","BuildCacheOfCameraCollision",0x41AC40,0,-1,512},{"CCollision","CameraConeCastVsWorldCollision",0x41B000,0,-1,512},
    {"CCollision","TestLineOfSight",0x417730,0,-1,1024},{"CCollision","ProcessLineOfSight",0x417950,0,-1,1024},
}};

// ---------------------------------------------------------------- fixtures ----
constexpr int NM=6, NB=8, NV=6, NO=6, NP=6, NSN=512, NDN=512, NCACHE=100;
constexpr int NMAT=NB+NV+NO+NP+2;
constexpr size_t PED_STRIDE=0x7C4, VEH_STRIDE=0xA18, OBJ_STRIDE=0x19C, BLD_STRIDE=0x100, ARENA=0x200;
constexpr uintptr_t A_SECTORS=0xB7D0B8, A_REPEAT=0xB992B8, SECTORS_SIZE=120*120*8, REPEAT_SIZE=16*16*12, A_MI=0xA9B0C8;
alignas(16) inline std::array<uint8,NP*PED_STRIDE> pedStore{};
alignas(16) inline std::array<uint8,NV*VEH_STRIDE> vehStore{};
alignas(16) inline std::array<uint8,NO*OBJ_STRIDE> objStore{};
alignas(16) inline std::array<uint8,NB*BLD_STRIDE> bldStore{};
alignas(16) inline std::array<uint8,0x14> pedHdr{}, vehHdr{}, objHdr{}, bldHdr{}, snHdr{}, dnHdr{};
alignas(16) inline std::array<uint8,2*NP> pedSt{}; alignas(16) inline std::array<uint8,2*NV> vehSt{}; alignas(16) inline std::array<uint8,2*NO> objSt{}; // capacidad doble: no depende del tamano exacto del elemento del pool
alignas(16) inline std::array<uint8,128> bldSt{}; alignas(16) inline std::array<uint8,NSN> snSt{}; alignas(16) inline std::array<uint8,NDN> dnSt{};
alignas(16) inline std::array<uint32,NSN*2> snodes{};
alignas(16) inline std::array<uint32,NDN*3> dnodes{};
alignas(16) inline std::array<std::array<uint8,0x40>,NM+1> miBuf{};
alignas(16) inline std::array<std::array<uint8,0x30>,NM+1> cmBuf{}, cdBuf{};
alignas(16) inline std::array<std::array<uint8,ARENA>,NM+1> arena{};
alignas(16) inline std::array<std::array<uint8,0x48>,NMAT> mats{};
alignas(16) inline std::array<std::array<uint8,0x40>,NP> pedIntel{}, holdTask{};
alignas(16) inline std::array<uint8,0x40> plyIntel{}, tkPass{}, tkDrv{}, clump{}, rwCam{};
alignas(16) inline std::array<uint8,0x800> plyPed{};
alignas(16) inline std::array<uint8,NCACHE*16> cacheA{}, cacheB{}, cacheC{};
alignas(16) inline std::array<uint8,0x2C> colPt{};
alignas(16) inline std::array<uint8,0x14> spAb{}, spBb{}, spSb{};
alignas(16) inline std::array<uint8,0x1C> boxb{};
alignas(16) inline std::array<uint8,0x10> triB{}, planeB{};
alignas(16) inline std::array<uint8,0x40> vertsB{};
alignas(16) inline std::array<uint8,0x20> lineB{};

inline uint8* PedP(int k){ return pedStore.data()+k*PED_STRIDE; } inline uint8* VehP(int k){ return vehStore.data()+k*VEH_STRIDE; }
inline uint8* ObjP(int k){ return objStore.data()+k*OBJ_STRIDE; } inline uint8* BldP(int k){ return bldStore.data()+k*BLD_STRIDE; }
inline CEntity* E(uint8* p){ return reinterpret_cast<CEntity*>(p); }
inline int snUsed{}, dnUsed{};
inline uint16 scanNow{};

struct Rng {
    uint32 s{1};
    uint32 operator()(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float((*this)()%1000000)/1000000.f; }
    int n(int m){ return m<=1 ? 0 : int((*this)()%uint32(m)); }
    bool one(int m){ return (*this)()%uint32(m)==0; }
    CVector vec(float r){ return CVector{uni(-r,r),uni(-r,r),uni(-r,r)}; }
};
inline CVector Cross3(const CVector& a,const CVector& b){ return CVector{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
inline CVector Sub3(const CVector& a,const CVector& b){ return CVector{a.x-b.x,a.y-b.y,a.z-b.z}; }
inline CVector Add3(const CVector& a,const CVector& b){ return CVector{a.x+b.x,a.y+b.y,a.z+b.z}; }
inline CVector Mul3(const CVector& a,float f){ return CVector{a.x*f,a.y*f,a.z*f}; }

// valores que devuelven los parches y datos de los fixtures que ellos leen
inline struct Fx {
    uint8* player{}; uint8* playerVeh{}; CVector speed{}; uint8* avoid{}; bool passTask{}, drvTask{};
    uint8* holdOfIntel[NP]{};
} fx;

// ------------------------------------------------------------------ trazas ----
struct Tr { uint32 k,a,b,c; };
inline std::array<Tr,8192> tr{}; inline int trn{};
inline void Tc(uint32 k,uint32 a=0,uint32 b=0,uint32 c=0){ if(trn<8192)tr[trn++]={k,a,b,c}; }

// parches (sin reservar memoria: solo escriben en el array fijo)
inline void __cdecl SInitialise(){ Tc(1); }
inline void __cdecl SColShutdown(){ Tc(2); }
inline void __cdecl SLoadCollision(CVector p,bool b){ Tc(3+(b?1:0),F(p.x),F(p.y),F(p.z)); }
inline void __cdecl SLoadAll(bool b){ Tc(5,b); }
inline void __fastcall SRemovePlanes(CCollisionData* self,void*){ Tc(6,U(self)); }
inline bool __fastcall SIsPlayer(CPed* self,void*){ const bool r=At<uint8>(self,0x7B0)!=0; Tc(7,U(self),r); return r; }
inline CVector* __fastcall SBoundCentre(CEntity* self,void*,CVector* out){ *out=At<CVector>(self,4); Tc(8,U(self)); return out; }
inline CTaskSimpleHoldEntity* __fastcall SGetTaskHold(void* self,void*,bool arg){ Tc(9,U(self),arg); for(int k=0;k<NP;++k)if(self==pedIntel[k].data())return reinterpret_cast<CTaskSimpleHoldEntity*>(fx.holdOfIntel[k]); return nullptr; }
inline void __fastcall SRegisterReference(CEntity* self,void*,CEntity** ref){ Tc(10,U(self),U(ref)); }
inline void __fastcall SCleanUpOldReference(CEntity* self,void*,CEntity** ref){ Tc(11,U(self),U(ref)); }
inline CColModel* __fastcall SAnimate(void* self,void*,RpClump* c){ Tc(12,U(self),U(c)); return nullptr; }
inline bool __fastcall SIsExtra(void* self,void*,CEntity* e){ const bool r=At<uint8>(e,0xFA)!=0; Tc(13,U(self),U(e),r); return r; }
inline bool __fastcall SNoFlyers(CEntity* self,void*){ const bool r=At<uint8>(self,0xF9)!=0; Tc(14,U(self),r); return r; }
inline bool __cdecl SCameraToIgnore(CEntity* e){ const bool r=At<uint8>(e,0xFB)!=0; Tc(15,U(e),r); return r; }
inline CPlayerPed* __cdecl SFindPlayerPed(int32 a){ Tc(16,uint32(a)); return reinterpret_cast<CPlayerPed*>(fx.player); }
inline CVehicle* __cdecl SFindPlayerVehicle(int32 a,bool b){ Tc(17,uint32(a),b); return reinterpret_cast<CVehicle*>(fx.playerVeh); }
inline CVector& __cdecl SFindPlayerSpeed(int32 a){ Tc(18,uint32(a)); return fx.speed; }
inline CTask* __fastcall SFindActiveTask(void* self,void*,int32 type){ Tc(19,U(self),uint32(type)); return type==0x2BD ? (fx.drvTask ? reinterpret_cast<CTask*>(tkDrv.data()) : nullptr) : type==0x2BC ? (fx.passTask ? reinterpret_cast<CTask*>(tkPass.data()) : nullptr) : nullptr; }
inline CVehicle* __fastcall SAvoidVehicle(void* self,void*){ Tc(20,U(self)); return reinterpret_cast<CVehicle*>(fx.avoid); }
inline void __cdecl SClearScanCodes(){ Tc(21); }
inline void __cdecl SCalcPlanes(CCollisionData* cd){ Tc(22,U(cd)); }

// ----------------------------------------------------------- estado global ----
struct Range { uintptr_t at; size_t n; const char* tag; };
inline std::vector<Range> ranges;
inline void InitRanges() {
    ranges={ {0x965590,0x10,"gn"},{0x9655A0,0x28,"inv"},{0x9655C8,4,"colCache2"},{0x9655CC,4,"colCache"},{0x9655D0,4,"crossings"},{0x9655D4,4,"inMemory"},
        {0x9655D8,4,"numEntries"},{0x9655DC,4,"numInvisible"},{0x9655E4,1,"dblSided"},{0x96592C,0x34,"colModelCache"},{0x9689D4,8,"sitting"},
        {0xB6EC6C,4,"lastRadius"},{0xB7CD68,4,"ignore"},{0xB7CD78,2,"scanCode"},{0x8A5B14,4,"camFlags"},{0x8A5B18,8,"relVel"},
        {0xB6F081,1,"activeCam"},{0xB6F19C,3*0x238,"cams"},{0xB6F9CC,12,"camPos"},{0xC1703C,4,"rwCamera"},
        {0xB74484,4,"poolSn"},{0xB74488,4,"poolDn"},{0xB74490,4,"poolPed"},{0xB74494,4,"poolVeh"},{0xB74498,4,"poolBld"},{0xB7449C,4,"poolObj"},
        {A_MI+4,4*(NM+1),"mi"},{A_SECTORS,SECTORS_SIZE,"sectors"},{A_REPEAT,REPEAT_SIZE,"repeat"},
        {uintptr_t(pedStore.data()),pedStore.size(),"pedStore"},{uintptr_t(vehStore.data()),vehStore.size(),"vehStore"},
        {uintptr_t(objStore.data()),objStore.size(),"objStore"},{uintptr_t(bldStore.data()),bldStore.size(),"bldStore"},
        {uintptr_t(snodes.data()),sizeof(snodes),"snodes"},{uintptr_t(dnodes.data()),sizeof(dnodes),"dnodes"},
        {uintptr_t(cacheA.data()),cacheA.size(),"cacheA"},{uintptr_t(cacheB.data()),cacheB.size(),"cacheB"},{uintptr_t(cacheC.data()),cacheC.size(),"cacheC"},
        {uintptr_t(mats.data()),sizeof(mats),"mats"},{uintptr_t(cmBuf.data()),sizeof(cmBuf),"cm"},{uintptr_t(cdBuf.data()),sizeof(cdBuf),"cd"},
        {uintptr_t(arena.data()),sizeof(arena),"arena"},{uintptr_t(miBuf.data()),sizeof(miBuf),"miBuf"},{uintptr_t(colPt.data()),colPt.size(),"colPt"},
        {uintptr_t(plyPed.data()),plyPed.size(),"plyPed"},{uintptr_t(plyIntel.data()),plyIntel.size(),"plyIntel"},
        {uintptr_t(pedIntel.data()),sizeof(pedIntel),"pedIntel"},{uintptr_t(holdTask.data()),sizeof(holdTask),"holdTask"},{uintptr_t(rwCam.data()),rwCam.size(),"rwCam"},
        {uintptr_t(clump.data()),clump.size(),"clump"} };
}
inline std::vector<uint8> Dump() { std::vector<uint8> d; for(auto& r:ranges){ auto* p=reinterpret_cast<const uint8*>(r.at); d.insert(d.end(),p,p+r.n); } return d; }
inline void Restore(const std::vector<uint8>& d) { size_t o=0; for(auto& r:ranges){ std::memcpy(reinterpret_cast<void*>(r.at),d.data()+o,r.n); o+=r.n; } }
inline json Diffs(const std::vector<uint8>& a,const std::vector<uint8>& b) {
    json out=json::array(); size_t o=0;
    for(auto& r:ranges){ for(size_t k=0;k<r.n&&out.size()<24;++k)if(a[o+k]!=b[o+k])out.push_back({r.tag,r.at+k,k,a[o+k],b[o+k]}); o+=r.n; }
    return out;
}
// nombres estables para punteros a los fixtures
struct Buf { uintptr_t p; size_t n; std::string nm; size_t stride; };
inline std::vector<Buf> bufs;
inline void RegisterBuffers() {
    bufs.clear(); auto add=[](const void* p,size_t n,std::string nm,size_t stride=0){ bufs.push_back({uintptr_t(p),n,std::move(nm),stride}); };
    add(pedStore.data(),pedStore.size(),"ped",PED_STRIDE); add(vehStore.data(),vehStore.size(),"veh",VEH_STRIDE); add(objStore.data(),objStore.size(),"obj",OBJ_STRIDE);
    add(bldStore.data(),bldStore.size(),"bld",BLD_STRIDE); add(snodes.data(),sizeof(snodes),"sn",8); add(dnodes.data(),sizeof(dnodes),"dn",12);
    add(cacheA.data(),cacheA.size(),"cacheA",16); add(cacheB.data(),cacheB.size(),"cacheB",16); add(cacheC.data(),cacheC.size(),"cacheC",16);
    add(mats.data(),sizeof(mats),"mat",0x48); add(cmBuf.data(),sizeof(cmBuf),"cm",0x30); add(cdBuf.data(),sizeof(cdBuf),"cd",0x30); add(arena.data(),sizeof(arena),"arena",ARENA);
    add(miBuf.data(),sizeof(miBuf),"mi",0x40); add(pedIntel.data(),sizeof(pedIntel),"pedIntel",0x40); add(holdTask.data(),sizeof(holdTask),"holdTask",0x40);
    add(plyPed.data(),plyPed.size(),"plyPed"); add(plyIntel.data(),plyIntel.size(),"plyIntel"); add(tkPass.data(),tkPass.size(),"tkPass"); add(tkDrv.data(),tkDrv.size(),"tkDrv");
    add(clump.data(),clump.size(),"clump"); add(rwCam.data(),rwCam.size(),"rwCam");
    add(reinterpret_cast<void*>(0x9655A0),0x28,"inv",4); add(reinterpret_cast<void*>(0x96592C),0x34,"cache",4);
}
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    for(auto& b:bufs)if(v>=b.p&&v<b.p+b.n){ const size_t off=v-b.p; return b.stride ? b.nm+"#"+std::to_string(off/b.stride)+"+"+std::to_string(off%b.stride) : b.nm+"+"+std::to_string(off); }
    return v;
}
// traza -> json (los punteros a fixtures con nombre estable; el resto tal cual)
inline json TraceJson() {
    json t=json::array();
    for(int k=0;k<trn;++k){ const auto& r=tr[k]; json row=json::array({r.k});
        switch(r.k){
        case 6: case 7: case 8: case 9: case 10: case 11: case 12: case 13: case 14: case 15: case 19: case 20: case 22:
            row.push_back(Name(r.a)); row.push_back((r.k==10||r.k==11) ? Name(r.b) : json(r.b)); row.push_back(r.c); break;
        default: row.push_back(r.a); row.push_back(r.b); row.push_back(r.c); }
        t.push_back(row); }
    return t;
}

// --------------------------------------------------------- generadores ----
inline void MakeMat(Rng& r,uint8* m,CVector pos) {
    std::memset(m,0,0x48);
    const float yaw=r.uni(-3.1f,3.1f), pit=r.uni(-0.7f,0.7f), rol=r.uni(-0.7f,0.7f); const float sy=std::sin(yaw),cy=std::cos(yaw),sp=std::sin(pit),cp=std::cos(pit),sr=std::sin(rol),cr=std::cos(rol);
    const bool ident=r.one(6);
    At<CVector>(m,0x00)=ident ? CVector{1,0,0} : CVector{cy*cr,sy*cr,-sr};
    At<CVector>(m,0x10)=ident ? CVector{0,1,0} : CVector{-sy*cp+cy*sr*sp,cy*cp+sy*sr*sp,cr*sp};
    At<CVector>(m,0x20)=ident ? CVector{0,0,1} : CVector{sy*sp+cy*sr*cp,-cy*sp+sy*sr*cp,cr*cp};
    At<CVector>(m,0x30)=pos;
}
inline int16 Cl16(float f){ return int16(std::clamp(int(std::lround(f)),-32767,32767)); }
// plano comprimido de un triangulo (normal * 4096, distancia * 128, orientacion por eje dominante)
inline void MakePlane(uint8* p,const CVector& a,const CVector& b,const CVector& c) {
    CVector n=Cross3(Sub3(b,a),Sub3(c,a)); float len=std::sqrt(n.x*n.x+n.y*n.y+n.z*n.z); if(!(len>1e-6f)){ n=CVector{0,0,1}; len=1.f; } n=Mul3(n,1.f/len);
    At<int16>(p,0)=Cl16(n.x*4096.f); At<int16>(p,2)=Cl16(n.y*4096.f); At<int16>(p,4)=Cl16(n.z*4096.f); At<int16>(p,6)=Cl16((a.x*n.x+a.y*n.y+a.z*n.z)*128.f);
    const float ax=std::abs(n.x), ay=std::abs(n.y), az=std::abs(n.z); const int axis=(ax>ay&&ax>az) ? 0 : (ay>az ? 1 : 2);
    p[8]=uint8(2*axis+((axis==0 ? n.x : axis==1 ? n.y : n.z)<0 ? 1 : 0));
}
// colision falsa del modelo m: esferas, cajas, triangulos (+ grupos de caras) con los planos ya calculados
inline void BuildCol(Rng& r,int m,bool nullData) {
    auto* cm=cmBuf[m].data(); auto* cd=cdBuf[m].data(); auto* ar=arena[m].data(); std::memset(cm,0,0x30); std::memset(cd,0,0x30); std::memset(ar,0,ARENA);
    const int nS=r.n(5), nB=r.n(4); const int nT=r.n(9); const int nV=nT ? 3+r.n(6) : r.n(3);
    CVector lo{1e9f,1e9f,1e9f}, hi{-1e9f,-1e9f,-1e9f};
    auto ext=[&](const CVector& p,float rad){ lo.x=std::min(lo.x,p.x-rad);lo.y=std::min(lo.y,p.y-rad);lo.z=std::min(lo.z,p.z-rad);hi.x=std::max(hi.x,p.x+rad);hi.y=std::max(hi.y,p.y+rad);hi.z=std::max(hi.z,p.z+rad); };
    uint8* sph=ar+0; uint8* box=ar+0x60; uint8* ver=ar+0xC0;
    for(int k=0;k<nS;++k){ uint8* s=sph+0x14*k; const CVector c=r.vec(4.f); const float rad=r.uni(0.2f,2.f); At<CVector>(s,0)=c; At<float>(s,0xC)=rad; s[0x10]=uint8(r()%179); s[0x11]=uint8(r()); s[0x12]=uint8(r()); ext(c,rad); }
    for(int k=0;k<nB;++k){ uint8* b=box+0x1C*k; const CVector a=r.vec(4.f); const CVector c=Add3(a,CVector{r.uni(0.1f,3.f),r.uni(0.1f,3.f),r.uni(0.1f,3.f)}); At<CVector>(b,0)=a; At<CVector>(b,0xC)=c; b[0x18]=uint8(r()%179); b[0x19]=uint8(r()); b[0x1A]=uint8(r()); ext(a,0.f); ext(c,0.f); }
    CVector v[10]; for(int k=0;k<nV;++k){ int16* p=reinterpret_cast<int16*>(ver+6*k); for(int j=0;j<3;++j)p[j]=int16(int(r()%1536)-768); v[k]=CVector{p[0]/128.f,p[1]/128.f,p[2]/128.f}; ext(v[k],0.f); }
    const bool fg=nT>=3&&r.one(3); const int nFG=fg ? 1+r.n(3) : 0; uint8* trisBase=ar+0x100+(fg ? nFG*0x1C+4 : 0); uint8* pl=ar+0x1A0;
    for(int k=0;k<nT;++k){ uint16* t=reinterpret_cast<uint16*>(trisBase+8*k); t[0]=uint16(r.n(nV)); t[1]=uint16(r.n(nV)); t[2]=uint16(r.n(nV)); trisBase[8*k+6]=uint8(r()%179); trisBase[8*k+7]=uint8(r());
        MakePlane(pl+10*k,v[t[0]],v[t[1]],v[t[2]]); }
    if(fg){ *reinterpret_cast<uint32*>(trisBase-4)=uint32(nFG);
        for(int g=0;g<nFG;++g){ uint8* f=trisBase-0x20-0x1C*g; const int first=r.n(nT); const int last=std::min(nT-1,first+r.n(nT)); CVector gl{1e9f,1e9f,1e9f}, gh{-1e9f,-1e9f,-1e9f};
            for(int k=first;k<=last;++k)for(int j=0;j<3;++j){ const CVector& q=v[reinterpret_cast<uint16*>(trisBase+8*k)[j]]; gl.x=std::min(gl.x,q.x);gl.y=std::min(gl.y,q.y);gl.z=std::min(gl.z,q.z);gh.x=std::max(gh.x,q.x);gh.y=std::max(gh.y,q.y);gh.z=std::max(gh.z,q.z); }
            const float mg=r.one(4) ? 0.f : r.uni(0.f,1.5f); At<CVector>(f,0)=Sub3(gl,CVector{mg,mg,mg}); At<CVector>(f,0xC)=Add3(gh,CVector{mg,mg,mg}); At<uint16>(f,0x18)=uint16(first); At<uint16>(f,0x1A)=uint16(last); } }
    if(nS+nB+nV==0)ext(CVector{0,0,0},0.5f);
    const CVector c{(lo.x+hi.x)/2,(lo.y+hi.y)/2,(lo.z+hi.z)/2}; const float diag=std::sqrt((hi.x-lo.x)*(hi.x-lo.x)+(hi.y-lo.y)*(hi.y-lo.y)+(hi.z-lo.z)*(hi.z-lo.z));
    const float rad=std::max(0.5f,diag/2.f*r.uni(0.8f,1.2f));
    At<CVector>(cm,0)=lo; At<CVector>(cm,0xC)=hi; At<CVector>(cm,0x18)=c; At<float>(cm,0x24)=rad;
    At<uint16>(cd,0)=uint16(nS); At<uint16>(cd,2)=uint16(nB); At<uint16>(cd,4)=uint16(nT); cd[7]=fg ? 2 : 0;
    At<uint32>(cd,8)=U(sph); At<uint32>(cd,0xC)=U(box); At<uint32>(cd,0x14)=U(ver); At<uint32>(cd,0x18)=U(trisBase); At<uint32>(cd,0x1C)=nT ? U(pl) : 0;
    At<uint32>(cm,0x2C)=nullData ? 0 : U(cd);
}
// modelos: slots 1..NM (edificio/vehiculo/objeto) y NM+1 (peaton: colision de golpe con 12 esferas en +0x34)
inline void BuildModels(Rng& r) {
    for(int m=0;m<NM;++m){ BuildCol(r,m,m==0&&r.one(4)); std::memset(miBuf[m].data(),0,0x40); At<uint32>(miBuf[m].data(),0x14)=U(cmBuf[m].data()); G<uint32>(A_MI+4*(m+1))=U(miBuf[m].data()); }
    auto* cm=cmBuf[NM].data(); auto* cd=cdBuf[NM].data(); auto* ar=arena[NM].data(); std::memset(cm,0,0x30); std::memset(cd,0,0x30); std::memset(ar,0,ARENA);
    for(int k=0;k<12;++k){ uint8* s=ar+0x14*k; At<CVector>(s,0)=r.vec(2.5f); At<float>(s,0xC)=r.uni(0.1f,1.2f); }
    At<uint16>(cd,0)=12; At<uint32>(cd,8)=U(ar); At<uint32>(cm,0x2C)=U(cd); At<float>(cm,0x24)=3.f;
    std::memset(miBuf[NM].data(),0,0x40); At<uint32>(miBuf[NM].data(),0x14)=U(cm); At<uint32>(miBuf[NM].data(),0x34)=U(cm); G<uint32>(A_MI+4*(NM+1))=U(miBuf[NM].data());
}
inline void MakeEnt(Rng& r,uint8* e,size_t stride,int type,int matIdx,CVector pos,int modelSlot) {
    std::memset(e,0,stride);
    At<uint8>(e,0x36)=uint8(type|(r()&0xF8)); uint32 fl=r()&~0x81u; if(r.n(5))fl|=1; if(r.n(4))fl|=0x80; At<uint32>(e,0x1C)=fl;
    At<uint16>(e,0x2C)=r.one(2) ? scanNow : uint16(r()); At<int16>(e,0x22)=int16(modelSlot); At<uint32>(e,0x14)=U(mats[matIdx].data()); At<CVector>(e,4)=pos;
    MakeMat(r,mats[matIdx].data(),pos);
    At<uint8>(e,0xF8)=r.one(4); At<uint8>(e,0xF9)=r.one(3); At<uint8>(e,0xFA)=r.one(8); At<uint8>(e,0xFB)=r.one(4);
}
inline void MakeVeh(Rng& r,int k,CVector pos){ auto* e=VehP(k); MakeEnt(r,e,VEH_STRIDE,2,NB+k,pos,1+r.n(NM)); At<uint8>(e,0x48B)=0xFF; At<CVector>(e,0x44)=r.vec(r.one(3) ? 0.01f : 0.8f);
    At<uint8>(e,0x43)=r.one(2) ? 1 : 0; At<uint32>(e,0x590)=uint32(r.one(3) ? r.n(12) : (r.one(2) ? 9 : 0)); }
inline void SetPools(){
    auto hdr=[](std::array<uint8,0x14>& h,void* store,uint8* st,int cap){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=store; At<void*>(h.data(),4)=st; At<int32>(h.data(),8)=cap; At<int32>(h.data(),0xC)=-1; std::memset(st,0,size_t(cap)); };
    hdr(pedHdr,pedStore.data(),pedSt.data(),2*NP); hdr(vehHdr,vehStore.data(),vehSt.data(),2*NV); hdr(objHdr,objStore.data(),objSt.data(),2*NO);
    hdr(bldHdr,bldStore.data(),bldSt.data(),128); hdr(snHdr,snodes.data(),snSt.data(),NSN); hdr(dnHdr,dnodes.data(),dnSt.data(),NDN);
    G<void*>(0xB74484)=snHdr.data(); G<void*>(0xB74488)=dnHdr.data(); G<void*>(0xB74490)=pedHdr.data(); G<void*>(0xB74494)=vehHdr.data(); G<void*>(0xB74498)=bldHdr.data(); G<void*>(0xB7449C)=objHdr.data();
}
inline int Clamp120(int v){ return v<0 ? 0 : v>119 ? 119 : v; }
// lista de edificios (un solo enlace, { item, siguiente }) en el sector (x, y) tras acotarlo como CWorld::GetSector
inline void AddBuilding(int x,int y,uint8* ent){ if(snUsed>=NSN)return; uint32* head=reinterpret_cast<uint32*>(A_SECTORS)+2*(Clamp120(y)*120+Clamp120(x));
    uint32* n=&snodes[2*snUsed++]; n[0]=U(ent); n[1]=0; uint32* t=reinterpret_cast<uint32*>(*head); if(!t){ *head=U(n); return; } while(t[1])t=reinterpret_cast<uint32*>(t[1]); t[1]=U(n); }
// listas de los sectores repetidos (doble enlace, { item, siguiente, anterior }): slot 0 vehiculos, 1 peatones, 2 objetos; indice con `& 0xF`
inline void AddRepeat(int x,int y,int slot,uint8* ent){ if(dnUsed>=NDN)return; uint32* head=reinterpret_cast<uint32*>(A_REPEAT)+3*((y&15)*16+(x&15))+slot;
    uint32* n=&dnodes[3*dnUsed++]; n[0]=U(ent); n[1]=0; n[2]=0; uint32* t=reinterpret_cast<uint32*>(*head); if(!t){ *head=U(n); return; } while(t[1])t=reinterpret_cast<uint32*>(t[1]); t[1]=U(n); n[2]=U(t); }
inline void FakeCamera(Rng& r){
    G<uint8>(0xB6F081)=uint8(r.n(3)); for(int c=0;c<3;++c){ uint8* cam=reinterpret_cast<uint8*>(0xB6F19C)+0x238*c; At<CVector>(cam,0x190)=r.vec(1.f); At<CVector>(cam,0x19C)=r.vec(3.f); }
    std::memset(rwCam.data(),0,rwCam.size()); At<float>(rwCam.data(),0x80)=r.uni(0.05f,1.f); G<void*>(0xC1703C)=rwCam.data();
}
inline void FakePlayer(Rng& r){
    std::memset(plyPed.data(),0,plyPed.size()); std::memset(plyIntel.data(),0,plyIntel.size()); At<uint32>(plyPed.data(),0x47C)=U(plyIntel.data());
    fx.player=plyPed.data(); fx.playerVeh=r.one(2) ? VehP(r.n(NV)) : nullptr; fx.speed=r.vec(r.one(2) ? 0.5f : 0.05f); fx.avoid=r.one(3) ? nullptr : VehP(r.n(NV));
    fx.passTask=r.one(3); fx.drvTask=r.one(3);
}
// Mundo completo alrededor de un punto: entidades a `spread` unidades, sectores vecinos con listas al azar y globales de la camara
inline void BuildWorld(Rng& r,CVector c,int radiusSectors,bool withPeds,float spread) {
    BuildModels(r);
    scanNow=uint16(1+r()%0xFFFE); if(r.one(8))scanNow=0xFFFF; G<uint16>(0xB7CD78)=scanNow;
    for(int k=0;k<NB;++k)MakeEnt(r,BldP(k),BLD_STRIDE,1,k,Add3(c,r.vec(spread)),1+r.n(NM));
    for(int k=0;k<NV;++k)MakeVeh(r,k,Add3(c,r.vec(spread)));
    for(int k=0;k<NO;++k)MakeEnt(r,ObjP(k),OBJ_STRIDE,4,NB+NV+k,Add3(c,r.vec(spread)),1+r.n(NM));
    clump[0]=2;
    for(int k=0;k<NP;++k){ auto* e=PedP(k); MakeEnt(r,e,PED_STRIDE,3,NB+NV+NO+k,Add3(c,r.vec(1.6f)),NM+1); At<uint32>(e,0x18)=U(clump.data()); At<uint32>(e,0x47C)=U(pedIntel[k].data()); At<uint8>(e,0x7B0)=r.one(6);
        std::memset(holdTask[k].data(),0,0x40); fx.holdOfIntel[k]=nullptr;
        if(k<2&&r.one(2)){ fx.holdOfIntel[k]=holdTask[k].data(); At<uint32>(holdTask[k].data(),8)=r.one(3) ? 0 : U(r.one(2) ? ObjP(r.n(NO)) : BldP(r.n(NB))); } }
    G<uint32>(0xB7CD68)=r.one(3) ? 0 : U(r.one(2) ? VehP(r.n(NV)) : r.one(2) ? BldP(r.n(NB)) : ObjP(r.n(NO)));
    G<uint8>(0x8A5B14)=r.n(5)!=0; G<uint8>(0x8A5B15)=r.n(5)!=0; G<uint8>(0x8A5B16)=r.n(5)!=0; G<uint8>(0x8A5B17)=r.n(5)!=0;
    G<float>(0x8A5B18)=r.one(8) ? 100.f : r.uni(0.0005f,0.5f); G<int32>(0x8A5B1C)=30;
    G<uint32>(0x9689D8)=r.one(2) ? U(VehP(r.n(NV))) : 0; G<int32>(0x9689D4)=r.one(4) ? 0 : r.n(40);
    std::memset(reinterpret_cast<void*>(A_SECTORS),0,SECTORS_SIZE); std::memset(reinterpret_cast<void*>(A_REPEAT),0,REPEAT_SIZE); snUsed=dnUsed=0;
    const int sx0=int(std::floor(c.x*0.02f+60.f)), sy0=int(std::floor(c.y*0.02f+60.f));
    for(int dy=-radiusSectors;dy<=radiusSectors;++dy)for(int dx=-radiusSectors;dx<=radiusSectors;++dx){ const int x=sx0+dx, y=sy0+dy;
        for(int n=r.n(4);n>0;--n)AddBuilding(x,y,BldP(r.n(NB)));
        for(int n=r.n(4);n>0;--n)AddRepeat(x,y,0,VehP(r.n(NV)));
        for(int n=r.n(4);n>0;--n)AddRepeat(x,y,2,ObjP(r.n(NO)));
        if(withPeds)for(int n=r.n(4);n>0;--n)AddRepeat(x,y,1,PedP(r.n(NP))); }
    SetPools(); FakeCamera(r); FakePlayer(r);
    G<uint32>(0x9655CC)=U(cacheA.data()); G<uint32>(0x9655C8)=U(cacheB.data()); std::memset(cacheA.data(),0xCD,cacheA.size()); std::memset(cacheB.data(),0xCD,cacheB.size());
    G<uint32>(0x9655D8)=0; G<uint32>(0x9655DC)=0; G<uint8>(0x9655E4)=0; G<uint32>(0x9655D0)=r();
}
inline void RandSphere(Rng& r,uint8* s,CVector c,float spread,float rmin,float rmax){ At<CVector>(s,0)=Add3(c,r.vec(spread)); At<float>(s,0xC)=r.uni(rmin,rmax); s[0x10]=uint8(r()); s[0x11]=uint8(r()); s[0x12]=uint8(r()); s[0x13]=uint8(r()); }
inline CVector RandCenter(Rng& r){ return r.one(10) ? CVector{r.uni(-4200.f,4200.f),r.uni(-4200.f,4200.f),r.uni(-20.f,150.f)} : CVector{r.uni(-2900.f,2900.f),r.uni(-2900.f,2900.f),r.uni(-20.f,150.f)}; }
inline json Cps(const std::array<uint8,0x2C>& a){ json j=json::array(); const auto* w=reinterpret_cast<const uint32*>(a.data()); for(int k=0;k<11;++k)j.push_back(w[k]); return j; }
inline json CacheJson(const uint8* c,int n){ json j=json::array(); for(int k=0;k<n;++k){ json e=json::array(); for(int w=0;w<4;++w){ const uint32 v=reinterpret_cast<const uint32*>(c+16*k)[w]; e.push_back(w==1 ? Name(v) : json(v)); } j.push_back(e); } return j; }

// ------------------------------------------------------------ ejecucion ----
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"colworld59c-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"colworld59c-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("colworld59c");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"colworld59c-only.txt"); if(f)std::getline(f,only); }
    // comprobaciones de disposicion: si algo no cuadra se avisa en vez de comparar basura
    {
        alignas(16) uint8 b[0x900]{};
        At<uint32>(b,0x47C)=0x1234;
        auto* ped=reinterpret_cast<CPed*>(b); auto* intel=reinterpret_cast<CPedIntelligence*>(b); auto* hold=reinterpret_cast<CTaskSimpleHoldEntity*>(b);
        auto* pm=reinterpret_cast<CPedModelInfo*>(b); auto* cam=reinterpret_cast<CCam*>(b); auto* cd=reinterpret_cast<CCollisionData*>(b); auto* cm=reinterpret_cast<CColModel*>(b);
        auto* rc=reinterpret_cast<RwCamera*>(b); auto* ev=reinterpret_cast<CVehicle*>(b); auto* au=reinterpret_cast<CAutomobile*>(b); auto* bk=reinterpret_cast<CBike*>(b);
        auto off=[&](const void* p){ return intptr_t(reinterpret_cast<const uint8*>(p)-b); };
        const std::array<std::pair<const char*,intptr_t>,16> lay{{
            {"cdSpheres",off(&cd->m_pSpheres)-8},{"cdBoxes",off(&cd->m_pBoxes)-0xC},{"cdVerts",off(&cd->m_pVertices)-0x14},{"cdTris",off(&cd->m_pTriangles)-0x18},
            {"cdPlanes",off(&cd->m_pTrianglePlanes)-0x1C},{"cmData",off(&cm->m_pColData)-0x2C},{"intelTm",off(&intel->m_TaskMgr)-4},{"holdEnt",off(&hold->m_pEntityToHold)-8},
            {"pedHit",off(&pm->m_pHitColModel)-0x34},{"camFront",off(&cam->m_vecFront)-0x190},{"camSource",off(&cam->m_vecSource)-0x19C},{"rwNear",off(&rc->nearPlane)-0x80},
            {"vehType",off(&ev->m_nVehicleType)-0x590},{"autoWheels",off(&au->m_apWheelCollisionEntity)-0x8E8},{"bikeGround",off(&bk->m_aGroundPhysicalPtrs)-0x7C0},
            {"pedIntel",intptr_t(reinterpret_cast<uintptr_t>(ped->GetIntelligence()))-0x1234}}};
        for(auto& [nm,d]:lay)if(d!=0){ Write({{"event","error"},{"reason","layout"},{"field",nm},{"delta",d}}); return; }
        if(reinterpret_cast<uintptr_t>(&TheCamera.m_aCams[0])!=0xB6F19C||reinterpret_cast<uintptr_t>(&TheCamera.m_nActiveCam)!=0xB6F081||reinterpret_cast<uintptr_t>(&Scene.m_pRwCamera)!=0xC1703C
           ||reinterpret_cast<uintptr_t>(&TheCamera.m_mCameraMatrix.GetPosition())!=0xB6F9CC||reinterpret_cast<uintptr_t>(&CWorld::ms_aSectors)!=A_SECTORS
           ||reinterpret_cast<uintptr_t>(&CWorld::ms_aRepeatSectors)!=A_REPEAT||sizeof(CCam)!=0x238||sizeof(CRepeatSector)!=12||sizeof(CSector)!=8){
            Write({{"event","error"},{"reason","layout"},{"field","globals"}}); return; }
    }
    std::array<ReversibleHooks::ReversibleHook::Base*,20> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,20> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    InitRanges(); RegisterBuffers(); const auto saved=Dump();
    Write({{"event","player_ready"},{"fp_control",_controlfp(0,0)}});
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        // Cuerpos DLL resueltos antes de escribir ningun parche.
        struct P { uintptr_t exe; uintptr_t dll; uintptr_t stub; };
        const std::vector<P> plan{
            {0x4113F0,bit_cast<uintptr_t>(&CColStore::Initialise),bit_cast<uintptr_t>(&SInitialise)},
            {0x4114D0,bit_cast<uintptr_t>(&CColStore::Shutdown),bit_cast<uintptr_t>(&SColShutdown)},
            {0x410860,bit_cast<uintptr_t>(&CColStore::LoadCollision),bit_cast<uintptr_t>(&SLoadCollision)},
            {0x40EA10,bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),bit_cast<uintptr_t>(&SLoadAll)},
            {0x40F6A0,bit_cast<uintptr_t>(&CCollisionData::RemoveTrianglePlanes),bit_cast<uintptr_t>(&SRemovePlanes)},
            {0x5DF8F0,bit_cast<uintptr_t>(&CPed::IsPlayer),bit_cast<uintptr_t>(&SIsPlayer)},
            {0x534250,bit_cast<uintptr_t>(static_cast<CVector(CEntity::*)() const>(&CEntity::GetBoundCentre)),bit_cast<uintptr_t>(&SBoundCentre)},
            {0x600FF0,bit_cast<uintptr_t>(&CPedIntelligence::GetTaskHold),bit_cast<uintptr_t>(&SGetTaskHold)},
            {0x571B70,bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),bit_cast<uintptr_t>(&SRegisterReference)},
            {0x571A00,bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),bit_cast<uintptr_t>(&SCleanUpOldReference)},
            {0x4C7170,bit_cast<uintptr_t>(&CPedModelInfo::AnimatePedColModelSkinnedWorld),bit_cast<uintptr_t>(&SAnimate)},
            {0x50CE80,bit_cast<uintptr_t>(&CCamera::IsExtraEntityToIgnore),bit_cast<uintptr_t>(&SIsExtra)},
            {0x532D40,bit_cast<uintptr_t>(&CEntity::DoesNotCollideWithFlyers),bit_cast<uintptr_t>(&SNoFlyers)},
            {0x563F40,bit_cast<uintptr_t>(&CWorld::CameraToIgnoreThisObject),bit_cast<uintptr_t>(&SCameraToIgnore)},
            {0x56E210,bit_cast<uintptr_t>(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),bit_cast<uintptr_t>(&SFindPlayerPed)},
            {0x56E0D0,bit_cast<uintptr_t>(static_cast<CVehicle*(*)(int32,bool)>(&FindPlayerVehicle)),bit_cast<uintptr_t>(&SFindPlayerVehicle)},
            {0x56E090,bit_cast<uintptr_t>(static_cast<CVector&(*)(int32)>(&FindPlayerSpeed)),bit_cast<uintptr_t>(&SFindPlayerSpeed)},
            {0x681740,bit_cast<uintptr_t>(&CTaskManager::FindActiveTaskByType),bit_cast<uintptr_t>(&SFindActiveTask)},
            {0x63A690,bit_cast<uintptr_t>(&CTaskComplexEnterCar::GetCameraAvoidVehicle),bit_cast<uintptr_t>(&SAvoidVehicle)},
            {0x563470,bit_cast<uintptr_t>(&CWorld::ClearScanCodes),bit_cast<uintptr_t>(&SClearScanCodes)},
            {0x416330,bit_cast<uintptr_t>(static_cast<void(*)(CCollisionData*)>(&CCollision::CalculateTrianglePlanes)),bit_cast<uintptr_t>(&SCalcPlanes)},
        };
        std::vector<std::unique_ptr<EntryPatch>> patches;
        for(const auto& p:plan){ patches.push_back(std::make_unique<EntryPatch>(p.exe,reinterpret_cast<void*>(p.stub))); patches.back()->Enable(false);
                                 patches.push_back(std::make_unique<EntryPatch>(p.dll,reinterpret_cast<void*>(p.stub))); patches.back()->Enable(false); }
        auto stubs=[&](bool on){ for(auto& p:patches)p->Enable(on); };
        std::vector<uint8> dumps[2];
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    Rng r; r.s=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x59C0u; if(!r.s)r.s=1; trn=0;
                    Restore(saved);
                    json out=json::object(); json ret=nullptr; json pre=json::object(); const uintptr_t a=fn.address;
                    // Activa parches y hooks, comprueba el conmutador DESPUES de State y empieza una traza limpia.
                    auto arm=[&]{ stubs(true); trn=0; for(auto* h:hooks)h->State(candidate); out["switches"]=ReversingProbe::JumpsToDll(a)==candidate; };

                    if(nm=="Init"||nm=="Shutdown"||nm=="Update"){
                        // lista de enlaces: cabecera aleatoria (los campos que la funcion no escribe se conservan) y, para Shutdown, lista usada falsa
                        for(int k=0;k<0x34;k+=4)G<uint32>(0x96592C+k)=r();
                        G<uint32>(0x9655D4)=r();
                        uint32 base=0;
                        if(nm=="Shutdown"){
                            void* block=reinterpret_cast<void*(__cdecl*)(uint32)>(0x821195)(0x258); std::memset(block,0,0x258); base=U(block);
                            uint32 idx[50]; for(int k=0;k<50;++k)idx[k]=uint32(k); for(int k=49;k>0;--k)std::swap(idx[k],idx[r.n(k+1)]);
                            const int nUsed=r.one(8) ? 0 : r.n(51); uint32 prev=0x96592C;
                            for(int k=0;k<nUsed;++k){ const uint32 link=base+12*idx[k]; G<uint32>(link)=r.one(4) ? 0 : U(cdBuf[r.n(NM+1)].data()); G<uint32>(link+4)=prev; G<uint32>(prev+8)=link; prev=link; }
                            G<uint32>(prev+8)=0x965938; G<uint32>(0x965938+4)=prev; G<uint32>(0x96595C)=base; pre["used"]=nUsed; }
                        arm(); reinterpret_cast<void(__cdecl*)()>(a)(); stubs(false);
                        uint32 lo=0, hi=0; const uint32 links=G<uint32>(0x96595C);
                        if(nm=="Init"){ lo=links; hi=links+0x258; } else if(nm=="Shutdown"){ lo=base; hi=base+0x258; }
                        auto nrm=[&](uint32 v)->json{ return (lo&&v>=lo&&v<hi) ? json("L+"+std::to_string(v-lo)) : json(v); };
                        json norm=json::array(); for(int k=0;k<0x34;k+=4)norm.push_back(k==0x30 ? (nm=="Init" ? json(links ? "links" : "null") : json(links)) : nrm(G<uint32>(0x96592C+k)));
                        // Init: estado de los enlaces nuevos (el campo data es basura del monton y no se compara)
                        if(nm=="Init"&&links)for(int k=0;k<50;++k){ norm.push_back(nrm(G<uint32>(links+12*k+4))); norm.push_back(nrm(G<uint32>(links+12*k+8))); }
                        out["norm"]=norm;
                        if(nm=="Init"&&links)reinterpret_cast<void(__cdecl*)(void*)>(0x8213AE)(reinterpret_cast<void*>(links));
                        if(nm=="Shutdown"&&links){ out["leaked"]=true; reinterpret_cast<void(__cdecl*)(void*)>(0x8213AE)(reinterpret_cast<void*>(links)); }
                        for(int k=0;k<0x34;k+=4)G<uint32>(0x96592C+k)=0; // los punteros reservados difieren entre ejecuciones: ya normalizados arriba
                    }
                    else if(nm=="SortOutCollisionAfterLoad"){
                        G<CVector>(0xB6F9CC)=r.vec(r.one(4) ? 4000.f : 300.f);
                        arm(); reinterpret_cast<void(__cdecl*)()>(a)(); stubs(false);
                    }
                    else if(nm=="IsThisVehicleSittingOnMe"){
                        for(int k=0;k<NV;++k)MakeVeh(r,k,CVector{0,0,0});
                        SetPools();
                        uint8* me=r.one(8) ? nullptr : VehP(r.n(NV)); uint8* other=VehP(r.n(NV)); uint8* veh=r.one(6) ? nullptr : r.one(2) ? other : VehP(r.n(NV));
                        if(me){ for(int w=0;w<4;++w){ At<uint32>(me,0x8E8+4*w)=r.one(3) ? 0 : U(r.one(2) ? veh : VehP(r.n(NV))); At<uint32>(me,0x7C0+4*w)=r.one(3) ? 0 : U(r.one(2) ? veh : VehP(r.n(NV))); } }
                        pre["args"]=json::array({Name(U(veh)),Name(U(me))});
                        arm(); ret=reinterpret_cast<bool(__cdecl*)(CVehicle*,CVehicle*)>(a)(reinterpret_cast<CVehicle*>(veh),reinterpret_cast<CVehicle*>(me)); stubs(false);
                    }
                    else if(nm=="ResetMadeInvisibleObjects"){
                        BuildWorld(r,RandCenter(r),0,true,14.f);
                        const int cnt=r.one(8) ? 0 : r.n(11); G<uint32>(0x9655DC)=uint32(cnt);
                        for(int k=0;k<10;++k){ uint8* e=(k<cnt&&!r.one(5)) ? (r.one(2) ? PedP(r.n(NP)) : r.one(2) ? ObjP(r.n(NO)) : BldP(r.n(NB))) : nullptr; G<uint32>(0x9655A0+4*k)=U(e); if(e&&r.one(2))At<uint32>(e,0x1C)&=~0x80u; }
                        arm(); reinterpret_cast<void(__cdecl*)()>(a)(); stubs(false);
                    }
                    else if(nm=="CheckCameraCollisionPeds"||nm=="CheckPeds"){
                        const CVector c=RandCenter(r); BuildWorld(r,c,1,true,14.f);
                        // los 4 primeros peatones muy cerca del punto (dentro del cilindro de 1.0); el resto lejos (como mucho 4 + 2 retenidos)
                        const CVector pos=Add3(c,r.vec(0.9f));
                        for(int k=0;k<NP;++k)At<CVector>(PedP(k),4)= k<4&&r.n(4) ? Add3(pos,CVector{r.uni(-1.2f,1.2f),r.uni(-1.2f,1.2f),0.f}) : Add3(pos,CVector{500.f,500.f,0.f});
                        G<uint32>(0x9655DC)=r.one(4) ? uint32(r.n(4)) : 0u;
                        CVector normal=r.vec(1.f); float nearest=r.uni(0.f,5.f);
                        pre["pos"]=json::array({F(pos.x),F(pos.y),F(pos.z)});
                        if(nm=="CheckPeds"){
                            const int sx=int(std::floor(c.x*0.02f+60.f))+(r.one(5) ? -70 : 0), sy=int(std::floor(c.y*0.02f+60.f))+(r.one(5) ? -70 : 0); const int w=r.n(3), h=r.n(3);
                            G<int32>(0x96559C)=sx; G<int32>(0x965598)=sy; G<int32>(0x965594)=sx+w; G<int32>(0x965590)=sy+h;
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const CVector*,const CVector*,float*)>(a)(&pos,&normal,&nearest); }
                        else {
                            const int sx=int(std::floor(c.x*0.02f+60.f))+r.n(3)-1-(r.one(6) ? 40 : 0), sy=int(std::floor(c.y*0.02f+60.f))+r.n(3)-1-(r.one(6) ? 40 : 0); pre["sector"]=json::array({sx,sy});
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(int32,int32,const CVector*,const CVector*,float*)>(a)(sx,sy,&pos,&normal,&nearest); }
                        stubs(false); out["nearest"]=F(nearest);
                    }
                    else if(nm=="SphereCastVsBBox"||nm=="SphereCastVsSphere"||nm=="SphereCastVersusVsPoly"){
                        const CVector c=r.vec(3.f); RandSphere(r,spAb.data(),c,r.one(6) ? 0.f : 2.f,0.f,r.one(8) ? 0.f : 2.5f); RandSphere(r,spBb.data(),c,r.one(6) ? 0.f : 3.f,0.f,r.one(8) ? 0.f : 2.5f);
                        if(r.one(4))At<float>(spBb.data(),0xC)=At<float>(spAb.data(),0xC);
                        if(nm=="SphereCastVsBBox"){
                            std::memset(boxb.data(),0,boxb.size()); const CVector lo=r.vec(2.5f); At<CVector>(boxb.data(),0)=lo; At<CVector>(boxb.data(),0xC)=r.one(10) ? lo : Add3(lo,CVector{r.uni(0.f,3.f),r.uni(0.f,3.f),r.uni(0.f,3.f)});
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,const void*)>(a)(spAb.data(),spBb.data(),boxb.data()); }
                        else if(nm=="SphereCastVsSphere"){
                            RandSphere(r,spSb.data(),c,2.5f,0.f,2.5f);
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,const void*)>(a)(spAb.data(),spBb.data(),spSb.data()); }
                        else {
                            // triangulo y plano: el plano sale de los vertices (80%) o es aleatorio
                            std::memset(vertsB.data(),0,vertsB.size()); std::memset(triB.data(),0,triB.size()); std::memset(planeB.data(),0,planeB.size());
                            for(int k=0;k<8;++k)for(int j=0;j<3;++j)At<int16>(vertsB.data(),6*k+2*j)=int16(int(r()%700)-350);
                            At<uint16>(triB.data(),0)=uint16(r.n(8)); At<uint16>(triB.data(),2)=uint16(r.n(8)); At<uint16>(triB.data(),4)=uint16(r.n(8));
                            const auto vv=[&](int k){ return CVector{At<int16>(vertsB.data(),6*k)/128.f,At<int16>(vertsB.data(),6*k+2)/128.f,At<int16>(vertsB.data(),6*k+4)/128.f}; };
                            const CVector va=vv(At<uint16>(triB.data(),0)), vb=vv(At<uint16>(triB.data(),2)), vc=vv(At<uint16>(triB.data(),4));
                            if(r.n(5))MakePlane(planeB.data(),va,vb,vc); else for(int k=0;k<4;++k)At<int16>(planeB.data(),2*k)=int16(int(r()%8193)-4096);
                            if(r.one(3)){ CVector n=Cross3(Sub3(vb,va),Sub3(vc,va)); const float len=std::sqrt(n.x*n.x+n.y*n.y+n.z*n.z); if(len>1e-6f){ n=Mul3(n,1.f/len);
                                const CVector p=Add3(Add3(va,Mul3(Sub3(vb,va),r.uni(0.f,1.f))),Mul3(Sub3(vc,va),r.uni(0.f,0.5f)));
                                At<CVector>(spAb.data(),0)=Add3(p,Mul3(n,r.uni(-2.f,3.f))); At<CVector>(spBb.data(),0)=Sub3(At<CVector>(spAb.data(),0),Mul3(n,r.uni(0.f,6.f))); } }
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,const void*,const void*,const void*)>(a)(spAb.data(),spBb.data(),triB.data(),planeB.data(),vertsB.data()); }
                        stubs(false);
                    }
                    else if(nm=="TestLineOfSight"||nm=="ProcessLineOfSight"){
                        BuildModels(r); const int m=r.n(NM); auto* mt=mats[0].data(); MakeMat(r,mt,r.vec(10.f));
                        auto wp=[&](CVector l){ return CVector{ At<CVector>(mt,0x20).x*l.z+At<CVector>(mt,0x10).x*l.y+At<CVector>(mt,0).x*l.x+At<CVector>(mt,0x30).x,
                            At<CVector>(mt,0x20).y*l.z+At<CVector>(mt,0).y*l.x+At<CVector>(mt,0x10).y*l.y+At<CVector>(mt,0x30).y,
                            At<CVector>(mt,0x20).z*l.z+At<CVector>(mt,0).z*l.x+At<CVector>(mt,0x10).z*l.y+At<CVector>(mt,0x30).z }; };
                        std::memset(lineB.data(),0,lineB.size()); At<CVector>(lineB.data(),0)=wp(r.vec(9.f)); At<CVector>(lineB.data(),0x10)=r.one(8) ? At<CVector>(lineB.data(),0) : wp(r.vec(9.f));
                        const bool see=r.n(3)==0, shoot=r.n(3)==0; float maxT=r.one(4) ? 1e24f : r.uni(0.5f,30.f); std::memset(colPt.data(),0xCD,colPt.size());
                        G<uint32>(0x9655D0)=uint32(r.n(50)); pre["flags"]=json::array({see,shoot,F(maxT),m});
                        arm();
                        if(nm=="TestLineOfSight") ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,void*,bool,bool)>(a)(lineB.data(),mats[0].data(),cmBuf[m].data(),see,shoot);
                        else { ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,void*,void*,float*,bool,bool)>(a)(lineB.data(),mats[0].data(),cmBuf[m].data(),colPt.data(),&maxT,see,shoot); out["maxT"]=F(maxT); out["cp"]=Cps(colPt); }
                        stubs(false);
                    }
                    else if(nm=="SphereCastVsEntity"||nm=="SphereCastVsCaches"){
                        const CVector c=r.vec(3.f); BuildWorld(r,c,0,false,2.5f);
                        RandSphere(r,spAb.data(),c,1.5f,0.1f,1.6f); RandSphere(r,spBb.data(),c,2.5f,0.1f,1.6f); if(r.one(3))At<float>(spBb.data(),0xC)=At<float>(spAb.data(),0xC);
                        std::memset(cacheC.data(),0xCD,cacheC.size());
                        if(nm=="SphereCastVsEntity"){
                            uint8* ent=r.one(3) ? VehP(r.n(NV)) : r.one(2) ? ObjP(r.n(NO)) : BldP(r.n(NB)); G<uint32>(0x9655D8)=r.one(8) ? uint32(98+r.n(2)) : uint32(r.n(60)); G<uint8>(0x9655E4)=uint8(r.n(2));
                            pre["ent"]=Name(U(ent));
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,CEntity*)>(a)(spAb.data(),spBb.data(),E(ent)); stubs(false); out["n"]=G<uint32>(0x9655D8); }
                        else {
                            // cache de entrada: regiones de 1..3 entidades con colision y entradas validas para sus indices
                            int numIn=0; auto* in=cacheB.data(); const int regions=1+r.n(3); std::memset(in,0xCD,cacheB.size());
                            for(int g=0;g<regions&&numIn<NCACHE-8;++g){
                                uint8* ent=r.one(3) ? VehP(r.n(NV)) : r.one(2) ? ObjP(r.n(NO)) : BldP(r.n(NB)); const int ms=At<int16>(ent,0x22)-1;
                                if(!At<uint32>(cmBuf[ms].data(),0x2C))continue; // el EXE no admite regiones sin datos de colision
                                auto* cd=cdBuf[ms].data(); const int nS=At<uint16>(cd,0), nBx=At<uint16>(cd,2), nT=At<uint16>(cd,4); const int ne=1+r.n(5);
                                for(int q=0;q<ne&&numIn<NCACHE-1;++q){
                                    uint8* e=in+16*numIn; std::memset(e,0xAB,16); std::vector<int> opts; if(nT)opts.push_back(1); if(nS)opts.push_back(2); if(nBx)opts.push_back(3);
                                    const int type=opts.empty() ? 0 : opts[size_t(r.n(int(opts.size())))]; e[0]=uint8(type); At<uint32>(e,4)=q==0 ? U(ent) : 0u;
                                    if(type==1){ int idx=r.n(nT); if(r.one(3))idx=0xFFFF-idx; At<uint16>(e,8)=uint16(idx); } else if(type==2)At<uint16>(e,0xA)=uint16(r.n(nS)); else if(type==3)At<uint16>(e,0xC)=uint16(r.n(nBx));
                                    ++numIn; } }
                            int numOut=r.one(4) ? r.n(10) : 0; CVector vel=r.vec(r.one(4) ? 0.f : 2.5f);
                            pre["numIn"]=numIn; pre["numOut0"]=numOut;
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const void*,const CVector*,int32,void*,int32*,void*)>(a)(spAb.data(),&vel,numIn,in,&numOut,cacheC.data()); stubs(false);
                            out["numOut"]=numOut; out["cache"]=CacheJson(cacheC.data(),std::min(numOut+1,NCACHE)); }
                    }
                    else { // CheckCameraCollision{Buildings,Vehicles,Objects}, BuildCacheOfCameraCollision, CameraConeCastVsWorldCollision
                        const bool big=nm=="BuildCacheOfCameraCollision"||nm=="CameraConeCastVsWorldCollision";
                        const CVector c=RandCenter(r); BuildWorld(r,c,big ? 2 : 1,false,9.f);
                        RandSphere(r,spAb.data(),c,1.f,0.2f,2.f); RandSphere(r,spBb.data(),c,r.one(5) ? 0.f : 5.f,0.2f,2.f); if(r.one(2))At<float>(spBb.data(),0xC)=At<float>(spAb.data(),0xC);
                        RandSphere(r,spSb.data(),c,2.f,2.f,12.f); std::memset(boxb.data(),0,boxb.size());
                        const int sx=int(std::floor(c.x*0.02f+60.f)), sy=int(std::floor(c.y*0.02f+60.f));
                        if(nm=="CheckCameraCollisionBuildings"||nm=="CheckCameraCollisionObjects"){
                            const int X=sx+r.n(3)-1, Y=sy+r.n(3)-1; pre["xy"]=json::array({X,Y});
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(int32,int32,const void*,const void*,const void*,const void*)>(a)(X,Y,boxb.data(),spSb.data(),spAb.data(),spBb.data()); }
                        else if(nm=="CheckCameraCollisionVehicles"){
                            const int X=sx+r.n(3)-1, Y=sy+r.n(3)-1; pre["xy"]=json::array({X,Y}); CVector vel=r.vec(0.6f); const bool useVel=r.n(3)!=0;
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(int32,int32,const void*,const void*,const void*,const void*,const CVector*)>(a)(X,Y,boxb.data(),spSb.data(),spAb.data(),spBb.data(),useVel ? &vel : nullptr); }
                        else if(nm=="BuildCacheOfCameraCollision"){
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*)>(a)(spAb.data(),spBb.data()); }
                        else {
                            float dst=r.uni(-1.f,1.f); const float minDist=r.one(6) ? 0.f : r.uni(0.f,0.9f); pre["minDist"]=F(minDist);
                            arm(); ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,float*,float)>(a)(spAb.data(),spBb.data(),&dst,minDist); out["dst"]=F(dst);
                            G<uint32>(0x9655CC)=0; G<uint32>(0x9655C8)=0; } // apuntan a las cachés de la pila de la funcion: direcciones distintas por ejecucion
                        stubs(false); out["n"]=G<uint32>(0x9655D8);
                    }
                    out["trace"]=TraceJson(); out["pre"]=pre;
                    auto d=Dump(); out["state"]=Fnv(d.data(),d.size()); dumps[candidate]=std::move(d);
                    out["ret"]=ret; Restore(saved); return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["trace"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_diffs",Diffs(dumps[0],dumps[1])},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        stubs(false); Restore(saved);
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Restore(saved);
    const bool restored=Dump()==saved;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
