/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedAttractorPedPlacer
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include <StdInc.h>
#include <Plugins/TwoDEffectPlugin/2dEffect.h>

#include "PedPlacement.h"
#include "PedAttractorPedPlacer.h"


void CPedAttractorPedPlacer::InjectHooks() {
    RH_ScopedClass(CPedAttractorPedPlacer);
    RH_ScopedCategory("Attractors");

    RH_ScopedGlobalInstall(PlacePedAtEffect, 0x5EA390);
}

// Como el original: CVector::FromMultiply (0x59C670), con el orden de sumas del EXE
static CVector TransformPointOG(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x + m.GetPosition().x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y + m.GetPosition().y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y + m.GetPosition().z,
    };
}

// Como el original: Multiply3x3 (0x59C790), con el orden de sumas del EXE
static CVector TransformVectorOG(const CMatrix& m, const CVector& v) {
    return {
        m.GetUp().x * v.z + m.GetForward().x * v.y + m.GetRight().x * v.x,
        m.GetUp().y * v.z + m.GetRight().y * v.x + m.GetForward().y * v.y,
        m.GetUp().z * v.z + m.GetRight().z * v.x + m.GetForward().z * v.y,
    };
}

// 0x5EA390
void CPedAttractorPedPlacer::PlacePedAtEffect(C2dEffectPedAttractor const& fx, CEntity* entity, CPed* ped, float forwardOffsetMultiplier) {
    // Como el original: matriz local construida por defecto; identidad (0x59AE70) o copia (0x59BBC0) de la del entity
    CMatrix mat;
    if (entity) {
        mat = entity->GetMatrix();
    } else {
        mat.SetUnity();
    }

    const auto fxPos    = TransformPointOG(mat, fx.m_Pos);
    const auto fxUseDir = TransformVectorOG(mat, fx.m_vecUseDir);
    const auto fxFwdDir = TransformVectorOG(mat, fx.m_vecForwardDir);
    // (El original también calcula la dirección de cola con Multiply3x3, sin usarla.)

    const CVector offset{ fxFwdDir.x * forwardOffsetMultiplier, fxFwdDir.y * forwardOffsetMultiplier, fxFwdDir.z * forwardOffsetMultiplier };
    ped->SetPosn(std::get<CVector>(CPedPlacement::FindZCoorForPed(CVector{ fxPos.x + offset.x, fxPos.y + offset.y, fxPos.z + offset.z })));

    // Como el original: SetHeading en línea; las rotaciones se escriben antes de girar la matriz (sin crearla si no existe)
    const float heading = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(fxUseDir.x, fxUseDir.y, 0.0f, 0.0f));
    ped->m_fAimingRotation  = heading;
    ped->m_fCurrentRotation = heading;
    if (ped->m_matrix) {
        ped->m_matrix->SetRotateZOnly(heading);
    } else {
        ped->m_placement.m_fHeading = heading;
    }

    // Como el original: el ángulo se calcula de nuevo y se vuelve a escribir en ambas rotaciones
    const float heading2 = CGeneral::LimitRadianAngle(CGeneral::GetRadianAngleBetweenPoints(fxUseDir.x, fxUseDir.y, 0.0f, 0.0f));
    ped->m_fAimingRotation  = heading2;
    ped->m_fCurrentRotation = heading2;
}
