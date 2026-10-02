#pragma once
// Banco dentro del motor: nucleo y datos (lista de 70, 44 funciones): CBox, CBuilding, CCollisionPlugin,
// CColTrianglePlane, CCompressedMatrixNotAligned, CControllerState, CData, CDraw, CDummyObject, CEntryInfoNode,
// CNodeRoute, CObjectData, CObjectPool, CPedPool, CObjectSaveStructure, CSimpleVariablesSaveStructure,
// CVehicleSaveStructure, CTempColModels, ModelIndices, FxPrimBP_c, CWindModifiers y CPlantColEntEntry.
// Original (hooks del lote desactivados) frente a candidato con fixtures deterministas. Los pools son siempre
// FALSOS (puntero global sustituido durante la llamada); los objetos, entidades y estructuras de guardado son
// buffers propios. Se simulan y trazan: rand, operator new[]/delete[] (sin reservar memoria), CFileMgr
// (Read/ReadLine/SetDir/OpenFile/CloseFile), CFileLoader::LoadLine, CMemoryMgr::Malloc/Free (arena propia),
// CModelInfo::GetModelInfo(UInt16), CStreaming::RemoveModel, CObject::Create, CheckIfNewEntityNeedsScript,
// Attach/DetachFromRwObject, Register/CleanUpOldReference, CEntity::GetColModel, CPlantLocTri::Release,
// CColModel::RemoveCollisionVolumes, FindFxSystemBP, FxMemoryPool_c::GetMem, FxInfoManager_c::Load, CText::Get,
// GetLocalTime, las funciones de truco 90/91 y el registro de plugins RW. Todo el estado global tocado (tablas,
// relojes, clima, cámara, trucos, plantillas de colisión, índices de modelo, modificadores de viento, listas de
// plantas, punteros de pool) se guarda y se restaura tras cada par y al final.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ModelInfo.h"
#include "FileMgr.h"
#include "FileLoader.h"
#include "MemoryMgr.h"
#include "FxManager.h"
#include "FxMemoryPool.h"
#include "Text/Text.h"
#include "Object.h"
#include "ColModel.h"
#include "PlantLocTri.h"
#include "ScriptsForBrains.h"
#include "../ProbeRuntime.hpp"

namespace Core70Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline uint32 seed{}, rngCalls{};
inline int __cdecl FakeRand() { trace.push_back({"rand"}); return int((seed*2654435761u+rngCalls++*40503u)>>9)&0x7FFF; }
inline uint16 FakeRandomNumber() { return uint16(FakeRand()); }

// --- registro sin reservas (stubs activos mientras operator new[] está sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- normalización: los dwords que apuntan a buffers conocidos se sustituyen por (región, desplazamiento) ---
struct Region { const void* p; size_t n; };
inline json Norm(const void* p,size_t n,std::initializer_list<Region> rs){
    json r=json::array(); auto* b=static_cast<const uint8*>(p);
    for(size_t i=0;i+4<=n;i+=4){ uint32 v; std::memcpy(&v,b+i,4); uint32 o=v; uint32 k=0;
        for(auto& R:rs){ const auto s=uint32(uintptr_t(R.p)); if(R.p&&v>=s&&v<s+R.n){ o=0xA0000000u|(k<<24)|(v-s); break; } ++k; }
        r.push_back(o); }
    for(size_t i=n&~size_t(3);i<n;++i)r.push_back(b[i]);
    return r;
}
inline json Rel(const void* p,const void* base,size_t n){ const auto u=uintptr_t(p), s=uintptr_t(base); return p==nullptr ? json(nullptr) : u>=s&&u<s+n ? json(intptr_t(u-s)) : json("ext"); }

// --- arena para operator new[] / CMemoryMgr::Malloc ---
alignas(16) inline std::array<uint8,0x4000> arena{}; inline size_t arenaUsed{};
inline void* __cdecl FakeNewArr(size_t n){ Push(1,uint32(n)); return n<=0x400 ? arena.data() : nullptr; }
inline void __cdecl FakeDelArr(void* p){ Push(2,uint32(intptr_t(p)-intptr_t(arena.data()))); }
inline void* ArenaAlloc(uint32 size){ const size_t off=arenaUsed; if(off+size>arena.size())return nullptr; arenaUsed=(off+size+15)&~size_t(15); return arena.data()+off; }
inline void* __cdecl FakeMalloc2(uint32 size,uint32 hint){ Push(3,size,hint); return ArenaAlloc(size); }
inline void* __cdecl FakeMalloc1(uint32 size){ Push(3,size,0); return ArenaAlloc(size); }
inline void __cdecl FakeFree(void* p){ Push(4,uint32(intptr_t(p)-intptr_t(arena.data()))); }

// --- CFileMgr / CFileLoader ---
inline uint32 readState{}, readCalls{}, readFailMask{};
inline size_t __cdecl FakeRead(FILESTREAM f,void* buf,size_t size){ auto* b=static_cast<uint8*>(buf); for(size_t i=0;i<size;++i){readState=readState*1103515245u+12345u;b[i]=uint8(readState>>16);}
    const bool fail=(readFailMask>>(readCalls++%32))&1; Push(5,uint32(uintptr_t(f)),uint32(size)); return fail ? 0 : size; }
inline std::vector<std::string> lines; inline size_t lineIdx{}; inline char lineBuf[512]{};
inline char* __cdecl FakeLoadLine(FILESTREAM f){ trace.push_back({"load_line",uintptr_t(f)}); if(lineIdx>=lines.size())return nullptr; strncpy(lineBuf,lines[lineIdx++].c_str(),sizeof(lineBuf)-1); return lineBuf; }
inline bool __cdecl FakeReadLine(FILESTREAM f,char* buf,int32 n){ trace.push_back({"read_line",uintptr_t(f),n}); if(lineIdx>=lines.size()){ if(n>0)buf[0]=0; return false; }
    strncpy(buf,lines[lineIdx++].c_str(),size_t(n-1)); buf[n-1]=0; return true; }
inline int32 __cdecl FakeSetDir(const char* p){ trace.push_back({"set_dir",std::string(p)}); return 0; }
inline FILESTREAM __cdecl FakeOpenFile(const char* p,const char* m){ trace.push_back({"open",std::string(p),std::string(m)}); return reinterpret_cast<FILESTREAM>(0x4321); }
inline int32 __cdecl FakeCloseFile(FILESTREAM f){ trace.push_back({"close",uintptr_t(f)}); return 0; }

// --- modelos / streaming ---
alignas(16) inline std::array<std::array<uint8,0x40>,8> fakeMis{};
inline uint32 miSalt{};
inline CBaseModelInfo* __cdecl FakeGetMI(const char* n,int32* idx){ const auto h=uint32(Fnv(n,strlen(n)))^miSalt; trace.push_back({"get_mi",std::string(n),idx!=nullptr});
    if(h%5==0)return nullptr; if(idx)*idx=int32(h%20000); return reinterpret_cast<CBaseModelInfo*>(fakeMis[h%8].data()); }
constexpr uintptr_t MI_IDX_BEGIN=0x8CD4F4, MI_IDX_END=0x8CD784;
inline CBaseModelInfo* __cdecl FakeGetMI16(const char* n,uint16* idx){ const auto h=uint32(Fnv(n,strlen(n)))^miSalt;
    trace.push_back({"get_mi16",std::string(n),idx ? json(intptr_t(uintptr_t(idx))-intptr_t(MI_IDX_BEGIN)) : json(nullptr)});
    if(h%4==0)return nullptr; if(idx)*idx=uint16(h%20000); return reinterpret_cast<CBaseModelInfo*>(fakeMis[h%8].data()); }
inline void __cdecl FakeRemoveModel(int32 id){ trace.push_back({"remove_model",id}); }
inline void* fakeBld{};
inline void __fastcall FakeDeleteRw(void* self,void*){ trace.push_back({"delete_rw",self==fakeBld}); }
inline void __fastcall FakeVTrap(void* self,void*){ trace.push_back({"vtrap"}); }

