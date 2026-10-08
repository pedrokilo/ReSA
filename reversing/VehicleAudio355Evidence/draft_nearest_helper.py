from pathlib import Path
Path('reversing/VehicleAudio355Evidence/Plane_DamageNearestHelper.txt').write_text(r'''static bool PlaneSelectCloser355(const CVector* delta,float* closest){
 uint16 compareStatus;
 __asm {
  mov ecx,delta
  fld dword ptr [ecx+8]
  fld dword ptr [ecx+4]
  fld dword ptr [ecx]
  fld st(0)
  fmul st,st(1)
  fld st(2)
  fmul st,st(3)
  faddp st(1),st
  fld st(3)
  fmul st,st(4)
  faddp st(1),st
  fstp st(3)
  fstp st(0)
  fstp st(0)
  mov edx,closest
  fld dword ptr [edx]
  fmul dword ptr [edx]
  fld st(1)
  fcompp
  fnstsw ax
  mov compareStatus,ax
  test ah,5
  jp plane_not_closer
  fsqrt
  fstp dword ptr [edx]
  jmp plane_closer_done
 plane_not_closer:
  fstp st(0)
 plane_closer_done:
 }
 return (compareStatus&0x500)==0x100;
}
''')
