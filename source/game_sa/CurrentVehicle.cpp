/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCurrentVehicle
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Evidencia aprobada vinculada: 0/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "CurrentVehicle.h"
#include "Hud.h"

void CCurrentVehicle::InjectHooks() {
    RH_ScopedClass(CCurrentVehicle);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Display, 0x571EA0);
    RH_ScopedInstall(Process, 0x572040);
}

CCurrentVehicle::CCurrentVehicle() {
    Init();
}

// 0x572030
void CCurrentVehicle::Init() {
    m_pVehicle = nullptr;
}

// 0x571EA0
void CCurrentVehicle::Display() const {
    if (m_pVehicle) {
        auto* mi = m_pVehicle->GetVehicleModelInfo();
        CHud::SetVehicleName(TheText.Get(mi->m_szGameName));
    } else {
        CHud::SetVehicleName(nullptr);
    }
}

// 0x572040
void CCurrentVehicle::Process() {
    m_pVehicle = FindPlayerVehicle();
    Display();
}
