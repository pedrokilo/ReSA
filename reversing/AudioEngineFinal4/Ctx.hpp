#pragma once
// Contexto de una ejecucion (un lado de un par): generador, fixtures del mundo falso y utilidades de salida.
#include "Support.hpp"

namespace AudioEngineFinal4Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

#define ENGP static_cast<void*>(eng.data())

struct Ctx {
    const Fn* fn{}; uintptr_t a{}; std::string nm; int i{}; uint32 f{}; bool candidate{};
    uint32 st{1};
    json out, ret;
    bool engCustom{};          // la funcion calcula ella misma el resumen del engine (constructor / destructor)
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    float uni(float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); }
    void fill(void* p,size_t n){ auto* b=static_cast<uint8*>(p); size_t k=0; for(;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(b+k,&v,4); } for(;k<n;++k)b[k]=uint8(next()); }
    bool chance(uint32 n){ return n && next()%n==0; }
    bool coin(){ return (next()&1u)!=0; }
    uint32 u8(){ return next()&0xFFu; }

    // float del dominio habitual: 0, +-1, uniformes y algun NaN silencioso (nunca NaN de senalizacion)
    float fl(){
        switch(next()%14){
        case 0: return 0.f; case 1: return -0.f; case 2: return 1.f; case 3: return -1.f; case 4: return 0.005f; case 5: return 0.995f;
        case 6: return uni(-1.f,1.f); case 7: return uni(-100.f,100.f); case 8: return uni(-5000.f,5000.f); case 9: return std::bit_cast<float>(0x7FC00000u);
        case 10: return uni(0.f,64.f); default: return uni(0.f,2.f); } }
    // identificador de emisora (eRadioID es int8): dominio del juego y algun byte cualquiera
    int32 radio(){ return chance(8) ? int32(int8(next())) : int32(next()%15)-1; }

    // --- guiones de los dobles ---
    void SeqLead(uint32 t,int maxN,int32 tail,int32 lo,int32 hi){
        auto& s=seqs[t]; s.n=int32(next()%uint32(maxN+1)); s.pos=0; s.tail=tail;
        for(int k=0;k<s.n;++k)s.v[k]= lo+int32(next()%uint32(hi-lo+1)); }
    void SeqBool(uint32 t,int maxN,int32 tail){ SeqLead(t,maxN,tail,0,1); }
    void SeqConst(uint32 t,int32 v){ auto& s=seqs[t]; s.n=0; s.pos=0; s.tail=v; }
    void SeqFloat(uint32 t,float v){ SeqConst(t,int32(F(v))); }

    // --- fixtures ---
    // Mundo falso estatico: opacos aleatorios, vectores finitos, nombres, cadena de ajustes y GlobalWeapon con ceros.
    void BuildWorld(){
        std::memset(&w,0,sizeof(w));
        for(auto& e:w.ent)fill(e,0x100);
        for(auto& v:w.vec){ for(int k=0;k<3;++k){ const float x=fl(); std::memcpy(v+4*k,&x,4); } }
        for(auto& s:w.sett)fill(s,0x80);
        fill(w.settRef,0x80);
        for(int k=0;k<4;++k){ const int len=1+int(next()%30); for(int q=0;q<len;++q)w.names[k][q]=char('A'+next()%26); }
        for(auto& l:w.listen)l=int32(next()%100000);
        At<uint32>(w.gw,0)=0x862E5C;                   // vtable del EXE de CAEGlobalWeaponAudioEntity
    }
    // CAudioEngine falso: bytes aleatorios con los campos propios en su dominio.
    void Begin(){
        std::memset(seqs.data(),0,sizeof(Seq)*seqs.size());
        BuildWorld();
        fill(eng.data(),ENG_SIZE);
        eng[0]=uint8(next()&1u); eng[1]=uint8(next()&1u);                      // m_bPlayingMissionCompleteTrack / m_bStoppingMissionCompleteTrack
        At<int8>(eng.data(),2)=int8(radio()); At<int8>(eng.data(),3)=int8(radio());
        At<int32>(eng.data(),4)=int32(next()%48);                              // m_nBackgroundAudioChannel
        At<void*>(eng.data(),0x1E34)= chance(4) ? nullptr : static_cast<void*>(w.gw);   // m_GlobalWeaponAE
    }
    void* Ent(){ return w.ent[next()%6]; }
    void* EntN(){ return chance(5) ? nullptr : Ent(); }
    void* Vec(){ return w.vec[next()%6]; }
    json Bytes(const void* p,size_t n){ return ReversingProbe::Bytes(p,n); }
};
}
