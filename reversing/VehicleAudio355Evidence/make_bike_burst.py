from pathlib import Path
p=Path('reversing/BikeBurst355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeRatios355/LiveComparison.hpp').read_text().replace('BikeRatios355Live','BikeBurst355Live').replace('bike-ratios355','bike-burst355').replace('#include "SurfaceInfos_c.h"','#include "SurfaceInfos_c.h"\n#include "AEVehicleAudioEntity.h"\n#include "CarCtrl.h"')
a=s.index('inline int32 __fastcall Adhesion');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline float __cdecl RandomFloat(float lo,float hi){const auto result=(scenario%5-2)*.125f;trace.push_back({"random",std::bit_cast<uint32>(lo),std::bit_cast<uint32>(hi),std::bit_cast<uint32>(result)});return result;}
inline void __fastcall Audio(void*,void*,eAudioEvents event,float volume){trace.push_back({"audio",int(event),std::bit_cast<uint32>(volume)});}
inline void __cdecl RealPhysics(CVehicle* vehicle){trace.push_back({"real_physics"});vehicle->SetStatus(STATUS_PHYSICS);}
inline void __fastcall Move(void* raw,void*,CVector force){trace.push_back({"move",std::bit_cast<uint32>(force.x),std::bit_cast<uint32>(force.y),std::bit_cast<uint32>(force.z)});static_cast<CBike*>(raw)->m_vecMoveSpeed+=force*.125f;}
inline void __fastcall Turn(void* raw,void*,CVector force,CVector point){trace.push_back({"turn",std::bit_cast<uint32>(force.x),std::bit_cast<uint32>(force.y),std::bit_cast<uint32>(force.z),std::bit_cast<uint32>(point.x),std::bit_cast<uint32>(point.y),std::bit_cast<uint32>(point.z)});static_cast<CBike*>(raw)->m_vecTurnSpeed+=force*.125f;}
'''+s[b:]
s=s.replace('"DoBurstAndSoftGroundRatios",0,0x871360,52','"BurstTyre",0,0x871360,44')
a=s.index(' auto& modelSlot=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 EntryPatch r(0x41BD90,RandomFloat),rd(bit_cast<uintptr_t>(static_cast<float(*)(float,float)>(&CGeneral::GetRandomNumberInRange<float>)),RandomFloat);
 EntryPatch a(0x4F6420,Audio),ad(bit_cast<uintptr_t>(static_cast<void(CAEVehicleAudioEntity::*)(eAudioEvents,float)>(&CAEVehicleAudioEntity::AddAudioEvent)),Audio);
 EntryPatch p(0x423FC0,RealPhysics),pd(bit_cast<uintptr_t>(&CCarCtrl::SwitchVehicleToRealPhysics),RealPhysics);
 EntryPatch m(0x5429F0,Move),md(bit_cast<uintptr_t>(static_cast<void(CPhysical::*)(CVector)>(&CPhysical::ApplyMoveForce)),Move);
 EntryPatch t(0x542A50,Turn),td(bit_cast<uintptr_t>(&CPhysical::ApplyTurnForce),Turn);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('    hooks[n]->State',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(CPed)> driver{};
   auto run=[&](bool candidate){scenario=i;trace=json::array();memset(object.data(),0xCD,object.size());memset(matrix.data(),0,matrix.size());memset(driver.data(),0,driver.size());auto* self=reinterpret_cast<CBike*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data())={1.f,.25f,.125f};*reinterpret_cast<CVector*>(matrix.data()+0x10)={0.f,1.f,.25f};
    self->m_nPhysicalFlags=i%17==0?0x20000000u:0;self->vehicleFlags.bTyresDontBurst=i%19==0;self->SetStatus(static_cast<eEntityStatus>((i/7)%7));self->m_fMass=8.f;self->m_fTurnMass=16.f;self->m_vecMoveSpeed={float(i%9)*.125f,float(i%7)*.125f,i%31==0?std::bit_cast<float>(0x7FC00000u):.25f};self->m_vecTurnSpeed={0.f,0.f,0.f};self->m_pDriver=i%3==0?nullptr:reinterpret_cast<CPed*>(driver.data());
    static constexpr uint8 components[]{0,1,13,15,14,2};const auto component=components[(i/3)%std::size(components)];const auto mapped=component==13?0:component==15?1:component;object[0x65C+mapped]=uint8((i/11)%3);self->m_aRatioHistory[2]=i%5==0?std::bit_cast<float>(0x7FC00000u):float(i%3)*.5f;self->m_aRatioHistory[3]=float((i/13)%3)*.5f;
'''+s[b:]
s=s.replace('0x6B6950','0x6BEB20').replace('reinterpret_cast<void(__thiscall*)(void*)>(address)(self);','const auto ret=reinterpret_cast<bool(__thiscall*)(void*,uint8,bool)>(address)(self,component,i%2!=0);').replace('return json{{"switch",true},{"object"','return json{{"switch",true},{"return",ret},{"object"')
s=s.replace(',{"model",Hash(mi.data(),mi.size())}','')
a=s.index(' modelSlot=oldModel;');b=s.index(' for(size_t n=',a);s=s[:a]+' }\n bool restored=true;\n'+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/BikeRatios355',name).read_text().replace('BikeRatios355Live','BikeBurst355Live').replace('bike-ratios355','bike-burst355')
 if name=='build_probe.py':t=t.replace("'General.h'","'General.cpp','General.h','CarCtrl.cpp','CarCtrl.h','Audio/Entities/AEVehicleAudioEntity.cpp','Audio/Entities/AEVehicleAudioEntity.h','Entity/Physical.cpp','Entity/Physical.h'")
 p.joinpath(name).write_text(t)
p.joinpath('README.md').write_text('BurstTyre VMT 0x871360 slot 44, original 0x6BEB20: 1024 isolated EXE/DLL pairs prepared, runtime pending. Protection flags, raw component mapping/status, repeated bursts, status switch, physical forces, driver/rear impact ratios and speed NaN. Exact return, object/matrix hash and force/audio/RNG trace. Front-event branch after component 13 mapping is unreachable in EXE; component 14 raw status access retained. Evidence ../VehicleAudio355Evidence/Bike_6BEB20.txt (combo reports unavailable REA pseudo; full ASM present). Floating force arithmetic requires runtime comparison before approval.\n')
source=Path('source/game_sa/Entity/Vehicle/Bike.cpp');t=source.read_text();a=t.index('bool CBike::BurstTyre(');b=t.index('\n}',a)+2;native=Path('reversing/VehicleAudio355Evidence/Bike_BurstNative.txt').read_text().replace('GetStatus() == STATUS_PHYSICS','GetStatus() == STATUS_SIMPLE');t=t[:a]+native+t[b:];source.write_text(t)
