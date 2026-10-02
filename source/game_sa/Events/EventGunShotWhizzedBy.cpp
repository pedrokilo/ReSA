/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEventGunShotWhizzedBy
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
#include "EventGunShotWhizzedBy.h"


void CEventGunShotWhizzedBy::InjectHooks()
{
    RH_ScopedVirtualClass(CEventGunShotWhizzedBy, 0x85B730, 17);
    RH_ScopedCategory("Events");

    RH_ScopedInstall(Constructor, 0x4B1FD0);
    RH_ScopedVMTInstall(AffectsPed, 0x4B5120);
}

// 0x4B1FD0
CEventGunShotWhizzedBy::CEventGunShotWhizzedBy(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound) :
    CEventGunShot(entity, startPoint, endPoint, bHasNoSound)
{
}

// 0x4B1FD0
CEventGunShotWhizzedBy* CEventGunShotWhizzedBy::Constructor(CEntity* entity, const CVector& startPoint, const CVector& endPoint, bool bHasNoSound)
{
    this->CEventGunShotWhizzedBy::CEventGunShotWhizzedBy(entity, startPoint, endPoint, bHasNoSound);
    return this;
}

// 0x4B5120
bool CEventGunShotWhizzedBy::AffectsPed(CPed* ped)
{
    if (!ped->IsPlayer() && ped->IsAlive()) {
        CVector direction = m_endPoint - m_startPoint;
        direction.Normalise();
        CVector distance = ped->GetPosition() - m_startPoint;
        float squaredMagnitude = (distance * direction).SquaredMagnitude();
        if (squaredMagnitude > 0.0f) {
            CVector2D point = (direction * squaredMagnitude) + m_startPoint;
            return 2.0f * 2.0f > (ped->GetPosition() - point).SquaredMagnitude();
        }
    }
    return false;
}