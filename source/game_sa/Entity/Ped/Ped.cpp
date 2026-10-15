/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPed
 * Reconstrucción: COMPLETA — 145/145 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 145/145 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

#include "Ped.h"
#include "PedPlacement.h"
#include "X87Math.h"

#include "PedType.h"
#include "Buoyancy.h"
#include "TaskSimpleSwim.h"
#include "PedStats.h"
#include "Conversations.h"
#include "TaskSimpleLand.h"
#include "AEAudioUtility.h"
#include "PedClothesDesc.h"
#include "EventAcquaintancePedHate.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskComplexGoPickUpEntity.h"
#include "Radar.h"
#include "PostEffects.h"
#include "PedStdBonePositions.h"
#include "TaskSimpleJetPack.h"
#include "PedSaveStructure.h"
#include "TaskSimpleStandStill.h"
#include "TaskComplexFacial.h"
#include "WeaponInfo.h"
#include "Shadows.h"
#include "EventSoundQuiet.h"
#include "TaskComplexDestroyCarMelee.h"
#include "Crime.h"
#include "TaskComplexEnterCarAsDriver.h"
#include "RealTimeShadowManager.h"
#include "WindModifiers.h"

void CPed::InjectHooks() {
    RH_ScopedVirtualClass(CPed, 0x86C358, 26);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(Constructor, 0x5E8030);
    RH_ScopedInstall(Destructor, 0x5E8620);

    RH_ScopedInstall(RequestDelayedWeapon, 0x5E8910);
    RH_ScopedInstall(DettachPedFromEntity, 0x5E7EC0);
    RH_ScopedInstall(AttachPedToBike, 0x5E7E60);
    RH_ScopedInstall(AttachPedToEntity, 0x5E7CB0);
    RH_ScopedInstall(OurPedCanSeeThisEntity, 0x5E1660);
    RH_ScopedOverloadedInstall(operator delete, "anon", 0x5E4760, void (*)(void*));
    RH_ScopedOverloadedInstall(operator new, "anon", 0x5E4720, void* (*)(unsigned));
    RH_ScopedOverloadedInstall(operator new, "poolIndexed", 0x5E4730, void* (*)(unsigned, int32));
    RH_ScopedInstall(SpawnFlyingComponent, 0x5F0190);
    RH_ScopedInstall(PedCanPickUpPickUp, 0x455560);
    RH_ScopedInstall(Update, 0x5DEBE0);
    RH_ScopedInstall(Initialise, 0x5DEBB0);
    RH_ScopedInstall(UpdateStatLeavingVehicle, 0x5E01B0);
    RH_ScopedInstall(UpdateStatEnteringVehicle, 0x5E01A0);
    RH_ScopedInstall(ShoulderBoneRotation, 0x5DF560);
    RH_ScopedInstall(RestoreHeadingRateCB, 0x5DFD70);
    RH_ScopedInstall(PedIsInvolvedInConversation, 0x43AB90);
    RH_ScopedInstall(ClearWeapons, 0x5E6320);
    RH_ScopedInstall(ClearWeapon, 0x5E62B0);
    RH_ScopedOverloadedInstall(SetCurrentWeapon, "WepType", 0x5E6280, void(CPed::*)(eWeaponType));
    RH_ScopedOverloadedInstall(SetCurrentWeapon, "Slot", 0x5E61F0, void(CPed::*)(int32));
    RH_ScopedOverloadedInstall(GiveWeapon, "", 0x5E6080, eWeaponSlot(CPed::*)(eWeaponType, uint32, bool));
    RH_ScopedInstall(TakeOffGoggles, 0x5E6010);
    RH_ScopedInstall(AddWeaponModel, 0x5E5ED0);
    RH_ScopedInstall(PlayFootSteps, 0x5E57F0);
    RH_ScopedInstall(DoFootLanded, 0x5E5380);
    RH_ScopedInstall(ClearAll, 0x5E5320);
    RH_ScopedInstall(CalculateNewOrientation, 0x5E52E0);
    RH_ScopedInstall(CalculateNewVelocity, 0x5E4C50);
    RH_ScopedInstall(SetCharCreatedBy, 0x5E47E0);
    RH_ScopedInstall(SetPedState, 0x5E4500);
    RH_ScopedInstall(GiveObjectToPedToHold, 0x5E4390);
    RH_ScopedInstall(ClearLookFlag, 0x5E1950);
    RH_ScopedInstall(WorkOutHeadingForMovingFirstPerson, 0x5E1A00);
    RH_ScopedInstall(UpdatePosition, 0x5E1B10);
    RH_ScopedInstall(MakeTyresMuddySectorList<CPtrListSingleLink<CPhysical*>>, 0x6AE0D0);
    RH_ScopedInstall(IsPedInControl, 0x5E3960);
    RH_ScopedInstall(RemoveWeaponModel, 0x5E3990);
    RH_ScopedInstall(RemoveWeaponWhenEnteringVehicle, 0x5E6370);
    RH_ScopedInstall(AddGogglesModel, 0x5E3A90);
    RH_ScopedInstall(SetWeaponSkill, 0x5E3C10);
    RH_ScopedInstall(ClearLook, 0x5E3FF0);
    RH_ScopedInstall(TurnBody, 0x5E4000);
    RH_ScopedInstall(IsPointerValid, 0x5E4220);
    RH_ScopedOverloadedInstall(GetBonePosition, "Original", 0x5E4280, void(CPed::*)(CVector*, eBoneTag, bool));
    RH_ScopedInstall(PutOnGoggles, 0x5E3AE0);
    RH_ScopedInstall(ReplaceWeaponWhenExitingVehicle, 0x5E6490);
    RH_ScopedInstall(KillPedWithCar, 0x5F0360);
    RH_ScopedInstall(IsPedHeadAbovePos, 0x5F02C0);
    RH_ScopedInstall(RemoveWeaponAnims, 0x5F0250);
    RH_ScopedInstall(DoesLOSBulletHitPed, 0x5F01A0);
    RH_ScopedInstall(RemoveBodyPart, 0x5F0140);
    RH_ScopedInstall(Say, 0x5EFFE0);
    RH_ScopedInstall(SayScript, 0x5EFFB0);
    RH_ScopedInstall(CanPedHoldConversation, 0x5EFFA0);
    RH_ScopedInstall(EnablePedSpeechForScriptSpeech, 0x5EFF90);
    RH_ScopedInstall(DisablePedSpeechForScriptSpeech, 0x5EFF80);
    RH_ScopedInstall(EnablePedSpeech, 0x5EFF70);
    RH_ScopedInstall(DisablePedSpeech, 0x5EFF60);
    RH_ScopedInstall(GetPedTalking, 0x5EFF50);
    RH_ScopedInstall(GiveWeaponWhenJoiningGang, 0x5E8BE0);
    RH_ScopedInstall(GiveDelayedWeapon, 0x5E89B0);
    RH_ScopedOverloadedInstall(GetWeaponSkill, "Current", 0x5E6580, eWeaponSkill(CPed::*)());
    RH_ScopedOverloadedInstall(GetWeaponSkill, "WeaponType", 0x5E3B60, eWeaponSkill(CPed::*)(eWeaponType));
    RH_ScopedInstall(PreRenderAfterTest, 0x5E65A0);
    RH_ScopedInstall(SetIdle, 0x5E7980);
    RH_ScopedOverloadedInstall(SetLook, "Heading", 0x5E79B0, void(CPed::*)(float));
    RH_ScopedOverloadedInstall(SetLook, "Entity", 0x5E7A60, void(CPed::*)(CEntity *));
    RH_ScopedInstall(Look, 0x5E7B20);
    RH_ScopedInstall(ReplaceWeaponForScriptedCutscene, 0x5E6530);
    RH_ScopedInstall(RemoveWeaponForScriptedCutscene, 0x5E6550);
    RH_ScopedInstall(GiveWeaponAtStartOfFight, 0x5E8AB0);
    RH_ScopedInstall(ProcessBuoyancy, 0x5E1FA0);
    RH_ScopedInstall(PositionPedOutOfCollision, 0x5E0820);
    RH_ScopedInstall(GrantAmmo, 0x5DF220);
    RH_ScopedInstall(GetWeaponSlot, 0x5DF200);
    RH_ScopedInstall(PositionAnyPedOutOfCollision, 0x5E13C0);
    RH_ScopedInstall(CanBeDeletedEvenInVehicle, 0x5DF150);
    RH_ScopedInstall(CanBeDeleted, 0x5DF100);
    RH_ScopedInstall(CanStrafeOrMouseControl, 0x5DF090);
    RH_ScopedInstall(CanBeArrested, 0x5DF060);
    RH_ScopedInstall(CanSetPedState, 0x5DF030);
    RH_ScopedInstall(CanPedReturnToState, 0x5DF000);
    RH_ScopedInstall(UseGroundColModel, 0x5DEFE0);
    RH_ScopedInstall(IsPedShootable, 0x5DEFD0);
    RH_ScopedInstall(GetLocalDirection, 0x5DEF60);
    RH_ScopedInstall(ClearAimFlag, 0x5DEF20);
    RH_ScopedOverloadedInstall(SetAimFlag, "Entity", 0x5DEED0, void(CPed::*)(CEntity *));
    RH_ScopedOverloadedInstall(SetAimFlag, "Heading", 0x5E8830, void(CPed::*)(float));
    RH_ScopedOverloadedInstall(SetLookFlag, "Entity", 0x5DEE40, void(CPed::*)(CEntity *, bool, bool));
    RH_ScopedOverloadedInstall(SetLookFlag, "Heading", 0x5DEDC0, void(CPed::*)(float, bool, bool));
    RH_ScopedInstall(CanUseTorsoWhenLooking, 0x5DED90);
    RH_ScopedInstall(PedIsReadyForConversation, 0x43ABA0);
    RH_ScopedInstall(CreateDeadPedMoney, 0x4590F0);
    RH_ScopedOverloadedInstall(CreateDeadPedPickupCoors, "", 0x459180, void(CPed::*)(float&, float&, float&));
    RH_ScopedInstall(CreateDeadPedWeaponPickups, 0x4591D0);
    RH_ScopedInstall(IsWearingGoggles, 0x479D10);
    RH_ScopedInstall(SetAmmo, 0x5DF290);
    RH_ScopedInstall(SetStayInSamePlace, 0x481090);
    RH_ScopedInstall(SetPedStats, 0x5DEBC0);
    RH_ScopedInstall(SetMoveState, 0x5DEC00);
    RH_ScopedInstall(SetMoveAnimSpeed, 0x5DEC10);
    RH_ScopedInstall(StopNonPartialAnims, 0x5DED10);
    RH_ScopedInstall(RestartNonPartialAnims, 0x5DED50);
    RH_ScopedInstall(DoWeHaveWeaponAvailable, 0x5DF300);
    RH_ScopedInstall(RemoveGogglesModel, 0x5DF170);
    RH_ScopedInstall(SetGunFlashAlpha, 0x5DF400);
    RH_ScopedInstall(CanSeeEntity, 0x5E0730);
    RH_ScopedInstall(SetPedDefaultDecisionMaker, 0x5E06E0);
    RH_ScopedInstall(GetWalkAnimSpeed, 0x5E04B0);
    RH_ScopedInstall(StopPlayingHandSignal, 0x5E0480);
    RH_ScopedInstall(IsPlayingHandSignal, 0x5E0460);
    RH_ScopedInstall(CanThrowEntityThatThisPedIsHolding, 0x5E0400);
    RH_ScopedInstall(DropEntityThatThisPedIsHolding, 0x5E0360);
    RH_ScopedInstall(GetEntityThatThisPedIsHolding, 0x5E02E0);
    RH_ScopedInstall(GetHoldingTask, 0x5E0290);
    RH_ScopedInstall(ReleaseCoverPoint, 0x5E0270);
    RH_ScopedInstall(DoGunFlash, 0x5DF340);
    RH_ScopedInstall(GetTransformedBonePosition, 0x5E01C0);
    RH_ScopedInstall(IsAlive, 0x5E0170);
    RH_ScopedInstall(DeadPedMakesTyresBloody, 0x6B4200);
    RH_ScopedInstall(Undress, 0x5E00F0);
    RH_ScopedInstall(SetLookTimer, 0x5DF8D0);
    RH_ScopedInstall(RestoreHeadingRate, 0x5DFD60);
    RH_ScopedInstall(Dress, 0x5E0130);
    RH_ScopedInstall(IsPlayer, 0x5DF8F0);
    RH_ScopedInstall(GetBikeRidingSkill, 0x5DF510);
    RH_ScopedInstall(SetPedPositionInCar, 0x5DF910);
    RH_ScopedInstall(SetRadioStation, 0x5DFD90);
    RH_ScopedInstall(PositionAttachedPed, 0x5DFDF0);
    RH_ScopedInstall(ResetGunFlashAlpha, 0x5DF4E0);

    RH_ScopedVMTInstall(SetModelIndex, 0x5E4880);
    RH_ScopedVMTInstall(DeleteRwObject, 0x5DEBF0);
    //RH_ScopedVirtualInstall(ProcessControl, 0x5E8CD0, { .reversed = false });
    RH_ScopedVMTInstall(Teleport, 0x5E4110);
    //RH_ScopedVirtualInstall(SpecialEntityPreCollisionStuff, 0x5E3C30, { .reversed = false });
    //RH_ScopedVirtualInstall(SpecialEntityCalcCollisionSteps, 0x5E3E90, { .reversed = false });
    RH_ScopedVMTInstall(PreRender, 0x5E8A20);
    RH_ScopedVMTInstall(Render, 0x5E7680);
    RH_ScopedVMTInstall(SetupLighting, 0x553F00);
    RH_ScopedVMTInstall(RemoveLighting, 0x5533B0);
    RH_ScopedVMTInstall(FlagToDestroyWhenNextProcessed, 0x5E7B70);
    //RH_ScopedVirtualInstall(ProcessEntityCollision, 0x5E2530, { .reversed = false });
    RH_ScopedVMTInstall(SetMoveAnim, 0x5E4A00);
    RH_ScopedVMTInstall(Save, 0x5D5730);
    RH_ScopedVMTInstall(Load, 0x5D4640);

    RH_ScopedGlobalInstall(SetPedAtomicVisibilityCB, 0x5F0060);
}

// 0x5E8030
CPed::CPed(ePedType pedType) : CPhysical(), m_pedIK{CPedIK(this)} {
    m_vecAnimMovingShiftLocal = CVector2D();

    m_fHealth = 100.0f;
    m_fMaxHealth = 100.0f;
    m_fArmour = 0.0f;

    m_nPedType = pedType;
    SetTypePed();

    // 0x5E8196
    physicalFlags.bCanBeCollidedWith = true;
    physicalFlags.bDisableTurnForce = true;

    SetCreatedBy(PED_GAME);

    m_pVehicle = nullptr;
    m_nAntiSpazTimer = 0;
    m_nUnconsciousTimer = 0;
    m_nAttackTimer = 0;
    m_nLookTime = 0;
    m_nDeathTimeMS = 0;

    m_vecAnimMovingShift = CVector2D();
    field_56C = CVector();
    field_578 = CVector(0.0f, 0.0f, 1.0f);

    m_nPedState = PEDSTATE_IDLE;
    m_nMoveState = PEDMOVE_STILL;
    m_fCurrentRotation = 0.0f;
    m_fHeadingChangeRate = 15.0f;
    m_fMoveAnim = 0.1f;
    m_fAimingRotation = 0.0f;
    m_standingOnEntity = nullptr;
    m_nWeaponShootingRate = 40;
    field_594 = 0;
    m_pEntityIgnoredCollision = nullptr;
    m_nSwimmingMoveState = 0;
    m_pFire = nullptr;
    m_fireDmgMult = 1.0f;
    m_pTargetedObject = nullptr;
    m_pLookTarget = nullptr;
    m_fLookDirection = 0.0f;
    m_pContactEntity = nullptr;
    field_588 = 99999.992f;
    m_fMass = 70.0f;
    m_fTurnMass = 100.0f;
    m_fAirResistance = 1.f / 175.f;
    m_fElasticity = 0.05f;
    m_nBodypartToRemove = -1;
    // Como el original (0x5E82EA: and al,3 / neg al / sbb / inc): verdadero solo cuando rand & 3 == 0
    bHasACamera = CGeneral::GetRandomNumber() % 4 == 0;

    m_weaponAudio.Initialise(this);
    m_pedAudio.Initialise(this);

    m_acquaintance = CPedType::GetPedTypeAcquaintances(m_nPedType);
    m_nSavedWeapon = WEAPON_UNIDENTIFIED;
    m_nDelayedWeapon = WEAPON_UNIDENTIFIED;
    m_nActiveWeaponSlot = 0;

    for (auto& weapon : m_aWeapons ) {
        weapon.m_Type = WEAPON_UNARMED;
        weapon.m_State = WEAPONSTATE_READY;
        weapon.m_AmmoInClip = 0;
        weapon.m_TotalAmmo = 0;
        weapon.m_TimeForNextShotMs = 0;
    }

    m_nWeaponSkill = eWeaponSkill::STD;
    m_nFightingStyle = STYLE_STANDARD;
    m_nAllowedAttackMoves = 0;

    GiveWeapon(WEAPON_UNARMED, 0, true);

    m_nWeaponAccuracy = 60;
    m_nLastWeaponDamage = -1;
    m_pLastEntityDamage = nullptr;
    field_768 = 0;
    m_pAttachedTo = nullptr;
    m_nTurretAmmo = 0;
    // Como el original (0x5E8411): escribe solo el byte bajo de +0x460 (el EXE no toca los otros tres)
    *reinterpret_cast<uint8*>(&m_roadRageWith) = 0;
    field_468 = 0;
    m_nWeaponModelId = -1;
    m_nMoneyCount = 0;
    field_72F = 0;
    m_nTimeTillWeNeedThisPed = 0;
    m_VehDeadInFrontOf = nullptr;

    m_pWeaponObject = nullptr;
    m_pGunflashObject = nullptr;
    m_pGogglesObject = nullptr;
    m_pGogglesState = nullptr;

    m_nWeaponGunflashAlphaMP1 = 0;
    m_nWeaponGunFlashAlphaProgMP1 = 0;
    m_nWeaponGunflashAlphaMP2 = 0;
    m_nWeaponGunFlashAlphaProgMP2 = 0;

    m_pCoverPoint = nullptr;
    m_pEnex = nullptr;
    field_798 = -1;

    m_pIntelligence = new CPedIntelligence(this);
    m_pPlayerData = nullptr;

    if (!IsPlayer()) {
        GetTaskManager().SetTaskSecondary(new CTaskComplexFacial{}, TASK_SECONDARY_FACIAL_COMPLEX);
    }
    GetTaskManager().SetTask(new CTaskSimpleStandStill{ 0, true, false, 8.0 }, TASK_PRIMARY_DEFAULT, false);

    m_Wobble = 0.0f;
    m_fRemovalDistMultiplier = 1.0f;
    m_StreamedScriptBrainToLoad = -1;

    CPopulation::UpdatePedCount(this, 0);

    if (CCheat::IsActive(CHEAT_HAVE_ABOUNTY_ON_YOUR_HEAD)) {
        if (!IsPlayer()) {
            GetAcquaintance().SetAsAcquaintance(ACQUAINTANCE_HATE, CPedType::GetPedFlag(ePedType::PED_TYPE_PLAYER1));
            GetEventGroup().Add(CEventAcquaintancePedHate{FindPlayerPed()});
        }
    }
}

/*!
* @addr 0x5E8620
 */
CPed::~CPed() {
    CReplay::RecordPedDeleted(this);

    // Remove script brain
    if (bWaitingForScriptBrainToLoad) {
        CStreaming::SetMissionDoesntRequireModel(SCMToModelId(CTheScripts::ScriptsForBrains.m_aScriptForBrains[m_StreamedScriptBrainToLoad].m_StreamedScriptIndex));
        bWaitingForScriptBrainToLoad = false;
        CTheScripts::RemoveFromWaitingForScriptBrainArray(this, m_StreamedScriptBrainToLoad);
        m_StreamedScriptBrainToLoad = -1;
    }

    CWorld::Remove(this);
    CRadar::ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetRef(this));
    CConversations::RemoveConversationForPed(this);

    ClearReference(m_pVehicle);

    if (m_pFire) {
        m_pFire->Extinguish();
    }

    ReleaseCoverPoint();
    ClearWeapons();

    if (bMiamiViceCop) {
        CPopulation::NumMiamiViceCops--;
    }

    CPopulation::UpdatePedCount(this, 1);

    m_pedSpeech.Terminate();
    m_weaponAudio.Terminate();
    m_pedAudio.Terminate();

    delete m_pIntelligence;

    // Como el original (0x5E879F): limpia la referencia pero NO pone m_pLookTarget a nulo
    SafeCleanUpRef(m_pLookTarget);

    // Como el original (0x5E8765..0x5DE961): los destructores de los audios dejan la vtable base del EXE (0x859988)
    // en el audio de armas (+0x394) y en el del peaton (+0x138); el de voz (+0x294) deja la suya.
    *reinterpret_cast<uintptr_t*>(&m_weaponAudio) = 0x859988;
    *reinterpret_cast<uintptr_t*>(&m_pedAudio) = 0x859988;
}

/*!
* @addr 0x5E4720
*/
void* CPed::operator new(unsigned size) {
    return GetPedPool()->New();
}

/*!
* @addr 0x5E4730
*/
void* CPed::operator new(unsigned size, int32 poolRef) {
    return GetPedPool()->NewAt(poolRef);
}

/*!
* @addr 0x5E4760
*/
void CPed::operator delete(void* data) {
    GetPedPool()->Delete((CPed*)data);
}

// NOTSA
void CPed::operator delete(void* data, int poolRef) {
    GetPedPool()->Delete((CPed*)data);
}

/*!
* @addr 0x5E4A00
*/
void CPed::SetMoveAnim() {
    if (!IsAlive() || bIsDucking || m_pAttachedTo) {
        return;
    }

    const auto DoUpdateMoveAnim = [this](auto* assoc) {
        if (!bMoveAnimSpeedHasBeenSetByTask) {
            SetMoveAnimSpeed(assoc);
        }
    };

    if (m_nSwimmingMoveState == m_nMoveState) {
        // Como el original: cualquier estado >= WALK (con signo), y solo si la pendiente sale de [-0,01; 0,01] (NaN no entra)
        if ((int32)m_nMoveState < (int32)PEDMOVE_WALK) {
            return;
        }
        const auto slope = m_pedIK.m_fSlopePitch;
        if (!(slope > 0.01f || slope < -0.01f)) {
            return;
        }

        const auto GetAnimId = [this] {
            switch (m_nMoveState) {
            case PEDMOVE_RUN:
                return ANIM_ID_RUN;
            case PEDMOVE_SPRINT:
                return ANIM_ID_SPRINT;
            }
            return ANIM_ID_WALK;
        };

        if (const auto assoc = RpAnimBlendClumpGetAssociation(GetRpClump(), GetAnimId())) {
            DoUpdateMoveAnim(assoc);
        }
    } else if (m_nMoveState != PEDMOVE_NONE) {
        m_nSwimmingMoveState = m_nMoveState;

        switch (m_nMoveState) { // TODO: What's happening here?
        case PEDMOVE_WALK:
        case PEDMOVE_RUN:
        case PEDMOVE_SPRINT: {
            for (auto assoc = RpAnimBlendClumpGetFirstAssociation(GetRpClump(), ANIMATION_IS_PARTIAL); assoc; assoc = RpAnimBlendGetNextAssociation(assoc, ANIMATION_IS_PARTIAL)) {
                if ((assoc->m_Flags & ANIMATION_IS_FINISH_AUTO_REMOVE) == 0 && (assoc->m_Flags & ANIMATION_DONT_ADD_TO_PARTIAL_BLEND) == 0) {
                    assoc->m_BlendDelta = -2.f;
                    assoc->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
                }
            }

            ClearAimFlag();
            ClearLookFlag();

            break;
        }
        }

        // Do BlendAnimation and call `DoUpdateMoveAnim` afterwards
        const auto DoBlendAnim = [&, this](AssocGroupId grp, AnimationId animId, float blendDelta) {
            if (const auto assoc = CAnimManager::BlendAnimation(GetRpClump(), grp, animId, blendDelta)) {
                DoUpdateMoveAnim(assoc);
            }
        };

        switch (m_nMoveState) {
        case PEDMOVE_STILL:
            DoBlendAnim(m_nAnimGroup, ANIM_ID_IDLE, 4.f);
            return;

        case PEDMOVE_TURN_L:
            DoBlendAnim(ANIM_GROUP_DEFAULT, ANIM_ID_TURN_L, 16.f);
            return;

        case PEDMOVE_TURN_R:
            DoBlendAnim(ANIM_GROUP_DEFAULT, ANIM_ID_TURN_R, 16.f);
            return;

        case PEDMOVE_WALK:
            DoBlendAnim(m_nAnimGroup, ANIM_ID_WALK, 1.f);
            return;

        case PEDMOVE_RUN:
            DoBlendAnim(m_nAnimGroup, ANIM_ID_RUN, m_nPedState == PEDSTATE_FLEE_ENTITY ? 3.f : 1.f);
            return;

        case PEDMOVE_SPRINT: {
            // If we're in a group, and our leader is sprinting as well sprinting should be played with a different anim group
            // Como el original: el grupo y el lider se piden dos veces y se compara con signo con RUN (>= 6)
            if (CPedGroups::IsInPlayersGroup(this)) {
                if (CPedGroups::GetPedsGroup(this)->GetMembership().GetLeader()) {
                    if ((int32)CPedGroups::GetPedsGroup(this)->GetMembership().GetLeader()->m_nMoveState >= (int32)PEDMOVE_RUN) {
                        DoBlendAnim(ANIM_GROUP_PLAYER, ANIM_ID_SPRINT, 1.f);
                        return;
                    }
                }
            }
            DoBlendAnim(m_nAnimGroup, ANIM_ID_SPRINT, 1.f);
            return;
        }
        }
    } else {
        m_nSwimmingMoveState = PEDMOVE_NONE;
    }
}

/*!
* @addr 0x5D4640
 */
