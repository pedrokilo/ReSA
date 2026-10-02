/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventPlayerCollisionWithPed
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "EventPlayerCollisionWithPed.h"


void CEventPlayerCollisionWithPed::InjectHooks()
{
    RH_ScopedVirtualClass(CEventPlayerCollisionWithPed, 0x86C9B0, 16);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x5FEE40);
}

// 0x5FEE40
CEventPlayerCollisionWithPed::CEventPlayerCollisionWithPed(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState) :
    CEventPedCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState)
{
}

// 0x5FEE40
CEventPlayerCollisionWithPed* CEventPlayerCollisionWithPed::Constructor(int16 pieceType, float damageIntensity, CPed* victim, const CVector& collisionImpactVelocity, const CVector& collisionPos, eMoveState moveState, eMoveState victimMoveState)
{
    this->CEventPlayerCollisionWithPed::CEventPlayerCollisionWithPed(pieceType, damageIntensity, victim, collisionImpactVelocity, collisionPos, moveState, victimMoveState);
    return this;
}

