#pragma once
// Familia A: funciones escalares sobre un CVehicleModelInfo falso y globales del modelo (colores, mejoras de ruedas, reglas de extras).
#include "Ctx.hpp"

namespace VehModelInfoFinal4Live {
constexpr uintptr_t HANDLING_BASE=0xC2B9DC, MODELPTRS=0xA9B0C8;

// Pone en el EXE un valor de bandas de modelo para un id de handling (solo se tocan las 24 primeras entradas)
inline void SetHandlingFlags(int idx,uint32 flags){ At<uint32>(reinterpret_cast<void*>(HANDLING_BASE+0xE0u*unsigned(idx)),0xCC)=flags; }
inline void SetWeather(int16 nw,int16 old){ *reinterpret_cast<int16*>(0xC8131C)=nw; *reinterpret_cast<int16*>(0xC81320)=old; }
inline int16 PickWeather(Ctx& c){ static const int16 v[]{-1,0,1,4,5,8,16,10,12,17,7,2}; return v[c.next()%std::size(v)]; }
// comps/regla de extras: nibbles de 0..5 o 0xF (dominio real; valores 6..14 desbordan la lista local del EXE)
inline uint32 PickNibble(Ctx& c){ const uint32 k=c.next()%8; return k<6 ? k : (k==6 ? 0xFu : c.next()%6); }
inline uint32 PickComps(Ctx& c){
    const uint32 ruleA=c.next()%7<5 ? c.next()%5 : c.next()%8, ruleB=c.next()%7<5 ? c.next()%5 : c.next()%8;
    return PickNibble(c)|(PickNibble(c)<<4)|(PickNibble(c)<<8)|(ruleA<<12)|(PickNibble(c)<<16)|(PickNibble(c)<<20)|(PickNibble(c)<<24)|(ruleB<<28);
}

inline bool RunA(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; uint8* const o=w.mi[0];
    auto& out=c.out; auto& ret=c.ret;

    if(nm=="GetModelType"){ c.BuildWorld(); c.FillMi(0); ret=CallThis<uint8>(a,o); out["mi"]=c.Bytes(o+4,0x40); return true; }
    if(nm=="Init"){ auto P=MakePatches(G_CLUMP); c.BuildWorld(); c.FillMi(0); CallThis<void>(a,o); out["mi"]=c.Bytes(o+4,0x80); return true; }
    if(nm=="GetAnimFileIndex"){ c.BuildWorld(); c.FillMi(0); if(c.chance(3))At<int32>(o,0x304)=-1; ret=CallThis<int32>(a,o); return true; }
    if(nm=="GetNumDoors"){ c.BuildWorld(); c.FillMi(0); o[0x4C]= c.i<256 ? uint8(c.i) : uint8(c.next()); ret=CallThis<int32>(a,o); return true; }

    if(nm=="SetAnimFile"){
        static const char* const files[]{"null","NULL","Null","nul","null ","ped","bmx","biked","quad","","van","heli","nullx","nUlL"};
        c.BuildWorld(); c.FillMi(0);
        char* name=w.names[0]; std::memset(name,0,40);
        if(c.chance(8)){ const uint32 len=1+c.next()%38; for(uint32 k=0;k<len;++k)name[k]=char('a'+c.next()%26); } else std::strcpy(name,files[c.next()%std::size(files)]);
        std::memset(arena.data(),0xCD,arena.size()); arenaTop=0;
        { auto P=MakePatches(G_NEW); CallThis<void>(a,o,static_cast<const char*>(name)); }
        out["idx"]=Val(At<uint32>(o,0x304)); out["mi"]=c.Bytes(o+0x2F0,0x20); out["arena"]=c.Bytes(arena.data(),0x40);
        return true; }
    if(nm=="ConvertAnimFileIndex"){
        c.BuildWorld(); c.FillMi(0);
        char* name=w.names[0]; std::memset(name,0,40); std::strcpy(name,kFrameNames[c.next()%NFRAMENAMES]);
        const uint32 mode=c.next()%8;
        At<uint32>(o,0x304)= mode<2 ? 0xFFFFFFFFu : uint32(uintptr_t(name)); // un valor pequeño haría desreferenciar al GetAnimationBlockIndex real
        std::memset(arena.data(),0xCD,arena.size()); arenaTop=0;
        { auto P=MakePatches(G_NEW|G_ANIM); CallThis<void>(a,o); }
        out["idx"]=Val(At<uint32>(o,0x304)); return true; }

    if(nm=="GetNumRemaps"||nm=="AddRemap"){
        c.BuildWorld(); c.FillMi(0);
        const uint32 mode=c.next()%4; const int n=int(c.next()%6);
        for(int k=0;k<4;++k)At<int16>(o,0x2FA+2*k)= mode==3 ? int16(c.next()%3==0 ? -1 : int16(c.next()%500)) : (k<n ? int16(c.next()%500) : int16(-1));
        if(mode==1)At<int16>(o,0x2FA+2*(c.next()%4))=-1;
        if(nm=="GetNumRemaps")ret=CallThis<int32>(a,o);
        else { const int32 txd= c.next()%4==0 ? int32(c.next()) : int32(c.next()%600)-50; CallThis<void>(a,o,txd); }
        out["mi"]=c.Bytes(o+0x2F0,0x20); return true; }

    if(nm=="GetCustomCarPlateText"){
        c.BuildWorld(); c.FillMi(0); if(c.chance(3))o[0x28]=0; ret=Id(CallThis<char*>(a,o)); return true; }
    if(nm=="SetCustomCarPlateText"){
        c.BuildWorld(); c.FillMi(0);
        char* t=w.names[1]; std::memset(t,0,40); const uint32 len=c.next()%15; for(uint32 k=0;k<len;++k)t[k]=char('A'+c.next()%26);
        char* arg= c.chance(5) ? nullptr : t;
        CallThis<void>(a,o,arg); out["mi"]=c.Bytes(o+0x20,0x20); return true; }
    if(nm=="SetVehicleColour"){
        c.BuildWorld(); c.FillMi(0);
        const uint8 p=uint8(c.next()),s=uint8(c.next()),t=uint8(c.next()),q=uint8(c.next());
        CallThis<void>(a,o,p,s,t,q); out["cols"]=c.Bytes(reinterpret_cast<void*>(0xB4E3F0),4); out["mi"]=c.Bytes(o+0x2D0,8); return true; }
    if(nm=="IsUpgradeAvailable"){
        c.BuildWorld(); c.FillMi(0); At<void*>(o,0x5C)=c.FillVs(0);
        const int32 up=int32(c.next()%18); ret=CallThis<bool>(a,o,up); return true; }
    if(nm=="SetupLightFlags"){
        c.BuildWorld(); w.veh[0x584]= c.i<256 ? uint8(c.i) : uint8(c.next());
        CallCdecl<void>(a,static_cast<void*>(w.veh)); out["lights"]=c.Bytes(reinterpret_cast<void*>(0xB4E3E8),4); return true; }

    if(nm=="GetNumWheelUpgrades"||nm=="GetWheelUpgrade"||nm=="AddWheelUpgrade"){
        c.BuildWorld();
        c.fill(reinterpret_cast<void*>(0xB4E3F8),120);
        for(int s=0;s<4;++s)*reinterpret_cast<int16*>(0xB4E470+2*s)= nm=="AddWheelUpgrade" ? int16(c.next()%15) : (c.chance(6) ? int16(int16(c.next())) : int16(c.next()%15));
        const int32 set=int32(c.next()%4);
        if(nm=="GetNumWheelUpgrades")ret=CallCdecl<int32>(a,set);
        else if(nm=="GetWheelUpgrade")ret=CallCdecl<int32>(a,set,int32(c.next()%15));
        else { const int32 model= c.chance(4) ? int32(c.next()) : int32(c.next()%700)-20; CallCdecl<void>(a,set,model); }
        out["counts"]=c.Bytes(reinterpret_cast<void*>(0xB4E470),8); out["table"]=c.Bytes(reinterpret_cast<void*>(0xB4E3F8),120); return true; }
    if(nm=="DeleteVehicleColourTextures"){ c.BuildWorld(); CallCdecl<void>(a); out["cols"]=c.Bytes(reinterpret_cast<void*>(0xB4E3E8),0x20); return true; }

    if(nm=="ChooseVehicleColour"){
        c.BuildWorld(); c.FillMi(0);
        const uint8 nv= c.chance(6) ? 0 : uint8(1+c.next()%8);
        o[0x2D0]=nv; o[0x2D1]= c.next()%5 ? uint8(c.next()%8) : uint8(c.next());
        const bool special=c.chance(6);
        reinterpret_cast<uint8*>(0x969150)[0]= special&&c.chance(2) ? 1 : 0; reinterpret_cast<uint8*>(0x969150)[1]= special&&c.chance(2) ? 1 : 0;
        for(int k=0;k<4;++k)for(int q=0;q<8;++q)o[0x2B0+8*k+q]=uint8(c.next()%130);
        const int32 shift= c.next()%8==0 ? int32(c.next()%40)-20 : int32(c.next()%7);
        // vehiculo del jugador: a veces con el mismo modelo y los colores que elegiria la primera pasada
        tb.playerVehicle= c.chance(4) ? nullptr : static_cast<void*>(w.veh);
        const int id=400+int(c.next()%16); At<int16>(w.veh,0x22)=int16(id);
        *reinterpret_cast<void**>(MODELPTRS+4u*unsigned(id))= c.chance(5) ? static_cast<void*>(w.mi[1]) : static_cast<void*>(o);
        if(nv){ const int firstVar=(shift+o[0x2D1])%int(nv); const uint8 idx=uint8(firstVar);
            if(c.next()%3){ w.veh[0x434]=o[0x2B0+idx]; w.veh[0x435]=o[0x2B8+idx]; w.veh[0x436]=o[0x2C0+idx]; w.veh[0x437]=o[0x2C8+idx]; } }
        uint8 prim=0xAA,sec=0xBB,tert=0xCC,quat=0xDD;
        { auto P=MakePatches(G_PLAYER); CallThis<void>(a,o,&prim,&sec,&tert,&quat,shift); }
        out["cols"]=json::array({prim,sec,tert,quat}); out["last"]=o[0x2D1]; return true; }

    if(nm=="GetMaximumNumberOfPassengersFromNumberOfDoors"){
        c.BuildWorld();
        static const int ids[]{407,508,425,441,564,437,431,400,401,411,450,500,520,550,590,609};
        const int id= c.next()%3==0 ? ids[c.next()%7] : ids[c.next()%std::size(ids)];
        static const int types[]{0,0,0,1,2,3,4,5,6,7,8,9,10,11,-1};
        c.FillMi(0,types[c.next()%std::size(types)]);
        o[0x4C]= c.chance(8) ? uint8(0x80+c.next()%100) : uint8(c.next()%7);
        const uint32 hid=c.next()%24; At<uint16>(o,0x4A)=uint16(hid|(c.next()%3==0 ? (c.next()%255)<<8 : 0u));
        for(int h=0;h<24;++h)SetHandlingFlags(h,c.chance(3) ? (1u<<9) : (c.next()&0x2A0u));
        uint8* vs=c.FillVs(0); At<void*>(o,0x5C)=vs;
        for(int q=0;q<3;++q){ const uint32 k=c.next()%6; At<float>(vs,0x3C+4*q)= k==0 ? 0.f : k==1 ? -0.f : k==2 ? std::bit_cast<float>(0x7FC00000u) : c.uni(-2.f,2.f); }
        if(c.chance(2))for(int q=0;q<3;++q)At<float>(vs,0x3C+4*q)=0.f;
        *reinterpret_cast<void**>(MODELPTRS+4u*unsigned(id))=o;
        ret=CallCdecl<int32>(a,int32(id)); return true; }

    if(nm=="IsValidCompRule"){
        c.BuildWorld(); SetWeather(PickWeather(c),PickWeather(c)); ret=CallCdecl<bool>(a,int32(c.next()%8)); return true; }
    if(nm=="CountCompsInRule"){
        c.BuildWorld(); const uint32 k=c.next()%4;
        const uint32 comps= k==0 ? (c.next()&0x7FFFFFFFu) : k==1 ? (PickNibble(c)|(PickNibble(c)<<4)|(PickNibble(c)<<8)) : k==2 ? 0u : (c.next()&0xFFFu);
        ret=CallCdecl<int32>(a,int32(comps)); return true; }
    if(nm=="GetListOfComponentsNotUsedByRules"){
        c.BuildWorld(); SetWeather(PickWeather(c),PickWeather(c));
        std::memset(w.misc,0xCD,0x40); int32* list=reinterpret_cast<int32*>(w.misc);
        const uint32 rules=PickComps(c); const int32 numExtras=int32(c.next()%7);
        ret=CallCdecl<int32>(a,rules,numExtras,list); out["list"]=c.Bytes(list,24); return true; }
    if(nm=="ChooseComponent-Global"){
        c.BuildWorld(); rngState=c.next()|1u;
        const int32 rule=int32(c.next()%6);
        const uint32 k=c.next()%3; const int32 comps= k==0 ? int32(PickNibble(c)|(PickNibble(c)<<4)|(PickNibble(c)<<8)) : k==1 ? int32(c.next()&0x7FFFFFFFu) : int32(c.next()&0xFFFu);
        { auto P=MakePatches(G_RNG); ret=CallCdecl<int32>(a,rule,comps); }
        return true; }
    if(nm=="ChooseComponent"||nm=="ChooseSecondComponent"){
        c.BuildWorld(); c.FillMi(0); At<void*>(o,0x5C)=c.FillVs(0,int(c.next()%7));
        At<uint32>(o,0x54)=PickComps(c); SetWeather(PickWeather(c),PickWeather(c));
        const int slot= nm=="ChooseComponent" ? 0 : 1;
        reinterpret_cast<int8*>(0x8A6458)[0]=int8(c.next()%2 ? -2 : int(c.next()%8)-1); reinterpret_cast<int8*>(0x8A6458)[1]=int8(c.next()%2 ? -2 : int(c.next()%8)-1);
        (void)slot; rngState=c.next()|1u;
        { auto P=MakePatches(G_RNG); ret=CallThis<int32>(a,o); }
        out["toUse"]=c.Bytes(reinterpret_cast<void*>(0x8A6458),2); return true; }
    return false;
}
}
