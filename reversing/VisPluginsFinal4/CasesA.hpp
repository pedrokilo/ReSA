#pragma once
// Familia A: accesores del plugin, constructores/destructores de plugin, PluginAttach, distancias y producto escalar.
// Familia B: listas de alfa (Initialise, Shutdown, Insert*, Init*, AddWeaponPedForPC, RenderOrderedList y derivadas).
#include "Ctx.hpp"

namespace VisPluginsFinal4Live {
inline int32 PickIdx(Ctx& c){ switch(c.next()%8){ case 0: return -1; case 1: return 0; case 2: return 449+int32(c.next()%8); case 3: return 32767; case 4: return -32768; case 5: return 65535; case 6: return int32(c.next()); default: return int32(c.next()%2000); } }
inline uint32 PickFlag(Ctx& c){ switch(c.next()%6){ case 0: return 0; case 1: return 0xFFFFu; case 2: return 1u<<(c.next()%16); case 3: return c.next(); case 4: return 0xFFFF0000u|(c.next()&0xFFFFu); default: return c.next()&0xFFFFu; } }
inline float PickDist(Ctx& c,bool nan=true){
    switch(c.next()%12){ case 0: return 0.f; case 1: return 1e8f; case 2: return 2e8f; case 3: return -c.uni(0.f,100.f); case 4: return nan&&c.chance(2) ? Ctx::QNaN() : c.uni(0.f,5000.f);
        default: return c.uni(0.f,5000.f); }
}

inline bool RunA(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; auto& out=c.out; auto& ret=c.ret;
    auto G=[&](bool lists=false){ c.Scrub(lists); };
    (void)G;
    // Funciones de acceso: BuildScene deja el mundo completo; se eligen objetos concretos
    const bool simple = nm=="SetModelInfoIndex"||nm=="GetModelInfoIndex"||nm=="GetModelInfo"||nm=="SetAtomicId"||nm=="GetAtomicId"||nm=="SetUserValue"||nm=="GetUserValue"||nm=="SetAtomicFlag"||nm=="ClearAtomicFlag"
        ||nm=="SetFrameHierarchyId"||nm=="GetFrameHierarchyId"||nm=="SetClumpAlpha"||nm=="GetClumpAlpha"||nm=="GetClumpModelInfo"||nm=="AtomicCopyConstructor"||nm=="AtomicDestructor"||nm=="FrameConstructor"
        ||nm=="FrameCopyConstructor"||nm=="FrameDestructor"||nm=="ClumpConstructor"||nm=="ClumpCopyConstructor"||nm=="ClumpDestructor"||nm=="DefaultVisibilityCB"||nm=="IsClumpVisible"||nm=="SetAlphaCB"
        ||nm=="SetAtomicRenderCallback"||nm=="setClumpForAllAtomicsFlagCB"||nm=="clearClumpForAllAtomicsFlagCB"||nm=="SetClumpForAllAtomicsFlag"||nm=="ClearClumpForAllAtomicsFlag"||nm=="SetTextureCB"
        ||nm=="RenderWheelAtomicCB"||nm=="PluginAttach"||nm=="GetDistanceSquaredFromCamera-Frame"||nm=="GetDistanceSquaredFromCamera-RwV3d"||nm=="GetDotProductWithCameraVector";
    if(!simple)return false;
    c.BuildScene();
    uint8* at=c.RndAt(); uint8* at2=c.RndAt(); uint8* fr=c.RndFr(); uint8* fr2=c.RndFr(); uint8* cl=c.RndCl(); uint8* cl2=c.RndCl();
    const int32 off=int32(c.next()%64), sz=int32(c.next()%64);
    auto atomPlug=[&]{ out["atom"]=c.Bytes(at+atomOff,4); out["atom2"]=c.Bytes(at2+atomOff,4); };

    if(nm=="SetModelInfoIndex"){ CallCdecl<void>(a,static_cast<void*>(at),PickIdx(c)); atomPlug(); return true; }
    if(nm=="GetModelInfoIndex"){ ret=CallCdecl<int32>(a,static_cast<void*>(at)); return true; }
    if(nm=="GetModelInfo"){ if(c.chance(5))At<int16>(at,atomOff)=int16(MI_BASE+10); ret=Id(CallCdecl<void*>(a,static_cast<void*>(at))); out["idx"]=At<int16>(at,atomOff); return true; }
    if(nm=="SetAtomicId"){ CallCdecl<void>(a,static_cast<void*>(at),int32(c.next())); atomPlug(); return true; }
    if(nm=="GetAtomicId"){ ret=CallCdecl<int32>(a,static_cast<void*>(at)); return true; }
    if(nm=="SetUserValue"){ CallCdecl<void>(a,static_cast<void*>(at),c.next()); atomPlug(); return true; }
    if(nm=="GetUserValue"){ ret=CallCdecl<uint32>(a,static_cast<void*>(at))&0xFFFFu; return true; }   // devuelve AX (la parte alta de EAX es basura)
    if(nm=="SetAtomicFlag"||nm=="ClearAtomicFlag"){ CallCdecl<void>(a,static_cast<void*>(at),PickFlag(c)); atomPlug(); return true; }
    if(nm=="SetFrameHierarchyId"){ CallCdecl<void>(a,static_cast<void*>(fr),int32(c.next())); out["fr"]=At<uint32>(fr,frameOff); return true; }
    if(nm=="GetFrameHierarchyId"){ ret=CallCdecl<int32>(a,static_cast<void*>(fr)); return true; }
    if(nm=="SetClumpAlpha"){ CallCdecl<void>(a,static_cast<void*>(cl),int32(c.chance(3) ? c.next() : c.next()%300)); out["cl"]=c.Bytes(cl+clumpOff+4,4); return true; }
    if(nm=="GetClumpAlpha"){ ret=CallCdecl<int32>(a,static_cast<void*>(cl)); return true; }
    if(nm=="GetClumpModelInfo"){ At<uint32>(At<uint8*>(cl,4),frameOff)=c.chance(2) ? uint32(uintptr_t(w.mi[c.next()%NMI])) : c.next(); ret=Val(CallCdecl<uint32>(a,static_cast<void*>(cl))); return true; }

    if(nm=="AtomicCopyConstructor"){ if(c.chance(6))at2=at; ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),static_cast<const void*>(at2),off,sz)); atomPlug(); return true; }
    if(nm=="AtomicDestructor"||nm=="FrameDestructor"||nm=="ClumpDestructor"){ void* o= nm[0]=='A' ? static_cast<void*>(at) : nm[0]=='F' ? static_cast<void*>(fr) : static_cast<void*>(cl); ret=Id(CallCdecl<void*>(a,o,off,sz)); return true; }
    if(nm=="FrameConstructor"){ ret=Id(CallCdecl<void*>(a,static_cast<void*>(fr),off,sz)); out["fr"]=At<uint32>(fr,frameOff); return true; }
    if(nm=="FrameCopyConstructor"){ if(c.chance(6))fr2=fr; ret=Id(CallCdecl<void*>(a,static_cast<void*>(fr),static_cast<const void*>(fr2),off,sz)); out["fr"]=At<uint32>(fr,frameOff); out["fr2"]=At<uint32>(fr2,frameOff); return true; }
    if(nm=="ClumpConstructor"){ ret=Id(CallCdecl<void*>(a,static_cast<void*>(cl),off,sz)); out["cb"]=Canon(At<uint32>(cl,clumpOff)); out["alpha"]=At<int32>(cl,clumpOff+4); return true; }
    if(nm=="ClumpCopyConstructor"){ if(c.chance(6))cl2=cl; ret=Id(CallCdecl<void*>(a,static_cast<void*>(cl),static_cast<const void*>(cl2),off,sz)); out["cb"]=Canon(At<uint32>(cl,clumpOff)); out["alpha"]=At<int32>(cl,clumpOff+4); return true; }
    if(nm=="DefaultVisibilityCB"){ ret=CallCdecl<int32>(a,static_cast<void*>(cl)); return true; }
    if(nm=="IsClumpVisible"){
        At<uint32>(cl,clumpOff)= c.chance(3) ? uint32(EXE_DEFAULTVIS) : uint32(uintptr_t(&VisCb));
        { auto P=MakePatches(G_RW); ret=CallCdecl<int32>(a,static_cast<void*>(cl))&0xFF; }
        return true; }

    if(nm=="SetAlphaCB"){ uint8* m=c.RndMat(); const uint32 d=c.chance(2) ? c.next() : c.next()%256; ret=Id(CallCdecl<void*>(a,static_cast<void*>(m),reinterpret_cast<void*>(d))); out["m"]=c.Bytes(m+4,4); return true; }
    if(nm=="SetTextureCB"){ uint8* m=c.RndMat(); void* t= c.chance(4) ? nullptr : static_cast<void*>(w.tex[c.next()%NTX]);
        { auto P=MakePatches(G_RW); ret=Id(CallCdecl<void*>(a,static_cast<void*>(m),t)); } out["m"]=c.Bytes(m,8); return true; }
    if(nm=="RenderWheelAtomicCB"){ { auto P=MakePatches(G_RW); ret=Id(CallCdecl<void*>(a,static_cast<void*>(at))); } return true; }
    if(nm=="SetAtomicRenderCallback"){
        void* cb= c.pick({0u,1u,2u,3u,4u})==0 ? nullptr : reinterpret_cast<void*>(uintptr_t(c.pick({0x733240u,EXE_ADRC,uint32(uintptr_t(&CbA)),0x7331E0u})));
        CallCdecl<void>(a,static_cast<void*>(at),cb); out["cb"]=Canon(At<uint32>(at,0x48)); out["raw_is_exe_default"]=At<uint32>(at,0x48)==EXE_ADRC; return true; }
    if(nm=="setClumpForAllAtomicsFlagCB"||nm=="clearClumpForAllAtomicsFlagCB"){ ret=Id(CallCdecl<void*>(a,static_cast<void*>(at),reinterpret_cast<void*>(uintptr_t(PickFlag(c))))); atomPlug(); return true; }
    if(nm=="SetClumpForAllAtomicsFlag"||nm=="ClearClumpForAllAtomicsFlag"){
        { auto P=MakePatches(G_RW); CallCdecl<void>(a,static_cast<void*>(cl),int32(PickFlag(c))); }
        json fl=json::array(); for(int k=0;k<NAT;++k)fl.push_back(At<uint16>(w.at[k],atomOff+2)); out["fl"]=fl; return true; }

    if(nm=="PluginAttach"){
        const uint32 s0=Gu(0x8D608C), s1=Gu(0x8D6090), s2=Gu(0x8D6094);
        { auto P=MakePatches(G_RW|G_PLUG); ret=CallCdecl<uint32>(a)&0xFFu; }
        out["off"]={Gu(0x8D608C),Gu(0x8D6090),Gu(0x8D6094)}; out["old"]={s0,s1,s2};
        out["scratch"]=json::array({Fnv32(w.scratch[0],0x200),Fnv32(w.scratch[1],0x200)}); return true; }

    if(nm=="GetDistanceSquaredFromCamera-Frame"){
        { auto P=MakePatches(G_RW); ret=F(CallCdecl<float>(a,static_cast<void*>(fr))); } return true; }
    if(nm=="GetDistanceSquaredFromCamera-RwV3d"){
        auto* v=reinterpret_cast<float*>(w.vec); if(c.chance(5))for(int q=0;q<3;++q)v[q]=0.f;
        ret=F(CallCdecl<float>(a,static_cast<void*>(w.vec))); return true; }
    if(nm=="GetDotProductWithCameraVector"){
        uint32 id= c.chance(3) ? c.next() : PickFlag(c)&0xFFFFu; if(c.chance(4))id=0x8000u|(1u<<(c.next()%9));
        if(c.chance(5))id=0x300u|(c.next()&0xFFu);
        ret=F(CallCdecl<float>(a,static_cast<void*>(w.mx[0]),static_cast<void*>(w.mx[1]),id)); out["id"]=id; return true; }
    return false;
}

