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
void __fastcall Destructor(CTask* self, void* edx);  // Task.cpp: 0x61A3D0
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

// 0x6956D0 (lote 201A, sin validar; thiscall, sin argumentos de pila, `ret`, 97 bytes con marco SEH que solo desenrolla y se omite; el listado decia CTaskComplexSeekEntityAnyMeansXYOffset y SI lo es): cuerpo no virtual de
// ~CTaskComplexSeekEntityAnyMeansXYOffset del EXE (lo llama el destructor con borrado 0x6956B0). Orden del EXE: vtable de la clase (0x870CF0), si la entidad (+0xC) no es nula CEntity::CleanUpOldReference (0x571A00) sobre +0xC,
// vtable base del calculador de posiciones (+0x24) = 0x859DC4 (el destructor del calculador deja la vtable de CEntitySeekPosCalculator) y fin en CTaskComplex::Destructor (0x61A3D0, con hook; ECX = this). El destructor generado por el
// compilador dejaria la vtable de la DLL.
__declspec(noinline) void __fastcall AnyMeansXYOffsetDestructorBodyExe(void* self, void* /*edx*/) {
    auto* const raw = static_cast<uint8*>(self);
    *reinterpret_cast<uint32*>(raw + 0x00) = 0x870CF0;
    if (CEntity* const entity = *reinterpret_cast<CEntity**>(raw + 0xC)) {
        entity->CleanUpOldReference(reinterpret_cast<CEntity**>(raw + 0xC));
    }
    *reinterpret_cast<uint32*>(raw + 0x24) = 0x859DC4;
    CTaskComplexBody::Destructor(static_cast<CTask*>(self), nullptr);
}

// 0x6956B0 (lote 201A, sin validar; thiscall, un argumento de pila, `ret 4`, 30 bytes: `push esi; mov esi,ecx; call 0x6956D0; test byte [esp+8],1; je ..; push esi; call 0x61A5B0; add esp,4; mov eax,esi; pop esi; ret 4`): destructor con borrado
// de CTaskComplexSeekEntityAnyMeansXYOffset del EXE (ranura 0 de su vtable 0x870CF0). Como el original: destruye con el cuerpo (0x6956D0), libera `this` con CTask::operator delete (0x61A5B0, con hook) solo si el bit 0 del BYTE de banderas
// esta puesto y devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura 0).
__declspec(noinline) void* __fastcall AnyMeansXYOffsetScalarDeletingDestructor(void* self, void* /*edx*/, uint8 flags) {
    AnyMeansXYOffsetDestructorBodyExe(self, nullptr);
    if (flags & 1) {
        CTask::operator delete(self);
    }
    return self;
}
} // namespace

void CTaskComplexSeekEntityAnyMeansXYOffset::InjectHooks() {
    RH_ScopedVirtualClass(CTaskComplexSeekEntityAnyMeansXYOffset, 0x870CF0, 11);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(AnyMeansXYOffsetConstructorExe, "ConstructorExe", 0x695600); // lote 166A (banco Fn166A)
    RH_ScopedNamedGlobalInstall(AnyMeansXYOffsetDestructorBodyExe, "DestructorBody", 0x6956D0); // lote 201A (sin validar)
    RH_ScopedNamedGlobalInstall(AnyMeansXYOffsetScalarDeletingDestructor, "ScalarDeletingDestructor", 0x6956B0); // lote 201A (sin validar)
    RH_ScopedVMTInstall(Clone, 0x696030);
    RH_ScopedVMTInstall(GetTaskType, 0x695690);
    RH_ScopedVMTInstall(CreateNextSubTask, 0x6956A0);
    RH_ScopedVMTInstall(CreateFirstSubTask, 0x6960B0);
    RH_ScopedVMTInstall(ControlSubTask, 0x696160);
}
