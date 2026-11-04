/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CNodeAddress
 * Reconstrucción: COMPLETA — 3/3 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 3/3 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "NodeAddress.h"

void CNodeAddress::InjectHooks() {
    RH_ScopedClass(CNodeAddress);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(operator==, 0x420920);
    RH_ScopedInstall(operator!=, 0x420950);
    RH_ScopedInstall(operator<, 0x420980);
}

// 0x420920
bool CNodeAddress::operator==(CNodeAddress const& other) const {
    return m_wAreaId == other.m_wAreaId && m_wNodeId == other.m_wNodeId;
}

// 0x420950
bool CNodeAddress::operator!=(CNodeAddress const& other) const {
    return m_wAreaId != other.m_wAreaId || m_wNodeId != other.m_wNodeId;
}

// 0x420980
bool CNodeAddress::operator<(CNodeAddress const& other) const {
    if (m_wAreaId != other.m_wAreaId) {
        return m_wAreaId < other.m_wAreaId;
    }
    return m_wNodeId < other.m_wNodeId;
}
