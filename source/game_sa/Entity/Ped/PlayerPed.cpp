/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlayerPed
 * Reconstrucción: COMPLETA — 58/58 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 58/58 entradas.
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
#include "X87Math.h"
#include "PedGroups.h"
#include "PedGroup.h"
#include "PedGroupMembership.h"
#include "PedGroupIntelligence.h"
#include "Stats.h"
#include "Radar.h"
#include "Audio/Entities/AEPedSpeechAudioEntity.h"
#include "Events/EventScriptCommand.h"
#include "Events/EventGroupEvent.h"
#include "Events/EventNewGangMember.h"
#include "Events/EventDontJoinPlayerGroup.h"
#include "Tasks/TaskTypes/TaskComplexBeInGroup.h"
#include "Pools/PedPool.h"
#include "Pools/ObjectPool.h"
#include "Pools/VehiclePool.h"
#include "GameLogic.h"
#include "World.h"
#include "WeaponInfo.h"
#include "PedType.h"
#include "Wanted.h"
#include "Fire.h"
#include "PedIntelligence.h"
#include "Tasks/TaskTypes/TaskSimplePlayerOnFoot.h"
#include "Tasks/TaskTypes/TaskComplexFacial.h"
#include "Plugins/RpAnimBlendPlugin/RpAnimBlend.h"

// Ayudante de X87Math.cpp (PlayerPedSprintTailDblCpp) que devuelve un double: el ((max(ratio - 1, 0) * k) + 1) de CPlayerPed::ControlButtonSprint queda sin guardar
// en ST0 (0x60A7D3). Aqui se declara como float (alias del enlazador) para que el llamador reciba ST0 tal cual, como el EXE (mismo mecanismo que Placeable.cpp).
extern "C" float __cdecl PlayerPedSprintTailFltCpp(float ratio, float k);
#pragma comment(linker, "/alternatename:_PlayerPedSprintTailFltCpp=_PlayerPedSprintTailDblCpp")

bool CPlayerPed::bDebugPlayerInvincible;
bool CPlayerPed::bDebugTargeting;
bool CPlayerPed::bDebugTapToTarget;

// Como el original (0x60A820): solo escribe EAX, ECX (= datos del jugador, +0x480) y ST0, y deja EDX intacto; su llamador en 0x60B430
// conserva en EDX un puntero a la animacion a traves de la llamada. Este puente __fastcall recibe el EDX de entrada y termina en
// PlayerPedResidueFloatCpp (ECX = datos del jugador, EDX = el de entrada, ST0 = resultado), asi que no usa ensamblador.
// Sin comprobaciones de ejecucion (RTC) ni cookie de /GS para que no cambien ECX/EDX tras la ultima llamada.
#pragma runtime_checks("", off)
static float __declspec(safebuffers) __fastcall GetButtonSprintResultsKeepEdx(CPlayerPed* self, uint32 entryEdx, eSprintType sprintType) {
    const float result = self->GetButtonSprintResults(sprintType);
    return PlayerPedResidueFloatCpp(static_cast<uint32>(reinterpret_cast<uintptr_t>(self->GetPlayerData())), entryEdx, result);
}
#pragma runtime_checks("", restore)

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
    RH_ScopedInstall(ControlButtonSprint, 0x60A610);
    RH_ScopedNamedGlobalInstall(GetButtonSprintResultsKeepEdx, "GetButtonSprintResults", 0x60A820);
    RH_ScopedInstall(HandlePlayerBreath, 0x60A8D0);
    RH_ScopedOverloadedInstall(MakeChangesForNewWeapon, "", 0x60B460, void(CPlayerPed::*)(eWeaponType));
    RH_ScopedGlobalInstall(LOSBlockedBetweenPeds, 0x60B550);
    RH_ScopedInstall(DoesTargetHaveToBeBroken, 0x60C0C0);
    RH_ScopedInstall(SetPlayerMoveBlendRatio, 0x60C520);
    RH_ScopedInstall(FindPedToAttack, 0x60C5F0);
    RH_ScopedInstall(PlayerWantsToAttack, 0x60CC50);
    RH_ScopedInstall(ForceGroupToAlwaysFollow, 0x60C7C0);
    RH_ScopedInstall(ForceGroupToNeverFollow, 0x60C800);
    RH_ScopedInstall(MakeThisPedJoinOurGroup, 0x60C840);
    RH_ScopedInstall(SetInitialState, 0x60CD20);
    RH_ScopedOverloadedInstall(MakeChangesForNewWeapon, "BySlot", 0x60D000, void(CPlayerPed::*)(uint32));
    RH_ScopedInstall(EvaluateTarget, 0x60D020);
    RH_ScopedInstall(EvaluateNeighbouringTarget, 0x60D1C0);
    RH_ScopedInstall(ProcessGroupBehaviour, 0x60D350);
    RH_ScopedInstall(PlayerHasJustAttackedSomeone, 0x60D5A0);
    RH_ScopedInstall(SetupPlayerPed, 0x60D790);
    RH_ScopedInstall(FindWeaponLockOnTarget, 0x60DC50);

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
    CPlayerPedData* data = GetPlayerData();
    if (!data)
        return 0.0f; // 0x60A61E

    // Como el original: GetPad(0) / GetPad(1) segun el tipo de ped (0x598); cualquier otro tipo deja el pad en nullptr y las llamadas a CPad lo desreferencian
    CPad* pad = nullptr;
    if (m_nPedType == PED_TYPE_PLAYER1)
        pad = CPad::GetPad(0);
    else if (m_nPedType == PED_TYPE_PLAYER2)
        pad = CPad::GetPad(1);

    // Como el original (0x60A663..0x60A689): comparaciones ordenadas (con NaN, false)
    const bool canSprint = !data->m_bPlayerSprintDisabled && (data->m_fMoveSpeed > 0.0f || data->m_fTimeCanRun > 0.0f);
    const auto& sprintSet = PLAYER_SPRINT_SET[sprintType]; // 0x8D2460 + tipo * 32 (sin comprobar el rango, como el original)

    // Como el original: SprintJustDown (0x5407F0) se llama siempre y, si no aplica la primera rama, GetSprint (0x5407A0) tambien
    if (pad->SprintJustDown() && canSprint) {
        data->m_fMoveSpeed = PlayerPedSprintAddClampCpp(sprintSet.field_0, data->m_fMoveSpeed, sprintSet.field_10); // 0x60A69E..0x60A757
    } else if (pad->GetSprint() && canSprint) {
        data->m_fMoveSpeed = PlayerPedSprintDecayCpp(CTimer::GetTimeStep(), sprintSet.field_4, data->m_fMoveSpeed, 1.0f); // 0x60A6E3..0x60A757
    } else if (data->m_fMoveSpeed > 0.0f) {
        data->m_fMoveSpeed = PlayerPedSprintDecayCpp(CTimer::GetTimeStep(), sprintSet.field_8, data->m_fMoveSpeed, 0.0f); // 0x60A716..0x60A757
    }

    // 0x60A75A: el EXE vuelve a leer la velocidad
    const float speed = data->m_fMoveSpeed;
    float ratio;
    float adrenaline;
    if (speed > sprintSet.field_C) {
        // Como el original: ratio > 0 (0x60A78A) es siempre cierto con velocidad > field_C > 0
        ratio      = PlayerPedSprintRatioCpp(speed, sprintSet.field_C);
        adrenaline = sprintSet.field_18;
    } else if (speed > 0.0f && canSprint) {
        ratio      = 1.0f; // 0x60A7F5
        adrenaline = sprintSet.field_14;
    } else {
        return 0.0f; // 0x60A810
    }

    if (HandleSprintEnergy(true, adrenaline)) // 0x60A550
        return PlayerPedSprintTailFltCpp(ratio, sprintSet.field_1C); // 0x60A7A9..0x60A7D3, ST0 sin guardar

    data->m_fMoveSpeed = 0.0f; // 0x60A809
    return 0.0f;
}

