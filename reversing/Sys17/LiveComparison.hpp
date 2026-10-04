#pragma once
// Banco dentro del motor: CFileMgr (17 funciones, con InitUserDirectories) e IKChainManager_c (17).
// Original (hooks del lote desactivados) frente a candidato (activados) con fixtures deterministas.
// CFileMgr: nunca toca ficheros del juego. Las carpetas no se cambian de verdad: _getcwd/_chdir del EXE
// (0x836E91/0x836F1E) y de la DLL (_getcwd/_wgetcwd/_chdir/_wchdir) se simulan con traza; la busqueda de
// Mis documentos (registro en el EXE, SHGetFolderPathW en la DLL) y CreateFile/CreateDirectory/CloseHandle se
// simulan parcheando la IAT del EXE y la del ASI solo durante la llamada (otros hilos pasan a la funcion real).
// Las funciones de E/S trabajan sobre ficheros temporales en reversing/Sys17/artifacts/fmtmp: el original
// recibe un FILE* del CRT del EXE (fopen 0x8232D8) y el candidato uno del CRT de la DLL; se comparan retornos,
// buferes, posicion, indicadores EOF/error y el contenido final en disco. Se guardan y restauran ms_dirName,
// ms_rootDirName y los tres buferes de carpetas de usuario.
// IKChainManager_c: gestor falso (0xB18 bytes, cadenas y listas propias), peaton falso con inteligencia,
// tarea gestora IK y tareas IK falsas (vtable trampa), entidad objetivo falsa y arena para CTask::operator new.
// Se simulan y trazan: CPed::IsAlive, CEntity::GetIsOnScreen, CEntity::UpdateRpHAnim,
// BoneNodeManager_c::ReturnBoneNode, IKChain_c::Init/Update/Exit/IsFacingTarget,
// RpAnimBlendClumpGetAssociation, CTaskSimpleIKChain::BlendOut, UpdateLookAtInfo, UpdatePointArmInfo,
// AddIKChainTask y operator delete[] (solo en Exit/Reset, sin reservas). Se restauran el codigo de busqueda,
// el modo de camara del arma, byte_C1542C y las referencias de la entidad objetivo.
#include <fstream>
#include <cerrno>
#include <direct.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "FileMgr.h"
#include "extensions/Configs/Miscellaneous.hpp"
#include "Ragdoll/IKChainManager.h"
#include "Ragdoll/IKChain.h"
#include "Ragdoll/BoneNodeManager.h"
#include "TaskSimpleIKManager.h"
#include "TaskSimpleIKLookAt.h"
#include "TaskSimpleIKPointArm.h"
#include "../ProbeRuntime.hpp"

namespace Sys17Live {
inline bool done{}; inline std::ofstream log; inline json trace; inline DWORD benchTid{};
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline std::string Hex(uint64 h) { return std::format("{:016x}",h); }
// sin NaN de senalizacion (el x87 los silencia al copiarlos)
inline uint32 SafeWord(uint32 w) { if((w&0x7F800000u)==0x7F800000u&&(w&0x7FFFFFu)&&!(w&0x400000u))w|=0x400000u; return w; }
struct Rng { uint32 s{1}; uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; } float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; } };
inline Rng R;
inline void Fill(void* p,size_t n) { auto* w=static_cast<uint32*>(p); for(size_t k=0;k<n/4;++k)w[k]=SafeWord(R.next()); }
// registro sin reservas (operator delete[] sustituido)
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }

// =====================================================================================================
// CFileMgr
// =====================================================================================================
struct Crt {
    FILE*  (__cdecl* open)(const char*,const char*);
    int    (__cdecl* close)(FILE*);
    size_t (__cdecl* read)(void*,size_t,size_t,FILE*);
    size_t (__cdecl* write)(const void*,size_t,size_t,FILE*);
    int    (__cdecl* seek)(FILE*,long,int);
    long   (__cdecl* tell)(FILE*);
    char*  (__cdecl* gets)(char*,int,FILE*);
    bool   (__cdecl* err)(FILE*);
    bool   (__cdecl* eof)(FILE*);
};
// CRT del EXE: FILE::_flag en +0xC (_IOEOF 0x10, _IOERR 0x20); ftell por la interfaz de ficheros de RW como CFileMgr::Tell
inline long __cdecl XTell(FILE* f) { auto* fns=*reinterpret_cast<uint8**>(0xC97B24)+0xC4; return (*reinterpret_cast<long(__cdecl**)(void*)>(fns+0x28))(f); }
inline bool __cdecl XErr(FILE* f) { return (*reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(f)+0xC)&0x20)!=0; }
inline bool __cdecl XEof(FILE* f) { return (*reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(f)+0xC)&0x10)!=0; }
inline FILE*  __cdecl DOpen(const char* p,const char* m) { return fopen(p,m); }
inline int    __cdecl DClose(FILE* f) { return fclose(f); }
inline size_t __cdecl DRead(void* b,size_t s,size_t n,FILE* f) { return fread(b,s,n,f); }
inline size_t __cdecl DWriteF(const void* b,size_t s,size_t n,FILE* f) { return fwrite(b,s,n,f); }
inline int    __cdecl DSeek(FILE* f,long o,int w) { return fseek(f,o,w); }
inline long   __cdecl DTell(FILE* f) { return ftell(f); }
inline char*  __cdecl DGets(char* b,int n,FILE* f) { return fgets(b,n,f); }
inline bool   __cdecl DErr(FILE* f) { return ferror(f)!=0; }
inline bool   __cdecl DEof(FILE* f) { return feof(f)!=0; }
inline const Crt exeCrt{reinterpret_cast<FILE*(__cdecl*)(const char*,const char*)>(0x8232D8),reinterpret_cast<int(__cdecl*)(FILE*)>(0x82318B),
                        reinterpret_cast<size_t(__cdecl*)(void*,size_t,size_t,FILE*)>(0x823521),reinterpret_cast<size_t(__cdecl*)(const void*,size_t,size_t,FILE*)>(0x823674),
                        reinterpret_cast<int(__cdecl*)(FILE*,long,int)>(0x82374F),XTell,reinterpret_cast<char*(__cdecl*)(char*,int,FILE*)>(0x823798),XErr,XEof};
inline const Crt dllCrt{DOpen,DClose,DRead,DWriteF,DSeek,DTell,DGets,DErr,DEof};

inline std::filesystem::path tmpDir;
inline void Put(const std::filesystem::path& p,const std::vector<uint8>& d) {
    std::ofstream f(p,std::ios::binary|std::ios::trunc); if(!d.empty())f.write(reinterpret_cast<const char*>(d.data()),std::streamsize(d.size()));
}
inline json Disk(const std::filesystem::path& p) {
    std::error_code ec; if(!std::filesystem::exists(p,ec))return nullptr;
    std::ifstream f(p,std::ios::binary); std::vector<uint8> d((std::istreambuf_iterator<char>(f)),std::istreambuf_iterator<char>());
    return json::array({d.size(),Hex(Fnv(d.data(),d.size()))});
}
// estado del flujo tras la llamada (posicion solo en modo binario) y cierre con el CRT de su lado
inline json Post(const Crt& c,FILE* f,bool binary) {
    json r; r["tell"]=binary ? json(c.tell(f)) : json(nullptr); r["eof"]=c.eof(f); r["err"]=c.err(f); r["close"]=c.close(f); return r;
}

