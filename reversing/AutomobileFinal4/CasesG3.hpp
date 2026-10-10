#pragma once
// Grupo G3 del banco AutomobileFinal4: controles de vehiculos especiales y rutinas que tocan mucho estado externo.
//   BoostJumpControl, NitrousControl, StopNitroEffect, TowTruckControl, TankControl, FireTruckControl, BlowUpCarsInPath, BlowUpCar,
//   BlowUpCarCutSceneNoExtras, ProcessCarOnFireAndExplode, ScanForCrimes, PlaceOnRoadProperly, RcbanditCheck1CarWheels,
//   RcbanditCheckHitWheels, HydraulicControl, UpdateMovingCollision, ProcessHarvester, ProcessAutoBusDoors, PlayCarHorn, ProcessControlInputs.
// Interfaz obligatoria (la llama LiveComparison.hpp):
//   RunG3(Ctx&)       -> true si c.nm es una funcion de este grupo (ejecuta UN lado: original o candidato segun los hooks ya fijados)
//   StubsG3()         -> registra los dobles (AddStub/AddHookStub con propietario 3); devuelve los nombres que falten ("" si todo bien)
//   InitG3()          -> Reg() de buferes propios y Save() de globales propios (una vez, al arrancar)
//   LayoutG3(json&)   -> comprobaciones de disposicion propias; devuelve false si fallan
// Los fixtures se reconstruyen desde cero en cada llamada; todo el azar sale de c.next() (identico en ambos lados) y los dobles
// consumen un oraculo sembrado con c.next(). Ver README.md del banco para los limites (ramas con basura de pila del EXE, CObject, ...).
#include "CasesG3_c.hpp"
#include "PlayerInfo.h"
#include "Collision/CollisionData.h"
#include "Collision/ColModel.h"
#include "MouseControllerState.h"

