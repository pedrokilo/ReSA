// ============================================================================================================================================================
// CVehicle::FlyingControl (0x6D85F0, 10484 bytes). Reconstruccion en C++ con la maquina x87 de software FlcX87 (X87Math.h/.cpp): el EXE ejecuta el calculo en el x87 a 24 bits de
// precision y deja valores en los registros muertos, banderas acumuladas y codigos de condicion; FlcX87 reproduce cada instruccion con aritmetica entera y FlcX87::Commit() deja ese
// estado final en el hardware. Las secuencias que el EXE toma de funciones pequenas (operadores de CVector y CMatrix, _CIpow, _CIasin, _CIacos, __ftol2) estan en FlcVec* / FlcMat* /
// FlcCrt* (X87Math.cpp). Los colaboradores (CPhysical::ApplyTurnForce / ApplyMoveForce / GetSpeed, CPad, HeightAboveCeiling, GetColModel) son llamadas ordinarias.
//
// Rutas (flightModel, argumento 1; el EXE las elige con la tabla de bytes 0x6DAEF4 y la tabla de saltos 0x6DAEE4):
//   0           -> FlcRouteAirship   (0x6D8715..0x6D8B18)  [el juego no la usa]
//   1, 3, 4, 5  -> FlcRouteWing      (0x6D8B1D..0x6D986E)  1 = Baron/RC, 3 = avion y coche, 4 = sin uso, 5 = bote
//   2, 6, 8     -> FlcRouteRotor     (0x6D986E..0x6DA900)  6 = helicoptero e Hydra, 2 y 8 sin uso
//   7, > 8      -> solo prologo y cola
//   (todas)     -> cola comun FlcTail (0x6DA900..0x6DAED7)
// ============================================================================================================================================================

namespace {
#define M32(a) (*(uint32*)(uintptr_t)(uint32)(a))   // memoria de 32 bits por direccion (rutas traducidas del EXE)
// ---- Constantes del EXE (.rdata). Los operandos de memoria del x87 son referencias a estos objetos.
constexpr float  kFlcZero         = 0.0f;                 // 0x858B50
constexpr float  kFlcOne          = 1.0f;                 // 0x858624
constexpr float  kFlcMinusOne     = -1.0f;                // 0x858C1C
constexpr float  kFlcHalf         = 0.5f;                 // 0x858B8C
constexpr float  kFlcSix          = 6.0f;                 // 0x858B44
constexpr float  kFlcSpeedClamp   = 1.5f;                 // 0x858CE8: velocidad maxima por paso
constexpr float  kFlcClampSpeedSq = 2.25f;                // 0x86E104
constexpr float  kFlcTurnClamp    = 0.2f;                 // 0x858CC4: giro maximo por paso
constexpr float  kFlcClampTurnSq  = 0.04f;                // 0x858CEC

// ---- Campo del vehiculo por desplazamiento (los que no tienen miembro con nombre en CVehicle)
template<class T> inline T& FlcAt(CVehicle& v, size_t offset) { return *reinterpret_cast<T*>(reinterpret_cast<uint8*>(&v) + offset); }

// ---- Reproduce `test ah, mask` seguido de jp / jnp: el indicador de paridad es el de los 8 bits bajos del resultado.
inline bool FlcParityEven(unsigned int value) {
    value &= 0xFF;
    value ^= value >> 4;
    value ^= value >> 2;
    value ^= value >> 1;
    return (value & 1) == 0;
}

// ---- Variables locales del EXE que comparten el prologo, las rutas y la cola ([F+x] = desplazamiento desde la pila tras el prologo).
struct FlcFrame {
    CVehicle*            v;                // ESI
    FlcX87               fp;
    tFlyingHandlingData* fh;               // [ESI+0x388]
    eFlightModel         model;            // [F+0x264]
    float                skid;             // [F+0x268] leftRightSkid
    float                upDown;           // [F+0x26C] steeringUpDown
    float                sideways;         // [F+0x270] steeringLeftRight
    float                accel;            // [F+0x274] accelerationBreakStatus
    CPad*                pad;              // [F+0x14]: mando del conductor jugador
    float                vel[3];           // [F+0x40]: velocidad + deriva del viento
    float                com[3];           // [F+0x50]: centro de masas en el mundo (Multiply3x3 de la matriz por el centro de masas)
};

// =========================================================================================================================================
// Cola comun (0x6DA900..0x6DAED7): amortiguacion del giro y la velocidad, fuerzas de giro y recorte de velocidades.
// =========================================================================================================================================
// Una fuerza de giro por eje (0x6DAB58, 0x6DAC2B, 0x6DACFD): fuerza = (d * -eje) * masaDeGiro aplicada en el punto centroDeMasas + palanca.
void FlcApplyAxisTurn(FlcFrame& f, const float& d, const float* axis, const float* lever) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    float n2, n0d, force[3], point[3];
    fp.Fld(axis[0]); fp.Fmul(kFlcMinusOne);
    fp.Fld(axis[1]); fp.Fmul(kFlcMinusOne);
    fp.Fld(axis[2]); fp.Fmul(kFlcMinusOne);
    fp.Fstp(n2);
    fp.Fld(d); fp.Arith('*', 0, 0, 2, false); fp.Fstp(n0d);
    fp.Fld(d); fp.Arith('*', 0, 0, 1, false); fp.FstpSt(2); fp.FstpSt(0);
    fp.Fld(d); fp.Fmul(n2);
    fp.Fld(v.m_fTurnMass);
    fp.Fld(n0d); fp.Arith('*', 0, 0, 1, false); fp.Fstp(force[0]);
    fp.Fxch(2); fp.Arith('*', 0, 0, 2, false); fp.Fstp(force[1]);
    fp.Arith('*', 0, 0, 1, false); fp.Fstp(force[2]);
    fp.FstpSt(0);
    fp.Fld(f.com[0]); fp.Fadd(lever[0]); fp.Fstp(point[0]);
    fp.Fld(f.com[1]); fp.Fadd(lever[1]); fp.Fstp(point[1]);
    fp.Fld(f.com[2]); fp.Fadd(lever[2]); fp.Fstp(point[2]);
    v.ApplyTurnForce(CVector{ force[0], force[1], force[2] }, CVector{ point[0], point[1], point[2] });
}

// Recorta |vector| a `limit` cuando |vector|^2 > thresholdSq: vector = (vector * limit) * (1 / |vector|) (0x6DADBA..0x6DAE47 y 0x6DAE4A..0x6DAED4)
void FlcClampVector(FlcX87& fp, float* vec, const float& thresholdSq, const float& limit) {
    float magSq, cz, o0, o1, o2;
    fp.Fld(vec[2]); fp.Fld(vec[1]); fp.Fld(vec[0]);
    fp.FldSt(0); fp.Arith('*', 0, 0, 1, false);
    fp.FldSt(2); fp.Arith('*', 0, 0, 3, false);
    fp.Arith('+', 1, 1, 0, true);
    fp.FldSt(3); fp.Arith('*', 0, 0, 4, false);
    fp.Arith('+', 1, 1, 0, true);
    fp.Fst(magSq);
    fp.FstpSt(3); fp.FstpSt(0); fp.FstpSt(0);
    fp.FcomMem(thresholdSq, true);
    if (fp.Ah() & 0x41) return;                                    // jne: |v|^2 <= umbral o NaN
    fp.Fld(vec[0]); fp.Fmul(limit);
    fp.Fld(vec[1]); fp.Fmul(limit);
    fp.Fld(vec[2]); fp.Fmul(limit);
    fp.Fstp(cz);
    fp.Fld(magSq); fp.Fsqrt(); fp.Fdivr(kFlcOne); fp.Fst(magSq);
    fp.Arith('*', 0, 0, 2, false); fp.Fstp(o0);
    memcpy(&vec[0], &o0, 4);
    fp.Fmul(magSq); fp.Fstp(o1);
    fp.FstpSt(0);
    memcpy(&vec[1], &o1, 4);
    fp.Fld(cz); fp.Fmul(magSq); fp.Fstp(o2);
    memcpy(&vec[2], &o2, 4);
}

void FlcTail(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    const tFlyingHandlingData* fh = v.m_pFlyingHandlingData;
    const float* m = reinterpret_cast<const float*>(v.m_matrix);
    float t[3], l[3], w[3], d[3] = { 0.f, 0.f, 0.f }, scale;
    // amortiguacion de giro por eje: t = turnRes ^ timeStep
    fp.Fld(fh->m_vecTurnRes.x); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fstp(t[0]);
    fp.Fld(fh->m_vecTurnRes.y); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fstp(t[1]);
    fp.Fld(fh->m_vecTurnRes.z); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fstp(t[2]);
    FlcMatInvMul3x3(fp, l, m, &v.m_vecTurnSpeed.x);                // giro en ejes locales (0x59C810)
    scale = kFlcOne;                                               // [F+0x10]
    switch (f.model) {
    case FLIGHT_MODEL_PLANE: case FLIGHT_MODEL_UNK2: case FLIGHT_MODEL_BOAT:     // 3, 4, 5: 2 * |velocidad|
        FlcVecMagnitude(fp, &v.m_vecMoveSpeed.x); fp.Arith('+', 0, 0, 0, false); fp.Fstp(scale); break;
    case FLIGHT_MODEL_BARON:                                                     // 1: |velocidad| * 6
        FlcVecMagnitude(fp, &v.m_vecMoveSpeed.x); fp.Fmul(kFlcSix); fp.Fstp(scale); break;
    case FLIGHT_MODEL_HELI: case eFlightModel(8):                                // 6 y 8: |velocidad| + 1
        FlcVecMagnitude(fp, &v.m_vecMoveSpeed.x); fp.Fadd(kFlcOne); fp.Fstp(scale); break;
    default: break;
    }
    // un eje cada vez: con resistencia de velocidad > 0 el incremento de giro es d = (t / (1 + l*l*s)) ^ timeStep * l - l; si no, l = t ^ escala * l
    const float speedRes[3] = { fh->m_vecSpeedRes.x, fh->m_vecSpeedRes.y, fh->m_vecSpeedRes.z };
    for (int i = 0; i < 3; ++i) {
        fp.Fld(speedRes[i]); fp.FcomMem(kFlcZero, true);
        if (fp.Ah() & 0x41) {                                      // speedRes <= 0 o NaN
            fp.Fld(t[i]); fp.Fld(scale); FlcCrtPow(fp); fp.Fmul(l[i]); fp.Fstp(l[i]);
        } else {
            fp.Fld(l[i]); fp.Fmul(l[i]); fp.Fmul(speedRes[i]); fp.Fadd(kFlcOne); fp.Fdivr(t[i]);
            fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp); fp.Fmul(l[i]); fp.Fsub(l[i]); fp.Fstp(d[i]);
        }
    }
    FlcMatMul3x3(fp, w, m, l);                                     // giro de vuelta a ejes del mundo (0x59C790)
    memcpy(&v.m_vecTurnSpeed.x, &w[0], 4); memcpy(&v.m_vecTurnSpeed.y, &w[1], 4); memcpy(&v.m_vecTurnSpeed.z, &w[2], 4);
    fp.Fld(d[0]); fp.Fmul(scale); fp.Fstp(d[0]);
    fp.Fld(d[1]); fp.Fmul(scale); fp.Fstp(d[1]);
    fp.Fld(d[2]); fp.Fmul(scale); fp.Fstp(d[2]);
    // fuerzas de giro: eje X con -adelante y palanca arriba; eje Y con -arriba y palanca derecha; eje Z con -derecha y palanca adelante
    fp.Fld(d[0]); fp.FcomMem(kFlcZero, true);
    if (FlcParityEven(fp.Ah() & 0x44)) FlcApplyAxisTurn(f, d[0], &m[4], &m[8]);        // jnp: sin fuerza si d == 0
    fp.Fld(d[1]); fp.FcomMem(kFlcZero, true);
    if (FlcParityEven(fp.Ah() & 0x44)) FlcApplyAxisTurn(f, d[1], &m[8], &m[0]);
    fp.Fld(d[2]); fp.FcomMem(kFlcZero, true);
    if (FlcParityEven(fp.Ah() & 0x44)) FlcApplyAxisTurn(f, d[2], &m[0], &m[4]);
    FlcClampVector(fp, &v.m_vecMoveSpeed.x, kFlcClampSpeedSq, kFlcSpeedClamp);
    FlcClampVector(fp, &v.m_vecTurnSpeed.x, kFlcClampTurnSq, kFlcTurnClamp);
}

// =========================================================================================================================================
// Ruta 0 (flightModel == 0, 0x6D8715..0x6D8B18). El juego nunca la usa. Tres fuerzas de giro proporcionales a la deriva lateral y a la velocidad al cuadrado,
// dos de empuje (ApplyMoveForce) y el amortiguamiento del giro sobre Y.
// =========================================================================================================================================
constexpr double kFlcD001      = 0.001;       // 0x871FB0
constexpr double kFlcD003      = 0.003;       // 0x871FA8
constexpr double kFlcD05       = 0.5;         // 0x859060
constexpr double kFlcD005      = 0.05;        // 0x859068
constexpr double kFlcD35       = 3.5;         // 0x871F98
constexpr double kFlcD0072     = 0.0072000003419816494;   // 0x871F90 (0x3F7D7DBF60000000)
constexpr double kFlcD09f      = 0.89999997615814209;     // 0x86C308 (0.9f como double)
constexpr float  kFlcMinus4    = -4.0f;       // 0x859C3C
constexpr float  kFlcMinus128th = -0.0078125f; // 0x871FA0
constexpr float  kFlcF001      = 0.001f;      // 0x858CDC
constexpr float  kFlcF002      = 0.002f;      // 0x858F44
constexpr float  kFlcF008      = 0.008f;      // 0x863984
constexpr float  kFlcTwo       = 2.0f;        // (inmediato 0x40000000)

