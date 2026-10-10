#pragma once
// Casos de las 82 funciones de CAudioEngine. Cada rama prepara su fixture (CAudioEngine falso, guiones de los dobles, globales
// reales que lee la funcion), cierra el conjunto de parches justo despues de la llamada y deja el resumen en c.out / c.ret.
#include "Ctx.hpp"

namespace AudioEngineFinal4Live {
// Mascaras derivadas del EXE (se calculan una vez, con los hooks desactivados, en PrepareMasks)
inline std::array<uint8,ENG_SIZE> ctorImage{}, ctorWritten{}, vptrSlot{};
inline std::array<std::array<uint8,ENG_SIZE>,2> tagDump{};   // resumen por byte (vtables reducidas a marca) de cada lado, solo diagnostico
inline uint32 volSaved{}; inline void* fixtureVol{};
inline uint32 Rd32(const uint8* p,size_t o){ uint32 v; std::memcpy(&v,p+o,4); return v; }
inline bool InExe(uint32 v){ return v>=0x400000u&&v<0x1000000u; }

#define WITH(MASK, EXPR) { auto P_=MakePatches(MASK); EXPR; }

// Bytes que escribe el constructor del EXE (rellenos distintos) y ranuras de vptr (valor del EXE escrito en una ranura alineada).
inline void PrepareMasks(){
    alignas(16) static std::array<uint8,ENG_SIZE> b1{}, b2{}, d{};
    b1.fill(0xCD); b2.fill(0x00);
    CallThis<void*>(0x507670,b1.data()); CallThis<void*>(0x507670,b2.data());
    for(size_t i=0;i<ENG_SIZE;++i)ctorWritten[i]=(b1[i]!=0xCD)||(b2[i]!=0x00);
    for(size_t i=0;i+4<=ENG_SIZE;i+=4)if(ctorWritten[i]&&Rd32(b1.data(),i)==Rd32(b2.data(),i)&&InExe(Rd32(b1.data(),i)))vptrSlot[i]=1;
    ctorImage=b2;
    d=b2; CallThis<void*>(0x506CD0,d.data());
    for(size_t i=0;i+4<=ENG_SIZE;i+=4)if(d[i]!=b2[i]||d[i+1]!=b2[i+1]||d[i+2]!=b2[i+2]||d[i+3]!=b2[i+3])if(InExe(Rd32(d.data(),i)))vptrSlot[i]=1;
}
// Resumen de un objeto tras el constructor / destructor: bytes escritos por el EXE (con las ranuras de vptr reducidas a una marca)
// y numero de bytes ajenos modificados (la DLL puede poner a cero lo que el EXE no escribe, nunca otro valor).
inline void ObjectSummary(Ctx& c,const uint8* pre,const uint8* post,bool ctor){
    std::vector<uint8> m; uint32 extra=0; json badAt=json::array();
    auto& tg=tagDump[c.candidate?1:0]; tg.fill(0);
    for(size_t i=0;i<ENG_SIZE;++i){
        const size_t slot=i&~size_t(3);
        if(vptrSlot[slot]){
            if(i==slot){ const uint32 v=Rd32(post,i), p=Rd32(pre,i); m.push_back(uint8(v!=p)); tg[i]=uint8(v!=p); }
            continue; }
        tg[i]= ctor ? (ctorWritten[i] ? post[i] : uint8(post[i]!=pre[i]&&post[i]!=0 ? 0xEE : 0)) : (post[i]!=pre[i] ? post[i] : uint8(0));
        if(ctor){
            if(ctorWritten[i])m.push_back(post[i]);
            else if(post[i]!=pre[i]&&post[i]!=0){ ++extra; if(badAt.size()<16)badAt.push_back(uint32(i)); }
        } else {
            m.push_back(post[i]!=pre[i] ? post[i] : uint8(0)); if(post[i]!=pre[i]&&badAt.size()<64)badAt.push_back(uint32(i));
        }
    }
    c.out["obj"]=Fnv64(m.data(),m.size()); c.out["len"]=m.size(); c.out["extra"]=extra; c.out["at"]=badAt;
}

inline bool RunCase(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; auto& out=c.out; auto& ret=c.ret;

    // -------------------------------------------------------------------------------------------------------------
    // Reenvios a un solo doble
    // -------------------------------------------------------------------------------------------------------------
    if(nm=="AttachMissionAudioToObject"||nm=="AttachMissionAudioToPed"||nm=="AttachMissionAudioToPhysical"){
        c.Begin(); c.SeqConst(T_SC_ATT,int32(c.next()%8)); const uint32 slot=c.u8(); void* p=c.EntN(); void* r{};
        WITH(G_COMMON, r=CallThis<void*>(a,ENGP,slot,p)); ret=Id(r); return true; }
    if(nm=="ClearMissionAudio"){ c.Begin(); const uint32 slot=c.u8(); WITH(G_COMMON, CallThis<void>(a,ENGP,slot)); return true; }
    if(nm=="DisableEffectsLoading"||nm=="EnableEffectsLoading"){ c.Begin(); WITH(G_COMMON, CallCdecl<void>(a)); return true; }
    if(nm=="DisplayRadioStationName"){ c.Begin(); WITH(G_COMMON, CallThis<void>(a,ENGP)); return true; }
    if(nm=="DoesAmbienceTrackOverrideRadio"){
        c.Begin(); *reinterpret_cast<uint8*>(0x8AC15C)=uint8(c.next()&1u); bool r{};
        WITH(G_COMMON, r=CallCdecl<bool>(a)); ret=r; return true; }
    if(nm=="GetBeatInfo"){
        c.Begin(); c.SeqConst(T_HW_BEAT,int32(c.next())); c.SeqConst(T_HW_BEATF,c.coin() ? 0 : int32(c.next())); void* r{};
        WITH(G_COMMON, r=CallThis<void*>(a,ENGP)); ret=Id(r); return true; }
    if(nm=="IsBeatInfoPresent"){
        c.Begin(); c.SeqConst(T_HW_BEAT,int32(c.next())); c.SeqConst(T_HW_BEATF,c.coin() ? 0 : int32(c.next())); bool r{};
        WITH(G_COMMON, r=CallThis<bool>(a,ENGP)); ret=r; return true; }
    if(nm=="GetBeatTrackStatus"||nm=="GetCutsceneTrackStatus"){
        c.Begin(); c.SeqConst(T_CT_STAT,c.coin() ? 0 : int32(c.next()&0xFFu)); int8 r{};
        WITH(G_COMMON, r=CallThis<int8>(a,ENGP)); ret=int(r); return true; }
    if(nm=="GetCurrentRadioStationID"){
        c.Begin(); c.SeqConst(T_RD_CUR,c.radio()); int8 r{}; WITH(G_COMMON, r=CallThis<int8>(a,ENGP)); ret=int(r); return true; }
    if(nm=="GetMissionAudioEvent"){
        c.Begin(); c.SeqConst(T_SC_EVT,int32(c.next())); const uint32 slot=c.u8(); int32 r{};
        WITH(G_COMMON, r=CallThis<int32>(a,ENGP,slot)); ret=r; return true; }
    if(nm=="GetMissionAudioLoadingStatus"){
        c.Begin(); c.SeqConst(T_SC_STAT,int32(int8(c.next()))); const uint32 slot=c.u8(); int8 r{};
        WITH(G_COMMON, r=CallThis<int8>(a,ENGP,slot)); ret=int(r); return true; }
    if(nm=="GetMissionAudioPosition"){
        c.Begin(); c.SeqConst(T_SC_POS,int32(c.next()%8)); const uint32 slot=c.u8(); void* r{};
        WITH(G_COMMON, r=CallThis<void*>(a,ENGP,slot)); ret=Id(r); return true; }
    if(nm=="GetRadioStationListenTimes"){
        c.Begin(); c.SeqConst(T_RD_LIST,int32(c.next()&1u)); void* r{}; WITH(G_COMMON, r=CallThis<void*>(a,ENGP)); ret=Id(r); return true; }
    if(nm=="GetRadioStationName"){
        c.Begin(); c.SeqConst(T_RD_NAME,int32(c.next()%6)); const int32 id=c.radio(); const void* r{};
        WITH(G_COMMON, r=CallThis<const void*>(a,ENGP,id)); ret=Id(r); return true; }
    if(nm=="GetRadioStationNameKey"){
        c.Begin(); c.SeqConst(T_RD_KEY,int32(c.next())); const int32 id=c.radio(); std::memset(w.outbuf,0xAA,sizeof(w.outbuf));
        char* buf= c.chance(8) ? nullptr : reinterpret_cast<char*>(w.outbuf);
        WITH(G_COMMON, CallThis<void>(a,ENGP,id,buf)); out["buf"]=c.Bytes(w.outbuf,16); return true; }
    if(nm=="HasRadioRetuneJustStarted"||nm=="IsRadioRetuneInProgress"||nm=="IsLoadingTuneActive"){
        c.Begin(); const uint32 t= nm=="HasRadioRetuneJustStarted" ? T_RD_JUST : nm=="IsRadioRetuneInProgress" ? T_FE_RETUNE : T_FE_LOAD;
        c.SeqConst(t,int32(c.next()&1u)); bool r{}; WITH(G_COMMON, r=CallThis<bool>(a,ENGP)); ret=r; return true; }
    if(nm=="InitialiseRadioStationID"){ c.Begin(); const int32 id=c.radio(); WITH(G_COMMON, CallThis<void>(a,ENGP,id)); return true; }
    if(nm=="IsAmbienceRadioActive"||nm=="IsAmbienceTrackActive"||nm=="IsCutsceneTrackActive"||nm=="IsRadioOn"||nm=="IsVehicleRadioActive"){
        c.Begin(); const uint32 t= nm=="IsAmbienceRadioActive" ? T_AM_RAD : nm=="IsAmbienceTrackActive" ? T_AM_ACT : nm=="IsCutsceneTrackActive" ? T_CT_ACT : nm=="IsRadioOn" ? T_RD_ON : T_RD_VEH;
        c.SeqConst(t,int32(c.next()&1u)); bool r{};
        if(nm=="IsVehicleRadioActive"){ WITH(G_COMMON, r=CallThis<bool>(a,ENGP)); } else { WITH(G_COMMON, r=CallCdecl<bool>(a)); }
        ret=r; return true; }
    if(nm=="IsMissionAudioSampleFinished"){
        c.Begin(); c.SeqConst(T_SC_FIN,int32(c.next()&1u)); const uint32 slot=c.u8(); bool r{};
        WITH(G_COMMON, r=CallThis<bool>(a,ENGP,slot)); ret=r; return true; }
    if(nm=="PauseAllSounds"||nm=="ResumeAllSounds"){ c.Begin(); WITH(G_COMMON, CallCdecl<void>(a)); return true; }
    if(nm=="PauseBeatTrack"){ c.Begin(); const uint32 b=c.next()&1u; WITH(G_COMMON, CallThis<void>(a,ENGP,b)); return true; }
    if(nm=="PlayLoadedMissionAudio"){ c.Begin(); const uint32 slot=c.u8(); WITH(G_COMMON, CallThis<void>(a,ENGP,slot)); return true; }
    if(nm=="PlayPreloadedBeatTrack"){ c.Begin(); const uint32 b=c.next()&1u; WITH(G_COMMON, CallThis<void>(a,ENGP,b)); return true; }
    if(nm=="PlayPreloadedCutsceneTrack"){ c.Begin(); WITH(G_COMMON, CallCdecl<void>(a)); return true; }
    if(nm=="PreloadMissionAudio"){ c.Begin(); const uint32 slot=c.u8(); const int32 ev=int32(c.next()); WITH(G_COMMON, CallThis<void>(a,ENGP,slot,ev)); return true; }
    if(nm=="ReportBulletHit"){
        c.Begin(); void* e=c.EntN(); const uint32 su=c.next()%180u; void* p=c.Vec(); const float f=c.fl();
        WITH(G_COMMON, CallThis<void>(a,ENGP,e,su,p,f)); return true; }
    if(nm=="ReportCollision"){
        c.Begin(); void* e1=c.EntN(); void* e2=c.EntN(); const uint32 s1=c.next()%180u, s2=c.next()%180u; void* p=c.Vec(); void* n= c.chance(3) ? nullptr : c.Vec();
        const float f1=c.fl(), f2=c.fl(); const uint32 b1=c.next()&1u, b2=c.next()&1u;
        WITH(G_COMMON, CallThis<void>(a,ENGP,e1,e2,s1,s2,p,n,f1,f2,b1,b2)); return true; }
    if(nm=="ReportDoorMovement"){ c.Begin(); void* p=c.EntN(); WITH(G_COMMON, CallThis<void>(a,ENGP,p)); return true; }
    if(nm=="ReportFrontendAudioEvent"){
        c.Begin(); const uint32 ev=c.next()%400u; const float v=c.fl(), s=c.fl(); WITH(G_COMMON, CallThis<void>(a,ENGP,ev,v,s)); return true; }
    if(nm=="ReportGlassCollisionEvent"){ c.Begin(); const uint32 ev=c.next()%400u; void* p=c.Vec(); WITH(G_COMMON, CallThis<void>(a,ENGP,ev,p)); return true; }
    if(nm=="ReportMissionAudioEvent-obj"||nm=="ReportMissionAudioEvent-ped"||nm=="ReportMissionAudioEvent-veh"){
        c.Begin(); const uint32 ev=c.next()&0xFFFFu; void* p=c.EntN(); WITH(G_COMMON, CallThis<void>(a,ENGP,ev,p)); return true; }
    if(nm=="ReportMissionAudioEvent-phs"){
        c.Begin(); const uint32 ev=c.next()&0xFFFFu; void* p=c.EntN(); const float f1=c.fl(), f2=c.fl(); WITH(G_COMMON, CallThis<void>(a,ENGP,ev,p,f1,f2)); return true; }
    if(nm=="ReportMissionAudioEvent-vec"){ c.Begin(); const uint32 ev=c.next()&0xFFFFu; void* p=c.Vec(); WITH(G_COMMON, CallThis<void>(a,ENGP,ev,p)); return true; }
    if(nm=="ReportObjectDestruction"){ c.Begin(); void* e=c.EntN(); WITH(G_COMMON, CallThis<void>(a,ENGP,e)); return true; }
    if(nm=="ReportWaterSplash-phs"){
        c.Begin(); void* p=c.EntN(); const float v=c.fl(); const uint32 b=c.next()&1u; WITH(G_COMMON, CallThis<void>(a,ENGP,p,v,b)); return true; }
    if(nm=="ReportWaterSplash-vec"){
        c.Begin(); const Vec3f p{c.fl(),c.fl(),c.fl()}; const float v=c.fl(); WITH(G_COMMON, CallThis<void>(a,ENGP,p,v)); return true; }
    if(nm=="ReportWeaponEvent"){
        c.Begin(); const int32 ev=int32(c.next()%600u); const uint32 wt=c.next()%60u; void* p=c.EntN(); WITH(G_COMMON, CallThis<void>(a,ENGP,ev,wt,p)); return true; }
    if(nm=="ResetStatistics"||nm=="Restart"||nm=="StartLoadingTune"){ c.Begin(); WITH(G_COMMON, CallThis<void>(a,ENGP)); return true; }
    if(nm=="SayPedless"){
        c.Begin(); c.SeqConst(T_PL_SAY,int32(int16(c.next())));
        const uint32 ev=c.next()%700u, ctx=c.next()%200u; void* at=c.EntN(); const uint32 delay=c.next(); const float prob=c.fl();
        const uint32 o=c.next()&1u, fa=c.next()&1u, fe=c.next()&1u;
        WITH(G_COMMON, CallThis<void>(a,ENGP,ev,ctx,at,delay,prob,o,fa,fe)); return true; }
    if(nm=="SetBassEnhanceOnOff"||nm=="SetRadioAutoRetuneOnOff"){ c.Begin(); const uint32 b=c.next()&1u; WITH(G_COMMON, CallThis<void>(a,ENGP,b)); return true; }
    if(nm=="SetEffectsFaderScalingFactor"||nm=="SetMusicFaderScalingFactor"||nm=="SetNonStreamFaderScalingFactor"||nm=="SetStreamFaderScalingFactor"){
        c.Begin(); const float f=c.fl(); WITH(G_COMMON, CallThis<void>(a,ENGP,f)); return true; }
    if(nm=="SetEffectsMasterVolume"||nm=="SetMusicMasterVolume"){
        c.Begin(); const int32 v= c.i<256 ? int32(int8(c.i)) : int32(int8(c.next())); WITH(G_COMMON, CallThis<void>(a,ENGP,v)); return true; }
    if(nm=="SetMissionAudioPosition"){ c.Begin(); const uint32 slot=c.u8(); void* p=c.Vec(); WITH(G_COMMON, CallThis<void>(a,ENGP,slot,p)); return true; }
    if(nm=="SetRadioBassSetting"){ c.Begin(); const int32 b= c.chance(3) ? int32(int8(c.next())) : int32(c.next()%4u); WITH(G_COMMON, CallThis<void>(a,ENGP,b)); return true; }
    if(nm=="StartRadio-"){ c.Begin(); c.SeqConst(T_CT_STAT,c.coin() ? 0 : int32(1+c.next()%9u)); void* s=w.settRef; WITH(G_COMMON, CallThis<void>(a,ENGP,s)); return true; }
    if(nm=="StartRadio-1"){
        c.Begin(); c.SeqConst(T_CT_STAT,c.coin() ? 0 : int32(1+c.next()%9u)); const int32 id=c.radio(); const int32 bass= c.chance(3) ? int32(int8(c.next())) : int32(c.next()%3u);
        WITH(G_COMMON, CallThis<void>(a,ENGP,id,bass)); return true; }
    if(nm=="RetuneRadio"){
        c.Begin(); c.SeqConst(T_CT_STAT,c.coin() ? 0 : int32(1+c.next()%9u)); const int32 id=c.radio(); WITH(G_COMMON, CallThis<void>(a,ENGP,id)); return true; }
    if(nm=="StopRadio"){ c.Begin(); void* s= c.chance(3) ? nullptr : static_cast<void*>(w.settRef); const uint32 b=c.next()&1u; WITH(G_COMMON, CallThis<void>(a,ENGP,s,b)); return true; }

    // -------------------------------------------------------------------------------------------------------------
    // Funciones con logica propia (bucles de espera, ramas segun el guion)
    // -------------------------------------------------------------------------------------------------------------
    if(nm=="ServiceLoadingTune"){
        c.Begin(); float arg=c.fl(), cur=c.fl();
        switch(c.next()%6){
        case 0: cur=arg-0.005f; break;
        case 1: cur=arg-0.004f; break;
        case 2: cur=arg-0.006f; break;
        case 3: cur=c.uni(0.f,1.f); arg=c.uni(0.f,1.f); break;
        case 4: cur=0.f; arg=1.f; break;
        default: break; }
        c.SeqFloat(T_HW_GEF,cur);
        WITH(G_COMMON, CallThis<void>(a,ENGP,arg)); out["arg"]=F(arg); out["cur"]=F(cur); return true; }
    if(nm=="StopAmbienceTrack"){
        c.Begin(); c.SeqBool(T_AM_ACT,4,0); c.SeqLead(T_HW_TIME,3,0,-8,8); const uint32 b=c.next()&1u;
        WITH(G_COMMON, CallThis<void>(a,ENGP,b)); return true; }
    if(nm=="StopCutsceneTrack"||nm=="StopBeatTrack"){
        c.Begin(); c.SeqBool(T_CT_ACT,4,0); c.SeqLead(T_HW_TIME,3,0,-8,8); c.SeqBool(T_RD_VEH,1,int32(c.next()&1u)); c.SeqConst(T_VE_SET,int32(1+c.next()%2u));
        if(nm=="StopBeatTrack"){ WITH(G_COMMON, CallThis<void>(a,ENGP)); }
        else { const uint32 wait= c.next()%4u ? (c.next()&1u) : 1u; WITH(G_COMMON, CallThis<void>(a,ENGP,wait)); }
        return true; }
    if(nm=="PreloadBeatTrack"||nm=="PreloadCutsceneTrack"){
        c.Begin(); c.SeqBool(T_RD_ON,4,0); c.SeqBool(T_AM_ACT,3,0); c.SeqBool(T_CT_ACT,4,0); c.SeqLead(T_HW_TIME,3,0,-8,8); c.SeqConst(T_RD_CUR,c.radio()); c.SeqConst(T_VE_SET,int32(c.next()%3u));
        const int32 id=int32(int16(c.next()));
        if(nm=="PreloadBeatTrack"){ WITH(G_COMMON, CallThis<void>(a,ENGP,id)); }
        else { const uint32 wait=c.next()&1u; WITH(G_COMMON, CallThis<void>(a,ENGP,id,wait)); }
        return true; }
    if(nm=="Reset"){
        c.Begin(); c.SeqBool(T_RD_ON,4,0); c.SeqBool(T_AM_ACT,4,0); c.SeqBool(T_CT_ACT,4,0); c.SeqLead(T_HW_TIME,3,0,-8,8);
        WITH(G_COMMON, CallThis<void>(a,ENGP)); return true; }
    if(nm=="ResetSoundEffects"){ c.Begin(); WITH(G_COMMON, CallThis<void>(a,ENGP)); return true; }
    if(nm=="Shutdown"){
        c.Begin(); c.SeqBool(T_RD_ON,4,0); c.SeqBool(T_AM_ACT,4,0); c.SeqBool(T_CT_ACT,4,0); c.SeqLead(T_HW_TIME,3,0,-8,8);
        fixtureVol=RealNewArr(45401); *reinterpret_cast<void**>(0xBD00F8)=fixtureVol;
        WITH(G_COMMON|G_DEL, CallThis<void>(a,ENGP));
        out["vol"]=*reinterpret_cast<uint32*>(0xBD00F8)==0 ? 0 : 1; out["gw0"]=Id(At<void*>(eng.data(),0x1E34)); return true; }
    if(nm=="Service"){
        c.Begin(); static const int32 times[]{-4,-4,-6,-6,-5,0,1,7,1234,-1,-3,-7};
        c.SeqConst(T_HW_TIME,times[c.next()%std::size(times)]); c.SeqBool(T_RD_VEH,1,int32(c.next()&1u)); c.SeqConst(T_VE_SET,int32(1+c.next()%2u));
        c.SeqConst(T_FIND,c.coin() ? 0 : int32(1+c.next()%6u));
        *reinterpret_cast<uint8*>(0xB7CB49)=uint8(c.next()%3u==0); *reinterpret_cast<uint8*>(0xB7CB48)=uint8(c.next()%4u==0); *reinterpret_cast<uint8*>(0xB5F851)=uint8(c.next()&1u);
        WITH(G_COMMON|G_FIND, CallThis<void>(a,ENGP)); return true; }

    // -------------------------------------------------------------------------------------------------------------
    // Arranque: Initialise e InitialisePostLoading
    // -------------------------------------------------------------------------------------------------------------
    if(nm=="Initialise"){
        c.Begin();
        // hardware real: canales libres y tabla de ranuras (la inicializacion inlinada del EXE y AllocateChannels leen y escriben aqui)
        *reinterpret_cast<uint16*>(0xB5F944)= c.chance(5) ? uint16(0) : uint16(1+c.next()%3u);
        const uint32 busy=c.next()%6u;
        for(uint32 k=0;k<64;++k)*reinterpret_cast<uint16*>(0xB5F948+2*k)= k<busy ? uint16(1+c.next()%3u) : (c.next()%5u ? uint16(0) : uint16(1+c.next()%2u));
        c.fill(reinterpret_cast<void*>(0x8AE554),0x30); c.fill(reinterpret_cast<void*>(0x8AC15C),0x2C);
        c.SeqConst(T_HW_INIT,c.chance(6) ? 0 : 1); c.SeqConst(T_RD_INIT,c.chance(6) ? 0 : 1); c.SeqConst(T_SM_INIT,c.chance(6) ? 0 : 1);
        c.SeqConst(T_F_OPEN,c.chance(6) ? 0 : 1);
        c.SeqConst(T_F_READ, c.chance(6) ? int32(c.next()%3u ? 45400 : (c.next()%2u ? 0 : 45402)) : 45401);
        bool r{}; WITH(G_COMMON|G_INIT, r=CallThis<bool>(a,ENGP)); ret=r;
        out["vol"]=*reinterpret_cast<uint32*>(0xBD00F8)!=volSaved ? 1 : 0; return true; }
    if(nm=="InitialisePostLoading"){
        c.Begin(); c.SeqBool(T_HW_BANK,8,1);
        At<void*>(eng.data(),0x1E34)=reinterpret_cast<void*>(uintptr_t(0xDEADBEEF));   // lo sobrescribe la funcion
        WITH(G_COMMON|G_CTOR|G_ENG, CallThis<void>(a,ENGP));
        void* g=At<void*>(eng.data(),0x1E34); const bool heap=g&&Id(g)==0xE0000000u;
        if(heap){
            std::vector<uint8> b; for(size_t k=0x10;k<0x14;++k)b.push_back(At<uint8>(g,k)); for(size_t k=0x7C;k<0x7F;++k)b.push_back(At<uint8>(g,k));
            for(size_t k=0x84;k<0xA0;++k)b.push_back(At<uint8>(g,k)); b.push_back(At<uint8>(g,0xA4));
            void* ph=At<void*>(g,0xA0);
            out["gw"]=b; out["vt"]=At<uint32>(g,0)!=0 ? 1 : 0; out["phys"]=ph ? 1 : 0; out["pmat"]=ph ? Id(At<void*>(ph,0x14)) : 0u;
            RealDelete(ph); RealDelete(g);
        } else { out["gw"]=nullptr; out["gwid"]=Id(g); }
        At<void*>(eng.data(),0x1E34)=nullptr; return true; }

    // -------------------------------------------------------------------------------------------------------------
    // Constructor / destructor sobre un CAudioEngine falso
    // -------------------------------------------------------------------------------------------------------------
    if(nm=="Constructor"){
        alignas(16) static std::array<uint8,ENG_SIZE> pre{};
        if(c.coin())c.fill(pre.data(),ENG_SIZE); else pre.fill(0xCD);
        std::memcpy(eng.data(),pre.data(),ENG_SIZE);
        CallThis<void*>(a,ENGP);
        ObjectSummary(c,pre.data(),eng.data(),true); c.engCustom=true; return true; }
    if(nm=="Destructor"){
        alignas(16) static std::array<uint8,ENG_SIZE> pre{};
        std::memcpy(pre.data(),ctorImage.data(),ENG_SIZE);
        c.fill(pre.data(),0xB4); pre[0]&=1; pre[1]&=1;                                      // cabecera del engine y m_BeatInfo: la destruccion no los toca
        At<uint32>(pre.data(),0x1E34)=c.next();                                               // m_GlobalWeaponAE (el destructor no lo libera)
        std::memcpy(eng.data(),pre.data(),ENG_SIZE);
        CallThis<void*>(a,ENGP);
        ObjectSummary(c,pre.data(),eng.data(),false); c.engCustom=true; return true; }
    return false;
}
}
