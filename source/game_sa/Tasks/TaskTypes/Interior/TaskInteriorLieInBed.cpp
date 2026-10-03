/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorLieInBed
 * Reconstrucción: COMPLETA — 7/7 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 7/7 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskInteriorLieInBed.h"

#include "Interior/InteriorInfo_t.h"
#include "Interior/Interior_c.h"
#include "Interior/InteriorManager_c.h"

void CTaskInteriorLieInBed::InjectHooks() {
    RH_ScopedVirtualClass(CTaskInteriorLieInBed, 0x870338, 9);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x675E20);
    RH_ScopedInstall(Destructor, 0x675E90);

    RH_ScopedInstall(FinishAnimCB, 0x675FC0);

    RH_ScopedVMTInstall(Clone, 0x675EF0);
    RH_ScopedVMTInstall(GetTaskType, 0x675E80);
    RH_ScopedVMTInstall(MakeAbortable, 0x675F60);
    RH_ScopedVMTInstall(ProcessPed, 0x6772E0);
}

// 0x675E20
CTaskInteriorLieInBed::CTaskInteriorLieInBed(
    int32 duration,
    InteriorInfo_t* intInfo,
    bool rightHandSide,
    bool doInstantly
) :
    m_IntInfo{intInfo},
    m_GetOutAfterInterval{duration},
    m_bDoInstantly{doInstantly},
    m_BaseAnimId{ rightHandSide ? ANIM_ID_BED_IN_R : ANIM_ID_BED_IN_L },

#ifdef FIX_BUGS
    m_bRghtHandSide{rightHandSide}
#endif
{
}

// For `0x675EF0`
// Como el original: Clone construye con los campos del original (duración, info, lado derecho, instantáneo)
CTaskInteriorLieInBed::CTaskInteriorLieInBed(const CTaskInteriorLieInBed& o) :
    CTaskInteriorLieInBed{
        o.m_GetOutAfterInterval,
        o.m_IntInfo,
        o.m_bRghtHandSide,
        o.m_bDoInstantly
    }
{
}

// 0x675E90
CTaskInteriorLieInBed::~CTaskInteriorLieInBed() {
    if (m_Anim) {
        m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); // Como el original: callback de borrado
    }
}

// 0x675FC0
void CTaskInteriorLieInBed::FinishAnimCB(CAnimBlendAssociation* anim, void* data) {
    // Como el original: sin comprobar el tipo de la tarea; dos comprobaciones independientes
    const auto self = static_cast<CTaskInteriorLieInBed*>(static_cast<CTask*>(data));

    self->m_PrevAnimId = anim->GetAnimId();

    if ((int32)self->m_PrevAnimId == (int32)self->m_BaseAnimId + 2) { // Last animation in the sequence
        anim->SetBlendDelta(-1000.f);
        self->m_LastAnimFinished = true;
    }
    if (self->m_TaskAborting && (int32)self->m_PrevAnimId == (int32)self->m_BaseAnimId) {
        anim->SetBlendDelta(-1000.f);
        self->m_LastAnimFinished = true;
    }

    self->m_Anim = nullptr;
}

// 0x675F60
bool CTaskInteriorLieInBed::MakeAbortable(CPed* ped, eAbortPriority priority, CEvent const* event) {
    if (priority == ABORT_PRIORITY_IMMEDIATE) {
        if (m_Anim) {
            m_Anim->SetBlendDelta(-1000.f);
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr); // Como el original: callback de borrado
            m_Anim = nullptr;
        }
        ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().SetOnlyScriptPedAllowed();
        return true;
    } else {
        m_TaskAborting = true;
        return false;
    }
}

namespace {
// Como el original: operator*(CMatrix, CVector) 0x59C890 con su orden de sumas (solo x e y)
CVector2D LieInBedTransformPointXY(const CMatrix& m, const CVector& v) {
    return {
        ((m.GetUp().x * v.z + m.GetForward().x * v.y) + m.GetRight().x * v.x) + m.GetPosition().x,
        ((m.GetUp().y * v.z + m.GetRight().y * v.x) + m.GetForward().y * v.y) + m.GetPosition().y,
    };
}
} // namespace