void FlcRouteAirship(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    const float* m = reinterpret_cast<const float*>(v.m_matrix);      // derecha m[0..2], adelante m[4..6], arriba m[8..10]
    float lat, c, scaleArg, right[3], fwd[3], t1[3], t2[3], t3[3];
    // deriva lateral normalizada: lat = -(vel . derecha) / |vel|
    fp.Fld(f.vel[2]); fp.Fmul(m[2]); fp.Fld(f.vel[1]); fp.Fmul(m[1]); fp.Arith('+', 1, 1, 0, true); fp.Fld(f.vel[0]); fp.Fmul(m[0]); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(kFlcMinusOne); fp.Fstp(lat);
    FlcVecMagnitude(fp, f.vel);
    fp.Fdivr(lat);
    memcpy(fwd, &m[4], 12);
    fp.Fstp(lat);
    // c = (|vel|^2 * (adelante . vel)) * (adelante . vel)
    fp.Fld(fwd[1]); fp.Fmul(f.vel[1]); fp.Fld(fwd[0]); fp.Fmul(f.vel[0]); fp.Arith('+', 1, 1, 0, true); fp.Fld(fwd[2]); fp.Fmul(f.vel[2]); fp.Arith('+', 1, 1, 0, true);
    fp.Fstp(c);
    FlcVecMagnitudeSquared(fp, f.vel);
    fp.Fmul(c);
    memcpy(right, &m[0], 12);
    fp.Fmul(c);
    fp.Fstp(c);
    // giro: fuerza = derecha * (((v494 * 0.001 + lat * 0.003) * dt * c) * turnMass) en el punto adelante * -4
    fp.Fld(FlcAt<float>(v, 0x494)); fp.FmulD(kFlcD001); fp.Fld(lat); fp.FmulD(kFlcD003); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.Fmul(v.m_fTurnMass); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);                                                 // 0x40FEF0
    fp.Fld(fwd[0]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[0]);
    fp.Fld(fwd[1]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[1]);
    fp.Fld(fwd[2]); fp.Fmul(kFlcMinus4); fp.Fstp(fwd[2]);
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ fwd[0], fwd[1], fwd[2] });   // 0x6D883E
    // empuje lateral: derecha * ((dt * masa * c * lat) * 0.2)
    fp.Fld(CTimer::ms_fTimeStep); fp.Fmul(v.m_fMass);
    memcpy(right, &m[0], 12);
    fp.Fmul(c); fp.Fmul(lat); fp.Fmul(kFlcTurnClamp); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);
    v.ApplyMoveForce(CVector{ right[0], right[1], right[2] });                        // 0x6D889C
    FlcVecMulLeft(fp, t1, kFlcTwo, &m[8]);                                            // 0x40FE90: 2 * arriba
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ t1[0], t1[1], t1[2] });       // 0x6D88DB
    // deriva vertical: lat = -(vel . arriba) / |vel|
    fp.Fld(f.vel[2]); fp.Fmul(m[10]); fp.Fld(f.vel[1]); fp.Fmul(m[9]); fp.Arith('+', 1, 1, 0, true); fp.Fld(f.vel[0]); fp.Fmul(m[8]); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(kFlcMinusOne); fp.Fstp(lat);
    FlcVecMagnitude(fp, f.vel);
    fp.Fdivr(lat);
    fp.Fstp(lat);
    fp.Fld(kFlcZero);
    if (f.pad) {
        fp.Pop();
        const int steer = f.pad->GetSteeringUpDown();                                 // 0x53FBD0 (int16 con signo)
        fp.FldI(steer); fp.Fmul(kFlcMinus128th);
    }
    fp.Fmul(kFlcF001);
    memcpy(right, &m[8], 12);
    fp.Fld(lat); fp.Fmul(kFlcF002); fp.Arith('+', 1, 1, 0, true);
    fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.Fmul(v.m_fTurnMass); fp.Fstp(scaleArg);
    FlcVecScale(fp, right, scaleArg);
    v.ApplyTurnForce(CVector{ right[0], right[1], right[2] }, CVector{ fwd[0], fwd[1], fwd[2] });   // 0x6D89BE
    // empuje vertical: c = ((lat * 3.5 + 0.5) * masa * dt * c) * 0.05, limitado a dt * masa * 0.0072 si el techo esta por debajo y la fuerza es pequena
    fp.Fld(lat); fp.FmulD(kFlcD35);
    const uint8 statusBits = uint8(v.m_nTypeStatus >> 3);
    fp.FaddD(kFlcD05); fp.Fmul(v.m_fMass); fp.Fmul(CTimer::ms_fTimeStep); fp.Fmul(c); fp.FmulD(kFlcD005); fp.Fstp(c);
    if (statusBits == 0 || statusBits == 8) {
        fp.Fld(CTimer::ms_fTimeStep); fp.Fmul(v.m_fMass); fp.Fmul(kFlcF008); fp.FcomMem(c, true);
        if (!FlcParityEven(fp.Ah() & 5)) {                                            // jp 0x6D8A57
            const float height = v.HeightAboveCeiling(v.GetPosition().z, eFlightModel(0));            // 0x6D8A2F
            fp.FldRet(height); fp.FcomMem(kFlcZero, true);
            if (!(fp.Ah() & 0x41)) { fp.Fld(v.m_fMass); fp.Fmul(CTimer::ms_fTimeStep); fp.FmulD(kFlcD0072); fp.Fstp(c); }
        }
    }
    FlcVecMulLeft(fp, t1, c, &m[8]);                                                  // 0x6D8A6B: c * arriba
    FlcVecMulLeft(fp, t2, kFlcTwo, &m[8]);                                            // 0x6D8A95: 2 * arriba
    FlcVecAdd(fp, t3, t2, f.com);                                                     // 0x6D8AA6: punto = 2 * arriba + centro de masas
    v.ApplyMoveForce(CVector{ t1[0], t1[1], t1[2] });                                 // 0x6D8ACF
    v.ApplyTurnForce(CVector{ t1[0], t1[1], t1[2] }, CVector{ t3[0], t3[1], t3[2] }); // 0x6D8AFC
    fp.FldD(kFlcD09f); fp.Fld(CTimer::ms_fTimeStep); FlcCrtPow(fp);                   // 0.9 ^ dt
    fp.Fmul(v.m_vecTurnSpeed.y); fp.Fstp(v.m_vecTurnSpeed.y);
}
void FlcRouteWing(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    alignas(16) uint8 stk[0xB00] = {};                      // pila x86 del EXE (el marco [F+x] empieza en stk + 0x800)
    uint32 eax = 0, ecx = 0, edx = 0, ebx = (uint32)(uintptr_t)f.pad, esi = (uint32)(uintptr_t)f.v, edi = 0, ebp = 0;
    uint32 esp = (uint32)(uintptr_t)stk + 0x800;            // F
    uint32 fA = 0, fB = 0, fR = 0;
    M32(esp + 0x264) = (uint32)f.model; memcpy(stk + 0x800 + 0x268, &f.skid, 4); memcpy(stk + 0x800 + 0x26C, &f.upDown, 4);
    memcpy(stk + 0x800 + 0x270, &f.sideways, 4); memcpy(stk + 0x800 + 0x274, &f.accel, 4);
    M32(esp + 0x14) = ebx;                                  // mando del conductor
    memcpy(stk + 0x800 + 0x40, f.vel, 12);                  // velocidad + deriva del viento
    memcpy(stk + 0x800 + 0x50, f.com, 12);                  // centro de masas en el mundo
    M32(esp + 0x4C) = (uint32)(uintptr_t)&v.m_vecMoveSpeed;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8b1d: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8b24: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8b2a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8b2c: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8b5f;   // 6d8b2f: jp 0x6d8b5f
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8b31: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6d8b33: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6d8b5f;   // 6d8b3e: je 0x6d8b5f
    ecx = ebx;   // 6d8b40: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6d8b42: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8b47: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d8b4a: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8b4e: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8b52: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8b58: fstp dword ptr [esp + 0x268]
L_6d8b5f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8b5f: fld dword ptr [esp + 0x26c]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8b66: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8b6c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8b6e: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8be3;   // 6d8b71: jp 0x6d8be3
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8b73: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6d8b75: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6d8be3;   // 6d8b80: je 0x6d8be3
    ecx = ebx;   // 6d8b82: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6d8b84: call 0x53fbd0
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8b89: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6d8b8c: mov dword ptr [esp + 0x14], eax
    ecx = ebx;   // 6d8b90: mov ecx, ebx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8b92: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8b96: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8b9c: fstp dword ptr [esp + 0x26c]
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d8ba3: call 0x53fc10
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8ba8: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6d8bab: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8baf: fild dword ptr [esp + 0x14]
    fp.Fabs();   // 6d8bb3: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d8bb5: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8bbb: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8bbd: test ah, 0x41
    if (fR != 0) goto L_6d8be3;   // 6d8bc0: jne 0x6d8be3
    ecx = ebx;   // 6d8bc2: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d8bc4: call 0x53fc10
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8bc9: movsx edx, ax
    fR = (uint32)(0 - edx); edx = fR;   // 6d8bcc: neg edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d8bce: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8bd2: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d8bd6: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8bdc: fstp dword ptr [esp + 0x26c]
L_6d8be3:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8be3: fld dword ptr [esp + 0x26c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8bea: fld dword ptr [esp + 0x268]
    fp.Fpatan();   // 6d8bf1: fpatan 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8bf3: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858624)));   // 6d8bf7: fld dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8bfd: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf8)), true);   // 6d8c01: fcomp dword ptr [0x86daf8]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c07: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c09: test ah, 0x41
    if (fR != 0) goto L_6d8c2a;   // 6d8c0c: jne 0x6d8c2a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c0e: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859ab0)), true);   // 6d8c12: fcomp dword ptr [0x859ab0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c18: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c1a: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c2a;   // 6d8c1d: jp 0x6d8c2a
    fp.FstpSt(0);   // 6d8c1f: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c21: fld dword ptr [esp + 0x10]
    goto L_6d8cb5;   // 6d8c25: jmp 0x6d8cb5
L_6d8c2a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c2a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859ab0)), true);   // 6d8c2e: fcomp dword ptr [0x859ab0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c34: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c36: test ah, 0x41
    if (fR != 0) goto L_6d8c5a;   // 6d8c39: jne 0x6d8c5a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c3b: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf4)), true);   // 6d8c3f: fcomp dword ptr [0x86daf4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c45: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c47: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c5a;   // 6d8c4a: jp 0x6d8c5a
    fp.FstpSt(0);   // 6d8c4c: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c4e: fld dword ptr [esp + 0x10]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 - m   // 6d8c52: fsub dword ptr [0x858fe4]
    goto L_6d8cb5;   // 6d8c58: jmp 0x6d8cb5
L_6d8c5a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c5a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf4)), true);   // 6d8c5e: fcomp dword ptr [0x86daf4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c64: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c66: test ah, 0x41
    if (fR != 0) goto L_6d8c79;   // 6d8c69: jne 0x6d8c79
    fp.FstpSt(0);   // 6d8c6b: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c6d: fld dword ptr [esp + 0x10]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(0x858cb8)));   // ST0 = ST0 - m   // 6d8c71: fsub dword ptr [0x858cb8]
    goto L_6d8cb5;   // 6d8c77: jmp 0x6d8cb5
L_6d8c79:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c79: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf0)), true);   // 6d8c7d: fcomp dword ptr [0x86daf0]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8c83: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8c85: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d8c98;   // 6d8c88: jp 0x6d8c98
    fp.FstpSt(0);   // 6d8c8a: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c8c: fld dword ptr [esp + 0x10]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858cb8)));   // ST0 = ST0 + m   // 6d8c90: fadd dword ptr [0x858cb8]
    goto L_6d8cb5;   // 6d8c96: jmp 0x6d8cb5
L_6d8c98:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8c98: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x86daf8)), true);   // 6d8c9c: fcomp dword ptr [0x86daf8]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8ca2: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8ca4: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8cbd;   // 6d8ca7: jp 0x6d8cbd
    fp.FstpSt(0);   // 6d8ca9: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8cab: fld dword ptr [esp + 0x10]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 + m   // 6d8caf: fadd dword ptr [0x858fe4]
L_6d8cb5:;
    fp.Fcos();   // 6d8cb5: fcos 
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m / ST0   // 6d8cb7: fdivr dword ptr [0x858624]
L_6d8cbd:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8cbd: fld dword ptr [esp + 0x268]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8cc4: mov eax, dword ptr [esi + 0x14]
    fp.Arith('*', 0, 0, 1, false);   // 6d8cc7: fmul st(1)
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d8cc9: add eax, 0x10
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d8ccc: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d8cce: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d8cd1: mov eax, dword ptr [eax + 8]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6d8cd4: fstp dword ptr [esp + 0x268]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d8cdb: mov dword ptr [esp + 0x28], ecx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d8cdf: fmul dword ptr [esp + 0x26c]
    ecx = esi;   // 6d8ce6: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d8ce8: mov dword ptr [esp + 0x2c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d8cec: mov dword ptr [esp + 0x30], eax
    fp.Fchs();   // 6d8cf0: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6d8cf2: fstp dword ptr [esp + 0x26c]
    eax = (uint32)(uintptr_t)v.GetColModel();   // 6d8cf9: call 0x535300
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d8cfe: fld dword ptr [eax + 4]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d8d01: mov edi, dword ptr [esp + 0x264]
    fA = edi; fB = 0x1u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8d08: cmp edi, 1
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d8d0b: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d0f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d8d11: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d8d15: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d19: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d8d1b: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d8d1f: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 1, false);   // 6d8d23: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d8d25: fstp dword ptr [esp + 0x24]
    fp.FstpSt(0);   // 6d8d29: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d8d2b: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // ST0 = ST0 * m   // 6d8d2f: fmul dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d8d33: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d8d37: fmul dword ptr [esp + 0x40]
    fp.Arith('+', 1, 1, 0, true);   // 6d8d3b: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d8d3d: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // ST0 = ST0 * m   // 6d8d41: fmul dword ptr [esp + 0x48]
    fp.Arith('+', 1, 1, 0, true);   // 6d8d45: faddp st(1)
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8d47: fst dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // ST0 = ST0 * m   // 6d8d4b: fmul dword ptr [esp + 0x10]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d8d4f: fstp dword ptr [esp + 0x18]
    if (fR != 0) goto L_6d8db7;   // 6d8d53: jne 0x6d8db7
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d8d55: mov ecx, dword ptr [0xb7cb5c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d8d5b: mov edx, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = ecx;   // 6d8d61: push ecx
    esp -= 4; M32(esp) = edx;   // 6d8d62: push edx
    eax = (uint32)(esp + 0x3c);   // 6d8d63: lea eax, [esp + 0x3c]
    esp -= 4; M32(esp) = eax;   // 6d8d67: push eax
    ecx = (uint32)(esp + 0x194);   // 6d8d68: lea ecx, [esp + 0x194]
    esp -= 4; M32(esp) = ecx;   // 6d8d6f: push ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x44) = (uint32)(0x0u);   // 6d8d70: mov dword ptr [esp + 0x44], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x48) = (uint32)(0x0u);   // 6d8d78: mov dword ptr [esp + 0x48], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x4c) = (uint32)(0x3B83126Fu);   // 6d8d80: mov dword ptr [esp + 0x4c], 0x3b83126f
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d8d88: call 0x40fec0
    esp += 0xCu;   // 6d8d8d: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d8d90: push eax
    edx = (uint32)(esp + 0xd0);   // 6d8d91: lea edx, [esp + 0xd0]
    esp -= 4; M32(esp) = edx;   // 6d8d98: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d8d99: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d8d9e: mov edx, dword ptr [eax]
    ecx = esp;   // 6d8da0: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d8da2: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d8da4: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d8da7: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d8daa: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d8dad: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d8db0: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d8db2: call 0x5429f0
L_6d8db7:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8db7: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d8dbe: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8dc4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d8dc6: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d8e05;   // 6d8dc9: jp 0x6d8e05
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8dcb: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d8dcd: mov dword ptr [esp + 0x274], 0
    if (fR == 0) goto L_6d8e05;   // 6d8dd8: je 0x6d8e05
    ecx = ebx;   // 6d8dda: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d8ddc: call 0x5403f0
    ecx = ebx;   // 6d8de1: mov ecx, ebx
    ebp = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8de3: movsx ebp, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d8de6: call 0x540080
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d8deb: movsx ecx, ax
    fA = ebp; fB = ecx; fR = (fA - fB) & 0xFFFFFFFFu; ebp = fR;   // 6d8dee: sub ebp, ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ebp);   // 6d8df0: mov dword ptr [esp + 0x14], ebp
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d8df4: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d8df8: fmul dword ptr [0x859a3c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8dfe: fstp dword ptr [esp + 0x274]
L_6d8e05:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x590));   // 6d8e05: mov ecx, dword ptr [esi + 0x590]
    fA = ecx; fB = ecx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d8e0b: test ecx, ecx
    eax = 0x3F800000u;   // 6d8e0d: mov eax, 0x3f800000
    if (fR != 0) goto L_6d8efe;   // 6d8e12: jne 0x6d8efe
    ebx = (ebx & 0xFFFF0000u) | (((uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22))) & 0xFFFFu);   // 6d8e18: mov bx, word ptr [esi + 0x22]
    fA = (ebx & 0xFFFFu); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d8e1c: cmp bx, 0x21b
    if (fR == 0) goto L_6d8e6f;   // 6d8e21: je 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7d4)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e23: cmp dword ptr [esi + 0x7d4], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e29: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7d8)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e2b: cmp dword ptr [esi + 0x7d8], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e31: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7dc)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e33: cmp dword ptr [esi + 0x7dc], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e39: jne 0x6d8e6f
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x7e0)); fB = eax; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8e3b: cmp dword ptr [esi + 0x7e0], eax
    if (fR != 0) goto L_6d8e6f;   // 6d8e41: jne 0x6d8e6f
    fA = (ebx & 0xFFFFu); fB = 0x1CCu; fR = (fA - fB) & 0xFFFFu;   // 6d8e43: cmp bx, 0x1cc
    if (fR != 0) goto L_6d8efe;   // 6d8e48: jne 0x6d8efe
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x40));   // 6d8e4e: mov eax, dword ptr [esi + 0x40]
    fA = ((eax >> 8) & 0xFFu); fB = 0x1u; fR = (fA & fB) & 0xFFu;   // 6d8e51: test ah, 1
    if (fR == 0) goto L_6d8efe;   // 6d8e54: je 0x6d8efe
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8e5a: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858cc4)), true);   // 6d8e5e: fcomp dword ptr [0x858cc4]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e64: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8e66: test ah, 0x41
    if (fR == 0) goto L_6d8efe;   // 6d8e69: je 0x6d8efe
L_6d8e6f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8e6f: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8e76: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e7c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8e7e: test ah, 0x41
    if (fR == 0) goto L_6d8efe;   // 6d8e81: je 0x6d8efe
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8e83: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8e8a: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8e90: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8e92: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8eee;   // 6d8e95: jp 0x6d8eee
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d8e97: mov edx, dword ptr [esp + 0x4c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8e9b: mov eax, dword ptr [esi + 0x14]
    esp -= 4; M32(esp) = edx;   // 6d8e9e: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d8e9f: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d8ea2: push eax
    FlcVecDot(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4));   // 6d8ea3: call 0x40fdb0
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b38)), true);   // 6d8ea8: fcomp dword ptr [0x858b38]
    esp += 0x8u;   // 6d8eae: add esp, 8
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8eb1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8eb3: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8eee;   // 6d8eb6: jp 0x6d8eee
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8eb8: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x871f88)));   // ST0 = ST0 * m   // 6d8ebc: fmul dword ptr [0x871f88]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = m - ST0   // 6d8ec2: fsubr dword ptr [esp + 0x274]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8ec9: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d8ecf: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8ed1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8ed3: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8ee0;   // 6d8ed6: jp 0x6d8ee0
    fp.FstpSt(0);   // 6d8ed8: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8eda: fld dword ptr [0x858b50]
L_6d8ee0:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d8ee0: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8ee6: fmul dword ptr [ecx + 4]
    goto L_6d8fa6;   // 6d8ee9: jmp 0x6d8fa6
L_6d8eee:;
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d8eee: mov dword ptr [esp + 0x274], 0
    goto L_6d8fad;   // 6d8ef9: jmp 0x6d8fad
L_6d8efe:;
    ebx = (ebx & 0xFFFF0000u) | (((uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22))) & 0xFFFFu);   // 6d8efe: mov bx, word ptr [esi + 0x22]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8f02: fld dword ptr [esp + 0x274]
    fA = (ebx & 0xFFFFu); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d8f09: cmp bx, 0x21b
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d8f0e: mov ecx, dword ptr [esi + 0x388]
    if (fR != 0) goto L_6d8f1b;   // 6d8f14: jne 0x6d8f1b
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8f16: fmul dword ptr [ecx + 4]
    goto L_6d8f2a;   // 6d8f19: jmp 0x6d8f2a
L_6d8f1b:;
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = ST0 + m   // 6d8f1b: fadd dword ptr [0x858624]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d8f21: fmul dword ptr [ecx + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // ST0 = ST0 * m   // 6d8f24: fmul dword ptr [0x858b8c]
L_6d8f2a:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8f2a: fstp dword ptr [esp + 0x274]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8f31: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8f35: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f3b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8f3d: test ah, 0x41
    if (fR != 0) goto L_6d8fad;   // 6d8f40: jne 0x6d8fad
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d8f42: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d8f45: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f4b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d8f4d: test ah, 5
    if (FlcParityEven(fR)) goto L_6d8fad;   // 6d8f50: jp 0x6d8fad
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d8f52: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d8f55: fcomp dword ptr [0x858b50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d8f5b: fld dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f5f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x1u; fR = (fA & fB) & 0xFFu;   // 6d8f61: test ah, 1
    if (fR != 0) goto L_6d8f75;   // 6d8f64: jne 0x6d8f75
    fp.Fsub((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 - m   // 6d8f66: fsub dword ptr [ecx + 8]
    fp.FldSt(0);   // 6d8f69: fld st(0)
    fp.Arith('*', 1, 1, 0, true);   // 6d8f6b: fmulp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858f50)));   // ST0 = ST0 * m   // 6d8f6d: fmul dword ptr [0x858f50]
    goto L_6d8f82;   // 6d8f73: jmp 0x6d8f82
L_6d8f75:;
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 + m   // 6d8f75: fadd dword ptr [ecx + 8]
    fp.FldSt(0);   // 6d8f78: fld st(0)
    fp.Arith('*', 1, 1, 0, true);   // 6d8f7a: fmulp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b3c)));   // ST0 = ST0 * m   // 6d8f7c: fmul dword ptr [0x858b3c]
L_6d8f82:;
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d8f82: fsubr dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8f88: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d8f8e: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d8f90: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d8f92: test ah, 0x41
    if (fR != 0) goto L_6d8f9f;   // 6d8f95: jne 0x6d8f9f
    fp.FstpSt(0);   // 6d8f97: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d8f99: fld dword ptr [0x858b50]
L_6d8f9f:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = ST0 * m   // 6d8f9f: fmul dword ptr [esp + 0x274]
L_6d8fa6:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fa6: fstp dword ptr [esp + 0x274]
L_6d8fad:;
    fA = edi; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8fad: cmp edi, 4
    if (fR != 0) goto L_6d8fc1;   // 6d8fb0: jne 0x6d8fc1
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fb2: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c24)));   // ST0 = ST0 * m   // 6d8fb9: fmul dword ptr [0x858c24]
    goto L_6d8fd3;   // 6d8fbf: jmp 0x6d8fd3
