/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBouncingPanel
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "BouncingPanel.h"
#include "X87Math.h"

void CBouncingPanel::InjectHooks() {
    RH_ScopedClass(CBouncingPanel);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ResetPanel, 0x6F4910);
    RH_ScopedInstall(SetPanel, 0x6F4920);
    RH_ScopedInstall(ProcessPanel, 0x6F49A0);
}

// 0x6F4910
void CBouncingPanel::ResetPanel() {
    m_nFrameId = -1;
}

// 0x6F4920
void CBouncingPanel::SetPanel(int16 frameId, int16 axis, float angleLimit) {
    m_nFrameId = frameId;
    m_nAxis = axis;
    m_fAngleLimit = angleLimit;
    m_vecRotation = CVector();
    m_vecPos = CVector();
}

float CBouncingPanel::GetAngleChange(float velocity) const {
    return std::clamp(velocity, -BOUNCE_VEL_CHANGE_LIMIT, BOUNCE_VEL_CHANGE_LIMIT) * m_fAngleLimit;
}

namespace {
// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE
CVector Multiply3x3_MV_59C790(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// Como el original: Multiply3x3 (0x59C810), vector por matriz, con el orden de sumas del EXE
CVector Multiply3x3_VM_59C810(const CVector& v, const CMatrix& m) {
    return {
        m.GetRight().y * v.y + m.GetRight().z * v.z + v.x * m.GetRight().x,
        m.GetForward().y * v.y + m.GetForward().x * v.x + m.GetForward().z * v.z,
        m.GetUp().y * v.y + m.GetUp().x * v.x + m.GetUp().z * v.z,
    };
}

// Como el original: pos *= pow(damp, timestep); el factor se usa con la precision completa de ST0
// en las tres multiplicaciones (X87Math.cpp)
void MultiplyByPow(CVector& pos, float damp, float step) {
    PowScaleVector3Cpp(&pos.x, damp, step);
}
} // namespace

// 0x6F49A0
void CBouncingPanel::ProcessPanel(CVehicle* vehicle, RwFrame* frame, CVector moveForce, CVector turnForce, float fReturnMultiplier, float fDampMultiplier) {
    CMatrix  frameMat{RwFrameGetMatrix(frame)};
    const CMatrix& vehMat = *vehicle->m_matrix; // Como el original: lee el puntero de la matriz sin reservarla
    CVector  compPos = Multiply3x3_MV_59C790(vehMat, frameMat.GetPosition());
    const CVector speed = vehicle->GetSpeed(compPos);
    // Como el original: speed - (moveForce + turnForce x compPos)
    const CVector cross = CrossProduct(turnForce, compPos);
    const CVector sum{ moveForce.x + cross.x, moveForce.y + cross.y, moveForce.z + cross.z };
    CVector  angularVelocity = Multiply3x3_VM_59C810(CVector{ speed.x - sum.x, speed.y - sum.y, speed.z - sum.z }, vehMat);

    switch (m_nAxis) {
    case 0: {
        m_vecPos.y += GetAngleChange(angularVelocity.z);
        m_vecPos.z += GetAngleChange(angularVelocity.y);
        break;
    }
    case 1: {
        m_vecPos.x += GetAngleChange(angularVelocity.z);
        m_vecPos.z += GetAngleChange(angularVelocity.y);
        break;
    }
    case 2: {
        m_vecPos.x += GetAngleChange(angularVelocity.y);
        m_vecPos.y += GetAngleChange(angularVelocity.x);
        break;
    }
    }

    const bool areMultsSet = fReturnMultiplier > -1.0f && fDampMultiplier > -1.0f;
    {
        const float rtnMult = areMultsSet
            ? fReturnMultiplier
            : m_nAxis == 2 ? BOUNCE_HANGING_RETURN_MULT : BOUNCE_SPRING_RETURN_MULT;

        m_vecPos -= rtnMult * m_vecRotation * CTimer::ms_fTimeStep;
    }
    {
        const float dampMult = areMultsSet
            ? fDampMultiplier
            : m_nAxis == 2 ? BOUNCE_HANGING_DAMP_MULT : BOUNCE_SPRING_DAMP_MULT;

        MultiplyByPow(m_vecPos, dampMult, CTimer::ms_fTimeStep);
    }
    m_vecRotation += m_vecPos * CTimer::ms_fTimeStep;

    switch (m_nAxis) {
    case 0: {
        CVector& right = frameMat.GetRight();
        right.y = m_vecRotation.z;
        right.z = m_vecRotation.y;
        break;
    }
    case 1: {
        CVector& fwd = frameMat.GetForward();
        fwd.x = m_vecRotation.z;
        fwd.z = m_vecRotation.x;
        break;
    }
    case 2: {
        CVector& up = frameMat.GetUp();
        up.y = m_vecRotation.x;
        up.x = m_vecRotation.y;
        break;
    }
    }

    frameMat.UpdateRW();
}
