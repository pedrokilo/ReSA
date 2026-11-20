/* BEGIN GENERATED HOOK STATUS
 * Generado desde docs/hooks.csv; no editar este bloque manualmente.
 * Actualizar: python contrib/hooks2md.py --input docs/hooks.csv
 *   --output docs/ReversedClasses.md --update-source-status
 * Conteos por clase/grupo completo, no solo por este archivo.
 * Pendientes y bloqueadas pueden coincidir; no se suman.
 *
 * ESTADO: ExeCrt
 * Reconstrucción: COMPLETA — 9/9 funciones reconstruidas.
 * Pendientes de reconstruir: 0.
 * Funciones bloqueadas: 0.
 * Validación registrada: 0/9 entradas.
 * Validación: consultar vigencia en docs/ValidationInventory.md y
 *   reversing/ValidationRegistry/README.md. Sin evidencia vinculada
 *   no significa nunca probada ni fallida. Reconstruida no implica validada.
 * Alcance: solo entradas catalogadas; no certifica independencia del EXE.
END GENERATED HOOK STATUS */

#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include "CrtQsort.h"

// qsort del CRT del EXE (0x8247E0) y su ordenacion corta (0x824770): reconstruccion desde el desensamblado (qsort.c de MSVC 7.1).
// Las comparaciones y los intercambios se hacen en el mismo orden y con las mismas condiciones que el EXE (el intercambio se omite si
// los dos punteros son iguales; el intercambio es byte a byte). Sin dependencias del montón ni de x87.

namespace ExeCrt {
namespace {
constexpr unsigned int CUTOFF = 8;  // por debajo (<=) se usa la ordenacion corta
constexpr int          STKSIZ = 30; // pares (lo, hi) de la pila explicita

inline void SwapBytes(unsigned char* a, unsigned char* b, unsigned int width) {
    if (a == b) {
        return;
    }
    while (width--) {
        const unsigned char tmp = *a;
        *a++                    = *b;
        *b++                    = tmp;
    }
}

// 0x824770 (usercall: hi en EAX; pila: lo, width, comparador): coloca el maximo del rango al final, repetidamente
void ShortSort(unsigned char* lo, unsigned char* hi, unsigned int width, QsortCompare compare) {
    while (hi > lo) {
        unsigned char* max = lo;
        for (unsigned char* p = lo + width; p <= hi; p += width) {
            if (compare(p, max) > 0) {
                max = p;
            }
        }
        SwapBytes(max, hi, width);
        hi -= width;
    }
}
} // namespace

// 0x8247E0
void __cdecl Qsort(void* base, unsigned int num, unsigned int width, QsortCompare compare) {
    unsigned char* lostk[STKSIZ];
    unsigned char* histk[STKSIZ];

    if (num < 2 || width == 0) {
        return;
    }

    int            stkptr = 0;
    unsigned char* lo     = static_cast<unsigned char*>(base);
    unsigned char* hi     = lo + width * (num - 1);

recurse:
    const unsigned int size = static_cast<unsigned int>((hi - lo) / width) + 1;
    if (size <= CUTOFF) {
        ShortSort(lo, hi, width, compare);
    } else {
        unsigned char* mid = lo + (size / 2) * width;
        if (compare(lo, mid) > 0) {
            SwapBytes(lo, mid, width);
        }
        if (compare(lo, hi) > 0) {
            SwapBytes(lo, hi, width);
        }
        if (compare(mid, hi) > 0) {
            SwapBytes(mid, hi, width);
        }

        unsigned char* loguy = lo;
        unsigned char* higuy = hi;
        for (;;) {
            if (mid > loguy) {
                do {
                    loguy += width;
                } while (loguy < mid && compare(loguy, mid) <= 0);
            }
            if (mid <= loguy) {
                do {
                    loguy += width;
                } while (loguy <= hi && compare(loguy, mid) <= 0);
            }
            do {
                higuy -= width;
            } while (higuy > mid && compare(higuy, mid) > 0);
            if (higuy < loguy) {
                break;
            }
            SwapBytes(loguy, higuy, width);
            if (mid == higuy) {
                mid = loguy;
            }
        }

        higuy += width;
        if (mid < higuy) {
            do {
                higuy -= width;
            } while (higuy > mid && compare(higuy, mid) == 0);
        }
        if (mid >= higuy) {
            do {
                higuy -= width;
            } while (higuy > lo && compare(higuy, mid) == 0);
        }

        if (higuy - lo >= hi - loguy) {
            if (lo < higuy) {
                lostk[stkptr] = lo;
                histk[stkptr] = higuy;
                ++stkptr;
            }
            if (loguy < hi) {
                lo = loguy;
                goto recurse;
            }
        } else {
            if (loguy < hi) {
                lostk[stkptr] = loguy;
                histk[stkptr] = hi;
                ++stkptr;
            }
            if (lo < higuy) {
                hi = higuy;
                goto recurse;
            }
        }
    }

    --stkptr;
    if (stkptr >= 0) {
        lo = lostk[stkptr];
        hi = histk[stkptr];
        goto recurse;
    }
}
} // namespace ExeCrt

#ifndef CRT_BENCH
void CrtQsortInjectHooks() {
    RH_ScopedNamespaceName("ExeCrt");
    RH_ScopedCategory("Crt");

    RH_ScopedGlobalInstall(ExeCrt::Qsort, 0x8247E0);
}
#endif