inline void RegionRandom(Ctx& c){ c.fill(reinterpret_cast<void*>(GLOB_BASE+0x40),GLOB_SIZE-0x40); }

inline bool RunB(Ctx& c){
    const std::string& nm=c.nm; const uintptr_t a=c.a; auto& out=c.out; auto& ret=c.ret;
    auto dumpAll=[&]{ json l=json::array(); for(int li=0;li<NLISTS;++li)l.push_back(c.DumpList(li)); out["lists"]=l; };

    if(nm=="InitAlphaAtomicList"||nm=="InitAlphaEntityList"){
        c.BuildScene(); c.BuildAllLists(); c.BuildWList(WCAP,int(c.next()%10));
        { auto P=MakePatches(G_RW); CallCdecl<void>(a); }
        dumpAll(); return true; }
    if(nm=="Initialise"){
        c.BuildScene(); RegionRandom(c); std::memset(arena.data(),0xCD,arena.size()); arenaTop=0;
        { auto P=MakePatches(G_NEW); CallCdecl<void>(a); }
        out["region"]=c.Bytes(reinterpret_cast<void*>(GLOB_BASE+0x40),GLOB_SIZE-0x40); out["arena_used"]=Fnv64(arena.data(),arenaTop); return true; }
    if(nm=="Shutdown"){
        c.BuildScene(); RegionRandom(c); std::memset(arena.data(),0xCD,arena.size()); arenaTop=0;
        for(int li=0;li<NLISTS;++li)Gu(LADDR[li]+0x50)= c.chance(4) ? 0u : uint32(uintptr_t(arena.data())+16u*(c.next()%64u));
        Gu(WLADDR+0x30)= c.chance(4) ? 0u : uint32(uintptr_t(arena.data())+16u*(c.next()%64u));
        { auto P=MakePatches(G_NEW); CallCdecl<void>(a); }
        out["region"]=c.Bytes(reinterpret_cast<void*>(GLOB_BASE+0x40),GLOB_SIZE-0x40); return true; }

    if(nm=="InsertObjectIntoSortedList"||nm=="InsertEntityIntoUnderwaterList"||nm=="InsertAtomicIntoReallyDrawLastList"||nm=="InsertEntityIntoReallyDrawLastList"||nm=="InsertEntityIntoSortedList"){
        c.BuildScene(); c.BuildAllLists();
        // Distancias que coinciden con las existentes, extremos y NaN
        float dist=PickDist(c);
        for(int li=0;li<NLISTS&&c.chance(3);++li){ uint8* n=Ctx::LB(li); uint8* f=At<uint8*>(n,0x10); if(f!=n+0x14){ dist=Fl(At<uint32>(f,8)); break; } }
        void* ent=w.ent[c.next()%NENT]; void* atm=w.at[c.next()%NAT];
        if(nm=="InsertEntityIntoSortedList"){
            const uint16 grass=uint16(c.pick({uint32(MI_BASE+1),uint32(MI_BASE+3),0xFFFFu,0x8000u,0u,uint32(MI_BASE+5)})); *reinterpret_cast<uint16*>(0x8CD6F4)=grass;
            At<int16>(ent,0x22)= c.chance(2) ? int16(grass) : int16(c.pick({uint32(MI_BASE),uint32(MI_BASE+1),0xFFFFu,0x8000u}));
            { auto P=MakePatches(G_RW); ret=CallCdecl<uint32>(a,ent,dist)&0xFFu; } }
        else if(nm=="InsertObjectIntoSortedList"){
            void* fnp= reinterpret_cast<void*>(uintptr_t(c.pick({uint32(uintptr_t(&CbA)),uint32(uintptr_t(&CbB)),uint32(EXE_RENDERENTITY),uint32(EXE_RENDERATOMIC)})));
            { auto P=MakePatches(G_RW); ret=CallCdecl<uint32>(a,ent,dist,fnp)&0xFFu; } }
        else if(nm=="InsertAtomicIntoReallyDrawLastList"){ auto P=MakePatches(G_RW); ret=CallCdecl<uint32>(a,atm,dist)&0xFFu; }
        else { auto P=MakePatches(G_RW); ret=CallCdecl<uint32>(a,ent,dist)&0xFFu; }
        out["dist"]=F(dist); dumpAll(); return true; }

    if(nm=="AddWeaponPedForPC"){
        c.BuildScene(); const int cap= c.chance(6) ? 0 : 1+int(c.next()%unsigned(WCAP)); c.BuildWList(cap,int(c.next()%unsigned(cap+1)));
        void* ped= c.chance(8) ? nullptr : static_cast<void*>(w.ped[c.next()%NPED]);
        { auto P=MakePatches(G_RW); CallCdecl<void>(a,ped); }
        out["w"]=c.DumpWList(); return true; }

    if(nm=="RenderOrderedList"||nm=="RenderAlphaAtomics"||nm=="RenderBoatAlphaAtomics"||nm=="RenderFadingEntities"||nm=="RenderFadingUnderwaterEntities"||nm=="RenderReallyDrawLastObjects"){
        c.BuildScene(); c.BuildAllLists();
        { auto P=MakePatches(G_RW); RsGuard rs;
          if(nm=="RenderOrderedList")CallCdecl<void>(a,static_cast<void*>(Ctx::LB(int(c.next()%NLISTS))));
          else CallCdecl<void>(a); }
        dumpAll(); return true; }
    return false;
}
}
