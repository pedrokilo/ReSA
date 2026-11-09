#pragma once

#include "Pool.h"

class CPatrolRoutePool : public CPool<void*> {
public:
    static void InjectHooks() {
        RH_ScopedClass(CPatrolRoutePool);
        RH_ScopedCategory("Pools");

        RH_ScopedInstall(New, 0x41B660); // lote 132A (banco Fn132A)
    }

public:
    using CPool::CPool;

    // 0x41B660 (lote 132A, banco Fn132A): New del pool de rutas de patrulla del EXE (thiscall, sin argumentos, `ret`). Los elementos miden 0x1A4 bytes (el CPool<void*> de la DLL no los conoce):
    // se trabaja sobre el diseno del EXE: +0 objetos, +4 estados (bit 7 = libre, bits 0..6 = id), +8 tamano, +0xC cursor. Como el original: avanza el cursor una posicion ANTES de mirar y lo guarda; al
    // llegar al tamano (igualdad) lo pone a 0 y, si ya habia dado la vuelta, devuelve nulo; si no, da la vuelta (una sola vez). Con el primer estado libre (bit 7) lo marca ocupado e incrementa su id de
    // 7 bits (modulo 128, sin tocar el almacenamiento) y devuelve objetos + cursor * 0x1A4. Devuelve nulo (0) tambien en el caso de pool lleno.
    void* New() {
        auto* const raw = reinterpret_cast<uint8*>(this);
        bool wrapped = false;
        for (;;) {
            auto& cursor = *reinterpret_cast<int32*>(raw + 0xC);
            cursor = cursor + 1;
            if (cursor == *reinterpret_cast<int32*>(raw + 0x8)) {
                cursor = 0;
                if (wrapped) {
                    return nullptr;
                }
                wrapped = true;
            }
            auto* const states = *reinterpret_cast<uint8**>(raw + 0x4);
            uint8& state = states[*reinterpret_cast<int32*>(raw + 0xC)];
            if (state & 0x80) {
                state = static_cast<uint8>(((state & 0x7F) + 1) & 0x7F);
                return *reinterpret_cast<uint8**>(raw + 0x0) + static_cast<int32>(*reinterpret_cast<int32*>(raw + 0xC)) * 0x1A4;
            }
        }
    }
};
