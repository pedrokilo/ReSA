/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntitySeekPosCalculatorRadiusAngleOffset
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EntitySeekPosCalculatorRadiusAngleOffset.h"
#include "PedGeometryAnalyser.h"

void CEntitySeekPosCalculatorRadiusAngleOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorRadiusAngleOffset, 0x85A384, 1); // Como el original: vtable de una sola entrada
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946f0);
}

CEntitySeekPosCalculatorRadiusAngleOffset::CEntitySeekPosCalculatorRadiusAngleOffset(float radius, float angle) :
    m_radius{ radius },
    m_angle{ angle }
{
}

// 0x6946F0
void CEntitySeekPosCalculatorRadiusAngleOffset::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    // Como el original: el forward sale de la matriz o, sin ella, de (-fsin, fcos) del heading (x87, redondeado a float)
    float fwdX, fwdY;
    if (const auto* const mat = target.m_matrix) {
        fwdX = mat->GetForward().x;
        fwdY = mat->GetForward().y;
    } else {
        const float headingArg = target.m_placement.m_fHeading;
        __asm {
            fld  headingArg
            fsin
            fchs
            fstp fwdX
            fld  headingArg
            fcos
            fstp fwdY
        }
    }
    const float heading = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(fwdX, fwdY, 0.0f, 0.0f));
    const float angleArg = CGeneral::LimitRadianAngle(heading + m_angle);

    // Como el original: -fsin(a) se multiplica por el radio con precisión interna completa; fcos se guarda antes como float
    const float radiusArg = m_radius;
    float offX, cosA;
    __asm {
        fld  angleArg
        fsin
        fchs
        fmul radiusArg
        fstp offX
        fld  angleArg
        fcos
        fstp cosA
    }
    const float offY = radiusArg * cosA;
    const float offZ = radiusArg * 0.0f; // Como el original: radio * 0.0 (0x858B50), no un 0 literal

    const auto& pos = target.GetPosition();
    const CVector seekPos{ offX + pos.x, offY + pos.y, offZ + pos.z };
    CPedGeometryAnalyser::ComputeClearTarget(seeker, seekPos, outPos);
}
