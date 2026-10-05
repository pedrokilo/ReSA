#pragma once
// Banco dentro del motor: procesado de colision de CCollision (lista de 59b; 13 funciones). Original (hooks de las 13 funciones
// desactivados, codigo del EXE) frente a candidato con fixtures deterministas, todo en memoria propia (arena estatica):
//   - esferas, cajas, lineas, discos, triangulos con vertices comprimidos y planos calculados por el propio EXE (CColTrianglePlane::Set
//     en 0x411660) antes de cada llamada;
//   - CColModel / CCollisionData falsos (arrays contiguos, con o sin face groups, con o sin discos) y matrices de rotacion aleatorias;
//   - la cache de planos de triangulo (CCollision::ms_colModelCache en 0x96592C) se sustituye por una lista falsa de 3 a 8 enlaces
//     propios (los modelos falsos nunca se mezclan con los reales); se guarda entera (0x34 bytes) y se restaura al final.
// Se comparan el valor devuelto, los CColPoint completos (0x2C bytes: tambien los campos que el EXE deja sin escribir), los parametros de
// entrada/salida (distancias, banderas, CStoredCollPoly), el contador ms_iProcessLineNumCrossings (0x9655D0) y, en los modelos, una firma
// de los arrays y de los planos (los punteros de planos se enmascaran: solo cuenta que existan, su contenido y el indice de su enlace).
// No hay parches de entrada ni simulaciones: las funciones solo llaman a codigo de calculo, a CMemoryMgr::Malloc/Free (planos) y a
// g_surfaceInfos.IsSeeThrough (tabla real, solo lectura). Los planos reservados se liberan al terminar cada ejecucion.
// Fuera de los fixtures (comportamiento no definido o basura del EXE): ProcessLineBox con maxTouchDistance > 1 cuando el origen queda
// fuera de la caja y ningun plano acierta (el EXE devuelve verdadero con punto y normal sin inicializar), orientaciones de plano > 5,
// triangulos degenerados, NaN en las entradas y modelos con mas de 128 esferas, 64 cajas o 16 lineas.
#include <fstream>
#include <reversiblehooks/RootHookCategory.h>
#include <reversiblehooks/ReversibleHook/Base.h>
#include "Collision/Collision.h"
#include "Collision/ColModel.h"
#include "SurfaceInfos_c.h"
#include "PlayerPed.h"
#include "CutsceneMgr.h"
#include "../ProbeRuntime.hpp"

namespace ColProc59bLive {
inline bool done{}; inline std::ofstream log;
inline void Write(const json& r) { ReversingProbe::PumpMessages(); log<<r.dump()<<'\n'; log.flush(); }
inline uint32 F(float f) { return std::bit_cast<uint32>(f); }
inline uint64 Fnv(const void* p,size_t n,uint64 h=1469598103934665603ull){ auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){h^=b[i];h*=1099511628211ull;} return h; }
inline std::string Hex(const void* p,size_t n){ static const char* d="0123456789abcdef"; std::string s; s.reserve(n*2); auto* b=static_cast<const uint8*>(p); for(size_t i=0;i<n;++i){ s+=d[b[i]>>4]; s+=d[b[i]&15]; } return s; }
template<class T> inline T& At(void* base,size_t off){ return *reinterpret_cast<T*>(static_cast<uint8*>(base)+off); }
template<class T> inline T& At(uintptr_t addr){ return *reinterpret_cast<T*>(addr); }

constexpr float BIG=std::bit_cast<float>(0x6753C21Cu); // 1e24f del EXE
constexpr float PI_F=3.14159274f;
constexpr uintptr_t CACHE=0x96592C, CROSSINGS=0x9655D0;
inline uint32& Crossings(){ return At<uint32>(CROSSINGS); }
constexpr size_t CP_SIZE=0x2C;

// --- generador pseudoaleatorio determinista ---
struct Rng {
    uint32 s{1};
    uint32 next(){ s^=s<<13; s^=s>>17; s^=s<<5; return s; }
    float uni(float a,float b){ return a+(b-a)*float(next()%1000000)/1000000.f; }
    bool coin(uint32 n){ return next()%n==0; }          // 1 de cada n
    float pick(std::initializer_list<float> v){ return *(v.begin()+next()%v.size()); }
};
inline CVector RandVec(Rng& r,float a,float b){ const float x=r.uni(a,b), y=r.uni(a,b), z=r.uni(a,b); return CVector{x,y,z}; }
inline CVector RandUnit(Rng& r){ for(;;){ const CVector v=RandVec(r,-1.f,1.f); const float m=v.x*v.x+v.y*v.y+v.z*v.z;
    if(m>0.05f&&m<=1.f){ const float inv=1.f/std::sqrt(m); return CVector{v.x*inv,v.y*inv,v.z*inv}; } } }
inline CVector Cross3(const CVector& a,const CVector& b){ return CVector{a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x}; }
inline CVector Unit3(const CVector& v){ const float m=std::sqrt(v.x*v.x+v.y*v.y+v.z*v.z); const float inv=1.f/m; return CVector{v.x*inv,v.y*inv,v.z*inv}; }
inline CVector Lerp3(const CVector& a,const CVector& b,float t){ return CVector{a.x+(b.x-a.x)*t,a.y+(b.y-a.y)*t,a.z+(b.z-a.z)*t}; }

// --- arena propia (se pone a cero en cada reserva y se reinicia por ejecucion) ---
alignas(16) inline std::array<uint8,0x40000> arena{}; inline size_t arenaTop{};
inline uint8* Alloc(size_t n,size_t align=16){
    arenaTop=(arenaTop+align-1)&~(align-1); auto* p=arena.data()+arenaTop; arenaTop+=n;
    if(arenaTop>arena.size()) std::abort();
    std::memset(p,0,n); return p;
}
inline uint8* NewCp(){ auto* p=Alloc(CP_SIZE,4); std::memset(p,0xCD,CP_SIZE); return p; }
inline void Surface4(Rng& r,uint8* p,bool materialBelow179){ p[0]=uint8(materialBelow179 ? r.next()%179 : r.next()); p[1]=uint8(r.coin(3) ? r.next()%17 : r.next()); p[2]=uint8(r.next()); p[3]=uint8(r.next()); }

