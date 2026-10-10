#pragma once
// Contexto de una ejecucion (un lado de un par): generador, fixtures del mundo RW falso, listas de alfa y utilidades de salida.
#include "Support.hpp"

namespace VisPluginsFinal4Live {
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };

constexpr uintptr_t MODELPTRS=0xA9B0C8;
constexpr int MI_BASE=450;            // indices de model info falsos (tabla del EXE intercambiada temporalmente)
constexpr uintptr_t G_DIST=0xC88024, G_ANGLE=0xC88020, G_LOD0=0xC88040, G_MULTI=0xC88044, G_BIGLOD0=0xC88038, G_LOD1=0xC8803C,
                    G_CULL=0xC8802C, G_CULLBIG=0xC88028, G_PEDLOD=0xC88034, G_PEDFADE=0xC88030, G_CAMERA=0xC8804C, G_CAMPOS=0xC88050;
constexpr uintptr_t EXE_DEFAULTVIS=0x732A30, EXE_VEHVIS=0x7336F0, EXE_BIGVIS=0x732AB0, EXE_ADRC=0x7491C0, EXE_RENDERATOMIC=0x732B30, EXE_RENDERENTITY=0x732B40;

inline float& Gf(uintptr_t a){ return *reinterpret_cast<float*>(a); }
inline uint32& Gu(uintptr_t a){ return *reinterpret_cast<uint32*>(a); }

