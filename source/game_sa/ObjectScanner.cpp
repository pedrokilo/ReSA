/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CObjectScanner
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

#include "ObjectScanner.h"

namespace {
// 0x606010 (lote 204A, sin validar; el listado decia CPedIntelligencePool; thiscall, sin argumentos de pila, `ret`, 49 bytes): constructor de CObjectScanner del EXE (el escaner temporal de ScanForObjectPotentialCollisionEvents).
// Como el original y en este orden: +4 = 0, +8 = 0, las 16 entradas de +0xC a cero (REP STOSD), el entity mas cercano +0x4C = 0, el periodo +8 = 16 (el temporizador queda con cuenta 0 y periodo 16) y la vtable propia del EXE
// (0x86CD8C). No llama a CEntityScanner::CEntityScanner. Devuelve this (EAX). El constructor del compilador dejaria la vtable de la DLL.
CObjectScanner* __fastcall ObjectScannerConstructorExe(CObjectScanner* self, void* /*edx*/) {
    auto* const words = reinterpret_cast<uint32*>(self);
    words[1] = 0;
    words[2] = 0;
    for (size_t i = 0; i < 16; ++i) {
        words[3 + i] = 0;
    }
    words[0x13] = 0;
    words[2]    = 16;
    words[0]    = 0x86CD8C;
    return self;
}

// 0x606050 (lote 204A, sin validar; el listado decia CPedIntelligencePool; thiscall, sin argumentos de pila, `ret`, 11 bytes: `mov dword [ecx], 0x86CD40; jmp 0x5FF9D0`): cuerpo del destructor de CObjectScanner del EXE. Como el original:
// deja la vtable base de CEntityScanner (0x86CD40) y sigue con CEntityScanner::Clear (0x5FF9D0, con hook) por salto.
void __fastcall ObjectScannerDestructorBody(CObjectScanner* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = 0x86CD40;
    self->Clear();
}
} // namespace

void CObjectScanner::InjectHooks() {
    RH_ScopedClass(CObjectScanner);
    RH_ScopedCategoryGlobal();

    RH_ScopedInstall(ScanForObjectsInRange, 0x5FFF30); // lote 198A (sin validar)
    RH_ScopedNamedGlobalInstall(ObjectScannerConstructorExe, "Constructor", 0x606010); // lote 204A (sin validar)
    RH_ScopedNamedGlobalInstall(ObjectScannerDestructorBody, "Destructor", 0x606050);  // lote 204A (sin validar)
}

// 0x5FFF30 (lote 198A, sin validar; thiscall, un argumento de pila (el peaton), `ret 4`, 55 bytes): con vehiculo (+0x58C no nulo) Y en vehiculo (bit 8 de +0x46C) no hace nada; si no, si el peaton esta vivo (CPed::IsAlive 0x5E0170, que se evalua
// DESPUES de las dos comprobaciones anteriores), CEntityScanner::ScanForEntitiesInRange (0x5FFA20, llamada directa) con la lista 2 (objetos).
void CObjectScanner::ScanForObjectsInRange(const CPed& ped) {
    if (ped.m_pVehicle && ped.bInVehicle)
        return;

    if (ped.IsAlive())
        CEntityScanner::ScanForEntitiesInRange(REPEATSECTOR_OBJECTS, ped);
}

CObject* CObjectScanner::GetClosestObjectInRange() const {
    return static_cast<CObject*>(m_pClosestEntityInRange);
}
