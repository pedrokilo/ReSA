from pathlib import Path
math=Path('reversing/VehicleAudio355Evidence/Plane_GenerationMathDraft.txt').read_text();select=Path('reversing/VehicleAudio355Evidence/Plane_GenerationSelectionDraft.txt').read_text();select=select[select.index('static void'):]
code=r'''
static void PlaneGenerationEngine355(CVehicle* vehicle){auto* bytes=reinterpret_cast<uint8*>(vehicle);if(!(bytes[0x42D]&2))bytes[0x428]|=0x10;}
void CPlane::DoPlaneGenerationAndRemoval(){
 if((CTimer::GetFrameCounter()&31)!=30||!GenPlane_Active)return;
 const int32 aircraftCount=CountPlanesAndHelis();
 if(!GenPlane_Status){PlaneGenerationSelect355(aircraftCount);return;}
 if(GenPlane_Status!=1)return;
 if(StaticRef<uint8>(0x8E4CD0+uintptr_t(GenPlane_ModelIndex)*0x14)!=1){CStreaming::RequestModel(GenPlane_ModelIndex,8);return;}
 CVector coordinates,target;float heading,height;
 if(GenPlane_ModelIndex==497){
  FindPlaneCreationCoors(&coordinates,&target,&heading,&height,false);
  auto* storage=plugin::CallAndReturn<void*,0x6E2D50>(0xA18);
  auto* vehicle=storage?plugin::CallMethodAndReturn<CVehicle*,0x6C4190>(storage,GenPlane_ModelIndex,1):nullptr;
  vehicle->SetPosn(coordinates);PlaneGenerationHeading355(vehicle,heading);CCarCtrl::JoinCarWithRoadSystem(vehicle);
  auto* bytes=reinterpret_cast<uint8*>(vehicle);*reinterpret_cast<float*>(bytes+0x84C)=std::bit_cast<float>(0x3E6147AEu);vehicle->SetStatus(STATUS_PHYSICS);CWorld::Add(vehicle);
  *reinterpret_cast<float*>(bytes+0x9B0)=height;bytes[0x3BA]=38;*reinterpret_cast<float*>(bytes+0x9B4)=heading;*reinterpret_cast<float*>(bytes+0x9A8)=height;
  vehicle->SetVelocity(PlaneGenerationVelocity355(heading,true));bytes[0x42C]|=0x40;*reinterpret_cast<uint16*>(bytes+0x4A6)=255;bytes[0x3D0]=50;PlaneGenerationEngine355(vehicle);
  const auto random=CGeneral::GetRandomNumber()%2;
  if(random==1){
   const CVector planePosition=vehicle->GetPosition();const CVector playerPosition=FindPlayerCoors(-1);auto* ped=CWorld::FindUnsuspectingTargetPed(planePosition,playerPosition);
   if(ped){
    bytes[0x3BA]=39;auto** reference=reinterpret_cast<CEntity**>(bytes+0x41C);*reference=ped;ped->RegisterReference(reference);bytes[0x988]|=2;bytes[0x3DB]|=0xA0;
    auto* group=CPedGroups::GetPedsGroup(ped);
    if(group&&reinterpret_cast<uint8*>(group)[0x2D0]!=1){CEventDanger event{vehicle,200.f};event.m_TaskId=static_cast<eTaskType>(1505);group->GetIntelligence().AddEvent(&event);}
    else if(!ped->IsPlayer()&&reinterpret_cast<uint8*>(ped)[0x484]!=2&&*reinterpret_cast<int32*>(reinterpret_cast<uint8*>(ped)+0x598)!=6){CEventDanger event{vehicle,200.f};event.m_TaskId=static_cast<eTaskType>(911);ped->GetEventGroup().Add(&event,false);}
   }
  }else if(random==0){
   const CVector planePosition=vehicle->GetPosition();const CVector playerPosition=FindPlayerCoors(-1);auto* targetCar=CWorld::FindUnsuspectingTargetCar(planePosition,playerPosition);
   if(targetCar){auto** reference=reinterpret_cast<CEntity**>(bytes+0x41C);*reference=targetCar;bytes[0x3BA]=39;targetCar->RegisterReference(reference);bytes[0x988]|=2;bytes[0x3DB]|=0xA0;}
  }
 }else{
  const bool large=GenPlane_ModelIndex==577||GenPlane_ModelIndex==592;
  FindPlaneCreationCoors(&coordinates,&target,&heading,&height,large);
  auto* storage=plugin::CallAndReturn<void*,0x6E2D50>(0xA04);
  auto* plane=storage?plugin::CallMethodAndReturn<CPlane*,0x6C8E20>(storage,GenPlane_ModelIndex,1):nullptr;
  plane->SetPosn(coordinates);plane->m_planeCreationHeading=heading;PlaneGenerationHeading355(plane,heading);CCarCtrl::JoinCarWithRoadSystem(plane);
  plane->SetStatus(STATUS_PHYSICS);CWorld::Add(plane);auto* bytes=reinterpret_cast<uint8*>(plane);
  plane->m_planeHeading=heading;plane->m_altitude=height;bytes[0x3BA]=36;plane->m_planeHeadingPrev=heading;plane->m_maxAltitude=height;
  plane->SetVelocity(PlaneGenerationVelocity355(heading,false));bytes[0x42C]|=0x40;
  if(!large&&GenPlane_ModelIndex!=520)*reinterpret_cast<uint16*>(bytes+0x4A6)=150;
  else{*reinterpret_cast<uint16*>(bytes+0x4A6)=360;if(large)plane->m_nFlags&=0xFFFFFFFEu;}
  if(GenPlane_ModelIndex==520){bytes[0x3BA]=59;*reinterpret_cast<uint16*>(bytes+0x86C)=0;}
  plane->SetGearUp();PlaneGenerationEngine355(plane);CStreaming::SetModelIsDeletable(GenPlane_ModelIndex);CStreaming::SetModelTxdIsDeletable(GenPlane_ModelIndex);
 }
 GenPlane_LastTimeGenerated=CTimer::GetTimeInMS();GenPlane_Status=0;
}
'''
Path('reversing/VehicleAudio355Evidence/Plane_GenerationNativeDraft.txt').write_text(math+select+'\n'+code)
