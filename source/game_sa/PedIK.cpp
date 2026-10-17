/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedIK
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 9/9 entradas.
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
#include "X87Math.h"

static inline auto& bRotateWithNeck = StaticRef<bool>(0x8D2354);
// Como el original: PitchForSlope elige el producto de los muslos según este byte del EXE (0xC0AFC0)
static inline auto& bPitchForSlopeAbsAsin = StaticRef<bool>(0xC0AFC0);

namespace {
// Como el original: 0x859878 = 0x42652EE0 (57.29578f, no es 180/PI redondeado)
constexpr float IK_RAD_TO_DEG = 57.2957763671875f;

// Como el original: los bloques x87 se resuelven con los ayudantes compartidos de X87Math.cpp:
//   asin (_CIasin, 0x821E70)         -> CnvAsinFloatCpp (fstp dword del asin, todas las ramas)
//   fpatan(y, x) - rot (24 bits)     -> Atan2MinusCpp
//   fsin / fcos guardados como float -> TrigSimpleSinCpp / TrigSimpleCosCpp
//   fsin(a) * k                      -> TrigSimpleSinMulCpp
//   num / fcos(a)                    -> PedIKDivCosCpp
//   CMatrix::RotateX / RotateY       -> MatrixRotateXCpp / MatrixRotateYCpp (0x59B1E0 / 0x59B2C0)
//   rumbo del brazo                  -> PedIKArmHeadingDiffCpp
void SinCosExe(float angle, float& outSin, float& outCos) {
    outSin = TrigSimpleSinCpp(angle);
    outCos = TrigSimpleCosCpp(angle);
}

void RotateXExe(CMatrix& matrix, float angle) {
    MatrixRotateXCpp(reinterpret_cast<float*>(&matrix), angle);
}

void RotateYExe(CMatrix& matrix, float angle) {
    MatrixRotateYCpp(reinterpret_cast<float*>(&matrix), angle);
}
} // namespace

void CPedIK::InjectHooks() {
    RH_ScopedClass(CPedIK);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Constructor, 0x5FD8C0);
    RH_ScopedInstall(RotateTorso, 0x5FDDB0);
    RH_ScopedInstall(RotateTorsoForArm, 0x5FDF90);
    RH_ScopedOverloadedInstall(MoveLimb, "non-normalized", 0x5FDA60, MoveLimbResult(CPedIK::*)(LimbOrientation&, float, float, LimbMovementInfo&));
    RH_ScopedOverloadedInstall(MoveLimb, "normalized", 0x5FDB60, MoveLimbResult(CPedIK::*)(LimbOrientation&, float, float, LimbMovementInfo&, float));
    RH_ScopedInstall(PointGunAtPosition, 0x5FDE20);
    RH_ScopedInstall(PointGunInDirection, 0x5FDC00);
    RH_ScopedInstall(PitchForSlope, 0x5FE0E0);
    RH_ScopedInstall(GetWorldMatrix, 0x5FD8F0);
}

// 0x5FDDB0
void CPedIK::RotateTorso(AnimBlendFrameData* bone, LimbOrientation& orientation, bool changeRoll) {
    const auto q = &bone->KeyFrame->q;
    // Como el original: el flag elige el modo de combinación de la primera rotación
    RtQuatRotate(q, &XaxisIK, orientation.m_fYaw * IK_RAD_TO_DEG, changeRoll ? rwCOMBINEPRECONCAT : rwCOMBINEREPLACE);
    RtQuatRotate(q, &ZaxisIK, orientation.m_fPitch * IK_RAD_TO_DEG, rwCOMBINEPRECONCAT);
    m_pPed->bUpdateMatricesRequired = true; // Como el original: 0x470 |= 0x4000 (no bDontAcceptIKLookAts)
}

