#pragma once
// Banco dentro del motor: CWorld, grupo A (busquedas de entidades por sector; 15 funciones):
// FindObjectsInRange, FindObjectsOfTypeInRange, FindLodOfTypeInRange, FindObjectsKindaColliding, FindObjectsIntersectingCube,
// FindObjectsIntersectingAngledCollisionBox, FindMissionEntitiesIntersectingCube, FindNearestObjectOfType y las siete
// funciones de lista de sector <CPtrListDoubleLink<CPhysical*>> (se enganchan por direccion).
// Original (hooks del grupo desactivados) frente a candidato (DLL) con fixtures identicos y comparacion bit a bit.
// Mundo FALSO: las listas de sectores, de repeticion y LOD se vacian y se rellenan con nodos de pools LOCALES (simple y doble) y las
// entidades son bloques de 0xA20 bytes en memoria propia cubiertos por pools de entidades FALSOS (la DLL de depuracion valida nodos e Items).
// Modelos FALSOS (indices 19990..19995 con su modelo de colision) en ms_modelInfoPtrs; nada del juego real se toca.
// ClearScanCodes (que recorre las listas falsas) corre de verdad en las dos ejecuciones (con el hook del grupo C desactivado en el baseline).
// Estado global tocado: se guarda y se restaura antes de cada ejecucion y al final (punteros de pools, sectores, repeticion, LOD, scan code
// actual y las entradas de ms_modelInfoPtrs).
#include <fstream>
#include <functional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Entity.h"
#include "Physical.h"
#include "Ped.h"
#include "Building.h"
#include "Dummy.h"
#include "CutsceneMgr.h"
#include "Object.h"
#include "Vehicle.h"
#include "World.h"
#include "Models/ModelInfo.h"
#include "Models/BaseModelInfo.h"
#include "Collision/ColModel.h"
#include "Pools/PtrNodeDoubleLinkPool.h"
#include "Pools/PtrNodeSingleLinkPool.h"
#include "../ProbeRuntime.hpp"

