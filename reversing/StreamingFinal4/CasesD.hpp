#pragma once
// Familia D: streaming de vehiculos y peatones, y arranque / cierre / ficheros (Init2, LoadCdDirectory, ReadIniFile, Load, Save...).
#include "CasesC.hpp"

namespace StreamingFinal4Live {
alignas(16) inline std::array<uint8,4096> imgFile{};
inline std::array<std::array<char,48>,40> iniStore{};

// escribe una entrada del directorio (32 bytes) y devuelve el siguiente desplazamiento
inline void PutEntry(uint8* e,uint32 off,uint16 size,uint16 inArchive,const char* name){
    std::memset(e,0,32); At<uint32>(e,0)=off; At<uint16>(e,4)=size; At<uint16>(e,6)=inArchive; std::strncpy(reinterpret_cast<char*>(e)+8,name,24);
}

inline bool RunD(Ctx& c){
    const std::string& nm=c.nm;

    if(nm=="StreamAmbulanceAndMedic"||nm=="StreamFireEngineAndFireman"){
        Prep(c); AtA<int32>(CURRLEVEL)=int32(c.next()%4); RUN_RET(bool, M_EXT, int32(c.next()&1)); return true; }
    if(nm=="StreamCopModels"){ Prep(c); RUN_VOID(M_EXT, int32(c.next()%5)); return true; }
    if(nm=="StreamOneNewCar"){
        Prep(c); AtA<int32>(CURRLEVEL)=int32(c.next()%4);
        for(int g=0;g<18;++g)if(c.pct(25)){ auto* n=reinterpret_cast<uint16*>(0xC0EC78); auto* cars=reinterpret_cast<uint16*>(0xC0ED38); if(n[g]==0)n[g]=1; cars[g*23]=uint16(c.pct(50) ? 420 : 438); }
        RUN_VOID(M_EXT); return true; }
    if(nm=="StreamPedsForInterior"){ Prep(c); RUN_VOID(M_EXT, int32(c.next()%3)); return true; }
    if(nm=="StreamPedsIntoRandomSlots"){
        Prep(c); static std::array<int32,8> arr{}; for(auto& v:arr){ const uint32 k=c.next()%6; v= k<2 ? -1 : k<3 ? -2 : PedId(c.r); }
        RUN_VOID(M_EXT, static_cast<void*>(arr.data())); return true; }
    if(nm=="StreamVehiclesAndPeds"){ Prep(c); rs.haveWanted=true; RUN_VOID(M_EXT); return true; }
    if(nm=="StreamVehiclesAndPeds_Always"){ Prep(c); const CVector v=RandPoint(c); RUN_VOID(M_EXT|B_STREAMONE|B_GANGS|B_RECLASS, Pt(v)); return true; }
    if(nm=="StreamZoneModels"){ Prep(c); const CVector v=RandPoint(c); RUN_VOID(M_EXT, Pt(v)); return true; }
    if(nm=="StreamZoneModels_Gangs"){
        Prep(c); const CVector v=RandPoint(c);
        RUN_VOID(M_EXT, Pt(v)); return true; }

    if(nm=="InitImageList"){
        Prep(c); for(int k=0;k<8;++k){ auto* f=reinterpret_cast<uint8*>(0x8E48D8)+0x30*size_t(k); for(size_t o=0;o<0x30;++o)f[o]=uint8(c.next()); }
        RUN_VOID(M_EXT); return true; }
    if(nm=="Init2"){
        Prep(c); CStreaming::ms_streamingBufferSize=0;
        RUN_VOID(M_EXT|E_NEW|E_MEM|E_DIR|E_SCRIPT|B_LOADCDDIR); return true; }
    if(nm=="Shutdown"){
        Prep(c); CStreaming::ms_pStreamingBuffer[0]=streamBuf0.data(); CStreaming::ms_pStreamingBuffer[1]=streamBuf1.data();
        if(c.pct(20))CStreaming::ms_pExtraObjectsDir=nullptr;
        RUN_VOID(M_EXT|E_NEW|E_MEM|E_DIR); return true; }
    if(nm=="Load"||nm=="Save"){
        Prep(c); for(size_t k=0;k<sizeof(rs.loadBytes);++k)rs.loadBytes[k]= c.pct(40) ? uint8(0xFF) : uint8(c.next());
        RUN_RET(bool, M_EXT|E_STORE); return true; }

    if(nm=="LoadCdDirectory-char"||nm=="LoadCdDirectory-void"){
        Prep(c); const bool withName= nm=="LoadCdDirectory-char";
        // imagen falsa: version, numero de entradas y entradas de 32 bytes
        std::memset(imgFile.data(),0,imgFile.size()); std::memcpy(imgFile.data(),"VER2",4);
        const int n= c.pct(8) ? 0 : 1+int(c.next()%24); At<int32>(imgFile.data(),4)=n;
        static const char* const pat[]{"car%d.dff","unk%d.dff","gen%d.txd","col%d.col","ipl%d.ipl","nodes%d.dat","anim%d.ifp","rec%d.rrr","scr%d.scm","other%d.xyz","noext%d",
                                          "CAR%d.DFF","Gen%d.TxD","dffx%d.dff","a.b%d.dff","txd%d.txdx","m%d.IfP"};
        char name[40]; char prev[40]{};
        for(int k=0;k<n;++k){
            const uint32 sel=c.next()%std::size(pat);
            if(k>0&&c.pct(25)){ std::strcpy(name,prev); } else { std::snprintf(name,sizeof(name),pat[sel],int(c.next()%(sel==5 ? 60u : 40u))); }
            std::strcpy(prev,name);
            const uint16 size= c.pct(10) ? uint16(0) : uint16(1+c.next()%50); const uint16 inArchive= c.pct(30) ? uint16(1+c.next()%50) : uint16(0);
            PutEntry(imgFile.data()+8+32*size_t(k),c.next()&0xFFFFFFu,size,inArchive,name);
        }
        rs.fileImg=imgFile.data(); rs.fileImgLen=8+32*size_t(n); rs.filePos=0;
        // informacion previa para provocar el caso "ya tiene posicion y tamano"
        for(int k=0;k<12;++k){ const int32 id=400+int32(c.next()%200); if(c.pct(50)){ iSize(id)=1+c.next()%9; iCd(id)=-1; } }
        for(int k=0;k<6;++k){ const int32 id=RESOURCE_ID_TXD+int32(c.next()%NTXD); if(c.pct(50))iSize(id)=1+c.next()%9; }
        const uint64 m= M_EXT|E_FILE|E_DIR;
        if(withName){ const int32 img=int32(c.next()%4); RUN_VOID(m, "MODELS\\TEST.IMG", img); }
        else {
            for(int k=0;k<8;++k){ auto* f=reinterpret_cast<uint8*>(0x8E48D8)+0x30*size_t(k); std::memset(f,0,0x30);
                if(k<(c.pct(15) ? 0 : 1+int(c.next()%8))&&!c.pct(15)){ std::snprintf(reinterpret_cast<char*>(f),0x28,"MODELS\\IMG%d.IMG",k); f[0x28]=uint8(c.pct(70) ? 1 : 0); At<uint32>(f,0x2C)=uint32(k+4)<<24; } }
            RUN_VOID(m); }
        return true; }
    if(nm=="ReadIniFile"){
        Prep(c);
        static const char* const lines[]{"# comentario","","memory 4096","memory 2048","devkit_memory 8192","vehicles 15","vehicles 30","dontbuildpaths","pe_lightchangerate 0.0005",
            "pe_lightingbasecap 0.35","pe_lightingbasemult 1.2","pe_leftx 8","pe_rightx 16","pe_topy 4","pe_bottomy 12","pe_bRadiosity 1","pe_bRadiosity 0","def_brightness_pal 280",
            "unknown_key 5","MEMORY 1024","devkit_memory,3000","vehicles\t22","memory   100","Vehicles 18","PE_LEFTX 3","#devkit_memory 9","pe_lightchangerate 0.0035","pe_lightingbasecap 0.4"};
        const int n=1+int(c.next()%20); rs.iniCount=0;
        for(int k=0;k<n;++k){
            const uint32 sel=c.next()%(std::size(lines)+4);
            if(sel<std::size(lines)){ rs.iniLines[rs.iniCount++]=lines[sel]; }
            else { auto& b=iniStore[size_t(k)]; const uint32 q=sel-uint32(std::size(lines));
                   if(q==0)std::snprintf(b.data(),b.size(),"memory %u",c.next()%100000); else if(q==1)std::snprintf(b.data(),b.size(),"pe_lightchangerate 0.%06u",c.next()%1000000);
                   else if(q==2)std::snprintf(b.data(),b.size(),"pe_topy -%u",c.next()%500); else std::snprintf(b.data(),b.size(),"pe_lightingbasemult %u.%05u",c.next()%5,c.next()%100000);
                   rs.iniLines[rs.iniCount++]=b.data(); }
        }
        rs.iniPos=0;
        RUN_VOID(M_EXT|E_FILE); return true; }
    return false;
}
}
