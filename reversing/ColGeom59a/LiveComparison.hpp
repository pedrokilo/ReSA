#pragma once
// Banco dentro del motor: geometria pura de CCollision (lista 59a, 25 funciones): pruebas de solapamiento
// (esfera-esfera, esfera-caja, recta-caja, recta-esfera, recta-triangulo, esfera-triangulo, 2D recta-recta),
// distancias a rectas, punto en triangulo/poligono, puntos mas cercanos y distancia entre segmentos.
// Original (hooks de las 25 funciones desactivados: se llama al codigo del EXE) frente a candidato (hooks activos:
// el EXE salta a la DLL) con fixtures deterministas. Son funciones puras: se llaman por la direccion del EXE con
// buffers propios (CColLine 0x20, CBox 0x18, CColSphere 0x14, CColPoint 0x2C, CompressedVector de 6 bytes,
// CColTriangle de 8 bytes y CColTrianglePlane de 0xA) y se comparan el valor devuelto (float en ST0, bool en AL, int
// en EAX; NaN canonico), los buffers de salida a nivel de bytes (con byte centinela detras) y que las entradas no
// cambien. No se toca ningun estado global del juego: solo se guardan y restauran los estados de los 25 hooks.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace ColGeom59aLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 Fc(float f) { return std::isnan(f) ? 0x7FC00000u : std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,25> fns{{
    {"CCollision","TestSphereSphere",0x411E70,0,-1,1024},{"CCollision","CalculateColPointInsideBox",0x411EC0,0,-1,1024},{"CCollision","TestSphereBox",0x4120C0,0,-1,1024},
    {"CCollision","PointInTriangle",0x412700,0,-1,1024},{"CCollision","DistToLineSqr",0x412850,0,-1,1024},{"CCollision","DistToMathematicalLine",0x412970,0,-1,512},
    {"CCollision","DistToMathematicalLine2D",0x412A30,0,-1,512},{"CCollision","DistAlongLine2D",0x412A80,0,-1,256},{"CCollision","TestLineBox_DW",0x412C70,0,-1,1024},
    {"CCollision","TestLineBox",0x413070,0,-1,512},{"CCollision","TestVerticalLineBox",0x413080,0,-1,512},{"CCollision","Test2DLineAgainst2DLine",0x4138D0,0,-1,1024},
    {"CCollision","TestLineTriangle",0x413AC0,0,-1,1024},{"CCollision","GetBoundingBoxFromTwoSpheres",0x415230,0,-1,512},{"CCollision","RayPolyPOP",0x415620,0,-1,1024},
    {"CCollision","GetPrincipleAxis",0x4156D0,0,-1,512},{"CCollision","PointInPoly",0x415730,0,-1,1024},{"CCollision","Closest3",0x415950,0,-1,512},
    {"CCollision","ClosestSquaredDistanceBetweenFiniteLines",0x415A40,0,-1,1024},{"CCollision","TestSphereTriangle",0x4165B0,0,-1,1024},
    {"CCollision","TestLineSphere",0x417470,0,-1,1024},{"CCollision","DistToLine",0x417610,0,-1,512},{"CCollision","ClosestPointOnLine",0x417FD0,0,-1,1024},
    {"CCollision","ClosestPointsOnPoly",0x418100,0,-1,512},{"CCollision","ClosestPointOnPoly",0x418150,0,-1,512},
}};

using V3 = std::array<float,3>;
inline V3 Sub(const V3& a,const V3& b){ return {a[0]-b[0],a[1]-b[1],a[2]-b[2]}; }
inline V3 Add(const V3& a,const V3& b){ return {a[0]+b[0],a[1]+b[1],a[2]+b[2]}; }
inline V3 Mul(const V3& a,float k){ return {a[0]*k,a[1]*k,a[2]*k}; }
inline V3 Cross(const V3& a,const V3& b){ return {a[1]*b[2]-a[2]*b[1],a[2]*b[0]-a[0]*b[2],a[0]*b[1]-a[1]*b[0]}; }

