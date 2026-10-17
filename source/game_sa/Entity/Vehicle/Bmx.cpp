/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBmx
 * Reconstrucción: COMPLETA — 10/10 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 10/10 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CarCtrl.h"
#include "Shadows.h"
#include "X87Math.h"

namespace {
// Como el original: global sin nombre de 0xC1C83C (solo se lee en PreRender; el EXE nunca la escribe, vale 0)
inline auto& BmxPreRenderFlag = StaticRef<bool>(0xC1C83C);
// Como el original: BMX_PEDAL_LEANSTART (0xC1C820) es una global de .bss que ProcessControl lee cada vez
inline auto& BmxPedalLeanStart = StaticRef<float>(0xC1C820);

// Como el original: constantes del .rdata (no son las de common.h: TWO_PI = 6.2831855, FRAC_PI_2 = 1.5707964)
constexpr float kBmxTwoPi        = std::bit_cast<float>(0x40C90FDBu); // 0x858CBC
constexpr float kBmxHalfPi       = std::bit_cast<float>(0x3FC90FDBu); // 0x8D328C / 0x87150C
constexpr float kBmxPi           = std::bit_cast<float>(0x40490FDBu); // 0x871504
constexpr float kBmxDegToRad     = std::bit_cast<float>(0x3C8EFA35u); // 0x8595EC
constexpr float kBmxInvTwoPi     = std::bit_cast<float>(0x3E22F983u); // 0x8594F0
constexpr float kBmxInv255       = std::bit_cast<float>(0x3B808081u); // 0x859A3C
constexpr float kBmxK863984      = std::bit_cast<float>(0x3C03126Fu); // 0x863984 (0.008)

// Como el original: plantillas Max (0x420800) y Min (0x404330) del EXE; con NaN devuelven el segundo
float BmxMax(float a, float b) { return a > b ? a : b; }

// Como el original: el x87 (fsin, fcos, fpatan, _CIpow y _CIasin del EXE) vive en los ayudantes Trig*, Bmx*, PowScaleFloatCpp de X87Math.cpp.

// Como el original: Multiply3x3 (0x59C790), matriz por vector, con el orden de sumas del EXE
CVector BmxMultiply3x3_MV(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// Como el original: una animacion se crea de nuevo si no existe o si todavia no esta entrando (mezcla < 1 y delta <= 0)
bool BmxNeedsNewAnim(const CAnimBlendAssociation* a) {
    return !a || (a->GetBlendAmount() < 1.0f && a->GetBlendDelta() <= 0.0f);
}
} // namespace

void CBmx::InjectHooks() {
    RH_ScopedVirtualClass(CBmx, 0x871528, 67);
    RH_ScopedCategory("Vehicle");

    RH_ScopedInstall(Constructor, 0x6BF820);
    RH_ScopedVMTInstall(SetUpWheelColModel, 0x6BF9B0);
    RH_ScopedVMTInstall(BurstTyre, 0x6BF9C0);
    RH_ScopedVMTInstall(FindWheelWidth, 0x6C0550);
    RH_ScopedVMTInstall(ProcessControl, 0x6BFA30);
    RH_ScopedVMTInstall(ProcessDrivingAnims, 0x6BFB50);
    RH_ScopedVMTInstall(PreRender, 0x6C0810);
    RH_ScopedVMTInstall(ProcessAI, 0x6C1470);
    RH_ScopedInstall(ProcessBunnyHop, 0x6C0590);
    RH_ScopedInstall(LaunchBunnyHopCB, 0x6C0390);
}

// 0x6BF820
CBmx::CBmx(int32 modelIndex, eVehicleCreatedBy createdBy) :
    CBike(modelIndex, createdBy)
{
    auto mi                     = CModelInfo::GetModelInfo(modelIndex);
    m_nVehicleSubType           = VEHICLE_TYPE_BMX;
    m_RideAnimData.AnimGroup = CAnimManager::GetAnimBlocks()[mi->GetAnimFileIndex()].GroupId;
    if (m_RideAnimData.AnimGroup < ANIM_GROUP_BMX || m_RideAnimData.AnimGroup > ANIM_GROUP_CHOPPA) {
        m_RideAnimData.AnimGroup = ANIM_GROUP_BMX;
    }

    m_fControlJump     = 0.0f;
    m_fControlPedaling = 0.0f;
    m_fSprintLeanAngle = 0.0f;
    m_fCrankAngle      = 0.0f;
    m_fPedalAngleL     = 0.0f;
    m_fPedalAngleR     = 0.0f;
    m_nFixLeftHand     = false;
    m_nFixRightHand    = false;
    m_bIsFreewheeling  = false;

    const auto Calc = [&](eBmxNodes node) -> float {
        RwMatrix matrix;
        RwFrame* wheelFront = m_aBikeNodes[node];
        matrix              = *RwFrameGetMatrix(wheelFront);

        auto parent = RwFrameGetParent(wheelFront);
        if (parent) {
            do {
                RwMatrixTransform(&matrix, RwFrameGetMatrix(parent), rwCOMBINEPOSTCONCAT);
                parent = RwFrameGetParent(parent);
            } while (parent != wheelFront && parent);
        }
        return matrix.pos.y;
    };
    auto wheelFrontPosY = Calc(BMX_WHEEL_FRONT);
    auto wheelRearPosY  = Calc(BMX_WHEEL_REAR);

    m_fMidWheelDistY = wheelFrontPosY - wheelRearPosY;
    m_fMidWheelFracY = wheelFrontPosY / m_fMidWheelDistY;
}

// 0x6BF9D0
CBmx::~CBmx() {
    m_vehicleAudio.Terminate();
}

// 0x6BF9B0
bool CBmx::SetUpWheelColModel(CColModel* wheelCol) {
    return false;
}

// 0x6BF9C0
bool CBmx::BurstTyre(uint8 tyreComponentId, bool bPhysicalEffect) {
    return false;
}

// 0x6BFA30
void CBmx::ProcessControl() {
    constexpr float BMX_SPRINT_LEANMULT = std::bit_cast<float>(0x3E99999Au); // 0x8D3290 (0.3)
    constexpr float MTB_SPRINT_LEANMULT = std::bit_cast<float>(0x3DB22D0Eu); // 0x8D3298 (0.087)
    constexpr float BMX_PEDAL_LEANMULT  = std::bit_cast<float>(0x3D8F5C29u); // 0x8D3294 (0.07)
    constexpr float MTB_PEDAL_LEANMULT  = std::bit_cast<float>(0x3CA3D70Au); // 0x8D329C (0.02)

    CBike::ProcessControl();

    if (GetWasPostponed() || GetStatus() != STATUS_PLAYER || !m_pDriver) {
        return;
    }

    auto animBikeSprint = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_SPRINT);