bool CPed::Load() {
    // Como el original (0x5D4640): lee el tamaño (4 bytes) y lo ignora, sin assert (el EXE no lo comprueba)
    [[maybe_unused]] auto size = CGenericGameStorage::LoadDataFromWorkBuffer<uint32>();

    // TODO: Can't do `auto save = CGenericGameStorage::LoadDataFromWorkBuffer<CPedSaveStructure>()` dure to deleted copy constructor in CWanted which is used somehow inside.
    // Would be nice if someone with more knowledge of templates and shit can fix that
    CPedSaveStructure save;
    CGenericGameStorage::LoadDataFromWorkBuffer(save);

    save.Extract(this);
    return true;
}

/*!
* @addr 0x5D5730
*/
bool CPed::Save() {
    CPedSaveStructure save;
    save.Construct(this);

    CGenericGameStorage::SaveDataToWorkBuffer(sizeof(save));
    CGenericGameStorage::SaveDataToWorkBuffer(save);

    return true;
}

/*!
* @addr 0x43AB90
*/
bool CPed::PedIsInvolvedInConversation() {
    return this == CPedToPlayerConversations::m_pPed;
}

/*!
* @addr 0x43ABA0
*/
bool CPed::PedIsReadyForConversation(bool checkLocalPlayerWantedLevel) {
    // We don't talk when we're behind the wheel! (Nor when we're fighting...)
    if (bInVehicle || GetIntelligence()->GetTaskFighting()) {
        return false;
    }

    if (checkLocalPlayerWantedLevel && FindPlayerPed()->GetWanted()->GetWantedLevel() != eWantedLevel::WANTED_CLEAN) {
        return false;
    }

    // If we're doing any of these we don't have the mental power to chat...
    switch (m_nMoveState) {
    case PEDMOVE_JOG:
    case PEDMOVE_RUN:
    case PEDMOVE_SPRINT:
        return false;
    }

    if (!IsCreatedByMission()) { // Don't check if we've a chatting task/event if we're a mission ped
        if (GetIntelligence()->FindTaskByType(TASK_COMPLEX_PARTNER_CHAT)) {
            return false;
        }

        if (GetEventGroup().GetEventOfType(eEventType::EVENT_CHAT_PARTNER)) {
            return false;
        }
    }

    return true;
}

/*!
* @addr 0x455560
*/
bool CPed::PedCanPickUpPickUp() {
    // Como el original: CPedIntelligence::FindTaskByType (0x600EE0) del jugador 0, dos veces (no FindActiveTaskByType)
    return !FindPlayerPed(0)->GetIntelligence()->FindTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER)
        && !FindPlayerPed(0)->GetIntelligence()->FindTaskByType(TASK_COMPLEX_USE_MOBILE_PHONE);
}

/*!
* @addr 0x4590F0
*/
void CPed::CreateDeadPedMoney() {
    if (!CLocalisation::StealFromDeadPed()) {
        return;
    }

    switch (m_nPedType) {
    case PED_TYPE_COP:
    case PED_TYPE_MEDIC:
    case PED_TYPE_FIREMAN:
        return;
    }

    if (IsCreatedByMission() && !bMoneyHasBeenGivenByScript) {
        return;
    }

    if (bInVehicle) {
        return;
    }

    // Como el original: cmp cx,0Ah / jb (sin signo, el dinero 10 ya se crea) y movzx al pasar la cantidad
    if ((uint16)m_nMoneyCount >= 10) {
        CPickups::CreateSomeMoney(GetPosition(), (uint16)m_nMoneyCount);
        m_nMoneyCount = 0;
    }
}

/*!
* @addr 0x459180
* @brief Created a pickup close to the ped's position (Using CPickups::CreatePickupCoorsCloseToCoors)
* @param [out] outPickupX, outPickupY, outPickupZ Position of the created pickup.
*/
void CPed::CreateDeadPedPickupCoors(float& outPickupX, float& outPickupY, float& outPickupZ) {
    CPickups::CreatePickupCoorsCloseToCoors(GetPosition(), outPickupX, outPickupY, outPickupZ);
}

/*!
* @notsa
* @copybrief CPed::CreateDeadPedPickupCoors
* @param [out] pickupPos Position of the created pickup.
*/
void CPed::CreateDeadPedPickupCoors(CVector& pickupPos) {
    return CreateDeadPedPickupCoors(pickupPos.x, pickupPos.y, pickupPos.z);
}

/*!
* @notsa
*/
RpHAnimHierarchy& CPed::GetAnimHierarchy() const {
    return *GetAnimHierarchyFromSkinClump(GetRpClump());
}

CAnimBlendClumpData& CPed::GetAnimBlendData() const {
    return *RpAnimBlendClumpGetData(GetRpClump());
}

/*!
* @addr 0x4591D0
* @brief Create weapon/ammo pickups for dead ped
*/
void CPed::CreateDeadPedWeaponPickups() {
    if (bInVehicle || bDoesntDropWeaponsWhenDead) {
        return;
    }

    for (auto& wep : m_aWeapons) {
        switch (wep.m_Type) {
        case WEAPON_UNARMED:
        case WEAPON_DETONATOR:
            continue;
        }

        if (!wep.m_TotalAmmo && !wep.IsTypeMelee()) {
            continue; // Has no ammo, but isn't a melee weapon.. so it's a weapon with no ammo :D
        }

        // Now, create a pickup at close to our position
        CVector pickupPos;
        CreateDeadPedPickupCoors(pickupPos);
        pickupPos.z += 0.3f;

        // No. of ammo the pickups will contain
        // Como el original: min con signo entre la munición y la mitad (shr 1) de la tabla de la calle
        const int32 pickupAmmo{ std::min((int32)wep.m_TotalAmmo, (int32)(AmmoForWeapon_OnStreet[(size_t)wep.m_Type] >> 1)) };

        if (!CPickups::TryToMerge_WeaponType(
            pickupPos,
            wep.m_Type,
            ePickupType::PICKUP_ONCE_TIMEOUT,
            pickupAmmo,
            false
        )) {
            CPickups::GenerateNewOne_WeaponType(
                pickupPos,
                wep.m_Type,
                bDeathPickupsPersist ? ePickupType::PICKUP_ONCE_FOR_MISSION : ePickupType::PICKUP_ONCE_TIMEOUT,
                pickupAmmo,
                false,
                nullptr
            );
        }
    }

    // Como el original: tras recorrer las 13 armas llama a ClearWeapons (0x5E6320)
    ClearWeapons();
}

/*!
* @addr 0x5DEBB0
*/
void CPed::Initialise() {
    CPedType::Initialise();
    CCarEnterExit::SetAnimOffsetForEnterOrExitVehicle();
}

/*!
* @addr 0x5DEBC0
* @unused
*/
void CPed::SetPedStats(ePedStats statsType) {
    m_pStats = &CPedStats::ms_apPedStats[(size_t)statsType];
}

/*!
* @addr 0x5DEBE0
*/
void CPed::Update()
{
    // NOP
}

/*!
* @addr 0x5DEC00
*/
void CPed::SetMoveState(eMoveState moveState) {
    m_nMoveState = moveState;
}

/*!
* @addr 0x5DEC10
*/
void CPed::SetMoveAnimSpeed(CAnimBlendAssociation* association) {
    const auto pitchFactor = std::clamp(m_pedIK.m_fSlopePitch, -0.3f, 0.3f);
    if (IsCreatedByMission()) {
        association->m_Speed = pitchFactor + 1.f;
    } else {
        // Como el original: (semilla * 0,4) * (1 / 32767), no semilla * (1 / 32767) * 0,4
        association->m_Speed = pitchFactor + 1.2f - (float)m_nRandomSeed * 0.4f * RAND_MAX_FLOAT_RECIPROCAL;
    }
}

/*!
* @addr 0x5DED10
*/
void CPed::StopNonPartialAnims() {
    // Como el original: primera/siguiente asociacion (la primera comprueba que el clump tenga datos de animacion)
    for (auto* a = RpAnimBlendClumpGetFirstAssociation(GetRpClump()); a; a = RpAnimBlendGetNextAssociation(a)) {
        if (!(a->m_Flags & ANIMATION_IS_PARTIAL)) {
            a->SetFlag(ANIMATION_IS_PLAYING, false);
        }
    }
}

/*!
* @addr 0x5DED50
*/
void CPed::RestartNonPartialAnims() {
    // Como el original: primera/siguiente asociacion (la primera comprueba que el clump tenga datos de animacion)
    for (auto* a = RpAnimBlendClumpGetFirstAssociation(GetRpClump()); a; a = RpAnimBlendGetNextAssociation(a)) {
        if (!(a->m_Flags & ANIMATION_IS_PARTIAL)) {
            a->SetFlag(ANIMATION_IS_PLAYING, true);
        }
    }
}

/*!
* @addr 0x5DED90
*/
bool CPed::CanUseTorsoWhenLooking() const {
    switch (m_nPedState) {
    case PEDSTATE_DRIVING:
    case PEDSTATE_DRAGGED_FROM_CAR:
        return false;
    }

    if (bIsDucking) {
        return false;
    }

    return true;
}

/*!
* @addr 0x5DEDC0
*/
void CPed::SetLookFlag(float lookHeading, bool unused, bool ignoreLookTime) {
    UNUSED(unused);

    if (m_nLookTime >= CTimer::GetTimeInMS() && !ignoreLookTime) {
        return;
    }

    // Como el original: tambien se limpia bIsRestoringLook
    bIsRestoringLook = false;
    bIsLooking = true;
    m_fLookDirection = lookHeading;
    m_nLookTime = 0;

    ClearReference(m_pLookTarget);

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }
}

/*!
* @addr 0x5DEE40
* @brief Start looking at entity \a lookingTo
*/
void CPed::SetLookFlag(CEntity* lookingTo, bool unused, bool ignoreLookTime) {
    UNUSED(unused);

    if (m_nLookTime >= CTimer::GetTimeInMS() && !ignoreLookTime) {
        return;
    }

    bIsRestoringLook = false;
    bIsLooking = true;

    ChangeEntityReference(m_pLookTarget, lookingTo);

    m_fLookDirection = 999'999.f;
    m_nLookTime = 0;

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }
}

/*!
* @addr 0x5DEED0
*/
void CPed::SetAimFlag(CEntity* aimingTo) {
    bIsAimingGun = true;
    bIsRestoringGun = false;
    ChangeEntityReference(m_pLookTarget, aimingTo);
    m_nLookTime = 0;
}

/*!
* @addr 0x5DEF20
* @brief Clear gun aiming flag
*/
void CPed::ClearAimFlag() {
    if (bIsAimingGun) {
        bIsAimingGun = false;
        bIsRestoringGun = true;
        m_pedIK.bUseArm = false;
        m_nLookTime = 0;
    }

    if (GetPlayerData()) {
        GetPlayerData()->m_fLookPitch = 0.f;
    }
}

/*!
* @addr 0x5DEF60
* @returns Which quadrant a given point is in relative to the ped's rotation. (Google: "Angle quadrants" - https://www.mathstips.com/wp-content/uploads/2014/03/unit-circle.png)
* @param point Point should be relative to the ped's position. Eg.: point = actualPoint - ped.GetPostion2D()
*/
#pragma runtime_checks("", off)
__declspec(safebuffers) int32 CPed::GetLocalDirection(const CVector2D& point) const {
    // Como el original (0x5DEF60): fpatan a precision completa, constantes del EXE (pi/4 0x859AB0, 2pi 0x858CBC,
    // 2/pi 0x858FB8; NO 90 grados), bucle mientras el angulo sea < 0, truncado a 64 bits (_ftol2) y el resultado
    // solo se reduce modulo 4 cuando es > 3 (un valor <= 3, incluido uno negativo o INT_MIN por NaN, se devuelve tal cual).
    // La parte x87 (fpatan(-x, y) - rotacion de 0x558, + pi/4, + 2pi mientras < 0, * 2/pi y _ftol2) esta en X87Math.cpp.
    //
    // Registros de salida del EXE (0x5DEFC5, ret 4), que reproduce ResidueRegsCpp (X87Math.h) como ultima llamada:
    //   entero <= 3 : EAX/ECX/EDX = los que deja _ftol2 (0x821B40) con ECX de entrada = this (ver Ftol2RegsCpp)
    //   entero  > 3 : ECX = ((n - 4) >> 2) << 2, EDX = 0xFFFFFFFC - ECX, EAX = n + EDX (lea/shr/shl; mov/sub; add)
    // Sin comprobaciones de ejecucion (RTC) ni cookie tras esa llamada (contrib/audit_register_residue.py).
    const Ftol2Regs r = PedLocalDirectionRegsCpp(&point.x, m_fCurrentRotation, static_cast<uint32>(reinterpret_cast<uintptr_t>(this)));
    uint32 eax = r.eax, ecx = r.ecx, edx = r.edx;
    if (static_cast<int32>(eax) > 3) {
        ecx = ((eax - 4u) >> 2) << 2;
        edx = 0xFFFFFFFCu - ecx;
        eax += edx;
    }
    return static_cast<int32>(ResidueRegsCpp(ecx, edx, eax));
}
#pragma runtime_checks("", restore)

/*!
* @addr 0x5DEFD0
*/
bool CPed::IsPedShootable() const {
    // Como el original (0x5DEFD0: cmp [estado], 0x2E / setle): comparación con signo, los estados negativos también cuentan.
    return m_nPedState <= PEDSTATE_STATES_CAN_SHOOT;
}

/*!
* @addr 0x5DEFE0
*/
bool CPed::UseGroundColModel() const {
    switch (m_nPedState) {
    case PEDSTATE_FALL:
    case PEDSTATE_EVADE_DIVE:
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
        return true;
    }
    return false;
}

/*!
* @addr 0x5DF000
*/
bool CPed::CanPedReturnToState() const {
    // Como el original (0x5DF000): jg 0x26 -> false (comparación con signo, los estados negativos pasan) y
    // los seis estados excluidos devuelven false.
    if (m_nPedState > PEDSTATE_STATES_NO_AI) {
        return false;
    }
    switch (m_nPedState) {
    case PEDSTATE_LOOK_ENTITY:
    case PEDSTATE_SNIPER_MODE:
    case PEDSTATE_ATTACK:
    case PEDSTATE_FIGHT:
    case PEDSTATE_AIMGUN:
    case PEDSTATE_EVADE_STEP:
        return false;
    }
    return true;
}

/*!
* @addr 0x5DF030
*/
bool CPed::CanSetPedState() const {
    switch (m_nPedState) {
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
    case PEDSTATE_ARRESTED:
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_CARJACK:
    case PEDSTATE_STEAL_CAR:
        return false;
    }
    return true;
}

/*!
* @addr 0x5DF060
*/
bool CPed::CanBeArrested() const {
    switch (m_nPedState) {
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
    case PEDSTATE_ARRESTED:
    case PEDSTATE_ENTER_CAR:
    case PEDSTATE_EXIT_CAR:
        return false;
    }
    return true;
}

/*!
* @addr 5DF090
*/
bool CPed::CanStrafeOrMouseControl() const {
    switch (m_nPedState) {
    case PEDSTATE_IDLE:
    case PEDSTATE_FLEE_ENTITY:
    case PEDSTATE_FLEE_POSITION:
    case PEDSTATE_NONE:
    case PEDSTATE_AIMGUN:
    case PEDSTATE_ATTACK:
    case PEDSTATE_FIGHT:
    case PEDSTATE_JUMP:
    case PEDSTATE_ANSWER_MOBILE:
        return true;
    }
    return false;
}

/*!
* @addr 0x5DF100
* @brief Check if ped can be deleted
* @returns Always false if ped is in vehicle or is follower of player's group.
*/
bool CPed::CanBeDeleted() {
    return !bInVehicle && !IsFollowerOfGroup(FindPlayerGroup()) && CanBeDeletedEvenInVehicle();
}

/*!
* @addr 0x5DF150
* @brief Check if ped can be deleted even if it's in a vehicle.
* @returns False only if created by PED_UNKNOWN or PED_MISSION, true otherwise.
*/
bool CPed::CanBeDeletedEvenInVehicle() const {
    switch (GetCreatedBy()) {
    case ePedCreatedBy::PED_MISSION:
    case ePedCreatedBy::PED_GAME_MISSION:
        return false;
    }
    return true;
}

/*!
* @addr 0x5DF170
* @brief Remove goggles model, also disabled related PostFX.
*/
void CPed::RemoveGogglesModel() {
    if (!m_pGogglesObject) {
        return;
    }

    // Release model info
    CVisibilityPlugins::GetClumpModelInfo(m_pGogglesObject)->RemoveRef();

#ifdef SA_SKINNED_PEDS
    // Remove skin anim
    if (IsClumpSkinned(m_pGogglesObject)) {
        RpClumpForAllAtomics(m_pGogglesObject, AtomicRemoveAnimFromSkinCB, nullptr);
    }
#endif

    // Destroy clump
    RpClumpDestroy(m_pGogglesObject);
    m_pGogglesObject = nullptr;

    // Disable FX's of the goggles. (See mem. var. `m_pGogglesState` in the header)
    if (m_pGogglesState) {
        *m_pGogglesState = false;
        m_pGogglesState = nullptr;
    }
}

/*!
* @addr   0x5DF200
* @return \a weaponType weapon's slot (CWeaponInfo::GetWeaponInfo()->slot)
*/
int32 CPed::GetWeaponSlot(eWeaponType weaponType)
{
    return CWeaponInfo::GetWeaponInfo(weaponType)->m_nSlot;
}

/*!
* @addr 0x5DF220
* @brief Set \a weaponType's slot totalAmmo to \a ammo. Also changes the gun's state to `READY`
*/
void CPed::GrantAmmo(eWeaponType weaponType, uint32 ammo) {
    const auto wepSlot = GetWeaponSlot(weaponType);
    if (wepSlot != -1) {
        auto& wepInSlot = GetWeaponInSlot(wepSlot);

        // Como el original: suma de 32 bits y recorte con comparacion CON signo (jl), no sin signo
        const auto total = static_cast<int32>(wepInSlot.m_TotalAmmo + ammo);
        wepInSlot.m_TotalAmmo = total < 99'999 ? static_cast<uint32>(total) : 99'999u; // Clamp upper

        // TODO: Inlined
        if (wepInSlot.m_State == WEAPONSTATE_OUT_OF_AMMO) {
            if (static_cast<int32>(wepInSlot.m_TotalAmmo) > 0) {
                wepInSlot.m_State = WEAPONSTATE_READY;
            }
        }
    }
}

/*!
* @addr 0x5DF290
* @brief Im lazy to write it :D Similar to CPed::GrantAmmo
*/
void CPed::SetAmmo(eWeaponType weaponType, uint32 ammo) {
    const auto wepSlot = GetWeaponSlot(weaponType);
    if (wepSlot != -1) {
        auto& wepInSlot = GetWeaponInSlot(wepSlot);

        // Como el original: recorte con signo y el cargador baja al total (min con signo, no max)
        const auto total = static_cast<int32>(ammo) < 99'999 ? static_cast<int32>(ammo) : 99'999;
        wepInSlot.m_TotalAmmo = static_cast<uint32>(total);
        if (total < static_cast<int32>(wepInSlot.m_AmmoInClip)) {
            wepInSlot.m_AmmoInClip = static_cast<uint32>(total);
        }

        // TODO: Inlined
        if (wepInSlot.m_State == WEAPONSTATE_OUT_OF_AMMO) {
            if (total > 0) {
                wepInSlot.m_State = WEAPONSTATE_READY;
            }
        }
    }
}

/*!
* @addr 0x5DF300
* @brief Check if ped has a weapon of type \a weaponType
*/
bool CPed::DoWeHaveWeaponAvailable(eWeaponType weaponType) {
    const auto slot = GetWeaponSlot(weaponType);
    return slot != -1 && GetWeaponInSlot(slot).m_Type == weaponType;
}

/*!
* @addr 0x5DF340
* @brief Do gun flash by resetting it's alpha to max
*/
bool CPed::DoGunFlash(int32 duration, bool isLeftHand) {
    if (!m_pGunflashObject || !m_pWeaponObject) {
        return false;
    }

    // Really elegant.. ;D
    if (isLeftHand) {
        m_nWeaponGunflashAlphaMP2     = m_sGunFlashBlendStart;
        m_nWeaponGunFlashAlphaProgMP2 = (uint16)m_sGunFlashBlendStart / duration;
    } else {
        m_nWeaponGunflashAlphaMP1     = m_sGunFlashBlendStart;
        m_nWeaponGunFlashAlphaProgMP1 = (uint16)m_sGunFlashBlendStart / duration;
    }

    const auto angle = CGeneral::GetRandomNumberInRange(-360.0f, 360.0f);
    RwMatrixRotate(RwFrameGetMatrix(m_pGunflashObject), &CPedIK::XaxisIK, angle, rwCOMBINEPRECONCAT);

    return true;
}

/*!
* @addr 0x5DF400
* @brief Set alpha of gun flash object
*/
void CPed::SetGunFlashAlpha(bool rightHand) {
    if (!m_pGunflashObject) {
        return;
    }

    if (m_nWeaponGunflashAlphaMP1 < 0 && m_nWeaponGunflashAlphaMP2 < 0) { // Reordered a little.
        return;
    }

    auto& gunFlashAlphaInHand = rightHand ? m_nWeaponGunflashAlphaMP2 : m_nWeaponGunflashAlphaMP1;

    if (auto atomic = (RpAtomic*)GetFirstObject(m_pGunflashObject)) {
        // They used a clever trick to not have to convert to float..
        // Then they converted to a float to check if the number is higher than 255.. XDDD
        if (gunFlashAlphaInHand <= 0) {
            CVehicle::SetComponentAtomicAlpha(atomic, 0);
        } else {
            CVehicle::SetComponentAtomicAlpha(atomic, std::min(255, 350 * gunFlashAlphaInHand / m_sGunFlashBlendStart));
        }
        RpAtomicSetFlags(atomic, rpATOMICRENDER);
    }

    if (!gunFlashAlphaInHand) {
        gunFlashAlphaInHand = (uint16)-1;
    }
}

/*!
* @addr 0x5DF4E0
* @brief Reset alpha of gun flash object
*/
void CPed::ResetGunFlashAlpha() {
    if (m_pGunflashObject) {
        if (auto atomic = (RpAtomic*)GetFirstObject(m_pGunflashObject)) {
            RpAtomicSetFlags(atomic, 0);
            CVehicle::SetComponentAtomicAlpha(atomic, 0);
        }
    }
}

/*!
* @addr 0x5DF510
* @returns If ped is a player returns stat value BIKE_SKILL, otherwise 1 for mission peds and 0 for all others.
*/
float CPed::GetBikeRidingSkill() const {
    if (GetPlayerData()) {
        // Como el original (0x5DF510): el tope es 1.0 (la DLL usaba 1000) y un NaN se conserva (fcomp + test ah,5)
        const auto skill = CStats::GetStatValue(eStats::STAT_BIKE_SKILL) / 1000.f;
        return skill > 1.f ? 1.f : skill;
    }
    return IsCreatedByMission() ? 1.f : 0.f;
}

/*!
* @addr 0x5DF560
* @brief Deal with shoulder bone (clavicle) rotation based on arm and breast rotation
*/
void CPed::ShoulderBoneRotation(RpClump* clump) {
    // Como el original: jerarquia de la piel (0x734A40, no la del frame 0x734B10) y matriz = array + indice * 64 sin comprobar el rango
    const auto GetMatrixOf = [hier = GetAnimHierarchyFromSkinClump(clump)](eBoneTag bone) {
        const auto idx = RpHAnimIDGetIndex(hier, bone);
        return &RpHAnimHierarchyGetMatrixArray(hier)[idx];
    };
    const auto DoUpdate = [&](eBoneTag breast, eBoneTag upperArm, eBoneTag clavicle) {
        auto* const breastRwMat = GetMatrixOf(breast);

        // Make the breast's matrix same as the upper arm's
        RwMatrixCopy(breastRwMat, GetMatrixOf(upperArm));

        CMatrix breastMat{ breastRwMat };
        CMatrix clavicleMat{ GetMatrixOf(clavicle) };

        // Calculate breast to clavicle transformation matrix (and store it in breastMat)
        breastMat = Invert(clavicleMat) * breastMat;

        // Half it's X rotation
        float rx, ry, rz;
        breastMat.ConvertToEulerAngles(&rx, &ry, &rz, ORDER_ZYX | SWAP_XZ);
        // Originally there is an `if` check of a static bool value, which is always true.
        rx /= 2.f;
        breastMat.ConvertFromEulerAngles(rx, ry, rz, ORDER_ZYX | SWAP_XZ);

        // Transform it back into it's own space
        breastMat = clavicleMat * breastMat;

        // Finally, update it's RW associated matrix
        breastMat.UpdateRW();
    };
    DoUpdate(eBoneTag::BONE_L_BREAST, eBoneTag::BONE_L_UPPER_ARM, eBoneTag::BONE_L_CLAVICLE);
    DoUpdate(eBoneTag::BONE_R_BREAST, eBoneTag::BONE_R_UPPER_ARM, eBoneTag::BONE_R_CLAVICLE);
}

/*!
* @addr 0x5DF8D0
* @brief Set look timer relative to now, but only if it has expired.
* @param time Time the timer ends relative to now
*/
void CPed::SetLookTimer(uint32 time) {
    if (CTimer::GetTimeInMS() > m_nLookTime) {
        m_nLookTime = CTimer::GetTimeInMS() + time;
    }
}

/*!
* @addr 0x5DF8F0
*/
bool CPed::IsPlayer() const
{
    switch (m_nPedType) {
    case PED_TYPE_PLAYER1:
    case PED_TYPE_PLAYER2:
        return true;
    }
    return false;
}

// Ayudantes de X87Math.cpp (PedAtanNegXYDblCpp / PedAtanNegXYAddDblCpp) que devuelven un double: el fpatan(-x, y) [+ add] queda sin redondear en ST0.
// Aqui se declaran como float (alias del enlazador) para que el llamador reciba ST0 tal cual y lo guarde con fstp, como el EXE
// (mismo mecanismo que PlaceableHeadingFltCpp en Placeable.cpp).
extern "C" float __cdecl PedAtanNegXYFltCpp(float x, float y);
extern "C" float __cdecl PedAtanNegXYAddFltCpp(float x, float y, float add);
#pragma comment(linker, "/alternatename:_PedAtanNegXYFltCpp=_PedAtanNegXYDblCpp")
#pragma comment(linker, "/alternatename:_PedAtanNegXYAddFltCpp=_PedAtanNegXYAddDblCpp")

