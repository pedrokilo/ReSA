from pathlib import Path
p=Path('reversing/PlaneCreation355');p.mkdir(exist_ok=True)
s=Path('reversing/TrainHit355/LiveComparison.hpp').read_text().replace('TrainHit355Live','PlaneCreation355Live').replace('train-hit355','plane-creation355')
a=s.index('#include "Train.h"');b=s.index('#include "../ProbeRuntime',a);s=s[:a]+'''#include "Plane.h"
#include "Collision.h"
#include "ColSphere.h"
'''+s[b:]
a=s.index('struct Node{');b=s.index('inline void Write',a)
s=s[:a]+'''inline int draw{},playerCalls{},losCalls{};
inline uint32 F(float value){return std::bit_cast<uint32>(value);}
inline int32 __cdecl Random(){const auto r=(scenario*43+draw++*7919)&0x7FFF;trace.push_back({"random",r});return r;}
inline CVector* __cdecl Player(CVector* out,int32 id){*out=CVector{float(scenario%97-48+playerCalls),float(scenario%71-35-playerCalls),float(scenario%31-15)};++playerCalls;trace.push_back({"player",id,F(out->x),F(out->y),F(out->z)});return out;}
inline float __cdecl Heading(float x,float y){const auto result=float(scenario%13-6)*.125f;trace.push_back({"heading",F(x),F(y),F(result)});return result;}
inline void __cdecl Clear(){trace.push_back({"clear",StaticRef<uint16>(0xB7CD78)});StaticRef<CVector>(0xB6F9AC).z+=.25f;}
inline void __fastcall Sphere(CColSphere* sphere,void*,float radius,const CVector& center,eSurfaceType surface,uint8 piece,tColLighting lighting){trace.push_back({"sphere",F(radius),F(center.x),F(center.y),F(center.z),int(surface),piece,lighting.m_nLighting});memset(sphere,0xCD,sizeof(*sphere));sphere->m_fRadius=radius;sphere->m_vecCenter=center;sphere->m_Surface.m_nMaterial=surface;sphere->m_Surface.m_nPiece=piece;sphere->m_Surface.m_nLighting=lighting;StaticRef<CVector>(0xB6F9AC).x+=.125f;}
inline bool __cdecl Sight(const CVector& start,const CVector& end,bool buildings,bool vehicles,bool peds,bool objects,bool dummies,bool seeThrough,bool ignore){const auto r=(scenario+losCalls)%3!=0;++losCalls;trace.push_back({"sight",F(start.x),F(start.y),F(start.z),F(end.x),F(end.y),F(end.z),buildings,vehicles,peds,objects,dummies,seeThrough,ignore,r});const_cast<CVector&>(start).z+=.5f;return r;}
inline bool __cdecl Buildings(int32 x,int32 y,const CColBox&,const CColSphere& sum,const CColSphere& a,const CColSphere& b){const auto r=losCalls<=scenario%51;trace.push_back({"buildings",x,y,&sum==&a&&&a==&b,F(sum.m_fRadius),F(sum.m_vecCenter.x),F(sum.m_vecCenter.y),F(sum.m_vecCenter.z),r});return r;}
'''+s[b:]
s=s.replace('{"CTrain","TrainHitStuff<CPtrListSingleLink<CPhysical*>>",0x6F5CF0,0,-1,1024}', '{"CPlane","FindPlaneCreationCoors",0x6CD090,0,-1,1024}')
s=s.replace('std::array<uint8,12> saved;memcpy(saved.data(),reinterpret_cast<void*>(0x8CD6C8),saved.size());', 'const auto savedForward=StaticRef<CVector>(0xB6F9AC);const auto savedScan=StaticRef<uint16>(0xB7CD78);')
a=s.index(' vtable[4]');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch a(0x821B1E,Random),ad(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
 EntryPatch b(0x56E010,Player),bd(bit_cast<uintptr_t>(&FindPlayerCoors),Player);
 EntryPatch c(0x53CC70,Heading),cd(bit_cast<uintptr_t>(&CGeneral::GetATanOfXY),Heading);
 EntryPatch d(0x563470,Clear),dd(bit_cast<uintptr_t>(&CWorld::ClearScanCodes),Clear);
 EntryPatch e(0x40FD10,Sphere),ed(bit_cast<uintptr_t>(&CColSphere::Set),Sphere);
 EntryPatch f(0x56A490,Sight),fd(bit_cast<uintptr_t>(&CWorld::GetIsLineOfSightClear),Sight);
 EntryPatch g(0x41A820,Buildings),gd(bit_cast<uintptr_t>(&CCollision::CheckCameraCollisionBuildings),Buildings);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){std::array<CVector,2> output;std::array<float,2> numbers;
   auto run=[&](bool candidate){scenario=i;draw=playerCalls=losCalls=0;trace=json::array();output={CVector{-1.f,-2.f,-3.f},CVector{-4.f,-5.f,-6.f}};numbers={-123.f,-124.f};StaticRef<CVector>(0xB6F9AC)=CVector{float(i%7-3)*.25f,float(i%9-4)*.125f,float(i%5-2)*.1f};StaticRef<uint16>(0xB7CD78)=std::array<uint16,4>{0,65534,65535,100}[(i/2)%4];
    hooks[n]->State(candidate);if(ReversingProbe::JumpsToDll(fn.address)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__cdecl*)(CVector*,CVector*,float*,float*,bool)>(fn.address)(&output[0],&output[i%17==0?0:1],&numbers[0],&numbers[i%19==0?0:1],i%2!=0);
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"output",Hash(output.data(),sizeof(output))},{"numbers",Hash(numbers.data(),sizeof(numbers))},{"forward",Hash(&StaticRef<CVector>(0xB6F9AC),sizeof(CVector))},{"scan",StaticRef<uint16>(0xB7CD78)},{"trace",trace}};
'''+s[b:]
s=s.replace(' memcpy(reinterpret_cast<void*>(0x8CD6C8),saved.data(),saved.size());bool restored=true;', ' StaticRef<CVector>(0xB6F9AC)=savedForward;StaticRef<uint16>(0xB7CD78)=savedScan;bool restored=true;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/PlaneControl355',fn).read_text().replace('plane-control355','plane-creation355').replace('PlaneControl355Live','PlaneCreation355Live')
 if fn=='build_probe.py':s=s.replace("SOURCES=[", "SOURCES=[G+p for p in ('Collision/ColSphere.cpp','Collision/ColSphere.h','Collision/Collision.cpp','Collision/Collision.h','World.cpp','World.h','General.cpp','General.h')]+[")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('FindPlaneCreationCoors0x6CD090:1024 pares preparados. Small/big, retries1..50, RNG/player paired con secuencia mutable, direction globals, scan0/65534/65535, sphere/LOS/buildings semantictraces, aliases outputsvector y orientation/height. x87 fsin/fcos namedhelper retiene precision antesfmul/fadd. FakeLOS muta origin despuesendcalculado; sphereSet altera direction para nextiteration; Clear observa scanantesreset. CameraForward12 y scan2 restaurados. Box local EXE sin inicializar no observado porque callee41A820 no lo usa; bankcompara sphereparamsemanticos. Cuerpo draft Plane_CreationNative.txt pendiente aplicacion.\n')
print('PlaneCreation355 prepared')
