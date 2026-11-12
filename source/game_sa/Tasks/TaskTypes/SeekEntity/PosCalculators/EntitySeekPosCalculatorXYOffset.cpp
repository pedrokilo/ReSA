/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CEntitySeekPosCalculatorXYOffset
 * Reconstrucción: COMPLETA — 2/2 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 2/2 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "EntitySeekPosCalculatorXYOffset.h"
#include "PedGeometryAnalyser.h"

namespace {
// 0x65DE60 (lote 168A, banco Fn168A): constructor de CEntitySeekPosCalculatorXYOffset desde un desplazamiento del EXE (thiscall, un puntero de pila, `ret 4`, 36 bytes; el listado lo atribuia a CEventPassObject).
// Deja la vtable de la clase del EXE (0x86F8F8; la de la DLL no vale para los llamadores del EXE) y copia los tres dwords del desplazamiento (+4, +8 y +0xC) en el orden del EXE (primero la vtable, despues
// los tres dwords) como enteros, no como float (una carga x87 volveria silenciosos los NaN senalizantes); devuelve `this`.
__declspec(noinline) CEntitySeekPosCalculatorXYOffset* __fastcall XYOffsetConstructorFromOffset(CEntitySeekPosCalculatorXYOffset* self, void* /*edx*/, const uint32* offset) {
    uint32* const dst = reinterpret_cast<uint32*>(self);
    dst[0] = 0x86F8F8;
    dst[1] = offset[0];
    dst[2] = offset[1];
    dst[3] = offset[2];
    return self;
}
} // namespace

void CEntitySeekPosCalculatorXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CEntitySeekPosCalculatorXYOffset, 0x86F8F8, 1); // Como el original: vtable de una sola entrada
    RH_ScopedCategory("Tasks/TaskTypes/SeekPosCalculators");

    RH_ScopedNamedGlobalInstall(XYOffsetConstructorFromOffset, "Constructor", 0x65DE60); // lote 168A (banco Fn168A)
    RH_ScopedVMTInstall(ComputeEntitySeekPos, 0x6946a0);
}

// 0x6946A0
void CEntitySeekPosCalculatorXYOffset::ComputeEntitySeekPos(const CPed& seeker, const CEntity& target, CVector& outPos) {
    CPedGeometryAnalyser::ComputeClearTarget(seeker, target.GetPosition() + m_offsetXY, outPos);
}
