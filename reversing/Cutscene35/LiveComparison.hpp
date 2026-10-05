#pragma once
// Banco dentro del motor: CCutsceneMgr (35 funciones). Original (hooks de la clase desactivados) frente a
// candidato con fixtures deterministas. Nunca se cargan cinemáticas reales ni se crean objetos, peds o vehículos:
// se simulan y trazan (registro fijo sin reservas) FindPlayerPed/Coors, CClothes, CStreaming, CTimer, CTimeCycle,
// referencias de entidades, Fx (manager, sistemas y matrices), CMessages/CText, CRubbish, CWorld, CAnimManager,
// el grupo de asociaciones, RpAnimBlend/RwHelper/RW (jerarquías, skin, frames, streams), CGame, CIplStore, la cámara,
// el audio, CScriptResourceManager, CPlayerInfo, CHud, CPlantMgr, CModelInfo::GetModelInfo(nombre), las armas del ped,
// CWanted, CColStore, CRadar, CPad::Clear, CDirectory::ReadDirFile, CFileMgr, SetColModel, CObject::operator new y el
// constructor de CCutsceneObject (EXE + DLL), y operator new/delete (solo durante la llamada). Los objetos de cinemática,
// clumps, asociaciones, jerarquías, frames, atomics, entidades, sistemas de partículas, model infos (300-319) y el
// directorio CUTS.IMG son falsos; el .CUT se genera en memoria. Todo global tocado se guarda y restaura en cada ejecución.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CutsceneMgr.h"
#include "CutsceneObject.h"
#include "Clothes.h"
#include "Rubbish.h"
#include "PlantMgr.h"
#include "Collision/TempColModels.h"
#include "Fx.h"
#include "FxManager.h"
#include "FxSystem.h"
#include "Animation/AnimBlendAssocGroup.h"
#include "Animation/AnimBlendAssociation.h"
#include "Animation/AnimManager.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "RwHelper.h"
#include "Collision/ColStore.h"
#include "Radar.h"
#include "IplStore.h"
#include "Directory.h"
#include "FileMgr.h"
#include "ScriptResourceManager.h"
#include "AEPedSpeechAudioEntity.h"
#include "AudioEngine.h"
#include "WeaponInfo.h"
#include "Wanted.h"
#include "Hud.h"
#include "Messages.h"
#include "Text/Text.h"
#include "PlayerInfo.h"
#include "TimeCycle.h"
#include "ClumpModelInfo.h"
#include <platform/win/WinPlatform.h>
#include "../ProbeRuntime.hpp"

