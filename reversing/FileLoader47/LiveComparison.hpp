#pragma once
// Banco dentro del motor: CFileLoader (lista 34, 47 funciones: lectores de lineas dat/ide/ipl, cargadores de
// colision, ficheros RW, LinkLods, LoadObjectTypes/LoadScene/LoadLevel). Original (hooks de la clase desactivados,
// codigo del EXE) frente a candidato con fixtures deterministas. Nunca se lee un fichero real ni se toca un model
// info, pool o store real: las lineas se dan en memoria (CFileMgr::ReadLine/OpenFile/Read/CloseFile simulados), los
// creadores (model infos, zonas, garajes, enex, pickups, generadores, saltos, ciclos, occlusion, audio) son stubs
// trazados, los model infos son buffers falsos con una vtable falsa trazada y se colocan temporalmente en
// ms_modelInfoPtrs; los pools de TXD, enex, colision, edificios y dummies se sustituyen por pools falsos solo
// durante el caso. Se guardan y restauran por ejecucion todos los globales tocados (ms_line, el buffer de lineas,
// gString, gAtomicModelId, gpLoadedBuildings/gNumLoadedBuildings, el buffer de colision 0xBC40D8, el multiplicador
// LOD de la camara, ms_numAnimAssocDefinitions y ms_modelInfoPtrs). Los stubs no reservan memoria (traza fija).
#include <fstream>
#include <optional>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "FileLoader.h"
#include "FileObjectInstance.h"
#include "CarGenerator.h"
#include "TheCarGenerators.h"
#include "Occlusion.h"
#include "PedType.h"
#include "PedStats.h"
#include "StuntJumpManager.h"
#include "EntryExitManager.h"
#include "Garages.h"
#include "Glass.h"
#include "PlantMgr.h"
#include "ColAccel.h"
#include "ColStore.h"
#include "CullZones.h"
#include "AudioZones.h"
#include "IplStore.h"
#include "ObjectData.h"
#include "Building.h"
#include "CutsceneMgr.h"
#include "PlayerPed.h"
#include "Population.h"
#include "AEPedSpeechAudioEntity.h"
#include "AnimManager.h"
#include "cHandlingDataMgr.h"
#include "VisibilityPlugins.h"
#include "DamageAtomicModelInfo.h"
#include "Plugins/CollisionPlugin/CollisionPlugin.h"
#include "Plugins/NodeNamePlugin/NodeName.h"
#include "TimeInfo.h"
#include "TheZones.h"
#include "Pickups.h"
#include "TimeCycle.h"
#include "Train.h"
#include "LoadingScreen.h"
#include "MemoryMgr.h"
#include "ColModel.h"
#include "../ProbeRuntime.hpp"

void LinkLods(int32 numRelatedIPLs); // FileLoader.cpp (0x5B51E0)

namespace FileLoader47Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 S(const char* s){ if(!s)return 0; uint32 h=2166136261u; for(;*s;++s){h^=uint8(*s);h*=16777619u;} return h|1u; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }

// --- traza sin reservas ---
struct Ev { uint32 t, a, b; }; inline std::array<Ev,16384> evs{}; inline int nev{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nev<int(evs.size()))evs[nev++]={t,a,b}; }
inline json Trace(){ json r=json::array(); for(int i=0;i<nev;++i)r.push_back({evs[i].t,evs[i].a,evs[i].b}); return r; }
inline uint32 masks[4]{}, calls[4]{};
inline bool Bit(int m){ return (masks[m]>>(calls[m]++%32))&1; }

// --- regiones de fixtures: punteros como etiqueta+desplazamiento (estables entre ejecuciones) ---
struct Region { uintptr_t b; size_t n; uint32 tag; };
inline std::array<Region,48> regions{}; inline int nreg{};
inline void Reg(const void* b,size_t n,uint32 tag){ regions[nreg++]={uintptr_t(b),n,tag}; }
inline uint32 N(const void* p){
    if(!p)return 0; const auto v=uintptr_t(p);
    for(int i=0;i<nreg;++i)if(v>=regions[i].b&&v<regions[i].b+regions[i].n)return (regions[i].tag<<24)|uint32(v-regions[i].b);
    if(v>=0x400000&&v<0x1000000)return uint32(v); // datos del EXE (modelos de colision temporales, etc.)
    return 0xEE000000u; }

// --- fixtures ---
constexpr int NMI=14; constexpr size_t MISZ=0x400;
alignas(16) inline std::array<std::array<uint8,MISZ>,NMI> mis{};
inline std::array<uint8,NMI> miType{}, miNullAtomic{};
inline std::array<char[24],NMI> miName{};
inline std::array<uint32,NMI> miKey{};
inline std::array<int32,NMI> miId{};                    // indice en ms_modelInfoPtrs (o -1)
alignas(16) inline std::array<std::array<uint8,8>,4> otherTime{};
alignas(16) inline std::array<uint8,0x8000> arena{}; inline size_t arenaTop{};
alignas(16) inline std::array<std::array<uint8,0x40>,16> colModels{}; inline int colModelsUsed{};
alignas(16) inline std::array<std::array<uint8,0x60>,8> mats{}; inline int matsUsed{};
alignas(16) inline std::array<std::array<uint8,0x40>,8> streams{};
alignas(16) inline std::array<std::array<uint8,0x40>,8> clumps{};
alignas(16) inline std::array<std::array<uint8,0x80>,16> atomics{};
alignas(16) inline std::array<std::array<uint8,0x100>,24> frames{}; inline int framesUsed{};
alignas(16) inline std::array<std::array<uint8,0x60>,8> txds{};
alignas(16) inline std::array<std::array<uint8,0x38>,24> ents{};
alignas(16) inline std::array<CEntity*,64> iplArray{};
inline std::array<char[40],24> frameName{};
// pools falsos (cabecera CPool: objetos, mapa de estados, tamano, primer libre, propiedad, bloqueo)
struct FakePool { uint8* storage; uint8* states; int32 size; size_t stride; std::array<uint8,0x14> hdr{}; };
alignas(16) inline std::array<uint8,64*0xC> txdStorage{};   inline std::array<uint8,64> txdStates{};
alignas(16) inline std::array<uint8,32*0x3C> enexStorage{}; inline std::array<uint8,32> enexStates{};
alignas(16) inline std::array<uint8,8*0x2C> colStorage{};   inline std::array<uint8,8> colStates{};
alignas(16) inline std::array<uint8,32*0x38> bldStorage{};  inline std::array<uint8,32> bldStates{};
alignas(16) inline std::array<uint8,32*0x38> dumStorage{};  inline std::array<uint8,32> dumStates{};
inline FakePool txdPool{txdStorage.data(),txdStates.data(),64,0xC}, enexPool{enexStorage.data(),enexStates.data(),32,0x3C},
                colPool{colStorage.data(),colStates.data(),8,0x2C}, bldPool{bldStorage.data(),bldStates.data(),32,0x38}, dumPool{dumStorage.data(),dumStates.data(),32,0x38};
template<class Nx> inline void BuildPool(FakePool& p,Nx& next,int usedEvery){
    for(int32 i=0;i<p.size;++i){ for(size_t k=0;k<p.stride;++k)p.storage[i*p.stride+k]=uint8(next()); p.states[i]=uint8(usedEvery&&next()%usedEvery ? (next()&0x7F) : (0x80|(next()&0x7F))); }
    p.hdr.fill(0); At<void*>(p.hdr.data(),0)=p.storage; At<void*>(p.hdr.data(),4)=p.states; At<int32>(p.hdr.data(),8)=p.size;
    At<int32>(p.hdr.data(),0xC)=int32(next()%uint32(p.size))-1; }
inline int32 UsedSlot(FakePool& p,uint32 r){ for(int32 k=0;k<p.size;++k){ const int32 i=int32((r+uint32(k))%uint32(p.size)); if(!(p.states[i]&0x80))return i; } p.states[0]&=0x7F; return 0; }

// --- datos de fichero simulados ---
inline std::array<char[600],48> lines{}; inline int nlines{}, lineAt{};
alignas(16) inline std::array<uint8,0x2000> fileData{}; inline size_t fileSize{}, filePos{};
constexpr uintptr_t FAKE_FILE=0x00FF1230;
inline bool __cdecl FakeReadLine(FILESTREAM f,char* buf,int32 num){ Push(1,uint32(uintptr_t(f)),uint32(num)); if(lineAt>=nlines)return false;
    const char* s=lines[lineAt++]; int32 i=0; for(;s[i]&&i<num-1;++i)buf[i]=s[i]; buf[i]=0; return true; }
inline FILESTREAM __cdecl FakeOpenFile(const char* path,const char* mode){ Push(2,S(path),S(mode)); return reinterpret_cast<FILESTREAM>(FAKE_FILE); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ Push(3,uint32(uintptr_t(f))); return 0; }
// La cabecera del .col se lee en variables locales del llamador (pila): su direccion depende del marco de pila de cada
// implementacion (EXE vs DLL), asi que se enmascara; los buffers de datos (COLBUF, MS_LINE, ...) siguen trazandose.
inline uint32 NBuf(const void* p){ const uint32 v=N(p); return v<0x1000000u ? 0xEC000000u : v; }
inline size_t __cdecl FakeFileRead(FILESTREAM f,void* buf,size_t size){ Push(4,uint32(size),NBuf(buf)); size_t n=0; while(n<size&&filePos<fileSize)static_cast<uint8*>(buf)[n++]=fileData[filePos++]; return n; }

// --- model infos: vtable falsa trazada ---
inline int MiIndex(const void* p){ for(int k=0;k<NMI;++k)if(p==mis[k].data())return k; return -1; }
inline void* __fastcall Vt1(uint8* self,void*){ Push(20,N(self)); const int k=MiIndex(self); return k>=0&&miNullAtomic[k] ? nullptr : self; }
inline void* __fastcall Vt2(uint8* self,void*){ Push(21,N(self)); return self; }
inline uint8 __fastcall Vt4(uint8* self,void*){ Push(22,N(self)); const int k=MiIndex(self); return k>=0 ? miType[k] : 0; }
inline void* __fastcall Vt5(uint8* self,void*){ Push(23,N(self)); return self+0x20; }
inline void __fastcall Vt12(uint8* self,void*,const char* s){ Push(24,N(self),S(s)); }
inline void __fastcall Vt13(uint8* self,void*){ Push(25,N(self)); }
inline void __fastcall Vt15(uint8* self,void*,void* a){ Push(26,N(self),N(a)); }
inline void __fastcall Vt16(uint8* self,void*,void* c){ Push(27,N(self),N(c)); }
inline void __fastcall VtTrap(uint8* self,void*){ Push(29,N(self)); }
inline std::array<void*,24> fakeVt{};
inline void BuildVt(){ for(auto& v:fakeVt)v=reinterpret_cast<void*>(&VtTrap); fakeVt[1]=reinterpret_cast<void*>(&Vt1); fakeVt[2]=reinterpret_cast<void*>(&Vt2);
    fakeVt[4]=reinterpret_cast<void*>(&Vt4); fakeVt[5]=reinterpret_cast<void*>(&Vt5); fakeVt[12]=reinterpret_cast<void*>(&Vt12); fakeVt[13]=reinterpret_cast<void*>(&Vt13);
    fakeVt[15]=reinterpret_cast<void*>(&Vt15); fakeVt[16]=reinterpret_cast<void*>(&Vt16); }
inline const char* const nameBank[]{"lamppost1","cj_bench","LODbridge","trafficlight","ws_ref_bollard","des_cactus","sw_logs","cs_rock","barrier","hydrant","sfn_tree","dam_wall","ap_jet","kb_bed"};
template<class Nx> inline void BuildMis(Nx& next,uint8 forceType=0){
    for(int k=0;k<NMI;++k){ auto& m=mis[k]; for(auto& b:m)b=uint8(next()); At<void*>(m.data(),0)=fakeVt.data();
        strcpy(miName[k],nameBank[k]); miKey[k]=CKeyGen::GetUppercaseKey(miName[k]); At<uint32>(m.data(),4)=next()%4 ? miKey[k] : next();
        static const uint8 types[]{1,1,3,5,6,7}; miType[k]=forceType ? forceType : types[next()%std::size(types)]; miNullAtomic[k]=0;
        m[0x13]&=uint8(~0x30); m[0x13]|=0x08;                     // tipo especial 1 (nunca etiqueta: CTagManager queda fuera)
        m[0xD]=0; At<int16>(m.data(),0xE)=-1;                     // sin 2dfx (HasPreRenderEffects los recorre en SetModelIndexNoCreate)
        At<void*>(m.data(),0x14)=next()%3 ? colModels[k%colModels.size()].data() : nullptr; } }
