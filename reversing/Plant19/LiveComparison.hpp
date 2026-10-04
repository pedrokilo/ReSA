#pragma once
// Banco dentro del motor: gestor de plantas (lista de 19; 19 funciones de CPlantMgr).
// Original (hooks del grupo desactivados) frente a candidato con fixtures deterministas sobre objetos falsos.
// Nunca se crean objetos ni texturas de RW ni se cargan ficheros: RW (atomics, frames, texturas), CTxdStore,
// CStreaming, LoadModels, CPlantSurfPropMgr, CGrassRenderer, CPlantLocTri::Add/Release, CPlantColEntEntry::
// AddEntry/ReleaseEntry, ProcObjectMan_c, SurfaceInfos_c, CEntity::GetColModel/TransformFromObjectSpace,
// CCollisionData::GetTrianglePoint, TransformPoints, CCollision::TestSphereBox, CCamera::IsSphereVisible,
// CWorld::ClearScanCodes, las luces de CTimeCycle, rand y los estados de render se simulan con trazas.
// Las entradas de la lista, los triángulos, las entidades, sus modelos de colisión y las listas de edificios
// de los sectores son falsos. Todo el estado global tocado (globales de CPlantMgr y sus tablas, sectores,
// punteros de model infos, viento, reloj, código de búsqueda, área, espejo, calidad de efectos, balance
// día/noche, globales de CGrassRenderer) se guarda y se restaura en cada ejecución y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include "PlantMgr.h"
#include "PlantLocTri.h"
#include "PlantColEntEntry.h"
#include "PlantSurfPropMgr.h"
#include "GrassRenderer.h"
#include "ProcObjectMan.h"
#include "SurfaceInfos_c.h"
#include "TxdStore.h"
#include "Streaming.h"
#include "Camera.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace Plant19Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

constexpr uintptr_t GLOBALS=0xC03980; constexpr size_t GLOBALS_SZ=0xC09178-0xC03980;
constexpr uintptr_t SECTORS=0xB7D0B8; constexpr size_t SECTORS_SZ=120*120*8;
constexpr int32 MI_BASE=19990;

// --- buferes falsos (se nombran por region en las trazas) ---
alignas(16) inline std::array<std::array<uint8,0x40>,16> ents{};     // entidades (CBuilding)
alignas(16) inline std::array<std::array<uint8,0x30>,16> cms{};      // CColModel
alignas(16) inline std::array<std::array<uint8,0x30>,16> cds{};      // CCollisionData
alignas(16) inline std::array<std::array<uint8,0x200>,16> triBuf{};  // [8 grupos][int32 n][32 triangulos]
alignas(16) inline std::array<std::array<uint8,0x48>,16> mats{};     // CMatrix
alignas(16) inline std::array<std::array<uint8,0x14>,12> cents{};    // CPlantColEntEntry
alignas(16) inline std::array<std::array<void*,32>,12> objArr{};     // m_Objects
alignas(16) inline std::array<std::array<uint8,0x54>,32> ltris{};    // CPlantLocTri
alignas(16) inline std::array<std::array<uint8,0x20>,8> minfo{};     // model infos
alignas(16) inline std::array<std::array<uint8,0x7C>,4> props{};     // CPlantSurfProp
alignas(16) inline std::array<std::array<uint8,8>,64> nodes{};       // nodos de listas de sectores
alignas(16) inline std::array<std::array<uint8,0x60>,10> texs{};     // RwTexture
alignas(16) inline std::array<std::array<uint8,0x10>,16> atoms{};    // RpAtomic
alignas(16) inline std::array<uint8,0x40> frm{};                     // direcciones de RwFrame
alignas(16) inline std::array<void*,16> texTab{};                    // tablas de texturas de grassTexturesPtr
alignas(16) inline std::array<void*,8> heads{};                      // cabezas de listas para Move*
alignas(16) inline std::array<CVector,64> vtx{};
inline constexpr size_t TRI_OFF=4+28*8;
inline std::vector<std::pair<const void*,size_t>> regions;
inline json Id(const void* p){ if(!p) return nullptr; const auto u=uintptr_t(p);
    for(size_t k=0;k<regions.size();++k){ const auto b=uintptr_t(regions[k].first); if(u>=b&&u<b+regions[k].second) return json::array({k,u-b}); } return json::array({"ext",u}); }
inline void Reg(const void* p,size_t n){ regions.emplace_back(p,n); }

// --- dependencias simuladas ---
inline uint32 maskA{}, callsA{}, maskB{}, callsB{};
inline bool BitA(){ return (maskA>>(callsA++%32))&1; }
inline bool BitB(){ return (maskB>>(callsB++%32))&1; }
inline float amb[3]{};
inline float __cdecl FakeAmbR(){ trace.push_back({"ambient",0}); return amb[0]; }
inline float __cdecl FakeAmbG(){ trace.push_back({"ambient",1}); return amb[1]; }
inline float __cdecl FakeAmbB(){ trace.push_back({"ambient",2}); return amb[2]; }
inline void __cdecl FakeSetScanCode(uint16 c){ trace.push_back({"grass_scan_code",c}); }
inline void __cdecl FakeSetCamPos(const CVector& p){ trace.push_back({"grass_cam_pos",F(p.x),F(p.y),F(p.z)}); }
inline void __cdecl FakeSetWind(float b){ trace.push_back({"grass_wind",F(b)}); }
inline void __cdecl FakeSetCloseFar(float a,float b){ trace.push_back({"grass_close_far",F(a),F(b)}); }
inline bool __cdecl FakeSetModelsTab(uint32 t,RpAtomic** a){ trace.push_back({"grass_models_tab",t,Id(a)}); return t<4; }
inline void __cdecl FakeAddTriPlant(PPTriPlant* p,uint32 set){ auto b=ReversingProbe::Bytes(p,0x5C); b[0x46]=b[0x47]=0; /* relleno sin inicializar en la pila */ trace.push_back({"add_tri_plant",set,b}); }
inline void __cdecl FakeFlush(){ trace.push_back({"flush_tri_plants"}); }
// listas de entradas (enlaces en +0xC / +0x10)
inline uint8*& UHead(){ return *reinterpret_cast<uint8**>(0xC03998); }
inline uint8*& CHead(){ return *reinterpret_cast<uint8**>(0xC0399C); }
inline void Unlink(uint8* e,uint8*& head){ uint8* prev=At<uint8*>(e,0x10); uint8* next=At<uint8*>(e,0xC);
    if(prev){ if(next){At<uint8*>(next,0x10)=prev;At<uint8*>(prev,0xC)=next;} else At<uint8*>(prev,0xC)=nullptr; } else { head=next; if(next)At<uint8*>(next,0x10)=nullptr; } }
