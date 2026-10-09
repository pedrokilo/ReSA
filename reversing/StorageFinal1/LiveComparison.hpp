#pragma once
// Banco dentro del motor (flujo viejo, estilo Small2/PadMouse1/VehCtor62): CGenericGameStorage (5 funciones pendientes).
//   SaveDataToWorkBuffer-org (0x5D1270), LoadDataFromWorkBuffer-org (0x5D1300), ReportError (0x5D08C0), GenericSave (0x5D13E0), GenericLoad (0x5D17B0).
// Original (hook de la funcion desactivado: codigo del EXE) frente a candidato (hook activo) con fixtures deterministas por caso y la traza de
// todas las llamadas salientes. SIN ENTRADA/SALIDA REAL: no se abre ningun fichero ni se toca ningun guardado.
//  * Bufer de trabajo: bufer propio de 51201 bytes (SaveDataToWorkBuffer/LoadDataFromWorkBuffer) o reservado por el doble de OpenFile* con el operator
//    new[] del juego (GenericSave/GenericLoad: las funciones lo liberan con delete[] igual que el original).
//  * Simulados y trazados (en la direccion del EXE y en el cuerpo de la DLL): SaveWorkBuffer (0x5D0F80) y LoadWorkBuffer (0x5D10B0) con vaciado/recarga
//    programados (acierto, fallo con y sin ms_bFailed, y "fichero virtual" para GenericLoad), OpenFileForWriting (0x5D0DD0) y OpenFileForReading
//    (0x5D0D20), CFileMgr::CloseFile (0x5389D0), CPad::UpdatePads (0x541E50), CCheat::ResetCheats (0x438450), CSimpleVariablesSaveStructure::Construct
//    (0x5D1B80) y Extract (0x5D1EA0), ReportError (en GenericLoad), DoGameSpecificStuffAfterSucessLoad (0x618E90) y los 24 + 24 guardados/cargas de
//    bloques (CTheScripts::Save ... C3dMarkers::LoadUser3dMarkers) mas DoScriptSetupAfterPoolsHaveLoaded (0x5D3390). Ninguna carga real se ejecuta:
//    si un cuerpo de la DLL no se localiza, el banco aborta con un evento "error" antes de parchear nada.
//  * ReportError: el EXE formatea con sprintf (0x821BB5, que se sustituye) en un bufer local; la DLL emite el mismo texto por spdlog (se captura con un
//    sumidero temporal). Los bloques 25, 26 y >= 28 usan un bufer sin inicializar en el EXE y quedan fuera.
//  * Se mantiene ms_WorkBufferSize == 0xC800 en GenericLoad (con un tamano menor el EXE lee mas alla de la cabecera de 5 bytes: dominio insano). El caso
//    de ms_WorkBufferSize != 0xC800 (umbral variable de Load frente al 0xC800 fijo del resto) se cubre en LoadDataFromWorkBuffer-org con fixtures cuerdos.
//  * Rama NOTSA de GenericLoad (version del guardado distinta): el doble de Extract devuelve la version actual, asi que no se entra en ella.
#include <fstream>
#include <set>
#include <cstdarg>
#include <mutex>
#include <spdlog/sinks/base_sink.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include <reversiblehooks/ReversibleHook/Simple.h>
#include <reversiblehooks/ReversibleHook/Virtual.h>
#include "GenericGameStorage.h"
#include "Cheat.h"
#include "Pad.h"
#include "FileMgr.h"
#include "SimpleVariablesSaveStructure.h"
#include "TheCarGenerators.h"
#include "PedType.h"
#include "C_PcSave.h"
#include "TagManager.h"
#include "Radar.h"
#include "StuntJumpManager.h"
#include "EntryExitManager.h"
#include "TheScripts.h"
#include "Garages.h"
#include "../ProbeRuntime.hpp"

namespace StorageFinal1Live {
using S = CGenericGameStorage;
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
template<class T=uint32> T& At(void* base,size_t off) { return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T=uint32> T& AtA(uintptr_t a) { return *reinterpret_cast<T*>(a); }
template<class Fp> uintptr_t Addr(Fp f) { return std::bit_cast<uintptr_t>(f); }
struct Rng { uint32 s; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    bool pct(uint32 p){ return next()%100<p; }
    uint32 below(uint32 n){ return next()%n; } };

// Hilo del banco: otros hilos del juego pueden llamar a funciones parcheadas; sus llamadas no se trazan.
inline DWORD owner{};
inline bool Mine(){ return GetCurrentThreadId()==owner; }

// --- trazas ---
template<class V> inline json Jv(V v){
    if constexpr(std::is_floating_point_v<V>) return std::bit_cast<uint32>(float(v));
    else if constexpr(std::is_pointer_v<V>) return uintptr_t(v);
    else return v;
}
template<class... A> inline void T(const char* tag,A... a){ json e=json::array(); e.push_back(tag); (e.push_back(Jv(a)),...); trace.push_back(std::move(e)); }

// --- parches de entrada sin duplicados (se deshacen en orden inverso) ---
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> v; std::set<uintptr_t> seen;
    template<class Fp> void Add(uintptr_t at,Fp f){ if(at&&seen.insert(at).second)v.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,f)); }
    ~Patches(){ while(!v.empty())v.pop_back(); }
};