    if (animBikeSprint && animBikeSprint->GetBlendAmount() > 0.01f) {
        const float mult   = GetModelId() == MODEL_MTBIKE ? MTB_SPRINT_LEANMULT : BMX_SPRINT_LEANMULT;
        m_fSprintLeanAngle = BmxLeanTermCpp(
            animBikeSprint->GetCurrentTime(), animBikeSprint->GetHier()->GetTotalTime(), kBmxHalfPi, animBikeSprint->GetBlendAmount(), mult);
    } else {
        auto animBikePedal = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_PEDAL);
        const float mult   = GetModelId() == MODEL_MTBIKE ? MTB_PEDAL_LEANMULT : BMX_PEDAL_LEANMULT;
        if (animBikePedal && animBikePedal->GetBlendAmount() > 0.01f) {
            // Como el original: el termino se suma a LeanAngle sin guardarse antes como float (0x6BFB29)
            m_RideAnimData.LeanAngle = BmxLeanTermAddCpp(
                animBikePedal->GetCurrentTime(), animBikePedal->GetHier()->GetTotalTime(), BmxPedalLeanStart, animBikePedal->GetBlendAmount(), mult,
                m_RideAnimData.LeanAngle);
        }
        m_fSprintLeanAngle = 0.95f * m_fSprintLeanAngle;
    }
}