L_6d8fc1:;
    fA = edi; fB = 0x5u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d8fc1: cmp edi, 5
    if (fR != 0) goto L_6d8fda;   // 6d8fc4: jne 0x6d8fda
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fc6: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b1c)));   // ST0 = ST0 * m   // 6d8fcd: fmul dword ptr [0x858b1c]
L_6d8fd3:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d8fd3: fstp dword ptr [esp + 0x274]
L_6d8fda:;
    fA = (ebx & 0xFFFFu); fB = 0x208u; fR = (fA - fB) & 0xFFFFu;   // 6d8fda: cmp bx, 0x208
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d8fdf: mov edi, dword ptr [esi + 0x14]
    edx = (uint32)(edi + 0x10);   // 6d8fe2: lea edx, [edi + 0x10]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6d8fe5: mov eax, dword ptr [edx]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6d8fe7: mov ecx, dword ptr [edx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6d8fea: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(eax);   // 6d8fed: mov dword ptr [esp + 0x28], eax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6d8ff1: mov dword ptr [esp + 0x2c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(edx);   // 6d8ff5: mov dword ptr [esp + 0x30], edx
    if (fR != 0) goto L_6d9084;   // 6d8ff9: jne 0x6d9084
    eax = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x86c));   // 6d8fff: movzx eax, word ptr [esi + 0x86c]
    ecx = (uint32)(int32_t)(int16_t)((uint32)(*(uint16*)(uintptr_t)(uint32)(0x8d33c4)));   // 6d9006: movsx ecx, word ptr [0x8d33c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6d900d: mov dword ptr [esp + 0x14], eax
    edx = (uint32)(edi + 0x20);   // 6d9011: lea edx, [edi + 0x20]
    esp -= 4; M32(esp) = edx;   // 6d9014: push edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9015: fild dword ptr [esp + 0x18]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6d9019: mov dword ptr [esp + 0x18], ecx
    esp -= 4; M32(esp) = ecx;   // 6d901d: push ecx
    eax = (uint32)(esp + 0x250);   // 6d901e: lea eax, [esp + 0x250]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 * m   // 6d9025: fmul dword ptr [0x858fe4]
    fp.FidivI((*(int*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d902b: fidiv dword ptr [esp + 0x1c]
    fp.FldSt(0);   // 6d902f: fld st(0)
    fp.Fcos();   // 6d9031: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9033: fstp dword ptr [esp + 0x1c]
    fp.Fsin();   // 6d9037: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9039: fstp dword ptr [esp]
    esp -= 4; M32(esp) = eax;   // 6d903c: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d903d: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x20));   // 6d9042: mov ecx, dword ptr [esp + 0x20]
    esp += 0xCu;   // 6d9046: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9049: push eax
    fA = edi; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d904a: add edi, 0x10
    esp -= 4; M32(esp) = edi;   // 6d904d: push edi
    esp -= 4; M32(esp) = ecx;   // 6d904e: push ecx
    edx = (uint32)(esp + 0xec);   // 6d904f: lea edx, [esp + 0xec]
    esp -= 4; M32(esp) = edx;   // 6d9056: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9057: call 0x40fe90
    esp += 0xCu;   // 6d905c: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d905f: push eax
    eax = (uint32)(esp + 0x1a8);   // 6d9060: lea eax, [esp + 0x1a8]
    esp -= 4; M32(esp) = eax;   // 6d9067: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9068: call 0x40fe30
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d906d: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d906f: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9072: mov eax, dword ptr [eax + 8]
    esp += 0xCu;   // 6d9075: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d9078: mov dword ptr [esp + 0x28], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d907c: mov dword ptr [esp + 0x2c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d9080: mov dword ptr [esp + 0x30], eax
L_6d9084:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9084: fld dword ptr [esp + 0x28]
    esp -= 0xCu;   // 6d9088: sub esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d908b: fmul dword ptr [0x863984]
    ecx = esp;   // 6d9091: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9093: fld dword ptr [esp + 0x38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9097: fmul dword ptr [0x863984]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d909d: fld dword ptr [esp + 0x3c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d90a1: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d90a7: fstp dword ptr [esp + 0x48]
    fp.Fxch(1);   // 6d90ab: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90ad: fmul dword ptr [esp + 0x280]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90b4: fstp dword ptr [esp + 0x34]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90b8: fmul dword ptr [esp + 0x280]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d90bf: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x280)));   // ST0 = ST0 * m   // 6d90c3: fmul dword ptr [esp + 0x280]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // 6d90ca: fld dword ptr [esi + 0x8c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90d0: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6d90d4: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d90d6: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6d90da: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6d90dc: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d90de: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6d90e2: fmul st(1)
    fp.Fxch(1);   // 6d90e4: fxch st(1)
    fp.FstpSt(0);   // 6d90e6: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d90e8: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d90ec: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d90f2: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6d90f6: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d90fa: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d90fe: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9100: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9106: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6d910a: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d910e: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d9114: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d9117: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6d911b: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d911f: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6d9122: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9124: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9129: fld dword ptr [esp + 0x48]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d912d: mov eax, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6d9130: fmul dword ptr [eax + 8]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9133: mov ecx, dword ptr [eax]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9135: fld dword ptr [esp + 0x44]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9139: mov edx, dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d913c: fmul dword ptr [eax + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(ecx);   // 6d913f: mov dword ptr [esp + 0x28], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9143: mov ecx, dword ptr [esi + 0x388]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d9149: mov dword ptr [esp + 0x2c], edx
    fp.Arith('+', 1, 1, 0, true);   // 6d914d: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d914f: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d9153: fmul dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9155: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(eax);   // 6d9158: mov dword ptr [esp + 0x30], eax
    fp.Arith('+', 1, 1, 0, true);   // 6d915c: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d915e: fmul dword ptr [0x858c1c]
    fp.FldSt(0);   // 6d9164: fld st(0)
    fp.Fabs();   // 6d9166: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x14)));   // ST0 = ST0 * m   // 6d9168: fmul dword ptr [ecx + 0x14]
    fp.Arith('*', 1, 1, 0, true);   // 6d916b: fmulp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d916d: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6d9171: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9173: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 2, false);   // 6d9177: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9179: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d917d: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 2, false);   // 6d9181: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9183: fstp dword ptr [esp + 0x30]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9187: mov edx, dword ptr [esi + 0x8c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6d918d: mov dword ptr [esp + 0x14], edx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d9191: fmul dword ptr [esp + 0x14]
    esp -= 0xCu;   // 6d9195: sub esp, 0xc
    eax = esp;   // 6d9198: mov eax, esp
    fp.FstpSt(1);   // 6d919a: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d919c: fld dword ptr [esp + 0x38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d91a0: fmul dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d91a4: fld dword ptr [esp + 0x3c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d91a8: fmul dword ptr [esp + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d91ac: fstp dword ptr [esp + 0x70]
    fp.Fxch(1);   // 6d91b0: fxch st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91b2: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d91b8: fstp dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d91bc: mov ecx, dword ptr [esp + 0x40]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d91c0: mov dword ptr [eax], ecx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91c2: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d91c8: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d91cc: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d91d0: mov edx, dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d91d4: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d91da: mov dword ptr [eax + 4], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d91dd: fstp dword ptr [esp + 0x48]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d91e1: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d91e5: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6d91e8: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d91ea: call 0x5429f0
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d91ef: mov edi, dword ptr [esi + 0x14]
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x1c));   // 6d91f2: mov ebx, dword ptr [esp + 0x1c]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x20));   // 6d91f6: mov ebp, dword ptr [esp + 0x20]
    edx = edi;   // 6d91fa: mov edx, edi
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6d91fc: mov eax, dword ptr [edx]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6d91fe: mov ecx, dword ptr [edx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6d9201: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(eax);   // 6d9204: mov dword ptr [esp + 0x28], eax
    esp -= 0xCu;   // 6d9208: sub esp, 0xc
    eax = esp;   // 6d920b: mov eax, esp
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ebx);   // 6d920d: mov dword ptr [eax], ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x38) = (uint32)(ecx);   // 6d920f: mov dword ptr [esp + 0x38], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x30));   // 6d9213: mov ecx, dword ptr [esp + 0x30]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(edx);   // 6d9217: mov dword ptr [esp + 0x3c], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(ebp);   // 6d921b: mov dword ptr [eax + 4], ebp
    edx = (uint32)(esp + 0x104);   // 6d921e: lea edx, [esp + 0x104]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d9225: mov dword ptr [eax + 8], ecx
    esp -= 4; M32(esp) = edx;   // 6d9228: push edx
    ecx = esi;   // 6d9229: mov ecx, esi
    { CVector* out_ = (CVector*)(uintptr_t)M32(esp + 0); *out_ = v.GetSpeed(CVector{ (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)), (*(float*)(uintptr_t)(esp + 12)) }); eax = (uint32)(uintptr_t)out_; esp += 0x10; }   // 6d922b: call 0x542ce0
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 8)));   // 6d9230: fld dword ptr [eax + 8]
    fA = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22)); fB = 0x21Bu; fR = (fA - fB) & 0xFFFFu;   // 6d9233: cmp word ptr [esi + 0x22], 0x21b
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9239: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d923c: fld dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d923f: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9242: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax)));   // 6d9244: fld dword ptr [eax]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9246: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9248: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d924a: fmul dword ptr [0x858c1c]
    if (fR != 0) goto L_6d92b6;   // 6d9250: jne 0x6d92b6
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9252: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x274)), true);   // 6d9256: fcomp dword ptr [esp + 0x274]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d925d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d925f: test ah, 0x41
    if (fR != 0) goto L_6d926a;   // 6d9262: jne 0x6d926a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9264: fld dword ptr [esp + 0x10]
    goto L_6d9271;   // 6d9268: jmp 0x6d9271
L_6d926a:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d926a: fld dword ptr [esp + 0x274]
L_6d9271:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9271: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9275: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d927b: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d927d: test ah, 0x41
    if (FlcParityEven(fR)) goto L_6d92a3;   // 6d9280: jp 0x6d92a3
    fp.FstpSt(0);   // 6d9282: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9284: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x274)), true);   // 6d9288: fcomp dword ptr [esp + 0x274]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d928f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9291: test ah, 5
    if (FlcParityEven(fR)) goto L_6d929c;   // 6d9294: jp 0x6d929c
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9296: fld dword ptr [esp + 0x10]
    goto L_6d92a3;   // 6d929a: jmp 0x6d92a3
L_6d929c:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d929c: fld dword ptr [esp + 0x274]
L_6d92a3:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d92a3: mov eax, dword ptr [esi + 0x388]
    fp.FldSt(1);   // 6d92a9: fld st(1)
    fp.Fabs();   // 6d92ab: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6d92ad: fmul dword ptr [eax + 0x10]
    fp.Arith('*', 0, 0, 2, false);   // 6d92b0: fmul st(2)
    fp.Fxch(1);   // 6d92b2: fxch st(1)
    goto L_6d92c9;   // 6d92b4: jmp 0x6d92c9
L_6d92b6:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d92b6: mov eax, dword ptr [esi + 0x388]
    fp.FldSt(0);   // 6d92bc: fld st(0)
    fp.Fabs();   // 6d92be: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6d92c0: fmul dword ptr [eax + 0x10]
    fp.Arith('*', 0, 0, 1, false);   // 6d92c3: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d92c5: fld dword ptr [esp + 0x10]
L_6d92c9:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0xc)));   // ST0 = ST0 * m   // 6d92c9: fmul dword ptr [eax + 0xc]
    esp -= 4; M32(esp) = ecx;   // 6d92cc: push ecx
    ecx = (uint32)(esp + 0x2c);   // 6d92cd: lea ecx, [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d92d1: fmul dword ptr [esp + 0x26c]
    fp.Arith('+', 1, 1, 0, true);   // 6d92d8: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d92da: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d92e0: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d92e6: fstp dword ptr [esp]
    fp.FstpSt(0);   // 6d92e9: fstp st(0)
    FlcVecScale(fp, (float*)(uintptr_t)ecx, (*(float*)(uintptr_t)(esp + 0))); eax = ecx; esp += 4;   // 6d92eb: call 0x40fef0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d92f0: fld dword ptr [esp + 0x50]
    esp -= 0xCu;   // 6d92f4: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // ST0 = ST0 + m   // 6d92f7: fadd dword ptr [esp + 0x28]
    eax = esp;   // 6d92fb: mov eax, esp
    esp -= 0xCu;   // 6d92fd: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d9300: fstp dword ptr [esp + 0x4c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9304: mov ecx, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d9308: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d930c: mov dword ptr [eax], ecx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // ST0 = ST0 + m   // 6d930e: fadd dword ptr [esp + 0x38]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9312: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d9316: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d931a: mov edx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // ST0 = ST0 + m   // 6d931e: fadd dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d9322: mov dword ptr [eax + 4], edx
    edx = esp;   // 6d9325: mov edx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d9327: fstp dword ptr [esp + 0x54]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d932b: mov ecx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d932f: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9332: mov eax, dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9336: mov ecx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6d933a: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d933c: mov eax, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9340: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6d9343: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9345: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d9348: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6d934d: fld dword ptr [esp + 0x270]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9354: mov edi, dword ptr [esi + 0x14]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d9357: fcomp dword ptr [0x871f8c]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d935d: fld dword ptr [0xb7cb5c]
    ecx = edi;   // 6d9363: mov ecx, edi
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9365: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d9367: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d936a: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(eax);   // 6d936d: mov dword ptr [esp + 0x2c], eax
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9371: fnstsw ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(ecx);   // 6d9373: mov dword ptr [esp + 0x30], ecx
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d9377: test ah, 0x44
    esp -= 4; M32(esp) = ecx;   // 6d937a: push ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d937b: mov dword ptr [esp + 0x2c], edx
    ecx = (uint32)(esp + 0x2c);   // 6d937f: lea ecx, [esp + 0x2c]
    if (FlcParityEven(fR)) goto L_6d939b;   // 6d9383: jp 0x6d939b
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9385: mov edx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edx + 0x18)));   // ST0 = ST0 * m   // 6d938b: fmul dword ptr [edx + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d938e: fmul dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // ST0 = ST0 * m   // 6d9392: fmul dword ptr [esp + 0x26c]
    goto L_6d93af;   // 6d9399: jmp 0x6d93af
L_6d939b:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d939b: mov eax, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6d93a1: fmul dword ptr [eax + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6d93a4: fmul dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // ST0 = ST0 * m   // 6d93a8: fmul dword ptr [esp + 0x274]
L_6d93af:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d93af: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d93b5: fstp dword ptr [esp]
    FlcVecScale(fp, (float*)(uintptr_t)ecx, (*(float*)(uintptr_t)(esp + 0))); eax = ecx; esp += 4;   // 6d93b8: call 0x40fef0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d93bd: fld dword ptr [esp + 0x50]
    esp -= 0xCu;   // 6d93c1: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 + m   // 6d93c4: fadd dword ptr [edi + 0x20]
    ecx = esp;   // 6d93c7: mov ecx, esp
    esp -= 0xCu;   // 6d93c9: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d93cc: fstp dword ptr [esp + 0x4c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d93d0: mov edx, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d93d4: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d93d8: mov dword ptr [ecx], edx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 + m   // 6d93da: fadd dword ptr [edi + 0x24]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d93dd: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d93e1: fld dword ptr [esp + 0x70]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d93e5: mov eax, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 + m   // 6d93e9: fadd dword ptr [edi + 0x28]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d93ec: mov dword ptr [ecx + 4], eax
    eax = esp;   // 6d93ef: mov eax, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d93f1: fstp dword ptr [esp + 0x54]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d93f5: mov edx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d93f9: mov dword ptr [ecx + 8], edx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d93fc: mov ecx, dword ptr [esp + 0x40]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9400: mov edx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6d9404: mov dword ptr [eax], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d9406: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6d940a: mov dword ptr [eax + 4], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6d940d: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6d9410: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d9412: call 0x542a50
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9417: mov eax, dword ptr [esi + 0x14]
    edx = (uint32)(esp + 0x74);   // 6d941a: lea edx, [esp + 0x74]
    esp -= 4; M32(esp) = edx;   // 6d941e: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d941f: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d9422: push eax
    eax = (uint32)(esp + 0x208);   // 6d9423: lea eax, [esp + 0x208]
    esp -= 4; M32(esp) = eax;   // 6d942a: push eax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x80) = (uint32)(0x0u);   // 6d942b: mov dword ptr [esp + 0x80], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x84) = (uint32)(0x0u);   // 6d9436: mov dword ptr [esp + 0x84], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x88) = (uint32)(0x3F800000u);   // 6d9441: mov dword ptr [esp + 0x88], 0x3f800000
    FlcVecCross(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d944c: call 0x59c730
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858624)));   // 6d9451: fld dword ptr [0x858624]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9457: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9459: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d945c: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x34) = (uint32)(ecx);   // 6d945f: mov dword ptr [esp + 0x34], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9463: mov ecx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // 6d9466: fld dword ptr [ecx + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(eax);   // 6d9469: mov dword ptr [esp + 0x3c], eax
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d946d: fcomp dword ptr [0x858b50]
    esp += 0xCu;   // 6d9473: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(edx);   // 6d9476: mov dword ptr [esp + 0x2c], edx
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d947a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d947c: test ah, 0x41
    if (fR == 0) goto L_6d94ab;   // 6d947f: je 0x6d94ab
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9481: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d9485: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d948b: fstp dword ptr [esp + 0x28]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d948f: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d9493: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9499: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d949d: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d94a1: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d94a7: fstp dword ptr [esp + 0x30]
L_6d94ab:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // 6d94ab: fld dword ptr [ecx + 8]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d94ae: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d94b4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d94b6: test ah, 0x41
    if (fR != 0) goto L_6d94c3;   // 6d94b9: jne 0x6d94c3
    fp.FstpSt(0);   // 6d94bb: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // 6d94bd: fld dword ptr [0x858c1c]
