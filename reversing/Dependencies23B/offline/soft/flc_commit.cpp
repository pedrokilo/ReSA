// g23b: cierre de la maquina x87 de software (hardware real) -- se compila con las opciones de X87Math.cpp: /O2 /Oi /arch:IA32 /fp:fast
#include "flc_soft.h"
#include <string.h>
#include <float.h>
#include <math.h>

// En X87Math.cpp estas constantes y los X87PlantStmt* ya existen; aqui se copian para compilar solo.
static const volatile float kPlantOne = 1.0f;
static const volatile float kPlantZeroA = 0.0f;
#define X87_PLANT_ZERO (kPlantZeroA * kPlantOne)
#pragma float_control(precise, on, push)
static __declspec(noinline) void X87PlantStmt5Cpp(double d) {
    volatile double sink;
    sink = (((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)) + ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)))
         + (((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)) + ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + (kPlantOne * d))));
}
static __declspec(noinline) void X87PlantStmt4Cpp(double d) {
    volatile double sink;
    sink = ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + X87_PLANT_ZERO)) + ((X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + (kPlantOne * d)));
}
static __declspec(noinline) void X87PlantStmt3Cpp(double d) {
    volatile double sink;
    sink = (X87_PLANT_ZERO + X87_PLANT_ZERO) + (X87_PLANT_ZERO + (kPlantOne * d));
}
static __declspec(noinline) void X87PlantStmt2Cpp(double d) {
    volatile double sink;
    sink = X87_PLANT_ZERO + (kPlantOne * d);
}
static __declspec(noinline) void X87PlantStmt1Cpp(double d) {
    volatile double sink;
    sink = kPlantOne * d;
}
#pragma float_control(pop)

// ---- Plantas (niveles 6, 7 y 8 nuevos): Stmt(N) = ZT(N-2) + Stmt(N-1), con ZT(k) = arbol equilibrado de 2^k ceros. Se ejecutan con precision de 64 bits (Commit): `1 * d` es exacto.
#define FZ X87_PLANT_ZERO
#define FZ2 (FZ + FZ)
#define FZ4 (FZ2 + FZ2)
#define FZ8 (FZ4 + FZ4)
#define FZ16 (FZ8 + FZ8)
#define FZ32 (FZ16 + FZ16)
#define FZ64 (FZ32 + FZ32)
#pragma float_control(precise, on, push)
static __declspec(noinline) void FlcPlantStmt6(double d) { volatile double sink; sink = FZ16 + (FZ8 + (FZ4 + (FZ2 + (FZ + (kPlantOne * d))))); }
static __declspec(noinline) void FlcPlantStmt7(double d) { volatile double sink; sink = FZ32 + (FZ16 + (FZ8 + (FZ4 + (FZ2 + (FZ + (kPlantOne * d)))))); }
static __declspec(noinline) void FlcPlantStmt8(double d) { volatile double sink; sink = FZ64 + (FZ32 + (FZ16 + (FZ8 + (FZ4 + (FZ2 + (FZ + (kPlantOne * d))))))); }
#pragma float_control(pop)
static void FlcPlant(int level, double d) {
    switch (level) {
    case 1: X87PlantStmt1Cpp(d); break; case 2: X87PlantStmt2Cpp(d); break; case 3: X87PlantStmt3Cpp(d); break; case 4: X87PlantStmt4Cpp(d); break;
    case 5: X87PlantStmt5Cpp(d); break; case 6: FlcPlantStmt6(d); break; case 7: FlcPlantStmt7(d); break; default: FlcPlantStmt8(d); break;
    }
}

// ---- fsin / fcos / fpatan reales (el intrinseco de /arch:IA32 emite la instruccion x87 y devuelve ST0)
extern "C" double __cdecl sin(double);
extern "C" double __cdecl cos(double);
extern "C" double __cdecl atan2(double, double);
#pragma intrinsic(sin, cos, atan2)
double __cdecl FlcHwSin(double x) { return sin(x); }
double __cdecl FlcHwCos(double x) { return cos(x); }
double __cdecl FlcHwAtan2(double y, double x) { return atan2(y, x); }

// ---- constantes volatile
static const volatile float kFlcZero = 0.0f;
static const volatile float kFlcOne = 1.0f;
static const volatile float kFlcThree = 3.0f;
static const volatile float kFlcInf = __builtin_huge_valf();
static const volatile float kFlcDenormal = 1.0e-45f;
static const volatile double kFlcBigDouble = 1.0e300;
static const volatile double kFlcTinyDouble = 1.0e-300;
static volatile float gFlcSinkF;
static volatile double gFlcSinkD;
static volatile int gFlcSinkI;