// 0x6BFB50
void CBmx::ProcessDrivingAnims(CPed* driver, bool blend) {
    if (m_bOffscreen) {
        if (!driver || !driver->IsPlayer()) {
            return;
        }
    }

    m_nFixLeftHand  = 1;
    m_nFixRightHand = 1;

    // Como el original: con la animacion de salto ("bunny hop") solo se mezcla el cigueñal y no se hace nada mas
    if (auto* hop = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_BUNNYHOP)) {
        m_fCrankAngle  = (1.0f - hop->GetBlendAmount()) * m_fCrankAngle + kBmxHalfPi * hop->GetBlendAmount();
        m_fPedalAngleL = -m_fCrankAngle;
        m_fPedalAngleR = -m_fCrankAngle;
        return;
    }

    if (driver->GetPlayerData()) {
        driver->SetMoveState(PEDMOVE_NONE);
    }

    const float speed = DotProduct(m_vecMoveSpeed, m_matrix->GetForward());

    auto* pedalA  = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_PEDAL);
    auto* sprintA = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_SPRINT);
    auto* leftA   = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_LEFT);
    auto* rightA  = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_RIGHT);
    auto* fwdA    = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_FWD);
    auto* driveA  = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_DRIVEBYLHS);
    if (!driveA) {
        driveA = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_DRIVEBYRHS);
        if (!driveA) {
            driveA = RpAnimBlendClumpGetAssociation(driver->GetRpClump(), ANIM_ID_BIKE_DRIVEBYFT);
        }
    }

    // Como el original: limite de inclinacion segun el pedaleo
    const float leanLimit = m_fControlPedaling > 5.0f ? 0.7f : 0.4f;

    // Como el original: el pedaleo normal exige poca inclinacion (lateral y hacia delante), algo de velocidad y sin tiro
    if (std::fabs(m_RideAnimData.LeanAngle) < leanLimit && leanLimit > m_RideAnimData.LeanFwd && speed > 0.01f && !driveA) {
        float pedalRate;
        float threshold;
        if (GetModelId() == MODEL_MTBIKE) {
            const uint8 gear = m_nCurrentGear;
            threshold        = 2.0f;
            if (gear < 1) {
                pedalRate = 0.0f;
            } else {
                pedalRate = (5.0f * speed) / (float(gear) * m_pHandlingData->GetTransmission().m_MaxFlatVelocity - 0.25f);
            }
        } else {
            pedalRate = 3.0f * speed;
            threshold = 2.5f;
        }

        bool created = false;
        CAnimBlendAssociation* active;
        if (m_fControlPedaling > 5.0f && pedalRate < threshold) {
            if (BmxNeedsNewAnim(sprintA)) {
                sprintA = CAnimManager::BlendAnimation(driver->GetRpClump(), m_RideAnimData.AnimGroup, ANIM_ID_BIKE_SPRINT, 4.0f);
                created = true;
            }
            sprintA->SetFlag(ANIMATION_IS_PLAYING, true);
            if (pedalA) {
                pedalA->SetFlag(ANIMATION_IS_PLAYING, true);
                pedalA->SetSpeed(pedalRate);
                active = pedalA;
            } else {
                sprintA->SetSpeed(pedalRate);
                active = sprintA;
            }
        } else if (m_GasPedal == 0.0f && !(m_fControlPedaling > 0.0f) && GetStatus() != STATUS_SIMPLE) {
            // Como el original: rueda libre
            if (BmxNeedsNewAnim(pedalA)) {
                pedalA  = CAnimManager::BlendAnimation(driver->GetRpClump(), m_RideAnimData.AnimGroup, ANIM_ID_BIKE_PEDAL, 4.0f);
                created = true;
            }
            pedalA->SetFlag(ANIMATION_IS_PLAYING, false);
            if (!vehicleFlags.bIsHandbrakeOn) {
                if (m_aRatioHistory[0] < 1.0f || m_aRatioHistory[1] < 1.0f || m_aRatioHistory[2] < 1.0f || m_aRatioHistory[3] < 1.0f) {
                    m_bIsFreewheeling = true;
                }
            }
            active = pedalA;
        } else {
            if (BmxNeedsNewAnim(pedalA)) {
                pedalA  = CAnimManager::BlendAnimation(driver->GetRpClump(), m_RideAnimData.AnimGroup, ANIM_ID_BIKE_PEDAL, 4.0f);
                created = true;
            }
            pedalA->SetFlag(ANIMATION_IS_PLAYING, true);
            pedalA->SetSpeed(pedalRate);
            active = pedalA;
        }

        // Como el original: angulo del cigueñal
        const auto CrankFromTime = [&] {
            m_fCrankAngle = 0.0f - active->GetCurrentTime() / active->GetHier()->GetTotalTime() * kBmxTwoPi;
        };
        if (!active) {
            PowScaleFloatCpp(&m_fCrankAngle, 0.97f, CTimer::GetTimeStep());
        } else if (!created) {
            CrankFromTime();
        } else {
            float turnConstant = 0.0f;
            bool  haveTurn     = false;
            if (leftA && leftA->GetBlendAmount() > 0.5f) {
                turnConstant = kBmxPi;
                haveTurn     = true;
            } else if (rightA && rightA->GetBlendAmount() > 0.5f) {
                turnConstant = 0.0f;
                haveTurn     = true;
            } else if (fwdA && fwdA->GetBlendAmount() > 0.5f) {
                turnConstant = kBmxHalfPi;
                haveTurn     = true;
            }
            if (haveTurn && turnConstant > -1000.0f) {
                float rate = (0.0f - turnConstant) * kBmxInvTwoPi;
                if (rate < 0.0f) {
                    rate = rate + 1.0f;
                }
                active->SetCurrentTime(active->GetHier()->GetTotalTime() * rate);
                m_fCrankAngle = rate;
            } else {
                CrankFromTime();
            }
        }

        if (std::fabs(m_RideAnimData.AnimLeanLeft) > 0.05f || std::fabs(m_RideAnimData.AnimLeanFwd) > 0.05f) {
            m_RideAnimData.AnimLeanLeft = 0.95f * m_RideAnimData.AnimLeanLeft;
            m_RideAnimData.AnimLeanFwd  = 0.95f * m_RideAnimData.AnimLeanFwd;
        }
    } else {
        // Como el original: se desvanecen las animaciones de pedaleo/sprint que aun entran, o se usan las animaciones de la moto
        const auto Fading = [](const CAnimBlendAssociation* a) {
            return a && a->GetBlendDelta() >= 0.0f && a->GetBlendAmount() > 0.0f;
        };
        if (Fading(pedalA) || Fading(sprintA)) {
            if (pedalA) {
                pedalA->SetFlag(ANIMATION_IS_PLAYING, false);
                pedalA->SetBlendDelta(-8.0f);
            }
            if (sprintA) {
                sprintA->SetFlag(ANIMATION_IS_PLAYING, false);
                sprintA->SetBlendDelta(-8.0f);
            }
            m_RideAnimData.AnimLeanLeft = 0.95f * m_RideAnimData.AnimLeanLeft;
            m_RideAnimData.AnimLeanFwd  = 0.95f * m_RideAnimData.AnimLeanFwd;
        } else {
            CBike::ProcessRiderAnims(driver, this, &m_RideAnimData, m_BikeHandling, 0);
        }

        auto* ref = pedalA ? pedalA : sprintA;
        if (!ref) {
            m_fCrankAngle = 0.0f;
        } else {
            m_fCrankAngle = 0.0f - ref->GetCurrentTime() / ref->GetHier()->GetTotalTime() * kBmxTwoPi;
        }

        if (leftA && leftA->GetBlendAmount() > 0.1f) {
            m_fCrankAngle     = (1.0f - leftA->GetBlendAmount()) * m_fCrankAngle + kBmxPi * leftA->GetBlendAmount();
            m_bIsFreewheeling = true;
        } else if (rightA && rightA->GetBlendAmount() > 0.1f) {
            m_fCrankAngle     = (1.0f - rightA->GetBlendAmount()) * m_fCrankAngle + 0.0f * rightA->GetBlendAmount();
            m_bIsFreewheeling = true;
        } else if (fwdA && fwdA->GetBlendAmount() > 0.1f) {
            m_fCrankAngle = (1.0f - fwdA->GetBlendAmount()) * m_fCrankAngle + kBmxHalfPi * fwdA->GetBlendAmount();
        } else {
            PowScaleFloatCpp(&m_fCrankAngle, 0.97f, CTimer::GetTimeStep());
        }

        if (driveA) {
            m_nFixRightHand   = 0;
            m_bIsFreewheeling = true;
        }
    }

    // Como el original: solo el jugador marca la rueda delantera/trasera levantada para la camara
    if (driver->IsPlayer()) {
        bikeFlags.bWheelieForCamera = false;
        const float fwdZ            = m_matrix->GetForward().z;
        // jp tras "fcomp 0; test ah,41h": positivo o NaN
        const auto PositiveOrNaN = [](float v) { return !(v <= 0.0f); };

        bool wheelie = false;
        bool toB     = false;
        if (PositiveOrNaN(m_WheelCounts[0]) || PositiveOrNaN(m_WheelCounts[1]) || !(fwdZ > 0.0f)) {
            toB = true;
        } else if (!(m_WheelCounts[2] > 0.0f) && !(m_WheelCounts[3] > 0.0)) {
            toB = true;
        }
        if (!toB) {
            // 0x6C02C1: rueda delantera levantada
            const float ang = m_BikeHandling->m_fWheelieAng;
            const float a   = ang - fwdZ;
            const float b   = ang * 0.5f;
            wheelie         = b > a;
        } else if (!PositiveOrNaN(m_WheelCounts[2]) && !PositiveOrNaN(m_WheelCounts[3]) && fwdZ < 0.0f
                   && (m_WheelCounts[0] > 0.0f || m_WheelCounts[1] > 0.0f)) {
            // 0x6C0344: rueda trasera levantada
            const float ang = m_BikeHandling->m_fStoppieAng;
            const float a   = ang - fwdZ;
            const float b   = ang * 0.6f;
            wheelie         = b < a;
        }
        if (wheelie) {
            bikeFlags.bWheelieForCamera = true;
        }
    }

    m_fPedalAngleL = -m_fCrankAngle;
    m_fPedalAngleR = -m_fCrankAngle;
}

