#pragma once
// Banco dentro del motor: audio, balas, modelos, garajes y datos (32 funciones): CAEPedWeaponAudioEntity,
// CBulletInfo, CDamageAtomicModelInfo, CGarage, CPedStats, CPlantSurfPropMgr, CScriptResourceManager y
// CSpecialPlateHandler. Original (hooks del grupo desactivados) frente a candidato con fixtures deterministas.
// Se simulan y trazan: las funciones de CAEWeaponAudioEntity llamadas por la entidad del peatón, GetTaskSwim,
// ProcessLineOfSight y todas las llamadas de daño, efectos, cámara, animación, audio y cristales de
// CBulletInfo::Update, las primitivas RW y renderers de CDamageAtomicModelInfo, las llamadas de mundo, pool de
// vehículos, coches guardados y destrucción de CGarage, la lectura de ficheros (OpenFile/LoadLine/CloseFile/
// SetDir), GetSurfaceIdFromName, LoadDefaultDecisionMaker y new[]/delete[] (solo durante la llamada y sin
// reservar memoria). Todas las entidades son falsas; el estado global tocado (balas, banderas de CWorld,
// reloj, pool de vehículos, tabla de pedstats, tablas de plantas) se guarda y se restaura en cada ejecución.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "AEPedWeaponAudioEntity.h"
#include "BulletInfo.h"
#include "DamageAtomicModelInfo.h"
#include "Garage.h"
#include "PedStats.h"
#include "PlantSurfPropMgr.h"
#include "ScriptResourceManager.h"
#include "SpecialPlateHandler.h"
#include "CustomBuildingRenderer.h"
#include "CarFXRenderer.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "DecisionMakers/DecisionMakerTypesFileLoader.h"
#include "SurfaceInfos_c.h"
#include "PlayerInfo.h"
#include "Glass.h"
#include "AudioEngine.h"
#include "FireManager.h"
#include "Localisation.h"
#include "../ProbeRuntime.hpp"

namespace Data23Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline void T(json j) { trace.push_back(std::move(j)); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline intptr_t Off(const void* p,const void* b){ return intptr_t(p)-intptr_t(b); }
template<class M> uintptr_t MA(M m){ static_assert(sizeof(M)==sizeof(uintptr_t)); uintptr_t a{}; std::memcpy(&a,&m,sizeof(a)); return a; }

// --- arranque del banco (se ejecuta al cargar la DLL, solo si existe data23-runtime.start) ---
// GTA moria con 0xC0000409 ANTES de ejecutar ningun caso (sin registro): durante la carga de la partida de FastLoader,
// CTheScripts::Init (0x468D50) hace CFileMgr::SetDir("data\\script") + fopen("main.scm") y fread(NULL) -> __fastfail(5).
// CFileMgr::SetDir cambia el directorio de trabajo GLOBAL del proceso y el hilo de streaming de audio del EXE
// (CAEStreamThread::Service 0x4F1290 -> CAETrackLoader::GetDataStream 0x4E0D20) llama a SetDir("") (call en 0x4E0E3D)
// desde otro hilo: si cae entre el SetDir y el fopen, "main.scm" no se encuentra. En el volcado del fallo el hilo de
// streaming estaba preparando la pista 0xAE (tiempo -2) y el directorio de trabajo era la raiz del juego.
// Solo en el banco: ese call no hace nada (el streaming abre "AUDIO\\STREAMS\\..." relativo al directorio actual).
inline int __cdecl NoSetDir(const char*){ return 0; }
// Si aun asi GTA muere por un parametro invalido del CRT antes del primer fotograma, queda escrito donde (RVA en el ASI).
inline void __cdecl BootInvalidParameter(const wchar_t*,const wchar_t*,const wchar_t*,unsigned,uintptr_t){
    char cwd[MAX_PATH]{}; GetCurrentDirectoryA(MAX_PATH,cwd);
    auto clean=[](const char* s,size_t max){ std::string r; for(size_t i=0;i<max&&s[i];++i)r+=(uint8(s[i])>=0x20&&uint8(s[i])<0x7F) ? s[i] : '?'; return r; };
    HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&BootInvalidParameter),&mod);
    void* frames[24]{}; const auto n=CaptureStackBackTrace(1,24,frames,nullptr);
    json st=json::array(); for(USHORT k=0;k<n;++k)st.push_back(uint32(reinterpret_cast<uintptr_t>(frames[k])-reinterpret_cast<uintptr_t>(mod)));
    ReversingProbe::Fail({{"event","boot_invalid_parameter"},{"cwd",clean(cwd,MAX_PATH)},{"dirName",clean(reinterpret_cast<const char*>(0xB71A60),128)},
                          {"thread",GetCurrentThreadId()},{"stack_rva",st}},95);
}
inline bool BootSetup() {
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"data23-runtime.start"))return false;
    ReversingProbe::failureLog=dir/"data23-runtime.jsonl";
    _set_invalid_parameter_handler(BootInvalidParameter);
    constexpr uintptr_t site=0x4E0E3D; auto* p=reinterpret_cast<uint8*>(site);
    if(p[0]==0xE8 && uint32(site+5+*reinterpret_cast<int32*>(p+1))==0x5387D0u) {   // solo si sigue siendo "call SetDir"
        DWORD old{},ignored{}; const int32 rel=int32(reinterpret_cast<uintptr_t>(&NoSetDir)-(site+5));
        if(VirtualProtect(p+1,4,PAGE_EXECUTE_READWRITE,&old)){ std::memcpy(p+1,&rel,4); VirtualProtect(p+1,4,old,&ignored); FlushInstructionCache(GetCurrentProcess(),p,5); }
    }
    return true;
}
inline const bool bootSetup=BootSetup();

// --- RNG de los stubs (depende solo del caso y del orden de llamadas) ---
inline uint32 sst{};
inline uint32 SNext(){ sst^=sst<<13; sst^=sst>>17; sst^=sst<<5; return sst; }

// --- registro sin reservas (para stubs activos mientras new[]/delete[] están sustituidos) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,256> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }
alignas(16) inline std::array<uint8,0x900> arena{};
inline void* __cdecl FakeNewArr(size_t n){ Push(1,uint32(n)); return n<=arena.size() ? arena.data() : nullptr; }
inline void __cdecl FakeDelete(void* p){ Push(2,uint32(uintptr_t(p))); }

// --- parches de entrada agrupados ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v;
    template<class G> void Add(uintptr_t at,G f){ if(at)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
};

// --- entidades falsas ---
alignas(16) inline std::array<uint8,0x800> bPedT{}, bPedC{}, bPedC2{};
alignas(16) inline std::array<uint8,0xA18> bVeh{};
alignas(16) inline std::array<uint8,0x200> bObj{};
alignas(16) inline std::array<uint8,0x40> bBld{}, bDum{}, bClump{}, bAssoc{}, bIntel{};
alignas(16) inline std::array<uint8,0x100> bPd{};
alignas(16) inline std::array<uint8,0x50> bObjInfo{};
inline const char* EName(const void* p){
    if(!p)return "null";
    if(p==bPedT.data())return "ped_t"; if(p==bPedC.data())return "ped_c"; if(p==bPedC2.data())return "ped_c2";
    if(p==bVeh.data())return "veh"; if(p==bObj.data())return "obj"; if(p==bBld.data())return "bld"; if(p==bDum.data())return "dum";
    if(p==bClump.data())return "clump"; if(p==bAssoc.data())return "assoc"; if(p==bIntel.data())return "intel";
    return "other";
}
inline void __fastcall FakeTrap(void* self,void*){ T({"trap",EName(self)}); }
inline std::array<void*,64> vtTrap{}, vtVeh{}, vtObj{}, vtVehDel{};

