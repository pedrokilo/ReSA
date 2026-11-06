/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColPoint
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

#include "ColPoint.h"

#include "CustomBuildingDNPipeline.h"

// 0x59F0C0
float tColLighting::GetCurrentLighting(float fScale) const {
    const auto fDay = static_cast<float>(day) * fScale / 15.0F;
    const auto fNight = static_cast<float>(night) * fScale / 15.0F;
    return lerp(fDay, fNight, CCustomBuildingDNPipeline::m_fDNBalanceParam);
}

// 0x40FC80 (lote 105A; thiscall, `ret 4`; EAX = this): CColPoint::operator= del EXE. Copia el punto (+0x00..+0x08), la normal (+0x10..+0x18), tipo/pieza/iluminacion A (word +0x20 y byte +0x22), B (word
// +0x23 y byte +0x25) y la profundidad (+0x28); NO copia field_C (+0x0C), field_1C (+0x1C) ni el relleno +0x26/+0x27. Cada lectura va seguida de su escritura, en este orden, con los mismos
// anchos (si el origen solapa el destino, las escrituras anteriores se ven). __fastcall con un EDX ficticio: ECX = this y un argumento de pila, igual que thiscall.
CColPoint* __fastcall ColPointAssignExe(CColPoint* self, void* /*edx*/, const CColPoint* source)
{
    volatile uint8* const d = reinterpret_cast<volatile uint8*>(self);
    const volatile uint8* const s = reinterpret_cast<const volatile uint8*>(source);
    *reinterpret_cast<volatile uint32*>(d + 0x00) = *reinterpret_cast<const volatile uint32*>(s + 0x00);
    *reinterpret_cast<volatile uint32*>(d + 0x04) = *reinterpret_cast<const volatile uint32*>(s + 0x04);
    *reinterpret_cast<volatile uint32*>(d + 0x08) = *reinterpret_cast<const volatile uint32*>(s + 0x08);
    *reinterpret_cast<volatile uint32*>(d + 0x10) = *reinterpret_cast<const volatile uint32*>(s + 0x10);
    *reinterpret_cast<volatile uint32*>(d + 0x14) = *reinterpret_cast<const volatile uint32*>(s + 0x14);
    *reinterpret_cast<volatile uint32*>(d + 0x18) = *reinterpret_cast<const volatile uint32*>(s + 0x18);
    *reinterpret_cast<volatile uint16*>(d + 0x20) = *reinterpret_cast<const volatile uint16*>(s + 0x20);
    *reinterpret_cast<volatile uint8*>(d + 0x22) = *reinterpret_cast<const volatile uint8*>(s + 0x22);
    *reinterpret_cast<volatile uint16*>(d + 0x23) = *reinterpret_cast<const volatile uint16*>(s + 0x23);
    *reinterpret_cast<volatile uint8*>(d + 0x25) = *reinterpret_cast<const volatile uint8*>(s + 0x25);
    *reinterpret_cast<volatile uint32*>(d + 0x28) = *reinterpret_cast<const volatile uint32*>(s + 0x28);
    return self;
}

// Lote 105A (banco Fn105A): CColPoint::operator= (0x40FC80). Funcion libre para no tocar ColPoint.h; se llama desde InjectHooksMain.cpp.
void CColPointInjectHooksLote105A()
{
    RH_ScopedNamespaceName("CColPoint");
    RH_ScopedCategory("Collision");

    RH_ScopedGlobalInstall(ColPointAssignExe, 0x40FC80);
}
