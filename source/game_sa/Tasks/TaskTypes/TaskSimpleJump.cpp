/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskSimpleJump
 * Reconstrucción: COMPLETA — 8/8 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 8/8 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskSimpleJump.h"
#include "TaskSimpleClimb.h"
#include "Shadows.h"

void CTaskSimpleJump::InjectHooks() {
    RH_ScopedVirtualClass(CTaskSimpleJump, 0x87054C, 9);
    RH_ScopedCategory("Tasks/TaskTypes");
    RH_ScopedInstall(Constructor, 0x679AA0);
    RH_ScopedInstall(CheckIfJumpBlocked, 0x67D590);
    RH_ScopedInstall(Launch, 0x679B80);
    RH_ScopedInstall(StartLaunchAnim, 0x67D7A0);
    RH_ScopedInstall(JumpAnimFinishCB, 0x67A020);
    // VTABLE
    RH_ScopedVMTInstall(Clone, 0x67C510);
    RH_ScopedVMTInstall(MakeAbortable, 0x679B60);
    RH_ScopedVMTInstall(ProcessPed, 0x680C60);
}

CTaskSimpleJump* CTaskSimpleJump::Constructor(bool bCanClimb) {
    this->CTaskSimpleJump::CTaskSimpleJump(bCanClimb);
    return this;
}

// 0x679AA0
CTaskSimpleJump::CTaskSimpleJump(bool bCanClimb) {
    m_bCanClimb = bCanClimb;
    m_pClimbEntity = nullptr;
    m_bIsFinished = false;
    m_bIsJumpBlocked = false;
    m_bClimbJump = false;
    m_bLaunchAnimStarted = false;
    m_bHighJump = false;
    m_pAnim = nullptr;
}

// 0x679AF0
CTaskSimpleJump::~CTaskSimpleJump() {
    if (m_pAnim)
        m_pAnim->SetFinishCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);

    CEntity::SafeCleanUpRef(m_pClimbEntity);
};// 0x67C510
CTask* CTaskSimpleJump::Clone() const {
    auto newTask = new CTaskSimpleJump(m_bCanClimb);
    newTask->m_bCanClimb = this->m_bCanClimb; // Como el original: se copia el byte crudo (el parametro bool lo normaliza)
    newTask->m_bHighJump = this->m_bHighJump;
    return newTask;
}

// 0x679B60
bool CTaskSimpleJump::MakeAbortable(CPed* ped, eAbortPriority priority, const CEvent* event) {
    if (m_pAnim) {
        m_pAnim->m_Flags |= ANIMATION_IS_BLEND_AUTO_REMOVE;
        m_pAnim->m_BlendDelta = -4.0F;
    }

    return priority == ABORT_PRIORITY_IMMEDIATE;
}

// 0x680C60
bool CTaskSimpleJump::ProcessPed(CPed* ped) {
    if (!m_bIsFinished) {
        if (!m_pAnim) {
            if (!StartLaunchAnim(ped)) {
                m_bLaunchAnimStarted = false;
                return true;
            }
            m_bLaunchAnimStarted = true;
        }
        return this->m_bIsFinished;
    }

    if ((ped->IsPlayer() || m_bCanClimb) && CGame::CanSeeOutSideFromCurrArea())
        m_pClimbEntity = CTaskSimpleClimb::TestForClimb(ped, m_vecClimbPos, m_fClimbAngle, m_nClimbSurfaceType, true);

    if (!m_pClimbEntity)
        CheckIfJumpBlocked(ped);

    if (!m_bIsJumpBlocked) {
        Launch(ped);
    } else if (ped->IsPlayer()) {
        // Como el original: el evento vive en la pila y se pasa su direccion (sin copia)
        CEventSoundQuiet event{ ped, 45.f, (uint32)(-1), CVector{} };
        GetEventGlobalGroup()->Add(&event, false);
    }

    return true;
}

// 0x59C890 con su orden de sumas (CMatrix * CVector)
static CVector JumpMatrixTimesPoint59C890(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
        ((m.GetUp().z * v.z + m.GetRight().z * v.x) + m.GetForward().z * v.y) + m.GetPosition().z,
    };
}

