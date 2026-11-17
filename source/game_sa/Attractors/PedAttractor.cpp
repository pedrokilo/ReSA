/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: CPedAttractor
 * Reconstrucción: COMPLETA — 85/85 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 51/85 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#include "StdInc.h"

#include "PedAttractor.h"

#include "Tasks/TaskTypes/TaskComplexGoToAttractor.h"
#include "Tasks/TaskTypes/TaskComplexWaitAtAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUseAttractor.h"
#include "Tasks/TaskTypes/TaskComplexUsePairedAttractor.h"
#include "Tasks/TaskTypes/TaskSimpleStandStill.h"
#include "Pools/PoolRawDelete.h"

namespace {
struct SArrayElem20 { uint32 w[5]; };
struct SArrayElem12 { uint32 w[3]; };

// Lote 205A (sin validar): 15 funciones mas del mismo bloque de plantillas de la STL de MSVC 6 (0x5EB140..0x5EB3B2, cdecl, sin llamadores fuera de las instancias de SArray; son plantillas de la STL, no metodos de CPedAttractor).
// Mismas convenciones que arriba: iteradores como punteros por valor, valor por puntero, retorno de estructura en el primer argumento. Los llamadores empujan argumentos de relleno de la plantilla (etiquetas de categoria) que estas
// funciones no leen; con cdecl el llamador los limpia, asi que basta declarar los que se leen.
// std::copy_backward de 4 bytes (0x5EB140, 0x5EB1B0 y 0x5EB260, tres instancias identicas, 40 bytes cada una): si inicio == fin no copia; *out = inicio del destino; devuelve out.
template<int Id>
uint32** __cdecl SArrayCopyBackwardStride4(uint32** out, uint32* first, uint32* last, uint32* destEnd) {
    if (first != last) {
        do {
            const uint32 value = last[-1];
            --last;
            --destEnd;
            *destEnd = value;
        } while (last != first);
    }
    *out = destEnd;
    return out;
}

// Envoltorios de copy_backward (0x5EB170 de 20 bytes y 0x5EB220 de 12 bytes, 50 bytes cada uno): reenvian (out, first, last, destEnd) y repiten destEnd como quinto argumento (el EXE empuja cinco) a la funcion de arriba
// (0x5EAC50 / 0x5EACC0, con hook) y devuelven out (en ESI). El quinto argumento no lo lee la funcion, asi que aqui se omite.
SArrayElem20** __cdecl SArrayCopyBackwardStride20Wrapper(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* destEnd) {
    // Colaborador del lote 189A (sin hook en esta base): se llama por su direccion del EXE.
    reinterpret_cast<SArrayElem20** (__cdecl*)(SArrayElem20**, SArrayElem20*, SArrayElem20*, SArrayElem20*)>(0x5EAC50)(out, first, last, destEnd);
    return out;
}
SArrayElem12** __cdecl SArrayCopyBackwardStride12Wrapper(SArrayElem12** out, SArrayElem12* first, SArrayElem12* last, SArrayElem12* destEnd) {
    // Colaborador del lote 189A (sin hook en esta base): se llama por su direccion del EXE.
    reinterpret_cast<SArrayElem12** (__cdecl*)(SArrayElem12**, SArrayElem12*, SArrayElem12*, SArrayElem12*)>(0x5EACC0)(out, first, last, destEnd);
    return out;
}

// std::fill de 12 bytes (0x5EB1E0, 50 bytes): lee el valor en cada vuelta (tres dwords en orden); con inicio == fin no lo lee.
void __cdecl SArrayFillStride12(SArrayElem12* first, SArrayElem12* last, const SArrayElem12* value) {
    for (; first != last; ++first) {
        first->w[0] = value->w[0];
        first->w[1] = value->w[1];
        first->w[2] = value->w[2];
    }
}

// std::uninitialized_fill_n de 4 bytes (0x5EB2A0, 0x5EB310 y 0x5EB390, tres instancias identicas, 35 bytes cada una): con n == 0 (comparacion SIN signo, `jbe`) no hace nada; si no, n vueltas que escriben el valor (releido en cada
// vuelta) solo si el destino no es nulo (new(puntero) T(valor)), avanzando siempre.
template<int Id>
void __cdecl SArrayFillNStride4(uint32* dest, uint32 count, const uint32* value) {
    if (count == 0) {
        return;
    }
    do {
        if (dest) {
            *dest = *value;
        }
        ++dest;
        --count;
    } while (count != 0);
}

// Igual con elementos de 20 bytes (0x5EB2E0, 46 bytes; `rep movsd` de 5 dwords) y de 12 bytes (0x5EB340, 55 bytes; tres dwords en orden).
void __cdecl SArrayFillNStride20(SArrayElem20* dest, uint32 count, const SArrayElem20* value) {
    if (count == 0) {
        return;
    }
    do {
        if (dest) {
            for (int32 i = 0; i < 5; i++) {
                dest->w[i] = value->w[i];
            }
        }
        ++dest;
        --count;
    } while (count != 0);
}
void __cdecl SArrayFillNStride12(SArrayElem12* dest, uint32 count, const SArrayElem12* value) {
    if (count == 0) {
        return;
    }
    do {
        if (dest) {
            dest->w[0] = value->w[0];
            dest->w[1] = value->w[1];
            dest->w[2] = value->w[2];
        }
        ++dest;
        --count;
    } while (count != 0);
}
} // namespace

namespace {
// Lote 189A (sin validar): 12 funciones de plantilla de la STL de MSVC 6 (std::fill, std::uninitialized_copy, std::copy y std::copy_backward) que el EXE no expande en linea en el codigo de los vectores de los atractores
// (0x5EAAA0..0x5EAF04, cdecl, sin llamadores fuera de las instancias de SArray). El listado las atribuia a CPedAttractorPool; son el mismo bloque que SArraySizeStride20/12 (0x5E9800, PedShelterAttractor.cpp).
// Los iteradores son punteros (4 bytes) pasados por valor; el valor a rellenar se pasa por puntero; las dos copias que devuelven un iterador lo escriben en el puntero que reciben primero (retorno de estructura) y lo devuelven.
// Cada direccion de hook tiene su propia instancia de plantilla (RH no admite dos hooks hacia la misma funcion de la DLL: el cuerpo de la segunda y la tercera se resolvia a la entrada del EXE de la primera; lo detecto el banco Fn189A).
// Cada direccion de hook tiene su propia instancia de plantilla (RH no admite dos hooks hacia la misma funcion de la DLL: el cuerpo de la segunda y la tercera se resolvia a la entrada del EXE de la primera; lo detecto el banco Fn189A).
// Elementos de 4 bytes (SArray<puntero>), de 20 bytes (CPedTaskPair) y de 12 bytes (CVector). Las instancias identicas de distinto tipo de elemento tienen su propia direccion (A, B y C).

// std::fill: lee el valor en cada vuelta; con inicio == fin no lo lee. EAX queda en fin, nadie lo usa.
template<int Id>
void __cdecl SArrayFillStride4(uint32* first, uint32* last, const uint32* value) {
    for (; first != last; ++first) {
        *first = *value;
    }
}
void __cdecl SArrayFillStride20(SArrayElem20* first, SArrayElem20* last, const SArrayElem20* value) {
    for (; first != last; ++first) {
        *first = *value;
    }
}

// std::uninitialized_copy: construye en el destino solo si no es nulo (new(puntero) T(*origen)); devuelve el final del destino en EAX.
template<int Id>
uint32* __cdecl SArrayUninitCopyStride4(const uint32* first, const uint32* last, uint32* dest) {
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
    }
    return dest;
}
SArrayElem20* __cdecl SArrayUninitCopyStride20(const SArrayElem20* first, const SArrayElem20* last, SArrayElem20* dest) {
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
    }
    return dest;
}
SArrayElem12* __cdecl SArrayUninitCopyStride12(const SArrayElem12* first, const SArrayElem12* last, SArrayElem12* dest) {
    for (; first != last; ++first, ++dest) {
        if (dest) {
            *dest = *first;
        }
    }
    return dest;
}

// std::copy (hacia delante, con las mismas comprobaciones de rango vacio que el EXE): *out = final del destino; devuelve out.
SArrayElem20** __cdecl SArrayCopyStride20(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* dest) {
    if (first == last) {
        *out = dest;
        return out;
    }
    do {
        *dest = *first;
        ++first;
        ++dest;
    } while (first != last);
    *out = dest;
    return out;
}

