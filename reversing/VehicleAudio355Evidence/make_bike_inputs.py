from pathlib import Path
p=Path('reversing/BikeInputs355');p.mkdir(exist_ok=True)
s=Path('reversing/BikeDamage355/LiveComparison.hpp').read_text().replace('BikeDamage355Live','BikeInputs355Live').replace('bike-damage355','bike-inputs355').replace('#include "PlayerPed.h"','#include "PlayerPed.h"\n#include "Pad.h"')
a=s.index('inline CBike* current');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''inline int scenario{},padCalls{};inline CPad pad{};inline CPlayerPed* player{};
inline CPad* __cdecl GetPad(int32 index){trace.push_back({"pad",index,padCalls++});if(scenario%13==0){CTimer::UpdateTimeStep(CTimer::GetTimeStep()+.125f);StaticRef<float>(0xB73424)+=.25f;CTimer::SetTimeInMS(CTimer::GetTimeInMS()+0x100);}return &pad;}
inline bool __fastcall Exit(void*,void*){trace.push_back({"exit"});return scenario%7==0;}
inline int16 __fastcall Hand(void*,void*){trace.push_back({"hand"});return scenario%3==0?128:0;}
inline int16 __fastcall Left(void*,void*){static constexpr int16 values[]{-128,0,64,128,255};const auto result=values[(scenario/3)%5];trace.push_back({"left",result});return result;}
inline int16 __fastcall Up(void*,void*){static constexpr int16 values[]{-128,0,64,128,255};const auto result=values[(scenario/7)%5];trace.push_back({"up",result});return result;}
inline int16 __fastcall Accelerate(void*,void*){static constexpr int16 values[]{0,128,200,255};const auto result=values[(scenario/5)%4];trace.push_back({"accelerate",result});return result;}
inline int16 __fastcall Brake(void*,void*){static constexpr int16 values[]{0,128,200,255};const auto result=values[(scenario/11)%4];trace.push_back({"brake",result});return result;}
inline CPlayerPed* __cdecl Player(int32 id){trace.push_back({"player",id});return player;}
inline void __fastcall Keep(void*,void*){trace.push_back({"keep"});}
'''+s[b:]
s=s.replace('{"CBike","VehicleDamage",0,0x871360,56,1024}','{"CBike","ProcessControlInputs",0,0x871360,18,1024}')
s=s.replace('uint32 pairs=0,totalFailures=0;', '''std::array<uint8,16> savedVehicle;std::array<uint8,8> savedMouse;std::array<uint8,64> savedTimer;std::array<uint8,3> savedActive;const auto savedCamera=StaticRef<uint8>(0xB6EC2E);const auto savedSteer=StaticRef<float>(0xC1C804);const auto savedGame=StaticRef<int32>(0x96A8A0);memcpy(savedVehicle.data(),reinterpret_cast<void*>(0xC1CC00),16);memcpy(savedMouse.data(),reinterpret_cast<void*>(0xB73424),8);memcpy(savedTimer.data(),reinterpret_cast<void*>(0xB7CB48),64);memcpy(savedActive.data(),reinterpret_cast<void*>(0x97D6C0),3);uint32 pairs=0,totalFailures=0;''')
a=s.index(' EntryPatch pr(');b=s.index(' for(size_t n=',a)
s=s[:a]+''' EntryPatch gp(0x53FB70,GetPad),gpd(bit_cast<uintptr_t>(&CPad::GetPad),GetPad);
 EntryPatch ex(0x5400D0,Exit),exd(bit_cast<uintptr_t>(&CPad::GetExitVehicle),Exit);
 EntryPatch hb(0x540040,Hand),hbd(bit_cast<uintptr_t>(&CPad::GetHandBrake),Hand);
 EntryPatch le(0x53FB80,Left),led(bit_cast<uintptr_t>(&CPad::GetSteeringLeftRight),Left);
 EntryPatch up(0x53FBD0,Up),upd(bit_cast<uintptr_t>(&CPad::GetSteeringUpDown),Up);
 EntryPatch ac(0x5403F0,Accelerate),acd(bit_cast<uintptr_t>(&CPad::GetAccelerate),Accelerate);
 EntryPatch br(0x540080,Brake),brd(bit_cast<uintptr_t>(&CPad::GetBrake),Brake);
 EntryPatch pp(0x56E210,Player),ppd(bit_cast<uintptr_t>(&FindPlayerPed),Player);
 EntryPatch ke(0x60C1E0,Keep),ked(bit_cast<uintptr_t>(&CPlayerPed::KeepAreaAroundPlayerClear),Keep);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original=',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CBike)> object{};alignas(16) std::array<uint8,sizeof(CPlayerPed)> playerPed{};alignas(16) std::array<uint8,0x80> matrix{};alignas(16) std::array<uint8,sizeof(tHandlingData)> handling{};
   auto run=[&](bool candidate){scenario=i;padCalls=0;trace=json::array();memset(object.data(),0xCD,object.size());memset(playerPed.data(),0,playerPed.size());memset(matrix.data(),0,matrix.size());memset(handling.data(),0,handling.size());memset(&pad,0,sizeof(pad));auto* self=reinterpret_cast<CBike*>(object.data());player=reinterpret_cast<CPlayerPed*>(playerPed.data());*reinterpret_cast<uintptr_t*>(self)=0x871360;self->m_matrix=reinterpret_cast<CMatrixLink*>(matrix.data());*reinterpret_cast<CVector*>(matrix.data()+0x10)={0.f,1.f,0.f};self->m_pHandlingData=reinterpret_cast<tHandlingData*>(handling.data());self->m_pHandlingData->m_fSteeringLock=float(i%9+1)*5.f;
    static constexpr float steering[]{-1.5f,-1.f,-.4f,0.f,.4f,1.f,1.5f,std::bit_cast<float>(0x7FC00000u)};self->m_fRawSteerAngle=steering[(i/3)%8];self->m_RideAnimData.LeanFwd=steering[(i/7)%8];self->m_vecMoveSpeed={float(i%5)*.125f,float(i%9-4)*.125f,float(i%3)*.125f};self->m_GasPedal=-.25f;self->m_BrakePedal=.5f;self->m_nVehicleSubType=static_cast<eVehicleType>(i%5==0?10:9);object[0x424]=uint8(i%4-1);object[0x428]=uint8(i%2);object[0x429]=i%7==0?0x10:0;
    memset(reinterpret_cast<void*>(0xC1CC00),0,16);CVehicle::m_bEnableMouseSteering=i%3!=0;CVehicle::m_nLastControlInput=static_cast<eControllerType>(i%4);StaticRef<uint8>(0xB6EC2E)=uint8(i%2);StaticRef<float>(0xB73424)=i%5==0?0.f:float(i%11-5);StaticRef<float>(0xB73428)=i%7==0?0.f:float(i%9-4);StaticRef<float>(0xC1C804)=.125f;StaticRef<int32>(0x96A8A0)=i%3;CTimer::UpdateTimeStep(float(i%3));CTimer::SetTimeInMS(uint32(i*1024));for(size_t j=0;j<3;++j)StaticRef<uint8>(0x97D6C0+j)=uint8((i+int(j))%2);pad.NewState.m_bVehicleMouseLook=i%5==0?128:0;*reinterpret_cast<uint16*>(reinterpret_cast<uint8*>(&pad)+0x10E)=i%17==0?1:0;
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6BE310)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(CBike*,uint8)>(address)(self,uint8(i%2));self->m_matrix=nullptr;self->m_pHandlingData=nullptr;
    if(!candidate&&!trace.empty())++positiveCases;
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"mouse",Hash(reinterpret_cast<void*>(0xB73424),8)},{"timer",Hash(reinterpret_cast<void*>(0xB7CB48),64)},{"vehicle",Hash(reinterpret_cast<void*>(0xC1CC00),16)},{"steer",std::bit_cast<uint32>(StaticRef<float>(0xC1C804))},{"trace",trace}};
'''+s[b:]
s=s.replace(' bool restored=true;', ' memcpy(reinterpret_cast<void*>(0xC1CC00),savedVehicle.data(),16);memcpy(reinterpret_cast<void*>(0xB73424),savedMouse.data(),8);memcpy(reinterpret_cast<void*>(0xB7CB48),savedTimer.data(),64);memcpy(reinterpret_cast<void*>(0x97D6C0),savedActive.data(),3);StaticRef<uint8>(0xB6EC2E)=savedCamera;StaticRef<float>(0xC1C804)=savedSteer;StaticRef<int32>(0x96A8A0)=savedGame;\n bool restored=true;')
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/BikeDamage355',fn).read_text().replace('bike-damage355','bike-inputs355').replace('BikeDamage355Live','BikeInputs355Live')
 if fn=='build_probe.py':s=s.replace("'tHandlingData.h'","'tHandlingData.h','Pad.cpp','Pad.h','ControllerState.h','Timer.h','RideAnimData.h'")
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('ProcessControlInputs 0x6BE310 VMT871360 slot18:1024 pares pendientes. ASM integro Bike_InputsFull. Mouse/keyboard arbitration, cached control mode, mouse-look decay, clamp finite/NaN, pad axes/pedals, forward/reverse/stationary, BMX exclusion, broken engine timer pattern, disabled controls speed cap. Paired GetPad and eight methods/player calls traced; GetPad mutates step/mouse/clock in selected cases to check order. Globals saved/restored as contiguous vehicle16,mouse8,timer64,active3 plus camera/steer/game. x87 blending, multiply-add, angle and normalization; decay uses original CRT pow internal 822130 dependency with x87 input ABI (outside scoped function). Fixtures timestep0/1/2 or mutated binary steps; no certification of CRT itself. Hookfalse until EXE validation.\n')
