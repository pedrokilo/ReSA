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
 * Validación registrada: 0/1 entradas.
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

// 0x5EA390
void CPedAttractorPedPlacer::PlacePedAtEffect(C2dEffectPedAttractor const& fx, CEntity* entity, CPed* ped, float forwardOffsetMultiplier) {
    const auto mat = entity
        ? entity->GetMatrix()
        : CMatrix::Unity();

    const auto fxPos      = CPedAttractorManager::ComputeEffectPos(&fx, mat);
    const auto fxUseDir   = CPedAttractorManager::ComputeEffectUseDir(&fx, mat);
    const auto fxFwdDir   = CPedAttractorManager::ComputeEffectForwardDir(&fx, mat);
    //const auto fxQueueDir = CPedAttractorManager::ComputeEffectQueueDir(&fx, mat); // NOTSA: Actually unused

    ped->SetPosn(std::get<CVector>(CPedPlacement::FindZCoorForPed(fxPos + fxFwdDir * forwardOffsetMultiplier)));

    const auto angle = CVector2D{ fxUseDir }.Heading();
    ped->GetMatrix().SetRotateZOnly(angle);
    ped->m_fAimingRotation = angle;
    ped->m_fCurrentRotation = angle;
}
