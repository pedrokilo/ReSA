/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CVehicleSaveStructure
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "VehicleSaveStructure.h"

namespace {
// 0x5D28F0 (lote 192A, sin validar; el listado decia CObject: es el destructor de CVehicleSaveStructure (la estructura local de CVehicle::Load, 0x5D2900, y CVehicle::Save), cuyo primer miembro (+0x0) es un CMatrix; thiscall, sin argumentos de pila,
// 5 bytes: `jmp 0x59ACD0` (cuerpo del destructor de CMatrix); lo alcanza el EXE desde el desenrollado de CVehicle::Load/Save (0x83D5D3 y 0x83D6F3). Como el original: destruye la matriz con el cuerpo de CMatrix del EXE (0x59ACD0, con hook:
// CMatrix::Destructor; la bandera de propiedad se lee como dword y se comprueba antes que la matriz enlazada). Se engancha como S.
__declspec(noinline) void __fastcall VehicleSaveStructureDestructor(CVehicleSaveStructure* self, void* /*edx*/) {
    static_assert(offsetof(CVehicleSaveStructure, m_matrix) == 0);
    self->m_matrix.Destructor();
}
} // namespace

void CVehicleSaveStructure::InjectHooks() {
    RH_ScopedClass(CVehicleSaveStructure);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Construct, 0x5D2330);
    RH_ScopedInstall(Extract, 0x5D23B0);
    RH_ScopedNamedGlobalInstall(VehicleSaveStructureDestructor, "Destructor", 0x5D28F0); // lote 192A (sin validar)
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
