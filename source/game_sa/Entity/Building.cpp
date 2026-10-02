/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CBuilding
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

#include "Building.h"

/**
 * Registra las operaciones específicas de edificios estáticos y su pool.
 */
void CBuilding::InjectHooks()
{
    RH_ScopedVirtualClass(CBuilding, 0x8585C8, 22);
    RH_ScopedCategory("Entity");

    RH_ScopedInstall(ReplaceWithNewModel, 0x403EC0);
    RH_ScopedGlobalInstall(IsBuildingPointerValid, 0x4040E0);
}

CBuilding::CBuilding() : CEntity()
{
    SetTypeBuilding();
    SetUsesCollision(true);
}

// Los edificios se asignan exclusivamente desde CBuildingPool.
void* CBuilding::operator new(unsigned size)
{
    return GetBuildingPool()->New();
}

void CBuilding::operator delete(void* data)
{
    GetBuildingPool()->Delete(static_cast<CBuilding*>(data));
}

/**
 * Elimina el recurso RenderWare actual y cambia el model index.
 *
 * Si el modelo anterior ya no tiene referencias solicita a streaming que libere
 * también sus recursos compartidos.
 */
void CBuilding::ReplaceWithNewModel(int32 newModelIndex)
{
    DeleteRwObject();
    if (!CModelInfo::GetModelInfo(GetModelIndex())->m_nRefCount)
        CStreaming::RemoveModel(GetModelIndex());

    m_nModelIndex = newModelIndex;
}

bool IsBuildingPointerValid(CBuilding* building)
{
    if (!building)
        return false;

    return GetBuildingPool()->IsObjectValid(building);
}