// --- plugins RW ---
inline std::array<void*,6> pluginCbs{}; inline int32 pluginRet{}, streamRet{};
inline RwInt32 __cdecl FakeRegPlugin(RwInt32 size,RwUInt32 id,void* c,void* d,void* cp){ trace.push_back({"reg_plugin",size,id}); pluginCbs[0]=c; pluginCbs[1]=d; pluginCbs[2]=cp; return pluginRet; }
inline RwInt32 __cdecl FakeRegStream(RwUInt32 id,void* r,void* w,void* g){ trace.push_back({"reg_stream",id}); pluginCbs[3]=r; pluginCbs[4]=w; pluginCbs[5]=g; return streamRet; }

// --- CDummyObject ---
inline void* fakeDummy{}; inline void* fakeObj{}; inline void* fakeObjResult{};
inline CObject* __cdecl FakeObjCreate(CDummyObject* d){ trace.push_back({"obj_create",d==fakeDummy,(*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(d)+0x1C)>>13)&1}); return static_cast<CObject*>(fakeObjResult); }
inline void __fastcall FakeCheckScript(void* self,void*,CEntity* e,int8 type,void* unused){ trace.push_back({"check_script",uintptr_t(self)==0xA90CF0,e==fakeObj,type,uintptr_t(unused)}); }
inline void __fastcall FakeAttach(void* self,void*,RwObject* o,bool upd){ trace.push_back({"attach",self==fakeDummy,uintptr_t(o),upd,(*reinterpret_cast<uint32*>(static_cast<uint8*>(fakeObj)+0x1C)>>13)&1}); }
inline void __fastcall FakeDetach(void* self,void*){ trace.push_back({"detach",self==fakeObj}); }

// --- FX ---
alignas(16) inline std::array<uint8,0x20> fxMat{};
inline FxSystemBP_c* __fastcall FakeFindFx(void* self,void*,const char* n){ const auto h=uint32(Fnv(n,strlen(n))); trace.push_back({"find_fx",uintptr_t(self)==0xA9AE80,std::string(n)}); return reinterpret_cast<FxSystemBP_c*>(uintptr_t(0x05000000u+(h%64)*0x10)); }
inline void* __fastcall FakeGetMem(void* self,void*,int32 size,int32 align){ trace.push_back({"get_mem",intptr_t(uintptr_t(self))-intptr_t(0xA9AE80),size,align}); return fxMat.data(); }
inline void* fakeFxObj{};
inline void __fastcall FakeInfoLoad(void* self,void*,FILESTREAM f,int32 v){ trace.push_back({"info_load",intptr_t(uintptr_t(self))-intptr_t(uintptr_t(fakeFxObj)),uintptr_t(f),v}); }

// --- texto / hora / trucos ---
inline std::array<std::array<GxtChar,100>,7> gxt{}; inline uint32 gxtSalt{};
inline const GxtChar* __fastcall FakeTextGet(void* self,void*,const char* key){ trace.push_back({"text_get",uintptr_t(self)==0xC1B340,std::string(key)}); return gxt[(uint32(Fnv(key,strlen(key)))^gxtSalt)%gxt.size()].data(); }
inline volatile uint32 timeSeed{};
inline void WINAPI FakeGetLocalTime(LPSYSTEMTIME t){ const uint32 s=timeSeed; t->wYear=WORD(2000+s%30); t->wMonth=WORD(1+s%12); t->wDayOfWeek=WORD(s%7); t->wDay=WORD(1+s%28); t->wHour=WORD(s%24); t->wMinute=WORD((s>>5)%60); t->wSecond=WORD((s>>11)%60); t->wMilliseconds=WORD((s>>17)%1000); }
inline void __cdecl FakeCheat90(){ trace.push_back({"cheat_fn",90}); }
inline void __cdecl FakeCheat91(){ trace.push_back({"cheat_fn",91}); }

