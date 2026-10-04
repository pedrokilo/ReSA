#pragma once
// Banco dentro del motor: CAnimBlendAssociation (20 funciones, con los 4 constructores y los 3 Init). Original (hooks
// de la clase desactivados) frente a candidato con fixtures deterministas por caso.
// La asociacion, la de origen/sincronizada, la asociacion estatica, las jerarquias, secuencias, clump (byte de tipo 2
// y puntero de plugin en el offset real 0xB5F878), CAnimBlendClumpData, frame datas, nodos y la arena de nodos son
// buferes estaticos del banco (mismas direcciones en las dos ejecuciones, punteros comparados en crudo).
// Se simulan y trazan sin reservar memoria (solo durante la llamada y solo en el hilo principal):
// CMemoryMgr::MallocAlign/FreeAlign (0x72F4C0/0x72F4F0, arena fija rellena con 0xCD), CAnimManager::AddAnimBlockRef y
// UncompressAnimation (no se toca el almacen de bloques ni la cache), CAnimBlendNode::FindKeyFrame y
// SetupKeyFrameCompressed, RpAnimBlendClumpFindBone/FindFrameFromHashKey (respuesta fija por argumento), los callbacks
// (pueden cambiar el tipo de callback) y el destructor virtual (vtable falsa: el `delete this` se traza y no libera).
// Se compara el retorno, los bytes de las asociaciones (vptr como tipo; el callback por defecto de la DLL se lee como
// 0x4D1390), el hash de la arena y la traza.
#include <fstream>
#include <new>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MemoryMgr.h"
#include "AnimManager.h"
#include "AnimBlendAssociation.h"
#include "AnimBlendNode.h"
#include "../ProbeRuntime.hpp"

namespace Anim20Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }
inline uintptr_t Body(uintptr_t p){
    for(int k=0;k<4&&*reinterpret_cast<const uint8*>(p)==0xE9;++k){ const uintptr_t t=p+5+*reinterpret_cast<const int32*>(p+1); if(t>=0x400000&&t<0x1000000)break; p=t; }
    return p; }

// --- registro sin reservas (activo mientras las dependencias estan sustituidas) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,1024> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(!Main())return; if(nrec<recs.size())recs[nrec]={t,a,b}; ++nrec; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<std::min(nrec,recs.size());++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); r.push_back(nrec); return r; }

// --- buferes de los fixtures ---
alignas(16) inline std::array<uint8,0x40> A{}, A2{};          // asociacion probada y la de origen / sincronizada
alignas(16) inline std::array<uint8,0x14> S{};                // CAnimBlendStaticAssociation
alignas(16) inline std::array<uint8,0x18> H{}, H2{};          // jerarquias
alignas(16) inline std::array<uint8,16*12> seqs{};            // secuencias
alignas(16) inline std::array<uint8,0x400> clump{};
alignas(16) inline std::array<uint8,0x20> cdata{};            // CAnimBlendClumpData
alignas(16) inline std::array<uint8,48*0x18> frameDatas{};
alignas(16) inline std::array<uint8,48*0x18> srcNodes{}, nodes{};
alignas(16) inline std::array<uint8,48*4> seqPtrs{};
alignas(64) inline std::array<uint8,0x2000> arena{}; inline size_t arenaTop{};
constexpr size_t ARENA_START=0x100, ARENA_TAIL=0x100, FRAME_BASE=2*0x18;
inline uint32 findSeed{}; inline int32 findN{};

// --- dependencias simuladas ---
inline void* __cdecl FakeMallocAlign(uint32 size,uint32 align,uint32 hint){
    if(!Main()){ // como 0x72F4C0
        auto* m=reinterpret_cast<uint8*(__cdecl*)(size_t)>(0x824257)(size+align);
        auto* r=reinterpret_cast<uint8*>(uintptr_t(m+align)&~uintptr_t(align-1)); reinterpret_cast<void**>(r)[-1]=m; return r; }
    const size_t off=(arenaTop+63)&~size_t(63);
    void* p=size<=arena.size()-ARENA_TAIL-off ? arena.data()+off : nullptr; if(p)arenaTop=off+size;
    Push(1,size,align); Push(11,hint,U(p)); return p; }
inline void __cdecl FakeFreeAlign(void* p){
    if(!Main()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(reinterpret_cast<void**>(p)[-1]); return; }
    Push(2,U(p)); }
