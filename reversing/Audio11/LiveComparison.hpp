#pragma once
// Banco dentro del motor: voz sin peatón, decodificador WMA, ropa y aceleración de colisiones (lista de 11).
// 43 funciones de CAEPedlessSpeechAudioEntity, CAEWMADecoder, CClothes y CColAccel. Original (hooks del lote
// desactivados) frente a candidato con fixtures deterministas. Nada toca audio, Windows Media, ficheros ni pools reales:
// - CAEPedlessSpeechAudioEntity trabaja sobre un objeto falso (vtable del EXE 0x85F330) y entidades falsas; se simulan
//   CAESound (Initialise, SetPosition, RegisterWithPhysicalEntity, StopSoundAndForget), CAESoundManager::RequestNewSound,
//   CEntity::RegisterReference/CleanUpOldReference, las consultas de CAEPedSpeechAudioEntity (pantalla ancha, streaming,
//   hueco, sonido y banco, próxima vez, carga y reproducción), CAEAudioUtility::ResolveProbability y
//   CStreaming::IsVeryBusy. La cámara se coloca con m_matrix nulo y posición propia; los huecos de voz globales
//   (0xB61C38), las banderas de voz, el estado del juego y el contador de peticiones de streaming se restauran.
// - CAEWMADecoder trabaja sobre un objeto falso con lector, perfil, flujos, propiedades, cabecera y muestras COM falsos
//   (vtables propias trazadas). LoadLibraryA/GetProcAddress/FreeLibrary/CoInitialize se sustituyen por IAT solo en el
//   hilo principal; WMCreateSyncReader (0xB6BAD8) y el módulo (0xB6BAD4) se fijan y se restauran. operator new/delete y
//   CMemoryMgr::Malloc/Free se simulan durante la llamada.
// - CClothes usa un jugador, inteligencia, datos de jugador, ropa, tarea, clump, info de modelo, pool de TXD y grupos de
//   animación falsos; se simulan CStats::GetStatValue, CStreaming (AddImageToList, RequestModel, SetModelIsDeletable,
//   LoadAllRequestedModels), CTimer::Suspend/Resume, CClothesBuilder::CreateSkinnedClump, Add/RemoveTexDictionaryRef,
//   RpAnimBlendClumpExtract/GiveAssociations, CWorld::Remove, CPed::Dress y el fichero de reglas. PlayerClothes y las
//   reglas (0xBC12F8..0xBC1CF0) se guardan y se restauran; GetTextureDependency queda con su estado (envoltorio).
// - CColAccel trabaja sobre arrays falsos colgados de sus globales (0xBC4090..0xBC40BC, restaurados); entidades e infos
//   de modelo falsas en los índices 19990..19997 de CModelInfo::ms_modelInfoPtrs (restaurados); se simulan CFileMgr,
//   operator new[]/delete[], CColModel::operator new, SetColModel, DeleteCollisionModel y CColStore::IncludeModelIndex.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Audio/Entities/AEPedlessSpeechAudioEntity.h"
#include "Audio/Loaders/AEWMADecoder.h"
#include "Audio/AEAudioUtility.h"
#include "Audio/Managers/AESoundManager.h"
#include "Clothes.h"
#include "ClothesBuilder.h"
#include "PedClothesDesc.h"
#include "Collision/ColAccel.h"
#include "Collision/ColStore.h"
#include "Collision/ColModel.h"
#include "Stats.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "MemoryMgr.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"
#include "../ProbeRuntime.hpp"

namespace Audio11Live {
inline bool done{}; inline std::ofstream log; inline json trace;
// Las trazas de los falsos pueden emitirse mientras operator new/delete del EXE están sustituidos: el json se reserva
// entonces con malloc/free del EXE (0x824257/0x82413F), que es lo que usan operator new/delete fuera de la ventana.
inline int fakeDepth{};
#define TRACE(...) do { ++fakeDepth; trace.push_back(__VA_ARGS__); --fakeDepth; } while(0)
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint64 Hash64(const void* p,size_t n){ uint64 h=0x9E3779B97F4A7C15ull; auto* w=static_cast<const uint64*>(p); for(size_t i=0;i<n/8;++i){h^=w[i];h*=0x100000001B3ull;h^=h>>29;} return Fnv(static_cast<const uint8*>(p)+n/8*8,n%8,h); }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }
inline json Str(const char* s){ return s ? json(std::string(s,strnlen(s,64))) : json(nullptr); }

// --- registro sin reservas (mientras operator new/delete están sustituidos) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,256> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(json(*name)(const void*)=nullptr){ json r=json::array(); for(size_t i=0;i<nrec;++i){ const auto& c=recs[i];
    if(c.tag==2)r.push_back({c.tag,name ? name(reinterpret_cast<void*>(uintptr_t(c.a))) : json(c.a)}); else r.push_back({c.tag,c.a,c.b}); } return r; }
alignas(16) inline std::array<uint8,0x6000> arena{}; inline size_t arenaTop{}; inline uint32 newNullMask{}, newCount{};
inline uint32 Off(const void* p){
    const auto u=uintptr_t(p);
    if(!u)return 0xFFFFFFFFu;
    if(u>=uintptr_t(arena.data())&&u<uintptr_t(arena.data())+arena.size())return uint32(u-uintptr_t(arena.data()));
    return 0xEEEEEEEEu; }
inline void* __cdecl FakeNew(size_t n){
    if(fakeDepth)return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n ? n : 1);
    Push(1,uint32(n),newCount); const bool fail=(newNullMask>>(newCount++%32))&1;
    if(fail||arenaTop+n>arena.size())return nullptr;
    void* p=arena.data()+arenaTop; arenaTop=(arenaTop+n+15)&~size_t(15); return p; }
inline void __cdecl FakeDelete(void* p){ if(fakeDepth){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; } Push(2,uint32(uintptr_t(p))); }
inline void ResetArena(){ arena.fill(0xCD); arenaTop=0; newCount=0; nrec=0; }

// --- Win32 por IAT (solo el hilo principal; los demás hilos van a la API real) ---
inline HMODULE SelfModule(){ HMODULE m{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&SelfModule),&m); return m; }
inline uintptr_t ApiIn(const char* dll,const char* fn){ if(auto m=GetModuleHandleA(dll))if(auto p=GetProcAddress(m,fn))return uintptr_t(p); return 0; }
struct IatPatch {
    std::vector<std::pair<uintptr_t*,uintptr_t>> slots;
    static void Set(uintptr_t* t,uintptr_t v){ DWORD o{}; VirtualProtect(t,4,PAGE_READWRITE,&o); *t=v; VirtualProtect(t,4,o,&o); }
    template<class Fp> IatPatch(std::initializer_list<const char*> dlls,const char* fn,Fp fakeFn) {
        const auto fake=reinterpret_cast<uintptr_t>(fakeFn);
        std::vector<uintptr_t> reals; for(auto* d:dlls)if(auto r=ApiIn(d,fn))reals.push_back(r);
        for(HMODULE m:{GetModuleHandleA(nullptr),SelfModule()}) {
            auto* base=reinterpret_cast<uint8*>(m); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
            const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)continue;
            for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
                for(auto* t=reinterpret_cast<uintptr_t*>(base+d->FirstThunk);*t;++t)
                    if(std::ranges::find(reals,*t)!=reals.end()){ slots.push_back({t,*t}); Set(t,fake); }
        }
    }
    ~IatPatch(){ for(auto& [t,v]:slots)Set(t,v); }
};

// --- copia de regiones globales ---
struct Saved { uintptr_t at; std::vector<uint8> bytes; };
inline Saved Save(uintptr_t at,size_t n){ return {at,ReversingProbe::Bytes(reinterpret_cast<void*>(at),n)}; }
inline void Restore(const Saved& s){ std::memcpy(reinterpret_cast<void*>(s.at),s.bytes.data(),s.bytes.size()); }
inline bool Same(const Saved& s){ return ReversingProbe::Bytes(reinterpret_cast<void*>(s.at),s.bytes.size())==s.bytes; }

// ======================= CAEPedlessSpeechAudioEntity =======================
alignas(16) inline std::array<uint8,0x118> ps{};
alignas(16) inline std::array<std::array<uint8,0x40>,6> pents{};
alignas(16) inline std::array<std::array<uint8,0x48>,6> pmats{};
alignas(16) inline std::array<std::array<uint8,0x74>,4> psnds{};
constexpr uintptr_t SLOTS=0xB61C38; constexpr size_t SLOTS_SIZE=6*0x1C;
inline json PEnt(const void* p){ for(int k=0;k<6;++k)if(p==pents[k].data())return k; return p ? json("other") : json(nullptr); }
inline json PSnd(const void* p){ if(p==ps.data()+8)return "temp"; for(int k=0;k<4;++k)if(p==psnds[k].data())return k; return p ? json("other") : json(nullptr); }
inline json PRef(const void* p){ const auto u=uintptr_t(p); if(u>=uintptr_t(ps.data())&&u<uintptr_t(ps.data())+ps.size())return int(u-uintptr_t(ps.data())); return "other"; }
inline uint32 pMask{}, pSlotRet{}, pSoundRet{}, pSpecific{}, pReqRet{};
inline bool PBit(int b){ return (pMask>>b)&1; }
inline void __fastcall FakeSndInit(uint8* s,void*,uint32 bank,uint32 sfx,void* ent,float x,float y,float z,float vol,float roll,float speed,float dop,uint32 delay,uint32 flags,float var,uint32 play){
    TRACE({"snd_init",PSnd(s),bank&0xFFFF,sfx&0xFFFF,ent==ps.data(),F(x),F(y),F(z),F(vol),F(roll),F(speed),F(dop),delay&0xFF,flags&0xFFFF,F(var),play&0xFFFF}); }
