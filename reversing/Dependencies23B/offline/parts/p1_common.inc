
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
