/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLodAtomicModelInfo
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

#include "LodAtomicModelInfo.h"

/**
 * Registra las operaciones del model info especializado para LODs.
 */
void CLodAtomicModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CLodAtomicModelInfo, 0x85BC70, 16);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(AsLodAtomicModelInfoPtr, 0x4C5610);
    RH_ScopedVMTInstall(GetModelType, 0x4C5600);
    RH_ScopedVMTInstall(Init, 0x4C54D0);
}

// Inicializa los contadores usados para coordinar hijos LOD y los ya renderizados.
CLodAtomicModelInfo::CLodAtomicModelInfo() : CAtomicModelInfo()
{
    m_numChildren = 0;
    m_numChildrenRendered = 0;
}

// 0x4C5610
CLodAtomicModelInfo* CLodAtomicModelInfo::AsLodAtomicModelInfoPtr()
{
    return this;
}

// 0x4C5600
ModelInfoType CLodAtomicModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_LOD;
}

// 0x4C54D0
void CLodAtomicModelInfo::Init()
{
    CAtomicModelInfo::Init();
}