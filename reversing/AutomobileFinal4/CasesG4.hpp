#pragma once
// Grupo G4 del banco AutomobileFinal4 (9 funciones de CAutomobile con mucha fisica): ProcessSuspension (v70), DoBurstAndSoftGroundRatios
// (v52), DoHoverSuspensionRatios (v69), DoSoftGroundResistance (0x6A4AF0), ProcessCarWheelPair (0x6A4EC0), ProcessBuoyancy (0x6A8C00),
// ProcessEntityCollision (v22), ProcessControlCollisionCheck (v23) y VehicleDamage (v56).
// Interfaz obligatoria (la llama LiveComparison.hpp):
//   RunG4(Ctx&)       -> true si c.nm es una funcion de este grupo (ejecuta UN lado: original o candidato segun los hooks ya fijados)
//   StubsG4()         -> registra los dobles (AddStub/AddHookStub con propietario 4); devuelve los nombres que falten ("" si todo bien)
//   InitG4()          -> Reg() de buferes propios y Save() de globales propios (una vez, al arrancar)
//   LayoutG4(json&)   -> comprobaciones de disposicion propias; devuelve false si fallan
// Los coches son falsos (w.car[k]); los colaboradores que tocan estado real (fuerzas, muelles, agua, ProcessWheel, flotabilidad, colision,
// eventos, peatones, audio...) se doblan con trazas y efectos simplificados sobre los coches falsos; lo puro (CColPoint, tablas de
// superficies, x87, CDamageManager, GetSpeed...) se ejecuta de verdad. Los peatones/eventos solo se usan si resuelven todos sus simbolos.
#include "CasesG4_c.hpp"