// --- triangulos con vertices comprimidos (1/128) y plano calculado por el EXE ---
struct TriFix { int16* verts{}; uint8* tri{}; uint8* plane{}; CVector va{},vb{},vc{},n{}; };
inline CVector Decomp(const int16* v){ return CVector{float(v[0])*0.0078125f,float(v[1])*0.0078125f,float(v[2])*0.0078125f}; }
inline void ComputePlane(uint8* plane,const int16* verts,uint8* tri){
    reinterpret_cast<void(__thiscall*)(void*,const void*,const void*)>(0x411660)(plane,verts,tri);
}
inline TriFix MakeTri(Rng& r,float scale){
    TriFix t; const int nV=6; t.verts=reinterpret_cast<int16*>(Alloc(nV*6,2)); t.tri=Alloc(8,2); t.plane=Alloc(10,2);
    const int range=int(scale*128.f);
    uint16 ia=0,ib=1,ic=2;
    for(int tries=0;tries<30;++tries){
        for(int k=0;k<nV*3;++k) t.verts[k]=int16(int(r.next()%uint32(2*range+1))-range);
        ia=uint16(r.next()%nV); do{ ib=uint16(r.next()%nV); }while(ib==ia); do{ ic=uint16(r.next()%nV); }while(ic==ia||ic==ib);
        const CVector a=Decomp(t.verts+ia*3), b=Decomp(t.verts+ib*3), c=Decomp(t.verts+ic*3);
        const CVector cr=Cross3(CVector{b.x-a.x,b.y-a.y,b.z-a.z},CVector{c.x-a.x,c.y-a.y,c.z-a.z});
        if(std::sqrt(cr.x*cr.x+cr.y*cr.y+cr.z*cr.z)>0.12f*scale*scale) break;
        if(tries==29){ t.verts[ia*3]=0; t.verts[ia*3+1]=0; t.verts[ia*3+2]=0; t.verts[ib*3]=int16(range); t.verts[ib*3+1]=0; t.verts[ib*3+2]=0;
                       t.verts[ic*3]=0; t.verts[ic*3+1]=int16(range); t.verts[ic*3+2]=int16(range/4); }
    }
    At<uint16>(t.tri,0)=ia; At<uint16>(t.tri,2)=ib; At<uint16>(t.tri,4)=ic; t.tri[6]=uint8(r.next()%179); t.tri[7]=uint8(r.next());
    t.va=Decomp(t.verts+ia*3); t.vb=Decomp(t.verts+ib*3); t.vc=Decomp(t.verts+ic*3);
    ComputePlane(t.plane,t.verts,t.tri);
    const int16* pl=reinterpret_cast<const int16*>(t.plane);
    t.n=CVector{float(pl[0])*0.000244140625f,float(pl[1])*0.000244140625f,float(pl[2])*0.000244140625f};
    return t;
}
inline CVector TriPoint(Rng& r,const TriFix& t){ // punto interior aleatorio (coordenadas baricentricas)
    float u=r.uni(0.02f,0.98f), v=r.uni(0.02f,0.98f); if(u+v>0.98f){ u=0.98f-u*0.5f; v=0.98f-v*0.5f; if(u+v>0.98f){ u=0.3f; v=0.3f; } }
    const float w=1.f-u-v; return CVector{t.va.x*w+t.vb.x*u+t.vc.x*v,t.va.y*w+t.vb.y*u+t.vc.y*v,t.va.z*w+t.vb.z*u+t.vc.z*v};
}

// --- matrices de rotacion aleatorias (CMatrix en bruto: 0x48 bytes, sin RwMatrix asociada) ---
inline void MakeMatrix(Rng& r,uint8* m,float posRange,bool tilt,bool identityRot=false){
    std::memset(m,0,0x48);
    CVector right{1.f,0.f,0.f}, fwd{0.f,1.f,0.f}, up{0.f,0.f,1.f};
    if(!identityRot){
        const float yaw=r.uni(0.f,2.f*PI_F), pitch=tilt ? r.uni(-0.35f,0.35f) : 0.f, roll=tilt ? r.uni(-0.35f,0.35f) : 0.f;
        const float cy=std::cos(yaw), sy=std::sin(yaw), cp=std::cos(pitch), sp=std::sin(pitch), cr=std::cos(roll), sr=std::sin(roll);
        const float R00=cy*cr-sy*sp*sr, R01=-sy*cp, R02=cy*sr+sy*sp*cr;
        const float R10=sy*cr+cy*sp*sr, R11=cy*cp,  R12=sy*sr-cy*sp*cr;
        const float R20=-cp*sr,         R21=sp,     R22=cp*cr;
        right=CVector{R00,R10,R20}; fwd=CVector{R01,R11,R21}; up=CVector{R02,R12,R22};
    }
    At<CVector>(m,0x00)=right; At<CVector>(m,0x10)=fwd; At<CVector>(m,0x20)=up; At<CVector>(m,0x30)=RandVec(r,-posRange,posRange);
}

// --- cache de planos falsa ---
inline std::array<CLink<CCollisionData*>,8> fakeLinks;
inline CLinkList<CCollisionData*>& Cache(){ return *reinterpret_cast<CLinkList<CCollisionData*>*>(CACHE); }
inline void InstallFakeCache(size_t n){
    auto& L=Cache(); std::memset(&L,0,sizeof(L));
    L.usedListHead.next=&L.usedListTail; L.usedListTail.prev=&L.usedListHead;
    L.freeListHead.next=&L.freeListTail; L.freeListTail.prev=&L.freeListHead;
    L.links=fakeLinks.data();
    for(auto& l:fakeLinks) std::memset(&l,0,sizeof(l));
    for(int i=int(n)-1;i>=0;--i) fakeLinks[size_t(i)].Insert(&L.freeListHead);
}
inline int LinkIdx(const CLink<CCollisionData*>* p){
    auto& L=Cache();
    if(p>=fakeLinks.data()&&p<fakeLinks.data()+fakeLinks.size()) return int(p-fakeLinks.data());
    if(p==&L.usedListHead) return -2; if(p==&L.usedListTail) return -3; if(p==&L.freeListHead) return -4; if(p==&L.freeListTail) return -5;
    return -9;
}
// Firma de la cache: para cada enlace, su indice, el de sus vecinos y el modelo al que apunta; mas las cadenas desde las cabezas.
inline uint64 CacheSig(CCollisionData* const* cds,int k){
    uint64 h=1469598103934665603ull; auto& L=Cache();
    auto mix=[&](int64_t v){ h=Fnv(&v,8,h); };
    auto owner=[&](const CCollisionData* d){ for(int i=0;i<k;++i) if(cds[i]==d) return i; return d ? -7 : -1; };
    for(size_t i=0;i<fakeLinks.size();++i){ const auto& l=fakeLinks[i]; mix(LinkIdx(&l)); mix(LinkIdx(l.prev)); mix(LinkIdx(l.next)); mix(owner(l.data)); }
    auto chain=[&](const CLink<CCollisionData*>* from,const CLink<CCollisionData*>* stop){ int n=0; for(auto* p=from;p!=stop&&n<20;p=p->next,++n) mix(LinkIdx(p)); mix(n); };
    chain(L.usedListHead.next,&L.usedListTail); chain(L.freeListHead.next,&L.freeListTail);
    mix(LinkIdx(L.usedListHead.next)); mix(LinkIdx(L.usedListTail.prev)); mix(LinkIdx(L.freeListHead.next)); mix(LinkIdx(L.freeListTail.prev));
    return h;
}
// Firma de un CCollisionData: cabecera (con el puntero de planos enmascarado), arrays y planos; devuelve tambien si tiene planos.
struct Model {
    uint8* cm{}; CCollisionData* cd{}; int nS{},nB{},nT{},nL{},nV{},nG{}; bool disks{},fg{};
    uint8* spheres{}; uint8* boxes{}; uint8* lines{}; int16* verts{}; uint8* tris{}; uint8* fgBase{}; size_t fgBytes{};
};
inline uint64 ModelSig(const Model& m){
    uint8 head[0x30]; std::memcpy(head,m.cd,0x30);
    const bool hasPlanes=m.cd->m_pTrianglePlanes!=nullptr; std::memset(head+0x1C,0,4);
    uint64 h=Fnv(head,0x30); const uint8 hp=hasPlanes; h=Fnv(&hp,1,h);
    if(m.nS) h=Fnv(m.spheres,size_t(m.nS)*0x14,h);
    if(m.nB) h=Fnv(m.boxes,size_t(m.nB)*0x1C,h);
    if(m.nL) h=Fnv(m.lines,size_t(m.nL)*(m.disks ? 0x24 : 0x20),h);
    if(m.nV) h=Fnv(m.verts,size_t(m.nV)*6,h);
    if(m.nT){ h=Fnv(m.tris,size_t(m.nT)*8,h); if(m.fg) h=Fnv(m.fgBase,m.fgBytes,h); }
    if(hasPlanes&&m.nT){
        /* El byte +9 de cada plano (relleno) no lo escribe CColTrianglePlane::Set del EXE: es basura del heap, se enmascara. */
        for(int i=0;i<m.nT;++i) h=Fnv(reinterpret_cast<const uint8*>(m.cd->m_pTrianglePlanes)+size_t(i)*10,9,h);
        auto* link=*reinterpret_cast<CLink<CCollisionData*>**>((reinterpret_cast<uintptr_t>(m.cd->m_pTrianglePlanes)+size_t(m.nT)*10+3)&~uintptr_t(3));
        const int li=LinkIdx(link); h=Fnv(&li,4,h);
    }
    return h;
}
inline void ReleasePlanes(const Model& m){ if(m.cd->m_pTrianglePlanes) m.cd->RemoveTrianglePlanes(); }

