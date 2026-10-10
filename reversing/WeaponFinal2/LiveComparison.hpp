#pragma once
// Banco dentro del motor (estilo PickWeap1/PadMouse1): nueve funciones de CWeapon (GTA SA 1.0 US) que PickWeap1 dejo fuera.
// Original (hook de la funcion desactivado: codigo del EXE) frente a candidato (hook activo), mismos fixtures deterministas por caso,
// estado comparado (arma en un buffer del banco, entidades/peds/vehiculos/objetos falsos, pools de peds y objetos falsos intercambiados
// temporalmente, lista de referencias falsa) y traza de todas las llamadas salientes con sus argumentos. Ver README.md.
#include "Cases3.hpp"

namespace WeaponFinal2Live {

inline const std::array<Fn,9> fns{{
    {"CWeapon","ProcessLineOfSight",0x73B300,0,-1,256},
    {"CWeapon","DoWeaponEffect",0x73E690,0,-1,512},
    {"CWeapon","FireOneInstantHitRound",0x73AF00,0,-1,768},
    {"CWeapon","GenerateDamageEvent",0x73A530,0,-1,1024},
    {"CWeapon","DoBulletImpact",0x73B550,0,-1,2048},
    {"CWeapon","TakePhotograph",0x73C1F0,0,-1,640},
    {"CWeapon","FireInstantHitFromCar2",0x73CBA0,0,-1,640},
    {"CWeapon","FireSniper",0x73AAC0,0,-1,640},
    {"CWeapon","FireProjectile",0x741360,0,-1,1280},
}};

inline json RunCase(const std::string& nm,Rng& r){
    if(nm=="ProcessLineOfSight")return CaseProcLos(r);
    if(nm=="DoWeaponEffect")return CaseDoWeaponEffect(r);
    if(nm=="FireOneInstantHitRound")return CaseFireOne(r);
    if(nm=="GenerateDamageEvent")return CaseGenDamage(r);
    if(nm=="DoBulletImpact")return CaseDoBulletImpact(r);
    if(nm=="TakePhotograph")return CaseTakePhoto(r);
    if(nm=="FireInstantHitFromCar2")return CaseFireHit2(r);
    if(nm=="FireSniper")return CaseFireSniper(r);
    if(nm=="FireProjectile")return CaseFireProj(r);
    return json{{"error","unknown case"}};
}

// cuerpo de la DLL de un virtual: la ranura de la vtable del EXE contiene la funcion de la DLL cuando el hook VMT esta activo
inline uintptr_t VtBody(uintptr_t vt,int slot){ return FollowIn(*reinterpret_cast<const uintptr_t*>(vt+4u*uint32(slot))); }

inline void SnapRegions(){
    Snap(0xB6F190,0x700); Snap(kActiveCamIdx,1); Snap(kTimeMs,4); Snap(kPrevTimeMs,4); Snap(kPlayers,0x190*2); Snap(kPlayerInFocus,1);
    Snap(kSpreadRate,4); Snap(kIgnoreEntity,4); Snap(kIncludeBikers,3); Snap(kFlags73A,2); Snap(kTheCamera,0x20); Snap(0xB6F990,0x80);
    Snap(kRsMaxW,8); Snap(kProcLevel,4); Snap(kPedPoolPtr,4); Snap(kObjPoolPtr,4); Snap(kMoonSize,4); Snap(kCrossHair,0x58);
    Snap(reinterpret_cast<uintptr_t>(&CReferences::pEmptyList),4); }

// ---- actualizacion principal ----
inline void Update() {
    if(done)return;
    char exe[MAX_PATH]{}; GetModuleFileNameA(nullptr,exe,MAX_PATH); const auto dir=std::filesystem::path(exe).parent_path();
    if(!std::filesystem::exists(dir/"weaponfinal2-runtime.start")){done=true;return;}
    if(!log.is_open()){log.open(dir/"weaponfinal2-runtime.jsonl");Write({{"event","waiting_for_player"}});ReversingProbe::InstallFailureCapture("weaponfinal2");}
    auto* player=FindPlayerPed(); if(!player || CCutsceneMgr::IsRunning())return; done=true; owner=GetCurrentThreadId();
    InitModule();
    std::string only; { std::ifstream f(dir/"weaponfinal2-only.txt"); if(f)std::getline(f,only); }
    std::array<ReversibleHooks::ReversibleHook::Base*,fns.size()> hooks{};
    for(size_t f=0;f<fns.size();++f)if(!(hooks[f]=FindHook(fns[f].cls,fns[f].name))){Write({{"event","error"},{"reason","hook not found"},{"function",std::string(fns[f].cls)+"::"+fns[f].name}});return;}

    // --- cuerpos de la DLL de los colaboradores (se calculan TODOS antes de crear ningun EntryPatch, con los hooks en su estado inicial) ---
    D.los=Body(&CWorld::ProcessLineOfSight); D.handleGun=Body(&CBirds::HandleGunShot); D.oil=Body(&CShadows::GunShotSetsOilOnFire); D.reportCrime=Body(&CCrime::ReportCrime);
    D.incStat=Body(&CStats::IncrementStat); D.getStat=Body(static_cast<float(*)(eStats)>(&CStats::GetStatValue)); D.addLight=Body(&CPointLights::AddLight);
    D.addTrace1=Body(static_cast<void(*)(const CVector&,const CVector&,float,uint32,uint8)>(&CBulletTraces::AddTrace));
    D.addTrace2=Body(static_cast<void(*)(const CVector&,const CVector&,eWeaponType,CEntity*)>(&CBulletTraces::AddTrace));
    D.localDir=Body(&CPed::GetLocalDirection);
    D.addAnim=Body(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId)>(&CAnimManager::AddAnimation));
    D.blendAnim=Body(static_cast<CAnimBlendAssociation*(*)(RpClump*,AssocGroupId,AnimationId,float)>(&CAnimManager::BlendAnimation));
    D.firstAssoc=Body(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetFirstAssociation));
    D.getAssoc=Body(static_cast<CAnimBlendAssociation*(*)(RpClump*,uint32)>(&RpAnimBlendClumpGetAssociation));
    D.setTime=Body(&CAnimBlendAssociation::SetCurrentTime);
    D.genDamage=Body(&CWeapon::GenerateDamageEvent); D.inflict=Body(&CVehicle::InflictDamage); D.bulletHit=Body(&CAudioEngine::ReportBulletHit);
    D.waterLevel=Body(static_cast<bool(*)(float,float,float,float&,uint8,CVector*)>(&CWaterLevel::GetWaterLevel));
    D.glass=Body(&CGlass::WasGlassHitByBullet); D.addToMoving=Body(&CPhysical::AddToMovingList); D.applyForce=Body(&CPhysical::ApplyForce); D.objDamage=Body(&CObject::ObjectDamage);
    D.sphere2=Body(static_cast<bool(CCamera::*)(const CVector&,float)>(&CCamera::IsSphereVisible));
    D.impactFx=Body(&Fx_c::AddBulletImpact); D.bloodFx=Body(&Fx_c::AddBlood); D.tyreFx=Body(&Fx_c::AddTyreBurst); D.lighting=Body(&tColLighting::GetCurrentLighting);
    D.updHit=Body(&CStats::UpdateStatsWhenWeaponHit); D.sameGroup=Body(&CPedGroups::AreInSameGroup);
    D.skillCur=Body(static_cast<eWeaponSkill(CPed::*)()>(&CPed::GetWeaponSkill)); D.shake=Body(&CPad::StartShake_Distance);
    D.calcResp=Body(&CPedDamageResponseCalculator::ComputeDamageResponse); D.affects=VtBody(0x85AD78,5);
    D.computeDeath=Body(&CEventDamage::ComputeDeathAnim); D.computeDamage=Body(&CEventDamage::ComputeDamageAnim);
    D.locBlood=Body(&CLocalisation::Blood); D.locKick=Body(&CLocalisation::KickingWhenDown);
    D.simplest=Body(&CTaskManager::GetSimplestActiveTask); D.active=Body(&CTaskManager::GetActiveTask);
    D.groupAdd=Body(static_cast<CEvent*(CEventGroup::*)(CEvent*,bool)>(&CEventGroup::Add)); D.globalGroup=Body(&GetEventGlobalGroup);
    D.interesting=Body(&CInterestingEvents::Add); D.addBullet=Body(&CBulletInfo::AddBullet); D.camShake=Body(&CamShakeNoPos);
    D.doBulletImpact=Body(&CWeapon::DoBulletImpact); D.isLocked=Body(&CWeaponEffects::IsLockedOn); D.pickTarget=Body(&CWeapon::PickTargetForHeatSeekingMissile);
    D.losClear=Body(&CWorld::GetIsLineOfSightClear); D.testSphere=Body(&CWorld::TestSphereAgainstWorld);
    D.addProj=Body(&CProjectileInfo::AddProjectile); D.removeNotAdd=Body(&CProjectileInfo::RemoveNotAdd);
    D.createFx=Body(static_cast<FxSystem_c*(FxManager_c::*)(const char*,const CVector&,RwMatrix*,bool)>(&FxManager_c::CreateFxSystem));
    D.playKill=Body(&FxSystem_c::PlayAndKill); D.pictureTaken=Body(&CPickups::PictureTaken);
    D.bonePos=Body(static_cast<CVector(CPed::*)(eBoneTag,bool)>(&CPed::GetBonePosition)); D.calcScreen=Body(&CSprite::CalcScreenCoors);
    D.createMat=Body(&Fx_c::CreateMatFromVec); D.matCreate=Body(&RwMatrixCreate); D.matDestroy=Body(&RwMatrixDestroy);
    D.fxCopy=Body(&FxSystem_c::CopyParentMatrix); D.fxPlay=Body(&FxSystem_c::Play); D.fxMust=Body(&FxSystem_c::SetMustCreatePrts);
    D.fxConst=Body(&FxSystem_c::SetConstTime); D.fxSetMat=Body(&FxSystem_c::SetMatrix); D.fxKill=Body(&FxSystem_c::Kill);
    {   struct Nmv { const char* n; uintptr_t v; };
        const Nmv all[]{{"los",D.los},{"handleGun",D.handleGun},{"oil",D.oil},{"reportCrime",D.reportCrime},{"incStat",D.incStat},{"getStat",D.getStat},{"addLight",D.addLight},{"addTrace1",D.addTrace1},{"addTrace2",D.addTrace2},
            {"localDir",D.localDir},{"addAnim",D.addAnim},{"blendAnim",D.blendAnim},{"firstAssoc",D.firstAssoc},{"getAssoc",D.getAssoc},{"setTime",D.setTime},{"genDamage",D.genDamage},{"inflict",D.inflict},{"bulletHit",D.bulletHit},
            {"waterLevel",D.waterLevel},{"glass",D.glass},{"addToMoving",D.addToMoving},{"applyForce",D.applyForce},{"objDamage",D.objDamage},{"sphere2",D.sphere2},{"impactFx",D.impactFx},{"bloodFx",D.bloodFx},{"tyreFx",D.tyreFx},
            {"lighting",D.lighting},{"updHit",D.updHit},{"sameGroup",D.sameGroup},{"skillCur",D.skillCur},{"shake",D.shake},{"calcResp",D.calcResp},{"affects",D.affects},{"computeDeath",D.computeDeath},{"computeDamage",D.computeDamage},
            {"locBlood",D.locBlood},{"locKick",D.locKick},{"simplest",D.simplest},{"active",D.active},{"groupAdd",D.groupAdd},{"globalGroup",D.globalGroup},{"interesting",D.interesting},{"addBullet",D.addBullet},{"camShake",D.camShake},
            {"doBulletImpact",D.doBulletImpact},{"isLocked",D.isLocked},{"pickTarget",D.pickTarget},{"losClear",D.losClear},{"testSphere",D.testSphere},{"addProj",D.addProj},{"removeNotAdd",D.removeNotAdd},{"createFx",D.createFx},
            {"playKill",D.playKill},{"pictureTaken",D.pictureTaken},{"bonePos",D.bonePos},{"calcScreen",D.calcScreen},{"createMat",D.createMat},{"matCreate",D.matCreate},{"matDestroy",D.matDestroy},{"fxCopy",D.fxCopy},{"fxPlay",D.fxPlay},
            {"fxMust",D.fxMust},{"fxConst",D.fxConst},{"fxSetMat",D.fxSetMat},{"fxKill",D.fxKill}};
        json missing=json::array(); std::set<uintptr_t> seen; json dup=json::array();
        for(auto& e:all){ if(!e.v||!InDll(e.v))missing.push_back(e.n); else if(!seen.insert(e.v).second)dup.push_back(e.n); }
        if(!missing.empty()){ Write({{"event","error"},{"reason","DLL bodies not found"},{"missing",missing},{"duplicates",dup}}); return; }
        if(!dup.empty())Write({{"event","note"},{"reason","bodies compartidos por mas de un colaborador"},{"duplicates",dup}}); }

    InitVtables();
    if(named.empty()){ NameBuffers(); Nm(fPedStore.data(),fPedStore.size(),"pedstore"); Nm(fObjStore.data(),fObjStore.size(),"objstore"); Nm(fCpIn.data(),fCpIn.size(),"cpin"); }

    std::array<bool,fns.size()> enabled{},locked{};
    for(size_t f=0;f<fns.size();++f){enabled[f]=hooks[f]->Hooked();locked[f]=hooks[f]->Locked();hooks[f]->LockState(false);}

    // disposicion de campos de la DLL frente a los desplazamientos del EXE y despacho virtual
    bool layoutOk=true;
    { json lay=json::object();
      auto Chk=[&](const char* n,uint32 got,uint32 want){ lay[n]=got; if(got!=want){ layoutOk=false; lay[std::string(n)+"_expected"]=want; } };
      auto Off=[](const void* base,const void* m){ return uint32(reinterpret_cast<const uint8*>(m)-reinterpret_cast<const uint8*>(base)); };
      alignas(16) static uint8 eb[kEntSize]{};
      auto Clear=[&]{ std::memset(eb,0,sizeof(eb)); };
      // --- armas ---
      { Clear(); auto* w=reinterpret_cast<CWeapon*>(eb);
        Chk("CWeapon.m_State",Off(w,&w->m_State),4); Chk("CWeapon.m_AmmoInClip",Off(w,&w->m_AmmoInClip),8); Chk("CWeapon.m_TotalAmmo",Off(w,&w->m_TotalAmmo),0xC);
        Chk("CWeapon.m_TimeForNextShotMs",Off(w,&w->m_TimeForNextShotMs),0x10); Chk("CWeapon.m_FxSystem (0x73E716 [esi+18h])",Off(w,&w->m_FxSystem),0x18);
        auto* wi=reinterpret_cast<CWeaponInfo*>(eb); Chk("CWeaponInfo.m_nDamage (0x73B999 movsx word [edx+22h])",Off(wi,&wi->m_nDamage),0x22); }
      // --- entidades ---
      { Clear(); auto* e=reinterpret_cast<CEntity*>(eb);
        Chk("CPlaceable.m_matrix (0x73B2C5 [esi+14h])",uint32(offsetof(CPlaceable,m_matrix)),0x14); Chk("CPlaceable.m_placement (lea eax,[esi+4])",uint32(offsetof(CPlaceable,m_placement)),4);
        Chk("CEntity.m_nModelIndex (0x73C062 movsx [esi+22h])",Off(e,&e->m_nModelIndex),0x22);
        Chk("CEntity.m_bIsStatic (0x4633E0)",[&]{ Clear(); e->m_bIsStatic=true; return At<uint32>(eb,0x1C); }(),4);
        Chk("tipo/estado de CEntity (byte 0x36)",[&]{ Clear(); At<uint8>(eb,0x36)=uint8((5<<3)|3); return uint32(e->GetType())*16u+uint32(e->GetStatus()); }(),3u*16u+5u); }
      { Clear(); auto* p=reinterpret_cast<CPed*>(eb);
        Chk("CPed.m_nPedType (0x73B876 [ebx+598h])",Off(p,&p->m_nPedType),0x598); Chk("CPed.m_fHealth (0x73A5B1 [esi+540h])",Off(p,&p->m_fHealth),0x540);
        Chk("CPed.m_pTargetedObject (0x73B746 [ebx+71Ch])",Off(p,&p->m_pTargetedObject),0x71C); Chk("CPed.m_fContactSurfaceBrightness (0x73BAC9 [esi+12Ch])",Off(p,&p->m_fContactSurfaceBrightness),0x12C);
        Chk("CPed.m_roadRageWith (0x73ACFA [esi+460h])",Off(p,&p->m_roadRageWith),0x460); Chk("CPed.m_pVehicle",Off(p,&p->m_pVehicle),0x58C);
        Chk("CPed.bInVehicle (0x73A57A [esi+46Ch] bit 8)",[&]{ Clear(); p->bInVehicle=true; return At<uint32>(eb,0x46C); }(),0x100);
        Chk("CPed.bNoCriticalHits (0x73BA0A [esi+470h] bit 12)",[&]{ Clear(); p->bNoCriticalHits=true; return At<uint32>(eb,0x470); }(),0x1000);
        Chk("CPed.bHasBeenPhotographed (0x73C4D2 [esi+470h] bit 18)",[&]{ Clear(); p->bHasBeenPhotographed=true; return At<uint32>(eb,0x470); }(),0x40000);
        Chk("CPed.GetIntelligence (0x73A5E7 [esi+47Ch])",[&]{ Clear(); At<void*>(eb,0x47C)=fIntel[0].data(); return U(p->GetIntelligence()); }(),U(fIntel[0].data()));
        Chk("CPed.GetPlayerData (0x7416DC [esi+480h])",[&]{ Clear(); At<void*>(eb,0x480)=fPData[0].data(); return U(p->GetPlayerData()); }(),U(fPData[0].data()));
        Chk("CPed.GetPedState (0x73B02B [ecx+530h])",[&]{ Clear(); At<int32>(eb,0x530)=54; return uint32(p->GetPedState()); }(),54);
        Chk("CPed.GetRpClump (0x73A615 [esi+18h])",[&]{ Clear(); At<void*>(eb,0x18)=fClump.data(); return U(p->GetRpClump()); }(),U(fClump.data()));
        Chk("CPed.GetEventGroup (lea ecx,[esi+68h])",[&]{ Clear(); At<void*>(eb,0x47C)=fIntel[0].data(); return U(&p->GetEventGroup()); }(),U(fIntel[0].data()+0x68));
        Chk("CPed.GetTaskManager (lea ecx,[eax+4])",[&]{ Clear(); At<void*>(eb,0x47C)=fIntel[0].data(); return U(&p->GetTaskManager()); }(),U(fIntel[0].data()+4)); }
      { Clear(); auto* ph=reinterpret_cast<CPhysical*>(eb);
        Chk("CPhysical.m_fMass (0x73BF72 [esi+8Ch])",Off(ph,&ph->m_fMass),0x8C);
        Chk("physicalFlags.bDisableCollisionForce (0x73BC09 test [esi+40h],4)",[&]{ Clear(); ph->physicalFlags.bDisableCollisionForce=true; return At<uint32>(eb,0x40); }(),4);
        Chk("physicalFlags.bDisableZ (bit 7; 0x73BC67 test [esi+40h],0A0h agrupa ambos)",[&]{ Clear(); ph->physicalFlags.bDisableZ=true; return At<uint32>(eb,0x40); }(),0x80);
        Chk("physicalFlags.bDisableMoveForce (bit 5; 0x73BC67 test [esi+40h],0A0h agrupa ambos)",[&]{ Clear(); ph->physicalFlags.bDisableMoveForce=true; return At<uint32>(eb,0x40); }(),0x20);
        Chk("physicalFlags.bBulletProof (0x73B7E0 test [ebp+40h],40000h)",[&]{ Clear(); ph->physicalFlags.bBulletProof=true; return At<uint32>(eb,0x40); }(),0x40000); }
      { Clear(); auto* v=reinterpret_cast<CVehicle*>(eb);
        Chk("CVehicle.m_fHealth (0x73B777 [ebp+4C0h])",Off(v,&v->m_fHealth),0x4C0); Chk("CVehicle.m_pDriver (0x73BD7E [esi+460h])",Off(v,&v->m_pDriver),0x460);
        Chk("CVehicle.m_apPassengers (0x73BDEA lea ebp,[esi+464h])",Off(v,&v->m_apPassengers),0x464); Chk("CVehicle.m_nMaxPassengers (0x73BDD5 [esi+488h], el bucle de GetPassengers)",Off(v,&v->m_nMaxPassengers),0x488);
        Chk("vehicleFlags.bCanBeDamaged (0x73B7D7 test byte [ebp+42Ah],20h)",[&]{ Clear(); v->vehicleFlags.bCanBeDamaged=true; return uint32(At<uint8>(eb,0x42A)); }(),0x20); }
      { Clear(); auto* o=reinterpret_cast<CObject*>(eb);
        Chk("CObject.m_nColDamageEffect (0x73BB8B cmp byte [esi+144h],0C8h)",Off(o,&o->m_nColDamageEffect),0x144); Chk("CObject.m_fHealth (0x73B797 [ebp+154h])",Off(o,&o->m_fHealth),0x154);
        Chk("CObject.m_pObjectInfo (0x73B7B4 [ebp+160h])",Off(o,&o->m_pObjectInfo),0x160);
        Chk("CObject.objectFlags.bIsPhotographed (0x73C6E2 or [esi+140h],1000h)",[&]{ Clear(); o->objectFlags.bIsPhotographed=true; return At<uint32>(eb,0x140); }(),0x1000);
        auto* oi=reinterpret_cast<CObjectData*>(eb);
        Chk("CObjectData.m_fUprootLimit (0x73BC4C [ebp+14h])",Off(oi,&oi->m_fUprootLimit),0x14); Chk("CObjectData.m_fColDamageMultiplier (0x73BC19 [ebp+18h])",Off(oi,&oi->m_fColDamageMultiplier),0x18);
        Chk("CObjectData.m_fSmashMultiplier (0x73BBC9 [eax+34h])",Off(oi,&oi->m_fSmashMultiplier),0x34); Chk("CObjectData.m_nGunBreakMode (0x73BB9A [eax+48h])",Off(oi,&oi->m_nGunBreakMode),0x48); }
      { Clear(); auto* pd=reinterpret_cast<CPlayerPedData*>(eb);
        Chk("CPlayerPedData.m_nFireHSMissilePressedTime (0x7416FA [ebp+94h])",Off(pd,&pd->m_nFireHSMissilePressedTime),0x94); Chk("CPlayerPedData.m_LastHSMissileTarget (0x741718 [ebp+98h])",Off(pd,&pd->m_LastHSMissileTarget),0x98);
        Chk("CPlayerPedData.m_nModelIndexOfLastBuildingShot (0x73C066 [eax+9Ch])",Off(pd,&pd->m_nModelIndexOfLastBuildingShot),0x9C); }
      { Clear(); auto* cp=reinterpret_cast<CColPoint*>(eb);
        Chk("CColPoint.m_vecNormal (0x73B8F6 [edi+10h])",Off(cp,&cp->m_vecNormal),0x10); Chk("CColPoint.m_nSurfaceTypeB (0x73B92F [edi+23h])",Off(cp,&cp->m_nSurfaceTypeB),0x23);
        Chk("CColPoint.m_nPieceTypeB (0x73BA30 [edi+24h])",Off(cp,&cp->m_nPieceTypeB),0x24); Chk("CColPoint.m_nLightingB (0x73BB5A [edi+25h])",Off(cp,&cp->m_nLightingB),0x25); }
      { Clear(); auto* a=reinterpret_cast<CAnimBlendAssociation*>(eb);
        Chk("CAnimBlendAssociation.m_BlendAmount (0x73B055 [eax+18h])",Off(a,&a->m_BlendAmount),0x18); Chk("CAnimBlendAssociation.m_BlendDelta (0x73B05C [eax+1Ch])",Off(a,&a->m_BlendDelta),0x1C);
        Chk("CAnimBlendAssociation.m_Speed (0x73A7A1 [eax+24h])",Off(a,&a->m_Speed),0x24); Chk("CAnimBlendAssociation.m_Flags (0x73A66A [esi+2Eh])",Off(a,&a->m_Flags),0x2E); }
      // --- eventos ---
      { Clear(); auto* e=reinterpret_cast<CEventDamage*>(eb);
        Chk("CEvent.m_nTimeActive",Off(e,&e->m_nTimeActive),4); Chk("CEvent.m_bValid",Off(e,&e->m_bValid),8); Chk("CEventEditableResponse.m_bAddToEventGroup",Off(e,&e->m_bAddToEventGroup),0xC);
        Chk("CEventEditableResponse.m_TaskId",Off(e,&e->m_TaskId),0xE); Chk("CEventEditableResponse.m_FacialExpressionType",Off(e,&e->m_FacialExpressionType),0x10);
        Chk("CEventDamage.m_pSourceEntity (0x4AD97D [esi+14h])",Off(e,&e->m_pSourceEntity),0x14); Chk("CEventDamage.m_nStartTime",Off(e,&e->m_nStartTime),0x18); Chk("CEventDamage.m_weaponType",Off(e,&e->m_weaponType),0x1C);
        Chk("CEventDamage.m_pedPieceType",Off(e,&e->m_pedPieceType),0x20); Chk("CEventDamage.m_ucDirection",Off(e,&e->m_ucDirection),0x24);
        Chk("CEventDamage.m_nAnimGroup (0x73A779 [esp+50h])",Off(e,&e->m_nAnimGroup),0x28); Chk("CEventDamage.m_nAnimID (0x73A75A [esp+54h])",Off(e,&e->m_nAnimID),0x2C);
        Chk("CEventDamage.m_fAnimBlend (0x73A792 [esp+58h])",Off(e,&e->m_fAnimBlend),0x30); Chk("CEventDamage.m_fAnimSpeed (0x73A799 [esp+5Ch])",Off(e,&e->m_fAnimSpeed),0x34);
        Chk("CEventDamage.m_damageResponse (0x73A687 lea [esp+64h])",Off(e,&e->m_damageResponse),0x38);
        Chk("CEventDamage.m_bAnimAdded (0x73A7A9 or byte [esp+4Dh],4)",[&]{ Clear(); e->m_bAnimAdded=true; return uint32(At<uint8>(eb,0x25)); }(),4);
        Chk("CEventDamage.m_bStealthMode (0x73A838 and bl,10h)",[&]{ Clear(); e->m_bStealthMode=true; return uint32(At<uint8>(eb,0x25)); }(),0x10);
        auto* r0=&e->m_damageResponse; Chk("CPedDamageResponse.m_bHealthZero",Off(r0,&r0->m_bHealthZero),8); Chk("CPedDamageResponse.m_bCheckIfAffectsPed",Off(r0,&r0->m_bCheckIfAffectsPed),0xB);
        auto* g=reinterpret_cast<CEventGunShot*>(eb); Chk("CEventGunShot.m_firedBy (0x4AC73D [esi+14h])",Off(g,&g->m_firedBy),0x14); Chk("CEventGunShot.m_startPoint",Off(g,&g->m_startPoint),0x18);
        Chk("CEventGunShot.m_endPoint",Off(g,&g->m_endPoint),0x24); Chk("CEventGunShot.m_bHasNoSound",Off(g,&g->m_bHasNoSound),0x30);
        auto* vd=reinterpret_cast<CEventVehicleDamage*>(eb); Chk("CEventVehicleDamage.m_vehicle",Off(vd,&vd->m_vehicle),0x14); Chk("CEventVehicleDamage.m_attacker",Off(vd,&vd->m_attacker),0x18);
        Chk("CEventVehicleDamage.m_weaponType",Off(vd,&vd->m_weaponType),0x1C);
        auto* c=reinterpret_cast<CPedDamageResponseCalculator*>(eb); Chk("CPedDamageResponseCalculator.m_pDamager",Off(c,&c->m_pDamager),0); Chk("CPedDamageResponseCalculator.m_fDamageFactor",Off(c,&c->m_fDamageFactor),4);
        Chk("CPedDamageResponseCalculator.m_bodyPart",Off(c,&c->m_bodyPart),8); Chk("CPedDamageResponseCalculator.m_weaponType",Off(c,&c->m_weaponType),0xC); Chk("CPedDamageResponseCalculator.m_bSpeak",Off(c,&c->m_bSpeak),0x10);
        Chk("sizeof(CEventDamage)",uint32(sizeof(CEventDamage)),0x44); Chk("sizeof(CEventGunShot)",uint32(sizeof(CEventGunShot)),0x34); Chk("sizeof(CEventVehicleDamageWeapon)",uint32(sizeof(CEventVehicleDamageWeapon)),0x20); }
      // --- pools y globales ---
      Chk("sizeof(CCopPed) (zancada del pool de peds 0x73C27B imul 7C4h)",uint32(sizeof(CCopPed)),kPedStride); Chk("sizeof(CCutsceneObject) (zancada del pool de objetos 0x73C4FB imul 19Ch)",uint32(sizeof(CCutsceneObject)),kObjStride);
      Chk("&CWorld::Players",uint32(reinterpret_cast<uintptr_t>(&CWorld::Players[0])),uint32(kPlayers));
      Chk("&CCoronas::MoonSize",uint32(reinterpret_cast<uintptr_t>(&CCoronas::MoonSize)),uint32(kMoonSize));
      Chk("&TheCamera",uint32(reinterpret_cast<uintptr_t>(&TheCamera)),uint32(kTheCamera));
      Chk("&TheCamera.m_mCameraMatrix forward",uint32(reinterpret_cast<uintptr_t>(&TheCamera.m_mCameraMatrix.GetForward())),uint32(kCamMatFwd));
      Chk("&RsGlobal.maximumWidth",uint32(reinterpret_cast<uintptr_t>(&RsGlobal.maximumWidth)),uint32(kRsMaxW)); Chk("&RsGlobal.maximumHeight",uint32(reinterpret_cast<uintptr_t>(&RsGlobal.maximumHeight)),uint32(kRsMaxH));
      Chk("&CReferences::pEmptyList",uint32(reinterpret_cast<uintptr_t>(&CReferences::pEmptyList)),0xBA1768);
      Chk("&CWeapon::bPhotographHasBeenTaken",uint32(reinterpret_cast<uintptr_t>(&CWeapon::bPhotographHasBeenTaken)),uint32(kFlags73A)); Chk("&CWeapon::ms_bTakePhoto",uint32(reinterpret_cast<uintptr_t>(&CWeapon::ms_bTakePhoto)),uint32(kFlags73A)+1);
      Chk("&gCrossHair[0].m_nTimeWhenToDeactivate (0x7417AA)",uint32(reinterpret_cast<uintptr_t>(&gCrossHair[0].m_nTimeWhenToDeactivate)),0xC8A83C);
      Chk("&gCrossHair[0].m_color.r (0x74178B)",uint32(reinterpret_cast<uintptr_t>(&gCrossHair[0].m_color.r)),0xC8A84C); Chk("&gCrossHair[0].m_color.g (0x741792)",uint32(reinterpret_cast<uintptr_t>(&gCrossHair[0].m_color.g)),0xC8A84D);
      Chk("&gCrossHair[0].m_color.b (0x741799)",uint32(reinterpret_cast<uintptr_t>(&gCrossHair[0].m_color.b)),0xC8A84E); Chk("&gCrossHair[0].m_fRotation (0x7417A0)",uint32(reinterpret_cast<uintptr_t>(&gCrossHair[0].m_fRotation)),0xC8A85C);
      Chk("&CWorld::pIgnoreEntity",uint32(reinterpret_cast<uintptr_t>(&CWorld::pIgnoreEntity)),uint32(kIgnoreEntity)); Chk("&CWorld::bIncludeBikers",uint32(reinterpret_cast<uintptr_t>(&CWorld::bIncludeBikers)),uint32(kIncludeBikers));
      { const uint32 saved=AtA<uint32>(kProcLevel); AtA<uint32>(kProcLevel)=2; const uint32 hi=uint32(g_LoadMonitor.GetProcLevel()); AtA<uint32>(kProcLevel)=1; const uint32 mid=uint32(g_LoadMonitor.GetProcLevel()); AtA<uint32>(kProcLevel)=saved;
        Chk("g_LoadMonitor.GetProcLevel (0x73BF65 cmp [0B7298Ch],2)",hi*10u+mid,21u); }
      // --- enumeraciones ---
      Chk("WEAPON_PISTOL_SILENCED",uint32(WEAPON_PISTOL_SILENCED),0x17); Chk("WEAPON_TEARGAS",uint32(WEAPON_TEARGAS),0x11); Chk("WEAPON_FALL",uint32(WEAPON_FALL),0x36); Chk("WEAPON_UZI_DRIVEBY",uint32(WEAPON_UZI_DRIVEBY),0x34);
      Chk("WEAPON_MICRO_UZI",uint32(WEAPON_MICRO_UZI),0x1C); Chk("WEAPON_SHOTGUN",uint32(WEAPON_SHOTGUN),0x19); Chk("WEAPON_SPAS12_SHOTGUN",uint32(WEAPON_SPAS12_SHOTGUN),0x1B); Chk("WEAPON_DESERT_EAGLE",uint32(WEAPON_DESERT_EAGLE),0x18);
      Chk("WEAPON_MINIGUN",uint32(WEAPON_MINIGUN),0x26); Chk("WEAPON_RLAUNCHER",uint32(WEAPON_RLAUNCHER),0x23); Chk("WEAPON_RLAUNCHER_HS",uint32(WEAPON_RLAUNCHER_HS),0x24); Chk("WEAPON_ROCKET",uint32(WEAPON_ROCKET),0x13);
      Chk("WEAPON_GRENADE",uint32(WEAPON_GRENADE),0x10); Chk("WEAPON_REMOTE_SATCHEL_CHARGE",uint32(WEAPON_REMOTE_SATCHEL_CHARGE),0x27);
      Chk("TASK_SIMPLE_DEAD",uint32(TASK_SIMPLE_DEAD),0xDA); Chk("TASK_SIMPLE_GANG_DRIVEBY",uint32(TASK_SIMPLE_GANG_DRIVEBY),0x3FE); Chk("TASK_SIMPLE_STEALTH_KILL",uint32(TASK_SIMPLE_STEALTH_KILL),0x403);
      Chk("PEDSTATE_DIE",uint32(PEDSTATE_DIE),0x36); Chk("PEDSTATE_DEAD",uint32(PEDSTATE_DEAD),0x37);
      Chk("ANIM_ID_FLOOR_HIT",uint32(ANIM_ID_FLOOR_HIT),0x24); Chk("ANIM_ID_FLOOR_HIT_F",uint32(ANIM_ID_FLOOR_HIT_F),0x27); Chk("ANIM_ID_SHOT_PARTIAL",uint32(ANIM_ID_SHOT_PARTIAL),0x1C);
      Chk("ANIM_ID_NO_ANIMATION_SET",uint32(ANIM_ID_NO_ANIMATION_SET),0xBF); Chk("ANIM_ID_DOOR_LHINGE_O",uint32(ANIM_ID_DOOR_LHINGE_O),0xC0);
      Chk("ANIMATION_IS_FRONT",uint32(ANIMATION_IS_FRONT),0x800); Chk("ANIMATION_IS_FINISH_AUTO_REMOVE",uint32(ANIMATION_IS_FINISH_AUTO_REMOVE),8); Chk("ANIMATION_IS_PLAYING",uint32(ANIMATION_IS_PLAYING),1);
      Chk("STAT_BULLETS_FIRED",uint32(STAT_BULLETS_FIRED),0x7E); Chk("STAT_BULLETS_THAT_HIT",uint32(STAT_BULLETS_THAT_HIT),0x80); Chk("STAT_PHOTOGRAPHS_TAKEN",uint32(STAT_PHOTOGRAPHS_TAKEN),0xA6);
      Chk("CRIME_FIRE_WEAPON",uint32(CRIME_FIRE_WEAPON),1); Chk("CRIME_DAMAGED_PED",uint32(CRIME_DAMAGED_PED),2); Chk("CRIME_DAMAGE_CAR",uint32(CRIME_DAMAGE_CAR),4); Chk("CRIME_EXPLOSION",uint32(CRIME_EXPLOSION),0x11);
      Chk("WEAPON_FIRE_MELEE (0x73A6FB test eax,eax)",uint32(eWeaponFire::WEAPON_FIRE_MELEE),0); Chk("ANIM_GROUP_DEFAULT (0x73A63A push 0)",uint32(ANIM_GROUP_DEFAULT),0);
      Chk("BONE_HEAD (0x73C2FF push 5)",uint32(BONE_HEAD),5);
      Chk("SURFACE_WATER_SHALLOW",uint32(SURFACE_WATER_SHALLOW),0x27); Chk("PED_PIECE_HEAD",uint32(ePedPieceTypes::PED_PIECE_HEAD),9); Chk("WEAPONEFFECTS_LOCK_ON",uint32(WEAPONEFFECTS_LOCK_ON),0);
      Chk("INTERESTING_EVENT_22",uint32(CInterestingEvents::INTERESTING_EVENT_22),0x16);
      Chk("MODE_M16_1STPERSON",uint32(MODE_M16_1STPERSON),0x22); Chk("MODE_SNIPER",uint32(MODE_SNIPER),7); Chk("MODE_CAMERA",uint32(MODE_CAMERA),0x2E); Chk("MODE_ROCKETLAUNCHER",uint32(MODE_ROCKETLAUNCHER),8);
      Chk("MODE_ROCKETLAUNCHER_HS",uint32(MODE_ROCKETLAUNCHER_HS),0x33); Chk("MODE_M16_1STPERSON_RUNABOUT",uint32(MODE_M16_1STPERSON_RUNABOUT),0x2A); Chk("MODE_SNIPER_RUNABOUT",uint32(MODE_SNIPER_RUNABOUT),0x27);
      Chk("MODE_ROCKETLAUNCHER_RUNABOUT",uint32(MODE_ROCKETLAUNCHER_RUNABOUT),0x28); Chk("MODE_ROCKETLAUNCHER_RUNABOUT_HS",uint32(MODE_ROCKETLAUNCHER_RUNABOUT_HS),0x34); Chk("MODE_TWOPLAYER_IN_CAR_AND_SHOOTING",uint32(MODE_TWOPLAYER_IN_CAR_AND_SHOOTING),0x31);
      Chk("ENTITY_TYPE_BUILDING..DUMMY",uint32(ENTITY_TYPE_BUILDING)*10000u+uint32(ENTITY_TYPE_VEHICLE)*1000u+uint32(ENTITY_TYPE_PED)*100u+uint32(ENTITY_TYPE_OBJECT)*10u+uint32(ENTITY_TYPE_DUMMY),12345u);
      Chk("PED_TYPE_PLAYER1/PLAYER2/CIVMALE/CIVFEMALE",uint32(PED_TYPE_PLAYER1)*1000u+uint32(PED_TYPE_PLAYER2)*100u+uint32(PED_TYPE_CIVMALE)*10u+uint32(PED_TYPE_CIVFEMALE),145u);
      { uint32 bad=0; for(uint32 p=0;p<256;++p){ const bool dll=notsa::contains(eCarPiece_WheelPieces,(eCarPiece)p); if(dll!=(p>=13&&p<=16))++bad; } Chk("eCarPiece_WheelPieces (13..16, 0x73BD2A cmp al,0Dh/10h) piezas distintas",bad,0); }
      // --- despacho virtual: el EXE llama por la ranura (BurstTyre 0x2C, SetIsStatic 4, GetTaskType 4) y la DLL tiene que usar la misma ---
      { trace=json::array();
        Clear(); At<void*>(eb,0)=vehVt.data(); reinterpret_cast<CVehicle*>(eb)->BurstTyre(13,true);
        Chk("DLL: CVehicle::BurstTyre usa la ranura 0x2C de la vtable",uint32(trace.size()==1&&trace[0][0]=="burst_tyre"),1);
        trace=json::array(); Clear(); At<void*>(eb,0)=objVt.data(); reinterpret_cast<CObject*>(eb)->SetIsStatic(false);
        Chk("DLL: CObject::SetIsStatic usa la ranura 4 de la vtable",uint32(trace.size()==1&&trace[0][0]=="set_is_static"),1);
        trace=json::array(); Clear(); At<void*>(eb,0)=taskVt.data(); At<uint32>(eb,8)=0xDA;
        Chk("DLL: CTask::GetTaskType usa la ranura 4 de la vtable",reinterpret_cast<CTask*>(eb)->GetTaskType()==TASK_SIMPLE_DEAD ? 1u : 0u,1);
        trace=json::array(); }
      Write({{"event","layout"},{"ok",layoutOk},{"fields",lay}}); }

    // estado global guardado
    if(regions.empty())SnapRegions();
    const unsigned savedFP=_controlfp(0,0);

    bool passed=layoutOk; int pair=0;
    for(size_t f=0;f<fns.size();++f) {
        const auto& fn=fns[f]; int failures=0; const std::string cls=fn.cls, nm=fn.name, full=cls+"::"+nm;
        if(!only.empty()&&only!=full)continue;
        for(int i=0;i<fn.cases;++i) {
            Write({{"event","case_start"},{"function",full},{"case",i},{"seed",uint32(i)}});
            auto run=[&](bool candidate)->json {
                uint32 stt=uint32(i)*2654435761u^uint32(f*0x9E3779B9u)^0x2B7Au; if(!stt)stt=1; Rng r{stt};
                trace=json::array(); gRng=&r; ResetRefs(); ResetTasks(); gAssocNull=false; gLosCalls=0;
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
