/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CProjectile
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

#include "Projectile.h"

void CProjectile::InjectHooks() {
    RH_ScopedClass(CProjectile);
    RH_ScopedCategory("Entity/Object");

    RH_ScopedInstall(Constructor, 0x5A4030);
}

// 0x5A4030
CProjectile::CProjectile(int32 modelId) : CObject() {
    m_bTunnelTransition = true;
    m_fMass = 1.0F;
    m_fTurnMass = 1.0F;
    m_fAirResistance = 0.99999F;
    m_fElasticity = 0.75F;
    m_fBuoyancyConstant = std::bit_cast<float>(0x3A51B718u); // 0x3A51B718 (0.00080000004F) del EXE; 1.0F / 1250.0F da 0x3A51B717
    CObject::SetIsStatic(false);
    physicalFlags.bExplosionProof = true;
    CEntity::SetModelIndex(modelId);
    m_nObjectType = eObjectType::OBJECT_TYPE_DECORATION;
}
