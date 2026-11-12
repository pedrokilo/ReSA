#pragma once

// Libera la ranura de `obj` en un pool del EXE con el diseno de CPool (+0 almacenamiento, +4 estados, +0xC cursor del ultimo libre), exactamente como el operator delete en linea del EXE
// (CBuilding 0x4040A0, CDummy 0x5326F0, CPatrolRoute 0x41B820): indice = (obj - almacenamiento) / tamano del elemento (division con signo, truncada hacia cero, sin comprobar que `obj` pertenezca al pool);
// estados[indice] |= 0x80 (bit 7 = libre, el id de 7 bits no cambia) y, si indice < cursor (con signo), cursor = indice. No comprueba nulo ni rellena el almacenamiento.
inline void PoolFreeSlotRaw(void* pool, const void* obj, int32 elementSize) {
    auto* const raw     = static_cast<uint8*>(pool);
    const int32 index   = int32(reinterpret_cast<uintptr_t>(obj) - reinterpret_cast<uintptr_t>(*reinterpret_cast<uint8**>(raw + 0x0))) / elementSize;
    *(*reinterpret_cast<uint8**>(raw + 0x4) + index) |= 0x80;
    if (index < *reinterpret_cast<int32*>(raw + 0xC)) {
        *reinterpret_cast<int32*>(raw + 0xC) = index;
    }
}

// Lote 169A: igual que PoolFreeSlotRaw para elementos cuyo tamano es una potencia de dos (CTask: 0x80 = 1 << 7), como lo hacen las liberaciones EN LINEA del EXE (0x61A620, 0x61A660, 0x61A5B0): el indice es el
// desplazamiento aritmetico (`sar`, division hacia menos infinito, no truncada hacia cero) de la diferencia de 32 bits entre el objeto y el almacenamiento del pool.
inline void PoolFreeSlotRawShift(void* pool, const void* obj, int32 shift) {
    auto* const raw     = static_cast<uint8*>(pool);
    const int32 index   = int32(reinterpret_cast<uintptr_t>(obj) - reinterpret_cast<uintptr_t>(*reinterpret_cast<uint8**>(raw + 0x0))) >> shift;
    *(*reinterpret_cast<uint8**>(raw + 0x4) + index) |= 0x80;
    if (index < *reinterpret_cast<int32*>(raw + 0xC)) {
        *reinterpret_cast<int32*>(raw + 0xC) = index;
    }
}