namespace WldALive {
inline bool done{}; inline std::ofstream log; inline DWORD mainThread{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class V=uint32> inline V& At(void* base,size_t off) { return *reinterpret_cast<V*>(static_cast<uint8*>(base)+off); }
template<class V=uint32> inline V& AtA(uintptr_t addr) { return *reinterpret_cast<V*>(addr); }
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }

// --- direcciones del EXE ---
constexpr uintptr_t MI_PTRS=0xA9B0C8, POOLS=0xB74484, SECTORS=0xB7D0B8, REPEAT=0xB992B8, LODL=0xB99EB8, SCAN=0xB7CD78;
constexpr size_t SECTORS_SIZE=120*120*8, REPEAT_SIZE=16*16*12, LOD_SIZE=30*30*4, POOLS_SIZE=0x20;
constexpr int MODEL0=19990, NM=6;
constexpr size_t ES=0xA20; constexpr int NE=24, NDBL=8192, NSGL=8192;

// --- memoria falsa (direcciones fijas: iguales en las dos ejecuciones) ---
alignas(16) inline std::array<uint8,ES*NE> entMem{};
inline uint8* E(int k) { return entMem.data()+size_t(k)*ES; }
inline std::array<uint8,ES*NE/0x38+1> entStates{};
alignas(16) inline std::array<std::array<uint8,0x60>,NE> mats{};
alignas(16) inline std::array<std::array<uint8,0x100>,NM> miBuf{};
alignas(16) inline std::array<std::array<uint8,0x30>,NM> colBuf{};
alignas(16) inline std::array<uint8,0x100> outBuf{};
alignas(16) inline std::array<uint8,0x60> rotMat{}, boxBuf{};
alignas(16) inline std::array<uint8,NDBL*12> dblStore{}; inline std::array<uint8,NDBL> dblStates{};
alignas(16) inline std::array<uint8,NSGL*8> sglStore{}; inline std::array<uint8,NSGL> sglStates{};
alignas(16) inline std::array<uint8,0x20> dblHdr{}, sglHdr{}, dblImg{}, sglImg{};
alignas(16) inline std::array<uint8,0x14> vehHdr{}, pedHdr{}, objHdr{}, bldHdr{}, dumHdr{};
inline std::array<void*,32> miVt{};

inline std::string Name(const void* p) {
    const auto u=uintptr_t(p); if(!u)return "null";
    if(u>=uintptr_t(entMem.data())&&u<uintptr_t(entMem.data())+entMem.size())return std::format("e#{}+{:x}",(u-uintptr_t(entMem.data()))/ES,(u-uintptr_t(entMem.data()))%ES);
    return std::format("{:08x}",u);
}
template<int K> void __fastcall VTrap(void* self,void*) {
    Write({{"event","error"},{"reason","unexpected virtual call"},{"slot",K},{"self",Name(self)}}); log.flush(); ExitProcess(95);
}
template<int... K> std::array<void*,sizeof...(K)> TrapTable(std::integer_sequence<int,K...>) { return {reinterpret_cast<void*>(&VTrap<K>)...}; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=15;
inline const std::array<Fn,NFNS> fns{{
    {"CWorld","FindObjectsInRange",0x564A20,0,-1,640},{"CWorld","FindObjectsInRangeSectorList<CPtrListDoubleLink<CPhysical*>>",0x563500,0,-1,512},
    {"CWorld","FindObjectsOfTypeInRange",0x564C70,0,-1,640},{"CWorld","FindObjectsOfTypeInRangeSectorList<CPtrListDoubleLink<CPhysical*>>",0x5635C0,0,-1,512},
    {"CWorld","FindLodOfTypeInRange",0x564ED0,0,-1,640},
    {"CWorld","FindObjectsKindaColliding",0x568B80,0,-1,640},{"CWorld","FindObjectsKindaCollidingSectorList<CPtrListDoubleLink<CPhysical*>>",0x565000,0,-1,512},
    {"CWorld","FindObjectsIntersectingCube",0x568DD0,0,-1,640},{"CWorld","FindObjectsIntersectingCubeSectorList<CPtrListDoubleLink<CPhysical*>>",0x5650E0,0,-1,512},
    {"CWorld","FindObjectsIntersectingAngledCollisionBox",0x568FF0,0,-1,640},{"CWorld","FindObjectsIntersectingAngledCollisionBoxSectorList<CPtrListDoubleLink<CPhysical*>>",0x565200,0,-1,512},
    {"CWorld","FindMissionEntitiesIntersectingCube",0x569240,0,-1,640},{"CWorld","FindMissionEntitiesIntersectingCubeSectorList<CPtrListDoubleLink<CPhysical*>>",0x565300,0,-1,512},
    {"CWorld","FindNearestObjectOfType",0x5693F0,0,-1,640},{"CWorld","FindNearestObjectOfTypeSectorList<CPtrListDoubleLink<CPhysical*>>",0x565450,0,-1,512},
}};

// comprobacion de la disposicion de campos de la DLL frente al EXE (solo direcciones, sin leer nada)
inline std::string LayoutErrors() {
    std::string why;
    auto* e=reinterpret_cast<CEntity*>(E(0)); auto* mi=reinterpret_cast<CBaseModelInfo*>(miBuf[0].data()); auto* cm=reinterpret_cast<CColModel*>(colBuf[0].data());
    auto chk=[&](const char* nm,const void* addr,const void* base,size_t off){ if(uintptr_t(addr)-uintptr_t(base)!=off){ why+=nm; why+=' '; } };
    auto chkAbs=[&](const char* nm,const void* addr,uintptr_t want){ if(uintptr_t(addr)!=want){ why+=nm; why+=' '; } };
    chk("m_placement",&e->m_placement,e,4); chk("m_matrix",&e->m_matrix,e,0x14); chk("m_nModelIndex",&e->m_nModelIndex,e,0x22);
    chk("mi.m_pColModel",&mi->m_pColModel,mi,0x14); chk("cm.center",&cm->m_boundSphere.m_vecCenter,cm,0x18); chk("cm.radius",&cm->m_boundSphere.m_fRadius,cm,0x24);
    chkAbs("sectors",&CWorld::ms_aSectors,SECTORS); chkAbs("repeat",&CWorld::ms_aRepeatSectors,REPEAT); chkAbs("lod",&CWorld::ms_aLodPtrLists,LODL);
    chkAbs("scan",&CWorld::ms_nCurrentScanCode,SCAN);
    if(sizeof(CRepeatSector)!=12)why+="sizeof(CRepeatSector) "; if(sizeof(CSector)!=8)why+="sizeof(CSector) ";
    return why;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"wlda-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"wlda-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("wlda");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainThread=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"wlda-only.txt"); if(f)std::getline(f,only); }
    if(auto bad=LayoutErrors(); !bad.empty()){Write({{"event","error"},{"reason","layout mismatch"},{"fields",bad}});return;}
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    // ClearScanCodes (grupo C) corre de verdad con las listas falsas: en el baseline se usa la del EXE
    auto* const clearHook=FindHook("CWorld","ClearScanCodes"); bool clearEnabled{}, clearLocked{};
    if(clearHook){ clearEnabled=clearHook->Hooked(); clearLocked=clearHook->Locked(); clearHook->LockState(false); }
    auto setState=[&](bool on){ for(auto* h:hooks)h->State(on); if(clearHook)clearHook->State(on); };