// data is a ptr to CBmx
// 0x6C0390
void CBmx::LaunchBunnyHopCB(CAnimBlendAssociation* assoc, void* data) {
    auto bmx = static_cast<CBmx*>(data);
    if ((bmx->m_WheelCounts[0] > 0.0f || bmx->m_WheelCounts[1] > 0.0f) &&
        (bmx->m_WheelCounts[2] > 0.0f || bmx->m_WheelCounts[3] > 0.0f)
    ) {
        auto power = std::min(bmx->m_fControlJump / 25.0f, 1.0f) + 1.0f;
        if (bmx->GetStatus() == STATUS_PLAYER) {
            power *= CStats::GetFatAndMuscleModifier(STAT_MOD_6);
        }
        if (CCheat::IsActive(CHEAT_HUGE_BUNNY_HOP)) {
            power *= 5.0f;
        }
        bmx->ApplyMoveForce(0.06f * bmx->m_fMass * power * bmx->m_matrix->GetUp());
        bmx->ApplyTurnForce(0.01f * bmx->m_fTurnMass * power * bmx->m_matrix->GetUp(), bmx->m_matrix->GetForward());
    }
}

// 0x6C0500 | inlined | see 0x6C11F3
void CBmx::GetFrameOffset(float& fZOffset, float& fAngleOffset) {
    const auto d1 = m_aWheelSuspensionHeights[0] - m_aWheelOrigHeights[0];
    const auto d2 = m_aWheelSuspensionHeights[1] - m_aWheelOrigHeights[1];

    fZOffset     = (1.0f - m_fMidWheelFracY) * d1 + d2 * m_fMidWheelFracY;
    // Como el original: fpatan x87
    fAngleOffset = TrigAtan2Cpp(d1 - d2, m_fMidWheelDistY);
}

// 0x6C0550
float CBmx::FindWheelWidth(bool bRear) {
    return 0.07f;
}

// 0x6C0560
void CBmx::BlowUpCar(CEntity* damager, bool bHideExplosion) {
    // NOP
}

// 0x6C0590
void CBmx::ProcessBunnyHop() {
    auto* anim = m_pDriver
        ? RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_BUNNYHOP)
        : nullptr;

    if (GetStatus() != STATUS_PLAYER || !m_pDriver || !m_pDriver->IsPlayer()) {
        if (anim) {
            anim->SetFlag(ANIMATION_IS_PLAYING, true);
            anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
            anim->SetBlendDelta(-8.0f);
        }
        return;
    }

    auto pad = m_pDriver->AsPlayer()->GetPadFromPlayer();

    if (pad->IsLeftShoulder1Pressed() && !pad->DisablePlayerControls && m_fControlJump == 0.0f) {
        m_fControlJump = CTimer::GetTimeStep() + m_fControlJump;
        anim = CAnimManager::BlendAnimation(m_pDriver->GetRpClump(), m_RideAnimData.AnimGroup, ANIM_ID_BIKE_BUNNYHOP, 8.0f);
        if (anim) {
            anim->SetCurrentTime(0.0f);
            anim->SetFlag(ANIMATION_IS_PLAYING, false);
        }
    }

    if (m_fControlJump > 0.0f && !pad->DisablePlayerControls) {
        if (!anim) {
            m_fControlJump = 0.0f;
            return;
        }

        if (pad->IsLeftShoulder1()) {
            if (!anim->IsPlaying()) {
                m_fControlJump = std::min(CTimer::GetTimeStep() + m_fControlJump, 25.0f);
                anim->SetCurrentTime(m_fControlJump / 25.0f * 0.2f);
            }
        } else if (!anim->IsPlaying()) {
            if (anim->GetCurrentTime() < 0.2f) {
                anim->SetCurrentTime((0.2f - anim->GetCurrentTime()) / 0.2f * (anim->GetHier()->GetTotalTime() - 0.2f) + 0.2f);
            }
            anim->SetFlag(ANIMATION_IS_PLAYING, true);
            anim->SetSpeed(1.5f);
            anim->SetFinishCallback(CBmx::LaunchBunnyHopCB, this);
        }
    } else {
        // Como el original: sin salto en curso (o con los controles bloqueados) la animacion se retira
        if (m_fControlJump > 0.0f) {
            m_fControlJump = 0.0f;
        }
        if (!anim) {
            return;
        }
        anim->SetFlag(ANIMATION_IS_PLAYING, true);
        anim->SetFlag(ANIMATION_IS_BLEND_AUTO_REMOVE, true);
        anim->SetBlendDelta(-8.0f);
    }

    if (anim->GetBlendAmount() > 0.5f) {
        m_GasPedal                                       = 0.0f;
        FindPlayerPed()->GetPlayerData()->m_fMoveSpeed = 0.0f;
        if (!vehicleFlags.bIsHandbrakeOn && (m_aWheelRatios[0] < 1.0f || m_aWheelRatios[1] < 1.0f || m_aWheelRatios[2] < 1.0f || m_aWheelRatios[3] < 1.0f)) {
            m_bIsFreewheeling = true;
        }
    }
}

