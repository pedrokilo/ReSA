/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CLodTimeModelInfo
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

#include "LodTimeModelInfo.h"

void CLodTimeModelInfo::InjectHooks()
{
    RH_ScopedVirtualClass(CLodTimeModelInfo, 0x85BCF0, 16);
    RH_ScopedCategory("Models");

    // Las direcciones anteriores (0x4C5660/0x4C5670) eran las de CTimeModelInfo; las
    // propias de esta clase están en las entradas [4] y [5] de su vtable 0x85BCF0.
    RH_ScopedVMTInstall(GetModelType, 0x4C56C0);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C56D0);
}

// 0x4C56C0
ModelInfoType CLodTimeModelInfo::GetModelType()
{
    return ModelInfoType::MODEL_INFO_TIME;
}

// 0x4C56D0
CTimeInfo* CLodTimeModelInfo::GetTimeInfo()
{
    return &m_lodTimeInfo;
}