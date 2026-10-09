/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerPed
 * Reconstrucción: COMPLETA — 51/51 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 50/51 entradas.
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

#include "PlayerPed.h"
#include "TagManager.h"
#include "PedClothesDesc.h"
#include "PedStats.h"
#include "TaskSimpleUseGun.h"
#include "EntryExitManager.h"
#include "MBlur.h"
#include "EventPlayerCommandToGroupAttack.h"
#include "EventPlayerCommandToGroupGather.h"

bool CPlayerPed::bDebugPlayerInvincible;
bool CPlayerPed::bDebugTargeting;
bool CPlayerPed::bDebugTapToTarget;

void CPlayerPed::InjectHooks() {
    RH_ScopedVirtualClass(CPlayerPed, 0x86D168, 26);
    RH_ScopedCategory("Entity/Ped");

    RH_ScopedInstall(ResetSprintEnergy, 0x60A530);
    RH_ScopedInstall(ResetPlayerBreath, 0x60A8A0);
    RH_ScopedInstall(RemovePlayerPed, 0x6094A0);
    RH_ScopedInstall(Busted, 0x609EF0);
    RH_ScopedInstall(GetWantedLevel, 0x41BE60);
    RH_ScopedInstall(SetWantedLevel, 0x609F10);
    RH_ScopedInstall(SetWantedLevelNoDrop, 0x609F30);
    RH_ScopedInstall(CheatWantedLevel, 0x609F50);
    RH_ScopedInstall(DoStuffToGoOnFire, 0x60A020);
    RH_ScopedVMTInstall(Load, 0x5D46E0);
    RH_ScopedVMTInstall(Save, 0x5D57E0);
    RH_ScopedInstall(DeactivatePlayerPed, 0x609520);
    RH_ScopedInstall(ReactivatePlayerPed, 0x609540);
    RH_ScopedInstall(GetPadFromPlayer, 0x609560);
    RH_ScopedInstall(CanPlayerStartMission, 0x609590);
    RH_ScopedInstall(IsHidden, 0x609620);
    RH_ScopedInstall(ReApplyMoveAnims, 0x609650);
    RH_ScopedInstall(DoesPlayerWantNewWeapon, 0x609710);
    RH_ScopedInstall(ProcessPlayerWeapon, 0x6097F0);
    RH_ScopedInstall(PickWeaponAllowedFor2Player, 0x609800);
    RH_ScopedInstall(UpdateCameraWeaponModes, 0x609830);
    RH_ScopedInstall(ClearWeaponTarget, 0x609c80);
    RH_ScopedInstall(GetWeaponRadiusOnScreen, 0x609CD0);
    RH_ScopedInstall(PedCanBeTargettedVehicleWise, 0x609D90);
    RH_ScopedInstall(FindTargetPriority, 0x609DE0);
    RH_ScopedInstall(Clear3rdPersonMouseTarget, 0x609ED0);
    RH_ScopedInstall(CanIKReachThisTarget, 0x609F80);
    RH_ScopedInstall(GetPlayerInfoForThisPlayerPed, 0x609FF0);
    RH_ScopedInstall(AnnoyPlayerPed, 0x60A040);
    RH_ScopedInstall(ClearAdrenaline, 0x60A070);
    RH_ScopedInstall(DisbandPlayerGroup, 0x60A0A0);
    RH_ScopedInstall(MakeGroupRespondToPlayerTakingDamage, 0x60A110);
    RH_ScopedInstall(TellGroupToStartFollowingPlayer, 0x60A1D0);
    RH_ScopedInstall(MakePlayerGroupDisappear, 0x60A440);
    RH_ScopedInstall(MakePlayerGroupReappear, 0x60A4B0);
    RH_ScopedInstall(HandleSprintEnergy, 0x60A550);
    RH_ScopedInstall(GetButtonSprintResults, 0x60A820);
    RH_ScopedInstall(HandlePlayerBreath, 0x60A8D0);
    RH_ScopedOverloadedInstall(MakeChangesForNewWeapon, "", 0x60B460, void(CPlayerPed::*)(eWeaponType));
    RH_ScopedGlobalInstall(LOSBlockedBetweenPeds, 0x60B550);
    RH_ScopedInstall(DoesTargetHaveToBeBroken, 0x60C0C0);
    RH_ScopedInstall(SetPlayerMoveBlendRatio, 0x60C520);
    RH_ScopedInstall(FindPedToAttack, 0x60C5F0);
    RH_ScopedInstall(ForceGroupToAlwaysFollow, 0x60C7C0);
    RH_ScopedInstall(ForceGroupToNeverFollow, 0x60C800);
    RH_ScopedOverloadedInstall(MakeChangesForNewWeapon, "BySlot", 0x60D000, void(CPlayerPed::*)(uint32));
    RH_ScopedInstall(EvaluateTarget, 0x60D020);
    RH_ScopedInstall(PlayerHasJustAttackedSomeone, 0x60D5A0);
    RH_ScopedInstall(SetupPlayerPed, 0x60D790);

    RH_ScopedVMTInstall(ProcessControl, 0x60EA90);
    RH_ScopedVMTInstall(SetMoveAnim, 0x609490);
}

// TODO: To class and create 2 function
struct CPlayerPedDataSaveStructure {
    uint32          ChaosLevel{};
    eWantedLevel    WantedLevel{};
    CPedClothesDesc ClothesDesc{};
    uint32          ChosenWeapon{};
    // float          Multiplier{}; // Mobile
};

VALIDATE_SIZE(CPlayerPedDataSaveStructure, 0x84 /* + 0x4 */);

// 0x5D46E0
bool CPlayerPed::Load() {
    CPed::Load();

    CGenericGameStorage::LoadDataFromWorkBuffer<uint32>(); // Discard structure size
    auto sd = CGenericGameStorage::LoadDataFromWorkBuffer<CPlayerPedDataSaveStructure>();

    CWanted* wanted = GetPlayerWanted();
    wanted->m_ChaosLevel = sd.ChaosLevel;
    // Como el original: solo se lee el byte bajo del nivel guardado (movzx)
    wanted->m_WantedLevel = static_cast<eWantedLevel>(*reinterpret_cast<const uint8*>(&sd.WantedLevel));

    // Como el original: copia de 0x78 bytes con rep movs
    std::memcpy(GetPlayerData()->m_pPedClothesDesc, &sd.ClothesDesc, sizeof(CPedClothesDesc));
    GetPlayerData()->m_nChosenWeapon   = sd.ChosenWeapon;

    return true;
}

// 0x5D57E0
bool CPlayerPed::Save() {
    CPlayerPedDataSaveStructure saveData{};

    CWanted* wanted = GetPlayerWanted();
    saveData.ChaosLevel = wanted->m_ChaosLevel;
    // Como el original: solo se escriben los bytes bajos del nivel y del arma elegida (el resto queda sin inicializar)
    *reinterpret_cast<uint8*>(&saveData.WantedLevel) = static_cast<uint8>(wanted->m_WantedLevel);
    std::memcpy(&saveData.ClothesDesc, GetPlayerData()->m_pPedClothesDesc, sizeof(CPedClothesDesc));
    *reinterpret_cast<uint8*>(&saveData.ChosenWeapon) = GetPlayerData()->m_nChosenWeapon;

    CPed::Save();
    CGenericGameStorage::SaveDataToWorkBuffer(sizeof(CPlayerPedDataSaveStructure));
    CGenericGameStorage::SaveDataToWorkBuffer(saveData);

    return true;
}