namespace {
// Como el original: constantes exactas del EXE (las de common.h son imprecisas)
constexpr float PedE_PI      = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
constexpr float PedE_HALF_PI = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4
constexpr float PedE_TWO_PI  = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC

// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE
CVector PedE_Multiply3x3(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// Como el original: matriz * vector (0x59C890) = Multiply3x3 en el orden del EXE + posicion
CVector PedE_TransformPoint(const CMatrix& m, const CVector& v) {
    return {
        (m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x) + m.GetPosition().x,
        (m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y) + m.GetPosition().y,
        (m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// Como el original: rumbo de la entidad (matriz: fpatan; sin matriz: el rumbo guardado) + add
float PedE_HeadingAdd(const CEntity* e, float add) {
    if (const auto* const m = e->m_matrix) {
        return PedAtanNegXYAddFltCpp(m->GetForward().x, m->GetForward().y, add);
    }
    return e->m_placement.m_fHeading + add;
}

// Como el original: matriz del hueso = array de matrices + indice (jerarquia de GetAnimHierarchyFromSkinClump, sin comprobaciones)
RwMatrix* PedE_BoneMatrix(CPed* ped, int32 boneTag) {
    auto* const hierarchy = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
    const auto  index     = RpHAnimIDGetIndex(hierarchy, boneTag);
    return RpHAnimHierarchyGetMatrixArray(hierarchy) + index;
}
} // namespace

/*!
* @addr 0x5DF910
*/
void CPed::SetPedPositionInCar() {
    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }

    // Como el original: sin comprobar bInVehicle (el EXE no lo hace)
    CVector seatLocalPos = GetSeatPositionInVehicle();
    CMatrix vehMat = m_pVehicle->GetMatrix();
    CMatrix tempMat;

    if (m_pVehicle->IsBike()) {
        auto* bike = (CBike*)m_pVehicle;
        bike->CalculateLeanMatrix();
        vehMat = bike->m_mLeanMatrix;
    } else if (m_pVehicle->GetModelIndex() == MODEL_COMBINE) { // 532 ?
        float chassisPosZ = 0.0f;
        auto  autoMobile  = (CAutomobile*)m_pVehicle;

        if (autoMobile->m_aCarNodes[CAR_CHASSIS]) {
            tempMat.Attach(RwFrameGetMatrix(autoMobile->m_aCarNodes[CAR_CHASSIS]), false);
            chassisPosZ = tempMat.GetPosition().z;
            tempMat.Detach();
        }

        tempMat.SetTranslate({ 0.0f, 0.0f, -chassisPosZ });
        tempMat.RotateY(autoMobile->m_fDoomVerticalRotation); // AKA GunOrientation ?
        // Como el original: solo suma chassisPosZ a la z (RotateY ya movio la posicion; no se pone (0, 0, z))
        tempMat.GetPosition().z += chassisPosZ;
        vehMat *= tempMat;
    }

    vehMat.GetPosition() += PedE_Multiply3x3(vehMat, seatLocalPos);
    tempMat.SetUnity();

    // Como el original: AssocGroupId = ANIM_GROUP_STDCARAMIMS + grupo del handling; el grupo 13 es 0x65 (antes se comparaba con 13)
    if (static_cast<int32>(m_pVehicle->m_pHandlingData->GetAnimGroupId()) == 0x65) {
        if (this == m_pVehicle->m_apPassengers[1]) {
            // Como el original: fpatan - pi/2 sin redondear el arcotangente
            m_fCurrentRotation = PedE_HeadingAdd(m_pVehicle, -PedE_HALF_PI);
            tempMat.SetTranslate({ 0.0f, 0.0f, 0.0f });
            tempMat.RotateZ(-PedE_HALF_PI);
            tempMat.GetPosition().y += 0.6f;
            vehMat *= tempMat;
        } else if (this == m_pVehicle->m_apPassengers[2]) {
            m_fCurrentRotation = PedE_HeadingAdd(m_pVehicle, PedE_HALF_PI);
            tempMat.SetTranslate({ 0.0f, 0.0f, 0.0f });
            tempMat.RotateZ(PedE_HALF_PI);
            vehMat *= tempMat;
        } else {
            m_fCurrentRotation = m_pVehicle->GetHeading();
        }
    } else {
        m_fCurrentRotation = m_pVehicle->GetHeading();
    }

    m_fAimingRotation = m_fCurrentRotation;
    SetMatrix(vehMat);
}

/*!
* @addr 0x5DFD60
* @brief Set head changing rate to value stored in m_pStats
*/
void CPed::RestoreHeadingRate() {
    m_fHeadingChangeRate = m_pStats->m_fHeadingChangeRate;
}

/*!
* @addr 0x5DFD70
*/
void CPed::RestoreHeadingRateCB(CAnimBlendAssociation* assoc, void* data) {
    UNUSED(assoc);

    auto& ped = *((CPed*)data);
    ped.m_fHeadingChangeRate = ped.m_pStats->m_fHeadingChangeRate;
}

/*!
* @addr 0x5DFD90
* @brief Set random radio station if ped is in car. The station is chosen randomly, and is either `m_nRadio1` or `m_nRadio2` from the ped's CPedModelInfo
*/
void CPed::SetRadioStation()
{
    if (IsPlayer() || !m_pVehicle)
        return;

    if (m_pVehicle->m_pDriver == this) {
        const auto& mi = *(CPedModelInfo*)GetModelInfo();
        // Como el original (0x5DFDC5): cmp rand, 0x3FFF / jge -> m_nRadio2 (con 0x3FFF exacto sale la segunda emisora)
        m_pVehicle->m_vehicleAudio.m_AuSettings.RadioStation = (CGeneral::GetRandomNumber() < RAND_MAX / 2) ? mi.m_nRadio1 : mi.m_nRadio2;
    }
}

/*!
* @addr 0x5DFDF0
*/
void CPed::PositionAttachedPed() {
    if (!m_pAttachedTo) {
        return;
    }

    CMatrix attachedMatrix;
    auto offset = m_vecTurretOffset;
    if (m_pAttachedTo->m_nModelIndex == MODEL_FIRELA
        && static_cast<CAutomobile*>(m_pAttachedTo)->m_aCarNodes[CAR_MISC_B]
        && offset.z < -900.0f
    ) {
        attachedMatrix.Attach(RwFrameGetLTM(static_cast<CAutomobile*>(m_pAttachedTo)->m_aCarNodes[CAR_MISC_B]), false);
        attachedMatrix.Detach();
        offset.z += 1000.0f;
    } else {
        attachedMatrix = m_pAttachedTo->GetMatrix();
    }
    // Como el original: Multiply3x3 (0x59C790) en el orden de sumas del EXE
    attachedMatrix.GetPosition() += PedE_Multiply3x3(attachedMatrix, offset);
    // Como el original: fpatan a precision completa
    const auto attachedHeading = PedAtanNegXYFltCpp(attachedMatrix.GetForward().x, attachedMatrix.GetForward().y);

    if (!IsPlayer()) {
        auto heading = attachedHeading;
        switch (m_fTurretAngleA) {
        case 1: heading += PedE_HALF_PI; break;
        case 2: heading += PedE_PI; break;
        case 3: heading -= PedE_HALF_PI; break;
        }
        heading = CGeneral::LimitRadianAngle(heading);
        m_fCurrentRotation = CGeneral::LimitRadianAngle(m_fCurrentRotation);
        auto difference = m_fCurrentRotation - heading;
        if (difference > PedE_PI) {
            difference -= PedE_TWO_PI;
        } else if (difference < -PedE_PI) {
            difference += PedE_TWO_PI;
        }
        if (difference > m_fTurretAngleB) {
            m_fCurrentRotation = heading + m_fTurretAngleB;
        } else if (difference < -m_fTurretAngleB) {
            m_fCurrentRotation = heading - m_fTurretAngleB;
        }
        m_fCurrentRotation = CGeneral::LimitRadianAngle(m_fCurrentRotation);
    }

    CMatrix rotation;
    rotation.SetRotateZ(m_fCurrentRotation - attachedHeading);
    attachedMatrix *= rotation;
    SetMatrix(attachedMatrix);
    if (m_pAttachedTo->GetIsTypeVehicle() || m_pAttachedTo->GetIsTypeObject()) {
        m_vecMoveSpeed = m_pAttachedTo->m_vecMoveSpeed;
        m_vecTurnSpeed = m_pAttachedTo->m_vecTurnSpeed;
    }
    m_standingOnEntity = nullptr;
    bIsStanding = true;
}

/*!
* @addr 0x5E00F0
* @brief Remove ped from the world, and request special model for it.
*/
void CPed::Undress(char* modelName) {
    // Como el original: lee el modelo (movsx) antes de DeleteRwObject
    const int32 modelIndex = (int32)(int16)m_nModelIndex;
    DeleteRwObject();
    CStreaming::RequestSpecialModel(IsPlayer() ? 0 : modelIndex, modelName, STREAMING_KEEP_IN_MEMORY | STREAMING_MISSION_REQUIRED);
    CWorld::Remove(this);
}

/*!
* @addr 0x5E0130
* @brief Re-add ped to the world
*/
void CPed::Dress() {
    // Como el original: m_nModelIndex se pone a -1 antes de SetModelIndex y el estado se escribe directo (sin SetPedState)
    const int32 modelIndex = (int32)(int16)m_nModelIndex;
    m_nModelIndex = (uint16)-1;
    SetModelIndex(modelIndex);
    if (m_nPedState != PEDSTATE_DRIVING) {
        m_nPedState = PEDSTATE_IDLE;
    }
    CWorld::Add(this);
    RestoreHeadingRate();
}

/*!
* @addr 0x5E0170
* @brief Checks if the Pedestrian is still alive.
*/
bool CPed::IsAlive() const {
    switch (m_nPedState) {
    case PEDSTATE_DIE:
    case PEDSTATE_DEAD:
        return false;
    }
    return true;
}

/*!
* @addr 0x5E01A0
*/
void CPed::UpdateStatEnteringVehicle()
{
    // NOP
}

/*!
* @addr 0x5E01B0
*/
void CPed::UpdateStatLeavingVehicle()
{
    // NOP
}

/*!
* @addr 0x5E0270
* @brief Release current cover point
*/
void CPed::ReleaseCoverPoint() {
    if (m_pCoverPoint) {
        m_pCoverPoint->ReleaseCoverPointForPed(this);
        m_pCoverPoint = nullptr;
    }
}

/*!
* @addr 0x5E0290
* @returns Any active task of type HOLD_ENTITY, PICKUP_ENTITY, PUTDOWN_ENTITY.
*/
CTaskSimpleHoldEntity* CPed::GetHoldingTask() {
    // Man programming in C++03 must've been a pain.. if, if, if, if, if, if... IF.
    if (const auto task = GetTaskManager().FindActiveTaskFromList({ TASK_SIMPLE_HOLD_ENTITY, TASK_SIMPLE_PICKUP_ENTITY, TASK_SIMPLE_PUTDOWN_ENTITY })) {
        return static_cast<CTaskSimpleHoldEntity*>(task);
    }
    return nullptr;
}

/*!
* @addr 0x5E02E0
*/
CEntity* CPed::GetEntityThatThisPedIsHolding()
{
    if (const auto task = GetHoldingTask()) {
        return task->m_pEntityToHold;
    }

    if (const auto task = GetTaskManager().Find<CTaskComplexGoPickUpEntity>()) {
        return task->m_pEntity;
    }

    return nullptr;
}

/*!
* @addr  0x5E0360
* @brief Drop held entity, possibly deleting it.
*/
void CPed::DropEntityThatThisPedIsHolding(bool bDeleteHeldEntity) {
    if (const auto task = GetHoldingTask()) {
        // Como el original: la entidad sujeta se lee ANTES de soltarla (DropEntity puede borrar la referencia)
        const auto heldEntity = task->m_pEntityToHold;

        // Drop the entity
        task->DropEntity(this, true);

        // Delete held entity (If any)
        if (heldEntity && bDeleteHeldEntity) {
            {
                // Como el original: solo el tipo 2 (OBJECT_MISSION) se conserva; IsMissionObject() tambien acepta el 6
                if (!heldEntity->GetIsTypeObject() || heldEntity->AsObject()->m_nObjectType != OBJECT_MISSION) {
                    heldEntity->DeleteRwObject(); // TODO; Are these 3 lines inlined?
                    CWorld::Remove(heldEntity);
                    delete heldEntity;
                }
            }
        }
    }
}

/*!
* @addr 0x5E0400
* @returns If there's a holding task and the held entity can be thrown.
*/
bool CPed::CanThrowEntityThatThisPedIsHolding() {
    if (const auto task = GetHoldingTask()) {
        return task->CanThrowEntity();
    }
    return false;
}

/*!
* @addr 0x5E0460
* @returns If there's a HANDSIGNAL task
*/
bool CPed::IsPlayingHandSignal() {
    return GetTaskManager().HasAnyOf<TASK_COMPLEX_HANDSIGNAL_ANIM>();
}

/*!
* @addr 0x5E0480
* @brief Stop the HANDSINGAL task
*/
void CPed::StopPlayingHandSignal() {
    if (const auto task = GetTaskManager().Find<TASK_COMPLEX_HANDSIGNAL_ANIM>()) {
        task->MakeAbortable(this);
    }
}

/*!
* @addr 0x5E04B0
* @returns Get walk speed in units/s based on the ped's anim group's WALK anim.
*/
float CPed::GetWalkAnimSpeed() {
    auto hier = CAnimManager::GetAnimAssociation(m_nAnimGroup, ANIM_ID_WALK)->m_BlendHier;

    CAnimManager::UncompressAnimation(hier);
    auto* const seq = &hier->m_pSequences[ANIM_ID_WALK];

    // Como el original: el numero de frames se lee con signo (movsx / jle)
    if ((int16)seq->m_FramesNum <= 0) {
        return 0.f; // No frames
    }

    // NOTE: This is quite garbage, based on at least 5 assumptions, more of a hack than a solution from R*'s side.
    //       It won't work correctly if first frame has no translation, nor if the animation happens on any other axis than Y, etc..

    // Como el original: con traslacion se lee Trans.y (+0x18) del ultimo frame (32 bytes); sin traslacion se lee a +0x18 de
    // un frame de 20 bytes, es decir el imag.y del frame siguiente al ultimo. Siempre se resta el +0x18 del primero.
    const auto* const frames = static_cast<const uint8*>(seq->m_Frames);
    const auto lastIdx = (int32)(int16)seq->m_FramesNum - 1;
    const float lastFrameY = seq->HasTranslation()
        ? *reinterpret_cast<const float*>(frames + (lastIdx << 5) + 0x18)
        : *reinterpret_cast<const float*>(frames + lastIdx * 20 + 0x18);
    const float firstFrameY = *reinterpret_cast<const float*>(frames + 0x18);
    return (lastFrameY - firstFrameY) / hier->m_fTotalTime;
}

/*!
* @addr 0x5E06E0
*/
void CPed::SetPedDefaultDecisionMaker() {
    if (!IsPlayer()) {
        GetIntelligence()->SetPedDecisionMakerType(IsCreatedByMission() ? -1 : m_pStats->m_nDefaultDecisionMaker);
    } else {
        GetIntelligence()->SetPedDecisionMakerType(-2);
    }
}

/*!
* @addr 0x5E0730
* @returns If entity is a given range angle relative to our current rotation given by limitAngle [-limitAngle, limitAngle]
*/
bool CPed::CanSeeEntity(CEntity* entity, float limitAngle) {
    // Como el original: llevar el angulo a [0, 2pi] con las comparaciones del EXE (<= 2pi resta solo si es mayor; NaN no cambia)
    const auto FixRadianAngle = [](float angle) {
        if (angle > PedE_TWO_PI) {
            return angle - PedE_TWO_PI;
        }
        if (angle < 0.f) {
            return angle + PedE_TWO_PI;
        }
        return angle;
    };

    // Como el original: GetAngleBetweenPoints (0x53CEA0, grados) * 0.01745329238474369 (0x8595EC), no el angulo en radianes directo
    const auto pointAngle = FixRadianAngle(
        CGeneral::GetAngleBetweenPoints(entity->GetPosition().x, entity->GetPosition().y, GetPosition().x, GetPosition().y)
        * std::bit_cast<float>(0x3C8EFA35u)
    );

    // Como el original: tambien se lleva m_fCurrentRotation a [0, 2pi] (sin guardarlo) y se toma |pointAngle - rotacion|
    const auto currentRotation = FixRadianAngle(m_fCurrentRotation);
    auto       delta           = pointAngle - currentRotation;
    if (delta < 0.f) {
        delta = -delta;
    }

    return delta < limitAngle || delta > PedE_TWO_PI - limitAngle;
}

/*!
* @addr 0x5E0820
*/
bool CPed::PositionPedOutOfCollision(int32 exitDoor, CVehicle* vehicle, bool findClosestNode) {
    if (!vehicle && !(vehicle = m_pVehicle)) {
        return false;
    }
    if (bDonePositionOutOfCollision) {
        return true;
    }
    const auto& box = vehicle->GetColModel()->m_boundBox;
    const auto origin = vehicle->GetPosition();
    const auto oldPosition = GetPosition();
    CVector position = oldPosition;
    const bool usesCollision = m_bUsesCollision;
    CWorld::pIgnoreEntity = vehicle;
    physicalFlags.bForceHitReturnFalse = true;
    m_bUsesCollision = false;
    m_vecMoveSpeed = CVector{};

    const auto TryPosition = [&](const CVector& point) {
        position = point;
        SetPosn(position);
        return !CheckCollision() && CWorld::GetIsLineOfSightClear(origin, position, true, false, false, true, false, false, false);
    };
    bool found = false;
    if (vehicle->IsOnItsSide() && !vehicle->IsBike()) {
        found = TryPosition(origin + CVector{0.0f, 0.0f, box.m_vecMax.x + 1.0f});
    } else if (exitDoor) {
        found = TryPosition(CCarEnterExit::GetPositionToOpenCarDoor(vehicle, exitDoor));
        if (!found && vehicle->IsBike() && (exitDoor == 10 || exitDoor == 11)) {
            found = TryPosition(CCarEnterExit::GetPositionToOpenCarDoor(vehicle, exitDoor == 11 ? 9 : 8));
        }
    }
    const auto& right = vehicle->GetRight();
    const auto& forward = vehicle->GetForward();
    const float side = exitDoor == 8 || exitDoor == 9
        ? box.m_vecMax.x + 0.355f
        : box.m_vecMin.x - 0.355f + std::abs(right.z) * std::max(-box.m_vecMin.z, box.m_vecMax.z);
    if (!found) {
        const auto delta = oldPosition - origin;
        const float offset = side - ((delta.z * right.z + delta.y * right.y) + delta.x * right.x);
        found = TryPosition(oldPosition + right * offset);
    }
    const float step = (box.m_vecMax.y - box.m_vecMin.y) * (1.0f / 3.0f);
    for (int32 i = 0; i < 4 && !found; ++i) {
        found = TryPosition((origin + right * side) + forward * (box.m_vecMin.y + float(i) * step));
    }
    if (!found) found = TryPosition(origin + forward * (box.m_vecMin.y - 0.355f));
    if (!found) found = TryPosition(origin + forward * (box.m_vecMax.y + 0.355f));
    if (!found) found = TryPosition((origin - right * side) + forward * box.m_vecMin.y);
    if (!found) found = TryPosition((origin - right * side) + forward * box.m_vecMax.y);
    if (!found && vehicle->IsAutomobile()) {
        auto roof = origin + vehicle->GetUp() * box.m_vecMax.z;
        roof.z += 1.0f;
        found = TryPosition(roof);
    }
    CWorld::pIgnoreEntity = nullptr;
    physicalFlags.bForceHitReturnFalse = false;
    m_bUsesCollision = usesCollision;
    if (found) {
        bDonePositionOutOfCollision = true;
        m_vecMoveSpeed = m_vecTurnSpeed = CVector{};
        if (!vehicle->IsBike() || findClosestNode) {
            vehicle->m_vecMoveSpeed = vehicle->m_vecTurnSpeed = CVector{};
            vehicle->m_vecMoveSpeed.z -= 0.05f;
        }
        return true;
    }
    if (!findClosestNode) {
        return false;
    }
    const auto pedNode = ThePaths.FindNodeClosestToCoors(origin, PATH_TYPE_PED, 999999.875f, 0, 0, 0, 0, 0);
    const auto carNode = ThePaths.FindNodeClosestToCoors(origin, PATH_TYPE_VEH, 999999.875f, 0, 0, 0, 0, 0);
    if (pedNode.IsValid()) {
        position = ThePaths.GetPathNode(pedNode)->GetPosition();
    }
    if (carNode.IsValid()) {
        const auto carPosition = ThePaths.GetPathNode(carNode)->GetPosition();
        if ((position - origin).Magnitude2D() > (carPosition - origin).Magnitude2D()) {
            position = carPosition;
        }
    } else if (!pedNode.IsValid()) {
        return false;
    }
    position = CPedPlacement::FindZCoorForPed(position).first;
    SetPosn(position);
    SetHeading(vehicle->GetHeading());
    bDonePositionOutOfCollision = true;
    m_vecMoveSpeed = m_vecTurnSpeed = CVector{};
    vehicle->m_vecMoveSpeed = vehicle->m_vecTurnSpeed = CVector{};
    vehicle->m_vecMoveSpeed.z += 0.02f;
    return true;
}

/*!
* @addr     0x5E13C0
* @brief    Teleport ped to the furthest point from here that isn't colliding with a building or ped.
* @returns  If there was at lest one such point.
*/
bool CPed::PositionAnyPedOutOfCollision() {
    // Como el original (0x5E13C0): rejilla de 15x15 puntos con paso 0,5 alrededor del peaton (de -3,5 a +3,5 en x e y; z fija).
    // Se descartan los puntos que chocan con edificios u objetos. De los demas se guarda el MAS CERCANO (distancia al cuadrado
    // truncada a entero, umbral inicial 999): uno que no choca con vehiculos y otro que si.
    const CVector pos = GetPosition();
    int32 bestFree = 999, bestVehicle = 999;
    bool  freeFound = false, vehicleFound = false;
    CVector freePoint, vehiclePoint;
    CEntity* lastVehicleTest = reinterpret_cast<CEntity*>(0xF); // Como el original: edi vale 0xF hasta la primera prueba con vehiculos

    CVector testPoint;
    testPoint.y = pos.y - 3.5f;
    testPoint.z = pos.z;
    for (auto y = 0; y < 15; y++) {
        testPoint.x = pos.x - 3.5f;
        for (auto x = 0; x < 15; x++) {
            // Como el original: antes de cada prueba FindZCoorForPed (0x616920) corrige la z del punto en el sitio (y se arrastra al siguiente)
            testPoint = CPedPlacement::FindZCoorForPed(testPoint).first;
            if (!CWorld::TestSphereAgainstWorld(testPoint, 0.6f, this, /*buildings: */true, false, false, /*objects: */true, false, false)) {
                // Como el original: (dz^2 + dy^2) + dx^2
                const auto dx = testPoint.x - pos.x;
                const auto dy = testPoint.y - pos.y;
                const auto dz = testPoint.z - pos.z;
                const auto distSq = (dz * dz + dy * dy) + dx * dx;
                lastVehicleTest = CWorld::TestSphereAgainstWorld(testPoint, 0.6f, this, false, /*vehicles: */true, false, false, false, false);
                if (!lastVehicleTest) {
                    if (static_cast<float>(bestFree) > distSq) {
                        bestFree  = static_cast<int32>(distSq);
                        freePoint = testPoint;
                        freeFound = true;
                    }
                } else {
                    if (static_cast<float>(bestVehicle) > distSq) {
                        bestVehicle  = static_cast<int32>(distSq);
                        vehiclePoint = testPoint;
                        vehicleFound = true;
                    }
                }
            }
            testPoint.x += 0.5f;
        }
        testPoint.y += 0.5f;
    }

    // Set our position to one of the points
    if (freeFound) { // If there was a point not colliding with a vehicle use it
        SetPosn(freePoint);
        return true;
    }
    if (vehicleFound) { // Otherwise use the one colliding with a vehicle (z + altura maxima del modelo de la ULTIMA entidad probada, como el original)
        const auto maxZ = lastVehicleTest->GetColModel()->m_boundBox.m_vecMax.z;
        SetPosn(vehiclePoint.x, vehiclePoint.y, vehiclePoint.z + maxZ);
        return true;
    }
    return false;
}

/*!
* @addr 0x5E1660
*/
bool CPed::OurPedCanSeeThisEntity(CEntity* entity, bool isSpotted) {
    if (!isSpotted) {
        const auto dir2D{ entity->GetPosition2D() - GetPosition2D() };
        // Como el original: Magnitude (sqrt) >= 40 (NaN tambien descarta), no el cuadrado
        if (   DotProduct2D(dir2D, m_matrix->GetForward()) < 0.f // Is behind us
            || !(std::sqrt(dir2D.y * dir2D.y + dir2D.x * dir2D.x) < 40.f)
        ) {
            return false;
        }
    }

    // Como el original: el origen del rayo es nuestra posicion con z + 1
    auto origin{GetPosition()};
    origin.z += 1.f;
    auto target{entity->GetPosition()};
    if (entity->GetIsTypePed()) {
        target.z += 1.f; // Adjust for head pos?
    }

    // Seems like they explicitly use this one instead of `IsLineOfSightClear` because of the `shootThru` check.
    // Como el original: el CColPoint y la entidad quedan sin inicializar
    CColPoint cp;
    CEntity* hitEntity;
    return !CWorld::ProcessLineOfSight(origin, target, cp, hitEntity, true, false, false, isSpotted, false, false, false, isSpotted);
}

/*!
* @addr 0x5E17E0
* @unused
*/
void CPed::SortPeds(CPed** pedList, int32 arg1, int32 arg2)
{
    ((void(__thiscall *)(CPed*, CPed**, int32, int32))0x5E17E0)(this, pedList, arg1, arg2);
}

/*!
* @addr 0x5E1A00
* @brief Update `m_fFPSMoveHeading` depending on the ped's Up/Down, Left/Right control states.
*/
float CPed::WorkOutHeadingForMovingFirstPerson(float heading) {
    if (!IsPlayer() || !GetPlayerData()) {
        return 0.f; // Probably shouldn't ever happen, but okay
    }

    // Como el original: primero se lee izquierda/derecha (0x53FC90) y despues arriba/abajo (0x53FD30)
    const auto walkLeftRight = (float)CPad::GetPad()->GetPedWalkLeftRight();
    const auto walkUpDown = (float)CPad::GetPad()->GetPedWalkUpDown();
    if (walkUpDown == 0.f) {
        if (walkLeftRight != 0.f) {
            // Como el original: pi/2 exacto del EXE (0x3FC90FDB), no el HALF_PI impreciso de common.h
            GetPlayerData()->m_fFPSMoveHeading = std::bit_cast<float>(walkLeftRight < 0.f ? 0x3FC90FDBu : 0xBFC90FDBu);
        }
    } else {
        GetPlayerData()->m_fFPSMoveHeading = CGeneral::GetRadianAngleBetweenPoints(0.f, 0.f, -walkLeftRight, walkUpDown);
    }

    return CGeneral::LimitRadianAngle(heading + GetPlayerData()->m_fFPSMoveHeading);
}

/*!
* @addr 0x5E1B10
*/
void CPed::UpdatePosition() {
    if (CReplay::Mode == MODE_PLAYBACK) {
        return;
    }
    if (!bIsStanding) {
        const auto task = GetTaskManager().m_aPrimaryTasks[TASK_PRIMARY_PRIMARY];
        if (GetIntelligence()->GetTaskSwim() || GetIntelligence()->GetTaskJetPack()
            || (task && task->GetTaskType() == TASK_COMPLEX_USE_SWAT_ROPE)
        ) {
            SetHeading(m_fCurrentRotation);
        }
        return;
    }
    if (m_pAttachedTo) {
        return;
    }
    SetHeading(m_fCurrentRotation);

    const auto timeStep = CTimer::GetTimeStep();
    auto* const standingOn = static_cast<CPhysical*>(m_standingOnEntity);
    CVector2D change;
    if (standingOn) {
        CVector contactSpeed;
        if (!IsPlayer() && standingOn->GetIsTypeVehicle() && standingOn->AsVehicle()->IsBoat()) {
            // Como el original: (pos.x, pos.y, pos.z - 1) - posicion del suelo (la resta de 1 va antes)
            auto relative = CVector{GetPosition().x, GetPosition().y, GetPosition().z - 1.0f};
            relative -= standingOn->GetPosition();
            contactSpeed = standingOn->m_vecMoveSpeed + CrossProduct(standingOn->m_vecTurnSpeed, relative);
            contactSpeed += (-standingOn->m_vecTurnSpeed.SquaredMagnitude() * relative) * timeStep;
            m_vecMoveSpeed.z = contactSpeed.z;
        } else {
            contactSpeed = standingOn->GetSpeed(field_56C);
        }
        change = CVector2D{contactSpeed.x + m_vecAnimMovingShift.x - m_vecMoveSpeed.x,
                           contactSpeed.y + m_vecAnimMovingShift.y - m_vecMoveSpeed.y};
        m_fCurrentRotation += timeStep * standingOn->m_vecTurnSpeed.z;
        m_fAimingRotation += timeStep * standingOn->m_vecTurnSpeed.z;
    } else if (g_surfaceInfos.IsSteepSlope(m_nContactSurface) && (field_578.x != 0.0f || field_578.y != 0.0f)) {
        CVector2D normal{field_578.x, field_578.y};
        // Como el original (0x44E480): con |n|^2 == 0 solo pone x = 1; si no, multiplica por 1 / sqrt (NaN entra por esa rama)
        if (const auto squared = normal.x * normal.x + normal.y * normal.y; squared == 0.0f) {
            normal.x = 1.0f;
        } else {
            const auto reciprocal = 1.0f / std::sqrt(squared);
            normal.x *= reciprocal;
            normal.y *= reciprocal;
        }
        m_vecMoveSpeed = CVector{0.0f, 0.0f, -0.001f};
        change = m_vecAnimMovingShift + normal * 0.02f;
        const auto projection = normal.Dot(change);
        if (projection < 0.0f) {
            change -= normal * projection;
        }
    } else {
        change = m_vecAnimMovingShift - CVector2D{m_vecMoveSpeed.x, m_vecMoveSpeed.y};
    }

    if (standingOn && !standingOn->m_pAttachedTo
        && (!standingOn->physicalFlags.bDisableCollisionForce || standingOn->physicalFlags.bCollidable)
    ) {
        const auto magnitude = change.Magnitude();
        auto limit = magnitude;
        if (!standingOn->GetIsTypeVehicle()) {
            limit = timeStep * 0.01f;
        } else if (m_nPedState == PEDSTATE_DIE) {
            limit = timeStep * 0.002f;
        } else if (standingOn->AsVehicle()->IsBike() && standingOn->m_vecMoveSpeed.SquaredMagnitude() > sq(0.2f)) {
            limit = timeStep * 0.0002f;
        } else if (standingOn->AsVehicle()->IsAutomobile()) {
            limit = timeStep * 0.01f;
        }
        if (magnitude > limit) {
            change *= limit / magnitude;
        }
    } else if (!standingOn && bFallenDown) {
        const auto magnitude = change.Magnitude();
        const auto limit = timeStep * 0.01f;
        if (magnitude > limit) {
            change *= limit / magnitude;
        }
    }
    m_vecMoveSpeed.x += change.x;
    m_vecMoveSpeed.y += change.y;
}

/*!
* @addr 0x5E1FA0
*/
void CPed::ProcessBuoyancy()
{
    if (bInVehicle)
        return;

    // Como el original: (multiplicador * masa) * 0,008 (0x863984), no entre 125; 1,1 (0x858F14) o 1,8 (0x859BB4) si está muerto
    float fBuoyancy = 1.1F;
    if (m_nPedState == PEDSTATE_DEAD || m_nPedState == PEDSTATE_DIE)
        fBuoyancy = 1.8F;
    fBuoyancy = fBuoyancy * m_fMass;
    fBuoyancy = fBuoyancy * 0.008F;

    CVector vecBuoyancyTurnPoint;
    CVector vecBuoyancyForce;
    if (!mod_Buoyancy.ProcessBuoyancy(this, fBuoyancy, &vecBuoyancyTurnPoint, &vecBuoyancyForce)) {
        physicalFlags.bTouchingWater = false;
        // Como el original: GetTaskSwim lleva el cast inline y la tarea se vuelve a pedir con GetSimplestActiveTask
        if (GetIntelligence()->GetTaskSwim()) {
            static_cast<CTaskSimpleSwim*>(GetIntelligence()->GetTaskManager().GetSimplestActiveTask())->m_fSwimStopTime = 1000.0F;
        }

        return;
    }

    if (bIsStanding) {
        auto& standingOnEntity = m_pContactEntity;
        if (standingOnEntity && standingOnEntity->GetIsTypeVehicle()) {
            auto pStandingOnVehicle = standingOnEntity->AsVehicle();
            if (pStandingOnVehicle->IsBoat() && !pStandingOnVehicle->physicalFlags.bRenderScorched) {
                physicalFlags.bSubmergedInWater = false;
                if (!GetIntelligence()->GetTaskSwim())
                    return;

                auto* const swimTask = static_cast<CTaskSimpleSwim*>(GetIntelligence()->GetTaskManager().GetSimplestActiveTask());
                swimTask->m_fSwimStopTime = CTimer::GetTimeStep() + swimTask->m_fSwimStopTime;
                return;
            }
        }
    }

    if (GetPlayerData()) {
        const CVector vecPedPos = GetPosition();
        float fCheckZ = vecPedPos.z - 3.0F;
        CColPoint lineColPoint;
        CEntity* colEntity;
        if (CWorld::ProcessVerticalLine(vecPedPos, fCheckZ, lineColPoint, colEntity, false, true, false, false, false, false, nullptr)) {
            if (colEntity->GetIsTypeVehicle()) {
                auto colVehicle = colEntity->AsVehicle();
                if (colVehicle->IsBoat()
                    && !colVehicle->physicalFlags.bRenderScorched
                    && colVehicle->GetMatrix().GetUp().z > 0.0F) {

                    physicalFlags.bSubmergedInWater = false;
                    return;
                }
            }
        }
    }

    // Como el original: se piden los tres colores ambientales (sin usarlos) y se consume un número aleatorio
    CGeneral::GetRandomNumber();

    // Add splash particle if it's the first frame we're touching water, and
    // the movement of ped is downward, preventing particles from being created
    // if ped is standing still and water wave touches him
    if (!physicalFlags.bTouchingWater && m_vecMoveSpeed.z < -0.01F) {
        // Como el original: ((velocidad * paso) * 4) + posición, componente a componente
        const float ts = CTimer::GetTimeStep();
        const float mz = (ts * m_vecMoveSpeed.z) * 4.0F;
        const float my = (ts * m_vecMoveSpeed.y) * 4.0F;
        const float mx = (ts * m_vecMoveSpeed.x) * 4.0F;
        const CVector pedPos = GetPosition();
        CVector vecSplashPos;
        vecSplashPos.z = mz + pedPos.z;
        vecSplashPos.y = my + pedPos.y;
        vecSplashPos.x = mx + pedPos.x;
        float fWaterZ;
        if (CWaterLevel::GetWaterLevel(vecSplashPos, fWaterZ, true, nullptr)) {
            vecSplashPos.z = fWaterZ;
            g_fx.TriggerWaterSplash(vecSplashPos);
            AudioEngine.ReportWaterSplash(this, -100.0F, true);
        }
    }

    physicalFlags.bTouchingWater = true;
    physicalFlags.bSubmergedInWater = true;
    ApplyMoveForce(vecBuoyancyForce);

    // Como el original: paso * 0,008 (no entre 125) y comparaciones estrictas
    const float stepTerm = CTimer::GetTimeStep() * 0.008F;
    const float accelTerm = vecBuoyancyForce.z / m_fMass;
    if (stepTerm < accelTerm
        || GetPosition().z + 0.6F < mod_Buoyancy.m_fWaterLevel) {

        bool bPlayerSwimmingOrClimbing = false;
        if (!IsPlayer()) {
            // Como el original: las banderas se escriben antes de crear el evento
            bIsStanding = false;
            bIsDrowning = true;
            CEventInWater cEvent(0.75F);
            GetEventGroup().Add(&cEvent, false);
        }
        else {
            if (GetIntelligence()->GetTaskSwim()) {
                static_cast<CTaskSimpleSwim*>(GetIntelligence()->GetTaskManager().GetSimplestActiveTask())->m_fSwimStopTime = 0.0F;
                bPlayerSwimmingOrClimbing = true;
            }
            else if (GetIntelligence()->GetTaskManager().GetSimplestActiveTask()
                && GetIntelligence()->GetTaskManager().GetSimplestActiveTask()->GetTaskType() == TASK_SIMPLE_CLIMB) {
                // Como el original: GetTaskClimb inline (dos GetSimplestActiveTask y GetTaskType virtual)
                bPlayerSwimmingOrClimbing = true;
            }
            else {
                float fDen = CTimer::GetTimeStep() * m_fMass;
                fDen = fDen * 0.008F;
                const float fAcceleration = vecBuoyancyForce.z / fDen;
                CEventInWater cEvent(fAcceleration);
                GetEventGroup().Add(&cEvent, false);
            }

            bIsStanding = false;
            bIsDrowning = true;
            if (bPlayerSwimmingOrClimbing)
                return;
        }

        // Como el original: pow(0,9, paso) con el pow del CRT del EXE (0x822130, precisión x87 completa) y producto a float;
        // x e y siempre, z solo si es negativa (fcomp ordenado: con NaN no se multiplica). PowScaleVector3Cpp calcula los tres
        // productos con el mismo factor sin redondear (X87Math.cpp) sobre una copia, y z solo se vuelca si cumple la condición.
        CVector damped = m_vecMoveSpeed;
        PowScaleVector3Cpp(&damped.x, 0.9F, CTimer::GetTimeStep()); // 0x86C308 = (double)0.9F
        m_vecMoveSpeed.x = damped.x;
        m_vecMoveSpeed.y = damped.y;
        if (m_vecMoveSpeed.z < 0.0F) {
            m_vecMoveSpeed.z = damped.z;
        }

        return;
    }

    if (bIsStanding && GetIntelligence()->GetTaskSwim()) {
        auto* const swimTask = static_cast<CTaskSimpleSwim*>(GetIntelligence()->GetTaskManager().GetSimplestActiveTask());
        swimTask->m_fSwimStopTime = CTimer::GetTimeStep() + swimTask->m_fSwimStopTime;
        return;
    }

    if (GetPlayerData()) {
        CVector vecHeadPos(0.0F, 0.0F, 0.1F);
        GetTransformedBonePosition(vecHeadPos, eBoneTag::BONE_HEAD, false);
        if (vecHeadPos.z < mod_Buoyancy.m_fWaterLevel) {
            AsPlayer()->HandlePlayerBreath(true, 1.0F);
        }
    }
}

/*!
* @addr 0x5E3960
* @returns If ped can be moved, that is: not in the air or landing, is not arrested/dead/dying.
*/
bool CPed::IsPedInControl() const
{
    return !bIsLanding && !bIsInTheAir && IsAlive() && m_nPedState != PEDSTATE_ARRESTED;
}

/*!
* @addr 0x5E3990
* @brief Remove current weapon's object model. modelIndex should be the same as with which the object was created with.
*/
void CPed::RemoveWeaponModel(int32 modelIndex) {

    // For players remove any attached FX (Created in `AddWeaponModel` for molotov)
    if (IsPlayer()) {
        auto& activeWep = GetActiveWeapon();
        if (activeWep.m_FxSystem) {
            g_fxMan.DestroyFxSystem(activeWep.m_FxSystem);
            activeWep.m_FxSystem = nullptr;
        }
    }

    // Deal with weapon's loaded clump (if any)
    if (m_pWeaponObject) {
        if (   modelIndex == MODEL_INVALID
            || CModelInfo::GetModelInfo(modelIndex) == CVisibilityPlugins::GetClumpModelInfo(m_pWeaponObject)
        ) {
            // Release model info
            CVisibilityPlugins::GetClumpModelInfo(m_pWeaponObject)->RemoveRef();

            // Como el original: el EXE siempre quita la animacion de piel del clump del arma (sin SA_SKINNED_PEDS)
            if (IsClumpSkinned(m_pWeaponObject)) {
                RpClumpForAllAtomics(m_pWeaponObject, AtomicRemoveAnimFromSkinCB, nullptr);
            }

            // Destroy clump
            RpClumpDestroy(m_pWeaponObject);
            m_pWeaponObject = nullptr;
            m_pGunflashObject = nullptr;
        }
    }

    m_nWeaponGunflashAlphaMP1 = 0;
    m_nWeaponGunflashAlphaMP2 = 0;
    m_nWeaponModelId = -1;
}

/*!
* @addr 0x5E3A90
* @brief Creates goggles model for current infrared/night vision. See PutOnGoggles.
*/
void CPed::AddGogglesModel(int32 modelIndex, bool& inOutGogglesState) {
    if (modelIndex != MODEL_INVALID) {
        // Como el original: lee el model info primero y, si ya había gafas, las quita (sin assert)
        auto* const mi = CModelInfo::GetModelInfo(modelIndex);
        if (m_pGogglesObject) {
            RemoveGogglesModel();
        }
        m_pGogglesObject = reinterpret_cast<RpClump*>(mi->CreateInstance());
        mi->AddRef();

        m_pGogglesState = &inOutGogglesState;
        inOutGogglesState = true;
    }
}

/*!
* @addr 0x5E3AE0
* @brief Puts on goggles if current weapon is infrared/night vision. (Also removes weapon model from hand and enabled corresponding PostFX)
*/
void CPed::PutOnGoggles() {
    auto& wepInSlot = GetWeaponInSlot(GetWeaponSlot(WEAPON_INFRARED));

    // Game checks if wepInSlot.m_nType != UNARMED here, not sure why? Probably compiler mistake on switch case codegen..

    switch (wepInSlot.m_Type) {
    case WEAPON_INFRARED:
    case WEAPON_NIGHTVISION: {

        // Add(load) googles model and enable PostFX
        const auto DoAddGogglesModel = [&, this](bool& state) {
            AddGogglesModel(wepInSlot.GetWeaponInfo().m_nModelId1, state);
        };

        switch (wepInSlot.m_Type) {
        case WEAPON_INFRARED:
            DoAddGogglesModel(CPostEffects::m_bInfraredVision);
            break;
        case WEAPON_NIGHTVISION:
            DoAddGogglesModel(CPostEffects::m_bNightVision);
            break;
        }

        // Make sure weapon model doesn't get loaded (Because we've put the them on)
        wepInSlot.m_DontPlaceInHand = true;

        // If it was the active weapon: unload it's weapon model
        // Como el original: el slot activo se lee con signo (movsx) y sin comprobar el rango
        if (&wepInSlot == &m_aWeapons.data()[(int8)m_nActiveWeaponSlot]) {
            RemoveWeaponModel(wepInSlot.GetWeaponInfo().m_nModelId1);
        }

        break;
    }
    }
}

/*!
* @addr    0x5E6580
* @returns Weapon skill with current weapon
*/
eWeaponSkill CPed::GetWeaponSkill() {
    return GetWeaponSkill(GetActiveWeapon().m_Type);
}

/*!
* @addr     0x5E3B60
* @returns Skill with \a weaponType. In case we're a player it's based on our current stat level with this weapon type, otherwise the mem. var. `m_nWeaponSkill`.
*/
eWeaponSkill CPed::GetWeaponSkill(eWeaponType weaponType)
{
    if (!CWeaponInfo::TypeHasSkillStats(weaponType)) {
        return eWeaponSkill::STD;
    }

    if (IsPlayer())
    {
        // Como el original: el nivel exigido es un entero con signo (fild) y el valor de la estadistica se pide una vez
        // por comparacion, tras leer el nivel; con PRO se compara >=, y con STD (no con POOR) se decide entre STD y POOR
        const auto statIndex = (eStats)CWeaponInfo::GetSkillStatIndex(weaponType);
        const auto reqPro = (float)(int32)CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::PRO)->m_nReqStatLevel;
        if (reqPro <= CStats::GetStatValue(statIndex)) {
            return eWeaponSkill::PRO;
        }
        const auto reqStd = (float)(int32)CWeaponInfo::GetWeaponInfo(weaponType, eWeaponSkill::STD)->m_nReqStatLevel;
        if (reqStd <= CStats::GetStatValue(statIndex)) {
            return eWeaponSkill::STD; // Somewhere in-between poor and pro stat levels
        }
        return eWeaponSkill::POOR;
    } else {
        if (weaponType == WEAPON_PISTOL && m_nPedType == PED_TYPE_COP)
            return eWeaponSkill::COP;
        return m_nWeaponSkill;
    }
}

/*!
* @addr 0x5E3C10
* @brief Set weapon skill, unless we're a player ped (IsPlayer)
*/
void CPed::SetWeaponSkill(eWeaponType weaponType, eWeaponSkill skill)
{
    if (!IsPlayer()) {
        m_nWeaponSkill = skill;
    }
}

/*!
* @addr 0x5E1950
* @brief Clear ped look, and start restoring it
*/
void CPed::ClearLookFlag() {
    if (!bIsLooking) {
        return;
    }

    // Originally there's a do-while loop, but it will never iterate more than once, so I won't add it.
    // do { ...

    bIsLooking = false;
    bShakeFist = false; // Como el original: bit 11 de 0x470 (el DLL limpiaba bIsDrowning)
    bIsRestoringLook = true;

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }

    m_nLookTime = CTimer::GetTimeInMS() + (IsPlayer() ? 2000 : 4000);

    // .. } while ((PEDSTATE_LOOK_HEADING || PEDSTATE_LOOK_ENTITY) && bIsLooking), but `bIsLooking` will never be true at this point.
}

/*!
* @addr 0x5E3FF0
* @brief Just calls ClearLookFlag
*/
void CPed::ClearLook() {
    ClearLookFlag();
}

/*!
* @addr 0x5E4000
*/
/*!
* @addr    0x5E4000
* @brief   Turns ped to look at `m_pLookTarget`.
* @returns If `m_fCurrentRotation` changed.
*/
bool CPed::TurnBody() {
    if (m_pLookTarget) {
        m_fLookDirection = CGeneral::GetRadianAngleBetweenPoints(m_pLookTarget->GetPosition2D(), GetPosition2D());
    }

    // Como el original (0x5E4000): el rumbo limitado se corrige con +-2pi segun m_fCurrentRotation (esta NO se toca
    // aqui), se guarda en m_fAimingRotation y en m_fLookDirection, y el giro es diferencia * 0,2 (no / 5).
    constexpr float kPi    = std::bit_cast<float>(0x40490FDBu);
    constexpr float kTwoPi = std::bit_cast<float>(0x40C90FDBu);

    float look = CGeneral::LimitRadianAngle(m_fLookDirection);
    const float rot = m_fCurrentRotation;
    if (rot + kPi < look) {
        look -= kTwoPi;
    } else if (rot - kPi > look) {
        look += kTwoPi;
    }

    const float diff = rot - look;
    m_fAimingRotation = look;

    float absDiff = diff;
    if (diff < 0.f) {
        absDiff = -diff;
    }

    if (!(absDiff > 0.05f)) { // incluye NaN
        m_fCurrentRotation = rot;
        m_fLookDirection = look;
        return true;
    }

    m_fCurrentRotation = rot - diff * 0.2f;
    m_fLookDirection = look;
    return false;
}

/*!
* @addr 0x5E4220
* @brief Check if `this` is valid. Probably used by scripts?
*/
bool CPed::IsPointerValid() {
    // Como el original (0x5E4220): solo comprueba que el índice calculado por división esté en 0..capacidad-1 (0x8C);
    // no mira si el hueco está ocupado ni si el puntero pertenece al pool (IsObjectValid sí lo hacía).
    const auto idx = GetPedPool()->GetIndexUnchecked(this);
    if (idx < 0 || idx >= int32(GetPedPool()->GetSize())) {
        return false;
    }
    return !m_pCollisionList.IsEmpty() || this == FindPlayerPed();
}

/*!
* @addr 0x5E4280
* @brief Retrieve object-space position of the given \a bone.
* @param updateSkinBones if not already called `UpdateRpHAnim` will be called. If this param is not set, and the latter function wasn't yet called a default position will be returned.
*/
CVector CPed::GetBonePosition(eBoneTag bone, bool updateSkinBones) {
    if (!bCalledPreRender) {
        if (!updateSkinBones) {
            // Como el original: matriz * posicion estandar con el orden de sumas de 0x59C890
            return PedE_TransformPoint(*m_matrix, GetPedBoneStdPosition(bone));
        }
        UpdateRpHAnim();
        bCalledPreRender = true;
    }
    // Como el original: jerarquia de GetAnimHierarchyFromSkinClump; matriz = array + indice; sin jerarquia, la posicion del peaton
    if (auto* const hierarchy = GetAnimHierarchyFromSkinClump(GetRpClump())) {
        // Como el original: primero el indice (0x7C51A0) y despues el array de matrices (0x7C5120)
        const auto index = RpHAnimIDGetIndex(hierarchy, bone);
        return *RwMatrixGetPos(RpHAnimHierarchyGetMatrixArray(hierarchy) + index);
    }
    return GetPosition();
}

/*
* @addr 0x5E4280
* @brief Added for compatiblity reasons for hooking - use the version returning CVector where possible in code.
*/
void CPed::GetBonePosition(CVector* outVec, eBoneTag bone, bool updateSkinBones) {
    *outVec = CPed::GetBonePosition(bone, updateSkinBones);
}

/*!
* @addr 0x5E01C0
* @brief Transform inOutPos into the given \a bone's space
*
* @param [in,out] inOutPos The position to be transformed in-place.
* @param          updateSkinBones If `UpdateRpHAnim` should be called
*/
void CPed::GetTransformedBonePosition(RwV3d& inOutPos, eBoneTagU32 bone, bool updateSkinBones) { // todo: fix this too!!!
    // Pretty much the same as GetBonePosition..
    if (updateSkinBones) {
        if (!bCalledPreRender) {
            UpdateRpHAnim();
            bCalledPreRender = true;
        }
    } else if (!bCalledPreRender) { // Return static local bone position instead
        inOutPos = PedE_TransformPoint(*m_matrix, GetPedBoneStdPosition(bone));
        return;
    }

    // Return actual position (como el original: matriz del hueso de GetAnimHierarchyFromSkinClump)
    RwV3dTransformPoints(&inOutPos, &inOutPos, 1, PedE_BoneMatrix(this, bone));
}

/*!
* @addr 0x5E4390
* @brief Give ped an object to hold. If he already has one and \a replace is `true` they will drop it, otherwise do nothing.
*/
CObject* CPed::GiveObjectToPedToHold(int32 modelIndex, uint8 replace) {

    // Como el original: primero busca la tarea HOLD_ENTITY y despues consulta que sujeta el peaton (aunque no haya
    // tarea HOLD_ENTITY); si sujeta algo y replace es true lo suelta, y solo sigue si no habia tarea HOLD_ENTITY o se soltó.
    // Devuelve el objeto creado (EAX del EXE) o nullptr si no hizo nada.
    const auto holdTask = GetTaskManager().FindActiveTaskByType(TASK_SIMPLE_HOLD_ENTITY);
    bool dropped = false;
    if (GetEntityThatThisPedIsHolding() && replace) {
        DropEntityThatThisPedIsHolding(true);
        dropped = true;
    }
    if (holdTask && !dropped) {
        return nullptr;
    }

    // Create object
    auto object = new CObject(modelIndex, false);
    object->SetPosn(GetPosition());
    CWorld::Add(object);

    // Create task
    CVector pos{ 0.f, 0.f, 0.f };
    GetTaskManager().SetTaskSecondary(new CTaskSimpleHoldEntity(object, &pos, PED_NODE_RIGHT_HAND), TASK_SECONDARY_PARTIAL_ANIM);
    return object;
}

/*!
* @addr  0x5E4500
* @brief Set ped's state. If he's now !IsAlive() blip is deleted (if `bClearRadarBlipOnDeath` is set) and `ReleaseCoverPoint()` is called.
*/
void CPed::SetPedState(ePedState pedState) {
    m_nPedState = pedState;
    if (!IsAlive()) {
        ReleaseCoverPoint();
        if (bClearRadarBlipOnDeath) {
            CRadar::ClearBlipForEntity(BLIP_CHAR, GetPedPool()->GetRef(this));
            // TODO: Shouldn't we `bClearRadarBlipOnDeath = false` here?
        }
    }
}

/*!
* @addr 0x5E47E0
* @brief Set ped's created by. If created by mission, set it's hearing and seeing range to 30.
*/
void CPed::SetCharCreatedBy(ePedCreatedBy createdBy) {
    SetCreatedBy(createdBy);

    SetPedDefaultDecisionMaker();

    if (IsCreatedByMission()) {
        auto intel = GetIntelligence();

        intel->SetSeeingRange(30.f);
        intel->SetHearingRange(30.f);
        if (!IsPlayer()) {
            intel->m_fDmRadius = 0.f;
            intel->m_nDmNumPedsToScan = 0;
        }
    }
}

/*!
 * @addr 0x5E4C50
 */
void CPed::CalculateNewVelocity() {
    const auto turnMinimum = StaticRef<float>(0x86C284);
    const auto turnAcceleration = StaticRef<float>(0x86C288);
    const auto slopeBlend = StaticRef<float>(0x86C278);
    const auto slopeDecay = StaticRef<float>(0x86C280);
    const auto step = CTimer::GetTimeStep();
    if (!bIsInTheAir && !bIsLanding && m_nPedState != PEDSTATE_DIE && m_nPedState != PEDSTATE_DEAD && m_nPedState != PEDSTATE_ARRESTED) {
        const float turnStep = m_fHeadingChangeRate * 0.01745329238474369f * step;
        m_fCurrentRotation = CGeneral::LimitRadianAngle(m_fCurrentRotation);
        float target = CGeneral::LimitRadianAngle(m_fAimingRotation);
        if (target > m_fCurrentRotation + 3.1415927410125732f) target -= 6.2831854820251465f;
        else if (target < m_fCurrentRotation - 3.1415927410125732f) target += 6.2831854820251465f;
        const float delta = target - m_fCurrentRotation;
        if (IsPlayer()) m_fMoveAnim = 1.0f;
        else if (delta >= 0.0f && m_fMoveAnim < 0.0f) m_fMoveAnim = turnMinimum;
        else if (delta < 0.0f && m_fMoveAnim > 0.0f) m_fMoveAnim = -turnMinimum;
        const float limit = std::abs(m_fMoveAnim) * turnStep;
        bool turning = false;
        if (delta > limit) {
            m_fCurrentRotation += limit;
            m_fMoveAnim = std::min(m_fMoveAnim + step * turnAcceleration, 1.0f);
            turning = true;
        } else if (delta < -limit) {
            m_fCurrentRotation -= limit;
            m_fMoveAnim = std::max(m_fMoveAnim - step * turnAcceleration, -1.0f);
            turning = true;
        } else if (!IsPlayer() && std::abs(delta) > turnMinimum * turnStep) {
            m_fCurrentRotation += delta * 0.5f;
            m_fMoveAnim *= 0.5f;
        } else {
            m_fCurrentRotation += delta;
            m_fMoveAnim = std::max(std::abs(delta) / turnStep, turnMinimum);
        }
        if (!IsPlayer() && (m_nMoveState == PEDMOVE_NONE || m_nMoveState == PEDMOVE_STILL) && turning
            && !GetIntelligence()->GetTaskUseGun() && !GetIntelligence()->GetTaskFighting()) {
            m_nMoveState = delta > 0.0f ? PEDMOVE_TURN_L : PEDMOVE_TURN_R;
        } else if (m_nMoveState == PEDMOVE_TURN_L || m_nMoveState == PEDMOVE_TURN_R) {
            m_nMoveState = PEDMOVE_STILL;
        }
        m_pedIK.m_fBodyRoll = 0.0f;
    }
    const auto& forward = m_matrix->GetForward();
    const auto& right = m_matrix->GetRight();
    const float slopeForward = field_578.z * forward.z + field_578.y * forward.y + field_578.x * forward.x;
    const float slopeRight = field_578.z * right.z + field_578.y * right.y + field_578.x * right.x;
    if (m_pedIK.bSlopePitch) {
        if (m_nPedState != PEDSTATE_DIE && !bFallenDown
            && (m_nMoveState < PEDMOVE_WALK || g_surfaceInfos.IsStairs(m_nContactSurface) || bPedHitWallLastFrame)) {
            const float decay = m_pedIK.m_fSlopePitch != 0.0f || m_pedIK.m_fSlopeRoll != 0.0f ? CnvDecayPowCpp(slopeDecay, step) : 0.0f; // _CIpow del EXE (0x822130), fstp dword
            m_pedIK.m_fSlopePitch = std::abs(m_pedIK.m_fSlopePitch) > 0.01f ? m_pedIK.m_fSlopePitch * decay : 0.0f;
            m_pedIK.m_fSlopeRoll = std::abs(m_pedIK.m_fSlopeRoll) > 0.02f ? m_pedIK.m_fSlopeRoll * decay : 0.0f;
        } else {
            // _CIasin del EXE (0x821E70) a 24 bits (fpatan/fsqrt redondeados a float), no el asinf de la CRT de la DLL.
            m_pedIK.m_fSlopeRoll = m_nPedState == PEDSTATE_DIE || bFallenDown ? CnvAsinFloatCpp(std::clamp(slopeRight, -1.0f, 1.0f)) : 0.0f;
            // El asin del EXE queda en ST0 y se multiplica por (1 - mezcla) sin pasar por float (0x5E5131..0x5E514D).
            m_pedIK.m_fSlopePitch = CnvBlendedAsinCpp(std::clamp(slopeForward, -1.0f, 1.0f), slopeBlend, m_pedIK.m_fSlopePitch);
        }
    } else if (m_nMoveState >= PEDMOVE_WALK && !IsPlayer()) {
        // The original clears the roll even after calculating its decay.
        m_pedIK.m_fSlopeRoll = 0.0f;
    }
    const float forwardShift = std::sqrt(std::max(1.0f - slopeForward * slopeForward, 0.0f)) * m_vecAnimMovingShiftLocal.y;
    const float rightShift = std::sqrt(std::max(1.0f - slopeRight * slopeRight, 0.0f)) * m_vecAnimMovingShiftLocal.x;
    // El EXE pone 0x550/0x554 a +0.0 y les suma cada termino (0x5E51F9..0x5E5268): un -0.0 del primer producto pasa a +0.0.
    m_vecAnimMovingShift.x = CnvZeroPlusSumCpp(forward.x * forwardShift, right.x * rightShift);
    m_vecAnimMovingShift.y = CnvZeroPlusSumCpp(forward.y * forwardShift, right.y * rightShift);
    m_vecAnimMovingShift *= step < 0.01f && !CTimer::bSlowMotionActive ? 0.01f : 1.0f / step;
}

/*!
* @addr 0x5E52E0
*/
void CPed::CalculateNewOrientation() {
    if (CReplay::Mode != MODE_PLAYBACK && IsPedInControl()) {
        SetOrientation(0.f, 0.f, m_fCurrentRotation);
    }
}

/*!
* @addr 0x5E5320
*/
void CPed::ClearAll() {
    if (IsPedInControl() || IsStateDead()) {
        bRenderPedInCar = true;
        bKnockedUpIntoAir = false; // Como el original: bits 4 y 31 de 0x470 (el DLL limpiaba bHitSteepSlope y bCrouchWhenScared)
        bKnockedOffBike = false;
        m_nPedState = PEDSTATE_NONE;
        m_nMoveState = PEDMOVE_NONE;
        m_pEntityIgnoredCollision = nullptr;
    }
}

// Partículas de pisada. En el EXE (0x5E3630 lluvia, 0x5E37C0 polvo) los argumentos van en registros (EBX/ESI = peatón, EDI/EBX = posición,
// EAX = 4 partículas; el argumento de la pila no se lee), así que no se pueden llamar sin ensamblador y se reproducen aquí. La rand() del EXE
// (0x821B1E) se pide a CGeneral::GetRandomNumber (mismo valor) en el mismo orden: x, y (y velocidad x, y en el polvo).
namespace {
constexpr float kFootRandScale = std::bit_cast<float>(0x38000100u); // 0x858C7C = 1/32767

// 0x534540 y comprobación de distancia a la cámara: (dy² + dx²) > 100 (0x858628) sale; con NaN no sale.
bool FootParticlesInRange(CPed* ped, const CVector* position) {
    if (!ped->GetIsOnScreen()) {
        return false;
    }
    const CVector& camera = TheCamera.GetPosition();
    const float dx = position->x - camera.x;
    const float dy = position->y - camera.y;
    return !(dy * dy + dx * dx > 100.0F);
}

// ((rand * 1/32767) * 0,2 (0x858CC4) + base) - 0,1 (0x858B1C), un redondeo a float por operación.
float FootParticleJitter(float base) {
    const float random = static_cast<float>(CGeneral::GetRandomNumber());
    return ((random * kFootRandScale) * 0.2F + base) - 0.1F;
}
} // namespace

// 0x5E3630
__declspec(noinline) static void EmitFootRainParticles(CPed* ped, CVector* position) {
    if (!FootParticlesInRange(ped, position)) {
        return;
    }
    const FxPrtMult_c mult{ 1.0F, 1.0F, 1.0F, 0.1F, 0.15F, 0.0F, 0.15F };
    for (int32 i = 0; i < 4; ++i) {
        CVector point = *position;
        point.x = FootParticleJitter(point.x);
        point.y = FootParticleJitter(point.y);
        const CVector still{ 0.0F, 0.0F, 0.0F };
        const CVector& forward = ped->GetMatrix().GetForward(); // RwMatrix + 0x10
        // fmul del x87 (0x858CE8 = 1,5): el SSE de la DLL descartaria los subnormales que el EXE conserva
        const CVector moving{ X87TimesFloatCpp(forward.x, 1.5F), X87TimesFloatCpp(forward.y, 1.5F), X87TimesFloatCpp(forward.z, 1.5F) };
        g_fx.m_Splash->AddParticle(point, still, 0.0F, mult, -1.0F, 1.2F, 0.6F, false);
        g_fx.m_Splash->AddParticle(point, moving, 0.0F, mult, -1.0F, 1.2F, 0.6F, false);
    }
}

// 0x5E37C0
__declspec(noinline) static void EmitFootDustParticles(CPed* ped, CVector* position) {
    if (!FootParticlesInRange(ped, position)) {
        return;
    }
    const FxPrtMult_c mult{ 1.0F, 1.0F, 1.0F, 0.1F, 0.15F, 0.0F, 0.15F };
    if (!g_surfaceInfos.ProducesFootDust(ped->m_nContactSurface)) {
        return;
    }
    for (int32 i = 0; i < 4; ++i) {
        CVector point = *position;
        point.x = FootParticleJitter(point.x);
        point.y = FootParticleJitter(point.y);
        CVector velocity;
        // (rand * 1/32767) * 0,3 (0x858C24) - 0,15 (0x858FCC)
        velocity.x = (static_cast<float>(CGeneral::GetRandomNumber()) * kFootRandScale) * 0.3F - 0.15F;
        velocity.y = (static_cast<float>(CGeneral::GetRandomNumber()) * kFootRandScale) * 0.3F - 0.15F;
        velocity.z = 0.0F;
        if (!StaticRef<uint8>(0xB72C70)) { // el EXE solo emite si este byte es 0
            g_fx.m_SmokeII3expand->AddParticle(point, velocity, 0.0F, mult, -1.0F, 1.2F, 0.6F, false);
        }
    }
}

/*!
 * @addr 0x5E5380
 */
void CPed::DoFootLanded(bool leftFoot, uint8 arg1) {
    if (!bCalledPreRender || m_bDontUpdateHierarchy) return;
    CVector position;
    GetBonePosition(&position, leftFoot ? BONE_L_FOOT : BONE_R_FOOT, false);
    position.z -= 0.1f;
    const auto forward = GetForward();
    const auto right = GetRight();
    position += forward * 0.2f;
    if (bDoBloodyFootprints && CLocalisation::Blood()) {
        CShadows::AddPermanentShadow(SHADOW_DEFAULT, gpBloodPoolTex, &position,
            forward.x * StaticRef<float>(0x86C338), forward.y * StaticRef<float>(0x86C338),
            right.x * StaticRef<float>(0x86C33C), right.y * StaticRef<float>(0x86C33C),
            255, 200, 0, 0, 4.0f, 3000, 1.0f);
        if (uint32(m_nDeathTimeMS) > 20) m_nDeathTimeMS -= 20;
        else { m_nDeathTimeMS = 0; bDoBloodyFootprints = false; }
    }
    if (g_surfaceInfos.LeavesFootsteps(m_nContactSurface)
        && (CVector2D{TheCamera.GetPosition()} - CVector2D{GetPosition()}).Magnitude() < 10.0f) {
        CShadows::AddPermanentShadow(SHADOW_DEFAULT, gpShadowPedTex, &position,
            forward.x * StaticRef<float>(0x86C334), forward.y * StaticRef<float>(0x86C334),
            right.x * -0.1f, right.y * -0.1f, 120, 250, 250, 50, 4.0f, IsPlayer() ? 5000 : 2000, 1.0f);
    }
    if (CWeather::Rain > 0.1f && !CCullZones::CamNoRain() && !CCullZones::PlayerNoRain() && CGame::currArea == AREA_CODE_NORMAL_WORLD) {
        EmitFootRainParticles(this, &position);
    }
    EmitFootDustParticles(this, &position);
    if (arg1) {
        m_Wobble = 6.2831854820251465f;
        m_WobbleSpeed = (m_vecAnimMovingShift.y * m_vecAnimMovingShift.y + m_vecAnimMovingShift.x * m_vecAnimMovingShift.x) * 20.0f + 0.4f;
    }
    CVector splash;
    if (physicalFlags.bSubmergedInWater) {
        auto pos = GetPosition();
        float level;
        CWaterLevel::GetWaterLevel(pos.x, pos.y, pos.z + 1.5f, level, true, nullptr);
        splash = CVector{pos.x + CTimer::GetTimeStep() * m_vecMoveSpeed.x * 2.0f, pos.y + CTimer::GetTimeStep() * m_vecMoveSpeed.y * 2.0f, level};
        if (pos.z - 0.4f <= level) return;
    } else {
        if (!g_surfaceInfos.IsShallowWater(m_nContactSurface)) return;
        splash = position;
        splash.z += StaticRef<float>(0x8D21EC);
        splash += GetForwardVector() * StaticRef<float>(0x8D21E8);
    }
    g_fx.TriggerFootSplash(splash);
    m_pedAudio.AddAudioEvent(AE_PED_FOOTSTEP_LEFT, 0.0f, 1.0f, nullptr, eSurfaceType(39), 0, 0);
}

/*!
* @addr 0x5E57F0
*/
void CPed::PlayFootSteps() {
    auto* first = RpAnimBlendClumpGetFirstAssociation(GetRpClump());
    const bool movingAnim = first->m_AnimId == ANIM_ID_WALK || first->m_AnimId == ANIM_ID_RUN || first->m_AnimId == ANIM_ID_SPRINT;
    const bool skating = m_pStats == &CPedStats::ms_apPedStats[int(ePedStats::SKATER)];
    if (bDoBloodyFootprints && uint32(m_nDeathTimeMS) > 0 && uint32(m_nDeathTimeMS) < 300) {
        if (!--m_nDeathTimeMS) bDoBloodyFootprints = false;
    }
    if (!bIsStanding) return;
    const auto landing = [this] {
        if (!bIsLanding) return;
        auto* task = GetTaskManager().GetSimplestActiveTask();
        if (task->GetTaskType() == TASK_SIMPLE_LAND) {
            auto* land = static_cast<CTaskSimpleLand*>(task);
            if (land->RightFootLanded()) DoFootLanded(false, true);
            else if (land->LeftFootLanded()) DoFootLanded(true, true);
        }
    };
    float walkBlend = 0.0f, otherBlend = 0.0f;
    CAnimBlendAssociation* walk = nullptr;
    for (auto* anim = first; anim; anim = RpAnimBlendGetNextAssociation(anim)) {
        if (anim->m_Flags & ANIMATION_WALK) {
            walkBlend += anim->m_BlendAmount;
            walk = anim;
        } else if (!(anim->m_Flags & ANIMATION_DONT_ADD_TO_PARTIAL_BLEND) && anim->m_AnimId != ANIM_ID_FIGHT_IDLE
            && ((anim->m_Flags & ANIMATION_IS_PARTIAL) || !bIsDucking)) {
            otherBlend += anim->m_BlendAmount;
        }
    }
    if (!walk || walkBlend <= 0.5f || otherBlend >= 1.0f) { landing(); return; }
    float leftTime = walk->m_BlendHier->m_fTotalTime * (1.0f / 15.0f);
    float rightTime = walk->m_BlendHier->m_fTotalTime * 0.5f + leftTime;
    if (bIsDucking) { leftTime += 0.2f; rightTime += 0.2f; }
    const float time = walk->m_CurrentTime, previous = time - walk->m_TimeStep;
    if (skating) {
        const float skateTime = walk->m_AnimId == ANIM_ID_WALK ? 8.0f / 15.0f : 5.0f / 15.0f;
        float adhesion = 1.0f;
        switch (g_surfaceInfos.GetAdhesionGroup(m_nContactSurface)) {
        case ADHESION_GROUP_SAND:
            if (CGeneral::GetRandomNumber() % 64) m_vecAnimMovingShiftLocal *= 0.2f;
            landing(); return;
        case ADHESION_GROUP_WET:
            m_vecAnimMovingShiftLocal *= 0.3f;
            landing(); return;
        case ADHESION_GROUP_LOOSE:
            if (CGeneral::GetRandomNumber() % 128) m_vecAnimMovingShiftLocal *= 0.5f;
            adhesion = 0.5f;
            break;
        }
        if (m_pedAudio.m_bCanAddEvent) {
            const float speed = walk->m_AnimId == ANIM_ID_WALK ? 0.75f : 1.0f;
            if (time > 0.0f && previous <= 0.0f) m_pedAudio.AddAudioEvent(AE_PED_SKATE_LEFT, AudioLog10Times20Cpp(adhesion), speed);
            else if (adhesion > 0.2f && time > skateTime && previous <= skateTime) m_pedAudio.AddAudioEvent(AE_PED_SKATE_RIGHT, AudioLog10Times20Cpp(adhesion), speed);
        }
        landing(); return;
    }
    const bool left = time >= leftTime && previous < leftTime;
    const bool right = time >= rightTime && previous < rightTime;
    if (!left && !right) { landing(); return; }
    if (left && IsPlayer() && GetPlayerData()) {
        auto* data = GetPlayerData();
        const bool mask = data->m_pPedClothesDesc->GetIsWearingBalaclava();
        float level = 0.0f;
        if (m_nMoveState == PEDMOVE_SPRINT) level = mask ? 65.0f : 55.0f;
        else if (m_nMoveState == PEDMOVE_JOG || m_nMoveState == PEDMOVE_RUN) {
            if (data->m_fMoveBlendRatio >= 2.0f) level = mask ? 55.0f : 45.0f;
            else if (mask && data->m_fMoveBlendRatio > 1.1f) level = (data->m_fMoveBlendRatio - 1.0f) * 20.0f + 30.0f;
            else if (data->m_fMoveBlendRatio > 1.5f) level = (data->m_fMoveBlendRatio - 1.0f) * 15.0f + 30.0f;
        }
        if (level > 0.0f) {
            CEventSoundQuiet event{this, level, uint32(-1), CVector{}};
            GetEventGlobalGroup()->Add(&event);
        }
    }
    float volume, speed;
    if (bIsDucking) { volume = -18.0f; speed = 0.8f; }
    else {
        if (m_nMoveState == PEDMOVE_RUN) { volume = -6.0f; speed = 1.1f; }
        else if (m_nMoveState == PEDMOVE_SPRINT) { volume = 0.0f; speed = 1.2f; }
        else { volume = -12.0f; speed = 0.9f; }
        if (m_nAnimGroup == ANIM_GROUP_PLAYERSNEAK) { volume -= 6.0f; speed -= 0.1f; }
    }
    if (m_pedAudio.m_bCanAddEvent) m_pedAudio.AddAudioEvent(left ? AE_PED_FOOTSTEP_LEFT : AE_PED_FOOTSTEP_RIGHT, volume, speed);
    DoFootLanded(left, movingAnim);
    landing();
}

/*!
* @addr 0x5E5ED0
* @brief Create model for current active weapon. Also creates FX for molotov if `this->IsPlayer()`.
* @param modelIndex Model that should be created for the current weapon.
*/
void CPed::AddWeaponModel(int32 modelIndex) {
   if (modelIndex == MODEL_INVALID) {
       return;
   }

   // Make sure this weapon is supposed to have a model
   // (May be set to false even if it does, eg.: in case of infrared or night googles, see `TakeOffGoggles`)
   auto& activeWep = GetActiveWeapon();
   if (activeWep.m_DontPlaceInHand) {
       return;
   }

   // Remove old model (if any)
   if (m_pWeaponObject) {
       RemoveWeaponModel(MODEL_INVALID);
   }

   // Create clump for model
   auto& wepMI = *CModelInfo::GetModelInfo(modelIndex);
   m_pWeaponObject = (RpClump*)wepMI.CreateInstance();
   m_pGunflashObject = m_pWeaponObject ? CClumpModelInfo::GetFrameFromName(m_pWeaponObject, "gunflash") : nullptr;
   wepMI.AddRef();

   m_nWeaponModelId = modelIndex;

   // Create FX for molotovs
   if (IsPlayer()) {
       if (   activeWep.m_Type == WEAPON_MOLOTOV
           && modelIndex == eModelID::MODEL_MOLOTOV
           && !activeWep.m_FxSystem
        ) {
           // Como el original: jerarquia de la piel del clump -> indice del hueso -> matriz del array (sin comprobar nada)
           const auto hier = GetAnimHierarchyFromSkinClump(GetRpClump());
           const auto handIdx = RpHAnimIDGetIndex(hier, eBoneTag::BONE_R_HAND);
           const auto handMat = &RpHAnimHierarchyGetMatrixArray(hier)[handIdx];
           activeWep.m_FxSystem = g_fxMan.CreateFxSystem("molotov_flame", CVector{ 0.f, 0.f, 0.f }, handMat, false);
           if (const auto fx = activeWep.m_FxSystem) {
               fx->SetLocalParticles(true);
               fx->CopyParentMatrix();
               fx->Play();
           }
       }
   }
}

/*!
* @addr 0x5E6010
* @brief Take off goggles (Infrared/Night Vision weapon)
*/
void CPed::TakeOffGoggles()
{
    auto& wepInSlot = GetWeaponInSlot(GetWeaponSlot(WEAPON_INFRARED));

    // Game checks if wepInSlot.m_nType != UNARMED here, not sure why? Probably compiler mistake on switch case codegen..

    switch (wepInSlot.m_Type) {
    case WEAPON_INFRARED:
    case WEAPON_NIGHTVISION: {
        // Remove googles model
        RemoveGogglesModel();

        wepInSlot.m_DontPlaceInHand = false;

        // Since we've took off the goggles we might have to load it's weapon model
        // Como el original: el slot activo se lee con signo (movsx) y sin comprobar el rango
        if (&wepInSlot == &m_aWeapons.data()[(int8)m_nActiveWeaponSlot]) {
            AddWeaponModel(wepInSlot.GetWeaponInfo().m_nModelId1);
        }

        break;
    }
    }
}

/*!
* @addr 0x5E6080
* @brief Give ped weapon \a weaponType with ammo \a ammo. If ped has already the same weapon, just add the ammo to the weapon's current total ammo, and reload it.
* @returns Slot of \a weaponType
*/
eWeaponSlot CPed::GiveWeapon(eWeaponType weaponType, uint32 ammo, bool likeUnused) {
    const auto givenWepInfo = CWeaponInfo::GetWeaponInfo(weaponType);
    auto& wepInSlot = GetWeaponInSlot(givenWepInfo->m_nSlot);
    const auto wepSlot = (eWeaponSlot)givenWepInfo->m_nSlot;

    if (wepInSlot.m_Type != weaponType) { // Another weapon in the slot, remove it, and set this weapon

        // Remove previous weapon (and possibly add any ammo it had to `ammo`)
        if (wepInSlot.m_Type != WEAPON_UNARMED) {
            switch (wepSlot) {
            case eWeaponSlot::SHOTGUN:
            case eWeaponSlot::SMG:
            case eWeaponSlot::RIFLE: {
                ammo += wepInSlot.m_TotalAmmo;
                break;
            }
            }

            RemoveWeaponModel(wepInSlot.GetWeaponInfo().m_nModelId1);

            if (givenWepInfo->m_nSlot == CWeaponInfo::GetWeaponInfo(WEAPON_INFRARED)->m_nSlot) {
                RemoveGogglesModel();
            }

            wepInSlot.Shutdown();
        }

        // Give new
        wepInSlot.Initialise(weaponType, ammo, this);

        // Now `wepInSlot` is the weapon we've given to the player

        if (givenWepInfo->m_nSlot == m_nActiveWeaponSlot && !bInVehicle) {
            // Como el original: el modelo sale de la informacion del arma que quedo en la ranura activa
            AddWeaponModel(CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type)->m_nModelId1);
        }
    } else { // Same weapon already in the slot, update its ammo count and `Reload()` it
        if (wepSlot == eWeaponSlot::GIFT) { // Gifts have no ammo :D
            return eWeaponSlot::GIFT;
        }

        // Como el original: suma de 32 bits y recorte con comparacion con signo (jl)
        const auto total = static_cast<int32>(wepInSlot.m_TotalAmmo + ammo);
        wepInSlot.m_TotalAmmo = total < 99'999 ? static_cast<uint32>(total) : 99'999u;
        wepInSlot.Reload(this);

        // TODO: Inlined
        if (wepInSlot.m_State == WEAPONSTATE_OUT_OF_AMMO) {
            if (static_cast<int32>(wepInSlot.m_TotalAmmo) > 0) {
                wepInSlot.m_State = WEAPONSTATE_READY;
            }
        }
    }

