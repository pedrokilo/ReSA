#pragma once
// Familia C: lectura de canales, conversion de buffers, orden de lectura y bucles principales (Update, LoadScene...).
#include "CasesB.hpp"

namespace StreamingFinal4Live {
inline void PutInChannel(int ch,int slot,int32 id){
    auto* e=Chan(ch); At<int32>(e,4*slot)=id; At<int32>(e,0x40+4*slot)=int32((slot*7)%40); if(id>=0&&id<NTOTAL)iState(id)=3;
}
// Deja el canal en el estado pedido: 0 libre, 1 leyendo, 2 empezado (modelo grande), 3 error
inline void ForceChannel(Ctx& c,int ch,int status){
    auto* e=Chan(ch); bool has=false; for(int k=0;k<16;++k)if(At<int32>(e,4*k)>=0)has=true;
    if((status==1||status==3)&&!has){ PutInChannel(ch,0,PickWset(c)); if(c.pct(40))PutInChannel(ch,1,PickWset(c)); }
    if(status==2){
        int32 id=-1; for(int t=0;t<24;++t){ const int32 m=PickWset(c); if(m<20000||(m>=20000&&m<25000)){ id=m; break; } }
        if(id<0)id=TxdId(c.r);
        for(int k=0;k<16;++k)At<int32>(e,4*k)=-1;
        PutInChannel(ch,0,id); iState(id)=4; CStreaming::ms_bLoadingBigModel=true;
    }
    At<int32>(e,0x80)=status;
}

inline bool RunC(Ctx& c){
    const std::string& nm=c.nm;

    if(nm=="ConvertBufferToObject"||nm=="FinishLoadingLargeFile"){
        Prep(c); const bool conv=nm=="ConvertBufferToObject";
        int32 id; { const uint32 k=c.next()%10; id= k<4 ? PickDffWset(c) : k<7 ? TxdId(c.r) : PickWset(c); if(!conv&&c.pct(70))id= c.pct(60) ? TxdId(c.r) : PickDffWset(c); }
        if(!conv)id= c.pct(55) ? TxdId(c.r) : PickDffWset(c);                                   // el EXE solo trata DFF y TXD (otro id leeria una local sin inicializar)
        iState(id)= conv ? uint8(c.pct(80) ? 3 : c.next()%5) : uint8(c.pct(75) ? 4 : c.next()%5);
        iSize(id)=1+c.next()%12; iFlags(id)=uint8(c.next()&0x3F);
        if(id<20000){ auto* mi=miMem[size_t(id)%NMI].data(); auto* txd=txdMem.data()+12*size_t(At<int16>(mi,0xA)); if(c.pct(80))At<void*>(txd,0)=rwObjs[3].data();
                      const int32 an=At<int32>(mi,0x64); if(an>=0&&c.pct(80))reinterpret_cast<uint8*>(ANIMBLK)[0x20*size_t(an)]=1; }
        if(id>=20000&&id<25000){ auto* t=txdMem.data()+12*size_t(id-20000); const int16 par=At<int16>(t,6); if(par>=0&&c.pct(75))At<void*>(txdMem.data()+12*size_t(par),0)=rwObjs[4].data(); }
        CStreaming::ms_bLoadingBigModel=c.pct(25);
        uint8* buf= c.pct(50) ? streamBuf0.data()+2048*(c.next()%20) : streamBuf1.data()+2048*(c.next()%20);
        if(conv){ RUN_RET(bool, M_EXT, buf, id); }
        else { RUN_VOID(M_EXT, buf, id); }
        return true; }
    if(nm=="GetNextFileOnCd"){
        Prep(c); uint32 pos; const uint32 k=c.next()%5; if(k==0)pos=0; else if(k==1)pos=c.next(); else if(k==2){ pos=iOff(PickWset(c)); } else pos=(c.next()&1u)<<24|(c.next()%2000);
        RUN_RET(int32, M_EXT, pos, int32(c.next()&1)); return true; }
    if(nm=="RequestModelStream"){
        Prep(c); const int ch=int(c.next()&1); if(c.pct(25))CStreaming::ms_bLoadingBigModel=false;
        if(c.pct(20))ForceChannel(c,1-ch,c.pct(50) ? 1 : 0);
        RUN_VOID(M_EXT, ch); return true; }
    if(nm=="RequestFilesInChannel"){ Prep(c); const int ch=int(c.next()&1); ForceChannel(c,ch,1); if(c.pct(50))ForceChannel(c,1-ch,1); RUN_VOID(M_EXT, ch); return true; }
    if(nm=="ProcessLoadingChannel"){
        Prep(c); const int ch=int(c.next()&1); const uint32 k=c.next()%10;
        if(k<2){ } else if(k<5)ForceChannel(c,ch,1); else if(k<7)ForceChannel(c,ch,2); else if(k<9)ForceChannel(c,ch,3); else ForceChannel(c,ch,0);
        if(c.pct(30))ForceChannel(c,1-ch,c.pct(50) ? 1 : 0);
        if(c.pct(30))CStreaming::ms_channelError=c.pct(50) ? -1 : int32(c.next()&1);
        RUN_RET(bool, M_EXT|B_MAKESPACE|B_RETRY, ch); return true; }
    if(nm=="RetryLoadFile"){
        Prep(c); const int ch=int(c.next()&1); const uint32 k=c.next()%8; ForceChannel(c,ch, k<3 ? 3 : k<5 ? 1 : 0);
        CStreaming::ms_channelError= c.pct(15) ? -1 : int32(c.next()&1);
        RUN_VOID(M_EXT|B_PROCESSCH, ch); return true; }
    if(nm=="LoadRequestedModels"){
        Prep(c); const uint32 k=c.next()%6; if(k<2)ForceChannel(c,int(c.next()&1),1); else if(k==2)ForceChannel(c,0,2);
        RUN_VOID(M_EXT|B_PROCESSCH|B_REQSTREAM); return true; }
    if(nm=="LoadAllRequestedModels"){
        Prep(c); if(c.pct(30))ForceChannel(c,int(c.next()&1),1); if(c.pct(10))CStreaming::m_bLoadingAllRequestedModels=true;
        RUN_VOID(M_EXT|B_PROCESSCH|B_REQSTREAM|B_FLUSHCH, int32(c.next()&1)); return true; }
    if(nm=="FlushChannels"){
        Prep(c); for(int ch=0;ch<2;++ch){ const uint32 k=c.next()%6; if(k==0)ForceChannel(c,ch,1); else if(k==1)ForceChannel(c,ch,2); }
        RUN_VOID(M_EXT|B_PROCESSCH); return true; }
    if(nm=="Update"){
        Prep(c); RUN_VOID(M_EXT|B_ADDLODS|B_ADDMODELS|B_ALWAYS|B_STREAMVP|B_STREAMZONE|B_LOADREQ|B_PURGE); return true; }
    if(nm=="LoadScene"){
        Prep(c); const CVector v=RandPoint(c);
        RUN_VOID(M_EXT|B_REMOVEMODEL|B_DELETEALL|B_REQBIG|B_LOADALL|B_ADDMODELS|B_STREAMCOP|B_INSTANCE, Pt(v)); return true; }
    if(nm=="LoadSceneCollision"){ Prep(c); const CVector v=RandPoint(c); RUN_VOID(M_EXT|B_LOADALL, Pt(v)); return true; }
    if(nm=="LoadZoneVehicle"){ Prep(c); const CVector v=RandPoint(c); RUN_VOID(M_EXT, Pt(v)); return true; }
    if(nm=="AreAnimsUsedByRequestedModels"||nm=="AreTexturesUsedByRequestedModels"){
        Prep(c); const bool anim=nm=="AreAnimsUsedByRequestedModels";
        const int32 target= anim ? int32(c.next()%NANIM) : int32(c.next()%NTXD);
        // algunos modelos pedidos y en canales usan el objetivo
        for(int32 id:wset)if(id<20000&&c.pct(15)){ auto* mi=miMem[size_t(id)%NMI].data(); if(anim)At<int32>(mi,0x64)=target; else At<int16>(mi,0xA)=int16(target); }
        for(int ch=0;ch<2;++ch)if(c.pct(40)){ const int32 id=anim ? PickDffWset(c) : (c.pct(50) ? TxdId(c.r) : PickDffWset(c)); PutInChannel(ch,int(c.next()%16),id); if(c.pct(50))At<int32>(Chan(ch),0x80)=1; }
        RUN_RET(bool, 0, target); return true; }
    return false;
}
}
