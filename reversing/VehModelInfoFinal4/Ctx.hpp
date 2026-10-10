#pragma once
// Contexto de una ejecucion (un lado de un par): generador, fixtures del mundo RW falso y utilidades de salida.
#include "Support.hpp"

namespace VehModelInfoFinal4Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

// Offsets de plugin leidos del EXE en tiempo de ejecucion
inline int32 nameOff{}, frameVisOff{}, atomVisOff{};

inline const char* const kFrameNames[]{"chassis","door_lf_dummy","door_dam","door_ok","bonnet_dam","bonnet_ok","windscreen","windscreen_vlo","x_vlo","boat_hi",
    "moving_rotor","moving_rotor2","toprotor","rearrotor_x","wheel_lf_dummy","exhaust","misc_a","extra_b","chassis_vlo","boat_hi_x","toprotorx","Door_Dam","wing_ok_dam","hydralics"};
constexpr int NFRAMENAMES=int(std::size(kFrameNames));
inline const char* const kTexNames[]{"#remap1","remap200","plain","vehiclelights128","#hash","remapx","zzz","remap","vehiclelightson128","#"};
constexpr int NTEXNAMES=int(std::size(kTexNames));

struct Ctx {
    const Fn* fn{}; uintptr_t a{}; std::string nm; int i{}; uint32 f{}; bool candidate{};
    uint32 st{1};
    json out, ret;
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    float uni(float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); }
    void fill(void* p,size_t n){ auto* b=static_cast<uint8*>(p); size_t k=0; for(;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(b+k,&v,4); } for(;k<n;++k)b[k]=uint8(next()); }
    bool chance(uint32 n){ return n && next()%n==0; }

    // --- fixtures ---
    uint8* O() { return w.mi[0]; }
    // Model info falso: bytes aleatorios, vtable copiada del EXE (slot 4 -> stub GetModelType)
    void FillMi(int k,int32 type=INT32_MIN){
        auto* m=w.mi[k]; fill(m,MI_SIZE);
        std::memcpy(vtc[k].data(),reinterpret_cast<void*>(0x85C5C8),17*4);
        vtc[k][4]=reinterpret_cast<uintptr_t>(&S_GetModelTypeV);
        At<void*>(m,0)=vtc[k].data();
        if(type!=INT32_MIN)At<int32>(m,0x3C)=type;
    }
    // Estructura de vehiculo (CVehicleStructure) en el pool falso: contenido aleatorio coherente
    uint8* FillVs(int k,int nExtras=-1){
        auto* v=Vs(k); std::memset(v,0,0x314);
        for(int u=0;u<18;++u){ At<int32>(v,0xB4+0x20*u+0x1C)=chance(3) ? -1-int32(next()%3) : int32(next()%24); for(int q=0;q<7;++q)At<float>(v,0xB4+0x20*u+4*q)=uni(-2.f,2.f); }
        for(int d=0;d<15;++d)for(int q=0;q<3;++q)At<float>(v,12*d+4*q)=chance(5) ? 0.f : uni(-3.f,3.f);
        const int n= nExtras>=0 ? nExtras : int(next()%7);
        v[0x30C]=uint8(n);
        for(int e=0;e<6;++e)At<void*>(v,0x2F4+4*e)= e<n ? static_cast<void*>(w.at[e%NAT]) : nullptr;
        At<uint32>(v,0x310)=next();
        return v;
    }

    // Construye atomics, geometrias, materiales, texturas y frames con contenido aleatorio.
    void BuildWorld(int nFrames=-1){
        std::memset(&tb,0,sizeof(tb));
        fill(&w,sizeof(World));
        std::memset(w.names,0,sizeof(w.names));
        // texturas
        for(int k=0;k<NTEX;++k){ auto* t=w.tex[k]; std::memset(t+0x10,0,0x40); const char* n=kTexNames[next()%NTEXNAMES]; std::memcpy(t+0x10,n,std::min<size_t>(strlen(n),31)); At<uint32>(t,0x50)=next()%4; tb.texDict[k]=int(next()%3); }
        // materiales
        static const uint32 special[]{0xFF3C,0xAF00FF,0xFFFF00,0xFF00FF,0xAFFF,0xC8FF00,0xFFB9,0x3CFF};
        for(int k=0;k<NMAT;++k){ auto* m=w.mat[k];
            At<void*>(m,0)= chance(4) ? nullptr : static_cast<void*>(w.tex[next()%NTEX]);
            const uint32 cm=next()%4; uint32 col= cm==0 ? special[next()%8] : next();
            col=(col&0x00FFFFFFu)|((next()%3==0 ? 0xFFu : (next()&0xFFu))<<24); At<uint32>(m,4)=col;
            for(int q=0;q<3;++q)At<float>(m,0xC+4*q)=uni(0.f,2.f);
            tb.matEff[k]=next()%6; }
        // geometrias y mallas
        for(int g=0;g<NGEO;++g){ auto* ge=w.geo[g]; auto* me=w.mesh[g];
            At<void*>(ge,0x54)=me; const int nm=int(next()%7); std::memset(me,0,MESH_SIZE);
            At<uint16>(me,4)=uint16(nm); At<uint32>(me,0xC)=0;
            for(int q=0;q<8;++q){ At<void*>(me,0x10+0xC*q)=nullptr; At<uint32>(me,0x14+0xC*q)=next()%100; At<void*>(me,0x18+0xC*q)= chance(6) ? nullptr : static_cast<void*>(w.mat[next()%NMAT]); }
            tb.geoMatN[g]=int(next()%6); for(int q=0;q<6;++q)tb.geoMat[g][q]=w.mat[next()%NMAT]; }
        // frames: arbol aleatorio con el frame 0 como raiz
        const int n= nFrames>0 ? nFrames : 1+int(next()%NFR);
        treeN=n;
        for(int k=0;k<NFR;++k){ auto* fr=w.fr[k]; At<void*>(fr,0x98)=nullptr; At<void*>(fr,0x9C)=nullptr; At<void*>(fr,4)=nullptr;
            std::memset(fr+nameOff,0,24); const char* nmx=kFrameNames[next()%NFRAMENAMES]; std::memcpy(fr+nameOff,nmx,std::min<size_t>(strlen(nmx),23));
            At<int32>(fr,frameVisOff)= chance(3) ? int32(next()%24) : 0;
            auto* fm=reinterpret_cast<float*>(fr+0x10); for(int q=0;q<16;++q)fm[q]=uni(-2.f,2.f); }
        for(int k=1;k<n;++k){ auto* par=w.fr[next()%k]; auto* fr=w.fr[k]; At<void*>(fr,4)=par;
            if(next()%2){ At<void*>(fr,0x9C)=At<void*>(par,0x98); At<void*>(par,0x98)=fr; }
            else { void** link=&At<void*>(par,0x98); while(*link)link=&At<void*>(*link,0x9C); *link=fr; } }
        // clumps
        for(int k=0;k<NCL;++k){ auto* c=w.clump[k]; c[0]=2; At<void*>(c,4)=w.fr[0]; }
        // atomics
        for(int k=0;k<NAT;++k){ auto* at=w.at[k];
            at[0]=1; at[2]=uint8(next()%3 ? 4 : next()&7);                    // bandera de render habitual
            const int fi=int(next()%unsigned(n)); At<void*>(at,4)=w.fr[fi]; At<void*>(at,0x18)=w.geo[next()%NGEO];
            At<void*>(at,0x48)= chance(3) ? nullptr : reinterpret_cast<void*>(uintptr_t(0x733240+0x10*(next()%8)));
            At<uint16>(at,atomVisOff+2)=uint16(chance(4) ? next() : (next()%3 ? next()&0x3 : next()&0xFFFF)); }
        for(int k=0;k<NNA;++k){ auto* at=w.nat[k]; at[0]=1; at[2]=4; At<void*>(at,4)=nullptr; At<void*>(at,0x18)=w.geo[k%NGEO]; tb.cloneQueue[k]=at; }
        for(int k=0;k<NNF;++k){ tb.newFrameQueue[k]=w.nfr[k]; At<void*>(w.nfr[k],4)=nullptr; }
        // objetos por frame y atomics del clump
        for(int k=0;k<NAT;++k){ const int fi=FrIdx(At<void*>(w.at[k],4)); if(fi>=0&&tb.fobjN[fi]<3)tb.fobj[fi][tb.fobjN[fi]++]=w.at[k]; }
        tb.clumpAtomN[0]=0; for(int k=0;k<NAT;++k)if(next()%5)tb.clumpAtom[0][tb.clumpAtomN[0]++]=w.at[k];
        tb.clumpAtomN[1]=2; tb.clumpAtom[1][0]=w.at[0]; tb.clumpAtom[1][1]=w.at[1];
        tb.currentDict=w.dict[1]; tb.findCb=reinterpret_cast<void*>(uintptr_t(0x5555000));
        tb.nextCreateInstanceClump=w.clump[1];
    }
    int treeN{};
    json Bytes(const void* p,size_t n){ return ReversingProbe::Bytes(p,n); }
};
}