namespace AutomobileFinal4Live {

inline bool RunG4(Ctx& c){
    if(c.nm=="DoHoverSuspensionRatios")      return G4_RunHover(c);
    if(c.nm=="DoBurstAndSoftGroundRatios")   return G4_RunBurst(c);
    if(c.nm=="DoSoftGroundResistance")       return G4_RunSoft(c);
    if(c.nm=="ProcessSuspension")            return G4_RunSusp(c);
    if(c.nm=="ProcessCarWheelPair")          return G4_RunWheelPair(c);
    if(c.nm=="ProcessBuoyancy")              return G4_RunBuoy(c);
    if(c.nm=="ProcessEntityCollision")       return G4_RunEntCol(c);
    if(c.nm=="ProcessControlCollisionCheck") return G4_RunCtrlCol(c);
    if(c.nm=="VehicleDamage")                return G4_RunDamage(c);
    return false;
}

inline std::string StubsG4(){
    std::string miss;
    const auto H=[&](uint64 bit,uintptr_t exe,const char* cls,const char* name,void* fn){ if(!AddHookStub(4,bit,exe,cls,name,fn)) miss+=std::string(cls)+"::"+name+";"; };
    // fisica y colision
    H(G4_B_FORCE ,0x5429F0,"CPhysical","ApplyMoveForce-vec",FnPtr(&G4_MoveForce));
    H(G4_B_FORCE ,0x542A50,"CPhysical","ApplyTurnForce",FnPtr(&G4_TurnForce));
    H(G4_B_SPRING,0x543C90,"CPhysical","ApplySpringCollision",FnPtr(&G4_Spring));
    H(G4_B_SPRING,0x543D60,"CPhysical","ApplySpringCollisionAlt",FnPtr(&G4_SpringAlt));
    H(G4_B_SPRING,0x543E90,"CPhysical","ApplySpringDampening",FnPtr(&G4_Damp));
    H(G4_B_WATER ,0x6EB690,"CWaterLevel","GetWaterLevel-",FnPtr(&G4_Water));
    H(G4_B_WHEEL ,0x6D6C00,"CVehicle","ProcessWheel",FnPtr(&G4_Wheel));
    H(G4_B_BUOY  ,0x6C3EF0,"cBuoyancy","ProcessBuoyancy",FnPtr(&G4_Buoy));
    H(G4_B_COLL  ,0x4185C0,"CCollision","ProcessColModels",FnPtr(&G4_ProcCol));
    H(G4_B_COLL  ,0x543490,"CPhysical","AddCollisionRecord",FnPtr(&G4_AddRec));
    H(G4_B_REF   ,0x571B70,"CEntity","RegisterReference-",FnPtr(&G4_RegRef));
    H(G4_B_CHECK ,0x54D920,"CPhysical","CheckCollision",FnPtr(&G4_Check));
    H(G4_B_CHECK ,0x5433B0,"CPhysical","SkipPhysics",FnPtr(&G4_Skip));
    H(G4_B_CHECK ,0x542DD0,"CPhysical","ApplyMoveSpeed",FnPtr(&G4_MoveSpeed));
    H(G4_B_CHECK ,0x542E20,"CPhysical","ApplyTurnSpeed",FnPtr(&G4_TurnSpeed));
    // VehicleDamage
    H(G4_B_DMG,0x6D1280,"CVehicle","CanVehicleBeDamaged",FnPtr(&G4_CanDmg));
    H(G4_B_DMG,0x6B5A10,"CBike","DamageKnockOffRider",FnPtr(&G4_Knock));
    H(G4_B_DMG,0x5591E0,"CStats","GetPercentageProgress",FnPtr(&G4_Progress));
    H(G4_B_DMG,0x609F30,"CPlayerPed","SetWantedLevelNoDrop",FnPtr(&G4_Wanted));
    H(G4_B_DMG,0x53F920,"CPad","StartShake",FnPtr(&G4_Shake));
    H(G4_B_DMG,0x602590,"CInterestingEvents","Add",FnPtr(&G4_Interesting));
    H(G4_B_DMG,0x4F6420,"CAEVehicleAudioEntity","AddAudioEvent-0",FnPtr(&G4_AudioEvent));
    H(G4_B_DMG,0x4F4F00,"CAEVehicleAudioEntity","GetVehicleTypeForAudio",FnPtr(&G4_VehTypeAudio));
    H(G4_B_DMG,0x6D2A10,"CVehicle","PickRandomPassenger",FnPtr(&G4_PickPassenger));
    H(G4_B_DMG,0x5EFFE0,"CPed","Say",FnPtr(&G4_Say));
    H(G4_B_DMG,0x6C24B0,"CDamageManager","ApplyDamage",FnPtr(&G4_ApplyDamage));
    AddStub(4,G4_B_DMG,0x6C24B0,std::bit_cast<uintptr_t>(&CDamageManager::ApplyDamage),FnPtr(&G4_ApplyDamage));   // la DLL llama al miembro, no al envoltorio del hook
    H(G4_B_REF,0x571A00,"CEntity","CleanUpOldReference-",FnPtr(&G4_CleanUp));
    H(G4_B_DMG,0x6A6DC0,"CAutomobile","dmgDrawCarCollidingParticles",FnPtr(&G4_DmgDraw));
    H(G4_B_DMG,0x6D5490,"CVehicle","ReactToVehicleDamage",FnPtr(&G4_React));
    // jugador (comun, con traza): FindPlayerVehicle / FindPlayerPed
    if(!AddHookStub(0,G_PLAYER,0x56E0D0,"CWorld","FindPlayerVehicle",FnPtr(&S_FindPlayerVehicle))) miss+="CWorld::FindPlayerVehicle;";
    if(!AddHookStub(0,G_PLAYER,0x56E210,"CWorld","FindPlayerPed",FnPtr(&S_FindPlayerPed))) miss+="CWorld::FindPlayerPed;";
    // peatones y eventos (constructores/destructores de eventos y calculadora: se ejecutan los reales; solo se doblan Add/AffectsPed/Compute/Breath)
    // (opcionales: si falta alguno los casos se hacen sin conductor ni pasajeros)
    g4PedOk=true;
    const auto O=[&](uintptr_t exe,const char* cls,const char* name,void* fn){ if(!AddHookStub(4,G4_B_PED,exe,cls,name,fn)) g4PedOk=false; };
    const auto E=[&](uintptr_t exe,const char* mangled,void* fn){ const uintptr_t d=G4_Exported(mangled); if(!d){ g4PedOk=false; return; } AddStub(4,G4_B_PED,exe,d,fn); };
    O(0x4AB420,"CEventGroup","Add-",FnPtr(&G4_EvAdd));
    O(0x60A8D0,"CPlayerPed","HandlePlayerBreath",FnPtr(&G4_Breath));
    O(0x4B5AC0,"CPedDamageResponseCalculator","ComputeDamageResponse",FnPtr(&G4_PdrCompute));
    O(0x4B35A0,"CEventDamage","AffectsPed",FnPtr(&G4_EvDmgAffects));
    return miss;
}

inline void InitG4(){
    Reg(g4OutBuf.data(),g4OutBuf.size());
    for(auto& b:g4PedBuf)Reg(b.data(),b.size());
    for(auto& b:g4IntelBuf)Reg(b.data(),b.size());
    // globales del EXE que escriben o leen estas funciones (se restauran tras cada lado)
    Save(G4_X_TIMESTEP,4);                 // CTimer::ms_fTimeStep
    Save(G4_X_TIMEMS,4);                   // CTimer::m_snTimeInMilliseconds
    Save(G4_X_WETROADS,4);                 // CWeather::WetRoads
    Save(G4_X_REPLAY,4);                   // CReplay::Mode
    Save(G4_X_CHEATS,0x60);                // CCheat::m_aCheatsActive (CARS_ON_WATER, SMASH_N_BOOM...)
    Save(G4_X_AUTOCOLPTS,0x210);           // aAutomobileColPoints (12 x CColPoint)
    Save(G4_X_PLAYERS_VTC,4);              // CWorld::Players[0].m_nVehicleTimeCounter
    Save(G4_X_PLAYERS_VTC+0x190,4);        // CWorld::Players[1].m_nVehicleTimeCounter
    Save(G4_X_CURRAREA,4);                 // CGame::currArea (el EXE lo lee directamente en VehicleDamage)
    G4_BuildSurfaceTables();
}

inline bool LayoutG4(json& l){
    bool ok=true;
    const auto chk=[&](const char* n,size_t got,size_t want){ const bool g=got==want; ok&=g; l[n]=g; };
    // CAutomobile
    chk("g4_doors",offsetof(CAutomobile,m_doors),0x5B8);
    chk("g4_comp",offsetof(CAutomobile,m_fWheelsSuspensionCompression),0x7D4);
    chk("g4_compPrev",offsetof(CAutomobile,m_fWheelsSuspensionCompressionPrev),0x7E4);
    chk("g4_wheelCounts",offsetof(CAutomobile,m_WheelCounts),0x7F4);
    chk("g4_wheelRot",offsetof(CAutomobile,m_wheelRotation),0x828);
    chk("g4_wheelSpeed",offsetof(CAutomobile,m_wheelSpeed),0x848);
    chk("g4_burnoutSpeed",offsetof(CAutomobile,m_fWheelBurnoutSpeed),0x858);
    chk("g4_burnout",offsetof(CAutomobile,m_bDoingBurnout),0x86A);
    chk("g4_miscAngle",offsetof(CAutomobile,m_wMiscComponentAngle),0x86C);
    chk("g4_miscAnglePrev",offsetof(CAutomobile,m_wMiscComponentAnglePrev),0x86E);
    chk("g4_spring",offsetof(CAutomobile,m_aSuspensionSpringLength),0x878);
    chk("g4_line",offsetof(CAutomobile,m_aSuspensionLineLength),0x888);
    chk("g4_tireTemp",offsetof(CAutomobile,m_fTireTemperature),0x8A4);
    chk("g4_burnTimer",offsetof(CAutomobile,m_fBurnTimer),0x8E4);
    chk("g4_wheelEnt",offsetof(CAutomobile,m_apWheelCollisionEntity),0x8E8);
    chk("g4_wheelPos",offsetof(CAutomobile,m_vWheelCollisionPos),0x8F8);
    chk("g4_explVictim",offsetof(CAutomobile,m_pExplosionVictim),0x928);
    chk("g4_numContact",offsetof(CAutomobile,m_nNumContactWheels),0x960);
    chk("g4_wheelStates",offsetof(CAutomobile,m_WheelStates),0x968);
    chk("g4_heliRotor",offsetof(CAutomobile,m_fHeliRotorSpeed),0x84C);
    // CVehicle
    chk("g4_driver",offsetof(CAutomobile,m_pDriver),0x460);
    chk("g4_passengers",offsetof(CAutomobile,m_apPassengers),0x464);
    chk("g4_maxPassengers",offsetof(CAutomobile,m_nMaxPassengers),0x488);
    chk("g4_specialCol",offsetof(CAutomobile,m_vehicleSpecialColIndex),0x48B);
    chk("g4_steer",offsetof(CAutomobile,m_fSteerAngle),0x494);
    chk("g4_gas",offsetof(CAutomobile,m_GasPedal),0x49C);
    chk("g4_createdBy",offsetof(CAutomobile,m_nCreatedBy),0x4A4);
    chk("g4_towing",offsetof(CAutomobile,m_pTowingVehicle),0x4C4);
    chk("g4_towed",offsetof(CAutomobile,m_pVehicleBeingTowed),0x4C8);
    chk("g4_health",offsetof(CAutomobile,m_fHealth),0x4C0);
    chk("g4_lastWeapon",offsetof(CAutomobile,m_nLastWeaponDamageType),0x508);
    chk("g4_lastDamageEnt",offsetof(CAutomobile,m_pLastDamageEntity),0x50C);
    chk("g4_subtype",offsetof(CAutomobile,m_nVehicleSubType),0x594);
    chk("g4_damageMgr",offsetof(CAutomobile,m_damageManager),0x5A0);
    // CPhysical
    chk("g4_moveSpeed",offsetof(CAutomobile,m_vecMoveSpeed),0x44);
    chk("g4_turnSpeed",offsetof(CAutomobile,m_vecTurnSpeed),0x50);
    chk("g4_mass",offsetof(CAutomobile,m_fMass),0x8C);
    chk("g4_turnMass",offsetof(CAutomobile,m_fTurnMass),0x90);
    chk("g4_buoyancy",offsetof(CAutomobile,m_fBuoyancyConstant),0xA0);
    chk("g4_movingSpeed",offsetof(CAutomobile,m_fMovingSpeed),0xD4);
    chk("g4_damageIntensity",offsetof(CAutomobile,m_fDamageIntensity),0xD8);
    chk("g4_damageEntity",offsetof(CAutomobile,m_pDamageEntity),0xDC);
    chk("g4_lastImpactVel",offsetof(CAutomobile,m_vecLastCollisionImpactVelocity),0xE0);
    chk("g4_lastPosn",offsetof(CAutomobile,m_vecLastCollisionPosn),0xEC);
    chk("g4_pieceType",offsetof(CAutomobile,m_nPieceType),0xF8);
    // otras estructuras
    chk("g4_dmgLights",offsetof(CDamageManager,m_nLightsStatus),0x10);
    chk("g4_dmgPanels",offsetof(CDamageManager,m_nPanelsStatus),0x14);
    chk("g4_colModelData",offsetof(CColModel,m_pColData),0x2C);
    chk("g4_colDataTris",offsetof(CCollisionData,m_nNumTriangles),4);
    chk("g4_colDataLines",offsetof(CCollisionData,m_nNumLines),6);
    chk("g4_colDataPLines",offsetof(CCollisionData,m_pLines),0x10);
    chk("g4_colPointSurfA",offsetof(CColPoint,m_nSurfaceTypeA),0x20);
    chk("g4_colPointSurfB",offsetof(CColPoint,m_nSurfaceTypeB),0x23);
    chk("g4_colPointLightB",offsetof(CColPoint,m_nLightingB),0x25);
    chk("g4_colPointDepth",offsetof(CColPoint,m_fDepth),0x28);
    chk("g4_colPointSize",sizeof(CColPoint),0x2C);
    chk("g4_doorSize",sizeof(CDoor),0x18);
    chk("g4_doorAngle",offsetof(CDoor,m_angle),0xC);
    chk("g4_doorPrev",offsetof(CDoor,m_prevAngle),0x10);
    chk("g4_wheelSizeFront",offsetof(CVehicleModelInfo,m_fWheelSizeFront),0x40);
    chk("g4_wheelSizeRear",offsetof(CVehicleModelInfo,m_fWheelSizeRear),0x44);
    chk("g4_pedType",offsetof(CPed,m_nPedType),0x598);
    chk("g4_pedShift",offsetof(CPed,m_vecAnimMovingShiftLocal),0x4D8);
    return ok;
}

}
