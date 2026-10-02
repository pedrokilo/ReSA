#pragma once
// Banco dentro del motor: subconjunto de CIplStore sin carga de archivos ni borrado de entidades
// (posicion requerida, slots, nombres, cajas, arrays de entidades, peticiones/eliminaciones con el
// streaming simulado, callbacks de requerido, SetIplsRequired sobre el quadtree real, Save/Load y
// AddIplSlot). Original (hooks del lote desactivados) frente a candidato sobre el pool real de IPL,
// con banderas aleatorizadas por caso y restaurado (cabecera, almacenamiento y mapa de bytes) despues
// de cada ejecucion. CStreaming::RequestModel/RemoveModel, el bufer de guardado y operator new[] se
// simulan y se trazan. Se compara el pool, los globales, el retorno y la traza.
#include <fstream>
#include <crtdbg.h>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "IplStore.h"
#include "IplDef.h"
#include "GenericGameStorage.h"
#include "Collision/ColAccel.h"
#include "../ProbeRuntime.hpp"

namespace IplLive {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { log<<r.dump()<<'\n'; log.flush(); }
inline uint8* Pool() { return *reinterpret_cast<uint8**>(0x8E3FB0); }
inline uint8* Objects() { return *reinterpret_cast<uint8**>(Pool()); }
inline uint8* Map() { return *reinterpret_cast<uint8**>(Pool()+4); }
inline int32 Size() { return *reinterpret_cast<int32*>(Pool()+8); }
inline IplDef* Def(int i) { return (Map()[i]&0x80) ? nullptr : reinterpret_cast<IplDef*>(Objects()+i*sizeof(IplDef)); }
alignas(16) inline std::array<uint8,0x400> newBuffer{};
inline std::vector<uint8> loadStream; inline size_t loadPos{};
inline std::string Hash(const void* p,size_t n) { uint64 h=1469598103934665603ull; for(size_t k=0;k<n;++k){h^=static_cast<const uint8*>(p)[k];h*=1099511628211ull;} return std::format("{:016x}",h); }
inline json Name(uintptr_t v) {
    if(!v)return nullptr;
    if(v>=uintptr_t(Objects())&&v<uintptr_t(Objects())+Size()*sizeof(IplDef))return "def+"+std::to_string(v-uintptr_t(Objects()));
    if(v>=uintptr_t(newBuffer.data())&&v<uintptr_t(newBuffer.data())+newBuffer.size())return "new+"+std::to_string(v-uintptr_t(newBuffer.data()));
    return v;
}
inline bool __cdecl RequestModel(int32 id,int32 flags) { trace.push_back({"request_model",id,flags}); return true; }
inline void __cdecl RemoveModel(int32 id) { trace.push_back({"remove_model",id}); }
inline bool __cdecl LoadWB(void* p,int32 n) { trace.push_back({"load_work_buffer",n}); for(int32 k=0;k<n;++k)static_cast<uint8*>(p)[k]=loadStream[(loadPos++)%loadStream.size()]; return true; }
inline bool __cdecl SaveWB(void* p,int32 n) { trace.push_back({"save_work_buffer",ReversingProbe::Bytes(p,n)}); return true; }
inline void __cdecl LoadAllRequested(bool b) { trace.push_back({"load_all_requested",b}); }
inline void __fastcall MessageScreen(void*,void*,const char* key,bool a,bool b) { trace.push_back({"message_screen",std::string(key),a,b}); }
inline void __cdecl TimerSuspend() { trace.push_back({"timer_suspend"}); }
inline void __cdecl TimerResume() { trace.push_back({"timer_resume"}); }
inline bool __cdecl CacheLoading() { trace.push_back({"col_cache_loading"}); return false; }
inline void* __cdecl NewArray(size_t n) { trace.push_back({"operator_new_array",n}); return newBuffer.data(); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* name; uintptr_t address; int cases; };
inline const std::array<Fn,26> fns{{
    {"SetIplsRequired",0x404700,512},{"EnsureIplsAreInMemory",0x4053F0,512},{"RemoveAllIpls",0x405720,64},{"LoadAllRemainingIpls",0x405780,128},
    {"AddIplsNeededAtPosn",0x4045B0,64},{"ClearIplsNeededAtPosn",0x4045E0,32},{"EnableDynamicStreaming",0x404D30,128},{"FindIplSlot",0x404AC0,256},
    {"GetBoundingBox",0x404C70,64},{"GetIplEntityIndexArray",0x4047B0,64},{"GetIplName",0x404A60,64},{"GetNewIplEntityIndexArray",0x404780,64},
    {"HaveIplsLoaded",0x405600,512},{"IncludeEntity",0x404C90,512},{"SetIsInterior",0x404A90,128},{"RemoveIplAndIgnore",0x405890,128},
    {"RemoveIplWhenFarAway",0x4058D0,128},{"RequestIplAndIgnore",0x405850,128},{"RequestIpls",0x405520,512},{"RemoveRelatedIpls",0x405110,256},
    {"SetIfIplIsRequired",0x404660,512},{"SetIfIplIsRequiredReducedBB",0x404690,512},{"SetIfInteriorIplIsRequired",0x4045F0,512},
    {"Save",0x5D5420,64},{"Load",0x5D54A0,256},{"AddIplSlot",0x405AC0,128},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"ipl-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"ipl-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("ipl");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::array<ReversibleHooks::ReversibleHook::Base*,26> hooks{};
    for(size_t f=0;f<fns.size();++f){
        if(!(hooks[f]=FindHook("CIplStore",fns[f].name))){
            // los callbacks de requerido son funciones libres registradas aparte
            ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){for(auto& h:c.Items())if(h->Name()==fns[f].name)hooks[f]=h.get();});
        }
        if(!hooks[f]){Write({{"event","error"},{"reason","hook not found"},{"function",fns[f].name}});return;}
    }
    const int32 size=Size();
    const std::vector<uint8> savedHeader(Pool(),Pool()+0x14), savedObjects(Objects(),Objects()+size*sizeof(IplDef)), savedMap(Map(),Map()+size);
    struct Blob { void* p; size_t n; std::vector<uint8> saved; };
    std::vector<Blob> blobs{{&NumIplEntityIndexArrays,4},{&IplEntityIndexArrays,sizeof(IplEntityIndexArrays)},{&gbIplsNeededAtPosn,1},{&gvecIplsNeededAtPosn,sizeof(CVector)},
                            {reinterpret_cast<void*>(0x8E3EF8),4},{&CGame::currArea,sizeof(CGame::currArea)}};
    for(auto& b:blobs)b.saved.assign(static_cast<uint8*>(b.p),static_cast<uint8*>(b.p)+b.n);
    auto restorePool=[&]{ std::memcpy(Pool(),savedHeader.data(),savedHeader.size()); std::memcpy(Objects(),savedObjects.data(),savedObjects.size()); std::memcpy(Map(),savedMap.data(),savedMap.size()); };
    std::array<bool,26> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    // entidades reales para IncludeEntity
    std::vector<CEntity*> entities; for(int k=0;k<int(GetBuildingPool()->GetSize())&&entities.size()<200;++k)if(auto* b=GetBuildingPool()->GetAt(k))entities.push_back(b);
    for(int k=0;k<int(GetDummyPool()->GetSize())&&entities.size()<400;++k)if(auto* d=GetDummyPool()->GetAt(k))entities.push_back(d);
    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch d1(bit_cast<uintptr_t>(&CStreaming::RequestModel),RequestModel), d1e(0x4087E0,RequestModel);
        EntryPatch d2(bit_cast<uintptr_t>(&CStreaming::RemoveModel),RemoveModel), d2e(0x4089A0,RemoveModel);
        EntryPatch d3(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer)),LoadWB), d3e(0x5D1300,LoadWB);
        EntryPatch d4(bit_cast<uintptr_t>(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer)),SaveWB), d4e(0x5D1270,SaveWB);
        EntryPatch d5(bit_cast<uintptr_t>(&CStreaming::LoadAllRequestedModels),LoadAllRequested), d5e(0x40EA10,LoadAllRequested);
        EntryPatch d6(bit_cast<uintptr_t>(&CMenuManager::MessageScreen),MessageScreen), d6e(0x579330,MessageScreen);
        EntryPatch d7(bit_cast<uintptr_t>(&CTimer::Suspend),TimerSuspend), d7e(0x5619D0,TimerSuspend);
        EntryPatch d8(bit_cast<uintptr_t>(&CTimer::Resume),TimerResume), d8e(0x561A00,TimerResume);
        EntryPatch d9(bit_cast<uintptr_t>(&CColAccel::isCacheLoading),CacheLoading), d9e(0x5B2AC0,CacheLoading);
        const auto savedDisable=CStreaming::ms_disableStreaming;
        std::string only; if(std::ifstream of(dir/"ipl-only.txt"); of)std::getline(of,only);
        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; if(!only.empty()&&only!=fn.name)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 st{}; auto next=[&]{st^=st<<13;st^=st>>17;st^=st<<5;return st;};
                auto uni=[&](float a,float b){return a+(b-a)*float(next()%100000)/100000.f;};
                auto run=[&](bool candidate)->json {
                    st=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x1B1Bu; trace=json::array(); loadPos=0; const std::string nm0=fn.name;
                    restorePool(); for(auto& b:blobs)std::memcpy(b.p,b.saved.data(),b.n);
                    // banderas aleatorias en los IPL existentes (se restauran al acabar)
                    for(int k=0;k<size;++k)if(auto* d=Def(k)){ d->isInterior=next()%3==0; d->loaded=char(next()%2); d->loadRequested=next()%2; d->disableDynamicStreaming=next()%2; d->ignoreWhenDeleted=char(next()%2);
                        if(next()%8==0)d->staticIdx=int16(next()%6);
                        if(nm0=="LoadAllRemainingIpls"&&next()%4==0)std::swap(d->bb.left,d->bb.right); }
                    CStreaming::ms_disableStreaming=next()%6==0;
                    loadStream.assign(size+8,0); for(auto& b:loadStream)b=uint8(next()%3==0); *reinterpret_cast<int32*>(loadStream.data())=size;
                    *reinterpret_cast<int32*>(0x8E3EF8)=int32(next()%3); CGame::currArea=eAreaCodes(next()%3);
                    std::vector<int> valid; for(int k=0;k<size;++k)if(Def(k))valid.push_back(k);
                    const int slot=valid.empty() ? 0 : valid[next()%valid.size()];
                    // posiciones: cerca de la caja de algun IPL para cubrir los dos lados de los margenes
                    CVector pos{uni(-3000.f,3000.f),uni(-3000.f,3000.f),next()%4==0 ? uni(900.f,1100.f) : uni(0.f,100.f)};
                    if(next()%2&&Def(slot)){ const auto& bb=Def(slot)->bb; pos.x=uni(bb.left-250.f,bb.right+250.f); pos.y=uni(bb.top-250.f,bb.bottom+250.f); }
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const std::string nm=fn.name; json ret=nullptr;
                    if(nm=="SetIplsRequired")reinterpret_cast<void(__cdecl*)(const CVector&,int32)>(fn.address)(pos,int32(next()%4)-1);
                    else if(nm=="EnsureIplsAreInMemory")reinterpret_cast<void(__cdecl*)(const CVector&)>(fn.address)(pos);
                    else if(nm=="RemoveAllIpls"||nm=="LoadAllRemainingIpls")reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    else if(nm=="AddIplsNeededAtPosn")reinterpret_cast<void(__cdecl*)(const CVector&)>(fn.address)(pos);
                    else if(nm=="ClearIplsNeededAtPosn")reinterpret_cast<void(__cdecl*)()>(fn.address)();
                    else if(nm=="EnableDynamicStreaming")reinterpret_cast<void(__cdecl*)(int32,bool)>(fn.address)(slot,next()%2);
                    else if(nm=="FindIplSlot"){ char name[24]{}; if(next()%3&&Def(slot)){ strcpy_s(name,Def(slot)->name); if(next()%2)for(auto& c:name)c=char(toupper(c)); } else sprintf_s(name,"nope%u",next()%100);
                        ret=reinterpret_cast<int32(__cdecl*)(const char*)>(fn.address)(name); }
                    else if(nm=="GetBoundingBox"||nm=="GetIplName")ret=Name(uintptr_t(reinterpret_cast<void*(__cdecl*)(int32)>(fn.address)(slot)));
                    else if(nm=="GetIplEntityIndexArray"){ const int32 k=int32(next()%40); ret=uintptr_t(reinterpret_cast<void*(__cdecl*)(int32)>(fn.address)(k)); }
                    else if(nm=="GetNewIplEntityIndexArray"){ EntryPatch na(0x821195,NewArray); NumIplEntityIndexArrays=int32(next()%40); ret=reinterpret_cast<int32(__cdecl*)(int32)>(fn.address)(int32(1+next()%500)); }
                    else if(nm=="HaveIplsLoaded")ret=reinterpret_cast<bool(__cdecl*)(const CVector&,int32)>(fn.address)(pos,int32(next()%3)-1);
                    else if(nm=="IncludeEntity"){ if(!entities.empty()&&Def(slot)){ if(next()%3==0){ Def(slot)->firstBuilding=int16(next()%3000); Def(slot)->lastBuilding=int16(next()%3000); Def(slot)->firstDummy=int16(next()%3000); Def(slot)->lastDummy=int16(next()%3000); }
                        reinterpret_cast<void(__cdecl*)(int32,CEntity*)>(fn.address)(slot,entities[next()%entities.size()]); } }
                    else if(nm=="SetIsInterior")reinterpret_cast<void(__cdecl*)(int32,bool)>(fn.address)(slot,next()%2);
                    else if(nm=="RemoveIplAndIgnore"||nm=="RemoveIplWhenFarAway"||nm=="RequestIplAndIgnore"||nm=="RemoveRelatedIpls")reinterpret_cast<void(__cdecl*)(int32)>(fn.address)(nm=="RemoveRelatedIpls" ? int32(next()%6) : slot);
                    else if(nm=="RequestIpls")reinterpret_cast<void(__cdecl*)(const CVector&,int32)>(fn.address)(pos,int32(next()%2));
                    else if(nm.starts_with("SetIf")){ const CVector2D p2{pos.x,pos.y}; if(Def(slot))reinterpret_cast<void(__cdecl*)(const CVector2D&,IplDef*)>(fn.address)(p2,Def(slot)); }
                    else if(nm=="Save"||nm=="Load")ret=reinterpret_cast<bool(__cdecl*)()>(fn.address)();
                    else if(nm=="AddIplSlot"){ char name[24]{}; sprintf_s(name,"bench%u",next()%1000); const int32 r=reinterpret_cast<int32(__cdecl*)(const char*)>(fn.address)(name); ret=r;
                        // basura sin significado: resto del nombre tras el NUL y relleno final (el pool de la DLL rellena con 0xCD y strcpy_s con 0xFE)
                        if(r>=0&&r<size){ auto* d=Objects()+r*sizeof(IplDef); const auto len=strnlen(reinterpret_cast<char*>(d+0x10),18); std::memset(d+0x10+len,0,18-len); d[0x32]=d[0x33]=0; }
                        if(r>=0&&r<size)ret=json::array({r,ReversingProbe::Bytes(Objects()+r*sizeof(IplDef),sizeof(IplDef))}); }
                    json r=json::object(); r["ret"]=ret; r["trace"]=trace; r["switches"]=switches;
                    { json ch=json::array(); for(int k=0;k<size;++k)if(std::memcmp(Objects()+k*sizeof(IplDef),savedObjects.data()+k*sizeof(IplDef),sizeof(IplDef)))ch.push_back({k,ReversingProbe::Bytes(Objects()+k*sizeof(IplDef),sizeof(IplDef))}); r["defs"]=ch; }
                    r["map"]=Hash(Map(),size); r["header"]=ReversingProbe::Bytes(Pool(),0x14);
                    r["globals"]=json::array({NumIplEntityIndexArrays,Hash(&IplEntityIndexArrays,sizeof(IplEntityIndexArrays)),gbIplsNeededAtPosn,ReversingProbe::Bytes(&gvecIplsNeededAtPosn,sizeof(CVector)),*reinterpret_cast<int32*>(0x8E3EF8)});
                    for(auto& b:blobs)std::memcpy(b.p,b.saved.data(),b.n); CStreaming::ms_disableStreaming=savedDisable;
                    return r;
                };
                const auto original=run(false), candidate=run(true);
                restorePool();
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CIplStore::")+fn.name},{"case",i},{"equal",true},{"accepted",true},{"original",nullptr},{"candidate",nullptr}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",std::string("CIplStore::")+fn.name},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",std::string("CIplStore::")+fn.name},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    restorePool(); for(auto& b:blobs)std::memcpy(b.p,b.saved.data(),b.n);
    const bool restored=std::memcmp(Objects(),savedObjects.data(),savedObjects.size())==0&&std::memcmp(Map(),savedMap.data(),savedMap.size())==0;
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