// 0x60D5B0
CPlayerPed::CPlayerPed(int32 playerId, bool bGroupCreated) : CPed(PED_TYPE_PLAYER1) {
    m_pPlayerData = &CWorld::Players[playerId].m_PlayerData;
    GetPlayerData()->AllocateData();

    CPed::SetModelIndex(MODEL_PLAYER);

    CPlayerPed::SetInitialState(bGroupCreated);

    CEntity::ClearReference(m_pTargetedObject);

    SetPedState(PEDSTATE_IDLE);

    gPlayIdlesAnimBlockIndex = CAnimManager::GetAnimationBlockIndex("playidles");

    if (!bGroupCreated) {
        GetPlayerData()->m_nPlayerGroup = CPedGroups::AddGroup();

        auto& group = CPedGroups::GetGroup(GetPlayerData()->m_nPlayerGroup);
        group.GetIntelligence().SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM);
        group.m_bIsMissionGroup = true;
        group.m_groupMembership.SetLeader(this);
        group.Process();

        GetPlayerData()->m_bGroupStuffDisabled = false;
        GetPlayerData()->m_bGroupAlwaysFollow  = false;
        GetPlayerData()->m_bGroupNeverFollow   = false;
    }

    m_fMaxHealth = CStats::GetFatAndMuscleModifier(STAT_MOD_MAX_HEALTH);
    m_fHealth    = m_fMaxHealth;

    m_nFightingStyle      = STYLE_GRAB_KICK;
    m_nAllowedAttackMoves = 15;

    m_p3rdPersonMouseTarget = nullptr;
    field_7A0 = 0;
    m_pedSpeech.Initialise(this);
    GetIntelligence()->m_fDmRadius = 30.0f;
    GetIntelligence()->m_nDmNumPedsToScan = 2;

    bUsedForReplay = bGroupCreated;
}

// 0x6094A0
void CPlayerPed::RemovePlayerPed(int32 playerId) {
    // Como el original: CWorld::Players[playerId] directamente (sin el jugador en foco para ids negativos)
    CPlayerInfo* playerInfo = &CWorld::Players[playerId];
    CPed* player = playerInfo->m_pPed;
    if (player)
    {
        CVehicle* playerVehicle = player->m_pVehicle;
        if (playerVehicle && playerVehicle->m_pDriver == player)
        {
            playerVehicle->SetStatus(STATUS_PHYSICS);
            playerVehicle->m_GasPedal = 0.0f;
            playerVehicle->m_BrakePedal = 0.1f;
        }
        CWorld::Remove(static_cast<CEntity*>(player));
        delete player;
        playerInfo->m_pPed = nullptr;
    }
}

// 0x609520
void CPlayerPed::DeactivatePlayerPed(int32 playerId) {
    CWorld::Remove(CWorld::Players[playerId].m_pPed);
}

// 0x609540
void CPlayerPed::ReactivatePlayerPed(int32 playerId) {
    CWorld::Add(CWorld::Players[playerId].m_pPed);
}

// 0x609560
CPad* CPlayerPed::GetPadFromPlayer() const {
    switch (m_nPedType) {
    case PED_TYPE_PLAYER1:
        return CPad::GetPad(0);

    case PED_TYPE_PLAYER2:
        return CPad::GetPad(1);
    }
    return nullptr; // Como el original: sin assert, cualquier otro tipo devuelve nullptr
}

// 0x609590
bool CPlayerPed::CanPlayerStartMission() {
    if (CGameLogic::GameState != GAMELOGIC_STATE_PLAYING || CGameLogic::IsCoopGameGoingOn())
        return false;

    if (!IsPedInControl() && !IsStateDriving())
        return false;

    auto& taskMgr = GetTaskManager();
    if (taskMgr.GetTaskPrimary(TASK_PRIMARY_PHYSICAL_RESPONSE))
        return false;

    if (taskMgr.GetTaskPrimary(TASK_PRIMARY_EVENT_RESPONSE_NONTEMP))
        return false;

    auto primaryTask = taskMgr.GetTaskPrimary(TASK_PRIMARY_PRIMARY);
    if (primaryTask != nullptr && primaryTask->GetTaskType() != TASK_SIMPLE_CAR_DRIVE)
        return false;

    if (taskMgr.GetTaskSecondary(TASK_SECONDARY_ATTACK))
        return false;

    if (!IsAlive())
        return false;

    return !GetEventGroup().GetEventOfType(EVENT_SCRIPT_COMMAND);
}

// 0x609620
bool CPlayerPed::IsHidden() {
    // Como el original: solo fuera de un vehículo (bit 8 de 0x46C a 0)
    return !bInVehicle && GetLightingTotal() <= 0.05f;
}

// 0x609650
void CPlayerPed::ReApplyMoveAnims() {
    constexpr AnimationId anims[]{
        ANIM_ID_WALK,
        ANIM_ID_RUN,
        ANIM_ID_SPRINT,
        ANIM_ID_IDLE,
        ANIM_ID_WALK_START
    };
    for (const AnimationId& id : anims) {
        if (CAnimBlendAssociation* anim = RpAnimBlendClumpGetAssociation(GetRpClump(), id)) {
            if (anim->GetHashKey() != CAnimManager::GetAnimAssociation(m_nAnimGroup, id)->GetHashKey()) {
                CAnimBlendAssociation* addedAnim = CAnimManager::AddAnimation(GetRpClump(), m_nAnimGroup, id);
                addedAnim->m_BlendDelta = anim->m_BlendDelta;
                addedAnim->m_BlendAmount = anim->m_BlendAmount;

                anim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
                anim->m_BlendDelta = -1000.0f;
            }
        }
    }
}

// 0x609710
bool CPlayerPed::DoesPlayerWantNewWeapon(eWeaponType weaponType, bool arg1) {
    // GetPadFromPlayer(); // Called, but not used

    auto weaponSlot = GetWeaponSlot(weaponType);
    auto weaponInSlotType = GetWeaponInSlot(weaponSlot).m_Type;
    if (weaponInSlotType == weaponType)
        return true;

    if (weaponInSlotType == eWeaponType::WEAPON_UNARMED)
        return true;

    if (arg1)
        return false;

    // Como el original: con el jetpack se evalúa `bAimWithArm && !bAimWithArm`, que nunca es cierto,
    // así que la tarea del jetpack no cambia el resultado
    if (GetIntelligence()->GetTaskJetPack()) {
        if (m_nActiveWeaponSlot == weaponSlot
            && CWeaponInfo::GetWeaponInfo(weaponInSlotType, GetWeaponSkill(weaponInSlotType))->flags.bAimWithArm
            && !CWeaponInfo::GetWeaponInfo(weaponInSlotType, GetWeaponSkill(weaponInSlotType))->flags.bAimWithArm) {
            return false;
        }
    }

    /* !See comment!
    if (m_nActiveWeaponSlot == weaponSlot
        && CWeaponInfo::GetWeaponInfo(weaponInSlotType, GetWeaponSkill(weaponInSlotType))->flags.bAimWithArm   \ One of these two is always false, so
        && !CWeaponInfo::GetWeaponInfo(weaponInSlotType, GetWeaponSkill(weaponInSlotType))->flags.bAimWithArm) / the whole expression is always false
        return false;
    */

    if (m_nActiveWeaponSlot == weaponSlot) {
        switch (m_nPedState) {
        case PEDSTATE_ATTACK:
        case PEDSTATE_AIMGUN:
            return false;
        }
    }

    return true;
}

// 0x6097F0
void CPlayerPed::ProcessPlayerWeapon(CPad* pad) {
    /* empty */
}

// 0x609800
void CPlayerPed::PickWeaponAllowedFor2Player() {
    if (!GetWeaponInSlot(GetPlayerData()->m_nChosenWeapon).CanBeUsedFor2Player()) {
        GetPlayerData()->m_nChosenWeapon = eWeaponType::WEAPON_UNARMED;
    }
}