// --- regiones globales guardadas ---
struct Region { void* p; size_t n; std::vector<uint8> saved; };
inline std::vector<Region> regions;
inline void Snap(uintptr_t p,size_t n){ regions.push_back({reinterpret_cast<void*>(p),n,ReversingProbe::Bytes(reinterpret_cast<void*>(p),n)}); }
inline void RestoreRegions(){ for(auto& g:regions)std::memcpy(g.p,g.saved.data(),g.n); }
inline bool RegionsRestored(){ for(auto& g:regions)if(std::memcmp(g.p,g.saved.data(),g.n))return false; return true; }

// --- modulo de la DLL y busqueda de cuerpos ---
inline uintptr_t modBase{}, modEnd{};
inline void InitModule(){ HMODULE mod{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&InitModule),&mod);
    modBase=uintptr_t(mod); const auto* nt=reinterpret_cast<const IMAGE_NT_HEADERS*>(modBase+reinterpret_cast<const IMAGE_DOS_HEADER*>(modBase)->e_lfanew); modEnd=modBase+nt->OptionalHeader.SizeOfImage; }
inline bool InDll(uintptr_t a){ return a>=modBase&&a<modEnd; }
inline uintptr_t FollowIn(uintptr_t q){ for(int k=0;k<8&&InDll(q)&&*reinterpret_cast<const uint8*>(q)==0xE9;++k){ const uintptr_t t=q+5+*reinterpret_cast<const int32*>(q+1); if(!InDll(t))break; q=t; } return q; }
template<class Fp> inline uintptr_t Body(Fp f){ return FollowIn(Addr(f)); }
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}

// ======================= estado y buferes =======================
constexpr uint32 kBuf=0xC800, kSize=0x317FC;               // 51200 y SIZE_OF_ONE_GAME_IN_BYTES
constexpr size_t kGuard=32, kCaller=60000;
alignas(16) inline std::array<uint8,kGuard+kBuf+1+kGuard> workRaw{};
alignas(16) inline std::array<uint8,kGuard+kCaller+kGuard> callerRaw{};
inline uint8* WorkP(){ return workRaw.data()+kGuard; }
inline uint8* CallerP(){ return callerRaw.data()+kGuard; }
alignas(16) inline std::array<uint8,65536> pat{};          // datos de los bloques
alignas(16) inline std::array<uint8,32> opaque{};          // "FILE*" falso, nadie lo desreferencia
inline FILE* Handle(){ return reinterpret_cast<FILE*>(opaque.data()+16); }
inline uint8* NewWork(){ auto* p=new uint8[kBuf+1]; std::memset(p,0xCD,kBuf+1); return p; }
inline void FreeWorkIfAny(){ if(S::ms_WorkBuffer && S::ms_WorkBuffer!=WorkP()){ delete[] S::ms_WorkBuffer; } S::ms_WorkBuffer=nullptr; }
inline bool (__cdecl* const SaveFn)(void*,int32)=reinterpret_cast<bool(__cdecl*)(void*,int32)>(0x5D1270);
inline bool (__cdecl* const LoadFn)(void*,int32)=reinterpret_cast<bool(__cdecl*)(void*,int32)>(0x5D1300);

// guion del caso actual
struct Ctx {
    bool openOk{true};
    uint32 blockBytes[28]{};
    int failBlock{-1};
    int flushFailAt{-1}; bool flushFailFlag{};      // SaveWorkBuffer / LoadWorkBuffer: indice de la llamada que falla
    int closeRes{};
    bool poolsRet{true};
    uint32 refillWbs{kBuf};                          // ms_WorkBufferSize tras una recarga (modo no virtual)
    uint32 seed{};
    std::vector<uint8> stream; size_t streamPos{}; bool streamMode{};
    uint32 saveCalls{}, loadCalls{};
};
inline Ctx sc;