// 0x60A820
float CPlayerPed::GetButtonSprintResults(eSprintType sprintType) {
    const float speed = GetPlayerData()->m_fMoveSpeed;
    if (speed > PLAYER_SPRINT_THRESHOLD) {
        // Como el original: (velocidad / 5 - 1) * k + 1 con un redondeo a 24 bits por operacion en la x87. La rama
        // "si t < 0 entonces t = 0" del EXE (0x60A84B) es inalcanzable: con velocidad > 5, velocidad / 5 >= 1 y t >= 0.
        return PlayerPedSprintResultCpp(speed, PLAYER_SPRINT_THRESHOLD, PLAYER_SPRINT_SET[sprintType].field_1C);
    }
    return speed > 0.0f ? 1.0f : 0.0f; // Como el original: NaN y negativos dan 0
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

namespace {
// 0x609380 (cdecl, sin argumentos; devuelve AL): (byte 0x96917C | byte 0x96917D | byte 0x96917E) != 0. Sin reconstruir: se llama por la direccion del EXE.
bool JoinGroupFlagsExe() {
    return plugin::CallAndReturn<bool, 0x609380>();
}
} // namespace

// 0x60C840 (thiscall, `ret 4`; EAX/ECX/EDX los recarga el llamador): el jugador recluta a `ped` en su grupo. Orden de llamadas, relecturas de memoria (datos del jugador, indice del grupo
// y banderas del peaton) como el EXE. Sale sin hacer nada si el peaton va drogado (solo dice una frase), tiene la tarea 1000 activa, no es del tipo 8 y las tres banderas globales
// 0x96917C..E estan a cero, o ya es miembro del grupo. Con sitio libre (el limite sale de CStats::FindMaxNumberOfGroupMembers, el limite del script y dos comprobaciones de la estadistica
// 0xB5), el peaton sale de su grupo, recibe CTaskComplexBeInGroup mediante un CEventScriptCommand (ranura 3), pasa a seguidor, el grupo se actualiza, se avisa con un CEventGroupEvent
// de un CEventNewGangMember, y se le marca en el radar; sin sitio recibe un CEventDontJoinPlayerGroup.
void CPlayerPed::MakeThisPedJoinOurGroup(CPed* ped) {
    if (ped->bDruggedUp) { // test ah,4 de [ped + 0x478]
        Say(CTX_GLOBAL_DRUGGED_IGNORE, 0, 1.0f, false, false, false); // 0x5EFFE0 (0x52)
        return;
    }
    if (ped->GetTaskManager().FindActiveTaskByType(static_cast<eTaskType>(1000))) { // 0x681740
        return;
    }
    // El EXE relee los datos del jugador (+0x480) y el indice de grupo (+0x38) en cada uso: la membresia del grupo es la de la ranura 0xC09928 + indice * 0x2D4
    const auto group = [this]() -> CPedGroup& { return CPedGroups::ms_groups.data()[GetPlayerData()->m_nPlayerGroup]; };
    if (static_cast<int32>(ped->m_nPedType) != 8 && !StaticRef<uint8>(0x96917C) && !StaticRef<uint8>(0x96917D) && !StaticRef<uint8>(0x96917E)) {
        return;
    }
    if (group().GetMembership().IsMember(ped)) { // 0x5F6A10
        return;
    }

    CAEPedSpeechAudioEntity::SetCJMood(static_cast<eCJMood>(-1), 10000, 1, -1, -1); // 0x4E3ED0
    Say(CTX_GLOBAL_JOIN_ME_ASK, 0, 1.0f, true, false, false); // 0x5EFFE0 (0x7F)

    int32 limit = CStats::FindMaxNumberOfGroupMembers(); // 0x559A50
    {
        CPlayerPedData* const data = GetPlayerData();
        if (limit >= static_cast<int32>(data->m_nScriptLimitToGangSize)) {
            limit = data->m_nScriptLimitToGangSize;
        }
    }
    if (CStats::GetStatValue(static_cast<eStats>(0xB5)) == 1.0f || CStats::GetStatValue(static_cast<eStats>(0xB5)) == 2.0f) { // 0x558E40 (0x858624 y 0x858CA0)
        limit = 0;
    }

    bool join = false;
    if (JoinGroupFlagsExe()) { // 0x609380
        join = group().GetMembership().CountMembersExcludingLeader() < 7; // 0x5F6AA0
    }
    if (!join) {
        if (group().GetMembership().CountMembersExcludingLeader() >= limit || group().GetMembership().CountMembersExcludingLeader() >= limit) {
            // Sin sitio: el peaton se niega (0x7D) y recibe CEventDontJoinPlayerGroup en su grupo de eventos
            ped->Say(CTX_GLOBAL_JOIN_GANG_NO, 2500, 1.0f, true, false, false);
            CEventDontJoinPlayerGroup event{ this }; // 0x6090E0
            ped->GetEventGroup().Add(&event, false); // 0x4AB420
            return;
        }
    }

    if (CPedGroup* const current = CPedGroups::GetPedsGroup(ped)) { // 0x5F7E80
        current->GetMembership().RemoveMember(ped); // 0x5FB210
    }
    CTask* task = nullptr;
    if (void* const memory = CTask::operator new(sizeof(CTaskComplexBeInGroup))) { // 0x61A5A0
        const int32 groupId = static_cast<int32>(FindPlayerPed(-1)->GetPlayerData()->m_nPlayerGroup); // 0x56E210
        task = ::new (memory) CTaskComplexBeInGroup(groupId, false); // 0x632E50
    }
    CEventScriptCommand scriptEvent{ 3, task, false }; // 0x4B0A00 (ranura primaria 3)
    ped->GetEventGroup().Add(&scriptEvent, false); // 0x4AB420

    group().GetMembership().AddFollower(ped); // 0x5F8020
    group().Process(); // 0x5FC7E0
    ped->GiveWeaponWhenJoiningGang(); // 0x5E8BE0

    CEvent* newMember = nullptr;
    if (void* const memory = CEvent::operator new(sizeof(CEventNewGangMember))) { // 0x4B5620
        newMember = ::new (memory) CEventNewGangMember(ped); // 0x608F70
    }
    CEventGroupEvent groupEvent{ this, newMember }; // 0x4ADFD0
    group().GetIntelligence().AddEvent(&groupEvent); // 0x5F7470

    ped->bDrownsInWater = false; // and [ped + 0x470], 0xFFEFFFFF
    CStats::IncrementStat(static_cast<eStats>(0xEE), 1.0f); // 0x55C180
    CStats::DisplayScriptStatUpdateMessage(static_cast<eStatUpdateState>(1), static_cast<eStats>(0x150), 1.0f); // 0x55B980

    // Color del blip: tres bytes de la tabla de colores de bandas (indice 1) en 0x8D1345, 0x8D1351 y 0x8D135D, mas alfa 0xFF
    const uint32 colorA = StaticRef<uint8>(0x8D1345);
    const uint32 colorB = StaticRef<uint8>(0x8D1351);
    const uint32 colorC = StaticRef<uint8>(0x8D135D);
    const uint32 color = (((((colorA << 8) | colorB) << 8) | colorC) << 8) | 0xFFu;
    const int32 ref = static_cast<int32>(GetPedPool()->GetRef(ped)); // 0x4442D0
    const auto blip = CRadar::SetEntityBlip(BLIP_CHAR, ref, color, BLIP_DISPLAY_BLIPONLY); // 0x5839A0
    CRadar::ChangeBlipScale(blip, 2); // 0x583CC0
    CRadar::ChangeBlipColour(blip, static_cast<eBlipColour>(color)); // 0x583AB0
    CRadar::SetBlipFriendly(blip, true); // 0x583EB0

    ped->bClearRadarBlipOnDeath = true; // or [ped + 0x474], 0x2000
    const uint32 slot = GetPlayerData()->m_nPlayerGroup;
    const uint32 separation = StaticRef<uint32>(0x86C6C0);
    std::memcpy(reinterpret_cast<uint8*>(&CPedGroups::ms_groups.data()[slot].GetMembership()) + 0x24, &separation, sizeof(separation)); // separacion maxima del grupo del jugador (+0x24)
    ped->Say(CTX_GLOBAL_JOIN_GANG_YES, 2500, 1.0f, true, false, false); // 0x5EFFE0 (0x7E)
}

// 0x60CC50
bool CPlayerPed::PlayerWantsToAttack() {
    // Como el original (0x60CC50..0x60CD17): el grupo del jugador se indexa sin comprobar el rango (imul 0x2D4 + 0xC09920) y la funcion no deja un valor definido en EAX
    // (el unico llamador del EXE, 0x685A7A via 0x60D5A0, no lo lee): se devuelve false.
    auto& group = CPedGroups::ms_groups.data()[GetPlayerData()->m_nPlayerGroup];
    if (group.GetMembership().CountMembersExcludingLeader() < 1) {
        return false;
    }
    if (!group.m_bMembersEnterLeadersVehicle) {
        return false;
    }
    group.GetIntelligence().ReportAllBarScriptTasksFinished();

    // Objetivo: el fijado o, con raton en tercera persona y sin fijar, el del raton
    CEntity* target = m_pTargetedObject;
    if (CCamera::m_bUseMouse3rdPerson && !target) {
        target = m_p3rdPersonMouseTarget;
    }

    CPed* victim = nullptr;
    if (target) {
        if (target->GetType() == ENTITY_TYPE_PED) {
            victim = static_cast<CPed*>(target);
        } else {
            if (CTagManager::IsTag(*target)) {
                return false;
            }
            if (target->GetType() == ENTITY_TYPE_OBJECT && static_cast<CObject*>(target)->CanBeTargetted()) {
                return false;
            }
        }
    }
    if (!victim) {
        victim = FindPedToAttack();
    }
    if (!victim) {
        return false;
    }
    group.PlayerGaveCommand_Attack(this, victim);
    return false;
}

// 0x60CD20 (thiscall, `ret 4`, void): estado inicial del jugador. Los datos del jugador (+0x480) y las ranuras de tareas se releen en cada uso, como el original.
void CPlayerPed::SetInitialState(bool bGroupCreated) {
    static_assert(ANIM_GROUP_PLAYER == 0x36);
    static_assert(offsetof(CPed, m_nAnimGroup) == 0x4D4 && offsetof(CPed, m_pStats) == 0x59C && offsetof(CPed, m_pFire) == 0x730 && offsetof(CPed, m_nTurretAmmo) == 0x784 && offsetof(CPed, m_pAttachedTo) == 0xFC,
                  "campos de CPed que SetInitialState toca");
    CMBlur::ClearDrunkBlur(); // 0x71D780
    CTimer::ms_fTimeScale = 1.0f; // 0xB7CB64
    m_bUsesCollision = true; // or [this + 0x1C], 1
    physicalFlags.bApplyGravity = true; // or [this + 0x40], 2
    ClearAimFlag();  // 0x5DEF20
    ClearLookFlag(); // 0x5E1950
    bRenderPedInCar = true; // or [this + 0x46C], 0x2000
    if (m_pFire) {
        m_pFire->Extinguish(); // 0x5393F0
    }
    SetPedState(PEDSTATE_IDLE);  // 0x5E4500 (1)
    SetMoveState(PEDMOVE_STILL); // 0x5DEC00 (1)
    // Como el original: and [this + 0x474], ~0x42 / or 0x4000000; and [this + 0x46C], ~0x4000000
    bDontRender = false;
    bIsBeingArrested = false;
    bCanExitCar = true;
    bIsDucking = false;
    GetIntelligence()->FlushIntelligence(); // 0x601DA0
    RpAnimBlendClumpRemoveAllAssociations(reinterpret_cast<RpClump*>(GetRwObject())); // 0x4D6C00 (+0x18; sin la comprobacion de tipo de GetRpClump)

    CTask* playerOnFoot = nullptr;
    if (void* const memory = CTask::operator new(sizeof(CTaskSimplePlayerOnFoot))) { // 0x61A5A0 (0x1C)
        playerOnFoot = ::new (memory) CTaskSimplePlayerOnFoot(); // 0x685750
    }
    GetIntelligence()->GetTaskManager().SetTask(playerOnFoot, TASK_PRIMARY_DEFAULT, false); // 0x681AF0 (4)

    bIsPedDieAnimPlaying = false; // and [this + 0x46C], ~0x100000
    m_nAnimGroup = ANIM_GROUP_PLAYER;
    if (CPlayerPedData* const data = GetPlayerData()) {
        data->m_bAdrenaline = false; // and [datos + 0x34], ~2
    }
    SetRealMoveAnim(); // 0x60A9C0 (sin reconstruir)
    m_pStats->m_nTemper = 50; // mov byte [m_pStats + 0x25], 0x32
    if (m_pAttachedTo) {
        m_bUsesCollision = true; // or [this + 0x1C], 1 (solo si aun no lo esta)
    }
    m_pAttachedTo = nullptr;
    m_nTurretAmmo = 0;

    CTask* facial = nullptr;
    if (void* const memory = CTask::operator new(sizeof(CTaskComplexFacial))) { // 0x61A5A0 (0x20)
        facial = ::new (memory) CTaskComplexFacial(); // 0x690D20
    }
    GetIntelligence()->GetTaskManager().SetTaskSecondary(facial, TASK_SECONDARY_FACIAL_COMPLEX); // 0x681B60 (3)

    if (!bGroupCreated && !GetPlayerData()->m_bGroupNeverFollow) { // bit 0x400 de las banderas de los datos
        // Como el original: cada acceso al grupo del jugador relee [this + 0x480] + 0x38
        GetPlayerGroup().m_bMembersEnterLeadersVehicle = true; // byte +4 del grupo
        GetPlayerGroup().GetIntelligence().SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM); // 0x5FBB70 (5)
        CEventPlayerCommandToGroupAttack command{ nullptr }; // 0x4B23D0 (0, 0); vtable 0x86C710
        command.ComputeResponseTaskType(&GetPlayerGroup()); // 0x4B57A0
        if (command.WillRespond()) { // 0x4AC490
            auto* const gather = new CEventPlayerCommandToGroupGather(nullptr); // 0x4B5620 (0x1C) + 0x4B23D0 (2, 0); vtable 0x86D120
            gather->m_TaskId = command.m_TaskId;
            CEventGroupEvent groupEvent{ this, gather }; // 0x4ADFD0
            GetPlayerGroup().GetIntelligence().AddEvent(&groupEvent); // 0x5F7470
        }
    }
    if (CPlayerPedData* const data = GetPlayerData()) {
        data->SetInitialState(); // 0x56F210
    }
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
    return PlayerPedAtanMinusCpp(&CGeneral::GetATanOfXY, x, y, compensation);
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
    // Como el original (0x60D1C0..0x60D341): distancia (z2 + y2) + x2 con la x87 a 24 bits (X87Math.cpp); NaN sale
    if (!Hp66NeighbourInRangeCpp(&target->GetPosition().x, &GetPosition().x, maxDistance))
        return;

    // Como el original (movsx eax, byte [this+0x718]; imul eax, eax, 0x1C; lea edx, [eax+this+0x5A0]): ranura con signo y sin comprobar el rango (GetActiveWeapon() si lo comprueba)
    if (DoesTargetHaveToBeBroken(target, &m_aWeapons.data()[static_cast<int8>(m_nActiveWeaponSlot)]))
        return;

    // Rumbo respecto a la camara (no al jugador): GetATanOfXY (0x53CC70) deja el angulo en ST0 y el EXE le resta arg4 sin redondear antes
    const CVector& targetPos = target->GetPosition();
    const CVector& camPos    = TheCamera.GetPosition();
    float angle = Hp66NeighbourAngleCpp(&CGeneral::GetATanOfXY, &targetPos.x, &camPos.x, arg4);
    // Normalizado a [-pi, pi] sin recorte: los bucles del EXE no terminan con |angulo| infinito (como el original)
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

    const float absAngle = angle < 0.0f ? -angle : angle; // fchs solo si es menor que 0 (-0 y NaN se conservan)
    if (!(absAngle < 0.8726646304130554f)) // 0x86D1D0 (50 grados); NaN tambien sale
        return;

    // Prioridad: -100000.0 (0x86D1E8) salvo en el lado elegido (arg5 = derecha: angulo > 0, prioridad -angulo; si no, izquierda: angulo < 0, prioridad angulo)
    float priority;
    if (arg5) {
        priority = angle > 0.0f ? -angle : -100000.0f;
    } else {
        priority = angle < 0.0f ? angle : -100000.0f;
    }
    if (priority > *outTargetPriority) { // comparacion ordenada: con NaN no entra
        *outTarget         = target;
        *outTargetPriority = priority;
    }
}

