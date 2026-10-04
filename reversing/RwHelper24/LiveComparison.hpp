#pragma once
// Banco dentro del motor: ayudantes de RenderWare (lista de 24, 24 funciones de RwHelper).
// Original (hooks de RwHelper desactivados) frente a candidato sobre un mundo RW falso y determinista:
// un clump (tipo 2 en +0), 8 atomics, 8 frames (arbol con hijos/hermanos y lista de objetos), 4 geometrias
// (flags, lista de materiales, plugins de skin y 2dfx), materiales, texturas, jerarquias HAnim con interpoladores
// y 4 model infos falsos colocados temporalmente en CModelInfo::ms_modelInfoPtrs. Los offsets de los plugins se
// leen del EXE en tiempo de ejecucion. Se simulan y trazan: RpGeometryLock/Unlock (0x74C7D0/0x74C800),
// RtAnimAnimationDestroy (0x7CCF10), CTxdStore::RemoveRef (0x731A30 + simbolo DLL) y operator new (0x82119A,
// solo durante GetEventGlobalGroup y sin reservar memoria). Nunca se tocan modelos ni texturas reales.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "RwHelper.h"
#include "TxdStore.h"
#include "ModelInfo.h"
#include "Events/EventGlobalGroup.h"
#include "../ProbeRuntime.hpp"

namespace RwHelper24Live {
inline bool done{}; inline std::ofstream log; inline json trace;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

// --- registro sin reservas (operator new sustituido) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,64> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(nrec<recs.size())recs[nrec++]={t,a,b}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); return r; }

// --- mundo RW falso ---
constexpr size_t NF=8, NA=8, NG=4, NM=8, NT=8, NH=4, NO=6, NMI=4;
struct World {
    alignas(16) uint8 clump[0x40];
    alignas(16) uint8 frames[NF][0x200];
    alignas(16) uint8 atomics[NA][0x200];
    alignas(16) uint8 geoms[NG][0x200];
    alignas(16) uint8 mats[NM][0x40];
    alignas(16) uint8 texs[NT][0x60];
    alignas(16) uint8 hiers[NH][0x40];
    alignas(16) uint8 interps[NH][0x20];
    alignas(16) uint8 objs[NO][0x20];
    alignas(16) uint8 matLists[NG][4*NM];
    alignas(16) uint8 fx[NG][0x10];
    alignas(16) uint8 mi[NMI][0x40];
    alignas(16) uint8 arena[0x60];
};
inline World w;
constexpr int32 fakeIds[NMI]{19996,19997,19998,19999};
inline json Name(const void* p){ if(!p)return nullptr; const auto b=reinterpret_cast<uintptr_t>(&w), u=reinterpret_cast<uintptr_t>(p);
    if(u>=b&&u<b+sizeof(World))return json::array({"w",uint32(u-b)}); return json::array({"raw",uint32(u)}); }
template<class T=void*> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }

// --- stubs ---
inline RpGeometry* __cdecl FakeLock(RpGeometry* g,RwInt32 mode){ trace.push_back({"lock",Name(g),mode}); At<uint16>(g,0xC)|=uint16(mode); return g; }
inline RpGeometry* __cdecl FakeUnlock(RpGeometry* g){ trace.push_back({"unlock",Name(g)}); return g; }
inline RwBool __cdecl FakeAnimDestroy(RtAnimAnimation* a){ trace.push_back({"anim_destroy",Name(a)}); return TRUE; }
inline void __cdecl FakeTxdRemoveRef(int32 index){ trace.push_back({"txd_remove_ref",index}); }
inline bool newFails{};
inline void* __cdecl FakeNew(size_t n){ Push(1,uint32(n)); return newFails||n>sizeof(w.arena) ? nullptr : w.arena; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,24> fns{{
    {"RwHelper","GetEventGlobalGroup",0x4ABA50,0,-1,64},{"RwHelper","GetNameAndDamage",0x5370A0,0,-1,1024},
    {"RwHelper","GetFirstAtomicCallback",0x734810,0,-1,256},{"RwHelper","GetFirstAtomic",0x734820,0,-1,512},
    {"RwHelper","Get2DEffectAtomic",0x734880,0,-1,512},{"RwHelper","GetFirstObjectCallback",0x7348B0,0,-1,256},
    {"RwHelper","GetFirstObject",0x7348C0,0,-1,512},{"RwHelper","GetFirstFrameCallback",0x7348F0,0,-1,256},
    {"RwHelper","GetFirstChild",0x734900,0,-1,512},{"RwHelper","SkinAtomicGetHAnimHierarchCB",0x734A20,0,-1,256},
    {"RwHelper","GetAnimHierarchyFromSkinClump",0x734A40,0,-1,512},{"RwHelper","GetAnimHierarchyFromClump",0x734B10,0,-1,512},
    {"RwHelper","AtomicRemoveAnimFromSkinCB",0x734B90,0,-1,512},{"RwHelper","RpAtomicConvertGeometryToTL",0x734BE0,0,-1,512},
    {"RwHelper","RpAtomicConvertGeometryToTS",0x734C20,0,-1,512},{"RwHelper","atomicConvertGeometryToTL",0x734C60,0,-1,512},
    {"RwHelper","RpClumpConvertGeometryToTL",0x734CB0,0,-1,512},{"RwHelper","atomicConvertGeometryToTS",0x734CE0,0,-1,512},
    {"RwHelper","RpClumpConvertGeometryToTS",0x734D30,0,-1,512},{"RwHelper","forceLinearFilteringMatTexturesCB",0x734D60,0,-1,512},
    {"RwHelper","SetFilterModeOnAtomicsTextures",0x734D80,0,-1,512},{"RwHelper","forceLinearFilteringAtomicsCB",0x734DA0,0,-1,512},
    {"RwHelper","SetFilterModeOnClumpsTextures",0x734DC0,0,-1,512},{"RwHelper","RemoveRefsCB",0x7226D0,0,-1,512},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"rwhelper24-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"rwhelper24-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("rwhelper24");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"rwhelper24-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,24> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,24> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // offsets de plugins (EXE)
    const int32 visOff=*reinterpret_cast<int32*>(0x8D608C), skinAtOff=*reinterpret_cast<int32*>(0xC978A4), skinGeoOff=*reinterpret_cast<int32*>(0xC978A8),
                fxOff=*reinterpret_cast<int32*>(0xC3A1E0), hanimOff=*reinterpret_cast<int32*>(0xC9B8C8);
    auto fits=[](int32 off,int32 n){ return off>=0x48&&off+n<=0x200; };
    if(!fits(visOff,4)||!fits(skinAtOff,4)||!fits(skinGeoOff,4)||!fits(fxOff,4)||!fits(hanimOff,8)){
        for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
        Write({{"event","error"},{"reason","plugin offsets"},{"offsets",{visOff,skinAtOff,skinGeoOff,fxOff,hanimOff}}}); return; }

    // estado global guardado
    auto& globalGroup=*reinterpret_cast<CEventGlobalGroup**>(0xA9AF6C); CEventGlobalGroup* const savedGroup=globalGroup;
    auto& ptrs=CModelInfo::ms_modelInfoPtrs; std::array<CBaseModelInfo*,NMI> savedPtrs{}; for(size_t k=0;k<NMI;++k)savedPtrs[k]=ptrs[fakeIds[k]];
    auto restore=[&]{ globalGroup=savedGroup; for(size_t k=0;k<NMI;++k)ptrs[fakeIds[k]]=savedPtrs[k]; };
    auto restored=[&]{ bool ok=globalGroup==savedGroup; for(size_t k=0;k<NMI;++k)ok&=ptrs[fakeIds[k]]==savedPtrs[k]; return ok; };
    const uintptr_t dllGroupVt=uintptr_t(ReversibleHooks::detail::GetVTableAddress("CEventGlobalGroup"));

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        EntryPatch g1(0x74C7D0,FakeLock), g2(0x74C800,FakeUnlock), g3(0x7CCF10,FakeAnimDestroy);
        EntryPatch t1(bit_cast<uintptr_t>(&CTxdStore::RemoveRef),FakeTxdRemoveRef), t1e(0x731A30,FakeTxdRemoveRef);

        for(size_t f=0;f<fns.size();++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x24A5u; if(!stt)stt=1; trace=json::array(); nrec=0; newFails=false;
                    // --- construir el mundo ---
                    { auto* b=reinterpret_cast<uint8*>(&w); for(size_t k=0;k<sizeof(World);++k)b[k]=uint8(next()); }
                    // frames: arbol (padre de menor indice o suelto), plugin HAnim, lista de objetos vacia
                    std::array<int,NF> parent{}; parent[0]=-1; for(size_t k=1;k<NF;++k)parent[k]=next()%4 ? int(next()%k) : -1;
                    for(size_t k=0;k<NF;++k){ auto* fr=w.frames[k]; At(fr,0x98)=nullptr; At(fr,0x9C)=nullptr; At(fr,0x90)=fr+0x90; At(fr,0x94)=fr+0x90;
                        At(fr,hanimOff+4)=next()%4==0 ? static_cast<void*>(w.hiers[next()%NH]) : nullptr; }
                    for(size_t k=NF;k-->1;)if(parent[k]>=0){ auto* p=w.frames[parent[k]]; At(w.frames[k],0x9C)=At(p,0x98); At(p,0x98)=w.frames[k]; }
                    for(size_t k=0;k<NO;++k){ if(next()%4==0)continue; auto* fr=w.frames[next()%NF]; auto* link=w.objs[k]+8;
                        auto* last=At<uint8*>(fr,0x94); At(link,0)=fr+0x90; At(link,4)=last; At(last,0)=link; At(fr,0x94)=link; }
                    // geometrias
                    for(size_t k=0;k<NG;++k){ auto* g=w.geoms[k]; uint32 fl=next()&~0x01000001u; if(next()%2)fl|=1; if(next()%4==0)fl|=0x01000000; At<uint32>(g,8)=fl;
                        At(g,0x20)=w.matLists[k]; At<int32>(g,0x24)=next()%8==0 ? -int32(next()%3) : int32(next()%(NM+1));
                        for(size_t m=0;m<NM;++m)At(w.matLists[k],4*m)=w.mats[next()%NM];
                        At(g,skinGeoOff)=next()%3 ? reinterpret_cast<void*>(uintptr_t(0x20000000+(next()%0x1000)*0x10)) : nullptr;
                        At(g,fxOff)=next()%4 ? static_cast<void*>(w.fx[k]) : nullptr;
                        switch(next()%4){case 0:At<uint32>(w.fx[k],0)=0;break;case 1:At<uint32>(w.fx[k],0)=0x80000000u|next();break;default:At<uint32>(w.fx[k],0)=next()%3;} }
                    for(size_t k=0;k<NM;++k)At(w.mats[k],0)=next()%4 ? static_cast<void*>(w.texs[next()%NT]) : nullptr;
                    for(size_t k=0;k<NH;++k){ At(w.hiers[k],0x20)=w.interps[k]; At(w.interps[k],0)=next()%3 ? reinterpret_cast<void*>(uintptr_t(0x10000000+(next()%0x1000)*0x10)) : nullptr; }
                    for(size_t k=0;k<NMI;++k)ptrs[fakeIds[k]]=reinterpret_cast<CBaseModelInfo*>(w.mi[k]);
                    // atomics
                    for(size_t k=0;k<NA;++k){ auto* a=w.atomics[k]; a[0]=1; At(a,0x18)=w.geoms[next()%NG]; At<int16>(a,visOff)=int16(fakeIds[next()%NMI]);
                        At(a,skinAtOff)=next()%3 ? static_cast<void*>(w.hiers[next()%NH]) : nullptr; }
                    // clump: tipo 2, frame padre, lista de atomics (orden y cantidad aleatorios)
                    { auto* c=w.clump; c[0]=2; At(c,4)=w.frames[next()%NF]; uint8* prev=c+8; std::array<size_t,NA> order{}; for(size_t k=0;k<NA;++k)order[k]=k;
                        for(size_t k=NA;k>1;--k)std::swap(order[k-1],order[next()%k]); const size_t n=next()%5==0 ? 0 : 1+next()%NA;
                        for(size_t k=0;k<n;++k){ uint8* l=w.atomics[order[k]]+0x40; At(prev,0)=l; At(l,4)=prev; prev=l; } At(prev,0)=c+8; At(c,0xC)=prev; }

                    for(auto* h:hooks)h->State(candidate);
                    const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                    const uintptr_t a=fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    auto* atomic=w.atomics[next()%NA]; auto* frame=w.frames[next()%NF];

                    if(nm=="GetEventGlobalGroup"){
                        const uint32 mode=next()%8; std::memset(w.arena,0xCD,sizeof(w.arena));
                        globalGroup=mode<2 ? reinterpret_cast<CEventGlobalGroup*>(w.clump) : nullptr; newFails=mode==2;
                        CEventGlobalGroup* r; { EntryPatch n1(0x82119A,FakeNew); r=reinterpret_cast<CEventGlobalGroup*(__cdecl*)()>(a)(); }
                        ret=Name(r); out["global"]=Name(globalGroup); out["recs"]=Recs();
                        // El vptr del grupo creado se compara como tipo: se enmascara en la propia arena para que
                        // tampoco entre en el hash del mundo (vtable del EXE 0x85AAB4 frente a la de la DLL).
                        const uint32 vp=At<uint32>(w.arena,0); out["vptr"]=vp==0x85AAB4||vp==dllGroupVt ? json("CEventGlobalGroup") : json(vp);
                        if(vp==0x85AAB4||vp==dllGroupVt)At<uint32>(w.arena,0)=0x11111111u;
                        out["arena"]=ReversingProbe::Bytes(w.arena,sizeof(w.arena)); globalGroup=savedGroup;
                    }
                    else if(nm=="GetNameAndDamage"){
                        alignas(16) static std::array<char,80> nameBuf{}; alignas(16) static std::array<uint8,80> objName{}; static uint8 dmg{};
                        nameBuf.fill('x'); objName.fill(0xCD); dmg=0x77;
                        static const char alphabet[]="abcdefgh_ijkLl0dam-DAM1"; static const char* suffixes[]{"","_dam","_l0","_L0","_DAM","_l1","dam","_da","_","_l","l0","_dam_l0","_l0_dam"};
                        std::string s; const size_t len=next()%3==0 ? next()%4 : next()%30; for(size_t k=0;k<len;++k)s+=alphabet[next()%(sizeof(alphabet)-1)];
                        s+=suffixes[next()%std::size(suffixes)]; std::memcpy(nameBuf.data()+4,s.c_str(),s.size()+1);
                        reinterpret_cast<void(__cdecl*)(const char*,char*,bool&)>(a)(nameBuf.data()+4,reinterpret_cast<char*>(objName.data()),*reinterpret_cast<bool*>(&dmg));
                        ret=s; out["obj"]=ReversingProbe::Bytes(objName.data(),objName.size()); out["damage"]=dmg;
                    }
                    else if(nm=="GetFirstAtomicCallback"||nm=="GetFirstObjectCallback"||nm=="GetFirstFrameCallback"){
                        void* slot=reinterpret_cast<void*>(uintptr_t(next()));
                        void* arg=next()%4==0 ? reinterpret_cast<void*>(uintptr_t(next())) : nm=="GetFirstFrameCallback" ? static_cast<void*>(frame) : static_cast<void*>(atomic);
                        if(next()%8==0)arg=nullptr;
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(arg,&slot)); out["slot"]=Name(slot);
                    }
                    else if(nm=="SkinAtomicGetHAnimHierarchCB"){ void* slot=reinterpret_cast<void*>(uintptr_t(next()));
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomic,&slot)); out["slot"]=Name(slot); }
                    else if(nm=="GetFirstAtomic"||nm=="Get2DEffectAtomic"||nm=="GetAnimHierarchyFromSkinClump"||nm=="GetAnimHierarchyFromClump")
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*)>(a)(w.clump));
                    else if(nm=="GetFirstObject"||nm=="GetFirstChild")
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*)>(a)(frame));
                    else if(nm=="AtomicRemoveAnimFromSkinCB"||nm=="forceLinearFilteringAtomicsCB"||nm=="RemoveRefsCB")
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomic,reinterpret_cast<void*>(uintptr_t(next()))));
                    else if(nm=="RpAtomicConvertGeometryToTL"||nm=="RpAtomicConvertGeometryToTS")
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(atomic);
                    else if(nm=="atomicConvertGeometryToTL"||nm=="atomicConvertGeometryToTS"){ uint8 ok=uint8(next());
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(atomic,&ok)); out["flag"]=ok; }
                    else if(nm=="RpClumpConvertGeometryToTL"||nm=="RpClumpConvertGeometryToTS")
                        ret=reinterpret_cast<bool(__cdecl*)(void*)>(a)(w.clump);
                    else if(nm=="forceLinearFilteringMatTexturesCB")
                        ret=Name(reinterpret_cast<void*(__cdecl*)(void*,void*)>(a)(w.mats[next()%NM],reinterpret_cast<void*>(uintptr_t(next()))));
                    else if(nm=="SetFilterModeOnAtomicsTextures")
                        ret=reinterpret_cast<bool(__cdecl*)(void*,uint32)>(a)(atomic,next());
                    else if(nm=="SetFilterModeOnClumpsTextures")
                        ret=reinterpret_cast<bool(__cdecl*)(void*,uint32)>(a)(w.clump,next());

                    // estado observable del mundo
                    json flags=json::array(), filters=json::array(), anims=json::array(), refs=json::array();
                    for(auto& g:w.geoms)flags.push_back({At<uint32>(g,8),At<uint16>(g,0xC)});
                    for(auto& t:w.texs)filters.push_back(At<uint32>(t,0x50));
                    for(auto& p:w.interps)anims.push_back(Name(At(p,0)));
                    for(auto& m:w.mi)refs.push_back({At<uint16>(m,8),At<int16>(m,0xA)});
                    out["geoms"]=flags; out["textures"]=filters; out["anims"]=anims; out["refs"]=refs;
                    out["world"]=Fnv(&w,sizeof(World)); out["ret"]=ret; out["trace"]=trace;
                    restore();
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
    restore();
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored()}});
}
}