    { const auto mtraps=TrapTable(std::make_integer_sequence<int,32>{}); for(int k=0;k<32;++k)miVt[k]=mtraps[k]; }

    // --- estado global guardado ---
    struct Blob { uintptr_t at; std::vector<uint8> data; };
    std::vector<Blob> blobs;
    auto Save=[&](uintptr_t at,size_t n){ blobs.push_back({at,std::vector<uint8>(reinterpret_cast<uint8*>(at),reinterpret_cast<uint8*>(at)+n)}); };
    Save(MI_PTRS+size_t(MODEL0)*4,size_t(NM)*4); Save(POOLS,POOLS_SIZE); Save(SECTORS,SECTORS_SIZE); Save(REPEAT,REPEAT_SIZE); Save(LODL,LOD_SIZE); Save(SCAN,2);
    auto RestoreAll=[&]{ for(auto& b:blobs)std::memcpy(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size()); };
    auto Restored=[&]{ bool ok=true; for(auto& b:blobs)ok&=std::memcmp(reinterpret_cast<void*>(b.at),b.data.data(),b.data.size())==0; return ok; };

    // pools de nodos locales: cabecera construida una sola vez; pools de entidades falsos que cubren el bloque de entidades
    new (dblHdr.data()) CPtrNodeDoubleLinkPool(NDBL,dblStore.data(),dblStates.data()); dblImg=dblHdr;
    new (sglHdr.data()) CPtrNodeSingleLinkPool(NSGL,sglStore.data(),sglStates.data()); sglImg=sglHdr;
    auto PoolHdr=[&](std::array<uint8,0x14>& h,size_t stride){ std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=entMem.data(); At<void*>(h.data(),4)=entStates.data();
                                                             At<int32>(h.data(),8)=int32(entMem.size()/stride); At<int32>(h.data(),0xC)=-1; };
    PoolHdr(vehHdr,0x38); PoolHdr(pedHdr,0x38); PoolHdr(objHdr,0x38); PoolHdr(bldHdr,0x38); PoolHdr(dumHdr,0x38);
    auto* const poolPtrs=reinterpret_cast<void**>(POOLS); // 0 simple, 1 doble, 2 entry, 3 peatones, 4 vehiculos, 5 edificios, 6 objetos, 7 dummies