inline void* __cdecl FakeAddModel(int32 id){ Push(30,uint32(id)); return mis[uint32(id)%NMI].data(); }
inline void* __cdecl FakeAddDamage(int32 id){ Push(31,uint32(id)); return mis[uint32(id+5)%NMI].data(); }
inline void __fastcall FakeSetTexDict(uint8* self,void*,const char* n){ Push(32,N(self),S(n)); }
inline void __fastcall FakeSetColModel(uint8* self,void*,CColModel* c,bool lod){ Push(33,N(self),N(c)|(lod?0x80000000u:0)); }
inline uint8* timeResult{};
inline void* __fastcall FakeFindOtherTime(uint8* self,void*,const char* n){ Push(34,N(self),S(n)); return timeResult; }
inline int32 txdFindResult{}, txdAddResult{};
inline int32 __cdecl FakeFindTxd(const char* n){ Push(35,S(n)); return Bit(0) ? txdFindResult : -1; }
inline int32 __cdecl FakeAddTxd(const char* n){ Push(36,S(n)); return txdAddResult; }
inline int32 __fastcall FakeHandlingId(void* self,void*,const char* n){ Push(37,uint32(uintptr_t(self)),S(n)); return int32(S(n)%210); }
inline uint32 numGroups{};
inline const char* const groupNames[]{"default","player","fatman","man","shuffle","oldman","gang1","gang2","oldfatman","fatwoman","jogger","woman","sexywoman","busywoman"};
inline const char* __cdecl FakeGroupName(int32 i){ Push(38,uint32(i)); return groupNames[uint32(i)%std::size(groupNames)]; }
inline int32 __cdecl FakeFindPedType(const char* n){ Push(39,S(n)); return int32(S(n)%32); }
inline int32 __cdecl FakePedStat(const char* n){ Push(40,S(n)); return int32(S(n)%43); }
inline int32 __cdecl FakeRace(const char* n){ Push(41,S(n)); return int32(S(n)%6); }
inline int32 __cdecl FakeAudioType(const char* n){ Push(42,S(n)); return int32(S(n)%5); }
inline int32 __cdecl FakeGetVoice(const char* n,int16 t){ Push(43,S(n),uint32(uint16(t))); return int32(S(n)%400); }

// --- creadores del IPL ---
inline void __cdecl FakeCreateZone(const char* n,eZoneType t,CVector a,CVector b,eLevelName l,const char* g){ Push(50,S(n),uint32(t)); Push(51,F(a.x),F(a.y)); Push(51,F(a.z),F(b.x)); Push(51,F(b.y),F(b.z)); Push(52,uint32(l),S(g)); }
inline void __cdecl FakeMirrorZone(const CVector& p,float a,float b,float c,float d,float e,float f,eZoneAttributes fl,float v,float x,float y,float z){
    Push(53,F(p.x),F(p.y)); Push(53,F(p.z),F(a)); Push(53,F(b),F(c)); Push(53,F(d),F(e)); Push(53,F(f),uint32(fl)); Push(53,F(v),F(x)); Push(53,F(y),F(z)); }
inline void __cdecl FakeCullZone(const CVector& p,float a,float b,float c,float d,float e,float f,uint16 fl){ Push(54,F(p.x),F(p.y)); Push(54,F(p.z),F(a)); Push(54,F(b),F(c)); Push(54,F(d),F(e)); Push(54,F(f),fl); }
inline void __cdecl FakeOcclusion(float a,float b,float c,float d,float e,float f,float g,float h,float i,uint32 fl,bool in){ Push(55,F(a),F(b)); Push(55,F(c),F(d)); Push(55,F(e),F(f)); Push(55,F(g),F(h)); Push(55,F(i),fl); Push(55,in); }
inline void __cdecl FakeAudioBox(char* n,int32 id,bool on,CVector a,CVector b){ Push(56,S(n),uint32(id)); Push(56,on,F(a.x)); Push(56,F(a.y),F(a.z)); Push(56,F(b.x),F(b.y)); Push(56,F(b.z)); }
inline void __cdecl FakeAudioSphere(char* n,int32 id,bool on,CVector a,float r){ Push(57,S(n),uint32(id)); Push(57,on,F(a.x)); Push(57,F(a.y),F(a.z)); Push(57,F(r)); }
inline int32 enexResult{};
inline int32 __cdecl FakeEnex(float a,float b,float c,float d,float e,float f,float g,float h,float i,float j,float k,eAreaCodes area,CEntryExit::eFlags fl,int32 sky,int32 on,int32 off,int32 peds,const char* n){
    Push(58,F(a),F(b)); Push(58,F(c),F(d)); Push(58,F(e),F(f)); Push(58,F(g),F(h)); Push(58,F(i),F(j)); Push(58,F(k),uint32(area)); Push(58,uint32(fl),uint32(sky)); Push(58,uint32(on),uint32(off)); Push(58,uint32(peds),S(n)); return enexResult; }
inline void __cdecl FakeGarage(float a,float b,float c,float d,float e,float f,float g,float h,uint8 t,uint32 x,char* n,uint32 fl){ Push(59,F(a),F(b)); Push(59,F(c),F(d)); Push(59,F(e),F(f)); Push(59,F(g),F(h)); Push(59,t,x); Push(59,S(n),fl); }
inline int32 __cdecl FakeStunt(const CBoundingBox& s,const CBoundingBox& e,const CVector& c,int32 r){ Push(60,uint32(Fnv(&s,sizeof(s))),uint32(Fnv(&e,sizeof(e)))); Push(60,F(c.x),F(c.y)); Push(60,F(c.z),uint32(r)); return 0; }
inline int32 carGenResult{};
inline int32 __cdecl FakeCreateCarGen(CVector p,float ang,int32 mid,int16 c1,int16 c2,uint8 fs,uint8 al,uint8 dl,uint16 mn,uint16 mx,uint8 ipl,uint8 ign){
    Push(61,F(p.x),F(p.y)); Push(61,F(p.z),F(ang)); Push(61,uint32(mid),uint32(uint16(c1))|uint32(uint16(c2))<<16); Push(61,fs|al<<8|dl<<16|uint32(ipl)<<24,mn|uint32(mx)<<16); Push(61,ign); return carGenResult; }
inline void __fastcall FakeSwitchOn(uint8* self,void*){ Push(62,uint32(uintptr_t(self)-0xC27AD0)); }
inline int32 __cdecl FakePickup(CVector p,uint32 mid,uint8 type,uint32 ammo,uint32 money,bool empty,char* msg){ Push(63,F(p.x),F(p.y)); Push(63,F(p.z),mid); Push(63,type,ammo); Push(63,money,empty); Push(63,N(msg)); return 0; }
inline void __cdecl FakeTimeCycle(CBox& box,int16 fc,int32 col,float st,float fo,float lod){ Push(64,uint32(Fnv(&box,sizeof(CBox))),uint32(uint16(fc))); Push(64,uint32(col),F(st)); Push(64,F(fo),F(lod)); }
inline void* instResult{};
inline CEntity* __cdecl FakeLoadInst(CFileObjectInstance* inst,const char* n){ Push(65,uint32(Fnv(inst,0x28)),S(n)); return static_cast<CEntity*>(instResult); }

// --- RenderWare y ficheros RW ---
inline RwStream* streamResult{}; inline int findChunkBudget{};
inline RwStream* __cdecl FakeStreamOpen(int32 t,int32 a,const void* d){ Push(70,uint32(t)|uint32(a)<<8,S(static_cast<const char*>(d))); return streamResult; }
inline RwBool __cdecl FakeStreamClose(RwStream* s,void* d){ Push(71,N(s),N(d)); return TRUE; }
inline RwBool __cdecl FakeFindChunk(RwStream* s,RwUInt32 t,RwUInt32* l,RwUInt32* v){ Push(72,N(s),t); if(findChunkBudget<=0)return FALSE; --findChunkBudget; return Bit(1); }
inline int clumpsRead{};
inline RpClump* __cdecl FakeClumpRead(RwStream* s){ Push(73,N(s)); if(!Bit(2))return nullptr; return reinterpret_cast<RpClump*>(clumps[uint32(clumpsRead++)%clumps.size()].data()); }
inline RpAtomicCallBack exeSetRelated{}, dllSetRelated{}, exeFindRelated{}, dllFindRelated{};
inline uint32 Cb(const void* p){
    const auto v=uintptr_t(p);
    if(v==0x537150||v==uintptr_t(dllSetRelated))return 1; if(v==0x5B3930||v==uintptr_t(dllFindRelated))return 2;
    if(v==0x537290||v==std::bit_cast<uintptr_t>(&CloneAtomicToClumpCB))return 3; if(v==0x5B38F0||v==std::bit_cast<uintptr_t>(&AddTextureCB))return 4;
    return N(p); }
inline bool invokeCallbacks{};
inline RpClump* __cdecl FakeForAllAtomics(RpClump* c,RpAtomicCallBack cb,void* d){
    Push(74,N(c),Cb(reinterpret_cast<const void*>(cb))); Push(74,N(d));
    if(invokeCallbacks){ const int k=int((uintptr_t(c)-uintptr_t(clumps.data()))/0x40); const int na=int(clumps[k][0x10]%4);
        for(int i=0;i<na;++i){ auto* a=reinterpret_cast<RpAtomic*>(atomics[(k*2+i)%atomics.size()].data()); auto* r=cb(a,d); Push(75,N(r)); } }
    return c; }
inline RwBool __cdecl FakeClumpDestroy(RpClump* c){ Push(76,N(c)); return TRUE; }
inline RwTexDictionary* txdReadResult{}; inline RwTexDictionary* txdCreateResult{}; inline RwTexDictionary* txdCurrent{};
inline RwTexDictionary* __cdecl FakeTxdRead(RwStream* s){ Push(77,N(s)); return txdReadResult; }
inline RwTexDictionary* __cdecl FakeTxdCreate(){ Push(78); return txdCreateResult; }
inline const RwTexDictionary* __cdecl FakeForAllTextures(const RwTexDictionary* d,RwTextureCallBack cb,void* data){ Push(79,N(d),Cb(reinterpret_cast<const void*>(cb))); Push(79,N(data)); return d; }
inline RwTexDictionary* __cdecl FakeTxdGetCurrent(){ Push(80); return txdCurrent; }
inline RwTexDictionary* __cdecl FakeTxdSetCurrent(RwTexDictionary* d){ Push(81,N(d)); txdCurrent=d; return d; }
inline RwBool __cdecl FakeTxdDestroy(RwTexDictionary* d){ Push(82,N(d)); return TRUE; }
inline bool __cdecl FakeGtaRead1(RwStream* s){ Push(83,N(s)); return Bit(2); }
inline RpClump* __cdecl FakeGtaRead2(RwStream* s){ Push(84,N(s)); return Bit(2) ? reinterpret_cast<RpClump*>(clumps[3].data()) : nullptr; }
inline void __cdecl FakeUseCommon(){ Push(85); }
inline void __cdecl FakeStopCommon(){ Push(86); }
inline void __cdecl FakeColPluginSet(void* mi){ Push(87,N(mi)); }
inline RpClump* __cdecl FakeClumpCreate(){ Push(88); return reinterpret_cast<RpClump*>(clumps[7].data()); }
inline RwFrame* __cdecl FakeFrameCreate(){ Push(89); return reinterpret_cast<RwFrame*>(frames[8+uint32(framesUsed++)%16].data()); }
inline RwFrame* __cdecl FakeFrameClone(RwFrame* f){ Push(90,N(f)); return reinterpret_cast<RwFrame*>(frames[8+uint32(framesUsed++)%16].data()); }
inline RwFrame* __cdecl FakeFrameAddChild(RwFrame* p,RwFrame* c){ Push(91,N(p),N(c)); return p; }
inline const char* __cdecl FakeNodeName(RwFrame* f){ Push(92,N(f)); for(int k=0;k<int(frames.size());++k)if(f==reinterpret_cast<RwFrame*>(frames[k].data()))return frameName[k]; return ""; }
inline void* __cdecl FakeGetMiByName(const char* n,int32* idx){ Push(93,S(n),idx ? 1u : 0u); for(int k=0;k<NMI;++k)if(miId[k]!=-2&&!_stricmp(n,miName[k])){ if(idx)*idx=miId[k]>=0 ? miId[k] : 30000+k; return mis[k].data(); } return nullptr; }
inline void* __cdecl FakeGetMiByNameRange(const char* n,int32 lo,int32 hi){ Push(94,S(n),uint32(lo)^(uint32(hi)<<16)); for(int k=0;k<NMI;++k)if(miId[k]!=-2&&!_stricmp(n,miName[k]))return mis[k].data(); return nullptr; }
inline void __cdecl FakeRenderCb(RpAtomic* a,void* cb){ Push(95,N(a),N(cb)); }
inline RpClump* __cdecl FakeRemoveAtomic(RpClump* c,RpAtomic* a){ Push(96,N(c),N(a)); return c; }
inline RpAtomic* __cdecl FakeAtomicSetFrame(RpAtomic* a,RwFrame* f){ Push(97,N(a),N(f)); return a; }
inline void __cdecl FakeMiIndex(RpAtomic* a,int32 i){ Push(98,N(a),uint32(i)); }
inline void __fastcall FakeSetDamaged(uint8* self,void*,RpAtomic* a){ Push(99,N(self),N(a)); }