// ======================= dobles =======================
// SaveWorkBuffer(bool) : cdecl. Imita al real: ms_CheckSum += bytes, anade el checksum si se pide, FilePos += Pos, Pos = 0.
inline bool __cdecl S_SaveWB(bool inc){
    if(!Mine())return false;
    const int idx=int(sc.saveCalls++);
    T("save_work_buffer",uint8(inc),S::ms_WorkBufferPos,S::ms_FilePos,Fnv(S::ms_WorkBuffer,size_t(S::ms_WorkBufferPos>0&&S::ms_WorkBufferPos<=int32(kBuf+1)?S::ms_WorkBufferPos:0)),S::ms_CheckSum,uint8(S::ms_bFailed));
    if(S::ms_bFailed)return false;
    if(idx==sc.flushFailAt){ if(sc.flushFailFlag)S::ms_bFailed=true; return false; }
    if(S::ms_WorkBufferPos==0)return true;
    if(S::ms_WorkBufferPos>0&&S::ms_WorkBufferPos<=int32(kBuf))for(int32 i=0;i<S::ms_WorkBufferPos;++i)S::ms_CheckSum+=S::ms_WorkBuffer[i];
    if(inc)S::ms_WorkBufferPos+=4;
    S::ms_FilePos+=uint32(S::ms_WorkBufferPos); S::ms_WorkBufferPos=0;
    return true; }
// LoadWorkBuffer() : cdecl. Recarga el bufer completo (patron o fichero virtual) y fija Pos = 0 y ms_WorkBufferSize = tamano de la recarga.
inline bool __cdecl S_LoadWB(){
    if(!Mine())return false;
    const int idx=int(sc.loadCalls++);
    T("load_work_buffer",S::ms_WorkBufferPos,S::ms_WorkBufferSize,S::ms_FilePos,uint8(S::ms_bFailed));
    if(S::ms_bFailed)return false;
    if(idx==sc.flushFailAt){ if(sc.flushFailFlag)S::ms_bFailed=true; return false; }
    if(sc.streamMode){
        const size_t chunk=std::min<size_t>(kBuf,sc.stream.size()-sc.streamPos);
        if(chunk==0)return false;
        std::memcpy(S::ms_WorkBuffer,sc.stream.data()+sc.streamPos,chunk); sc.streamPos+=chunk;
        S::ms_WorkBufferSize=uint32(chunk); S::ms_FilePos+=uint32(chunk);
    } else {
        Rng r{sc.seed+uint32(idx)*7919u+1u}; for(uint32 i=0;i<kBuf;++i)S::ms_WorkBuffer[i]=uint8(r.next()>>8);
        S::ms_WorkBufferSize=sc.refillWbs; S::ms_FilePos+=kBuf;
    }
    S::ms_WorkBufferPos=0;
    return true; }
inline bool __cdecl S_OpenW(){
    if(!Mine())return false;
    T("open_for_writing",uint8(sc.openOk));
    if(!sc.openOk)return false;
    S::ms_FileHandle=Handle(); S::ms_FilePos=0; S::ms_WorkBufferPos=0;
    if(!S::ms_WorkBuffer)S::ms_WorkBuffer=NewWork();
    return true; }
inline bool __cdecl S_OpenR(const char* name,int32 slot){
    if(!Mine())return false;
    T("open_for_reading",name?1:0,slot,uint8(sc.openOk));
    if(!sc.openOk)return false;
    S::ms_FileHandle=Handle(); S::ms_FileSize=uint32(sc.stream.size()); S::ms_FilePos=0; S::ms_WorkBufferSize=kBuf; S::ms_WorkBufferPos=int32(kBuf);
    if(!S::ms_WorkBuffer)S::ms_WorkBuffer=NewWork();
    return true; }
inline int32 __cdecl S_CloseFile(void* h){ if(!Mine())return 0; T("cfilemgr_close_file",uint8(h==Handle())); return sc.closeRes; }
inline void __cdecl S_Pads(){ if(Mine())T("update_pads"); }
inline void __cdecl S_ResetCheats(){ if(Mine())T("reset_cheats"); }
inline void __cdecl S_AfterLoad(){ if(Mine())T("after_success_load"); }
inline void __cdecl S_PoolsSetup(){ if(Mine())T("script_setup_after_pools"); }
inline void __cdecl S_Report(int32 block,int32 err){ if(Mine())T("report_error",block,err); }
// CSimpleVariablesSaveStructure::Construct() : thiscall; rellena los 0x138 bytes de forma determinista
inline void __fastcall S_Construct(void* self,void*){ if(!Mine())return; T("construct_vars"); Rng r{sc.seed|1u}; for(size_t i=0;i<0x138;++i)At<uint8>(self,i)=uint8(r.next()>>5); }
// CSimpleVariablesSaveStructure::Extract(uint32&) : thiscall, ret 4. Devuelve la version actual (evita la rama NOTSA).
inline void __fastcall S_Extract(void* self,void*,uint32& version){ if(!Mine())return; T("extract_vars",Fnv(self,0x138)); version=S::GetCurrentVersionNumber(); }

