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
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleGangDriveBy.h"
#include "Events/EventDamage.h"
#include "X87Math.h"

void CTaskSimpleGangDriveBy::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleGangDriveBy, 0x86D944, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x6217D0);
    RH_ScopedInstall(PlayerTarget, 0x621960);
    RH_ScopedInstall(AITarget, 0x627600);
    RH_ScopedInstall(AimGun, 0x628350);
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

CTaskSimpleGangDriveBy* CTaskSimpleGangDriveBy::Constructor(CEntity* target, const CVector* targetPos, float abortRange, int8 frequencyPercentage, eDrivebyStyle drivebyStyle, bool seatRHS) {
    this->CTaskSimpleGangDriveBy::CTaskSimpleGangDriveBy(target, targetPos, abortRange, frequencyPercentage, drivebyStyle, seatRHS);
    return this;
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
    // SetupAnim (0x627B20) y FireGun (0x627CC0) siguen siendo dependencias separadas sin catalogar.
    if (ped->IsPlayer()) {
        PlayerTarget(ped);
    } else {
        AITarget(ped);
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
        AimGun(ped);
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

namespace {
// Constantes del EXE (solo se comparan; la aritmetica va en X87Math.cpp).
constexpr float kPiOver4           = std::bit_cast<float>(0x3F490FDBu); // 0x859AB0
constexpr float kPiOver2           = std::bit_cast<float>(0x3FC90FDBu); // 0x858FE4
constexpr float kPi                = std::bit_cast<float>(0x40490FDBu); // 0x858CB8
constexpr float kMinusPiOver4      = std::bit_cast<float>(0xBF490FDBu); // 0x86DAF8
constexpr float kThreePiOver4      = std::bit_cast<float>(0x4016CBE4u); // 0x86DAF4
constexpr float kMinusThreePiOver4 = std::bit_cast<float>(0xC016CBE4u); // 0x86DAF0
constexpr float kDeg15             = std::bit_cast<float>(0x3E860A92u); // 0x858F1C
constexpr float kMinusDeg15        = std::bit_cast<float>(0xBE860A92u); // 0x858F18
constexpr float kDeg165            = std::bit_cast<float>(0x40384E89u); // 0x86DB10
constexpr float kMinusDeg165       = std::bit_cast<float>(0xC0384E89u); // 0x86DB0C
constexpr float kDeg75             = std::bit_cast<float>(0x3FA78D36u); // 0x86DB08
constexpr float kMinusDeg75        = std::bit_cast<float>(0xBFA78D36u); // 0x86DB04
constexpr float kDeg105            = std::bit_cast<float>(0x3FEA9280u); // 0x86DB00
constexpr float kMinusDeg105       = std::bit_cast<float>(0xBFEA9280u); // 0x86DAFC
constexpr float kAimHeightOffset   = std::bit_cast<float>(0x3F333333u); // 0x858CB0
constexpr float kMinusOne          = -1.0f;                             // 0x858C1C

// fld dword + fstp dword: la x87 silencia un NaN senalizante (bit 22) y conserva el resto de la carga.
float QuietFloat(float value) {
    auto bits = std::bit_cast<uint32>(value);
    if ((bits & 0x7F800000u) == 0x7F800000u && (bits & 0x007FFFFFu) != 0u) {
        bits |= 0x00400000u;
    }
    return std::bit_cast<float>(bits);
}

// El EXE lee el indice de la camara activa sin comprobarlo.
CCam& ActiveCam() {
    return TheCamera.m_aCams.data()[TheCamera.m_nActiveCam];
}
}

// 0x621960
void CTaskSimpleGangDriveBy::PlayerTarget(CPed* ped) {
    auto& cam = ActiveCam();
    if (cam.m_nMode != MODE_AIMWEAPON_FROMCAR && cam.m_nMode != MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
        m_bInRangeToShoot = false;
        m_bInWeaponRange  = true;
    } else {
        m_bInRangeToShoot = true;
        m_bInWeaponRange  = true;

        CVector aim = cam.m_vecFront;
        if (cam.m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
            cam.Get_TwoPlayer_AimVector(aim);
        }

        // Como el original (0x6219E1..0x621A5F): rumbo de la camara - rumbo del vehiculo (matriz si existe, si no el placement), cuadrante 0..3.
        auto* veh = ped->m_pVehicle;
        const float* forward = veh->m_matrix ? &veh->m_matrix->GetForward().x : nullptr;
        const auto quadrant = GangDriveByPlayerQuadrantCpp(aim.x, aim.y, forward, veh->m_placement.m_fHeading);
        const bool seatRHS = m_bSeatRHS != 0;
        switch (quadrant) {
        case 0: m_nRequiredAnimID = 0xE9 + (seatRHS ? 4 : 0); break;
        case 1: m_nRequiredAnimID = 0xE7 + (seatRHS ? 5 : 0); break;
        case 2: m_nRequiredAnimID = 0xEA + (seatRHS ? 4 : 0); break;
        case 3: m_nRequiredAnimID = 0xE8 + (seatRHS ? 3 : 0); break;
        default: break; // el EXE hace ja sobre el valor sin signo: los negativos y > 3 no hacen nada
        }
    }

    if (!CGameLogic::IsCoopGameGoingOn()) {
        TheCamera.SetNewPlayerWeaponMode(MODE_AIMWEAPON_FROMCAR, 0, 0);
    }
    if (auto* data = ped->GetPlayerData()) {
        data->m_bFreeAiming = true; // or dword [datos+0x34], 8
    }
    if (ped->IsPlayer()) {
        ped->AsPlayer()->GetPlayerInfoForThisPlayerPed()->m_bCanDoDriveBy = false; // byte +0x153
    }
}

// 0x627600
void CTaskSimpleGangDriveBy::AITarget(CPed* ped) {
    const float weaponRange = m_pWeaponInfo->m_fWeaponRange;
    const float* pedPos     = &ped->GetPosition().x;

    // Objetivo: coordenadas relativas a la matriz de la entidad (se crea si falta) o las coordenadas absolutas.
    CVector transformed;
    const float* target = &m_vecCoords.x;
    if (m_pTargetEntity) {
        auto& matrix = m_pTargetEntity->GetMatrix();
        plugin::CallAndReturn<CVector*, 0x59C890, CVector*, const CMatrix*, const CVector*>(&transformed, &matrix, &m_vecCoords); // CMatrix * CVector del EXE
        target = &transformed.x;
    }

    auto* veh = ped->m_pVehicle;
    const float* forward = veh->m_matrix ? &veh->m_matrix->GetForward().x : nullptr;
    float geometry[5]; // dx, dy, dz, distancia, angulo
    GangDriveByAiGeometryCpp(target, pedPos, forward, veh->m_placement.m_fHeading, geometry);
    const float dx = geometry[0], dy = geometry[1], dz = geometry[2], distance = geometry[3], angle = geometry[4];

    m_bInRangeToShoot = false;
    m_bInWeaponRange  = distance < weaponRange;

    if (!m_bReachedAbortRange) {
        if (distance < m_fAbortRange) {
            m_bReachedAbortRange = true;
        }
    } else if (distance > m_fAbortRange) {
        m_nNextCommand = 7;
        return;
    }

    // Objetivo muerto (peatones y vehiculos): salud <= 0 (comparacion ordenada).
    if (auto* entity = m_pTargetEntity) {
        const auto type = entity->GetType();
        if (type == ENTITY_TYPE_PED) {
            if (entity->AsPed()->m_fHealth <= 0.0f) {
                m_nNextCommand = 7;
                return;
            }
        } else if (type == ENTITY_TYPE_VEHICLE) {
            if (entity->AsVehicle()->m_fHealth <= 0.0f) {
                m_nNextCommand = 7;
                return;
            }
        }
    }

    // Punto de mira en el mundo: delta + posicion del ped (ya reevaluada), un redondeo a float por coordenada.
    const float* pos = &ped->GetPosition().x;
    CVector aimPoint{ RpNanAddCpp(dx, pos[0]), RpNanAddCpp(dy, pos[1]), RpNanAddCpp(dz, pos[2]) };
    if (!plugin::CallMethodAndReturn<bool, 0x621B10>(this, ped, &aimPoint)) { // linea de vision / alcance (dependencia sin catalogar)
        m_bInRangeToShoot = false;
        return;
    }

    const int8 style = int8(m_nDrivebyStyle);
    int8 sideX    = 0;  // [esp+0x48]
    int8 sideY    = 0;  // [esp+0x13]
    int8 quadrant = -1; // dl
    if (style != 0 && style != 1 && style != 5 && style != 6) {
        quadrant = int8(GangDriveByAiQuadrantCpp(angle));
        if (angle > kDeg15 && angle < kDeg165) {
            sideX = -1;
        } else if (angle < kMinusDeg15 && angle > kMinusDeg165) {
            sideX = 1;
        }
        if (angle < kDeg75 && angle > kMinusDeg75) {
            sideY = -1;
        } else if (angle > kDeg105 && angle < kMinusDeg105) { // inalcanzable (el EXE compara con y); se conserva
            sideY = 1;
        }
    }

    // Direccion de disparo segun el estilo (jump table 0x627AE8).
    int8 direction = -1; // al
    const auto fallback = [&] { // 0x62796D
        if (quadrant == 1) {
            direction = 1;
        } else if (quadrant == 3) {
            direction = quadrant;
        }
    };
    switch (style) {
    case 0: direction = 1; break;
    case 1: direction = 3; break;
    case 2:
        if (m_nRequiredAnimID == ANIM_ID_NO_ANIMATION_SET) { direction = 1; } else { fallback(); }
        break;
    case 3:
        if (m_nRequiredAnimID == ANIM_ID_NO_ANIMATION_SET) { direction = 3; } else { fallback(); }
        break;
    case 4:
        if (m_nRequiredAnimID == ANIM_ID_NO_ANIMATION_SET) {
            if (sideX == -1) { direction = 1; } else if (sideX == 1) { direction = 3; }
        } else {
            fallback();
        }
        break;
    case 5: direction = 0; break;
    case 6: direction = 2; break;
    case 7:
        if (m_nRequiredAnimID == ANIM_ID_NO_ANIMATION_SET) {
            if (sideY == -1) { direction = 0; } else if (sideY == 1) { direction = 2; }
        } else if (quadrant == 0) {
            direction = 0;
        } else if (quadrant == 2) {
            direction = quadrant;
        }
        break;
    case 8: direction = quadrant; break;
    default: break;
    }

    // Animacion y zona de tiro (jump table 0x627B0C); los saltos del EXE comparan con fcomp + test ah (NaN: no entra).
    const bool seatRHS = m_bSeatRHS != 0;
    switch (direction) {
    case 0:
        m_nRequiredAnimID = 0xE9 + (seatRHS ? 4 : 0);
        if (distance < weaponRange && angle >= kMinusPiOver4 && angle <= kPiOver4) { m_bInRangeToShoot = true; }
        break;
    case 1:
        m_nRequiredAnimID = 0xE7 + (seatRHS ? 5 : 0);
        if (distance < weaponRange && angle >= kPiOver4 && angle <= kThreePiOver4) { m_bInRangeToShoot = true; }
        break;
    case 2:
        m_nRequiredAnimID = 0xEA + (seatRHS ? 4 : 0);
        if (distance < weaponRange && (angle >= kMinusThreePiOver4 || angle <= kThreePiOver4)) { m_bInRangeToShoot = true; }
        break;
    case 3:
        m_nRequiredAnimID = 0xE8 + (seatRHS ? 3 : 0);
        if (distance < weaponRange && angle >= kMinusThreePiOver4 && angle <= kMinusPiOver4) { m_bInRangeToShoot = true; }
        break;
    default: break;
    }
}

// 0x628350
void CTaskSimpleGangDriveBy::AimGun(CPed* ped) {
    ped->m_pedIK.m_nFlags &= ~4u; // bUseArm

    CVector target{ 0.0f, 0.0f, 0.0f };
    if (ped->IsPlayer()) {
        auto& cam = ActiveCam();
        float range = 20.0f; // 0x41A00000
        if (cam.m_nMode == MODE_TWOPLAYER_IN_CAR_AND_SHOOTING) {
            range = GangDriveByAimRangeCpp(&ped->GetPosition().x, &cam.m_vecSource.x);
        } else if (cam.m_nMode != MODE_AIMWEAPON_FROMCAR) {
            return;
        }
        const auto& pos = ped->GetPosition();
        const CVector muzzle{ pos.x, QuietFloat(pos.y), RpNanAddCpp(pos.z, kAimHeightOffset) };
        CVector source = muzzle; // salida (el EXE la deja sin inicializar en z)
        TheCamera.Find3rdPersonCamTargetVector(range, muzzle, source, target);
    } else if (m_pTargetEntity) {
        auto* entity = m_pTargetEntity;
        const bool zeroCoords = m_vecCoords.x == 0.0f && m_vecCoords.y == 0.0f && m_vecCoords.z == 0.0f;
        if (zeroCoords) {
            if (entity->GetType() == ENTITY_TYPE_PED) {
                entity->AsPed()->GetBonePosition(&target, BONE_SPINE1, false);
            } else {
                target = entity->GetPosition();
            }
        } else {
            CVector transformed;
            auto& matrix = entity->GetMatrix();
            plugin::CallAndReturn<CVector*, 0x59C890, CVector*, const CMatrix*, const CVector*>(&transformed, &matrix, &m_vecCoords);
            target = transformed;
        }
    } else if (!(m_vecCoords.x == 0.0f && m_vecCoords.y == 0.0f && m_vecCoords.z == 0.0f)) {
        target = m_vecCoords;
    }

    // Posicion del hombro derecho (hueso 22) con x e y sustituidas por las del ped.
    CVector gun{ 0.0f, 0.0f, 0.0f };
    ped->GetBonePosition(&gun, BONE_R_UPPER_ARM, false);
    const auto& pedPos = ped->GetPosition();
    gun.x = pedPos.x;
    gun.y = pedPos.y;

    float baseAngle = CGeneral::GetRadianAngleBetweenPoints(target.x, target.y, pedPos.x, pedPos.y);

    float elevation = 0.0f;
    if (auto* veh = ped->m_pVehicle; veh && int(veh->m_nVehicleType) == 9) { // moto
        const float* forward = veh->m_matrix ? &veh->m_matrix->GetForward().x : nullptr;
        const auto lean = *reinterpret_cast<const float*>(reinterpret_cast<const uint8*>(veh) + 0x648);
        const auto radianFn = reinterpret_cast<float(__cdecl*)(uint32, float, uint32, uint32)>(
            static_cast<float(*)(float, float, float, float)>(&CGeneral::GetRadianAngleBetweenPoints));
        elevation = GangDriveByBikeElevationCpp(radianFn, &target.x, &gun.x, baseAngle, forward, veh->m_placement.m_fHeading, lean);
    }

    bool flag = false;
    switch (int(m_pAnimAssoc->m_AnimId)) {
    case 0xE7:
    case 0xEC:
        flag = true;
        baseAngle = RpNanSubCpp(baseAngle, kPiOver2);
        elevation = X87TimesFloatCpp(elevation, kMinusOne);
        break;
    case 0xEB:
    case 0xE8:
        flag = true;
        baseAngle = RpNanAddCpp(baseAngle, kPiOver2);
        break;
    case 0xEA:
    case 0xEE:
        baseAngle = RpNanSubCpp(baseAngle, kPi);
        elevation = X87TimesFloatCpp(elevation, kMinusOne);
        break;
    default:
        break;
    }
    ped->m_pedIK.PointGunInDirection(baseAngle, elevation, flag, kMinusOne);
}
