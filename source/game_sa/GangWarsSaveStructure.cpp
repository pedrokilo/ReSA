/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CGangWarsSaveStructure
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

#include "GangWarsSaveStructure.h"

void CGangWarsSaveStructure::InjectHooks() {
    RH_ScopedClass(CGangWarsSaveStructure);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Construct, 0x5D2620); // lote 196A (sin validar)
    RH_ScopedInstall(Extract, 0x5D2740);   // lote 196A (sin validar)
}

// 0x5D2620 (lote 196A, sin validar; thiscall, sin argumentos de pila, `ret`, 282 bytes): copia los estados estaticos de CGangWars (0x96AB44..0x96ABD0) a la estructura de guardado (0x58 bytes). Los indices de zona se buscan
// con un bucle sobre los arrays del EXE (informacion de zonas: base 0xBA1DF0, pasos de 0x11, contador WORD 0xBA1DE8; zonas de navegacion: base 0xBA3798, pasos de 0x20, contador WORD 0xBA3794) y valen -1 si el puntero es
// nulo o no esta. El EXE lee los contadores como WORD SIN signo (movzx).
void CGangWarsSaveStructure::Construct() {
    bGangWarsActive = CGangWars::bGangWarsActive;
    State = CGangWars::State;
    TimeStarted = CGangWars::TimeStarted;
    GangWarZoneInfoIndex = -1;

    if (CGangWars::pZoneInfoToFightOver) {
        for (auto i = 0; i < static_cast<uint16>(CTheZones::TotalNumberOfZoneInfos); i++) {
            if (CGangWars::pZoneInfoToFightOver == CTheZones::ZoneInfoArray.data() + i) {
                GangWarZoneInfoIndex = i;
                break;
            }
        }
    }

    // Como el original (0x5D2671): el indice de zona de navegacion se pone a -1 siempre, y su bucle depende de pZoneToFightOver (0x96AB88), no de pZoneInfoToFightOver.
    GangWarNavigationZoneIndex = (uint32)-1;
    if (CGangWars::pZoneToFightOver) {
        for (auto i = 0; i < static_cast<uint16>(CTheZones::TotalNumberOfNavigationZones); i++) {
            if (CGangWars::pZoneToFightOver == CTheZones::NavigationZoneArray.data() + i) {
                GangWarNavigationZoneIndex = i;
                break;
            }
        }
    }

    CoorsOfPlayerAtStartOfWar = CGangWars::CoorsOfPlayerAtStartOfWar;
    Gang1 = CGangWars::Gang1;
    Gang2 = CGangWars::Gang2;
    WarFerocity = CGangWars::WarFerocity;
    LastTimeInArea = CGangWars::LastTimeInArea;
    State2 = CGangWars::State2;
    TimeTillNextAttack = CGangWars::TimeTillNextAttack;
    PointOfAttack = CGangWars::PointOfAttack;
    FightTimer = CGangWars::FightTimer;
    RadarBlip = CGangWars::RadarBlip;
    bPlayerIsCloseby = CGangWars::bPlayerIsCloseby;
    TerritoryUnderControlPercentage = CGangWars::TerritoryUnderControlPercentage;
    Difficulty = CGangWars::Difficulty;
}

// 0x5D2740 (lote 196A, sin validar; thiscall, `ret`, 234 bytes): inversa de Construct. Un indice -1 (comparado como DWORD) deja el puntero a nulo; si no, base + indice * 0x11 (informacion de zona) o * 0x20 (zona de navegacion).
void CGangWarsSaveStructure::Extract() const {
    CGangWars::bGangWarsActive = bGangWarsActive;
    CGangWars::State = State;
    CGangWars::TimeStarted = TimeStarted;
    CGangWars::pZoneInfoToFightOver = (GangWarZoneInfoIndex != -1) ? CTheZones::ZoneInfoArray.data() + GangWarZoneInfoIndex : nullptr;
    CGangWars::pZoneToFightOver = (GangWarNavigationZoneIndex != -1) ? CTheZones::NavigationZoneArray.data() + GangWarNavigationZoneIndex : nullptr;
    CGangWars::CoorsOfPlayerAtStartOfWar = CoorsOfPlayerAtStartOfWar;
    CGangWars::Gang1 = (eGangID)Gang1;
    CGangWars::Gang2 = Gang2;
    CGangWars::WarFerocity = WarFerocity;
    CGangWars::LastTimeInArea = LastTimeInArea;
    CGangWars::State2 = State2;
    CGangWars::TimeTillNextAttack = TimeTillNextAttack;
    CGangWars::PointOfAttack = PointOfAttack;
    CGangWars::FightTimer = FightTimer;
    CGangWars::RadarBlip = RadarBlip;
    CGangWars::bPlayerIsCloseby = bPlayerIsCloseby;
    CGangWars::TerritoryUnderControlPercentage = TerritoryUnderControlPercentage;
    CGangWars::Difficulty = Difficulty;
}
