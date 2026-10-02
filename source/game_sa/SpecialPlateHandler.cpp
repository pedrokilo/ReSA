/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CSpecialPlateHandler
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "SpecialPlateHandler.h"

void CSpecialPlateHandler::InjectHooks() {
    RH_ScopedClass(CSpecialPlateHandler);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(Init, 0x6F2D10);
    RH_ScopedInstall(Find, 0x6F2D30);
    RH_ScopedInstall(Add, 0x6F2D90);
    RH_ScopedInstall(Remove, 0x6F2DD0);
}

// unused
// 0x6F2D10
void CSpecialPlateHandler::Init() {
    m_nCount = 0;

    for (auto &entry : m_plateTextEntries) {
        entry.m_nCarGenId = -1;
        entry.m_szPlateText[0] = 0;
    }
}

// Writes plate text to outText if exists, "\0" otherwise.
// Como el original: devuelve el índice de la entrada o -1.
// 0x6F2D30
int32 CSpecialPlateHandler::Find(int32 carGenId, char* outText) {
    outText[0] = '\0';
    if (m_nCount == 0)
        return -1;

    for (auto i = 0; i < (int32)PLATES_COUNT; i++) {
        auto& entry = m_plateTextEntries[i];
        if (entry.m_nCarGenId == carGenId) {
            strcpy(outText, entry.m_szPlateText); // Como el original: strcpy (strcpy_s rellena con 0xFE en debug)
            return i;
        }
    }
    return -1;
}

// 0x6F2D90
void CSpecialPlateHandler::Add(int32 carGenId, const char* text) {
    if (m_nCount == PLATES_COUNT) {
        return;
    }
    m_plateTextEntries[m_nCount].m_nCarGenId = carGenId;
    strcpy(m_plateTextEntries[m_nCount].m_szPlateText, text); // Como el original: strcpy sin límite
    m_nCount++;
}

// unused
// 0x6F2DD0
void CSpecialPlateHandler::Remove(int32 plateTextId) {
    if (plateTextId >= 0 && m_nCount) {
        tCarGenPlateText* plateText = &m_plateTextEntries[plateTextId];
        plateText->m_nCarGenId = -1;
        plateText->m_szPlateText[0] = 0;
        if (plateTextId < (int32)(m_nCount - 1)) { // Como el original: comparación con signo
            *plateText = m_plateTextEntries[m_nCount - 1];
        }
        --m_nCount;
    }
}