// 0x5FDF90
void CPedIK::RotateTorsoForArm(const CVector& direction) {
    const auto& pedPos = m_pPed->GetPosition();
    const float difAngle = PedIKArmHeadingDiffCpp(&direction.x, &pedPos.x, m_pPed->m_fCurrentRotation);

    float resultAngle;
    if (difAngle > std::bit_cast<float>(0x3F490FDBu)) { // 0x86C860
        resultAngle = difAngle - std::bit_cast<float>(0x3F490FDBu);
        if (resultAngle > std::bit_cast<float>(0x3F490FDBu)) { // 0x86C864
            resultAngle = std::bit_cast<float>(0x3F490FDBu);
        }
    } else {
        if (!(difAngle < std::bit_cast<float>(0xBF860A92u))) { // 0x86C868
            return;
        }
        resultAngle = difAngle - std::bit_cast<float>(0xBF860A92u);
        if (resultAngle < std::bit_cast<float>(0xBEB2B8C3u)) { // 0x86C86C
            resultAngle = std::bit_cast<float>(0xBEB2B8C3u);
        }
    }

    if (resultAngle == 0.0f) {
        return;
    }

    // Como el original: sin bRotateWithNeck el torso gira el ángulo entero, no la mitad
    if (bRotateWithNeck) {
        resultAngle = resultAngle * 0.5f;
        RtQuatRotate(&m_pPed->m_apBones[PED_NODE_NECK]->KeyFrame->q, &XaxisIK, resultAngle * IK_RAD_TO_DEG, rwCOMBINEPOSTCONCAT);
    }
    RtQuatRotate(&m_pPed->m_apBones[PED_NODE_UPPER_TORSO]->KeyFrame->q, &XaxisIK, resultAngle * IK_RAD_TO_DEG, rwCOMBINEPOSTCONCAT);
}

// 0x5FDC00
bool CPedIK::PointGunInDirection(float zAngle, float distance, bool flag, float normalize) {
    const auto angle = CGeneral::LimitRadianAngle(zAngle - m_pPed->m_fCurrentRotation);
    bGunReachedTarget = false;
    bTorsoUsed = true;

    const auto hier  = GetAnimHierarchyFromSkinClump(m_pPed->GetRpClump());
    const auto index = RpHAnimIDGetIndex(hier, m_pPed->m_apBones[PED_NODE_RIGHT_CLAVICLE]->BoneTag);

    // Como el original: copia sin uso de la matriz de la clavícula en una matriz temporal de RW
    auto* const boneMatrix = RwMatrixCreate();
    *boneMatrix = RpHAnimHierarchyGetMatrixArray(hier)[index];
    RwMatrixDestroy(boneMatrix);

    // Como el original: un normalize NaN también toma la versión sin normalizar
    const auto limbResult = !(normalize >= 0.0f)
        ? MoveLimb(m_TorsoOrient, angle, distance, ms_torsoInfo)
        : MoveLimb(m_TorsoOrient, angle, distance, ms_torsoInfo, normalize);

    bool canReach = true;
    switch (limbResult) {
    case CANT_REACH_TARGET:
        canReach = false;
        break;
    case REACHED_TARGET:
        bGunReachedTarget = true;
        break;
    }

    // Como el original: vuelve a pedir la jerarquía y usa la matriz 2 del array
    const auto headHier  = GetAnimHierarchyFromSkinClump(m_pPed->GetRpClump());
    const auto head      = &RpHAnimHierarchyGetMatrixArray(headHier)[PED_NODE_HEAD];
    const auto headAngle = -CGeneral::LimitRadianAngle(Atan2MinusCpp(-head->at.y, -head->at.x, m_pPed->m_fCurrentRotation));

    float headSin, headCos;
    SinCosExe(headAngle, headSin, headCos);
    const auto axis = CVector{
        0.0f,
        flag ? headCos : -headSin,
        flag ? headSin :  headCos
    };

    const auto torsoQ = &m_pPed->m_apBones[PED_NODE_UPPER_TORSO]->KeyFrame->q;
    RtQuatRotate(torsoQ, &axis, m_TorsoOrient.m_fPitch * IK_RAD_TO_DEG, rwCOMBINEPOSTCONCAT);
    RtQuatRotate(torsoQ, &XaxisIK, m_TorsoOrient.m_fYaw * IK_RAD_TO_DEG, rwCOMBINEPOSTCONCAT);
    m_pPed->bUpdateMatricesRequired = true;

    return canReach;
}