// 0x67D590
bool CTaskSimpleJump::CheckIfJumpBlocked(CPed* ped) {
    if (ped->GetIsStuck() || ped->GetEventHandler().GetCurrentEventType() == EVENT_STUCK_IN_AIR)
        return false;

    auto pedColData = CModelInfo::GetModelInfo(ped->m_nModelIndex)->m_pColModel->m_pColData;

    // Como el original: la matriz se lee sin comprobar y la z se suma como ((fwd.z*0,25 + pos.z) + centro.z) + 0,25
    const CMatrix& mat = *ped->m_matrix;
    const CVector& pos = ped->GetPosition();
    const CVector posn{
        mat.GetForward().x * 0.25F + pos.x,
        mat.GetForward().y * 0.25F + pos.y,
        ((mat.GetForward().z * 0.25F + pos.z) + pedColData->m_pSpheres[2].m_vecCenter.z) + 0.25F,
    };

    m_bClimbJump = CWorld::TestSphereAgainstWorld(posn, 0.25F, nullptr, true, true, false, true, false, false) != nullptr;

    const CVector savedPedPosition = ped->GetPosition();
    ped->SetPosn(JumpMatrixTimesPoint59C890(*ped->m_matrix, CVector(0.0F, 0.0F, 0.75F)));

    // Como el original: el numero de esferas se lee con signo (movsx)
    for (int32 i = 0; i < (int16)pedColData->m_nNumSpheres; i++)
        pedColData->m_pSpheres[i].m_fRadius = 0.3F;

    if (ped->TestCollision(false))
        m_bIsJumpBlocked = true;

    ped->SetPosn(savedPedPosition);

    for (int32 i = 0; i < (int16)pedColData->m_nNumSpheres; i++)
        pedColData->m_pSpheres[i].m_fRadius = 0.35F;

    return m_bIsJumpBlocked;
}

// 0x679B80
void CTaskSimpleJump::Launch(CPed* ped) {
    // Como el original: interpolacion (b - a) * t + a
    float fHorizontalJumpSpeed = 0.1F;
    if (const auto sprintAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SPRINT)) {
        fHorizontalJumpSpeed = (0.22F - 0.17F) * sprintAnim->m_BlendAmount + 0.17F;
    } else if (const auto runAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_RUN)) {
        fHorizontalJumpSpeed = (0.17F - 0.1F) * runAnim->m_BlendAmount + 0.1F;
    }

    float fJumpForce = (ped->IsPlayer() || m_bHighJump) ? 8.5F : 4.5F;

    // Como el original: el modificador se pide dos veces
    if (ped->GetPlayerData()) {
        fHorizontalJumpSpeed = CStats::GetFatAndMuscleModifier(STAT_MOD_2) * fHorizontalJumpSpeed;
        fJumpForce = CStats::GetFatAndMuscleModifier(STAT_MOD_2) * fJumpForce;
    }

    if (CCheat::IsActive(CHEAT_MEGAJUMP) && ped->IsPlayer())
        fJumpForce *= 10.0F;

    ped->ApplyMoveForce(CVector(0.0F, 0.0F, fJumpForce));

    if (m_bClimbJump) {
        ped->m_vecMoveSpeed.x = 0.0F;
        ped->m_vecMoveSpeed.y = 0.0F;
    } else {
        // Como el original: se compara con la velocidad al cuadrado (x*x + y*y < v*v) o se exige estar sobre algo
        const float speedX = ped->m_vecMoveSpeed.x, speedY = ped->m_vecMoveSpeed.y;
        if ((fHorizontalJumpSpeed * fHorizontalJumpSpeed > speedX * speedX + speedY * speedY || ped->m_standingOnEntity) && !m_pClimbEntity) {
            // Como el original: fsin/fcos con precision interna completa, multiplicados por la velocidad
            const float rotation = ped->m_fCurrentRotation, speed = fHorizontalJumpSpeed;
            float sinSpeed, cosSpeed;
            __asm {
                fld  rotation
                fsin
                fmul speed
                fstp sinSpeed
                fld  rotation
                fcos
                fmul speed
                fstp cosSpeed
            }
            if (const auto standingOn = ped->m_standingOnEntity) {
                ped->m_vecMoveSpeed.x = standingOn->AsPhysical()->m_vecMoveSpeed.x - sinSpeed;
                ped->m_vecMoveSpeed.y = cosSpeed + standingOn->AsPhysical()->m_vecMoveSpeed.y;
            } else {
                ped->m_vecMoveSpeed.x = -sinSpeed;
                ped->m_vecMoveSpeed.y = cosSpeed;
            }
        }
    }

    ped->bIsStanding = false;
    ped->bIsInTheAir = true;

    if (!m_pClimbEntity) {
        if (m_bClimbJump) {
            auto anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_CLIMB_JUMP, 8.0F);
            anim->m_Flags |= ANIMATION_IS_FINISH_AUTO_REMOVE;
        } else
            CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, ANIM_ID_JUMP_GLIDE, 8.0F);
    }

    if (ped->bDoBloodyFootprints && CLocalisation::Blood()) {
        // Como el original: el tamano usa forward*0,26 y el lado right*0,14 de la matriz; la z es (v.z - 0,1) + fwd.z*0,2
        const auto FootPosition = [ped](ePedNode node) {
            CVector v{ 0.0F, 0.0F, 0.0F };
            const auto hier = GetAnimHierarchyFromSkinClump(ped->GetRpClump());
            const auto index = RpHAnimIDGetIndex(hier, ped->m_apBones[node]->BoneTag);
            RwV3dTransformPoints(&v, &v, 1, &RpHAnimHierarchyGetMatrixArray(hier)[index]);
            return v;
        };
        const CMatrix& mat = *ped->m_matrix;
        const CVector fwd = mat.GetForward();
        const CVector v1{ fwd.x * 0.2F, fwd.y * 0.2F, fwd.z * 0.2F };
        const float topX = fwd.x * 0.26F, topY = fwd.y * 0.26F, rightX = mat.GetRight().x * 0.14F, rightY = mat.GetRight().y * 0.14F;
        for (const auto node : { PED_NODE_LEFT_FOOT, PED_NODE_RIGHT_FOOT }) {
            CVector v = FootPosition(node);
            v.x = v1.x + v.x;
            v.y = v1.y + v.y;
            v.z = v1.z + (v.z - 0.1F);
            CShadows::AddPermanentShadow(SHADOW_DEFAULT, gpBloodPoolTex, &v, topX, topY, rightX, rightY, 255, 255, 0, 0, 4.0F, 3000, 1.0F);
        }

        // Como el original: comparacion sin signo (jbe)
        if ((uint32)ped->m_nDeathTimeMS <= 40) {
            ped->m_nDeathTimeMS = 0;
            ped->bDoBloodyFootprints = false;
        } else
            ped->m_nDeathTimeMS -= 40;
    }
}