// carpetas: getcwd/chdir simulados
inline std::string cwdFix; inline int chdirRet{};
inline char* __cdecl FGetcwd(char* b,int n) { trace.push_back({"getcwd",n}); std::strcpy(b,cwdFix.c_str()); return b; }
inline wchar_t* __cdecl FWGetcwd(wchar_t* b,int n) { trace.push_back({"getcwd",n}); for(size_t k=0;k<=cwdFix.size();++k)b[k]=wchar_t(uint8(cwdFix[k])); errno=0; return b; }
inline int __cdecl FChdir(const char* p) { trace.push_back({"chdir",std::string(p)}); return chdirRet; }
inline std::string Narrow(const wchar_t* w) { std::string s; for(;*w;++w)s.push_back(*w<0x80 ? char(*w) : '?'); return s; }
inline int __cdecl FWChdir(const wchar_t* p) { trace.push_back({"chdir",Narrow(p)}); return chdirRet; }
alignas(16) inline std::array<char,256> docsBuf{};
inline char* __cdecl FInitUser() { trace.push_back({"init_user_dirs"}); return docsBuf.data(); }

// InitUserDirectories: IAT del EXE (registro y ficheros ANSI) y del ASI (SHGetFolderPathW y ficheros anchos)
inline std::string docsFix; inline int docMode{}; inline uint32 existMask{}, createCalls{}; inline bool lookupOk{};
inline void* realRegOpen{}; inline void* realRegQuery{}; inline void* realRegClose{};
inline void* realCreateA{}; inline void* realCloseA{}; inline void* realMkdirA{};
inline void* realShell{}; inline void* realCreateW{}; inline void* realCloseW{}; inline void* realMkdirW{};
constexpr uintptr_t FAKE_HKEY=0x51700, FAKE_HANDLE=0x51710;
inline bool Mine() { return GetCurrentThreadId()==benchTid; }
inline HANDLE OpenDir(const std::string& p,DWORD acc,DWORD share,LPSECURITY_ATTRIBUTES sa,DWORD disp,DWORD fl,HANDLE t) {
    trace.push_back({"open_dir",p,acc,share,sa!=nullptr,disp,fl,t!=nullptr}); const uint32 k=createCalls++;
    return (existMask>>k)&1 ? reinterpret_cast<HANDLE>(FAKE_HANDLE+k) : INVALID_HANDLE_VALUE;
}
inline HANDLE WINAPI XCreateFileA(LPCSTR p,DWORD a,DWORD s,LPSECURITY_ATTRIBUTES sa,DWORD d,DWORD f,HANDLE t) {
    if(!Mine())return reinterpret_cast<decltype(&XCreateFileA)>(realCreateA)(p,a,s,sa,d,f,t); return OpenDir(p,a,s,sa,d,f,t); }
inline HANDLE WINAPI DCreateFileW(LPCWSTR p,DWORD a,DWORD s,LPSECURITY_ATTRIBUTES sa,DWORD d,DWORD f,HANDLE t) {
    if(!Mine())return reinterpret_cast<decltype(&DCreateFileW)>(realCreateW)(p,a,s,sa,d,f,t); return OpenDir(Narrow(p),a,s,sa,d,f,t); }
inline BOOL WINAPI XCreateDirectoryA(LPCSTR p,LPSECURITY_ATTRIBUTES sa) {
    if(!Mine())return reinterpret_cast<decltype(&XCreateDirectoryA)>(realMkdirA)(p,sa); trace.push_back({"mkdir",std::string(p),sa!=nullptr}); return TRUE; }
inline BOOL WINAPI DCreateDirectoryW(LPCWSTR p,LPSECURITY_ATTRIBUTES sa) {
    if(!Mine())return reinterpret_cast<decltype(&DCreateDirectoryW)>(realMkdirW)(p,sa); trace.push_back({"mkdir",Narrow(p),sa!=nullptr}); return TRUE; }
inline bool Fake(HANDLE h) { return uintptr_t(h)>=FAKE_HANDLE&&uintptr_t(h)<FAKE_HANDLE+16; }
inline BOOL WINAPI XCloseHandle(HANDLE h) {
    if(!Mine()||!Fake(h))return reinterpret_cast<decltype(&XCloseHandle)>(realCloseA)(h); trace.push_back({"close_dir",uintptr_t(h)-FAKE_HANDLE}); return TRUE; }
inline BOOL WINAPI DCloseHandle(HANDLE h) {
    if(!Mine()||!Fake(h))return reinterpret_cast<decltype(&DCloseHandle)>(realCloseW)(h); trace.push_back({"close_dir",uintptr_t(h)-FAKE_HANDLE}); return TRUE; }
inline LSTATUS WINAPI XRegOpen(HKEY k,LPCSTR sub,DWORD o,REGSAM s,PHKEY out) {
    if(!Mine())return reinterpret_cast<decltype(&XRegOpen)>(realRegOpen)(k,sub,o,s,out);
    lookupOk=k==HKEY_CURRENT_USER&&sub&&std::string(sub)=="Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders"&&o==0&&s==KEY_READ;
    if(docMode==1)return ERROR_FILE_NOT_FOUND; *out=reinterpret_cast<HKEY>(FAKE_HKEY); return ERROR_SUCCESS;
}
inline LSTATUS WINAPI XRegQuery(HKEY k,LPCSTR name,LPDWORD res,LPDWORD type,LPBYTE data,LPDWORD cb) {
    if(!Mine()||uintptr_t(k)!=FAKE_HKEY)return reinterpret_cast<decltype(&XRegQuery)>(realRegQuery)(k,name,res,type,data,cb);
    lookupOk=lookupOk&&name&&std::string(name)=="Personal"&&!res&&data&&cb&&*cb==0x100;
    if(docMode==2)return ERROR_FILE_NOT_FOUND;
    std::memcpy(data,docsFix.c_str(),docsFix.size()+1); *cb=DWORD(docsFix.size()+1); if(type)*type=REG_SZ; return ERROR_SUCCESS;
}
inline LSTATUS WINAPI XRegClose(HKEY k) { if(!Mine()||uintptr_t(k)!=FAKE_HKEY)return reinterpret_cast<decltype(&XRegClose)>(realRegClose)(k); return ERROR_SUCCESS; }
inline HRESULT WINAPI DShell(HWND w,int csidl,HANDLE tok,DWORD fl,LPWSTR path) {
    if(!Mine())return reinterpret_cast<decltype(&DShell)>(realShell)(w,csidl,tok,fl,path);
    lookupOk=w==nullptr&&csidl==5/*CSIDL_PERSONAL*/&&tok==nullptr&&fl==0/*SHGFP_TYPE_CURRENT*/;
    if(docMode)return E_FAIL; for(size_t k=0;k<=docsFix.size();++k)path[k]=wchar_t(uint8(docsFix[k])); return S_OK;
}
inline void** FindIat(HMODULE mod,const void* target) {
    if(!mod||!target)return nullptr;
    auto* base=reinterpret_cast<uint8*>(mod); auto* nt=reinterpret_cast<IMAGE_NT_HEADERS*>(base+reinterpret_cast<IMAGE_DOS_HEADER*>(base)->e_lfanew);
    const auto& dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_IMPORT]; if(!dir.VirtualAddress)return nullptr;
    for(auto* d=reinterpret_cast<IMAGE_IMPORT_DESCRIPTOR*>(base+dir.VirtualAddress);d->Name;++d)
        for(auto* t=reinterpret_cast<void**>(base+d->FirstThunk);*t;++t)if(*t==target)return t;
    return nullptr;
}
struct IatPatch {
    void** slot{}; void* saved{};
    IatPatch(void** s,void* r) : slot(s) { if(slot){saved=*slot;Set(r);} }
    void Set(void* v) const { DWORD o{},ign{}; VirtualProtect(slot,4,PAGE_READWRITE,&o); *slot=v; VirtualProtect(slot,4,o,&ign); }
    ~IatPatch() { if(slot)Set(saved); }
};
inline std::array<void**,4> dllSlots{}; // SHGetFolderPathW, CreateFileW, CreateDirectoryW, CloseHandle del ASI

