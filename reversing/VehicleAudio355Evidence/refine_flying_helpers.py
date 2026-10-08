from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/Plane_FlyingHelpersDraft.txt')
s=p.read_text()
s=s.replace('PlaneWaveAdvance355(int32 random,float step)', 'PlaneWaveAdvance355()')
s=s.replace('const uintptr_t truncate=0x821B40;float upper,lower;uint32 result;', 'const uintptr_t truncate=0x821B40,randomFunction=0x821B1E;float upper,lower;int32 random;uint32 result;')
s=s.replace('  fild random\n  fmul randomScale\n  fld upper', '  mov eax,randomFunction\n  call eax\n  mov random,eax\n  fild random\n  fmul randomScale\n  fld upper', 1)
s=s.replace('  fld step\n  fmul timeScale\n  fmul countScale', '  fld dword ptr ds:[0B7CB5Ch]\n  fmul timeScale\n  fmul countScale', 1)
# The CRT conversion leaves the underlying random normalization on the stack.
s=s.replace('  call eax\n  mov result,eax\n }\n return result;', '  call eax\n  fstp st(0)\n  mov result,eax\n }\n return result;', 1)
s += r'''
static float PlaneFlightBound355(float spread,bool subtract){
 const float one=1.f;const double extendedOne=1.;float result;
 __asm {
  cmp subtract,0
  jne flight_bound_subtract
  fld spread
  fadd extendedOne
  jmp flight_bound_store
 flight_bound_subtract:
  fld one
  fsub spread
 flight_bound_store:
  fstp result
 }
 return result;
}
static void PlaneFlightParticleVelocity355(CVector* result,const CVector* speed){
 const float quarter=StaticRef<float>(0x858C84),multiplier=StaticRef<float>(0x858B40);float roundedZ;
 __asm {
  mov edx,speed
  mov eax,result
  fld dword ptr [edx]
  fmul quarter
  fmul multiplier
  fstp dword ptr [eax]
  fld dword ptr [edx+4]
  fmul quarter
  fmul multiplier
  fstp dword ptr [eax+4]
  fld dword ptr [edx+8]
  fmul quarter
  fstp roundedZ
  fld roundedZ
  fmul multiplier
  fstp dword ptr [eax+8]
 }
}
static float PlaneFlightVelocityNoise355(float value,int32 random){
 const float randomScale=StaticRef<float>(0x858C7C),spread=StaticRef<float>(0x871A74),base=StaticRef<float>(0x858C20);float result;
 __asm {
  fild random
  fmul randomScale
  fmul spread
  fadd base
  fmul value
  fstp result
 }
 return result;
}
static float PlaneFlightPositionNoise355(float value,float strength,int32 random){
 const float randomScale=StaticRef<float>(0x858C7C);float negative=-strength,result;
 __asm {
  fild random
  fmul randomScale
  fld strength
  fsub negative
  fmul st,st(1)
  fadd negative
  fadd value
  fstp result
  fstp st(0)
 }
 return result;
}
static float PlaneFlightPropellerTarget355(float acceleration){
 const float standard=StaticRef<float>(0x8D33D0),maximum=StaticRef<float>(0x8D33D4),minimum=StaticRef<float>(0x8D33CC),zero=0.f;float result=standard;
 __asm {
  fld acceleration
  fcomp zero
  fnstsw ax
  test ah,41h
  jne flight_target_negative
  fld maximum
  fsub standard
  jmp flight_target_blend
 flight_target_negative:
  fld acceleration
  fcomp zero
  fnstsw ax
  test ah,5
  jp flight_target_done
  fld standard
  fsub minimum
 flight_target_blend:
  fmul acceleration
  fadd standard
  fstp result
 flight_target_done:
 }
 return result;
}
static bool PlaneFlightSpeedSquaredAbove355(const CVector* speed){
 const double threshold=StaticRef<double>(0x859068);uint16 comparison;
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
  fcomp threshold
  fstp st(0)
  fnstsw ax
  fstp st(0)
  fstp st(0)
  mov comparison,ax
 }
 return !(comparison&0x4100);
}
'''
p.write_text(s)