// Generador determinista por caso: el "modo" fija el tipo de numeros (flotantes, enteros pequenos con muchos empates, medios, grandes, con ceros).
struct Gen {
    uint32 s{}; int mode{};
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    float c(float k=1.f){
        switch(mode){
        case 0: return uni(-10.f,10.f)*k;
        case 1: return float(int(next()%7)-3)*k;
        case 2: return float(int(next()%13)-6)*0.5f*k;
        case 3: return uni(-1000.f,1000.f)*k;
        case 4: return float(int(next()%3)-1)*k;
        default: { const uint32 r=next()%8; return r==0 ? -0.f : r<3 ? 0.f : uni(-5.f,5.f)*k; }
        }
    }
    V3 v(float k=1.f){ V3 r; r[0]=c(k); r[1]=c(k); r[2]=c(k); return r; }
    V3 vnz(){ V3 r; do r=v(); while(r[0]==0.f&&r[1]==0.f&&r[2]==0.f); return r; } // no nulo (para direcciones)
    float pos(){ return std::fabs(c()); }
};

// Triangulo de colision con su plano, como los guarda el EXE (vertices de 6 bytes /128, normal /4096, desplazamiento /128, orientacion).
struct TriFix {
    alignas(16) std::array<int16,24> verts{};
    alignas(16) std::array<uint8,8>  tri{};
    alignas(16) std::array<uint8,12> plane{};
    V3 A{},B{},C{},n{};
};
inline TriFix MakeTri(Gen& g) {
    TriFix t{};
    static constexpr int ranges[4]{64,256,1280,12800};
    const int R=ranges[g.next()%4];
    for(auto& v:t.verts)v=int16(int(g.next()%uint32(2*R+1))-R);
    const int ia=int(g.next()%8), ib=int((ia+1+g.next()%7)%8); int ic; do ic=int(g.next()%8); while(ic==ia||ic==ib);
    auto same=[&](int p,int q){ return t.verts[p*3]==t.verts[q*3]&&t.verts[p*3+1]==t.verts[q*3+1]&&t.verts[p*3+2]==t.verts[q*3+2]; };
    while(same(ia,ib))t.verts[ib*3]+=1;
    while(same(ia,ic)||same(ib,ic))t.verts[ic*3+1]+=1;
    const uint16 idx[3]{uint16(ia),uint16(ib),uint16(ic)}; std::memcpy(t.tri.data(),idx,6);
    auto vf=[&](int k){ return V3{float(t.verts[k*3])*0.0078125f,float(t.verts[k*3+1])*0.0078125f,float(t.verts[k*3+2])*0.0078125f}; };
    t.A=vf(ia); t.B=vf(ib); t.C=vf(ic);
    // normal exacta del triangulo (convenio del EXE: (C-A) x (B-A)), cuantizada como CColTrianglePlane
    const V3 cr=Cross(Sub(t.C,t.A),Sub(t.B,t.A)); const double len=std::sqrt(double(cr[0])*cr[0]+double(cr[1])*cr[1]+double(cr[2])*cr[2]);
    V3 nu{1.f,0.f,0.f}; if(len>1e-9)nu={float(cr[0]/len),float(cr[1]/len),float(cr[2]/len)};
    int16 q[3]; for(int k=0;k<3;++k)q[k]=int16(std::clamp(std::lround(nu[k]*4096.f),-4096l,4096l));
    t.n=nu;
    const V3 nq{float(q[0])*0.000244140625f,float(q[1])*0.000244140625f,float(q[2])*0.000244140625f};
    int16 off=int16(std::clamp(std::lround((t.A[0]*nq[0]+t.A[1]*nq[1]+t.A[2]*nq[2])*128.f),-32000l,32000l));
    const float ax=std::fabs(nq[0]),ay=std::fabs(nq[1]),az=std::fabs(nq[2]);
    uint8 orient=ax>ay&&ax>az ? (nq[0]<=0.f ? 1 : 0) : ay>az ? (nq[1]<=0.f ? 3 : 2) : (nq[2]<=0.f ? 5 : 4);
    if(g.next()%4==0){ // plano aleatorio (cualquier orientacion 0..5)
        for(auto& x:q)x=int16(int(g.next()%8193)-4096); off=int16(int(g.next()%uint32(2*R+1))-R); orient=uint8(g.next()%6);
    }
    std::memcpy(t.plane.data(),q,6); std::memcpy(t.plane.data()+6,&off,2); t.plane[8]=orient; t.plane[9]=0;
    return t;
}
// Punto del plano del triangulo: dentro con probabilidad ~60 %, el resto en un entorno alrededor.
inline V3 PointNearTri(Gen& g,const TriFix& t) {
    float u=g.uni(0.f,1.f), w=g.uni(0.f,1.f);
    if(g.next()%5<3){ if(u+w>1.f){u=1.f-u;w=1.f-w;} } else { u=g.uni(-0.6f,1.6f); w=g.uni(-0.6f,1.6f); }
    switch(g.next()%8){ case 0: u=0.f; break; case 1: w=0.f; break; case 2: u=w=0.f; break; case 3: u=1.f; w=0.f; break; default: break; }
    return Add(t.A,Add(Mul(Sub(t.B,t.A),u),Mul(Sub(t.C,t.A),w)));
}

