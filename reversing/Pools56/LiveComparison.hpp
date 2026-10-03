#pragma once
// Banco dentro del motor: pools y clases sueltas (lista de 56). CPool<T>::New de 14 pools, CDummy
// IsDummyPointerValid, CEntryInfoList::Flush, CPedTaskPair::Flush, CStaticShadow::Free,
// CConversationNode::ClearRecursively, CTaskTimer::IsOutOfTime, CPathNode::GetPosition, CColLine::Set y
// FxEmitterPrt_c::operator new[]. Original (hooks del lote desactivados) frente a candidato con fixtures
// deterministas. Los pools se prueban siempre con un CPool FALSO (almacenamiento, mapa de bytes, m_nSize y
// m_nFirstFree propios); nunca con los pools reales. Se simulan y trazan: el destructor virtual de CTask y
// FxMemoryPool_c::GetMem. El estado global tocado (punteros de pool 0xB744A0/0xB7448C, pEmptyBunchList
// 0xC403D8, CConversations::m_Nodes 0x969570, CTimer::m_snTimeInMilliseconds 0xB7CB84) se guarda y se
// restaura en cada ejecucion.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "FxMemoryPool.h"
#include "../ProbeRuntime.hpp"

namespace Pools56Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- stubs ---
inline void* fakeTask{};
inline void* __fastcall FakeTaskDtor(void* self,void*,int32 flags){ trace.push_back({"task_dtor",self==fakeTask,flags}); return self; }
constexpr uintptr_t FX_MAN=0xA9AE80;
inline void* __fastcall FakeGetMem(void* self,void*,int32 size,int32 align){ trace.push_back({"get_mem",intptr_t(self)-intptr_t(FX_MAN),size,align}); return reinterpret_cast<void*>(0x10000000u+uint32(size)*3u); }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,23> fns{{
    {"CBuildingPool","New",0x403FA0,0,-1,512},{"CColModelPool","New",0x40FB80,0,-1,512},{"CDummyPool","New",0x532630,0,-1,512},
    {"CEntryInfoNodePool","New",0x536D10,0,-1,512},{"CEventPool","New",0x4B5570,0,-1,512},{"CNodeRoutePool","New",0x41B710,0,-1,512},
    {"CPedAttractorPool","New",0x5EA9F0,0,-1,512},{"CPedIntelligencePool","New",0x605EC0,0,-1,512},{"CPointRoutePool","New",0x41B5B0,0,-1,512},
    {"CPtrNodeDoubleLinkPool","New",0x5522E0,0,-1,512},{"CPtrNodeSingleLinkPool","New",0x552240,0,-1,512},{"CTaskAllocatorPool","New",0x69D8E0,0,-1,512},
    {"CTaskPool","New",0x61A500,0,-1,512},{"CVehiclePool","New",0x6E2A50,0,-1,256},
    {"CDummy","IsDummyPointerValid",0x532730,0,-1,1024},{"CEntryInfoList","Flush",0x536E10,0,-1,512},{"CPedTaskPair","Flush",0x5E95B0,0,-1,256},
    {"CStaticShadow","Free",0x707670,0,-1,512},{"CConversationNode","ClearRecursively",0x43A710,0,-1,512},{"CTaskTimer","IsOutOfTime",0x420E30,0,-1,1024},
    {"CPathNode","GetPosition",0x420A10,0,-1,1024},{"CColLine","Set",0x40EF10,0,-1,1024},{"FxEmitterPrt_c","operator new[]",0x4A18B0,0,-1,256},
}};
// tamano de objeto de cada pool (stride del EXE: imul/lea/shl de cada New)
inline size_t PoolStride(const std::string& cls) {
    static const std::pair<const char*,size_t> t[]{{"CBuildingPool",0x38},{"CColModelPool",0x30},{"CDummyPool",0x38},{"CEntryInfoNodePool",0x14},{"CEventPool",0x44},
        {"CNodeRoutePool",0x24},{"CPedAttractorPool",0xC4},{"CPedIntelligencePool",0x294},{"CPointRoutePool",0x64},{"CPtrNodeDoubleLinkPool",0xC},
        {"CPtrNodeSingleLinkPool",8},{"CTaskAllocatorPool",0x20},{"CTaskPool",0x80},{"CVehiclePool",0xA18}};
    for(auto& [n,s]:t)if(cls==n)return s; return 0;
}
constexpr int32 MAX_POOL=48;

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pools56-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pools56-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pools56");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"pools56-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,23> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,23> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // estado global guardado
    auto& timeMs=*reinterpret_cast<uint32*>(0xB7CB84); const uint32 savedTime=timeMs;
    auto& dummyPool=*reinterpret_cast<void**>(0xB744A0); void* const savedDummyPool=dummyPool;
    auto& entryPool=*reinterpret_cast<void**>(0xB7448C); void* const savedEntryPool=entryPool;
    auto& emptyBunch=*reinterpret_cast<void**>(0xC403D8); void* const savedEmptyBunch=emptyBunch;
    auto* convNodes=reinterpret_cast<uint8*>(0x969570); constexpr size_t NCONV=50, CONV_SZ=0x18; const std::vector<uint8> savedConv(convNodes,convNodes+NCONV*CONV_SZ);
    auto restoreAll=[&]{ timeMs=savedTime; dummyPool=savedDummyPool; entryPool=savedEntryPool; emptyBunch=savedEmptyBunch; std::memcpy(convNodes,savedConv.data(),savedConv.size()); };

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        static std::array<void*,16> fakeVt{}; for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTaskDtor);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x5A56u; if(!stt)stt=1; trace=json::array();
                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;

                    if(nm=="New"&&cls!="CDummy") {
                        // CPool falso: +0 m_pObjects, +4 m_byteMap, +8 m_nSize, +0xC m_nFirstFree, +0x10 flags
                        const size_t stride=PoolStride(cls);
                        alignas(16) static std::array<uint8,0x14> pool{}; static std::vector<uint8> storage(MAX_POOL*0xA18); static std::array<uint8,MAX_POOL> map{};
                        for(auto& b:pool)b=uint8(next());
                        const int32 size=next()%6==0 ? int32(1+next()%2) : int32(1+next()%MAX_POOL);
                        const size_t bytes=stride*size_t(size); for(size_t k=0;k<bytes;++k)storage[k]=uint8(next());
                        for(auto& b:map)b=uint8(next());
                        const uint32 mode=next()%7;
                        for(int32 k=0;k<size;++k){ bool freeSlot;
                            switch(mode){case 0:freeSlot=false;break;case 1:freeSlot=true;break;case 2:freeSlot=false;break;case 3:freeSlot=next()%8==0;break;default:freeSlot=next()%2;}
                            if(next()%8==0)map[k]=uint8((map[k]&0x80)|0x7F); // ID al limite: el incremento de 7 bits da la vuelta
                            map[k]=uint8(freeSlot ? map[k]|0x80 : map[k]&0x7F); }
                        if(mode==2)map[next()%size]|=0x80; // un solo hueco
                        int32 first; switch(next()%5){case 0:first=-1;break;case 1:first=size-1;break;default:first=int32(next()%size);}
                        *reinterpret_cast<uint8**>(pool.data())=storage.data(); *reinterpret_cast<uint8**>(pool.data()+4)=map.data();
                        *reinterpret_cast<int32*>(pool.data()+8)=size; *reinterpret_cast<int32*>(pool.data()+0xC)=first;
                        auto* r=reinterpret_cast<uint8*(__thiscall*)(void*)>(a)(pool.data());
                        ret=r ? json::array({(r-storage.data())/intptr_t(stride),(r-storage.data())%intptr_t(stride)}) : json(nullptr);
                        out["pool"]=ReversingProbe::Bytes(pool.data()+8,0xC); out["map"]=ReversingProbe::Bytes(map.data(),map.size()); out["storage"]=Fnv(storage.data(),bytes);
                        out["objects_ptr_ok"]=*reinterpret_cast<uint8**>(pool.data())==storage.data()&&*reinterpret_cast<uint8**>(pool.data()+4)==map.data();
                    }
                    else if(cls=="CDummy") {
                        alignas(16) static std::array<uint8,0x14> pool{}; for(auto& b:pool)b=uint8(next());
                        const uint32 storage=next()%4 ? 0x10000000u+(next()&0x0FFFFFF0u) : next();
                        int32 size; switch(next()%6){case 0:size=0;break;case 1:size=int32(next());break;case 2:size=int32(0x80000000u|next());break;default:size=int32(next()%64);}
                        *reinterpret_cast<uint32*>(pool.data())=storage; *reinterpret_cast<int32*>(pool.data()+8)=size;
                        uint32 p; switch(next()%8){
                            case 0:p=0;break; case 1:p=next();break; case 2:p=storage-1-next()%0x40;break; case 3:p=storage+0x80000000u+next()%0x100;break;
                            default:{ const int32 k=int32(next()%uint32(std::clamp(size,0,64)+5))-2; int32 off; switch(next()%3){case 0:off=0;break;case 1:off=int32(next()%0x38);break;default:off=-int32(next()%0x38);}
                                p=storage+uint32(k*0x38+off); } }
                        dummyPool=pool.data();
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(reinterpret_cast<void*>(uintptr_t(p)));
                        dummyPool=savedDummyPool; out["pool"]=ReversingProbe::Bytes(pool.data(),pool.size());
                    }
                    else if(cls=="CEntryInfoList") {
                        constexpr int32 N=32, NS=0x14;
                        alignas(16) static std::array<uint8,0x14> pool{}; alignas(16) static std::array<uint8,N*NS> nodes{}; static std::array<uint8,N> map{};
                        for(auto& b:pool)b=uint8(next()); for(auto& b:nodes)b=uint8(next()); for(auto& b:map)b=uint8(next());
                        int32 first; switch(next()%4){case 0:first=-1;break;case 1:first=int32(next());break;default:first=int32(next()%(N+8))-1;}
                        *reinterpret_cast<uint8**>(pool.data())=nodes.data(); *reinterpret_cast<uint8**>(pool.data()+4)=map.data();
                        *reinterpret_cast<int32*>(pool.data()+8)=N; *reinterpret_cast<int32*>(pool.data()+0xC)=first;
                        auto Node=[&](int32 k){ return nodes.data()+k*NS; };
                        auto Prev=[&](int32 k)->uint8*&{ return *reinterpret_cast<uint8**>(Node(k)+0xC); };
                        auto Next=[&](int32 k)->uint8*&{ return *reinterpret_cast<uint8**>(Node(k)+0x10); };
                        std::array<int32,N> perm{}; for(int32 k=0;k<N;++k)perm[k]=k; for(int32 k=N-1;k>0;--k)std::swap(perm[k],perm[next()%uint32(k+1)]);
                        const int32 len=next()%8==0 ? 0 : int32(1+next()%12);
                        for(int32 k=0;k<len;++k){ Prev(perm[k])=k ? Node(perm[k-1]) : nullptr; Next(perm[k])=k+1<len ? Node(perm[k+1]) : nullptr; if(next()%3==0)map[perm[k]]&=0x7F; }
                        if(len&&next()%6==0)Prev(perm[0])=Node(perm[len+next()%uint32(N-len)]); // anterior ajeno a la lista
                        uint8* head=len ? Node(perm[0]) : nullptr;
                        entryPool=pool.data();
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(&head);
                        entryPool=savedEntryPool;
                        out["head"]=head ? json(intptr_t(head-nodes.data())) : json(nullptr); out["nodes"]=ReversingProbe::Bytes(nodes.data(),nodes.size());
                        out["map"]=ReversingProbe::Bytes(map.data(),map.size()); out["pool"]=ReversingProbe::Bytes(pool.data(),pool.size());
                    }
                    else if(cls=="CPedTaskPair") {
                        alignas(16) static std::array<uint8,0x14> obj{}; alignas(16) static std::array<uint8,0x40> task{};
                        for(auto& b:obj)b=uint8(next()); for(auto& b:task)b=uint8(next()); *reinterpret_cast<void**>(task.data())=fakeVt.data(); fakeTask=task.data();
                        *reinterpret_cast<void**>(obj.data()+4)=next()%4 ? task.data() : nullptr;
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["task"]=Fnv(task.data(),task.size());
                    }
                    else if(cls=="CStaticShadow") {
                        constexpr int32 NB=8, BS=0x68;
                        alignas(16) static std::array<uint8,0x40> obj{}; alignas(16) static std::array<uint8,NB*BS> bunches{};
                        for(auto& b:obj)b=uint8(next()); for(auto& b:bunches)b=uint8(next());
                        auto B=[&](int32 k){ return bunches.data()+k*BS; };
                        auto NextOf=[&](int32 k)->uint8*&{ return *reinterpret_cast<uint8**>(B(k)+0x54); };
                        std::array<int32,NB> perm{}; for(int32 k=0;k<NB;++k)perm[k]=k; for(int32 k=NB-1;k>0;--k)std::swap(perm[k],perm[next()%uint32(k+1)]);
                        const int32 len=next()%4==0 ? 0 : int32(1+next()%6);
                        for(int32 k=0;k<len;++k)NextOf(perm[k])=k+1<len ? B(perm[k+1]) : nullptr;
                        *reinterpret_cast<uint8**>(obj.data()+4)=len ? B(perm[0]) : nullptr;
                        switch(next()%4){case 0:emptyBunch=nullptr;break;case 1:emptyBunch=reinterpret_cast<void*>(uintptr_t(0x12340000u|(next()&0xFFFC)));break;
                                         default:emptyBunch=len<NB ? B(perm[len+next()%uint32(NB-len)]) : nullptr;}
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(obj.data());
                        auto* h=static_cast<uint8*>(emptyBunch); out["head"]=h>=bunches.data()&&h<bunches.data()+bunches.size() ? json(intptr_t(h-bunches.data())) : json(uintptr_t(h));
                        emptyBunch=savedEmptyBunch;
                        auto Rel=[&](uint8* p)->json{ return p>=bunches.data()&&p<bunches.data()+bunches.size() ? json(intptr_t(p-bunches.data())) : json(uintptr_t(p)); };
                        json links=json::array(); for(int32 k=0;k<NB;++k)links.push_back(Rel(NextOf(k)));
                        out["links"]=links; out["obj_poly"]=Rel(*reinterpret_cast<uint8**>(obj.data()+4));
                        *reinterpret_cast<uint8**>(obj.data()+4)=nullptr; for(int32 k=0;k<NB;++k)NextOf(k)=nullptr; // punteros ya comparados de forma relativa
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["bunches"]=Fnv(bunches.data(),bunches.size());
                    }
                    else if(cls=="CConversationNode") {
                        for(size_t k=0;k<NCONV*CONV_SZ;++k)convNodes[k]=uint8(next());
                        for(int32 k=0;k<int32(NCONV);++k){ auto* n=convNodes+k*CONV_SZ;
                            for(int s=0;s<2;++s){ int16 v; const uint32 r=next()%8; if(r<2||k+1>=int32(NCONV))v=-1; else if(r==2)v=int16(0x8000|next()); else v=int16(k+1+next()%uint32(std::min<int32>(int32(NCONV)-k-1,6)));
                                *reinterpret_cast<int16*>(n+8+2*s)=v; } }
                        const int32 start=next()%3 ? int32(next()%10) : int32(next()%NCONV);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(convNodes+start*CONV_SZ);
                        out["nodes"]=ReversingProbe::Bytes(convNodes,NCONV*CONV_SZ); std::memcpy(convNodes,savedConv.data(),savedConv.size());
                    }
                    else if(cls=="CTaskTimer") {
                        alignas(16) static std::array<uint8,0xC> obj{}; for(auto& b:obj)b=uint8(next());
                        timeMs=next()%3 ? next()%10000000 : next();
                        *reinterpret_cast<uint32*>(obj.data())=next()%3 ? timeMs-next()%20000 : next();
                        *reinterpret_cast<int32*>(obj.data()+4)=next()%4 ? int32(next()%20000) : int32(next());
                        obj[8]=uint8(next()%4!=0); obj[9]=uint8(next()%3==0);
                        ret=reinterpret_cast<bool(__thiscall*)(void*)>(a)(obj.data());
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); timeMs=savedTime;
                    }
                    else if(cls=="CPathNode") {
                        alignas(16) static std::array<uint8,0x1C> node{}; for(auto& b:node)b=uint8(next());
                        if(next()%4==0)for(int k=0;k<3;++k)*reinterpret_cast<int16*>(node.data()+8+2*k)=std::array<int16,5>{0,-1,1,32767,-32768}[next()%5];
                        alignas(16) uint32 o[4]{0xCCCCCCCCu,0xCCCCCCCCu,0xCCCCCCCCu,0xCCCCCCCCu};
                        auto* r=reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(node.data(),o);
                        ret=json::array({o[0],o[1],o[2],o[3]}); out["ret_is_out"]=r==o; out["node"]=ReversingProbe::Bytes(node.data(),node.size());
                    }
                    else if(cls=="CColLine") {
                        alignas(16) static std::array<uint8,0x20> obj{}; alignas(16) static std::array<uint32,6> src{};
                        for(auto& b:obj)b=uint8(next());
                        for(auto& v:src){ switch(next()%6){case 0:v=0x7F800001u;break;case 1:v=0xFFC00000u|(next()&0x3FFFFF);break;case 2:v=0x80000000u;break;default:v=next();} }
                        auto Pick=[&](int which)->void*{ switch(next()%6){case 0:return obj.data();case 1:return obj.data()+4;case 2:return obj.data()+0x10;case 3:return obj.data()+0xC;default:return src.data()+which*3;} };
                        void* s=next()%4 ? static_cast<void*>(src.data()) : Pick(0); void* e=next()%4 ? static_cast<void*>(src.data()+3) : Pick(1);
                        reinterpret_cast<void(__thiscall*)(void*,const void*,const void*)>(a)(obj.data(),s,e);
                        out["obj"]=ReversingProbe::Bytes(obj.data(),obj.size()); out["src"]=json(std::vector<uint32>(src.begin(),src.end()));
                    }
                    else if(cls=="FxEmitterPrt_c") {
                        uint32 size; switch(next()%4){case 0:size=next();break;case 1:size=0;break;default:size=next()%100000;}
                        void* r; { EntryPatch g1(bit_cast<uintptr_t>(&FxMemoryPool_c::GetMem),FakeGetMem), g1e(0x4A9CA0,FakeGetMem); r=reinterpret_cast<void*(__cdecl*)(size_t)>(a)(size); }
                        ret=uintptr_t(r);
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
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",dummyPool==savedDummyPool&&entryPool==savedEntryPool&&emptyBunch==savedEmptyBunch&&timeMs==savedTime&&std::memcmp(convNodes,savedConv.data(),savedConv.size())==0}});
}
}