// 0x609830
void CPlayerPed::UpdateCameraWeaponModes(CPad* pad) {
    switch (GetActiveWeapon().m_Type) {
    case eWeaponType::WEAPON_M4:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_M16_1STPERSON, 0, 0);
        break;

    case eWeaponType::WEAPON_SNIPERRIFLE:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_SNIPER, 0, 0);
        break;

    case eWeaponType::WEAPON_RLAUNCHER:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_ROCKETLAUNCHER, 0, 0);
        break;

    case eWeaponType::WEAPON_RLAUNCHER_HS:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_ROCKETLAUNCHER_HS, 0, 0);
        break;

    case eWeaponType::WEAPON_CAMERA:
        TheCamera.SetNewPlayerWeaponMode(eCamMode::MODE_CAMERA, 0, 0);
        break;

    default:
        TheCamera.ClearPlayerWeaponMode();
        break;
    }
}

// 0x6098F0
void CPlayerPed::ProcessAnimGroups() {
    // El procesamiento de grupos de animación del jugador continúa delegado al ejecutable original.
    plugin::CallMethod<0x6098F0, CPlayerPed *>(this);
}

// 0x609C80
void CPlayerPed::ClearWeaponTarget() {
    if (IsPlayer()) {
        CEntity::ClearReference(m_pTargetedObject);
        TheCamera.ClearPlayerWeaponMode();
        CWeaponEffects::ClearCrossHair(m_nPedType);
    }
}

// 0x609CD0
float CPlayerPed::GetWeaponRadiusOnScreen() {
    CWeapon& wep = GetActiveWeapon();
    // Como el original: información con la habilidad actual (0x5E6580) y cuerpo a cuerpo según su tipo de disparo
    CWeaponInfo& wepInfo = *CWeaponInfo::GetWeaponInfo(wep.m_Type, GetWeaponSkill());

    const float accuracyProg = 0.5f / wepInfo.m_fAccuracy;
    if (wepInfo.m_nWeaponFire == eWeaponFire::WEAPON_FIRE_MELEE)
        return 0.0f;

    float radius;
    switch (wep.m_Type) {
    case eWeaponType::WEAPON_SHOTGUN:
    case eWeaponType::WEAPON_SPAS12_SHOTGUN:
    case eWeaponType::WEAPON_SAWNOFF_SHOTGUN:
        radius = accuracyProg * 1.0f;
        break;

    default: {
        float rangeProg = 15.0f / wepInfo.m_fWeaponRange;
        if (1.0f < rangeProg) // Como el original: NaN se conserva
            rangeProg = 1.0f;
        // Como el original: (0.5 * contador + 1) * (rango * precisión)
        radius = (0.5f * GetPlayerData()->m_fAttackButtonCounter + 1.0f) * (rangeProg * accuracyProg);
        if (bIsDucking)
            radius *= 0.5f;
        break;
    }
    }
    return radius < 0.2f ? 0.2f : radius; // Como el original: NaN se conserva
}

// 0x609D90
bool CPlayerPed::PedCanBeTargettedVehicleWise(CPed* ped) {
    if (ped->bInVehicle) {
        CVehicle* veh = ped->m_pVehicle;
        return veh && (veh->IsBike() || veh->vehicleFlags.bVehicleCanBeTargetted);
    }
    return true;
}

// 0x609DE0
float CPlayerPed::FindTargetPriority(CEntity* entity) {
    switch (entity->GetType()) {
    case ENTITY_TYPE_VEHICLE:
        return 0.1f;

    case ENTITY_TYPE_PED: {
        auto ped = entity->AsPed();

        // Como el original: se evalúan todas las condiciones en este orden (las llamadas se hacen siempre)
        // y la de mayor prioridad sobrescribe a las anteriores
        float priority = 0.1f;
        if (ped->IsCreatedBy(PED_MISSION))
            priority = 0.25f;
        if (ped->m_nPedType == PED_TYPE_GANG2)
            priority = 0.06f;
        if (CPedGroups::AreInSameGroup(this, ped))
            priority = 0.05f;
        if (ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_KILL_PED_ON_FOOT)
            || ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_ARREST_PED)) {
            priority = 0.8f;
        }
        if (ped->bThisPedIsATargetPriority)
            return 1.0f;
        return priority;
    }

    case ENTITY_TYPE_OBJECT: {
        switch (entity->AsObject()->m_nObjectType) {
        case eObjectType::OBJECT_MISSION:
        case eObjectType::OBJECT_MISSION2:
            return 0.1f;

        default:
            return 0.0f;
        }
    }
    default: {
        return 0.1f;
    }
    }
}

// 0x609ED0
void CPlayerPed::Clear3rdPersonMouseTarget() {
    CEntity::ClearReference(m_p3rdPersonMouseTarget);
}

// 0x609EF0
void CPlayerPed::Busted() {
    CWanted* wanted = GetWanted();
    if (wanted) {
        wanted->m_ChaosLevel = 0;
    }
}

// 0x41BE60
eWantedLevel CPlayerPed::GetWantedLevel() const {
    if (const auto* wanted = GetWanted()) {
        return wanted->GetWantedLevel();
    }

    return eWantedLevel::WANTED_CLEAN;
}

// 0x609F10
void CPlayerPed::SetWantedLevel(eWantedLevel level) {
    CWanted* wanted = GetWanted();
    wanted->SetWantedLevel(level);
}

// 0x609F30
void CPlayerPed::SetWantedLevelNoDrop(eWantedLevel level) {
    CWanted* wanted = GetWanted();
    wanted->SetWantedLevelNoDrop(level);
}

// 0x609F50
void CPlayerPed::CheatWantedLevel(eWantedLevel level) {
    CWanted* wanted = GetWanted();
    wanted->CheatWantedLevel(level);
}

// 0x609F80
bool CPlayerPed::CanIKReachThisTarget(CVector posn, CWeapon* weapon, bool arg2) {
    if (!weapon->GetWeaponInfo(this).flags.bAimWithArm) {
        const CVector thisPos = GetPosition();
        // Como el original: cierto salvo que la distancia 2D sea menor (NaN devuelve cierto)
        return !((posn - thisPos).Magnitude2D() < thisPos.z - posn.z);
    }
    return true;
}

// 0x609FF0
CPlayerInfo* CPlayerPed::GetPlayerInfoForThisPlayerPed() {
    // Como el original: recorre CWorld::Players directamente
    for (auto& info : CWorld::Players) {
        if (info.m_pPed == this)
            return &info;
    }
    return nullptr;
}

// 0x60A020
void CPlayerPed::DoStuffToGoOnFire() {
    if (m_nPedState == PEDSTATE_SNIPER_MODE)
        TheCamera.ClearPlayerWeaponMode();
}

// 0x60A040
void CPlayerPed::AnnoyPlayerPed(bool arg0) {
    auto& temper = m_pStats->m_nTemper;

    // Como el original: comparaciones con signo (jge/jl) sobre el byte
    if (static_cast<int8>(temper) < 52) {
        temper++;
    } else if (arg0) {
        if (static_cast<int8>(temper) < 55)
            temper++;
        else
            temper = 46;
    }
}

// 0x60A070
void CPlayerPed::ClearAdrenaline() {
    if (GetPlayerData()->m_bAdrenaline && GetPlayerData()->m_nAdrenalineEndTime != 0) {
        GetPlayerData()->m_nAdrenalineEndTime = 0;
        CTimer::ResetTimeScale();
    }
}

// 0x60A0A0
void CPlayerPed::DisbandPlayerGroup() {
    // Como el original: con miembros dice DISBAND_MANY/DISBAND_ONE y siempre quita a los seguidores
    if (const auto numMembers = GetPlayerGroup().GetMembership().CountMembersExcludingLeader(); numMembers > 0) {
        Say(numMembers > 1 ? CTX_GLOBAL_ORDER_DISBAND_MANY : CTX_GLOBAL_ORDER_DISBAND_ONE);
    }
    GetPlayerGroup().GetMembership().RemoveAllFollowers(true);
}

