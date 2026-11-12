#include "StdInc.h"

#include "TaskComplexKillAllThreats.h"

namespace CTaskComplexBody {
void* __fastcall Constructor(void* self, void* edx); // Task.cpp: 0x61A3B0
}

namespace {
// 0x623750 (lote 170A, banco Fn170A; el listado lo atribuia a CTaskSimpleGangDriveBy): Clone de CTaskComplexKillAllThreats del EXE (ranura 1 de su vtable 0x86DA1C; thiscall, sin argumentos de pila, `ret`, 106 bytes con marco
// SEH que solo desenrolla). La clase aun no esta reconstruida (sin campos): se leen los tres dwords +0xC, +0x10 y +0x14 en crudo, como el EXE (que los guarda en registros ANTES de construir). Como el original: reserva 0x18 bytes
// con CTask::operator new (0x61A5A0, con hook); sin memoria devuelve nulo; si no, base CTaskComplex (0x61A3B0, con hook), +0x10 y +0x14 copiados, vtable de la clase del EXE (0x86DA1C), +0xC copiado, y devuelve el objeto
// nuevo. Se engancha como S (la clase aun no tiene una declaracion virtual que nombre la ranura).
CTask* __fastcall CTaskComplexKillAllThreatsCloneExe(const CTaskComplexKillAllThreats* self, void* /*edx*/) {
    const uint8* const source = reinterpret_cast<const uint8*>(self);
    const uint32 field0C = *reinterpret_cast<const uint32*>(source + 0xC);
    const uint32 field10 = *reinterpret_cast<const uint32*>(source + 0x10);
    const uint32 field14 = *reinterpret_cast<const uint32*>(source + 0x14);
    void* const memory = CTask::operator new(0x18);
    if (!memory) {
        return nullptr;
    }
    CTaskComplexBody::Constructor(memory, nullptr);
    uint8* const bytes = static_cast<uint8*>(memory);
    *reinterpret_cast<uint32*>(bytes + 0x10) = field10;
    *reinterpret_cast<uint32*>(bytes + 0x14) = field14;
    *reinterpret_cast<uint32*>(bytes) = 0x86DA1C;
    *reinterpret_cast<uint32*>(bytes + 0xC) = field0C;
    return static_cast<CTask*>(memory);
}
} // namespace

void CTaskComplexKillAllThreats::InjectHooks() {
    // vtable 0x86DA1C (GetTaskType en [4], 11 ranuras; N por confirmar): al no reconstruir la clase se usa RH_ScopedClass y un enganche S de la entrada del EXE.
    RH_ScopedClass(CTaskComplexKillAllThreats);
    RH_ScopedCategory("Tasks/TaskTypes");

    RH_ScopedNamedGlobalInstall(CTaskComplexKillAllThreatsCloneExe, "Clone", 0x623750); // lote 170A (banco Fn170A)
    // RH_ScopedVMTInstall(GetTaskType, 0x6221B0);
    // TODO: instalar las funciones conforme se reconstruyan.
}
