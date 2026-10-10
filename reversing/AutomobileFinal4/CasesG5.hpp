#pragma once
// Grupo G5 del banco AutomobileFinal4: CAutomobile::ProcessControl (0x6B1880, vtable[10]) y CAutomobile::ProcessAI (0x6B4800, vtable[66]).
// Interfaz obligatoria (la llama LiveComparison.hpp):
//   RunG5(Ctx&)       -> true si c.nm es una funcion de este grupo (ejecuta UN lado: original o candidato segun los hooks ya fijados)
//   StubsG5()         -> registra los dobles (AddHookStub/AddStub con propietario 5); devuelve los nombres que falten ("" si todo bien)  [CasesG5_a.hpp]
//   InitG5()          -> Reg() de buferes propios y Save() de globales propios (una vez, al arrancar)
//   LayoutG5(json&)   -> comprobaciones de disposicion propias
// Un solo banco repartido en CasesG5_a.hpp (dobles), CasesG5_b.hpp (fixtures + ProcessControl) y CasesG5_c.hpp (ProcessAI).
#include "CasesG5_a.hpp"
#include "CasesG5_b.hpp"
#include "CasesG5_c.hpp"
#include "Entity/Vehicle/Plane.h"
#include "Entity/Vehicle/Heli.h"

namespace AutomobileFinal4Live {

inline bool RunG5(Ctx& c){
    if(c.nm=="ProcessControl"){ G5RunPC(c); return true; }
    if(c.nm=="ProcessAI"){ G5RunAI(c); return true; }
    return false;
}

inline void InitG5(){
    Reg(g5ped,sizeof(g5ped)); Reg(g5intel,sizeof(g5intel)); Reg(g5aux,sizeof(g5aux));
    // globales que las dos funciones leen (y que el banco fija en cada ejecucion)
    Save(0xB7CB5C,4); Save(0xB7CB84,4); Save(0xB7CB4C,4);                      // CTimer: paso, milisegundos, contador de fotogramas
    Save(0xC81340,4); Save(0xC81308,4); Save(0xB7CD6E,1);                      // CWeather::Earthquake / WetRoads, CWorld::bForceProcessControl
    Save(0x96914C,1); Save(0x969152,1); Save(0x969165,1); Save(0x96918B,1);    // CCheat: manejo perfecto, coches sobre agua, nitro, taxis con nitro
    Save(0xC1CB60,0xA0); Save(0xC1CC00,2);                                     // m_aSpecialHydraulicData, bDisableRemoteDetonation(+OnContact)
    Save(0xB6EC2E,1); Save(0xB6F081,1);                                        // m_bUseMouse3rdPerson, TheCamera.m_nActiveCam
    Save(0xB6F1A8,2); Save(0xB6F1A8+0x238,2); Save(0xB6F1A8+0x470,2);          // modo de cada camara
    Save(0xB6F02C,12); Save(0xB6F03C,4);                                       // posicion y matriz de TheCamera
    Save(0xB73418,0x20);                                                       // CPad::NewMouseControllerState
    Save(0x8D3214,4);                                                          // global (4.0f) que activa el bloque de direccion de ProcessControl
    Save(0xB7CD74,1); Save(0xB7CD98,0x320);                                    // CWorld::PlayerInFocus, CWorld::Players[2]
    Save(0x97D6C0,16);                                                         // CVehicleRecording::bUseCarAI
    Save(0xC1CB30,0x18);                                                       // estaticos que escribe cTransmission::CalculateDriveAcceleration
    // superficies reales con grupo de adherencia "arena" y marca de derrape "barro" (consulta pura de tablas)
    g5NumSand=0; g5NumMud=0;
    for(uint32 s=1;s<120;++s){
        if(g_surfaceInfos.GetAdhesionGroup(s)==ADHESION_GROUP_SAND && g5NumSand<g5Sand.size()) g5Sand[g5NumSand++]=uint8(s);
        if(g_surfaceInfos.GetSkidmarkType(s)==2 && g5NumMud<g5Mud.size()) g5Mud[g5NumMud++]=uint8(s);
    }
}

// Comprobaciones de disposicion propias (informativas: se devuelve true siempre para no abortar el banco comun)
inline bool LayoutG5(json& l){
    auto chk=[&](const char* n,size_t got,size_t want){ l[n]=(got==want); };
    chk("g5_comp",offsetof(CAutomobile,m_fWheelsSuspensionCompression),G5o::Comp); chk("g5_compPrev",offsetof(CAutomobile,m_fWheelsSuspensionCompressionPrev),G5o::CompPrev);
    chk("g5_counts",offsetof(CAutomobile,m_WheelCounts),G5o::Counts); chk("g5_inertia1",offsetof(CAutomobile,m_fIntertiaValue1),G5o::Inertia1);
    chk("g5_skid",offsetof(CAutomobile,m_wheelSkidmarkType),G5o::SkidType); chk("g5_muddy",offsetof(CAutomobile,m_wheelSkidmarkMuddy),G5o::Muddy);
    chk("g5_wheelRot",offsetof(CAutomobile,m_wheelRotation),G5o::WheelRot); chk("g5_wheelSpeed",offsetof(CAutomobile,m_wheelSpeed),G5o::WheelSpeed);
    chk("g5_burnout",offsetof(CAutomobile,m_bDoingBurnout),G5o::Burnout); chk("g5_miscAngle",offsetof(CAutomobile,m_wMiscComponentAngle),G5o::MiscAngle);
    chk("g5_busEnd",offsetof(CAutomobile,m_nBusDoorTimerEnd),G5o::BusEnd); chk("g5_spring",offsetof(CAutomobile,m_aSuspensionSpringLength),G5o::SpringLen);
    chk("g5_line",offsetof(CAutomobile,m_aSuspensionLineLength),G5o::LineLen); chk("g5_front",offsetof(CAutomobile,m_fFrontHeightAboveRoad),G5o::FrontH);
    chk("g5_carTraction",offsetof(CAutomobile,m_fCarTraction),G5o::CarTraction); chk("g5_tireTemp",offsetof(CAutomobile,m_fTireTemperature),G5o::TireTemp);
    chk("g5_wheelEnt",offsetof(CAutomobile,m_apWheelCollisionEntity),G5o::WheelEnt); chk("g5_wheelEntPos",offsetof(CAutomobile,m_vWheelCollisionPos),G5o::WheelEntPos);
    chk("g5_doom",offsetof(CAutomobile,m_fDoomVerticalRotation),G5o::Doom); chk("g5_contact",offsetof(CAutomobile,m_nNumContactWheels),G5o::Contact);
    chk("g5_driveWheels",offsetof(CAutomobile,m_NumDriveWheelsOnGround),G5o::DriveWheels); chk("g5_wheelState",offsetof(CAutomobile,m_WheelStates),G5o::WheelState);
    chk("g5_nitroFx",offsetof(CAutomobile,m_exhaustNitroFxSystem),G5o::NitroFx); chk("g5_fireCounter",offsetof(CAutomobile,m_fireParticleCounter),G5o::FireCounter);
    chk("g5_wheelCol",offsetof(CAutomobile,m_wheelColPoint),G5o::WheelCol); chk("g5_dmgMgr",offsetof(CAutomobile,m_damageManager),G5o::DmgMgr);
    chk("g5_alarm",offsetof(CAutomobile,m_nAlarmState),G5o::Alarm); chk("g5_driver",offsetof(CAutomobile,m_pDriver),G5o::Driver);
    chk("g5_pass",offsetof(CAutomobile,m_apPassengers),G5o::Pass); chk("g5_numPass",offsetof(CAutomobile,m_nNumPassengers),G5o::NumPass);
    chk("g5_maxPass",offsetof(CAutomobile,m_nMaxPassengers),G5o::MaxPass); chk("g5_nitro",offsetof(CAutomobile,m_nNitroBoosts),G5o::Nitro);
    chk("g5_specCol",offsetof(CAutomobile,m_vehicleSpecialColIndex),G5o::SpecCol); chk("g5_steer",offsetof(CAutomobile,m_fSteerAngle),G5o::Steer);
    chk("g5_gas",offsetof(CAutomobile,m_GasPedal),G5o::Gas); chk("g5_brake",offsetof(CAutomobile,m_BrakePedal),G5o::Brake);
    chk("g5_gear",offsetof(CAutomobile,m_nCurrentGear),G5o::Gear); chk("g5_spinAudio",offsetof(CAutomobile,m_fWheelSpinForAudio),G5o::SpinAudio);
    chk("g5_health",offsetof(CAutomobile,m_fHealth),G5o::Health); chk("g5_towing",offsetof(CAutomobile,m_pTowingVehicle),G5o::Towing);
    chk("g5_beingTowed",offsetof(CAutomobile,m_pVehicleBeingTowed),G5o::BeingTowed); chk("g5_bombOwner",offsetof(CAutomobile,m_pWhoInstalledBombOnMe),G5o::BombOwner);
    chk("g5_bombTimer",offsetof(CAutomobile,m_wBombTimer),G5o::BombTimer); chk("g5_detonator",offsetof(CAutomobile,m_pWhoDetonatedMe),G5o::Detonator);
    chk("g5_horn",offsetof(CAutomobile,m_HornCounter),G5o::Horn); chk("g5_subType",offsetof(CAutomobile,m_nVehicleSubType),G5o::SubType);
    chk("g5_moveSpd",offsetof(CAutomobile,m_vecMoveSpeed),G5o::MoveSpd); chk("g5_turnSpd",offsetof(CAutomobile,m_vecTurnSpeed),G5o::TurnSpd);
    chk("g5_mass",offsetof(CAutomobile,m_fMass),G5o::Mass); chk("g5_turnMass",offsetof(CAutomobile,m_fTurnMass),G5o::TurnMass);
    chk("g5_com",offsetof(CAutomobile,m_vecCentreOfMass),G5o::CoM); chk("g5_dmgInt",offsetof(CAutomobile,m_fDamageIntensity),G5o::DmgInt);
    chk("g5_dmgEnt",offsetof(CAutomobile,m_pDamageEntity),G5o::DmgEnt); chk("g5_impact",offsetof(CAutomobile,m_vecLastCollisionImpactVelocity),G5o::Impact);
    chk("g5_movingSpeed",offsetof(CAutomobile,m_fMovingSpeed),G5o::MovingSpeed); chk("g5_fake",offsetof(CAutomobile,m_nFakePhysics),G5o::Fake);
    chk("g5_force",offsetof(CAutomobile,m_vecForce),G5o::Force); chk("g5_torque",offsetof(CAutomobile,m_vecTorque),G5o::Torque);
    chk("g5_velFreq",offsetof(CAutomobile,m_fVelocityFrequency),G5o::VelFreq);
    chk("g5_planeBreak",offsetof(CPlane,m_fAccelerationBreakStatus),G5o::PlaneBreak); chk("g5_heliBreak",offsetof(CHeli,m_fAccelerationBreakStatus),G5o::HeliBreak);
    chk("g5_players",reinterpret_cast<uintptr_t>(&CWorld::Players[0]),0xB7CD98u);
    chk("g5_remoteVeh",offsetof(CPlayerInfo,m_pRemoteVehicle),0xB0u); chk("g5_carLess3",offsetof(CPlayerInfo,m_nCarLess3WheelCounter),0x100u);
    chk("g5_colData",offsetof(CColModel,m_pColData),0x2Cu); chk("g5_numLines",offsetof(CCollisionData,m_nNumLines),6u);
    chk("g5_mouse",offsetof(CMouseControllerState,m_AmountMoved),0xCu);
    chk("g5_pedState",offsetof(CPed,m_nPedState),0x530u); chk("g5_pedType",offsetof(CPed,m_nPedType),0x598u); { alignas(16) static char b5[0x700]; std::memset(b5,0,sizeof b5); *reinterpret_cast<uintptr_t*>(b5+0x480)=0x1234; chk("g5_pedPlayerData",reinterpret_cast<CPed*>(b5)->GetPlayerData()==reinterpret_cast<CPlayerPedData*>(uintptr_t(0x1234)) ? size_t(0x480) : size_t(0),0x480u); }
    chk("g5_apSpeed",offsetof(CAutomobile,m_autoPilot)+offsetof(CAutoPilot,m_speed),G5o::ApSpeed);
    chk("g5_audio",offsetof(CAutomobile,m_vehicleAudio),G5o::Audio); chk("g5_handling",offsetof(CAutomobile,m_pHandlingData),G5o::Handling);
    chk("g5_handlingFlags",offsetof(CAutomobile,m_nHandlingFlagsIntValue),G5o::HFlags); chk("g5_vehicleFlags",offsetof(CAutomobile,m_nVehicleUpperFlags),G5o::VFlags);
    chk("g5_createdBy",offsetof(CAutomobile,m_nCreatedBy),G5o::CreatedBy);
    chk("g5_mission",offsetof(CAutomobile,m_autoPilot)+offsetof(CAutoPilot,m_nCarMission),G5o::Mission); chk("g5_recId",offsetof(CAutomobile,m_autoPilot)+offsetof(CAutoPilot,m_vehicleRecordingId),G5o::RecId);
    chk("g5_camMode",reinterpret_cast<uintptr_t>(&TheCamera.m_aCams[0].m_nMode),0xB6F1A8u); chk("g5_camActive",reinterpret_cast<uintptr_t>(&TheCamera.m_nActiveCam),0xB6F081u);
    chk("g5_mouse3rd",reinterpret_cast<uintptr_t>(&CCamera::m_bUseMouse3rdPerson),0xB6EC2Eu);
    return true;
}
}
