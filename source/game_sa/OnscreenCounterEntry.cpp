/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: COnscreenCounterEntry
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
#include "OnscreenCounterEntry.h"
#include "TheScripts.h"

void COnscreenCounterEntry::InjectHooks() {
    RH_ScopedClass(COnscreenCounterEntry);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ProcessForDisplayCounter, 0x44CA90);
    RH_ScopedInstall(SetColourID, 0x44CB00);
}

void COnscreenCounterEntry::Init() {
    Clear();

    m_bFlashWhenFirstDisplayed = false;
    m_nColourId                = HUD_COLOUR_RED;
    memset(&m_szDisplayedText, 0, sizeof(m_szDisplayedText));           // original initialization
}

void COnscreenCounterEntry::Clear() {
    m_nVarId                  = 0;
    m_nMaxVarValue            = 0;
    m_nType                   = eOnscreenCounter::SIMPLE;
    m_bEnabled                = false;
    memset(&m_szDescriptionTextKey, 0, sizeof(m_szDescriptionTextKey)); // original initialization
}

// 0x44CA90
void COnscreenCounterEntry::ProcessForDisplayCounter(eOnscreenCounter type) {
    const auto t2 = *CTheScripts::GetPointerToScriptVariable(m_nVarId);
    const auto t1 = std::max<int32>(0, t2);

    switch (type) {
    case eOnscreenCounter::SIMPLE:
    case eOnscreenCounter::LINE:
        sprintf_s(m_szDisplayedText, "%d", t1);
        break;
    case eOnscreenCounter::DOUBLE:
        sprintf_s(m_szDisplayedText, "%d / %d", t1, t2);
        break;
    }
}

// 0x44CB00
void COnscreenCounterEntry::SetColourID(eHudColours colorId) {
    m_nColourId = colorId;
}