// --- modelos de colision falsos ---
inline Model BuildModel(Rng& r,int maxS,int maxB,int maxT,int maxL,bool allowDisk,bool allowFG,float scale){
    Model m;
    m.nS=int(r.next()%uint32(maxS+1)); m.nB=int(r.next()%uint32(maxB+1)); m.nT=int(r.next()%uint32(maxT+1)); m.nL=int(r.next()%uint32(maxL+1));
    m.disks=allowDisk&&m.nL&&r.coin(3); m.fg=allowFG&&m.nT>=2&&r.coin(3);
    m.nV=m.nT ? 4+int(r.next()%5) : 0;
    m.cm=Alloc(0x30); m.cd=reinterpret_cast<CCollisionData*>(Alloc(0x30));
    CVector lo{1e9f,1e9f,1e9f}, hi{-1e9f,-1e9f,-1e9f};
    auto grow=[&](const CVector& p,float pad){ lo.x=std::min(lo.x,p.x-pad); lo.y=std::min(lo.y,p.y-pad); lo.z=std::min(lo.z,p.z-pad);
                                              hi.x=std::max(hi.x,p.x+pad); hi.y=std::max(hi.y,p.y+pad); hi.z=std::max(hi.z,p.z+pad); };
    if(m.nS){ m.spheres=Alloc(size_t(m.nS)*0x14); for(int i=0;i<m.nS;++i){ auto* s=m.spheres+i*0x14; const CVector c=RandVec(r,-scale,scale); const float rad=r.uni(0.2f,0.9f)*scale*0.6f;
        At<CVector>(s,0)=c; At<float>(s,0xC)=rad; Surface4(r,s+0x10,true); grow(c,rad); } }
    if(m.nB){ m.boxes=Alloc(size_t(m.nB)*0x1C); for(int i=0;i<m.nB;++i){ auto* b=m.boxes+i*0x1C; const CVector mn=RandVec(r,-scale,scale*0.5f);
        const CVector mx{mn.x+r.uni(0.2f,1.2f)*scale,mn.y+r.uni(0.2f,1.2f)*scale,mn.z+r.uni(0.2f,1.2f)*scale};
        At<CVector>(b,0)=mn; At<CVector>(b,0xC)=mx; Surface4(r,b+0x18,true); grow(mn,0.f); grow(mx,0.f); } }
    if(m.nL){
        if(m.disks){ m.lines=Alloc(size_t(m.nL)*0x24); for(int i=0;i<m.nL;++i){ auto* d=m.lines+i*0x24; const CVector c=RandVec(r,-scale*0.7f,scale*0.7f); const float rad=r.uni(0.25f,0.7f)*scale*0.5f;
            At<CVector>(d,0)=c; At<float>(d,0xC)=rad; Surface4(r,d+0x10,true);
            At<CVector>(d,0x14)=r.coin(3) ? CVector{0.f,1.f,0.f} : r.coin(2) ? CVector{1.f,0.f,0.f} : RandUnit(r); At<float>(d,0x20)=r.uni(0.05f,0.4f)*scale*0.5f; grow(c,rad); } }
        else { m.lines=Alloc(size_t(m.nL)*0x20); for(int i=0;i<m.nL;++i){ auto* l=m.lines+i*0x20; std::memset(l,0xCD,0x20);
            const CVector a=RandVec(r,-scale,scale), b=RandVec(r,-scale,scale); At<CVector>(l,0)=a; At<CVector>(l,0x10)=b; grow(a,0.f); grow(b,0.f); } }
    }
    if(m.nT){
        m.verts=reinterpret_cast<int16*>(Alloc(size_t(m.nV)*6,2));
        const int range=int(scale*128.f);
        for(int k=0;k<m.nV*3;++k) m.verts[k]=int16(int(r.next()%uint32(2*range+1))-range);
        for(int k=0;k<m.nV;++k) grow(Decomp(m.verts+k*3),0.f);
        if(m.fg){ m.nG=1+int(r.next()%uint32(std::min(3,m.nT))); m.fgBytes=size_t(m.nG)*0x1C+4; }
        m.fgBase=Alloc(m.fgBytes+size_t(m.nT)*8,4); m.tris=m.fgBase+m.fgBytes;
        for(int i=0;i<m.nT;++i){ auto* t=m.tris+i*8; uint16 ia,ib,ic;
            for(int tries=0;;++tries){ ia=uint16(r.next()%m.nV); do{ ib=uint16(r.next()%m.nV); }while(ib==ia); do{ ic=uint16(r.next()%m.nV); }while(ic==ia||ic==ib);
                const CVector a=Decomp(m.verts+ia*3), b=Decomp(m.verts+ib*3), c=Decomp(m.verts+ic*3);
                const CVector cr=Cross3(CVector{b.x-a.x,b.y-a.y,b.z-a.z},CVector{c.x-a.x,c.y-a.y,c.z-a.z});
                if(std::sqrt(cr.x*cr.x+cr.y*cr.y+cr.z*cr.z)>0.1f*scale*scale||tries>40) break; }
            At<uint16>(t,0)=ia; At<uint16>(t,2)=ib; At<uint16>(t,4)=ic; t[6]=uint8(r.next()%179); t[7]=uint8(r.next()); }
        if(m.fg){
            *reinterpret_cast<uint32*>(m.tris-4)=uint32(m.nG);
            // trozos contiguos de triangulos repartidos entre los grupos en orden aleatorio (el orden del EXE: grupo g en tris - 0x20 - g * 0x1C)
            std::array<int,4> cut{}; cut[0]=0; for(int g=1;g<m.nG;++g) cut[size_t(g)]=cut[size_t(g-1)]+1+int(r.next()%uint32(std::max(1,m.nT-cut[size_t(g-1)]-(m.nG-g))));
            cut[size_t(m.nG)]=m.nT;
            std::array<int,3> order{0,1,2}; for(int i=m.nG-1;i>0;--i) std::swap(order[size_t(i)],order[r.next()%uint32(i+1)]);
            for(int g=0;g<m.nG;++g){
                const int part=order[size_t(g)]; const int first=cut[size_t(part)], last=cut[size_t(part)+1]-1;
                CVector gl{1e9f,1e9f,1e9f}, gh{-1e9f,-1e9f,-1e9f};
                for(int t=first;t<=last;++t) for(int c=0;c<3;++c){ const CVector p=Decomp(m.verts+At<uint16>(m.tris+t*8,size_t(c)*2)*3);
                    gl.x=std::min(gl.x,p.x); gl.y=std::min(gl.y,p.y); gl.z=std::min(gl.z,p.z); gh.x=std::max(gh.x,p.x); gh.y=std::max(gh.y,p.y); gh.z=std::max(gh.z,p.z); }
                auto* grp=m.tris-0x20-size_t(g)*0x1C;
                At<CVector>(grp,0)=CVector{gl.x-0.05f,gl.y-0.05f,gl.z-0.05f}; At<CVector>(grp,0xC)=CVector{gh.x+0.05f,gh.y+0.05f,gh.z+0.05f};
                At<uint16>(grp,0x18)=uint16(first); At<uint16>(grp,0x1A)=uint16(last);
            }
        }
    }
    if(m.nS==0&&m.nB==0&&m.nL==0&&m.nT==0){ lo=CVector{-0.5f,-0.5f,-0.5f}; hi=CVector{0.5f,0.5f,0.5f}; }
    const float margin=r.uni(0.f,0.4f);
    const CVector mn{lo.x-margin,lo.y-margin,lo.z-margin}, mx{hi.x+margin,hi.y+margin,hi.z+margin};
    const CVector ctr{(mn.x+mx.x)*0.5f,(mn.y+mx.y)*0.5f,(mn.z+mx.z)*0.5f};
    const float rad=std::sqrt((mx.x-ctr.x)*(mx.x-ctr.x)+(mx.y-ctr.y)*(mx.y-ctr.y)+(mx.z-ctr.z)*(mx.z-ctr.z))+r.uni(0.f,0.3f);
    At<CVector>(m.cm,0)=mn; At<CVector>(m.cm,0xC)=mx; At<CVector>(m.cm,0x18)=ctr; At<float>(m.cm,0x24)=rad; At<CCollisionData*>(m.cm,0x2C)=m.cd;
    // cabecera de CCollisionData en bruto
    At<uint16>(m.cd,0)=uint16(m.nS); At<uint16>(m.cd,2)=uint16(m.nB); At<uint16>(m.cd,4)=uint16(m.nT); At<uint8>(m.cd,6)=uint8(m.nL);
    At<uint8>(m.cd,7)=uint8((m.disks ? 1 : 0)|(m.fg ? 2 : 0));
    At<void*>(m.cd,8)=m.spheres; At<void*>(m.cd,0xC)=m.boxes; At<void*>(m.cd,0x10)=m.lines; At<void*>(m.cd,0x14)=m.verts;
    At<void*>(m.cd,0x18)=m.nT ? m.tris : nullptr; At<void*>(m.cd,0x1C)=nullptr;
    return m;
}