// 0x6C0810
void CBmx::PreRender() {
    CVehicle::PreRender();

    CColModel* const colModel = GetColModel();
    CCollisionData* const colData = colModel->m_pColData;

    if (vehicleFlags.bVehicleColProcessed) {
        DoBurstAndSoftGroundRatios();

        // Como el original: rueda delantera (linea 0)
        {
            const float a = 1.0f - m_fSuspensionLength[0] / m_fLineLength[0];
            const float m = m_aWheelRatios[1] < m_aWheelRatios[0] ? m_aWheelRatios[1] : m_aWheelRatios[0];
            const float t = (m - a) / (1.0f - a);
            float z       = colData->m_pLines[0].m_vecStart.z;
            if (t > 0.0f) {
                z = z - t * m_fSuspensionLength[0];
            }
            m_aWheelSuspensionHeights[0] = (z - m_aWheelSuspensionHeights[0]) * 0.75f + m_aWheelSuspensionHeights[0];
        }
        // Como el original: rueda trasera (linea 2)
        {
            const float a = 1.0f - m_fSuspensionLength[2] / m_fLineLength[2];
            const float m = m_aWheelRatios[3] < m_aWheelRatios[2] ? m_aWheelRatios[3] : m_aWheelRatios[2];
            const float t = (m - a) / (1.0f - a);
            float z       = colData->m_pLines[2].m_vecStart.z;
            if (t > 0.0f) {
                z = z - t * m_fSuspensionLength[2];
            }
            m_aWheelSuspensionHeights[1] = (z - m_aWheelSuspensionHeights[1]) * 0.75f + m_aWheelSuspensionHeights[1];
        }
    }

    const auto status = GetStatus();
    if (status == STATUS_PHYSICS || status == STATUS_PLAYER || status == STATUS_PLAYER_PLAYBACK_FROM_BUFFER || status == STATUS_SIMPLE) {
        // Como el original: se calcula (y no se usa) la velocidad; las particulas de cada rueda salen del rayo elegido
        const float speedMag = m_vecMoveSpeed.Magnitude();
        for (int32 w = 0; w < 2; ++w) {
            int32 ray;
            if (w == 0) {
                ray = (!(m_aRatioHistory[0] < 1.0f) && m_aRatioHistory[1] < 1.0f) ? 1 : 0;
            } else {
                ray = (!(m_aRatioHistory[3] < 1.0f) && m_aRatioHistory[2] < 1.0f) ? 2 : 3;
            }

            uint32 flags = (w == 0 || m_WheelStates[1] == WHEEL_STATE_FIXED) ? 4u : 0u;

            const float S = BmxSinTimes2Cpp(m_RideAnimData.LeanAngle, GetColModel()->m_boundBox.m_vecMin.z, 0.8f);
            CVector     P = m_aWheelColPoints[ray].m_vecPoint;
            P.x           = S * m_matrix->GetRight().x + P.x;
            P.y           = P.y + S * m_matrix->GetRight().y;
            P.z           = P.z + S * m_matrix->GetRight().z;
            if (m_bWheelBloody.data()[w]) {
                flags += 1;
            }
            if (m_bMoreSkidMarks.data()[w]) {
                flags += 2;
            }
            const float dir = m_RideAnimData.LeanAngle > 0.0f ? -1.0f : 1.0f;

            AddSingleWheelParticles(
                m_WheelStates[w],
                m_nWheelStatus.data()[w],
                m_aRatioHistory[ray],
                speedMag,
                &m_aWheelColPoints[ray],
                &P,
                dir,
                w,
                (uint32)m_aWheelSkidmarkType[w],
                &m_bWheelBloody.data()[w],
                flags
            );
        }
    }

    m_bLeanMatrixCalculated = false;
    CalculateLeanMatrix();
    CShadows::StoreShadowForVehicle(this, VEH_SHD_BIKE);

    CMatrix mat;
    const float steer = m_fSteerAngle;
    const CVector steerAxis{ -TrigSimpleSinCpp(steer), TrigSimpleCosCpp(steer), 0.0f };
    const CVector steerDir = BmxMultiply3x3_MV(*m_matrix, steerAxis);
    const CVector fwdCopy  = m_matrix->GetForward();

    auto* const mi = static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo((int32)(int16)m_nModelIndex));

    // Como el original: giro de las ruedas
    if (m_WheelCounts[0] > 0.0f || m_WheelCounts[1] > 0.0f) {
        const float minHist = m_aRatioHistory[0] < m_aRatioHistory[1] ? m_aRatioHistory[0] : m_aRatioHistory[1];
        const CVector p{
            0.0f,
            (colData->m_pLines[1].m_vecStart.y + colData->m_pLines[0].m_vecStart.y) * 0.5f,
            (colData->m_pLines[0].m_vecStart.z - minHist * m_fSuspensionLength[0]) - mi->m_fWheelSizeFront * 0.5f
        };
        const CVector speed = GetSpeed(p);
        const float   av    = ProcessWheelRotation(WHEEL_STATE_NORMAL, steerDir, speed, mi->m_fWheelSizeFront * 0.5f);
        m_aWheelAngularVelocity[0] = av;
        m_aWheelPitchAngles[0]     = av * CTimer::GetTimeStep() + m_aWheelPitchAngles[0];
    }
    if (m_WheelCounts[2] > 0.0f || m_WheelCounts[3] > 0.0f) {
        const float minHist = m_aRatioHistory[2] < m_aRatioHistory[3] ? m_aRatioHistory[2] : m_aRatioHistory[3];
        // Como el original: el radio de la z usa el tamaño de la rueda delantera (0x40) tambien aqui
        const CVector p{
            0.0f,
            (colData->m_pLines[3].m_vecStart.y + colData->m_pLines[2].m_vecStart.y) * 0.5f,
            (colData->m_pLines[2].m_vecStart.z - minHist * m_fSuspensionLength[2]) - mi->m_fWheelSizeFront * 0.5f
        };
        const CVector speed = GetSpeed(p);
        const float   av    = ProcessWheelRotation(m_WheelStates[1], fwdCopy, speed, mi->m_fWheelSizeRear * 0.5f);
        m_aWheelAngularVelocity[1] = av;
        m_aWheelPitchAngles[1]     = av * CTimer::GetTimeStep() + m_aWheelPitchAngles[1];
    }

    // Como el original: horquilla delantera (y manillar dentro de ella)
    if (auto* node = m_aBikeNodes[BMX_FORKS_FRONT]) {
        mat.Attach(RwFrameGetMatrix(node), false);
        const CVector saved = mat.GetPosition();

        RwMatrix rwTmp{};
        CMatrix  tmp;
        tmp.m_pAttachMatrix       = &rwTmp;
        tmp.m_bOwnsAttachedMatrix = false;
        tmp.SetUnity();
        tmp.UpdateRW();

        const float steerAngleModel = mi->m_fBikeSteerAngle;
        // Como el original: el producto grados por radianes queda en ST0 y pasa directo a fsin / fcos (0x6C0E0D..0x6C0E32)
        CVector axis{ 0.0f, BmxSinScaledCpp(steerAngleModel, kBmxDegToRad), -BmxCosScaledCpp(steerAngleModel, kBmxDegToRad) };
        axis.Normalise();
        CQuaternion quat;
        quat.Set(&axis, -m_RideAnimData.BarSteerAngle);
        quat.Get(&rwTmp);
        tmp.Update();

        mat.SetUnity();
        mat = mat * tmp;
        mat.GetPosition() += saved;
        mat.UpdateRW();

        if (auto* handlebars = m_aBikeNodes[BMX_HANDLEBARS]) {
            mat.Attach(RwFrameGetMatrix(handlebars), false);
            const CVector savedBars = mat.GetPosition();
            const auto    st        = GetStatus();
            if (st != STATUS_ABANDONED && st != STATUS_WRECKED) {
                mat.SetTranslate(savedBars);
            } else {
                mat.SetUnity();
                mat *= tmp;
                mat.GetPosition() += savedBars;
            }
            mat.UpdateRW();
        }
    }

    // Como el original: horquilla trasera
    if (auto* node = m_aBikeNodes[BMX_FORKS_REAR]) {
        const float ratio = (m_aWheelSuspensionHeights[1] - m_aWheelOrigHeights[1]) / m_fSwingArmLength;
        const float angle = BmxNegAsinCpp(ratio);
        mat.Attach(RwFrameGetMatrix(node), false);
        const CVector saved = mat.GetPosition();
        mat.SetRotate(angle, 0.0f, 0.0f);
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }

    // Como el original: rueda delantera
    {
        mat.Attach(RwFrameGetMatrix(m_aBikeNodes[BMX_WHEEL_FRONT]), false);
        CVector saved = mat.GetPosition();
        if (BmxPreRenderFlag) {
            saved.z = m_aWheelSuspensionHeights[0] - m_fForkZOffset;
            saved.y = ((colData->m_pLines[1].m_vecStart.y + colData->m_pLines[0].m_vecStart.y) * 0.5f - m_fForkYOffset)
                - (m_aWheelSuspensionHeights[0] - m_aWheelOrigHeights[0]) * m_fSteerAngleTan;
        }
        if (m_nWheelStatus.data()[0] == 1) {
            mat.SetRotate(m_aWheelPitchAngles[0], 0.0f, TrigSimpleSinMulCpp(m_aWheelPitchAngles[0], 0.02f));
        } else {
            mat.SetRotateX(m_aWheelPitchAngles[0]);
        }
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }

    // Como el original: rueda trasera
    {
        mat.Attach(RwFrameGetMatrix(m_aBikeNodes[BMX_WHEEL_REAR]), false);
        CVector saved = mat.GetPosition();
        // Como el original: 0x6C114E compara el nodo en +0x5AC (horquilla trasera, indice 3), no el de la delantera
        if (BmxPreRenderFlag && m_aBikeNodes[BMX_FORKS_REAR] == nullptr) {
            saved.z = m_aWheelSuspensionHeights[1];
        }
        if (m_nWheelStatus.data()[1] == 1) {
            mat.SetRotate(m_aWheelPitchAngles[1], 0.0f, TrigSimpleSinMulCpp(m_aWheelPitchAngles[1], 0.04f));
        } else {
            mat.SetRotateX(m_aWheelPitchAngles[1]);
        }
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }

    // Como el original: chasis (inclinacion lateral y compresion)
    if (auto* node = m_aBikeNodes[BMX_CHASSIS]) {
        float zOff   = 0.0f;
        float angOff = 0.0f;
        if (!BmxPreRenderFlag) {
            GetFrameOffset(zOff, angOff);
        }
        mat.Attach(RwFrameGetMatrix(node), false);
        CVector saved = mat.GetPosition();
        saved.z       = BmxChassisZCpp(m_RideAnimData.LeanAngle, colModel->m_boundBox.m_vecMin.z, zOff);
        mat.SetRotateX(std::fabs(m_RideAnimData.LeanAngle) * -0.05f + angOff);
        mat.RotateY(m_fSprintLeanAngle + m_RideAnimData.LeanAngle);
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }

    // Como el original: cigueñal y pedales
    if (auto* node = m_aBikeNodes[BMX_CHAINSET]) {
        mat.Attach(RwFrameGetMatrix(node), false);
        const CVector saved = mat.GetPosition();
        mat.SetRotate(m_fCrankAngle, 0.0f, 0.0f);
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }
    if (auto* node = m_aBikeNodes[BMX_PEDAL_R]) {
        mat.Attach(RwFrameGetMatrix(node), false);
        const CVector saved = mat.GetPosition();
        mat.SetRotate(m_fPedalAngleL, 0.0f, 0.0f);
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }
    if (auto* node = m_aBikeNodes[BMX_PEDAL_L]) {
        mat.Attach(RwFrameGetMatrix(node), false);
        const CVector saved = mat.GetPosition();
        mat.SetRotate(m_fPedalAngleR, 0.0f, 0.0f);
        mat.GetPosition() += saved;
        mat.UpdateRW();
    }
}

