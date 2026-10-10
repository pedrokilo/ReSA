#pragma once
// Familia B: callbacks sobre materiales, atomics, geometrias y frames falsos; banderas de componentes; mapas de entorno.
#include "CasesA.hpp"

namespace VehModelInfoFinal4Live {
// Globales de las materiales editables (se guardan y restauran en cada ejecucion)
inline void SetupEditableGlobals(Ctx& c){
    *reinterpret_cast<void**>(0xB4E47C)= c.chance(5) ? nullptr : static_cast<void*>(w.tex[c.next()%NTEX]);   // ms_pRemapTexture
    *reinterpret_cast<void**>(0xB4E68C)= c.chance(8) ? nullptr : static_cast<void*>(w.tex[c.next()%NTEX]);   // ms_pLightsTexture
    *reinterpret_cast<void**>(0xB4E690)= static_cast<void*>(w.tex[c.next()%NTEX]);                             // ms_pLightsOnTexture
    for(int k=0;k<4;++k){ *reinterpret_cast<uint8*>(0xB4E3E8+k)=uint8(c.next()%2); *reinterpret_cast<uint8*>(0xB4E3F0+k)=uint8(c.next()%128); }
    c.fill(reinterpret_cast<void*>(0xB4E480),512);
    for(int k=0;k<3;++k)*reinterpret_cast<float*>(0x8A645C+4*k)=c.uni(0.f,2.f);
}
// Material con un color especial / textura especial para recorrer todas las ramas de SetEditableMaterialsCB
inline void MakeSpecialMaterial(Ctx& c,uint8* m){
    static const uint8 cols[][3]{{255,175,0},{0,255,200},{185,255,0},{255,60,0},{60,255,0},{255,0,175},{0,255,255},{255,0,255},{1,2,3},{255,255,255}};
    const uint32 k=c.next()%12;
    if(k<10){ m[4]=cols[k][0]; m[5]=cols[k][1]; m[6]=cols[k][2]; }
    m[7]= c.chance(3) ? uint8(c.next()) : uint8(255);
    const uint32 t=c.next()%5;
    if(t==0)At<void*>(m,0)=*reinterpret_cast<void**>(0xB4E68C);       // textura de luces
    else if(t==1){ At<void*>(m,0)=w.tex[c.next()%NTEX]; }
    else if(t==2)At<void*>(m,0)=nullptr;
    else if(t==3)At<void*>(m,0)=*reinterpret_cast<void**>(0xB4E690);
}
inline json EntriesOut(void* base,void* cur){
    json e=json::array(); const int n=int((reinterpret_cast<uint8*>(cur)-reinterpret_cast<uint8*>(base))/8);
    for(int k=0;k<n+1&&k<16;++k)e.push_back({Val(At<uint32>(base,8*k)),Val(At<uint32>(base,8*k+4))});
    return json{{"n",n},{"e",e}};
}

inline bool RunB(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; uint8* const o=w.mi[0];
    auto& out=c.out; auto& ret=c.ret;

    if(nm=="HasAlphaMaterialCB"){
        c.BuildWorld(); uint8* m=w.mat[c.next()%NMAT]; if(c.chance(2))m[7]=255;
        w.misc[0]=uint8(c.next()%2); ret=Id(CallCdecl<void*>(a,static_cast<void*>(m),static_cast<void*>(w.misc))); out["flag"]=w.misc[0]; return true; }
    if(nm=="RemoveWindowAlphaCB"){
        c.BuildWorld(); uint8* m=w.mat[c.next()%NMAT]; if(c.chance(2))m[7]=255;
        void* pp=w.entries; ret=Id(CallCdecl<void*>(a,static_cast<void*>(m),static_cast<void*>(&pp))); out["entries"]=EntriesOut(w.entries,pp); return true; }
    if(nm=="SetEditableMaterialsCB-RpMaterial"){
        c.BuildWorld(); SetupEditableGlobals(c); uint8* m=w.mat[c.next()%NMAT]; MakeSpecialMaterial(c,m);
        void* pp=w.entries; ret=Id(CallCdecl<void*>(a,static_cast<void*>(m),static_cast<void*>(&pp))); out["entries"]=EntriesOut(w.entries,pp);
        out["mat"]=c.Bytes(m,0x20); return true; }
    if(nm=="SetEditableMaterialsCB-RpAtomic"){
        auto P=MakePatches(G_TREE); c.BuildWorld(); SetupEditableGlobals(c);
        uint8* at=w.at[c.next()%NAT]; at[2]= c.chance(4) ? 0 : 4; At<uint16>(at,atomVisOff+2)= c.chance(2) ? 0x1000u|(c.next()&0xFFu) : (c.next()&0xEFFFu);
        const int g=GeoIdx(At<void*>(at,0x18)); for(int q=0;q<tb.geoMatN[g];++q)MakeSpecialMaterial(c,static_cast<uint8*>(tb.geoMat[g][q]));
        void* pp=w.entries; ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),static_cast<void*>(&pp))); out["entries"]=EntriesOut(w.entries,pp); return true; }
    if(nm=="SetEditableMaterials"){
        auto P=MakePatches(G_TREE); c.BuildWorld(); SetupEditableGlobals(c);
        c.fill(reinterpret_cast<void*>(0xB4DBE8),2048);
        for(int k=0;k<NAT;++k){ uint8* at=w.at[k]; at[2]= c.chance(5) ? 0 : 4; At<uint16>(at,atomVisOff+2)= c.chance(3) ? 0x1000u|(c.next()&0xFFu) : (c.next()&0xEFFFu); }
        for(int g=0;g<NGEO;++g)for(int q=0;q<tb.geoMatN[g];++q)MakeSpecialMaterial(c,static_cast<uint8*>(tb.geoMat[g][q]));
        CallCdecl<void>(a,static_cast<void*>(w.clump[0]));
        json e=json::array(); for(int k=0;k<40;++k)e.push_back({Val(At<uint32>(reinterpret_cast<void*>(0xB4DBE8),8*k)),Val(At<uint32>(reinterpret_cast<void*>(0xB4DBE8),8*k+4))});
        out["entries"]=e; out["rest"]=Fnv64(reinterpret_cast<void*>(0xB4DBE8),2048); return true; }
    if(nm=="ResetEditableMaterials"){
        c.BuildWorld(); const int n=int(c.next()%12); auto* ent=reinterpret_cast<uint8*>(0xB4DBE8);
        c.fill(ent,2048);
        for(int k=0;k<n;++k){ uint8* target= c.chance(2) ? static_cast<uint8*>(w.mat[c.next()%NMAT]) : static_cast<uint8*>(w.tex[c.next()%NTEX]);
            At<void*>(ent,8*k)=target+4*(c.next()%8); At<uint32>(ent,8*k+4)=c.next(); }
        At<void*>(ent,8*n)=nullptr;
        CallCdecl<void>(a,static_cast<void*>(w.clump[0])); return true; }

    if(nm=="HideDamagedAtomicCB"||nm=="HideAllComponentsAtomicCB"){
        c.BuildWorld(); uint8* at=w.at[c.next()%NAT]; at[2]=uint8(c.next());
        if(nm=="HideDamagedAtomicCB"){ ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),static_cast<void*>(nullptr))); }
        else { const uint32 d= c.chance(3) ? c.next() : (c.next()&0xFFFFu); ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),reinterpret_cast<void*>(uintptr_t(d)))); }
        out["flags"]=at[2]; out["id"]=At<uint16>(at,atomVisOff+2); return true; }
    if(nm=="SetAtomicRendererCB"||nm=="SetAtomicRendererCB_RealHeli"||nm=="SetAtomicRendererCB_Plane"||nm=="SetAtomicRendererCB_Boat"
       ||nm=="SetAtomicRendererCB_Heli"||nm=="SetAtomicRendererCB_Train"){
        auto P=MakePatches(G_TREE|G_VIS); c.BuildWorld(); uint8* at=w.at[c.next()%NAT]; at[2]=uint8(c.next());
        const int g=GeoIdx(At<void*>(at,0x18)); for(int q=0;q<tb.geoMatN[g];++q){ uint8* m=static_cast<uint8*>(tb.geoMat[g][q]); m[7]= c.chance(2) ? 255 : uint8(c.next()); }
        // el nombre del frame del atomic: de la lista, con prefijos largos que prueban strncmp/strstr
        uint8* fr=static_cast<uint8*>(At<void*>(at,4)); std::memset(fr+nameOff,0,24);
        const char* nmx=kFrameNames[c.next()%NFRAMENAMES]; std::memcpy(fr+nameOff,nmx,std::min<size_t>(strlen(nmx),23));
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),static_cast<void*>(w.clump[0]))); out["flags"]=at[2]; return true; }
    if(nm=="SetAtomicFlagCB"||nm=="ClearAtomicFlagCB"){
        c.BuildWorld(); uint8* at=w.at[c.next()%NAT]; const uint32 d= c.chance(4) ? c.next() : (c.next()&0xFFFFu);
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),reinterpret_cast<void*>(uintptr_t(d)))); out["id"]=At<uint16>(at,atomVisOff+2); return true; }
    if(nm=="GetOkAndDamagedAtomicCB"){
        c.BuildWorld(); uint8* at=w.at[c.next()%NAT]; At<uint16>(at,atomVisOff+2)=uint16(c.next()%4 ? c.next()&0x3 : c.next());
        At<uint32>(w.misc,0)=0xCDCDCDCDu; At<uint32>(w.misc,4)=0xCDCDCDCDu;
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),static_cast<void*>(w.misc))); out["arr"]=json::array({Val(At<uint32>(w.misc,0)),Val(At<uint32>(w.misc,4))}); return true; }
    if(nm=="MoveObjectsCB"){
        auto P=MakePatches(G_RWOBJ); c.BuildWorld(); uint8* at=w.at[c.next()%NAT]; uint8* fr=w.fr[c.next()%NFR];
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),static_cast<void*>(fr))); return true; }
    if(nm=="CollapseFramesCB"){
        auto P=MakePatches(G_TREE|G_RWOBJ); c.BuildWorld(int(1+c.next()%NFR));
        uint8* fr=w.fr[c.next()%unsigned(c.treeN)]; uint8* data=w.fr[c.next()%unsigned(c.treeN)];
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(fr),static_cast<void*>(data))); return true; }
    if(nm=="StoreAtomicUsedMaterialsCB"){
        auto P=MakePatches(G_MATFX); c.BuildWorld(); uint8* at=w.at[c.next()%NAT];
        At<uint16>(at,atomVisOff+2)= c.chance(2) ? 0x2000u : (c.next()&0xDFFFu);
        std::memset(w.matList,0,0x20); At<void*>(w.matList,0)=w.misc; At<int32>(w.matList,4)=int32(c.next()%6); At<int32>(w.matList,8)=20;
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),static_cast<void*>(w.matList))); return true; }
    if(nm=="GetMatFXEffectMaterialCB"){
        auto P=MakePatches(G_MATFX); c.BuildWorld(); uint8* m=w.mat[c.next()%NMAT]; At<int32>(w.misc,0)=int32(0xCDCDCDCD);
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(m),static_cast<void*>(w.misc))); out["val"]=At<int32>(w.misc,0); return true; }
    if(nm=="SetEnvironmentMapCB"||nm=="SetEnvMapCoeffCB"){
        auto P=MakePatches(G_MATFX); c.BuildWorld(); const int mi=int(c.next()%NMAT); uint8* m=w.mat[mi];
        if(c.chance(2))tb.matEff[mi]=2;
        *reinterpret_cast<void**>(0xB4E6B8)= c.chance(3) ? nullptr : static_cast<void*>(w.nfr[0]);   // carFrame
        *reinterpret_cast<void**>(0xB4E3EC)= static_cast<void*>(w.tex[c.next()%NTEX]);              // gpWhiteTexture
        if(c.chance(3))At<void*>(m,0)=nullptr;
        uint32 d;
        if(nm=="SetEnvironmentMapCB"){ static const uint32 v[]{0xFFFFu,0x1FFFFu,0x10000u,0u,0xFFFEu,0x2FFFFu,0xFFFFFFFFu,5u,0x80000000u}; d= c.chance(2) ? 0xFFFFu : v[c.next()%std::size(v)]; }
        else { const uint32 k=c.next()%6; d= k==0 ? c.next() : k==1 ? uint32(-int32(c.next()%2000)) : k==2 ? 0x7FFFFFFFu : k==3 ? 0x80000000u : (c.next()%2000); }
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(m),reinterpret_cast<void*>(uintptr_t(d)))); return true; }
    if(nm=="SetRenderPipelinesCB"){ c.BuildWorld(); ret=Id(CallCdecl<void*>(a,static_cast<void*>(w.at[c.next()%NAT]),static_cast<void*>(w.misc))); return true; }
    if(nm=="SetEnvironmentMapAtomicCB"||nm=="SetEnvMapCoeffAtomicCB"){
        auto P=MakePatches(G_TREE|G_MATFX); c.BuildWorld(); const uint32 d=c.next();
        ret=Id(CallCdecl<void*>(a,static_cast<void*>(w.at[c.next()%NAT]),reinterpret_cast<void*>(uintptr_t(d)))); return true; }

    if(nm=="DisableEnvMap"||nm=="SetEnvMapCoeff"||nm=="SetAtomicRenderCallbacks"){
        auto P=MakePatches(G_TREE|G_MATFX|G_VIS); c.BuildWorld(); c.FillMi(0);
        At<void*>(o,0x1C)= c.chance(4) ? nullptr : static_cast<void*>(w.clump[0]);
        if(nm=="DisableEnvMap")CallThis<void>(a,o);
        else if(nm=="SetEnvMapCoeff"){ const uint32 k=c.next()%8; const float v= k==0 ? std::bit_cast<float>(0x7FC00000u) : k==1 ? c.uni(-1e10f,1e10f) : k==2 ? 0.f : k==3 ? -c.uni(0.f,3.f) : c.uni(-0.5f,4.f);
            CallThis<void>(a,o,v); }
        else { static const int types[]{-1,0,1,2,3,4,5,6,7,8,9,10,11}; At<int32>(o,0x3C)=types[c.next()%std::size(types)]; CallThis<void>(a,o); }
        return true; }
    if(nm=="SetDirtTextures"){
        auto P=MakePatches(G_MATFX); c.BuildWorld(); c.FillMi(0);
        for(int k=0;k<32;++k)At<void*>(o,0x230+4*k)= c.chance(4) ? nullptr : static_cast<void*>(w.mat[c.next()%NMAT]);
        for(int k=0;k<16;++k)*reinterpret_cast<void**>(0xC02BD0+4*k)=w.tex[c.next()%NTEX];
        CallCdecl<void>(a,static_cast<void*>(o),int32(c.next()%16)); return true; }
    if(nm=="SetVehicleComponentFlags"){
        auto P=MakePatches(G_TREE); c.BuildWorld(); c.FillMi(0);
        uint32 fl=0; for(int b=0;b<32;++b)if(c.chance(3))fl|=1u<<b; if(c.chance(6))fl=c.next();
        const uint32 hid=c.next()%24; At<uint16>(o,0x4A)=uint16(hid|(c.chance(3) ? (c.next()%255)<<8 : 0u));
        for(int h=0;h<24;++h)SetHandlingFlags(h,c.next()&(c.chance(4) ? 0xFFFFFFFFu : 0x80000201u));
        uint8* fr=w.fr[c.next()%NFR]; CallThis<void>(a,o,static_cast<void*>(fr),fl); return true; }
    return false;
}
}