// bloques: Save (cdecl o thiscall segun el bloque) y Load
inline bool BlockSave(int b,void* self){
    if(!Mine())return true;
    const bool selfOk= b==5 ? self==reinterpret_cast<void*>(0x96F050) : b==15 ? self==reinterpret_cast<void*>(0xB7CD98+0x190*uintptr_t(AtA<uint8>(0xB7CD74))) : self==nullptr;
    T("save_block",b,uint8(selfOk));
    uint32 n=sc.blockBytes[b]; const uint8* src=pat.data()+((uint32(b)*977u)&0x3FFF);
    while(n){ const uint32 chunk=std::min<uint32>(n,30000u); if(!SaveFn(const_cast<uint8*>(src),int32(chunk)))break; n-=chunk; src+=chunk; }
    if(b==sc.failBlock)S::ms_bFailed=true;
    return true; }
inline bool BlockLoad(int b,void* self){
    if(!Mine())return true;
    const bool selfOk= b==5 ? self==reinterpret_cast<void*>(0x96F050) : b==15 ? self==reinterpret_cast<void*>(0xB7CD98+0x190*uintptr_t(AtA<uint8>(0xB7CD74))) : self==nullptr;
    uint32 n=sc.blockBytes[b]; uint64 h=1469598103934665603ull; uint32 total=0; bool ok=true;
    alignas(16) static std::array<uint8,30016> scratch{};
    while(n){ const uint32 chunk=std::min<uint32>(n,30000u); if(!LoadFn(scratch.data(),int32(chunk))){ok=false;break;} h=Fnv(scratch.data(),chunk,h); n-=chunk; total+=chunk; }
    T("load_block",b,uint8(selfOk),total,uint8(ok),h);
    if(b==sc.failBlock)S::ms_bFailed=true;
    return b==2 ? sc.poolsRet : true; }
template<int B> inline bool __cdecl SaveS(){ return BlockSave(B,nullptr); }
template<int B> inline bool __fastcall SaveM(void* self,void*){ return BlockSave(B,self); }
template<int B> inline bool __cdecl LoadS(){ return BlockLoad(B,nullptr); }
template<int B> inline bool __fastcall LoadM(void* self,void*){ return BlockLoad(B,self); }

// sprintf del EXE (0x821BB5) para ReportError: formatea de verdad y traza el texto
inline bool captureReport{};
inline int __cdecl S_Sprintf(char* dst,const char* fmt,...){
    va_list va; va_start(va,fmt); const int n=std::vsnprintf(dst,0x7FFFFFFF,fmt,va); va_end(va);
    if(Mine()&&captureReport)trace.push_back(json::array({"sprintf_msg",std::string(dst)}));
    return n; }
class CaptureSink : public spdlog::sinks::base_sink<std::mutex> {
protected:
    void sink_it_(const spdlog::details::log_msg& m) override { if(Mine()&&captureReport)trace.push_back(json::array({"sprintf_msg",std::string(m.payload.data(),m.payload.size())})); }
    void flush_() override {}
};

// ======================= tabla de funciones =======================
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,5> fns{{
    {"CGenericGameStorage","SaveDataToWorkBuffer-org",0x5D1270,0,-1,1620},
    {"CGenericGameStorage","LoadDataFromWorkBuffer-org",0x5D1300,0,-1,1620},
    {"CGenericGameStorage","ReportError",0x5D08C0,0,-1,208},
    {"CGenericGameStorage","GenericSave",0x5D13E0,0,-1,768},
    {"CGenericGameStorage","GenericLoad",0x5D17B0,0,-1,768},
}};

// ======================= fixtures =======================
inline json GlobalsOut(){ json o=json::object();
    o["pos"]=S::ms_WorkBufferPos; o["failed"]=uint8(S::ms_bFailed); o["loading"]=uint8(S::ms_bLoading); o["wbs"]=S::ms_WorkBufferSize;
    o["file_pos"]=S::ms_FilePos; o["checksum"]=S::ms_CheckSum; o["work_null"]=(S::ms_WorkBuffer==nullptr); return o; }

