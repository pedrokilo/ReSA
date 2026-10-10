#pragma once
// Familia C: jerarquia del vehiculo (PreprocessHierarchy), instancias y clump (CreateInstance, SetClump, DeleteRwObject), ruedas,
// pipelines, matricula, remaps y reduccion de materiales.
#include "CasesB.hpp"

namespace VehModelInfoFinal4Live {
// Pool falso de CVehicleStructure en B4E680 (formato crudo de CPool: objetos, estados, capacidad, ultimo libre, bandera de propiedad)
inline void SetupFakePool(Ctx& c,bool allowFull=false){
    std::memset(w.poolObj,0,sizeof(w.poolObj)); std::memset(w.poolFlags,0,sizeof(w.poolFlags));
    At<void*>(w.poolObj,0)=w.vsPool; At<void*>(w.poolObj,4)=w.poolFlags; At<int32>(w.poolObj,8)=NVS;
    At<int32>(w.poolObj,0xC)=int32(c.next()%(NVS+1))-1; w.poolObj[0x10]=1; w.poolObj[0x11]=0;
    for(int k=0;k<NVS;++k)w.poolFlags[k]= c.chance(2) ? uint8(0x80|(c.next()&0x7F)) : uint8(c.next()&0x7F);
    if(!allowFull){ w.poolFlags[c.next()%NVS]|=0x80; }
    *reinterpret_cast<void**>(0xB4E680)=w.poolObj;
}

inline bool RunC(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; uint8* const o=w.mi[0];
    auto& out=c.out; auto& ret=c.ret;

    if(nm=="GetWheelPosn"||nm=="GetOriginalCompPosition"){
        auto P=MakePatches(G_TREE|G_RWOBJ|G_CLUMP); c.BuildWorld();
        static const int types[]{0,1,2,3,4,4,4,5,6,9,10};
        c.FillMi(0,types[c.next()%std::size(types)]); At<void*>(o,0x1C)=w.clump[0];
        std::memset(w.outVec,0xCD,12);
        tb.frameForIdCount= c.chance(6) ? 0 : 1; tb.frameForId[0]=w.fr[c.next()%unsigned(c.treeN)]; tb.nextMatrix=0; c.fill(w.mx[0],0x50); c.fill(w.mx[1],0x50);
        for(int k=0;k<4;++k)At<float>(w.mx[k],0x30)=c.uni(-5.f,5.f);
        if(nm=="GetWheelPosn"){
            if(tb.frameForIdCount==0)tb.frameForIdCount=1;
            CallThis<void>(a,o,int32(c.next()%4),static_cast<void*>(w.outVec),c.chance(3)); }
        else ret=CallThis<bool>(a,o,static_cast<void*>(w.outVec),int32(c.next()%20));
        out["vec"]=c.Bytes(w.outVec,12); return true; }

    if(nm=="SetRenderPipelines"){
        auto P=MakePatches(G_CAR|G_RWOBJ); c.BuildWorld(); c.FillMi(0); At<void*>(o,0x1C)=w.clump[0];
        *reinterpret_cast<void**>(0xB4E6B8)= c.chance(2) ? nullptr : static_cast<void*>(w.nfr[7]);
        CallThis<void>(a,o); out["car"]=Id(*reinterpret_cast<void**>(0xB4E6B8)); return true; }
    if(nm=="SetCarCustomPlate"){
        auto P=MakePatches(G_CAR); c.BuildWorld(); c.FillMi(0); At<void*>(o,0x1C)=w.clump[0];
        tb.plateMaterial= c.chance(3) ? nullptr : static_cast<void*>(w.mat[c.next()%NMAT]);
        CallThis<void>(a,o); out["mi"]=c.Bytes(o+0x20,0x20); out["calls"]=tb.plateCalls; return true; }
    if(nm=="ReduceMaterialsInVehicle"){
        auto P=MakePatches(G_SCRATCH|G_NEW|G_TIMER|G_TREE|G_MATFX|G_VIS); c.BuildWorld(); c.FillMi(0); At<void*>(o,0x1C)=w.clump[0];
        At<void*>(o,0x5C)=c.FillVs(0,int(c.next()%7));
        for(int g=0;g<NGEO;++g){ for(int q=0;q<8;++q)At<void*>(w.mesh[g],0x18+0xC*q)=w.mat[c.next()%NMAT]; }
        std::memset(arena.data(),0xCD,0x400); arenaTop=0;
        CallThis<void>(a,o);
        P.~PatchSet(); new(&P) PatchSet(0);   // quita los parches ANTES de construir json: con operator new parcheado escribiria en la arena
        out["arena"]=c.Bytes(arena.data(),0x40); return true; }

    if(nm=="AssignRemapTxd"){
        auto P=MakePatches(G_CLUMP); c.BuildWorld(); c.FillMi(1); gmiMode= c.chance(5) ? 0 : 1; gmiType= c.chance(4) ? uint8(c.next()%10) : uint8(6);
        uint8* mi=w.mi[1]; const int n=int(c.next()%5);
        for(int k=0;k<4;++k)At<int16>(mi,0x2FA+2*k)= k<n ? int16(c.next()%300) : int16(-1);
        char* name=w.names[0]; std::memset(name,0,40);
        const uint32 base=1+c.next()%12; for(uint32 k=0;k<base;++k)name[k]=char('a'+c.next()%26);
        const uint32 digits=c.next()%4; for(uint32 k=0;k<digits;++k)name[base+k]=char('0'+c.next()%10);
        CallCdecl<void>(a,static_cast<const char*>(name),int16(int16(c.next())));
        out["remaps"]=c.Bytes(mi+0x2F0,0x20); return true; }

    if(nm=="PreprocessHierarchy"){
        auto P=MakePatches(G_TREE|G_RWOBJ|G_VIS);
        c.BuildWorld(int(2+c.next()%(NFR-1)));
        c.FillMi(0); At<void*>(o,0x1C)=w.clump[0];
        const int type=int(c.next()%12); At<int32>(o,0x3C)=type;
        const uint32 hid=c.next()%24; At<uint16>(o,0x4A)=uint16(hid|(c.chance(4) ? (c.next()%255)<<8 : 0u));
        for(int h=0;h<24;++h)SetHandlingFlags(h,c.chance(3) ? 0x20000000u : (c.next()&0x20000000u));
        uint8* vs=c.FillVs(0,0); At<uint32>(vs,0x310)=0; for(int u=0;u<18;++u)At<int32>(vs,0xB4+0x20*u+0x1C)=-1; At<void*>(o,0x5C)=vs;
        o[0x4C]=uint8(c.next());
        // tabla de componentes falsa (12 bytes por entrada: nombre, id de jerarquia, banderas)
        uint8* tbl=w.misc; std::memset(tbl,0,0x100);
        static const uint32 extraBits[]{0x20,0x40,0x80,0x100,0x400,0x800,0x1000,0x2000,0x4000,0x8000,0x40000,0x400000};
        const int n=1+int(c.next()%9); int extras=0, count=0;
        for(int k=0;k<n&&count<20;++k){
            uint32 kind=c.next()%9; if(kind==1&&extras>=6)kind=6;
            const int fj=int(c.next()%unsigned(c.treeN));
            uint32 flags=0; uint32 id=0; const char* nameP=nullptr;
            switch(kind){
            case 0: flags=0x8; id=c.next()%15; break;
            case 1: flags=0x200; id=c.next()%24; ++extras; break;
            case 2: flags=0x20000; id=c.next()%18; break;
            case 3: flags=0x10000|0x4; id=2+c.next()%6; break;
            case 4: flags=0x4; id=2+c.next()%6; break;
            case 5: flags=0x10|0x2; id=1+c.next()%23; break;
            case 6: flags=0x1; id=1+c.next()%23; break;
            case 7: flags=0x100000; id=1+c.next()%23; break;
            default: flags=0x200000; id=1+c.next()%23; break;
            }
            if(kind!=0&&kind!=2&&kind!=1)for(uint32 b:extraBits)if(c.chance(4))flags|=b;
            if(kind==1)for(uint32 b:extraBits)if(c.chance(5))flags|=b;
            const bool byName= kind==0||kind==1||kind==2;
            if(byName){ At<int32>(w.fr[fj],frameVisOff)=0; nameP=reinterpret_cast<const char*>(w.fr[fj]+nameOff); }
            else { At<int32>(w.fr[fj],frameVisOff)=int32(id); nameP=kFrameNames[c.next()%NFRAMENAMES]; if(tb.fobjN[fj]==0){ tb.fobj[fj][0]=w.at[c.next()%NAT]; tb.fobjN[fj]=1; } }
            if(byName&&kind==1&&tb.fobjN[fj]==0&&c.chance(2)){ tb.fobj[fj][0]=w.at[c.next()%NAT]; tb.fobjN[fj]=1; }
            At<const char*>(tbl,12*count)=nameP; At<uint32>(tbl,12*count+4)=id; At<uint32>(tbl,12*count+8)=flags; ++count;
        }
        At<const char*>(tbl,12*count)=nullptr;
        *reinterpret_cast<void**>(0x8A7740+4u*unsigned(type))=tbl;
        CallThis<void>(a,o);
        out["vs"]=c.Bytes(vs,0x314); out["doors"]=o[0x4C]; return true; }

    if(nm=="CreateInstance"){
        auto P=MakePatches(G_CLUMP|G_OWN|G_RWOBJ|G_TREE|G_MATFX); c.BuildWorld();
        static const int types[]{0,1,2,3,4,5,6,7,8,9,10,11,-1};
        c.FillMi(0,types[c.next()%std::size(types)]);
        const int ne=int(c.next()%7); At<void*>(o,0x5C)=c.FillVs(0,ne);
        for(int k=0;k<2;++k)tb.chooseRet[k]= c.chance(4) ? -1 : int32(c.next()%6);
        tb.frameForIdCount= c.chance(3) ? 0 : 1; tb.frameForId[0]=w.fr[c.next()%unsigned(c.treeN)];
        for(int k=0;k<2;++k)reinterpret_cast<int8*>(0xB4E478)[k]=int8(0x55);
        ret=Id(CallThis<void*>(a,o)); out["used"]=c.Bytes(reinterpret_cast<void*>(0xB4E478),2); return true; }
    if(nm=="SetClump"){
        auto P=MakePatches(G_CLUMP|G_OWN); c.BuildWorld(); std::memset(w.vsPool,0,sizeof(w.vsPool));
        static const int types[]{0,1,2,3,4,5,6,7,8,9,10,11};
        c.FillMi(0,types[c.next()%std::size(types)]); SetupFakePool(c);
        CallThis<void>(a,o,static_cast<void*>(w.clump[0]));
        out["pool"]=c.Bytes(w.poolObj,0x14); out["flags"]=c.Bytes(w.poolFlags,NVS); out["struct"]=Id(At<void*>(o,0x5C));
        out["cols"]=c.Bytes(o+0x2D0,8); return true; }
    if(nm=="DeleteRwObject"){
        auto P=MakePatches(G_CLUMP|G_RWOBJ); c.BuildWorld(); std::memset(w.vsPool,0,sizeof(w.vsPool));
        c.FillMi(0); SetupFakePool(c,true);
        const int s=int(c.next()%NVS); w.poolFlags[s]&=0x7F; uint8* vs=c.FillVs(s,int(c.next()%7));
        for(int e=0;e<6;++e)if(At<void*>(vs,0x2F4+4*e)){ At<void*>(w.at[e%NAT],4)=w.nfr[e]; }
        At<void*>(o,0x5C)= c.chance(5) ? nullptr : static_cast<void*>(vs);
        CallThis<void>(a,o);
        out["pool"]=c.Bytes(w.poolObj,0x14); out["flags"]=c.Bytes(w.poolFlags,NVS); out["struct"]=Id(At<void*>(o,0x5C)); return true; }
    return false;
}
}
