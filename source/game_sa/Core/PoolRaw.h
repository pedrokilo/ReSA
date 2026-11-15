#pragma once

#include <Base.h>

namespace notsa::pool_raw {
// Liberacion de una ranura de un CPool<T> exactamente como el EXE (CPool<T>::Delete 0x5A73C0, CVehicleStructure::operator delete 0x4C9580 y los borrados en linea de los destructores con borrado): indice =
// (puntero - datos) / tamano del elemento (division con signo, hacia cero; sin comprobar que el puntero pertenezca al pool ni que la ranura estuviera ocupada), estado |= 0x80 y cursor de la ultima ranura libre =
// min con signo (cursor, indice). CPool::Delete de la DLL comprueba ambas cosas con `assert` y por eso no vale para reproducir el EXE con entradas fuera del uso normal. Disposicion de CPool<T> (0x14 bytes):
// +0 datos, +4 estados, +8 capacidad, +0xC cursor, +0x10 BYTE propietario, +0x11 BYTE sin memoria.
struct PoolLayout {
    uint8* storage;
    uint8* states;
    int32  capacity;
    int32  lastFree;
};

inline void ReleaseSlot(void* pool, const void* obj, int32 elementSize) {
    auto* const p = static_cast<PoolLayout*>(pool);
    const int32 index = static_cast<int32>(reinterpret_cast<uintptr_t>(obj) - reinterpret_cast<uintptr_t>(p->storage)) / elementSize;
    p->states[index] |= 0x80;
    if (index < p->lastFree) {
        p->lastFree = index;
    }
}
} // namespace notsa::pool_raw