inline void __fastcall FakeSndSetPos(uint8* s,void*,float x,float y,float z){ TRACE({"snd_set_pos",PSnd(s),F(x),F(y),F(z)}); }
inline void __fastcall FakeSndRegister(uint8* s,void*,void* e){ TRACE({"snd_register",PSnd(s),PEnt(e)}); }
inline void __fastcall FakeSndStop(uint8* s,void*){ TRACE({"snd_stop_forget",PSnd(s)}); }
inline void* __fastcall FakeReqNew(void* m,void*,void* s){ TRACE({"request_new_sound",uintptr_t(m)==0xB62CB0,PSnd(s)}); return pReqRet<4 ? static_cast<void*>(psnds[pReqRet].data()) : nullptr; }
inline void __fastcall FakeCleanUp(void* e,void*,void* ref){ TRACE({"cleanup_ref",PEnt(e),PRef(ref)}); }
inline void __fastcall FakeRegRef(void* e,void*,void* ref){ TRACE({"register_ref",PEnt(e),PRef(ref)}); }
inline bool __fastcall FakeWide(void* s,void*,int32 g){ TRACE({"widescreen_ctx",s==ps.data(),int16(g)}); return PBit(0); }
inline bool __fastcall FakeStreamCtx(void* s,void*,int32 g){ TRACE({"streaming_ctx",s==ps.data(),int16(g)}); return PBit(1); }
inline int16 __fastcall FakeCanWePlay(void* s,void*,int32 g){ TRACE({"can_we_play",s==ps.data(),int16(g)}); return int16(pSlotRet); }
inline int16 __fastcall FakeGetIds(void* s,void*,int32 g,int16* out){ TRACE({"get_sound_ids",s==ps.data(),int16(g)}); *out=int16(pSpecific); /* eSpecificSpeechContext es int16 */ return int16(pSoundRet); }
inline void __fastcall FakeSetNext(void* s,void*,int32 g){ TRACE({"set_next_play_time",s==ps.data(),int16(g)}); }
inline void __fastcall FakeLoadPlay(void* s,void*,uint32 t){ TRACE({"load_and_play",s==ps.data(),t}); }
inline bool __cdecl FakeProb(float p){ TRACE({"resolve_probability",F(p)}); return PBit(2); }
inline bool __cdecl FakeBusy(){ TRACE({"very_busy"}); return PBit(3); }

// ======================= CAEWMADecoder =======================
constexpr size_t WMA_SIZE=0x20038;
alignas(16) inline std::array<uint8,WMA_SIZE> wd{};
struct Com { void** vt; uint32 id; uint32 pad[2]; };
alignas(16) inline Com cReader{}, cProfile{}, cProps{}, cHeader{};
alignas(16) inline std::array<Com,6> cStreams{}; alignas(16) inline std::array<Com,8> cBufs{};
inline std::array<void*,32> vtReader{}, vtProfile{}, vtStream{}, vtProps{}, vtHeader{}, vtBuf{};
alignas(16) inline std::array<uint8,0x20> wStream{};
alignas(16) inline std::array<uint8,0x80> mediaBuf{}; alignas(16) inline std::array<uint8,0x20> waveFmt{};
alignas(16) inline std::array<std::array<uint8,0x22100>,3> sampleData{};
alignas(16) inline std::array<uint8,0x24100> wDest{};
inline json CName(const void* p){
    if(!p)return nullptr; if(p==&cReader)return "reader"; if(p==&cProfile)return "profile"; if(p==&cProps)return "props"; if(p==&cHeader)return "header";
    for(int k=0;k<6;++k)if(p==&cStreams[k])return "stream"+std::to_string(k);
    for(int k=0;k<8;++k)if(p==&cBufs[k])return "buf"+std::to_string(k);
    if(p==wStream.data())return "datastream"; if(p==mediaBuf.data())return "media";
    if(uintptr_t(p)>=uintptr_t(arena.data())&&uintptr_t(p)<uintptr_t(arena.data())+arena.size())return json::array({"arena",uintptr_t(p)-uintptr_t(arena.data())}); return "other"; }
struct WSample { HRESULT hr, bhr; uint16 stream; uint32 len; uint32 data; };
inline std::array<WSample,8> wSamples{}; inline uint32 wNumSamples{}, wSampleIdx{};
inline uint32 wMask{}; inline uint32 wStreamCount{}; inline std::array<uint32,6> wStreamNum{}, wStreamAudio{}; inline std::array<HRESULT,6> wGetStreamHr{}, wNumHr{}, wTypeHr{};
inline HRESULT wCountHr{}, wCreateHr{}, wOpenHr{}, wQiProfileHr{}, wQiHeaderHr{}, wPropsHr{};
inline uint32 wMediaSize{}, wAttrCount{}, wOutputNum{}; inline uint64 wDuration{}; inline uint64 wTime{}, wDur{};
inline bool WBit(int b){ return (wMask>>b)&1; }
inline ULONG __stdcall CRelease(Com* s){ TRACE({"release",CName(s)}); return 0; }
inline HRESULT __stdcall CBad(Com* s){ TRACE({"bad_slot",CName(s)}); return E_NOTIMPL; }
inline HRESULT __stdcall RQi(Com* s,const GUID* iid,void** out){
    const bool prof=std::memcmp(iid,reinterpret_cast<void*>(0x85F840),16)==0, head=std::memcmp(iid,reinterpret_cast<void*>(0x85F880),16)==0;
    TRACE({"query_interface",CName(s),prof ? "profile" : head ? "header3" : "other"});
    const HRESULT hr=prof ? wQiProfileHr : head ? wQiHeaderHr : E_NOINTERFACE;
    *out=SUCCEEDED(hr) ? (prof ? static_cast<void*>(&cProfile) : head ? static_cast<void*>(&cHeader) : nullptr) : nullptr; return hr; }
inline HRESULT __stdcall RClose(Com* s){ TRACE({"close",CName(s)}); return S_OK; }
inline HRESULT __stdcall RSetRange(Com* s,uint64 start,int64 dur){ TRACE({"set_range",CName(s),start,dur}); return S_OK; }
inline HRESULT __stdcall RNext(Com* s,WORD stream,INSSBuffer** buf,uint64* time,uint64* dur,DWORD* flags,DWORD* outNum,WORD* streamOut){
    TRACE({"get_next_sample",CName(s),stream});
    if(wSampleIdx>=wNumSamples)return HRESULT(0xC00D0BCFu); /* NS_E_NO_MORE_SAMPLES */
    const auto k=wSampleIdx++; const auto& smp=wSamples[k];
    if(FAILED(smp.hr))return smp.hr;
    *buf=reinterpret_cast<INSSBuffer*>(&cBufs[k]); *time=wTime+k*1000; *dur=wDur+k; *flags=k; *outNum=k+1; *streamOut=smp.stream; return smp.hr; }
inline HRESULT __stdcall RSelect(Com* s,WORD count,WORD* streams,WMT_STREAM_SELECTION* sel){ TRACE({"set_streams_selected",CName(s),count,streams ? streams[0] : 0xFFFF,sel ? int(*sel) : -1}); return S_OK; }
inline HRESULT __stdcall RReadSamples(Com* s,WORD stream,BOOL comp){ TRACE({"set_read_stream_samples",CName(s),stream,comp}); return S_OK; }
inline HRESULT __stdcall RProps(Com* s,DWORD out,IWMOutputMediaProps** p){ TRACE({"get_output_props",CName(s),out}); *p=reinterpret_cast<IWMOutputMediaProps*>(&cProps); return wPropsHr; }
inline HRESULT __stdcall ROutNum(Com* s,WORD stream,DWORD* out){ TRACE({"get_output_number",CName(s),stream}); *out=wOutputNum; return S_OK; }
inline HRESULT __stdcall ROpenStream(Com* s,void* st){ TRACE({"open_stream",CName(s),CName(st)}); return wOpenHr; }
inline HRESULT __stdcall PCount(Com* s,DWORD* n){ TRACE({"get_stream_count",CName(s)}); *n=wStreamCount; return wCountHr; }
inline HRESULT __stdcall PGetStream(Com* s,DWORD i,void** out){ TRACE({"get_stream",CName(s),i}); const HRESULT hr=i<6 ? wGetStreamHr[i] : E_FAIL; *out=SUCCEEDED(hr)&&i<6 ? static_cast<void*>(&cStreams[i]) : nullptr; return hr; }
inline int StreamIdx(const Com* s){ for(int k=0;k<6;++k)if(s==&cStreams[k])return k; return 0; }
inline HRESULT __stdcall SType(Com* s,GUID* g){ const int k=StreamIdx(s); TRACE({"get_stream_type",CName(s)});
    if(wStreamAudio[k])std::memcpy(g,reinterpret_cast<void*>(0x85FB70),16); else { std::memcpy(g,reinterpret_cast<void*>(0x85FB70),16); reinterpret_cast<uint8*>(g)[k%16]^=uint8(1+wStreamAudio[k]+k); } return wTypeHr[k]; }
inline HRESULT __stdcall SNum(Com* s,WORD* n){ const int k=StreamIdx(s); TRACE({"get_stream_number",CName(s)}); *n=uint16(wStreamNum[k]); return wNumHr[k]; }
inline HRESULT __stdcall MType(Com* s,WM_MEDIA_TYPE* mt,DWORD* size){
    TRACE({"get_media_type",CName(s),CName(mt),mt ? json(*size) : json(nullptr)}); // la primera vez el EXE pasa un tamaño sin inicializar
    if(!mt){ *size=wMediaSize; return S_OK; }
    reinterpret_cast<uint8**>(reinterpret_cast<uint8*>(mt)+0x44)[0]=WBit(10) ? nullptr : waveFmt.data(); return S_OK; }
