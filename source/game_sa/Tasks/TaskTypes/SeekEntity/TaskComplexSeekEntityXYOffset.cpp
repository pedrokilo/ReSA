#include "StdInc.h"
#include "TaskComplexSeekEntityXYOffset.h"
#include "X87Math.h"

void CTaskComplexSeekEntityXYOffset::InjectHooks() {

}

// 0x4BC470: ECX = this, EDX de entrada y la distancia en pila (ret 4). Si la distancia no es igual a la actual (un NaN también la cambia), la guarda
// y reinicia el temporizador y deja EAX = bits de la distancia y EDX = hora; si es igual, EAX conserva su mitad alta y toma la palabra de estado del x87 en AX.
#pragma runtime_checks("", off)
__declspec(safebuffers) uint32 __fastcall SetMaxEntityDist2DRegs(void* self, uint32 entryEdx, uint32 distanceBits) {
    const uint32 entryEax = EntryEaxCpp();
    auto* const  base     = reinterpret_cast<uint8*>(self);
    auto&        current  = *reinterpret_cast<float*>(base + 0x18);
    const uint32 status   = X87CompareStatusCpp(reinterpret_cast<const float*>(&distanceBits), &current);
    // test ah,44h / jnp: solo salta (no guarda) si exactamente una de C3 y C2 está activa, o sea, si son iguales
    if (((status >> 14) ^ (status >> 10)) & 1u) {
        return ResidueRegsCpp(reinterpret_cast<uint32>(self), entryEdx, (entryEax & 0xFFFF0000u) | status);
    }
    const uint32 now = CTimer::m_snTimeInMilliseconds;
    memcpy(&current, &distanceBits, sizeof distanceBits);
    *reinterpret_cast<uint32*>(base + 0x28) = now;
    *reinterpret_cast<uint32*>(base + 0x2C) = 0;
    base[0x30]                              = 1;
    return ResidueRegsCpp(reinterpret_cast<uint32>(self), now, distanceBits);
}
#pragma runtime_checks("", restore)

template<> void CTaskComplexSeekEntity<CEntitySeekPosCalculatorXYOffset>::SetMaxEntityDist2D(float distance) {
    uint32 bits;
    memcpy(&bits, &distance, sizeof bits);
    SetMaxEntityDist2DRegs(this, 0, bits);
}