// std::copy_backward: recorre desde el final; *out = inicio del destino; devuelve out.
SArrayElem20** __cdecl SArrayCopyBackwardStride20(SArrayElem20** out, SArrayElem20* first, SArrayElem20* last, SArrayElem20* destEnd) {
    if (first == last) {
        *out = destEnd;
        return out;
    }
    do {
        --last;
        --destEnd;
        *destEnd = *last;
    } while (last != first);
    *out = destEnd;
    return out;
}
SArrayElem12** __cdecl SArrayCopyBackwardStride12(SArrayElem12** out, SArrayElem12* first, SArrayElem12* last, SArrayElem12* destEnd) {
    if (first == last) {
        *out = destEnd;
        return out;
    }
    do {
        --last;
        --destEnd;
        *destEnd = *last;
    } while (last != first);
    *out = destEnd;
    return out;
}

// Lote 209A (sin validar): 0x4F3D80 (39 bytes, thiscall con ECX sin leer, tres argumentos de pila, `ret 0Ch`; el listado la atribuia a CAEUserRadioTrackManager): envoltorio de uninitialized_fill_n de elementos de 4 bytes (mismos bytes que 0x5EC070, 0x5EC110
// y 0x5EC1B0, sin enganchar). Reenvia (destino, cantidad, valor) a la funcion cdecl de arriba (0x5EB2A0 y gemelas; el EXE empuja cinco argumentos, tres reales y dos de relleno que no se leen) y devuelve destino + cantidad * 4 (EAX = EDI + ESI * 4),
// con la cantidad SIN signo y la aritmetica de punteros del EXE (envuelve a 32 bits).
uint32* __stdcall SArrayFillNReturnEndStride4(uint32* dest, uint32 count, const uint32* value) {
    SArrayFillNStride4<0>(dest, count, value);
    return dest + count;
}

// Lote 219A (sin validar; el listado atribuia estas funciones a CPedAttractorManager): mas instancias de la STL de MSVC 6 junto a los vectores de CPedAttractor y de CPedAttractorManager (0x5EC040..0x5ED4B0 son plantillas de
// std::vector, no metodos de la clase). Mismas convenciones que arriba. Las tres variantes de `fill_n que devuelve el final` (0x5EC070, 0x5EC110, 0x5EC1B0) son del mismo cuerpo que 0x4F3D80 y los nueve vaciados (0x5EC040,
// 0x5EC0B0, 0x5EC180, 0x5EC1F0, 0x5EC260, 0x5EC2D0, 0x5EC340, 0x5EC3B0, 0x5EC3E0) del mismo cuerpo que 0x5EC820: se enganchan con las funciones C++ de esos (comprobado instruccion por instruccion contra el desensamblado, salvo
// destinos de llamada y la direccion del manejador SEH).
// Disposicion de un vector de la STL de MSVC 6 (como SArray): asignador (+0, sin leer), inicio (+4), fin (+8) y limite de capacidad (+0xC).
template<typename T>
struct SArrayRaw {
    void* allocator;
    T*    first;
    T*    last;
    T*    end;
};

// 0x5EC0E0 (42 bytes) y 0x5EC140 (42 bytes), thiscall con ECX sin leer, tres argumentos de pila, `ret 0Ch`: igual que 0x5EC070 pero con elementos de 20 y de 12 bytes. Llaman a uninitialized_fill_n (0x5EB2E0 / 0x5EB340) y
// devuelven destino + cantidad * tamano (EAX = EDI + (ESI * 5) * 4, o EDI + (ESI * 3) * 4), con la cantidad SIN signo y la aritmetica de punteros del EXE (envuelve a 32 bits).
SArrayElem20* __stdcall SArrayFillNReturnEndStride20(SArrayElem20* dest, uint32 count, const SArrayElem20* value) {
    SArrayFillNStride20(dest, count, value);
    return dest + count;
}
SArrayElem12* __stdcall SArrayFillNReturnEndStride12(SArrayElem12* dest, uint32 count, const SArrayElem12* value) {
    SArrayFillNStride12(dest, count, value);
    return dest + count;
}

// 0x5EC220 (49 bytes, thiscall con (salida, posicion) en la pila, `ret 8`): std::vector<T*>::erase(posicion) de elementos de 4 bytes (la devolucion del iterador por puntero de salida es la de la STL de MSVC 6).
// Como el original: el fin se lee UNA vez antes del bucle; con posicion + 1 != fin copia hacia delante cada elemento sobre el anterior (do-while: con posicion == fin no termina, igual que el EXE); despues el fin (+8) baja 4 bytes
// (releido) y se escribe `*salida = posicion`. Devuelve la salida (EAX). No comprueba rango ni destruye nada.
uint32** __fastcall SArrayEraseStride4(SArrayRaw<uint32>* self, void* /*edx*/, uint32** out, uint32* where) {
    uint32* const last = self->last;
    uint32* src        = where + 1;
    if (src != last) {
        do {
            src[-1] = *src;
            ++src;
        } while (src != last);
    }
    self->last = reinterpret_cast<uint32*>(reinterpret_cast<uint32>(self->last) - 4);
    *out       = where;
    return out;
}

// Direcciones de los colaboradores de cada instancia de insert(posicion, n, valor) de elementos de 4 bytes (todas son plantillas de la STL ya reconstruidas arriba, con hook; se llaman por direccion fija igual que en el EXE
// para que el registro de llamadas de los bancos las vea). A: 0x5ECF10; B: 0x5ED4B0 (y la de CAEUserRadioTrackManager, 0x4F4320, tiene el mismo cuerpo con otros colaboradores).
struct SArrayInsertInstanceA {
    static constexpr unsigned kUninitCopy = 0x5EAE10; // uninitialized_copy (cdecl: origen, fin, destino)
    static constexpr unsigned kFillN      = 0x5EB2A0; // uninitialized_fill_n (cdecl: destino, n, &valor)
    static constexpr unsigned kCopyWrap   = 0x5EB930; // envoltorio stdcall de uninitialized_copy
    static constexpr unsigned kFillNEnd   = 0x5EC070; // fill_n que devuelve el final (stdcall)
    static constexpr unsigned kCopyBack   = 0x5EB140; // copy_backward (cdecl: salida, inicio, fin, finDestino)
    static constexpr unsigned kFill       = 0x5EAAA0; // fill (cdecl: inicio, fin, &valor)
    static constexpr unsigned kXlen       = 0x5ECAB0; // vector::_Xlen
};
struct SArrayInsertInstanceB {
    static constexpr unsigned kUninitCopy = 0x5EAE70;
    static constexpr unsigned kFillN      = 0x5EB310;
    static constexpr unsigned kCopyWrap   = 0x5EB990;
    static constexpr unsigned kFillNEnd   = 0x5EC110;
    static constexpr unsigned kCopyBack   = 0x5EB1B0;
    static constexpr unsigned kFill       = 0x5EAAF0;
    static constexpr unsigned kXlen       = 0x5ECBB0;
};