inline std::string Narrow(const wchar_t* w){ std::string r; if(w)for(int k=0;k<32&&w[k];++k)r.push_back(char(w[k])); return r; }
inline HRESULT __stdcall HIndices(Com* s,WORD stream,LPCWSTR name,WORD* lang,WORD* idx,WORD* count){
    TRACE({"get_attribute_indices",CName(s),stream,Narrow(name),lang!=nullptr,Off(idx),idx ? json(*count) : json(nullptr)}); // sin índices, el EXE pasa un contador sin inicializar
    if(idx){ for(uint32 k=0;k<std::min<uint32>(*count,wAttrCount);++k)idx[k]=uint16(3+k*2); } *count=uint16(wAttrCount); return S_OK; }
inline HRESULT __stdcall HByIndex(Com* s,WORD stream,WORD index,LPWSTR name,WORD* nameLen,WMT_ATTR_DATATYPE* type,WORD* lang,BYTE* value,DWORD* len){
    TRACE({"get_attribute_by_index",CName(s),stream,index,name!=nullptr,lang!=nullptr,*len});
    *nameLen=9; *type=WMT_TYPE_QWORD; std::memcpy(value,&wDuration,8); return S_OK; }
inline HRESULT __stdcall BGetBuffer(Com* s,BYTE** p,DWORD* len){
    int k=0; for(int j=0;j<8;++j)if(s==&cBufs[j])k=j; TRACE({"get_buffer_and_length",CName(s)});
    *p=sampleData[k%3].data()+(k/3)*0x40; *len=wSamples[k].len; return wSamples[k].bhr; }
inline HRESULT __stdcall FakeCreateReader(IUnknown* u,DWORD rights,IWMSyncReader** out){
    TRACE({"wm_create_sync_reader",u!=nullptr,rights}); *out=SUCCEEDED(wCreateHr) ? reinterpret_cast<IWMSyncReader*>(&cReader) : nullptr; return wCreateHr; }
inline HMODULE wLoadRet{}; inline uintptr_t wProcRet{};
inline decltype(&LoadLibraryA) realLoadLibraryA{}; inline decltype(&GetProcAddress) realGetProcAddress{}; inline decltype(&FreeLibrary) realFreeLibrary{}; inline decltype(&CoInitialize) realCoInitialize{};
constexpr uintptr_t FAKE_MODULE=0x7FFE0000;
inline HMODULE WINAPI FakeLoadLibraryA(LPCSTR n){
    if(!Main())return realLoadLibraryA(n);
    TRACE({"load_library",Str(n)}); return wLoadRet; }
inline FARPROC WINAPI FakeGetProcAddress(HMODULE m,LPCSTR n){
    if(!Main()||(uintptr_t(m)!=FAKE_MODULE&&m!=nullptr&&uintptr_t(m)!=0xFFFFFFFFu))return realGetProcAddress(m,n);
    TRACE({"get_proc_address",uint32(uintptr_t(m)),uintptr_t(n)<0x10000 ? json(uintptr_t(n)) : Str(n)}); return reinterpret_cast<FARPROC>(wProcRet); }
inline BOOL WINAPI FakeFreeLibrary(HMODULE m){
    if(!Main()||(uintptr_t(m)!=FAKE_MODULE&&m!=nullptr&&uintptr_t(m)!=0xFFFFFFFFu))return realFreeLibrary(m);
    TRACE({"free_library",uint32(uintptr_t(m))}); return TRUE; }
inline HRESULT WINAPI FakeCoInitialize(LPVOID r){
    if(!Main())return realCoInitialize(r);
    TRACE({"co_initialize",r!=nullptr}); return WBit(11) ? S_FALSE : S_OK; }
inline void* __cdecl FakeMalloc(uint32 n,uint32 /*hint*/){ TRACE({"mem_malloc",n}); return mediaBuf.data(); }
inline void __cdecl FakeFree(void* p){ TRACE({"mem_free",CName(p)}); }

// ======================= CClothes =======================
constexpr int MI_FAKE=19990;
alignas(16) inline std::array<uint8,0x800> cPlayer{};
alignas(16) inline std::array<uint8,0x100> cIntel{};
alignas(16) inline std::array<uint8,0x40> cPlayerData{};
alignas(16) inline std::array<uint8,0x78> cDesc{};
alignas(16) inline std::array<uint8,0x40> cTask{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> cClumps{};
alignas(16) inline std::array<uint8,0x80> cMi{};
alignas(16) inline std::array<uint8,0x14> cPool{}; alignas(16) inline std::array<uint8,8*12> cPoolObjs{}; alignas(16) inline std::array<uint8,8> cPoolFlags{};
alignas(16) inline std::array<uint8,0x40*0x14> cGroups{}; alignas(16) inline std::array<std::array<uint8,0x20>,2> cBlocks{};
inline std::array<void*,96> vtPlayer{}; inline std::array<void*,32> vtMi{}, vtTask{};
inline float statFat{}, statMus{}; inline uint32 cMask{};
inline bool CBit(int b){ return (cMask>>b)&1; }
inline json CObj(const void* p){
    if(!p)return nullptr; if(p==cPlayer.data())return "player"; if(p==cDesc.data())return "desc"; if(p==cMi.data())return "mi"; if(p==cTask.data())return "task";
    for(int k=0;k<3;++k)if(p==cClumps[k].data())return "clump"+std::to_string(k);
    if(uintptr_t(p)==0xBC1C78)return "PlayerClothes"; if(uintptr_t(p)<0x100000)return uintptr_t(p); return "other"; }
inline float __cdecl FakeStat(int32 s){ TRACE({"get_stat",s}); return s==0x15 ? statFat : s==0x17 ? statMus : 0.f; }
inline uint32 __cdecl FakeAddImage(const char* n,uint32 notPlayer){ TRACE({"add_image",Str(n),notPlayer&0xFF}); return 7; }
inline void __cdecl FakeSuspend(){ TRACE({"timer_suspend"}); }
inline void __cdecl FakeResume(){ TRACE({"timer_resume"}); }
inline void* __cdecl FakeSkinned(void* clump,void* dict,void* nc,void* oc,uint32 cut){ TRACE({"create_skinned_clump",CObj(clump),uintptr_t(dict),CObj(nc),CObj(oc),cut&0xFF}); return CBit(0) ? nullptr : cClumps[2].data(); }
inline void __fastcall FakeAddTexRef(void* mi,void*){ TRACE({"add_tex_ref",CObj(mi)}); }
inline void __fastcall FakeRemoveTexRef(void* mi,void*){ TRACE({"remove_tex_ref",CObj(mi)}); }
inline void __cdecl FakeLoadAll(uint32 b){ TRACE({"load_all_requested",b&0xFF}); }
inline void __cdecl FakeRequestModel(int32 id,int32 fl){ TRACE({"request_model",id,fl}); }
inline void __cdecl FakeSetDeletable(int32 id){ TRACE({"set_model_deletable",id}); }
inline void* __cdecl FakeExtract(void* c){ TRACE({"extract_associations",CObj(c)}); return reinterpret_cast<void*>(uintptr_t(0x5A5A0)); }
inline void __cdecl FakeGive(void* c,void* a){ TRACE({"give_associations",CObj(c),uintptr_t(a)}); }
inline void __cdecl FakeWorldRemove(void* e){ TRACE({"world_remove",CObj(e)}); }
inline void __fastcall FakeDress(void* p,void*){ TRACE({"dress",CObj(p)}); }
inline void __fastcall FakeDeleteRw(void* p,void*){ TRACE({"delete_rw_object",CObj(p)}); }
inline void __fastcall FakeSetClump(void* p,void*,void* c){ TRACE({"set_clump",CObj(p),CObj(c)}); }
inline bool __fastcall FakeMakeAbortable(void* t,void*,void* ped,int32 prio,void* ev){ TRACE({"make_abortable",CObj(t),CObj(ped),prio,ev!=nullptr}); return true; }
inline void __fastcall FakeVBad(void* p,void*){ TRACE({"bad_slot",CObj(p)}); }
// fichero de reglas
inline std::vector<std::string> lines; inline size_t lineIdx{}; inline std::array<char,512> lineBuf{};
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ TRACE({"open",Str(p),Str(m)}); return reinterpret_cast<FILESTREAM>(uintptr_t(0x4321)); }
inline FILESTREAM __cdecl FakeOpenForWriting(const char* p){ TRACE({"open_for_writing",Str(p)}); return reinterpret_cast<FILESTREAM>(uintptr_t(0x4322)); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ TRACE({"close",uintptr_t(f)}); return 0; }
inline char* __cdecl FakeLoadLine(FILESTREAM f){ TRACE({"load_line",uintptr_t(f)}); if(lineIdx>=lines.size())return nullptr; lineBuf.fill(0); std::memcpy(lineBuf.data(),lines[lineIdx].data(),std::min(lines[lineIdx].size(),lineBuf.size()-1)); ++lineIdx; return lineBuf.data(); }