struct Ctx {
    const Fn* fn{}; uintptr_t a{}; std::string nm; int i{}; uint32 f{}; bool candidate{};
    uint32 st{1};
    int capBuilt[NLISTS]{};   // enlaces construidos por lista (solo se normalizan esos)
    json out, ret;
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    float uni(float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); }
    void fill(void* p,size_t n){ auto* b=static_cast<uint8*>(p); size_t k=0; for(;k+4<=n;k+=4){ const uint32 v=next(); std::memcpy(b+k,&v,4); } for(;k<n;++k)b[k]=uint8(next()); }
    bool chance(uint32 n){ return n && next()%n==0; }
    uint32 pick(std::initializer_list<uint32> l){ return *(l.begin()+next()%l.size()); }
    json Bytes(const void* p,size_t n){ return ReversingProbe::Bytes(p,n); }
    static float QNaN(){ return std::bit_cast<float>(0x7FC00000u); }

    // --- fixtures ---
    uint8* RndAt(){ return w.at[next()%NAT]; }
    uint8* RndFr(){ return w.fr[next()%NFR]; }
    uint8* RndCl(){ return w.cl[next()%NCL]; }
    uint8* RndMat(){ return w.mat[next()%NMAT]; }
    float fl(float lim){ return uni(-lim,lim); }
    void RndMatrix(void* m,float lim){ auto* p=reinterpret_cast<float*>(m); for(int q=0;q<16;++q)p[q]=fl(lim); }

    // Mundo completo con contenido aleatorio coherente (todos los punteros apuntan a objetos del propio mundo)
    void BuildScene(){
        std::memset(&tb,0,sizeof(tb));
        fill(&w,sizeof(World));
        std::memset(w.guard,0x5A,sizeof(w.guard));
        // frames: el de indice 0 es el del clump del coche
        for(int k=0;k<NFR;++k){ auto* fr=w.fr[k]; RndMatrix(fr+0x10,40.f); RndMatrix(fr+0x50,40.f);
            At<int32>(fr,frameOff)= chance(3) ? int32(next()%24) : int32(next()); At<void*>(fr,4)=nullptr; }
        // camara y su frame
        RndMatrix(w.camFr+0x10,200.f); RndMatrix(w.camFr+0x50,200.f); At<void*>(w.cam,4)=w.camFr; w.cam[0]=4;
        for(int k=0;k<4;++k)RndMatrix(w.mx[k],60.f);
        { auto* v=reinterpret_cast<float*>(w.vec); for(int q=0;q<16;++q)v[q]=fl(100.f); }
        // texturas y materiales
        for(int k=0;k<NMAT;++k){ auto* m=w.mat[k]; At<void*>(m,0)= chance(3) ? nullptr : static_cast<void*>(w.tex[next()%NTX]); }
        // geometrias (tabla de materiales: puede repetir materiales)
        for(int g=0;g<NGEO;++g){ auto* ge=w.geo[g]; const int nm=int(next()%9); At<uint32>(ge,8)=next()&~0x40u; if(chance(4))At<uint32>(ge,8)|=0x40u;
            for(int q=0;q<12;++q)w.geoMats[g][q]=uint32(uintptr_t(w.mat[next()%NMAT]));
            if(nm>=2&&chance(3))w.geoMats[g][1]=w.geoMats[g][0];
            At<uint32>(ge,0x20)=uint32(uintptr_t(w.geoMats[g])); At<int32>(ge,0x24)=nm; }
        // model infos y modelos de colision
        for(int q=0;q<17;++q)w.vtMi[q]=uint32(uintptr_t(&S_Trap)); w.vtMi[4]=uint32(uintptr_t(&S_ModelType));
        tb.miType=uint8(pick({6,6,6,5,1,7,3}));
        for(int k=0;k<NMI;++k){ auto* mi=w.mi[k]; auto* co=w.col[k];
            At<void*>(mi,0)=w.vtMi; mi[0x0C]=uint8(chance(4) ? 255 : next()); mi[0x12]=uint8((chance(3)?4:0)|(chance(3)?8:0)|(next()&0xE3));
            At<void*>(mi,0x14)=co; At<float>(mi,0x18)=uni(10.f,600.f);
            At<int32>(mi,0x3C)= int32(pick({0,1,2,3,4,5,6,6,7,8,8,9,10,11}));
            At<float>(co,0x24)=uni(0.5f,60.f); for(int q=0;q<3;++q)At<float>(co,0x18+4*q)=fl(8.f); }
        // clumps (tipo 2 en el offset 0)
        for(int k=0;k<NCL;++k){ auto* cl=w.cl[k]; cl[0]=2; At<void*>(cl,4)=w.fr[k%2 ? 0 : k]; At<uint32>(cl,clumpOff)=uint32(pick({EXE_DEFAULTVIS,EXE_VEHVIS,EXE_BIGVIS}));
            At<int32>(cl,clumpOff+4)= chance(3) ? 255 : int32(next()%256); }
        // atomics
        for(int k=0;k<NAT;++k){ auto* at=w.at[k]; at[0]=1; at[2]=uint8(next()%4 ? 4 : next()&7);
            At<void*>(at,4)=w.fr[1+next()%(NFR-1)]; At<void*>(at,0x18)=w.geo[next()%NGEO]; At<void*>(at,0x3C)=w.cl[next()%NCL];
            At<uint32>(at,0x48)= chance(3) ? 0u : uint32(pick({0x733240u,0x7331E0u,EXE_ADRC,0x733F80u}));
            At<uint32>(at,0x4C)= chance(2) ? 2u : 0u;
            for(int q=0;q<4;++q)At<float>(at,0x1C+4*q)= q<3 ? fl(10.f) : uni(0.2f,12.f);
            uint32 fg=next()&0xFFFFu; if(chance(3))fg&=~0x400u; if(chance(2))fg&=~0x80u; if(chance(3))fg|=0x8000u;
            At<uint16>(at,atomOff+2)=uint16(chance(5) ? 0 : fg);
            At<int16>(at,atomOff)= int16(chance(3) ? -1 : MI_BASE+int(next()%NMI)); }
        // objetos de arma y peds
        for(int k=0;k<NWC;++k){ auto* c=w.wc[k]; c[0]=2; At<void*>(c,4)=w.wfr[k]; RndMatrix(w.wfr[k]+0x10,30.f); RndMatrix(w.wfr[k]+0x50,30.f); }
        for(int q=0;q<32;++q){ w.vtEnt[q]=uint32(uintptr_t(&S_Trap)); w.vtPed[q]=uint32(uintptr_t(&S_Trap)); }
        w.vtEnt[19]=uint32(uintptr_t(&S_EntSetup)); w.vtEnt[20]=uint32(uintptr_t(&S_EntRemove));
        w.vtPed[19]=uint32(uintptr_t(&S_PedSetup)); w.vtPed[20]=uint32(uintptr_t(&S_EntRemove));
        for(int k=0;k<NPED;++k){ auto* p=w.ped[k]; At<void*>(p,0)=w.vtPed; At<void*>(p,0x18)=w.cl[1+k%(NCL-1)];
            At<void*>(p,0x4F4)= chance(5) ? nullptr : static_cast<void*>(w.wc[next()%NWC]); p[0x718]=uint8(next()%13);
            for(int s=0;s<13;++s)At<int32>(p,0x5A0+0x1C*s)= chance(4) ? 46 : int32(next()%47);
            for(int q=0;q<4;++q)At<uint32>(p,0x20+4*q)=next(); }
        for(int k=0;k<NWI;++k){ At<uint32>(w.wi[k],0x18)= (next()&~0x800u)|(chance(2) ? 0x800u : 0u); }
        for(int k=0;k<NHX;++k)RndMatrix(w.hmx[k],20.f);
        // entidades
        for(int k=0;k<NENT;++k){ auto* e=w.ent[k]; At<void*>(e,0)=w.vtEnt; At<void*>(e,0x18)= (k%3==0) ? static_cast<void*>(w.cl[k%NCL]) : static_cast<void*>(w.at[k%NAT]);
            At<int16>(e,0x22)=int16(MI_BASE+int(next()%NMI)); At<void*>(e,0x30)= chance(2) ? nullptr : static_cast<void*>(w.ent[(k+1)%NENT]);
            uint32 fg=next(); fg&=~((1u<<13)|(1u<<15)|(1u<<20)|(1u<<27)|(1u<<8)); if(chance(2))fg|=1u<<15; if(chance(3))fg|=1u<<20; if(chance(2))fg|=1u<<27; if(chance(3))fg|=1u<<8;
            At<uint32>(e,0x1C)=fg; }
        // tablas de los stubs
        for(int k=0;k<NCL;++k){ int n=0; for(int q=0;q<NAT;++q)if(next()%3==0&&n<NAT)tb.clumpAtom[k][n++]=w.at[q]; tb.clumpAtomN[k]=n; }
        tb.reg[0]=int32(pick({1u,0x78u,0x7Cu,0xFFFFFFFFu,0u})); tb.reg[1]=int32(pick({1u,0x8Cu,0xFFFFFFFFu,0x20u})); tb.reg[2]=int32(pick({1u,0x90u,0xFFFFFFFFu,4u}));
        tb.visRet=int(next()&1); tb.plugOff[0]=atomOff; tb.plugOff[1]=frameOff; tb.plugOff[2]=clumpOff;
        // tabla de model infos del EXE
        for(int k=0;k<NMI;++k)Gu(MODELPTRS+4u*unsigned(MI_BASE+k))=uint32(uintptr_t(w.mi[k]));
        Gu(MODELPTRS+4u*unsigned(MI_BASE+10))=0;
        // camara activa
        Gu(G_CAMERA)=uint32(uintptr_t(w.cam)); Gu(G_CAMPOS)=uint32(uintptr_t(w.camFr+0x40));
    }

    // Distancias y umbrales del EXE: aleatorios, con aciertos exactos de los limites y algun NaN
    float Near(float ref,bool nan=true){
        switch(next()%14){ case 0: return ref; case 1: return std::nextafter(ref,0.f); case 2: return std::nextafter(ref,1e30f); case 3: return nan&&chance(2) ? QNaN() : ref*uni(0.f,2.f);
            default: return ref*uni(0.f,2.f); }
    }
    void SetDists(bool nan=true){
        const float lod0=uni(1500.f,30000.f), multi=lod0*uni(0.3f,1.2f), big=lod0*uni(1.f,6.f), cull=uni(50.f,3000.f), cullBig=uni(300.f,20000.f);
        Gf(G_LOD0)=lod0; Gf(G_MULTI)=multi; Gf(G_BIGLOD0)=big; Gf(G_LOD1)=chance(2) ? big : lod0*uni(0.5f,4.f); Gf(G_CULL)=cull; Gf(G_CULLBIG)=cullBig;
        Gf(G_PEDLOD)=uni(500.f,5000.f); Gf(G_PEDFADE)=uni(500.f,5000.f);
        const float refs[]{lod0,multi,big,cull,cullBig,Gf(G_PEDLOD),Gf(G_LOD1)};
        Gf(G_DIST)=Near(refs[next()%7],nan);
        switch(next()%8){ case 0: Gf(G_ANGLE)=std::bit_cast<float>(0x3E4CCCCDu); break; case 1: Gf(G_ANGLE)=std::nextafter(std::bit_cast<float>(0x3E4CCCCDu),0.f); break;
            case 2: Gf(G_ANGLE)=std::nextafter(std::bit_cast<float>(0x3E4CCCCDu),1.f); break; case 3: Gf(G_ANGLE)= nan&&chance(3) ? QNaN() : uni(-1.5f,1.5f); break;
            default: Gf(G_ANGLE)=uni(-1.5f,1.5f); break; }
    }
    // Camara del juego (multiplicadores y clip) para CalculateFading / RenderEntity / SetRenderWareCamera
    void SetCameraMults(){
        Gf(0xB6F118)=Fl(pick({F(1.f),F(0.5f),F(2.f),F(3.f),F(1.7f)}));
        Gf(0xB6F11C)=Fl(pick({F(1.f),F(0.6f),F(1.5f),F(2.f),F(0.9f)}));
        Gf(0xB76848)=pick({0u,1u}) ? uni(80.f,1500.f) : 400.f; Gf(0x8CD804)=Fl(pick({F(1.f),F(2.f),F(0.5f),F(1.3f)}));
    }

    // --- listas de alfa (CLinkList<AlphaObjectInfo>: enlace de 20 bytes, datos en 0/4/8, prev 0xC, next 0x10) ---
    static uint8* LB(int li){ return reinterpret_cast<uint8*>(LADDR[li]); }
    // Lista con 'cap' enlaces, 'nUsed' ya usados (ordenados ascendente por distancia)
    void BuildList(int li,int cap,int nUsed,bool testCbs=true){
        capBuilt[li]=cap; uint8* L=LB(li); uint8* nodes=Nodes(li); fill(L,0x58); fill(nodes,size_t(LCAP[li])*20);
        uint8 *head=L,*tail=L+0x14,*fh=L+0x28,*ft=L+0x3C;
        At<void*>(head,0x10)=tail; At<void*>(tail,0xC)=head; At<void*>(fh,0x10)=ft; At<void*>(ft,0xC)=fh; At<void*>(L,0x50)=nodes;
        At<float>(head,8)=0.f; At<float>(tail,8)=1e8f;
        for(int k=cap-1;k>=0;--k){ uint8* n=nodes+20*k; At<void*>(n,0x10)=At<void*>(fh,0x10); At<void*>(At<uint8*>(fh,0x10),0xC)=n; At<void*>(n,0xC)=fh; At<void*>(fh,0x10)=n; }
        for(int u=0;u<nUsed&&u<cap;++u){
            uint8* n=At<uint8*>(fh,0x10); if(n==ft)break;
            At<void*>(n,0)= (next()%3) ? static_cast<void*>(w.ent[next()%NENT]) : static_cast<void*>(w.at[next()%NAT]);
            At<uint32>(n,4)= testCbs ? uint32(uintptr_t(next()&1 ? &CbA : &CbB)) : uint32(EXE_RENDERENTITY);
            At<float>(n,8)=uni(0.f,5000.f);
            At<void*>(At<uint8*>(n,0x10),0xC)=At<void*>(n,0xC); At<void*>(At<uint8*>(n,0xC),0x10)=At<void*>(n,0x10);   // fuera de la libre
            uint8* i=At<uint8*>(head,0x10); while(i!=tail&&Fl(At<uint32>(i,8))<Fl(At<uint32>(n,8)))i=At<uint8*>(i,0x10);   // dentro de la usada, ordenado
            uint8* prev=At<uint8*>(i,0xC); At<void*>(n,0x10)=i; At<void*>(i,0xC)=n; At<void*>(n,0xC)=prev; At<void*>(prev,0x10)=n;
        }
    }
    void BuildAllLists(){ for(int li=0;li<NLISTS;++li){ const int cap= chance(8) ? 0 : 1+int(next()%unsigned(LCAP[li])); BuildList(li,cap,int(next()%unsigned(cap+1))); } }
    json DumpList(int li){
        uint8* L=LB(li); json used=json::array(), freel=json::array(); int guardN=0;
        for(uint8* n=At<uint8*>(L,0x10); n&&n!=L+0x14&&guardN<LCAP[li]+3; n=At<uint8*>(n,0x10),++guardN)
            used.push_back({Id(At<void*>(n,0)),uint32(Canon(At<uint32>(n,4))),At<uint32>(n,8),int((n-Nodes(li))/20)});
        guardN=0; for(uint8* n=At<uint8*>(L,0x28+0x10); n&&n!=L+0x3C&&guardN<LCAP[li]+3; n=At<uint8*>(n,0x10),++guardN)freel.push_back(int((n-Nodes(li))/20));
        return json{{"used",used},{"free",freel},{"hd",{Id(At<void*>(L,0xC)),Id(At<void*>(L,0x10)),At<uint32>(L,8)}},{"tl",{Id(At<void*>(L,0x14+0xC)),Id(At<void*>(L,0x14+0x10)),At<uint32>(L,0x14+8)}},
                    {"links",Id(At<void*>(L,0x50))}};
    }
    // Lista de peds con arma (CLinkList<CPed*>: enlace de 12 bytes, dato en 0, prev 4, next 8; cabeceras en 0/0xC/0x18/0x24, links en 0x30)
    void BuildWList(int cap,int nUsed){
        uint8* L=reinterpret_cast<uint8*>(WLADDR); fill(L,0x34); fill(w.lnw,sizeof(w.lnw));
        uint8 *head=L,*tail=L+0xC,*fh=L+0x18,*ft=L+0x24;
        At<void*>(head,8)=tail; At<void*>(tail,4)=head; At<void*>(fh,8)=ft; At<void*>(ft,4)=fh; At<void*>(L,0x30)=w.lnw;
        for(int k=cap-1;k>=0;--k){ uint8* n=w.lnw+12*k; At<void*>(n,8)=At<void*>(fh,8); At<void*>(At<uint8*>(fh,8),4)=n; At<void*>(n,4)=fh; At<void*>(fh,8)=n; }
        for(int u=0;u<nUsed&&u<cap;++u){
            uint8* n=At<uint8*>(fh,8); if(n==ft)break;
            At<void*>(n,0)= chance(6) ? nullptr : static_cast<void*>(w.ped[next()%NPED]);
            At<void*>(At<uint8*>(n,8),4)=At<void*>(n,4); At<void*>(At<uint8*>(n,4),8)=At<void*>(n,8);
            uint8* nx=At<uint8*>(head,8); At<void*>(n,8)=nx; At<void*>(nx,4)=n; At<void*>(n,4)=head; At<void*>(head,8)=n;   // al principio de la usada
        }
    }
    json DumpWList(){
        uint8* L=reinterpret_cast<uint8*>(WLADDR); json used=json::array(), freel=json::array(); int g=0;
        for(uint8* n=At<uint8*>(L,8); n&&n!=L+0xC&&g<WCAP+3; n=At<uint8*>(n,8),++g)used.push_back({Id(At<void*>(n,0)),int((n-w.lnw)/12)});
        g=0; for(uint8* n=At<uint8*>(L,0x18+8); n&&n!=L+0x24&&g<WCAP+3; n=At<uint8*>(n,8),++g)freel.push_back(int((n-w.lnw)/12));
        return json{{"used",used},{"free",freel},{"links",Id(At<void*>(L,0x30))},{"hd",{Id(At<void*>(L,4)),Id(At<void*>(L,8))}},{"tl",{Id(At<void*>(L,0xC+4)),Id(At<void*>(L,0xC+8))}}};
    }
    // Normaliza en el mundo los punteros a funciones (la DLL guarda los suyos donde el EXE guarda la direccion del original)
    void Scrub(bool lists){
        for(int k=0;k<NAT;++k)At<uint32>(w.at[k],0x48)=uint32(Canon(At<uint32>(w.at[k],0x48)));
        for(int k=0;k<NCL;++k)At<uint32>(w.cl[k],clumpOff)=uint32(Canon(At<uint32>(w.cl[k],clumpOff)));
        if(lists)for(int li=0;li<NLISTS;++li){ uint8* nd=Nodes(li); for(int k=0;k<capBuilt[li];++k)At<uint32>(nd,20*k+4)=uint32(Canon(At<uint32>(nd,20*k+4))); }
    }
};
}