namespace Cutscene35Live {
using ReversingProbe::EntryPatch; using std::bit_cast;
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return bit_cast<uint32>(f); }
inline uint32 P(const void* p) { return uint32(uintptr_t(p)); }
inline uint32 H(const char* s) { if(!s) return 0xFFFFFFFFu; uint32 h=2166136261u; for(;*s;++s){h^=uint8(*s);h*=16777619u;} return h; }
inline uint64 Fnv(const void* p,size_t n) { uint64 h=1469598103934665603ull; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T> inline T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- registro fijo sin reservas (operator new puede estar sustituido durante la llamada) ---
struct Rec { const char* tag; uint32 n; uint32 v[8]; };
inline std::array<Rec,4096> recs{}; inline size_t nrec{}; inline bool recOverflow{};
inline void Push(const char* tag,std::initializer_list<uint32> vals={}) {
    if(nrec>=recs.size()){recOverflow=true;return;}
    Rec& r=recs[nrec++]; r.tag=tag; r.n=0; for(uint32 x:vals) if(r.n<8) r.v[r.n++]=x;
}
inline json Recs() {
    json a=json::array();
    for(size_t i=0;i<nrec;++i){ json e=json::array({recs[i].tag}); for(uint32 k=0;k<recs[i].n;++k) e.push_back(recs[i].v[k]); a.push_back(e); }
    if(recOverflow) a.push_back("overflow");
    return a;
}

// --- objetos falsos ---
template<size_t N,size_t S> using Bufs=std::array<std::array<uint8,S>,N>;
alignas(16) inline std::array<uint8,0x800> ped{};
alignas(16) inline std::array<uint8,0x40> plData{}, wanted{}, otherMi{}, dirObj{}, stream{};
alignas(16) inline Bufs<8,0x1A0> objs{}, newObjs{};
alignas(16) inline Bufs<8,0x48> objMats{};
alignas(16) inline Bufs<16,0x100> clumps{};
alignas(16) inline Bufs<16,0x20> clumpData{};
alignas(16) inline Bufs<16,0x40> frameData{};
alignas(16) inline Bufs<16,0xA0> frames{};
alignas(16) inline Bufs<8,0x80> atomics{};
alignas(16) inline Bufs<16,0x40> assocs{};
alignas(16) inline Bufs<8,0x40> newAssocs{}, fxs{}, newFx{};
alignas(16) inline Bufs<8,0x20> statics{}, hiers{}, keys{}, dirEntries{};
alignas(16) inline Bufs<8,0x10> seqs{};
alignas(16) inline Bufs<24,0x40> ents{};
alignas(16) inline Bufs<20,0x40> mis{};
alignas(16) inline Bufs<8,0x70> weaponInfos{};
alignas(16) inline Bufs<32,0x40> matArray{};
alignas(16) inline Bufs<4,16> gxt{};
alignas(16) inline std::array<char,4096> fileText{}; inline uint32 fileLen{};
alignas(16) inline std::array<uint8,0x20000> arena{}; inline uint32 arenaUsed{};
inline std::array<void*,32> vt{};

// --- parámetros de los stubs (fijados por el fixture) ---
struct Cfg {
    uint32 salt, animMask, miMask, resMask, atomicMask, frameMask, removeVeh, finishTime, dirLoadCount;
    float spline; int32 fadeStatus; float coors[3];
    std::array<uint8,16> clumpAtoms; std::array<std::array<uint8,4>,16> clumpAtomList; std::array<int8,16> clumpAssoc;
};
inline Cfg cfg{};
inline uint32 nNewObj{}, nNewAssoc{}, nNewFx{};
inline int ClumpIndex(const void* c) {
    const auto d=intptr_t(c)-intptr_t(clumps.data());
    return d>=0 && d<intptr_t(sizeof(clumps)) && d%0x100==0 ? int(d/0x100) : -1;
}
inline uint32 Off(const void* p) { const auto d=intptr_t(p)-intptr_t(arena.data()); return d>=0&&d<intptr_t(arena.size()) ? uint32(d) : P(p); }

// --- jugador, ropa, streaming ---
inline CPlayerPed* __cdecl S_FindPlayerPed(int32 id) { Push("FindPlayerPed",{uint32(id)}); return reinterpret_cast<CPlayerPed*>(ped.data()); }
inline CVector* __cdecl S_FindPlayerCoors(CVector* out,int32 id) { Push("FindPlayerCoors",{uint32(id)}); out->x=cfg.coors[0]; out->y=cfg.coors[1]; out->z=cfg.coors[2]; return out; }
inline void __cdecl S_RebuildPlayerIfNeeded(void* p) { Push("RebuildPlayerIfNeeded",{P(p)}); }
inline void __cdecl S_RebuildCutscenePlayer(void* p,int32 m) { Push("RebuildCutscenePlayer",{P(p),uint32(m)}); }
inline void __cdecl S_RequestModel(int32 m,int32 f) { Push("RequestModel",{uint32(m),uint32(f)}); }
inline void __cdecl S_LoadAllRequestedModels(bool b) { Push("LoadAllRequestedModels",{b}); }
inline void __cdecl S_SetMissionDoesntRequireModel(int32 m) { Push("SetMissionDoesntRequireModel",{uint32(m)}); }
inline void __cdecl S_SetModelIsDeletable(int32 m) { Push("SetModelIsDeletable",{uint32(m)}); }
inline void __cdecl S_SetModelTxdIsDeletable(int32 m) { Push("SetModelTxdIsDeletable",{uint32(m)}); }
inline void __cdecl S_RemoveModel(int32 m) { Push("RemoveModel",{uint32(m)}); }
inline void __cdecl S_RequestSpecialModel(int32 m,const char* n,int32 f) { Push("RequestSpecialModel",{uint32(m),H(n),uint32(f)}); }
inline void __cdecl S_ImGonnaUse() { Push("ImGonnaUseStreamingMemory"); }
inline void __cdecl S_IHaveUsed() { Push("IHaveUsedStreamingMemory"); }
inline void __cdecl S_MakeSpaceFor(uint32 n) { Push("MakeSpaceFor",{n}); }
inline void __cdecl S_RemoveCurrentZonesModels() { Push("RemoveCurrentZonesModels"); }
inline void __cdecl S_RemoveUnusedModelsInLoadedList() { Push("RemoveUnusedModelsInLoadedList"); }
inline void __cdecl S_StreamZoneModels(const CVector& v) { Push("StreamZoneModels",{F(v.x),F(v.y),F(v.z)}); }
inline void __cdecl S_ForceLayerToRead(int32 a) { Push("ForceLayerToRead",{uint32(a)}); }
inline void __cdecl S_LoadInitialPeds() { Push("LoadInitialPeds"); }
inline void __cdecl S_LoadInitialWeapons() { Push("LoadInitialWeapons"); }
inline void __cdecl S_LoadInitialVehicles() { Push("LoadInitialVehicles"); }
inline void __cdecl S_SetLoadVehiclesInLoadScene(bool b) { Push("SetLoadVehiclesInLoadScene",{b}); }
inline void __cdecl S_LoadScene(const CVector& v) { Push("LoadScene",{F(v.x),F(v.y),F(v.z)}); }
inline bool __cdecl S_RemoveLoadedVehicle() { Push("RemoveLoadedVehicle"); if(cfg.removeVeh){--cfg.removeVeh;return true;} return false; }

// --- reloj, ciclo, referencias ---
inline void __cdecl S_TimerSuspend() { Push("TimerSuspend"); }
inline void __cdecl S_TimerResume() { Push("TimerResume"); }
inline void __cdecl S_TimerUpdate() { Push("TimerUpdate"); }
inline void __cdecl S_StartExtraColour(int32 c,bool b) { Push("StartExtraColour",{uint32(c),b}); }
inline void __cdecl S_StopExtraColour(bool b) { Push("StopExtraColour",{b}); }
inline void __fastcall S_RegisterReference(void* self,void*,CEntity** r) { Push("RegisterReference",{P(self),P(r)}); }
inline void __fastcall S_CleanUpOldReference(void* self,void*,CEntity** r) { Push("CleanUpOldReference",{P(self),P(r)}); }
inline void __fastcall S_UpdateRpHAnim(void* self,void*) { Push("UpdateRpHAnim",{P(self)}); }

// --- Fx ---
inline void __fastcall S_DestroyFxSystem(void* self,void*,void* fx) { Push("DestroyFxSystem",{P(self),P(fx)}); }
inline void* __fastcall S_CreateFxSystem(void* self,void*,const char* n,const RwMatrix& m,RwMatrix* om,bool b) {
    Push("CreateFxSystem",{P(self),H(n),uint32(Fnv(&m,sizeof(RwMatrix))),P(om),b}); return newFx[nNewFx++%newFx.size()].data(); }
inline void __fastcall S_CreateMatFromVec(void* self,void*,RwMatrix* out,const CVector* o,const CVector* d) {
    Push("CreateMatFromVec",{P(self),F(o->x),F(o->y),F(o->z),F(d->x),F(d->y),F(d->z)});
    auto* f=reinterpret_cast<float*>(out); for(int k=0;k<16;++k) f[k]=float(k)*0.5f;
    f[8]=d->x; f[9]=d->y; f[10]=d->z; f[12]=o->x; f[13]=o->y; f[14]=o->z; }
inline void __fastcall S_FxPlay(void* self,void*) { Push("FxPlay",{P(self)}); }
inline void __fastcall S_FxStop(void* self,void*) { Push("FxStop",{P(self)}); }

// --- mensajes, texto, basura, mundo ---
inline void __cdecl S_ClearMessages(bool b) { Push("ClearMessages",{b}); }
inline void __cdecl S_ClearThisPrintBigNow(uint16 s) { Push("ClearThisPrintBigNow",{s}); }
inline void __cdecl S_AddMessageJump(const void* t,uint32 time,uint16 flag,bool prev) { Push("AddMessageJump",{P(t),time,flag,prev}); }
inline const void* __fastcall S_TextGet(void* self,void*,const char* key) { const uint32 h=H(key); Push("TextGet",{P(self),h}); return gxt[h%4].data(); }
inline void __cdecl S_RubbishSetVisibility(bool b) { Push("RubbishSetVisibility",{b}); }
inline void __cdecl S_WorldRemove(void* e) { Push("WorldRemove",{P(e)}); }
inline void __cdecl S_WorldAdd(void* e) { Push("WorldAdd",{P(e)}); }
inline void __cdecl S_FindObjectsOfTypeInRange(uint32 model,const CVector& p,float r,bool b2D,int16* outCount,int16 maxCount,CEntity** out,bool bb,bool bv,bool bp,bool bo,bool bd) {
    Push("FindObjectsOfTypeInRange",{model,F(p.x),F(p.y),F(p.z),F(r),uint32(b2D)|uint32(bb)<<1|uint32(bv)<<2|uint32(bp)<<3|uint32(bo)<<4|uint32(bd)<<5,uint32(uint16(maxCount))});
    const uint32 h=(model*2654435761u)^cfg.salt; const int16 n=int16(h%5);
    for(int16 k=0;k<n&&k<maxCount;++k) out[k]=reinterpret_cast<CEntity*>(ents[((h>>4)+uint32(k)*7)%24].data());
    *outCount=n; }
inline void __cdecl S_ClearExcitingStuffFromArea(const CVector& p,float r,uint8 b) { Push("ClearExcitingStuffFromArea",{F(p.x),F(p.y),F(p.z),F(r),b}); }

// --- animación ---
inline void __cdecl S_RemoveLastAnimFile() { Push("RemoveLastAnimFile"); }
inline void __cdecl S_LoadAnimFile(void* s,bool c,const void* u) { Push("LoadAnimFile",{P(s),c,P(u)}); }
inline void __fastcall S_DestroyAssociations(void* self,void*) { Push("DestroyAssociations",{P(self)}); }
inline void __fastcall S_CreateAssociations(void* self,void*,const char* b,const char* a,const char* m,uint32 n) { Push("CreateAssociations",{P(self),H(b),P(a),P(m),n}); }
inline void* __fastcall S_GetAnimation(void* self,void*,const char* n) { const uint32 h=H(n); Push("GetAnimation",{P(self),h}); return (cfg.animMask>>(h%32))&1 ? statics[h%8].data() : nullptr; }
inline void* __fastcall S_CopyAnimation(void* self,void*,const char* n) {
    const uint32 h=H(n); Push("CopyAnimation",{P(self),h}); auto& a=newAssocs[nNewAssoc++%newAssocs.size()];
    At<void*>(a.data(),0x14)=hiers[(h>>3)%8].data(); return a.data(); }
inline void __fastcall S_SetCurrentTime(void* self,void*,float t) { Push("SetCurrentTime",{P(self),F(t)}); }
inline void* __cdecl S_FindFrame(RpClump* c,const char* n) { const uint32 h=H(n); Push("RpAnimBlendClumpFindFrame",{P(c),h}); return frameData[h%16].data(); }
inline void* __cdecl S_GetFirstAssociation(RpClump* c) { Push("GetFirstAssociation",{P(c)}); const int i=ClumpIndex(c); return i<0||cfg.clumpAssoc[i]<0 ? nullptr : assocs[cfg.clumpAssoc[i]].data(); }

// --- RwHelper / RW ---
inline void* __cdecl S_GetAnimHierarchyFromSkinClump(RpClump* c) { Push("GetAnimHierarchyFromSkinClump",{P(c)}); return reinterpret_cast<uint8*>(c)+0x80; }
inline int32 __cdecl S_RpHAnimIDGetIndex(void* h,int32 id) { Push("RpHAnimIDGetIndex",{P(h),uint32(id)}); return int32(((uint32(id)*2654435761u)^cfg.salt)%32); }
inline void* __cdecl S_GetFirstAtomic(RpClump* c) { Push("GetFirstAtomic",{P(c)}); const int i=ClumpIndex(c); return i>=0&&((cfg.atomicMask>>i)&1) ? atomics[i%8].data() : nullptr; }
inline void* __cdecl S_RpSkinGeometryGetSkin(void* g) { Push("RpSkinGeometryGetSkin",{P(g)}); return P(g)&4 ? reinterpret_cast<void*>(0x5000) : nullptr; }
inline void* __cdecl S_GetMatrixArray(void* h) { Push("RpHAnimHierarchyGetMatrixArray",{P(h)}); return matArray.data(); }
inline void* __cdecl S_GetFrameFromName(RpClump* c,const char* n) { const uint32 h=H(n); Push("GetFrameFromName",{P(c),h}); return (cfg.frameMask>>(h%32))&1 ? frames[h%16].data() : nullptr; }
inline void* __cdecl S_RwFrameGetLTM(void* f) { Push("RwFrameGetLTM",{P(f)}); return static_cast<uint8*>(f)+0x50; }
inline RpClump* __cdecl S_ForAllAtomics(RpClump* c,RpAtomic*(*cb)(RpAtomic*,void*),void* data) {
    const int i=ClumpIndex(c); Push("RpClumpForAllAtomics",{P(c),cb!=nullptr});
    if(i<0) return c;
    for(uint8 k=0;k<cfg.clumpAtoms[i];++k) if(!cb(reinterpret_cast<RpAtomic*>(atomics[cfg.clumpAtomList[i][k]].data()),data)) break;
    return c; }
inline void __cdecl S_ResyncSphere(void* a) { Push("ResyncInterpolatedSphere",{P(a)}); }
inline void* __cdecl S_RwStreamOpen(int32 t,int32 a,const void* d) { Push("RwStreamOpen",{uint32(t),uint32(a),H(static_cast<const char*>(d))}); return stream.data(); }
inline void* __cdecl S_RwStreamSkip(void* s,uint32 off) { Push("RwStreamSkip",{P(s),off}); return s; }
inline uint32 __cdecl S_RwStreamRead(void* s,void* buf,uint32 len) {
    Push("RwStreamRead",{P(s),Off(buf),len}); auto* b=static_cast<char*>(buf); const uint32 n=std::min(len,fileLen);
    std::memcpy(b,fileText.data(),n); std::memset(b+n,0,len-n); return len; }
inline int32 __cdecl S_RwStreamClose(void* s,void* d) { Push("RwStreamClose",{P(s),P(d)}); return 1; }

// --- juego, IPL, cámara ---
inline void __cdecl S_DrasticTidyUpMemory(bool b) { Push("DrasticTidyUpMemory",{b}); }
inline void __cdecl S_AddIplsNeededAtPosn(const CVector& v) { Push("AddIplsNeededAtPosn",{F(v.x),F(v.y),F(v.z)}); }
inline void __cdecl S_ClearIplsNeededAtPosn() { Push("ClearIplsNeededAtPosn"); }
inline void __fastcall S_CamRestoreWithJumpCut(void* s,void*) { Push("CamRestoreWithJumpCut",{P(s)}); }
inline void __fastcall S_CamSetWideScreenOff(void* s,void*) { Push("CamSetWideScreenOff",{P(s)}); }
inline void __fastcall S_CamSetWideScreenOn(void* s,void*) { Push("CamSetWideScreenOn",{P(s)}); }
inline void __fastcall S_CamDeleteCutSceneCamDataMemory(void* s,void*) { Push("CamDeleteCutSceneCamDataMemory",{P(s)}); }
inline void __fastcall S_CamFinishCutscene(void* s,void*) { Push("CamFinishCutscene",{P(s)}); }
inline int32 __fastcall S_CamGetScreenFadeStatus(void* s,void*) { Push("CamGetScreenFadeStatus",{P(s)}); return cfg.fadeStatus; }
inline uint32 __fastcall S_CamGetCutSceneFinishTime(void* s,void*) { Push("CamGetCutSceneFinishTime",{P(s)}); return cfg.finishTime; }
inline float __fastcall S_CamGetPositionAlongSpline(void* s,void*) { Push("CamGetPositionAlongSpline",{P(s)}); return cfg.spline; }
inline void __fastcall S_CamLoadPathSplines(void* s,void*,void* f) { Push("CamLoadPathSplines",{P(s),P(f)}); }
inline void __fastcall S_CamSetNearClipScript(void* s,void*,float v) { Push("CamSetNearClipScript",{P(s),F(v)}); }
inline void __fastcall S_CamSetCamCutSceneOffSet(void* s,void*,const CVector& v) { Push("CamSetCamCutSceneOffSet",{P(s),F(v.x),F(v.y),F(v.z)}); }
inline void __fastcall S_CamTakeControlWithSpline(void* s,void*,uint16 t) { Push("CamTakeControlWithSpline",{P(s),t}); }
inline void __fastcall S_CamFade(void* s,void*,float d,uint16 dir) { Push("CamFade",{P(s),F(d),dir}); }

// --- audio ---
inline void __fastcall S_StopCutsceneTrack(void* s,void*,bool b) { Push("StopCutsceneTrack",{P(s),b}); }
inline void __cdecl S_EnableEffectsLoading() { Push("EnableEffectsLoading"); }
inline void __cdecl S_DisableEffectsLoading() { Push("DisableEffectsLoading"); }
inline void __fastcall S_PreloadCutsceneTrack(void* s,void*,int16 id,bool w) { Push("PreloadCutsceneTrack",{P(s),uint32(uint16(id)),w}); }
inline void __cdecl S_PlayPreloadedCutsceneTrack() { Push("PlayPreloadedCutsceneTrack"); }
inline void __cdecl S_EnableAllPedSpeech() { Push("EnableAllPedSpeech"); }
inline void __cdecl S_DisableAllPedSpeech() { Push("DisableAllPedSpeech"); }

// --- scripts, jugador, HUD, plantas, model infos ---
inline bool __fastcall S_HasResourceBeenRequested(void* s,void*,int32 m,int32 t) { Push("HasResourceBeenRequested",{P(s),uint32(m),uint32(t)}); return (cfg.resMask>>(uint32(m)%32))&1; }
inline void __fastcall S_MakePlayerSafe(void* s,void*,bool c,float r) { Push("MakePlayerSafe",{P(s),c,F(r)}); }
inline void __cdecl S_SetHelpMessage(const void* t,bool a,bool b,bool c) { Push("SetHelpMessage",{P(t),a,b,c}); }
inline bool __cdecl S_PreUpdateOnceForNewCameraPos(const CVector& v) { Push("PreUpdateOnceForNewCameraPos",{F(v.x),F(v.y),F(v.z)}); return true; }
inline void* __cdecl S_GetModelInfoByName(const char* n,int32* idx) {
    const uint32 h=H(n)^cfg.salt; Push("GetModelInfoByName",{H(n),idx!=nullptr});
    if(!((cfg.miMask>>(h%32))&1)) return nullptr;
    if(idx) *idx=int32(400+h%200);
    return otherMi.data(); }

// --- ped, armas, wanted, colisión, radar, pad, directorio, ficheros, model info ---
inline void __fastcall S_RemoveWeaponAnims(void* s,void*,int32 a,float d) { Push("RemoveWeaponAnims",{P(s),uint32(a),F(d)}); }
inline void __fastcall S_ClearWeapons(void* s,void*) { Push("ClearWeapons",{P(s)}); }
inline int32 __fastcall S_GiveWeapon(void* s,void*,int32 t,uint32 ammo,bool b) { Push("GiveWeapon",{P(s),uint32(t),ammo,b}); return 0; }
inline void __fastcall S_ResetSprintEnergy(void* s,void*) { Push("ResetSprintEnergy",{P(s)}); }
inline void* __cdecl S_GetWeaponInfo(int32 t,uint8 skill) { Push("GetWeaponInfo",{uint32(t),skill}); return weaponInfos[uint32(t)%8].data(); }
inline void __fastcall S_ClearQdCrimes(void* s,void*) { Push("ClearQdCrimes",{P(s)}); }
inline void __cdecl S_RemoveAllCollision() { Push("RemoveAllCollision"); }
inline void __cdecl S_RemoveRadarSections() { Push("RemoveRadarSections"); }
inline void __fastcall S_PadClear(void* s,void*,bool a,bool b) { Push("PadClear",{P(s),a,b}); }
inline void __fastcall S_ReadDirFile(void* s,void*,const char* n) { Push("ReadDirFile",{P(s),H(n)}); if(s==dirObj.data()) At<uint32>(s,8)=cfg.dirLoadCount; }
inline void* __cdecl S_OpenFile(const char* p,const char* m) { Push("FileOpen",{H(p),H(m)}); return reinterpret_cast<void*>(0x7E57F11E); }
inline bool __cdecl S_FileSeek(void* f,int32 off,int32 o) { Push("FileSeek",{P(f),uint32(off),uint32(o)}); return true; }
inline int32 __cdecl S_CloseFile(void* f) { Push("FileClose",{P(f)}); return 0; }
inline void __fastcall S_SetColModel(void* s,void*,void* c,bool lod) { Push("SetColModel",{P(s),P(c),lod}); }

// --- creación de objetos de cinemática: memoria fija, constructor falso y vtable trazada ---
inline void* __cdecl S_ObjectNew(size_t n) { Push("ObjectNew",{uint32(n)}); return newObjs[nNewObj++%newObjs.size()].data(); }
inline void* __fastcall S_CutsceneObjectCtor(void* s,void*) { Push("CutsceneObjectCtor",{P(s)}); At<void*>(s,0)=vt.data(); At<void*>(s,0x18)=nullptr; return s; }
inline void __fastcall V_Dtor(void* s,void*,uint32 flags) { Push("vDtor",{P(s),flags}); }
inline void __fastcall V_SetModelIndex(void* s,void*,uint32 idx) {
    Push("vSetModelIndex",{P(s),idx}); At<uint16>(s,0x22)=uint16(idx);
    const auto dn=intptr_t(s)-intptr_t(newObjs.data()), df=intptr_t(s)-intptr_t(objs.data());
    if(dn>=0&&dn<intptr_t(sizeof(newObjs))) At<void*>(s,0x18)=clumps[8+size_t(dn/0x1A0)].data();
    else if(df>=0&&df<intptr_t(sizeof(objs))) At<void*>(s,0x18)=clumps[size_t(df/0x1A0)].data(); }
inline void __fastcall V_DeleteRwObject(void* s,void*) { Push("vDeleteRwObject",{P(s)}); }
inline void __fastcall V_Trap(void* s,void*) { Push("vTrap",{P(s)}); }

// --- operator new/delete del EXE (solo durante la llamada) ---
inline void* __cdecl S_New(size_t n) { Push("new",{uint32(n)}); const uint32 at=(arenaUsed+15)&~15u; if(at+n>arena.size()) return nullptr; arenaUsed=at+uint32(n); return arena.data()+at; }
inline void* __cdecl S_NewArr(size_t n) { Push("newArr",{uint32(n)}); const uint32 at=(arenaUsed+15)&~15u; if(at+n>arena.size()) return nullptr; arenaUsed=at+uint32(n); return arena.data()+at; }
inline void __cdecl S_Delete(void* p) { Push("delete",{Off(p)}); }
inline void __cdecl S_DeleteArr(void* p) { Push("deleteArr",{Off(p)}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,35> fns{{
    {"CCutsceneMgr","AddCutsceneHead",0x5B0380,0,-1,64},{"CCutsceneMgr","AppendToNextCutscene",0x4D5DB0,0,-1,256},
    {"CCutsceneMgr","AttachObjectToBone",0x5B0450,0,-1,256},{"CCutsceneMgr","AttachObjectToFrame",0x5B0480,0,-1,256},
    {"CCutsceneMgr","AttachObjectToParent",0x5B04B0,0,-1,256},{"CCutsceneMgr","BuildCutscenePlayer",0x4D5E20,0,-1,64},
    {"CCutsceneMgr","CalculateBoundingSphereRadiusCB",0x5B0130,0,-1,1024},{"CCutsceneMgr","UpdateCutsceneObjectBoundingBox",0x5B01E0,0,-1,512},
    {"CCutsceneMgr","CreateCutsceneObject",0x5B02A0,0,-1,256},{"CCutsceneMgr","DeleteCutsceneData",0x4D5ED0,0,-1,256},
    {"CCutsceneMgr","DeleteCutsceneData_overlay",0x5AFD60,0,-1,512},{"CCutsceneMgr","FinishCutscene",0x5B04D0,0,-1,256},
    {"CCutsceneMgr","GetCutsceneTimeInMilleseconds",0x5B0550,0,-1,1024},{"CCutsceneMgr","HasCutsceneFinished",0x5B0570,0,-1,512},
    {"CCutsceneMgr","HideRequestedObjects",0x5AFAD0,0,-1,512},{"CCutsceneMgr","Initialise",0x4D5A20,0,-1,64},
    {"CCutsceneMgr","IsCutsceneSkipButtonBeingPressed",0x4D5D10,0,-1,1024},{"CCutsceneMgr","LoadAnimationUncompressed",0x4D5AB0,0,-1,256},
    {"CCutsceneMgr","LoadCutsceneData",0x4D5E80,0,-1,128},{"CCutsceneMgr","LoadCutsceneData_loading",0x5B11C0,0,-1,512},
    {"CCutsceneMgr","LoadCutsceneData_overlay",0x5B13F0,0,-1,256},{"CCutsceneMgr","LoadCutsceneData_postload",0x5AFBC0,0,-1,256},
    {"CCutsceneMgr","LoadCutsceneData_preload",0x5B05A0,0,-1,1024},{"CCutsceneMgr","LoadEverythingBecauseCutsceneDeletedAllOfIt",0x4D5C10,0,-1,256},
    {"CCutsceneMgr","RemoveCutscenePlayer",0x4D5E50,0,-1,64},{"CCutsceneMgr","RemoveEverythingBecauseCutsceneDoesntFitInMemory",0x4D5AF0,0,-1,256},
    {"CCutsceneMgr","SetCutsceneAnim",0x5B0390,0,-1,512},{"CCutsceneMgr","SetCutsceneAnimToLoop",0x5B0420,0,-1,256},
    {"CCutsceneMgr","SetHeadAnim",0x5B0440,0,-1,64},{"CCutsceneMgr","SetupCutsceneToStart",0x5B14D0,0,-1,512},
    {"CCutsceneMgr","Shutdown",0x4D5E60,0,-1,64},{"CCutsceneMgr","SkipCutscene",0x5B1700,0,-1,128},
    {"CCutsceneMgr","StartCutscene",0x5B1460,0,-1,256},{"CCutsceneMgr","Update",0x4D5D00,0,-1,256},
    {"CCutsceneMgr","Update_overlay",0x5B1720,0,-1,1024},
}};

// --- estado global guardado (todas las regiones que el fixture o las funciones escriben) ---
struct Region { uintptr_t addr; size_t size; };
inline const std::array<Region,21> regions{{
    {0xB5EB58,0xB5F860-0xB5EB58},{0xBC1CF8,0xBC4040-0xBC1CF8},{0x8AC158,1},{0x968A30,20*0x30},{0x8D2530,4},{0x8A5B20,4},
    {0x9654B0,1},{0xB7CD6D,1},{0xB71670,352},{0xB73458,0x134},{0xB72F20,0xB73440-0xB72F20},{0xC920EC,4},{0xBAAD40,128},
    {0xA9B0C8+300*4,20*4},{0x8E4CC0+290*20,110*20},{0xB6F081,1},{0xB6F1A8,3*0x238},{0xB7CB58,4},{0xB79E44,4},{0xB7C484,4},{0xB5F878,4},
}};
inline std::vector<std::vector<uint8>> saved;
inline void SaveAll() { saved.clear(); for(auto& r:regions) saved.emplace_back(reinterpret_cast<uint8*>(r.addr),reinterpret_cast<uint8*>(r.addr)+r.size); }
inline void RestoreAll() { for(size_t k=0;k<regions.size();++k) std::memcpy(reinterpret_cast<void*>(regions[k].addr),saved[k].data(),regions[k].size); }
inline bool StateRestored() { for(size_t k=0;k<regions.size();++k) if(std::memcmp(reinterpret_cast<void*>(regions[k].addr),saved[k].data(),regions[k].size)) return false; return true; }

// variables de CCutsceneMgr comparadas una a una (<=16 bytes en crudo, el resto por hash)
struct Var { const char* name; uintptr_t addr; size_t size; };
inline const std::array<Var,38> vars{{
    {"numPlayerWeaponsToRestore",0xB5EB58,4},{"weaponsAmmo",0xB5EB5C,52},{"weaponsType",0xB5EB90,52},{"appendAnim",0xB5EBC8,1600},
    {"appendObject",0xB5F208,1600},{"dir",0xB5F848,4},{"loadStatus",0xB5F84C,4},{"flags850",0xB5F850,6},{"numAppend",0xB5F858,4},
    {"restoreEverything",0xB5F85C,1},{"finishing",0xBC1CF8,1},{"zoneWeapons",0xBC1CFC,4},{"zoneAmmo",0xBC1D00,52},{"zoneType",0xBC1D34,52},
    {"prevDensities",0xBC1D68,8},{"particles",0xBC1D70,0x360},{"toHide",0xBC20D0,0x898},{"hidden",0xBC2968,200},{"numAttach",0xBC2A30,4},
    {"repeat",0xBC2A34,50},{"attach",0xBC2A68,600},{"uncompressed",0xBC2CC0,256},{"textDuration",0xBC2DC0,256},{"textStart",0xBC2EC0,256},
    {"textOutput",0xBC2FC0,512},{"modelIndex",0xBC31C0,200},{"loadAnim",0xBC3288,1600},{"loadObject",0xBC38C8,1600},{"timer",0xBC3F08,4},
    {"name",0xBC3F0C,8},{"objects",0xBC3F18,200},{"playStatus",0xBC3FE0,4},{"counts",0xBC3FE4,0x1C},{"extra",0xBC4000,8},
    {"assocGroup",0xBC4020,0x14},{"offset",0xBC4034,12},{"shadows",0x8AC158,1},{"colCut",0x968A30,20*0x30},
}};
inline json Collect() {
    json o=json::object();
    for(auto& v:vars) o[v.name]=v.size<=16 ? json(ReversingProbe::Bytes(reinterpret_cast<void*>(v.addr),v.size)) : json(Fnv(reinterpret_cast<void*>(v.addr),v.size));
    o["densities"]=json::array({*reinterpret_cast<uint32*>(0x8D2530),*reinterpret_cast<uint32*>(0x8A5B20)});
    o["streamFlags"]=json::array({*reinterpret_cast<uint8*>(0x9654B0),*reinterpret_cast<uint8*>(0xB7CD6D)});
    o["gString"]=Fnv(reinterpret_cast<void*>(0xB71670),352); o["pad0"]=Fnv(reinterpret_cast<void*>(0xB73458),0x134);
    o["bigMessage"]=*reinterpret_cast<uint8*>(0xBAAD40); o["info"]=Fnv(reinterpret_cast<void*>(0x8E4CC0+290*20),110*20);
    o["mis"]=Fnv(mis.data(),sizeof(mis)); o["ped"]=Fnv(ped.data(),ped.size()); o["objs"]=Fnv(objs.data(),sizeof(objs)); o["newObjs"]=Fnv(newObjs.data(),sizeof(newObjs));
    o["objMats"]=Fnv(objMats.data(),sizeof(objMats)); o["clumps"]=Fnv(clumps.data(),sizeof(clumps)); o["clumpData"]=Fnv(clumpData.data(),sizeof(clumpData));
    o["frameData"]=Fnv(frameData.data(),sizeof(frameData)); o["frames"]=Fnv(frames.data(),sizeof(frames)); o["atomics"]=Fnv(atomics.data(),sizeof(atomics));
    o["assocs"]=Fnv(assocs.data(),sizeof(assocs)); o["newAssocs"]=Fnv(newAssocs.data(),sizeof(newAssocs)); o["statics"]=Fnv(statics.data(),sizeof(statics));
    o["hiers"]=Fnv(hiers.data(),sizeof(hiers)); o["ents"]=Fnv(ents.data(),sizeof(ents)); o["fxs"]=Fnv(fxs.data(),sizeof(fxs)); o["newFx"]=Fnv(newFx.data(),sizeof(newFx));
    o["dirObj"]=ReversingProbe::Bytes(dirObj.data(),0x10); o["dirEntries"]=Fnv(dirEntries.data(),sizeof(dirEntries));
    o["arena"]=Fnv(arena.data(),arenaUsed); o["arenaUsed"]=arenaUsed; o["recs"]=Recs();
    return o;
}

// --- fixture ---
inline uint32 st{};
inline uint32 Next() { st^=st<<13; st^=st>>17; st^=st<<5; return st; }
inline uint32 R(uint32 n) { return n ? Next()%n : 0; }
inline float Uf(float a,float b) { return a+(b-a)*float(Next()%1000000)/1000000.f; }
inline float Q(float a,float b) { return float(int(Uf(a,b)*4.f))/4.f; } // cuartos: exactos en decimal y en binario
inline void Fill(void* p,size_t n) { auto* b=static_cast<uint8*>(p); for(size_t k=0;k<n;k+=4){ const uint32 v=Next(); for(size_t j=0;j<4&&k+j<n;++j) b[k+j]=uint8(v>>(8*j)); } }
inline void Str(char* dst,const char* s) { std::strcpy(dst,s); }
inline std::string Qs(float a,float b) { char t[32]; std::snprintf(t,sizeof(t),"%g",Q(a,b)); return t; }
inline const char* csNames[]{"intro1a","SWEET1A","finale","FINALE","Finale","smoke2a","xyzzy","BCESAR2","epilog","cas_1a","DATE6b"};
inline const char* animNames[]{"walk","IDLE_a","run01","csplay_hand","sweet_a","Jump","cs_cam","Wave2"};
inline const char* objNames[]{"csplay","CSPLAY","sweet","smoke","cs_door","car01","ryder","lamp_b"};
inline const char* partNames[]{"Bip01","head","L Hand","wheel_lf","door","12","3","0"};
inline void RandName(char* dst,size_t maxLen) { const size_t n=R(uint32(maxLen+1)); static const char cs[]="abcdefghijKLMNOPQRstuvwxyz0123456789_-"; for(size_t k=0;k<n;++k) dst[k]=cs[R(sizeof(cs)-1)]; dst[n]=0; }

inline void GenFile() {
    std::string t; auto nl=[&]{ static const char* s[]{"\n","\r\n","\n\n","\r\r\n"}; t+=s[R(4)]; }; auto line=[&](const std::string& s){ t+=s; nl(); };
    int nModels=0,nUnc=0,nPfx=0,nText=0,nRemove=0;
    const uint32 nSec=R(9);
    for(uint32 s=0;s<nSec;++s) switch(R(10)) {
    case 0: line("info"); if(R(5)) line("offset "+Qs(-3000.f,3000.f)+" "+Qs(-3000.f,3000.f)+(R(3)?" ":"  ")+Qs(-50.f,300.f)); if(R(4)==0) line("speed 1"); line("end"); break;
    case 1: { line("model"); for(uint32 k=R(4);k-->0&&nModels<8;) { std::string l=std::to_string(int(R(5))-1)+(R(2)?",":" ")+objNames[R(8)]; for(uint32 a=R(4);a-->0&&nModels<8;++nModels) l+=std::string(R(2)?",":" ")+animNames[R(8)]; line(l); } line("end"); break; }
    case 2: line("text"); for(uint32 k=R(3);k-->0&&nText<12;++nText) { char key[8]; RandName(key,1+R(6)); if(!key[0]) Str(key,"CS1"); line(std::to_string(R(20000))+","+std::to_string(R(4000))+","+key); } line("end"); break;
    case 3: line("uncompress"); for(uint32 k=R(3);k-->0&&nUnc<6;++nUnc) line(animNames[R(8)]); line("end"); break;
    case 4: line("attach"); for(uint32 k=R(3);k-->0;) line(std::to_string(R(6))+","+std::to_string(R(6))+","+std::to_string(R(40))); line("end"); break;
    case 5: line("remove"); for(uint32 k=R(3);k-->0&&nRemove<10;++nRemove) line(Qs(-3000.f,3000.f)+","+Qs(-3000.f,3000.f)+","+Qs(-10.f,100.f)+","+objNames[R(8)]); line("end"); break;
    case 6: line("peffect"); for(uint32 k=R(2)+1;k-->0&&nPfx<3;++nPfx) line(std::string(R(2)?"prt_smoke":"water_splash")+","+std::to_string(R(5000))+","+std::to_string(int(R(5000))-1)+","+std::to_string(int(R(7))-1)+","+partNames[R(8)]+","+Qs(-5.f,5.f)+","+Qs(-5.f,5.f)+","+Qs(-5.f,5.f)+","+Qs(-1.f,1.f)+","+Qs(-1.f,1.f)+","+Qs(-1.f,1.f)); line("end"); break;
    case 7: line("extracol"); for(uint32 k=R(3)+1;k-->0;) line(std::to_string(R(4))); line("end"); break;
    case 8: line("motion"); line("1 2 3"); line("end"); break;
    default: line(R(2)?"end":"unknown"); break;
    }
    if(t.size()>1900) t.resize(1900);
    fileLen=uint32(t.size()); std::memcpy(fileText.data(),t.data(),t.size()); fileText[t.size()]=0;
}

inline void Build(const std::string& nm) {
    using CM=CCutsceneMgr;
    // parámetros de los stubs
    cfg.salt=Next(); cfg.animMask=Next(); cfg.miMask=Next(); cfg.resMask=Next(); cfg.atomicMask=Next(); cfg.frameMask=Next();
    cfg.removeVeh=R(4); cfg.finishTime=R(3) ? R(60000) : Next(); cfg.dirLoadCount=R(4) ? 1+R(8) : 0; cfg.fadeStatus=int32(R(3));
    { static const uint32 sp[]{0x3F800000u,0x3F7FFFFFu,0x3F800001u,0u,0x7FC00000u}; cfg.spline=R(3) ? bit_cast<float>(sp[R(5)]) : Uf(0.f,1.f); }
    cfg.coors[0]=Uf(-3000.f,3000.f); cfg.coors[1]=Uf(-3000.f,3000.f); cfg.coors[2]=Uf(0.f,100.f);
    for(size_t k=0;k<16;++k){ cfg.clumpAtoms[k]=uint8(R(4)); for(auto& a:cfg.clumpAtomList[k]) a=uint8(R(8)); cfg.clumpAssoc[k]=R(3) ? int8(R(16)) : int8(-1); }
    nNewObj=nNewAssoc=nNewFx=0; arenaUsed=0; std::memset(arena.data(),0xCD,arena.size()); nrec=0; recOverflow=false;

    // vtable falsa
    for(auto& v:vt) v=reinterpret_cast<void*>(&V_Trap);
    vt[0]=reinterpret_cast<void*>(&V_Dtor); vt[5]=reinterpret_cast<void*>(&V_SetModelIndex); vt[8]=reinterpret_cast<void*>(&V_DeleteRwObject);

    // ped del jugador (fuera de vehículo: bInVehicle limpio)
    Fill(ped.data(),ped.size()); At<uint32>(ped.data(),0x46C)&=~0x100u; At<void*>(ped.data(),0x480)=R(5) ? plData.data() : nullptr;
    Fill(plData.data(),plData.size()); At<void*>(plData.data(),0)=R(5) ? wanted.data() : nullptr; Fill(wanted.data(),wanted.size());
    for(int k=0;k<13;++k){ At<int32>(ped.data(),0x5A0+k*0x1C)=R(3) ? 0 : int32(1+R(45)); At<uint32>(ped.data(),0x5A0+k*0x1C+0xC)=R(2) ? R(500) : Next(); }

    // frames (padres en orden creciente: sin ciclos) y atomics
    for(size_t k=0;k<16;++k){ auto* f=frames[k].data(); Fill(f,0xA0); At<void*>(f,4)=k+1<16&&R(3) ? frames[k+1].data() : nullptr;
        for(int r=0;r<3;++r) for(int c=0;c<3;++c) At<float>(f,0x10+r*0x10+c*4)=Uf(-1.f,1.f);
        At<float>(f,0x40)=Uf(-4.f,4.f); At<float>(f,0x44)=Uf(-4.f,4.f); At<float>(f,0x48)=Uf(-4.f,4.f);
        At<uint32>(f,0x1C)=0; At<uint32>(f,0x2C)=0; At<uint32>(f,0x3C)=0; At<uint32>(f,0x4C)=0; }
    for(size_t k=0;k<8;++k){ auto* a=atomics[k].data(); Fill(a,0x80); a[0]=1; At<void*>(a,4)=frames[R(16)].data(); At<uint32>(a,0x18)=R(2) ? 0x6000+R(16)*4 : 0;
        At<float>(a,0x1C)=Uf(-3.f,3.f); At<float>(a,0x20)=Uf(-3.f,3.f); At<float>(a,0x24)=Uf(-3.f,3.f); At<float>(a,0x28)=Uf(0.f,4.f); }

    // jerarquías, secuencias y keyframes; asociaciones
    for(size_t k=0;k<8;++k){ Fill(keys[k].data(),0x20); At<float>(keys[k].data(),0x14)=Uf(-20.f,20.f); At<float>(keys[k].data(),0x18)=Uf(-20.f,20.f); At<float>(keys[k].data(),0x1C)=Uf(-20.f,20.f);
        Fill(seqs[k].data(),0x10); At<void*>(seqs[k].data(),8)=keys[k].data();
        Fill(hiers[k].data(),0x20); At<void*>(hiers[k].data(),4)=seqs[k].data(); hiers[k][0xA]=uint8(R(2)); hiers[k][0xB]=uint8(R(2));
        Fill(statics[k].data(),0x20); At<void*>(statics[k].data(),0x10)=hiers[R(8)].data();
        Fill(newAssocs[k].data(),0x40); Fill(fxs[k].data(),0x40); Fill(newFx[k].data(),0x40); }
    for(auto& a:assocs){ Fill(a.data(),0x40); At<void*>(a.data(),0x14)=hiers[R(8)].data(); }

    // clumps con datos de animación
    const uint32 clumpOffset=*reinterpret_cast<uint32*>(0xB5F878);
    for(size_t k=0;k<16;++k){ auto* c=clumps[k].data(); std::memset(c,0,0x100); c[0]=uint8(R(6) ? 2 : 1); At<void*>(c,4)=frames[k].data();
        if(clumpOffset+4<=0x100) At<void*>(c,clumpOffset)=clumpData[k].data();
        Fill(clumpData[k].data(),0x20); At<void*>(clumpData[k].data(),0)=R(2) ? assocs[R(16)].data()+4 : nullptr; At<void*>(clumpData[k].data(),0x10)=frameData[k].data();
        Fill(frameData[k].data(),0x40); }

    // objetos de cinemática falsos
    const uint32 numObjs=R(9);
    for(size_t k=0;k<8;++k){ auto* o=objs[k].data(); Fill(o,0x1A0); At<void*>(o,0)=vt.data(); At<void*>(o,0x14)=R(2) ? objMats[k].data() : nullptr;
        At<void*>(o,0x18)=clumps[k].data(); At<int16>(o,0x22)=int16(R(3)==0 ? 300+R(20) : R(2) ? 290+R(40) : R(1000));
        At<void*>(o,0x17C)=R(2) ? nullptr : frames[k].data(); Fill(objMats[k].data(),0x48);
        for(int j=0;j<16;++j) if(j%4!=3) At<float>(objMats[k].data(),j*4)=Uf(-100.f,100.f); }
    for(auto& o:newObjs) std::memset(o.data(),0xCD,o.size());
    for(auto& e:ents) Fill(e.data(),e.size());
    for(auto& w:weaponInfos){ Fill(w.data(),w.size()); At<int32>(w.data(),0xC)=int32(330+R(40)); At<int32>(w.data(),0x10)=R(3) ? -1 : int32(330+R(40)); }
    Fill(matArray.data(),sizeof(matArray)); Fill(gxt.data(),sizeof(gxt)); Fill(otherMi.data(),otherMi.size()); Fill(stream.data(),stream.size());

    // model infos de cinemática (300-319) y estado de streaming
    auto** ptrs=reinterpret_cast<void**>(0xA9B0C8);
    for(size_t k=0;k<20;++k){ auto* m=mis[k].data(); Fill(m,0x40); At<int16>(m,8)=R(3) ? 0 : int16(Next()); At<void*>(m,0x1C)=R(3) ? clumps[R(8)].data() : nullptr; ptrs[300+k]=m; }
    auto* info=reinterpret_cast<uint8*>(0x8E4CC0);
    for(int m=300;m<=340;++m) info[m*20+0x10]=uint8(R(2) ? 1 : R(5));
    info[341*20+0x10]=0;
    for(int m=330;m<372;++m) info[m*20+6]=uint8(Next());

    // directorio CUTS.IMG falso
    Fill(dirObj.data(),dirObj.size()); At<void*>(dirObj.data(),0)=dirEntries.data(); At<uint32>(dirObj.data(),4)=8; dirObj[0xC]=uint8(R(2));
    char cs[8]; Str(cs,csNames[R(uint32(std::size(csNames)))]);
    static const char* exts[]{".CUT",".IFP",".DAT",".cut",".ifp",".dat"};
    uint32 nEntries=0;
    for(size_t k=0;k<8;++k){ auto* e=dirEntries[k].data(); Fill(e,0x20); At<uint32>(e,0)=R(4) ? R(0x4000) : Next(); At<uint16>(e,4)=uint16(1+R(2)); At<uint16>(e,6)=0;
        char nmE[24]; if(R(5)) { std::snprintf(nmE,sizeof(nmE),"%s%s",R(6) ? cs : csNames[R(uint32(std::size(csNames)))],exts[R(6)]); } else Str(nmE,"other.ifp");
        std::memcpy(e+8,nmE,std::strlen(nmE)+1); }
    nEntries=R(4) ? 1+R(8) : 0; At<uint32>(dirObj.data(),8)=nEntries;
    GenFile();

    // globales de CCutsceneMgr
    CM::numPlayerWeaponsToRestore=int32(R(5))-1;
    for(int k=0;k<13;++k){ CM::playerWeaponsToRestore_Type[k]=int32(1+R(45)); CM::playerWeaponsToRestore_Ammo[k]=int32(R(9999)); CM::playerWeaponsToRestoreInZone_Type[k]=int32(1+R(45)); CM::playerWeaponsToRestoreInZone_Ammo[k]=int32(R(9999)); }
    CM::numPlayerWeaponsToRestoreInZone=int32(R(5))-1;
    Fill(reinterpret_cast<void*>(0xB5EBC8),3200);
    for(int k=0;k<50;++k){ RandName(CM::ms_cAppendAnimName[k],R(2) ? 31 : 8); RandName(CM::ms_cAppendObjectName[k],R(2) ? 31 : 8); }
    for(int k=0;k<4;++k){ Str(CM::ms_cAppendAnimName[k],animNames[R(8)]); Str(CM::ms_cAppendObjectName[k],objNames[R(8)]); }
    CM::ms_numAppendObjectNames=int32(R(4));
    CM::ms_pCutsceneDir=reinterpret_cast<CDirectory*>(dirObj.data());
    CM::ms_cutsceneLoadStatus=CM::LoadStatus(R(3));
    for(int k=0;k<6;++k) reinterpret_cast<uint8*>(0xB5F850)[k]=uint8(R(2));
    CM::restoreEverythingAfterCutscene=R(2);
    *reinterpret_cast<uint8*>(0xBC1CF8)=uint8(R(2));
    CM::m_fPrevCarDensity=Uf(0.f,2.f); CM::m_fPrevPedDensity=Uf(0.f,2.f);
    Fill(reinterpret_cast<void*>(0xBC1D70),0x360);
    for(int k=0;k<8;++k){ auto& p=CM::ms_pParticleEffects[k]; RandName(p.m_szEffectName,12); p.m_pFxSystem=R(3) ? reinterpret_cast<FxSystem_c*>(fxs[k].data()) : nullptr;
        p.m_nStartTime=int32(R(30000)); p.m_nEndTime=R(4) ? int32(R(20000)) : -1; p.m_nObjectId=int32(R(numObjs+3))-1; Str(p.m_szObjectPart,partNames[R(8)]);
        p.m_vecPosn.x=Q(-10.f,10.f); p.m_vecPosn.y=Q(-10.f,10.f); p.m_vecPosn.z=Q(-10.f,10.f); p.m_vecDirection.x=Q(-1.f,1.f); p.m_vecDirection.y=Q(-1.f,1.f); p.m_vecDirection.z=Q(-1.f,1.f);
        p.m_bPlaying=R(2); p.m_bStopped=R(2); }
    CM::ms_iNumParticleEffects=R(5);
    Fill(reinterpret_cast<void*>(0xBC20D0),0x898);
    for(int k=0;k<50;++k){ auto& c=CM::ms_crToHideItems[k]; c.m_vecPosn.x=Uf(-3000.f,3000.f); c.m_vecPosn.y=Uf(-3000.f,3000.f); c.m_vecPosn.z=Uf(0.f,100.f); Str(c.m_szObjectName,objNames[R(8)]); }
    for(int k=0;k<50;++k) CM::ms_pHiddenEntities[k]=R(4) ? reinterpret_cast<CEntity*>(ents[R(24)].data()) : nullptr;
    CM::ms_iNumHiddenEntities=R(16)==0 ? 0xFFFFFFFFu : R(7);
    for(int k=0;k<50;++k){ auto& a=CM::ms_iAttachObjectToBone[k]; a.m_nCutscenePedObjectId=numObjs ? int32(R(numObjs)) : 0; a.m_nCutsceneAttachmentObjectId=numObjs ? int32(R(numObjs)) : 0; a.m_nBoneId=int32(R(60)); }
    CM::ms_numAttachObjectToBones=numObjs ? int32(R(4)) : 0;
    for(int k=0;k<50;++k) CM::ms_bRepeatObject[k]=char(k ? R(2) : 0);
    Fill(reinterpret_cast<void*>(0xBC2CC0),256); for(int k=0;k<8;++k) Str(CM::ms_aUncompressedCutsceneAnims[k],R(4) ? animNames[R(8)] : "");
    CM::ms_numUncompressedCutsceneAnims=R(8);
    Fill(reinterpret_cast<void*>(0xBC2DC0),0x400);
    for(int k=0;k<64;++k){ CM::ms_iTextDuration[k]=int32(R(5000)); CM::ms_iTextStartTime[k]=int32(R(30000)); char key[8]; RandName(key,7); Str(CM::ms_cTextOutput[k],key); }
    CM::ms_numTextOutput=int32(R(6)); CM::ms_currTextOutput=int32(R(uint32(CM::ms_numTextOutput+2)));
    for(int k=0;k<50;++k) CM::ms_iModelIndex[k]=eModelID(R(4)==0 ? 300+R(20) : R(3)==0 ? 2+R(2) : R(700));
    Fill(reinterpret_cast<void*>(0xBC3288),3200);
    for(int k=0;k<50;++k){ Str(CM::ms_cLoadAnimName[k],R(4) ? animNames[R(8)] : ""); Str(CM::ms_cLoadObjectName[k],objNames[R(8)]); }
    CM::ms_numLoadObjectNames=int32(R(6));
    CM::ms_cutsceneTimerS=R(8) ? Uf(0.f,40.f) : Uf(-1.f,5000.f);
    std::memset(CM::ms_cutsceneName,0,8); Str(CM::ms_cutsceneName,cs);
    for(int k=0;k<50;++k) CM::ms_pCutsceneObjects[k]=uint32(k)<numObjs ? reinterpret_cast<CCutsceneObject*>(objs[k].data()) : nullptr;
    CM::ms_cutscenePlayStatus=CM::PlayStatus(R(6)); CM::ms_numCutsceneObjs=int32(numObjs);
    CM::m_PrevExtraColour=int32(R(30)); CM::m_PrevExtraColourOn=R(2); CM::m_bDontClearZone=R(2); CM::dataFileLoaded=R(2);
    Fill(reinterpret_cast<void*>(0xBC4020),0x14);
    CM::ms_cutsceneOffset.x=Q(-3000.f,3000.f); CM::ms_cutsceneOffset.y=Q(-3000.f,3000.f); CM::ms_cutsceneOffset.z=Q(-50.f,300.f);
    CM::ms_useCutsceneShadows=int8(R(2));

    // otros globales leídos
    CPopulation::PedDensityMultiplier=Uf(0.f,2.f); CCarCtrl::CarDensityMultiplier=Uf(0.f,2.f);
    *reinterpret_cast<uint8*>(0x9654B0)=uint8(R(2)); *reinterpret_cast<uint8*>(0xB7CD6D)=uint8(R(2));
    Fill(reinterpret_cast<void*>(0xB71670),352); reinterpret_cast<char*>(0xB71670)[351]=0;
    CTimer::ms_fTimeStepNonClipped=R(6) ? Uf(0.f,3.f) : Uf(0.f,300.f);
    CTimeCycle::m_ExtraColour=int32(R(30)); CTimeCycle::m_bExtraColourOn=R(3) ? R(2) : Next();
    TheCamera.GetActiveCam().m_nMode=eCamMode(R(3) ? 17 : R(40));
    auto* pad=reinterpret_cast<uint8*>(0xB73458);
    At<int16>(pad,0x20)=int16(R(4)==0 ? 255 : 0); At<int16>(pad,0x50)=int16(R(3)==0 ? 255 : 0);
    *reinterpret_cast<uint8*>(0xB73418)=uint8(R(5)==0); *reinterpret_cast<uint8*>(0xB7342C)=uint8(R(3)==0);
    auto* nk=reinterpret_cast<uint8*>(0xB73190); auto* ok=reinterpret_cast<uint8*>(0xB72F20);
    for(size_t off:{size_t(0x23C),size_t(0x25A),size_t(0x58)}){ At<int16>(nk,off)=int16(R(6)==0); At<int16>(ok,off)=int16(R(3)==0); }
    *reinterpret_cast<int32*>(0xC920EC)=R(6)==0 ? 0 : R(3)==0 ? 0x100 : 1;

    // ajustes por función
    if(nm=="SetCutsceneAnimToLoop") cfg.animMask=0xFFFFFFFFu;
    if(nm=="Shutdown"&&R(4)==0) CM::ms_pCutsceneDir=nullptr;
    if(nm=="LoadCutsceneData"||nm=="LoadCutsceneData_overlay"||nm=="LoadCutsceneData_preload"){ CM::ms_cutsceneLoadStatus=CM::LoadStatus::NOT_LOADED; CM::ms_numCutsceneObjs=0; }
    if(nm=="DeleteCutsceneData"||nm=="DeleteCutsceneData_overlay") if(R(6)) CM::ms_cutsceneLoadStatus=CM::LoadStatus(1+R(2));
    if(nm=="Update"||nm=="Update_overlay") if(R(5)) CM::ms_cutsceneLoadStatus=CM::LoadStatus(1+R(2));
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"cutscene35-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"cutscene35-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("cutscene35");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning() || CCutsceneMgr::ms_cutsceneLoadStatus!=CCutsceneMgr::LoadStatus::NOT_LOADED)return; done=true;
    std::string only; { std::ifstream f(dir/"cutscene35-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,35> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,35> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    if(*reinterpret_cast<uint32*>(0xB5F878)+4>0x100){Write({{"event","error"},{"reason","ClumpOffset fuera del clump falso"},{"offset",*reinterpret_cast<uint32*>(0xB5F878)}});return;}

    // constructor DLL de CCutsceneObject (exportado por NOTSA_EXPORT_VTABLE; si no, por símbolo)
    uintptr_t dllCtor=0;
    { HMODULE m{}; if(GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(bit_cast<uintptr_t>(&CCutsceneMgr::Update)),&m)&&m)
          dllCtor=reinterpret_cast<uintptr_t>(GetProcAddress(m,"??0CCutsceneObject@@QAE@XZ"));
      if(!dllCtor){ SymInitialize(GetCurrentProcess(),nullptr,TRUE); alignas(SYMBOL_INFO) char buf[sizeof(SYMBOL_INFO)+256]{}; auto* si=reinterpret_cast<SYMBOL_INFO*>(buf); si->SizeOfStruct=sizeof(SYMBOL_INFO); si->MaxNameLen=255;
          if(SymFromName(GetCurrentProcess(),"CCutsceneObject::CCutsceneObject",si)) dllCtor=uintptr_t(si->Address); } }
    if(!dllCtor){Write({{"event","error"},{"reason","constructor DLL de CCutsceneObject no encontrado"}});return;}

    SaveAll();
    bool passed=true; int pair=0;
    {
        // direcciones resueltas antes de escribir ningún parche; sin duplicados
        std::vector<std::pair<uintptr_t,void*>> specs;
        auto S=[&](uintptr_t exeAddr,uintptr_t dllAddr,auto fake){ void* f=reinterpret_cast<void*>(fake); if(exeAddr)specs.emplace_back(exeAddr,f); if(dllAddr&&dllAddr!=exeAddr)specs.emplace_back(dllAddr,f); };
        #define D(x) bit_cast<uintptr_t>(x)
        S(0x56E210,D(static_cast<CPlayerPed*(*)(int32)>(&FindPlayerPed)),&S_FindPlayerPed);
        S(0x56E010,D(static_cast<CVector(*)(int32)>(&FindPlayerCoors)),&S_FindPlayerCoors);
        S(0x5A8390,D(&CClothes::RebuildPlayerIfNeeded),&S_RebuildPlayerIfNeeded); S(0x5A8270,D(&CClothes::RebuildCutscenePlayer),&S_RebuildCutscenePlayer);
        S(0x4087E0,D(&CStreaming::RequestModel),&S_RequestModel); S(0x40EA10,D(&CStreaming::LoadAllRequestedModels),&S_LoadAllRequestedModels);
        S(0x409C90,D(&CStreaming::SetMissionDoesntRequireModel),&S_SetMissionDoesntRequireModel); S(0x409C10,D(&CStreaming::SetModelIsDeletable),&S_SetModelIsDeletable);
        S(0x409C70,D(&CStreaming::SetModelTxdIsDeletable),&S_SetModelTxdIsDeletable); S(0x4089A0,D(&CStreaming::RemoveModel),&S_RemoveModel);
        S(0x409D10,D(&CStreaming::RequestSpecialModel),&S_RequestSpecialModel); S(0x407BE0,D(&CStreaming::ImGonnaUseStreamingMemory),&S_ImGonnaUse);
        S(0x407BF0,D(&CStreaming::IHaveUsedStreamingMemory),&S_IHaveUsed); S(0x40E120,D(&CStreaming::MakeSpaceFor),&S_MakeSpaceFor);
        S(0x40B080,D(&CStreaming::RemoveCurrentZonesModels),&S_RemoveCurrentZonesModels); S(0x407AC0,D(&CStreaming::RemoveUnusedModelsInLoadedList),&S_RemoveUnusedModelsInLoadedList);
        S(0x40A560,D(&CStreaming::StreamZoneModels),&S_StreamZoneModels); S(0x407A10,D(&CStreaming::ForceLayerToRead),&S_ForceLayerToRead);
        S(0x40D3D0,D(&CStreaming::LoadInitialPeds),&S_LoadInitialPeds); S(0x40A120,D(&CStreaming::LoadInitialWeapons),&S_LoadInitialWeapons);
        S(0x407F20,D(&CStreaming::LoadInitialVehicles),&S_LoadInitialVehicles); S(0x407A30,D(&CStreaming::SetLoadVehiclesInLoadScene),&S_SetLoadVehiclesInLoadScene);
        S(0x40EB70,D(&CStreaming::LoadScene),&S_LoadScene); S(0x40C020,D(&CStreaming::RemoveLoadedVehicle),&S_RemoveLoadedVehicle);
        S(0x5619D0,D(&CTimer::Suspend),&S_TimerSuspend); S(0x561A00,D(&CTimer::Resume),&S_TimerResume); S(0x561B10,D(&CTimer::Update),&S_TimerUpdate);
        S(0x55FEC0,D(&CTimeCycle::StartExtraColour),&S_StartExtraColour); S(0x55FF20,D(&CTimeCycle::StopExtraColour),&S_StopExtraColour);
        S(0x571B70,D(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),&S_RegisterReference);
        S(0x571A00,D(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),&S_CleanUpOldReference);
        S(0x532B20,D(&CEntity::UpdateRpHAnim),&S_UpdateRpHAnim);
        S(0x4A9810,D(&FxManager_c::DestroyFxSystem),&S_DestroyFxSystem);
        S(0x4A9BB0,D(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const RwMatrix&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem)),&S_CreateFxSystem);
        S(0x49E950,D(&Fx_c::CreateMatFromVec),&S_CreateMatFromVec); S(0x4AA2F0,D(&FxSystem_c::Play),&S_FxPlay); S(0x4AA390,D(&FxSystem_c::Stop),&S_FxStop);
        S(0x69DCD0,D(&CMessages::ClearMessages),&S_ClearMessages); S(0x69ED80,D(&CMessages::ClearThisPrintBigNow),&S_ClearThisPrintBigNow);
        S(0x69F1E0,D(&CMessages::AddMessageJump),&S_AddMessageJump); S(0x6A0050,D(&CText::Get),&S_TextGet); S(0x7204C0,D(&CRubbish::SetVisibility),&S_RubbishSetVisibility);
        S(0x563280,D(&CWorld::Remove),&S_WorldRemove); S(0x563220,D(&CWorld::Add),&S_WorldAdd);
        S(0x564C70,D(&CWorld::FindObjectsOfTypeInRange),&S_FindObjectsOfTypeInRange); S(0x56A0D0,D(&CWorld::ClearExcitingStuffFromArea),&S_ClearExcitingStuffFromArea);
        S(0x4D3ED0,D(&CAnimManager::RemoveLastAnimFile),&S_RemoveLastAnimFile);
        S(0x4D47F0,D(static_cast<void(*)(RwStream*,bool,const char(*)[32])>(&CAnimManager::LoadAnimFile)),&S_LoadAnimFile);
        S(0x4CDFF0,D(&CAnimBlendAssocGroup::DestroyAssociations),&S_DestroyAssociations);
        S(0x4CE3B0,D(static_cast<void(CAnimBlendAssocGroup::*)(const char*,const char*,const char*,uint32)>(&CAnimBlendAssocGroup::CreateAssociations)),&S_CreateAssociations);
        S(0x4CE040,D(static_cast<CAnimBlendStaticAssociation*(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::GetAnimation)),&S_GetAnimation);
        S(0x4CE0B0,D(static_cast<CAnimBlendAssociation*(CAnimBlendAssocGroup::*)(const char*)>(&CAnimBlendAssocGroup::CopyAnimation)),&S_CopyAnimation);
        S(0x4CEA80,D(&CAnimBlendAssociation::SetCurrentTime),&S_SetCurrentTime);
        S(0x4D62A0,D(&RpAnimBlendClumpFindFrame),&S_FindFrame);
        S(0x4D15E0,D(static_cast<CAnimBlendAssociation*(*)(RpClump*)>(&RpAnimBlendClumpGetFirstAssociation)),&S_GetFirstAssociation);
        S(0x734A40,D(&GetAnimHierarchyFromSkinClump),&S_GetAnimHierarchyFromSkinClump); S(0x734820,D(&GetFirstAtomic),&S_GetFirstAtomic);
        S(0x4C5400,D(&CClumpModelInfo::GetFrameFromName),&S_GetFrameFromName);
        // RW: los símbolos DLL son envoltorios del EXE, basta con la dirección del EXE
        S(0x7C51A0,0,&S_RpHAnimIDGetIndex); S(0x7C7550,0,&S_RpSkinGeometryGetSkin); S(0x7C5120,0,&S_GetMatrixArray); S(0x7F0990,0,&S_RwFrameGetLTM);
        S(0x749B70,0,&S_ForAllAtomics); S(0x7491F0,0,&S_ResyncSphere);
        S(0x7ECEF0,0,&S_RwStreamOpen); S(0x7ECD00,0,&S_RwStreamSkip); S(0x7EC9D0,0,&S_RwStreamRead); S(0x7ECE20,0,&S_RwStreamClose);
        S(0x53C810,D(&CGame::DrasticTidyUpMemory),&S_DrasticTidyUpMemory);
        S(0x4045B0,D(&CIplStore::AddIplsNeededAtPosn),&S_AddIplsNeededAtPosn); S(0x4045E0,D(&CIplStore::ClearIplsNeededAtPosn),&S_ClearIplsNeededAtPosn);
        S(0x50BAB0,D(&CCamera::RestoreWithJumpCut),&S_CamRestoreWithJumpCut); S(0x50C150,D(&CCamera::SetWideScreenOff),&S_CamSetWideScreenOff);
        S(0x50C140,D(&CCamera::SetWideScreenOn),&S_CamSetWideScreenOn); S(0x5B24A0,D(&CCamera::DeleteCutSceneCamDataMemory),&S_CamDeleteCutSceneCamDataMemory);
        S(0x514950,D(&CCamera::FinishCutscene),&S_CamFinishCutscene); S(0x50AE20,D(&CCamera::GetScreenFadeStatus),&S_CamGetScreenFadeStatus);
        S(0x50AD90,D(&CCamera::GetCutSceneFinishTime),&S_CamGetCutSceneFinishTime); S(0x50AF80,D(&CCamera::GetPositionAlongSpline),&S_CamGetPositionAlongSpline);
        S(0x5B24D0,D(&CCamera::LoadPathSplines),&S_CamLoadPathSplines); S(0x50BF90,D(&CCamera::SetNearClipScript),&S_CamSetNearClipScript);
        S(0x50BD20,D(&CCamera::SetCamCutSceneOffSet),&S_CamSetCamCutSceneOffSet); S(0x50CAE0,D(&CCamera::TakeControlWithSpline),&S_CamTakeControlWithSpline);
        S(0x50AC20,D(&CCamera::Fade),&S_CamFade);
        S(0x507080,D(&CAudioEngine::StopCutsceneTrack),&S_StopCutsceneTrack); S(0x506E90,D(&CAudioEngine::EnableEffectsLoading),&S_EnableEffectsLoading);
        S(0x506E80,D(&CAudioEngine::DisableEffectsLoading),&S_DisableEffectsLoading); S(0x507E30,D(&CAudioEngine::PreloadCutsceneTrack),&S_PreloadCutsceneTrack);
        S(0x507070,D(&CAudioEngine::PlayPreloadedCutsceneTrack),&S_PlayPreloadedCutsceneTrack);
        S(0x4E3EC0,D(&CAEPedSpeechAudioEntity::EnableAllPedSpeech),&S_EnableAllPedSpeech); S(0x4E3EB0,D(&CAEPedSpeechAudioEntity::DisableAllPedSpeech),&S_DisableAllPedSpeech);
        S(0x470620,D(&CScriptResourceManager::HasResourceBeenRequested),&S_HasResourceBeenRequested); S(0x56E870,D(&CPlayerInfo::MakePlayerSafe),&S_MakePlayerSafe);
        S(0x588BE0,D(&CHud::SetHelpMessage),&S_SetHelpMessage); S(0x5DCF30,D(&CPlantMgr::PreUpdateOnceForNewCameraPos),&S_PreUpdateOnceForNewCameraPos);
        S(0x4C5940,D(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo)),&S_GetModelInfoByName);
        S(0x5F0250,D(&CPed::RemoveWeaponAnims),&S_RemoveWeaponAnims); S(0x5E6320,D(&CPed::ClearWeapons),&S_ClearWeapons);
        S(0x5E6080,D(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)),&S_GiveWeapon);
        S(0x60A530,D(&CPlayerPed::ResetSprintEnergy),&S_ResetSprintEnergy);
        S(0x743C60,D(static_cast<CWeaponInfo*(*)(eWeaponType,eWeaponSkill)>(&CWeaponInfo::GetWeaponInfo)),&S_GetWeaponInfo);
        S(0x561FE0,D(&CWanted::ClearQdCrimes),&S_ClearQdCrimes); S(0x410E00,D(&CColStore::RemoveAllCollision),&S_RemoveAllCollision);
        S(0x584BF0,D(&CRadar::RemoveRadarSections),&S_RemoveRadarSections); S(0x541A70,D(&CPad::Clear),&S_PadClear);
        S(0x532350,D(&CDirectory::ReadDirFile),&S_ReadDirFile);
        S(0x538900,D(&CFileMgr::OpenFile),&S_OpenFile); S(0x538990,D(&CFileMgr::Seek),&S_FileSeek); S(0x5389D0,D(&CFileMgr::CloseFile),&S_CloseFile);
        S(0x4C4BC0,D(&CBaseModelInfo::SetColModel),&S_SetColModel);
        S(0x5A1EE0,D(static_cast<void*(*)(size_t)>(&CObject::operator new)),&S_ObjectNew);
        S(0x5B19D0,dllCtor,&S_CutsceneObjectCtor);
        #undef D
        std::vector<std::unique_ptr<EntryPatch>> patches; std::vector<uintptr_t> seen;
        for(auto& [addr,fake]:specs){ if(std::find(seen.begin(),seen.end(),addr)!=seen.end())continue; seen.push_back(addr); patches.push_back(std::make_unique<EntryPatch>(addr,fake)); }

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    RestoreAll();
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC575u; if(!st)st=1;
                    Build(nm);
                    // argumentos (fijados antes de cambiar los hooks)
                    const uint32 a1=Next(), a2=Next(), a3=Next(), a4=Next();
                    char str1[40]; RandName(str1,a1%3 ? 12 : 31);
                    static const char* anims[]{"walk","IDLE_a","run01","sweet_a","Jump","cs_cam"};
                    const char* animArg=a2%4 ? anims[a2%6] : str1;
                    char csArg[8]; std::memcpy(csArg,CCutsceneMgr::ms_cutsceneName,8);
                    float maxR=a3%8==0 ? bit_cast<float>(0x7FC00000u) : a3%8==1 ? -0.f : Uf(0.f,10.f);
                    if(nm=="UpdateCutsceneObjectBoundingBox"){ auto& rad=*reinterpret_cast<float*>(0x968A30+(a2%20)*0x30+0x24);
                        rad=a3%5==0 ? 20.f : a3%5==1 ? bit_cast<float>(0x7FC00000u) : a3%5==2 ? Uf(0.f,19.9f) : Uf(20.f,60.f); }
                    if(nm=="GetCutsceneTimeInMilleseconds"){ switch(a3%6){case 0:CCutsceneMgr::ms_cutsceneTimerS=Uf(-100.f,0.f);break;case 1:CCutsceneMgr::ms_cutsceneTimerS=Uf(0.f,1e6f);break;
                        case 2:CCutsceneMgr::ms_cutsceneTimerS=Uf(1e12f,1e16f);break;case 3:CCutsceneMgr::ms_cutsceneTimerS=float(a4%100000)*0.001f;break;default:CCutsceneMgr::ms_cutsceneTimerS=Uf(0.f,100.f);} }
                    if(nm=="FinishCutscene"){ static const uint32 big[]{0x80000000u,0xFFFFFFFFu,0x01000001u,0x7FFFFFFFu,0x00FFFFFFu}; if(a3%3==0) cfg.finishTime=big[a4%5]; }
                    if(nm=="LoadAnimationUncompressed") CCutsceneMgr::ms_numUncompressedCutsceneAnims=a3%8;
                    if(nm=="AppendToNextCutscene") CCutsceneMgr::ms_numAppendObjectNames=int32(a3%50);
                    if(nm=="CreateCutsceneObject"&&a3%4==0) CCutsceneMgr::ms_numCutsceneObjs=0;

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    uint64 ret=0; int kind=0; // 0 sin retorno, 1 entero, 2 bool, 3 puntero, 4 uint64
                    {
                        EntryPatch n1(0x82119A,&S_New),n2(0x821195,&S_NewArr),d1(0x8214BD,&S_Delete),d2(0x8213AE,&S_DeleteArr);
                        void* obj1=objs[a1%8].data(); void* obj2=objs[a2%8].data();
                        if(nm=="AddCutsceneHead"){ kind=1; ret=uint32(reinterpret_cast<int32(__cdecl*)(void*,int32)>(a)(obj1,int32(a3))); }
                        else if(nm=="AppendToNextCutscene") reinterpret_cast<void(__cdecl*)(const char*,const char*)>(a)(str1,animArg);
                        else if(nm=="AttachObjectToBone") reinterpret_cast<void(__cdecl*)(void*,void*,int32)>(a)(obj1,obj2,int32(a3%64)-2);
                        else if(nm=="AttachObjectToFrame") reinterpret_cast<void(__cdecl*)(void*,void*,const char*)>(a)(obj1,obj2,partNames[a3%8]);
                        else if(nm=="AttachObjectToParent") reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(obj1,obj2);
                        else if(nm=="CalculateBoundingSphereRadiusCB"){ kind=3; ret=P(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomics[a1%8].data(),&maxR)); }
                        else if(nm=="UpdateCutsceneObjectBoundingBox") reinterpret_cast<void(__cdecl*)(void*,int32)>(a)(a1%5 ? clumps[a4%16].data() : nullptr,int32(300+a2%20));
                        else if(nm=="CreateCutsceneObject"){ kind=3; const int32 model=a2%3 ? int32(300+a4%20) : int32(a4%700); ret=P(reinterpret_cast<void*(__cdecl*)(int32)>(a)(model)); }
                        else if(nm=="GetCutsceneTimeInMilleseconds"){ kind=4; ret=reinterpret_cast<uint64(__cdecl*)()>(a)(); }
                        else if(nm=="HasCutsceneFinished"||nm=="IsCutsceneSkipButtonBeingPressed"){ kind=2; ret=reinterpret_cast<bool(__cdecl*)()>(a)(); }
                        else if(nm=="LoadAnimationUncompressed") reinterpret_cast<void(__cdecl*)(const char*)>(a)(a2%4 ? animArg : str1);
                        else if(nm=="LoadCutsceneData"||nm=="LoadCutsceneData_overlay") reinterpret_cast<void(__cdecl*)(const char*)>(a)(a2%5 ? csArg : csNames[a3%std::size(csNames)]);
                        else if(nm=="SetCutsceneAnim"||nm=="SetHeadAnim") reinterpret_cast<void(__cdecl*)(const char*,void*)>(a)(animArg,obj1);
                        else if(nm=="SetCutsceneAnimToLoop") reinterpret_cast<void(__cdecl*)(const char*)>(a)(animArg);
                        else reinterpret_cast<void(__cdecl*)()>(a)();
                    }
                    json out=Collect(); out["switches"]=switches; out["maxR"]=F(maxR);
                    out["ret"]=kind==0 ? json(nullptr) : kind==2 ? json(bool(ret&0xFF)) : kind==4 ? json(std::to_string(ret)) : json(uint32(ret));
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["recs"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
        RestoreAll();
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",StateRestored()}});
}
}