L_6d94c3:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x18)));   // 6d94c3: fld dword ptr [ecx + 0x18]
    esp -= 4; M32(esp) = ecx;   // 6d94c6: push ecx
    fp.Fabs();   // 6d94c7: fabs 
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d94c9: fsubr dword ptr [0x858624]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d94cf: fld dword ptr [esp + 0x34]
    edx = (uint32)(esp + 0x114);   // 6d94d3: lea edx, [esp + 0x114]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d94da: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d94dd: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d94e1: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d94e4: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d94e6: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6d94ea: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6d94ec: faddp st(1)
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d94ee: fsubr dword ptr [0x858624]
    fp.Arith('*', 0, 0, 2, false);   // 6d94f4: fmul st(2)
    fp.Arith('*', 1, 1, 0, true);   // 6d94f6: fmulp st(1)
    fp.FstpSt(1);   // 6d94f8: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d94fa: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x20)));   // ST0 = ST0 + m   // 6d94fe: fadd dword ptr [ecx + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9501: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6d9505: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x24)));   // ST0 = ST0 + m   // 6d9509: fadd dword ptr [ecx + 0x24]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d950c: fstp dword ptr [esp + 0x3c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x5c)));   // 6d9510: fld dword ptr [esp + 0x5c]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // ST0 = ST0 + m   // 6d9514: fadd dword ptr [ecx + 0x28]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9517: mov ecx, dword ptr [esi + 0x388]
    esp -= 4; M32(esp) = ecx;   // 6d951d: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d951e: fstp dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x1c)));   // ST0 = ST0 * m   // 6d9522: fmul dword ptr [ecx + 0x1c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9525: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d952b: fmul dword ptr [esi + 0x90]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // ST0 = ST0 * m   // 6d9531: fmul dword ptr [0x858b8c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9537: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6d953a: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d953b: call 0x40fe90
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9540: mov edx, dword ptr [esp + 0x40]
    ecx = esp;   // 6d9544: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9546: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9548: mov edx, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d954c: mov dword ptr [ecx + 4], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d954f: mov edx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d9553: mov dword ptr [ecx + 8], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9556: mov edx, dword ptr [eax]
    esp -= 0xCu;   // 6d9558: sub esp, 0xc
    ecx = esp;   // 6d955b: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d955d: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d955f: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9562: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9565: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9568: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d956b: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d956d: call 0x542a50
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9572: mov edi, dword ptr [esi + 0x14]
    ecx = (uint32)(edi + 0x20);   // 6d9575: lea ecx, [edi + 0x20]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9578: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d957a: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d957d: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(edx);   // 6d9580: mov dword ptr [esp + 0x28], edx
    esp -= 0xCu;   // 6d9584: sub esp, 0xc
    edx = esp;   // 6d9587: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(esp + 0x3c) = (uint32)(ecx);   // 6d9589: mov dword ptr [esp + 0x3c], ecx
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ebx);   // 6d958d: mov dword ptr [edx], ebx
    ecx = (uint32)(esp + 0x1c4);   // 6d958f: lea ecx, [esp + 0x1c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x38) = (uint32)(eax);   // 6d9596: mov dword ptr [esp + 0x38], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x30));   // 6d959a: mov eax, dword ptr [esp + 0x30]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ebp);   // 6d959e: mov dword ptr [edx + 4], ebp
    esp -= 4; M32(esp) = ecx;   // 6d95a1: push ecx
    ecx = esi;   // 6d95a2: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d95a4: mov dword ptr [edx + 8], eax
    { CVector* out_ = (CVector*)(uintptr_t)M32(esp + 0); *out_ = v.GetSpeed(CVector{ (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)), (*(float*)(uintptr_t)(esp + 12)) }); eax = (uint32)(uintptr_t)out_; esp += 0x10; }   // 6d95a7: call 0x542ce0
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 8)));   // 6d95ac: fld dword ptr [eax + 8]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 * m   // 6d95af: fmul dword ptr [edi + 0x28]
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 4)));   // 6d95b2: fld dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 * m   // 6d95b5: fmul dword ptr [edi + 0x24]
    fp.Arith('+', 1, 1, 0, true);   // 6d95b8: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax)));   // 6d95ba: fld dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d95bc: mov eax, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 * m   // 6d95c2: fmul dword ptr [edi + 0x20]
    esp -= 0xCu;   // 6d95c5: sub esp, 0xc
    edx = esp;   // 6d95c8: mov edx, esp
    esp -= 0xCu;   // 6d95ca: sub esp, 0xc
    fp.Arith('+', 1, 1, 0, true);   // 6d95cd: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d95cf: fmul dword ptr [0x858c1c]
    fp.FldSt(0);   // 6d95d5: fld st(0)
    fp.Fabs();   // 6d95d7: fabs 
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6d95d9: fmul dword ptr [eax + 0x24]
    fp.Arith('*', 1, 1, 0, true);   // 6d95dc: fmulp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d95de: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6d95e2: fmul dword ptr [eax + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // ST0 = ST0 * m   // 6d95e5: fmul dword ptr [esp + 0x284]
    fp.Arith('+', 1, 1, 0, true);   // 6d95ec: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d95ee: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6d95f4: fmul dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d95fa: fld dword ptr [esp + 0x40]
    fp.Arith('*', 0, 0, 1, false);   // 6d95fe: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d9600: fstp dword ptr [esp + 0x40]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9604: fld dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6d9608: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d960a: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d960e: fld dword ptr [esp + 0x48]
    fp.Arith('*', 0, 0, 1, false);   // 6d9612: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9614: fstp dword ptr [esp + 0x48]
    fp.FstpSt(0);   // 6d9618: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x68)));   // 6d961a: fld dword ptr [esp + 0x68]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // ST0 = ST0 + m   // 6d961e: fadd dword ptr [esp + 0x34]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6d9622: fstp dword ptr [esp + 0x4c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9626: mov eax, dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6d962a: fld dword ptr [esp + 0x6c]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6d962e: mov dword ptr [edx], eax
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // ST0 = ST0 + m   // 6d9630: fadd dword ptr [esp + 0x38]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9634: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6d9638: fld dword ptr [esp + 0x70]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6d963c: mov ecx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // ST0 = ST0 + m   // 6d9640: fadd dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9644: mov dword ptr [edx + 4], ecx
    ecx = esp;   // 6d9647: mov ecx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6d9649: fstp dword ptr [esp + 0x54]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6d964d: mov eax, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9651: mov dword ptr [edx + 8], eax
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6d9654: mov edx, dword ptr [esp + 0x40]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6d9658: mov eax, dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d965c: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6d965e: mov edx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6d9662: mov dword ptr [ecx + 4], eax
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6d9665: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6d9668: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6d966a: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d966f: fld dword ptr [esp + 0x48]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9673: mov edi, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x28)));   // ST0 = ST0 * m   // 6d9676: fmul dword ptr [edi + 0x28]
    ecx = (uint32)(esp + 0x40);   // 6d9679: lea ecx, [esp + 0x40]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d967d: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x24)));   // ST0 = ST0 * m   // 6d9681: fmul dword ptr [edi + 0x24]
    fp.Arith('+', 1, 1, 0, true);   // 6d9684: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d9686: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 * m   // 6d968a: fmul dword ptr [edi + 0x20]
    fp.Arith('+', 1, 1, 0, true);   // 6d968d: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d968f: fstp dword ptr [esp + 0x14]
    FlcVecMagnitude(fp, (float*)(uintptr_t)ecx);   // 6d9693: call 0x4082c0
    esp -= 4; M32(esp) = ecx;   // 6d9698: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9699: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0x3C23D70Au;   // 6d969c: push 0x3c23d70a
    FlcFloatMax(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96a1: call 0x420800
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // ST0 = m / ST0   // 6d96a6: fdivr dword ptr [esp + 0x1c]
    esp += 0x4u;   // 6d96aa: add esp, 4
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d96ad: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0x3F800000u;   // 6d96b0: push 0x3f800000
    FlcFloatMin(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96b5: call 0x404330
    esp += 0x4u;   // 6d96ba: add esp, 4
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d96bd: fstp dword ptr [esp]
    esp -= 4; M32(esp) = 0xBF800000u;   // 6d96c0: push 0xbf800000
    FlcFloatMax(fp, (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)));   // 6d96c5: call 0x420800
    esp += 0x8u;   // 6d96ca: add esp, 8
    FlcCrtAsin(fp);   // 6d96cd: call 0x821e70
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x594));   // 6d96d2: mov ebx, dword ptr [esi + 0x594]
    fA = ebx; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d96d8: cmp ebx, 4
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6d96db: fmul dword ptr [0x858c1c]
    if (fR != 0) goto L_6d970d;   // 6d96e1: jne 0x6d970d
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871d98)), false);   // 6d96e3: fcom dword ptr [0x871d98]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d96e9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d96eb: test ah, 0x41
    if (fR != 0) goto L_6d970d;   // 6d96ee: jne 0x6d970d
    fp.Fld(CTimer::ms_fTimeStep);   // 6d96f0: fld dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d96f6: fmul dword ptr [0x858b38]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c4c)));   // ST0 = ST0 * m   // 6d96fc: fmul dword ptr [0x858c4c]
    eax = (uint32)FlcCrtFtol2(fp);   // 6d9702: call 0x821b40
    fA = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x9a0)); fB = eax; fR = (fA + fB) & 0xFFFFFFFFu; *(uint32*)(uintptr_t)(uint32)(esi + 0x9a0) = (uint32)(fR);   // 6d9707: add dword ptr [esi + 0x9a0], eax
L_6d970d:;
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d970d: mov edx, dword ptr [esp + 0x264]
    fA = edx; fB = 0x1u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d9714: cmp edx, 1
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9717: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x28)));   // 6d971d: fld dword ptr [ecx + 0x28]
    if (fR != 0) goto L_6d974f;   // 6d9720: jne 0x6d974f
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d3670)));   // 6d9722: fld dword ptr [0x8d3670]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9728: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d972e: fstp dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9732: fld dword ptr [esp + 0x18]
    fp.Arith('*', 0, 0, 1, false);   // 6d9736: fmul st(1)
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x14)), true);   // 6d9738: fcomp dword ptr [esp + 0x14]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d973c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d973e: test ah, 0x41
    if (fR != 0) goto L_6d976a;   // 6d9741: jne 0x6d976a
    fp.FstpSt(0);   // 6d9743: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9745: fld dword ptr [esp + 0x14]
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 / m   // 6d9749: fdiv dword ptr [esp + 0x18]
    goto L_6d976a;   // 6d974d: jmp 0x6d976a
L_6d974f:;
    fA = ebx; fB = 0x4u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d974f: cmp ebx, 4
    if (fR != 0) goto L_6d976a;   // 6d9752: jne 0x6d976a
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x9cc)));   // 6d9754: fld dword ptr [esi + 0x9cc]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d975a: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9760: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9762: test ah, 5
    if (FlcParityEven(fR)) goto L_6d976a;   // 6d9765: jp 0x6d976a
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x34)));   // ST0 = ST0 * m   // 6d9767: fmul dword ptr [ecx + 0x34]
L_6d976a:;
    fp.Fxch(1);   // 6d976a: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x2c)));   // ST0 = ST0 * m   // 6d976c: fmul dword ptr [ecx + 0x2c]
    fp.Arith('+', 0, 0, 1, false);   // 6d976f: fadd st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d9771: fmul dword ptr [esi + 0x8c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9777: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6d977d: fmul dword ptr [esp + 0x18]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9781: fst dword ptr [esp + 0x14]
    fp.FstpSt(1);   // 6d9785: fstp st(1)
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9787: fld dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d978d: fmul dword ptr [esi + 0x8c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9793: fmul dword ptr [0x863984]
    fp.Fxch(1);   // 6d9799: fxch st(1)
    fp.Fcompp();   // 6d979b: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d979d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d979f: test ah, 0x41
    if (fR != 0) goto L_6d983a;   // 6d97a2: jne 0x6d983a
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d97a8: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d97aa: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d97b2;   // 6d97ad: jne 0x6d97b2
    eax = (uint32)(esi + 4);   // 6d97af: lea eax, [esi + 4]
L_6d97b2:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d97b2: mov eax, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d97b5: push edx
    esp -= 4; M32(esp) = eax;   // 6d97b6: push eax
    ecx = esi;   // 6d97b7: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d97b9: call 0x6d2600
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d97be: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d97c4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d97c6: test ah, 0x41
    if (fR != 0) goto L_6d983a;   // 6d97c9: jne 0x6d983a
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d97cb: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d97cd: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d97d5;   // 6d97d0: jne 0x6d97d5
    eax = (uint32)(esi + 4);   // 6d97d2: lea eax, [esi + 4]
L_6d97d5:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d97d5: mov ecx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d97d8: push edx
    esp -= 4; M32(esp) = ecx;   // 6d97d9: push ecx
    ecx = esi;   // 6d97da: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d97dc: call 0x6d2600
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d97e1: fmul dword ptr [0x858b38]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d97e7: fsubr dword ptr [0x858624]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d97ed: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d97f3: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d97f5: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9802;   // 6d97f8: jp 0x6d9802
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d97fa: fld dword ptr [0x858b50]
    goto L_6d9824;   // 6d9800: jmp 0x6d9824
L_6d9802:;
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9802: test edi, edi
    eax = (uint32)(edi + 0x30);   // 6d9804: lea eax, [edi + 0x30]
    if (fR != 0) goto L_6d980c;   // 6d9807: jne 0x6d980c
    eax = (uint32)(esi + 4);   // 6d9809: lea eax, [esi + 4]
L_6d980c:;
    esp -= 4; M32(esp) = edx;   // 6d980c: push edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d980d: mov edx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = edx;   // 6d9810: push edx
    ecx = esi;   // 6d9811: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9813: call 0x6d2600
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b38)));   // ST0 = ST0 * m   // 6d9818: fmul dword ptr [0x858b38]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(0x858624)));   // ST0 = m - ST0   // 6d981e: fsubr dword ptr [0x858624]
L_6d9824:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x8c)));   // ST0 = ST0 * m   // 6d9824: fmul dword ptr [esi + 0x8c]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d982a: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9830: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // 6d9836: fstp dword ptr [esp + 0x14]
L_6d983a:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x14));   // 6d983a: mov eax, dword ptr [esp + 0x14]
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d983e: add edi, 0x20
    esp -= 4; M32(esp) = edi;   // 6d9841: push edi
    esp -= 4; M32(esp) = eax;   // 6d9842: push eax
    ecx = (uint32)(esp + 0x94);   // 6d9843: lea ecx, [esp + 0x94]
    esp -= 4; M32(esp) = ecx;   // 6d984a: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d984b: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9850: mov ecx, dword ptr [eax]
    edx = esp;   // 6d9852: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6d9854: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9856: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9859: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d985c: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6d985f: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9861: mov dword ptr [edx + 8], eax
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9864: call 0x5429f0
    return;   // 6d9869: jmp 0x6da900
}
void FlcRouteRotor(FlcFrame& f) {
    CVehicle& v = *f.v;
    FlcX87& fp = f.fp;
    alignas(16) uint8 stk[0xB00] = {};                      // pila x86 del EXE (el marco [F+x] empieza en stk + 0x800)
    uint32 eax = 0, ecx = 0, edx = 0, ebx = (uint32)(uintptr_t)f.pad, esi = (uint32)(uintptr_t)f.v, edi = 0, ebp = 0;
    uint32 esp = (uint32)(uintptr_t)stk + 0x800;            // F
    uint32 fA = 0, fB = 0, fR = 0;
    M32(esp + 0x264) = (uint32)f.model; memcpy(stk + 0x800 + 0x268, &f.skid, 4); memcpy(stk + 0x800 + 0x26C, &f.upDown, 4);
    memcpy(stk + 0x800 + 0x270, &f.sideways, 4); memcpy(stk + 0x800 + 0x274, &f.accel, 4);
    M32(esp + 0x14) = ebx;                                  // mando del conductor
    memcpy(stk + 0x800 + 0x40, f.vel, 12);                  // velocidad + deriva del viento
    memcpy(stk + 0x800 + 0x50, f.com, 12);                  // centro de masas en el mundo
    M32(esp + 0x4C) = (uint32)(uintptr_t)&v.m_vecMoveSpeed;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d986e: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ecx + 0x3c)));   // 6d9874: fld dword ptr [ecx + 0x3c]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9877: fld dword ptr [0xb7cb5c]
    FlcCrtPow(fp);   // 6d987d: call 0x822130
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6d9882: mov eax, dword ptr [esp + 0x4c]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x264));   // 6d9886: mov ebp, dword ptr [esp + 0x264]
    fA = ebp; fB = 0x8u; fR = (fA - fB) & 0xFFFFFFFFu;   // 6d988d: cmp ebp, 8
    fp.FldSt(0);   // 6d9890: fld st(0)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d9892: fmul dword ptr [eax]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(eax)));   // 6d9894: fstp dword ptr [eax]
    fp.FldSt(0);   // 6d9896: fld st(0)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x48)));   // ST0 = ST0 * m   // 6d9898: fmul dword ptr [esi + 0x48]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x48)));   // 6d989b: fstp dword ptr [esi + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // ST0 = ST0 * m   // 6d989e: fmul dword ptr [esi + 0x4c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // 6d98a1: fstp dword ptr [esi + 0x4c]
    if (fR != 0) goto L_6d9b9d;   // 6d98a4: jne 0x6d9b9d
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d98aa: mov eax, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d98ad: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6d98b1: fmul dword ptr [eax + 0x18]
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d98b4: add eax, 0x10
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d98b7: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d98bb: fmul dword ptr [eax + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d98be: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6d98c0: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6d98c4: fmul dword ptr [eax]
    fp.Arith('+', 1, 1, 0, true);   // 6d98c6: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d98c8: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d98cc: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), false);   // 6d98d3: fcom dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d98d9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d98db: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d9912;   // 6d98de: jp 0x6d9912
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d98e0: test ebx, ebx
    fp.FstpSt(0);   // 6d98e2: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d98e4: fld dword ptr [0x858b50]
    if (fR == 0) goto L_6d9912;   // 6d98ea: je 0x6d9912
    ecx = ebx;   // 6d98ec: mov ecx, ebx
    fp.FstpSt(0);   // 6d98ee: fstp st(0)
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d98f0: call 0x5403f0
    ecx = ebx;   // 6d98f5: mov ecx, ebx
    edi = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d98f7: movsx edi, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d98fa: call 0x540080
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d98ff: movsx edx, ax
    fA = edi; fB = edx; fR = (fA - fB) & 0xFFFFFFFFu; edi = fR;   // 6d9902: sub edi, edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edi);   // 6d9904: mov dword ptr [esp + 0x18], edi
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9908: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d990c: fmul dword ptr [0x859a3c]
L_6d9912:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9912: fld dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9916: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d991c: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d991e: test ah, 0x41
    if (fR == 0) goto L_6d9965;   // 6d9921: je 0x6d9965
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), false);   // 6d9923: fcom dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9929: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d992b: test ah, 0x41
    if (fR == 0) goto L_6d9965;   // 6d992e: je 0x6d9965
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9930: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9936: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d993a: fmul dword ptr [ecx + 8]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859000)));   // ST0 = ST0 * m   // 6d993d: fmul dword ptr [0x859000]
    fp.Arith('-', 1, 1, 0, true);   // 6d9943: fsubp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9945: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d994b: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d994d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d994f: test ah, 5
    if (FlcParityEven(fR)) goto L_6d995c;   // 6d9952: jp 0x6d995c
    fp.FstpSt(0);   // 6d9954: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9956: fld dword ptr [0x858b50]