inline json Exec(const Fn& fn,int i,size_t f,bool candidate) {
    const std::string nm=fn.name; const uintptr_t a=fn.address;
    Gen g; g.s=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x59A11u; if(!g.s)g.s=1; g.mode=int(g.next()%6);
    json out={{"switches",ReversingProbe::JumpsToDll(fn.address)==candidate}}; json ret=nullptr;
    auto put=[&](const char* k,const void* p,size_t n){ out[k]=ReversingProbe::Bytes(p,n); };
    // Buffers de entrada/salida (se rellenan con bytes de patron o aleatorios: lo que el EXE no escribe debe quedar igual)
    alignas(16) float L[8]{}, S1[8]{}, S2[8]{}, B6[8]{}, P[4]{}, Q[4]{}, T9[12]{}, N3[4]{}, O[16]{};
    auto setv=[&](float* d,const V3& v){ d[0]=v[0]; d[1]=v[1]; d[2]=v[2]; };

    if(nm=="TestSphereSphere") {
        V3 c1=g.v(), c2=g.v(); float r1=g.pos(), r2=g.pos();
        switch(g.next()%4){ case 0: c2=c1; c2[g.next()%3]+=r1+r2; break; case 1: c2=c1; c2[g.next()%3]-=r1+r2; break; case 2: c2=c1; break; default: break; }
        if(g.next()%8==0)r1=-r1;
        setv(S1,c1); S1[3]=r1; setv(S2,c2); S2[3]=r2; S1[4]=S2[4]=std::bit_cast<float>(0x01010101u);
        ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*)>(a)(S1,S2);
        put("s1",S1,20); put("s2",S2,20);
    }
    else if(nm=="CalculateColPointInsideBox") {
        V3 mn=g.v(), ext; for(int k=0;k<3;++k)ext[k]=g.next()%10==0 ? 0.f : g.pos();
        const V3 mx=Add(mn,ext); V3 p=Add(mn,Mul(ext,0.5f));
        switch(g.next()%5){ case 0: p=g.v(); break; case 1: p[g.next()%3]+=g.c(); break; case 2: p=Add(mn,{ext[0]*0.25f,ext[1]*0.25f,ext[2]*0.25f}); break; case 3: break; default: p=Add(p,g.v(0.25f)); break; }
        for(int k=0;k<6;++k)B6[k]=k<3 ? mn[k] : mx[k-3]; setv(P,p);
        alignas(16) std::array<uint8,0x30> cp{}; for(auto& b:cp)b=uint8(g.next());
        reinterpret_cast<void(__cdecl*)(const void*,const void*,void*)>(a)(B6,P,cp.data());
        put("cp",cp.data(),0x30); put("box",B6,24); put("pt",P,12);
        ret=Fnv(cp.data(),0x2C);
    }
    else if(nm=="TestSphereBox") {
        V3 mn=g.v(), ext; for(int k=0;k<3;++k)ext[k]=g.next()%10==0 ? 0.f : g.pos();
        const V3 mx=Add(mn,ext); V3 cen=Add(mn,Mul(ext,0.5f)); if(g.next()%2)cen=g.v(); float r=g.pos();
        if(g.next()%3==0){ const int ax=int(g.next()%3); cen=Add(mn,Mul(ext,0.5f)); cen[ax]=mx[ax]+r; if(g.next()%2)cen[ax]=mn[ax]-r; } // tangente exacta
        for(int k=0;k<6;++k)B6[k]=k<3 ? mn[k] : mx[k-3]; setv(S1,cen); S1[3]=r;
        ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*)>(a)(S1,B6);
        put("s",S1,16); put("box",B6,24);
    }
    else if(nm=="PointInTriangle") {
        V3 A=g.v(), Bv=g.v(), Cv=g.v();
        switch(g.next()%7){ case 0: Cv=Add(A,Mul(Sub(Bv,A),float(int(g.next()%5)-2))); break; case 1: Bv=A; break; case 2: A=Bv=Cv; break; default: break; } // degenerados
        float u=g.uni(0.f,1.f), w=g.uni(0.f,1.f); if(g.next()%2){ if(u+w>1.f){u=1.f-u;w=1.f-w;} } else { u=g.uni(-0.5f,1.5f); w=g.uni(-0.5f,1.5f); }
        switch(g.next()%7){ case 0: u=0.f; break; case 1: w=0.f; break; case 2: u=w=0.f; break; case 3: u=1.f; w=0.f; break; case 4: u=0.5f; w=0.5f; break; default: break; }
        V3 p=Add(A,Add(Mul(Sub(Bv,A),u),Mul(Sub(Cv,A),w))); if(g.next()%6==0)p=g.v();
        setv(T9,A); setv(T9+3,Bv); setv(T9+6,Cv); setv(P,p);
        ret=reinterpret_cast<bool(__stdcall*)(const void*,const void*)>(a)(P,T9);
        put("tri",T9,36); put("pt",P,12);
    }
    else if(nm=="DistToLineSqr"||nm=="DistToLine") {
        V3 l0=g.v(), l1=g.next()%12==0 ? l0 : g.v();
        const V3 d=Sub(l1,l0); static constexpr float ts[]{-1.f,0.f,0.25f,0.5f,1.f,1.5f,2.f};
        V3 p=Add(l0,Mul(d,ts[g.next()%7])); if(g.next()%3)p=Add(p,g.v(0.5f)); if(g.next()%8==0)p=g.v();
        setv(S1,l0); setv(S2,l1); setv(P,p);
        ret=Fc(reinterpret_cast<float(__cdecl*)(const void*,const void*,const void*)>(a)(S1,S2,P));
        put("l0",S1,12); put("l1",S2,12); put("pt",P,12);
    }
    else if(nm=="DistToMathematicalLine") {
        V3 l0=g.v(), l1; do l1=g.v(); while(l1==l0); // sin linea nula (0/0 da NaN)
        const V3 d=Sub(l1,l0); V3 p=Add(l0,Mul(d,float(int(g.next()%7)-2)*0.5f)); if(g.next()%3)p=Add(p,g.v(0.5f)); if(g.next()%8==0)p=g.v();
        setv(S1,l0); setv(S2,l1); setv(P,p);
        ret=Fc(reinterpret_cast<float(__cdecl*)(const void*,const void*,const void*)>(a)(S1,S2,P));
        put("l0",S1,12); put("l1",S2,12); put("pt",P,12);
    }
    else if(nm=="DistToMathematicalLine2D"||nm=="DistAlongLine2D") {
        const float sx=g.c(), sy=g.c(); float dx=g.c(), dy=g.c();
        if(g.next()%3==0){ const float k=g.c(); dx=k; dy=g.next()%2 ? 0.f : k; }
        float px=g.c(), py=g.c(); if(g.next()%3==0){ const float t=float(int(g.next()%5)-1); px=sx+dx*t; py=sy+dy*t; }
        ret=Fc(reinterpret_cast<float(__cdecl*)(float,float,float,float,float,float)>(a)(sx,sy,dx,dy,px,py));
    }
    else if(nm=="TestLineBox_DW"||nm=="TestLineBox"||nm=="TestVerticalLineBox") {
        V3 mn=g.v(), ext; for(int k=0;k<3;++k)ext[k]=g.next()%12==0 ? 0.f : g.pos();
        const V3 mx=Add(mn,ext); V3 s=g.v(), e=g.v();
        switch(g.next()%6) {
        case 0: s=Add(mn,{ext[0]*float(g.next()%3)*0.5f,ext[1]*float(g.next()%3)*0.5f,ext[2]*float(g.next()%3)*0.5f}); break; // sobre caras/aristas/centro
        case 1: e=Add(mn,{ext[0]*float(g.next()%3)*0.5f,ext[1]*float(g.next()%3)*0.5f,ext[2]*float(g.next()%3)*0.5f}); break;
        case 2: { const V3 cen=Add(mn,Mul(ext,0.5f)); const V3 d=g.vnz(); s=Sub(cen,Mul(d,2.f+g.pos())); e=Add(cen,Mul(d,2.f+g.pos())); break; } // atraviesa por el centro
        case 3: e=s; e[g.next()%3]=g.c(); break; // eje alineado
        case 4: if(nm=="TestVerticalLineBox"){ e=s; e[2]=g.c(); } else { s[g.next()%3]=mn[g.next()%3]; e[g.next()%3]=mx[g.next()%3]; } break;
        default: break; }
        if(nm=="TestVerticalLineBox"&&g.next()%2){ e[0]=s[0]; e[1]=s[1]; }
        for(int k=0;k<6;++k)B6[k]=k<3 ? mn[k] : mx[k-3]; setv(L,s); setv(L+4,e); L[3]=L[7]=std::bit_cast<float>(0x3F000001u);
        ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*)>(a)(L,B6);
        put("line",L,32); put("box",B6,24);
    }
    else if(nm=="Test2DLineAgainst2DLine") {
        float v[8]; for(auto& x:v)x=g.c();
        switch(g.next()%6) {
        case 0: { const float k=float(int(g.next()%5)-2); v[4]=v[0]+v[2]*0.5f; v[5]=v[1]+v[3]*0.5f; v[6]=v[2]*k; v[7]=v[3]*k; break; } // paralelas / colineales
        case 1: v[2]=v[3]=0.f; break; // linea 1 nula
        case 2: v[6]=v[7]=0.f; break; // linea 2 nula
        case 3: v[4]=v[0]; v[5]=v[1]; break; // mismo origen
        case 4: v[4]=v[0]+v[2]; v[5]=v[1]+v[3]; break; // toca el final
        default: break; }
        ret=reinterpret_cast<bool(__cdecl*)(float,float,float,float,float,float,float,float)>(a)(v[0],v[1],v[2],v[3],v[4],v[5],v[6],v[7]);
    }
    else if(nm=="TestLineTriangle") {
        const TriFix t=MakeTri(g); const V3 p=PointNearTri(g,t); V3 s,e;
        const V3 n=t.n; const float h1=g.uni(0.05f,4.f)*(g.next()%2 ? 1.f : -1.f), h2=g.uni(0.05f,4.f)*(g.next()%2 ? 1.f : -1.f);
        switch(g.next()%5) {
        case 0: case 1: s=Add(p,Mul(n,h1)); e=Sub(p,Mul(n,std::fabs(h2)*(h1<0.f ? -1.f : 1.f))); break; // cruza el plano
        case 2: s=Add(p,Mul(n,h1)); e=Add(p,Mul(n,h2)); break; // puede quedar del mismo lado
        case 3: s=Add(p,Mul(n,1.f+std::fabs(h1))); e=Add(s,Add(Mul(Sub(t.B,t.A),g.uni(-1.f,1.f)),Mul(Sub(t.C,t.A),g.uni(-1.f,1.f)))); break; // paralela al plano y alejada
        default: s=Add(t.A,Mul(g.v(),0.5f)); e=Add(t.B,Mul(g.v(),0.5f)); break; }
        setv(L,s); setv(L+4,e); L[3]=L[7]=0.f;
        ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,const void*,const void*)>(a)(L,t.verts.data(),t.tri.data(),t.plane.data());
        put("line",L,32);
    }
    else if(nm=="TestSphereTriangle") {
        const TriFix t=MakeTri(g); V3 p=PointNearTri(g,t); const float h=g.uni(-3.f,3.f)*(g.next()%3 ? 0.4f : 1.f);
        V3 cen=Add(p,Mul(t.n,h)); if(g.next()%6==0)cen=Add(t.A,Mul(g.v(),0.3f)); // cerca de un vertice
        float r=g.uni(0.02f,3.f); if(g.next()%2)r+=std::fabs(h);
        setv(S1,cen); S1[3]=r; S1[4]=0.f;
        ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,const void*,const void*)>(a)(S1,t.verts.data(),t.tri.data(),t.plane.data());
        put("s",S1,16);
    }
    else if(nm=="TestLineSphere") {
        V3 cen=g.v(); float r=g.pos(); V3 s=g.v(), e;
        switch(g.next()%7) {
        case 0: { const V3 d=g.vnz(); s=Sub(cen,Mul(d,2.f+g.pos()+r)); e=Add(cen,Mul(d,2.f+g.pos()+r)); break; } // atraviesa
        case 1: e=s; break; // nula
        case 2: e=Add(s,Mul(g.v(),1e-7f)); break; // casi nula (< 1e-6)
        case 3: { const V3 d=g.vnz(); s=Add(cen,Mul(d,0.1f)); e=Add(cen,Mul(d,5.f+r)); break; } // empieza dentro
        case 4: { V3 d{0.f,0.f,0.f}, pe{0.f,0.f,0.f}; const int ax=int(g.next()%3); d[ax]=1.f; pe[(ax+1)%3]=1.f; const V3 tp=Add(cen,Mul(d,r)); s=Add(tp,Mul(pe,2.f+g.pos())); e=Sub(tp,Mul(pe,2.f+g.pos())); break; } // tangente (mismo plano de contacto)
        default: e=g.v(); break; }
        setv(L,s); setv(L+4,e); L[3]=L[7]=0.f; setv(S1,cen); S1[3]=r; S1[4]=0.f;
        ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*)>(a)(L,S1);
        put("line",L,32); put("s",S1,16);
    }
    else if(nm=="GetBoundingBoxFromTwoSpheres") {
        V3 c1=g.v(), c2=g.v(); const float r1=g.pos(), r2=g.pos(); if(g.next()%3==0){ c2[g.next()%3]=c1[g.next()%3]; } if(g.next()%6==0)c2=c1;
        setv(S1,c1); S1[3]=r1; S1[4]=0.f; setv(S2,c2); S2[3]=r2; S2[4]=0.f;
        alignas(16) std::array<uint8,0x20> box{}; for(auto& b:box)b=uint8(g.next());
        reinterpret_cast<void(__cdecl*)(void*,const void*,const void*)>(a)(box.data(),S1,S2);
        put("box",box.data(),24); put("sA",S1,16); put("sB",S2,16); // el EXE no escribe el material/pieza/iluminacion (0x18..0x1B): no se compara
        ret=Fnv(box.data(),24);
    }
    else if(nm=="GetPrincipleAxis") {
        V3 n=g.v(); if(g.next()%4==0){ n[1]=n[0]; if(g.next()%2)n[2]=n[0]; } if(g.next()%6==0)n[g.next()%3]=-n[g.next()%3];
        setv(P,n);
        ret=reinterpret_cast<int32(__cdecl*)(const void*)>(a)(P);
        put("n",P,12);
    }
    else if(nm=="PointInPoly") {
        V3 A=g.v(), Bv=g.v(), Cv=g.v();
        V3 n=Cross(Sub(Cv,A),Sub(Bv,A)); if(g.next()%2)n=Mul(n,-1.f); if(g.next()%4==0)n=g.v(); if(g.next()%8==0){ n[1]=n[0]; if(g.next()%2)n[2]=n[0]; }
        float u=g.uni(0.f,1.f), w=g.uni(0.f,1.f); if(g.next()%2){ if(u+w>1.f){u=1.f-u;w=1.f-w;} } else { u=g.uni(-0.5f,1.5f); w=g.uni(-0.5f,1.5f); }
        switch(g.next()%7){ case 0: u=0.f; break; case 1: w=0.f; break; case 2: u=w=0.f; break; case 3: u=1.f; w=0.f; break; case 4: u=0.5f; w=0.5f; break; default: break; }
        V3 p=Add(A,Add(Mul(Sub(Bv,A),u),Mul(Sub(Cv,A),w))); if(g.next()%6==0)p=g.v();
        setv(T9,A); setv(T9+3,Bv); setv(T9+6,Cv); setv(P,p); setv(N3,n);
        alignas(16) std::array<uint8,8> tri{};
        ret=reinterpret_cast<bool(__cdecl*)(const void*,const void*,const void*,const void*)>(a)(P,tri.data(),N3,T9);
        put("tri",T9,36); put("pt",P,12); put("n",N3,12);
    }
    else if(nm=="RayPolyPOP") {
        V3 n=g.vnz(), pp=g.v(), dir=g.v(); V3 pos=g.v();
        switch(g.next()%5) {
        case 0: pos=Sub(pp,Mul(n,float(int(g.next()%3)+0)*0.5f)); break; // detras del plano o sobre el
        case 1: { const V3 d=Sub(pos,pp); const float dot=d[0]*n[0]+d[1]*n[1]+d[2]*n[2]; if(dot>0.f)pos=Sub(pos,Mul(n,2.f*dot/(n[0]*n[0]+n[1]*n[1]+n[2]*n[2]))); break; } // refleja a la parte trasera
        case 2: dir=Mul(n,-1.f); break; // hacia el plano
        case 3: dir=Mul(n,1.f); break; // alejandose
        default: break; }
        setv(S1,pos); setv(S2,dir); setv(N3,n); setv(P,pp); alignas(16) std::array<uint8,8> tri{};
        ret=reinterpret_cast<bool(__cdecl*)(void*,void*,void*,void*,void*)>(a)(S1,S2,tri.data(),N3,P);
        put("pos",S1,12); put("dir",S2,12); put("n",N3,12); put("pp",P,12);
    }
    else if(nm=="Closest3") {
        V3 A=g.v(), Bv=g.v(), Cv=g.v(); V3 p=g.v();
        switch(g.next()%5){ case 0: Bv=A; break; case 1: Cv=Bv; break; case 2: p=A; break; case 3: Cv=A; break; default: break; }
        setv(T9,A); setv(T9+3,Bv); setv(T9+6,Cv); setv(P,p);
        reinterpret_cast<void(__cdecl*)(void*,void*)>(a)(T9,P);
        put("tri",T9,36); put("pt",P,12); ret=Fnv(P,12);
    }
    else if(nm=="ClosestPointOnLine") {
        V3 l0=g.v(), l1; do l1=g.v(); while(l1==l0);
        const V3 d=Sub(l1,l0); static constexpr float ts[]{-1.f,0.f,0.25f,0.5f,1.f,1.5f,2.f};
        V3 p=Add(l0,Mul(d,ts[g.next()%7])); if(g.next()%3)p=Add(p,g.v(0.5f)); if(g.next()%8==0)p=g.v();
        setv(S1,l0); setv(S2,l1); setv(P,p); for(auto& x:O)x=std::bit_cast<float>(0xA5A5A5A5u);
        reinterpret_cast<void(__cdecl*)(const void*,const void*,const void*,void*)>(a)(P,S1,S2,O);
        put("out",O,16); put("l0",S1,12); put("l1",S2,12); put("pt",P,12); ret=Fnv(O,12);
    }
    else if(nm=="ClosestPointsOnPoly"||nm=="ClosestPointOnPoly") {
        V3 A=g.v(), Bv, Cv; do Bv=g.v(); while(Bv==A); do Cv=g.v(); while(Cv==A||Cv==Bv);
        V3 p=g.v(); if(g.next()%4==0)p=Add(A,Mul(Sub(Bv,A),0.5f)); if(g.next()%6==0)p=Add(p,Mul(Sub(Cv,Bv),1.5f));
        setv(T9,A); setv(T9+3,Bv); setv(T9+6,Cv); setv(P,p); alignas(16) std::array<uint8,8> tri{};
        if(nm=="ClosestPointsOnPoly") {
            alignas(16) std::array<float,12> o{}; for(auto& x:o)x=std::bit_cast<float>(0xA5A5A5A5u);
            reinterpret_cast<void(__cdecl*)(void*,void*,void*,void*)>(a)(tri.data(),T9,P,o.data());
            put("out",o.data(),48); ret=Fnv(o.data(),36);
        } else {
            reinterpret_cast<void(__cdecl*)(void*,void*,void*)>(a)(tri.data(),T9,P);
            ret=Fnv(P,12);
        }
        put("tri",T9,36); put("pt",P,12);
    }
    else if(nm=="ClosestSquaredDistanceBetweenFiniteLines") {
        V3 s1=g.v(), u=g.v(), s2=g.v(), e2=g.v();
        switch(g.next()%7) {
        case 0: e2=Add(s2,Mul(u,float(int(g.next()%5)-2)*0.5f)); break; // paralelos
        case 1: s2=Add(s1,Mul(u,0.5f)); e2=Add(s2,Mul(u,float(int(g.next()%3)-1))); break; // colineales
        case 2: u={0.f,0.f,0.f}; break; // segmento 1 nulo
        case 3: e2=s2; break; // segmento 2 nulo
        case 4: e2=Add(s2,Mul(u,1.f+1e-3f)); e2[0]+=1e-3f; break; // casi paralelos
        case 5: s2=Add(s1,Mul(u,g.uni(0.f,1.f))); e2=Add(s2,g.vnz()); break; // se cortan en el 1
        default: break; }
        const float aU=(u[0]*u[0]+u[1]*u[1])+u[2]*u[2];
        setv(S1,s1); setv(S2,s2); setv(P,e2); setv(N3,u);
        ret=Fc(reinterpret_cast<float(__cdecl*)(const void*,const void*,const void*,const void*,float)>(a)(S1,S2,P,N3,aU));
        put("s1",S1,12); put("s2",S2,12); put("e2",P,12); put("u",N3,12); out["a"]=Fc(aU);
    }
    else { out["unknown"]=nm; }
    out["ret"]=ret; return out;
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"colgeom59a-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"colgeom59a-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("colgeom59a");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"colgeom59a-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,25> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,25> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);
        Write({{"event","hook_state"},{"function",std::string(fns[f].cls)+"::"+fns[f].name},{"enabled",enabled[f]},{"locked",locked[f]}});}
    Write({{"event","fpu"},{"control_word",_control87(0,0)}});

    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string full=std::string(fn.cls)+"::"+fn.name;
        if(!only.empty()&&only!=full)continue;
        std::vector<json> res[2];
        for(int pass=0;pass<2;++pass) { // 0 = EXE (hooks fuera), 1 = DLL (hooks dentro); se conmutan los 25 hooks una vez por pasada
            for(auto* h:hooks)h->State(pass==1);
            res[pass].reserve(size_t(fn.cases));
            for(int i=0;i<fn.cases;++i)res[pass].push_back(Exec(fn,i,f,pass==1));
        }
        for(int i=0;i<fn.cases;++i) {
            const auto& original=res[0][size_t(i)]; const auto& candidate=res[1][size_t(i)];
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original["ret"]},{"candidate",candidate["ret"]}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    bool restored=true;
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];}
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
