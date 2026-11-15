/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedScanner
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
#include "PedScanner.h"

namespace {
// 0x606000 (lote 204A, sin validar; el listado decia CPedIntelligencePool; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov dword [ecx], 0x86CD40; jmp 0x5FF9D0`): cuerpo del destructor de CPedScanner del EXE (el escaner
// de peatones de CPedIntelligence, +0x124; solo lo llama el desenrollado SEH de CPedIntelligence). Como el original: deja la vtable base de CEntityScanner (0x86CD40) y sigue con CEntityScanner::Clear (0x5FF9D0, con hook) por salto.
void __fastcall PedScannerDestructorBody(CPedScanner* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CD40;
    self->Clear();
}
} // namespace

// Lote 109A (banco Fn109A): GetClosestPedInRange (0x5FFF20; thiscall, devuelve el dword +0x4C del escaner en EAX).
void CPedScannerInjectHooksLote109A() {
    RH_ScopedClass(CPedScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(GetClosestPedInRange, 0x5FFF20);
    RH_ScopedNamedGlobalInstall(PedScannerDestructorBody, "Destructor", 0x606000); // lote 204A (sin validar)
}

// 0x5FFEF0
void CPedScanner::ScanForPedsInRange(const CPed& ped) {
    if (ped.IsAlive())
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_PEDS, ped);
}

// 0x5FFF20
CPed* CPedScanner::GetClosestPedInRange() const {
    return static_cast<CPed*>(m_pClosestEntityInRange);
}