// 0x60D350 (thiscall, `ret 4`): botones del pad para el grupo del jugador. Cruceta arriba: al mantenerla 1200 ms se disuelve el grupo; si se suelta antes, el peaton apuntado
// se une al grupo (o el grupo empieza a seguir al jugador). Cruceta abajo: igual, pero sin la comprobacion de reclutamiento deshabilitado en la pulsacion larga. Cada 32 fotogramas
// (fotograma 6) los peatones de tipo 8 pasan a odiar o dejar de odiar a los policias segun el nivel de busqueda del jugador.
void CPlayerPed::ProcessGroupBehaviour(CPad* pad) {
    constexpr uint32 GROUP_STUFF_DISABLED = 0x100;   // bit 8 de CPlayerPedData::m_nPlayerFlags
    constexpr uint16 HOLD_TIME            = 0x4B0;   // 1200 ms

    // Como el original: el objetivo (ebp) se lee una sola vez; con el raton en 3a persona (0xB6EC2E) y sin objetivo se usa el del raton (+0x79C)
    CEntity* target = m_pTargetedObject;
    if (StaticRef<bool>(0xB6EC2E) && !target) {
        target = m_p3rdPersonMouseTarget;
    }
    const auto TargetIsPed = [&] { return target && target->GetType() == ENTITY_TYPE_PED; };
    // Como el original: _ftol2 (0x821B40) de paso_sin_recortar * 0.02f (0x858B38) * 1000 (0x858C4C); se suma a un uint16 (add word ptr)
    const auto HoldStep = [] {
        const float step = (CTimer::ms_fTimeStepNonClipped * StaticRef<float>(0x858B38)) * StaticRef<float>(0x858C4C);
        return static_cast<uint16>(static_cast<uint32>(static_cast<int64>(step)));
    };

    if (!FindPlayerVehicle(-1, false)) { // 0x56E0D0
        const bool up = pad->GetGroupControlForward(); // 0x541190
        CPlayerPedData* const data = GetPlayerData(); // edi: se conserva hasta el final de esta rama
        if (up) {
            data->m_nPadUpPressedInMilliseconds += HoldStep();
            CPlayerPedData* const now = GetPlayerData();
            if (now->m_nPadUpPressedInMilliseconds == HOLD_TIME && !(now->m_nPlayerFlags & GROUP_STUFF_DISABLED)) {
                DisbandPlayerGroup(); // 0x60A0A0
            }
        } else {
            const uint16 held = data->m_nPadUpPressedInMilliseconds;
            if (held != 0 && held < HOLD_TIME) {
                if (TargetIsPed() && (static_cast<CPed*>(target)->m_nPedType == 8 || JoinGroupFlagsExe())) { // 0x609380 solo si el tipo no es 8
                    if (!(data->m_nPlayerFlags & GROUP_STUFF_DISABLED)) { // con el puntero leido antes de las llamadas
                        MakeThisPedJoinOurGroup(static_cast<CPed*>(target)); // 0x60C840
                    }
                } else {
                    TellGroupToStartFollowingPlayer(true, true, false); // 0x60A1D0
                }
            }
            GetPlayerData()->m_nPadUpPressedInMilliseconds = 0;
        }
    }

    if (GetPlayerData()->m_nPlayerFlags & GROUP_STUFF_DISABLED) {
        return;
    }

    if (!FindPlayerVehicle(-1, false)) { // 0x56E0D0
        const bool down = pad->GetGroupControlBack(); // 0x5411B0
        if (down) {
            GetPlayerData()->m_nPadDownPressedInMilliseconds += HoldStep();
            if (GetPlayerData()->m_nPadDownPressedInMilliseconds >= HOLD_TIME) {
                DisbandPlayerGroup(); // 0x60A0A0
            }
        } else {
            const uint16 held = GetPlayerData()->m_nPadDownPressedInMilliseconds;
            if (held != 0 && held < HOLD_TIME) {
                if (TargetIsPed() && static_cast<CPed*>(target)->m_nPedType == 8) {
                    MakeThisPedJoinOurGroup(static_cast<CPed*>(target)); // 0x60C840
                } else {
                    TellGroupToStartFollowingPlayer(false, true, false); // 0x60A1D0
                }
            }
            GetPlayerData()->m_nPadDownPressedInMilliseconds = 0;
        }
    }

    // Cada 32 fotogramas (0xB7CB4C & 0x1F == 6), de atras adelante por el pool de peatones (0xB74490): los de tipo 8 odian a los policias con nivel de busqueda > 0
    if ((CTimer::GetFrameCounter() & 0x1F) != 6) {
        return;
    }
    CPedPool* const pool = StaticRef<CPedPool*>(0xB74490);
    for (size_t slot = pool->GetSize(); slot != 0; --slot) {
        CPed* const ped = pool->GetAt(slot - 1);
        if (!ped || ped->m_nPedType != 8) {
            continue;
        }
        // Como el original: nivel de busqueda del jugador (FindPlayerPed(-1)->datos->CWanted, +0x2C) antes de pedir la bandera del tipo de policia
        CPlayerPedData* const playerData = FindPlayerPed(-1)->GetPlayerData(); // 0x56E210
        const CWanted* const wanted      = playerData ? playerData->m_pWanted : nullptr;
        const int32 wantedLevel          = static_cast<int32>(wanted->m_WantedLevel);
        const uint32 copFlag             = CPedType::GetPedFlag(static_cast<ePedType>(6)); // 0x608830
        if (wantedLevel > 0) {
            ped->m_acquaintance.SetAsAcquaintance(4, copFlag); // 0x608DA0
        } else {
            ped->m_acquaintance.ClearAsAcquaintance(4, copFlag); // 0x608980
        }
    }
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
// 0x60DC50 (thiscall, bool): busca el objetivo de fijacion del arma activa. Primero conserva (o suelta) el objetivo fijado; sin objetivo, con el bote de spray busca etiquetas cercanas y despues evalua
// peatones, objetos y (en cooperativo) vehiculos con EvaluateTarget. Las lecturas del arma, de los datos del jugador y de la informacion del arma se repiten donde las repite el original.
bool CPlayerPed::FindWeaponLockOnTarget() {
    static_assert(offsetof(CPed, m_pVehicle) == 0x58C && offsetof(CPed, m_nPedType) == 0x598 && offsetof(CPed, m_nPedState) == 0x530 && offsetof(CPed, m_nActiveWeaponSlot) == 0x718 && offsetof(CPed, m_pTargetedObject) == 0x71C,
                  "campos de CPed que FindWeaponLockOnTarget toca");
    static_assert(offsetof(CVehicle, m_nVehicleType) == 0x590, "CVehicle::m_nVehicleType");
    static_assert(offsetof(CVehicle, m_nVehicleSubType) == 0x594, "CVehicle::m_nVehicleSubType");
    static_assert(PEDSTATE_DIE == 0x36, "PEDSTATE_DIE");
    static_assert(PEDSTATE_DEAD == 0x37, "PEDSTATE_DEAD");
    static_assert(WEAPON_SPRAYCAN == 41, "WEAPON_SPRAYCAN");
    // Como el original: movsx de la ranura (con signo) y imul 0x1C, sin comprobar el rango
    const auto activeWeapon = [this]() -> CWeapon& { return m_aWeapons.data()[static_cast<int8>(m_nActiveWeaponSlot)]; };
    const auto setTarget = [this](CEntity* const entity) {
        if (m_pTargetedObject) {
            m_pTargetedObject->CleanUpOldReference(&m_pTargetedObject); // 0x571A00
        }
        m_pTargetedObject = entity;
        entity->RegisterReference(&m_pTargetedObject); // 0x571B70
        reinterpret_cast<uint8*>(GetPlayerData())[0x85] = 1; // byte +0x85 de los datos del jugador
    };

    const eWeaponType weaponType = activeWeapon().m_Type;
    const eWeaponSkill skill = GetWeaponSkill(); // 0x5E6580
    CWeaponInfo* const info = CWeaponInfo::GetWeaponInfo(weaponType, skill); // 0x743C60
    const float range0 = info->m_fTargetRange; // +4: copia que usa el bote de spray

    // ---- ya hay objetivo: se conserva mientras siga dentro del alcance
    if (CEntity* const target = m_pTargetedObject) {
        // Como el original: sqrt(dy*dy + dx*dx) con la x87 a 24 bits (exponente extendido), guardado como float
        const float distance = Pp79TargetDistCpp(&target->GetPosition().x, &GetPosition().x);
        const float range1 = info->m_fTargetRange;
        const float multiplier = CWeapon::TargetWeaponRangeMultiplier(target, this); // 0x73B380
        if (!(multiplier * range1 >= distance)) { // Como el original: NaN tambien sale
            if (m_pTargetedObject) {
                m_pTargetedObject->CleanUpOldReference(&m_pTargetedObject);
            }
            m_pTargetedObject = nullptr;
            return false;
        }
        return true;
    }

    // ---- rumbo: el del vector hacia delante o, si el stick esta fuera de la zona muerta, el del stick respecto a la camara
    float heading = CGeneral::GetATanOfXY(m_matrix->GetForward().x, m_matrix->GetForward().y); // 0x53CC70 (se guarda como float)
    CEntity* best = nullptr;
    float bestPriority = -10000.0f; // 0xC61C4000
    CPad* pad = nullptr;
    if (m_nPedType == 0) {
        pad = CPad::GetPad(0); // 0x53FB70
    } else if (m_nPedType == 1) {
        pad = CPad::GetPad(1);
    }
    const float deadZone = StaticRef<float>(0x858B34); // 60.0f
    bool stick = static_cast<float>(std::abs(static_cast<int32>(pad->GetPedWalkLeftRight()))) > deadZone; // 0x53FC90
    if (!stick) {
        stick = static_cast<float>(std::abs(static_cast<int32>(pad->GetPedWalkUpDown()))) > deadZone; // 0x53FD30
    }
    if (stick) {
        // Como el original: se vuelve a pedir arriba/abajo y despues izquierda/derecha (en ese orden)
        const int32 upDown = pad->GetPedWalkUpDown();
        const int32 leftRight = pad->GetPedWalkLeftRight();
        const float stickAngle = CGeneral::GetRadianAngleBetweenPoints(0.0f, 0.0f, static_cast<float>(-leftRight), static_cast<float>(upDown)); // 0x53CBE0
        const float adjusted = (stickAngle - StaticRef<float>(0xB6F178)) + StaticRef<float>(0x858FE4); // - orientacion de la camara + pi/2
        heading = CGeneral::LimitRadianAngle(adjusted); // 0x53CB50
    }

    // ---- bote de spray: etiquetas a menos de range0 metros, la de menor angulo respecto al rumbo
    if (activeWeapon().m_Type == WEAPON_SPRAYCAN) {
        float sprayHeading;
        if (m_matrix) {
            sprayHeading = Pp79SprayHeadingCpp(&m_matrix->GetForward().x);
        } else {
            sprayHeading = m_placement.m_fHeading;
        }
        CVector centre;
        Pp79SprayCentreCpp(&m_matrix->GetForward().x, &GetPosition().x, StaticRef<float>(0x859000), &centre.x); // 8.0f
        CEntity* found[15];
        int16 count;
        CWorld::FindObjectsInRange(centre, 8.0f, false, &count, 15, found, true, false, false, false, false); // 0x564A20
        float bestAngle = 3.14159274101257324f; // 0x40490FDB
        CEntity* chosen = nullptr;
        for (int16 i = 0; i < count; ++i) {
            CEntity* const entity = found[i];
            if (!CTagManager::IsTag(*entity)) { // 0x49CCE0
                continue;
            }
            if (CTagManager::GetAlpha(*entity) >= 0xFF) { // 0x49CF90
                continue;
            }
            double angle;
            if (!Pp79SprayPickCpp(&entity->GetPosition().x, &GetPosition().x, range0, sprayHeading, &angle)) {
                continue;
            }
            // Como el original: fcom [mejor]; con NaN solo se acepta el primero
            if (angle < bestAngle || !chosen) {
                bestAngle = static_cast<float>(angle);
                chosen = entity;
            }
        }
        if (chosen && !LOSBlockedBetweenPeds(this, chosen)) { // 0x60B550
            setTarget(chosen);
            return true;
        }
    }

    // ---- peatones (de atras adelante)
    const float viewLimit = StaticRef<float>(0x8D243C) * StaticRef<float>(0x8631D4); // 140 grados en radianes
    const auto limitAngle = [](float angle) {
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
        return angle < 0.0f ? -angle : angle; // fchs solo si es menor que 0 (-0 y NaN se conservan)
    };
    CPedPool* const pedPool = StaticRef<CPedPool*>(0xB74490);
    for (size_t slot = pedPool->GetSize(); slot != 0; --slot) {
        CPed* const ped = pedPool->GetAt(slot - 1);
        if (!ped || ped == this) {
            continue;
        }
        if (ped->m_nPedState == PEDSTATE_DIE || ped->m_nPedState == PEDSTATE_DEAD) { // 0x36 y 0x37
            continue;
        }
        if (ped->bInVehicle) {
            CVehicle* const vehicle = ped->m_pVehicle;
            if (!vehicle) {
                continue;
            }
            // Como el original: moto (9) o bit 2 del byte +0x42D del vehiculo
            if (vehicle->m_nVehicleType != VEHICLE_TYPE_BIKE && !(reinterpret_cast<const uint8*>(vehicle)[0x42D] & 4)) {
                continue;
            }
        }
        if (ped->bNeverEverTargetThisPed) {
            continue;
        }
        if ((ped->m_nPedType == 0 || ped->m_nPedType == 1) && StaticRef<uint8>(0x96A8B2)) {
            continue;
        }
        if (CPedGroups::AreInSameGroup(ped, this)) { // 0x5F7F40
            continue;
        }
        const CVector& pedPos = ped->GetPosition();
        const CVector& myPos = GetPosition();
        const float dy = pedPos.y - myPos.y;
        const float dx = pedPos.x - myPos.x;
        float angle = limitAngle(PlayerPedATanMinus(dx, dy, heading));
        if (!(angle < viewLimit)) {
            continue;
        }
        // Punto 3 m por detras: posicion - normalizado(vector hacia delante) * 3
        CVector forward = m_matrix->GetForward();
        forward.Normalise(); // 0x59C910
        const CVector scaled = forward * StaticRef<float>(0x8D2440); // 0x40FEC0
        const CVector behind = GetPosition() - scaled; // 0x40FE60
        const float dy2 = ped->GetPosition().y - behind.y;
        const float dx2 = ped->GetPosition().x - behind.x;
        angle = limitAngle(PlayerPedATanMinus(dx2, dy2, heading));
        if (!(angle < StaticRef<float>(0x8D2438) * StaticRef<float>(0x8631D4))) { // 90 grados
            continue;
        }
        const CVector toPed = ped->GetPosition() - GetPosition();
        const float distance = toPed.Magnitude(); // 0x4082C0
        const float rangeNow = info->m_fTargetRange; // Como el original: se lee antes de llamar a TargetWeaponRangeMultiplier
        const float adjustedRange = CWeapon::TargetWeaponRangeMultiplier(ped, this) * rangeNow;
        if (!(distance < adjustedRange)) {
            continue;
        }
        EvaluateTarget(ped, best, bestPriority, adjustedRange, heading, false); // 0x60D020
    }

    // ---- objetos
    CObjectPool* const objectPool = StaticRef<CObjectPool*>(0xB7449C);
    for (size_t slot = objectPool->GetSize(); slot != 0; --slot) {
        CObject* const object = objectPool->GetAt(slot - 1);
        if (!object || !object->CanBeTargetted()) { // 0x59F320
            continue;
        }
        if (reinterpret_cast<const uint8*>(object)[0x140] & 0x40) {
            continue;
        }
        if (!object->GetRwObject()) {
            continue;
        }
        const CVector position = object->GetPosition();
        if (!CanIKReachThisTarget(position, &activeWeapon(), true)) { // 0x609F80
            continue;
        }
        EvaluateTarget(object, best, bestPriority, info->m_fTargetRange, heading, true);
    }

    // ---- vehiculos (solo en cooperativo)
    if (CGameLogic::IsCoopGameGoingOn()) { // 0x441390
        CVehiclePool* const vehiclePool = StaticRef<CVehiclePool*>(0xB74494);
        for (size_t slot = vehiclePool->GetSize(); slot != 0; --slot) {
            CVehicle* const vehicle = vehiclePool->GetAt(slot - 1);
            if (!vehicle || (vehicle->m_nPhysicalFlags & 0x20000000) || vehicle->m_nVehicleSubType == VEHICLE_TYPE_BMX) {
                continue;
            }
            const CVector position = vehicle->GetPosition();
            if (!CanIKReachThisTarget(position, &activeWeapon(), true)) { // 0x609F80
                continue;
            }
            EvaluateTarget(vehicle, best, bestPriority, info->m_fTargetRange, heading, true);
        }
    }

    if (!best) {
        return false;
    }
    setTarget(best);
    return true;
}

// 0x60E530
bool CPlayerPed::FindNextWeaponLockOnTarget(CEntity* arg0, bool arg1) {
    return plugin::CallMethodAndReturn<bool, 0x60E530, CPlayerPed *, CEntity*, bool>(this, arg0, arg1);
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
            float lookDir = CGeneral::LimitRadianAngle(TrigAtan2Cpp(-activeCam.m_vecFront.x, activeCam.m_vecFront.y));
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
