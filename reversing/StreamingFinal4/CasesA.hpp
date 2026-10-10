#pragma once
// Familia A: lectores de estado, peticiones, listas de modelos y acceso a ranuras (sin recorrer el mundo).
#include "Ctx.hpp"

namespace StreamingFinal4Live {
// Llamada a la funcion probada con los parches activos SOLO durante la llamada (el JSON de salida se construye despues, con operator new restaurado)
template<class... A> inline void CallV(Ctx& c,uint64 mask,A... args){ auto P_=PatchSet(mask); CallCdecl<void>(c.a,args...); }
template<class R,class... A> inline R CallR(Ctx& c,uint64 mask,A... args){ auto P_=PatchSet(mask); return CallCdecl<R>(c.a,args...); }
#define RUN_VOID(...) do { CallV(c, __VA_ARGS__); c.ret=0; } while(0)
#define RUN_RET(T, ...) do { c.ret=json(CallR<T>(c, __VA_ARGS__)); } while(0)

inline int32 PickWset(Ctx& c){ return wset[c.next()%unsigned(wset.size())]; }
inline int32 PickDffWset(Ctx& c){ for(int t=0;t<16;++t){ const int32 m=PickWset(c); if(m<20000)return m; } return PedId(c.r); }
inline int32 PickVehWset(Ctx& c){ for(int t=0;t<24;++t){ const int32 m=PickWset(c); if(m>=400&&m<612)return m; } return VehId(c.r); }
inline int32 PickPedWset(Ctx& c){ for(int t=0;t<24;++t){ const int32 m=PickWset(c); if(m<300)return m; } return PedId(c.r); }
inline int32 FlagsArg(Ctx& c){ const uint32 k=c.next()%8; return k==0 ? 0 : k==1 ? 0x10 : k==2 ? 0x18 : k==3 ? 0x0A : int32(c.next()&0xFF); }

inline bool RunA(Ctx& c){
    const std::string& nm=c.nm;

    if(nm=="AddImageToList"){
        Prep(c);
        static const char* const names[]{"MODELS\\GTA3.IMG","MODELS\\GTA_INT.IMG","MODELS\\PLAYER.IMG","X","MODELS\\EXTRA_LONG_NAME_FOR_THE_IMAGE.IMG","",
                                           "a\\b.img","MODELS\\CUTSCENE.IMG"};
        const int used=int(c.next()%10);                          // 0..9 ranuras ocupadas (8 = todas)
        for(int k=0;k<8;++k){ auto* f=reinterpret_cast<uint8*>(0x8E48D8)+0x30*size_t(k); std::memset(f,0xAB,0x30);
            if(k<used){ f[0]='A'+uint8(k); f[1]=0; } else { f[0]=0; }
            f[0x28]=uint8(c.next()); At<uint32>(f,0x2C)=c.next(); }
        const char* name=names[c.next()%std::size(names)]; const int32 flag=int32(c.next()&1);
        RUN_RET(uint32, E_CD, name, flag);
        return true; }
    if(nm=="ClearFlagForAll"){ Prep(c); RUN_VOID(0, int32(c.next()%4==0 ? c.next() : (c.next()&0xFF))); return true; }
    if(nm=="ClearSlots"){ Prep(c); RUN_VOID(M_EXT|E_MEM, int32(c.next()%9)); return true; }   // el EXE (0x40BB9C `cmp edi,eax`) no acota a 8: los llamadores pasan 8 como maximo
    if(nm=="DisableCopBikes"){ Prep(c); RUN_VOID(0, int32(c.next()&1)); return true; }
    if(nm=="FindMIPedSlotForInterior"){ Prep(c); for(int k=0;k<8;++k)if(CStreaming::ms_pedsLoaded[size_t(k)]>=0&&c.pct(60))iState(CStreaming::ms_pedsLoaded[size_t(k)])=1;
        const int32 arg= c.pct(60) ? int32(c.next()%16) : int32(c.next()&0x7FFFFFF); RUN_RET(int32, 0, arg); return true; }
    if(nm=="GetDefaultCopModel"||nm=="GetDefaultCopCarModel"){
        Prep(c); AtA<int32>(CURRLEVEL)=int32(c.next()%5);
        if(nm=="GetDefaultCopModel"){ RUN_RET(int32, 0); }
        else { const int32 ignore=int32(c.next()&1); RUN_RET(int32, 0, ignore); }
        return true; }
    if(nm=="GetDefaultFiremanModel"||nm=="GetDefaultMedicModel"){ Prep(c); AtA<int32>(CURRLEVEL)=int32(c.next()%4); RUN_RET(int32, 0); return true; }
    if(nm=="GetDefaultCabDriverModel"){
        Prep(c); AtA<int32>(CURRLEVEL)=int32(c.next()%4); auto* cab=reinterpret_cast<int32*>(0x8A5AF4);
        if(c.pct(70)){ const int32 idx=AtA<int32>(0x965524); iState(cab[idx])=0; }                    // el modelo previo ya no esta: se elige otro
        RUN_RET(int32, E_RNG);
        return true; }
    if(nm=="HasSpecialCharLoaded"){ Prep(c); RUN_RET(bool, 0, int32(c.next()%10)); return true; }
    if(nm=="HasVehicleUpgradeLoaded"){
        Prep(c); auto* up=reinterpret_cast<int16*>(0xB4E6D8);
        for(int k=0;k<30;++k){ if(up[k]>=0&&up[k]<NTOTAL){ if(c.pct(60))iState(up[k])=uint8(c.pct(50)?1:0); if(up[30+k]>=0&&c.pct(60))iState(up[30+k])=uint8(c.pct(50)?1:0); } }
        const int32 id= c.pct(75) ? 1000+int32(c.next()%40) : PickWset(c); if(c.pct(50))iState(id)=1;
        RUN_RET(bool, 0, id); return true; }
    if(nm=="IsVeryBusy"){ Prep(c); RUN_RET(bool, 0); return true; }
    if(nm=="IsCarModelNeededInCurrentZone"){
        Prep(c); int32 id=VehId(c.r);
        if(c.pct(65)){ const uint32 g=c.next()%34; auto* n=reinterpret_cast<uint16*>(0xC0EC78); auto* cars=reinterpret_cast<uint16*>(0xC0ED38); if(n[g])id=cars[g*23+c.next()%n[g]]; }
        RUN_RET(bool, 0, id); return true; }
    if(nm=="WeAreTryingToPhaseVehicleOut"){
        Prep(c); const int32 id=PickWset(c);
        const uint32 k=c.next()%8; if(k==0){ iNext(id)=-1; iPrev(id)=int16(c.next()%100); } else if(k==1){ iNext(id)=-1; iPrev(id)=-1; } else if(k==2){ iNext(id)=-2; iPrev(id)=int16(c.next()%50); }
        else if(k==3){ iNext(id)=int16(c.next()%100); iPrev(id)=-1; } else if(k==4){ iNext(id)=int16(c.next()%100); iPrev(id)=int16(c.next()%100); }
        if(c.pct(50))iState(id)=1;
        RUN_RET(bool, 0, id); return true; }
    if(nm=="ReclassifyLoadedCars"){ Prep(c); RUN_VOID(M_EXT); return true; }
    if(nm=="AddToLoadedVehiclesList"){ Prep(c); const int32 id=PickVehWset(c); RUN_RET(bool, M_EXT, id); return true; }
    if(nm=="RemoveCarModel"){ Prep(c); const int32 id= c.pct(75) ? PickVehWset(c) : VehId(c.r); RUN_VOID(M_EXT, id); return true; }
    if(nm=="RemoveLoadedVehicle"){ Prep(c); RUN_RET(bool, M_EXT); return true; }
    if(nm=="RemoveLoadedZoneModel"){ Prep(c); RUN_RET(bool, M_EXT); return true; }
    if(nm=="RemoveInappropriatePedModels"){ Prep(c); RUN_VOID(M_EXT); return true; }
    if(nm=="RemoveCurrentZonesModels"){ Prep(c); RUN_VOID(M_EXT); return true; }
    if(nm=="RemoveDodgyPedsFromRandomSlots"){
        Prep(c); static const int32 dodgy[]{MODEL_VWFYST1,MODEL_VBFYST2,MODEL_VHFYST3,MODEL_SBFYSTR,MODEL_SWFYSTR};
        for(int k=0;k<8;++k)if(c.pct(40)){ CStreaming::ms_pedsLoaded[size_t(k)]=static_cast<eModelID>(dodgy[c.next()%5]); }
        CStreaming::ms_numPedsLoaded=0; for(auto p:CStreaming::ms_pedsLoaded)if(p>=0)++CStreaming::ms_numPedsLoaded;
        RUN_VOID(M_EXT); return true; }
    if(nm=="RemoveAllUnusedModels"){ Prep(c); RUN_VOID(M_EXT); return true; }
    if(nm=="PossiblyStreamCarOutAfterCreation"){ Prep(c); const int32 id=PickVehWset(c); RUN_VOID(M_EXT, id); return true; }
    if(nm=="ReInit"){ Prep(c,W_LISTS); RUN_VOID(M_EXT|B_FLUSHREQ); return true; }
    if(nm=="PurgeRequestList"){ Prep(c); RUN_VOID(M_EXT); return true; }
    if(nm=="FlushRequestList"){ Prep(c); RUN_VOID(M_EXT|B_PROCESSCH|B_FLUSHCH); return true; }

    if(nm=="RequestModel"){ Prep(c); const int32 id= c.pct(85) ? PickWset(c) : AnyId(c.r); RUN_VOID(M_EXT, id, FlagsArg(c)); return true; }
    if(nm=="RequestTxdModel"){ Prep(c); const int32 slot=int32(c.next()%NTXD); RUN_VOID(M_EXT, slot, FlagsArg(c)); return true; }
    if(nm=="RequestVehicleUpgrade"){
        Prep(c); const int32 id=1000+int32(c.next()%40); for(int32 m:{id,id+1})if(c.pct(40))iState(m)=uint8(c.next()%5);
        RUN_VOID(M_EXT, id, FlagsArg(c)); return true; }
    if(nm=="RequestFile"){
        Prep(c); const int32 id=PickDffWset(c); auto* f=reinterpret_cast<uint8*>(0x8E48D8);
        uint32 want = At<uint32>(f+0x30*size_t(iImg(id)&7),0x2C)+iOff(id);
        uint32 posRaw=want&0xFFFFFFu; int32 img=int32(want>>24); uint32 size=iSize(id);
        const uint32 k=c.next()%6; if(k==0)posRaw=c.next()&0xFFFFFFu; else if(k==1)img=int32(c.next()%4); else if(k==2)size=1+c.next()%20; else if(k==3)posRaw|=(c.next()&0xFF)<<24; else if(k==4){ iSize(id)=0; }
        RUN_VOID(M_EXT, id, posRaw, size, img, FlagsArg(c)); return true; }
    if(nm=="RequestSpecialChar"||nm=="RequestSpecialModel"){
        static const char* const names[]{"BIGSMOKE","CELESTE","wuzimu","zero","t_bone_mdl","OG_Loc","hern","emmet","","a"};
        Prep(c); const bool isChar=nm=="RequestSpecialChar";
        const int32 slot= isChar ? int32(c.next()%10) : int32(c.next()%3==0 ? 384+c.next()%10 : c.next()%3==1 ? 300+c.next()%20 : 290+c.next()%10);
        const int32 model= isChar ? slot+290 : slot; const char* name=names[c.next()%std::size(names)];
        auto* mi=miMem[size_t(model)%NMI].data();
        if(c.pct(35)){ At<uint32>(mi,4)=CKeyGen::GetUppercaseKey(name); if(c.pct(80))iSize(model)=1+c.next()%9; else iSize(model)=0; }
        if(c.pct(60))At<uint16>(mi,8)=uint16(1+c.next()%3); else At<uint16>(mi,8)=0;
        // peatones / objetos que usan el modelo y otro modelo con la misma clave (el ultimo con esa clave decide el TXD)
        for(int k=0;k<nPed;++k)if(c.pct(50))At<uint16>(Ped(k),0x22)=uint16(model);
        for(int k=0;k<nObj;++k)if(c.pct(50))At<uint16>(Obj(k),0x22)=uint16(model);
        if(c.pct(50)){ const int32 other=int32(c.next()%1000); At<uint32>(miMem[size_t(other)%NMI].data(),4)=At<uint32>(mi,4); }
        if(c.pct(50))iState(model)=uint8(c.next()%4);
        // el nombre debe poder encontrarse en el directorio extra (doble de FindItem siempre acierta)
        if(isChar){ RUN_VOID(M_EXT|E_DIR, slot, name, FlagsArg(c)); }
        else { RUN_VOID(M_EXT|E_DIR, model, name, FlagsArg(c)); }
        return true; }
    if(nm=="SetModelIsDeletable"){ Prep(c); const int32 id=PickWset(c); RUN_VOID(M_EXT, id); return true; }
    if(nm=="SetMissionDoesntRequireModel"){ Prep(c); const int32 id= c.pct(60) ? PickDffWset(c) : PickWset(c); RUN_VOID(M_EXT, id); return true; }
    if(nm=="RemoveModel"){
        Prep(c); const int32 id=PickWset(c);
        if(c.pct(25)){ iState(id)=uint8(3+c.next()%2); }                       // leyendo / terminando
        RUN_VOID(M_EXT, id); return true; }
    if(nm=="RemoveTxdModel"){ Prep(c); RUN_VOID(M_EXT, int32(c.next()%NTXD)); return true; }
    if(nm=="RemoveReferencedTxds"){ Prep(c); RUN_RET(bool, M_EXT, uint32(c.next()%3==0 ? c.next()&0x7FFFFFFF : CStreaming::ms_memoryUsedBytes-c.next()%4000)); return true; }
    if(nm=="RemoveLeastUsedModel"){
        Prep(c); AtA<int32>(CURRLEVEL)=int32(c.next()%4);
        RUN_RET(bool, M_EXT|B_DELETELEAST|B_REMOVELOADEDVEH|B_REMOVELOADEDZONE, int32(c.pct(40) ? 0 : (c.pct(40) ? 0x20 : int32(c.next()&0x3F)))); return true; }
    if(nm=="MakeSpaceFor"){
        Prep(c); uint32 n= c.pct(50) ? c.next()%20000 : c.next()%0x3000000;
        if(c.pct(40))CStreaming::ms_memoryAvailable=CStreaming::ms_memoryUsedBytes+c.next()%100000;
        RUN_VOID(M_EXT|B_REMOVELEAST|B_DELETEBEHIND, n); return true; }
    if(nm=="LoadInitialPeds"){ Prep(c); RUN_VOID(M_EXT); return true; }
    if(nm=="LoadInitialWeapons"){ Prep(c); RUN_VOID(M_EXT); return true; }
    return false;
}
}