L_6d995c:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d995c: fmul dword ptr [ecx + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d995f: fstp dword ptr [esp + 0x18]
    goto L_6d997d;   // 6d9963: jmp 0x6d997d
L_6d9965:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9965: mov eax, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d996b: fld dword ptr [esp + 0x10]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6d996f: fmul dword ptr [eax + 8]
    fp.Arith('-', 0, 1, 0, false);   // 6d9972: fsubr st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6d9974: fmul dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9977: fstp dword ptr [esp + 0x18]
    fp.FstpSt(0);   // 6d997b: fstp st(0)
L_6d997d:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d997d: mov eax, dword ptr [0xb7cb5c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9982: mov ecx, dword ptr [esi + 0x8c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x18));   // 6d9988: mov edx, dword ptr [esp + 0x18]
    esp -= 4; M32(esp) = eax;   // 6d998c: push eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d998d: mov eax, dword ptr [esi + 0x14]
    esp -= 4; M32(esp) = ecx;   // 6d9990: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9991: push edx
    fA = eax; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; eax = fR;   // 6d9992: add eax, 0x10
    esp -= 4; M32(esp) = eax;   // 6d9995: push eax
    ecx = (uint32)(esp + 0x240);   // 6d9996: lea ecx, [esp + 0x240]
    esp -= 4; M32(esp) = ecx;   // 6d999d: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d999e: call 0x40fec0
    esp += 0xCu;   // 6d99a3: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d99a6: push eax
    edx = (uint32)(esp + 0x14c);   // 6d99a7: lea edx, [esp + 0x14c]
    esp -= 4; M32(esp) = edx;   // 6d99ae: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d99af: call 0x40fec0
    esp += 0xCu;   // 6d99b4: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d99b7: push eax
    eax = (uint32)(esp + 0x1d8);   // 6d99b8: lea eax, [esp + 0x1d8]
    esp -= 4; M32(esp) = eax;   // 6d99bf: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d99c0: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d99c5: mov edx, dword ptr [eax]
    ecx = esp;   // 6d99c7: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d99c9: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d99cb: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d99ce: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d99d1: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d99d4: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d99d7: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d99d9: call 0x5429f0
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d99de: mov edi, dword ptr [esi + 0x14]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0x8d34e8));   // 6d99e1: mov edx, dword ptr [0x8d34e8]
    ecx = (uint32)(edi + 0x10);   // 6d99e7: lea ecx, [edi + 0x10]
    esp -= 4; M32(esp) = ecx;   // 6d99ea: push ecx
    esp -= 4; M32(esp) = edx;   // 6d99eb: push edx
    eax = (uint32)(esp + 0x160);   // 6d99ec: lea eax, [esp + 0x160]
    esp -= 4; M32(esp) = eax;   // 6d99f3: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d99f4: call 0x40fe90
    esp -= 4; M32(esp) = eax;   // 6d99f9: push eax
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d99fa: add edi, 0x20
    ecx = (uint32)(esp + 0x2c);   // 6d99fd: lea ecx, [esp + 0x2c]
    esp -= 4; M32(esp) = edi;   // 6d9a01: push edi
    esp -= 4; M32(esp) = ecx;   // 6d9a02: push ecx
    FlcVecSub(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9a03: call 0x40fe60
    esp += 0x18u;   // 6d9a08: add esp, 0x18
    ecx = (uint32)(esp + 0x1c);   // 6d9a0b: lea ecx, [esp + 0x1c]
    FlcVecNormalise(fp, (float*)(uintptr_t)ecx);   // 6d9a0f: call 0x59c910
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9a14: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d9a18: fmul dword ptr [esp + 0x40]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x590));   // 6d9a1c: mov eax, dword ptr [esi + 0x590]
    fA = eax; fB = eax; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9a22: test eax, eax
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9a24: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // ST0 = ST0 * m   // 6d9a28: fmul dword ptr [esp + 0x48]
    fp.Arith('+', 1, 1, 0, true);   // 6d9a2c: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9a2e: fld dword ptr [esp + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // ST0 = ST0 * m   // 6d9a32: fmul dword ptr [esp + 0x44]
    fp.Arith('+', 1, 1, 0, true);   // 6d9a36: faddp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a38: fstp dword ptr [esp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8717ac)));   // 6d9a3c: fld dword ptr [0x8717ac]
    if (fR != 0) goto L_6d9b04;   // 6d9a42: jne 0x6d9b04
    fp.FstpSt(0);   // 6d9a48: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d34d8)));   // 6d9a4a: fld dword ptr [0x8d34d8]
    fp.Fchs();   // 6d9a50: fchs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x10)), false);   // 6d9a52: fcom dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9a56: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9a58: test ah, 0x41
    if (fR != 0) goto L_6d9a61;   // 6d9a5b: jne 0x6d9a61
    fp.FldSt(0);   // 6d9a5d: fld st(0)
    goto L_6d9a65;   // 6d9a5f: jmp 0x6d9a65
L_6d9a61:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a61: fld dword ptr [esp + 0x10]
L_6d9a65:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b50)));   // 6d9a65: fld dword ptr [0x858b50]
    fp.FcomSt(1, true);   // 6d9a6b: fcomp st(1)
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9a6d: fnstsw ax
    fp.FstpSt(0);   // 6d9a6f: fstp st(0)
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9a71: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9ad6;   // 6d9a74: jp 0x6d9ad6
    *(uint32*)(uintptr_t)(uint32)(esp + 0x10) = (uint32)(0x0u);   // 6d9a76: mov dword ptr [esp + 0x10], 0
L_6d9a7e:;
    fp.FstpSt(0);   // 6d9a7e: fstp st(0)
L_6d9a80:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9a80: fld dword ptr [esp + 0x10]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6d9a84: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34d4)));   // ST0 = ST0 * m   // 6d9a8a: fmul dword ptr [0x8d34d4]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // ST0 = m - ST0   // 6d9a90: fsubr dword ptr [esi + 0x84c]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9a96: fst dword ptr [esp + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9a9a: fstp dword ptr [esi + 0x84c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x8d34dc)));   // 6d9aa0: fld dword ptr [0x8d34dc]
    fp.Fld(CTimer::ms_fTimeStep);   // 6d9aa6: fld dword ptr [0xb7cb5c]
    FlcCrtPow(fp);   // 6d9aac: call 0x822130
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6d9ab1: fmul dword ptr [esp + 0x18]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858ee8)), false);   // 6d9ab5: fcom dword ptr [0x858ee8]
    fp.Fst((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9abb: fst dword ptr [esi + 0x84c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9ac1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9ac3: test ah, 0x41
    if (fR != 0) goto L_6d9ae7;   // 6d9ac6: jne 0x6d9ae7
    fp.FstpSt(0);   // 6d9ac8: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esi + 0x84c) = (uint32)(0x3ECCCCCDu);   // 6d9aca: mov dword ptr [esi + 0x84c], 0x3ecccccd
    goto L_6d9afe;   // 6d9ad4: jmp 0x6d9afe
L_6d9ad6:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(esp + 0x10)), false);   // 6d9ad6: fcom dword ptr [esp + 0x10]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9ada: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9adc: test ah, 0x41
    if (fR != 0) goto L_6d9a7e;   // 6d9adf: jne 0x6d9a7e
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9ae1: fstp dword ptr [esp + 0x10]
    goto L_6d9a80;   // 6d9ae5: jmp 0x6d9a80
L_6d9ae7:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x859018)), true);   // 6d9ae7: fcomp dword ptr [0x859018]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9aed: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6d9aef: test ah, 5
    if (FlcParityEven(fR)) goto L_6d9afe;   // 6d9af2: jp 0x6d9afe
    *(uint32*)(uintptr_t)(uint32)(esi + 0x84c) = (uint32)(0x3DA3D70Au);   // 6d9af4: mov dword ptr [esi + 0x84c], 0x3da3d70a
L_6d9afe:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x84c)));   // 6d9afe: fld dword ptr [esi + 0x84c]
L_6d9b04:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9b04: fld dword ptr [esp + 0x1c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9b08: mov edx, dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b0e: fmul dword ptr [0x863984]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9b14: mov eax, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = edx;   // 6d9b1a: push edx
    esp -= 4; M32(esp) = eax;   // 6d9b1b: push eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6d9b1c: fstp dword ptr [esp + 0x3c]
    esp -= 4; M32(esp) = ecx;   // 6d9b20: push ecx
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9b21: fld dword ptr [esp + 0x2c]
    ecx = (uint32)(esp + 0x40);   // 6d9b25: lea ecx, [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b29: fmul dword ptr [0x863984]
    edx = (uint32)(esp + 0x224);   // 6d9b2f: lea edx, [esp + 0x224]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9b36: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9b3a: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9b3e: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9b44: fstp dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34e0)));   // ST0 = ST0 * m   // 6d9b48: fmul dword ptr [0x8d34e0]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9b4e: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x8d34e4)));   // ST0 = ST0 * m   // 6d9b52: fmul dword ptr [0x8d34e4]
    fp.Arith('-', 1, 1, 0, true);   // 6d9b58: fsubp st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9b5a: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6d9b5d: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9b5e: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b5f: call 0x40fec0
    esp += 0xCu;   // 6d9b64: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9b67: push eax
    eax = (uint32)(esp + 0x17c);   // 6d9b68: lea eax, [esp + 0x17c]
    esp -= 4; M32(esp) = eax;   // 6d9b6f: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b70: call 0x40fec0
    esp += 0xCu;   // 6d9b75: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9b78: push eax
    ecx = (uint32)(esp + 0xa0);   // 6d9b79: lea ecx, [esp + 0xa0]
    esp -= 4; M32(esp) = ecx;   // 6d9b80: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9b81: call 0x40fec0
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9b86: mov ecx, dword ptr [eax]
    edx = esp;   // 6d9b88: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6d9b8a: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9b8c: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9b8f: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6d9b92: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6d9b95: mov dword ptr [edx + 8], eax
    goto L_6d9ef2;   // 6d9b98: jmp 0x6d9ef2
L_6d9b9d:;
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9b9d: mov edi, dword ptr [esi + 0x14]
    ecx = (uint32)(edi + 0x20);   // 6d9ba0: lea ecx, [edi + 0x20]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx));   // 6d9ba3: mov edx, dword ptr [ecx]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 4));   // 6d9ba5: mov eax, dword ptr [ecx + 4]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 8));   // 6d9ba8: mov ecx, dword ptr [ecx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(eax);   // 6d9bab: mov dword ptr [esp + 0x20], eax
    fA = (uint32)(*(uint8*)(uintptr_t)(uint32)(esi + 0x42b)); fB = 0x10u; fR = (fA & fB) & 0xFFu;   // 6d9baf: test byte ptr [esi + 0x42b], 0x10
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(edx);   // 6d9bb6: mov dword ptr [esp + 0x1c], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(ecx);   // 6d9bba: mov dword ptr [esp + 0x24], ecx
    if (fR == 0) goto L_6d9c04;   // 6d9bbe: je 0x6d9c04
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9bc0: fld dword ptr [esp + 0x1c]
    FlcCrtAsin(fp);   // 6d9bc4: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bc9: fmul dword ptr [0x858b90]
    fp.Fsin();   // 6d9bcf: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9bd1: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9bd5: fld dword ptr [esp + 0x20]
    FlcCrtAsin(fp);   // 6d9bd9: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bde: fmul dword ptr [0x858b90]
    fp.Fsin();   // 6d9be4: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9be6: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9bea: fld dword ptr [esp + 0x24]
    FlcCrtAcos(fp);   // 6d9bee: call 0x822380
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b90)));   // ST0 = ST0 * m   // 6d9bf3: fmul dword ptr [0x858b90]
    fp.Fcos();   // 6d9bf9: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6d9bfb: fstp dword ptr [esp + 0x24]
    goto L_6d9c95;   // 6d9bff: jmp 0x6d9c95
