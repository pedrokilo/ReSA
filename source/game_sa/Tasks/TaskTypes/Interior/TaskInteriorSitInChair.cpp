/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CTaskInteriorSitInChair
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
#include "X87Math.h"

#include "TaskInteriorSitInChair.h"

#include "AnimBlendAssociation.h"
#include "Ragdoll/IKChainManager.h"

namespace {
// Lote 176A (sin validar). La clase conserva la vtable del EXE (0x870314) y sus campos nativos como bytes (m_OriginalFields: info +8, duracion +0xC, modo +0x10, animacion +0x14, banderas +0x2A), por eso estas funciones
// se enganchan por direccion (S) en lugar de por la vtable de la DLL.
constexpr uint32 SIT_IN_CHAIR_VTABLE = 0x870314;
static_assert(CTaskInteriorSitInChair::Type == 0x57F); // GetTaskType del EXE (0x675C80)

template<typename T>
T& SitInChairField(CTaskInteriorSitInChair* self, size_t offset) {
    return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(self) + offset);
}

// 0x675C80: `mov eax, 0x57F; ret` (TASK_INTERIOR_SIT_ON_CHAIR, ranura 4 de la vtable 0x870314).
eTaskType __fastcall SitInChairGetTaskType(CTaskInteriorSitInChair* /*self*/, void* /*edx*/) {
    return CTaskInteriorSitInChair::Type;
}

// 0x675C90 (thiscall, sin argumentos de pila, `ret`, 93 bytes con marco SEH que solo desenrolla y aqui se omite): cuerpo del destructor del EXE. Como el original: vtable propia (0x870314); si hay animacion (+0x14), le pone
// el callback de borrado por defecto con datos nulos (CAnimBlendAssociation::SetDeleteCallback, 0x4CEBC0, con hook; la animacion NO se suelta ni se pone a nulo el campo) y sigue con el cuerpo de ~CTask (0x61A3A0,
// CTask::Destructor, con hook). El destructor generado por el compilador dejaria la vtable de la DLL.
__declspec(noinline) void __fastcall SitInChairDestructorBody(CTaskInteriorSitInChair* self, void* /*edx*/) {
    *reinterpret_cast<uint32*>(self) = SIT_IN_CHAIR_VTABLE;
    if (auto* const anim = SitInChairField<CAnimBlendAssociation*>(self, 0x14)) {
        anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
    }
    self->CTask::Destructor();
}

// 0x675CF0 (thiscall, sin argumentos de pila, `ret`, 106 bytes con marco SEH que solo libera la memoria si el constructor lanza (se omite)): Clone del EXE (ranura 1). CTask::operator new (0x61A5A0) con 0x2C bytes;
// sin memoria devuelve nulo; si no, llama al constructor (0x675C30, con hook) con la informacion (+8), la duracion (+0xC) y el modo (+0x10, byte sin signo) y devuelve lo que el constructor deja en EAX (la direccion reservada).
CTask* __fastcall SitInChairClone(CTaskInteriorSitInChair* self, void* /*edx*/) {
    void* const memory = CTask::operator new(sizeof(CTaskInteriorSitInChair));
    if (!memory) {
        return nullptr;
    }
    auto* const clone = static_cast<CTaskInteriorSitInChair*>(memory);
    return reinterpret_cast<CTask*>(static_cast<uintptr_t>(clone->Constructor(
        reinterpret_cast<void*>(static_cast<uintptr_t>(SitInChairField<uint32>(self, 0x08))),
        SitInChairField<uint32>(self, 0x0C),
        SitInChairField<uint8>(self, 0x10)
    )));
}

// 0x675D60 (thiscall, tres argumentos de pila, `ret 0xC`, 99 bytes): MakeAbortable del EXE (ranura 2). Con prioridad distinta de INMEDIATA (2) solo pide la parada (byte +0x2A = 1) y devuelve falso. Con prioridad
// inmediata: si el IKChainManager (0xC15448) esta mirando al peaton (IsLooking, 0x6181A0), AbortLookAt con 250 ms (0x618280); si hay animacion (+0x14), pone su m_BlendDelta (+0x1C) a -1000.0 (0xC47A0000),
// le pone el callback de borrado por defecto (0x4CEBC0) y borra el campo; devuelve verdadero. El evento no se usa.
bool __fastcall SitInChairMakeAbortable(CTaskInteriorSitInChair* self, void* /*edx*/, CPed* ped, eAbortPriority priority, const CEvent* /*event*/) {
    if (priority != ABORT_PRIORITY_IMMEDIATE) {
        SitInChairField<uint8>(self, 0x2A) = 1;
        return false;
    }
    if (g_ikChainMan.IsLooking(ped)) {
        g_ikChainMan.AbortLookAt(ped, 250);
    }
    if (auto* const anim = SitInChairField<CAnimBlendAssociation*>(self, 0x14)) {
        anim->m_BlendDelta = -1000.0f;
        anim->SetDeleteCallback(CDefaultAnimCallback::DefaultAnimCB, nullptr);
        SitInChairField<CAnimBlendAssociation*>(self, 0x14) = nullptr;
    }
    return true;
}
} // namespace

void CTaskInteriorSitInChair::InjectHooks() {
    RH_ScopedClass(CTaskInteriorSitInChair);
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x675C80);
    RH_ScopedInstall(Constructor, 0x675C30);
    RH_ScopedNamedGlobalInstall(SitInChairGetTaskType, "GetTaskType", 0x675C80);         // lote 176A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairDestructorBody, "Destructor", 0x675C90);       // lote 176A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairClone, "Clone", 0x675CF0);                     // lote 176A (sin validar)
    RH_ScopedNamedGlobalInstall(SitInChairMakeAbortable, "MakeAbortable", 0x675D60);     // lote 176A (sin validar)
}

// Como el original: conserva la vtable EXE para los demás métodos. Deja EAX = this, ECX = (info con el byte bajo = modo) y EDX = duracion.
#pragma runtime_checks("", off)
uint32 CTaskInteriorSitInChair::Constructor(void* info, uint32 duration, uint8 mode) {
    auto* const fields = reinterpret_cast<uint8*>(this);
    *reinterpret_cast<uint32*>(fields + 0x04) = 0;
    *reinterpret_cast<uint32*>(fields + 0x00) = 0x870314;
    *reinterpret_cast<uint32*>(fields + 0x1C) = 0;
    *reinterpret_cast<uint32*>(fields + 0x20) = 0;
    fields[0x24] = 0;
    fields[0x25] = 0;
    *reinterpret_cast<uint32*>(fields + 0x08) = static_cast<uint32>(reinterpret_cast<uintptr_t>(info));
    *reinterpret_cast<uint32*>(fields + 0x14) = 0;
    fields[0x29] = 0;
    fields[0x28] = 0;
    fields[0x2A] = 0;
    *reinterpret_cast<uint32*>(fields + 0x0C) = duration;
    fields[0x10] = mode;
    *reinterpret_cast<uint32*>(fields + 0x18) = 0xFFFFFFFFu;
    const uint32 infoBits = static_cast<uint32>(reinterpret_cast<uintptr_t>(info));
    return ResidueRegsCpp((infoBits & 0xFFFFFF00u) | mode, duration, static_cast<uint32>(reinterpret_cast<uintptr_t>(this)));
}
#pragma runtime_checks("", restore)
