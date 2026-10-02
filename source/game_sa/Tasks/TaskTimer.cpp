/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskTimer
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

/*
    Plugin-SDK file
    Authors: GTA Community. See more here
    https://github.com/DK22Pac/plugin-sdk
    Do not delete this comment block. Respect others' work!
*/
#include "StdInc.h"

void CTaskTimer::InjectHooks() {
    RH_ScopedClass(CTaskTimer);
    RH_ScopedCategory("Tasks");

    RH_ScopedInstall(IsOutOfTime, 0x420E30);
}

// 0x420E30
bool CTaskTimer::IsOutOfTime() {
    if (!m_bStarted) {
        return false;
    }

    if (m_bStopped) {
        m_nStartTime = CTimer::GetTimeInMS();
        m_bStopped = false;
    }

    return CTimer::GetTimeInMS() >= (m_nStartTime + m_nInterval);
}