inline void PushFront(uint8* e,uint8*& head){ At<uint8*>(e,0xC)=head; At<uint8*>(e,0x10)=nullptr; head=e; if(At<uint8*>(e,0xC))At<uint8*>(At<uint8*>(e,0xC),0x10)=e; }
inline CPlantColEntEntry* __fastcall FakeAddEntry(CPlantColEntEntry* self,void*,CEntity* e){ const bool ok=BitB(); trace.push_back({"add_entry",Id(self),Id(e),ok}); auto* s=reinterpret_cast<uint8*>(self);
    At<CEntity*>(s,0)=e; if(!ok)return nullptr; Unlink(s,UHead()); PushFront(s,CHead()); return self; }
inline void __fastcall FakeReleaseEntry(CPlantColEntEntry* self,void*){ trace.push_back({"release_entry",Id(self)}); auto* s=reinterpret_cast<uint8*>(self); At<void*>(s,0)=nullptr; Unlink(s,CHead()); PushFront(s,UHead()); }
inline CPlantLocTri* __fastcall FakeTriAdd(CPlantLocTri* self,void*,const CVector& p1,const CVector& p2,const CVector& p3,uint8 surface,uint8 light,bool cp,bool co){
    const bool ok=BitB(); trace.push_back({"tri_add",Id(self),F(p1.x),F(p1.y),F(p1.z),F(p2.x),F(p2.y),F(p2.z),F(p3.x),F(p3.y),F(p3.z),surface,light,cp,co,ok});
    if(!ok)return nullptr; auto* s=reinterpret_cast<uint8*>(self); At<uint8>(s,0x48)=uint8((cp?1:0)|(co?2:0)); auto& uh=*reinterpret_cast<uint8**>(0xC03984); if(uh==s)uh=At<uint8*>(s,0x4C); return self; }
inline void __fastcall FakeTriRelease(CPlantLocTri* self,void*){ trace.push_back({"tri_release",Id(self)}); }
inline uint8 __fastcall FakeTriAdded(void* self,void*,CPlantLocTri* t){ const bool r=BitA(); trace.push_back({"tri_added",intptr_t(self)-0xBB7CB0,Id(t),r}); return r; }
inline uint32 plantsBits{}, objectsBits{};
inline bool __fastcall FakeCreatesPlants(void* self,void*,uint32 id){ const bool r=(plantsBits>>(id%32))&1; trace.push_back({"creates_plants",intptr_t(self)-0xB79538,id,r}); return r; }
inline bool __fastcall FakeCreatesObjects(void* self,void*,uint32 id){ const bool r=(objectsBits>>(id%32))&1; trace.push_back({"creates_objects",intptr_t(self)-0xB79538,id,r}); return r; }
inline CPlantSurfProp* __cdecl FakeSurf(uint16 i){ trace.push_back({"surf_props",i}); return reinterpret_cast<CPlantSurfProp*>(props[i%4].data()); }
inline bool __cdecl FakeSurfInit(){ const bool r=BitA(); trace.push_back({"surf_props_init",r}); return r; }
inline void* __fastcall FakeGetColModel(void* self,void*){ trace.push_back({"get_col_model",Id(self)}); for(size_t k=0;k<ents.size();++k)if(self==ents[k].data())return cms[k].data(); return cms[15].data(); }
inline CVector* __fastcall FakeTFOS(void* self,void*,CVector* out,const CVector& off){ const auto p=At<CVector>(self,4);
    *out=CVector{p.x+off.x*0.75f,p.y+off.y*0.75f,p.z+off.z}; trace.push_back({"from_object_space",Id(self),F(off.x),F(off.y),F(off.z)}); return out; }
inline void __fastcall FakeTriPoint(void* self,void*,CVector& out,int32 id){ out=vtx[uint32(id)%64]; trace.push_back({"tri_point",Id(self),id}); }
inline void __cdecl FakeTransformPoints(RwV3d* out,int n,const CMatrix& m,RwV3d* in){ const auto* q=reinterpret_cast<const float*>(&m);
    for(int k=0;k<n;++k){ const float x=in[k].x,y=in[k].y,z=in[k].z; out[k]={q[0]*x+q[4]*y+q[8]*z+q[12],q[1]*x+q[5]*y+q[9]*z+q[13],q[2]*x+q[6]*y+q[10]*z+q[14]}; }
    trace.push_back({"transform_points",n,Id(&m),Id(out)==Id(in)}); }
inline bool __cdecl FakeSphereBox(const CSphere& s,const CBox& b){ const bool r=BitA();
    trace.push_back({"sphere_box",F(s.m_vecCenter.x),F(s.m_vecCenter.y),F(s.m_vecCenter.z),F(s.m_fRadius),F(b.m_vecMin.x),F(b.m_vecMin.y),F(b.m_vecMin.z),F(b.m_vecMax.x),F(b.m_vecMax.y),F(b.m_vecMax.z),r}); return r; }
