from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/Plane_FlyingHelpersDraft.txt')
p.write_text(r'''
static uint32 PlaneWaveAdvance355(int32 random,float step){
 const float spread=StaticRef<float>(0x8D33E0),one=StaticRef<float>(0x858624),randomScale=StaticRef<float>(0x858C7C),timeScale=StaticRef<float>(0x858B38),countScale=StaticRef<float>(0x858C4C),unsignedAdjustment=StaticRef<float>(0x858C54);const double rangeBase=StaticRef<double>(0x85A310);const uintptr_t truncate=0x821B40;float upper,lower;uint32 result;
 __asm {
  fld spread
  fadd rangeBase
  fstp upper
  fld one
  fsub spread
  fstp lower
  fild random
  fmul randomScale
  fld upper
  fsub lower
  fmulp st(1),st
  fadd lower
  fld step
  fmul timeScale
  fmul countScale
  mov eax,truncate
  call eax
  mov result,eax
  fild result
  cmp result,0
  jge unsigned_wave_count
  fadd unsignedAdjustment
 unsigned_wave_count:
  fmulp st(1),st
  mov eax,truncate
  call eax
  mov result,eax
 }
 return result;
}
static float PlaneDamageSpeed355(const CVector* speed){
 const float scale=StaticRef<float>(0x858B3C),one=1.f;float result;
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
  fmul scale
  fcom one
  fnstsw ax
  test ah,41h
  jne plane_speed_store
  fstp st(0)
  fld one
 plane_speed_store:
  fstp result
 }
 return result;
}
static float PlaneDamageNoise355(float oldValue,float amplitude,int32 random,float squaredDamage,float step,float speed){
 const float randomScale=StaticRef<float>(0x858C7C);float result;
 __asm {
  fild random
  fmul randomScale
  fld amplitude
  fadd amplitude
  fmulp st(1),st
  fsub amplitude
  fmul squaredDamage
  fmul step
  fmul speed
  fadd oldValue
  fstp result
 }
 return result;
}
static float PlanePropellerWave355(float oldValue,float upper,float lower,int32 random,int32 damage,uint32 wave,uint32 period,float step){
 const float randomScale=StaticRef<float>(0x858C7C),circle=StaticRef<float>(0x858CBC),unsignedAdjustment=StaticRef<float>(0x858C54),one=1.f,amount=StaticRef<float>(0x8D3444);float result;
 __asm {
  fild random
  fmul randomScale
  fild damage
  fild wave
  cmp wave,0
  jge unsigned_wave
  fadd unsignedAdjustment
 unsigned_wave:
  fmul circle
  fild period
  cmp period,0
  jge unsigned_period
  fadd unsignedAdjustment
 unsigned_period:
  fdivp st(1),st
  fsin
  fsub one
  fld upper
  fsub lower
  fmul st,st(3)
  fadd lower
  fmulp st(1),st
  fmul st,st(1)
  fmul st,st(1)
  fmul step
  fmul amount
  fadd oldValue
  fstp result
  fstp st(0)
  fstp st(0)
 }
 return result;
}
''')
