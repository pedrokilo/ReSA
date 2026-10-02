/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CWaterCannons
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "WaterCannons.h"
#include "WaterCannon.h"

void CWaterCannons::InjectHooks() {
    RH_ScopedClass(CWaterCannons);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x728C80);
    RH_ScopedInstall(UpdateOne, 0x728CB0);
    RH_ScopedInstall(Update, 0x72A3C0);
    RH_ScopedInstall(Render, 0x729B30);
}

// 0x728C80
void CWaterCannons::Init() {
    for (auto& cannon : aCannons) {
        cannon.Init();
    }
}

// 0x728CB0
void CWaterCannons::UpdateOne(uint32 vehicle, CVector* start, CVector* end) {
    for (auto& cannon : aCannons) {
        if (cannon.m_nId == vehicle) {
            cannon.Update_NewInput(start, end);
            return;
        }
    }

    // find free slot and update
    auto i = 0;
    while (aCannons[i].m_nId) {
        if (++i >= MAX_WATER_CANNONS)
            return;
    }

    auto& cannon = aCannons[i];
    cannon.Init();
    cannon.m_nId = vehicle;
    cannon.Update_NewInput(start, end);
}

// 0x72A3C0
void CWaterCannons::Update() {
    ZoneScoped;

    int16 index = 0;
    for (auto& cannon : aCannons) {
        cannon.m_Audio.Service();
        if (cannon.m_nId > 0) {
            cannon.Update_OncePerFrame(index);
        }
        ++index;
    }
}

// 0x729B30
void CWaterCannons::Render() {
    ZoneScoped;

    for (auto& cannon : aCannons) {
        cannon.Render();
    }
}
