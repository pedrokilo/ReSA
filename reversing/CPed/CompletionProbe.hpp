#pragma once
namespace PedCompletionProbe { inline uint32 calls[6]{}; }
// Sustitutos de las particulas de pisada de la DLL (EmitFootRainParticles / EmitFootDustParticles, estaticas de Ped.cpp): [0] lluvia, [1] polvo.
// El banco los fija mientras compara DoFootLanded; vacios, DoFootLanded ejecuta las particulas C++ reales.
namespace PedCompletionProbe { inline void (__cdecl* footEmit[2])(void*, void*){}; }

namespace PedCompletionProbe {
inline CNodeAddress Closest(CVector origin,ePathType type) {
    CNodeAddress out;
    reinterpret_cast<CNodeAddress*(__thiscall*)(CPathFind*,CNodeAddress*,CVector,int,float,int,int,int,int,int)>(0x44F460)(&ThePaths,&out,origin,int(type),999999.875f,0,0,0,0,0);
    return out;
}
}