L_6d9c04:;
    fA = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x22)); fB = 0x208u; fR = (fA - fB) & 0xFFFFu;   // 6d9c04: cmp word ptr [esi + 0x22], 0x208
    if (fR != 0) goto L_6d9c95;   // 6d9c0a: jne 0x6d9c95
    edx = (uint32)(*(uint16*)(uintptr_t)(uint32)(esi + 0x86c));   // 6d9c10: movzx edx, word ptr [esi + 0x86c]
    eax = (uint32)(int32_t)(int16_t)((uint32)(*(uint16*)(uintptr_t)(uint32)(0x8d33c4)));   // 6d9c17: movsx eax, word ptr [0x8d33c4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6d9c1e: mov dword ptr [esp + 0x18], edx
    ecx = (uint32)(edi + 0x20);   // 6d9c22: lea ecx, [edi + 0x20]
    esp -= 4; M32(esp) = ecx;   // 6d9c25: push ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9c26: fild dword ptr [esp + 0x1c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(eax);   // 6d9c2a: mov dword ptr [esp + 0x1c], eax
    esp -= 4; M32(esp) = ecx;   // 6d9c2e: push ecx
    edx = (uint32)(esp + 0xac);   // 6d9c2f: lea edx, [esp + 0xac]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858fe4)));   // ST0 = ST0 * m   // 6d9c36: fmul dword ptr [0x858fe4]
    fp.FidivI((*(int*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9c3c: fidiv dword ptr [esp + 0x20]
    fp.FldSt(0);   // 6d9c40: fld st(0)
    fp.Fcos();   // 6d9c42: fcos 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6d9c44: fstp dword ptr [esp + 0x20]
    fp.Fsin();   // 6d9c48: fsin 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9c4a: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6d9c4d: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c4e: call 0x40fe90
    esp += 0xCu;   // 6d9c53: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9c56: push eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x1c));   // 6d9c57: mov eax, dword ptr [esp + 0x1c]
    fA = edi; fB = 0x10u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6d9c5b: add edi, 0x10
    esp -= 4; M32(esp) = edi;   // 6d9c5e: push edi
    esp -= 4; M32(esp) = eax;   // 6d9c5f: push eax
    ecx = (uint32)(esp + 0xc8);   // 6d9c60: lea ecx, [esp + 0xc8]
    esp -= 4; M32(esp) = ecx;   // 6d9c67: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c68: call 0x40fe90
    esp += 0xCu;   // 6d9c6d: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9c70: push eax
    edx = (uint32)(esp + 0xdc);   // 6d9c71: lea edx, [esp + 0xdc]
    esp -= 4; M32(esp) = edx;   // 6d9c78: push edx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9c79: call 0x40fe30
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9c7e: mov ecx, dword ptr [eax]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9c80: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9c83: mov eax, dword ptr [eax + 8]
    esp += 0xCu;   // 6d9c86: add esp, 0xc
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6d9c89: mov dword ptr [esp + 0x1c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(edx);   // 6d9c8d: mov dword ptr [esp + 0x20], edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(eax);   // 6d9c91: mov dword ptr [esp + 0x24], eax
L_6d9c95:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9c95: fld dword ptr [esp + 0x48]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // ST0 = ST0 * m   // 6d9c99: fmul dword ptr [esp + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6d9c9d: fld dword ptr [esp + 0x44]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6d9ca1: fmul dword ptr [esp + 0x20]
    fp.Arith('+', 1, 1, 0, true);   // 6d9ca5: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9ca7: fld dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // ST0 = ST0 * m   // 6d9cab: fmul dword ptr [esp + 0x40]
    fp.Arith('+', 1, 1, 0, true);   // 6d9caf: faddp st(1)
    fp.Fst((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cb1: fst dword ptr [esp + 0x10]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9cb5: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9cbb: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9cbd: test ah, 0x41
    if (fR != 0) goto L_6d9ccc;   // 6d9cc0: jne 0x6d9ccc
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cc2: fld dword ptr [esp + 0x10]
    fp.Arith('+', 0, 0, 0, false);   // 6d9cc6: fadd st(0), st(0)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9cc8: fstp dword ptr [esp + 0x10]
L_6d9ccc:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9ccc: fld dword ptr [esp + 0x274]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6d9cd3: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9cd9: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6d9cdb: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6d9d5a;   // 6d9cde: jp 0x6d9d5a
    fA = ebx; fB = ebx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9ce0: test ebx, ebx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x274) = (uint32)(0x0u);   // 6d9ce2: mov dword ptr [esp + 0x274], 0
    if (fR == 0) goto L_6d9d5a;   // 6d9ced: je 0x6d9d5a
    ecx = ebx;   // 6d9cef: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetAccelerate();   // 6d9cf1: call 0x5403f0
    ecx = ebx;   // 6d9cf6: mov ecx, ebx
    edi = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9cf8: movsx edi, ax
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetBrake();   // 6d9cfb: call 0x540080
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d00: movsx ecx, ax
    fA = edi; fB = ecx; fR = (fA - fB) & 0xFFFFFFFFu; edi = fR;   // 6d9d03: sub edi, ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edi);   // 6d9d05: mov dword ptr [esp + 0x18], edi
    ecx = ebx;   // 6d9d09: mov ecx, ebx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d0b: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x859a3c)));   // ST0 = ST0 * m   // 6d9d0f: fmul dword ptr [0x859a3c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d15: fstp dword ptr [esp + 0x274]
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d9d1c: call 0x53fc10
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d21: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6d9d24: mov dword ptr [esp + 0x18], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d28: fild dword ptr [esp + 0x18]
    fp.Fabs();   // 6d9d2c: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6d9d2e: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9d34: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9d36: test ah, 0x41
    if (fR != 0) goto L_6d9d5a;   // 6d9d39: jne 0x6d9d5a
    ecx = ebx;   // 6d9d3b: mov ecx, ebx
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunUpDown();   // 6d9d3d: call 0x53fc10
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6d9d42: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(eax);   // 6d9d45: mov dword ptr [esp + 0x18], eax
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9d49: fild dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6d9d4d: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d53: fstp dword ptr [esp + 0x274]
L_6d9d5a:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9d5a: mov ecx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x274)));   // 6d9d60: fld dword ptr [esp + 0x274]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6d9d67: fmul dword ptr [ecx + 4]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9d6a: mov edx, dword ptr [0xb7cb5c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9d70: mov eax, dword ptr [esi + 0x8c]
    esp -= 4; M32(esp) = edx;   // 6d9d76: push edx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x8631d0)));   // ST0 = ST0 + m   // 6d9d77: fadd dword ptr [0x8631d0]
    esp -= 4; M32(esp) = eax;   // 6d9d7d: push eax
    esp -= 4; M32(esp) = 0x3C03126Fu;   // 6d9d7e: push 0x3c03126f
    esp -= 4; M32(esp) = ecx;   // 6d9d83: push ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9d84: fstp dword ptr [esp + 0x28]
    ecx = (uint32)(esp + 0x44);   // 6d9d88: lea ecx, [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(0x858b8c)));   // 6d9d8c: fld dword ptr [0x858b8c]
    edx = (uint32)(esp + 0xfc);   // 6d9d92: lea edx, [esp + 0xfc]
    fp.Fsub((*(float*)(uintptr_t)(uint32)(esi + 0x4c)));   // ST0 = ST0 - m   // 6d9d99: fsub dword ptr [esi + 0x4c]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x44) = (uint32)(0x0u);   // 6d9d9c: mov dword ptr [esp + 0x44], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x48) = (uint32)(0x0u);   // 6d9da4: mov dword ptr [esp + 0x48], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x4c) = (uint32)(0x3F800000u);   // 6d9dac: mov dword ptr [esp + 0x4c], 0x3f800000
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6d9db4: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6d9db7: push ecx
    esp -= 4; M32(esp) = edx;   // 6d9db8: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9db9: call 0x40fec0
    esp += 0xCu;   // 6d9dbe: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9dc1: push eax
    eax = (uint32)(esp + 0x114);   // 6d9dc2: lea eax, [esp + 0x114]
    esp -= 4; M32(esp) = eax;   // 6d9dc9: push eax
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9dca: call 0x40fec0
    esp += 0xCu;   // 6d9dcf: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9dd2: push eax
    ecx = (uint32)(esp + 0x128);   // 6d9dd3: lea ecx, [esp + 0x128]
    esp -= 4; M32(esp) = ecx;   // 6d9dda: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9ddb: call 0x40fec0
    esp += 0xCu;   // 6d9de0: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9de3: push eax
    edx = (uint32)(esp + 0x13c);   // 6d9de4: lea edx, [esp + 0x13c]
    esp -= 4; M32(esp) = edx;   // 6d9deb: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9dec: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9df1: mov edx, dword ptr [eax]
    ecx = esp;   // 6d9df3: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9df5: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9df7: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9dfa: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9dfd: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9e00: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6d9e03: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9e05: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6d9e0a: fld dword ptr [esp + 0x10]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9e0e: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6d9e14: fmul dword ptr [ecx + 8]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9e17: mov edx, dword ptr [esi + 0x14]
    fA = edx; fB = edx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9e1a: test edx, edx
    eax = (uint32)(edx + 0x30);   // 6d9e1c: lea eax, [edx + 0x30]
    fp.Fsubr((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = m - ST0   // 6d9e1f: fsubr dword ptr [esp + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9e23: fstp dword ptr [esp + 0x18]
    if (fR != 0) goto L_6d9e2c;   // 6d9e27: jne 0x6d9e2c
    eax = (uint32)(esi + 4);   // 6d9e29: lea eax, [esi + 4]
L_6d9e2c:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9e2c: mov eax, dword ptr [eax + 8]
    esp -= 4; M32(esp) = ebp;   // 6d9e2f: push ebp
    esp -= 4; M32(esp) = eax;   // 6d9e30: push eax
    ecx = esi;   // 6d9e31: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9e33: call 0x6d2600
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9e38: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9e3e: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9e40: test ah, 0x41
    if (fR != 0) goto L_6d9e6f;   // 6d9e43: jne 0x6d9e6f
    fA = edx; fB = edx; fR = (fA & fB) & 0xFFFFFFFFu;   // 6d9e45: test edx, edx
    eax = (uint32)(edx + 0x30);   // 6d9e47: lea eax, [edx + 0x30]
    if (fR != 0) goto L_6d9e4f;   // 6d9e4a: jne 0x6d9e4f
    eax = (uint32)(esi + 4);   // 6d9e4c: lea eax, [esi + 4]
L_6d9e4f:;
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9e4f: mov ecx, dword ptr [eax + 8]
    esp -= 4; M32(esp) = ebp;   // 6d9e52: push ebp
    esp -= 4; M32(esp) = ecx;   // 6d9e53: push ecx
    ecx = esi;   // 6d9e54: mov ecx, esi
    { const float h_ = v.HeightAboveCeiling((*(float*)(uintptr_t)(esp + 0)), (eFlightModel)(int)M32(esp + 4)); fp.FldRet(h_); esp += 8; }   // 6d9e56: call 0x6d2600
    fp.Fadd((*(float*)(uintptr_t)(uint32)(0x85862c)));   // ST0 = ST0 + m   // 6d9e5b: fadd dword ptr [0x85862c]
    fp.Fdivr((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = m / ST0   // 6d9e61: fdivr dword ptr [esp + 0x18]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x85862c)));   // ST0 = ST0 * m   // 6d9e65: fmul dword ptr [0x85862c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9e6b: fstp dword ptr [esp + 0x18]
L_6d9e6f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6d9e6f: fld dword ptr [esp + 0x1c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(0xb7cb5c));   // 6d9e73: mov edx, dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9e79: fmul dword ptr [0x863984]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6d9e7f: mov eax, dword ptr [esi + 0x8c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x18));   // 6d9e85: mov ecx, dword ptr [esp + 0x18]
    esp -= 4; M32(esp) = edx;   // 6d9e89: push edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6d9e8a: fstp dword ptr [esp + 0x38]
    esp -= 4; M32(esp) = eax;   // 6d9e8e: push eax
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9e8f: fld dword ptr [esp + 0x28]
    esp -= 4; M32(esp) = ecx;   // 6d9e93: push ecx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9e94: fmul dword ptr [0x863984]
    edx = (uint32)(esp + 0x40);   // 6d9e9a: lea edx, [esp + 0x40]
    esp -= 4; M32(esp) = edx;   // 6d9e9e: push edx
    eax = (uint32)(esp + 0x15c);   // 6d9e9f: lea eax, [esp + 0x15c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6d9ea6: fstp dword ptr [esp + 0x48]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6d9eaa: fld dword ptr [esp + 0x34]
    esp -= 4; M32(esp) = eax;   // 6d9eae: push eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x863984)));   // ST0 = ST0 * m   // 6d9eaf: fmul dword ptr [0x863984]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6d9eb5: fstp dword ptr [esp + 0x50]
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9eb9: call 0x40fec0
    esp += 0xCu;   // 6d9ebe: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9ec1: push eax
    ecx = (uint32)(esp + 0x170);   // 6d9ec2: lea ecx, [esp + 0x170]
    esp -= 4; M32(esp) = ecx;   // 6d9ec9: push ecx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9eca: call 0x40fec0
    esp += 0xCu;   // 6d9ecf: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9ed2: push eax
    edx = (uint32)(esp + 0x184);   // 6d9ed3: lea edx, [esp + 0x184]
    esp -= 4; M32(esp) = edx;   // 6d9eda: push edx
    FlcVecMulRight(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (*(float*)(uintptr_t)(esp + 8))); eax = M32(esp + 0);   // 6d9edb: call 0x40fec0
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6d9ee0: mov edx, dword ptr [eax]
    ecx = esp;   // 6d9ee2: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6d9ee4: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6d9ee6: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6d9ee9: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6d9eec: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6d9eef: mov dword ptr [ecx + 8], eax
L_6d9ef2:;
    ecx = esi;   // 6d9ef2: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6d9ef4: call 0x5429f0
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9ef9: mov ebx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x28)));   // 6d9efc: fld dword ptr [ebx + 0x28]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6d9eff: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9f05: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9f07: test ah, 0x41
    if (fR != 0) goto L_6da142;   // 6d9f0a: jne 0x6da142
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9f10: mov ecx, dword ptr [esi + 0x388]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x28) = (uint32)(0x0u);   // 6d9f16: mov dword ptr [esp + 0x28], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(0x0u);   // 6d9f1e: mov dword ptr [esp + 0x2c], 0
    *(uint32*)(uintptr_t)(uint32)(esp + 0x30) = (uint32)(0x3F800000u);   // 6d9f26: mov dword ptr [esp + 0x30], 0x3f800000
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 0x38));   // 6d9f2e: mov edx, dword ptr [ecx + 0x38]
    esp -= 4; M32(esp) = 0xC813E0u;   // 6d9f31: push 0xc813e0
    esp -= 4; M32(esp) = edx;   // 6d9f36: push edx
    eax = (uint32)(esp + 0x19c);   // 6d9f37: lea eax, [esp + 0x19c]
    esp -= 4; M32(esp) = eax;   // 6d9f3e: push eax
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6d9f3f: call 0x40fe90
    esp += 0xCu;   // 6d9f44: add esp, 0xc
    esp -= 4; M32(esp) = eax;   // 6d9f47: push eax
    ecx = (uint32)(esp + 0x2c);   // 6d9f48: lea ecx, [esp + 0x2c]
    FlcVecAddTo(fp, (float*)(uintptr_t)ecx, (float*)(uintptr_t)M32(esp + 0)); eax = ecx; esp += 4;   // 6d9f4c: call 0x411a00
    ecx = (uint32)(esp + 0x28);   // 6d9f51: lea ecx, [esp + 0x28]
    FlcVecNormalise(fp, (float*)(uintptr_t)ecx);   // 6d9f55: call 0x59c910
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6d9f5a: mov ebp, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6d9f60: fld dword ptr [ebp + 0x28]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6d9f63: mov edi, dword ptr [esi + 0x14]
    fp.Fchs();   // 6d9f66: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9f68: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9f6c: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9f6f: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9f73: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f76: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9f78: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9f7c: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f7e: faddp st(1)
    fp.FldSt(1);   // 6d9f80: fld st(1)
    fp.Fcompp();   // 6d9f82: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9f84: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9f86: test ah, 0x41
    if (fR != 0) goto L_6d9f8f;   // 6d9f89: jne 0x6d9f8f
    fp.FldSt(0);   // 6d9f8b: fld st(0)
    goto L_6d9fa7;   // 6d9f8d: jmp 0x6d9fa7
L_6d9f8f:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9f8f: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9f93: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9f96: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9f9a: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9f9d: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9f9f: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9fa3: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fa5: faddp st(1)
L_6d9fa7:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(ebp + 0x28)), true);   // 6d9fa7: fcomp dword ptr [ebp + 0x28]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9faa: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9fac: test ah, 0x41
    if (fR != 0) goto L_6d9fbc;   // 6d9faf: jne 0x6d9fbc
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6d9fb1: mov ecx, dword ptr [ebp + 0x28]
    fp.FstpSt(0);   // 6d9fb4: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6d9fb6: mov dword ptr [esp + 0x18], ecx
    goto L_6d9ffd;   // 6d9fba: jmp 0x6d9ffd
L_6d9fbc:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9fbc: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9fc0: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9fc3: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9fc7: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fca: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9fcc: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9fd0: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fd2: faddp st(1)
    fp.FldSt(1);   // 6d9fd4: fld st(1)
    fp.Fcompp();   // 6d9fd6: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6d9fd8: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6d9fda: test ah, 0x41
    if (fR == 0) goto L_6d9ff9;   // 6d9fdd: je 0x6d9ff9
    fp.FstpSt(0);   // 6d9fdf: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6d9fe1: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 8)));   // ST0 = ST0 * m   // 6d9fe5: fmul dword ptr [edi + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6d9fe8: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 4)));   // ST0 = ST0 * m   // 6d9fec: fmul dword ptr [edi + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6d9fef: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6d9ff1: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi)));   // ST0 = ST0 * m   // 6d9ff5: fmul dword ptr [edi]
    fp.Arith('+', 1, 1, 0, true);   // 6d9ff7: faddp st(1)
L_6d9ff9:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6d9ff9: fstp dword ptr [esp + 0x18]
L_6d9ffd:;
    edx = (uint32)(esp + 0x50);   // 6d9ffd: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da001: push edx
    eax = (uint32)(esp + 0x1b0);   // 6da002: lea eax, [esp + 0x1b0]
    esp -= 4; M32(esp) = edi;   // 6da009: push edi
    esp -= 4; M32(esp) = eax;   // 6da00a: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da00b: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da010: fld dword ptr [esp + 0x24]
    fp.Fchs();   // 6da014: fchs 
    esp += 0xCu;   // 6da016: add esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da019: fmul dword ptr [ebp + 0x2c]
    fA = edi; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; edi = fR;   // 6da01c: add edi, 0x20
    esp -= 4; M32(esp) = edi;   // 6da01f: push edi
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da020: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da026: push ecx
    ecx = (uint32)(esp + 0x1cc);   // 6da027: lea ecx, [esp + 0x1cc]
    ebx = eax;   // 6da02e: mov ebx, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da030: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da036: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6da039: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da03a: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx));   // 6da03f: mov ecx, dword ptr [ebx]
    edx = esp;   // 6da041: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da043: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 4));   // 6da045: mov ecx, dword ptr [ebx + 4]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da048: mov dword ptr [edx + 4], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 8));   // 6da04b: mov ecx, dword ptr [ebx + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(ecx);   // 6da04e: mov dword ptr [edx + 8], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da051: mov ecx, dword ptr [eax]
    esp -= 0xCu;   // 6da053: sub esp, 0xc
    edx = esp;   // 6da056: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da058: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da05a: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da05d: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da060: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da063: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da065: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da068: call 0x542a50
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da06d: mov ebx, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x28)));   // 6da073: fld dword ptr [ebx + 0x28]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da076: mov ebp, dword ptr [esi + 0x14]
    fp.Fchs();   // 6da079: fchs 
    ecx = (uint32)(ebp + 0x10);   // 6da07b: lea ecx, [ebp + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da07e: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da082: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da085: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da089: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da08c: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da08e: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da092: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da094: faddp st(1)
    fp.FldSt(1);   // 6da096: fld st(1)
    fp.Fcompp();   // 6da098: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da09a: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da09c: test ah, 0x41
    if (fR != 0) goto L_6da0a5;   // 6da09f: jne 0x6da0a5
    fp.FldSt(0);   // 6da0a1: fld st(0)
    goto L_6da0bd;   // 6da0a3: jmp 0x6da0bd
L_6da0a5:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0a5: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0a9: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0ac: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da0b0: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da0b3: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da0b5: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da0b9: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da0bb: faddp st(1)
L_6da0bd:;
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(ebx + 0x28)), true);   // 6da0bd: fcomp dword ptr [ebx + 0x28]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da0c0: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da0c2: test ah, 0x41
    if (fR != 0) goto L_6da0d2;   // 6da0c5: jne 0x6da0d2
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebx + 0x28));   // 6da0c7: mov edx, dword ptr [ebx + 0x28]
    fp.FstpSt(0);   // 6da0ca: fstp st(0)
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(edx);   // 6da0cc: mov dword ptr [esp + 0x18], edx
    goto L_6da113;   // 6da0d0: jmp 0x6da113
L_6da0d2:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0d2: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0d6: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0d9: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da0dd: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da0e0: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da0e2: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da0e6: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da0e8: faddp st(1)
    fp.FldSt(1);   // 6da0ea: fld st(1)
    fp.Fcompp();   // 6da0ec: fcompp 
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da0ee: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da0f0: test ah, 0x41
    if (fR == 0) goto L_6da10f;   // 6da0f3: je 0x6da10f
    fp.FstpSt(0);   // 6da0f5: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da0f7: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 8)));   // ST0 = ST0 * m   // 6da0fb: fmul dword ptr [ecx + 8]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da0fe: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 4)));   // ST0 = ST0 * m   // 6da102: fmul dword ptr [ecx + 4]
    fp.Arith('+', 1, 1, 0, true);   // 6da105: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da107: fld dword ptr [esp + 0x28]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx)));   // ST0 = ST0 * m   // 6da10b: fmul dword ptr [ecx]
    fp.Arith('+', 1, 1, 0, true);   // 6da10d: faddp st(1)
L_6da10f:;
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da10f: fstp dword ptr [esp + 0x18]
L_6da113:;
    eax = (uint32)(esp + 0x50);   // 6da113: lea eax, [esp + 0x50]
    esp -= 4; M32(esp) = eax;   // 6da117: push eax
    esp -= 4; M32(esp) = ecx;   // 6da118: push ecx
    ecx = (uint32)(esp + 0x1e4);   // 6da119: lea ecx, [esp + 0x1e4]
    esp -= 4; M32(esp) = ecx;   // 6da120: push ecx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da121: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da126: fld dword ptr [esp + 0x24]
    esp += 0xCu;   // 6da12a: add esp, 0xc
    fp.Fchs();   // 6da12d: fchs 
    fA = ebp; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebp = fR;   // 6da12f: add ebp, 0x20
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebx + 0x2c)));   // ST0 = ST0 * m   // 6da132: fmul dword ptr [ebx + 0x2c]
    esp -= 4; M32(esp) = ebp;   // 6da135: push ebp
    edx = (uint32)(esp + 0x1f8);   // 6da136: lea edx, [esp + 0x1f8]
    goto L_6da22e;   // 6da13d: jmp 0x6da22e