// --- colisión / plantas ---
constexpr uintptr_t TCM_BEGIN=0x968A00, TCM_END=0x969000;
inline void __fastcall FakeRemoveColVol(void* self,void*){ trace.push_back({"remove_col",intptr_t(uintptr_t(self))-intptr_t(TCM_BEGIN)}); }
alignas(16) inline std::array<uint8,8*0x14> ents{}; alignas(16) inline std::array<uint8,0x40> plantEnt{}, plantCol{}, plantColData{}; alignas(16) inline std::array<uint8,16*0x40> plantTris{};
inline bool plantColNull{};
inline void __fastcall FakeRegisterRef(void* self,void*,CEntity** ref){ trace.push_back({"register_ref",self==plantEnt.data(),Rel(ref,ents.data(),ents.size())}); }
inline void __fastcall FakeCleanUpRef(void* self,void*,CEntity** ref){ trace.push_back({"cleanup_ref",self==plantEnt.data(),Rel(ref,ents.data(),ents.size())}); }
inline CColModel* __fastcall FakeGetColModel(void* self,void*){ trace.push_back({"get_col",self==plantEnt.data()}); return reinterpret_cast<CColModel*>(plantCol.data()); }
inline void __fastcall FakeTriRelease(void* self,void*){ trace.push_back({"tri_release",Rel(self,plantTris.data(),plantTris.size())}); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,44> fns{{
    {"CBox","Set",0x40EDE0,0,-1,512},{"CBox","Recalc",0x40EE20,0,-1,1024},
    {"CBuilding","IsBuildingPointerValid",0x4040E0,0,-1,1024},{"CBuilding","ReplaceWithNewModel",0x403EC0,0,-1,256},
    {"CCollisionPlugin","PluginAttach",0x41B310,0,-1,64},{"CCollisionPlugin","SetModelInfo",0x41B350,0,-1,64},
    {"CColTrianglePlane","GetNormal-Out",0x411610,0,-1,512},{"CColTrianglePlane","Set",0x411660,0,-1,1024},
    {"CCompressedMatrixNotAligned","CompressFromFullMatrix",0x59BAD0,0,-1,512},{"CCompressedMatrixNotAligned","DecompressIntoFullMatrix",0x59B9F0,0,-1,512},
    {"CControllerState","CheckForInput",0x53EFF0,0,-1,512},{"CControllerState","Clear",0x53EF80,0,-1,64},
    {"CData","Load",0x69F5D0,0,-1,512},{"CData","Unload",0x69F640,0,-1,128},
    {"CDraw","CalculateAspectRatio",0x6FF420,0,-1,64},{"CDraw","SetFOV",0x6FF410,0,-1,64},
    {"CDummyObject","CreateObject",0x59EAC0,0,-1,256},{"CDummyObject","UpdateFromObject",0x59EB70,0,-1,256},
    {"CEntryInfoNode","operator new",0x536DC0,0,-1,256},{"CEntryInfoNode","operator delete",0x536DD0,0,-1,256},
    {"CNodeRoute","operator new",0x41B860,0,-1,256},{"CNodeRoute","operator delete",0x41B870,0,-1,256},
    {"CObjectData","Initialise",0x5B5360,0,-1,128},{"CObjectData","SetObjectData",0x5A2D00,0,-1,512},
    {"CObjectPool","GetAt",0x404870,0,-1,512},{"CObjectPool","New",0x5A1C20,0,-1,512},
    {"CPedPool","GetAtRef",0x404910,0,-1,512},{"CPedPool","New",0x5E45E0,0,-1,256},
    {"CObjectSaveStructure","Construct",0x5D2160,0,-1,256},{"CObjectSaveStructure","Extract",0x5D2220,0,-1,256},
    {"CSimpleVariablesSaveStructure","Construct",0x5D1B80,0,-1,128},{"CSimpleVariablesSaveStructure","Extract",0x5D1EA0,0,-1,128},
    {"CVehicleSaveStructure","Construct",0x5D2330,0,-1,256},{"CVehicleSaveStructure","Extract",0x5D23B0,0,-1,256},
    {"CTempColModels","Initialise",0x5BA8C0,0,-1,16},{"CTempColModels","Shutdown",0x41B360,0,-1,16},
    {"ModelIndices","InitModelIndices",0x5BCA10,0,-1,16},{"ModelIndices","MatchAllModelStrings",0x5B57C0,0,-1,32},
    {"FxPrimBP_c","GetRWMatrix",0x4A9DC0,0,-1,512},{"FxPrimBP_c","Load",0x5C2010,0,-1,256},
    {"CWindModifiers","FindWindModifier",0x72C950,0,-1,1024},{"CWindModifiers","RegisterOne",0x72C8B0,0,-1,512},
    {"CPlantColEntEntry","AddEntry",0x5DB7D0,0,-1,256},{"CPlantColEntEntry","ReleaseEntry",0x5DB8A0,0,-1,256},
}};

// regiones globales que el banco puede tocar; se restauran tras cada par
struct Snap { uintptr_t a; size_t n; std::vector<uint8> v; };
constexpr uintptr_t OBJINFO=0xBB4A90; constexpr size_t OBJINFO_SZ=160*0x50;
constexpr uintptr_t WIND=0xC81450; constexpr size_t WIND_SZ=8+16*0x14;
// variables simples (CSimpleVariablesSaveStructure)
inline const std::pair<uintptr_t,size_t> simpleVars[]{
    {0xB78A00,8},{0xB72910,1},{0xB7290C,4},{0xB72914,4},{0xB6F99C,0x40},{0xB70144,0x1C},{0x969130,0x5D},{0xB7CB4C,0x3C},{0xC8130C,0x1C},
    {0xB6F0DC,4},{0xB6F0F0,4},{0x8CC381,1},{0xB73402,1},{0xB79E3C,0xC},{0xB7C484,4},{0xC228A0,4},{0xB72958,2},{0x8CDEE4,8},{0xB9B7EC,3},
    {0xBA6774,4},{0xC0BC15,1},{0x8A5B58+90*4,8},
};

inline void __cdecl InvalidParam(const wchar_t* e,const wchar_t* f,const wchar_t* file,unsigned line,uintptr_t){
    char b[1024]{}; snprintf(b,sizeof(b),"%ls | %ls | %ls:%u",e?e:L"",f?f:L"",file?file:L"",line);
    Write({{"event","invalid_parameter"},{"detail",std::string(b)},{"stack",ReversingProbe::SymbolStack()}}); }
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"core70-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"core70-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("core70"); _set_invalid_parameter_handler(InvalidParam); _set_thread_local_invalid_parameter_handler(InvalidParam);}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"core70-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,44> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,44> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    const uintptr_t padMode=uintptr_t(CPad::GetPad(0))+0x10A;
    std::vector<Snap> snaps;
    auto addSnap=[&](uintptr_t a,size_t n){ snaps.push_back({a,n,std::vector<uint8>(reinterpret_cast<uint8*>(a),reinterpret_cast<uint8*>(a)+n)}); };
    for(uintptr_t a:{0xB74498u,0x8E3FB0u,0xB7448Cu,0xB744B8u})addSnap(a,4);
    addSnap(0x9689DC,8); addSnap(0x8D5038,4); addSnap(0xC3EFA4,4); addSnap(0xBA6793,1); addSnap(0xB6F065,1);
    addSnap(OBJINFO,OBJINFO_SZ); for(auto& [a,n]:simpleVars)addSnap(a,n); addSnap(padMode,2);
    addSnap(TCM_BEGIN,TCM_END-TCM_BEGIN); addSnap(MI_IDX_BEGIN,MI_IDX_END-MI_IDX_BEGIN); addSnap(WIND,WIND_SZ); addSnap(0xC03998,8);
    auto restoreAll=[&]{ for(auto& s:snaps)std::memcpy(reinterpret_cast<void*>(s.a),s.v.data(),s.n); };
    auto intact=[&]{ for(auto& s:snaps)if(std::memcmp(reinterpret_cast<void*>(s.a),s.v.data(),s.n)!=0)return false; return true; };
    auto** const miPtrs=reinterpret_cast<void**>(0xA9B0C8); const uint64 miPtrsHash=Fnv(miPtrs,20000*4);

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        static std::array<void*,32> fakeVt{}; for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeVTrap); fakeVt[8]=reinterpret_cast<void*>(&FakeDeleteRw);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
                auto randF=[&]()->uint32{ switch(next()%12){case 0:return 0x7FC00000u|(next()&0x3FFFFF);case 1:return 0x80000000u;case 2:return 0;case 3:return F(uni(-1e6f,1e6f));case 4:return 0x7F800000u;default:return F(uni(-100.f,100.f));} };
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xC070u; if(!stt)stt=1; trace=json::array(); seed=uint32(i); rngCalls=0; nrec=0; arenaUsed=0;
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(cls=="CBox") {
                        alignas(16) static std::array<uint32,8> box{}; alignas(16) static std::array<uint32,6> src{};
                        for(auto& v:box)v=randF(); for(auto& v:src)v=randF();
                        if(nm=="Recalc"){ if(next()%4==0)for(int k=0;k<3;++k)if(next()%2)box[k]=box[k+3]; reinterpret_cast<void(__thiscall*)(void*)>(a)(box.data()); }
                        else { auto pick=[&](int w)->void*{ switch(next()%4){case 0:return box.data();case 1:return box.data()+3;default:return src.data()+3*w;} };
                            void* mn=next()%3 ? static_cast<void*>(src.data()) : pick(0); void* mx=next()%3 ? static_cast<void*>(src.data()+3) : pick(1);
                            reinterpret_cast<void(__thiscall*)(void*,const void*,const void*)>(a)(box.data(),mn,mx); out["src"]=std::vector<uint32>(src.begin(),src.end()); }
                        out["box"]=std::vector<uint32>(box.begin(),box.end());
                    }
                    else if(cls=="CBuilding"&&nm=="IsBuildingPointerValid") {
                        alignas(16) static std::array<uint8,0x14> pool{}; static std::array<uint8,64> map{};
                        for(auto& b:pool)b=uint8(next()); for(auto& b:map)b=uint8(next());
                        const uint32 storage=next()%4 ? 0x10000000u+(next()&0x0FFFFFF0u) : next()|0x01000000u;
                        int32 size; switch(next()%6){case 0:size=0;break;case 1:size=-int32(next()%100);break;default:size=int32(1+next()%64);}
                        *reinterpret_cast<uint32*>(pool.data())=storage; *reinterpret_cast<uint8**>(pool.data()+4)=map.data(); *reinterpret_cast<int32*>(pool.data()+8)=size;
                        uint32 p; switch(next()%8){
                            case 0:p=0;break; case 1:p=next();break; case 2:p=storage-1-next()%0x40;break; case 3:p=storage-0x38*(1+next()%4)-next()%0x38;break;
                            default:{ const int32 k=int32(next()%uint32(std::clamp(size,0,64)+3)); const int32 off=next()%3 ? 0 : int32(next()%0x38); p=storage+uint32(k*0x38+off); } }
                        *reinterpret_cast<void**>(0xB74498)=pool.data();
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(reinterpret_cast<void*>(uintptr_t(p)));
                        out["pool"]=ReversingProbe::Bytes(pool.data()+8,0xC);
                    }
                    else if(cls=="CBuilding") {
                        alignas(16) static std::array<uint8,0x40> bld{}; alignas(16) static std::array<uint8,0x40> mi{};
                        for(auto& b:bld)b=uint8(next()); for(auto& b:mi)b=uint8(next());
                        *reinterpret_cast<void**>(bld.data())=fakeVt.data(); fakeBld=bld.data();
                        const int16 idx=int16(next()%20000); *reinterpret_cast<int16*>(bld.data()+0x22)=idx;
                        *reinterpret_cast<uint16*>(mi.data()+8)=next()%2 ? 0 : uint16(next());
                        void* const savedMi=miPtrs[idx]; miPtrs[idx]=mi.data();
                        const int32 newModel=next()%3 ? int32(next()%20000) : int32(next());
                        { EntryPatch r1(bit_cast<uintptr_t>(&CStreaming::RemoveModel),FakeRemoveModel), r1e(0x4089A0,FakeRemoveModel);
                          reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(bld.data(),newModel); }
                        miPtrs[idx]=savedMi;
                        out["bld"]=ReversingProbe::Bytes(bld.data()+4,bld.size()-4); out["mi"]=ReversingProbe::Bytes(mi.data(),mi.size());
                    }
                    else if(cls=="CCollisionPlugin") {
                        auto& offset=*reinterpret_cast<int32*>(0x9689DC); auto& model=*reinterpret_cast<uint32*>(0x9689E0);
                        offset=int32(next()); model=next();
                        if(nm=="PluginAttach"){ pluginRet=next()%4 ? int32(next()%0x400) : -1; streamRet=int32(next()%3)-1; pluginCbs.fill(nullptr);
                            { EntryPatch p1(0x74BDD0,FakeRegPlugin), p2(0x74BE70,FakeRegStream); ret=int(reinterpret_cast<uint8(__cdecl*)()>(a)()); }
                            alignas(16) static std::array<uint8,0x20> obj{}, obj2{}; const int32 off=int32(next()%0x40), sz=int32(next()%0x20);
                            json cb=json::array();
                            cb.push_back(pluginCbs[0] ? Rel(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(pluginCbs[0])(obj.data(),off,sz),obj.data(),obj.size()) : json("null"));
                            cb.push_back(pluginCbs[1] ? Rel(reinterpret_cast<void*(__cdecl*)(void*,int32,int32)>(pluginCbs[1])(obj.data(),off,sz),obj.data(),obj.size()) : json("null"));
                            cb.push_back(pluginCbs[2] ? Rel(reinterpret_cast<void*(__cdecl*)(void*,const void*,int32,int32)>(pluginCbs[2])(obj.data(),obj2.data(),off,sz),obj.data(),obj.size()) : json("null"));
                            cb.push_back(pluginCbs[3]!=nullptr);
                            cb.push_back(pluginCbs[4] ? json(uintptr_t(reinterpret_cast<void*(__cdecl*)(void*,int32,const void*,int32,int32)>(pluginCbs[4])(reinterpret_cast<void*>(0x1234),int32(next()%100),obj.data(),off,sz))) : json("null"));
                            cb.push_back(pluginCbs[5] ? json(reinterpret_cast<int32(__cdecl*)(const void*,int32,int32)>(pluginCbs[5])(obj.data(),off,sz)) : json("null"));
                            out["callbacks"]=cb; }
                        else reinterpret_cast<void(__cdecl*)(void*)>(a)(reinterpret_cast<void*>(uintptr_t(next())));
                        out["offset"]=offset; out["model"]=model;
                    }
                    else if(cls=="CColTrianglePlane") {
                        alignas(16) static std::array<uint8,12> plane{}; for(auto& b:plane)b=uint8(next());
                        if(nm=="GetNormal-Out"){ if(next()%3==0)for(int k=0;k<3;++k)*reinterpret_cast<int16*>(plane.data()+2*k)=std::array<int16,6>{0,4096,-4096,1,-1,32767}[next()%6];
                            alignas(16) uint32 o[4]{0xCCCCCCCCu,0xCCCCCCCCu,0xCCCCCCCCu,0xCCCCCCCCu}; reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(plane.data(),o); ret=json::array({o[0],o[1],o[2],o[3]}); }
                        else { alignas(16) static std::array<int16,8*3> verts{}; alignas(16) static std::array<uint8,8> tri{};
                            const uint32 mode=next()%6;
                            for(auto& v:verts){ switch(mode){case 0:v=int16(next());break;case 1:v=int16(int32(next()%201)-100);break;case 2:v=int16(int32(next()%5)-2);break;default:v=int16(int32(next()%8001)-4000);} }
                            for(auto& b:tri)b=uint8(next());
                            for(int k=0;k<3;++k)*reinterpret_cast<uint16*>(tri.data()+2*k)=uint16(next()%8);
                            if(next()%8==0){ for(int k=0;k<3;++k)verts[3*7+k]=verts[3*6+k]; *reinterpret_cast<uint16*>(tri.data())=6; *reinterpret_cast<uint16*>(tri.data()+2)=7; } // degenerado
                            if(next()%8==0){ for(int k=0;k<3;++k)verts[3*5+k]=int16(verts[3*3+k]*2-verts[3*4+k]); } // colineal
                            reinterpret_cast<void(__thiscall*)(void*,const void*,const void*)>(a)(plane.data(),verts.data(),tri.data()); }
                        out["plane"]=ReversingProbe::Bytes(plane.data(),plane.size());
                    }
                    else if(cls=="CCompressedMatrixNotAligned") {
                        alignas(16) static std::array<uint8,0x18> cm{}; alignas(16) static std::array<float,0x12> mat{};
                        for(auto& b:cm)b=uint8(next());
                        for(auto& v:mat){ switch(next()%8){case 0:v=uni(-300.f,300.f);break;case 1:v=std::bit_cast<float>(randF());break;case 2:v=float(int(next()%3)-1);break;default:v=uni(-1.2f,1.2f);} }
                        if(nm=="CompressFromFullMatrix"){ reinterpret_cast<void(__thiscall*)(void*,const void*)>(a)(cm.data(),mat.data()); out["cm"]=ReversingProbe::Bytes(cm.data(),cm.size()); }
                        else { for(int k=0;k<3;++k)reinterpret_cast<float*>(cm.data())[k]=uni(-3000.f,3000.f);
                            reinterpret_cast<uint32*>(mat.data())[0x10]=0; // m_pAttachMatrix nulo
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(cm.data(),mat.data()); out["mat"]=ReversingProbe::Bytes(mat.data(),sizeof(mat)); }
                    }
                    else if(cls=="CControllerState") {
                        alignas(16) static std::array<int16,0x18> cs{};
                        for(auto& v:cs)v=next()%5 ? 0 : int16(next()); if(next()%4==0)cs.fill(0); if(next()%6==0){ cs.fill(0); cs[next()%0x18]=int16(1+next()%100); }
                        if(nm=="Clear")reinterpret_cast<void(__thiscall*)(void*)>(a)(cs.data()); else ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(cs.data());
                        out["cs"]=ReversingProbe::Bytes(cs.data(),sizeof(cs));
                    }
                    else if(cls=="CData") {
                        alignas(16) static std::array<uint8,8> obj{}; for(auto& b:obj)b=uint8(next());
                        for(auto& b:arena)b=uint8(next()&0xFF);
                        if(nm=="Load"){ readState=next(); readCalls=0; readFailMask=next()%3==0 ? next()&next() : 0;
                            const uint32 len=next()%8==0 ? 0 : next()%6==0 ? 1+next()%200 : 1+next()%32; uint32 offset=next()%2 ? 0 : next();
                            const uint32 unknown=next()%10<7 ? 0 : next()%2 ? 1 : uint32(next()&0xFF);
                            bool r; { EntryPatch n1(0x821195,FakeNewArr), d1(0x8213AE,FakeDelArr), rd(bit_cast<uintptr_t>(&CFileMgr::Read),FakeRead), rde(0x538950,FakeRead);
                                r=reinterpret_cast<bool(__thiscall*)(void*,uint32,FILESTREAM,uint32*,uint32)>(a)(obj.data(),len,reinterpret_cast<FILESTREAM>(0x1234),&offset,unknown); }
                            ret=r; out["offset"]=offset; out["data"]=ReversingProbe::Bytes(arena.data(),len); }
                        else { *reinterpret_cast<void**>(obj.data())=next()%5 ? arena.data()+16*(next()%4) : nullptr;
                            { EntryPatch d1(0x8213AE,FakeDelArr), d2(0x8214BD,FakeDelArr); reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data()); } }
                        out["obj"]=Norm(obj.data(),obj.size(),{{arena.data(),arena.size()}}); out["recs"]=Recs();
                    }
                    else if(cls=="CDraw") {
                        auto& fov=*reinterpret_cast<float*>(0x8D5038); auto& aspect=*reinterpret_cast<float*>(0xC3EFA4);
                        auto& wsMenu=*reinterpret_cast<uint8*>(0xBA6793); auto& wsCam=*reinterpret_cast<uint8*>(0xB6F065);
                        static const uint8 vals[]{0,1,0,1,2,0xFF}; wsMenu=vals[next()%6]; wsCam=vals[next()%6]; aspect=uni(0.f,3.f);
                        if(nm=="SetFOV"){ fov=uni(0.f,180.f); reinterpret_cast<void(__cdecl*)(float)>(a)(std::bit_cast<float>(randF())); }
                        else ret=F(reinterpret_cast<float(__cdecl*)()>(a)());
                        out["fov"]=F(fov); out["aspect"]=F(aspect);
                    }
                    else if(cls=="CDummyObject") {
                        alignas(16) static std::array<uint8,0x40> dummy{}; alignas(16) static std::array<uint8,0x200> obj{};
                        for(auto& b:dummy)b=uint8(next()); for(auto& b:obj)b=uint8(next()); fakeDummy=dummy.data(); fakeObj=obj.data();
                        if(nm=="CreateObject"){ fakeObjResult=next()%4 ? obj.data() : nullptr;
                            { EntryPatch c1(bit_cast<uintptr_t>(static_cast<CObject*(*)(CDummyObject*)>(&CObject::Create)),FakeObjCreate), c1e(0x5A2070,FakeObjCreate);
                              EntryPatch s1(bit_cast<uintptr_t>(&CScriptsForBrains::CheckIfNewEntityNeedsScript),FakeCheckScript), s1e(0x46FF20,FakeCheckScript);
                              ret=Rel(reinterpret_cast<void*(__thiscall*)(void*)>(a)(dummy.data()),obj.data(),obj.size()); } }
                        else {
                            alignas(16) static CMatrix mat; alignas(16) static std::array<uint8,0x20> rwObj{}; alignas(16) static std::array<uint8,0x60> frame{};
                            for(auto& b:frame)b=uint8(next()); for(auto& b:rwObj)b=uint8(next()); *reinterpret_cast<void**>(rwObj.data()+4)=frame.data();
                            auto* mf=reinterpret_cast<float*>(&mat); for(int k=0;k<16;++k)mf[k]=uni(-2.f,2.f); mat.m_pAttachMatrix=nullptr; mat.m_bOwnsAttachedMatrix=false;
                            auto* pl=reinterpret_cast<float*>(dummy.data()+4); for(int k=0;k<3;++k)pl[k]=uni(-3000.f,3000.f); pl[3]=uni(-3.2f,3.2f);
                            *reinterpret_cast<void**>(dummy.data()+0x14)=next()%2 ? &mat : nullptr; *reinterpret_cast<void**>(dummy.data()+0x18)=next()%4 ? rwObj.data() : nullptr;
                            // pool IPL falso: almacenamiento de IplDef (0x34) y mapa de bytes
                            constexpr int32 NIPL=64; alignas(16) static std::array<uint8,0x14> pool{}; alignas(16) static std::array<uint8,NIPL*0x34> defs{}; static std::array<uint8,NIPL> map{};
                            for(auto& b:pool)b=uint8(next()); for(auto& b:defs)b=uint8(next()); for(auto& b:map)b=uint8(next());
                            *reinterpret_cast<uint8**>(pool.data())=defs.data(); *reinterpret_cast<uint8**>(pool.data()+4)=map.data(); *reinterpret_cast<int32*>(pool.data()+8)=NIPL;
                            const uint8 ipl=next()%2 ? 0 : uint8(1+next()%(NIPL-1)); obj[0x2E]=ipl; if(ipl)map[ipl]&=0x7F; // ranura ocupada
                            *reinterpret_cast<void**>(0x8E3FB0)=pool.data();
                            { EntryPatch t1(bit_cast<uintptr_t>(&CEntity::AttachToRwObject),FakeAttach), t1e(0x533ED0,FakeAttach);
                              EntryPatch t2(bit_cast<uintptr_t>(&CEntity::DetachFromRwObject),FakeDetach), t2e(0x533FB0,FakeDetach);
                              reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(dummy.data(),obj.data()); }
                            out["frame"]=ReversingProbe::Bytes(frame.data(),frame.size()); }
                        out["dummy"]=Norm(dummy.data(),dummy.size(),{{obj.data(),obj.size()}}); out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size());
                    }
                    else if((cls=="CEntryInfoNode"||cls=="CNodeRoute"||cls=="CObjectPool"||cls=="CPedPool")&&(nm=="New"||nm=="operator new"||nm=="operator delete"||nm=="GetAt"||nm=="GetAtRef")) {
                        // CPool falso: +0 m_pObjects, +4 m_byteMap, +8 m_nSize, +0xC m_nFirstFree, +0x10 flags
                        const size_t stride=cls=="CEntryInfoNode" ? 0x14 : cls=="CNodeRoute" ? 0x24 : cls=="CObjectPool" ? 0x19C : 0x7C4;
                        constexpr int32 MAXP=48;
                        alignas(16) static std::array<uint8,0x14> pool{}; static std::vector<uint8> storage(MAXP*0x7C4); static std::array<uint8,MAXP> map{};
                        for(auto& b:pool)b=uint8(next());
                        const int32 size=next()%6==0 ? int32(1+next()%2) : int32(1+next()%MAXP);
                        const size_t bytes=stride*size_t(size); for(size_t k=0;k<bytes;++k)storage[k]=uint8(next());
                        for(auto& b:map)b=uint8(next());
                        const uint32 mode=next()%7;
                        for(int32 k=0;k<size;++k){ bool freeSlot;
                            switch(mode){case 0:freeSlot=false;break;case 1:freeSlot=true;break;case 2:freeSlot=false;break;case 3:freeSlot=next()%8==0;break;default:freeSlot=next()%2;}
                            if(next()%8==0)map[k]=uint8((map[k]&0x80)|0x7F);
                            map[k]=uint8(freeSlot ? map[k]|0x80 : map[k]&0x7F); }
                        if(mode==2)map[next()%size]|=0x80;
                        int32 first; switch(next()%5){case 0:first=-1;break;case 1:first=size-1;break;default:first=int32(next()%size);}
                        *reinterpret_cast<uint8**>(pool.data())=storage.data(); *reinterpret_cast<uint8**>(pool.data()+4)=map.data();
                        *reinterpret_cast<int32*>(pool.data()+8)=size; *reinterpret_cast<int32*>(pool.data()+0xC)=first;
                        auto Res=[&](void* r){ auto* p=static_cast<uint8*>(r); return p ? json::array({(p-storage.data())/intptr_t(stride),(p-storage.data())%intptr_t(stride)}) : json(nullptr); };
                        if(nm=="New") ret=Res(reinterpret_cast<void*(__thiscall*)(void*)>(a)(pool.data()));
                        else if(nm=="GetAt"){ const int32 k=int32(next()%size); ret=Res(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(pool.data(),k)); }
                        else if(nm=="GetAtRef"){ const int32 k=int32(next()%size); const int32 ref=(k<<8)|int32(next()%3==0 ? map[k] : next()%3==0 ? (map[k]^0x80) : uint8(next()));
                            ret=Res(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(pool.data(),ref)); }
                        else {
                            auto& gp=*reinterpret_cast<void**>(cls=="CEntryInfoNode" ? 0xB7448C : 0xB744B8); gp=pool.data();
                            if(nm=="operator new") ret=Res(reinterpret_cast<void*(__cdecl*)(uint32)>(a)(uint32(stride)));
                            else { const int32 k=int32(next()%size); reinterpret_cast<void(__cdecl*)(void*)>(a)(storage.data()+size_t(k)*stride); }
                        }
                        out["pool"]=ReversingProbe::Bytes(pool.data()+8,0xC); out["map"]=ReversingProbe::Bytes(map.data(),map.size()); out["storage"]=Fnv(storage.data(),bytes);
                        out["objects_ptr_ok"]=*reinterpret_cast<uint8**>(pool.data())==storage.data()&&*reinterpret_cast<uint8**>(pool.data()+4)==map.data();
                    }
                    else if(cls=="CObjectData"&&nm=="Initialise") {
                        auto* info=reinterpret_cast<uint8*>(OBJINFO); for(size_t k=0;k<OBJINFO_SZ;++k)info[k]=uint8(next());
                        for(auto& m:fakeMis)for(auto& b:m)b=uint8(next()); miSalt=next();
                        static const char* names[]{"lamppost1","bench","Barrel1","bin1","dyn_box1","cj_chair","wglasssmash","trafficcone","phonebooth1","dyn_roadbarrier_3","kmb_rock","bouy"};
                        static const char* effects[]{"none","prt_spark","explosion_small","prt_boatsplash","water_hydrant"};
                        lines.clear(); lineIdx=0; std::vector<std::string> made;
                        const int nl=int(1+next()%30);
                        for(int k=0;k<nl;++k){
                            const uint32 kind=next()%12; char b[512];
                            if(kind==0){ lines.push_back(""); continue; }
                            if(kind==1){ lines.push_back("; comentario"); continue; }
                            if(kind==2&&!made.empty()){ lines.push_back(made[next()%made.size()]); continue; }
                            const bool def=next()%3==0;
                            auto fl=[&](float lo,float hi,int dec){ char t[32]; snprintf(t,sizeof(t),"%.*f",dec,uni(lo,hi)); return std::string(t); };
                            const std::string mass=def||next()%6==0 ? "99999.0" : fl(1.f,5000.f,1), turn=next()%5==0 ? "99999.0" : fl(1.f,9000.f,1);
                            const std::string colMul=def||next()%4==0 ? "1.0" : fl(0.f,5.f,2);
                            static const int dmg[]{0,1,20,21,200,202}; const int cde=def||next()%2 ? 0 : dmg[next()%6];
                            const int spec=def ? (next()%2 ? 0 : 4) : int(next()%10); const int cam=int(next()%3); const int expl=int(next()%2); const int fxt=next()%3 ? 0 : int(next()%5)-1;
                            const std::string sub=next()%10==0 ? "0" : fl(5.f,200.f,1);
                            int n=snprintf(b,sizeof(b),"%s %s %s %s %s %s %s %s %d %d %d %d %d",names[next()%std::size(names)],mass.c_str(),turn.c_str(),fl(0.9f,1.f,3).c_str(),fl(0.f,1.f,2).c_str(),
                                           sub.c_str(),fl(0.f,1000.f,1).c_str(),colMul.c_str(),cde,spec,cam,expl,fxt);
                            uint32 extra=next()%4; if(fxt>0&&extra==0)extra=1; // con efecto, el nombre siempre esta (si no, ambos leen basura de la pila)
                            if(extra>=1)n+=snprintf(b+n,sizeof(b)-n," %s %s %s %s",fl(-2.f,2.f,2).c_str(),fl(-2.f,2.f,2).c_str(),fl(-2.f,2.f,2).c_str(),effects[next()%std::size(effects)]);
                            if(extra>=2)n+=snprintf(b+n,sizeof(b)-n," %s %s %s %s %s %d %d",fl(0.f,2.f,2).c_str(),fl(-1.f,1.f,2).c_str(),fl(-1.f,1.f,2).c_str(),fl(0.f,1.f,2).c_str(),fl(0.f,1.f,2).c_str(),int(next()%3),int(next()%300)-20);
                            lines.push_back(b); made.push_back(b);
                        }
                        if(next()%4==0)lines.insert(lines.begin()+next()%(lines.size()+1),"*");
                        { EntryPatch p1(bit_cast<uintptr_t>(&CFileMgr::SetDir),FakeSetDir), p1e(0x5387D0,FakeSetDir);
                          EntryPatch p2(bit_cast<uintptr_t>(&CFileMgr::OpenFile),FakeOpenFile), p2e(0x538900,FakeOpenFile);
                          EntryPatch p3(bit_cast<uintptr_t>(&CFileMgr::CloseFile),FakeCloseFile), p3e(0x5389D0,FakeCloseFile);
                          EntryPatch p4(bit_cast<uintptr_t>(static_cast<char*(*)(FILESTREAM)>(&CFileLoader::LoadLine)),FakeLoadLine), p4e(0x536F80,FakeLoadLine);
                          EntryPatch p5(bit_cast<uintptr_t>(static_cast<CBaseModelInfo*(*)(const char*,int32*)>(&CModelInfo::GetModelInfo)),FakeGetMI), p5e(0x4C5940,FakeGetMI);
                          EntryPatch p6(bit_cast<uintptr_t>(&FxManager_c::FindFxSystemBP),FakeFindFx), p6e(0x4A9360,FakeFindFx);
                          try { reinterpret_cast<void(__cdecl*)(const char*)>(a)("DATA\\OBJECT.DAT"); } catch(const std::exception& ex){ Write({{"event","cpp_exception"},{"candidate",candidate},{"what",std::string(ex.what())}}); throw; } }
                        out["info_hash"]=Fnv(info,OBJINFO_SZ); out["info"]=ReversingProbe::Bytes(info,0x50*16);
                        json mis=json::array(); for(auto& m:fakeMis)mis.push_back(ReversingProbe::Bytes(m.data(),m.size())); out["mis"]=mis;
                    }
                    else if(cls=="CObjectData") {
                        auto* info=reinterpret_cast<uint8*>(OBJINFO);
                        for(int e=0;e<16;++e){ auto* p=info+e*0x50; for(int k=0;k<0x50;++k)p[k]=uint8(next());
                            // sin NaN señalizadores: el EXE copia los floats con fld/fstp (los silencia) y la DLL en crudo; los datos reales nunca los tienen
                            for(int k=0;k<0x50;k+=4){ auto& w=*reinterpret_cast<uint32*>(p+k); if((w&0x7F800000u)==0x7F800000u&&(w&0x7FFFFFu)&&!(w&0x400000u))w|=0x400000u; }
                            auto* fp=reinterpret_cast<float*>(p); static const float masses[]{99999.f,99998.f,99997.99f,50.f,1500.f}; fp[0]=next()%3 ? masses[next()%5] : uni(0.f,200000.f);
                            p[0x1C]=uint8(next()%3==0 ? next() : next()%2); p[0x1D]=uint8(next()%4==0 ? next() : next()%11); }
                        alignas(16) static std::array<uint8,0x40> mi{}, col{}; alignas(16) static std::array<uint8,0x180> obj{};
                        for(auto& b:mi)b=uint8(next()); for(auto& b:col)b=uint8(next()); for(auto& b:obj)b=uint8(next());
                        *reinterpret_cast<int16*>(mi.data()+0x10)=next()%4==0 ? int16(-1) : int16(next()%16); *reinterpret_cast<void**>(mi.data()+0x14)=col.data();
                        reinterpret_cast<float*>(col.data())[2]=uni(-5.f,5.f);
                        const int32 idx=int32(next()%20000); void* const savedMi=miPtrs[idx]; miPtrs[idx]=mi.data();
                        reinterpret_cast<void(__cdecl*)(int32,void*)>(a)(idx,obj.data());
                        miPtrs[idx]=savedMi;
                        out["obj"]=Norm(obj.data(),obj.size(),{{info,OBJINFO_SZ}}); out["mi"]=ReversingProbe::Bytes(mi.data()+0x10,2);
                    }
                    else if(cls=="CObjectSaveStructure"||cls=="CVehicleSaveStructure") {
                        const bool veh=cls=="CVehicleSaveStructure";
                        alignas(16) static std::array<uint8,0x500> obj{}; alignas(16) static std::array<float,0x12> mat{}; alignas(16) static std::array<uint8,0x70> ss{}; alignas(16) static std::array<uint8,0x40> rw{};
                        for(auto& b:obj)b=uint8(next()); for(auto& b:ss)b=uint8(next()); for(auto& b:rw)b=uint8(next());
                        for(auto& v:mat){ switch(next()%8){case 0:v=uni(-300.f,300.f);break;case 1:v=std::bit_cast<float>(randF());break;default:v=uni(-1.2f,1.2f);} }
                        *reinterpret_cast<void**>(obj.data()+0x14)=mat.data();
                        if(nm=="Construct"){
                            if(veh)*reinterpret_cast<void**>(ss.data()+0x40)=next()%3 ? nullptr : rw.data();
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(ss.data(),obj.data()); }
                        else {
                            reinterpret_cast<uint32*>(mat.data())[0x10]=veh&&next()%3==0 ? uint32(uintptr_t(rw.data())) : 0;
                            if(veh)for(int k=0;k<16;++k)reinterpret_cast<float*>(ss.data())[k]=uni(-2.f,2.f); else for(int k=0;k<3;++k)reinterpret_cast<float*>(ss.data())[k]=uni(-3000.f,3000.f);
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(ss.data(),obj.data()); }
                        const std::initializer_list<Region> rs{{rw.data(),rw.size()},{mat.data(),sizeof(mat)}};
                        out["ss"]=Norm(ss.data(),ss.size(),rs); out["obj"]=Norm(obj.data(),obj.size(),rs); out["mat"]=Norm(mat.data(),sizeof(mat),rs); out["rw"]=ReversingProbe::Bytes(rw.data(),rw.size());
                    }
                    else if(cls=="CSimpleVariablesSaveStructure") {
                        for(auto& [ra,rn]:simpleVars){ auto* p=reinterpret_cast<uint8*>(ra); for(size_t k=0;k<rn;++k)p[k]=uint8(next()); }
                        // sin NaN señalizadores en los floats (el EXE los copia con fld/fstp y los silencia)
                        for(auto& [ra,rn]:simpleVars){ if(ra%4||rn%4)continue; auto* w=reinterpret_cast<uint32*>(ra); for(size_t k=0;k<rn/4;++k)if((w[k]&0x7F800000u)==0x7F800000u&&(w[k]&0x7FFFFFu)&&!(w[k]&0x400000u))w[k]|=0x400000u; }
                        *reinterpret_cast<uint16*>(padMode)=uint16(next());
                        for(uintptr_t b:{0xB72910u,0x96918Au,0x96918Bu,0x96918Cu,0xB73402u,0xC0BC15u,0xB70144u,0xB72958u,0xB72959u,0xB9B7ECu,0xB9B7EDu,0xB9B7EEu})*reinterpret_cast<uint8*>(b)=uint8(next()%2);
                        auto** cheatFns=reinterpret_cast<void**>(0x8A5B58);
                        cheatFns[90]=next()%2 ? reinterpret_cast<void*>(&FakeCheat90) : nullptr; cheatFns[91]=next()%2 ? reinterpret_cast<void*>(&FakeCheat91) : nullptr;
                        alignas(16) static std::array<uint8,0x140> ss{}; for(auto& b:ss)b=uint8(next());
                        if(nm=="Construct"){
                            static const char* keys[]{"INT1","SWEET1","RYDER2","CRASH3","ITBEG","MPACKID","XXXXXXX","LA1FIN2"};
                            auto* lmp=reinterpret_cast<char*>(0xB78A00); if(next()%3==0)lmp[0]=0; else strcpy(lmp,keys[next()%std::size(keys)]);
                            static const size_t lens[]{0,5,21,22,23,40,99};
                            for(size_t g=0;g<gxt.size();++g){ for(size_t k=0;k<lens[g];++k)gxt[g][k]=GxtChar('A'+(next()%26)); gxt[g][lens[g]]=0; }
                            gxtSalt=next(); timeSeed=next();
                            const uintptr_t glt=reinterpret_cast<uintptr_t>(&GetLocalTime), glte=*reinterpret_cast<uintptr_t*>(0x85811C);
                            { EntryPatch t1(bit_cast<uintptr_t>(&CText::Get),FakeTextGet), t1e(0x6A0050,FakeTextGet);
                              EntryPatch g1(glt,FakeGetLocalTime); std::optional<EntryPatch> g2; if(glte!=glt)g2.emplace(glte,FakeGetLocalTime);
                              reinterpret_cast<void(__thiscall*)(void*)>(a)(ss.data()); }
                            out["ss"]=ReversingProbe::Bytes(ss.data(),ss.size()); }
                        else {
                            for(uint32 o:{0x68u,0x8Du,0x90u,0xC8u,0xD0u,0xE0u,0xE1u,0xECu,0xEDu,0xEEu,0x11Cu,0x134u})ss[o]=uint8(next()%2);
                            ss[0x135]=uint8(next()%4==0 ? 2 : next()%2); ss[0x136]=uint8(next()%2);
                            auto& t=*reinterpret_cast<uint32*>(ss.data()+0x94); switch(next()%4){case 0:t=4140000000u+next()%1000;break;case 1:t=4140000000u-next()%2;break;default:t=next()%100000000;}
                            uint32 version=0xDEADBEEF;
                            reinterpret_cast<void(__thiscall*)(void*,uint32*)>(a)(ss.data(),&version);
                            json g=json::array(); for(auto& [ra,rn]:simpleVars)g.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(ra),rn));
                            g.push_back(ReversingProbe::Bytes(reinterpret_cast<void*>(padMode),2)); out["globals"]=g; out["version"]=version; }
                    }
                    else if(cls=="CTempColModels") {
                        auto* r=reinterpret_cast<uint8*>(TCM_BEGIN); const size_t n=TCM_END-TCM_BEGIN;
                        for(size_t k=0;k<n;++k)r[k]=uint8(next()); for(auto& b:arena)b=uint8(next());
                        if(nm=="Initialise"){
                            { EntryPatch m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc2), m1e(0x72F420,FakeMalloc2);
                              EntryPatch m2(bit_cast<uintptr_t>(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)),FakeMalloc1);
                              reinterpret_cast<void(__cdecl*)()>(a)(); }
                            out["models"]=Norm(r,n,{{arena.data(),arena.size()}}); out["arena"]=Norm(arena.data(),arenaUsed,{{arena.data(),arena.size()}}); out["recs"]=Recs(); }
                        else { EntryPatch c1(bit_cast<uintptr_t>(&CColModel::RemoveCollisionVolumes),FakeRemoveColVol), c1e(0x40F9E0,FakeRemoveColVol); reinterpret_cast<void(__cdecl*)()>(a)(); out["models"]=Fnv(r,n); }
                    }
                    else if(cls=="ModelIndices") {
                        auto* r=reinterpret_cast<uint8*>(MI_IDX_BEGIN); const size_t n=MI_IDX_END-MI_IDX_BEGIN; for(size_t k=0;k<n;++k)r[k]=uint8(next());
                        if(nm=="InitModelIndices")reinterpret_cast<void(__cdecl*)()>(a)();
                        else { miSalt=next(); EntryPatch g1(bit_cast<uintptr_t>(&CModelInfo::GetModelInfoUInt16),FakeGetMI16), g1e(0x4C59F0,FakeGetMI16); reinterpret_cast<void(__cdecl*)()>(a)(); }
                        out["indices"]=ReversingProbe::Bytes(r,n);
                    }
                    else if(cls=="FxPrimBP_c") {
                        alignas(16) static std::array<uint8,0x40> obj{}; alignas(16) static std::array<int16,12> buf{};
                        for(auto& b:obj)b=uint8(next()); for(auto& v:buf)v=next()%6==0 ? std::array<int16,5>{0,32767,-32767,-32768,1}[next()%5] : int16(next());
                        for(auto& b:fxMat)b=uint8(next()); fakeFxObj=obj.data();
                        if(nm=="GetRWMatrix"){ *reinterpret_cast<void**>(obj.data()+8)=next()%4 ? buf.data() : nullptr;
                            alignas(16) static std::array<uint8,0x40> m{}; for(auto& b:m)b=uint8(next());
                            reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(obj.data(),m.data()); out["m"]=ReversingProbe::Bytes(m.data(),m.size()); }
                        else {
                            static std::array<char,4*32> names{}; for(auto& c:names)c=char(next());
                            static const int32 versions[]{100,101,102,108,109}; const int32 version=versions[next()%5];
                            lines.clear(); lineIdx=0; char b[256];
                            auto fl=[&](float lo,float hi){ char t[32]; snprintf(t,sizeof(t),"%.4f",uni(lo,hi)); return std::string(t); };
                            static const char* texs[]{"cloudmasked","smoke","sphere","shad_exp","target256","waterclear256","lockonFire"};
                            lines.push_back("FX_PRIM_BASE_DATA:"); lines.push_back(std::string("NAME: prim")+std::to_string(next()%100));
                            if(next()%3==0) lines.push_back("MATRIX: 1.000 0.000 0.000 0.000 1.000 0.000 0.000 0.000 1.000 0.000 0.000 0.000");
                            else { std::string m="MATRIX:"; for(int k=0;k<12;++k){ const uint32 c=next()%6; m+=" "+(c==0 ? std::string(k%4==0 ? "1.000" : "0.000") : c==1 ? fl(-200.f,200.f) : fl(-1.f,1.f)); } lines.push_back(m); }
                            lines.push_back(std::string("TEXTURE: ")+texs[next()%std::size(texs)]);
                            if(version>101){ for(const char* fld:{"TEXTURE2: ","TEXTURE3: ","TEXTURE4: "})lines.push_back(std::string(fld)+(next()%3 ? "NULL" : texs[next()%std::size(texs)])); }
                            snprintf(b,sizeof(b),"ALPHAON: %d",next()%4 ? int(next()%2) : int(next()%600)-100); lines.push_back(b);
                            snprintf(b,sizeof(b),"SRCBLENDID: %d",int(next()%12)); lines.push_back(b);
                            snprintf(b,sizeof(b),"DSTBLENDID: %d",next()%8 ? int(next()%12) : int(next()%1000)); lines.push_back(b);
                            { EntryPatch l1(bit_cast<uintptr_t>(&CFileMgr::ReadLine),FakeReadLine), l1e(0x5389B0,FakeReadLine);
                              EntryPatch g1(bit_cast<uintptr_t>(&FxMemoryPool_c::GetMem),FakeGetMem), g1e(0x4A9CA0,FakeGetMem);
                              EntryPatch i1(0x5C0B70,FakeInfoLoad);
                              ret=reinterpret_cast<bool(__thiscall*)(void*,FILESTREAM,int32,void*)>(a)(obj.data(),reinterpret_cast<FILESTREAM>(0x5678),version,names.data()); }
                            out["names"]=ReversingProbe::Bytes(names.data(),names.size()); out["fxmat"]=ReversingProbe::Bytes(fxMat.data(),fxMat.size()); out["lines_used"]=lineIdx; }
                        out["obj"]=Norm(obj.data(),0x2C,{{buf.data(),sizeof(buf)},{fxMat.data(),fxMat.size()}});
                    }
                    else if(cls=="CWindModifiers") {
                        auto& number=*reinterpret_cast<int32*>(WIND); auto* arr=reinterpret_cast<uint8*>(WIND+8);
                        for(size_t k=0;k<16*0x14;++k)arr[k]=uint8(next());
                        if(nm=="RegisterOne"){ number=next()%6==0 ? int32(16+next()%3) : next()%10==0 ? -int32(next()%3) : int32(next()%16);
                            const CVector cam=TheCamera.GetPosition(); const float r=next()%4==0 ? uni(199.f,201.f) : uni(0.f,300.f);
                            CVector d{uni(-1.f,1.f),uni(-1.f,1.f),uni(-1.f,1.f)}; const float m=std::sqrt(d.x*d.x+d.y*d.y+d.z*d.z); if(m>0.f)d=d*(r/m);
                            const CVector pos=cam+d; const int32 act=next()%3 ? 1 : int32(next()%4)-1;
                            reinterpret_cast<void(__cdecl*)(CVector,int32,float)>(a)(pos,act,uni(-2.f,5.f)); }
                        else { number=next()%10==0 ? -int32(next()%3) : int32(next()%17);
                            const CVector pos{uni(-3000.f,3000.f),uni(-3000.f,3000.f),uni(-10.f,300.f)};
                            for(int e=0;e<16;++e){ auto* w=arr+e*0x14; auto* wf=reinterpret_cast<float*>(w);
                                wf[0]=pos.x+uni(-60.f,60.f); wf[1]=pos.y+uni(-60.f,60.f); wf[2]=pos.z+15.f+uni(-45.f,45.f);
                                if(next()%8==0){ wf[0]=pos.x+uni(-1.f,1.f)*std::array<float,3>{20.f,50.f,0.f}[next()%3]; wf[1]=pos.y; }
                                *reinterpret_cast<int32*>(w+0xC)=next()%5 ? 1 : std::array<int32,4>{0,2,-1,0x101}[next()%4]; wf[4]=uni(-1.f,6.f); }
                            float ox=uni(-5.f,5.f), oy=uni(-5.f,5.f);
                            { EntryPatch r1(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),FakeRandomNumber), r1e(0x821B1E,FakeRand);
                              ret=reinterpret_cast<bool(__cdecl*)(CVector,float*,float*)>(a)(pos,&ox,&oy); }
                            out["ox"]=F(ox); out["oy"]=F(oy); }
                        out["wind"]=ReversingProbe::Bytes(reinterpret_cast<void*>(WIND),WIND_SZ);
                    }
                    else if(cls=="CPlantColEntEntry") {
                        for(auto& b:ents)b=uint8(next()); for(auto& b:plantEnt)b=uint8(next()); for(auto& b:plantCol)b=uint8(next()); for(auto& b:plantColData)b=uint8(next());
                        for(auto& b:arena)b=uint8(next());
                        auto E=[&](int k){ return ents.data()+k*0x14; };
                        auto Next=[&](int k)->uint8*&{ return *reinterpret_cast<uint8**>(E(k)+0xC); };
                        auto Prev=[&](int k)->uint8*&{ return *reinterpret_cast<uint8**>(E(k)+0x10); };
                        std::array<int,8> perm{}; for(int k=0;k<8;++k)perm[k]=k; for(int k=7;k>0;--k)std::swap(perm[k],perm[next()%uint32(k+1)]);
                        const int nu=int(next()%6), nc=int(next()%(8-nu)); // listas libre y cercana
                        auto link=[&](int from,int cnt)->uint8*{ for(int k=0;k<cnt;++k){ Prev(perm[from+k])=k ? E(perm[from+k-1]) : nullptr; Next(perm[from+k])=k+1<cnt ? E(perm[from+k+1]) : nullptr; } return cnt ? E(perm[from]) : nullptr; };
                        auto& unusedHead=*reinterpret_cast<uint8**>(0xC03998); auto& closeHead=*reinterpret_cast<uint8**>(0xC0399C);
                        unusedHead=link(0,nu); closeHead=link(nu,nc);
                        for(int k=nu+nc;k<8;++k){ Prev(perm[k])=nullptr; Next(perm[k])=nullptr; } // fuera de las listas: sin enlaces
                        const bool add=nm=="AddEntry";
                        int self; if(add){ self=nu ? perm[next()%nu] : perm[7]; } else { self=nc ? perm[nu+next()%nc] : perm[7]; }
                        if(add){
                            *reinterpret_cast<void**>(plantCol.data()+0x2C)=next()%6 ? plantColData.data() : nullptr;
                            int16 nt; switch(next()%6){case 0:nt=0;break;case 1:nt=int16(-int(next()%3000)-1);break;default:nt=int16(1+next()%64);}
                            *reinterpret_cast<int16*>(plantColData.data()+4)=nt;
                            void* r; { EntryPatch g1(bit_cast<uintptr_t>(&CEntity::GetColModel),FakeGetColModel), g1e(0x535300,FakeGetColModel);
                              EntryPatch r1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::RegisterReference)),FakeRegisterRef), r1e(0x571B70,FakeRegisterRef);
                              EntryPatch m1(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc2), m1e(0x72F420,FakeMalloc2);
                              EntryPatch m2(bit_cast<uintptr_t>(static_cast<void*(*)(uint32)>(&CMemoryMgr::Malloc)),FakeMalloc1);
                              r=reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(E(self),plantEnt.data()); }
                            ret=Rel(r,ents.data(),ents.size()); out["arena"]=ReversingProbe::Bytes(arena.data(),64*4); }
                        else {
                            const uint32 nt=next()%4==0 ? 0 : 1+next()%16; auto** objs=reinterpret_cast<void**>(arena.data());
                            for(uint32 k=0;k<16;++k)objs[k]=next()%3 ? plantTris.data()+0x40*k : nullptr;
                            *reinterpret_cast<void**>(E(self)+4)=next()%5 ? static_cast<void*>(arena.data()) : nullptr; *reinterpret_cast<uint16*>(E(self)+8)=uint16(nt);
                            *reinterpret_cast<void**>(E(self))=next()%4 ? static_cast<void*>(plantEnt.data()) : nullptr;
                            { EntryPatch t1(bit_cast<uintptr_t>(&CPlantLocTri::Release),FakeTriRelease), t1e(0x5DB6D0,FakeTriRelease);
                              EntryPatch f1(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree), f1e(0x72F430,FakeFree);
                              EntryPatch c1(bit_cast<uintptr_t>(static_cast<void(CEntity::*)(CEntity**)>(&CEntity::CleanUpOldReference)),FakeCleanUpRef), c1e(0x571A00,FakeCleanUpRef);
                              reinterpret_cast<void(__thiscall*)(void*)>(a)(E(self)); } }
                        const std::initializer_list<Region> rs{{ents.data(),ents.size()},{arena.data(),arena.size()},{plantEnt.data(),plantEnt.size()}};
                        out["ents"]=Norm(ents.data(),ents.size(),rs); out["heads"]=Norm(reinterpret_cast<void*>(0xC03998),8,rs); out["recs"]=Recs();
                    }
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
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",intact()&&Fnv(miPtrs,20000*4)==miPtrsHash}});
}
}
