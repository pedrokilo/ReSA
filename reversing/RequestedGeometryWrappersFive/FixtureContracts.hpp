#pragma once
#include <fstream>
#include "PedGeometryAnalyser.h"
#include "Entity/Entity.h"
#include "../RequestedParallelSmall/HookSnapshots.hpp"
namespace RequestedGeometryWrappersFiveLive {
inline int seed{}, function{};
inline CEntity* entity{};
inline CVector* output{};
inline float* dots{};
inline CVector* producedCorners{};
inline CVector* producedCentre{};
inline bool contract{};
inline json trace, rawPointers;
inline uint32 Bits(float x) { return std::bit_cast<uint32>(x); }
inline json VectorBits(const CVector& v) { return {Bits(v.x), Bits(v.y), Bits(v.z)}; }
inline CVector Corner(int i) { return {float(seed%31-15)+float(i)*.25f, float(seed%19-9)-float(i)*.5f, float(seed%13-6)}; }
inline CVector CentreValue() { return {float(seed%17-8),float(seed%23-11),float(seed%11-5)}; }
inline void RecordRaw(const char* role,const void* address) { rawPointers.push_back({{"role",role},{"address",reinterpret_cast<uintptr_t>(address)}}); }
inline void __cdecl Corners(float z,CEntity& e,CVector* out) {
    contract &= &e==entity && (function!=0 || out==output);
    producedCorners=out;RecordRaw("corners_producer",out);
    for(int i=0;i<4;++i)out[i]=Corner(i);
    trace.push_back({{"helper","corners"},{"z_bits",Bits(z)},{"entity_identity",&e==entity},{"external_output",function==0},{"corners_payload",ReversingProbe::Bytes(out,48)}});
}
inline void __cdecl Centre(float z,const CVector* corners,CVector& out) {
    contract &= corners==producedCorners && (function!=1 || &out==output);
    RecordRaw("corners_consumer",corners);producedCentre=&out;RecordRaw("centre_producer",&out);
    for(int i=0;i<4;++i)contract &= VectorBits(corners[i])==VectorBits(Corner(i));
    out=CentreValue();
    trace.push_back({{"helper","centre"},{"z_bits",Bits(z)},{"corners_identity",corners==producedCorners},{"corners_payload",ReversingProbe::Bytes(corners,48)},{"external_output",function==1},{"centre_payload",VectorBits(out)}});
}
inline void WritePlanes(CVector* normals,float* offsets) {
    for(int i=0;i<4;++i){normals[i]={float(seed%29+i),float(seed%7-i),float(i)};offsets[i]=float(seed%41-i)*.125f;}
}
inline void __cdecl Planes(float z,const CVector* corners,CVector(*planes)[4],float* offsets) {
    contract &= corners==producedCorners && &(*planes)[0]==output && offsets==dots;
    for(int i=0;i<4;++i)contract &= VectorBits(corners[i])==VectorBits(Corner(i));
    RecordRaw("corners_consumer",corners);WritePlanes(&(*planes)[0],offsets);
    trace.push_back({{"helper","planes"},{"z_bits",Bits(z)},{"corners_identity",corners==producedCorners},{"corners_payload",ReversingProbe::Bytes(corners,48)},{"external_outputs",&(*planes)[0]==output&&offsets==dots}});
}
inline CVector* __cdecl Segment(const CVector* corners,CVector& centre,CVector* normals,float* offsets) {
    contract &= corners==producedCorners && &centre==producedCentre && normals==output && offsets==dots;
    for(int i=0;i<4;++i)contract &= VectorBits(corners[i])==VectorBits(Corner(i));
    contract &= VectorBits(centre)==VectorBits(CentreValue());
    RecordRaw("corners_consumer",corners);RecordRaw("centre_consumer",&centre);WritePlanes(normals,offsets);
    trace.push_back({{"helper","segment"},{"corners_identity",corners==producedCorners},{"centre_identity",&centre==producedCentre},{"corners_payload",ReversingProbe::Bytes(corners,48)},{"centre_payload",VectorBits(centre)},{"external_outputs",normals==output&&offsets==dots}});
    // Caller compares only identity to the recorded producer; never dereference after return.
    return const_cast<CVector*>(corners);
}
}