// 0x5FDE20
void CPedIK::PointGunAtPosition(const CVector& aimAt, float normalize) {
    const auto& offset = CWeaponInfo::GetWeaponInfo(m_pPed)->GetAimingOffset();
    const auto aimFrom = m_pPed->GetPosition()
        + (m_pPed->bIsDucking ? offset.DuckX : offset.AimX) * m_pPed->GetRight()
        + (m_pPed->bIsDucking ? offset.DuckZ : offset.AimZ) * m_pPed->GetUp();
    PointGunInDirection(
        CGeneral::GetRadianAngleBetweenPoints(aimAt, aimFrom),
        CGeneral::GetRadianAngleBetweenPoints(
            aimAt.z, CVector2D::Dist(aimFrom, aimAt),
            aimFrom.z, 0.0f
        ),
        false,
        normalize
    );
}

// 0x5FE0E0
void CPedIK::PitchForSlope() {
    const auto clumpData = RpAnimBlendClumpGetData(m_pPed->GetRpClump());
    const auto hier      = GetAnimHierarchyFromSkinClump(m_pPed->GetRpClump());

    // Como el original: "muriendo" incluye bFallenDown, pero los límites solo miran el estado
    const auto state       = m_pPed->m_nPedState;
    const bool isDeadState = state == PEDSTATE_DIE || state == PEDSTATE_DEAD;
    const bool dying       = isDeadState || m_pPed->bFallenDown;
    const auto IsSignificant = [](float v) { return v > 0.01f || v < -0.01f; };
    // Como el original: operator*= (0x411A80 -> 0x59BE30) multiplica con el x87 (exponente extendido en los productos intermedios);
    // el producto en float de SSE desbordaba a inf/NaN con matrices de ~1e30 (fsin/fcos fuera de rango dejan el argumento).
    const auto MulExe = [](CMatrix& lhs, const CMatrix& rhs) {
        CMatrix product;
        PedIKFixMatMulCpp(reinterpret_cast<const float*>(&lhs), reinterpret_cast<const float*>(&rhs), reinterpret_cast<float*>(&product));
        lhs = product;
    };

    if (IsSignificant(m_fSlopePitch) && !isDeadState) {
        const float moveMult = m_pPed->m_nMoveState == PEDMOVE_WALK ? 0.85f : 1.0f;
        const float oneMinus = 1.0f - 0.9f;
        m_fSlopePitchLimitMult = oneMinus * moveMult + 0.9f * m_fSlopePitchLimitMult;

        // Como el original: primero el límite superior; el inferior solo si no se aplicó
        const float highBound = m_fSlopePitchLimitMult * std::bit_cast<float>(0x3F060A92u); // 0x86C870
        if (highBound < m_fSlopePitch) {
            m_fSlopePitch = highBound;
        } else {
            const float lowBound = m_fSlopePitchLimitMult * std::bit_cast<float>(0xBF1C61AAu); // 0x86C874
            if (lowBound > m_fSlopePitch) {
                m_fSlopePitch = lowBound;
            }
        }
    }

    if (IsSignificant(m_fSlopeRoll) && !isDeadState) {
        const float limit = std::bit_cast<float>(0x3F490FDBu); // 0x86C884
        if (m_fSlopeRoll > limit) {
            m_fSlopeRoll = limit;
        }
        if (-limit > m_fSlopeRoll) {
            m_fSlopeRoll = -limit;
        }
    }

    if (IsSignificant(m_fBodyRoll)) {
        const float limit = std::bit_cast<float>(0x3F060A92u); // 0x86C888
        if (m_fBodyRoll > limit) {
            m_fBodyRoll = limit;
        }
        if (-limit > m_fBodyRoll) {
            m_fBodyRoll = -limit;
        }
    }

    if (dying) {
        CMatrix matResult(m_pPed->GetModellingMatrix(), false);
        CMatrix matTransl;
        matTransl.SetTranslate(CVector{ 0.0f, 0.0f, 1.0f });
        RotateXExe(matTransl, -m_fSlopePitch);
        RotateYExe(matTransl, m_fSlopeRoll);
        MulExe(matResult, matTransl);
        matResult.GetPosition().z -= 1.0f;
        matResult.UpdateRW();
        m_pPed->UpdateRwFrame();
        bSlopePitch = false;
        return;
    }

    const auto GetBoneQuat = [&](eBoneTag bone) {
        return &clumpData->m_FrameDatas[RpHAnimIDGetIndex(hier, bone)].KeyFrame->q;
    };
    const auto GetBoneMatrix = [&](eBoneTag bone) {
        return &RpHAnimHierarchyGetMatrixArray(hier)[RpHAnimIDGetIndex(hier, bone)];
    };

    if (IsSignificant(m_fSlopePitch)) {
        const auto spineQ = GetBoneQuat(BONE_SPINE);
        auto& taskMgr = m_pPed->GetIntelligence()->GetTaskManager();
        // Como el original: comprueba que haya tarea activa antes de pedir su tipo
        if (taskMgr.GetActiveTask() && taskMgr.GetActiveTask()->GetTaskType() == TASK_SIMPLE_STEALTH_KILL) {
            RtQuatRotate(spineQ, &ZaxisIK, m_fSlopePitch * IK_RAD_TO_DEG, rwCOMBINEPRECONCAT);
            m_pPed->bUpdateMatricesRequired = true;
        } else {
            RtQuatRotate(spineQ, &ZaxisIK, -m_fSlopePitch * IK_RAD_TO_DEG * 0.75f, rwCOMBINEPRECONCAT);
            m_pPed->bUpdateMatricesRequired = true;

            // Como el original: pantorrilla y pie usan el asin del muslo y se rotan en POSTCONCAT
            const auto RotateLeg = [&](eBoneTag thigh, eBoneTag calf, eBoneTag foot) {
                const float thighAsin  = CnvAsinFloatCpp(GetBoneMatrix(thigh)->up.z);
                const float thighAngle = bPitchForSlopeAbsAsin
                    ? std::abs(thighAsin) * m_fSlopePitch * 30.0f
                    : 30.0f * m_fSlopePitch * thighAsin;
                RtQuatRotate(GetBoneQuat(thigh), &ZaxisIK, thighAngle, rwCOMBINEPOSTCONCAT);

                float calfBase = -(thighAsin * m_fSlopePitch);
                if (0.0f > calfBase) {
                    calfBase = 0.0f;
                }
                RtQuatRotate(GetBoneQuat(calf), &ZaxisIK, calfBase * -30.0f, rwCOMBINEPOSTCONCAT);

                const float footBase = (0.0f > thighAsin) ? 0.0f : thighAsin;
                RtQuatRotate(GetBoneQuat(foot), &ZaxisIK, footBase * m_fSlopePitch * -30.0f, rwCOMBINEPOSTCONCAT);
            };
            RotateLeg(BONE_R_THIGH, BONE_R_CALF, BONE_R_FOOT);
            RotateLeg(BONE_L_THIGH, BONE_L_CALF, BONE_L_FOOT);

            CMatrix result(m_pPed->GetModellingMatrix(), false);
            CMatrix translate;
            translate.SetTranslate(CVector{ 0.0f, 0.0f, 0.0f });
            RotateXExe(translate, -(0.5f * m_fSlopePitch));
            MulExe(result, translate);

            const float fwdScale = TrigSimpleSinMulCpp(m_fSlopePitch, 0.75f);
            const auto& fwd = m_pPed->GetForward();
            auto& pos = result.GetPosition();
            pos.x = pos.x + fwdScale * fwd.x;
            pos.y = pos.y + fwdScale * fwd.y;
            const float lift = PedIKDivCosCpp(0.3f, m_fSlopeRoll) - 0.3f;
            pos.z = (pos.z + fwdScale * fwd.z) + lift;
            result.UpdateRW();
            m_pPed->UpdateRwFrame();
        }
    } else {
        m_fSlopePitch = 0.0f;
    }

    if (IsSignificant(m_fSlopeRoll)) {
        const auto RotateFoot = [&](eBoneTag bone) {
            const auto q = GetBoneQuat(bone);
            const auto hierMatrix = GetBoneMatrix(bone);
            const auto angle = CGeneral::LimitRadianAngle(Atan2MinusCpp(hierMatrix->at.y, hierMatrix->at.x, m_pPed->m_fCurrentRotation));
            float footSin, footCos;
            SinCosExe(angle, footSin, footCos);
            const CVector axis{ 0.0f, footCos, footSin };
            RtQuatRotate(q, &axis, m_fSlopeRoll * IK_RAD_TO_DEG, rwCOMBINEPRECONCAT);
        };
        RotateFoot(BONE_L_FOOT);
        RotateFoot(BONE_R_FOOT);
        m_pPed->bUpdateMatricesRequired = true;
    } else {
        m_fSlopeRoll = 0.0f;
    }

    bSlopePitch = false;
}