// 0x60A110
void CPlayerPed::MakeGroupRespondToPlayerTakingDamage(CEventDamage& damageEvent) {
    auto& group = GetPlayerGroup();
    if (!damageEvent.m_pSourceEntity)
        return;
    if (group.GetMembership().CountMembersExcludingLeader() < 1)
        return;
    if (!group.m_bMembersEnterLeadersVehicle)
        return;

    CEventGroupEvent groupEvent(this, damageEvent.Clone());
    group.GetIntelligence().AddEvent(&groupEvent);
}

// 0x60A1D0
void CPlayerPed::TellGroupToStartFollowingPlayer(bool arg0, bool arg1, bool arg2) {
    if (GetPlayerData()->m_bGroupAlwaysFollow && !arg0)
        return;
    if (GetPlayerData()->m_bGroupNeverFollow && arg0)
        return;

    CPedGroup& group = GetPlayerGroup();
    CPedGroupIntelligence& groupIntel = group.GetIntelligence();
    CPedGroupMembership& membership = group.GetMembership();
    if (!arg2 && !membership.CountMembersExcludingLeader())
        return;

    group.m_bMembersEnterLeadersVehicle = arg0;
    groupIntel.SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM);
    if (arg0) {
        // Como el original: el evento local es un CEventPlayerCommandToGroupAttack (vtable 0x86C710)
        CEventPlayerCommandToGroupAttack playerCmdEvent(nullptr);
        playerCmdEvent.ComputeResponseTaskType(&group);
        if (playerCmdEvent.WillRespond()) {
            auto gatherCmdEvent = new CEventPlayerCommandToGroupGather(nullptr); // Como el original: vtable 0x86D120
            gatherCmdEvent->m_TaskId = playerCmdEvent.m_TaskId;

            CEventGroupEvent groupEvent(this, gatherCmdEvent);
            groupIntel.AddEvent(&groupEvent);
        }
    }

    if (arg1) {
        const int32 nMembers = membership.CountMembersExcludingLeader();
        if (nMembers <= 0)
            return;

        // Como el original: la distancia al más lejano se pide siempre, también para esperar
        const float distToFurthest = group.FindDistanceToFurthestMember();
        if (arg0) {
            // Como el original: < 3 muy cerca, < 10 cerca y el resto (NaN incluido) lejos
            if (nMembers > 1) {
                Say(distToFurthest < 3.0f ? CTX_GLOBAL_ORDER_FOLLOW_VNEAR_MANY : distToFurthest < 10.0f ? CTX_GLOBAL_ORDER_FOLLOW_NEAR_MANY : CTX_GLOBAL_ORDER_FOLLOW_FAR_MANY);
            } else {
                Say(distToFurthest < 3.0f ? CTX_GLOBAL_ORDER_FOLLOW_VNEAR_ONE : distToFurthest < 10.0f ? CTX_GLOBAL_ORDER_FOLLOW_NEAR_ONE : CTX_GLOBAL_ORDER_FOLLOW_FAR_ONE);
            }
        } else if (nMembers > 1) {
            Say(CTX_GLOBAL_ORDER_WAIT_MANY);
        } else {
            Say(CTX_GLOBAL_ORDER_WAIT_ONE);
        }
    }
}

// 0x60A440
void CPlayerPed::MakePlayerGroupDisappear() {
    CPedGroupMembership& membership = GetPlayerGroup().GetMembership();
    for (int i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (CPed* member = membership.GetMember(i)) {
            if (!member->IsCreatedByMission()) {
                // Como el original: and [flags], ~0x81 (bUsesCollision y bIsVisible)
                member->SetUsesCollision(false);
                member->SetIsVisible(false);

                abTempNeverLeavesGroup[i] = member->bNeverLeavesGroup;
                member->bNeverLeavesGroup = true;
            }
        }
    }
}

// 0x60A4B0
void CPlayerPed::MakePlayerGroupReappear() {
    CPedGroupMembership& membership = GetPlayerGroup().GetMembership();
    for (int i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; i++) {
        if (CPed* member = membership.GetMember(i)) {
            if (!member->IsCreatedByMission()) {
                member->SetIsVisible(true);
                if (!member->bInVehicle)
                    member->SetUsesCollision(true);
                member->bNeverLeavesGroup = abTempNeverLeavesGroup[i];
            }
        }
    }
}

// 0x60A530
void CPlayerPed::ResetSprintEnergy()
{
    GetPlayerData()->m_fTimeCanRun = CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN);
}

// 0x60A550
bool CPlayerPed::HandleSprintEnergy(bool sprint, float adrenalineConsumedPerTimeStep) {
    float& timeCanRun = GetPlayerData()->m_fTimeCanRun;
    if (sprint) {
        if (CWorld::Players[CWorld::PlayerInFocus].m_bDoesNotGetTired)
            return true;
        if (GetPlayerData()->m_bAdrenaline || adrenalineConsumedPerTimeStep == 0.0f)
            return true;

        if (timeCanRun > -150.0f) { // 0x86D1D8
            // Como el original: max de 0x420800, un NaN se conserva
            const float t = timeCanRun - CTimer::GetTimeStep() * adrenalineConsumedPerTimeStep;
            timeCanRun = (-150.0f > t) ? -150.0f : t;
            return true;
        }
    } else {
        if (CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN) > timeCanRun) {
            timeCanRun += CTimer::GetTimeStep() * adrenalineConsumedPerTimeStep * 0.5f;
        }
    }
    return false;
}

constexpr auto PLAYER_SPRINT_THRESHOLD{ 5.0f }; // 0x8D2458
constexpr struct tPlayerSprintSet { // From 0x8D2460
    float field_0;
    float field_4;
    float field_8;
    float field_C;
    float field_10;
    float field_14;
    float field_18;
    float field_1C;
} PLAYER_SPRINT_SET[] = {
    // 0x0, 0x4,  0x8,  0xC,  0x10,  0x14, 0x18, 0x1C
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 1.0f, 0.5f, 0.3f }, // GROUND
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 0.0f, 0.4f, 1.0f }, // BMX
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 1.0f, 0.3f, 0.3f }, // WATER
    { 4.0f, 0.7f, 0.2f, 5.0f, 10.0f, 0.0f, 0.0f, 1.0f }  // UNDERWATER
};

// 0x60A610
float CPlayerPed::ControlButtonSprint(eSprintType sprintType) {
    // El cálculo del control de sprint continúa delegado al ejecutable original.
    return plugin::CallMethodAndReturn<float, 0x60A610, CPlayerPed *, eSprintType>(this, sprintType);
}

// The original caller at 0x60B430 keeps an animation pointer in EDX across this call.
// Keep the ABI bridge separate from the C++ calculation so compiler changes cannot clobber it.
static float __fastcall GetButtonSprintResultsImpl(CPlayerPed* player, void*, eSprintType sprintType) {
    const auto speed = player->GetPlayerData()->m_fMoveSpeed;
    if (speed > PLAYER_SPRINT_THRESHOLD) {
        // Keep the original x87 intermediate precision instead of rounding each SSE operation.
        const auto multiplier = PLAYER_SPRINT_SET[sprintType].field_1C;
        const float one = 1.0f;
        float result;
        __asm {
            fld speed
            fdiv PLAYER_SPRINT_THRESHOLD
            fsub one
            fmul multiplier
            fadd one
            fstp result
        }
        return result;
    }
    return speed > 0.0f ? 1.0f : 0.0f;
}

// 0x60A820
__declspec(naked) float CPlayerPed::GetButtonSprintResults(eSprintType) {
    __asm {
        push edx
        push dword ptr [esp + 8]
        call GetButtonSprintResultsImpl
        pop edx
        ret 4
    }
}

// 0x60A8A0
void CPlayerPed::ResetPlayerBreath() {
    GetPlayerData()->m_fBreath = CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG);
    GetPlayerData()->m_bRequireHandleBreath = false;
}

