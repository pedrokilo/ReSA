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
 * Validación registrada: 2/2 entradas.
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

    // Como el original (0x4040F2): índice = diferencia con signo de 32 bits / 0x38 (trunca hacia cero),
    // así que un puntero hasta 0x37 bytes antes del almacenamiento cuenta como ranura 0.
    const auto* const pool    = GetBuildingPool();
    const auto* const storage = *reinterpret_cast<const uint8* const*>(pool);                          // CPool::m_pObjects (+0x0)
    const auto* const flags   = *reinterpret_cast<const int8* const*>(reinterpret_cast<const uint8*>(pool) + 0x4); // CPool::m_byteMap (+0x4)
    const int32 size          = *reinterpret_cast<const int32*>(reinterpret_cast<const uint8*>(pool) + 0x8);       // CPool::m_nSize (+0x8)
    const int32 idx = int32(reinterpret_cast<uintptr_t>(building) - reinterpret_cast<uintptr_t>(storage)) / int32(sizeof(CBuilding));
    return idx >= 0 && idx < size && flags[idx] >= 0;
}
