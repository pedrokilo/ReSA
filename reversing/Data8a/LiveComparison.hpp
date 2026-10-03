#pragma once
// Banco dentro del motor: pistas de cinematica, canal estatico, generador de coches, checkpoints, colision y bandas
// (lista de 8, 47 funciones). CAECutsceneTrackManager, CAEStaticChannel (VMT), CCarGenerator, CCheckpoints,
// CCollisionData y CGangs. Original (hooks del lote desactivados, codigo del EXE) frente a candidato con fixtures
// identicos en buffers propios. Nada toca el hardware de audio ni crea vehiculos:
// - CAEAudioHardware (PlayTrack, StopTrack, StopSound, canales, Service...), CAESmoothFadeThread::RequestFade y
//   CAEAudioChannel (UpdateStatus, posicion, conversiones) se simulan con trazas; los buffers DirectSound son
//   objetos COM falsos con vtable propia.
// - CCarGenerator usa generadores falsos, model infos y modelos de colision falsos colocados temporalmente en
//   huecos nulos de CModelInfo::ms_modelInfoPtrs, entidades falsas, un pool de vehiculos falso (se cambia el
//   puntero 0xB74494) y stubs de FindObjectsKindaColliding, FindPlayer*, camara, oclusion, zonas, streaming,
//   PickRandomCar, IsBoatModel y ProcessVerticalLine (siempre falso: DoInternalProcessing solo se prueba hasta
//   antes de crear el vehiculo). En Process, DoInternalProcessing se sustituye por un stub con traza.
// - CCheckpoints trabaja sobre el array real (0xC7F158) guardado y restaurado; CCheckpoint::Render y
//   RwRenderStateSet (puntero del motor) se simulan.
// - CCollisionData usa objetos y arrays propios; CMemoryMgr::Malloc/Free van a una arena fija;
//   CCollision::RemoveTrianglePlanes y CColTrianglePlane::Set se simulan.
// - CGangs trabaja sobre Gang/GangAttackWithCops reales guardados y restaurados; Save/Load del work buffer,
//   InitSequence y FindSequenceElement se simulan.
// Todos los globales tocados se guardan y se restauran en cada ejecucion y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "AECutsceneTrackManager.h"
#include "AEStaticChannel.h"
#include "AEAudioHardware.h"
#include "AESmoothFadeThread.h"
#include "CarGenerator.h"
#include "Checkpoints.h"
#include "Checkpoint.h"
#include "CollisionData.h"
#include "Collision.h"
#include "ColTrianglePlane.h"
#include "Gangs.h"
#include "GangInfo.h"
#include "GenericGameStorage.h"
#include "CarCtrl.h"
#include "Population.h"
#include "LoadedCarGroup.h"
#include "TheZones.h"
#include "Streaming.h"
#include "ModelInfo.h"
#include "Occlusion.h"
#include "Camera.h"
#include "MemoryMgr.h"
#include "World.h"
#include "../ProbeRuntime.hpp"