// 0x5ECF10 y 0x5ED4B0 (lote 219A, sin validar; 645 bytes cada una con marco SEH que solo libera el bloque nuevo si una copia lanzara: omitido; thiscall con (posicion, cantidad, const T&), `ret 0Ch`): std::vector<T*>::insert(pos, n, valor)
// de MSVC 6/9 (vector de punteros de 4 bytes), mismo cuerpo que 0x4F4320 (UserTracksLegacyVector::Insert, lote 209A) con los colaboradores de cada instancia. Como el original:
//  - El valor se copia a una variable local al entrar (su direccion es el argumento de relleno de las llamadas). capacidad = inicio ? (limite - inicio) >> 2 : 0 (desplazamiento con signo). Con n == 0 no hace nada.
//  - tamano = inicio ? (fin - inicio) >> 2 : 0; si (0x3FFFFFFF - tamano) < n (sin signo): vector::_Xlen (no vuelve).
//  - Capacidad insuficiente (capacidad < tamano + n, sin signo): nueva = (0x3FFFFFFF - capacidad / 2 < capacidad) ? 0 : capacidad + capacidad / 2; si es menor que tamano + n, la nueva es tamano + n; reserva nueva * 4 con operator new (0x82119A);
//    copia [inicio, pos) (uninitialized_copy), rellena n con el valor (fill_n), copia [pos, fin) detras, libera el bloque viejo (0x8214BD) y coloca inicio, fin = inicio + (n + tamano anterior) y limite = inicio + nueva * 4.
//  - Con capacidad: si hay menos de n elementos tras pos, copia la cola a pos + n, rellena n - cola desde el fin (fill_n que devuelve el final), fin += n y rellena [pos, fin anterior); si no, copia los ultimos n a fin, fin = su retorno,
//    copia hacia atras [pos, fin anterior - n) hasta fin anterior y rellena [pos, pos + n).
template<typename Inst>
void __fastcall SArrayInsertNStride4(SArrayRaw<uint32>* self, void* /*edx*/, uint32* pos, uint32 count, const uint32* value) {
    using Self = SArrayRaw<uint32>*;
    const auto ElementsBetween = [](const uint32* upper, const uint32* lower) -> uint32 {
        return static_cast<uint32>(static_cast<int32>(reinterpret_cast<uint32>(upper) - reinterpret_cast<uint32>(lower)) >> 2);
    };
    const auto Advance = [](uint32* base, uint32 elements) -> uint32* { // aritmetica de punteros de 32 bits del EXE (elementos * 4, con envoltura)
        return reinterpret_cast<uint32*>(reinterpret_cast<uint32>(base) + elements * 4);
    };

    uint32 valueSlot = *value;
    const uint32 capacity = self->first ? ElementsBetween(self->end, self->first) : 0;
    if (count == 0) {
        return;
    }
    uint32 size = self->first ? ElementsBetween(self->last, self->first) : 0;
    if (0x3FFFFFFFu - size < count) {
        plugin::CallMethod<Inst::kXlen, Self>(self);
        return;
    }
    size = self->first ? ElementsBetween(self->last, self->first) : 0;
    if (capacity < size + count) {
        const uint32 half = capacity >> 1;
        uint32 newCapacity = (0x3FFFFFFFu - half < capacity) ? 0 : capacity + half;
        const uint32 needed = (self->first ? ElementsBetween(self->last, self->first) : 0) + count;
        if (newCapacity < needed) {
            newCapacity = needed;
        }
        const uint32 bytes = newCapacity << 2;
        uint32* const block = static_cast<uint32*>(::operator new(bytes));
        uint32* const afterPrefix = plugin::CallAndReturn<uint32*, Inst::kUninitCopy, uint32*, uint32*, uint32*>(self->first, pos, block);
        plugin::Call<Inst::kFillN, uint32*, uint32, const uint32*>(afterPrefix, count, &valueSlot);
        plugin::CallAndReturn<uint32*, Inst::kUninitCopy, uint32*, uint32*, uint32*>(pos, self->last, Advance(afterPrefix, count));
        const uint32 newSize = count + (self->first ? ElementsBetween(self->last, self->first) : 0);
        if (self->first) {
            ::operator delete(self->first);
        }
        const uint32 newEnd = bytes + reinterpret_cast<uint32>(block);
        uint32* const newLast = Advance(block, newSize);
        self->end   = reinterpret_cast<uint32*>(newEnd);
        self->last  = newLast;
        self->first = block;
        return;
    }

    uint32* const oldLast = self->last;
    const uint32 bytes = count << 2;
    if (ElementsBetween(oldLast, pos) < count) {
        plugin::CallMethodAndReturn<uint32*, Inst::kCopyWrap, Self, uint32*, uint32*, uint32*>(self, pos, oldLast, Advance(pos, count));
        const uint32 tail = ElementsBetween(self->last, pos);
        plugin::CallMethodAndReturn<uint32*, Inst::kFillNEnd, Self, uint32*, uint32, const uint32*>(self, self->last, count - tail, &valueSlot);
        self->last = reinterpret_cast<uint32*>(reinterpret_cast<uint32>(self->last) + bytes);
        plugin::Call<Inst::kFill, uint32*, uint32*, const uint32*>(pos, oldLast, &valueSlot);
    } else {
        uint32* const lastN = reinterpret_cast<uint32*>(reinterpret_cast<uint32>(oldLast) - bytes);
        self->last = plugin::CallMethodAndReturn<uint32*, Inst::kCopyWrap, Self, uint32*, uint32*, uint32*>(self, lastN, oldLast, oldLast);
        uint32* scratch;
        plugin::Call<Inst::kCopyBack, uint32**, uint32*, uint32*, uint32*>(&scratch, pos, lastN, oldLast);
        plugin::Call<Inst::kFill, uint32*, uint32*, const uint32*>(pos, Advance(pos, count), &valueSlot);
    }
}

// 0x5ED1B0 (lote 219A, sin validar; 739 bytes con marco SEH que solo libera el bloque nuevo si una copia lanzara: omitido; thiscall con (posicion, cantidad, const T&), `ret 0Ch`): std::vector<CPedTaskPair>::insert(pos, n, valor)
// (elementos de 20 bytes; vector m_PedTaskPairs de CPedAttractor). Mismo esquema que el de 4 bytes, con estas diferencias del EXE:
//  - El valor (20 bytes, `rep movsd`) se copia a una variable local al entrar, ANTES de comprobar n == 0; su direccion es el argumento de relleno.
//  - Los tamanos son cocientes CON signo entre 20 (imul 0x66666667; sar 3 y suma del signo) y el maximo es 0xCCCCCCC (0xFFFFFFFF / 20); vector::_Xlen es 0x5ECB30.
//  - Si la nueva capacidad no alcanza tamano + n, se toma `size()` (0x5E9800, llamada al EXE) + n, no una resta en linea.
//  - Colaboradores: uninitialized_copy 0x5EAE40, fill_n 0x5EB2E0, envoltorio de copia 0x5EB960, fill_n que devuelve el final 0x5EC0E0, envoltorio de copy_backward 0x5EB170 (salida por puntero) y fill 0x5EAAC0.
void __fastcall SArrayInsertNStride20(SArrayRaw<SArrayElem20>* self, void* /*edx*/, SArrayElem20* pos, uint32 count, const SArrayElem20* value) {
    using Elem = SArrayElem20;
    using Self = SArrayRaw<Elem>*;
    const auto ElementsBetween = [](const Elem* upper, const Elem* lower) -> uint32 {
        return static_cast<uint32>(static_cast<int32>(reinterpret_cast<uint32>(upper) - reinterpret_cast<uint32>(lower)) / 20);
    };
    const auto Advance = [](Elem* base, uint32 elements) -> Elem* { // aritmetica de punteros de 32 bits del EXE (elementos * 20, con envoltura)
        return reinterpret_cast<Elem*>(reinterpret_cast<uint32>(base) + elements * 20);
    };

    Elem valueLocal = *value;
    const uint32 capacity = self->first ? ElementsBetween(self->end, self->first) : 0;
    if (count == 0) {
        return;
    }
    uint32 size = self->first ? ElementsBetween(self->last, self->first) : 0;
    if (0xCCCCCCCu - size < count) {
        plugin::CallMethod<0x5ECB30, Self>(self);
        return;
    }
    size = self->first ? ElementsBetween(self->last, self->first) : 0;
    if (capacity < size + count) {
        const uint32 half = capacity >> 1;
        uint32 newCapacity = (0xCCCCCCCu - half < capacity) ? 0 : capacity + half;
        const uint32 needed = (self->first ? ElementsBetween(self->last, self->first) : 0) + count;
        if (newCapacity < needed) {
            newCapacity = static_cast<uint32>(plugin::CallMethodAndReturn<int32, 0x5E9800, Self>(self)) + count;
        }
        const uint32 bytes = newCapacity * 20;
        Elem* const block = static_cast<Elem*>(::operator new(bytes));
        Elem* const afterPrefix = plugin::CallAndReturn<Elem*, 0x5EAE40, Elem*, Elem*, Elem*>(self->first, pos, block);
        plugin::Call<0x5EB2E0, Elem*, uint32, const Elem*>(afterPrefix, count, &valueLocal);
        plugin::CallAndReturn<Elem*, 0x5EAE40, Elem*, Elem*, Elem*>(pos, self->last, Advance(afterPrefix, count));
        const uint32 newSize = count + (self->first ? ElementsBetween(self->last, self->first) : 0);
        if (self->first) {
            ::operator delete(self->first);
        }
        const uint32 newEnd = bytes + reinterpret_cast<uint32>(block);
        Elem* const newLast = Advance(block, newSize);
        self->end   = reinterpret_cast<Elem*>(newEnd);
        self->last  = newLast;
        self->first = block;
        return;
    }

    Elem* const oldLast = self->last;
    const uint32 bytes = count * 20;
    if (ElementsBetween(oldLast, pos) < count) {
        plugin::CallMethodAndReturn<Elem*, 0x5EB960, Self, Elem*, Elem*, Elem*>(self, pos, oldLast, Advance(pos, count));
        const uint32 tail = ElementsBetween(self->last, pos);
        plugin::CallMethodAndReturn<Elem*, 0x5EC0E0, Self, Elem*, uint32, const Elem*>(self, self->last, count - tail, &valueLocal);
        self->last = reinterpret_cast<Elem*>(reinterpret_cast<uint32>(self->last) + bytes);
        plugin::Call<0x5EAAC0, Elem*, Elem*, const Elem*>(pos, reinterpret_cast<Elem*>(reinterpret_cast<uint32>(self->last) - bytes), &valueLocal);
    } else {
        Elem* const lastN = reinterpret_cast<Elem*>(reinterpret_cast<uint32>(oldLast) - bytes);
        self->last = plugin::CallMethodAndReturn<Elem*, 0x5EB960, Self, Elem*, Elem*, Elem*>(self, lastN, oldLast, oldLast);
        Elem* scratch = lastN; // el EXE usa como salida la variable donde dejo lastN (la funcion la sobrescribe y nadie la lee)
        plugin::Call<0x5EB170, Elem**, Elem*, Elem*, Elem*>(&scratch, pos, lastN, oldLast);
        plugin::Call<0x5EAAC0, Elem*, Elem*, const Elem*>(pos, Advance(pos, count), &valueLocal);
    }
}

