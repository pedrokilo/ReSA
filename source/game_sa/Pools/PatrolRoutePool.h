/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPatrolRoutePool
 * Reconstrucción: COMPLETA — 1/1 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 1/1 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

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
    __declspec(noinline) void* New() { // sin expansion en linea: lo llama CPatrolRoute::operator new (lote 146A) y el banco puede sustituirlo
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
