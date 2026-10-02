/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexGoToPointShooting
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TaskComplexGoToPointShooting.h"

void CTaskComplexGoToPointShooting::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexGoToPointShooting, 0x86fe2c, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedInstall(Constructor, 0x668C70);

    RH_ScopedVMTInstall(Clone, 0x66CE10);
    RH_ScopedVMTInstall(GetTaskType, 0x668CC0);
}

// 0x668C70
CTaskComplexGoToPointShooting::CTaskComplexGoToPointShooting(
    eMoveState moveState,
    const CVector& movePos,
    CEntity* aimAtEntity,
    CVector aimPos,
    const float targetRadius,
    const float slowDownDist
) :
    CTaskComplexGoToPointAiming{
        moveState,
        movePos,
        aimAtEntity,
        aimPos,
        targetRadius,
        slowDownDist
    }
{
    // NOP
}

CTaskComplexGoToPointShooting::CTaskComplexGoToPointShooting(const CTaskComplexGoToPointShooting& o) :
    CTaskComplexGoToPointAiming{o}
{
}

// 0x66CE10: the original calls the base constructor, then installs Shooting's vtable.
CTask* CTaskComplexGoToPointShooting::Clone() const {
    return new CTaskComplexGoToPointShooting{*this};
}

// 0x668CC0
eTaskType CTaskComplexGoToPointShooting::GetTaskType() const {
    return Type;
}
