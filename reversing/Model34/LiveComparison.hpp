#pragma once
// Banco dentro del motor: CBaseModelInfo (lista de 34, 33 funciones; 9 de ellas por la vtable 0x85BB9C).
// Original (hooks del lote desactivados) frente a candidato con model infos falsos en buffers del banco.
// - La vtable falsa del objeto traza GetTimeInfo (slot 5), DeleteRwObject (slot 8) y GetRwModelType (slot 9).
// - CTxdStore::FindTxdSlot/AddTxdSlot/AddRef/RemoveRef, Get2DEffectAtomic y CColModel::operator delete se
//   sustituyen (EXE + DLL) por stubs trazados sin reservas. El destructor de CColModel corre de verdad sobre
//   un CColModel falso sin datos (m_pColData nulo: no libera nada).
// - El plugin 2DFX (inline en el EXE) se rellena de verdad en una geometría falsa en el offset 0xC3A1E0.
// - SetColModel usa un slot libre de ms_modelInfoPtrs para el modelo horario emparejado (se restaura por ejecución).
// - SetBaseModelInfoFlags se llama con EDX centinela: el EXE lo conserva y sus llamadores lo usan.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "ModelInfo.h"
#include "TxdStore.h"
#include "RwHelper.h"
#include "Collision/ColModel.h"
#include "../ProbeRuntime.hpp"

namespace Model34Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 Fnv32(const void* p,size_t n){ uint32 h=2166136261u; auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=16777619u;} return h; }

// --- registro sin reservas ---
struct Rec { uint32 tag, a, b, c; };
inline std::array<Rec,256> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0,uint32 c=0){ if(nrec<recs.size())recs[nrec++]={t,a,b,c}; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<nrec;++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b,recs[i].c}); return r; }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }

// --- stubs (EXE + DLL) ---
inline int32 findRet{}, addRet{};
inline int32 __cdecl FakeFindTxdSlot(const char* n){ Push(1,Fnv32(n,strlen(n))); return findRet; }
inline int32 __cdecl FakeAddTxdSlot(const char* n){ Push(2,Fnv32(n,strlen(n))); return addRet; }
inline void __cdecl FakeTxdAddRef(int32 i){ Push(3,uint32(i)); }
inline void __cdecl FakeTxdRemoveRef(int32 i){ Push(4,uint32(i)); }
inline RpAtomic* fxAtomicRet{};
inline RpAtomic* __cdecl FakeGet2DEffectAtomic(RpClump* c){ Push(5,U(c)); return fxAtomicRet; }
inline void __cdecl FakeColModelDelete(void* p){ Push(6,U(p)); }

// --- vtable falsa del model info ---
inline CTimeInfo* timeInfoRet{}; inline uint32 rwTypeRet{};
inline CTimeInfo* __fastcall FakeGetTimeInfo(uint8* self,void*){ Push(7,U(self),*reinterpret_cast<uint32*>(self+0x14),*reinterpret_cast<uint16*>(self+0x12)); return timeInfoRet; }
inline void __fastcall FakeDeleteRwObject(uint8* self,void*){ Push(8,U(self),*reinterpret_cast<uint32*>(self+0x14),*reinterpret_cast<uint16*>(self+0x12)); }
inline uint32 __fastcall FakeGetRwModelType(uint8* self,void*){ Push(9,U(self)); return rwTypeRet; }
inline void __fastcall FakeTrap(uint8* self,void*){ Push(99,U(self)); }