// 1620 casos = 9 tamanos x 10 posiciones x 2 (fallo previo) x 3 (modo de recarga) x 3 (ms_WorkBufferSize)
constexpr int32 kSizes[9]{-1,0,1,3,5,16,100,5000,40000};
constexpr int32 kPos[10]{0,1,317,40000,51100,51184,51195,51196,51199,51200};
constexpr uint32 kWbs[3]{0xC800,51000,40000};
inline json CaseWorkBuf(int i,bool isLoad){
    int k=i; const int si=k%9; k/=9; const int pi=k%10; k/=10; const int pf=k%2; k/=2; const int md=k%3; k/=3; const int wi=k%3;
    const int32 size=kSizes[si]; int32 pos=kPos[pi]; const uint32 wbs=kWbs[wi];
    // dominio cuerdo de Load: el EXE usa 0xC800 - pos como resto; si pos + size supera ms_WorkBufferSize el resto no puede superar el tamano pedido
    // Con ms_WorkBufferSize < 0xC800 el EXE (0x5D132D) se llama a si mismo con resto = 0xC800 - pos y los mismos pos/umbral: si resto > 0 recursa sin fin
    // (desbordamiento de pila, 0xC0000005 en el caso 575). Dominio cuerdo: la rama solo termina si resto <= 0, es decir pos = 0xC800.
    if(isLoad&&size>0&&uint32(pos+size)>wbs){ if(wbs<kBuf)pos=int32(kBuf); else if(int32(kBuf)-pos>size)pos=int32(kBuf)-size; }
    sc=Ctx{}; sc.seed=uint32(i)*2654435761u+1u; sc.flushFailAt= md ? 0 : -1; sc.flushFailFlag= md==2; sc.refillWbs=kWbs[(wi+1)%3];
    std::memset(workRaw.data(),0xA5,workRaw.size()); std::memset(callerRaw.data(),0xD3,callerRaw.size());
    { Rng r{sc.seed|1u}; for(uint32 j=0;j<kBuf;++j)WorkP()[j]=uint8(r.next()>>7); for(size_t j=0;j<kCaller;++j)CallerP()[j]=uint8(r.next()>>9); }
    S::ms_WorkBuffer=WorkP(); S::ms_WorkBufferPos=pos; S::ms_bFailed= pf; S::ms_WorkBufferSize=wbs; S::ms_FilePos=1234; S::ms_CheckSum=0x55AA; S::ms_FileHandle=Handle();
    const bool al= (isLoad ? LoadFn : SaveFn)(CallerP(),size);
    json o=GlobalsOut(); o["al"]=al; o["work"]=Fnv(workRaw.data(),workRaw.size()); o["caller"]=Fnv(callerRaw.data(),callerRaw.size());
    o["fixture"]={{"size",size},{"pos",pos},{"prior_failed",pf},{"mode",md},{"wbs",wbs}};
    S::ms_WorkBuffer=nullptr;
    return o; }

// 208 casos = 26 bloques x 8 codigos. Bloques 25, 26 y >= 28 usan un bufer sin inicializar en el EXE (fuera del corpus).
constexpr int kReportBlocks[26]{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,27};
constexpr int32 kReportCodes[8]{0,1,2,3,4,255,-1,0x7FFFFFFF};
inline json CaseReport(int i){
    const int32 block=kReportBlocks[i%26], code=kReportCodes[(i/26)%8];
    captureReport=true;
    reinterpret_cast<void(__cdecl*)(int32,int32)>(0x5D08C0)(block,code);
    captureReport=false;
    return json::object(); }

// --- GenericSave / GenericLoad ---
constexpr int kBlocks[24]{1,2,3,4,5,6,8,9,10,11,12,15,16,17,18,19,20,21,22,23,24,25,26,27};   // bloques con guardado/carga (el 0 lo hace la propia funcion; 7, 13 y 14 no hacen nada)
inline void Distribute(Rng& r,uint32 total,const int* blocks,int n){
    uint32 w[28]{}; uint64 sum=0; for(int j=0;j<n;++j){ w[blocks[j]]=r.below(100)+1; sum+=w[blocks[j]]; }
    uint32 left=total;
    for(int j=0;j<n;++j){ const int b=blocks[j]; const uint32 v= j==n-1 ? left : std::min<uint32>(left,uint32(uint64(total)*w[b]/sum)); sc.blockBytes[b]=v; left-=v; } }