inline void __cdecl FakeClearScanCodes(){ trace.push_back({"clear_scan_codes"}); }
inline uint8 __fastcall FakeMiType(void* self,void*){ return At<uint8>(self,0x1F); }
inline std::array<void*,8> miVt{};
inline bool __fastcall FakeSphereVisible(void* self,void*,const CVector& c,float r,RwMatrix* m){ const bool v=BitA(); trace.push_back({"sphere_visible",F(c.x),F(c.y),F(c.z),F(r),intptr_t(m)-intptr_t(self),v}); return v; }
inline RwBool __cdecl StateSet(RwRenderState st,void* v){ trace.push_back({"render_state_set",int(st),uintptr_t(v)}); return TRUE; }
// RW, diccionarios de texturas y streaming
inline RpAtomic* __cdecl FakeAtomicSetFrame(RpAtomic* a,RwFrame* f){ trace.push_back({"atomic_set_frame",Id(a),Id(f)}); return a; }
inline RwBool __cdecl FakeFrameDestroy(RwFrame* f){ trace.push_back({"frame_destroy",Id(f)}); return TRUE; }
inline RwBool __cdecl FakeAtomicDestroy(RpAtomic* a){ trace.push_back({"atomic_destroy",Id(a)}); return TRUE; }
inline RwBool __cdecl FakeTexDestroy(RwTexture* t){ trace.push_back({"texture_destroy",Id(t)}); return TRUE; }
inline uint32 texNext{};
inline RwTexture* __cdecl FakeTexRead(const char* n,const char* m){ trace.push_back({"texture_read",std::string(n),m==nullptr}); return reinterpret_cast<RwTexture*>(texs[texNext++%10].data()); }
inline int32 findSlotResult{};
inline int32 __cdecl FakeFindTxd(const char* n){ trace.push_back({"find_txd_slot",std::string(n),findSlotResult}); return findSlotResult; }
inline int32 __cdecl FakeAddTxd(const char* n){ trace.push_back({"add_txd_slot",std::string(n)}); return 77; }
inline bool __cdecl FakeLoadTxd(int32 s,const char* f){ trace.push_back({"load_txd",s,std::string(f)}); return true; }
inline void __cdecl FakeAddRef(int32 s){ trace.push_back({"txd_add_ref",s}); }
inline void __cdecl FakeSetTxd(int32 s){ trace.push_back({"set_current_txd",s}); }
inline void __cdecl FakePushTxd(){ trace.push_back({"push_current_txd"}); }
inline void __cdecl FakePopTxd(){ trace.push_back({"pop_current_txd"}); }
inline void __cdecl FakeRemoveTxd(int32 s){ trace.push_back({"remove_txd_slot",s}); }
inline void __cdecl FakeMakeSpace(size_t n){ trace.push_back({"make_space_for",n}); }
inline void __cdecl FakeGonnaUse(){ trace.push_back({"im_gonna_use_streaming_memory"}); }
inline void __cdecl FakeHaveUsed(){ trace.push_back({"i_have_used_streaming_memory"}); }
inline bool __cdecl LoadModelsImpl(const char* const* names,RpAtomic** atomics){ json n=json::array(); for(int k=0;k<4;++k)n.push_back(std::string(names[k])); const bool r=BitB();
    trace.push_back({"load_models",n,Id(atomics),r}); const int base=atomics==reinterpret_cast<RpAtomic**>(0xC039F0) ? 0 : 4; for(int k=0;k<4;++k)atomics[k]=reinterpret_cast<RpAtomic*>(atoms[base+k].data()); return r; }
