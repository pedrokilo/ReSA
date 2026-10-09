#pragma once
// Banco dentro del motor (flujo viejo, estilo PadMouse1/PoolsWave4): funciones pendientes de CPickups y CWeapon (GTA SA 1.0 US).
// Original (hook de la funcion desactivado: codigo del EXE) frente a candidato (hook activo), mismos fixtures deterministas por caso,
// estado completo comparado (pickups reales guardados/restaurados, arma en un buffer del banco, entidades/peds/vehiculos falsos) y traza
// de todas las llamadas salientes con sus argumentos. Ver README.md para el detalle de dobles, dominios y lo que queda fuera.
#include "WeaponCases2.hpp"

namespace PickWeap1Live {

inline const std::array<Fn,60> fns{{
    {"CPickups","Init",0x454A70,0,-1,64},
    {"CPickups","ReInit",0x456E60,0,-1,128},
    {"CPickups","AddToCollectedPickupsArray",0x455240,0,-1,128},
    {"CPickups","GetActualPickupIndex",0x4552A0,0,-1,256},
    {"CPickups","GetNewUniquePickupIndex",0x456A30,0,-1,256},
    {"CPickups","GetUniquePickupIndex",0x455280,0,-1,128},
    {"CPickups","IsPickUpPickedUp",0x454B40,0,-1,128},
    {"CPickups","PassTime",0x455200,0,-1,128},
    {"CPickups","UpdateMoneyPerDay",0x455680,0,-1,128},
    {"CPickups","RemovePickUp",0x4573D0,0,-1,128},
    {"CPickups","RemoveMissionPickUps",0x456DE0,0,-1,64},
    {"CPickups","RemovePickUpsInArea",0x456D30,0,-1,192},
    {"CPickups","RemovePickupObjects",0x455470,0,-1,64},
    {"CPickups","RemoveUnnecessaryPickups",0x4563A0,0,-1,192},
    {"CPickups","TestForPickupsInBubble",0x456450,0,-1,192},
    {"CPickups","TryToMerge_WeaponType",0x4555A0,0,-1,256},
    {"CPickups","FindPickUpForThisObject",0x4551C0,0,-1,128},
    {"CPickups","DetonateMinesHitByGunShot",0x4590C0,0,-1,64},
    {"CPickups","ModelForWeapon",0x454AC0,0,-1,94},
    {"CPickups","WeaponForModel",0x454AE0,0,-1,256},
    {"CPickups","PlayerCanPickUpThisWeaponTypeAtThisMoment",0x4554C0,0,-1,128},
    {"CPickups","ModifyStringLabelForControlSetting",0x454B70,0,-1,128},
    {"CPickups","Load",0x5D35A0,0,-1,64},
    {"CPickups","Save",0x5D3540,0,-1,64},
    {"CPickups","DoMoneyEffects",0x454E80,0,-1,128},
    {"CPickups","DoCollectableEffects",0x455E20,0,-1,256},
    {"CPickups","DoMineEffects",0x4560E0,0,-1,256},
    {"CPickups","CreateSomeMoney",0x458970,0,-1,192},
    {"CPickups","GenerateNewOne_WeaponType",0x457380,0,-1,128},
    {"CPickups","PickedUpOyster",0x4552D0,0,-1,128},
    {"CPickups","PickedUpHorseShoe",0x455390,0,-1,128},
    {"CPickups","PictureTaken",0x456A70,0,-1,256},
    {"CPickups","RenderPickUpText",0x455000,0,-1,256},
    {"CPickups","GivePlayerGoodiesWithPickUpMI",0x4564F0,0,-1,384},
    {"CWeapon","IsTypeMelee",0x73B1C0,0,-1,94},
    {"CWeapon","IsType2Handed",0x73B1E0,0,-1,94},
    {"CWeapon","IsTypeProjectile",0x73B210,0,-1,94},
    {"CWeapon","HasWeaponAmmoToBeUsed",0x73B2A0,0,-1,128},
    {"CWeapon","CanBeUsedFor2Player-Method",0x73DEF0,0,-1,94},
    {"CWeapon","CanBeUsedFor2Player-Static",0x73B240,0,-1,128},
    {"CWeapon","Constructor",0x73B430,0,-1,256},
    {"CWeapon","Initialise",0x73B4A0,0,-1,256},
    {"CWeapon","Reload",0x73AEB0,0,-1,192},
    {"CWeapon","Shutdown",0x73A380,0,-1,128},
    {"CWeapon","StopWeaponEffect",0x73B360,0,-1,94},
    {"CWeapon","TargetWeaponRangeMultiplier",0x73B380,0,-1,256},
    {"CWeapon","InitialiseWeapons",0x73A300,0,-1,8},
    {"CWeapon","ShutdownWeapons",0x73A330,0,-1,8},
    {"CWeapon","UpdateWeapons",0x73A360,0,-1,8},
    {"CWeapon","EvaluateTargetForHeatSeekingMissile",0x73E560,0,-1,256},
    {"CWeapon","PickTargetForHeatSeekingMissile",0x73F910,0,-1,256},
    {"CWeapon","SetUpPelletCol",0x73C710,0,-1,256},
    {"CWeapon","AddGunshell",0x73A3E0,0,-1,192},
    {"CWeapon","FireFromCar",0x73FA20,0,-1,256},
    {"CWeapon","DoDoomAiming",0x73CDC0,0,-1,256},
    {"CWeapon","LaserScopeDot",0x73A8D0,0,-1,192},
    {"CWeapon","Fire",0x742300,0,-1,768},
    {"CWeapon","FireAreaEffect",0x73E800,0,-1,384},
    {"CWeapon","FireM16_1stPerson",0x741C00,0,-1,384},
    {"CWeapon","Update",0x73DB40,0,-1,512},
}};

// ---- despacho caso -> nombre del hook ----
inline json RunCase(const std::string& nm,Rng& r){
    if(nm=="Init")return CaseInit(r);
    if(nm=="ReInit")return CaseReInit(r);
    if(nm=="AddToCollectedPickupsArray")return CaseAddCollected(r);
    if(nm=="GetActualPickupIndex")return CaseActualIndex(r,0);
    if(nm=="GetUniquePickupIndex")return CaseActualIndex(r,1);
    if(nm=="GetNewUniquePickupIndex")return CaseActualIndex(r,2);
    if(nm=="IsPickUpPickedUp")return CaseIsPickedUp(r);
    if(nm=="PassTime")return CasePassTime(r);
    if(nm=="UpdateMoneyPerDay")return CaseMoneyPerDay(r);
    if(nm=="RemovePickUp")return CaseRemovePickUp(r);
    if(nm=="RemoveMissionPickUps")return CaseRemoveMission(r);
    if(nm=="RemovePickUpsInArea")return CaseRemoveInArea(r);
    if(nm=="RemovePickupObjects")return CaseRemoveObjects(r);
    if(nm=="RemoveUnnecessaryPickups")return CaseRemoveUnnecessary(r);
    if(nm=="TestForPickupsInBubble")return CaseBubble(r);
    if(nm=="TryToMerge_WeaponType")return CaseMerge(r);
    if(nm=="FindPickUpForThisObject")return CaseFindForObject(r);
    if(nm=="DetonateMinesHitByGunShot")return CaseDetonate(r);
    if(nm=="ModelForWeapon")return CaseModelForWeapon(r);
    if(nm=="WeaponForModel")return CaseWeaponForModel(r);
    if(nm=="PlayerCanPickUpThisWeaponTypeAtThisMoment")return CasePlayerCanPickUp(r);
    if(nm=="ModifyStringLabelForControlSetting")return CaseModifyLabel(r);
    if(nm=="Load")return CaseSaveLoad(r,true);
    if(nm=="Save")return CaseSaveLoad(r,false);
    if(nm=="DoMoneyEffects")return CaseEffect(r,0);
    if(nm=="DoCollectableEffects")return CaseEffect(r,1);
    if(nm=="DoMineEffects")return CaseEffect(r,2);
    if(nm=="CreateSomeMoney")return CaseCreateSomeMoney(r);
    if(nm=="GenerateNewOne_WeaponType")return CaseGenerateWeapon(r);
    if(nm=="PickedUpOyster")return CaseCollectable(r,0);
    if(nm=="PickedUpHorseShoe")return CaseCollectable(r,1);
    if(nm=="PictureTaken")return CasePictureTaken(r);
    if(nm=="RenderPickUpText")return CaseRenderText(r);
    if(nm=="GivePlayerGoodiesWithPickUpMI")return CaseGoodies(r);
    if(nm=="IsTypeMelee")return CaseWeaponQuery(r,0);
    if(nm=="IsType2Handed")return CaseWeaponQuery(r,1);
    if(nm=="IsTypeProjectile")return CaseWeaponQuery(r,2);
    if(nm=="HasWeaponAmmoToBeUsed")return CaseWeaponQuery(r,3);
    if(nm=="CanBeUsedFor2Player-Method")return CaseWeaponQuery(r,4);
    if(nm=="CanBeUsedFor2Player-Static")return CaseCanBeUsedStatic(r);
    if(nm=="Constructor")return CaseConstructor(r);
    if(nm=="Initialise")return CaseInitialise(r);
    if(nm=="Reload")return CaseReload(r);
    if(nm=="Shutdown")return CaseShutdown(r);
    if(nm=="StopWeaponEffect")return CaseStopEffect(r);
    if(nm=="TargetWeaponRangeMultiplier")return CaseRangeMultiplier(r);
    if(nm=="InitialiseWeapons")return CaseWeaponsInit(r,0);
    if(nm=="ShutdownWeapons")return CaseWeaponsInit(r,1);
    if(nm=="UpdateWeapons")return CaseWeaponsInit(r,2);
    if(nm=="EvaluateTargetForHeatSeekingMissile")return CaseEvaluate(r);
    if(nm=="PickTargetForHeatSeekingMissile")return CasePickTarget(r);
    if(nm=="SetUpPelletCol")return CasePelletCol(r);
    if(nm=="AddGunshell")return CaseGunshell(r);
    if(nm=="FireFromCar")return CaseFireFromCar(r);
    if(nm=="DoDoomAiming")return CaseDoomAiming(r);
    if(nm=="LaserScopeDot")return CaseLaserDot(r);
    if(nm=="Fire")return CaseFire(r);
    if(nm=="FireAreaEffect")return CaseFireArea(r);
    if(nm=="FireM16_1stPerson")return CaseFireM16(r);
    if(nm=="Update")return CaseUpdate(r);
    return json{{"error","unknown case"}};
}

// ---- modelos de arma: toda id de modelo de arma sin ModelInfo recibe uno falso (AddRef/RemoveRef estan doblados) ----
inline std::vector<uintptr_t> weaponMiSlots;
alignas(16) inline std::array<std::array<uint8,0x80>,128> fakeWeaponMi{};
inline void PrepareWeaponMi(){
    for(int t=0;t<47;++t){
        const auto* wi=CWeaponInfo::GetWeaponInfo(eWeaponType(t),eWeaponSkill::STD); if(!wi)continue;
        for(int32 id:{wi->m_nModelId1,wi->m_nModelId2}){
            if(id<0||id>=20000)continue; const uintptr_t slot=kModelInfoPtrs+4u*uint32(id);
            if(AtA<uint32>(slot)==0&&std::find(weaponMiSlots.begin(),weaponMiSlots.end(),slot)==weaponMiSlots.end()&&weaponMiSlots.size()<fakeWeaponMi.size()){ weaponMiSlots.push_back(slot); Snap(slot,4); } } } }
inline void ApplyWeaponMi(){ for(size_t k=0;k<weaponMiSlots.size();++k)AtA<void*>(weaponMiSlots[k])=fakeWeaponMi[k].data(); }

// ---- actualizacion principal ----
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"pickweap1-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"pickweap1-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("pickweap1");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"pickweap1-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,fns.size()> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch) ---
    D.clearBlip=Body(static_cast<void(*)(eBlipType,int32)>(&CRadar::ClearBlipForEntity));
    D.worldRemove=Body(&CWorld::Remove);
    D.processGunShot=Body(&CPickup::ProcessGunShot);
    D.getTaskJetPack=Body(&CPedIntelligence::GetTaskJetPack);
    D.saveData=Body(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::SaveDataToWorkBuffer));
    D.loadData=Body(static_cast<bool(*)(void*,int32)>(&CGenericGameStorage::LoadDataFromWorkBuffer));
    D.shadow=Body(&CShadows::StoreStaticShadow);
    D.corona=Body(static_cast<void(*)(uint32,CEntity*,uint8,uint8,uint8,uint8,const CVector&,float,float,eCoronaType,eCoronaFlareType,eCoronaReflType,eCoronaLOSCheck,eCoronaTrail,float,bool,float,bool,float,bool,bool)>(&CCoronas::RegisterCorona));
    D.getRandom=Body(&CGeneral::GetRandomNumber);
    D.groundZ=Body(&CWorld::FindGroundZFor3DCoord);
    D.audio=Body(&CAudioEngine::ReportFrontendAudioEvent);
    D.incStat=Body(&CStats::IncrementStat);
    D.getStat=Body(static_cast<float(*)(eStats)>(&CStats::GetStatValue));
    D.trigMsg=Body(&CGarages::TriggerMessage);
    D.pctProgress=Body(&CStats::GetPercentageProgress);
    D.fov=Body(&CCamera::FindCamFOV);
    D.sphereVis=Body(static_cast<bool(CCamera::*)(const CVector&,float,RwMatrix*)>(&CCamera::IsSphereVisible));
    D.updHealth=Body(&CStats::UpdateStatsAddToHealth);
    D.resetSprint=Body(&CPlayerPed::ResetSprintEnergy);
    D.getWanted=Body(&CPlayerPed::GetWantedLevel);
    D.setWanted=Body(&CPlayerPed::SetWantedLevel);
    D.fSetProp=Body(&CFont::SetProportional); D.fSetBack=Body(&CFont::SetBackground); D.fSetScale=Body(&CFont::SetScale); D.fSetOrient=Body(&CFont::SetOrientation);
    D.fSetCentre=Body(&CFont::SetCentreSize); D.fSetColor=Body(&CFont::SetColor); D.fSetStyle=Body(&CFont::SetFontStyle); D.fPrint=Body(&CFont::PrintString);
    D.insertNum=Body(&CMessages::InsertNumberInString);
    D.pickOyster=Body(&CPickups::PickedUpOyster); D.pickHorse=Body(&CPickups::PickedUpHorseShoe);
    D.addRef=Body(&CBaseModelInfo::AddRef); D.removeRef=Body(&CBaseModelInfo::RemoveRef); D.fxKill=Body(&FxSystem_c::Kill);
    D.shotInfoInit=Body(&CShotInfo::Initialise); D.shotInfoShutdown=Body(&CShotInfo::Shutdown); D.shotInfoUpdate=Body(&CShotInfo::Update);
    D.explInit=Body(&CExplosion::Initialise); D.explShutdown=Body(&CExplosion::Shutdown); D.explUpdate=Body(&CExplosion::Update);
    D.projInit=Body(&CProjectileInfo::Initialise); D.projShutdown=Body(&CProjectileInfo::Shutdown); D.projUpdate=Body(&CProjectileInfo::Update);
    D.bulletInit=Body(&CBulletInfo::Initialise); D.bulletShutdown=Body(&CBulletInfo::Shutdown); D.bulletUpdate=Body(&CBulletInfo::Update);
    D.weaponInfoInit=Body(&CWeaponInfo::Initialise); D.weaponInfoShutdown=Body(&CWeaponInfo::Shutdown); D.removeColVolumes=Body(&CColModel::RemoveCollisionVolumes);
    D.getWeaponSkill=Body(static_cast<eWeaponSkill(CPed::*)(eWeaponType)>(&CPed::GetWeaponSkill));
    D.evaluateTarget=Body(&CWeapon::EvaluateTargetForHeatSeekingMissile);
    D.getIsOnScreen=Body(&CEntity::GetIsOnScreen);
    D.addParticle=Body(static_cast<void(FxSystem_c::*)(const CVector&,const CVector&,float,const FxPrtMult_c&,float,float,float,bool)>(&FxSystem_c::AddParticle));
    D.findObjectsInRange=Body(&CWorld::FindObjectsInRange);
    D.canSeeEntity=Body(&CPed::CanSeeEntity);
    D.losWorld=Body(&CWorld::ProcessLineOfSight);
    D.addAudioEvent=Body(&CAEPedWeaponAudioEntity::AddAudioEvent);
    D.getTaskUseGun=Body(&CPedIntelligence::GetTaskUseGun);
    D.getAssoc=Body(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation));
    D.getActiveTask=Body(&CTaskManager::GetActiveTask);
    D.fireInstantHitFromCar=Body(&CWeapon::FireInstantHitFromCar);
    D.calcScreen=Body(&CSprite::CalcScreenCoors);
    D.fireInstantHit=Body(&CWeapon::FireInstantHit); D.fireProjectile=Body(&CWeapon::FireProjectile); D.fireM16=Body(&CWeapon::FireM16_1stPerson);
    D.fireSniper=Body(&CWeapon::FireSniper); D.fireArea=Body(&CWeapon::FireAreaEffect); D.takePhoto=Body(&CWeapon::TakePhotograph);
    D.useDetonator=Body(&CWorld::UseDetonator);
    D.say=Body(&CPed::Say); D.giveWeapon=Body(static_cast<eWeaponSlot(CPed::*)(eWeaponType,uint32,bool)>(&CPed::GiveWeapon)); D.getWeaponSlot=Body(static_cast<int32(CPed::*)(eWeaponType)>(&CPed::GetWeaponSlot));
    D.setCurrentWeapon=Body(static_cast<void(CPed::*)(eWeaponType)>(&CPed::SetCurrentWeapon));
    D.padClear=Body(&CPad::Clear); D.padShake=Body(&CPad::StartShake);
    D.addShot=Body(&CShotInfo::AddShot); D.doWeaponEffect=Body(&CWeapon::DoWeaponEffect);
    D.startFire=Body(&CCreepingFire::TryToStartFireAtCoors); D.reportCrime=Body(&CCrime::ReportCrime);
    D.find3rd=Body(&CCamera::Find3rdPersonCamTargetVector); D.mouseCam=Body(&CCam::Using3rdPersonMouseCam);
    D.bonePos=Body(static_cast<void(CPed::*)(CVector*,eBoneTag,bool)>(&CPed::GetBonePosition));
    D.handleGunShot=Body(&CBirds::HandleGunShot); D.oilOnFire=Body(&CShadows::GunShotSetsOilOnFire);
    D.checkOccupant=Body(&CWeapon::CheckForShootingVehicleOccupant); D.rangeMult=Body(&CWeapon::TargetWeaponRangeMultiplier); D.doBulletImpact=Body(&CWeapon::DoBulletImpact);
    {   struct Nmv { const char* n; uintptr_t v; };
        const Nmv all[]{{"clearBlip",D.clearBlip},{"worldRemove",D.worldRemove},{"processGunShot",D.processGunShot},{"getTaskJetPack",D.getTaskJetPack},{"saveData",D.saveData},{"loadData",D.loadData},
            {"shadow",D.shadow},{"corona",D.corona},{"getRandom",D.getRandom},{"groundZ",D.groundZ},{"audio",D.audio},{"incStat",D.incStat},{"getStat",D.getStat},{"trigMsg",D.trigMsg},{"pctProgress",D.pctProgress},
            {"fov",D.fov},{"sphereVis",D.sphereVis},{"updHealth",D.updHealth},{"resetSprint",D.resetSprint},{"getWanted",D.getWanted},{"setWanted",D.setWanted},{"fSetProp",D.fSetProp},{"fSetBack",D.fSetBack},
            {"fSetScale",D.fSetScale},{"fSetOrient",D.fSetOrient},{"fSetCentre",D.fSetCentre},{"fSetColor",D.fSetColor},{"fSetStyle",D.fSetStyle},{"fPrint",D.fPrint},{"insertNum",D.insertNum},{"pickOyster",D.pickOyster},
            {"pickHorse",D.pickHorse},{"addRef",D.addRef},{"removeRef",D.removeRef},{"fxKill",D.fxKill},{"shotInfoInit",D.shotInfoInit},{"shotInfoShutdown",D.shotInfoShutdown},{"shotInfoUpdate",D.shotInfoUpdate},
            {"explInit",D.explInit},{"explShutdown",D.explShutdown},{"explUpdate",D.explUpdate},{"projInit",D.projInit},{"projShutdown",D.projShutdown},{"projUpdate",D.projUpdate},{"bulletInit",D.bulletInit},
            {"bulletShutdown",D.bulletShutdown},{"bulletUpdate",D.bulletUpdate},{"weaponInfoInit",D.weaponInfoInit},{"weaponInfoShutdown",D.weaponInfoShutdown},{"removeColVolumes",D.removeColVolumes},
            {"getWeaponSkill",D.getWeaponSkill},{"evaluateTarget",D.evaluateTarget},{"getIsOnScreen",D.getIsOnScreen},{"addParticle",D.addParticle},{"findObjectsInRange",D.findObjectsInRange},{"canSeeEntity",D.canSeeEntity},
            {"losWorld",D.losWorld},{"addAudioEvent",D.addAudioEvent},{"getTaskUseGun",D.getTaskUseGun},{"getAssoc",D.getAssoc},{"getActiveTask",D.getActiveTask},{"fireInstantHitFromCar",D.fireInstantHitFromCar},
            {"calcScreen",D.calcScreen},{"fireInstantHit",D.fireInstantHit},{"fireProjectile",D.fireProjectile},{"fireM16",D.fireM16},{"fireSniper",D.fireSniper},{"fireArea",D.fireArea},{"takePhoto",D.takePhoto},
            {"useDetonator",D.useDetonator},{"say",D.say},{"giveWeapon",D.giveWeapon},{"getWeaponSlot",D.getWeaponSlot},{"setCurrentWeapon",D.setCurrentWeapon},{"padClear",D.padClear},{"padShake",D.padShake},
            {"addShot",D.addShot},{"doWeaponEffect",D.doWeaponEffect},{"startFire",D.startFire},{"reportCrime",D.reportCrime},{"find3rd",D.find3rd},{"mouseCam",D.mouseCam},{"bonePos",D.bonePos},
            {"handleGunShot",D.handleGunShot},{"oilOnFire",D.oilOnFire},{"checkOccupant",D.checkOccupant},{"rangeMult",D.rangeMult},{"doBulletImpact",D.doBulletImpact}};
        json missing=json::array(); std::set<uintptr_t> seen; json dup=json::array();
        for(auto& e:all){ if(!e.v||!InDll(e.v))missing.push_back(e.n); else if(!seen.insert(e.v).second)dup.push_back(e.n); }
        if(!missing.empty()){ Write({{"event","error"},{"reason","DLL bodies not found"},{"missing",missing},{"duplicates",dup}}); return; }
        if(!dup.empty())Write({{"event","note"},{"reason","bodies compartidos por mas de un colaborador"},{"duplicates",dup}}); }

    // --- vtables falsas ---
    objVt.fill(nullptr); objVt[0]=reinterpret_cast<void*>(&S_ObjDtor); miVt.fill(nullptr); miVt[4]=reinterpret_cast<void*>(&S_ModelType); taskVt.fill(nullptr); taskVt[4]=reinterpret_cast<void*>(&S_TaskType);
    for(auto& o:fakeObj){ std::memset(o.data(),0,o.size()); At<void*>(o.data(),0)=objVt.data(); }
    if(named.empty()){
        Nm(fakeObj[0].data(),sizeof(fakeObj),"fakeobj"); Nm(fakePed.data(),fakePed.size(),"fakeped"); NameWeaponBuffers(); }

    std::array<bool,fns.size()> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // disposicion de campos de la DLL frente a los desplazamientos del EXE
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      alignas(16) static uint8 pk[0x40]{}; auto* pp=reinterpret_cast<CPickup*>(pk);
      auto Off=[](const void* base,const void* m){ return uint32(reinterpret_cast<const uint8*>(m)-reinterpret_cast<const uint8*>(base)); };
      Chk("sizeof(CPickup) (0x4552B2 shl edx,5)",uint32(sizeof(CPickup)),0x20);
      Chk("CPickup.m_pObject (0x4551CF cmp [ecx],edx con ecx=0x9788C4)",Off(pp,&pp->m_pObject),4);
      Chk("CPickup.m_nAmmo (0x45566F)",Off(pp,&pp->m_nAmmo),8);
      Chk("CPickup.m_nRegenerationTime (0x45521C)",Off(pp,&pp->m_nRegenerationTime),0xC);
      Chk("CPickup.m_vecPos (0x4555CF movsx [ecx-2])",Off(pp,&pp->m_vecPos),0x10);
      Chk("CPickup.m_nMoneyPerDay (0x4556AB)",Off(pp,&pp->m_nMoneyPerDay),0x16);
      Chk("CPickup.m_nModelIndex (0x4555C7 movzx [ecx+6])",Off(pp,&pp->m_nModelIndex),0x18);
      Chk("CPickup.m_nReferenceIndex (0x455249 [ecx+9788DAh])",Off(pp,&pp->m_nReferenceIndex),0x1A);
      Chk("CPickup.m_nPickupType (0x4551CF [ecx+18h] con base 0x9788C4)",Off(pp,&pp->m_nPickupType),0x1C);
      Chk("sizeof(tPickupMessage) (0x45509B add esi,24h)",uint32(sizeof(tPickupMessage)),0x24);
      alignas(16) static uint8 wp[0x20]{}; auto* ww=reinterpret_cast<CWeapon*>(wp);
      Chk("CWeapon.m_State",Off(ww,&ww->m_State),4); Chk("CWeapon.m_AmmoInClip",Off(ww,&ww->m_AmmoInClip),8); Chk("CWeapon.m_TotalAmmo",Off(ww,&ww->m_TotalAmmo),0xC);
      Chk("CWeapon.m_TimeForNextShotMs",Off(ww,&ww->m_TimeForNextShotMs),0x10); Chk("CWeapon.m_IsFirstPersonWeaponModeSelected",Off(ww,&ww->m_IsFirstPersonWeaponModeSelected),0x14);
      Chk("CWeapon.m_DontPlaceInHand",Off(ww,&ww->m_DontPlaceInHand),0x15); Chk("CWeapon.m_FxSystem",Off(ww,&ww->m_FxSystem),0x18);
      alignas(16) static uint8 wi[0x80]{}; auto* wi0=reinterpret_cast<CWeaponInfo*>(wi);
      Chk("CWeaponInfo.m_fWeaponRange (0x73A8E8 mov ecx,[eax+8])",Off(wi0,&wi0->m_fWeaponRange),8); Chk("CWeaponInfo.m_nModelId1 (0x73A38D)",Off(wi0,&wi0->m_nModelId1),0xC);
      Chk("CWeaponInfo.m_nModelId2 (0x73A3AE)",Off(wi0,&wi0->m_nModelId2),0x10); Chk("CWeaponInfo.m_nAmmoClip (0x73AEDA movsx [eax+20h])",Off(wi0,&wi0->m_nAmmoClip),0x20);
      Chk("CWeaponInfo.m_fAnimLoopStart (0x7429CB)",Off(wi0,&wi0->m_fAnimLoopStart),0x40); Chk("CWeaponInfo.m_fAnimLoopEnd (0x7429CB)",Off(wi0,&wi0->m_fAnimLoopEnd),0x44);
      Chk("CPlaceable.m_matrix (0x455E28 [esi+14h])",uint32(offsetof(CPlaceable,m_matrix)),0x14); Chk("CPlaceable.m_placement (0x455E3A lea eax,[esi+4])",uint32(offsetof(CPlaceable,m_placement)),4);
      Chk("&CWorld::Players (0x45651A)",uint32(reinterpret_cast<uintptr_t>(&CWorld::Players[0])),0xB7CD98);
      Chk("&CPickups::aPickUps (0x454A79)",uint32(reinterpret_cast<uintptr_t>(CPickups::aPickUps.data())),0x9788C0);
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty()){ SnapPickupRegions(); SnapWeaponRegions(); PrepareWeaponMi(); }
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2B7Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); gRng=&r; ApplyWeaponMi();
                hooks[f]->State(candidate);
                const bool switches= ReversingProbe::JumpsToDll(fn.address)==candidate;   // despues de State(), no antes
                json out={{"switches",switches}};
                _controlfp(_PC_24,_MCW_PC);                  // x87 a 24 bits como el juego
                out["state"]=RunCase(nm,r);
                const unsigned fp=_controlfp(0,0); _controlfp(savedFP,_MCW_PC|_MCW_RC);
                out["fp"]=fp; out["trace"]=trace;
                RestoreRegions(); gRng=nullptr;
                return out;
            };
            const auto original=run(false), candidate=run(true);
            bool ok=original==candidate&&original["switches"]==true&&candidate["switches"]==true;
            passed&=ok; failures+=!ok;
            auto Summary=[](const json& r)->json{ const auto s=r.dump(); return std::format("{:016x}",Fnv(s.data(),s.size())); };
            if(ok)Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",true},{"accepted",true},{"original",Summary(original)},{"candidate",Summary(candidate)},{"events",original["trace"].size()}});
            else { json diff=json::array(); for(auto& [k,v]:original.items())if(candidate[k]!=v)diff.push_back(k);
                json sd=json::array(); if(original.contains("state")&&candidate.contains("state")&&original["state"].is_object())for(auto& [k,v]:original["state"].items())if(candidate["state"][k]!=v)sd.push_back(k);
                Write({{"event","comparison"},{"pair",pair++},{"function",full},{"case",i},{"equal",false},{"accepted",false},{"differs",diff},{"state_differs",sd},
                       {"original",failures<=2 ? original:json(nullptr)},{"candidate",failures<=2 ? candidate:json(nullptr)}}); }
        }
        Write({{"event","function_summary"},{"function",full},{"failures",failures},{"cases",fn.cases}});
    }
    for(size_t f=0;f<fns.size();++f){hooks[f]->State(enabled[f]);hooks[f]->LockState(locked[f]);}
    RestoreRegions();
    bool restored=RegionsRestored()&&_controlfp(0,0)==savedFP;
    for(size_t f=0;f<fns.size();++f)restored&=hooks[f]->Hooked()==enabled[f]&&hooks[f]->Locked()==locked[f];
    Write({{"event",passed ? "complete":"error"},{"pairs",pair},{"layout_ok",layoutOk},{"state_restored",restored}});
}
}