// 0x60A8D0
void CPlayerPed::HandlePlayerBreath(bool bDecreaseAir, float fMultiplier) {
    float& breath = GetPlayerData()->m_fBreath;
    float  decreaseAmount = CTimer::GetTimeStep() * fMultiplier;
    if (!bDecreaseAir || CCheat::IsActive(CHEAT_INFINITE_OXYGEN)) {
        if (CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) > breath)
            breath += decreaseAmount * 2.0f;
    } else {
        if (breath > 0.0f && bDrownsInWater) {
            // Como el original: solo pasa a 0 si queda por debajo (NaN y -0 se conservan)
            const float b = breath - decreaseAmount;
            breath = (0.0f > b) ? 0.0f : b;
        } else {
            // Como el original: _ftol2 (truncado de 64 bits)
            CWeapon::GenerateDamageEvent(this, this, eWeaponType::WEAPON_DROWNING, static_cast<int32>(static_cast<int64>(decreaseAmount * 3.0f)), PED_PIECE_TORSO, 0);
        }
    }
    GetPlayerData()->m_bRequireHandleBreath = false;
}

// 0x60A9C0
void CPlayerPed::SetRealMoveAnim() {
    plugin::CallMethod<0x60A9C0, CPlayerPed *>(this);
}

// 0x60B460
void CPlayerPed::MakeChangesForNewWeapon(eWeaponType weaponType) {
    GetActiveWeapon().StopWeaponEffect();
    if (m_nPedState == PEDSTATE_SNIPER_MODE)
        TheCamera.ClearPlayerWeaponMode();

    SetCurrentWeapon(weaponType);

    GetPlayerData()->m_nChosenWeapon = m_nActiveWeaponSlot;
    GetPlayerData()->m_fAttackButtonCounter= 0.0f;

    CWeapon& wep = GetActiveWeapon();
    // Como el original: habilidad actual (0x5E6580)
    CWeaponInfo& wepInfo = *CWeaponInfo::GetWeaponInfo(wep.m_Type, GetWeaponSkill());

    // Como el original: mínimo con signo (jl) entre la munición total y el cargador (movsx)
    wep.m_AmmoInClip = std::min<int32>(static_cast<int32>(wep.m_TotalAmmo), static_cast<int32>(static_cast<int16>(wepInfo.m_nAmmoClip)));

    if (!wepInfo.flags.bCanAim)
        ClearWeaponTarget();

    if (!wepInfo.flags.bOnlyFreeAim)
        GetPlayerData()->m_bFreeAiming = false;


    // Como el original: or byte [+0x2E], 9 (reproducir y quitar al terminar)
    if (auto anim = RpAnimBlendClumpGetAssociation(GetRpClump(), ANIM_ID_FIRE))
        anim->m_Flags |= ANIMATION_IS_PLAYING | ANIMATION_IS_FINISH_AUTO_REMOVE;

    TheCamera.ClearPlayerWeaponMode();
}

// 0x60B550
bool LOSBlockedBetweenPeds(CEntity* entity1, CEntity* entity2) {
    CVector origin{};
    if (entity1->GetIsTypePed()) {
        entity1->AsPed()->GetBonePosition(&origin, eBoneTag::BONE_NECK, false);
        if (entity1->AsPed()->bIsDucking)
            origin.z += 0.35f;
    } else {
        origin = entity1->GetPosition();
    }

    // Como el original: el destino es el cuello o la posición de entity2
    CVector target;
    if (entity2->GetIsTypePed()) {
        entity2->AsPed()->GetBonePosition(&target, eBoneTag::BONE_NECK, false);
    } else {
        target = entity2->GetPosition();
    }

    CColPoint colPoint;
    CEntity* hitEntity;
    if (CWorld::ProcessLineOfSight(origin, target, colPoint, hitEntity, true, false, false, true, false, false, false, true))
        return hitEntity != entity2;
    return false;
}

// 0x60B650
void CPlayerPed::Compute3rdPersonMouseTarget(bool meleeWeapon) {
    // El cálculo del objetivo en tercera persona continúa delegado al ejecutable original.
    plugin::CallMethod<0x60B650, CPlayerPed *, bool>(this, meleeWeapon);
}

// 0x60BA80
void CPlayerPed::DrawTriangleForMouseRecruitPed() {
    plugin::CallMethod<0x60BA80, CPlayerPed *>(this);
}

// 0x60C0C0
bool CPlayerPed::DoesTargetHaveToBeBroken(CEntity* target, CWeapon* weapon) {
    if (!target->GetIsVisible()) {
        return true;
    }

    // Como el original: primero el multiplicador, luego la distancia (suma en orden z, y, x) y la habilidad del arma
    const float rangeMultiplier = CWeapon::TargetWeaponRangeMultiplier(target, this);
    const CVector d = target->GetPosition() - GetPosition();
    const float dist = std::sqrt(d.z * d.z + d.y * d.y + d.x * d.x);
    if (CWeaponInfo::GetWeaponInfo(weapon->m_Type, GetWeaponSkill(weapon->m_Type))->m_fTargetRange * rangeMultiplier < dist) {
        return true;
    }


    if (weapon->m_Type == eWeaponType::WEAPON_SPRAYCAN) {
        if (target->GetIsTypeBuilding()) {
            if (CTagManager::IsTag(*target)) {
                if (CTagManager::GetAlpha(*target) == 255) { // they probably used -1
                    return true;
                }
            }
        }
    }

    return !CanIKReachThisTarget(target->GetPosition(), weapon, false);
}

// 0x60C1E0
void CPlayerPed::KeepAreaAroundPlayerClear() {
    plugin::CallMethod<0x60C1E0, CPlayerPed *>(this);
}

// 0x60C520
void CPlayerPed::SetPlayerMoveBlendRatio(CVector* point) {
    float& moveBlendRatio = GetPlayerData()->m_fMoveBlendRatio;
    if (point) {
        // Como el original: dy² + dx², se guarda y solo se recorta si es mayor que 2 (NaN se conserva)
        const CVector& pos = GetPosition();
        const float dx = point->x - pos.x;
        const float dy = point->y - pos.y;
        const float mag = std::sqrt(dy * dy + dx * dx);
        moveBlendRatio = mag + mag;
        if (moveBlendRatio > 2.0f)
            moveBlendRatio = 2.0f;
    } else {
        switch (m_nMoveState)
        {
        case PEDMOVE_WALK:
            moveBlendRatio = 1.0f;
            break;

        case PEDMOVE_RUN:
            moveBlendRatio = 1.8f;
            break;

        case PEDMOVE_SPRINT:
            moveBlendRatio = 2.5f;
            break;

        default:
            moveBlendRatio = 0.0f;
            break;
        }
    }
    SetRealMoveAnim();
}

// 0x60C5F0
CPed* CPlayerPed::FindPedToAttack() {
    auto* const pool = GetPedPool();
    int32 i = pool->GetSize();

    CVector origin = FindPlayerCoors(-1);
    origin.z = 0.0f;

    // Como el original: frente de TheCamera.m_mCameraMatrix (0xB6F9AC), no el de su CPlaceable
    const auto& camFwd = StaticRef<CVector>(0xB6F9AC);
    CVector end;
    end.x = camFwd.x * 100.0f + origin.x;
    end.y = origin.y + camFwd.y * 100.0f;
    end.z = 0.0f;

    CPed* closestPed{};
    float closestDistance = 99999.9f; // 0x47C34FF3

    // Como el original: del último slot al primero
    while (i != 0) {
        --i;
        CPed* ped = pool->GetAt(i);
        if (!ped)
            continue;
        if (ped->IsPlayer())
            continue;
        if (!ped->IsAlive())
            continue;
        if (GetPlayerGroup().GetMembership().IsMember(ped))
            continue;
        if (ped->m_nPedType == PED_TYPE_GANG2)
            continue;

        CVector point = ped->GetPosition();
        point.z = 0.0f;

        float dist = CCollision::DistToLine(origin, end, point);
        const float dx = point.x - origin.x;
        const float dy = point.y - origin.y;
        const float pointDist = std::sqrt(dx * dx + dy * dy);
        if (pointDist > 20.0f)
            dist = (pointDist - 20.0f) * 0.2f + dist; // Como el original: producto por 0.2 (0x858CC4)

        if (ped->m_nPedType >= PED_TYPE_GANG1 && ped->m_nPedType <= PED_TYPE_GANG10) {
            dist = dist * 0.5f - 2.0f;
            if (!(dist > 0.0f)) // Como el original: NaN pasa a 0
                dist = 0.0f;
        }

        if (dist < closestDistance) {
            closestDistance = dist;
            closestPed = ped;
        }
    }
    return closestPed;
}

