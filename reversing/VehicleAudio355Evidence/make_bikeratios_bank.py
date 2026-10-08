from pathlib import Path
p=Path('reversing/BikeRatios355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeWheel355/LiveComparison.hpp').read_text().replace('BikeWheel355Live','BikeRatios355Live').replace('bike-wheel355','bike-ratios355').replace('#include "Bike.h"','#include "Bike.h"\n#include "SurfaceInfos_c.h"')
a=s.index('alignas(16) inline');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline int scenario{},draw{};
inline int32 __fastcall Adhesion(void*,void*,SurfaceId id){const auto group=int(id)%7==4?4:2;trace.push_back({"adhesion",int(id),group});return group;}
inline int32 __cdecl Random(){const auto r=(scenario*43+draw++*7919)&0x7FFF;trace.push_back({"random",r});return r;}
'''+s[b:]
s=s.replace('{"CBike","SetUpWheelColModel",0,0x871360,43,1024}', '{"CBike","DoBurstAndSoftGroundRatios",0,0x871360,52,1024}')
s=s.replace('const auto oldModel=modelSlot;uint32 pairs=0,totalFailures=0;', 'const auto oldModel=modelSlot;auto& tankSlot=StaticRef<CBaseModelInfo*>(0xA9B0C8+432*4);const auto oldTank=tankSlot;const auto oldStep=CTimer::GetTimeStep();const auto oldWet=StaticRef<float>(0xC81308);uint32 pairs=0,totalFailures=0;')
a=s.index(' EntryPatch c(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch a(0x55E5C0,Adhesion),ad(bit_cast<uintptr_t>(&SurfaceInfos_c::GetAdhesionGroup),Adhesion);
 EntryPatch b(0x821B1E,Random),bd(bit_cast<uintptr_t>(&CGeneral::GetRandomNumber),Random);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CVehicleModelInfo)> mi{};alignas(16) std::array<uint8,0x80> matrix{};
   auto run=[&](bool candidate){scenario=i;draw=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(mi.data(),0,mi.size());memset(matrix.data(),0,matrix.size());
    auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_nModelIndex=i%17==0?432:400;*reinterpret_cast<uintptr_t*>(object.data()+0x14)=uintptr_t(matrix.data());
    auto* model=reinterpret_cast<CVehicleModelInfo*>(mi.data());modelSlot=model;tankSlot=model;model->m_fWheelSizeFront=float(i%15+1)*.125f;model->m_fWheelSizeRear=float(i%11+1)*.125f;
    *reinterpret_cast<CVector*>(matrix.data()+0x10)=CVector{float(i%9-4)*.25f,1.f,float(i%7-3)*.125f};self->m_vecMoveSpeed=CVector{float(i%13-6)*.05f,float(i%11-5)*.05f,float(i%7-3)*.05f};
    self->m_nHandlingFlagsIntValue=static_cast<eVehicleHandlingFlags>(std::array<uint32,4>{0,0x100000,0x200000,0x300000}[(i/27)%4]);
    static constexpr float ratios[]{-.25f,0.f,.2f,.5f,.9f,1.f,1.25f,std::bit_cast<float>(0x7FC00000u)};
    for(size_t w=0;w<4;++w){self->m_aWheelRatios[w]=ratios[(i/3+int(w))%8];self->m_aWheelColPoints[w].m_nSurfaceTypeB=static_cast<eSurfaceType>(i%5==0?178:(i+int(w))%21);self->m_fLineLength[w]=float((i+int(w))%9+1)*.25f;self->m_fSuspensionLength[w]=self->m_fLineLength[w]*float((i/9+int(w))%5)*.25f;}
    for(size_t w=0;w<2;++w){self->m_nWheelStatus[w]=uint8((i/9+int(w))%4);self->m_aWheelPitchAngles[w]=float((i+int(w))%41-20)*.17f;self->m_aWheelAngularVelocity[w]=float((i/7+int(w))%7-3)*.2f;}
    CTimer::UpdateTimeStep(float(i%5)*.5f);StaticRef<float>(0xC81308)=float((i/13)%5)*.25f;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6B6950)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"matrix",Hash(matrix.data(),matrix.size())},{"model",Hash(mi.data(),mi.size())},{"trace",trace}};
'''+s[b:]
s=s.replace(' modelSlot=oldModel;', ' modelSlot=oldModel;tankSlot=oldTank;CTimer::UpdateTimeStep(oldStep);StaticRef<float>(0xC81308)=oldWet;')
s=s.replace('bool restored=modelSlot==oldModel;', 'bool restored=modelSlot==oldModel&&tankSlot==oldTank;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeWheel355',fn).read_text().replace('bike-wheel355','bike-ratios355').replace('BikeWheel355Live','BikeRatios355Live')
 if fn=='build_probe.py':s=s.replace("'RenderWare/rw/rwcore.h'", "'RenderWare/rw/rwcore.h','SurfaceInfos_c.cpp','SurfaceInfos_c.h','General.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('DoBurstAndSoftGroundRatios 0x6B6950 VMT871360slot52:1024 pares pendientes. Wheelstatus0-3, ratios-.25..1.25/NaN, rail178,sand/other Adhesion, front/rear sizes, RNG deterministic traced, wheel phase crossing+/-, gas flags100000/200000 priority, model432 exclusion, timestep0..2/wet0..1. Globals modelslots400/432,step,wet restaurados. Floor puroCRT, fixtures magnitudefinite para conversiones speed. Cuerpo Bike_RatiosNative.txt pendiente aplicación.\n')
print('BikeRatios355 prepared')