// --- CAEPedWeaponAudioEntity ---
inline void* audBase{}; inline uint32 swimMask{}, swimCalls{};
inline void __fastcall SWeaponFire(void* s,void*,int32 t,void* e,int32 ev){ T({"weapon_fire",Off(s,audBase),t,EName(e),ev}); }
inline void __fastcall SWeaponReload(void* s,void*,int32 t,void* e,int32 ev){ T({"weapon_reload",Off(s,audBase),t,EName(e),ev}); }
inline void __fastcall SStealthKill(void* s,void*,int32 t,void* e,int32 ev){ T({"stealth_kill",Off(s,audBase),t,EName(e),ev}); }
inline void __fastcall SChainsaw(void* s,void*,void* e,int32 ev){ T({"chainsaw",Off(s,audBase),EName(e),ev}); }
inline void __fastcall SWAInit(void* s,void*){ T({"wa_initialise",Off(s,audBase)}); }
inline void __fastcall SWATerm(void* s,void*){ T({"wa_terminate",Off(s,audBase)}); }
inline void __fastcall SPlayGas(void* s,void*,void* e){ T({"play_gas",Off(s,audBase),EName(e)}); }
inline void __fastcall SStopGas(void* s,void*){ T({"stop_gas",Off(s,audBase)}); }
inline void* __fastcall SGetTaskSwim(void* intel,void*){ const bool r=(swimMask>>(swimCalls++%32))&1; T({"get_task_swim",EName(intel),r}); return r ? bDum.data() : nullptr; }

// --- CBulletInfo::Update ---
inline json V3(const CVector& v){ return json::array({F(v.x),F(v.y),F(v.z)}); }
inline bool __fastcall SIsPointerValid(void* p,void*){ const bool r=SNext()%4!=0; T({"pointer_valid",EName(p),r}); return r; }
inline bool __cdecl SPlos(const CVector& o,const CVector& t,CColPoint& cp,CEntity*& e,bool b1,bool b2,bool b3,bool b4,bool b5,bool b6,bool b7,bool b8){
    T({"plos",V3(o),V3(t),EName(*reinterpret_cast<void**>(0xB7CD68)),ReversingProbe::Bytes(reinterpret_cast<void*>(0xB7CD6F),3),b1,b2,b3,b4,b5,b6,b7,b8});
    e=nullptr;
    if(SNext()%5<2)return false;
    static void* const pick[]{bPedT.data(),bPedT.data(),bPedC.data(),bVeh.data(),bVeh.data(),bObj.data(),bObj.data(),bBld.data(),bDum.data()};
    e=static_cast<CEntity*>(pick[SNext()%std::size(pick)]);
    auto* b=reinterpret_cast<uint8*>(&cp); for(size_t k=0;k<sizeof(CColPoint);++k)b[k]=uint8(SNext());
    auto u=[](float a,float c){ return a+(c-a)*float(SNext()%100000)/100000.f; };
    cp.m_vecPoint=CVector{u(o.x,t.x),u(o.y,t.y),u(o.z,t.z)};
    cp.m_vecNormal=CVector{u(-1.f,1.f),u(-1.f,1.f),u(-1.f,1.f)};
    { // asin del CRT del EXE con argumento -dot >= 1 (+-1 aprox., 1.5, 2, 1e30, FLT_MAX por eje -> +inf) y diminuto (subnormales)
        const uint32 mode=SNext()%5;
        const CVector d{t.x-o.x,t.y-o.y,t.z-o.z}; const float len=std::sqrt((d.x*d.x+d.y*d.y)+d.z*d.z);
        if(mode==0&&len>0.f){ static const float sc[]{1.f,0.99999994f,1.0000001f,1.5f,2.f,1e30f,1e-30f,1e-40f,1e-44f}; const float k=sc[SNext()%std::size(sc)]/len; cp.m_vecNormal=CVector{-d.x*k,-d.y*k,-d.z*k}; }
        else if(mode==1&&len>0.f){ const float m=3.4028235e38f; cp.m_vecNormal=CVector{d.x>0.f ? -m : m,d.y>0.f ? -m : m,d.z>0.f ? -m : m}; }
    }
    *reinterpret_cast<float*>(b+0xC)=0.f; *reinterpret_cast<float*>(b+0x1C)=0.f; *reinterpret_cast<float*>(b+0x28)=u(0.f,1.f);
    static const uint8 pieces[]{0,3,9,12,13,14,15,16,17,20};
    b[0x24]=pieces[SNext()%std::size(pieces)];
    return true;
}
inline bool __cdecl SCheckOccupant(CEntity** pe,CColPoint* cp,int32 type,const CVector& o,const CVector& t){
    T({"check_occupant",EName(*pe),type,V3(o),V3(t),F(cp->m_vecPoint.x)});
    if(*pe==static_cast<void*>(bVeh.data())&&SNext()%6==0)*pe=reinterpret_cast<CEntity*>(bPedT.data());
    return false;
}
inline int32 __fastcall SLocalDir(void* p,void*,const CVector2D* v){ const int32 r=int32((F(v->x)^F(v->y))&3); T({"local_dir",EName(p),F(v->x),F(v->y)}); return r; }
inline bool __cdecl SDamageEvent(CPed* v,CEntity* c,int32 type,int32 dmg,int32 piece,uint8 dir){ T({"damage_event",EName(v),EName(c),type,dmg,piece&0xFF,dir}); return true; }
inline void __cdecl SReportCrime(int32 crime,CEntity* e,CPed* p){ T({"report_crime",crime,EName(e),EName(p)}); }
inline uint32 bloodMask{};
inline bool __cdecl SBlood(){ const bool r=(bloodMask>>(SNext()%32))&1; T({"blood",r}); return r; }
inline void __fastcall SAddBlood(void* fx,void*,const CVector* p,const CVector* n,int32 amount,float br){ T({"add_blood",Off(fx,&g_fx),V3(*p),V3(*n),amount,F(br)}); }
inline void* __cdecl SFirstAssoc(void* clump,uint32 flags){ const bool r=SNext()%2; T({"first_assoc",EName(clump),flags,r}); return r ? bAssoc.data() : nullptr; }
inline void* __cdecl SBlend(void* clump,int32 grp,int32 anim,float d){ const bool r=SNext()%4!=0; T({"blend",EName(clump),grp,anim,F(d),r}); return r ? bAssoc.data() : nullptr; }
inline void __fastcall SSetCurrentTime(void* a,void*,float t){ T({"set_current_time",EName(a),F(t)}); }
inline bool __fastcall SBurstTyre(void* v,void*,uint32 comp,uint32 phys){ T({"burst_tyre",EName(v),comp&0xFF,phys&0xFF}); return true; }
inline void __fastcall SInflictDamage(void* v,void*,void* dmgr,int32 type,float in,float x,float y,float z){ T({"inflict_damage",EName(v),EName(dmgr),type,F(in),F(x),F(y),F(z)}); }
inline void* __fastcall SStartFire(void* fm,void*,void* tgt,void* cr,float size,uint32 a3,uint32 time,uint32 gens){ T({"start_fire",Off(fm,&gFireManager),EName(tgt),EName(cr),F(size),a3&0xFF,time,int(int8(gens&0xFF))}); return nullptr; }
inline bool __fastcall SSphereVisible(void* cam,void*,const CVector* o,float r,void* m){ const bool v=SNext()%3!=0; T({"sphere_visible",Off(cam,&TheCamera),V3(*o),F(r),Off(m,cam),v}); return v; }
inline float __cdecl SLightExe(uint32 l,float s){ T({"lighting",l&0xFF,F(s)}); return float(l&0xFF)*0.0625f+s; }
inline float __fastcall SLightDll(const uint8* self,void*,float s){ T({"lighting",*self,F(s)}); return float(*self)*0.0625f+s; }
inline void __fastcall SBulletImpact(void* fx,void*,const CVector* p,const CVector* n,int32 surf,int32 amount,float l){ T({"bullet_impact",Off(fx,&g_fx),V3(*p),V3(*n),surf,amount,F(l)}); }
inline void __fastcall STyreBurst(void* fx,void*,const CVector* p,const CVector* v){ T({"tyre_burst",Off(fx,&g_fx),V3(*p),V3(*v)}); }
inline void __fastcall SObjectDamage(void* o,void*,float d,const CVector* p,const CVector* n,void* dm,int32 type){ T({"object_damage",EName(o),F(d),V3(*p),V3(*n),EName(dm),type}); }
inline void __fastcall SSetIsStatic(void* e,void*,uint32 b){ T({"set_is_static",EName(e),b&0xFF}); }
inline void __fastcall SAddToMoving(void* p,void*){ T({"add_to_moving",EName(p)}); }
inline void __fastcall SApplyMoveForce(void* p,void*,float x,float y,float z){ T({"apply_move_force",EName(p),F(x),F(y),F(z)}); }
inline void __fastcall SBulletHit(void* ae,void*,void* e,uint32 surf,const CVector* p,float a){ T({"bullet_hit",Off(ae,&AudioEngine),EName(e),surf&0xFF,V3(*p),F(a)}); }
inline void __cdecl SGlassHit(void* e,float x,float y,float z){ T({"glass_hit",EName(e),F(x),F(y),F(z)}); }
inline void __cdecl SSniperCol(CVector* a,CVector* b){ T({"sniper_collision",Off(a,reinterpret_cast<void*>(0xC888A0)),Off(b,reinterpret_cast<void*>(0xC888A0)),V3(*a),V3(*b)}); }