    bool passed=true; int pair=0;
    {
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string nm=fn.name, full=std::string(fn.cls)+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x7A7Au; if(!st)st=1;
                    RestoreAll();
                    std::memcpy(dblHdr.data(),dblImg.data(),dblImg.size()); dblStore.fill(0xFD); dblStates.fill(0x80);
                    std::memcpy(sglHdr.data(),sglImg.data(),sglImg.size()); sglStore.fill(0xFD); sglStates.fill(0x80);
                    entStates.fill(0);
                    poolPtrs[0]=sglHdr.data(); poolPtrs[1]=dblHdr.data(); poolPtrs[3]=pedHdr.data(); poolPtrs[4]=vehHdr.data(); poolPtrs[5]=bldHdr.data(); poolPtrs[6]=objHdr.data(); poolPtrs[7]=dumHdr.data();
                    std::memset(reinterpret_cast<void*>(SECTORS),0,SECTORS_SIZE); std::memset(reinterpret_cast<void*>(REPEAT),0,REPEAT_SIZE); std::memset(reinterpret_cast<void*>(LODL),0,LOD_SIZE);
                    const std::string nmf=nm;
                    const bool isList=nmf.find("SectorList")!=std::string::npos;
                    const bool isLod=nmf=="FindLodOfTypeInRange";
                    // las funciones que no leen el modelo de informacion admiten indices de modelo >= 0x8000 (se leen con signo)
                    const bool negOk=nmf.rfind("FindObjectsInRange",0)==0||nmf.rfind("FindObjectsOfType",0)==0||nmf.rfind("FindNearest",0)==0||isLod;

                    // ---------- scan code actual ----------
                    { const uint32 m=next()%40; AtA<uint16>(SCAN)= m==0 ? uint16(0xFFFF) : m==1 ? uint16(0xFFFE) : m==2 ? uint16(0) : m==3 ? uint16(1) : uint16(next()); }
                    const uint16 cur0=AtA<uint16>(SCAN);
                    const uint16 expect=isList ? cur0 : (cur0>=65535u ? uint16(1) : uint16(cur0+1)); // codigo vigente durante el recorrido

                    // ---------- modelos falsos (indices 19990..19995) ----------
                    float mrad[NM]{};
                    for(int m=0;m<NM;++m){
                        auto& cb=colBuf[size_t(m)]; auto& mb=miBuf[size_t(m)];
                        std::memset(cb.data(),0,cb.size()); std::memset(mb.data(),0xCD,mb.size());
                        const uint32 mm=next()%8; mrad[m]= mm==0 ? 0.f : mm==1 ? uni(0.f,0.5f) : mm==2 ? uni(20.f,60.f) : uni(0.3f,15.f);
                        At<CVector>(cb.data(),0)=CVector{-uni(0.05f,6.f),-uni(0.05f,6.f),-uni(0.05f,4.f)}; At<CVector>(cb.data(),0xC)=CVector{uni(0.05f,6.f),uni(0.05f,6.f),uni(0.05f,4.f)};
                        At<CVector>(cb.data(),0x18)=CVector{uni(-3.f,3.f),uni(-3.f,3.f),uni(-3.f,3.f)}; At<float>(cb.data(),0x24)=mrad[m];
                        At<void*>(mb.data(),0)=miVt.data(); At<void*>(mb.data(),0x14)=cb.data();
                        AtA<void*>(MI_PTRS+size_t(MODEL0+m)*4)=mb.data();
                    }

                    // ---------- region y entidades ----------
                    float bx0,bx1,by0,by1;
                    { const uint32 r=next()%6;
                      if(isLod){ bx0=uni(-3150.f,2400.f); bx1=bx0+uni(10.f,500.f); by0=uni(-3150.f,2400.f); by1=by0+uni(10.f,500.f); }
                      else if(r<3){ bx0=-250.f; bx1=250.f; by0=-250.f; by1=250.f; }
                      else if(r==3){ bx0=-3300.f; bx1=3300.f; by0=-3300.f; by1=3300.f; }
                      else if(r==4){ bx0=2800.f; bx1=3200.f; by0=-3200.f; by1=-2800.f; }
                      else { const float cx=uni(-2900.f,2900.f), cy=uni(-2900.f,2900.f); bx0=cx-120.f; bx1=cx+120.f; by0=cy-120.f; by1=cy+120.f; } }
                    float epx[NE]{}, epy[NE]{}, epz[NE]{}; int etype[NE]{}, emodel[NE]{};
                    auto FillMat=[&](void* m,const CVector& pos,bool rotated){
                        auto unit=[&]{ CVector v{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float l=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z); return l<0.1f ? CVector{1.f,0.f,0.f} : CVector{v.x/l,v.y/l,v.z/l}; };
                        auto cross=[&](const CVector& a,const CVector& b){ return CVector{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; };
                        CVector r{1.f,0.f,0.f}, fw{0.f,1.f,0.f}, u{0.f,0.f,1.f};
                        if(rotated){ r=unit(); CVector u0=unit(); fw=cross(u0,r); const float l=std::sqrt(fw.x*fw.x+fw.y*fw.y+fw.z*fw.z); fw=l<0.05f ? CVector{0.f,1.f,0.f} : CVector{fw.x/l,fw.y/l,fw.z/l}; u=cross(r,fw);
                                     if(next()%3==0){ const float sx=uni(0.2f,3.f); r=CVector{r.x*sx,r.y*sx,r.z*sx}; } }
                        std::memset(m,0,0x48);
                        At<CVector>(m,0)=r; At<CVector>(m,0x10)=fw; At<CVector>(m,0x20)=u; At<CVector>(m,0x30)=pos;
                        for(size_t o:{size_t(0xC),size_t(0x1C),size_t(0x2C),size_t(0x3C)})At<uint32>(m,o)=0;
                    };
                    for(int k=0;k<NE;++k){
                        uint8* e=E(k); std::memset(e,0xCD,ES);
                        const int type=1+int(next()%5); etype[k]=type;
                        int model=MODEL0+int(next()%NM);
                        if(negOk&&next()%7==0){ static const int odd[]{0x8001,0xFFFF,0x8000,0xC000}; model=odd[next()%4]; }
                        emodel[k]=model;
                        epx[k]=uni(bx0,bx1); epy[k]=uni(by0,by1); epz[k]=next()%5==0 ? 0.f : uni(-20.f,120.f);
                        At<void*>(e,0)=miVt.data(); At<void*>(e,0x14)=nullptr; At<void*>(e,0x18)=nullptr;
                        At<uint32>(e,0x1C)=next(); At<uint16>(e,0x20)=uint16(next()); At<uint16>(e,0x22)=uint16(model); At<void*>(e,0x24)=nullptr;
                        At<uint16>(e,0x2C)= next()%4==0 ? expect : next()%5==0 ? cur0 : uint16(next());
                        At<uint8>(e,0x36)=uint8(type|((next()%32)<<3));
                        At<int8>(e,0x48B)=int8(-1);
                        auto Small=[&](uint32 hi){ const uint32 m=next()%8; return m<3 ? uint8(2) : uint8(next()%hi); };
                        At<uint8>(e,0x13C)= next()%4==0 ? uint8(6) : Small(9); At<uint8>(e,0x484)=Small(5); At<uint8>(e,0x4A4)=Small(6);
                        const bool hasMat=next()%2!=0;
                        FillMat(mats[size_t(k)].data(),CVector{epx[k],epy[k],epz[k]},hasMat);
                        if(hasMat)At<void*>(e,0x14)=mats[size_t(k)].data();
                        else { At<CVector>(e,4)=CVector{epx[k],epy[k],epz[k]}; At<float>(e,0x10)=next()%5==0 ? 0.f : uni(-7.f,7.f); }
                    }
                    // ---------- listas de sectores ----------
                    auto Sx=[](float v){ return int(std::floor(v*0.02f+60.f)); };
                    if(!isList){
                        for(int k=0;k<NE;++k){
                            const int copies=1+int(next()%3);
                            for(int c=0;c<copies;++c){
                                if(isLod){
                                    const int lx=std::clamp(int(epx[k]*0.005f+15.f)+int(next()%3)-1,0,29), ly=std::clamp(int(epy[k]*0.005f+15.f)+int(next()%3)-1,0,29);
                                    reinterpret_cast<CPtrListSingleLink<CEntity*>*>(LODL)[ly*30+lx].AddItem(reinterpret_cast<CEntity*>(E(k)));
                                    continue;
                                }
                                const int x=Sx(epx[k])+int(next()%3)-1, y=Sx(epy[k])+int(next()%3)-1;
                                const int cx=std::clamp(x,0,119), cy=std::clamp(y,0,119);
                                switch(etype[k]){
                                case ENTITY_TYPE_BUILDING: CWorld::GetSector(cx,cy).Buildings.AddItem(reinterpret_cast<CBuilding*>(E(k))); break;
                                case ENTITY_TYPE_DUMMY:    CWorld::GetSector(cx,cy).Dummies.AddItem(reinterpret_cast<CDummy*>(E(k))); break;
                                case ENTITY_TYPE_VEHICLE:  CWorld::ms_aRepeatSectors[y&15][x&15].Vehicles.AddItem(reinterpret_cast<CVehicle*>(E(k))); break;
                                case ENTITY_TYPE_PED:      CWorld::ms_aRepeatSectors[y&15][x&15].Peds.AddItem(reinterpret_cast<CPed*>(E(k))); break;
                                default:                   CWorld::ms_aRepeatSectors[y&15][x&15].Objects.AddItem(reinterpret_cast<CObject*>(E(k))); break;
                                }
                            }
                        }
                    }
                    // lista suelta para las funciones de lista de sector (nodos reales del pool doble falso)
                    // sin destructor: el de la lista desengancharia nodos falsos despues de restaurar los pools reales
                    alignas(16) static std::array<uint8,16> flatBuf{}; flatBuf.fill(0);
                    auto& flatList=*reinterpret_cast<CPtrListDoubleLink<CPhysical*>*>(flatBuf.data());
                    if(isList){ const int n=int(next()%(NE+4)); for(int c=0;c<n;++c)flatList.AddItem(reinterpret_cast<CPhysical*>(E(int(next()%NE)))); }

                    // ---------- consulta ----------
                    for(auto& b:outBuf)b=0xCD;
                    int16 count= isList ? int16(next()%4) : int16(0x5A5A);
                    CEntity** const outPtr= next()%6==0 ? nullptr : reinterpret_cast<CEntity**>(outBuf.data());
                    static const int16 maxes[]{0,1,2,3,5,12,40,-1,100};
                    const int16 maxCount= next()%3==0 ? maxes[next()%std::size(maxes)] : int16(40);
                    CEntity* hit= isList ? reinterpret_cast<CEntity*>(next()%2 ? E(int(next()%NE)) : nullptr) : nullptr;
                    CEntity* nearestRet{}; float nearestDist= uni(0.f,200.f);
                    if(isList&&next()%3==0)nearestDist=1e9f;
                    const bool b2D=next()%2!=0;
                    bool fl[5]; for(auto& b:fl)b=next()%5!=0;
                    CVector pt{};
                    { const uint32 m=next()%4; if(m<3){ const int k=int(next()%NE); pt=CVector{epx[k]+uni(-30.f,30.f),epy[k]+uni(-30.f,30.f),epz[k]+uni(-30.f,30.f)}; }
                      else pt=CVector{uni(bx0,bx1),uni(by0,by1),uni(-20.f,120.f)}; }
                    auto RndRadius=[&]{ const uint32 m=next()%12; return m==0 ? 0.f : m==1 ? uni(0.f,3.f) : m==2 ? uni(150.f,700.f) : m==3 ? std::bit_cast<float>(0x7FC00000u) : uni(2.f,120.f); };
                    float radius=RndRadius();
                    if(isLod){ // los indices LOD deben quedar en 0..29: x-r >= -3190 y x+r < 3000
                        const float cx=(bx0+bx1)*0.5f, cy=(by0+by1)*0.5f; pt.x=cx+uni(-30.f,30.f); pt.y=cy+uni(-30.f,30.f);
                        radius=std::min(std::min(uni(10.f,350.f),pt.x+3190.f),std::min(2990.f-pt.x,std::min(pt.y+3190.f,2990.f-pt.y))); if(!(radius>=0.f))radius=0.f; }
                    auto DistSq=[&](int k,bool d2){ const float dx=pt.x-epx[k], dy=pt.y-epy[k], dz=pt.z-epz[k]; if(d2){ const float a=dx*dx; const float b=dy*dy; return a+b; }
                                                    const float a=dx*dx; const float b=dz*dz; const float c=a+b; const float d=dy*dy; return c+d; };
                    // empates: el radio se ajusta a la distancia de una entidad (con +-1 ulp)
                    if(!isLod&&(nmf.rfind("FindObjectsInRange",0)==0||nmf.rfind("FindObjectsOfType",0)==0)&&next()%4==0){
                        const int k=int(next()%NE); radius=std::sqrt(DistSq(k,b2D)); const uint32 j=next()%3; if(j==0)radius=std::nextafter(radius,1e9f); else if(j==1)radius=std::nextafter(radius,-1e9f); }
                    if(nmf.rfind("FindNearest",0)==0&&next()%3==0){
                        const int k=int(next()%NE); radius=std::sqrt(DistSq(k,b2D)); const uint32 j=next()%3; if(j==0)radius=std::nextafter(radius,1e9f); else if(j==1)radius=std::nextafter(radius,-1e9f); if(isList)nearestDist=radius; }
                    // identificador de modelo
                    uint32 modelId; { const uint32 m=next()%6; modelId= m<3 ? uint32(emodel[next()%NE]) : m==3 ? uint32(MODEL0+int(next()%NM)) : m==4 ? 0xFFFF8001u : uint32(next()%70000); }
                    if(nmf.rfind("FindNearest",0)==0){ const uint32 m=next()%8; if(m==0)modelId=0xFFFFFFFFu; else if(m==1)modelId=uint32(-int(next()%2000)-1); else if(m==2)modelId=0xFFFF8000u; }
                    // cubo
                    CVector ca{}, cb{};
                    { const float hx=next()%4==0 ? uni(0.f,5.f) : uni(5.f,260.f), hy=next()%4==0 ? uni(0.f,5.f) : uni(5.f,260.f), hz=next()%3==0 ? uni(0.f,5.f) : uni(5.f,150.f);
                      ca=CVector{pt.x-hx,pt.y-hy,pt.z-hz}; cb=CVector{pt.x+hx,pt.y+hy,pt.z+hz};
                      if(next()%12==0)std::swap(ca.x,cb.x);
                      if(next()%12==0)std::swap(ca.y,cb.y);
                      if(next()%12==0)ca.z=std::bit_cast<float>(0x7FC00000u);
                      if(next()%3==0){ // empate con el borde de una entidad (pos +- radio de su modelo, con +-1 ulp)
                          const int k=int(next()%NE); if(emodel[k]>=MODEL0&&emodel[k]<MODEL0+NM){ const float r=mrad[emodel[k]-MODEL0]; const uint32 ax=next()%3; const uint32 j=next()%3;
                              float* const lo=&ca.x+ax; float* const hi=&cb.x+ax; const float p=ax==0 ? epx[k] : ax==1 ? epy[k] : epz[k];
                              if(next()%2){ *lo=p+r; if(j==0)*lo=std::nextafter(*lo,1e9f); else if(j==1)*lo=std::nextafter(*lo,-1e9f); }
                              else { *hi=p-r; if(j==0)*hi=std::nextafter(*hi,1e9f); else if(j==1)*hi=std::nextafter(*hi,-1e9f); } } } }
                    // caja angulada
                    { std::memset(rotMat.data(),0,rotMat.size()); FillMat(rotMat.data(),CVector{0.f,0.f,0.f},next()%4!=0);
                      std::memset(boxBuf.data(),0,boxBuf.size());
                      At<CVector>(boxBuf.data(),0)=CVector{-uni(0.1f,40.f),-uni(0.1f,40.f),-uni(0.1f,40.f)}; At<CVector>(boxBuf.data(),0xC)=CVector{uni(0.1f,40.f),uni(0.1f,40.f),uni(0.1f,40.f)};
                      if(next()%6==0)std::swap(At<float>(boxBuf.data(),0),At<float>(boxBuf.data(),0xC)); }
                    float ax1=pt.x-uni(0.f,200.f), ay1=pt.y-uni(0.f,200.f), ax2=pt.x+uni(0.f,200.f), ay2=pt.y+uni(0.f,200.f);
                    if(next()%10==0)std::swap(ax1,ax2);
                    // modo de la lista de mision: vehiculos, peatones u objetos (las ocho combinaciones)
                    const bool mvv=next()%3==0, mpp=next()%3==0, moo=next()%3==0;

                    // ---------- ejecucion ----------
                    setState(candidate);
                    bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t target=fn.address;
                    using SecArgsRange = void(__cdecl*)(void*,const CVector*,float,bool,int16*,int16,CEntity**);
                    if(nmf=="FindObjectsInRange"||nmf=="FindObjectsKindaColliding")
                        reinterpret_cast<void(__cdecl*)(const CVector*,float,bool,int16*,int16,CEntity**,bool,bool,bool,bool,bool)>(target)(&pt,radius,b2D,&count,maxCount,outPtr,fl[0],fl[1],fl[2],fl[3],fl[4]);
                    else if(nmf=="FindObjectsOfTypeInRange")
                        reinterpret_cast<void(__cdecl*)(uint32,const CVector*,float,bool,int16*,int16,CEntity**,bool,bool,bool,bool,bool)>(target)(modelId,&pt,radius,b2D,&count,maxCount,outPtr,fl[0],fl[1],fl[2],fl[3],fl[4]);
                    else if(nmf=="FindLodOfTypeInRange")
                        reinterpret_cast<void(__cdecl*)(uint32,const CVector*,float,bool,int16*,int16,CEntity**)>(target)(modelId,&pt,radius,b2D,&count,maxCount,outPtr);
                    else if(nmf=="FindObjectsIntersectingCube")
                        reinterpret_cast<void(__cdecl*)(const CVector*,const CVector*,int16*,int16,CEntity**,bool,bool,bool,bool,bool)>(target)(&ca,&cb,&count,maxCount,outPtr,fl[0],fl[1],fl[2],fl[3],fl[4]);
                    else if(nmf=="FindObjectsIntersectingAngledCollisionBox")
                        reinterpret_cast<void(__cdecl*)(const void*,const void*,const CVector*,float,float,float,float,int16*,int16,CEntity**,bool,bool,bool,bool,bool)>(target)(boxBuf.data(),rotMat.data(),&pt,ax1,ay1,ax2,ay2,&count,maxCount,outPtr,fl[0],fl[1],fl[2],fl[3],fl[4]);
                    else if(nmf=="FindMissionEntitiesIntersectingCube")
                        reinterpret_cast<void(__cdecl*)(const CVector*,const CVector*,int16*,int16,CEntity**,bool,bool,bool)>(target)(&ca,&cb,&count,maxCount,outPtr,fl[0],fl[1],fl[2]);
                    else if(nmf=="FindNearestObjectOfType")
                        nearestRet=reinterpret_cast<CEntity*(__cdecl*)(int32,const CVector*,float,bool,bool,bool,bool,bool,bool)>(target)(int32(modelId),&pt,radius,b2D,fl[0],fl[1],fl[2],fl[3],fl[4]);
                    else if(nmf.rfind("FindObjectsInRangeSectorList",0)==0||nmf.rfind("FindObjectsKindaCollidingSectorList",0)==0)
                        reinterpret_cast<SecArgsRange>(target)(&flatList,&pt,radius,b2D,&count,maxCount,outPtr);
                    else if(nmf.rfind("FindObjectsOfTypeInRangeSectorList",0)==0)
                        reinterpret_cast<void(__cdecl*)(uint32,void*,const CVector*,float,bool,int16*,int16,CEntity**)>(target)(modelId,&flatList,&pt,radius,b2D,&count,maxCount,outPtr);
                    else if(nmf.rfind("FindObjectsIntersectingCubeSectorList",0)==0)
                        reinterpret_cast<void(__cdecl*)(void*,const CVector*,const CVector*,int16*,int16,CEntity**)>(target)(&flatList,&ca,&cb,&count,maxCount,outPtr);
                    else if(nmf.rfind("FindObjectsIntersectingAngledCollisionBoxSectorList",0)==0)
                        reinterpret_cast<void(__cdecl*)(void*,const void*,const void*,const CVector*,int16*,int16,CEntity**)>(target)(&flatList,boxBuf.data(),rotMat.data(),&pt,&count,maxCount,outPtr);
                    else if(nmf.rfind("FindMissionEntitiesIntersectingCubeSectorList",0)==0)
                        reinterpret_cast<void(__cdecl*)(void*,const CVector*,const CVector*,int16*,int16,CEntity**,bool,bool,bool)>(target)(&flatList,&ca,&cb,&count,maxCount,outPtr,mvv,mpp,moo);
                    else if(nmf.rfind("FindNearestObjectOfTypeSectorList",0)==0)
                        reinterpret_cast<void(__cdecl*)(int32,void*,const CVector*,float,bool,CEntity**,float*)>(target)(int32(modelId),&flatList,&pt,radius,b2D,&hit,&nearestDist);
                    setState(false);

                    json outs=json::array(); for(int k=0;k<std::clamp<int>(count,0,40);++k)outs.push_back(Name(reinterpret_cast<void**>(outBuf.data())[k]));
                    json r={{"count",int(count)},{"outs",outs},{"out_hash",Hash(outBuf.data(),outBuf.size())},{"nearest",Name(nearestRet)},{"hit",Name(hit)},{"dist",F(nearestDist)},
                            {"ents",Hash(entMem.data(),entMem.size())},{"scan",int(AtA<uint16>(SCAN))},
                            {"world",{Hash(reinterpret_cast<void*>(SECTORS),SECTORS_SIZE),Hash(reinterpret_cast<void*>(REPEAT),REPEAT_SIZE),Hash(reinterpret_cast<void*>(LODL),LOD_SIZE),
                                      Hash(dblStore.data(),dblStore.size()),Hash(dblStates.data(),dblStates.size()),Hash(dblHdr.data(),dblHdr.size()),
                                      Hash(sglStore.data(),sglStore.size()),Hash(sglStates.data(),sglStates.size()),Hash(sglHdr.data(),sglHdr.size())}},
                            {"switches",switches}};
                    RestoreAll();
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["outs"]},{"candidate",candidate["outs"]}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    const bool intactBefore=Restored();
    RestoreAll();
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    if(clearHook){ clearHook->State(clearEnabled); clearHook->LockState(clearLocked); }
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"globals_were_modified",!intactBefore},{"state_restored",Restored()}});
}
}
