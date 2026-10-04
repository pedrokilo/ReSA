/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGangLeader
 * Reconstrucción: COMPLETA — 13/13 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 10.
 * Validación registrada: 13/13 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * BLOQUEADAS (nombre | dirección | tipo):
 *   Clone | 0x00000000 | V
 *   ControlSubTask | 0x00000000 | V
 *   CreateFirstSubTask | 0x00000000 | V
 *   CreateNextSubTask | 0x00000000 | V
 *   DoGangAbuseSpeech | 0x0065E860 | S
 *   GetRandomGangAmbientAnim | 0x0065E730 | S
 *   MakeAbortable | 0x00000000 | V
 *   ScanForStuff | 0x00000000 | V
 *   ShouldLoadGangAnims | 0x0065E7F0 | S
 *   TryToPassObject | 0x0065EA50 | S
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "Ragdoll/IKChainManager.h"
#include "ModelIndices.h"
#include "EventPassObject.h"
#include "EventGroupEvent.h"
#include "EventLeanOnVehicle.h"
#include "TaskComplexBeInGroup.h"
#include "TaskComplexPassObject.h"
#include "TaskSimpleHoldEntity.h"
#include "TaskSimpleRunAnim.h"
#include "TaskComplexPlayHandSignalAnim.h"
#include "TaskComplexGangLeader.h"
#include "TaskComplexWanderGang.h"
#include "TaskComplexTurnToFaceEntityOrCoord.h"
#include "TaskSimpleStandStill.h"

/// NOTSA - All th gang talk anims
constexpr AnimationId s_gangTalkAnims[]{
    ANIM_ID_PRTIAL_GNGTLKA,
    ANIM_ID_PRTIAL_GNGTLKB,
    ANIM_ID_PRTIAL_GNGTLKC,
    ANIM_ID_PRTIAL_GNGTLKD,

    ANIM_ID_PRTIAL_GNGTLKE,
    ANIM_ID_PRTIAL_GNGTLKF,
    ANIM_ID_PRTIAL_GNGTLKG,
    ANIM_ID_PRTIAL_GNGTLKH,
};

void CTaskComplexGangLeader::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGangLeader, 0x86F8FC, 12);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x65DED0);
    RH_ScopedInstall(Destructor, 0x65DF30);

    // Independently compared against the original; see reversing/GangLeader.
    RH_ScopedVMTInstall(GetTaskType, 0x65DF20);

    // Antes bloqueados por cuelgues (PR#449); el código ya coincide con el EXE (banco CTasks13).

    RH_ScopedInstall(GetRandomGangAmbientAnim, 0x65E730);
    RH_ScopedInstall(ShouldLoadGangAnims, 0x65E7F0);
    RH_ScopedInstall(DoGangAbuseSpeech, 0x65E860);
    RH_ScopedInstall(TryToPassObject, 0x65EA50);

    RH_ScopedVMTInstall(Clone, 0x661FA0);
    RH_ScopedVMTInstall(MakeAbortable, 0x65DFA0);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x65DFF0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x65E1F0);
    RH_ScopedVMTInstall(ControlSubTask, 0x662370);
    RH_ScopedVMTInstall(ScanForStuff, 0x65E200);
}

// Como el original: rand() * (1 / 32768) (0x858B14)
static constexpr float GANG_LEADER_RAND_RECIP = 3.0517578125e-05f;

// 0x65DED0
CTaskComplexGangLeader::CTaskComplexGangLeader(CPedGroup* gang) :
    m_gang{gang}
{
}

CTaskComplexGangLeader::CTaskComplexGangLeader(const CTaskComplexGangLeader& o) :
    CTaskComplexGangLeader{ o.m_gang }
{
}

// 0x65DF30
CTaskComplexGangLeader::~CTaskComplexGangLeader() {
    if (m_animsReferenced) {
        UnrefAnimBlock();
    }
}