inline ReversibleHooks::ReversibleHook::Base* FindHook(const char* cls,const char* fn) {
    ReversibleHooks::ReversibleHook::Base* r{};
    ReversibleHooks::GetRootCategory().ForEachCategory([&](auto& c){if(c.Name()==cls)for(auto& h:c.Items())if(h->Name()==fn)r=h.get();});
    return r;
}
struct Fn { const char* cls; const char* name; uintptr_t address; uintptr_t vt; int slot; int cases; };
inline const std::array<Fn,13> fns{{
    {"CCollision","ProcessSphereBox",0x412130,0,-1,640},{"CCollision","ProcessLineSphere",0x412AA0,0,-1,512},
    {"CCollision","ProcessLineBox",0x413100,0,-1,640},{"CCollision","ProcessDiscCollision",0x413960,0,-1,512},
    {"CCollision","ProcessLineTriangle",0x4140F0,0,-1,640},{"CCollision","ProcessVerticalLineTriangle",0x4147E0,0,-1,640},
    {"CCollision","IsStoredPolyStillValidVerticalLine",0x414D70,0,-1,512},{"CCollision","CalculateTrianglePlanes-colData",0x416330,0,-1,256},
    {"CCollision","RemoveTrianglePlanes-colData",0x416400,0,-1,256},{"CCollision","ProcessSphereSphere",0x416450,0,-1,512},
    {"CCollision","ProcessSphereTriangle",0x416BA0,0,-1,1024},{"CCollision","ProcessVerticalLine",0x417BF0,0,-1,512},
    {"CCollision","ProcessColModels",0x4185C0,0,-1,1536},
}};

