/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCreepingFire
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

#include "CreepingFire.h"
#include "FireManager.h"

void CCreepingFire::InjectHooks() {
    RH_ScopedClass(CCreepingFire);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(SetReadyToBurn, 0x539CC0);
    RH_ScopedInstall(Update, 0x539CE0);
    RH_ScopedInstall(TryToStartFireAtCoors, 0x53A450);
}

// 0x539CC0
void CCreepingFire::SetReadyToBurn() {
    for (int32 i = 0; i < 32; i++) {
        for (int32 j = 0; j < 32; j++) {
            m_aFireStatus[i][j] = 0;
        }
    }
}

// 0x539CE0
void CCreepingFire::Update() {
    ZoneScoped;

    uint8& status = m_aFireStatus[CTimer::GetFrameCounter() % 32][(CTimer::GetFrameCounter() / 32) % 32];
    if (status == 4) {
        status = 0;
    } else if (status > 4 && status <= 6) {
        status -= 1;
    }
}

// 0x53A450
bool CCreepingFire::TryToStartFireAtCoors(CVector pos, uint8 nGenerations, bool a5, bool bScriptFire, float fDistance) {
    uint8& status = m_aFireStatus[(size_t)pos.x % 32][(size_t)pos.y % 32];
    if (status == 0)
        return false;

    if (!gFireManager.PlentyFiresAvailable())
        return false;

    CEntity* hitEntity{};
    CColPoint cp;
    if (!CWorld::ProcessVerticalLine(pos, pos.z - fDistance, cp, hitEntity, true, false, false, false, false, false, nullptr))
        return false;

    pos.z = cp.m_vecPoint.z;
    status = 6;

    CFire* fire = gFireManager.StartFire(pos, 0.8f, true, nullptr, 20'000u, nGenerations, !bScriptFire);
    if (!fire)
        return false;

    fire->SetIsFirstGen(false);
    fire->SetIsScript(bScriptFire);

    return true;
}