// --- CDamageAtomicModelInfo ---
inline void* miBase{}; inline uint32 pipeMask{}, pipeCalls{};
inline void __fastcall SBaseInit(void* s,void*){ T({"base_init",Off(s,miBase)}); }
inline void __fastcall SAtomicDelete(void* s,void*){ T({"atomic_delete_rw",Off(s,miBase)}); }
inline RwBool __cdecl SRpAtomicDestroy(RpAtomic* a){ T({"rp_atomic_destroy",Off(a,miBase)}); return TRUE; }
inline RwBool __cdecl SRwFrameDestroy(RwFrame* f){ T({"rw_frame_destroy",Off(f,miBase)}); return TRUE; }
inline bool __cdecl SIsCBPC(RpAtomic* a){ const bool r=(pipeMask>>(pipeCalls++%32))&1; T({"is_cbpc",Off(a,miBase),r}); return r; }
inline bool __cdecl SIsCCPC(RpAtomic* a){ const bool r=(pipeMask>>(pipeCalls++%32))&1; T({"is_ccpc",Off(a,miBase),r}); return r; }
inline void __cdecl SAtomicSetup(RpAtomic* a){ T({"cb_atomic_setup",Off(a,miBase)}); }
inline RpAtomic* __cdecl SSetFx(RpAtomic* a,void* d){ T({"set_fx_pipelines",Off(a,miBase),uintptr_t(d)}); return a; }

// --- CGarage ---
inline void* garBase{}; inline uint8* vehBase{}; inline void* storedBase{};
inline json VName(const void* v){ const auto o=Off(v,vehBase); return o>=0&&o<0xA18*10&&o%0xA18==0 ? json(o/0xA18) : json(EName(v)); }
inline bool __fastcall SInsideGarage(void* g,void*,float x,float y,float z){ const bool r=SNext()%3!=0; T({"inside_garage",Off(g,garBase),F(x),F(y),F(z),r}); return r; }
inline bool __fastcall SSphereOutside(void* g,void*,void* e,float r){ const bool v=SNext()%4==0; T({"sphere_outside",Off(g,garBase),VName(e),F(r),v}); return v; }
inline void __fastcall SStoreCar(void* sc,void*,void* v){ T({"store_car",Off(sc,storedBase),VName(v)}); }
inline void __fastcall SCancelEnterExe(void*,void*,void* v){ T({"cancel_entering",VName(v)}); }
inline void __fastcall SCancelEnterDll(void*,void*,void* v){ T({"cancel_entering",VName(v)}); }
inline void __cdecl SWorldRemove(void* e){ T({"world_remove",VName(e)}); }
inline void* __fastcall SVehDelete(void* v,void*,int32 flags){ T({"vehicle_delete",VName(v),flags}); return v; }

