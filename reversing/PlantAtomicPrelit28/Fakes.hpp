#pragma once
// Memoria de los casos, traza y falsos de RenderWare del banco PlantAtomicPrelit28. Solo C++ estandar (sin tipos del juego): lo incluyen Core.hpp (dentro del juego) y
// host_selftest.cpp (autoprueba en consola x86, sin juego, que ejecuta una transcripcion literal del desensamblado contra estos mismos falsos).
#include <array>
#include <cstddef>
#include <cstring>
#include <string>
#include "Fixtures.hpp"

namespace PlantPrelitFix {
// ------------------------------------------------------------------------------------------------ memoria de los casos
alignas(16) inline std::array<u8,kOldArena> oldArena{}, oldRef{};     // geometria vieja + atomo (+ guardas aleatorias)
alignas(16) inline std::array<u8,kArena> newArena{}, newRef{}, scratch{};   // geometria nueva que entrega el falso de RpGeometryCreate
inline void Put32(u8* base,u32 at,u32 v){std::memcpy(base+at,&v,4);}
inline u32 Get32(const u8* base,u32 at){u32 v{};std::memcpy(&v,base+at,4);return v;}
inline u32 PtrOf(const u8* p){return u32(uintptr_t(p));}
inline std::string Hex64(u64 v){constexpr char d[]="0123456789abcdef";std::string s(16,'0');for(int i=15;i>=0;--i){s[i]=d[v&15];v>>=4;}return s;}
// Relleno de la arena vieja: palabras aleatorias (guardas incluidas); luego geometria, morph target y atomo coherentes; luego los datos del modo del caso.
inline void FillOld(const Fx& f){
    u8* a=oldArena.data();
    FillWords(a,kOldArena,f.oldSeed);
    Put32(a,kGeomAt+8,f.flags);Put32(a,kGeomAt+0x10,u32(f.numT));Put32(a,kGeomAt+0x14,u32(f.numV));Put32(a,kGeomAt+0x18,1);Put32(a,kGeomAt+0x1C,1);
    Put32(a,kGeomAt+kGeomPtrTri,PtrOf(a+kTriAt));Put32(a,kGeomAt+kGeomPtrTex,PtrOf(a+kTexAt));Put32(a,kGeomAt+kGeomPtrMorph,PtrOf(a+kMorphAt));
    Put32(a,kMorphAt+kMorphPtrParent,PtrOf(a+kGeomAt));Put32(a,kMorphAt+kMorphPtrVerts,PtrOf(a+kVertsAt));
    Put32(a,kAtomicAt+0x18,PtrOf(a+kGeomAt));
    FillData(a+kVertsAt,size_t(f.numV)*12,f.dataSeed,f.mode,0);
    FillData(a+kTexAt,size_t(f.numV)*8,f.dataSeed,f.mode,1);
    FillData(a+kTriAt,size_t(f.numT)*8,f.dataSeed,f.mode,2);
}
// Arena nueva: palabras aleatorias (los bytes no escritos por la funcion deben seguir iguales); luego flags y punteros de la geometria/morph target.
inline void FillNew(const Fx& f){
    u8* a=newArena.data();
    FillWords(a,kArena,f.newSeed);
    Put32(a,kGeomAt+8,f.newFlags);
    Put32(a,kGeomAt+kGeomPtrTri,PtrOf(a+kTriAt));Put32(a,kGeomAt+kGeomPtrTex,PtrOf(a+kTexAt));Put32(a,kGeomAt+kGeomPtrMorph,PtrOf(a+kMorphAt));
    Put32(a,kMorphAt+kMorphPtrParent,PtrOf(a+kGeomAt));Put32(a,kMorphAt+kMorphPtrVerts,PtrOf(a+kVertsAt));
}
inline u64 MaskedNewHash(){
    scratch=newArena;
    Put32(scratch.data(),kGeomAt+kGeomPtrTri,0);Put32(scratch.data(),kGeomAt+kGeomPtrTex,0);Put32(scratch.data(),kGeomAt+kGeomPtrMorph,0);
    Put32(scratch.data(),kMorphAt+kMorphPtrParent,0);Put32(scratch.data(),kMorphAt+kMorphPtrVerts,0);
    return Fnv64(scratch.data(),scratch.size());
}
inline bool NewPtrsOk(){
    for(const u32 at:{kGeomAt+kGeomPtrTri,kGeomAt+kGeomPtrTex,kGeomAt+kGeomPtrMorph,kMorphAt+kMorphPtrParent,kMorphAt+kMorphPtrVerts})
        if(Get32(newArena.data(),at)!=Get32(newRef.data(),at))return false;
    return true;
}

// ---- traza sin reservas (los falsos no reservan memoria ni escriben JSON)
struct Ev {u32 kind,a,b,c,d;};
inline std::array<Ev,512> evs{};inline unsigned nev{},evLost{};
inline void Push(u32 k,u32 a=0,u32 b=0,u32 c=0,u32 d=0){if(nev<evs.size()){evs[nev]=Ev{k,a,b,c,d};++nev;}else ++evLost;}
inline u32 GeomLabel(const void* g){return g==oldArena.data()+kGeomAt?1u:g==newArena.data()+kGeomAt?2u:0xFFu;}
// Indice del triangulo dentro del arreglo `triangles` de la propia geometria (0xFFFF si no cae en uno).
inline u32 TriIndex(const void* g,const void* tri){
    if(GeomLabel(g)==0xFF)return 0xFFFF;
    const u8* base=reinterpret_cast<const u8*>(uintptr_t(Get32(static_cast<const u8*>(g),kGeomPtrTri)));
    const u8* t=static_cast<const u8*>(tri);
    if(t<base||size_t(t-base)>=size_t(8)*kMaxT||((t-base)&7))return 0xFFFF;
    return u32((t-base)/8);
}

// Falsos (todos cdecl, como las funciones de RenderWare).
inline void* __cdecl FakeCreate(i32 numV,i32 numT,u32 format){Push(1,u32(numV),u32(numT),format);return newArena.data()+kGeomAt;}
inline void* __cdecl FakeGetMaterial(const void* geometry,const void* triangle){
    const u32 label=GeomLabel(geometry),idx=TriIndex(geometry,triangle);
    u32 matIndex{};
    if(idx!=0xFFFF)matIndex=static_cast<const u8*>(triangle)[6]|(u32(static_cast<const u8*>(triangle)[7])<<8);
    Push(2,label,idx);
    return reinterpret_cast<void*>(uintptr_t(0x4D000000u|((idx*0x101u+matIndex+0x1234u)&0xFFFFu)));
}
inline void* __cdecl FakeSetMaterial(void* geometry,void* triangle,void* material){
    const u32 label=GeomLabel(geometry),idx=TriIndex(geometry,triangle);
    Push(3,label,idx,u32(uintptr_t(material)));
    if(label==2&&idx!=0xFFFF){const u16 m=u16(uintptr_t(material)&0xFFFFu);std::memcpy(static_cast<u8*>(triangle)+6,&m,2);}
    return geometry;
}
inline void* __cdecl FakeUnlock(void* geometry){
    const u64 h=MaskedNewHash();
    Push(4,GeomLabel(geometry),u32(h),u32(h>>32),Get32(newArena.data(),kGeomAt+8));
    return geometry;
}
inline void* __cdecl FakeSetGeometry(void* atomic,void* geometry,u32 flags){
    Push(5,atomic==oldArena.data()+kAtomicAt?1u:0xFFu,GeomLabel(geometry),flags,Get32(newArena.data(),kGeomAt+8));
    return atomic;
}
}
