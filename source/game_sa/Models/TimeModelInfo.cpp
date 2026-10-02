/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTimeModelInfo
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

#include "ModelInfo.h"

/**
 * Registra las operaciones específicas de los modelos con visibilidad por horario.
 */
void CTimeModelInfo::InjectHooks() {
    RH_ScopedVirtualClass(CTimeModelInfo, 0x85BCB0, 16);
    RH_ScopedCategory("Models");

    RH_ScopedVMTInstall(GetModelType, 0x4C5660);
    RH_ScopedVMTInstall(GetTimeInfo, 0x4C5670);
}

// 0x4C5660
ModelInfoType CTimeModelInfo::GetModelType() {
    return ModelInfoType::MODEL_INFO_TIME;
}

// 0x4C5670
// Expone los metadatos horarios embebidos para que CBaseModelInfo pueda tratar pares día/noche.
CTimeInfo* CTimeModelInfo::GetTimeInfo() {
    return &m_timeInfo;
}