    if (wepInSlot.m_State != WEAPONSTATE_OUT_OF_AMMO) {
        wepInSlot.m_State = WEAPONSTATE_READY;
    }

    return wepSlot;
}

/*!
* @notsa
*/
void CPed::GiveWeaponSet1() {
    GiveWeapon(WEAPON_BRASSKNUCKLE, 1, true);
    GiveWeapon(WEAPON_BASEBALLBAT, 1, true);
    GiveWeapon(WEAPON_MOLOTOV, 10, true);
    GiveWeapon(WEAPON_PISTOL, 100, true);
    GiveWeapon(WEAPON_SHOTGUN, 50, true);
    GiveWeapon(WEAPON_MICRO_UZI, 150, true);
    GiveWeapon(WEAPON_AK47, 120, true);
    GiveWeapon(WEAPON_COUNTRYRIFLE, 25, true);
    GiveWeapon(WEAPON_RLAUNCHER, 200, true);
    GiveWeapon(WEAPON_SPRAYCAN, 200, true);
}

/*!
* @notsa
*/
void CPed::GiveWeaponSet2() {
    GiveWeapon(WEAPON_KNIFE, 0, true);
    GiveWeapon(WEAPON_GRENADE, 10, true);
    GiveWeapon(WEAPON_DESERT_EAGLE, 40, true);
    GiveWeapon(WEAPON_SAWNOFF_SHOTGUN, 40, true);
    GiveWeapon(WEAPON_TEC9, 150, true);
    GiveWeapon(WEAPON_M4, 150, true);
    GiveWeapon(WEAPON_SNIPERRIFLE, 21, true);
    GiveWeapon(WEAPON_FLAMETHROWER, 500, true);
    GiveWeapon(WEAPON_EXTINGUISHER, 200, true);
}