inline void __cdecl FakeUncompress(CAnimBlendHierarchy* h){ Push(3,U(h)); }
inline void __cdecl FakeAddRef(int32 id){ Push(4,uint32(id)); }
inline bool __fastcall FakeFindKeyFrame(void* node,void*,float t){ Push(5,U(node),F(t)); return (U(node)>>3)&1; }
inline bool __fastcall FakeSetupCompressed(void* node,void*){ Push(6,U(node)); return (U(node)>>4)&1; }
inline uint8* FindAnswer(uint32 v){
    const uint32 h=(v*2654435761u)^findSeed; if((h>>7)%4==0)return nullptr;
    const int32 idx=int32((h>>11)%uint32(findN+3))-1; return frameDatas.data()+FRAME_BASE+idx*0x18; }
inline void* __cdecl FakeFindBone(void* c,int32 tag){ auto* r=FindAnswer(uint32(tag)); Push(7,U(c),uint32(tag)); Push(8,U(r)); return r; }
inline void* __cdecl FakeFindHash(void* c,uint32 key){ auto* r=FindAnswer(key^0x5A5A5A5Au); Push(9,U(c),key); Push(10,U(r)); return r; }
// Callback de animacion: con el bit 4 de data cambia el tipo de callback (el EXE lo relee en UpdateBlend).
inline void FakeCb(CAnimBlendAssociation* a,void* data){
    Push(12,U(a),U(data));
    if(U(data)&0x10)*reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(a)+0x30)=U(data)&3; }
// Destructor virtual (slot 0, con borrado): solo traza.
inline void* __fastcall FakeDtor(void* self,void*,uint32 flags){ Push(13,U(self),flags); return self; }
inline std::array<void*,4> fakeVt{};

// Parches de una sola ejecucion: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> P; std::vector<uintptr_t> seen;
    Patches(){ P.reserve(24); seen.reserve(24); }
    template<class Fk> void Add(uintptr_t at,Fk fake){ if(!at||std::ranges::find(seen,at)!=seen.end())return; seen.push_back(at); P.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fake)); }
    ~Patches(){ while(!P.empty())P.pop_back(); }
};
inline const std::array<uintptr_t,8> exePatched{0x72F4C0,0x72F4F0,0x4D41C0,0x4D3FB0,0x4D0240,0x4D0650,0x4D6400,0x4D6370};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,20> fns{{
    {"CAnimBlendAssociation","Constructor0-",0x4CE9B0,0,-1,512},{"CAnimBlendAssociation","Constructor1-clump_hier",0x4CEFC0,0,-1,512},
    {"CAnimBlendAssociation","Constructor2-blend",0x4CF020,0,-1,512},{"CAnimBlendAssociation","Constructor3-static_blend",0x4CF080,0,-1,512},
    {"CAnimBlendAssociation","Init-0",0x4CED50,0,-1,768},{"CAnimBlendAssociation","Init-1",0x4CEE40,0,-1,768},
    {"CAnimBlendAssociation","Init-2",0x4CEEC0,0,-1,768},{"CAnimBlendAssociation","AllocateAnimBlendNodeArray",0x4CE9F0,0,-1,512},
    {"CAnimBlendAssociation","FreeAnimBlendNodeArray",0x4CEA40,0,-1,128},{"CAnimBlendAssociation","ReferenceAnimBlock",0x4CEA50,0,-1,256},
    {"CAnimBlendAssociation","SetBlend",0x4CEBA0,0,-1,128},{"CAnimBlendAssociation","SetBlendTo",0x4CEB80,0,-1,256},
    {"CAnimBlendAssociation","SetCurrentTime",0x4CEA80,0,-1,1024},{"CAnimBlendAssociation","SetDeleteCallback",0x4CEBC0,0,-1,64},
    {"CAnimBlendAssociation","SetFinishCallback",0x4CEBE0,0,-1,64},{"CAnimBlendAssociation","Start",0x4CEB70,0,-1,512},
    {"CAnimBlendAssociation","SyncAnimation",0x4CEB40,0,-1,512},{"CAnimBlendAssociation","UpdateBlend",0x4D1490,0,-1,1024},
    {"CAnimBlendAssociation","UpdateTime",0x4D13D0,0,-1,1024},{"CAnimBlendAssociation","UpdateTimeStep",0x4D13A0,0,-1,512},
}};

