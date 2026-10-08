#pragma once
#include <Pad.h>
#include <Camera.h>
#include <MatrixLink.h>
#include <MenuManager.h>
#include <CutsceneMgr.h>
#include <Vehicle.h>
#include <World.h>
#include <cmath>
namespace RequestedPadDistanceTrainNop {
static_assert(sizeof(CPad)==0x134&&sizeof(CVector)==12&&sizeof(CVector2D)==8&&sizeof(CVehicle)==0x5A0&&sizeof(CCamera)==0xD78&&sizeof(CMatrixLink)==0x54);
static_assert(offsetof(CPad,ShakeDur)==0x10C&&offsetof(CPad,ShakeFreq)==0x110);
// Native RET 8 and stack reads prove two floats by value. Blocks the current reference ABI.
static_assert(std::is_same_v<decltype(&CPad::StartShake_Train),void(CPad::*)(CVector2D)>,"Train native ABI is by-value CVector2D; coordinator must fix header/definition before building");
inline constexpr size_t counts[]{1024,10240,2688};
inline constexpr uintptr_t addresses[]{0x53FB40,0x53F9A0,0x53FA70};
inline constexpr const char* keys[]{"CPad::ProcessPCSpecificStuff","CPad::StartShake_Distance","CPad::StartShake_Train"};
inline constexpr std::array<std::pair<int16,int16>,16> durations{{{-32768,-32768},{-32768,-1},{-1,-32768},{-1,0},{0,-1},{0,0},{0,1},{1,0},{1,1},{1,100},{99,100},{100,99},{100,100},{32767,-32768},{-32768,32767},{32767,32767}}};
inline constexpr int16 trainDurations[]{-32768,-1,0,99,100,101,32767};
inline constexpr uint8 frequencies[]{0,1,127,128,255};
inline CVector Delta(size_t n){const float lo=std::nextafter(70.f,0.f),hi=std::nextafter(70.f,100.f);const std::array<CVector,16> values{{{0,0,0},{lo,0,0},{70,0,0},{hi,0,0},{-lo,0,0},{0,lo,0},{0,70,0},{0,hi,0},{0,0,lo},{0,0,70},{0,0,hi},{42,56,0},{42,std::nextafter(56.f,0.f),0},{42,std::nextafter(56.f,100.f),0},{1,2,3},{20,-30,40}}};return values[n];}
inline CVector Origin(size_t n){const std::array<CVector,4> values{{{0,0,0},{13.5f,-7.25f,4.f},{-17.25f,11.5f,-3.f},{.125f,-.25f,103.5f}}};return values[n%4];}
struct Fixture {int bank;size_t seed,geometry{},mode{},gates{},vehicle{},durationRole{};int16 oldDuration{},requestDuration{};uint8 requestFrequency{};CVector camera{},point{};};
inline Fixture Case(int bank,size_t seed){Fixture f{bank,seed};size_t n=seed;if(bank==0){f.mode=(seed/4)%2;f.gates=seed%4;f.durationRole=(seed/8)%4;f.oldDuration=int16(seed*127);f.requestFrequency=uint8(seed);f.camera=Origin(f.durationRole);f.point=Delta(seed%16);return f;}
 f.geometry=n%16;n/=16;f.mode=n%2;n/=2;f.gates=n%4;n/=4;
 if(bank==1){f.requestFrequency=frequencies[n%5];n/=5;f.durationRole=n;f.oldDuration=durations[n].first;f.requestDuration=durations[n].second;}
 else{f.vehicle=n%3;n/=3;f.durationRole=n;f.oldDuration=trainDurations[n];}
 f.camera=Origin(f.durationRole);const auto d=Delta(f.geometry);f.point=CVector{float(f.camera.x+d.x),float(f.camera.y+d.y),float(f.camera.z+d.z)};return f;}
inline CPad PadSeed(int bank,size_t seed,int16 oldDuration){auto pad=CPad::Pads[0];auto* b=reinterpret_cast<uint8*>(&pad);const auto s=bank*50000+seed;for(size_t k=0;k<sizeof(pad);++k)b[k]=uint8(s*73+k*29+(k>>3));for(size_t k=0x118;k<=0x11F;++k)b[k]&=1;pad.ShakeDur=oldDuration;b[0x110]=uint8(seed*17+3);return pad;}
struct GuardedPad {std::array<uint8,16> before;CPad value;std::array<uint8,16> after;explicit GuardedPad(const CPad& p):value(p){before.fill(0xA5);after.fill(0x5A);}bool Guards()const{return std::ranges::all_of(before,[](uint8 b){return b==0xA5;})&&std::ranges::all_of(after,[](uint8 b){return b==0x5A;});}};
struct GuardedMatrix {std::array<uint8,16> before;CMatrixLink value;std::array<uint8,16> after;GuardedMatrix(){before.fill(0xA5);after.fill(0x5A);std::memset(&value,0,sizeof(value));value.GetRight()=CVector{1,0,0};value.GetForward()=CVector{0,1,0};value.GetUp()=CVector{0,0,1};}bool Guards()const{return std::ranges::all_of(before,[](uint8 b){return b==0xA5;})&&std::ranges::all_of(after,[](uint8 b){return b==0x5A;});}};
struct VehicleImage {std::array<uint8,16> before;alignas(CVehicle)std::array<uint8,sizeof(CVehicle)> bytes;std::array<uint8,16> after;void Fill(uint8 value,int32 type){before.fill(0xA5);bytes.fill(value);after.fill(0x5A);std::memcpy(bytes.data()+0x590,&type,4);}CVehicle* Get(){return reinterpret_cast<CVehicle*>(bytes.data());}bool Guards()const{return std::ranges::all_of(before,[](uint8 b){return b==0xA5;})&&std::ranges::all_of(after,[](uint8 b){return b==0x5A;});}};
}