/*!
* @notsa
*/
void CPed::GiveWeaponSet3() {
    GiveWeapon(WEAPON_CHAINSAW, 0, true);
    GiveWeapon(WEAPON_REMOTE_SATCHEL_CHARGE, 5, true);
    GiveWeapon(WEAPON_PISTOL_SILENCED, 40, true);
    GiveWeapon(WEAPON_SPAS12_SHOTGUN, 30, true);
    GiveWeapon(WEAPON_MP5, 100, true);
    GiveWeapon(WEAPON_M4, 150, true);
    GiveWeapon(WEAPON_RLAUNCHER_HS, 200, true);
}

/*!
 * @notsa
 */
void CPed::GiveWeaponSet4() {
    // todo: GiveWeapon(WEAPON_INFRARED, 200, true);
    // todo: GiveWeapon(WEAPON_NIGHTVISION, 200, true);
    GiveWeapon(WEAPON_MINIGUN, 500, true);
    GiveWeapon(WEAPON_DILDO2, 0, true);
}

/*!
* @addr 0x5E61F0
* @brief Set current weapon to be the one in \a slot
*/
void CPed::SetCurrentWeapon(int32 slot) {
    if (slot == -1) {
        return;
    }

    // Remove current weapon's model (if any)
    if (const auto currWepType = GetActiveWeapon().m_Type; currWepType != WEAPON_UNARMED) {
        RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(currWepType)->m_nModelId1);
    }

    // Set as active slot
    m_nActiveWeaponSlot = slot;

    // Set chosen weapon in player data
    if (const auto playerData = AsPlayer()->GetPlayerData()) {
        playerData->m_nChosenWeapon = slot;
    }

    // Load weapon in this slot (if any)
    if (const auto wepInSlotType = m_aWeapons[slot].m_Type; wepInSlotType != WEAPON_UNARMED) {
        AddWeaponModel(CWeaponInfo::GetWeaponInfo(wepInSlotType)->m_nModelId1);
    }
}