L_6da142:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 8)));   // 6da142: fld dword ptr [ebx + 8]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da145: mov ebp, dword ptr [esi + 0x388]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6da14b: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da151: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6da153: test ah, 5
    if (FlcParityEven(fR)) goto L_6da161;   // 6da156: jp 0x6da161
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6da158: mov ecx, dword ptr [ebp + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6da15b: mov dword ptr [esp + 0x18], ecx
    goto L_6da16a;   // 6da15f: jmp 0x6da16a
L_6da161:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6da161: fld dword ptr [ebp + 0x28]
    fp.Fchs();   // 6da164: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da166: fstp dword ptr [esp + 0x18]
L_6da16a:;
    edx = (uint32)(esp + 0x50);   // 6da16a: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da16e: push edx
    eax = (uint32)(esp + 0x210);   // 6da16f: lea eax, [esp + 0x210]
    esp -= 4; M32(esp) = ebx;   // 6da176: push ebx
    esp -= 4; M32(esp) = eax;   // 6da177: push eax
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da178: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da17d: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da181: fmul dword ptr [ebp + 0x2c]
    esp += 0xCu;   // 6da184: add esp, 0xc
    fA = ebx; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebx = fR;   // 6da187: add ebx, 0x20
    esp -= 4; M32(esp) = ebx;   // 6da18a: push ebx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da18b: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da191: push ecx
    ecx = (uint32)(esp + 0x22c);   // 6da192: lea ecx, [esp + 0x22c]
    edi = eax;   // 6da199: mov edi, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da19b: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da1a1: fstp dword ptr [esp]
    esp -= 4; M32(esp) = ecx;   // 6da1a4: push ecx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da1a5: call 0x40fe90
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi));   // 6da1aa: mov ecx, dword ptr [edi]
    edx = esp;   // 6da1ac: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da1ae: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 4));   // 6da1b0: mov ecx, dword ptr [edi + 4]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da1b3: mov dword ptr [edx + 4], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 8));   // 6da1b6: mov ecx, dword ptr [edi + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(ecx);   // 6da1b9: mov dword ptr [edx + 8], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da1bc: mov ecx, dword ptr [eax]
    esp -= 0xCu;   // 6da1be: sub esp, 0xc
    edx = esp;   // 6da1c1: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(ecx);   // 6da1c3: mov dword ptr [edx], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da1c5: mov ecx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da1c8: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da1cb: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da1ce: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da1d0: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da1d3: call 0x542a50
    ebx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da1d8: mov ebx, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebx + 0x18)));   // 6da1db: fld dword ptr [ebx + 0x18]
    ebp = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da1de: mov ebp, dword ptr [esi + 0x388]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858b50)), true);   // 6da1e4: fcomp dword ptr [0x858b50]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da1ea: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x5u; fR = (fA & fB) & 0xFFu;   // 6da1ec: test ah, 5
    if (FlcParityEven(fR)) goto L_6da1fa;   // 6da1ef: jp 0x6da1fa
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ebp + 0x28));   // 6da1f1: mov ecx, dword ptr [ebp + 0x28]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x18) = (uint32)(ecx);   // 6da1f4: mov dword ptr [esp + 0x18], ecx
    goto L_6da203;   // 6da1f8: jmp 0x6da203
L_6da1fa:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(ebp + 0x28)));   // 6da1fa: fld dword ptr [ebp + 0x28]
    fp.Fchs();   // 6da1fd: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da1ff: fstp dword ptr [esp + 0x18]
L_6da203:;
    edx = (uint32)(esp + 0x50);   // 6da203: lea edx, [esp + 0x50]
    esp -= 4; M32(esp) = edx;   // 6da207: push edx
    eax = (uint32)(ebx + 0x10);   // 6da208: lea eax, [ebx + 0x10]
    esp -= 4; M32(esp) = eax;   // 6da20b: push eax
    ecx = (uint32)(esp + 0x244);   // 6da20c: lea ecx, [esp + 0x244]
    esp -= 4; M32(esp) = ecx;   // 6da213: push ecx
    FlcVecAdd(fp, (float*)(uintptr_t)M32(esp + 0), (float*)(uintptr_t)M32(esp + 4), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da214: call 0x40fe30
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da219: fld dword ptr [esp + 0x24]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ebp + 0x2c)));   // ST0 = ST0 * m   // 6da21d: fmul dword ptr [ebp + 0x2c]
    esp += 0xCu;   // 6da220: add esp, 0xc
    fA = ebx; fB = 0x20u; fR = (fA + fB) & 0xFFFFFFFFu; ebx = fR;   // 6da223: add ebx, 0x20
    esp -= 4; M32(esp) = ebx;   // 6da226: push ebx
    edx = (uint32)(esp + 0x84);   // 6da227: lea edx, [esp + 0x84]
L_6da22e:;
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da22e: fmul dword ptr [0xb7cb5c]
    esp -= 4; M32(esp) = ecx;   // 6da234: push ecx
    edi = eax;   // 6da235: mov edi, eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da237: fmul dword ptr [esi + 0x90]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp)));   // 6da23d: fstp dword ptr [esp]
    esp -= 4; M32(esp) = edx;   // 6da240: push edx
    FlcVecMulLeft(fp, (float*)(uintptr_t)M32(esp + 0), (*(float*)(uintptr_t)(esp + 4)), (float*)(uintptr_t)M32(esp + 8)); eax = M32(esp + 0);   // 6da241: call 0x40fe90
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi));   // 6da246: mov edx, dword ptr [edi]
    ecx = esp;   // 6da248: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da24a: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 4));   // 6da24c: mov edx, dword ptr [edi + 4]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6da24f: mov dword ptr [ecx + 4], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edi + 8));   // 6da252: mov edx, dword ptr [edi + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da255: mov dword ptr [ecx + 8], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da258: mov edx, dword ptr [eax]
    esp -= 0xCu;   // 6da25a: sub esp, 0xc
    ecx = esp;   // 6da25d: mov ecx, esp
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da25f: mov dword ptr [ecx], edx
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da261: mov edx, dword ptr [eax + 4]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da264: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(edx);   // 6da267: mov dword ptr [ecx + 4], edx
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(eax);   // 6da26a: mov dword ptr [ecx + 8], eax
    ecx = esi;   // 6da26d: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da26f: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da274: fld dword ptr [esp + 0x26c]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da27b: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFFFF00u) | (((uint32)(*(uint8*)(uintptr_t)(uint32)(0xc1c970))) & 0xFFu);   // 6da281: mov al, byte ptr [0xc1c970]
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x14));   // 6da286: mov edi, dword ptr [esp + 0x14]
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da28a: test al, al
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da28c: fnstsw ax
    if (fR != 0) goto L_6da3a1;   // 6da28e: jne 0x6da3a1
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da294: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da2c7;   // 6da297: jp 0x6da2c7
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da299: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6da29b: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6da2c7;   // 6da2a6: je 0x6da2c7
    ecx = edi;   // 6da2a8: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6da2aa: call 0x53fbd0
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da2af: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da2b2: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da2b6: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da2ba: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da2c0: fstp dword ptr [esp + 0x26c]
L_6da2c7:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da2c7: fld dword ptr [esp + 0x270]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da2ce: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da2d4: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da2d6: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da30b;   // 6da2d9: jp 0x6da30b
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da2db: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0x0u);   // 6da2dd: mov dword ptr [esp + 0x270], 0
    if (fR == 0) goto L_6da30b;   // 6da2e8: je 0x6da30b
    ecx = edi;   // 6da2ea: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6da2ec: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da2f1: movsx edx, ax
    fR = (uint32)(0 - edx); edx = fR;   // 6da2f4: neg edx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da2f6: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da2fa: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da2fe: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da304: fstp dword ptr [esp + 0x270]
L_6da30b:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da30b: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da312: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da318: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da31a: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da468;   // 6da31d: jp 0x6da468
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da323: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6da325: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6da468;   // 6da330: je 0x6da468
    ecx = edi;   // 6da336: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookRight() ? 1u : 0u;   // 6da338: call 0x53fe10
    eax = (eax & 0xFFu);   // 6da33d: movzx eax, al
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6da340: mov dword ptr [esp + 0x14], eax
    ecx = edi;   // 6da344: mov ecx, edi
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da346: fild dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da34a: fstp dword ptr [esp + 0x268]
    eax = ((CPad*)(uintptr_t)ecx)->GetLookLeft() ? 1u : 0u;   // 6da351: call 0x53fdd0
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da356: test al, al
    if (fR == 0) goto L_6da365;   // 6da358: je 0x6da365
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0xBF800000u);   // 6da35a: mov dword ptr [esp + 0x268], 0xbf800000
L_6da365:;
    ecx = edi;   // 6da365: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunLeftRight();   // 6da367: call 0x53fc50
    ecx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da36c: movsx ecx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da36f: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da373: fild dword ptr [esp + 0x14]
    fp.Fabs();   // 6da377: fabs 
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x858624)), true);   // 6da379: fcomp dword ptr [0x858624]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da37f: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x41u; fR = (fA & fB) & 0xFFu;   // 6da381: test ah, 0x41
    if (fR != 0) goto L_6da468;   // 6da384: jne 0x6da468
    ecx = edi;   // 6da38a: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetCarGunLeftRight();   // 6da38c: call 0x53fc50
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da391: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da394: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da398: fild dword ptr [esp + 0x14]
    goto L_6da45b;   // 6da39c: jmp 0x6da45b
L_6da3a1:;
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da3a1: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da3d4;   // 6da3a4: jp 0x6da3d4
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da3a6: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x26c) = (uint32)(0x0u);   // 6da3a8: mov dword ptr [esp + 0x26c], 0
    if (fR == 0) goto L_6da3d4;   // 6da3b3: je 0x6da3d4
    ecx = edi;   // 6da3b5: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringUpDown();   // 6da3b7: call 0x53fbd0
    eax = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da3bc: movsx eax, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(eax);   // 6da3bf: mov dword ptr [esp + 0x14], eax
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da3c3: fild dword ptr [esp + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da3c7: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da3cd: fstp dword ptr [esp + 0x26c]
L_6da3d4:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da3d4: fld dword ptr [esp + 0x270]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da3db: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da3e1: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da3e3: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da410;   // 6da3e6: jp 0x6da410
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da3e8: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0x0u);   // 6da3ea: mov dword ptr [esp + 0x270], 0
    if (fR == 0) goto L_6da410;   // 6da3f5: je 0x6da410
    ecx = edi;   // 6da3f7: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookLeft() ? 1u : 0u;   // 6da3f9: call 0x53fdd0
    ecx = (eax & 0xFFu);   // 6da3fe: movzx ecx, al
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(ecx);   // 6da401: mov dword ptr [esp + 0x14], ecx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da405: fild dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da409: fstp dword ptr [esp + 0x270]
L_6da410:;
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da410: fld dword ptr [esp + 0x268]
    fp.FcomMem((*(float*)(uintptr_t)(uint32)(0x871f8c)), true);   // 6da417: fcomp dword ptr [0x871f8c]
    eax = (eax & 0xFFFF0000u) | (fp.Ah() << 8);   // 6da41d: fnstsw ax
    fA = ((eax >> 8) & 0xFFu); fB = 0x44u; fR = (fA & fB) & 0xFFu;   // 6da41f: test ah, 0x44
    if (FlcParityEven(fR)) goto L_6da468;   // 6da422: jp 0x6da468
    fA = edi; fB = edi; fR = (fA & fB) & 0xFFFFFFFFu;   // 6da424: test edi, edi
    *(uint32*)(uintptr_t)(uint32)(esp + 0x268) = (uint32)(0x0u);   // 6da426: mov dword ptr [esp + 0x268], 0
    if (fR == 0) goto L_6da468;   // 6da431: je 0x6da468
    ecx = edi;   // 6da433: mov ecx, edi
    eax = ((CPad*)(uintptr_t)ecx)->GetLookRight() ? 1u : 0u;   // 6da435: call 0x53fe10
    fA = (eax & 0xFFu); fB = (eax & 0xFFu); fR = (fA & fB) & 0xFFu;   // 6da43a: test al, al
    if (fR == 0) goto L_6da449;   // 6da43c: je 0x6da449
    *(uint32*)(uintptr_t)(uint32)(esp + 0x270) = (uint32)(0xBF800000u);   // 6da43e: mov dword ptr [esp + 0x270], 0xbf800000
L_6da449:;
    ecx = edi;   // 6da449: mov ecx, edi
    eax = (uint32)(int32)((CPad*)(uintptr_t)ecx)->GetSteeringLeftRight();   // 6da44b: call 0x53fb80
    edx = (uint32)(int32_t)(int16_t)((eax & 0xFFFFu));   // 6da450: movsx edx, ax
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da453: mov dword ptr [esp + 0x14], edx
    fp.FldI((*(int*)(uintptr_t)(uint32)(esp + 0x14)));   // 6da457: fild dword ptr [esp + 0x14]
L_6da45b:;
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858b88)));   // ST0 = ST0 * m   // 6da45b: fmul dword ptr [0x858b88]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da461: fstp dword ptr [esp + 0x268]
L_6da468:;
    fA = (uint32)(*(uint8*)(uintptr_t)(uint32)(esi + 0x42b)); fB = 0x10u; fR = (fA & fB) & 0xFFu;   // 6da468: test byte ptr [esi + 0x42b], 0x10
    if (fR == 0) goto L_6da4ae;   // 6da46f: je 0x6da4ae
    edi = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da471: mov edi, dword ptr [esi + 0x388]
    fp.Fld((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // 6da477: fld dword ptr [edi + 0x20]
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(edi + 0x2c)));   // ST0 = ST0 / m   // 6da47a: fdiv dword ptr [edi + 0x2c]
    FlcCrtAsin(fp);   // 6da47d: call 0x821e70
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c84)));   // ST0 = ST0 * m   // 6da482: fmul dword ptr [0x858c84]
    fp.Fsin();   // 6da488: fsin 
    fp.Fdiv((*(float*)(uintptr_t)(uint32)(edi + 0x20)));   // ST0 = ST0 / m   // 6da48a: fdiv dword ptr [edi + 0x20]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(edi + 0x2c)));   // ST0 = ST0 * m   // 6da48d: fmul dword ptr [edi + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da490: fld dword ptr [esp + 0x26c]
    fp.Arith('*', 0, 0, 1, false);   // 6da497: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x26c)));   // 6da499: fstp dword ptr [esp + 0x26c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // ST0 = ST0 * m   // 6da4a0: fmul dword ptr [esp + 0x270]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x270)));   // 6da4a7: fstp dword ptr [esp + 0x270]
