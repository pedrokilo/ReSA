#pragma once

#include "FxTools.h"

class NOTSA_EXPORT_VTABLE FxInterpInfo_c {
public:
    bool    m_bLooped;  // (4 bytes in Manhunt)
    int8    m_nNumKeys;
    int16   m_nCount;
    uint16* m_pTimes;

public:
    FxInterpInfo_c();
    ~FxInterpInfo_c() = default; // 0x4A8430

    virtual void Load(FILESTREAM file) = 0;

protected:
    // Cuerpo comun de los cuatro GetVal(float* outValues, float delta) del EXE (Float 0x4A8470, U255 0x4A8800, 32 0x4A89C0, 255 0x4A8B80).
    // Cada subclase lo instancia con el tipo de su tabla de claves y su escala (claves enteras: clave * escala; Float: sin escala).
    // Aritmetica x87 del EXE: los tiempos (uint16 * 1/256), el plegado del tiempo, t y las claves se evaluan en registro con exponente
    // extendido (aqui `double`: con la palabra de control a 24 bits cada operacion se redondea a 24 bits, igual que el EXE); solo se
    // guardan como float el tiempo plegado, t y cada salida. Las copias de claves float (1 clave / ultima clave) son bit a bit (mov).
    // El EXE relee m_nCount como un solo byte con signo (`movsx byte ptr [esi+6]`).
    template <typename TKey>
    void InterpolateKeys(TKey* const* keys, float* outValues, float delta, float scale) const;

    // _ftol2 (0x821B40) usado como `(int)q`: trunca hacia cero a 64 bits y devuelve los 32 bits bajos; NaN, infinitos y |q| >= 2^63 dan 0.
    static int32 TruncateLow32(double q) {
        const uint64 bits = std::bit_cast<uint64>(q);
        const int32  exponent = int32((bits >> 52) & 0x7FF) - 1023;
        if (exponent < 0 || exponent >= 63) {
            return 0;
        }
        const uint64 mantissa = (bits & 0xFFFFFFFFFFFFFull) | (1ull << 52);
        uint64 magnitude = exponent >= 52 ? mantissa << (exponent - 52) : mantissa >> (52 - exponent);
        if (bits >> 63) {
            magnitude = 0 - magnitude;
        }
        return int32(uint32(magnitude));
    }
};

template <typename TKey>
void FxInterpInfo_c::InterpolateKeys(TKey* const* keys, float* outValues, float delta, float scale) const {
    constexpr double TIME_SCALE = 0.00390625; // 0x859AA0 = 1/256
    const int32 numKeys = m_nNumKeys;
    const int32 count   = int8(m_nCount);

    const auto Key = [&](TKey key) -> double {
        if constexpr (std::is_same_v<TKey, float>) {
            return key;
        } else {
            return double(key) * double(scale);
        }
    };
    const auto CopyKey = [&](int32 index) {
        for (int32 i = 0; i < count; i++) {
            if constexpr (std::is_same_v<TKey, float>) {
                std::memcpy(&outValues[i], &keys[i][index], sizeof(float));
            } else {
                outValues[i] = float(Key(keys[i][index]));
            }
        }
    };

    if (numKeys == 1) {
        CopyKey(0);
        return;
    }

    volatile float time = delta; // el EXE lo guarda como float en la pila
    if (m_bLooped) {
        const double total = double(m_pTimes[numKeys - 1]) * TIME_SCALE;
        const double ratio = double(delta) / total;
        time               = float(double(delta) - double(TruncateLow32(ratio)) * total);
    }

    int32  next     = 1;
    double nextTime = 0.0;
    for (; next < numKeys; next++) {
        nextTime = double(m_pTimes[next]) * TIME_SCALE;
        if (double(float(time)) < nextTime) { // fcomp ordenado: con NaN no corta
            break;
        }
    }
    if (next >= numKeys) {
        CopyKey(numKeys - 1);
        return;
    }

    const double prevTime = double(m_pTimes[next - 1]) * TIME_SCALE;
    volatile float t = float((double(float(time)) - prevTime) / (nextTime - prevTime));
    for (int32 i = 0; i < count; i++) {
        if constexpr (std::is_same_v<TKey, float>) {
            // Expresion en float, sin intermedios en double: el EXE hace fld clave[i]; fsub clave[i-1] (operando de memoria); fmul t; fadd clave[i-1] (memoria), y con un NaN
            // senalizador en memoria el x87 prefiere el NaN silencioso del otro operando (si fueran dos registros ganaria el de mayor significando).
            outValues[i] = (keys[i][next] - keys[i][next - 1]) * t + keys[i][next - 1];
        } else {
            const double prev = Key(keys[i][next - 1]);
            const double cur  = Key(keys[i][next]);
            outValues[i]      = float((cur - prev) * double(float(t)) + prev);
        }
    }
}
