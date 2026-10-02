/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTimeInfo
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "TimeInfo.h"

/**
 * Registra la lógica que enlaza las variantes diurna/nocturna de modelos temporizados.
 */
void CTimeInfo::InjectHooks() {
    RH_ScopedClass(CTimeInfo);
    RH_ScopedCategory("Models");

    RH_ScopedInstall(FindOtherTimeModel, 0x4C47E0);
    RH_ScopedInstall(GetOtherTimeModel, 0x4C4A30);
    RH_ScopedInstall(GetTimeOff, 0x407330);
    RH_ScopedInstall(GetTimeOn, 0x407320);
    RH_ScopedInstall(SetOtherTimeModel, 0x5B3440);
    RH_ScopedInstall(SetTimes, 0x5B3430);
}

// 0x4C47E0
/**
 * Resuelve la pareja temporal sustituyendo el sufijo _nt por _dy o viceversa.
 *
 * Si existe un model info temporal con el nombre resultante, almacena su índice en
 * m_nOtherTimeModel para evitar repetir la resolución por nombre.
 */
CTimeInfo* CTimeInfo::FindOtherTimeModel(const char* modelName) {
    char timeSwitchModelName[24];
    strcpy_s(timeSwitchModelName, modelName);

    char* nightSuffix = strstr(timeSwitchModelName, "_nt");

    if (nightSuffix) {
        strncpy_s(nightSuffix, 4, "_dy", 4);
    }
    else {
        char* daySuffix = strstr(timeSwitchModelName, "_dy");

        if (!daySuffix)
            return nullptr;

        strncpy_s(daySuffix, 4, "_nt", 4);
    }

    int32 index = 0;
    CBaseModelInfo* modelInfo = CModelInfo::GetModelInfoFromHashKey(CKeyGen::GetUppercaseKey(timeSwitchModelName), &index);
    CTimeInfo* timeInfo = modelInfo ? modelInfo->GetTimeInfo() : nullptr;

    if (timeInfo) {
        m_nOtherTimeModel = index;
        return timeInfo;
    }

    return nullptr;
}