// 0x65E730
AnimationId CTaskComplexGangLeader::GetRandomGangAmbientAnim(CPed* ped, CEntity* entity) {
    if (entity) {
        // Como el original: el modelo de la entidad con signo contra el indice (word) sin signo
        const auto entityModel = (int32)(int16)entity->m_nModelIndex;
        const auto isFemaleAnimModel = [ped] { return ped->m_nModelIndex == MODEL_BALLAS2 || ped->m_nModelIndex == MODEL_FAM1; };
        if (entityModel == (int32)(uint16)eModelID(ModelIndices::MI_GANG_DRINK)) {
            if (CGeneral::GetRandomNumber() < 0x3FFF) {
                return isFemaleAnimModel() ? ANIM_ID_DRNKBR_PRTL_F : ANIM_ID_DRNKBR_PRTL;
            }
            return (AnimationId)(ANIM_ID_PRTIAL_GNGTLKA + CGeneral::GetRandomIntInRange(0, 4));
        }
        if (entityModel == (int32)(uint16)eModelID(ModelIndices::MI_GANG_SMOKE)) {
            // Como el original: con el porro se usa la animacion de fumar (0x65E710)
            if (CGeneral::GetRandomNumber() < 0x3FFF) {
                return isFemaleAnimModel() ? ANIM_ID_SMKCIG_PRTL_F : ANIM_ID_SMKCIG_PRTL;
            }
            return (AnimationId)(ANIM_ID_PRTIAL_GNGTLKA + CGeneral::GetRandomIntInRange(0, 8));
        }
    }
    // Como el original: 279 - (int)(rand * (1/32768) * -8)
    return (AnimationId)(ANIM_ID_PRTIAL_GNGTLKA - (int32)((float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP * -8.f));
}

// 0x65E7F0
bool CTaskComplexGangLeader::ShouldLoadGangAnims() {
    // Como el original: primero la velocidad del vehiculo del jugador (contra 0.04, tambien con NaN se sigue) y despues el streaming
    const auto player = FindPlayerPed();
    if (player->bInVehicle && player->m_pVehicle) {
        const auto& speed = player->m_pVehicle->m_vecMoveSpeed;
        if (speed.x * speed.x + speed.y * speed.y + speed.z * speed.z > 0.04f) { // 0x863244
            return false;
        }
    }
    return !CStreaming::IsVeryBusy();
}

// 0x65E860
void CTaskComplexGangLeader::DoGangAbuseSpeech(CPed* talker, CPed* sayTo) {
    if (!talker->IsGangster()) {
        return;
    }

    if (!sayTo->IsGangster() && !sayTo->IsPlayer()) {
        return;
    }

    if (const auto phrase = [&] {
        switch (sayTo->m_nPedType) {
        case PED_TYPE_GANG1:   return CTX_GLOBAL_ABUSE_GANG_BALLAS;
        case PED_TYPE_GANG2:
        case PED_TYPE_PLAYER1:
        case PED_TYPE_PLAYER2: return CTX_GLOBAL_ABUSE_GANG_FAMILIES;
        case PED_TYPE_GANG4:   return CTX_GLOBAL_ABUSE_RIFA;
        case PED_TYPE_GANG5:   return CTX_GLOBAL_ABUSE_DA_NANG;
        case PED_TYPE_GANG6:   return CTX_GLOBAL_ABUSE_MAFIA;
        case PED_TYPE_GANG7:   return CTX_GLOBAL_ABUSE_TRIAD;
        case PED_TYPE_GANG8:   return CTX_GLOBAL_ABUSE_GANG_VLA;
        default:               return CTX_GLOBAL_NO_SPEECH;
        }
    }()) {
        talker->Say(phrase);
    }
}

// 0x65EA50
CPed* CTaskComplexGangLeader::TryToPassObject(CPed* ped, CPedGroup* group) {
    // Como el original: funcion cdecl (estatica); el miembro mas cercano del grupo a menos de 4 (estricto)
    // y solo si quien pasa el objeto no es un jugador
    float dist;
    if (const auto closest = group->GetClosestGroupPed(ped, &dist)) {
        if (dist < 4.f) { // 0x858B90
            if (!ped->IsPlayer()) {
                return closest;
            }
        }
    }
    return nullptr;
}

// 0x65DFA0
bool CTaskComplexGangLeader::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (m_pSubTask && !m_pSubTask->MakeAbortable(ped, priority, event)) {
        return false;
    }
    ped->bDontAcceptIKLookAts = false;
    return true;
}

