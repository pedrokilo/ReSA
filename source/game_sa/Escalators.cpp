/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEscalators
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

#include "Escalators.h"
#include "Escalator.h"

void CEscalators::InjectHooks() {
    RH_ScopedClass(CEscalators);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x717C50);
    RH_ScopedInstall(Shutdown, 0x717940);
    RH_ScopedInstall(AddOne, 0x717C90);
    RH_ScopedInstall(Update, 0x718580);
}

// 0x717940
void CEscalators::Shutdown() {
    rng::for_each(aEscalators, &CEscalator::Remove);
}

// 0x717C50
void CEscalators::Init() {
    Shutdown();
    rng::for_each(aEscalators, &CEscalator::SwitchOff);
}

// 0x717C90
void CEscalators::AddOne(const CVector& vecStart, const CVector& vecBottom, const CVector& vecTop, const CVector& vecEnd, bool moveDown, CEntity* entity) {
    for (CEscalator& escalator : aEscalators) {
        if (!escalator.m_Exist) {
            escalator.AddThisOne(vecStart, vecBottom, vecTop, vecEnd, moveDown, entity);
            break;
        }
    }
}

// 0x718580
void CEscalators::Update() {
    if (CReplay::Mode != MODE_PLAYBACK) {
        rng::for_each(GetAllExists(), &CEscalator::Update);
    }
}
