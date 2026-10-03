#pragma once
// Banco dentro del motor: CAnimBlendHierarchy (16 funciones) y CAnimBlendNode (16 funciones). Original (hooks del
// lote desactivados) frente a candidato con fixtures deterministas por caso.
// Las jerarquias, secuencias, keyframes (comprimidos o no), nodos y asociaciones son buferes estaticos del banco
// (mismas direcciones en las dos ejecuciones, asi que los punteros se comparan en crudo).
// Se simulan y trazan sin reservar memoria (solo durante la llamada y solo en el hilo principal): CMemoryMgr::Malloc
// (arena fija rellena con 0xCD), CMemoryMgr::Free, CMemoryMgr::MoveMemory, operator delete/delete[],
// CAnimManager::RemoveFromUncompressedCache y CAnimManager::UncompressAnimation. Las secuencias usan el codigo
// real de CAnimBlendSequence (ya validado) y CQuaternion::Slerp llama al del EXE.
// Se compara el retorno, los bytes de la jerarquia/secuencias/nodo, el hash de keyframes y arena y la traza.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "MemoryMgr.h"
#include "AnimManager.h"
#include "AnimBlendHierarchy.h"
#include "AnimBlendNode.h"
#include "../ProbeRuntime.hpp"
#undef MoveMemory