// ======================= CColAccel =======================
constexpr uintptr_t CA=0xBC4090; constexpr size_t CA_SIZE=0x2C;
alignas(16) inline std::array<uint8,16*0x30> aCol{};
alignas(16) inline std::array<uint8,32*0x14> aIpl{};
alignas(16) inline std::array<int32,16> aSect{};
alignas(16) inline std::array<uint8,256*0x34> aDefs{};
alignas(16) inline std::array<uint8,16*0x18> aBounds{};
alignas(16) inline std::array<uint8,0x30> aColModel{};
alignas(16) inline std::array<std::array<uint8,0x30>,4> aNewCol{}; inline uint32 aNewIdx{};
alignas(16) inline std::array<std::array<uint8,0x40>,12> aEnts{}; inline std::array<void*,12> aEntPtrs{};
alignas(16) inline std::array<std::array<uint8,0x40>,8> aMi{};
alignas(16) inline std::array<std::array<uint8,0x30>,3> aColModels{};
inline json APtr(const void* p){
    if(!p)return nullptr; const auto u=uintptr_t(p);
    auto in=[&](const auto& a){ return u>=uintptr_t(a.data())&&u<uintptr_t(a.data())+sizeof(a); };
    if(in(aCol))return json::array({"col",u-uintptr_t(aCol.data())}); if(in(aIpl))return json::array({"ipl",u-uintptr_t(aIpl.data())});
    if(in(aSect))return json::array({"sect",u-uintptr_t(aSect.data())}); if(in(aDefs))return json::array({"defs",u-uintptr_t(aDefs.data())});
    if(in(aBounds))return json::array({"bounds",u-uintptr_t(aBounds.data())}); if(in(arena))return json::array({"arena",u-uintptr_t(arena.data())});
    if(in(aNewCol))return json::array({"newcol",u-uintptr_t(aNewCol.data())}); if(in(aMi))return json::array({"mi",(u-uintptr_t(aMi.data()))/0x40});
    if(in(aColModels))return json::array({"cm",(u-uintptr_t(aColModels.data()))/0x30});
    if(u>=CA&&u<CA+CA_SIZE)return json::array({"global",u-CA}); return "other"; }
