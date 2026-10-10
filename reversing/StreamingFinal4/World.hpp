#pragma once
// Mundo falso del banco StreamingFinal4: model infos, pools de entidades, listas de sectores con nodos de los pools de nodos, pool de TXD,
// informacion de streaming (listas de cargados/pedidos, canales, directorio), poblacion, zona, camara, jugador y ajustes globales.
// Todo vive en memoria propia con direcciones estables o en memoria global del EXE que se guarda y se restaura en cada ejecucion.
#include "Support.hpp"
#include <algorithm>

namespace StreamingFinal4Live {
constexpr int NTOTAL=26316;
constexpr uintptr_t INFO=0x8E4CC0, MIPTRS=0xA9B0C8, POOLS=0xB74484, SECTORS=0xB7D0B8, REPEAT=0xB992B8, LODL=0xB99EB8, PLAYERS=0xB7CD98, CAMERA=0xB6F028,
    SCANADDR=0xB7CD78, TXDPOOL=0xC8800C, ANIMBLK=0xB5D4B0, CURRAREA=0xB72914, CURRLEVEL=0xBA6718;
constexpr size_t SECTORS_SIZE=120*120*8, REPEAT_SIZE=16*16*12, LOD_SIZE=30*30*4;
constexpr int LS=26312, LE=26313, RS=26314, RE=26315;  // centinelas de las listas de cargados / pedidos

struct Rng {
    uint32 st{1};
    uint32 next(){ st^=st<<13; st^=st>>17; st^=st<<5; return st; }
    uint32 below(uint32 n){ return n ? next()%n : 0; }
    bool chance(uint32 n){ return n && next()%n==0; }           // 1 de cada n
    bool pct(uint32 p){ return next()%100<p; }                  // p por ciento
    float uni(float lo,float hi){ return lo+(hi-lo)*float(next()&0xFFFFFF)/float(0xFFFFFF); }
};

// ---------------------------------------------------------------------------------------------------------------------
// Memoria falsa (direcciones estables)
// ---------------------------------------------------------------------------------------------------------------------
constexpr int NMI=1024, NBLD=40, NDUM=24, NOBJ=24, NPED=6, NSGL=4096, NDBL=2048, NTXD=40, NANIM=8, NCOL=16, NRW=512;
constexpr size_t MI_SIZE=0x70, BS=0x38, OS=0x19C, PS=0x7C4;
alignas(16) inline std::array<std::array<uint8,MI_SIZE>,NMI> miMem{};
alignas(16) inline std::array<std::array<uint8,0x10>,NMI> atomMem{};
alignas(16) inline std::array<std::array<uint8,0x04>,NMI> timeMem{};
alignas(16) inline std::array<std::array<uint8,0x30>,NCOL> colMem{};
alignas(16) inline std::array<std::array<uint8,8>,NRW> rwObjs{};         // objetos RW falsos (solo importa su direccion)
inline std::array<void*,64> miVt{}, entVt{};                              // vtables falsas (se rellenan en Stubs.hpp)

alignas(16) inline std::array<uint8,BS*NBLD> bldMem{}; inline std::array<uint8,NBLD> bldSt{};
alignas(16) inline std::array<uint8,BS*NDUM> dumMem{}; inline std::array<uint8,NDUM> dumSt{};
alignas(16) inline std::array<uint8,OS*NOBJ> objMem{}; inline std::array<uint8,NOBJ> objSt{};
alignas(16) inline std::array<uint8,PS*NPED> pedMem{}; inline std::array<uint8,NPED> pedSt{};
alignas(16) inline std::array<std::array<uint8,0x50>,NBLD+NDUM+NOBJ+NPED> entMat{};  // matrices falsas de las entidades
alignas(16) inline std::array<uint8,8*NSGL> sglStore{}; inline std::array<uint8,NSGL> sglSt{};
alignas(16) inline std::array<uint8,12*NDBL> dblStore{}; inline std::array<uint8,NDBL> dblSt{};
alignas(16) inline std::array<uint8,0x20> sglHdr{}, dblHdr{}, sglImg{}, dblImg{};
alignas(16) inline std::array<uint8,0x14> bldHdr{}, dumHdr{}, objHdr{}, pedHdr{}, txdHdr{};
constexpr int NTXDPOOL=5000;   // tamano real del pool de TXD (Init2 recorre TOTAL_TXD_MODEL_IDS=5000 con GetAt); solo las NTXD primeras ranuras estan vivas
alignas(16) inline std::array<uint8,12*NTXDPOOL> txdMem{}; inline std::array<uint8,NTXDPOOL> txdSt{};
alignas(16) inline std::array<uint8,0x600> vehMem{};          // vehiculo falso (jugador / remoto)
alignas(16) inline std::array<uint8,0x80> wantedMem{};
alignas(16) inline std::array<uint8,0x40> zoneInfoMem{};
alignas(16) inline std::array<uint8,0x100> rwCamMem{};
alignas(16) inline std::array<uint8,0x40> dirMem{};            // CDirectory falso
alignas(16) inline std::array<std::array<uint8,0x48>,4> miscMat{};
alignas(2048) inline std::array<uint8,0x40000> streamBuf0{}, streamBuf1{};
inline int nSgl{}, nDbl{};
inline int nBld{}, nDum{}, nObj{}, nPed{};

// ---------------------------------------------------------------------------------------------------------------------
// Acceso a CStreamingInfo (20 bytes) por desplazamientos del EXE
// ---------------------------------------------------------------------------------------------------------------------
inline uint8* Inf(int32 i){ return reinterpret_cast<uint8*>(INFO)+20*size_t(i); }
inline int16& iNext(int32 i){ return At<int16>(Inf(i),0); }
inline int16& iPrev(int32 i){ return At<int16>(Inf(i),2); }
inline int16& iCd(int32 i){ return At<int16>(Inf(i),4); }
inline uint8& iFlags(int32 i){ return Inf(i)[6]; }
inline uint8& iImg(int32 i){ return Inf(i)[7]; }
inline uint32& iOff(int32 i){ return At<uint32>(Inf(i),8); }
inline uint32& iSize(int32 i){ return At<uint32>(Inf(i),0xC); }
inline uint8& iState(int32 i){ return Inf(i)[0x10]; }
inline void LAdd(int32 idx,int32 after){ iNext(idx)=iNext(after); iPrev(iNext(idx))=int16(idx); iPrev(idx)=int16(after); iNext(after)=int16(idx); }

inline std::vector<int32> wset;

// ---------------------------------------------------------------------------------------------------------------------
// Identificadores de modelos
// ---------------------------------------------------------------------------------------------------------------------
inline int32 PedId(Rng& r){ return int32(r.below(300)); }
inline int32 VehId(Rng& r){ return 400+int32(r.below(212)); }
inline int32 ObjId(Rng& r){ return 612+int32(r.below(388)); }
inline int32 TxdId(Rng& r){ return 20000+int32(r.below(NTXD)); }
inline int32 DffId(Rng& r){ const uint32 k=r.below(10); return k<3 ? PedId(r) : k<6 ? VehId(r) : ObjId(r); }
inline int32 AnyId(Rng& r){
    const uint32 k=r.below(20);
    if(k<10)return DffId(r);
    if(k<14)return TxdId(r);
    if(k<15)return RESOURCE_ID_COL+int32(r.below(8));
    if(k<16)return RESOURCE_ID_IPL+int32(r.below(8));
    if(k<17)return RESOURCE_ID_DAT+int32(r.below(8));
    if(k<18)return RESOURCE_ID_IFP+int32(r.below(NANIM));
    if(k<19)return RESOURCE_ID_RRR+int32(r.below(6));
    return RESOURCE_ID_SCM+int32(r.below(12));
}
inline uint32 TypeOfSlot(int k){          // tipo de model info del hueco k (ids 0..1023)
    if(k<300)return 7;
    if(k>=400&&k<612)return 6;
    if(k<400)return k%2 ? 5u : 4u;
    const uint32 q=uint32(k)%6; return q<3 ? 1u : q<5 ? 3u : 8u;
}

// ---------------------------------------------------------------------------------------------------------------------
// Pools falsos
// ---------------------------------------------------------------------------------------------------------------------
inline void PoolHdr(std::array<uint8,0x14>& h,void* storage,void* states,int32 count){
    std::memset(h.data(),0,h.size()); At<void*>(h.data(),0)=storage; At<void*>(h.data(),4)=states; At<int32>(h.data(),8)=count; At<int32>(h.data(),0xC)=-1;
}
inline uint8* Bld(int k){ return bldMem.data()+BS*size_t(k); }
inline uint8* Dum(int k){ return dumMem.data()+BS*size_t(k); }
inline uint8* Obj(int k){ return objMem.data()+OS*size_t(k); }
inline uint8* Ped(int k){ return pedMem.data()+PS*size_t(k); }
inline uint8* NewSgl(void* item,void* next){
    const int k=nSgl++; if(k>=NSGL)std::abort();
    uint8* n=sglStore.data()+8*size_t(k); sglSt[size_t(k)]=0; At<void*>(n,0)=item; At<void*>(n,4)=next; return n;
}
inline uint8* NewDbl(void* item,void* next,void* prev){
    const int k=nDbl++; if(k>=NDBL)std::abort();
    uint8* n=dblStore.data()+12*size_t(k); dblSt[size_t(k)]=0; At<void*>(n,0)=item; At<void*>(n,4)=next; At<void*>(n,8)=prev; return n;
}
inline void SglPush(uint32* head,void* item){ *head=uint32(uintptr_t(NewSgl(item,reinterpret_cast<void*>(uintptr_t(*head))))); }
inline void DblPush(uint32* head,void* item){
    uint8* n=NewDbl(item,reinterpret_cast<void*>(uintptr_t(*head)),nullptr);
    if(*head)At<void*>(reinterpret_cast<void*>(uintptr_t(*head)),8)=n;
    *head=uint32(uintptr_t(n));
}

// ---------------------------------------------------------------------------------------------------------------------
// Entidades falsas: CBuilding/CDummy (0x38), CObject (0x19C), CPed (0x7C4). Campos del EXE: +0 vptr, +4 posicion, +0x14 matriz, +0x18 objeto RW,
// +0x1C banderas, +0x22 modelo, +0x28 enlace de streaming, +0x2C codigo de escaneo, +0x2F area, +0x30 LOD, +0x36 tipo y estado.
// ---------------------------------------------------------------------------------------------------------------------
inline void MakeEntity(uint8* e,size_t size,int matIdx,uint32 type,Rng& r,float cx,float cy,int32 model){
    for(size_t k=0;k<size;k+=4)At<uint32>(e,k)=r.next();
    At<void*>(e,0)=entVt.data();
    At<float>(e,4)=cx+r.uni(-300.f,300.f); At<float>(e,8)=cy+r.uni(-300.f,300.f); At<float>(e,0xC)=r.uni(-20.f,150.f); At<float>(e,0x10)=r.uni(-3.2f,3.2f);
    At<void*>(e,0x14)=nullptr;
    if(r.pct(25)){ auto& m=entMat[size_t(matIdx)]; std::memset(m.data(),0,m.size()); At<float>(m.data(),0x30)=At<float>(e,4)+r.uni(-5.f,5.f); At<float>(m.data(),0x34)=At<float>(e,8)+r.uni(-5.f,5.f);
                          At<float>(m.data(),0x38)=At<float>(e,0xC); At<void*>(e,0x14)=m.data(); }
    At<void*>(e,0x18)= r.pct(50) ? static_cast<void*>(rwObjs[r.below(NRW)].data()) : nullptr;
    uint32 fl=r.next()&~(0x80u|0x100u|0x400u|0x2000u|0x20000u|0x80000u);
    if(r.pct(80))fl|=0x80u; if(r.pct(12))fl|=0x100u; if(r.pct(10))fl|=0x400u; if(r.pct(10))fl|=0x2000u; if(r.pct(25))fl|=0x20000u; if(r.pct(8))fl|=0x80000u;
    At<uint32>(e,0x1C)=fl;
    At<uint16>(e,0x22)=uint16(model);
    At<void*>(e,0x24)=nullptr; At<void*>(e,0x28)=nullptr;
    At<uint16>(e,0x2C)= r.pct(15) ? AtA<uint16>(SCANADDR) : uint16(r.next());
    const uint32 ak=r.below(10);
    e[0x2F]= ak<6 ? uint8(0) : ak<7 ? uint8(AtA<uint32>(CURRAREA)) : ak<8 ? uint8(13) : uint8(r.below(19));
    At<void*>(e,0x30)=nullptr;
    e[0x36]=uint8(type|(r.below(32)<<3));
    if(size==OS)e[0x13C]= r.pct(55) ? uint8(1) : uint8(r.below(6));
}

// ---------------------------------------------------------------------------------------------------------------------
// Model infos falsos (0x70 bytes). Campos del EXE: +4 clave, +8 referencias (u16), +0xA TXD (i16), +0x14 volumen de colision, +0x18 distancia de dibujo,
// +0x1C objeto RW, +0x4E banderas de vehiculo, +0x50 usos. Campos propios para la vtable falsa: +0x58 tipo, +0x5C atomic, +0x60 tipo RW, +0x64 anim, +0x68 horario.
// ---------------------------------------------------------------------------------------------------------------------
inline void BuildModelInfos(Rng& r){
    // el primer byte de un RwObject es su tipo: indices impares = atomic (rpATOMIC=1), pares = clump (rpCLUMP=2); la DLL lo comprueba con assert en GetRwObject
    for(int k=0;k<NRW;++k){ std::memset(rwObjs[size_t(k)].data(),0,8); rwObjs[size_t(k)][0]= (k&1) ? uint8(1) : uint8(2); }
    for(int k=0;k<NMI;++k){
        auto* m=miMem[size_t(k)].data(); for(size_t o=0;o<MI_SIZE;o+=4)At<uint32>(m,o)=r.next();
        At<void*>(m,0)=miVt.data();
        At<uint16>(m,8)= r.pct(65) ? uint16(0) : uint16(1+r.below(4));
        At<int16>(m,0xA)=int16(r.below(NTXD));
        At<void*>(m,0x14)=colMem[size_t(k)%NCOL].data();
        At<float>(m,0x18)=r.uni(20.f,300.f);
        const uint32 ty=TypeOfSlot(k);
        { const bool atomic=(ty==1||ty==3||ty==8); const uint32 idx=2u*r.below(NRW/2)+(atomic ? 1u : 0u);
          At<void*>(m,0x1C)= r.pct(50) ? static_cast<void*>(rwObjs[idx].data()) : nullptr; }
        m[0x4E]=uint8(r.next()); m[0x50]=uint8(r.below(8));
        At<uint32>(m,0x58)=ty;
        auto* at=atomMem[size_t(k)].data(); for(size_t o=0;o<0x10;o+=4)At<uint32>(at,o)=r.next();
        At<void*>(m,0x5C)= (ty==1||ty==3||ty==8||r.chance(30)) ? static_cast<void*>(at) : nullptr;
        At<uint32>(m,0x60)= (ty==1||ty==3||ty==8) ? 1u : 2u;
        At<int32>(m,0x64)= r.pct(25) ? int32(r.below(NANIM)) : -1;
        auto* ti=timeMem[size_t(k)].data(); ti[0]=uint8(r.below(24)); ti[1]=uint8(r.below(24)); ti[2]=0; ti[3]=0;
        At<void*>(m,0x68)= ty==3 ? static_cast<void*>(ti) : nullptr;
    }
    for(int k=0;k<NCOL;++k){ auto* c=colMem[size_t(k)].data(); for(size_t o=0;o<0x30;o+=4)At<uint32>(c,o)=r.next(); At<float>(c,0x24)=r.uni(0.5f,80.f); }
    auto* tbl=reinterpret_cast<uint32*>(MIPTRS);
    for(int i=0;i<20000;++i)tbl[i]=uint32(uintptr_t(miMem[size_t(i)%NMI].data()));
}

// ---------------------------------------------------------------------------------------------------------------------
// TXD falsos: pool de 40 ranuras vivas (+0 diccionario, +4 referencias, +6 padre, +8 hash)
// ---------------------------------------------------------------------------------------------------------------------
inline void BuildTxd(Rng& r){
    txdSt.fill(0x80); std::memset(txdMem.data()+12*NTXD,0,txdMem.size()-12*size_t(NTXD));
    for(int k=0;k<NTXD;++k){ txdSt[size_t(k)]=0;
        auto* e=txdMem.data()+12*size_t(k);
        At<void*>(e,0)= r.pct(65) ? static_cast<void*>(rwObjs[r.below(NRW)].data()) : nullptr;
        At<uint16>(e,4)= r.pct(50) ? uint16(0) : uint16(1+r.below(3));
        At<int16>(e,6)= (k>0&&r.pct(40)) ? int16(r.below(unsigned(k))) : int16(-1);
        At<uint32>(e,8)=r.next();
    }
    PoolHdr(txdHdr,txdMem.data(),txdSt.data(),NTXDPOOL);
    AtA<uintptr_t>(TXDPOOL)=uintptr_t(txdHdr.data());
    for(int k=0;k<NANIM;++k){ auto* b=reinterpret_cast<uint8*>(ANIMBLK)+0x20*size_t(k); for(size_t o=0;o<0x20;o+=4)At<uint32>(b,o)=r.next(); b[0]=r.pct(55) ? 1 : 0; At<uint32>(b,4)=r.pct(60) ? 0u : r.below(3); }
}

// ---------------------------------------------------------------------------------------------------------------------
// Listas de sectores, pools de entidades y camara / jugador
// ---------------------------------------------------------------------------------------------------------------------
inline int SecX(float x){ return int(std::floor(x*0.02f+60.f)); }
inline void BuildEntities(Rng& r,uint32 flags){
    // pools de nodos locales
    std::memcpy(sglHdr.data(),sglImg.data(),sglImg.size()); std::memcpy(dblHdr.data(),dblImg.data(),dblImg.size());
    sglStore.fill(0xFD); sglSt.fill(0x80); dblStore.fill(0xFD); dblSt.fill(0x80); nSgl=0; nDbl=0;
    std::memset(reinterpret_cast<void*>(SECTORS),0,SECTORS_SIZE); std::memset(reinterpret_cast<void*>(REPEAT),0,REPEAT_SIZE); std::memset(reinterpret_cast<void*>(LODL),0,LOD_SIZE);

    // camara
    auto* cam=reinterpret_cast<uint8*>(CAMERA);
    const float cx=r.pct(70) ? r.uni(-900.f,900.f) : r.uni(-2900.f,2900.f), cy=r.pct(70) ? r.uni(-900.f,900.f) : r.uni(-2900.f,2900.f);   // sector 2..118: dominio real del mundo (ver bucles de DeleteRwObjectsBehindCamera)
    At<void*>(cam,0x14)=reinterpret_cast<void*>(uintptr_t(0xB6F99C));             // m_matrix = &m_mCameraMatrix
    auto* cm=reinterpret_cast<uint8*>(0xB6F99C);
    At<float>(cm,0x30)=cx; At<float>(cm,0x34)=cy; At<float>(cm,0x38)=r.uni(-5.f,120.f);
    float fx=0.f,fy=1.f;
    const uint32 dk=r.below(8);
    if(dk==0){fx=1.f;fy=0.f;} else if(dk==1){fx=-1.f;fy=0.f;} else if(dk==2){fx=0.f;fy=-1.f;} else if(dk==3){fx=0.f;fy=1.f;}
    else if(dk==4){fx=r.uni(-1.f,1.f); fy=fx;} else { fx=r.uni(-1.f,1.f); fy=r.uni(-1.f,1.f); }
    At<float>(cm,0x10)=fx; At<float>(cm,0x14)=fy; At<float>(cm,0x18)=r.uni(-1.f,1.f);
    At<float>(cam,0xF0)=r.uni(0.5f,3.f);                                            // m_fLODDistMultiplier (0xB6F118)
    std::memset(rwCamMem.data(),0,rwCamMem.size()); At<float>(rwCamMem.data(),0x84)=r.uni(100.f,700.f);
    At<void*>(cam,0x954)=rwCamMem.data();                                           // m_pRwCamera (0xB6F97C)

    // entidades
    nBld=8+int(r.below(unsigned(NBLD-8+1))); nDum=4+int(r.below(unsigned(NDUM-4+1))); nObj=4+int(r.below(unsigned(NOBJ-4+1))); nPed=2+int(r.below(unsigned(NPED-2+1)));
    bldSt.fill(0x80); dumSt.fill(0x80); objSt.fill(0x80); pedSt.fill(0x80);
    auto model=[&](bool lowIds)->int32{ if(!wset.empty()&&r.pct(60)){ for(int t=0;t<8;++t){ const int32 m=wset[r.below(unsigned(wset.size()))]; if(m<20000)return m; } } return lowIds ? PedId(r) : ObjId(r); };
    for(int k=0;k<nBld;++k){ bldSt[size_t(k)]=0; MakeEntity(Bld(k),BS,k,1,r,cx,cy,model(false)); }
    for(int k=0;k<nDum;++k){ dumSt[size_t(k)]=0; MakeEntity(Dum(k),BS,NBLD+k,5,r,cx,cy,model(false)); }
    for(int k=0;k<nObj;++k){ objSt[size_t(k)]=0; MakeEntity(Obj(k),OS,NBLD+NDUM+k,4,r,cx,cy,model(false)); }
    for(int k=0;k<nPed;++k){ pedSt[size_t(k)]=0; MakeEntity(Ped(k),PS,NBLD+NDUM+NOBJ+k,3,r,cx,cy,PedId(r)); At<uint32>(Ped(k),0x46C)=r.next(); At<void*>(Ped(k),0x584)= (r.pct(30)&&nBld>0) ? static_cast<void*>(Bld(int(r.below(unsigned(nBld))))) : nullptr; }
    // LOD: algunas entidades apuntan a otra del mismo pool
    // cadenas de LOD sin ciclos (FindLastLOD del EXE y de la DLL seguirian +0x30 sin fin): solo hacia indices mayores
    for(int k=0;k+1<nBld;++k)if(r.pct(15))At<void*>(Bld(k),0x30)=Bld(k+1+int(r.below(unsigned(nBld-k-1))));
    for(int k=0;k+1<nDum;++k)if(r.pct(15))At<void*>(Dum(k),0x30)=Dum(k+1+int(r.below(unsigned(nDum-k-1))));
    for(int k=0;k+1<nObj;++k)if(r.pct(10))At<void*>(Obj(k),0x30)=Obj(k+1+int(r.below(unsigned(nObj-k-1))));
    PoolHdr(bldHdr,bldMem.data(),bldSt.data(),NBLD); PoolHdr(dumHdr,dumMem.data(),dumSt.data(),NDUM); PoolHdr(objHdr,objMem.data(),objSt.data(),NOBJ); PoolHdr(pedHdr,pedMem.data(),pedSt.data(),NPED);
    auto* pools=reinterpret_cast<uint32*>(POOLS);
    pools[0]=uint32(uintptr_t(sglHdr.data())); pools[1]=uint32(uintptr_t(dblHdr.data())); pools[3]=uint32(uintptr_t(pedHdr.data()));
    pools[5]=uint32(uintptr_t(bldHdr.data())); pools[6]=uint32(uintptr_t(objHdr.data())); pools[7]=uint32(uintptr_t(dumHdr.data()));
    if(flags&1u){
        // listas de sectores: celdas cercanas a la camara (y alguna lejana), con entidades repartidas
        const int sx0=SecX(cx), sy0=SecX(cy);
        auto cell=[&](int& sx,int& sy){ const uint32 k=r.below(10); const int span= k<8 ? 14 : 60; sx=sx0+int(r.below(unsigned(2*span+1)))-span; sy=sy0+int(r.below(unsigned(2*span+1)))-span; sx=std::clamp(sx,0,119); sy=std::clamp(sy,0,119); };
        for(int k=0;k<nBld;++k){ int sx,sy; cell(sx,sy); SglPush(&AtA<uint32>(SECTORS+8u*unsigned(sy*120+sx)),Bld(k)); }
        for(int k=0;k<nDum;++k){ int sx,sy; cell(sx,sy); DblPush(&AtA<uint32>(SECTORS+8u*unsigned(sy*120+sx)+4u),Dum(k)); }
        for(int k=0;k<nObj;++k){ int sx,sy; cell(sx,sy); DblPush(&AtA<uint32>(REPEAT+12u*unsigned((sy&15)*16+(sx&15))+8u),Obj(k)); }
        for(int k=0;k<nBld;++k)if(r.pct(40)){ const int lx=std::clamp(int(std::floor(At<float>(Bld(k),4)*0.005f+15.f)),0,29), ly=std::clamp(int(std::floor(At<float>(Bld(k),8)*0.005f+15.f)),0,29);
                                              SglPush(&AtA<uint32>(LODL+4u*unsigned(ly*30+lx)),Bld(k)); }
    }
    // jugador
    auto* pl=reinterpret_cast<uint8*>(PLAYERS);
    At<void*>(pl,0)= (r.pct(92)&&nPed>0) ? static_cast<void*>(Ped(0)) : nullptr;
    // vehiculo del jugador / remoto
    std::memset(vehMem.data(),0,vehMem.size());
    At<void*>(vehMem.data(),0)=entVt.data(); At<float>(vehMem.data(),4)=cx+r.uni(-10.f,10.f); At<float>(vehMem.data(),8)=cy+r.uni(-10.f,10.f); At<float>(vehMem.data(),0xC)=r.uni(0.f,80.f);
    At<void*>(vehMem.data(),0x14)=nullptr;
    At<float>(vehMem.data(),0x44)=r.pct(30) ? 0.f : r.uni(-1.f,1.f); At<float>(vehMem.data(),0x48)=r.pct(30) ? 0.f : r.uni(-1.f,1.f);
    At<uint32>(vehMem.data(),0x594)= r.pct(20) ? 4u : r.pct(25) ? 3u : r.below(12);
    At<void*>(pl,0xB0)= r.pct(25) ? static_cast<void*>(vehMem.data()) : nullptr;                   // m_pRemoteVehicle (0xB7CE48)
    // codigo de escaneo actual (lejos de 0xFFFF para no despertar CWorld::ClearScanCodes)
    AtA<uint16>(SCANADDR)=uint16(1+r.below(4000));
}

// ---------------------------------------------------------------------------------------------------------------------
// Informacion de streaming, canales, listas de cargados / pedidos y estadisticas
// ---------------------------------------------------------------------------------------------------------------------
inline void InitLists(){
    for(int32 i=0;i<NTOTAL;++i){ std::memset(Inf(i),0,20); iNext(i)=-1; iPrev(i)=-1; iCd(i)=-1; }
    iNext(LS)=LE; iPrev(LE)=LS; iNext(RS)=RE; iPrev(RE)=RS;
    AtA<uintptr_t>(0x9654B4)=INFO; AtA<uintptr_t>(0x8E4C60)=uintptr_t(Inf(LS)); AtA<uintptr_t>(0x8E4C5C)=uintptr_t(Inf(LE));
    AtA<uintptr_t>(0x8E4C58)=uintptr_t(Inf(RS)); AtA<uintptr_t>(0x8E4C54)=uintptr_t(Inf(RE));
}
inline void BuildInfo(Rng& r){
    InitLists(); wset.clear();
    auto add=[&](int32 id){ if(id>=0&&id<NTOTAL&&std::find(wset.begin(),wset.end(),id)==wset.end())wset.push_back(id); };
    for(int32 id:{7,87,244,246,256,257,285,286,287,334,345,346,420,427,432,433,438,488,490,497})add(id);
    for(int32 id=374;id<=381;++id)add(id);
    for(int32 id=290;id<=299;++id)add(id);
    for(int32 id=300;id<=319;++id)add(id);
    for(int32 id=384;id<=393;++id)add(id);
    // todos los TXD vivos del pool falso y todos los bloques de animacion estan en el directorio con tamano: GetNextFileOnCd / RequestModelStream piden el TXD o IFP de
    // cualquier DFF (RequestModel en 0x409xxx) y un modelo de tamano 0 dejaria `pos` sin inicializar (0x40CC41 cmp ecx,ebp; je 0x40CC68)
    for(int32 id=RESOURCE_ID_TXD;id<RESOURCE_ID_TXD+NTXD;++id)add(id);
    for(int32 id=RESOURCE_ID_IFP;id<RESOURCE_ID_IFP+NANIM;++id)add(id);
    for(int k=0;k<10;++k)add(PedId(r)); for(int k=0;k<12;++k)add(VehId(r)); for(int k=0;k<16;++k)add(ObjId(r)); for(int k=0;k<8;++k)add(TxdId(r)); for(int k=0;k<14;++k)add(AnyId(r));
    // tamanos, imagen y cadena del directorio: dos imagenes, ids mezclados, desplazamientos crecientes
    std::vector<int32> byImg[2];
    // tamano nunca 0: los modelos de la cadena del directorio tienen tamano; con 0 GetCdPosnAndSize no escribe `pos` y RequestModelStream (0x40CC68 / 0x40CC70) usa una pila sin inicializar distinta en el EXE y en la DLL
    for(int32 id:wset){ iSize(id)= r.pct(80) ? 1u+r.below(14) : 1u+r.below(60); iImg(id)=uint8(r.below(2)); byImg[iImg(id)].push_back(id); }
    for(auto& v:byImg){
        for(size_t k=v.size();k>1;--k)std::swap(v[k-1],v[r.below(unsigned(k))]);
        uint32 off=r.below(100);
        for(size_t k=0;k<v.size();++k){ iOff(v[k])=off; off+=iSize(v[k])+r.below(4); iCd(v[k])= (k+1<v.size()&&r.pct(92)) ? int16(v[k+1]) : int16(-1); }
    }
    // estados
    int req=0,pri=0; uint64 used=0;
    std::vector<int32> reading;
    for(int32 id:wset){
        const uint32 roll=r.below(100);
        if(roll<38){ iState(id)=0; if(r.pct(20))iFlags(id)=uint8(r.below(64)&0x3F); }
        else if(roll<68){ // cargado
            iState(id)=1; uint8 fl=0; if(r.pct(20))fl|=2; if(r.pct(15))fl|=4; if(r.pct(25))fl|=8; if(r.pct(10))fl|=1; if(r.pct(6))fl|=0x20; iFlags(id)=fl;
            used+=uint64(iSize(id))*2048u;
            if(!(fl&6)&&r.pct(92))LAdd(id,LS);
        } else if(roll<88){ // pedido
            iState(id)=2; uint8 fl=0; if(r.pct(25))fl|=0x10; if(r.pct(20))fl|=8; if(r.pct(10))fl|=2; if(r.pct(8))fl|=4; if(r.pct(5))fl|=0x20; iFlags(id)=fl;
            LAdd(id,RS); ++req; if(fl&0x10)++pri;
        } else if(roll<96){ iState(id)=3; iFlags(id)=uint8(r.pct(40) ? 8 : 0); reading.push_back(id); }
        else { iState(id)=0; }
    }
    // todo modelo que no esta libre tiene tamano en el directorio (si no, el EXE y la DLL leerian posiciones sin inicializar)
    for(int32 id:wset)if(iState(id)!=0&&iSize(id)==0)iSize(id)=1+r.below(9);
    // canales
    auto* ch=reinterpret_cast<uint8*>(0x8E4A60);
    std::memset(ch,0,0x130);
    for(int c=0;c<2;++c){ auto* e=ch+0x98*size_t(c); for(int k=0;k<16;++k){ At<int32>(e,4*k)=-1; At<int32>(e,0x40+4*k)=0; } }
    int slot[2]{0,0}; uint32 sect[2]{0,0};
    for(int32 id:reading){ const int c=int(r.below(2)); if(slot[c]>=16){ iState(id)=0; continue; }
        auto* e=ch+0x98*size_t(c); At<int32>(e,4*slot[c])=id; At<int32>(e,0x40+4*slot[c])=int32(sect[c]); sect[c]+=iSize(id); ++slot[c]; }
    for(int c=0;c<2;++c){ auto* e=ch+0x98*size_t(c);
        At<int32>(e,0x80)= slot[c] ? 1 : 0; At<int32>(e,0x84)=int32(r.below(120))-60; At<uint32>(e,0x88)=r.next()&0x1FFFFFFu; At<int32>(e,0x8C)=int32(sect[c]); At<int32>(e,0x90)=int32(r.below(4));
        At<int32>(e,0x94)=0; }
    CStreaming::ms_bLoadingBigModel=false;
    CStreaming::ms_channelError=-1;
    CStreaming::ms_numModelsRequested=req;
    CStreaming::ms_numPriorityRequests=uint32(pri+(r.pct(20) ? 1+int(r.below(2)) : 0));
    CStreaming::ms_memoryUsedBytes=uint32(std::min<uint64>(used+r.below(5000),0x7FFFFFFFu));
    CStreaming::ms_memoryAvailable= r.pct(60) ? size_t(0x3200000) : size_t(r.below(0x2000000)+0x100000);
    CStreaming::desiredNumVehiclesLoaded=r.below(32);
    CStreaming::ms_bLoadVehiclesInLoadScene=r.pct(50);
    CStreaming::ms_streamingBufferSize=4+r.below(40);
    CStreaming::ms_pStreamingBuffer[0]=streamBuf0.data(); CStreaming::ms_pStreamingBuffer[1]=streamBuf1.data();
    CStreaming::ms_bEnableRequestListPurge=r.pct(70);
    CStreaming::ms_disableStreaming=r.pct(15);
    CStreaming::m_bBoatsNeeded=r.pct(30);
    CStreaming::m_bCopBikeLoaded=r.pct(30);
    CStreaming::m_bDisableCopBikes=r.pct(20);
    CStreaming::ms_nTimePassedSinceLastCopBikeStreamedIn=r.pct(50) ? AtA<uint32>(0xB7CB84)+r.below(50000) : (AtA<uint32>(0xB7CB84)>r.below(60000) ? AtA<uint32>(0xB7CB84)-r.below(60000) : 0u);
    CStreaming::m_bStreamHarvesterModelsThisFrame=r.pct(20);
    CStreaming::m_bHarvesterModelsRequested=r.pct(30);
    CStreaming::ms_currentZoneType=r.pct(30) ? -1 : int32(r.below(20));
    CStreaming::ms_loadedGangs=uint16(r.pct(40) ? r.below(1024) : 0);
    CStreaming::ms_loadedGangCars=uint16(r.pct(40) ? r.below(1024) : 0);
    CStreaming::ms_lastCullZone=-1;
    CStreaming::ms_oldSectorX=0; CStreaming::ms_oldSectorY=0;
    CStreaming::ms_bLoadingScene=r.pct(10);
    CStreaming::m_bLoadingAllRequestedModels=false;
    CStreaming::m_bModelStreamNotLoaded=r.pct(50);
    CStreaming::ms_bIsInitialised=1;
    AtA<uint32>(0x8E4898)=r.next()&0x03FFFFFFu;                                    // gLastCdStreamPosn
    AtA<int32>(0x965534)=int32(r.below(2)); AtA<int32>(0x965524)=int32(r.below(6)); AtA<int32>(0x965528)=r.pct(50) ? 420 : (r.pct(50) ? 438 : 0);
    AtA<int32>(0x96552C)=int32(r.below(20)); AtA<int32>(0x965530)=int32(r.below(3))-1; AtA<int32>(0x9654CC)=int32(r.below(4))-1; AtA<int32>(0x9654D0)=int32(r.below(4))-1;
    AtA<int32>(0x9654D4)=int32(r.below(40));
    // archivos de imagen
    for(int k=0;k<8;++k){ auto* f=reinterpret_cast<uint8*>(0x8E48D8)+0x30*size_t(k); std::memset(f,0,0x30);
        if(k<4){ const char* nm= k==0 ? "MODELS\\GTA3.IMG" : k==1 ? "MODELS\\GTA_INT.IMG" : k==2 ? "MODELS\\PLAYER.IMG" : "MODELS\\EXTRA.IMG"; std::strcpy(reinterpret_cast<char*>(f),nm);
                  f[0x28]= k<2 ? 1 : uint8(r.pct(50)); At<uint32>(f,0x2C)=uint32(k)<<24; } }
}

// ---------------------------------------------------------------------------------------------------------------------
// Poblacion, zona, trucos, entorno
// ---------------------------------------------------------------------------------------------------------------------
inline void BuildPopulation(Rng& r){
    // grupos de coches (30 + 4 trucos): 23 entradas de 16 bits
    auto* cars=reinterpret_cast<uint16*>(0xC0ED38); auto* ncars=reinterpret_cast<uint16*>(0xC0EC78);
    for(int g=0;g<34;++g){ const uint32 n= (g>=18&&g<28) ? 1+r.below(23) : r.below(24); ncars[g]=uint16(n);
        for(int k=0;k<23;++k)cars[g*23+k]= (uint32(k)<n) ? uint16(VehId(r)) : uint16(2000); }
    // grupos de peatones: tres grupos por fila de traduccion con el mismo numero de elementos; las filas 18..32 (bandas, camellos, tiendas, oficinas,
    // maridos y esposas) usan 45 grupos distintos y las filas 0..17 reutilizan grupos que el codigo probado no lee
    auto* trans=reinterpret_cast<uint32*>(0x8D2540); auto* nped=reinterpret_cast<uint16*>(0xC0ECC0); auto* peds=reinterpret_cast<uint16*>(0xC0F358);
    const int totalRows=int(POPCYCLE_TOTAL_GROUPS);
    for(int row=0;row<totalRows;++row){ const uint32 n=1+r.below(20); for(int z=0;z<3;++z){
        const uint32 g= row>=18 ? uint32(3*(row-18)+z) : uint32(45+((3*row+z)%15)); trans[row*3+z]=g; nped[g]=uint16(n);
        for(int k=0;k<21;++k)peds[g*21+k]= (uint32(k)<n) ? uint16(PedId(r)) : uint16(2000); } }
    // grupos de coches cargados (aleatorios, sin repetir)
    auto fill=[&](CLoadedCarGroup& g,uint32 maxN){ g.Clear(); const uint32 n=r.below(maxN+1); for(uint32 k=0;k<n;++k)g.AddMember(static_cast<eModelID>(VehId(r))); };
    fill(CPopulation::m_LoadedBoats,5); fill(CPopulation::m_InAppropriateLoadedCars,8); fill(CPopulation::m_AppropriateLoadedCars,10);
    for(auto& g:CPopulation::m_LoadedGangCars)fill(g,4);
    fill(*reinterpret_cast<CLoadedCarGroup*>(0xC0EBF4),3);
    fill(CStreaming::ms_vehiclesLoaded,14);
    CPopulation::CurrentWorldZone=r.below(3);
    // peds cargados
    CStreaming::ms_numPedsLoaded=0;
    for(auto& p:CStreaming::ms_pedsLoaded)p=MODEL_INVALID;
    const uint32 np=r.below(9); for(uint32 k=0;k<np;++k){ CStreaming::ms_pedsLoaded[k]=static_cast<eModelID>(PedId(r)); ++CStreaming::ms_numPedsLoaded; }
    for(uint32 k=np;k<8;++k)if(r.pct(10))CStreaming::ms_pedsLoaded[k]=static_cast<eModelID>(PedId(r)); // ranuras ocupadas sin contar (dominio inconsistente leve)
    CStreaming::ms_numPedsLoaded=0; for(auto p:CStreaming::ms_pedsLoaded)if(p>=0)++CStreaming::ms_numPedsLoaded;
    for(auto& v:CStreaming::ms_NextPedToLoadFromGroup)v=int32(r.below(8));
    // modelos por defecto
    for(auto& v:CStreaming::ms_aDefaultCopCarModel)v=VehId(r); for(auto& v:CStreaming::ms_aDefaultCopModel)v=PedId(r);
    CStreaming::ms_DefaultCopBikeModel=VehId(r); CStreaming::ms_DefaultCopBikerModel=PedId(r);
    for(auto& v:CStreaming::ms_aDefaultAmbulanceModel)v=VehId(r); for(auto& v:CStreaming::ms_aDefaultMedicModel)v=PedId(r);
    for(auto& v:CStreaming::ms_aDefaultFireEngineModel)v=VehId(r); for(auto& v:CStreaming::ms_aDefaultFiremanModel)v=PedId(r);
    auto* cab=reinterpret_cast<int32*>(0x8A5AF4); { static const int32 cabReal[6]{262,261,220,234,182,206}; for(int k=0;k<6;++k)cab[k]=cabReal[k]; }   // tabla constante del EXE (0x8A5AF4); la DLL usa MODEL_BMOCD.. fijos, aleatorizarla es fuera de dominio
    // los modelos por defecto tambien estan en el conjunto de trabajo (con estados aleatorios)
    auto touch=[&](int32 id){ if(std::find(wset.begin(),wset.end(),id)==wset.end()){ wset.push_back(id); iSize(id)=1+r.below(10); iImg(id)=uint8(r.below(2)); iOff(id)=r.below(1000); const uint32 s=r.below(10); iState(id)= s<4 ? 0 : s<8 ? 1 : 2;
        if(iState(id)==2){ LAdd(id,RS); ++CStreaming::ms_numModelsRequested; } else if(iState(id)==1&&r.pct(80))LAdd(id,LS); } };
    for(auto v:CStreaming::ms_aDefaultCopCarModel)touch(v); for(auto v:CStreaming::ms_aDefaultCopModel)touch(v); touch(CStreaming::ms_DefaultCopBikeModel); touch(CStreaming::ms_DefaultCopBikerModel);
    for(auto v:CStreaming::ms_aDefaultAmbulanceModel)touch(v); for(auto v:CStreaming::ms_aDefaultMedicModel)touch(v);
    for(auto v:CStreaming::ms_aDefaultFireEngineModel)touch(v); for(auto v:CStreaming::ms_aDefaultFiremanModel)touch(v);
    for(int k=0;k<6;++k)touch(cab[k]);
    for(auto id:CStreaming::ms_pedsLoaded)if(id>=0)touch(id);
    // identificadores de los trozos de cosechadora
    auto* hv=reinterpret_cast<uint16*>(0x8CD6E4); for(int k=0;k<8;++k)hv[k]=uint16(ObjId(r)); for(int k=0;k<8;k+=2)touch(hv[k]);
    // mejoras enlazadas de vehiculos: 30 + 30 + contador
    auto* up=reinterpret_cast<int16*>(0xB4E6D8); const int n=int(r.below(12)); for(int k=0;k<30;++k){ up[k]= k<n ? int16(1000+int(r.below(40))) : int16(-1); up[30+k]= k<n ? int16(1000+int(r.below(40))) : int16(-1); }
    up[60]=int16(n);
}
inline void BuildEnv(Rng& r){
    // zona de poblacion
    std::memset(zoneInfoMem.data(),0,zoneInfoMem.size());
    for(int k=0;k<10;++k)zoneInfoMem[size_t(k)]= r.pct(40) ? uint8(1+r.below(200)) : uint8(0);
    zoneInfoMem[0xF]=uint8(r.below(20)|(r.below(8)<<5)); zoneInfoMem[0x10]=uint8(r.next());
    AtA<uint32>(0xC0BC68)= r.pct(88) ? uint32(uintptr_t(zoneInfoMem.data())) : 0u;                      // CPopCycle::m_pCurrZoneInfo
    AtA<int32>(0xC0BC70)=int32(r.below(2)); AtA<int32>(0xC0BC74)=int32(r.below(12));
    auto* perc=reinterpret_cast<uint8*>(0xC0BC78); for(int k=0;k<12*2*20*18;++k)perc[k]= r.pct(35) ? uint8(1+r.below(9)) : uint8(0);
    { const uint32 k=r.below(5); AtA<float>(0xC0E978)= k==0 ? 0.f : k==1 ? 0.03f : k==2 ? 0.02f : k==3 ? 0.5f : 1.f; }
    AtA<int32>(CURRLEVEL)=int32(r.below(4));
    // trucos
    std::memset(reinterpret_cast<void*>(0x969130),0,0x60);
    if(r.pct(20)){ for(int idx:{41,75,46,47,44,70})if(r.pct(25))AtA<uint8>(0x969130+unsigned(idx))=1; }
    if(r.pct(15))AtA<uint8>(0x969130+43u)=1;                                                              // GANGS_CONTROLS_THE_STREETS
    if(r.pct(10))AtA<uint8>(0x969130+unsigned(r.below(0x60)))=1;
    // entorno
    AtA<uint32>(CURRAREA)= r.pct(75) ? 0u : r.below(19);
    AtA<float>(0xB76848)=r.uni(100.f,800.f); AtA<uint8>(0xB76850)=r.pct(20) ? 1 : 0; AtA<uint8>(0xB745C1)=r.pct(75) ? 1 : 0; AtA<float>(0x8CD804)=r.uni(1.f,3.f);
    AtA<uint8>(0xB5F852)=r.pct(10) ? 1 : 0; AtA<uint32>(0xA43088)=r.pct(10) ? 1u : 0u;
    AtA<uint32>(0xB7CB84)=r.next()&0x0FFFFFFFu;                                                           // CTimer::m_snTimeInMilliseconds
    { const uint32 k=r.below(8); AtA<uint32>(0xB7CB4C)= k==0 ? 106u+128u*r.below(100) : k==1 ? 64u*r.below(100) : r.next()&0xFFFFFu; }     // numero de fotograma
    AtA<uint8>(0xB7CB48)=r.pct(8) ? 1 : 0; AtA<uint8>(0xB7CB49)=r.pct(8) ? 1 : 0; AtA<uint8>(0xB7CB8C)=r.pct(50) ? 1 : 0;
    // ordenes de busqueda
    std::memset(wantedMem.data(),0,wantedMem.size()); At<int32>(wantedMem.data(),0x2C)=int32(r.below(7));
    // scripts en streaming: usuarios por script (byte en +4 de cada ranura de 32)
    for(int k=0;k<82;++k){ auto* s=reinterpret_cast<uint8*>(0xA47B60)+0x20*size_t(k); s[4]= r.pct(65) ? uint8(0) : uint8(1+r.below(3)); }
    // estado de directorio extra y ajustes que escriben ReadIniFile / Init2
    AtA<uint32>(0x8E48D0)=uint32(uintptr_t(dirMem.data())); for(size_t k=0;k<dirMem.size();k+=4)At<uint32>(dirMem.data(),k)=r.next();
}
}