// Lote 206A (sin validar): envolturas stdcall de std::uninitialized_copy (0x5EB930, 0x5EB960, 0x5EB990, 0x5EB9D0 y 0x5EBA00; `ret 0Ch`, tres argumentos de pila (origen, fin, destino)). Cada una reserva en la pila un marco
// de 0x14 bytes con los tres argumentos y un hueco sin inicializar (el ECX de entrada, basura), llama a la funcion cdecl de la tabla de 189A (0x5EAE10 / 0x5EAE40 / 0x5EAE70 / 0x5EAEA0 / 0x5EAEE0), la limpia con
// `add esp, 14h` y devuelve su EAX (el final del destino). No usan `this`. Las tres de elemento de 4 bytes (0x5EB930, 0x5EB990 y 0x5EBA00) comparten cuerpo, como las funciones de 189A.
template<int Id>
uint32* __stdcall SArrayUninitCopyWrapStride4(const uint32* first, const uint32* last, uint32* dest) {
    return SArrayUninitCopyStride4<Id>(first, last, dest);
}
SArrayElem20* __stdcall SArrayUninitCopyWrapStride20(const SArrayElem20* first, const SArrayElem20* last, SArrayElem20* dest) {
    return SArrayUninitCopyStride20(first, last, dest);
}
SArrayElem12* __stdcall SArrayUninitCopyWrapStride12(const SArrayElem12* first, const SArrayElem12* last, SArrayElem12* dest) {
    return SArrayUninitCopyStride12(first, last, dest);
}

// 0x5EC820 (39 bytes, thiscall sin argumentos de pila, `ret`): liberar el almacenamiento de un vector SArray de elementos sin destructor (el inicio esta en +4, el fin en +8 y el limite en +0xC): si el inicio no es nulo
// lo libera con operator delete del EXE (0x8214BD) y pone los tres punteros a cero. Es el mismo trozo que el destructor de los vectores de CPedAttractor 0x5EC410 trae en linea.
void __fastcall SArrayTidyStride4(SArray<uint32>* self, void* /*edx*/) {
    // Como el EXE: sin recorrer los elementos (SArray::clear destruiria el rango [inicio, fin), que con un fin incoherente no termina); operator delete solo con el inicio no nulo y despues los tres punteros a cero
    if (self->_First) {
        ::operator delete(self->_First);
    }
    self->_First = nullptr;
    self->_Last  = nullptr;
    self->_End   = nullptr;
}

// Lote 209A: una instancia propia por direccion de hook (0x4F3F30 y 0x4F4030): RH no admite dos hooks hacia la misma funcion de la DLL (el cuerpo de la segunda se resolvia a la entrada del EXE de la primera; lo detecto el banco Fn209A).
template<int Id>
void __fastcall SArrayTidyStride4Inst(SArray<uint32>* self, void* edx) {
    SArrayTidyStride4(self, edx);
}

// 0x5EC4B0 (76 bytes, thiscall con el BYTE de banderas en la pila, `ret 4`): destructor con borrado de CPedAttractor del EXE (ranura 1 de su vtable 0x86C538, tras GetType). Llama al cuerpo del destructor (0x5EC410, con
// hook: aqui el destructor de la clase llamado sin pasar por la vtable) y, solo si el bit 0 del BYTE de banderas esta puesto, libera `this` en el pool de atractores (0xB744C4) EN LINEA (indice = (this - almacenamiento) / 0xC4 con signo,
// estado |= 0x80, cursor = min con signo), como CPedAttractor::operator delete (0x5EAFE0, con hook). Devuelve `this`. Se engancha como S (RH_ScopedVMTInstall no puede nombrar la ranura del destructor).
CPedAttractor* __fastcall PedAttractorScalarDeletingDestructor(CPedAttractor* self, void* /*edx*/, uint8 flags) {
    self->Destructor();
    if (flags & 1) {
        CPedAttractor::operator delete(self);
    }
    return self;
}

// Lote 220A (sin validar): instancias de plantilla del vector de MSVC 6 (`insert`, `_Insert_n` y `push_back`) sobre los mismos tres tipos de elemento que los ayudantes de 189A y 205A (4, 12 y 20 bytes). Como aquellos,
// no son metodos de CPedAttractorManager (el listado las atribuia a esa clase) sino plantillas de la STL que el EXE no expande en linea; todas son thiscall con `this` = el vector (SArray: +4 inicio, +8 fin, +0xC limite) y los
// argumentos de pila los limpia la propia funcion, asi que se declaran __fastcall con el vector como primer argumento (EDX sin usar). Los contadores usan la division con signo por el tamano del elemento (`sar` por 4 y
// multiplicacion magica por 12 o 20: truncan hacia cero) y las comparaciones de rango son SIN signo (`jae`), como el EXE.
struct SArrayHelpers4 {
    using T = uint32;
    static constexpr uint32 kMaxSize = 0x3FFFFFFF;
    static int32 Count(int32 bytes) { return bytes >> 2; }
    static void  Fill(T* first, T* last, const T* value) { SArrayFillStride4<0>(first, last, value); }
    static void  FillN(T* dest, uint32 count, const T* value) { SArrayFillNStride4<0>(dest, count, value); }
    static T*    UninitCopy(const T* first, const T* last, T* dest) { return SArrayUninitCopyStride4<0>(first, last, dest); }
    static void  CopyBackward(T* first, T* last, T* destEnd) { T* out; SArrayCopyBackwardStride4<0>(&out, first, last, destEnd); }
    static void  LengthError(SArray<T>* self) { plugin::CallMethod<0x5ECCB0, SArray<T>*>(self); } // _Xlen de esta instancia (lanza length_error)
};
struct SArrayHelpers12 {
    using T = SArrayElem12;
    static constexpr uint32 kMaxSize = 0x15555555;
    static int32 Count(int32 bytes) { return bytes / 12; }
    static void  Fill(T* first, T* last, const T* value) { SArrayFillStride12(first, last, value); }
    static void  FillN(T* dest, uint32 count, const T* value) { SArrayFillNStride12(dest, count, value); }
    static T*    UninitCopy(const T* first, const T* last, T* dest) { return SArrayUninitCopyStride12(first, last, dest); }
    static void  CopyBackward(T* first, T* last, T* destEnd) { T* out; SArrayCopyBackwardStride12(&out, first, last, destEnd); }
    static void  LengthError(SArray<T>* self) { plugin::CallMethod<0x5ECC30, SArray<T>*>(self); } // _Xlen de esta instancia (lanza length_error)
};

template<typename H>
int32 SArrayDiffCount(const typename H::T* from, const typename H::T* to) {
    return H::Count(static_cast<int32>(reinterpret_cast<intptr_t>(to) - reinterpret_cast<intptr_t>(from)));
}