using Out=json;
// ---------------------------------------------------------------------------------------------------------------------------------
// Fixtures y llamadas por funcion. Cada una rellena `out` (lo que se compara) y `det` (solo para el informe de un fallo).
// ---------------------------------------------------------------------------------------------------------------------------------
inline void RunSphereBox(Rng& r,uintptr_t a,Out& out,json& det){
    auto* sph=Alloc(0x14,4); auto* box=Alloc(0x1C,4); auto* cp=NewCp();
    CVector mn=RandVec(r,-5.f,5.f); CVector sz{r.uni(0.2f,5.f),r.uni(0.2f,5.f),r.uni(0.2f,5.f)};
    if(r.coin(25)) sz.x=0.f; if(r.coin(25)) sz.y=0.f; if(r.coin(25)) sz.z=0.f;
    float rad=r.coin(3) ? r.pick({0.25f,0.5f,1.f,1.5f,2.f,3.f}) : r.uni(0.05f,4.f);
    CVector mx{mn.x+sz.x,mn.y+sz.y,mn.z+sz.z}; CVector c{};
    switch(r.next()%10){
        case 0: case 1: case 2: c=CVector{r.uni(mn.x,mx.x),r.uni(mn.y,mx.y),r.uni(mn.z,mx.z)}; break;                                  // dentro
        case 3: case 4: case 5: c=CVector{r.uni(mn.x-rad*1.2f,mx.x+rad*1.2f),r.uni(mn.y-rad*1.2f,mx.y+rad*1.2f),r.uni(mn.z-rad*1.2f,mx.z+rad*1.2f)}; break; // cerca
        case 6: c=CVector{r.coin(2) ? mn.x : mx.x,r.uni(mn.y,mx.y),r.uni(mn.z,mx.z)}; if(r.coin(2)) std::swap(c.x,c.y); break;          // sobre un plano limite
        case 7: { // roce exacto de una cara: distancia == radio (valores exactos)
            mn=CVector{0.f,0.f,0.f}; mx=CVector{r.pick({1.f,2.f,3.f}),r.pick({1.f,2.f}),r.pick({1.f,2.f})}; rad=r.pick({0.5f,1.f,2.f});
            c=CVector{r.uni(mn.x,mx.x),r.uni(mn.y,mx.y),r.uni(mn.z,mx.z)};
            const uint32 ax=r.next()%3; const bool hiSide=r.coin(2); float* cc=&c.x; const float* lo=&mn.x; const float* hi=&mx.x;
            cc[ax]=hiSide ? hi[ax]+rad : lo[ax]-rad; break; }
        case 8: c=RandVec(r,-12.f,12.f); break;
        default: c=RandVec(r,-6.f,6.f); break;
    }
    float minDist=r.pick({BIG,BIG,r.uni(0.f,25.f),0.f,1.f,4.f,rad*rad});
    At<CVector>(sph,0)=c; At<float>(sph,0xC)=rad; Surface4(r,sph+0x10,false);
    At<CVector>(box,0)=mn; At<CVector>(box,0xC)=mx; Surface4(r,box+0x18,false);
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CColSphere&,const CColBox&,CColPoint&,float&)>(a)(*reinterpret_cast<CColSphere*>(sph),*reinterpret_cast<CColBox*>(box),*reinterpret_cast<CColPoint*>(cp),minDist);
    out["ret"]=ret; out["min"]=F(minDist); out["cp"]=Hex(cp,CP_SIZE);
}
inline void RunLineSphere(Rng& r,uintptr_t a,Out& out,json& det){
    auto* line=Alloc(0x20,4); std::memset(line,0xCD,0x20); auto* sph=Alloc(0x14,4); auto* cp=NewCp();
    const CVector s=RandVec(r,-6.f,6.f); const CVector dir=RandUnit(r); const float len=r.coin(12) ? 0.f : r.uni(0.3f,8.f);
    const CVector e{s.x+dir.x*len,s.y+dir.y*len,s.z+dir.z*len}; CVector c{};
    switch(r.next()%10){
        case 6: c=CVector{s.x+r.uni(-0.4f,0.4f),s.y+r.uni(-0.4f,0.4f),s.z+r.uni(-0.4f,0.4f)}; break;       // origen dentro
        case 7: c=CVector{e.x+r.uni(-0.4f,0.4f),e.y+r.uni(-0.4f,0.4f),e.z+r.uni(-0.4f,0.4f)}; break;       // extremo dentro
        case 8: { const float t0=r.uni(-0.6f,-0.05f); const CVector p=Lerp3(s,e,t0); const CVector o=RandUnit(r); const float d=r.uni(0.f,1.2f); c=CVector{p.x+o.x*d,p.y+o.y*d,p.z+o.z*d}; break; } // detras
        case 9: c=RandVec(r,-7.f,7.f); break;
        default: { const float t0=r.uni(-0.2f,1.3f); const CVector p=Lerp3(s,e,t0); const CVector o=RandUnit(r); const float d=r.uni(0.f,1.6f); c=CVector{p.x+o.x*d,p.y+o.y*d,p.z+o.z*d}; break; }
    }
    const float rad=r.uni(0.2f,2.2f); float depth=r.pick({1.f,r.uni(0.1f,1.f),2.f,BIG,0.5f,0.f});
    At<CVector>(line,0)=s; At<CVector>(line,0x10)=e; At<CVector>(sph,0)=c; At<float>(sph,0xC)=rad; Surface4(r,sph+0x10,false);
    Crossings()=7u;
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CColLine&,const CColSphere&,CColPoint&,float&)>(a)(*reinterpret_cast<CColLine*>(line),*reinterpret_cast<CColSphere*>(sph),*reinterpret_cast<CColPoint*>(cp),depth);
    out["ret"]=ret; out["depth"]=F(depth); out["cp"]=Hex(cp,CP_SIZE); out["crossings"]=Crossings();
}
inline void RunLineBox(Rng& r,uintptr_t a,Out& out,json& det){
    auto* line=Alloc(0x20,4); std::memset(line,0xCD,0x20); auto* box=Alloc(0x1C,4); auto* cp=NewCp();
    const CVector mn=RandVec(r,-4.f,4.f); const CVector mx{mn.x+r.uni(0.3f,4.f),mn.y+r.uni(0.3f,4.f),mn.z+r.uni(0.3f,4.f)};
    CVector s{}, e{};
    const uint32 mode=r.next()%10;
    if(mode<3) s=CVector{r.uni(mn.x,mx.x),r.uni(mn.y,mx.y),r.uni(mn.z,mx.z)};                  // origen dentro
    else s=RandVec(r,-8.f,8.f);
    const CVector ctr{(mn.x+mx.x)*0.5f,(mn.y+mx.y)*0.5f,(mn.z+mx.z)*0.5f};
    if(mode<2){ e=CVector{r.uni(mn.x,mx.x),r.uni(mn.y,mx.y),r.uni(mn.z,mx.z)}; }               // extremo dentro
    else { const CVector tgt{ctr.x+r.uni(-1.5f,1.5f),ctr.y+r.uni(-1.5f,1.5f),ctr.z+r.uni(-1.5f,1.5f)}; const float k=r.uni(1.0f,2.4f); e=Lerp3(s,tgt,k); }
    if(mode==8){ s.x=r.coin(2) ? mn.x : mx.x; }                                                   // origen justo sobre un plano
    if(mode==9){ e.y=s.y; if(r.coin(2)) e.z=s.z; }                                                // paralela a una cara
    const bool startInside=s.x>mn.x&&s.y>mn.y&&s.z>mn.z&&s.x<mx.x&&s.y<mx.y&&s.z<mx.z;
    float maxT=r.pick({1.f,0.9f,0.5f,0.1f,0.99f,0.3f});
    if(startInside) maxT=r.pick({1.f,2.f,0.5f,0.f,BIG,0.3f});
    At<CVector>(line,0)=s; At<CVector>(line,0x10)=e; At<CVector>(box,0)=mn; At<CVector>(box,0xC)=mx; Surface4(r,box+0x18,false);
    Crossings()=7u;
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CColLine&,const CColBox&,CColPoint&,float&)>(a)(*reinterpret_cast<CColLine*>(line),*reinterpret_cast<CColBox*>(box),*reinterpret_cast<CColPoint*>(cp),maxT);
    out["ret"]=ret; out["max"]=F(maxT); out["cp"]=Hex(cp,CP_SIZE); out["crossings"]=Crossings();
}
inline void RunDisc(Rng& r,uintptr_t a,Out& out,json& det){
    auto* mat=Alloc(0x48,16); auto* tri=NewCp(); auto* disk=Alloc(0x24,4); auto* dcp=NewCp(); auto* lcp=NewCp(); auto* flag=Alloc(4,4); auto* ratio=Alloc(4,4);
    const bool slab=r.coin(3);                                  // fixture que cae en la rama de la linea (normal casi perpendicular al grosor, cerca del plano)
    MakeMatrix(r,mat,2.f,r.coin(2),slab||r.coin(3));
    if(slab) At<CVector>(mat,0x30)=RandVec(r,-0.3f,0.3f);
    const CVector center=RandVec(r,-2.f,2.f); const float rad=r.uni(0.3f,1.5f); const float thick=r.uni(0.05f,0.7f);
    CVector t=r.coin(3) ? CVector{0.f,0.f,1.f} : r.coin(2) ? CVector{0.f,1.f,0.f} : RandUnit(r);
    At<CVector>(disk,0)=center; At<float>(disk,0xC)=rad; Surface4(r,disk+0x10,false); disk[0x11]=uint8(r.coin(4) ? 17+r.next()%200 : r.next()%17);
    At<CVector>(disk,0x14)=t; At<float>(disk,0x20)=thick;
    CVector pt=RandVec(r,-3.f,3.f), nrm=RandUnit(r);
    if(slab){ const CVector o=RandUnit(r); nrm=Unit3(Cross3(t,o)); const CVector tt=CVector{t.x*r.uni(-0.5f,0.5f)*thick,t.y*r.uni(-0.5f,0.5f)*thick,t.z*r.uni(-0.5f,0.5f)*thick};
              const float d=r.uni(-0.5f,0.5f); pt=CVector{center.x+nrm.x*d+tt.x-At<float>(mat,0x30),center.y+nrm.y*d+tt.y-At<float>(mat,0x34),center.z+nrm.z*d+tt.z-At<float>(mat,0x38)}; }
    At<CVector>(tri,0)=pt; At<CVector>(tri,0x10)=nrm; Surface4(r,tri+0x20,false); tri[0x23]=uint8(r.next()); At<float>(tri,0x28)=r.uni(-1.f,2.f);
    At<float>(dcp,0x28)=r.coin(5) ? At<float>(tri,0x28) : r.uni(-1.f,1.5f);
    At<uint8>(flag,0)=uint8(r.coin(2)); At<float>(ratio,0)=r.coin(4) ? -1e8f : r.uni(-3.f,2.f);
    bool& lineFlag=*reinterpret_cast<bool*>(flag);
    const bool ret=reinterpret_cast<bool(__cdecl*)(CColPoint&,const CMatrix&,const CColDisk&,CColPoint&,bool&,float&,CColPoint&)>(a)(
        *reinterpret_cast<CColPoint*>(tri),*reinterpret_cast<CMatrix*>(mat),*reinterpret_cast<CColDisk*>(disk),*reinterpret_cast<CColPoint*>(dcp),lineFlag,At<float>(ratio,0),*reinterpret_cast<CColPoint*>(lcp));
    out["ret"]=ret; out["flag"]=At<uint8>(flag,0); out["ratio"]=F(At<float>(ratio,0)); out["tri"]=Hex(tri,CP_SIZE); out["disk"]=Hex(dcp,CP_SIZE); out["line"]=Hex(lcp,CP_SIZE);
}
inline void RunLineTriangle(Rng& r,uintptr_t a,Out& out,json& det,bool vertical){
    auto t=MakeTri(r,r.pick({2.f,4.f,6.f})); auto* line=Alloc(0x20,4); std::memset(line,0xCD,0x20); auto* cp=NewCp(); auto* poly2=Alloc(0x2C,4); std::memset(poly2,0xCD,0x2C);
    const CVector P=TriPoint(r,t); const CVector jit=r.coin(2) ? CVector{0.f,0.f,0.f} : RandVec(r,-0.9f,0.9f);
    const float h1=r.uni(0.1f,2.5f)*(r.coin(6) ? -1.f : 1.f), h2=r.uni(0.1f,2.5f)*(r.coin(6) ? -1.f : 1.f);
    CVector s{}, e{};
    if(vertical){
        const float x=P.x+(r.coin(2) ? jit.x*0.3f : jit.x), y=P.y+(r.coin(2) ? jit.y*0.3f : jit.y);
        s=CVector{x,y,P.z+h1}; e=CVector{x,y,P.z-h2};
        if(r.coin(8)){ e.x=x+r.uni(-0.3f,0.3f); } // no del todo vertical
    } else {
        const CVector j2=RandVec(r,-0.9f,0.9f);
        s=CVector{P.x+t.n.x*h1+jit.x,P.y+t.n.y*h1+jit.y,P.z+t.n.z*h1+jit.z}; e=CVector{P.x-t.n.x*h2+j2.x,P.y-t.n.y*h2+j2.y,P.z-t.n.z*h2+j2.z};
        if(r.coin(12)) s=P; // origen justo en el plano
    }
    float maxT=r.pick({1.f,2.f,BIG,0.5f,r.uni(0.05f,1.5f)});
    At<CVector>(line,0)=s; At<CVector>(line,0x10)=e;
    CStoredCollPoly* cpoly=r.coin(3) ? nullptr : reinterpret_cast<CStoredCollPoly*>(poly2);
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CColLine&,const CompressedVector*,const CColTriangle&,const CColTrianglePlane&,CColPoint&,float&,CStoredCollPoly*)>(a)(
        *reinterpret_cast<CColLine*>(line),reinterpret_cast<const CompressedVector*>(t.verts),*reinterpret_cast<CColTriangle*>(t.tri),*reinterpret_cast<CColTrianglePlane*>(t.plane),*reinterpret_cast<CColPoint*>(cp),maxT,cpoly);
    out["ret"]=ret; out["max"]=F(maxT); out["cp"]=Hex(cp,CP_SIZE); out["poly"]=cpoly ? Hex(poly2,0x2C) : std::string("null");
    det["plane"]=Hex(t.plane,10); det["line"]=Hex(line,0x20);
}
inline void RunIsStoredPoly(Rng& r,uintptr_t a,Out& out,json& det){
    auto* poly=Alloc(0x2C,4); std::memset(poly,0xCD,0x2C); auto* cp=NewCp();
    CVector v0=RandVec(r,-6.f,6.f), v1{}, v2{};
    for(int tries=0;;++tries){ v1=RandVec(r,-6.f,6.f); v2=RandVec(r,-6.f,6.f);
        const CVector cr=Cross3(CVector{v1.x-v0.x,v1.y-v0.y,v1.z-v0.z},CVector{v2.x-v0.x,v2.y-v0.y,v2.z-v0.z});
        if(std::sqrt(cr.x*cr.x+cr.y*cr.y+cr.z*cr.z)>1.f||tries>40) break; }
    At<CVector>(poly,0)=v0; At<CVector>(poly,0xC)=v1; At<CVector>(poly,0x18)=v2;
    poly[0x24]=r.coin(7) ? uint8(0) : r.coin(9) ? uint8(0x7F) : uint8(1);
    // punto del triangulo y linea vertical que lo atraviesa (u origen/final al mismo lado, o fuera del triangulo)
    float u=r.uni(0.02f,0.98f), v=r.uni(0.02f,0.98f); if(u+v>0.98f){ u=0.98f-u*0.5f; v=0.98f-v*0.5f; }
    const float w=1.f-u-v; const CVector P{v0.x*w+v1.x*u+v2.x*v,v0.y*w+v1.y*u+v2.y*v,v0.z*w+v1.z*u+v2.z*v};
    const CVector jit=r.coin(3) ? RandVec(r,-1.5f,1.5f) : CVector{0.f,0.f,0.f};
    const float h1=r.uni(0.1f,3.f)*(r.coin(5) ? -1.f : 1.f), h2=r.uni(0.1f,3.f)*(r.coin(5) ? -1.f : 1.f);
    const CVector origin{P.x+jit.x,P.y+jit.y,P.z+h1}; const float lnMag=P.z-h2;
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CVector&,float,CColPoint&,CStoredCollPoly*)>(a)(origin,lnMag,*reinterpret_cast<CColPoint*>(cp),reinterpret_cast<CStoredCollPoly*>(poly));
    out["ret"]=ret; out["cp"]=Hex(cp,CP_SIZE); out["poly"]=Hex(poly,0x2C);
}
// Secuencias de CalculateTrianglePlanes / RemoveTrianglePlanes sobre varios modelos y una cache falsa de 3 a 8 enlaces.
inline void RunCacheOps(Rng& r,uintptr_t a,Out& out,json& det,bool removeFn){
    const int K=2+int(r.next()%5); const size_t links=3+r.next()%6; InstallFakeCache(links);
    std::array<Model,6> ms{}; std::array<CCollisionData*,6> cds{};
    for(int k=0;k<K;++k){ ms[size_t(k)]=BuildModel(r,0,0,5,0,false,false,3.f); cds[size_t(k)]=ms[size_t(k)].cd; }
    uint64 chain=1469598103934665603ull; const int L=6+int(r.next()%11);
    for(int o=0;o<L;++o){
        const int k=int(r.next()%uint32(K)); const bool remove=removeFn ? r.next()%10<7 : r.next()%10<2;
        reinterpret_cast<void(__cdecl*)(CCollisionData*)>(remove ? (removeFn ? a : uintptr_t(0x416400)) : (removeFn ? uintptr_t(0x416330) : a))(cds[size_t(k)]);
        const uint64 sig=CacheSig(cds.data(),K); chain=Fnv(&sig,8,chain);
        for(int i=0;i<K;++i){ const uint64 ms2=ModelSig(ms[size_t(i)]); chain=Fnv(&ms2,8,chain); }
    }
    out["chain"]=chain; out["cache"]=CacheSig(cds.data(),K);
    json sigs=json::array(); for(int i=0;i<K;++i) sigs.push_back(ModelSig(ms[size_t(i)])); out["models"]=sigs;
    for(int k=0;k<K;++k) ReleasePlanes(ms[size_t(k)]);
}
inline void RunSphereSphere(Rng& r,uintptr_t a,Out& out,json& det){
    auto* sa=Alloc(0x14,4); auto* sb=Alloc(0x14,4); auto* cp=NewCp();
    const float ra=r.uni(0.2f,2.5f), rb=r.coin(5) ? r.pick({0.f,0.5f,1.f}) : r.uni(0.1f,2.5f);
    const CVector ca=RandVec(r,-3.f,3.f); CVector cb{};
    switch(r.next()%8){
        case 0: cb=ca; break;                                                                                                   // centros coincidentes
        case 1: { cb=CVector{ca.x+(ra+rb),ca.y,ca.z}; break; }                                                                  // roce exacto
        case 2: { const CVector o=RandUnit(r); const float d=(ra+rb)*r.uni(0.9f,1.1f); cb=CVector{ca.x+o.x*d,ca.y+o.y*d,ca.z+o.z*d}; break; } // al limite
        case 3: cb=RandVec(r,-8.f,8.f); break;
        default: { const CVector o=RandUnit(r); const float d=(ra+rb)*r.uni(0.1f,1.0f); cb=CVector{ca.x+o.x*d,ca.y+o.y*d,ca.z+o.z*d}; break; }
    }
    float maxT=r.pick({BIG,BIG,r.uni(0.f,6.f),0.f,0.5f,1.f});
    At<CVector>(sa,0)=ca; At<float>(sa,0xC)=ra; Surface4(r,sa+0x10,false); At<CVector>(sb,0)=cb; At<float>(sb,0xC)=rb; Surface4(r,sb+0x10,false);
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CColSphere&,const CColSphere&,CColPoint&,float&)>(a)(*reinterpret_cast<CColSphere*>(sa),*reinterpret_cast<CColSphere*>(sb),*reinterpret_cast<CColPoint*>(cp),maxT);
    out["ret"]=ret; out["max"]=F(maxT); out["cp"]=Hex(cp,CP_SIZE);
}
inline void RunSphereTriangle(Rng& r,uintptr_t a,Out& out,json& det){
    auto t=MakeTri(r,r.pick({2.f,3.f,5.f})); auto* sph=Alloc(0x14,4); auto* cp=NewCp();
    const float rad=r.uni(0.15f,2.f);
    CVector q{}; const uint32 mode=r.next()%10;
    if(mode<4) q=TriPoint(r,t);                                                                            // proyeccion dentro
    else if(mode<7){ // borde: punto de una arista desplazado hacia fuera en el plano
        const int ed=int(r.next()%3); const CVector& p0=ed==0 ? t.va : ed==1 ? t.vb : t.vc; const CVector& p1=ed==0 ? t.vb : ed==1 ? t.vc : t.va;
        const CVector m=Lerp3(p0,p1,r.uni(0.f,1.f)); const CVector cen{(t.va.x+t.vb.x+t.vc.x)/3.f,(t.va.y+t.vb.y+t.vc.y)/3.f,(t.va.z+t.vb.z+t.vc.z)/3.f};
        CVector out2=Unit3(Cross3(CVector{p1.x-p0.x,p1.y-p0.y,p1.z-p0.z},t.n)); const CVector toM{m.x-cen.x,m.y-cen.y,m.z-cen.z};
        if(out2.x*toM.x+out2.y*toM.y+out2.z*toM.z<0.f) out2=CVector{-out2.x,-out2.y,-out2.z};
        const float d=r.uni(0.05f,1.2f); q=CVector{m.x+out2.x*d,m.y+out2.y*d,m.z+out2.z*d}; }
    else if(mode<9){ const CVector& v=r.coin(3) ? t.va : r.coin(2) ? t.vb : t.vc; const CVector o=RandUnit(r); const float d=r.uni(0.1f,1.2f); q=CVector{v.x+o.x*d,v.y+o.y*d,v.z+o.z*d}; }  // vertice
    else q=RandVec(r,-6.f,6.f);
    const float h=r.uni(-1.3f,1.3f)*rad; const CVector c{q.x+t.n.x*h,q.y+t.n.y*h,q.z+t.n.z*h};
    float maxT=r.pick({BIG,BIG,r.uni(0.f,4.f),r.uni(0.f,1.f),0.f});
    At<CVector>(sph,0)=c; At<float>(sph,0xC)=rad; Surface4(r,sph+0x10,false);
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CColSphere&,const CompressedVector*,const CColTriangle&,const CColTrianglePlane&,CColPoint&,float&)>(a)(
        *reinterpret_cast<CColSphere*>(sph),reinterpret_cast<const CompressedVector*>(t.verts),*reinterpret_cast<CColTriangle*>(t.tri),*reinterpret_cast<CColTrianglePlane*>(t.plane),*reinterpret_cast<CColPoint*>(cp),maxT);
    out["ret"]=ret; out["max"]=F(maxT); out["cp"]=Hex(cp,CP_SIZE);
    det["plane"]=Hex(t.plane,10);
}
inline void RunVerticalLine(Rng& r,uintptr_t a,Out& out,json& det){
    InstallFakeCache(3+r.next()%6);
    auto m=BuildModel(r,3,3,6,0,false,true,2.5f);
    if(r.coin(20)) At<CCollisionData*>(m.cm,0x2C)=nullptr;                 // modelo sin datos
    auto* mat=Alloc(0x48,16); MakeMatrix(r,mat,3.f,r.coin(2)); auto* line=Alloc(0x20,4); std::memset(line,0xCD,0x20);
    auto* cp=NewCp(); auto* poly=Alloc(0x2C,4); std::memset(poly,0xCD,0x2C);
    const CVector pos=At<CVector>(mat,0x30); const float x=pos.x+r.uni(-2.5f,2.5f), y=pos.y+r.uni(-2.5f,2.5f);
    At<CVector>(line,0)=CVector{x,y,pos.z+r.uni(0.5f,4.f)}; At<CVector>(line,0x10)=CVector{x,y,pos.z-r.uni(0.5f,4.f)};
    float maxT=r.pick({1.f,0.8f,0.5f,0.95f,0.3f}); const bool see=r.coin(2), shoot=r.coin(2); CStoredCollPoly* op=r.coin(3) ? nullptr : reinterpret_cast<CStoredCollPoly*>(poly);
    Crossings()=7u;
    const bool ret=reinterpret_cast<bool(__cdecl*)(const CColLine&,const CMatrix&,CColModel&,CColPoint&,float&,bool,bool,CStoredCollPoly*)>(a)(
        *reinterpret_cast<CColLine*>(line),*reinterpret_cast<CMatrix*>(mat),*reinterpret_cast<CColModel*>(m.cm),*reinterpret_cast<CColPoint*>(cp),maxT,see,shoot,op);
    out["ret"]=ret; out["max"]=F(maxT); out["cp"]=Hex(cp,CP_SIZE); out["poly"]=op ? Hex(poly,0x2C) : std::string("null"); out["crossings"]=Crossings();
    out["model"]=ModelSig(m); det["flags"]=json::array({see,shoot});
    ReleasePlanes(m);
}
inline void RunColModels(Rng& r,uintptr_t a,Out& out,json& det){
    InstallFakeCache(3+r.next()%6);
    auto A=BuildModel(r,3,3,5,3,true,true,2.f); auto B=BuildModel(r,3,3,5,3,true,true,2.f);
    auto* ma=Alloc(0x48,16); auto* mb=Alloc(0x48,16); MakeMatrix(r,ma,1.2f,r.coin(2)); MakeMatrix(r,mb,1.2f,r.coin(2));
    if(r.coin(30)){ At<CVector>(mb,0x30)=CVector{0.f,0.f,0.f}; }
    const uint32 nullMode=r.next()%40; if(nullMode==0) At<CCollisionData*>(A.cm,0x2C)=nullptr; if(nullMode==1) At<CCollisionData*>(B.cm,0x2C)=nullptr;
    auto* cps=Alloc(32*CP_SIZE,4); std::memset(cps,0xCD,32*CP_SIZE);
    auto* lcps=Alloc(16*CP_SIZE,4); std::memset(lcps,0xCD,16*CP_SIZE);
    auto* mt=reinterpret_cast<float*>(Alloc(16*4,4));
    for(int i=0;i<16;++i) mt[i]=A.disks ? r.uni(-2.f,1.f) : r.pick({1.f,1.f,0.9f,r.uni(0.4f,1.f)});
    if(A.disks&&r.coin(3)) for(int i=0;i<16;++i) mt[i]=r.uni(-3.f,3.f);
    const bool retAll=r.coin(2);
    Crossings()=7u;
    const int32 n=reinterpret_cast<int32(__cdecl*)(const CMatrix&,CColModel&,const CMatrix&,CColModel&,std::array<CColPoint,32>&,CColPoint*,float*,bool)>(a)(
        *reinterpret_cast<CMatrix*>(ma),*reinterpret_cast<CColModel*>(A.cm),*reinterpret_cast<CMatrix*>(mb),*reinterpret_cast<CColModel*>(B.cm),
        *reinterpret_cast<std::array<CColPoint,32>*>(cps),reinterpret_cast<CColPoint*>(lcps),mt,retAll);
    out["ret"]=n; out["cps"]=Fnv(cps,32*CP_SIZE); out["lines"]=Fnv(lcps,16*CP_SIZE);
    uint64 mh=1469598103934665603ull; for(int i=0;i<16;++i){ const uint32 b=F(mt[i]); mh=Fnv(&b,4,mh); } out["touch"]=mh;
    out["a"]=A.cd ? ModelSig(A) : 0; out["b"]=B.cd ? ModelSig(B) : 0; out["crossings"]=Crossings();
    json cl=json::array(); for(int i=0;i<std::min(std::max(n,0)+1,32);++i) cl.push_back(Hex(cps+size_t(i)*CP_SIZE,CP_SIZE)); det["sphere_cps"]=cl;
    json ln=json::array(); for(int i=0;i<A.nL;++i) ln.push_back(json::array({Hex(lcps+size_t(i)*CP_SIZE,CP_SIZE),F(mt[i])})); det["lines"]=ln;
    det["info"]=json{{"A",{A.nS,A.nB,A.nT,A.nL,A.disks,A.fg}},{"B",{B.nS,B.nB,B.nT,B.nL,B.disks,B.fg}},{"return_all",retAll}};
    ReleasePlanes(A); ReleasePlanes(B);
}

inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"colproc59b-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"colproc59b-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("colproc59b");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true;
    std::string only; { std::ifstream f(dir/"colproc59b-only.txt"); if(f)std::getline(f,only); }
    if(reinterpret_cast<uintptr_t>(&CCollision::ms_colModelCache)!=CACHE||reinterpret_cast<uintptr_t>(&CCollision::ms_iProcessLineNumCrossings)!=CROSSINGS||sizeof(CLinkList<CCollisionData*>)!=0x34){
        Write({{"event","error"},{"reason","layout"}}); return; }
    std::array<ReversibleHooks::ReversibleHook::Base*,13> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}
    std::array<bool,13> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // Estado real guardado: la lista de la cache de planos (0x34 bytes) y los contadores
    std::array<uint8,0x34> savedCache{}; std::memcpy(savedCache.data(),&Cache(),0x34);
    const uint32 savedCrossings=Crossings(), savedInMemory=At<uint32>(0x9655D4);
    Write({{"event","player_ready"},{"fp_control",_controlfp(0,0)}});
    bool passed=true; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            auto run=[&](bool candidate)->std::pair<json,json> {
                Rng rng; rng.s=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x59B59Bu; if(!rng.s)rng.s=1; arenaTop=0;
                for(auto* h:hooks)h->State(candidate);
                const bool switches=ReversingProbe::JumpsToDll(fn.address)==candidate; const uintptr_t a=fn.address;
                json out={{"switches",switches}}; json det=json::object();
                if(nm=="ProcessSphereBox")                    RunSphereBox(rng,a,out,det);
                else if(nm=="ProcessLineSphere")              RunLineSphere(rng,a,out,det);
                else if(nm=="ProcessLineBox")                 RunLineBox(rng,a,out,det);
                else if(nm=="ProcessDiscCollision")           RunDisc(rng,a,out,det);
                else if(nm=="ProcessLineTriangle")            RunLineTriangle(rng,a,out,det,false);
                else if(nm=="ProcessVerticalLineTriangle")    RunLineTriangle(rng,a,out,det,true);
                else if(nm=="IsStoredPolyStillValidVerticalLine") RunIsStoredPoly(rng,a,out,det);
                else if(nm=="CalculateTrianglePlanes-colData") RunCacheOps(rng,a,out,det,false);
                else if(nm=="RemoveTrianglePlanes-colData")   RunCacheOps(rng,a,out,det,true);
                else if(nm=="ProcessSphereSphere")            RunSphereSphere(rng,a,out,det);
                else if(nm=="ProcessSphereTriangle")          RunSphereTriangle(rng,a,out,det);
                else if(nm=="ProcessVerticalLine")            RunVerticalLine(rng,a,out,det);
                else if(nm=="ProcessColModels")               RunColModels(rng,a,out,det);
                return {out,det};
            };
            const auto [original,odet]=run(false); const auto [candidate,cdet]=run(true);
            const bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",original},{"candidate",candidate}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(!candidate.contains(k)||candidate[k]!=v)diff.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)},
                       {"detail_original",failures<=2 ? odet:json(nullptr)},{"detail_candidate",failures<=2 ? cdet:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    std::memcpy(&Cache(),savedCache.data(),0x34); Crossings()=savedCrossings; At<uint32>(0x9655D4)=savedInMemory;
    const bool restored=std::memcmp(&Cache(),savedCache.data(),0x34)==0&&Crossings()==savedCrossings;
    Write({{"event",passed&&restored ? "complete":"error"},{"pairs",pair},{"state_restored",restored}});
}
}