namespace Data8aLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
template<class T=uint32> T& At(void* b,size_t o) { return *reinterpret_cast<T*>(static_cast<uint8*>(b)+o); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 U(const void* p) { return uint32(uintptr_t(p)); }

constexpr uintptr_t TIMER=0xB7CB84, PAUSE=0xB7CB48, PREV_PT=0x8AE588, NUM_CH=0xB5F898, HOURS=0xB70153, PARKED=0x9690A0, GENDIST=0xB6F11C;
constexpr uintptr_t GEN_CLOSE=0xC279D0, AREA=0xB72914, VEH_POOL=0xB74494, CPS=0xC7F158, NUM_CPS=0xC7C6D4, GANGS=0xC091E0, ZONE=0xC0FCBC, ENGINE=0xC97B24;
constexpr uintptr_t MI_PTRS=0xA9B0C8, SINFO=0x8E4CC0, HW=0xB5F8B8, FADE=0xB608D0, CH_VT=0x85F3CC;
constexpr size_t NSLOT=8;

// --- estado por caso ---
inline uint32 maskA{}, callsA{}, maskB{}, callsB{};
inline bool NextA() { return (maskA>>(callsA++%32))&1; }
inline bool NextB() { return (maskB>>(callsB++%32))&1; }

// --- CAEAudioHardware ---
inline int32 lengthRet{}, playingRet{}, ptCalls{}; inline std::array<int32,8> ptSeq{};
inline json Self(void* s) { return uintptr_t(s)==HW ? json("hw") : json(U(s)); }
inline int32 __fastcall FakeLength(void* s,void*) { trace.push_back({"track_length",Self(s)}); return lengthRet; }
inline int32 __fastcall FakePlayingId(void* s,void*) { trace.push_back({"playing_id",Self(s)}); return playingRet; }
inline int32 __fastcall FakePlayTime(void* s,void*) { const int32 r=ptCalls<3 ? ptSeq[ptCalls%8] : -2; ++ptCalls; trace.push_back({"track_play_time",Self(s),r}); return r; }
inline void __fastcall FakePlayTrack(void* s,void*,uint32 id,int32 nxt,uint32 off,uint8 fl,uint8 u,uint8 un) { trace.push_back({"play_track",Self(s),id,nxt,off,fl,u,un}); }
inline void __fastcall FakeStopTrack(void* s,void*) { trace.push_back({"stop_track",Self(s)}); }
inline void __fastcall FakeStopSound(void* s,void*,int16 ch,uint16 slot) { trace.push_back({"stop_sound",Self(s),ch,slot}); }
inline void __fastcall FakeBassEq(void* s,void*) { trace.push_back({"disable_bass_eq",Self(s)}); }
inline void __fastcall FakeChFlags(void* s,void*,int16 ch,uint16 id,int16 fl) { trace.push_back({"channel_flags",Self(s),ch,id,fl}); }
inline void __fastcall FakeChVolume(void* s,void*,int16 ch,uint16 id,float v,uint8 x) { trace.push_back({"channel_volume",Self(s),ch,id,F(v),x}); }
inline void __fastcall FakeStartPlayback(void* s,void*) { trace.push_back({"start_track_playback",Self(s)}); }
inline void __fastcall FakeFreqScale(void* s,void*,int16 ch,uint16 id,float f) { trace.push_back({"freq_scale",Self(s),ch,id,F(f)}); }
inline void __fastcall FakeHwService(void* s,void*) { trace.push_back({"hw_service",Self(s)}); }

// --- CAEStaticChannel: canal y buffers DirectSound falsos ---
struct ComObj { void** vt; uint32 id; };
inline std::array<void*,21> comVt{};
inline ComObj dsbMain{comVt.data(),1}, dsb3d{comVt.data(),2};
alignas(16) inline std::array<uint8,0x400> lockBuf{}; alignas(16) inline std::array<uint8,0x200> pcm{};
inline uint32 statusFix{}, posFix{};
inline ULONG __stdcall ComRelease(ComObj* o) { trace.push_back({"release",o->id,*reinterpret_cast<uint32*>(NUM_CH)}); return 0; }
inline HRESULT __stdcall ComLock(ComObj* o,DWORD off,DWORD n,void** p1,DWORD* b1,void** p2,DWORD* b2,DWORD fl) {
    trace.push_back({"lock",o->id,off,n,p2!=nullptr,b2!=nullptr,fl}); *p1=lockBuf.data(); *b1=n; return S_OK; }
inline HRESULT __stdcall ComUnlock(ComObj* o,void* p1,DWORD b1,void* p2,DWORD b2) { trace.push_back({"unlock",o->id,p1==lockBuf.data(),b1,p2!=nullptr,b2}); return S_OK; }
inline HRESULT __stdcall ComPlay(ComObj* o,DWORD r,DWORD pr,DWORD fl) { trace.push_back({"play",o->id,r,pr,fl}); return S_OK; }
inline HRESULT __stdcall ComSetPos(ComObj* o,DWORD p) { trace.push_back({"set_position",o->id,p}); return S_OK; }
inline HRESULT __stdcall ComSetVolume(ComObj* o,LONG v) { trace.push_back({"set_volume",o->id,v}); return S_OK; }
inline HRESULT __stdcall ComStop(ComObj* o) { trace.push_back({"stop",o->id}); return S_OK; }
// CAEAudioChannel::UpdateStatus/GetCurrentPlaybackPosition/Convert* corren de verdad (su hook no cambia entre las dos
// ejecuciones); no se parchean porque el hilo de audio los llama sobre canales reales. El buffer COM falso responde.
inline HRESULT __stdcall ComGetPos(ComObj* o,DWORD* play,DWORD* write) { trace.push_back({"get_current_position",o->id,write!=nullptr,posFix}); if(play)*play=posFix; if(write)*write=0; return S_OK; }
inline HRESULT __stdcall ComGetStatus(ComObj* o,DWORD* st) { trace.push_back({"get_status",o->id,statusFix}); *st=statusFix; return S_OK; }
inline bool __fastcall FakeRequestFade(void* s,void*,ComObj* b,float v,int16 t,uint8 stop) { const bool r=NextA(); trace.push_back({"request_fade",uintptr_t(s)==FADE,b ? int32(b->id) : 0,F(v),t,stop,r}); return r; }

// --- CCarGenerator ---
alignas(16) inline std::array<std::array<uint8,0x60>,NSLOT> infos{};
alignas(16) inline std::array<std::array<uint8,0x30>,NSLOT> cols{};
alignas(16) inline std::array<std::array<uint8,0x40>,NSLOT> ents{};
alignas(16) inline std::array<std::array<uint8,0x40>,NSLOT> mats{};
inline std::array<int32,NSLOT> slots{};
alignas(16) inline std::array<uint8,0x14> fakePool{}; alignas(16) inline std::array<uint8,4*0xA18> vehs{}; inline std::array<uint8,4> vstates{};
inline CVector centre{}, speed{}, coors{}; inline int32 pickRet{}, entCount{}, handleAfter{};
inline json V(const CVector& v) { return {F(v.x),F(v.y),F(v.z)}; }
inline void __cdecl FakeFokc(const CVector& p,float r,uint8 b2D,int16* cnt,int16 maxCount,CEntity** out,uint8 bld,uint8 veh,uint8 ped,uint8 obj,uint8 dum) {
    trace.push_back({"find_objects_kinda_colliding",V(p),F(r),b2D,maxCount,bld,veh,ped,obj,dum,entCount});
    *cnt=int16(entCount); for(int32 k=0;k<entCount;++k)out[k]=reinterpret_cast<CEntity*>(ents[k%NSLOT].data()); }
inline const CVector& __cdecl FakeCentre(int32 id) { trace.push_back({"centre_of_world",id}); return centre; }
inline CVector __cdecl FakeCoors(int32 id) { trace.push_back({"player_coors",id}); return coors; }
inline CVector& __cdecl FakeSpeed(int32 id) { trace.push_back({"player_speed",id}); return speed; }
inline bool __fastcall FakeSphereVisible(void* cam,void*,const CVector& c,float r) { const bool v=NextB(); trace.push_back({"sphere_visible",uintptr_t(cam)==0xB6F028,V(c),F(r),v}); return v; }
inline bool __cdecl FakeOccluded(CVector p,float r) { const bool v=NextB(); trace.push_back({"position_occluded",V(p),F(r),v}); return v; }
inline CZoneInfo* __cdecl FakeZoneInfo(const CVector& p,CZone** z) { trace.push_back({"zone_info",V(p),z!=nullptr}); if(z)*z=nullptr; return nullptr; }
inline int32 __fastcall FakePickCar(void* s,void*,uint8 a,uint8 b) { trace.push_back({"pick_random_car",uintptr_t(s)==0xC0E9F8,a,b,pickRet}); return pickRet; }
inline void __cdecl FakeRequestModel(int32 id,int32 fl) { trace.push_back({"request_model",id,fl}); }
inline bool __cdecl FakeIsBoat(int32 id) { trace.push_back({"is_boat_model",id}); return false; }
inline bool __cdecl FakeVerticalLine(const CVector& o,float d,CColPoint&,CEntity*&,uint8 b,uint8 v,uint8 p,uint8 ob,uint8 du,uint8 see,void* poly) {
    trace.push_back({"process_vertical_line",V(o),F(d),b,v,p,ob,du,see,poly!=nullptr}); return false; }
inline void __fastcall FakeDoInternal(uint8* g,void*) { trace.push_back({"do_internal_processing",ReversingProbe::Bytes(g,0x20)}); At<int16>(g,0x18)=int16(handleAfter); }

// --- CCheckpoints ---
inline RwBool __cdecl FakeRenderState(int32 s,void* v) { trace.push_back({"render_state",s,U(v)}); return TRUE; }
inline void __fastcall FakeCpRender(uint8* cp,void*) { trace.push_back({"checkpoint_render",int32((uintptr_t(cp)-CPS)/0x38),int32((uintptr_t(cp)-CPS)%0x38)}); }

// --- CCollisionData ---
alignas(16) inline std::array<uint8,0x4000> arena{}; inline size_t arenaTop{};
alignas(16) inline std::array<uint8,0x30> col{}, colSrc{};
alignas(16) inline std::array<uint8,8*36> sSph{}, sLin{}, sBox{}, oSph{}, oLin{}, oBox{};
alignas(16) inline std::array<uint8,72*6> sVer{}, sShV{}, oVer{}, oShV{};
alignas(16) inline std::array<uint8,16*8> sTri{}, sShT{}, oTri{}, oShT{};
alignas(16) inline std::array<uint8,0x10> link{};
inline json P(const void* p) {
    const auto u=uintptr_t(p); if(!u)return nullptr;
    if(u>=uintptr_t(arena.data())&&u<uintptr_t(arena.data())+arena.size())return json::array({"arena",u-uintptr_t(arena.data())});
    const std::pair<const uint8*,const char*> bufs[]{{sSph.data(),"sSph"},{sLin.data(),"sLin"},{sBox.data(),"sBox"},{oSph.data(),"oSph"},{oLin.data(),"oLin"},{oBox.data(),"oBox"},
        {sVer.data(),"sVer"},{sShV.data(),"sShV"},{oVer.data(),"oVer"},{oShV.data(),"oShV"},{sTri.data(),"sTri"},{sShT.data(),"sShT"},{oTri.data(),"oTri"},{oShT.data(),"oShT"},{col.data(),"col"},{colSrc.data(),"colSrc"}};
    for(auto& [b,n]:bufs)if(u>=uintptr_t(b)&&u<uintptr_t(b)+0x200)return json::array({n,u-uintptr_t(b)});
    return U(p); }
inline void* __cdecl FakeMalloc(uint32 n,uint32 hint) { const size_t off=arenaTop; const bool ok=n<=arena.size()-off; trace.push_back({"malloc",n,hint,ok ? json(off) : json(nullptr)}); if(!ok)return nullptr; arenaTop=(off+n+15)&~size_t(15); return arena.data()+off; }
inline void __cdecl FakeFree(void* p) { trace.push_back({"free",P(p)}); }
inline void __cdecl FakeColRemovePlanes(CCollisionData* d) { trace.push_back({"collision_remove_triangle_planes",P(d)}); }
inline void __fastcall FakePlaneSet(uint8* plane,void*,const void* verts,const void* tri) {
    trace.push_back({"plane_set",P(plane),P(verts),P(tri)}); for(int k=0;k<10;++k)plane[k]=uint8(k*17+(uintptr_t(tri)&0xFF)); }

// --- CGangs ---
inline void __cdecl FakeInitSequence(int32 n) { trace.push_back({"init_sequence",n}); }
inline std::array<int32,8> seqFix{};
inline int32 __cdecl FakeFindSeq(int32 i) { const int32 r=seqFix[uint32(i)%8]; trace.push_back({"find_sequence_element",i,r}); return r; }
inline bool __cdecl FakeSaveWb(void* d,int32 n) { trace.push_back({"save_work_buffer",int32(uintptr_t(d)-GANGS),n,Fnv(d,size_t(std::clamp(n,0,64)))}); return true; }
inline bool __cdecl FakeLoadWb(void* d,int32 n) { trace.push_back({"load_work_buffer",int32(uintptr_t(d)-GANGS),n}); for(int32 k=0;k<n;++k)static_cast<uint8*>(d)[k]=uint8(k*29+n+(uintptr_t(d)&0xFF)); return true; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,47> fns{{
    {"CAECutsceneTrackManager","Initialise",0x5B9630,0,-1,128},{"CAECutsceneTrackManager","PlayPreloadedCutsceneTrack",0x4DBE70,0,-1,64},
    {"CAECutsceneTrackManager","StopCutsceneTrack",0x4DBE80,0,-1,256},{"CAECutsceneTrackManager","IsCutsceneTrackActive",0x4DBEB0,0,-1,128},
    {"CAECutsceneTrackManager","GetCutsceneTrackStatus",0x4DBEC0,0,-1,256},{"CAECutsceneTrackManager","PauseTrack",0x4DBF00,0,-1,64},
    {"CAECutsceneTrackManager","PreloadBeatTrack",0x4DC130,0,-1,256},{"CAECutsceneTrackManager","Service",0x4DBFB0,0,-1,1024},
    {"CAEStaticChannel","Service",0,0x85F3CC,1,1024},{"CAEStaticChannel","IsSoundPlaying",0,0x85F3CC,2,256},{"CAEStaticChannel","GetPlayTime",0,0x85F3CC,3,256},
    {"CAEStaticChannel","GetLength",0,0x85F3CC,4,128},{"CAEStaticChannel","Play",0,0x85F3CC,5,512},{"CAEStaticChannel","SynchPlayback",0,0x85F3CC,6,512},
    {"CAEStaticChannel","Stop",0,0x85F3CC,7,512},
    {"CCarGenerator","CalcNextGen",0x6F2E40,0,-1,64},{"CCarGenerator","SwitchOff",0x6F2E30,0,-1,64},{"CCarGenerator","SwitchOn",0x6F32C0,0,-1,64},
    {"CCarGenerator","Setup",0x6F2E50,0,-1,512},{"CCarGenerator","CheckForBlockage",0x6F32E0,0,-1,1024},{"CCarGenerator","CheckIfWithinRangeOfAnyPlayers",0x6F2F40,0,-1,1024},
    {"CCarGenerator","DoInternalProcessing",0x6F34D0,0,-1,1024},{"CCarGenerator","Process",0x6F3E90,0,-1,1024},
    {"CCheckpoints","Init",0x722880,0,-1,64},{"CCheckpoints","Shutdown",0x7228F0,0,-1,64},{"CCheckpoints","SetHeading",0x722970,0,-1,512},
    {"CCheckpoints","Update",0x7229C0,0,-1,1024},{"CCheckpoints","PlaceMarker",0x722C40,0,-1,1024},{"CCheckpoints","UpdatePos",0x722900,0,-1,512},
    {"CCheckpoints","DeleteCP",0x722FC0,0,-1,512},{"CCheckpoints","Render",0x726060,0,-1,256},
    {"CCollisionData","RemoveCollisionVolumes",0x40F070,0,-1,256},{"CCollisionData","Copy",0x40F120,0,-1,1024},{"CCollisionData","CalculateTrianglePlanes",0x40F590,0,-1,256},
    {"CCollisionData","RemoveTrianglePlanes",0x40F6A0,0,-1,128},{"CCollisionData","GetTrianglePoint",0x40F5E0,0,-1,256},{"CCollisionData","GetShadTrianglePoint",0x40F640,0,-1,256},
    {"CCollisionData","SetLinkPtr",0x40F6C0,0,-1,256},{"CCollisionData","GetLinkPtr",0x40F6E0,0,-1,256},
    {"CGangs","Initialise",0x5DE680,0,-1,64},{"CGangs","Load",0x5D3A90,0,-1,64},{"CGangs","Save",0x5D3A60,0,-1,64},
    {"CGangs","SetGangWeapons",0x5DE550,0,-1,256},{"CGangs","ChooseGangPedModel",0x5DE5A0,0,-1,512},{"CGangs","GetWillAttackPlayerWithCops",0x5DE500,0,-1,128},
    {"CGangs","SetWillAttackPlayerWithCops",0x5DE4E0,0,-1,128},{"CGangs","SetGangPedModelOverride",0x5DE580,0,-1,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"data8a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"data8a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("data8a");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"data8a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,47> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,47> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // huecos nulos de ms_modelInfoPtrs para los model infos falsos
    auto** miPtrs=reinterpret_cast<void**>(MI_PTRS);
    { size_t n=0; for(int32 id=19999;id>=1000&&n<NSLOT;--id)if(!miPtrs[id])slots[n++]=id;
      if(n<NSLOT){ Write({{"event","error"},{"reason","no free model info slots"}}); for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);} return; } }

    // regiones globales guardadas
    std::vector<std::pair<uintptr_t,size_t>> regions{{TIMER,4},{PAUSE,2},{PREV_PT,4},{NUM_CH,4},{HOURS,1},{PARKED,4},{GENDIST,4},{GEN_CLOSE,1},{AREA,4},{VEH_POOL,4},
        {CPS,32*0x38},{NUM_CPS,4},{GANGS,0xB0},{ZONE,4}};
    for(auto id:slots){ regions.push_back({MI_PTRS+4*uintptr_t(id),4}); regions.push_back({SINFO+20*uintptr_t(id),20}); }
    std::vector<std::vector<uint8>> saved; for(auto& [a,n]:regions)saved.emplace_back(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n);
    auto* engine=*reinterpret_cast<uint8**>(ENGINE); auto& rsSlot=At<void*>(engine,0x20); void* const rsSaved=rsSlot;
    auto restoreGlobals=[&]{ for(size_t k=0;k<regions.size();++k)std::memcpy(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second); rsSlot=rsSaved; };
    auto globalsIntact=[&]{ for(size_t k=0;k<regions.size();++k)if(std::memcmp(reinterpret_cast<void*>(regions[k].first),saved[k].data(),regions[k].second))return false; return rsSlot==rsSaved; };

    comVt.fill(nullptr);
    comVt[2]=reinterpret_cast<void*>(&ComRelease); comVt[4]=reinterpret_cast<void*>(&ComGetPos); comVt[9]=reinterpret_cast<void*>(&ComGetStatus); comVt[11]=reinterpret_cast<void*>(&ComLock); comVt[12]=reinterpret_cast<void*>(&ComPlay);
    comVt[13]=reinterpret_cast<void*>(&ComSetPos); comVt[15]=reinterpret_cast<void*>(&ComSetVolume); comVt[18]=reinterpret_cast<void*>(&ComStop); comVt[19]=reinterpret_cast<void*>(&ComUnlock);

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch g1(bit_cast<uintptr_t>(&CWorld::FindObjectsKindaColliding),FakeFokc), g1e(0x568B80,FakeFokc);
        EntryPatch g2(bit_cast<uintptr_t>(&FindPlayerCentreOfWorld),FakeCentre), g2e(0x56E250,FakeCentre);
        EntryPatch g3(bit_cast<uintptr_t>(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),FakeCoors), g3e(0x56E010,FakeCoors);
        EntryPatch g4(bit_cast<uintptr_t>(static_cast<CVector&(*)(int32)>(&FindPlayerSpeed)),FakeSpeed), g4e(0x56E090,FakeSpeed);
        EntryPatch g5(bit_cast<uintptr_t>(static_cast<bool(CCamera::*)(const CVector&,float)>(&CCamera::IsSphereVisible)),FakeSphereVisible), g5e(0x420D40,FakeSphereVisible);
        EntryPatch g6(bit_cast<uintptr_t>(&COcclusion::IsPositionOccluded),FakeOccluded), g6e(0x7200B0,FakeOccluded);
        EntryPatch g7(bit_cast<uintptr_t>(static_cast<CZoneInfo*(*)(const CVector&,CZone**)>(&CTheZones::GetZoneInfo)),FakeZoneInfo), g7e(0x572400,FakeZoneInfo);
        EntryPatch g8(bit_cast<uintptr_t>(&CLoadedCarGroup::PickRandomCar),FakePickCar), g8e(0x611C50,FakePickCar);
        EntryPatch g9(bit_cast<uintptr_t>(&CStreaming::RequestModel),FakeRequestModel), g9e(0x4087E0,FakeRequestModel);
        EntryPatch g10(bit_cast<uintptr_t>(&CModelInfo::IsBoatModel),FakeIsBoat), g10e(0x4C5A70,FakeIsBoat);
        EntryPatch g11(bit_cast<uintptr_t>(&CWorld::ProcessVerticalLine),FakeVerticalLine), g11e(0x5674E0,FakeVerticalLine);
        EntryPatch k1(bit_cast<uintptr_t>(&CCheckpoint::Render),FakeCpRender), k1e(0x725C00,FakeCpRender);
        EntryPatch s1(bit_cast<uintptr_t>(&CCarCtrl::InitSequence),FakeInitSequence), s1e(0x421740,FakeInitSequence);
        EntryPatch s2(bit_cast<uintptr_t>(&CCarCtrl::FindSequenceElement),FakeFindSeq), s2e(0x421770,FakeFindSeq);
        EntryPatch s3(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),FakeSaveWb), s3e(0x5D1270,FakeSaveWb);
        EntryPatch s4(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),FakeLoadWb), s4e(0x5D1300,FakeLoadWb);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x8Au; if(!stt)stt=1; trace=json::array();
                    maskA=next(); maskB=next(); callsA=callsB=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto& timer=*reinterpret_cast<uint32*>(TIMER);
                    timer=next()%4 ? next()%5000000 : next();

                    if(cls=="CAECutsceneTrackManager") {
                        // stubs del hardware de audio solo durante el caso (el hilo de streaming nunca los ve)
                        EntryPatch h1(bit_cast<uintptr_t>(&CAEAudioHardware::GetTrackLengthMs),FakeLength), h1e(0x4D8F70,FakeLength);
                        EntryPatch h2(bit_cast<uintptr_t>(&CAEAudioHardware::GetPlayingTrackID),FakePlayingId), h2e(0x4D8F90,FakePlayingId);
                        EntryPatch h3(bit_cast<uintptr_t>(&CAEAudioHardware::GetTrackPlayTime),FakePlayTime), h3e(0x4D8F60,FakePlayTime);
                        EntryPatch h4(bit_cast<uintptr_t>(&CAEAudioHardware::PlayTrack),FakePlayTrack), h4e(0x4D8F10,FakePlayTrack);
                        EntryPatch h5(bit_cast<uintptr_t>(&CAEAudioHardware::StopTrack),FakeStopTrack), h5e(0x4D8F50,FakeStopTrack);
                        EntryPatch h6(bit_cast<uintptr_t>(&CAEAudioHardware::StopSound),FakeStopSound), h6e(0x4D88E0,FakeStopSound);
                        EntryPatch h7(bit_cast<uintptr_t>(&CAEAudioHardware::DisableBassEq),FakeBassEq), h7e(0x4D94D0,FakeBassEq);
                        EntryPatch h8(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelFlags),FakeChFlags), h8e(0x4D9500,FakeChFlags);
                        EntryPatch h9(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelVolume),FakeChVolume), h9e(0x4D8870,FakeChVolume);
                        EntryPatch h10(bit_cast<uintptr_t>(&CAEAudioHardware::StartTrackPlayback),FakeStartPlayback), h10e(0x4D8F30,FakeStartPlayback);
                        EntryPatch h11(bit_cast<uintptr_t>(&CAEAudioHardware::SetChannelFrequencyScalingFactor),FakeFreqScale), h11e(0x4D8960,FakeFreqScale);
                        EntryPatch h12(bit_cast<uintptr_t>(&CAEAudioHardware::Service),FakeHwService), h12e(0x4D9870,FakeHwService);
                        alignas(16) std::array<uint8,0x30> trk{}; for(auto& b:trk)b=uint8(next());
                        At<int32>(trk.data(),8)=next()%8 ? int32(next()%9) : int32(next()%14)-2;
                        trk[0]=uint8(next()%2); trk[1]=uint8(next()%2); trk[0x1C]=uint8(next()%2);
                        At<int32>(trk.data(),4)=next()%3 ? int32(next()%8) : int32(next());
                        lengthRet=int32(next()); playingRet=next()%3 ? int32(next()%200) : -1; ptCalls=0;
                        static const int32 pts[]{-2,-6,-7,-1,0,1234,-3,77777};
                        for(auto& p:ptSeq)p=pts[next()%8];
                        reinterpret_cast<uint8*>(PAUSE)[0]=uint8(next()%4==0); reinterpret_cast<uint8*>(PAUSE)[1]=uint8(next()%4==0);
                        *reinterpret_cast<int32*>(PREV_PT)=int32(next());
                        if(nm=="Initialise")ret=reinterpret_cast<uint8(__thiscall*)(void*,int32)>(a)(trk.data(),next()%2 ? int32(next()%8) : int32(next()));
                        else if(nm=="PlayPreloadedCutsceneTrack"||nm=="StopCutsceneTrack")reinterpret_cast<void(__thiscall*)(void*)>(a)(trk.data());
                        else if(nm=="IsCutsceneTrackActive"||nm=="GetCutsceneTrackStatus")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(trk.data());
                        else if(nm=="PauseTrack")reinterpret_cast<void(__thiscall*)(void*,bool)>(a)(trk.data(),next()%2!=0);
                        else if(nm=="PreloadBeatTrack")reinterpret_cast<void(__thiscall*)(void*,int16,bool)>(a)(trk.data(),int16(next()%14),next()%2!=0);
                        else reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(trk.data(),pts[next()%8]);
                        out["obj"]=ReversingProbe::Bytes(trk.data(),trk.size()); out["prev"]=*reinterpret_cast<int32*>(PREV_PT);
                    }
                    else if(cls=="CAEStaticChannel") {
                        EntryPatch c5(bit_cast<uintptr_t>(&CAESmoothFadeThread::RequestFade),FakeRequestFade), c5e(0x4EEAA0,FakeRequestFade);
                        alignas(16) static std::array<uint8,0x90> ch{}; for(auto& b:ch)b=uint8(next());
                        At<uintptr_t>(ch.data(),0)=CH_VT;
                        At<void*>(ch.data(),8)=next()%6 ? &dsbMain : nullptr; At<void*>(ch.data(),0xC)=next()%2 ? &dsb3d : nullptr;
                        At<float>(ch.data(),0x34)=uni(-100.f,0.f); ch[0x38]=uint8(next()%3==0); ch[0x44]=uint8(next()%2);
                        At<uint32>(ch.data(),0x5C)=next()%4 ? next()%4 : next();
                        ch[0x60]=uint8(next()%2); ch[0x61]=uint8(next()%2); ch[0x62]=uint8(next()%2);
                        At<int32>(ch.data(),0x64)=next()%3 ? 0 : int32(next()%256);
                        const uint32 sync=next(); At<uint32>(ch.data(),0x70)=sync;
                        At<int32>(ch.data(),0x74)=next()%4 ? int32(next()%2000) : int32(next());
                        At<int32>(ch.data(),0x78)=int32(next()%512); At<int32>(ch.data(),0x7C)=int32(next()%64);
                        At<uint16>(ch.data(),0x80)=uint16(next()%9); At<void*>(ch.data(),0x84)=pcm.data();
                        timer=next()%3 ? sync+next()%3000 : next();
                        statusFix=(next()%4 ? next()%4 : next())&~2u; posFix=next()%3 ? next()%200000 : next(); /* sin DSBSTATUS_BUFFERLOST: Lost() no se prueba */
                        { static const uint32 rates[]{22050,44100,32000,11025,48000}; At<uint16>(ch.data(),0x49)=uint16(1+next()%2); At<uint32>(ch.data(),0x4B)=rates[next()%5]; }
                        for(auto& b:pcm)b=uint8(next()); lockBuf.fill(0xCD);
                        *reinterpret_cast<uint32*>(NUM_CH)=5+next()%10;
                        if(nm=="Service")reinterpret_cast<void(__thiscall*)(void*)>(a)(ch.data());
                        else if(nm=="IsSoundPlaying")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(ch.data());
                        else if(nm=="GetPlayTime")ret=reinterpret_cast<int16(__thiscall*)(void*)>(a)(ch.data());
                        else if(nm=="GetLength")ret=reinterpret_cast<uint16(__thiscall*)(void*)>(a)(ch.data());
                        else if(nm=="Play"){ static const float scales[]{0.f,1.f,-0.f,0.5f,std::bit_cast<float>(0x7FC00000u),2.f};
                            if(!At<void*>(ch.data(),8))At<void*>(ch.data(),8)=&dsbMain; // el EXE no comprueba el buffer
                            reinterpret_cast<void(__thiscall*)(void*,int16,int8,float)>(a)(ch.data(),next()%3 ? int16(next()) : int16(0),int8(next()),scales[next()%6]); }
                        else if(nm=="SynchPlayback")reinterpret_cast<void(__thiscall*)(void*)>(a)(ch.data());
                        else reinterpret_cast<void(__thiscall*)(void*)>(a)(ch.data());
                        out["obj"]=ReversingProbe::Bytes(ch.data(),ch.size()); out["lock"]=Fnv(lockBuf.data(),lockBuf.size()); out["channels"]=*reinterpret_cast<uint32*>(NUM_CH);
                    }
                    else if(cls=="CCarGenerator") {
                        // model infos y colisiones falsos en huecos nulos
                        for(size_t k=0;k<NSLOT;++k){
                            auto* mi=infos[k].data(); for(int q=0;q<0x60;++q)mi[q]=uint8(next());
                            auto* cm=cols[k].data(); At<float>(cm,0)=uni(-3.f,0.f); At<float>(cm,4)=uni(-6.f,0.f); At<float>(cm,8)=uni(-3.f,0.f);
                            At<float>(cm,0xC)=uni(0.f,3.f); At<float>(cm,0x10)=uni(0.f,6.f); At<float>(cm,0x14)=uni(0.f,3.f); At<float>(cm,0x24)=uni(0.5f,6.f);
                            At<void*>(mi,0x14)=k<4||next()%5 ? cm : nullptr; /* las entidades usan los huecos 0..3: el EXE no comprueba su colision */ At<int32>(mi,0x3C)=next()%3 ? 5 : int32(next()%11); mi[0x4D]=uint8(next()%3 ? 5 : next()%12);
                            miPtrs[slots[k]]=mi; reinterpret_cast<uint8*>(SINFO+20*uintptr_t(slots[k]))[0x10]=uint8(next()%3 ? 1 : next()%4);
                            auto* e=ents[k].data(); for(int q=0;q<0x40;++q)e[q]=uint8(next());
                            auto* m=mats[k].data(); At<float>(m,0x38)=uni(-20.f,60.f);
                            At<float>(e,0xC)=uni(-20.f,60.f); At<void*>(e,0x14)=next()%2 ? m : nullptr; At<int16>(e,0x22)=int16(slots[next()%4]);
                        }
                        alignas(16) std::array<uint8,0x20> gen{}; for(auto& b:gen)b=uint8(next());
                        auto pickSlot=[&]{ return slots[next()%NSLOT]; };
                        const uint32 mk=next()%10; int32 model=mk<4 ? pickSlot() : mk<7 ? -pickSlot() : mk==7 ? -1 : mk==8 ? 0 : (next()%2 ? 588 : pickSlot());
                        if(model==0&&nm!="CheckIfWithinRangeOfAnyPlayers"&&nm!="Process")model=pickSlot(); // el modelo 0 real no se toca
                        At<int16>(gen.data(),0)=int16(model);
                        for(int q=0;q<3;++q)At<int16>(gen.data(),4+2*q)=int16(int32(next()%8000)-4000);
                        At<int16>(gen.data(),0x18)=-1; At<uint16>(gen.data(),0x1A)=uint16(next()%3==0 ? 0 : next()%3==0 ? 0xFFFF : next()%5);
                        const CVector gp{float(At<int16>(gen.data(),4))*0.125f,float(At<int16>(gen.data(),6))*0.125f,float(At<int16>(gen.data(),8))*0.125f};
                        centre=CVector{gp.x+uni(-300.f,300.f),gp.y+uni(-300.f,300.f),gp.z+uni(-70.f,70.f)};
                        if(next()%32==0)centre.x=std::bit_cast<float>(0x7FC00000u);
                        if(next()%8==0)At<int16>(gen.data(),8)=int16(next()%2 ? 7600+next()%400 : 7000+next()%1000); // cerca de z = 950
                        speed=CVector{uni(-30.f,30.f),uni(-30.f,30.f),uni(-5.f,5.f)}; if(next()%32==0)speed.y=std::bit_cast<float>(0x7FC00000u);
                        coors=CVector{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(0.f,100.f)};
                        *reinterpret_cast<float*>(GENDIST)=uni(0.5f,2.f); *reinterpret_cast<uint8*>(GEN_CLOSE)=uint8(next()%4==0);
                        *reinterpret_cast<int32*>(AREA)=next()%3==0 ? int32(next()%18) : 0;
                        *reinterpret_cast<uint8*>(HOURS)=uint8(next()%24); *reinterpret_cast<int32*>(PARKED)=next()%8==0 ? -int32(next()%3)-1 : int32(next()%15);
                        entCount=int32(next()%9); pickRet=next()%5==0 ? -1 : pickSlot();
                        if(nm=="CalcNextGen")ret=reinterpret_cast<uint32(__cdecl*)()>(a)();
                        else if(nm=="SwitchOff"||nm=="SwitchOn")reinterpret_cast<void(__thiscall*)(void*)>(a)(gen.data());
                        else if(nm=="Setup"){ const CVector p{uni(-4000.f,4000.f),uni(-4000.f,4000.f),uni(-50.f,1100.f)};
                            reinterpret_cast<void(__thiscall*)(void*,CVector,float,int32,int16,int16,uint8,uint8,uint8,uint16,uint16,uint8,bool)>(a)(gen.data(),p,uni(-720.f,720.f),
                                int32(next()%3 ? next()%800 : next()),int16(next()%3 ? next()%128 : -1),int16(next()%3 ? next()%128 : -1),uint8(next()),uint8(next()%101),uint8(next()%101),
                                uint16(next()),uint16(next()),uint8(next()),next()%2!=0); }
                        else if(nm=="CheckForBlockage")ret=reinterpret_cast<uint8(__thiscall*)(void*,int32)>(a)(gen.data(),next()%16==0 ? 588 : pickSlot());
                        else if(nm=="CheckIfWithinRangeOfAnyPlayers")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(gen.data());
                        else if(nm=="DoInternalProcessing")reinterpret_cast<void(__thiscall*)(void*)>(a)(gen.data());
                        else { // Process
                            for(auto& b:vehs)b=uint8(next()); for(auto& s:vstates)s=uint8(next());
                            At<void*>(fakePool.data(),0)=vehs.data(); At<void*>(fakePool.data(),4)=vstates.data(); At<int32>(fakePool.data(),8)=4; At<int32>(fakePool.data(),0xC)=0; fakePool[0x10]=0;
                            *reinterpret_cast<void**>(VEH_POOL)=fakePool.data();
                            for(int q=0;q<4;++q){ auto& st=vehs[q*0xA18+0x36]; if(next()%2)st&=7; }
                            auto ref=[&]{ const uint32 idx=next()%4; return int32((idx<<8)|(next()%4 ? vstates[idx] : uint8(next()))); };
                            At<int16>(gen.data(),0x18)=next()%2 ? int16(-1) : int16(ref()); handleAfter=next()%3 ? -1 : ref();
                            At<uint32>(gen.data(),0x14)=next()%3 ? timer+next()%10-5 : next();
                            { EntryPatch d1(bit_cast<uintptr_t>(&CCarGenerator::DoInternalProcessing),FakeDoInternal), d1e(0x6F34D0,FakeDoInternal);
                              reinterpret_cast<void(__thiscall*)(void*)>(a)(gen.data()); }
                            out["vehs"]=Fnv(vehs.data(),vehs.size());
                        }
                        out["obj"]=ReversingProbe::Bytes(gen.data(),gen.size());
                    }
                    else if(cls=="CCheckpoints") {
                        auto* cps=reinterpret_cast<uint8*>(CPS);
                        static const uint16 types[]{0,1,2,3,4,5,6,7,8,0x101,9,7,8,6,5};
                        for(int k=0;k<32;++k){ auto* cp=cps+k*0x38; for(int q=0;q<0x38;++q)cp[q]=uint8(next());
                            At<uint16>(cp,0)=types[next()%15]; cp[2]=uint8(next()%2); cp[3]=uint8(next()%2); At<int32>(cp,4)=int32(next()%40);
                            for(int q=0;q<3;++q)At<float>(cp,0x10+4*q)=uni(-50.f,50.f);
                            for(int q=0;q<3;++q)At<float>(cp,0x1C+4*q)=uni(-1.f,1.f);
                            At<float>(cp,0x28)=uni(0.f,1.f); At<float>(cp,0x2C)=uni(0.5f,8.f); At<float>(cp,0x30)=uni(0.f,300.f); At<float>(cp,0x34)=uni(-1.f,1.f); }
                        auto* nearCp=cps+(next()%32)*0x38;
                        coors=centre=CVector{At<float>(nearCp,0x10)+uni(-6.f,6.f),At<float>(nearCp,0x14)+uni(-6.f,6.f),At<float>(nearCp,0x18)+uni(-2.f,2.f)};
                        if(next()%32==0)coors.x=std::bit_cast<float>(0x7FC00000u);
                        if(nm=="Init")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Shutdown")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="SetHeading")reinterpret_cast<void(__cdecl*)(uint32,float)>(a)(next()%40,uni(-720.f,720.f));
                        else if(nm=="Update")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="PlaceMarker"){ static const uint16 ptypes[]{3,5,7,0,1,2,4,6,8,3,5,7};
                            const CVector pos=next()%2 ? CVector{coors.x+uni(-3.f,3.f),coors.y+uni(-3.f,3.f),coors.z} : CVector{uni(-60.f,60.f),uni(-60.f,60.f),uni(-5.f,5.f)};
                            const CVector dir{uni(-60.f,60.f),uni(-60.f,60.f),uni(-5.f,5.f)};
                            if(next()%3==0)for(int k=0;k<32;++k)if(At<uint16>(cps+k*0x38,0)==0x101)At<uint16>(cps+k*0x38,0)=uint16(next()%9); // sin huecos libres
                            auto* r=reinterpret_cast<uint8*(__cdecl*)(uint32,uint16,const CVector*,const CVector*,float,uint8,uint8,uint8,uint8,uint16,float,int16)>(a)(
                                next()%40,ptypes[next()%12],&pos,&dir,uni(0.5f,8.f),uint8(next()),uint8(next()),uint8(next()),uint8(next()),uint16(next()),uni(0.f,1.f),int16(next()));
                            ret=r ? json(int32((uintptr_t(r)-CPS)/0x38)) : json(nullptr); }
                        else if(nm=="UpdatePos"){ const CVector p{uni(-60.f,60.f),uni(-60.f,60.f),uni(-5.f,5.f)}; reinterpret_cast<void(__cdecl*)(uint32,const CVector*)>(a)(next()%40,&p); }
                        else if(nm=="DeleteCP"){ for(int k=0;k<4;++k)reinterpret_cast<void(__cdecl*)(uint32,uint16)>(a)(next()%40,uint16(types[next()%15])); }
                        else { rsSlot=reinterpret_cast<void*>(&FakeRenderState); reinterpret_cast<void(__cdecl*)()>(a)(); rsSlot=rsSaved; }
                        out["cps"]=ReversingProbe::Bytes(cps,32*0x38); out["num"]=*reinterpret_cast<uint32*>(NUM_CPS);
                    }
                    else if(cls=="CCollisionData") {
                        // CMemoryMgr solo durante el caso
                        EntryPatch m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc), m1e(0x72F420,FakeMalloc);
                        EntryPatch m2(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), m2e(0x72F430,FakeFree);
                        EntryPatch m3(bit_cast<uintptr_t>(static_cast<void(*)(CCollisionData*)>(&CCollision::RemoveTrianglePlanes)),FakeColRemovePlanes), m3e(0x416400,FakeColRemovePlanes);
                        EntryPatch m4(bit_cast<uintptr_t>(&CColTrianglePlane::Set),FakePlaneSet), m4e(0x411660,FakePlaneSet);
                        arena.fill(0xCD); arenaTop=0;
                        for(auto* b:{&sSph,&sLin,&sBox,&oSph,&oLin,&oBox})for(auto& x:*b)x=uint8(next());
                        for(auto* b:{&sVer,&sShV,&oVer,&oShV})for(auto& x:*b)x=uint8(next());
                        for(auto* b:{&sTri,&sShT,&oTri,&oShT})for(auto& x:*b)x=uint8(next());
                        for(auto& x:col)x=uint8(next()); for(auto& x:colSrc)x=uint8(next());
                        auto* d=col.data(); auto* s=colSrc.data();
                        auto fillTris=[&](std::array<uint8,16*8>& t,uint32 n){ const bool zero=next()%8==0; for(uint32 k=0;k<n;++k)for(int q=0;q<3;++q)At<uint16>(t.data(),k*8+2*q)=zero ? 0 : uint16(next()%70); };
                        if(nm=="Copy") {
                            At<uint16>(s,0)=uint16(next()%3==0 ? 0 : next()%7); s[6]=uint8(next()%3==0 ? 0 : next()%7); At<uint16>(s,2)=uint16(next()%3==0 ? 0 : next()%7);
                            At<uint16>(s,4)=uint16(next()%3==0 ? 0 : next()%13); At<uint32>(s,0x20)=next()%3==0 ? 0 : next()%13; s[7]=uint8(next());
                            At<void*>(s,8)=sSph.data(); At<void*>(s,0x10)=sLin.data(); At<void*>(s,0xC)=sBox.data(); At<void*>(s,0x14)=sVer.data(); At<void*>(s,0x18)=sTri.data();
                            At<void*>(s,0x28)=sShV.data(); At<void*>(s,0x2C)=sShT.data(); fillTris(sTri,At<uint16>(s,4)); fillTris(sShT,At<uint32>(s,0x20));
                            auto same=[&](auto v,auto other){ return next()%2 ? decltype(v)(other) : v; };
                            At<uint16>(d,0)=same(uint16(next()%7),At<uint16>(s,0)); d[6]=same(uint8(next()%7),s[6]); At<uint16>(d,2)=same(uint16(next()%7),At<uint16>(s,2));
                            At<uint16>(d,4)=same(uint16(next()%13),At<uint16>(s,4)); At<uint32>(d,0x20)=same(uint32(next()%13),At<uint32>(s,0x20)); d[7]=uint8(next()%2 ? s[7] : next());
                            auto ptr=[&](uint32 n,void* o){ return n||next()%2 ? o : nullptr; };
                            At<void*>(d,8)=ptr(At<uint16>(d,0),oSph.data()); At<void*>(d,0x10)=ptr(d[6],oLin.data()); At<void*>(d,0xC)=ptr(At<uint16>(d,2),oBox.data());
                            At<void*>(d,0x14)=ptr(At<uint16>(d,4),oVer.data()); At<void*>(d,0x18)=ptr(At<uint16>(d,4),oTri.data());
                            At<void*>(d,0x28)=ptr(At<uint32>(d,0x20),oShV.data()); At<void*>(d,0x2C)=ptr(At<uint32>(d,0x20),oShT.data());
                            auto* r=reinterpret_cast<uint8*(__thiscall*)(void*,const void*)>(a)(d,s); ret=P(r);
                            out["old"]=Fnv(oSph.data(),oSph.size(),Fnv(oLin.data(),oLin.size(),Fnv(oBox.data(),oBox.size(),Fnv(oTri.data(),oTri.size(),Fnv(oShT.data(),oShT.size())))));
                        }
                        else if(nm=="RemoveCollisionVolumes"){ for(size_t q:{8u,0xCu,0x10u,0x14u,0x18u,0x28u,0x2Cu})if(next()%3==0)At<void*>(d,q)=nullptr; else At<void*>(d,q)=oBox.data()+q;
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(d); }
                        else if(nm=="CalculateTrianglePlanes"){ At<uint16>(d,4)=next()%16==0 ? uint16(0xFFF0u+next()%16) : uint16(next()%13); At<void*>(d,0x14)=sVer.data(); At<void*>(d,0x18)=sTri.data();
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(d); }
                        else if(nm=="RemoveTrianglePlanes"){ At<void*>(d,0x1C)=next()%3==0 ? nullptr : arena.data()+16*(next()%8); reinterpret_cast<void(__thiscall*)(void*)>(a)(d); }
                        else if(nm=="GetTrianglePoint"||nm=="GetShadTrianglePoint"){ At<void*>(d,0x14)=sVer.data(); At<void*>(d,0x28)=sShV.data(); CVector o; std::memset(&o,0xCD,sizeof(o));
                            reinterpret_cast<void(__thiscall*)(void*,CVector*,int32)>(a)(d,&o,int32(next()%72)); out["out"]=V(o); }
                        else if(nm=="SetLinkPtr"){ At<uint16>(d,4)=uint16(next()%40); At<void*>(d,0x1C)=arena.data()+next()%4;
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(d,reinterpret_cast<void*>(uintptr_t(next()))); }
                        else { At<uint16>(d,4)=uint16(next()%40); auto* planes=arena.data()+next()%4; At<void*>(d,0x1C)=planes;
                            auto* slot=reinterpret_cast<uint8*>((uintptr_t(planes)+10*At<uint16>(d,4)+3)&~uintptr_t(3));
                            At<void*>(link.data(),0)=d; At<void*>(slot,0)=link.data();
                            ret=P(reinterpret_cast<void*(__thiscall*)(void*)>(a)(d)); }
                        json obj=json::array(); for(size_t q=0;q<0x30;q+=4){ const bool isPtr=q>=8&&q!=0x20&&q!=0x24; obj.push_back(isPtr ? P(At<void*>(d,q)) : json(At<uint32>(d,q))); }
                        out["obj"]=obj; out["arena"]=Fnv(arena.data(),arena.size()); out["top"]=arenaTop;
                    }
                    else if(cls=="CGangs") {
                        auto* g=reinterpret_cast<uint8*>(GANGS); for(int q=0;q<0xB0;++q)g[q]=uint8(next());
                        for(int k=0;k<10;++k){ g[0x10+16*k]=uint8(next()%3==0 ? next() : 0xFF); g[k]=uint8(next()%2); }
                        *reinterpret_cast<uint32*>(ZONE)=next()%3; for(auto& s:seqFix)s=int32(next()%21);
                        if(nm=="Initialise")reinterpret_cast<void(__cdecl*)()>(a)();
                        else if(nm=="Load"||nm=="Save")ret=reinterpret_cast<uint8(__cdecl*)()>(a)();
                        else if(nm=="SetGangWeapons"){ auto w=[&]{ return next()%5==0 ? -int32(next()%3)-1 : int32(next()%47); };
                            const int32 w1=w(),w2=w(),w3=w(); reinterpret_cast<void(__cdecl*)(int16,int32,int32,int32)>(a)(int16(next()%10),w1,w2,w3); }
                        else if(nm=="ChooseGangPedModel")ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(int32(next()%10)|(next()%4==0 ? int32(next()&0xFFFF0000u) : 0));
                        else if(nm=="GetWillAttackPlayerWithCops")ret=reinterpret_cast<uint8(__cdecl*)(int32)>(a)(int32(next()%22)-2);
                        else if(nm=="SetWillAttackPlayerWithCops")reinterpret_cast<void(__cdecl*)(int32,bool)>(a)(int32(next()%22)-2,next()%2!=0);
                        else reinterpret_cast<void(__cdecl*)(int16,int8)>(a)(int16(next()%10),int8(next()));
                        out["gangs"]=ReversingProbe::Bytes(g,0xB0);
                    }
                    out["ret"]=ret; out["trace"]=trace; restoreGlobals(); return out;
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
    restoreGlobals();
    const bool restored=globalsIntact();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