L_6da4ae:;
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da4ae: mov eax, dword ptr [esi + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da4b1: fld dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 + m   // 6da4b5: fadd dword ptr [eax + 0x10]
    esp -= 0xCu;   // 6da4b8: sub esp, 0xc
    edx = esp;   // 6da4bb: mov edx, esp
    esp -= 0xCu;   // 6da4bd: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6da4c0: fstp dword ptr [esp + 0x4c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // 6da4c4: fld dword ptr [esp + 0x6c]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // ST0 = ST0 + m   // 6da4c8: fadd dword ptr [eax + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da4cb: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da4cf: fld dword ptr [esp + 0x70]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 + m   // 6da4d3: fadd dword ptr [eax + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da4d6: fstp dword ptr [esp + 0x54]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4da: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6da4e1: fmul dword ptr [eax + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4e4: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6da4eb: fmul dword ptr [eax + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x284)));   // 6da4ee: fld dword ptr [esp + 0x284]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x28)));   // ST0 = ST0 * m   // 6da4f5: fmul dword ptr [eax + 0x28]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da4f8: mov eax, dword ptr [esi + 0x388]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 0x20));   // 6da4fe: mov ecx, dword ptr [eax + 0x20]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6da501: mov dword ptr [esp + 0x2c], ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da505: fstp dword ptr [esp + 0x7c]
    fp.Fxch(1);   // 6da509: fxch st(1)
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da50b: mov eax, dword ptr [esp + 0x4c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da50f: fmul dword ptr [esp + 0x2c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da513: mov ecx, dword ptr [esp + 0x50]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da517: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da519: mov eax, dword ptr [esp + 0x54]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da51d: fstp dword ptr [esp + 0x34]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da521: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da524: mov dword ptr [edx + 8], eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da527: fmul dword ptr [esp + 0x2c]
    ecx = esp;   // 6da52b: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da52d: fld dword ptr [esp + 0x7c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da531: fmul dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da535: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da53b: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6da53f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da541: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da545: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da547: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da549: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da54d: fmul st(1)
    fp.Fxch(1);   // 6da54f: fxch st(1)
    fp.FstpSt(0);   // 6da551: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da553: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da557: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da55d: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da561: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da565: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da569: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da56b: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da571: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da575: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da579: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6da57f: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da582: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da586: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da58a: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6da58d: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da58f: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da594: fld dword ptr [esp + 0x50]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da598: mov eax, dword ptr [esi + 0x14]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 + m   // 6da59b: fadd dword ptr [eax]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da59d: fstp dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da5a1: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 + m   // 6da5a5: fadd dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da5a8: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6da5ac: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 + m   // 6da5b0: fadd dword ptr [eax + 8]
    esp -= 0xCu;   // 6da5b3: sub esp, 0xc
    edx = esp;   // 6da5b6: mov edx, esp
    esp -= 0xCu;   // 6da5b8: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da5bb: fstp dword ptr [esp + 0x54]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5bf: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x20)));   // ST0 = ST0 * m   // 6da5c6: fmul dword ptr [eax + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5c9: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x24)));   // ST0 = ST0 * m   // 6da5d0: fmul dword ptr [eax + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x288)));   // 6da5d3: fld dword ptr [esp + 0x288]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x28)));   // ST0 = ST0 * m   // 6da5da: fmul dword ptr [eax + 0x28]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da5dd: mov eax, dword ptr [esi + 0x388]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 0x18));   // 6da5e3: mov ecx, dword ptr [eax + 0x18]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x2c) = (uint32)(ecx);   // 6da5e6: mov dword ptr [esp + 0x2c], ecx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da5ea: fstp dword ptr [esp + 0x7c]
    fp.Fxch(1);   // 6da5ee: fxch st(1)
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da5f0: mov eax, dword ptr [esp + 0x4c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da5f4: fmul dword ptr [esp + 0x2c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da5f8: mov ecx, dword ptr [esp + 0x50]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da5fc: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da5fe: mov eax, dword ptr [esp + 0x54]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da602: fstp dword ptr [esp + 0x34]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da606: mov dword ptr [edx + 4], ecx
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da609: mov dword ptr [edx + 8], eax
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da60c: fmul dword ptr [esp + 0x2c]
    ecx = esp;   // 6da610: mov ecx, esp
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da612: fld dword ptr [esp + 0x7c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // ST0 = ST0 * m   // 6da616: fmul dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da61a: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da620: fld dword ptr [esp + 0x34]
    fp.Arith('*', 0, 0, 1, false);   // 6da624: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da626: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da62a: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da62c: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da62e: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da632: fmul st(1)
    fp.Fxch(1);   // 6da634: fxch st(1)
    fp.FstpSt(0);   // 6da636: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da638: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da63c: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da642: fstp dword ptr [esp + 0x34]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da646: mov edx, dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da64a: fld dword ptr [esp + 0x44]
    *(uint32*)(uintptr_t)(uint32)(ecx) = (uint32)(edx);   // 6da64e: mov dword ptr [ecx], edx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da650: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da656: fstp dword ptr [esp + 0x38]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da65a: mov eax, dword ptr [esp + 0x38]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da65e: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 4) = (uint32)(eax);   // 6da664: mov dword ptr [ecx + 4], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da667: fstp dword ptr [esp + 0x3c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da66b: mov edx, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(ecx + 8) = (uint32)(edx);   // 6da66f: mov dword ptr [ecx + 8], edx
    ecx = esi;   // 6da672: mov ecx, esi
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da674: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6da679: fld dword ptr [esp + 0x48]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da67d: mov eax, dword ptr [esi + 0x14]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 * m   // 6da680: fmul dword ptr [eax + 8]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax));   // 6da683: mov ecx, dword ptr [eax]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da685: fld dword ptr [esp + 0x44]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 4));   // 6da689: mov edx, dword ptr [eax + 4]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 * m   // 6da68c: fmul dword ptr [eax + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6da68f: mov dword ptr [esp + 0x1c], ecx
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(edx);   // 6da693: mov dword ptr [esp + 0x20], edx
    fp.Arith('+', 1, 1, 0, true);   // 6da697: faddp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da699: fld dword ptr [esp + 0x40]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 * m   // 6da69d: fmul dword ptr [eax]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(eax + 8));   // 6da69f: mov eax, dword ptr [eax + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(eax);   // 6da6a2: mov dword ptr [esp + 0x24], eax
    fp.Arith('+', 1, 1, 0, true);   // 6da6a6: faddp st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(0x858c1c)));   // ST0 = ST0 * m   // 6da6a8: fmul dword ptr [0x858c1c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da6ae: fstp dword ptr [esp + 0x18]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // 6da6b2: fld dword ptr [esp + 0x18]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da6b6: mov ecx, dword ptr [esi + 0x388]
    fp.Fabs();   // 6da6bc: fabs 
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x8c));   // 6da6be: mov edx, dword ptr [esi + 0x8c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da6c4: fstp dword ptr [esp + 0x10]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da6c8: mov dword ptr [esp + 0x14], edx
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da6cc: fld dword ptr [esp + 0x10]
    esp -= 0xCu;   // 6da6d0: sub esp, 0xc
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x14)));   // ST0 = ST0 * m   // 6da6d3: fmul dword ptr [ecx + 0x14]
    eax = esp;   // 6da6d6: mov eax, esp
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // ST0 = ST0 * m   // 6da6d8: fmul dword ptr [esp + 0x24]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x28)));   // 6da6dc: fld dword ptr [esp + 0x28]
    fp.Arith('*', 0, 0, 1, false);   // 6da6e0: fmul st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6e2: fld dword ptr [esp + 0x2c]
    fp.Arith('*', 0, 0, 2, false);   // 6da6e6: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6e8: fstp dword ptr [esp + 0x2c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da6ec: fld dword ptr [esp + 0x30]
    fp.Arith('*', 0, 0, 2, false);   // 6da6f0: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da6f2: fstp dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da6f6: fmul dword ptr [esp + 0x20]
    fp.FstpSt(1);   // 6da6fa: fstp st(1)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x2c)));   // 6da6fc: fld dword ptr [esp + 0x2c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da700: fmul dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x30)));   // 6da704: fld dword ptr [esp + 0x30]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // ST0 = ST0 * m   // 6da708: fmul dword ptr [esp + 0x20]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da70c: fstp dword ptr [esp + 0x70]
    fp.Fxch(1);   // 6da710: fxch st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da712: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da718: fstp dword ptr [esp + 0x40]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x40));   // 6da71c: mov ecx, dword ptr [esp + 0x40]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da720: mov dword ptr [eax], ecx
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da722: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da728: fstp dword ptr [esp + 0x44]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // 6da72c: fld dword ptr [esp + 0x70]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x44));   // 6da730: mov edx, dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da734: fmul dword ptr [0xb7cb5c]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da73a: mov dword ptr [eax + 4], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x48)));   // 6da73d: fstp dword ptr [esp + 0x48]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6da741: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da745: mov dword ptr [eax + 8], ecx
    ecx = esi;   // 6da748: mov ecx, esi
    v.ApplyMoveForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }); esp += 0xC;   // 6da74a: call 0x5429f0
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x10)));   // 6da74f: fld dword ptr [esp + 0x10]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da753: mov eax, dword ptr [esi + 0x14]
    edx = eax;   // 6da756: mov edx, eax
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx));   // 6da758: mov ecx, dword ptr [edx]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x1c) = (uint32)(ecx);   // 6da75a: mov dword ptr [esp + 0x1c], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 4));   // 6da75e: mov ecx, dword ptr [edx + 4]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x20) = (uint32)(ecx);   // 6da761: mov dword ptr [esp + 0x20], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da765: mov ecx, dword ptr [esi + 0x388]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0x10)));   // ST0 = ST0 * m   // 6da76b: fmul dword ptr [ecx + 0x10]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(edx + 8));   // 6da76e: mov edx, dword ptr [edx + 8]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x24) = (uint32)(edx);   // 6da771: mov dword ptr [esp + 0x24], edx
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x18)));   // ST0 = ST0 * m   // 6da775: fmul dword ptr [esp + 0x18]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da779: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(ecx + 0xc)));   // ST0 = ST0 * m   // 6da780: fmul dword ptr [ecx + 0xc]
    fp.Arith('+', 1, 1, 0, true);   // 6da783: faddp st(1)
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da785: fmul dword ptr [0xb7cb5c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // ST0 = ST0 * m   // 6da78b: fmul dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da791: fld dword ptr [esp + 0x1c]
    fp.Arith('*', 0, 0, 1, false);   // 6da795: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da797: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da79b: fld dword ptr [esp + 0x20]
    fp.Arith('*', 0, 0, 1, false);   // 6da79f: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da7a1: fstp dword ptr [esp + 0x20]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da7a5: fld dword ptr [esp + 0x24]
    fp.Arith('*', 0, 0, 1, false);   // 6da7a9: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x24)));   // 6da7ab: fstp dword ptr [esp + 0x24]
    fp.FstpSt(0);   // 6da7af: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // 6da7b1: fld dword ptr [eax + 0x10]
    fp.Fchs();   // 6da7b4: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // 6da7b6: fld dword ptr [eax + 0x14]
    fp.Fchs();   // 6da7b9: fchs 
    fp.Fld((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // 6da7bb: fld dword ptr [eax + 0x18]
    fp.Fchs();   // 6da7be: fchs 
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da7c0: fstp dword ptr [esp + 0x64]
    fp.Fxch(1);   // 6da7c4: fxch st(1)
    esp -= 0xCu;   // 6da7c6: sub esp, 0xc
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x5c)));   // ST0 = ST0 + m   // 6da7c9: fadd dword ptr [esp + 0x5c]
    eax = esp;   // 6da7cd: mov eax, esp
    esp -= 0xCu;   // 6da7cf: sub esp, 0xc
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x4c)));   // 6da7d2: fstp dword ptr [esp + 0x4c]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x4c));   // 6da7d6: mov ecx, dword ptr [esp + 0x4c]
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da7da: mov dword ptr [eax], ecx
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x6c)));   // ST0 = ST0 + m   // 6da7dc: fadd dword ptr [esp + 0x6c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da7e0: fstp dword ptr [esp + 0x50]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x7c)));   // 6da7e4: fld dword ptr [esp + 0x7c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x50));   // 6da7e8: mov edx, dword ptr [esp + 0x50]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(esp + 0x70)));   // ST0 = ST0 + m   // 6da7ec: fadd dword ptr [esp + 0x70]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da7f0: mov dword ptr [eax + 4], edx
    edx = esp;   // 6da7f3: mov edx, esp
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da7f5: fstp dword ptr [esp + 0x54]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x54));   // 6da7f9: mov ecx, dword ptr [esp + 0x54]
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da7fd: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da800: mov eax, dword ptr [esp + 0x34]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da804: mov ecx, dword ptr [esp + 0x38]
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da808: mov dword ptr [edx], eax
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da80a: mov eax, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da80e: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da811: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da813: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da816: call 0x542a50
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x50)));   // 6da81b: fld dword ptr [esp + 0x50]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x14));   // 6da81f: mov eax, dword ptr [esi + 0x14]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax)));   // ST0 = ST0 + m   // 6da822: fadd dword ptr [eax]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esi + 0x388));   // 6da824: mov ecx, dword ptr [esi + 0x388]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(ecx + 0xc));   // 6da82a: mov edx, dword ptr [ecx + 0xc]
    *(uint32*)(uintptr_t)(uint32)(esp + 0x14) = (uint32)(edx);   // 6da82d: mov dword ptr [esp + 0x14], edx
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x34)));   // 6da831: fstp dword ptr [esp + 0x34]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x54)));   // 6da835: fld dword ptr [esp + 0x54]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 4)));   // ST0 = ST0 + m   // 6da839: fadd dword ptr [eax + 4]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x38)));   // 6da83c: fstp dword ptr [esp + 0x38]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x58)));   // 6da840: fld dword ptr [esp + 0x58]
    fp.Fadd((*(float*)(uintptr_t)(uint32)(eax + 8)));   // ST0 = ST0 + m   // 6da844: fadd dword ptr [eax + 8]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da847: fstp dword ptr [esp + 0x3c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da84b: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x10)));   // ST0 = ST0 * m   // 6da852: fmul dword ptr [eax + 0x10]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da855: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x14)));   // ST0 = ST0 * m   // 6da85c: fmul dword ptr [eax + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x268)));   // 6da85f: fld dword ptr [esp + 0x268]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(eax + 0x18)));   // ST0 = ST0 * m   // 6da866: fmul dword ptr [eax + 0x18]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da869: fstp dword ptr [esp + 0x64]
    fp.Fxch(1);   // 6da86d: fxch st(1)
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da86f: fmul dword ptr [esp + 0x14]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da873: fstp dword ptr [esp + 0x1c]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da877: fmul dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x64)));   // 6da87b: fld dword ptr [esp + 0x64]
    fp.Fmul((*(float*)(uintptr_t)(uint32)(esp + 0x14)));   // ST0 = ST0 * m   // 6da87f: fmul dword ptr [esp + 0x14]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esi + 0x90)));   // 6da883: fld dword ptr [esi + 0x90]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da889: fld dword ptr [esp + 0x1c]
    fp.Arith('*', 0, 0, 1, false);   // 6da88d: fmul st(1)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da88f: fstp dword ptr [esp + 0x40]
    fp.Fxch(2);   // 6da893: fxch st(2)
    fp.Arith('*', 0, 0, 2, false);   // 6da895: fmul st(2)
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da897: fstp dword ptr [esp + 0x44]
    fp.Arith('*', 0, 0, 1, false);   // 6da89b: fmul st(1)
    fp.Fxch(1);   // 6da89d: fxch st(1)
    fp.FstpSt(0);   // 6da89f: fstp st(0)
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x40)));   // 6da8a1: fld dword ptr [esp + 0x40]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8a5: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x1c)));   // 6da8ab: fstp dword ptr [esp + 0x1c]
    fp.Fld((*(float*)(uintptr_t)(uint32)(esp + 0x44)));   // 6da8af: fld dword ptr [esp + 0x44]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8b3: fmul dword ptr [0xb7cb5c]
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x20)));   // 6da8b9: fstp dword ptr [esp + 0x20]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x34));   // 6da8bd: mov ecx, dword ptr [esp + 0x34]
    fp.Fmul(CTimer::ms_fTimeStep);   // ST0 = ST0 * m   // 6da8c1: fmul dword ptr [0xb7cb5c]
    edx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x38));   // 6da8c7: mov edx, dword ptr [esp + 0x38]
    esp -= 0xCu;   // 6da8cb: sub esp, 0xc
    eax = esp;   // 6da8ce: mov eax, esp
    *(uint32*)(uintptr_t)(uint32)(eax) = (uint32)(ecx);   // 6da8d0: mov dword ptr [eax], ecx
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x48));   // 6da8d2: mov ecx, dword ptr [esp + 0x48]
    *(uint32*)(uintptr_t)(uint32)(eax + 4) = (uint32)(edx);   // 6da8d6: mov dword ptr [eax + 4], edx
    *(uint32*)(uintptr_t)(uint32)(eax + 8) = (uint32)(ecx);   // 6da8d9: mov dword ptr [eax + 8], ecx
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x28));   // 6da8dc: mov eax, dword ptr [esp + 0x28]
    ecx = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x2c));   // 6da8e0: mov ecx, dword ptr [esp + 0x2c]
    esp -= 0xCu;   // 6da8e4: sub esp, 0xc
    edx = esp;   // 6da8e7: mov edx, esp
    *(uint32*)(uintptr_t)(uint32)(edx) = (uint32)(eax);   // 6da8e9: mov dword ptr [edx], eax
    fp.Fstp((*(float*)(uintptr_t)(uint32)(esp + 0x3c)));   // 6da8eb: fstp dword ptr [esp + 0x3c]
    eax = (uint32)(*(uint32*)(uintptr_t)(uint32)(esp + 0x3c));   // 6da8ef: mov eax, dword ptr [esp + 0x3c]
    *(uint32*)(uintptr_t)(uint32)(edx + 4) = (uint32)(ecx);   // 6da8f3: mov dword ptr [edx + 4], ecx
    ecx = esi;   // 6da8f6: mov ecx, esi
    *(uint32*)(uintptr_t)(uint32)(edx + 8) = (uint32)(eax);   // 6da8f8: mov dword ptr [edx + 8], eax
    v.ApplyTurnForce(CVector{ (*(float*)(uintptr_t)(esp + 0)), (*(float*)(uintptr_t)(esp + 4)), (*(float*)(uintptr_t)(esp + 8)) }, CVector{ (*(float*)(uintptr_t)(esp + 12)), (*(float*)(uintptr_t)(esp + 16)), (*(float*)(uintptr_t)(esp + 20)) }); esp += 0x18;   // 6da8fb: call 0x542a50
}
}  // namespace

// 0x6D85F0
void CVehicle::FlyingControl(eFlightModel flightModel, float leftRightSkid, float steeringUpDown, float steeringLeftRight, float accelerationBreakStatus) {
    FlcFrame f{ this, FlcX87{}, m_pFlyingHandlingData, flightModel, leftRightSkid, steeringUpDown, steeringLeftRight, accelerationBreakStatus, nullptr, {}, {} };
    FlcX87& fp = f.fp;
    if (!f.fh) { fp.Commit(); return; }                            // 0x6D8604: sin datos de vuelo
    fp.Fld(CTimer::ms_fTimeStep); fp.FcomMem(kFlcZero, true);
    if (!FlcParityEven(fp.Ah() & 0x41)) { fp.Commit(); return; }   // 0x6D861B: jnp, sale con tiempo <= 0
    // 0x6D8621: el mando solo se lee con el vehiculo del jugador y un conductor jugador
    if (GetStatus() == STATUS_PLAYER && m_pDriver && m_pDriver->IsPlayer())
        f.pad = m_pDriver->AsPlayer()->GetPadFromPlayer();
    // 0x6D8652: velocidad + deriva del viento (un vehiculo de mision no la tiene)
    float wind[3] = { 0.f, 0.f, 0.f };
    f.vel[1] = 0.f; f.vel[2] = 0.f;
    fp.Fld(kFlcZero);
    if (m_nCreatedBy != 2) {
        fp.Pop();
        fp.Fld(f.fh->m_fWindMult);
        fp.Fld(CWeather::WindDir.x); fp.Arith('*', 0, 0, 1, false); fp.Fstp(wind[0]);
        fp.Fld(CWeather::WindDir.y); fp.Arith('*', 0, 0, 1, false); fp.Fstp(wind[1]);
        fp.Fmul(CWeather::WindDir.z); fp.Fstp(wind[2]);
        fp.Fld(wind[0]); fp.Fchs();
        fp.Fld(wind[1]); fp.Fchs(); fp.Fstp(f.vel[1]);
        fp.Fld(wind[2]); fp.Fchs(); fp.Fstp(f.vel[2]);
    }
    fp.Fadd(m_vecMoveSpeed.x); fp.Fstp(f.vel[0]);
    fp.Fld(f.vel[1]); fp.Fadd(m_vecMoveSpeed.y); fp.Fstp(f.vel[1]);
    fp.Fld(f.vel[2]); fp.Fadd(m_vecMoveSpeed.z); fp.Fstp(f.vel[2]);
    FlcMatMul3x3(fp, f.com, reinterpret_cast<const float*>(m_matrix), &m_vecCentreOfMass.x);   // 0x59C790
    switch (flightModel) {
    case FLIGHT_MODEL_UNK: FlcRouteAirship(f); break;
    case FLIGHT_MODEL_BARON: case FLIGHT_MODEL_PLANE: case FLIGHT_MODEL_UNK2: case FLIGHT_MODEL_BOAT: FlcRouteWing(f); break;
    case FLIGHT_MODEL_RC: case FLIGHT_MODEL_HELI: case eFlightModel(8): FlcRouteRotor(f); break;
    default: break;
    }
    FlcTail(f);
    fp.Commit();
}