// vector::_Insert_n(pos, n, &x): copia el valor a un local, con n == 0 sale; si tamano + n desborda max_size llama a _Xlen; si no cabe reserva (capacidad + capacidad / 2, o tamano + n si no basta; operator new del EXE, sin
// comprobar nulo), copia [inicio, pos) al bloque nuevo, rellena n copias, copia [pos, fin) detras, libera el bloque viejo (operator delete solo si hay inicio) y reescribe limite, fin e inicio en ese orden. Si cabe: con menos de n
// elementos tras pos copia [pos, fin) a pos + n, rellena la cola con n - (fin - pos) copias y rellena [pos, fin viejo); si no, copia los ultimos n elementos al final, copy_backward del resto y rellena [pos, pos + n).
template<typename H>
void SArrayInsertN(SArray<typename H::T>* self, typename H::T* pos, uint32 n, const typename H::T* x) {
    using T = typename H::T;
    const T tmp = *x;
    T* const first = self->_First;
    const uint32 cap = first ? static_cast<uint32>(SArrayDiffCount<H>(first, self->_End)) : 0;
    if (n == 0) {
        return;
    }
    const uint32 size = first ? static_cast<uint32>(SArrayDiffCount<H>(first, self->_Last)) : 0;
    if (H::kMaxSize - size < n) {
        H::LengthError(self);
        return;
    }
    if (cap < size + n) {
        uint32 newCap = (H::kMaxSize - (cap >> 1) >= cap) ? cap + (cap >> 1) : 0;
        if (newCap < size + n) {
            newCap = size + n;
        }
        T* const newBuf = static_cast<T*>(::operator new(newCap * sizeof(T)));
        T* const mid = H::UninitCopy(self->_First, pos, newBuf);
        H::FillN(mid, n, &tmp);
        H::UninitCopy(pos, self->_Last, mid + n);
        T* const oldFirst = self->_First;
        const uint32 oldSize = oldFirst ? static_cast<uint32>(SArrayDiffCount<H>(oldFirst, self->_Last)) : 0;
        if (oldFirst) {
            ::operator delete(oldFirst);
        }
        self->_End   = newBuf + newCap;
        self->_Last  = newBuf + (n + oldSize);
        self->_First = newBuf;
        return;
    }
    T* const last = self->_Last;
    if (static_cast<uint32>(SArrayDiffCount<H>(pos, last)) < n) {
        H::UninitCopy(pos, last, pos + n);
        const uint32 tail = static_cast<uint32>(SArrayDiffCount<H>(pos, self->_Last));
        H::FillN(self->_Last, n - tail, &tmp);
        T* const oldLast = self->_Last;
        self->_Last = oldLast + n;
        H::Fill(pos, oldLast, &tmp);
    } else {
        self->_Last = H::UninitCopy(last - n, last, last);
        H::CopyBackward(pos, last - n, last);
        H::Fill(pos, pos + n, &tmp);
    }
}

// vector::insert(pos, &x) de un elemento (`ret 0Ch`, argumentos (salida, pos, &x), devuelve la salida en EAX): indice = (pos - inicio) con signo si hay inicio y el vector no esta vacio (0 si no), _Insert_n(pos, 1, &x) y
// *salida = inicio (releido) + indice.
template<typename H>
typename H::T** SArrayInsertOne(SArray<typename H::T>* self, typename H::T** out, typename H::T* pos, const typename H::T* x) {
    using T = typename H::T;
    T* const first = self->_First;
    int32 index = 0;
    if (first && SArrayDiffCount<H>(first, self->_Last) != 0) {
        index = SArrayDiffCount<H>(first, pos);
    }
    SArrayInsertN<H>(self, pos, 1, x);
    *out = self->_First + index;
    return out;
}

// 0x5ED750 (739 bytes, `ret 0Ch`; el listado la atribuia a CPedAttractorManager): vector::_Insert_n de elementos de 12 bytes (CVector del vector de desplazamientos de CPedShelterAttractor), argumentos (pos, n, &x). Maximo 0x15555555.
// Llama a 0x5EAEA0 (copia sin inicializar), 0x5EB340 (relleno de n), 0x5EB1E0 (relleno), 0x5EB220 (copy_backward) y 0x5EB9D0 / 0x5EC140 (envolturas) con las funciones C++ de arriba, y a _Xlen (0x5ECC30) por direccion.
void __fastcall SArrayInsertNStride12(SArray<SArrayElem12>* self, void* /*edx*/, SArrayElem12* pos, uint32 n, const SArrayElem12* x) {
    SArrayInsertN<SArrayHelpers12>(self, pos, n, x);
}

// 0x5EDA50 (645 bytes, `ret 0Ch`; listado: CPedAttractorManager): vector::_Insert_n de punteros de 4 bytes, la tercera instancia (la que usan 0x5EDEC0 y 0x5EE400), argumentos (pos, n, &x). Maximo 0x3FFFFFFF; _Xlen es 0x5ECCB0. Las dos
// primeras instancias (0x5ECF10 y 0x5ED4B0) y la de 20 bytes (0x5ED1B0) tienen el mismo cuerpo y no son de este lote.
void __fastcall SArrayInsertNStride4C(SArray<uint32>* self, void* /*edx*/, uint32* pos, uint32 n, const uint32* x) {
    SArrayInsertN<SArrayHelpers4>(self, pos, n, x);
}

// 0x5EDE50 (109 bytes, `ret 0Ch`; listado: CPedAttractorManager): vector::insert de un elemento de 12 bytes; llama a 0x5ED750 (arriba).
SArrayElem12** __fastcall SArrayInsertStride12(SArray<SArrayElem12>* self, void* /*edx*/, SArrayElem12** out, SArrayElem12* pos, const SArrayElem12* x) {
    return SArrayInsertOne<SArrayHelpers12>(self, out, pos, x);
}

// 0x5EDD90 (111 bytes, `ret 0Ch`; listado: CPedAttractorManager): vector::insert de un elemento de 20 bytes (CPedTaskPair): como la anterior (el indice se calcula con la division por 20 de multiplicacion magica 0x66666667) pero la
// llamada es a _Insert_n de 20 bytes (0x5ED1B0, thiscall con (pos, 1, &x), sin reconstruir en este lote: se llama al EXE por direccion).
SArrayElem20** __fastcall SArrayInsertStride20(SArray<SArrayElem20>* self, void* /*edx*/, SArrayElem20** out, SArrayElem20* pos, const SArrayElem20* x) {
    SArrayElem20* const first = self->_First;
    int32 index = 0;
    if (first) {
        const int32 count = static_cast<int32>(reinterpret_cast<intptr_t>(self->_Last) - reinterpret_cast<intptr_t>(first)) / 20;
        if (count != 0) {
            index = static_cast<int32>(reinterpret_cast<intptr_t>(pos) - reinterpret_cast<intptr_t>(first)) / 20;
        }
    }
    plugin::CallMethod<0x5ED1B0, SArray<SArrayElem20>*, SArrayElem20*, uint32, const SArrayElem20*>(self, pos, 1u, x);
    *out = self->_First + index;
    return out;
}

// vector::push_back(&x) (`ret 4`): si hay inicio y tamano < capacidad (SIN signo) construye el elemento en el fin con el relleno de n = 1 (si el destino no es nulo) y avanza el fin; si no llama a vector::insert(fin, x).
template<typename H, typename InsertAtEnd>
void SArrayPushBack(SArray<typename H::T>* self, const typename H::T* x, InsertAtEnd&& insertAtEnd) {
    using T = typename H::T;
    T* const first = self->_First;
    const uint32 size = first ? static_cast<uint32>(SArrayDiffCount<H>(first, self->_Last)) : 0;
    if (first && size < static_cast<uint32>(SArrayDiffCount<H>(first, self->_End))) {
        T* const last = self->_Last;
        H::FillN(last, 1, x);
        self->_Last = last + 1;
        return;
    }
    insertAtEnd(self->_Last);
}

// 0x5EE200 (102 bytes, listado: CPedAttractorManager): push_back de punteros de 4 bytes, primera instancia; el camino lento es _Insert_n 0x5ECF10 (thiscall con (fin, 1, &x), sin reconstruir en este lote: se llama por direccion).
void __fastcall SArrayPushBackStride4A(SArray<uint32>* self, void* /*edx*/, const uint32* x) {
    SArrayPushBack<SArrayHelpers4>(self, x, [&](uint32* end) {
        plugin::CallMethod<0x5ECF10, SArray<uint32>*, uint32*, uint32, const uint32*>(self, end, 1u, x);
    });
}

