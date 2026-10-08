from pathlib import Path
p=Path('reversing/PlaneControl355');p.mkdir(exist_ok=True)
s=Path('reversing/MonsterControl355/LiveComparison.hpp').read_text().replace('MonsterControl355Live','PlaneControl355Live').replace('monster-control355','plane-control355').replace('#include "MonsterTruck.h"','#include "Plane.h"\n#include "Pad.h"\n#include "FxSystem.h"\n#include "FxPrtMult.h"')
a=s.index('inline void __fastcall BaseControl');b=s.index('inline ReversibleHooks',a)
s=s[:a]+'''alignas(16) inline std::array<uint8,sizeof(CPed)> driver{};alignas(16) inline std::array<uint8,sizeof(FxSystem_c)> smoke{},outputSmoke{};
inline RwMatrix particleMatrix{};
inline void __fastcall BaseControl(void*,void*){trace.push_back({"base_control"});}
inline void __fastcall Weapons(void*,void*){trace.push_back({"weapons"});}
inline void __fastcall Wheel(CDamageManager* self,void*,eCarWheel wheel,eCarWheelStatus status){trace.push_back({"wheel",int(wheel),int(status)});self->m_anWheelsStatus[wheel]=status;}
inline void __fastcall Composite(void*,void*,RwMatrix* out){trace.push_back({"composite"});*out=particleMatrix;}
inline void __fastcall Particle(void*,void*,const CVector& pos,const CVector& velocity,float time,const FxPrtMult_c& mult,float rotation,float light,float limit,bool local){trace.push_back({"particle",std::bit_cast<uint32>(pos.x),std::bit_cast<uint32>(pos.y),std::bit_cast<uint32>(pos.z),std::bit_cast<uint32>(velocity.x),std::bit_cast<uint32>(velocity.y),std::bit_cast<uint32>(velocity.z),std::bit_cast<uint32>(time),Hash(&mult,sizeof(mult)),std::bit_cast<uint32>(rotation),std::bit_cast<uint32>(light),std::bit_cast<uint32>(limit),local});}
inline void __fastcall Kill(void*,void*){trace.push_back({"kill"});}
'''+s[b:]
a=s.index('inline const std::array<Fn,4>');b=s.index('inline void Update()',a);s=s[:a]+'inline const std::array<Fn,1> fns{{{"CPlane","ProcessControl",0,0x871948,10,1024}}};\n'+s[b:]
s=s.replace('Base*,4>','Base*,1>').replace('bool,4>','bool,1>').replace('n<4','n<1')
a=s.index(' const auto oldStep=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' const auto oldStep=CTimer::GetTimeStep();const auto oldSmoke=StaticRef<FxSystem_c*>(0xA9AE20);std::array<uint8,2*sizeof(CPad)> oldPads{};memcpy(oldPads.data(),&CPad::Pads[0],oldPads.size());uint32 pairs=0,totalFailures=0;
 {
 using ReversingProbe::EntryPatch;using std::bit_cast;
 const auto controlTarget=reinterpret_cast<uintptr_t>(static_cast<ReversibleHooks::ReversibleHook::Virtual*>(FindHook("CAutomobile","ProcessControl"))->GetHookOurAddress());
 EntryPatch c(0x6B1880,BaseControl),cd(controlTarget,BaseControl);
 EntryPatch w(0x6C21A0,Wheel),wd(bit_cast<uintptr_t>(&CDamageManager::SetWheelStatus),Wheel);
 EntryPatch v(0x6E3950,Weapons),vd(bit_cast<uintptr_t>(&CVehicle::ProcessWeapons),Weapons);
 EntryPatch m(0x4AA8C0,Composite),md(bit_cast<uintptr_t>(&FxSystem_c::GetCompositeMatrix),Composite);
 EntryPatch p(0x4AA440,Particle),pd(bit_cast<uintptr_t>(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle)),Particle);
 EntryPatch k(0x4AA3F0,Kill),kd(bit_cast<uintptr_t>(&FxSystem_c::Kill),Kill);
'''+s[b:]
a=s.index('  for(int i=');b=s.index('   };const auto original',a)
s=s[:a]+'''  for(int i=0;i<fn.cases;++i){alignas(16) std::array<uint8,sizeof(CPlane)> object{};
   auto run=[&](bool candidate){trace=json::array();memset(object.data(),0xCD,object.size());memset(driver.data(),0,sizeof(driver));memset(&CPad::Pads[0],0,sizeof(CPad)*2);auto* self=reinterpret_cast<CPlane*>(object.data());*reinterpret_cast<uintptr_t*>(self)=0x871948;
    self->SetStatus(i%3==0?STATUS_PLAYER:STATUS_PHYSICS);self->m_nModelIndex=std::array<uint16,5>{MODEL_CROPDUST,MODEL_STUNT,MODEL_SKIMMER,MODEL_VORTEX,MODEL_SHAMAL}[(i/3)%5];self->m_pDriver=i%7==0?nullptr:reinterpret_cast<CPed*>(driver.data());reinterpret_cast<CPed*>(driver.data())->m_nPedType=static_cast<ePedType>((i/7)%5);
    self->vehicleFlags.bEngineOn=(i/11)%2;self->vehicleFlags.bIsDrowning=(i/13)%2;self->m_bSmokeEjectorEnabled=(i/17)%2;
    for(int j=0;j<2;++j){CPad::Pads[j].NewState.ShockButtonR=int16((i+j)%2);CPad::Pads[j].OldState.ShockButtonR=int16((i/2+j)%2);}
    self->field_9A0=std::array<uint32,7>{0,1,65535,65536,0x7FC00000,0xFFFFFFFF,0x12345678}[i%7];self->m_pSmokeParticle=i%5==0?nullptr:reinterpret_cast<FxSystem_c*>(smoke.data());self->m_nSmokeTimer=std::array<uint32,6>{0,20,100,1000,0xFFFFFFFF,0x80000000}[(i/5)%6];
    self->m_vecMoveSpeed=CVector{float(i%11-5)*.125f,float(i%7-3)*.25f,float(i%5-2)*.125f};CTimer::ms_fTimeStep=float(i%9)*.25f;StaticRef<FxSystem_c*>(0xA9AE20)=reinterpret_cast<FxSystem_c*>(outputSmoke.data());particleMatrix.pos={float(i%13),-4.f,3.f};
    hooks[n]->State(candidate);const auto address=reinterpret_cast<uintptr_t*>(fn.vt)[fn.slot];if((address!=0x6C9260)!=candidate)return json{{"switch",false}};
    reinterpret_cast<void(__thiscall*)(void*)>(address)(self);
    return json{{"switch",true},{"object",Hash(object.data()+4,object.size()-4)},{"pads",Hash(&CPad::Pads[0],sizeof(CPad)*2)},{"trace",trace}};
'''+s[b:]
a=s.index(' CTimer::ms_fTimeStep=');b=s.index(' for(size_t n=',a)
s=s[:a]+''' CTimer::ms_fTimeStep=oldStep;StaticRef<FxSystem_c*>(0xA9AE20)=oldSmoke;memcpy(&CPad::Pads[0],oldPads.data(),oldPads.size());bool restored=CTimer::GetTimeStep()==oldStep&&StaticRef<FxSystem_c*>(0xA9AE20)==oldSmoke&&memcmp(&CPad::Pads[0],oldPads.data(),oldPads.size())==0;
'''+s[b:]
p.joinpath('LiveComparison.hpp').write_text(s)
for fn in ['build_probe.py','run_local.py']:
 s=Path('reversing/MonsterControl355',fn).read_text().replace('MonsterControl355Live','PlaneControl355Live').replace('monster-control355','plane-control355')
 if fn=='build_probe.py':
  a=s.index('SOURCES=');b=s.index("if __name__",a);s=s[:a]+"SOURCES=[G+p for p in ('Entity/Vehicle/Plane.cpp','Entity/Vehicle/Plane.h','Entity/Vehicle/Automobile.cpp','Entity/Vehicle/Automobile.h','Entity/Vehicle/Vehicle.cpp','Entity/Vehicle/Vehicle.h','DamageManager.cpp','DamageManager.h','Fx/FxSystem.cpp','Fx/FxSystem.h','Fx/FxPrtMult.cpp','Fx/FxPrtMult.h','Pad.h','Timer.h')]\n"+s[b:]
 p.joinpath(fn).write_text(s)
p.joinpath('README.md').write_text('ProcessControl VMT 0x871948 slot10 (0x6C9260): 1.024 pares preparados, pendiente ejecución. Modelos Cropdust/Stunt/Skimmer/Vortex/Shamal; driver nulo/tipos; pads, engine/drowning, raw counter9A0, smoke timer firmado y partículas. Comparación de objeto/pads y trazas de basecontrol, ruedas, weapons y FX simétricas. campo9A0 corregido uint32: EXE prueba/copía bits enteros, no float. TimeStep se limita a valores finitos válidos; conversión MSVC x86 int64 reproduce _ftol2 y lowEAX. Restaura pads, timestep, smoke-global y hooks. Evidencia ASM ../VehicleAudio355Evidence/Plane_6C9260.txt.\n')