// reparto de bytes de los bloques; en el 25 % de los casos el siguiente encabezado cae a 0..7 bytes del final del bufer (tras 5 + 0x138 + 5 bytes)
inline void FillBlocks(Rng& r){
    constexpr uint32 totals[14]{0,100,5000,49148,51200,51203,100000,153600,153601,200000,202743,202748,202760,230000};
    uint32 total= r.pct(70) ? totals[r.below(14)] : r.below(260000);
    if(r.pct(25)){
        const uint32 rr=r.below(8); sc.blockBytes[1]=kBuf-322-rr; total= total>sc.blockBytes[1] ? total-sc.blockBytes[1] : 0;
        constexpr int rest[23]{2,3,4,5,6,8,9,10,11,12,15,16,17,18,19,20,21,22,23,24,25,26,27}; Distribute(r,total,rest,23);
    } else Distribute(r,total,kBlocks,24);
    sc.failBlock= r.pct(10) ? kBlocks[r.below(24)] : -1;
    sc.flushFailAt= r.pct(18) ? int(r.below(5)) : -1; sc.flushFailFlag=r.pct(50); sc.closeRes= r.pct(15) ? 1 : 0; sc.poolsRet=!r.pct(30); }
inline json CaseSave(int i){
    Rng r{uint32(i)*2246822519u+0x51u}; sc=Ctx{}; sc.seed=r.next()|1u; sc.openOk=!r.pct(10); FillBlocks(r);
    // estado previo "basura" y nombres de guardado (0xC16DB8 y 0xC16030, 0x104 bytes cada uno)
    S::ms_WorkBuffer=nullptr; S::ms_WorkBufferPos=0x1234; S::ms_FilePos=0xABCD; S::ms_bFailed=true; S::ms_bLoading=false; S::ms_CheckSum=0xDEADBEEF; S::ms_FileHandle=nullptr; S::ms_WorkBufferSize=kBuf;
    constexpr uint32 lens[7]{0,5,100,255,258,259,300};
    const uint32 L=lens[r.below(7)]; auto* nm=reinterpret_cast<uint8*>(0xC16DB8); auto* js=reinterpret_cast<uint8*>(0xC16030);
    for(uint32 j=0;j<0x104;++j){ nm[j]= j<L ? uint8(0x41+j%26) : j==L ? uint8(0) : uint8(0xE0+j%16); js[j]=0x77; }
    const bool al=reinterpret_cast<bool(__cdecl*)()>(0x5D13E0)();
    json o=GlobalsOut(); o["al"]=al; o["just_saved"]=ReversingProbe::Bytes(js,0x104); o["save_name"]=Fnv(nm,0x104); o["name_len"]=L;
    FreeWorkIfAny();
    return o; }
inline json CaseLoad(int i){
    Rng r{uint32(i)*3266489917u+0x77u}; sc=Ctx{}; sc.seed=r.next()|1u; sc.openOk=!r.pct(10); sc.streamMode=true; FillBlocks(r);
    const int badBlock= r.pct(12) ? int(r.below(28)) : -1;
    // fichero virtual: 28 bloques con su encabezado "BLOCK" (uno puede estar corrupto) y su carga util
    sc.stream.clear();
    for(int b=0;b<28;++b){
        const char* tag="BLOCK"; for(int k=0;k<5;++k)sc.stream.push_back(uint8(tag[k]));
        if(b==badBlock)sc.stream[sc.stream.size()-5+r.below(5)]^=0x20;
        const uint32 n= b==0 ? 0x138u : sc.blockBytes[b];
        for(uint32 k=0;k<n;++k)sc.stream.push_back(uint8((k*31u+uint32(b)*7u+sc.seed)>>(k&3)));
    }
    S::ms_WorkBuffer=nullptr; S::ms_WorkBufferPos=0x1234; S::ms_FilePos=0xABCD; S::ms_bFailed=true; S::ms_bLoading=false; S::ms_CheckSum=0xDEADBEEF; S::ms_FileHandle=nullptr; S::ms_WorkBufferSize=0x777; S::ms_FileSize=0x999;
    bool variablesLoaded=true;
    const bool al=reinterpret_cast<bool(__cdecl*)(bool*)>(0x5D17B0)(&variablesLoaded);
    json o=GlobalsOut(); o["al"]=al; o["variables_loaded"]=variablesLoaded; o["stream_pos"]=sc.streamPos; o["stream_size"]=sc.stream.size(); o["bad_block"]=badBlock; o["file_size"]=S::ms_FileSize;
    FreeWorkIfAny();
    return o; }