// 0x65DFF0
CTask* CTaskComplexGangLeader::CreateNextSubTask(CPed* ped) {
    // Como el original: se cuentan los miembros antes de mirar la subtarea
    const auto numMembers = (int32)m_gang->GetMembership().CountMembers();
    if (m_pSubTask) {
        if (m_pSubTask->GetTaskType() == TASK_SIMPLE_STAND_STILL || m_pSubTask->GetTaskType() == TASK_COMPLEX_HANDSIGNAL_ANIM) {
            // Como el original: miembro (int)(numMembers * rand / 32768), que puede ser un hueco vacio
            const float rnd = (float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP;
            if (const auto mem = m_gang->GetMembership().GetMember((int32)((float)numMembers * rnd))) {
                return new CTaskComplexTurnToFaceEntityOrCoord{ mem, 0.5f, 0.2f };
            }
        }
    }

    // Como el original: se queda quieto si hay al menos 3 miembros y (int)(rand * 100 / 32768) <= 95
    const auto chance = (int32)((float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP * 100.f);
    if (numMembers >= 3 && chance <= 95) {
        return new CTaskSimpleStandStill{ 5000 };
    }

    // Como el original: intervalo 15000 - (int)(rand * -15000 / 32768), es decir [15000, 30000)
    const auto rndInterval = (int32)((float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP * -15000.f); // 0x86F934
    m_wanderTimer.m_nStartTime = CTimer::GetTimeInMS();
    m_wanderTimer.m_nInterval  = 15'000 - rndInterval;
    m_wanderTimer.m_bStarted   = true;
    return new CTaskComplexWanderGang{ PEDMOVE_WALK, (uint8)(int32)((float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP * 8.f), 5000, true, 0.05f };
}

// 0x65E1F0
CTask* CTaskComplexGangLeader::CreateFirstSubTask(CPed* ped) {
    return CreateNextSubTask(ped);
}

// 0x662370
CTask* CTaskComplexGangLeader::ControlSubTask(CPed* ped) {
    // Como el original: se vuelven a aceptar las miradas IK en cada control
    ped->bDontAcceptIKLookAts = false;

    // Make sure anmims are loaded (if they can/need to be)
    if (m_animsReferenced) { // 0x66239B
        if (!ShouldLoadGangAnims()) {
            UnrefAnimBlock();
        }
    } else if (ShouldLoadGangAnims()) {
        const auto blk = CAnimManager::GetAnimationBlockIndex("gangs");
        if (CAnimManager::GetAnimBlocks()[blk].IsLoaded) {
            CAnimManager::AddAnimBlockRef(blk);
            m_animsReferenced = true;
        } else {
            CStreaming::RequestModel(IFPToModelId(blk), STREAMING_KEEP_IN_MEMORY);
        }
    }

    // If we're wandering and the wander time is out of time...
    // Como el original: el tipo de la subtarea se pide dos veces (la primera se descarta) y sin comprobar el puntero
    (void)m_pSubTask->GetTaskType();
    if (m_pSubTask->GetTaskType() == TASK_COMPLEX_WANDER) { // 0x66241F
        if (m_wanderTimer.IsOutOfTime()) {
            if (static_cast<CTaskComplexWander*>(m_pSubTask)->GetDistSqOfClosestPathNodeToPed(ped) < 2.f) { // Como el original: estricto
                m_gang->GetIntelligence().SetDefaultTaskAllocatorType(ePedGroupDefaultTaskAllocatorType::RANDOM);
                // Above call causes this task to be flushed (deleted), and changes our vfptr to `CTaskComplex`'s.
                // If we return non-null here, `CTaskManager::ParentsControlChildren` will be called, and calls our
                // `ControlSubTask` causing an assert (as CTaskComplex defines it as `pure` (`= 0`))
                // This is an OG bug, and we can't even wrap it into `FIX_BUGS` because it literally aborts the process,
                // makes no sense keeping it.
                return nullptr; //return new CTaskSimpleStandStill{ 500 };
            }
        }
    }

    if (m_exhaleTimer.IsOutOfTime()) { // 0x6624C1
        if (ped->GetRpClump()) {
            if (auto matrix = RwFrameGetMatrix(RpClumpGetFrame(ped->GetRpClump()))) {
                if (const auto fx = g_fxMan.CreateFxSystem("exhale", CVector{ 0.f, 0.1f, 0.f }, matrix)) {
                    fx->AttachToBone(ped, eBoneTag::BONE_HEAD);
                    fx->PlayAndKill();
                }
            }
            m_exhaleTimer.Stop();
        }
    }

    ScanForStuff(ped);

    if (!ped->IsVisible()) {
        return m_pSubTask;
    }

    // If ped isn't already looking at someone, find a random member to look at them
    if (!g_ikChainMan.IsLooking(ped)) { // 0x662574
        // Como el original: (int)(rand * 100 / 32768) > 95; el tiempo se sortea antes que el miembro (que puede ser un hueco vacio)
        if ((int32)((float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP * 100.f) > 95) {
            const auto lookTime = (float)CGeneral::GetRandomIntInRange(3000, 5000);
            const auto mem      = m_gang->GetMembership().GetMember(CGeneral::GetRandomIntInRange(0, 8));
            if (mem && mem != ped) {
                g_ikChainMan.LookAt(
                    "TaskGangLeader",
                    ped,
                    mem,
                    (int32)lookTime,
                    BONE_HEAD,
                    nullptr,
                    true,
                    0.15f,
                    500,
                    3,
                    false
                );
            }
        }
    }

    if (!m_animsReferenced || (int32)ped->m_nMoveState >= PEDMOVE_RUN) { // 0x66261A
        return m_pSubTask;
    }

    const auto pedHeldEntity = ped->GetEntityThatThisPedIsHolding();

    if (!pedHeldEntity) {
        // If they're already playing an anim, early out
        if (ped->IsPlayingHandSignal() || ped->GetTaskManager().GetTaskSecondary(TASK_SECONDARY_PARTIAL_ANIM)) {
            return m_pSubTask;
        }

        // Como el original: 51..55 -> animacion de charla, 100 -> senal con la mano, otro valor -> nada
        const auto rnd = CGeneral::GetRandomIntInRange(0, 500);
        if (rnd > 50 && rnd < 56) {
            const auto task = new CTaskSimpleRunAnim{ ANIM_GROUP_GANGS, (AnimationId)(ANIM_ID_PRTIAL_GNGTLKA + (int32)((float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP * 8.f)), 4.f, false };
            ped->GetTaskManager().SetTaskSecondary(task, TASK_SECONDARY_PARTIAL_ANIM);
        } else if (rnd == 100) {
            ped->GetTaskManager().SetTaskSecondary(new CTaskComplexPlayHandSignalAnim{ ANIM_ID_UNDEFINED, 4.f }, TASK_SECONDARY_PARTIAL_ANIM);
        }
        return m_pSubTask;
    }

    // Enum to be in the same order as anims below
    enum {
        DRNKBR_PRTL,
        SMKCIG_PRTL,
        DRNKBR_PRTL_F,
        SMKCIG_PRTL_F,
    };
    const auto GetAnim = [ped](AnimationId id) { return RpAnimBlendClumpGetAssociation(ped->GetRpClump(), id); };
    const CAnimBlendAssociation* anims[]{
        GetAnim(ANIM_ID_DRNKBR_PRTL),
        GetAnim(ANIM_ID_SMKCIG_PRTL),
        GetAnim(ANIM_ID_DRNKBR_PRTL_F),
        GetAnim(ANIM_ID_SMKCIG_PRTL_F),
    };
    const bool anyOfTheAnimsPlaying = rng::any_of(anims, notsa::NotIsNull{});

    // If any of the anims are playing, stop looking, start exhale timer of smkcig anims
    if (anyOfTheAnimsPlaying) { // 0x662696
        if (g_ikChainMan.IsLooking(ped)) {
            g_ikChainMan.AbortLookAt(ped, 250);
        }

        ped->bDontAcceptIKLookAts = true;

        // Start exhale timer (for smkcig anims)
        const auto smoking = (anims[SMKCIG_PRTL] && anims[SMKCIG_PRTL]->m_CurrentTime < 0.5f)
                          || (anims[SMKCIG_PRTL_F] && anims[SMKCIG_PRTL_F]->m_CurrentTime < 0.5f);
        if (smoking && !m_exhaleTimer.m_bStarted) {
            m_exhaleTimer.m_nStartTime = CTimer::GetTimeInMS();
            m_exhaleTimer.m_nInterval  = 2700;
            m_exhaleTimer.m_bStarted   = true;
        }
    }

    // Como el original: modelo con signo contra el indice (word) sin signo
    if ((int32)(int16)pedHeldEntity->m_nModelIndex == (int32)(uint16)eModelID(ModelIndices::MI_GANG_DRINK)) { // 0x662729
        ped->Say(CTX_GLOBAL_BOOZE_RECEIVE, 0, 0.2f);
    } else if ((int32)(int16)pedHeldEntity->m_nModelIndex == (int32)(uint16)eModelID(ModelIndices::MI_GANG_SMOKE)) {
        ped->Say(CTX_GLOBAL_SPLIFF_RECEIVE, 0, 0.2f);
    }

    // Now, pass on the entity held in hand (if not already)

    if (ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_PASS_OBJECT)) { // 0x662766
        return m_pSubTask;
    }

    if (CGeneral::GetRandomIntInRange(0, 500) != 200) { // 99.8%
        // Como el original: una vez de cada 100 ((int)(rand * 100 / 32768) == 50)
        if (CGeneral::GetRandomIntInRange(0, 100) == 50) {
            if (const auto task = static_cast<CTaskSimpleHoldEntity*>(ped->GetTaskManager().FindActiveTaskByType(TASK_SIMPLE_HOLD_ENTITY))) {
                task->PlayAnim(GetRandomGangAmbientAnim(ped, pedHeldEntity), ANIM_GROUP_GANGS);
            }
        }
        return m_pSubTask;
    }

    if (anyOfTheAnimsPlaying) { // 0x66279A
        return m_pSubTask;
    }

    if (const auto passObjTo = TryToPassObject(ped, m_gang)) { // 0x6627CD
        if (!passObjTo->GetEntityThatThisPedIsHolding() && passObjTo->IsCurrentlyUnarmed()) {
            // Very similar to code above, but not quite the same!
            if ((int32)(int16)pedHeldEntity->m_nModelIndex == (int32)(uint16)eModelID(ModelIndices::MI_GANG_DRINK)) {
                passObjTo->Say(CTX_GLOBAL_BOOZE_REQUEST);
            } else if ((int32)(int16)pedHeldEntity->m_nModelIndex == (int32)(uint16)eModelID(ModelIndices::MI_GANG_SMOKE)) {
                passObjTo->Say(CTX_GLOBAL_SPLIFF_REQUEST);
            }
            // Como el original: el evento vive hasta despues de crear la tarea
            CEventPassObject passEvent{ ped, false };
            passObjTo->GetEventGroup().Add(&passEvent, false);
            return new CTaskComplexPassObject{ passObjTo, true };
        }
    }

    return m_pSubTask;
}

// 0x65E200
void CTaskComplexGangLeader::ScanForStuff(CPed* ped) {
    // Como el original: con el temporizador sin arrancar se explora siempre
    if (m_scanTimer.m_bStarted && !m_scanTimer.IsOutOfTime()) {
        return;
    }

    const auto rndChance = (int32)((float)CGeneral::GetRandomNumber() * GANG_LEADER_RAND_RECIP * 100.f);
    if (rndChance < 5) { // 5% chance
        // Find a nearby vehicle to lean onto
        const auto intel = ped->GetIntelligence();
        for (auto i = 0; i < 16; i++) {
            const auto veh = static_cast<CVehicle*>(intel->GetVehicleEntities()[i]);
            // 0x65E2C1
            // Como el original: se mira el subtipo del vehiculo (+0x594), no el tipo
            if (!veh || !veh->IsSubAutomobile() || veh->GetStatus() != STATUS_ABANDONED || veh->vehicleFlags.bHasGangLeaningOn || veh->IsMissionVehicle()) {
                continue;
            }

            // 0x65E304 y 0x65E351 (en ese orden; con NaN tampoco se acepta)
            const auto& vehPos = veh->GetPosition();
            const auto& pedPos = ped->GetPosition();
            const float dx = vehPos.x - pedPos.x, dy = vehPos.y - pedPos.y, dz = vehPos.z - pedPos.z;
            if (!(dx * dx + dy * dy + dz * dz < 300.f)) {
                continue;
            }
            if (!(std::abs(vehPos.z - pedPos.z) < 5.f)) {
                continue;
            }

            // Como el original: el tiempo se sortea antes de reservar el evento; el evento de grupo se destruye
            // despues de arrancar el temporizador
            const auto leanTime = CGeneral::GetRandomIntInRange(10'000, 25'000);
            CEventGroupEvent groupEvent{ ped, new CEventLeanOnVehicle{ veh, leanTime } };
            m_gang->GetIntelligence().AddEvent(&groupEvent);
            m_scanTimer.Start(60'000);

            // 0x65E3E8
#ifdef FIX_BUGS
            break;
#endif
        }
    } else if (rndChance == 20) { // 1% chance
        // Try recruiting one nearby ped to the gang
        const auto intel = ped->GetIntelligence();
        for (auto i = 0; i < 16; i++) {
            const auto scannedPed = static_cast<CPed*>(intel->GetPedEntities()[i]);
            if (!scannedPed || !scannedPed->IsCreatedBy(PED_GAME) || ped->m_nPedType != scannedPed->m_nPedType) {
                continue;
            }

            // Como el original: el grupo se consulta antes de mirar si va en vehiculo
            const auto scannedPedGrp = CPedGroups::GetPedsGroup(scannedPed);

            // Already in the gang
            if (scannedPedGrp == m_gang || scannedPed->bInVehicle) {
                continue;
            }

            // In the player's gang
            if (FindPlayerPed()->GetPlayerGroup().GetMembership().IsMember(scannedPed)) { // 0x65E494
                continue;
            }

            // Can it join a gang at all?
#ifdef FIX_BUGS
            if (const auto wander = static_cast<CTaskComplexWander*>(scannedPed->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_WANDER))) { // 0x65E4BE
#else
            if (const auto wander = static_cast<CTaskComplexWander*>(ped->GetTaskManager().FindActiveTaskByType(TASK_COMPLEX_WANDER))) {
#endif
                if (wander->GetWanderType() == WANDER_TYPE_GANG) {
                    if (!static_cast<CTaskComplexWanderGang*>(wander)->CanJoinGang()) {
                        continue;
                    }
                }
            }

            // If scanned ped has no group try to add them to this gang
            if (!scannedPedGrp) { // 0x65E4EA
                // Como el original: el id del grupo se pide antes de contar los miembros sin el lider (< 7);
                // el comando se anade al grupo de eventos del peaton explorado y despues se procesa la banda
                const auto gangId = CPedGroups::GetGroupId(m_gang);
                if (m_gang->GetMembership().CountMembersExcludingLeader() < 7) {
                    CEventScriptCommand scriptCommand{ TASK_PRIMARY_PRIMARY, new CTaskComplexBeInGroup{ gangId, false }, false };
                    scannedPed->GetEventGroup().Add(&scriptCommand, false);
                    m_gang->GetMembership().AddFollower(scannedPed);
                    m_gang->Process();
                }
            }

            // Find a member close enough to the scanned ped, and make them partners
            // Como el original: distancia al cuadrado (0x406DA0) aceptada en [16, 100]
            CPed* closestMem  = nullptr;
            float closestDist = std::bit_cast<float>(0x4E6E6B28u); // 1e9
            for (auto m = 0; m < 8; m++) {
                const auto mem = m_gang->GetMembership().GetMember(m);
                if (!mem || mem == scannedPed) {
                    continue;
                }
                const auto dist = (scannedPed->GetPosition() - mem->GetPosition()).SquaredMagnitude();
                if (dist < closestDist) {
                    closestDist = dist;
                    closestMem  = mem;
                }
            }
            if (closestDist <= 100.f && closestDist >= 16.f) { // 0x65E61F
                const auto partnerType = CGeneral::GetRandomIntInRange(0, 7);
                CEventCreatePartnerTask scannedEvent{ partnerType, closestMem, true, 0.5f };
                scannedPed->GetEventGroup().Add(&scannedEvent, false);
                CEventCreatePartnerTask memberEvent{ partnerType, scannedPed, false, 0.5f };
                closestMem->GetEventGroup().Add(&memberEvent, false);
            }

            m_scanTimer.Start(10'000);

            break;
        }
    }
}

void CTaskComplexGangLeader::UnrefAnimBlock() {
    CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex("gangs"));
    m_animsReferenced = false;
}