// cdecl (modelInfo, flags) con EDX centinela; devuelve EDX tras la llamada.
inline uint32 CallKeepEdx(uintptr_t fn,void* mi,uint32 flags) {
    uint32 edxOut;
    __asm {
        push flags
        push mi
        mov edx, 5EDC0DE5h
        call dword ptr [fn]
        add esp, 8
        mov edxOut, edx
    }
    return edxOut;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
constexpr size_t NF=33;
inline const std::array<Fn,NF> fns{{
    {"CBaseModelInfo","AsAtomicModelInfoPtr",0,0x85BB9C,1,32},{"CBaseModelInfo","AsDamageAtomicModelInfoPtr",0,0x85BB9C,2,32},
    {"CBaseModelInfo","AsLodAtomicModelInfoPtr",0,0x85BB9C,3,32},{"CBaseModelInfo","GetTimeInfo",0,0x85BB9C,5,32},
    {"CBaseModelInfo","Init",0,0x85BB9C,6,128},{"CBaseModelInfo","Shutdown",0,0x85BB9C,7,512},
    {"CBaseModelInfo","SetAnimFile",0,0x85BB9C,12,32},{"CBaseModelInfo","ConvertAnimFileIndex",0,0x85BB9C,13,32},
    {"CBaseModelInfo","GetAnimFileIndex",0,0x85BB9C,14,32},
    {"CBaseModelInfo","SetTexDictionary",0x4C4B40,0,-1,512},{"CBaseModelInfo","ClearTexDictionary",0x4C4B70,0,-1,64},
    {"CBaseModelInfo","AddTexDictionaryRef",0x4C4B80,0,-1,256},{"CBaseModelInfo","RemoveTexDictionaryRef",0x4C4B90,0,-1,256},
    {"CBaseModelInfo","AddRef",0x4C4BA0,0,-1,256},{"CBaseModelInfo","RemoveRef",0x4C4BB0,0,-1,256},
    {"CBaseModelInfo","SetColModel",0x4C4BC0,0,-1,1024},{"CBaseModelInfo","Init2dEffects",0x4C4C20,0,-1,64},
    {"CBaseModelInfo","DeleteCollisionModel",0x4C4C40,0,-1,256},{"CBaseModelInfo","Get2dEffect",0x4C4C70,0,-1,2048},
    {"CBaseModelInfo","Add2dEffect",0x4C4D20,0,-1,1024},
    {"CBaseModelInfo","GetIsDrawLast",0x5328C0,0,-1,512},{"CBaseModelInfo","HasBeenPreRendered",0x5328B0,0,-1,512},
    {"CBaseModelInfo","HasComplexHierarchy",0x4C4E00,0,-1,512},{"CBaseModelInfo","IsBackfaceCulled",0x5328F0,0,-1,512},
    {"CBaseModelInfo","IsLod",0x4C4A00,0,-1,512},{"CBaseModelInfo","IsRoad",0x4C4DF0,0,-1,512},
    {"CBaseModelInfo","SetHasBeenPreRendered",0x4C42F0,0,-1,256},{"CBaseModelInfo","SetIsLod",0x4C4A10,0,-1,256},
    {"CBaseModelInfo","SetOwnsColModel",0x5328D0,0,-1,256},{"CBaseModelInfo","IncreaseAlpha",0x532900,0,-1,512},
    {"CBaseModelInfo","IsBreakableStatuePart",0x59F090,0,-1,512},{"CBaseModelInfo","IsTagModel",0x49CC20,0,-1,512},
    {"CBaseModelInfo","SetBaseModelInfoFlags",0x5B3AD0,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"model34-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"model34-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("model34");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"model34-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,NF> hooks{};
    for(size_t f=0;f<NF;++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,NF> enabled{},locked{};
    for(size_t f=0;f<NF;++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // slot libre de ms_modelInfoPtrs para el modelo horario emparejado
    auto& ptrs=CModelInfo::ms_modelInfoPtrs; int32 freeIdx=-1;
    for(int32 k=CModelInfo::NUM_MODEL_INFOS-1;k>=0;--k)if(!ptrs[k]){freeIdx=k;break;}
    if(freeIdx<0){Write({{"event","error"},{"reason","no free model info slot"}});return;}
    const int32 fxOffset=*reinterpret_cast<int32*>(0xC3A1E0);
    if(fxOffset<0||fxOffset>0x1F0){Write({{"event","error"},{"reason","2dfx plugin offset"},{"offset",fxOffset}});return;}
    auto* const store=reinterpret_cast<uint8*>(CModelInfo::Get2dEffectStore());

    bool passed=true; int pair=0;
    {
        using ReversingProbe::EntryPatch; using std::bit_cast;
        // Cuerpos DLL resueltos antes de escribir cualquier parche.
        const uintptr_t dFind=bit_cast<uintptr_t>(static_cast<int32(*)(const char*)>(&CTxdStore::FindTxdSlot));
        const uintptr_t dAdd=bit_cast<uintptr_t>(&CTxdStore::AddTxdSlot);
        const uintptr_t dRef=bit_cast<uintptr_t>(&CTxdStore::AddRef);
        const uintptr_t dUnref=bit_cast<uintptr_t>(&CTxdStore::RemoveRef);
        const uintptr_t dFxAtomic=bit_cast<uintptr_t>(&Get2DEffectAtomic);
        const uintptr_t dColDel=bit_cast<uintptr_t>(static_cast<void(*)(void*)>(&CColModel::operator delete));
        EntryPatch t1(dFind,FakeFindTxdSlot), t1e(0x731850,FakeFindTxdSlot);
        EntryPatch t2(dAdd,FakeAddTxdSlot), t2e(0x731C80,FakeAddTxdSlot);
        EntryPatch t3(dRef,FakeTxdAddRef), t3e(0x731A00,FakeTxdAddRef);
        EntryPatch t4(dUnref,FakeTxdRemoveRef), t4e(0x731A30,FakeTxdRemoveRef);
        EntryPatch g1(dFxAtomic,FakeGet2DEffectAtomic), g1e(0x734880,FakeGet2DEffectAtomic);
        EntryPatch c1(dColDel,FakeColModelDelete), c1e(0x40FC40,FakeColModelDelete);

        static std::array<void*,16> fakeVt{}; for(auto& v:fakeVt)v=reinterpret_cast<void*>(&FakeTrap);
        fakeVt[5]=reinterpret_cast<void*>(&FakeGetTimeInfo); fakeVt[8]=reinterpret_cast<void*>(&FakeDeleteRwObject); fakeVt[9]=reinterpret_cast<void*>(&FakeGetRwModelType);
        alignas(16) static std::array<uint8,0x40> objA{}, objB{}, col{}, atomic{}, clump{};
        alignas(16) static std::array<uint8,0x200> geom{}; alignas(16) static std::array<uint8,4+0x40*8> plg{};
        alignas(16) static std::array<uint8,4> timeInfo{};
        static const char* const txdNames[]{"generic","vehicle","LAEstate","cj_ext","particle","gangbanger"};

        for(size_t f=0;f<NF;++f) {
            const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
            if(!only.empty()&&only!=full)continue;
            for(int i=0;i<fn.cases;++i) {
                uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
                auto run=[&](bool candidate)->json {
                    stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x34B1u; if(!stt)stt=1; nrec=0;
                    findRet=addRet=0; fxAtomicRet=nullptr; timeInfoRet=nullptr; rwTypeRet=0;
                    for(auto& b:objA)b=uint8(next()); for(auto& b:objB)b=uint8(next());
                    *reinterpret_cast<void**>(objA.data())=fakeVt.data(); *reinterpret_cast<void**>(objB.data())=fakeVt.data();
                    void* const savedSlot=ptrs[freeIdx];
                    for(auto* h:hooks)h->State(candidate);
                    const uintptr_t vtTarget=fn.slot>=0 ? reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot] : 0;
                    const bool switches=fn.slot>=0 ? ((vtTarget>=0x400000&&vtTarget<0x1000000)!=candidate) : (ReversingProbe::JumpsToDll(fn.address)==candidate);
                    const uintptr_t a=fn.slot>=0 ? vtTarget : fn.address;
                    json out={{"switches",switches}}; json ret=nullptr;
                    uint8* const A=objA.data();
                    auto& flags=*reinterpret_cast<uint16*>(A+0x12);
                    auto setCol=[&]{ for(auto& b:col)b=uint8(next()); *reinterpret_cast<void**>(col.data()+0x2C)=nullptr; /* sin datos: el destructor no libera */
                                     *reinterpret_cast<void**>(A+0x14)=next()%4 ? col.data() : nullptr; };

                    if(nm=="AsAtomicModelInfoPtr"||nm=="AsDamageAtomicModelInfoPtr"||nm=="AsLodAtomicModelInfoPtr"||nm=="GetTimeInfo")
                        ret=U(reinterpret_cast<void*(__thiscall*)(void*)>(a)(A));
                    else if(nm=="GetAnimFileIndex")ret=reinterpret_cast<int32(__thiscall*)(void*)>(a)(A);
                    else if(nm=="ConvertAnimFileIndex"||nm=="Init"||nm=="ClearTexDictionary"||nm=="Init2dEffects"||nm=="IncreaseAlpha"
                            ||nm=="AddTexDictionaryRef"||nm=="RemoveTexDictionaryRef"||nm=="AddRef"||nm=="RemoveRef"){
                        if(nm=="IncreaseAlpha")A[0x0C]=i<256 ? uint8(i) : uint8(next());
                        if(nm.find("Ref")!=std::string::npos&&next()%4==0)*reinterpret_cast<int16*>(A+0x0A)=int16(int32(next()%3)-1);
                        reinterpret_cast<void(__thiscall*)(void*)>(a)(A); }
                    else if(nm=="SetAnimFile")reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(A,txdNames[next()%6]);
                    else if(nm=="Shutdown"||nm=="DeleteCollisionModel"){ setCol(); reinterpret_cast<void(__thiscall*)(void*)>(a)(A); out["col"]=ReversingProbe::Bytes(col.data(),col.size()); }
                    else if(nm=="SetTexDictionary"){
                        switch(next()%8){case 0:case 1:findRet=-1;break;case 2:findRet=0xFFFF;break;case 3:findRet=int32(0x10000*(1+next()%4)+0xFFFF);break;
                                         case 4:findRet=int32(next());break;default:findRet=int32(next()%5000);}
                        addRet=next()%4 ? int32(next()%5000) : int32(next());
                        reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(A,txdNames[next()%6]); }
                    else if(nm=="SetColModel"){
                        for(auto& b:timeInfo)b=uint8(next());
                        auto& other=*reinterpret_cast<int16*>(timeInfo.data()+2);
                        other=next()%3==0 ? int16(-1) : int16(freeIdx);
                        timeInfoRet=next()%3==0 ? nullptr : reinterpret_cast<CTimeInfo*>(timeInfo.data());
                        ptrs[freeIdx]=reinterpret_cast<CBaseModelInfo*>(objB.data());
                        void* const cm=next()%4==0 ? nullptr : next()%2 ? col.data() : reinterpret_cast<void*>(uintptr_t(next()));
                        uint32 lod=next(); switch(next()%4){case 0:lod&=0xFFFFFF00u;break;case 1:lod=(lod&0xFFFFFF00u)|1u;break;case 2:lod=next()%2;break;default:break;}
                        reinterpret_cast<void(__thiscall*)(void*,void*,uint32)>(a)(A,cm,lod);
                        out["b"]=ReversingProbe::Bytes(objB.data(),0x20); ptrs[freeIdx]=static_cast<CBaseModelInfo*>(savedSlot); }
                    else if(nm=="Get2dEffect"){
                        for(auto& b:geom)b=uint8(next()); for(auto& b:plg)b=uint8(next());
                        for(auto& b:atomic)b=uint8(next()); for(auto& b:clump)b=uint8(next());
                        atomic[0]=1; clump[0]=2; *reinterpret_cast<void**>(atomic.data()+0x18)=geom.data();
                        const bool hasPlugin=next()%5!=0;
                        *reinterpret_cast<void**>(geom.data()+fxOffset)=hasPlugin ? plg.data() : nullptr;
                        const uint32 pc=next()%6==0 ? next() : next()%9; *reinterpret_cast<uint32*>(plg.data())=pc;
                        static const uint32 types[]{1,1,1,2,2,2,0,3};
                        rwTypeRet=types[next()%8];
                        const uint32 rwPick=next()%5;
                        void* const rw=rwPick==0 ? nullptr : rwTypeRet==1 ? (void*)atomic.data() : rwTypeRet==2 ? (void*)clump.data() : (rwPick%2 ? (void*)atomic.data() : (void*)clump.data());
                        *reinterpret_cast<void**>(A+0x1C)=rw;
                        fxAtomicRet=next()%4 ? reinterpret_cast<RpAtomic*>(atomic.data()) : nullptr;
                        const bool hasGeom=rw&&(rwTypeRet==1||(rwTypeRet==2&&fxAtomicRet));
                        const int32 stored=int32(A[0x0D])-(hasGeom&&hasPlugin ? int32(pc) : 0);
                        int32 index;
                        if(hasGeom&&hasPlugin)index=next()%3==0 ? stored-1-int32(next()%40) : stored+int32(next()%12);
                        else index=stored-1-int32(next()%300);
                        if(next()%16==0&&!(hasGeom&&hasPlugin))index=int32(next()|0x80000000u); // índice negativo grande (camino del store)
                        ret=U(reinterpret_cast<void*(__thiscall*)(void*,int32)>(a)(A,index));
                        out["index"]=index; out["store"]=U(store); out["plg"]=U(plg.data()); }
                    else if(nm=="Add2dEffect"){
                        if(next()%3==0)*reinterpret_cast<int16*>(A+0x0E)=int16(-int32(1+next()%3));
                        const int32 d=next()%4 ? int32(4+0x40*(next()%100)) : int32(next()%0x4000)-0x1000;
                        reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(A,store+d); }
                    else if(nm=="SetHasBeenPreRendered"||nm=="SetIsLod"||nm=="SetOwnsColModel"){
                        uint32 v=next(); switch(next()%4){case 0:v&=0xFFFFFF00u;break;case 1:v=0;break;case 2:v=1;break;default:break;}
                        reinterpret_cast<void(__thiscall*)(void*,uint32)>(a)(A,v); }
                    else if(nm=="SetBaseModelInfoFlags"){
                        uint32 v=next(); if(next()%2)v&=0x003FFFFFu;
                        ret=CallKeepEdx(a,A,v); }
                    else { // consultas: EAX completo
                        if(next()%4==0)flags=uint16(i%2 ? 0x5800 : 0x3000)|uint16(next()&0x87FF);
                        ret=reinterpret_cast<uint32(__thiscall*)(void*)>(a)(A); }

                    out["obj"]=ReversingProbe::Bytes(A,0x20); out["ret"]=ret; out["recs"]=Recs();
                    out["slot_restored"]=ptrs[freeIdx]==savedSlot;
                    return out;
                };
                const auto original=run(false), candidate=run(true);
                const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true&&original["slot_restored"]==true;
                passed&=ok; failures+=!ok;
                if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]},{"events",original["recs"].size()}});
                else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                    Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                           {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
            }
            Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
        }
    }
    for(size_t f=0;f<NF;++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",ptrs[freeIdx]==nullptr}});
}
}
