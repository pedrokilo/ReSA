/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPlaceName
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

#include "PlaceName.h"

#include "EntryExitManager.h"
#include "Hud.h"

void CPlaceName::InjectHooks() {
    RH_ScopedClass(CPlaceName);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetForMap, 0x571D90);
    RH_ScopedInstall(Process, 0x571F20);
}

// 0x571E90
CPlaceName::CPlaceName() {
    m_pZone = nullptr;
}

// 0x571F10
void CPlaceName::Init() {
    m_pZone = nullptr;
    m_nAdditionalTimer = 0;
}

// 0x571E40
void CPlaceName::ProcessAfterFrontEndShutDown() {
    CHud::m_ZoneState = NAME_DONT_SHOW;
    CHud::m_pLastZoneName = nullptr;
    m_nAdditionalTimer = 250;
}

// 0x571D90
const GxtChar* CPlaceName::GetForMap(const CVector2D pos) {
    CVector posn = CalcPosition();
    CZone* zone = CTheZones::FindSmallestZoneForPosition({ pos.x, pos.y, posn.z }, false);
    return zone->GetTranslatedName();
}

// 0x571F20
void CPlaceName::Process() {
    CVector pos = CalcPosition();
    CZone* smallestZone = CTheZones::FindSmallestZoneForPosition(pos, false);
    if (!smallestZone) {
        Init();
    }

    if ((smallestZone == m_pZone || CGame::currArea == AREA_CODE_1) 
            && m_pZone
            || smallestZone
            && m_pZone // yep, checked twice (Pirulax: gotta account for those cosmis bit flipping rays?)
            && memcmp(smallestZone->m_TextLabel, m_pZone->m_TextLabel, sizeof(smallestZone->m_TextLabel)) == 0
    ) {
        if (m_nAdditionalTimer) {
            m_nAdditionalTimer -= 1;
        }
    } else {
        m_pZone = smallestZone;
        m_nAdditionalTimer = 250;
    }

    Display();
}

// 0x571E60
void CPlaceName::Display() const {
    if (m_pZone) {
        CHud::SetZoneName(m_pZone->GetTranslatedName(), false);
    } else {
        CHud::SetZoneName(nullptr, false);
    }
}

CVector CPlaceName::CalcPosition() {
    auto player = FindPlayerPed();
    if (player->bInVehicle) {
        return player->m_pVehicle->GetPosition();
    } else {
        auto posn = player->GetPosition();
        CEntryExitManager::GetPositionRelativeToOutsideWorld(posn);
        return posn;
    }
}
