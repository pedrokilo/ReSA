/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CCrime
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

#include "Crime.h"
#include "PedType.h"
#include "eWantedLevel.h"
#include "World.h"
#include "Localisation.h"
#include "Messages.h"

void CCrime::InjectHooks() {
    RH_ScopedClass(CCrime);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(FindImmediateDetectionRange, 0x531FC0);
    RH_ScopedInstall(ReportCrime, 0x532010);
}

// 0x531FC0
float CCrime::FindImmediateDetectionRange(eCrimeType CrimeType) {
    switch (CrimeType) {
    case CRIME_DESTROY_HELI:
    case CRIME_DESTROY_PLANE:
    case CRIME_EXPLOSION:
        return 60.0f;
    case CRIME_DESTROY_VEHICLE:
        return 30.0f;
    default:
        return 14.0f;
    }
}

// 0x532010
void CCrime::ReportCrime(eCrimeType crimeType, CEntity* pVictim, CPed* pCommitedby) {
    if (!pCommitedby || !pCommitedby->IsPlayer()) {
        return;
    }

    bool isPedCriminal = false;
    if (pVictim && pVictim->GetIsTypePed()) {
        isPedCriminal = CPedType::PoliceDontCareAboutCrimesAgainstPedType(pVictim->AsPed()->m_nPedType);
    }

    if (crimeType == CRIME_DAMAGED_PED) {
        // Como el original: el EXE no comprueba que la víctima sea no nula en este punto (se mantiene la guarda).
        if (pVictim
            && pVictim->GetIsTypePed()
            && IsPedPointerValid(pVictim->AsPed())
            && pCommitedby->AsPlayer()->GetWantedLevel() == eWantedLevel::WANTED_CLEAN
            && pVictim->AsPed()->bBeingChasedByPolice // Vanilla bug here
        ) {
            if (!pVictim->AsPed()->IsStateDying()) {
                // Como el original: no comprueba el texto y suma el dinero al jugador en foco, sea cual sea el autor.
                CMessages::AddBigMessage(TheText.Get("GOODBOY"), 5'000, eMessageStyle::STYLE_MIDDLE); // Good Citizen Bonus! +$50
                CWorld::Players[CWorld::PlayerInFocus].m_nMoney += 50;
            }
            return;
        }
    } else if (crimeType == CRIME_NONE) {
        return;
    }

    // Como el original: copia la posición del autor y usa siempre el CWanted de FindPlayerPed(-1), sin exigir víctima.
    const CVector comittedByPos = pCommitedby->GetPosition();
    if (FindPlayerPed()->GetPlayerWanted()->m_Multiplier >= 0.0f) {
        // Como el original: primero consulta la presencia policial y solo después la versión alemana.
        if (CWanted::WorkOutPolicePresence(comittedByPos, FindImmediateDetectionRange(crimeType))
            || (notsa::contains({CRIME_DAMAGE_CAR, CRIME_DAMAGE_COP_CAR, CRIME_SET_PED_ON_FIRE, CRIME_SET_COP_PED_ON_FIRE}, crimeType) && CLocalisation::GermanGame())) {
            FindPlayerPed()->GetPlayerWanted()->RegisterCrime_Immediately(crimeType, comittedByPos, (uint32)pVictim, isPedCriminal);
            FindPlayerPed()->GetPlayerWanted()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1); // We will never know if this is a bug or not.
        } else {
            FindPlayerPed()->GetPlayerWanted()->RegisterCrime(crimeType, comittedByPos, (uint32)pVictim, isPedCriminal);
        }
    }

    // Como el original: CPlayerPed::SetWantedLevelNoDrop (0x609F30) sobre FindPlayerPed(-1).
    switch (crimeType) {
    case CRIME_DAMAGED_COP:   FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_1); break;
    case CRIME_DAMAGE_COP_CAR:
    case CRIME_STAB_COP:      FindPlayerPed()->SetWantedLevelNoDrop(eWantedLevel::WANTED_LEVEL_2); break;
    }
}