namespace Anim16Live {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline DWORD mainTid{};
inline bool Main(){ return GetCurrentThreadId()==mainTid; }

// --- registro sin reservas (activo mientras Malloc/Free/delete estan sustituidos) ---
struct Rec { uint32 tag, a, b; };
inline std::array<Rec,2048> recs{}; inline size_t nrec{};
inline void Push(uint32 t,uint32 a=0,uint32 b=0){ if(!Main())return; if(nrec<recs.size())recs[nrec]={t,a,b}; ++nrec; }
inline json Recs(){ json r=json::array(); for(size_t i=0;i<std::min(nrec,recs.size());++i)r.push_back({recs[i].tag,recs[i].a,recs[i].b}); r.push_back(nrec); return r; }
inline uint32 U(const void* p){ return uint32(uintptr_t(p)); }

// --- buferes de los fixtures ---
alignas(16) inline std::array<uint8,0x18> hier{};
alignas(16) inline std::array<uint8,4+8*12> seqMem{};      // cookie de new[] + hasta 8 CAnimBlendSequence
alignas(16) inline std::array<uint8,0x1000> frames{};      // keyframes (con holgura antes y despues)
alignas(16) inline std::array<uint8,0x2000> heap{}; inline size_t heapTop{};
alignas(16) inline std::array<uint8,0x18> node{};
alignas(16) inline std::array<uint8,0x40> assoc{};
alignas(16) inline std::array<uint8,0x10> nodeSeq{};
constexpr size_t FRAME_BASE=0x40;

// --- dependencias simuladas ---
inline void* __cdecl FakeMalloc(uint32 n,uint32 h){
    if(!Main())return reinterpret_cast<void*(__cdecl*)(size_t)>(0x824257)(n);
    void* p=n<=heap.size()-heapTop ? heap.data()+heapTop : nullptr; if(p)heapTop+=(size_t(n)+15)&~size_t(15);
    Push(1,n,h); Push(11,U(p)); return p; }
inline void __cdecl FakeFree(void* p){
    if(!Main()){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    Push(2,U(p)); }
// Solo se registran los delete de los fixtures (el array de secuencias con su cookie, o algo dentro de los
// buferes del banco). El resto (p. ej. los EntryPatch y vectores de Patches liberados mientras 0x8214BD sigue
// parcheado) son memoria real del heap: se reenvian al free del EXE (0x8214BD es jmp 0x82413F).
inline bool InFixtures(const void* p){
    const auto u=uintptr_t(p);
    auto in=[&](const auto& b){ return u>=uintptr_t(b.data())&&u<uintptr_t(b.data())+b.size(); };
    return in(seqMem)||in(frames)||in(heap)||in(hier)||in(node)||in(assoc)||in(nodeSeq);
}
inline void __cdecl FakeDelete(void* p){
    if(!Main()||!InFixtures(p)){ reinterpret_cast<void(__cdecl*)(void*)>(0x82413F)(p); return; }
    Push(3,U(p)); }
inline void __cdecl FakeRemoveCache(CAnimBlendHierarchy* h){ Push(4,U(h)); }
inline void __cdecl FakeUncompressAnim(CAnimBlendHierarchy* h){ Push(5,U(h)); }
inline uint32 moveSeed{};
inline void* __cdecl FakeMove(void* p){
    if(!Main())return p;
    // Respuesta fija para cada puntero dentro del caso (las llamadas repetidas responden igual).
    const uint32 h=(U(p)*2654435761u)^moveSeed; void* r=(h>>8)%3==0 ? p : static_cast<uint8*>(p)+int32((h>>12)%17)*16-128;
    Push(6,U(p),U(r)); return r; }

// Parches de una sola ejecucion: sin direcciones repetidas y deshechos en orden inverso.
struct Patches {
    std::vector<std::unique_ptr<ReversingProbe::EntryPatch>> P; std::vector<uintptr_t> seen;
    Patches(){ P.reserve(16); seen.reserve(16); } // sin realojar (ni liberar) mientras se instalan
    template<class F> void Add(uintptr_t at,F fake){ if(!at||std::ranges::find(seen,at)!=seen.end())return; seen.push_back(at); P.push_back(std::make_unique<ReversingProbe::EntryPatch>(at,fake)); }
    ~Patches(){ while(!P.empty())P.pop_back(); }
};
inline const std::array<uintptr_t,7> exePatched{0x72F420,0x72F430,0x72F500,0x8214BD,0x8213AE,0x4D42A0,0x4D41C0};

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,32> fns{{
    {"CAnimBlendHierarchy","Constructor",0x4CF270,0,-1,64},{"CAnimBlendHierarchy","Destructor",0x4CF970,0,-1,256},
    {"CAnimBlendHierarchy","Shutdown",0x4CF980,0,-1,256},{"CAnimBlendHierarchy","RemoveAnimSequences",0x4CF8E0,0,-1,256},
    {"CAnimBlendHierarchy","AllocSequenceBlock",0x4CF510,0,-1,256},{"CAnimBlendHierarchy","FindSequence",0x4CF290,0,-1,512},
    {"CAnimBlendHierarchy","SetName",0x4CF2D0,0,-1,128},{"CAnimBlendHierarchy","CalcTotalTime",0x4CF2F0,0,-1,512},
    {"CAnimBlendHierarchy","CalcTotalTimeCompressed",0x4CF3E0,0,-1,512},{"CAnimBlendHierarchy","RemoveQuaternionFlips",0x4CF4E0,0,-1,256},
    {"CAnimBlendHierarchy","GetSequenceBlock",0x4CF560,0,-1,128},{"CAnimBlendHierarchy","Uncompress",0x4CF5F0,0,-1,512},
    {"CAnimBlendHierarchy","CompressKeyframes",0x4CF6C0,0,-1,512},{"CAnimBlendHierarchy","RemoveUncompressedData",0x4CF760,0,-1,512},
    {"CAnimBlendHierarchy","MoveMemory",0x4CF800,0,-1,256},{"CAnimBlendHierarchy","Print",0x4CF8A0,0,-1,64},
    {"CAnimBlendNode","Init",0x4CFB70,0,-1,128},{"CAnimBlendNode","NextKeyFrameNoCalc",0x4CFB90,0,-1,512},
    {"CAnimBlendNode","NextKeyFrame",0x4D04A0,0,-1,512},{"CAnimBlendNode","NextKeyFrameCompressed",0x4D0570,0,-1,512},
    {"CAnimBlendNode","CalcTheta",0x4D00E0,0,-1,1024},{"CAnimBlendNode","CalcDeltas",0x4D0190,0,-1,512},
    {"CAnimBlendNode","CalcDeltasCompressed",0x4D0350,0,-1,512},{"CAnimBlendNode","FindKeyFrame",0x4D0240,0,-1,1024},
    {"CAnimBlendNode","SetupKeyFrameCompressed",0x4D0650,0,-1,256},{"CAnimBlendNode","UpdateTime",0x4D0160,0,-1,512},
    {"CAnimBlendNode","GetCurrentTranslation",0x4CFC50,0,-1,512},{"CAnimBlendNode","GetCurrentTranslationCompressed",0x4CFE60,0,-1,512},
    {"CAnimBlendNode","GetEndTranslation",0x4CFD90,0,-1,256},{"CAnimBlendNode","GetEndTranslationCompressed",0x4D0000,0,-1,256},
    {"CAnimBlendNode","Update",0x4D06C0,0,-1,1024},{"CAnimBlendNode","UpdateCompressed",0x4D08D0,0,-1,1024},
}};

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"anim16-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"anim16-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("anim16");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    mainTid=GetCurrentThreadId();
    std::string only; { std::ifstream f(dir/"anim16-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,32> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,32> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}
    std::array<std::array<uint8,5>,7> savedEntries{};
    for(size_t k=0;k<exePatched.size();++k)std::memcpy(savedEntries[k].data(),reinterpret_cast<void*>(exePatched[k]),5);

    static const char* names[]{"Root","Pelvis","Spine","Spine1","Neck","Head","L Brow","R Brow","Jaw","Bip01 R Clavicle","R UpperArm","R Forearm",
                               "R Hand","R Finger","R Finger01","Bip01 L Clavicle","L UpperArm","L Forearm","L Hand","L Finger","L Finger01","L Thigh",
                               "L Calf","L Foot","L Toe0","R Thigh","R Calf","R Foot","R Toe0","Belly","R Breast","L Breast","","root","PELVIS","walk_civi"};

    bool passed=true; int pair=0;
    using std::bit_cast;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            uint32 stt{}; auto next=[&]{stt^=stt<<13;stt^=stt>>17;stt^=stt<<5;return stt;};
            auto uni=[&](float a,float b){return a+(b-a)*float(next()%1000000)/1000000.f;};
            auto At=[](void* base,size_t off)->uint8*{ return static_cast<uint8*>(base)+off; };
            // Quaternion normalizado (a veces repetido) en floats.
            float lastQ[4]{0.f,0.f,0.f,1.f};
            auto quat=[&](float* q){
                if(next()%6==0){ std::memcpy(q,lastQ,16); return; }
                for(int k=0;k<4;++k)q[k]=uni(-1.f,1.f);
                const float len=std::sqrt(q[0]*q[0]+q[1]*q[1]+q[2]*q[2]+q[3]*q[3]);
                if(len<0.001f){ q[0]=q[1]=q[2]=0.f; q[3]=1.f; } else for(int k=0;k<4;++k)q[k]/=len;
                if(next()%5==0)for(int k=0;k<4;++k)q[k]=-q[k];
                std::memcpy(lastQ,q,16); };
            // Un keyframe en el formato pedido; positive fuerza tiempo > 0.
            auto frame=[&](uint8* p,bool c,bool trans,bool positive){
                float q[4]; quat(q);
                if(!c){ auto* v=reinterpret_cast<float*>(p); std::memcpy(v,q,16);
                    v[4]=positive ? uni(0.01f,0.3f) : (next()%6==0 ? 0.f : uni(0.f,0.4f));
                    if(trans){ v[5]=uni(-4.f,4.f); v[6]=uni(-4.f,4.f); v[7]=uni(-4.f,4.f); } }
                else { auto* v=reinterpret_cast<int16*>(p); for(int k=0;k<4;++k)v[k]=int16(int32(q[k]*4096.f));
                    v[4]=positive ? int16(1+next()%20) : int16(next()%6==0 ? 0 : next()%30);
                    if(trans){ v[5]=int16(int32(next()%8193)-4096); v[6]=int16(int32(next()%8193)-4096); v[7]=int16(int32(next()%8193)-4096); } } };
            auto stride=[](bool c,bool trans){ return c ? (trans ? 16 : 10) : (trans ? 32 : 20); };

            // Jerarquia con nseq secuencias sobre seqMem (cookie = numero de elementos) y keyframes en frames.
            auto buildHier=[&](bool c,bool allowZero,bool allowNull){
                for(auto& b:frames)b=uint8(next()); for(auto& b:seqMem)b=uint8(next()); for(auto& b:hier)b=uint8(next());
                int32 nseq=int32(1+next()%6); if(allowZero&&next()%12==0)nseq=0;
                const bool shared=next()%2; uint8* fr=frames.data()+FRAME_BASE;
                for(int32 k=0;k<nseq;++k){
                    uint8* s=seqMem.data()+4+k*12;
                    const bool trans=next()%2;
                    const uint16 flags=uint16((next()%4==0 ? (next()&0xFFE0) : 0)|(next()%8 ? 1 : 0)|(trans ? 2 : 0)|(c ? 4 : 0)|((shared||next()%5==0) ? 8 : 0)|(next()%4==0 ? 0x10 : 0));
                    int32 nf=int32(1+next()%10); if(allowZero&&next()%8==0)nf=0;
                    *reinterpret_cast<uint32*>(s)=next(); *reinterpret_cast<uint16*>(s+4)=flags; *reinterpret_cast<uint16*>(s+6)=uint16(nf); *reinterpret_cast<uint8**>(s+8)=fr;
                    if(next()%10==0&&!c)*reinterpret_cast<uint8**>(s+8)=nf ? fr : nullptr;
                    for(int32 j=0;j<nf;++j)frame(fr+j*stride(c,trans),c,trans,false);
                    fr+=nf*stride(c,trans); if(!shared&&next()%3==0)fr+=4*(next()%4);
                }
                *reinterpret_cast<uint32*>(seqMem.data())=uint32(nseq);
                *reinterpret_cast<uint32*>(At(hier.data(),0))=next();
                *reinterpret_cast<uint8**>(At(hier.data(),4))=(allowNull&&next()%5==0) ? nullptr : seqMem.data()+4;
                *reinterpret_cast<uint16*>(At(hier.data(),8))=uint16(nseq);
                hier[0xA]=uint8(c ? 1 : 0); hier[0xB]=uint8(next()%2);
                *reinterpret_cast<int32*>(At(hier.data(),0xC))=next()%2 ? -1 : int32(next()%200);
                *reinterpret_cast<float*>(At(hier.data(),0x10))=next()%2 ? 0.f : uni(0.f,3.f);
                return nseq; };

            // Nodo sobre nodeSeq/assoc; minFrames = minimo de keyframes; tiempos > 0 si la animacion es ciclica (si no,
            // NextKeyFrame/FindKeyFrame no terminan, tambien en el EXE).
            auto buildNode=[&](bool c,int32 minFrames,bool positiveAlways){
                for(auto& b:frames)b=uint8(next()); for(auto& b:assoc)b=uint8(next()); for(auto& b:nodeSeq)b=uint8(next()); for(auto& b:node)b=uint8(next());
                const uint16 aflags=uint16(next()); *reinterpret_cast<uint16*>(At(assoc.data(),0x2E))=aflags;
                const bool looped=(aflags&2)!=0;
                *reinterpret_cast<float*>(At(assoc.data(),0x18))=next()%8==0 ? 0.f : next()%8==0 ? 1.f : uni(-0.5f,1.5f);
                *reinterpret_cast<float*>(At(assoc.data(),0x28))=next()%8==0 ? 0.f : uni(0.f,0.5f);
                const bool trans=next()%2;
                const uint16 flags=uint16((next()%4==0 ? (next()&0xFFE0) : 0)|(next()%8 ? 1 : 0)|(trans ? 2 : 0)|(c ? 4 : 0)|(next()%2 ? 8 : 0));
                const int32 nf=minFrames+int32(next()%uint32(11-minFrames));
                uint8* fr=frames.data()+FRAME_BASE;
                *reinterpret_cast<uint32*>(nodeSeq.data())=next(); *reinterpret_cast<uint16*>(nodeSeq.data()+4)=flags; *reinterpret_cast<uint16*>(nodeSeq.data()+6)=uint16(nf); *reinterpret_cast<uint8**>(nodeSeq.data()+8)=fr;
                const bool positive=positiveAlways||looped||next()%2;
                // Keyframes validos tambien en -2, -1 y nf: el EXE lee esos indices sin comprobar (actual -1, ultimo de 0 frames).
                for(int32 j=-2;j<nf+1;++j)frame(fr+j*stride(c,trans),c,trans,positive);
                const int32 span=std::max<int32>(nf,1);
                *reinterpret_cast<float*>(At(node.data(),0))=uni(0.f,3.f);
                *reinterpret_cast<float*>(At(node.data(),4))=uni(-2.f,5.f);
                *reinterpret_cast<int16*>(At(node.data(),8))=int16(next()%8==0 ? -1 : next()%16==0 ? nf : int32(next()%uint32(span)));
                *reinterpret_cast<int16*>(At(node.data(),0xA))=int16(next()%8==0 ? -1 : int32(next()%uint32(span)));
                *reinterpret_cast<float*>(At(node.data(),0xC))=next()%8==0 ? 0.f : uni(-0.6f,0.4f);
                *reinterpret_cast<uint8**>(At(node.data(),0x10))=nodeSeq.data();
                *reinterpret_cast<uint8**>(At(node.data(),0x14))=assoc.data();
                return nf; };

            auto run=[&](bool candidate)->json {
                stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0xA16u; if(!stt)stt=1; nrec=0; heap.fill(0xCD); heapTop=0; moveSeed=next();
                lastQ[0]=lastQ[1]=lastQ[2]=0.f; lastQ[3]=1.f;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate;
                const uintptr_t a=fn.address;
                json out={{"switches",switches}}; json ret=nullptr;
                // Llamada con las dependencias sustituidas solo durante ella (sin json ni registro de la DLL dentro).
                auto call=[&](auto&& body){
                    Patches p;
                    p.Add(0x72F420,FakeMalloc); p.Add(bit_cast<uintptr_t>(static_cast<void*(*)(uint32,uint32)>(&CMemoryMgr::Malloc)),FakeMalloc);
                    p.Add(0x72F430,FakeFree); p.Add(bit_cast<uintptr_t>(&CMemoryMgr::Free),FakeFree);
                    p.Add(0x72F500,FakeMove); p.Add(bit_cast<uintptr_t>(static_cast<void*(*)(void*)>(&CMemoryMgr::MoveMemory)),FakeMove);
                    p.Add(0x4D42A0,FakeRemoveCache); p.Add(bit_cast<uintptr_t>(&CAnimManager::RemoveFromUncompressedCache),FakeRemoveCache);
                    p.Add(0x4D41C0,FakeUncompressAnim); p.Add(bit_cast<uintptr_t>(&CAnimManager::UncompressAnimation),FakeUncompressAnim);
                    p.Add(0x8214BD,FakeDelete); p.Add(0x8213AE,FakeDelete);
                    body(); };
                void* H=hier.data(); void* N=node.data();

                if(cls=="CAnimBlendHierarchy") {
                    if(nm=="Constructor"){ for(auto& b:hier)b=uint8(next()); void* r{}; call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*)>(a)(H); }); ret=r==H; }
                    else if(nm=="Destructor"||nm=="Shutdown"||nm=="RemoveAnimSequences"){ buildHier(next()%2,true,true); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(H); }); }
                    else if(nm=="AllocSequenceBlock"){ buildHier(next()%2,true,false); const uint32 comp=next()%2; uint8* r{}; call([&]{ r=reinterpret_cast<uint8*(__thiscall*)(void*,uint32)>(a)(H,comp); }); ret=U(r); }
                    else if(nm=="FindSequence"){ const int32 n=buildHier(next()%2,true,false);
                        for(int32 k=0;k<n;++k)if(next()%3==0)*reinterpret_cast<uint32*>(seqMem.data()+4+k*12)=CKeyGen::GetUppercaseKey(names[next()%std::size(names)]);
                        const char* name=names[next()%std::size(names)]; CAnimBlendSequence* r{};
                        call([&]{ r=reinterpret_cast<CAnimBlendSequence*(__thiscall*)(void*,const char*)>(a)(H,name); }); ret=U(r); }
                    else if(nm=="SetName"){ buildHier(false,false,false); const char* name=names[next()%std::size(names)]; call([&]{ reinterpret_cast<void(__thiscall*)(void*,const char*)>(a)(H,name); }); }
                    else if(nm=="CalcTotalTime"||nm=="CalcTotalTimeCompressed"){ buildHier(nm=="CalcTotalTimeCompressed",false,false); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(H); }); }
                    else if(nm=="RemoveQuaternionFlips"){ buildHier(false,true,false); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(H); }); }
                    else if(nm=="GetSequenceBlock"){ buildHier(next()%2,false,false); void* r{}; call([&]{ r=reinterpret_cast<void*(__thiscall*)(void*)>(a)(H); }); ret=U(r); }
                    else if(nm=="Uncompress"){ buildHier(true,false,false); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(H); }); }
                    else if(nm=="CompressKeyframes"||nm=="RemoveUncompressedData"){ buildHier(false,true,false); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(H); }); }
                    else if(nm=="MoveMemory"){ buildHier(next()%2,true,true); uint8 r{}; call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(H); }); ret=r; }
                    else if(nm=="Print"){ buildHier(false,true,false); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(H); }); }
                    out["hier"]=ReversingProbe::Bytes(hier.data(),hier.size()); out["seqs"]=ReversingProbe::Bytes(seqMem.data(),seqMem.size());
                    out["frames"]=Fnv(frames.data(),frames.size()); out["heap"]=Fnv(heap.data(),heap.size());
                }
                else {
                    if(nm=="Init"){ for(auto& b:node)b=uint8(next()); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(N); }); }
                    else if(nm=="CalcTheta"){
                        alignas(16) float qa[4], qb[4]; quat(qa); quat(qb);
                        switch(next()%8){
                        case 0: std::memcpy(qb,qa,16); break;                                    // coseno ~1 (recorte y theta 0)
                        case 1: for(int k=0;k<4;++k)qb[k]=qa[k]*uni(1.f,1.5f); break;            // coseno > 1
                        case 2: for(int k=0;k<4;++k){qa[k]=0.f;} break;                          // coseno 0
                        case 3: for(int k=0;k<4;++k)qb[k]=qa[k]*uni(0.f,1.f); break;
                        default: break; }
                        const float d=qa[3]*qb[3]+qa[2]*qb[2]+qa[1]*qb[1]+qa[0]*qb[0];
                        if(d< -0.999f)for(int k=0;k<4;++k)qb[k]=-qb[k];                         // fuera: acos con NaN (errno del CRT)
                        float theta=bit_cast<float>(0x77777777u), inv=bit_cast<float>(0x77777777u);
                        call([&]{ reinterpret_cast<void(__cdecl*)(const float*,const float*,float*,float*)>(a)(qa,qb,&theta,&inv); });
                        ret=json::array({F(theta),F(inv)}); }
                    else if(nm=="CalcDeltas"){ buildNode(false,1,false); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(N); }); }
                    else if(nm=="CalcDeltasCompressed"){ buildNode(true,1,false); call([&]{ reinterpret_cast<void(__thiscall*)(void*)>(a)(N); }); }
                    else if(nm=="NextKeyFrame"||nm=="NextKeyFrameNoCalc"||nm=="NextKeyFrameCompressed"){ buildNode(nm=="NextKeyFrameCompressed",0,false);
                        uint8 r{}; call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(N); }); ret=r; }
                    else if(nm=="FindKeyFrame"){ const int32 nf=buildNode(false,0,true);
                        float total=0.f, cum[12]{}; for(int32 j=1;j<nf;++j){ total+=reinterpret_cast<const float*>(frames.data()+FRAME_BASE+j*stride(false,(nodeSeq[4]&2)!=0))[4]; cum[j]=total; }
                        float t; switch(next()%6){ case 0: t=nf>1 ? cum[1+next()%uint32(nf-1)] : 0.f; break; case 1: t=0.f; break; case 2: t=uni(-0.2f,0.f); break; default: t=uni(0.f,total*2.2f+0.1f); }
                        uint8 r{}; call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*,float)>(a)(N,t); }); ret=r; }
                    else if(nm=="SetupKeyFrameCompressed"){ buildNode(true,0,false); uint8 r{}; call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(N); }); ret=r; }
                    else if(nm=="UpdateTime"){ buildNode(false,0,false); uint8 r{}; call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*)>(a)(N); }); ret=r; }
                    else if(nm.rfind("Get",0)==0){ buildNode(nm.find("Compressed")!=std::string::npos,0,false);
                        CVector v{bit_cast<float>(0x7F7F7F7Fu),bit_cast<float>(0x7F7F7F7Fu),bit_cast<float>(0x7F7F7F7Fu)};
                        const float w=next()%6==0 ? 1.f : next()%8==0 ? 0.f : uni(-0.5f,1.5f);
                        call([&]{ reinterpret_cast<void(__thiscall*)(void*,CVector*,float)>(a)(N,&v,w); });
                        ret=json::array({F(v.x),F(v.y),F(v.z)}); }
                    else { buildNode(nm=="UpdateCompressed",1,false);
                        CVector v{bit_cast<float>(0x7F7F7F7Fu),bit_cast<float>(0x7F7F7F7Fu),bit_cast<float>(0x7F7F7F7Fu)};
                        CQuaternion q{bit_cast<float>(0x7F7F7F7Fu),bit_cast<float>(0x7F7F7F7Fu),bit_cast<float>(0x7F7F7F7Fu),bit_cast<float>(0x7F7F7F7Fu)};
                        const float w=next()%6==0 ? 1.f : next()%8==0 ? 0.f : uni(-0.5f,1.5f); uint8 r{};
                        call([&]{ r=reinterpret_cast<uint8(__thiscall*)(void*,CVector*,CQuaternion*,float)>(a)(N,&v,&q,w); });
                        ret=json::array({r,F(v.x),F(v.y),F(v.z),F(q.x),F(q.y),F(q.z),F(q.w)}); }
                    out["node"]=ReversingProbe::Bytes(node.data(),node.size()); out["frames"]=Fnv(frames.data(),frames.size());
                }
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