// ======================= actualizacion principal =======================
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"storagefinal1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"storagefinal1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("storagefinal1");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"storagefinal1-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,5> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (si falta uno no se parchea ni se ejecuta nada) ---
    const uintptr_t dSaveWB=Body(&S::SaveWorkBuffer), dLoadWB=Body(&S::LoadWorkBuffer), dOpenW=Body(&S::OpenFileForWriting), dOpenR=Body(&S::OpenFileForReading);
    const uintptr_t dClose=Body(&CFileMgr::CloseFile), dPads=Body(&CPad::UpdatePads), dCheats=Body(&CCheat::ResetCheats);
    const uintptr_t dConstruct=Body(&CSimpleVariablesSaveStructure::Construct), dExtract=Body(&CSimpleVariablesSaveStructure::Extract);
    const uintptr_t dReport=Body(&S::ReportError), dAfter=Body(&S::DoGameSpecificStuffAfterSucessLoad), dPoolsSetup=Body(&CTheScripts::DoScriptSetupAfterPoolsHaveLoaded);
    struct Blk { int b; uintptr_t exeS, dllS, exeL, dllL; bool member; };
    const std::array<Blk,24> blks{{
        {1,0x5D4C40,Body(&CTheScripts::Save),0x5D4FD0,Body(&CTheScripts::Load),false},
        {2,0x5D0880,Body(&CPools::Save),0x5D0890,Body(&CPools::Load),false},
        {3,0x5D3160,Body(&CGarages::Save),0x5D3270,Body(&CGarages::Load),false},
        {4,0x5D33C0,Body(&CGameLogic::Save),0x5D3440,Body(&CGameLogic::Load),false},
        {5,0x5D34C0,Body(&CPathFind::Save),0x5D3500,Body(&CPathFind::Load),true},
        {6,0x5D3540,Body(&CPickups::Save),0x5D35A0,Body(&CPickups::Load),false},
        {8,0x5D3620,Body(&CRestart::Save),0x5D3770,Body(&CRestart::Load),false},
        {9,0x5D5860,Body(&CRadar::Save),0x5D53C0,Body(&CRadar::Load),false},
        {10,0x5D2E60,Body(&CTheZones::Save),0x5D2F40,Body(&CTheZones::Load),false},
        {11,0x5D3A60,Body(&CGangs::Save),0x5D3A90,Body(&CGangs::Load),false},
        {12,0x5D38C0,Body(&CTheCarGenerators::Save),0x5D39B0,Body(&CTheCarGenerators::Load),false},
        {15,0x5D3AC0,Body(&CPlayerInfo::Save),0x5D3B00,Body(&CPlayerInfo::Load),true},
        {16,0x5D3B40,Body(&CStats::Save),0x5D3BF0,Body(&CStats::Load),false},
        {17,0x5D3C70,Body(&CSetPieces::Save),0x5D3CA0,Body(&CSetPieces::Load),false},
        {18,0x5D29A0,Body(&CStreaming::Save),0x5D29E0,Body(&CStreaming::Load),false},
        {19,0x5D3CD0,Body(&CPedType::Save),0x5D3D10,Body(&CPedType::Load),false},
        {20,0x5D3D60,Body(&CTagManager::Save),0x5D3DA0,Body(&CTagManager::Load),false},
        {21,0x5D5420,Body(&CIplStore::Save),0x5D54A0,Body(&CIplStore::Load),false},
        {22,0x5D3DE0,Body(&CShopping::Save),0x5D3E40,Body(&CShopping::Load),false},
        {23,0x5D5530,Body(&CGangWars::Save),0x5D3EB0,Body(&CGangWars::Load),false},
        {24,0x5D5570,Body(&CStuntJumpManager::Save),0x5D5920,Body(&CStuntJumpManager::Load),false},
        {25,0x5D5970,Body(&CEntryExitManager::Save),0x5D55C0,Body(&CEntryExitManager::Load),false},
        {26,0x5D3EE0,Body(&CAERadioTrackManager::Save),0x5D40E0,Body(&CAERadioTrackManager::Load),false},
        {27,0x5D4300,Body(&C3dMarkers::SaveUser3dMarkers),0x5D42E0,Body(&C3dMarkers::LoadUser3dMarkers),false},
    }};
    {   std::set<uintptr_t> all{dSaveWB,dLoadWB,dOpenW,dOpenR,dClose,dPads,dCheats,dConstruct,dExtract,dReport,dAfter,dPoolsSetup}; bool ok=!all.count(0)&&all.size()==12;
        for(auto& k:blks){ for(auto a:{k.dllS,k.dllL}){ ok&=a!=0; ok&=all.insert(a).second; } }
        if(!ok){ Write({{"event","error"},{"reason","DLL bodies not found or overlapping"}}); return; } }
    // guardados/cargas de bloques: una plantilla por numero de bloque (las rutas 5 y 15 son miembros thiscall)
    auto AddBlocks=[&](Patches& P,bool save){
#define BLK(n) { const Blk& k=blks[size_t(idx)]; if(save){ if(k.member){P.Add(k.exeS,&SaveM<n>);P.Add(k.dllS,&SaveM<n>);} else {P.Add(k.exeS,&SaveS<n>);P.Add(k.dllS,&SaveS<n>);} } \
        else { if(k.member){P.Add(k.exeL,&LoadM<n>);P.Add(k.dllL,&LoadM<n>);} else {P.Add(k.exeL,&LoadS<n>);P.Add(k.dllL,&LoadS<n>);} } ++idx; }
        int idx=0;
        BLK(1) BLK(2) BLK(3) BLK(4) BLK(5) BLK(6) BLK(8) BLK(9) BLK(10) BLK(11) BLK(12) BLK(15) BLK(16) BLK(17) BLK(18) BLK(19) BLK(20) BLK(21) BLK(22) BLK(23) BLK(24) BLK(25) BLK(26) BLK(27)