// --- colision ---
inline void* __cdecl FakeMalloc(uint32 size,uint32 hint){ Push(100,size,hint); arenaTop=(arenaTop+15)&~size_t(15); if(uint64_t(arenaTop)+uint64_t(size)>uint64_t(arena.size())){ arenaTop=0; return nullptr; } void* r=arena.data()+arenaTop; arenaTop+=size; return r; }
inline void* __cdecl FakeMalloc1(uint32 size){ return FakeMalloc(size,0xDEAD); }
inline void* __cdecl FakeNew(size_t size){ Push(101,uint32(size)); return FakeMalloc(uint32(size),0xBEEF); }
inline void* __cdecl FakeColModelNew(unsigned size){ Push(102,size); return colModels[8+uint32(colModelsUsed++)%8].data(); }
inline bool __cdecl FakePlantFlag(void* mi){ Push(103,N(mi)); return true; }
inline void __cdecl FakeIncludeModel(int32 slot,int32 id){ Push(104,uint32(slot),uint32(id)); }
inline void __cdecl FakeAddCacheCol(int32 packed,const CColModel& cm){ Push(105,uint32(packed),N(&cm)); }

// --- LinkLods ---
inline bool __cdecl FakeIsCacheLoading(){ const bool r=Bit(3); Push(110,r); return r; }
inline void __cdecl FakeAddIplEntity(CEntity** e,int32 n,int32 i){ Push(111,N(e),uint32(n)); Push(111,uint32(i)); }
inline void __cdecl FakeCacheIplSection(CEntity** e,int32 n){ Push(112,N(e),uint32(n)); }
inline void __fastcall FakeSetupBig(uint8* self,void*){ Push(113,N(self)); }
inline void __fastcall FakeDeleteCol(uint8* self,void*){ Push(114,N(self)); }
inline void __cdecl FakeWorldAdd(CEntity* e){ Push(115,N(e)); }

// --- LoadObjectInstance (instancia) ---
inline uint32 rngState{};
inline int __cdecl FakeRand(){ rngState=rngState*1103515245u+12345u; Push(120); return int(rngState>>16)&0x7FFF; }
inline uint16 FakeRandomNumber(){ return uint16(FakeRand()); }
inline bool __cdecl FakeIsGlass(CEntity* e){ const bool r=Bit(0); Push(121,N(e),r); return r; }
inline void __fastcall FakeAllocStatic(uint8* self,void*){ auto& m=At<uint8*>(self,0x14); Push(122,N(self),N(m)); if(!m){ m=mats[uint32(matsUsed++)%mats.size()].data(); At<void*>(m,0x48)=self; } }
inline void __fastcall FakeAllocMatrix(uint8* self,void*){ auto& m=At<uint8*>(self,0x14); Push(123,N(self),N(m)); if(!m){ m=mats[uint32(matsUsed++)%mats.size()].data(); At<void*>(m,0x48)=self; } }
inline void __fastcall FakeUpdateMatrix(uint8* self,void*,void* m){ Push(124,uint32(Fnv(self,0x10)),N(m)); }
inline void __fastcall FakeSetRotateQuat(uint8* self,void*,const CQuaternion& q){ Push(125,N(self),F(q.x)); Push(125,F(q.y),F(q.z)); Push(125,F(q.w)); }
inline void __fastcall FakeSetRotateZ(uint8* self,void*,float a){ Push(126,N(self),F(a)); }
inline void __cdecl FakeTrainCrossing(CMatrix* m,float a){ Push(127,N(m),F(a)); }

