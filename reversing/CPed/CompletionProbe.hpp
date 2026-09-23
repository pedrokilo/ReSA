#pragma once
namespace PedCompletionProbe { inline uint32 calls[6]{}; }

namespace PedCompletionProbe {
inline CNodeAddress Closest(CVector origin,ePathType type) {
    CNodeAddress out;
    reinterpret_cast<CNodeAddress*(__thiscall*)(CPathFind*,CNodeAddress*,CVector,int,float,int,int,int,int,int)>(0x44F460)(&ThePaths,&out,origin,int(type),999999.875f,0,0,0,0,0);
    return out;
}
}
