/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CIplDefPool
 * Reconstrucción: COMPLETA — 4/4 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 4/4 entradas.
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

        RH_ScopedInstall(Constructor, 0x405900); // lote 161A (banco Fn161A)
        RH_ScopedInstall(DestructorBody, 0x405960); // lote 161A (banco Fn161A)
        RH_ScopedInstall(New, 0x004059B0);
        RH_ScopedInstall(ScalarDeletingDestructor, 0x00405A60); // lote 130A (banco Fn130A)
    }

public:
    using CPool::CPool;

    // 0x4059B0: the original policy (search after the cursor, storage left untouched)
    IplDef* New() {
        return NewLegacy();
    }

    // 0x405900 (lote 161A, banco Fn161A): constructor del pool del EXE (thiscall, dos argumentos de pila (capacidad con signo y nombre, que no se guarda), `ret 8`, 91 bytes). Como el original: reserva con
    // operator new[] (0x821195) capacidad * 0x34 (imul de 32 bits) y guarda el puntero en +0, reserva `capacidad` bytes (como dword sin signo) y los guarda en +4; escribe el byte propietario (+0x10) a 1,
    // la capacidad (+8) y -1 (+0xC); si la capacidad es > 0 pone a 0x80 (ranura libre, id 0) cada estado, releyendo el puntero de estados (+4) en cada paso (no se guarda en un registro: si el almacenamiento
    // de estados pisa la cabecera, el bucle sigue al puntero nuevo). No rellena el almacenamiento ni toca +0x11..+0x13. Devuelve this.
    CIplDefPool* Constructor(int32 size, const char* /*name*/) {
        auto* const raw = reinterpret_cast<uint8*>(this);
        *reinterpret_cast<void**>(raw + 0x0) = ::operator new[](static_cast<size_t>(static_cast<uint32>(size) * 0x34u));
        *reinterpret_cast<void**>(raw + 0x4) = ::operator new[](static_cast<size_t>(static_cast<uint32>(size)));
        *(raw + 0x10) = 1;
        *reinterpret_cast<int32*>(raw + 0x8) = size;
        *reinterpret_cast<int32*>(raw + 0xC) = -1;
        for (int32 i = 0; i < size; ++i) {
            *(*reinterpret_cast<uint8**>(raw + 0x4) + i) |= 0x80;
            *(*reinterpret_cast<uint8**>(raw + 0x4) + i) &= 0x80;
        }
        return this;
    }

    // 0x405960 (lote 161A, banco Fn161A; sin llamadores: copia fuera de linea de la parte de liberacion de 0x405A60, thiscall, sin argumentos, `ret`, 54 bytes): si el tamano (+8, con signo) es > 0 libera con
    // delete[] (0x8213AE) el almacenamiento (+0) y despues los estados (+4) SOLO si el byte propietario (+0x10) no es cero, y pone a 0 +0, +4, +8 y +0xC (el byte propietario NO se toca).
    void DestructorBody() {
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
    }

    // 0x405A60 (lote 130A, banco Fn130A): destructor con borrado del pool del EXE (thiscall, un argumento de pila, `ret 4`). Como el original: la liberacion de DestructorBody (0x405960, en linea en el EXE)
    // y, si el bit 0 del BYTE de banderas esta puesto, operator delete (0x8214BD) sobre this. Devuelve this.
    CIplDefPool* ScalarDeletingDestructor(uint8 flags) {
        DestructorBody();
        if (flags & 1) {
            ::operator delete(this);
        }
        return this;
    }
};