// =====================================================================================================
// IKChainManager_c
// =====================================================================================================
alignas(16) inline std::array<uint8,0xB18> mgr{};
alignas(16) inline std::array<uint8,sizeof(CPed)> ped{};
alignas(16) inline std::array<uint8,sizeof(CPed)> target{};
alignas(16) inline std::array<uint8,0x300> intel{};
alignas(16) inline std::array<uint8,0x40> ikm{};
alignas(16) inline std::array<std::array<uint8,0x60>,4> tasks{};
alignas(16) inline std::array<uint8,0x200> arena{};
alignas(16) inline std::array<uint8,0x40> clump{};
alignas(16) inline std::array<uint8,0x80> frame{};
alignas(16) inline std::array<uint8,0x60> mat{};
alignas(16) inline std::array<std::array<uint8,0x40>,3> cpeds{};
alignas(16) inline std::array<std::array<uint32,4>,32> bones{};
alignas(16) inline std::array<uint8,0x100> boneNodes{};
alignas(16) inline std::array<uint8,0x60> chainFake{};
inline const char* const purposes[3]{"Sys17","","LookAtTest"};
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    auto in=[&](const void* b,size_t n,const std::string& nm)->std::optional<std::string>{ if(v>=uintptr_t(b)&&v<uintptr_t(b)+n)return nm+"+"+std::to_string(v-uintptr_t(b)); return std::nullopt; };
    for(int k=0;k<3;++k)if(v==uintptr_t(purposes[k]))return "purpose#"+std::to_string(k);
    if(v==0xC10820)return "bone_node_manager";
    if(auto s=in(mgr.data(),mgr.size(),"mgr"))return *s; if(auto s=in(ped.data(),ped.size(),"ped"))return *s;
    if(auto s=in(target.data(),target.size(),"target"))return *s; if(auto s=in(intel.data(),intel.size(),"intel"))return *s;
    if(auto s=in(ikm.data(),ikm.size(),"ikm"))return *s; if(auto s=in(arena.data(),arena.size(),"arena"))return *s;
    for(int k=0;k<4;++k)if(auto s=in(tasks[k].data(),tasks[k].size(),"task"+std::to_string(k)))return *s;
    if(auto s=in(clump.data(),clump.size(),"clump"))return *s; if(auto s=in(frame.data(),frame.size(),"frame"))return *s;
    if(auto s=in(mat.data(),mat.size(),"mat"))return *s; if(auto s=in(chainFake.data(),chainFake.size(),"chain"))return *s;
    for(int k=0;k<3;++k)if(auto s=in(cpeds[k].data(),cpeds[k].size(),"cped"+std::to_string(k)))return *s;
    if(auto s=in(bones.data(),sizeof(bones),"bones"))return *s; if(auto s=in(boneNodes.data(),boneNodes.size(),"bone_nodes"))return *s;
    return v;
}
inline json Name(const void* p) { return Name(uintptr_t(p)); }
inline json Region(const void* p,size_t n,size_t from=0) { json r=json::array(); const auto* w=static_cast<const uint32*>(p); for(size_t k=from/4;k<n/4;++k)r.push_back(Name(uintptr_t(w[k]))); return r; }

// tareas falsas: una llamada virtual inesperada queda en la traza
template<int N> void* __fastcall FTrap(void*,void*) { trace.push_back({"unexpected_slot",N}); return nullptr; }
inline std::array<void*,16> trapVt{};
template<int... N> void FillTraps(std::integer_sequence<int,N...>) { ((trapVt[N]=reinterpret_cast<void*>(&FTrap<N>)),...); }

// dependencias simuladas
inline bool aliveRet{}, onScreenRet{}, facingRet{}, initRet{}; inline uint32 assocMask{}, assocCalls{};
inline size_t arenaUsed{}; inline std::vector<std::pair<size_t,size_t>> allocs;
inline bool __fastcall FIsAlive(void* p,void*) { trace.push_back({"is_alive",Name(p)}); return aliveRet; }
inline bool __fastcall FOnScreen(void* e,void*) { trace.push_back({"is_on_screen",Name(e)}); return onScreenRet; }
inline void __fastcall FUpdateRpHAnim(void* e,void*) { trace.push_back({"update_rp_hanim",Name(e)}); }
inline CAnimBlendAssociation* __cdecl FAssoc(RpClump* c,uint32 id) {
    trace.push_back({"get_association",Name(c),id}); return (assocMask>>(assocCalls++%32))&1 ? reinterpret_cast<CAnimBlendAssociation*>(boneNodes.data()+0x80) : nullptr; }
inline void __fastcall FBlendOut(void* t,void*,int32 ms) { trace.push_back({"blend_out",Name(t),ms}); }
inline void __fastcall FUpdLook(void* t,void*,const char* pu,void* p,void* e,int32 time,int32 bone,float x,float y,float z,uint32 torso,float speed,int32 blend,int32 prio) {
    trace.push_back({"update_look_at",Name(t),Name(pu),Name(p),Name(e),time,bone,F(x),F(y),F(z),torso&0xFF,F(speed),blend,prio}); }
inline void __fastcall FUpdPoint(void* t,void*,const char* pu,void* e,int32 bone,float x,float y,float z,float speed,int32 blend) {
    trace.push_back({"update_point_arm",Name(t),Name(pu),Name(e),bone,F(x),F(y),F(z),F(speed),blend}); }
inline void __fastcall FAddTask(void* m,void*,void* t,int32 slot) { trace.push_back({"add_ik_chain_task",Name(m),Name(t),slot}); }
inline void* __cdecl FTaskNew(size_t n) {
    trace.push_back({"task_new",n}); if(arenaUsed+0x80>arena.size()||n>0x80)return nullptr;
    void* p=arena.data()+arenaUsed; allocs.emplace_back(arenaUsed,n); arenaUsed+=0x80; return p; }
inline bool __fastcall FIsFacing(void* c,void*) { trace.push_back({"is_facing_target",Name(c)}); return facingRet; }
inline bool __fastcall FChainInit(void* c,void*,const char* nm,int32 slot,void* p,int32 eff,float ex,float ey,float ez,int32 piv,void* ent,int32 off,float ox,float oy,float oz,float speed,int32 prio) {
    trace.push_back({"chain_init",Name(c),Name(nm),slot,Name(p),eff,F(ex),F(ey),F(ez),piv,Name(ent),off,F(ox),F(oy),F(oz),F(speed),prio}); return initRet; }
inline void __fastcall FChainUpdate(void* c,void*,float dt) { trace.push_back({"chain_update",Name(c),F(dt)}); }
inline void __fastcall FChainExit(void* c,void*) { trace.push_back({"chain_exit",Name(c)}); }
inline void __fastcall FReturnBone(void* self,void*,void* b) { Push(1,uint32(uintptr_t(self)),uint32(uintptr_t(b))); }
inline void __cdecl FDeleteArr(void* p) { Push(2,uint32(uintptr_t(p))); }
inline json Recs() { json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag==1 ? "return_bone_node" : "delete_array",Name(uintptr_t(recs[i].a)),Name(uintptr_t(recs[i].b))}); return r; }
inline void CleanRefs() { auto* e=reinterpret_cast<CEntity*>(target.data()); while(auto* r=e->m_pReferences)e->CleanUpOldReference(r->m_ppEntity); }
inline json RefList() { json l=json::array(); for(auto* r=reinterpret_cast<CEntity*>(target.data())->m_pReferences;r;r=r->m_pNext)l.push_back(Name(uintptr_t(r->m_ppEntity))); return l; }
inline uint32& W(void* base,size_t off) { return *reinterpret_cast<uint32*>(static_cast<uint8*>(base)+off); }