// 0x6C1470
bool CBmx::ProcessAI(uint32& extraHandlingFlags) {
    auto* const mi = static_cast<CVehicleModelInfo*>(CModelInfo::GetModelInfo((int32)(int16)m_nModelIndex));

    m_autoPilot.carCtrlFlags.bHonkAtCar = false;
    m_autoPilot.carCtrlFlags.bHonkAtPed = false;
    m_bIsFreewheeling                   = false;

    // Como el original: Min(0.1, |v|) con el segundo operando; Magnitude se llama dos veces cuando no supera 0.1
    const auto LimitedSpeed = [this]() -> float {
        const float mag = m_vecMoveSpeed.Magnitude();
        return !(mag > 0.1f) ? m_vecMoveSpeed.Magnitude() : 0.1f;
    };
    const auto SquaredSpeed = [this]() -> float {
        return m_vecMoveSpeed.x * m_vecMoveSpeed.x + m_vecMoveSpeed.y * m_vecMoveSpeed.y + m_vecMoveSpeed.z * m_vecMoveSpeed.z;
    };

    switch (GetStatus()) {
    case STATUS_PLAYER: {
        extraHandlingFlags += 2;
        bikeFlags.bGettingPickedUp = false;
        if (!m_pDriver) {
            break;
        }
        if (!m_pDriver->IsPlayer()) {
            break;
        }

        ProcessControlInputs((uint8)m_pDriver->m_nPedType);
        CPad* const    pad    = m_pDriver->AsPlayer()->GetPadFromPlayer();
        CPlayerPed* const player = m_pDriver->AsPlayer();

        // Como el original: la inclinacion hacia delante o atras mueve el centro de masa y aplica un giro
        if (m_RideAnimData.LeanFwd < 0.0f) {
            m_vecCentreOfMass.y = m_BikeHandling->m_fLeanBakCOM * m_RideAnimData.LeanFwd + m_pHandlingData->m_vecCentreOfMass.y;
            if (!((m_BrakePedal != 0.0f || vehicleFlags.bIsHandbrakeOn) && m_nNoOfContactWheels != 0)) {
                const float speedLimit = LimitedSpeed();
                float       gasFactor;
                if (m_nModelIndex == 0x1D4) {
                    gasFactor = m_GasPedal * 0.7f + 0.3f;
                } else {
                    gasFactor = (m_GasPedal + 1.0f) * 0.5f;
                }
                const float f = (((gasFactor * m_BikeHandling->m_fLeanBakForce) * m_fTurnMass) * m_RideAnimData.LeanFwd) * speedLimit;
                const CVector point = m_vecCentreOfMass + m_matrix->GetForward();
                const float   tf    = -(CTimer::GetTimeStep() * f);
                ApplyTurnForce(m_matrix->GetUp() * tf, point);
            }
        } else {
            m_vecCentreOfMass.y = m_BikeHandling->m_fLeanFwdCOM * m_RideAnimData.LeanFwd + m_pHandlingData->m_vecCentreOfMass.y;
            if (m_BrakePedal < 0.0f || m_nNoOfContactWheels == 0) {
                const float speedLimit = LimitedSpeed();
                const float f = ((m_BikeHandling->m_fLeanFwdForce * m_fTurnMass) * speedLimit) * m_RideAnimData.LeanFwd;
                const CVector point = m_vecCentreOfMass + m_matrix->GetForward();
                const float   tf    = -(CTimer::GetTimeStep() * f);
                ApplyTurnForce(m_matrix->GetUp() * tf, point);
            }
        }

        PruneReferences();
        if (GetStatus() == STATUS_PLAYER) {
            DoDriveByShootings();
        }
        DoSoftGroundResistance(extraHandlingFlags);

        // Como el original: con las cuatro ruedas en el aire (compresion exactamente 1.0) el jugador gira la moto con el mando
        if (m_aWheelRatios[0] == 1.0f && m_aWheelRatios[1] == 1.0f && m_aWheelRatios[2] == 1.0f && m_aWheelRatios[3] == 1.0f) {
            const float dot = DotProduct(m_vecTurnSpeed, m_matrix->GetUp());
            bool        apply = false;
            if (dot < 0.04f) {
                apply = float(int32(pad->GetSteeringLeftRight())) < 0.0f;
            }
            if (!apply) {
                if (-0.04f < dot) {
                    apply = float(int32(pad->GetSteeringLeftRight())) > 0.0f;
                }
            }
            if (apply) {
                const float ts = CTimer::GetTimeStep();
                const float f  = ((float(int32(pad->GetSteeringLeftRight())) * 0.0078125f) * m_fTurnMass * ts) * 0.002f;
                ApplyTurnForce(m_matrix->GetRight() * f, m_matrix->GetForward());
            }
        }

        ProcessBunnyHop();

        auto* const sprintA = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_SPRINT);
        bool        sprinting = false;
        if (player->ControlButtonSprint(SPRINT_BMX) > 1.2f) {
            sprinting = true;
        } else if (player->GetButtonSprintResults(SPRINT_BMX) > 1.0f && sprintA && sprintA->GetBlendAmount() > 0.5f) {
            sprinting = true;
        }

        if (sprinting) {
            bikeFlags.bPlayerBoost = true;
            m_fControlPedaling     = player->GetPlayerData()->m_fMoveSpeed;
        } else {
            const int16 acc = pad->GetAccelerate();
            const float v   = 1.0f - (float(int32(acc)) * kBmxInv255) * 0.5f;
            player->HandleSprintEnergy(false, BmxMax(0.5f, v));
            if (player->GetButtonSprintResults(SPRINT_BMX) > 0.0f) {
                m_fControlPedaling = std::bit_cast<float>(0x409CCCCDu);
                if (m_GasPedal == 0.0f && m_BrakePedal == 0.0f) {
                    m_GasPedal = 1.0f;
                }
            } else if (player->GetPlayerData()->m_fTimeCanRun < 0.0f) {
                if (pad->GetAccelerateJustDown()) {
                    m_fControlPedaling = std::bit_cast<float>(0x409CCCCDu);
                } else {
                    m_fControlPedaling = BmxMax(0.0f, m_fControlPedaling - 0.4f);
                }
            } else {
                m_fControlPedaling = 0.0f;
            }
        }

        CStats::UpdateStatsWhenCycling(bikeFlags.bPlayerBoost, this);
        if (pad->CarGunJustDown()) {
            ActivateBomb();
        }
        break;
    }
    case STATUS_PLAYER_PLAYBACK_FROM_BUFFER:
        extraHandlingFlags += 2;
        break;
    case STATUS_SIMPLE: {
        CCarAI::UpdateCarAI(this);
        CPhysical::ProcessControl();
        CCarCtrl::UpdateCarOnRails(this);

        const float simpleSpeed = m_autoPilot.m_speed * 0.02f;
        const uint8 onGroundNow = m_NumDriveWheelsOnGround;
        m_nNoOfContactWheels                = 2;
        m_NumDriveWheelsOnGroundLastFrame   = onGroundNow;
        m_NumDriveWheelsOnGround            = 2;
        m_pHandlingData->GetTransmission().CalculateGearForSimpleCar(simpleSpeed, m_nCurrentGear);

        m_aWheelPitchAngles[0] = ProcessWheelRotation(WHEEL_STATE_NORMAL, m_matrix->GetForward(), m_vecMoveSpeed, mi->m_fWheelSizeFront * 0.5f) + m_aWheelPitchAngles[0];
        m_aWheelPitchAngles[1] = ProcessWheelRotation(WHEEL_STATE_NORMAL, m_matrix->GetForward(), m_vecMoveSpeed, mi->m_fWheelSizeRear * 0.5f) + m_aWheelPitchAngles[1];

        PlayHornIfNecessary();
        ReduceHornCounter();
        vehicleFlags.bVehicleColProcessed = false;
        vehicleFlags.bAudioChangingGear   = false;
        bikeFlags.bWheelieForCamera       = false;
        m_fControlPedaling                = 0.0f;
        break;
    }
    case STATUS_PHYSICS: {
        CCarAI::UpdateCarAI(this);
        CCarCtrl::SteerAICarWithPhysics(this);
        PlayHornIfNecessary();
        extraHandlingFlags += 2;
        bikeFlags.bWheelieForCamera = false;
        if (vehicleFlags.bIsBeingCarJacked) {
            vehicleFlags.bIsHandbrakeOn = true;
            m_GasPedal                  = 0.0f;
            m_BrakePedal                = 1.0f;
        } else {
            bikeFlags.bGettingPickedUp = false;
        }
        if (m_fControlPedaling > 0.0f) {
            const float timeStep = CTimer::GetTimeStep();
            float       dec;
            if (m_fControlPedaling > 5.0f) {
                dec = timeStep * 0.02f;
            } else {
                dec = timeStep * 0.01f;
                if (vehicleFlags.bUseCarCheats) {
                    dec = dec + dec;
                }
            }
            m_fControlPedaling = m_fControlPedaling - dec;
            if (m_fControlPedaling < 0.0f) {
                m_fControlPedaling = 0.0f;
            }
        }
        break;
    }
    case STATUS_ABANDONED: {
        m_BrakePedal = 0.0f;
        if (SquaredSpeed() < 0.01f || bikeFlags.bOnSideStand) {
            vehicleFlags.bIsHandbrakeOn = true;
        } else {
            vehicleFlags.bIsHandbrakeOn = false;
        }
        m_GasPedal             = 0.0f;
        m_HornCounter          = 0; // Como el original: dword en 0x514
        if ((m_pDriver || m_apPassengers[0] || vehicleFlags.bIsBeingCarJacked) && !bikeFlags.bOnSideStand) {
            extraHandlingFlags += 2;
        }
        m_RideAnimData.AnimLeanLeft = 0.0f;
        m_RideAnimData.AnimLeanFwd  = 0.0f;
        bikeFlags.bWheelieForCamera = false;
        m_fControlPedaling          = 0.0f;
        if (vehicleFlags.bIsBeingCarJacked) {
            vehicleFlags.bIsHandbrakeOn = true;
            m_GasPedal                  = 0.0f;
            m_BrakePedal                = 1.0f;
        }
        break;
    }
    case STATUS_WRECKED:
        vehicleFlags.bIsHandbrakeOn = true;
        bikeFlags.bWheelieForCamera = false;
        m_BrakePedal                = 0.05f;
        m_fSteerAngle               = 0.0f;
        m_GasPedal                  = 0.0f;
        m_HornCounter                = 0; // Como el original: dword en 0x514
        m_fControlPedaling          = 0.0f;
        m_RideAnimData.AnimLeanLeft = 0.0f;
        m_RideAnimData.AnimLeanFwd  = 0.0f;
        break;
    case STATUS_FORCED_STOP: {
        if (SquaredSpeed() < 0.01f) {
            m_BrakePedal                = 1.0f;
            vehicleFlags.bIsHandbrakeOn = true;
        } else {
            m_BrakePedal                = 0.0f;
            vehicleFlags.bIsHandbrakeOn = false;
        }
        m_fSteerAngle         = 0.0f;
        m_GasPedal            = 0.0f;
        m_HornCounter         = 0; // Como el original: dword en 0x514
        extraHandlingFlags += 2;
        bikeFlags.bWheelieForCamera = false;
        m_fControlPedaling          = 0.0f;
        break;
    }
    default:
        break;
    }

    // Como el original: animaciones de inclinacion del conductor (hacia delante/atras o tiro lateral)
    if (m_pDriver) {
        RpClump* const clump = m_pDriver->GetRpClump();
        auto* leanA = RpAnimBlendClumpGetAssociation(clump, ANIM_ID_BIKE_FWD);
        if (!leanA || leanA->GetBlendAmount() < 0.5f) {
            leanA = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_BACK);
        }
        auto* driveA = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_DRIVEBYLHS);
        if (!driveA || driveA->GetBlendAmount() < 0.5f) {
            driveA = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_DRIVEBYRHS);
        }
        if (!driveA || driveA->GetBlendAmount() < 0.5f) {
            driveA = RpAnimBlendClumpGetAssociation(m_pDriver->GetRpClump(), ANIM_ID_BIKE_DRIVEBYFT);
        }

        if ((leanA && leanA->GetBlendAmount() > 0.5f) || (driveA && driveA->GetBlendAmount() > 0.5f)) {
            const bool handbrake = vehicleFlags.bIsHandbrakeOn;
            m_GasPedal           = 0.0f;
            if (handbrake) {
                return true;
            }
            if (m_aWheelRatios[0] < 1.0f || m_aWheelRatios[1] < 1.0f || m_aWheelRatios[2] < 1.0f || m_aWheelRatios[3] < 1.0f) {
                m_bIsFreewheeling = true;
            }
            return true;
        }
    }

    // Como el original: empuje hacia delante cuando se pedalea con la rueda trasera en el suelo
    if (!(m_fControlPedaling > 5.0f)) {
        return true;
    }
    if (!(m_aWheelRatios[2] < 1.0f || m_aWheelRatios[3] < 1.0f)) {
        return true;
    }

    const float along = DotProduct(m_vecMoveSpeed, m_matrix->GetForward());
    float       v     = 2.4f - (along / m_pHandlingData->GetTransmission().m_MaxFlatVelocity) * 1.5f;
    if (v > 2.0f) {
        v = 2.0f;
    } else if (v < 0.0f) {
        v = 0.0f;
    }
    if (GetStatus() == STATUS_PLAYER) {
        v = CStats::GetFatAndMuscleModifier(STAT_MOD_5) * v;
    } else if (vehicleFlags.bUseCarCheats) {
        v = v * 1.25f;
    }
    const float y = (v * ((CTimer::GetTimeStep() * m_fMass) * 0.3f)) * kBmxK863984;
    ApplyMoveForce(CVector{ y * m_matrix->GetForward().x, y * m_matrix->GetForward().y, y * m_matrix->GetForward().z });
    return true;
}