#undef BLK
    };

    std::array<bool,5> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    { Rng g{12345u}; for(auto& b:pat)b=uint8(g.next()>>3); }
    // estado global guardado
    if(regions.empty()){ Snap(0xC16EE8,0x18); Snap(0xC16134,4); Snap(0xC16030,0x104); Snap(0xC16DB8,0x104); Snap(0x8D2BD0,0x20); Snap(0xC16EF8,8); }
    const auto sinkOwner=std::make_shared<CaptureSink>();
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        if(nm=="ReportError")spdlog::default_logger()->sinks().push_back(sinkOwner);
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                trace=json::array();
                hooks[f]->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                Patches P;
                if(f<2){ P.Add(0x5D0F80,S_SaveWB); P.Add(dSaveWB,S_SaveWB); P.Add(0x5D10B0,S_LoadWB); P.Add(dLoadWB,S_LoadWB);
                         out["state"]=CaseWorkBuf(i,f==1); }
                else if(f==2){ P.Add(0x821BB5,S_Sprintf); out["state"]=CaseReport(i); }
                else if(f==3){
                    P.Add(0x5D0F80,S_SaveWB); P.Add(dSaveWB,S_SaveWB); P.Add(0x5D0DD0,S_OpenW); P.Add(dOpenW,S_OpenW);
                    P.Add(0x5389D0,S_CloseFile); P.Add(dClose,S_CloseFile); P.Add(0x541E50,S_Pads); P.Add(dPads,S_Pads);
                    P.Add(0x5D1B80,S_Construct); P.Add(dConstruct,S_Construct);
                    AddBlocks(P,true);
                    out["state"]=CaseSave(i); }
                else {
                    P.Add(0x438450,S_ResetCheats); P.Add(dCheats,S_ResetCheats); P.Add(0x5D0D20,S_OpenR); P.Add(dOpenR,S_OpenR);
                    P.Add(0x5D10B0,S_LoadWB); P.Add(dLoadWB,S_LoadWB); P.Add(0x5D1B80,S_Construct); P.Add(dConstruct,S_Construct);
                    P.Add(0x5D1EA0,S_Extract); P.Add(dExtract,S_Extract); P.Add(0x5D08C0,S_Report); P.Add(dReport,S_Report);
                    P.Add(0x5389D0,S_CloseFile); P.Add(dClose,S_CloseFile); P.Add(0x618E90,S_AfterLoad); P.Add(dAfter,S_AfterLoad);
                    P.Add(0x5D3390,S_PoolsSetup); P.Add(dPoolsSetup,S_PoolsSetup);
                    AddBlocks(P,false);
                    out["state"]=CaseLoad(i); }
                out["trace"]=trace;
                RestoreRegions();
                return out;
            };
            const auto original=run(false), candidate=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                json sd=json::array(); if(original.contains("state")&&candidate.contains("state"))for(auto& [k,v]:original["state"].items())if(candidate["state"][k]!=v)sd.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_differs",sd},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        if(nm=="ReportError"){ auto& v=spdlog::default_logger()->sinks(); v.erase(std::remove(v.begin(),v.end(),std::static_pointer_cast<spdlog::sinks::sink>(sinkOwner)),v.end()); }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    bool restored=RegionsRestored();
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
