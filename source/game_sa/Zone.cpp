/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CZone
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "Zone.h"

// Returns pointer to GXT name string.
// 0x5720C0
const GxtChar* CZone::GetTranslatedName() const {
    return TheText.Get(m_TextLabel);
}

// Lote 124A (banco Fn124A): 0x5720C0 es CZone::GetTranslatedName del EXE (el listado de pendientes decia CUserDisplay): thiscall, sin argumentos de pila, `ret`, 15 bytes:
// `add ecx,8; push ecx; mov ecx,0C1B340h; call 0x6A0050; ret`, es decir CText::Get(TheText, this + 8) (m_TextLabel) y devuelve su resultado. Lo llaman CPlaceName::GetForMap
// y CPlaceName::Process. Solo se instala este hook (CZone no tiene InjectHooks propio para no tocar Zone.h).
void CZoneInjectHooksLote124A() {
    RH_ScopedClass(CZone);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetTranslatedName, 0x5720C0);
}
