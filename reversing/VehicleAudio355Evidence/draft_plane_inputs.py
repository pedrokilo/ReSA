from pathlib import Path
b=Path('source/game_sa/Entity/Vehicle/Bike.cpp').read_text();a=b.index('static float BikeControlBlend355');z=b.index('// 0x6BE310',a)
h=b[a:z].replace('Bike','Plane').replace('0x87130C','0x871910')
code=r'''
static float PlaneInputSkid355(float oldValue,float target,float step){
 const float factor=StaticRef<float>(0x858CC4),lower=-1.f,upper=1.f;float result;
 __asm {
  fld target
  fsub oldValue
  fmul step
  fmul factor
  fadd oldValue
  fcom lower
  fnstsw ax
  test ah,41h
  jne skid_lower
  fcom upper
  fnstsw ax
  test ah,5h
  jp skid_upper
  jmp skid_store
 skid_lower:
  fcom lower
  fnstsw ax
  test ah,41h
  je skid_store
  fstp st(0)
  fld lower
  jmp skid_store
 skid_upper:
  fstp st(0)
  fld upper
 skid_store:
  fstp result
 }
 return result;
}
static float PlaneGroundSteer355(float speed,float degrees,float skid){
 const float maximum=StaticRef<float>(0x858B1C),one=1.f,radians=StaticRef<float>(0x8595EC);float result;
 __asm {
  fld one
  fsub speed
  fld maximum
  fcomp st(1)
  fnstsw ax
  test ah,41h
  jne ground_angle
  fstp st(0)
  fld maximum
 ground_angle:
  fld degrees
  fmul radians
  fmul skid
  fmulp st(1),st
  fchs
  fstp result
 }
 return result;
}
static int16 PlaneNozzleChange355(int32 axis,float step){
 const float rate=StaticRef<float>(0x8D33DC),scale=StaticRef<float>(0x858B88);const uintptr_t truncate=0x821B40;int32 result;
 __asm {
  fild axis
  fmul step
  fmul rate
  fmul scale
  mov eax,truncate
  call eax
  mov result,eax
 }
 return int16(result);
}
void CPlane::ProcessControlInputs(uint8 playerNum){
 auto* bytes=reinterpret_cast<uint8*>(this);
 const float forwardSpeed=PlaneForwardSpeed355(&m_vecMoveSpeed,&m_matrix->GetForward());
 auto& mode=StaticRef<int32>(0xC1CC04);auto& mouseX=StaticRef<float>(0xB73424);auto& mouseY=StaticRef<float>(0xB73428);
 bool mouse=false,keyboard=true;
 if(StaticRef<bool>(0xB6EC2E)&&StaticRef<bool>(0xC1CC03)){
  CPad::GetPad(playerNum);
  if(mouseX!=0.f)mouse=true;
  else{
   CPad::GetPad(playerNum);
   if(mouseY!=0.f)mouse=true;
   else if((std::abs(m_fSteeringLeftRight)>0.f||std::abs(m_fSteeringUpDown)>0.f)&&mode==2){
    if(CPad::GetPad(playerNum)->GetSteeringLeftRight()==0&&CPad::GetPad(playerNum)->GetSteeringUpDown()==0)mouse=true;
   }
  }
  if(!mouse)keyboard=CPad::GetPad(playerNum)->GetSteeringLeftRight()!=0||CPad::GetPad(playerNum)->GetSteeringUpDown()!=0||mode!=2;
 }
 if(mouse){
  mode=2;
  if(!CPad::GetPad(playerNum)->NewState.m_bVehicleMouseLook){
   CPad::GetPad(playerNum);m_fSteeringLeftRight=PlaneControlProductAdd355(m_fSteeringLeftRight,mouseX,StaticRef<float>(0x871908));
   CPad::GetPad(playerNum);m_fSteeringUpDown=PlaneControlProductAdd355(m_fSteeringUpDown,mouseY,StaticRef<float>(0x871908));
  }
  if(std::abs(m_fSteeringLeftRight)<StaticRef<float>(0x87190C))PlaneMouseDecay355(&m_fSteeringLeftRight);
  if(std::abs(m_fSteeringUpDown)<StaticRef<float>(0x87190C))PlaneMouseDecay355(&m_fSteeringUpDown);
 }else if(keyboard){
  mode=0;float step=CTimer::GetTimeStep();const int32 lr=CPad::GetPad(playerNum)->GetSteeringLeftRight();
  m_fSteeringLeftRight=PlaneControlBlend355(m_fSteeringLeftRight,lr,step,StaticRef<float>(0x858CC4));
  step=CTimer::GetTimeStep();const int32 ud=CPad::GetPad(playerNum)->GetSteeringUpDown();
  m_fSteeringUpDown=PlaneControlBlend355(m_fSteeringUpDown,ud,step,StaticRef<float>(0x858CC4));
 }
 auto clamp=[](float value){return value>-1.f?(value<1.f?value:1.f):-1.f;};
 m_fSteeringLeftRight=clamp(m_fSteeringLeftRight);m_fSteeringUpDown=clamp(m_fSteeringUpDown);
 if(m_nModelIndex==539){
  const float step=CTimer::GetTimeStep();const int32 axis=CPad::GetPad(playerNum)->GetSteeringLeftRight();
  const float target=float(axis)*StaticRef<float>(0x858B88);m_fLeftRightSkid=PlaneInputSkid355(m_fLeftRightSkid,target,step);
 }else{
  const float right=float(uint8(CPad::GetPad(playerNum)->GetLookRight()));const bool left=CPad::GetPad(playerNum)->GetLookLeft();
  m_fLeftRightSkid=PlaneInputSkid355(m_fLeftRightSkid,left?-1.f:right,CTimer::GetTimeStep());
 }
 m_fSteerAngle=PlaneGroundSteer355(forwardSpeed,m_pHandlingData->m_fSteeringLock,m_fLeftRightSkid);
 if(m_nModelIndex==539&&CPad::GetPad(playerNum)->GetHandBrake()){
  const float multiplier=StaticRef<float>(0x858CE8);m_fSteerAngle*=multiplier;m_fLeftRightSkid*=multiplier;
 }
 const int32 accelerator=CPad::GetPad(playerNum)->GetAccelerate();const int32 brake=CPad::GetPad(playerNum)->GetBrake();
 m_fAccelerationBreakStatus=float(accelerator-brake)*StaticRef<float>(0x859A3C);
 auto* pad=CPad::GetPad(playerNum);auto* padBytes=reinterpret_cast<uint8*>(pad);
 const bool toggle=*reinterpret_cast<int16*>(padBytes+0x26)!=0&&*reinterpret_cast<int16*>(padBytes+0x56)==0;
 const auto* flying=reinterpret_cast<const uint8*>(m_pFlyingHandlingData);
 if(toggle&&std::ranges::all_of(m_wheelDistToGround,[](float v){return std::bit_cast<uint32>(v)==0x3F800000u;})&&*reinterpret_cast<const float*>(flying+0x30)>1.f&&m_nModelIndex!=464){
  if(m_fLandingGearStatus==0.f){bytes[0x5A5]=bytes[0x5A6]=bytes[0x5A7]=bytes[0x5A8]=2;m_fLandingGearStatus=PlaneControlProductAdd355(0.f,CTimer::GetTimeStep(),StaticRef<float>(0x871904));}
  else if(std::bit_cast<uint32>(m_fLandingGearStatus)==0x3F800000u)m_fLandingGearStatus=PlaneControlProductAdd355(-1.f,CTimer::GetTimeStep(),StaticRef<float>(0x871904));
  else m_fLandingGearStatus=-m_fLandingGearStatus;
 }else if(m_fLandingGearStatus<0.f){
  m_fLandingGearStatus=PlaneControlProductAdd355(m_fLandingGearStatus,CTimer::GetTimeStep(),StaticRef<float>(0x871904));
  if(m_fLandingGearStatus>=0.f){m_fLandingGearStatus=0.f;bytes[0x5A5]=bytes[0x5A6]=bytes[0x5A7]=bytes[0x5A8]=0;m_fAirResistance=m_pHandlingData->m_fDragMult/StaticRef<float>(0x871900)*StaticRef<float>(0x858B8C);}
 }else if(m_fLandingGearStatus>0.f&&m_fLandingGearStatus<1.f){
  m_fLandingGearStatus=PlaneControlProductAdd355(m_fLandingGearStatus,CTimer::GetTimeStep(),StaticRef<float>(0x871904));if(m_fLandingGearStatus>=1.f)SetGearUp();
 }
 if(m_nModelIndex==520&&std::abs(CPad::GetPad(playerNum)->GetCarGunUpDown())>StaticRef<float>(0x85862C)){
  auto& nozzle=*reinterpret_cast<uint16*>(bytes+0x86C);*reinterpret_cast<uint16*>(bytes+0x86E)=nozzle;
  float step=CTimer::GetTimeStep();const int32 firstAxis=CPad::GetPad(playerNum)->GetCarGunUpDown();const int32 first=int32(nozzle)+PlaneNozzleChange355(firstAxis,step);
  if(first<0)nozzle=0;else{step=CTimer::GetTimeStep();const int32 secondAxis=CPad::GetPad(playerNum)->GetCarGunUpDown();nozzle=uint16(int32(nozzle)+PlaneNozzleChange355(secondAxis,step));}
  const int32 limit=int16(HARRIER_NOZZLE_ROTATE_LIMIT);if(int32(nozzle)>limit)nozzle=uint16(limit);
 }
 bytes[0x428]&=0xDF;m_fGasPedal=0.f;
 if(forwardSpeed>0.f&&CPad::GetPad(playerNum)->GetBrake()!=0){const int32 input=CPad::GetPad(playerNum)->GetBrake();m_fBrakePedal=float(input)*StaticRef<float>(forwardSpeed>StaticRef<float>(0x858F9C)?0x871A70:0x859A3C);}
 else if(m_vecMoveSpeed.Magnitude()<StaticRef<float>(0x858B1C)&&CPad::GetPad(playerNum)->GetBrake()<10&&CPad::GetPad(playerNum)->GetAccelerate()<10)m_fBrakePedal=.5f;
 else m_fBrakePedal=0.f;
 if(CPad::GetPad(playerNum)->DisablePlayerControls){bytes[0x428]|=0x20;m_fBrakePedal=1.f;m_fGasPedal=0.f;FindPlayerPed(-1)->KeepAreaAroundPlayerClear();PlaneInputSpeedCap355(&m_vecMoveSpeed);}
}
'''
Path('reversing/VehicleAudio355Evidence/Plane_InputsNativeDraft.txt').write_text(h+code)