// --- ficheros ---
inline std::vector<std::string> lines; inline size_t lineIdx{}; inline std::array<char,512> lineBuf{};
inline FILESTREAM __cdecl SOpenFile(const char* p,const char* m){ T({"open_file",std::string(p),std::string(m)}); return reinterpret_cast<FILESTREAM>(0x1234); }
inline int32 __cdecl SCloseFile(FILESTREAM f){ T({"close_file",uintptr_t(f)}); return 0; }
inline int32 __cdecl SSetDir(const char* p){ T({"set_dir",std::string(p)}); return 0; }
inline char* __cdecl SLoadLine(FILESTREAM f){ T({"load_line",uintptr_t(f)}); if(lineIdx>=lines.size())return nullptr; strcpy(lineBuf.data(),lines[lineIdx++].c_str()); return lineBuf.data(); }
inline void __cdecl SLoadDefaultDM(){ T({"load_default_decision_maker"}); }
inline uint32 __fastcall SSurfaceId(void* self,void*,const char* n){
    if(!n){ T({"surface_id",nullptr}); return 0; }
    uint32 id=0; if(std::strncmp(n,"SURF_",5)==0)id=uint32(std::atoi(n+5));
    T({"surface_id",Off(self,&g_surfaceInfos),std::string(n),id}); return id;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
inline uintptr_t OurAddress(ReversibleHooks::ReversibleHook::Base* h) {
    if(!h)return 0;
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Virtual)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(h)->GetHookOurAddress());
    if(h->Type()==ReversibleHooks::ReversibleHook::Base::HookType::Simple)return uintptr_t(static_cast<ReversibleHooks::ReversibleHook::Simple*>(h)->GetHookOurAddress());
    return 0;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,32> fns{{
    {"CAEPedWeaponAudioEntity","AddAudioEvent",0x4E69F0,0,-1,1024},{"CAEPedWeaponAudioEntity","Initialise",0x4E6AA0,0,-1,128},
    {"CAEPedWeaponAudioEntity","Service",0x4E6AE0,0,-1,512},{"CAEPedWeaponAudioEntity","Terminate",0x4E6AC0,0,-1,128},
    {"CBulletInfo","AddBullet",0x736010,0,-1,1024},{"CBulletInfo","Initialise",0x735FD0,0,-1,64},{"CBulletInfo","Shutdown",0x736000,0,-1,16},
    {"CBulletInfo","Update",0x7360D0,0,-1,1024},
    {"CDamageAtomicModelInfo","AsDamageAtomicModelInfoPtr",0,0x85BC30,2,64},{"CDamageAtomicModelInfo","DeleteRwObject",0,0x85BC30,8,256},
    {"CDamageAtomicModelInfo","Init",0,0x85BC30,6,128},{"CDamageAtomicModelInfo","SetDamagedAtomic",0x4C48D0,0,-1,256},
    {"CGarage","BuildRotatedDoorMatrix",0x4479F0,0,-1,1024},{"CGarage","CloseThisGarage",0x447D70,0,-1,256},{"CGarage","OpenThisGarage",0x447D50,0,-1,256},
    {"CGarage","StoreAndRemoveCarsForThisHideOut",0x449900,0,-1,512},
    {"CPedStats","GetPedStatType",0x6088D0,0,-1,512},{"CPedStats","Initialise",0x5BF9D0,0,-1,64},{"CPedStats","LoadPedStats",0x5BB890,0,-1,256},
    {"CPedStats","Shutdown",0x608850,0,-1,64},
    {"CPlantSurfPropMgr","AllocSurfProperties",0x5DD370,0,-1,1024},{"CPlantSurfPropMgr","GetSurfProperties",0x6F9DE0,0,-1,512},
    {"CPlantSurfPropMgr","Initialise",0x5DD6C0,0,-1,64},{"CPlantSurfPropMgr","LoadPlantsDat",0x5DD3B0,0,-1,512},
    {"CScriptResourceManager","AddToResourceManager",0x4704B0,0,-1,1024},{"CScriptResourceManager","HasResourceBeenRequested",0x470620,0,-1,512},
    {"CScriptResourceManager","Initialise",0x470480,0,-1,64},{"CScriptResourceManager","RemoveFromResourceManager",0x470510,0,-1,1024},
    {"CSpecialPlateHandler","Add",0x6F2D90,0,-1,512},{"CSpecialPlateHandler","Find",0x6F2D30,0,-1,512},{"CSpecialPlateHandler","Init",0x6F2D10,0,-1,64},
    {"CSpecialPlateHandler","Remove",0x6F2DD0,0,-1,512},
}};