// unused
// 0x5FD8F0
RwMatrixTag* CPedIK::GetWorldMatrix(RwFrame* frame, RwMatrixTag* transformMat) {
    *transformMat = *RwFrameGetMatrix(frame);
    for (frame = RwFrameGetParent(frame); frame; frame = RwFrameGetParent(frame)) {
        RwMatrixTransform(transformMat, RwFrameGetMatrix(frame), rwCOMBINEPOSTCONCAT);
    }
    return transformMat;
}

// 0x5FDA60
MoveLimbResult CPedIK::MoveLimb(LimbOrientation& limb, float targetYaw, float targetPitch, LimbMovementInfo& moveInfo) {
    auto result = HAVENT_REACHED_TARGET;

    // yaw
    if (std::abs(limb.m_fYaw - targetYaw) < moveInfo.yawD) {
        limb.m_fYaw = targetYaw;
        result = REACHED_TARGET;
    } else if (limb.m_fYaw < targetYaw) {
        limb.m_fYaw += moveInfo.yawD;
    } else if (limb.m_fYaw > targetYaw) {
        limb.m_fYaw -= moveInfo.yawD;
    }

    // Como el original: se compara primero con el máximo (sin std::clamp)
    if (limb.m_fYaw > moveInfo.maxYaw) {
        limb.m_fYaw = moveInfo.maxYaw;
        result = CANT_REACH_TARGET;
    } else if (limb.m_fYaw < moveInfo.minYaw) {
        limb.m_fYaw = moveInfo.minYaw;
        result = CANT_REACH_TARGET;
    }

    // pitch
    // Como el original: el resultado solo vuelve a HAVENT_REACHED_TARGET si el pitch se mueve
    if (std::abs(limb.m_fPitch - targetPitch) < moveInfo.pitchD) {
        limb.m_fPitch = targetPitch;
    } else if (limb.m_fPitch < targetPitch) {
        limb.m_fPitch += moveInfo.pitchD;
        result = HAVENT_REACHED_TARGET;
    } else if (limb.m_fPitch > targetPitch) {
        limb.m_fPitch -= moveInfo.pitchD;
        result = HAVENT_REACHED_TARGET;
    }

    if (limb.m_fPitch > moveInfo.maxPitch) {
        limb.m_fPitch = moveInfo.maxPitch;
        return CANT_REACH_TARGET;
    }
    if (limb.m_fPitch < moveInfo.minPitch) {
        limb.m_fPitch = moveInfo.minPitch;
        return CANT_REACH_TARGET;
    }
    return result;
}

// 0x5FDB60
MoveLimbResult CPedIK::MoveLimb(LimbOrientation& limb, float targetYaw, float targetPitch, LimbMovementInfo& moveInfo, float normalize) {
    auto result = HAVENT_REACHED_TARGET;

    limb.m_fYaw   = targetYaw * normalize;
    limb.m_fPitch = targetPitch * normalize;

    // Como el original: se compara primero con el máximo (sin std::clamp)
    if (limb.m_fYaw > moveInfo.maxYaw) {
        limb.m_fYaw = moveInfo.maxYaw;
        result = CANT_REACH_TARGET;
    } else if (limb.m_fYaw < moveInfo.minYaw) {
        limb.m_fYaw = moveInfo.minYaw;
        result = CANT_REACH_TARGET;
    }

    if (limb.m_fPitch > moveInfo.maxPitch) {
        limb.m_fPitch = moveInfo.maxPitch;
        return CANT_REACH_TARGET;
    }
    if (limb.m_fPitch < moveInfo.minPitch) {
        limb.m_fPitch = moveInfo.minPitch;
        return CANT_REACH_TARGET;
    }
    if (result != CANT_REACH_TARGET && normalize > 0.9f) {
        return REACHED_TARGET;
    }
    return result;
}