// 0x60C7C0
void CPlayerPed::ForceGroupToAlwaysFollow(bool enable) {
    GetPlayerData()->m_bGroupAlwaysFollow = enable;
    if (enable)
        TellGroupToStartFollowingPlayer(true, false, true);
}

// 0x60C800
void CPlayerPed::ForceGroupToNeverFollow(bool enable) {
    GetPlayerData()->m_bGroupNeverFollow = enable;
    if (enable)
        TellGroupToStartFollowingPlayer(false, false, true);
}

// 0x60C840
void CPlayerPed::MakeThisPedJoinOurGroup(CPed* ped) {
    plugin::CallMethod<0x60C840, CPlayerPed *, CPed*>(this, ped);
}

// 0x60CC50
bool CPlayerPed::PlayerWantsToAttack() {
    return plugin::CallMethodAndReturn<bool, 0x60CC50, CPlayerPed *>(this);
}

// 0x60CD20
void CPlayerPed::SetInitialState(bool bGroupCreated) {
    // La configuración inicial del estado del jugador continúa delegada al ejecutable original.
    plugin::CallMethod<0x60CD20, CPlayerPed *, bool>(this, bGroupCreated);
}

// 0x60D000
void CPlayerPed::MakeChangesForNewWeapon(uint32 weaponSlot) {
    if (weaponSlot != -1)
        MakeChangesForNewWeapon(GetWeaponInSlot(weaponSlot).m_Type);
}

static auto& PLAYER_MAX_TARGET_VIEW_ANGLE = StaticRef<float>(0x8D243C); // 140.0f

// Como el original: GetATanOfXY (0x53CC70) deja el ángulo en ST0 sin redondear a float y el EXE le resta
// la compensación directamente (una sola rotura a 24 bits)
static float PlayerPedATanMinus(float x, float y, float compensation) {
    float (*const atanOfXY)(float, float) = &CGeneral::GetATanOfXY;
    float result;
    __asm {
        push y
        push x
        call atanOfXY
        add esp, 8
        fsub compensation
        fstp result
    }
    return result;
}

constexpr float PLAYER_PI = 3.14159274101257324f;     // 0x858CB8
constexpr float PLAYER_TWO_PI = 6.28318548202514648f; // 0x858CBC


// 0x60D020
void CPlayerPed::EvaluateTarget(CEntity* target, CEntity *& outTarget, float & outTargetPriority, float maxDistance, float compensationRotRad, bool arg5) {
    const CVector dir = target->GetPosition() - GetPosition();
    // Como el original: suma en orden z, y, x
    const float dist = std::sqrt(dir.z * dir.z + dir.y * dir.y + dir.x * dir.x);

    if (!(dist <= maxDistance)) // Como el original: NaN tambien sale
        return;

    if (DoesTargetHaveToBeBroken(target, &GetActiveWeapon()))
        return;

    // Como el original: LimitRadianAngle inlinado (sin recorte), por 57.29578 (0x859878) y fchs si no es >= 0
    float angle = PlayerPedATanMinus(dir.x, dir.y, compensationRotRad);
    if (angle > PLAYER_PI) {
        do {
            angle -= PLAYER_TWO_PI;
        } while (angle > PLAYER_PI);
    }
    if (angle < -PLAYER_PI) {
        do {
            angle += PLAYER_TWO_PI;
        } while (angle < -PLAYER_PI);
    }
    float targetAngleDeg = angle * 57.2957763671875f;
    if (targetAngleDeg < 0.0f) // Como el original: NaN no cambia de signo
        targetAngleDeg = -targetAngleDeg;

    float viewAngleMultiplier = 1.0f - targetAngleDeg / PLAYER_MAX_TARGET_VIEW_ANGLE;
    if (dist > 1.0f)
        viewAngleMultiplier = viewAngleMultiplier / std::sqrt(std::sqrt(dist)); // Take quad root of dist

    const float targetPriority = FindTargetPriority(target) * viewAngleMultiplier;
    if (targetPriority > outTargetPriority && !LOSBlockedBetweenPeds(this, target)) {
        outTarget = target;
        outTargetPriority = targetPriority;
    }
}

// 0x60D1C0
void CPlayerPed::EvaluateNeighbouringTarget(CEntity* target, CEntity** outTarget, float* outTargetPriority, float maxDistance, float arg4, bool arg5) {
    plugin::CallMethod<0x60D1C0, CPlayerPed *, CEntity*, CEntity**, float*, float, float, bool>(this, target, outTarget, outTargetPriority, maxDistance, arg4, arg5);
}

// 0x60D350
void CPlayerPed::ProcessGroupBehaviour(CPad* pad) {
    plugin::CallMethod<0x60D350, CPlayerPed *, CPad*>(this, pad);
}

// 0x60D5A0
bool CPlayerPed::PlayerHasJustAttackedSomeone() {
    return PlayerWantsToAttack();
}

// 0x60D790
void CPlayerPed::SetupPlayerPed(int32 playerId) {
    auto ped = new CPlayerPed(playerId, false);
    // Como el original (0x60D7D9): playerId * 0x190 indexa directamente CWorld::Players (sin la sustitucion por
    // PlayerInFocus de FindPlayerInfo cuando playerId < 0)
    auto& playerInfo = CWorld::Players[playerId];
    playerInfo.m_pPed = ped;

    if (playerId == 1)
        ped->m_nPedType = PED_TYPE_PLAYER2;

    ped->SetOrientation(0.0f, 0.0f, 0.0f);
    CWorld::Add(ped);
    ped->m_nWeaponAccuracy = 100;
    playerInfo.m_nPlayerState = ePlayerState::PLAYERSTATE_PLAYING;
}

// 0x60D850
void CPlayerPed::ProcessWeaponSwitch(CPad* pad) {
    plugin::CallMethod<0x60D850, CPlayerPed *, CPad*>(this, pad);
}

// 0x60DC50
bool CPlayerPed::FindWeaponLockOnTarget() {
    return plugin::CallMethodAndReturn<bool, 0x60DC50, CPlayerPed *>(this);
}

// 0x60E530
bool CPlayerPed::FindNextWeaponLockOnTarget(CEntity* arg0, bool arg1) {
    return plugin::CallMethodAndReturn<bool, 0x60E530, CPlayerPed *, CEntity*, bool>(this, arg0, arg1);
}

// Como el original: fpatan (precisión completa) con el resultado redondeado a float al guardarlo
static float PlayerPedAtan2(float y, float x) {
    float result;
    __asm {
        fld y
        fld x
        fpatan
        fstp result
    }
    return result;
}

