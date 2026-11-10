// Sin PCH (CMakeLists.txt): StdInc.h redefine NOMINMAX y el aviso C4005 es error; mismo recurso que Event.cpp y Task.cpp.
#pragma warning(push, 0)
#include "StdInc.h"
#pragma warning(pop)

#include "PedGroupDefaultTaskAllocators.h"

// 0x5F7200 (lote 151A, banco Fn151A): ver la declaracion en la cabecera. El EXE solo escribe EAX y ECX (EDX y los registros no volatiles quedan intactos): el codigo no puede llevar marco ni comprobaciones, asi que se
// optimiza por pragma, que solo actua con /Zi y sin comprobaciones de ejecucion (CMakeLists.txt). La comparacion es sin signo (`ja`): cualquier valor mayor que 5, incluidos los negativos, devuelve 0.
#pragma runtime_checks("", off)
#pragma optimize("gty", on)
const CPedGroupDefaultTaskAllocator* CPedGroupDefaultTaskAllocators::GetExe(uint32 type) {
    switch (type) {
    case 0:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC09908); // FOLLOW_ANY_MEANS
    case 1:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC0990C); // FOLLOW_LIMITED
    case 2:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC09910); // STAND_STILL
    case 3:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC09914); // CHAT
    case 4:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC0991C); // SIT_IN_LEADER_CAR
    case 5:  return reinterpret_cast<const CPedGroupDefaultTaskAllocator*>(0xC09918); // RANDOM
    default: return nullptr;
    }
}
#pragma optimize("", on)
#pragma runtime_checks("", restore)