// ---- elevan UNA bandera acumulada con un unico registro x87 (T-1); el destino es una variable global volatile que nadie lee
#pragma float_control(precise, on, push)
static __declspec(noinline) void FlcRaiseIE() { gFlcSinkF = kFlcZero * kFlcInf; }
static __declspec(noinline) void FlcRaiseDE() { gFlcSinkF = kFlcDenormal * kFlcOne; }
static __declspec(noinline) void FlcRaiseZE() { gFlcSinkF = kFlcOne / kFlcZero; }
static __declspec(noinline) void FlcRaisePE() { gFlcSinkF = kFlcOne / kFlcThree; }
static __declspec(noinline) void FlcRaiseOE() { gFlcSinkF = float(kFlcBigDouble); }
static __declspec(noinline) void FlcRaiseUE() { gFlcSinkF = float(kFlcTinyDouble); }
#pragma float_control(pop)

// ---- fld dword [a]; fcomp dword [b]  (la forma sin `precise` da esta secuencia, igual que GangDriveByGtCpp)
static __declspec(noinline) void FlcCmpFF(const volatile float* a, const volatile float* b) { gFlcSinkI = (double(*a) > double(*b)) ? 1 : 0; }
// ---- fld qword [a]; fstp dword [sink]: deja a en T-1 y C1 = redondeo de la conversion
static __declspec(noinline) void FlcStoreD(const volatile double* a) { gFlcSinkF = float(*a); }

void FlcX87::Commit() {
    if (maxDepth == 0) return;                                         // el EXE no toco la x87
    // 1. banderas acumuladas
    if (state & FLC_IE) FlcRaiseIE();
    if (state & FLC_DE) FlcRaiseDE();
    if (state & FLC_ZE) FlcRaiseZE();
    if (state & FLC_PE) FlcRaisePE();
    if (state & FLC_OE) FlcRaiseOE();
    if (state & FLC_UE) FlcRaiseUE();
    // 2. registros muertos T-maxDepth .. T-2
    double vals[8];
    unsigned long long bits[8];
    for (int i = 0; i < maxDepth; ++i) { bits[i] = FlcToDouble(reg[i]); memcpy(&vals[i], &bits[i], 8); }
    unsigned int cwPrevious = 0, cwIgnored = 0;
    __control87_2(0, 0, &cwPrevious, nullptr);
    __control87_2(_PC_64, _MCW_PC, &cwIgnored, nullptr);                       // precision de 64 bits: `1 * d` es exacto con 53 bits
    for (int k = maxDepth - 1; k >= 1; --k) FlcPlant(k + 1, vals[k]);
    __control87_2(cwPrevious, _MCW_PC, &cwIgnored, nullptr);
    // 3. codigos de condicion, T-1 y C1. Los valores se escriben como enteros: ninguna carga/almacenamiento de coma flotante fuera de la secuencia buscada.
    static unsigned long long vBits;
    static unsigned int faBits, fbBits;
    vBits = bits[0];
    const unsigned int kQNaN = 0x7FC00000u, kOne = 0x3F800000u;
    if (ccValid) {                                  // fld [a]; fcomp [b] con 0 / 1 / NaN fija C0/C2/C3 (y C1 = 0)
        if (cc == (FLC_C0 | FLC_C2 | FLC_C3)) { faBits = kQNaN; fbBits = 0; }
        else if (cc == FLC_C3) { faBits = 0; fbBits = 0; }
        else if (cc == FLC_C0) { faBits = 0; fbBits = kOne; }
        else { faBits = kOne; fbBits = 0; }
        FlcCmpFF(reinterpret_cast<const volatile float*>(&faBits), reinterpret_cast<const volatile float*>(&fbBits));
    }
    if (lastClass == 2) FlcStoreD(reinterpret_cast<const volatile double*>(&vBits));   // fld qword [v]; fstp dword: T-1 = v y C1 = redondeo del almacenamiento
    else {
        __control87_2(_PC_64, _MCW_PC, &cwIgnored, nullptr);
        FlcPlant(1, vals[0]);                                                // T-1 = v, C1 = 0
        __control87_2(cwPrevious, _MCW_PC, &cwIgnored, nullptr);
    }
}

// Banderas acumuladas del x87 real (formato FLC_*), sin modificarlas.
unsigned int FlcHwSticky() {
    unsigned int x87 = 0, sse = 0;
    _statusfp2(&x87, &sse);
    unsigned int r = 0;
    if (x87 & _SW_INVALID) r |= FLC_IE;
    if (x87 & _SW_DENORMAL) r |= FLC_DE;
    if (x87 & _SW_ZERODIVIDE) r |= FLC_ZE;
    if (x87 & _SW_OVERFLOW) r |= FLC_OE;
    if (x87 & _SW_UNDERFLOW) r |= FLC_UE;
    if (x87 & _SW_INEXACT) r |= FLC_PE;
    return r;
}