// 0x5EE300 (102 bytes): push_back de 4 bytes, segunda instancia; camino lento 0x5ED4B0 (por direccion, sin reconstruir en este lote).
void __fastcall SArrayPushBackStride4B(SArray<uint32>* self, void* /*edx*/, const uint32* x) {
    SArrayPushBack<SArrayHelpers4>(self, x, [&](uint32* end) {
        plugin::CallMethod<0x5ED4B0, SArray<uint32>*, uint32*, uint32, const uint32*>(self, end, 1u, x);
    });
}

// 0x5EE400 (102 bytes): push_back de 4 bytes, tercera instancia; camino lento _Insert_n 0x5EDA50 (arriba).
void __fastcall SArrayPushBackStride4C(SArray<uint32>* self, void* /*edx*/, const uint32* x) {
    SArrayPushBack<SArrayHelpers4>(self, x, [&](uint32* end) {
        SArrayInsertN<SArrayHelpers4>(self, end, 1, x);
    });
}

// 0x5EE370 (136 bytes): push_back de 12 bytes; camino lento vector::insert de 12 bytes (0x5EDE50, arriba) con (salida, fin, &x): la salida es un hueco de pila que nadie lee.
void __fastcall SArrayPushBackStride12(SArray<SArrayElem12>* self, void* /*edx*/, const SArrayElem12* x) {
    SArrayPushBack<SArrayHelpers12>(self, x, [&](SArrayElem12* end) {
        SArrayElem12* result;
        SArrayInsertStride12(self, nullptr, &result, end, x);
    });
}

// 0x5EE270 (138 bytes): push_back de 20 bytes (el relleno de n = 1 es 0x5EB2E0, SArrayFillNStride20); camino lento vector::insert de 20 bytes (0x5EDD90, arriba).
void __fastcall SArrayPushBackStride20(SArray<SArrayElem20>* self, void* /*edx*/, const SArrayElem20* x) {
    SArrayElem20* const first = self->_First;
    const uint32 size = first ? static_cast<uint32>(static_cast<int32>(reinterpret_cast<intptr_t>(self->_Last) - reinterpret_cast<intptr_t>(first)) / 20) : 0;
    if (first && size < static_cast<uint32>(static_cast<int32>(reinterpret_cast<intptr_t>(self->_End) - reinterpret_cast<intptr_t>(first)) / 20)) {
        SArrayElem20* const last = self->_Last;
        SArrayFillNStride20(last, 1, x);
        self->_Last = last + 1;
        return;
    }
    SArrayElem20* result;
    SArrayInsertStride20(self, nullptr, &result, self->_Last, x);
}
} // namespace

void CPedAttractor::InjectHooks() {
    RH_ScopedVirtualClass(CPedAttractor, 0x86C538, 6);
    RH_ScopedCategory("Attractors");

    // Lote 189A (sin validar): ayudantes de plantilla de los vectores (ver arriba)
    RH_ScopedNamedGlobalInstall(SArrayFillStride4<0>, "SArrayFillStride4A", 0x5EAAA0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride20, "SArrayFillStride20", 0x5EAAC0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride4<1>, "SArrayFillStride4B", 0x5EAAF0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride4<2>, "SArrayFillStride4C", 0x5EAB10);
    RH_ScopedNamedGlobalInstall(SArrayCopyStride20, "SArrayCopyStride20", 0x5EAB80);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride20, "SArrayCopyBackwardStride20", 0x5EAC50);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride12, "SArrayCopyBackwardStride12", 0x5EACC0);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride4<0>, "SArrayUninitCopyStride4A", 0x5EAE10);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride20, "SArrayUninitCopyStride20", 0x5EAE40);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride4<1>, "SArrayUninitCopyStride4B", 0x5EAE70);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride12, "SArrayUninitCopyStride12", 0x5EAEA0);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyStride4<2>, "SArrayUninitCopyStride4C", 0x5EAEE0);
    // Lote 206A (sin validar): envolturas stdcall de las copias anteriores, vaciado de vector y destructor con borrado
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride4<0>, "SArrayUninitCopyWrapStride4A", 0x5EB930);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride20, "SArrayUninitCopyWrapStride20", 0x5EB960);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride4<1>, "SArrayUninitCopyWrapStride4B", 0x5EB990);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride12, "SArrayUninitCopyWrapStride12", 0x5EB9D0);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride4<2>, "SArrayUninitCopyWrapStride4C", 0x5EBA00);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4", 0x5EC820);
    RH_ScopedNamedGlobalInstall(PedAttractorScalarDeletingDestructor, "ScalarDeletingDestructor", 0x5EC4B0);

    // Lote 220A (sin validar): vector::_Insert_n, insert y push_back de las instancias de 4, 12 y 20 bytes (ver arriba)
    RH_ScopedNamedGlobalInstall(SArrayInsertNStride12, "SArrayInsertNStride12", 0x5ED750);
    RH_ScopedNamedGlobalInstall(SArrayInsertNStride4C, "SArrayInsertNStride4C", 0x5EDA50);
    RH_ScopedNamedGlobalInstall(SArrayInsertStride20, "SArrayInsertStride20", 0x5EDD90);
    RH_ScopedNamedGlobalInstall(SArrayInsertStride12, "SArrayInsertStride12", 0x5EDE50);
    RH_ScopedNamedGlobalInstall(SArrayPushBackStride4A, "SArrayPushBackStride4A", 0x5EE200);
    RH_ScopedNamedGlobalInstall(SArrayPushBackStride20, "SArrayPushBackStride20", 0x5EE270);
    RH_ScopedNamedGlobalInstall(SArrayPushBackStride4B, "SArrayPushBackStride4B", 0x5EE300);
    RH_ScopedNamedGlobalInstall(SArrayPushBackStride12, "SArrayPushBackStride12", 0x5EE370);
    RH_ScopedNamedGlobalInstall(SArrayPushBackStride4C, "SArrayPushBackStride4C", 0x5EE400);

    // Lote 205A (sin validar): mas ayudantes de plantilla de los vectores (ver arriba)
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride4<0>, "SArrayCopyBackwardStride4A", 0x5EB140);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride20Wrapper, "SArrayCopyBackwardStride20Wrapper", 0x5EB170);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride4<1>, "SArrayCopyBackwardStride4B", 0x5EB1B0);
    RH_ScopedNamedGlobalInstall(SArrayFillStride12, "SArrayFillStride12", 0x5EB1E0);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride12Wrapper, "SArrayCopyBackwardStride12Wrapper", 0x5EB220);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride4<2>, "SArrayCopyBackwardStride4C", 0x5EB260);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride4<0>, "SArrayFillNStride4A", 0x5EB2A0);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride20, "SArrayFillNStride20", 0x5EB2E0);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride4<1>, "SArrayFillNStride4B", 0x5EB310);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride12, "SArrayFillNStride12", 0x5EB340);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride4<2>, "SArrayFillNStride4C", 0x5EB390);

    // Lote 209A (sin validar): instancias de las mismas plantillas junto a los vectores de CAEUserRadioTrackManager (0x4F3AE0..0x4F4030; el listado las atribuia a esa clase). Mismos bytes que las gemelas de arriba (comprobado con
    // una busqueda de la secuencia de bytes en el EXE), asi que se enganchan con las mismas funciones C++.
    RH_ScopedNamedGlobalInstall(SArrayFillStride4<3>, "SArrayFillStride4D", 0x4F3AE0);
    RH_ScopedNamedGlobalInstall(SArrayCopyBackwardStride4<3>, "SArrayCopyBackwardStride4D", 0x4F3B00);
    RH_ScopedNamedGlobalInstall(SArrayFillNStride4<3>, "SArrayFillNStride4D", 0x4F3B30);
    RH_ScopedNamedGlobalInstall(SArrayUninitCopyWrapStride4<3>, "SArrayUninitCopyWrapStride4D", 0x4F3CE0);
    RH_ScopedNamedGlobalInstall(SArrayFillNReturnEndStride4, "SArrayFillNReturnEndStride4", 0x4F3D80);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4Inst<1>, "SArrayTidyStride4B", 0x4F3F30);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4Inst<2>, "SArrayTidyStride4C", 0x4F4030);

    // Lote 219A (sin validar): instancias de las mismas plantillas junto a los vectores de CPedAttractor y de CPedAttractorManager (0x5EC040..0x5ED4B0; el listado las atribuia a CPedAttractorManager). Vaciados de vector,
    // fill_n que devuelven el final, erase y los tres insert(pos, n, valor); mismas funciones C++ para los cuerpos identicos (ver arriba). 0x5ED0A3 y 0x5ED388 (9 bytes) son manejadores catch de 0x5ECF10 y 0x5ED1B0 (no enganchables).
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4D", 0x5EC040);
    RH_ScopedNamedGlobalInstall(SArrayFillNReturnEndStride4, "SArrayFillNReturnEndStride4B", 0x5EC070);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4E", 0x5EC0B0);
    RH_ScopedNamedGlobalInstall(SArrayFillNReturnEndStride20, "SArrayFillNReturnEndStride20", 0x5EC0E0);
    RH_ScopedNamedGlobalInstall(SArrayFillNReturnEndStride4, "SArrayFillNReturnEndStride4C", 0x5EC110);
    RH_ScopedNamedGlobalInstall(SArrayFillNReturnEndStride12, "SArrayFillNReturnEndStride12", 0x5EC140);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4F", 0x5EC180);
    RH_ScopedNamedGlobalInstall(SArrayFillNReturnEndStride4, "SArrayFillNReturnEndStride4D", 0x5EC1B0);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4G", 0x5EC1F0);
    RH_ScopedNamedGlobalInstall(SArrayEraseStride4, "SArrayEraseStride4", 0x5EC220);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4H", 0x5EC260);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4I", 0x5EC2D0);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4J", 0x5EC340);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4K", 0x5EC3B0);
    RH_ScopedNamedGlobalInstall(SArrayTidyStride4, "SArrayTidyStride4L", 0x5EC3E0);
    RH_ScopedNamedGlobalInstall(SArrayInsertNStride4<SArrayInsertInstanceA>, "SArrayInsertNStride4A", 0x5ECF10);
    RH_ScopedNamedGlobalInstall(SArrayInsertNStride20, "SArrayInsertNStride20", 0x5ED1B0);
    RH_ScopedNamedGlobalInstall(SArrayInsertNStride4<SArrayInsertInstanceB>, "SArrayInsertNStride4B", 0x5ED4B0);

    RH_ScopedInstall(operator delete, 0x5EAFE0); // lote 205A (sin validar)
    RH_ScopedInstall(Constructor, 0x5EDFB0);
    RH_ScopedInstall(Destructor, 0x5EC410);

    RH_ScopedInstall(SetTaskForPed, 0x5EECA0);
    RH_ScopedInstall(RegisterPed, 0x5EEE30);
    RH_ScopedInstall(DeRegisterPed, 0x5EC5B0);
    RH_ScopedInstall(IsRegisteredWithPed, 0x5EB4C0);
    // Como el original: las virtuales se enganchan por la vtable (huecos 2-5 de 0x86C538); con RH_ScopedInstall
    // el símbolo de la DLL es un thunk de llamada virtual que vuelve a la entrada enganchada -> bucle infinito
    RH_ScopedVMTInstall(IsAtHeadOfQueue, 0x5EB530);
    RH_ScopedInstall(GetTaskForPed, 0x5EC500);
    RH_ScopedInstall(GetQueueSlot, 0x5EB550);
    //RH_ScopedInstall(GetNoOfRegisteredPeds, 0xdeadbeef, { .reversed = false }); // Address incorrect
    RH_ScopedInstall(GetHeadOfQueue, 0x5EB590);
    RH_ScopedInstall(GetTailOfQueue, 0x5EB5B0);
    RH_ScopedInstall(HasEmptySlot, 0x5EAF10);
    //RH_ScopedInstall(ComputeFreeSlot, 0x0, { .reversed = false });
    RH_ScopedInstall(ComputeDeltaPos, 0x5E9600);
    RH_ScopedInstall(ComputeDeltaHeading, 0x5E9640);
    RH_ScopedInstall(ComputeAttractTime, 0x5E95E0);
    RH_ScopedVMTOverloadedInstall(ComputeAttractPos, "v", 0x5EA110, void(CPedAttractor::*)(int32, CVector&));
    RH_ScopedVMTOverloadedInstall(ComputeAttractHeading, "v", 0x5EA1C0, void(CPedAttractor::*)(int32, float&));
    RH_ScopedVMTInstall(BroadcastDeparture, 0x5EF160);
    RH_ScopedInstall(BroadcastArrival, 0x5EEF80);
    RH_ScopedInstall(AbortPedTasks, 0x5EAF60);
}