// 0x6772E0
bool CTaskInteriorLieInBed::ProcessPed(CPed* ped) {
    const auto currAnimId = m_Anim
        ? (int32)m_Anim->GetAnimId()
        : (int32)ANIM_ID_UNDEFINED;
    const auto baseAnimId = (int32)m_BaseAnimId;

    ped->SetMoveState(PEDMOVE_STILL);

    if (m_LastAnimFinished) {
        if (!RpAnimBlendClumpGetAssociation(ped->GetRpClump(), (AnimationId)(baseAnimId + 2))) { // Check if last anim has really finished
            ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().SetOnlyScriptPedAllowed();
            return true;
        }
    }

    // Como el original: la nueva animación siempre es del grupo INT_HOUSE y lleva FinishAnimCB
    const auto BlendNextAnim = [&, this](int32 animId, float blendDelta) {
        m_Anim = CAnimManager::BlendAnimation(ped->GetRpClump(), ANIM_GROUP_INT_HOUSE, (AnimationId)animId, blendDelta);
        m_Anim->SetFinishCallback(FinishAnimCB, this);
    };

    if (m_TaskAborting) {
        if (!g_interiorMan.AreAnimsLoaded(ANIM_GROUP_DEFAULT)) {
            ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().SetOnlyScriptPedAllowed();
            return true;
        }

        if (currAnimId == baseAnimId) {
            m_Anim->SetBlendDelta(-8.f);
        } else if (currAnimId == baseAnimId + 1) {
            if (!m_UpdatePedPos) {
                m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
                BlendNextAnim(baseAnimId + 2, 1000.f);
                m_UpdatePedPos = true;
                return false;
            }
        } else if (currAnimId == baseAnimId + 2) {
            m_Anim->SetSpeed(3.f); // Como el original: cambia la velocidad, no el delta de mezcla
        }
    }

    if (!m_Anim) { // Create animation
        if (!g_interiorMan.AreAnimsLoaded(ANIM_GROUP_DEFAULT)) {
            return false;
        }
        if (m_PrevAnimId == ANIM_ID_UNDEFINED) {
            ped->GetIntelligence()->GetEventScanner().GetAcquaintanceScanner().TurnOffAllScanners();
            if (m_bDoInstantly) {
                m_GetOutTimer.Start(m_GetOutAfterInterval);
                BlendNextAnim(baseAnimId + 1, 1000.f);
                m_UpdatePedPos = true;
            } else {
                // Como el original: sin temporizador ni actualización de posición al entrar en la cama
                BlendNextAnim(baseAnimId, 4.f);
            }
        } else if ((int32)m_PrevAnimId == baseAnimId) {
            m_GetOutTimer.Start(m_GetOutAfterInterval);
            BlendNextAnim(baseAnimId + 1, 1000.f);
            m_UpdatePedPos = true;
        }
        return false;
    }

    if (m_UpdatePedPos) {
        const auto& pedPos = ped->GetPosition();
        CVector2D   posXY{ pedPos.x, pedPos.y };
        const float posZ = pedPos.z;
        switch (currAnimId) {
        case ANIM_ID_BED_LOOP_L:
        case ANIM_ID_BED_OUT_L:
            posXY = LieInBedTransformPointXY(*ped->m_matrix, CCarEnterExit::ms_vecPedBedLAnimOffset);
            break;
        case ANIM_ID_BED_LOOP_R:
        case ANIM_ID_BED_OUT_R:
            posXY = LieInBedTransformPointXY(*ped->m_matrix, CCarEnterExit::ms_vecPedBedRAnimOffset);
            break;
        default: // Como el original: conserva la posición
            break;
        }
        ped->SetPosn(CVector{ posXY.x, posXY.y, posZ });
        m_UpdatePedPos = false; // Como el original: solo una vez por animación
        if (currAnimId == baseAnimId + 1) {
            const float heading = CGeneral::LimitRadianAngle(ped->m_fCurrentRotation + 3.1415927f);
            ped->m_fCurrentRotation = heading;
            ped->m_fAimingRotation  = heading;
            ped->SetHeading(heading);
        }
    }

    if (m_GetOutTimer.IsOutOfTime()) {
        if ((int32)m_Anim->GetAnimId() != baseAnimId + 2) {
            m_Anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
            BlendNextAnim(baseAnimId + 2, 1000.f);
            m_UpdatePedPos = true;
        }
    }

    // Como el original: el desplazamiento solo se aplica durante la animación de entrada
    if ((int32)m_Anim->GetAnimId() != baseAnimId) {
        return false;
    }

    const CVector intPos = m_IntInfo->Pos;
    const auto&   pedPos = ped->GetPosition();
    const float   dx     = intPos.x - pedPos.x;
    const float   dy     = intPos.y - pedPos.y;
    const float   dz     = intPos.z - pedPos.z;
    const float   mag    = std::sqrt(dz * dz + dy * dy + dx * dx);
    const float   shift  = mag < 0.02f ? mag : 0.02f; // Como el original: NaN usa 0.02
    const float   inv    = 1.0f / mag;
    const float   sx     = dx * inv * shift;
    const float   sy     = dy * inv * shift;
    const float   sz     = dz * inv * shift;

    // Now transform the point into ped's object space from world space
    const auto& mat = *ped->m_matrix;
    ped->m_vecAnimMovingShiftLocal.x = (mat.GetRight().z * sz + mat.GetRight().y * sy) + sx * mat.GetRight().x;
    ped->m_vecAnimMovingShiftLocal.y = (mat.GetForward().z * sz + mat.GetForward().y * sy) + sx * mat.GetForward().x;

    ped->m_fAimingRotation = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(m_IntInfo->Dir.x, m_IntInfo->Dir.y, 0.f, 0.f));

    return false;
}

// NOTSA
AnimationId CTaskInteriorLieInBed::GetAnimIdInSeq(AnimSeqIdx sequenceIdx) {
    using enum AnimSeqIdx;

    switch (m_BaseAnimId) {
    case ANIM_ID_BED_IN_R:
        switch (sequenceIdx) {
        case GET_IN:  return ANIM_ID_BED_IN_R;
        case LOOP:    return ANIM_ID_BED_LOOP_R;
        case GET_OUT: return ANIM_ID_BED_OUT_R;
        default: NOTSA_UNREACHABLE();
        }
    case ANIM_ID_BED_IN_L:
        switch (sequenceIdx) {
        case GET_IN:  return ANIM_ID_BED_IN_L;
        case LOOP:    return ANIM_ID_BED_LOOP_L;
        case GET_OUT: return ANIM_ID_BED_OUT_L;
        default: NOTSA_UNREACHABLE();
        }
    default:
        NOTSA_UNREACHABLE();
    }
}