// 0x60EA90
void CPlayerPed::ProcessControl() {
    CPlayerPedData* pd = GetPlayerData();
    if (pd->m_nCarDangerCounter)
        pd->m_nCarDangerCounter--;
    if (!pd->m_nCarDangerCounter)
        pd->m_pDangerCar = nullptr;
    if (pd->m_nFadeDrunkenness) {
        if (static_cast<int32>(pd->m_nDrunkenness) - 1 > 0) {
            --pd->m_nDrunkenness;
        } else {
            pd->m_nDrunkenness = 0;
            CMBlur::ClearDrunkBlur();
            pd->m_nFadeDrunkenness = 0;
        }
    }
    if (pd->m_nDrunkenness) // Como el original: producto por 1/255 (0x859A3C)
        CMBlur::SetDrunkBlur(static_cast<float>(pd->m_nDrunkenness) * 0.003921568859368563f);
    if (pd->m_bRequireHandleBreath) {
        if (CStats::GetFatAndMuscleModifier(STAT_MOD_AIR_IN_LUNG) > pd->m_fBreath)
            pd->m_fBreath = (CTimer::GetTimeStep() + CTimer::GetTimeStep()) + pd->m_fBreath;
        pd->m_bRequireHandleBreath = false;
    }
    pd->m_bRequireHandleBreath = true;
    CPed::ProcessControl();
    // Como el original: or dword [0x478], 0x100 (bit 104 de las banderas del peatón)
    *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(this) + 0x478) |= 0x100;

    // Como el original: si el control se aplazó (bit 6 de las banderas de la entidad) no se hace nada más
    if (GetWasPostponed())
        return;

    CPad* pad = GetPadFromPlayer();
    GetWanted()->Update(); // Como el original: sin comprobar el puntero
    PruneReferences();

    if (GetActiveWeapon().m_Type == WEAPON_MINIGUN) {
        const auto* weaponInfo = CWeaponInfo::GetWeaponInfo(WEAPON_MINIGUN, eWeaponSkill::STD);
        CAnimBlendAssociation* animAssoc = GetIntelligence()->GetTaskUseGun() ? GetIntelligence()->GetTaskUseGun()->m_Anim : nullptr;
        if (animAssoc && animAssoc->m_CurrentTime - animAssoc->m_TimeStep < weaponInfo->m_fAnimLoopEnd) {
            if (pd->m_fGunSpinSpeed < 0.45f) { // 0x86D098
                pd->m_fGunSpinSpeed = CTimer::GetTimeStep() * 0.025f + pd->m_fGunSpinSpeed;
                if (pd->m_fGunSpinSpeed > 0.45f)
                    pd->m_fGunSpinSpeed = 0.45f;
            }
            if (pad->GetWeapon(this) && static_cast<int32>(GetActiveWeapon().m_TotalAmmo) > 0 && animAssoc->m_CurrentTime >= weaponInfo->m_fAnimLoopStart)
                m_weaponAudio.AddAudioEvent(AE_WEAPON_FIRE_MINIGUN_AMMO);
            else
                m_weaponAudio.AddAudioEvent(AE_WEAPON_FIRE_MINIGUN_NO_AMMO);
        } else if (pd->m_fGunSpinSpeed > 0.0f) {
            // Como el original: también se frena con tarea sin animación o fuera del bucle
            pd->m_fGunSpinSpeed = pd->m_fGunSpinSpeed - CTimer::GetTimeStep() * 0.003f;
            if (pd->m_fGunSpinSpeed < 0.0f)
                pd->m_fGunSpinSpeed = 0.0f;
        }
    }
    if (GetActiveWeapon().m_Type == WEAPON_CHAINSAW && m_nPedState != PEDSTATE_ATTACK && !bInVehicle) {
        GetIntelligence()->GetTaskSwim(); // Como el original: se llama y se ignora el resultado
    }
    if (m_pTargetedObject) {
        ClearReference(m_p3rdPersonMouseTarget);
        CVector effectPos{};
        float markColor = 1.0f;
        bool checkColor = true;
        switch (m_pTargetedObject->GetType()) {
        case ENTITY_TYPE_PED: {
            CPed* targetPed = m_pTargetedObject->AsPed();
            const auto* weaponInfo = CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, GetWeaponSkill());
            markColor = targetPed->m_fHealth / targetPed->m_fMaxHealth;
            const float targetHeadRange = weaponInfo->GetTargetHeadRange();
            // Como el original: el resto de casos (muerto, sin disparo instantáneo o fuera de rango) apunta al torso
            if (targetPed->IsAlive()
                && CWeaponInfo::GetWeaponInfo(GetActiveWeapon().m_Type, eWeaponSkill::STD)->m_nWeaponFire == WEAPON_FIRE_INSTANT_HIT
                && targetHeadRange * targetHeadRange > (targetPed->GetPosition() - GetPosition()).SquaredMagnitude()) {
                pd->m_nTargetBone = BONE_HEAD;
                pd->m_vecTargetBoneOffset.x = 0.05f;
            } else {
                pd->m_nTargetBone = BONE_SPINE1;
                pd->m_vecTargetBoneOffset.x = 0.2f;
            }
            effectPos = pd->m_vecTargetBoneOffset;
            targetPed->GetTransformedBonePosition(effectPos, static_cast<eBoneTag>(pd->m_nTargetBone), false);
            if (markColor > 0.0f && !targetPed->bInVehicle) {
                if (targetPed->m_nMoveState != PEDMOVE_STILL)
                    effectPos += targetPed->m_vecMoveSpeed * CTimer::GetTimeStep();
            } else if (targetPed->bInVehicle) {
                if (CVehicle* targetVeh = targetPed->m_pVehicle)
                    effectPos += (targetVeh->m_vecMoveSpeed + targetVeh->m_vecTurnSpeed) * CTimer::GetTimeStep();
            }
            break;
        }
        case ENTITY_TYPE_VEHICLE: {
            CVehicle* targetVeh = m_pTargetedObject->AsVehicle();
            markColor = targetVeh->m_fHealth * 0.001f; // Como el original: la salud del vehículo también colorea la marca
            effectPos = (targetVeh->m_vecMoveSpeed + targetVeh->m_vecTurnSpeed) * CTimer::GetTimeStep() + targetVeh->GetPosition();
            break;
        }
        case ENTITY_TYPE_OBJECT: {
            CObject* targetObj = m_pTargetedObject->AsObject();
            markColor = targetObj->m_fHealth * 0.001f;
            effectPos = targetObj->GetPosition() + targetObj->m_vecMoveSpeed * CTimer::GetTimeStep();
            break;
        }
        default:
            effectPos = m_pTargetedObject->GetPosition();
            checkColor = false;
            break;
        }

        uint8 r = 0, g = 0, b = 0;
        if (!checkColor || markColor > 0.0f) {
            if (checkColor && markColor > 1.0f)
                markColor = 1.0f;
            // Como el original: mezcla con los términos * 0.0f y _ftol2
            const float inv = 1.0f - markColor;
            r = static_cast<uint8>(static_cast<int64>(inv * 255.0f + markColor * 0.0f));
            g = static_cast<uint8>(static_cast<int64>(markColor * 255.0f + inv * 0.0f));
            b = static_cast<uint8>(static_cast<int64>(inv * 0.0f + markColor * 0.0f));
        }
        const CVector& pos = GetPosition();
        const float dx = effectPos.x - pos.x;
        const float dy = effectPos.y - pos.y;
        const float dz = effectPos.z - pos.z;
        const float dist = std::sqrt(dx * dx + dz * dz + dy * dy); // Como el original: orden x, z, y
        CWeaponEffects::MarkTarget(static_cast<CrossHairId>(m_nPedType), effectPos, r, g, b, 255u, 1.0f - dist * 0.02f, false);
    }

    switch (m_nMoveState) {
    case PEDMOVE_NONE:
        if (bInVehicle && m_pVehicle && !m_pVehicle->IsSubBMX())
            HandleSprintEnergy(false, 1.0f);
        break;
    case PEDMOVE_RUN:
        if (CStats::GetFatAndMuscleModifier(STAT_MOD_TIME_CAN_RUN) > pd->m_fTimeCanRun)
            pd->m_fTimeCanRun = CTimer::GetTimeStep() * 0.15f + pd->m_fTimeCanRun;
        break;
    case PEDMOVE_SPRINT:
        break;
    default:
        HandleSprintEnergy(false, 1.0f);
        break;
    }
    GetActiveWeapon().Update(this);
    if (m_nPedState == PEDSTATE_DEAD || m_nPedState == PEDSTATE_DIE) {
        ClearWeaponTarget();
        return;
    }
    if (pad) {
        if (pad->WeaponJustDown(this)) {
            const auto weaponType = GetActiveWeapon().m_Type;
            if (!TheCamera.Using1stPersonWeaponMode() || GetActiveWeapon().m_State == WEAPONSTATE_OUT_OF_AMMO) {
                if (!GetIntelligence()->GetTaskSwim()) {
                    if (weaponType == WEAPON_SNIPERRIFLE) {
                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_FIRE_FAIL_SNIPERRIFFLE, 0.0f, 1.0f);
                    } else if (weaponType == WEAPON_RLAUNCHER || weaponType == WEAPON_RLAUNCHER_HS) {
                        AudioEngine.ReportFrontendAudioEvent(AE_FRONTEND_FIRE_FAIL_ROCKET, 0.0f, 1.0f);
                    }
                }
            }
        }
        if (IsPedShootable() && m_nPedState != PEDSTATE_ANSWER_MOBILE) {
            if (!CWorld::Players[CWorld::FindPlayerSlotWithPedPointer(this)].m_pRemoteVehicle)
                ProcessWeaponSwitch(pad);
        }
    }
    ProcessAnimGroups();
    if (pad) {
        auto& activeCam = TheCamera.GetActiveCam();
        if (activeCam.m_nMode == MODE_FOLLOWPED && !activeCam.m_nDirectionWasLooking) {
            m_nLookTime = 0;
            float lookDir = CGeneral::LimitRadianAngle(PlayerPedAtan2(-activeCam.m_vecFront.x, activeCam.m_vecFront.y));
            const float angle = std::fabs(lookDir - m_fCurrentRotation);
            // Como el original: 30, 150, 210 y 330 grados son las constantes en radianes del EXE; NaN limpia la mirada
            if (m_nPedState != PEDSTATE_ATTACK && angle > 0.5235987901687622f && angle < 5.759586811065674f) {
                if (angle > 2.6179938316345215f && angle < 3.665191650390625f) {
                    const float dir1 = CGeneral::LimitRadianAngle(m_fCurrentRotation - 2.6179938316345215f);
                    const float dir2 = CGeneral::LimitRadianAngle(m_fCurrentRotation + 2.6179938316345215f);
                    if (std::bit_cast<uint32>(m_fLookDirection) != 0x497423F0u && !bIsDucking) { // 999999.0f comparado por bits
                        lookDir = std::fabs(dir2 - m_fLookDirection) > std::fabs(dir1 - m_fLookDirection) ? dir1 : dir2;
                    } else {
                        lookDir = dir1;
                    }
                }
                SetLookFlag(lookDir, true, false);
                // Como el original: el paso en ms se trunca a entero sin signo antes de multiplicarlo por 5
                const auto stepMs = static_cast<uint32>(static_cast<int64>(CTimer::GetTimeStep() * 0.02f * 1000.0f));
                SetLookTimer(static_cast<uint32>(static_cast<int64>(static_cast<float>(stepMs) * 5.0f)));
            } else {
                ClearLookFlag();
            }
        }
    }
    if (m_nMoveState == PEDMOVE_SPRINT && bIsLooking) {
        ClearLookFlag();
        SetLookTimer(250);
    }
    // Como el original: velocidad 2D; NaN reinicia el temporizador
    if (std::sqrt(m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y) < 0.1f) {
        if (!pd->m_nStandStillTimer) {
            pd->m_nStandStillTimer = CTimer::GetTimeInMS() + 500;
        } else if (CTimer::GetTimeInMS() > pd->m_nStandStillTimer) {
            pd->m_bStoppedMoving = true;
        }
    } else {
        pd->m_nStandStillTimer = 0;
        pd->m_bStoppedMoving = false;
    }
    if (pd->m_bDontAllowWeaponChange && IsPlayer()) {
        if (!CPad::GetPad(0)->GetTarget())
            pd->m_bDontAllowWeaponChange = false;
    }
    if (m_nPedState != PEDSTATE_SNIPER_MODE && GetActiveWeapon().m_State == WEAPONSTATE_FIRING)
        pd->m_nLastTimeFiring = CTimer::GetTimeInMS();
    ProcessGroupBehaviour(pad);
    if (bInVehicle)
        CCarCtrl::RegisterVehicleOfInterest(m_pVehicle);
    if (!GetIsVisible())
        UpdateRpHAnim();
    if (bInVehicle) {
        if (CPad::GetPad(0)->IsDPadDownPressed()) {
            pd->m_bPlayersGangActive = false;
        } else if (CPad::GetPad(0)->IsDPadUpPressed()) {
            pd->m_bPlayersGangActive = true;
        }
    }
    if (physicalFlags.bSubmergedInWater) {
        const CVector pos = GetPosition();
        if (CWaterLevel::GetWaterLevel(pos.x, pos.y, pos.z + 1.5f, pd->m_fWaterHeight, true, nullptr)) {
            // Como el original: los límites usan la z sin el 1.5 añadido
            const float playerMinZ = pos.z + GetColModel()->GetBoundingBox().m_vecMin.z;
            const float playerMaxZ = pos.z + GetColModel()->GetBoundingBox().m_vecMax.z;
            const float waterHeight = pd->m_fWaterHeight;
            if (!(waterHeight < playerMaxZ) && !std::isunordered(waterHeight, playerMaxZ)) {
                pd->m_nWaterCoverPerc = 100;
            } else if (waterHeight > playerMinZ || std::isunordered(waterHeight, playerMinZ)) {
                pd->m_nWaterCoverPerc = static_cast<uint8>(static_cast<int64>((waterHeight - playerMinZ) / (playerMaxZ - playerMinZ) * 100.0f));
            } else {
                pd->m_nWaterCoverPerc = 0;
            }
        } else {
            physicalFlags.bSubmergedInWater = false;
        }
    } else {
        pd->m_nWaterCoverPerc = 0;
    }
    if ((CTimer::GetFrameCounter() & 0x7F) == 0 && !FindPlayerVehicle(-1, false)) {
        auto& group = CPedGroups::GetGroup(pd->m_nPlayerGroup);
        if (group.m_bMembersEnterLeadersVehicle) {
            const int32 memberCount = group.GetMembership().CountMembersExcludingLeader();
            // Como el original: FindDistanceToNearestMember se llama dos veces
            if (memberCount > 0 && group.FindDistanceToNearestMember(nullptr) > 20.0f && group.FindDistanceToNearestMember(nullptr) < 100.0f
                && CGame::currArea == AREA_CODE_NORMAL_WORLD) {
                Say(memberCount == 1 ? CTX_GLOBAL_ORDER_KEEP_UP_ONE : CTX_GLOBAL_ORDER_KEEP_UP_MANY);
                for (int32 i = 0; i < TOTAL_PED_GROUP_FOLLOWERS; ++i) {
                    CPed* member = group.GetMembership().GetMember(i);
                    if (member && CGeneral::GetRandomNumberInRange(0.0f, 1.0f) < 0.5f) {
                        member->Say(CTX_GLOBAL_FOLLOW_REPLY, CGeneral::GetRandomIntInRange(3000, 4500));
                    }
                }
            }
        }
    }
    if (!bInVehicle && GetLightingTotal() <= 0.05f && !CEntryExitManager::WeAreInInteriorTransition())
        Say(CTX_GLOBAL_BREATHING);
}

// 0x609490
void CPlayerPed::SetMoveAnim() {
    //nop
}