// 0x5EAFD0
void* CPedAttractor::operator new(unsigned size) {
    return GetPedAttractorPool()->New();
}

// 0x5EAFE0
// Lote 205A (sin validar; cdecl con un argumento de pila, `ret`, 49 bytes): libera la ranura de `object` en el pool de atractores (puntero global 0xB744C4, leido en cada llamada) EN LINEA, igual que el resto de operator delete
// del EXE: indice = (object - almacenamiento) / 0xC4 con signo (los slots miden 0xC4, no sizeof(CPedAttractor)), estados[indice] |= 0x80 y, si indice < cursor (con signo), cursor = indice. No comprueba nulo ni rellena el almacenamiento.
void CPedAttractor::operator delete(void* object) {
    PoolFreeSlotRaw(GetPedAttractorPool(), object, 0xC4);
}

// 0x5EDFB0
CPedAttractor::CPedAttractor(
    C2dEffectPedAttractor* fx,
    CEntity*               entity,
    eMoveState             moveState,
    size_t                 maxNoOfPeds,
    float                  spacing,
    float                  achieveQueueTime,
    float                  achieveQueueShuffleTime,
    float                  arriveRange,
    float                  headingRange,
    float                  deltaPos,
    float                  deltaHeading
) :
    m_Fx{fx},
    m_Entity{entity},
    m_MoveState{moveState},
    m_MaxNumPeds{maxNoOfPeds},
    m_Spacing{spacing},
    m_AchieveQueueTime{achieveQueueTime},
    m_AchieveQueueShuffleTime{achieveQueueShuffleTime},
    m_ArriveRange{arriveRange},
    m_HeadingRange{headingRange},
    m_DeltaPos{deltaPos},
    m_DeltaHeading{deltaHeading}
{
    const CMatrix mat = entity
        ? entity->GetMatrix()
        : CMatrix::Unity();
    m_Pos      = CPedAttractorManager::ComputeEffectPos(fx, mat);
    m_QueueDir = CPedAttractorManager::ComputeEffectQueueDir(fx, mat);
    m_UseDir   = CPedAttractorManager::ComputeEffectUseDir(fx, mat);

    strcpy(m_ScriptName, fx->m_szScriptName); // Como el original: strcpy (0x821BB5); strcpy_s rellenaba con 0xFE en Debug
}

void CPedAttractor::Shutdown() {
    ms_tasks.clear();
}

// 0x5EECA0
void CPedAttractor::SetTaskForPed(CPed* ped, CTask* task) {
    const auto numPairs = (int32)m_PedTaskPairs.size();
    for (int32 i = 0; i < numPairs; ++i) {
        auto& pair = m_PedTaskPairs.begin()[i];
        if (pair.Ped != ped) {
            continue;
        }
        if (!pair.UsedTask && pair.Task) { // 0x5EED79
            if (const auto erased = rng::find(ms_tasks, pair.Task); erased != ms_tasks.end()) { ms_tasks.erase(erased); } // Como el original: no borra si no está // 0x5EEDA0, 0x5EEDAD
            delete pair.Task; // 0x5EEDD1
        }
        pair.Task     = task;
        pair.UsedTask = false;
        ms_tasks.emplace_back(task); // Como el original: la tarea nueva se apunta en ms_tasks (0x5EEDF2)
        return;
    }
    // Como el original (0x5EED19): par nuevo {ped, tarea, -1, 0, false} y la tarea también va a ms_tasks
    m_PedTaskPairs.emplace_back(CPedTaskPair{ .Ped = ped, .Task = task, .Slot = TASK_SECONDARY_INVALID, .MatchID = 0, .UsedTask = false });
    ms_tasks.emplace_back(task);
}

// 0x5EEE30
bool CPedAttractor::RegisterPed(CPed* ped) {
    if (const auto it = rng::find(m_AttractPeds, ped); it != m_AttractPeds.end()) { // Como el original: si ya estaba, lo quita y devuelve false
        m_AttractPeds.erase(it);
        return false;
    }
    if ((int32)(m_ArrivedPeds.size() + m_AttractPeds.size()) >= (int32)m_MaxNumPeds) { // Como el original: con signo (jge)
        return false;
    }
    m_AttractPeds.emplace_back(ped); // Como el original: entra en m_AttractPeds (0x5EEED1), no en la cola
    const auto slot = (int32)m_ArrivedPeds.size();
    CVector pos;
    ComputeAttractPos(slot, pos);
    float heading;
    ComputeAttractHeading(slot, heading);
    const float time = m_AchieveQueueTime;
    SetTaskForPed(ped, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, m_MoveState });
    return true;
}

