/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CObjectSaveStructure
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "ObjectSaveStructure.h"

void CObjectSaveStructure::InjectHooks()
{
    RH_ScopedClass(CObjectSaveStructure);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Construct, 0x5D2160);
    RH_ScopedInstall(Extract, 0x5D2220);
}

void CObjectSaveStructure::Construct(CObject* obj)
{
    m_matrix.CompressFromFullMatrix(*obj->m_matrix);
    m_nBonusValue  = obj->m_nBonusValue;
    m_nCostValue   = obj->m_wCostValue;
    m_nRemovalTime = obj->m_nRemovalTime;
    m_nEntityFlags = obj->m_nFlags;
    m_nObjectFlags = obj->m_nObjectFlags;
    m_nObjectType  = obj->m_nObjectType;
    m_nColDamageEffect = obj->m_nColDamageEffect;
    m_nSpecialColResponseCase = obj->m_nSpecialColResponseCase;
    m_nUnused = 0;

    m_nPhysicalFlags = 0;

    if (obj->physicalFlags.bDisableCollisionForce)
        m_bDisableCollisionForce = true;

    if (obj->physicalFlags.bDontApplySpeed)
        m_bDontApplySpeed = true;

    if (obj->physicalFlags.bBulletProof)
        m_bBulletProof = true;

    if (obj->physicalFlags.bFireProof)
        m_bFireProof = true;

    if (obj->physicalFlags.bCollisionProof)
        m_bCollisionProof = true;

    if (obj->physicalFlags.bMeleeProof)
        m_bMeleeProof = true;

    if (obj->physicalFlags.bExplosionProof)
        m_bExplosionProof = true;
}

void CObjectSaveStructure::Extract(CObject* obj)
{
    m_matrix.DecompressIntoFullMatrix(*obj->m_matrix);
    obj->m_nBonusValue = m_nBonusValue;
    obj->m_wCostValue = m_nCostValue;
    obj->m_nRemovalTime = m_nRemovalTime;
    obj->m_nFlags = m_nEntityFlags;
    obj->m_nObjectFlags = m_nObjectFlags;
    obj->m_nObjectType = m_nObjectType;
    obj->m_nColDamageEffect = m_nColDamageEffect;
    obj->m_nSpecialColResponseCase = m_nSpecialColResponseCase;

    obj->physicalFlags.bDisableCollisionForce = m_bDisableCollisionForce;
    obj->physicalFlags.bDontApplySpeed = m_bDontApplySpeed;
    obj->physicalFlags.bBulletProof = m_bBulletProof;
    obj->physicalFlags.bFireProof = m_bFireProof;
    obj->physicalFlags.bCollisionProof = m_bCollisionProof;
    obj->physicalFlags.bMeleeProof = m_bMeleeProof;
    obj->physicalFlags.bExplosionProof = m_bExplosionProof;
}
