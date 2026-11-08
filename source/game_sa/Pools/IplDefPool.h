/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CIplDefPool
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
#include <IplDef.h>

/**
 * Pool especializado para definiciones IPL cargadas por el mundo.
 *
 * Hereda la administración de slots, handles y reutilización de memoria de CPool;
 * esta clase fija el tipo almacenado y conecta sus operaciones con los hooks del juego.
 * Los objetos obtenidos continúan perteneciendo al pool hasta que se liberan mediante
 * la interfaz correspondiente de CPool.
 */
class CIplDefPool : public CPool<IplDef> {
public:
    /**
     * Registra las operaciones específicas de CIplDefPool en sus direcciones originales.
     * La lógica común de administración de memoria permanece centralizada en CPool.
     */
    static void InjectHooks() {
        RH_ScopedClass(CIplDefPool);
        RH_ScopedCategory("Pools");

        //RH_ScopedInstall(Constructor, 0x405900);
        RH_ScopedInstall(New, 0x004059B0);
        RH_ScopedInstall(ScalarDeletingDestructor, 0x00405A60); // lote 130A (banco Fn130A)
    }

public:
    using CPool::CPool;

    // 0x4059B0: the original policy (search after the cursor, storage left untouched)
    IplDef* New() {
        return NewLegacy();
    }

    // 0x405A60 (lote 130A, banco Fn130A): destructor con borrado del pool del EXE (thiscall, un argumento de pila, `ret 4`). Como el original: si el tamano (+8, con signo) es > 0 libera con delete[]
    // (0x8213AE) el almacenamiento (+0) y despues los estados (+4) SOLO si el byte propietario (+0x10) no es cero, y pone a 0 +0, +4, +8 y +0xC (el byte propietario NO se toca); despues, si el bit 0 del
    // BYTE de banderas esta puesto, operator delete (0x8214BD) sobre this. Devuelve this.
    CIplDefPool* ScalarDeletingDestructor(uint8 flags) {
        auto* const raw = reinterpret_cast<uint8*>(this);
        if (*reinterpret_cast<int32*>(raw + 0x8) > 0) {
            if (*(raw + 0x10) != 0) {
                ::operator delete[](*reinterpret_cast<void**>(raw + 0x0));
                ::operator delete[](*reinterpret_cast<void**>(raw + 0x4));
            }
            *reinterpret_cast<void**>(raw + 0x0) = nullptr;
            *reinterpret_cast<void**>(raw + 0x4) = nullptr;
            *reinterpret_cast<int32*>(raw + 0x8) = 0;
            *reinterpret_cast<int32*>(raw + 0xC) = 0;
        }
        if (flags & 1) {
            ::operator delete(this);
        }
        return this;
    }
};
