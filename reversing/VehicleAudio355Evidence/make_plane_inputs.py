from pathlib import Path
p=Path('reversing/PlaneInputs355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeInputs355/LiveComparison.hpp').read_text().replace('BikeInputs355Live','PlaneInputs355Live').replace('bike-inputs355','plane-inputs355').replace('"Bike.h"','"Plane.h"').replace('CBike','CPlane').replace('0x871360','0x871948').replace('0x6BE310','0x6CADD0')
pos=s.index('inline CPlayerPed* __cdecl Player')
s=s[:pos]+'''inline bool __fastcall LookRight(void*,void*){trace.push_back({"look_right"});return scenario%3==0;}
inline bool __fastcall LookLeft(void*,void*){trace.push_back({"look_left"});return scenario%5==0;}
inline int16 __fastcall Gun(void*,void*){static constexpr int16 values[]{-128,-16,0,16,128};const auto result=values[(scenario/17)%5];trace.push_back({"gun",result});return result;}
inline void __fastcall Gear(CPlane* self,void*){trace.push_back({"gear_up"});self->m_fLandingGearStatus=1.f;self->m_fAirResistance=.125f;}
'''+s[pos:]
s=s.replace(' EntryPatch ex(0x5400D0,Exit),exd(bit_cast<uintptr_t>(&CPad::GetExitVehicle),Exit);',' EntryPatch rr(0x53FE10,LookRight),rrd(bit_cast<uintptr_t>(&CPad::GetLookRight),LookRight),ll(0x53FDD0,LookLeft),lld(bit_cast<uintptr_t>(&CPad::GetLookLeft),LookLeft),gu(0x53FC10,Gun),gud(bit_cast<uintptr_t>(&CPad::GetCarGunUpDown),Gun),ge(0x6CAC20,Gear),ged(bit_cast<uintptr_t>(&CPlane::SetGearUp),Gear);')
s=s.replace('alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};','alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};alignas(16) std::array<uint8,0x80> flying{};')
a=s.index('    static constexpr float steering[]');b=s.index('    memset(reinterpret_cast<void*>(0xC1CC00)',a)
s=s[:a]+'''    memset(flying.data(),0,flying.size());self->m_pFlyingHandlingData=reinterpret_cast<tFlyingHandlingData*>(flying.data());*reinterpret_cast<float*>(flying.data()+0x30)=i%3==0?.5f:2.f;self->m_pHandlingData->m_fDragMult=2.f;
    static constexpr float steering[]{-1.5f,-1.f,-.4f,0.f,.4f,1.f,1.5f,std::bit_cast<float>(0x7FC00000u)};self->m_fSteeringLeftRight=steering[(i/3)%8];self->m_fSteeringUpDown=steering[(i/7)%8];self->m_fLeftRightSkid=steering[(i/11)%8];self->m_vecMoveSpeed={float(i%5)*.125f,float(i%9-4)*.125f,float(i%3)*.125f};self->m_GasPedal=-.25f;self->m_BrakePedal=.5f;self->m_nModelIndex=i%3==0?539:i%3==1?520:464;self->m_fWheelsSuspensionCompression.fill(i%2?1.f:.5f);static constexpr float gear[]{-.01f,-.4f,0.f,.4f,.99f,1.f};self->m_fLandingGearStatus=gear[(i/19)%6];*reinterpret_cast<uint16*>(object.data()+0x86C)=uint16(i%4==0?0:i%4==1?10:i%4==2?4000:5000);object[0x428]=0xFF;
'''+s[b:]
s=s.replace('CVehicle::m_bEnableMouseSteering=i%3!=0;','StaticRef<uint8>(0xC1CC03)=uint8(i%3!=0);')
s=s.replace('pad.NewState.m_bVehicleMouseLook=i%5==0?128:0;','pad.NewState.m_bVehicleMouseLook=i%5==0?128:0;*reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&pad)+0x26)=i%7==0?128:0;*reinterpret_cast<int16*>(reinterpret_cast<uint8*>(&pad)+0x56)=i%11==0?128:0;')
s=s.replace('self->m_pHandlingData=nullptr;','self->m_pHandlingData=nullptr;self->m_pFlyingHandlingData=nullptr;')
(p/'LiveComparison.hpp').write_text(s)
for name in ['build_probe.py','run_local.py']:
 t=Path('reversing/BikeInputs355',name).read_text().replace('bike-inputs355','plane-inputs355').replace('BikeInputs355Live','PlaneInputs355Live').replace('Bike.cpp','Plane.cpp').replace('Bike.h','Plane.h')
 (p/name).write_text(t)
(p/'README.md').write_text('ProcessControlInputs 0x6CADD0 VMT0x871948 slot24: 1024 pairs prepared, pending runtime. Full plane object and mutable pad dependency traces. Mouse/keyboard arbitration, captured timestep before axis query, per-axis blending/decay/clamp, skid controls, steering/braking, landing-gear toggles and completion, Hydra signed low16 nozzle increments with repeated pad reads, disable-controls speed cap. Pad queries mutate timestep/mouse/time in selected fixtures. Contiguous vehicle/mouse/timer/active globals saved/restored; finite and NaN old axes, models539/520/464. Named x87 helpers preserve forward-dot, interpolation, pre-rounding gear completion and nozzle low16 conversion. External CRT pow helper0x822130 remains dependency. Scoped hook false until runtime approval. ASM Plane_InputsFull endsRET6CB7B4.\n')
