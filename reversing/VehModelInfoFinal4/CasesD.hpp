#pragma once
// Familia D: texturas comunes, mapas de entorno, ficheros de datos (CARCOLS/CARMODS) y SetupCommonData, siempre con dobles.
#include "CasesC.hpp"

namespace VehModelInfoFinal4Live {
inline std::array<std::array<char,160>,48> lineStore{};
inline void AddLine(const char* t){ if(fileLineCount<lineStore.size()){ std::snprintf(lineStore[fileLineCount].data(),160,"%s",t); fileLines[fileLineCount]=lineStore[fileLineCount].data(); ++fileLineCount; } }
inline void AddLinef(const char* fmt,int a,int b=0,int cc=0,int d=0){ char buf[160]; std::snprintf(buf,160,fmt,a,b,cc,d); AddLine(buf); }
inline const char* const kModelNames[]{"infernus","bullet","admiral","bus","hydralics","stereo","wheel_sport1","nto_b_l","spl_b_mar_m","bnt_chr_bar"};
inline const char* ModelName(Ctx& c){ return kModelNames[c.next()%std::size(kModelNames)]; }

inline bool RunD(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; uint8* const o=w.mi[0];
    auto& out=c.out; auto& ret=c.ret; (void)o;

    if(nm=="FindTextureCB"){
        auto P=MakePatches(G_TEX); c.BuildWorld();
        *reinterpret_cast<void**>(0xB4E688)=w.dict[0];
        char* name=w.names[0]; std::memset(name,0,40);
        if(c.next()%5<3){ const char* src=reinterpret_cast<const char*>(w.tex[c.next()%NTEX])+0x10; std::strncpy(name,src,31); if(c.chance(4))name[0]='r'; }
        else { const char* t=kTexNames[c.next()%NTEXNAMES]; std::strncpy(name,t,31); }
        ret=Id(CallCdecl<void*>(a,static_cast<const char*>(name)));
        out["names"]=c.Bytes(w.tex,sizeof(w.tex)); return true; }
    if(nm=="UseCommonVehicleTexDicationary"){
        auto P=MakePatches(G_TEX); c.BuildWorld(); *reinterpret_cast<uint32*>(0xB4E6A0)=c.next();
        CallCdecl<void>(a); out["saved"]=Val(*reinterpret_cast<uint32*>(0xB4E6A0)); return true; }
    if(nm=="StopUsingCommonVehicleTexDicationary"){
        auto P=MakePatches(G_TEX); c.BuildWorld(); *reinterpret_cast<uint32*>(0xB4E6A0)=c.chance(4) ? 0u : (c.next()&0xFFFFFFu);
        CallCdecl<void>(a); out["saved"]=*reinterpret_cast<uint32*>(0xB4E6A0); return true; }
    if(nm=="ShutdownLightTexture"){
        auto P=MakePatches(G_TEX); c.BuildWorld();
        *reinterpret_cast<void**>(0xB4E68C)= c.chance(3) ? nullptr : static_cast<void*>(w.tex[c.next()%NTEX]);
        *reinterpret_cast<void**>(0xB4E690)= c.chance(3) ? nullptr : static_cast<void*>(w.tex[c.next()%NTEX]);
        CallCdecl<void>(a); out["lights"]=json::array({Val(*reinterpret_cast<uint32*>(0xB4E68C)),Val(*reinterpret_cast<uint32*>(0xB4E690))}); return true; }
    if(nm=="LoadEnvironmentMaps"){
        auto P=MakePatches(G_TXD|G_TEX); c.BuildWorld();
        *reinterpret_cast<void**>(0xB4E3EC)= c.chance(2) ? nullptr : static_cast<void*>(w.tex[c.next()%NTEX]);
        CallCdecl<void>(a); out["white"]=Val(*reinterpret_cast<uint32*>(0xB4E3EC)); return true; }
    if(nm=="ShutdownEnvironmentMaps"){
        auto P=MakePatches(G_TEX|G_RWOBJ); c.BuildWorld();
        *reinterpret_cast<void**>(0xB4E3EC)= static_cast<void*>(w.tex[c.next()%NTEX]);
        *reinterpret_cast<void**>(0xB4E6B8)= c.chance(3) ? nullptr : static_cast<void*>(w.nfr[c.next()%NNF]);
        *reinterpret_cast<uint8*>(0xB4E6BC)=uint8(c.next());
        CallCdecl<void>(a);
        out["g"]=json::array({Val(*reinterpret_cast<uint32*>(0xB4E3EC)),Val(*reinterpret_cast<uint32*>(0xB4E6B8)),*reinterpret_cast<uint8*>(0xB4E6BC)}); return true; }

    if(nm=="LoadVehicleColours"){
        auto P=MakePatches(G_FILE); c.BuildWorld(); for(int k=0;k<NMI;++k)c.FillMi(k);
        c.fill(reinterpret_cast<void*>(0xB4E480),512);
        fileLineCount=0; fileLinePos=0;
        AddLine("# carcols\n"); if(c.chance(4))AddLine("\n"); if(c.chance(4))AddLine("   \t \n");
        if(c.chance(3))AddLine("junk line\n");
        AddLine("col\n");
        const int nc=1+int(c.next()%8);
        for(int k=0;k<nc;++k){
            if(k>0&&c.chance(8)){ AddLine("77.93,96\t\t\t# 98 malachite poly\t\t\tblue\n"); continue; }
            const char* ending= c.chance(4) ? "\r\n" : "\n"; char buf[160];
            std::snprintf(buf,160,"%s%d,%d,%d # col %d%s",c.chance(5) ? "  \t" : "",int(c.next()%256),int(c.next()%256),int(c.next()%256),k,ending); AddLine(buf);
            if(c.chance(10))AddLine("# comentario\n"); }
        AddLine("end\n");
        AddLine("car\n"); const int n2=int(c.next()%3);
        for(int k=0;k<n2;++k){ char buf[160]; const int nv=1+int(c.next()%8); int len=std::snprintf(buf,160,"%s",ModelName(c));
            for(int v=0;v<nv;++v)len+=std::snprintf(buf+len,160-len,", %d,%d",int(c.next()%126),int(c.next()%126)); std::snprintf(buf+len,160-len,"\n"); AddLine(buf); }
        AddLine("end\n");
        AddLine("car4\n"); const int n4=int(c.next()%3);
        for(int k=0;k<n4;++k){ char buf[160]; const int nv=1+int(c.next()%8); int len=std::snprintf(buf,160,"%s",ModelName(c));
            for(int v=0;v<nv;++v)len+=std::snprintf(buf+len,160-len,", %d,%d,%d,%d",int(c.next()%126),int(c.next()%126),int(c.next()%126),int(c.next()%126)); std::snprintf(buf+len,160-len,"\n"); AddLine(buf); }
        AddLine("end\n");
        CallCdecl<void>(a);
        out["table"]=c.Bytes(reinterpret_cast<void*>(0xB4E480),512); return true; }
    if(nm=="LoadVehicleUpgrades"){
        auto P=MakePatches(G_FILE|G_UPG); c.BuildWorld(); for(int k=0;k<NMI;++k)c.FillMi(k);
        c.fill(reinterpret_cast<void*>(0xB4E3F8),120); c.fill(reinterpret_cast<void*>(0xB4E470),8);
        std::memset(reinterpret_cast<void*>(0xB4E6D8),0,0x7C);
        fileLineCount=0; fileLinePos=0;
        AddLine("# carmods"); if(c.chance(4))AddLine("");
        AddLine("link"); const int nl=int(c.next()%5);
        for(int k=0;k<nl;++k){ char buf[160]; std::snprintf(buf,160,"%s, %s",ModelName(c),ModelName(c)); AddLine(buf); }
        AddLine("end");
        AddLine("mods"); const int nmods=int(c.next()%3);
        for(int k=0;k<nmods;++k){ char buf[160]; int len=std::snprintf(buf,160,"%s",ModelName(c)); const int nu=int(c.next()%15);
            for(int u=0;u<nu;++u)len+=std::snprintf(buf+len,160-len,"%s%s",c.chance(3) ? "\t" : ", ",ModelName(c)); AddLine(buf); }
        AddLine("end");
        AddLine("wheel"); const int nw=1+int(c.next()%2); int budget=14;
        for(int k=0;k<nw;++k){ char buf[160]; int len=std::snprintf(buf,160,"%d",int(c.next()%4)); const int nt=int(c.next()%unsigned(budget/2+1)); budget-=nt;
            for(int u=0;u<nt;++u)len+=std::snprintf(buf+len,160-len,", %s",ModelName(c)); AddLine(buf); }
        AddLine("end");
        CallCdecl<void>(a);
        out["wheels"]=c.Bytes(reinterpret_cast<void*>(0xB4E3F8),128); out["links"]=c.Bytes(reinterpret_cast<void*>(0xB4E6D8),0x7C); return true; }

    if(nm=="SetupCommonData"){
        auto P=MakePatches(G_LOADERS|G_FILE|G_TXD|G_TEX|G_CAR|G_NEW); c.BuildWorld();
        std::memset(w.txdPool,0,sizeof(w.txdPool)); std::memset(w.txdObj,0,sizeof(w.txdObj));
        At<void*>(w.txdPool,0)=w.txdObj; At<void*>(w.txdPool,4)=w.txdFlags; At<int32>(w.txdPool,8)=4;
        for(int k=0;k<4;++k){ w.txdFlags[k]=0; At<void*>(w.txdObj[k],0)=w.dict[0]; }   // ranuras usadas (0x5B8F79: bit 7 = libre)
        At<int32>(w.misc,0)= c.chance(2) ? -1 : int32(c.next()%4);                       // ranura que devuelve FindTxdSlot("vehicle")
        *reinterpret_cast<void**>(0xC8800C)=w.txdPool;
        *reinterpret_cast<void**>(0xB4E688)=nullptr;
        std::memset(arena.data(),0xCD,arena.size()); arenaTop=0;
        CallCdecl<void>(a);
        P.~PatchSet(); new(&P) PatchSet(0);   // quita los parches ANTES de construir json: con operator new parcheado escribiria en la arena
        out["txd"]=Id(*reinterpret_cast<void**>(0xB4E688)); out["lights"]=json::array({Id(*reinterpret_cast<void**>(0xB4E68C)),Id(*reinterpret_cast<void**>(0xB4E690))});
        uint8* pool=*reinterpret_cast<uint8**>(0xB4E680);
        out["pool"]=Id(pool);
        // Ruido conocido de la DLL: CPool(...) rellena el almacenamiento con 0xFD (NOMANSLAND) y pone a 0 m_DealWithNoMemory (+0x11);
        // el EXE (0x5B8340) no escribe ninguno de los dos. Se enmascaran antes del hash de la arena.
        if(Known(pool)){ std::memset(At<void*>(pool,0),0xCD,50*0x314); pool[0x11]=0xCD; pool[0x12]=0xCD; pool[0x13]=0xCD; }
        if(Known(pool)){ out["poolHdr"]=json::array({Val(At<uint32>(pool,0)),Val(At<uint32>(pool,4)),At<int32>(pool,8),At<int32>(pool,0xC),pool[0x10]});
            out["poolFlags"]=c.Bytes(At<void*>(pool,4),50); }
        return true; }
    return false;
}
}
