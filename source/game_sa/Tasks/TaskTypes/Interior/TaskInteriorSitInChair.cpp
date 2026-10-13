#include "StdInc.h"
#include "X87Math.h"

#include "TaskInteriorSitInChair.h"

void CTaskInteriorSitInChair::InjectHooks() {
    RH_ScopedClass(CTaskInteriorSitInChair);
    RH_ScopedCategory("Tasks/TaskTypes");

    // RH_ScopedVMTInstall(GetTaskType, 0x675C80);
    RH_ScopedInstall(Constructor, 0x675C30);
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