/*!
* @addr 0x5E6280
* @brief Set current weapon as \a weaponType
*/
void CPed::SetCurrentWeapon(eWeaponType weaponType) {
    SetCurrentWeapon(GetWeaponSlot(weaponType));
}

/*!
* @addr  0x5E62B0
* @brief If ped's current weapon is \a weaponType clear it, and set current weapon as UNARMED
*/
void CPed::ClearWeapon(eWeaponType weaponType)
{
    auto wepSlot = CWeaponInfo::GetWeaponInfo(weaponType)->m_nSlot;
    if (wepSlot == -1) {
        return; // Weapon has no slot. (How could this happen?)
    }

    auto& wep = m_aWeapons[wepSlot];
    if (wep.m_Type != weaponType) {
        return; // Slot doesn't contain the given weapon - Might happen as some weapons share slots.
    }

    if (m_nActiveWeaponSlot == wepSlot) {
        SetCurrentWeapon(WEAPON_UNARMED);
    }

    wep.Shutdown();

    switch (weaponType) {
    case WEAPON_NIGHTVISION:
    case WEAPON_INFRARED:
        RemoveGogglesModel();
        break;
    }
}

/*!
* @addr 0x5E6320
* @brief Clears every weapon from the ped.
*/
void CPed::ClearWeapons()
{
    RemoveWeaponModel(MODEL_INVALID);
    RemoveGogglesModel();
    for (auto& m_aWeapon : m_aWeapons) {
        m_aWeapon.Shutdown();
    }
    CWeaponInfo* getWeaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_UNARMED, eWeaponSkill::STD);
    SetCurrentWeapon(getWeaponInfo->m_nSlot);
}

/*!
* @addr 0x5E6370
* @brief Saves ped's in foot weapon and equips a weapon that can be used in a vehicle if available.
* @param isJetpack  1: when entering jetpack, 0: any other vehicle
*/
void CPed::RemoveWeaponWhenEnteringVehicle(int32 isJetpack) {
    // Como el original: la informacion STD se pide por tipo (GetWeaponInfo(ped, skill) evalua GetWeaponSkill() y consultaria la estadistica)
    // Como el original: no hay assert; el EXE solo compara isJetpack con 1
    if (GetPlayerData()) {
        GetPlayerData()->m_bInVehicleDontAllowWeaponChange = true;
    }

    if (m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
        return;
    }

    const auto SaveCurrentWeaponAndEquipInSlot = [&](eWeaponSlot slot) {
        // if (m_nSavedWeapon == WEAPON_UNIDENTIFIED) // always true
        m_nSavedWeapon = GetActiveWeapon().GetType();
        SetCurrentWeapon(GetWeaponInSlot(slot).GetWeaponInfo().m_nSlot);
    };

    const auto EquipHandgunIfPossible = [&] {
        const auto &shotgun = GetWeaponInSlot(eWeaponSlot::SHOTGUN), &handgun = GetWeaponInSlot(eWeaponSlot::HANDGUN);
        // Como el original: la municion se compara con signo (jg/jle)
        if (shotgun.GetType() == WEAPON_SAWNOFF_SHOTGUN && static_cast<int32>(shotgun.GetTotalAmmo()) > 0 || handgun.GetType() == WEAPON_PISTOL && static_cast<int32>(handgun.GetTotalAmmo()) > 0) {
            SaveCurrentWeaponAndEquipInSlot(eWeaponSlot::HANDGUN);
        } else {
            RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, eWeaponSkill::STD)->m_nModelId1);
        }
    };

    const auto HandleDriveByWeapons = [&](const CWeapon& w) {
        if (static_cast<int32>(w.GetTotalAmmo()) > 0) {
            SaveCurrentWeaponAndEquipInSlot(eWeaponSlot::SMG);
        } else if (isJetpack != 1) {
            RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, eWeaponSkill::STD)->m_nModelId1);
        } else {
            EquipHandgunIfPossible();
        }
    };

    if (!IsPlayer() || !AsPlayer()->GetPlayerInfoForThisPlayerPed()->m_bCanDoDriveBy) {
        return RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, eWeaponSkill::STD)->m_nModelId1);
    }

    if (const auto& w = GetWeaponInSlot(eWeaponSlot::SMG); notsa::contains({ WEAPON_MICRO_UZI, WEAPON_TEC9 }, w.GetType())) {
        return HandleDriveByWeapons(w);
    }

    if (isJetpack == 1) {
        // Whole logic of this function is complicated asf because the player is not
        // supposed to be able to use MP5 while using jetpack. Only Tec-9 or Uzi.
        return EquipHandgunIfPossible();
    }

    if (const auto& w = GetWeaponInSlot(eWeaponSlot::SMG); w.GetType() == eWeaponType::WEAPON_MP5) {
        return HandleDriveByWeapons(w);
    }

    // Como el original: sin Uzi/Tec-9/MP5 utilizable (y sin jetpack) tambien se quita el modelo del arma
    return RemoveWeaponModel(CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, eWeaponSkill::STD)->m_nModelId1);
}

/*!
* @addr  0x5E6490
* @brief If ped has no saved weapon and is not a player load current weapon's model, otherwise set saved weapon as current.
*/
void CPed::ReplaceWeaponWhenExitingVehicle() {
    if (GetPlayerData()) {
        GetPlayerData()->m_bInVehicleDontAllowWeaponChange = false;
    }

    if (IsPlayer() && m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
        SetCurrentWeapon(m_nSavedWeapon);
        m_nSavedWeapon = WEAPON_UNIDENTIFIED;
    } else {                                                           // Not player, or has no saved weapon
        AddWeaponModel(GetActiveWeapon().GetWeaponInfo().m_nModelId1); // Load current active weapon
    }
}

/*!
* @addr 0x5E6530
* @brief Save ped's current weapon and set current to UNARMED.
*/
void CPed::ReplaceWeaponForScriptedCutscene()
{
    m_nSavedWeapon = GetActiveWeapon().m_Type;
    SetCurrentWeapon(0);
}

/*!
* @addr 0x5E6550
* @brief Set ped's current weapon to saved weapon
*/
void CPed::RemoveWeaponForScriptedCutscene()
{
    if (m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
        CWeaponInfo* weaponInfo = CWeaponInfo::GetWeaponInfo(m_nSavedWeapon, eWeaponSkill::STD);
        SetCurrentWeapon(weaponInfo->m_nSlot);
        m_nSavedWeapon = WEAPON_UNIDENTIFIED;
    }
}

/*!
* @addr 0x5E65A0
*/
void CPed::PreRenderAfterTest()
{
    // Como el original: cada GetTaskXxx se llama dos veces (una para comprobar y otra para usar el resultado)
    auto* const intel = GetIntelligence();
    if (intel->GetTaskSwim()) {
        intel->GetTaskSwim()->ApplyRollAndPitch(this);
        m_pedIK.bSlopePitch = false;
    } else if (intel->GetTaskJetPack()) {
        intel->GetTaskJetPack()->ApplyRollAndPitch(this);
        m_pedIK.bSlopePitch = false;
    }

    if (intel->GetTaskInAir()) {
        // Como el original: CTaskSimpleInAir::ApplyRollAndPitch (0x678E80) es un 'ret 4' vacío
        intel->GetTaskInAir();
        m_pedIK.bSlopePitch = false;
    } else if (m_pedIK.bSlopePitch || !IsPlayer() && m_pedIK.m_fSlopePitch != 0.0f) {
        m_pedIK.PitchForSlope();
    }
    bCalledPreRender = true;
    UpdateRpHAnim();

    // Como el original: el slot activo se lee con signo (movsx) y sin comprobar el rango
    if (!CTimer::bSkipProcessThisFrame && m_pWeaponObject && GetPlayerData()) {
        if (m_aWeapons.data()[(int8)m_nActiveWeaponSlot].m_Type == eWeaponType::WEAPON_MINIGUN) {
            if (const auto f = CClumpModelInfo::GetFrameFromName(m_pWeaponObject, "minigun2")) {
                const float spin = CTimer::GetTimeStep() * GetPlayerData()->m_fGunSpinSpeed;
                RwMatrixRotate(
                    RwFrameGetMatrix(f),
                    &CPedIK::XaxisIK,
                    spin * 57.2957763671875f, // 0x859878
                    rwCOMBINEPRECONCAT
                );
            }
        }
    }

    if (GetIsVisible() && CTimeCycle::GetShadowStrength()) {
        // Como el original: dos banderas y la tarea de entrar al coche como conductor (TASK_COMPLEX_ENTER_CAR_AS_DRIVER, 0x2BD)
        bool leavingOrDragged = false;
        bool enteringCar      = false;
        CTask* activeTask     = nullptr;
        if (bInVehicle) {
            if (intel->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_LEAVE_CAR)) {
                leavingOrDragged = true;
            } else if (intel->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_DRAG_PED_FROM_CAR)) {
                leavingOrDragged = true;
            }
        } else {
            activeTask = intel->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_ENTER_CAR_AS_DRIVER);
            if (activeTask) {
                enteringCar = true;
            }
        }

        // Low quality circle below feet shadow
        const auto DrawDummyShadow = [&] {
            const auto cur = (size_t)CTimeCycle::m_CurrentStoredValue;
            CShadows::StoreShadowForPedObject(
                this,
                CTimeCycle::m_fShadowDisplacementX.data()[cur],
                CTimeCycle::m_fShadowDisplacementY.data()[cur],
                CTimeCycle::m_fShadowFrontX.data()[cur],
                CTimeCycle::m_fShadowFrontY.data()[cur],
                CTimeCycle::m_fShadowSideX.data()[cur],
                CTimeCycle::m_fShadowSideY.data()[cur]
            );
        };

        // Como el original: solo el jugador 1 (tipo 0) usa la sombra en tiempo real con calidad alta
        // Como el original: GetFxQuality se llama otra vez si la primera no es VERY_HIGH
        const bool realTimeShadowsPossible = g_fx.GetFxQuality() == FX_QUALITY_VERY_HIGH || (g_fx.GetFxQuality() == FX_QUALITY_HIGH && m_nPedType == PED_TYPE_PLAYER1);
        if (realTimeShadowsPossible) {
            CVector rootPos;
            GetBonePosition(&rootPos, eBoneTag::BONE_ROOT, false);
            const CVector& camPos = TheCamera.GetPosition();
            // Como el original: (y - cy)² + (x - cx)² y se sigue solo con distSq <= MAX (el NaN no pasa)
            const float dy = rootPos.y - camPos.y;
            const float dyy = dy * dy;
            const float dx = rootPos.x - camPos.x;
            const float dxx = dx * dx;
            const float distSq = dyy + dxx;
            if (distSq <= MAX_DISTANCE_PED_SHADOWS_SQR && !physicalFlags.bSubmergedInWater) {
                const auto IsVehicleRTShadable = [](eVehicleType t) {
                    return t == VEHICLE_TYPE_BMX || t == VEHICLE_TYPE_BIKE || t == VEHICLE_TYPE_QUAD;
                };

                bool drawRealTimeShadow = true;
                if (bInVehicle) {
                    drawRealTimeShadow = false;
                    if (m_pVehicle) {
                        drawRealTimeShadow = IsVehicleRTShadable(m_pVehicle->m_nVehicleSubType);
                    }
                }

                if (enteringCar) {
                    drawRealTimeShadow = false;
                    if (const auto* targetVeh = static_cast<CTaskComplexEnterCarAsDriver*>(activeTask)->GetTargetCar()) {
                        drawRealTimeShadow = IsVehicleRTShadable(targetVeh->m_nVehicleSubType);
                    }
                }

                CVector spinePos;
                GetBonePosition(&spinePos, eBoneTag::BONE_SPINE1, false);
                bool doIt = drawRealTimeShadow;
                // Como el original: si no esta vivo (je 0x5E6892) tambien se usa bIsDucking
                if (!IsAlive() || GetPosition().z - 0.2f > spinePos.z) {
                    doIt = bIsDucking;
                }

                if (doIt) {
                    g_realTimeShadowMan.DoShadowThisFrame(this);
                    if (!m_pShadowData) {
                        if (!bInVehicle || leavingOrDragged) {
                            DrawDummyShadow();
                        }
                    }
                }
            }
        } else {
            if (!bInVehicle || leavingOrDragged) {
                DrawDummyShadow();
            }
        }
    }

    if (GetModelId() == MODEL_PLAYER) {
        ShoulderBoneRotation(GetRpClump());
        m_bDontUpdateHierarchy = true;
    }

    // Como el original: la jerarquía se pide una vez aquí (y otra más abajo para la sangre)
    auto* const hier = GetAnimHierarchyFromSkinClump(GetRpClump());
    const auto GetHierMatrix = [hier](int32 id) {
        const auto idx = RpHAnimIDGetIndex(hier, id);
        return RpHAnimHierarchyGetMatrixArray(hier) + idx;
    };

    float windMod = 1.0f;
    bool rainAffectsPlayer = false;
    if (IsPlayer()) {
        if (CWindModifiers::FindWindModifier(GetPosition(), &windMod, &windMod)) {
            if (!CCullZones::PlayerNoRain()) {
                rainAffectsPlayer = true;
            }
        }
    }

    // Como el original: solo comprueba m_pVehicle (no bInVehicle)
    bool drivingOpenTopVeh = false;
    if (m_nPedState == PEDSTATE_DRIVING && m_pVehicle) {
        if (m_pVehicle->IsBike()) {
            drivingOpenTopVeh = true;
        } else if (m_pVehicle->IsAutomobile() && m_pVehicle->IsOpenTopCar()) {
            drivingOpenTopVeh = true;
        }
    }

    // Como el original: x = lo + (rand * 1/32767) * (hi - lo) con cada operación redondeada a float
    const auto RandInRange = [](float lo, float hi) {
        float t = (float)(int32)CGeneral::GetRandomNumber();
        t = t * RAND_MAX_FLOAT_RECIPROCAL;
        float d = hi - lo;
        d = t * d;
        return d + lo;
    };

    // Como el original: solo se mira la primera clave de modelo (torso) del jugador
    bool wearingTorsoOrVest = false;
    if (GetPlayerData()) {
        if (GetPlayerData()->m_pPedClothesDesc->m_anModelKeys[0] == CKeyGen::GetUppercaseKey("vest")) {
            wearingTorsoOrVest = true;
        } else if (GetPlayerData()->m_pPedClothesDesc->m_anModelKeys[0] == CKeyGen::GetUppercaseKey("torso")) {
            wearingTorsoOrVest = true;
        }
    }

    if (!wearingTorsoOrVest && (rainAffectsPlayer || drivingOpenTopVeh)) {
        // Como el original: la velocidad del vehículo es la componente en su vector 'forward' ((z + y) + x), no el módulo
        float vehSpeed = 0.0f;
        if (drivingOpenTopVeh) {
            const auto& fwd = m_pVehicle->GetMatrix().GetForward();
            float s = m_pVehicle->m_vecMoveSpeed.z * fwd.z;
            const float sy = m_pVehicle->m_vecMoveSpeed.y * fwd.y;
            s = s + sy;
            const float sx = m_pVehicle->m_vecMoveSpeed.x * fwd.x;
            vehSpeed = s + sx;
        }

        if (rainAffectsPlayer) {
            float w = windMod - 1.0f;
            w = std::fabs(w);
            if (w > vehSpeed) {
                vehSpeed = w;
            }
        }

        const auto ScaleHier = [&](float rangeConst, int32 boneId) {
            RwV3d scale;
            float range = rangeConst * vehSpeed;
            scale.x = RandInRange(1.0f - range, 1.0f + range);
            range = rangeConst * vehSpeed;
            scale.y = RandInRange(1.0f - range, 1.0f + range);
            range = rangeConst * vehSpeed;
            scale.z = RandInRange(1.0f - range, 1.0f + range);
            RwMatrixScale(GetHierMatrix(boneId), &scale, rwCOMBINEPRECONCAT);
            return scale;
        };

        ScaleHier(0.2f, 4);                       // 0x8D1378
        auto scale = ScaleHier(0.1f, 31);         // 0x8D137C
        RwMatrixScale(GetHierMatrix(21), &scale, rwCOMBINEPRECONCAT);
        if (drivingOpenTopVeh || !intel->GetTaskJetPack()) {
            RwMatrixScale(GetHierMatrix(3), &scale, rwCOMBINEPRECONCAT);
        }
        scale = ScaleHier(0.2f, 32);              // 0x8D1380
        RwMatrixScale(GetHierMatrix(22), &scale, rwCOMBINEPRECONCAT);
    }

    // Como el original: la bandera es bRemoveHead (bit 15), no bIsTalking
    if (bRemoveHead && m_nBodypartToRemove == 2) {
        const RwV3d scale{ 0.0f, 0.0f, 0.0f };
        RwMatrixScale(GetHierMatrix(5), &scale, rwCOMBINEPRECONCAT);
        RwMatrixScale(GetHierMatrix(8), &scale, rwCOMBINEPRECONCAT);
        RwMatrixScale(GetHierMatrix(6), &scale, rwCOMBINEPRECONCAT);
        RwMatrixScale(GetHierMatrix(7), &scale, rwCOMBINEPRECONCAT);
    }

    if (m_Wobble > 0.0f) {
        // Como el original: fsin a precisión completa (x87), * 5 (0x8D21F0) por fmul y cambio de signo (fchs, exacto sobre el float)
        const float angle = -TrigSimpleSinMulCpp(m_Wobble, 5.0f);
        m_Wobble = m_Wobble - CTimer::GetTimeStep() * m_WobbleSpeed;

        // Como el original: los huesos 302/301 solo para NO jugadores (tipo distinto de 0 y 1)
        if (!IsPlayer()) {
            RwMatrixRotate(GetHierMatrix(302), &CPedIK::ZaxisIK, angle, rwCOMBINEPRECONCAT);
            RwMatrixRotate(GetHierMatrix(301), &CPedIK::ZaxisIK, angle, rwCOMBINEPRECONCAT);
        }
        RwMatrixRotate(GetHierMatrix(201), &CPedIK::ZaxisIK, angle, rwCOMBINEPRECONCAT);
    }

    if (CWeather::Earthquake > 0.0f) {
        // Como el original: lo + t*(hi-lo) y luego * 0,0025 (0x86C340)
        float swing = RandInRange(-CWeather::Earthquake, CWeather::Earthquake);
        swing = swing * 0.0025f;

        static constexpr int32 bones[] = { 0x2A, 0x34, 0x21, 0x17, 0x20, 0x16, 0x2B, 0x35, 0x22, 0x18, 5 };
        for (const auto bone : bones) {
            RwMatrixRotate(GetHierMatrix(bone), &CPedIK::ZaxisIK, swing, rwCOMBINEPOSTCONCAT);
        }
    }

    // Como el original: bRemoveHead, no bIsTalking; la jerarquía se vuelve a pedir
    if (bRemoveHead && m_nBodypartToRemove == 2 && m_nPedState != PEDSTATE_DEAD && !bIsDyingStuck && (CTimer::GetFrameCounter() & 7) > 3) {
        auto* const bloodHier = GetAnimHierarchyFromSkinClump(GetRpClump());
        const auto bloodIdx = RpHAnimIDGetIndex(bloodHier, 5);
        const auto* const bloodMat = RpHAnimHierarchyGetMatrixArray(bloodHier) + bloodIdx;
        const CVector bloodPos = bloodMat->pos;
        const CVector bloodDir = 0.6f * GetUp();
        g_fx.AddBlood(bloodPos, bloodDir, 16, m_fContactSurfaceBrightness);
    }

    if (CWeather::Rain > 0.3f
        && TheCamera.m_fSoundDistUp > 15.0f
        && !bInVehicle
        && CGame::CanSeeOutSideFromCurrArea()
        && GetPosition().z < 900.0f
        && !CCullZones::CamNoRain()
    ) {
        const CVector toCam = TheCamera.GetPosition() - GetPosition();
        if (toCam.Magnitude() < 25.0f) {
            auto* const pedModelInfo = GetModelInfo()->AsPedModelInfoPtr();
            pedModelInfo->AnimatePedColModelSkinnedWorld(GetRpClump());
            // Como el original: el col model de impactos (m_pHitColModel), no el col model base
            const auto* const hitColData = pedModelInfo->m_pHitColModel->m_pColData;

            const CVector s = FindPlayerSpeed(); // Como el original: se copia (el NaN pasa las comprobaciones)
            const bool doSplash = !(std::fabs(s.x) > 0.05f)
                && !(std::fabs(s.y) > 0.05f)
                && m_nPedState != PEDSTATE_FALL && m_nPedState != PEDSTATE_DIE && m_nPedState != PEDSTATE_DEAD
                && m_nPedState != PEDSTATE_ATTACK && m_nPedState != PEDSTATE_FIGHT
                && IsPedHeadAbovePos(0.3f)
                && !RpAnimBlendClumpGetAssociation(GetRpClump(), ANIM_ID_IDLE_TIRED);

            const int16 numSpheres = (int16)hitColData->m_nNumSpheres;
            if (doSplash && numSpheres > 0) {
                for (int32 i = 0; i < numSpheres; ++i) {
                    const auto& sphere = hitColData->m_pSpheres[i];
                    const uint8 piece = sphere.m_Surface.m_nPiece;
                    if (piece < 5 || !(piece <= 6 || piece == 9)) {
                        continue;
                    }
                    const FxPrtMult_c prt{ 1.0f, 1.0f, 1.0f, 0.35f, 0.01f, 0.0f, 0.03f };
                    // Como el original: x e y con +-0,08 aleatorio y z = z + radio * 0,75 (0x858F34)
                    CVector pos = sphere.m_vecCenter;
                    pos.x = RandInRange(-0.08f, 0.08f) + pos.x;
                    pos.y = RandInRange(-0.08f, 0.08f) + pos.y;
                    pos.z = sphere.m_fRadius * 0.75f + pos.z;
                    CVector vel;
                    vel.x = s.x * 50.0f;
                    vel.y = s.y * 50.0f;
                    vel.z = s.z * 50.0f;
                    g_fx.m_Splash->AddParticle(pos, vel, 0.0f, prt);
                }
            }
        }
    }

    // Como el original: m_nWetness es con signo (jle) y m_nWaterCoverPerc se compara sin signo
    if (GetPlayerData() && (int8)GetPlayerData()->m_nWetness > 0 && GetPlayerData()->m_nWaterCoverPerc < 30u) {
        FxPrtMult_c p{ 1.0f, 1.0f, 1.0f, 0.2f, 0.15f, 0.0f, 0.1f };
        CVector pos = GetPosition();
        // Como el original: ((rand/32767) * escala + pos) - desplazamiento
        const auto Jitter = [](float base, float scaleFactor) {
            float t = (float)(int32)CGeneral::GetRandomNumber();
            t = t * RAND_MAX_FLOAT_RECIPROCAL;
            const float scaled = t * scaleFactor;
            return scaled + base;
        };
        pos.x = Jitter(pos.x, 0.6f) - 0.3f;
        pos.y = Jitter(pos.y, 0.6f) - 0.3f;
        pos.z = Jitter(pos.z, 1.0f) - 0.8f;
        const float wetness = (float)(int32)(int8)GetPlayerData()->m_nWetness;
        float a = wetness * p.m_Color.alpha;
        a = a * 0.01f; // 0x858C58
        p.m_Color.alpha = a;
        const CVector zero{ 0.0f, 0.0f, 0.0f };
        g_fx.m_WaterSplash->AddParticle(pos, zero, 0.0f, p);
    }

    if (bInVehicle && m_pVehicle) {
        m_fContactSurfaceBrightness = m_pVehicle->m_fContactSurfaceBrightness;
    }
}
/*!
* @addr 0x5E7980
* @brief Set ped's state to IDLE
*/
void CPed::SetIdle() {
    switch (m_nPedState) {
    case PEDSTATE_IDLE:
    case PEDSTATE_MUG:
    case PEDSTATE_FLEE_ENTITY:
        break;

    case PEDSTATE_AIMGUN:
        m_nPedState = PEDSTATE_IDLE;
        m_nMoveState = PEDMOVE_STILL;
        break;

    default:
        m_nMoveState = PEDMOVE_STILL;
        break;
    }
}