// 0x67D7A0
bool CTaskSimpleJump::StartLaunchAnim(CPed* ped) {
    m_pAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_JUMP_LAUNCH);
    if (!m_pAnim)
        m_pAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_JUMP_LAUNCH_R);
    if (m_pAnim)
        return false;

    // Como el original: en pendiente pronunciada solo cuenta el producto escalar ((z + y) + x); la direccion permitida
    // del jugador se mira solo fuera de ella
    const CVector& fwd = ped->m_matrix->GetForward();
    bool blocked;
    if (g_surfaceInfos.IsSteepSlope(ped->m_nContactSurface)) {
        blocked = (fwd.z * ped->field_578.z + fwd.y * ped->field_578.y) + fwd.x * ped->field_578.x < 0.0F;
    } else {
        blocked = ped->IsPlayer() && !CGameLogic::IsPlayerAllowedToGoInThisDirection(ped, fwd, 5.0F);
    }
    if (blocked) {
        ped->m_pedIK.bSlopePitch = true;
        return false;
    }

    auto moveAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_SPRINT);
    if (!moveAnim || moveAnim->m_BlendAmount < 0.3F)
        moveAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_RUN);
    if (!moveAnim || moveAnim->m_BlendAmount < 0.3F)
        moveAnim = RpAnimBlendClumpGetAssociation(ped->GetRpClump(), ANIM_ID_WALK);

    float fMoveAnimBlendAmount = 0.0F;
    if (moveAnim && moveAnim->m_BlendAmount > 0.3F) {
        fMoveAnimBlendAmount = moveAnim->m_CurrentTime / moveAnim->m_BlendHier->m_fTotalTime + 0.367F;
        if (fMoveAnimBlendAmount > 1.0F)
            fMoveAnimBlendAmount -= 1.0F;
    }

    // Como el original: NaN elige la animacion derecha (solo < 0,5 elige la normal)
    m_pAnim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_DEFAULT, !(fMoveAnimBlendAmount < 0.5F) ? ANIM_ID_JUMP_LAUNCH_R : ANIM_ID_JUMP_LAUNCH, 8.0F);

    if (ped->GetPlayerData())
        m_pAnim->m_Speed = CStats::GetFatAndMuscleModifier(STAT_MOD_2);
    m_pAnim->SetFinishCallback(JumpAnimFinishCB, this);
    ped->m_fAimingRotation = ped->m_fCurrentRotation;

    return true;
}

// 0x67A020
void CTaskSimpleJump::JumpAnimFinishCB(CAnimBlendAssociation* anim, void* data) {
    auto task = reinterpret_cast<CTaskSimpleJump*>(data);
    task->m_bIsFinished = true;
    task->m_pAnim = nullptr;
}
