#ifndef CRT_BENCH
#include "StdInc.h"
#endif
#include <cstring>
#include "CrtStaticInit.h"

// Ver CrtStaticInit.h.

namespace ExeCrt {
namespace {
struct X87Init {
    uint8_t  shape;
    uint32_t dest, a, b, c;
    uint8_t  sizes; // bit0 A double, bit1 B double, bit2 C double, bit3 destino double
};

const X87Init kInits[] = {
#include "CrtStaticInitTable.inc"
};

double Load(uint32_t address, bool isDouble) {
    if (isDouble) {
        double v;
        std::memcpy(&v, reinterpret_cast<const void*>(address), sizeof(v));
        return v;
    }
    float v;
    std::memcpy(&v, reinterpret_cast<const void*>(address), sizeof(v));
    return static_cast<double>(v);
}

// Resultado del inicializador como double (x87 de 53 bits); `volatile` fuerza el redondeo a double tras cada operacion aunque el compilador use la x87
double Evaluate(const X87Init& init) {
    volatile double a = Load(init.a, init.sizes & 1);
    volatile double b = Load(init.b, init.sizes & 2);
    volatile double r = 0.0;
    switch (init.shape) {
    case 0: r = a / b; break;
    case 1: r = a * b; break;
    case 2: {
        volatile double product = a * b;
        volatile double c = Load(init.c, init.sizes & 4);
        r = c / product;
        break;
    }
    case 3: r = a - b; break;
    }
    return r;
}

void Store(const X87Init& init, double value) {
    if (init.sizes & 8) {
        std::memcpy(reinterpret_cast<void*>(init.dest), &value, sizeof(value));
    } else {
        const float f = static_cast<float>(value);
        std::memcpy(reinterpret_cast<void*>(init.dest), &f, sizeof(f));
    }
}
} // namespace

StaticInitReport __cdecl VerifyX87StaticInitializers() {
    StaticInitReport report;
    for (const X87Init& init : kInits) {
        const double value = Evaluate(init);
        uint8_t expected[8], actual[8];
        const size_t size = (init.sizes & 8) ? 8 : 4;
        if (init.sizes & 8) {
            std::memcpy(expected, &value, 8);
        } else {
            const float f = static_cast<float>(value);
            std::memcpy(expected, &f, 4);
        }
        std::memcpy(actual, reinterpret_cast<const void*>(init.dest), size);
        ++report.total;
        if (std::memcmp(expected, actual, size) == 0) {
            ++report.matched;
        } else {
            if (report.mismatched == 0) {
                report.firstMismatch = init.dest;
            }
            ++report.mismatched;
        }
    }
    return report;
}

void __cdecl RunX87StaticInitializers() {
    for (const X87Init& init : kInits) {
        Store(init, Evaluate(init));
    }
}
} // namespace ExeCrt