// =====================================================================================================
inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NFNS=34;
inline const std::array<Fn,NFNS> fns{{
    {"CFileMgr","Initialise",0x5386F0,0,-1,128},{"CFileMgr","ChangeDir",0x538730,0,-1,512},{"CFileMgr","SetDir",0x5387D0,0,-1,512},
    {"CFileMgr","SetDirMyDocuments",0x538860,0,-1,256},{"CFileMgr","InitUserDirectories",0x744FB0,0,-1,256},
    {"CFileMgr","LoadFile",0x538890,0,-1,256},{"CFileMgr","OpenFile",0x538900,0,-1,384},{"CFileMgr","OpenFileForWriting",0x538910,0,-1,128},
    {"CFileMgr","OpenFileForAppending",0x538930,0,-1,128},{"CFileMgr","Read",0x538950,0,-1,512},{"CFileMgr","Write",0x538970,0,-1,384},
    {"CFileMgr","Seek",0x538990,0,-1,512},{"CFileMgr","ReadLine",0x5389B0,0,-1,512},{"CFileMgr","CloseFile",0x5389D0,0,-1,128},
    {"CFileMgr","GetTotalSize",0x5389E0,0,-1,256},{"CFileMgr","Tell",0x538A20,0,-1,256},{"CFileMgr","GetErrorReadWrite",0x538A50,0,-1,256},
    {"IKChainManager_c","Init",0x6180A0,0,-1,64},{"IKChainManager_c","Exit",0x6180D0,0,-1,256},{"IKChainManager_c","Reset",0x618140,0,-1,128},
    {"IKChainManager_c","Update",0x6186D0,0,-1,512},{"IKChainManager_c","AddIKChain",0x618750,0,-1,512},{"IKChainManager_c","RemoveIKChain",0x618170,0,-1,512},
    {"IKChainManager_c","CanAccept",0x618800,0,-1,1024},{"IKChainManager_c","CanAcceptLookAt",0x6188B0,0,-1,1024},
    {"IKChainManager_c","LookAt",0x618970,0,-1,1024},{"IKChainManager_c","PointArm",0x618B60,0,-1,1024},
    {"IKChainManager_c","IsLooking",0x6181A0,0,-1,256},{"IKChainManager_c","GetLookAtEntity",0x6181D0,0,-1,256},
    {"IKChainManager_c","GetLookAtOffset",0x618210,0,-1,256},{"IKChainManager_c","AbortLookAt",0x618280,0,-1,256},
    {"IKChainManager_c","IsArmPointing",0x6182B0,0,-1,256},{"IKChainManager_c","AbortPointArm",0x6182F0,0,-1,256},
    {"IKChainManager_c","IsFacingTarget",0x618330,0,-1,256},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"sys17-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"sys17-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("sys17");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"sys17-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NFNS> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NFNS> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    benchTid=GetCurrentThreadId();
    FillTraps(std::make_integer_sequence<int,16>{});

    // carpeta temporal del banco (nunca ficheros del juego)
    { std::filesystem::path p(__FILE__); std::error_code ec;
      tmpDir=p.is_absolute() ? std::filesystem::weakly_canonical(p.parent_path()/"artifacts"/"fmtmp",ec) : dir/"sys17-tmp";
      std::filesystem::create_directories(tmpDir,ec); }
    const auto work=tmpDir/"work.bin", missing=tmpDir/"missing.bin", nodir=tmpDir/"nodir"/"x.bin";
    const std::string workS=work.string(), missingS=missing.string(), nodirS=nodir.string();

    // IAT: EXE (fijas, ver InitUserDirectories 0x744FB0) y ASI (buscadas por valor)
    HMODULE self{}; GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS|GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,reinterpret_cast<LPCSTR>(&done),&self);
    const HMODULE k32=GetModuleHandleA("kernel32.dll"), sh32=GetModuleHandleA("shell32.dll");
    auto proc=[&](HMODULE m,const char* n)->void*{ return m ? reinterpret_cast<void*>(GetProcAddress(m,n)) : nullptr; };
    dllSlots={FindIat(self,proc(sh32,"SHGetFolderPathW")),FindIat(self,proc(k32,"CreateFileW")),FindIat(self,proc(k32,"CreateDirectoryW")),FindIat(self,proc(k32,"CloseHandle"))};
    const bool dllIatOk=dllSlots[0]&&dllSlots[1]&&dllSlots[2]&&dllSlots[3];
    Write({{"event","info"},{"tmp",tmpDir.string()},{"dll_iat",dllIatOk},{"exe_iat",json::array({
        *reinterpret_cast<void**>(0x858008)==proc(GetModuleHandleA("advapi32.dll"),"RegOpenKeyExA"),*reinterpret_cast<void**>(0x8580A0)==proc(k32,"CreateFileA"),
        *reinterpret_cast<void**>(0x858120)==proc(k32,"CreateDirectoryA"),*reinterpret_cast<void**>(0x858078)==proc(k32,"CloseHandle")})}});

    // estado global guardado
    const auto savedDir=ReversingProbe::Bytes(CFileMgr::ms_dirName,128), savedRoot=ReversingProbe::Bytes(CFileMgr::ms_rootDirName,128);
    auto* userDirs=reinterpret_cast<uint8*>(0xC92168); const std::vector<uint8> savedUser(userDirs,userDirs+0x300);
    auto& scan=*reinterpret_cast<uint16*>(0xB7CD78); const uint16 savedScan=scan;
    auto& camMode=*reinterpret_cast<int16*>(0xB6F858); const int16 savedCamMode=camMode;
    auto& disableLookAts=*reinterpret_cast<uint8*>(0xC1542C); const uint8 savedDisable=disableLookAts;
    const auto savedSaveDir=g_MiscConfig.SaveDirectoryPath;
    auto restoreFm=[&]{ std::memcpy(CFileMgr::ms_dirName,savedDir.data(),128); std::memcpy(CFileMgr::ms_rootDirName,savedRoot.data(),128); std::memcpy(userDirs,savedUser.data(),0x300); };
    auto freeRefs=[]{int n=0;for(auto* r=CReferences::pEmptyList;r;r=r->m_pNext)++n;return n;}; const int savedFree=freeRefs();

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch s1(bit_cast<uintptr_t>(&CPed::IsAlive),FIsAlive), s1e(0x5E0170,FIsAlive);
        EntryPatch s2(bit_cast<uintptr_t>(&CEntity::GetIsOnScreen),FOnScreen), s2e(0x534540,FOnScreen);
        EntryPatch s3(bit_cast<uintptr_t>(&CEntity::UpdateRpHAnim),FUpdateRpHAnim), s3e(0x532B20,FUpdateRpHAnim);
        EntryPatch s4(bit_cast<uintptr_t>(&BoneNodeManager_c::ReturnBoneNode),FReturnBone), s4e(0x617470,FReturnBone);
        EntryPatch s5(bit_cast<uintptr_t>(&IKChain_c::Init),FChainInit), s5e(0x618370,FChainInit);
        EntryPatch s6(bit_cast<uintptr_t>(&IKChain_c::Update),FChainUpdate), s6e(0x6184B0,FChainUpdate);
        EntryPatch s7(bit_cast<uintptr_t>(&IKChain_c::IsFacingTarget),FIsFacing), s7e(0x617E60,FIsFacing);
        EntryPatch s8(bit_cast<uintptr_t>(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation)),FAssoc), s8e(0x4D68B0,FAssoc);
        EntryPatch s9(bit_cast<uintptr_t>(&CTaskSimpleIKChain::BlendOut),FBlendOut), s9e(0x633C40,FBlendOut);
        EntryPatch s10(bit_cast<uintptr_t>(&CTaskSimpleIKLookAt::UpdateLookAtInfo),FUpdLook), s10e(0x634050,FUpdLook);
        EntryPatch s11(bit_cast<uintptr_t>(&CTaskSimpleIKPointArm::UpdatePointArmInfo),FUpdPoint), s11e(0x634370,FUpdPoint);
        EntryPatch s12(bit_cast<uintptr_t>(&CTaskSimpleIKManager::AddIKChainTask),FAddTask), s12e(0x633940,FAddTask);
        EntryPatch s13(bit_cast<uintptr_t>(static_cast<void*(*)(size_t)>(&CTask::operator new)),FTaskNew), s13e(0x61A5A0,FTaskNew);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                auto run=[&](bool candidate)->json {
                    R.s=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5517u; if(!R.s)R.s=1;
                    trace=json::array(); nrec=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    const Crt& C=candidate ? dllCrt : exeCrt;

                    if(cls=="CFileMgr") {
                        auto randStr=[&](size_t lo,size_t hi,const char* alpha){ std::string s; const size_t n=lo+R.next()%(hi-lo+1); const size_t al=std::strlen(alpha); for(size_t k=0;k<n;++k)s.push_back(alpha[R.next()%al]); return s; };
                        auto prefill=[&](char* buf,size_t n,const std::string& s){ for(size_t k=0;k<n;++k)buf[k]=char(1+R.next()%255); std::memcpy(buf,s.c_str(),s.size()+1); };
                        if(nm=="Initialise"||nm=="ChangeDir"||nm=="SetDir"||nm=="SetDirMyDocuments") {
                            static const char* roots[]{"C:\\Sys17\\Root\\","D:\\Grand Theft Auto San Andreas\\","C:\\","E:\\a\\b\\","C:\\Juegos\\GTA SA"};
                            static const char* rels[]{"","models\\","data\\maps\\","anim","DATA\\Paths\\"};
                            static const char* args[]{"","\\","\\models","\\models\\","data","data\\","anim\\","\\DATA\\MAPS","x","\\x\\y\\z\\w","..\\","a\\b"};
                            std::string root=R.next()%5 ? roots[R.next()%5] : randStr(1,20,"abcXYZ\\:. ");
                            std::string cur=R.next()%4 ? root+rels[R.next()%5] : randStr(0,40,"abcdef\\._ ");
                            if(cur.size()>70)cur.resize(70);
                            prefill(CFileMgr::ms_rootDirName,128,root); prefill(CFileMgr::ms_dirName,128,cur);
                            std::string arg=R.next()%3 ? args[R.next()%12] : randStr(1,20,"abcdefgh\\._");
                            cwdFix=R.next()%4 ? std::string(roots[R.next()%5]) : randStr(1,60,"C:\\abcdefXYZ _.");
                            if(cwdFix.back()=='\\'&&cwdFix.size()>3&&R.next()%2)cwdFix.pop_back();
                            chdirRet=R.next()%5==0 ? -1 : 0;
                            prefill(docsBuf.data(),docsBuf.size(),R.next()%4 ? std::string("C:\\Users\\Sys17\\Documents\\GTA San Andreas User Files") : randStr(1,100,"C:\\abcXYZ _."));
                            {
                                EntryPatch g1(0x836E91,FGetcwd), g2(bit_cast<uintptr_t>(&_getcwd),FGetcwd), g3(bit_cast<uintptr_t>(&_wgetcwd),FWGetcwd);
                                EntryPatch c1(0x836F1E,FChdir), c2(bit_cast<uintptr_t>(&_chdir),FChdir), c3(bit_cast<uintptr_t>(&_wchdir),FWChdir);
                                if(nm=="Initialise")reinterpret_cast<void(__cdecl*)()>(a)();
                                else if(nm=="SetDirMyDocuments"){ EntryPatch u1(0x744FB0,FInitUser), u2(bit_cast<uintptr_t>(&InitUserDirectories),FInitUser); ret=reinterpret_cast<int32(__cdecl*)()>(a)(); }
                                else ret=reinterpret_cast<int32(__cdecl*)(const char*)>(a)(arg.c_str());
                            }
                            out["dir"]=ReversingProbe::Bytes(CFileMgr::ms_dirName,128); out["root"]=ReversingProbe::Bytes(CFileMgr::ms_rootDirName,128);
                            restoreFm();
                        }
                        else if(nm=="InitUserDirectories") {
                            for(size_t k=0;k<0x300;++k)userDirs[k]=uint8(1+R.next()%255);
                            userDirs[0x200]=0; // la DLL guarda cache: con el primer byte a 0 recorre la funcion entera como el EXE
                            static const char* tails[]{"\\Documents","\\OneDrive\\Documentos","\\Docs","\\My Documents"};
                            docsFix=std::string(R.next()%2 ? "C:\\Users\\" : "D:\\Perfiles\\")+randStr(3,10,"abcdefghijklmnopqrstuvwxyz")+tails[R.next()%4];
                            while(R.next()%3==0&&docsFix.size()<130)docsFix+="\\"+randStr(1,12,"abcdefXYZ _.");
                            docMode=R.next()%5==0 ? int(1+R.next()%2) : 0; existMask=R.next()&7; createCalls=0; lookupOk=false;
                            if(!dllIatOk){ out["error"]="dll iat not found"; }
                            else {
                                g_MiscConfig.SaveDirectoryPath.reset();
                                char* r{};
                                {
                                    // punteros reales antes de parchear (otros hilos pasan por los stubs hacia ellos)
                                    auto at=[](uintptr_t s){ return *reinterpret_cast<void**>(s); };
                                    realRegOpen=at(0x858008); realRegQuery=at(0x85800C); realRegClose=at(0x858000); realCreateA=at(0x8580A0); realCloseA=at(0x858078); realMkdirA=at(0x858120);
                                    realShell=*dllSlots[0]; realCreateW=*dllSlots[1]; realMkdirW=*dllSlots[2]; realCloseW=*dllSlots[3];
                                    IatPatch e1(reinterpret_cast<void**>(0x858008),reinterpret_cast<void*>(&XRegOpen)), e2(reinterpret_cast<void**>(0x85800C),reinterpret_cast<void*>(&XRegQuery)),
                                             e3(reinterpret_cast<void**>(0x858000),reinterpret_cast<void*>(&XRegClose)), e4(reinterpret_cast<void**>(0x8580A0),reinterpret_cast<void*>(&XCreateFileA)),
                                             e5(reinterpret_cast<void**>(0x858078),reinterpret_cast<void*>(&XCloseHandle)), e6(reinterpret_cast<void**>(0x858120),reinterpret_cast<void*>(&XCreateDirectoryA));
                                    IatPatch d1(dllSlots[0],reinterpret_cast<void*>(&DShell)), d2(dllSlots[1],reinterpret_cast<void*>(&DCreateFileW)),
                                             d3(dllSlots[2],reinterpret_cast<void*>(&DCreateDirectoryW)), d4(dllSlots[3],reinterpret_cast<void*>(&DCloseHandle));
                                    r=reinterpret_cast<char*(__cdecl*)()>(a)();
                                }
                                g_MiscConfig.SaveDirectoryPath=savedSaveDir;
                                ret=r==reinterpret_cast<char*>(0xC92368) ? json("gta_user_dir_path") : r ? json(uintptr_t(r)) : json(nullptr);
                                out["lookup_ok"]=lookupOk;
                            }
                            out["user_dirs"]=ReversingProbe::Bytes(userDirs,0x300);
                            restoreFm();
                        }
                        else {
                            // ficheros temporales: contenido identico en ambas ejecuciones
                            std::error_code ec; std::filesystem::remove(missing,ec);
                            auto genBinary=[&]{ static const size_t sizes[]{0,1,7,100,4095,4096,4097,0x3FFF,0x4000,0x4001,0x8000,12345};
                                const size_t n=R.next()%3 ? R.next()%20000 : sizes[R.next()%12]; std::vector<uint8> d(n); for(auto& b:d)b=uint8(R.next()); return d; };
                            auto genText=[&]{ std::vector<uint8> d; const int lines=int(R.next()%60);
                                for(int l=0;l<lines;++l){ const int len=int(R.next()%(R.next()%5==0 ? 400u : 80u)); for(int k=0;k<len;++k)d.push_back(uint8(32+R.next()%95));
                                    const uint32 e=R.next()%4; if(e==3&&l+1==lines)break; if(e==0)d.push_back('\r'); d.push_back('\n'); }
                                return d; };
                            auto pick=[&](std::initializer_list<const char*> l){ return std::string(*(l.begin()+R.next()%l.size())); };
                            auto isBin=[](const std::string& m){ return m.find('b')!=std::string::npos; };
                            auto canWrite=[](const std::string& m){ return m.find_first_of("wa+")!=std::string::npos; };
                            if(nm=="LoadFile") {
                                const std::string mode=pick({"rb","rb","r","rt"}); const bool bin=isBin(mode);
                                const auto content=bin ? genBinary() : genText(); Put(work,content);
                                const uint32 ps=R.next()%20; const std::string& path=ps==0 ? missingS : ps==1 ? nodirS : workS;
                                std::vector<uint8> buf(content.size()+0x4000+64); for(auto& b:buf)b=uint8(R.next());
                                const size_t r=reinterpret_cast<size_t(__cdecl*)(const char*,uint8*,size_t,const char*)>(a)(path.c_str(),buf.data(),R.next(),mode.c_str());
                                ret=r;
                                // en modo texto el CRT puede dejar bytes crudos tras lo devuelto: solo se compara lo leido y el terminador
                                out["buf"]=bin||r==size_t(-1) ? Hex(Fnv(buf.data(),buf.size())) : Hex(Fnv(buf.data(),std::min(buf.size(),r+1)));
                            }
                            else if(nm=="OpenFile"||nm=="OpenFileForWriting"||nm=="OpenFileForAppending") {
                                const std::string mode=nm=="OpenFileForWriting" ? "wb" : nm=="OpenFileForAppending" ? "a" : pick({"rb","r","wb","w","ab","a","r+b","w+b","rt","wt"});
                                const bool bin=isBin(mode); Put(work,bin ? genBinary() : genText());
                                const uint32 ps=R.next()%8; const std::string& path=ps==0 ? missingS : ps==1 ? nodirS : workS;
                                FILE* fp=nm=="OpenFile" ? reinterpret_cast<FILE*(__cdecl*)(const char*,const char*)>(a)(path.c_str(),mode.c_str())
                                                        : reinterpret_cast<FILE*(__cdecl*)(const char*)>(a)(path.c_str());
                                ret=fp!=nullptr;
                                if(fp){
                                    if(canWrite(mode)){ static const char marker[]="SYS17 marcador\n"; out["wrote"]=C.write(marker,1,sizeof(marker)-1,fp); }
                                    else { std::vector<uint8> d(70000); const size_t n=C.read(d.data(),1,d.size(),fp); out["read"]=json::array({n,Hex(Fnv(d.data(),n))}); }
                                    out["post"]=Post(C,fp,bin&&mode[0]!='a');
                                }
                                out["disk"]=Disk(work); out["disk_missing"]=Disk(missing);
                            }
                            else {
                                // flujo abierto con el CRT de cada lado
                                std::string mode; std::vector<uint8> content;
                                if(nm=="Read")mode=pick({"rb","r+b","r"});
                                else if(nm=="Write")mode=pick({"wb","r+b","ab","a","w","rb"});
                                else if(nm=="Seek")mode=pick({"rb","r+b"});
                                else if(nm=="ReadLine")mode=pick({"r","rb","rt"});
                                else if(nm=="CloseFile")mode=pick({"rb","wb","ab","r+b"});
                                else if(nm=="GetErrorReadWrite")mode=pick({"rb","r+b","wb"});
                                else mode=pick({"rb","r+b","wb"}); // Tell, GetTotalSize
                                const bool bin=isBin(mode);
                                content=(nm=="ReadLine"||!bin) ? genText() : genBinary(); Put(work,content);
                                FILE* fp=C.open(workS.c_str(),mode.c_str());
                                if(!fp){ out["error"]="fixture open failed"; }
                                else {
                                    const bool readable=mode[0]=='r', writeOnly=mode[0]!='r'&&mode.find('+')==std::string::npos;
                                    std::vector<uint8> scratch(0x2000);
                                    // operacion previa
                                    const uint32 pre=R.next()%3;
                                    if(nm=="ReadLine"){ const uint32 skip=R.next()%6; std::array<char,512> l{}; for(uint32 k=0;k<skip;++k)C.gets(l.data(),int(l.size()),fp); }
                                    else if(nm=="Write"){ if(mode=="r+b"&&pre)C.seek(fp,long(R.next()%(content.size()+1)),SEEK_SET); }
                                    else if(nm=="GetErrorReadWrite"){
                                        if(readable&&pre==1){ while(C.read(scratch.data(),1,scratch.size(),fp)==scratch.size()){} }
                                        else if(pre==2){ const uint8 b[8]{1,2,3,4,5,6,7,8}; C.write(b,1,1+R.next()%8,fp); } // en "rb" deja el error puesto
                                    }
                                    else if(writeOnly){ if(pre){ std::vector<uint8> d(R.next()%9000); for(auto& b:d)b=uint8(R.next()); C.write(d.data(),1,d.size(),fp); } }
                                    else if(bin&&pre==1){ C.seek(fp,long(R.next()%(content.size()+10)),SEEK_SET); }
                                    else if(pre==2){ C.read(scratch.data(),1,R.next()%5000,fp); }
                                    if(nm=="Read"){
                                        static const size_t sizes[]{0,1,0x1000,0x4000,4095,4097};
                                        const size_t n=R.next()%4 ? R.next()%20000 : sizes[R.next()%6];
                                        std::vector<uint8> buf(n+64); for(auto& b:buf)b=uint8(R.next());
                                        const size_t r=reinterpret_cast<size_t(__cdecl*)(FILE*,void*,size_t)>(a)(fp,buf.data(),n); ret=r;
                                        out["buf"]=bin ? Hex(Fnv(buf.data(),buf.size())) : Hex(Fnv(buf.data(),std::min(r,buf.size())));
                                    }
                                    else if(nm=="Write"){
                                        const size_t n=mode=="rb" ? 1+R.next()%200 : R.next()%4 ? R.next()%9000 : size_t(4096*(R.next()%3));
                                        std::vector<uint8> d(n); for(auto& b:d)b=uint8(R.next());
                                        ret=reinterpret_cast<size_t(__cdecl*)(FILE*,const void*,size_t)>(a)(fp,d.data(),n);
                                    }
                                    else if(nm=="Seek"){
                                        const int origin=int(R.next()%3); const long sz=long(content.size());
                                        long off=origin==SEEK_SET ? long(R.next()%(sz+51))-20 : origin==SEEK_CUR ? long(R.next()%(2*sz+41))-sz-20 : long(R.next()%(sz+71))-sz-20;
                                        ret=reinterpret_cast<bool(__cdecl*)(FILE*,long,int32)>(a)(fp,off,origin);
                                        out["pos"]=C.tell(fp); std::array<uint8,32> b{}; const size_t n=C.read(b.data(),1,b.size(),fp); out["next"]=std::vector<uint8>(b.begin(),b.begin()+n);
                                    }
                                    else if(nm=="ReadLine"){
                                        std::array<char,512> buf{}; for(auto& b:buf)b=char(R.next());
                                        const int32 n=R.next()%4 ? int32(2+R.next()%299) : int32(2+R.next()%8);
                                        ret=reinterpret_cast<bool(__cdecl*)(FILE*,char*,int32)>(a)(fp,buf.data(),n);
                                        out["buf"]=ReversingProbe::Bytes(buf.data(),buf.size());
                                    }
                                    else if(nm=="CloseFile"){ ret=reinterpret_cast<int32(__cdecl*)(FILE*)>(a)(fp); fp=nullptr; }
                                    else ret=reinterpret_cast<int32(__cdecl*)(FILE*)>(a)(fp); // GetTotalSize, Tell, GetErrorReadWrite
                                    if(fp)out["post"]=Post(C,fp,bin&&mode[0]!='a'); // en modo anexar la posicion antes de escribir depende del CRT
                                }
                                out["disk"]=Disk(work);
                            }
                        }
                    }
                    else { // IKChainManager_c
                        // --- fixture comun ---
                        std::memset(ped.data(),0,ped.size()); std::memset(intel.data(),0,intel.size());
                        std::memset(target.data(),0,target.size()); reinterpret_cast<CEntity*>(target.data())->SetType(ENTITY_TYPE_PED);
                        Fill(ikm.data(),ikm.size()); for(auto& t:tasks)Fill(t.data(),t.size()); Fill(clump.data(),clump.size()); clump[0]=2;
                        Fill(frame.data(),frame.size()); Fill(mat.data(),mat.size()); Fill(chainFake.data(),chainFake.size());
                        for(auto& c:cpeds)Fill(c.data(),c.size());
                        std::memset(arena.data(),0xCD,arena.size()); arenaUsed=0; allocs.clear();
                        void* P=ped.data(); const CVector cam=TheCamera.GetPosition();
                        static const float ranges[]{20.f,999.f,998.9999f,999.0001f,0.f,-5.f,std::numeric_limits<float>::infinity(),std::numeric_limits<float>::quiet_NaN()};
                        float range=nm=="CanAcceptLookAt"||nm=="LookAt" ? 20.f : R.next()%3 ? R.uni(0.f,60.f) : ranges[R.next()%8];
                        { const float base=std::isfinite(range)&&std::fabs(range)<200.f ? std::fabs(range) : 100.f; const float dist=R.next()%10==0 ? 0.f : base*R.uni(0.5f,1.5f);
                          CVector d{R.uni(-1.f,1.f),R.uni(-1.f,1.f),R.uni(-1.f,1.f)}; float len=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z); if(len<1e-3f){d=CVector{1.f,0.f,0.f};len=1.f;}
                          const CVector pos{cam.x+d.x/len*dist,cam.y+d.y/len*dist,cam.z+d.z/len*dist};
                          *reinterpret_cast<CVector*>(ped.data()+4)=pos;
                          if(R.next()%3){ W(P,0x14)=uint32(uintptr_t(mat.data())); *reinterpret_cast<CVector*>(mat.data()+0x30)=pos; *reinterpret_cast<CVector*>(ped.data()+4)=CVector{R.uni(-9.f,9.f),R.uni(-9.f,9.f),R.uni(-9.f,9.f)}; } }
                        W(P,0x18)=R.next()%8 ? uint32(uintptr_t(clump.data())) : 0; W(clump.data(),4)=uint32(uintptr_t(frame.data()));
                        W(P,0x474)=R.next(); W(P,0x528)=R.next(); W(P,0x47C)=uint32(uintptr_t(intel.data()));
                        const bool needMgr=nm=="AbortLookAt"||nm=="AbortPointArm";
                        W(intel.data(),0x2C)=(needMgr||R.next()%4) ? uint32(uintptr_t(ikm.data())) : 0;
                        W(ikm.data(),0)=uint32(uintptr_t(trapVt.data()));
                        for(int k=0;k<4;++k){ W(ikm.data(),8+4*k)=R.next()%2 ? uint32(uintptr_t(tasks[k].data())) : 0;
                            W(tasks[k].data(),0)=uint32(uintptr_t(trapVt.data())); tasks[k][0x58]=uint8(R.next()%2); tasks[k][0x59]=uint8(int8(int32(R.next()%17)-4));
                            W(tasks[k].data(),0x10)=R.next()%3 ? uint32(uintptr_t(chainFake.data())) : 0; }
                        aliveRet=R.next()%6!=0; onScreenRet=R.next()%6!=0; facingRet=R.next()%2; initRet=R.next()%5!=0; assocMask=R.next()%4==0 ? R.next() : 0; assocCalls=0;
                        { static const int16 modes[]{7,4,53,16,0}; camMode=R.next()%5==0 ? int16(7) : modes[R.next()%5]; }
                        disableLookAts=uint8(R.next()%6==0);
                        // gestor falso: cadenas en listas activa/libre enlazadas como List_c
                        void* M=mgr.data(); Fill(M,mgr.size());
                        {
                            std::array<std::vector<int>,2> lists;
                            for(int c=0;c<32;++c){ uint32 s=R.next()%3; if(nm=="Init"&&s==2)s=0; if(s)lists[s-1].push_back(c); }
                            if(lists[0].empty()&&(nm=="RemoveIKChain"||R.next()%2)){ for(int c=0;c<32;++c)if(std::find(lists[1].begin(),lists[1].end(),c)==lists[1].end()){lists[0].push_back(c);break;} }
                            if(lists[0].empty()&&nm=="RemoveIKChain"){ lists[0].push_back(lists[1].back()); lists[1].pop_back(); }
                            for(auto& l:lists)for(size_t k=l.size();k>1;--k)std::swap(l[k-1],l[R.next()%k]);
                            for(int L=0;L<2;++L){ auto& l=lists[L]; auto ch=[&](int c){ return mgr.data()+c*0x58; };
                                for(size_t k=0;k<l.size();++k){ W(ch(l[k]),0)=k ? uint32(uintptr_t(ch(l[k-1]))) : 0; W(ch(l[k]),4)=k+1<l.size() ? uint32(uintptr_t(ch(l[k+1]))) : 0; }
                                W(M,0xB00+L*0xC)=l.empty() ? 0 : uint32(uintptr_t(ch(l.front()))); W(M,0xB04+L*0xC)=l.empty() ? 0 : uint32(uintptr_t(ch(l.back()))); W(M,0xB08+L*0xC)=uint32(l.size()); }
                            for(int c=0;c<32;++c){ auto* ch=mgr.data()+c*0x58;
                                W(ch,8)=R.next()%4 ? uint32(uintptr_t(cpeds[R.next()%3].data())) : 0; W(ch,0xC)=R.next()%5; W(ch,0x10)=uint32(uintptr_t(bones[c].data()));
                                for(int k=0;k<4;++k)bones[c][k]=uint32(uintptr_t(boneNodes.data()+(c*4+k)%0x80)); ch[0x55]=uint8(R.next()%8 ? R.next()%4 : R.next()); }
                            out["lists"]=json::array({lists[0],lists[1]});
                            if(nm=="RemoveIKChain")out["pick"]=lists[0][R.next()%lists[0].size()];
                        }
                        // --- llamada ---
                        CVector offVec{R.uni(-2.f,2.f),R.uni(-2.f,2.f),R.uni(-2.f,2.f)};
                        auto boolArg=[&](uint32 v){ return R.next()%8==0 ? (v|(R.next()&0xFFFFFF00u)) : v; };
                        static const int32 bonesTags[]{5,4,3,2,0,24,34,0x10005};
                        if(nm=="Init")ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(M);
                        else if(nm=="Exit"||nm=="Reset"){ EntryPatch d(0x8213AE,FDeleteArr); reinterpret_cast<void(__thiscall*)(void*)>(a)(M); }
                        else if(nm=="Update"){
                            scan=uint16(1+R.next()%0xFFF0); // sin vuelta a 0xFFFF: CWorld::ClearScanCodes es protegida y no se puede simular en la DLL
                            for(auto& c:cpeds)*reinterpret_cast<uint16*>(c.data()+0x2C)=R.next()%2 ? uint16(scan+1+R.next()%4) : uint16(R.next());
                            const float dt=R.next()%4 ? R.uni(0.f,0.1f) : R.uni(-1.f,5.f);
                            reinterpret_cast<void(__thiscall*)(void*,float)>(a)(M,dt);
                            out["scan"]=scan; json cs=json::array(); for(auto& c:cpeds)cs.push_back(*reinterpret_cast<uint16*>(c.data()+0x2C)); out["cped_scan"]=cs;
                            scan=savedScan;
                        }
                        else if(nm=="AddIKChain"){
                            const char* pu=purposes[R.next()%3]; const int32 slot=int32(R.next()%4); void* ent=R.next()%3 ? target.data() : nullptr;
                            auto r=reinterpret_cast<void*(__thiscall*)(void*,const char*,int32,void*,int32,float,float,float,int32,void*,int32,float,float,float,float,int32)>(a)(
                                M,pu,slot,P,bonesTags[R.next()%8],R.uni(-1.f,1.f),R.uni(-1.f,1.f),R.uni(-1.f,1.f),bonesTags[R.next()%8],ent,bonesTags[R.next()%8],
                                R.uni(-3.f,3.f),R.uni(-3.f,3.f),R.uni(-3.f,3.f),R.uni(0.f,1.f),int32(R.next()%8)-2);
                            ret=Name(r);
                        }
                        else if(nm=="RemoveIKChain"){
                            EntryPatch e1(0x617870,FChainExit), e2(bit_cast<uintptr_t>(&IKChain_c::Exit),FChainExit);
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(M,mgr.data()+out["pick"].get<int>()*0x58);
                        }
                        else if(nm=="CanAccept")ret=reinterpret_cast<bool(__thiscall*)(void*,void*,float)>(a)(M,P,range);
                        else if(nm=="CanAcceptLookAt")ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(M,P);
                        else if(nm=="LookAt"){
                            const char* pu=purposes[R.next()%3]; void* ent=R.next()%4 ? target.data() : nullptr;
                            static const int32 times[]{-1,0,1,250,500,3000,10000,999999};
                            const int32 time=times[R.next()%8]; const int32 bone=bonesTags[R.next()%8]; CVector* off=R.next()%3 ? &offVec : nullptr;
                            const uint32 torso=boolArg(R.next()%2); const float speed=R.next()%4 ? R.uni(0.f,1.f) : 0.25f; const int32 blend=int32(R.next()%2001);
                            const int32 prio=int32(R.next()%14)-3; const uint32 force=boolArg(R.next()%4==0);
                            reinterpret_cast<void(__thiscall*)(void*,const char*,void*,void*,int32,int32,CVector*,uint32,float,int32,int32,uint32)>(a)(M,pu,P,ent,time,bone,off,torso,speed,blend,prio,force);
                        }
                        else if(nm=="PointArm"){
                            const char* pu=purposes[R.next()%3]; const int32 arm=int32(R.next()%2); void* ent=R.next()%4 ? target.data() : nullptr;
                            const int32 bone=bonesTags[R.next()%8]; CVector* off=R.next()%3 ? &offVec : nullptr; const float speed=R.uni(0.f,1.f); const int32 blend=int32(R.next()%2001);
                            reinterpret_cast<void(__thiscall*)(void*,const char*,int32,void*,void*,int32,CVector*,float,int32,float)>(a)(M,pu,arm,P,ent,bone,off,speed,blend,range);
                        }
                        else if(nm=="IsLooking")ret=reinterpret_cast<bool(__thiscall*)(void*,void*)>(a)(M,P);
                        else if(nm=="GetLookAtEntity")ret=Name(reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(M,P));
                        else if(nm=="GetLookAtOffset"){ CVector o{-1234.f,-1234.f,-1234.f}; reinterpret_cast<void(__thiscall*)(void*,void*,CVector*)>(a)(M,P,&o); ret=json::array({F(o.x),F(o.y),F(o.z)}); }
                        else if(nm=="AbortLookAt")reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(a)(M,P,R.next()%3 ? 250u : R.next()%3000);
                        else if(nm=="IsArmPointing")ret=reinterpret_cast<bool(__stdcall*)(int32,void*)>(a)(int32(R.next()%2),P);
                        else if(nm=="AbortPointArm")reinterpret_cast<void(__stdcall*)(int32,void*,int32)>(a)(int32(R.next()%2),P,R.next()%3 ? 250 : int32(R.next()%3000));
                        else if(nm=="IsFacingTarget")ret=reinterpret_cast<bool(__thiscall*)(void*,void*,int32)>(a)(M,P,int32(R.next()%4));
                        // --- resultado ---
                        const bool mgrFn=nm=="Init"||nm=="Exit"||nm=="Reset"||nm=="Update"||nm=="AddIKChain"||nm=="RemoveIKChain";
                        out["mgr"]=mgrFn ? Region(mgr.data(),mgr.size()) : json(Hex(Fnv(mgr.data(),mgr.size())));
                        out["recs"]=Recs(); out["intel"]=Region(intel.data(),0x40); out["ikm"]=Region(ikm.data(),ikm.size());
                        json ts=json::array(); for(auto& t:tasks)ts.push_back(Hex(Fnv(t.data(),t.size()))); out["tasks"]=ts;
                        out["ped"]=Hex(Fnv(ped.data(),ped.size())); out["cpeds"]=Hex(Fnv(cpeds.data(),sizeof(cpeds)));
                        json al=json::array(); for(auto& [off,size]:allocs){ auto* t=reinterpret_cast<CTask*>(arena.data()+off);
                            al.push_back({{"offset",off},{"size",size},{"type",int(t->GetTaskType())},{"words",Region(arena.data()+off,std::min<size_t>(size,0x80),4)}}); }
                        out["arena"]=al; out["refs"]=RefList();
                        CleanRefs();
                    }
                    out["ret"]=ret; out["trace"]=trace; return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&!original.contains("error");
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
    restoreFm(); scan=savedScan; camMode=savedCamMode; disableLookAts=savedDisable; g_MiscConfig.SaveDirectoryPath=savedSaveDir; CleanRefs();
    { std::error_code ec; std::filesystem::remove(work,ec); std::filesystem::remove(missing,ec); }
    const bool restored=std::memcmp(CFileMgr::ms_dirName,savedDir.data(),128)==0&&std::memcmp(CFileMgr::ms_rootDirName,savedRoot.data(),128)==0
                      &&std::memcmp(userDirs,savedUser.data(),0x300)==0&&scan==savedScan&&camMode==savedCamMode&&disableLookAts==savedDisable&&freeRefs()==savedFree;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
