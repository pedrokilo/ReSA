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