// 0x5EC5B0
bool CPedAttractor::DeRegisterPed(CPed* ped) {
    if (const auto erased = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); erased != m_PedTaskPairs.end()) { m_PedTaskPairs.erase(erased); } // Como el original: no borra si no está
    if (const auto it = rng::find(m_AttractPeds, ped); it != m_AttractPeds.end()) { // Como el original: busca y borra en m_AttractPeds (0x5EC600)
        m_AttractPeds.erase(it);
        return true;
    }
    return BroadcastDeparture(ped);
}

// 0x5EB4C0
bool CPedAttractor::IsRegisteredWithPed(const CPed* ped) const {
    return rng::contains(m_AttractPeds, ped)
        || rng::contains(m_ArrivedPeds, ped);
}

// 0x5EB530
bool CPedAttractor::IsAtHeadOfQueue(CPed* ped) {
    return *m_ArrivedPeds.begin() == ped; // Como el original: lee el primer hueco aunque la cola esté vacía (o sin reservar)
}

// 0x5EC500
CTask* CPedAttractor::GetTaskForPed(CPed* ped) {
    const auto it = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped);
    if (it == m_PedTaskPairs.end()) {
        return nullptr;
    }
    if (!it->UsedTask) {
        // Como el original (0x5EC55D): solo se quita si está en la lista; si no, no se borra nada
        if (const auto t = rng::find(ms_tasks, it->Task); t != ms_tasks.end()) {
            ms_tasks.erase(t);
        }
    }
    it->UsedTask = true;
    return it->Task;
}

// 0x5EB550
int32 CPedAttractor::GetQueueSlot(const CPed* ped) {
    const auto numArrived = (int32)m_ArrivedPeds.size();
    for (int32 i = 0; i < numArrived; ++i) {
        if (m_ArrivedPeds.begin()[i] == ped) {
            return i;
        }
    }
    return -1; // Como el original: -1 si no está en la cola
}

// 0x5EB590
CPed* CPedAttractor::GetHeadOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.front();
}

// 0x5EB5B0
CPed* CPedAttractor::GetTailOfQueue() const {
    return m_ArrivedPeds.empty()
        ? nullptr
        : m_ArrivedPeds.back();
}

int32 CPedAttractor::ComputeFreeSlot() {
    assert(0);
    return 0;
}

// 0x5E9600
float CPedAttractor::ComputeDeltaPos() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaPos, m_DeltaPos);
}

// 0x5E9640
float CPedAttractor::ComputeDeltaHeading() const {
    return CGeneral::GetRandomNumberInRange(-m_DeltaHeading, m_DeltaHeading);
}

// inlined
// 0x5E95E0
void CPedAttractor::ComputeAttractTime(int32 unused, bool time1_or_time2, float& outTime) const {
    if (time1_or_time2)
        outTime = m_AchieveQueueShuffleTime;
    else
        outTime = m_AchieveQueueTime;
}

// 0x5EA110
void CPedAttractor::ComputeAttractPos(int32 pedId, CVector& outPos) {
    if (m_Fx) {
        outPos = m_Pos - m_QueueDir * (float)pedId * m_Spacing; // Como el original: (dir · id) · separación

        if (pedId) {
            outPos.x += ComputeDeltaPos();
            outPos.y += ComputeDeltaPos();
        }
    }
}

// bQueue - bad name?
// 0x5EA1C0
void CPedAttractor::ComputeAttractHeading(int32 bQueue, float& heading) {
    if (m_Fx) {
        const CVector dir = bQueue ? m_QueueDir : m_UseDir; // Como el original: copia la dirección sin modificar m_UseDir
        heading = CGeneral::GetRadianAngleBetweenPoints(dir.x, dir.y, 0.0f, 0.0f);
        if (bQueue)
            heading += CPedAttractor::ComputeDeltaHeading();
    }
}

// 0x5EF160
bool CPedAttractor::BroadcastDeparture(CPed* ped) {
    // Como el original: el peatón que se va se busca en la cola (m_ArrivedPeds)
    const auto numArrived = (int32)m_ArrivedPeds.size();
    int32 idx = 0;
    for (; idx < numArrived; ++idx) {
        if (m_ArrivedPeds.begin()[idx] == ped) {
            break;
        }
    }
    if (idx >= numArrived) {
        return false;
    }
    // Los que van detrás avanzan un hueco: esperan 2000 · i ms y van al hueco i - 1 (0x5EF1C6)
    for (int32 i = idx + 1, slot = idx; i < numArrived; ++i, ++slot) {
        CPed* const queuedPed = m_ArrivedPeds.begin()[i];
        CVector pos;
        ComputeAttractPos(slot, pos);
        float heading;
        ComputeAttractHeading(slot, heading);
        const float time = m_AchieveQueueShuffleTime;
        auto* const seq = new CTaskComplexSequence;
        seq->AddTask(new CTaskSimpleStandStill{ 2000 * i, false, false, 8.0f });
        seq->AddTask(new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK });
        SetTaskForPed(queuedPed, seq);
    }
    if (const auto erased = rng::find(m_PedTaskPairs, ped, &CPedTaskPair::Ped); erased != m_PedTaskPairs.end()) { m_PedTaskPairs.erase(erased); } // Como el original: no borra si no está // 0x5EF2F6
    m_ArrivedPeds.erase(m_ArrivedPeds.begin() + idx); // 0x5EF334
    const auto numAttract = (int32)m_AttractPeds.size(); // 0x5EF371
    for (int32 i = 0; i < numAttract; ++i) {
        CPed* const attractedPed = m_AttractPeds.begin()[i];
        const auto slot = (int32)m_ArrivedPeds.size();
        CVector pos;
        ComputeAttractPos(slot, pos);
        float heading;
        ComputeAttractHeading(slot, heading);
        const float time = m_AchieveQueueTime;
        SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK });
    }
    return true;
}

// 0x5EEF80
bool CPedAttractor::BroadcastArrival(CPed* ped) {
    if (!rng::contains(m_ArrivedPeds, ped)) {
        m_ArrivedPeds.emplace_back(ped); // 0x5EEFCB
        if (const auto erased = rng::find(m_AttractPeds, ped); erased != m_AttractPeds.end()) { m_AttractPeds.erase(erased); } // Como el original: no borra si no está // 0x5EF002
        for (auto it = m_AttractPeds.begin(); it != m_AttractPeds.end();) { // 0x5EF013: relee el final en cada vuelta
            CPed* const attractedPed = *it++;
            const auto slot = (int32)m_ArrivedPeds.size();
            CVector pos;
            ComputeAttractPos(slot, pos);
            float heading;
            ComputeAttractHeading(slot, heading);
            const float time = m_AchieveQueueTime;
            SetTaskForPed(attractedPed, new CTaskComplexGoToAttractor{ this, pos, heading, time, slot, PEDMOVE_WALK }); // Como el original: PEDMOVE_WALK (push 4) // 0x5EF07B
        }
    }

    if (!IsAtHeadOfQueue(ped)) { // 0x5EF129
        SetTaskForPed(ped, new CTaskComplexWaitAtAttractor{ this, GetQueueSlot(ped) });
    } else if (GetType() == PED_ATTRACTOR_SCRIPTED) { // 0x5EF0D6 - inverted (!)
        SetTaskForPed(ped, new CTaskComplexUsePairedAttractor{ this });
    } else { // 0x5EF0FA
        SetTaskForPed(ped, new CTaskComplexUseAttractor{ this });
    }

    return true;
}

// 0x5EAF60
void CPedAttractor::AbortPedTasks() {
    const auto count = static_cast<int32>(m_PedTaskPairs.size());
    for (int32 i = 0; i < count; ++i) {
        auto& p = m_PedTaskPairs._First[i];
        p.Ped = nullptr;
        // Como el original: el destructor todavía ve el puntero Task.
        delete p.Task;
        p.Task = nullptr;
        p.Slot = TASK_SECONDARY_INVALID;
    }
    // TODO/NOTE: Why not clear the array?
}
