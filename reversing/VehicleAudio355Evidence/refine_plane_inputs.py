from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/Plane_InputsNativeDraft.txt');s=p.read_text()
a=s.index('void CPlane::ProcessControlInputs')
h=r'''
static bool PlaneGearAdvance355(float* gear,float step,float bound){
 const float factor=StaticRef<float>(0x871904);uint16 status;
 __asm {
  mov edx,gear
  fld step
  fmul factor
  fadd dword ptr [edx]
  fst dword ptr [edx]
  fcomp bound
  fnstsw ax
  mov status,ax
 }
 return !(status&0x100);
}
static bool PlaneSpeedBelow355(const CVector* speed,float bound){
 uint16 status;
 __asm {
  mov edx,speed
  fld dword ptr [edx+8]
  fld dword ptr [edx+4]
  fld dword ptr [edx]
  fld st(0)
  fmul st,st(1)
  fld st(2)
  fmul st,st(3)
  faddp st(1),st
  fld st(3)
  fmul st,st(4)
  faddp st(1),st
  fsqrt
  fstp st(3)
  fstp st(0)
  fstp st(0)
  fcomp bound
  fnstsw ax
  mov status,ax
 }
 return (status&0x500)==0x100;
}
static float PlaneGearResistance355(float drag){
 const float divisor=StaticRef<float>(0x871900),factor=StaticRef<float>(0x858B8C);float result;
 __asm {fld drag
 fdiv divisor
 fmul factor
 fstp result}
 return result;
}
'''
s=s[:a]+h+s[a:]
s=s.replace('m_fLandingGearStatus=PlaneControlProductAdd355(m_fLandingGearStatus,CTimer::GetTimeStep(),StaticRef<float>(0x871904));\n  if(m_fLandingGearStatus>=0.f)', 'const bool complete=PlaneGearAdvance355(&m_fLandingGearStatus,CTimer::GetTimeStep(),0.f);\n  if(complete)')
s=s.replace('m_fAirResistance=m_pHandlingData->m_fDragMult/StaticRef<float>(0x871900)*StaticRef<float>(0x858B8C);','m_fAirResistance=PlaneGearResistance355(m_pHandlingData->m_fDragMult);')
s=s.replace('m_fLandingGearStatus=PlaneControlProductAdd355(m_fLandingGearStatus,CTimer::GetTimeStep(),StaticRef<float>(0x871904));if(m_fLandingGearStatus>=1.f)SetGearUp();','if(PlaneGearAdvance355(&m_fLandingGearStatus,CTimer::GetTimeStep(),1.f))SetGearUp();')
s=s.replace('m_vecMoveSpeed.Magnitude()<StaticRef<float>(0x858B1C)','PlaneSpeedBelow355(&m_vecMoveSpeed,StaticRef<float>(0x858B1C))')
p.write_text(s)