// regiones globales tocadas
constexpr uintptr_t BULLETS=0xC88740; constexpr size_t NBULLETS=0x160+0x18;   // aBulletInfos + PlayerSniperBulletStart/End
constexpr uintptr_t WFLAGS=0xB7CD68; constexpr size_t NWFLAGS=0xA;             // pIgnoreEntity .. bIncludeDeadPeds
constexpr uintptr_t PLANTS=0xC38070; constexpr size_t NPLANTS=0xC39ED8-0xC38070; // m_SurfPropPtrTab, m_SurfPropTab, m_countSurfPropsAllocated

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"data23-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"data23-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("data23");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"data23-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,32> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    const uintptr_t atomicDeleteOur=OurAddress(FindHook("CAtomicModelInfo","DeleteRwObject")), baseInitOur=OurAddress(FindHook("CBaseModelInfo","Init"));
    if(!atomicDeleteOur||!baseInitOur){Write({{"event","error"},{"reason","dependency hook not found"},{"atomic_delete",atomicDeleteOur},{"base_init",baseInitOur}});return;}
    std::array<bool,32> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& timeStep=*reinterpret_cast<float*>(0xB7CB5C); const float savedStep=timeStep;
    auto* bullets=reinterpret_cast<uint8*>(BULLETS); const std::vector<uint8> savedBullets(bullets,bullets+NBULLETS);
    auto* wflags=reinterpret_cast<uint8*>(WFLAGS); const std::vector<uint8> savedWFlags(wflags,wflags+NWFLAGS);
    auto* plants=reinterpret_cast<uint8*>(PLANTS); const std::vector<uint8> savedPlants(plants,plants+NPLANTS);
    auto& vehPool=*reinterpret_cast<void**>(0xB74494); void* const savedVehPool=vehPool;
    auto& pedStats=*reinterpret_cast<void**>(0xC0BBEC); void* const savedPedStats=pedStats;
    auto restoreAll=[&]{ timeMs=savedTime; timeStep=savedStep; std::memcpy(bullets,savedBullets.data(),NBULLETS); std::memcpy(wflags,savedWFlags.data(),NWFLAGS);
                         std::memcpy(plants,savedPlants.data(),NPLANTS); vehPool=savedVehPool; pedStats=savedPedStats; };
    auto stateOk=[&]{ return timeMs==savedTime&&F(timeStep)==F(savedStep)&&std::memcmp(bullets,savedBullets.data(),NBULLETS)==0&&std::memcmp(wflags,savedWFlags.data(),NWFLAGS)==0
                         &&std::memcmp(plants,savedPlants.data(),NPLANTS)==0&&vehPool==savedVehPool&&pedStats==savedPedStats; };

    for(auto& v:vtTrap)v=reinterpret_cast<void*>(&FakeTrap);
    vtVeh=vtTrap; vtVeh[44]=reinterpret_cast<void*>(&SBurstTyre);
    vtObj=vtTrap; vtObj[4]=reinterpret_cast<void*>(&SSetIsStatic);
    vtVehDel=vtTrap; vtVehDel[0]=reinterpret_cast<void*>(&SVehDelete);

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xDA23u; if(!stt)stt=1; trace=json::array(); nrec=0;
                    sst=next()|1; swimCalls=pipeCalls=0; lines.clear(); lineIdx=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CAEPedWeaponAudioEntity") {
                        alignas(16) static std::array<uint8,0xA8> ent{}; for(auto& b:ent)b=uint8(next()); *reinterpret_cast<uintptr_t*>(ent.data())=0x86C2AC; audBase=ent.data();
                        for(auto& b:bPedT)b=uint8(next()); bPedT[0x718]=uint8(next()%13);
                        static const int32 wt[]{0,0,22,24,25,31,34,37,37,38,9,4,16,46};
                        for(int s=0;s<13;++s)*reinterpret_cast<int32*>(bPedT.data()+0x5A0+s*0x1C)=wt[next()%std::size(wt)];
                        *reinterpret_cast<void**>(bPedT.data()+0x47C)=bIntel.data(); *reinterpret_cast<void**>(bPedT.data())=vtTrap.data();
                        ent[0xA0]=uint8(next()%4 ? 1 : 0); *reinterpret_cast<void**>(ent.data()+0xA4)=next()%6 ? bPedT.data() : nullptr;
                        swimMask=next();
                        Patches P;
                        P.Add(0x504F80,SWeaponFire); P.Add(MA(&CAEWeaponAudioEntity::WeaponFire),SWeaponFire);
                        P.Add(0x503690,SWeaponReload); P.Add(MA(&CAEWeaponAudioEntity::WeaponReload),SWeaponReload);
                        P.Add(0x503B20,SStealthKill); P.Add(MA(&CAEWeaponAudioEntity::ReportStealthKill),SStealthKill);
                        P.Add(0x503910,SChainsaw); P.Add(MA(&CAEWeaponAudioEntity::ReportChainsawEvent),SChainsaw);
                        P.Add(0x503450,SWAInit); P.Add(MA(&CAEWeaponAudioEntity::Initialise),SWAInit);
                        P.Add(0x503480,SWATerm); P.Add(MA(&CAEWeaponAudioEntity::Terminate),SWATerm);
                        P.Add(0x503870,SPlayGas); P.Add(MA(&CAEWeaponAudioEntity::PlayFlameThrowerIdleGasLoop),SPlayGas);
                        P.Add(0x5034E0,SStopGas); P.Add(MA(&CAEWeaponAudioEntity::StopFlameThrowerIdleGasLoop),SStopGas);
                        P.Add(0x601070,SGetTaskSwim); P.Add(MA(&CPedIntelligence::GetTaskSwim),SGetTaskSwim);
                        if(nm=="AddAudioEvent"){ const int32 ev=next()%6 ? 0x91+int32(next()%12) : int32(next()%300); reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(ent.data(),ev); }
                        else if(nm=="Initialise"){ void* const p=next()%4 ? static_cast<void*>(bPedT.data()) : next()%2 ? static_cast<void*>(bDum.data()) : nullptr; reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(ent.data(),p); }
                        else if(nm=="Terminate")reinterpret_cast<void(__thiscall*)(void*)>(a)(ent.data());
                        else reinterpret_cast<void(__thiscall*)(void*)>(a)(ent.data());
                        out["ent"]=ReversingProbe::Bytes(ent.data(),ent.size());
                    }
                    else if(cls=="CBulletInfo") {
                        for(size_t k=0;k<NBULLETS;++k)bullets[k]=uint8(next());
                        static const int32 types[]{22,23,24,25,26,27,28,29,30,31,32,33,34,34,34,37,38};
                        void* const creators[]{nullptr,bPedC.data(),bPedC2.data(),bPedT.data(),bVeh.data(),bObj.data(),bPedC.data(),bPedC2.data()};
                        timeMs=next()%5==0 ? next() : 100000+next()%1000000; timeStep=next()%8==0 ? 0.f : uni(0.f,3.f);
                        for(int k=0;k<8;++k){ auto* b=bullets+k*0x2C;
                            *reinterpret_cast<int32*>(b)=types[next()%std::size(types)]; *reinterpret_cast<void**>(b+4)=creators[next()%std::size(creators)];
                            float dt; switch(next()%5){case 0:dt=float(timeMs);break;case 1:dt=float(int32(timeMs))+float(int32(next()%3000)-1500);break;case 2:dt=uni(-1e9f,4.3e9f);break;default:dt=float(timeMs)+uni(-2000.f,2000.f);}
                            *reinterpret_cast<float*>(b+8)=dt; b[0xC]=uint8(next()%4 ? 1 : 0);
                            CVector p{uni(-2900.f,2900.f),uni(-2900.f,2900.f),uni(-100.f,500.f)};
                            if(next()%8==0)p.x=next()%2 ? uni(2990.f,3010.f) : uni(-3010.f,-2990.f); if(next()%8==0)p.y=next()%2 ? uni(2990.f,3010.f) : uni(-3010.f,-2990.f);
                            CVector v{uni(-300.f,300.f),uni(-300.f,300.f),uni(-100.f,100.f)}; if(next()%10==0)v*=50.f;
                            *reinterpret_cast<CVector*>(b+0x10)=p; *reinterpret_cast<CVector*>(b+0x1C)=v; *reinterpret_cast<int16*>(b+0x28)=int16(next());
                        }
                        if(nm=="Initialise")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="AddBullet"){ void* const cr=reinterpret_cast<void*>(uintptr_t(next())); const int32 type=int32(next()%47);
                            const CVector p{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-100.f,1000.f)}, v{uni(-500.f,500.f),uni(-500.f,500.f),uni(-500.f,500.f)};
                            ret=reinterpret_cast<bool(__cdecl*)(void*,int32,CVector,CVector)>(a)(cr,type,p,v); }
                        else {
                            // entidades falsas
                            auto setEnt=[&](auto& buf,uint8 type,void* vt){ for(auto& b:buf)b=uint8(next()); *reinterpret_cast<void**>(buf.data())=vt; *reinterpret_cast<void**>(buf.data()+0x14)=nullptr;
                                *reinterpret_cast<CVector*>(buf.data()+4)=CVector{uni(-2900.f,2900.f),uni(-2900.f,2900.f),uni(-50.f,200.f)}; buf[0x36]=uint8((buf[0x36]&~7)|type); };
                            setEnt(bBld,1,vtTrap.data()); setEnt(bDum,5,vtTrap.data()); setEnt(bVeh,2,vtVeh.data()); setEnt(bObj,4,vtObj.data());
                            for(auto* ped:{&bPedT,&bPedC,&bPedC2}){ setEnt(*ped,3,vtTrap.data()); auto* d=ped->data();
                                static const int32 states[]{1,7,54,55,55,0};
                                *reinterpret_cast<int32*>(d+0x530)=states[next()%std::size(states)]; *reinterpret_cast<int32*>(d+0x598)=next()%3==0 ? 6 : int32(next()%20);
                                *reinterpret_cast<float*>(d+0x12C)=uni(0.f,1.f); *reinterpret_cast<void**>(d+0x18)=bClump.data();
                                void* const att[]{nullptr,nullptr,bVeh.data(),bObj.data(),bBld.data(),bDum.data()}; *reinterpret_cast<void**>(d+0xFC)=att[next()%std::size(att)];
                                *reinterpret_cast<void**>(d+0x480)=nullptr; }
                            *reinterpret_cast<void**>(bPedC2.data()+0x480)=bPd.data(); for(auto& b:bPd)b=uint8(next());
                            for(auto& b:bClump)b=uint8(next()); bClump[0]=2; for(auto& b:bAssoc)b=uint8(next());
                            *reinterpret_cast<void**>(bVeh.data()+0xFC)=nullptr;
                            static const uint8 effects[]{0,100,199,200,201,250}; bObj[0x144]=effects[next()%std::size(effects)];
                            *reinterpret_cast<void**>(bObj.data()+0x160)=bObjInfo.data(); for(auto& b:bObjInfo)b=uint8(next());
                            *reinterpret_cast<uint32*>(bObjInfo.data()+0x48)=uint32(next()%4);
                            static const float mults[]{1.f,50.f,99.8f,99.9f,100.f,150.f}; *reinterpret_cast<float*>(bObjInfo.data()+0x18)=mults[next()%std::size(mults)];
                            if(next()%16==0)*reinterpret_cast<uint32*>(bObjInfo.data()+0x18)=0x7FC00000u; // NaN silencioso
                            static const float uproots[]{-1.f,0.f,0.f,5.f}; *reinterpret_cast<float*>(bObjInfo.data()+0x14)=uproots[next()%std::size(uproots)];
                            *reinterpret_cast<float*>(bObjInfo.data()+0x34)=uni(0.f,3.f);
                            bloodMask=next();
                            Patches P;
                            P.Add(0x5E4220,SIsPointerValid); P.Add(MA(&CPed::IsPointerValid),SIsPointerValid);
                            P.Add(0x56BA00,SPlos); P.Add(MA(&CWorld::ProcessLineOfSight),SPlos);
                            P.Add(0x73F480,SCheckOccupant); P.Add(MA(&CWeapon::CheckForShootingVehicleOccupant),SCheckOccupant); // el candidato en C++ llama a la funcion de la DLL
                            P.Add(0x5DEF60,SLocalDir); P.Add(MA(&CPed::GetLocalDirection),SLocalDir);
                            P.Add(0x73A530,SDamageEvent); P.Add(MA(&CWeapon::GenerateDamageEvent),SDamageEvent);
                            P.Add(0x532010,SReportCrime); P.Add(MA(&CCrime::ReportCrime),SReportCrime);
                            P.Add(0x56D230,SBlood); P.Add(MA(&CLocalisation::Blood),SBlood);
                            P.Add(0x49EB00,SAddBlood); P.Add(0x49F3D0,SBulletImpact); P.Add(MA(&Fx_c::AddBulletImpact),SBulletImpact); /* lote 78A: ya es C++ real en la DLL */ P.Add(0x49F300,STyreBurst); P.Add(MA(&Fx_c::AddTyreBurst),STyreBurst); /* ya era C++ real en la DLL (hallado en la regresion del lote 78A) */
                            P.Add(0x4D6A70,SFirstAssoc); P.Add(MA(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation)),SFirstAssoc);
                            P.Add(0x4D4610,SBlend); P.Add(MA(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation)),SBlend);
                            P.Add(0x4CEA80,SSetCurrentTime); P.Add(MA(&CAnimBlendAssociation::SetCurrentTime),SSetCurrentTime);
                            P.Add(0x6D7C90,SInflictDamage);
                            P.Add(0x53A050,SStartFire); P.Add(MA(static_cast<CFire*(CFireManager::*)(CEntity*,CEntity*,float,uint8,uint32,int8)>(&CFireManager::StartFire)),SStartFire);
                            P.Add(0x420C40,SSphereVisible); P.Add(MA(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible)),SSphereVisible);
                            P.Add(0x59F0C0,SLightExe); P.Add(MA(&tColLighting::GetCurrentLighting),SLightDll);
                            P.Add(0x5A0D90,SObjectDamage); P.Add(MA(&CObject::ObjectDamage),SObjectDamage);
                            P.Add(0x542800,SAddToMoving); P.Add(MA(&CPhysical::AddToMovingList),SAddToMoving);
                            P.Add(0x5429F0,SApplyMoveForce); P.Add(MA(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),SApplyMoveForce);
                            P.Add(0x506EC0,SBulletHit); P.Add(MA(&CAudioEngine::ReportBulletHit),SBulletHit);
                            P.Add(0x71C0D0,SGlassHit); P.Add(MA(&CGlass::WasGlassHitByBullet),SGlassHit);
                            P.Add(0x6C6890,SSniperCol); P.Add(MA(&CHeli::TestSniperCollision),SSniperCol);
                            reinterpret_cast<void(__cdecl*)()>(a)();
                            out["pd"]=ReversingProbe::Bytes(bPd.data(),bPd.size()); out["assoc"]=ReversingProbe::Bytes(bAssoc.data(),bAssoc.size());
                        }
                        out["bullets"]=ReversingProbe::Bytes(bullets,NBULLETS); out["wflags"]=ReversingProbe::Bytes(wflags,NWFLAGS);
                        timeMs=savedTime; timeStep=savedStep; std::memcpy(bullets,savedBullets.data(),NBULLETS); std::memcpy(wflags,savedWFlags.data(),NWFLAGS);
                    }
                    else if(cls=="CDamageAtomicModelInfo") {
                        alignas(16) static std::array<uint8,0x80> mi{}; for(auto& b:mi)b=uint8(next()); *reinterpret_cast<uintptr_t*>(mi.data())=0x85BC30; miBase=mi.data();
                        // atomo falso en +0x40 con su frame (siempre no nulo: RwFrameDestroy del DLL lo exige) en +0x60
                        *reinterpret_cast<void**>(mi.data()+0x44)=mi.data()+0x60;
                        pipeMask=next();
                        Patches P;
                        P.Add(0x4C4B10,SBaseInit); P.Add(baseInitOur,SBaseInit);
                        P.Add(0x4C4440,SAtomicDelete); P.Add(atomicDeleteOur,SAtomicDelete);
                        P.Add(0x749DC0,SRpAtomicDestroy); P.Add(0x7F05A0,SRwFrameDestroy);
                        P.Add(0x5D7F40,SIsCBPC); P.Add(MA(&CCustomBuildingRenderer::IsCBPCPipelineAttached),SIsCBPC);
                        P.Add(0x5D7F00,SAtomicSetup); P.Add(MA(&CCustomBuildingRenderer::AtomicSetup),SAtomicSetup);
                        P.Add(0x5D5B80,SIsCCPC); P.Add(MA(&CCarFXRenderer::IsCCPCPipelineAttached),SIsCCPC);
                        P.Add(0x5D5B60,SSetFx); P.Add(MA(&CCarFXRenderer::SetCustomFXAtomicRenderPipelinesVMICB),SSetFx);
                        if(nm=="AsDamageAtomicModelInfoPtr")ret=Off(reinterpret_cast<void*(__thiscall*)(void*)>(a)(mi.data()),mi.data());
                        else if(nm=="Init")reinterpret_cast<void(__thiscall*)(void*)>(a)(mi.data());
                        else if(nm=="DeleteRwObject"){ *reinterpret_cast<void**>(mi.data()+0x20)=next()%4 ? mi.data()+0x40 : nullptr; reinterpret_cast<void(__thiscall*)(void*)>(a)(mi.data()); }
                        else { void* const at=next()%6 ? mi.data()+0x40 : nullptr; reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(mi.data(),at); }
                        out["mi"]=ReversingProbe::Bytes(mi.data(),mi.size());
                    }
                    else if(cls=="CGarage") {
                        alignas(16) static std::array<uint8,0xE0> gar{}; for(auto& b:gar)b=uint8(next()); garBase=gar.data();
                        if(nm=="OpenThisGarage"||nm=="CloseThisGarage"){ gar[0x4D]=uint8(next()%8); reinterpret_cast<void(__thiscall*)(void*)>(a)(gar.data()); }
                        else if(nm=="BuildRotatedDoorMatrix"){
                            alignas(16) static std::array<uint8,0x40> ent{}; alignas(16) static std::array<uint8,0x48> mat{};
                            for(auto& b:ent)b=uint8(next()); for(auto& b:mat)b=uint8(next());
                            for(int k=0;k<16;++k)if(k%4!=3)*reinterpret_cast<float*>(mat.data()+k*4)=uni(-1.f,1.f);
                            *reinterpret_cast<void**>(ent.data()+0x14)=mat.data(); *reinterpret_cast<void**>(ent.data())=vtTrap.data();
                            float pos; switch(next()%8){case 0:pos=0.f;break;case 1:pos=1.f;break;case 2:pos=float(next()%9)*0.125f;break;case 3:pos=uni(-50.f,50.f);break;default:pos=uni(0.f,1.f);}
                            if(i>=768){ // estres x87 (casos 768..1023): puerta y forward con NaN (carga/signo distintos, senalizante), +-inf, +-FLT_MAX, subnormales, +-0 y +-1e30/1e-30
                                static const uint32 stress[]{0x7FC12345u,0xFFC00001u,0x7FA00001u,0x7F800000u,0xFF800000u,0x7F7FFFFFu,0xFF7FFFFFu,0x00000001u,0x80000001u,0x007FFFFFu,0x80000000u,0x00800000u,0x7149F2CAu,0x0DA24260u,0xF149F2CAu,0x8DA24260u};
                                pos=bit_cast<float>(stress[next()%16]);
                                const uint32 pick=next()%2, comp=next()%3, value=stress[next()%16];
                                if(pick)*reinterpret_cast<uint32*>(mat.data()+(4+comp)*4)=value;
                            }
                            reinterpret_cast<void(__cdecl*)(void*,float)>(a)(ent.data(),pos);
                            out["mat"]=ReversingProbe::Bytes(mat.data(),mat.size()); out["ent"]=Fnv(ent.data(),ent.size());
                        }
                        else {
                            alignas(16) static std::array<uint8,0xA18*10> vehs{}; alignas(16) static std::array<uint8,16> map{}; alignas(16) static std::array<uint8,0x14> pool{};
                            alignas(16) static std::array<uint8,0x100+0x40> stored{}; alignas(16) static std::array<std::array<uint8,0x48>,10> mats{};
                            vehBase=vehs.data(); storedBase=stored.data();
                            for(auto& b:stored)b=uint8(next());
                            const int32 n=int32(next()%11);
                            for(int32 k=0;k<n;++k){ auto* v=vehs.data()+k*0xA18; std::memset(v,0,0xA18); for(int q=0;q<0x40;++q)v[q]=uint8(next());
                                *reinterpret_cast<void**>(v)=vtVehDel.data(); v[0x36]=uint8((v[0x36]&~7)|2);
                                if(next()%4==0){ std::memset(mats[k].data(),0,0x48); *reinterpret_cast<CVector*>(mats[k].data()+0x30)=CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(0.f,50.f)}; *reinterpret_cast<void**>(v+0x14)=mats[k].data(); }
                                else { *reinterpret_cast<void**>(v+0x14)=nullptr; *reinterpret_cast<CVector*>(v+4)=CVector{uni(-100.f,100.f),uni(-100.f,100.f),uni(0.f,50.f)}; }
                                v[0x4A4]=uint8(1+next()%4); map[k]=uint8(next()%4==0 ? 0x80|(next()&0x7F) : next()&0x7F); }
                            std::memset(pool.data(),0,pool.size()); *reinterpret_cast<void**>(pool.data())=vehs.data(); *reinterpret_cast<void**>(pool.data()+4)=map.data(); *reinterpret_cast<int32*>(pool.data()+8)=n;
                            *reinterpret_cast<int32*>(pool.data()+0xC)=-1;
                            vehPool=pool.data();
                            int32 maxSlot=int32(next()%9)-2; if(next()%10==0)maxSlot=int32(next());
                            {
                                Patches P;
                                P.Add(0x448740,SInsideGarage); P.Add(0x449050,SSphereOutside); P.Add(0x449760,SStoreCar); P.Add(MA(static_cast<bool(CGarage::*)(CVector)>(&CGarage::IsPointInsideGarage)),SInsideGarage); P.Add(MA(&CGarage::EntityHasASphereWayOutsideGarage),SSphereOutside); // el candidato en C++ llama a las funciones de la DLL
                                P.Add(0x56E860,SCancelEnterExe); P.Add(MA(static_cast<void(CPlayerInfo::*)(CVehicle*)>(&CPlayerInfo::CancelPlayerEnteringCars)),SCancelEnterDll);
                                P.Add(0x563280,SWorldRemove); P.Add(MA(&CWorld::Remove),SWorldRemove);
                                reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(gar.data(),stored.data(),maxSlot);
                            }
                            vehPool=savedVehPool;
                            out["stored"]=ReversingProbe::Bytes(stored.data(),stored.size()); out["vehs"]=Fnv(vehs.data(),size_t(n)*0xA18); out["map"]=ReversingProbe::Bytes(map.data(),map.size());
                        }
                        out["gar"]=ReversingProbe::Bytes(gar.data(),gar.size());
                    }
                    else if(cls=="CPedStats") {
                        alignas(16) static std::array<uint8,43*0x34+0x40> stats{}; for(auto& b:stats)b=uint8(next());
                        auto statName=[](uint32 k){ static const char* base[]{"PLAYER","COP","MEDIC","FIREMAN","GANG","STREET_GUY","SUIT_GUY","SENSIBLE_GUY","GEEK_GUY","OLD_GUY","TOUGH_GUY","STEED"};
                            return std::string(base[k%std::size(base)])+(k>=std::size(base) ? std::to_string(k) : std::string()); };
                        auto genLines=[&]{ const uint32 total=next()%50; uint32 valid=0;
                            for(uint32 k=0;k<total;++k){ const uint32 r=next()%10;
                                if(r==0){ lines.push_back("# comentario "+std::to_string(next()%100)); continue; }
                                if(r==1){ lines.push_back(""); continue; }
                                if(valid>=43)continue; ++valid;
                                auto q=[&](int lo,int hi){ return std::to_string(lo+int(next()%uint32(hi-lo+1))); };
                                auto fq=[&](int lo,int hi){ const int v=lo*4+int(next()%uint32((hi-lo)*4+1)); char s[32]; sprintf(s,"%.2f",float(v)*0.25f); return std::string(s); };
                                lines.push_back(statName(next()%60)+" "+fq(0,40)+" "+fq(0,30)+" "+q(0,300)+" "+q(0,300)+" "+q(0,120)+" "+q(0,300)+" "+fq(0,3)+" "+fq(0,3)+" "+q(0,70000)+" "+q(-5,40)); } };
                        if(nm=="GetPedStatType"){ pedStats=stats.data();
                            for(uint32 k=0;k<43;++k){ auto* e=stats.data()+k*0x34; const auto s=statName(next()%30); std::memset(e+4,0,4); strcpy(reinterpret_cast<char*>(e+4),s.c_str()); }
                            std::string q=next()%5 ? statName(next()%30) : std::string("NOEXISTE")+std::to_string(next()%10);
                            ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(q.c_str()); }
                        else if(nm=="LoadPedStats"){ pedStats=stats.data(); genLines();
                            Patches P; P.Add(0x538900,SOpenFile); P.Add(MA(&CFileMgr::OpenFile),SOpenFile); P.Add(0x5389D0,SCloseFile); P.Add(MA(&CFileMgr::CloseFile),SCloseFile);
                            P.Add(0x536F80,SLoadLine); P.Add(MA(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),SLoadLine);
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="Initialise"){ pedStats=stats.data(); for(auto& b:arena)b=uint8(next()); genLines();
                            Patches P; P.Add(0x538900,SOpenFile); P.Add(MA(&CFileMgr::OpenFile),SOpenFile); P.Add(0x5389D0,SCloseFile); P.Add(MA(&CFileMgr::CloseFile),SCloseFile);
                            P.Add(0x536F80,SLoadLine); P.Add(MA(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),SLoadLine);
                            P.Add(0x5BF400,SLoadDefaultDM); P.Add(MA(&CDecisionMakerTypesFileLoader::LoadDefaultDecisionMaker),SLoadDefaultDM);
                            { EntryPatch n2(0x821195,FakeNewArr); reinterpret_cast<void(__cdecl*)()>(a)(); }
                            out["arena"]=ReversingProbe::Bytes(arena.data(),arena.size()); out["recs"]=Recs(); out["global"]=Off(pedStats,arena.data()); }
                        else { pedStats=stats.data()+0x34*(next()%3);
                            { EntryPatch d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__cdecl*)()>(a)(); }
                            json r=Recs(); for(auto& e:r)e[1]=intptr_t(e[1].get<uint32>())-intptr_t(uintptr_t(stats.data())); out["recs"]=r; out["global"]=Off(pedStats,stats.data()); }
                        out["stats"]=ReversingProbe::Bytes(stats.data(),stats.size()); pedStats=savedPedStats;
                    }
                    else if(cls=="CPlantSurfPropMgr") {
                        auto** ptrTab=reinterpret_cast<uint8**>(PLANTS); auto* tab=plants+0x2C8; auto& count=*reinterpret_cast<uint32*>(PLANTS+0x1E64);
                        for(size_t k=0;k<NPLANTS;++k)plants[k]=uint8(next());
                        auto validTables=[&]{ for(int k=0;k<178;++k)ptrTab[k]=next()%3==0 ? tab+0x7C*(next()%57) : nullptr; count=next()%58; };
                        auto genLines=[&]{ const uint32 total=1+next()%14;
                            for(uint32 k=0;k<total;++k){ const uint32 r=next()%16;
                                if(r==0){ lines.push_back(";comentario "+std::to_string(next()%100)); continue; }
                                if(r==1){ lines.push_back(";the end"); continue; }
                                std::string s=next()%12 ? "SURF_"+std::to_string(1+next()%20) : std::string("BADNAME");
                                uint32 nt=next()%4 ? 18 : uint32(next()%21); if(nt==0)nt=1;
                                static const char* fl[]{"1.5","0.25","-0.75","2","0","0.125","3.5","1"};
                                for(uint32 t=1;t<nt;++t){ s+=next()%3 ? " " : "\t";
                                    if(t==1)s+=std::to_string(next()%5); else if(t<=10)s+=std::to_string(next()%300); else s+=fl[next()%std::size(fl)]; }
                                lines.push_back(s); } };
                        if(nm=="AllocSurfProperties"){ validTables(); count=next()%62; const uint16 id=uint16(next()%178); const bool clear=next()%6==0;
                            auto* r=reinterpret_cast<uint8*(__cdecl*)(uint16,bool)>(a)(id,clear); ret=r ? json(Off(r,tab)) : json(nullptr); }
                        else if(nm=="GetSurfProperties"){ const uint16 id=uint16(next()%3 ? next()%178 : next()%65536); ret=uintptr_t(reinterpret_cast<void*(__cdecl*)(uint16)>(a)(id)); }
                        else { validTables(); genLines();
                            Patches P; P.Add(0x538900,SOpenFile); P.Add(MA(&CFileMgr::OpenFile),SOpenFile); P.Add(0x5389D0,SCloseFile); P.Add(MA(&CFileMgr::CloseFile),SCloseFile);
                            P.Add(0x536F80,SLoadLine); P.Add(MA(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),SLoadLine);
                            P.Add(0x5387D0,SSetDir); P.Add(MA(&CFileMgr::SetDir),SSetDir);
                            P.Add(0x55D220,SSurfaceId); P.Add(MA(&SurfaceInfos_c::GetSurfaceIdFromName),SSurfaceId);
                            if(nm=="Initialise")ret=reinterpret_cast<bool(__cdecl*)()>(a)();
                            else ret=reinterpret_cast<bool(__cdecl*)(const char*)>(a)("PLANTS.DAT"); }
                        json tabOut=json::array(); for(int k=0;k<178;++k){ const auto o=Off(ptrTab[k],tab); tabOut.push_back(o>=0&&o<0x7C*57 ? json(o) : json(uintptr_t(ptrTab[k]))); }
                        out["ptr_tab"]=tabOut; out["tab"]=ReversingProbe::Bytes(tab,0x7C*57); out["count"]=count;
                        std::memcpy(plants,savedPlants.data(),NPLANTS);
                    }
                    else if(cls=="CScriptResourceManager") {
                        alignas(16) static std::array<uint8,0x384+0x10> m{}; for(auto& b:m)b=uint8(next());
                        static const int32 ids[]{-1,1,2,3,4,5,6,300};
                        static const uintptr_t threads[]{0,0x1000,0x2000,0x3000};
                        for(int k=0;k<75;++k){ auto* e=m.data()+k*0xC; *reinterpret_cast<int32*>(e)=ids[next()%std::size(ids)];
                            *reinterpret_cast<uintptr_t*>(e+4)=threads[next()%std::size(threads)]; *reinterpret_cast<uint16*>(e+8)=uint16(next()%3==0 ? 0 : next()%5); }
                        const int32 id=ids[next()%std::size(ids)]; const int32 type=next()%8 ? int32(next()%4) : int32(next()%0x20000); void* const th=reinterpret_cast<void*>(threads[next()%std::size(threads)]);
                        if(nm=="Initialise")reinterpret_cast<void(__thiscall*)(void*)>(a)(m.data());
                        else if(nm=="AddToResourceManager")reinterpret_cast<void(__thiscall*)(void*,int32,int32,void*)>(a)(m.data(),id,type,th);
                        else if(nm=="RemoveFromResourceManager")ret=reinterpret_cast<bool(__thiscall*)(void*,int32,int32,void*)>(a)(m.data(),id,type,th);
                        else ret=reinterpret_cast<bool(__thiscall*)(void*,int32,int32)>(a)(m.data(),id,type);
                        out["m"]=ReversingProbe::Bytes(m.data(),m.size());
                    }
                    else if(cls=="CSpecialPlateHandler") {
                        alignas(16) static std::array<uint8,0xF4+0x1C> h{}; for(auto& b:h)b=uint8(next());
                        static const int32 ids[]{-1,0,1,2,3,4,5,6,7,8,9};
                        auto randText=[&](char* d){ const uint32 len=next()%12; for(uint32 k=0;k<len;++k)d[k]=char('A'+next()%26); d[len]=0; };
                        for(int k=0;k<15;++k){ auto* e=h.data()+k*0x10; *reinterpret_cast<int32*>(e)=ids[next()%std::size(ids)]; randText(reinterpret_cast<char*>(e+4)); }
                        *reinterpret_cast<uint32*>(h.data()+0xF0)=next()%16;
                        const int32 id=ids[next()%std::size(ids)];
                        if(nm=="Init")reinterpret_cast<void(__thiscall*)(void*)>(a)(h.data());
                        else if(nm=="Find"){ std::array<char,16> outText{}; for(auto& c:outText)c=char('a'+next()%26);
                            ret=reinterpret_cast<int32(__thiscall*)(void*,int32,char*)>(a)(h.data(),id,outText.data()); out["text"]=ReversingProbe::Bytes(outText.data(),outText.size()); }
                        else if(nm=="Add"){ std::array<char,16> text{}; for(auto& c:text)c=char('a'+next()%26); randText(text.data());
                            reinterpret_cast<void(__thiscall*)(void*,int32,const char*)>(a)(h.data(),id,text.data()); }
                        else { const int32 idx=int32(next()%17)-2; reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(h.data(),idx); }
                        out["h"]=ReversingProbe::Bytes(h.data(),h.size());
                    }
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                // Antes de ejecutar: si el proceso muere, el ultimo case_start del registro dice que funcion/caso fue (la semilla se recalcula igual al repetirlo).
                Write({{"event","case_start"},{"function",full},{"case",i},{"pair",pair},{"seed",uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xDA23u}});
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
    restoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateOk()}});
}
}
