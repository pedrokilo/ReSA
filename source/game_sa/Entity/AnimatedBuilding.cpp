/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CAnimatedBuilding
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "AnimatedBuilding.h"

void CAnimatedBuilding::InjectHooks()
{
    RH_ScopedVirtualClass(CAnimatedBuilding, 0x8639B0, 22);
    RH_ScopedCategory("Entity");

    RH_ScopedVMTInstall(ProcessControl, 0x537A30);
    RH_ScopedVMTInstall(ProcessCollision, 0x537A40);
    RH_ScopedVMTInstall(PreRender, 0x403F00);
}

// 0x537A30
void CAnimatedBuilding::ProcessControl()
{}

// 0x537A40
void CAnimatedBuilding::ProcessCollision()
{
    SetIsInSafePosition(true);
}

// 0x403F00
void CAnimatedBuilding::PreRender()
{
    if (!GetRwObject())
        return;

    CEntity::PreRender();
    CEntity::UpdateRwFrame();
    CEntity::UpdateRpHAnim();
}
