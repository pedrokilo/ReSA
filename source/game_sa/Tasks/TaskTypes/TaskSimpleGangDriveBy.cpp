/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleGangDriveBy
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGangDriveBy.h"
#include "Events/EventDamage.h"

void CTaskSimpleGangDriveBy::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGangDriveBy, 0x86D944, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedVMTInstall(GetTaskType, 0x6218B0);
    // Stubs hacia el original: activarlos provocaría recursión infinita.
    RH_ScopedVMTInstall(Clone, 0x6236D0);
    RH_ScopedVMTInstall(MakeAbortable, 0x62D290);
    RH_ScopedVMTInstall(ProcessPed, 0x62D3B0);
}

CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS) {
    m_bSeatRHS             = seatRHS;
    m_nDrivebyStyle        = drivebyStyle;
    m_fAbortRange          = abortRange;
    m_pTargetEntity        = target;
    m_nFrequencyPercentage = frequencyPercentage;
    m_bIsFinished          = false;
    m_bAnimsReferenced     = false;
    m_bInRangeToShoot      = false;
    m_bInWeaponRange       = false;
    m_bReachedAbortRange   = false;
    m_bFromScriptCommand   = false;
    m_nBurstShots          = -1;
    m_nFakeShootDirn       = -1;
    m_nAttackTimer         = -1;
    m_nLastCommand         = 0;
    m_nNextCommand         = 1;
    m_nLOSCheckTime        = 0;
    m_nLOSBlocked          = true;
    m_pAnimAssoc           = nullptr;
    m_nRequiredAnimID      = ANIM_ID_NO_ANIMATION_SET;
    m_nRequiredAnimGroup   = ANIM_GROUP_DEFAULT;
    m_pWeaponInfo          = nullptr;
    m_vecCoords            = CVector{ 0.0f, 0.0f, 0.0f }; // Como el original (0x621836): +0x38..+0x40 a cero antes de copiar la posición
    CEntity::SafeRegisterRef(m_pTargetEntity);
    if (targetPos) {
        m_vecCoords = *targetPos;
    }
}

CTaskSimpleGangDriveBy::~CTaskSimpleGangDriveBy()
{
    if (m_bAnimsReferenced)
        CAnimManager::RemoveAnimBlockRef(CAnimManager::GetAnimationBlockIndex(m_nRequiredAnimGroup));

    if (m_pAnimAssoc)
        m_pAnimAssoc->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);

    CEntity::SafeCleanUpRef(m_pTargetEntity);
}

CTask* CTaskSimpleGangDriveBy::Clone() const {
    auto* clone = new CTaskSimpleGangDriveBy(m_pTargetEntity, &m_vecCoords, m_fAbortRange, m_nFrequencyPercentage, m_nDrivebyStyle, m_bSeatRHS);
    clone->m_bFromScriptCommand = m_bFromScriptCommand;
    return clone;
}

bool CTaskSimpleGangDriveBy::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (priority != ABORT_PRIORITY_URGENT && priority != ABORT_PRIORITY_IMMEDIATE) {
        m_nNextCommand = 6;
        return false;
    }
    if (event && event->GetEventType() == EVENT_DAMAGE && !static_cast<const CEventDamage*>(event)->HasKilledPed()) {
        return false;
    }
    m_nNextCommand = 7;
    // SetUpAnim is a separate, uncatalogued dependency at 0x627B20.
    if (plugin::CallMethodAndReturn<bool, 0x627B20>(this, ped)) {
        m_nLastCommand = m_nNextCommand;
        if (priority == ABORT_PRIORITY_IMMEDIATE) {
            m_pAnimAssoc->m_BlendDelta = -1000.0f;
        } else if (!m_pAnimAssoc->IsPlaying()) {
            m_pAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
        }
    }
    if (ped->GetPlayerData()) {
        ped->GetPlayerData()->m_bFreeAiming = true;
    }
    if (ped->IsPlayer()) {
        ped->AsPlayer()->GetPlayerInfoForThisPlayerPed()->m_bCanDoDriveBy = true;
    }
    if (m_pAnimAssoc) {
        if (!m_pAnimAssoc->IsPlaying() && (m_pAnimAssoc->m_BlendDelta > 0.0f ||
            (m_pAnimAssoc->m_BlendAmount > 0.0f && m_pAnimAssoc->m_BlendDelta >= 0.0f))) {
            m_pAnimAssoc->m_BlendDelta = -8.0f;
        }
        m_pAnimAssoc->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        m_pAnimAssoc = nullptr;
    }
    m_bIsFinished = true;
    return true;
}

