/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehicleSaveStructure
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

#include "VehicleSaveStructure.h"

void CVehicleSaveStructure::InjectHooks() {
    RH_ScopedClass(CVehicleSaveStructure);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Construct, 0x5D2330);
    RH_ScopedInstall(Extract, 0x5D23B0);
}

// 0x5D2330
void CVehicleSaveStructure::Construct(CVehicle* v) {
    m_matrix = *v->m_matrix;
    m_nPrimaryColor = v->m_nPrimaryColor;
    m_nSecondaryColor = v->m_nSecondaryColor;
    m_nTertiaryColor = v->m_nTertiaryColor;
    m_nQuaternaryColor = v->m_nQuaternaryColor;
    m_nAlarmState = v->m_nAlarmState;
    m_nMaxPassengers = v->m_nMaxPassengers;
    m_fSteerAngle = v->m_fSteerAngle;
    m_fGasPedal = v->m_GasPedal;
    m_fBrakePedal = v->m_BrakePedal;
    m_nVehicleUpperFlags = v->m_nVehicleUpperFlags;
    m_nVehicleLowerFlags = v->m_nVehicleLowerFlags;
}

// 0x5D23B0
void CVehicleSaveStructure::Extract(CVehicle* v) {
    v->SetMatrix(m_matrix);
    v->m_nPrimaryColor = m_nPrimaryColor;
    v->m_nSecondaryColor = m_nSecondaryColor;
    v->m_nTertiaryColor = m_nTertiaryColor;
    v->m_nQuaternaryColor = m_nQuaternaryColor;
    v->m_nAlarmState = m_nAlarmState;
    v->m_nMaxPassengers = m_nMaxPassengers;
    v->m_fSteerAngle = m_fSteerAngle;
    v->m_GasPedal = m_fGasPedal;
    v->m_BrakePedal = m_fBrakePedal;
    v->m_nVehicleUpperFlags = m_nVehicleUpperFlags;
    v->m_nVehicleLowerFlags = m_nVehicleLowerFlags;
}