/*!
* @addr 0x5E79B0
* @brief Set ped to look in direction \a heading
*/
void CPed::SetLook(float heading) {
    if (!IsPedInControl()) {
        return;
    }

    m_nPedState = PEDSTATE_LOOK_HEADING;

    if (m_nLookTime >= CTimer::GetTimeInMS()) {
        return;
    }

    // Como el original: tambien se limpia bIsRestoringLook
    bIsRestoringLook = false;
    bIsLooking = true;
    m_fLookDirection = heading;
    m_nLookTime = 0;

    ClearReference(m_pLookTarget);

    if (CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = false;
    }
}

/*!
* @addr 0x5E7A60
* @brief Set ped to look at \a entity
*/
void CPed::SetLook(CEntity* entity) {
    if (IsPedInControl()) {
        m_nPedState = PEDSTATE_LOOK_ENTITY;
        SetLookFlag(entity, false, false);
    }
}

/*!
* @addr 0x5E7B20
* @copydoc TurnBody
*/
bool CPed::Look() {
    return TurnBody();
}

/*!
* @addr 0x5E7CB0
* @brief Attach a weapon to us or something, not sure.
* @returns \a entity
*/
CEntity* CPed::AttachPedToEntity(CEntity* entity, CVector offset, uint16 turretAngleA, float turretAngleB, eWeaponType weaponType) {
    if (!entity || bInVehicle) {
        return nullptr;
    }

    // BUG/NOTE: ClearReference not called here?
    m_pAttachedTo = entity->AsPhysical();
    assert(m_pAttachedTo);
    m_pAttachedTo->RegisterReference(reinterpret_cast<CEntity**>(&m_pAttachedTo));

    m_vecTurretOffset = offset;
    m_fTurretAngleB = turretAngleB;
    m_fTurretAngleA = turretAngleA;

    // Deal collision with `entity`
    if (!IsPlayer()) {
        if (entity->GetIsTypeVehicle()) {
            m_pEntityIgnoredCollision = entity->AsPhysical();
        }
    } else { // For player just disable collision
        SetUsesCollision(false);
    }

    if (m_nSavedWeapon == WEAPON_UNIDENTIFIED) {
        m_nSavedWeapon = GetActiveWeapon().m_Type;
        m_nTurretAmmo = GetActiveWeapon().m_TotalAmmo; // todo: unify types
    }

    if (!IsPlayer()) {
        GiveWeapon(weaponType, 30'000, true);
        SetCurrentWeapon(weaponType);
        PositionAttachedPed();
    } else {
        if (weaponType != WEAPON_UNARMED) {
            GiveWeapon(weaponType, 30'000, true);
        }

        GetPlayerData()->m_nChosenWeapon = weaponType;
        // Como el original: CPlayerPed::MakeChangesForNewWeapon (0x60B460) tras guardar el arma elegida (faltaba)
        AsPlayer()->MakeChangesForNewWeapon(weaponType);

        if (weaponType == WEAPON_CAMERA) {
            TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_CAMERA);
        } else {
            // With the pool cue we can aim as well, so it needs a different cam mode.
            if (   entity->m_nModelIndex == eModelID::MODEL_POOLCUE
                && !CWeaponInfo::GetWeaponInfo(weaponType)->flags.b1stPerson
            ) {
                TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_AIMWEAPON_ATTACHED);
                GetPlayerData()->m_bFreeAiming = true;
            } else {
                TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_HELICANNON_1STPERSON);
            }
        }
        m_nPedState = PEDSTATE_SNIPER_MODE;
        PositionAttachedPed();
    }

    return entity;
}

/*!
* @addr 0x5E7E60
*/
void CPed::AttachPedToBike(CEntity* entity, CVector offset, uint16 turretAngleA, float turretAngleB, float turretPosnMode, eWeaponType weaponType) {
    if (AttachPedToEntity(entity, offset, turretAngleA, turretAngleB, weaponType)) {
        m_nTurretPosnMode = turretPosnMode;
    }
}

/*!
* @addr 0x5E7EC0
*/
void CPed::DettachPedFromEntity(){
    const auto wasAttachedTo = m_pAttachedTo;

    // BUG/NOTE: Again, ClearOldReference not called.
    m_pAttachedTo = nullptr;

    switch (m_nPedState) {
    case PEDSTATE_DIE: {
        m_pEntityIgnoredCollision = wasAttachedTo;
        // Como el original: fuerza = (forward * -4) con la z + 4.0 (0x858B90), que faltaba
        auto force = wasAttachedTo->GetMatrix().GetForward() * -4.f;
        force.z += 4.f;
        ApplyMoveForce(force);
        bIsStanding = false;
        break;
    }
    case PEDSTATE_DEAD: // Skip this
        break;

    default: {
        CAnimManager::BlendAnimation(GetRpClump(), m_nAnimGroup, ANIM_ID_IDLE, 1000.f);
        // Como el original: or [+0x1C], 1 = m_bUsesCollision (antes se ponia bIsStanding)
        m_bUsesCollision = true;

        // Restore old weapon if any
        if (m_nSavedWeapon != WEAPON_UNIDENTIFIED) {
            GetActiveWeapon().m_AmmoInClip = 0;
            GetActiveWeapon().m_TotalAmmo = 0;

            SetCurrentWeapon(m_nSavedWeapon);
            GetActiveWeapon().m_TotalAmmo = (uint32)m_nTurretAmmo;

            m_nSavedWeapon = WEAPON_UNIDENTIFIED;
        }

        if (IsPlayer()) {
            AsPlayer()->ClearWeaponTarget();
        }

        break;
    }
    }
}

/*!
* @addr 0x5E8830
*/
void CPed::SetAimFlag(float heading) {
    bIsAimingGun = true;
    bIsRestoringGun = false;
    // Como el original: se guarda el rumbo
    m_fLookDirection = heading;
    ClearReference(m_pLookTarget);
    m_nLookTime = 0;

    if (bIsDucking) {
        m_pedIK.bUseArm = false;
    } // It's intentionally overwritten

    m_pedIK.bUseArm = CanWeRunAndFireWithWeapon();
}

/*!
* @addr 0x5E88E0
*/
bool CPed::CanWeRunAndFireWithWeapon() {
    return GetActiveWeapon().GetWeaponInfo(this).flags.bAimWithArm;
}

/*!
* @addr 0x5E8910
*/
void CPed::RequestDelayedWeapon() {
    if (m_nDelayedWeapon == WEAPON_UNIDENTIFIED) {
        return;
    }

    // Simplified a little using an array. Originally it had too much copy paste.

    const auto models = CWeaponInfo::GetWeaponInfo(m_nDelayedWeapon)->GetModels();

    // Request models
    for (auto model : models) {
        if (model != -1) {
            CStreaming::RequestModel(model, STREAMING_KEEP_IN_MEMORY);
        }
    }

    // If it has no model, or at least one model is loaded..
    if (rng::all_of(models, [](auto m) { return m == -1 || CStreaming::IsModelLoaded(m); })) {
        GiveWeapon(m_nDelayedWeapon, m_nDelayedWeaponAmmo, true);
        m_nDelayedWeapon = WEAPON_UNIDENTIFIED;
    }
}

/*!
* @addr 0x5E89B0
* @brief Set delayed weapon (If ped doesn't already have one). Also drops entity in hand unless ped is a player.
*/
void CPed::GiveDelayedWeapon(eWeaponType weaponType, uint32 ammo) {
    // If not a player drop entity in ped's hand (if any)
    if (!IsPlayer()) {
        if (const auto task = (CTaskSimpleHoldEntity*)GetIntelligence()->GetTaskHold(false)) {
            if (task->m_pEntityToHold) {
                if (task->m_nBoneFrameId == ePedNode::PED_NODE_RIGHT_HAND) {
                    DropEntityThatThisPedIsHolding(true);
                }
            }
        }
    }

    // Set delayed weapon (If ped doesn't already have one)
    if (m_nDelayedWeapon == WEAPON_UNIDENTIFIED) {
        m_nDelayedWeaponAmmo = ammo;
        m_nDelayedWeapon = weaponType;
        RequestDelayedWeapon();
    }
}

/*!
* @addr 0x5E8A30
*/
bool IsPedPointerValid(CPed* ped) {
    if(!IsPedPointerValid_NotInWorld(ped)) {
        return false;
    }

    if (ped->IsInVehicle()) {
        return IsEntityPointerValid(ped->m_pVehicle);
    }

    return (ped->m_pCollisionList.GetNodePtr() || ped == FindPlayerPed());
}

inline bool IsPedPointerValid_NotInWorld(CPed* ped) {
    return GetPedPool()->IsObjectValid(ped);
}

/*!
* @addr 0x5E8AB0
*/
void CPed::GiveWeaponAtStartOfFight()
{
    if (GetCreatedBy() != PED_MISSION && GetActiveWeapon().m_Type == WEAPON_UNARMED)
    {
        const auto GiveRandomWeaponByType = [this](eWeaponType type, uint16 maxRandom)
        {
            if ((m_nRandomSeed % 1024) >= maxRandom)
                return;

            if (m_nDelayedWeapon != WEAPON_UNIDENTIFIED)
                return;

            GiveDelayedWeapon(type, 50);
            SetCurrentWeapon(GetWeaponSlot(type));
        };

        switch (m_nPedType)
        {
        case PED_TYPE_GANG1:
        case PED_TYPE_GANG2:
        case PED_TYPE_GANG3:
        case PED_TYPE_GANG4:
        case PED_TYPE_GANG5:
        case PED_TYPE_GANG6:
        case PED_TYPE_GANG7:
        case PED_TYPE_GANG8:
        case PED_TYPE_GANG9:
        case PED_TYPE_GANG10:
            GiveRandomWeaponByType(WEAPON_PISTOL, 400);
            break;
        case PED_TYPE_DEALER:
        case PED_TYPE_CRIMINAL:
        case PED_TYPE_PROSTITUTE:
            GiveRandomWeaponByType(WEAPON_KNIFE, 200);
            GiveRandomWeaponByType(WEAPON_PISTOL, 400);
            break;
        default:
            break;
        }
    }
}

/*!
* @addr 5E8BE0
* @brief If ped has no weapons give them one. (AK-47 if `CHEAT_NO_ONE_CAN_STOP_US` is active, RLauncher if `CHEAT_ROCKET_MAYHEM` is active, or a pistol otherwise)
*/
void CPed::GiveWeaponWhenJoiningGang()
{
    if (GetActiveWeapon().m_Type == WEAPON_UNARMED && m_nDelayedWeapon == WEAPON_UNIDENTIFIED) {
        if (CCheat::IsActive(CHEAT_NO_ONE_CAN_STOP_US)) {
            GiveDelayedWeapon(WEAPON_AK47, 200);
            SetCurrentWeapon(CWeaponInfo::GetWeaponInfo(WEAPON_AK47, eWeaponSkill::STD)->m_nSlot);
        }
        else {
            CWeaponInfo* weaponInfo = nullptr;
            if (CCheat::IsActive(CHEAT_ROCKET_MAYHEM)) {
                GiveDelayedWeapon(WEAPON_RLAUNCHER, 200);
                weaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_RLAUNCHER, eWeaponSkill::STD);
            }
            else {
                GiveDelayedWeapon(WEAPON_PISTOL, 200);
                weaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_PISTOL, eWeaponSkill::STD);
            }
            SetCurrentWeapon(weaponInfo->m_nSlot);
        }
    }
}

/*!
* @addr 0x5EFF50
*/
bool CPed::GetPedTalking() {
    return m_pedSpeech.GetPedTalking();
}

/*!
* @addr 0x5EFF60
*/
void CPed::DisablePedSpeech(bool stopCurrentSpeech) {
    m_pedSpeech.DisablePedSpeech(stopCurrentSpeech);
}

/*!
* @addr 0x5EFF70
*/
void CPed::EnablePedSpeech() {
    m_pedSpeech.EnablePedSpeech();
}

/*!
* @addr 0x5EFF80
*/
void CPed::DisablePedSpeechForScriptSpeech(bool stopCurrentSpeech) {
    m_pedSpeech.DisablePedSpeechForScriptSpeech(stopCurrentSpeech);
}

/*!
* @addr 0x5EFF90
*/
void CPed::EnablePedSpeechForScriptSpeech() {
    m_pedSpeech.EnablePedSpeechForScriptSpeech();
}

/*!
* @addr 0x5EFFA0
*/
bool CPed::CanPedHoldConversation() const {
    return m_pedSpeech.CanPedHoldConversation();
}

/*!
* @addr 0x5EFFB0
*/
void CPed::SayScript(eAudioEvents scriptID, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    m_pedSpeech.AddScriptSayEvent(AE_SCRIPT_SPEECH_PED, scriptID, overrideSilence, isForceAudible, isFrontEnd);
}

/*!
* @addr 0x5EFFE0
* @returns Played soundID - TODO: I'm not sure about this..
*/
int16 CPed::Say(eGlobalSpeechContext gCtx, uint32 startTimeDelay, float probability, bool overrideSilence, bool isForceAudible, bool isFrontEnd) {
    return gCtx != CTX_GLOBAL_NO_SPEECH
        ? m_pedSpeech.AddSayEvent(AE_SPEECH_PED, gCtx, startTimeDelay, probability, overrideSilence, isForceAudible, isFrontEnd)
        : -1;
}

/*!
* @addr 0x5F0060
*/
RwObject* SetPedAtomicVisibilityCB(RwObject* rwObject, void* data) {
    if (!data) {
        rwObjectSetFlags(rwObject, 0); // TODO: Figure out what the flag is
    }
    return rwObject;
}

/*!
* @addr 0x5F0140
* @brief Remove body part
* @todo See if it works with body parts other than the head
*/
void CPed::RemoveBodyPart(ePedNode pedNode, char localDir) {
    UNUSED(localDir);

    // Como el original: sin comprobar el rango del índice
    if (m_apBones.data()[pedNode]->KeyFrame) {
        if (CLocalisation::ShootLimbs()) {
            bRemoveHead = true;
            m_nBodypartToRemove = pedNode;
        }
    } else {
        NOTSA_LOG_DEBUG("Trying to remove ped component");
    }
}

/*!
* @addr 0x5F0190
*/
void CPed::SpawnFlyingComponent(int32 arg0, char arg1)
{
    // NOP
}

/*!
* @addr 0x5F01A0
*/
/*!
* @addr 0x5F01A0
* @brief Check if line of sight bullet would hit the ped (Does a basic check of colpoint.point.z against head position)
* @returns 0, 1 - Yes , 2 - No. Always `1` if ped is falling.
*/
uint8 CPed::DoesLOSBulletHitPed(CColPoint& colPoint) {
    // TODO: Below is just a copy of the code in `IsPedHeadAbovePos` - A separate function should be made.
    // Como el original: el punto (0, 0, 0) se transforma en el sitio con RwV3dTransformPoints y la matriz del hueso de GetAnimHierarchyFromSkinClump
    RwV3d headPos{};
    RwV3dTransformPoints(&headPos, &headPos, 1, PedE_BoneMatrix(this, m_apBones[ePedNode::PED_NODE_HEAD]->BoneTag));

    if (m_nPedState == PEDSTATE_FALL || colPoint.m_vecPoint.z < headPos.z) { // Ped falling, adjust
        return 1;
    }
    // Como el original: con NaN devuelve 0
    return headPos.z + 0.2f > colPoint.m_vecPoint.z ? 2 : 0;
}

/*!
* @addr  0x5F0250
* @brief Remove all weapon animations by blending them into the IDLE animation.
* @param likeUnused Unused
*/
void CPed::RemoveWeaponAnims(int32 likeUnused, float blendDelta) {
    UNUSED(likeUnused);

    bool bFoundNotPartialAnim{};
    for (auto i = 0; i < 34; i++) { // TODO: Magic number `34`
        if (const auto assoc = RpAnimBlendClumpGetAssociation(GetRpClump(), ANIM_ID_FIRE)) {
            assoc->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
            if ((assoc->m_Flags & ANIMATION_IS_PARTIAL)) {
                assoc->m_BlendDelta = blendDelta;
            } else {
                bFoundNotPartialAnim = true;
            }
        }
    }

    if (bFoundNotPartialAnim) {
        CAnimManager::BlendAnimation(GetRpClump(), m_nAnimGroup, ANIM_ID_IDLE, -blendDelta);
    }
}

/*!
* @addr 0x5F02C0
* @returns If world space \a zPos is above ped's head
*/
bool CPed::IsPedHeadAbovePos(float zPos) {
    // Como el original: el punto (0, 0, 0) se transforma en el sitio con RwV3dTransformPoints y la matriz del hueso de GetAnimHierarchyFromSkinClump
    RwV3d headPos{};
    RwV3dTransformPoints(&headPos, &headPos, 1, PedE_BoneMatrix(this, m_apBones[ePedNode::PED_NODE_HEAD]->BoneTag));

    return zPos + GetPosition().z < headPos.z;
}

/*!
* @addr 0x5F0360
*/
void CPed::KillPedWithCar(CVehicle* car, float fDamageIntensity, bool bPlayDeadAnimation) {
    auto& tasks = GetTaskManager();
    if (const auto* task = tasks.GetSimplestActiveTask()) {
        switch (task->GetTaskType()) {
        case TASK_SIMPLE_FALL:
        case TASK_SIMPLE_DIE:
            if (!m_pEntityIgnoredCollision || car->GetStatus() == STATUS_PLAYER) {
                m_pEntityIgnoredCollision = car;
            }
            return;
        case TASK_SIMPLE_DEAD:
            return;
        }
    }
    if (m_pContactEntity && m_pContactEntity->GetIsTypeVehicle()) {
        if (m_pContactEntity->AsVehicle()->IsBoat() || IsPlayer()) return;
    } else if (m_pVehicle == car && (car->IsBoat() || car->IsSubPlane())) {
        return;
    }
    if (const auto* task = tasks.FindActiveTaskByType(TASK_COMPLEX_DESTROY_CAR_MELEE)) {
        if (static_cast<const CTaskComplexDestroyCarMelee*>(task)->m_VehToDestroy == car && car->m_vecMoveSpeed.SquaredMagnitude() < 0.0225f) return;
    }
    CVector offset = GetPosition() - car->GetPosition();
    if (fDamageIntensity > 12.0f && !IsPlayer()) {
        uint8 impact = 0;
        const uint8 random = CGeneral::GetRandomNumber() & 3;
        auto weapon = WEAPON_RAMMEDBYCAR;
        if (car == FindPlayerVehicle(-1, false)) {
            const auto frequency = uint8(std::min(car->m_vecMoveSpeed.Magnitude() * (1.0f / car->m_fMass) * 200000.0f + 80.0f, 250.0f));
            CPad::GetPad(0)->StartShake(int16(40000 / frequency), frequency, 0);
        }
        bIsStanding = false;
        auto direction = GetLocalDirection(CVector2D{-car->m_vecMoveSpeed.x, -car->m_vecMoveSpeed.y});
        const auto& box = car->GetColModel()->m_boundBox;
        const float side = DotProduct(offset, car->GetRight());
        const float height = DotProduct(offset, car->GetUp());
        bool runOver = car->IsSubTrain();
        bool bounce = false;
        if (runOver) {
            impact = 2;
        } else if (DotProduct(car->m_vecMoveSpeed, car->GetForward()) >= 0.0f) {
            if (std::abs(side) > box.m_vecMax.x * 0.99f) {
                impact = side > 0.0f ? 4 : 3;
                runOver = std::abs(DotProduct(offset, car->GetForward())) < car->GetColModel()->m_boundBox.m_vecMax.y * 0.85f;
            } else if ((random == 0 || (height > 0.1f && random > 1)) && !car->m_pHandlingData->m_bIsBig) {
                bounce = true;
                impact = 1;
                const float front = car->GetColModel()->m_boundBox.m_vecMax.y;
                const float back = car->GetColModel()->m_boundBox.m_vecMin.y;
                float top = car->GetColModel()->m_boundBox.m_vecMax.z;
                float distance;
                if (car->GetForward().z < -0.2f) {
                    top = ((car->GetPosition() + car->GetUp() * top) + car->GetForward() * back).z;
                    distance = front - back;
                } else if (car->GetForward().z > 0.1f) {
                    top = ((car->GetPosition() + car->GetUp() * top) + car->GetForward() * front).z;
                    distance = front;
                    if (top - GetPosition().z > 0.0f) {
                        GetPosition().z += (top - GetPosition().z) * 0.5f;
                        top += (top - GetPosition().z) * 0.25f;
                    }
                } else {
                    top = (car->GetPosition() + car->GetUp() * top).z;
                    distance = car->GetColModel()->m_boundBox.m_vecMax.y;
                }
                float lift = (top - GetPosition().z) / (distance / car->m_vecMoveSpeed.Magnitude());
                // El EXE (0x5F094E): fild, fmul 0.002, fadd 1.5, fmul lift con cada paso redondeado a 24 bits
                lift = KpcBounceLiftScaleCpp(lift, CGeneral::GetRandomNumber() & 255);
                auto travel = car->m_vecMoveSpeed;
                travel.Normalise();
                m_vecMoveSpeed = travel * (lift * 0.2f);
                m_vecMoveSpeed.z += lift;
                direction = (direction + 2) % 4;
                if (car->IsAutomobile()) {
                    if (auto* bonnet = car->AsAutomobile()->RemoveBonnetInPedCollision()) {
                        bonnet->m_vecMoveSpeed = (CGeneral::GetRandomNumber() & 1)
                            ? (m_vecMoveSpeed + car->GetRight() * 0.1f) + car->GetUp() * 0.5f
                            : (m_vecMoveSpeed - car->GetRight() * 0.1f) + car->GetUp() * 0.5f;
                        bonnet->ApplyTurnForce(car->GetUp() * 10.0f, car->GetForward());
                    }
                }
                offset = GetPosition() - car->GetPosition();
            } else {
                impact = 2;
                runOver = true;
            }
        }
        if (runOver) {
            weapon = WEAPON_RUNOVERBYCAR;
            m_vecMoveSpeed = car->m_vecMoveSpeed * 0.9f;
            m_vecMoveSpeed.z = 0.0f;
            if (direction == 1 || direction == 3) direction = 2;
        }
        if (CLocalisation::KnockDownPeds()) {
            car->m_vehicleAudio.AddAudioEvent(bounce ? AE_PED_BOUNCE : AE_PED_CRUNCH, 0.0f);
        }
        if (car->m_pDriver) {
            CCrime::ReportCrime(m_nPedType == PED_TYPE_COP ? CRIME_KILL_COP_PED_WITH_CAR : CRIME_KILL_PED_WITH_CAR, this, car->m_pDriver);
        }
        CPedDamageResponseCalculator calculator(car, 1000.0f, weapon, PED_PIECE_TORSO, false);
        CEventDamage event(car, CTimer::GetTimeInMS(), weapon, PED_PIECE_TORSO, uint8(direction), false, bInVehicle);
        const uint8 animRandom = CGeneral::GetRandomNumber() & 3;
        auto anim = AnimationId(191);
        switch (direction) {
        case 0: anim = impact == 3 && animRandom > 1 ? AnimationId(25) : impact == 4 && animRandom > 1 ? AnimationId(27) : AnimationId(24); break;
        case 1: anim = AnimationId(22); break;
        case 2: anim = impact == 3 && animRandom > 1 ? AnimationId(22) : impact == 4 && animRandom > 1 ? AnimationId(23) : AnimationId(26); break;
        case 3: anim = AnimationId(23); break;
        }
        if (event.AffectsPed(this)) {
            const float speed = car->m_vecMoveSpeed.Magnitude();
            const float blend = speed * (weapon == WEAPON_RAMMEDBYCAR ? 8.0f : 12.0f) + 4.0f;
            const float animSpeed = weapon == WEAPON_RAMMEDBYCAR ? 1.0f : float(double(speed * 16.0f) + 1.0);
            calculator.ComputeDamageResponse(this, event.m_damageResponse, true);
            event.m_nAnimGroup = ANIM_GROUP_DEFAULT;
            event.m_nAnimID = anim;
            event.m_fAnimBlend = blend;
            event.m_fAnimSpeed = animSpeed;
            if (bPlayDeadAnimation) {
                CAnimManager::BlendAnimation(GetRpClump(), ANIM_GROUP_DEFAULT, anim, blend)->m_Speed = animSpeed;
                event.m_bAnimAdded = true;
            }
            GetEventGroup().Add(&event);
            if (!m_pEntityIgnoredCollision) m_pEntityIgnoredCollision = car;
            bKnockedUpIntoAir = impact == 1;
            GetIntelligence()->m_collisionScanner.m_bAlreadyHitByCar = true;
        } else {
            bKnockedUpIntoAir = false;
        }
        Say(CTX_GLOBAL_PAIN_DEATH_HIGH);
    } else {
        if (!(m_vecLastCollisionImpactVelocity.z < -0.8f && fDamageIntensity > 3.0f)
            && !(fDamageIntensity > 6.0f && (!IsPlayer() || fDamageIntensity > 10.0f))) return;
        bIsStanding = false;
        const auto direction = GetLocalDirection(CVector2D{-car->m_vecMoveSpeed.x, -car->m_vecMoveSpeed.y});
        const float damage = IsPlayer() && car->IsSubTrain() ? 150.0f : 30.0f;
        CPedDamageResponseCalculator calculator(car, damage, WEAPON_RAMMEDBYCAR, PED_PIECE_TORSO, false);
        CEventDamage event(car, CTimer::GetTimeInMS(), WEAPON_RAMMEDBYCAR, PED_PIECE_TORSO, uint8(direction), false, bInVehicle);
        if (event.AffectsPed(this)) {
            calculator.ComputeDamageResponse(this, event.m_damageResponse, true);
            event.m_nAnimGroup = ANIM_GROUP_DEFAULT;
            event.m_nAnimID = AnimationId(direction + 24);
            event.m_fAnimBlend = 8.0f;
            event.m_fAnimSpeed = 1.0f;
            if (bPlayDeadAnimation) {
                CAnimManager::BlendAnimation(GetRpClump(), ANIM_GROUP_DEFAULT, event.m_nAnimID, 8.0f)->m_Speed = 1.0f;
                event.m_bAnimAdded = true;
            }
            GetEventGroup().Add(&event);
            if (!IsPlayer() || m_bHasHitWall || car->IsSubTrain() || m_vecLastCollisionImpactVelocity.z < -0.8f) m_pEntityIgnoredCollision = car;
            GetIntelligence()->m_collisionScanner.m_bAlreadyHitByCar = true;
        }
        bKnockedUpIntoAir = false;
        if (car->IsSubTrain()) {
            if (m_bHasHitWall) {
                m_vecMoveSpeed = CVector{};
            } else {
                auto direction = car->m_vecMoveSpeed;
                direction.Normalise();
                m_vecMoveSpeed -= direction * DotProduct(direction, m_vecMoveSpeed);
                m_vecMoveSpeed += car->m_vecMoveSpeed * 0.3f;
            }
        } else if (!m_bHasHitWall) {
            m_vecMoveSpeed = car->m_vecMoveSpeed * 0.75f;
        }
        m_vecMoveSpeed.z = 0.0f;
        if (CLocalisation::KnockDownPeds()) car->m_vehicleAudio.AddAudioEvent(AE_PED_KNOCK_DOWN, 0.0f);
        Say(CTX_GLOBAL_PAIN_LOW);
    }
    const auto& up = car->GetUp();
    offset -= up * ((offset.y * up.y + offset.z * up.z) + offset.x * up.x);
    auto direction = offset;
    direction.Normalise();
    const float speed = DotProduct(direction, car->m_vecMoveSpeed);
    if (!bKnockedUpIntoAir) direction.z -= 0.2f;
    const float forceFactor = speed * (car->IsBike() ? StaticRef<float>(0x8D22AC) : StaticRef<float>(0x8D22A8));
    const auto point = offset * StaticRef<float>(0x8D22A4);
    const float massScale = std::min(1.0f, car->m_fMass / StaticRef<float>(0x8D22A0));
    const float force = std::min(car->m_fMass, StaticRef<float>(0x8D22A0)) * massScale * forceFactor;
    car->ApplyForce(direction * force, point, true);
}