inline bool __cdecl FakeLoadModelsDll(std::initializer_list<const char*> l,RpAtomic** a){ std::array<const char*,4> n{"","","",""}; size_t k=0; for(auto* s:l)if(k<4)n[k++]=s; return LoadModelsImpl(n.data(),a); }
// 0x5DD220: ecx = tabla de atomics, eax = array de 4 nombres
__declspec(naked) inline void FakeLoadModelsExe() {
    __asm {
        push ecx
        push eax
        call LoadModelsImpl
        add esp, 8
        ret
    }
}
// funciones del propio grupo simuladas dentro de una ejecucion (tras fijar el estado de los hooks)
inline bool __cdecl FakeColUpdate(const CVector& p,bool fast){ trace.push_back({"col_entity_cache_update",F(p.x),F(p.y),F(p.z),fast}); return true; }
inline bool __cdecl FakeProcEntry(const CPlantColEntEntry& e,const CVector& c,int32 mask){ trace.push_back({"process_entry",Id(&e),F(c.x),F(c.y),F(c.z),mask}); return true; }
inline void __cdecl FakeAddLoc(const CPlantColEntEntry& e,const CVector& c,int32 mask,int32 s,int32 t){ trace.push_back({"add_loc_tris",Id(&e),F(c.x),F(c.y),F(c.z),mask,s,t}); }
inline void __cdecl FakeRemoveLoc(const CPlantColEntEntry& e,const CVector& c,int32 mask,int32 s,int32 t){ trace.push_back({"remove_loc_tris",Id(&e),F(c.x),F(c.y),F(c.z),mask,s,t}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t Our(ReversibleHooks::ReversibleHook::Base* h){ return h ? uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress()) : 0; }

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,19> fns{{
    {"CPlantMgr","CalculateWindBending",0x5DB3D0,0,-1,1024},{"CPlantMgr","Initialise",0x5DD910,0,-1,128},{"CPlantMgr","MoveColEntToList",0x5DB5F0,0,-1,512},
    {"CPlantMgr","MoveLocTriToList",0x5DB590,0,-1,512},{"CPlantMgr","PreUpdateOnceForNewCameraPos",0x5DCF30,0,-1,256},{"CPlantMgr","ReloadConfig",0x5DD780,0,-1,64},
    {"CPlantMgr","Render",0x5DBAE0,0,-1,512},{"CPlantMgr","SetPlantFriendlyFlagInAtomicMI",0x5DB650,0,-1,512},{"CPlantMgr","Shutdown",0x5DB940,0,-1,256},
    {"CPlantMgr","Update",0x5DCFA0,0,-1,512},{"CPlantMgr","UpdateAmbientColor",0x5DB310,0,-1,1024},{"CPlantMgr","_ColEntityCache_Add",0x5DBEB0,0,-1,512},
    {"CPlantMgr","_ColEntityCache_FindInCache",0x5DB530,0,-1,256},{"CPlantMgr","_ColEntityCache_Remove",0x5DBEF0,0,-1,256},{"CPlantMgr","_ColEntityCache_Update",0x5DC510,0,-1,1024},
    {"CPlantMgr","_ProcessEntryCollisionDataSections",0x5DCD80,0,-1,1024},{"CPlantMgr","_ProcessEntryCollisionDataSections_AddLocTris",0x5DC8B0,0,-1,1024},
    {"CPlantMgr","_ProcessEntryCollisionDataSections_RemoveLocTris",0x5DBF20,0,-1,1024},{"CPlantMgr","_UpdateLocTris",0x5DCF00,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"plant19-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"plant19-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("plant19");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"plant19-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,19> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    using std::bit_cast;
    const uintptr_t dllColUpdate=Our(FindHook("CPlantMgr","_ColEntityCache_Update")), dllProcEntry=Our(FindHook("CPlantMgr","_ProcessEntryCollisionDataSections"));
    const uintptr_t dllAddLoc=Our(FindHook("CPlantMgr","_ProcessEntryCollisionDataSections_AddLocTris")), dllRemoveLoc=Our(FindHook("CPlantMgr","_ProcessEntryCollisionDataSections_RemoveLocTris"));
    if(!dllColUpdate||!dllProcEntry||!dllAddLoc||!dllRemoveLoc){Write({{"event","error"},{"reason","dll address not found"}});return;}
    std::array<bool,19> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(regions.empty()){ for(auto& b:ents)Reg(b.data(),b.size()); for(auto& b:cms)Reg(b.data(),b.size()); for(auto& b:cds)Reg(b.data(),b.size()); for(auto& b:triBuf)Reg(b.data(),b.size());
        for(auto& b:mats)Reg(b.data(),b.size()); for(auto& b:cents)Reg(b.data(),b.size()); for(auto& b:objArr)Reg(b.data(),sizeof(b)); for(auto& b:ltris)Reg(b.data(),b.size());
        for(auto& b:minfo)Reg(b.data(),b.size()); for(auto& b:props)Reg(b.data(),b.size()); for(auto& b:texs)Reg(b.data(),b.size()); for(auto& b:atoms)Reg(b.data(),b.size());
        Reg(frm.data(),frm.size()); Reg(texTab.data(),sizeof(texTab)); Reg(heads.data(),sizeof(heads)); Reg(reinterpret_cast<void*>(GLOBALS),GLOBALS_SZ); }
    for(auto& v:miVt)v=reinterpret_cast<void*>(&FakeMiType);

    // estado global guardado
    auto Snap=[](uintptr_t a,size_t n){ auto* p=reinterpret_cast<uint8*>(a); return std::vector<uint8>(p,p+n); };
    struct Saved { uintptr_t a; std::vector<uint8> v; };
    std::vector<Saved> saved;
    for(auto [a,n]:std::initializer_list<std::pair<uintptr_t,size_t>>{
            {GLOBALS,GLOBALS_SZ},{0x8D1330,2},{0xC02DB8,0x30},{0x8D132C,4},{0xC812F0,4},{0xB7CB84,4},{0x8D12C0,4},{0xA9AE54,4},{0xB6F998,1},
            {0xB7CD78,2},{0xB72914,4},{SECTORS,SECTORS_SZ},{0xA9B0C8+MI_BASE*4,8*4}})
        saved.push_back({a,Snap(a,n)});
    auto RestoreAll=[&]{ for(auto& s:saved)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
        EntryPatch ar(bit_cast<uintptr_t>(&CTimeCycle::GetAmbientRed),FakeAmbR), are(0x560330,FakeAmbR), ag(bit_cast<uintptr_t>(&CTimeCycle::GetAmbientGreen),FakeAmbG), age(0x560340,FakeAmbG);
        EntryPatch ab(bit_cast<uintptr_t>(&CTimeCycle::GetAmbientBlue),FakeAmbB), abe(0x560350,FakeAmbB);
        EntryPatch g1(bit_cast<uintptr_t>(&CGrassRenderer::SetCurrentScanCode),FakeSetScanCode), g1e(0x5DABB0,FakeSetScanCode);
        EntryPatch g2(bit_cast<uintptr_t>(&CGrassRenderer::SetGlobalCameraPos),FakeSetCamPos), g2e(0x5DABC0,FakeSetCamPos);
        EntryPatch g3(bit_cast<uintptr_t>(&CGrassRenderer::SetGlobalWindBending),FakeSetWind), g3e(0x5DAC00,FakeSetWind);
        EntryPatch g4(bit_cast<uintptr_t>(&CGrassRenderer::SetCloseFarAlphaDist),FakeSetCloseFar), g4e(0x5DABE0,FakeSetCloseFar);
        EntryPatch g5(bit_cast<uintptr_t>(&CGrassRenderer::SetPlantModelsTab),FakeSetModelsTab), g5e(0x5DACC0,FakeSetModelsTab);
        EntryPatch g6(bit_cast<uintptr_t>(&CGrassRenderer::AddTriPlant),FakeAddTriPlant), g6e(0x5DB1D0,FakeAddTriPlant);
        EntryPatch g7(bit_cast<uintptr_t>(&CGrassRenderer::FlushTriPlantBuffer),FakeFlush), g7e(0x5DB250,FakeFlush);
        EntryPatch e1(bit_cast<uintptr_t>(&CPlantColEntEntry::AddEntry),FakeAddEntry), e1e(0x5DB7D0,FakeAddEntry);
        EntryPatch e2(bit_cast<uintptr_t>(&CPlantColEntEntry::ReleaseEntry),FakeReleaseEntry), e2e(0x5DB8A0,FakeReleaseEntry);
        EntryPatch t1(bit_cast<uintptr_t>(&CPlantLocTri::Add),FakeTriAdd), t1e(0x5DC290,FakeTriAdd);
        EntryPatch t2(bit_cast<uintptr_t>(&CPlantLocTri::Release),FakeTriRelease), t2e(0x5DB6D0,FakeTriRelease);
        EntryPatch pa(bit_cast<uintptr_t>(&ProcObjectMan_c::ProcessTriangleAdded),FakeTriAdded), pae(0x5A3F20,FakeTriAdded);
        EntryPatch s1(bit_cast<uintptr_t>(&SurfaceInfos_c::CreatesPlants),FakeCreatesPlants), s1e(0x55E9D0,FakeCreatesPlants);
        EntryPatch s2(bit_cast<uintptr_t>(&SurfaceInfos_c::CreatesObjects),FakeCreatesObjects), s2e(0x55E9F0,FakeCreatesObjects);
        EntryPatch sp(bit_cast<uintptr_t>(&CPlantSurfPropMgr::GetSurfProperties),FakeSurf), spe(0x6F9DE0,FakeSurf);
        EntryPatch si(bit_cast<uintptr_t>(&CPlantSurfPropMgr::Initialise),FakeSurfInit), sie(0x5DD6C0,FakeSurfInit);
        EntryPatch cm(bit_cast<uintptr_t>(&CEntity::GetColModel),FakeGetColModel), cme(0x535300,FakeGetColModel);
        EntryPatch tf(bit_cast<uintptr_t>(static_cast<CVector*(CEntity::*)(CVector&,const CVector&) const>(&CEntity::TransformFromObjectSpace)),FakeTFOS), tfe(0x5334F0,FakeTFOS);
        EntryPatch tp(bit_cast<uintptr_t>(&CCollisionData::GetTrianglePoint),FakeTriPoint), tpe(0x40F5E0,FakeTriPoint);
        EntryPatch tr(bit_cast<uintptr_t>(&TransformPoints),FakeTransformPoints), tre(0x54EEF0,FakeTransformPoints);
        EntryPatch sb(bit_cast<uintptr_t>(&CCollision::TestSphereBox),FakeSphereBox), sbe(0x4120C0,FakeSphereBox);
        EntryPatch cs(bit_cast<uintptr_t>(&CWorld::ClearScanCodes),FakeClearScanCodes), cse(0x563470,FakeClearScanCodes);
        EntryPatch sv(0x420C40,FakeSphereVisible);
        EntryPatch w1(0x74BF20,FakeAtomicSetFrame), w2(0x7F05A0,FakeFrameDestroy), w3(0x749DC0,FakeAtomicDestroy), w4(0x7F3820,FakeTexDestroy), w5(0x7F3AC0,FakeTexRead);
        EntryPatch x1(bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot)),FakeFindTxd), x1e(0x731850,FakeFindTxd);
        EntryPatch x2(bit_cast<uintptr_t>(&CTxdStore::AddTxdSlot),FakeAddTxd), x2e(0x731C80,FakeAddTxd);
        EntryPatch x3(bit_cast<uintptr_t>(static_cast<bool(*)(int32,const char*)>(&CTxdStore::LoadTxd)),FakeLoadTxd), x3e(0x7320B0,FakeLoadTxd);
        EntryPatch x4(bit_cast<uintptr_t>(&CTxdStore::AddRef),FakeAddRef), x4e(0x731A00,FakeAddRef);
        EntryPatch x5(bit_cast<uintptr_t>(&CTxdStore::SetCurrentTxd),FakeSetTxd), x5e(0x7319C0,FakeSetTxd);
        EntryPatch x6(bit_cast<uintptr_t>(&CTxdStore::PushCurrentTxd),FakePushTxd), x6e(0x7316A0,FakePushTxd);
        EntryPatch x7(bit_cast<uintptr_t>(&CTxdStore::PopCurrentTxd),FakePopTxd), x7e(0x7316B0,FakePopTxd);
        EntryPatch x8(bit_cast<uintptr_t>(&CTxdStore::RemoveTxdSlot),FakeRemoveTxd), x8e(0x731CD0,FakeRemoveTxd);
        EntryPatch m1(bit_cast<uintptr_t>(&CStreaming::MakeSpaceFor),FakeMakeSpace), m1e(0x40E120,FakeMakeSpace);
        EntryPatch m2(bit_cast<uintptr_t>(&CStreaming::ImGonnaUseStreamingMemory),FakeGonnaUse), m2e(0x407BE0,FakeGonnaUse);
        EntryPatch m3(bit_cast<uintptr_t>(&CStreaming::IHaveUsedStreamingMemory),FakeHaveUsed), m3e(0x407BF0,FakeHaveUsed);
        EntryPatch lm(bit_cast<uintptr_t>(&CPlantMgr::LoadModels),FakeLoadModelsDll), lme(0x5DD220,FakeLoadModelsExe);
        auto& fpSet=RWSRCGLOBAL(dOpenDevice).fpRenderStateSet; const auto savedSet=fpSet; fpSet=StateSet;

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto fill=[&](void* p,size_t n){ auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;++k)b[k]=uint8(next()); };
                // CMatrix falsa con giro en Z y posicion dada
                auto mkMat=[&](int k,const CVector& pos){ auto* q=reinterpret_cast<float*>(mats[k].data()); const float h=uni(-3.2f,3.2f), s=std::sin(h), c=std::cos(h);
                    std::fill(q,q+18,0.f); q[0]=c;q[1]=s;q[2]=uni(-0.05f,0.05f); q[4]=-s;q[5]=c;q[6]=uni(-0.05f,0.05f); q[8]=uni(-0.05f,0.05f);q[9]=uni(-0.05f,0.05f);q[10]=1.f;
                    q[12]=pos.x;q[13]=pos.y;q[14]=pos.z; At<void*>(mats[k].data(),0x40)=nullptr; At<uint8>(mats[k].data(),0x44)=0; };
                // entidad falsa k: posicion, matriz, modelo de colision y datos de colision con hasta 32 triangulos y 8 grupos
                auto mkEnt=[&](int k,const CVector& pos){ uint8* e=ents[k].data(); fill(e,0x40); At<CVector>(e,4)=pos; At<float>(e,0x10)=uni(-3.f,3.f); mkMat(k,pos); At<void*>(e,0x14)=mats[k].data();
                    uint8* c=cms[k].data(); fill(c,0x30); At<CVector>(c,0x18)=CVector{uni(-10.f,10.f),uni(-10.f,10.f),uni(-5.f,5.f)}; At<float>(c,0x24)=uni(0.f,60.f); At<void*>(c,0x2C)=cds[k].data();
                    uint8* d=cds[k].data(); fill(d,0x30); uint8* t=triBuf[k].data(); fill(t,triBuf[k].size()); At<void*>(d,0x18)=t+TRI_OFF;
                    for(int n=0;n<32;++n){ uint8* tt=t+TRI_OFF+n*8; At<uint16>(tt,0)=uint16(next()%64); At<uint16>(tt,2)=uint16(next()%64); At<uint16>(tt,4)=uint16(next()%64); tt[6]=uint8(next()%48); } };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1919u; if(!stt)stt=1; trace=json::array(); seed=uint32(i)^uint32(f<<20); rngCalls=0;
                    maskA=next(); callsA=0; maskB=next(); callsB=0; if(next()%5==0)maskB=~0u; if(next()%7==0)maskA=0;
                    plantsBits=next(); objectsBits=next(); if(next()%4==0)plantsBits=0; if(next()%4==0)objectsBits=0; texNext=0;
                    for(auto& b:frm)b=uint8(next()); for(auto& v:vtx)v=CVector{uni(-15.f,15.f),uni(-15.f,15.f),uni(-4.f,4.f)};
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto& closeTri=*reinterpret_cast<std::array<uint8*,4>*>(0xC03988); auto& unusedTri=*reinterpret_cast<uint8**>(0xC03984);
                    // listas de entradas falsas: nc en la lista cercana y nu en la libre
                    auto mkEntryLists=[&](int nc,int nu,int nEnt){ for(auto& c:cents){ fill(c.data(),0x14); At<void*>(c.data(),0xC)=nullptr; At<void*>(c.data(),0x10)=nullptr; }
                        CHead()=nullptr; UHead()=nullptr;
                        for(int k=nc-1;k>=0;--k){ uint8* c=cents[k].data(); At<void*>(c,0)=next()%6 ? static_cast<void*>(ents[next()%nEnt].data()) : nullptr; PushFront(c,CHead()); }
                        for(int k=nu-1;k>=0;--k){ uint8* c=cents[nc+k].data(); At<void*>(c,0)=nullptr; PushFront(c,UHead()); } };
                    auto entsOut=[&]{ return ReversingProbe::Bytes(cents.data(),sizeof(cents)); };

                    if(nm=="CalculateWindBending") {
                        At<uint32>(reinterpret_cast<void*>(0xC0916C),0)=next()%3 ? next()&~1u : next()|1u; At<uint32>(reinterpret_cast<void*>(0xC09168),0)=next();
                        float& wind=*reinterpret_cast<float*>(0xC812F0); const uint32 r=next()%12;
                        wind=r<4 ? uni(0.f,0.5f) : r<7 ? uni(0.5f,1.5f) : r==7 ? 0.5f : r==8 ? 0.2f : r==9 ? std::bit_cast<float>(0x7FC00000u) : r==10 ? uni(-1.f,0.f) : std::nextafter(0.5f,0.f);
                        *reinterpret_cast<uint32*>(0xB7CB84)=next()%4 ? next() : next()%100000;
                        ret=F(reinterpret_cast<float(__cdecl*)()>(a)());
                        out["state"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC09168),8);
                    }
                    else if(nm=="UpdateAmbientColor") {
                        for(auto& c:amb){ const uint32 r=next()%10; c=r<5 ? uni(-0.1f,0.6f) : r==5 ? 0.3f : r==6 ? uni(-1.f,-0.1f) : r==7 ? uni(0.6f,3.f) : r==8 ? uni(102.f,104.f) : uni(0.29f,0.31f); }
                        fill(reinterpret_cast<void*>(0xC03A44),4);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["color"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC03A44),4);
                    }
                    else if(nm=="MoveLocTriToList"||nm=="MoveColEntToList") {
                        const bool tri=nm=="MoveLocTriToList"; const size_t nx=tri ? 0x4C : 0xC, pv=tri ? 0x50 : 0x10, sz=tri ? 0x54 : 0x14;
                        auto obj=[&](int k)->uint8*{ return tri ? ltris[k].data() : cents[k].data(); };
                        for(auto& h:heads)h=nullptr; std::array<int,8> lst{}; std::array<uint8*,4> tails{};
                        for(int k=0;k<8;++k){ uint8* o=obj(k); fill(o,sz); lst[k]=int(next()%5); At<uint8*>(o,nx)=nullptr;
                            if(lst[k]<4){ At<uint8*>(o,pv)=tails[lst[k]]; if(tails[lst[k]])At<uint8*>(tails[lst[k]],nx)=o; else heads[lst[k]]=o; tails[lst[k]]=o; } }
                        int target=int(next()%8); for(int k=0;k<8&&lst[target]==4;++k)target=(target+1)%8;
                        const int from=lst[target]<4&&next()%6 ? lst[target] : int(next()%4), to=int(next()%4);
                        reinterpret_cast<void(__cdecl*)(void**,void**,void*)>(a)(&heads[from],&heads[to],obj(target));
                        json o=json::array(); for(int k=0;k<8;++k)o.push_back(ReversingProbe::Bytes(obj(k),sz)); out["objs"]=o; json h=json::array(); for(auto* p:heads)h.push_back(Id(p)); out["heads"]=h;
                    }
                    else if(nm=="SetPlantFriendlyFlagInAtomicMI") {
                        mkEnt(0,CVector{0.f,0.f,0.f}); uint8* mi=minfo[0].data(); fill(mi,0x20); At<void*>(mi,0x14)=cms[0].data();
                        if(next()%6==0)At<void*>(cms[0].data(),0x2C)=nullptr;
                        int16 nt; switch(next()%8){case 0:nt=0;break;case 1:nt=int16(-int(next()%3000)-1);break;default:nt=int16(1+next()%32);}
                        At<int16>(cds[0].data(),4)=nt;
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(mi);
                        out["mi"]=ReversingProbe::Bytes(mi,0x20);
                    }
                    else if(nm=="Shutdown") {
                        for(int k=0;k<12;++k)mkEnt(k,CVector{0.f,0.f,0.f}); mkEntryLists(int(next()%5),int(next()%4),12);
                        auto* models=reinterpret_cast<void**>(0xC039F0); auto* textures=reinterpret_cast<void**>(0xC039A0);
                        for(int k=0;k<16;++k){ uint8* at=atoms[k].data(); fill(at,0x10); At<void*>(at,4)=next()%3 ? static_cast<void*>(frm.data()+k*4) : nullptr; models[k]=next()%4 ? static_cast<void*>(at) : nullptr;
                            textures[k]=next()%4 ? static_cast<void*>(texs[k%10].data()) : nullptr; }
                        findSlotResult=next()%3==0 ? -1 : int32(next()%60);
                        reinterpret_cast<void(__cdecl*)()>(a)();
                        out["tabs"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC039A0),0x90); out["entries"]=entsOut(); out["heads"]=json::array({Id(UHead()),Id(CHead())});
                    }
                    else if(nm=="ReloadConfig"||nm=="Initialise") {
                        fill(reinterpret_cast<void*>(0xC03984),0xC09168-0xC03984);
                        for(auto& t:texs)fill(t.data(),t.size()); findSlotResult=next()%3==0 ? -1 : int32(next()%60);
                        if(next()%6)maskA|=1u; // la carga de plantas.dat suele funcionar
                        ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                        out["globals"]=Fnv(reinterpret_cast<void*>(GLOBALS),GLOBALS_SZ);
                        out["heads"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC03984),0x1C); out["tabs"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC039A0),0xA8);
                        out["first_tri"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC03A48),0x54); out["last_tri"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC03A48+255*0x54),0x54);
                        out["first_entry"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC08E48),0x14); out["last_entry"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xC08E48+39*0x14),0x14);
                        out["tex"]=At<uint32>(reinterpret_cast<void*>(0xC09174),0)!=0 ? json(Id(*reinterpret_cast<void**>(0xC09174))) : json(nullptr);
                        json tx=json::array(); for(auto& t:texs)tx.push_back(At<uint32>(t.data(),0x50)); out["tex_flags"]=tx;
                    }
                    else if(nm=="Render") {
                        At<int32>(reinterpret_cast<void*>(0xA9AE54),0)=next()%6==0 ? 0 : int32(1+next()%3); At<uint8>(reinterpret_cast<void*>(0xB6F998),0)=uint8(next()%2);
                        At<float>(reinterpret_cast<void*>(0x8D12C0),0)=next()%5==0 ? float(next()%2) : uni(0.f,1.f);
                        for(int k=0;k<16;++k)texTab[k]=reinterpret_cast<void*>(uintptr_t(0x1000+k*0x10));
                        auto* gtp=reinterpret_cast<void**>(0xC039E0); for(int k=0;k<4;++k)gtp[k]=&texTab[(k*4+next()%2*4)%16];
                        for(auto& p:props){ fill(p.data(),p.size()); for(int k=0;k<3;++k){ auto* d=p.data()+4+k*0x28; At<uint16>(d,0)=next()%4==0 ? uint16(0xFFFF) : uint16(next()%40); At<uint16>(d,2)=uint16(next()%4);
                            for(size_t o=0xC;o<0x28;o+=4)At<float>(d,o)=uni(0.f,2.f); } }
                        for(auto& h:closeTri)h=nullptr; std::array<uint8*,4> tails{};
                        for(int k=0;k<12;++k){ uint8* t=ltris[k].data(); fill(t,0x54); for(size_t o=0;o<0x34;o+=4)At<float>(t,o)=uni(-500.f,500.f); for(size_t o=0x34;o<0x40;o+=4)At<float>(t,o)=uni(0.f,1.f);
                            At<uint8>(t,0x46)=uint8(next()%8); At<uint8>(t,0x48)=uint8((next()&~1u)|(next()%10<7 ? 1u : 0u)); At<uint8*>(t,0x4C)=nullptr;
                            const uint32 l=next()%5; if(l<4){ At<uint8*>(t,0x50)=tails[l]; if(tails[l])At<uint8*>(tails[l],0x4C)=t; else closeTri[l]=t; tails[l]=t; } }
                        reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    else if(nm=="Update"||nm=="PreUpdateOnceForNewCameraPos"||nm=="_UpdateLocTris") {
                        for(int k=0;k<6;++k)mkEnt(k,CVector{0.f,0.f,0.f}); mkEntryLists(int(next()%5),int(next()%3),6);
                        At<int16>(reinterpret_cast<void*>(0xC03980),0)=int16(next()); At<uint32>(reinterpret_cast<void*>(0xC0916C),0)=next(); At<uint32>(reinterpret_cast<void*>(0xC09168),0)=next();
                        At<uint8>(reinterpret_cast<void*>(0xC09170),0)=uint8(next()); At<uint8>(reinterpret_cast<void*>(0xC09171),0)=next()%4==0 ? uint8(0x1F+0x20*(next()%8)) : uint8(next());
                        *reinterpret_cast<float*>(0xC812F0)=uni(0.f,1.2f); *reinterpret_cast<uint32*>(0xB7CB84)=next(); for(auto& c:amb)c=uni(-0.1f,0.6f);
                        const CVector pos{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,200.f)};
                        EntryPatch pe(0x5DCD80,FakeProcEntry), pd(dllProcEntry,FakeProcEntry);
                        std::unique_ptr<EntryPatch> ce, cd2; if(nm!="_UpdateLocTris"){ ce=std::make_unique<EntryPatch>(0x5DC510,FakeColUpdate); cd2=std::make_unique<EntryPatch>(dllColUpdate,FakeColUpdate); }
                        if(nm=="_UpdateLocTris")ret=reinterpret_cast<bool(__cdecl*)(const CVector*,int32)>(a)(&pos,next()%3==0 ? int32(0xFAFAFAFA) : int32(next()%10));
                        else ret=reinterpret_cast<bool(__cdecl*)(const CVector*)>(a)(&pos);
                        out["globals"]=json::array({ReversingProbe::Bytes(reinterpret_cast<void*>(0xC03980),2),ReversingProbe::Bytes(reinterpret_cast<void*>(0xC03A44),4),ReversingProbe::Bytes(reinterpret_cast<void*>(0xC09168),12)});
                    }
                    else if(nm=="_ColEntityCache_Add"||nm=="_ColEntityCache_FindInCache"||nm=="_ColEntityCache_Remove") {
                        for(int k=0;k<8;++k)mkEnt(k,CVector{0.f,0.f,0.f}); mkEntryLists(int(next()%7),int(next()%5),6);
                        void* target=ents[next()%8].data(); if(next()%8==0)target=nullptr;
                        if(nm=="_ColEntityCache_Add")ret=Id(reinterpret_cast<void*(__cdecl*)(void*,bool)>(a)(target,next()%3!=0));
                        else if(nm=="_ColEntityCache_FindInCache")ret=Id(reinterpret_cast<void*(__cdecl*)(void*)>(a)(target));
                        else reinterpret_cast<void(__cdecl*)(void*)>(a)(target);
                        out["entries"]=entsOut(); out["heads"]=json::array({Id(UHead()),Id(CHead())});
                    }
                    else if(nm=="_ColEntityCache_Update") {
                        const CVector cam{next()%10==0 ? uni(-3100.f,3100.f) : uni(-2900.f,2900.f),next()%10==0 ? uni(-3100.f,3100.f) : uni(-2900.f,2900.f),uni(0.f,100.f)};
                        auto& area=*reinterpret_cast<int32*>(0xB72914); area=int32(next()%3); auto& wsc=*reinterpret_cast<uint16*>(0xB7CD78); wsc=next()%8==0 ? uint16(0xFFFF) : uint16(1+next()%0xFFFD);
                        const uint16 cur=wsc==0xFFFF ? 1 : uint16(wsc+1);
                        for(int m=0;m<8;++m){ uint8* mi=minfo[m].data(); fill(mi,0x20); At<void*>(mi,0)=miVt.data(); At<uint8>(mi,0x1F)=next()%5 ? 1 : uint8(next()%4); reinterpret_cast<CBaseModelInfo**>(0xA9B0C8)[MI_BASE+m]=reinterpret_cast<CBaseModelInfo*>(mi); }
                        for(int k=0;k<12;++k){ mkEnt(k,CVector{cam.x+uni(-450.f,450.f),cam.y+uni(-450.f,450.f),uni(-20.f,60.f)}); uint8* e=ents[k].data();
                            if(next()%6==0)At<uint32>(e,0x1C)|=0x4000000u; else At<uint32>(e,0x1C)&=~0x4000000u;
                            At<int16>(e,0x22)=int16(MI_BASE+next()%8); At<uint16>(e,0x2C)=next()%6==0 ? cur : next()%6==0 ? wsc : uint16(next());
                            At<uint8>(e,0x2F)=next()%5==0 ? uint8(13) : next()%4==0 ? uint8(next()%5) : uint8(area); }
                        mkEntryLists(int(next()%6),next()%8==0 ? 0 : int(1+next()%5),12);
                        for(size_t s=0;s<120*120;++s)*reinterpret_cast<void**>(SECTORS+s*8)=nullptr;
                        const int cx=int((cam.x-340.f)/50.f+60.f), cy=int((cam.y-340.f)/50.f+60.f);
                        const int nNodes=int(next()%48);
                        for(int n=0;n<nNodes;++n){ const int x=std::clamp(cx-1+int(next()%16),0,119), y=std::clamp(cy-1+int(next()%16),0,119); auto& head=*reinterpret_cast<uint8**>(SECTORS+(y*120+x)*8);
                            uint8* nd=nodes[n].data(); At<void*>(nd,0)=ents[next()%12].data(); At<uint8*>(nd,4)=head; head=nd; }
                        ret=reinterpret_cast<bool(__cdecl*)(const CVector*,bool)>(a)(&cam,next()%4==0);
                        out["entries"]=entsOut(); out["heads"]=json::array({Id(UHead()),Id(CHead())}); out["scan"]=wsc;
                        json sc=json::array(); for(auto& e:ents)sc.push_back(At<uint16>(e.data(),0x2C)); out["ent_scan"]=sc;
                    }
                    else if(nm=="_ProcessEntryCollisionDataSections") {
                        mkEnt(0,CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)}); mkEntryLists(1,0,1); uint8* en=cents[0].data(); At<void*>(en,0)=ents[0].data();
                        At<void*>(en,4)=objArr[0].data(); uint16 n=uint16(1+next()%32); if(next()%10==0)n=uint16(0x8000|next()); At<uint16>(en,8)=n;
                        uint8* d=cds[0].data(); At<int16>(d,4)=next()%5 ? int16(n) : int16(next()); if(next()%10==0)At<void*>(cms[0].data(),0x2C)=nullptr;
                        At<uint8>(d,7)=uint8(next()%2 ? At<uint8>(d,7)|2u : At<uint8>(d,7)&~2u);
                        uint8* tris=triBuf[0].data()+TRI_OFF; *reinterpret_cast<int32*>(tris-4)=next()%10==0 ? -int32(next()%3) : int32(next()%9);
                        for(int g=0;g<8;++g){ uint8* fg=tris-4-28*(g+1); for(size_t o=0;o<0x18;o+=4)At<float>(fg,o)=uni(-80.f,80.f); At<int16>(fg,0x18)=int16(next()%8==0 ? -int(next()%100) : next()%40); At<int16>(fg,0x1A)=int16(next()%12==0 ? 0x8000|next() : next()%40); }
                        const CVector c{At<CVector>(ents[0].data(),4).x+uni(-150.f,150.f),At<CVector>(ents[0].data(),4).y+uni(-150.f,150.f),uni(0.f,100.f)};
                        EntryPatch ad(dllAddLoc,FakeAddLoc), ae(0x5DC8B0,FakeAddLoc), rd(dllRemoveLoc,FakeRemoveLoc), re(0x5DBF20,FakeRemoveLoc);
                        ret=reinterpret_cast<bool(__cdecl*)(void*,const CVector*,int32)>(a)(en,&c,next()%3==0 ? int32(0xFAFAFAFA) : int32(next()%10));
                    }
                    else if(nm=="_ProcessEntryCollisionDataSections_AddLocTris"||nm=="_ProcessEntryCollisionDataSections_RemoveLocTris") {
                        const bool add=nm=="_ProcessEntryCollisionDataSections_AddLocTris";
                        const CVector c{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                        mkEnt(0,CVector{c.x+uni(-110.f,110.f),c.y+uni(-110.f,110.f),c.z+uni(-20.f,20.f)}); mkEntryLists(1,0,1); uint8* en=cents[0].data(); At<void*>(en,0)=ents[0].data();
                        At<void*>(en,4)=objArr[0].data(); At<uint16>(en,8)=32; if(add&&next()%10==0)At<void*>(cms[0].data(),0x2C)=nullptr;
                        unusedTri=nullptr;
                        for(int k=0;k<32;++k){ uint8* t=ltris[k].data(); fill(t,0x54); const CVector b{c.x+uni(-130.f,130.f),c.y+uni(-130.f,130.f),c.z+uni(-30.f,30.f)};
                            for(int v=0;v<4;++v)At<CVector>(t,v*12)=CVector{b.x+uni(-12.f,12.f),b.y+uni(-12.f,12.f),b.z+uni(-3.f,3.f)}; At<uint8*>(t,0x4C)=nullptr; At<uint8*>(t,0x50)=nullptr; }
                        for(int k=0;k<32;++k)objArr[0][k]=next()%(add ? 3 : 4)==0 ? nullptr : static_cast<void*>(ltris[next()%16].data());
                        if(add){ const int u=int(next()%7); for(int k=u-1;k>=0;--k){ uint8* t=ltris[16+k].data(); At<uint8*>(t,0x4C)=unusedTri; unusedTri=t; } }
                        int32 s=int32(next()%32), e=int32(next()%32); if(next()%6&&s>e)std::swap(s,e);
                        const uint32 mr=next()%4; const int32 mask=mr==0 ? int32(0xFAFAFAFA) : mr==1 ? int32(8+next()%8) : int32(next()%8);
                        reinterpret_cast<void(__cdecl*)(void*,const CVector*,int32,int32,int32)>(a)(en,&c,mask,s,e);
                        json o=json::array(); for(auto* p:objArr[0])o.push_back(Id(p)); out["objects"]=o; json fl=json::array(); for(auto& t:ltris)fl.push_back(t[0x48]); out["flags"]=fl; out["unused"]=Id(unusedTri);
                    }
                    out["ret"]=ret; out["trace"]=trace;
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
        fpSet=savedSet;
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    bool restored=true; for(auto& s:saved)restored&=std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.v.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