// --- LoadObjectTypes / LoadScene / LoadLevel: lectores sustituidos ---
inline int32 loaderSeq{};
template<int K> inline int32 __cdecl LineStub(const char* l){ Push(130,K,S(l)); return (loaderSeq++%5)==0 ? -1 : int32(K*100+loaderSeq); }
inline int32 __cdecl PathHeaderStub(const char* l,int32& type){ Push(131,S(l)); type=int32(S(l)%4); return int32(S(l)%1000); }
inline void __cdecl PedNodeStub(const char* l,int32 a,int32 b){ Push(132,S(l),uint32(a)); Push(132,uint32(b)); }
inline void __cdecl CarNodeStub(const char* l,int32 a,int32 b,bool c){ Push(133,S(l),uint32(a)); Push(133,uint32(b),c); }
inline void __cdecl OcclStub(const char* l,const char* f){ Push(134,S(l),S(f)); }
inline void __cdecl CarGenStub(const char* l,int32 ipl){ Push(135,S(l),uint32(ipl)); }
inline int32 instSeq{};
inline CEntity* __cdecl InstStub(const char* l){ Push(136,S(l)); return reinterpret_cast<CEntity*>(uintptr_t(0x00F00000+(instSeq++)*4)); }
inline int32 __cdecl NewIplArrayStub(int32 n){ Push(137,uint32(n)); return 7; }
inline CEntity** __cdecl GetIplArrayStub(int32 i){ Push(138,uint32(i)); return iplArray.data(); }
inline int32 __cdecl SetupRelatedStub(const char* n,int32 i,CEntity** p){ Push(139,S(n),uint32(i)); Push(139,N(p)); return 2; }
inline void __cdecl LinkLodsStub(int32 n){ Push(140,uint32(n)); }
inline void __cdecl RemoveRelatedStub(int32 i){ Push(141,uint32(i)); }
template<int K> inline void __cdecl NameStub(const char* n){ Push(142,K,S(n)); }
template<int K> inline void __cdecl VoidStub(){ Push(143,K); }
inline RwTexDictionary* __cdecl LoadTxdStub(const char* n){ Push(144,S(n)); return reinterpret_cast<RwTexDictionary*>(txds[5].data()); }
inline void __cdecl ColFileStub(const char* n,uint8 id){ Push(145,S(n),id); }
inline uint32 __cdecl AddImageStub(const char* n,bool b){ Push(146,S(n),b); return 0; }
inline void __cdecl LoadingScreenStub(const char* a,const char* b,const char* c){ Push(147,S(a),S(b)); Push(147,S(c)); }
inline void __cdecl LoadingFileStub(const char* s){ Push(148,S(s)); }
inline void __cdecl ObjectDataStub(const char* s){ Push(149,S(s)); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,47> fns{{
    {"CFileLoader","LoadLine-File",0x536F80,0,-1,512},{"CFileLoader","LoadLine-Bufer",0x536FE0,0,-1,512},
    {"CFileLoader","LoadObject",0x5B3C60,0,-1,512},{"CFileLoader","LoadTimeObject",0x5B3DE0,0,-1,512},
    {"CFileLoader","LoadWeaponObject",0x5B3FB0,0,-1,256},{"CFileLoader","LoadClumpObject",0x5B4040,0,-1,256},
    {"CFileLoader","LoadAnimatedClumpObject",0x5B40C0,0,-1,256},{"CFileLoader","LoadVehicleObject",0x5B6F30,0,-1,512},
    {"CFileLoader","LoadPedObject",0x5B7420,0,-1,256},{"CFileLoader","LoadTXDParent",0x5B75E0,0,-1,256},
    {"CFileLoader","LoadPathHeader",0x5B41C0,0,-1,128},{"CFileLoader","LoadPedPathNode",0x5B41F0,0,-1,64},
    {"CFileLoader","LoadCarPathNode",0x5B4380,0,-1,64},{"CFileLoader","LoadGarage",0x5B4530,0,-1,256},
    {"CFileLoader","LoadStuntJump",0x5B45D0,0,-1,128},{"CFileLoader","LoadCarGenerator-1",0x5B4740,0,-1,256},
    {"CFileLoader","LoadCarGenerator-0",0x537990,0,-1,256},{"CFileLoader","LoadPickup",0x5B47B0,0,-1,256},
    {"CFileLoader","LoadZone",0x5B4AB0,0,-1,256},{"CFileLoader","LoadCullZone",0x5B4B40,0,-1,256},
    {"CFileLoader","LoadOcclusionVolume",0x5B4C80,0,-1,256},{"CFileLoader","LoadAudioZone",0x5B4D70,0,-1,256},
    {"CFileLoader","LoadEntryExit",0x5B8030,0,-1,256},{"CFileLoader","LoadTimeCyclesModifier",0x5B81D0,0,-1,256},
    {"CFileLoader","LoadObjectInstance-file",0x538690,0,-1,256},{"CFileLoader","LoadObjectInstance-inst",0x538090,0,-1,512},
    {"CFileLoader","FindRelatedModelInfoCB",0x5B3930,0,-1,256},{"CFileLoader","SetRelatedModelInfoCB",0x537150,0,-1,256},
    {"CFileLoader","LoadAtomicFile-stream",0x5371F0,0,-1,256},{"CFileLoader","LoadAtomicFile-",0x5B39D0,0,-1,128},
    {"CFileLoader","LoadClumpFile-1",0x5372D0,0,-1,256},{"CFileLoader","LoadClumpFile-",0x5B3A30,0,-1,128},
    {"CFileLoader","StartLoadClumpFile",0x5373F0,0,-1,128},{"CFileLoader","FinishLoadClumpFile",0x537450,0,-1,128},
    {"CFileLoader","LoadTexDictionary",0x5B3860,0,-1,128},{"CFileLoader","AddTexDictionaries",0x5B3910,0,-1,64},
    {"CFileLoader","LoadCollisionModel",0x537580,0,-1,512},{"CFileLoader","LoadCollisionModelVer2",0x537EE0,0,-1,256},
    {"CFileLoader","LoadCollisionModelVer3",0x537CE0,0,-1,256},{"CFileLoader","LoadCollisionModelVer4",0x537AE0,0,-1,256},
    {"CFileLoader","LoadCollisionFile-Buffer",0x538440,0,-1,256},{"CFileLoader","LoadCollisionFileFirstTime",0x5B5000,0,-1,256},
    {"CFileLoader","LoadCollisionFile-File",0x5B4E60,0,-1,256},{"CFileLoader","LinkLods",0x5B51E0,0,-1,256},
    {"CFileLoader","LoadObjectTypes",0x5B8400,0,-1,128},{"CFileLoader","LoadScene",0x5B8700,0,-1,128},
    {"CFileLoader","LoadLevel",0x5B9030,0,-1,128},
}};

// --- generador de lineas ---
struct LB {
    char* p; int n{};
    void Sep(){ if(n&&n<590)p[n++]=' '; p[n]=0; }
    void Str(const char* s){ Sep(); n+=snprintf(p+n,size_t(600-n),"%s",s); }
    void I(int32 v){ Sep(); n+=snprintf(p+n,size_t(600-n),"%d",v); }
    void X(uint32 v){ Sep(); n+=snprintf(p+n,size_t(600-n),"%x",v); }
    void Fx(int32 hundredths){ Sep(); n+=snprintf(p+n,size_t(600-n),"%.2f",double(hundredths)/100.0); }
};
inline const char alpha[]="abcdefghijklmnopqrstuvwxyz0123456789_";
template<class Nx> inline void Nm(Nx& next,char* o,int maxLen){ const int len=1+int(next()%uint32(maxLen)); for(int i=0;i<len;++i)o[i]=alpha[next()%(sizeof(alpha)-1)]; o[len]=0; }
template<class Nx> inline int32 Fv(Nx& next){ switch(next()%4){ case 0: return int32(next()%2001)-1000; case 1: return int32(next()%400001)-200000; case 2: return int32(next()%21)*25; default: return int32(next()%100001)-50000; } }

// --- estado global del juego tocado por el banco ---
struct Blob { uintptr_t p; size_t n; std::vector<uint8> saved; };
constexpr uintptr_t MS_LINE=0xB71848, HEAPBUF=0xC8E0C8, GSTRING=0xB71670, GATOMIC=0xB71840, GNUM=0xBCC0D8, GLOADED=0xBCC0E0, COLBUF=0xBC40D8, LODMULT=0xB6F118,
                    NUMGROUPS=0xB4EA28, MIPTRS=0xA9B0C8, TXDPOOLP=0xC8800C, ENEXPOOLP=0x96A7D8, COLPOOLP=0x965560, BLDPOOLP=0xB74498, DUMPOOLP=0xB744A0;
constexpr int32 FIRST_ID=1900;
struct Pair { uintptr_t exe, dll; };

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"fileloader47-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"fileloader47-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("fileloader47");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; int onlyCase=-1; { std::ifstream f(dir/"fileloader47-only.txt"); if(f){ std::getline(f,only); std::string c; if(std::getline(f,c)&&!c.empty())onlyCase=std::atoi(c.c_str()); } } // 2.a linea opcional: numero de caso a repetir
    std::array<ReversibleHooks::ReversibleHook::Base*,47> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,47> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    using std::bit_cast;
    // vtables exportadas por la DLL (NOTSA_EXPORT_VTABLE) para normalizar el vptr de las entidades creadas
    HMODULE self{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&Update),&self);
    const uintptr_t dllVtBld=uintptr_t(GetProcAddress(self,"??_7CBuilding@@6B@")), dllVtAnim=uintptr_t(GetProcAddress(self,"??_7CAnimatedBuilding@@6B@")), dllVtDum=uintptr_t(GetProcAddress(self,"??_7CDummyObject@@6B@"));
    auto VtClass=[&](uintptr_t v)->json{ if(v==0x8585C8||(dllVtBld&&v==dllVtBld))return "building"; if(v==0x8639B0||(dllVtAnim&&v==dllVtAnim))return "animated"; if(v==0x866E78||(dllVtDum&&v==dllVtDum))return "dummy"; return "other"; };

    // Cuerpos de la DLL resueltos ANTES de escribir ningun parche
    auto B=[](auto fp){ return Body(bit_cast<uintptr_t>(fp)); };
    const Pair pReadLine{0x5389B0,B(&CFileMgr::ReadLine)}, pOpen{0x538900,B(&CFileMgr::OpenFile)}, pRead{0x538950,B(&CFileMgr::Read)}, pClose{0x5389D0,B(&CFileMgr::CloseFile)};
    const Pair pAddAtomic{0x4C6620,B(&CModelInfo::AddAtomicModel)}, pAddDamage{0x4C6650,B(&CModelInfo::AddDamageAtomicModel)}, pAddTime{0x4C66B0,B(&CModelInfo::AddTimeModel)},
               pAddWeapon{0x4C6710,B(&CModelInfo::AddWeaponModel)}, pAddClump{0x4C6740,B(&CModelInfo::AddClumpModel)}, pAddVehicle{0x4C6770,B(&CModelInfo::AddVehicleModel)}, pAddPed{0x4C67A0,B(&CModelInfo::AddPedModel)};
    const Pair pSetTex{0x4C4B40,B(&CBaseModelInfo::SetTexDictionary)}, pSetCol{0x4C4BC0,B(&CBaseModelInfo::SetColModel)}, pOtherTime{0x4C47E0,B(&CTimeInfo::FindOtherTimeModel)};
    const Pair pFindTxd{0x731850,B(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot))}, pAddTxd{0x731C80,B(&CTxdStore::AddTxdSlot)}, pHandling{0x6F4FD0,B(&cHandlingDataMgr::GetHandlingId)};
    const Pair pGroupName{0x4D3A20,B(&CAnimManager::GetAnimGroupName)}, pPedType{0x608790,B(&CPedType::FindPedType)}, pPedStat{0x6088D0,B(&CPedStats::GetPedStatType)},
               pRace{0x5B6D40,B(&CPopulation::FindPedRaceFromName)}, pAudioType{0x4E3C60,B(&CAEPedSpeechAudioEntity::GetAudioPedType)}, pVoice{0x4E3CD0,B(&CAEPedSpeechAudioEntity::GetVoice)};
    const Pair pZone{0x5728A0,B(&CTheZones::CreateZone)}, pMirror{0x72DC10,B(&CCullZones::AddMirrorAttributeZone)}, pCull{0x72DF70,B(&CCullZones::AddCullZone)}, pOccl{0x71DCD0,B(&COcclusion::AddOne)},
               pABox{0x508240,B(&CAudioZones::RegisterAudioBox)}, pASphere{0x5081C0,B(&CAudioZones::RegisterAudioSphere)}, pEnex{0x43FA00,B(&CEntryExitManager::AddOne)}, pGarage{0x4471E0,B(&CGarages::AddOne)},
               pStunt{0x49CB40,B(&CStuntJumpManager::AddOne)}, pCarGen{0x6F31A0,B(&CTheCarGenerators::CreateCarGenerator)}, pSwitchOn{0x6F32C0,B(&CCarGenerator::SwitchOn)}, pPickup{0x456F20,B(&CPickups::GenerateNewOne_ExeAbi)} /* CPickups::GenerateNewOne (DLL) es un envoltorio (tPickupReference por puntero oculto) que llama a GenerateNewOne_ExeAbi (cdecl, int32), el cuerpo enganchado en 0x456F20: se falsean ambos, igual que PickupsGenNew28 */,
               pTimeCycle{0x55FF40,B(&CTimeCycle::AddOne)}, pInst{0x538090,B(static_cast<CEntity*(*)(CFileObjectInstance*,const char*)>(&CFileLoader::LoadObjectInstance))};
    const Pair pUseCommon{0x4C75A0,B(&CVehicleModelInfo::UseCommonVehicleTexDicationary)}, pStopCommon{0x4C75C0,B(&CVehicleModelInfo::StopUsingCommonVehicleTexDicationary)},
               pColPlugin{0x41B350,B(&CCollisionPlugin::SetModelInfo)}, pNodeName{0x72FB30,B(&GetFrameNodeName)},
               pMiByName{0x4C5940,B(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo))}, pMiByRange{0x4C5A20,B(static_cast<CBaseModelInfo*(*)(const char*,int32,int32)>(&CModelInfo::GetModelInfo))},
               pRenderCb{0x7328A0,B(&CVisibilityPlugins::SetAtomicRenderCallback)}, pMiIndex{0x732230,B(&CVisibilityPlugins::SetModelInfoIndex)}, pDamaged{0x4C48D0,B(&CDamageAtomicModelInfo::SetDamagedAtomic)};
    const Pair pMalloc{0x72F420,B(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc))}, pMalloc1{0,B(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc))}, pNew{0x82119A,0},
               pColNew{0x40FC30,B(&CColModel::operator new)}, pPlant{0x5DB650,B(&CPlantMgr::SetPlantFriendlyFlagInAtomicMI)}, pInclude{0x410820,B(&CColStore::IncludeModelIndex)}, pCacheCol{0x5B2C20,B(&CColAccel::addCacheCol)};
    const Pair pCacheLoading{0x5B2AC0,B(&CColAccel::isCacheLoading)}, pAddIpl{0x5B3040,B(&CColAccel::addIPLEntity)}, pCacheIpl{0x5B2F10,B(&CColAccel::cacheIPLSection)},
               pSetupBig{0x533150,B(&CEntity::SetupBigBuilding)}, pDeleteCol{0x4C4C40,B(&CBaseModelInfo::DeleteCollisionModel)}, pWorldAdd{0x563220,B(static_cast<void(*)(CEntity*)>(&CWorld::Add))};
    const Pair pRand{0x821B1E,0}, pRandom{0,B(&CGeneral::GetRandomNumber)}, pGlass{0x46A760,B(&CGlass::IsObjectGlass)}, pAllocStatic{0x54F4C0,B(&CPlaceable::AllocateStaticMatrix)}, pAllocMatrix{0x54F560,B(&CPlaceable::AllocateMatrix)},
               pUpdateMatrix{0x54F1B0,B(&CSimpleTransform::UpdateMatrix)}, pSetRotate{0x59BBF0,B(static_cast<void(CMatrix::*)(const CQuaternion&)>(&CMatrix::SetRotate))}, pSetRotateZ{0x59B020,B(&CMatrix::SetRotateZOnly)},
               pTrain{0x59F200,B(&CObject::SetMatrixForTrainCrossing)};
    const Pair lObject{0x5B3C60,B(&CFileLoader::LoadObject)}, lTime{0x5B3DE0,B(&CFileLoader::LoadTimeObject)}, lWeapon{0x5B3FB0,B(&CFileLoader::LoadWeaponObject)}, lClump{0x5B4040,B(&CFileLoader::LoadClumpObject)},
               lAnim{0x5B40C0,B(&CFileLoader::LoadAnimatedClumpObject)}, lVehicle{0x5B6F30,B(&CFileLoader::LoadVehicleObject)}, lPed{0x5B7420,B(&CFileLoader::LoadPedObject)}, lPath{0x5B41C0,B(&CFileLoader::LoadPathHeader)},
               lPedNode{0x5B41F0,B(&CFileLoader::LoadPedPathNode)}, lCarNode{0x5B4380,B(&CFileLoader::LoadCarPathNode)}, l2dfx{0x5B7670,B(&CFileLoader::Load2dEffect)} /* desde de6ed4a4d Load2dEffect es un cuerpo C++ enganchado en 0x5B7670 (antes un plugin::Call al EXE): LoadObjectTypes de la DLL lo llama directo y hay que falsear tambien el cuerpo */, lTxdp{0x5B75E0,B(&CFileLoader::LoadTXDParent)},
               lInstFile{0x538690,B(static_cast<CEntity*(*)(const char*)>(&CFileLoader::LoadObjectInstance))}, lZone{0x5B4AB0,B(&CFileLoader::LoadZone)}, lCull{0x5B4B40,B(&CFileLoader::LoadCullZone)},
               lOccl{0x5B4C80,B(&CFileLoader::LoadOcclusionVolume)}, lAudio{0x5B4D70,B(&CFileLoader::LoadAudioZone)}, lEnex{0x5B8030,B(&CFileLoader::LoadEntryExit)}, lGarage{0x5B4530,B(&CFileLoader::LoadGarage)},
               lPickup{0x5B47B0,B(&CFileLoader::LoadPickup)}, lCarGen{0x5B4740,B(static_cast<void(*)(const char*,int32)>(&CFileLoader::LoadCarGenerator))}, lJump{0x5B45D0,B(&CFileLoader::LoadStuntJump)},
               lTcyc{0x5B81D0,B(&CFileLoader::LoadTimeCyclesModifier)}, pNewIpl{0x404780,B(&CIplStore::GetNewIplEntityIndexArray)}, pGetIpl{0x4047B0,B(&CIplStore::GetIplEntityIndexArray)},
               pSetupRelated{0x404DE0,B(&CIplStore::SetupRelatedIpls)}, pRemoveRelated{0x405110,B(&CIplStore::RemoveRelatedIpls)}, pLinkLods{0x5B51E0,B(&LinkLods)};
    const Pair lvTxd{0x5B3860,B(&CFileLoader::LoadTexDictionary)}, lvScreen{0x53DED0,B(static_cast<void(*)(const char*,const char*,const char*)>(&LoadingScreen))}, lvFile{0x5B3680,B(&LoadingScreenLoadingFile)},
               lvImage{0x407610,B(&CStreaming::AddImageToList)}, lvCol{0x5B4E60,B(static_cast<void(*)(const char*,uint8)>(&CFileLoader::LoadCollisionFile))},
               lvAtomic{0x5B39D0,B(static_cast<void(*)(const char*)>(&CFileLoader::LoadAtomicFile))}, lvClump{0x5B3A30,B(static_cast<void(*)(const char*)>(&CFileLoader::LoadClumpFile))},
               lvIde{0x5B8400,B(&CFileLoader::LoadObjectTypes)}, lvIpl{0x5B8700,B(&CFileLoader::LoadScene)}, lvMatch{0x5B57C0,B(&MatchAllModelStrings)}, lvObjData{0x5B5360,B(&CObjectData::Initialise)},
               lvCommon{0x5B8F00,B(&CVehicleModelInfo::SetupCommonData)}, lvInit2{0x5B8AD0,B(&CStreaming::Init2)}, lvChunk{0x590D00,B(&CLoadingScreen::NewChunkLoaded)},
               lvBoxes{0x4113D0,B(&CColStore::LoadAllBoundingBoxes)}, lvRemaining{0x405780,B(&CIplStore::LoadAllRemainingIpls)}, lvPost{0x410EC0,B(&CColStore::BoundingBoxesPostProcess)},
               lvTrains{0x6F7440,B(&CTrain::InitTrains)}, lvRemoveCol{0x410E00,B(&CColStore::RemoveAllCollision)};
    const Pair rwOpen{0x7ECEF0,0}, rwClose{0x7ECE20,0}, rwFind{0x7ED2D0,0}, rwClumpRead{0x74B420,0}, rwForAll{0x749B70,0}, rwClumpDestroy{0x74A310,0}, rwTxdRead{0x730FC0,0}, rwTxdCreate{0x7F3600,0},
               rwForAllTex{0x7F3730,0}, rwTxdGet{0x7F3A90,0}, rwTxdSet{0x7F3A70,0}, rwTxdDestroy{0x7F36A0,0}, rwGta1{0x72E570,0}, rwGta2{0x72E620,0}, rwClumpCreate{0x74A290,0}, rwFrameCreate{0x7F0410,0},
               rwFrameClone{0x7EFFB0,0}, rwAddChild{0x7F0B00,0}, rwRemoveAtomic{0x74A4C0,0}, rwAtomicSetFrame{0x74BF20,0};
    dllSetRelated=reinterpret_cast<RpAtomicCallBack>(bit_cast<uintptr_t>(&CFileLoader::SetRelatedModelInfoCB)); dllFindRelated=reinterpret_cast<RpAtomicCallBack>(bit_cast<uintptr_t>(&CFileLoader::FindRelatedModelInfoCB));

    // regiones de fixtures
    nreg=0; Reg(mis.data(),sizeof(mis),1); Reg(otherTime.data(),sizeof(otherTime),2); Reg(arena.data(),arena.size(),3); Reg(colModels.data(),sizeof(colModels),4); Reg(mats.data(),sizeof(mats),5);
    Reg(streams.data(),sizeof(streams),6); Reg(clumps.data(),sizeof(clumps),7); Reg(atomics.data(),sizeof(atomics),8); Reg(frames.data(),sizeof(frames),9); Reg(txds.data(),sizeof(txds),10);
    Reg(ents.data(),sizeof(ents),11); Reg(iplArray.data(),sizeof(iplArray),12); Reg(txdStorage.data(),txdStorage.size(),13); Reg(enexStorage.data(),enexStorage.size(),14); Reg(colStorage.data(),colStorage.size(),15);
    Reg(bldStorage.data(),bldStorage.size(),16); Reg(dumStorage.data(),dumStorage.size(),17); Reg(fileData.data(),fileData.size(),18);
    Reg(reinterpret_cast<void*>(GLOADED),4096*4,19); Reg(reinterpret_cast<void*>(COLBUF),0x8000,20); Reg(reinterpret_cast<void*>(MS_LINE),512,21); Reg(reinterpret_cast<void*>(HEAPBUF),0x4000,22);
    BuildVt();
    const int32 trainCrossing=int32(*reinterpret_cast<uint16*>(0x8CD6B4));

    // estado real guardado
    std::vector<Blob> blobs{{MS_LINE,512,{}},{HEAPBUF,1024,{}},{GSTRING,352,{}},{GATOMIC,4,{}},{GNUM,4,{}},{GLOADED,4096*4,{}},{COLBUF,0x8000,{}},{LODMULT,4,{}},{NUMGROUPS,4,{}},{MIPTRS,20000*4,{}},
                            {TXDPOOLP,4,{}},{ENEXPOOLP,4,{}},{COLPOOLP,4,{}},{BLDPOOLP,4,{}},{DUMPOOLP,4,{}}};
    for(auto& b:blobs)b.saved.assign(reinterpret_cast<uint8*>(b.p),reinterpret_cast<uint8*>(b.p)+b.n);
    auto restoreAll=[&]{ for(auto& b:blobs)if(std::memcmp(reinterpret_cast<void*>(b.p),b.saved.data(),b.n))std::memcpy(reinterpret_cast<void*>(b.p),b.saved.data(),b.n); };
    auto stateRestored=[&]{ for(auto& b:blobs)if(std::memcmp(reinterpret_cast<void*>(b.p),b.saved.data(),b.n))return false; return true; };
    auto ptrs=reinterpret_cast<CBaseModelInfo**>(MIPTRS);
    auto Swap=[&](uintptr_t poolPtrAddr,FakePool& p){ *reinterpret_cast<void**>(poolPtrAddr)=p.hdr.data(); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch;
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                if(onlyCase>=0&&i!=onlyCase)continue;
                // Antes de ejecutar: si el proceso muere, el ultimo case_start del log dice que caso fue (la semilla se recalcula igual al repetirlo).
                Write({{"event","case_start"},{"function",full},{"case",i},{"pair",pair},{"seed",uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xF1E47u}});
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xF1E47u; if(!stt)stt=1; nev=0; arenaTop=0; colModelsUsed=0; matsUsed=0; framesUsed=0; clumpsRead=0; loaderSeq=0; instSeq=0; rngState=uint32(i)*7919u;
                    for(int k=0;k<4;++k){ masks[k]=next(); calls[k]=0; }
                    restoreAll();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    // parches activos solo durante el caso (despues de State, deshechos antes del siguiente State)
                    std::array<std::optional<EntryPatch>,72> pp; int npp=0;
                    auto P=[&](const Pair& pr,auto stub){ if(pr.exe)pp[npp++].emplace(pr.exe,stub); if(pr.dll&&pr.dll!=pr.exe)pp[npp++].emplace(pr.dll,stub); };
                    auto Unpatch=[&]{ for(int k=npp-1;k>=0;--k)pp[k].reset(); npp=0; };
                    // fixtures comunes
                    for(auto& c:colModels)for(auto& b:c)b=uint8(next());
                    for(auto& t:otherTime)for(auto& b:t)b=uint8(next());
                    for(auto& b:arena)b=uint8(next());
                    for(int k=0;k<NMI;++k)miId[k]=FIRST_ID+k;
                    char* L=lines[0]; L[0]=0; LB lb{L};
                    char n1[64]{}, n2[64]{}, n3[64]{}, n4[64]{}, n5[64]{}, n6[64]{}, n7[64]{}, n8[64]{};
                    auto MisHash=[&]{ return std::to_string(Fnv(mis.data(),sizeof(mis))); };

                    // ====================== lectura de lineas ======================
                    if(nm=="LoadLine-File") {
                        auto* ms=reinterpret_cast<uint8*>(MS_LINE); for(int k=0;k<512;++k)ms[k]=uint8(next());
                        static const char pool[]{' ','\t',',','\r','\n','#','a','B','z','0','9','.','-','_','\x7f','\x80','\xa0','\xff','\x01','\x1f'};
                        nlines=next()%10 ? 1 : 0; lineAt=0; const int len=int(next()%(next()%8 ? 120 : 590));
                        for(int k=0;k<len;++k)L[k]=next()%3 ? pool[next()%std::size(pool)] : char('a'+next()%26); L[len]=0;
                        P(pReadLine,FakeReadLine);
                        char* r=reinterpret_cast<char*(__cdecl*)(FILESTREAM)>(a)(reinterpret_cast<FILESTREAM>(FAKE_FILE));
                        Unpatch();
                        ret=r ? json(intptr_t(r-reinterpret_cast<char*>(MS_LINE))) : json(nullptr); out["line"]=ReversingProbe::Bytes(ms,512);
                    }
                    else if(nm=="LoadLine-Bufer") {
                        auto* hb=reinterpret_cast<uint8*>(HEAPBUF); for(int k=0;k<1024;++k)hb[k]=uint8(next());
                        static char buf[720]; static const char pool[]{' ','\t',',','\r','\n','\n','a','B','z','0','.','\x80','\xff','\x01','\0'};
                        const int len=int(next()%(next()%6 ? 100 : 700)); for(int k=0;k<len;++k)buf[k]=next()%3 ? pool[next()%std::size(pool)] : char('a'+next()%26); buf[len]=0; buf[len+1]=0;
                        if(next()%5==0)buf[0]=0;
                        int32 size; switch(next()%5){ case 0: size=len; break; case 1: size=int32(next()%uint32(len+2)); break; case 2: size=-int32(next()%4); break; default: size=len+int32(next()%20); }
                        char* it=buf; int32 sz=size;
                        char* r=reinterpret_cast<char*(__cdecl*)(char**,int32*)>(a)(&it,&sz);
                        ret=r ? json(intptr_t(r-reinterpret_cast<char*>(HEAPBUF))) : json(nullptr);
                        out["it"]=intptr_t(it-buf); out["size"]=sz; out["heap"]=ReversingProbe::Bytes(hb,760);
                    }
                    // ====================== IDE ======================
                    else if(nm=="LoadObject"||nm=="LoadTimeObject"||nm=="LoadWeaponObject"||nm=="LoadClumpObject"||nm=="LoadAnimatedClumpObject") {
                        BuildMis(next); timeResult=next()%2 ? otherTime[next()%4].data() : nullptr;
                        const int32 id=int32(next()%20000); Nm(next,n1,23); Nm(next,n2,23);
                        const int32 dd=next()%3 ? int32(next()%100000) : int32(next()%801)-400; const int32 fl=int32(next()); const int32 on=int32(next()%30), off=int32(next()%30);
                        const int v=int(next()%8);
                        lb.I(id); lb.Str(n1); lb.Str(n2);
                        if(nm=="LoadObject"){
                            switch(v){ case 0: case 1: lb.Fx(dd); lb.I(fl); break; case 2: lb.I(1); lb.Fx(dd); lb.I(fl); break; case 3: lb.I(2); lb.Fx(dd); lb.Fx(Fv(next)); lb.I(fl); break;
                                case 4: lb.I(3); lb.Fx(dd); lb.Fx(Fv(next)); lb.Fx(Fv(next)); lb.I(fl); break; case 5: lb.I(next()%2 ? 0 : -int32(next()%9)); lb.I(fl); break; case 6: L[0]=0; lb.n=0; lb.I(id); lb.Str(n1); break; default: lb.Str("x"); }
                            P(pAddAtomic,FakeAddModel); P(pAddDamage,FakeAddDamage); P(pSetTex,FakeSetTexDict);
                        } else if(nm=="LoadTimeObject"){
                            switch(v){ case 0: case 1: lb.Fx(dd); lb.I(fl); lb.I(on); lb.I(off); break; case 2: lb.I(1); lb.Fx(dd); lb.I(fl); lb.I(on); lb.I(off); break;
                                case 3: lb.I(2); lb.Fx(dd); lb.Fx(Fv(next)); lb.I(fl); lb.I(on); lb.I(off); break; case 4: lb.I(3); lb.Fx(dd); lb.Fx(Fv(next)); lb.Fx(Fv(next)); lb.I(fl); lb.I(on); lb.I(off); break;
                                case 5: lb.I(next()%2 ? 0 : -int32(next()%9)); lb.I(fl); lb.I(on); lb.I(off); break; case 6: L[0]=0; lb.n=0; lb.I(id); break; default: lb.Str("x"); }
                            P(pAddTime,FakeAddModel); P(pSetTex,FakeSetTexDict); P(pOtherTime,FakeFindOtherTime);
                        } else if(nm=="LoadWeaponObject"){
                            Nm(next,n3,15); lb.Str(n3); lb.I(int32(next()%50)); lb.Fx(dd);
                            P(pAddWeapon,FakeAddModel); P(pSetTex,FakeSetTexDict); P(pSetCol,FakeSetColModel);
                        } else if(nm=="LoadClumpObject"){
                            if(v==6){ L[0]=0; lb.n=0; lb.I(id); lb.Str(n1); }
                            P(pAddClump,FakeAddModel); P(pSetTex,FakeSetTexDict); P(pSetCol,FakeSetColModel);
                        } else {
                            if(next()%3)strcpy(n3,"null"); else Nm(next,n3,15);
                            if(v==6){ L[0]=0; lb.n=0; lb.I(id); lb.Str(n1); lb.Str(n2); } else { lb.Str(n3); lb.Fx(dd); lb.I(fl); }
                            P(pAddClump,FakeAddModel); P(pSetTex,FakeSetTexDict);
                        }
                        ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(L);
                        Unpatch(); out["mi"]=MisHash(); out["other"]=Fnv(otherTime.data(),sizeof(otherTime));
                    }
                    else if(nm=="LoadVehicleObject") {
                        BuildMis(next); BuildPool(txdPool,next,2); Swap(TXDPOOLP,txdPool);
                        for(auto& m:mis)At<int16>(m.data(),0xA)=int16(UsedSlot(txdPool,next()));
                        txdFindResult=int32(next()%64); txdAddResult=int32(next()%64);
                        static const char* const types[]{"car","mtruck","quad","heli","plane","boat","train","f_heli","f_plane","bike","bmx","trailer","zzz","Car"};
                        static const char* const classes[]{"normal","poorfamily","richfamily","executive","worker","big","taxi","moped","motorbike","leisureboat","workerboat","bicycle","ignore","rich","Normal"};
                        const char* type=types[next()%std::size(types)];
                        Nm(next,n1,23); Nm(next,n2,23); Nm(next,n3,15); Nm(next,n4,31); Nm(next,n5,15);
                        if(next()%3==0)n4[0]='_';
                        lb.I(int32(next()%20000)); lb.Str(n1); lb.Str(n2); lb.Str(type); lb.Str(n3); lb.Str(n4); lb.Str(n5); lb.Str(classes[next()%std::size(classes)]);
                        lb.I(int32(next()%100)); lb.I(int32(next()%256)); lb.X(next());
                        const std::string_view t{type}; int extra;
                        if(t=="car"||t=="mtruck"||t=="quad"||t=="heli"||t=="plane"||t=="trailer"||t=="bike"||t=="bmx") extra=3+int(next()%2);
                        else if(t=="f_plane") extra=1+int(next()%4); else extra=int(next()%5);
                        if(extra>0)lb.I(int32(next()%2000)-500); if(extra>1)lb.Fx(int32(next()%200)); if(extra>2)lb.Fx(int32(next()%200)); if(extra>3)lb.I(int32(next()%300)-150);
                        P(pAddVehicle,FakeAddModel); P(pSetTex,FakeSetTexDict); P(pFindTxd,FakeFindTxd); P(pAddTxd,FakeAddTxd); P(pHandling,FakeHandlingId);
                        ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(L);
                        Unpatch(); out["mi"]=MisHash(); out["txd"]=Fnv(txdStorage.data(),txdStorage.size());
                    }
                    else if(nm=="LoadPedObject") {
                        BuildMis(next); numGroups=next()%15; *reinterpret_cast<uint32*>(NUMGROUPS)=numGroups;
                        Nm(next,n1,23); Nm(next,n2,23); Nm(next,n3,23); Nm(next,n4,23); Nm(next,n6,15); Nm(next,n7,19); Nm(next,n8,40);
                        const char* group=next()%3 ? groupNames[next()%std::size(groupNames)] : n4;
                        lb.I(int32(next()%20000)); lb.Str(n1); lb.Str(n2); lb.Str(n3); lb.Str(n4); lb.Str(group); lb.X(next()); lb.X(next()); lb.Str(n6);
                        lb.I(int32(next()%12)-1); lb.I(int32(next()%12)-1); lb.Str(n7); lb.Str(n8); Nm(next,n8,40); lb.Str(n8);
                        P(pAddPed,FakeAddModel); P(pSetTex,FakeSetTexDict); P(pSetCol,FakeSetColModel); P(pGroupName,FakeGroupName); P(pPedType,FakeFindPedType); P(pPedStat,FakePedStat);
                        P(pRace,FakeRace); P(pAudioType,FakeAudioType); P(pVoice,FakeGetVoice);
                        ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(L);
                        Unpatch(); out["mi"]=MisHash();
                    }
                    else if(nm=="LoadTXDParent") {
                        BuildPool(txdPool,next,2); Swap(TXDPOOLP,txdPool); txdFindResult=UsedSlot(txdPool,next()); txdAddResult=UsedSlot(txdPool,next());
                        Nm(next,n1,31); Nm(next,n2,31); lb.Str(n1); lb.Str(n2);
                        P(pFindTxd,FakeFindTxd); P(pAddTxd,FakeAddTxd);
                        ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(L);
                        Unpatch(); out["txd"]=ReversingProbe::Bytes(txdStorage.data(),txdStorage.size());
                    }
                    else if(nm=="LoadPathHeader") {
                        lb.I(int32(next()%5)-1); lb.I(int32(next()%5000)-100); Nm(next,n1,40); lb.Str(n1);
                        int32 type=0x5A5A; ret=reinterpret_cast<int32(__cdecl*)(const char*,int32&)>(a)(L,type); out["type"]=type;
                    }
                    else if(nm=="LoadPedPathNode"||nm=="LoadCarPathNode") {
                        for(int k=0;k<13;++k){ if(k<4||k==8||k==9||k==10||k==12)lb.I(int32(next()%200)-50); else lb.Fx(Fv(next)); }
                        if(nm=="LoadPedPathNode")reinterpret_cast<void(__cdecl*)(const char*,int32,int32)>(a)(L,int32(next()%3)-1,int32(next()%12));
                        else reinterpret_cast<void(__cdecl*)(const char*,int32,int32,bool)>(a)(L,int32(next()%3)-1,int32(next()%12),next()%2);
                    }
                    // ====================== IPL ======================
                    else if(nm=="LoadGarage") {
                        for(int k=0;k<8;++k)lb.Fx(Fv(next)); lb.I(int32(next()%40)); lb.I(int32(next()%50)); Nm(next,n1,7); if(next()%6)lb.Str(n1);
                        P(pGarage,FakeGarage); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                    }
                    else if(nm=="LoadStuntJump") {
                        const int nf=next()%5 ? 16 : int(next()%16); for(int k=0;k<nf;++k){ if(k==15)lb.I(int32(next()%10000)); else lb.Fx(Fv(next)); }
                        P(pStunt,FakeStunt); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                    }
                    else if(nm=="LoadCarGenerator-1"||nm=="LoadCarGenerator-0") {
                        carGenResult=next()%4 ? int32(next()%500) : -1;
                        P(pCarGen,FakeCreateCarGen); P(pSwitchOn,FakeSwitchOn);
                        const int32 ipl=int32(next()%300)-20;
                        if(nm=="LoadCarGenerator-1"){
                            const int nf=next()%5 ? 12 : int(next()%12);
                            for(int k=0;k<nf;++k){ if(k<4)lb.Fx(Fv(next)); else if(k==4)lb.I(next()%4 ? int32(400+next()%212) : int32(next()%70000)-1); else lb.I(int32(next()%300)-20); }
                            reinterpret_cast<void(__cdecl*)(const char*,int32)>(a)(L,ipl);
                        } else {
                            alignas(16) std::array<uint8,0x30> cg{}; for(auto& b:cg)b=uint8(next());
                            for(int k=0;k<4;++k)At<float>(cg.data(),k*4)=float(Fv(next))/100.f;
                            reinterpret_cast<void(__cdecl*)(void*,int32)>(a)(cg.data(),ipl);
                        }
                        Unpatch();
                    }
                    else if(nm=="LoadPickup") {
                        const int32 type=int32(next()%64)-3; lb.I(type); const int nf=next()%8 ? 3 : int(next()%3); for(int k=0;k<nf;++k)lb.Fx(Fv(next));
                        P(pPickup,FakePickup); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                    }
                    else if(nm=="LoadZone") {
                        Nm(next,n1,23); Nm(next,n2,11); lb.Str(n1); lb.I(int32(next()%4)); for(int k=0;k<6;++k)lb.Fx(Fv(next)); lb.I(int32(next()%5)); if(next()%6)lb.Str(n2);
                        P(pZone,FakeCreateZone); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                    }
                    else if(nm=="LoadCullZone") {
                        for(int k=0;k<9;++k)lb.Fx(Fv(next)); lb.I(int32(next()%0x10000));
                        switch(next()%4){ case 0: break; case 1: lb.I(int32(next()%10)); break; default: for(int k=0;k<4;++k)lb.Fx(Fv(next)); }
                        P(pMirror,FakeMirrorZone); P(pCull,FakeCullZone); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                    }
                    else if(nm=="LoadOcclusionVolume") {
                        const int nf=next()%4 ? 10 : 7+int(next()%3); for(int k=0;k<nf;++k){ if(k==9)lb.I(int32(next()%8)-2); else lb.Fx(Fv(next)); }
                        static const char* const files[]{"DATA\\MAPS\\LA\\LAn_int.ipl","DATA\\MAPS\\leveldes\\levelmap.ipl","abcint.ipl","abcintxipl","LA_INT.IPL","gen_int1.ipl","xx_intX.ipl","seabed.ipl"};
                        P(pOccl,FakeOcclusion); reinterpret_cast<void(__cdecl*)(const char*,const char*)>(a)(L,files[next()%std::size(files)]); Unpatch();
                    }
                    else if(nm=="LoadAudioZone") {
                        Nm(next,n1,7); lb.Str(n1); lb.I(int32(next()%300)); lb.I(int32(next()%5)-1);
                        const int nf=next()%2 ? 6 : 4; for(int k=0;k<nf;++k)lb.Fx(Fv(next));
                        P(pABox,FakeAudioBox); P(pASphere,FakeAudioSphere); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                    }
                    else if(nm=="LoadEntryExit") {
                        BuildPool(enexPool,next,2); Swap(ENEXPOOLP,enexPool); enexResult=UsedSlot(enexPool,next());
                        for(int k=0;k<11;++k)lb.Fx(Fv(next)); lb.I(int32(next()%18)); lb.I(int32(next()%0x1000));
                        Nm(next,n1,20); switch(next()%4){ case 0: snprintf(n2,sizeof(n2),"\"%s\"",n1); break; case 1: snprintf(n2,sizeof(n2),"%s\"x",n1); break; default: strcpy(n2,n1); }
                        lb.Str(n2); lb.I(int32(next()%6)); lb.I(int32(next()%5)); lb.I(int32(next()%24)); lb.I(int32(next()%25));
                        P(pEnex,FakeEnex); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                        out["enex"]=ReversingProbe::Bytes(enexStorage.data(),enexStorage.size());
                    }
                    else if(nm=="LoadTimeCyclesModifier") {
                        for(int k=0;k<6;++k)lb.Fx(Fv(next)); lb.I(int32(next()%3000)); lb.I(int32(next()%40)); lb.Fx(int32(next()%200));
                        const int ex=int(next()%4); for(int k=0;k<ex;++k)lb.Fx(int32(next()%30000));
                        P(pTimeCycle,FakeTimeCycle); reinterpret_cast<void(__cdecl*)(const char*)>(a)(L); Unpatch();
                    }
                    else if(nm=="LoadObjectInstance-file") {
                        instResult=ents[next()%ents.size()].data(); Nm(next,n1,23);
                        lb.I(int32(next()%20000)); lb.Str(n1); lb.I(int32(next())); for(int k=0;k<7;++k)lb.Fx(Fv(next)); lb.I(int32(next()%300)-1);
                        P(pInst,FakeLoadInst); ret=N(reinterpret_cast<CEntity*(__cdecl*)(const char*)>(a)(L)); Unpatch();
                    }
                    // ====================== instancia ======================
                    else if(nm=="LoadObjectInstance-inst") {
                        BuildMis(next);
                        for(int k=0;k<NMI;++k){ auto& m=mis[k]; miType[k]=next()%2 ? 1 : 5; At<int16>(m.data(),0x10)=next()%2 ? int16(-1) : int16(next()%6);
                            static const uint8 specials[]{0,1,2,4,5,7}; m[0x13]=uint8((m[0x13]&0x87)|(specials[next()%std::size(specials)]<<3));
                            At<float>(m.data(),0x18)=float(int32(next()%500))/100.f; At<void*>(m.data(),0x14)=next()%4 ? colModels[k%8].data() : nullptr; ptrs[miId[k]]=reinterpret_cast<CBaseModelInfo*>(m.data()); }
                        ptrs[trainCrossing]=reinterpret_cast<CBaseModelInfo*>(mis[NMI-1].data());
                        BuildPool(colPool,next,2); Swap(COLPOOLP,colPool);
                        for(int32 s=0;s<colPool.size;++s)for(int k=0;k<4;++k)At<float>(colStorage.data()+s*0x2C,k*4)=float(Fv(next))/100.f;
                        for(int k=0;k<8;++k){ auto* c=colModels[k].data(); for(int q=0;q<10;++q)At<float>(c,q*4)=float(Fv(next))/100.f; c[0x29]=uint8(next()%4);
                            c[0x28]=next()%3 ? uint8(0) : uint8(UsedSlot(colPool,next())); }
                        BuildPool(bldPool,next,0); BuildPool(dumPool,next,0); Swap(BLDPOOLP,bldPool); Swap(DUMPOOLP,dumPool);
                        for(auto& m:mats){ for(auto& b:m)b=uint8(next()); for(int q=0;q<16;++q)At<float>(m.data(),q*4)=float(Fv(next))/1000.f; }
                        alignas(16) std::array<uint8,0x28> inst{}; for(int k=0;k<3;++k)At<float>(inst.data(),k*4)=float(Fv(next))/100.f;
                        auto q=[&]{ return next()%2 ? float(int32(next()%101)-50)/1000.f : float(int32(next()%2001)-1000)/1000.f; };
                        At<float>(inst.data(),0xC)=q(); At<float>(inst.data(),0x10)=q(); At<float>(inst.data(),0x14)=float(int32(next()%2001)-1000)/1000.f;
                        At<float>(inst.data(),0x18)=next()%10 ? float(int32(next()%2001)-1000)/1000.f : 1.25f;
                        if(next()%6==0){ At<float>(inst.data(),0xC)=0.f; } if(next()%6==0){ At<float>(inst.data(),0x10)=0.f; }
                        At<int32>(inst.data(),0x1C)=next()%8 ? miId[next()%(NMI-1)] : trainCrossing; At<uint32>(inst.data(),0x20)=next()&0x1FFF; At<int32>(inst.data(),0x24)=int32(next()%40)-1;
                        Nm(next,n1,23);
                        P(pRand,FakeRand); P(pRandom,FakeRandomNumber); P(pGlass,FakeIsGlass); P(pAllocStatic,FakeAllocStatic); P(pAllocMatrix,FakeAllocMatrix); P(pUpdateMatrix,FakeUpdateMatrix);
                        P(pSetRotate,FakeSetRotateQuat); P(pSetRotateZ,FakeSetRotateZ); P(pTrain,FakeTrainCrossing);
                        auto* e=reinterpret_cast<uint8*(__cdecl*)(void*,const char*)>(a)(inst.data(),n1);
                        Unpatch();
                        ret=N(e); out["inst"]=ReversingProbe::Bytes(inst.data(),inst.size());
                        if(e&&N(e)>>24!=0xEE){ out["class"]=VtClass(At<uintptr_t>(e,0)); std::array<uint8,0x38> eb{}; std::memcpy(eb.data(),e,0x38); At<uint32>(eb.data(),0)=0; At<uint32>(eb.data(),0x14)=N(At<void*>(e,0x14));
                            out["entity"]=ReversingProbe::Bytes(eb.data(),eb.size()); }
                        out["pools"]=json::array({ReversingProbe::Bytes(bldStates.data(),bldStates.size()),ReversingProbe::Bytes(dumStates.data(),dumStates.size()),At<int32>(bldPool.hdr.data(),0xC),At<int32>(dumPool.hdr.data(),0xC)});
                        out["col"]=ReversingProbe::Bytes(colStorage.data(),colStorage.size()); out["mats"]=Fnv(mats.data(),sizeof(mats)); out["mi"]=MisHash();
                    }
                    // ====================== RW ======================
                    else if(nm=="FindRelatedModelInfoCB"||nm=="SetRelatedModelInfoCB"||nm=="LoadAtomicFile-stream"||nm=="LoadAtomicFile-"||nm=="LoadClumpFile-1"||nm=="LoadClumpFile-"||
                            nm=="StartLoadClumpFile"||nm=="FinishLoadClumpFile"||nm=="LoadTexDictionary"||nm=="AddTexDictionaries") {
                        BuildMis(next);
                        for(int k=0;k<NMI;++k){ miType[k]=next()%2 ? 6 : 5; miNullAtomic[k]=0; ptrs[miId[k]]=reinterpret_cast<CBaseModelInfo*>(mis[k].data()); At<void*>(mis[k].data(),0x1C)=next()%3 ? txds[0].data() : nullptr; }
                        ptrs[508]=reinterpret_cast<CBaseModelInfo*>(mis[3].data());
                        for(auto& c:clumps){ for(auto& b:c)b=uint8(next()); }
                        for(auto& t:atomics)for(auto& b:t)b=uint8(next());
                        for(int k=0;k<int(clumps.size());++k)At<void*>(clumps[k].data(),4)=frames[k].data();
                        for(int k=0;k<int(atomics.size());++k)At<void*>(atomics[k].data(),4)=frames[k%8].data();
                        static const char* const suffix[]{"","","_dam","_l0","_L0","_DAM","x"};
                        for(int k=0;k<int(frames.size());++k){ for(auto& b:frames[k])b=uint8(next()); if(next()%5)snprintf(frameName[k],40,"%s%s",miName[next()%NMI],suffix[next()%std::size(suffix)]); else Nm(next,frameName[k],20); }
                        for(int k=0;k<NMI;++k)if(next()%6==0)miId[k]=-2;   // invisibles para la busqueda por nombre
                        streamResult=next()%6 ? reinterpret_cast<RwStream*>(streams[0].data()) : nullptr; findChunkBudget=int(1+next()%4);
                        txdReadResult=next()%3 ? reinterpret_cast<RwTexDictionary*>(txds[1].data()) : nullptr; txdCreateResult=reinterpret_cast<RwTexDictionary*>(txds[2].data());
                        *reinterpret_cast<uint32*>(GATOMIC)=uint32(FIRST_ID+int32(next()%NMI)); invokeCallbacks=true;
                        P(rwOpen,FakeStreamOpen); P(rwClose,FakeStreamClose); P(rwFind,FakeFindChunk); P(rwClumpRead,FakeClumpRead); P(rwForAll,FakeForAllAtomics); P(rwClumpDestroy,FakeClumpDestroy);
                        P(rwTxdRead,FakeTxdRead); P(rwTxdCreate,FakeTxdCreate); P(rwForAllTex,FakeForAllTextures); P(rwGta1,FakeGtaRead1); P(rwGta2,FakeGtaRead2); P(rwClumpCreate,FakeClumpCreate);
                        P(rwFrameCreate,FakeFrameCreate); P(rwFrameClone,FakeFrameClone); P(rwAddChild,FakeFrameAddChild); P(rwRemoveAtomic,FakeRemoveAtomic); P(rwAtomicSetFrame,FakeAtomicSetFrame);
                        P(pUseCommon,FakeUseCommon); P(pStopCommon,FakeStopCommon); P(pColPlugin,FakeColPluginSet); P(pNodeName,FakeNodeName); P(pMiByName,FakeGetMiByName);
                        P(pRenderCb,FakeRenderCb); P(pMiIndex,FakeMiIndex); P(pDamaged,FakeSetDamaged);
                        auto* stream=reinterpret_cast<RwStream*>(streams[1].data());
                        const uint32 id=next()%6==0 ? 508u : uint32(FIRST_ID+int32(next()%NMI));
                        if(nm=="FindRelatedModelInfoCB"||nm=="SetRelatedModelInfoCB")
                            ret=N(reinterpret_cast<RpAtomic*(__cdecl*)(RpAtomic*,void*)>(a)(reinterpret_cast<RpAtomic*>(atomics[next()%atomics.size()].data()),clumps[next()%4].data()));
                        else if(nm=="LoadAtomicFile-stream"){ for(auto& m:mis)if(next()%2)m[0x13]|=0x80; else m[0x13]&=0x7F; ret=reinterpret_cast<bool(__cdecl*)(RwStream*,uint32)>(a)(stream,id); }
                        else if(nm=="LoadAtomicFile-"){ Nm(next,n1,40); reinterpret_cast<void(__cdecl*)(const char*)>(a)(n1); }
                        else if(nm=="LoadClumpFile-1"){ for(auto& m:mis)if(next()%3==0)m[0x13]|=0x02; else m[0x13]&=0xFD; invokeCallbacks=false; ret=reinterpret_cast<bool(__cdecl*)(RwStream*,uint32)>(a)(stream,id); }
                        else if(nm=="LoadClumpFile-"){ Nm(next,n1,40); reinterpret_cast<void(__cdecl*)(const char*)>(a)(n1); }
                        else if(nm=="StartLoadClumpFile"||nm=="FinishLoadClumpFile") ret=reinterpret_cast<bool(__cdecl*)(RwStream*,uint32)>(a)(stream,id);
                        else if(nm=="LoadTexDictionary"){ Nm(next,n1,40); ret=N(reinterpret_cast<RwTexDictionary*(__cdecl*)(const char*)>(a)(n1)); }
                        else reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(txds[next()%8].data(),txds[next()%8].data());
                        Unpatch(); invokeCallbacks=false;
                        out["mi"]=MisHash(); out["clumps"]=Fnv(clumps.data(),sizeof(clumps)); out["atomicId"]=*reinterpret_cast<uint32*>(GATOMIC);
                    }
                    // ====================== colision ======================
                    else if(nm=="LoadCollisionModel"||nm=="LoadCollisionModelVer2"||nm=="LoadCollisionModelVer3"||nm=="LoadCollisionModelVer4") {
                        static std::array<uint8,0x1000> cb{}; for(auto& b:cb)b=uint8(next()%7==0 ? next() : 0);
                        auto W32=[&](size_t at,uint32 v){ std::memcpy(cb.data()+at,&v,4); }; auto WF=[&](size_t at,float v){ std::memcpy(cb.data()+at,&v,4); };
                        auto W16=[&](size_t at,uint16 v){ std::memcpy(cb.data()+at,&v,2); };
                        size_t pos=0; uint32 dataSize=0;
                        if(nm=="LoadCollisionModel"){
                            for(int k=0;k<10;++k)WF(k*4,float(Fv(next))/100.f); pos=40;
                            auto cnt=[&](uint32 mx){ switch(next()%8){ case 0: return 0u; case 1: return 0x10000u|next()%3; case 2: return 0x8000u+next()%4; default: return next()%mx; } };
                            const uint32 ns=cnt(4); W32(pos,ns); pos+=4; if(int16(ns)>0)for(int32 k=0;k<int16(ns);++k){ for(int q=0;q<4;++q)WF(pos+q*4,float(Fv(next))/100.f); for(int q=16;q<20;++q)cb[pos+q]=uint8(next()); pos+=20; }
                            const uint32 nl=next()%6==0 ? 0x80u+next()%8 : next()%3; W32(pos,nl|(next()%4==0 ? next()&0xFFFFFF00u : 0)); pos+=4; if(int8(nl)>0)pos+=size_t(int8(nl))*24;
                            const uint32 nb=cnt(4); W32(pos,nb); pos+=4; if(int16(nb)>0)for(int32 k=0;k<int16(nb);++k){ for(int q=0;q<6;++q)WF(pos+q*4,float(Fv(next))/100.f); for(int q=24;q<28;++q)cb[pos+q]=uint8(next()); pos+=28; }
                            const uint32 nv=next()%8==0 ? 0x80000000u|next() : next()%9; W32(pos,nv); pos+=4;
                            if(int32(nv)>0)for(int32 k=0;k<int32(nv)*3;++k){ WF(pos,next()%4 ? float(Fv(next))/100.f : float(int32(next()%600001)-300000)/100.f); pos+=4; }
                            const uint32 nt=cnt(5); W32(pos,nt); pos+=4; if(int16(nt)>0)for(int32 k=0;k<int16(nt);++k){ for(int q=0;q<3;++q)W32(pos+q*4,next()%3 ? next()%40 : next()); for(int q=12;q<16;++q)cb[pos+q]=uint8(next()); pos+=16; }
                        } else {
                            const size_t hsz=nm=="LoadCollisionModelVer2" ? 0x4C : nm=="LoadCollisionModelVer3" ? 0x58 : 0x5C;
                            for(int k=0;k<10;++k)WF(k*4,float(Fv(next))/100.f);
                            W16(0x28,uint16(next()%5)); W16(0x2A,uint16(next()%5)); W16(0x2C,uint16(next()%5)); cb[0x2E]=uint8(next()%4); W32(0x30,next()%32);
                            const uint32 extra=next()%6==0 ? 0u : 16u+next()%400; dataSize=uint32(hsz)+extra;
                            const int32 adj=nm=="LoadCollisionModelVer2" ? 0x38 : nm=="LoadCollisionModelVer3" ? 0x44 : 0x48;
                            auto Off=[&]{ return next()%3==0 ? 0u : uint32(adj+0x30+int32(next()%(extra ? extra : 1))); }; // puntero = p + off - adj = p+0x30+r
                            for(int k=0;k<6;++k)W32(0x34+k*4,Off());
                            if(hsz>0x4C){ const uint32 nsf=next()%3==0 ? 0u : next()%5==0 ? 0x80000001u : 1u+next()%6; W32(0x4C,nsf); W32(0x50,Off());
                                uint32 sf=0; if(extra>=64&&next()%4){ const uint32 k0=next()%(extra-48); sf=uint32(adj+0x30)+k0; } W32(0x54,next()%5 ? sf : 0u);
                                if(int32(nsf)>0&&sf){ const uint32 room=(extra-(sf-uint32(adj+0x30)))/8; W32(0x4C,std::min<uint32>(nsf,std::max<uint32>(room,1u))); } }
                            pos=hsz+extra;
                        }
                        auto* cm=colModels[0].data();
                        P(pMalloc,FakeMalloc); P(pMalloc1,FakeMalloc1); P(pNew,FakeNew);
                        if(nm=="LoadCollisionModel") reinterpret_cast<void(__cdecl*)(uint8*,void*)>(a)(cb.data(),cm);
                        else reinterpret_cast<void(__cdecl*)(uint8*,uint32,void*,const char*)>(a)(cb.data(),dataSize,cm,"colmodel");
                        Unpatch();
                        std::array<uint8,0x30> cmb{}; std::memcpy(cmb.data(),cm,0x30); At<uint32>(cmb.data(),0x2C)=N(At<void*>(cm,0x2C)); out["cm"]=ReversingProbe::Bytes(cmb.data(),cmb.size());
                        if(auto* cd=At<uint8*>(cm,0x2C); cd&&N(cd)>>24==3){ std::array<uint8,0x30> d{}; std::memcpy(d.data(),cd,0x30); for(size_t o:{8u,0xCu,0x10u,0x14u,0x18u,0x1Cu,0x28u,0x2Cu})At<uint32>(d.data(),o)=N(At<void*>(cd,o));
                            out["cd"]=ReversingProbe::Bytes(d.data(),d.size()); }
                        out["arena"]=std::to_string(Fnv(arena.data(),arenaTop)); out["top"]=arenaTop;
                    }
                    else if(nm=="LoadCollisionFile-Buffer"||nm=="LoadCollisionFileFirstTime"||nm=="LoadCollisionFile-File") {
                        BuildMis(next); std::memset(ptrs,0,20000*4);
                        for(int k=0;k<NMI;++k){ auto& m=mis[k]; miType[k]=next()%2 ? 1 : 5; if(next()%4)m[0x12]|=0x80; else m[0x12]&=0x7F; ptrs[miId[k]]=reinterpret_cast<CBaseModelInfo*>(m.data());
                            At<void*>(m.data(),0x14)=next()%3 ? colModels[k%8].data() : nullptr; }
                        BuildPool(colPool,next,2); Swap(COLPOOLP,colPool); const uint8 colId=uint8(UsedSlot(colPool,next()));
                        const bool isFile=nm=="LoadCollisionFile-File";
                        if(isFile){ At<void*>(mis[0].data(),0x14)=colModels[0].data(); mis[0][0x12]|=0x80; }
                        auto* buf=fileData.data(); size_t pos=0; fileData.fill(0);
                        const int nrec=int(1+next()%5);
                        for(int r=0;r<nrec&&pos<0x1800;++r){
                            static const char* const cc[]{"COLL","COL2","COL3","COL4","COLX"};
                            int ver=int(next()%5); if(ver==4&&next()%2)ver=int(next()%4); if(isFile&&r==0)ver=int(next()%3); if(!isFile&&ver==4&&r==0)ver=1;
                            size_t payload; std::array<uint8,0x200> pl{};
                            for(int k=0;k<10;++k){ const float v=float(Fv(next))/100.f; std::memcpy(pl.data()+k*4,&v,4); }
                            if(ver==0){ payload=40+20; }
                            else { const size_t hsz=ver==1 ? 0x4C : ver==2 ? 0x58 : 0x5C; const uint32 fl=next()%32; std::memcpy(pl.data()+0x30,&fl,4); payload=hsz+(next()%3 ? 16+next()%64 : 0); }
                            const uint32 size=uint32(0x18+payload);
                            std::memcpy(buf+pos,cc[ver],4); std::memcpy(buf+pos+4,&size,4);
                            char nmv[22]{}; const int mk=int(next()%NMI); if(next()%5)strcpy(nmv,miName[mk]); else Nm(next,nmv,20); if(next()%6==0)nmv[0]=char(std::toupper(nmv[0]));
                            if(isFile&&r==0)strcpy(nmv,miName[0]);
                            std::memcpy(buf+pos+8,nmv,22);
                            const uint16 mid=uint16(next()%3==0 ? 20000+next()%40000 : next()%5==0 ? next()%1900 : uint32(miId[next()%NMI]));
                            std::memcpy(buf+pos+30,&mid,2); std::memcpy(buf+pos+32,pl.data(),payload); pos+=32+payload;
                        }
                        size_t tail=next()%3==0 ? next()%12 : 0;
                        // Con exactamente 8 bytes sobrantes el EXE lee fourcc=0 y size=0 con el nombre anterior intacto: size-24 negativo,
                        // Malloc enorme (nullptr) y memcpy a nullptr, un fallo real del EXE (y de la DLL) que no se puede comparar.
                        if(isFile&&tail>4)tail=0; // Con >4 bytes sobrantes size queda en 0 (o parcial): size-24 ~ 4 GB, el EXE hace Malloc/memcpy gigantes (con 32 bits el FakeMalloc daba la vuelta y se pisaba el heap)
                        const size_t total=pos+tail;
                        P(pMalloc,FakeMalloc); P(pMalloc1,FakeMalloc1); P(pNew,FakeNew); P(pColNew,FakeColModelNew); P(pSetCol,FakeSetColModel); P(pMiByName,FakeGetMiByName); P(pMiByRange,FakeGetMiByNameRange);
                        P(pPlant,FakePlantFlag); P(pInclude,FakeIncludeModel); P(pCacheCol,FakeAddCacheCol);
                        if(isFile){ fileSize=total; filePos=0; P(pOpen,FakeOpenFile); P(pRead,FakeFileRead); P(pClose,FakeCloseFile);
                            reinterpret_cast<void(__cdecl*)(const char*,uint8)>(a)("DATA\\MAPS\\test.col",colId); }
                        else ret=reinterpret_cast<bool(__cdecl*)(uint8*,uint32,uint8)>(a)(buf,uint32(total),colId);
                        Unpatch();
                        std::array<uint8,sizeof(colModels)> cms{}; std::memcpy(cms.data(),colModels.data(),sizeof(colModels));
                        for(int k=0;k<int(colModels.size());++k)At<uint32>(cms.data()+k*0x40,0x2C)=N(At<void*>(colModels[k].data(),0x2C));
                        // Fuera de CFileLoader: el constructor de CColModel de la DLL pone a cero la esfera (el EXE 0x40FB60 no);
                        // en LoadCollisionFile-File el modelo nuevo nunca se carga, asi que esa esfera se enmascara.
                        if(isFile)for(int k=8;k<16;++k)std::memset(cms.data()+k*0x40+0x18,0,0x10);
                        out["cms"]=std::to_string(Fnv(cms.data(),cms.size())); out["arena"]=std::to_string(Fnv(arena.data(),arenaTop)); out["mi"]=MisHash();
                        if(isFile)out["colbuf"]=std::to_string(Fnv(reinterpret_cast<void*>(COLBUF),0x2000));
                    }
                    // ====================== LinkLods ======================
                    else if(nm=="LinkLods") {
                        BuildMis(next);
                        for(int k=0;k<NMI;++k){ auto& m=mis[k]; ptrs[miId[k]]=reinterpret_cast<CBaseModelInfo*>(m.data()); At<float>(m.data(),0x18)=float(int32(next()%50000))/100.f;
                            At<void*>(m.data(),0x14)=next()%4 ? colModels[next()%4].data() : nullptr; }
                        const int32 n=int32(next()%13), rel=int32(next()%5);
                        auto* arr=reinterpret_cast<uint8**>(GLOADED);
                        for(int32 k=0;k<n+rel;++k){ auto* e=ents[k].data(); for(int q=0;q<0x38;++q)e[q]=uint8(next()); At<int16>(e,0x22)=int16(miId[next()%NMI]); e[0x34]=uint8(next()%3);
                            if(k<n)At<int32>(e,0x30)=next()%3==0 ? -1 : int32(next()%uint32(n+rel)); else At<void*>(e,0x30)=next()%2 ? nullptr : ents[next()%uint32(n+rel)].data(); arr[k]=e; }
                        *reinterpret_cast<uint32*>(GNUM)=uint32(n); *reinterpret_cast<float*>(LODMULT)=float(int32(next()%300)+50)/100.f;
                        P(pCacheLoading,FakeIsCacheLoading); P(pAddIpl,FakeAddIplEntity); P(pCacheIpl,FakeCacheIplSection); P(pSetupBig,FakeSetupBig); P(pDeleteCol,FakeDeleteCol); P(pSetCol,FakeSetColModel); P(pWorldAdd,FakeWorldAdd);
                        reinterpret_cast<void(__cdecl*)(int32)>(a)(rel);
                        Unpatch();
                        std::array<uint8,sizeof(ents)> eb{}; std::memcpy(eb.data(),ents.data(),sizeof(ents)); for(int k=0;k<int(ents.size());++k)At<uint32>(eb.data()+k*0x38,0x30)=N(At<void*>(ents[k].data(),0x30));
                        out["ents"]=std::to_string(Fnv(eb.data(),eb.size())); out["mi"]=MisHash();
                    }
                    // ====================== IDE / IPL / DAT completos ======================
                    else if(nm=="LoadObjectTypes"||nm=="LoadScene"||nm=="LoadLevel") {
                        nlines=0; lineAt=0;
                        auto Add=[&](const char* s){ if(nlines<int(lines.size()))strcpy(lines[nlines++],s); };
                        char tmp[200];
                        auto Data=[&]{ Nm(next,n1,12); snprintf(tmp,sizeof(tmp),"%s%d, %s %d.%d",next()%5==0 ? "  " : "",int(next()%2000),n1,int(next()%100),int(next()%10)); Add(tmp); };
                        if(nm=="LoadObjectTypes"){
                            static const char* const secs[]{"objs","tobj","weap","hier","anim","cars","peds","path","2dfx","txdp","mlo","junk","objsX"};
                            while(nlines<40){ const int r=int(next()%10); if(r==0)Add("# comment"); else if(r==1)Add(""); else if(r==2)Add("end");
                                else { const char* s=secs[next()%std::size(secs)]; Add(s); const int nd=std::string_view{s}=="path" ? int(next()%28) : int(next()%5); for(int k=0;k<nd&&nlines<44;++k)Data(); if(next()%3)Add("end"); } }
                            P(pReadLine,FakeReadLine); P(pOpen,FakeOpenFile); P(pClose,FakeCloseFile);
                            P(lObject,&LineStub<1>); P(lTime,&LineStub<3>); P(lWeapon,&LineStub<4>); P(lClump,&LineStub<5>); P(lAnim,&LineStub<6>); P(lVehicle,&LineStub<7>); P(lPed,&LineStub<8>);
                            P(lPath,PathHeaderStub); P(lPedNode,PedNodeStub); P(lCarNode,CarNodeStub); P(l2dfx,&LineStub<10>); P(lTxdp,&LineStub<11>);
                        } else if(nm=="LoadScene"){
                            static const char* const secs[]{"inst","mult","zone","cull","occl","grge","enex","pick","cars","jump","tcyc","auzo","junk","instX"};
                            while(nlines<40){ const int r=int(next()%10); if(r==0)Add("# comment"); else if(r==1)Add(""); else if(r==2)Add("end");
                                else { Add(secs[next()%std::size(secs)]); const int nd=int(next()%5); for(int k=0;k<nd&&nlines<44;++k)Data(); if(next()%3)Add("end"); } }
                            if(next()%4==0){ Add("path"); Data(); Data(); }
                            for(auto& p:iplArray)p=reinterpret_cast<CEntity*>(uintptr_t(next()));
                            P(pReadLine,FakeReadLine); P(pOpen,FakeOpenFile); P(pClose,FakeCloseFile);
                            P(lInstFile,InstStub); P(lZone,&LineStub<4>); P(lCull,&LineStub<5>); P(lOccl,OcclStub); P(lGarage,&LineStub<8>); P(lEnex,&LineStub<9>); P(lPickup,&LineStub<10>);
                            P(lCarGen,CarGenStub); P(lJump,&LineStub<12>); P(lTcyc,&LineStub<13>); P(lAudio,&LineStub<14>); P(lPath,PathHeaderStub); P(lPedNode,PedNodeStub); P(lCarNode,CarNodeStub);
                            P(pNewIpl,NewIplArrayStub); P(pGetIpl,GetIplArrayStub); P(pSetupRelated,SetupRelatedStub); P(pLinkLods,LinkLodsStub); P(pRemoveRelated,RemoveRelatedStub);
                        } else {
                            static const char* const cmds[]{"TEXDICTION","IMG","COLFILE 0","MODELFILE","HIERFILE","IDE","IPL","SPLASH","EXITX","IDEX","XYZ"};
                            static const char* const paths[]{"MODELS\\GTA_INT.IMG","MODELS\\GTA3.IMG","DATA\\MAPS\\LA\\LAn.ide","DATA\\MAPS\\generic.ipl","models\\generic\\vehicle.txd","particle.txd","x"};
                            while(nlines<30){ const int r=int(next()%12); if(r==0)Add("# comment"); else if(r==1)Add(""); else { snprintf(tmp,sizeof(tmp),"%s %s",cmds[next()%std::size(cmds)],paths[next()%std::size(paths)]); Add(tmp); } }
                            if(next()%3==0){ const int at=int(next()%uint32(nlines)); strcpy(lines[at],"EXIT"); }
                            std::memset(ptrs,0,20000*4); for(int k=0;k<4;++k)ptrs[miId[k]]=reinterpret_cast<CBaseModelInfo*>(mis[k].data());
                            for(int k=0;k<NMI;++k){ auto& m=mis[k]; for(auto& b:m)b=uint8(next()); At<void*>(m.data(),0)=fakeVt.data(); }
                            txdCurrent=next()%3 ? reinterpret_cast<RwTexDictionary*>(txds[3].data()) : nullptr; txdCreateResult=reinterpret_cast<RwTexDictionary*>(txds[4].data());
                            auto* gs=reinterpret_cast<uint8*>(GSTRING); for(int k=0;k<352;++k)gs[k]=uint8(next());
                            P(pReadLine,FakeReadLine); P(pOpen,FakeOpenFile); P(pClose,FakeCloseFile);
                            P(rwTxdGet,FakeTxdGetCurrent); P(rwTxdCreate,FakeTxdCreate); P(rwTxdSet,FakeTxdSetCurrent); P(rwForAllTex,FakeForAllTextures); P(rwTxdDestroy,FakeTxdDestroy);
                            P(lvTxd,LoadTxdStub); P(lvScreen,LoadingScreenStub); P(lvFile,LoadingFileStub); P(lvImage,AddImageStub); P(lvCol,ColFileStub); P(lvAtomic,&NameStub<1>); P(lvClump,&NameStub<2>);
                            P(lvIde,&NameStub<3>); P(lvIpl,&NameStub<4>); P(lvMatch,&VoidStub<1>); P(lvObjData,ObjectDataStub); P(lvCommon,&VoidStub<2>); P(lvInit2,&VoidStub<3>); P(lvChunk,&VoidStub<4>);
                            P(lvBoxes,&VoidStub<5>); P(lvRemaining,&VoidStub<6>); P(lvPost,&VoidStub<7>); P(lvTrains,&VoidStub<8>); P(lvRemoveCol,&VoidStub<9>);
                        }
                        Nm(next,n1,30); snprintf(n2,sizeof(n2),"DATA\\MAPS\\%s.ipl",n1);
                        reinterpret_cast<void(__cdecl*)(const char*)>(a)(n2);
                        Unpatch();
                        out["line"]=std::to_string(Fnv(reinterpret_cast<void*>(MS_LINE),512)); out["lines"]=lineAt;
                        if(nm=="LoadScene"){ out["num"]=*reinterpret_cast<uint32*>(GNUM); out["loaded"]=std::to_string(Fnv(reinterpret_cast<void*>(GLOADED),64*4)); out["ipl"]=std::to_string(Fnv(iplArray.data(),sizeof(iplArray))); }
                        if(nm=="LoadLevel"){ out["gstring"]=std::to_string(Fnv(reinterpret_cast<void*>(GSTRING),352)); out["txd"]=N(txdCurrent); }
                    }
                    out["ret"]=ret; out["trace"]=Trace();
                    restoreAll();
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
    restoreAll();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",stateRestored()}});
}
}