inline size_t __cdecl FakeWrite(FILESTREAM f,const void* buf,size_t n){ TRACE({"write",uintptr_t(f),APtr(buf),n,buf ? Fnv(buf,std::min<size_t>(n,0x3400)) : 0}); return n; }
inline void* __cdecl FakeColNew(unsigned n){ TRACE({"colmodel_new",n}); return aNewCol[aNewIdx++%4].data(); }
inline void __fastcall FakeSetColModel(void* mi,void*,void* cm,uint32 lod){ TRACE({"set_col_model",APtr(mi),APtr(cm),lod&0xFF}); }
inline void __fastcall FakeDeleteCol(void* mi,void*){ TRACE({"delete_collision_model",APtr(mi)}); }
inline void __cdecl FakeIncludeModel(int32 slot,int32 id){ TRACE({"include_model_index",slot,id}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFN=43;
inline const std::array<Fn,NFN> fns{{
    {"CAEPedlessSpeechAudioEntity","Constructor",0x4E6070,0,-1,128},{"CAEPedlessSpeechAudioEntity","Initialise",0x4E4E10,0,-1,128},
    {"CAEPedlessSpeechAudioEntity","StopCurrentSpeech",0x4E4E80,0,-1,256},{"CAEPedlessSpeechAudioEntity","AddSayEvent",0x4E60D0,0,-1,1024},
    {"CAEPedlessSpeechAudioEntity","UpdateParameters",0,0x85F330,0,512},{"CAEPedlessSpeechAudioEntity","Terminate",0,0x85F330,2,256},
    {"CAEPedlessSpeechAudioEntity","PlayLoadedSound",0,0x85F330,3,512},{"CAEPedlessSpeechAudioEntity","GetAllocatedVoice",0,0x85F330,4,64},
    {"CAEPedlessSpeechAudioEntity","WillPedChatAboutTopic",0,0x85F330,5,64},{"CAEPedlessSpeechAudioEntity","GetPedType",0,0x85F330,6,64},
    {"CAEPedlessSpeechAudioEntity","IsPedFemaleForAudio",0,0x85F330,7,64},
    {"CAEWMADecoder","Constructor",0x502720,0,-1,64},{"CAEWMADecoder","Destructor",0x502760,0,-1,128},
    {"CAEWMADecoder","FillBuffer",0,0x862D90,1,1024},{"CAEWMADecoder","GetSampleRate",0,0x862D90,5,128},
    {"CAEWMADecoder","GetStreamID",0,0x862D90,7,64},{"CAEWMADecoder","GetStreamLengthMs",0,0x862D90,2,128},
    {"CAEWMADecoder","GetStreamPlayTimeMs",0,0x862D90,3,512},{"CAEWMADecoder","InitLibrary",0x502B80,0,-1,256},
    {"CAEWMADecoder","Initialise",0,0x862D90,0,512},{"CAEWMADecoder","SelectStreamIndex",0x502990,0,-1,512},
    {"CAEWMADecoder","SetCursor",0,0x862D90,4,256},
    {"CClothes","ConstructPedModel",0x5A81E0,0,-1,256},{"CClothes","GetDefaultPlayerMotionGroup",0x5A81B0,0,-1,512},
    {"CClothes","GetDependentTexture",0x5A7F30,0,-1,256},{"CClothes","GetPlayerMotionGroupToLoad",0x5A7FB0,0,-1,512},
    {"CClothes","Init",0x5A80D0,0,-1,64},{"CClothes","LoadClothesFile",0x5A7B30,0,-1,256},
    {"CClothes","RebuildCutscenePlayer",0x5A8270,0,-1,256},{"CClothes","RebuildPlayer",0x5A82C0,0,-1,256},
    {"CClothes","RebuildPlayerIfNeeded",0x5A8390,0,-1,512},{"CClothes","RequestMotionGroupAnims",0x5A8120,0,-1,256},
    {"CColAccel","addCacheCol",0x5B2C20,0,-1,512},{"CColAccel","addColDef",0x5B2DD0,0,-1,256},
    {"CColAccel","addIPLEntity",0x5B3040,0,-1,1024},{"CColAccel","cacheIPLSection",0x5B2F10,0,-1,512},
    {"CColAccel","cacheLoadCol",0x5B2CC0,0,-1,256},{"CColAccel","endCache",0x5B2AD0,0,-1,256},
    {"CColAccel","getColDef",0x5B2E60,0,-1,256},{"CColAccel","getIplDef",0x5B2EF0,0,-1,256},
    {"CColAccel","isCacheLoading",0x5B2AC0,0,-1,128},{"CColAccel","setIplDef",0x5B2ED0,0,-1,256},
    {"CColAccel","startCache",0x5B31A0,0,-1,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dirPath=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dirPath/"audio11-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dirPath/"audio11-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("audio11");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; mainTid=GetCurrentThreadId();
    realLoadLibraryA=reinterpret_cast<decltype(&LoadLibraryA)>(ApiIn("kernel32.dll","LoadLibraryA")); realGetProcAddress=reinterpret_cast<decltype(&GetProcAddress)>(ApiIn("kernel32.dll","GetProcAddress"));
    realFreeLibrary=reinterpret_cast<decltype(&FreeLibrary)>(ApiIn("kernel32.dll","FreeLibrary")); realCoInitialize=reinterpret_cast<decltype(&CoInitialize)>(ApiIn("ole32.dll","CoInitialize"));
    if(!realLoadLibraryA||!realGetProcAddress||!realFreeLibrary||!realCoInitialize){Write({{"event","error"},{"reason","api not found"}});return;}
    std::string only; { std::ifstream f(dirPath/"audio11-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFN> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFN> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // vtables de la DLL del decodificador WMA (se construye y destruye uno sin stream ni lector)
    uint32 dllWmaVt{}, dllBaseVt{};
    { auto* d=new(wd.data()) CAEWMADecoder(nullptr); dllWmaVt=At<uint32>(wd.data(),0); d->~CAEWMADecoder(); dllBaseVt=At<uint32>(wd.data(),0); }
    auto vtName=[&](uint32 v)->json{ if(v==0x862D90||v==dllWmaVt)return "CAEWMADecoder"; if(v==0x85F418||v==dllBaseVt)return "CAEStreamingDecoder"; if(v==0x85F330)return "CAEPedlessSpeechAudioEntity"; return v; };
    // vtables COM falsas
    for(auto* vt:{&vtReader,&vtProfile,&vtStream,&vtProps,&vtHeader,&vtBuf})for(auto& s:*vt)s=reinterpret_cast<void*>(&CBad);
    for(auto* vt:{&vtReader,&vtProfile,&vtStream,&vtProps,&vtHeader,&vtBuf})(*vt)[2]=reinterpret_cast<void*>(&CRelease);
    vtReader[0]=reinterpret_cast<void*>(&RQi); vtReader[4]=reinterpret_cast<void*>(&RClose); vtReader[5]=reinterpret_cast<void*>(&RSetRange);
    vtReader[7]=reinterpret_cast<void*>(&RNext); vtReader[8]=reinterpret_cast<void*>(&RSelect); vtReader[10]=reinterpret_cast<void*>(&RReadSamples);
    vtReader[15]=reinterpret_cast<void*>(&RProps); vtReader[19]=reinterpret_cast<void*>(&ROutNum); vtReader[23]=reinterpret_cast<void*>(&ROpenStream);
    vtProfile[8]=reinterpret_cast<void*>(&PCount); vtProfile[9]=reinterpret_cast<void*>(&PGetStream);
    vtStream[3]=reinterpret_cast<void*>(&SType); vtStream[4]=reinterpret_cast<void*>(&SNum);
    vtProps[4]=reinterpret_cast<void*>(&MType);
    vtHeader[18]=reinterpret_cast<void*>(&HIndices); vtHeader[19]=reinterpret_cast<void*>(&HByIndex);
    vtBuf[7]=reinterpret_cast<void*>(&BGetBuffer);
    cReader={vtReader.data(),1}; cProfile={vtProfile.data(),2}; cProps={vtProps.data(),3}; cHeader={vtHeader.data(),4};
    for(auto& s:cStreams)s={vtStream.data(),5}; for(auto& b:cBufs)b={vtBuf.data(),6};
    // vtables falsas de la ropa
    for(auto& s:vtPlayer)s=reinterpret_cast<void*>(&FakeVBad); for(auto& s:vtMi)s=reinterpret_cast<void*>(&FakeVBad); for(auto& s:vtTask)s=reinterpret_cast<void*>(&FakeVBad);
    vtPlayer[8]=reinterpret_cast<void*>(&FakeDeleteRw); vtMi[8]=reinterpret_cast<void*>(&FakeDeleteRw); vtMi[16]=reinterpret_cast<void*>(&FakeSetClump);
    vtTask[6]=reinterpret_cast<void*>(&FakeMakeAbortable);

    // estado global guardado
    const std::vector<Saved> savedAll{
        Save(SLOTS,SLOTS_SIZE), Save(0xB613E0,0x20), Save(0xB6F028,0x48), Save(0xB6F118,4), Save(0x96A8B0,4), Save(0x8E4CB8,4), Save(0xB6B98C,4),
        Save(0xB6BAD4,8), Save(0xBC12F8,0xBC1CF0-0xBC12F8), Save(CA,CA_SIZE), Save(0xA9B0C8+4*MI_FAKE,4*8), Save(0xC8800C,4), Save(0xB4EA34,4) };
    auto restoreAll=[&]{ for(auto& s:savedAll)Restore(s); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            std::vector<std::unique_ptr<EntryPatch>> P;
            auto Add=[&](uintptr_t at,auto fake){ P.push_back(std::make_unique<EntryPatch>(at,fake)); };
            if(cls=="CAEPedlessSpeechAudioEntity") {
                Add(bit_cast<uintptr_t>(&CAESound::Initialise),FakeSndInit); Add(0x4EFE50,FakeSndInit);
                Add(bit_cast<uintptr_t>(&CAESound::SetPosition),FakeSndSetPos); Add(0x4EF880,FakeSndSetPos);
                Add(bit_cast<uintptr_t>(&CAESound::RegisterWithPhysicalEntity),FakeSndRegister); Add(0x4EF820,FakeSndRegister);
                Add(bit_cast<uintptr_t>(&CAESound::StopSoundAndForget),FakeSndStop); Add(0x4EF850,FakeSndStop);
                Add(bit_cast<uintptr_t>(&CAESoundManager::RequestNewSound),FakeReqNew); Add(0x4EFB10,FakeReqNew);
                Add(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUp); Add(0x571A00,FakeCleanUp);
                Add(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegRef); Add(0x571B70,FakeRegRef);
                Add(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::IsGlobalContextImportantForWidescreen),FakeWide); Add(0x4E46B0,FakeWide);
                Add(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::IsGlobalContextImportantForStreaming),FakeStreamCtx); Add(0x4E4510,FakeStreamCtx);
                Add(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::CanWePlayGlobalSpeechContext),FakeCanWePlay); Add(0x4E5F10,FakeCanWePlay);
                Add(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::GetSoundAndBankIDs),FakeGetIds); Add(0x4E5920,FakeGetIds);
                Add(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::SetNextPlayTime),FakeSetNext); Add(0x4E4A20,FakeSetNext);
                Add(bit_cast<uintptr_t>(&CAEPedSpeechAudioEntity::LoadAndPlaySpeech),FakeLoadPlay); Add(0x4E4840,FakeLoadPlay);
                Add(bit_cast<uintptr_t>(&CAEAudioUtility::ResolveProbability),FakeProb); Add(0x4D9C80,FakeProb);
                Add(bit_cast<uintptr_t>(&CStreaming::IsVeryBusy),FakeBusy); Add(0x4076A0,FakeBusy);
            } else if(cls=="CAEWMADecoder") {
                // CMemoryMgr::Malloc/Free, la IAT y operator new/delete se sustituyen solo durante la llamada (otros hilos los usan)
            } else if(cls=="CClothes") {
                Add(bit_cast<uintptr_t>(static_cast<float(*)(eStats)>(&CStats::GetStatValue)),FakeStat); Add(0x558E40,FakeStat);
                Add(bit_cast<uintptr_t>(&CStreaming::AddImageToList),FakeAddImage); Add(0x407610,FakeAddImage);
                Add(bit_cast<uintptr_t>(&CStreaming::RequestModel),FakeRequestModel); Add(0x4087E0,FakeRequestModel);
                Add(bit_cast<uintptr_t>(&CStreaming::SetModelIsDeletable),FakeSetDeletable); Add(0x409C10,FakeSetDeletable);
                Add(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),FakeLoadAll); Add(0x40EA10,FakeLoadAll);
                Add(bit_cast<uintptr_t>(&CTimer::Suspend),FakeSuspend); Add(0x5619D0,FakeSuspend);
                Add(bit_cast<uintptr_t>(&CTimer::Resume),FakeResume); Add(0x561A00,FakeResume);
                Add(bit_cast<uintptr_t>(&CClothesBuilder::CreateSkinnedClump),FakeSkinned); Add(0x5A69D0,FakeSkinned);
                Add(bit_cast<uintptr_t>(&CBaseModelInfo::AddTexDictionaryRef),FakeAddTexRef); Add(0x4C4B80,FakeAddTexRef);
                Add(bit_cast<uintptr_t>(&CBaseModelInfo::RemoveTexDictionaryRef),FakeRemoveTexRef); Add(0x4C4B90,FakeRemoveTexRef);
                Add(bit_cast<uintptr_t>(&RpAnimBlendClumpExtractAssociations),FakeExtract); Add(0x4D6BE0,FakeExtract);
                Add(bit_cast<uintptr_t>(&RpAnimBlendClumpGiveAssociations),FakeGive); Add(0x4D6C30,FakeGive);
                Add(bit_cast<uintptr_t>(&CWorld::Remove),FakeWorldRemove); Add(0x563280,FakeWorldRemove);
                Add(bit_cast<uintptr_t>(&CPed::Dress),FakeDress); Add(0x5E0130,FakeDress);
                Add(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile); Add(0x538900,FakeOpenFile);
                Add(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile); Add(0x5389D0,FakeCloseFile);
                Add(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),FakeLoadLine); Add(0x536F80,FakeLoadLine);
            } else {
                Add(bit_cast<uintptr_t>(&CFileMgr::OpenFileForWriting),FakeOpenForWriting); Add(0x538910,FakeOpenForWriting);
                Add(bit_cast<uintptr_t>(&CFileMgr::Write),FakeWrite); Add(0x538970,FakeWrite);
                Add(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile); Add(0x5389D0,FakeCloseFile);
                Add(bit_cast<uintptr_t>(static_cast<void*(*)(unsigned)>(&CColModel::operator new)),FakeColNew); Add(0x40FC30,FakeColNew);
                Add(bit_cast<uintptr_t>(&CBaseModelInfo::SetColModel),FakeSetColModel); Add(0x4C4BC0,FakeSetColModel);
                Add(bit_cast<uintptr_t>(&CBaseModelInfo::DeleteCollisionModel),FakeDeleteCol); Add(0x4C4C40,FakeDeleteCol);
                Add(bit_cast<uintptr_t>(&CColStore::IncludeModelIndex),FakeIncludeModel); Add(0x410820,FakeIncludeModel);
            }
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x11Au; if(!stt)stt=1; trace=json::array(); nrec=0;
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CAEPedlessSpeechAudioEntity") {
                        for(auto& b:ps)b=uint8(next());
                        At<uint32>(ps.data(),0)=0x85F330;
                        for(int k=0;k<6;++k){ for(auto& b:pents[k])b=uint8(next()); for(auto& b:pmats[k])b=uint8(next());
                            At<void*>(pents[k].data(),0x14)=next()%3 ? nullptr : static_cast<void*>(pmats[k].data());
                            const int32 ox=int32(next()%90)-45, oy=int32(next()%90)-45, oz=int32(next()%20)-10;
                            auto* pos=At<void*>(pents[k].data(),0x14) ? pmats[k].data()+0x30 : pents[k].data()+4;
                            At<float>(pos,0)=float(ox); At<float>(pos,4)=float(oy); At<float>(pos,8)=float(oz);
                            if(next()%5==0){ static const float exact[][3]{{36,48,0},{60,0,0},{0,0,60},{48,36,0},{36,48,0.5f},{0,60,0.001f},{59.99f,0,0}}; const auto& e=exact[next()%7];
                                At<float>(pos,0)=e[0]; At<float>(pos,4)=e[1]; At<float>(pos,8)=e[2]; } }
                        for(auto& s:psnds)for(auto& b:s)b=uint8(next());
                        for(auto& s:psnds)At<int32>(s.data(),0xC)=next()%4 ? 0x34 : int32(next()%0x60);
                        auto* slots=reinterpret_cast<uint8*>(SLOTS); for(size_t k=0;k<SLOTS_SIZE;++k)slots[k]=uint8(next());
                        for(int k=0;k<6;++k)At<int16>(slots,0x1C*k)=int16(next()%3 ? (next()%2 ? 5 : int(next()%6)) : int(next()%8));
                        pMask=next(); pSlotRet=next()%5 ? next()%5 : uint32(int32(next()%8)-1); pSoundRet=next()%5 ? next()%200 : uint32(-int32(next()%3)-1);
                        pSpecific=next(); pReqRet=next()%3 ? next()%4 : 4;
                        // cámara sin matriz: posición en el origen
                        At<void*>(reinterpret_cast<void*>(0xB6F028),0x14)=nullptr; At<float>(reinterpret_cast<void*>(0xB6F028),4)=0.f; At<float>(reinterpret_cast<void*>(0xB6F028),8)=0.f; At<float>(reinterpret_cast<void*>(0xB6F028),0xC)=0.f;
                        *reinterpret_cast<uint8*>(0xB6F065)=uint8(next()%4==0); *reinterpret_cast<uint8*>(0xB613E6)=uint8(next()%8==0); *reinterpret_cast<uint8*>(0xB613E5)=uint8(next()%2);
                        *reinterpret_cast<uint8*>(0x96A8B0)=uint8(next()%6==0 ? 2 : next()%4);
                        *reinterpret_cast<int32*>(0x8E4CB8)=next()%3 ? int32(next()%15) : int32(14+next()%4);
                        *reinterpret_cast<void**>(0xB6B98C)=next()%2 ? reinterpret_cast<void*>(uintptr_t(0x8C8000)) : nullptr;
                        auto ent=[&]()->void*{ return next()%5 ? static_cast<void*>(pents[next()%6].data()) : nullptr; };
                        ps[0x90]=uint8(next()%6 ? 1 : 0); ps[0x98]=uint8(next()%3==0); ps[0x99]=uint8(next()%4==0); ps[0x9A]=uint8(next()%8==0); ps[0x9B]=uint8(next()%2); ps[0x9C]=uint8(next()%2);
                        At<int16>(ps.data(),0xA8)=int16(next()%6); At<void*>(ps.data(),0x114)=ent(); At<int16>(ps.data(),0x92)=int16(int(next()%7)-1);
                        At<void*>(ps.data(),0xA0)=next()%4 ? static_cast<void*>(psnds[next()%4].data()) : nullptr;
                        auto* self=ps.data();
                        if(nm=="Constructor"){ ps.fill(0xCD); ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(self))-intptr_t(self); }
                        else if(nm=="Initialise")reinterpret_cast<void(__thiscall*)(void*)>(a)(self);
                        else if(nm=="StopCurrentSpeech"){ if(next()%2)ps[0x98]=1; reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="AddSayEvent"){ ps[0x98]=uint8(next()%4==0); ps[0x99]=uint8(next()%5==0); ps[0x9A]=uint8(next()%10==0);
                            static const int32 ctxs[]{0xAD,0xAE,0xAF,0xAC,0xB0,0x100AD,int32(0xFFFF00AEu),0,-1,0x7FFF};
                            const int32 g=next()%4 ? int32(0xAD+next()%3) : ctxs[next()%10];
                            const int32 ev=next()%6 ? 0x34 : int32(next()%0x60); void* at=next()%8 ? static_cast<void*>(pents[next()%6].data()) : nullptr;
                            ret=int32(reinterpret_cast<int16(__thiscall*)(void*,int32,int32,void*,uint32,float,uint32,uint32,uint32)>(a)(self,ev,g,at,next()%3000,uni(0.f,1.f),next()%2,next()%3==0,next()%3==0)); }
                        else if(nm=="UpdateParameters"){ static const int16 times[]{-1,-1,0,1,100,-2,32767};
                            const int16 t=next()%2 ? times[next()%7] : int16(next());
                            reinterpret_cast<void(__thiscall*)(void*,void*,int32)>(a)(self,psnds[next()%4].data(),t); }
                        else if(nm=="Terminate"){ if(next()%2)ps[0x98]=1; reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="PlayLoadedSound"){ At<int16>(self,0xA8)=int16(next()%5); At<int16>(self,0xA4)=int16(next()); if(next()%2)ps[0x98]=1;
                            reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="WillPedChatAboutTopic")ret=reinterpret_cast<uint8(__thiscall*)(void*,int32)>(a)(self,int32(next()%40));
                        else if(nm=="IsPedFemaleForAudio")ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(self);
                        else ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(self)&0xFFFF; // GetAllocatedVoice / GetPedType: solo AX
                        out["vptr"]=vtName(At<uint32>(self,0)); At<uint32>(self,0)=0;
                        out["obj"]=ReversingProbe::Bytes(self,ps.size()); out["slots"]=ReversingProbe::Bytes(slots,SLOTS_SIZE); out["player_speaking"]=*reinterpret_cast<uint8*>(0xB613E5);
                    }
                    else if(cls=="CAEWMADecoder") {
                        auto* self=wd.data();
                        const bool big=nm=="FillBuffer"||nm=="Constructor";
                        for(size_t k=0;k<0x20;++k)self[k]=uint8(next()); for(size_t k=0x20020;k<WMA_SIZE;++k)self[k]=uint8(next());
                        if(big){ uint32 x=next()|1; for(size_t k=0x20;k<0x20020;k+=4){ x^=x<<13;x^=x>>17;x^=x<<5; At<uint32>(self,k)=x; } }
                        At<uint32>(self,0)=0x862D90; At<void*>(self,4)=wStream.data(); self[8]=uint8(next()%4 ? 1 : 0);
                        At<void*>(self,0x20030)=&cReader; At<uint16>(self,0x20024)=uint16(next()%4 ? 1+next()%3 : next());
                        for(auto& b:wStream)b=uint8(next());
                        wMask=next(); wTime=(uint64(next())<<(next()%32))|next(); wDur=uint64(next()%400000);
                        if(nm=="Constructor"){ for(size_t k=0;k<0x20;++k)self[k]=0xCD; for(size_t k=0x20020;k<WMA_SIZE;++k)self[k]=0xCD;
                            ret=intptr_t(reinterpret_cast<uint8*(__thiscall*)(void*,void*)>(a)(self,nullptr))-intptr_t(self); }
                        else if(nm=="Destructor"){ At<void*>(self,4)=nullptr; ResetArena();
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__thiscall*)(void*)>(a)(self); }
                            out["recs"]=Recs(CName); }
                        else if(nm=="FillBuffer"){
                            static const uint32 sizes[]{0,1,3,4,64,1000,4096,0x10000};
                            const uint32 n=next()%3 ? next()%5000 : sizes[next()%8];
                            uint32& used=At<uint32>(self,0x20020);
                            switch(next()%6){ case 0: case 1: used=0; break; case 2: used=n ? next()%n : 0; break; case 3: used=n; break; case 4: used=n+next()%3000; break; default: used=next()%0x20001; }
                            wNumSamples=next()%7; wSampleIdx=0;
                            for(uint32 k=0;k<8;++k){ auto& s=wSamples[k]; s.hr=next()%10==0 ? E_FAIL : next()%8==0 ? S_FALSE : S_OK; s.bhr=next()%10==0 ? E_FAIL : S_OK;
                                s.stream=uint16(next()%4 ? At<uint16>(self,0x20024) : next()%5); s.len=next()%16==0 ? 0x1F000+next()%0x3000 : next()%3000; }
                            for(int k=0;k<3;++k){ uint32 x=next()|1; for(size_t j=0;j<0x2000;j+=4){ x^=x<<13;x^=x>>17;x^=x<<5; At<uint32>(sampleData[k].data(),j)=x; } }
                            std::memset(wDest.data(),0xAB,std::min<size_t>(n+64,wDest.size()));
                            ret=uint32(reinterpret_cast<size_t(__thiscall*)(void*,void*,size_t)>(a)(self,wDest.data(),n));
                            out["dest"]=Hash64(wDest.data(),std::min<size_t>(n+64,wDest.size())); out["temp"]=Hash64(self+0x20,0x20000); }
                        else if(nm=="GetStreamPlayTimeMs"){ uint64 t,d; switch(next()%6){ case 0: t=next()%100000000; d=next()%100000; break; case 1: t=(uint64(next())<<32)|next(); d=next(); break;
                                case 2: t=0x8000000000000000ull|(uint64(next())<<20); d=next(); break; case 3: t=0xFFFFFFFFFFFFFFFFull-next()%1000; d=next()%2000; break;
                                case 4: t=(1ull<<53)+next(); d=next()%7; break; default: t=uint64(next())*10000+next()%2*5000; d=0; }
                            At<uint64>(self,0x10)=t; At<uint64>(self,0x18)=d; ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="SetCursor"){ static const uint32 ps2[]{0,1,429496,429497,0xFFFFFFFF,0x80000000,1000000};
                            reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(self,next()%2 ? next()%600000 : next()%3 ? ps2[next()%7] : next()); }
                        else if(nm=="GetStreamID"){ At<int32>(wStream.data(),0x1C)=int32(next()); ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(self); }
                        else if(nm=="InitLibrary"){
                            static const uintptr_t mods[]{FAKE_MODULE,0,0xFFFFFFFF};
                            *reinterpret_cast<void**>(0xB6BAD8)=next()%6==0 ? reinterpret_cast<void*>(&FakeCreateReader) : nullptr;
                            *reinterpret_cast<uintptr_t*>(0xB6BAD4)=next()%2 ? 0 : 0x1234;
                            wLoadRet=reinterpret_cast<HMODULE>(next()%4 ? mods[0] : mods[1+next()%2]);
                            wProcRet=next()%5 ? std::bit_cast<uintptr_t>(&FakeCreateReader) : 0; wCreateHr=next()%4 ? S_OK : next()%2 ? E_FAIL : S_FALSE;
                            { IatPatch i1({"kernel32.dll","kernelbase.dll"},"LoadLibraryA",FakeLoadLibraryA), i2({"kernel32.dll","kernelbase.dll"},"GetProcAddress",FakeGetProcAddress),
                                       i3({"kernel32.dll","kernelbase.dll"},"FreeLibrary",FakeFreeLibrary);
                              ret=reinterpret_cast<uint8(__cdecl*)()>(a)(); }
                            const auto fp=*reinterpret_cast<uintptr_t*>(0xB6BAD8);
                            out["create_reader"]=fp==std::bit_cast<uintptr_t>(&FakeCreateReader) ? json("fake") : json(fp); out["module"]=*reinterpret_cast<uint32*>(0xB6BAD4); }
                        else if(nm=="Initialise"||nm=="SelectStreamIndex"){
                            wStreamCount=next()%4 ? 1+next()%5 : next()%7; wCountHr=next()%10==0 ? E_FAIL : S_OK;
                            for(int k=0;k<6;++k){ wStreamNum[k]=next()%6 ? 1+next()%4 : 0; wStreamAudio[k]=next()%3==0; wGetStreamHr[k]=next()%12==0 ? E_FAIL : S_OK;
                                wNumHr[k]=next()%12==0 ? E_FAIL : S_OK; wTypeHr[k]=next()%12==0 ? E_FAIL : next()%8==0 ? S_FALSE : S_OK; }
                            wCreateHr=next()%10==0 ? E_FAIL : S_OK; wOpenHr=next()%10==0 ? E_FAIL : S_OK; wQiProfileHr=next()%10==0 ? E_NOINTERFACE : S_OK;
                            wQiHeaderHr=next()%8==0 ? E_NOINTERFACE : S_OK; wPropsHr=next()%8==0 ? E_FAIL : S_OK;
                            wMediaSize=0x48+next()%0x30; wAttrCount=1+next()%3; wOutputNum=next()%4;
                            static const uint64 durs[]{70000000ull,69990000ull,69999999ull,0ull,0xFFFFFFFFFFFFFFFFull,1234567890ull};
                            wDuration=next()%2 ? durs[next()%6] : (uint64(next())<<(next()%20))|next();
                            for(auto& b:waveFmt)b=uint8(next()); At<uint16>(waveFmt.data(),2)=uint16(next()%4 ? 2 : next()%3); At<uint16>(waveFmt.data(),0xE)=uint16(next()%4 ? 16 : 8);
                            for(auto& b:mediaBuf)b=uint8(next());
                            *reinterpret_cast<void**>(0xB6BAD8)=next()%10 ? reinterpret_cast<void*>(&FakeCreateReader) : nullptr;
                            ResetArena();
                            if(nm=="Initialise"){ IatPatch c1({"ole32.dll","combase.dll"},"CoInitialize",FakeCoInitialize); /* antes de sustituir new/delete: su vector usa el montón real */
                                EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete);
                                EntryPatch m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc),m1e(0x72F420,FakeMalloc),m2(bit_cast<uintptr_t>(static_cast<void(*)(void*)>(&CMemoryMgr::Free)),FakeFree),m2e(0x72F430,FakeFree);
                                ret=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(self); }
                            else { void* prof=next()%10 ? static_cast<void*>(&cProfile) : nullptr; ret=int32(reinterpret_cast<HRESULT(__thiscall*)(void*,void*)>(a)(self,prof)); }
                            out["recs"]=Recs(CName); out["arena"]=Hash64(arena.data(),0x100); out["reader"]=CName(At<void*>(self,0x20030)); At<uint32>(self,0x20030)=0; }
                        else ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(self); // GetSampleRate / GetStreamLengthMs
                        out["vptr"]=vtName(At<uint32>(self,0)); At<uint32>(self,0)=0;
                        if(At<void*>(self,4)==wStream.data())At<uint32>(self,4)=0x5EAA;
                        if(At<void*>(self,0x20030)==&cReader)At<uint32>(self,0x20030)=0x5EAB;
                        out["head"]=ReversingProbe::Bytes(self,0x20); out["tail"]=ReversingProbe::Bytes(self+0x20020,0x18);
                        *reinterpret_cast<uint64*>(0xB6BAD4)=*reinterpret_cast<const uint64*>(savedAll[7].bytes.data());
                    }
                    else if(cls=="CClothes") {
                        static const uint32 vals[]{0x43FA0000u,0x43FA0001u,0x43F9FFFFu,0u,0x7FC00000u,0x447A0000u,0x80000000u,0xFFC00000u};
                        statFat=next()%3 ? uni(0.f,1000.f) : std::bit_cast<float>(vals[next()%8]); statMus=next()%3 ? uni(0.f,1000.f) : std::bit_cast<float>(vals[next()%8]);
                        if(next()%6==0)statMus=statFat;
                        cMask=next();
                        auto* rules=reinterpret_cast<uint8*>(0xBC12F8); const size_t rulesSize=0xBC1CF0-0xBC12F8;
                        for(size_t k=0;k<rulesSize;++k)rules[k]=uint8(next());
                        At<uint32>(rules,4)=next()%300;
                        // jugador falso
                        for(auto& b:cPlayer)b=uint8(next()); for(auto& b:cIntel)b=uint8(next()); for(auto& b:cPlayerData)b=uint8(next()); for(auto& b:cDesc)b=uint8(next());
                        for(auto& c:cClumps){ for(auto& b:c)b=uint8(next()); c[0]=2; }
                        At<void*>(cPlayer.data(),0)=vtPlayer.data(); At<void*>(cPlayer.data(),0x18)=cClumps[0].data(); At<int16>(cPlayer.data(),0x22)=int16(MI_FAKE);
                        At<void*>(cPlayer.data(),0x47C)=cIntel.data(); At<void*>(cPlayer.data(),0x480)=cPlayerData.data(); At<void*>(cPlayerData.data(),4)=cDesc.data();
                        At<void*>(cIntel.data(),4+0x14+5*4)=next()%3 ? static_cast<void*>(cTask.data()) : nullptr; At<void*>(cTask.data(),0)=vtTask.data();
                        if(next()%4==0){ At<float>(cDesc.data(),0x70)=statFat; } if(next()%4==0){ At<float>(cDesc.data(),0x74)=statMus; }
                        if(nm=="RebuildPlayerIfNeeded"&&next()%2){ At<float>(cDesc.data(),0x70)=statFat; At<float>(cDesc.data(),0x74)=statMus; if(next()%3==0)At<float>(cDesc.data(),0x70+4*(next()%2))=uni(0.f,1000.f); }
                        // info de modelo y pool de TXD falsos
                        for(auto& b:cMi)b=uint8(next()); At<void*>(cMi.data(),0)=vtMi.data(); At<int16>(cMi.data(),0xA)=int16(next()%8); At<void*>(cMi.data(),0x1C)=cClumps[1].data();
                        for(auto& b:cPoolObjs)b=uint8(next()); cPoolFlags.fill(0); for(int k=0;k<8;++k)cPoolFlags[k]=uint8(next()%0x7F);
                        At<void*>(cPool.data(),0)=cPoolObjs.data(); At<void*>(cPool.data(),4)=cPoolFlags.data(); At<int32>(cPool.data(),8)=8; At<int32>(cPool.data(),0xC)=-1; cPool[0x10]=0;
                        CModelInfo::ms_modelInfoPtrs[MI_FAKE]=reinterpret_cast<CBaseModelInfo*>(cMi.data());
                        *reinterpret_cast<void**>(0xC8800C)=cPool.data();
                        // grupos de animación falsos
                        for(auto& b:cGroups)b=uint8(next()); for(auto& bl:cBlocks)for(auto& b:bl)b=uint8(next());
                        for(int g=0;g<0x40;++g)At<void*>(cGroups.data(),g*0x14)=next()%4 ? static_cast<void*>(cBlocks[next()%2].data()) : nullptr;
                        cBlocks[0][0x10]=uint8(next()%2); cBlocks[1][0x10]=uint8(next()%3 ? next()%2 : next());
                        *reinterpret_cast<void**>(0xB4EA34)=cGroups.data();
                        auto* pl=cPlayer.data();
                        if(nm=="ConstructPedModel"){ const void* old=next()%3 ? reinterpret_cast<void*>(uintptr_t(0xBC1C78)) : nullptr;
                            reinterpret_cast<void(__cdecl*)(uint32,void*,const void*,uint32)>(a)(MI_FAKE,cDesc.data(),old,next()%2); }
                        else if(nm=="GetDefaultPlayerMotionGroup"||nm=="GetPlayerMotionGroupToLoad")ret=reinterpret_cast<int32(__cdecl*)()>(a)();
                        else if(nm=="GetDependentTexture"){ const int32 v=next()%3 ? int32(next()%12)-1 : int32(next()); ret=reinterpret_cast<int32(__cdecl*)(int32)>(a)(v); }
                        else if(nm=="Init"||nm=="LoadClothesFile"){
                            lines.clear(); lineIdx=0;
                            static const char* names[]{"gimpleg","policetr","torso","head","hands","legs","feet","necklace","watch","glasses","hat","extra1","pimp","medic","shoe","-","gimpmask","vest"};
                            static const char* parts[]{"torso","head","hands","legs","feet","necklace","watch","glasses","hat","extra1","bogus"};
                            static const char* tags[]{"cuts","setc","tex","hide","ignore","endignore","exclusive","endexclusive"};
                            auto caseMix=[&](std::string s){ for(auto& c:s)if(next()%2)c=char(toupper(c)); return s; };
                            auto sep=[&]{ static const char* s[]{" ","\t",","," \t","\t\t"}; return std::string(s[next()%5]); };
                            const int nl=int(next()%30);
                            // Se simula el estado de bloque del EXE: "rule..." solo fuera de un bloque (dentro sería una etiqueta
                            // desconocida, que en el EXE reutiliza un valor sin inicializar y puede leer argumentos nulos).
                            bool inBlock=false;
                            for(int l=0;l<nl;++l){ const uint32 r=next()%12; std::string ln;
                                if(r==0)ln="# comment "+std::to_string(next()%100); else if(r==1)ln="";
                                else if(r==2){ const bool alt=next()%2; ln=inBlock ? "# no rule" : alt ? "rule" : "rules x"; }
                                else if(r==3)ln=next()%2 ? "end" : "endif";
                                else { const int t=int(next()%8); ln=caseMix(tags[t])+sep();
                                    if(t==1){ ln+=names[next()%18]+sep()+parts[next()%11]+sep()+names[next()%18]+sep()+names[next()%18]; }
                                    else if(t==3)ln+=std::string(names[next()%18])+sep()+parts[next()%11];
                                    else ln+=std::string(names[next()%18])+sep()+names[next()%18];
                                    if(next()%4==0)ln=sep()+ln; }
                                if(!ln.empty()&&ln[0]!='#'){ if(!inBlock){ if(!strncmp("rule",ln.c_str(),4))inBlock=true; } else if(!strncmp("end",ln.c_str(),3))inBlock=false; }
                                lines.push_back(ln); }
                            if(nm=="Init"){ auto* pc=reinterpret_cast<uint8*>(0xBC1C78); for(size_t k=0;k<0x78;++k)pc[k]=uint8(next()); }
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="RebuildCutscenePlayer")reinterpret_cast<void(__cdecl*)(void*,int32)>(a)(pl,MI_FAKE);
                        else if(nm=="RebuildPlayer")reinterpret_cast<void(__cdecl*)(void*,uint32)>(a)(pl,next()%2);
                        else if(nm=="RebuildPlayerIfNeeded")reinterpret_cast<void(__cdecl*)(void*)>(a)(pl);
                        else reinterpret_cast<void(__cdecl*)()>(a)(); // RequestMotionGroupAnims
                        out["desc"]=ReversingProbe::Bytes(cDesc.data(),cDesc.size()); out["rules"]=Hash64(rules,0xBC1C60-0xBC12F8); out["rule_head"]=ReversingProbe::Bytes(rules,0x40);
                        out["player_clothes"]=ReversingProbe::Bytes(reinterpret_cast<void*>(0xBC1C78),0x78); out["player"]=Hash64(cPlayer.data(),cPlayer.size()); out["mi"]=Hash64(cMi.data(),cMi.size());
                        CModelInfo::ms_modelInfoPtrs[MI_FAKE]=*reinterpret_cast<CBaseModelInfo* const*>(savedAll[10].bytes.data());
                        Restore(savedAll[11]); Restore(savedAll[12]); Restore(savedAll[8]);
                    }
                    else { // CColAccel
                        auto* g=reinterpret_cast<uint8*>(CA);
                        for(auto& b:aCol)b=uint8(next()); for(auto& b:aIpl)b=uint8(next()); for(auto& v:aSect)v=int32(next()%12); for(auto& b:aDefs)b=uint8(next());
                        for(auto& b:aBounds)b=uint8(next()); for(auto& b:aColModel)b=uint8(next()); for(auto& c:aNewCol)for(auto& b:c)b=uint8(next()); aNewIdx=0;
                        for(auto& c:aColModels)for(auto& b:c)b=uint8(next());
                        static const int32 states[]{0,1,2,3,-1};
                        auto keep=[&]{ return !(nm=="endCache"&&next()%8==0); }; // solo endCache admite arrays nulos
                        At<void*>(g,0)=keep() ? static_cast<void*>(aBounds.data()) : nullptr; At<void*>(g,4)=keep() ? static_cast<void*>(aDefs.data()) : nullptr;
                        At<void*>(g,8)=keep() ? static_cast<void*>(aSect.data()) : nullptr; At<int32>(g,0xC)=int32(next());
                        At<int32>(g,0x10)=next()%4 ? int32(next()%3) : states[next()%5];
                        At<void*>(g,0x14)=keep() ? static_cast<void*>(aCol.data()) : nullptr; At<int32>(g,0x18)=int32(next()%12);
                        At<void*>(g,0x1C)=keep() ? static_cast<void*>(aIpl.data()) : nullptr; At<int32>(g,0x20)=int32(next()%24);
                        At<int32>(g,0x24)=int32(next()%10); At<int32>(g,0x28)=int32(next()%12);
                        // infos de modelo falsas en 19990..19997
                        for(int k=0;k<8;++k){ for(auto& b:aMi[k])b=uint8(next()); At<void*>(aMi[k].data(),0x14)=next()%4 ? static_cast<void*>(aColModels[next()%3].data()) : nullptr;
                            At<float>(aMi[k].data(),0x18)=next()%3 ? uni(0.f,600.f) : float(next()%2 ? 300 : 150);
                            CModelInfo::ms_modelInfoPtrs[MI_FAKE+k]=reinterpret_cast<CBaseModelInfo*>(aMi[k].data()); }
                        for(int k=0;k<12;++k){ for(auto& b:aEnts[k])b=uint8(next()); aEntPtrs[k]=aEnts[k].data(); At<int16>(aEnts[k].data(),0x22)=int16(MI_FAKE+next()%8); }
                        for(int k=0;k<12;++k){ At<void*>(aEnts[k].data(),0x30)=aEnts[next()%12].data(); aEnts[k][0x34]=uint8(next()%3 ? next()%3 : next()); }
                        *reinterpret_cast<float*>(0xB6F118)=next()%3 ? 1.f : uni(0.5f,3.f);
                        if(nm=="isCacheLoading")ret=reinterpret_cast<uint8(__cdecl*)()>(a)();
                        else if(nm=="startCache"){ ResetArena(); newNullMask=next()%4==0 ? next() : 0;
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__cdecl*)()>(a)(); }
                            out["recs"]=Recs(APtr); out["arena"]=Hash64(arena.data(),arena.size()); newNullMask=0; }
                        else if(nm=="endCache"){ ResetArena();
                            { EntryPatch n1(0x82119A,FakeNew),n2(0x821195,FakeNew),d1(0x8214BD,FakeDelete),d2(0x8213AE,FakeDelete); reinterpret_cast<void(__cdecl*)()>(a)(); }
                            out["recs"]=Recs(APtr); }
                        else if(nm=="addCacheCol"){ At<int32>(g,0x18)=int32(next()%15); const int32 se=next()%2 ? int32(next()%20000) : int32(next());
                            reinterpret_cast<void(__cdecl*)(int32,void*)>(a)(se,aColModel.data()); }
                        else if(nm=="cacheLoadCol"){ At<int32>(g,0x18)=next()%6==0 ? -int32(next()%3) : int32(next()%5);
                            for(int k=0;k<16;++k){ At<int16>(aCol.data(),k*0x30+0x28)=int16(MI_FAKE+next()%8); At<int16>(aCol.data(),k*0x30+0x2A)=0; }
                            reinterpret_cast<void(__cdecl*)()>(a)(); }
                        else if(nm=="addColDef"){ At<int32>(g,0x28)=int32(next()%15); alignas(16) std::array<uint8,0x2C> cd{}; for(auto& b:cd)b=uint8(next());
                            reinterpret_cast<void(__cdecl*)(std::array<uint8,0x2C>)>(a)(cd); }
                        else if(nm=="getColDef"){ At<int32>(g,0x28)=int32(next()%15); alignas(16) std::array<uint8,0x2C> cd{}; for(auto& b:cd)b=uint8(next());
                            reinterpret_cast<void(__cdecl*)(void*)>(a)(cd.data()); out["coldef"]=ReversingProbe::Bytes(cd.data(),cd.size()); }
                        else if(nm=="setIplDef"){ alignas(16) std::array<uint8,0x34> d{}; for(auto& b:d)b=uint8(next());
                            reinterpret_cast<void(__cdecl*)(int32,std::array<uint8,0x34>)>(a)(int32(next()%8),d); }
                        else if(nm=="getIplDef"){ alignas(16) std::array<uint8,0x40> d{}; for(auto& b:d)b=uint8(next());
                            void* r=reinterpret_cast<void*(__cdecl*)(void*,int32)>(a)(d.data(),int32(next()%8)); ret=r==d.data(); out["ipldef"]=ReversingProbe::Bytes(d.data(),d.size()); }
                        else if(nm=="cacheIPLSection"){ At<int32>(g,0x24)=int32(next()%8); At<int32>(g,0x20)=int32(next()%12);
                            for(int k=0;k<32;++k){ uint8* e=aIpl.data()+k*0x14; At<int32>(e,0)=int32(next()%12); At<int32>(e,4)=int32(next()%12);
                                At<int32>(e,8)=MI_FAKE+int32(next()%8); At<int32>(e,0xC)=MI_FAKE+int32(next()%8); At<uint32>(e,0x10)=next()%4 ? next()%16 : next(); }
                            for(auto& v:aSect)v=int32(next()%20);
                            reinterpret_cast<void(__cdecl*)(void*,int32)>(a)(aEntPtrs.data(),12); }
                        else { At<int32>(g,0x20)=int32(next()%31); const int32 cnt=int32(next()%13); const int32 idx=int32(next()%12);
                            if(next()%3==0)At<void*>(aEnts[idx].data(),0x30)=aEnts[(idx+1+next()%11)%12].data();
                            reinterpret_cast<void(__cdecl*)(void*,int32,int32)>(a)(aEntPtrs.data(),cnt,idx); }
                        json gl=json::array(); for(size_t off=0;off<CA_SIZE;off+=4){ const uint32 v=At<uint32>(g,off); const bool ptr=off==0||off==4||off==8||off==0x14||off==0x1C; gl.push_back(ptr ? APtr(reinterpret_cast<void*>(uintptr_t(v))) : json(v)); }
                        out["globals"]=gl; out["col"]=ReversingProbe::Bytes(aCol.data(),aCol.size()); out["ipl"]=ReversingProbe::Bytes(aIpl.data(),aIpl.size());
                        out["sect"]=aSect; out["defs"]=ReversingProbe::Bytes(aDefs.data(),8*0x34); out["defs_hash"]=Hash64(aDefs.data(),aDefs.size()); out["bounds"]=ReversingProbe::Bytes(aBounds.data(),aBounds.size());
                        out["newcol"]=Hash64(aNewCol.data(),sizeof(aNewCol)); json ents=json::array(); for(auto& e:aEnts)ents.push_back(At<uint32>(e.data(),0x1C)); out["ent_flags"]=ents;
                        json mis=json::array(); for(auto& m:aMi)mis.push_back({m[0x12],At<uint32>(m.data(),0x18),APtr(At<void*>(m.data(),0x14))}); out["mi"]=mis;
                        Restore(savedAll[9]); Restore(savedAll[10]); Restore(savedAll[3]);
                    }
                    restoreAll();
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                restoreAll();
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
    bool restored=true; for(auto& s:savedAll)restored&=Same(s);
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