bool CTaskSimpleGangDriveBy::ProcessPed(CPed* ped) {
    auto* info = CWeaponInfo::GetWeaponInfo(ped);
    if ((info && info->GetFireType() != WEAPON_FIRE_INSTANT_HIT) ||
        (ped->m_pVehicle && !ped->m_pVehicle->CanPedLeanOut(ped)) || m_bIsFinished) {
        return true;
    }
    if (!m_nLOSCheckTime) {
        const auto now = CTimer::GetTimeInMS();
        m_nLOSCheckTime = now + CGeneral::GetRandomIntInRange(0, 1000);
    }
    if (!m_pWeaponInfo) {
        m_pWeaponInfo = CWeaponInfo::GetWeaponInfo(ped);
    }
    if (!ped->bInVehicle || !ped->m_pVehicle || ped->m_fHealth < 1.0f) {
        MakeAbortable(ped, ABORT_PRIORITY_URGENT, nullptr);
        return true;
    }
    if (!ped->IsPlayer()) {
        // Original bit 20 of the flag word at +0x478.
        *reinterpret_cast<uint32*>(reinterpret_cast<uint8*>(ped) + 0x478) |= 0x100000;
    }
    if (m_nRequiredAnimGroup == ANIM_GROUP_DEFAULT) {
        auto* veh = ped->m_pVehicle;
        if (int(veh->m_nVehicleSubType) == 2) {
            m_nRequiredAnimGroup = AssocGroupId(75);
            m_bSeatRHS = false;
        } else if (int(veh->m_nVehicleType) == 9) {
            const auto bikeType = *reinterpret_cast<int32*>(reinterpret_cast<uint8*>(veh) + 0x640);
            m_nRequiredAnimGroup = AssocGroupId(bikeType == 2 && veh->m_pDriver == ped ? 74 : 73);
            m_bSeatRHS = false;
        } else {
            if (int(veh->m_nVehicleSubType) == 0) {
                if (veh->m_apPassengers[0] == ped) {
                    m_bSeatRHS = true;
                    veh->SetWindowOpenFlag(8);
                } else if (veh->m_apPassengers[1] == ped) {
                    m_bSeatRHS = false;
                    veh->SetWindowOpenFlag(11);
                } else if (veh->m_apPassengers[2] == ped) {
                    m_bSeatRHS = true;
                    veh->SetWindowOpenFlag(9);
                }
            }
            m_nRequiredAnimGroup = AssocGroupId(72);
        }
        if (!ped->IsPlayer() && !ped->m_pWeaponObject) {
            ped->AddWeaponModel(CWeaponInfo::GetWeaponInfo(ped->GetActiveWeapon().m_Type, eWeaponSkill::STD)->m_nModelId1);
        }
    }
    // These targeting/animation/fire/aim helpers are separate uncatalogued dependencies.
    if (ped->IsPlayer()) {
        plugin::CallMethod<0x621960>(this, ped);
    } else {
        plugin::CallMethod<0x627600>(this, ped);
    }
    if (m_nRequiredAnimGroup == AssocGroupId(74) && int(ped->m_pVehicle->m_nVehicleType) == 9) {
        *(reinterpret_cast<uint8*>(ped->m_pVehicle) + 0x7B4) = 1;
    }
    auto& loopStart = StaticRef<float>(0x8D2E7C);
    auto& loopEnd = StaticRef<float>(0x8D2E80);
    auto& fireTime = StaticRef<float>(0x8D2E84);
    const auto crossed = [](const CAnimBlendAssociation* a, float t) {
        return a->m_CurrentTime > t && a->m_CurrentTime - a->m_TimeStep < t;
    };
    if (plugin::CallMethodAndReturn<bool, 0x627B20>(this, ped)) {
        auto* a = m_pAnimAssoc;
        if (a->IsPlaying() && crossed(a, fireTime) && (m_nLastCommand == 2 || m_nLastCommand == 3)) {
            plugin::CallMethod<0x627CC0>(this, ped);
        }
        a = m_pAnimAssoc;
        const bool playing = a->IsPlaying();
        if (!playing && m_nAttackTimer <= 0 && (m_nNextCommand == 2 || m_nNextCommand == 3) &&
            (a->m_BlendHier->m_fTotalTime > a->m_CurrentTime || !(a->m_BlendDelta < 0.0f))) {
            a->m_Flags |= ANIMATION_IS_PLAYING;
            m_nBurstShots = char(CGeneral::GetRandomIntInRange(1, int(float(int16(m_pWeaponInfo->m_nAmmoClip)) * 0.5f)));
            m_nLastCommand = m_nNextCommand;
            m_nNextCommand = 0;
        } else if (!playing) {
            if (m_nNextCommand == 7) {
                a->m_Flags |= ANIMATION_IS_PLAYING;
                m_nLastCommand = m_nNextCommand;
                m_nNextCommand = 7;
            } else if (int(a->m_AnimId) != m_nRequiredAnimID) {
                a->m_Flags |= ANIMATION_IS_PLAYING;
            }
        } else if (m_nLastCommand == 1 && int(a->m_AnimId) == m_nRequiredAnimID && crossed(a, loopStart)) {
            a->m_Flags &= ~ANIMATION_IS_PLAYING;
            a->SetCurrentTime(loopStart);
        }
        a = m_pAnimAssoc;
        if (int(a->m_AnimId) == m_nRequiredAnimID && crossed(a, loopEnd)) {
            if (int(ped->GetActiveWeapon().m_State) == 2 && m_nNextCommand < 3) {
                m_nNextCommand = 2;
                m_nAttackTimer = 2000;
            } else if ((m_nNextCommand == 2 || m_nNextCommand == 3 || (m_nLastCommand == 3 && m_nBurstShots > 0)) && m_bInRangeToShoot) {
                a->SetCurrentTime(loopStart);
                m_pAnimAssoc->m_Flags |= ANIMATION_IS_PLAYING;
                if (m_nNextCommand > m_nLastCommand) m_nLastCommand = m_nNextCommand;
                m_nNextCommand = 0;
                m_nBurstShots = m_nLastCommand == 3 ? char(m_nBurstShots - 1) : 0;
            } else if (m_nNextCommand == 1) {
                a->SetCurrentTime(loopStart);
                m_pAnimAssoc->m_Flags &= ~ANIMATION_IS_PLAYING;
                m_nLastCommand = 1;
                m_nNextCommand = 0;
                m_nAttackTimer = -1;
            }
        }
    }
    if (m_nLastCommand && m_nLastCommand < 4 && m_pAnimAssoc &&
        (m_pAnimAssoc->m_BlendAmount > 0.5f || (m_pAnimAssoc->m_BlendDelta > 0.0f && !m_pAnimAssoc->IsPlaying()))) {
        plugin::CallMethod<0x628350>(this, ped);
    }
    if (m_nAttackTimer < 0) {
        m_nAttackTimer = int16(CGeneral::GetRandomIntInRange(200, (100 - m_nFrequencyPercentage) * 100));
    } else {
        m_nAttackTimer = int16(m_nAttackTimer - int(CTimer::GetTimeStep() * 0.02f * 1000.0f));
    }
    if (ped->IsPlayer() && m_nNextCommand < 4) {
        m_nAttackTimer = 0;
        m_nNextCommand = char((ped->AsPlayer()->GetPadFromPlayer()->GetWeapon(ped) != 0) + 1);
    } else if (!m_nNextCommand) {
        switch (m_nLastCommand) {
        case 1:
            if (m_nFrequencyPercentage || !m_bInWeaponRange) {
                if (m_bInRangeToShoot) {
                    m_nNextCommand = 3;
                } else if (m_nAttackTimer <= 0) {
                    m_nAttackTimer = int16(int(CTimer::GetTimeStep() * 0.02f * 1000.0f) << 1);
                }
            } else {
                m_nNextCommand = 1;
            }
            break;
        case 6:
        case 7:
            m_nNextCommand = 7;
            break;
        default:
            if (m_bInWeaponRange) m_nNextCommand = 1;
            break;
        }
    }
    if (m_nLastCommand != 4 && !m_pAnimAssoc && (m_nLastCommand == 7 || m_nNextCommand == 7)) {
        m_bIsFinished = true;
    }
    if (ped->IsPlayer()) ped->GetActiveWeapon().Update(ped);
    const auto& speed = ped->m_pVehicle->m_vecMoveSpeed;
    const auto speedSq = speed.x * speed.x + speed.y * speed.y;
    if (speedSq > 0.5f) {
        ped->Say(eGlobalSpeechContext(31));
    } else if (speedSq < 0.01f) {
        ped->Say(eGlobalSpeechContext(30));
    }
    return false;
}