inline uint32 dllVt{}, dllDefaultCb{}, dllDefaultCbBody{};
// Bytes de una asociacion: vptr de la DLL leido como el del EXE (tipo) y callback por defecto de la DLL como 0x4D1390.
inline std::vector<uint8> AssocBytes(const uint8* p){
    auto v=ReversingProbe::Bytes(p,0x3C); uint32 w;
    std::memcpy(&w,v.data(),4); if(w==dllVt)w=0x85C6D0; std::memcpy(v.data(),&w,4);
    std::memcpy(&w,v.data()+0x34,4); if(w==dllDefaultCb||w==dllDefaultCbBody)w=0x4D1390; std::memcpy(v.data()+0x34,&w,4);
    return v; }

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"anim20-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"anim20-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("anim20");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"anim20-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,20> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,20> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    std::array<std::array<uint8,5>,8> savedEntries{};
    for(size_t k=0;k<exePatched.size();++k)std::memcpy(savedEntries[k].data(),reinterpret_cast<void*>(exePatched[k]),5);

    using std::bit_cast;
    // vtable de la DLL: constructor por defecto de la DLL sobre un bufer (no reserva ni llama a nada).
    { alignas(16) static uint8 tmp[0x40]{}; new (tmp) CAnimBlendAssociation(); std::memcpy(&dllVt,tmp,4); }
    dllDefaultCb=bit_cast<uint32>(&CDefaultAnimCallback::DefaultAnimCB); dllDefaultCbBody=uint32(Body(dllDefaultCb));
    fakeVt[0]=reinterpret_cast<void*>(&FakeDtor);
    const uint32 clumpOffset=*reinterpret_cast<const uint32*>(0xB5F878);
    if(clumpOffset+4>clump.size()){Write({{"event","error"},{"reason","clump plugin offset out of fixture"},{"offset",clumpOffset}});return;}

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto W=[](auto& buf,size_t off,auto v){ std::memcpy(buf.data()+off,&v,sizeof(v)); };
            auto Rnd=[&](auto& buf){ for(auto& b:buf)b=uint8(next()); };
            constexpr float kQNaN=std::numeric_limits<float>::quiet_NaN();

            // Jerarquia: con secuencias (Init-0/Constructor1) o solo con tiempo total.
            auto buildHier=[&](auto& h,bool withSeqs,float total){
                Rnd(h);
                if(withSeqs){
                    Rnd(seqs); const int32 n=int32(next()%11);
                    for(int32 k=0;k<n;++k){
                        const size_t s=size_t(k)*12;
                        const uint16 flags=uint16((next()&0xFFE0)|(next()%2 ? 0x10 : 0)|(next()&0xF));
                        const int32 nf=next()%6==0 ? 0 : next()%10==0 ? -int32(1+next()%3) : int32(1+next()%8);
                        W(seqs,s+4,flags); W(seqs,s+6,uint16(nf));
                        if(next()%3==0)W(seqs,s,uint16(next()%40)); // bone tags pequenos
                    }
                    W(h,4,seqs.data()); W(h,8,uint16(next()%16==0 ? (0x8000|next()%16) : uint32(n)));
                }
                h[0xB]=uint8(next()%2 ? 0 : 1+next()%3);
                W(h,0xC,int32(next()%300)-1);
                W(h,0x10,total); };
            auto total=[&]{ return next()%8==0 ? 1.f : uni(0.05f,3.f); };
            // Clump falso: byte de tipo 2 y el plugin de animacion apuntando a cdata.
            auto buildClump=[&](int32 nf){
                Rnd(clump); clump[0]=2; W(clump,clumpOffset,cdata.data());
                Rnd(cdata); Rnd(frameDatas);
                W(cdata,8,uint32(next()%4==0 ? (next()&0xFFFF0000u) : 0)|uint32(uint16(nf)));
                W(cdata,0x10,frameDatas.data()+FRAME_BASE);
                findN=nf; findSeed=next(); };
            // Asociacion con nodos (cuenta de 16 bits, a veces negativa) y vtable falsa.
            auto buildAssoc=[&](auto& a,auto& nodeBuf,uint8* hier){
                Rnd(a); Rnd(nodeBuf);
                W(a,0,fakeVt.data());
                const int32 n=int32(next()%12);
                W(a,0xC,uint16(next()%16==0 ? (0x8000|next()%40) : uint32(n)));
                for(int32 k=0;k<n;++k)if(next()%3==0)W(nodeBuf,size_t(k)*0x18+0x10,uint32(0));
                W(a,0x10,nodeBuf.data()); W(a,0x14,hier);
                W(a,0x18,next()%8==0 ? 0.f : next()%8==0 ? 1.f : uni(-0.5f,1.5f));
                W(a,0x1C,next()%6==0 ? 0.f : uni(-4.f,4.f));
                W(a,0x20,uni(0.f,3.f)); W(a,0x24,next()%4==0 ? 1.f : uni(-1.f,3.f)); W(a,0x28,uni(-0.1f,0.5f));
                W(a,0x2E,uint16(next()));
                W(a,0x30,uint32(next()%4==0 ? next() : next()%3));
                W(a,0x34,&FakeCb); W(a,0x38,uint32(next())); };

            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA20u; if(!stt)stt=1; nrec=0; arena.fill(0xCD); arenaTop=ARENA_START;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                const uintptr_t a=fn.address;
                json out={{"switches",switches}}; json ret=nullptr;
                auto call=[&](auto&& body){
                    Patches p;
                    p.Add(0x72F4C0,FakeMallocAlign); p.Add(0x72F4F0,FakeFreeAlign);
                    p.Add(0x4D41C0,FakeUncompress); p.Add(Body(bit_cast<uintptr_t>(&CAnimManager::UncompressAnimation)),FakeUncompress);
                    p.Add(0x4D3FB0,FakeAddRef); p.Add(Body(bit_cast<uintptr_t>(&CAnimManager::AddAnimBlockRef)),FakeAddRef);
                    p.Add(0x4D0240,FakeFindKeyFrame); p.Add(Body(bit_cast<uintptr_t>(&CAnimBlendNode::FindKeyFrame)),FakeFindKeyFrame);
                    p.Add(0x4D0650,FakeSetupCompressed); p.Add(Body(bit_cast<uintptr_t>(&CAnimBlendNode::SetupKeyFrameCompressed)),FakeSetupCompressed);
                    p.Add(0x4D6400,FakeFindBone); p.Add(Body(bit_cast<uintptr_t>(static_cast<AnimBlendFrameData*(*)(RpClump*,eBoneTag32)>(&RpAnimBlendClumpFindBone))),FakeFindBone);
                    p.Add(0x4D6370,FakeFindHash); p.Add(Body(bit_cast<uintptr_t>(static_cast<AnimBlendFrameData*(*)(RpClump*,uint32)>(&RpAnimBlendClumpFindFrameFromHashKey))),FakeFindHash);
                    body(); };
                void* T=A.data(); void* T2=A2.data();
                // El bufer probado: aleatorio o 0xCD (constructores).
                auto prefill=[&]{ if(next()%2)A.fill(0xCD); else Rnd(A); };

                if(nm.rfind("Constructor",0)==0){
                    prefill(); void* r{};
                    if(nm=="Constructor0-"){ call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*)>(a)(T); }); }
                    else if(nm=="Constructor1-clump_hier"){ buildHier(H,true,total()); buildClump(int32(next()%41));
                        call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*,void*,void*)>(a)(T,clump.data(),H.data()); }); }
                    else if(nm=="Constructor2-blend"){ buildHier(H,false,total()); buildAssoc(A2,srcNodes,H.data());
                        if(next()%2)W(A2,0xC,uint16(next()%41));
                        W(A2,0x2E,uint16(next()&~(next()%2 ? 0x4000u : 0u)));
                        call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(T,T2); }); }
                    else { buildHier(H,false,total()); Rnd(S); Rnd(seqPtrs);
                        W(S,4,uint16(next()%16==0 ? (0x8000|next()%40) : next()%41)); W(S,0xA,uint16(next()&~(next()%2 ? 0x4000u : 0u)));
                        W(S,0xC,seqPtrs.data()); W(S,0x10,H.data());
                        call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*,void*)>(a)(T,S.data()); }); }
                    ret=r==T;
                }
                else if(nm=="Init-0"){ prefill(); buildHier(H,true,total()); buildClump(int32(next()%41));
                    call([&]{ reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(a)(T,clump.data(),H.data()); }); }
                else if(nm=="Init-1"){ prefill(); buildHier(H,false,total()); buildAssoc(A2,srcNodes,H.data());
                    if(next()%2)W(A2,0xC,uint16(next()%41));
                    call([&]{ reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(T,T2); }); }
                else if(nm=="Init-2"){ prefill(); buildHier(H,false,total()); Rnd(S); Rnd(seqPtrs);
                    W(S,4,uint16(next()%16==0 ? (0x8000|next()%40) : next()%41)); W(S,0xC,seqPtrs.data()); W(S,0x10,H.data());
                    call([&]{ reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(T,S.data()); }); }
                else if(nm=="AllocateAnimBlendNodeArray"){ prefill();
                    const int32 n=next()%16==0 ? -int32(next()%5) : next()%16==0 ? int32(next()%300) : int32(next()%41);
                    call([&]{ reinterpret_cast<void(__thiscall*)(void*,int32)>(a)(T,n); }); }
                else {
                    const float tt=next()%16==0 ? 0.f : total(); buildHier(H,false,tt); buildAssoc(A,nodes,H.data());
                    if(nm=="FreeAnimBlendNodeArray"){ W(A,0x10,next()%3==0 ? nullptr : arena.data()+ARENA_START+64*(next()%8));
                        call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(T); }); }
                    else if(nm=="ReferenceAnimBlock"){ call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(T); }); }
                    else if(nm=="SetBlend"||nm=="SetBlendTo"){ const float x=uni(-0.5f,1.5f), d=next()%4==0 ? 0.f : uni(-8.f,8.f);
                        call([&]{ reinterpret_cast<void(__thiscall*)(void*,float,float)>(a)(T,x,d); }); }
                    else if(nm=="SetDeleteCallback"||nm=="SetFinishCallback"){ void* cb=reinterpret_cast<void*>(uintptr_t(next())); void* d=reinterpret_cast<void*>(uintptr_t(next()));
                        call([&]{ reinterpret_cast<void(__thiscall*)(void*,void*,void*)>(a)(T,cb,d); }); }
                    else if(nm=="SetCurrentTime"||nm=="Start"){
                        // Con total 0 y ciclica el bucle no termina (tambien en el EXE): entonces tiempo < 0.
                        const bool looped=(A[0x2E]&2)!=0;
                        float t=next()%8==0 ? tt : next()%8==0 ? 0.f : next()%32==0 ? kQNaN : uni(-1.f,tt*6.f+0.5f);
                        if(tt==0.f&&looped&&!(t<0.f))t=-uni(0.01f,1.f);
                        call([&]{ reinterpret_cast<void(__thiscall*)(void*,float)>(a)(T,t); }); }
                    else if(nm=="SyncAnimation"){ buildHier(H2,false,total()); buildAssoc(A2,srcNodes,H2.data());
                        if(tt==0.f)W(H,0x10,1.f);
                        W(A2,0x20,next()%8==0 ? *reinterpret_cast<float*>(H2.data()+0x10) : uni(-0.5f,3.5f));
                        call([&]{ reinterpret_cast<void(__thiscall*)(void*,void*)>(a)(T,T2); }); }
                    else if(nm=="UpdateBlend"){
                        W(A,0x18,next()%6==0 ? 0.f : next()%6==0 ? 1.f : uni(-0.5f,1.5f));
                        W(A,0x1C,next()%6==0 ? 0.f : next()%24==0 ? kQNaN : uni(-6.f,6.f));
                        if(next()%2)W(A,0x30,uint32(1+next()%2));
                        W(A,0x38,uint32(next()&~0x10u)|(next()%3==0 ? 0x10u : 0u));
                        const float ts=next()%8==0 ? 0.f : uni(0.f,1.f); uint8 r{};
                        call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*,float)>(a)(T,ts); }); ret=r; }
                    else if(nm=="UpdateTime"){
                        const int pick=int(next()%8);
                        W(A,0x20,pick==0 ? tt : pick==1 ? kQNaN : uni(-0.2f,tt*1.2f+0.1f));
                        W(A,0x28,next()%24==0 ? kQNaN : next()%8==0 ? 0.f : uni(-0.1f,tt*0.8f+0.2f));
                        if(next()%2)W(A,0x30,uint32(1));
                        const float x=uni(-1.f,1.f), y=uni(-1.f,1.f); uint8 r{};
                        call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*,float,float)>(a)(T,x,y); }); ret=r; }
                    else { const float ts=next()%8==0 ? 0.f : uni(0.f,2.f), mult=next()%8==0 ? 1.f : uni(0.f,3.f);
                        call([&]{ reinterpret_cast<void(__thiscall*)(void*,float,float)>(a)(T,ts,mult); }); }
                }
                out["assoc"]=AssocBytes(A.data()); out["assoc2"]=AssocBytes(A2.data());
                out["arena"]=Fnv(arena.data(),arena.size()); out["nodes"]=Fnv(nodes.data(),nodes.size());
                out["ret"]=ret; out["recs"]=Recs(); return out;
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
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    bool restored=true;
    for(size_t k=0;k<exePatched.size();++k)restored&=std::memcmp(savedEntries[k].data(),reinterpret_cast<void*>(exePatched[k]),5)==0;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
