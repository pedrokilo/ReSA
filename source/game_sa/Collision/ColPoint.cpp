/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CColPoint
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 *
 * ESTADO: tColLighting
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
#include "X87Math.h"

// 0x59F0C0
float tColLighting::GetCurrentLighting(float fScale) const {
    const auto fDay = static_cast<float>(day) * fScale / 15.0F;
    const auto fNight = static_cast<float>(night) * fScale / 15.0F;
    return lerp(fDay, fNight, CCustomBuildingDNPipeline::m_fDNBalanceParam);
}

// 0x59F0C0 (lote 116A, banco Fn116A): la funcion del EXE no es un metodo: recibe en la pila el byte de iluminacion (dia en el nibble bajo, noche en el alto) y el factor, lee el balance dia/noche
// (0x8D12C0) y devuelve el resultado en ST0 sin guardarlo como float (cdecl, `ret`).
static double __cdecl ColLightingCurrentExe(unsigned int lighting, float scale)
{
    return ColLightingCurrentCpp(lighting & 0xFu, (lighting & 0xFFu) >> 4, scale, CCustomBuildingDNPipeline::m_fDNBalanceParam);
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

// 0x47E050 (lote 144A, banco Fn144A; thiscall, sin argumentos de pila, `ret`, 4 bytes: `mov al,[ecx+23h]; ret`; solo lo llama CAutomobile::ProcessCarWheelPair, 0x6A54DE, que lee AL con movzx; el listado
// lo atribuia a CTheCarGenerators): tipo de superficie B del punto (m_nSurfaceTypeB, +0x23). Solo AL es contrato (el resto de EAX queda como estaba). __fastcall con EDX ficticio: ECX = this, igual que thiscall.
static uint8 __fastcall ColPointSurfaceTypeBExe(const CColPoint* self, void* /*edx*/)
{
    return *reinterpret_cast<const volatile uint8*>(reinterpret_cast<const uint8*>(self) + 0x23);
}

// Lote 105A (banco Fn105A): CColPoint::operator= (0x40FC80). Funcion libre para no tocar ColPoint.h; se llama desde InjectHooksMain.cpp.
void CColPointInjectHooksLote105A()
{
    RH_ScopedNamespaceName("CColPoint");
    RH_ScopedCategory("Collision");

    RH_ScopedGlobalInstall(ColPointAssignExe, 0x40FC80);
    RH_ScopedNamedGlobalInstall(ColPointSurfaceTypeBExe, "GetSurfaceTypeB", 0x47E050); // lote 144A (banco Fn144A)
}

// Lote 116A (banco Fn116A): tColLighting::GetCurrentLighting (0x59F0C0). Funcion libre como la anterior; se llama desde InjectHooksMain.cpp.
void tColLightingInjectHooksLote116A()
{
    RH_ScopedNamespaceName("tColLighting");
    RH_ScopedCategory("Collision");

    RH_ScopedNamedGlobalInstall(ColLightingCurrentExe, "GetCurrentLighting", 0x59F0C0);
}
