from pathlib import Path
p=Path('reversing/VehicleAudio355Evidence/Plane_FlyingHelpersDraft.txt');s=p.read_text();s+=r'''
static float PlaneFlightDecay355(float oldValue,int32 damage,bool storeFactor){
 const float scale=StaticRef<float>(0x858CC4),one=1.f;float decay,result;
 __asm {
  fild damage
  fmul scale
  cmp storeFactor,0
  je flight_decay_extended
  fstp decay
  fld decay
 flight_decay_extended:
  fsubr one
  fmul oldValue
  fstp result
 }
 return result;
}
static float PlaneFlightProductAdd355(float value,float a,float b,bool subtract){
 float result;
 __asm {
  fld a
  fmul b
  cmp subtract,0
  je flight_add_product
  fsubr value
  jmp flight_product_store
 flight_add_product:
  fadd value
 flight_product_store:
  fstp result
 }
 return result;
}
static float PlaneFloatFlightBlend355(float value,float target,float step,float factor){
 float result;
 __asm {
  fld target
  fsub value
  fmul step
  fmul factor
  fadd value
  fstp result
 }
 return result;
}
static bool PlaneFlightDistanceBelow355(const CVector* first,const CVector* second,float threshold){
 uint16 comparison;
 __asm {
  mov eax,first
  mov edx,second
  fld dword ptr [eax]
  fsub dword ptr [edx]
  fld dword ptr [eax+4]
  fsub dword ptr [edx+4]
  fld dword ptr [eax+8]
  fsub dword ptr [edx+8]
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
 return (comparison&0x500)==0x100;
}
static bool PlaneFlightRollCompare355(CPlane* plane,float threshold,bool greater){
 const uintptr_t roll=0x420B30;uint16 comparison;
 __asm {
  mov ecx,plane
  mov eax,roll
  call eax
  fabs
  fcomp threshold
  fnstsw ax
  mov comparison,ax
 }
 return greater?!(comparison&0x4100):(comparison&0x500)==0x100;
}
static void PlaneFlightCeiling355(CPlane* plane,int32 model,float* target){
 const uintptr_t ceiling=0x6D2600;const float zero=0.f,lower=-1.f,scale=StaticRef<float>(0x858B38),adjust=StaticRef<float>(0x858CEC);
 __asm {
  mov ecx,plane
  mov eax,dword ptr [ecx+14h]
  test eax,eax
  je ceiling_first_placement
  add eax,30h
  jmp ceiling_first_ready
 ceiling_first_placement:
  lea eax,[ecx+4]
 ceiling_first_ready:
  push model
  push dword ptr [eax+8]
  mov eax,ceiling
  call eax
  fcomp zero
  fnstsw ax
  test ah,41h
  jne ceiling_done
  mov ecx,plane
  mov eax,dword ptr [ecx+14h]
  test eax,eax
  je ceiling_second_placement
  add eax,30h
  jmp ceiling_second_ready
 ceiling_second_placement:
  lea eax,[ecx+4]
 ceiling_second_ready:
  push model
  push dword ptr [eax+8]
  mov eax,ceiling
  call eax
  fmul scale
  fld zero
  fcomp st(1)
  fnstsw ax
  test ah,41h
  jne ceiling_target_multiply
  fstp st(0)
  fld zero
 ceiling_target_multiply:
  mov edx,target
  fmul dword ptr [edx]
  fstp dword ptr [edx]
  mov ecx,plane
  mov eax,dword ptr [ecx+14h]
  test eax,eax
  je ceiling_third_placement
  add eax,30h
  jmp ceiling_third_ready
 ceiling_third_placement:
  lea eax,[ecx+4]
 ceiling_third_ready:
  push model
  push dword ptr [eax+8]
  mov eax,ceiling
  call eax
  fmul adjust
  mov edx,plane
  fsubr dword ptr [edx+994h]
  fld lower
  fcomp st(1)
  fnstsw ax
  test ah,41h
  jne ceiling_acceleration_store
  fstp st(0)
  fld lower
 ceiling_acceleration_store:
  mov edx,plane
  fstp dword ptr [edx+994h]
 ceiling_done:
 }
}
''';p.write_text(s)