/*!
* @addr 0x6AE0D0
*/
template<typename PtrListType>
void CPed::MakeTyresMuddySectorList(PtrListType& ptrList)
{
    CAutomobile* automobile = nullptr;
    CBike* bike = nullptr;
    for (auto* node = ptrList.GetNode(); node;) {
        auto* vehicle = static_cast<CVehicle*>(node->Item);
        node = node->Next;
        if (vehicle->IsScanCodeCurrent()) continue;
        vehicle->SetCurrentScanCode();
        const auto separation = GetPosition() - vehicle->GetPosition();
        if (std::abs(separation.x) >= 10.0f || std::abs(separation.y) >= 10.0f) continue;
        // The original retains these pointers for vehicle types other than car/bike.
        if (vehicle->IsAutomobile()) { automobile = static_cast<CAutomobile*>(vehicle); bike = nullptr; }
        else if (vehicle->IsBike()) { automobile = nullptr; bike = static_cast<CBike*>(vehicle); }
        if (vehicle->m_vecMoveSpeed.x * vehicle->m_vecMoveSpeed.x + vehicle->m_vecMoveSpeed.y * vehicle->m_vecMoveSpeed.y <= 0.05f) continue;
        if (!automobile && !bike) continue;
        auto* selected = automobile ? static_cast<CVehicle*>(automobile) : static_cast<CVehicle*>(bike);
        for (int wheel = 0; wheel < (automobile ? 4 : 2); ++wheel) {
            auto& bloody = automobile ? automobile->m_wheelSkidmarkBloodState[wheel] : bike->m_bWheelBloody[wheel];
            const float compression = automobile ? automobile->m_fWheelsSuspensionCompression[wheel] : bike->m_aWheelRatios[wheel * 2];
            if (bloody || compression >= 1.0f) continue;
            const auto& box = CModelInfo::GetModelInfo(selected->m_nModelIndex)->GetColModel()->m_boundBox;
            CVector point;
            if (automobile) point = CVector{wheel < 2 ? -box.m_vecMax.x : box.m_vecMax.x, (wheel == 0 || wheel == 2) ? box.m_vecMax.y : box.m_vecMin.y, 0.0f};
            else point = CVector{0.0f, (wheel == 0 ? box.m_vecMax.y : box.m_vecMin.y) * 0.8f, 0.0f};
            point = selected->GetMatrix().TransformPoint(point);
            const auto offset = point - GetPosition();
            if (std::abs(offset.z) >= 2.0f || offset.y * offset.y + offset.x * offset.x >= 1.0f) continue;
            if (CLocalisation::Blood()) {
                bloody = true;
                selected->m_vehicleAudio.AddAudioEvent(AE_PED_DRIVE_OVER, 0.0f);
            }
            if (selected->m_fMass <= (automobile ? 500.0f : 100.0f)) continue;
            const float moveForce = automobile ? std::min(m_fMass * 0.001f, 1.0f) * 50.0f : 10.0f;
            const float turnForce = automobile ? std::min(m_fTurnMass * 0.0005f, 1.0f) * 50.0f : 10.0f;
            selected->ApplyMoveForce(0.0f, 0.0f, moveForce);
            selected->ApplyTurnForce(CVector{0.0f, 0.0f, turnForce}, point - selected->GetPosition());
            if (selected == FindPlayerVehicle(-1, false)) CPad::GetPad(0)->StartShake(300, 70, 0);
        }
    }
}

/*!
* @addr  0x6B4200
* @brief Do sector list processing in a range of -/+2 (Calls MakeTyresMuddySectorList)
*/
void CPed::DeadPedMakesTyresBloody() {
    // Como el original: sectores normales (no LOD) con x*0,02f + 60 (no x/50), floor y truncado (_ftol2); inicio >= 0 y fin <= 0x77
    const auto& pos = GetPosition();
    const auto SectorOf = [](float p, float offset) -> int32 {
        float v = p + offset;
        v = v * 0.02f;
        v = v + 60.0f;
        return (int32)(int64)std::floor(v); // _ftol2: truncamiento a 64 bits y se queda con los 32 bajos
    };
    int32 startSectorX = SectorOf(pos.x, -2.0f);
    if (startSectorX <= 0) {
        startSectorX = 0;
    }
    int32 startSectorY = SectorOf(pos.y, -2.0f);
    if (startSectorY <= 0) {
        startSectorY = 0;
    }
    int32 endSectorX = SectorOf(pos.x, 2.0f);
    if (endSectorX >= 0x77) {
        endSectorX = 0x77;
    }
    int32 endSectorY = SectorOf(pos.y, 2.0f);
    if (endSectorY >= 0x77) {
        endSectorY = 0x77;
    }

    CWorld::AdvanceCurrentScanCode();

    for (int32 sy = startSectorY; sy <= endSectorY; ++sy) {
        for (int32 sx = startSectorX; sx <= endSectorX; ++sx) {
            MakeTyresMuddySectorList(CWorld::GetRepeatSector(sx, sy).Vehicles);
        }
    }
}

// Instanciación explícita: el banco de validación necesita el símbolo que llama DeadPedMakesTyresBloody
template void CPed::MakeTyresMuddySectorList<CPtrListDoubleLink<CVehicle*>>(CPtrListDoubleLink<CVehicle*>&);

/*!
* @notsa
* @returns If player is in a vehicle that has a driver as a passenger
*/
bool CPed::IsInVehicleThatHasADriver() {
    if (bInVehicle) { // todo: IsInVehicleAsPassenger - Before refactoring check if `IsPassanger` returns true if `this` is the driver.
        if (m_pVehicle && m_pVehicle->IsPassenger(this) && m_pVehicle->m_pDriver)
            return true;
    }
    return false;
}

/*!
* @notsa
*/
CPedGroup* CPed::GetGroup() const {
    return CPedGroups::GetPedsGroup(this);
}

/*!
* @notsa
*/
int32 CPed::GetGroupId() {
    return GetGroup()
        ? GetGroup()->GetId()
        : -1;
}

/*!
* @notsa
* @returns If ped is follower of \a group
*/
bool CPed::IsFollowerOfGroup(const CPedGroup& group) const {
    return group.GetMembership().IsFollower(this);
}

/*!
* @addr 0x5E4880
* @brief Set model index (Also re-inits animblend, MoneyCount, and default decision-marker)
*/
void CPed::SetModelIndex(uint32 modelIndex) {
    // Como el original: sin assert del jugador
    SetIsVisible(true);

    CEntity::SetModelIndex(modelIndex);

    RpAnimBlendClumpInit(GetRpClump());
    RpAnimBlendClumpFillFrameArray(GetRpClump(), m_apBones.data());

    auto* mi = GetModelInfo()->AsPedModelInfoPtr();

    SetPedStats(mi->m_nStatType);
    RestoreHeadingRate();

    SetPedDefaultDecisionMaker();

    // Set random money count
    // Como el original: primero el dinero según el grupo (rand() % 50 + 20 o rand() % 25) y después la tirada del 3 % (400)
    if (CPopCycle::IsPedInGroup((int16)m_nModelIndex, POPCYCLE_GROUP_BUSINESS) || CPopCycle::IsPedInGroup((int16)m_nModelIndex, POPCYCLE_GROUP_CASUAL_RICH)) {
        m_nMoneyCount = (int16)(CGeneral::GetRandomNumber() % 50 + 20);
    } else {
        m_nMoneyCount = (int16)(CGeneral::GetRandomNumber() % 25);
    }
    {
        float roll = (float)(int32)CGeneral::GetRandomNumber();
        roll = roll * 3.0517578125e-05f;
        roll = roll * 100.0f;
        if ((int32)roll < 3) {
            m_nMoneyCount = 400;
        }
    }

    m_nAnimGroup = mi->m_nAnimType;
    CAnimManager::AddAnimation(GetRpClump(), m_nAnimGroup, ANIM_ID_IDLE);

    if (!CanUseTorsoWhenLooking()) {
        m_pedIK.bTorsoUsed = true;
    }

    // Deal with animation stuff once again
    RpAnimBlendClumpGetData(GetRpClump())->m_PedPosition = (CVector*)&m_vecAnimMovingShiftLocal;

    // Create hit col model
    if (!mi->m_pHitColModel) {
        mi->CreateHitColModelSkinned(GetRpClump());
    }

    // And finally update our rph anim
    UpdateRpHAnim();
}

/*!
* @addr 0x5DEBF0
*/
void CPed::DeleteRwObject()
{
    CEntity::DeleteRwObject();
}

/*!
* @addr 0x5E8CD0
*/
void CPed::ProcessControl()
{
    // El control del ped en cada frame continúa delegado al ejecutable original.
    plugin::CallMethod<0x5E8CD0, CPed*>(this);
}

/*!
* @addr 0x5E4110
* @brief Set player's position (Also resets move/turn speed, and cancels all tasks)
*/
void CPed::Teleport(CVector destination, bool resetRotation) {
    UNUSED(resetRotation);

    if (IsPlayer() || GetTaskManager().HasAnyOf<TASK_COMPLEX_LEAVE_CAR>()) {
        GetIntelligence()->FlushImmediately(true);
    }

    CWorld::Remove(this);
    SetPosn(destination);
    bIsStanding = false;
    ClearReference(m_pDamageEntity);
    // todo: m_pDamageEntity = nullptr;
    CWorld::Add(this);

    m_vecMoveSpeed.Reset();
    m_vecTurnSpeed.Reset();
}

/*!
* @addr 0x5E3C30
*/
void CPed::SpecialEntityPreCollisionStuff(CPhysical* colPhysical,
                                          bool  bIgnoreStuckCheck,
                                          bool& bCollisionDisabled,
                                          bool& bCollidedEntityCollisionIgnored,
                                          bool& bCollidedEntityUnableToMove,
                                          bool& bThisOrCollidedEntityStuck)
{
    // La preparación de colisión propia del ped continúa delegada al ejecutable original.
    plugin::CallMethod<0x5E3C30, CPed*, CPhysical*, bool, bool&, bool&, bool&, bool&>(this, colPhysical, bIgnoreStuckCheck, bCollisionDisabled, bCollidedEntityCollisionIgnored, bCollidedEntityUnableToMove, bThisOrCollidedEntityStuck);
}

/*!
* @addr 0x5E3E90
*/
uint8 CPed::SpecialEntityCalcCollisionSteps(bool& bProcessCollisionBeforeSettingTimeStep, bool& unk2)
{
    // La selección de pasos de colisión continúa delegada al ejecutable original.
    return plugin::CallMethodAndReturn<uint8, 0x5E3E90, CPed*, bool&, bool&>(this, bProcessCollisionBeforeSettingTimeStep, unk2);
}

/*!
* @addr 0x5E8A20
*/
void CPed::PreRender()
{
    if (m_nPedState != PEDSTATE_DRIVING)
        PreRenderAfterTest();
}

/*!
* @addr 0x5E7680
*/
void CPed::Render() {
    // Save alpha fn. ref for player peds
    uint32 storedAlphaRef{1};
    if (IsPlayer()) {
        RwRenderStateGet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(&storedAlphaRef));
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(1u));
    }

    // 0x5E76BE
    if (bDontRender || !(GetIsVisible() || CMirrors::ShouldRenderPeds())) {
        return;
    }

    // 0x5E76F9 - 0x5E7735
    // Now do some extra checks if in vehicle (possibly early out)
    if (   bInVehicle
        && m_pVehicle
        && !GetTaskManager().FindActiveTaskFromList({ TASK_COMPLEX_LEAVE_CAR, TASK_COMPLEX_CAR_SLOW_BE_DRAGGED_OUT_AND_STAND_UP })
    ) {
        // 0x5E774A
        if (!bRenderPedInCar) {
            return;
        }

        // 0x5E7765 - 0x5E7774
        if (   !m_pVehicle->IsBike()
            && !m_pVehicle->IsSubQuad()
            && !IsPlayer()
        ) {
            // 0x5E77DD
            // Okay, let's check if the ped is close enough to the camera

            // Como el original: se sale solo si distSq > rango² (fcompp + test ah,41h): con igualdad o NaN se dibuja
            const auto IsPedOutOfRangeOfCamera = [this](float range) {
                const float distSq = (TheCamera.GetPosition() - GetPosition()).SquaredMagnitude();
                const float finalRange = range * TheCamera.m_fLODDistMultiplier;
                const float finalRangeSq = finalRange * finalRange;
                return distSq > finalRangeSq;
            };
            if (IsPedOutOfRangeOfCamera(m_pVehicle->IsBoat() ? 40.f : 25.f)) { // Boats have bigger range
                return;
            }
        }
    }

    RenderBigHead();
    RenderThinBody();

    // 0x5E77E3
    // Render us (And any extra FX)
    if (CPostEffects::IsVisionFXActive()) {
        CPostEffects::InfraredVisionStoreAndSetLightsForHeatObjects(this);
        CPostEffects::NightVisionSetLights();
        CEntity::Render();
        CPostEffects::InfraredVisionRestoreLightsForHeatObjects();
    } else {
        CEntity::Render();
    }

    // 0x5E7817
    // Render weapon (and gun flash) as well. (Done for local player only if flag is set.)
    if (m_pWeaponObject) {
        if (!GetPlayerData() || GetPlayerData()->m_bRenderWeapon) {
            // Como el original: el flag de CPhysical (bit 8, bSubmergedInWater) decide si se consulta GetTaskSwim
            if ((!physicalFlags.bSubmergedInWater || !GetIntelligence()->GetTaskSwim()) && !GetIntelligence()->GetTaskHold(false)) {
                CVisibilityPlugins::AddWeaponPedForPC(this);
                if (m_nWeaponGunflashAlphaMP1 > 0 || m_nWeaponGunflashAlphaMP2 > 0) {
                    ResetGunFlashAlpha();
                }
            }
        }
    }

    // 0x5E787C
    // Render goggles object
    if (m_pGogglesObject) {
        // Como el original: jerarquía de la ropa de piel (GetAnimHierarchyFromSkinClump) y matriz = array + índice (sin comprobaciones)
        auto* const gogglesHier = GetAnimHierarchyFromSkinClump(GetRpClump());
        const auto gogglesBoneIdx = RpHAnimIDGetIndex(gogglesHier, (RwInt32)eBoneTag::BONE_HEAD);
        const auto* const headMat = RpHAnimHierarchyGetMatrixArray(gogglesHier) + gogglesBoneIdx;

        // Update goggle's matrix with head's
        *RwFrameGetMatrix(RpClumpGetFrame(m_pGogglesObject)) = *headMat; // TODO: Is there a better way to do this?

        // Calculate it's new position
        RwV3d pos{0.f, 0.084f, 0.f};                  // Offset
        RwV3dTransformPoints(&pos, &pos, 1, headMat); // Transform offset into the head's space

        RwV3dAssign(RwMatrixGetPos(RwFrameGetMatrix(RpClumpGetFrame(m_pGogglesObject))), &pos);
        RwFrameUpdateObjects(RpClumpGetFrame(m_pGogglesObject)); // After changing the position it has to be updated

        RpClumpRender(m_pGogglesObject);
    }

    // 0x5E7927
    // Render JetPack (if any)
    // Como el original: GetTaskJetPack se llama dos veces (una para comprobar y otra para usarla)
    if (GetIntelligence()->GetTaskJetPack()) {
        GetIntelligence()->GetTaskJetPack()->RenderJetPack(this);
    }

    bHasBeenRendered = true;

    // 0x5E794D
    // Restore alpha test fn for player peds
    if (IsPlayer()) {
        RwRenderStateSet(rwRENDERSTATEALPHATESTFUNCTIONREF, RWRSTATE(storedAlphaRef));
    }
}

// https://github.com/gennariarmando/bobble-heads
// NOTSA
void CPed::RenderBigHead() const {
    if (!CCheat::IsActive(CHEAT_BIG_HEAD)) {
        return;
    }

    auto hier = GetAnimHierarchyFromSkinClump(GetRpClump());
    auto* matrices = RpHAnimHierarchyGetMatrixArray(hier);

    const float scale = 3.0f;
    const CVector s = { scale, scale, scale };
    CVector t = { 0.0f, -(scale / 6.0f) / 10.0f, 0.0f };

    for (auto& bone : { BONE_L_BROW, BONE_R_BROW, BONE_JAW }) {
        auto index = RpHAnimIDGetIndex(hier, bone);
        if (RwMatrix* mat = &matrices[index]) {
            RwMatrixScale(mat, &s, rwCOMBINEPRECONCAT);
            if (bone == BONE_JAW) {
                t.x = ((scale / 8.0f) / 10.0f) / 8.0f;
                t.y /= 8.0f;
            }
            RwMatrixTranslate(mat, &t, rwCOMBINEPRECONCAT);
        }
    }

    auto index = RpHAnimIDGetIndex(hier, BONE_HEAD);
    if (RwMatrix* mat = &matrices[index]) {
        RwMatrixScale(mat, &s, rwCOMBINEPRECONCAT);
    }
}

bool CPed::CanBeCriminal() const {
    if (IsPlayer() || IsCreatedBy(PED_MISSION)) {
        return false;
    }

    switch (m_nPedType) {
    case PED_TYPE_COP:
    case PED_TYPE_MEDIC:
    case PED_TYPE_FIREMAN:
    case PED_TYPE_MISSION1:
    case PED_TYPE_MISSION2:
    case PED_TYPE_MISSION3:
    case PED_TYPE_MISSION4:
    case PED_TYPE_MISSION5:
    case PED_TYPE_MISSION6:
    case PED_TYPE_MISSION7:
    case PED_TYPE_MISSION8:
        return false;
    }

    return true;
}

// NOTSA
void CPed::RenderThinBody() const {
    if (!CCheat::IsActive(CHEAT_THIN_BODY)) {
        return;
    }
}

/*!
 * @addr 0x553F00
 */
bool CPed::SetupLighting() {
  ActivateDirectional();
  return CRenderer::SetupLightingForEntity(this);
}

/*!
* @addr 0x5533B0
*/
void CPed::RemoveLighting(bool bRemove) {
    UNUSED(bRemove);

    if (!physicalFlags.bRenderScorched) {
        CPointLights::RemoveLightsAffectingObject();
    }

    SetAmbientColours();
    DeActivateDirectional();
}

/*!
* @addr 0x5E7B70
*/
void CPed::FlagToDestroyWhenNextProcessed() {
    m_bRemoveFromWorld = true;

    if (!IsInVehicle()) {
        return;
    }

    if (m_pVehicle->IsDriver(this)) {
        ClearReference(m_pVehicle->m_pDriver);
        if (IsPlayer() && m_pVehicle->GetStatus() != STATUS_WRECKED) {
            m_pVehicle->SetStatus(STATUS_ABANDONED);
        }
    } else {
        m_pVehicle->RemovePassenger(this);
    }

    bInVehicle = false;

    if (IsVehiclePointerValid(m_pVehicle)) {
        SafeCleanUpRef(m_pVehicle);
    }

    m_pVehicle = nullptr;

    SetPedState(IsCreatedByMission() ? PEDSTATE_DEAD : PEDSTATE_NONE);
}

/*!
* @addr 0x5E2530
*/
int32 CPed::ProcessEntityCollision(CEntity* entity, CColPoint* colPoint)
{
    return plugin::CallMethodAndReturn<int32, 0x5E2530, CPed*, CEntity*, CColPoint*>(this, entity, colPoint);
}

// NOTSA
bool CPed::IsInVehicleAsPassenger() const noexcept {
    return bInVehicle && m_pVehicle && m_pVehicle->m_pDriver != this;
}

// NOTSA
CVector CPed::GetSeatPositionInVehicle() const {
    auto* mi = m_pVehicle->GetVehicleModelInfo();
    if (this == m_pVehicle->GetDriver()) {
        CVector pos = mi->GetFrontSeatPosn();

        if (!m_pVehicle->IsBoat() && !m_pVehicle->IsBike()) {
            pos.x = -pos.x;
        }

        if (m_pVehicle->IsSubBMX()) {
            pos.z -= (0.001f * std::abs(m_pVehicle->AsBmx()->m_fControlJump));
        }

        return pos;
    } else if (this == m_pVehicle->m_apPassengers[0]) {
        return m_pVehicle->IsBike() || m_pVehicle->IsSubQuad() ? mi->GetBackSeatPosn() : mi->GetFrontSeatPosn();
    } else if (this == m_pVehicle->m_apPassengers[1]) {
        CVector pos   = mi->GetBackSeatPosn();
        pos.x = -pos.x;
        return pos;
    } else if (this == m_pVehicle->m_apPassengers[2]) {
        return mi->GetBackSeatPosn();
    }
    return mi->GetFrontSeatPosn(); /* Default to front seat position */
}

bool CPed::IsJoggingOrFaster() const {
    switch (m_nMoveState) {
    case PEDMOVE_JOG:
    case PEDMOVE_RUN:
    case PEDMOVE_SPRINT:
        return true;
    }
    return false;
}

bool CPed::IsRunningOrSprinting() const {
    switch (m_nMoveState) {
    case PEDMOVE_RUN:
    case PEDMOVE_SPRINT:
        return true;
    }
    return false;
}

bool CPed::IsPedStandingInPlace() const {
    switch (m_nMoveState) {
    case PEDMOVE_NONE:
    case PEDMOVE_STILL:
    case PEDMOVE_TURN_L:
    case PEDMOVE_TURN_R:
        return true;
    }
    return false;
}

// 0x6497A0
bool SayJacked(CPed* jacked, CVehicle* vehicle, uint32 timeDelay) {
    switch (vehicle->m_vehicleAudio.GetVehicleTypeForAudio()) {
    case eAEVehicleAudioType::CAR:
        return jacked->Say(CTX_GLOBAL_JACKED_CAR, timeDelay) != -1;
    case eAEVehicleAudioType::BIKE:
    case eAEVehicleAudioType::GENERIC:
        return jacked->Say(CTX_GLOBAL_JACKED_GENERIC, timeDelay) != -1;
    default:
        NOTSA_UNREACHABLE();
    }
}

// 0x6497F0
bool SayJacking(CPed* jacker, CPed* jacked, CVehicle* vehicle, uint32 timeDelay) {
    switch (vehicle->m_vehicleAudio.GetVehicleTypeForAudio()) {
    case eAEVehicleAudioType::BIKE:
        return jacker->Say(CTX_GLOBAL_JACKING_BIKE, timeDelay) != -1;
    case eAEVehicleAudioType::CAR:
        return jacked->GetSpeechAE().IsPedFemaleForAudio()
            ? jacker->Say(CTX_GLOBAL_JACKING_CAR_FEM, timeDelay) != -1
            : jacker->Say(CTX_GLOBAL_JACKING_CAR_MALE, timeDelay) != -1;
    case eAEVehicleAudioType::GENERIC:
        return jacker->Say(CTX_GLOBAL_JACKING_GENERIC, timeDelay) != -1;
    default:
        NOTSA_UNREACHABLE();
    }
}

// NOTSA
int32 CPed::GetPadNumber() const {
    switch (m_nPedType) {
    case PED_TYPE_PLAYER1: return 0;
    case PED_TYPE_PLAYER2: return 1;
    default:               NOTSA_UNREACHABLE();
    }
}

bool CPed::IsRightArmBlockedNow() const {
    if (bIsDucking) {
        return bDuckRightArmBlocked;
    }
    return bRightArmBlocked;
}
