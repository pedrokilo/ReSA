/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskComplexSeekEntityAnyMeansXYOffset
 * Reconstrucción: COMPLETA — 6/6 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 6/6 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"
#include "TaskComplexSeekEntityAnyMeansXYOffset.h"

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x695600 (lote 166A, banco Fn166A; thiscall, tres argumentos de pila, `ret 0Ch`, 135 bytes con marco SEH que solo desenrolla; el listado decia CTaskComplexFollowPedFootsteps; lo llama Clone 0x696030 y
// otro constructor del EXE, 0x6967E7): constructor del EXE de CTaskComplexSeekEntityAnyMeansXYOffset con el desplazamiento a cero. Como el original: base CTaskComplex (0x61A3B0, con hook), entidad (+0xC),
// periodo (+0x10) y radio (+0x14, los bits del float tal cual), vtable 0x870CF0 (la del EXE), temporizador de busqueda a cero (+0x18 inicio, +0x1C intervalo, +0x20 arrancado, +0x21 parado; el relleno
// +0x22..+0x23 no se escribe), vtable del calculador de posiciones 0x86F8F8 (+0x24) con el desplazamiento a cero (+0x28, +0x2C, +0x30) y, si la entidad no es nula (se relee de +0xC), la referencia
// registrada con CEntity::RegisterReference (0x571B70) sobre +0xC. Devuelve this. El radio llega como los bits del float (un NaN senalizante se conserva).
__declspec(noinline) void* __fastcall AnyMeansXYOffsetConstructorExe(void* self, void* /*edx*/, CEntity* entity, int32 period, uint32 radiusBits) {
    auto* const raw = static_cast<uint8*>(self);
    CTaskComplexBody::Constructor(self, nullptr);
    *reinterpret_cast<CEntity**>(raw + 0xC)  = entity;
    *reinterpret_cast<int32*>(raw + 0x10)    = period;
    *reinterpret_cast<uint32*>(raw + 0x14)   = radiusBits;
    *reinterpret_cast<uint32*>(raw + 0x00)   = 0x870CF0;
    *reinterpret_cast<uint32*>(raw + 0x18)   = 0;
    *reinterpret_cast<uint32*>(raw + 0x1C)   = 0;
    *(raw + 0x20)                            = 0;
    *(raw + 0x21)                            = 0;
    *reinterpret_cast<uint32*>(raw + 0x24)   = 0x86F8F8;
    *reinterpret_cast<uint32*>(raw + 0x28)   = 0;
    *reinterpret_cast<uint32*>(raw + 0x2C)   = 0;
    *reinterpret_cast<uint32*>(raw + 0x30)   = 0;
    if (CEntity* const registered = *reinterpret_cast<CEntity**>(raw + 0xC)) {
        registered->RegisterReference(reinterpret_cast<CEntity**>(raw + 0xC));
    }
    return self;
}
} // namespace

void CTaskComplexSeekEntityAnyMeansXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityAnyMeansXYOffset, 0x870CF0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(AnyMeansXYOffsetConstructorExe, "ConstructorExe", 0x695600); // lote 166A (banco Fn166A)
    RH_ScopedVMTInstall(Clone, 0x696030);
    RH_ScopedVMTInstall(GetTaskType, 0x695690);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6956A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6960B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x696160);
}