namespace AutomobileFinal4Live {

inline bool RunG3(Ctx& c){
    const std::string& n=c.nm;
    if(n=="BoostJumpControl"){ g3::R_BoostJump(c); return true; }
    if(n=="NitrousControl"){ g3::R_Nitrous(c); return true; }
    if(n=="StopNitroEffect"){ g3::R_StopNitro(c); return true; }
    if(n=="TowTruckControl"){ g3::R_TowTruck(c); return true; }
    if(n=="TankControl"){ g3::R_Tank(c); return true; }
    if(n=="FireTruckControl"){ g3::R_FireTruck(c); return true; }
    if(n=="BlowUpCarsInPath"){ g3::R_BlowUpInPath(c); return true; }
    if(n=="BlowUpCar"){ g3::R_BlowUpCar(c); return true; }
    if(n=="BlowUpCarCutSceneNoExtras"){ g3::R_BlowUpCut(c); return true; }
    if(n=="ProcessCarOnFireAndExplode"){ g3::R_CarOnFire(c); return true; }
    if(n=="ScanForCrimes"){ g3::R_ScanForCrimes(c); return true; }
    if(n=="PlaceOnRoadProperly"){ g3::R_PlaceOnRoad(c); return true; }
    if(n=="RcbanditCheck1CarWheels"){ g3::R_RcCheck1(c); return true; }
    if(n=="RcbanditCheckHitWheels"){ g3::R_RcHitWheels(c); return true; }
    if(n=="HydraulicControl"){ g3::R_Hydraulic(c); return true; }
    if(n=="UpdateMovingCollision"){ g3::R_UpdateMovingCollision(c); return true; }
    if(n=="ProcessHarvester"){ g3::R_Harvester(c); return true; }
    if(n=="ProcessAutoBusDoors"){ g3::R_BusDoors(c); return true; }
    if(n=="PlayCarHorn"){ g3::R_PlayCarHorn(c); return true; }
    if(n=="ProcessControlInputs"){ g3::R_ControlInputs(c); return true; }
    return false;
}

inline std::string StubsG3(){ return g3::Register(); }

inline void InitG3(){
    using namespace g3;
    Reg(&gw,sizeof(gw));
    // Inicializacion de la estatica local 'wheelMatrix' de RcbanditCheck1CarWheels en el EXE (0xC1C220, guarda en 0xC1C268): la rama de
    // inicializacion llama a atexit; se deja ya inicializada (campos 0xC1C260/0xC1C264 a cero) ANTES de guardar el estado.
    if(!(AtA<uint32>(0xC1C268)&1u)){ AtA<uint32>(0xC1C268)|=1u; AtA<uint32>(0xC1C260)=0; AtA<uint32>(0xC1C264)=0; }
    Save(A_TIMESTEP,4); Save(A_TIMEMS,4); Save(A_FRAMECTR,4);
    Save(A_SCANCODE,2); Save(A_IGNOREENT,4); Save(A_PLAYERFOCUS,1); Save(A_PLAYERS,PLAYER_SIZE*2);
    Save(A_GAMESTATE,1); Save(A_SKIPSTATE,4); Save(A_NEWSHELI,1); Save(A_AMBULANCES,8); Save(A_TEMPOBJS,2);
    Save(A_SCRATCH,4); Save(A_STATICMAT,0x50);
    Save(A_HYDRAULIC,4*0x28); Save(A_SPECIALCOL,4*0x30);
    Save(A_CTRLFLAGS,8); Save(A_USEMOUSE3RD,1); Save(A_NEWMOUSE,0x14);
    Save(A_REPEAT,16*16*12); Save(A_POOLS,0x40);
    Save(A_NASTY,1); Save(A_MODELIDX,0x18); Save(A_HARVSTREAM,1);
    Save(uintptr_t(CPad::GetPad(0)),2*sizeof(CPad));
    Save(uintptr_t(&TheCamera.m_nActiveCam),1); Save(uintptr_t(&TheCamera.m_aCams),sizeof(TheCamera.m_aCams));
}

inline bool LayoutG3(json& l){
    using namespace g3;
    bool ok=true;
    auto chk=[&](const char* nm,size_t got,size_t want){ const bool g=got==want; ok&=g; l[nm]=g; };
    // desplazamientos de CAutomobile / CVehicle / CPhysical usados con direcciones crudas por este grupo
    chk("g3_tire_temp",offsetof(CAutomobile,m_fTireTemperature),0x8A4);       chk("g3_nitro_boosts",offsetof(CAutomobile,m_nNitroBoosts),0x48A);
    chk("g3_gas",offsetof(CAutomobile,m_GasPedal),0x49C);                       chk("g3_brake",offsetof(CAutomobile,m_BrakePedal),0x4A0);
    chk("g3_steer",offsetof(CAutomobile,m_fSteerAngle),0x494);                  chk("g3_steer2",offsetof(CAutomobile,m_f2ndSteerAngle),0x498);
    chk("g3_rawsteer",offsetof(CAutomobile,m_fRawSteerAngle),0x58C);            chk("g3_driver",offsetof(CAutomobile,m_pDriver),0x460);
    chk("g3_miscangle",offsetof(CAutomobile,m_wMiscComponentAngle),0x86C);      chk("g3_bus_end",offsetof(CAutomobile,m_nBusDoorTimerEnd),0x870);
    chk("g3_bus_start",offsetof(CAutomobile,m_nBusDoorTimerStart),0x874);       chk("g3_doom_v",offsetof(CAutomobile,m_fDoomVerticalRotation),0x94C);
    chk("g3_doom_h",offsetof(CAutomobile,m_fDoomHorizontalRotation),0x950);     chk("g3_gunfire",offsetof(CAutomobile,m_nGunFiringTime),0x4D4);
    chk("g3_burn",offsetof(CAutomobile,m_fBurnTimer),0x8E4);                    chk("g3_victim",offsetof(CAutomobile,m_pExplosionVictim),0x928);
    chk("g3_harv_cnt",offsetof(CAutomobile,m_harvesterParticleCounter),0x980);  chk("g3_fire_cnt",offsetof(CAutomobile,m_fireParticleCounter),0x981);
    chk("g3_prevspeed",offsetof(CAutomobile,m_PrevSpeed),0x8B0);                chk("g3_horn_cnt",offsetof(CAutomobile,m_HornCounter),0x514);
    chk("g3_horn_timer",offsetof(CAutomobile,m_nCarHornTimer),0x519);           chk("g3_comedy",offsetof(CAutomobile,m_comedyControlState),0x51A);
    chk("g3_fire_fx",offsetof(CAutomobile,m_pFireParticle),0x57C);              chk("g3_alarm",offsetof(CAutomobile,m_nAlarmState),0x45C);
    chk("g3_getting_in",offsetof(CAutomobile,m_nGettingInFlags),0x486);         chk("g3_towed",offsetof(CAutomobile,m_pVehicleBeingTowed),0x4C8);
    chk("g3_susp_comp",offsetof(CAutomobile,m_fWheelsSuspensionCompression),0x7D4); chk("g3_susp_prev",offsetof(CAutomobile,m_fWheelsSuspensionCompressionPrev),0x7E4);
    chk("g3_susp_spring",offsetof(CAutomobile,m_aSuspensionSpringLength),0x878);    chk("g3_susp_line",offsetof(CAutomobile,m_aSuspensionLineLength),0x888);
    chk("g3_rear_h",offsetof(CAutomobile,m_fRearHeightAboveRoad),0x89C);        chk("g3_mass",offsetof(CAutomobile,m_fMass),0x8C);
    chk("g3_turnmass",offsetof(CAutomobile,m_fTurnMass),0x90);                  chk("g3_movespeed",offsetof(CAutomobile,m_vecMoveSpeed),0x44);
    chk("g3_blown_t",offsetof(CAutomobile,m_nTimeWhenBlowedUp),0x4D8);          chk("g3_bomb_timer",offsetof(CAutomobile,m_wBombTimer),0x4DE);
    chk("g3_nodes",offsetof(CAutomobile,m_aCarNodes),0x648);                    chk("g3_subtype",offsetof(CAutomobile,m_nVehicleSubType),0x594);
    chk("g3_exhaust_fx",offsetof(CAutomobile,m_exhaustNitroFxSystem),0x978);
    // banderas con campos de bits: se escribe por el miembro y se lee el byte del EXE
    auto bit=[&](const char* nm,size_t off,uint8 want,auto&& set){ std::memset(Cr(1),0,CAR_SIZE); set(Car(1)); chk(nm,Cr(1)[off],want); std::memset(Cr(1),0,CAR_SIZE); };
    bit("g3_f_locked",0x428,0x08,[](CAutomobile* v){ v->vehicleFlags.bIsLocked=1; });
    bit("g3_f_engineon",0x428,0x10,[](CAutomobile* v){ v->vehicleFlags.bEngineOn=1; });
    bit("g3_f_handbrake",0x428,0x20,[](CAutomobile* v){ v->vehicleFlags.bIsHandbrakeOn=1; });
    bit("g3_f_ambulance",0x428,0x02,[](CAutomobile* v){ v->vehicleFlags.bIsAmbulanceOnDuty=1; });
    bit("g3_f_firetruck",0x428,0x04,[](CAutomobile* v){ v->vehicleFlags.bIsFireTruckOnDuty=1; });
    bit("g3_f_lights",0x428,0x40,[](CAutomobile* v){ v->vehicleFlags.bLightsOn=1; });
    bit("g3_f_comedy",0x429,0x10,[](CAutomobile* v){ v->vehicleFlags.bComedyControls=1; });
    bit("g3_f_candamage",0x42A,0x20,[](CAutomobile* v){ v->vehicleFlags.bCanBeDamaged=1; });
    bit("g3_f_drowning",0x42B,0x40,[](CAutomobile* v){ v->vehicleFlags.bIsDrowning=1; });
    bit("g3_f_enginebroken",0x42D,0x02,[](CAutomobile* v){ v->vehicleFlags.bEngineBroken=1; });
    bit("g3_f_siren",0x42D,0x80,[](CAutomobile* v){ v->vehicleFlags.bSirenOrAlarm=1; });
    bit("g3_f_tankexplodes",0x868,0x20,[](CAutomobile* v){ std::memset(&v->autoFlags,0,sizeof(v->autoFlags)); v->autoFlags.bTankExplodesCars=1; });
    bit("g3_f_nos",0x38E,0x08,[](CAutomobile* v){ v->handlingFlags.bNosInst=1; });
    bit("g3_f_scorched",0x43,0x20,[](CAutomobile* v){ v->physicalFlags.bRenderScorched=1; });
    bit("g3_f_visible",0x1C,0x80,[](CAutomobile* v){ v->m_bIsVisible=1; });
    bit("g3_f_renderdamaged",0x1D,0x02,[](CAutomobile* v){ v->m_bRenderDamaged=1; });
    bit("g3_f_tunnel",0x1F,0x40,[](CAutomobile* v){ v->m_bTunnel=1; });
    bit("g3_f_tunneltrans",0x1F,0x80,[](CAutomobile* v){ v->m_bTunnelTransition=1; });
    bit("g3_f_honk",0x3DB,0x01,[](CAutomobile* v){ v->m_autoPilot.carCtrlFlags.bHonkAtCar=1; });
    // mando: los campos de DisablePlayerControls / bApplyBrakes se mueven y se restauran
    { auto* p=CPad::GetPad(0); const uint16 s1=p->DisablePlayerControls; const bool s2=p->bApplyBrakes;
      p->DisablePlayerControls=0x1234; p->bApplyBrakes=true; chk("g3_pad_disable",At<uint16>(p,0x10E),0x1234); chk("g3_pad_brakes",At<uint8>(p,0x118),1);
      p->DisablePlayerControls=s1; p->bApplyBrakes=s2;
      chk("g3_pad_stride",uintptr_t(CPad::GetPad(1))-uintptr_t(CPad::GetPad(0)),sizeof(CPad)); }
    chk("g3_mouse_x",offsetof(CMouseControllerState,m_AmountMoved),0xC); chk("g3_mouse_size",sizeof(CMouseControllerState),0x14);
    chk("g3_cd_triangles",offsetof(CCollisionData,m_pTriangles),0x18); chk("g3_cd_vertices",offsetof(CCollisionData,m_pVertices),0x14);
    chk("g3_cd_lines",offsetof(CCollisionData,m_pLines),0x10); chk("g3_cd_spheres",offsetof(CCollisionData,m_pSpheres),0x8);
    chk("g3_cd_planes",offsetof(CCollisionData,m_pTrianglePlanes),0x1C); chk("g3_cm_coldata",offsetof(CColModel,m_pColData),0x2C);
    chk("g3_pi_bigGun",offsetof(CPlayerInfo,m_nLastTimeBigGunFired),0xEC); chk("g3_pi_havoc",offsetof(CPlayerInfo,m_nHavocCaused),0x140);
    chk("g3_pi_chase",offsetof(CPlayerInfo,m_fCurrentChaseValue),0x148);
    chk("g3_ped_fits",sizeof(CPed)<=PED_SIZE ? 1 : 0,1); chk("g3_node_size",sizeof(CPtrNodeDoubleLink<void*>),12);
    chk("g3_pool_hdr_fits",sizeof(CPtrNodeDoubleLinkPool)<=0x20 ? 1 : 0,1); chk("g3_world_fits",sizeof(g3::G3World)<(1u<<20) ? 1 : 0,1);
    // tHandlingData::m_bHbRearwheelSteer (byte 0xD0, bit 6)
    { std::memset(w.hd[0],0,HD_SIZE); reinterpret_cast<tHandlingData*>(w.hd[0])->m_bHbRearwheelSteer=1; chk("g3_hd_hbrear",w.hd[0][0xD0],0x40); std::memset(w.hd[0],0,HD_SIZE); }
    return ok;
}

} // namespace AutomobileFinal4Live
